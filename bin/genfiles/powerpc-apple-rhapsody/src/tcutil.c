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
void*v;};struct _tuple0{void*f1;void*f2;};void*Cyc_Core_fst(struct _tuple0*);int
Cyc_Core_intcmp(int,int);extern char Cyc_Core_Invalid_argument[21];struct Cyc_Core_Invalid_argument_struct{
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
int Cyc_printf(struct _dyneither_ptr,struct _dyneither_ptr);struct _dyneither_ptr Cyc_vrprintf(
struct _RegionHandle*,struct _dyneither_ptr,struct _dyneither_ptr);extern char Cyc_FileCloseError[
19];extern char Cyc_FileOpenError[18];struct Cyc_FileOpenError_struct{char*tag;
struct _dyneither_ptr f1;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
struct Cyc_List_List*Cyc_List_list(struct _dyneither_ptr);int Cyc_List_length(
struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_copy(struct Cyc_List_List*x);
struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);struct
Cyc_List_List*Cyc_List_rmap(struct _RegionHandle*,void*(*f)(void*),struct Cyc_List_List*
x);struct Cyc_List_List*Cyc_List_rmap_c(struct _RegionHandle*,void*(*f)(void*,void*),
void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[18];struct Cyc_List_List*
Cyc_List_map2(void*(*f)(void*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y);
void Cyc_List_iter(void(*f)(void*),struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_revappend(
struct Cyc_List_List*x,struct Cyc_List_List*y);struct Cyc_List_List*Cyc_List_imp_rev(
struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rappend(struct _RegionHandle*,
struct Cyc_List_List*x,struct Cyc_List_List*y);struct Cyc_List_List*Cyc_List_imp_append(
struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[8];void*Cyc_List_nth(
struct Cyc_List_List*x,int n);int Cyc_List_exists_c(int(*pred)(void*,void*),void*
env,struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_zip(struct Cyc_List_List*
x,struct Cyc_List_List*y);struct Cyc_List_List*Cyc_List_rzip(struct _RegionHandle*
r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y);struct
_tuple1{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct _tuple1 Cyc_List_rsplit(
struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x);int Cyc_List_mem(
int(*compare)(void*,void*),struct Cyc_List_List*l,void*x);void*Cyc_List_assoc_cmp(
int(*cmp)(void*,void*),struct Cyc_List_List*l,void*x);int Cyc_List_list_cmp(int(*
cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2);struct Cyc_Lineno_Pos{
struct _dyneither_ptr logical_file;struct _dyneither_ptr line;int line_no;int col;};
extern char Cyc_Position_Exit[9];struct Cyc_Position_Segment;struct Cyc_List_List*
Cyc_Position_strings_of_segments(struct Cyc_List_List*);struct Cyc_Position_Error{
struct _dyneither_ptr source;struct Cyc_Position_Segment*seg;void*kind;struct
_dyneither_ptr desc;};struct Cyc_Position_Error*Cyc_Position_mk_err_elab(struct Cyc_Position_Segment*,
struct _dyneither_ptr);extern char Cyc_Position_Nocontext[14];extern int Cyc_Position_num_errors;
extern int Cyc_Position_max_errors;void Cyc_Position_post_error(struct Cyc_Position_Error*);
struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct
_union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{
int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct
_union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_Loc_n Loc_n;};union Cyc_Absyn_Nmspace
Cyc_Absyn_Loc_n;union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(struct Cyc_List_List*);
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*);struct _tuple2{union
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
struct _tuple2*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{
int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct
_union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};
union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype
UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};union Cyc_Absyn_DatatypeInfoU
Cyc_Absyn_KnownDatatype(struct Cyc_Absyn_Datatypedecl**);struct Cyc_Absyn_DatatypeInfo{
union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;struct Cyc_Core_Opt*
rgn;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple2*datatype_name;
struct _tuple2*field_name;int is_extensible;};struct
_union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo
val;};struct _tuple3{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*
f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple3 val;
};union Cyc_Absyn_DatatypeFieldInfoU{struct
_union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct
_union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};union Cyc_Absyn_DatatypeFieldInfoU
Cyc_Absyn_KnownDatatypefield(struct Cyc_Absyn_Datatypedecl*,struct Cyc_Absyn_Datatypefield*);
struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;
struct Cyc_List_List*targs;};struct _tuple4{void*f1;struct _tuple2*f2;struct Cyc_Core_Opt*
f3;};struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple4 val;};struct
_union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{
struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr
KnownAggr;};union Cyc_Absyn_AggrInfoU Cyc_Absyn_UnknownAggr(void*,struct _tuple2*,
struct Cyc_Core_Opt*);union Cyc_Absyn_AggrInfoU Cyc_Absyn_KnownAggr(struct Cyc_Absyn_Aggrdecl**);
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
;struct Cyc_Absyn_EnumType_struct{int tag;struct _tuple2*f1;struct Cyc_Absyn_Enumdecl*
f2;};struct Cyc_Absyn_AnonEnumType_struct{int tag;struct Cyc_List_List*f1;};struct
Cyc_Absyn_RgnHandleType_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_struct{
int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_struct{int tag;struct _tuple2*
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
_tuple5{void*f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple5 val;};
struct _tuple6{void*f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple6
val;};struct _tuple7{void*f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple7
val;};struct _tuple8{void*f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;
struct _tuple8 val;};struct _union_Cnst_Float_c{int tag;struct _dyneither_ptr val;};
struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{
struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct
_union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct
_union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct
_union_Cnst_String_c String_c;};extern union Cyc_Absyn_Cnst Cyc_Absyn_Null_c;struct
Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*
vai;};struct Cyc_Absyn_StructField_struct{int tag;struct _dyneither_ptr*f1;};struct
Cyc_Absyn_TupleIndex_struct{int tag;unsigned int f1;};struct Cyc_Absyn_MallocInfo{
int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;
int fat_result;};struct Cyc_Absyn_Const_e_struct{int tag;union Cyc_Absyn_Cnst f1;};
struct Cyc_Absyn_Var_e_struct{int tag;struct _tuple2*f1;void*f2;};struct Cyc_Absyn_UnknownId_e_struct{
int tag;struct _tuple2*f1;};struct Cyc_Absyn_Primop_e_struct{int tag;void*f1;struct
Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_struct{int tag;struct Cyc_Absyn_Exp*
f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;void*f2;};struct Cyc_Absyn_Conditional_e_struct{int
tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};
struct Cyc_Absyn_And_e_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*
f2;};struct Cyc_Absyn_Or_e_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*
f2;};struct Cyc_Absyn_SeqExp_e_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*
f2;};struct Cyc_Absyn_UnknownCall_e_struct{int tag;struct Cyc_Absyn_Exp*f1;struct
Cyc_List_List*f2;};struct Cyc_Absyn_FnCall_e_struct{int tag;struct Cyc_Absyn_Exp*f1;
struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;};struct Cyc_Absyn_Throw_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoInstantiate_e_struct{int tag;
struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_struct{int tag;struct Cyc_Absyn_Exp*
f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_struct{int tag;void*f1;struct
Cyc_Absyn_Exp*f2;int f3;void*f4;};struct Cyc_Absyn_Address_e_struct{int tag;struct
Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_struct{int tag;struct Cyc_Absyn_Exp*f1;
struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_struct{int tag;void*f1;};
struct Cyc_Absyn_Sizeofexp_e_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_struct{
int tag;void*f1;void*f2;};struct Cyc_Absyn_Gentyp_e_struct{int tag;struct Cyc_List_List*
f1;void*f2;};struct Cyc_Absyn_Deref_e_struct{int tag;struct Cyc_Absyn_Exp*f1;};
struct Cyc_Absyn_AggrMember_e_struct{int tag;struct Cyc_Absyn_Exp*f1;struct
_dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_struct{int tag;struct
Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_struct{
int tag;struct Cyc_List_List*f1;};struct _tuple9{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Tqual
f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_struct{int tag;struct _tuple9*f1;struct
Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_struct{int tag;struct Cyc_List_List*f1;
};struct Cyc_Absyn_Comprehension_e_struct{int tag;struct Cyc_Absyn_Vardecl*f1;
struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_Aggregate_e_struct{
int tag;struct _tuple2*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*
f4;};struct Cyc_Absyn_AnonStruct_e_struct{int tag;void*f1;struct Cyc_List_List*f2;}
;struct Cyc_Absyn_Datatype_e_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*
f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_struct{int tag;
struct _tuple2*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};
struct Cyc_Absyn_AnonEnum_e_struct{int tag;struct _tuple2*f1;void*f2;struct Cyc_Absyn_Enumfield*
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
_tuple10{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_struct{
int tag;struct _tuple10 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_struct{
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_struct{int tag;struct
Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_struct{int tag;struct _dyneither_ptr*f1;
struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_struct{int tag;struct Cyc_Absyn_Exp*
f1;struct _tuple10 f2;struct _tuple10 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Fallthru_s_struct{
int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_struct{
int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_struct{
int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_struct{
int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple10 f2;};struct Cyc_Absyn_TryCatch_s_struct{
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
struct _tuple2*f1;};struct Cyc_Absyn_UnknownCall_p_struct{int tag;struct _tuple2*f1;
struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_struct{int tag;struct Cyc_Absyn_Exp*
f1;};struct Cyc_Absyn_Pat{void*r;struct Cyc_Core_Opt*topt;struct Cyc_Position_Segment*
loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*
pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;struct Cyc_Position_Segment*
loc;};struct Cyc_Absyn_Global_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct
Cyc_Absyn_Funname_b_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_struct{int tag;struct
Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_Absyn_Vardecl{void*sc;struct _tuple2*name;struct Cyc_Absyn_Tqual tq;
void*type;struct Cyc_Absyn_Exp*initializer;struct Cyc_Core_Opt*rgn;struct Cyc_List_List*
attributes;int escapes;};struct Cyc_Absyn_Fndecl{void*sc;int is_inline;struct
_tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*effect;void*ret_type;
struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;
struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;struct Cyc_Core_Opt*
cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;
struct Cyc_List_List*attributes;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*
name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*
attributes;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct
Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{
void*kind;void*sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*
impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Datatypefield{struct
_tuple2*name;struct Cyc_List_List*typs;struct Cyc_Position_Segment*loc;void*sc;};
struct Cyc_Absyn_Datatypedecl{void*sc;struct _tuple2*name;struct Cyc_List_List*tvs;
struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct
_tuple2*name;struct Cyc_Absyn_Exp*tag;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Enumdecl{
void*sc;struct _tuple2*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{
struct _tuple2*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*
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
f2;};struct Cyc_Absyn_Using_d_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*
f2;};struct Cyc_Absyn_ExternC_d_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_struct{
int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Decl{void*
r;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_ArrayElement_struct{int tag;
struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_struct{int tag;struct
_dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];int Cyc_Absyn_qvar_cmp(
struct _tuple2*,struct _tuple2*);int Cyc_Absyn_tvar_cmp(struct Cyc_Absyn_Tvar*,
struct Cyc_Absyn_Tvar*);struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(struct Cyc_Position_Segment*);
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(struct Cyc_Position_Segment*);union Cyc_Absyn_Constraint*
Cyc_Absyn_new_conref(void*x);union Cyc_Absyn_Constraint*Cyc_Absyn_empty_conref();
union Cyc_Absyn_Constraint*Cyc_Absyn_compress_conref(union Cyc_Absyn_Constraint*x);
void*Cyc_Absyn_conref_val(union Cyc_Absyn_Constraint*x);void*Cyc_Absyn_conref_def(
void*y,union Cyc_Absyn_Constraint*x);void*Cyc_Absyn_conref_constr(void*y,union Cyc_Absyn_Constraint*
x);extern union Cyc_Absyn_Constraint*Cyc_Absyn_true_conref;extern union Cyc_Absyn_Constraint*
Cyc_Absyn_false_conref;extern union Cyc_Absyn_Constraint*Cyc_Absyn_bounds_one_conref;
void*Cyc_Absyn_compress_kb(void*);void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,
struct Cyc_Core_Opt*tenv);extern void*Cyc_Absyn_uint_typ;extern void*Cyc_Absyn_ulong_typ;
extern void*Cyc_Absyn_ulonglong_typ;extern void*Cyc_Absyn_sint_typ;extern void*Cyc_Absyn_slong_typ;
extern void*Cyc_Absyn_slonglong_typ;extern void*Cyc_Absyn_empty_effect;extern
struct _tuple2*Cyc_Absyn_datatype_print_arg_qvar;extern struct _tuple2*Cyc_Absyn_datatype_scanf_arg_qvar;
extern void*Cyc_Absyn_bounds_one;void*Cyc_Absyn_atb_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual
tq,void*b,union Cyc_Absyn_Constraint*zero_term);struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(
struct Cyc_Absyn_Exp*);struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,struct
Cyc_Position_Segment*);struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct
Cyc_List_List*,struct _dyneither_ptr*);struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(
struct Cyc_Absyn_Aggrdecl*,struct _dyneither_ptr*);struct _tuple11{struct Cyc_Absyn_Tqual
f1;void*f2;};struct _tuple11*Cyc_Absyn_lookup_tuple_field(struct Cyc_List_List*,
int);struct _dyneither_ptr Cyc_Absyn_attribute2string(void*);int Cyc_Absyn_fntype_att(
void*a);struct _tuple12{void*f1;struct _tuple2*f2;};struct _tuple12 Cyc_Absyn_aggr_kinded_name(
union Cyc_Absyn_AggrInfoU);struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(
union Cyc_Absyn_AggrInfoU info);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;
struct Cyc_Absynpp_Params{int expand_typedefs: 1;int qvar_to_Cids: 1;int
add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int rewrite_temp_tvars: 1;int
print_all_tvars: 1;int print_all_kinds: 1;int print_all_effects: 1;int
print_using_stmts: 1;int print_externC_stmts: 1;int print_full_evars: 1;int
print_zeroterm: 1;int generate_line_directives: 1;int use_curr_namespace: 1;struct Cyc_List_List*
curr_namespace;};struct _dyneither_ptr Cyc_Absynpp_typ2string(void*);struct
_dyneither_ptr Cyc_Absynpp_kind2string(void*);struct _dyneither_ptr Cyc_Absynpp_kindbound2string(
void*);struct _dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);struct
_dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple2*);struct Cyc_Iter_Iter{void*
env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);
struct Cyc_Set_Set;extern char Cyc_Set_Absent[11];struct Cyc_Dict_T;struct Cyc_Dict_Dict{
int(*rel)(void*,void*);struct _RegionHandle*r;struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[
12];extern char Cyc_Dict_Absent[11];struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict
d,void*k,void*v);void*Cyc_Dict_lookup(struct Cyc_Dict_Dict d,void*k);struct _tuple0*
Cyc_Dict_rchoose(struct _RegionHandle*r,struct Cyc_Dict_Dict d);struct _tuple0*Cyc_Dict_rchoose(
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
Cyc_Position_Segment*,struct _tuple2*);struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl(
struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,struct _tuple2*);struct Cyc_Absyn_Datatypedecl**
Cyc_Tcenv_lookup_datatypedecl(struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,
struct _tuple2*);struct Cyc_Absyn_Enumdecl**Cyc_Tcenv_lookup_enumdecl(struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*,struct _tuple2*);struct Cyc_Absyn_Typedefdecl*Cyc_Tcenv_lookup_typedefdecl(
struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,struct _tuple2*);struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_allow_valueof(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*);struct Cyc_List_List*
Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*);struct Cyc_Core_Opt*Cyc_Tcenv_lookup_opt_type_vars(
struct Cyc_Tcenv_Tenv*te);int Cyc_Tcenv_region_outlives(struct Cyc_Tcenv_Tenv*,void*
r1,void*r2);unsigned long Cyc_strlen(struct _dyneither_ptr s);int Cyc_strcmp(struct
_dyneither_ptr s1,struct _dyneither_ptr s2);int Cyc_strptrcmp(struct _dyneither_ptr*
s1,struct _dyneither_ptr*s2);struct _dyneither_ptr Cyc_strconcat(struct
_dyneither_ptr,struct _dyneither_ptr);struct _tuple13{unsigned int f1;int f2;};
struct _tuple13 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);int Cyc_Evexp_same_const_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);int Cyc_Evexp_lte_const_exp(struct
Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);int Cyc_Evexp_const_exp_cmp(struct Cyc_Absyn_Exp*
e1,struct Cyc_Absyn_Exp*e2);void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap);void Cyc_Tcutil_terr(struct Cyc_Position_Segment*,struct
_dyneither_ptr fmt,struct _dyneither_ptr ap);void Cyc_Tcutil_warn(struct Cyc_Position_Segment*,
struct _dyneither_ptr fmt,struct _dyneither_ptr ap);void Cyc_Tcutil_flush_warnings();
extern struct Cyc_Core_Opt*Cyc_Tcutil_empty_var_set;void*Cyc_Tcutil_copy_type(void*
t);int Cyc_Tcutil_kind_leq(void*k1,void*k2);void*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*
t);void*Cyc_Tcutil_typ_kind(void*t);void*Cyc_Tcutil_compress(void*t);void Cyc_Tcutil_unchecked_cast(
struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*,void*);int Cyc_Tcutil_coerce_arg(
struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*);int Cyc_Tcutil_coerce_assign(
struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*);int Cyc_Tcutil_coerce_to_bool(
struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*);int Cyc_Tcutil_coerce_list(struct Cyc_Tcenv_Tenv*,
void*,struct Cyc_List_List*);int Cyc_Tcutil_coerce_uint_typ(struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Exp*);int Cyc_Tcutil_coerce_sint_typ(struct Cyc_Tcenv_Tenv*,struct
Cyc_Absyn_Exp*);int Cyc_Tcutil_coerceable(void*);int Cyc_Tcutil_silent_castable(
struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,void*,void*);void*Cyc_Tcutil_castable(
struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,void*,void*);int Cyc_Tcutil_is_integral(
struct Cyc_Absyn_Exp*);int Cyc_Tcutil_is_numeric(struct Cyc_Absyn_Exp*);int Cyc_Tcutil_is_function_type(
void*t);int Cyc_Tcutil_is_pointer_type(void*t);int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*
e);int Cyc_Tcutil_is_pointer_or_boxed(void*t,int*is_dyneither_ptr);extern struct
Cyc_Core_Opt Cyc_Tcutil_rk;extern struct Cyc_Core_Opt Cyc_Tcutil_trk;extern struct Cyc_Core_Opt
Cyc_Tcutil_urk;extern struct Cyc_Core_Opt Cyc_Tcutil_ak;extern struct Cyc_Core_Opt Cyc_Tcutil_bk;
extern struct Cyc_Core_Opt Cyc_Tcutil_mk;extern struct Cyc_Core_Opt Cyc_Tcutil_ek;
extern struct Cyc_Core_Opt Cyc_Tcutil_ik;struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_opt(
void*k);void*Cyc_Tcutil_kind_to_bound(void*k);struct _tuple14{struct Cyc_Absyn_Tvar*
f1;void*f2;};struct _tuple14 Cyc_Tcutil_swap_kind(void*t,void*kb);int Cyc_Tcutil_zero_to_null(
struct Cyc_Tcenv_Tenv*,void*t,struct Cyc_Absyn_Exp*e);void*Cyc_Tcutil_max_arithmetic_type(
void*,void*);void Cyc_Tcutil_explain_failure();int Cyc_Tcutil_unify(void*,void*);
int Cyc_Tcutil_typecmp(void*,void*);void*Cyc_Tcutil_substitute(struct Cyc_List_List*,
void*);void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,
void*);int Cyc_Tcutil_subset_effect(int may_constrain_evars,void*e1,void*e2);int
Cyc_Tcutil_region_in_effect(int constrain,void*r,void*e);void*Cyc_Tcutil_fndecl2typ(
struct Cyc_Absyn_Fndecl*);struct _tuple14*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*,
struct Cyc_Absyn_Tvar*);struct _tuple15{struct Cyc_List_List*f1;struct _RegionHandle*
f2;};struct _tuple14*Cyc_Tcutil_r_make_inst_var(struct _tuple15*,struct Cyc_Absyn_Tvar*);
void Cyc_Tcutil_check_bitfield(struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*
te,void*field_typ,struct Cyc_Absyn_Exp*width,struct _dyneither_ptr*fn);void Cyc_Tcutil_check_contains_assign(
struct Cyc_Absyn_Exp*);void Cyc_Tcutil_check_valid_toplevel_type(struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Tenv*,void*);void Cyc_Tcutil_check_fndecl_valid_type(struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Fndecl*);void Cyc_Tcutil_check_type(struct
Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*bound_tvars,void*
k,int allow_evars,void*);void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*vs,
struct Cyc_Position_Segment*loc,struct _dyneither_ptr err_msg);void Cyc_Tcutil_check_unique_tvars(
struct Cyc_Position_Segment*,struct Cyc_List_List*);void Cyc_Tcutil_check_nonzero_bound(
struct Cyc_Position_Segment*,union Cyc_Absyn_Constraint*);void Cyc_Tcutil_check_bound(
struct Cyc_Position_Segment*,unsigned int i,union Cyc_Absyn_Constraint*);int Cyc_Tcutil_is_bound_one(
union Cyc_Absyn_Constraint*b);int Cyc_Tcutil_equal_tqual(struct Cyc_Absyn_Tqual tq1,
struct Cyc_Absyn_Tqual tq2);struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(
struct _RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,
void*,struct Cyc_List_List*fields);int Cyc_Tcutil_is_tagged_pointer_typ(void*);int
Cyc_Tcutil_is_tagged_pointer_typ_elt(void*t,void**elt_typ_dest);int Cyc_Tcutil_is_zero_pointer_typ_elt(
void*t,void**elt_typ_dest);int Cyc_Tcutil_is_zero_ptr_type(void*t,void**ptr_type,
int*is_dyneither,void**elt_type);int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*
e1,void**ptr_type,int*is_dyneither,void**elt_type);int Cyc_Tcutil_is_noalias_pointer(
void*t);int Cyc_Tcutil_is_noalias_path(struct _RegionHandle*,struct Cyc_Absyn_Exp*e);
int Cyc_Tcutil_is_noalias_pointer_or_aggr(struct _RegionHandle*,void*t);void*Cyc_Tcutil_array_to_ptr(
struct Cyc_Tcenv_Tenv*,void*t,struct Cyc_Absyn_Exp*e);struct _tuple16{int f1;void*f2;
};struct _tuple16 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*
e);void*Cyc_Tcutil_normalize_effect(void*e);struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(
void*k);int Cyc_Tcutil_new_tvar_id();void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*);
void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*);int Cyc_Tcutil_is_temp_tvar(
struct Cyc_Absyn_Tvar*);void Cyc_Tcutil_rewrite_temp_tvar(struct Cyc_Absyn_Tvar*);
int Cyc_Tcutil_same_atts(struct Cyc_List_List*,struct Cyc_List_List*);int Cyc_Tcutil_bits_only(
void*t);int Cyc_Tcutil_is_const_exp(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e);
void*Cyc_Tcutil_snd_tqt(struct _tuple11*);int Cyc_Tcutil_supports_default(void*);
int Cyc_Tcutil_admits_zero(void*t);int Cyc_Tcutil_is_noreturn(void*);int Cyc_Tcutil_extract_const_from_typedef(
struct Cyc_Position_Segment*,int declared_const,void*);struct Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(
void*t,struct Cyc_List_List*atts);int Cyc_Tcutil_rgn_of_pointer(void*t,void**rgn);
void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);struct
Cyc_Tcexp_TestEnv{struct _tuple0*eq;int isTrue;};struct Cyc_Tcexp_TestEnv Cyc_Tcexp_tcTest(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr msg_part);void
Cyc_Tc_tcAggrdecl(struct Cyc_Tcenv_Tenv*,struct Cyc_Tcenv_Genv*,struct Cyc_Position_Segment*,
struct Cyc_Absyn_Aggrdecl*);void Cyc_Tc_tcDatatypedecl(struct Cyc_Tcenv_Tenv*,
struct Cyc_Tcenv_Genv*,struct Cyc_Position_Segment*,struct Cyc_Absyn_Datatypedecl*);
void Cyc_Tc_tcEnumdecl(struct Cyc_Tcenv_Tenv*,struct Cyc_Tcenv_Genv*,struct Cyc_Position_Segment*,
struct Cyc_Absyn_Enumdecl*);extern int Cyc_Cyclone_tovc_r;char Cyc_Tcutil_Unify[10]="\000\000\000\000Unify\000";
void Cyc_Tcutil_unify_it(void*t1,void*t2);void*Cyc_Tcutil_t1_failure=(void*)0;int
Cyc_Tcutil_tq1_const=0;void*Cyc_Tcutil_t2_failure=(void*)0;int Cyc_Tcutil_tq2_const=
0;struct _dyneither_ptr Cyc_Tcutil_failure_reason=(struct _dyneither_ptr){(void*)0,(
void*)0,(void*)(0 + 0)};void Cyc_Tcutil_explain_failure();void Cyc_Tcutil_explain_failure(){
if(Cyc_Position_num_errors >= Cyc_Position_max_errors)return;Cyc_fflush((struct
Cyc___cycFILE*)Cyc_stderr);{const char*_tmpB62;const char*_tmpB61;const char*
_tmpB60;void*_tmpB5F[2];struct Cyc_String_pa_struct _tmpB5E;struct Cyc_String_pa_struct
_tmpB5D;struct _dyneither_ptr s1=(struct _dyneither_ptr)((_tmpB5D.tag=0,((_tmpB5D.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(Cyc_Tcutil_t1_failure)),((
_tmpB5E.tag=0,((_tmpB5E.f1=(struct _dyneither_ptr)(Cyc_Tcutil_tq1_const?(_tmpB61="const ",
_tag_dyneither(_tmpB61,sizeof(char),7)):((_tmpB62="",_tag_dyneither(_tmpB62,
sizeof(char),1)))),((_tmpB5F[0]=& _tmpB5E,((_tmpB5F[1]=& _tmpB5D,Cyc_aprintf(((
_tmpB60="%s%s",_tag_dyneither(_tmpB60,sizeof(char),5))),_tag_dyneither(_tmpB5F,
sizeof(void*),2))))))))))))));const char*_tmpB6B;const char*_tmpB6A;const char*
_tmpB69;void*_tmpB68[2];struct Cyc_String_pa_struct _tmpB67;struct Cyc_String_pa_struct
_tmpB66;struct _dyneither_ptr s2=(struct _dyneither_ptr)((_tmpB66.tag=0,((_tmpB66.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(Cyc_Tcutil_t2_failure)),((
_tmpB67.tag=0,((_tmpB67.f1=(struct _dyneither_ptr)(Cyc_Tcutil_tq2_const?(_tmpB6A="const ",
_tag_dyneither(_tmpB6A,sizeof(char),7)):((_tmpB6B="",_tag_dyneither(_tmpB6B,
sizeof(char),1)))),((_tmpB68[0]=& _tmpB67,((_tmpB68[1]=& _tmpB66,Cyc_aprintf(((
_tmpB69="%s%s",_tag_dyneither(_tmpB69,sizeof(char),5))),_tag_dyneither(_tmpB68,
sizeof(void*),2))))))))))))));int pos=2;{const char*_tmpB6F;void*_tmpB6E[1];struct
Cyc_String_pa_struct _tmpB6D;(_tmpB6D.tag=0,((_tmpB6D.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)s1),((_tmpB6E[0]=& _tmpB6D,Cyc_fprintf(Cyc_stderr,((_tmpB6F="  %s",
_tag_dyneither(_tmpB6F,sizeof(char),5))),_tag_dyneither(_tmpB6E,sizeof(void*),1)))))));}
pos +=_get_dyneither_size(s1,sizeof(char));if(pos + 5 >= 80){{const char*_tmpB72;
void*_tmpB71;(_tmpB71=0,Cyc_fprintf(Cyc_stderr,((_tmpB72="\n\t",_tag_dyneither(
_tmpB72,sizeof(char),3))),_tag_dyneither(_tmpB71,sizeof(void*),0)));}pos=8;}
else{{const char*_tmpB75;void*_tmpB74;(_tmpB74=0,Cyc_fprintf(Cyc_stderr,((_tmpB75=" ",
_tag_dyneither(_tmpB75,sizeof(char),2))),_tag_dyneither(_tmpB74,sizeof(void*),0)));}
++ pos;}{const char*_tmpB78;void*_tmpB77;(_tmpB77=0,Cyc_fprintf(Cyc_stderr,((
_tmpB78="and ",_tag_dyneither(_tmpB78,sizeof(char),5))),_tag_dyneither(_tmpB77,
sizeof(void*),0)));}pos +=4;if(pos + _get_dyneither_size(s2,sizeof(char))>= 80){{
const char*_tmpB7B;void*_tmpB7A;(_tmpB7A=0,Cyc_fprintf(Cyc_stderr,((_tmpB7B="\n\t",
_tag_dyneither(_tmpB7B,sizeof(char),3))),_tag_dyneither(_tmpB7A,sizeof(void*),0)));}
pos=8;}{const char*_tmpB7F;void*_tmpB7E[1];struct Cyc_String_pa_struct _tmpB7D;(
_tmpB7D.tag=0,((_tmpB7D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s2),((
_tmpB7E[0]=& _tmpB7D,Cyc_fprintf(Cyc_stderr,((_tmpB7F="%s ",_tag_dyneither(
_tmpB7F,sizeof(char),4))),_tag_dyneither(_tmpB7E,sizeof(void*),1)))))));}pos +=
_get_dyneither_size(s2,sizeof(char))+ 1;if(pos + 17 >= 80){{const char*_tmpB82;void*
_tmpB81;(_tmpB81=0,Cyc_fprintf(Cyc_stderr,((_tmpB82="\n\t",_tag_dyneither(
_tmpB82,sizeof(char),3))),_tag_dyneither(_tmpB81,sizeof(void*),0)));}pos=8;}{
const char*_tmpB85;void*_tmpB84;(_tmpB84=0,Cyc_fprintf(Cyc_stderr,((_tmpB85="are not compatible. ",
_tag_dyneither(_tmpB85,sizeof(char),21))),_tag_dyneither(_tmpB84,sizeof(void*),0)));}
pos +=17;if(Cyc_Tcutil_failure_reason.curr != (_tag_dyneither(0,0,0)).curr){if(pos
+ Cyc_strlen((struct _dyneither_ptr)Cyc_Tcutil_failure_reason)>= 80){const char*
_tmpB88;void*_tmpB87;(_tmpB87=0,Cyc_fprintf(Cyc_stderr,((_tmpB88="\n\t",
_tag_dyneither(_tmpB88,sizeof(char),3))),_tag_dyneither(_tmpB87,sizeof(void*),0)));}{
const char*_tmpB8C;void*_tmpB8B[1];struct Cyc_String_pa_struct _tmpB8A;(_tmpB8A.tag=
0,((_tmpB8A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_failure_reason),((
_tmpB8B[0]=& _tmpB8A,Cyc_fprintf(Cyc_stderr,((_tmpB8C="%s",_tag_dyneither(_tmpB8C,
sizeof(char),3))),_tag_dyneither(_tmpB8B,sizeof(void*),1)))))));}}{const char*
_tmpB8F;void*_tmpB8E;(_tmpB8E=0,Cyc_fprintf(Cyc_stderr,((_tmpB8F="\n",
_tag_dyneither(_tmpB8F,sizeof(char),2))),_tag_dyneither(_tmpB8E,sizeof(void*),0)));}
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);}}void Cyc_Tcutil_terr(struct Cyc_Position_Segment*
loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);void Cyc_Tcutil_terr(struct
Cyc_Position_Segment*loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){Cyc_Position_post_error(
Cyc_Position_mk_err_elab(loc,(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,
fmt,ap)));}void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){struct
_dyneither_ptr msg=(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap);{
const char*_tmpB93;void*_tmpB92[1];struct Cyc_String_pa_struct _tmpB91;(_tmpB91.tag=
0,((_tmpB91.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg),((_tmpB92[0]=&
_tmpB91,Cyc_fprintf(Cyc_stderr,((_tmpB93="Compiler Error (Tcutil::impos): %s\n",
_tag_dyneither(_tmpB93,sizeof(char),36))),_tag_dyneither(_tmpB92,sizeof(void*),1)))))));}
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);{struct Cyc_Core_Impossible_struct
_tmpB96;struct Cyc_Core_Impossible_struct*_tmpB95;(int)_throw((void*)((_tmpB95=
_cycalloc(sizeof(*_tmpB95)),((_tmpB95[0]=((_tmpB96.tag=Cyc_Core_Impossible,((
_tmpB96.f1=msg,_tmpB96)))),_tmpB95)))));}}static struct _dyneither_ptr Cyc_Tcutil_tvar2string(
struct Cyc_Absyn_Tvar*tv);static struct _dyneither_ptr Cyc_Tcutil_tvar2string(struct
Cyc_Absyn_Tvar*tv){return*tv->name;}void Cyc_Tcutil_print_tvars(struct Cyc_List_List*
tvs);void Cyc_Tcutil_print_tvars(struct Cyc_List_List*tvs){for(0;tvs != 0;tvs=tvs->tl){
const char*_tmpB9B;void*_tmpB9A[2];struct Cyc_String_pa_struct _tmpB99;struct Cyc_String_pa_struct
_tmpB98;(_tmpB98.tag=0,((_tmpB98.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kindbound2string(((struct Cyc_Absyn_Tvar*)tvs->hd)->kind)),((_tmpB99.tag=
0,((_tmpB99.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_tvar2string((
struct Cyc_Absyn_Tvar*)tvs->hd)),((_tmpB9A[0]=& _tmpB99,((_tmpB9A[1]=& _tmpB98,Cyc_fprintf(
Cyc_stderr,((_tmpB9B="%s::%s ",_tag_dyneither(_tmpB9B,sizeof(char),8))),
_tag_dyneither(_tmpB9A,sizeof(void*),2)))))))))))));}{const char*_tmpB9E;void*
_tmpB9D;(_tmpB9D=0,Cyc_fprintf(Cyc_stderr,((_tmpB9E="\n",_tag_dyneither(_tmpB9E,
sizeof(char),2))),_tag_dyneither(_tmpB9D,sizeof(void*),0)));}Cyc_fflush((struct
Cyc___cycFILE*)Cyc_stderr);}static struct Cyc_List_List*Cyc_Tcutil_warning_segs=0;
static struct Cyc_List_List*Cyc_Tcutil_warning_msgs=0;void Cyc_Tcutil_warn(struct
Cyc_Position_Segment*sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);void Cyc_Tcutil_warn(
struct Cyc_Position_Segment*sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
struct _dyneither_ptr msg=(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,
fmt,ap);{struct Cyc_List_List*_tmpB9F;Cyc_Tcutil_warning_segs=((_tmpB9F=_cycalloc(
sizeof(*_tmpB9F)),((_tmpB9F->hd=sg,((_tmpB9F->tl=Cyc_Tcutil_warning_segs,_tmpB9F))))));}{
struct _dyneither_ptr*_tmpBA2;struct Cyc_List_List*_tmpBA1;Cyc_Tcutil_warning_msgs=((
_tmpBA1=_cycalloc(sizeof(*_tmpBA1)),((_tmpBA1->hd=((_tmpBA2=_cycalloc(sizeof(*
_tmpBA2)),((_tmpBA2[0]=msg,_tmpBA2)))),((_tmpBA1->tl=Cyc_Tcutil_warning_msgs,
_tmpBA1))))));}}void Cyc_Tcutil_flush_warnings();void Cyc_Tcutil_flush_warnings(){
if(Cyc_Tcutil_warning_segs == 0)return;{const char*_tmpBA5;void*_tmpBA4;(_tmpBA4=0,
Cyc_fprintf(Cyc_stderr,((_tmpBA5="***Warnings***\n",_tag_dyneither(_tmpBA5,
sizeof(char),16))),_tag_dyneither(_tmpBA4,sizeof(void*),0)));}{struct Cyc_List_List*
_tmp35=Cyc_Position_strings_of_segments(Cyc_Tcutil_warning_segs);Cyc_Tcutil_warning_segs=
0;Cyc_Tcutil_warning_msgs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_Tcutil_warning_msgs);while(Cyc_Tcutil_warning_msgs != 0){{const char*_tmpBAA;
void*_tmpBA9[2];struct Cyc_String_pa_struct _tmpBA8;struct Cyc_String_pa_struct
_tmpBA7;(_tmpBA7.tag=0,((_tmpBA7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((
struct _dyneither_ptr*)((struct Cyc_List_List*)_check_null(Cyc_Tcutil_warning_msgs))->hd)),((
_tmpBA8.tag=0,((_tmpBA8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((
struct _dyneither_ptr*)((struct Cyc_List_List*)_check_null(_tmp35))->hd)),((
_tmpBA9[0]=& _tmpBA8,((_tmpBA9[1]=& _tmpBA7,Cyc_fprintf(Cyc_stderr,((_tmpBAA="%s: %s\n",
_tag_dyneither(_tmpBAA,sizeof(char),8))),_tag_dyneither(_tmpBA9,sizeof(void*),2)))))))))))));}
_tmp35=_tmp35->tl;Cyc_Tcutil_warning_msgs=((struct Cyc_List_List*)_check_null(Cyc_Tcutil_warning_msgs))->tl;}{
const char*_tmpBAD;void*_tmpBAC;(_tmpBAC=0,Cyc_fprintf(Cyc_stderr,((_tmpBAD="**************\n",
_tag_dyneither(_tmpBAD,sizeof(char),16))),_tag_dyneither(_tmpBAC,sizeof(void*),0)));}
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);}}struct Cyc_Core_Opt*Cyc_Tcutil_empty_var_set=
0;static int Cyc_Tcutil_fast_tvar_cmp(struct Cyc_Absyn_Tvar*tv1,struct Cyc_Absyn_Tvar*
tv2);static int Cyc_Tcutil_fast_tvar_cmp(struct Cyc_Absyn_Tvar*tv1,struct Cyc_Absyn_Tvar*
tv2){return tv1->identity - tv2->identity;}void*Cyc_Tcutil_compress(void*t);void*
Cyc_Tcutil_compress(void*t){void*_tmp3C=t;struct Cyc_Core_Opt*_tmp3D;void**_tmp3E;
void**_tmp3F;void***_tmp40;struct Cyc_Core_Opt*_tmp41;struct Cyc_Core_Opt**_tmp42;
_LL1: if(_tmp3C <= (void*)4)goto _LL9;if(*((int*)_tmp3C)!= 0)goto _LL3;_tmp3D=((
struct Cyc_Absyn_Evar_struct*)_tmp3C)->f2;if(_tmp3D != 0)goto _LL3;_LL2: goto _LL4;
_LL3: if(*((int*)_tmp3C)!= 16)goto _LL5;_tmp3E=((struct Cyc_Absyn_TypedefType_struct*)
_tmp3C)->f4;if(_tmp3E != 0)goto _LL5;_LL4: return t;_LL5: if(*((int*)_tmp3C)!= 16)
goto _LL7;_tmp3F=((struct Cyc_Absyn_TypedefType_struct*)_tmp3C)->f4;_tmp40=(void***)&((
struct Cyc_Absyn_TypedefType_struct*)_tmp3C)->f4;_LL6: {void*t2=Cyc_Tcutil_compress(*((
void**)_check_null(*_tmp40)));if(t2 != *((void**)_check_null(*_tmp40))){void**
_tmpBAE;*_tmp40=((_tmpBAE=_cycalloc(sizeof(*_tmpBAE)),((_tmpBAE[0]=t2,_tmpBAE))));}
return t2;}_LL7: if(*((int*)_tmp3C)!= 0)goto _LL9;_tmp41=((struct Cyc_Absyn_Evar_struct*)
_tmp3C)->f2;_tmp42=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)_tmp3C)->f2;
_LL8: {void*t2=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(*
_tmp42))->v);if(t2 != (void*)((struct Cyc_Core_Opt*)_check_null(*_tmp42))->v){
struct Cyc_Core_Opt*_tmpBAF;*_tmp42=((_tmpBAF=_cycalloc(sizeof(*_tmpBAF)),((
_tmpBAF->v=(void*)t2,_tmpBAF))));}return t2;}_LL9:;_LLA: return t;_LL0:;}void*Cyc_Tcutil_copy_type(
void*t);static struct Cyc_List_List*Cyc_Tcutil_copy_types(struct Cyc_List_List*ts);
static struct Cyc_List_List*Cyc_Tcutil_copy_types(struct Cyc_List_List*ts){return
Cyc_List_map(Cyc_Tcutil_copy_type,ts);}static union Cyc_Absyn_Constraint*Cyc_Tcutil_copy_conref(
union Cyc_Absyn_Constraint*cptr);static union Cyc_Absyn_Constraint*Cyc_Tcutil_copy_conref(
union Cyc_Absyn_Constraint*cptr){union Cyc_Absyn_Constraint*_tmp45=cptr;union Cyc_Absyn_Constraint
_tmp46;int _tmp47;union Cyc_Absyn_Constraint _tmp48;void*_tmp49;union Cyc_Absyn_Constraint
_tmp4A;union Cyc_Absyn_Constraint*_tmp4B;_LLC: _tmp46=*_tmp45;if((_tmp46.No_constr).tag
!= 3)goto _LLE;_tmp47=(int)(_tmp46.No_constr).val;_LLD: return Cyc_Absyn_empty_conref();
_LLE: _tmp48=*_tmp45;if((_tmp48.Eq_constr).tag != 1)goto _LL10;_tmp49=(void*)(
_tmp48.Eq_constr).val;_LLF: return Cyc_Absyn_new_conref(_tmp49);_LL10: _tmp4A=*
_tmp45;if((_tmp4A.Forward_constr).tag != 2)goto _LLB;_tmp4B=(union Cyc_Absyn_Constraint*)(
_tmp4A.Forward_constr).val;_LL11: return Cyc_Tcutil_copy_conref(_tmp4B);_LLB:;}
static void*Cyc_Tcutil_copy_kindbound(void*kb);static void*Cyc_Tcutil_copy_kindbound(
void*kb){void*_tmp4C=Cyc_Absyn_compress_kb(kb);void*_tmp4D;_LL13: if(*((int*)
_tmp4C)!= 1)goto _LL15;_LL14: {struct Cyc_Absyn_Unknown_kb_struct _tmpBB2;struct Cyc_Absyn_Unknown_kb_struct*
_tmpBB1;return(void*)((_tmpBB1=_cycalloc(sizeof(*_tmpBB1)),((_tmpBB1[0]=((
_tmpBB2.tag=1,((_tmpBB2.f1=0,_tmpBB2)))),_tmpBB1))));}_LL15: if(*((int*)_tmp4C)!= 
2)goto _LL17;_tmp4D=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp4C)->f2;_LL16: {
struct Cyc_Absyn_Less_kb_struct _tmpBB5;struct Cyc_Absyn_Less_kb_struct*_tmpBB4;
return(void*)((_tmpBB4=_cycalloc(sizeof(*_tmpBB4)),((_tmpBB4[0]=((_tmpBB5.tag=2,((
_tmpBB5.f1=0,((_tmpBB5.f2=(void*)_tmp4D,_tmpBB5)))))),_tmpBB4))));}_LL17:;_LL18:
return kb;_LL12:;}static struct Cyc_Absyn_Tvar*Cyc_Tcutil_copy_tvar(struct Cyc_Absyn_Tvar*
tv);static struct Cyc_Absyn_Tvar*Cyc_Tcutil_copy_tvar(struct Cyc_Absyn_Tvar*tv){
struct Cyc_Absyn_Tvar*_tmpBB6;return(_tmpBB6=_cycalloc(sizeof(*_tmpBB6)),((
_tmpBB6->name=tv->name,((_tmpBB6->identity=- 1,((_tmpBB6->kind=Cyc_Tcutil_copy_kindbound(
tv->kind),_tmpBB6)))))));}static struct _tuple9*Cyc_Tcutil_copy_arg(struct _tuple9*
arg);static struct _tuple9*Cyc_Tcutil_copy_arg(struct _tuple9*arg){struct _tuple9
_tmp54;struct Cyc_Core_Opt*_tmp55;struct Cyc_Absyn_Tqual _tmp56;void*_tmp57;struct
_tuple9*_tmp53=arg;_tmp54=*_tmp53;_tmp55=_tmp54.f1;_tmp56=_tmp54.f2;_tmp57=
_tmp54.f3;{struct _tuple9*_tmpBB7;return(_tmpBB7=_cycalloc(sizeof(*_tmpBB7)),((
_tmpBB7->f1=_tmp55,((_tmpBB7->f2=_tmp56,((_tmpBB7->f3=Cyc_Tcutil_copy_type(
_tmp57),_tmpBB7)))))));}}static struct _tuple11*Cyc_Tcutil_copy_tqt(struct _tuple11*
arg);static struct _tuple11*Cyc_Tcutil_copy_tqt(struct _tuple11*arg){struct _tuple11
_tmp5A;struct Cyc_Absyn_Tqual _tmp5B;void*_tmp5C;struct _tuple11*_tmp59=arg;_tmp5A=*
_tmp59;_tmp5B=_tmp5A.f1;_tmp5C=_tmp5A.f2;{struct _tuple11*_tmpBB8;return(_tmpBB8=
_cycalloc(sizeof(*_tmpBB8)),((_tmpBB8->f1=_tmp5B,((_tmpBB8->f2=Cyc_Tcutil_copy_type(
_tmp5C),_tmpBB8)))));}}static struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_copy_field(
struct Cyc_Absyn_Aggrfield*f);static struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_copy_field(
struct Cyc_Absyn_Aggrfield*f){struct Cyc_Absyn_Aggrfield*_tmpBB9;return(_tmpBB9=
_cycalloc(sizeof(*_tmpBB9)),((_tmpBB9->name=f->name,((_tmpBB9->tq=f->tq,((
_tmpBB9->type=Cyc_Tcutil_copy_type(f->type),((_tmpBB9->width=f->width,((_tmpBB9->attributes=
f->attributes,_tmpBB9)))))))))));}static struct _tuple0*Cyc_Tcutil_copy_rgncmp(
struct _tuple0*x);static struct _tuple0*Cyc_Tcutil_copy_rgncmp(struct _tuple0*x){
struct _tuple0 _tmp60;void*_tmp61;void*_tmp62;struct _tuple0*_tmp5F=x;_tmp60=*
_tmp5F;_tmp61=_tmp60.f1;_tmp62=_tmp60.f2;{struct _tuple0*_tmpBBA;return(_tmpBBA=
_cycalloc(sizeof(*_tmpBBA)),((_tmpBBA->f1=Cyc_Tcutil_copy_type(_tmp61),((_tmpBBA->f2=
Cyc_Tcutil_copy_type(_tmp62),_tmpBBA)))));}}static struct Cyc_Absyn_Enumfield*Cyc_Tcutil_copy_enumfield(
struct Cyc_Absyn_Enumfield*f);static struct Cyc_Absyn_Enumfield*Cyc_Tcutil_copy_enumfield(
struct Cyc_Absyn_Enumfield*f){struct Cyc_Absyn_Enumfield*_tmpBBB;return(_tmpBBB=
_cycalloc(sizeof(*_tmpBBB)),((_tmpBBB->name=f->name,((_tmpBBB->tag=f->tag,((
_tmpBBB->loc=f->loc,_tmpBBB)))))));}void*Cyc_Tcutil_copy_type(void*t);void*Cyc_Tcutil_copy_type(
void*t){void*_tmp65=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*_tmp66;struct Cyc_Absyn_DatatypeInfo
_tmp67;union Cyc_Absyn_DatatypeInfoU _tmp68;struct Cyc_List_List*_tmp69;struct Cyc_Core_Opt*
_tmp6A;struct Cyc_Absyn_DatatypeFieldInfo _tmp6B;union Cyc_Absyn_DatatypeFieldInfoU
_tmp6C;struct Cyc_List_List*_tmp6D;struct Cyc_Absyn_PtrInfo _tmp6E;void*_tmp6F;
struct Cyc_Absyn_Tqual _tmp70;struct Cyc_Absyn_PtrAtts _tmp71;void*_tmp72;union Cyc_Absyn_Constraint*
_tmp73;union Cyc_Absyn_Constraint*_tmp74;union Cyc_Absyn_Constraint*_tmp75;struct
Cyc_Absyn_PtrLoc*_tmp76;struct Cyc_Absyn_ArrayInfo _tmp77;void*_tmp78;struct Cyc_Absyn_Tqual
_tmp79;struct Cyc_Absyn_Exp*_tmp7A;union Cyc_Absyn_Constraint*_tmp7B;struct Cyc_Position_Segment*
_tmp7C;struct Cyc_Absyn_FnInfo _tmp7D;struct Cyc_List_List*_tmp7E;struct Cyc_Core_Opt*
_tmp7F;void*_tmp80;struct Cyc_List_List*_tmp81;int _tmp82;struct Cyc_Absyn_VarargInfo*
_tmp83;struct Cyc_List_List*_tmp84;struct Cyc_List_List*_tmp85;struct Cyc_List_List*
_tmp86;struct Cyc_Absyn_AggrInfo _tmp87;union Cyc_Absyn_AggrInfoU _tmp88;struct
_tuple4 _tmp89;void*_tmp8A;struct _tuple2*_tmp8B;struct Cyc_Core_Opt*_tmp8C;struct
Cyc_List_List*_tmp8D;struct Cyc_Absyn_AggrInfo _tmp8E;union Cyc_Absyn_AggrInfoU
_tmp8F;struct Cyc_Absyn_Aggrdecl**_tmp90;struct Cyc_List_List*_tmp91;void*_tmp92;
struct Cyc_List_List*_tmp93;struct _tuple2*_tmp94;struct Cyc_Absyn_Enumdecl*_tmp95;
struct Cyc_List_List*_tmp96;void*_tmp97;struct Cyc_Absyn_Exp*_tmp98;void*_tmp99;
void*_tmp9A;void*_tmp9B;struct _tuple2*_tmp9C;struct Cyc_List_List*_tmp9D;struct
Cyc_Absyn_Typedefdecl*_tmp9E;void*_tmp9F;struct Cyc_List_List*_tmpA0;void*_tmpA1;
_LL1A: if((int)_tmp65 != 0)goto _LL1C;_LL1B: goto _LL1D;_LL1C: if(_tmp65 <= (void*)4)
goto _LL28;if(*((int*)_tmp65)!= 0)goto _LL1E;_LL1D: return t;_LL1E: if(*((int*)_tmp65)
!= 1)goto _LL20;_tmp66=((struct Cyc_Absyn_VarType_struct*)_tmp65)->f1;_LL1F: {
struct Cyc_Absyn_VarType_struct _tmpBBE;struct Cyc_Absyn_VarType_struct*_tmpBBD;
return(void*)((_tmpBBD=_cycalloc(sizeof(*_tmpBBD)),((_tmpBBD[0]=((_tmpBBE.tag=1,((
_tmpBBE.f1=Cyc_Tcutil_copy_tvar(_tmp66),_tmpBBE)))),_tmpBBD))));}_LL20: if(*((int*)
_tmp65)!= 2)goto _LL22;_tmp67=((struct Cyc_Absyn_DatatypeType_struct*)_tmp65)->f1;
_tmp68=_tmp67.datatype_info;_tmp69=_tmp67.targs;_tmp6A=_tmp67.rgn;_LL21: {struct
Cyc_Core_Opt*_tmpBBF;struct Cyc_Core_Opt*_tmpA4=(unsigned int)_tmp6A?(_tmpBBF=
_cycalloc(sizeof(*_tmpBBF)),((_tmpBBF->v=(void*)Cyc_Tcutil_copy_type((void*)
_tmp6A->v),_tmpBBF))): 0;struct Cyc_Absyn_DatatypeType_struct _tmpBC5;struct Cyc_Absyn_DatatypeInfo
_tmpBC4;struct Cyc_Absyn_DatatypeType_struct*_tmpBC3;return(void*)((_tmpBC3=
_cycalloc(sizeof(*_tmpBC3)),((_tmpBC3[0]=((_tmpBC5.tag=2,((_tmpBC5.f1=((_tmpBC4.datatype_info=
_tmp68,((_tmpBC4.targs=Cyc_Tcutil_copy_types(_tmp69),((_tmpBC4.rgn=_tmpA4,
_tmpBC4)))))),_tmpBC5)))),_tmpBC3))));}_LL22: if(*((int*)_tmp65)!= 3)goto _LL24;
_tmp6B=((struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp65)->f1;_tmp6C=_tmp6B.field_info;
_tmp6D=_tmp6B.targs;_LL23: {struct Cyc_Absyn_DatatypeFieldType_struct _tmpBCB;
struct Cyc_Absyn_DatatypeFieldInfo _tmpBCA;struct Cyc_Absyn_DatatypeFieldType_struct*
_tmpBC9;return(void*)((_tmpBC9=_cycalloc(sizeof(*_tmpBC9)),((_tmpBC9[0]=((
_tmpBCB.tag=3,((_tmpBCB.f1=((_tmpBCA.field_info=_tmp6C,((_tmpBCA.targs=Cyc_Tcutil_copy_types(
_tmp6D),_tmpBCA)))),_tmpBCB)))),_tmpBC9))));}_LL24: if(*((int*)_tmp65)!= 4)goto
_LL26;_tmp6E=((struct Cyc_Absyn_PointerType_struct*)_tmp65)->f1;_tmp6F=_tmp6E.elt_typ;
_tmp70=_tmp6E.elt_tq;_tmp71=_tmp6E.ptr_atts;_tmp72=_tmp71.rgn;_tmp73=_tmp71.nullable;
_tmp74=_tmp71.bounds;_tmp75=_tmp71.zero_term;_tmp76=_tmp71.ptrloc;_LL25: {void*
_tmpAC=Cyc_Tcutil_copy_type(_tmp6F);void*_tmpAD=Cyc_Tcutil_copy_type(_tmp72);
union Cyc_Absyn_Constraint*_tmpAE=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*
cptr))Cyc_Tcutil_copy_conref)(_tmp73);struct Cyc_Absyn_Tqual _tmpAF=_tmp70;union
Cyc_Absyn_Constraint*_tmpB0=Cyc_Tcutil_copy_conref(_tmp74);union Cyc_Absyn_Constraint*
_tmpB1=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(
_tmp75);struct Cyc_Absyn_PointerType_struct _tmpBD5;struct Cyc_Absyn_PtrAtts _tmpBD4;
struct Cyc_Absyn_PtrInfo _tmpBD3;struct Cyc_Absyn_PointerType_struct*_tmpBD2;return(
void*)((_tmpBD2=_cycalloc(sizeof(*_tmpBD2)),((_tmpBD2[0]=((_tmpBD5.tag=4,((
_tmpBD5.f1=((_tmpBD3.elt_typ=_tmpAC,((_tmpBD3.elt_tq=_tmpAF,((_tmpBD3.ptr_atts=((
_tmpBD4.rgn=_tmpAD,((_tmpBD4.nullable=_tmpAE,((_tmpBD4.bounds=_tmpB0,((_tmpBD4.zero_term=
_tmpB1,((_tmpBD4.ptrloc=_tmp76,_tmpBD4)))))))))),_tmpBD3)))))),_tmpBD5)))),
_tmpBD2))));}_LL26: if(*((int*)_tmp65)!= 5)goto _LL28;_LL27: goto _LL29;_LL28: if((
int)_tmp65 != 1)goto _LL2A;_LL29: goto _LL2B;_LL2A: if(_tmp65 <= (void*)4)goto _LL46;
if(*((int*)_tmp65)!= 6)goto _LL2C;_LL2B: return t;_LL2C: if(*((int*)_tmp65)!= 7)goto
_LL2E;_tmp77=((struct Cyc_Absyn_ArrayType_struct*)_tmp65)->f1;_tmp78=_tmp77.elt_type;
_tmp79=_tmp77.tq;_tmp7A=_tmp77.num_elts;_tmp7B=_tmp77.zero_term;_tmp7C=_tmp77.zt_loc;
_LL2D: {struct Cyc_Absyn_ArrayType_struct _tmpBDB;struct Cyc_Absyn_ArrayInfo _tmpBDA;
struct Cyc_Absyn_ArrayType_struct*_tmpBD9;return(void*)((_tmpBD9=_cycalloc(
sizeof(*_tmpBD9)),((_tmpBD9[0]=((_tmpBDB.tag=7,((_tmpBDB.f1=((_tmpBDA.elt_type=
Cyc_Tcutil_copy_type(_tmp78),((_tmpBDA.tq=_tmp79,((_tmpBDA.num_elts=_tmp7A,((
_tmpBDA.zero_term=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*cptr))
Cyc_Tcutil_copy_conref)(_tmp7B),((_tmpBDA.zt_loc=_tmp7C,_tmpBDA)))))))))),
_tmpBDB)))),_tmpBD9))));}_LL2E: if(*((int*)_tmp65)!= 8)goto _LL30;_tmp7D=((struct
Cyc_Absyn_FnType_struct*)_tmp65)->f1;_tmp7E=_tmp7D.tvars;_tmp7F=_tmp7D.effect;
_tmp80=_tmp7D.ret_typ;_tmp81=_tmp7D.args;_tmp82=_tmp7D.c_varargs;_tmp83=_tmp7D.cyc_varargs;
_tmp84=_tmp7D.rgn_po;_tmp85=_tmp7D.attributes;_LL2F: {struct Cyc_List_List*_tmpB9=((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Absyn_Tvar*),struct
Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_tvar,_tmp7E);struct Cyc_Core_Opt*
_tmpBDC;struct Cyc_Core_Opt*_tmpBA=_tmp7F == 0?0:((_tmpBDC=_cycalloc(sizeof(*
_tmpBDC)),((_tmpBDC->v=(void*)Cyc_Tcutil_copy_type((void*)_tmp7F->v),_tmpBDC))));
void*_tmpBB=Cyc_Tcutil_copy_type(_tmp80);struct Cyc_List_List*_tmpBC=((struct Cyc_List_List*(*)(
struct _tuple9*(*f)(struct _tuple9*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_arg,
_tmp81);int _tmpBD=_tmp82;struct Cyc_Absyn_VarargInfo*cyc_varargs2=0;if(_tmp83 != 0){
struct Cyc_Absyn_VarargInfo*cv=(struct Cyc_Absyn_VarargInfo*)_tmp83;struct Cyc_Absyn_VarargInfo*
_tmpBDD;cyc_varargs2=((_tmpBDD=_cycalloc(sizeof(*_tmpBDD)),((_tmpBDD->name=cv->name,((
_tmpBDD->tq=cv->tq,((_tmpBDD->type=Cyc_Tcutil_copy_type(cv->type),((_tmpBDD->inject=
cv->inject,_tmpBDD))))))))));}{struct Cyc_List_List*_tmpBF=((struct Cyc_List_List*(*)(
struct _tuple0*(*f)(struct _tuple0*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_rgncmp,
_tmp84);struct Cyc_List_List*_tmpC0=_tmp85;struct Cyc_Absyn_FnType_struct _tmpBE3;
struct Cyc_Absyn_FnInfo _tmpBE2;struct Cyc_Absyn_FnType_struct*_tmpBE1;return(void*)((
_tmpBE1=_cycalloc(sizeof(*_tmpBE1)),((_tmpBE1[0]=((_tmpBE3.tag=8,((_tmpBE3.f1=((
_tmpBE2.tvars=_tmpB9,((_tmpBE2.effect=_tmpBA,((_tmpBE2.ret_typ=_tmpBB,((_tmpBE2.args=
_tmpBC,((_tmpBE2.c_varargs=_tmpBD,((_tmpBE2.cyc_varargs=cyc_varargs2,((_tmpBE2.rgn_po=
_tmpBF,((_tmpBE2.attributes=_tmpC0,_tmpBE2)))))))))))))))),_tmpBE3)))),_tmpBE1))));}}
_LL30: if(*((int*)_tmp65)!= 9)goto _LL32;_tmp86=((struct Cyc_Absyn_TupleType_struct*)
_tmp65)->f1;_LL31: {struct Cyc_Absyn_TupleType_struct _tmpBE6;struct Cyc_Absyn_TupleType_struct*
_tmpBE5;return(void*)((_tmpBE5=_cycalloc(sizeof(*_tmpBE5)),((_tmpBE5[0]=((
_tmpBE6.tag=9,((_tmpBE6.f1=((struct Cyc_List_List*(*)(struct _tuple11*(*f)(struct
_tuple11*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_tqt,_tmp86),
_tmpBE6)))),_tmpBE5))));}_LL32: if(*((int*)_tmp65)!= 10)goto _LL34;_tmp87=((struct
Cyc_Absyn_AggrType_struct*)_tmp65)->f1;_tmp88=_tmp87.aggr_info;if((_tmp88.UnknownAggr).tag
!= 1)goto _LL34;_tmp89=(struct _tuple4)(_tmp88.UnknownAggr).val;_tmp8A=_tmp89.f1;
_tmp8B=_tmp89.f2;_tmp8C=_tmp89.f3;_tmp8D=_tmp87.targs;_LL33: {struct Cyc_Absyn_AggrType_struct
_tmpBEC;struct Cyc_Absyn_AggrInfo _tmpBEB;struct Cyc_Absyn_AggrType_struct*_tmpBEA;
return(void*)((_tmpBEA=_cycalloc(sizeof(*_tmpBEA)),((_tmpBEA[0]=((_tmpBEC.tag=10,((
_tmpBEC.f1=((_tmpBEB.aggr_info=Cyc_Absyn_UnknownAggr(_tmp8A,_tmp8B,_tmp8C),((
_tmpBEB.targs=Cyc_Tcutil_copy_types(_tmp8D),_tmpBEB)))),_tmpBEC)))),_tmpBEA))));}
_LL34: if(*((int*)_tmp65)!= 10)goto _LL36;_tmp8E=((struct Cyc_Absyn_AggrType_struct*)
_tmp65)->f1;_tmp8F=_tmp8E.aggr_info;if((_tmp8F.KnownAggr).tag != 2)goto _LL36;
_tmp90=(struct Cyc_Absyn_Aggrdecl**)(_tmp8F.KnownAggr).val;_tmp91=_tmp8E.targs;
_LL35: {struct Cyc_Absyn_AggrType_struct _tmpBF2;struct Cyc_Absyn_AggrInfo _tmpBF1;
struct Cyc_Absyn_AggrType_struct*_tmpBF0;return(void*)((_tmpBF0=_cycalloc(sizeof(*
_tmpBF0)),((_tmpBF0[0]=((_tmpBF2.tag=10,((_tmpBF2.f1=((_tmpBF1.aggr_info=Cyc_Absyn_KnownAggr(
_tmp90),((_tmpBF1.targs=Cyc_Tcutil_copy_types(_tmp91),_tmpBF1)))),_tmpBF2)))),
_tmpBF0))));}_LL36: if(*((int*)_tmp65)!= 11)goto _LL38;_tmp92=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp65)->f1;_tmp93=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp65)->f2;_LL37: {
struct Cyc_Absyn_AnonAggrType_struct _tmpBF5;struct Cyc_Absyn_AnonAggrType_struct*
_tmpBF4;return(void*)((_tmpBF4=_cycalloc(sizeof(*_tmpBF4)),((_tmpBF4[0]=((
_tmpBF5.tag=11,((_tmpBF5.f1=(void*)_tmp92,((_tmpBF5.f2=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Tcutil_copy_field,_tmp93),_tmpBF5)))))),_tmpBF4))));}_LL38: if(*((
int*)_tmp65)!= 12)goto _LL3A;_tmp94=((struct Cyc_Absyn_EnumType_struct*)_tmp65)->f1;
_tmp95=((struct Cyc_Absyn_EnumType_struct*)_tmp65)->f2;_LL39: {struct Cyc_Absyn_EnumType_struct
_tmpBF8;struct Cyc_Absyn_EnumType_struct*_tmpBF7;return(void*)((_tmpBF7=_cycalloc(
sizeof(*_tmpBF7)),((_tmpBF7[0]=((_tmpBF8.tag=12,((_tmpBF8.f1=_tmp94,((_tmpBF8.f2=
_tmp95,_tmpBF8)))))),_tmpBF7))));}_LL3A: if(*((int*)_tmp65)!= 13)goto _LL3C;_tmp96=((
struct Cyc_Absyn_AnonEnumType_struct*)_tmp65)->f1;_LL3B: {struct Cyc_Absyn_AnonEnumType_struct
_tmpBFB;struct Cyc_Absyn_AnonEnumType_struct*_tmpBFA;return(void*)((_tmpBFA=
_cycalloc(sizeof(*_tmpBFA)),((_tmpBFA[0]=((_tmpBFB.tag=13,((_tmpBFB.f1=((struct
Cyc_List_List*(*)(struct Cyc_Absyn_Enumfield*(*f)(struct Cyc_Absyn_Enumfield*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_enumfield,_tmp96),_tmpBFB)))),
_tmpBFA))));}_LL3C: if(*((int*)_tmp65)!= 18)goto _LL3E;_tmp97=(void*)((struct Cyc_Absyn_TagType_struct*)
_tmp65)->f1;_LL3D: {struct Cyc_Absyn_TagType_struct _tmpBFE;struct Cyc_Absyn_TagType_struct*
_tmpBFD;return(void*)((_tmpBFD=_cycalloc(sizeof(*_tmpBFD)),((_tmpBFD[0]=((
_tmpBFE.tag=18,((_tmpBFE.f1=(void*)Cyc_Tcutil_copy_type(_tmp97),_tmpBFE)))),
_tmpBFD))));}_LL3E: if(*((int*)_tmp65)!= 17)goto _LL40;_tmp98=((struct Cyc_Absyn_ValueofType_struct*)
_tmp65)->f1;_LL3F: {struct Cyc_Absyn_ValueofType_struct _tmpC01;struct Cyc_Absyn_ValueofType_struct*
_tmpC00;return(void*)((_tmpC00=_cycalloc(sizeof(*_tmpC00)),((_tmpC00[0]=((
_tmpC01.tag=17,((_tmpC01.f1=_tmp98,_tmpC01)))),_tmpC00))));}_LL40: if(*((int*)
_tmp65)!= 14)goto _LL42;_tmp99=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp65)->f1;_LL41: {struct Cyc_Absyn_RgnHandleType_struct _tmpC04;struct Cyc_Absyn_RgnHandleType_struct*
_tmpC03;return(void*)((_tmpC03=_cycalloc(sizeof(*_tmpC03)),((_tmpC03[0]=((
_tmpC04.tag=14,((_tmpC04.f1=(void*)Cyc_Tcutil_copy_type(_tmp99),_tmpC04)))),
_tmpC03))));}_LL42: if(*((int*)_tmp65)!= 15)goto _LL44;_tmp9A=(void*)((struct Cyc_Absyn_DynRgnType_struct*)
_tmp65)->f1;_tmp9B=(void*)((struct Cyc_Absyn_DynRgnType_struct*)_tmp65)->f2;_LL43: {
struct Cyc_Absyn_DynRgnType_struct _tmpC07;struct Cyc_Absyn_DynRgnType_struct*
_tmpC06;return(void*)((_tmpC06=_cycalloc(sizeof(*_tmpC06)),((_tmpC06[0]=((
_tmpC07.tag=15,((_tmpC07.f1=(void*)Cyc_Tcutil_copy_type(_tmp9A),((_tmpC07.f2=(
void*)Cyc_Tcutil_copy_type(_tmp9B),_tmpC07)))))),_tmpC06))));}_LL44: if(*((int*)
_tmp65)!= 16)goto _LL46;_tmp9C=((struct Cyc_Absyn_TypedefType_struct*)_tmp65)->f1;
_tmp9D=((struct Cyc_Absyn_TypedefType_struct*)_tmp65)->f2;_tmp9E=((struct Cyc_Absyn_TypedefType_struct*)
_tmp65)->f3;_LL45: {struct Cyc_Absyn_TypedefType_struct _tmpC0A;struct Cyc_Absyn_TypedefType_struct*
_tmpC09;return(void*)((_tmpC09=_cycalloc(sizeof(*_tmpC09)),((_tmpC09[0]=((
_tmpC0A.tag=16,((_tmpC0A.f1=_tmp9C,((_tmpC0A.f2=Cyc_Tcutil_copy_types(_tmp9D),((
_tmpC0A.f3=_tmp9E,((_tmpC0A.f4=0,_tmpC0A)))))))))),_tmpC09))));}_LL46: if((int)
_tmp65 != 3)goto _LL48;_LL47: goto _LL49;_LL48: if((int)_tmp65 != 2)goto _LL4A;_LL49:
return t;_LL4A: if(_tmp65 <= (void*)4)goto _LL4C;if(*((int*)_tmp65)!= 19)goto _LL4C;
_tmp9F=(void*)((struct Cyc_Absyn_AccessEff_struct*)_tmp65)->f1;_LL4B: {struct Cyc_Absyn_AccessEff_struct
_tmpC0D;struct Cyc_Absyn_AccessEff_struct*_tmpC0C;return(void*)((_tmpC0C=
_cycalloc(sizeof(*_tmpC0C)),((_tmpC0C[0]=((_tmpC0D.tag=19,((_tmpC0D.f1=(void*)
Cyc_Tcutil_copy_type(_tmp9F),_tmpC0D)))),_tmpC0C))));}_LL4C: if(_tmp65 <= (void*)4)
goto _LL4E;if(*((int*)_tmp65)!= 20)goto _LL4E;_tmpA0=((struct Cyc_Absyn_JoinEff_struct*)
_tmp65)->f1;_LL4D: {struct Cyc_Absyn_JoinEff_struct _tmpC10;struct Cyc_Absyn_JoinEff_struct*
_tmpC0F;return(void*)((_tmpC0F=_cycalloc(sizeof(*_tmpC0F)),((_tmpC0F[0]=((
_tmpC10.tag=20,((_tmpC10.f1=Cyc_Tcutil_copy_types(_tmpA0),_tmpC10)))),_tmpC0F))));}
_LL4E: if(_tmp65 <= (void*)4)goto _LL19;if(*((int*)_tmp65)!= 21)goto _LL19;_tmpA1=(
void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp65)->f1;_LL4F: {struct Cyc_Absyn_RgnsEff_struct
_tmpC13;struct Cyc_Absyn_RgnsEff_struct*_tmpC12;return(void*)((_tmpC12=_cycalloc(
sizeof(*_tmpC12)),((_tmpC12[0]=((_tmpC13.tag=21,((_tmpC13.f1=(void*)Cyc_Tcutil_copy_type(
_tmpA1),_tmpC13)))),_tmpC12))));}_LL19:;}int Cyc_Tcutil_kind_leq(void*k1,void*k2);
int Cyc_Tcutil_kind_leq(void*k1,void*k2){if(k1 == k2)return 1;{struct _tuple0 _tmpC14;
struct _tuple0 _tmpE4=(_tmpC14.f1=k1,((_tmpC14.f2=k2,_tmpC14)));void*_tmpE5;void*
_tmpE6;void*_tmpE7;void*_tmpE8;void*_tmpE9;void*_tmpEA;void*_tmpEB;void*_tmpEC;
void*_tmpED;void*_tmpEE;_LL51: _tmpE5=_tmpE4.f1;if((int)_tmpE5 != 2)goto _LL53;
_tmpE6=_tmpE4.f2;if((int)_tmpE6 != 1)goto _LL53;_LL52: goto _LL54;_LL53: _tmpE7=
_tmpE4.f1;if((int)_tmpE7 != 2)goto _LL55;_tmpE8=_tmpE4.f2;if((int)_tmpE8 != 0)goto
_LL55;_LL54: goto _LL56;_LL55: _tmpE9=_tmpE4.f1;if((int)_tmpE9 != 1)goto _LL57;_tmpEA=
_tmpE4.f2;if((int)_tmpEA != 0)goto _LL57;_LL56: goto _LL58;_LL57: _tmpEB=_tmpE4.f1;
if((int)_tmpEB != 3)goto _LL59;_tmpEC=_tmpE4.f2;if((int)_tmpEC != 5)goto _LL59;_LL58:
goto _LL5A;_LL59: _tmpED=_tmpE4.f1;if((int)_tmpED != 4)goto _LL5B;_tmpEE=_tmpE4.f2;
if((int)_tmpEE != 5)goto _LL5B;_LL5A: return 1;_LL5B:;_LL5C: return 0;_LL50:;}}static
int Cyc_Tcutil_is_region_kind(void*k);static int Cyc_Tcutil_is_region_kind(void*k){
void*_tmpEF=k;_LL5E: if((int)_tmpEF != 3)goto _LL60;_LL5F: goto _LL61;_LL60: if((int)
_tmpEF != 5)goto _LL62;_LL61: goto _LL63;_LL62: if((int)_tmpEF != 4)goto _LL64;_LL63:
return 1;_LL64:;_LL65: return 0;_LL5D:;}void*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*
tv);void*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*tv){void*_tmpF0=Cyc_Absyn_compress_kb(
tv->kind);void*_tmpF1;void*_tmpF2;_LL67: if(*((int*)_tmpF0)!= 0)goto _LL69;_tmpF1=(
void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmpF0)->f1;_LL68: return _tmpF1;_LL69: if(*((
int*)_tmpF0)!= 2)goto _LL6B;_tmpF2=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmpF0)->f2;_LL6A: return _tmpF2;_LL6B:;_LL6C: {const char*_tmpC17;void*_tmpC16;(
_tmpC16=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmpC17="kind not suitably constrained!",_tag_dyneither(_tmpC17,sizeof(char),31))),
_tag_dyneither(_tmpC16,sizeof(void*),0)));}_LL66:;}struct _tuple14 Cyc_Tcutil_swap_kind(
void*t,void*kb);struct _tuple14 Cyc_Tcutil_swap_kind(void*t,void*kb){void*_tmpF5=
Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*_tmpF6;_LL6E: if(_tmpF5 <= (void*)4)
goto _LL70;if(*((int*)_tmpF5)!= 1)goto _LL70;_tmpF6=((struct Cyc_Absyn_VarType_struct*)
_tmpF5)->f1;_LL6F: {void*_tmpF7=_tmpF6->kind;_tmpF6->kind=kb;{struct _tuple14
_tmpC18;return(_tmpC18.f1=_tmpF6,((_tmpC18.f2=_tmpF7,_tmpC18)));}}_LL70:;_LL71: {
const char*_tmpC1C;void*_tmpC1B[1];struct Cyc_String_pa_struct _tmpC1A;(_tmpC1A.tag=
0,((_tmpC1A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t)),((_tmpC1B[0]=& _tmpC1A,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap))Cyc_Tcutil_impos)(((_tmpC1C="swap_kind: cannot update the kind of %s",
_tag_dyneither(_tmpC1C,sizeof(char),40))),_tag_dyneither(_tmpC1B,sizeof(void*),1)))))));}
_LL6D:;}void*Cyc_Tcutil_typ_kind(void*t);void*Cyc_Tcutil_typ_kind(void*t){void*
_tmpFC=Cyc_Tcutil_compress(t);struct Cyc_Core_Opt*_tmpFD;struct Cyc_Absyn_Tvar*
_tmpFE;void*_tmpFF;struct Cyc_Absyn_DatatypeFieldInfo _tmp100;union Cyc_Absyn_DatatypeFieldInfoU
_tmp101;struct _tuple3 _tmp102;struct Cyc_Absyn_Datatypedecl*_tmp103;struct Cyc_Absyn_Datatypefield*
_tmp104;struct Cyc_Absyn_DatatypeFieldInfo _tmp105;union Cyc_Absyn_DatatypeFieldInfoU
_tmp106;struct Cyc_Absyn_UnknownDatatypeFieldInfo _tmp107;struct Cyc_Absyn_Enumdecl*
_tmp108;struct Cyc_Absyn_AggrInfo _tmp109;union Cyc_Absyn_AggrInfoU _tmp10A;struct
_tuple4 _tmp10B;struct Cyc_Absyn_AggrInfo _tmp10C;union Cyc_Absyn_AggrInfoU _tmp10D;
struct Cyc_Absyn_Aggrdecl**_tmp10E;struct Cyc_Absyn_Aggrdecl*_tmp10F;struct Cyc_Absyn_Aggrdecl
_tmp110;struct Cyc_Absyn_AggrdeclImpl*_tmp111;struct Cyc_Absyn_Enumdecl*_tmp112;
struct Cyc_Absyn_PtrInfo _tmp113;struct Cyc_Absyn_Typedefdecl*_tmp114;_LL73: if(
_tmpFC <= (void*)4)goto _LL77;if(*((int*)_tmpFC)!= 0)goto _LL75;_tmpFD=((struct Cyc_Absyn_Evar_struct*)
_tmpFC)->f1;_LL74: return(void*)((struct Cyc_Core_Opt*)_check_null(_tmpFD))->v;
_LL75: if(*((int*)_tmpFC)!= 1)goto _LL77;_tmpFE=((struct Cyc_Absyn_VarType_struct*)
_tmpFC)->f1;_LL76: return Cyc_Tcutil_tvar_kind(_tmpFE);_LL77: if((int)_tmpFC != 0)
goto _LL79;_LL78: return(void*)1;_LL79: if(_tmpFC <= (void*)4)goto _LL7B;if(*((int*)
_tmpFC)!= 5)goto _LL7B;_tmpFF=(void*)((struct Cyc_Absyn_IntType_struct*)_tmpFC)->f2;
_LL7A: return(_tmpFF == (void*)((void*)2) || _tmpFF == (void*)((void*)3))?(void*)2:(
void*)1;_LL7B: if((int)_tmpFC != 1)goto _LL7D;_LL7C: goto _LL7E;_LL7D: if(_tmpFC <= (
void*)4)goto _LL85;if(*((int*)_tmpFC)!= 6)goto _LL7F;_LL7E: goto _LL80;_LL7F: if(*((
int*)_tmpFC)!= 8)goto _LL81;_LL80: return(void*)1;_LL81: if(*((int*)_tmpFC)!= 15)
goto _LL83;_LL82: goto _LL84;_LL83: if(*((int*)_tmpFC)!= 14)goto _LL85;_LL84: return(
void*)2;_LL85: if((int)_tmpFC != 3)goto _LL87;_LL86: return(void*)4;_LL87: if((int)
_tmpFC != 2)goto _LL89;_LL88: return(void*)3;_LL89: if(_tmpFC <= (void*)4)goto _LL8B;
if(*((int*)_tmpFC)!= 2)goto _LL8B;_LL8A: return(void*)2;_LL8B: if(_tmpFC <= (void*)4)
goto _LL8D;if(*((int*)_tmpFC)!= 3)goto _LL8D;_tmp100=((struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmpFC)->f1;_tmp101=_tmp100.field_info;if((_tmp101.KnownDatatypefield).tag != 2)
goto _LL8D;_tmp102=(struct _tuple3)(_tmp101.KnownDatatypefield).val;_tmp103=
_tmp102.f1;_tmp104=_tmp102.f2;_LL8C: if(_tmp104->typs == 0)return(void*)2;else{
return(void*)1;}_LL8D: if(_tmpFC <= (void*)4)goto _LL8F;if(*((int*)_tmpFC)!= 3)goto
_LL8F;_tmp105=((struct Cyc_Absyn_DatatypeFieldType_struct*)_tmpFC)->f1;_tmp106=
_tmp105.field_info;if((_tmp106.UnknownDatatypefield).tag != 1)goto _LL8F;_tmp107=(
struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmp106.UnknownDatatypefield).val;
_LL8E: {const char*_tmpC1F;void*_tmpC1E;(_tmpC1E=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpC1F="typ_kind: Unresolved DatatypeFieldType",
_tag_dyneither(_tmpC1F,sizeof(char),39))),_tag_dyneither(_tmpC1E,sizeof(void*),0)));}
_LL8F: if(_tmpFC <= (void*)4)goto _LL91;if(*((int*)_tmpFC)!= 12)goto _LL91;_tmp108=((
struct Cyc_Absyn_EnumType_struct*)_tmpFC)->f2;if(_tmp108 != 0)goto _LL91;_LL90: goto
_LL92;_LL91: if(_tmpFC <= (void*)4)goto _LL93;if(*((int*)_tmpFC)!= 10)goto _LL93;
_tmp109=((struct Cyc_Absyn_AggrType_struct*)_tmpFC)->f1;_tmp10A=_tmp109.aggr_info;
if((_tmp10A.UnknownAggr).tag != 1)goto _LL93;_tmp10B=(struct _tuple4)(_tmp10A.UnknownAggr).val;
_LL92: return(void*)0;_LL93: if(_tmpFC <= (void*)4)goto _LL95;if(*((int*)_tmpFC)!= 
10)goto _LL95;_tmp10C=((struct Cyc_Absyn_AggrType_struct*)_tmpFC)->f1;_tmp10D=
_tmp10C.aggr_info;if((_tmp10D.KnownAggr).tag != 2)goto _LL95;_tmp10E=(struct Cyc_Absyn_Aggrdecl**)(
_tmp10D.KnownAggr).val;_tmp10F=*_tmp10E;_tmp110=*_tmp10F;_tmp111=_tmp110.impl;
_LL94: return _tmp111 == 0?(void*)0:(void*)1;_LL95: if(_tmpFC <= (void*)4)goto _LL97;
if(*((int*)_tmpFC)!= 11)goto _LL97;_LL96: goto _LL98;_LL97: if(_tmpFC <= (void*)4)
goto _LL99;if(*((int*)_tmpFC)!= 13)goto _LL99;_LL98: return(void*)1;_LL99: if(_tmpFC
<= (void*)4)goto _LL9B;if(*((int*)_tmpFC)!= 12)goto _LL9B;_tmp112=((struct Cyc_Absyn_EnumType_struct*)
_tmpFC)->f2;_LL9A: if(_tmp112->fields == 0)return(void*)0;else{return(void*)1;}
_LL9B: if(_tmpFC <= (void*)4)goto _LL9D;if(*((int*)_tmpFC)!= 4)goto _LL9D;_tmp113=((
struct Cyc_Absyn_PointerType_struct*)_tmpFC)->f1;_LL9C: {void*_tmp117=Cyc_Absyn_conref_def((
void*)((void*)0),(_tmp113.ptr_atts).bounds);_LLAE: if((int)_tmp117 != 0)goto _LLB0;
_LLAF: return(void*)1;_LLB0: if(_tmp117 <= (void*)1)goto _LLAD;if(*((int*)_tmp117)!= 
0)goto _LLAD;_LLB1: return(void*)2;_LLAD:;}_LL9D: if(_tmpFC <= (void*)4)goto _LL9F;
if(*((int*)_tmpFC)!= 17)goto _LL9F;_LL9E: return(void*)7;_LL9F: if(_tmpFC <= (void*)
4)goto _LLA1;if(*((int*)_tmpFC)!= 18)goto _LLA1;_LLA0: return(void*)2;_LLA1: if(
_tmpFC <= (void*)4)goto _LLA3;if(*((int*)_tmpFC)!= 7)goto _LLA3;_LLA2: goto _LLA4;
_LLA3: if(_tmpFC <= (void*)4)goto _LLA5;if(*((int*)_tmpFC)!= 9)goto _LLA5;_LLA4:
return(void*)1;_LLA5: if(_tmpFC <= (void*)4)goto _LLA7;if(*((int*)_tmpFC)!= 16)goto
_LLA7;_tmp114=((struct Cyc_Absyn_TypedefType_struct*)_tmpFC)->f3;_LLA6: if(_tmp114
== 0  || _tmp114->kind == 0){const char*_tmpC23;void*_tmpC22[1];struct Cyc_String_pa_struct
_tmpC21;(_tmpC21.tag=0,((_tmpC21.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmpC22[0]=& _tmpC21,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpC23="typ_kind: typedef found: %s",
_tag_dyneither(_tmpC23,sizeof(char),28))),_tag_dyneither(_tmpC22,sizeof(void*),1)))))));}
return(void*)((struct Cyc_Core_Opt*)_check_null(_tmp114->kind))->v;_LLA7: if(
_tmpFC <= (void*)4)goto _LLA9;if(*((int*)_tmpFC)!= 19)goto _LLA9;_LLA8: goto _LLAA;
_LLA9: if(_tmpFC <= (void*)4)goto _LLAB;if(*((int*)_tmpFC)!= 20)goto _LLAB;_LLAA:
goto _LLAC;_LLAB: if(_tmpFC <= (void*)4)goto _LL72;if(*((int*)_tmpFC)!= 21)goto _LL72;
_LLAC: return(void*)6;_LL72:;}int Cyc_Tcutil_unify(void*t1,void*t2);int Cyc_Tcutil_unify(
void*t1,void*t2){struct _handler_cons _tmp11B;_push_handler(& _tmp11B);{int _tmp11D=
0;if(setjmp(_tmp11B.handler))_tmp11D=1;if(!_tmp11D){Cyc_Tcutil_unify_it(t1,t2);{
int _tmp11E=1;_npop_handler(0);return _tmp11E;};_pop_handler();}else{void*_tmp11C=(
void*)_exn_thrown;void*_tmp120=_tmp11C;_LLB3: if(_tmp120 != Cyc_Tcutil_Unify)goto
_LLB5;_LLB4: return 0;_LLB5:;_LLB6:(void)_throw(_tmp120);_LLB2:;}}}static void Cyc_Tcutil_occurslist(
void*evar,struct _RegionHandle*r,struct Cyc_List_List*env,struct Cyc_List_List*ts);
static void Cyc_Tcutil_occurs(void*evar,struct _RegionHandle*r,struct Cyc_List_List*
env,void*t);static void Cyc_Tcutil_occurs(void*evar,struct _RegionHandle*r,struct
Cyc_List_List*env,void*t){t=Cyc_Tcutil_compress(t);{void*_tmp121=t;struct Cyc_Absyn_Tvar*
_tmp122;struct Cyc_Core_Opt*_tmp123;struct Cyc_Core_Opt*_tmp124;struct Cyc_Core_Opt**
_tmp125;struct Cyc_Absyn_PtrInfo _tmp126;struct Cyc_Absyn_ArrayInfo _tmp127;void*
_tmp128;struct Cyc_Absyn_FnInfo _tmp129;struct Cyc_List_List*_tmp12A;struct Cyc_Core_Opt*
_tmp12B;void*_tmp12C;struct Cyc_List_List*_tmp12D;int _tmp12E;struct Cyc_Absyn_VarargInfo*
_tmp12F;struct Cyc_List_List*_tmp130;struct Cyc_List_List*_tmp131;struct Cyc_List_List*
_tmp132;struct Cyc_Absyn_DatatypeInfo _tmp133;struct Cyc_List_List*_tmp134;struct
Cyc_Core_Opt*_tmp135;struct Cyc_List_List*_tmp136;struct Cyc_Absyn_DatatypeFieldInfo
_tmp137;struct Cyc_List_List*_tmp138;struct Cyc_Absyn_AggrInfo _tmp139;struct Cyc_List_List*
_tmp13A;struct Cyc_List_List*_tmp13B;void*_tmp13C;void*_tmp13D;void*_tmp13E;void*
_tmp13F;struct Cyc_List_List*_tmp140;_LLB8: if(_tmp121 <= (void*)4)goto _LLD8;if(*((
int*)_tmp121)!= 1)goto _LLBA;_tmp122=((struct Cyc_Absyn_VarType_struct*)_tmp121)->f1;
_LLB9: if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,
env,_tmp122)){{const char*_tmpC24;Cyc_Tcutil_failure_reason=((_tmpC24="(type variable would escape scope)",
_tag_dyneither(_tmpC24,sizeof(char),35)));}(int)_throw((void*)Cyc_Tcutil_Unify);}
goto _LLB7;_LLBA: if(*((int*)_tmp121)!= 0)goto _LLBC;_tmp123=((struct Cyc_Absyn_Evar_struct*)
_tmp121)->f2;_tmp124=((struct Cyc_Absyn_Evar_struct*)_tmp121)->f4;_tmp125=(struct
Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)_tmp121)->f4;_LLBB: if(t == evar){{
const char*_tmpC25;Cyc_Tcutil_failure_reason=((_tmpC25="(occurs check)",
_tag_dyneither(_tmpC25,sizeof(char),15)));}(int)_throw((void*)Cyc_Tcutil_Unify);}
else{if(_tmp123 != 0)Cyc_Tcutil_occurs(evar,r,env,(void*)_tmp123->v);else{int
problem=0;{struct Cyc_List_List*s=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(*_tmp125))->v;for(0;s != 0;s=s->tl){if(!((int(*)(int(*compare)(struct
Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*
x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,(struct Cyc_Absyn_Tvar*)s->hd)){
problem=1;break;}}}if(problem){struct Cyc_List_List*_tmp143=0;{struct Cyc_List_List*
s=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*_tmp125))->v;for(0;s
!= 0;s=s->tl){if(((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,
env,(struct Cyc_Absyn_Tvar*)s->hd)){struct Cyc_List_List*_tmpC26;_tmp143=((_tmpC26=
_cycalloc(sizeof(*_tmpC26)),((_tmpC26->hd=(struct Cyc_Absyn_Tvar*)s->hd,((_tmpC26->tl=
_tmp143,_tmpC26))))));}}}{struct Cyc_Core_Opt*_tmpC27;*_tmp125=((_tmpC27=
_cycalloc(sizeof(*_tmpC27)),((_tmpC27->v=_tmp143,_tmpC27))));}}}}goto _LLB7;_LLBC:
if(*((int*)_tmp121)!= 4)goto _LLBE;_tmp126=((struct Cyc_Absyn_PointerType_struct*)
_tmp121)->f1;_LLBD: Cyc_Tcutil_occurs(evar,r,env,_tmp126.elt_typ);Cyc_Tcutil_occurs(
evar,r,env,(_tmp126.ptr_atts).rgn);goto _LLB7;_LLBE: if(*((int*)_tmp121)!= 7)goto
_LLC0;_tmp127=((struct Cyc_Absyn_ArrayType_struct*)_tmp121)->f1;_tmp128=_tmp127.elt_type;
_LLBF: Cyc_Tcutil_occurs(evar,r,env,_tmp128);goto _LLB7;_LLC0: if(*((int*)_tmp121)
!= 8)goto _LLC2;_tmp129=((struct Cyc_Absyn_FnType_struct*)_tmp121)->f1;_tmp12A=
_tmp129.tvars;_tmp12B=_tmp129.effect;_tmp12C=_tmp129.ret_typ;_tmp12D=_tmp129.args;
_tmp12E=_tmp129.c_varargs;_tmp12F=_tmp129.cyc_varargs;_tmp130=_tmp129.rgn_po;
_tmp131=_tmp129.attributes;_LLC1: env=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(r,
_tmp12A,env);if(_tmp12B != 0)Cyc_Tcutil_occurs(evar,r,env,(void*)_tmp12B->v);Cyc_Tcutil_occurs(
evar,r,env,_tmp12C);for(0;_tmp12D != 0;_tmp12D=_tmp12D->tl){Cyc_Tcutil_occurs(
evar,r,env,(*((struct _tuple9*)_tmp12D->hd)).f3);}if(_tmp12F != 0)Cyc_Tcutil_occurs(
evar,r,env,_tmp12F->type);for(0;_tmp130 != 0;_tmp130=_tmp130->tl){struct _tuple0
_tmp147;void*_tmp148;void*_tmp149;struct _tuple0*_tmp146=(struct _tuple0*)_tmp130->hd;
_tmp147=*_tmp146;_tmp148=_tmp147.f1;_tmp149=_tmp147.f2;Cyc_Tcutil_occurs(evar,r,
env,_tmp148);Cyc_Tcutil_occurs(evar,r,env,_tmp149);}goto _LLB7;_LLC2: if(*((int*)
_tmp121)!= 9)goto _LLC4;_tmp132=((struct Cyc_Absyn_TupleType_struct*)_tmp121)->f1;
_LLC3: for(0;_tmp132 != 0;_tmp132=_tmp132->tl){Cyc_Tcutil_occurs(evar,r,env,(*((
struct _tuple11*)_tmp132->hd)).f2);}goto _LLB7;_LLC4: if(*((int*)_tmp121)!= 2)goto
_LLC6;_tmp133=((struct Cyc_Absyn_DatatypeType_struct*)_tmp121)->f1;_tmp134=
_tmp133.targs;_tmp135=_tmp133.rgn;_LLC5: if((unsigned int)_tmp135)Cyc_Tcutil_occurs(
evar,r,env,(void*)_tmp135->v);Cyc_Tcutil_occurslist(evar,r,env,_tmp134);goto
_LLB7;_LLC6: if(*((int*)_tmp121)!= 16)goto _LLC8;_tmp136=((struct Cyc_Absyn_TypedefType_struct*)
_tmp121)->f2;_LLC7: _tmp138=_tmp136;goto _LLC9;_LLC8: if(*((int*)_tmp121)!= 3)goto
_LLCA;_tmp137=((struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp121)->f1;_tmp138=
_tmp137.targs;_LLC9: _tmp13A=_tmp138;goto _LLCB;_LLCA: if(*((int*)_tmp121)!= 10)
goto _LLCC;_tmp139=((struct Cyc_Absyn_AggrType_struct*)_tmp121)->f1;_tmp13A=
_tmp139.targs;_LLCB: Cyc_Tcutil_occurslist(evar,r,env,_tmp13A);goto _LLB7;_LLCC:
if(*((int*)_tmp121)!= 11)goto _LLCE;_tmp13B=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp121)->f2;_LLCD: for(0;_tmp13B != 0;_tmp13B=_tmp13B->tl){Cyc_Tcutil_occurs(evar,
r,env,((struct Cyc_Absyn_Aggrfield*)_tmp13B->hd)->type);}goto _LLB7;_LLCE: if(*((
int*)_tmp121)!= 18)goto _LLD0;_tmp13C=(void*)((struct Cyc_Absyn_TagType_struct*)
_tmp121)->f1;_LLCF: _tmp13D=_tmp13C;goto _LLD1;_LLD0: if(*((int*)_tmp121)!= 19)goto
_LLD2;_tmp13D=(void*)((struct Cyc_Absyn_AccessEff_struct*)_tmp121)->f1;_LLD1:
_tmp13E=_tmp13D;goto _LLD3;_LLD2: if(*((int*)_tmp121)!= 14)goto _LLD4;_tmp13E=(void*)((
struct Cyc_Absyn_RgnHandleType_struct*)_tmp121)->f1;_LLD3: _tmp13F=_tmp13E;goto
_LLD5;_LLD4: if(*((int*)_tmp121)!= 21)goto _LLD6;_tmp13F=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp121)->f1;_LLD5: Cyc_Tcutil_occurs(evar,r,env,_tmp13F);goto _LLB7;_LLD6: if(*((
int*)_tmp121)!= 20)goto _LLD8;_tmp140=((struct Cyc_Absyn_JoinEff_struct*)_tmp121)->f1;
_LLD7: Cyc_Tcutil_occurslist(evar,r,env,_tmp140);goto _LLB7;_LLD8:;_LLD9: goto _LLB7;
_LLB7:;}}static void Cyc_Tcutil_occurslist(void*evar,struct _RegionHandle*r,struct
Cyc_List_List*env,struct Cyc_List_List*ts);static void Cyc_Tcutil_occurslist(void*
evar,struct _RegionHandle*r,struct Cyc_List_List*env,struct Cyc_List_List*ts){for(0;
ts != 0;ts=ts->tl){Cyc_Tcutil_occurs(evar,r,env,(void*)ts->hd);}}static void Cyc_Tcutil_unify_list(
struct Cyc_List_List*t1,struct Cyc_List_List*t2);static void Cyc_Tcutil_unify_list(
struct Cyc_List_List*t1,struct Cyc_List_List*t2){for(0;t1 != 0  && t2 != 0;(t1=t1->tl,
t2=t2->tl)){Cyc_Tcutil_unify_it((void*)t1->hd,(void*)t2->hd);}if(t1 != 0  || t2 != 
0)(int)_throw((void*)Cyc_Tcutil_Unify);}static void Cyc_Tcutil_unify_tqual(struct
Cyc_Absyn_Tqual tq1,void*t1,struct Cyc_Absyn_Tqual tq2,void*t2);static void Cyc_Tcutil_unify_tqual(
struct Cyc_Absyn_Tqual tq1,void*t1,struct Cyc_Absyn_Tqual tq2,void*t2){if(tq1.print_const
 && !tq1.real_const){const char*_tmpC2A;void*_tmpC29;(_tmpC29=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpC2A="tq1 real_const not set.",
_tag_dyneither(_tmpC2A,sizeof(char),24))),_tag_dyneither(_tmpC29,sizeof(void*),0)));}
if(tq2.print_const  && !tq2.real_const){const char*_tmpC2D;void*_tmpC2C;(_tmpC2C=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmpC2D="tq2 real_const not set.",_tag_dyneither(_tmpC2D,sizeof(char),24))),
_tag_dyneither(_tmpC2C,sizeof(void*),0)));}if((tq1.real_const != tq2.real_const
 || tq1.q_volatile != tq2.q_volatile) || tq1.q_restrict != tq2.q_restrict){Cyc_Tcutil_t1_failure=
t1;Cyc_Tcutil_t2_failure=t2;Cyc_Tcutil_tq1_const=tq1.real_const;Cyc_Tcutil_tq2_const=
tq2.real_const;{const char*_tmpC2E;Cyc_Tcutil_failure_reason=((_tmpC2E="(qualifiers don't match)",
_tag_dyneither(_tmpC2E,sizeof(char),25)));}(int)_throw((void*)Cyc_Tcutil_Unify);}
Cyc_Tcutil_tq1_const=0;Cyc_Tcutil_tq2_const=0;}int Cyc_Tcutil_equal_tqual(struct
Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2);int Cyc_Tcutil_equal_tqual(struct Cyc_Absyn_Tqual
tq1,struct Cyc_Absyn_Tqual tq2){return(tq1.real_const == tq2.real_const  && tq1.q_volatile
== tq2.q_volatile) && tq1.q_restrict == tq2.q_restrict;}static void Cyc_Tcutil_unify_it_conrefs(
int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,
struct _dyneither_ptr reason);static void Cyc_Tcutil_unify_it_conrefs(int(*cmp)(void*,
void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct
_dyneither_ptr reason){x=Cyc_Absyn_compress_conref(x);y=Cyc_Absyn_compress_conref(
y);if(x == y)return;{union Cyc_Absyn_Constraint*_tmp14F=x;union Cyc_Absyn_Constraint
_tmp150;int _tmp151;union Cyc_Absyn_Constraint _tmp152;void*_tmp153;union Cyc_Absyn_Constraint
_tmp154;union Cyc_Absyn_Constraint*_tmp155;_LLDB: _tmp150=*_tmp14F;if((_tmp150.No_constr).tag
!= 3)goto _LLDD;_tmp151=(int)(_tmp150.No_constr).val;_LLDC:{union Cyc_Absyn_Constraint
_tmpC2F;*x=(((_tmpC2F.Forward_constr).val=y,(((_tmpC2F.Forward_constr).tag=2,
_tmpC2F))));}return;_LLDD: _tmp152=*_tmp14F;if((_tmp152.Eq_constr).tag != 1)goto
_LLDF;_tmp153=(void*)(_tmp152.Eq_constr).val;_LLDE: {union Cyc_Absyn_Constraint*
_tmp157=y;union Cyc_Absyn_Constraint _tmp158;int _tmp159;union Cyc_Absyn_Constraint
_tmp15A;void*_tmp15B;union Cyc_Absyn_Constraint _tmp15C;union Cyc_Absyn_Constraint*
_tmp15D;_LLE2: _tmp158=*_tmp157;if((_tmp158.No_constr).tag != 3)goto _LLE4;_tmp159=(
int)(_tmp158.No_constr).val;_LLE3:*y=*x;return;_LLE4: _tmp15A=*_tmp157;if((
_tmp15A.Eq_constr).tag != 1)goto _LLE6;_tmp15B=(void*)(_tmp15A.Eq_constr).val;
_LLE5: if(cmp(_tmp153,_tmp15B)!= 0){Cyc_Tcutil_failure_reason=reason;(int)_throw((
void*)Cyc_Tcutil_Unify);}return;_LLE6: _tmp15C=*_tmp157;if((_tmp15C.Forward_constr).tag
!= 2)goto _LLE1;_tmp15D=(union Cyc_Absyn_Constraint*)(_tmp15C.Forward_constr).val;
_LLE7: {const char*_tmpC32;void*_tmpC31;(_tmpC31=0,Cyc_Tcutil_impos(((_tmpC32="unify_conref: forward after compress(2)",
_tag_dyneither(_tmpC32,sizeof(char),40))),_tag_dyneither(_tmpC31,sizeof(void*),0)));}
_LLE1:;}_LLDF: _tmp154=*_tmp14F;if((_tmp154.Forward_constr).tag != 2)goto _LLDA;
_tmp155=(union Cyc_Absyn_Constraint*)(_tmp154.Forward_constr).val;_LLE0: {const
char*_tmpC35;void*_tmpC34;(_tmpC34=0,Cyc_Tcutil_impos(((_tmpC35="unify_conref: forward after compress",
_tag_dyneither(_tmpC35,sizeof(char),37))),_tag_dyneither(_tmpC34,sizeof(void*),0)));}
_LLDA:;}}static int Cyc_Tcutil_unify_conrefs(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*
x,union Cyc_Absyn_Constraint*y);static int Cyc_Tcutil_unify_conrefs(int(*cmp)(void*,
void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y){struct
_handler_cons _tmp162;_push_handler(& _tmp162);{int _tmp164=0;if(setjmp(_tmp162.handler))
_tmp164=1;if(!_tmp164){Cyc_Tcutil_unify_it_conrefs(cmp,x,y,(struct _dyneither_ptr)
_tag_dyneither(0,0,0));{int _tmp165=1;_npop_handler(0);return _tmp165;};
_pop_handler();}else{void*_tmp163=(void*)_exn_thrown;void*_tmp167=_tmp163;_LLE9:
if(_tmp167 != Cyc_Tcutil_Unify)goto _LLEB;_LLEA: return 0;_LLEB:;_LLEC:(void)_throw(
_tmp167);_LLE8:;}}}static int Cyc_Tcutil_boundscmp(void*b1,void*b2);static int Cyc_Tcutil_boundscmp(
void*b1,void*b2){struct _tuple0 _tmpC36;struct _tuple0 _tmp169=(_tmpC36.f1=b1,((
_tmpC36.f2=b2,_tmpC36)));void*_tmp16A;void*_tmp16B;void*_tmp16C;void*_tmp16D;
void*_tmp16E;struct Cyc_Absyn_Exp*_tmp16F;void*_tmp170;struct Cyc_Absyn_Exp*
_tmp171;_LLEE: _tmp16A=_tmp169.f1;if((int)_tmp16A != 0)goto _LLF0;_tmp16B=_tmp169.f2;
if((int)_tmp16B != 0)goto _LLF0;_LLEF: return 0;_LLF0: _tmp16C=_tmp169.f1;if((int)
_tmp16C != 0)goto _LLF2;_LLF1: return - 1;_LLF2: _tmp16D=_tmp169.f2;if((int)_tmp16D != 
0)goto _LLF4;_LLF3: return 1;_LLF4: _tmp16E=_tmp169.f1;if(_tmp16E <= (void*)1)goto
_LLED;if(*((int*)_tmp16E)!= 0)goto _LLED;_tmp16F=((struct Cyc_Absyn_Upper_b_struct*)
_tmp16E)->f1;_tmp170=_tmp169.f2;if(_tmp170 <= (void*)1)goto _LLED;if(*((int*)
_tmp170)!= 0)goto _LLED;_tmp171=((struct Cyc_Absyn_Upper_b_struct*)_tmp170)->f1;
_LLF5: return Cyc_Evexp_const_exp_cmp(_tmp16F,_tmp171);_LLED:;}static int Cyc_Tcutil_unify_it_bounds(
void*b1,void*b2);static int Cyc_Tcutil_unify_it_bounds(void*b1,void*b2){struct
_tuple0 _tmpC37;struct _tuple0 _tmp173=(_tmpC37.f1=b1,((_tmpC37.f2=b2,_tmpC37)));
void*_tmp174;void*_tmp175;void*_tmp176;void*_tmp177;void*_tmp178;struct Cyc_Absyn_Exp*
_tmp179;void*_tmp17A;struct Cyc_Absyn_Exp*_tmp17B;_LLF7: _tmp174=_tmp173.f1;if((
int)_tmp174 != 0)goto _LLF9;_tmp175=_tmp173.f2;if((int)_tmp175 != 0)goto _LLF9;_LLF8:
return 0;_LLF9: _tmp176=_tmp173.f1;if((int)_tmp176 != 0)goto _LLFB;_LLFA: return - 1;
_LLFB: _tmp177=_tmp173.f2;if((int)_tmp177 != 0)goto _LLFD;_LLFC: return 1;_LLFD:
_tmp178=_tmp173.f1;if(_tmp178 <= (void*)1)goto _LLF6;if(*((int*)_tmp178)!= 0)goto
_LLF6;_tmp179=((struct Cyc_Absyn_Upper_b_struct*)_tmp178)->f1;_tmp17A=_tmp173.f2;
if(_tmp17A <= (void*)1)goto _LLF6;if(*((int*)_tmp17A)!= 0)goto _LLF6;_tmp17B=((
struct Cyc_Absyn_Upper_b_struct*)_tmp17A)->f1;_LLFE: return Cyc_Evexp_const_exp_cmp(
_tmp179,_tmp17B);_LLF6:;}static int Cyc_Tcutil_attribute_case_number(void*att);
static int Cyc_Tcutil_attribute_case_number(void*att){void*_tmp17C=att;_LL100: if(
_tmp17C <= (void*)17)goto _LL102;if(*((int*)_tmp17C)!= 0)goto _LL102;_LL101: return 0;
_LL102: if((int)_tmp17C != 0)goto _LL104;_LL103: return 1;_LL104: if((int)_tmp17C != 1)
goto _LL106;_LL105: return 2;_LL106: if((int)_tmp17C != 2)goto _LL108;_LL107: return 3;
_LL108: if((int)_tmp17C != 3)goto _LL10A;_LL109: return 4;_LL10A: if((int)_tmp17C != 4)
goto _LL10C;_LL10B: return 5;_LL10C: if(_tmp17C <= (void*)17)goto _LL10E;if(*((int*)
_tmp17C)!= 1)goto _LL10E;_LL10D: return 6;_LL10E: if((int)_tmp17C != 5)goto _LL110;
_LL10F: return 7;_LL110: if(_tmp17C <= (void*)17)goto _LL112;if(*((int*)_tmp17C)!= 2)
goto _LL112;_LL111: return 8;_LL112: if((int)_tmp17C != 6)goto _LL114;_LL113: return 9;
_LL114: if((int)_tmp17C != 7)goto _LL116;_LL115: return 10;_LL116: if((int)_tmp17C != 8)
goto _LL118;_LL117: return 11;_LL118: if((int)_tmp17C != 9)goto _LL11A;_LL119: return 12;
_LL11A: if((int)_tmp17C != 10)goto _LL11C;_LL11B: return 13;_LL11C: if((int)_tmp17C != 
11)goto _LL11E;_LL11D: return 14;_LL11E: if((int)_tmp17C != 12)goto _LL120;_LL11F:
return 15;_LL120: if((int)_tmp17C != 13)goto _LL122;_LL121: return 16;_LL122: if((int)
_tmp17C != 14)goto _LL124;_LL123: return 17;_LL124: if((int)_tmp17C != 15)goto _LL126;
_LL125: return 18;_LL126: if(_tmp17C <= (void*)17)goto _LL12A;if(*((int*)_tmp17C)!= 3)
goto _LL128;_LL127: return 19;_LL128: if(*((int*)_tmp17C)!= 4)goto _LL12A;_LL129:
return 20;_LL12A:;_LL12B: return 21;_LLFF:;}static int Cyc_Tcutil_attribute_cmp(void*
att1,void*att2);static int Cyc_Tcutil_attribute_cmp(void*att1,void*att2){struct
_tuple0 _tmpC38;struct _tuple0 _tmp17E=(_tmpC38.f1=att1,((_tmpC38.f2=att2,_tmpC38)));
void*_tmp17F;int _tmp180;void*_tmp181;int _tmp182;void*_tmp183;int _tmp184;void*
_tmp185;int _tmp186;void*_tmp187;int _tmp188;void*_tmp189;int _tmp18A;void*_tmp18B;
struct _dyneither_ptr _tmp18C;void*_tmp18D;struct _dyneither_ptr _tmp18E;void*
_tmp18F;void*_tmp190;int _tmp191;int _tmp192;void*_tmp193;void*_tmp194;int _tmp195;
int _tmp196;_LL12D: _tmp17F=_tmp17E.f1;if(_tmp17F <= (void*)17)goto _LL12F;if(*((int*)
_tmp17F)!= 0)goto _LL12F;_tmp180=((struct Cyc_Absyn_Regparm_att_struct*)_tmp17F)->f1;
_tmp181=_tmp17E.f2;if(_tmp181 <= (void*)17)goto _LL12F;if(*((int*)_tmp181)!= 0)
goto _LL12F;_tmp182=((struct Cyc_Absyn_Regparm_att_struct*)_tmp181)->f1;_LL12E:
_tmp184=_tmp180;_tmp186=_tmp182;goto _LL130;_LL12F: _tmp183=_tmp17E.f1;if(_tmp183
<= (void*)17)goto _LL131;if(*((int*)_tmp183)!= 4)goto _LL131;_tmp184=((struct Cyc_Absyn_Initializes_att_struct*)
_tmp183)->f1;_tmp185=_tmp17E.f2;if(_tmp185 <= (void*)17)goto _LL131;if(*((int*)
_tmp185)!= 4)goto _LL131;_tmp186=((struct Cyc_Absyn_Initializes_att_struct*)
_tmp185)->f1;_LL130: _tmp188=_tmp184;_tmp18A=_tmp186;goto _LL132;_LL131: _tmp187=
_tmp17E.f1;if(_tmp187 <= (void*)17)goto _LL133;if(*((int*)_tmp187)!= 1)goto _LL133;
_tmp188=((struct Cyc_Absyn_Aligned_att_struct*)_tmp187)->f1;_tmp189=_tmp17E.f2;
if(_tmp189 <= (void*)17)goto _LL133;if(*((int*)_tmp189)!= 1)goto _LL133;_tmp18A=((
struct Cyc_Absyn_Aligned_att_struct*)_tmp189)->f1;_LL132: return Cyc_Core_intcmp(
_tmp188,_tmp18A);_LL133: _tmp18B=_tmp17E.f1;if(_tmp18B <= (void*)17)goto _LL135;if(*((
int*)_tmp18B)!= 2)goto _LL135;_tmp18C=((struct Cyc_Absyn_Section_att_struct*)
_tmp18B)->f1;_tmp18D=_tmp17E.f2;if(_tmp18D <= (void*)17)goto _LL135;if(*((int*)
_tmp18D)!= 2)goto _LL135;_tmp18E=((struct Cyc_Absyn_Section_att_struct*)_tmp18D)->f1;
_LL134: return Cyc_strcmp((struct _dyneither_ptr)_tmp18C,(struct _dyneither_ptr)
_tmp18E);_LL135: _tmp18F=_tmp17E.f1;if(_tmp18F <= (void*)17)goto _LL137;if(*((int*)
_tmp18F)!= 3)goto _LL137;_tmp190=(void*)((struct Cyc_Absyn_Format_att_struct*)
_tmp18F)->f1;_tmp191=((struct Cyc_Absyn_Format_att_struct*)_tmp18F)->f2;_tmp192=((
struct Cyc_Absyn_Format_att_struct*)_tmp18F)->f3;_tmp193=_tmp17E.f2;if(_tmp193 <= (
void*)17)goto _LL137;if(*((int*)_tmp193)!= 3)goto _LL137;_tmp194=(void*)((struct
Cyc_Absyn_Format_att_struct*)_tmp193)->f1;_tmp195=((struct Cyc_Absyn_Format_att_struct*)
_tmp193)->f2;_tmp196=((struct Cyc_Absyn_Format_att_struct*)_tmp193)->f3;_LL136: {
int _tmp197=Cyc_Core_intcmp((int)((unsigned int)_tmp190),(int)((unsigned int)
_tmp194));if(_tmp197 != 0)return _tmp197;{int _tmp198=Cyc_Core_intcmp(_tmp191,
_tmp195);if(_tmp198 != 0)return _tmp198;return Cyc_Core_intcmp(_tmp192,_tmp196);}}
_LL137:;_LL138: return Cyc_Core_intcmp(Cyc_Tcutil_attribute_case_number(att1),Cyc_Tcutil_attribute_case_number(
att2));_LL12C:;}static int Cyc_Tcutil_equal_att(void*a1,void*a2);static int Cyc_Tcutil_equal_att(
void*a1,void*a2){return Cyc_Tcutil_attribute_cmp(a1,a2)== 0;}int Cyc_Tcutil_same_atts(
struct Cyc_List_List*a1,struct Cyc_List_List*a2);int Cyc_Tcutil_same_atts(struct Cyc_List_List*
a1,struct Cyc_List_List*a2){{struct Cyc_List_List*a=a1;for(0;a != 0;a=a->tl){if(!
Cyc_List_exists_c(Cyc_Tcutil_equal_att,(void*)a->hd,a2))return 0;}}{struct Cyc_List_List*
a=a2;for(0;a != 0;a=a->tl){if(!Cyc_List_exists_c(Cyc_Tcutil_equal_att,(void*)a->hd,
a1))return 0;}}return 1;}static void*Cyc_Tcutil_rgns_of(void*t);static void*Cyc_Tcutil_rgns_of_field(
struct Cyc_Absyn_Aggrfield*af);static void*Cyc_Tcutil_rgns_of_field(struct Cyc_Absyn_Aggrfield*
af){return Cyc_Tcutil_rgns_of(af->type);}static struct _tuple14*Cyc_Tcutil_region_free_subst(
struct Cyc_Absyn_Tvar*tv);static struct _tuple14*Cyc_Tcutil_region_free_subst(
struct Cyc_Absyn_Tvar*tv){void*t;{void*_tmp199=Cyc_Tcutil_tvar_kind(tv);_LL13A:
if((int)_tmp199 != 4)goto _LL13C;_LL13B: t=(void*)3;goto _LL139;_LL13C: if((int)
_tmp199 != 3)goto _LL13E;_LL13D: t=(void*)2;goto _LL139;_LL13E: if((int)_tmp199 != 6)
goto _LL140;_LL13F: t=Cyc_Absyn_empty_effect;goto _LL139;_LL140: if((int)_tmp199 != 7)
goto _LL142;_LL141:{struct Cyc_Absyn_ValueofType_struct _tmpC3B;struct Cyc_Absyn_ValueofType_struct*
_tmpC3A;t=(void*)((_tmpC3A=_cycalloc(sizeof(*_tmpC3A)),((_tmpC3A[0]=((_tmpC3B.tag=
17,((_tmpC3B.f1=Cyc_Absyn_uint_exp(0,0),_tmpC3B)))),_tmpC3A))));}goto _LL139;
_LL142:;_LL143: t=Cyc_Absyn_sint_typ;goto _LL139;_LL139:;}{struct _tuple14*_tmpC3C;
return(_tmpC3C=_cycalloc(sizeof(*_tmpC3C)),((_tmpC3C->f1=tv,((_tmpC3C->f2=t,
_tmpC3C)))));}}static void*Cyc_Tcutil_rgns_of(void*t);static void*Cyc_Tcutil_rgns_of(
void*t){void*_tmp19D=Cyc_Tcutil_compress(t);void*_tmp19E;void*_tmp19F;void*
_tmp1A0;struct Cyc_Absyn_DatatypeInfo _tmp1A1;struct Cyc_List_List*_tmp1A2;struct
Cyc_Core_Opt*_tmp1A3;struct Cyc_Absyn_PtrInfo _tmp1A4;void*_tmp1A5;struct Cyc_Absyn_PtrAtts
_tmp1A6;void*_tmp1A7;struct Cyc_Absyn_ArrayInfo _tmp1A8;void*_tmp1A9;struct Cyc_List_List*
_tmp1AA;struct Cyc_Absyn_DatatypeFieldInfo _tmp1AB;struct Cyc_List_List*_tmp1AC;
struct Cyc_Absyn_AggrInfo _tmp1AD;struct Cyc_List_List*_tmp1AE;struct Cyc_List_List*
_tmp1AF;struct Cyc_Absyn_FnInfo _tmp1B0;struct Cyc_List_List*_tmp1B1;struct Cyc_Core_Opt*
_tmp1B2;void*_tmp1B3;struct Cyc_List_List*_tmp1B4;struct Cyc_Absyn_VarargInfo*
_tmp1B5;struct Cyc_List_List*_tmp1B6;void*_tmp1B7;struct Cyc_List_List*_tmp1B8;
_LL145: if((int)_tmp19D != 0)goto _LL147;_LL146: goto _LL148;_LL147: if((int)_tmp19D != 
1)goto _LL149;_LL148: goto _LL14A;_LL149: if(_tmp19D <= (void*)4)goto _LL16D;if(*((int*)
_tmp19D)!= 6)goto _LL14B;_LL14A: goto _LL14C;_LL14B: if(*((int*)_tmp19D)!= 12)goto
_LL14D;_LL14C: goto _LL14E;_LL14D: if(*((int*)_tmp19D)!= 13)goto _LL14F;_LL14E: goto
_LL150;_LL14F: if(*((int*)_tmp19D)!= 17)goto _LL151;_LL150: goto _LL152;_LL151: if(*((
int*)_tmp19D)!= 5)goto _LL153;_LL152: return Cyc_Absyn_empty_effect;_LL153: if(*((
int*)_tmp19D)!= 0)goto _LL155;_LL154: goto _LL156;_LL155: if(*((int*)_tmp19D)!= 1)
goto _LL157;_LL156: {void*_tmp1B9=Cyc_Tcutil_typ_kind(t);_LL17A: if((int)_tmp1B9 != 
3)goto _LL17C;_LL17B: goto _LL17D;_LL17C: if((int)_tmp1B9 != 4)goto _LL17E;_LL17D: goto
_LL17F;_LL17E: if((int)_tmp1B9 != 5)goto _LL180;_LL17F: {struct Cyc_Absyn_AccessEff_struct
_tmpC3F;struct Cyc_Absyn_AccessEff_struct*_tmpC3E;return(void*)((_tmpC3E=
_cycalloc(sizeof(*_tmpC3E)),((_tmpC3E[0]=((_tmpC3F.tag=19,((_tmpC3F.f1=(void*)t,
_tmpC3F)))),_tmpC3E))));}_LL180: if((int)_tmp1B9 != 6)goto _LL182;_LL181: return t;
_LL182: if((int)_tmp1B9 != 7)goto _LL184;_LL183: return Cyc_Absyn_empty_effect;_LL184:;
_LL185: {struct Cyc_Absyn_RgnsEff_struct _tmpC42;struct Cyc_Absyn_RgnsEff_struct*
_tmpC41;return(void*)((_tmpC41=_cycalloc(sizeof(*_tmpC41)),((_tmpC41[0]=((
_tmpC42.tag=21,((_tmpC42.f1=(void*)t,_tmpC42)))),_tmpC41))));}_LL179:;}_LL157:
if(*((int*)_tmp19D)!= 14)goto _LL159;_tmp19E=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp19D)->f1;_LL158: {struct Cyc_Absyn_AccessEff_struct _tmpC45;struct Cyc_Absyn_AccessEff_struct*
_tmpC44;return(void*)((_tmpC44=_cycalloc(sizeof(*_tmpC44)),((_tmpC44[0]=((
_tmpC45.tag=19,((_tmpC45.f1=(void*)_tmp19E,_tmpC45)))),_tmpC44))));}_LL159: if(*((
int*)_tmp19D)!= 15)goto _LL15B;_tmp19F=(void*)((struct Cyc_Absyn_DynRgnType_struct*)
_tmp19D)->f1;_tmp1A0=(void*)((struct Cyc_Absyn_DynRgnType_struct*)_tmp19D)->f2;
_LL15A: {struct Cyc_Absyn_AccessEff_struct _tmpC48;struct Cyc_Absyn_AccessEff_struct*
_tmpC47;return(void*)((_tmpC47=_cycalloc(sizeof(*_tmpC47)),((_tmpC47[0]=((
_tmpC48.tag=19,((_tmpC48.f1=(void*)_tmp1A0,_tmpC48)))),_tmpC47))));}_LL15B: if(*((
int*)_tmp19D)!= 2)goto _LL15D;_tmp1A1=((struct Cyc_Absyn_DatatypeType_struct*)
_tmp19D)->f1;_tmp1A2=_tmp1A1.targs;_tmp1A3=_tmp1A1.rgn;_LL15C: {struct Cyc_List_List*
ts=Cyc_List_map(Cyc_Tcutil_rgns_of,_tmp1A2);if((unsigned int)_tmp1A3){struct Cyc_Absyn_AccessEff_struct*
_tmpC4E;struct Cyc_Absyn_AccessEff_struct _tmpC4D;struct Cyc_List_List*_tmpC4C;ts=((
_tmpC4C=_cycalloc(sizeof(*_tmpC4C)),((_tmpC4C->hd=(void*)((void*)((_tmpC4E=
_cycalloc(sizeof(*_tmpC4E)),((_tmpC4E[0]=((_tmpC4D.tag=19,((_tmpC4D.f1=(void*)((
void*)_tmp1A3->v),_tmpC4D)))),_tmpC4E))))),((_tmpC4C->tl=ts,_tmpC4C))))));}{
struct Cyc_Absyn_JoinEff_struct _tmpC51;struct Cyc_Absyn_JoinEff_struct*_tmpC50;
return Cyc_Tcutil_normalize_effect((void*)((_tmpC50=_cycalloc(sizeof(*_tmpC50)),((
_tmpC50[0]=((_tmpC51.tag=20,((_tmpC51.f1=ts,_tmpC51)))),_tmpC50)))));}}_LL15D:
if(*((int*)_tmp19D)!= 4)goto _LL15F;_tmp1A4=((struct Cyc_Absyn_PointerType_struct*)
_tmp19D)->f1;_tmp1A5=_tmp1A4.elt_typ;_tmp1A6=_tmp1A4.ptr_atts;_tmp1A7=_tmp1A6.rgn;
_LL15E: {struct Cyc_Absyn_JoinEff_struct _tmpC60;struct Cyc_Absyn_AccessEff_struct*
_tmpC5F;struct Cyc_Absyn_AccessEff_struct _tmpC5E;void*_tmpC5D[2];struct Cyc_Absyn_JoinEff_struct*
_tmpC5C;return Cyc_Tcutil_normalize_effect((void*)((_tmpC5C=_cycalloc(sizeof(*
_tmpC5C)),((_tmpC5C[0]=((_tmpC60.tag=20,((_tmpC60.f1=((_tmpC5D[1]=Cyc_Tcutil_rgns_of(
_tmp1A5),((_tmpC5D[0]=(void*)((_tmpC5F=_cycalloc(sizeof(*_tmpC5F)),((_tmpC5F[0]=((
_tmpC5E.tag=19,((_tmpC5E.f1=(void*)_tmp1A7,_tmpC5E)))),_tmpC5F)))),Cyc_List_list(
_tag_dyneither(_tmpC5D,sizeof(void*),2)))))),_tmpC60)))),_tmpC5C)))));}_LL15F:
if(*((int*)_tmp19D)!= 7)goto _LL161;_tmp1A8=((struct Cyc_Absyn_ArrayType_struct*)
_tmp19D)->f1;_tmp1A9=_tmp1A8.elt_type;_LL160: return Cyc_Tcutil_normalize_effect(
Cyc_Tcutil_rgns_of(_tmp1A9));_LL161: if(*((int*)_tmp19D)!= 9)goto _LL163;_tmp1AA=((
struct Cyc_Absyn_TupleType_struct*)_tmp19D)->f1;_LL162: {struct Cyc_List_List*
_tmp1CC=0;for(0;_tmp1AA != 0;_tmp1AA=_tmp1AA->tl){struct Cyc_List_List*_tmpC61;
_tmp1CC=((_tmpC61=_cycalloc(sizeof(*_tmpC61)),((_tmpC61->hd=(void*)(*((struct
_tuple11*)_tmp1AA->hd)).f2,((_tmpC61->tl=_tmp1CC,_tmpC61))))));}_tmp1AC=_tmp1CC;
goto _LL164;}_LL163: if(*((int*)_tmp19D)!= 3)goto _LL165;_tmp1AB=((struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp19D)->f1;_tmp1AC=_tmp1AB.targs;_LL164: _tmp1AE=_tmp1AC;goto _LL166;_LL165: if(*((
int*)_tmp19D)!= 10)goto _LL167;_tmp1AD=((struct Cyc_Absyn_AggrType_struct*)_tmp19D)->f1;
_tmp1AE=_tmp1AD.targs;_LL166: {struct Cyc_Absyn_JoinEff_struct _tmpC64;struct Cyc_Absyn_JoinEff_struct*
_tmpC63;return Cyc_Tcutil_normalize_effect((void*)((_tmpC63=_cycalloc(sizeof(*
_tmpC63)),((_tmpC63[0]=((_tmpC64.tag=20,((_tmpC64.f1=Cyc_List_map(Cyc_Tcutil_rgns_of,
_tmp1AE),_tmpC64)))),_tmpC63)))));}_LL167: if(*((int*)_tmp19D)!= 11)goto _LL169;
_tmp1AF=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp19D)->f2;_LL168: {struct Cyc_Absyn_JoinEff_struct
_tmpC67;struct Cyc_Absyn_JoinEff_struct*_tmpC66;return Cyc_Tcutil_normalize_effect((
void*)((_tmpC66=_cycalloc(sizeof(*_tmpC66)),((_tmpC66[0]=((_tmpC67.tag=20,((
_tmpC67.f1=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Aggrfield*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of_field,_tmp1AF),_tmpC67)))),
_tmpC66)))));}_LL169: if(*((int*)_tmp19D)!= 18)goto _LL16B;_LL16A: return Cyc_Absyn_empty_effect;
_LL16B: if(*((int*)_tmp19D)!= 8)goto _LL16D;_tmp1B0=((struct Cyc_Absyn_FnType_struct*)
_tmp19D)->f1;_tmp1B1=_tmp1B0.tvars;_tmp1B2=_tmp1B0.effect;_tmp1B3=_tmp1B0.ret_typ;
_tmp1B4=_tmp1B0.args;_tmp1B5=_tmp1B0.cyc_varargs;_tmp1B6=_tmp1B0.rgn_po;_LL16C: {
void*_tmp1D2=Cyc_Tcutil_substitute(((struct Cyc_List_List*(*)(struct _tuple14*(*f)(
struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_region_free_subst,
_tmp1B1),(void*)((struct Cyc_Core_Opt*)_check_null(_tmp1B2))->v);return Cyc_Tcutil_normalize_effect(
_tmp1D2);}_LL16D: if((int)_tmp19D != 3)goto _LL16F;_LL16E: goto _LL170;_LL16F: if((int)
_tmp19D != 2)goto _LL171;_LL170: return Cyc_Absyn_empty_effect;_LL171: if(_tmp19D <= (
void*)4)goto _LL173;if(*((int*)_tmp19D)!= 19)goto _LL173;_LL172: goto _LL174;_LL173:
if(_tmp19D <= (void*)4)goto _LL175;if(*((int*)_tmp19D)!= 20)goto _LL175;_LL174:
return t;_LL175: if(_tmp19D <= (void*)4)goto _LL177;if(*((int*)_tmp19D)!= 21)goto
_LL177;_tmp1B7=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp19D)->f1;_LL176:
return Cyc_Tcutil_rgns_of(_tmp1B7);_LL177: if(_tmp19D <= (void*)4)goto _LL144;if(*((
int*)_tmp19D)!= 16)goto _LL144;_tmp1B8=((struct Cyc_Absyn_TypedefType_struct*)
_tmp19D)->f2;_LL178: {struct Cyc_Absyn_JoinEff_struct _tmpC6A;struct Cyc_Absyn_JoinEff_struct*
_tmpC69;return Cyc_Tcutil_normalize_effect((void*)((_tmpC69=_cycalloc(sizeof(*
_tmpC69)),((_tmpC69[0]=((_tmpC6A.tag=20,((_tmpC6A.f1=Cyc_List_map(Cyc_Tcutil_rgns_of,
_tmp1B8),_tmpC6A)))),_tmpC69)))));}_LL144:;}void*Cyc_Tcutil_normalize_effect(
void*e);void*Cyc_Tcutil_normalize_effect(void*e){e=Cyc_Tcutil_compress(e);{void*
_tmp1D5=e;struct Cyc_List_List*_tmp1D6;struct Cyc_List_List**_tmp1D7;void*_tmp1D8;
_LL187: if(_tmp1D5 <= (void*)4)goto _LL18B;if(*((int*)_tmp1D5)!= 20)goto _LL189;
_tmp1D6=((struct Cyc_Absyn_JoinEff_struct*)_tmp1D5)->f1;_tmp1D7=(struct Cyc_List_List**)&((
struct Cyc_Absyn_JoinEff_struct*)_tmp1D5)->f1;_LL188: {int redo_join=0;{struct Cyc_List_List*
effs=*_tmp1D7;for(0;effs != 0;effs=effs->tl){void*_tmp1D9=(void*)effs->hd;effs->hd=(
void*)Cyc_Tcutil_compress(Cyc_Tcutil_normalize_effect(_tmp1D9));{void*_tmp1DA=(
void*)effs->hd;void*_tmp1DB;_LL18E: if(_tmp1DA <= (void*)4)goto _LL192;if(*((int*)
_tmp1DA)!= 20)goto _LL190;_LL18F: goto _LL191;_LL190: if(*((int*)_tmp1DA)!= 19)goto
_LL192;_tmp1DB=(void*)((struct Cyc_Absyn_AccessEff_struct*)_tmp1DA)->f1;if((int)
_tmp1DB != 2)goto _LL192;_LL191: redo_join=1;goto _LL18D;_LL192:;_LL193: goto _LL18D;
_LL18D:;}}}if(!redo_join)return e;{struct Cyc_List_List*effects=0;{struct Cyc_List_List*
effs=*_tmp1D7;for(0;effs != 0;effs=effs->tl){void*_tmp1DC=Cyc_Tcutil_compress((
void*)effs->hd);struct Cyc_List_List*_tmp1DD;void*_tmp1DE;_LL195: if(_tmp1DC <= (
void*)4)goto _LL199;if(*((int*)_tmp1DC)!= 20)goto _LL197;_tmp1DD=((struct Cyc_Absyn_JoinEff_struct*)
_tmp1DC)->f1;_LL196: effects=Cyc_List_revappend(_tmp1DD,effects);goto _LL194;
_LL197: if(*((int*)_tmp1DC)!= 19)goto _LL199;_tmp1DE=(void*)((struct Cyc_Absyn_AccessEff_struct*)
_tmp1DC)->f1;if((int)_tmp1DE != 2)goto _LL199;_LL198: goto _LL194;_LL199:;_LL19A:{
struct Cyc_List_List*_tmpC6B;effects=((_tmpC6B=_cycalloc(sizeof(*_tmpC6B)),((
_tmpC6B->hd=(void*)_tmp1DC,((_tmpC6B->tl=effects,_tmpC6B))))));}goto _LL194;
_LL194:;}}*_tmp1D7=Cyc_List_imp_rev(effects);return e;}}_LL189: if(*((int*)_tmp1D5)
!= 21)goto _LL18B;_tmp1D8=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp1D5)->f1;
_LL18A: {void*_tmp1E0=Cyc_Tcutil_compress(_tmp1D8);_LL19C: if(_tmp1E0 <= (void*)4)
goto _LL1A0;if(*((int*)_tmp1E0)!= 0)goto _LL19E;_LL19D: goto _LL19F;_LL19E: if(*((int*)
_tmp1E0)!= 1)goto _LL1A0;_LL19F: return e;_LL1A0:;_LL1A1: return Cyc_Tcutil_rgns_of(
_tmp1D8);_LL19B:;}_LL18B:;_LL18C: return e;_LL186:;}}static void*Cyc_Tcutil_dummy_fntype(
void*eff);static void*Cyc_Tcutil_dummy_fntype(void*eff){struct Cyc_Absyn_FnType_struct
_tmpC75;struct Cyc_Core_Opt*_tmpC74;struct Cyc_Absyn_FnInfo _tmpC73;struct Cyc_Absyn_FnType_struct*
_tmpC72;struct Cyc_Absyn_FnType_struct*_tmp1E1=(_tmpC72=_cycalloc(sizeof(*_tmpC72)),((
_tmpC72[0]=((_tmpC75.tag=8,((_tmpC75.f1=((_tmpC73.tvars=0,((_tmpC73.effect=((
_tmpC74=_cycalloc(sizeof(*_tmpC74)),((_tmpC74->v=(void*)eff,_tmpC74)))),((
_tmpC73.ret_typ=(void*)0,((_tmpC73.args=0,((_tmpC73.c_varargs=0,((_tmpC73.cyc_varargs=
0,((_tmpC73.rgn_po=0,((_tmpC73.attributes=0,_tmpC73)))))))))))))))),_tmpC75)))),
_tmpC72)));return Cyc_Absyn_atb_typ((void*)_tmp1E1,(void*)2,Cyc_Absyn_empty_tqual(
0),Cyc_Absyn_bounds_one,Cyc_Absyn_false_conref);}int Cyc_Tcutil_region_in_effect(
int constrain,void*r,void*e);int Cyc_Tcutil_region_in_effect(int constrain,void*r,
void*e){r=Cyc_Tcutil_compress(r);if(r == (void*)2  || r == (void*)3)return 1;{void*
_tmp1E6=Cyc_Tcutil_compress(e);void*_tmp1E7;struct Cyc_List_List*_tmp1E8;void*
_tmp1E9;struct Cyc_Core_Opt*_tmp1EA;struct Cyc_Core_Opt*_tmp1EB;struct Cyc_Core_Opt**
_tmp1EC;struct Cyc_Core_Opt*_tmp1ED;_LL1A3: if(_tmp1E6 <= (void*)4)goto _LL1AB;if(*((
int*)_tmp1E6)!= 19)goto _LL1A5;_tmp1E7=(void*)((struct Cyc_Absyn_AccessEff_struct*)
_tmp1E6)->f1;_LL1A4: if(constrain)return Cyc_Tcutil_unify(r,_tmp1E7);_tmp1E7=Cyc_Tcutil_compress(
_tmp1E7);if(r == _tmp1E7)return 1;{struct _tuple0 _tmpC76;struct _tuple0 _tmp1EF=(
_tmpC76.f1=r,((_tmpC76.f2=_tmp1E7,_tmpC76)));void*_tmp1F0;struct Cyc_Absyn_Tvar*
_tmp1F1;void*_tmp1F2;struct Cyc_Absyn_Tvar*_tmp1F3;_LL1AE: _tmp1F0=_tmp1EF.f1;if(
_tmp1F0 <= (void*)4)goto _LL1B0;if(*((int*)_tmp1F0)!= 1)goto _LL1B0;_tmp1F1=((
struct Cyc_Absyn_VarType_struct*)_tmp1F0)->f1;_tmp1F2=_tmp1EF.f2;if(_tmp1F2 <= (
void*)4)goto _LL1B0;if(*((int*)_tmp1F2)!= 1)goto _LL1B0;_tmp1F3=((struct Cyc_Absyn_VarType_struct*)
_tmp1F2)->f1;_LL1AF: return Cyc_Absyn_tvar_cmp(_tmp1F1,_tmp1F3)== 0;_LL1B0:;_LL1B1:
return 0;_LL1AD:;}_LL1A5: if(*((int*)_tmp1E6)!= 20)goto _LL1A7;_tmp1E8=((struct Cyc_Absyn_JoinEff_struct*)
_tmp1E6)->f1;_LL1A6: for(0;_tmp1E8 != 0;_tmp1E8=_tmp1E8->tl){if(Cyc_Tcutil_region_in_effect(
constrain,r,(void*)_tmp1E8->hd))return 1;}return 0;_LL1A7: if(*((int*)_tmp1E6)!= 21)
goto _LL1A9;_tmp1E9=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp1E6)->f1;_LL1A8: {
void*_tmp1F4=Cyc_Tcutil_rgns_of(_tmp1E9);void*_tmp1F5;_LL1B3: if(_tmp1F4 <= (void*)
4)goto _LL1B5;if(*((int*)_tmp1F4)!= 21)goto _LL1B5;_tmp1F5=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp1F4)->f1;_LL1B4: if(!constrain)return 0;{void*_tmp1F6=Cyc_Tcutil_compress(
_tmp1F5);struct Cyc_Core_Opt*_tmp1F7;struct Cyc_Core_Opt*_tmp1F8;struct Cyc_Core_Opt**
_tmp1F9;struct Cyc_Core_Opt*_tmp1FA;_LL1B8: if(_tmp1F6 <= (void*)4)goto _LL1BA;if(*((
int*)_tmp1F6)!= 0)goto _LL1BA;_tmp1F7=((struct Cyc_Absyn_Evar_struct*)_tmp1F6)->f1;
_tmp1F8=((struct Cyc_Absyn_Evar_struct*)_tmp1F6)->f2;_tmp1F9=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Evar_struct*)_tmp1F6)->f2;_tmp1FA=((struct Cyc_Absyn_Evar_struct*)
_tmp1F6)->f4;_LL1B9: {void*_tmp1FB=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,
_tmp1FA);Cyc_Tcutil_occurs(_tmp1FB,Cyc_Core_heap_region,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp1FA))->v,r);{struct Cyc_Absyn_JoinEff_struct
_tmpC85;struct Cyc_Absyn_AccessEff_struct*_tmpC84;struct Cyc_Absyn_AccessEff_struct
_tmpC83;void*_tmpC82[2];struct Cyc_Absyn_JoinEff_struct*_tmpC81;void*_tmp1FC=Cyc_Tcutil_dummy_fntype((
void*)((_tmpC81=_cycalloc(sizeof(*_tmpC81)),((_tmpC81[0]=((_tmpC85.tag=20,((
_tmpC85.f1=((_tmpC82[1]=(void*)((_tmpC84=_cycalloc(sizeof(*_tmpC84)),((_tmpC84[0]=((
_tmpC83.tag=19,((_tmpC83.f1=(void*)r,_tmpC83)))),_tmpC84)))),((_tmpC82[0]=
_tmp1FB,Cyc_List_list(_tag_dyneither(_tmpC82,sizeof(void*),2)))))),_tmpC85)))),
_tmpC81)))));{struct Cyc_Core_Opt*_tmpC86;*_tmp1F9=((_tmpC86=_cycalloc(sizeof(*
_tmpC86)),((_tmpC86->v=(void*)_tmp1FC,_tmpC86))));}return 1;}}_LL1BA:;_LL1BB:
return 0;_LL1B7:;}_LL1B5:;_LL1B6: return Cyc_Tcutil_region_in_effect(constrain,r,
_tmp1F4);_LL1B2:;}_LL1A9: if(*((int*)_tmp1E6)!= 0)goto _LL1AB;_tmp1EA=((struct Cyc_Absyn_Evar_struct*)
_tmp1E6)->f1;_tmp1EB=((struct Cyc_Absyn_Evar_struct*)_tmp1E6)->f2;_tmp1EC=(struct
Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)_tmp1E6)->f2;_tmp1ED=((struct Cyc_Absyn_Evar_struct*)
_tmp1E6)->f4;_LL1AA: if(_tmp1EA == 0  || (void*)_tmp1EA->v != (void*)6){const char*
_tmpC89;void*_tmpC88;(_tmpC88=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpC89="effect evar has wrong kind",
_tag_dyneither(_tmpC89,sizeof(char),27))),_tag_dyneither(_tmpC88,sizeof(void*),0)));}
if(!constrain)return 0;{void*_tmp205=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,
_tmp1ED);Cyc_Tcutil_occurs(_tmp205,Cyc_Core_heap_region,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp1ED))->v,r);{struct Cyc_Absyn_JoinEff_struct
_tmpC9E;struct Cyc_List_List*_tmpC9D;struct Cyc_Absyn_AccessEff_struct _tmpC9C;
struct Cyc_Absyn_AccessEff_struct*_tmpC9B;struct Cyc_List_List*_tmpC9A;struct Cyc_Absyn_JoinEff_struct*
_tmpC99;struct Cyc_Absyn_JoinEff_struct*_tmp206=(_tmpC99=_cycalloc(sizeof(*
_tmpC99)),((_tmpC99[0]=((_tmpC9E.tag=20,((_tmpC9E.f1=((_tmpC9A=_cycalloc(sizeof(*
_tmpC9A)),((_tmpC9A->hd=(void*)_tmp205,((_tmpC9A->tl=((_tmpC9D=_cycalloc(sizeof(*
_tmpC9D)),((_tmpC9D->hd=(void*)((void*)((_tmpC9B=_cycalloc(sizeof(*_tmpC9B)),((
_tmpC9B[0]=((_tmpC9C.tag=19,((_tmpC9C.f1=(void*)r,_tmpC9C)))),_tmpC9B))))),((
_tmpC9D->tl=0,_tmpC9D)))))),_tmpC9A)))))),_tmpC9E)))),_tmpC99)));{struct Cyc_Core_Opt*
_tmpC9F;*_tmp1EC=((_tmpC9F=_cycalloc(sizeof(*_tmpC9F)),((_tmpC9F->v=(void*)((
void*)_tmp206),_tmpC9F))));}return 1;}}_LL1AB:;_LL1AC: return 0;_LL1A2:;}}static int
Cyc_Tcutil_type_in_effect(int may_constrain_evars,void*t,void*e);static int Cyc_Tcutil_type_in_effect(
int may_constrain_evars,void*t,void*e){t=Cyc_Tcutil_compress(t);{void*_tmp20E=Cyc_Tcutil_normalize_effect(
Cyc_Tcutil_compress(e));struct Cyc_List_List*_tmp20F;void*_tmp210;struct Cyc_Core_Opt*
_tmp211;struct Cyc_Core_Opt*_tmp212;struct Cyc_Core_Opt**_tmp213;struct Cyc_Core_Opt*
_tmp214;_LL1BD: if(_tmp20E <= (void*)4)goto _LL1C5;if(*((int*)_tmp20E)!= 19)goto
_LL1BF;_LL1BE: return 0;_LL1BF: if(*((int*)_tmp20E)!= 20)goto _LL1C1;_tmp20F=((
struct Cyc_Absyn_JoinEff_struct*)_tmp20E)->f1;_LL1C0: for(0;_tmp20F != 0;_tmp20F=
_tmp20F->tl){if(Cyc_Tcutil_type_in_effect(may_constrain_evars,t,(void*)_tmp20F->hd))
return 1;}return 0;_LL1C1: if(*((int*)_tmp20E)!= 21)goto _LL1C3;_tmp210=(void*)((
struct Cyc_Absyn_RgnsEff_struct*)_tmp20E)->f1;_LL1C2: _tmp210=Cyc_Tcutil_compress(
_tmp210);if(t == _tmp210)return 1;if(may_constrain_evars)return Cyc_Tcutil_unify(t,
_tmp210);{void*_tmp215=Cyc_Tcutil_rgns_of(t);void*_tmp216;_LL1C8: if(_tmp215 <= (
void*)4)goto _LL1CA;if(*((int*)_tmp215)!= 21)goto _LL1CA;_tmp216=(void*)((struct
Cyc_Absyn_RgnsEff_struct*)_tmp215)->f1;_LL1C9: {struct _tuple0 _tmpCA0;struct
_tuple0 _tmp218=(_tmpCA0.f1=Cyc_Tcutil_compress(_tmp216),((_tmpCA0.f2=_tmp210,
_tmpCA0)));void*_tmp219;struct Cyc_Absyn_Tvar*_tmp21A;void*_tmp21B;struct Cyc_Absyn_Tvar*
_tmp21C;_LL1CD: _tmp219=_tmp218.f1;if(_tmp219 <= (void*)4)goto _LL1CF;if(*((int*)
_tmp219)!= 1)goto _LL1CF;_tmp21A=((struct Cyc_Absyn_VarType_struct*)_tmp219)->f1;
_tmp21B=_tmp218.f2;if(_tmp21B <= (void*)4)goto _LL1CF;if(*((int*)_tmp21B)!= 1)goto
_LL1CF;_tmp21C=((struct Cyc_Absyn_VarType_struct*)_tmp21B)->f1;_LL1CE: return Cyc_Tcutil_unify(
t,_tmp210);_LL1CF:;_LL1D0: return _tmp216 == _tmp210;_LL1CC:;}_LL1CA:;_LL1CB: return
Cyc_Tcutil_type_in_effect(may_constrain_evars,t,_tmp215);_LL1C7:;}_LL1C3: if(*((
int*)_tmp20E)!= 0)goto _LL1C5;_tmp211=((struct Cyc_Absyn_Evar_struct*)_tmp20E)->f1;
_tmp212=((struct Cyc_Absyn_Evar_struct*)_tmp20E)->f2;_tmp213=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Evar_struct*)_tmp20E)->f2;_tmp214=((struct Cyc_Absyn_Evar_struct*)
_tmp20E)->f4;_LL1C4: if(_tmp211 == 0  || (void*)_tmp211->v != (void*)6){const char*
_tmpCA3;void*_tmpCA2;(_tmpCA2=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpCA3="effect evar has wrong kind",
_tag_dyneither(_tmpCA3,sizeof(char),27))),_tag_dyneither(_tmpCA2,sizeof(void*),0)));}
if(!may_constrain_evars)return 0;{void*_tmp21F=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_ek,_tmp214);Cyc_Tcutil_occurs(_tmp21F,Cyc_Core_heap_region,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp214))->v,t);{struct Cyc_Absyn_JoinEff_struct
_tmpCB8;struct Cyc_List_List*_tmpCB7;struct Cyc_Absyn_RgnsEff_struct _tmpCB6;struct
Cyc_Absyn_RgnsEff_struct*_tmpCB5;struct Cyc_List_List*_tmpCB4;struct Cyc_Absyn_JoinEff_struct*
_tmpCB3;struct Cyc_Absyn_JoinEff_struct*_tmp220=(_tmpCB3=_cycalloc(sizeof(*
_tmpCB3)),((_tmpCB3[0]=((_tmpCB8.tag=20,((_tmpCB8.f1=((_tmpCB4=_cycalloc(sizeof(*
_tmpCB4)),((_tmpCB4->hd=(void*)_tmp21F,((_tmpCB4->tl=((_tmpCB7=_cycalloc(sizeof(*
_tmpCB7)),((_tmpCB7->hd=(void*)((void*)((_tmpCB5=_cycalloc(sizeof(*_tmpCB5)),((
_tmpCB5[0]=((_tmpCB6.tag=21,((_tmpCB6.f1=(void*)t,_tmpCB6)))),_tmpCB5))))),((
_tmpCB7->tl=0,_tmpCB7)))))),_tmpCB4)))))),_tmpCB8)))),_tmpCB3)));{struct Cyc_Core_Opt*
_tmpCB9;*_tmp213=((_tmpCB9=_cycalloc(sizeof(*_tmpCB9)),((_tmpCB9->v=(void*)((
void*)_tmp220),_tmpCB9))));}return 1;}}_LL1C5:;_LL1C6: return 0;_LL1BC:;}}static int
Cyc_Tcutil_variable_in_effect(int may_constrain_evars,struct Cyc_Absyn_Tvar*v,void*
e);static int Cyc_Tcutil_variable_in_effect(int may_constrain_evars,struct Cyc_Absyn_Tvar*
v,void*e){e=Cyc_Tcutil_compress(e);{void*_tmp228=e;struct Cyc_Absyn_Tvar*_tmp229;
struct Cyc_List_List*_tmp22A;void*_tmp22B;struct Cyc_Core_Opt*_tmp22C;struct Cyc_Core_Opt*
_tmp22D;struct Cyc_Core_Opt**_tmp22E;struct Cyc_Core_Opt*_tmp22F;_LL1D2: if(_tmp228
<= (void*)4)goto _LL1DA;if(*((int*)_tmp228)!= 1)goto _LL1D4;_tmp229=((struct Cyc_Absyn_VarType_struct*)
_tmp228)->f1;_LL1D3: return Cyc_Absyn_tvar_cmp(v,_tmp229)== 0;_LL1D4: if(*((int*)
_tmp228)!= 20)goto _LL1D6;_tmp22A=((struct Cyc_Absyn_JoinEff_struct*)_tmp228)->f1;
_LL1D5: for(0;_tmp22A != 0;_tmp22A=_tmp22A->tl){if(Cyc_Tcutil_variable_in_effect(
may_constrain_evars,v,(void*)_tmp22A->hd))return 1;}return 0;_LL1D6: if(*((int*)
_tmp228)!= 21)goto _LL1D8;_tmp22B=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp228)->f1;_LL1D7: {void*_tmp230=Cyc_Tcutil_rgns_of(_tmp22B);void*_tmp231;
_LL1DD: if(_tmp230 <= (void*)4)goto _LL1DF;if(*((int*)_tmp230)!= 21)goto _LL1DF;
_tmp231=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp230)->f1;_LL1DE: if(!
may_constrain_evars)return 0;{void*_tmp232=Cyc_Tcutil_compress(_tmp231);struct Cyc_Core_Opt*
_tmp233;struct Cyc_Core_Opt*_tmp234;struct Cyc_Core_Opt**_tmp235;struct Cyc_Core_Opt*
_tmp236;_LL1E2: if(_tmp232 <= (void*)4)goto _LL1E4;if(*((int*)_tmp232)!= 0)goto
_LL1E4;_tmp233=((struct Cyc_Absyn_Evar_struct*)_tmp232)->f1;_tmp234=((struct Cyc_Absyn_Evar_struct*)
_tmp232)->f2;_tmp235=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)
_tmp232)->f2;_tmp236=((struct Cyc_Absyn_Evar_struct*)_tmp232)->f4;_LL1E3: {void*
_tmp237=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,_tmp236);if(!((
int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*
l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp236))->v,v))return 0;{struct Cyc_Absyn_JoinEff_struct
_tmpCC8;struct Cyc_Absyn_VarType_struct*_tmpCC7;struct Cyc_Absyn_VarType_struct
_tmpCC6;void*_tmpCC5[2];struct Cyc_Absyn_JoinEff_struct*_tmpCC4;void*_tmp238=Cyc_Tcutil_dummy_fntype((
void*)((_tmpCC4=_cycalloc(sizeof(*_tmpCC4)),((_tmpCC4[0]=((_tmpCC8.tag=20,((
_tmpCC8.f1=((_tmpCC5[1]=(void*)((_tmpCC7=_cycalloc(sizeof(*_tmpCC7)),((_tmpCC7[0]=((
_tmpCC6.tag=1,((_tmpCC6.f1=v,_tmpCC6)))),_tmpCC7)))),((_tmpCC5[0]=_tmp237,Cyc_List_list(
_tag_dyneither(_tmpCC5,sizeof(void*),2)))))),_tmpCC8)))),_tmpCC4)))));{struct Cyc_Core_Opt*
_tmpCC9;*_tmp235=((_tmpCC9=_cycalloc(sizeof(*_tmpCC9)),((_tmpCC9->v=(void*)
_tmp238,_tmpCC9))));}return 1;}}_LL1E4:;_LL1E5: return 0;_LL1E1:;}_LL1DF:;_LL1E0:
return Cyc_Tcutil_variable_in_effect(may_constrain_evars,v,_tmp230);_LL1DC:;}
_LL1D8: if(*((int*)_tmp228)!= 0)goto _LL1DA;_tmp22C=((struct Cyc_Absyn_Evar_struct*)
_tmp228)->f1;_tmp22D=((struct Cyc_Absyn_Evar_struct*)_tmp228)->f2;_tmp22E=(struct
Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)_tmp228)->f2;_tmp22F=((struct Cyc_Absyn_Evar_struct*)
_tmp228)->f4;_LL1D9: if(_tmp22C == 0  || (void*)_tmp22C->v != (void*)6){const char*
_tmpCCC;void*_tmpCCB;(_tmpCCB=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpCCC="effect evar has wrong kind",
_tag_dyneither(_tmpCCC,sizeof(char),27))),_tag_dyneither(_tmpCCB,sizeof(void*),0)));}{
void*_tmp241=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,_tmp22F);if(
!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*
l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp22F))->v,v))return 0;{struct Cyc_Absyn_JoinEff_struct
_tmpCE1;struct Cyc_List_List*_tmpCE0;struct Cyc_Absyn_VarType_struct _tmpCDF;struct
Cyc_Absyn_VarType_struct*_tmpCDE;struct Cyc_List_List*_tmpCDD;struct Cyc_Absyn_JoinEff_struct*
_tmpCDC;struct Cyc_Absyn_JoinEff_struct*_tmp242=(_tmpCDC=_cycalloc(sizeof(*
_tmpCDC)),((_tmpCDC[0]=((_tmpCE1.tag=20,((_tmpCE1.f1=((_tmpCDD=_cycalloc(sizeof(*
_tmpCDD)),((_tmpCDD->hd=(void*)_tmp241,((_tmpCDD->tl=((_tmpCE0=_cycalloc(sizeof(*
_tmpCE0)),((_tmpCE0->hd=(void*)((void*)((_tmpCDE=_cycalloc(sizeof(*_tmpCDE)),((
_tmpCDE[0]=((_tmpCDF.tag=1,((_tmpCDF.f1=v,_tmpCDF)))),_tmpCDE))))),((_tmpCE0->tl=
0,_tmpCE0)))))),_tmpCDD)))))),_tmpCE1)))),_tmpCDC)));{struct Cyc_Core_Opt*_tmpCE2;*
_tmp22E=((_tmpCE2=_cycalloc(sizeof(*_tmpCE2)),((_tmpCE2->v=(void*)((void*)
_tmp242),_tmpCE2))));}return 1;}}_LL1DA:;_LL1DB: return 0;_LL1D1:;}}static int Cyc_Tcutil_evar_in_effect(
void*evar,void*e);static int Cyc_Tcutil_evar_in_effect(void*evar,void*e){e=Cyc_Tcutil_compress(
e);{void*_tmp24A=e;struct Cyc_List_List*_tmp24B;void*_tmp24C;_LL1E7: if(_tmp24A <= (
void*)4)goto _LL1ED;if(*((int*)_tmp24A)!= 20)goto _LL1E9;_tmp24B=((struct Cyc_Absyn_JoinEff_struct*)
_tmp24A)->f1;_LL1E8: for(0;_tmp24B != 0;_tmp24B=_tmp24B->tl){if(Cyc_Tcutil_evar_in_effect(
evar,(void*)_tmp24B->hd))return 1;}return 0;_LL1E9: if(*((int*)_tmp24A)!= 21)goto
_LL1EB;_tmp24C=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp24A)->f1;_LL1EA: {
void*_tmp24D=Cyc_Tcutil_rgns_of(_tmp24C);void*_tmp24E;_LL1F0: if(_tmp24D <= (void*)
4)goto _LL1F2;if(*((int*)_tmp24D)!= 21)goto _LL1F2;_tmp24E=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp24D)->f1;_LL1F1: return 0;_LL1F2:;_LL1F3: return Cyc_Tcutil_evar_in_effect(evar,
_tmp24D);_LL1EF:;}_LL1EB: if(*((int*)_tmp24A)!= 0)goto _LL1ED;_LL1EC: return evar == 
e;_LL1ED:;_LL1EE: return 0;_LL1E6:;}}int Cyc_Tcutil_subset_effect(int
may_constrain_evars,void*e1,void*e2);int Cyc_Tcutil_subset_effect(int
may_constrain_evars,void*e1,void*e2){void*_tmp24F=Cyc_Tcutil_compress(e1);struct
Cyc_List_List*_tmp250;void*_tmp251;struct Cyc_Absyn_Tvar*_tmp252;void*_tmp253;
struct Cyc_Core_Opt*_tmp254;struct Cyc_Core_Opt**_tmp255;struct Cyc_Core_Opt*
_tmp256;_LL1F5: if(_tmp24F <= (void*)4)goto _LL1FF;if(*((int*)_tmp24F)!= 20)goto
_LL1F7;_tmp250=((struct Cyc_Absyn_JoinEff_struct*)_tmp24F)->f1;_LL1F6: for(0;
_tmp250 != 0;_tmp250=_tmp250->tl){if(!Cyc_Tcutil_subset_effect(
may_constrain_evars,(void*)_tmp250->hd,e2))return 0;}return 1;_LL1F7: if(*((int*)
_tmp24F)!= 19)goto _LL1F9;_tmp251=(void*)((struct Cyc_Absyn_AccessEff_struct*)
_tmp24F)->f1;_LL1F8: return Cyc_Tcutil_region_in_effect(0,_tmp251,e2) || 
may_constrain_evars  && Cyc_Tcutil_unify(_tmp251,(void*)2);_LL1F9: if(*((int*)
_tmp24F)!= 1)goto _LL1FB;_tmp252=((struct Cyc_Absyn_VarType_struct*)_tmp24F)->f1;
_LL1FA: return Cyc_Tcutil_variable_in_effect(may_constrain_evars,_tmp252,e2);
_LL1FB: if(*((int*)_tmp24F)!= 21)goto _LL1FD;_tmp253=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp24F)->f1;_LL1FC: {void*_tmp257=Cyc_Tcutil_rgns_of(_tmp253);void*_tmp258;
_LL202: if(_tmp257 <= (void*)4)goto _LL204;if(*((int*)_tmp257)!= 21)goto _LL204;
_tmp258=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp257)->f1;_LL203: return Cyc_Tcutil_type_in_effect(
may_constrain_evars,_tmp258,e2) || may_constrain_evars  && Cyc_Tcutil_unify(
_tmp258,Cyc_Absyn_sint_typ);_LL204:;_LL205: return Cyc_Tcutil_subset_effect(
may_constrain_evars,_tmp257,e2);_LL201:;}_LL1FD: if(*((int*)_tmp24F)!= 0)goto
_LL1FF;_tmp254=((struct Cyc_Absyn_Evar_struct*)_tmp24F)->f2;_tmp255=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Evar_struct*)_tmp24F)->f2;_tmp256=((struct Cyc_Absyn_Evar_struct*)
_tmp24F)->f4;_LL1FE: if(!Cyc_Tcutil_evar_in_effect(e1,e2)){struct Cyc_Core_Opt*
_tmpCE3;*_tmp255=((_tmpCE3=_cycalloc(sizeof(*_tmpCE3)),((_tmpCE3->v=(void*)Cyc_Absyn_empty_effect,
_tmpCE3))));}return 1;_LL1FF:;_LL200: {const char*_tmpCE7;void*_tmpCE6[1];struct
Cyc_String_pa_struct _tmpCE5;(_tmpCE5.tag=0,((_tmpCE5.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string(e1)),((_tmpCE6[0]=& _tmpCE5,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpCE7="subset_effect: bad effect: %s",
_tag_dyneither(_tmpCE7,sizeof(char),30))),_tag_dyneither(_tmpCE6,sizeof(void*),1)))))));}
_LL1F4:;}static int Cyc_Tcutil_unify_effect(void*e1,void*e2);static int Cyc_Tcutil_unify_effect(
void*e1,void*e2){e1=Cyc_Tcutil_normalize_effect(e1);e2=Cyc_Tcutil_normalize_effect(
e2);if(Cyc_Tcutil_subset_effect(0,e1,e2) && Cyc_Tcutil_subset_effect(0,e2,e1))
return 1;if(Cyc_Tcutil_subset_effect(1,e1,e2) && Cyc_Tcutil_subset_effect(1,e2,e1))
return 1;return 0;}static int Cyc_Tcutil_sub_rgnpo(struct Cyc_List_List*rpo1,struct
Cyc_List_List*rpo2);static int Cyc_Tcutil_sub_rgnpo(struct Cyc_List_List*rpo1,
struct Cyc_List_List*rpo2){{struct Cyc_List_List*r1=rpo1;for(0;r1 != 0;r1=r1->tl){
struct _tuple0 _tmp25E;void*_tmp25F;void*_tmp260;struct _tuple0*_tmp25D=(struct
_tuple0*)r1->hd;_tmp25E=*_tmp25D;_tmp25F=_tmp25E.f1;_tmp260=_tmp25E.f2;{int found=
_tmp25F == (void*)2;{struct Cyc_List_List*r2=rpo2;for(0;r2 != 0  && !found;r2=r2->tl){
struct _tuple0 _tmp262;void*_tmp263;void*_tmp264;struct _tuple0*_tmp261=(struct
_tuple0*)r2->hd;_tmp262=*_tmp261;_tmp263=_tmp262.f1;_tmp264=_tmp262.f2;if(Cyc_Tcutil_unify(
_tmp25F,_tmp263) && Cyc_Tcutil_unify(_tmp260,_tmp264)){found=1;break;}}}if(!
found)return 0;}}}return 1;}static int Cyc_Tcutil_same_rgn_po(struct Cyc_List_List*
rpo1,struct Cyc_List_List*rpo2);static int Cyc_Tcutil_same_rgn_po(struct Cyc_List_List*
rpo1,struct Cyc_List_List*rpo2){return Cyc_Tcutil_sub_rgnpo(rpo1,rpo2) && Cyc_Tcutil_sub_rgnpo(
rpo2,rpo1);}struct _tuple17{struct Cyc_Absyn_VarargInfo*f1;struct Cyc_Absyn_VarargInfo*
f2;};struct _tuple18{struct Cyc_Core_Opt*f1;struct Cyc_Core_Opt*f2;};void Cyc_Tcutil_unify_it(
void*t1,void*t2);void Cyc_Tcutil_unify_it(void*t1,void*t2){Cyc_Tcutil_t1_failure=
t1;Cyc_Tcutil_t2_failure=t2;Cyc_Tcutil_failure_reason=(struct _dyneither_ptr)
_tag_dyneither(0,0,0);t1=Cyc_Tcutil_compress(t1);t2=Cyc_Tcutil_compress(t2);if(
t1 == t2)return;{void*_tmp265=t1;struct Cyc_Core_Opt*_tmp266;struct Cyc_Core_Opt*
_tmp267;struct Cyc_Core_Opt**_tmp268;struct Cyc_Core_Opt*_tmp269;_LL207: if(_tmp265
<= (void*)4)goto _LL209;if(*((int*)_tmp265)!= 0)goto _LL209;_tmp266=((struct Cyc_Absyn_Evar_struct*)
_tmp265)->f1;_tmp267=((struct Cyc_Absyn_Evar_struct*)_tmp265)->f2;_tmp268=(struct
Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)_tmp265)->f2;_tmp269=((struct Cyc_Absyn_Evar_struct*)
_tmp265)->f4;_LL208: Cyc_Tcutil_occurs(t1,Cyc_Core_heap_region,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp269))->v,t2);{void*_tmp26A=Cyc_Tcutil_typ_kind(
t2);if(Cyc_Tcutil_kind_leq(_tmp26A,(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp266))->v)){{struct Cyc_Core_Opt*_tmpCE8;*_tmp268=((_tmpCE8=_cycalloc(sizeof(*
_tmpCE8)),((_tmpCE8->v=(void*)t2,_tmpCE8))));}return;}else{{void*_tmp26C=t2;
struct Cyc_Core_Opt*_tmp26D;struct Cyc_Core_Opt**_tmp26E;struct Cyc_Core_Opt*
_tmp26F;struct Cyc_Absyn_PtrInfo _tmp270;_LL20C: if(_tmp26C <= (void*)4)goto _LL210;
if(*((int*)_tmp26C)!= 0)goto _LL20E;_tmp26D=((struct Cyc_Absyn_Evar_struct*)
_tmp26C)->f2;_tmp26E=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)
_tmp26C)->f2;_tmp26F=((struct Cyc_Absyn_Evar_struct*)_tmp26C)->f4;_LL20D: {struct
Cyc_List_List*_tmp271=(struct Cyc_List_List*)_tmp269->v;{struct Cyc_List_List*s2=(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp26F))->v;for(0;s2 != 0;
s2=s2->tl){if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,
_tmp271,(struct Cyc_Absyn_Tvar*)s2->hd)){{const char*_tmpCE9;Cyc_Tcutil_failure_reason=((
_tmpCE9="(type variable would escape scope)",_tag_dyneither(_tmpCE9,sizeof(char),
35)));}(int)_throw((void*)Cyc_Tcutil_Unify);}}}if(Cyc_Tcutil_kind_leq((void*)
_tmp266->v,_tmp26A)){{struct Cyc_Core_Opt*_tmpCEA;*_tmp26E=((_tmpCEA=_cycalloc(
sizeof(*_tmpCEA)),((_tmpCEA->v=(void*)t1,_tmpCEA))));}return;}{const char*_tmpCEB;
Cyc_Tcutil_failure_reason=((_tmpCEB="(kinds are incompatible)",_tag_dyneither(
_tmpCEB,sizeof(char),25)));}goto _LL20B;}_LL20E: if(*((int*)_tmp26C)!= 4)goto
_LL210;_tmp270=((struct Cyc_Absyn_PointerType_struct*)_tmp26C)->f1;if(!((void*)
_tmp266->v == (void*)2))goto _LL210;_LL20F: {union Cyc_Absyn_Constraint*_tmp275=Cyc_Absyn_compress_conref((
_tmp270.ptr_atts).bounds);{union Cyc_Absyn_Constraint*_tmp276=_tmp275;union Cyc_Absyn_Constraint
_tmp277;int _tmp278;_LL213: _tmp277=*_tmp276;if((_tmp277.No_constr).tag != 3)goto
_LL215;_tmp278=(int)(_tmp277.No_constr).val;_LL214:{struct
_union_Constraint_Eq_constr*_tmpCEC;(_tmpCEC=& _tmp275->Eq_constr,((_tmpCEC->tag=
1,_tmpCEC->val=Cyc_Absyn_bounds_one)));}{struct Cyc_Core_Opt*_tmpCED;*_tmp268=((
_tmpCED=_cycalloc(sizeof(*_tmpCED)),((_tmpCED->v=(void*)t2,_tmpCED))));}return;
_LL215:;_LL216: goto _LL212;_LL212:;}goto _LL20B;}_LL210:;_LL211: goto _LL20B;_LL20B:;}{
const char*_tmpCEE;Cyc_Tcutil_failure_reason=((_tmpCEE="(kinds are incompatible)",
_tag_dyneither(_tmpCEE,sizeof(char),25)));}(int)_throw((void*)Cyc_Tcutil_Unify);}}
_LL209:;_LL20A: goto _LL206;_LL206:;}{struct _tuple0 _tmpCEF;struct _tuple0 _tmp27D=(
_tmpCEF.f1=t2,((_tmpCEF.f2=t1,_tmpCEF)));void*_tmp27E;void*_tmp27F;void*_tmp280;
void*_tmp281;struct Cyc_Absyn_Tvar*_tmp282;void*_tmp283;struct Cyc_Absyn_Tvar*
_tmp284;void*_tmp285;struct Cyc_Absyn_AggrInfo _tmp286;union Cyc_Absyn_AggrInfoU
_tmp287;struct Cyc_List_List*_tmp288;void*_tmp289;struct Cyc_Absyn_AggrInfo _tmp28A;
union Cyc_Absyn_AggrInfoU _tmp28B;struct Cyc_List_List*_tmp28C;void*_tmp28D;struct
_tuple2*_tmp28E;void*_tmp28F;struct _tuple2*_tmp290;void*_tmp291;struct Cyc_List_List*
_tmp292;void*_tmp293;struct Cyc_List_List*_tmp294;void*_tmp295;struct Cyc_Absyn_DatatypeInfo
_tmp296;union Cyc_Absyn_DatatypeInfoU _tmp297;struct Cyc_Absyn_Datatypedecl**
_tmp298;struct Cyc_Absyn_Datatypedecl*_tmp299;struct Cyc_List_List*_tmp29A;struct
Cyc_Core_Opt*_tmp29B;void*_tmp29C;struct Cyc_Absyn_DatatypeInfo _tmp29D;union Cyc_Absyn_DatatypeInfoU
_tmp29E;struct Cyc_Absyn_Datatypedecl**_tmp29F;struct Cyc_Absyn_Datatypedecl*
_tmp2A0;struct Cyc_List_List*_tmp2A1;struct Cyc_Core_Opt*_tmp2A2;void*_tmp2A3;
struct Cyc_Absyn_DatatypeFieldInfo _tmp2A4;union Cyc_Absyn_DatatypeFieldInfoU
_tmp2A5;struct _tuple3 _tmp2A6;struct Cyc_Absyn_Datatypedecl*_tmp2A7;struct Cyc_Absyn_Datatypefield*
_tmp2A8;struct Cyc_List_List*_tmp2A9;void*_tmp2AA;struct Cyc_Absyn_DatatypeFieldInfo
_tmp2AB;union Cyc_Absyn_DatatypeFieldInfoU _tmp2AC;struct _tuple3 _tmp2AD;struct Cyc_Absyn_Datatypedecl*
_tmp2AE;struct Cyc_Absyn_Datatypefield*_tmp2AF;struct Cyc_List_List*_tmp2B0;void*
_tmp2B1;struct Cyc_Absyn_PtrInfo _tmp2B2;void*_tmp2B3;struct Cyc_Absyn_Tqual _tmp2B4;
struct Cyc_Absyn_PtrAtts _tmp2B5;void*_tmp2B6;union Cyc_Absyn_Constraint*_tmp2B7;
union Cyc_Absyn_Constraint*_tmp2B8;union Cyc_Absyn_Constraint*_tmp2B9;void*_tmp2BA;
struct Cyc_Absyn_PtrInfo _tmp2BB;void*_tmp2BC;struct Cyc_Absyn_Tqual _tmp2BD;struct
Cyc_Absyn_PtrAtts _tmp2BE;void*_tmp2BF;union Cyc_Absyn_Constraint*_tmp2C0;union Cyc_Absyn_Constraint*
_tmp2C1;union Cyc_Absyn_Constraint*_tmp2C2;void*_tmp2C3;void*_tmp2C4;void*_tmp2C5;
void*_tmp2C6;void*_tmp2C7;void*_tmp2C8;void*_tmp2C9;void*_tmp2CA;void*_tmp2CB;
int _tmp2CC;void*_tmp2CD;int _tmp2CE;void*_tmp2CF;void*_tmp2D0;void*_tmp2D1;void*
_tmp2D2;void*_tmp2D3;struct Cyc_Absyn_Exp*_tmp2D4;void*_tmp2D5;struct Cyc_Absyn_Exp*
_tmp2D6;void*_tmp2D7;struct Cyc_Absyn_ArrayInfo _tmp2D8;void*_tmp2D9;struct Cyc_Absyn_Tqual
_tmp2DA;struct Cyc_Absyn_Exp*_tmp2DB;union Cyc_Absyn_Constraint*_tmp2DC;void*
_tmp2DD;struct Cyc_Absyn_ArrayInfo _tmp2DE;void*_tmp2DF;struct Cyc_Absyn_Tqual
_tmp2E0;struct Cyc_Absyn_Exp*_tmp2E1;union Cyc_Absyn_Constraint*_tmp2E2;void*
_tmp2E3;struct Cyc_Absyn_FnInfo _tmp2E4;struct Cyc_List_List*_tmp2E5;struct Cyc_Core_Opt*
_tmp2E6;void*_tmp2E7;struct Cyc_List_List*_tmp2E8;int _tmp2E9;struct Cyc_Absyn_VarargInfo*
_tmp2EA;struct Cyc_List_List*_tmp2EB;struct Cyc_List_List*_tmp2EC;void*_tmp2ED;
struct Cyc_Absyn_FnInfo _tmp2EE;struct Cyc_List_List*_tmp2EF;struct Cyc_Core_Opt*
_tmp2F0;void*_tmp2F1;struct Cyc_List_List*_tmp2F2;int _tmp2F3;struct Cyc_Absyn_VarargInfo*
_tmp2F4;struct Cyc_List_List*_tmp2F5;struct Cyc_List_List*_tmp2F6;void*_tmp2F7;
struct Cyc_List_List*_tmp2F8;void*_tmp2F9;struct Cyc_List_List*_tmp2FA;void*
_tmp2FB;void*_tmp2FC;struct Cyc_List_List*_tmp2FD;void*_tmp2FE;void*_tmp2FF;
struct Cyc_List_List*_tmp300;void*_tmp301;void*_tmp302;void*_tmp303;void*_tmp304;
void*_tmp305;void*_tmp306;void*_tmp307;void*_tmp308;void*_tmp309;void*_tmp30A;
void*_tmp30B;void*_tmp30C;void*_tmp30D;void*_tmp30E;void*_tmp30F;void*_tmp310;
void*_tmp311;void*_tmp312;void*_tmp313;void*_tmp314;_LL218: _tmp27E=_tmp27D.f1;
if(_tmp27E <= (void*)4)goto _LL21A;if(*((int*)_tmp27E)!= 0)goto _LL21A;_LL219: Cyc_Tcutil_unify_it(
t2,t1);return;_LL21A: _tmp27F=_tmp27D.f1;if((int)_tmp27F != 0)goto _LL21C;_tmp280=
_tmp27D.f2;if((int)_tmp280 != 0)goto _LL21C;_LL21B: return;_LL21C: _tmp281=_tmp27D.f1;
if(_tmp281 <= (void*)4)goto _LL21E;if(*((int*)_tmp281)!= 1)goto _LL21E;_tmp282=((
struct Cyc_Absyn_VarType_struct*)_tmp281)->f1;_tmp283=_tmp27D.f2;if(_tmp283 <= (
void*)4)goto _LL21E;if(*((int*)_tmp283)!= 1)goto _LL21E;_tmp284=((struct Cyc_Absyn_VarType_struct*)
_tmp283)->f1;_LL21D: {struct _dyneither_ptr*_tmp315=_tmp282->name;struct
_dyneither_ptr*_tmp316=_tmp284->name;int _tmp317=_tmp282->identity;int _tmp318=
_tmp284->identity;void*_tmp319=Cyc_Tcutil_tvar_kind(_tmp282);void*_tmp31A=Cyc_Tcutil_tvar_kind(
_tmp284);if(_tmp318 == _tmp317  && Cyc_strptrcmp(_tmp315,_tmp316)== 0){if(_tmp319
!= _tmp31A){{const char*_tmpCF5;void*_tmpCF4[3];struct Cyc_String_pa_struct _tmpCF3;
struct Cyc_String_pa_struct _tmpCF2;struct Cyc_String_pa_struct _tmpCF1;Cyc_Tcutil_failure_reason=(
struct _dyneither_ptr)((_tmpCF1.tag=0,((_tmpCF1.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_kind2string(_tmp31A)),((_tmpCF2.tag=0,((_tmpCF2.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(_tmp319)),((
_tmpCF3.tag=0,((_tmpCF3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp315),((
_tmpCF4[0]=& _tmpCF3,((_tmpCF4[1]=& _tmpCF2,((_tmpCF4[2]=& _tmpCF1,Cyc_aprintf(((
_tmpCF5="same type variable %s has kinds %s and %s",_tag_dyneither(_tmpCF5,
sizeof(char),42))),_tag_dyneither(_tmpCF4,sizeof(void*),3))))))))))))))))))));}(
int)_throw((void*)Cyc_Tcutil_Unify);}return;}{const char*_tmpCF6;Cyc_Tcutil_failure_reason=((
_tmpCF6="(variable types are not the same)",_tag_dyneither(_tmpCF6,sizeof(char),
34)));}goto _LL217;}_LL21E: _tmp285=_tmp27D.f1;if(_tmp285 <= (void*)4)goto _LL220;
if(*((int*)_tmp285)!= 10)goto _LL220;_tmp286=((struct Cyc_Absyn_AggrType_struct*)
_tmp285)->f1;_tmp287=_tmp286.aggr_info;_tmp288=_tmp286.targs;_tmp289=_tmp27D.f2;
if(_tmp289 <= (void*)4)goto _LL220;if(*((int*)_tmp289)!= 10)goto _LL220;_tmp28A=((
struct Cyc_Absyn_AggrType_struct*)_tmp289)->f1;_tmp28B=_tmp28A.aggr_info;_tmp28C=
_tmp28A.targs;_LL21F: {void*_tmp322;struct _tuple2*_tmp323;struct _tuple12 _tmp321=
Cyc_Absyn_aggr_kinded_name(_tmp28B);_tmp322=_tmp321.f1;_tmp323=_tmp321.f2;{void*
_tmp325;struct _tuple2*_tmp326;struct _tuple12 _tmp324=Cyc_Absyn_aggr_kinded_name(
_tmp287);_tmp325=_tmp324.f1;_tmp326=_tmp324.f2;if(_tmp322 != _tmp325){{const char*
_tmpCF7;Cyc_Tcutil_failure_reason=((_tmpCF7="(struct and union type)",
_tag_dyneither(_tmpCF7,sizeof(char),24)));}goto _LL217;}if(Cyc_Absyn_qvar_cmp(
_tmp323,_tmp326)!= 0){{const char*_tmpCF8;Cyc_Tcutil_failure_reason=((_tmpCF8="(different type name)",
_tag_dyneither(_tmpCF8,sizeof(char),22)));}goto _LL217;}Cyc_Tcutil_unify_list(
_tmp28C,_tmp288);return;}}_LL220: _tmp28D=_tmp27D.f1;if(_tmp28D <= (void*)4)goto
_LL222;if(*((int*)_tmp28D)!= 12)goto _LL222;_tmp28E=((struct Cyc_Absyn_EnumType_struct*)
_tmp28D)->f1;_tmp28F=_tmp27D.f2;if(_tmp28F <= (void*)4)goto _LL222;if(*((int*)
_tmp28F)!= 12)goto _LL222;_tmp290=((struct Cyc_Absyn_EnumType_struct*)_tmp28F)->f1;
_LL221: if(Cyc_Absyn_qvar_cmp(_tmp28E,_tmp290)== 0)return;{const char*_tmpCF9;Cyc_Tcutil_failure_reason=((
_tmpCF9="(different enum types)",_tag_dyneither(_tmpCF9,sizeof(char),23)));}goto
_LL217;_LL222: _tmp291=_tmp27D.f1;if(_tmp291 <= (void*)4)goto _LL224;if(*((int*)
_tmp291)!= 13)goto _LL224;_tmp292=((struct Cyc_Absyn_AnonEnumType_struct*)_tmp291)->f1;
_tmp293=_tmp27D.f2;if(_tmp293 <= (void*)4)goto _LL224;if(*((int*)_tmp293)!= 13)
goto _LL224;_tmp294=((struct Cyc_Absyn_AnonEnumType_struct*)_tmp293)->f1;_LL223: {
int bad=0;for(0;_tmp292 != 0  && _tmp294 != 0;(_tmp292=_tmp292->tl,_tmp294=_tmp294->tl)){
struct Cyc_Absyn_Enumfield*_tmp32A=(struct Cyc_Absyn_Enumfield*)_tmp292->hd;struct
Cyc_Absyn_Enumfield*_tmp32B=(struct Cyc_Absyn_Enumfield*)_tmp294->hd;if(Cyc_Absyn_qvar_cmp(
_tmp32A->name,_tmp32B->name)!= 0){{const char*_tmpCFA;Cyc_Tcutil_failure_reason=((
_tmpCFA="(different names for enum fields)",_tag_dyneither(_tmpCFA,sizeof(char),
34)));}bad=1;break;}if(_tmp32A->tag == _tmp32B->tag)continue;if(_tmp32A->tag == 0
 || _tmp32B->tag == 0){{const char*_tmpCFB;Cyc_Tcutil_failure_reason=((_tmpCFB="(different tag values for enum fields)",
_tag_dyneither(_tmpCFB,sizeof(char),39)));}bad=1;break;}if(!Cyc_Evexp_same_const_exp((
struct Cyc_Absyn_Exp*)_check_null(_tmp32A->tag),(struct Cyc_Absyn_Exp*)_check_null(
_tmp32B->tag))){{const char*_tmpCFC;Cyc_Tcutil_failure_reason=((_tmpCFC="(different tag values for enum fields)",
_tag_dyneither(_tmpCFC,sizeof(char),39)));}bad=1;break;}}if(bad)goto _LL217;if(
_tmp292 == 0  && _tmp294 == 0)return;{const char*_tmpCFD;Cyc_Tcutil_failure_reason=((
_tmpCFD="(different number of fields for enums)",_tag_dyneither(_tmpCFD,sizeof(
char),39)));}goto _LL217;}_LL224: _tmp295=_tmp27D.f1;if(_tmp295 <= (void*)4)goto
_LL226;if(*((int*)_tmp295)!= 2)goto _LL226;_tmp296=((struct Cyc_Absyn_DatatypeType_struct*)
_tmp295)->f1;_tmp297=_tmp296.datatype_info;if((_tmp297.KnownDatatype).tag != 2)
goto _LL226;_tmp298=(struct Cyc_Absyn_Datatypedecl**)(_tmp297.KnownDatatype).val;
_tmp299=*_tmp298;_tmp29A=_tmp296.targs;_tmp29B=_tmp296.rgn;_tmp29C=_tmp27D.f2;
if(_tmp29C <= (void*)4)goto _LL226;if(*((int*)_tmp29C)!= 2)goto _LL226;_tmp29D=((
struct Cyc_Absyn_DatatypeType_struct*)_tmp29C)->f1;_tmp29E=_tmp29D.datatype_info;
if((_tmp29E.KnownDatatype).tag != 2)goto _LL226;_tmp29F=(struct Cyc_Absyn_Datatypedecl**)(
_tmp29E.KnownDatatype).val;_tmp2A0=*_tmp29F;_tmp2A1=_tmp29D.targs;_tmp2A2=
_tmp29D.rgn;_LL225: if(_tmp299 == _tmp2A0  || Cyc_Absyn_qvar_cmp(_tmp299->name,
_tmp2A0->name)== 0){if(_tmp2A2 != 0  && _tmp29B != 0)Cyc_Tcutil_unify_it((void*)
_tmp2A2->v,(void*)_tmp29B->v);else{if(_tmp2A2 != 0  || _tmp29B != 0)goto
Datatype_fail;}Cyc_Tcutil_unify_list(_tmp2A1,_tmp29A);return;}Datatype_fail: {
const char*_tmpCFE;Cyc_Tcutil_failure_reason=((_tmpCFE="(different datatype types)",
_tag_dyneither(_tmpCFE,sizeof(char),27)));}goto _LL217;_LL226: _tmp2A3=_tmp27D.f1;
if(_tmp2A3 <= (void*)4)goto _LL228;if(*((int*)_tmp2A3)!= 3)goto _LL228;_tmp2A4=((
struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp2A3)->f1;_tmp2A5=_tmp2A4.field_info;
if((_tmp2A5.KnownDatatypefield).tag != 2)goto _LL228;_tmp2A6=(struct _tuple3)(
_tmp2A5.KnownDatatypefield).val;_tmp2A7=_tmp2A6.f1;_tmp2A8=_tmp2A6.f2;_tmp2A9=
_tmp2A4.targs;_tmp2AA=_tmp27D.f2;if(_tmp2AA <= (void*)4)goto _LL228;if(*((int*)
_tmp2AA)!= 3)goto _LL228;_tmp2AB=((struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp2AA)->f1;_tmp2AC=_tmp2AB.field_info;if((_tmp2AC.KnownDatatypefield).tag != 2)
goto _LL228;_tmp2AD=(struct _tuple3)(_tmp2AC.KnownDatatypefield).val;_tmp2AE=
_tmp2AD.f1;_tmp2AF=_tmp2AD.f2;_tmp2B0=_tmp2AB.targs;_LL227: if((_tmp2A7 == _tmp2AE
 || Cyc_Absyn_qvar_cmp(_tmp2A7->name,_tmp2AE->name)== 0) && (_tmp2A8 == _tmp2AF
 || Cyc_Absyn_qvar_cmp(_tmp2A8->name,_tmp2AF->name)== 0)){Cyc_Tcutil_unify_list(
_tmp2B0,_tmp2A9);return;}{const char*_tmpCFF;Cyc_Tcutil_failure_reason=((_tmpCFF="(different datatype field types)",
_tag_dyneither(_tmpCFF,sizeof(char),33)));}goto _LL217;_LL228: _tmp2B1=_tmp27D.f1;
if(_tmp2B1 <= (void*)4)goto _LL22A;if(*((int*)_tmp2B1)!= 4)goto _LL22A;_tmp2B2=((
struct Cyc_Absyn_PointerType_struct*)_tmp2B1)->f1;_tmp2B3=_tmp2B2.elt_typ;_tmp2B4=
_tmp2B2.elt_tq;_tmp2B5=_tmp2B2.ptr_atts;_tmp2B6=_tmp2B5.rgn;_tmp2B7=_tmp2B5.nullable;
_tmp2B8=_tmp2B5.bounds;_tmp2B9=_tmp2B5.zero_term;_tmp2BA=_tmp27D.f2;if(_tmp2BA <= (
void*)4)goto _LL22A;if(*((int*)_tmp2BA)!= 4)goto _LL22A;_tmp2BB=((struct Cyc_Absyn_PointerType_struct*)
_tmp2BA)->f1;_tmp2BC=_tmp2BB.elt_typ;_tmp2BD=_tmp2BB.elt_tq;_tmp2BE=_tmp2BB.ptr_atts;
_tmp2BF=_tmp2BE.rgn;_tmp2C0=_tmp2BE.nullable;_tmp2C1=_tmp2BE.bounds;_tmp2C2=
_tmp2BE.zero_term;_LL229: Cyc_Tcutil_unify_it(_tmp2BC,_tmp2B3);Cyc_Tcutil_unify_it(
_tmp2B6,_tmp2BF);Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;{const char*
_tmpD00;((void(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*
y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,
_tmp2C2,_tmp2B9,((_tmpD00="(not both zero terminated)",_tag_dyneither(_tmpD00,
sizeof(char),27))));}Cyc_Tcutil_unify_tqual(_tmp2BD,_tmp2BC,_tmp2B4,_tmp2B3);{
const char*_tmpD01;Cyc_Tcutil_unify_it_conrefs(Cyc_Tcutil_unify_it_bounds,_tmp2C1,
_tmp2B8,((_tmpD01="(different pointer bounds)",_tag_dyneither(_tmpD01,sizeof(
char),27))));}{void*_tmp334=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp2C1);
_LL253: if((int)_tmp334 != 0)goto _LL255;_LL254: return;_LL255:;_LL256: goto _LL252;
_LL252:;}{const char*_tmpD02;((void(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*
x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(
Cyc_Core_intcmp,_tmp2C0,_tmp2B7,((_tmpD02="(incompatible pointer types)",
_tag_dyneither(_tmpD02,sizeof(char),29))));}return;_LL22A: _tmp2C3=_tmp27D.f1;if(
_tmp2C3 <= (void*)4)goto _LL22C;if(*((int*)_tmp2C3)!= 5)goto _LL22C;_tmp2C4=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp2C3)->f1;_tmp2C5=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp2C3)->f2;_tmp2C6=_tmp27D.f2;if(_tmp2C6 <= (void*)4)goto _LL22C;if(*((int*)
_tmp2C6)!= 5)goto _LL22C;_tmp2C7=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp2C6)->f1;
_tmp2C8=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp2C6)->f2;_LL22B: if(_tmp2C7
== _tmp2C4  && ((_tmp2C8 == _tmp2C5  || _tmp2C8 == (void*)2  && _tmp2C5 == (void*)3)
 || _tmp2C8 == (void*)3  && _tmp2C5 == (void*)2))return;{const char*_tmpD03;Cyc_Tcutil_failure_reason=((
_tmpD03="(different integral types)",_tag_dyneither(_tmpD03,sizeof(char),27)));}
goto _LL217;_LL22C: _tmp2C9=_tmp27D.f1;if((int)_tmp2C9 != 1)goto _LL22E;_tmp2CA=
_tmp27D.f2;if((int)_tmp2CA != 1)goto _LL22E;_LL22D: return;_LL22E: _tmp2CB=_tmp27D.f1;
if(_tmp2CB <= (void*)4)goto _LL230;if(*((int*)_tmp2CB)!= 6)goto _LL230;_tmp2CC=((
struct Cyc_Absyn_DoubleType_struct*)_tmp2CB)->f1;_tmp2CD=_tmp27D.f2;if(_tmp2CD <= (
void*)4)goto _LL230;if(*((int*)_tmp2CD)!= 6)goto _LL230;_tmp2CE=((struct Cyc_Absyn_DoubleType_struct*)
_tmp2CD)->f1;_LL22F: if(_tmp2CC == _tmp2CE)return;goto _LL217;_LL230: _tmp2CF=
_tmp27D.f1;if(_tmp2CF <= (void*)4)goto _LL232;if(*((int*)_tmp2CF)!= 18)goto _LL232;
_tmp2D0=(void*)((struct Cyc_Absyn_TagType_struct*)_tmp2CF)->f1;_tmp2D1=_tmp27D.f2;
if(_tmp2D1 <= (void*)4)goto _LL232;if(*((int*)_tmp2D1)!= 18)goto _LL232;_tmp2D2=(
void*)((struct Cyc_Absyn_TagType_struct*)_tmp2D1)->f1;_LL231: Cyc_Tcutil_unify_it(
_tmp2D0,_tmp2D2);return;_LL232: _tmp2D3=_tmp27D.f1;if(_tmp2D3 <= (void*)4)goto
_LL234;if(*((int*)_tmp2D3)!= 17)goto _LL234;_tmp2D4=((struct Cyc_Absyn_ValueofType_struct*)
_tmp2D3)->f1;_tmp2D5=_tmp27D.f2;if(_tmp2D5 <= (void*)4)goto _LL234;if(*((int*)
_tmp2D5)!= 17)goto _LL234;_tmp2D6=((struct Cyc_Absyn_ValueofType_struct*)_tmp2D5)->f1;
_LL233: if(!Cyc_Evexp_same_const_exp(_tmp2D4,_tmp2D6)){{const char*_tmpD04;Cyc_Tcutil_failure_reason=((
_tmpD04="(cannot prove expressions are the same)",_tag_dyneither(_tmpD04,sizeof(
char),40)));}goto _LL217;}return;_LL234: _tmp2D7=_tmp27D.f1;if(_tmp2D7 <= (void*)4)
goto _LL236;if(*((int*)_tmp2D7)!= 7)goto _LL236;_tmp2D8=((struct Cyc_Absyn_ArrayType_struct*)
_tmp2D7)->f1;_tmp2D9=_tmp2D8.elt_type;_tmp2DA=_tmp2D8.tq;_tmp2DB=_tmp2D8.num_elts;
_tmp2DC=_tmp2D8.zero_term;_tmp2DD=_tmp27D.f2;if(_tmp2DD <= (void*)4)goto _LL236;
if(*((int*)_tmp2DD)!= 7)goto _LL236;_tmp2DE=((struct Cyc_Absyn_ArrayType_struct*)
_tmp2DD)->f1;_tmp2DF=_tmp2DE.elt_type;_tmp2E0=_tmp2DE.tq;_tmp2E1=_tmp2DE.num_elts;
_tmp2E2=_tmp2DE.zero_term;_LL235: Cyc_Tcutil_unify_it(_tmp2DF,_tmp2D9);Cyc_Tcutil_unify_tqual(
_tmp2E0,_tmp2DF,_tmp2DA,_tmp2D9);Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=
t2;{const char*_tmpD05;((void(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,
union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(
Cyc_Core_intcmp,_tmp2DC,_tmp2E2,((_tmpD05="(not both zero terminated)",
_tag_dyneither(_tmpD05,sizeof(char),27))));}if(_tmp2DB == _tmp2E1)return;if(
_tmp2DB == 0  || _tmp2E1 == 0)goto _LL217;if(Cyc_Evexp_same_const_exp((struct Cyc_Absyn_Exp*)
_tmp2DB,(struct Cyc_Absyn_Exp*)_tmp2E1))return;{const char*_tmpD06;Cyc_Tcutil_failure_reason=((
_tmpD06="(different array sizes)",_tag_dyneither(_tmpD06,sizeof(char),24)));}
goto _LL217;_LL236: _tmp2E3=_tmp27D.f1;if(_tmp2E3 <= (void*)4)goto _LL238;if(*((int*)
_tmp2E3)!= 8)goto _LL238;_tmp2E4=((struct Cyc_Absyn_FnType_struct*)_tmp2E3)->f1;
_tmp2E5=_tmp2E4.tvars;_tmp2E6=_tmp2E4.effect;_tmp2E7=_tmp2E4.ret_typ;_tmp2E8=
_tmp2E4.args;_tmp2E9=_tmp2E4.c_varargs;_tmp2EA=_tmp2E4.cyc_varargs;_tmp2EB=
_tmp2E4.rgn_po;_tmp2EC=_tmp2E4.attributes;_tmp2ED=_tmp27D.f2;if(_tmp2ED <= (void*)
4)goto _LL238;if(*((int*)_tmp2ED)!= 8)goto _LL238;_tmp2EE=((struct Cyc_Absyn_FnType_struct*)
_tmp2ED)->f1;_tmp2EF=_tmp2EE.tvars;_tmp2F0=_tmp2EE.effect;_tmp2F1=_tmp2EE.ret_typ;
_tmp2F2=_tmp2EE.args;_tmp2F3=_tmp2EE.c_varargs;_tmp2F4=_tmp2EE.cyc_varargs;
_tmp2F5=_tmp2EE.rgn_po;_tmp2F6=_tmp2EE.attributes;_LL237: {int done=0;struct
_RegionHandle _tmp33A=_new_region("rgn");struct _RegionHandle*rgn=& _tmp33A;
_push_region(rgn);{struct Cyc_List_List*inst=0;while(_tmp2EF != 0){if(_tmp2E5 == 0){{
const char*_tmpD07;Cyc_Tcutil_failure_reason=((_tmpD07="(second function type has too few type variables)",
_tag_dyneither(_tmpD07,sizeof(char),50)));}(int)_throw((void*)Cyc_Tcutil_Unify);}{
void*_tmp33C=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp2EF->hd);void*
_tmp33D=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp2E5->hd);if(_tmp33C != 
_tmp33D){{const char*_tmpD0D;void*_tmpD0C[3];struct Cyc_String_pa_struct _tmpD0B;
struct Cyc_String_pa_struct _tmpD0A;struct Cyc_String_pa_struct _tmpD09;Cyc_Tcutil_failure_reason=(
struct _dyneither_ptr)((_tmpD09.tag=0,((_tmpD09.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_kind2string(_tmp33D)),((_tmpD0A.tag=0,((_tmpD0A.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(_tmp33C)),((
_tmpD0B.tag=0,((_tmpD0B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_tvar2string((
struct Cyc_Absyn_Tvar*)_tmp2EF->hd)),((_tmpD0C[0]=& _tmpD0B,((_tmpD0C[1]=& _tmpD0A,((
_tmpD0C[2]=& _tmpD09,Cyc_aprintf(((_tmpD0D="(type var %s has different kinds %s and %s)",
_tag_dyneither(_tmpD0D,sizeof(char),44))),_tag_dyneither(_tmpD0C,sizeof(void*),3))))))))))))))))))));}(
int)_throw((void*)Cyc_Tcutil_Unify);}{struct _tuple14*_tmpD17;struct Cyc_Absyn_VarType_struct
_tmpD16;struct Cyc_Absyn_VarType_struct*_tmpD15;struct Cyc_List_List*_tmpD14;inst=((
_tmpD14=_region_malloc(rgn,sizeof(*_tmpD14)),((_tmpD14->hd=((_tmpD17=
_region_malloc(rgn,sizeof(*_tmpD17)),((_tmpD17->f1=(struct Cyc_Absyn_Tvar*)
_tmp2E5->hd,((_tmpD17->f2=(void*)((_tmpD15=_cycalloc(sizeof(*_tmpD15)),((_tmpD15[
0]=((_tmpD16.tag=1,((_tmpD16.f1=(struct Cyc_Absyn_Tvar*)_tmp2EF->hd,_tmpD16)))),
_tmpD15)))),_tmpD17)))))),((_tmpD14->tl=inst,_tmpD14))))));}_tmp2EF=_tmp2EF->tl;
_tmp2E5=_tmp2E5->tl;}}if(_tmp2E5 != 0){{const char*_tmpD18;Cyc_Tcutil_failure_reason=((
_tmpD18="(second function type has too many type variables)",_tag_dyneither(
_tmpD18,sizeof(char),51)));}_npop_handler(0);goto _LL217;}if(inst != 0){{struct Cyc_Absyn_FnType_struct
_tmpD24;struct Cyc_Absyn_FnInfo _tmpD23;struct Cyc_Absyn_FnType_struct*_tmpD22;
struct Cyc_Absyn_FnType_struct _tmpD1E;struct Cyc_Absyn_FnInfo _tmpD1D;struct Cyc_Absyn_FnType_struct*
_tmpD1C;Cyc_Tcutil_unify_it((void*)((_tmpD1C=_cycalloc(sizeof(*_tmpD1C)),((
_tmpD1C[0]=((_tmpD1E.tag=8,((_tmpD1E.f1=((_tmpD1D.tvars=0,((_tmpD1D.effect=
_tmp2F0,((_tmpD1D.ret_typ=_tmp2F1,((_tmpD1D.args=_tmp2F2,((_tmpD1D.c_varargs=
_tmp2F3,((_tmpD1D.cyc_varargs=_tmp2F4,((_tmpD1D.rgn_po=_tmp2F5,((_tmpD1D.attributes=
_tmp2F6,_tmpD1D)))))))))))))))),_tmpD1E)))),_tmpD1C)))),Cyc_Tcutil_rsubstitute(
rgn,inst,(void*)((_tmpD22=_cycalloc(sizeof(*_tmpD22)),((_tmpD22[0]=((_tmpD24.tag=
8,((_tmpD24.f1=((_tmpD23.tvars=0,((_tmpD23.effect=_tmp2E6,((_tmpD23.ret_typ=
_tmp2E7,((_tmpD23.args=_tmp2E8,((_tmpD23.c_varargs=_tmp2E9,((_tmpD23.cyc_varargs=
_tmp2EA,((_tmpD23.rgn_po=_tmp2EB,((_tmpD23.attributes=_tmp2EC,_tmpD23)))))))))))))))),
_tmpD24)))),_tmpD22))))));}done=1;}}if(done){_npop_handler(0);return;}Cyc_Tcutil_unify_it(
_tmp2F1,_tmp2E7);for(0;_tmp2F2 != 0  && _tmp2E8 != 0;(_tmp2F2=_tmp2F2->tl,_tmp2E8=
_tmp2E8->tl)){struct Cyc_Absyn_Tqual _tmp34F;void*_tmp350;struct _tuple9 _tmp34E=*((
struct _tuple9*)_tmp2F2->hd);_tmp34F=_tmp34E.f2;_tmp350=_tmp34E.f3;{struct Cyc_Absyn_Tqual
_tmp352;void*_tmp353;struct _tuple9 _tmp351=*((struct _tuple9*)_tmp2E8->hd);_tmp352=
_tmp351.f2;_tmp353=_tmp351.f3;Cyc_Tcutil_unify_it(_tmp350,_tmp353);Cyc_Tcutil_unify_tqual(
_tmp34F,_tmp350,_tmp352,_tmp353);}}Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=
t2;if(_tmp2F2 != 0  || _tmp2E8 != 0){{const char*_tmpD25;Cyc_Tcutil_failure_reason=((
_tmpD25="(function types have different number of arguments)",_tag_dyneither(
_tmpD25,sizeof(char),52)));}_npop_handler(0);goto _LL217;}if(_tmp2F3 != _tmp2E9){{
const char*_tmpD26;Cyc_Tcutil_failure_reason=((_tmpD26="(only one function type takes C varargs)",
_tag_dyneither(_tmpD26,sizeof(char),41)));}_npop_handler(0);goto _LL217;}{int
bad_cyc_vararg=0;{struct _tuple17 _tmpD27;struct _tuple17 _tmp357=(_tmpD27.f1=
_tmp2F4,((_tmpD27.f2=_tmp2EA,_tmpD27)));struct Cyc_Absyn_VarargInfo*_tmp358;
struct Cyc_Absyn_VarargInfo*_tmp359;struct Cyc_Absyn_VarargInfo*_tmp35A;struct Cyc_Absyn_VarargInfo*
_tmp35B;struct Cyc_Absyn_VarargInfo*_tmp35C;struct Cyc_Absyn_VarargInfo _tmp35D;
struct Cyc_Core_Opt*_tmp35E;struct Cyc_Absyn_Tqual _tmp35F;void*_tmp360;int _tmp361;
struct Cyc_Absyn_VarargInfo*_tmp362;struct Cyc_Absyn_VarargInfo _tmp363;struct Cyc_Core_Opt*
_tmp364;struct Cyc_Absyn_Tqual _tmp365;void*_tmp366;int _tmp367;_LL258: _tmp358=
_tmp357.f1;if(_tmp358 != 0)goto _LL25A;_tmp359=_tmp357.f2;if(_tmp359 != 0)goto
_LL25A;_LL259: goto _LL257;_LL25A: _tmp35A=_tmp357.f1;if(_tmp35A != 0)goto _LL25C;
_LL25B: goto _LL25D;_LL25C: _tmp35B=_tmp357.f2;if(_tmp35B != 0)goto _LL25E;_LL25D:
bad_cyc_vararg=1;{const char*_tmpD28;Cyc_Tcutil_failure_reason=((_tmpD28="(only one function type takes varargs)",
_tag_dyneither(_tmpD28,sizeof(char),39)));}goto _LL257;_LL25E: _tmp35C=_tmp357.f1;
if(_tmp35C == 0)goto _LL257;_tmp35D=*_tmp35C;_tmp35E=_tmp35D.name;_tmp35F=_tmp35D.tq;
_tmp360=_tmp35D.type;_tmp361=_tmp35D.inject;_tmp362=_tmp357.f2;if(_tmp362 == 0)
goto _LL257;_tmp363=*_tmp362;_tmp364=_tmp363.name;_tmp365=_tmp363.tq;_tmp366=
_tmp363.type;_tmp367=_tmp363.inject;_LL25F: Cyc_Tcutil_unify_it(_tmp360,_tmp366);
Cyc_Tcutil_unify_tqual(_tmp35F,_tmp360,_tmp365,_tmp366);if(_tmp361 != _tmp367){
bad_cyc_vararg=1;{const char*_tmpD29;Cyc_Tcutil_failure_reason=((_tmpD29="(only one function type injects varargs)",
_tag_dyneither(_tmpD29,sizeof(char),41)));}}goto _LL257;_LL257:;}if(
bad_cyc_vararg){_npop_handler(0);goto _LL217;}{int bad_effect=0;{struct _tuple18
_tmpD2A;struct _tuple18 _tmp36B=(_tmpD2A.f1=_tmp2F0,((_tmpD2A.f2=_tmp2E6,_tmpD2A)));
struct Cyc_Core_Opt*_tmp36C;struct Cyc_Core_Opt*_tmp36D;struct Cyc_Core_Opt*_tmp36E;
struct Cyc_Core_Opt*_tmp36F;_LL261: _tmp36C=_tmp36B.f1;if(_tmp36C != 0)goto _LL263;
_tmp36D=_tmp36B.f2;if(_tmp36D != 0)goto _LL263;_LL262: goto _LL260;_LL263: _tmp36E=
_tmp36B.f1;if(_tmp36E != 0)goto _LL265;_LL264: goto _LL266;_LL265: _tmp36F=_tmp36B.f2;
if(_tmp36F != 0)goto _LL267;_LL266: bad_effect=1;goto _LL260;_LL267:;_LL268:
bad_effect=!Cyc_Tcutil_unify_effect((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp2F0))->v,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp2E6))->v);goto _LL260;
_LL260:;}Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;if(bad_effect){{const
char*_tmpD2B;Cyc_Tcutil_failure_reason=((_tmpD2B="(function type effects do not unify)",
_tag_dyneither(_tmpD2B,sizeof(char),37)));}_npop_handler(0);goto _LL217;}if(!Cyc_Tcutil_same_atts(
_tmp2EC,_tmp2F6)){{const char*_tmpD2C;Cyc_Tcutil_failure_reason=((_tmpD2C="(function types have different attributes)",
_tag_dyneither(_tmpD2C,sizeof(char),43)));}_npop_handler(0);goto _LL217;}if(!Cyc_Tcutil_same_rgn_po(
_tmp2EB,_tmp2F5)){{const char*_tmpD2D;Cyc_Tcutil_failure_reason=((_tmpD2D="(function types have different region lifetime orderings)",
_tag_dyneither(_tmpD2D,sizeof(char),58)));}_npop_handler(0);goto _LL217;}
_npop_handler(0);return;}};_pop_region(rgn);}_LL238: _tmp2F7=_tmp27D.f1;if(
_tmp2F7 <= (void*)4)goto _LL23A;if(*((int*)_tmp2F7)!= 9)goto _LL23A;_tmp2F8=((
struct Cyc_Absyn_TupleType_struct*)_tmp2F7)->f1;_tmp2F9=_tmp27D.f2;if(_tmp2F9 <= (
void*)4)goto _LL23A;if(*((int*)_tmp2F9)!= 9)goto _LL23A;_tmp2FA=((struct Cyc_Absyn_TupleType_struct*)
_tmp2F9)->f1;_LL239: for(0;_tmp2FA != 0  && _tmp2F8 != 0;(_tmp2FA=_tmp2FA->tl,
_tmp2F8=_tmp2F8->tl)){struct Cyc_Absyn_Tqual _tmp374;void*_tmp375;struct _tuple11
_tmp373=*((struct _tuple11*)_tmp2FA->hd);_tmp374=_tmp373.f1;_tmp375=_tmp373.f2;{
struct Cyc_Absyn_Tqual _tmp377;void*_tmp378;struct _tuple11 _tmp376=*((struct
_tuple11*)_tmp2F8->hd);_tmp377=_tmp376.f1;_tmp378=_tmp376.f2;Cyc_Tcutil_unify_it(
_tmp375,_tmp378);Cyc_Tcutil_unify_tqual(_tmp374,_tmp375,_tmp377,_tmp378);}}if(
_tmp2FA == 0  && _tmp2F8 == 0)return;Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=
t2;{const char*_tmpD2E;Cyc_Tcutil_failure_reason=((_tmpD2E="(tuple types have different numbers of components)",
_tag_dyneither(_tmpD2E,sizeof(char),51)));}goto _LL217;_LL23A: _tmp2FB=_tmp27D.f1;
if(_tmp2FB <= (void*)4)goto _LL23C;if(*((int*)_tmp2FB)!= 11)goto _LL23C;_tmp2FC=(
void*)((struct Cyc_Absyn_AnonAggrType_struct*)_tmp2FB)->f1;_tmp2FD=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp2FB)->f2;_tmp2FE=_tmp27D.f2;if(_tmp2FE <= (void*)4)goto _LL23C;if(*((int*)
_tmp2FE)!= 11)goto _LL23C;_tmp2FF=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp2FE)->f1;_tmp300=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp2FE)->f2;_LL23B:
if(_tmp2FF != _tmp2FC){{const char*_tmpD2F;Cyc_Tcutil_failure_reason=((_tmpD2F="(struct and union type)",
_tag_dyneither(_tmpD2F,sizeof(char),24)));}goto _LL217;}for(0;_tmp300 != 0  && 
_tmp2FD != 0;(_tmp300=_tmp300->tl,_tmp2FD=_tmp2FD->tl)){struct Cyc_Absyn_Aggrfield*
_tmp37B=(struct Cyc_Absyn_Aggrfield*)_tmp300->hd;struct Cyc_Absyn_Aggrfield*
_tmp37C=(struct Cyc_Absyn_Aggrfield*)_tmp2FD->hd;if(Cyc_strptrcmp(_tmp37B->name,
_tmp37C->name)!= 0){{const char*_tmpD30;Cyc_Tcutil_failure_reason=((_tmpD30="(different member names)",
_tag_dyneither(_tmpD30,sizeof(char),25)));}(int)_throw((void*)Cyc_Tcutil_Unify);}
Cyc_Tcutil_unify_it(_tmp37B->type,_tmp37C->type);Cyc_Tcutil_unify_tqual(_tmp37B->tq,
_tmp37B->type,_tmp37C->tq,_tmp37C->type);if(!Cyc_Tcutil_same_atts(_tmp37B->attributes,
_tmp37C->attributes)){Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;{const
char*_tmpD31;Cyc_Tcutil_failure_reason=((_tmpD31="(different attributes on member)",
_tag_dyneither(_tmpD31,sizeof(char),33)));}(int)_throw((void*)Cyc_Tcutil_Unify);}
if((_tmp37B->width != 0  && _tmp37C->width == 0  || _tmp37C->width != 0  && _tmp37B->width
== 0) || (_tmp37B->width != 0  && _tmp37C->width != 0) && !Cyc_Evexp_same_const_exp((
struct Cyc_Absyn_Exp*)_check_null(_tmp37B->width),(struct Cyc_Absyn_Exp*)
_check_null(_tmp37C->width))){Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;{
const char*_tmpD32;Cyc_Tcutil_failure_reason=((_tmpD32="(different bitfield widths on member)",
_tag_dyneither(_tmpD32,sizeof(char),38)));}(int)_throw((void*)Cyc_Tcutil_Unify);}}
if(_tmp300 == 0  && _tmp2FD == 0)return;Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=
t2;{const char*_tmpD33;Cyc_Tcutil_failure_reason=((_tmpD33="(different number of members)",
_tag_dyneither(_tmpD33,sizeof(char),30)));}goto _LL217;_LL23C: _tmp301=_tmp27D.f1;
if((int)_tmp301 != 2)goto _LL23E;_tmp302=_tmp27D.f2;if((int)_tmp302 != 2)goto _LL23E;
_LL23D: return;_LL23E: _tmp303=_tmp27D.f1;if((int)_tmp303 != 3)goto _LL240;_tmp304=
_tmp27D.f2;if((int)_tmp304 != 3)goto _LL240;_LL23F: return;_LL240: _tmp305=_tmp27D.f1;
if(_tmp305 <= (void*)4)goto _LL242;if(*((int*)_tmp305)!= 14)goto _LL242;_tmp306=(
void*)((struct Cyc_Absyn_RgnHandleType_struct*)_tmp305)->f1;_tmp307=_tmp27D.f2;
if(_tmp307 <= (void*)4)goto _LL242;if(*((int*)_tmp307)!= 14)goto _LL242;_tmp308=(
void*)((struct Cyc_Absyn_RgnHandleType_struct*)_tmp307)->f1;_LL241: Cyc_Tcutil_unify_it(
_tmp306,_tmp308);return;_LL242: _tmp309=_tmp27D.f1;if(_tmp309 <= (void*)4)goto
_LL244;if(*((int*)_tmp309)!= 15)goto _LL244;_tmp30A=(void*)((struct Cyc_Absyn_DynRgnType_struct*)
_tmp309)->f1;_tmp30B=(void*)((struct Cyc_Absyn_DynRgnType_struct*)_tmp309)->f2;
_tmp30C=_tmp27D.f2;if(_tmp30C <= (void*)4)goto _LL244;if(*((int*)_tmp30C)!= 15)
goto _LL244;_tmp30D=(void*)((struct Cyc_Absyn_DynRgnType_struct*)_tmp30C)->f1;
_tmp30E=(void*)((struct Cyc_Absyn_DynRgnType_struct*)_tmp30C)->f2;_LL243: Cyc_Tcutil_unify_it(
_tmp30A,_tmp30D);Cyc_Tcutil_unify_it(_tmp30B,_tmp30E);return;_LL244: _tmp30F=
_tmp27D.f1;if(_tmp30F <= (void*)4)goto _LL246;if(*((int*)_tmp30F)!= 20)goto _LL246;
_LL245: goto _LL247;_LL246: _tmp310=_tmp27D.f2;if(_tmp310 <= (void*)4)goto _LL248;if(*((
int*)_tmp310)!= 20)goto _LL248;_LL247: goto _LL249;_LL248: _tmp311=_tmp27D.f1;if(
_tmp311 <= (void*)4)goto _LL24A;if(*((int*)_tmp311)!= 19)goto _LL24A;_LL249: goto
_LL24B;_LL24A: _tmp312=_tmp27D.f1;if(_tmp312 <= (void*)4)goto _LL24C;if(*((int*)
_tmp312)!= 21)goto _LL24C;_LL24B: goto _LL24D;_LL24C: _tmp313=_tmp27D.f2;if(_tmp313
<= (void*)4)goto _LL24E;if(*((int*)_tmp313)!= 21)goto _LL24E;_LL24D: goto _LL24F;
_LL24E: _tmp314=_tmp27D.f2;if(_tmp314 <= (void*)4)goto _LL250;if(*((int*)_tmp314)!= 
19)goto _LL250;_LL24F: if(Cyc_Tcutil_unify_effect(t1,t2))return;{const char*_tmpD34;
Cyc_Tcutil_failure_reason=((_tmpD34="(effects don't unify)",_tag_dyneither(
_tmpD34,sizeof(char),22)));}goto _LL217;_LL250:;_LL251: goto _LL217;_LL217:;}(int)
_throw((void*)Cyc_Tcutil_Unify);}int Cyc_Tcutil_star_cmp(int(*cmp)(void*,void*),
void*a1,void*a2);int Cyc_Tcutil_star_cmp(int(*cmp)(void*,void*),void*a1,void*a2){
if(a1 == a2)return 0;if(a1 == 0  && a2 != 0)return - 1;if(a1 != 0  && a2 == 0)return 1;
return cmp((void*)_check_null(a1),(void*)_check_null(a2));}static int Cyc_Tcutil_tqual_cmp(
struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2);static int Cyc_Tcutil_tqual_cmp(
struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2){int _tmp382=(tq1.real_const + (
tq1.q_volatile << 1))+ (tq1.q_restrict << 2);int _tmp383=(tq2.real_const + (tq2.q_volatile
<< 1))+ (tq2.q_restrict << 2);return Cyc_Core_intcmp(_tmp382,_tmp383);}static int
Cyc_Tcutil_conrefs_cmp(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union
Cyc_Absyn_Constraint*y);static int Cyc_Tcutil_conrefs_cmp(int(*cmp)(void*,void*),
union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y){x=Cyc_Absyn_compress_conref(
x);y=Cyc_Absyn_compress_conref(y);if(x == y)return 0;{union Cyc_Absyn_Constraint*
_tmp384=x;union Cyc_Absyn_Constraint _tmp385;int _tmp386;union Cyc_Absyn_Constraint
_tmp387;void*_tmp388;union Cyc_Absyn_Constraint _tmp389;union Cyc_Absyn_Constraint*
_tmp38A;_LL26A: _tmp385=*_tmp384;if((_tmp385.No_constr).tag != 3)goto _LL26C;
_tmp386=(int)(_tmp385.No_constr).val;_LL26B: return - 1;_LL26C: _tmp387=*_tmp384;if((
_tmp387.Eq_constr).tag != 1)goto _LL26E;_tmp388=(void*)(_tmp387.Eq_constr).val;
_LL26D: {union Cyc_Absyn_Constraint*_tmp38B=y;union Cyc_Absyn_Constraint _tmp38C;
int _tmp38D;union Cyc_Absyn_Constraint _tmp38E;void*_tmp38F;union Cyc_Absyn_Constraint
_tmp390;union Cyc_Absyn_Constraint*_tmp391;_LL271: _tmp38C=*_tmp38B;if((_tmp38C.No_constr).tag
!= 3)goto _LL273;_tmp38D=(int)(_tmp38C.No_constr).val;_LL272: return 1;_LL273:
_tmp38E=*_tmp38B;if((_tmp38E.Eq_constr).tag != 1)goto _LL275;_tmp38F=(void*)(
_tmp38E.Eq_constr).val;_LL274: return cmp(_tmp388,_tmp38F);_LL275: _tmp390=*_tmp38B;
if((_tmp390.Forward_constr).tag != 2)goto _LL270;_tmp391=(union Cyc_Absyn_Constraint*)(
_tmp390.Forward_constr).val;_LL276: {const char*_tmpD37;void*_tmpD36;(_tmpD36=0,
Cyc_Tcutil_impos(((_tmpD37="unify_conref: forward after compress(2)",
_tag_dyneither(_tmpD37,sizeof(char),40))),_tag_dyneither(_tmpD36,sizeof(void*),0)));}
_LL270:;}_LL26E: _tmp389=*_tmp384;if((_tmp389.Forward_constr).tag != 2)goto _LL269;
_tmp38A=(union Cyc_Absyn_Constraint*)(_tmp389.Forward_constr).val;_LL26F: {const
char*_tmpD3A;void*_tmpD39;(_tmpD39=0,Cyc_Tcutil_impos(((_tmpD3A="unify_conref: forward after compress",
_tag_dyneither(_tmpD3A,sizeof(char),37))),_tag_dyneither(_tmpD39,sizeof(void*),0)));}
_LL269:;}}static int Cyc_Tcutil_tqual_type_cmp(struct _tuple11*tqt1,struct _tuple11*
tqt2);static int Cyc_Tcutil_tqual_type_cmp(struct _tuple11*tqt1,struct _tuple11*tqt2){
struct _tuple11 _tmp397;struct Cyc_Absyn_Tqual _tmp398;void*_tmp399;struct _tuple11*
_tmp396=tqt1;_tmp397=*_tmp396;_tmp398=_tmp397.f1;_tmp399=_tmp397.f2;{struct
_tuple11 _tmp39B;struct Cyc_Absyn_Tqual _tmp39C;void*_tmp39D;struct _tuple11*_tmp39A=
tqt2;_tmp39B=*_tmp39A;_tmp39C=_tmp39B.f1;_tmp39D=_tmp39B.f2;{int _tmp39E=Cyc_Tcutil_tqual_cmp(
_tmp398,_tmp39C);if(_tmp39E != 0)return _tmp39E;return Cyc_Tcutil_typecmp(_tmp399,
_tmp39D);}}}static int Cyc_Tcutil_aggrfield_cmp(struct Cyc_Absyn_Aggrfield*f1,
struct Cyc_Absyn_Aggrfield*f2);static int Cyc_Tcutil_aggrfield_cmp(struct Cyc_Absyn_Aggrfield*
f1,struct Cyc_Absyn_Aggrfield*f2){int _tmp39F=Cyc_strptrcmp(f1->name,f2->name);if(
_tmp39F != 0)return _tmp39F;{int _tmp3A0=Cyc_Tcutil_tqual_cmp(f1->tq,f2->tq);if(
_tmp3A0 != 0)return _tmp3A0;{int _tmp3A1=Cyc_Tcutil_typecmp(f1->type,f2->type);if(
_tmp3A1 != 0)return _tmp3A1;{int _tmp3A2=Cyc_List_list_cmp(Cyc_Tcutil_attribute_cmp,
f1->attributes,f2->attributes);if(_tmp3A2 != 0)return _tmp3A2;return((int(*)(int(*
cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*
a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,f1->width,f2->width);}}}}static
int Cyc_Tcutil_enumfield_cmp(struct Cyc_Absyn_Enumfield*e1,struct Cyc_Absyn_Enumfield*
e2);static int Cyc_Tcutil_enumfield_cmp(struct Cyc_Absyn_Enumfield*e1,struct Cyc_Absyn_Enumfield*
e2){int _tmp3A3=Cyc_Absyn_qvar_cmp(e1->name,e2->name);if(_tmp3A3 != 0)return
_tmp3A3;return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),
struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,
e1->tag,e2->tag);}static int Cyc_Tcutil_type_case_number(void*t);static int Cyc_Tcutil_type_case_number(
void*t){void*_tmp3A4=t;_LL278: if((int)_tmp3A4 != 0)goto _LL27A;_LL279: return 0;
_LL27A: if(_tmp3A4 <= (void*)4)goto _LL286;if(*((int*)_tmp3A4)!= 0)goto _LL27C;
_LL27B: return 1;_LL27C: if(*((int*)_tmp3A4)!= 1)goto _LL27E;_LL27D: return 2;_LL27E:
if(*((int*)_tmp3A4)!= 2)goto _LL280;_LL27F: return 3;_LL280: if(*((int*)_tmp3A4)!= 3)
goto _LL282;_LL281: return 4;_LL282: if(*((int*)_tmp3A4)!= 4)goto _LL284;_LL283:
return 5;_LL284: if(*((int*)_tmp3A4)!= 5)goto _LL286;_LL285: return 6;_LL286: if((int)
_tmp3A4 != 1)goto _LL288;_LL287: return 7;_LL288: if(_tmp3A4 <= (void*)4)goto _LL29C;
if(*((int*)_tmp3A4)!= 6)goto _LL28A;_LL289: return 8;_LL28A: if(*((int*)_tmp3A4)!= 7)
goto _LL28C;_LL28B: return 9;_LL28C: if(*((int*)_tmp3A4)!= 8)goto _LL28E;_LL28D:
return 10;_LL28E: if(*((int*)_tmp3A4)!= 9)goto _LL290;_LL28F: return 11;_LL290: if(*((
int*)_tmp3A4)!= 10)goto _LL292;_LL291: return 12;_LL292: if(*((int*)_tmp3A4)!= 11)
goto _LL294;_LL293: return 14;_LL294: if(*((int*)_tmp3A4)!= 12)goto _LL296;_LL295:
return 16;_LL296: if(*((int*)_tmp3A4)!= 13)goto _LL298;_LL297: return 17;_LL298: if(*((
int*)_tmp3A4)!= 14)goto _LL29A;_LL299: return 18;_LL29A: if(*((int*)_tmp3A4)!= 16)
goto _LL29C;_LL29B: return 19;_LL29C: if((int)_tmp3A4 != 3)goto _LL29E;_LL29D: return 20;
_LL29E: if((int)_tmp3A4 != 2)goto _LL2A0;_LL29F: return 21;_LL2A0: if(_tmp3A4 <= (void*)
4)goto _LL2A2;if(*((int*)_tmp3A4)!= 19)goto _LL2A2;_LL2A1: return 22;_LL2A2: if(
_tmp3A4 <= (void*)4)goto _LL2A4;if(*((int*)_tmp3A4)!= 20)goto _LL2A4;_LL2A3: return
23;_LL2A4: if(_tmp3A4 <= (void*)4)goto _LL2A6;if(*((int*)_tmp3A4)!= 21)goto _LL2A6;
_LL2A5: return 24;_LL2A6: if(_tmp3A4 <= (void*)4)goto _LL2A8;if(*((int*)_tmp3A4)!= 18)
goto _LL2A8;_LL2A7: return 26;_LL2A8: if(_tmp3A4 <= (void*)4)goto _LL2AA;if(*((int*)
_tmp3A4)!= 15)goto _LL2AA;_LL2A9: return 27;_LL2AA: if(_tmp3A4 <= (void*)4)goto _LL277;
if(*((int*)_tmp3A4)!= 17)goto _LL277;_LL2AB: return 28;_LL277:;}int Cyc_Tcutil_typecmp(
void*t1,void*t2);int Cyc_Tcutil_typecmp(void*t1,void*t2){t1=Cyc_Tcutil_compress(
t1);t2=Cyc_Tcutil_compress(t2);if(t1 == t2)return 0;{int _tmp3A5=Cyc_Core_intcmp(
Cyc_Tcutil_type_case_number(t1),Cyc_Tcutil_type_case_number(t2));if(_tmp3A5 != 0)
return _tmp3A5;{struct _tuple0 _tmpD3B;struct _tuple0 _tmp3A7=(_tmpD3B.f1=t2,((
_tmpD3B.f2=t1,_tmpD3B)));void*_tmp3A8;void*_tmp3A9;void*_tmp3AA;struct Cyc_Absyn_Tvar*
_tmp3AB;void*_tmp3AC;struct Cyc_Absyn_Tvar*_tmp3AD;void*_tmp3AE;struct Cyc_Absyn_AggrInfo
_tmp3AF;union Cyc_Absyn_AggrInfoU _tmp3B0;struct Cyc_List_List*_tmp3B1;void*_tmp3B2;
struct Cyc_Absyn_AggrInfo _tmp3B3;union Cyc_Absyn_AggrInfoU _tmp3B4;struct Cyc_List_List*
_tmp3B5;void*_tmp3B6;struct _tuple2*_tmp3B7;void*_tmp3B8;struct _tuple2*_tmp3B9;
void*_tmp3BA;struct Cyc_List_List*_tmp3BB;void*_tmp3BC;struct Cyc_List_List*
_tmp3BD;void*_tmp3BE;struct Cyc_Absyn_DatatypeInfo _tmp3BF;union Cyc_Absyn_DatatypeInfoU
_tmp3C0;struct Cyc_Absyn_Datatypedecl**_tmp3C1;struct Cyc_Absyn_Datatypedecl*
_tmp3C2;struct Cyc_List_List*_tmp3C3;struct Cyc_Core_Opt*_tmp3C4;void*_tmp3C5;
struct Cyc_Absyn_DatatypeInfo _tmp3C6;union Cyc_Absyn_DatatypeInfoU _tmp3C7;struct
Cyc_Absyn_Datatypedecl**_tmp3C8;struct Cyc_Absyn_Datatypedecl*_tmp3C9;struct Cyc_List_List*
_tmp3CA;struct Cyc_Core_Opt*_tmp3CB;void*_tmp3CC;struct Cyc_Absyn_DatatypeFieldInfo
_tmp3CD;union Cyc_Absyn_DatatypeFieldInfoU _tmp3CE;struct _tuple3 _tmp3CF;struct Cyc_Absyn_Datatypedecl*
_tmp3D0;struct Cyc_Absyn_Datatypefield*_tmp3D1;struct Cyc_List_List*_tmp3D2;void*
_tmp3D3;struct Cyc_Absyn_DatatypeFieldInfo _tmp3D4;union Cyc_Absyn_DatatypeFieldInfoU
_tmp3D5;struct _tuple3 _tmp3D6;struct Cyc_Absyn_Datatypedecl*_tmp3D7;struct Cyc_Absyn_Datatypefield*
_tmp3D8;struct Cyc_List_List*_tmp3D9;void*_tmp3DA;struct Cyc_Absyn_PtrInfo _tmp3DB;
void*_tmp3DC;struct Cyc_Absyn_Tqual _tmp3DD;struct Cyc_Absyn_PtrAtts _tmp3DE;void*
_tmp3DF;union Cyc_Absyn_Constraint*_tmp3E0;union Cyc_Absyn_Constraint*_tmp3E1;
union Cyc_Absyn_Constraint*_tmp3E2;void*_tmp3E3;struct Cyc_Absyn_PtrInfo _tmp3E4;
void*_tmp3E5;struct Cyc_Absyn_Tqual _tmp3E6;struct Cyc_Absyn_PtrAtts _tmp3E7;void*
_tmp3E8;union Cyc_Absyn_Constraint*_tmp3E9;union Cyc_Absyn_Constraint*_tmp3EA;
union Cyc_Absyn_Constraint*_tmp3EB;void*_tmp3EC;void*_tmp3ED;void*_tmp3EE;void*
_tmp3EF;void*_tmp3F0;void*_tmp3F1;void*_tmp3F2;void*_tmp3F3;void*_tmp3F4;int
_tmp3F5;void*_tmp3F6;int _tmp3F7;void*_tmp3F8;struct Cyc_Absyn_ArrayInfo _tmp3F9;
void*_tmp3FA;struct Cyc_Absyn_Tqual _tmp3FB;struct Cyc_Absyn_Exp*_tmp3FC;union Cyc_Absyn_Constraint*
_tmp3FD;void*_tmp3FE;struct Cyc_Absyn_ArrayInfo _tmp3FF;void*_tmp400;struct Cyc_Absyn_Tqual
_tmp401;struct Cyc_Absyn_Exp*_tmp402;union Cyc_Absyn_Constraint*_tmp403;void*
_tmp404;struct Cyc_Absyn_FnInfo _tmp405;struct Cyc_List_List*_tmp406;struct Cyc_Core_Opt*
_tmp407;void*_tmp408;struct Cyc_List_List*_tmp409;int _tmp40A;struct Cyc_Absyn_VarargInfo*
_tmp40B;struct Cyc_List_List*_tmp40C;struct Cyc_List_List*_tmp40D;void*_tmp40E;
struct Cyc_Absyn_FnInfo _tmp40F;struct Cyc_List_List*_tmp410;struct Cyc_Core_Opt*
_tmp411;void*_tmp412;struct Cyc_List_List*_tmp413;int _tmp414;struct Cyc_Absyn_VarargInfo*
_tmp415;struct Cyc_List_List*_tmp416;struct Cyc_List_List*_tmp417;void*_tmp418;
struct Cyc_List_List*_tmp419;void*_tmp41A;struct Cyc_List_List*_tmp41B;void*
_tmp41C;void*_tmp41D;struct Cyc_List_List*_tmp41E;void*_tmp41F;void*_tmp420;
struct Cyc_List_List*_tmp421;void*_tmp422;void*_tmp423;void*_tmp424;void*_tmp425;
void*_tmp426;void*_tmp427;void*_tmp428;void*_tmp429;void*_tmp42A;void*_tmp42B;
void*_tmp42C;void*_tmp42D;void*_tmp42E;void*_tmp42F;void*_tmp430;struct Cyc_Absyn_Exp*
_tmp431;void*_tmp432;struct Cyc_Absyn_Exp*_tmp433;void*_tmp434;void*_tmp435;void*
_tmp436;void*_tmp437;void*_tmp438;void*_tmp439;_LL2AD: _tmp3A8=_tmp3A7.f1;if(
_tmp3A8 <= (void*)4)goto _LL2AF;if(*((int*)_tmp3A8)!= 0)goto _LL2AF;_tmp3A9=_tmp3A7.f2;
if(_tmp3A9 <= (void*)4)goto _LL2AF;if(*((int*)_tmp3A9)!= 0)goto _LL2AF;_LL2AE: {
const char*_tmpD3E;void*_tmpD3D;(_tmpD3D=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpD3E="typecmp: can only compare closed types",
_tag_dyneither(_tmpD3E,sizeof(char),39))),_tag_dyneither(_tmpD3D,sizeof(void*),0)));}
_LL2AF: _tmp3AA=_tmp3A7.f1;if(_tmp3AA <= (void*)4)goto _LL2B1;if(*((int*)_tmp3AA)!= 
1)goto _LL2B1;_tmp3AB=((struct Cyc_Absyn_VarType_struct*)_tmp3AA)->f1;_tmp3AC=
_tmp3A7.f2;if(_tmp3AC <= (void*)4)goto _LL2B1;if(*((int*)_tmp3AC)!= 1)goto _LL2B1;
_tmp3AD=((struct Cyc_Absyn_VarType_struct*)_tmp3AC)->f1;_LL2B0: return Cyc_Core_intcmp(
_tmp3AD->identity,_tmp3AB->identity);_LL2B1: _tmp3AE=_tmp3A7.f1;if(_tmp3AE <= (
void*)4)goto _LL2B3;if(*((int*)_tmp3AE)!= 10)goto _LL2B3;_tmp3AF=((struct Cyc_Absyn_AggrType_struct*)
_tmp3AE)->f1;_tmp3B0=_tmp3AF.aggr_info;_tmp3B1=_tmp3AF.targs;_tmp3B2=_tmp3A7.f2;
if(_tmp3B2 <= (void*)4)goto _LL2B3;if(*((int*)_tmp3B2)!= 10)goto _LL2B3;_tmp3B3=((
struct Cyc_Absyn_AggrType_struct*)_tmp3B2)->f1;_tmp3B4=_tmp3B3.aggr_info;_tmp3B5=
_tmp3B3.targs;_LL2B2: {struct _tuple2*_tmp43D;struct _tuple12 _tmp43C=Cyc_Absyn_aggr_kinded_name(
_tmp3B0);_tmp43D=_tmp43C.f2;{struct _tuple2*_tmp43F;struct _tuple12 _tmp43E=Cyc_Absyn_aggr_kinded_name(
_tmp3B4);_tmp43F=_tmp43E.f2;{int _tmp440=Cyc_Absyn_qvar_cmp(_tmp43D,_tmp43F);if(
_tmp440 != 0)return _tmp440;else{return Cyc_List_list_cmp(Cyc_Tcutil_typecmp,
_tmp3B1,_tmp3B5);}}}}_LL2B3: _tmp3B6=_tmp3A7.f1;if(_tmp3B6 <= (void*)4)goto _LL2B5;
if(*((int*)_tmp3B6)!= 12)goto _LL2B5;_tmp3B7=((struct Cyc_Absyn_EnumType_struct*)
_tmp3B6)->f1;_tmp3B8=_tmp3A7.f2;if(_tmp3B8 <= (void*)4)goto _LL2B5;if(*((int*)
_tmp3B8)!= 12)goto _LL2B5;_tmp3B9=((struct Cyc_Absyn_EnumType_struct*)_tmp3B8)->f1;
_LL2B4: return Cyc_Absyn_qvar_cmp(_tmp3B7,_tmp3B9);_LL2B5: _tmp3BA=_tmp3A7.f1;if(
_tmp3BA <= (void*)4)goto _LL2B7;if(*((int*)_tmp3BA)!= 13)goto _LL2B7;_tmp3BB=((
struct Cyc_Absyn_AnonEnumType_struct*)_tmp3BA)->f1;_tmp3BC=_tmp3A7.f2;if(_tmp3BC
<= (void*)4)goto _LL2B7;if(*((int*)_tmp3BC)!= 13)goto _LL2B7;_tmp3BD=((struct Cyc_Absyn_AnonEnumType_struct*)
_tmp3BC)->f1;_LL2B6: return((int(*)(int(*cmp)(struct Cyc_Absyn_Enumfield*,struct
Cyc_Absyn_Enumfield*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(
Cyc_Tcutil_enumfield_cmp,_tmp3BB,_tmp3BD);_LL2B7: _tmp3BE=_tmp3A7.f1;if(_tmp3BE <= (
void*)4)goto _LL2B9;if(*((int*)_tmp3BE)!= 2)goto _LL2B9;_tmp3BF=((struct Cyc_Absyn_DatatypeType_struct*)
_tmp3BE)->f1;_tmp3C0=_tmp3BF.datatype_info;if((_tmp3C0.KnownDatatype).tag != 2)
goto _LL2B9;_tmp3C1=(struct Cyc_Absyn_Datatypedecl**)(_tmp3C0.KnownDatatype).val;
_tmp3C2=*_tmp3C1;_tmp3C3=_tmp3BF.targs;_tmp3C4=_tmp3BF.rgn;_tmp3C5=_tmp3A7.f2;
if(_tmp3C5 <= (void*)4)goto _LL2B9;if(*((int*)_tmp3C5)!= 2)goto _LL2B9;_tmp3C6=((
struct Cyc_Absyn_DatatypeType_struct*)_tmp3C5)->f1;_tmp3C7=_tmp3C6.datatype_info;
if((_tmp3C7.KnownDatatype).tag != 2)goto _LL2B9;_tmp3C8=(struct Cyc_Absyn_Datatypedecl**)(
_tmp3C7.KnownDatatype).val;_tmp3C9=*_tmp3C8;_tmp3CA=_tmp3C6.targs;_tmp3CB=
_tmp3C6.rgn;_LL2B8: if(_tmp3C9 == _tmp3C2)return 0;{int _tmp441=Cyc_Absyn_qvar_cmp(
_tmp3C9->name,_tmp3C2->name);if(_tmp441 != 0)return _tmp441;if((unsigned int)
_tmp3CB  && (unsigned int)_tmp3C4){int _tmp442=Cyc_Tcutil_typecmp((void*)_tmp3CB->v,(
void*)_tmp3C4->v);if(_tmp442 != 0)return _tmp442;}else{if((unsigned int)_tmp3CB)
return - 1;else{return 1;}}return Cyc_List_list_cmp(Cyc_Tcutil_typecmp,_tmp3CA,
_tmp3C3);}_LL2B9: _tmp3CC=_tmp3A7.f1;if(_tmp3CC <= (void*)4)goto _LL2BB;if(*((int*)
_tmp3CC)!= 3)goto _LL2BB;_tmp3CD=((struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp3CC)->f1;_tmp3CE=_tmp3CD.field_info;if((_tmp3CE.KnownDatatypefield).tag != 2)
goto _LL2BB;_tmp3CF=(struct _tuple3)(_tmp3CE.KnownDatatypefield).val;_tmp3D0=
_tmp3CF.f1;_tmp3D1=_tmp3CF.f2;_tmp3D2=_tmp3CD.targs;_tmp3D3=_tmp3A7.f2;if(
_tmp3D3 <= (void*)4)goto _LL2BB;if(*((int*)_tmp3D3)!= 3)goto _LL2BB;_tmp3D4=((
struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp3D3)->f1;_tmp3D5=_tmp3D4.field_info;
if((_tmp3D5.KnownDatatypefield).tag != 2)goto _LL2BB;_tmp3D6=(struct _tuple3)(
_tmp3D5.KnownDatatypefield).val;_tmp3D7=_tmp3D6.f1;_tmp3D8=_tmp3D6.f2;_tmp3D9=
_tmp3D4.targs;_LL2BA: if(_tmp3D7 == _tmp3D0)return 0;{int _tmp443=Cyc_Absyn_qvar_cmp(
_tmp3D0->name,_tmp3D7->name);if(_tmp443 != 0)return _tmp443;{int _tmp444=Cyc_Absyn_qvar_cmp(
_tmp3D1->name,_tmp3D8->name);if(_tmp444 != 0)return _tmp444;return Cyc_List_list_cmp(
Cyc_Tcutil_typecmp,_tmp3D9,_tmp3D2);}}_LL2BB: _tmp3DA=_tmp3A7.f1;if(_tmp3DA <= (
void*)4)goto _LL2BD;if(*((int*)_tmp3DA)!= 4)goto _LL2BD;_tmp3DB=((struct Cyc_Absyn_PointerType_struct*)
_tmp3DA)->f1;_tmp3DC=_tmp3DB.elt_typ;_tmp3DD=_tmp3DB.elt_tq;_tmp3DE=_tmp3DB.ptr_atts;
_tmp3DF=_tmp3DE.rgn;_tmp3E0=_tmp3DE.nullable;_tmp3E1=_tmp3DE.bounds;_tmp3E2=
_tmp3DE.zero_term;_tmp3E3=_tmp3A7.f2;if(_tmp3E3 <= (void*)4)goto _LL2BD;if(*((int*)
_tmp3E3)!= 4)goto _LL2BD;_tmp3E4=((struct Cyc_Absyn_PointerType_struct*)_tmp3E3)->f1;
_tmp3E5=_tmp3E4.elt_typ;_tmp3E6=_tmp3E4.elt_tq;_tmp3E7=_tmp3E4.ptr_atts;_tmp3E8=
_tmp3E7.rgn;_tmp3E9=_tmp3E7.nullable;_tmp3EA=_tmp3E7.bounds;_tmp3EB=_tmp3E7.zero_term;
_LL2BC: {int _tmp445=Cyc_Tcutil_typecmp(_tmp3E5,_tmp3DC);if(_tmp445 != 0)return
_tmp445;{int _tmp446=Cyc_Tcutil_typecmp(_tmp3E8,_tmp3DF);if(_tmp446 != 0)return
_tmp446;{int _tmp447=Cyc_Tcutil_tqual_cmp(_tmp3E6,_tmp3DD);if(_tmp447 != 0)return
_tmp447;{int _tmp448=Cyc_Tcutil_conrefs_cmp(Cyc_Tcutil_boundscmp,_tmp3EA,_tmp3E1);
if(_tmp448 != 0)return _tmp448;{int _tmp449=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*
x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_conrefs_cmp)(Cyc_Core_intcmp,_tmp3EB,
_tmp3E2);if(_tmp449 != 0)return _tmp449;{void*_tmp44A=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,
_tmp3EA);_LL2E2: if((int)_tmp44A != 0)goto _LL2E4;_LL2E3: return 0;_LL2E4:;_LL2E5:
goto _LL2E1;_LL2E1:;}return((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*
x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_conrefs_cmp)(Cyc_Core_intcmp,_tmp3E9,
_tmp3E0);}}}}}_LL2BD: _tmp3EC=_tmp3A7.f1;if(_tmp3EC <= (void*)4)goto _LL2BF;if(*((
int*)_tmp3EC)!= 5)goto _LL2BF;_tmp3ED=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp3EC)->f1;_tmp3EE=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp3EC)->f2;
_tmp3EF=_tmp3A7.f2;if(_tmp3EF <= (void*)4)goto _LL2BF;if(*((int*)_tmp3EF)!= 5)goto
_LL2BF;_tmp3F0=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp3EF)->f1;_tmp3F1=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp3EF)->f2;_LL2BE: if(_tmp3F0 != _tmp3ED)
return Cyc_Core_intcmp((int)((unsigned int)_tmp3F0),(int)((unsigned int)_tmp3ED));
if(_tmp3F1 != _tmp3EE)return Cyc_Core_intcmp((int)((unsigned int)_tmp3F1),(int)((
unsigned int)_tmp3EE));return 0;_LL2BF: _tmp3F2=_tmp3A7.f1;if((int)_tmp3F2 != 1)
goto _LL2C1;_tmp3F3=_tmp3A7.f2;if((int)_tmp3F3 != 1)goto _LL2C1;_LL2C0: return 0;
_LL2C1: _tmp3F4=_tmp3A7.f1;if(_tmp3F4 <= (void*)4)goto _LL2C3;if(*((int*)_tmp3F4)!= 
6)goto _LL2C3;_tmp3F5=((struct Cyc_Absyn_DoubleType_struct*)_tmp3F4)->f1;_tmp3F6=
_tmp3A7.f2;if(_tmp3F6 <= (void*)4)goto _LL2C3;if(*((int*)_tmp3F6)!= 6)goto _LL2C3;
_tmp3F7=((struct Cyc_Absyn_DoubleType_struct*)_tmp3F6)->f1;_LL2C2: if(_tmp3F5 == 
_tmp3F7)return 0;else{if(_tmp3F5)return - 1;else{return 1;}}_LL2C3: _tmp3F8=_tmp3A7.f1;
if(_tmp3F8 <= (void*)4)goto _LL2C5;if(*((int*)_tmp3F8)!= 7)goto _LL2C5;_tmp3F9=((
struct Cyc_Absyn_ArrayType_struct*)_tmp3F8)->f1;_tmp3FA=_tmp3F9.elt_type;_tmp3FB=
_tmp3F9.tq;_tmp3FC=_tmp3F9.num_elts;_tmp3FD=_tmp3F9.zero_term;_tmp3FE=_tmp3A7.f2;
if(_tmp3FE <= (void*)4)goto _LL2C5;if(*((int*)_tmp3FE)!= 7)goto _LL2C5;_tmp3FF=((
struct Cyc_Absyn_ArrayType_struct*)_tmp3FE)->f1;_tmp400=_tmp3FF.elt_type;_tmp401=
_tmp3FF.tq;_tmp402=_tmp3FF.num_elts;_tmp403=_tmp3FF.zero_term;_LL2C4: {int
_tmp44B=Cyc_Tcutil_tqual_cmp(_tmp401,_tmp3FB);if(_tmp44B != 0)return _tmp44B;{int
_tmp44C=Cyc_Tcutil_typecmp(_tmp400,_tmp3FA);if(_tmp44C != 0)return _tmp44C;{int
_tmp44D=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*
y))Cyc_Tcutil_conrefs_cmp)(Cyc_Core_intcmp,_tmp3FD,_tmp403);if(_tmp44D != 0)
return _tmp44D;if(_tmp3FC == _tmp402)return 0;if(_tmp3FC == 0  || _tmp402 == 0){const
char*_tmpD41;void*_tmpD40;(_tmpD40=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpD41="missing expression in array index",
_tag_dyneither(_tmpD41,sizeof(char),34))),_tag_dyneither(_tmpD40,sizeof(void*),0)));}
return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*
a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,_tmp3FC,
_tmp402);}}}_LL2C5: _tmp404=_tmp3A7.f1;if(_tmp404 <= (void*)4)goto _LL2C7;if(*((int*)
_tmp404)!= 8)goto _LL2C7;_tmp405=((struct Cyc_Absyn_FnType_struct*)_tmp404)->f1;
_tmp406=_tmp405.tvars;_tmp407=_tmp405.effect;_tmp408=_tmp405.ret_typ;_tmp409=
_tmp405.args;_tmp40A=_tmp405.c_varargs;_tmp40B=_tmp405.cyc_varargs;_tmp40C=
_tmp405.rgn_po;_tmp40D=_tmp405.attributes;_tmp40E=_tmp3A7.f2;if(_tmp40E <= (void*)
4)goto _LL2C7;if(*((int*)_tmp40E)!= 8)goto _LL2C7;_tmp40F=((struct Cyc_Absyn_FnType_struct*)
_tmp40E)->f1;_tmp410=_tmp40F.tvars;_tmp411=_tmp40F.effect;_tmp412=_tmp40F.ret_typ;
_tmp413=_tmp40F.args;_tmp414=_tmp40F.c_varargs;_tmp415=_tmp40F.cyc_varargs;
_tmp416=_tmp40F.rgn_po;_tmp417=_tmp40F.attributes;_LL2C6: {const char*_tmpD44;
void*_tmpD43;(_tmpD43=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmpD44="typecmp: function types not handled",_tag_dyneither(
_tmpD44,sizeof(char),36))),_tag_dyneither(_tmpD43,sizeof(void*),0)));}_LL2C7:
_tmp418=_tmp3A7.f1;if(_tmp418 <= (void*)4)goto _LL2C9;if(*((int*)_tmp418)!= 9)goto
_LL2C9;_tmp419=((struct Cyc_Absyn_TupleType_struct*)_tmp418)->f1;_tmp41A=_tmp3A7.f2;
if(_tmp41A <= (void*)4)goto _LL2C9;if(*((int*)_tmp41A)!= 9)goto _LL2C9;_tmp41B=((
struct Cyc_Absyn_TupleType_struct*)_tmp41A)->f1;_LL2C8: return((int(*)(int(*cmp)(
struct _tuple11*,struct _tuple11*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))
Cyc_List_list_cmp)(Cyc_Tcutil_tqual_type_cmp,_tmp41B,_tmp419);_LL2C9: _tmp41C=
_tmp3A7.f1;if(_tmp41C <= (void*)4)goto _LL2CB;if(*((int*)_tmp41C)!= 11)goto _LL2CB;
_tmp41D=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)_tmp41C)->f1;_tmp41E=((
struct Cyc_Absyn_AnonAggrType_struct*)_tmp41C)->f2;_tmp41F=_tmp3A7.f2;if(_tmp41F
<= (void*)4)goto _LL2CB;if(*((int*)_tmp41F)!= 11)goto _LL2CB;_tmp420=(void*)((
struct Cyc_Absyn_AnonAggrType_struct*)_tmp41F)->f1;_tmp421=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp41F)->f2;_LL2CA: if(_tmp420 != _tmp41D){if(_tmp420 == (void*)0)return - 1;else{
return 1;}}return((int(*)(int(*cmp)(struct Cyc_Absyn_Aggrfield*,struct Cyc_Absyn_Aggrfield*),
struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_aggrfield_cmp,
_tmp421,_tmp41E);_LL2CB: _tmp422=_tmp3A7.f1;if(_tmp422 <= (void*)4)goto _LL2CD;if(*((
int*)_tmp422)!= 14)goto _LL2CD;_tmp423=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp422)->f1;_tmp424=_tmp3A7.f2;if(_tmp424 <= (void*)4)goto _LL2CD;if(*((int*)
_tmp424)!= 14)goto _LL2CD;_tmp425=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp424)->f1;_LL2CC: return Cyc_Tcutil_typecmp(_tmp423,_tmp425);_LL2CD: _tmp426=
_tmp3A7.f1;if(_tmp426 <= (void*)4)goto _LL2CF;if(*((int*)_tmp426)!= 15)goto _LL2CF;
_tmp427=(void*)((struct Cyc_Absyn_DynRgnType_struct*)_tmp426)->f1;_tmp428=(void*)((
struct Cyc_Absyn_DynRgnType_struct*)_tmp426)->f2;_tmp429=_tmp3A7.f2;if(_tmp429 <= (
void*)4)goto _LL2CF;if(*((int*)_tmp429)!= 15)goto _LL2CF;_tmp42A=(void*)((struct
Cyc_Absyn_DynRgnType_struct*)_tmp429)->f1;_tmp42B=(void*)((struct Cyc_Absyn_DynRgnType_struct*)
_tmp429)->f2;_LL2CE: {int _tmp452=Cyc_Tcutil_typecmp(_tmp427,_tmp42A);if(_tmp452
!= 0)return _tmp452;else{return Cyc_Tcutil_typecmp(_tmp428,_tmp42B);}}_LL2CF:
_tmp42C=_tmp3A7.f1;if(_tmp42C <= (void*)4)goto _LL2D1;if(*((int*)_tmp42C)!= 18)
goto _LL2D1;_tmp42D=(void*)((struct Cyc_Absyn_TagType_struct*)_tmp42C)->f1;_tmp42E=
_tmp3A7.f2;if(_tmp42E <= (void*)4)goto _LL2D1;if(*((int*)_tmp42E)!= 18)goto _LL2D1;
_tmp42F=(void*)((struct Cyc_Absyn_TagType_struct*)_tmp42E)->f1;_LL2D0: return Cyc_Tcutil_typecmp(
_tmp42D,_tmp42F);_LL2D1: _tmp430=_tmp3A7.f1;if(_tmp430 <= (void*)4)goto _LL2D3;if(*((
int*)_tmp430)!= 17)goto _LL2D3;_tmp431=((struct Cyc_Absyn_ValueofType_struct*)
_tmp430)->f1;_tmp432=_tmp3A7.f2;if(_tmp432 <= (void*)4)goto _LL2D3;if(*((int*)
_tmp432)!= 17)goto _LL2D3;_tmp433=((struct Cyc_Absyn_ValueofType_struct*)_tmp432)->f1;
_LL2D2: return Cyc_Evexp_const_exp_cmp(_tmp431,_tmp433);_LL2D3: _tmp434=_tmp3A7.f1;
if(_tmp434 <= (void*)4)goto _LL2D5;if(*((int*)_tmp434)!= 20)goto _LL2D5;_LL2D4: goto
_LL2D6;_LL2D5: _tmp435=_tmp3A7.f2;if(_tmp435 <= (void*)4)goto _LL2D7;if(*((int*)
_tmp435)!= 20)goto _LL2D7;_LL2D6: goto _LL2D8;_LL2D7: _tmp436=_tmp3A7.f1;if(_tmp436
<= (void*)4)goto _LL2D9;if(*((int*)_tmp436)!= 19)goto _LL2D9;_LL2D8: goto _LL2DA;
_LL2D9: _tmp437=_tmp3A7.f1;if(_tmp437 <= (void*)4)goto _LL2DB;if(*((int*)_tmp437)!= 
21)goto _LL2DB;_LL2DA: goto _LL2DC;_LL2DB: _tmp438=_tmp3A7.f2;if(_tmp438 <= (void*)4)
goto _LL2DD;if(*((int*)_tmp438)!= 21)goto _LL2DD;_LL2DC: goto _LL2DE;_LL2DD: _tmp439=
_tmp3A7.f2;if(_tmp439 <= (void*)4)goto _LL2DF;if(*((int*)_tmp439)!= 19)goto _LL2DF;
_LL2DE: {const char*_tmpD47;void*_tmpD46;(_tmpD46=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpD47="typecmp: effects not handled",
_tag_dyneither(_tmpD47,sizeof(char),29))),_tag_dyneither(_tmpD46,sizeof(void*),0)));}
_LL2DF:;_LL2E0: {const char*_tmpD4A;void*_tmpD49;(_tmpD49=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpD4A="Unmatched case in typecmp",
_tag_dyneither(_tmpD4A,sizeof(char),26))),_tag_dyneither(_tmpD49,sizeof(void*),0)));}
_LL2AC:;}}}int Cyc_Tcutil_is_arithmetic_type(void*t);int Cyc_Tcutil_is_arithmetic_type(
void*t){void*_tmp457=Cyc_Tcutil_compress(t);_LL2E7: if(_tmp457 <= (void*)4)goto
_LL2E9;if(*((int*)_tmp457)!= 5)goto _LL2E9;_LL2E8: goto _LL2EA;_LL2E9: if((int)
_tmp457 != 1)goto _LL2EB;_LL2EA: goto _LL2EC;_LL2EB: if(_tmp457 <= (void*)4)goto _LL2F1;
if(*((int*)_tmp457)!= 6)goto _LL2ED;_LL2EC: goto _LL2EE;_LL2ED: if(*((int*)_tmp457)
!= 12)goto _LL2EF;_LL2EE: goto _LL2F0;_LL2EF: if(*((int*)_tmp457)!= 13)goto _LL2F1;
_LL2F0: return 1;_LL2F1:;_LL2F2: return 0;_LL2E6:;}int Cyc_Tcutil_will_lose_precision(
void*t1,void*t2);int Cyc_Tcutil_will_lose_precision(void*t1,void*t2){t1=Cyc_Tcutil_compress(
t1);t2=Cyc_Tcutil_compress(t2);{struct _tuple0 _tmpD4B;struct _tuple0 _tmp459=(
_tmpD4B.f1=t1,((_tmpD4B.f2=t2,_tmpD4B)));void*_tmp45A;int _tmp45B;void*_tmp45C;
int _tmp45D;void*_tmp45E;void*_tmp45F;void*_tmp460;void*_tmp461;void*_tmp462;void*
_tmp463;void*_tmp464;void*_tmp465;void*_tmp466;void*_tmp467;void*_tmp468;void*
_tmp469;void*_tmp46A;void*_tmp46B;void*_tmp46C;void*_tmp46D;void*_tmp46E;void*
_tmp46F;void*_tmp470;void*_tmp471;void*_tmp472;void*_tmp473;void*_tmp474;void*
_tmp475;void*_tmp476;void*_tmp477;void*_tmp478;void*_tmp479;void*_tmp47A;void*
_tmp47B;void*_tmp47C;void*_tmp47D;void*_tmp47E;void*_tmp47F;void*_tmp480;void*
_tmp481;void*_tmp482;void*_tmp483;void*_tmp484;void*_tmp485;void*_tmp486;void*
_tmp487;void*_tmp488;void*_tmp489;void*_tmp48A;void*_tmp48B;void*_tmp48C;void*
_tmp48D;void*_tmp48E;void*_tmp48F;void*_tmp490;void*_tmp491;void*_tmp492;void*
_tmp493;void*_tmp494;void*_tmp495;_LL2F4: _tmp45A=_tmp459.f1;if(_tmp45A <= (void*)
4)goto _LL2F6;if(*((int*)_tmp45A)!= 6)goto _LL2F6;_tmp45B=((struct Cyc_Absyn_DoubleType_struct*)
_tmp45A)->f1;_tmp45C=_tmp459.f2;if(_tmp45C <= (void*)4)goto _LL2F6;if(*((int*)
_tmp45C)!= 6)goto _LL2F6;_tmp45D=((struct Cyc_Absyn_DoubleType_struct*)_tmp45C)->f1;
_LL2F5: return !_tmp45D  && _tmp45B;_LL2F6: _tmp45E=_tmp459.f1;if(_tmp45E <= (void*)4)
goto _LL2F8;if(*((int*)_tmp45E)!= 6)goto _LL2F8;_tmp45F=_tmp459.f2;if((int)_tmp45F
!= 1)goto _LL2F8;_LL2F7: goto _LL2F9;_LL2F8: _tmp460=_tmp459.f1;if(_tmp460 <= (void*)
4)goto _LL2FA;if(*((int*)_tmp460)!= 6)goto _LL2FA;_tmp461=_tmp459.f2;if(_tmp461 <= (
void*)4)goto _LL2FA;if(*((int*)_tmp461)!= 5)goto _LL2FA;_LL2F9: goto _LL2FB;_LL2FA:
_tmp462=_tmp459.f1;if(_tmp462 <= (void*)4)goto _LL2FC;if(*((int*)_tmp462)!= 6)goto
_LL2FC;_tmp463=_tmp459.f2;if(_tmp463 <= (void*)4)goto _LL2FC;if(*((int*)_tmp463)!= 
18)goto _LL2FC;_LL2FB: goto _LL2FD;_LL2FC: _tmp464=_tmp459.f1;if((int)_tmp464 != 1)
goto _LL2FE;_tmp465=_tmp459.f2;if(_tmp465 <= (void*)4)goto _LL2FE;if(*((int*)
_tmp465)!= 18)goto _LL2FE;_LL2FD: goto _LL2FF;_LL2FE: _tmp466=_tmp459.f1;if((int)
_tmp466 != 1)goto _LL300;_tmp467=_tmp459.f2;if(_tmp467 <= (void*)4)goto _LL300;if(*((
int*)_tmp467)!= 5)goto _LL300;_LL2FF: return 1;_LL300: _tmp468=_tmp459.f1;if(_tmp468
<= (void*)4)goto _LL302;if(*((int*)_tmp468)!= 5)goto _LL302;_tmp469=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp468)->f2;if((int)_tmp469 != 4)goto _LL302;
_tmp46A=_tmp459.f2;if(_tmp46A <= (void*)4)goto _LL302;if(*((int*)_tmp46A)!= 5)goto
_LL302;_tmp46B=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp46A)->f2;if((int)
_tmp46B != 4)goto _LL302;_LL301: return 0;_LL302: _tmp46C=_tmp459.f1;if(_tmp46C <= (
void*)4)goto _LL304;if(*((int*)_tmp46C)!= 5)goto _LL304;_tmp46D=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp46C)->f2;if((int)_tmp46D != 4)goto _LL304;_LL303: return 1;_LL304: _tmp46E=
_tmp459.f1;if(_tmp46E <= (void*)4)goto _LL306;if(*((int*)_tmp46E)!= 5)goto _LL306;
_tmp46F=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp46E)->f2;if((int)_tmp46F != 
3)goto _LL306;_tmp470=_tmp459.f2;if(_tmp470 <= (void*)4)goto _LL306;if(*((int*)
_tmp470)!= 5)goto _LL306;_tmp471=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp470)->f2;
if((int)_tmp471 != 2)goto _LL306;_LL305: goto _LL307;_LL306: _tmp472=_tmp459.f1;if(
_tmp472 <= (void*)4)goto _LL308;if(*((int*)_tmp472)!= 5)goto _LL308;_tmp473=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp472)->f2;if((int)_tmp473 != 2)goto _LL308;
_tmp474=_tmp459.f2;if(_tmp474 <= (void*)4)goto _LL308;if(*((int*)_tmp474)!= 5)goto
_LL308;_tmp475=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp474)->f2;if((int)
_tmp475 != 3)goto _LL308;_LL307: return 0;_LL308: _tmp476=_tmp459.f1;if(_tmp476 <= (
void*)4)goto _LL30A;if(*((int*)_tmp476)!= 5)goto _LL30A;_tmp477=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp476)->f2;if((int)_tmp477 != 3)goto _LL30A;_tmp478=_tmp459.f2;if((int)_tmp478 != 
1)goto _LL30A;_LL309: goto _LL30B;_LL30A: _tmp479=_tmp459.f1;if(_tmp479 <= (void*)4)
goto _LL30C;if(*((int*)_tmp479)!= 5)goto _LL30C;_tmp47A=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp479)->f2;if((int)_tmp47A != 2)goto _LL30C;_tmp47B=_tmp459.f2;if((int)_tmp47B != 
1)goto _LL30C;_LL30B: goto _LL30D;_LL30C: _tmp47C=_tmp459.f1;if(_tmp47C <= (void*)4)
goto _LL30E;if(*((int*)_tmp47C)!= 5)goto _LL30E;_tmp47D=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp47C)->f2;if((int)_tmp47D != 3)goto _LL30E;_tmp47E=_tmp459.f2;if(_tmp47E <= (
void*)4)goto _LL30E;if(*((int*)_tmp47E)!= 5)goto _LL30E;_tmp47F=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp47E)->f2;if((int)_tmp47F != 1)goto _LL30E;_LL30D: goto _LL30F;_LL30E: _tmp480=
_tmp459.f1;if(_tmp480 <= (void*)4)goto _LL310;if(*((int*)_tmp480)!= 5)goto _LL310;
_tmp481=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp480)->f2;if((int)_tmp481 != 
2)goto _LL310;_tmp482=_tmp459.f2;if(_tmp482 <= (void*)4)goto _LL310;if(*((int*)
_tmp482)!= 5)goto _LL310;_tmp483=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp482)->f2;
if((int)_tmp483 != 1)goto _LL310;_LL30F: goto _LL311;_LL310: _tmp484=_tmp459.f1;if(
_tmp484 <= (void*)4)goto _LL312;if(*((int*)_tmp484)!= 18)goto _LL312;_tmp485=
_tmp459.f2;if(_tmp485 <= (void*)4)goto _LL312;if(*((int*)_tmp485)!= 5)goto _LL312;
_tmp486=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp485)->f2;if((int)_tmp486 != 
1)goto _LL312;_LL311: goto _LL313;_LL312: _tmp487=_tmp459.f1;if(_tmp487 <= (void*)4)
goto _LL314;if(*((int*)_tmp487)!= 5)goto _LL314;_tmp488=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp487)->f2;if((int)_tmp488 != 3)goto _LL314;_tmp489=_tmp459.f2;if(_tmp489 <= (
void*)4)goto _LL314;if(*((int*)_tmp489)!= 5)goto _LL314;_tmp48A=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp489)->f2;if((int)_tmp48A != 0)goto _LL314;_LL313: goto _LL315;_LL314: _tmp48B=
_tmp459.f1;if(_tmp48B <= (void*)4)goto _LL316;if(*((int*)_tmp48B)!= 5)goto _LL316;
_tmp48C=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp48B)->f2;if((int)_tmp48C != 
2)goto _LL316;_tmp48D=_tmp459.f2;if(_tmp48D <= (void*)4)goto _LL316;if(*((int*)
_tmp48D)!= 5)goto _LL316;_tmp48E=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp48D)->f2;
if((int)_tmp48E != 0)goto _LL316;_LL315: goto _LL317;_LL316: _tmp48F=_tmp459.f1;if(
_tmp48F <= (void*)4)goto _LL318;if(*((int*)_tmp48F)!= 5)goto _LL318;_tmp490=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp48F)->f2;if((int)_tmp490 != 1)goto _LL318;
_tmp491=_tmp459.f2;if(_tmp491 <= (void*)4)goto _LL318;if(*((int*)_tmp491)!= 5)goto
_LL318;_tmp492=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp491)->f2;if((int)
_tmp492 != 0)goto _LL318;_LL317: goto _LL319;_LL318: _tmp493=_tmp459.f1;if(_tmp493 <= (
void*)4)goto _LL31A;if(*((int*)_tmp493)!= 18)goto _LL31A;_tmp494=_tmp459.f2;if(
_tmp494 <= (void*)4)goto _LL31A;if(*((int*)_tmp494)!= 5)goto _LL31A;_tmp495=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp494)->f2;if((int)_tmp495 != 0)goto _LL31A;
_LL319: return 1;_LL31A:;_LL31B: return 0;_LL2F3:;}}int Cyc_Tcutil_coerce_list(struct
Cyc_Tcenv_Tenv*te,void*t,struct Cyc_List_List*es);int Cyc_Tcutil_coerce_list(
struct Cyc_Tcenv_Tenv*te,void*t,struct Cyc_List_List*es){struct _RegionHandle*
_tmp496=Cyc_Tcenv_get_fnrgn(te);{struct Cyc_Core_Opt*max_arith_type=0;{struct Cyc_List_List*
el=es;for(0;el != 0;el=el->tl){void*t1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt))->v);if(Cyc_Tcutil_is_arithmetic_type(
t1)){if(max_arith_type == 0  || Cyc_Tcutil_will_lose_precision(t1,(void*)
max_arith_type->v)){struct Cyc_Core_Opt*_tmpD4C;max_arith_type=((_tmpD4C=
_region_malloc(_tmp496,sizeof(*_tmpD4C)),((_tmpD4C->v=(void*)t1,_tmpD4C))));}}}}
if(max_arith_type != 0){if(!Cyc_Tcutil_unify(t,(void*)max_arith_type->v))return 0;}}{
struct Cyc_List_List*el=es;for(0;el != 0;el=el->tl){if(!Cyc_Tcutil_coerce_assign(
te,(struct Cyc_Absyn_Exp*)el->hd,t)){{const char*_tmpD51;void*_tmpD50[2];struct Cyc_String_pa_struct
_tmpD4F;struct Cyc_String_pa_struct _tmpD4E;(_tmpD4E.tag=0,((_tmpD4E.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt))->v)),((_tmpD4F.tag=0,((
_tmpD4F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((
_tmpD50[0]=& _tmpD4F,((_tmpD50[1]=& _tmpD4E,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)
el->hd)->loc,((_tmpD51="type mismatch: expecting %s but found %s",_tag_dyneither(
_tmpD51,sizeof(char),41))),_tag_dyneither(_tmpD50,sizeof(void*),2)))))))))))));}
return 0;}}}return 1;}int Cyc_Tcutil_coerce_to_bool(struct Cyc_Tcenv_Tenv*te,struct
Cyc_Absyn_Exp*e);int Cyc_Tcutil_coerce_to_bool(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*
e){if(!Cyc_Tcutil_coerce_sint_typ(te,e)){void*_tmp49C=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v);_LL31D: if(_tmp49C <= (void*)4)goto
_LL31F;if(*((int*)_tmp49C)!= 4)goto _LL31F;_LL31E: Cyc_Tcutil_unchecked_cast(te,e,
Cyc_Absyn_uint_typ,(void*)3);goto _LL31C;_LL31F:;_LL320: return 0;_LL31C:;}return 1;}
int Cyc_Tcutil_is_integral_type(void*t);int Cyc_Tcutil_is_integral_type(void*t){
void*_tmp49D=Cyc_Tcutil_compress(t);_LL322: if(_tmp49D <= (void*)4)goto _LL32A;if(*((
int*)_tmp49D)!= 5)goto _LL324;_LL323: goto _LL325;_LL324: if(*((int*)_tmp49D)!= 18)
goto _LL326;_LL325: goto _LL327;_LL326: if(*((int*)_tmp49D)!= 12)goto _LL328;_LL327:
goto _LL329;_LL328: if(*((int*)_tmp49D)!= 13)goto _LL32A;_LL329: return 1;_LL32A:;
_LL32B: return 0;_LL321:;}int Cyc_Tcutil_coerce_uint_typ(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Absyn_Exp*e);int Cyc_Tcutil_coerce_uint_typ(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Absyn_Exp*e){if(Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,Cyc_Absyn_uint_typ))return 1;if(Cyc_Tcutil_is_integral_type((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)){if(Cyc_Tcutil_will_lose_precision((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_Absyn_uint_typ)){const
char*_tmpD54;void*_tmpD53;(_tmpD53=0,Cyc_Tcutil_warn(e->loc,((_tmpD54="integral size mismatch; conversion supplied",
_tag_dyneither(_tmpD54,sizeof(char),44))),_tag_dyneither(_tmpD53,sizeof(void*),0)));}
Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_uint_typ,(void*)1);return 1;}return 0;}
int Cyc_Tcutil_coerce_sint_typ(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e);int
Cyc_Tcutil_coerce_sint_typ(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){if(Cyc_Tcutil_unify((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_Absyn_sint_typ))return 1;
if(Cyc_Tcutil_is_integral_type((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)){
if(Cyc_Tcutil_will_lose_precision((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,
Cyc_Absyn_sint_typ)){const char*_tmpD57;void*_tmpD56;(_tmpD56=0,Cyc_Tcutil_warn(e->loc,((
_tmpD57="integral size mismatch; conversion supplied",_tag_dyneither(_tmpD57,
sizeof(char),44))),_tag_dyneither(_tmpD56,sizeof(void*),0)));}Cyc_Tcutil_unchecked_cast(
te,e,Cyc_Absyn_sint_typ,(void*)1);return 1;}return 0;}int Cyc_Tcutil_silent_castable(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void*t1,void*t2);int Cyc_Tcutil_silent_castable(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void*t1,void*t2){t1=Cyc_Tcutil_compress(
t1);t2=Cyc_Tcutil_compress(t2);{struct _tuple0 _tmpD58;struct _tuple0 _tmp4A3=(
_tmpD58.f1=t1,((_tmpD58.f2=t2,_tmpD58)));void*_tmp4A4;struct Cyc_Absyn_PtrInfo
_tmp4A5;void*_tmp4A6;struct Cyc_Absyn_PtrInfo _tmp4A7;void*_tmp4A8;struct Cyc_Absyn_DatatypeInfo
_tmp4A9;union Cyc_Absyn_DatatypeInfoU _tmp4AA;struct Cyc_Absyn_Datatypedecl**
_tmp4AB;struct Cyc_Absyn_Datatypedecl*_tmp4AC;struct Cyc_List_List*_tmp4AD;struct
Cyc_Core_Opt*_tmp4AE;struct Cyc_Core_Opt _tmp4AF;void*_tmp4B0;void*_tmp4B1;struct
Cyc_Absyn_DatatypeInfo _tmp4B2;union Cyc_Absyn_DatatypeInfoU _tmp4B3;struct Cyc_Absyn_Datatypedecl**
_tmp4B4;struct Cyc_Absyn_Datatypedecl*_tmp4B5;struct Cyc_List_List*_tmp4B6;struct
Cyc_Core_Opt*_tmp4B7;struct Cyc_Core_Opt _tmp4B8;void*_tmp4B9;void*_tmp4BA;struct
Cyc_Absyn_ArrayInfo _tmp4BB;void*_tmp4BC;struct Cyc_Absyn_Tqual _tmp4BD;struct Cyc_Absyn_Exp*
_tmp4BE;union Cyc_Absyn_Constraint*_tmp4BF;void*_tmp4C0;struct Cyc_Absyn_ArrayInfo
_tmp4C1;void*_tmp4C2;struct Cyc_Absyn_Tqual _tmp4C3;struct Cyc_Absyn_Exp*_tmp4C4;
union Cyc_Absyn_Constraint*_tmp4C5;void*_tmp4C6;struct Cyc_Absyn_DatatypeFieldInfo
_tmp4C7;union Cyc_Absyn_DatatypeFieldInfoU _tmp4C8;struct _tuple3 _tmp4C9;struct Cyc_Absyn_Datatypedecl*
_tmp4CA;struct Cyc_Absyn_Datatypefield*_tmp4CB;struct Cyc_List_List*_tmp4CC;void*
_tmp4CD;struct Cyc_Absyn_DatatypeInfo _tmp4CE;union Cyc_Absyn_DatatypeInfoU _tmp4CF;
struct Cyc_Absyn_Datatypedecl**_tmp4D0;struct Cyc_Absyn_Datatypedecl*_tmp4D1;
struct Cyc_List_List*_tmp4D2;void*_tmp4D3;struct Cyc_Absyn_PtrInfo _tmp4D4;void*
_tmp4D5;struct Cyc_Absyn_Tqual _tmp4D6;struct Cyc_Absyn_PtrAtts _tmp4D7;void*_tmp4D8;
union Cyc_Absyn_Constraint*_tmp4D9;union Cyc_Absyn_Constraint*_tmp4DA;union Cyc_Absyn_Constraint*
_tmp4DB;void*_tmp4DC;struct Cyc_Absyn_DatatypeInfo _tmp4DD;union Cyc_Absyn_DatatypeInfoU
_tmp4DE;struct Cyc_Absyn_Datatypedecl**_tmp4DF;struct Cyc_Absyn_Datatypedecl*
_tmp4E0;struct Cyc_List_List*_tmp4E1;struct Cyc_Core_Opt*_tmp4E2;void*_tmp4E3;void*
_tmp4E4;_LL32D: _tmp4A4=_tmp4A3.f1;if(_tmp4A4 <= (void*)4)goto _LL32F;if(*((int*)
_tmp4A4)!= 4)goto _LL32F;_tmp4A5=((struct Cyc_Absyn_PointerType_struct*)_tmp4A4)->f1;
_tmp4A6=_tmp4A3.f2;if(_tmp4A6 <= (void*)4)goto _LL32F;if(*((int*)_tmp4A6)!= 4)goto
_LL32F;_tmp4A7=((struct Cyc_Absyn_PointerType_struct*)_tmp4A6)->f1;_LL32E: {int
okay=1;if(!((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*
y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,(_tmp4A5.ptr_atts).nullable,(
_tmp4A7.ptr_atts).nullable))okay=!((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
0,(_tmp4A5.ptr_atts).nullable);if(!Cyc_Tcutil_unify_conrefs(Cyc_Tcutil_unify_it_bounds,(
_tmp4A5.ptr_atts).bounds,(_tmp4A7.ptr_atts).bounds)){struct _tuple0 _tmpD59;struct
_tuple0 _tmp4E6=(_tmpD59.f1=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,(_tmp4A5.ptr_atts).bounds),((
_tmpD59.f2=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,(_tmp4A7.ptr_atts).bounds),
_tmpD59)));void*_tmp4E7;void*_tmp4E8;void*_tmp4E9;void*_tmp4EA;void*_tmp4EB;
struct Cyc_Absyn_Exp*_tmp4EC;void*_tmp4ED;struct Cyc_Absyn_Exp*_tmp4EE;void*
_tmp4EF;void*_tmp4F0;struct Cyc_Absyn_Exp*_tmp4F1;_LL33C: _tmp4E7=_tmp4E6.f1;if(
_tmp4E7 <= (void*)1)goto _LL33E;if(*((int*)_tmp4E7)!= 0)goto _LL33E;_tmp4E8=_tmp4E6.f2;
if((int)_tmp4E8 != 0)goto _LL33E;_LL33D: goto _LL33F;_LL33E: _tmp4E9=_tmp4E6.f1;if((
int)_tmp4E9 != 0)goto _LL340;_tmp4EA=_tmp4E6.f2;if((int)_tmp4EA != 0)goto _LL340;
_LL33F: okay=1;goto _LL33B;_LL340: _tmp4EB=_tmp4E6.f1;if(_tmp4EB <= (void*)1)goto
_LL342;if(*((int*)_tmp4EB)!= 0)goto _LL342;_tmp4EC=((struct Cyc_Absyn_Upper_b_struct*)
_tmp4EB)->f1;_tmp4ED=_tmp4E6.f2;if(_tmp4ED <= (void*)1)goto _LL342;if(*((int*)
_tmp4ED)!= 0)goto _LL342;_tmp4EE=((struct Cyc_Absyn_Upper_b_struct*)_tmp4ED)->f1;
_LL341: okay=okay  && Cyc_Evexp_lte_const_exp(_tmp4EE,_tmp4EC);if(!((int(*)(int y,
union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp4A7.ptr_atts).zero_term)){
const char*_tmpD5C;void*_tmpD5B;(_tmpD5B=0,Cyc_Tcutil_warn(loc,((_tmpD5C="implicit cast to shorter array",
_tag_dyneither(_tmpD5C,sizeof(char),31))),_tag_dyneither(_tmpD5B,sizeof(void*),0)));}
goto _LL33B;_LL342: _tmp4EF=_tmp4E6.f1;if((int)_tmp4EF != 0)goto _LL33B;_tmp4F0=
_tmp4E6.f2;if(_tmp4F0 <= (void*)1)goto _LL33B;if(*((int*)_tmp4F0)!= 0)goto _LL33B;
_tmp4F1=((struct Cyc_Absyn_Upper_b_struct*)_tmp4F0)->f1;_LL343: if(((int(*)(int y,
union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp4A5.ptr_atts).zero_term)
 && Cyc_Tcutil_is_bound_one((_tmp4A7.ptr_atts).bounds))goto _LL33B;okay=0;goto
_LL33B;_LL33B:;}okay=okay  && Cyc_Tcutil_unify(_tmp4A5.elt_typ,_tmp4A7.elt_typ);
okay=okay  && (Cyc_Tcutil_unify((_tmp4A5.ptr_atts).rgn,(_tmp4A7.ptr_atts).rgn)
 || Cyc_Tcenv_region_outlives(te,(_tmp4A5.ptr_atts).rgn,(_tmp4A7.ptr_atts).rgn));
okay=okay  && (!(_tmp4A5.elt_tq).real_const  || (_tmp4A7.elt_tq).real_const);okay=
okay  && (((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*
y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,(_tmp4A5.ptr_atts).zero_term,(
_tmp4A7.ptr_atts).zero_term) || ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
1,(_tmp4A5.ptr_atts).zero_term) && (_tmp4A7.elt_tq).real_const);return okay;}
_LL32F: _tmp4A8=_tmp4A3.f1;if(_tmp4A8 <= (void*)4)goto _LL331;if(*((int*)_tmp4A8)!= 
2)goto _LL331;_tmp4A9=((struct Cyc_Absyn_DatatypeType_struct*)_tmp4A8)->f1;_tmp4AA=
_tmp4A9.datatype_info;if((_tmp4AA.KnownDatatype).tag != 2)goto _LL331;_tmp4AB=(
struct Cyc_Absyn_Datatypedecl**)(_tmp4AA.KnownDatatype).val;_tmp4AC=*_tmp4AB;
_tmp4AD=_tmp4A9.targs;_tmp4AE=_tmp4A9.rgn;if(_tmp4AE == 0)goto _LL331;_tmp4AF=*
_tmp4AE;_tmp4B0=(void*)_tmp4AF.v;_tmp4B1=_tmp4A3.f2;if(_tmp4B1 <= (void*)4)goto
_LL331;if(*((int*)_tmp4B1)!= 2)goto _LL331;_tmp4B2=((struct Cyc_Absyn_DatatypeType_struct*)
_tmp4B1)->f1;_tmp4B3=_tmp4B2.datatype_info;if((_tmp4B3.KnownDatatype).tag != 2)
goto _LL331;_tmp4B4=(struct Cyc_Absyn_Datatypedecl**)(_tmp4B3.KnownDatatype).val;
_tmp4B5=*_tmp4B4;_tmp4B6=_tmp4B2.targs;_tmp4B7=_tmp4B2.rgn;if(_tmp4B7 == 0)goto
_LL331;_tmp4B8=*_tmp4B7;_tmp4B9=(void*)_tmp4B8.v;_LL330: if(_tmp4AC != _tmp4B5  || 
!Cyc_Tcenv_region_outlives(te,_tmp4B0,_tmp4B9))return 0;for(0;_tmp4AD != 0  && 
_tmp4B6 != 0;(_tmp4AD=_tmp4AD->tl,_tmp4B6=_tmp4B6->tl)){if(!Cyc_Tcutil_unify((
void*)_tmp4AD->hd,(void*)_tmp4B6->hd))return 0;}if(_tmp4AD != 0  || _tmp4B6 != 0)
return 0;return 1;_LL331: _tmp4BA=_tmp4A3.f1;if(_tmp4BA <= (void*)4)goto _LL333;if(*((
int*)_tmp4BA)!= 7)goto _LL333;_tmp4BB=((struct Cyc_Absyn_ArrayType_struct*)_tmp4BA)->f1;
_tmp4BC=_tmp4BB.elt_type;_tmp4BD=_tmp4BB.tq;_tmp4BE=_tmp4BB.num_elts;_tmp4BF=
_tmp4BB.zero_term;_tmp4C0=_tmp4A3.f2;if(_tmp4C0 <= (void*)4)goto _LL333;if(*((int*)
_tmp4C0)!= 7)goto _LL333;_tmp4C1=((struct Cyc_Absyn_ArrayType_struct*)_tmp4C0)->f1;
_tmp4C2=_tmp4C1.elt_type;_tmp4C3=_tmp4C1.tq;_tmp4C4=_tmp4C1.num_elts;_tmp4C5=
_tmp4C1.zero_term;_LL332: {int okay;okay=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*
x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp4BF,
_tmp4C5) && ((_tmp4BE != 0  && _tmp4C4 != 0) && Cyc_Evexp_same_const_exp((struct Cyc_Absyn_Exp*)
_tmp4BE,(struct Cyc_Absyn_Exp*)_tmp4C4));return(okay  && Cyc_Tcutil_unify(_tmp4BC,
_tmp4C2)) && (!_tmp4BD.real_const  || _tmp4C3.real_const);}_LL333: _tmp4C6=_tmp4A3.f1;
if(_tmp4C6 <= (void*)4)goto _LL335;if(*((int*)_tmp4C6)!= 3)goto _LL335;_tmp4C7=((
struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp4C6)->f1;_tmp4C8=_tmp4C7.field_info;
if((_tmp4C8.KnownDatatypefield).tag != 2)goto _LL335;_tmp4C9=(struct _tuple3)(
_tmp4C8.KnownDatatypefield).val;_tmp4CA=_tmp4C9.f1;_tmp4CB=_tmp4C9.f2;_tmp4CC=
_tmp4C7.targs;_tmp4CD=_tmp4A3.f2;if(_tmp4CD <= (void*)4)goto _LL335;if(*((int*)
_tmp4CD)!= 2)goto _LL335;_tmp4CE=((struct Cyc_Absyn_DatatypeType_struct*)_tmp4CD)->f1;
_tmp4CF=_tmp4CE.datatype_info;if((_tmp4CF.KnownDatatype).tag != 2)goto _LL335;
_tmp4D0=(struct Cyc_Absyn_Datatypedecl**)(_tmp4CF.KnownDatatype).val;_tmp4D1=*
_tmp4D0;_tmp4D2=_tmp4CE.targs;_LL334: if((_tmp4CA == _tmp4D1  || Cyc_Absyn_qvar_cmp(
_tmp4CA->name,_tmp4D1->name)== 0) && _tmp4CB->typs == 0){for(0;_tmp4CC != 0  && 
_tmp4D2 != 0;(_tmp4CC=_tmp4CC->tl,_tmp4D2=_tmp4D2->tl)){if(!Cyc_Tcutil_unify((
void*)_tmp4CC->hd,(void*)_tmp4D2->hd))break;}if(_tmp4CC == 0  && _tmp4D2 == 0)
return 1;}return 0;_LL335: _tmp4D3=_tmp4A3.f1;if(_tmp4D3 <= (void*)4)goto _LL337;if(*((
int*)_tmp4D3)!= 4)goto _LL337;_tmp4D4=((struct Cyc_Absyn_PointerType_struct*)
_tmp4D3)->f1;_tmp4D5=_tmp4D4.elt_typ;_tmp4D6=_tmp4D4.elt_tq;_tmp4D7=_tmp4D4.ptr_atts;
_tmp4D8=_tmp4D7.rgn;_tmp4D9=_tmp4D7.nullable;_tmp4DA=_tmp4D7.bounds;_tmp4DB=
_tmp4D7.zero_term;_tmp4DC=_tmp4A3.f2;if(_tmp4DC <= (void*)4)goto _LL337;if(*((int*)
_tmp4DC)!= 2)goto _LL337;_tmp4DD=((struct Cyc_Absyn_DatatypeType_struct*)_tmp4DC)->f1;
_tmp4DE=_tmp4DD.datatype_info;if((_tmp4DE.KnownDatatype).tag != 2)goto _LL337;
_tmp4DF=(struct Cyc_Absyn_Datatypedecl**)(_tmp4DE.KnownDatatype).val;_tmp4E0=*
_tmp4DF;_tmp4E1=_tmp4DD.targs;_tmp4E2=_tmp4DD.rgn;_LL336:{void*_tmp4F4=Cyc_Tcutil_compress(
_tmp4D5);struct Cyc_Absyn_DatatypeFieldInfo _tmp4F5;union Cyc_Absyn_DatatypeFieldInfoU
_tmp4F6;struct _tuple3 _tmp4F7;struct Cyc_Absyn_Datatypedecl*_tmp4F8;struct Cyc_Absyn_Datatypefield*
_tmp4F9;struct Cyc_List_List*_tmp4FA;_LL345: if(_tmp4F4 <= (void*)4)goto _LL347;if(*((
int*)_tmp4F4)!= 3)goto _LL347;_tmp4F5=((struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp4F4)->f1;_tmp4F6=_tmp4F5.field_info;if((_tmp4F6.KnownDatatypefield).tag != 2)
goto _LL347;_tmp4F7=(struct _tuple3)(_tmp4F6.KnownDatatypefield).val;_tmp4F8=
_tmp4F7.f1;_tmp4F9=_tmp4F7.f2;_tmp4FA=_tmp4F5.targs;_LL346: if(!Cyc_Tcutil_unify(
_tmp4D8,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp4E2))->v) && !Cyc_Tcenv_region_outlives(
te,_tmp4D8,(void*)_tmp4E2->v))return 0;if(!((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*
x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp4D9,
Cyc_Absyn_false_conref))return 0;if(!Cyc_Tcutil_unify_conrefs(Cyc_Tcutil_unify_it_bounds,
_tmp4DA,Cyc_Absyn_bounds_one_conref))return 0;if(!((int(*)(int(*cmp)(int,int),
union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(
Cyc_Core_intcmp,_tmp4DB,Cyc_Absyn_false_conref))return 0;if(Cyc_Absyn_qvar_cmp(
_tmp4E0->name,_tmp4F8->name)== 0  && _tmp4F9->typs != 0){int okay=1;for(0;_tmp4FA != 
0  && _tmp4E1 != 0;(_tmp4FA=_tmp4FA->tl,_tmp4E1=_tmp4E1->tl)){if(!Cyc_Tcutil_unify((
void*)_tmp4FA->hd,(void*)_tmp4E1->hd)){okay=0;break;}}if((!okay  || _tmp4FA != 0)
 || _tmp4E1 != 0)return 0;return 1;}goto _LL344;_LL347:;_LL348: goto _LL344;_LL344:;}
return 0;_LL337: _tmp4E3=_tmp4A3.f1;if(_tmp4E3 <= (void*)4)goto _LL339;if(*((int*)
_tmp4E3)!= 18)goto _LL339;_tmp4E4=_tmp4A3.f2;if(_tmp4E4 <= (void*)4)goto _LL339;if(*((
int*)_tmp4E4)!= 5)goto _LL339;_LL338: return 0;_LL339:;_LL33A: return Cyc_Tcutil_unify(
t1,t2);_LL32C:;}}int Cyc_Tcutil_is_pointer_type(void*t);int Cyc_Tcutil_is_pointer_type(
void*t){void*_tmp4FB=Cyc_Tcutil_compress(t);_LL34A: if(_tmp4FB <= (void*)4)goto
_LL34C;if(*((int*)_tmp4FB)!= 4)goto _LL34C;_LL34B: return 1;_LL34C:;_LL34D: return 0;
_LL349:;}int Cyc_Tcutil_rgn_of_pointer(void*t,void**rgn);int Cyc_Tcutil_rgn_of_pointer(
void*t,void**rgn){void*_tmp4FC=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo
_tmp4FD;struct Cyc_Absyn_PtrAtts _tmp4FE;void*_tmp4FF;_LL34F: if(_tmp4FC <= (void*)4)
goto _LL351;if(*((int*)_tmp4FC)!= 4)goto _LL351;_tmp4FD=((struct Cyc_Absyn_PointerType_struct*)
_tmp4FC)->f1;_tmp4FE=_tmp4FD.ptr_atts;_tmp4FF=_tmp4FE.rgn;_LL350:*rgn=_tmp4FF;
return 1;_LL351:;_LL352: return 0;_LL34E:;}int Cyc_Tcutil_is_pointer_or_boxed(void*t,
int*is_dyneither_ptr);int Cyc_Tcutil_is_pointer_or_boxed(void*t,int*
is_dyneither_ptr){void*_tmp500=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo
_tmp501;struct Cyc_Absyn_PtrAtts _tmp502;union Cyc_Absyn_Constraint*_tmp503;_LL354:
if(_tmp500 <= (void*)4)goto _LL356;if(*((int*)_tmp500)!= 4)goto _LL356;_tmp501=((
struct Cyc_Absyn_PointerType_struct*)_tmp500)->f1;_tmp502=_tmp501.ptr_atts;
_tmp503=_tmp502.bounds;_LL355:*is_dyneither_ptr=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,
_tmp503)== (void*)0;return 1;_LL356:;_LL357: return Cyc_Tcutil_typ_kind(t)== (void*)
2;_LL353:;}int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*e);int Cyc_Tcutil_is_zero(
struct Cyc_Absyn_Exp*e){void*_tmp504=e->r;union Cyc_Absyn_Cnst _tmp505;struct
_tuple7 _tmp506;int _tmp507;union Cyc_Absyn_Cnst _tmp508;struct _tuple5 _tmp509;char
_tmp50A;union Cyc_Absyn_Cnst _tmp50B;struct _tuple6 _tmp50C;short _tmp50D;union Cyc_Absyn_Cnst
_tmp50E;struct _tuple8 _tmp50F;long long _tmp510;void*_tmp511;struct Cyc_Absyn_Exp*
_tmp512;_LL359: if(*((int*)_tmp504)!= 0)goto _LL35B;_tmp505=((struct Cyc_Absyn_Const_e_struct*)
_tmp504)->f1;if((_tmp505.Int_c).tag != 4)goto _LL35B;_tmp506=(struct _tuple7)(
_tmp505.Int_c).val;_tmp507=_tmp506.f2;if(_tmp507 != 0)goto _LL35B;_LL35A: goto
_LL35C;_LL35B: if(*((int*)_tmp504)!= 0)goto _LL35D;_tmp508=((struct Cyc_Absyn_Const_e_struct*)
_tmp504)->f1;if((_tmp508.Char_c).tag != 2)goto _LL35D;_tmp509=(struct _tuple5)(
_tmp508.Char_c).val;_tmp50A=_tmp509.f2;if(_tmp50A != 0)goto _LL35D;_LL35C: goto
_LL35E;_LL35D: if(*((int*)_tmp504)!= 0)goto _LL35F;_tmp50B=((struct Cyc_Absyn_Const_e_struct*)
_tmp504)->f1;if((_tmp50B.Short_c).tag != 3)goto _LL35F;_tmp50C=(struct _tuple6)(
_tmp50B.Short_c).val;_tmp50D=_tmp50C.f2;if(_tmp50D != 0)goto _LL35F;_LL35E: goto
_LL360;_LL35F: if(*((int*)_tmp504)!= 0)goto _LL361;_tmp50E=((struct Cyc_Absyn_Const_e_struct*)
_tmp504)->f1;if((_tmp50E.LongLong_c).tag != 5)goto _LL361;_tmp50F=(struct _tuple8)(
_tmp50E.LongLong_c).val;_tmp510=_tmp50F.f2;if(_tmp510 != 0)goto _LL361;_LL360:
return 1;_LL361: if(*((int*)_tmp504)!= 15)goto _LL363;_tmp511=(void*)((struct Cyc_Absyn_Cast_e_struct*)
_tmp504)->f1;_tmp512=((struct Cyc_Absyn_Cast_e_struct*)_tmp504)->f2;_LL362: return
Cyc_Tcutil_is_zero(_tmp512) && Cyc_Tcutil_admits_zero(_tmp511);_LL363:;_LL364:
return 0;_LL358:;}struct Cyc_Core_Opt Cyc_Tcutil_trk={(void*)((void*)5)};struct Cyc_Core_Opt
Cyc_Tcutil_urk={(void*)((void*)4)};struct Cyc_Core_Opt Cyc_Tcutil_rk={(void*)((
void*)3)};struct Cyc_Core_Opt Cyc_Tcutil_ak={(void*)((void*)0)};struct Cyc_Core_Opt
Cyc_Tcutil_bk={(void*)((void*)2)};struct Cyc_Core_Opt Cyc_Tcutil_mk={(void*)((void*)
1)};struct Cyc_Core_Opt Cyc_Tcutil_ik={(void*)((void*)7)};struct Cyc_Core_Opt Cyc_Tcutil_ek={(
void*)((void*)6)};struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_opt(void*k);struct Cyc_Core_Opt*
Cyc_Tcutil_kind_to_opt(void*k){void*_tmp513=k;_LL366: if((int)_tmp513 != 0)goto
_LL368;_LL367: return(struct Cyc_Core_Opt*)& Cyc_Tcutil_ak;_LL368: if((int)_tmp513 != 
1)goto _LL36A;_LL369: return(struct Cyc_Core_Opt*)& Cyc_Tcutil_mk;_LL36A: if((int)
_tmp513 != 2)goto _LL36C;_LL36B: return(struct Cyc_Core_Opt*)& Cyc_Tcutil_bk;_LL36C:
if((int)_tmp513 != 3)goto _LL36E;_LL36D: return(struct Cyc_Core_Opt*)& Cyc_Tcutil_rk;
_LL36E: if((int)_tmp513 != 4)goto _LL370;_LL36F: return(struct Cyc_Core_Opt*)& Cyc_Tcutil_urk;
_LL370: if((int)_tmp513 != 5)goto _LL372;_LL371: return(struct Cyc_Core_Opt*)& Cyc_Tcutil_trk;
_LL372: if((int)_tmp513 != 6)goto _LL374;_LL373: return(struct Cyc_Core_Opt*)& Cyc_Tcutil_ek;
_LL374: if((int)_tmp513 != 7)goto _LL365;_LL375: return(struct Cyc_Core_Opt*)& Cyc_Tcutil_ik;
_LL365:;}static void**Cyc_Tcutil_kind_to_b(void*k);static void**Cyc_Tcutil_kind_to_b(
void*k){static struct Cyc_Absyn_Eq_kb_struct ab_v={0,(void*)((void*)0)};static
struct Cyc_Absyn_Eq_kb_struct mb_v={0,(void*)((void*)1)};static struct Cyc_Absyn_Eq_kb_struct
bb_v={0,(void*)((void*)2)};static struct Cyc_Absyn_Eq_kb_struct rb_v={0,(void*)((
void*)3)};static struct Cyc_Absyn_Eq_kb_struct ub_v={0,(void*)((void*)4)};static
struct Cyc_Absyn_Eq_kb_struct tb_v={0,(void*)((void*)5)};static struct Cyc_Absyn_Eq_kb_struct
eb_v={0,(void*)((void*)6)};static struct Cyc_Absyn_Eq_kb_struct ib_v={0,(void*)((
void*)7)};static void*ab=(void*)& ab_v;static void*mb=(void*)& mb_v;static void*bb=(
void*)& bb_v;static void*rb=(void*)& rb_v;static void*ub=(void*)& ub_v;static void*tb=(
void*)& tb_v;static void*eb=(void*)& eb_v;static void*ib=(void*)& ib_v;void*_tmp514=k;
_LL377: if((int)_tmp514 != 0)goto _LL379;_LL378: return& ab;_LL379: if((int)_tmp514 != 
1)goto _LL37B;_LL37A: return& mb;_LL37B: if((int)_tmp514 != 2)goto _LL37D;_LL37C:
return& bb;_LL37D: if((int)_tmp514 != 3)goto _LL37F;_LL37E: return& rb;_LL37F: if((int)
_tmp514 != 4)goto _LL381;_LL380: return& ub;_LL381: if((int)_tmp514 != 5)goto _LL383;
_LL382: return& tb;_LL383: if((int)_tmp514 != 6)goto _LL385;_LL384: return& eb;_LL385:
if((int)_tmp514 != 7)goto _LL376;_LL386: return& ib;_LL376:;}void*Cyc_Tcutil_kind_to_bound(
void*k);void*Cyc_Tcutil_kind_to_bound(void*k){return*Cyc_Tcutil_kind_to_b(k);}
struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_bound_opt(void*k);struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_bound_opt(
void*k){return(struct Cyc_Core_Opt*)Cyc_Tcutil_kind_to_b(k);}int Cyc_Tcutil_zero_to_null(
struct Cyc_Tcenv_Tenv*te,void*t2,struct Cyc_Absyn_Exp*e1);int Cyc_Tcutil_zero_to_null(
struct Cyc_Tcenv_Tenv*te,void*t2,struct Cyc_Absyn_Exp*e1){if(Cyc_Tcutil_is_pointer_type(
t2) && Cyc_Tcutil_is_zero(e1)){{struct Cyc_Absyn_Const_e_struct _tmpD5F;struct Cyc_Absyn_Const_e_struct*
_tmpD5E;e1->r=(void*)((_tmpD5E=_cycalloc(sizeof(*_tmpD5E)),((_tmpD5E[0]=((
_tmpD5F.tag=0,((_tmpD5F.f1=Cyc_Absyn_Null_c,_tmpD5F)))),_tmpD5E))));}{struct Cyc_Core_Opt*
_tmp51F=Cyc_Tcenv_lookup_opt_type_vars(te);struct Cyc_Absyn_PointerType_struct
_tmpD69;struct Cyc_Absyn_PtrAtts _tmpD68;struct Cyc_Absyn_PtrInfo _tmpD67;struct Cyc_Absyn_PointerType_struct*
_tmpD66;struct Cyc_Absyn_PointerType_struct*_tmp520=(_tmpD66=_cycalloc(sizeof(*
_tmpD66)),((_tmpD66[0]=((_tmpD69.tag=4,((_tmpD69.f1=((_tmpD67.elt_typ=Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_ak,_tmp51F),((_tmpD67.elt_tq=Cyc_Absyn_empty_tqual(
0),((_tmpD67.ptr_atts=((_tmpD68.rgn=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_trk,
_tmp51F),((_tmpD68.nullable=Cyc_Absyn_true_conref,((_tmpD68.bounds=Cyc_Absyn_empty_conref(),((
_tmpD68.zero_term=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((
_tmpD68.ptrloc=0,_tmpD68)))))))))),_tmpD67)))))),_tmpD69)))),_tmpD66)));((struct
Cyc_Core_Opt*)_check_null(e1->topt))->v=(void*)((void*)_tmp520);return Cyc_Tcutil_coerce_arg(
te,e1,t2);}}return 0;}struct _dyneither_ptr Cyc_Tcutil_coercion2string(void*c);
struct _dyneither_ptr Cyc_Tcutil_coercion2string(void*c){void*_tmp525=c;_LL388: if((
int)_tmp525 != 0)goto _LL38A;_LL389: {const char*_tmpD6A;return(_tmpD6A="unknown",
_tag_dyneither(_tmpD6A,sizeof(char),8));}_LL38A: if((int)_tmp525 != 1)goto _LL38C;
_LL38B: {const char*_tmpD6B;return(_tmpD6B="no coercion",_tag_dyneither(_tmpD6B,
sizeof(char),12));}_LL38C: if((int)_tmp525 != 2)goto _LL38E;_LL38D: {const char*
_tmpD6C;return(_tmpD6C="null check",_tag_dyneither(_tmpD6C,sizeof(char),11));}
_LL38E: if((int)_tmp525 != 3)goto _LL387;_LL38F: {const char*_tmpD6D;return(_tmpD6D="other coercion",
_tag_dyneither(_tmpD6D,sizeof(char),15));}_LL387:;}int Cyc_Tcutil_coerce_arg(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t2);int Cyc_Tcutil_coerce_arg(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t2){void*t1=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);void*c;if(Cyc_Tcutil_unify(
t1,t2))return 1;if(Cyc_Tcutil_is_arithmetic_type(t2) && Cyc_Tcutil_is_arithmetic_type(
t1)){if(Cyc_Tcutil_will_lose_precision(t1,t2)){const char*_tmpD72;void*_tmpD71[2];
struct Cyc_String_pa_struct _tmpD70;struct Cyc_String_pa_struct _tmpD6F;(_tmpD6F.tag=
0,((_tmpD6F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t2)),((_tmpD70.tag=0,((_tmpD70.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t1)),((_tmpD71[0]=& _tmpD70,((_tmpD71[1]=& _tmpD6F,Cyc_Tcutil_warn(
e->loc,((_tmpD72="integral size mismatch; %s -> %s conversion supplied",
_tag_dyneither(_tmpD72,sizeof(char),53))),_tag_dyneither(_tmpD71,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_unchecked_cast(te,e,t2,(void*)1);return 1;}else{if(Cyc_Tcutil_silent_castable(
te,e->loc,t1,t2)){Cyc_Tcutil_unchecked_cast(te,e,t2,(void*)3);return 1;}else{if(
Cyc_Tcutil_zero_to_null(te,t2,e))return 1;else{if((c=Cyc_Tcutil_castable(te,e->loc,
t1,t2))!= (void*)0){Cyc_Tcutil_unchecked_cast(te,e,t2,c);if(c != (void*)2){const
char*_tmpD77;void*_tmpD76[2];struct Cyc_String_pa_struct _tmpD75;struct Cyc_String_pa_struct
_tmpD74;(_tmpD74.tag=0,((_tmpD74.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2)),((_tmpD75.tag=0,((_tmpD75.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmpD76[0]=& _tmpD75,((_tmpD76[
1]=& _tmpD74,Cyc_Tcutil_warn(e->loc,((_tmpD77="implicit cast from %s to %s",
_tag_dyneither(_tmpD77,sizeof(char),28))),_tag_dyneither(_tmpD76,sizeof(void*),2)))))))))))));}
return 1;}else{return 0;}}}}}int Cyc_Tcutil_coerce_assign(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Absyn_Exp*e,void*t);int Cyc_Tcutil_coerce_assign(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e,void*t){return Cyc_Tcutil_coerce_arg(te,e,t);}int Cyc_Tcutil_coerceable(
void*t);int Cyc_Tcutil_coerceable(void*t){void*_tmp532=Cyc_Tcutil_compress(t);
_LL391: if(_tmp532 <= (void*)4)goto _LL393;if(*((int*)_tmp532)!= 5)goto _LL393;
_LL392: goto _LL394;_LL393: if((int)_tmp532 != 1)goto _LL395;_LL394: goto _LL396;_LL395:
if(_tmp532 <= (void*)4)goto _LL397;if(*((int*)_tmp532)!= 6)goto _LL397;_LL396:
return 1;_LL397:;_LL398: return 0;_LL390:;}static struct _tuple11*Cyc_Tcutil_flatten_typ_f(
struct _tuple15*env,struct Cyc_Absyn_Aggrfield*x);static struct _tuple11*Cyc_Tcutil_flatten_typ_f(
struct _tuple15*env,struct Cyc_Absyn_Aggrfield*x){struct Cyc_List_List*_tmp534;
struct _RegionHandle*_tmp535;struct _tuple15 _tmp533=*env;_tmp534=_tmp533.f1;
_tmp535=_tmp533.f2;{struct _tuple11*_tmpD78;return(_tmpD78=_region_malloc(_tmp535,
sizeof(*_tmpD78)),((_tmpD78->f1=x->tq,((_tmpD78->f2=Cyc_Tcutil_rsubstitute(
_tmp535,_tmp534,x->type),_tmpD78)))));}}static struct _tuple11*Cyc_Tcutil_rcopy_tqt(
struct _RegionHandle*r,struct _tuple11*x);static struct _tuple11*Cyc_Tcutil_rcopy_tqt(
struct _RegionHandle*r,struct _tuple11*x){struct Cyc_Absyn_Tqual _tmp538;void*
_tmp539;struct _tuple11 _tmp537=*x;_tmp538=_tmp537.f1;_tmp539=_tmp537.f2;{struct
_tuple11*_tmpD79;return(_tmpD79=_region_malloc(r,sizeof(*_tmpD79)),((_tmpD79->f1=
_tmp538,((_tmpD79->f2=_tmp539,_tmpD79)))));}}static struct Cyc_List_List*Cyc_Tcutil_flatten_typ(
struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,void*t1);static struct Cyc_List_List*
Cyc_Tcutil_flatten_typ(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,void*t1){t1=
Cyc_Tcutil_compress(t1);{void*_tmp53B=t1;struct Cyc_List_List*_tmp53C;struct Cyc_Absyn_AggrInfo
_tmp53D;union Cyc_Absyn_AggrInfoU _tmp53E;struct Cyc_Absyn_Aggrdecl**_tmp53F;struct
Cyc_Absyn_Aggrdecl*_tmp540;struct Cyc_List_List*_tmp541;void*_tmp542;struct Cyc_List_List*
_tmp543;_LL39A: if((int)_tmp53B != 0)goto _LL39C;_LL39B: return 0;_LL39C: if(_tmp53B <= (
void*)4)goto _LL3A2;if(*((int*)_tmp53B)!= 9)goto _LL39E;_tmp53C=((struct Cyc_Absyn_TupleType_struct*)
_tmp53B)->f1;_LL39D: return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct
_tuple11*(*f)(struct _RegionHandle*,struct _tuple11*),struct _RegionHandle*env,
struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_rcopy_tqt,r,_tmp53C);_LL39E:
if(*((int*)_tmp53B)!= 10)goto _LL3A0;_tmp53D=((struct Cyc_Absyn_AggrType_struct*)
_tmp53B)->f1;_tmp53E=_tmp53D.aggr_info;if((_tmp53E.KnownAggr).tag != 2)goto _LL3A0;
_tmp53F=(struct Cyc_Absyn_Aggrdecl**)(_tmp53E.KnownAggr).val;_tmp540=*_tmp53F;
_tmp541=_tmp53D.targs;_LL39F: if(((_tmp540->kind == (void*)1  || _tmp540->impl == 0)
 || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp540->impl))->exist_vars != 0)
 || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp540->impl))->rgn_po != 0){
struct _tuple11*_tmpD7C;struct Cyc_List_List*_tmpD7B;return(_tmpD7B=_region_malloc(
r,sizeof(*_tmpD7B)),((_tmpD7B->hd=((_tmpD7C=_region_malloc(r,sizeof(*_tmpD7C)),((
_tmpD7C->f1=Cyc_Absyn_const_tqual(0),((_tmpD7C->f2=t1,_tmpD7C)))))),((_tmpD7B->tl=
0,_tmpD7B)))));}{struct Cyc_List_List*_tmp546=((struct Cyc_List_List*(*)(struct
_RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(r,r,_tmp540->tvs,_tmp541);struct _tuple15 _tmpD7D;struct _tuple15
env=(_tmpD7D.f1=_tmp546,((_tmpD7D.f2=r,_tmpD7D)));return((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _tuple11*(*f)(struct _tuple15*,struct Cyc_Absyn_Aggrfield*),
struct _tuple15*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_typ_f,&
env,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp540->impl))->fields);}_LL3A0:
if(*((int*)_tmp53B)!= 11)goto _LL3A2;_tmp542=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp53B)->f1;if((int)_tmp542 != 0)goto _LL3A2;_tmp543=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp53B)->f2;_LL3A1: {struct _tuple15 _tmpD7E;struct _tuple15 env=(_tmpD7E.f1=0,((
_tmpD7E.f2=r,_tmpD7E)));return((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct _tuple11*(*f)(struct _tuple15*,struct Cyc_Absyn_Aggrfield*),struct _tuple15*
env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_typ_f,& env,
_tmp543);}_LL3A2:;_LL3A3: {struct _tuple11*_tmpD81;struct Cyc_List_List*_tmpD80;
return(_tmpD80=_region_malloc(r,sizeof(*_tmpD80)),((_tmpD80->hd=((_tmpD81=
_region_malloc(r,sizeof(*_tmpD81)),((_tmpD81->f1=Cyc_Absyn_const_tqual(0),((
_tmpD81->f2=t1,_tmpD81)))))),((_tmpD80->tl=0,_tmpD80)))));}_LL399:;}}static int
Cyc_Tcutil_sub_attributes(struct Cyc_List_List*a1,struct Cyc_List_List*a2);static
int Cyc_Tcutil_sub_attributes(struct Cyc_List_List*a1,struct Cyc_List_List*a2){{
struct Cyc_List_List*t=a1;for(0;t != 0;t=t->tl){void*_tmp54B=(void*)t->hd;_LL3A5:
if((int)_tmp54B != 16)goto _LL3A7;_LL3A6: goto _LL3A8;_LL3A7: if((int)_tmp54B != 3)
goto _LL3A9;_LL3A8: goto _LL3AA;_LL3A9: if(_tmp54B <= (void*)17)goto _LL3AB;if(*((int*)
_tmp54B)!= 4)goto _LL3AB;_LL3AA: continue;_LL3AB:;_LL3AC: if(!Cyc_List_exists_c(Cyc_Tcutil_equal_att,(
void*)t->hd,a2))return 0;_LL3A4:;}}for(0;a2 != 0;a2=a2->tl){if(!Cyc_List_exists_c(
Cyc_Tcutil_equal_att,(void*)a2->hd,a1))return 0;}return 1;}static int Cyc_Tcutil_ptrsubtype(
struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2);static int Cyc_Tcutil_subtype(
struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2);static int Cyc_Tcutil_subtype(
struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2){if(Cyc_Tcutil_unify(
t1,t2))return 1;{struct Cyc_List_List*a=assume;for(0;a != 0;a=a->tl){if(Cyc_Tcutil_unify(
t1,(*((struct _tuple0*)a->hd)).f1) && Cyc_Tcutil_unify(t2,(*((struct _tuple0*)a->hd)).f2))
return 1;}}t1=Cyc_Tcutil_compress(t1);t2=Cyc_Tcutil_compress(t2);{struct _tuple0
_tmpD82;struct _tuple0 _tmp54D=(_tmpD82.f1=t1,((_tmpD82.f2=t2,_tmpD82)));void*
_tmp54E;struct Cyc_Absyn_PtrInfo _tmp54F;void*_tmp550;struct Cyc_Absyn_Tqual _tmp551;
struct Cyc_Absyn_PtrAtts _tmp552;void*_tmp553;union Cyc_Absyn_Constraint*_tmp554;
union Cyc_Absyn_Constraint*_tmp555;union Cyc_Absyn_Constraint*_tmp556;void*_tmp557;
struct Cyc_Absyn_PtrInfo _tmp558;void*_tmp559;struct Cyc_Absyn_Tqual _tmp55A;struct
Cyc_Absyn_PtrAtts _tmp55B;void*_tmp55C;union Cyc_Absyn_Constraint*_tmp55D;union Cyc_Absyn_Constraint*
_tmp55E;union Cyc_Absyn_Constraint*_tmp55F;void*_tmp560;struct Cyc_Absyn_DatatypeInfo
_tmp561;union Cyc_Absyn_DatatypeInfoU _tmp562;struct Cyc_Absyn_Datatypedecl**
_tmp563;struct Cyc_Absyn_Datatypedecl*_tmp564;struct Cyc_List_List*_tmp565;struct
Cyc_Core_Opt*_tmp566;struct Cyc_Core_Opt _tmp567;void*_tmp568;void*_tmp569;struct
Cyc_Absyn_DatatypeInfo _tmp56A;union Cyc_Absyn_DatatypeInfoU _tmp56B;struct Cyc_Absyn_Datatypedecl**
_tmp56C;struct Cyc_Absyn_Datatypedecl*_tmp56D;struct Cyc_List_List*_tmp56E;struct
Cyc_Core_Opt*_tmp56F;struct Cyc_Core_Opt _tmp570;void*_tmp571;void*_tmp572;struct
Cyc_Absyn_FnInfo _tmp573;void*_tmp574;struct Cyc_Absyn_FnInfo _tmp575;_LL3AE:
_tmp54E=_tmp54D.f1;if(_tmp54E <= (void*)4)goto _LL3B0;if(*((int*)_tmp54E)!= 4)goto
_LL3B0;_tmp54F=((struct Cyc_Absyn_PointerType_struct*)_tmp54E)->f1;_tmp550=
_tmp54F.elt_typ;_tmp551=_tmp54F.elt_tq;_tmp552=_tmp54F.ptr_atts;_tmp553=_tmp552.rgn;
_tmp554=_tmp552.nullable;_tmp555=_tmp552.bounds;_tmp556=_tmp552.zero_term;
_tmp557=_tmp54D.f2;if(_tmp557 <= (void*)4)goto _LL3B0;if(*((int*)_tmp557)!= 4)goto
_LL3B0;_tmp558=((struct Cyc_Absyn_PointerType_struct*)_tmp557)->f1;_tmp559=
_tmp558.elt_typ;_tmp55A=_tmp558.elt_tq;_tmp55B=_tmp558.ptr_atts;_tmp55C=_tmp55B.rgn;
_tmp55D=_tmp55B.nullable;_tmp55E=_tmp55B.bounds;_tmp55F=_tmp55B.zero_term;_LL3AF:
if(_tmp551.real_const  && !_tmp55A.real_const)return 0;if((!((int(*)(int(*cmp)(int,
int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(
Cyc_Core_intcmp,_tmp554,_tmp55D) && ((int(*)(int y,union Cyc_Absyn_Constraint*x))
Cyc_Absyn_conref_def)(0,_tmp554)) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))
Cyc_Absyn_conref_def)(0,_tmp55D))return 0;if((!((int(*)(int(*cmp)(int,int),union
Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(
Cyc_Core_intcmp,_tmp556,_tmp55F) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))
Cyc_Absyn_conref_def)(0,_tmp556)) && ((int(*)(int y,union Cyc_Absyn_Constraint*x))
Cyc_Absyn_conref_def)(0,_tmp55F))return 0;if(!Cyc_Tcutil_unify(_tmp553,_tmp55C)
 && !Cyc_Tcenv_region_outlives(te,_tmp553,_tmp55C))return 0;if(!Cyc_Tcutil_unify_conrefs(
Cyc_Tcutil_unify_it_bounds,_tmp555,_tmp55E)){struct _tuple0 _tmpD83;struct _tuple0
_tmp577=(_tmpD83.f1=Cyc_Absyn_conref_val(_tmp555),((_tmpD83.f2=Cyc_Absyn_conref_val(
_tmp55E),_tmpD83)));void*_tmp578;void*_tmp579;void*_tmp57A;struct Cyc_Absyn_Exp*
_tmp57B;void*_tmp57C;struct Cyc_Absyn_Exp*_tmp57D;_LL3B7: _tmp578=_tmp577.f1;if(
_tmp578 <= (void*)1)goto _LL3B9;if(*((int*)_tmp578)!= 0)goto _LL3B9;_tmp579=_tmp577.f2;
if((int)_tmp579 != 0)goto _LL3B9;_LL3B8: goto _LL3B6;_LL3B9: _tmp57A=_tmp577.f1;if(
_tmp57A <= (void*)1)goto _LL3BB;if(*((int*)_tmp57A)!= 0)goto _LL3BB;_tmp57B=((
struct Cyc_Absyn_Upper_b_struct*)_tmp57A)->f1;_tmp57C=_tmp577.f2;if(_tmp57C <= (
void*)1)goto _LL3BB;if(*((int*)_tmp57C)!= 0)goto _LL3BB;_tmp57D=((struct Cyc_Absyn_Upper_b_struct*)
_tmp57C)->f1;_LL3BA: if(!Cyc_Evexp_lte_const_exp(_tmp57D,_tmp57B))return 0;goto
_LL3B6;_LL3BB:;_LL3BC: return 0;_LL3B6:;}{struct _tuple0*_tmpD86;struct Cyc_List_List*
_tmpD85;return Cyc_Tcutil_ptrsubtype(te,((_tmpD85=_cycalloc(sizeof(*_tmpD85)),((
_tmpD85->hd=((_tmpD86=_cycalloc(sizeof(*_tmpD86)),((_tmpD86->f1=t1,((_tmpD86->f2=
t2,_tmpD86)))))),((_tmpD85->tl=assume,_tmpD85)))))),_tmp550,_tmp559);}_LL3B0:
_tmp560=_tmp54D.f1;if(_tmp560 <= (void*)4)goto _LL3B2;if(*((int*)_tmp560)!= 2)goto
_LL3B2;_tmp561=((struct Cyc_Absyn_DatatypeType_struct*)_tmp560)->f1;_tmp562=
_tmp561.datatype_info;if((_tmp562.KnownDatatype).tag != 2)goto _LL3B2;_tmp563=(
struct Cyc_Absyn_Datatypedecl**)(_tmp562.KnownDatatype).val;_tmp564=*_tmp563;
_tmp565=_tmp561.targs;_tmp566=_tmp561.rgn;if(_tmp566 == 0)goto _LL3B2;_tmp567=*
_tmp566;_tmp568=(void*)_tmp567.v;_tmp569=_tmp54D.f2;if(_tmp569 <= (void*)4)goto
_LL3B2;if(*((int*)_tmp569)!= 2)goto _LL3B2;_tmp56A=((struct Cyc_Absyn_DatatypeType_struct*)
_tmp569)->f1;_tmp56B=_tmp56A.datatype_info;if((_tmp56B.KnownDatatype).tag != 2)
goto _LL3B2;_tmp56C=(struct Cyc_Absyn_Datatypedecl**)(_tmp56B.KnownDatatype).val;
_tmp56D=*_tmp56C;_tmp56E=_tmp56A.targs;_tmp56F=_tmp56A.rgn;if(_tmp56F == 0)goto
_LL3B2;_tmp570=*_tmp56F;_tmp571=(void*)_tmp570.v;_LL3B1: if(_tmp564 != _tmp56D  || 
!Cyc_Tcenv_region_outlives(te,_tmp568,_tmp571))return 0;for(0;_tmp565 != 0  && 
_tmp56E != 0;(_tmp565=_tmp565->tl,_tmp56E=_tmp56E->tl)){if(!Cyc_Tcutil_unify((
void*)_tmp565->hd,(void*)_tmp56E->hd))return 0;}if(_tmp565 != 0  || _tmp56E != 0)
return 0;return 1;_LL3B2: _tmp572=_tmp54D.f1;if(_tmp572 <= (void*)4)goto _LL3B4;if(*((
int*)_tmp572)!= 8)goto _LL3B4;_tmp573=((struct Cyc_Absyn_FnType_struct*)_tmp572)->f1;
_tmp574=_tmp54D.f2;if(_tmp574 <= (void*)4)goto _LL3B4;if(*((int*)_tmp574)!= 8)goto
_LL3B4;_tmp575=((struct Cyc_Absyn_FnType_struct*)_tmp574)->f1;_LL3B3: if(_tmp573.tvars
!= 0  || _tmp575.tvars != 0){struct Cyc_List_List*_tmp580=_tmp573.tvars;struct Cyc_List_List*
_tmp581=_tmp575.tvars;if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp580)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp581))return 0;{
struct _RegionHandle*_tmp582=Cyc_Tcenv_get_fnrgn(te);struct Cyc_List_List*inst=0;
while(_tmp580 != 0){if(Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp580->hd)!= 
Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)_check_null(
_tmp581))->hd))return 0;{struct _tuple14*_tmpD90;struct Cyc_Absyn_VarType_struct
_tmpD8F;struct Cyc_Absyn_VarType_struct*_tmpD8E;struct Cyc_List_List*_tmpD8D;inst=((
_tmpD8D=_region_malloc(_tmp582,sizeof(*_tmpD8D)),((_tmpD8D->hd=((_tmpD90=
_region_malloc(_tmp582,sizeof(*_tmpD90)),((_tmpD90->f1=(struct Cyc_Absyn_Tvar*)
_tmp581->hd,((_tmpD90->f2=(void*)((_tmpD8E=_cycalloc(sizeof(*_tmpD8E)),((_tmpD8E[
0]=((_tmpD8F.tag=1,((_tmpD8F.f1=(struct Cyc_Absyn_Tvar*)_tmp580->hd,_tmpD8F)))),
_tmpD8E)))),_tmpD90)))))),((_tmpD8D->tl=inst,_tmpD8D))))));}_tmp580=_tmp580->tl;
_tmp581=_tmp581->tl;}if(inst != 0){_tmp573.tvars=0;_tmp575.tvars=0;{struct Cyc_Absyn_FnType_struct
_tmpD96;struct Cyc_Absyn_FnType_struct*_tmpD95;struct Cyc_Absyn_FnType_struct
_tmpD93;struct Cyc_Absyn_FnType_struct*_tmpD92;return Cyc_Tcutil_subtype(te,assume,(
void*)((_tmpD92=_cycalloc(sizeof(*_tmpD92)),((_tmpD92[0]=((_tmpD93.tag=8,((
_tmpD93.f1=_tmp573,_tmpD93)))),_tmpD92)))),(void*)((_tmpD95=_cycalloc(sizeof(*
_tmpD95)),((_tmpD95[0]=((_tmpD96.tag=8,((_tmpD96.f1=_tmp575,_tmpD96)))),_tmpD95)))));}}}}
if(!Cyc_Tcutil_subtype(te,assume,_tmp573.ret_typ,_tmp575.ret_typ))return 0;{
struct Cyc_List_List*_tmp58B=_tmp573.args;struct Cyc_List_List*_tmp58C=_tmp575.args;
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp58B)!= ((int(*)(struct
Cyc_List_List*x))Cyc_List_length)(_tmp58C))return 0;for(0;_tmp58B != 0;(_tmp58B=
_tmp58B->tl,_tmp58C=_tmp58C->tl)){struct Cyc_Absyn_Tqual _tmp58E;void*_tmp58F;
struct _tuple9 _tmp58D=*((struct _tuple9*)_tmp58B->hd);_tmp58E=_tmp58D.f2;_tmp58F=
_tmp58D.f3;{struct Cyc_Absyn_Tqual _tmp591;void*_tmp592;struct _tuple9 _tmp590=*((
struct _tuple9*)((struct Cyc_List_List*)_check_null(_tmp58C))->hd);_tmp591=_tmp590.f2;
_tmp592=_tmp590.f3;if(_tmp591.real_const  && !_tmp58E.real_const  || !Cyc_Tcutil_subtype(
te,assume,_tmp592,_tmp58F))return 0;}}if(_tmp573.c_varargs != _tmp575.c_varargs)
return 0;if(_tmp573.cyc_varargs != 0  && _tmp575.cyc_varargs != 0){struct Cyc_Absyn_VarargInfo
_tmp593=*_tmp573.cyc_varargs;struct Cyc_Absyn_VarargInfo _tmp594=*_tmp575.cyc_varargs;
if((_tmp594.tq).real_const  && !(_tmp593.tq).real_const  || !Cyc_Tcutil_subtype(te,
assume,_tmp594.type,_tmp593.type))return 0;}else{if(_tmp573.cyc_varargs != 0  || 
_tmp575.cyc_varargs != 0)return 0;}if(!Cyc_Tcutil_subset_effect(1,(void*)((struct
Cyc_Core_Opt*)_check_null(_tmp573.effect))->v,(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp575.effect))->v))return 0;if(!Cyc_Tcutil_sub_rgnpo(_tmp573.rgn_po,
_tmp575.rgn_po))return 0;if(!Cyc_Tcutil_sub_attributes(_tmp573.attributes,_tmp575.attributes))
return 0;return 1;}_LL3B4:;_LL3B5: return 0;_LL3AD:;}}static int Cyc_Tcutil_isomorphic(
void*t1,void*t2);static int Cyc_Tcutil_isomorphic(void*t1,void*t2){struct _tuple0
_tmpD97;struct _tuple0 _tmp596=(_tmpD97.f1=Cyc_Tcutil_compress(t1),((_tmpD97.f2=
Cyc_Tcutil_compress(t2),_tmpD97)));void*_tmp597;void*_tmp598;void*_tmp599;void*
_tmp59A;_LL3BE: _tmp597=_tmp596.f1;if(_tmp597 <= (void*)4)goto _LL3C0;if(*((int*)
_tmp597)!= 5)goto _LL3C0;_tmp598=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp597)->f2;
_tmp599=_tmp596.f2;if(_tmp599 <= (void*)4)goto _LL3C0;if(*((int*)_tmp599)!= 5)goto
_LL3C0;_tmp59A=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp599)->f2;_LL3BF:
return(_tmp598 == _tmp59A  || _tmp598 == (void*)2  && _tmp59A == (void*)3) || _tmp598
== (void*)3  && _tmp59A == (void*)2;_LL3C0:;_LL3C1: return 0;_LL3BD:;}static int Cyc_Tcutil_ptrsubtype(
struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2);static int Cyc_Tcutil_ptrsubtype(
struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2){struct
_RegionHandle*_tmp59B=Cyc_Tcenv_get_fnrgn(te);struct Cyc_List_List*tqs1=Cyc_Tcutil_flatten_typ(
_tmp59B,te,t1);struct Cyc_List_List*tqs2=Cyc_Tcutil_flatten_typ(_tmp59B,te,t2);
for(0;tqs2 != 0;(tqs2=tqs2->tl,tqs1=tqs1->tl)){if(tqs1 == 0)return 0;{struct
_tuple11 _tmp59D;struct Cyc_Absyn_Tqual _tmp59E;void*_tmp59F;struct _tuple11*_tmp59C=(
struct _tuple11*)tqs1->hd;_tmp59D=*_tmp59C;_tmp59E=_tmp59D.f1;_tmp59F=_tmp59D.f2;{
struct _tuple11 _tmp5A1;struct Cyc_Absyn_Tqual _tmp5A2;void*_tmp5A3;struct _tuple11*
_tmp5A0=(struct _tuple11*)tqs2->hd;_tmp5A1=*_tmp5A0;_tmp5A2=_tmp5A1.f1;_tmp5A3=
_tmp5A1.f2;if(_tmp5A2.real_const  && Cyc_Tcutil_subtype(te,assume,_tmp59F,_tmp5A3))
continue;else{if(Cyc_Tcutil_unify(_tmp59F,_tmp5A3))continue;else{if(Cyc_Tcutil_isomorphic(
_tmp59F,_tmp5A3))continue;else{return 0;}}}}}}return 1;}static int Cyc_Tcutil_is_char_type(
void*t);static int Cyc_Tcutil_is_char_type(void*t){void*_tmp5A4=Cyc_Tcutil_compress(
t);void*_tmp5A5;_LL3C3: if(_tmp5A4 <= (void*)4)goto _LL3C5;if(*((int*)_tmp5A4)!= 5)
goto _LL3C5;_tmp5A5=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp5A4)->f2;if((int)
_tmp5A5 != 0)goto _LL3C5;_LL3C4: return 1;_LL3C5:;_LL3C6: return 0;_LL3C2:;}void*Cyc_Tcutil_castable(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void*t1,void*t2);void*Cyc_Tcutil_castable(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void*t1,void*t2){if(Cyc_Tcutil_unify(
t1,t2))return(void*)1;t1=Cyc_Tcutil_compress(t1);t2=Cyc_Tcutil_compress(t2);if(
t2 == (void*)0)return(void*)1;{void*_tmp5A6=t2;void*_tmp5A7;void*_tmp5A8;_LL3C8:
if(_tmp5A6 <= (void*)4)goto _LL3CC;if(*((int*)_tmp5A6)!= 5)goto _LL3CA;_tmp5A7=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp5A6)->f2;if((int)_tmp5A7 != 2)goto
_LL3CA;_LL3C9: goto _LL3CB;_LL3CA: if(*((int*)_tmp5A6)!= 5)goto _LL3CC;_tmp5A8=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp5A6)->f2;if((int)_tmp5A8 != 3)goto _LL3CC;
_LL3CB: if(Cyc_Tcutil_typ_kind(t1)== (void*)2)return(void*)1;goto _LL3C7;_LL3CC:;
_LL3CD: goto _LL3C7;_LL3C7:;}{void*_tmp5A9=t1;struct Cyc_Absyn_PtrInfo _tmp5AA;void*
_tmp5AB;struct Cyc_Absyn_Tqual _tmp5AC;struct Cyc_Absyn_PtrAtts _tmp5AD;void*_tmp5AE;
union Cyc_Absyn_Constraint*_tmp5AF;union Cyc_Absyn_Constraint*_tmp5B0;union Cyc_Absyn_Constraint*
_tmp5B1;struct Cyc_Absyn_ArrayInfo _tmp5B2;void*_tmp5B3;struct Cyc_Absyn_Tqual
_tmp5B4;struct Cyc_Absyn_Exp*_tmp5B5;union Cyc_Absyn_Constraint*_tmp5B6;struct Cyc_Absyn_Enumdecl*
_tmp5B7;void*_tmp5B8;_LL3CF: if(_tmp5A9 <= (void*)4)goto _LL3D7;if(*((int*)_tmp5A9)
!= 4)goto _LL3D1;_tmp5AA=((struct Cyc_Absyn_PointerType_struct*)_tmp5A9)->f1;
_tmp5AB=_tmp5AA.elt_typ;_tmp5AC=_tmp5AA.elt_tq;_tmp5AD=_tmp5AA.ptr_atts;_tmp5AE=
_tmp5AD.rgn;_tmp5AF=_tmp5AD.nullable;_tmp5B0=_tmp5AD.bounds;_tmp5B1=_tmp5AD.zero_term;
_LL3D0:{void*_tmp5B9=t2;struct Cyc_Absyn_PtrInfo _tmp5BA;void*_tmp5BB;struct Cyc_Absyn_Tqual
_tmp5BC;struct Cyc_Absyn_PtrAtts _tmp5BD;void*_tmp5BE;union Cyc_Absyn_Constraint*
_tmp5BF;union Cyc_Absyn_Constraint*_tmp5C0;union Cyc_Absyn_Constraint*_tmp5C1;
_LL3E0: if(_tmp5B9 <= (void*)4)goto _LL3E2;if(*((int*)_tmp5B9)!= 4)goto _LL3E2;
_tmp5BA=((struct Cyc_Absyn_PointerType_struct*)_tmp5B9)->f1;_tmp5BB=_tmp5BA.elt_typ;
_tmp5BC=_tmp5BA.elt_tq;_tmp5BD=_tmp5BA.ptr_atts;_tmp5BE=_tmp5BD.rgn;_tmp5BF=
_tmp5BD.nullable;_tmp5C0=_tmp5BD.bounds;_tmp5C1=_tmp5BD.zero_term;_LL3E1: {void*
coercion=(void*)3;struct _tuple0*_tmpD9A;struct Cyc_List_List*_tmpD99;struct Cyc_List_List*
_tmp5C2=(_tmpD99=_cycalloc(sizeof(*_tmpD99)),((_tmpD99->hd=((_tmpD9A=_cycalloc(
sizeof(*_tmpD9A)),((_tmpD9A->f1=t1,((_tmpD9A->f2=t2,_tmpD9A)))))),((_tmpD99->tl=
0,_tmpD99)))));int _tmp5C3=Cyc_Tcutil_ptrsubtype(te,_tmp5C2,_tmp5AB,_tmp5BB) && (
!_tmp5AC.real_const  || _tmp5BC.real_const);Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=
t2;{int zeroterm_ok=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union
Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp5B1,
_tmp5C1) || !((int(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp5C1);
int _tmp5C4=_tmp5C3?0:((Cyc_Tcutil_bits_only(_tmp5AB) && Cyc_Tcutil_is_char_type(
_tmp5BB)) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmp5C1)) && (_tmp5BC.real_const  || !_tmp5AC.real_const);int bounds_ok=Cyc_Tcutil_unify_conrefs(
Cyc_Tcutil_unify_it_bounds,_tmp5B0,_tmp5C0);if(!bounds_ok  && !_tmp5C4){struct
_tuple0 _tmpD9B;struct _tuple0 _tmp5C6=(_tmpD9B.f1=Cyc_Absyn_conref_val(_tmp5B0),((
_tmpD9B.f2=Cyc_Absyn_conref_val(_tmp5C0),_tmpD9B)));void*_tmp5C7;struct Cyc_Absyn_Exp*
_tmp5C8;void*_tmp5C9;struct Cyc_Absyn_Exp*_tmp5CA;_LL3E5: _tmp5C7=_tmp5C6.f1;if(
_tmp5C7 <= (void*)1)goto _LL3E7;if(*((int*)_tmp5C7)!= 0)goto _LL3E7;_tmp5C8=((
struct Cyc_Absyn_Upper_b_struct*)_tmp5C7)->f1;_tmp5C9=_tmp5C6.f2;if(_tmp5C9 <= (
void*)1)goto _LL3E7;if(*((int*)_tmp5C9)!= 0)goto _LL3E7;_tmp5CA=((struct Cyc_Absyn_Upper_b_struct*)
_tmp5C9)->f1;_LL3E6: if(Cyc_Evexp_lte_const_exp(_tmp5CA,_tmp5C8))bounds_ok=1;goto
_LL3E4;_LL3E7:;_LL3E8: bounds_ok=1;goto _LL3E4;_LL3E4:;}if(((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,_tmp5AF) && !((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,_tmp5BF))coercion=(void*)2;if(((bounds_ok  && 
zeroterm_ok) && (_tmp5C3  || _tmp5C4)) && (Cyc_Tcutil_unify(_tmp5AE,_tmp5BE) || 
Cyc_Tcenv_region_outlives(te,_tmp5AE,_tmp5BE)))return coercion;else{return(void*)
0;}}}_LL3E2:;_LL3E3: goto _LL3DF;_LL3DF:;}return(void*)0;_LL3D1: if(*((int*)_tmp5A9)
!= 7)goto _LL3D3;_tmp5B2=((struct Cyc_Absyn_ArrayType_struct*)_tmp5A9)->f1;_tmp5B3=
_tmp5B2.elt_type;_tmp5B4=_tmp5B2.tq;_tmp5B5=_tmp5B2.num_elts;_tmp5B6=_tmp5B2.zero_term;
_LL3D2:{void*_tmp5CD=t2;struct Cyc_Absyn_ArrayInfo _tmp5CE;void*_tmp5CF;struct Cyc_Absyn_Tqual
_tmp5D0;struct Cyc_Absyn_Exp*_tmp5D1;union Cyc_Absyn_Constraint*_tmp5D2;_LL3EA: if(
_tmp5CD <= (void*)4)goto _LL3EC;if(*((int*)_tmp5CD)!= 7)goto _LL3EC;_tmp5CE=((
struct Cyc_Absyn_ArrayType_struct*)_tmp5CD)->f1;_tmp5CF=_tmp5CE.elt_type;_tmp5D0=
_tmp5CE.tq;_tmp5D1=_tmp5CE.num_elts;_tmp5D2=_tmp5CE.zero_term;_LL3EB: {int okay;
okay=((_tmp5B5 != 0  && _tmp5D1 != 0) && ((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*
x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp5B6,
_tmp5D2)) && Cyc_Evexp_lte_const_exp((struct Cyc_Absyn_Exp*)_tmp5D1,(struct Cyc_Absyn_Exp*)
_tmp5B5);return(okay  && Cyc_Tcutil_unify(_tmp5B3,_tmp5CF)) && (!_tmp5B4.real_const
 || _tmp5D0.real_const)?(void*)3:(void*)0;}_LL3EC:;_LL3ED: return(void*)0;_LL3E9:;}
return(void*)0;_LL3D3: if(*((int*)_tmp5A9)!= 12)goto _LL3D5;_tmp5B7=((struct Cyc_Absyn_EnumType_struct*)
_tmp5A9)->f2;_LL3D4:{void*_tmp5D3=t2;struct Cyc_Absyn_Enumdecl*_tmp5D4;_LL3EF: if(
_tmp5D3 <= (void*)4)goto _LL3F1;if(*((int*)_tmp5D3)!= 12)goto _LL3F1;_tmp5D4=((
struct Cyc_Absyn_EnumType_struct*)_tmp5D3)->f2;_LL3F0: if((_tmp5B7->fields != 0  && 
_tmp5D4->fields != 0) && ((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct
Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp5B7->fields))->v)>= ((int(*)(
struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp5D4->fields))->v))return(void*)1;goto _LL3EE;_LL3F1:;_LL3F2: goto
_LL3EE;_LL3EE:;}goto _LL3D6;_LL3D5: if(*((int*)_tmp5A9)!= 5)goto _LL3D7;_LL3D6: goto
_LL3D8;_LL3D7: if((int)_tmp5A9 != 1)goto _LL3D9;_LL3D8: goto _LL3DA;_LL3D9: if(_tmp5A9
<= (void*)4)goto _LL3DD;if(*((int*)_tmp5A9)!= 6)goto _LL3DB;_LL3DA: return Cyc_Tcutil_coerceable(
t2)?(void*)1:(void*)0;_LL3DB: if(*((int*)_tmp5A9)!= 14)goto _LL3DD;_tmp5B8=(void*)((
struct Cyc_Absyn_RgnHandleType_struct*)_tmp5A9)->f1;_LL3DC:{void*_tmp5D5=t2;void*
_tmp5D6;_LL3F4: if(_tmp5D5 <= (void*)4)goto _LL3F6;if(*((int*)_tmp5D5)!= 14)goto
_LL3F6;_tmp5D6=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)_tmp5D5)->f1;
_LL3F5: if(Cyc_Tcenv_region_outlives(te,_tmp5B8,_tmp5D6))return(void*)1;goto
_LL3F3;_LL3F6:;_LL3F7: goto _LL3F3;_LL3F3:;}return(void*)0;_LL3DD:;_LL3DE: return(
void*)0;_LL3CE:;}}void Cyc_Tcutil_unchecked_cast(struct Cyc_Tcenv_Tenv*te,struct
Cyc_Absyn_Exp*e,void*t,void*c);void Cyc_Tcutil_unchecked_cast(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e,void*t,void*c){if(!Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,t)){struct Cyc_Absyn_Exp*_tmp5D7=Cyc_Absyn_copy_exp(e);{
struct Cyc_Absyn_Cast_e_struct _tmpD9E;struct Cyc_Absyn_Cast_e_struct*_tmpD9D;e->r=(
void*)((_tmpD9D=_cycalloc(sizeof(*_tmpD9D)),((_tmpD9D[0]=((_tmpD9E.tag=15,((
_tmpD9E.f1=(void*)t,((_tmpD9E.f2=_tmp5D7,((_tmpD9E.f3=0,((_tmpD9E.f4=(void*)c,
_tmpD9E)))))))))),_tmpD9D))));}{struct Cyc_Core_Opt*_tmpD9F;e->topt=((_tmpD9F=
_cycalloc(sizeof(*_tmpD9F)),((_tmpD9F->v=(void*)t,_tmpD9F))));}}}int Cyc_Tcutil_is_integral(
struct Cyc_Absyn_Exp*e);int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*e){void*
_tmp5DB=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
_LL3F9: if(_tmp5DB <= (void*)4)goto _LL403;if(*((int*)_tmp5DB)!= 5)goto _LL3FB;
_LL3FA: goto _LL3FC;_LL3FB: if(*((int*)_tmp5DB)!= 12)goto _LL3FD;_LL3FC: goto _LL3FE;
_LL3FD: if(*((int*)_tmp5DB)!= 13)goto _LL3FF;_LL3FE: goto _LL400;_LL3FF: if(*((int*)
_tmp5DB)!= 18)goto _LL401;_LL400: return 1;_LL401: if(*((int*)_tmp5DB)!= 0)goto
_LL403;_LL402: return Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,
Cyc_Absyn_sint_typ);_LL403:;_LL404: return 0;_LL3F8:;}int Cyc_Tcutil_is_numeric(
struct Cyc_Absyn_Exp*e);int Cyc_Tcutil_is_numeric(struct Cyc_Absyn_Exp*e){if(Cyc_Tcutil_is_integral(
e))return 1;{void*_tmp5DC=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);_LL406: if((int)_tmp5DC != 1)goto _LL408;_LL407: goto
_LL409;_LL408: if(_tmp5DC <= (void*)4)goto _LL40A;if(*((int*)_tmp5DC)!= 6)goto
_LL40A;_LL409: return 1;_LL40A:;_LL40B: return 0;_LL405:;}}int Cyc_Tcutil_is_function_type(
void*t);int Cyc_Tcutil_is_function_type(void*t){void*_tmp5DD=Cyc_Tcutil_compress(
t);_LL40D: if(_tmp5DD <= (void*)4)goto _LL40F;if(*((int*)_tmp5DD)!= 8)goto _LL40F;
_LL40E: return 1;_LL40F:;_LL410: return 0;_LL40C:;}void*Cyc_Tcutil_max_arithmetic_type(
void*t1,void*t2);void*Cyc_Tcutil_max_arithmetic_type(void*t1,void*t2){struct
_tuple0 _tmpDA0;struct _tuple0 _tmp5DF=(_tmpDA0.f1=t1,((_tmpDA0.f2=t2,_tmpDA0)));
void*_tmp5E0;int _tmp5E1;void*_tmp5E2;int _tmp5E3;void*_tmp5E4;void*_tmp5E5;void*
_tmp5E6;void*_tmp5E7;void*_tmp5E8;void*_tmp5E9;void*_tmp5EA;void*_tmp5EB;void*
_tmp5EC;void*_tmp5ED;void*_tmp5EE;void*_tmp5EF;void*_tmp5F0;void*_tmp5F1;void*
_tmp5F2;void*_tmp5F3;void*_tmp5F4;void*_tmp5F5;void*_tmp5F6;void*_tmp5F7;void*
_tmp5F8;void*_tmp5F9;void*_tmp5FA;void*_tmp5FB;void*_tmp5FC;void*_tmp5FD;void*
_tmp5FE;void*_tmp5FF;void*_tmp600;void*_tmp601;_LL412: _tmp5E0=_tmp5DF.f1;if(
_tmp5E0 <= (void*)4)goto _LL414;if(*((int*)_tmp5E0)!= 6)goto _LL414;_tmp5E1=((
struct Cyc_Absyn_DoubleType_struct*)_tmp5E0)->f1;_tmp5E2=_tmp5DF.f2;if(_tmp5E2 <= (
void*)4)goto _LL414;if(*((int*)_tmp5E2)!= 6)goto _LL414;_tmp5E3=((struct Cyc_Absyn_DoubleType_struct*)
_tmp5E2)->f1;_LL413: if(_tmp5E1)return t1;else{return t2;}_LL414: _tmp5E4=_tmp5DF.f1;
if(_tmp5E4 <= (void*)4)goto _LL416;if(*((int*)_tmp5E4)!= 6)goto _LL416;_LL415:
return t1;_LL416: _tmp5E5=_tmp5DF.f2;if(_tmp5E5 <= (void*)4)goto _LL418;if(*((int*)
_tmp5E5)!= 6)goto _LL418;_LL417: return t2;_LL418: _tmp5E6=_tmp5DF.f1;if((int)
_tmp5E6 != 1)goto _LL41A;_LL419: goto _LL41B;_LL41A: _tmp5E7=_tmp5DF.f2;if((int)
_tmp5E7 != 1)goto _LL41C;_LL41B: return(void*)1;_LL41C: _tmp5E8=_tmp5DF.f1;if(
_tmp5E8 <= (void*)4)goto _LL41E;if(*((int*)_tmp5E8)!= 5)goto _LL41E;_tmp5E9=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp5E8)->f1;if((int)_tmp5E9 != 1)goto _LL41E;
_tmp5EA=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp5E8)->f2;if((int)_tmp5EA != 
4)goto _LL41E;_LL41D: goto _LL41F;_LL41E: _tmp5EB=_tmp5DF.f2;if(_tmp5EB <= (void*)4)
goto _LL420;if(*((int*)_tmp5EB)!= 5)goto _LL420;_tmp5EC=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp5EB)->f1;if((int)_tmp5EC != 1)goto _LL420;_tmp5ED=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp5EB)->f2;if((int)_tmp5ED != 4)goto _LL420;_LL41F: return Cyc_Absyn_ulonglong_typ;
_LL420: _tmp5EE=_tmp5DF.f1;if(_tmp5EE <= (void*)4)goto _LL422;if(*((int*)_tmp5EE)!= 
5)goto _LL422;_tmp5EF=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp5EE)->f2;if((
int)_tmp5EF != 4)goto _LL422;_LL421: goto _LL423;_LL422: _tmp5F0=_tmp5DF.f2;if(
_tmp5F0 <= (void*)4)goto _LL424;if(*((int*)_tmp5F0)!= 5)goto _LL424;_tmp5F1=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp5F0)->f2;if((int)_tmp5F1 != 4)goto _LL424;
_LL423: return Cyc_Absyn_slonglong_typ;_LL424: _tmp5F2=_tmp5DF.f1;if(_tmp5F2 <= (
void*)4)goto _LL426;if(*((int*)_tmp5F2)!= 5)goto _LL426;_tmp5F3=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp5F2)->f1;if((int)_tmp5F3 != 1)goto _LL426;_tmp5F4=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp5F2)->f2;if((int)_tmp5F4 != 3)goto _LL426;_LL425: goto _LL427;_LL426: _tmp5F5=
_tmp5DF.f2;if(_tmp5F5 <= (void*)4)goto _LL428;if(*((int*)_tmp5F5)!= 5)goto _LL428;
_tmp5F6=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp5F5)->f1;if((int)_tmp5F6 != 
1)goto _LL428;_tmp5F7=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp5F5)->f2;if((
int)_tmp5F7 != 3)goto _LL428;_LL427: return Cyc_Absyn_ulong_typ;_LL428: _tmp5F8=
_tmp5DF.f1;if(_tmp5F8 <= (void*)4)goto _LL42A;if(*((int*)_tmp5F8)!= 5)goto _LL42A;
_tmp5F9=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp5F8)->f1;if((int)_tmp5F9 != 
1)goto _LL42A;_tmp5FA=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp5F8)->f2;if((
int)_tmp5FA != 2)goto _LL42A;_LL429: goto _LL42B;_LL42A: _tmp5FB=_tmp5DF.f2;if(
_tmp5FB <= (void*)4)goto _LL42C;if(*((int*)_tmp5FB)!= 5)goto _LL42C;_tmp5FC=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp5FB)->f1;if((int)_tmp5FC != 1)goto _LL42C;
_tmp5FD=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp5FB)->f2;if((int)_tmp5FD != 
2)goto _LL42C;_LL42B: return Cyc_Absyn_uint_typ;_LL42C: _tmp5FE=_tmp5DF.f1;if(
_tmp5FE <= (void*)4)goto _LL42E;if(*((int*)_tmp5FE)!= 5)goto _LL42E;_tmp5FF=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp5FE)->f2;if((int)_tmp5FF != 3)goto _LL42E;
_LL42D: goto _LL42F;_LL42E: _tmp600=_tmp5DF.f2;if(_tmp600 <= (void*)4)goto _LL430;if(*((
int*)_tmp600)!= 5)goto _LL430;_tmp601=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp600)->f2;if((int)_tmp601 != 3)goto _LL430;_LL42F: return Cyc_Absyn_slong_typ;
_LL430:;_LL431: return Cyc_Absyn_sint_typ;_LL411:;}void Cyc_Tcutil_check_contains_assign(
struct Cyc_Absyn_Exp*e);void Cyc_Tcutil_check_contains_assign(struct Cyc_Absyn_Exp*
e){void*_tmp602=e->r;struct Cyc_Core_Opt*_tmp603;_LL433: if(*((int*)_tmp602)!= 4)
goto _LL435;_tmp603=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp602)->f2;if(_tmp603
!= 0)goto _LL435;_LL434:{const char*_tmpDA3;void*_tmpDA2;(_tmpDA2=0,Cyc_Tcutil_warn(
e->loc,((_tmpDA3="assignment in test",_tag_dyneither(_tmpDA3,sizeof(char),19))),
_tag_dyneither(_tmpDA2,sizeof(void*),0)));}goto _LL432;_LL435:;_LL436: goto _LL432;
_LL432:;}static int Cyc_Tcutil_constrain_kinds(void*c1,void*c2);static int Cyc_Tcutil_constrain_kinds(
void*c1,void*c2){c1=Cyc_Absyn_compress_kb(c1);c2=Cyc_Absyn_compress_kb(c2);{
struct _tuple0 _tmpDA4;struct _tuple0 _tmp607=(_tmpDA4.f1=c1,((_tmpDA4.f2=c2,_tmpDA4)));
void*_tmp608;void*_tmp609;void*_tmp60A;void*_tmp60B;void*_tmp60C;struct Cyc_Core_Opt*
_tmp60D;struct Cyc_Core_Opt**_tmp60E;void*_tmp60F;struct Cyc_Core_Opt*_tmp610;
struct Cyc_Core_Opt**_tmp611;void*_tmp612;struct Cyc_Core_Opt*_tmp613;struct Cyc_Core_Opt**
_tmp614;void*_tmp615;void*_tmp616;void*_tmp617;void*_tmp618;void*_tmp619;void*
_tmp61A;struct Cyc_Core_Opt*_tmp61B;struct Cyc_Core_Opt**_tmp61C;void*_tmp61D;void*
_tmp61E;struct Cyc_Core_Opt*_tmp61F;struct Cyc_Core_Opt**_tmp620;void*_tmp621;void*
_tmp622;struct Cyc_Core_Opt*_tmp623;struct Cyc_Core_Opt**_tmp624;void*_tmp625;
_LL438: _tmp608=_tmp607.f1;if(*((int*)_tmp608)!= 0)goto _LL43A;_tmp609=(void*)((
struct Cyc_Absyn_Eq_kb_struct*)_tmp608)->f1;_tmp60A=_tmp607.f2;if(*((int*)_tmp60A)
!= 0)goto _LL43A;_tmp60B=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp60A)->f1;
_LL439: return _tmp609 == _tmp60B;_LL43A: _tmp60C=_tmp607.f2;if(*((int*)_tmp60C)!= 1)
goto _LL43C;_tmp60D=((struct Cyc_Absyn_Unknown_kb_struct*)_tmp60C)->f1;_tmp60E=(
struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)_tmp60C)->f1;_LL43B:{
struct Cyc_Core_Opt*_tmpDA5;*_tmp60E=((_tmpDA5=_cycalloc(sizeof(*_tmpDA5)),((
_tmpDA5->v=(void*)c1,_tmpDA5))));}return 1;_LL43C: _tmp60F=_tmp607.f1;if(*((int*)
_tmp60F)!= 1)goto _LL43E;_tmp610=((struct Cyc_Absyn_Unknown_kb_struct*)_tmp60F)->f1;
_tmp611=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)_tmp60F)->f1;
_LL43D:{struct Cyc_Core_Opt*_tmpDA6;*_tmp611=((_tmpDA6=_cycalloc(sizeof(*_tmpDA6)),((
_tmpDA6->v=(void*)c2,_tmpDA6))));}return 1;_LL43E: _tmp612=_tmp607.f1;if(*((int*)
_tmp612)!= 2)goto _LL440;_tmp613=((struct Cyc_Absyn_Less_kb_struct*)_tmp612)->f1;
_tmp614=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)_tmp612)->f1;
_tmp615=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp612)->f2;_tmp616=_tmp607.f2;
if(*((int*)_tmp616)!= 0)goto _LL440;_tmp617=(void*)((struct Cyc_Absyn_Eq_kb_struct*)
_tmp616)->f1;_LL43F: if(Cyc_Tcutil_kind_leq(_tmp617,_tmp615)){{struct Cyc_Core_Opt*
_tmpDA7;*_tmp614=((_tmpDA7=_cycalloc(sizeof(*_tmpDA7)),((_tmpDA7->v=(void*)c2,
_tmpDA7))));}return 1;}else{return 0;}_LL440: _tmp618=_tmp607.f1;if(*((int*)_tmp618)
!= 0)goto _LL442;_tmp619=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp618)->f1;
_tmp61A=_tmp607.f2;if(*((int*)_tmp61A)!= 2)goto _LL442;_tmp61B=((struct Cyc_Absyn_Less_kb_struct*)
_tmp61A)->f1;_tmp61C=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp61A)->f1;_tmp61D=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp61A)->f2;
_LL441: if(Cyc_Tcutil_kind_leq(_tmp619,_tmp61D)){{struct Cyc_Core_Opt*_tmpDA8;*
_tmp61C=((_tmpDA8=_cycalloc(sizeof(*_tmpDA8)),((_tmpDA8->v=(void*)c1,_tmpDA8))));}
return 1;}else{return 0;}_LL442: _tmp61E=_tmp607.f1;if(*((int*)_tmp61E)!= 2)goto
_LL437;_tmp61F=((struct Cyc_Absyn_Less_kb_struct*)_tmp61E)->f1;_tmp620=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*)_tmp61E)->f1;_tmp621=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp61E)->f2;_tmp622=_tmp607.f2;if(*((int*)_tmp622)!= 2)goto _LL437;_tmp623=((
struct Cyc_Absyn_Less_kb_struct*)_tmp622)->f1;_tmp624=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*)_tmp622)->f1;_tmp625=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp622)->f2;_LL443: if(Cyc_Tcutil_kind_leq(_tmp621,_tmp625)){{struct Cyc_Core_Opt*
_tmpDA9;*_tmp624=((_tmpDA9=_cycalloc(sizeof(*_tmpDA9)),((_tmpDA9->v=(void*)c1,
_tmpDA9))));}return 1;}else{if(Cyc_Tcutil_kind_leq(_tmp625,_tmp621)){{struct Cyc_Core_Opt*
_tmpDAA;*_tmp620=((_tmpDAA=_cycalloc(sizeof(*_tmpDAA)),((_tmpDAA->v=(void*)c2,
_tmpDAA))));}return 1;}else{return 0;}}_LL437:;}}static int Cyc_Tcutil_tvar_id_counter=
0;int Cyc_Tcutil_new_tvar_id();int Cyc_Tcutil_new_tvar_id(){return Cyc_Tcutil_tvar_id_counter
++;}static int Cyc_Tcutil_tvar_counter=0;struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(
void*k);struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k){int i=Cyc_Tcutil_tvar_counter
++;const char*_tmpDAE;void*_tmpDAD[1];struct Cyc_Int_pa_struct _tmpDAC;struct
_dyneither_ptr s=(struct _dyneither_ptr)((_tmpDAC.tag=1,((_tmpDAC.f1=(
unsigned long)i,((_tmpDAD[0]=& _tmpDAC,Cyc_aprintf(((_tmpDAE="#%d",_tag_dyneither(
_tmpDAE,sizeof(char),4))),_tag_dyneither(_tmpDAD,sizeof(void*),1))))))));struct
_dyneither_ptr*_tmpDB1;struct Cyc_Absyn_Tvar*_tmpDB0;return(_tmpDB0=_cycalloc(
sizeof(*_tmpDB0)),((_tmpDB0->name=((_tmpDB1=_cycalloc(sizeof(struct
_dyneither_ptr)* 1),((_tmpDB1[0]=s,_tmpDB1)))),((_tmpDB0->identity=- 1,((_tmpDB0->kind=
k,_tmpDB0)))))));}int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*t);int Cyc_Tcutil_is_temp_tvar(
struct Cyc_Absyn_Tvar*t){struct _dyneither_ptr _tmp631=*t->name;return*((const char*)
_check_dyneither_subscript(_tmp631,sizeof(char),0))== '#';}void Cyc_Tcutil_rewrite_temp_tvar(
struct Cyc_Absyn_Tvar*t);void Cyc_Tcutil_rewrite_temp_tvar(struct Cyc_Absyn_Tvar*t){{
const char*_tmpDB5;void*_tmpDB4[1];struct Cyc_String_pa_struct _tmpDB3;(_tmpDB3.tag=
0,((_tmpDB3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*t->name),((_tmpDB4[
0]=& _tmpDB3,Cyc_printf(((_tmpDB5="%s",_tag_dyneither(_tmpDB5,sizeof(char),3))),
_tag_dyneither(_tmpDB4,sizeof(void*),1)))))));}if(!Cyc_Tcutil_is_temp_tvar(t))
return;{const char*_tmpDB6;struct _dyneither_ptr _tmp635=Cyc_strconcat(((_tmpDB6="`",
_tag_dyneither(_tmpDB6,sizeof(char),2))),(struct _dyneither_ptr)*t->name);{char
_tmpDB9;char _tmpDB8;struct _dyneither_ptr _tmpDB7;(_tmpDB7=_dyneither_ptr_plus(
_tmp635,sizeof(char),1),((_tmpDB8=*((char*)_check_dyneither_subscript(_tmpDB7,
sizeof(char),0)),((_tmpDB9='t',((_get_dyneither_size(_tmpDB7,sizeof(char))== 1
 && (_tmpDB8 == '\000'  && _tmpDB9 != '\000')?_throw_arraybounds(): 1,*((char*)
_tmpDB7.curr)=_tmpDB9)))))));}{struct _dyneither_ptr*_tmpDBA;t->name=((_tmpDBA=
_cycalloc(sizeof(struct _dyneither_ptr)* 1),((_tmpDBA[0]=(struct _dyneither_ptr)
_tmp635,_tmpDBA))));}}}struct _tuple19{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual
f2;void*f3;};static struct _tuple9*Cyc_Tcutil_fndecl2typ_f(struct _tuple19*x);
static struct _tuple9*Cyc_Tcutil_fndecl2typ_f(struct _tuple19*x){struct Cyc_Core_Opt*
_tmpDBD;struct _tuple9*_tmpDBC;return(_tmpDBC=_cycalloc(sizeof(*_tmpDBC)),((
_tmpDBC->f1=(struct Cyc_Core_Opt*)((_tmpDBD=_cycalloc(sizeof(*_tmpDBD)),((_tmpDBD->v=(*
x).f1,_tmpDBD)))),((_tmpDBC->f2=(*x).f2,((_tmpDBC->f3=(*x).f3,_tmpDBC)))))));}
void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*fd);void*Cyc_Tcutil_fndecl2typ(
struct Cyc_Absyn_Fndecl*fd){if(fd->cached_typ == 0){struct Cyc_List_List*_tmp63D=0;{
struct Cyc_List_List*atts=fd->attributes;for(0;atts != 0;atts=atts->tl){if(Cyc_Absyn_fntype_att((
void*)atts->hd)){struct Cyc_List_List*_tmpDBE;_tmp63D=((_tmpDBE=_cycalloc(sizeof(*
_tmpDBE)),((_tmpDBE->hd=(void*)((void*)atts->hd),((_tmpDBE->tl=_tmp63D,_tmpDBE))))));}}}{
struct Cyc_Absyn_FnType_struct _tmpDC4;struct Cyc_Absyn_FnInfo _tmpDC3;struct Cyc_Absyn_FnType_struct*
_tmpDC2;return(void*)((_tmpDC2=_cycalloc(sizeof(*_tmpDC2)),((_tmpDC2[0]=((
_tmpDC4.tag=8,((_tmpDC4.f1=((_tmpDC3.tvars=fd->tvs,((_tmpDC3.effect=fd->effect,((
_tmpDC3.ret_typ=fd->ret_type,((_tmpDC3.args=((struct Cyc_List_List*(*)(struct
_tuple9*(*f)(struct _tuple19*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_fndecl2typ_f,
fd->args),((_tmpDC3.c_varargs=fd->c_varargs,((_tmpDC3.cyc_varargs=fd->cyc_varargs,((
_tmpDC3.rgn_po=fd->rgn_po,((_tmpDC3.attributes=_tmp63D,_tmpDC3)))))))))))))))),
_tmpDC4)))),_tmpDC2))));}}return(void*)((struct Cyc_Core_Opt*)_check_null(fd->cached_typ))->v;}
struct _tuple20{void*f1;struct Cyc_Absyn_Tqual f2;void*f3;};static void*Cyc_Tcutil_fst_fdarg(
struct _tuple20*t);static void*Cyc_Tcutil_fst_fdarg(struct _tuple20*t){return(*t).f1;}
void*Cyc_Tcutil_snd_tqt(struct _tuple11*t);void*Cyc_Tcutil_snd_tqt(struct _tuple11*
t){return(*t).f2;}static struct _tuple11*Cyc_Tcutil_map2_tq(struct _tuple11*pr,void*
t);static struct _tuple11*Cyc_Tcutil_map2_tq(struct _tuple11*pr,void*t){struct
_tuple11*_tmpDC5;return(_tmpDC5=_cycalloc(sizeof(*_tmpDC5)),((_tmpDC5->f1=(*pr).f1,((
_tmpDC5->f2=t,_tmpDC5)))));}struct _tuple21{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Tqual
f2;};struct _tuple22{struct _tuple21*f1;void*f2;};static struct _tuple22*Cyc_Tcutil_substitute_f1(
struct _RegionHandle*rgn,struct _tuple9*y);static struct _tuple22*Cyc_Tcutil_substitute_f1(
struct _RegionHandle*rgn,struct _tuple9*y){struct _tuple21*_tmpDC8;struct _tuple22*
_tmpDC7;return(_tmpDC7=_region_malloc(rgn,sizeof(*_tmpDC7)),((_tmpDC7->f1=((
_tmpDC8=_region_malloc(rgn,sizeof(*_tmpDC8)),((_tmpDC8->f1=(*y).f1,((_tmpDC8->f2=(*
y).f2,_tmpDC8)))))),((_tmpDC7->f2=(*y).f3,_tmpDC7)))));}static struct _tuple9*Cyc_Tcutil_substitute_f2(
struct _tuple22*w);static struct _tuple9*Cyc_Tcutil_substitute_f2(struct _tuple22*w){
struct _tuple21*_tmp646;void*_tmp647;struct _tuple22 _tmp645=*w;_tmp646=_tmp645.f1;
_tmp647=_tmp645.f2;{struct Cyc_Core_Opt*_tmp649;struct Cyc_Absyn_Tqual _tmp64A;
struct _tuple21 _tmp648=*_tmp646;_tmp649=_tmp648.f1;_tmp64A=_tmp648.f2;{struct
_tuple9*_tmpDC9;return(_tmpDC9=_cycalloc(sizeof(*_tmpDC9)),((_tmpDC9->f1=_tmp649,((
_tmpDC9->f2=_tmp64A,((_tmpDC9->f3=_tmp647,_tmpDC9)))))));}}}static void*Cyc_Tcutil_field_type(
struct Cyc_Absyn_Aggrfield*f);static void*Cyc_Tcutil_field_type(struct Cyc_Absyn_Aggrfield*
f){return f->type;}static struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_zip_field_type(
struct Cyc_Absyn_Aggrfield*f,void*t);static struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_zip_field_type(
struct Cyc_Absyn_Aggrfield*f,void*t){struct Cyc_Absyn_Aggrfield*_tmpDCA;return(
_tmpDCA=_cycalloc(sizeof(*_tmpDCA)),((_tmpDCA->name=f->name,((_tmpDCA->tq=f->tq,((
_tmpDCA->type=t,((_tmpDCA->width=f->width,((_tmpDCA->attributes=f->attributes,
_tmpDCA)))))))))));}static struct Cyc_List_List*Cyc_Tcutil_substs(struct
_RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*ts);static struct
Cyc_Absyn_Exp*Cyc_Tcutil_copye(struct Cyc_Absyn_Exp*old,void*r);static struct Cyc_Absyn_Exp*
Cyc_Tcutil_copye(struct Cyc_Absyn_Exp*old,void*r){struct Cyc_Absyn_Exp*_tmpDCB;
return(_tmpDCB=_cycalloc(sizeof(*_tmpDCB)),((_tmpDCB->topt=old->topt,((_tmpDCB->r=
r,((_tmpDCB->loc=old->loc,((_tmpDCB->annot=old->annot,_tmpDCB)))))))));}static
struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubsexp(struct _RegionHandle*r,struct Cyc_List_List*
inst,struct Cyc_Absyn_Exp*e);static struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubsexp(struct
_RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_Absyn_Exp*e){void*_tmp64E=e->r;
void*_tmp64F;struct Cyc_List_List*_tmp650;struct Cyc_Absyn_Exp*_tmp651;struct Cyc_Absyn_Exp*
_tmp652;struct Cyc_Absyn_Exp*_tmp653;struct Cyc_Absyn_Exp*_tmp654;struct Cyc_Absyn_Exp*
_tmp655;struct Cyc_Absyn_Exp*_tmp656;struct Cyc_Absyn_Exp*_tmp657;struct Cyc_Absyn_Exp*
_tmp658;struct Cyc_Absyn_Exp*_tmp659;void*_tmp65A;struct Cyc_Absyn_Exp*_tmp65B;int
_tmp65C;void*_tmp65D;void*_tmp65E;struct Cyc_Absyn_Exp*_tmp65F;void*_tmp660;void*
_tmp661;void*_tmp662;_LL445: if(*((int*)_tmp64E)!= 0)goto _LL447;_LL446: goto _LL448;
_LL447: if(*((int*)_tmp64E)!= 33)goto _LL449;_LL448: goto _LL44A;_LL449: if(*((int*)
_tmp64E)!= 34)goto _LL44B;_LL44A: goto _LL44C;_LL44B: if(*((int*)_tmp64E)!= 1)goto
_LL44D;_LL44C: return e;_LL44D: if(*((int*)_tmp64E)!= 3)goto _LL44F;_tmp64F=(void*)((
struct Cyc_Absyn_Primop_e_struct*)_tmp64E)->f1;_tmp650=((struct Cyc_Absyn_Primop_e_struct*)
_tmp64E)->f2;_LL44E: if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp650)
== 1){struct Cyc_Absyn_Exp*_tmp663=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp650))->hd;struct Cyc_Absyn_Exp*_tmp664=Cyc_Tcutil_rsubsexp(r,inst,
_tmp663);if(_tmp664 == _tmp663)return e;{struct Cyc_Absyn_Primop_e_struct _tmpDD1;
struct Cyc_Absyn_Exp*_tmpDD0[1];struct Cyc_Absyn_Primop_e_struct*_tmpDCF;return Cyc_Tcutil_copye(
e,(void*)((_tmpDCF=_cycalloc(sizeof(*_tmpDCF)),((_tmpDCF[0]=((_tmpDD1.tag=3,((
_tmpDD1.f1=(void*)_tmp64F,((_tmpDD1.f2=((_tmpDD0[0]=_tmp664,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpDD0,sizeof(struct Cyc_Absyn_Exp*),
1)))),_tmpDD1)))))),_tmpDCF)))));}}else{if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp650)== 2){struct Cyc_Absyn_Exp*_tmp668=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp650))->hd;struct Cyc_Absyn_Exp*_tmp669=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp650->tl))->hd;struct Cyc_Absyn_Exp*_tmp66A=
Cyc_Tcutil_rsubsexp(r,inst,_tmp668);struct Cyc_Absyn_Exp*_tmp66B=Cyc_Tcutil_rsubsexp(
r,inst,_tmp669);if(_tmp66A == _tmp668  && _tmp66B == _tmp669)return e;{struct Cyc_Absyn_Primop_e_struct
_tmpDD7;struct Cyc_Absyn_Exp*_tmpDD6[2];struct Cyc_Absyn_Primop_e_struct*_tmpDD5;
return Cyc_Tcutil_copye(e,(void*)((_tmpDD5=_cycalloc(sizeof(*_tmpDD5)),((_tmpDD5[
0]=((_tmpDD7.tag=3,((_tmpDD7.f1=(void*)_tmp64F,((_tmpDD7.f2=((_tmpDD6[1]=_tmp66B,((
_tmpDD6[0]=_tmp66A,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpDD6,sizeof(struct Cyc_Absyn_Exp*),2)))))),_tmpDD7)))))),
_tmpDD5)))));}}else{const char*_tmpDDA;void*_tmpDD9;return(_tmpDD9=0,((struct Cyc_Absyn_Exp*(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpDDA="primop does not have 1 or 2 args!",
_tag_dyneither(_tmpDDA,sizeof(char),34))),_tag_dyneither(_tmpDD9,sizeof(void*),0)));}}
_LL44F: if(*((int*)_tmp64E)!= 6)goto _LL451;_tmp651=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp64E)->f1;_tmp652=((struct Cyc_Absyn_Conditional_e_struct*)_tmp64E)->f2;
_tmp653=((struct Cyc_Absyn_Conditional_e_struct*)_tmp64E)->f3;_LL450: {struct Cyc_Absyn_Exp*
_tmp671=Cyc_Tcutil_rsubsexp(r,inst,_tmp651);struct Cyc_Absyn_Exp*_tmp672=Cyc_Tcutil_rsubsexp(
r,inst,_tmp652);struct Cyc_Absyn_Exp*_tmp673=Cyc_Tcutil_rsubsexp(r,inst,_tmp653);
if((_tmp671 == _tmp651  && _tmp672 == _tmp652) && _tmp673 == _tmp653)return e;{struct
Cyc_Absyn_Conditional_e_struct _tmpDDD;struct Cyc_Absyn_Conditional_e_struct*
_tmpDDC;return Cyc_Tcutil_copye(e,(void*)((_tmpDDC=_cycalloc(sizeof(*_tmpDDC)),((
_tmpDDC[0]=((_tmpDDD.tag=6,((_tmpDDD.f1=_tmp671,((_tmpDDD.f2=_tmp672,((_tmpDDD.f3=
_tmp673,_tmpDDD)))))))),_tmpDDC)))));}}_LL451: if(*((int*)_tmp64E)!= 7)goto _LL453;
_tmp654=((struct Cyc_Absyn_And_e_struct*)_tmp64E)->f1;_tmp655=((struct Cyc_Absyn_And_e_struct*)
_tmp64E)->f2;_LL452: {struct Cyc_Absyn_Exp*_tmp676=Cyc_Tcutil_rsubsexp(r,inst,
_tmp654);struct Cyc_Absyn_Exp*_tmp677=Cyc_Tcutil_rsubsexp(r,inst,_tmp655);if(
_tmp676 == _tmp654  && _tmp677 == _tmp655)return e;{struct Cyc_Absyn_And_e_struct
_tmpDE0;struct Cyc_Absyn_And_e_struct*_tmpDDF;return Cyc_Tcutil_copye(e,(void*)((
_tmpDDF=_cycalloc(sizeof(*_tmpDDF)),((_tmpDDF[0]=((_tmpDE0.tag=7,((_tmpDE0.f1=
_tmp676,((_tmpDE0.f2=_tmp677,_tmpDE0)))))),_tmpDDF)))));}}_LL453: if(*((int*)
_tmp64E)!= 8)goto _LL455;_tmp656=((struct Cyc_Absyn_Or_e_struct*)_tmp64E)->f1;
_tmp657=((struct Cyc_Absyn_Or_e_struct*)_tmp64E)->f2;_LL454: {struct Cyc_Absyn_Exp*
_tmp67A=Cyc_Tcutil_rsubsexp(r,inst,_tmp656);struct Cyc_Absyn_Exp*_tmp67B=Cyc_Tcutil_rsubsexp(
r,inst,_tmp657);if(_tmp67A == _tmp656  && _tmp67B == _tmp657)return e;{struct Cyc_Absyn_Or_e_struct
_tmpDE3;struct Cyc_Absyn_Or_e_struct*_tmpDE2;return Cyc_Tcutil_copye(e,(void*)((
_tmpDE2=_cycalloc(sizeof(*_tmpDE2)),((_tmpDE2[0]=((_tmpDE3.tag=8,((_tmpDE3.f1=
_tmp67A,((_tmpDE3.f2=_tmp67B,_tmpDE3)))))),_tmpDE2)))));}}_LL455: if(*((int*)
_tmp64E)!= 9)goto _LL457;_tmp658=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp64E)->f1;
_tmp659=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp64E)->f2;_LL456: {struct Cyc_Absyn_Exp*
_tmp67E=Cyc_Tcutil_rsubsexp(r,inst,_tmp658);struct Cyc_Absyn_Exp*_tmp67F=Cyc_Tcutil_rsubsexp(
r,inst,_tmp659);if(_tmp67E == _tmp658  && _tmp67F == _tmp659)return e;{struct Cyc_Absyn_SeqExp_e_struct
_tmpDE6;struct Cyc_Absyn_SeqExp_e_struct*_tmpDE5;return Cyc_Tcutil_copye(e,(void*)((
_tmpDE5=_cycalloc(sizeof(*_tmpDE5)),((_tmpDE5[0]=((_tmpDE6.tag=9,((_tmpDE6.f1=
_tmp67E,((_tmpDE6.f2=_tmp67F,_tmpDE6)))))),_tmpDE5)))));}}_LL457: if(*((int*)
_tmp64E)!= 15)goto _LL459;_tmp65A=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp64E)->f1;
_tmp65B=((struct Cyc_Absyn_Cast_e_struct*)_tmp64E)->f2;_tmp65C=((struct Cyc_Absyn_Cast_e_struct*)
_tmp64E)->f3;_tmp65D=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp64E)->f4;_LL458: {
struct Cyc_Absyn_Exp*_tmp682=Cyc_Tcutil_rsubsexp(r,inst,_tmp65B);void*_tmp683=Cyc_Tcutil_rsubstitute(
r,inst,_tmp65A);if(_tmp682 == _tmp65B  && _tmp683 == _tmp65A)return e;{struct Cyc_Absyn_Cast_e_struct
_tmpDE9;struct Cyc_Absyn_Cast_e_struct*_tmpDE8;return Cyc_Tcutil_copye(e,(void*)((
_tmpDE8=_cycalloc(sizeof(*_tmpDE8)),((_tmpDE8[0]=((_tmpDE9.tag=15,((_tmpDE9.f1=(
void*)_tmp683,((_tmpDE9.f2=_tmp682,((_tmpDE9.f3=_tmp65C,((_tmpDE9.f4=(void*)
_tmp65D,_tmpDE9)))))))))),_tmpDE8)))));}}_LL459: if(*((int*)_tmp64E)!= 18)goto
_LL45B;_tmp65E=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp64E)->f1;_LL45A: {
void*_tmp686=Cyc_Tcutil_rsubstitute(r,inst,_tmp65E);if(_tmp686 == _tmp65E)return e;{
struct Cyc_Absyn_Sizeoftyp_e_struct _tmpDEC;struct Cyc_Absyn_Sizeoftyp_e_struct*
_tmpDEB;return Cyc_Tcutil_copye(e,(void*)((_tmpDEB=_cycalloc(sizeof(*_tmpDEB)),((
_tmpDEB[0]=((_tmpDEC.tag=18,((_tmpDEC.f1=(void*)_tmp686,_tmpDEC)))),_tmpDEB)))));}}
_LL45B: if(*((int*)_tmp64E)!= 19)goto _LL45D;_tmp65F=((struct Cyc_Absyn_Sizeofexp_e_struct*)
_tmp64E)->f1;_LL45C: {struct Cyc_Absyn_Exp*_tmp689=Cyc_Tcutil_rsubsexp(r,inst,
_tmp65F);if(_tmp689 == _tmp65F)return e;{struct Cyc_Absyn_Sizeofexp_e_struct _tmpDEF;
struct Cyc_Absyn_Sizeofexp_e_struct*_tmpDEE;return Cyc_Tcutil_copye(e,(void*)((
_tmpDEE=_cycalloc(sizeof(*_tmpDEE)),((_tmpDEE[0]=((_tmpDEF.tag=19,((_tmpDEF.f1=
_tmp689,_tmpDEF)))),_tmpDEE)))));}}_LL45D: if(*((int*)_tmp64E)!= 20)goto _LL45F;
_tmp660=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp64E)->f1;_tmp661=(void*)((
struct Cyc_Absyn_Offsetof_e_struct*)_tmp64E)->f2;_LL45E: {void*_tmp68C=Cyc_Tcutil_rsubstitute(
r,inst,_tmp660);if(_tmp68C == _tmp660)return e;{struct Cyc_Absyn_Offsetof_e_struct
_tmpDF2;struct Cyc_Absyn_Offsetof_e_struct*_tmpDF1;return Cyc_Tcutil_copye(e,(void*)((
_tmpDF1=_cycalloc(sizeof(*_tmpDF1)),((_tmpDF1[0]=((_tmpDF2.tag=20,((_tmpDF2.f1=(
void*)_tmp68C,((_tmpDF2.f2=(void*)_tmp661,_tmpDF2)))))),_tmpDF1)))));}}_LL45F:
if(*((int*)_tmp64E)!= 40)goto _LL461;_tmp662=(void*)((struct Cyc_Absyn_Valueof_e_struct*)
_tmp64E)->f1;_LL460: {void*_tmp68F=Cyc_Tcutil_rsubstitute(r,inst,_tmp662);if(
_tmp68F == _tmp662)return e;{struct Cyc_Absyn_Valueof_e_struct _tmpDF5;struct Cyc_Absyn_Valueof_e_struct*
_tmpDF4;return Cyc_Tcutil_copye(e,(void*)((_tmpDF4=_cycalloc(sizeof(*_tmpDF4)),((
_tmpDF4[0]=((_tmpDF5.tag=40,((_tmpDF5.f1=(void*)_tmp68F,_tmpDF5)))),_tmpDF4)))));}}
_LL461:;_LL462: {const char*_tmpDF8;void*_tmpDF7;return(_tmpDF7=0,((struct Cyc_Absyn_Exp*(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpDF8="non-type-level-expression in Tcutil::rsubsexp",
_tag_dyneither(_tmpDF8,sizeof(char),46))),_tag_dyneither(_tmpDF7,sizeof(void*),0)));}
_LL444:;}void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*rgn,struct Cyc_List_List*
inst,void*t);void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*rgn,struct Cyc_List_List*
inst,void*t){void*_tmp694=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*_tmp695;
struct Cyc_Absyn_AggrInfo _tmp696;union Cyc_Absyn_AggrInfoU _tmp697;struct Cyc_List_List*
_tmp698;struct Cyc_Absyn_DatatypeInfo _tmp699;union Cyc_Absyn_DatatypeInfoU _tmp69A;
struct Cyc_List_List*_tmp69B;struct Cyc_Core_Opt*_tmp69C;struct Cyc_Absyn_DatatypeFieldInfo
_tmp69D;union Cyc_Absyn_DatatypeFieldInfoU _tmp69E;struct Cyc_List_List*_tmp69F;
struct _tuple2*_tmp6A0;struct Cyc_List_List*_tmp6A1;struct Cyc_Absyn_Typedefdecl*
_tmp6A2;void**_tmp6A3;struct Cyc_Absyn_ArrayInfo _tmp6A4;void*_tmp6A5;struct Cyc_Absyn_Tqual
_tmp6A6;struct Cyc_Absyn_Exp*_tmp6A7;union Cyc_Absyn_Constraint*_tmp6A8;struct Cyc_Position_Segment*
_tmp6A9;struct Cyc_Absyn_PtrInfo _tmp6AA;void*_tmp6AB;struct Cyc_Absyn_Tqual _tmp6AC;
struct Cyc_Absyn_PtrAtts _tmp6AD;void*_tmp6AE;union Cyc_Absyn_Constraint*_tmp6AF;
union Cyc_Absyn_Constraint*_tmp6B0;union Cyc_Absyn_Constraint*_tmp6B1;struct Cyc_Absyn_FnInfo
_tmp6B2;struct Cyc_List_List*_tmp6B3;struct Cyc_Core_Opt*_tmp6B4;void*_tmp6B5;
struct Cyc_List_List*_tmp6B6;int _tmp6B7;struct Cyc_Absyn_VarargInfo*_tmp6B8;struct
Cyc_List_List*_tmp6B9;struct Cyc_List_List*_tmp6BA;struct Cyc_List_List*_tmp6BB;
void*_tmp6BC;struct Cyc_List_List*_tmp6BD;struct Cyc_Core_Opt*_tmp6BE;void*_tmp6BF;
void*_tmp6C0;void*_tmp6C1;void*_tmp6C2;struct Cyc_Absyn_Exp*_tmp6C3;void*_tmp6C4;
void*_tmp6C5;struct Cyc_List_List*_tmp6C6;_LL464: if(_tmp694 <= (void*)4)goto _LL486;
if(*((int*)_tmp694)!= 1)goto _LL466;_tmp695=((struct Cyc_Absyn_VarType_struct*)
_tmp694)->f1;_LL465: {struct _handler_cons _tmp6C7;_push_handler(& _tmp6C7);{int
_tmp6C9=0;if(setjmp(_tmp6C7.handler))_tmp6C9=1;if(!_tmp6C9){{void*_tmp6CA=((void*(*)(
int(*cmp)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,
struct Cyc_Absyn_Tvar*x))Cyc_List_assoc_cmp)(Cyc_Absyn_tvar_cmp,inst,_tmp695);
_npop_handler(0);return _tmp6CA;};_pop_handler();}else{void*_tmp6C8=(void*)
_exn_thrown;void*_tmp6CC=_tmp6C8;_LL499: if(_tmp6CC != Cyc_Core_Not_found)goto
_LL49B;_LL49A: return t;_LL49B:;_LL49C:(void)_throw(_tmp6CC);_LL498:;}}}_LL466: if(*((
int*)_tmp694)!= 10)goto _LL468;_tmp696=((struct Cyc_Absyn_AggrType_struct*)_tmp694)->f1;
_tmp697=_tmp696.aggr_info;_tmp698=_tmp696.targs;_LL467: {struct Cyc_List_List*
_tmp6CD=Cyc_Tcutil_substs(rgn,inst,_tmp698);struct Cyc_Absyn_AggrType_struct
_tmpDFE;struct Cyc_Absyn_AggrInfo _tmpDFD;struct Cyc_Absyn_AggrType_struct*_tmpDFC;
return _tmp6CD == _tmp698?t:(void*)((_tmpDFC=_cycalloc(sizeof(*_tmpDFC)),((_tmpDFC[
0]=((_tmpDFE.tag=10,((_tmpDFE.f1=((_tmpDFD.aggr_info=_tmp697,((_tmpDFD.targs=
_tmp6CD,_tmpDFD)))),_tmpDFE)))),_tmpDFC))));}_LL468: if(*((int*)_tmp694)!= 2)goto
_LL46A;_tmp699=((struct Cyc_Absyn_DatatypeType_struct*)_tmp694)->f1;_tmp69A=
_tmp699.datatype_info;_tmp69B=_tmp699.targs;_tmp69C=_tmp699.rgn;_LL469: {struct
Cyc_List_List*_tmp6D1=Cyc_Tcutil_substs(rgn,inst,_tmp69B);struct Cyc_Core_Opt*
new_r;if((unsigned int)_tmp69C){void*_tmp6D2=Cyc_Tcutil_rsubstitute(rgn,inst,(
void*)_tmp69C->v);if(_tmp6D2 == (void*)_tmp69C->v)new_r=_tmp69C;else{struct Cyc_Core_Opt*
_tmpDFF;new_r=((_tmpDFF=_cycalloc(sizeof(*_tmpDFF)),((_tmpDFF->v=(void*)_tmp6D2,
_tmpDFF))));}}else{new_r=_tmp69C;}{struct Cyc_Absyn_DatatypeType_struct _tmpE05;
struct Cyc_Absyn_DatatypeInfo _tmpE04;struct Cyc_Absyn_DatatypeType_struct*_tmpE03;
return _tmp6D1 == _tmp69B  && new_r == _tmp69C?t:(void*)((_tmpE03=_cycalloc(sizeof(*
_tmpE03)),((_tmpE03[0]=((_tmpE05.tag=2,((_tmpE05.f1=((_tmpE04.datatype_info=
_tmp69A,((_tmpE04.targs=_tmp6D1,((_tmpE04.rgn=new_r,_tmpE04)))))),_tmpE05)))),
_tmpE03))));}}_LL46A: if(*((int*)_tmp694)!= 3)goto _LL46C;_tmp69D=((struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp694)->f1;_tmp69E=_tmp69D.field_info;_tmp69F=_tmp69D.targs;_LL46B: {struct Cyc_List_List*
_tmp6D7=Cyc_Tcutil_substs(rgn,inst,_tmp69F);struct Cyc_Absyn_DatatypeFieldType_struct
_tmpE0B;struct Cyc_Absyn_DatatypeFieldInfo _tmpE0A;struct Cyc_Absyn_DatatypeFieldType_struct*
_tmpE09;return _tmp6D7 == _tmp69F?t:(void*)((_tmpE09=_cycalloc(sizeof(*_tmpE09)),((
_tmpE09[0]=((_tmpE0B.tag=3,((_tmpE0B.f1=((_tmpE0A.field_info=_tmp69E,((_tmpE0A.targs=
_tmp6D7,_tmpE0A)))),_tmpE0B)))),_tmpE09))));}_LL46C: if(*((int*)_tmp694)!= 16)
goto _LL46E;_tmp6A0=((struct Cyc_Absyn_TypedefType_struct*)_tmp694)->f1;_tmp6A1=((
struct Cyc_Absyn_TypedefType_struct*)_tmp694)->f2;_tmp6A2=((struct Cyc_Absyn_TypedefType_struct*)
_tmp694)->f3;_tmp6A3=((struct Cyc_Absyn_TypedefType_struct*)_tmp694)->f4;_LL46D: {
struct Cyc_List_List*_tmp6DB=Cyc_Tcutil_substs(rgn,inst,_tmp6A1);struct Cyc_Absyn_TypedefType_struct
_tmpE0E;struct Cyc_Absyn_TypedefType_struct*_tmpE0D;return _tmp6DB == _tmp6A1?t:(
void*)((_tmpE0D=_cycalloc(sizeof(*_tmpE0D)),((_tmpE0D[0]=((_tmpE0E.tag=16,((
_tmpE0E.f1=_tmp6A0,((_tmpE0E.f2=_tmp6DB,((_tmpE0E.f3=_tmp6A2,((_tmpE0E.f4=
_tmp6A3,_tmpE0E)))))))))),_tmpE0D))));}_LL46E: if(*((int*)_tmp694)!= 7)goto _LL470;
_tmp6A4=((struct Cyc_Absyn_ArrayType_struct*)_tmp694)->f1;_tmp6A5=_tmp6A4.elt_type;
_tmp6A6=_tmp6A4.tq;_tmp6A7=_tmp6A4.num_elts;_tmp6A8=_tmp6A4.zero_term;_tmp6A9=
_tmp6A4.zt_loc;_LL46F: {void*_tmp6DE=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6A5);
struct Cyc_Absyn_ArrayType_struct _tmpE14;struct Cyc_Absyn_ArrayInfo _tmpE13;struct
Cyc_Absyn_ArrayType_struct*_tmpE12;return _tmp6DE == _tmp6A5?t:(void*)((_tmpE12=
_cycalloc(sizeof(*_tmpE12)),((_tmpE12[0]=((_tmpE14.tag=7,((_tmpE14.f1=((_tmpE13.elt_type=
_tmp6DE,((_tmpE13.tq=_tmp6A6,((_tmpE13.num_elts=_tmp6A7,((_tmpE13.zero_term=
_tmp6A8,((_tmpE13.zt_loc=_tmp6A9,_tmpE13)))))))))),_tmpE14)))),_tmpE12))));}
_LL470: if(*((int*)_tmp694)!= 4)goto _LL472;_tmp6AA=((struct Cyc_Absyn_PointerType_struct*)
_tmp694)->f1;_tmp6AB=_tmp6AA.elt_typ;_tmp6AC=_tmp6AA.elt_tq;_tmp6AD=_tmp6AA.ptr_atts;
_tmp6AE=_tmp6AD.rgn;_tmp6AF=_tmp6AD.nullable;_tmp6B0=_tmp6AD.bounds;_tmp6B1=
_tmp6AD.zero_term;_LL471: {void*_tmp6E2=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6AB);
void*_tmp6E3=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6AE);union Cyc_Absyn_Constraint*
_tmp6E4=_tmp6B0;{void*_tmp6E5=Cyc_Absyn_conref_def((void*)((void*)0),_tmp6B0);
struct Cyc_Absyn_Exp*_tmp6E6;_LL49E: if(_tmp6E5 <= (void*)1)goto _LL4A0;if(*((int*)
_tmp6E5)!= 0)goto _LL4A0;_tmp6E6=((struct Cyc_Absyn_Upper_b_struct*)_tmp6E5)->f1;
_LL49F: {struct Cyc_Absyn_Exp*_tmp6E7=Cyc_Tcutil_rsubsexp(rgn,inst,_tmp6E6);if(
_tmp6E7 != _tmp6E6){struct Cyc_Absyn_Upper_b_struct _tmpE17;struct Cyc_Absyn_Upper_b_struct*
_tmpE16;_tmp6E4=Cyc_Absyn_new_conref((void*)((_tmpE16=_cycalloc(sizeof(*_tmpE16)),((
_tmpE16[0]=((_tmpE17.tag=0,((_tmpE17.f1=_tmp6E7,_tmpE17)))),_tmpE16)))));}goto
_LL49D;}_LL4A0:;_LL4A1: goto _LL49D;_LL49D:;}if((_tmp6E2 == _tmp6AB  && _tmp6E3 == 
_tmp6AE) && _tmp6E4 == _tmp6B0)return t;{struct Cyc_Absyn_PointerType_struct _tmpE21;
struct Cyc_Absyn_PtrAtts _tmpE20;struct Cyc_Absyn_PtrInfo _tmpE1F;struct Cyc_Absyn_PointerType_struct*
_tmpE1E;return(void*)((_tmpE1E=_cycalloc(sizeof(*_tmpE1E)),((_tmpE1E[0]=((
_tmpE21.tag=4,((_tmpE21.f1=((_tmpE1F.elt_typ=_tmp6E2,((_tmpE1F.elt_tq=_tmp6AC,((
_tmpE1F.ptr_atts=((_tmpE20.rgn=_tmp6E3,((_tmpE20.nullable=_tmp6AF,((_tmpE20.bounds=
_tmp6E4,((_tmpE20.zero_term=_tmp6B1,((_tmpE20.ptrloc=0,_tmpE20)))))))))),_tmpE1F)))))),
_tmpE21)))),_tmpE1E))));}}_LL472: if(*((int*)_tmp694)!= 8)goto _LL474;_tmp6B2=((
struct Cyc_Absyn_FnType_struct*)_tmp694)->f1;_tmp6B3=_tmp6B2.tvars;_tmp6B4=
_tmp6B2.effect;_tmp6B5=_tmp6B2.ret_typ;_tmp6B6=_tmp6B2.args;_tmp6B7=_tmp6B2.c_varargs;
_tmp6B8=_tmp6B2.cyc_varargs;_tmp6B9=_tmp6B2.rgn_po;_tmp6BA=_tmp6B2.attributes;
_LL473:{struct Cyc_List_List*_tmp6EE=_tmp6B3;for(0;_tmp6EE != 0;_tmp6EE=_tmp6EE->tl){
struct _tuple14*_tmpE2B;struct Cyc_Absyn_VarType_struct _tmpE2A;struct Cyc_Absyn_VarType_struct*
_tmpE29;struct Cyc_List_List*_tmpE28;inst=((_tmpE28=_region_malloc(rgn,sizeof(*
_tmpE28)),((_tmpE28->hd=((_tmpE2B=_region_malloc(rgn,sizeof(*_tmpE2B)),((_tmpE2B->f1=(
struct Cyc_Absyn_Tvar*)_tmp6EE->hd,((_tmpE2B->f2=(void*)((_tmpE29=_cycalloc(
sizeof(*_tmpE29)),((_tmpE29[0]=((_tmpE2A.tag=1,((_tmpE2A.f1=(struct Cyc_Absyn_Tvar*)
_tmp6EE->hd,_tmpE2A)))),_tmpE29)))),_tmpE2B)))))),((_tmpE28->tl=inst,_tmpE28))))));}}{
struct Cyc_List_List*_tmp6F4;struct Cyc_List_List*_tmp6F5;struct _tuple1 _tmp6F3=((
struct _tuple1(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x))Cyc_List_rsplit)(rgn,rgn,((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct _tuple22*(*f)(struct _RegionHandle*,struct _tuple9*),struct _RegionHandle*env,
struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_substitute_f1,rgn,_tmp6B6));
_tmp6F4=_tmp6F3.f1;_tmp6F5=_tmp6F3.f2;{struct Cyc_List_List*_tmp6F6=Cyc_Tcutil_substs(
rgn,inst,_tmp6F5);struct Cyc_List_List*_tmp6F7=((struct Cyc_List_List*(*)(struct
_tuple9*(*f)(struct _tuple22*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_substitute_f2,((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmp6F4,_tmp6F6));struct Cyc_Core_Opt*
eff2;if(_tmp6B4 == 0)eff2=0;else{void*_tmp6F8=Cyc_Tcutil_rsubstitute(rgn,inst,(
void*)_tmp6B4->v);if(_tmp6F8 == (void*)_tmp6B4->v)eff2=_tmp6B4;else{struct Cyc_Core_Opt*
_tmpE2C;eff2=((_tmpE2C=_cycalloc(sizeof(*_tmpE2C)),((_tmpE2C->v=(void*)_tmp6F8,
_tmpE2C))));}}{struct Cyc_Absyn_VarargInfo*cyc_varargs2;if(_tmp6B8 == 0)
cyc_varargs2=0;else{struct Cyc_Core_Opt*_tmp6FB;struct Cyc_Absyn_Tqual _tmp6FC;void*
_tmp6FD;int _tmp6FE;struct Cyc_Absyn_VarargInfo _tmp6FA=*_tmp6B8;_tmp6FB=_tmp6FA.name;
_tmp6FC=_tmp6FA.tq;_tmp6FD=_tmp6FA.type;_tmp6FE=_tmp6FA.inject;{void*_tmp6FF=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmp6FD);if(_tmp6FF == _tmp6FD)cyc_varargs2=_tmp6B8;else{struct Cyc_Absyn_VarargInfo*
_tmpE2D;cyc_varargs2=((_tmpE2D=_cycalloc(sizeof(*_tmpE2D)),((_tmpE2D->name=
_tmp6FB,((_tmpE2D->tq=_tmp6FC,((_tmpE2D->type=_tmp6FF,((_tmpE2D->inject=_tmp6FE,
_tmpE2D))))))))));}}}{struct Cyc_List_List*rgn_po2;struct Cyc_List_List*_tmp702;
struct Cyc_List_List*_tmp703;struct _tuple1 _tmp701=Cyc_List_rsplit(rgn,rgn,_tmp6B9);
_tmp702=_tmp701.f1;_tmp703=_tmp701.f2;{struct Cyc_List_List*_tmp704=Cyc_Tcutil_substs(
rgn,inst,_tmp702);struct Cyc_List_List*_tmp705=Cyc_Tcutil_substs(rgn,inst,_tmp703);
if(_tmp704 == _tmp702  && _tmp705 == _tmp703)rgn_po2=_tmp6B9;else{rgn_po2=Cyc_List_zip(
_tmp704,_tmp705);}{struct Cyc_Absyn_FnType_struct _tmpE33;struct Cyc_Absyn_FnInfo
_tmpE32;struct Cyc_Absyn_FnType_struct*_tmpE31;return(void*)((_tmpE31=_cycalloc(
sizeof(*_tmpE31)),((_tmpE31[0]=((_tmpE33.tag=8,((_tmpE33.f1=((_tmpE32.tvars=
_tmp6B3,((_tmpE32.effect=eff2,((_tmpE32.ret_typ=Cyc_Tcutil_rsubstitute(rgn,inst,
_tmp6B5),((_tmpE32.args=_tmp6F7,((_tmpE32.c_varargs=_tmp6B7,((_tmpE32.cyc_varargs=
cyc_varargs2,((_tmpE32.rgn_po=rgn_po2,((_tmpE32.attributes=_tmp6BA,_tmpE32)))))))))))))))),
_tmpE33)))),_tmpE31))));}}}}}}_LL474: if(*((int*)_tmp694)!= 9)goto _LL476;_tmp6BB=((
struct Cyc_Absyn_TupleType_struct*)_tmp694)->f1;_LL475: {struct Cyc_List_List*
_tmp709=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _tuple11*),
struct Cyc_List_List*x))Cyc_List_rmap)(rgn,Cyc_Tcutil_snd_tqt,_tmp6BB);struct Cyc_List_List*
_tmp70A=Cyc_Tcutil_substs(rgn,inst,_tmp709);if(_tmp70A == _tmp709)return t;{struct
Cyc_List_List*_tmp70B=((struct Cyc_List_List*(*)(struct _tuple11*(*f)(struct
_tuple11*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_map2_tq,
_tmp6BB,_tmp70A);struct Cyc_Absyn_TupleType_struct _tmpE36;struct Cyc_Absyn_TupleType_struct*
_tmpE35;return(void*)((_tmpE35=_cycalloc(sizeof(*_tmpE35)),((_tmpE35[0]=((
_tmpE36.tag=9,((_tmpE36.f1=_tmp70B,_tmpE36)))),_tmpE35))));}}_LL476: if(*((int*)
_tmp694)!= 11)goto _LL478;_tmp6BC=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp694)->f1;_tmp6BD=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp694)->f2;_LL477: {
struct Cyc_List_List*_tmp70E=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*
f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_rmap)(rgn,Cyc_Tcutil_field_type,
_tmp6BD);struct Cyc_List_List*_tmp70F=Cyc_Tcutil_substs(rgn,inst,_tmp70E);if(
_tmp70F == _tmp70E)return t;{struct Cyc_List_List*_tmp710=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*,void*),struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_zip_field_type,_tmp6BD,
_tmp70F);struct Cyc_Absyn_AnonAggrType_struct _tmpE39;struct Cyc_Absyn_AnonAggrType_struct*
_tmpE38;return(void*)((_tmpE38=_cycalloc(sizeof(*_tmpE38)),((_tmpE38[0]=((
_tmpE39.tag=11,((_tmpE39.f1=(void*)_tmp6BC,((_tmpE39.f2=_tmp710,_tmpE39)))))),
_tmpE38))));}}_LL478: if(*((int*)_tmp694)!= 0)goto _LL47A;_tmp6BE=((struct Cyc_Absyn_Evar_struct*)
_tmp694)->f2;_LL479: if(_tmp6BE != 0)return Cyc_Tcutil_rsubstitute(rgn,inst,(void*)
_tmp6BE->v);else{return t;}_LL47A: if(*((int*)_tmp694)!= 14)goto _LL47C;_tmp6BF=(
void*)((struct Cyc_Absyn_RgnHandleType_struct*)_tmp694)->f1;_LL47B: {void*_tmp713=
Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6BF);struct Cyc_Absyn_RgnHandleType_struct
_tmpE3C;struct Cyc_Absyn_RgnHandleType_struct*_tmpE3B;return _tmp713 == _tmp6BF?t:(
void*)((_tmpE3B=_cycalloc(sizeof(*_tmpE3B)),((_tmpE3B[0]=((_tmpE3C.tag=14,((
_tmpE3C.f1=(void*)_tmp713,_tmpE3C)))),_tmpE3B))));}_LL47C: if(*((int*)_tmp694)!= 
15)goto _LL47E;_tmp6C0=(void*)((struct Cyc_Absyn_DynRgnType_struct*)_tmp694)->f1;
_tmp6C1=(void*)((struct Cyc_Absyn_DynRgnType_struct*)_tmp694)->f2;_LL47D: {void*
_tmp716=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6C0);void*_tmp717=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmp6C1);struct Cyc_Absyn_DynRgnType_struct _tmpE3F;struct Cyc_Absyn_DynRgnType_struct*
_tmpE3E;return _tmp716 == _tmp6C0  && _tmp717 == _tmp6C1?t:(void*)((_tmpE3E=
_cycalloc(sizeof(*_tmpE3E)),((_tmpE3E[0]=((_tmpE3F.tag=15,((_tmpE3F.f1=(void*)
_tmp716,((_tmpE3F.f2=(void*)_tmp717,_tmpE3F)))))),_tmpE3E))));}_LL47E: if(*((int*)
_tmp694)!= 18)goto _LL480;_tmp6C2=(void*)((struct Cyc_Absyn_TagType_struct*)
_tmp694)->f1;_LL47F: {void*_tmp71A=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6C2);
struct Cyc_Absyn_TagType_struct _tmpE42;struct Cyc_Absyn_TagType_struct*_tmpE41;
return _tmp71A == _tmp6C2?t:(void*)((_tmpE41=_cycalloc(sizeof(*_tmpE41)),((_tmpE41[
0]=((_tmpE42.tag=18,((_tmpE42.f1=(void*)_tmp71A,_tmpE42)))),_tmpE41))));}_LL480:
if(*((int*)_tmp694)!= 17)goto _LL482;_tmp6C3=((struct Cyc_Absyn_ValueofType_struct*)
_tmp694)->f1;_LL481: {struct Cyc_Absyn_Exp*_tmp71D=Cyc_Tcutil_rsubsexp(rgn,inst,
_tmp6C3);struct Cyc_Absyn_ValueofType_struct _tmpE45;struct Cyc_Absyn_ValueofType_struct*
_tmpE44;return _tmp71D == _tmp6C3?t:(void*)((_tmpE44=_cycalloc(sizeof(*_tmpE44)),((
_tmpE44[0]=((_tmpE45.tag=17,((_tmpE45.f1=_tmp71D,_tmpE45)))),_tmpE44))));}_LL482:
if(*((int*)_tmp694)!= 12)goto _LL484;_LL483: goto _LL485;_LL484: if(*((int*)_tmp694)
!= 13)goto _LL486;_LL485: goto _LL487;_LL486: if((int)_tmp694 != 0)goto _LL488;_LL487:
goto _LL489;_LL488: if(_tmp694 <= (void*)4)goto _LL48A;if(*((int*)_tmp694)!= 5)goto
_LL48A;_LL489: goto _LL48B;_LL48A: if((int)_tmp694 != 1)goto _LL48C;_LL48B: goto _LL48D;
_LL48C: if(_tmp694 <= (void*)4)goto _LL48E;if(*((int*)_tmp694)!= 6)goto _LL48E;
_LL48D: goto _LL48F;_LL48E: if((int)_tmp694 != 3)goto _LL490;_LL48F: goto _LL491;_LL490:
if((int)_tmp694 != 2)goto _LL492;_LL491: return t;_LL492: if(_tmp694 <= (void*)4)goto
_LL494;if(*((int*)_tmp694)!= 21)goto _LL494;_tmp6C4=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp694)->f1;_LL493: {void*_tmp720=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6C4);
struct Cyc_Absyn_RgnsEff_struct _tmpE48;struct Cyc_Absyn_RgnsEff_struct*_tmpE47;
return _tmp720 == _tmp6C4?t:(void*)((_tmpE47=_cycalloc(sizeof(*_tmpE47)),((_tmpE47[
0]=((_tmpE48.tag=21,((_tmpE48.f1=(void*)_tmp720,_tmpE48)))),_tmpE47))));}_LL494:
if(_tmp694 <= (void*)4)goto _LL496;if(*((int*)_tmp694)!= 19)goto _LL496;_tmp6C5=(
void*)((struct Cyc_Absyn_AccessEff_struct*)_tmp694)->f1;_LL495: {void*_tmp723=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmp6C5);struct Cyc_Absyn_AccessEff_struct _tmpE4B;struct Cyc_Absyn_AccessEff_struct*
_tmpE4A;return _tmp723 == _tmp6C5?t:(void*)((_tmpE4A=_cycalloc(sizeof(*_tmpE4A)),((
_tmpE4A[0]=((_tmpE4B.tag=19,((_tmpE4B.f1=(void*)_tmp723,_tmpE4B)))),_tmpE4A))));}
_LL496: if(_tmp694 <= (void*)4)goto _LL463;if(*((int*)_tmp694)!= 20)goto _LL463;
_tmp6C6=((struct Cyc_Absyn_JoinEff_struct*)_tmp694)->f1;_LL497: {struct Cyc_List_List*
_tmp726=Cyc_Tcutil_substs(rgn,inst,_tmp6C6);struct Cyc_Absyn_JoinEff_struct
_tmpE4E;struct Cyc_Absyn_JoinEff_struct*_tmpE4D;return _tmp726 == _tmp6C6?t:(void*)((
_tmpE4D=_cycalloc(sizeof(*_tmpE4D)),((_tmpE4D[0]=((_tmpE4E.tag=20,((_tmpE4E.f1=
_tmp726,_tmpE4E)))),_tmpE4D))));}_LL463:;}static struct Cyc_List_List*Cyc_Tcutil_substs(
struct _RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*ts);static
struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*rgn,struct Cyc_List_List*
inst,struct Cyc_List_List*ts){if(ts == 0)return 0;{void*_tmp729=(void*)ts->hd;
struct Cyc_List_List*_tmp72A=ts->tl;void*_tmp72B=Cyc_Tcutil_rsubstitute(rgn,inst,
_tmp729);struct Cyc_List_List*_tmp72C=Cyc_Tcutil_substs(rgn,inst,_tmp72A);if(
_tmp729 == _tmp72B  && _tmp72A == _tmp72C)return ts;{struct Cyc_List_List*_tmpE4F;
return(struct Cyc_List_List*)((struct Cyc_List_List*)((_tmpE4F=_cycalloc(sizeof(*
_tmpE4F)),((_tmpE4F->hd=(void*)_tmp72B,((_tmpE4F->tl=_tmp72C,_tmpE4F)))))));}}}
void*Cyc_Tcutil_substitute(struct Cyc_List_List*inst,void*t);extern void*Cyc_Tcutil_substitute(
struct Cyc_List_List*inst,void*t){return Cyc_Tcutil_rsubstitute(Cyc_Core_heap_region,
inst,t);}struct _tuple14*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*s,struct Cyc_Absyn_Tvar*
tv);struct _tuple14*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*s,struct Cyc_Absyn_Tvar*
tv){struct Cyc_Core_Opt*_tmp72E=Cyc_Tcutil_kind_to_opt(Cyc_Tcutil_tvar_kind(tv));
struct Cyc_Core_Opt*_tmpE52;struct _tuple14*_tmpE51;return(_tmpE51=_cycalloc(
sizeof(*_tmpE51)),((_tmpE51->f1=tv,((_tmpE51->f2=Cyc_Absyn_new_evar(_tmp72E,((
_tmpE52=_cycalloc(sizeof(*_tmpE52)),((_tmpE52->v=s,_tmpE52))))),_tmpE51)))));}
struct _tuple14*Cyc_Tcutil_r_make_inst_var(struct _tuple15*env,struct Cyc_Absyn_Tvar*
tv);struct _tuple14*Cyc_Tcutil_r_make_inst_var(struct _tuple15*env,struct Cyc_Absyn_Tvar*
tv){struct _tuple15 _tmp732;struct Cyc_List_List*_tmp733;struct _RegionHandle*
_tmp734;struct _tuple15*_tmp731=env;_tmp732=*_tmp731;_tmp733=_tmp732.f1;_tmp734=
_tmp732.f2;{struct Cyc_Core_Opt*_tmp735=Cyc_Tcutil_kind_to_opt(Cyc_Tcutil_tvar_kind(
tv));struct Cyc_Core_Opt*_tmpE55;struct _tuple14*_tmpE54;return(_tmpE54=
_region_malloc(_tmp734,sizeof(*_tmpE54)),((_tmpE54->f1=tv,((_tmpE54->f2=Cyc_Absyn_new_evar(
_tmp735,((_tmpE55=_cycalloc(sizeof(*_tmpE55)),((_tmpE55->v=_tmp733,_tmpE55))))),
_tmpE54)))));}}static struct Cyc_List_List*Cyc_Tcutil_add_free_tvar(struct Cyc_Position_Segment*
loc,struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv);static struct Cyc_List_List*
Cyc_Tcutil_add_free_tvar(struct Cyc_Position_Segment*loc,struct Cyc_List_List*tvs,
struct Cyc_Absyn_Tvar*tv){{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
if(Cyc_strptrcmp(((struct Cyc_Absyn_Tvar*)tvs2->hd)->name,tv->name)== 0){void*k1=((
struct Cyc_Absyn_Tvar*)tvs2->hd)->kind;void*k2=tv->kind;if(!Cyc_Tcutil_constrain_kinds(
k1,k2)){const char*_tmpE5B;void*_tmpE5A[3];struct Cyc_String_pa_struct _tmpE59;
struct Cyc_String_pa_struct _tmpE58;struct Cyc_String_pa_struct _tmpE57;(_tmpE57.tag=
0,((_tmpE57.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kindbound2string(
k2)),((_tmpE58.tag=0,((_tmpE58.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kindbound2string(k1)),((_tmpE59.tag=0,((_tmpE59.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*tv->name),((_tmpE5A[0]=& _tmpE59,((_tmpE5A[
1]=& _tmpE58,((_tmpE5A[2]=& _tmpE57,Cyc_Tcutil_terr(loc,((_tmpE5B="type variable %s is used with inconsistent kinds %s and %s",
_tag_dyneither(_tmpE5B,sizeof(char),59))),_tag_dyneither(_tmpE5A,sizeof(void*),3)))))))))))))))))));}
if(tv->identity == - 1)tv->identity=((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity;
else{if(tv->identity != ((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity){const char*
_tmpE5E;void*_tmpE5D;(_tmpE5D=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpE5E="same type variable has different identity!",
_tag_dyneither(_tmpE5E,sizeof(char),43))),_tag_dyneither(_tmpE5D,sizeof(void*),0)));}}
return tvs;}}}tv->identity=Cyc_Tcutil_new_tvar_id();{struct Cyc_List_List*_tmpE5F;
return(_tmpE5F=_cycalloc(sizeof(*_tmpE5F)),((_tmpE5F->hd=tv,((_tmpE5F->tl=tvs,
_tmpE5F)))));}}static struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar(struct Cyc_List_List*
tvs,struct Cyc_Absyn_Tvar*tv);static struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar(
struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){if(tv->identity == - 1){const char*
_tmpE62;void*_tmpE61;(_tmpE61=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpE62="fast_add_free_tvar: bad identity in tv",
_tag_dyneither(_tmpE62,sizeof(char),39))),_tag_dyneither(_tmpE61,sizeof(void*),0)));}{
struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){struct Cyc_Absyn_Tvar*
_tmp742=(struct Cyc_Absyn_Tvar*)tvs2->hd;if(_tmp742->identity == - 1){const char*
_tmpE65;void*_tmpE64;(_tmpE64=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpE65="fast_add_free_tvar: bad identity in tvs2",
_tag_dyneither(_tmpE65,sizeof(char),41))),_tag_dyneither(_tmpE64,sizeof(void*),0)));}
if(_tmp742->identity == tv->identity)return tvs;}}{struct Cyc_List_List*_tmpE66;
return(_tmpE66=_cycalloc(sizeof(*_tmpE66)),((_tmpE66->hd=tv,((_tmpE66->tl=tvs,
_tmpE66)))));}}struct _tuple23{struct Cyc_Absyn_Tvar*f1;int f2;};static struct Cyc_List_List*
Cyc_Tcutil_fast_add_free_tvar_bool(struct _RegionHandle*r,struct Cyc_List_List*tvs,
struct Cyc_Absyn_Tvar*tv,int b);static struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar_bool(
struct _RegionHandle*r,struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv,int b){if(
tv->identity == - 1){const char*_tmpE69;void*_tmpE68;(_tmpE68=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpE69="fast_add_free_tvar_bool: bad identity in tv",
_tag_dyneither(_tmpE69,sizeof(char),44))),_tag_dyneither(_tmpE68,sizeof(void*),0)));}{
struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){struct _tuple23 _tmp749;
struct Cyc_Absyn_Tvar*_tmp74A;int _tmp74B;int*_tmp74C;struct _tuple23*_tmp748=(
struct _tuple23*)tvs2->hd;_tmp749=*_tmp748;_tmp74A=_tmp749.f1;_tmp74B=_tmp749.f2;
_tmp74C=(int*)&(*_tmp748).f2;if(_tmp74A->identity == - 1){const char*_tmpE6C;void*
_tmpE6B;(_tmpE6B=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmpE6C="fast_add_free_tvar_bool: bad identity in tvs2",_tag_dyneither(_tmpE6C,
sizeof(char),46))),_tag_dyneither(_tmpE6B,sizeof(void*),0)));}if(_tmp74A->identity
== tv->identity){*_tmp74C=*_tmp74C  || b;return tvs;}}}{struct _tuple23*_tmpE6F;
struct Cyc_List_List*_tmpE6E;return(_tmpE6E=_region_malloc(r,sizeof(*_tmpE6E)),((
_tmpE6E->hd=((_tmpE6F=_region_malloc(r,sizeof(*_tmpE6F)),((_tmpE6F->f1=tv,((
_tmpE6F->f2=b,_tmpE6F)))))),((_tmpE6E->tl=tvs,_tmpE6E)))));}}static struct Cyc_List_List*
Cyc_Tcutil_add_bound_tvar(struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv);
static struct Cyc_List_List*Cyc_Tcutil_add_bound_tvar(struct Cyc_List_List*tvs,
struct Cyc_Absyn_Tvar*tv){if(tv->identity == - 1){const char*_tmpE73;void*_tmpE72[1];
struct Cyc_String_pa_struct _tmpE71;(_tmpE71.tag=0,((_tmpE71.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_tvar2string(tv)),((_tmpE72[0]=&
_tmpE71,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmpE73="bound tvar id for %s is NULL",_tag_dyneither(_tmpE73,sizeof(char),29))),
_tag_dyneither(_tmpE72,sizeof(void*),1)))))));}{struct Cyc_List_List*_tmpE74;
return(_tmpE74=_cycalloc(sizeof(*_tmpE74)),((_tmpE74->hd=tv,((_tmpE74->tl=tvs,
_tmpE74)))));}}static struct Cyc_List_List*Cyc_Tcutil_add_free_evar(struct
_RegionHandle*r,struct Cyc_List_List*es,void*e,int b);static struct Cyc_List_List*
Cyc_Tcutil_add_free_evar(struct _RegionHandle*r,struct Cyc_List_List*es,void*e,int
b){void*_tmp755=Cyc_Tcutil_compress(e);int _tmp756;_LL4A3: if(_tmp755 <= (void*)4)
goto _LL4A5;if(*((int*)_tmp755)!= 0)goto _LL4A5;_tmp756=((struct Cyc_Absyn_Evar_struct*)
_tmp755)->f3;_LL4A4:{struct Cyc_List_List*es2=es;for(0;es2 != 0;es2=es2->tl){
struct _tuple7 _tmp758;void*_tmp759;int _tmp75A;int*_tmp75B;struct _tuple7*_tmp757=(
struct _tuple7*)es2->hd;_tmp758=*_tmp757;_tmp759=_tmp758.f1;_tmp75A=_tmp758.f2;
_tmp75B=(int*)&(*_tmp757).f2;{void*_tmp75C=Cyc_Tcutil_compress(_tmp759);int
_tmp75D;_LL4A8: if(_tmp75C <= (void*)4)goto _LL4AA;if(*((int*)_tmp75C)!= 0)goto
_LL4AA;_tmp75D=((struct Cyc_Absyn_Evar_struct*)_tmp75C)->f3;_LL4A9: if(_tmp756 == 
_tmp75D){if(b != *_tmp75B)*_tmp75B=1;return es;}goto _LL4A7;_LL4AA:;_LL4AB: goto
_LL4A7;_LL4A7:;}}}{struct _tuple7*_tmpE77;struct Cyc_List_List*_tmpE76;return(
_tmpE76=_region_malloc(r,sizeof(*_tmpE76)),((_tmpE76->hd=((_tmpE77=
_region_malloc(r,sizeof(*_tmpE77)),((_tmpE77->f1=e,((_tmpE77->f2=b,_tmpE77)))))),((
_tmpE76->tl=es,_tmpE76)))));}_LL4A5:;_LL4A6: return es;_LL4A2:;}static struct Cyc_List_List*
Cyc_Tcutil_remove_bound_tvars(struct _RegionHandle*rgn,struct Cyc_List_List*tvs,
struct Cyc_List_List*btvs);static struct Cyc_List_List*Cyc_Tcutil_remove_bound_tvars(
struct _RegionHandle*rgn,struct Cyc_List_List*tvs,struct Cyc_List_List*btvs){struct
Cyc_List_List*r=0;for(0;tvs != 0;tvs=tvs->tl){int present=0;{struct Cyc_List_List*b=
btvs;for(0;b != 0;b=b->tl){if(((struct Cyc_Absyn_Tvar*)tvs->hd)->identity == ((
struct Cyc_Absyn_Tvar*)b->hd)->identity){present=1;break;}}}if(!present){struct
Cyc_List_List*_tmpE78;r=((_tmpE78=_region_malloc(rgn,sizeof(*_tmpE78)),((_tmpE78->hd=(
struct Cyc_Absyn_Tvar*)tvs->hd,((_tmpE78->tl=r,_tmpE78))))));}}r=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(r);return r;}static struct Cyc_List_List*
Cyc_Tcutil_remove_bound_tvars_bool(struct _RegionHandle*r,struct Cyc_List_List*tvs,
struct Cyc_List_List*btvs);static struct Cyc_List_List*Cyc_Tcutil_remove_bound_tvars_bool(
struct _RegionHandle*r,struct Cyc_List_List*tvs,struct Cyc_List_List*btvs){struct
Cyc_List_List*res=0;for(0;tvs != 0;tvs=tvs->tl){struct Cyc_Absyn_Tvar*_tmp762;int
_tmp763;struct _tuple23 _tmp761=*((struct _tuple23*)tvs->hd);_tmp762=_tmp761.f1;
_tmp763=_tmp761.f2;{int present=0;{struct Cyc_List_List*b=btvs;for(0;b != 0;b=b->tl){
if(_tmp762->identity == ((struct Cyc_Absyn_Tvar*)b->hd)->identity){present=1;
break;}}}if(!present){struct Cyc_List_List*_tmpE79;res=((_tmpE79=_region_malloc(r,
sizeof(*_tmpE79)),((_tmpE79->hd=(struct _tuple23*)tvs->hd,((_tmpE79->tl=res,
_tmpE79))))));}}}res=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
res);return res;}void Cyc_Tcutil_check_bitfield(struct Cyc_Position_Segment*loc,
struct Cyc_Tcenv_Tenv*te,void*field_typ,struct Cyc_Absyn_Exp*width,struct
_dyneither_ptr*fn);void Cyc_Tcutil_check_bitfield(struct Cyc_Position_Segment*loc,
struct Cyc_Tcenv_Tenv*te,void*field_typ,struct Cyc_Absyn_Exp*width,struct
_dyneither_ptr*fn){if(width != 0){unsigned int w=0;if(!Cyc_Tcutil_is_const_exp(te,(
struct Cyc_Absyn_Exp*)width)){const char*_tmpE7D;void*_tmpE7C[1];struct Cyc_String_pa_struct
_tmpE7B;(_tmpE7B.tag=0,((_tmpE7B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*
fn),((_tmpE7C[0]=& _tmpE7B,Cyc_Tcutil_terr(loc,((_tmpE7D="bitfield %s does not have constant width",
_tag_dyneither(_tmpE7D,sizeof(char),41))),_tag_dyneither(_tmpE7C,sizeof(void*),1)))))));}
else{unsigned int _tmp769;int _tmp76A;struct _tuple13 _tmp768=Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)width);_tmp769=_tmp768.f1;_tmp76A=_tmp768.f2;if(!_tmp76A){
const char*_tmpE80;void*_tmpE7F;(_tmpE7F=0,Cyc_Tcutil_terr(loc,((_tmpE80="bitfield width cannot use sizeof or offsetof",
_tag_dyneither(_tmpE80,sizeof(char),45))),_tag_dyneither(_tmpE7F,sizeof(void*),0)));}
w=_tmp769;}{void*_tmp76D=Cyc_Tcutil_compress(field_typ);void*_tmp76E;_LL4AD: if(
_tmp76D <= (void*)4)goto _LL4AF;if(*((int*)_tmp76D)!= 5)goto _LL4AF;_tmp76E=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp76D)->f2;_LL4AE:{void*_tmp76F=_tmp76E;_LL4B2:
if((int)_tmp76F != 0)goto _LL4B4;_LL4B3: if(w > 8){const char*_tmpE83;void*_tmpE82;(
_tmpE82=0,Cyc_Tcutil_terr(loc,((_tmpE83="bitfield larger than type",
_tag_dyneither(_tmpE83,sizeof(char),26))),_tag_dyneither(_tmpE82,sizeof(void*),0)));}
goto _LL4B1;_LL4B4: if((int)_tmp76F != 1)goto _LL4B6;_LL4B5: if(w > 16){const char*
_tmpE86;void*_tmpE85;(_tmpE85=0,Cyc_Tcutil_terr(loc,((_tmpE86="bitfield larger than type",
_tag_dyneither(_tmpE86,sizeof(char),26))),_tag_dyneither(_tmpE85,sizeof(void*),0)));}
goto _LL4B1;_LL4B6: if((int)_tmp76F != 3)goto _LL4B8;_LL4B7: goto _LL4B9;_LL4B8: if((
int)_tmp76F != 2)goto _LL4BA;_LL4B9: if(w > 32){const char*_tmpE89;void*_tmpE88;(
_tmpE88=0,Cyc_Tcutil_terr(loc,((_tmpE89="bitfield larger than type",
_tag_dyneither(_tmpE89,sizeof(char),26))),_tag_dyneither(_tmpE88,sizeof(void*),0)));}
goto _LL4B1;_LL4BA: if((int)_tmp76F != 4)goto _LL4B1;_LL4BB: if(w > 64){const char*
_tmpE8C;void*_tmpE8B;(_tmpE8B=0,Cyc_Tcutil_terr(loc,((_tmpE8C="bitfield larger than type",
_tag_dyneither(_tmpE8C,sizeof(char),26))),_tag_dyneither(_tmpE8B,sizeof(void*),0)));}
goto _LL4B1;_LL4B1:;}goto _LL4AC;_LL4AF:;_LL4B0:{const char*_tmpE91;void*_tmpE90[2];
struct Cyc_String_pa_struct _tmpE8F;struct Cyc_String_pa_struct _tmpE8E;(_tmpE8E.tag=
0,((_tmpE8E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
field_typ)),((_tmpE8F.tag=0,((_tmpE8F.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*fn),((_tmpE90[0]=& _tmpE8F,((_tmpE90[1]=& _tmpE8E,Cyc_Tcutil_terr(
loc,((_tmpE91="bitfield %s must have integral type but has type %s",
_tag_dyneither(_tmpE91,sizeof(char),52))),_tag_dyneither(_tmpE90,sizeof(void*),2)))))))))))));}
goto _LL4AC;_LL4AC:;}}}static void Cyc_Tcutil_check_field_atts(struct Cyc_Position_Segment*
loc,struct _dyneither_ptr*fn,struct Cyc_List_List*atts);static void Cyc_Tcutil_check_field_atts(
struct Cyc_Position_Segment*loc,struct _dyneither_ptr*fn,struct Cyc_List_List*atts){
for(0;atts != 0;atts=atts->tl){void*_tmp77C=(void*)atts->hd;_LL4BD: if((int)
_tmp77C != 5)goto _LL4BF;_LL4BE: continue;_LL4BF: if(_tmp77C <= (void*)17)goto _LL4C1;
if(*((int*)_tmp77C)!= 1)goto _LL4C1;_LL4C0: continue;_LL4C1:;_LL4C2: {const char*
_tmpE96;void*_tmpE95[2];struct Cyc_String_pa_struct _tmpE94;struct Cyc_String_pa_struct
_tmpE93;(_tmpE93.tag=0,((_tmpE93.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*
fn),((_tmpE94.tag=0,((_tmpE94.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absyn_attribute2string((void*)atts->hd)),((_tmpE95[0]=& _tmpE94,((_tmpE95[1]=&
_tmpE93,Cyc_Tcutil_terr(loc,((_tmpE96="bad attribute %s on member %s",
_tag_dyneither(_tmpE96,sizeof(char),30))),_tag_dyneither(_tmpE95,sizeof(void*),2)))))))))))));}
_LL4BC:;}}struct Cyc_Tcutil_CVTEnv{struct _RegionHandle*r;struct Cyc_List_List*
kind_env;struct Cyc_List_List*free_vars;struct Cyc_List_List*free_evars;int
generalize_evars;int fn_result;};int Cyc_Tcutil_extract_const_from_typedef(struct
Cyc_Position_Segment*loc,int declared_const,void*t);int Cyc_Tcutil_extract_const_from_typedef(
struct Cyc_Position_Segment*loc,int declared_const,void*t){void*_tmp781=t;struct
Cyc_Absyn_Typedefdecl*_tmp782;void**_tmp783;_LL4C4: if(_tmp781 <= (void*)4)goto
_LL4C6;if(*((int*)_tmp781)!= 16)goto _LL4C6;_tmp782=((struct Cyc_Absyn_TypedefType_struct*)
_tmp781)->f3;_tmp783=((struct Cyc_Absyn_TypedefType_struct*)_tmp781)->f4;_LL4C5:
if((((struct Cyc_Absyn_Typedefdecl*)_check_null(_tmp782))->tq).real_const  || (
_tmp782->tq).print_const){if(declared_const){const char*_tmpE99;void*_tmpE98;(
_tmpE98=0,Cyc_Tcutil_warn(loc,((_tmpE99="extra const",_tag_dyneither(_tmpE99,
sizeof(char),12))),_tag_dyneither(_tmpE98,sizeof(void*),0)));}return 1;}if((
unsigned int)_tmp783)return Cyc_Tcutil_extract_const_from_typedef(loc,
declared_const,*_tmp783);else{return declared_const;}_LL4C6:;_LL4C7: return
declared_const;_LL4C3:;}static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type_level_exp(
struct Cyc_Absyn_Exp*e,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv);
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type(struct Cyc_Position_Segment*
loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv,void*expected_kind,
void*t,int put_in_effect);static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv
cvtenv,void*expected_kind,void*t,int put_in_effect){static struct Cyc_Core_Opt urgn={(
void*)((void*)3)};static struct Cyc_Core_Opt hrgn={(void*)((void*)2)};{void*_tmp786=
Cyc_Tcutil_compress(t);struct Cyc_Core_Opt*_tmp787;struct Cyc_Core_Opt**_tmp788;
struct Cyc_Core_Opt*_tmp789;struct Cyc_Core_Opt**_tmp78A;struct Cyc_Absyn_Tvar*
_tmp78B;struct Cyc_List_List*_tmp78C;struct _tuple2*_tmp78D;struct Cyc_Absyn_Enumdecl*
_tmp78E;struct Cyc_Absyn_Enumdecl**_tmp78F;struct Cyc_Absyn_DatatypeInfo _tmp790;
union Cyc_Absyn_DatatypeInfoU _tmp791;union Cyc_Absyn_DatatypeInfoU*_tmp792;struct
Cyc_List_List*_tmp793;struct Cyc_List_List**_tmp794;struct Cyc_Core_Opt*_tmp795;
struct Cyc_Core_Opt**_tmp796;struct Cyc_Absyn_DatatypeFieldInfo _tmp797;union Cyc_Absyn_DatatypeFieldInfoU
_tmp798;union Cyc_Absyn_DatatypeFieldInfoU*_tmp799;struct Cyc_List_List*_tmp79A;
struct Cyc_Absyn_PtrInfo _tmp79B;void*_tmp79C;struct Cyc_Absyn_Tqual _tmp79D;struct
Cyc_Absyn_Tqual*_tmp79E;struct Cyc_Absyn_PtrAtts _tmp79F;void*_tmp7A0;union Cyc_Absyn_Constraint*
_tmp7A1;union Cyc_Absyn_Constraint*_tmp7A2;union Cyc_Absyn_Constraint*_tmp7A3;void*
_tmp7A4;struct Cyc_Absyn_Exp*_tmp7A5;struct Cyc_Absyn_ArrayInfo _tmp7A6;void*
_tmp7A7;struct Cyc_Absyn_Tqual _tmp7A8;struct Cyc_Absyn_Tqual*_tmp7A9;struct Cyc_Absyn_Exp*
_tmp7AA;struct Cyc_Absyn_Exp**_tmp7AB;union Cyc_Absyn_Constraint*_tmp7AC;struct Cyc_Position_Segment*
_tmp7AD;struct Cyc_Absyn_FnInfo _tmp7AE;struct Cyc_List_List*_tmp7AF;struct Cyc_List_List**
_tmp7B0;struct Cyc_Core_Opt*_tmp7B1;struct Cyc_Core_Opt**_tmp7B2;void*_tmp7B3;
struct Cyc_List_List*_tmp7B4;int _tmp7B5;struct Cyc_Absyn_VarargInfo*_tmp7B6;struct
Cyc_List_List*_tmp7B7;struct Cyc_List_List*_tmp7B8;struct Cyc_List_List*_tmp7B9;
void*_tmp7BA;struct Cyc_List_List*_tmp7BB;struct Cyc_Absyn_AggrInfo _tmp7BC;union
Cyc_Absyn_AggrInfoU _tmp7BD;union Cyc_Absyn_AggrInfoU*_tmp7BE;struct Cyc_List_List*
_tmp7BF;struct Cyc_List_List**_tmp7C0;struct _tuple2*_tmp7C1;struct Cyc_List_List*
_tmp7C2;struct Cyc_List_List**_tmp7C3;struct Cyc_Absyn_Typedefdecl*_tmp7C4;struct
Cyc_Absyn_Typedefdecl**_tmp7C5;void**_tmp7C6;void***_tmp7C7;void*_tmp7C8;void*
_tmp7C9;void*_tmp7CA;void*_tmp7CB;void*_tmp7CC;struct Cyc_List_List*_tmp7CD;
_LL4C9: if((int)_tmp786 != 0)goto _LL4CB;_LL4CA: goto _LL4C8;_LL4CB: if(_tmp786 <= (
void*)4)goto _LL4DF;if(*((int*)_tmp786)!= 0)goto _LL4CD;_tmp787=((struct Cyc_Absyn_Evar_struct*)
_tmp786)->f1;_tmp788=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)
_tmp786)->f1;_tmp789=((struct Cyc_Absyn_Evar_struct*)_tmp786)->f2;_tmp78A=(struct
Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)_tmp786)->f2;_LL4CC: if(*_tmp788 == 
0)*_tmp788=Cyc_Tcutil_kind_to_opt(expected_kind);if((cvtenv.fn_result  && cvtenv.generalize_evars)
 && Cyc_Tcutil_is_region_kind(expected_kind)){if(expected_kind == (void*)4)*
_tmp78A=(struct Cyc_Core_Opt*)& urgn;else{*_tmp78A=(struct Cyc_Core_Opt*)& hrgn;}}
else{if(cvtenv.generalize_evars){struct Cyc_Absyn_Less_kb_struct _tmpE9C;struct Cyc_Absyn_Less_kb_struct*
_tmpE9B;struct Cyc_Absyn_Tvar*_tmp7CE=Cyc_Tcutil_new_tvar((void*)((_tmpE9B=
_cycalloc(sizeof(*_tmpE9B)),((_tmpE9B[0]=((_tmpE9C.tag=2,((_tmpE9C.f1=0,((
_tmpE9C.f2=(void*)expected_kind,_tmpE9C)))))),_tmpE9B)))));{struct Cyc_Absyn_VarType_struct*
_tmpEA2;struct Cyc_Absyn_VarType_struct _tmpEA1;struct Cyc_Core_Opt*_tmpEA0;*
_tmp78A=((_tmpEA0=_cycalloc(sizeof(*_tmpEA0)),((_tmpEA0->v=(void*)((void*)((
_tmpEA2=_cycalloc(sizeof(*_tmpEA2)),((_tmpEA2[0]=((_tmpEA1.tag=1,((_tmpEA1.f1=
_tmp7CE,_tmpEA1)))),_tmpEA2))))),_tmpEA0))));}_tmp78B=_tmp7CE;goto _LL4CE;}else{
cvtenv.free_evars=Cyc_Tcutil_add_free_evar(cvtenv.r,cvtenv.free_evars,t,
put_in_effect);}}goto _LL4C8;_LL4CD: if(*((int*)_tmp786)!= 1)goto _LL4CF;_tmp78B=((
struct Cyc_Absyn_VarType_struct*)_tmp786)->f1;_LL4CE:{void*_tmp7D4=Cyc_Absyn_compress_kb(
_tmp78B->kind);struct Cyc_Core_Opt*_tmp7D5;struct Cyc_Core_Opt**_tmp7D6;_LL4FE: if(*((
int*)_tmp7D4)!= 1)goto _LL500;_tmp7D5=((struct Cyc_Absyn_Unknown_kb_struct*)
_tmp7D4)->f1;_tmp7D6=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)
_tmp7D4)->f1;_LL4FF:{struct Cyc_Absyn_Less_kb_struct*_tmpEA8;struct Cyc_Absyn_Less_kb_struct
_tmpEA7;struct Cyc_Core_Opt*_tmpEA6;*_tmp7D6=((_tmpEA6=_cycalloc(sizeof(*_tmpEA6)),((
_tmpEA6->v=(void*)((void*)((_tmpEA8=_cycalloc(sizeof(*_tmpEA8)),((_tmpEA8[0]=((
_tmpEA7.tag=2,((_tmpEA7.f1=0,((_tmpEA7.f2=(void*)expected_kind,_tmpEA7)))))),
_tmpEA8))))),_tmpEA6))));}goto _LL4FD;_LL500:;_LL501: goto _LL4FD;_LL4FD:;}cvtenv.kind_env=
Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmp78B);cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(
cvtenv.r,cvtenv.free_vars,_tmp78B,put_in_effect);{void*_tmp7DA=Cyc_Absyn_compress_kb(
_tmp78B->kind);struct Cyc_Core_Opt*_tmp7DB;struct Cyc_Core_Opt**_tmp7DC;void*
_tmp7DD;_LL503: if(*((int*)_tmp7DA)!= 2)goto _LL505;_tmp7DB=((struct Cyc_Absyn_Less_kb_struct*)
_tmp7DA)->f1;_tmp7DC=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp7DA)->f1;_tmp7DD=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp7DA)->f2;
_LL504: if(Cyc_Tcutil_kind_leq(expected_kind,_tmp7DD)){struct Cyc_Absyn_Less_kb_struct*
_tmpEAE;struct Cyc_Absyn_Less_kb_struct _tmpEAD;struct Cyc_Core_Opt*_tmpEAC;*
_tmp7DC=((_tmpEAC=_cycalloc(sizeof(*_tmpEAC)),((_tmpEAC->v=(void*)((void*)((
_tmpEAE=_cycalloc(sizeof(*_tmpEAE)),((_tmpEAE[0]=((_tmpEAD.tag=2,((_tmpEAD.f1=0,((
_tmpEAD.f2=(void*)expected_kind,_tmpEAD)))))),_tmpEAE))))),_tmpEAC))));}goto
_LL502;_LL505:;_LL506: goto _LL502;_LL502:;}goto _LL4C8;_LL4CF: if(*((int*)_tmp786)
!= 13)goto _LL4D1;_tmp78C=((struct Cyc_Absyn_AnonEnumType_struct*)_tmp786)->f1;
_LL4D0: {struct Cyc_Tcenv_Genv*ge=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,
struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);struct _RegionHandle*
_tmp7E1=Cyc_Tcenv_get_fnrgn(te);{struct Cyc_List_List*prev_fields=0;unsigned int
tag_count=0;for(0;_tmp78C != 0;_tmp78C=_tmp78C->tl){struct Cyc_Absyn_Enumfield*
_tmp7E2=(struct Cyc_Absyn_Enumfield*)_tmp78C->hd;if(((int(*)(int(*compare)(struct
_dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct
_dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,(*_tmp7E2->name).f2)){
const char*_tmpEB2;void*_tmpEB1[1];struct Cyc_String_pa_struct _tmpEB0;(_tmpEB0.tag=
0,((_tmpEB0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp7E2->name).f2),((
_tmpEB1[0]=& _tmpEB0,Cyc_Tcutil_terr(_tmp7E2->loc,((_tmpEB2="duplicate enum field name %s",
_tag_dyneither(_tmpEB2,sizeof(char),29))),_tag_dyneither(_tmpEB1,sizeof(void*),1)))))));}
else{struct Cyc_List_List*_tmpEB3;prev_fields=((_tmpEB3=_region_malloc(_tmp7E1,
sizeof(*_tmpEB3)),((_tmpEB3->hd=(*_tmp7E2->name).f2,((_tmpEB3->tl=prev_fields,
_tmpEB3))))));}if(_tmp7E2->tag == 0)_tmp7E2->tag=(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(
tag_count,_tmp7E2->loc);else{if(!Cyc_Tcutil_is_const_exp(te,(struct Cyc_Absyn_Exp*)
_check_null(_tmp7E2->tag))){const char*_tmpEB7;void*_tmpEB6[1];struct Cyc_String_pa_struct
_tmpEB5;(_tmpEB5.tag=0,((_tmpEB5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*
_tmp7E2->name).f2),((_tmpEB6[0]=& _tmpEB5,Cyc_Tcutil_terr(loc,((_tmpEB7="enum field %s: expression is not constant",
_tag_dyneither(_tmpEB7,sizeof(char),42))),_tag_dyneither(_tmpEB6,sizeof(void*),1)))))));}}{
unsigned int t1=(Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)_check_null(
_tmp7E2->tag))).f1;tag_count=t1 + 1;(*_tmp7E2->name).f1=Cyc_Absyn_Abs_n(te->ns);{
struct Cyc_Tcenv_AnonEnumRes_struct*_tmpEBD;struct Cyc_Tcenv_AnonEnumRes_struct
_tmpEBC;struct _tuple7*_tmpEBB;ge->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict
d,struct _dyneither_ptr*k,struct _tuple7*v))Cyc_Dict_insert)(ge->ordinaries,(*
_tmp7E2->name).f2,(struct _tuple7*)((_tmpEBB=_cycalloc(sizeof(*_tmpEBB)),((
_tmpEBB->f1=(void*)((_tmpEBD=_cycalloc(sizeof(*_tmpEBD)),((_tmpEBD[0]=((_tmpEBC.tag=
4,((_tmpEBC.f1=(void*)t,((_tmpEBC.f2=_tmp7E2,_tmpEBC)))))),_tmpEBD)))),((_tmpEBB->f2=
1,_tmpEBB)))))));}}}}goto _LL4C8;}_LL4D1: if(*((int*)_tmp786)!= 12)goto _LL4D3;
_tmp78D=((struct Cyc_Absyn_EnumType_struct*)_tmp786)->f1;_tmp78E=((struct Cyc_Absyn_EnumType_struct*)
_tmp786)->f2;_tmp78F=(struct Cyc_Absyn_Enumdecl**)&((struct Cyc_Absyn_EnumType_struct*)
_tmp786)->f2;_LL4D2: if(*_tmp78F == 0  || ((struct Cyc_Absyn_Enumdecl*)_check_null(*
_tmp78F))->fields == 0){struct _handler_cons _tmp7ED;_push_handler(& _tmp7ED);{int
_tmp7EF=0;if(setjmp(_tmp7ED.handler))_tmp7EF=1;if(!_tmp7EF){{struct Cyc_Absyn_Enumdecl**
ed=Cyc_Tcenv_lookup_enumdecl(te,loc,_tmp78D);*_tmp78F=(struct Cyc_Absyn_Enumdecl*)*
ed;};_pop_handler();}else{void*_tmp7EE=(void*)_exn_thrown;void*_tmp7F1=_tmp7EE;
_LL508: if(_tmp7F1 != Cyc_Dict_Absent)goto _LL50A;_LL509: {struct Cyc_Tcenv_Genv*
_tmp7F2=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))
Cyc_Dict_lookup)(te->ae,te->ns);struct Cyc_Absyn_Enumdecl*_tmpEBE;struct Cyc_Absyn_Enumdecl*
_tmp7F3=(_tmpEBE=_cycalloc(sizeof(*_tmpEBE)),((_tmpEBE->sc=(void*)3,((_tmpEBE->name=
_tmp78D,((_tmpEBE->fields=0,_tmpEBE)))))));Cyc_Tc_tcEnumdecl(te,_tmp7F2,loc,
_tmp7F3);{struct Cyc_Absyn_Enumdecl**ed=Cyc_Tcenv_lookup_enumdecl(te,loc,_tmp78D);*
_tmp78F=(struct Cyc_Absyn_Enumdecl*)*ed;goto _LL507;}}_LL50A:;_LL50B:(void)_throw(
_tmp7F1);_LL507:;}}}{struct Cyc_Absyn_Enumdecl*ed=(struct Cyc_Absyn_Enumdecl*)
_check_null(*_tmp78F);*_tmp78D=(ed->name)[_check_known_subscript_notnull(1,0)];
goto _LL4C8;}_LL4D3: if(*((int*)_tmp786)!= 2)goto _LL4D5;_tmp790=((struct Cyc_Absyn_DatatypeType_struct*)
_tmp786)->f1;_tmp791=_tmp790.datatype_info;_tmp792=(union Cyc_Absyn_DatatypeInfoU*)&(((
struct Cyc_Absyn_DatatypeType_struct*)_tmp786)->f1).datatype_info;_tmp793=_tmp790.targs;
_tmp794=(struct Cyc_List_List**)&(((struct Cyc_Absyn_DatatypeType_struct*)_tmp786)->f1).targs;
_tmp795=_tmp790.rgn;_tmp796=(struct Cyc_Core_Opt**)&(((struct Cyc_Absyn_DatatypeType_struct*)
_tmp786)->f1).rgn;_LL4D4: {struct Cyc_List_List*_tmp7F5=*_tmp794;{union Cyc_Absyn_DatatypeInfoU
_tmp7F6=*_tmp792;struct Cyc_Absyn_UnknownDatatypeInfo _tmp7F7;struct _tuple2*
_tmp7F8;int _tmp7F9;struct Cyc_Absyn_Datatypedecl**_tmp7FA;struct Cyc_Absyn_Datatypedecl*
_tmp7FB;_LL50D: if((_tmp7F6.UnknownDatatype).tag != 1)goto _LL50F;_tmp7F7=(struct
Cyc_Absyn_UnknownDatatypeInfo)(_tmp7F6.UnknownDatatype).val;_tmp7F8=_tmp7F7.name;
_tmp7F9=_tmp7F7.is_extensible;_LL50E: {struct Cyc_Absyn_Datatypedecl**tudp;{
struct _handler_cons _tmp7FC;_push_handler(& _tmp7FC);{int _tmp7FE=0;if(setjmp(
_tmp7FC.handler))_tmp7FE=1;if(!_tmp7FE){tudp=Cyc_Tcenv_lookup_datatypedecl(te,
loc,_tmp7F8);;_pop_handler();}else{void*_tmp7FD=(void*)_exn_thrown;void*_tmp800=
_tmp7FD;_LL512: if(_tmp800 != Cyc_Dict_Absent)goto _LL514;_LL513: {struct Cyc_Tcenv_Genv*
_tmp801=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))
Cyc_Dict_lookup)(te->ae,te->ns);struct Cyc_Absyn_Datatypedecl*_tmpEBF;struct Cyc_Absyn_Datatypedecl*
_tmp802=(_tmpEBF=_cycalloc(sizeof(*_tmpEBF)),((_tmpEBF->sc=(void*)3,((_tmpEBF->name=
_tmp7F8,((_tmpEBF->tvs=0,((_tmpEBF->fields=0,((_tmpEBF->is_extensible=_tmp7F9,
_tmpEBF)))))))))));Cyc_Tc_tcDatatypedecl(te,_tmp801,loc,_tmp802);tudp=Cyc_Tcenv_lookup_datatypedecl(
te,loc,_tmp7F8);if(_tmp7F5 != 0){{const char*_tmpEC3;void*_tmpEC2[1];struct Cyc_String_pa_struct
_tmpEC1;(_tmpEC1.tag=0,((_tmpEC1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp7F8)),((_tmpEC2[0]=& _tmpEC1,Cyc_Tcutil_terr(loc,((
_tmpEC3="declare parameterized datatype %s before using",_tag_dyneither(_tmpEC3,
sizeof(char),47))),_tag_dyneither(_tmpEC2,sizeof(void*),1)))))));}return cvtenv;}
goto _LL511;}_LL514:;_LL515:(void)_throw(_tmp800);_LL511:;}}}if(_tmp7F9  && !(*
tudp)->is_extensible){const char*_tmpEC7;void*_tmpEC6[1];struct Cyc_String_pa_struct
_tmpEC5;(_tmpEC5.tag=0,((_tmpEC5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp7F8)),((_tmpEC6[0]=& _tmpEC5,Cyc_Tcutil_terr(loc,((
_tmpEC7="datatype %s was not declared @extensible",_tag_dyneither(_tmpEC7,
sizeof(char),41))),_tag_dyneither(_tmpEC6,sizeof(void*),1)))))));}*_tmp792=Cyc_Absyn_KnownDatatype(
tudp);_tmp7FB=*tudp;goto _LL510;}_LL50F: if((_tmp7F6.KnownDatatype).tag != 2)goto
_LL50C;_tmp7FA=(struct Cyc_Absyn_Datatypedecl**)(_tmp7F6.KnownDatatype).val;
_tmp7FB=*_tmp7FA;_LL510: if(!((unsigned int)*_tmp796)){struct Cyc_Core_Opt*_tmpEC8;*
_tmp796=((_tmpEC8=_cycalloc(sizeof(*_tmpEC8)),((_tmpEC8->v=(void*)Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,0),_tmpEC8))));}if((unsigned int)*_tmp796){
void*_tmp80B=(void*)((struct Cyc_Core_Opt*)_check_null(*_tmp796))->v;cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,(void*)3,_tmp80B,1);}{struct Cyc_List_List*tvs=_tmp7FB->tvs;for(0;
_tmp7F5 != 0  && tvs != 0;(_tmp7F5=_tmp7F5->tl,tvs=tvs->tl)){void*t1=(void*)_tmp7F5->hd;
void*k1=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd);cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,k1,t1,1);}if(_tmp7F5 != 0){const char*_tmpECC;void*_tmpECB[1];struct
Cyc_String_pa_struct _tmpECA;(_tmpECA.tag=0,((_tmpECA.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp7FB->name)),((_tmpECB[0]=&
_tmpECA,Cyc_Tcutil_terr(loc,((_tmpECC="too many type arguments for datatype %s",
_tag_dyneither(_tmpECC,sizeof(char),40))),_tag_dyneither(_tmpECB,sizeof(void*),1)))))));}
if(tvs != 0){struct Cyc_List_List*hidden_ts=0;for(0;tvs != 0;tvs=tvs->tl){void*k1=
Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd);void*e=Cyc_Absyn_new_evar(0,
0);{struct Cyc_List_List*_tmpECD;hidden_ts=((_tmpECD=_cycalloc(sizeof(*_tmpECD)),((
_tmpECD->hd=(void*)e,((_tmpECD->tl=hidden_ts,_tmpECD))))));}cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,k1,e,1);}*_tmp794=Cyc_List_imp_append(*_tmp794,Cyc_List_imp_rev(
hidden_ts));}goto _LL50C;}_LL50C:;}goto _LL4C8;}_LL4D5: if(*((int*)_tmp786)!= 3)
goto _LL4D7;_tmp797=((struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp786)->f1;
_tmp798=_tmp797.field_info;_tmp799=(union Cyc_Absyn_DatatypeFieldInfoU*)&(((
struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp786)->f1).field_info;_tmp79A=
_tmp797.targs;_LL4D6:{union Cyc_Absyn_DatatypeFieldInfoU _tmp810=*_tmp799;struct
Cyc_Absyn_UnknownDatatypeFieldInfo _tmp811;struct _tuple2*_tmp812;struct _tuple2*
_tmp813;int _tmp814;struct _tuple3 _tmp815;struct Cyc_Absyn_Datatypedecl*_tmp816;
struct Cyc_Absyn_Datatypefield*_tmp817;_LL517: if((_tmp810.UnknownDatatypefield).tag
!= 1)goto _LL519;_tmp811=(struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmp810.UnknownDatatypefield).val;
_tmp812=_tmp811.datatype_name;_tmp813=_tmp811.field_name;_tmp814=_tmp811.is_extensible;
_LL518: {struct Cyc_Absyn_Datatypedecl*tud;struct Cyc_Absyn_Datatypefield*tuf;{
struct _handler_cons _tmp818;_push_handler(& _tmp818);{int _tmp81A=0;if(setjmp(
_tmp818.handler))_tmp81A=1;if(!_tmp81A){*Cyc_Tcenv_lookup_datatypedecl(te,loc,
_tmp812);;_pop_handler();}else{void*_tmp819=(void*)_exn_thrown;void*_tmp81C=
_tmp819;_LL51C: if(_tmp81C != Cyc_Dict_Absent)goto _LL51E;_LL51D:{const char*_tmpED1;
void*_tmpED0[1];struct Cyc_String_pa_struct _tmpECF;(_tmpECF.tag=0,((_tmpECF.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp812)),((
_tmpED0[0]=& _tmpECF,Cyc_Tcutil_terr(loc,((_tmpED1="unbound datatype %s",
_tag_dyneither(_tmpED1,sizeof(char),20))),_tag_dyneither(_tmpED0,sizeof(void*),1)))))));}
return cvtenv;_LL51E:;_LL51F:(void)_throw(_tmp81C);_LL51B:;}}}{struct
_handler_cons _tmp820;_push_handler(& _tmp820);{int _tmp822=0;if(setjmp(_tmp820.handler))
_tmp822=1;if(!_tmp822){{struct _RegionHandle*_tmp823=Cyc_Tcenv_get_fnrgn(te);void*
_tmp824=Cyc_Tcenv_lookup_ordinary(_tmp823,te,loc,_tmp813);struct Cyc_Absyn_Datatypedecl*
_tmp825;struct Cyc_Absyn_Datatypefield*_tmp826;_LL521: if(*((int*)_tmp824)!= 2)
goto _LL523;_tmp825=((struct Cyc_Tcenv_DatatypeRes_struct*)_tmp824)->f1;_tmp826=((
struct Cyc_Tcenv_DatatypeRes_struct*)_tmp824)->f2;_LL522: tuf=_tmp826;tud=_tmp825;
if(_tmp814  && !tud->is_extensible){const char*_tmpED5;void*_tmpED4[1];struct Cyc_String_pa_struct
_tmpED3;(_tmpED3.tag=0,((_tmpED3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp812)),((_tmpED4[0]=& _tmpED3,Cyc_Tcutil_terr(loc,((
_tmpED5="datatype %s was not declared @extensible",_tag_dyneither(_tmpED5,
sizeof(char),41))),_tag_dyneither(_tmpED4,sizeof(void*),1)))))));}goto _LL520;
_LL523:;_LL524:{const char*_tmpEDA;void*_tmpED9[2];struct Cyc_String_pa_struct
_tmpED8;struct Cyc_String_pa_struct _tmpED7;(_tmpED7.tag=0,((_tmpED7.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp812)),((
_tmpED8.tag=0,((_tmpED8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp813)),((_tmpED9[0]=& _tmpED8,((_tmpED9[1]=& _tmpED7,Cyc_Tcutil_terr(loc,((
_tmpEDA="unbound field %s in type datatype %s",_tag_dyneither(_tmpEDA,sizeof(
char),37))),_tag_dyneither(_tmpED9,sizeof(void*),2)))))))))))));}{struct Cyc_Tcutil_CVTEnv
_tmp82E=cvtenv;_npop_handler(0);return _tmp82E;}_LL520:;};_pop_handler();}else{
void*_tmp821=(void*)_exn_thrown;void*_tmp830=_tmp821;_LL526: if(_tmp830 != Cyc_Dict_Absent)
goto _LL528;_LL527:{const char*_tmpEDF;void*_tmpEDE[2];struct Cyc_String_pa_struct
_tmpEDD;struct Cyc_String_pa_struct _tmpEDC;(_tmpEDC.tag=0,((_tmpEDC.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp812)),((
_tmpEDD.tag=0,((_tmpEDD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp813)),((_tmpEDE[0]=& _tmpEDD,((_tmpEDE[1]=& _tmpEDC,Cyc_Tcutil_terr(loc,((
_tmpEDF="unbound field %s in type datatype %s",_tag_dyneither(_tmpEDF,sizeof(
char),37))),_tag_dyneither(_tmpEDE,sizeof(void*),2)))))))))))));}return cvtenv;
_LL528:;_LL529:(void)_throw(_tmp830);_LL525:;}}}*_tmp799=Cyc_Absyn_KnownDatatypefield(
tud,tuf);_tmp816=tud;_tmp817=tuf;goto _LL51A;}_LL519: if((_tmp810.KnownDatatypefield).tag
!= 2)goto _LL516;_tmp815=(struct _tuple3)(_tmp810.KnownDatatypefield).val;_tmp816=
_tmp815.f1;_tmp817=_tmp815.f2;_LL51A: {struct Cyc_List_List*tvs=_tmp816->tvs;for(
0;_tmp79A != 0  && tvs != 0;(_tmp79A=_tmp79A->tl,tvs=tvs->tl)){void*t1=(void*)
_tmp79A->hd;void*k1=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd);cvtenv=
Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k1,t1,1);}if(_tmp79A != 0){const char*
_tmpEE4;void*_tmpEE3[2];struct Cyc_String_pa_struct _tmpEE2;struct Cyc_String_pa_struct
_tmpEE1;(_tmpEE1.tag=0,((_tmpEE1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp817->name)),((_tmpEE2.tag=0,((_tmpEE2.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp816->name)),((
_tmpEE3[0]=& _tmpEE2,((_tmpEE3[1]=& _tmpEE1,Cyc_Tcutil_terr(loc,((_tmpEE4="too many type arguments for datatype %s.%s",
_tag_dyneither(_tmpEE4,sizeof(char),43))),_tag_dyneither(_tmpEE3,sizeof(void*),2)))))))))))));}
if(tvs != 0){const char*_tmpEE9;void*_tmpEE8[2];struct Cyc_String_pa_struct _tmpEE7;
struct Cyc_String_pa_struct _tmpEE6;(_tmpEE6.tag=0,((_tmpEE6.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp817->name)),((
_tmpEE7.tag=0,((_tmpEE7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp816->name)),((_tmpEE8[0]=& _tmpEE7,((_tmpEE8[1]=& _tmpEE6,Cyc_Tcutil_terr(loc,((
_tmpEE9="too few type arguments for datatype %s.%s",_tag_dyneither(_tmpEE9,
sizeof(char),42))),_tag_dyneither(_tmpEE8,sizeof(void*),2)))))))))))));}goto
_LL516;}_LL516:;}goto _LL4C8;_LL4D7: if(*((int*)_tmp786)!= 4)goto _LL4D9;_tmp79B=((
struct Cyc_Absyn_PointerType_struct*)_tmp786)->f1;_tmp79C=_tmp79B.elt_typ;_tmp79D=
_tmp79B.elt_tq;_tmp79E=(struct Cyc_Absyn_Tqual*)&(((struct Cyc_Absyn_PointerType_struct*)
_tmp786)->f1).elt_tq;_tmp79F=_tmp79B.ptr_atts;_tmp7A0=_tmp79F.rgn;_tmp7A1=
_tmp79F.nullable;_tmp7A2=_tmp79F.bounds;_tmp7A3=_tmp79F.zero_term;_LL4D8: {int
is_zero_terminated;cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)0,
_tmp79C,1);_tmp79E->real_const=Cyc_Tcutil_extract_const_from_typedef(loc,_tmp79E->print_const,
_tmp79C);cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)5,_tmp7A0,1);{
union Cyc_Absyn_Constraint*_tmp83D=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*
x))Cyc_Absyn_compress_conref)(_tmp7A3);union Cyc_Absyn_Constraint _tmp83E;int
_tmp83F;union Cyc_Absyn_Constraint _tmp840;int _tmp841;_LL52B: _tmp83E=*_tmp83D;if((
_tmp83E.No_constr).tag != 3)goto _LL52D;_tmp83F=(int)(_tmp83E.No_constr).val;
_LL52C:{void*_tmp842=Cyc_Tcutil_compress(_tmp79C);void*_tmp843;_LL532: if(_tmp842
<= (void*)4)goto _LL534;if(*((int*)_tmp842)!= 5)goto _LL534;_tmp843=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp842)->f2;if((int)_tmp843 != 0)goto _LL534;
_LL533:((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*
y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp7A3,Cyc_Absyn_true_conref);
is_zero_terminated=1;goto _LL531;_LL534:;_LL535:((int(*)(int(*cmp)(int,int),union
Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(
Cyc_Core_intcmp,_tmp7A3,Cyc_Absyn_false_conref);is_zero_terminated=0;goto _LL531;
_LL531:;}goto _LL52A;_LL52D: _tmp840=*_tmp83D;if((_tmp840.Eq_constr).tag != 1)goto
_LL52F;_tmp841=(int)(_tmp840.Eq_constr).val;if(_tmp841 != 1)goto _LL52F;_LL52E: if(
!Cyc_Tcutil_admits_zero(_tmp79C)){const char*_tmpEED;void*_tmpEEC[1];struct Cyc_String_pa_struct
_tmpEEB;(_tmpEEB.tag=0,((_tmpEEB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp79C)),((_tmpEEC[0]=& _tmpEEB,Cyc_Tcutil_terr(loc,((
_tmpEED="cannot have a pointer to zero-terminated %s elements",_tag_dyneither(
_tmpEED,sizeof(char),53))),_tag_dyneither(_tmpEEC,sizeof(void*),1)))))));}
is_zero_terminated=1;goto _LL52A;_LL52F:;_LL530: is_zero_terminated=0;goto _LL52A;
_LL52A:;}{void*_tmp847=Cyc_Absyn_conref_constr(Cyc_Absyn_bounds_one,_tmp7A2);
struct Cyc_Absyn_Exp*_tmp848;_LL537: if((int)_tmp847 != 0)goto _LL539;_LL538: goto
_LL536;_LL539: if(_tmp847 <= (void*)1)goto _LL536;if(*((int*)_tmp847)!= 0)goto
_LL536;_tmp848=((struct Cyc_Absyn_Upper_b_struct*)_tmp847)->f1;_LL53A: {struct
_RegionHandle*_tmp849=Cyc_Tcenv_get_fnrgn(te);{struct Cyc_Tcenv_Tenv*_tmp84A=Cyc_Tcenv_allow_valueof(
_tmp849,te);Cyc_Tcexp_tcExp(_tmp84A,0,_tmp848);}cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(
_tmp848,te,cvtenv);if(!Cyc_Tcutil_coerce_uint_typ(te,_tmp848)){const char*_tmpEF0;
void*_tmpEEF;(_tmpEEF=0,Cyc_Tcutil_terr(loc,((_tmpEF0="pointer bounds expression is not an unsigned int",
_tag_dyneither(_tmpEF0,sizeof(char),49))),_tag_dyneither(_tmpEEF,sizeof(void*),0)));}{
unsigned int _tmp84E;int _tmp84F;struct _tuple13 _tmp84D=Cyc_Evexp_eval_const_uint_exp(
_tmp848);_tmp84E=_tmp84D.f1;_tmp84F=_tmp84D.f2;if(is_zero_terminated  && (!
_tmp84F  || _tmp84E < 1)){const char*_tmpEF3;void*_tmpEF2;(_tmpEF2=0,Cyc_Tcutil_terr(
loc,((_tmpEF3="zero-terminated pointer cannot point to empty sequence",
_tag_dyneither(_tmpEF3,sizeof(char),55))),_tag_dyneither(_tmpEF2,sizeof(void*),0)));}
goto _LL536;}}_LL536:;}goto _LL4C8;}_LL4D9: if(*((int*)_tmp786)!= 18)goto _LL4DB;
_tmp7A4=(void*)((struct Cyc_Absyn_TagType_struct*)_tmp786)->f1;_LL4DA: cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,(void*)7,_tmp7A4,1);goto _LL4C8;_LL4DB: if(*((int*)_tmp786)!= 17)
goto _LL4DD;_tmp7A5=((struct Cyc_Absyn_ValueofType_struct*)_tmp786)->f1;_LL4DC: {
struct _RegionHandle*_tmp852=Cyc_Tcenv_get_fnrgn(te);{struct Cyc_Tcenv_Tenv*
_tmp853=Cyc_Tcenv_allow_valueof(_tmp852,te);Cyc_Tcexp_tcExp(_tmp853,0,_tmp7A5);}
if(!Cyc_Tcutil_coerce_uint_typ(te,_tmp7A5)){const char*_tmpEF6;void*_tmpEF5;(
_tmpEF5=0,Cyc_Tcutil_terr(loc,((_tmpEF6="valueof_t requires an int expression",
_tag_dyneither(_tmpEF6,sizeof(char),37))),_tag_dyneither(_tmpEF5,sizeof(void*),0)));}
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp7A5,te,cvtenv);goto _LL4C8;}
_LL4DD: if(*((int*)_tmp786)!= 5)goto _LL4DF;_LL4DE: goto _LL4E0;_LL4DF: if((int)
_tmp786 != 1)goto _LL4E1;_LL4E0: goto _LL4E2;_LL4E1: if(_tmp786 <= (void*)4)goto _LL4EF;
if(*((int*)_tmp786)!= 6)goto _LL4E3;_LL4E2: goto _LL4C8;_LL4E3: if(*((int*)_tmp786)
!= 7)goto _LL4E5;_tmp7A6=((struct Cyc_Absyn_ArrayType_struct*)_tmp786)->f1;_tmp7A7=
_tmp7A6.elt_type;_tmp7A8=_tmp7A6.tq;_tmp7A9=(struct Cyc_Absyn_Tqual*)&(((struct
Cyc_Absyn_ArrayType_struct*)_tmp786)->f1).tq;_tmp7AA=_tmp7A6.num_elts;_tmp7AB=(
struct Cyc_Absyn_Exp**)&(((struct Cyc_Absyn_ArrayType_struct*)_tmp786)->f1).num_elts;
_tmp7AC=_tmp7A6.zero_term;_tmp7AD=_tmp7A6.zt_loc;_LL4E4: {struct Cyc_Absyn_Exp*
_tmp856=*_tmp7AB;cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)1,
_tmp7A7,1);_tmp7A9->real_const=Cyc_Tcutil_extract_const_from_typedef(loc,_tmp7A9->print_const,
_tmp7A7);{int is_zero_terminated;{union Cyc_Absyn_Constraint*_tmp857=((union Cyc_Absyn_Constraint*(*)(
union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)(_tmp7AC);union Cyc_Absyn_Constraint
_tmp858;int _tmp859;union Cyc_Absyn_Constraint _tmp85A;int _tmp85B;_LL53C: _tmp858=*
_tmp857;if((_tmp858.No_constr).tag != 3)goto _LL53E;_tmp859=(int)(_tmp858.No_constr).val;
_LL53D:((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*
y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp7AC,Cyc_Absyn_false_conref);
is_zero_terminated=0;goto _LL53B;_LL53E: _tmp85A=*_tmp857;if((_tmp85A.Eq_constr).tag
!= 1)goto _LL540;_tmp85B=(int)(_tmp85A.Eq_constr).val;if(_tmp85B != 1)goto _LL540;
_LL53F: if(!Cyc_Tcutil_admits_zero(_tmp7A7)){const char*_tmpEFA;void*_tmpEF9[1];
struct Cyc_String_pa_struct _tmpEF8;(_tmpEF8.tag=0,((_tmpEF8.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp7A7)),((_tmpEF9[
0]=& _tmpEF8,Cyc_Tcutil_terr(loc,((_tmpEFA="cannot have a zero-terminated array of %s elements",
_tag_dyneither(_tmpEFA,sizeof(char),51))),_tag_dyneither(_tmpEF9,sizeof(void*),1)))))));}
is_zero_terminated=1;goto _LL53B;_LL540:;_LL541: is_zero_terminated=0;goto _LL53B;
_LL53B:;}if(_tmp856 == 0){if(is_zero_terminated)*_tmp7AB=(_tmp856=(struct Cyc_Absyn_Exp*)
Cyc_Absyn_uint_exp(1,0));else{{const char*_tmpEFD;void*_tmpEFC;(_tmpEFC=0,Cyc_Tcutil_warn(
loc,((_tmpEFD="array bound defaults to 1 here",_tag_dyneither(_tmpEFD,sizeof(
char),31))),_tag_dyneither(_tmpEFC,sizeof(void*),0)));}*_tmp7AB=(_tmp856=(struct
Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(1,0));}}Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)
_tmp856);if(!Cyc_Tcutil_is_const_exp(te,(struct Cyc_Absyn_Exp*)_tmp856)){const
char*_tmpF00;void*_tmpEFF;(_tmpEFF=0,Cyc_Tcutil_terr(loc,((_tmpF00="array bounds expression is not constant",
_tag_dyneither(_tmpF00,sizeof(char),40))),_tag_dyneither(_tmpEFF,sizeof(void*),0)));}
if(!Cyc_Tcutil_coerce_uint_typ(te,(struct Cyc_Absyn_Exp*)_tmp856)){const char*
_tmpF03;void*_tmpF02;(_tmpF02=0,Cyc_Tcutil_terr(loc,((_tmpF03="array bounds expression is not an unsigned int",
_tag_dyneither(_tmpF03,sizeof(char),47))),_tag_dyneither(_tmpF02,sizeof(void*),0)));}{
unsigned int _tmp866;int _tmp867;struct _tuple13 _tmp865=Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)_tmp856);_tmp866=_tmp865.f1;_tmp867=_tmp865.f2;if((
is_zero_terminated  && _tmp867) && _tmp866 < 1){const char*_tmpF06;void*_tmpF05;(
_tmpF05=0,Cyc_Tcutil_warn(loc,((_tmpF06="zero terminated array cannot have zero elements",
_tag_dyneither(_tmpF06,sizeof(char),48))),_tag_dyneither(_tmpF05,sizeof(void*),0)));}
if((_tmp867  && _tmp866 < 1) && Cyc_Cyclone_tovc_r){{const char*_tmpF09;void*
_tmpF08;(_tmpF08=0,Cyc_Tcutil_warn(loc,((_tmpF09="arrays with 0 elements are not supported except with gcc -- changing to 1.",
_tag_dyneither(_tmpF09,sizeof(char),75))),_tag_dyneither(_tmpF08,sizeof(void*),0)));}*
_tmp7AB=(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(1,0);}goto _LL4C8;}}}_LL4E5: if(*((
int*)_tmp786)!= 8)goto _LL4E7;_tmp7AE=((struct Cyc_Absyn_FnType_struct*)_tmp786)->f1;
_tmp7AF=_tmp7AE.tvars;_tmp7B0=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_struct*)
_tmp786)->f1).tvars;_tmp7B1=_tmp7AE.effect;_tmp7B2=(struct Cyc_Core_Opt**)&(((
struct Cyc_Absyn_FnType_struct*)_tmp786)->f1).effect;_tmp7B3=_tmp7AE.ret_typ;
_tmp7B4=_tmp7AE.args;_tmp7B5=_tmp7AE.c_varargs;_tmp7B6=_tmp7AE.cyc_varargs;
_tmp7B7=_tmp7AE.rgn_po;_tmp7B8=_tmp7AE.attributes;_LL4E6: {int num_convs=0;int
seen_cdecl=0;int seen_stdcall=0;int seen_fastcall=0;int seen_format=0;void*ft=(void*)
0;int fmt_desc_arg=- 1;int fmt_arg_start=- 1;for(0;_tmp7B8 != 0;_tmp7B8=_tmp7B8->tl){
if(!Cyc_Absyn_fntype_att((void*)_tmp7B8->hd)){const char*_tmpF0D;void*_tmpF0C[1];
struct Cyc_String_pa_struct _tmpF0B;(_tmpF0B.tag=0,((_tmpF0B.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((void*)_tmp7B8->hd)),((
_tmpF0C[0]=& _tmpF0B,Cyc_Tcutil_terr(loc,((_tmpF0D="bad function type attribute %s",
_tag_dyneither(_tmpF0D,sizeof(char),31))),_tag_dyneither(_tmpF0C,sizeof(void*),1)))))));}{
void*_tmp86F=(void*)_tmp7B8->hd;void*_tmp870;int _tmp871;int _tmp872;_LL543: if((
int)_tmp86F != 0)goto _LL545;_LL544: if(!seen_stdcall){seen_stdcall=1;++ num_convs;}
goto _LL542;_LL545: if((int)_tmp86F != 1)goto _LL547;_LL546: if(!seen_cdecl){
seen_cdecl=1;++ num_convs;}goto _LL542;_LL547: if((int)_tmp86F != 2)goto _LL549;
_LL548: if(!seen_fastcall){seen_fastcall=1;++ num_convs;}goto _LL542;_LL549: if(
_tmp86F <= (void*)17)goto _LL54B;if(*((int*)_tmp86F)!= 3)goto _LL54B;_tmp870=(void*)((
struct Cyc_Absyn_Format_att_struct*)_tmp86F)->f1;_tmp871=((struct Cyc_Absyn_Format_att_struct*)
_tmp86F)->f2;_tmp872=((struct Cyc_Absyn_Format_att_struct*)_tmp86F)->f3;_LL54A:
if(!seen_format){seen_format=1;ft=_tmp870;fmt_desc_arg=_tmp871;fmt_arg_start=
_tmp872;}else{const char*_tmpF10;void*_tmpF0F;(_tmpF0F=0,Cyc_Tcutil_terr(loc,((
_tmpF10="function can't have multiple format attributes",_tag_dyneither(_tmpF10,
sizeof(char),47))),_tag_dyneither(_tmpF0F,sizeof(void*),0)));}goto _LL542;_LL54B:;
_LL54C: goto _LL542;_LL542:;}}if(num_convs > 1){const char*_tmpF13;void*_tmpF12;(
_tmpF12=0,Cyc_Tcutil_terr(loc,((_tmpF13="function can't have multiple calling conventions",
_tag_dyneither(_tmpF13,sizeof(char),49))),_tag_dyneither(_tmpF12,sizeof(void*),0)));}
Cyc_Tcutil_check_unique_tvars(loc,*_tmp7B0);{struct Cyc_List_List*b=*_tmp7B0;for(
0;b != 0;b=b->tl){((struct Cyc_Absyn_Tvar*)b->hd)->identity=Cyc_Tcutil_new_tvar_id();
cvtenv.kind_env=Cyc_Tcutil_add_bound_tvar(cvtenv.kind_env,(struct Cyc_Absyn_Tvar*)
b->hd);{void*_tmp877=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)b->hd)->kind);
void*_tmp878;_LL54E: if(*((int*)_tmp877)!= 0)goto _LL550;_tmp878=(void*)((struct
Cyc_Absyn_Eq_kb_struct*)_tmp877)->f1;if((int)_tmp878 != 1)goto _LL550;_LL54F:{
const char*_tmpF17;void*_tmpF16[1];struct Cyc_String_pa_struct _tmpF15;(_tmpF15.tag=
0,((_tmpF15.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)
b->hd)->name),((_tmpF16[0]=& _tmpF15,Cyc_Tcutil_terr(loc,((_tmpF17="function attempts to abstract Mem type variable %s",
_tag_dyneither(_tmpF17,sizeof(char),51))),_tag_dyneither(_tmpF16,sizeof(void*),1)))))));}
goto _LL54D;_LL550:;_LL551: goto _LL54D;_LL54D:;}}}{struct _RegionHandle*_tmp87C=Cyc_Tcenv_get_fnrgn(
te);{struct Cyc_Tcutil_CVTEnv _tmpF18;struct Cyc_Tcutil_CVTEnv _tmp87D=(_tmpF18.r=
_tmp87C,((_tmpF18.kind_env=cvtenv.kind_env,((_tmpF18.free_vars=0,((_tmpF18.free_evars=
0,((_tmpF18.generalize_evars=cvtenv.generalize_evars,((_tmpF18.fn_result=1,
_tmpF18)))))))))));_tmp87D=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp87D,(void*)1,
_tmp7B3,1);_tmp87D.fn_result=0;{struct Cyc_List_List*a=_tmp7B4;for(0;a != 0;a=a->tl){
struct _tuple9*_tmp87E=(struct _tuple9*)a->hd;void*_tmp87F=(*_tmp87E).f3;_tmp87D=
Cyc_Tcutil_i_check_valid_type(loc,te,_tmp87D,(void*)1,_tmp87F,1);((*_tmp87E).f2).real_const=
Cyc_Tcutil_extract_const_from_typedef(loc,((*_tmp87E).f2).print_const,_tmp87F);}}
if(_tmp7B6 != 0){if(_tmp7B5){const char*_tmpF1B;void*_tmpF1A;(_tmpF1A=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpF1B="both c_vararg and cyc_vararg",
_tag_dyneither(_tmpF1B,sizeof(char),29))),_tag_dyneither(_tmpF1A,sizeof(void*),0)));}{
void*_tmp883;int _tmp884;struct Cyc_Absyn_VarargInfo _tmp882=*_tmp7B6;_tmp883=
_tmp882.type;_tmp884=_tmp882.inject;_tmp87D=Cyc_Tcutil_i_check_valid_type(loc,te,
_tmp87D,(void*)1,_tmp883,1);(_tmp7B6->tq).real_const=Cyc_Tcutil_extract_const_from_typedef(
loc,(_tmp7B6->tq).print_const,_tmp883);if(_tmp884){void*_tmp885=Cyc_Tcutil_compress(
_tmp883);_LL553: if(_tmp885 <= (void*)4)goto _LL555;if(*((int*)_tmp885)!= 2)goto
_LL555;_LL554: goto _LL552;_LL555:;_LL556:{const char*_tmpF1E;void*_tmpF1D;(_tmpF1D=
0,Cyc_Tcutil_terr(loc,((_tmpF1E="can't inject a non-datatype type",
_tag_dyneither(_tmpF1E,sizeof(char),33))),_tag_dyneither(_tmpF1D,sizeof(void*),0)));}
goto _LL552;_LL552:;}}}if(seen_format){int _tmp888=((int(*)(struct Cyc_List_List*x))
Cyc_List_length)(_tmp7B4);if((((fmt_desc_arg < 0  || fmt_desc_arg > _tmp888) || 
fmt_arg_start < 0) || _tmp7B6 == 0  && fmt_arg_start != 0) || _tmp7B6 != 0  && 
fmt_arg_start != _tmp888 + 1){const char*_tmpF21;void*_tmpF20;(_tmpF20=0,Cyc_Tcutil_terr(
loc,((_tmpF21="bad format descriptor",_tag_dyneither(_tmpF21,sizeof(char),22))),
_tag_dyneither(_tmpF20,sizeof(void*),0)));}else{void*_tmp88C;struct _tuple9
_tmp88B=*((struct _tuple9*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp7B4,
fmt_desc_arg - 1);_tmp88C=_tmp88B.f3;{void*_tmp88D=Cyc_Tcutil_compress(_tmp88C);
struct Cyc_Absyn_PtrInfo _tmp88E;void*_tmp88F;struct Cyc_Absyn_PtrAtts _tmp890;union
Cyc_Absyn_Constraint*_tmp891;union Cyc_Absyn_Constraint*_tmp892;_LL558: if(_tmp88D
<= (void*)4)goto _LL55A;if(*((int*)_tmp88D)!= 4)goto _LL55A;_tmp88E=((struct Cyc_Absyn_PointerType_struct*)
_tmp88D)->f1;_tmp88F=_tmp88E.elt_typ;_tmp890=_tmp88E.ptr_atts;_tmp891=_tmp890.bounds;
_tmp892=_tmp890.zero_term;_LL559:{struct _tuple0 _tmpF22;struct _tuple0 _tmp894=(
_tmpF22.f1=Cyc_Tcutil_compress(_tmp88F),((_tmpF22.f2=Cyc_Absyn_conref_def((void*)((
void*)0),_tmp891),_tmpF22)));void*_tmp895;void*_tmp896;void*_tmp897;void*_tmp898;
_LL55D: _tmp895=_tmp894.f1;if(_tmp895 <= (void*)4)goto _LL55F;if(*((int*)_tmp895)!= 
5)goto _LL55F;_tmp896=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp895)->f1;if((
int)_tmp896 != 2)goto _LL55F;_tmp897=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp895)->f2;if((int)_tmp897 != 0)goto _LL55F;_tmp898=_tmp894.f2;if((int)_tmp898 != 
0)goto _LL55F;_LL55E: goto _LL55C;_LL55F:;_LL560:{const char*_tmpF25;void*_tmpF24;(
_tmpF24=0,Cyc_Tcutil_terr(loc,((_tmpF25="format descriptor is not a char ? type",
_tag_dyneither(_tmpF25,sizeof(char),39))),_tag_dyneither(_tmpF24,sizeof(void*),0)));}
goto _LL55C;_LL55C:;}goto _LL557;_LL55A:;_LL55B:{const char*_tmpF28;void*_tmpF27;(
_tmpF27=0,Cyc_Tcutil_terr(loc,((_tmpF28="format descriptor is not a char ? type",
_tag_dyneither(_tmpF28,sizeof(char),39))),_tag_dyneither(_tmpF27,sizeof(void*),0)));}
goto _LL557;_LL557:;}if(fmt_arg_start != 0){int problem;{struct _tuple0 _tmpF29;
struct _tuple0 _tmp89E=(_tmpF29.f1=ft,((_tmpF29.f2=Cyc_Tcutil_compress(((struct Cyc_Absyn_VarargInfo*)
_check_null(_tmp7B6))->type),_tmpF29)));void*_tmp89F;void*_tmp8A0;struct Cyc_Absyn_DatatypeInfo
_tmp8A1;union Cyc_Absyn_DatatypeInfoU _tmp8A2;struct Cyc_Absyn_Datatypedecl**
_tmp8A3;struct Cyc_Absyn_Datatypedecl*_tmp8A4;void*_tmp8A5;void*_tmp8A6;struct Cyc_Absyn_DatatypeInfo
_tmp8A7;union Cyc_Absyn_DatatypeInfoU _tmp8A8;struct Cyc_Absyn_Datatypedecl**
_tmp8A9;struct Cyc_Absyn_Datatypedecl*_tmp8AA;_LL562: _tmp89F=_tmp89E.f1;if((int)
_tmp89F != 0)goto _LL564;_tmp8A0=_tmp89E.f2;if(_tmp8A0 <= (void*)4)goto _LL564;if(*((
int*)_tmp8A0)!= 2)goto _LL564;_tmp8A1=((struct Cyc_Absyn_DatatypeType_struct*)
_tmp8A0)->f1;_tmp8A2=_tmp8A1.datatype_info;if((_tmp8A2.KnownDatatype).tag != 2)
goto _LL564;_tmp8A3=(struct Cyc_Absyn_Datatypedecl**)(_tmp8A2.KnownDatatype).val;
_tmp8A4=*_tmp8A3;_LL563: problem=Cyc_Absyn_qvar_cmp(_tmp8A4->name,Cyc_Absyn_datatype_print_arg_qvar)
!= 0;goto _LL561;_LL564: _tmp8A5=_tmp89E.f1;if((int)_tmp8A5 != 1)goto _LL566;_tmp8A6=
_tmp89E.f2;if(_tmp8A6 <= (void*)4)goto _LL566;if(*((int*)_tmp8A6)!= 2)goto _LL566;
_tmp8A7=((struct Cyc_Absyn_DatatypeType_struct*)_tmp8A6)->f1;_tmp8A8=_tmp8A7.datatype_info;
if((_tmp8A8.KnownDatatype).tag != 2)goto _LL566;_tmp8A9=(struct Cyc_Absyn_Datatypedecl**)(
_tmp8A8.KnownDatatype).val;_tmp8AA=*_tmp8A9;_LL565: problem=Cyc_Absyn_qvar_cmp(
_tmp8AA->name,Cyc_Absyn_datatype_scanf_arg_qvar)!= 0;goto _LL561;_LL566:;_LL567:
problem=1;goto _LL561;_LL561:;}if(problem){const char*_tmpF2C;void*_tmpF2B;(
_tmpF2B=0,Cyc_Tcutil_terr(loc,((_tmpF2C="format attribute and vararg types don't match",
_tag_dyneither(_tmpF2C,sizeof(char),46))),_tag_dyneither(_tmpF2B,sizeof(void*),0)));}}}}{
struct Cyc_List_List*rpo=_tmp7B7;for(0;rpo != 0;rpo=rpo->tl){struct _tuple0 _tmp8AE;
void*_tmp8AF;void*_tmp8B0;struct _tuple0*_tmp8AD=(struct _tuple0*)rpo->hd;_tmp8AE=*
_tmp8AD;_tmp8AF=_tmp8AE.f1;_tmp8B0=_tmp8AE.f2;_tmp87D=Cyc_Tcutil_i_check_valid_type(
loc,te,_tmp87D,(void*)6,_tmp8AF,1);_tmp87D=Cyc_Tcutil_i_check_valid_type(loc,te,
_tmp87D,(void*)5,_tmp8B0,1);}}if(*_tmp7B2 != 0)_tmp87D=Cyc_Tcutil_i_check_valid_type(
loc,te,_tmp87D,(void*)6,(void*)((struct Cyc_Core_Opt*)_check_null(*_tmp7B2))->v,1);
else{struct Cyc_List_List*effect=0;{struct Cyc_List_List*tvs=_tmp87D.free_vars;
for(0;tvs != 0;tvs=tvs->tl){struct Cyc_Absyn_Tvar*_tmp8B2;int _tmp8B3;struct
_tuple23 _tmp8B1=*((struct _tuple23*)tvs->hd);_tmp8B2=_tmp8B1.f1;_tmp8B3=_tmp8B1.f2;
if(!_tmp8B3)continue;{void*_tmp8B4=Cyc_Absyn_compress_kb(_tmp8B2->kind);struct
Cyc_Core_Opt*_tmp8B5;struct Cyc_Core_Opt**_tmp8B6;void*_tmp8B7;struct Cyc_Core_Opt*
_tmp8B8;struct Cyc_Core_Opt**_tmp8B9;void*_tmp8BA;void*_tmp8BB;void*_tmp8BC;void*
_tmp8BD;struct Cyc_Core_Opt*_tmp8BE;struct Cyc_Core_Opt**_tmp8BF;void*_tmp8C0;void*
_tmp8C1;struct Cyc_Core_Opt*_tmp8C2;struct Cyc_Core_Opt**_tmp8C3;_LL569: if(*((int*)
_tmp8B4)!= 2)goto _LL56B;_tmp8B5=((struct Cyc_Absyn_Less_kb_struct*)_tmp8B4)->f1;
_tmp8B6=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)_tmp8B4)->f1;
_tmp8B7=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp8B4)->f2;if((int)_tmp8B7 != 
5)goto _LL56B;_LL56A: _tmp8B9=_tmp8B6;_tmp8BA=(void*)3;goto _LL56C;_LL56B: if(*((int*)
_tmp8B4)!= 2)goto _LL56D;_tmp8B8=((struct Cyc_Absyn_Less_kb_struct*)_tmp8B4)->f1;
_tmp8B9=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)_tmp8B4)->f1;
_tmp8BA=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp8B4)->f2;if(!(_tmp8BA == (
void*)3  || _tmp8BA == (void*)4))goto _LL56D;_LL56C:*_tmp8B9=Cyc_Tcutil_kind_to_bound_opt(
_tmp8BA);_tmp8BB=_tmp8BA;goto _LL56E;_LL56D: if(*((int*)_tmp8B4)!= 0)goto _LL56F;
_tmp8BB=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp8B4)->f1;if(!((_tmp8BB == (
void*)3  || _tmp8BB == (void*)4) || _tmp8BB == (void*)5))goto _LL56F;_LL56E:{struct
Cyc_Absyn_AccessEff_struct*_tmpF3B;struct Cyc_Absyn_VarType_struct*_tmpF3A;struct
Cyc_Absyn_VarType_struct _tmpF39;struct Cyc_Absyn_AccessEff_struct _tmpF38;struct
Cyc_List_List*_tmpF37;effect=((_tmpF37=_cycalloc(sizeof(*_tmpF37)),((_tmpF37->hd=(
void*)((void*)((_tmpF3B=_cycalloc(sizeof(*_tmpF3B)),((_tmpF3B[0]=((_tmpF38.tag=
19,((_tmpF38.f1=(void*)((void*)((_tmpF3A=_cycalloc(sizeof(*_tmpF3A)),((_tmpF3A[0]=((
_tmpF39.tag=1,((_tmpF39.f1=_tmp8B2,_tmpF39)))),_tmpF3A))))),_tmpF38)))),_tmpF3B))))),((
_tmpF37->tl=effect,_tmpF37))))));}goto _LL568;_LL56F: if(*((int*)_tmp8B4)!= 2)goto
_LL571;_tmp8BC=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp8B4)->f2;if((int)
_tmp8BC != 7)goto _LL571;_LL570: goto _LL572;_LL571: if(*((int*)_tmp8B4)!= 0)goto
_LL573;_tmp8BD=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp8B4)->f1;if((int)
_tmp8BD != 7)goto _LL573;_LL572: goto _LL568;_LL573: if(*((int*)_tmp8B4)!= 2)goto
_LL575;_tmp8BE=((struct Cyc_Absyn_Less_kb_struct*)_tmp8B4)->f1;_tmp8BF=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*)_tmp8B4)->f1;_tmp8C0=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp8B4)->f2;if((int)_tmp8C0 != 6)goto _LL575;_LL574:*_tmp8BF=Cyc_Tcutil_kind_to_bound_opt((
void*)6);goto _LL576;_LL575: if(*((int*)_tmp8B4)!= 0)goto _LL577;_tmp8C1=(void*)((
struct Cyc_Absyn_Eq_kb_struct*)_tmp8B4)->f1;if((int)_tmp8C1 != 6)goto _LL577;_LL576:{
struct Cyc_Absyn_VarType_struct*_tmpF41;struct Cyc_Absyn_VarType_struct _tmpF40;
struct Cyc_List_List*_tmpF3F;effect=((_tmpF3F=_cycalloc(sizeof(*_tmpF3F)),((
_tmpF3F->hd=(void*)((void*)((_tmpF41=_cycalloc(sizeof(*_tmpF41)),((_tmpF41[0]=((
_tmpF40.tag=1,((_tmpF40.f1=_tmp8B2,_tmpF40)))),_tmpF41))))),((_tmpF3F->tl=effect,
_tmpF3F))))));}goto _LL568;_LL577: if(*((int*)_tmp8B4)!= 1)goto _LL579;_tmp8C2=((
struct Cyc_Absyn_Unknown_kb_struct*)_tmp8B4)->f1;_tmp8C3=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Unknown_kb_struct*)_tmp8B4)->f1;_LL578:{struct Cyc_Absyn_Less_kb_struct*
_tmpF47;struct Cyc_Absyn_Less_kb_struct _tmpF46;struct Cyc_Core_Opt*_tmpF45;*
_tmp8C3=((_tmpF45=_cycalloc(sizeof(*_tmpF45)),((_tmpF45->v=(void*)((void*)((
_tmpF47=_cycalloc(sizeof(*_tmpF47)),((_tmpF47[0]=((_tmpF46.tag=2,((_tmpF46.f1=0,((
_tmpF46.f2=(void*)((void*)0),_tmpF46)))))),_tmpF47))))),_tmpF45))));}goto _LL57A;
_LL579:;_LL57A:{struct Cyc_Absyn_RgnsEff_struct*_tmpF56;struct Cyc_Absyn_VarType_struct*
_tmpF55;struct Cyc_Absyn_VarType_struct _tmpF54;struct Cyc_Absyn_RgnsEff_struct
_tmpF53;struct Cyc_List_List*_tmpF52;effect=((_tmpF52=_cycalloc(sizeof(*_tmpF52)),((
_tmpF52->hd=(void*)((void*)((_tmpF56=_cycalloc(sizeof(*_tmpF56)),((_tmpF56[0]=((
_tmpF53.tag=21,((_tmpF53.f1=(void*)((void*)((_tmpF55=_cycalloc(sizeof(*_tmpF55)),((
_tmpF55[0]=((_tmpF54.tag=1,((_tmpF54.f1=_tmp8B2,_tmpF54)))),_tmpF55))))),_tmpF53)))),
_tmpF56))))),((_tmpF52->tl=effect,_tmpF52))))));}goto _LL568;_LL568:;}}}{struct
Cyc_List_List*ts=_tmp87D.free_evars;for(0;ts != 0;ts=ts->tl){void*_tmp8D5;int
_tmp8D6;struct _tuple7 _tmp8D4=*((struct _tuple7*)ts->hd);_tmp8D5=_tmp8D4.f1;
_tmp8D6=_tmp8D4.f2;if(!_tmp8D6)continue;{void*_tmp8D7=Cyc_Tcutil_typ_kind(
_tmp8D5);_LL57C: if((int)_tmp8D7 != 5)goto _LL57E;_LL57D: goto _LL57F;_LL57E: if((int)
_tmp8D7 != 4)goto _LL580;_LL57F: goto _LL581;_LL580: if((int)_tmp8D7 != 3)goto _LL582;
_LL581:{struct Cyc_Absyn_AccessEff_struct*_tmpF5C;struct Cyc_Absyn_AccessEff_struct
_tmpF5B;struct Cyc_List_List*_tmpF5A;effect=((_tmpF5A=_cycalloc(sizeof(*_tmpF5A)),((
_tmpF5A->hd=(void*)((void*)((_tmpF5C=_cycalloc(sizeof(*_tmpF5C)),((_tmpF5C[0]=((
_tmpF5B.tag=19,((_tmpF5B.f1=(void*)_tmp8D5,_tmpF5B)))),_tmpF5C))))),((_tmpF5A->tl=
effect,_tmpF5A))))));}goto _LL57B;_LL582: if((int)_tmp8D7 != 6)goto _LL584;_LL583:{
struct Cyc_List_List*_tmpF5D;effect=((_tmpF5D=_cycalloc(sizeof(*_tmpF5D)),((
_tmpF5D->hd=(void*)_tmp8D5,((_tmpF5D->tl=effect,_tmpF5D))))));}goto _LL57B;_LL584:
if((int)_tmp8D7 != 7)goto _LL586;_LL585: goto _LL57B;_LL586:;_LL587:{struct Cyc_Absyn_RgnsEff_struct*
_tmpF63;struct Cyc_Absyn_RgnsEff_struct _tmpF62;struct Cyc_List_List*_tmpF61;effect=((
_tmpF61=_cycalloc(sizeof(*_tmpF61)),((_tmpF61->hd=(void*)((void*)((_tmpF63=
_cycalloc(sizeof(*_tmpF63)),((_tmpF63[0]=((_tmpF62.tag=21,((_tmpF62.f1=(void*)
_tmp8D5,_tmpF62)))),_tmpF63))))),((_tmpF61->tl=effect,_tmpF61))))));}goto _LL57B;
_LL57B:;}}}{struct Cyc_Absyn_JoinEff_struct*_tmpF69;struct Cyc_Absyn_JoinEff_struct
_tmpF68;struct Cyc_Core_Opt*_tmpF67;*_tmp7B2=((_tmpF67=_cycalloc(sizeof(*_tmpF67)),((
_tmpF67->v=(void*)((void*)((_tmpF69=_cycalloc(sizeof(*_tmpF69)),((_tmpF69[0]=((
_tmpF68.tag=20,((_tmpF68.f1=effect,_tmpF68)))),_tmpF69))))),_tmpF67))));}}if(*
_tmp7B0 != 0){struct Cyc_List_List*bs=*_tmp7B0;for(0;bs != 0;bs=bs->tl){void*
_tmp8E2=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)bs->hd)->kind);struct Cyc_Core_Opt*
_tmp8E3;struct Cyc_Core_Opt**_tmp8E4;struct Cyc_Core_Opt*_tmp8E5;struct Cyc_Core_Opt**
_tmp8E6;void*_tmp8E7;struct Cyc_Core_Opt*_tmp8E8;struct Cyc_Core_Opt**_tmp8E9;void*
_tmp8EA;struct Cyc_Core_Opt*_tmp8EB;struct Cyc_Core_Opt**_tmp8EC;void*_tmp8ED;
struct Cyc_Core_Opt*_tmp8EE;struct Cyc_Core_Opt**_tmp8EF;void*_tmp8F0;void*_tmp8F1;
_LL589: if(*((int*)_tmp8E2)!= 1)goto _LL58B;_tmp8E3=((struct Cyc_Absyn_Unknown_kb_struct*)
_tmp8E2)->f1;_tmp8E4=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)
_tmp8E2)->f1;_LL58A:{const char*_tmpF6D;void*_tmpF6C[1];struct Cyc_String_pa_struct
_tmpF6B;(_tmpF6B.tag=0,((_tmpF6B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((
struct Cyc_Absyn_Tvar*)bs->hd)->name),((_tmpF6C[0]=& _tmpF6B,Cyc_Tcutil_warn(loc,((
_tmpF6D="Type variable %s unconstrained, assuming boxed",_tag_dyneither(_tmpF6D,
sizeof(char),47))),_tag_dyneither(_tmpF6C,sizeof(void*),1)))))));}_tmp8E6=
_tmp8E4;goto _LL58C;_LL58B: if(*((int*)_tmp8E2)!= 2)goto _LL58D;_tmp8E5=((struct Cyc_Absyn_Less_kb_struct*)
_tmp8E2)->f1;_tmp8E6=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp8E2)->f1;_tmp8E7=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp8E2)->f2;if((
int)_tmp8E7 != 1)goto _LL58D;_LL58C: _tmp8E9=_tmp8E6;goto _LL58E;_LL58D: if(*((int*)
_tmp8E2)!= 2)goto _LL58F;_tmp8E8=((struct Cyc_Absyn_Less_kb_struct*)_tmp8E2)->f1;
_tmp8E9=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)_tmp8E2)->f1;
_tmp8EA=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp8E2)->f2;if((int)_tmp8EA != 
0)goto _LL58F;_LL58E:*_tmp8E9=Cyc_Tcutil_kind_to_bound_opt((void*)2);goto _LL588;
_LL58F: if(*((int*)_tmp8E2)!= 2)goto _LL591;_tmp8EB=((struct Cyc_Absyn_Less_kb_struct*)
_tmp8E2)->f1;_tmp8EC=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp8E2)->f1;_tmp8ED=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp8E2)->f2;if((
int)_tmp8ED != 5)goto _LL591;_LL590:*_tmp8EC=Cyc_Tcutil_kind_to_bound_opt((void*)3);
goto _LL588;_LL591: if(*((int*)_tmp8E2)!= 2)goto _LL593;_tmp8EE=((struct Cyc_Absyn_Less_kb_struct*)
_tmp8E2)->f1;_tmp8EF=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp8E2)->f1;_tmp8F0=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp8E2)->f2;
_LL592:*_tmp8EF=Cyc_Tcutil_kind_to_bound_opt(_tmp8F0);goto _LL588;_LL593: if(*((
int*)_tmp8E2)!= 0)goto _LL595;_tmp8F1=(void*)((struct Cyc_Absyn_Eq_kb_struct*)
_tmp8E2)->f1;if((int)_tmp8F1 != 1)goto _LL595;_LL594:{const char*_tmpF70;void*
_tmpF6F;(_tmpF6F=0,Cyc_Tcutil_terr(loc,((_tmpF70="functions can't abstract M types",
_tag_dyneither(_tmpF70,sizeof(char),33))),_tag_dyneither(_tmpF6F,sizeof(void*),0)));}
goto _LL588;_LL595:;_LL596: goto _LL588;_LL588:;}}cvtenv.kind_env=Cyc_Tcutil_remove_bound_tvars(
Cyc_Core_heap_region,_tmp87D.kind_env,*_tmp7B0);_tmp87D.free_vars=Cyc_Tcutil_remove_bound_tvars_bool(
_tmp87D.r,_tmp87D.free_vars,*_tmp7B0);{struct Cyc_List_List*tvs=_tmp87D.free_vars;
for(0;tvs != 0;tvs=tvs->tl){struct Cyc_Absyn_Tvar*_tmp8F8;int _tmp8F9;struct
_tuple23 _tmp8F7=*((struct _tuple23*)tvs->hd);_tmp8F8=_tmp8F7.f1;_tmp8F9=_tmp8F7.f2;
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,
_tmp8F8,_tmp8F9);}}{struct Cyc_List_List*evs=_tmp87D.free_evars;for(0;evs != 0;evs=
evs->tl){void*_tmp8FB;int _tmp8FC;struct _tuple7 _tmp8FA=*((struct _tuple7*)evs->hd);
_tmp8FB=_tmp8FA.f1;_tmp8FC=_tmp8FA.f2;cvtenv.free_evars=Cyc_Tcutil_add_free_evar(
cvtenv.r,cvtenv.free_evars,_tmp8FB,_tmp8FC);}}}goto _LL4C8;}}_LL4E7: if(*((int*)
_tmp786)!= 9)goto _LL4E9;_tmp7B9=((struct Cyc_Absyn_TupleType_struct*)_tmp786)->f1;
_LL4E8: for(0;_tmp7B9 != 0;_tmp7B9=_tmp7B9->tl){struct _tuple11*_tmp8FE=(struct
_tuple11*)_tmp7B9->hd;cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)
1,(*_tmp8FE).f2,1);((*_tmp8FE).f1).real_const=Cyc_Tcutil_extract_const_from_typedef(
loc,((*_tmp8FE).f1).print_const,(*_tmp8FE).f2);}goto _LL4C8;_LL4E9: if(*((int*)
_tmp786)!= 11)goto _LL4EB;_tmp7BA=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp786)->f1;_tmp7BB=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp786)->f2;_LL4EA: {
struct _RegionHandle*_tmp8FF=Cyc_Tcenv_get_fnrgn(te);{struct Cyc_List_List*
prev_fields=0;for(0;_tmp7BB != 0;_tmp7BB=_tmp7BB->tl){struct Cyc_Absyn_Aggrfield
_tmp901;struct _dyneither_ptr*_tmp902;struct Cyc_Absyn_Tqual _tmp903;struct Cyc_Absyn_Tqual*
_tmp904;void*_tmp905;struct Cyc_Absyn_Exp*_tmp906;struct Cyc_List_List*_tmp907;
struct Cyc_Absyn_Aggrfield*_tmp900=(struct Cyc_Absyn_Aggrfield*)_tmp7BB->hd;
_tmp901=*_tmp900;_tmp902=_tmp901.name;_tmp903=_tmp901.tq;_tmp904=(struct Cyc_Absyn_Tqual*)&(*
_tmp900).tq;_tmp905=_tmp901.type;_tmp906=_tmp901.width;_tmp907=_tmp901.attributes;
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*
l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,_tmp902)){
const char*_tmpF74;void*_tmpF73[1];struct Cyc_String_pa_struct _tmpF72;(_tmpF72.tag=
0,((_tmpF72.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp902),((_tmpF73[
0]=& _tmpF72,Cyc_Tcutil_terr(loc,((_tmpF74="duplicate field %s",_tag_dyneither(
_tmpF74,sizeof(char),19))),_tag_dyneither(_tmpF73,sizeof(void*),1)))))));}{const
char*_tmpF75;if(Cyc_strcmp((struct _dyneither_ptr)*_tmp902,((_tmpF75="",
_tag_dyneither(_tmpF75,sizeof(char),1))))!= 0){struct Cyc_List_List*_tmpF76;
prev_fields=((_tmpF76=_region_malloc(_tmp8FF,sizeof(*_tmpF76)),((_tmpF76->hd=
_tmp902,((_tmpF76->tl=prev_fields,_tmpF76))))));}}cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,(void*)1,_tmp905,1);_tmp904->real_const=Cyc_Tcutil_extract_const_from_typedef(
loc,_tmp904->print_const,_tmp905);if(_tmp7BA == (void*)1  && !Cyc_Tcutil_bits_only(
_tmp905)){const char*_tmpF7A;void*_tmpF79[1];struct Cyc_String_pa_struct _tmpF78;(
_tmpF78.tag=0,((_tmpF78.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp902),((
_tmpF79[0]=& _tmpF78,Cyc_Tcutil_warn(loc,((_tmpF7A="union member %s is not `bits-only' so it can only be written and not read",
_tag_dyneither(_tmpF7A,sizeof(char),74))),_tag_dyneither(_tmpF79,sizeof(void*),1)))))));}
Cyc_Tcutil_check_bitfield(loc,te,_tmp905,_tmp906,_tmp902);Cyc_Tcutil_check_field_atts(
loc,_tmp902,_tmp907);}}goto _LL4C8;}_LL4EB: if(*((int*)_tmp786)!= 10)goto _LL4ED;
_tmp7BC=((struct Cyc_Absyn_AggrType_struct*)_tmp786)->f1;_tmp7BD=_tmp7BC.aggr_info;
_tmp7BE=(union Cyc_Absyn_AggrInfoU*)&(((struct Cyc_Absyn_AggrType_struct*)_tmp786)->f1).aggr_info;
_tmp7BF=_tmp7BC.targs;_tmp7C0=(struct Cyc_List_List**)&(((struct Cyc_Absyn_AggrType_struct*)
_tmp786)->f1).targs;_LL4EC:{union Cyc_Absyn_AggrInfoU _tmp910=*_tmp7BE;struct
_tuple4 _tmp911;void*_tmp912;struct _tuple2*_tmp913;struct Cyc_Core_Opt*_tmp914;
struct Cyc_Absyn_Aggrdecl**_tmp915;struct Cyc_Absyn_Aggrdecl*_tmp916;_LL598: if((
_tmp910.UnknownAggr).tag != 1)goto _LL59A;_tmp911=(struct _tuple4)(_tmp910.UnknownAggr).val;
_tmp912=_tmp911.f1;_tmp913=_tmp911.f2;_tmp914=_tmp911.f3;_LL599: {struct Cyc_Absyn_Aggrdecl**
adp;{struct _handler_cons _tmp917;_push_handler(& _tmp917);{int _tmp919=0;if(setjmp(
_tmp917.handler))_tmp919=1;if(!_tmp919){adp=Cyc_Tcenv_lookup_aggrdecl(te,loc,
_tmp913);{struct Cyc_Absyn_Aggrdecl*_tmp91A=*adp;if(_tmp91A->kind != _tmp912){if(
_tmp91A->kind == (void*)0){const char*_tmpF7F;void*_tmpF7E[2];struct Cyc_String_pa_struct
_tmpF7D;struct Cyc_String_pa_struct _tmpF7C;(_tmpF7C.tag=0,((_tmpF7C.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp913)),((
_tmpF7D.tag=0,((_tmpF7D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp913)),((_tmpF7E[0]=& _tmpF7D,((_tmpF7E[1]=& _tmpF7C,Cyc_Tcutil_terr(loc,((
_tmpF7F="expecting struct %s instead of union %s",_tag_dyneither(_tmpF7F,sizeof(
char),40))),_tag_dyneither(_tmpF7E,sizeof(void*),2)))))))))))));}else{const char*
_tmpF84;void*_tmpF83[2];struct Cyc_String_pa_struct _tmpF82;struct Cyc_String_pa_struct
_tmpF81;(_tmpF81.tag=0,((_tmpF81.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp913)),((_tmpF82.tag=0,((_tmpF82.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp913)),((
_tmpF83[0]=& _tmpF82,((_tmpF83[1]=& _tmpF81,Cyc_Tcutil_terr(loc,((_tmpF84="expecting union %s instead of struct %s",
_tag_dyneither(_tmpF84,sizeof(char),40))),_tag_dyneither(_tmpF83,sizeof(void*),2)))))))))))));}}
if((unsigned int)_tmp914  && (int)_tmp914->v){if(!((unsigned int)_tmp91A->impl)
 || !((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp91A->impl))->tagged){const
char*_tmpF88;void*_tmpF87[1];struct Cyc_String_pa_struct _tmpF86;(_tmpF86.tag=0,((
_tmpF86.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp913)),((_tmpF87[0]=& _tmpF86,Cyc_Tcutil_terr(loc,((_tmpF88="@tagged qualfiers don't agree on union %s",
_tag_dyneither(_tmpF88,sizeof(char),42))),_tag_dyneither(_tmpF87,sizeof(void*),1)))))));}}*
_tmp7BE=Cyc_Absyn_KnownAggr(adp);};_pop_handler();}else{void*_tmp918=(void*)
_exn_thrown;void*_tmp927=_tmp918;_LL59D: if(_tmp927 != Cyc_Dict_Absent)goto _LL59F;
_LL59E: {struct Cyc_Tcenv_Genv*_tmp928=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict
d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);struct Cyc_Absyn_Aggrdecl*
_tmpF89;struct Cyc_Absyn_Aggrdecl*_tmp929=(_tmpF89=_cycalloc(sizeof(*_tmpF89)),((
_tmpF89->kind=_tmp912,((_tmpF89->sc=(void*)3,((_tmpF89->name=_tmp913,((_tmpF89->tvs=
0,((_tmpF89->impl=0,((_tmpF89->attributes=0,_tmpF89)))))))))))));Cyc_Tc_tcAggrdecl(
te,_tmp928,loc,_tmp929);adp=Cyc_Tcenv_lookup_aggrdecl(te,loc,_tmp913);*_tmp7BE=
Cyc_Absyn_KnownAggr(adp);if(*_tmp7C0 != 0){{const char*_tmpF8D;void*_tmpF8C[1];
struct Cyc_String_pa_struct _tmpF8B;(_tmpF8B.tag=0,((_tmpF8B.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp913)),((
_tmpF8C[0]=& _tmpF8B,Cyc_Tcutil_terr(loc,((_tmpF8D="declare parameterized type %s before using",
_tag_dyneither(_tmpF8D,sizeof(char),43))),_tag_dyneither(_tmpF8C,sizeof(void*),1)))))));}
return cvtenv;}goto _LL59C;}_LL59F:;_LL5A0:(void)_throw(_tmp927);_LL59C:;}}}
_tmp916=*adp;goto _LL59B;}_LL59A: if((_tmp910.KnownAggr).tag != 2)goto _LL597;
_tmp915=(struct Cyc_Absyn_Aggrdecl**)(_tmp910.KnownAggr).val;_tmp916=*_tmp915;
_LL59B: {struct Cyc_List_List*tvs=_tmp916->tvs;struct Cyc_List_List*ts=*_tmp7C0;
for(0;ts != 0  && tvs != 0;(ts=ts->tl,tvs=tvs->tl)){void*k=Cyc_Tcutil_tvar_kind((
struct Cyc_Absyn_Tvar*)tvs->hd);cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,
cvtenv,k,(void*)ts->hd,1);}if(ts != 0){const char*_tmpF91;void*_tmpF90[1];struct
Cyc_String_pa_struct _tmpF8F;(_tmpF8F.tag=0,((_tmpF8F.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp916->name)),((_tmpF90[0]=&
_tmpF8F,Cyc_Tcutil_terr(loc,((_tmpF91="too many parameters for type %s",
_tag_dyneither(_tmpF91,sizeof(char),32))),_tag_dyneither(_tmpF90,sizeof(void*),1)))))));}
if(tvs != 0){struct Cyc_List_List*hidden_ts=0;for(0;tvs != 0;tvs=tvs->tl){void*k=
Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd);void*e=Cyc_Absyn_new_evar(0,
0);{struct Cyc_List_List*_tmpF92;hidden_ts=((_tmpF92=_cycalloc(sizeof(*_tmpF92)),((
_tmpF92->hd=(void*)e,((_tmpF92->tl=hidden_ts,_tmpF92))))));}cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,k,e,1);}*_tmp7C0=Cyc_List_imp_append(*_tmp7C0,Cyc_List_imp_rev(
hidden_ts));}}_LL597:;}goto _LL4C8;_LL4ED: if(*((int*)_tmp786)!= 16)goto _LL4EF;
_tmp7C1=((struct Cyc_Absyn_TypedefType_struct*)_tmp786)->f1;_tmp7C2=((struct Cyc_Absyn_TypedefType_struct*)
_tmp786)->f2;_tmp7C3=(struct Cyc_List_List**)&((struct Cyc_Absyn_TypedefType_struct*)
_tmp786)->f2;_tmp7C4=((struct Cyc_Absyn_TypedefType_struct*)_tmp786)->f3;_tmp7C5=(
struct Cyc_Absyn_Typedefdecl**)&((struct Cyc_Absyn_TypedefType_struct*)_tmp786)->f3;
_tmp7C6=((struct Cyc_Absyn_TypedefType_struct*)_tmp786)->f4;_tmp7C7=(void***)&((
struct Cyc_Absyn_TypedefType_struct*)_tmp786)->f4;_LL4EE: {struct Cyc_List_List*
targs=*_tmp7C3;struct Cyc_Absyn_Typedefdecl*td;{struct _handler_cons _tmp932;
_push_handler(& _tmp932);{int _tmp934=0;if(setjmp(_tmp932.handler))_tmp934=1;if(!
_tmp934){td=Cyc_Tcenv_lookup_typedefdecl(te,loc,_tmp7C1);;_pop_handler();}else{
void*_tmp933=(void*)_exn_thrown;void*_tmp936=_tmp933;_LL5A2: if(_tmp936 != Cyc_Dict_Absent)
goto _LL5A4;_LL5A3:{const char*_tmpF96;void*_tmpF95[1];struct Cyc_String_pa_struct
_tmpF94;(_tmpF94.tag=0,((_tmpF94.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp7C1)),((_tmpF95[0]=& _tmpF94,Cyc_Tcutil_terr(loc,((
_tmpF96="unbound typedef name %s",_tag_dyneither(_tmpF96,sizeof(char),24))),
_tag_dyneither(_tmpF95,sizeof(void*),1)))))));}return cvtenv;_LL5A4:;_LL5A5:(void)
_throw(_tmp936);_LL5A1:;}}}*_tmp7C5=(struct Cyc_Absyn_Typedefdecl*)td;_tmp7C1[0]=(
td->name)[_check_known_subscript_notnull(1,0)];{struct Cyc_List_List*tvs=td->tvs;
struct Cyc_List_List*ts=targs;struct _RegionHandle*_tmp93A=Cyc_Tcenv_get_fnrgn(te);{
struct Cyc_List_List*inst=0;for(0;ts != 0  && tvs != 0;(ts=ts->tl,tvs=tvs->tl)){void*
k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd);cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,k,(void*)ts->hd,1);{struct _tuple14*_tmpF99;struct Cyc_List_List*
_tmpF98;inst=((_tmpF98=_region_malloc(_tmp93A,sizeof(*_tmpF98)),((_tmpF98->hd=((
_tmpF99=_region_malloc(_tmp93A,sizeof(*_tmpF99)),((_tmpF99->f1=(struct Cyc_Absyn_Tvar*)
tvs->hd,((_tmpF99->f2=(void*)ts->hd,_tmpF99)))))),((_tmpF98->tl=inst,_tmpF98))))));}}
if(ts != 0){const char*_tmpF9D;void*_tmpF9C[1];struct Cyc_String_pa_struct _tmpF9B;(
_tmpF9B.tag=0,((_tmpF9B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp7C1)),((_tmpF9C[0]=& _tmpF9B,Cyc_Tcutil_terr(loc,((_tmpF9D="too many parameters for typedef %s",
_tag_dyneither(_tmpF9D,sizeof(char),35))),_tag_dyneither(_tmpF9C,sizeof(void*),1)))))));}
if(tvs != 0){struct Cyc_List_List*hidden_ts=0;for(0;tvs != 0;tvs=tvs->tl){void*k=
Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd);void*e=Cyc_Absyn_new_evar(0,
0);{struct Cyc_List_List*_tmpF9E;hidden_ts=((_tmpF9E=_cycalloc(sizeof(*_tmpF9E)),((
_tmpF9E->hd=(void*)e,((_tmpF9E->tl=hidden_ts,_tmpF9E))))));}cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,k,e,1);{struct _tuple14*_tmpFA1;struct Cyc_List_List*_tmpFA0;inst=(
struct Cyc_List_List*)((_tmpFA0=_cycalloc(sizeof(*_tmpFA0)),((_tmpFA0->hd=(struct
_tuple14*)((_tmpFA1=_cycalloc(sizeof(*_tmpFA1)),((_tmpFA1->f1=(struct Cyc_Absyn_Tvar*)
tvs->hd,((_tmpFA1->f2=e,_tmpFA1)))))),((_tmpFA0->tl=inst,_tmpFA0))))));}}*
_tmp7C3=Cyc_List_imp_append(targs,Cyc_List_imp_rev(hidden_ts));}if(td->defn != 0){
void*new_typ=Cyc_Tcutil_rsubstitute(_tmp93A,inst,(void*)((struct Cyc_Core_Opt*)
_check_null(td->defn))->v);void**_tmpFA2;*_tmp7C7=((_tmpFA2=_cycalloc(sizeof(*
_tmpFA2)),((_tmpFA2[0]=new_typ,_tmpFA2))));}}goto _LL4C8;}}_LL4EF: if((int)_tmp786
!= 3)goto _LL4F1;_LL4F0: goto _LL4F2;_LL4F1: if((int)_tmp786 != 2)goto _LL4F3;_LL4F2:
goto _LL4C8;_LL4F3: if(_tmp786 <= (void*)4)goto _LL4F5;if(*((int*)_tmp786)!= 14)goto
_LL4F5;_tmp7C8=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)_tmp786)->f1;
_LL4F4: cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)5,_tmp7C8,1);
goto _LL4C8;_LL4F5: if(_tmp786 <= (void*)4)goto _LL4F7;if(*((int*)_tmp786)!= 15)goto
_LL4F7;_tmp7C9=(void*)((struct Cyc_Absyn_DynRgnType_struct*)_tmp786)->f1;_tmp7CA=(
void*)((struct Cyc_Absyn_DynRgnType_struct*)_tmp786)->f2;_LL4F6: cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,(void*)3,_tmp7C9,0);cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,
cvtenv,(void*)3,_tmp7CA,1);goto _LL4C8;_LL4F7: if(_tmp786 <= (void*)4)goto _LL4F9;
if(*((int*)_tmp786)!= 19)goto _LL4F9;_tmp7CB=(void*)((struct Cyc_Absyn_AccessEff_struct*)
_tmp786)->f1;_LL4F8: cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)5,
_tmp7CB,1);goto _LL4C8;_LL4F9: if(_tmp786 <= (void*)4)goto _LL4FB;if(*((int*)_tmp786)
!= 21)goto _LL4FB;_tmp7CC=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp786)->f1;
_LL4FA: cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)0,_tmp7CC,1);
goto _LL4C8;_LL4FB: if(_tmp786 <= (void*)4)goto _LL4C8;if(*((int*)_tmp786)!= 20)goto
_LL4C8;_tmp7CD=((struct Cyc_Absyn_JoinEff_struct*)_tmp786)->f1;_LL4FC: for(0;
_tmp7CD != 0;_tmp7CD=_tmp7CD->tl){cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,
cvtenv,(void*)6,(void*)_tmp7CD->hd,1);}goto _LL4C8;_LL4C8:;}if(!Cyc_Tcutil_kind_leq(
Cyc_Tcutil_typ_kind(t),expected_kind)){{void*_tmp944=t;struct Cyc_Core_Opt*
_tmp945;struct Cyc_Core_Opt*_tmp946;_LL5A7: if(_tmp944 <= (void*)4)goto _LL5A9;if(*((
int*)_tmp944)!= 0)goto _LL5A9;_tmp945=((struct Cyc_Absyn_Evar_struct*)_tmp944)->f1;
_tmp946=((struct Cyc_Absyn_Evar_struct*)_tmp944)->f2;_LL5A8: {struct
_dyneither_ptr s;{struct Cyc_Core_Opt*_tmp947=_tmp945;struct Cyc_Core_Opt _tmp948;
void*_tmp949;_LL5AC: if(_tmp947 != 0)goto _LL5AE;_LL5AD:{const char*_tmpFA3;s=((
_tmpFA3="kind=NULL ",_tag_dyneither(_tmpFA3,sizeof(char),11)));}goto _LL5AB;
_LL5AE: if(_tmp947 == 0)goto _LL5AB;_tmp948=*_tmp947;_tmp949=(void*)_tmp948.v;
_LL5AF:{const char*_tmpFA7;void*_tmpFA6[1];struct Cyc_String_pa_struct _tmpFA5;s=(
struct _dyneither_ptr)((_tmpFA5.tag=0,((_tmpFA5.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_kind2string(_tmp949)),((_tmpFA6[0]=& _tmpFA5,Cyc_aprintf(((
_tmpFA7="kind=%s ",_tag_dyneither(_tmpFA7,sizeof(char),9))),_tag_dyneither(
_tmpFA6,sizeof(void*),1))))))));}goto _LL5AB;_LL5AB:;}{struct Cyc_Core_Opt*_tmp94E=
_tmp946;struct Cyc_Core_Opt _tmp94F;void*_tmp950;_LL5B1: if(_tmp94E != 0)goto _LL5B3;
_LL5B2:{const char*_tmpFAB;void*_tmpFAA[1];struct Cyc_String_pa_struct _tmpFA9;s=(
struct _dyneither_ptr)((_tmpFA9.tag=0,((_tmpFA9.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)s),((_tmpFAA[0]=& _tmpFA9,Cyc_aprintf(((_tmpFAB="%s ref=NULL",
_tag_dyneither(_tmpFAB,sizeof(char),12))),_tag_dyneither(_tmpFAA,sizeof(void*),1))))))));}
goto _LL5B0;_LL5B3: if(_tmp94E == 0)goto _LL5B0;_tmp94F=*_tmp94E;_tmp950=(void*)
_tmp94F.v;_LL5B4:{const char*_tmpFB0;void*_tmpFAF[2];struct Cyc_String_pa_struct
_tmpFAE;struct Cyc_String_pa_struct _tmpFAD;s=(struct _dyneither_ptr)((_tmpFAD.tag=
0,((_tmpFAD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
_tmp950)),((_tmpFAE.tag=0,((_tmpFAE.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)s),((_tmpFAF[0]=& _tmpFAE,((_tmpFAF[1]=& _tmpFAD,Cyc_aprintf(((
_tmpFB0="%s ref=%s",_tag_dyneither(_tmpFB0,sizeof(char),10))),_tag_dyneither(
_tmpFAF,sizeof(void*),2))))))))))))));}goto _LL5B0;_LL5B0:;}{const char*_tmpFB4;
void*_tmpFB3[1];struct Cyc_String_pa_struct _tmpFB2;(_tmpFB2.tag=0,((_tmpFB2.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmpFB3[0]=& _tmpFB2,Cyc_fprintf(
Cyc_stderr,((_tmpFB4="evar info: %s\n",_tag_dyneither(_tmpFB4,sizeof(char),15))),
_tag_dyneither(_tmpFB3,sizeof(void*),1)))))));}goto _LL5A6;}_LL5A9:;_LL5AA: goto
_LL5A6;_LL5A6:;}{const char*_tmpFBA;void*_tmpFB9[3];struct Cyc_String_pa_struct
_tmpFB8;struct Cyc_String_pa_struct _tmpFB7;struct Cyc_String_pa_struct _tmpFB6;(
_tmpFB6.tag=0,((_tmpFB6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(
expected_kind)),((_tmpFB7.tag=0,((_tmpFB7.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_kind2string(Cyc_Tcutil_typ_kind(t))),((_tmpFB8.tag=0,((
_tmpFB8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((
_tmpFB9[0]=& _tmpFB8,((_tmpFB9[1]=& _tmpFB7,((_tmpFB9[2]=& _tmpFB6,Cyc_Tcutil_terr(
loc,((_tmpFBA="type %s has kind %s but as used here needs kind %s",
_tag_dyneither(_tmpFBA,sizeof(char),51))),_tag_dyneither(_tmpFB9,sizeof(void*),3)))))))))))))))))));}}
return cvtenv;}static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type_level_exp(
struct Cyc_Absyn_Exp*e,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv);
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type_level_exp(struct Cyc_Absyn_Exp*
e,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv){{void*_tmp960=e->r;
struct Cyc_List_List*_tmp961;struct Cyc_Absyn_Exp*_tmp962;struct Cyc_Absyn_Exp*
_tmp963;struct Cyc_Absyn_Exp*_tmp964;struct Cyc_Absyn_Exp*_tmp965;struct Cyc_Absyn_Exp*
_tmp966;struct Cyc_Absyn_Exp*_tmp967;struct Cyc_Absyn_Exp*_tmp968;struct Cyc_Absyn_Exp*
_tmp969;struct Cyc_Absyn_Exp*_tmp96A;void*_tmp96B;struct Cyc_Absyn_Exp*_tmp96C;
void*_tmp96D;void*_tmp96E;void*_tmp96F;struct Cyc_Absyn_Exp*_tmp970;_LL5B6: if(*((
int*)_tmp960)!= 0)goto _LL5B8;_LL5B7: goto _LL5B9;_LL5B8: if(*((int*)_tmp960)!= 33)
goto _LL5BA;_LL5B9: goto _LL5BB;_LL5BA: if(*((int*)_tmp960)!= 34)goto _LL5BC;_LL5BB:
goto _LL5BD;_LL5BC: if(*((int*)_tmp960)!= 1)goto _LL5BE;_LL5BD: goto _LL5B5;_LL5BE:
if(*((int*)_tmp960)!= 3)goto _LL5C0;_tmp961=((struct Cyc_Absyn_Primop_e_struct*)
_tmp960)->f2;_LL5BF: for(0;_tmp961 != 0;_tmp961=_tmp961->tl){cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp((
struct Cyc_Absyn_Exp*)_tmp961->hd,te,cvtenv);}goto _LL5B5;_LL5C0: if(*((int*)
_tmp960)!= 6)goto _LL5C2;_tmp962=((struct Cyc_Absyn_Conditional_e_struct*)_tmp960)->f1;
_tmp963=((struct Cyc_Absyn_Conditional_e_struct*)_tmp960)->f2;_tmp964=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp960)->f3;_LL5C1: cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp962,te,
cvtenv);cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp963,te,cvtenv);cvtenv=
Cyc_Tcutil_i_check_valid_type_level_exp(_tmp964,te,cvtenv);goto _LL5B5;_LL5C2: if(*((
int*)_tmp960)!= 7)goto _LL5C4;_tmp965=((struct Cyc_Absyn_And_e_struct*)_tmp960)->f1;
_tmp966=((struct Cyc_Absyn_And_e_struct*)_tmp960)->f2;_LL5C3: _tmp967=_tmp965;
_tmp968=_tmp966;goto _LL5C5;_LL5C4: if(*((int*)_tmp960)!= 8)goto _LL5C6;_tmp967=((
struct Cyc_Absyn_Or_e_struct*)_tmp960)->f1;_tmp968=((struct Cyc_Absyn_Or_e_struct*)
_tmp960)->f2;_LL5C5: _tmp969=_tmp967;_tmp96A=_tmp968;goto _LL5C7;_LL5C6: if(*((int*)
_tmp960)!= 9)goto _LL5C8;_tmp969=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp960)->f1;
_tmp96A=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp960)->f2;_LL5C7: cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(
_tmp969,te,cvtenv);cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp96A,te,
cvtenv);goto _LL5B5;_LL5C8: if(*((int*)_tmp960)!= 15)goto _LL5CA;_tmp96B=(void*)((
struct Cyc_Absyn_Cast_e_struct*)_tmp960)->f1;_tmp96C=((struct Cyc_Absyn_Cast_e_struct*)
_tmp960)->f2;_LL5C9: cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp96C,te,
cvtenv);cvtenv=Cyc_Tcutil_i_check_valid_type(e->loc,te,cvtenv,(void*)0,_tmp96B,1);
goto _LL5B5;_LL5CA: if(*((int*)_tmp960)!= 20)goto _LL5CC;_tmp96D=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)
_tmp960)->f1;_LL5CB: _tmp96E=_tmp96D;goto _LL5CD;_LL5CC: if(*((int*)_tmp960)!= 18)
goto _LL5CE;_tmp96E=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp960)->f1;
_LL5CD: cvtenv=Cyc_Tcutil_i_check_valid_type(e->loc,te,cvtenv,(void*)0,_tmp96E,1);
goto _LL5B5;_LL5CE: if(*((int*)_tmp960)!= 40)goto _LL5D0;_tmp96F=(void*)((struct Cyc_Absyn_Valueof_e_struct*)
_tmp960)->f1;_LL5CF: cvtenv=Cyc_Tcutil_i_check_valid_type(e->loc,te,cvtenv,(void*)
7,_tmp96F,1);goto _LL5B5;_LL5D0: if(*((int*)_tmp960)!= 19)goto _LL5D2;_tmp970=((
struct Cyc_Absyn_Sizeofexp_e_struct*)_tmp960)->f1;_LL5D1: cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(
_tmp970,te,cvtenv);goto _LL5B5;_LL5D2:;_LL5D3: {const char*_tmpFBD;void*_tmpFBC;(
_tmpFBC=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmpFBD="non-type-level-expression in Tcutil::i_check_valid_type_level_exp",
_tag_dyneither(_tmpFBD,sizeof(char),66))),_tag_dyneither(_tmpFBC,sizeof(void*),0)));}
_LL5B5:;}return cvtenv;}static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_check_valid_type(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv
cvt,void*expected_kind,void*t);static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_check_valid_type(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv
cvt,void*expected_kind,void*t){struct Cyc_List_List*_tmp973=cvt.kind_env;cvt=Cyc_Tcutil_i_check_valid_type(
loc,te,cvt,expected_kind,t,1);{struct Cyc_List_List*vs=cvt.free_vars;for(0;vs != 0;
vs=vs->tl){struct Cyc_Absyn_Tvar*_tmp975;struct _tuple23 _tmp974=*((struct _tuple23*)
vs->hd);_tmp975=_tmp974.f1;cvt.kind_env=Cyc_Tcutil_fast_add_free_tvar(_tmp973,
_tmp975);}}{struct Cyc_List_List*evs=cvt.free_evars;for(0;evs != 0;evs=evs->tl){
void*_tmp977;struct _tuple7 _tmp976=*((struct _tuple7*)evs->hd);_tmp977=_tmp976.f1;{
void*_tmp978=Cyc_Tcutil_compress(_tmp977);struct Cyc_Core_Opt*_tmp979;struct Cyc_Core_Opt**
_tmp97A;_LL5D5: if(_tmp978 <= (void*)4)goto _LL5D7;if(*((int*)_tmp978)!= 0)goto
_LL5D7;_tmp979=((struct Cyc_Absyn_Evar_struct*)_tmp978)->f4;_tmp97A=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Evar_struct*)_tmp978)->f4;_LL5D6: if(*_tmp97A == 0){struct Cyc_Core_Opt*
_tmpFBE;*_tmp97A=((_tmpFBE=_cycalloc(sizeof(*_tmpFBE)),((_tmpFBE->v=_tmp973,
_tmpFBE))));}else{struct Cyc_List_List*_tmp97C=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(*_tmp97A))->v;struct Cyc_List_List*_tmp97D=0;for(0;_tmp97C != 0;
_tmp97C=_tmp97C->tl){if(((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,
_tmp973,(struct Cyc_Absyn_Tvar*)_tmp97C->hd)){struct Cyc_List_List*_tmpFBF;_tmp97D=((
_tmpFBF=_cycalloc(sizeof(*_tmpFBF)),((_tmpFBF->hd=(struct Cyc_Absyn_Tvar*)_tmp97C->hd,((
_tmpFBF->tl=_tmp97D,_tmpFBF))))));}}{struct Cyc_Core_Opt*_tmpFC0;*_tmp97A=((
_tmpFC0=_cycalloc(sizeof(*_tmpFC0)),((_tmpFC0->v=_tmp97D,_tmpFC0))));}}goto
_LL5D4;_LL5D7:;_LL5D8: goto _LL5D4;_LL5D4:;}}}return cvt;}void Cyc_Tcutil_check_valid_toplevel_type(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,void*t);void Cyc_Tcutil_check_valid_toplevel_type(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,void*t){int
generalize_evars=Cyc_Tcutil_is_function_type(t);struct Cyc_List_List*_tmp980=Cyc_Tcenv_lookup_type_vars(
te);struct _RegionHandle*_tmp981=Cyc_Tcenv_get_fnrgn(te);struct Cyc_Tcutil_CVTEnv
_tmpFC1;struct Cyc_Tcutil_CVTEnv _tmp982=Cyc_Tcutil_check_valid_type(loc,te,((
_tmpFC1.r=_tmp981,((_tmpFC1.kind_env=_tmp980,((_tmpFC1.free_vars=0,((_tmpFC1.free_evars=
0,((_tmpFC1.generalize_evars=generalize_evars,((_tmpFC1.fn_result=0,_tmpFC1)))))))))))),(
void*)1,t);struct Cyc_List_List*_tmp983=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct Cyc_Absyn_Tvar*(*f)(struct _tuple23*),struct Cyc_List_List*x))
Cyc_List_rmap)(_tmp981,(struct Cyc_Absyn_Tvar*(*)(struct _tuple23*))Cyc_Core_fst,
_tmp982.free_vars);struct Cyc_List_List*_tmp984=((struct Cyc_List_List*(*)(struct
_RegionHandle*,void*(*f)(struct _tuple7*),struct Cyc_List_List*x))Cyc_List_rmap)(
_tmp981,(void*(*)(struct _tuple7*))Cyc_Core_fst,_tmp982.free_evars);if(_tmp980 != 
0){struct Cyc_List_List*_tmp985=0;{struct Cyc_List_List*_tmp986=_tmp983;for(0;(
unsigned int)_tmp986;_tmp986=_tmp986->tl){struct Cyc_Absyn_Tvar*_tmp987=(struct
Cyc_Absyn_Tvar*)_tmp986->hd;int found=0;{struct Cyc_List_List*_tmp988=_tmp980;for(
0;(unsigned int)_tmp988;_tmp988=_tmp988->tl){if(Cyc_Absyn_tvar_cmp(_tmp987,(
struct Cyc_Absyn_Tvar*)_tmp988->hd)== 0){found=1;break;}}}if(!found){struct Cyc_List_List*
_tmpFC2;_tmp985=((_tmpFC2=_region_malloc(_tmp981,sizeof(*_tmpFC2)),((_tmpFC2->hd=(
struct Cyc_Absyn_Tvar*)_tmp986->hd,((_tmpFC2->tl=_tmp985,_tmpFC2))))));}}}_tmp983=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp985);}{
struct Cyc_List_List*x=_tmp983;for(0;x != 0;x=x->tl){void*_tmp98A=Cyc_Absyn_compress_kb(((
struct Cyc_Absyn_Tvar*)x->hd)->kind);struct Cyc_Core_Opt*_tmp98B;struct Cyc_Core_Opt**
_tmp98C;struct Cyc_Core_Opt*_tmp98D;struct Cyc_Core_Opt**_tmp98E;void*_tmp98F;
struct Cyc_Core_Opt*_tmp990;struct Cyc_Core_Opt**_tmp991;void*_tmp992;struct Cyc_Core_Opt*
_tmp993;struct Cyc_Core_Opt**_tmp994;void*_tmp995;void*_tmp996;_LL5DA: if(*((int*)
_tmp98A)!= 1)goto _LL5DC;_tmp98B=((struct Cyc_Absyn_Unknown_kb_struct*)_tmp98A)->f1;
_tmp98C=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)_tmp98A)->f1;
_LL5DB: _tmp98E=_tmp98C;goto _LL5DD;_LL5DC: if(*((int*)_tmp98A)!= 2)goto _LL5DE;
_tmp98D=((struct Cyc_Absyn_Less_kb_struct*)_tmp98A)->f1;_tmp98E=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*)_tmp98A)->f1;_tmp98F=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp98A)->f2;if((int)_tmp98F != 1)goto _LL5DE;_LL5DD:*_tmp98E=Cyc_Tcutil_kind_to_bound_opt((
void*)2);goto _LL5D9;_LL5DE: if(*((int*)_tmp98A)!= 2)goto _LL5E0;_tmp990=((struct
Cyc_Absyn_Less_kb_struct*)_tmp98A)->f1;_tmp991=(struct Cyc_Core_Opt**)&((struct
Cyc_Absyn_Less_kb_struct*)_tmp98A)->f1;_tmp992=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp98A)->f2;if((int)_tmp992 != 5)goto _LL5E0;_LL5DF:*_tmp991=Cyc_Tcutil_kind_to_bound_opt((
void*)3);goto _LL5D9;_LL5E0: if(*((int*)_tmp98A)!= 2)goto _LL5E2;_tmp993=((struct
Cyc_Absyn_Less_kb_struct*)_tmp98A)->f1;_tmp994=(struct Cyc_Core_Opt**)&((struct
Cyc_Absyn_Less_kb_struct*)_tmp98A)->f1;_tmp995=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp98A)->f2;_LL5E1:*_tmp994=Cyc_Tcutil_kind_to_bound_opt(_tmp995);goto _LL5D9;
_LL5E2: if(*((int*)_tmp98A)!= 0)goto _LL5E4;_tmp996=(void*)((struct Cyc_Absyn_Eq_kb_struct*)
_tmp98A)->f1;if((int)_tmp996 != 1)goto _LL5E4;_LL5E3:{const char*_tmpFC6;void*
_tmpFC5[1];struct Cyc_String_pa_struct _tmpFC4;(_tmpFC4.tag=0,((_tmpFC4.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_tvar2string((struct Cyc_Absyn_Tvar*)
x->hd)),((_tmpFC5[0]=& _tmpFC4,Cyc_Tcutil_terr(loc,((_tmpFC6="type variable %s cannot have kind M",
_tag_dyneither(_tmpFC6,sizeof(char),36))),_tag_dyneither(_tmpFC5,sizeof(void*),1)))))));}
goto _LL5D9;_LL5E4:;_LL5E5: goto _LL5D9;_LL5D9:;}}if(_tmp983 != 0  || _tmp984 != 0){{
void*_tmp99A=Cyc_Tcutil_compress(t);struct Cyc_Absyn_FnInfo _tmp99B;struct Cyc_List_List*
_tmp99C;struct Cyc_List_List**_tmp99D;struct Cyc_Core_Opt*_tmp99E;void*_tmp99F;
struct Cyc_List_List*_tmp9A0;int _tmp9A1;struct Cyc_Absyn_VarargInfo*_tmp9A2;struct
Cyc_List_List*_tmp9A3;struct Cyc_List_List*_tmp9A4;_LL5E7: if(_tmp99A <= (void*)4)
goto _LL5E9;if(*((int*)_tmp99A)!= 8)goto _LL5E9;_tmp99B=((struct Cyc_Absyn_FnType_struct*)
_tmp99A)->f1;_tmp99C=_tmp99B.tvars;_tmp99D=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_struct*)
_tmp99A)->f1).tvars;_tmp99E=_tmp99B.effect;_tmp99F=_tmp99B.ret_typ;_tmp9A0=
_tmp99B.args;_tmp9A1=_tmp99B.c_varargs;_tmp9A2=_tmp99B.cyc_varargs;_tmp9A3=
_tmp99B.rgn_po;_tmp9A4=_tmp99B.attributes;_LL5E8: if(*_tmp99D == 0){*_tmp99D=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_copy)(_tmp983);_tmp983=0;}
goto _LL5E6;_LL5E9:;_LL5EA: goto _LL5E6;_LL5E6:;}if(_tmp983 != 0){const char*_tmpFCA;
void*_tmpFC9[1];struct Cyc_String_pa_struct _tmpFC8;(_tmpFC8.tag=0,((_tmpFC8.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)_tmp983->hd)->name),((
_tmpFC9[0]=& _tmpFC8,Cyc_Tcutil_terr(loc,((_tmpFCA="unbound type variable %s",
_tag_dyneither(_tmpFCA,sizeof(char),25))),_tag_dyneither(_tmpFC9,sizeof(void*),1)))))));}
if(_tmp984 != 0)for(0;_tmp984 != 0;_tmp984=_tmp984->tl){void*e=(void*)_tmp984->hd;
void*_tmp9A8=Cyc_Tcutil_typ_kind(e);_LL5EC: if((int)_tmp9A8 != 4)goto _LL5EE;_LL5ED:
if(!Cyc_Tcutil_unify(e,(void*)3)){const char*_tmpFCD;void*_tmpFCC;(_tmpFCC=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmpFCD="can't unify evar with unique region!",_tag_dyneither(_tmpFCD,sizeof(
char),37))),_tag_dyneither(_tmpFCC,sizeof(void*),0)));}goto _LL5EB;_LL5EE: if((int)
_tmp9A8 != 5)goto _LL5F0;_LL5EF: goto _LL5F1;_LL5F0: if((int)_tmp9A8 != 3)goto _LL5F2;
_LL5F1: if(!Cyc_Tcutil_unify(e,(void*)2)){const char*_tmpFD0;void*_tmpFCF;(_tmpFCF=
0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmpFD0="can't unify evar with heap!",_tag_dyneither(_tmpFD0,sizeof(char),28))),
_tag_dyneither(_tmpFCF,sizeof(void*),0)));}goto _LL5EB;_LL5F2: if((int)_tmp9A8 != 6)
goto _LL5F4;_LL5F3: if(!Cyc_Tcutil_unify(e,Cyc_Absyn_empty_effect)){const char*
_tmpFD3;void*_tmpFD2;(_tmpFD2=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpFD3="can't unify evar with {}!",
_tag_dyneither(_tmpFD3,sizeof(char),26))),_tag_dyneither(_tmpFD2,sizeof(void*),0)));}
goto _LL5EB;_LL5F4:;_LL5F5:{const char*_tmpFD8;void*_tmpFD7[2];struct Cyc_String_pa_struct
_tmpFD6;struct Cyc_String_pa_struct _tmpFD5;(_tmpFD5.tag=0,((_tmpFD5.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpFD6.tag=0,((
_tmpFD6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(e)),((
_tmpFD7[0]=& _tmpFD6,((_tmpFD7[1]=& _tmpFD5,Cyc_Tcutil_terr(loc,((_tmpFD8="hidden type variable %s isn't abstracted in type %s",
_tag_dyneither(_tmpFD8,sizeof(char),52))),_tag_dyneither(_tmpFD7,sizeof(void*),2)))))))))))));}
goto _LL5EB;_LL5EB:;}}}void Cyc_Tcutil_check_fndecl_valid_type(struct Cyc_Position_Segment*
loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Fndecl*fd);void Cyc_Tcutil_check_fndecl_valid_type(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Fndecl*fd){
void*t=Cyc_Tcutil_fndecl2typ(fd);Cyc_Tcutil_check_valid_toplevel_type(loc,te,t);{
void*_tmp9B4=Cyc_Tcutil_compress(t);struct Cyc_Absyn_FnInfo _tmp9B5;struct Cyc_List_List*
_tmp9B6;struct Cyc_Core_Opt*_tmp9B7;void*_tmp9B8;_LL5F7: if(_tmp9B4 <= (void*)4)
goto _LL5F9;if(*((int*)_tmp9B4)!= 8)goto _LL5F9;_tmp9B5=((struct Cyc_Absyn_FnType_struct*)
_tmp9B4)->f1;_tmp9B6=_tmp9B5.tvars;_tmp9B7=_tmp9B5.effect;_tmp9B8=_tmp9B5.ret_typ;
_LL5F8: fd->tvs=_tmp9B6;fd->effect=_tmp9B7;goto _LL5F6;_LL5F9:;_LL5FA: {const char*
_tmpFDB;void*_tmpFDA;(_tmpFDA=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpFDB="check_fndecl_valid_type: not a FnType",
_tag_dyneither(_tmpFDB,sizeof(char),38))),_tag_dyneither(_tmpFDA,sizeof(void*),0)));}
_LL5F6:;}{struct _RegionHandle*_tmp9BB=Cyc_Tcenv_get_fnrgn(te);{const char*_tmpFDC;
Cyc_Tcutil_check_unique_vars(((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct _dyneither_ptr*(*f)(struct _tuple19*),struct Cyc_List_List*x))Cyc_List_rmap)(
_tmp9BB,(struct _dyneither_ptr*(*)(struct _tuple19*t))Cyc_Tcutil_fst_fdarg,fd->args),
loc,((_tmpFDC="function declaration has repeated parameter",_tag_dyneither(
_tmpFDC,sizeof(char),44))));}{struct Cyc_Core_Opt*_tmpFDD;fd->cached_typ=((
_tmpFDD=_cycalloc(sizeof(*_tmpFDD)),((_tmpFDD->v=(void*)t,_tmpFDD))));}}}void Cyc_Tcutil_check_type(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*
bound_tvars,void*expected_kind,int allow_evars,void*t);void Cyc_Tcutil_check_type(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*
bound_tvars,void*expected_kind,int allow_evars,void*t){struct _RegionHandle*
_tmp9BE=Cyc_Tcenv_get_fnrgn(te);struct Cyc_Tcutil_CVTEnv _tmpFDE;struct Cyc_Tcutil_CVTEnv
_tmp9BF=Cyc_Tcutil_check_valid_type(loc,te,((_tmpFDE.r=_tmp9BE,((_tmpFDE.kind_env=
bound_tvars,((_tmpFDE.free_vars=0,((_tmpFDE.free_evars=0,((_tmpFDE.generalize_evars=
0,((_tmpFDE.fn_result=0,_tmpFDE)))))))))))),expected_kind,t);struct Cyc_List_List*
_tmp9C0=Cyc_Tcutil_remove_bound_tvars(_tmp9BE,((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct Cyc_Absyn_Tvar*(*f)(struct _tuple23*),struct Cyc_List_List*x))
Cyc_List_rmap)(_tmp9BE,(struct Cyc_Absyn_Tvar*(*)(struct _tuple23*))Cyc_Core_fst,
_tmp9BF.free_vars),bound_tvars);struct Cyc_List_List*_tmp9C1=((struct Cyc_List_List*(*)(
struct _RegionHandle*,void*(*f)(struct _tuple7*),struct Cyc_List_List*x))Cyc_List_rmap)(
_tmp9BE,(void*(*)(struct _tuple7*))Cyc_Core_fst,_tmp9BF.free_evars);{struct Cyc_List_List*
fs=_tmp9C0;for(0;fs != 0;fs=fs->tl){struct _dyneither_ptr*_tmp9C2=((struct Cyc_Absyn_Tvar*)
fs->hd)->name;const char*_tmpFE3;void*_tmpFE2[2];struct Cyc_String_pa_struct
_tmpFE1;struct Cyc_String_pa_struct _tmpFE0;(_tmpFE0.tag=0,((_tmpFE0.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpFE1.tag=0,((
_tmpFE1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp9C2),((_tmpFE2[0]=&
_tmpFE1,((_tmpFE2[1]=& _tmpFE0,Cyc_Tcutil_terr(loc,((_tmpFE3="unbound type variable %s in type %s",
_tag_dyneither(_tmpFE3,sizeof(char),36))),_tag_dyneither(_tmpFE2,sizeof(void*),2)))))))))))));}}
if(!allow_evars  && _tmp9C1 != 0)for(0;_tmp9C1 != 0;_tmp9C1=_tmp9C1->tl){void*e=(
void*)_tmp9C1->hd;void*_tmp9C7=Cyc_Tcutil_typ_kind(e);_LL5FC: if((int)_tmp9C7 != 4)
goto _LL5FE;_LL5FD: if(!Cyc_Tcutil_unify(e,(void*)3)){const char*_tmpFE6;void*
_tmpFE5;(_tmpFE5=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmpFE6="can't unify evar with unique region!",_tag_dyneither(_tmpFE6,sizeof(
char),37))),_tag_dyneither(_tmpFE5,sizeof(void*),0)));}goto _LL5FB;_LL5FE: if((int)
_tmp9C7 != 5)goto _LL600;_LL5FF: goto _LL601;_LL600: if((int)_tmp9C7 != 3)goto _LL602;
_LL601: if(!Cyc_Tcutil_unify(e,(void*)2)){const char*_tmpFE9;void*_tmpFE8;(_tmpFE8=
0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmpFE9="can't unify evar with heap!",_tag_dyneither(_tmpFE9,sizeof(char),28))),
_tag_dyneither(_tmpFE8,sizeof(void*),0)));}goto _LL5FB;_LL602: if((int)_tmp9C7 != 6)
goto _LL604;_LL603: if(!Cyc_Tcutil_unify(e,Cyc_Absyn_empty_effect)){const char*
_tmpFEC;void*_tmpFEB;(_tmpFEB=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpFEC="can't unify evar with {}!",
_tag_dyneither(_tmpFEC,sizeof(char),26))),_tag_dyneither(_tmpFEB,sizeof(void*),0)));}
goto _LL5FB;_LL604:;_LL605:{const char*_tmpFF1;void*_tmpFF0[2];struct Cyc_String_pa_struct
_tmpFEF;struct Cyc_String_pa_struct _tmpFEE;(_tmpFEE.tag=0,((_tmpFEE.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpFEF.tag=0,((
_tmpFEF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(e)),((
_tmpFF0[0]=& _tmpFEF,((_tmpFF0[1]=& _tmpFEE,Cyc_Tcutil_terr(loc,((_tmpFF1="hidden type variable %s isn't abstracted in type %s",
_tag_dyneither(_tmpFF1,sizeof(char),52))),_tag_dyneither(_tmpFF0,sizeof(void*),2)))))))))))));}
goto _LL5FB;_LL5FB:;}}void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*tv);
void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*tv){if(tv->identity == - 1)
tv->identity=Cyc_Tcutil_new_tvar_id();}void Cyc_Tcutil_add_tvar_identities(struct
Cyc_List_List*tvs);void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*tvs){((
void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_iter)(
Cyc_Tcutil_add_tvar_identity,tvs);}static void Cyc_Tcutil_check_unique_unsorted(
int(*cmp)(void*,void*),struct Cyc_List_List*vs,struct Cyc_Position_Segment*loc,
struct _dyneither_ptr(*a2string)(void*),struct _dyneither_ptr msg);static void Cyc_Tcutil_check_unique_unsorted(
int(*cmp)(void*,void*),struct Cyc_List_List*vs,struct Cyc_Position_Segment*loc,
struct _dyneither_ptr(*a2string)(void*),struct _dyneither_ptr msg){for(0;vs != 0;vs=
vs->tl){struct Cyc_List_List*vs2=vs->tl;for(0;vs2 != 0;vs2=vs2->tl){if(cmp((void*)
vs->hd,(void*)vs2->hd)== 0){const char*_tmpFF6;void*_tmpFF5[2];struct Cyc_String_pa_struct
_tmpFF4;struct Cyc_String_pa_struct _tmpFF3;(_tmpFF3.tag=0,((_tmpFF3.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)a2string((void*)vs->hd)),((_tmpFF4.tag=0,((
_tmpFF4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg),((_tmpFF5[0]=&
_tmpFF4,((_tmpFF5[1]=& _tmpFF3,Cyc_Tcutil_terr(loc,((_tmpFF6="%s: %s",
_tag_dyneither(_tmpFF6,sizeof(char),7))),_tag_dyneither(_tmpFF5,sizeof(void*),2)))))))))))));}}}}
static struct _dyneither_ptr Cyc_Tcutil_strptr2string(struct _dyneither_ptr*s);
static struct _dyneither_ptr Cyc_Tcutil_strptr2string(struct _dyneither_ptr*s){
return*s;}void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*vs,struct Cyc_Position_Segment*
loc,struct _dyneither_ptr msg);void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*
vs,struct Cyc_Position_Segment*loc,struct _dyneither_ptr msg){((void(*)(int(*cmp)(
struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*vs,struct Cyc_Position_Segment*
loc,struct _dyneither_ptr(*a2string)(struct _dyneither_ptr*),struct _dyneither_ptr
msg))Cyc_Tcutil_check_unique_unsorted)(Cyc_strptrcmp,vs,loc,Cyc_Tcutil_strptr2string,
msg);}void Cyc_Tcutil_check_unique_tvars(struct Cyc_Position_Segment*loc,struct Cyc_List_List*
tvs);void Cyc_Tcutil_check_unique_tvars(struct Cyc_Position_Segment*loc,struct Cyc_List_List*
tvs){const char*_tmpFF7;((void(*)(int(*cmp)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*vs,struct Cyc_Position_Segment*loc,struct _dyneither_ptr(*
a2string)(struct Cyc_Absyn_Tvar*),struct _dyneither_ptr msg))Cyc_Tcutil_check_unique_unsorted)(
Cyc_Absyn_tvar_cmp,tvs,loc,Cyc_Tcutil_tvar2string,((_tmpFF7="duplicate type variable",
_tag_dyneither(_tmpFF7,sizeof(char),24))));}struct _tuple24{struct Cyc_Absyn_Aggrfield*
f1;int f2;};struct _tuple25{struct Cyc_List_List*f1;void*f2;};struct _tuple26{struct
Cyc_Absyn_Aggrfield*f1;void*f2;};struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(
struct _RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,
void*aggr_kind,struct Cyc_List_List*sdfields);struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(
struct _RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,
void*aggr_kind,struct Cyc_List_List*sdfields){struct Cyc_List_List*fields=0;{
struct Cyc_List_List*sd_fields=sdfields;for(0;sd_fields != 0;sd_fields=sd_fields->tl){
const char*_tmpFF8;if(Cyc_strcmp((struct _dyneither_ptr)*((struct Cyc_Absyn_Aggrfield*)
sd_fields->hd)->name,((_tmpFF8="",_tag_dyneither(_tmpFF8,sizeof(char),1))))!= 0){
struct _tuple24*_tmpFFB;struct Cyc_List_List*_tmpFFA;fields=((_tmpFFA=_cycalloc(
sizeof(*_tmpFFA)),((_tmpFFA->hd=((_tmpFFB=_cycalloc(sizeof(*_tmpFFB)),((_tmpFFB->f1=(
struct Cyc_Absyn_Aggrfield*)sd_fields->hd,((_tmpFFB->f2=0,_tmpFFB)))))),((_tmpFFA->tl=
fields,_tmpFFA))))));}}}fields=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(fields);{const char*_tmpFFD;const char*_tmpFFC;struct
_dyneither_ptr aggr_str=aggr_kind == (void*)0?(_tmpFFD="struct",_tag_dyneither(
_tmpFFD,sizeof(char),7)):((_tmpFFC="union",_tag_dyneither(_tmpFFC,sizeof(char),6)));
struct Cyc_List_List*ans=0;for(0;des != 0;des=des->tl){struct _tuple25 _tmp9DC;
struct Cyc_List_List*_tmp9DD;void*_tmp9DE;struct _tuple25*_tmp9DB=(struct _tuple25*)
des->hd;_tmp9DC=*_tmp9DB;_tmp9DD=_tmp9DC.f1;_tmp9DE=_tmp9DC.f2;if(_tmp9DD == 0){
struct Cyc_List_List*_tmp9DF=fields;for(0;_tmp9DF != 0;_tmp9DF=_tmp9DF->tl){if(!(*((
struct _tuple24*)_tmp9DF->hd)).f2){(*((struct _tuple24*)_tmp9DF->hd)).f2=1;{struct
Cyc_Absyn_FieldName_struct*_tmp1003;struct Cyc_Absyn_FieldName_struct _tmp1002;
struct Cyc_List_List*_tmp1001;(*((struct _tuple25*)des->hd)).f1=(struct Cyc_List_List*)((
_tmp1001=_cycalloc(sizeof(*_tmp1001)),((_tmp1001->hd=(void*)((void*)((_tmp1003=
_cycalloc(sizeof(*_tmp1003)),((_tmp1003[0]=((_tmp1002.tag=1,((_tmp1002.f1=((*((
struct _tuple24*)_tmp9DF->hd)).f1)->name,_tmp1002)))),_tmp1003))))),((_tmp1001->tl=
0,_tmp1001))))));}{struct _tuple26*_tmp1006;struct Cyc_List_List*_tmp1005;ans=((
_tmp1005=_region_malloc(rgn,sizeof(*_tmp1005)),((_tmp1005->hd=((_tmp1006=
_region_malloc(rgn,sizeof(*_tmp1006)),((_tmp1006->f1=(*((struct _tuple24*)_tmp9DF->hd)).f1,((
_tmp1006->f2=_tmp9DE,_tmp1006)))))),((_tmp1005->tl=ans,_tmp1005))))));}break;}}
if(_tmp9DF == 0){const char*_tmp100A;void*_tmp1009[1];struct Cyc_String_pa_struct
_tmp1008;(_tmp1008.tag=0,((_tmp1008.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)aggr_str),((_tmp1009[0]=& _tmp1008,Cyc_Tcutil_terr(loc,((_tmp100A="too many arguments to %s",
_tag_dyneither(_tmp100A,sizeof(char),25))),_tag_dyneither(_tmp1009,sizeof(void*),
1)))))));}}else{if(_tmp9DD->tl != 0){const char*_tmp100D;void*_tmp100C;(_tmp100C=0,
Cyc_Tcutil_terr(loc,((_tmp100D="multiple designators are not yet supported",
_tag_dyneither(_tmp100D,sizeof(char),43))),_tag_dyneither(_tmp100C,sizeof(void*),
0)));}else{void*_tmp9EA=(void*)_tmp9DD->hd;struct _dyneither_ptr*_tmp9EB;_LL607:
if(*((int*)_tmp9EA)!= 0)goto _LL609;_LL608:{const char*_tmp1011;void*_tmp1010[1];
struct Cyc_String_pa_struct _tmp100F;(_tmp100F.tag=0,((_tmp100F.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)aggr_str),((_tmp1010[0]=& _tmp100F,Cyc_Tcutil_terr(
loc,((_tmp1011="array designator used in argument to %s",_tag_dyneither(_tmp1011,
sizeof(char),40))),_tag_dyneither(_tmp1010,sizeof(void*),1)))))));}goto _LL606;
_LL609: if(*((int*)_tmp9EA)!= 1)goto _LL606;_tmp9EB=((struct Cyc_Absyn_FieldName_struct*)
_tmp9EA)->f1;_LL60A: {struct Cyc_List_List*_tmp9EF=fields;for(0;_tmp9EF != 0;
_tmp9EF=_tmp9EF->tl){if(Cyc_strptrcmp(_tmp9EB,((*((struct _tuple24*)_tmp9EF->hd)).f1)->name)
== 0){if((*((struct _tuple24*)_tmp9EF->hd)).f2){const char*_tmp1015;void*_tmp1014[
1];struct Cyc_String_pa_struct _tmp1013;(_tmp1013.tag=0,((_tmp1013.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*_tmp9EB),((_tmp1014[0]=& _tmp1013,Cyc_Tcutil_terr(
loc,((_tmp1015="member %s has already been used as an argument",_tag_dyneither(
_tmp1015,sizeof(char),47))),_tag_dyneither(_tmp1014,sizeof(void*),1)))))));}(*((
struct _tuple24*)_tmp9EF->hd)).f2=1;{struct _tuple26*_tmp1018;struct Cyc_List_List*
_tmp1017;ans=((_tmp1017=_region_malloc(rgn,sizeof(*_tmp1017)),((_tmp1017->hd=((
_tmp1018=_region_malloc(rgn,sizeof(*_tmp1018)),((_tmp1018->f1=(*((struct _tuple24*)
_tmp9EF->hd)).f1,((_tmp1018->f2=_tmp9DE,_tmp1018)))))),((_tmp1017->tl=ans,
_tmp1017))))));}break;}}if(_tmp9EF == 0){const char*_tmp101C;void*_tmp101B[1];
struct Cyc_String_pa_struct _tmp101A;(_tmp101A.tag=0,((_tmp101A.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*_tmp9EB),((_tmp101B[0]=& _tmp101A,Cyc_Tcutil_terr(
loc,((_tmp101C="bad field designator %s",_tag_dyneither(_tmp101C,sizeof(char),24))),
_tag_dyneither(_tmp101B,sizeof(void*),1)))))));}goto _LL606;}_LL606:;}}}if(
aggr_kind == (void*)0)for(0;fields != 0;fields=fields->tl){if(!(*((struct _tuple24*)
fields->hd)).f2){{const char*_tmp101F;void*_tmp101E;(_tmp101E=0,Cyc_Tcutil_terr(
loc,((_tmp101F="too few arguments to struct",_tag_dyneither(_tmp101F,sizeof(char),
28))),_tag_dyneither(_tmp101E,sizeof(void*),0)));}break;}}else{int found=0;for(0;
fields != 0;fields=fields->tl){if((*((struct _tuple24*)fields->hd)).f2){if(found){
const char*_tmp1022;void*_tmp1021;(_tmp1021=0,Cyc_Tcutil_terr(loc,((_tmp1022="only one member of a union is allowed",
_tag_dyneither(_tmp1022,sizeof(char),38))),_tag_dyneither(_tmp1021,sizeof(void*),
0)));}found=1;}}if(!found){const char*_tmp1025;void*_tmp1024;(_tmp1024=0,Cyc_Tcutil_terr(
loc,((_tmp1025="missing member for union",_tag_dyneither(_tmp1025,sizeof(char),
25))),_tag_dyneither(_tmp1024,sizeof(void*),0)));}}return((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(ans);}}int Cyc_Tcutil_is_tagged_pointer_typ_elt(
void*t,void**elt_typ_dest);int Cyc_Tcutil_is_tagged_pointer_typ_elt(void*t,void**
elt_typ_dest){void*_tmpA00=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmpA01;
void*_tmpA02;struct Cyc_Absyn_PtrAtts _tmpA03;union Cyc_Absyn_Constraint*_tmpA04;
_LL60C: if(_tmpA00 <= (void*)4)goto _LL60E;if(*((int*)_tmpA00)!= 4)goto _LL60E;
_tmpA01=((struct Cyc_Absyn_PointerType_struct*)_tmpA00)->f1;_tmpA02=_tmpA01.elt_typ;
_tmpA03=_tmpA01.ptr_atts;_tmpA04=_tmpA03.bounds;_LL60D: {void*_tmpA05=Cyc_Absyn_conref_constr((
void*)((void*)0),_tmpA04);_LL611: if((int)_tmpA05 != 0)goto _LL613;_LL612:*
elt_typ_dest=_tmpA02;return 1;_LL613:;_LL614: return 0;_LL610:;}_LL60E:;_LL60F:
return 0;_LL60B:;}int Cyc_Tcutil_is_zero_pointer_typ_elt(void*t,void**elt_typ_dest);
int Cyc_Tcutil_is_zero_pointer_typ_elt(void*t,void**elt_typ_dest){void*_tmpA06=
Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmpA07;void*_tmpA08;struct Cyc_Absyn_PtrAtts
_tmpA09;union Cyc_Absyn_Constraint*_tmpA0A;_LL616: if(_tmpA06 <= (void*)4)goto
_LL618;if(*((int*)_tmpA06)!= 4)goto _LL618;_tmpA07=((struct Cyc_Absyn_PointerType_struct*)
_tmpA06)->f1;_tmpA08=_tmpA07.elt_typ;_tmpA09=_tmpA07.ptr_atts;_tmpA0A=_tmpA09.zero_term;
_LL617:*elt_typ_dest=_tmpA08;return((int(*)(int y,union Cyc_Absyn_Constraint*x))
Cyc_Absyn_conref_def)(0,_tmpA0A);_LL618:;_LL619: return 0;_LL615:;}int Cyc_Tcutil_is_zero_ptr_type(
void*t,void**ptr_type,int*is_dyneither,void**elt_type);int Cyc_Tcutil_is_zero_ptr_type(
void*t,void**ptr_type,int*is_dyneither,void**elt_type){void*_tmpA0B=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_PtrInfo _tmpA0C;void*_tmpA0D;struct Cyc_Absyn_PtrAtts _tmpA0E;
union Cyc_Absyn_Constraint*_tmpA0F;union Cyc_Absyn_Constraint*_tmpA10;struct Cyc_Absyn_ArrayInfo
_tmpA11;void*_tmpA12;struct Cyc_Absyn_Tqual _tmpA13;struct Cyc_Absyn_Exp*_tmpA14;
union Cyc_Absyn_Constraint*_tmpA15;_LL61B: if(_tmpA0B <= (void*)4)goto _LL61F;if(*((
int*)_tmpA0B)!= 4)goto _LL61D;_tmpA0C=((struct Cyc_Absyn_PointerType_struct*)
_tmpA0B)->f1;_tmpA0D=_tmpA0C.elt_typ;_tmpA0E=_tmpA0C.ptr_atts;_tmpA0F=_tmpA0E.bounds;
_tmpA10=_tmpA0E.zero_term;_LL61C: if(((int(*)(int y,union Cyc_Absyn_Constraint*x))
Cyc_Absyn_conref_def)(0,_tmpA10)){*ptr_type=t;*elt_type=_tmpA0D;{void*_tmpA16=
Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmpA0F);_LL622: if((int)_tmpA16 != 0)
goto _LL624;_LL623:*is_dyneither=1;goto _LL621;_LL624:;_LL625:*is_dyneither=0;goto
_LL621;_LL621:;}return 1;}else{return 0;}_LL61D: if(*((int*)_tmpA0B)!= 7)goto _LL61F;
_tmpA11=((struct Cyc_Absyn_ArrayType_struct*)_tmpA0B)->f1;_tmpA12=_tmpA11.elt_type;
_tmpA13=_tmpA11.tq;_tmpA14=_tmpA11.num_elts;_tmpA15=_tmpA11.zero_term;_LL61E: if(((
int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpA15)){*
elt_type=_tmpA12;*is_dyneither=0;{struct Cyc_Absyn_PointerType_struct _tmp103A;
struct Cyc_Absyn_PtrAtts _tmp1039;struct Cyc_Absyn_Upper_b_struct _tmp1038;struct Cyc_Absyn_Upper_b_struct*
_tmp1037;struct Cyc_Absyn_PtrInfo _tmp1036;struct Cyc_Absyn_PointerType_struct*
_tmp1035;*ptr_type=(void*)((_tmp1035=_cycalloc(sizeof(*_tmp1035)),((_tmp1035[0]=((
_tmp103A.tag=4,((_tmp103A.f1=((_tmp1036.elt_typ=_tmpA12,((_tmp1036.elt_tq=
_tmpA13,((_tmp1036.ptr_atts=((_tmp1039.rgn=(void*)2,((_tmp1039.nullable=Cyc_Absyn_false_conref,((
_tmp1039.bounds=Cyc_Absyn_new_conref((void*)((_tmp1037=_cycalloc(sizeof(*
_tmp1037)),((_tmp1037[0]=((_tmp1038.tag=0,((_tmp1038.f1=(struct Cyc_Absyn_Exp*)
_check_null(_tmpA14),_tmp1038)))),_tmp1037))))),((_tmp1039.zero_term=_tmpA15,((
_tmp1039.ptrloc=0,_tmp1039)))))))))),_tmp1036)))))),_tmp103A)))),_tmp1035))));}
return 1;}else{return 0;}_LL61F:;_LL620: return 0;_LL61A:;}int Cyc_Tcutil_is_zero_ptr_deref(
struct Cyc_Absyn_Exp*e1,void**ptr_type,int*is_dyneither,void**elt_type);int Cyc_Tcutil_is_zero_ptr_deref(
struct Cyc_Absyn_Exp*e1,void**ptr_type,int*is_dyneither,void**elt_type){void*
_tmpA1D=e1->r;struct Cyc_Absyn_Exp*_tmpA1E;struct Cyc_Absyn_Exp*_tmpA1F;struct Cyc_Absyn_Exp*
_tmpA20;struct Cyc_Absyn_Exp*_tmpA21;struct Cyc_Absyn_Exp*_tmpA22;struct Cyc_Absyn_Exp*
_tmpA23;_LL627: if(*((int*)_tmpA1D)!= 15)goto _LL629;_LL628: {const char*_tmp103E;
void*_tmp103D[1];struct Cyc_String_pa_struct _tmp103C;(_tmp103C.tag=0,((_tmp103C.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e1)),((
_tmp103D[0]=& _tmp103C,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmp103E="we have a cast in a lhs:  %s",_tag_dyneither(
_tmp103E,sizeof(char),29))),_tag_dyneither(_tmp103D,sizeof(void*),1)))))));}
_LL629: if(*((int*)_tmpA1D)!= 22)goto _LL62B;_tmpA1E=((struct Cyc_Absyn_Deref_e_struct*)
_tmpA1D)->f1;_LL62A: _tmpA1F=_tmpA1E;goto _LL62C;_LL62B: if(*((int*)_tmpA1D)!= 25)
goto _LL62D;_tmpA1F=((struct Cyc_Absyn_Subscript_e_struct*)_tmpA1D)->f1;_LL62C:
return Cyc_Tcutil_is_zero_ptr_type((void*)((struct Cyc_Core_Opt*)_check_null(
_tmpA1F->topt))->v,ptr_type,is_dyneither,elt_type);_LL62D: if(*((int*)_tmpA1D)!= 
24)goto _LL62F;_tmpA20=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmpA1D)->f1;_LL62E:
_tmpA21=_tmpA20;goto _LL630;_LL62F: if(*((int*)_tmpA1D)!= 23)goto _LL631;_tmpA21=((
struct Cyc_Absyn_AggrMember_e_struct*)_tmpA1D)->f1;_LL630: if(Cyc_Tcutil_is_zero_ptr_type((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpA21->topt))->v,ptr_type,is_dyneither,
elt_type)){const char*_tmp1042;void*_tmp1041[1];struct Cyc_String_pa_struct
_tmp1040;(_tmp1040.tag=0,((_tmp1040.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_exp2string(e1)),((_tmp1041[0]=& _tmp1040,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1042="found zero pointer aggregate member assignment: %s",
_tag_dyneither(_tmp1042,sizeof(char),51))),_tag_dyneither(_tmp1041,sizeof(void*),
1)))))));}return 0;_LL631: if(*((int*)_tmpA1D)!= 14)goto _LL633;_tmpA22=((struct Cyc_Absyn_Instantiate_e_struct*)
_tmpA1D)->f1;_LL632: _tmpA23=_tmpA22;goto _LL634;_LL633: if(*((int*)_tmpA1D)!= 13)
goto _LL635;_tmpA23=((struct Cyc_Absyn_NoInstantiate_e_struct*)_tmpA1D)->f1;_LL634:
if(Cyc_Tcutil_is_zero_ptr_type((void*)((struct Cyc_Core_Opt*)_check_null(_tmpA23->topt))->v,
ptr_type,is_dyneither,elt_type)){const char*_tmp1046;void*_tmp1045[1];struct Cyc_String_pa_struct
_tmp1044;(_tmp1044.tag=0,((_tmp1044.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_exp2string(e1)),((_tmp1045[0]=& _tmp1044,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1046="found zero pointer instantiate/noinstantiate: %s",
_tag_dyneither(_tmp1046,sizeof(char),49))),_tag_dyneither(_tmp1045,sizeof(void*),
1)))))));}return 0;_LL635: if(*((int*)_tmpA1D)!= 1)goto _LL637;_LL636: return 0;
_LL637:;_LL638: {const char*_tmp104A;void*_tmp1049[1];struct Cyc_String_pa_struct
_tmp1048;(_tmp1048.tag=0,((_tmp1048.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_exp2string(e1)),((_tmp1049[0]=& _tmp1048,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp104A="found bad lhs in is_zero_ptr_deref: %s",
_tag_dyneither(_tmp104A,sizeof(char),39))),_tag_dyneither(_tmp1049,sizeof(void*),
1)))))));}_LL626:;}int Cyc_Tcutil_is_tagged_pointer_typ(void*t);int Cyc_Tcutil_is_tagged_pointer_typ(
void*t){void*ignore=(void*)0;return Cyc_Tcutil_is_tagged_pointer_typ_elt(t,&
ignore);}static int Cyc_Tcutil_is_noalias_region(void*r);static int Cyc_Tcutil_is_noalias_region(
void*r){void*_tmpA30=Cyc_Tcutil_compress(r);struct Cyc_Absyn_Tvar*_tmpA31;_LL63A:
if((int)_tmpA30 != 3)goto _LL63C;_LL63B: return 1;_LL63C: if(_tmpA30 <= (void*)4)goto
_LL63E;if(*((int*)_tmpA30)!= 1)goto _LL63E;_tmpA31=((struct Cyc_Absyn_VarType_struct*)
_tmpA30)->f1;_LL63D: return Cyc_Tcutil_tvar_kind(_tmpA31)== (void*)4  || Cyc_Tcutil_tvar_kind(
_tmpA31)== (void*)5;_LL63E:;_LL63F: return 0;_LL639:;}int Cyc_Tcutil_is_noalias_pointer(
void*t);int Cyc_Tcutil_is_noalias_pointer(void*t){void*_tmpA32=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_PtrInfo _tmpA33;struct Cyc_Absyn_PtrAtts _tmpA34;void*_tmpA35;
_LL641: if(_tmpA32 <= (void*)4)goto _LL643;if(*((int*)_tmpA32)!= 4)goto _LL643;
_tmpA33=((struct Cyc_Absyn_PointerType_struct*)_tmpA32)->f1;_tmpA34=_tmpA33.ptr_atts;
_tmpA35=_tmpA34.rgn;_LL642: return Cyc_Tcutil_is_noalias_region(_tmpA35);_LL643:;
_LL644: return 0;_LL640:;}int Cyc_Tcutil_is_noalias_pointer_or_aggr(struct
_RegionHandle*rgn,void*t);int Cyc_Tcutil_is_noalias_pointer_or_aggr(struct
_RegionHandle*rgn,void*t){void*_tmpA36=Cyc_Tcutil_compress(t);if(Cyc_Tcutil_is_noalias_pointer(
_tmpA36))return 1;{void*_tmpA37=_tmpA36;struct Cyc_List_List*_tmpA38;struct Cyc_Absyn_AggrInfo
_tmpA39;union Cyc_Absyn_AggrInfoU _tmpA3A;struct Cyc_Absyn_Aggrdecl**_tmpA3B;struct
Cyc_List_List*_tmpA3C;struct Cyc_List_List*_tmpA3D;struct Cyc_Absyn_AggrInfo
_tmpA3E;union Cyc_Absyn_AggrInfoU _tmpA3F;struct _tuple4 _tmpA40;struct Cyc_Absyn_DatatypeInfo
_tmpA41;union Cyc_Absyn_DatatypeInfoU _tmpA42;struct Cyc_List_List*_tmpA43;struct
Cyc_Core_Opt*_tmpA44;struct Cyc_Absyn_DatatypeFieldInfo _tmpA45;union Cyc_Absyn_DatatypeFieldInfoU
_tmpA46;struct Cyc_List_List*_tmpA47;_LL646: if(_tmpA37 <= (void*)4)goto _LL652;if(*((
int*)_tmpA37)!= 9)goto _LL648;_tmpA38=((struct Cyc_Absyn_TupleType_struct*)_tmpA37)->f1;
_LL647: while(_tmpA38 != 0){if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,(*((
struct _tuple11*)_tmpA38->hd)).f2))return 1;_tmpA38=_tmpA38->tl;}return 0;_LL648:
if(*((int*)_tmpA37)!= 10)goto _LL64A;_tmpA39=((struct Cyc_Absyn_AggrType_struct*)
_tmpA37)->f1;_tmpA3A=_tmpA39.aggr_info;if((_tmpA3A.KnownAggr).tag != 2)goto _LL64A;
_tmpA3B=(struct Cyc_Absyn_Aggrdecl**)(_tmpA3A.KnownAggr).val;_tmpA3C=_tmpA39.targs;
_LL649: if((*_tmpA3B)->impl == 0)return 0;else{struct Cyc_List_List*_tmpA48=((struct
Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,(*_tmpA3B)->tvs,_tmpA3C);struct
Cyc_List_List*_tmpA49=((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmpA3B)->impl))->fields;
void*t;while(_tmpA49 != 0){t=Cyc_Tcutil_rsubstitute(rgn,_tmpA48,((struct Cyc_Absyn_Aggrfield*)
_tmpA49->hd)->type);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,t))return 1;
_tmpA49=_tmpA49->tl;}return 0;}_LL64A: if(*((int*)_tmpA37)!= 11)goto _LL64C;_tmpA3D=((
struct Cyc_Absyn_AnonAggrType_struct*)_tmpA37)->f2;_LL64B: while(_tmpA3D != 0){if(
Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,((struct Cyc_Absyn_Aggrfield*)_tmpA3D->hd)->type))
return 1;_tmpA3D=_tmpA3D->tl;}return 0;_LL64C: if(*((int*)_tmpA37)!= 10)goto _LL64E;
_tmpA3E=((struct Cyc_Absyn_AggrType_struct*)_tmpA37)->f1;_tmpA3F=_tmpA3E.aggr_info;
if((_tmpA3F.UnknownAggr).tag != 1)goto _LL64E;_tmpA40=(struct _tuple4)(_tmpA3F.UnknownAggr).val;
_LL64D: {const char*_tmp104D;void*_tmp104C;(_tmp104C=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp104D="got unknown aggr in is_noalias_aggr",
_tag_dyneither(_tmp104D,sizeof(char),36))),_tag_dyneither(_tmp104C,sizeof(void*),
0)));}_LL64E: if(*((int*)_tmpA37)!= 2)goto _LL650;_tmpA41=((struct Cyc_Absyn_DatatypeType_struct*)
_tmpA37)->f1;_tmpA42=_tmpA41.datatype_info;_tmpA43=_tmpA41.targs;_tmpA44=_tmpA41.rgn;
_LL64F: {union Cyc_Absyn_DatatypeInfoU _tmpA4C=_tmpA42;struct Cyc_Absyn_UnknownDatatypeInfo
_tmpA4D;struct _tuple2*_tmpA4E;int _tmpA4F;struct Cyc_Absyn_Datatypedecl**_tmpA50;
struct Cyc_Absyn_Datatypedecl*_tmpA51;struct Cyc_Absyn_Datatypedecl _tmpA52;struct
Cyc_List_List*_tmpA53;struct Cyc_Core_Opt*_tmpA54;_LL655: if((_tmpA4C.UnknownDatatype).tag
!= 1)goto _LL657;_tmpA4D=(struct Cyc_Absyn_UnknownDatatypeInfo)(_tmpA4C.UnknownDatatype).val;
_tmpA4E=_tmpA4D.name;_tmpA4F=_tmpA4D.is_extensible;_LL656: {const char*_tmp1050;
void*_tmp104F;(_tmp104F=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap))Cyc_Tcutil_impos)(((_tmp1050="got unknown datatype in is_noalias_aggr",
_tag_dyneither(_tmp1050,sizeof(char),40))),_tag_dyneither(_tmp104F,sizeof(void*),
0)));}_LL657: if((_tmpA4C.KnownDatatype).tag != 2)goto _LL654;_tmpA50=(struct Cyc_Absyn_Datatypedecl**)(
_tmpA4C.KnownDatatype).val;_tmpA51=*_tmpA50;_tmpA52=*_tmpA51;_tmpA53=_tmpA52.tvs;
_tmpA54=_tmpA52.fields;_LL658: if(_tmpA44 == 0)return 0;return Cyc_Tcutil_is_noalias_region((
void*)_tmpA44->v);_LL654:;}_LL650: if(*((int*)_tmpA37)!= 3)goto _LL652;_tmpA45=((
struct Cyc_Absyn_DatatypeFieldType_struct*)_tmpA37)->f1;_tmpA46=_tmpA45.field_info;
_tmpA47=_tmpA45.targs;_LL651: {union Cyc_Absyn_DatatypeFieldInfoU _tmpA57=_tmpA46;
struct Cyc_Absyn_UnknownDatatypeFieldInfo _tmpA58;struct _tuple3 _tmpA59;struct Cyc_Absyn_Datatypedecl*
_tmpA5A;struct Cyc_Absyn_Datatypefield*_tmpA5B;_LL65A: if((_tmpA57.UnknownDatatypefield).tag
!= 1)goto _LL65C;_tmpA58=(struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmpA57.UnknownDatatypefield).val;
_LL65B: {const char*_tmp1053;void*_tmp1052;(_tmp1052=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1053="got unknown datatype field in is_noalias_aggr",
_tag_dyneither(_tmp1053,sizeof(char),46))),_tag_dyneither(_tmp1052,sizeof(void*),
0)));}_LL65C: if((_tmpA57.KnownDatatypefield).tag != 2)goto _LL659;_tmpA59=(struct
_tuple3)(_tmpA57.KnownDatatypefield).val;_tmpA5A=_tmpA59.f1;_tmpA5B=_tmpA59.f2;
_LL65D: {struct Cyc_List_List*_tmpA5E=((struct Cyc_List_List*(*)(struct
_RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(rgn,rgn,_tmpA5A->tvs,_tmpA47);struct Cyc_List_List*_tmpA5F=
_tmpA5B->typs;while(_tmpA5F != 0){_tmpA36=Cyc_Tcutil_rsubstitute(rgn,_tmpA5E,(*((
struct _tuple11*)_tmpA5F->hd)).f2);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,
_tmpA36))return 1;_tmpA5F=_tmpA5F->tl;}return 0;}_LL659:;}_LL652:;_LL653: return 0;
_LL645:;}}static int Cyc_Tcutil_is_noalias_field(struct _RegionHandle*r,void*t,
struct _dyneither_ptr*f);static int Cyc_Tcutil_is_noalias_field(struct _RegionHandle*
r,void*t,struct _dyneither_ptr*f){void*_tmpA60=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo
_tmpA61;union Cyc_Absyn_AggrInfoU _tmpA62;struct Cyc_Absyn_Aggrdecl**_tmpA63;struct
Cyc_Absyn_Aggrdecl*_tmpA64;struct Cyc_List_List*_tmpA65;struct Cyc_List_List*
_tmpA66;_LL65F: if(_tmpA60 <= (void*)4)goto _LL663;if(*((int*)_tmpA60)!= 10)goto
_LL661;_tmpA61=((struct Cyc_Absyn_AggrType_struct*)_tmpA60)->f1;_tmpA62=_tmpA61.aggr_info;
if((_tmpA62.KnownAggr).tag != 2)goto _LL661;_tmpA63=(struct Cyc_Absyn_Aggrdecl**)(
_tmpA62.KnownAggr).val;_tmpA64=*_tmpA63;_tmpA65=_tmpA61.targs;_LL660: _tmpA66=
_tmpA64->impl == 0?0:((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpA64->impl))->fields;
goto _LL662;_LL661: if(*((int*)_tmpA60)!= 11)goto _LL663;_tmpA66=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmpA60)->f2;_LL662: {struct Cyc_Absyn_Aggrfield*_tmpA67=Cyc_Absyn_lookup_field(
_tmpA66,f);{struct Cyc_Absyn_Aggrfield*_tmpA68=_tmpA67;struct Cyc_Absyn_Aggrfield
_tmpA69;void*_tmpA6A;_LL666: if(_tmpA68 != 0)goto _LL668;_LL667: {const char*
_tmp1056;void*_tmp1055;(_tmp1055=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1056="is_noalias_field: missing field",
_tag_dyneither(_tmp1056,sizeof(char),32))),_tag_dyneither(_tmp1055,sizeof(void*),
0)));}_LL668: if(_tmpA68 == 0)goto _LL665;_tmpA69=*_tmpA68;_tmpA6A=_tmpA69.type;
_LL669: return Cyc_Tcutil_is_noalias_pointer_or_aggr(r,_tmpA6A);_LL665:;}return 0;}
_LL663:;_LL664: {const char*_tmp105A;void*_tmp1059[1];struct Cyc_String_pa_struct
_tmp1058;(_tmp1058.tag=0,((_tmp1058.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp1059[0]=& _tmp1058,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp105A="is_noalias_field: invalid type |%s|",
_tag_dyneither(_tmp105A,sizeof(char),36))),_tag_dyneither(_tmp1059,sizeof(void*),
1)))))));}_LL65E:;}static int Cyc_Tcutil_is_noalias_path_aux(struct _RegionHandle*r,
struct Cyc_Absyn_Exp*e,int ignore_leaf);static int Cyc_Tcutil_is_noalias_path_aux(
struct _RegionHandle*r,struct Cyc_Absyn_Exp*e,int ignore_leaf){void*_tmpA70=e->r;
void*_tmpA71;struct Cyc_Absyn_Exp*_tmpA72;struct _dyneither_ptr*_tmpA73;struct Cyc_Absyn_Exp*
_tmpA74;struct Cyc_Absyn_Exp*_tmpA75;void*_tmpA76;void*_tmpA77;void*_tmpA78;
struct Cyc_Absyn_Exp*_tmpA79;struct Cyc_Absyn_Exp*_tmpA7A;struct Cyc_Absyn_Exp*
_tmpA7B;struct Cyc_Absyn_Exp*_tmpA7C;void*_tmpA7D;struct Cyc_Absyn_Exp*_tmpA7E;
struct Cyc_Absyn_Stmt*_tmpA7F;_LL66B: if(*((int*)_tmpA70)!= 5)goto _LL66D;_LL66C:
goto _LL66E;_LL66D: if(*((int*)_tmpA70)!= 7)goto _LL66F;_LL66E: goto _LL670;_LL66F:
if(*((int*)_tmpA70)!= 8)goto _LL671;_LL670: goto _LL672;_LL671: if(*((int*)_tmpA70)
!= 12)goto _LL673;_LL672: goto _LL674;_LL673: if(*((int*)_tmpA70)!= 16)goto _LL675;
_LL674: goto _LL676;_LL675: if(*((int*)_tmpA70)!= 18)goto _LL677;_LL676: goto _LL678;
_LL677: if(*((int*)_tmpA70)!= 19)goto _LL679;_LL678: goto _LL67A;_LL679: if(*((int*)
_tmpA70)!= 20)goto _LL67B;_LL67A: goto _LL67C;_LL67B: if(*((int*)_tmpA70)!= 21)goto
_LL67D;_LL67C: goto _LL67E;_LL67D: if(*((int*)_tmpA70)!= 27)goto _LL67F;_LL67E: goto
_LL680;_LL67F: if(*((int*)_tmpA70)!= 29)goto _LL681;_LL680: goto _LL682;_LL681: if(*((
int*)_tmpA70)!= 28)goto _LL683;_LL682: goto _LL684;_LL683: if(*((int*)_tmpA70)!= 33)
goto _LL685;_LL684: goto _LL686;_LL685: if(*((int*)_tmpA70)!= 34)goto _LL687;_LL686:
goto _LL688;_LL687: if(*((int*)_tmpA70)!= 36)goto _LL689;_LL688: goto _LL68A;_LL689:
if(*((int*)_tmpA70)!= 1)goto _LL68B;_tmpA71=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmpA70)->f2;if(_tmpA71 <= (void*)1)goto _LL68B;if(*((int*)_tmpA71)!= 0)goto _LL68B;
_LL68A: goto _LL68C;_LL68B: if(*((int*)_tmpA70)!= 3)goto _LL68D;_LL68C: return 0;
_LL68D: if(*((int*)_tmpA70)!= 22)goto _LL68F;_LL68E: goto _LL690;_LL68F: if(*((int*)
_tmpA70)!= 24)goto _LL691;_LL690: return 0;_LL691: if(*((int*)_tmpA70)!= 23)goto
_LL693;_tmpA72=((struct Cyc_Absyn_AggrMember_e_struct*)_tmpA70)->f1;_tmpA73=((
struct Cyc_Absyn_AggrMember_e_struct*)_tmpA70)->f2;_LL692: return(ignore_leaf  || 
Cyc_Tcutil_is_noalias_field(r,(void*)((struct Cyc_Core_Opt*)_check_null(_tmpA72->topt))->v,
_tmpA73)) && Cyc_Tcutil_is_noalias_path_aux(r,_tmpA72,0);_LL693: if(*((int*)
_tmpA70)!= 25)goto _LL695;_tmpA74=((struct Cyc_Absyn_Subscript_e_struct*)_tmpA70)->f1;
_tmpA75=((struct Cyc_Absyn_Subscript_e_struct*)_tmpA70)->f2;_LL694: {void*_tmpA80=
Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmpA74->topt))->v);
struct Cyc_Absyn_PtrInfo _tmpA81;void*_tmpA82;struct Cyc_List_List*_tmpA83;_LL6B8:
if(_tmpA80 <= (void*)4)goto _LL6BC;if(*((int*)_tmpA80)!= 4)goto _LL6BA;_tmpA81=((
struct Cyc_Absyn_PointerType_struct*)_tmpA80)->f1;_tmpA82=_tmpA81.elt_typ;_LL6B9:
return 0;_LL6BA: if(*((int*)_tmpA80)!= 9)goto _LL6BC;_tmpA83=((struct Cyc_Absyn_TupleType_struct*)
_tmpA80)->f1;_LL6BB: {unsigned int _tmpA85;int _tmpA86;struct _tuple13 _tmpA84=Cyc_Evexp_eval_const_uint_exp(
_tmpA75);_tmpA85=_tmpA84.f1;_tmpA86=_tmpA84.f2;if(!_tmpA86){const char*_tmp105D;
void*_tmp105C;(_tmp105C=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap))Cyc_Tcutil_impos)(((_tmp105D="is_noalias_path_aux: non-constant subscript",
_tag_dyneither(_tmp105D,sizeof(char),44))),_tag_dyneither(_tmp105C,sizeof(void*),
0)));}{struct _handler_cons _tmpA89;_push_handler(& _tmpA89);{int _tmpA8B=0;if(
setjmp(_tmpA89.handler))_tmpA8B=1;if(!_tmpA8B){{void*_tmpA8C=(*((struct _tuple11*(*)(
struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmpA83,(int)_tmpA85)).f2;int _tmpA8D=(
ignore_leaf  || Cyc_Tcutil_is_noalias_pointer_or_aggr(r,_tmpA8C)) && Cyc_Tcutil_is_noalias_path_aux(
r,_tmpA74,0);_npop_handler(0);return _tmpA8D;};_pop_handler();}else{void*_tmpA8A=(
void*)_exn_thrown;void*_tmpA8F=_tmpA8A;_LL6BF: if(_tmpA8F != Cyc_List_Nth)goto
_LL6C1;_LL6C0: {const char*_tmp1060;void*_tmp105F;return(_tmp105F=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1060="is_noalias_path_aux: out-of-bounds subscript",
_tag_dyneither(_tmp1060,sizeof(char),45))),_tag_dyneither(_tmp105F,sizeof(void*),
0)));}_LL6C1:;_LL6C2:(void)_throw(_tmpA8F);_LL6BE:;}}}}_LL6BC:;_LL6BD: {const
char*_tmp1063;void*_tmp1062;(_tmp1062=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1063="is_noalias_path_aux: subscript on non-pointer/tuple",
_tag_dyneither(_tmp1063,sizeof(char),52))),_tag_dyneither(_tmp1062,sizeof(void*),
0)));}_LL6B7:;}_LL695: if(*((int*)_tmpA70)!= 32)goto _LL697;_LL696: goto _LL698;
_LL697: if(*((int*)_tmpA70)!= 26)goto _LL699;_LL698: goto _LL69A;_LL699: if(*((int*)
_tmpA70)!= 30)goto _LL69B;_LL69A: goto _LL69C;_LL69B: if(*((int*)_tmpA70)!= 31)goto
_LL69D;_LL69C: goto _LL69E;_LL69D: if(*((int*)_tmpA70)!= 0)goto _LL69F;_LL69E: goto
_LL6A0;_LL69F: if(*((int*)_tmpA70)!= 35)goto _LL6A1;_LL6A0: goto _LL6A2;_LL6A1: if(*((
int*)_tmpA70)!= 17)goto _LL6A3;_LL6A2: goto _LL6A4;_LL6A3: if(*((int*)_tmpA70)!= 1)
goto _LL6A5;_tmpA76=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmpA70)->f2;if(
_tmpA76 <= (void*)1)goto _LL6A5;if(*((int*)_tmpA76)!= 3)goto _LL6A5;_LL6A4: goto
_LL6A6;_LL6A5: if(*((int*)_tmpA70)!= 1)goto _LL6A7;_tmpA77=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmpA70)->f2;if(_tmpA77 <= (void*)1)goto _LL6A7;if(*((int*)_tmpA77)!= 4)goto _LL6A7;
_LL6A6: goto _LL6A8;_LL6A7: if(*((int*)_tmpA70)!= 1)goto _LL6A9;_tmpA78=(void*)((
struct Cyc_Absyn_Var_e_struct*)_tmpA70)->f2;if(_tmpA78 <= (void*)1)goto _LL6A9;if(*((
int*)_tmpA78)!= 2)goto _LL6A9;_LL6A8: {int _tmpA94=ignore_leaf  || Cyc_Tcutil_is_noalias_pointer_or_aggr(
r,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);return _tmpA94;}_LL6A9:
if(*((int*)_tmpA70)!= 6)goto _LL6AB;_tmpA79=((struct Cyc_Absyn_Conditional_e_struct*)
_tmpA70)->f2;_LL6AA: _tmpA7A=_tmpA79;goto _LL6AC;_LL6AB: if(*((int*)_tmpA70)!= 9)
goto _LL6AD;_tmpA7A=((struct Cyc_Absyn_SeqExp_e_struct*)_tmpA70)->f2;_LL6AC:
_tmpA7B=_tmpA7A;goto _LL6AE;_LL6AD: if(*((int*)_tmpA70)!= 4)goto _LL6AF;_tmpA7B=((
struct Cyc_Absyn_AssignOp_e_struct*)_tmpA70)->f1;_LL6AE: return Cyc_Tcutil_is_noalias_path_aux(
r,_tmpA7B,ignore_leaf);_LL6AF: if(*((int*)_tmpA70)!= 11)goto _LL6B1;_tmpA7C=((
struct Cyc_Absyn_FnCall_e_struct*)_tmpA70)->f1;_LL6B0: {void*_tmpA95=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpA7C->topt))->v);struct Cyc_Absyn_FnInfo
_tmpA96;void*_tmpA97;struct Cyc_Absyn_PtrInfo _tmpA98;void*_tmpA99;_LL6C4: if(
_tmpA95 <= (void*)4)goto _LL6C8;if(*((int*)_tmpA95)!= 8)goto _LL6C6;_tmpA96=((
struct Cyc_Absyn_FnType_struct*)_tmpA95)->f1;_tmpA97=_tmpA96.ret_typ;_LL6C5:
return ignore_leaf  || Cyc_Tcutil_is_noalias_pointer_or_aggr(r,_tmpA97);_LL6C6: if(*((
int*)_tmpA95)!= 4)goto _LL6C8;_tmpA98=((struct Cyc_Absyn_PointerType_struct*)
_tmpA95)->f1;_tmpA99=_tmpA98.elt_typ;_LL6C7:{void*_tmpA9A=Cyc_Tcutil_compress(
_tmpA99);struct Cyc_Absyn_FnInfo _tmpA9B;void*_tmpA9C;_LL6CB: if(_tmpA9A <= (void*)4)
goto _LL6CD;if(*((int*)_tmpA9A)!= 8)goto _LL6CD;_tmpA9B=((struct Cyc_Absyn_FnType_struct*)
_tmpA9A)->f1;_tmpA9C=_tmpA9B.ret_typ;_LL6CC: return ignore_leaf  || Cyc_Tcutil_is_noalias_pointer_or_aggr(
r,_tmpA9C);_LL6CD:;_LL6CE: goto _LL6CA;_LL6CA:;}goto _LL6C9;_LL6C8:;_LL6C9: {const
char*_tmp1067;void*_tmp1066[1];struct Cyc_String_pa_struct _tmp1065;(_tmp1065.tag=
0,((_tmp1065.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpA7C->topt))->v)),((_tmp1066[0]=&
_tmp1065,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp1067="Fncall has non function type %s",_tag_dyneither(_tmp1067,sizeof(char),
32))),_tag_dyneither(_tmp1066,sizeof(void*),1)))))));}_LL6C3:;}_LL6B1: if(*((int*)
_tmpA70)!= 15)goto _LL6B3;_tmpA7D=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmpA70)->f1;
_tmpA7E=((struct Cyc_Absyn_Cast_e_struct*)_tmpA70)->f2;_LL6B2: return Cyc_Tcutil_is_noalias_pointer_or_aggr(
r,_tmpA7D) && Cyc_Tcutil_is_noalias_path_aux(r,_tmpA7E,1);_LL6B3: if(*((int*)
_tmpA70)!= 38)goto _LL6B5;_tmpA7F=((struct Cyc_Absyn_StmtExp_e_struct*)_tmpA70)->f1;
_LL6B4: while(1){void*_tmpAA0=_tmpA7F->r;struct Cyc_Absyn_Stmt*_tmpAA1;struct Cyc_Absyn_Stmt*
_tmpAA2;struct Cyc_Absyn_Decl*_tmpAA3;struct Cyc_Absyn_Stmt*_tmpAA4;struct Cyc_Absyn_Exp*
_tmpAA5;_LL6D0: if(_tmpAA0 <= (void*)1)goto _LL6D6;if(*((int*)_tmpAA0)!= 1)goto
_LL6D2;_tmpAA1=((struct Cyc_Absyn_Seq_s_struct*)_tmpAA0)->f1;_tmpAA2=((struct Cyc_Absyn_Seq_s_struct*)
_tmpAA0)->f2;_LL6D1: _tmpA7F=_tmpAA2;goto _LL6CF;_LL6D2: if(*((int*)_tmpAA0)!= 11)
goto _LL6D4;_tmpAA3=((struct Cyc_Absyn_Decl_s_struct*)_tmpAA0)->f1;_tmpAA4=((
struct Cyc_Absyn_Decl_s_struct*)_tmpAA0)->f2;_LL6D3: _tmpA7F=_tmpAA4;goto _LL6CF;
_LL6D4: if(*((int*)_tmpAA0)!= 0)goto _LL6D6;_tmpAA5=((struct Cyc_Absyn_Exp_s_struct*)
_tmpAA0)->f1;_LL6D5: return Cyc_Tcutil_is_noalias_path_aux(r,_tmpAA5,ignore_leaf);
_LL6D6:;_LL6D7: {const char*_tmp106A;void*_tmp1069;(_tmp1069=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp106A="is_noalias_stmt_exp: ill-formed StmtExp",
_tag_dyneither(_tmp106A,sizeof(char),40))),_tag_dyneither(_tmp1069,sizeof(void*),
0)));}_LL6CF:;}_LL6B5:;_LL6B6: return 0;_LL66A:;}int Cyc_Tcutil_is_noalias_path(
struct _RegionHandle*r,struct Cyc_Absyn_Exp*e);int Cyc_Tcutil_is_noalias_path(
struct _RegionHandle*r,struct Cyc_Absyn_Exp*e){return Cyc_Tcutil_is_noalias_path_aux(
r,e,0);}struct _tuple16 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*te,struct
Cyc_Absyn_Exp*e);struct _tuple16 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e){struct _tuple16 _tmp106B;struct _tuple16 bogus_ans=(
_tmp106B.f1=0,((_tmp106B.f2=(void*)2,_tmp106B)));void*_tmpAA8=e->r;struct _tuple2*
_tmpAA9;void*_tmpAAA;struct Cyc_Absyn_Exp*_tmpAAB;struct _dyneither_ptr*_tmpAAC;
int _tmpAAD;struct Cyc_Absyn_Exp*_tmpAAE;struct _dyneither_ptr*_tmpAAF;int _tmpAB0;
struct Cyc_Absyn_Exp*_tmpAB1;struct Cyc_Absyn_Exp*_tmpAB2;struct Cyc_Absyn_Exp*
_tmpAB3;_LL6D9: if(*((int*)_tmpAA8)!= 1)goto _LL6DB;_tmpAA9=((struct Cyc_Absyn_Var_e_struct*)
_tmpAA8)->f1;_tmpAAA=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmpAA8)->f2;_LL6DA: {
void*_tmpAB4=_tmpAAA;struct Cyc_Absyn_Vardecl*_tmpAB5;struct Cyc_Absyn_Vardecl*
_tmpAB6;struct Cyc_Absyn_Vardecl*_tmpAB7;struct Cyc_Absyn_Vardecl*_tmpAB8;_LL6E6:
if((int)_tmpAB4 != 0)goto _LL6E8;_LL6E7: goto _LL6E9;_LL6E8: if(_tmpAB4 <= (void*)1)
goto _LL6EA;if(*((int*)_tmpAB4)!= 1)goto _LL6EA;_LL6E9: return bogus_ans;_LL6EA: if(
_tmpAB4 <= (void*)1)goto _LL6EC;if(*((int*)_tmpAB4)!= 0)goto _LL6EC;_tmpAB5=((
struct Cyc_Absyn_Global_b_struct*)_tmpAB4)->f1;_LL6EB: {void*_tmpAB9=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);_LL6F3: if(_tmpAB9 <= (void*)
4)goto _LL6F5;if(*((int*)_tmpAB9)!= 7)goto _LL6F5;_LL6F4: {struct _tuple16 _tmp106C;
return(_tmp106C.f1=1,((_tmp106C.f2=(void*)2,_tmp106C)));}_LL6F5:;_LL6F6: {struct
_tuple16 _tmp106D;return(_tmp106D.f1=(_tmpAB5->tq).real_const,((_tmp106D.f2=(void*)
2,_tmp106D)));}_LL6F2:;}_LL6EC: if(_tmpAB4 <= (void*)1)goto _LL6EE;if(*((int*)
_tmpAB4)!= 3)goto _LL6EE;_tmpAB6=((struct Cyc_Absyn_Local_b_struct*)_tmpAB4)->f1;
_LL6ED: {void*_tmpABC=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);_LL6F8: if(_tmpABC <= (void*)4)goto _LL6FA;if(*((int*)
_tmpABC)!= 7)goto _LL6FA;_LL6F9: {struct _tuple16 _tmp106E;return(_tmp106E.f1=1,((
_tmp106E.f2=(void*)((struct Cyc_Core_Opt*)_check_null(_tmpAB6->rgn))->v,_tmp106E)));}
_LL6FA:;_LL6FB: _tmpAB6->escapes=1;{struct _tuple16 _tmp106F;return(_tmp106F.f1=(
_tmpAB6->tq).real_const,((_tmp106F.f2=(void*)((struct Cyc_Core_Opt*)_check_null(
_tmpAB6->rgn))->v,_tmp106F)));}_LL6F7:;}_LL6EE: if(_tmpAB4 <= (void*)1)goto _LL6F0;
if(*((int*)_tmpAB4)!= 4)goto _LL6F0;_tmpAB7=((struct Cyc_Absyn_Pat_b_struct*)
_tmpAB4)->f1;_LL6EF: _tmpAB8=_tmpAB7;goto _LL6F1;_LL6F0: if(_tmpAB4 <= (void*)1)goto
_LL6E5;if(*((int*)_tmpAB4)!= 2)goto _LL6E5;_tmpAB8=((struct Cyc_Absyn_Param_b_struct*)
_tmpAB4)->f1;_LL6F1: _tmpAB8->escapes=1;{struct _tuple16 _tmp1070;return(_tmp1070.f1=(
_tmpAB8->tq).real_const,((_tmp1070.f2=(void*)((struct Cyc_Core_Opt*)_check_null(
_tmpAB8->rgn))->v,_tmp1070)));}_LL6E5:;}_LL6DB: if(*((int*)_tmpAA8)!= 23)goto
_LL6DD;_tmpAAB=((struct Cyc_Absyn_AggrMember_e_struct*)_tmpAA8)->f1;_tmpAAC=((
struct Cyc_Absyn_AggrMember_e_struct*)_tmpAA8)->f2;_tmpAAD=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmpAA8)->f3;_LL6DC: if(_tmpAAD)return bogus_ans;{void*_tmpAC0=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpAAB->topt))->v);struct Cyc_List_List*
_tmpAC1;struct Cyc_Absyn_AggrInfo _tmpAC2;union Cyc_Absyn_AggrInfoU _tmpAC3;struct
Cyc_Absyn_Aggrdecl**_tmpAC4;struct Cyc_Absyn_Aggrdecl*_tmpAC5;_LL6FD: if(_tmpAC0 <= (
void*)4)goto _LL701;if(*((int*)_tmpAC0)!= 11)goto _LL6FF;_tmpAC1=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmpAC0)->f2;_LL6FE: {struct Cyc_Absyn_Aggrfield*_tmpAC6=Cyc_Absyn_lookup_field(
_tmpAC1,_tmpAAC);if(_tmpAC6 != 0  && _tmpAC6->width != 0){struct _tuple16 _tmp1071;
return(_tmp1071.f1=(_tmpAC6->tq).real_const,((_tmp1071.f2=(Cyc_Tcutil_addressof_props(
te,_tmpAAB)).f2,_tmp1071)));}return bogus_ans;}_LL6FF: if(*((int*)_tmpAC0)!= 10)
goto _LL701;_tmpAC2=((struct Cyc_Absyn_AggrType_struct*)_tmpAC0)->f1;_tmpAC3=
_tmpAC2.aggr_info;if((_tmpAC3.KnownAggr).tag != 2)goto _LL701;_tmpAC4=(struct Cyc_Absyn_Aggrdecl**)(
_tmpAC3.KnownAggr).val;_tmpAC5=*_tmpAC4;_LL700: {struct Cyc_Absyn_Aggrfield*
_tmpAC8=Cyc_Absyn_lookup_decl_field(_tmpAC5,_tmpAAC);if(_tmpAC8 != 0  && _tmpAC8->width
!= 0){struct _tuple16 _tmp1072;return(_tmp1072.f1=(_tmpAC8->tq).real_const,((
_tmp1072.f2=(Cyc_Tcutil_addressof_props(te,_tmpAAB)).f2,_tmp1072)));}return
bogus_ans;}_LL701:;_LL702: return bogus_ans;_LL6FC:;}_LL6DD: if(*((int*)_tmpAA8)!= 
24)goto _LL6DF;_tmpAAE=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmpAA8)->f1;_tmpAAF=((
struct Cyc_Absyn_AggrArrow_e_struct*)_tmpAA8)->f2;_tmpAB0=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmpAA8)->f3;_LL6DE: if(_tmpAB0)return bogus_ans;{void*_tmpACA=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpAAE->topt))->v);struct Cyc_Absyn_PtrInfo
_tmpACB;void*_tmpACC;struct Cyc_Absyn_PtrAtts _tmpACD;void*_tmpACE;_LL704: if(
_tmpACA <= (void*)4)goto _LL706;if(*((int*)_tmpACA)!= 4)goto _LL706;_tmpACB=((
struct Cyc_Absyn_PointerType_struct*)_tmpACA)->f1;_tmpACC=_tmpACB.elt_typ;_tmpACD=
_tmpACB.ptr_atts;_tmpACE=_tmpACD.rgn;_LL705: {struct Cyc_Absyn_Aggrfield*finfo;{
void*_tmpACF=Cyc_Tcutil_compress(_tmpACC);struct Cyc_List_List*_tmpAD0;struct Cyc_Absyn_AggrInfo
_tmpAD1;union Cyc_Absyn_AggrInfoU _tmpAD2;struct Cyc_Absyn_Aggrdecl**_tmpAD3;struct
Cyc_Absyn_Aggrdecl*_tmpAD4;_LL709: if(_tmpACF <= (void*)4)goto _LL70D;if(*((int*)
_tmpACF)!= 11)goto _LL70B;_tmpAD0=((struct Cyc_Absyn_AnonAggrType_struct*)_tmpACF)->f2;
_LL70A: finfo=Cyc_Absyn_lookup_field(_tmpAD0,_tmpAAF);goto _LL708;_LL70B: if(*((int*)
_tmpACF)!= 10)goto _LL70D;_tmpAD1=((struct Cyc_Absyn_AggrType_struct*)_tmpACF)->f1;
_tmpAD2=_tmpAD1.aggr_info;if((_tmpAD2.KnownAggr).tag != 2)goto _LL70D;_tmpAD3=(
struct Cyc_Absyn_Aggrdecl**)(_tmpAD2.KnownAggr).val;_tmpAD4=*_tmpAD3;_LL70C: finfo=
Cyc_Absyn_lookup_decl_field(_tmpAD4,_tmpAAF);goto _LL708;_LL70D:;_LL70E: return
bogus_ans;_LL708:;}if(finfo != 0  && finfo->width != 0){struct _tuple16 _tmp1073;
return(_tmp1073.f1=(finfo->tq).real_const,((_tmp1073.f2=_tmpACE,_tmp1073)));}
return bogus_ans;}_LL706:;_LL707: return bogus_ans;_LL703:;}_LL6DF: if(*((int*)
_tmpAA8)!= 22)goto _LL6E1;_tmpAB1=((struct Cyc_Absyn_Deref_e_struct*)_tmpAA8)->f1;
_LL6E0: {void*_tmpAD6=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmpAB1->topt))->v);struct Cyc_Absyn_PtrInfo _tmpAD7;struct Cyc_Absyn_Tqual
_tmpAD8;struct Cyc_Absyn_PtrAtts _tmpAD9;void*_tmpADA;_LL710: if(_tmpAD6 <= (void*)4)
goto _LL712;if(*((int*)_tmpAD6)!= 4)goto _LL712;_tmpAD7=((struct Cyc_Absyn_PointerType_struct*)
_tmpAD6)->f1;_tmpAD8=_tmpAD7.elt_tq;_tmpAD9=_tmpAD7.ptr_atts;_tmpADA=_tmpAD9.rgn;
_LL711: {struct _tuple16 _tmp1074;return(_tmp1074.f1=_tmpAD8.real_const,((_tmp1074.f2=
_tmpADA,_tmp1074)));}_LL712:;_LL713: return bogus_ans;_LL70F:;}_LL6E1: if(*((int*)
_tmpAA8)!= 25)goto _LL6E3;_tmpAB2=((struct Cyc_Absyn_Subscript_e_struct*)_tmpAA8)->f1;
_tmpAB3=((struct Cyc_Absyn_Subscript_e_struct*)_tmpAA8)->f2;_LL6E2: {void*t=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpAB2->topt))->v);void*_tmpADC=t;
struct Cyc_List_List*_tmpADD;struct Cyc_Absyn_PtrInfo _tmpADE;struct Cyc_Absyn_Tqual
_tmpADF;struct Cyc_Absyn_PtrAtts _tmpAE0;void*_tmpAE1;struct Cyc_Absyn_ArrayInfo
_tmpAE2;struct Cyc_Absyn_Tqual _tmpAE3;_LL715: if(_tmpADC <= (void*)4)goto _LL71B;if(*((
int*)_tmpADC)!= 9)goto _LL717;_tmpADD=((struct Cyc_Absyn_TupleType_struct*)_tmpADC)->f1;
_LL716: {unsigned int _tmpAE5;int _tmpAE6;struct _tuple13 _tmpAE4=Cyc_Evexp_eval_const_uint_exp(
_tmpAB3);_tmpAE5=_tmpAE4.f1;_tmpAE6=_tmpAE4.f2;if(!_tmpAE6)return bogus_ans;{
struct _tuple11*_tmpAE7=Cyc_Absyn_lookup_tuple_field(_tmpADD,(int)_tmpAE5);if(
_tmpAE7 != 0){struct _tuple16 _tmp1075;return(_tmp1075.f1=((*_tmpAE7).f1).real_const,((
_tmp1075.f2=(Cyc_Tcutil_addressof_props(te,_tmpAB2)).f2,_tmp1075)));}return
bogus_ans;}}_LL717: if(*((int*)_tmpADC)!= 4)goto _LL719;_tmpADE=((struct Cyc_Absyn_PointerType_struct*)
_tmpADC)->f1;_tmpADF=_tmpADE.elt_tq;_tmpAE0=_tmpADE.ptr_atts;_tmpAE1=_tmpAE0.rgn;
_LL718: {struct _tuple16 _tmp1076;return(_tmp1076.f1=_tmpADF.real_const,((_tmp1076.f2=
_tmpAE1,_tmp1076)));}_LL719: if(*((int*)_tmpADC)!= 7)goto _LL71B;_tmpAE2=((struct
Cyc_Absyn_ArrayType_struct*)_tmpADC)->f1;_tmpAE3=_tmpAE2.tq;_LL71A: {struct
_tuple16 _tmp1077;return(_tmp1077.f1=_tmpAE3.real_const,((_tmp1077.f2=(Cyc_Tcutil_addressof_props(
te,_tmpAB2)).f2,_tmp1077)));}_LL71B:;_LL71C: return bogus_ans;_LL714:;}_LL6E3:;
_LL6E4:{const char*_tmp107A;void*_tmp1079;(_tmp1079=0,Cyc_Tcutil_terr(e->loc,((
_tmp107A="unary & applied to non-lvalue",_tag_dyneither(_tmp107A,sizeof(char),30))),
_tag_dyneither(_tmp1079,sizeof(void*),0)));}return bogus_ans;_LL6D8:;}void*Cyc_Tcutil_array_to_ptr(
struct Cyc_Tcenv_Tenv*te,void*e_typ,struct Cyc_Absyn_Exp*e);void*Cyc_Tcutil_array_to_ptr(
struct Cyc_Tcenv_Tenv*te,void*e_typ,struct Cyc_Absyn_Exp*e){void*_tmpAEE=Cyc_Tcutil_compress(
e_typ);struct Cyc_Absyn_ArrayInfo _tmpAEF;void*_tmpAF0;struct Cyc_Absyn_Tqual
_tmpAF1;union Cyc_Absyn_Constraint*_tmpAF2;_LL71E: if(_tmpAEE <= (void*)4)goto
_LL720;if(*((int*)_tmpAEE)!= 7)goto _LL720;_tmpAEF=((struct Cyc_Absyn_ArrayType_struct*)
_tmpAEE)->f1;_tmpAF0=_tmpAEF.elt_type;_tmpAF1=_tmpAEF.tq;_tmpAF2=_tmpAEF.zero_term;
_LL71F: {void*_tmpAF4;struct _tuple16 _tmpAF3=Cyc_Tcutil_addressof_props(te,e);
_tmpAF4=_tmpAF3.f2;{struct Cyc_Absyn_Upper_b_struct _tmp107D;struct Cyc_Absyn_Upper_b_struct*
_tmp107C;return Cyc_Absyn_atb_typ(_tmpAF0,_tmpAF4,_tmpAF1,(void*)((_tmp107C=
_cycalloc(sizeof(*_tmp107C)),((_tmp107C[0]=((_tmp107D.tag=0,((_tmp107D.f1=e,
_tmp107D)))),_tmp107C)))),_tmpAF2);}}_LL720:;_LL721: return e_typ;_LL71D:;}void Cyc_Tcutil_check_bound(
struct Cyc_Position_Segment*loc,unsigned int i,union Cyc_Absyn_Constraint*b);void
Cyc_Tcutil_check_bound(struct Cyc_Position_Segment*loc,unsigned int i,union Cyc_Absyn_Constraint*
b){b=Cyc_Absyn_compress_conref(b);{void*_tmpAF7=Cyc_Absyn_conref_constr(Cyc_Absyn_bounds_one,
b);struct Cyc_Absyn_Exp*_tmpAF8;_LL723: if((int)_tmpAF7 != 0)goto _LL725;_LL724:
return;_LL725: if(_tmpAF7 <= (void*)1)goto _LL722;if(*((int*)_tmpAF7)!= 0)goto
_LL722;_tmpAF8=((struct Cyc_Absyn_Upper_b_struct*)_tmpAF7)->f1;_LL726: {
unsigned int _tmpAFA;int _tmpAFB;struct _tuple13 _tmpAF9=Cyc_Evexp_eval_const_uint_exp(
_tmpAF8);_tmpAFA=_tmpAF9.f1;_tmpAFB=_tmpAF9.f2;if(_tmpAFB  && _tmpAFA <= i){const
char*_tmp1082;void*_tmp1081[2];struct Cyc_Int_pa_struct _tmp1080;struct Cyc_Int_pa_struct
_tmp107F;(_tmp107F.tag=1,((_tmp107F.f1=(unsigned long)((int)i),((_tmp1080.tag=1,((
_tmp1080.f1=(unsigned long)((int)_tmpAFA),((_tmp1081[0]=& _tmp1080,((_tmp1081[1]=&
_tmp107F,Cyc_Tcutil_terr(loc,((_tmp1082="dereference is out of bounds: %d <= %d",
_tag_dyneither(_tmp1082,sizeof(char),39))),_tag_dyneither(_tmp1081,sizeof(void*),
2)))))))))))));}return;}_LL722:;}}void Cyc_Tcutil_check_nonzero_bound(struct Cyc_Position_Segment*
loc,union Cyc_Absyn_Constraint*b);void Cyc_Tcutil_check_nonzero_bound(struct Cyc_Position_Segment*
loc,union Cyc_Absyn_Constraint*b){Cyc_Tcutil_check_bound(loc,0,b);}int Cyc_Tcutil_is_bound_one(
union Cyc_Absyn_Constraint*b);int Cyc_Tcutil_is_bound_one(union Cyc_Absyn_Constraint*
b){void*_tmpB00=Cyc_Absyn_conref_def((void*)((void*)0),b);struct Cyc_Absyn_Exp*
_tmpB01;_LL728: if(_tmpB00 <= (void*)1)goto _LL72A;if(*((int*)_tmpB00)!= 0)goto
_LL72A;_tmpB01=((struct Cyc_Absyn_Upper_b_struct*)_tmpB00)->f1;_LL729: {
unsigned int _tmpB03;int _tmpB04;struct _tuple13 _tmpB02=Cyc_Evexp_eval_const_uint_exp(
_tmpB01);_tmpB03=_tmpB02.f1;_tmpB04=_tmpB02.f2;return _tmpB04  && _tmpB03 == 1;}
_LL72A:;_LL72B: return 0;_LL727:;}int Cyc_Tcutil_bits_only(void*t);int Cyc_Tcutil_bits_only(
void*t){void*_tmpB05=Cyc_Tcutil_compress(t);struct Cyc_Absyn_ArrayInfo _tmpB06;
void*_tmpB07;union Cyc_Absyn_Constraint*_tmpB08;struct Cyc_List_List*_tmpB09;
struct Cyc_Absyn_AggrInfo _tmpB0A;union Cyc_Absyn_AggrInfoU _tmpB0B;struct _tuple4
_tmpB0C;struct Cyc_Absyn_AggrInfo _tmpB0D;union Cyc_Absyn_AggrInfoU _tmpB0E;struct
Cyc_Absyn_Aggrdecl**_tmpB0F;struct Cyc_Absyn_Aggrdecl*_tmpB10;struct Cyc_List_List*
_tmpB11;struct Cyc_List_List*_tmpB12;_LL72D: if((int)_tmpB05 != 0)goto _LL72F;_LL72E:
goto _LL730;_LL72F: if(_tmpB05 <= (void*)4)goto _LL731;if(*((int*)_tmpB05)!= 5)goto
_LL731;_LL730: goto _LL732;_LL731: if((int)_tmpB05 != 1)goto _LL733;_LL732: goto _LL734;
_LL733: if(_tmpB05 <= (void*)4)goto _LL743;if(*((int*)_tmpB05)!= 6)goto _LL735;
_LL734: return 1;_LL735: if(*((int*)_tmpB05)!= 12)goto _LL737;_LL736: goto _LL738;
_LL737: if(*((int*)_tmpB05)!= 13)goto _LL739;_LL738: return 0;_LL739: if(*((int*)
_tmpB05)!= 7)goto _LL73B;_tmpB06=((struct Cyc_Absyn_ArrayType_struct*)_tmpB05)->f1;
_tmpB07=_tmpB06.elt_type;_tmpB08=_tmpB06.zero_term;_LL73A: return !((int(*)(int y,
union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpB08) && Cyc_Tcutil_bits_only(
_tmpB07);_LL73B: if(*((int*)_tmpB05)!= 9)goto _LL73D;_tmpB09=((struct Cyc_Absyn_TupleType_struct*)
_tmpB05)->f1;_LL73C: for(0;_tmpB09 != 0;_tmpB09=_tmpB09->tl){if(!Cyc_Tcutil_bits_only((*((
struct _tuple11*)_tmpB09->hd)).f2))return 0;}return 1;_LL73D: if(*((int*)_tmpB05)!= 
10)goto _LL73F;_tmpB0A=((struct Cyc_Absyn_AggrType_struct*)_tmpB05)->f1;_tmpB0B=
_tmpB0A.aggr_info;if((_tmpB0B.UnknownAggr).tag != 1)goto _LL73F;_tmpB0C=(struct
_tuple4)(_tmpB0B.UnknownAggr).val;_LL73E: return 0;_LL73F: if(*((int*)_tmpB05)!= 10)
goto _LL741;_tmpB0D=((struct Cyc_Absyn_AggrType_struct*)_tmpB05)->f1;_tmpB0E=
_tmpB0D.aggr_info;if((_tmpB0E.KnownAggr).tag != 2)goto _LL741;_tmpB0F=(struct Cyc_Absyn_Aggrdecl**)(
_tmpB0E.KnownAggr).val;_tmpB10=*_tmpB0F;_tmpB11=_tmpB0D.targs;_LL740: if(_tmpB10->impl
== 0)return 0;{struct _RegionHandle _tmpB13=_new_region("rgn");struct _RegionHandle*
rgn=& _tmpB13;_push_region(rgn);{struct Cyc_List_List*_tmpB14=((struct Cyc_List_List*(*)(
struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(rgn,rgn,_tmpB10->tvs,_tmpB11);{struct Cyc_List_List*fs=((struct
Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpB10->impl))->fields;for(0;fs != 0;fs=fs->tl){
if(!Cyc_Tcutil_bits_only(Cyc_Tcutil_rsubstitute(rgn,_tmpB14,((struct Cyc_Absyn_Aggrfield*)
fs->hd)->type))){int _tmpB15=0;_npop_handler(0);return _tmpB15;}}}{int _tmpB16=1;
_npop_handler(0);return _tmpB16;}};_pop_region(rgn);}_LL741: if(*((int*)_tmpB05)!= 
11)goto _LL743;_tmpB12=((struct Cyc_Absyn_AnonAggrType_struct*)_tmpB05)->f2;_LL742:
for(0;_tmpB12 != 0;_tmpB12=_tmpB12->tl){if(!Cyc_Tcutil_bits_only(((struct Cyc_Absyn_Aggrfield*)
_tmpB12->hd)->type))return 0;}return 1;_LL743:;_LL744: return 0;_LL72C:;}struct
_tuple27{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};static int Cyc_Tcutil_cnst_exp(
struct Cyc_Tcenv_Tenv*te,int var_okay,struct Cyc_Absyn_Exp*e);static int Cyc_Tcutil_cnst_exp(
struct Cyc_Tcenv_Tenv*te,int var_okay,struct Cyc_Absyn_Exp*e){void*_tmpB17=e->r;
struct _tuple2*_tmpB18;void*_tmpB19;struct Cyc_Absyn_Exp*_tmpB1A;struct Cyc_Absyn_Exp*
_tmpB1B;struct Cyc_Absyn_Exp*_tmpB1C;struct Cyc_Absyn_Exp*_tmpB1D;struct Cyc_Absyn_Exp*
_tmpB1E;struct Cyc_Absyn_Exp*_tmpB1F;struct Cyc_Absyn_Exp*_tmpB20;void*_tmpB21;
struct Cyc_Absyn_Exp*_tmpB22;void*_tmpB23;void*_tmpB24;struct Cyc_Absyn_Exp*
_tmpB25;struct Cyc_Absyn_Exp*_tmpB26;struct Cyc_Absyn_Exp*_tmpB27;struct Cyc_Absyn_Exp*
_tmpB28;struct Cyc_List_List*_tmpB29;struct Cyc_List_List*_tmpB2A;struct Cyc_List_List*
_tmpB2B;void*_tmpB2C;struct Cyc_List_List*_tmpB2D;struct Cyc_List_List*_tmpB2E;
struct Cyc_List_List*_tmpB2F;_LL746: if(*((int*)_tmpB17)!= 0)goto _LL748;_LL747:
goto _LL749;_LL748: if(*((int*)_tmpB17)!= 18)goto _LL74A;_LL749: goto _LL74B;_LL74A:
if(*((int*)_tmpB17)!= 19)goto _LL74C;_LL74B: goto _LL74D;_LL74C: if(*((int*)_tmpB17)
!= 20)goto _LL74E;_LL74D: goto _LL74F;_LL74E: if(*((int*)_tmpB17)!= 21)goto _LL750;
_LL74F: goto _LL751;_LL750: if(*((int*)_tmpB17)!= 33)goto _LL752;_LL751: goto _LL753;
_LL752: if(*((int*)_tmpB17)!= 34)goto _LL754;_LL753: return 1;_LL754: if(*((int*)
_tmpB17)!= 1)goto _LL756;_tmpB18=((struct Cyc_Absyn_Var_e_struct*)_tmpB17)->f1;
_tmpB19=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmpB17)->f2;_LL755: {void*
_tmpB30=_tmpB19;struct Cyc_Absyn_Vardecl*_tmpB31;_LL775: if(_tmpB30 <= (void*)1)
goto _LL779;if(*((int*)_tmpB30)!= 1)goto _LL777;_LL776: return 1;_LL777: if(*((int*)
_tmpB30)!= 0)goto _LL779;_tmpB31=((struct Cyc_Absyn_Global_b_struct*)_tmpB30)->f1;
_LL778: {void*_tmpB32=Cyc_Tcutil_compress(_tmpB31->type);_LL77E: if(_tmpB32 <= (
void*)4)goto _LL782;if(*((int*)_tmpB32)!= 7)goto _LL780;_LL77F: goto _LL781;_LL780:
if(*((int*)_tmpB32)!= 8)goto _LL782;_LL781: return 1;_LL782:;_LL783: return var_okay;
_LL77D:;}_LL779: if((int)_tmpB30 != 0)goto _LL77B;_LL77A: return 0;_LL77B:;_LL77C:
return var_okay;_LL774:;}_LL756: if(*((int*)_tmpB17)!= 6)goto _LL758;_tmpB1A=((
struct Cyc_Absyn_Conditional_e_struct*)_tmpB17)->f1;_tmpB1B=((struct Cyc_Absyn_Conditional_e_struct*)
_tmpB17)->f2;_tmpB1C=((struct Cyc_Absyn_Conditional_e_struct*)_tmpB17)->f3;_LL757:
return(Cyc_Tcutil_cnst_exp(te,0,_tmpB1A) && Cyc_Tcutil_cnst_exp(te,0,_tmpB1B))
 && Cyc_Tcutil_cnst_exp(te,0,_tmpB1C);_LL758: if(*((int*)_tmpB17)!= 9)goto _LL75A;
_tmpB1D=((struct Cyc_Absyn_SeqExp_e_struct*)_tmpB17)->f1;_tmpB1E=((struct Cyc_Absyn_SeqExp_e_struct*)
_tmpB17)->f2;_LL759: return Cyc_Tcutil_cnst_exp(te,0,_tmpB1D) && Cyc_Tcutil_cnst_exp(
te,0,_tmpB1E);_LL75A: if(*((int*)_tmpB17)!= 13)goto _LL75C;_tmpB1F=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmpB17)->f1;_LL75B: _tmpB20=_tmpB1F;goto _LL75D;_LL75C: if(*((int*)_tmpB17)!= 14)
goto _LL75E;_tmpB20=((struct Cyc_Absyn_Instantiate_e_struct*)_tmpB17)->f1;_LL75D:
return Cyc_Tcutil_cnst_exp(te,var_okay,_tmpB20);_LL75E: if(*((int*)_tmpB17)!= 15)
goto _LL760;_tmpB21=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmpB17)->f1;_tmpB22=((
struct Cyc_Absyn_Cast_e_struct*)_tmpB17)->f2;_tmpB23=(void*)((struct Cyc_Absyn_Cast_e_struct*)
_tmpB17)->f4;if((int)_tmpB23 != 1)goto _LL760;_LL75F: return Cyc_Tcutil_cnst_exp(te,
var_okay,_tmpB22);_LL760: if(*((int*)_tmpB17)!= 15)goto _LL762;_tmpB24=(void*)((
struct Cyc_Absyn_Cast_e_struct*)_tmpB17)->f1;_tmpB25=((struct Cyc_Absyn_Cast_e_struct*)
_tmpB17)->f2;_LL761: return Cyc_Tcutil_cnst_exp(te,var_okay,_tmpB25);_LL762: if(*((
int*)_tmpB17)!= 16)goto _LL764;_tmpB26=((struct Cyc_Absyn_Address_e_struct*)
_tmpB17)->f1;_LL763: return Cyc_Tcutil_cnst_exp(te,1,_tmpB26);_LL764: if(*((int*)
_tmpB17)!= 29)goto _LL766;_tmpB27=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmpB17)->f2;_tmpB28=((struct Cyc_Absyn_Comprehension_e_struct*)_tmpB17)->f3;
_LL765: return Cyc_Tcutil_cnst_exp(te,0,_tmpB27) && Cyc_Tcutil_cnst_exp(te,0,
_tmpB28);_LL766: if(*((int*)_tmpB17)!= 28)goto _LL768;_tmpB29=((struct Cyc_Absyn_Array_e_struct*)
_tmpB17)->f1;_LL767: _tmpB2A=_tmpB29;goto _LL769;_LL768: if(*((int*)_tmpB17)!= 31)
goto _LL76A;_tmpB2A=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmpB17)->f2;_LL769:
_tmpB2B=_tmpB2A;goto _LL76B;_LL76A: if(*((int*)_tmpB17)!= 30)goto _LL76C;_tmpB2B=((
struct Cyc_Absyn_Aggregate_e_struct*)_tmpB17)->f3;_LL76B: for(0;_tmpB2B != 0;
_tmpB2B=_tmpB2B->tl){if(!Cyc_Tcutil_cnst_exp(te,0,(*((struct _tuple27*)_tmpB2B->hd)).f2))
return 0;}return 1;_LL76C: if(*((int*)_tmpB17)!= 3)goto _LL76E;_tmpB2C=(void*)((
struct Cyc_Absyn_Primop_e_struct*)_tmpB17)->f1;_tmpB2D=((struct Cyc_Absyn_Primop_e_struct*)
_tmpB17)->f2;_LL76D: _tmpB2E=_tmpB2D;goto _LL76F;_LL76E: if(*((int*)_tmpB17)!= 26)
goto _LL770;_tmpB2E=((struct Cyc_Absyn_Tuple_e_struct*)_tmpB17)->f1;_LL76F: _tmpB2F=
_tmpB2E;goto _LL771;_LL770: if(*((int*)_tmpB17)!= 32)goto _LL772;_tmpB2F=((struct
Cyc_Absyn_Datatype_e_struct*)_tmpB17)->f1;_LL771: for(0;_tmpB2F != 0;_tmpB2F=
_tmpB2F->tl){if(!Cyc_Tcutil_cnst_exp(te,0,(struct Cyc_Absyn_Exp*)_tmpB2F->hd))
return 0;}return 1;_LL772:;_LL773: return 0;_LL745:;}int Cyc_Tcutil_is_const_exp(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e);int Cyc_Tcutil_is_const_exp(struct
Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){return Cyc_Tcutil_cnst_exp(te,0,e);}
static int Cyc_Tcutil_fields_support_default(struct Cyc_List_List*tvs,struct Cyc_List_List*
ts,struct Cyc_List_List*fs);int Cyc_Tcutil_supports_default(void*t);int Cyc_Tcutil_supports_default(
void*t){void*_tmpB33=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmpB34;
struct Cyc_Absyn_PtrAtts _tmpB35;union Cyc_Absyn_Constraint*_tmpB36;union Cyc_Absyn_Constraint*
_tmpB37;struct Cyc_Absyn_ArrayInfo _tmpB38;void*_tmpB39;struct Cyc_List_List*
_tmpB3A;struct Cyc_Absyn_AggrInfo _tmpB3B;union Cyc_Absyn_AggrInfoU _tmpB3C;struct
Cyc_List_List*_tmpB3D;struct Cyc_List_List*_tmpB3E;_LL785: if((int)_tmpB33 != 0)
goto _LL787;_LL786: goto _LL788;_LL787: if(_tmpB33 <= (void*)4)goto _LL789;if(*((int*)
_tmpB33)!= 5)goto _LL789;_LL788: goto _LL78A;_LL789: if((int)_tmpB33 != 1)goto _LL78B;
_LL78A: goto _LL78C;_LL78B: if(_tmpB33 <= (void*)4)goto _LL79B;if(*((int*)_tmpB33)!= 
6)goto _LL78D;_LL78C: return 1;_LL78D: if(*((int*)_tmpB33)!= 4)goto _LL78F;_tmpB34=((
struct Cyc_Absyn_PointerType_struct*)_tmpB33)->f1;_tmpB35=_tmpB34.ptr_atts;
_tmpB36=_tmpB35.nullable;_tmpB37=_tmpB35.bounds;_LL78E: {void*_tmpB3F=Cyc_Absyn_conref_def((
void*)((void*)0),_tmpB37);_LL79E: if((int)_tmpB3F != 0)goto _LL7A0;_LL79F: return 1;
_LL7A0:;_LL7A1: return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
1,_tmpB36);_LL79D:;}_LL78F: if(*((int*)_tmpB33)!= 7)goto _LL791;_tmpB38=((struct
Cyc_Absyn_ArrayType_struct*)_tmpB33)->f1;_tmpB39=_tmpB38.elt_type;_LL790: return
Cyc_Tcutil_supports_default(_tmpB39);_LL791: if(*((int*)_tmpB33)!= 9)goto _LL793;
_tmpB3A=((struct Cyc_Absyn_TupleType_struct*)_tmpB33)->f1;_LL792: for(0;_tmpB3A != 
0;_tmpB3A=_tmpB3A->tl){if(!Cyc_Tcutil_supports_default((*((struct _tuple11*)
_tmpB3A->hd)).f2))return 0;}return 1;_LL793: if(*((int*)_tmpB33)!= 10)goto _LL795;
_tmpB3B=((struct Cyc_Absyn_AggrType_struct*)_tmpB33)->f1;_tmpB3C=_tmpB3B.aggr_info;
_tmpB3D=_tmpB3B.targs;_LL794: {struct Cyc_Absyn_Aggrdecl*_tmpB40=Cyc_Absyn_get_known_aggrdecl(
_tmpB3C);if(_tmpB40->impl == 0)return 0;if(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmpB40->impl))->exist_vars != 0)return 0;return Cyc_Tcutil_fields_support_default(
_tmpB40->tvs,_tmpB3D,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpB40->impl))->fields);}
_LL795: if(*((int*)_tmpB33)!= 11)goto _LL797;_tmpB3E=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmpB33)->f2;_LL796: return Cyc_Tcutil_fields_support_default(0,0,_tmpB3E);_LL797:
if(*((int*)_tmpB33)!= 13)goto _LL799;_LL798: goto _LL79A;_LL799: if(*((int*)_tmpB33)
!= 12)goto _LL79B;_LL79A: return 1;_LL79B:;_LL79C: return 0;_LL784:;}static int Cyc_Tcutil_fields_support_default(
struct Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs);static int
Cyc_Tcutil_fields_support_default(struct Cyc_List_List*tvs,struct Cyc_List_List*ts,
struct Cyc_List_List*fs){struct _RegionHandle _tmpB41=_new_region("rgn");struct
_RegionHandle*rgn=& _tmpB41;_push_region(rgn);{struct Cyc_List_List*_tmpB42=((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,tvs,ts);for(0;fs != 0;fs=fs->tl){
void*t=Cyc_Tcutil_rsubstitute(rgn,_tmpB42,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type);
if(!Cyc_Tcutil_supports_default(t)){int _tmpB43=0;_npop_handler(0);return _tmpB43;}}}{
int _tmpB44=1;_npop_handler(0);return _tmpB44;};_pop_region(rgn);}int Cyc_Tcutil_admits_zero(
void*t);int Cyc_Tcutil_admits_zero(void*t){void*_tmpB45=Cyc_Tcutil_compress(t);
struct Cyc_Absyn_PtrInfo _tmpB46;struct Cyc_Absyn_PtrAtts _tmpB47;union Cyc_Absyn_Constraint*
_tmpB48;union Cyc_Absyn_Constraint*_tmpB49;_LL7A3: if(_tmpB45 <= (void*)4)goto
_LL7A5;if(*((int*)_tmpB45)!= 5)goto _LL7A5;_LL7A4: goto _LL7A6;_LL7A5: if((int)
_tmpB45 != 1)goto _LL7A7;_LL7A6: goto _LL7A8;_LL7A7: if(_tmpB45 <= (void*)4)goto _LL7AB;
if(*((int*)_tmpB45)!= 6)goto _LL7A9;_LL7A8: return 1;_LL7A9: if(*((int*)_tmpB45)!= 4)
goto _LL7AB;_tmpB46=((struct Cyc_Absyn_PointerType_struct*)_tmpB45)->f1;_tmpB47=
_tmpB46.ptr_atts;_tmpB48=_tmpB47.nullable;_tmpB49=_tmpB47.bounds;_LL7AA: {void*
_tmpB4A=Cyc_Absyn_conref_def((void*)((void*)0),_tmpB49);_LL7AE: if((int)_tmpB4A != 
0)goto _LL7B0;_LL7AF: return 0;_LL7B0:;_LL7B1: return((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,_tmpB48);_LL7AD:;}_LL7AB:;_LL7AC: return 0;_LL7A2:;}int
Cyc_Tcutil_is_noreturn(void*t);int Cyc_Tcutil_is_noreturn(void*t){{void*_tmpB4B=
Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmpB4C;void*_tmpB4D;struct Cyc_Absyn_FnInfo
_tmpB4E;struct Cyc_List_List*_tmpB4F;_LL7B3: if(_tmpB4B <= (void*)4)goto _LL7B7;if(*((
int*)_tmpB4B)!= 4)goto _LL7B5;_tmpB4C=((struct Cyc_Absyn_PointerType_struct*)
_tmpB4B)->f1;_tmpB4D=_tmpB4C.elt_typ;_LL7B4: return Cyc_Tcutil_is_noreturn(_tmpB4D);
_LL7B5: if(*((int*)_tmpB4B)!= 8)goto _LL7B7;_tmpB4E=((struct Cyc_Absyn_FnType_struct*)
_tmpB4B)->f1;_tmpB4F=_tmpB4E.attributes;_LL7B6: for(0;_tmpB4F != 0;_tmpB4F=_tmpB4F->tl){
void*_tmpB50=(void*)_tmpB4F->hd;_LL7BA: if((int)_tmpB50 != 3)goto _LL7BC;_LL7BB:
return 1;_LL7BC:;_LL7BD: continue;_LL7B9:;}goto _LL7B2;_LL7B7:;_LL7B8: goto _LL7B2;
_LL7B2:;}return 0;}struct Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(void*t,
struct Cyc_List_List*atts);struct Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(
void*t,struct Cyc_List_List*atts){void*_tmpB51=Cyc_Tcutil_compress(t);struct Cyc_Absyn_FnInfo
_tmpB52;struct Cyc_List_List*_tmpB53;struct Cyc_List_List**_tmpB54;_LL7BF: if(
_tmpB51 <= (void*)4)goto _LL7C1;if(*((int*)_tmpB51)!= 8)goto _LL7C1;_tmpB52=((
struct Cyc_Absyn_FnType_struct*)_tmpB51)->f1;_tmpB53=_tmpB52.attributes;_tmpB54=(
struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_struct*)_tmpB51)->f1).attributes;
_LL7C0: {struct Cyc_List_List*_tmpB55=0;for(0;atts != 0;atts=atts->tl){if(Cyc_Absyn_fntype_att((
void*)atts->hd)){struct Cyc_List_List*_tmp1083;*_tmpB54=((_tmp1083=_cycalloc(
sizeof(*_tmp1083)),((_tmp1083->hd=(void*)((void*)atts->hd),((_tmp1083->tl=*
_tmpB54,_tmp1083))))));}else{struct Cyc_List_List*_tmp1084;_tmpB55=((_tmp1084=
_cycalloc(sizeof(*_tmp1084)),((_tmp1084->hd=(void*)((void*)atts->hd),((_tmp1084->tl=
_tmpB55,_tmp1084))))));}}return _tmpB55;}_LL7C1:;_LL7C2: {const char*_tmp1087;void*
_tmp1086;(_tmp1086=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmp1087="transfer_fn_type_atts",_tag_dyneither(_tmp1087,
sizeof(char),22))),_tag_dyneither(_tmp1086,sizeof(void*),0)));}_LL7BE:;}
