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
Cyc_Core_intcmp(int,int);extern char Cyc_Core_Invalid_argument[17];struct Cyc_Core_Invalid_argument_struct{
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
int Cyc_printf(struct _dyneither_ptr,struct _dyneither_ptr);struct _dyneither_ptr Cyc_vrprintf(
struct _RegionHandle*,struct _dyneither_ptr,struct _dyneither_ptr);extern char Cyc_FileCloseError[
15];struct Cyc_FileCloseError_struct{char*tag;};extern char Cyc_FileOpenError[14];
struct Cyc_FileOpenError_struct{char*tag;struct _dyneither_ptr f1;};struct Cyc_List_List{
void*hd;struct Cyc_List_List*tl;};struct Cyc_List_List*Cyc_List_list(struct
_dyneither_ptr);int Cyc_List_length(struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_copy(struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_map(void*(*f)(
void*),struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rmap(struct
_RegionHandle*,void*(*f)(void*),struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rmap_c(
struct _RegionHandle*,void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);
extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_struct{char*
tag;};struct Cyc_List_List*Cyc_List_map2(void*(*f)(void*,void*),struct Cyc_List_List*
x,struct Cyc_List_List*y);void Cyc_List_iter(void(*f)(void*),struct Cyc_List_List*x);
struct Cyc_List_List*Cyc_List_revappend(struct Cyc_List_List*x,struct Cyc_List_List*
y);struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_rappend(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*
y);struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*x,struct Cyc_List_List*
y);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_struct{char*tag;};void*Cyc_List_nth(
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
extern char Cyc_Position_Exit[5];struct Cyc_Position_Exit_struct{char*tag;};struct
Cyc_Position_Segment;struct Cyc_List_List*Cyc_Position_strings_of_segments(struct
Cyc_List_List*);struct Cyc_Position_Lex_struct{int tag;};struct Cyc_Position_Parse_struct{
int tag;};struct Cyc_Position_Elab_struct{int tag;};struct Cyc_Position_Error{struct
_dyneither_ptr source;struct Cyc_Position_Segment*seg;void*kind;struct
_dyneither_ptr desc;};struct Cyc_Position_Error*Cyc_Position_mk_err_elab(struct Cyc_Position_Segment*,
struct _dyneither_ptr);extern char Cyc_Position_Nocontext[10];struct Cyc_Position_Nocontext_struct{
char*tag;};extern int Cyc_Position_num_errors;extern int Cyc_Position_max_errors;
void Cyc_Position_post_error(struct Cyc_Position_Error*);struct
_union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{
int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};
union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct
_union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_Loc_n Loc_n;};union Cyc_Absyn_Nmspace
Cyc_Absyn_Loc_n;union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(struct Cyc_List_List*);
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*);struct _tuple2{union
Cyc_Absyn_Nmspace f1;struct _dyneither_ptr*f2;};enum Cyc_Absyn_Scope{Cyc_Absyn_Static
 = 0,Cyc_Absyn_Abstract  = 1,Cyc_Absyn_Public  = 2,Cyc_Absyn_Extern  = 3,Cyc_Absyn_ExternC
 = 4,Cyc_Absyn_Register  = 5};struct Cyc_Absyn_Tqual{int print_const;int q_volatile;
int q_restrict;int real_const;struct Cyc_Position_Segment*loc;};enum Cyc_Absyn_Size_of{
Cyc_Absyn_Char_sz  = 0,Cyc_Absyn_Short_sz  = 1,Cyc_Absyn_Int_sz  = 2,Cyc_Absyn_Long_sz
 = 3,Cyc_Absyn_LongLong_sz  = 4};enum Cyc_Absyn_Kind{Cyc_Absyn_AnyKind  = 0,Cyc_Absyn_MemKind
 = 1,Cyc_Absyn_BoxKind  = 2,Cyc_Absyn_RgnKind  = 3,Cyc_Absyn_UniqueRgnKind  = 4,Cyc_Absyn_TopRgnKind
 = 5,Cyc_Absyn_EffKind  = 6,Cyc_Absyn_IntKind  = 7};enum Cyc_Absyn_Sign{Cyc_Absyn_Signed
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
int tag;};struct Cyc_Absyn_Upper_b_struct{int tag;struct Cyc_Absyn_Exp*f1;};extern
struct Cyc_Absyn_DynEither_b_struct Cyc_Absyn_DynEither_b_val;struct Cyc_Absyn_PtrLoc{
struct Cyc_Position_Segment*ptr_loc;struct Cyc_Position_Segment*rgn_loc;struct Cyc_Position_Segment*
zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;union Cyc_Absyn_Constraint*nullable;
union Cyc_Absyn_Constraint*bounds;union Cyc_Absyn_Constraint*zero_term;struct Cyc_Absyn_PtrLoc*
ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_typ;struct Cyc_Absyn_Tqual elt_tq;struct
Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_Numelts_ptrqual_struct{int tag;struct
Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Region_ptrqual_struct{int tag;void*f1;};struct
Cyc_Absyn_Thin_ptrqual_struct{int tag;};struct Cyc_Absyn_Fat_ptrqual_struct{int tag;
};struct Cyc_Absyn_Zeroterm_ptrqual_struct{int tag;};struct Cyc_Absyn_Nozeroterm_ptrqual_struct{
int tag;};struct Cyc_Absyn_Notnull_ptrqual_struct{int tag;};struct Cyc_Absyn_Nullable_ptrqual_struct{
int tag;};struct Cyc_Absyn_VarargInfo{struct Cyc_Core_Opt*name;struct Cyc_Absyn_Tqual
tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;struct
Cyc_Core_Opt*effect;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*
cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;};struct
Cyc_Absyn_UnknownDatatypeInfo{struct _tuple2*name;int is_extensible;};struct
_union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo
val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**
val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype
UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};union Cyc_Absyn_DatatypeInfoU
Cyc_Absyn_KnownDatatype(struct Cyc_Absyn_Datatypedecl**);struct Cyc_Absyn_DatatypeInfo{
union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{
struct _tuple2*datatype_name;struct _tuple2*field_name;int is_extensible;};struct
_union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo
val;};struct _tuple3{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*
f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple3 val;
};union Cyc_Absyn_DatatypeFieldInfoU{struct
_union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct
_union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};union Cyc_Absyn_DatatypeFieldInfoU
Cyc_Absyn_KnownDatatypefield(struct Cyc_Absyn_Datatypedecl*,struct Cyc_Absyn_Datatypefield*);
struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;
struct Cyc_List_List*targs;};struct _tuple4{enum Cyc_Absyn_AggrKind f1;struct
_tuple2*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int tag;
struct _tuple4 val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**
val;};union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;
struct _union_AggrInfoU_KnownAggr KnownAggr;};union Cyc_Absyn_AggrInfoU Cyc_Absyn_UnknownAggr(
enum Cyc_Absyn_AggrKind,struct _tuple2*,struct Cyc_Core_Opt*);union Cyc_Absyn_AggrInfoU
Cyc_Absyn_KnownAggr(struct Cyc_Absyn_Aggrdecl**);struct Cyc_Absyn_AggrInfo{union
Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{
void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*
zero_term;struct Cyc_Position_Segment*zt_loc;};struct Cyc_Absyn_VoidType_struct{
int tag;};struct Cyc_Absyn_Evar_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Core_Opt*
f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_struct{int tag;struct Cyc_Absyn_Tvar*
f1;};struct Cyc_Absyn_DatatypeType_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;}
;struct Cyc_Absyn_DatatypeFieldType_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo
f1;};struct Cyc_Absyn_PointerType_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};
struct Cyc_Absyn_IntType_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of
f2;};struct Cyc_Absyn_FloatType_struct{int tag;};struct Cyc_Absyn_DoubleType_struct{
int tag;int f1;};struct Cyc_Absyn_ArrayType_struct{int tag;struct Cyc_Absyn_ArrayInfo
f1;};struct Cyc_Absyn_FnType_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_struct{int tag;struct Cyc_Absyn_AggrInfo
f1;};struct Cyc_Absyn_AnonAggrType_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct
Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_struct{int tag;struct _tuple2*f1;
struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_struct{int tag;struct
Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_struct{int tag;void*f1;};struct
Cyc_Absyn_DynRgnType_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_struct{
int tag;struct _tuple2*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;
void**f4;};struct Cyc_Absyn_ValueofType_struct{int tag;struct Cyc_Absyn_Exp*f1;};
struct Cyc_Absyn_TagType_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_struct{
int tag;};struct Cyc_Absyn_UniqueRgn_struct{int tag;};struct Cyc_Absyn_AccessEff_struct{
int tag;void*f1;};struct Cyc_Absyn_JoinEff_struct{int tag;struct Cyc_List_List*f1;};
struct Cyc_Absyn_RgnsEff_struct{int tag;void*f1;};extern struct Cyc_Absyn_HeapRgn_struct
Cyc_Absyn_HeapRgn_val;extern struct Cyc_Absyn_UniqueRgn_struct Cyc_Absyn_UniqueRgn_val;
extern struct Cyc_Absyn_VoidType_struct Cyc_Absyn_VoidType_val;extern struct Cyc_Absyn_FloatType_struct
Cyc_Absyn_FloatType_val;struct Cyc_Absyn_NoTypes_struct{int tag;struct Cyc_List_List*
f1;struct Cyc_Position_Segment*f2;};struct Cyc_Absyn_WithTypes_struct{int tag;
struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;struct Cyc_Core_Opt*f4;
struct Cyc_List_List*f5;};enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0,Cyc_Absyn_Scanf_ft
 = 1};struct Cyc_Absyn_Regparm_att_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_struct{
int tag;};struct Cyc_Absyn_Cdecl_att_struct{int tag;};struct Cyc_Absyn_Fastcall_att_struct{
int tag;};struct Cyc_Absyn_Noreturn_att_struct{int tag;};struct Cyc_Absyn_Const_att_struct{
int tag;};struct Cyc_Absyn_Aligned_att_struct{int tag;int f1;};struct Cyc_Absyn_Packed_att_struct{
int tag;};struct Cyc_Absyn_Section_att_struct{int tag;struct _dyneither_ptr f1;};
struct Cyc_Absyn_Nocommon_att_struct{int tag;};struct Cyc_Absyn_Shared_att_struct{
int tag;};struct Cyc_Absyn_Unused_att_struct{int tag;};struct Cyc_Absyn_Weak_att_struct{
int tag;};struct Cyc_Absyn_Dllimport_att_struct{int tag;};struct Cyc_Absyn_Dllexport_att_struct{
int tag;};struct Cyc_Absyn_No_instrument_function_att_struct{int tag;};struct Cyc_Absyn_Constructor_att_struct{
int tag;};struct Cyc_Absyn_Destructor_att_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_struct{
int tag;};struct Cyc_Absyn_Format_att_struct{int tag;enum Cyc_Absyn_Format_Type f1;
int f2;int f3;};struct Cyc_Absyn_Initializes_att_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_struct{
int tag;};struct Cyc_Absyn_Mode_att_struct{int tag;struct _dyneither_ptr f1;};struct
Cyc_Absyn_Carray_mod_struct{int tag;union Cyc_Absyn_Constraint*f1;struct Cyc_Position_Segment*
f2;};struct Cyc_Absyn_ConstArray_mod_struct{int tag;struct Cyc_Absyn_Exp*f1;union
Cyc_Absyn_Constraint*f2;struct Cyc_Position_Segment*f3;};struct Cyc_Absyn_Pointer_mod_struct{
int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_struct{
int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_struct{int tag;struct Cyc_List_List*
f1;struct Cyc_Position_Segment*f2;int f3;};struct Cyc_Absyn_Attributes_mod_struct{
int tag;struct Cyc_Position_Segment*f1;struct Cyc_List_List*f2;};struct
_union_Cnst_Null_c{int tag;int val;};struct _tuple5{enum Cyc_Absyn_Sign f1;char f2;};
struct _union_Cnst_Char_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign
f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple6 val;};struct _tuple7{
enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple7 val;};
struct _tuple8{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{
int tag;struct _tuple8 val;};struct _union_Cnst_Float_c{int tag;struct _dyneither_ptr
val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{
struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct
_union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct
_union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct
_union_Cnst_String_c String_c;};extern union Cyc_Absyn_Cnst Cyc_Absyn_Null_c;enum 
Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0,Cyc_Absyn_Times  = 1,Cyc_Absyn_Minus  = 2,Cyc_Absyn_Div
 = 3,Cyc_Absyn_Mod  = 4,Cyc_Absyn_Eq  = 5,Cyc_Absyn_Neq  = 6,Cyc_Absyn_Gt  = 7,Cyc_Absyn_Lt
 = 8,Cyc_Absyn_Gte  = 9,Cyc_Absyn_Lte  = 10,Cyc_Absyn_Not  = 11,Cyc_Absyn_Bitnot  = 
12,Cyc_Absyn_Bitand  = 13,Cyc_Absyn_Bitor  = 14,Cyc_Absyn_Bitxor  = 15,Cyc_Absyn_Bitlshift
 = 16,Cyc_Absyn_Bitlrshift  = 17,Cyc_Absyn_Bitarshift  = 18,Cyc_Absyn_Numelts  = 19};
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0,Cyc_Absyn_PostInc  = 1,Cyc_Absyn_PreDec
 = 2,Cyc_Absyn_PostDec  = 3};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct
Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_struct{
int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_struct{int tag;
unsigned int f1;};enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion
 = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{
int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;
int fat_result;};struct Cyc_Absyn_Const_e_struct{int tag;union Cyc_Absyn_Cnst f1;};
struct Cyc_Absyn_Var_e_struct{int tag;struct _tuple2*f1;void*f2;};struct Cyc_Absyn_UnknownId_e_struct{
int tag;struct _tuple2*f1;};struct Cyc_Absyn_Primop_e_struct{int tag;enum Cyc_Absyn_Primop
f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_struct{int tag;struct Cyc_Absyn_Exp*
f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};
struct Cyc_Absyn_And_e_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*
f2;};struct Cyc_Absyn_Or_e_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*
f2;};struct Cyc_Absyn_SeqExp_e_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*
f2;};struct Cyc_Absyn_UnknownCall_e_struct{int tag;struct Cyc_Absyn_Exp*f1;struct
Cyc_List_List*f2;};struct Cyc_Absyn_FnCall_e_struct{int tag;struct Cyc_Absyn_Exp*f1;
struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;};struct Cyc_Absyn_Throw_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoInstantiate_e_struct{int tag;
struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_struct{int tag;struct Cyc_Absyn_Exp*
f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_struct{int tag;void*f1;struct
Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_struct{int tag;struct Cyc_Absyn_Exp*
f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_struct{int tag;void*f1;};
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
struct Cyc_Absyn_Skip_s_struct{int tag;};struct Cyc_Absyn_Exp_s_struct{int tag;
struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_struct{int tag;struct Cyc_Absyn_Stmt*
f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_struct{int tag;struct Cyc_Absyn_Exp*
f1;};struct Cyc_Absyn_IfThenElse_s_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*
f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple10{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*
f2;};struct Cyc_Absyn_While_s_struct{int tag;struct _tuple10 f1;struct Cyc_Absyn_Stmt*
f2;};struct Cyc_Absyn_Break_s_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_struct{
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_struct{int tag;struct
_dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_struct{int tag;
struct Cyc_Absyn_Exp*f1;struct _tuple10 f2;struct _tuple10 f3;struct Cyc_Absyn_Stmt*f4;
};struct Cyc_Absyn_Switch_s_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*
f2;};struct Cyc_Absyn_Fallthru_s_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**
f2;};struct Cyc_Absyn_Decl_s_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*
f2;};struct Cyc_Absyn_Label_s_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*
f2;};struct Cyc_Absyn_Do_s_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple10 f2;
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
int tag;struct _tuple2*f1;};struct Cyc_Absyn_UnknownCall_p_struct{int tag;struct
_tuple2*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_struct{int tag;
struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;struct Cyc_Core_Opt*topt;
struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*
pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*
body;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Unresolved_b_struct{int tag;
};struct Cyc_Absyn_Global_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_struct{
int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_struct{int tag;struct
Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_Absyn_Pat_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{
enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_Absyn_Tqual tq;void*type;
struct Cyc_Absyn_Exp*initializer;struct Cyc_Core_Opt*rgn;struct Cyc_List_List*
attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int
is_inline;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*effect;
void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*
cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;struct Cyc_Core_Opt*
cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;
struct Cyc_List_List*attributes;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*
name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*
attributes;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct
Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{
enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*
tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Datatypefield{
struct _tuple2*name;struct Cyc_List_List*typs;struct Cyc_Position_Segment*loc;enum 
Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct
_tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;
};struct Cyc_Absyn_Enumfield{struct _tuple2*name;struct Cyc_Absyn_Exp*tag;struct Cyc_Position_Segment*
loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct
Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple2*name;struct Cyc_Absyn_Tqual
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
int tag;struct _tuple2*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_struct{int tag;
struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_struct{
int tag;};struct Cyc_Absyn_Portoff_d_struct{int tag;};struct Cyc_Absyn_Decl{void*r;
struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_ArrayElement_struct{int tag;
struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_struct{int tag;struct
_dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_struct{
char*tag;};int Cyc_Absyn_qvar_cmp(struct _tuple2*,struct _tuple2*);int Cyc_Absyn_tvar_cmp(
struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*);struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(
struct Cyc_Position_Segment*);struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(struct
Cyc_Position_Segment*);union Cyc_Absyn_Constraint*Cyc_Absyn_new_conref(void*x);
union Cyc_Absyn_Constraint*Cyc_Absyn_empty_conref();union Cyc_Absyn_Constraint*Cyc_Absyn_compress_conref(
union Cyc_Absyn_Constraint*x);void*Cyc_Absyn_conref_val(union Cyc_Absyn_Constraint*
x);void*Cyc_Absyn_conref_def(void*y,union Cyc_Absyn_Constraint*x);void*Cyc_Absyn_conref_constr(
void*y,union Cyc_Absyn_Constraint*x);extern union Cyc_Absyn_Constraint*Cyc_Absyn_true_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref;void*Cyc_Absyn_compress_kb(
void*);void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
extern void*Cyc_Absyn_uint_typ;extern void*Cyc_Absyn_ulong_typ;extern void*Cyc_Absyn_ulonglong_typ;
extern void*Cyc_Absyn_sint_typ;extern void*Cyc_Absyn_slong_typ;extern void*Cyc_Absyn_slonglong_typ;
extern void*Cyc_Absyn_empty_effect;extern struct _tuple2*Cyc_Absyn_datatype_print_arg_qvar;
extern struct _tuple2*Cyc_Absyn_datatype_scanf_arg_qvar;extern void*Cyc_Absyn_bounds_one;
void*Cyc_Absyn_atb_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*b,union Cyc_Absyn_Constraint*
zero_term);struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);struct
Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct
_dyneither_ptr*);struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(struct Cyc_Absyn_Aggrdecl*,
struct _dyneither_ptr*);struct _tuple11{struct Cyc_Absyn_Tqual f1;void*f2;};struct
_tuple11*Cyc_Absyn_lookup_tuple_field(struct Cyc_List_List*,int);struct
_dyneither_ptr Cyc_Absyn_attribute2string(void*);int Cyc_Absyn_fntype_att(void*a);
struct _tuple12{enum Cyc_Absyn_AggrKind f1;struct _tuple2*f2;};struct _tuple12 Cyc_Absyn_aggr_kinded_name(
union Cyc_Absyn_AggrInfoU);struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(
union Cyc_Absyn_AggrInfoU info);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;
struct Cyc_Absynpp_Params{int expand_typedefs: 1;int qvar_to_Cids: 1;int
add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int rewrite_temp_tvars: 1;int
print_all_tvars: 1;int print_all_kinds: 1;int print_all_effects: 1;int
print_using_stmts: 1;int print_externC_stmts: 1;int print_full_evars: 1;int
print_zeroterm: 1;int generate_line_directives: 1;int use_curr_namespace: 1;struct Cyc_List_List*
curr_namespace;};struct _dyneither_ptr Cyc_Absynpp_typ2string(void*);struct
_dyneither_ptr Cyc_Absynpp_kind2string(enum Cyc_Absyn_Kind);struct _dyneither_ptr
Cyc_Absynpp_kindbound2string(void*);struct _dyneither_ptr Cyc_Absynpp_exp2string(
struct Cyc_Absyn_Exp*);struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple2*);
struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(
struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;extern char Cyc_Set_Absent[7];struct
Cyc_Set_Absent_struct{char*tag;};struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(
void*,void*);struct _RegionHandle*r;struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[
8];struct Cyc_Dict_Present_struct{char*tag;};extern char Cyc_Dict_Absent[7];struct
Cyc_Dict_Absent_struct{char*tag;};struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict
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
Cyc_Tcenv_Fenv;struct Cyc_Tcenv_NotLoop_j_struct{int tag;};struct Cyc_Tcenv_CaseEnd_j_struct{
int tag;};struct Cyc_Tcenv_FnEnd_j_struct{int tag;};struct Cyc_Tcenv_Stmt_j_struct{
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;
struct Cyc_Dict_Dict ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof;};struct
_RegionHandle*Cyc_Tcenv_get_fnrgn(struct Cyc_Tcenv_Tenv*);void*Cyc_Tcenv_lookup_ordinary(
struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,struct
_tuple2*);struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl(struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*,struct _tuple2*);struct Cyc_Absyn_Datatypedecl**Cyc_Tcenv_lookup_datatypedecl(
struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,struct _tuple2*);struct Cyc_Absyn_Enumdecl**
Cyc_Tcenv_lookup_enumdecl(struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,
struct _tuple2*);struct Cyc_Absyn_Typedefdecl*Cyc_Tcenv_lookup_typedefdecl(struct
Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,struct _tuple2*);struct Cyc_Tcenv_Tenv*
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
t);int Cyc_Tcutil_kind_leq(enum Cyc_Absyn_Kind k1,enum Cyc_Absyn_Kind k2);enum Cyc_Absyn_Kind
Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*t,enum Cyc_Absyn_Kind def);enum Cyc_Absyn_Kind
Cyc_Tcutil_typ_kind(void*t);void*Cyc_Tcutil_compress(void*t);void Cyc_Tcutil_unchecked_cast(
struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*,enum Cyc_Absyn_Coercion);int Cyc_Tcutil_coerce_arg(
struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*);int Cyc_Tcutil_coerce_assign(
struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*);int Cyc_Tcutil_coerce_to_bool(
struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*);int Cyc_Tcutil_coerce_list(struct Cyc_Tcenv_Tenv*,
void*,struct Cyc_List_List*);int Cyc_Tcutil_coerce_uint_typ(struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Exp*);int Cyc_Tcutil_coerce_sint_typ(struct Cyc_Tcenv_Tenv*,struct
Cyc_Absyn_Exp*);int Cyc_Tcutil_coerceable(void*);int Cyc_Tcutil_silent_castable(
struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,void*,void*);enum Cyc_Absyn_Coercion
Cyc_Tcutil_castable(struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,void*,void*);
int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*);int Cyc_Tcutil_is_numeric(struct
Cyc_Absyn_Exp*);int Cyc_Tcutil_is_function_type(void*t);int Cyc_Tcutil_is_pointer_type(
void*t);int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*e);int Cyc_Tcutil_is_pointer_or_boxed(
void*t,int*is_dyneither_ptr);void*Cyc_Tcutil_pointer_elt_type(void*t);void*Cyc_Tcutil_pointer_region(
void*t);extern struct Cyc_Core_Opt Cyc_Tcutil_rk;extern struct Cyc_Core_Opt Cyc_Tcutil_trk;
extern struct Cyc_Core_Opt Cyc_Tcutil_urk;extern struct Cyc_Core_Opt Cyc_Tcutil_ak;
extern struct Cyc_Core_Opt Cyc_Tcutil_bk;extern struct Cyc_Core_Opt Cyc_Tcutil_mk;
extern struct Cyc_Core_Opt Cyc_Tcutil_ek;extern struct Cyc_Core_Opt Cyc_Tcutil_ik;
struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_opt(enum Cyc_Absyn_Kind k);void*Cyc_Tcutil_kind_to_bound(
enum Cyc_Absyn_Kind k);int Cyc_Tcutil_unify_kindbound(void*,void*);struct _tuple14{
struct Cyc_Absyn_Tvar*f1;void*f2;};struct _tuple14 Cyc_Tcutil_swap_kind(void*t,void*
kb);int Cyc_Tcutil_zero_to_null(struct Cyc_Tcenv_Tenv*,void*t,struct Cyc_Absyn_Exp*
e);void*Cyc_Tcutil_max_arithmetic_type(void*,void*);void Cyc_Tcutil_explain_failure();
int Cyc_Tcutil_unify(void*,void*);int Cyc_Tcutil_typecmp(void*,void*);void*Cyc_Tcutil_substitute(
struct Cyc_List_List*,void*);void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,
struct Cyc_List_List*,void*);int Cyc_Tcutil_subset_effect(int may_constrain_evars,
void*e1,void*e2);int Cyc_Tcutil_region_in_effect(int constrain,void*r,void*e);void*
Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*);struct _tuple14*Cyc_Tcutil_make_inst_var(
struct Cyc_List_List*,struct Cyc_Absyn_Tvar*);struct _tuple15{struct Cyc_List_List*
f1;struct _RegionHandle*f2;};struct _tuple14*Cyc_Tcutil_r_make_inst_var(struct
_tuple15*,struct Cyc_Absyn_Tvar*);void Cyc_Tcutil_check_bitfield(struct Cyc_Position_Segment*
loc,struct Cyc_Tcenv_Tenv*te,void*field_typ,struct Cyc_Absyn_Exp*width,struct
_dyneither_ptr*fn);void Cyc_Tcutil_check_contains_assign(struct Cyc_Absyn_Exp*);
void Cyc_Tcutil_check_valid_toplevel_type(struct Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*,
void*);void Cyc_Tcutil_check_fndecl_valid_type(struct Cyc_Position_Segment*,struct
Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Fndecl*);void Cyc_Tcutil_check_type(struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*bound_tvars,enum Cyc_Absyn_Kind k,int
allow_evars,void*);void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*vs,
struct Cyc_Position_Segment*loc,struct _dyneither_ptr err_msg);void Cyc_Tcutil_check_unique_tvars(
struct Cyc_Position_Segment*,struct Cyc_List_List*);void Cyc_Tcutil_check_nonzero_bound(
struct Cyc_Position_Segment*,union Cyc_Absyn_Constraint*);void Cyc_Tcutil_check_bound(
struct Cyc_Position_Segment*,unsigned int i,union Cyc_Absyn_Constraint*);int Cyc_Tcutil_is_bound_one(
union Cyc_Absyn_Constraint*b);int Cyc_Tcutil_equal_tqual(struct Cyc_Absyn_Tqual tq1,
struct Cyc_Absyn_Tqual tq2);struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(
struct _RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,
enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields);int Cyc_Tcutil_is_tagged_pointer_typ(
void*);int Cyc_Tcutil_is_tagged_pointer_typ_elt(void*t,void**elt_typ_dest);int Cyc_Tcutil_is_zero_pointer_typ_elt(
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
struct Cyc_Absyn_Enumdecl*);extern int Cyc_Cyclone_tovc_r;enum Cyc_Cyclone_C_Compilers{
Cyc_Cyclone_Gcc_c  = 0,Cyc_Cyclone_Vc_c  = 1};char Cyc_Tcutil_Unify[6]="Unify";
struct Cyc_Tcutil_Unify_struct{char*tag;};struct Cyc_Tcutil_Unify_struct Cyc_Tcutil_Unify_val={
Cyc_Tcutil_Unify};void Cyc_Tcutil_unify_it(void*t1,void*t2);void*Cyc_Tcutil_t1_failure=(
void*)& Cyc_Absyn_VoidType_val;int Cyc_Tcutil_tq1_const=0;void*Cyc_Tcutil_t2_failure=(
void*)& Cyc_Absyn_VoidType_val;int Cyc_Tcutil_tq2_const=0;struct _dyneither_ptr Cyc_Tcutil_failure_reason=(
struct _dyneither_ptr){(void*)0,(void*)0,(void*)(0 + 0)};void Cyc_Tcutil_explain_failure();
void Cyc_Tcutil_explain_failure(){if(Cyc_Position_num_errors >= Cyc_Position_max_errors)
return;Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);{const char*_tmpE5F;const char*
_tmpE5E;const char*_tmpE5D;void*_tmpE5C[2];struct Cyc_String_pa_struct _tmpE5B;
struct Cyc_String_pa_struct _tmpE5A;struct _dyneither_ptr s1=(struct _dyneither_ptr)((
_tmpE5A.tag=0,((_tmpE5A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
Cyc_Tcutil_t1_failure)),((_tmpE5B.tag=0,((_tmpE5B.f1=(struct _dyneither_ptr)(Cyc_Tcutil_tq1_const?(
_tmpE5E="const ",_tag_dyneither(_tmpE5E,sizeof(char),7)):((_tmpE5F="",
_tag_dyneither(_tmpE5F,sizeof(char),1)))),((_tmpE5C[0]=& _tmpE5B,((_tmpE5C[1]=&
_tmpE5A,Cyc_aprintf(((_tmpE5D="%s%s",_tag_dyneither(_tmpE5D,sizeof(char),5))),
_tag_dyneither(_tmpE5C,sizeof(void*),2))))))))))))));const char*_tmpE68;const char*
_tmpE67;const char*_tmpE66;void*_tmpE65[2];struct Cyc_String_pa_struct _tmpE64;
struct Cyc_String_pa_struct _tmpE63;struct _dyneither_ptr s2=(struct _dyneither_ptr)((
_tmpE63.tag=0,((_tmpE63.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
Cyc_Tcutil_t2_failure)),((_tmpE64.tag=0,((_tmpE64.f1=(struct _dyneither_ptr)(Cyc_Tcutil_tq2_const?(
_tmpE67="const ",_tag_dyneither(_tmpE67,sizeof(char),7)):((_tmpE68="",
_tag_dyneither(_tmpE68,sizeof(char),1)))),((_tmpE65[0]=& _tmpE64,((_tmpE65[1]=&
_tmpE63,Cyc_aprintf(((_tmpE66="%s%s",_tag_dyneither(_tmpE66,sizeof(char),5))),
_tag_dyneither(_tmpE65,sizeof(void*),2))))))))))))));int pos=2;{const char*_tmpE6C;
void*_tmpE6B[1];struct Cyc_String_pa_struct _tmpE6A;(_tmpE6A.tag=0,((_tmpE6A.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)s1),((_tmpE6B[0]=& _tmpE6A,Cyc_fprintf(
Cyc_stderr,((_tmpE6C="  %s",_tag_dyneither(_tmpE6C,sizeof(char),5))),
_tag_dyneither(_tmpE6B,sizeof(void*),1)))))));}pos +=_get_dyneither_size(s1,
sizeof(char));if(pos + 5 >= 80){{const char*_tmpE6F;void*_tmpE6E;(_tmpE6E=0,Cyc_fprintf(
Cyc_stderr,((_tmpE6F="\n\t",_tag_dyneither(_tmpE6F,sizeof(char),3))),
_tag_dyneither(_tmpE6E,sizeof(void*),0)));}pos=8;}else{{const char*_tmpE72;void*
_tmpE71;(_tmpE71=0,Cyc_fprintf(Cyc_stderr,((_tmpE72=" ",_tag_dyneither(_tmpE72,
sizeof(char),2))),_tag_dyneither(_tmpE71,sizeof(void*),0)));}++ pos;}{const char*
_tmpE75;void*_tmpE74;(_tmpE74=0,Cyc_fprintf(Cyc_stderr,((_tmpE75="and ",
_tag_dyneither(_tmpE75,sizeof(char),5))),_tag_dyneither(_tmpE74,sizeof(void*),0)));}
pos +=4;if(pos + _get_dyneither_size(s2,sizeof(char))>= 80){{const char*_tmpE78;
void*_tmpE77;(_tmpE77=0,Cyc_fprintf(Cyc_stderr,((_tmpE78="\n\t",_tag_dyneither(
_tmpE78,sizeof(char),3))),_tag_dyneither(_tmpE77,sizeof(void*),0)));}pos=8;}{
const char*_tmpE7C;void*_tmpE7B[1];struct Cyc_String_pa_struct _tmpE7A;(_tmpE7A.tag=
0,((_tmpE7A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s2),((_tmpE7B[0]=&
_tmpE7A,Cyc_fprintf(Cyc_stderr,((_tmpE7C="%s ",_tag_dyneither(_tmpE7C,sizeof(
char),4))),_tag_dyneither(_tmpE7B,sizeof(void*),1)))))));}pos +=
_get_dyneither_size(s2,sizeof(char))+ 1;if(pos + 17 >= 80){{const char*_tmpE7F;void*
_tmpE7E;(_tmpE7E=0,Cyc_fprintf(Cyc_stderr,((_tmpE7F="\n\t",_tag_dyneither(
_tmpE7F,sizeof(char),3))),_tag_dyneither(_tmpE7E,sizeof(void*),0)));}pos=8;}{
const char*_tmpE82;void*_tmpE81;(_tmpE81=0,Cyc_fprintf(Cyc_stderr,((_tmpE82="are not compatible. ",
_tag_dyneither(_tmpE82,sizeof(char),21))),_tag_dyneither(_tmpE81,sizeof(void*),0)));}
pos +=17;if(Cyc_Tcutil_failure_reason.curr != (_tag_dyneither(0,0,0)).curr){if(pos
+ Cyc_strlen((struct _dyneither_ptr)Cyc_Tcutil_failure_reason)>= 80){const char*
_tmpE85;void*_tmpE84;(_tmpE84=0,Cyc_fprintf(Cyc_stderr,((_tmpE85="\n\t",
_tag_dyneither(_tmpE85,sizeof(char),3))),_tag_dyneither(_tmpE84,sizeof(void*),0)));}{
const char*_tmpE89;void*_tmpE88[1];struct Cyc_String_pa_struct _tmpE87;(_tmpE87.tag=
0,((_tmpE87.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_failure_reason),((
_tmpE88[0]=& _tmpE87,Cyc_fprintf(Cyc_stderr,((_tmpE89="%s",_tag_dyneither(_tmpE89,
sizeof(char),3))),_tag_dyneither(_tmpE88,sizeof(void*),1)))))));}}{const char*
_tmpE8C;void*_tmpE8B;(_tmpE8B=0,Cyc_fprintf(Cyc_stderr,((_tmpE8C="\n",
_tag_dyneither(_tmpE8C,sizeof(char),2))),_tag_dyneither(_tmpE8B,sizeof(void*),0)));}
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);}}void Cyc_Tcutil_terr(struct Cyc_Position_Segment*
loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);void Cyc_Tcutil_terr(struct
Cyc_Position_Segment*loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){Cyc_Position_post_error(
Cyc_Position_mk_err_elab(loc,(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,
fmt,ap)));}void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){struct
_dyneither_ptr msg=(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap);{
const char*_tmpE90;void*_tmpE8F[1];struct Cyc_String_pa_struct _tmpE8E;(_tmpE8E.tag=
0,((_tmpE8E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg),((_tmpE8F[0]=&
_tmpE8E,Cyc_fprintf(Cyc_stderr,((_tmpE90="Compiler Error (Tcutil::impos): %s\n",
_tag_dyneither(_tmpE90,sizeof(char),36))),_tag_dyneither(_tmpE8F,sizeof(void*),1)))))));}
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);{struct Cyc_Core_Impossible_struct
_tmpE93;struct Cyc_Core_Impossible_struct*_tmpE92;(int)_throw((void*)((_tmpE92=
_cycalloc(sizeof(*_tmpE92)),((_tmpE92[0]=((_tmpE93.tag=Cyc_Core_Impossible,((
_tmpE93.f1=msg,_tmpE93)))),_tmpE92)))));}}static struct _dyneither_ptr Cyc_Tcutil_tvar2string(
struct Cyc_Absyn_Tvar*tv);static struct _dyneither_ptr Cyc_Tcutil_tvar2string(struct
Cyc_Absyn_Tvar*tv){return*tv->name;}void Cyc_Tcutil_print_tvars(struct Cyc_List_List*
tvs);void Cyc_Tcutil_print_tvars(struct Cyc_List_List*tvs){for(0;tvs != 0;tvs=tvs->tl){
const char*_tmpE98;void*_tmpE97[2];struct Cyc_String_pa_struct _tmpE96;struct Cyc_String_pa_struct
_tmpE95;(_tmpE95.tag=0,((_tmpE95.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kindbound2string(((struct Cyc_Absyn_Tvar*)tvs->hd)->kind)),((_tmpE96.tag=
0,((_tmpE96.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_tvar2string((
struct Cyc_Absyn_Tvar*)tvs->hd)),((_tmpE97[0]=& _tmpE96,((_tmpE97[1]=& _tmpE95,Cyc_fprintf(
Cyc_stderr,((_tmpE98="%s::%s ",_tag_dyneither(_tmpE98,sizeof(char),8))),
_tag_dyneither(_tmpE97,sizeof(void*),2)))))))))))));}{const char*_tmpE9B;void*
_tmpE9A;(_tmpE9A=0,Cyc_fprintf(Cyc_stderr,((_tmpE9B="\n",_tag_dyneither(_tmpE9B,
sizeof(char),2))),_tag_dyneither(_tmpE9A,sizeof(void*),0)));}Cyc_fflush((struct
Cyc___cycFILE*)Cyc_stderr);}static struct Cyc_List_List*Cyc_Tcutil_warning_segs=0;
static struct Cyc_List_List*Cyc_Tcutil_warning_msgs=0;void Cyc_Tcutil_warn(struct
Cyc_Position_Segment*sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);void Cyc_Tcutil_warn(
struct Cyc_Position_Segment*sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
struct _dyneither_ptr msg=(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,
fmt,ap);{struct Cyc_List_List*_tmpE9C;Cyc_Tcutil_warning_segs=((_tmpE9C=_cycalloc(
sizeof(*_tmpE9C)),((_tmpE9C->hd=sg,((_tmpE9C->tl=Cyc_Tcutil_warning_segs,_tmpE9C))))));}{
struct _dyneither_ptr*_tmpE9F;struct Cyc_List_List*_tmpE9E;Cyc_Tcutil_warning_msgs=((
_tmpE9E=_cycalloc(sizeof(*_tmpE9E)),((_tmpE9E->hd=((_tmpE9F=_cycalloc(sizeof(*
_tmpE9F)),((_tmpE9F[0]=msg,_tmpE9F)))),((_tmpE9E->tl=Cyc_Tcutil_warning_msgs,
_tmpE9E))))));}}void Cyc_Tcutil_flush_warnings();void Cyc_Tcutil_flush_warnings(){
if(Cyc_Tcutil_warning_segs == 0)return;{const char*_tmpEA2;void*_tmpEA1;(_tmpEA1=0,
Cyc_fprintf(Cyc_stderr,((_tmpEA2="***Warnings***\n",_tag_dyneither(_tmpEA2,
sizeof(char),16))),_tag_dyneither(_tmpEA1,sizeof(void*),0)));}{struct Cyc_List_List*
_tmp36=Cyc_Position_strings_of_segments(Cyc_Tcutil_warning_segs);Cyc_Tcutil_warning_segs=
0;Cyc_Tcutil_warning_msgs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_Tcutil_warning_msgs);while(Cyc_Tcutil_warning_msgs != 0){{const char*_tmpEA7;
void*_tmpEA6[2];struct Cyc_String_pa_struct _tmpEA5;struct Cyc_String_pa_struct
_tmpEA4;(_tmpEA4.tag=0,((_tmpEA4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((
struct _dyneither_ptr*)((struct Cyc_List_List*)_check_null(Cyc_Tcutil_warning_msgs))->hd)),((
_tmpEA5.tag=0,((_tmpEA5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((
struct _dyneither_ptr*)((struct Cyc_List_List*)_check_null(_tmp36))->hd)),((
_tmpEA6[0]=& _tmpEA5,((_tmpEA6[1]=& _tmpEA4,Cyc_fprintf(Cyc_stderr,((_tmpEA7="%s: %s\n",
_tag_dyneither(_tmpEA7,sizeof(char),8))),_tag_dyneither(_tmpEA6,sizeof(void*),2)))))))))))));}
_tmp36=_tmp36->tl;Cyc_Tcutil_warning_msgs=((struct Cyc_List_List*)_check_null(Cyc_Tcutil_warning_msgs))->tl;}{
const char*_tmpEAA;void*_tmpEA9;(_tmpEA9=0,Cyc_fprintf(Cyc_stderr,((_tmpEAA="**************\n",
_tag_dyneither(_tmpEAA,sizeof(char),16))),_tag_dyneither(_tmpEA9,sizeof(void*),0)));}
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);}}struct Cyc_Core_Opt*Cyc_Tcutil_empty_var_set=
0;static int Cyc_Tcutil_fast_tvar_cmp(struct Cyc_Absyn_Tvar*tv1,struct Cyc_Absyn_Tvar*
tv2);static int Cyc_Tcutil_fast_tvar_cmp(struct Cyc_Absyn_Tvar*tv1,struct Cyc_Absyn_Tvar*
tv2){return tv1->identity - tv2->identity;}void*Cyc_Tcutil_compress(void*t);void*
Cyc_Tcutil_compress(void*t){void*_tmp3D=t;struct Cyc_Core_Opt*_tmp3F;void**_tmp41;
void**_tmp43;void***_tmp44;struct Cyc_Core_Opt*_tmp46;struct Cyc_Core_Opt**_tmp47;
_LL1: {struct Cyc_Absyn_Evar_struct*_tmp3E=(struct Cyc_Absyn_Evar_struct*)_tmp3D;
if(_tmp3E->tag != 1)goto _LL3;else{_tmp3F=_tmp3E->f2;if(_tmp3F != 0)goto _LL3;}}_LL2:
goto _LL4;_LL3: {struct Cyc_Absyn_TypedefType_struct*_tmp40=(struct Cyc_Absyn_TypedefType_struct*)
_tmp3D;if(_tmp40->tag != 18)goto _LL5;else{_tmp41=_tmp40->f4;if(_tmp41 != 0)goto
_LL5;}}_LL4: return t;_LL5: {struct Cyc_Absyn_TypedefType_struct*_tmp42=(struct Cyc_Absyn_TypedefType_struct*)
_tmp3D;if(_tmp42->tag != 18)goto _LL7;else{_tmp43=_tmp42->f4;_tmp44=(void***)&
_tmp42->f4;}}_LL6: {void*t2=Cyc_Tcutil_compress(*((void**)_check_null(*_tmp44)));
if(t2 != *((void**)_check_null(*_tmp44))){void**_tmpEAB;*_tmp44=((_tmpEAB=
_cycalloc(sizeof(*_tmpEAB)),((_tmpEAB[0]=t2,_tmpEAB))));}return t2;}_LL7: {struct
Cyc_Absyn_Evar_struct*_tmp45=(struct Cyc_Absyn_Evar_struct*)_tmp3D;if(_tmp45->tag
!= 1)goto _LL9;else{_tmp46=_tmp45->f2;_tmp47=(struct Cyc_Core_Opt**)& _tmp45->f2;}}
_LL8: {void*t2=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(*
_tmp47))->v);if(t2 != (void*)((struct Cyc_Core_Opt*)_check_null(*_tmp47))->v){
struct Cyc_Core_Opt*_tmpEAC;*_tmp47=((_tmpEAC=_cycalloc(sizeof(*_tmpEAC)),((
_tmpEAC->v=t2,_tmpEAC))));}return t2;}_LL9:;_LLA: return t;_LL0:;}void*Cyc_Tcutil_copy_type(
void*t);static struct Cyc_List_List*Cyc_Tcutil_copy_types(struct Cyc_List_List*ts);
static struct Cyc_List_List*Cyc_Tcutil_copy_types(struct Cyc_List_List*ts){return((
struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Tcutil_copy_type,ts);}static union Cyc_Absyn_Constraint*Cyc_Tcutil_copy_conref(
union Cyc_Absyn_Constraint*cptr);static union Cyc_Absyn_Constraint*Cyc_Tcutil_copy_conref(
union Cyc_Absyn_Constraint*cptr){union Cyc_Absyn_Constraint*_tmp4A=cptr;union Cyc_Absyn_Constraint
_tmp4B;int _tmp4C;union Cyc_Absyn_Constraint _tmp4D;void*_tmp4E;union Cyc_Absyn_Constraint
_tmp4F;union Cyc_Absyn_Constraint*_tmp50;_LLC: _tmp4B=*_tmp4A;if((_tmp4B.No_constr).tag
!= 3)goto _LLE;_tmp4C=(int)(_tmp4B.No_constr).val;_LLD: return Cyc_Absyn_empty_conref();
_LLE: _tmp4D=*_tmp4A;if((_tmp4D.Eq_constr).tag != 1)goto _LL10;_tmp4E=(void*)(
_tmp4D.Eq_constr).val;_LLF: return Cyc_Absyn_new_conref(_tmp4E);_LL10: _tmp4F=*
_tmp4A;if((_tmp4F.Forward_constr).tag != 2)goto _LLB;_tmp50=(union Cyc_Absyn_Constraint*)(
_tmp4F.Forward_constr).val;_LL11: return Cyc_Tcutil_copy_conref(_tmp50);_LLB:;}
static void*Cyc_Tcutil_copy_kindbound(void*kb);static void*Cyc_Tcutil_copy_kindbound(
void*kb){void*_tmp51=Cyc_Absyn_compress_kb(kb);enum Cyc_Absyn_Kind _tmp54;_LL13: {
struct Cyc_Absyn_Unknown_kb_struct*_tmp52=(struct Cyc_Absyn_Unknown_kb_struct*)
_tmp51;if(_tmp52->tag != 1)goto _LL15;}_LL14: {struct Cyc_Absyn_Unknown_kb_struct
_tmpEAF;struct Cyc_Absyn_Unknown_kb_struct*_tmpEAE;return(void*)((_tmpEAE=
_cycalloc(sizeof(*_tmpEAE)),((_tmpEAE[0]=((_tmpEAF.tag=1,((_tmpEAF.f1=0,_tmpEAF)))),
_tmpEAE))));}_LL15: {struct Cyc_Absyn_Less_kb_struct*_tmp53=(struct Cyc_Absyn_Less_kb_struct*)
_tmp51;if(_tmp53->tag != 2)goto _LL17;else{_tmp54=_tmp53->f2;}}_LL16: {struct Cyc_Absyn_Less_kb_struct
_tmpEB2;struct Cyc_Absyn_Less_kb_struct*_tmpEB1;return(void*)((_tmpEB1=_cycalloc(
sizeof(*_tmpEB1)),((_tmpEB1[0]=((_tmpEB2.tag=2,((_tmpEB2.f1=0,((_tmpEB2.f2=
_tmp54,_tmpEB2)))))),_tmpEB1))));}_LL17:;_LL18: return kb;_LL12:;}static struct Cyc_Absyn_Tvar*
Cyc_Tcutil_copy_tvar(struct Cyc_Absyn_Tvar*tv);static struct Cyc_Absyn_Tvar*Cyc_Tcutil_copy_tvar(
struct Cyc_Absyn_Tvar*tv){struct Cyc_Absyn_Tvar*_tmpEB3;return(_tmpEB3=_cycalloc(
sizeof(*_tmpEB3)),((_tmpEB3->name=tv->name,((_tmpEB3->identity=- 1,((_tmpEB3->kind=
Cyc_Tcutil_copy_kindbound(tv->kind),_tmpEB3)))))));}static struct _tuple9*Cyc_Tcutil_copy_arg(
struct _tuple9*arg);static struct _tuple9*Cyc_Tcutil_copy_arg(struct _tuple9*arg){
struct _tuple9 _tmp5B;struct Cyc_Core_Opt*_tmp5C;struct Cyc_Absyn_Tqual _tmp5D;void*
_tmp5E;struct _tuple9*_tmp5A=arg;_tmp5B=*_tmp5A;_tmp5C=_tmp5B.f1;_tmp5D=_tmp5B.f2;
_tmp5E=_tmp5B.f3;{struct _tuple9*_tmpEB4;return(_tmpEB4=_cycalloc(sizeof(*_tmpEB4)),((
_tmpEB4->f1=_tmp5C,((_tmpEB4->f2=_tmp5D,((_tmpEB4->f3=Cyc_Tcutil_copy_type(
_tmp5E),_tmpEB4)))))));}}static struct _tuple11*Cyc_Tcutil_copy_tqt(struct _tuple11*
arg);static struct _tuple11*Cyc_Tcutil_copy_tqt(struct _tuple11*arg){struct _tuple11
_tmp61;struct Cyc_Absyn_Tqual _tmp62;void*_tmp63;struct _tuple11*_tmp60=arg;_tmp61=*
_tmp60;_tmp62=_tmp61.f1;_tmp63=_tmp61.f2;{struct _tuple11*_tmpEB5;return(_tmpEB5=
_cycalloc(sizeof(*_tmpEB5)),((_tmpEB5->f1=_tmp62,((_tmpEB5->f2=Cyc_Tcutil_copy_type(
_tmp63),_tmpEB5)))));}}static struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_copy_field(
struct Cyc_Absyn_Aggrfield*f);static struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_copy_field(
struct Cyc_Absyn_Aggrfield*f){struct Cyc_Absyn_Aggrfield*_tmpEB6;return(_tmpEB6=
_cycalloc(sizeof(*_tmpEB6)),((_tmpEB6->name=f->name,((_tmpEB6->tq=f->tq,((
_tmpEB6->type=Cyc_Tcutil_copy_type(f->type),((_tmpEB6->width=f->width,((_tmpEB6->attributes=
f->attributes,_tmpEB6)))))))))));}static struct _tuple0*Cyc_Tcutil_copy_rgncmp(
struct _tuple0*x);static struct _tuple0*Cyc_Tcutil_copy_rgncmp(struct _tuple0*x){
struct _tuple0 _tmp67;void*_tmp68;void*_tmp69;struct _tuple0*_tmp66=x;_tmp67=*
_tmp66;_tmp68=_tmp67.f1;_tmp69=_tmp67.f2;{struct _tuple0*_tmpEB7;return(_tmpEB7=
_cycalloc(sizeof(*_tmpEB7)),((_tmpEB7->f1=Cyc_Tcutil_copy_type(_tmp68),((_tmpEB7->f2=
Cyc_Tcutil_copy_type(_tmp69),_tmpEB7)))));}}static struct Cyc_Absyn_Enumfield*Cyc_Tcutil_copy_enumfield(
struct Cyc_Absyn_Enumfield*f);static struct Cyc_Absyn_Enumfield*Cyc_Tcutil_copy_enumfield(
struct Cyc_Absyn_Enumfield*f){struct Cyc_Absyn_Enumfield*_tmpEB8;return(_tmpEB8=
_cycalloc(sizeof(*_tmpEB8)),((_tmpEB8->name=f->name,((_tmpEB8->tag=f->tag,((
_tmpEB8->loc=f->loc,_tmpEB8)))))));}void*Cyc_Tcutil_copy_type(void*t);void*Cyc_Tcutil_copy_type(
void*t){void*_tmp6C=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*_tmp70;struct Cyc_Absyn_DatatypeInfo
_tmp72;union Cyc_Absyn_DatatypeInfoU _tmp73;struct Cyc_List_List*_tmp74;struct Cyc_Absyn_DatatypeFieldInfo
_tmp76;union Cyc_Absyn_DatatypeFieldInfoU _tmp77;struct Cyc_List_List*_tmp78;struct
Cyc_Absyn_PtrInfo _tmp7A;void*_tmp7B;struct Cyc_Absyn_Tqual _tmp7C;struct Cyc_Absyn_PtrAtts
_tmp7D;void*_tmp7E;union Cyc_Absyn_Constraint*_tmp7F;union Cyc_Absyn_Constraint*
_tmp80;union Cyc_Absyn_Constraint*_tmp81;struct Cyc_Absyn_PtrLoc*_tmp82;struct Cyc_Absyn_ArrayInfo
_tmp87;void*_tmp88;struct Cyc_Absyn_Tqual _tmp89;struct Cyc_Absyn_Exp*_tmp8A;union
Cyc_Absyn_Constraint*_tmp8B;struct Cyc_Position_Segment*_tmp8C;struct Cyc_Absyn_FnInfo
_tmp8E;struct Cyc_List_List*_tmp8F;struct Cyc_Core_Opt*_tmp90;void*_tmp91;struct
Cyc_List_List*_tmp92;int _tmp93;struct Cyc_Absyn_VarargInfo*_tmp94;struct Cyc_List_List*
_tmp95;struct Cyc_List_List*_tmp96;struct Cyc_List_List*_tmp98;struct Cyc_Absyn_AggrInfo
_tmp9A;union Cyc_Absyn_AggrInfoU _tmp9B;struct _tuple4 _tmp9C;enum Cyc_Absyn_AggrKind
_tmp9D;struct _tuple2*_tmp9E;struct Cyc_Core_Opt*_tmp9F;struct Cyc_List_List*_tmpA0;
struct Cyc_Absyn_AggrInfo _tmpA2;union Cyc_Absyn_AggrInfoU _tmpA3;struct Cyc_Absyn_Aggrdecl**
_tmpA4;struct Cyc_List_List*_tmpA5;enum Cyc_Absyn_AggrKind _tmpA7;struct Cyc_List_List*
_tmpA8;struct _tuple2*_tmpAA;struct Cyc_Absyn_Enumdecl*_tmpAB;struct Cyc_List_List*
_tmpAD;void*_tmpAF;struct Cyc_Absyn_Exp*_tmpB1;void*_tmpB3;void*_tmpB5;void*
_tmpB6;struct _tuple2*_tmpB8;struct Cyc_List_List*_tmpB9;struct Cyc_Absyn_Typedefdecl*
_tmpBA;void*_tmpBE;struct Cyc_List_List*_tmpC0;void*_tmpC2;_LL1A: {struct Cyc_Absyn_VoidType_struct*
_tmp6D=(struct Cyc_Absyn_VoidType_struct*)_tmp6C;if(_tmp6D->tag != 0)goto _LL1C;}
_LL1B: goto _LL1D;_LL1C: {struct Cyc_Absyn_Evar_struct*_tmp6E=(struct Cyc_Absyn_Evar_struct*)
_tmp6C;if(_tmp6E->tag != 1)goto _LL1E;}_LL1D: return t;_LL1E: {struct Cyc_Absyn_VarType_struct*
_tmp6F=(struct Cyc_Absyn_VarType_struct*)_tmp6C;if(_tmp6F->tag != 2)goto _LL20;
else{_tmp70=_tmp6F->f1;}}_LL1F: {struct Cyc_Absyn_VarType_struct _tmpEBB;struct Cyc_Absyn_VarType_struct*
_tmpEBA;return(void*)((_tmpEBA=_cycalloc(sizeof(*_tmpEBA)),((_tmpEBA[0]=((
_tmpEBB.tag=2,((_tmpEBB.f1=Cyc_Tcutil_copy_tvar(_tmp70),_tmpEBB)))),_tmpEBA))));}
_LL20: {struct Cyc_Absyn_DatatypeType_struct*_tmp71=(struct Cyc_Absyn_DatatypeType_struct*)
_tmp6C;if(_tmp71->tag != 3)goto _LL22;else{_tmp72=_tmp71->f1;_tmp73=_tmp72.datatype_info;
_tmp74=_tmp72.targs;}}_LL21: {struct Cyc_Absyn_DatatypeType_struct _tmpEC1;struct
Cyc_Absyn_DatatypeInfo _tmpEC0;struct Cyc_Absyn_DatatypeType_struct*_tmpEBF;return(
void*)((_tmpEBF=_cycalloc(sizeof(*_tmpEBF)),((_tmpEBF[0]=((_tmpEC1.tag=3,((
_tmpEC1.f1=((_tmpEC0.datatype_info=_tmp73,((_tmpEC0.targs=Cyc_Tcutil_copy_types(
_tmp74),_tmpEC0)))),_tmpEC1)))),_tmpEBF))));}_LL22: {struct Cyc_Absyn_DatatypeFieldType_struct*
_tmp75=(struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp6C;if(_tmp75->tag != 4)goto
_LL24;else{_tmp76=_tmp75->f1;_tmp77=_tmp76.field_info;_tmp78=_tmp76.targs;}}
_LL23: {struct Cyc_Absyn_DatatypeFieldType_struct _tmpEC7;struct Cyc_Absyn_DatatypeFieldInfo
_tmpEC6;struct Cyc_Absyn_DatatypeFieldType_struct*_tmpEC5;return(void*)((_tmpEC5=
_cycalloc(sizeof(*_tmpEC5)),((_tmpEC5[0]=((_tmpEC7.tag=4,((_tmpEC7.f1=((_tmpEC6.field_info=
_tmp77,((_tmpEC6.targs=Cyc_Tcutil_copy_types(_tmp78),_tmpEC6)))),_tmpEC7)))),
_tmpEC5))));}_LL24: {struct Cyc_Absyn_PointerType_struct*_tmp79=(struct Cyc_Absyn_PointerType_struct*)
_tmp6C;if(_tmp79->tag != 5)goto _LL26;else{_tmp7A=_tmp79->f1;_tmp7B=_tmp7A.elt_typ;
_tmp7C=_tmp7A.elt_tq;_tmp7D=_tmp7A.ptr_atts;_tmp7E=_tmp7D.rgn;_tmp7F=_tmp7D.nullable;
_tmp80=_tmp7D.bounds;_tmp81=_tmp7D.zero_term;_tmp82=_tmp7D.ptrloc;}}_LL25: {void*
_tmpCB=Cyc_Tcutil_copy_type(_tmp7B);void*_tmpCC=Cyc_Tcutil_copy_type(_tmp7E);
union Cyc_Absyn_Constraint*_tmpCD=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*
cptr))Cyc_Tcutil_copy_conref)(_tmp7F);struct Cyc_Absyn_Tqual _tmpCE=_tmp7C;union
Cyc_Absyn_Constraint*_tmpCF=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*
cptr))Cyc_Tcutil_copy_conref)(_tmp80);union Cyc_Absyn_Constraint*_tmpD0=((union
Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(
_tmp81);struct Cyc_Absyn_PointerType_struct _tmpED1;struct Cyc_Absyn_PtrAtts _tmpED0;
struct Cyc_Absyn_PtrInfo _tmpECF;struct Cyc_Absyn_PointerType_struct*_tmpECE;return(
void*)((_tmpECE=_cycalloc(sizeof(*_tmpECE)),((_tmpECE[0]=((_tmpED1.tag=5,((
_tmpED1.f1=((_tmpECF.elt_typ=_tmpCB,((_tmpECF.elt_tq=_tmpCE,((_tmpECF.ptr_atts=((
_tmpED0.rgn=_tmpCC,((_tmpED0.nullable=_tmpCD,((_tmpED0.bounds=_tmpCF,((_tmpED0.zero_term=
_tmpD0,((_tmpED0.ptrloc=_tmp82,_tmpED0)))))))))),_tmpECF)))))),_tmpED1)))),
_tmpECE))));}_LL26: {struct Cyc_Absyn_IntType_struct*_tmp83=(struct Cyc_Absyn_IntType_struct*)
_tmp6C;if(_tmp83->tag != 6)goto _LL28;}_LL27: goto _LL29;_LL28: {struct Cyc_Absyn_FloatType_struct*
_tmp84=(struct Cyc_Absyn_FloatType_struct*)_tmp6C;if(_tmp84->tag != 7)goto _LL2A;}
_LL29: goto _LL2B;_LL2A: {struct Cyc_Absyn_DoubleType_struct*_tmp85=(struct Cyc_Absyn_DoubleType_struct*)
_tmp6C;if(_tmp85->tag != 8)goto _LL2C;}_LL2B: return t;_LL2C: {struct Cyc_Absyn_ArrayType_struct*
_tmp86=(struct Cyc_Absyn_ArrayType_struct*)_tmp6C;if(_tmp86->tag != 9)goto _LL2E;
else{_tmp87=_tmp86->f1;_tmp88=_tmp87.elt_type;_tmp89=_tmp87.tq;_tmp8A=_tmp87.num_elts;
_tmp8B=_tmp87.zero_term;_tmp8C=_tmp87.zt_loc;}}_LL2D: {struct Cyc_Absyn_ArrayType_struct
_tmpED7;struct Cyc_Absyn_ArrayInfo _tmpED6;struct Cyc_Absyn_ArrayType_struct*
_tmpED5;return(void*)((_tmpED5=_cycalloc(sizeof(*_tmpED5)),((_tmpED5[0]=((
_tmpED7.tag=9,((_tmpED7.f1=((_tmpED6.elt_type=Cyc_Tcutil_copy_type(_tmp88),((
_tmpED6.tq=_tmp89,((_tmpED6.num_elts=_tmp8A,((_tmpED6.zero_term=((union Cyc_Absyn_Constraint*(*)(
union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(_tmp8B),((_tmpED6.zt_loc=
_tmp8C,_tmpED6)))))))))),_tmpED7)))),_tmpED5))));}_LL2E: {struct Cyc_Absyn_FnType_struct*
_tmp8D=(struct Cyc_Absyn_FnType_struct*)_tmp6C;if(_tmp8D->tag != 10)goto _LL30;
else{_tmp8E=_tmp8D->f1;_tmp8F=_tmp8E.tvars;_tmp90=_tmp8E.effect;_tmp91=_tmp8E.ret_typ;
_tmp92=_tmp8E.args;_tmp93=_tmp8E.c_varargs;_tmp94=_tmp8E.cyc_varargs;_tmp95=
_tmp8E.rgn_po;_tmp96=_tmp8E.attributes;}}_LL2F: {struct Cyc_List_List*_tmpD8=((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Absyn_Tvar*),struct
Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_tvar,_tmp8F);struct Cyc_Core_Opt*
_tmpED8;struct Cyc_Core_Opt*_tmpD9=_tmp90 == 0?0:((_tmpED8=_cycalloc(sizeof(*
_tmpED8)),((_tmpED8->v=Cyc_Tcutil_copy_type((void*)_tmp90->v),_tmpED8))));void*
_tmpDA=Cyc_Tcutil_copy_type(_tmp91);struct Cyc_List_List*_tmpDB=((struct Cyc_List_List*(*)(
struct _tuple9*(*f)(struct _tuple9*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_arg,
_tmp92);int _tmpDC=_tmp93;struct Cyc_Absyn_VarargInfo*cyc_varargs2=0;if(_tmp94 != 0){
struct Cyc_Absyn_VarargInfo*cv=(struct Cyc_Absyn_VarargInfo*)_tmp94;struct Cyc_Absyn_VarargInfo*
_tmpED9;cyc_varargs2=((_tmpED9=_cycalloc(sizeof(*_tmpED9)),((_tmpED9->name=cv->name,((
_tmpED9->tq=cv->tq,((_tmpED9->type=Cyc_Tcutil_copy_type(cv->type),((_tmpED9->inject=
cv->inject,_tmpED9))))))))));}{struct Cyc_List_List*_tmpDE=((struct Cyc_List_List*(*)(
struct _tuple0*(*f)(struct _tuple0*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_rgncmp,
_tmp95);struct Cyc_List_List*_tmpDF=_tmp96;struct Cyc_Absyn_FnType_struct _tmpEDF;
struct Cyc_Absyn_FnInfo _tmpEDE;struct Cyc_Absyn_FnType_struct*_tmpEDD;return(void*)((
_tmpEDD=_cycalloc(sizeof(*_tmpEDD)),((_tmpEDD[0]=((_tmpEDF.tag=10,((_tmpEDF.f1=((
_tmpEDE.tvars=_tmpD8,((_tmpEDE.effect=_tmpD9,((_tmpEDE.ret_typ=_tmpDA,((_tmpEDE.args=
_tmpDB,((_tmpEDE.c_varargs=_tmpDC,((_tmpEDE.cyc_varargs=cyc_varargs2,((_tmpEDE.rgn_po=
_tmpDE,((_tmpEDE.attributes=_tmpDF,_tmpEDE)))))))))))))))),_tmpEDF)))),_tmpEDD))));}}
_LL30: {struct Cyc_Absyn_TupleType_struct*_tmp97=(struct Cyc_Absyn_TupleType_struct*)
_tmp6C;if(_tmp97->tag != 11)goto _LL32;else{_tmp98=_tmp97->f1;}}_LL31: {struct Cyc_Absyn_TupleType_struct
_tmpEE2;struct Cyc_Absyn_TupleType_struct*_tmpEE1;return(void*)((_tmpEE1=
_cycalloc(sizeof(*_tmpEE1)),((_tmpEE1[0]=((_tmpEE2.tag=11,((_tmpEE2.f1=((struct
Cyc_List_List*(*)(struct _tuple11*(*f)(struct _tuple11*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Tcutil_copy_tqt,_tmp98),_tmpEE2)))),_tmpEE1))));}_LL32: {
struct Cyc_Absyn_AggrType_struct*_tmp99=(struct Cyc_Absyn_AggrType_struct*)_tmp6C;
if(_tmp99->tag != 12)goto _LL34;else{_tmp9A=_tmp99->f1;_tmp9B=_tmp9A.aggr_info;if((
_tmp9B.UnknownAggr).tag != 1)goto _LL34;_tmp9C=(struct _tuple4)(_tmp9B.UnknownAggr).val;
_tmp9D=_tmp9C.f1;_tmp9E=_tmp9C.f2;_tmp9F=_tmp9C.f3;_tmpA0=_tmp9A.targs;}}_LL33: {
struct Cyc_Absyn_AggrType_struct _tmpEE8;struct Cyc_Absyn_AggrInfo _tmpEE7;struct Cyc_Absyn_AggrType_struct*
_tmpEE6;return(void*)((_tmpEE6=_cycalloc(sizeof(*_tmpEE6)),((_tmpEE6[0]=((
_tmpEE8.tag=12,((_tmpEE8.f1=((_tmpEE7.aggr_info=Cyc_Absyn_UnknownAggr(_tmp9D,
_tmp9E,_tmp9F),((_tmpEE7.targs=Cyc_Tcutil_copy_types(_tmpA0),_tmpEE7)))),_tmpEE8)))),
_tmpEE6))));}_LL34: {struct Cyc_Absyn_AggrType_struct*_tmpA1=(struct Cyc_Absyn_AggrType_struct*)
_tmp6C;if(_tmpA1->tag != 12)goto _LL36;else{_tmpA2=_tmpA1->f1;_tmpA3=_tmpA2.aggr_info;
if((_tmpA3.KnownAggr).tag != 2)goto _LL36;_tmpA4=(struct Cyc_Absyn_Aggrdecl**)(
_tmpA3.KnownAggr).val;_tmpA5=_tmpA2.targs;}}_LL35: {struct Cyc_Absyn_AggrType_struct
_tmpEEE;struct Cyc_Absyn_AggrInfo _tmpEED;struct Cyc_Absyn_AggrType_struct*_tmpEEC;
return(void*)((_tmpEEC=_cycalloc(sizeof(*_tmpEEC)),((_tmpEEC[0]=((_tmpEEE.tag=12,((
_tmpEEE.f1=((_tmpEED.aggr_info=Cyc_Absyn_KnownAggr(_tmpA4),((_tmpEED.targs=Cyc_Tcutil_copy_types(
_tmpA5),_tmpEED)))),_tmpEEE)))),_tmpEEC))));}_LL36: {struct Cyc_Absyn_AnonAggrType_struct*
_tmpA6=(struct Cyc_Absyn_AnonAggrType_struct*)_tmp6C;if(_tmpA6->tag != 13)goto
_LL38;else{_tmpA7=_tmpA6->f1;_tmpA8=_tmpA6->f2;}}_LL37: {struct Cyc_Absyn_AnonAggrType_struct
_tmpEF1;struct Cyc_Absyn_AnonAggrType_struct*_tmpEF0;return(void*)((_tmpEF0=
_cycalloc(sizeof(*_tmpEF0)),((_tmpEF0[0]=((_tmpEF1.tag=13,((_tmpEF1.f1=_tmpA7,((
_tmpEF1.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_field,_tmpA8),_tmpEF1)))))),
_tmpEF0))));}_LL38: {struct Cyc_Absyn_EnumType_struct*_tmpA9=(struct Cyc_Absyn_EnumType_struct*)
_tmp6C;if(_tmpA9->tag != 14)goto _LL3A;else{_tmpAA=_tmpA9->f1;_tmpAB=_tmpA9->f2;}}
_LL39: {struct Cyc_Absyn_EnumType_struct _tmpEF4;struct Cyc_Absyn_EnumType_struct*
_tmpEF3;return(void*)((_tmpEF3=_cycalloc(sizeof(*_tmpEF3)),((_tmpEF3[0]=((
_tmpEF4.tag=14,((_tmpEF4.f1=_tmpAA,((_tmpEF4.f2=_tmpAB,_tmpEF4)))))),_tmpEF3))));}
_LL3A: {struct Cyc_Absyn_AnonEnumType_struct*_tmpAC=(struct Cyc_Absyn_AnonEnumType_struct*)
_tmp6C;if(_tmpAC->tag != 15)goto _LL3C;else{_tmpAD=_tmpAC->f1;}}_LL3B: {struct Cyc_Absyn_AnonEnumType_struct
_tmpEF7;struct Cyc_Absyn_AnonEnumType_struct*_tmpEF6;return(void*)((_tmpEF6=
_cycalloc(sizeof(*_tmpEF6)),((_tmpEF6[0]=((_tmpEF7.tag=15,((_tmpEF7.f1=((struct
Cyc_List_List*(*)(struct Cyc_Absyn_Enumfield*(*f)(struct Cyc_Absyn_Enumfield*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_enumfield,_tmpAD),_tmpEF7)))),
_tmpEF6))));}_LL3C: {struct Cyc_Absyn_TagType_struct*_tmpAE=(struct Cyc_Absyn_TagType_struct*)
_tmp6C;if(_tmpAE->tag != 20)goto _LL3E;else{_tmpAF=(void*)_tmpAE->f1;}}_LL3D: {
struct Cyc_Absyn_TagType_struct _tmpEFA;struct Cyc_Absyn_TagType_struct*_tmpEF9;
return(void*)((_tmpEF9=_cycalloc(sizeof(*_tmpEF9)),((_tmpEF9[0]=((_tmpEFA.tag=20,((
_tmpEFA.f1=(void*)Cyc_Tcutil_copy_type(_tmpAF),_tmpEFA)))),_tmpEF9))));}_LL3E: {
struct Cyc_Absyn_ValueofType_struct*_tmpB0=(struct Cyc_Absyn_ValueofType_struct*)
_tmp6C;if(_tmpB0->tag != 19)goto _LL40;else{_tmpB1=_tmpB0->f1;}}_LL3F: {struct Cyc_Absyn_ValueofType_struct
_tmpEFD;struct Cyc_Absyn_ValueofType_struct*_tmpEFC;return(void*)((_tmpEFC=
_cycalloc(sizeof(*_tmpEFC)),((_tmpEFC[0]=((_tmpEFD.tag=19,((_tmpEFD.f1=_tmpB1,
_tmpEFD)))),_tmpEFC))));}_LL40: {struct Cyc_Absyn_RgnHandleType_struct*_tmpB2=(
struct Cyc_Absyn_RgnHandleType_struct*)_tmp6C;if(_tmpB2->tag != 16)goto _LL42;else{
_tmpB3=(void*)_tmpB2->f1;}}_LL41: {struct Cyc_Absyn_RgnHandleType_struct _tmpF00;
struct Cyc_Absyn_RgnHandleType_struct*_tmpEFF;return(void*)((_tmpEFF=_cycalloc(
sizeof(*_tmpEFF)),((_tmpEFF[0]=((_tmpF00.tag=16,((_tmpF00.f1=(void*)Cyc_Tcutil_copy_type(
_tmpB3),_tmpF00)))),_tmpEFF))));}_LL42: {struct Cyc_Absyn_DynRgnType_struct*
_tmpB4=(struct Cyc_Absyn_DynRgnType_struct*)_tmp6C;if(_tmpB4->tag != 17)goto _LL44;
else{_tmpB5=(void*)_tmpB4->f1;_tmpB6=(void*)_tmpB4->f2;}}_LL43: {struct Cyc_Absyn_DynRgnType_struct
_tmpF03;struct Cyc_Absyn_DynRgnType_struct*_tmpF02;return(void*)((_tmpF02=
_cycalloc(sizeof(*_tmpF02)),((_tmpF02[0]=((_tmpF03.tag=17,((_tmpF03.f1=(void*)
Cyc_Tcutil_copy_type(_tmpB5),((_tmpF03.f2=(void*)Cyc_Tcutil_copy_type(_tmpB6),
_tmpF03)))))),_tmpF02))));}_LL44: {struct Cyc_Absyn_TypedefType_struct*_tmpB7=(
struct Cyc_Absyn_TypedefType_struct*)_tmp6C;if(_tmpB7->tag != 18)goto _LL46;else{
_tmpB8=_tmpB7->f1;_tmpB9=_tmpB7->f2;_tmpBA=_tmpB7->f3;}}_LL45: {struct Cyc_Absyn_TypedefType_struct
_tmpF06;struct Cyc_Absyn_TypedefType_struct*_tmpF05;return(void*)((_tmpF05=
_cycalloc(sizeof(*_tmpF05)),((_tmpF05[0]=((_tmpF06.tag=18,((_tmpF06.f1=_tmpB8,((
_tmpF06.f2=Cyc_Tcutil_copy_types(_tmpB9),((_tmpF06.f3=_tmpBA,((_tmpF06.f4=0,
_tmpF06)))))))))),_tmpF05))));}_LL46: {struct Cyc_Absyn_UniqueRgn_struct*_tmpBB=(
struct Cyc_Absyn_UniqueRgn_struct*)_tmp6C;if(_tmpBB->tag != 22)goto _LL48;}_LL47:
goto _LL49;_LL48: {struct Cyc_Absyn_HeapRgn_struct*_tmpBC=(struct Cyc_Absyn_HeapRgn_struct*)
_tmp6C;if(_tmpBC->tag != 21)goto _LL4A;}_LL49: return t;_LL4A: {struct Cyc_Absyn_AccessEff_struct*
_tmpBD=(struct Cyc_Absyn_AccessEff_struct*)_tmp6C;if(_tmpBD->tag != 23)goto _LL4C;
else{_tmpBE=(void*)_tmpBD->f1;}}_LL4B: {struct Cyc_Absyn_AccessEff_struct _tmpF09;
struct Cyc_Absyn_AccessEff_struct*_tmpF08;return(void*)((_tmpF08=_cycalloc(
sizeof(*_tmpF08)),((_tmpF08[0]=((_tmpF09.tag=23,((_tmpF09.f1=(void*)Cyc_Tcutil_copy_type(
_tmpBE),_tmpF09)))),_tmpF08))));}_LL4C: {struct Cyc_Absyn_JoinEff_struct*_tmpBF=(
struct Cyc_Absyn_JoinEff_struct*)_tmp6C;if(_tmpBF->tag != 24)goto _LL4E;else{_tmpC0=
_tmpBF->f1;}}_LL4D: {struct Cyc_Absyn_JoinEff_struct _tmpF0C;struct Cyc_Absyn_JoinEff_struct*
_tmpF0B;return(void*)((_tmpF0B=_cycalloc(sizeof(*_tmpF0B)),((_tmpF0B[0]=((
_tmpF0C.tag=24,((_tmpF0C.f1=Cyc_Tcutil_copy_types(_tmpC0),_tmpF0C)))),_tmpF0B))));}
_LL4E: {struct Cyc_Absyn_RgnsEff_struct*_tmpC1=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp6C;if(_tmpC1->tag != 25)goto _LL19;else{_tmpC2=(void*)_tmpC1->f1;}}_LL4F: {
struct Cyc_Absyn_RgnsEff_struct _tmpF0F;struct Cyc_Absyn_RgnsEff_struct*_tmpF0E;
return(void*)((_tmpF0E=_cycalloc(sizeof(*_tmpF0E)),((_tmpF0E[0]=((_tmpF0F.tag=25,((
_tmpF0F.f1=(void*)Cyc_Tcutil_copy_type(_tmpC2),_tmpF0F)))),_tmpF0E))));}_LL19:;}
struct _tuple17{enum Cyc_Absyn_Kind f1;enum Cyc_Absyn_Kind f2;};int Cyc_Tcutil_kind_leq(
enum Cyc_Absyn_Kind k1,enum Cyc_Absyn_Kind k2);int Cyc_Tcutil_kind_leq(enum Cyc_Absyn_Kind
k1,enum Cyc_Absyn_Kind k2){if(k1 == k2)return 1;{struct _tuple17 _tmpF10;struct
_tuple17 _tmp103=(_tmpF10.f1=k1,((_tmpF10.f2=k2,_tmpF10)));enum Cyc_Absyn_Kind
_tmp104;enum Cyc_Absyn_Kind _tmp105;enum Cyc_Absyn_Kind _tmp106;enum Cyc_Absyn_Kind
_tmp107;enum Cyc_Absyn_Kind _tmp108;enum Cyc_Absyn_Kind _tmp109;enum Cyc_Absyn_Kind
_tmp10A;enum Cyc_Absyn_Kind _tmp10B;enum Cyc_Absyn_Kind _tmp10C;enum Cyc_Absyn_Kind
_tmp10D;_LL51: _tmp104=_tmp103.f1;if(_tmp104 != Cyc_Absyn_BoxKind)goto _LL53;
_tmp105=_tmp103.f2;if(_tmp105 != Cyc_Absyn_MemKind)goto _LL53;_LL52: goto _LL54;
_LL53: _tmp106=_tmp103.f1;if(_tmp106 != Cyc_Absyn_BoxKind)goto _LL55;_tmp107=
_tmp103.f2;if(_tmp107 != Cyc_Absyn_AnyKind)goto _LL55;_LL54: goto _LL56;_LL55:
_tmp108=_tmp103.f1;if(_tmp108 != Cyc_Absyn_MemKind)goto _LL57;_tmp109=_tmp103.f2;
if(_tmp109 != Cyc_Absyn_AnyKind)goto _LL57;_LL56: goto _LL58;_LL57: _tmp10A=_tmp103.f1;
if(_tmp10A != Cyc_Absyn_RgnKind)goto _LL59;_tmp10B=_tmp103.f2;if(_tmp10B != Cyc_Absyn_TopRgnKind)
goto _LL59;_LL58: goto _LL5A;_LL59: _tmp10C=_tmp103.f1;if(_tmp10C != Cyc_Absyn_UniqueRgnKind)
goto _LL5B;_tmp10D=_tmp103.f2;if(_tmp10D != Cyc_Absyn_TopRgnKind)goto _LL5B;_LL5A:
return 1;_LL5B:;_LL5C: return 0;_LL50:;}}static int Cyc_Tcutil_is_region_kind(enum 
Cyc_Absyn_Kind k);static int Cyc_Tcutil_is_region_kind(enum Cyc_Absyn_Kind k){
switch(k){case Cyc_Absyn_RgnKind: _LL5D: goto _LL5E;case Cyc_Absyn_TopRgnKind: _LL5E:
goto _LL5F;case Cyc_Absyn_UniqueRgnKind: _LL5F: return 1;default: _LL60: return 0;}}
enum Cyc_Absyn_Kind Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*tv,enum Cyc_Absyn_Kind
def);enum Cyc_Absyn_Kind Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*tv,enum Cyc_Absyn_Kind
def){void*_tmp10E=Cyc_Absyn_compress_kb(tv->kind);enum Cyc_Absyn_Kind _tmp110;
enum Cyc_Absyn_Kind _tmp112;_LL63: {struct Cyc_Absyn_Eq_kb_struct*_tmp10F=(struct
Cyc_Absyn_Eq_kb_struct*)_tmp10E;if(_tmp10F->tag != 0)goto _LL65;else{_tmp110=
_tmp10F->f1;}}_LL64: return _tmp110;_LL65: {struct Cyc_Absyn_Less_kb_struct*_tmp111=(
struct Cyc_Absyn_Less_kb_struct*)_tmp10E;if(_tmp111->tag != 2)goto _LL67;else{
_tmp112=_tmp111->f2;}}_LL66: return _tmp112;_LL67:;_LL68:{struct Cyc_Absyn_Less_kb_struct
_tmpF13;struct Cyc_Absyn_Less_kb_struct*_tmpF12;tv->kind=(void*)((_tmpF12=
_cycalloc(sizeof(*_tmpF12)),((_tmpF12[0]=((_tmpF13.tag=2,((_tmpF13.f1=0,((
_tmpF13.f2=def,_tmpF13)))))),_tmpF12))));}return def;_LL62:;}int Cyc_Tcutil_unify_kindbound(
void*kb1,void*kb2);int Cyc_Tcutil_unify_kindbound(void*kb1,void*kb2){struct
_tuple0 _tmpF14;struct _tuple0 _tmp116=(_tmpF14.f1=Cyc_Absyn_compress_kb(kb1),((
_tmpF14.f2=Cyc_Absyn_compress_kb(kb2),_tmpF14)));void*_tmp117;enum Cyc_Absyn_Kind
_tmp119;void*_tmp11A;enum Cyc_Absyn_Kind _tmp11C;void*_tmp11D;struct Cyc_Core_Opt*
_tmp11F;struct Cyc_Core_Opt**_tmp120;enum Cyc_Absyn_Kind _tmp121;void*_tmp122;
enum Cyc_Absyn_Kind _tmp124;void*_tmp125;enum Cyc_Absyn_Kind _tmp127;void*_tmp128;
struct Cyc_Core_Opt*_tmp12A;struct Cyc_Core_Opt**_tmp12B;enum Cyc_Absyn_Kind
_tmp12C;void*_tmp12D;struct Cyc_Core_Opt*_tmp12F;struct Cyc_Core_Opt**_tmp130;
enum Cyc_Absyn_Kind _tmp131;void*_tmp132;struct Cyc_Core_Opt*_tmp134;struct Cyc_Core_Opt**
_tmp135;enum Cyc_Absyn_Kind _tmp136;void*_tmp137;struct Cyc_Core_Opt*_tmp139;
struct Cyc_Core_Opt**_tmp13A;void*_tmp13B;void*_tmp13C;void*_tmp13D;struct Cyc_Core_Opt*
_tmp13F;struct Cyc_Core_Opt**_tmp140;_LL6A: _tmp117=_tmp116.f1;{struct Cyc_Absyn_Eq_kb_struct*
_tmp118=(struct Cyc_Absyn_Eq_kb_struct*)_tmp117;if(_tmp118->tag != 0)goto _LL6C;
else{_tmp119=_tmp118->f1;}}_tmp11A=_tmp116.f2;{struct Cyc_Absyn_Eq_kb_struct*
_tmp11B=(struct Cyc_Absyn_Eq_kb_struct*)_tmp11A;if(_tmp11B->tag != 0)goto _LL6C;
else{_tmp11C=_tmp11B->f1;}}_LL6B: return _tmp119 == _tmp11C;_LL6C: _tmp11D=_tmp116.f1;{
struct Cyc_Absyn_Less_kb_struct*_tmp11E=(struct Cyc_Absyn_Less_kb_struct*)_tmp11D;
if(_tmp11E->tag != 2)goto _LL6E;else{_tmp11F=_tmp11E->f1;_tmp120=(struct Cyc_Core_Opt**)&
_tmp11E->f1;_tmp121=_tmp11E->f2;}}_tmp122=_tmp116.f2;{struct Cyc_Absyn_Eq_kb_struct*
_tmp123=(struct Cyc_Absyn_Eq_kb_struct*)_tmp122;if(_tmp123->tag != 0)goto _LL6E;
else{_tmp124=_tmp123->f1;}}_LL6D: if(Cyc_Tcutil_kind_leq(_tmp124,_tmp121)){{void**
_tmpF15;*_tmp120=(struct Cyc_Core_Opt*)((_tmpF15=_cycalloc(sizeof(*_tmpF15)),((
_tmpF15[0]=kb2,_tmpF15))));}return 1;}else{return 0;}_LL6E: _tmp125=_tmp116.f1;{
struct Cyc_Absyn_Eq_kb_struct*_tmp126=(struct Cyc_Absyn_Eq_kb_struct*)_tmp125;if(
_tmp126->tag != 0)goto _LL70;else{_tmp127=_tmp126->f1;}}_tmp128=_tmp116.f2;{struct
Cyc_Absyn_Less_kb_struct*_tmp129=(struct Cyc_Absyn_Less_kb_struct*)_tmp128;if(
_tmp129->tag != 2)goto _LL70;else{_tmp12A=_tmp129->f1;_tmp12B=(struct Cyc_Core_Opt**)&
_tmp129->f1;_tmp12C=_tmp129->f2;}}_LL6F: if(Cyc_Tcutil_kind_leq(_tmp127,_tmp12C)){{
void**_tmpF16;*_tmp12B=(struct Cyc_Core_Opt*)((_tmpF16=_cycalloc(sizeof(*_tmpF16)),((
_tmpF16[0]=kb1,_tmpF16))));}return 1;}else{return 0;}_LL70: _tmp12D=_tmp116.f1;{
struct Cyc_Absyn_Less_kb_struct*_tmp12E=(struct Cyc_Absyn_Less_kb_struct*)_tmp12D;
if(_tmp12E->tag != 2)goto _LL72;else{_tmp12F=_tmp12E->f1;_tmp130=(struct Cyc_Core_Opt**)&
_tmp12E->f1;_tmp131=_tmp12E->f2;}}_tmp132=_tmp116.f2;{struct Cyc_Absyn_Less_kb_struct*
_tmp133=(struct Cyc_Absyn_Less_kb_struct*)_tmp132;if(_tmp133->tag != 2)goto _LL72;
else{_tmp134=_tmp133->f1;_tmp135=(struct Cyc_Core_Opt**)& _tmp133->f1;_tmp136=
_tmp133->f2;}}_LL71: if(Cyc_Tcutil_kind_leq(_tmp131,_tmp136)){{void**_tmpF17;*
_tmp135=(struct Cyc_Core_Opt*)((_tmpF17=_cycalloc(sizeof(*_tmpF17)),((_tmpF17[0]=
kb1,_tmpF17))));}return 1;}else{if(Cyc_Tcutil_kind_leq(_tmp136,_tmp131)){{void**
_tmpF18;*_tmp130=(struct Cyc_Core_Opt*)((_tmpF18=_cycalloc(sizeof(*_tmpF18)),((
_tmpF18[0]=kb2,_tmpF18))));}return 1;}else{return 0;}}_LL72: _tmp137=_tmp116.f1;{
struct Cyc_Absyn_Unknown_kb_struct*_tmp138=(struct Cyc_Absyn_Unknown_kb_struct*)
_tmp137;if(_tmp138->tag != 1)goto _LL74;else{_tmp139=_tmp138->f1;_tmp13A=(struct
Cyc_Core_Opt**)& _tmp138->f1;}}_tmp13B=_tmp116.f2;_LL73: _tmp13C=_tmp13B;_tmp140=
_tmp13A;goto _LL75;_LL74: _tmp13C=_tmp116.f1;_tmp13D=_tmp116.f2;{struct Cyc_Absyn_Unknown_kb_struct*
_tmp13E=(struct Cyc_Absyn_Unknown_kb_struct*)_tmp13D;if(_tmp13E->tag != 1)goto
_LL69;else{_tmp13F=_tmp13E->f1;_tmp140=(struct Cyc_Core_Opt**)& _tmp13E->f1;}}
_LL75:{void**_tmpF19;*_tmp140=(struct Cyc_Core_Opt*)((_tmpF19=_cycalloc(sizeof(*
_tmpF19)),((_tmpF19[0]=_tmp13C,_tmpF19))));}return 1;_LL69:;}struct _tuple14 Cyc_Tcutil_swap_kind(
void*t,void*kb);struct _tuple14 Cyc_Tcutil_swap_kind(void*t,void*kb){void*_tmp146=
Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*_tmp148;_LL77: {struct Cyc_Absyn_VarType_struct*
_tmp147=(struct Cyc_Absyn_VarType_struct*)_tmp146;if(_tmp147->tag != 2)goto _LL79;
else{_tmp148=_tmp147->f1;}}_LL78: {void*_tmp149=_tmp148->kind;_tmp148->kind=kb;{
struct _tuple14 _tmpF1A;return(_tmpF1A.f1=_tmp148,((_tmpF1A.f2=_tmp149,_tmpF1A)));}}
_LL79:;_LL7A: {const char*_tmpF1E;void*_tmpF1D[1];struct Cyc_String_pa_struct
_tmpF1C;(_tmpF1C.tag=0,((_tmpF1C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmpF1D[0]=& _tmpF1C,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpF1E="swap_kind: cannot update the kind of %s",
_tag_dyneither(_tmpF1E,sizeof(char),40))),_tag_dyneither(_tmpF1D,sizeof(void*),1)))))));}
_LL76:;}enum Cyc_Absyn_Kind Cyc_Tcutil_typ_kind(void*t);enum Cyc_Absyn_Kind Cyc_Tcutil_typ_kind(
void*t){void*_tmp14E=Cyc_Tcutil_compress(t);struct Cyc_Core_Opt*_tmp150;struct Cyc_Absyn_Tvar*
_tmp152;enum Cyc_Absyn_Size_of _tmp155;struct Cyc_Absyn_DatatypeFieldInfo _tmp161;
union Cyc_Absyn_DatatypeFieldInfoU _tmp162;struct _tuple3 _tmp163;struct Cyc_Absyn_Datatypedecl*
_tmp164;struct Cyc_Absyn_Datatypefield*_tmp165;struct Cyc_Absyn_DatatypeFieldInfo
_tmp167;union Cyc_Absyn_DatatypeFieldInfoU _tmp168;struct Cyc_Absyn_UnknownDatatypeFieldInfo
_tmp169;struct Cyc_Absyn_AggrInfo _tmp16B;union Cyc_Absyn_AggrInfoU _tmp16C;struct
_tuple4 _tmp16D;struct Cyc_Absyn_AggrInfo _tmp16F;union Cyc_Absyn_AggrInfoU _tmp170;
struct Cyc_Absyn_Aggrdecl**_tmp171;struct Cyc_Absyn_Aggrdecl*_tmp172;struct Cyc_Absyn_Aggrdecl
_tmp173;struct Cyc_Absyn_AggrdeclImpl*_tmp174;struct Cyc_Absyn_PtrInfo _tmp177;
struct Cyc_Absyn_Typedefdecl*_tmp17D;_LL7C: {struct Cyc_Absyn_Evar_struct*_tmp14F=(
struct Cyc_Absyn_Evar_struct*)_tmp14E;if(_tmp14F->tag != 1)goto _LL7E;else{_tmp150=
_tmp14F->f1;}}_LL7D: return(enum Cyc_Absyn_Kind)((struct Cyc_Core_Opt*)_check_null(
_tmp150))->v;_LL7E: {struct Cyc_Absyn_VarType_struct*_tmp151=(struct Cyc_Absyn_VarType_struct*)
_tmp14E;if(_tmp151->tag != 2)goto _LL80;else{_tmp152=_tmp151->f1;}}_LL7F: return Cyc_Tcutil_tvar_kind(
_tmp152,Cyc_Absyn_BoxKind);_LL80: {struct Cyc_Absyn_VoidType_struct*_tmp153=(
struct Cyc_Absyn_VoidType_struct*)_tmp14E;if(_tmp153->tag != 0)goto _LL82;}_LL81:
return Cyc_Absyn_MemKind;_LL82: {struct Cyc_Absyn_IntType_struct*_tmp154=(struct
Cyc_Absyn_IntType_struct*)_tmp14E;if(_tmp154->tag != 6)goto _LL84;else{_tmp155=
_tmp154->f2;}}_LL83: return(_tmp155 == (enum Cyc_Absyn_Size_of)Cyc_Absyn_Int_sz
 || _tmp155 == (enum Cyc_Absyn_Size_of)Cyc_Absyn_Long_sz)?Cyc_Absyn_BoxKind: Cyc_Absyn_MemKind;
_LL84: {struct Cyc_Absyn_FloatType_struct*_tmp156=(struct Cyc_Absyn_FloatType_struct*)
_tmp14E;if(_tmp156->tag != 7)goto _LL86;}_LL85: goto _LL87;_LL86: {struct Cyc_Absyn_DoubleType_struct*
_tmp157=(struct Cyc_Absyn_DoubleType_struct*)_tmp14E;if(_tmp157->tag != 8)goto
_LL88;}_LL87: goto _LL89;_LL88: {struct Cyc_Absyn_FnType_struct*_tmp158=(struct Cyc_Absyn_FnType_struct*)
_tmp14E;if(_tmp158->tag != 10)goto _LL8A;}_LL89: return Cyc_Absyn_MemKind;_LL8A: {
struct Cyc_Absyn_DynRgnType_struct*_tmp159=(struct Cyc_Absyn_DynRgnType_struct*)
_tmp14E;if(_tmp159->tag != 17)goto _LL8C;}_LL8B: goto _LL8D;_LL8C: {struct Cyc_Absyn_EnumType_struct*
_tmp15A=(struct Cyc_Absyn_EnumType_struct*)_tmp14E;if(_tmp15A->tag != 14)goto _LL8E;}
_LL8D: goto _LL8F;_LL8E: {struct Cyc_Absyn_AnonEnumType_struct*_tmp15B=(struct Cyc_Absyn_AnonEnumType_struct*)
_tmp14E;if(_tmp15B->tag != 15)goto _LL90;}_LL8F: goto _LL91;_LL90: {struct Cyc_Absyn_RgnHandleType_struct*
_tmp15C=(struct Cyc_Absyn_RgnHandleType_struct*)_tmp14E;if(_tmp15C->tag != 16)goto
_LL92;}_LL91: return Cyc_Absyn_BoxKind;_LL92: {struct Cyc_Absyn_UniqueRgn_struct*
_tmp15D=(struct Cyc_Absyn_UniqueRgn_struct*)_tmp14E;if(_tmp15D->tag != 22)goto
_LL94;}_LL93: return Cyc_Absyn_UniqueRgnKind;_LL94: {struct Cyc_Absyn_HeapRgn_struct*
_tmp15E=(struct Cyc_Absyn_HeapRgn_struct*)_tmp14E;if(_tmp15E->tag != 21)goto _LL96;}
_LL95: return Cyc_Absyn_RgnKind;_LL96: {struct Cyc_Absyn_DatatypeType_struct*
_tmp15F=(struct Cyc_Absyn_DatatypeType_struct*)_tmp14E;if(_tmp15F->tag != 3)goto
_LL98;}_LL97: return Cyc_Absyn_AnyKind;_LL98: {struct Cyc_Absyn_DatatypeFieldType_struct*
_tmp160=(struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp14E;if(_tmp160->tag != 4)
goto _LL9A;else{_tmp161=_tmp160->f1;_tmp162=_tmp161.field_info;if((_tmp162.KnownDatatypefield).tag
!= 2)goto _LL9A;_tmp163=(struct _tuple3)(_tmp162.KnownDatatypefield).val;_tmp164=
_tmp163.f1;_tmp165=_tmp163.f2;}}_LL99: return Cyc_Absyn_MemKind;_LL9A: {struct Cyc_Absyn_DatatypeFieldType_struct*
_tmp166=(struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp14E;if(_tmp166->tag != 4)
goto _LL9C;else{_tmp167=_tmp166->f1;_tmp168=_tmp167.field_info;if((_tmp168.UnknownDatatypefield).tag
!= 1)goto _LL9C;_tmp169=(struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmp168.UnknownDatatypefield).val;}}
_LL9B: {const char*_tmpF21;void*_tmpF20;(_tmpF20=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpF21="typ_kind: Unresolved DatatypeFieldType",
_tag_dyneither(_tmpF21,sizeof(char),39))),_tag_dyneither(_tmpF20,sizeof(void*),0)));}
_LL9C: {struct Cyc_Absyn_AggrType_struct*_tmp16A=(struct Cyc_Absyn_AggrType_struct*)
_tmp14E;if(_tmp16A->tag != 12)goto _LL9E;else{_tmp16B=_tmp16A->f1;_tmp16C=_tmp16B.aggr_info;
if((_tmp16C.UnknownAggr).tag != 1)goto _LL9E;_tmp16D=(struct _tuple4)(_tmp16C.UnknownAggr).val;}}
_LL9D: return Cyc_Absyn_AnyKind;_LL9E: {struct Cyc_Absyn_AggrType_struct*_tmp16E=(
struct Cyc_Absyn_AggrType_struct*)_tmp14E;if(_tmp16E->tag != 12)goto _LLA0;else{
_tmp16F=_tmp16E->f1;_tmp170=_tmp16F.aggr_info;if((_tmp170.KnownAggr).tag != 2)
goto _LLA0;_tmp171=(struct Cyc_Absyn_Aggrdecl**)(_tmp170.KnownAggr).val;_tmp172=*
_tmp171;_tmp173=*_tmp172;_tmp174=_tmp173.impl;}}_LL9F: return _tmp174 == 0?Cyc_Absyn_AnyKind:
Cyc_Absyn_MemKind;_LLA0: {struct Cyc_Absyn_AnonAggrType_struct*_tmp175=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp14E;if(_tmp175->tag != 13)goto _LLA2;}_LLA1: return Cyc_Absyn_MemKind;_LLA2: {
struct Cyc_Absyn_PointerType_struct*_tmp176=(struct Cyc_Absyn_PointerType_struct*)
_tmp14E;if(_tmp176->tag != 5)goto _LLA4;else{_tmp177=_tmp176->f1;}}_LLA3: {void*
_tmp183=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((
void*)& Cyc_Absyn_DynEither_b_val,(_tmp177.ptr_atts).bounds);_LLB5: {struct Cyc_Absyn_DynEither_b_struct*
_tmp184=(struct Cyc_Absyn_DynEither_b_struct*)_tmp183;if(_tmp184->tag != 0)goto
_LLB7;}_LLB6: return Cyc_Absyn_MemKind;_LLB7: {struct Cyc_Absyn_Upper_b_struct*
_tmp185=(struct Cyc_Absyn_Upper_b_struct*)_tmp183;if(_tmp185->tag != 1)goto _LLB4;}
_LLB8: return Cyc_Absyn_BoxKind;_LLB4:;}_LLA4: {struct Cyc_Absyn_ValueofType_struct*
_tmp178=(struct Cyc_Absyn_ValueofType_struct*)_tmp14E;if(_tmp178->tag != 19)goto
_LLA6;}_LLA5: return Cyc_Absyn_IntKind;_LLA6: {struct Cyc_Absyn_TagType_struct*
_tmp179=(struct Cyc_Absyn_TagType_struct*)_tmp14E;if(_tmp179->tag != 20)goto _LLA8;}
_LLA7: return Cyc_Absyn_BoxKind;_LLA8: {struct Cyc_Absyn_ArrayType_struct*_tmp17A=(
struct Cyc_Absyn_ArrayType_struct*)_tmp14E;if(_tmp17A->tag != 9)goto _LLAA;}_LLA9:
goto _LLAB;_LLAA: {struct Cyc_Absyn_TupleType_struct*_tmp17B=(struct Cyc_Absyn_TupleType_struct*)
_tmp14E;if(_tmp17B->tag != 11)goto _LLAC;}_LLAB: return Cyc_Absyn_MemKind;_LLAC: {
struct Cyc_Absyn_TypedefType_struct*_tmp17C=(struct Cyc_Absyn_TypedefType_struct*)
_tmp14E;if(_tmp17C->tag != 18)goto _LLAE;else{_tmp17D=_tmp17C->f3;}}_LLAD: if(
_tmp17D == 0  || _tmp17D->kind == 0){const char*_tmpF25;void*_tmpF24[1];struct Cyc_String_pa_struct
_tmpF23;(_tmpF23.tag=0,((_tmpF23.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmpF24[0]=& _tmpF23,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpF25="typ_kind: typedef found: %s",
_tag_dyneither(_tmpF25,sizeof(char),28))),_tag_dyneither(_tmpF24,sizeof(void*),1)))))));}
return(enum Cyc_Absyn_Kind)((struct Cyc_Core_Opt*)_check_null(_tmp17D->kind))->v;
_LLAE: {struct Cyc_Absyn_AccessEff_struct*_tmp17E=(struct Cyc_Absyn_AccessEff_struct*)
_tmp14E;if(_tmp17E->tag != 23)goto _LLB0;}_LLAF: goto _LLB1;_LLB0: {struct Cyc_Absyn_JoinEff_struct*
_tmp17F=(struct Cyc_Absyn_JoinEff_struct*)_tmp14E;if(_tmp17F->tag != 24)goto _LLB2;}
_LLB1: goto _LLB3;_LLB2: {struct Cyc_Absyn_RgnsEff_struct*_tmp180=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp14E;if(_tmp180->tag != 25)goto _LL7B;}_LLB3: return Cyc_Absyn_EffKind;_LL7B:;}
int Cyc_Tcutil_unify(void*t1,void*t2);int Cyc_Tcutil_unify(void*t1,void*t2){struct
_handler_cons _tmp189;_push_handler(& _tmp189);{int _tmp18B=0;if(setjmp(_tmp189.handler))
_tmp18B=1;if(!_tmp18B){Cyc_Tcutil_unify_it(t1,t2);{int _tmp18C=1;_npop_handler(0);
return _tmp18C;};_pop_handler();}else{void*_tmp18A=(void*)_exn_thrown;void*
_tmp18E=_tmp18A;_LLBA: {struct Cyc_Tcutil_Unify_struct*_tmp18F=(struct Cyc_Tcutil_Unify_struct*)
_tmp18E;if(_tmp18F->tag != Cyc_Tcutil_Unify)goto _LLBC;}_LLBB: return 0;_LLBC:;_LLBD:(
void)_throw(_tmp18E);_LLB9:;}}}static void Cyc_Tcutil_occurslist(void*evar,struct
_RegionHandle*r,struct Cyc_List_List*env,struct Cyc_List_List*ts);static void Cyc_Tcutil_occurs(
void*evar,struct _RegionHandle*r,struct Cyc_List_List*env,void*t);static void Cyc_Tcutil_occurs(
void*evar,struct _RegionHandle*r,struct Cyc_List_List*env,void*t){t=Cyc_Tcutil_compress(
t);{void*_tmp190=t;struct Cyc_Absyn_Tvar*_tmp192;struct Cyc_Core_Opt*_tmp194;
struct Cyc_Core_Opt*_tmp195;struct Cyc_Core_Opt**_tmp196;struct Cyc_Absyn_PtrInfo
_tmp198;struct Cyc_Absyn_ArrayInfo _tmp19A;void*_tmp19B;struct Cyc_Absyn_FnInfo
_tmp19D;struct Cyc_List_List*_tmp19E;struct Cyc_Core_Opt*_tmp19F;void*_tmp1A0;
struct Cyc_List_List*_tmp1A1;int _tmp1A2;struct Cyc_Absyn_VarargInfo*_tmp1A3;struct
Cyc_List_List*_tmp1A4;struct Cyc_List_List*_tmp1A5;struct Cyc_List_List*_tmp1A7;
struct Cyc_Absyn_DatatypeInfo _tmp1A9;struct Cyc_List_List*_tmp1AA;struct Cyc_List_List*
_tmp1AC;struct Cyc_Absyn_DatatypeFieldInfo _tmp1AE;struct Cyc_List_List*_tmp1AF;
struct Cyc_Absyn_AggrInfo _tmp1B1;struct Cyc_List_List*_tmp1B2;struct Cyc_List_List*
_tmp1B4;void*_tmp1B6;void*_tmp1B8;void*_tmp1BA;void*_tmp1BC;struct Cyc_List_List*
_tmp1BE;_LLBF: {struct Cyc_Absyn_VarType_struct*_tmp191=(struct Cyc_Absyn_VarType_struct*)
_tmp190;if(_tmp191->tag != 2)goto _LLC1;else{_tmp192=_tmp191->f1;}}_LLC0: if(!((int(*)(
int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,
struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,_tmp192)){{
const char*_tmpF26;Cyc_Tcutil_failure_reason=((_tmpF26="(type variable would escape scope)",
_tag_dyneither(_tmpF26,sizeof(char),35)));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
goto _LLBE;_LLC1: {struct Cyc_Absyn_Evar_struct*_tmp193=(struct Cyc_Absyn_Evar_struct*)
_tmp190;if(_tmp193->tag != 1)goto _LLC3;else{_tmp194=_tmp193->f2;_tmp195=_tmp193->f4;
_tmp196=(struct Cyc_Core_Opt**)& _tmp193->f4;}}_LLC2: if(t == evar){{const char*
_tmpF27;Cyc_Tcutil_failure_reason=((_tmpF27="(occurs check)",_tag_dyneither(
_tmpF27,sizeof(char),15)));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}else{if(
_tmp194 != 0)Cyc_Tcutil_occurs(evar,r,env,(void*)_tmp194->v);else{int problem=0;{
struct Cyc_List_List*s=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*
_tmp196))->v;for(0;s != 0;s=s->tl){if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,
struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(
Cyc_Tcutil_fast_tvar_cmp,env,(struct Cyc_Absyn_Tvar*)s->hd)){problem=1;break;}}}
if(problem){struct Cyc_List_List*_tmp1C1=0;{struct Cyc_List_List*s=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(*_tmp196))->v;for(0;s != 0;s=s->tl){if(((int(*)(
int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,
struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,(struct Cyc_Absyn_Tvar*)
s->hd)){struct Cyc_List_List*_tmpF28;_tmp1C1=((_tmpF28=_cycalloc(sizeof(*_tmpF28)),((
_tmpF28->hd=(struct Cyc_Absyn_Tvar*)s->hd,((_tmpF28->tl=_tmp1C1,_tmpF28))))));}}}{
struct Cyc_Core_Opt*_tmpF29;*_tmp196=((_tmpF29=_cycalloc(sizeof(*_tmpF29)),((
_tmpF29->v=_tmp1C1,_tmpF29))));}}}}goto _LLBE;_LLC3: {struct Cyc_Absyn_PointerType_struct*
_tmp197=(struct Cyc_Absyn_PointerType_struct*)_tmp190;if(_tmp197->tag != 5)goto
_LLC5;else{_tmp198=_tmp197->f1;}}_LLC4: Cyc_Tcutil_occurs(evar,r,env,_tmp198.elt_typ);
Cyc_Tcutil_occurs(evar,r,env,(_tmp198.ptr_atts).rgn);goto _LLBE;_LLC5: {struct Cyc_Absyn_ArrayType_struct*
_tmp199=(struct Cyc_Absyn_ArrayType_struct*)_tmp190;if(_tmp199->tag != 9)goto _LLC7;
else{_tmp19A=_tmp199->f1;_tmp19B=_tmp19A.elt_type;}}_LLC6: Cyc_Tcutil_occurs(evar,
r,env,_tmp19B);goto _LLBE;_LLC7: {struct Cyc_Absyn_FnType_struct*_tmp19C=(struct
Cyc_Absyn_FnType_struct*)_tmp190;if(_tmp19C->tag != 10)goto _LLC9;else{_tmp19D=
_tmp19C->f1;_tmp19E=_tmp19D.tvars;_tmp19F=_tmp19D.effect;_tmp1A0=_tmp19D.ret_typ;
_tmp1A1=_tmp19D.args;_tmp1A2=_tmp19D.c_varargs;_tmp1A3=_tmp19D.cyc_varargs;
_tmp1A4=_tmp19D.rgn_po;_tmp1A5=_tmp19D.attributes;}}_LLC8: env=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(
r,_tmp19E,env);if(_tmp19F != 0)Cyc_Tcutil_occurs(evar,r,env,(void*)_tmp19F->v);
Cyc_Tcutil_occurs(evar,r,env,_tmp1A0);for(0;_tmp1A1 != 0;_tmp1A1=_tmp1A1->tl){Cyc_Tcutil_occurs(
evar,r,env,(*((struct _tuple9*)_tmp1A1->hd)).f3);}if(_tmp1A3 != 0)Cyc_Tcutil_occurs(
evar,r,env,_tmp1A3->type);for(0;_tmp1A4 != 0;_tmp1A4=_tmp1A4->tl){struct _tuple0
_tmp1C5;void*_tmp1C6;void*_tmp1C7;struct _tuple0*_tmp1C4=(struct _tuple0*)_tmp1A4->hd;
_tmp1C5=*_tmp1C4;_tmp1C6=_tmp1C5.f1;_tmp1C7=_tmp1C5.f2;Cyc_Tcutil_occurs(evar,r,
env,_tmp1C6);Cyc_Tcutil_occurs(evar,r,env,_tmp1C7);}goto _LLBE;_LLC9: {struct Cyc_Absyn_TupleType_struct*
_tmp1A6=(struct Cyc_Absyn_TupleType_struct*)_tmp190;if(_tmp1A6->tag != 11)goto
_LLCB;else{_tmp1A7=_tmp1A6->f1;}}_LLCA: for(0;_tmp1A7 != 0;_tmp1A7=_tmp1A7->tl){
Cyc_Tcutil_occurs(evar,r,env,(*((struct _tuple11*)_tmp1A7->hd)).f2);}goto _LLBE;
_LLCB: {struct Cyc_Absyn_DatatypeType_struct*_tmp1A8=(struct Cyc_Absyn_DatatypeType_struct*)
_tmp190;if(_tmp1A8->tag != 3)goto _LLCD;else{_tmp1A9=_tmp1A8->f1;_tmp1AA=_tmp1A9.targs;}}
_LLCC: Cyc_Tcutil_occurslist(evar,r,env,_tmp1AA);goto _LLBE;_LLCD: {struct Cyc_Absyn_TypedefType_struct*
_tmp1AB=(struct Cyc_Absyn_TypedefType_struct*)_tmp190;if(_tmp1AB->tag != 18)goto
_LLCF;else{_tmp1AC=_tmp1AB->f2;}}_LLCE: _tmp1AF=_tmp1AC;goto _LLD0;_LLCF: {struct
Cyc_Absyn_DatatypeFieldType_struct*_tmp1AD=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp190;if(_tmp1AD->tag != 4)goto _LLD1;else{_tmp1AE=_tmp1AD->f1;_tmp1AF=_tmp1AE.targs;}}
_LLD0: _tmp1B2=_tmp1AF;goto _LLD2;_LLD1: {struct Cyc_Absyn_AggrType_struct*_tmp1B0=(
struct Cyc_Absyn_AggrType_struct*)_tmp190;if(_tmp1B0->tag != 12)goto _LLD3;else{
_tmp1B1=_tmp1B0->f1;_tmp1B2=_tmp1B1.targs;}}_LLD2: Cyc_Tcutil_occurslist(evar,r,
env,_tmp1B2);goto _LLBE;_LLD3: {struct Cyc_Absyn_AnonAggrType_struct*_tmp1B3=(
struct Cyc_Absyn_AnonAggrType_struct*)_tmp190;if(_tmp1B3->tag != 13)goto _LLD5;
else{_tmp1B4=_tmp1B3->f2;}}_LLD4: for(0;_tmp1B4 != 0;_tmp1B4=_tmp1B4->tl){Cyc_Tcutil_occurs(
evar,r,env,((struct Cyc_Absyn_Aggrfield*)_tmp1B4->hd)->type);}goto _LLBE;_LLD5: {
struct Cyc_Absyn_TagType_struct*_tmp1B5=(struct Cyc_Absyn_TagType_struct*)_tmp190;
if(_tmp1B5->tag != 20)goto _LLD7;else{_tmp1B6=(void*)_tmp1B5->f1;}}_LLD6: _tmp1B8=
_tmp1B6;goto _LLD8;_LLD7: {struct Cyc_Absyn_AccessEff_struct*_tmp1B7=(struct Cyc_Absyn_AccessEff_struct*)
_tmp190;if(_tmp1B7->tag != 23)goto _LLD9;else{_tmp1B8=(void*)_tmp1B7->f1;}}_LLD8:
_tmp1BA=_tmp1B8;goto _LLDA;_LLD9: {struct Cyc_Absyn_RgnHandleType_struct*_tmp1B9=(
struct Cyc_Absyn_RgnHandleType_struct*)_tmp190;if(_tmp1B9->tag != 16)goto _LLDB;
else{_tmp1BA=(void*)_tmp1B9->f1;}}_LLDA: _tmp1BC=_tmp1BA;goto _LLDC;_LLDB: {struct
Cyc_Absyn_RgnsEff_struct*_tmp1BB=(struct Cyc_Absyn_RgnsEff_struct*)_tmp190;if(
_tmp1BB->tag != 25)goto _LLDD;else{_tmp1BC=(void*)_tmp1BB->f1;}}_LLDC: Cyc_Tcutil_occurs(
evar,r,env,_tmp1BC);goto _LLBE;_LLDD: {struct Cyc_Absyn_JoinEff_struct*_tmp1BD=(
struct Cyc_Absyn_JoinEff_struct*)_tmp190;if(_tmp1BD->tag != 24)goto _LLDF;else{
_tmp1BE=_tmp1BD->f1;}}_LLDE: Cyc_Tcutil_occurslist(evar,r,env,_tmp1BE);goto _LLBE;
_LLDF:;_LLE0: goto _LLBE;_LLBE:;}}static void Cyc_Tcutil_occurslist(void*evar,struct
_RegionHandle*r,struct Cyc_List_List*env,struct Cyc_List_List*ts);static void Cyc_Tcutil_occurslist(
void*evar,struct _RegionHandle*r,struct Cyc_List_List*env,struct Cyc_List_List*ts){
for(0;ts != 0;ts=ts->tl){Cyc_Tcutil_occurs(evar,r,env,(void*)ts->hd);}}static void
Cyc_Tcutil_unify_list(struct Cyc_List_List*t1,struct Cyc_List_List*t2);static void
Cyc_Tcutil_unify_list(struct Cyc_List_List*t1,struct Cyc_List_List*t2){for(0;t1 != 
0  && t2 != 0;(t1=t1->tl,t2=t2->tl)){Cyc_Tcutil_unify_it((void*)t1->hd,(void*)t2->hd);}
if(t1 != 0  || t2 != 0)(int)_throw((void*)& Cyc_Tcutil_Unify_val);}static void Cyc_Tcutil_unify_tqual(
struct Cyc_Absyn_Tqual tq1,void*t1,struct Cyc_Absyn_Tqual tq2,void*t2);static void Cyc_Tcutil_unify_tqual(
struct Cyc_Absyn_Tqual tq1,void*t1,struct Cyc_Absyn_Tqual tq2,void*t2){if(tq1.print_const
 && !tq1.real_const){const char*_tmpF2C;void*_tmpF2B;(_tmpF2B=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpF2C="tq1 real_const not set.",
_tag_dyneither(_tmpF2C,sizeof(char),24))),_tag_dyneither(_tmpF2B,sizeof(void*),0)));}
if(tq2.print_const  && !tq2.real_const){const char*_tmpF2F;void*_tmpF2E;(_tmpF2E=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmpF2F="tq2 real_const not set.",_tag_dyneither(_tmpF2F,sizeof(char),24))),
_tag_dyneither(_tmpF2E,sizeof(void*),0)));}if((tq1.real_const != tq2.real_const
 || tq1.q_volatile != tq2.q_volatile) || tq1.q_restrict != tq2.q_restrict){Cyc_Tcutil_t1_failure=
t1;Cyc_Tcutil_t2_failure=t2;Cyc_Tcutil_tq1_const=tq1.real_const;Cyc_Tcutil_tq2_const=
tq2.real_const;{const char*_tmpF30;Cyc_Tcutil_failure_reason=((_tmpF30="(qualifiers don't match)",
_tag_dyneither(_tmpF30,sizeof(char),25)));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
Cyc_Tcutil_tq1_const=0;Cyc_Tcutil_tq2_const=0;}int Cyc_Tcutil_equal_tqual(struct
Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2);int Cyc_Tcutil_equal_tqual(struct Cyc_Absyn_Tqual
tq1,struct Cyc_Absyn_Tqual tq2){return(tq1.real_const == tq2.real_const  && tq1.q_volatile
== tq2.q_volatile) && tq1.q_restrict == tq2.q_restrict;}static void Cyc_Tcutil_unify_it_conrefs(
int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,
struct _dyneither_ptr reason);static void Cyc_Tcutil_unify_it_conrefs(int(*cmp)(void*,
void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct
_dyneither_ptr reason){x=Cyc_Absyn_compress_conref(x);y=Cyc_Absyn_compress_conref(
y);if(x == y)return;{union Cyc_Absyn_Constraint*_tmp1CD=x;union Cyc_Absyn_Constraint
_tmp1CE;int _tmp1CF;union Cyc_Absyn_Constraint _tmp1D0;void*_tmp1D1;union Cyc_Absyn_Constraint
_tmp1D2;union Cyc_Absyn_Constraint*_tmp1D3;_LLE2: _tmp1CE=*_tmp1CD;if((_tmp1CE.No_constr).tag
!= 3)goto _LLE4;_tmp1CF=(int)(_tmp1CE.No_constr).val;_LLE3:{union Cyc_Absyn_Constraint
_tmpF31;*x=(((_tmpF31.Forward_constr).val=y,(((_tmpF31.Forward_constr).tag=2,
_tmpF31))));}return;_LLE4: _tmp1D0=*_tmp1CD;if((_tmp1D0.Eq_constr).tag != 1)goto
_LLE6;_tmp1D1=(void*)(_tmp1D0.Eq_constr).val;_LLE5: {union Cyc_Absyn_Constraint*
_tmp1D5=y;union Cyc_Absyn_Constraint _tmp1D6;int _tmp1D7;union Cyc_Absyn_Constraint
_tmp1D8;void*_tmp1D9;union Cyc_Absyn_Constraint _tmp1DA;union Cyc_Absyn_Constraint*
_tmp1DB;_LLE9: _tmp1D6=*_tmp1D5;if((_tmp1D6.No_constr).tag != 3)goto _LLEB;_tmp1D7=(
int)(_tmp1D6.No_constr).val;_LLEA:*y=*x;return;_LLEB: _tmp1D8=*_tmp1D5;if((
_tmp1D8.Eq_constr).tag != 1)goto _LLED;_tmp1D9=(void*)(_tmp1D8.Eq_constr).val;
_LLEC: if(cmp(_tmp1D1,_tmp1D9)!= 0){Cyc_Tcutil_failure_reason=reason;(int)_throw((
void*)& Cyc_Tcutil_Unify_val);}return;_LLED: _tmp1DA=*_tmp1D5;if((_tmp1DA.Forward_constr).tag
!= 2)goto _LLE8;_tmp1DB=(union Cyc_Absyn_Constraint*)(_tmp1DA.Forward_constr).val;
_LLEE: {const char*_tmpF34;void*_tmpF33;(_tmpF33=0,Cyc_Tcutil_impos(((_tmpF34="unify_conref: forward after compress(2)",
_tag_dyneither(_tmpF34,sizeof(char),40))),_tag_dyneither(_tmpF33,sizeof(void*),0)));}
_LLE8:;}_LLE6: _tmp1D2=*_tmp1CD;if((_tmp1D2.Forward_constr).tag != 2)goto _LLE1;
_tmp1D3=(union Cyc_Absyn_Constraint*)(_tmp1D2.Forward_constr).val;_LLE7: {const
char*_tmpF37;void*_tmpF36;(_tmpF36=0,Cyc_Tcutil_impos(((_tmpF37="unify_conref: forward after compress",
_tag_dyneither(_tmpF37,sizeof(char),37))),_tag_dyneither(_tmpF36,sizeof(void*),0)));}
_LLE1:;}}static int Cyc_Tcutil_unify_conrefs(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*
x,union Cyc_Absyn_Constraint*y);static int Cyc_Tcutil_unify_conrefs(int(*cmp)(void*,
void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y){struct
_handler_cons _tmp1E0;_push_handler(& _tmp1E0);{int _tmp1E2=0;if(setjmp(_tmp1E0.handler))
_tmp1E2=1;if(!_tmp1E2){Cyc_Tcutil_unify_it_conrefs(cmp,x,y,(struct _dyneither_ptr)
_tag_dyneither(0,0,0));{int _tmp1E3=1;_npop_handler(0);return _tmp1E3;};
_pop_handler();}else{void*_tmp1E1=(void*)_exn_thrown;void*_tmp1E5=_tmp1E1;_LLF0: {
struct Cyc_Tcutil_Unify_struct*_tmp1E6=(struct Cyc_Tcutil_Unify_struct*)_tmp1E5;
if(_tmp1E6->tag != Cyc_Tcutil_Unify)goto _LLF2;}_LLF1: return 0;_LLF2:;_LLF3:(void)
_throw(_tmp1E5);_LLEF:;}}}static int Cyc_Tcutil_boundscmp(void*b1,void*b2);static
int Cyc_Tcutil_boundscmp(void*b1,void*b2){struct _tuple0 _tmpF38;struct _tuple0
_tmp1E8=(_tmpF38.f1=b1,((_tmpF38.f2=b2,_tmpF38)));void*_tmp1E9;void*_tmp1EB;void*
_tmp1ED;void*_tmp1EF;void*_tmp1F1;struct Cyc_Absyn_Exp*_tmp1F3;void*_tmp1F4;
struct Cyc_Absyn_Exp*_tmp1F6;_LLF5: _tmp1E9=_tmp1E8.f1;{struct Cyc_Absyn_DynEither_b_struct*
_tmp1EA=(struct Cyc_Absyn_DynEither_b_struct*)_tmp1E9;if(_tmp1EA->tag != 0)goto
_LLF7;}_tmp1EB=_tmp1E8.f2;{struct Cyc_Absyn_DynEither_b_struct*_tmp1EC=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp1EB;if(_tmp1EC->tag != 0)goto _LLF7;}_LLF6: return 0;_LLF7: _tmp1ED=_tmp1E8.f1;{
struct Cyc_Absyn_DynEither_b_struct*_tmp1EE=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp1ED;if(_tmp1EE->tag != 0)goto _LLF9;}_LLF8: return - 1;_LLF9: _tmp1EF=_tmp1E8.f2;{
struct Cyc_Absyn_DynEither_b_struct*_tmp1F0=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp1EF;if(_tmp1F0->tag != 0)goto _LLFB;}_LLFA: return 1;_LLFB: _tmp1F1=_tmp1E8.f1;{
struct Cyc_Absyn_Upper_b_struct*_tmp1F2=(struct Cyc_Absyn_Upper_b_struct*)_tmp1F1;
if(_tmp1F2->tag != 1)goto _LLF4;else{_tmp1F3=_tmp1F2->f1;}}_tmp1F4=_tmp1E8.f2;{
struct Cyc_Absyn_Upper_b_struct*_tmp1F5=(struct Cyc_Absyn_Upper_b_struct*)_tmp1F4;
if(_tmp1F5->tag != 1)goto _LLF4;else{_tmp1F6=_tmp1F5->f1;}}_LLFC: return Cyc_Evexp_const_exp_cmp(
_tmp1F3,_tmp1F6);_LLF4:;}static int Cyc_Tcutil_unify_it_bounds(void*b1,void*b2);
static int Cyc_Tcutil_unify_it_bounds(void*b1,void*b2){struct _tuple0 _tmpF39;struct
_tuple0 _tmp1F8=(_tmpF39.f1=b1,((_tmpF39.f2=b2,_tmpF39)));void*_tmp1F9;void*
_tmp1FB;void*_tmp1FD;void*_tmp1FF;void*_tmp201;struct Cyc_Absyn_Exp*_tmp203;void*
_tmp204;struct Cyc_Absyn_Exp*_tmp206;_LLFE: _tmp1F9=_tmp1F8.f1;{struct Cyc_Absyn_DynEither_b_struct*
_tmp1FA=(struct Cyc_Absyn_DynEither_b_struct*)_tmp1F9;if(_tmp1FA->tag != 0)goto
_LL100;}_tmp1FB=_tmp1F8.f2;{struct Cyc_Absyn_DynEither_b_struct*_tmp1FC=(struct
Cyc_Absyn_DynEither_b_struct*)_tmp1FB;if(_tmp1FC->tag != 0)goto _LL100;}_LLFF:
return 0;_LL100: _tmp1FD=_tmp1F8.f1;{struct Cyc_Absyn_DynEither_b_struct*_tmp1FE=(
struct Cyc_Absyn_DynEither_b_struct*)_tmp1FD;if(_tmp1FE->tag != 0)goto _LL102;}
_LL101: return - 1;_LL102: _tmp1FF=_tmp1F8.f2;{struct Cyc_Absyn_DynEither_b_struct*
_tmp200=(struct Cyc_Absyn_DynEither_b_struct*)_tmp1FF;if(_tmp200->tag != 0)goto
_LL104;}_LL103: return 1;_LL104: _tmp201=_tmp1F8.f1;{struct Cyc_Absyn_Upper_b_struct*
_tmp202=(struct Cyc_Absyn_Upper_b_struct*)_tmp201;if(_tmp202->tag != 1)goto _LLFD;
else{_tmp203=_tmp202->f1;}}_tmp204=_tmp1F8.f2;{struct Cyc_Absyn_Upper_b_struct*
_tmp205=(struct Cyc_Absyn_Upper_b_struct*)_tmp204;if(_tmp205->tag != 1)goto _LLFD;
else{_tmp206=_tmp205->f1;}}_LL105: return Cyc_Evexp_const_exp_cmp(_tmp203,_tmp206);
_LLFD:;}static int Cyc_Tcutil_attribute_case_number(void*att);static int Cyc_Tcutil_attribute_case_number(
void*att){void*_tmp207=att;_LL107: {struct Cyc_Absyn_Regparm_att_struct*_tmp208=(
struct Cyc_Absyn_Regparm_att_struct*)_tmp207;if(_tmp208->tag != 0)goto _LL109;}
_LL108: return 0;_LL109: {struct Cyc_Absyn_Stdcall_att_struct*_tmp209=(struct Cyc_Absyn_Stdcall_att_struct*)
_tmp207;if(_tmp209->tag != 1)goto _LL10B;}_LL10A: return 1;_LL10B: {struct Cyc_Absyn_Cdecl_att_struct*
_tmp20A=(struct Cyc_Absyn_Cdecl_att_struct*)_tmp207;if(_tmp20A->tag != 2)goto
_LL10D;}_LL10C: return 2;_LL10D: {struct Cyc_Absyn_Fastcall_att_struct*_tmp20B=(
struct Cyc_Absyn_Fastcall_att_struct*)_tmp207;if(_tmp20B->tag != 3)goto _LL10F;}
_LL10E: return 3;_LL10F: {struct Cyc_Absyn_Noreturn_att_struct*_tmp20C=(struct Cyc_Absyn_Noreturn_att_struct*)
_tmp207;if(_tmp20C->tag != 4)goto _LL111;}_LL110: return 4;_LL111: {struct Cyc_Absyn_Const_att_struct*
_tmp20D=(struct Cyc_Absyn_Const_att_struct*)_tmp207;if(_tmp20D->tag != 5)goto
_LL113;}_LL112: return 5;_LL113: {struct Cyc_Absyn_Aligned_att_struct*_tmp20E=(
struct Cyc_Absyn_Aligned_att_struct*)_tmp207;if(_tmp20E->tag != 6)goto _LL115;}
_LL114: return 6;_LL115: {struct Cyc_Absyn_Packed_att_struct*_tmp20F=(struct Cyc_Absyn_Packed_att_struct*)
_tmp207;if(_tmp20F->tag != 7)goto _LL117;}_LL116: return 7;_LL117: {struct Cyc_Absyn_Section_att_struct*
_tmp210=(struct Cyc_Absyn_Section_att_struct*)_tmp207;if(_tmp210->tag != 8)goto
_LL119;}_LL118: return 8;_LL119: {struct Cyc_Absyn_Nocommon_att_struct*_tmp211=(
struct Cyc_Absyn_Nocommon_att_struct*)_tmp207;if(_tmp211->tag != 9)goto _LL11B;}
_LL11A: return 9;_LL11B: {struct Cyc_Absyn_Shared_att_struct*_tmp212=(struct Cyc_Absyn_Shared_att_struct*)
_tmp207;if(_tmp212->tag != 10)goto _LL11D;}_LL11C: return 10;_LL11D: {struct Cyc_Absyn_Unused_att_struct*
_tmp213=(struct Cyc_Absyn_Unused_att_struct*)_tmp207;if(_tmp213->tag != 11)goto
_LL11F;}_LL11E: return 11;_LL11F: {struct Cyc_Absyn_Weak_att_struct*_tmp214=(struct
Cyc_Absyn_Weak_att_struct*)_tmp207;if(_tmp214->tag != 12)goto _LL121;}_LL120:
return 12;_LL121: {struct Cyc_Absyn_Dllimport_att_struct*_tmp215=(struct Cyc_Absyn_Dllimport_att_struct*)
_tmp207;if(_tmp215->tag != 13)goto _LL123;}_LL122: return 13;_LL123: {struct Cyc_Absyn_Dllexport_att_struct*
_tmp216=(struct Cyc_Absyn_Dllexport_att_struct*)_tmp207;if(_tmp216->tag != 14)goto
_LL125;}_LL124: return 14;_LL125: {struct Cyc_Absyn_No_instrument_function_att_struct*
_tmp217=(struct Cyc_Absyn_No_instrument_function_att_struct*)_tmp207;if(_tmp217->tag
!= 15)goto _LL127;}_LL126: return 15;_LL127: {struct Cyc_Absyn_Constructor_att_struct*
_tmp218=(struct Cyc_Absyn_Constructor_att_struct*)_tmp207;if(_tmp218->tag != 16)
goto _LL129;}_LL128: return 16;_LL129: {struct Cyc_Absyn_Destructor_att_struct*
_tmp219=(struct Cyc_Absyn_Destructor_att_struct*)_tmp207;if(_tmp219->tag != 17)
goto _LL12B;}_LL12A: return 17;_LL12B: {struct Cyc_Absyn_No_check_memory_usage_att_struct*
_tmp21A=(struct Cyc_Absyn_No_check_memory_usage_att_struct*)_tmp207;if(_tmp21A->tag
!= 18)goto _LL12D;}_LL12C: return 18;_LL12D: {struct Cyc_Absyn_Format_att_struct*
_tmp21B=(struct Cyc_Absyn_Format_att_struct*)_tmp207;if(_tmp21B->tag != 19)goto
_LL12F;}_LL12E: return 19;_LL12F: {struct Cyc_Absyn_Initializes_att_struct*_tmp21C=(
struct Cyc_Absyn_Initializes_att_struct*)_tmp207;if(_tmp21C->tag != 20)goto _LL131;}
_LL130: return 20;_LL131:;_LL132: return 21;_LL106:;}static int Cyc_Tcutil_attribute_cmp(
void*att1,void*att2);static int Cyc_Tcutil_attribute_cmp(void*att1,void*att2){
struct _tuple0 _tmpF3A;struct _tuple0 _tmp21E=(_tmpF3A.f1=att1,((_tmpF3A.f2=att2,
_tmpF3A)));void*_tmp21F;int _tmp221;void*_tmp222;int _tmp224;void*_tmp225;int
_tmp227;void*_tmp228;int _tmp22A;void*_tmp22B;int _tmp22D;void*_tmp22E;int _tmp230;
void*_tmp231;struct _dyneither_ptr _tmp233;void*_tmp234;struct _dyneither_ptr
_tmp236;void*_tmp237;enum Cyc_Absyn_Format_Type _tmp239;int _tmp23A;int _tmp23B;
void*_tmp23C;enum Cyc_Absyn_Format_Type _tmp23E;int _tmp23F;int _tmp240;_LL134:
_tmp21F=_tmp21E.f1;{struct Cyc_Absyn_Regparm_att_struct*_tmp220=(struct Cyc_Absyn_Regparm_att_struct*)
_tmp21F;if(_tmp220->tag != 0)goto _LL136;else{_tmp221=_tmp220->f1;}}_tmp222=
_tmp21E.f2;{struct Cyc_Absyn_Regparm_att_struct*_tmp223=(struct Cyc_Absyn_Regparm_att_struct*)
_tmp222;if(_tmp223->tag != 0)goto _LL136;else{_tmp224=_tmp223->f1;}}_LL135: _tmp227=
_tmp221;_tmp22A=_tmp224;goto _LL137;_LL136: _tmp225=_tmp21E.f1;{struct Cyc_Absyn_Initializes_att_struct*
_tmp226=(struct Cyc_Absyn_Initializes_att_struct*)_tmp225;if(_tmp226->tag != 20)
goto _LL138;else{_tmp227=_tmp226->f1;}}_tmp228=_tmp21E.f2;{struct Cyc_Absyn_Initializes_att_struct*
_tmp229=(struct Cyc_Absyn_Initializes_att_struct*)_tmp228;if(_tmp229->tag != 20)
goto _LL138;else{_tmp22A=_tmp229->f1;}}_LL137: _tmp22D=_tmp227;_tmp230=_tmp22A;
goto _LL139;_LL138: _tmp22B=_tmp21E.f1;{struct Cyc_Absyn_Aligned_att_struct*_tmp22C=(
struct Cyc_Absyn_Aligned_att_struct*)_tmp22B;if(_tmp22C->tag != 6)goto _LL13A;else{
_tmp22D=_tmp22C->f1;}}_tmp22E=_tmp21E.f2;{struct Cyc_Absyn_Aligned_att_struct*
_tmp22F=(struct Cyc_Absyn_Aligned_att_struct*)_tmp22E;if(_tmp22F->tag != 6)goto
_LL13A;else{_tmp230=_tmp22F->f1;}}_LL139: return Cyc_Core_intcmp(_tmp22D,_tmp230);
_LL13A: _tmp231=_tmp21E.f1;{struct Cyc_Absyn_Section_att_struct*_tmp232=(struct Cyc_Absyn_Section_att_struct*)
_tmp231;if(_tmp232->tag != 8)goto _LL13C;else{_tmp233=_tmp232->f1;}}_tmp234=
_tmp21E.f2;{struct Cyc_Absyn_Section_att_struct*_tmp235=(struct Cyc_Absyn_Section_att_struct*)
_tmp234;if(_tmp235->tag != 8)goto _LL13C;else{_tmp236=_tmp235->f1;}}_LL13B: return
Cyc_strcmp((struct _dyneither_ptr)_tmp233,(struct _dyneither_ptr)_tmp236);_LL13C:
_tmp237=_tmp21E.f1;{struct Cyc_Absyn_Format_att_struct*_tmp238=(struct Cyc_Absyn_Format_att_struct*)
_tmp237;if(_tmp238->tag != 19)goto _LL13E;else{_tmp239=_tmp238->f1;_tmp23A=_tmp238->f2;
_tmp23B=_tmp238->f3;}}_tmp23C=_tmp21E.f2;{struct Cyc_Absyn_Format_att_struct*
_tmp23D=(struct Cyc_Absyn_Format_att_struct*)_tmp23C;if(_tmp23D->tag != 19)goto
_LL13E;else{_tmp23E=_tmp23D->f1;_tmp23F=_tmp23D->f2;_tmp240=_tmp23D->f3;}}_LL13D: {
int _tmp241=Cyc_Core_intcmp((int)((unsigned int)_tmp239),(int)((unsigned int)
_tmp23E));if(_tmp241 != 0)return _tmp241;{int _tmp242=Cyc_Core_intcmp(_tmp23A,
_tmp23F);if(_tmp242 != 0)return _tmp242;return Cyc_Core_intcmp(_tmp23B,_tmp240);}}
_LL13E:;_LL13F: return Cyc_Core_intcmp(Cyc_Tcutil_attribute_case_number(att1),Cyc_Tcutil_attribute_case_number(
att2));_LL133:;}static int Cyc_Tcutil_equal_att(void*a1,void*a2);static int Cyc_Tcutil_equal_att(
void*a1,void*a2){return Cyc_Tcutil_attribute_cmp(a1,a2)== 0;}int Cyc_Tcutil_same_atts(
struct Cyc_List_List*a1,struct Cyc_List_List*a2);int Cyc_Tcutil_same_atts(struct Cyc_List_List*
a1,struct Cyc_List_List*a2){{struct Cyc_List_List*a=a1;for(0;a != 0;a=a->tl){if(!((
int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(
Cyc_Tcutil_equal_att,(void*)a->hd,a2))return 0;}}{struct Cyc_List_List*a=a2;for(0;
a != 0;a=a->tl){if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*
x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)a->hd,a1))return 0;}}return 1;}
static void*Cyc_Tcutil_rgns_of(void*t);static void*Cyc_Tcutil_rgns_of_field(struct
Cyc_Absyn_Aggrfield*af);static void*Cyc_Tcutil_rgns_of_field(struct Cyc_Absyn_Aggrfield*
af){return Cyc_Tcutil_rgns_of(af->type);}static struct _tuple14*Cyc_Tcutil_region_free_subst(
struct Cyc_Absyn_Tvar*tv);static struct _tuple14*Cyc_Tcutil_region_free_subst(
struct Cyc_Absyn_Tvar*tv){void*t;switch(Cyc_Tcutil_tvar_kind(tv,Cyc_Absyn_BoxKind)){
case Cyc_Absyn_UniqueRgnKind: _LL140: t=(void*)& Cyc_Absyn_UniqueRgn_val;break;case
Cyc_Absyn_RgnKind: _LL141: t=(void*)& Cyc_Absyn_HeapRgn_val;break;case Cyc_Absyn_EffKind:
_LL142: t=Cyc_Absyn_empty_effect;break;case Cyc_Absyn_IntKind: _LL143:{struct Cyc_Absyn_ValueofType_struct
_tmpF3D;struct Cyc_Absyn_ValueofType_struct*_tmpF3C;t=(void*)((_tmpF3C=_cycalloc(
sizeof(*_tmpF3C)),((_tmpF3C[0]=((_tmpF3D.tag=19,((_tmpF3D.f1=Cyc_Absyn_uint_exp(
0,0),_tmpF3D)))),_tmpF3C))));}break;default: _LL144: t=Cyc_Absyn_sint_typ;break;}{
struct _tuple14*_tmpF3E;return(_tmpF3E=_cycalloc(sizeof(*_tmpF3E)),((_tmpF3E->f1=
tv,((_tmpF3E->f2=t,_tmpF3E)))));}}static void*Cyc_Tcutil_rgns_of(void*t);static
void*Cyc_Tcutil_rgns_of(void*t){void*_tmp246=Cyc_Tcutil_compress(t);void*_tmp251;
void*_tmp253;void*_tmp254;struct Cyc_Absyn_DatatypeInfo _tmp256;struct Cyc_List_List*
_tmp257;struct Cyc_Absyn_PtrInfo _tmp259;void*_tmp25A;struct Cyc_Absyn_PtrAtts
_tmp25B;void*_tmp25C;struct Cyc_Absyn_ArrayInfo _tmp25E;void*_tmp25F;struct Cyc_List_List*
_tmp261;struct Cyc_Absyn_DatatypeFieldInfo _tmp263;struct Cyc_List_List*_tmp264;
struct Cyc_Absyn_AggrInfo _tmp266;struct Cyc_List_List*_tmp267;struct Cyc_List_List*
_tmp269;struct Cyc_Absyn_FnInfo _tmp26C;struct Cyc_List_List*_tmp26D;struct Cyc_Core_Opt*
_tmp26E;void*_tmp26F;struct Cyc_List_List*_tmp270;struct Cyc_Absyn_VarargInfo*
_tmp271;struct Cyc_List_List*_tmp272;void*_tmp278;struct Cyc_List_List*_tmp27A;
_LL147: {struct Cyc_Absyn_VoidType_struct*_tmp247=(struct Cyc_Absyn_VoidType_struct*)
_tmp246;if(_tmp247->tag != 0)goto _LL149;}_LL148: goto _LL14A;_LL149: {struct Cyc_Absyn_FloatType_struct*
_tmp248=(struct Cyc_Absyn_FloatType_struct*)_tmp246;if(_tmp248->tag != 7)goto
_LL14B;}_LL14A: goto _LL14C;_LL14B: {struct Cyc_Absyn_DoubleType_struct*_tmp249=(
struct Cyc_Absyn_DoubleType_struct*)_tmp246;if(_tmp249->tag != 8)goto _LL14D;}
_LL14C: goto _LL14E;_LL14D: {struct Cyc_Absyn_EnumType_struct*_tmp24A=(struct Cyc_Absyn_EnumType_struct*)
_tmp246;if(_tmp24A->tag != 14)goto _LL14F;}_LL14E: goto _LL150;_LL14F: {struct Cyc_Absyn_AnonEnumType_struct*
_tmp24B=(struct Cyc_Absyn_AnonEnumType_struct*)_tmp246;if(_tmp24B->tag != 15)goto
_LL151;}_LL150: goto _LL152;_LL151: {struct Cyc_Absyn_ValueofType_struct*_tmp24C=(
struct Cyc_Absyn_ValueofType_struct*)_tmp246;if(_tmp24C->tag != 19)goto _LL153;}
_LL152: goto _LL154;_LL153: {struct Cyc_Absyn_IntType_struct*_tmp24D=(struct Cyc_Absyn_IntType_struct*)
_tmp246;if(_tmp24D->tag != 6)goto _LL155;}_LL154: return Cyc_Absyn_empty_effect;
_LL155: {struct Cyc_Absyn_Evar_struct*_tmp24E=(struct Cyc_Absyn_Evar_struct*)
_tmp246;if(_tmp24E->tag != 1)goto _LL157;}_LL156: goto _LL158;_LL157: {struct Cyc_Absyn_VarType_struct*
_tmp24F=(struct Cyc_Absyn_VarType_struct*)_tmp246;if(_tmp24F->tag != 2)goto _LL159;}
_LL158: switch(Cyc_Tcutil_typ_kind(t)){case Cyc_Absyn_RgnKind: _LL17B: goto _LL17C;
case Cyc_Absyn_UniqueRgnKind: _LL17C: goto _LL17D;case Cyc_Absyn_TopRgnKind: _LL17D: {
struct Cyc_Absyn_AccessEff_struct _tmpF41;struct Cyc_Absyn_AccessEff_struct*_tmpF40;
return(void*)((_tmpF40=_cycalloc(sizeof(*_tmpF40)),((_tmpF40[0]=((_tmpF41.tag=23,((
_tmpF41.f1=(void*)t,_tmpF41)))),_tmpF40))));}case Cyc_Absyn_EffKind: _LL17E: return
t;case Cyc_Absyn_IntKind: _LL17F: return Cyc_Absyn_empty_effect;default: _LL180: {
struct Cyc_Absyn_RgnsEff_struct _tmpF44;struct Cyc_Absyn_RgnsEff_struct*_tmpF43;
return(void*)((_tmpF43=_cycalloc(sizeof(*_tmpF43)),((_tmpF43[0]=((_tmpF44.tag=25,((
_tmpF44.f1=(void*)t,_tmpF44)))),_tmpF43))));}}_LL159: {struct Cyc_Absyn_RgnHandleType_struct*
_tmp250=(struct Cyc_Absyn_RgnHandleType_struct*)_tmp246;if(_tmp250->tag != 16)goto
_LL15B;else{_tmp251=(void*)_tmp250->f1;}}_LL15A: {struct Cyc_Absyn_AccessEff_struct
_tmpF47;struct Cyc_Absyn_AccessEff_struct*_tmpF46;return(void*)((_tmpF46=
_cycalloc(sizeof(*_tmpF46)),((_tmpF46[0]=((_tmpF47.tag=23,((_tmpF47.f1=(void*)
_tmp251,_tmpF47)))),_tmpF46))));}_LL15B: {struct Cyc_Absyn_DynRgnType_struct*
_tmp252=(struct Cyc_Absyn_DynRgnType_struct*)_tmp246;if(_tmp252->tag != 17)goto
_LL15D;else{_tmp253=(void*)_tmp252->f1;_tmp254=(void*)_tmp252->f2;}}_LL15C: {
struct Cyc_Absyn_AccessEff_struct _tmpF4A;struct Cyc_Absyn_AccessEff_struct*_tmpF49;
return(void*)((_tmpF49=_cycalloc(sizeof(*_tmpF49)),((_tmpF49[0]=((_tmpF4A.tag=23,((
_tmpF4A.f1=(void*)_tmp254,_tmpF4A)))),_tmpF49))));}_LL15D: {struct Cyc_Absyn_DatatypeType_struct*
_tmp255=(struct Cyc_Absyn_DatatypeType_struct*)_tmp246;if(_tmp255->tag != 3)goto
_LL15F;else{_tmp256=_tmp255->f1;_tmp257=_tmp256.targs;}}_LL15E: {struct Cyc_List_List*
ts=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Tcutil_rgns_of,_tmp257);struct Cyc_Absyn_JoinEff_struct _tmpF4D;struct Cyc_Absyn_JoinEff_struct*
_tmpF4C;return Cyc_Tcutil_normalize_effect((void*)((_tmpF4C=_cycalloc(sizeof(*
_tmpF4C)),((_tmpF4C[0]=((_tmpF4D.tag=24,((_tmpF4D.f1=ts,_tmpF4D)))),_tmpF4C)))));}
_LL15F: {struct Cyc_Absyn_PointerType_struct*_tmp258=(struct Cyc_Absyn_PointerType_struct*)
_tmp246;if(_tmp258->tag != 5)goto _LL161;else{_tmp259=_tmp258->f1;_tmp25A=_tmp259.elt_typ;
_tmp25B=_tmp259.ptr_atts;_tmp25C=_tmp25B.rgn;}}_LL160: {struct Cyc_Absyn_JoinEff_struct
_tmpF5C;struct Cyc_Absyn_AccessEff_struct*_tmpF5B;struct Cyc_Absyn_AccessEff_struct
_tmpF5A;void*_tmpF59[2];struct Cyc_Absyn_JoinEff_struct*_tmpF58;return Cyc_Tcutil_normalize_effect((
void*)((_tmpF58=_cycalloc(sizeof(*_tmpF58)),((_tmpF58[0]=((_tmpF5C.tag=24,((
_tmpF5C.f1=((_tmpF59[1]=Cyc_Tcutil_rgns_of(_tmp25A),((_tmpF59[0]=(void*)((
_tmpF5B=_cycalloc(sizeof(*_tmpF5B)),((_tmpF5B[0]=((_tmpF5A.tag=23,((_tmpF5A.f1=(
void*)_tmp25C,_tmpF5A)))),_tmpF5B)))),((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpF59,sizeof(void*),2)))))),
_tmpF5C)))),_tmpF58)))));}_LL161: {struct Cyc_Absyn_ArrayType_struct*_tmp25D=(
struct Cyc_Absyn_ArrayType_struct*)_tmp246;if(_tmp25D->tag != 9)goto _LL163;else{
_tmp25E=_tmp25D->f1;_tmp25F=_tmp25E.elt_type;}}_LL162: return Cyc_Tcutil_normalize_effect(
Cyc_Tcutil_rgns_of(_tmp25F));_LL163: {struct Cyc_Absyn_TupleType_struct*_tmp260=(
struct Cyc_Absyn_TupleType_struct*)_tmp246;if(_tmp260->tag != 11)goto _LL165;else{
_tmp261=_tmp260->f1;}}_LL164: {struct Cyc_List_List*_tmp28A=0;for(0;_tmp261 != 0;
_tmp261=_tmp261->tl){struct Cyc_List_List*_tmpF5D;_tmp28A=((_tmpF5D=_cycalloc(
sizeof(*_tmpF5D)),((_tmpF5D->hd=(*((struct _tuple11*)_tmp261->hd)).f2,((_tmpF5D->tl=
_tmp28A,_tmpF5D))))));}_tmp264=_tmp28A;goto _LL166;}_LL165: {struct Cyc_Absyn_DatatypeFieldType_struct*
_tmp262=(struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp246;if(_tmp262->tag != 4)
goto _LL167;else{_tmp263=_tmp262->f1;_tmp264=_tmp263.targs;}}_LL166: _tmp267=
_tmp264;goto _LL168;_LL167: {struct Cyc_Absyn_AggrType_struct*_tmp265=(struct Cyc_Absyn_AggrType_struct*)
_tmp246;if(_tmp265->tag != 12)goto _LL169;else{_tmp266=_tmp265->f1;_tmp267=_tmp266.targs;}}
_LL168: {struct Cyc_Absyn_JoinEff_struct _tmpF60;struct Cyc_Absyn_JoinEff_struct*
_tmpF5F;return Cyc_Tcutil_normalize_effect((void*)((_tmpF5F=_cycalloc(sizeof(*
_tmpF5F)),((_tmpF5F[0]=((_tmpF60.tag=24,((_tmpF60.f1=((struct Cyc_List_List*(*)(
void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of,_tmp267),
_tmpF60)))),_tmpF5F)))));}_LL169: {struct Cyc_Absyn_AnonAggrType_struct*_tmp268=(
struct Cyc_Absyn_AnonAggrType_struct*)_tmp246;if(_tmp268->tag != 13)goto _LL16B;
else{_tmp269=_tmp268->f2;}}_LL16A: {struct Cyc_Absyn_JoinEff_struct _tmpF63;struct
Cyc_Absyn_JoinEff_struct*_tmpF62;return Cyc_Tcutil_normalize_effect((void*)((
_tmpF62=_cycalloc(sizeof(*_tmpF62)),((_tmpF62[0]=((_tmpF63.tag=24,((_tmpF63.f1=((
struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Tcutil_rgns_of_field,_tmp269),_tmpF63)))),_tmpF62)))));}
_LL16B: {struct Cyc_Absyn_TagType_struct*_tmp26A=(struct Cyc_Absyn_TagType_struct*)
_tmp246;if(_tmp26A->tag != 20)goto _LL16D;}_LL16C: return Cyc_Absyn_empty_effect;
_LL16D: {struct Cyc_Absyn_FnType_struct*_tmp26B=(struct Cyc_Absyn_FnType_struct*)
_tmp246;if(_tmp26B->tag != 10)goto _LL16F;else{_tmp26C=_tmp26B->f1;_tmp26D=_tmp26C.tvars;
_tmp26E=_tmp26C.effect;_tmp26F=_tmp26C.ret_typ;_tmp270=_tmp26C.args;_tmp271=
_tmp26C.cyc_varargs;_tmp272=_tmp26C.rgn_po;}}_LL16E: {void*_tmp290=Cyc_Tcutil_substitute(((
struct Cyc_List_List*(*)(struct _tuple14*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Tcutil_region_free_subst,_tmp26D),(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp26E))->v);return Cyc_Tcutil_normalize_effect(_tmp290);}_LL16F: {
struct Cyc_Absyn_UniqueRgn_struct*_tmp273=(struct Cyc_Absyn_UniqueRgn_struct*)
_tmp246;if(_tmp273->tag != 22)goto _LL171;}_LL170: goto _LL172;_LL171: {struct Cyc_Absyn_HeapRgn_struct*
_tmp274=(struct Cyc_Absyn_HeapRgn_struct*)_tmp246;if(_tmp274->tag != 21)goto _LL173;}
_LL172: return Cyc_Absyn_empty_effect;_LL173: {struct Cyc_Absyn_AccessEff_struct*
_tmp275=(struct Cyc_Absyn_AccessEff_struct*)_tmp246;if(_tmp275->tag != 23)goto
_LL175;}_LL174: goto _LL176;_LL175: {struct Cyc_Absyn_JoinEff_struct*_tmp276=(
struct Cyc_Absyn_JoinEff_struct*)_tmp246;if(_tmp276->tag != 24)goto _LL177;}_LL176:
return t;_LL177: {struct Cyc_Absyn_RgnsEff_struct*_tmp277=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp246;if(_tmp277->tag != 25)goto _LL179;else{_tmp278=(void*)_tmp277->f1;}}_LL178:
return Cyc_Tcutil_rgns_of(_tmp278);_LL179: {struct Cyc_Absyn_TypedefType_struct*
_tmp279=(struct Cyc_Absyn_TypedefType_struct*)_tmp246;if(_tmp279->tag != 18)goto
_LL146;else{_tmp27A=_tmp279->f2;}}_LL17A: {struct Cyc_Absyn_JoinEff_struct _tmpF66;
struct Cyc_Absyn_JoinEff_struct*_tmpF65;return Cyc_Tcutil_normalize_effect((void*)((
_tmpF65=_cycalloc(sizeof(*_tmpF65)),((_tmpF65[0]=((_tmpF66.tag=24,((_tmpF66.f1=((
struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Tcutil_rgns_of,_tmp27A),_tmpF66)))),_tmpF65)))));}_LL146:;}void*Cyc_Tcutil_normalize_effect(
void*e);void*Cyc_Tcutil_normalize_effect(void*e){e=Cyc_Tcutil_compress(e);{void*
_tmp293=e;struct Cyc_List_List*_tmp295;struct Cyc_List_List**_tmp296;void*_tmp298;
_LL183: {struct Cyc_Absyn_JoinEff_struct*_tmp294=(struct Cyc_Absyn_JoinEff_struct*)
_tmp293;if(_tmp294->tag != 24)goto _LL185;else{_tmp295=_tmp294->f1;_tmp296=(struct
Cyc_List_List**)& _tmp294->f1;}}_LL184: {int redo_join=0;{struct Cyc_List_List*effs=*
_tmp296;for(0;effs != 0;effs=effs->tl){void*_tmp299=(void*)effs->hd;effs->hd=(
void*)Cyc_Tcutil_compress(Cyc_Tcutil_normalize_effect(_tmp299));{void*_tmp29A=(
void*)effs->hd;void*_tmp29D;_LL18A: {struct Cyc_Absyn_JoinEff_struct*_tmp29B=(
struct Cyc_Absyn_JoinEff_struct*)_tmp29A;if(_tmp29B->tag != 24)goto _LL18C;}_LL18B:
goto _LL18D;_LL18C: {struct Cyc_Absyn_AccessEff_struct*_tmp29C=(struct Cyc_Absyn_AccessEff_struct*)
_tmp29A;if(_tmp29C->tag != 23)goto _LL18E;else{_tmp29D=(void*)_tmp29C->f1;{struct
Cyc_Absyn_HeapRgn_struct*_tmp29E=(struct Cyc_Absyn_HeapRgn_struct*)_tmp29D;if(
_tmp29E->tag != 21)goto _LL18E;}}}_LL18D: redo_join=1;goto _LL189;_LL18E:;_LL18F:
goto _LL189;_LL189:;}}}if(!redo_join)return e;{struct Cyc_List_List*effects=0;{
struct Cyc_List_List*effs=*_tmp296;for(0;effs != 0;effs=effs->tl){void*_tmp29F=Cyc_Tcutil_compress((
void*)effs->hd);struct Cyc_List_List*_tmp2A1;void*_tmp2A3;_LL191: {struct Cyc_Absyn_JoinEff_struct*
_tmp2A0=(struct Cyc_Absyn_JoinEff_struct*)_tmp29F;if(_tmp2A0->tag != 24)goto _LL193;
else{_tmp2A1=_tmp2A0->f1;}}_LL192: effects=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_revappend)(_tmp2A1,effects);goto _LL190;_LL193: {
struct Cyc_Absyn_AccessEff_struct*_tmp2A2=(struct Cyc_Absyn_AccessEff_struct*)
_tmp29F;if(_tmp2A2->tag != 23)goto _LL195;else{_tmp2A3=(void*)_tmp2A2->f1;{struct
Cyc_Absyn_HeapRgn_struct*_tmp2A4=(struct Cyc_Absyn_HeapRgn_struct*)_tmp2A3;if(
_tmp2A4->tag != 21)goto _LL195;}}}_LL194: goto _LL190;_LL195:;_LL196:{struct Cyc_List_List*
_tmpF67;effects=((_tmpF67=_cycalloc(sizeof(*_tmpF67)),((_tmpF67->hd=_tmp29F,((
_tmpF67->tl=effects,_tmpF67))))));}goto _LL190;_LL190:;}}*_tmp296=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(effects);return e;}}_LL185: {struct Cyc_Absyn_RgnsEff_struct*
_tmp297=(struct Cyc_Absyn_RgnsEff_struct*)_tmp293;if(_tmp297->tag != 25)goto _LL187;
else{_tmp298=(void*)_tmp297->f1;}}_LL186: {void*_tmp2A6=Cyc_Tcutil_compress(
_tmp298);_LL198: {struct Cyc_Absyn_Evar_struct*_tmp2A7=(struct Cyc_Absyn_Evar_struct*)
_tmp2A6;if(_tmp2A7->tag != 1)goto _LL19A;}_LL199: goto _LL19B;_LL19A: {struct Cyc_Absyn_VarType_struct*
_tmp2A8=(struct Cyc_Absyn_VarType_struct*)_tmp2A6;if(_tmp2A8->tag != 2)goto _LL19C;}
_LL19B: return e;_LL19C:;_LL19D: return Cyc_Tcutil_rgns_of(_tmp298);_LL197:;}_LL187:;
_LL188: return e;_LL182:;}}static void*Cyc_Tcutil_dummy_fntype(void*eff);static void*
Cyc_Tcutil_dummy_fntype(void*eff){struct Cyc_Absyn_FnType_struct _tmpF71;struct Cyc_Core_Opt*
_tmpF70;struct Cyc_Absyn_FnInfo _tmpF6F;struct Cyc_Absyn_FnType_struct*_tmpF6E;
struct Cyc_Absyn_FnType_struct*_tmp2A9=(_tmpF6E=_cycalloc(sizeof(*_tmpF6E)),((
_tmpF6E[0]=((_tmpF71.tag=10,((_tmpF71.f1=((_tmpF6F.tvars=0,((_tmpF6F.effect=((
_tmpF70=_cycalloc(sizeof(*_tmpF70)),((_tmpF70->v=eff,_tmpF70)))),((_tmpF6F.ret_typ=(
void*)& Cyc_Absyn_VoidType_val,((_tmpF6F.args=0,((_tmpF6F.c_varargs=0,((_tmpF6F.cyc_varargs=
0,((_tmpF6F.rgn_po=0,((_tmpF6F.attributes=0,_tmpF6F)))))))))))))))),_tmpF71)))),
_tmpF6E)));return Cyc_Absyn_atb_typ((void*)_tmp2A9,(void*)& Cyc_Absyn_HeapRgn_val,
Cyc_Absyn_empty_tqual(0),Cyc_Absyn_bounds_one,Cyc_Absyn_false_conref);}int Cyc_Tcutil_region_in_effect(
int constrain,void*r,void*e);int Cyc_Tcutil_region_in_effect(int constrain,void*r,
void*e){r=Cyc_Tcutil_compress(r);if(r == (void*)& Cyc_Absyn_HeapRgn_val  || r == (
void*)& Cyc_Absyn_UniqueRgn_val)return 1;{void*_tmp2AE=Cyc_Tcutil_compress(e);void*
_tmp2B0;struct Cyc_List_List*_tmp2B2;void*_tmp2B4;struct Cyc_Core_Opt*_tmp2B6;
struct Cyc_Core_Opt*_tmp2B7;struct Cyc_Core_Opt**_tmp2B8;struct Cyc_Core_Opt*
_tmp2B9;_LL19F: {struct Cyc_Absyn_AccessEff_struct*_tmp2AF=(struct Cyc_Absyn_AccessEff_struct*)
_tmp2AE;if(_tmp2AF->tag != 23)goto _LL1A1;else{_tmp2B0=(void*)_tmp2AF->f1;}}_LL1A0:
if(constrain)return Cyc_Tcutil_unify(r,_tmp2B0);_tmp2B0=Cyc_Tcutil_compress(
_tmp2B0);if(r == _tmp2B0)return 1;{struct _tuple0 _tmpF72;struct _tuple0 _tmp2BB=(
_tmpF72.f1=r,((_tmpF72.f2=_tmp2B0,_tmpF72)));void*_tmp2BC;struct Cyc_Absyn_Tvar*
_tmp2BE;void*_tmp2BF;struct Cyc_Absyn_Tvar*_tmp2C1;_LL1AA: _tmp2BC=_tmp2BB.f1;{
struct Cyc_Absyn_VarType_struct*_tmp2BD=(struct Cyc_Absyn_VarType_struct*)_tmp2BC;
if(_tmp2BD->tag != 2)goto _LL1AC;else{_tmp2BE=_tmp2BD->f1;}}_tmp2BF=_tmp2BB.f2;{
struct Cyc_Absyn_VarType_struct*_tmp2C0=(struct Cyc_Absyn_VarType_struct*)_tmp2BF;
if(_tmp2C0->tag != 2)goto _LL1AC;else{_tmp2C1=_tmp2C0->f1;}}_LL1AB: return Cyc_Absyn_tvar_cmp(
_tmp2BE,_tmp2C1)== 0;_LL1AC:;_LL1AD: return 0;_LL1A9:;}_LL1A1: {struct Cyc_Absyn_JoinEff_struct*
_tmp2B1=(struct Cyc_Absyn_JoinEff_struct*)_tmp2AE;if(_tmp2B1->tag != 24)goto _LL1A3;
else{_tmp2B2=_tmp2B1->f1;}}_LL1A2: for(0;_tmp2B2 != 0;_tmp2B2=_tmp2B2->tl){if(Cyc_Tcutil_region_in_effect(
constrain,r,(void*)_tmp2B2->hd))return 1;}return 0;_LL1A3: {struct Cyc_Absyn_RgnsEff_struct*
_tmp2B3=(struct Cyc_Absyn_RgnsEff_struct*)_tmp2AE;if(_tmp2B3->tag != 25)goto _LL1A5;
else{_tmp2B4=(void*)_tmp2B3->f1;}}_LL1A4: {void*_tmp2C2=Cyc_Tcutil_rgns_of(
_tmp2B4);void*_tmp2C4;_LL1AF: {struct Cyc_Absyn_RgnsEff_struct*_tmp2C3=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp2C2;if(_tmp2C3->tag != 25)goto _LL1B1;else{_tmp2C4=(void*)_tmp2C3->f1;}}_LL1B0:
if(!constrain)return 0;{void*_tmp2C5=Cyc_Tcutil_compress(_tmp2C4);struct Cyc_Core_Opt*
_tmp2C7;struct Cyc_Core_Opt*_tmp2C8;struct Cyc_Core_Opt**_tmp2C9;struct Cyc_Core_Opt*
_tmp2CA;_LL1B4: {struct Cyc_Absyn_Evar_struct*_tmp2C6=(struct Cyc_Absyn_Evar_struct*)
_tmp2C5;if(_tmp2C6->tag != 1)goto _LL1B6;else{_tmp2C7=_tmp2C6->f1;_tmp2C8=_tmp2C6->f2;
_tmp2C9=(struct Cyc_Core_Opt**)& _tmp2C6->f2;_tmp2CA=_tmp2C6->f4;}}_LL1B5: {void*
_tmp2CB=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,_tmp2CA);Cyc_Tcutil_occurs(
_tmp2CB,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp2CA))->v,r);{struct Cyc_Absyn_JoinEff_struct _tmpF81;struct Cyc_Absyn_AccessEff_struct*
_tmpF80;struct Cyc_Absyn_AccessEff_struct _tmpF7F;void*_tmpF7E[2];struct Cyc_Absyn_JoinEff_struct*
_tmpF7D;void*_tmp2CC=Cyc_Tcutil_dummy_fntype((void*)((_tmpF7D=_cycalloc(sizeof(*
_tmpF7D)),((_tmpF7D[0]=((_tmpF81.tag=24,((_tmpF81.f1=((_tmpF7E[1]=(void*)((
_tmpF80=_cycalloc(sizeof(*_tmpF80)),((_tmpF80[0]=((_tmpF7F.tag=23,((_tmpF7F.f1=(
void*)r,_tmpF7F)))),_tmpF80)))),((_tmpF7E[0]=_tmp2CB,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpF7E,sizeof(void*),2)))))),
_tmpF81)))),_tmpF7D)))));{struct Cyc_Core_Opt*_tmpF82;*_tmp2C9=((_tmpF82=
_cycalloc(sizeof(*_tmpF82)),((_tmpF82->v=_tmp2CC,_tmpF82))));}return 1;}}_LL1B6:;
_LL1B7: return 0;_LL1B3:;}_LL1B1:;_LL1B2: return Cyc_Tcutil_region_in_effect(
constrain,r,_tmp2C2);_LL1AE:;}_LL1A5: {struct Cyc_Absyn_Evar_struct*_tmp2B5=(
struct Cyc_Absyn_Evar_struct*)_tmp2AE;if(_tmp2B5->tag != 1)goto _LL1A7;else{_tmp2B6=
_tmp2B5->f1;_tmp2B7=_tmp2B5->f2;_tmp2B8=(struct Cyc_Core_Opt**)& _tmp2B5->f2;
_tmp2B9=_tmp2B5->f4;}}_LL1A6: if(_tmp2B6 == 0  || (enum Cyc_Absyn_Kind)_tmp2B6->v != 
Cyc_Absyn_EffKind){const char*_tmpF85;void*_tmpF84;(_tmpF84=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpF85="effect evar has wrong kind",
_tag_dyneither(_tmpF85,sizeof(char),27))),_tag_dyneither(_tmpF84,sizeof(void*),0)));}
if(!constrain)return 0;{void*_tmp2D5=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,
_tmp2B9);Cyc_Tcutil_occurs(_tmp2D5,Cyc_Core_heap_region,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp2B9))->v,r);{struct Cyc_Absyn_JoinEff_struct
_tmpF9A;struct Cyc_List_List*_tmpF99;struct Cyc_Absyn_AccessEff_struct _tmpF98;
struct Cyc_Absyn_AccessEff_struct*_tmpF97;struct Cyc_List_List*_tmpF96;struct Cyc_Absyn_JoinEff_struct*
_tmpF95;struct Cyc_Absyn_JoinEff_struct*_tmp2D6=(_tmpF95=_cycalloc(sizeof(*
_tmpF95)),((_tmpF95[0]=((_tmpF9A.tag=24,((_tmpF9A.f1=((_tmpF96=_cycalloc(sizeof(*
_tmpF96)),((_tmpF96->hd=_tmp2D5,((_tmpF96->tl=((_tmpF99=_cycalloc(sizeof(*
_tmpF99)),((_tmpF99->hd=(void*)((_tmpF97=_cycalloc(sizeof(*_tmpF97)),((_tmpF97[0]=((
_tmpF98.tag=23,((_tmpF98.f1=(void*)r,_tmpF98)))),_tmpF97)))),((_tmpF99->tl=0,
_tmpF99)))))),_tmpF96)))))),_tmpF9A)))),_tmpF95)));{struct Cyc_Core_Opt*_tmpF9B;*
_tmp2B8=((_tmpF9B=_cycalloc(sizeof(*_tmpF9B)),((_tmpF9B->v=(void*)_tmp2D6,
_tmpF9B))));}return 1;}}_LL1A7:;_LL1A8: return 0;_LL19E:;}}static int Cyc_Tcutil_type_in_effect(
int may_constrain_evars,void*t,void*e);static int Cyc_Tcutil_type_in_effect(int
may_constrain_evars,void*t,void*e){t=Cyc_Tcutil_compress(t);{void*_tmp2DE=Cyc_Tcutil_normalize_effect(
Cyc_Tcutil_compress(e));struct Cyc_List_List*_tmp2E1;void*_tmp2E3;struct Cyc_Core_Opt*
_tmp2E5;struct Cyc_Core_Opt*_tmp2E6;struct Cyc_Core_Opt**_tmp2E7;struct Cyc_Core_Opt*
_tmp2E8;_LL1B9: {struct Cyc_Absyn_AccessEff_struct*_tmp2DF=(struct Cyc_Absyn_AccessEff_struct*)
_tmp2DE;if(_tmp2DF->tag != 23)goto _LL1BB;}_LL1BA: return 0;_LL1BB: {struct Cyc_Absyn_JoinEff_struct*
_tmp2E0=(struct Cyc_Absyn_JoinEff_struct*)_tmp2DE;if(_tmp2E0->tag != 24)goto _LL1BD;
else{_tmp2E1=_tmp2E0->f1;}}_LL1BC: for(0;_tmp2E1 != 0;_tmp2E1=_tmp2E1->tl){if(Cyc_Tcutil_type_in_effect(
may_constrain_evars,t,(void*)_tmp2E1->hd))return 1;}return 0;_LL1BD: {struct Cyc_Absyn_RgnsEff_struct*
_tmp2E2=(struct Cyc_Absyn_RgnsEff_struct*)_tmp2DE;if(_tmp2E2->tag != 25)goto _LL1BF;
else{_tmp2E3=(void*)_tmp2E2->f1;}}_LL1BE: _tmp2E3=Cyc_Tcutil_compress(_tmp2E3);
if(t == _tmp2E3)return 1;if(may_constrain_evars)return Cyc_Tcutil_unify(t,_tmp2E3);{
void*_tmp2E9=Cyc_Tcutil_rgns_of(t);void*_tmp2EB;_LL1C4: {struct Cyc_Absyn_RgnsEff_struct*
_tmp2EA=(struct Cyc_Absyn_RgnsEff_struct*)_tmp2E9;if(_tmp2EA->tag != 25)goto _LL1C6;
else{_tmp2EB=(void*)_tmp2EA->f1;}}_LL1C5: {struct _tuple0 _tmpF9C;struct _tuple0
_tmp2ED=(_tmpF9C.f1=Cyc_Tcutil_compress(_tmp2EB),((_tmpF9C.f2=_tmp2E3,_tmpF9C)));
void*_tmp2EE;struct Cyc_Absyn_Tvar*_tmp2F0;void*_tmp2F1;struct Cyc_Absyn_Tvar*
_tmp2F3;_LL1C9: _tmp2EE=_tmp2ED.f1;{struct Cyc_Absyn_VarType_struct*_tmp2EF=(
struct Cyc_Absyn_VarType_struct*)_tmp2EE;if(_tmp2EF->tag != 2)goto _LL1CB;else{
_tmp2F0=_tmp2EF->f1;}}_tmp2F1=_tmp2ED.f2;{struct Cyc_Absyn_VarType_struct*_tmp2F2=(
struct Cyc_Absyn_VarType_struct*)_tmp2F1;if(_tmp2F2->tag != 2)goto _LL1CB;else{
_tmp2F3=_tmp2F2->f1;}}_LL1CA: return Cyc_Tcutil_unify(t,_tmp2E3);_LL1CB:;_LL1CC:
return _tmp2EB == _tmp2E3;_LL1C8:;}_LL1C6:;_LL1C7: return Cyc_Tcutil_type_in_effect(
may_constrain_evars,t,_tmp2E9);_LL1C3:;}_LL1BF: {struct Cyc_Absyn_Evar_struct*
_tmp2E4=(struct Cyc_Absyn_Evar_struct*)_tmp2DE;if(_tmp2E4->tag != 1)goto _LL1C1;
else{_tmp2E5=_tmp2E4->f1;_tmp2E6=_tmp2E4->f2;_tmp2E7=(struct Cyc_Core_Opt**)&
_tmp2E4->f2;_tmp2E8=_tmp2E4->f4;}}_LL1C0: if(_tmp2E5 == 0  || (enum Cyc_Absyn_Kind)
_tmp2E5->v != Cyc_Absyn_EffKind){const char*_tmpF9F;void*_tmpF9E;(_tmpF9E=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpF9F="effect evar has wrong kind",
_tag_dyneither(_tmpF9F,sizeof(char),27))),_tag_dyneither(_tmpF9E,sizeof(void*),0)));}
if(!may_constrain_evars)return 0;{void*_tmp2F6=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_ek,_tmp2E8);Cyc_Tcutil_occurs(_tmp2F6,Cyc_Core_heap_region,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp2E8))->v,t);{struct Cyc_Absyn_JoinEff_struct
_tmpFB4;struct Cyc_List_List*_tmpFB3;struct Cyc_Absyn_RgnsEff_struct _tmpFB2;struct
Cyc_Absyn_RgnsEff_struct*_tmpFB1;struct Cyc_List_List*_tmpFB0;struct Cyc_Absyn_JoinEff_struct*
_tmpFAF;struct Cyc_Absyn_JoinEff_struct*_tmp2F7=(_tmpFAF=_cycalloc(sizeof(*
_tmpFAF)),((_tmpFAF[0]=((_tmpFB4.tag=24,((_tmpFB4.f1=((_tmpFB0=_cycalloc(sizeof(*
_tmpFB0)),((_tmpFB0->hd=_tmp2F6,((_tmpFB0->tl=((_tmpFB3=_cycalloc(sizeof(*
_tmpFB3)),((_tmpFB3->hd=(void*)((_tmpFB1=_cycalloc(sizeof(*_tmpFB1)),((_tmpFB1[0]=((
_tmpFB2.tag=25,((_tmpFB2.f1=(void*)t,_tmpFB2)))),_tmpFB1)))),((_tmpFB3->tl=0,
_tmpFB3)))))),_tmpFB0)))))),_tmpFB4)))),_tmpFAF)));{struct Cyc_Core_Opt*_tmpFB5;*
_tmp2E7=((_tmpFB5=_cycalloc(sizeof(*_tmpFB5)),((_tmpFB5->v=(void*)_tmp2F7,
_tmpFB5))));}return 1;}}_LL1C1:;_LL1C2: return 0;_LL1B8:;}}static int Cyc_Tcutil_variable_in_effect(
int may_constrain_evars,struct Cyc_Absyn_Tvar*v,void*e);static int Cyc_Tcutil_variable_in_effect(
int may_constrain_evars,struct Cyc_Absyn_Tvar*v,void*e){e=Cyc_Tcutil_compress(e);{
void*_tmp2FF=e;struct Cyc_Absyn_Tvar*_tmp301;struct Cyc_List_List*_tmp303;void*
_tmp305;struct Cyc_Core_Opt*_tmp307;struct Cyc_Core_Opt*_tmp308;struct Cyc_Core_Opt**
_tmp309;struct Cyc_Core_Opt*_tmp30A;_LL1CE: {struct Cyc_Absyn_VarType_struct*
_tmp300=(struct Cyc_Absyn_VarType_struct*)_tmp2FF;if(_tmp300->tag != 2)goto _LL1D0;
else{_tmp301=_tmp300->f1;}}_LL1CF: return Cyc_Absyn_tvar_cmp(v,_tmp301)== 0;_LL1D0: {
struct Cyc_Absyn_JoinEff_struct*_tmp302=(struct Cyc_Absyn_JoinEff_struct*)_tmp2FF;
if(_tmp302->tag != 24)goto _LL1D2;else{_tmp303=_tmp302->f1;}}_LL1D1: for(0;_tmp303
!= 0;_tmp303=_tmp303->tl){if(Cyc_Tcutil_variable_in_effect(may_constrain_evars,v,(
void*)_tmp303->hd))return 1;}return 0;_LL1D2: {struct Cyc_Absyn_RgnsEff_struct*
_tmp304=(struct Cyc_Absyn_RgnsEff_struct*)_tmp2FF;if(_tmp304->tag != 25)goto _LL1D4;
else{_tmp305=(void*)_tmp304->f1;}}_LL1D3: {void*_tmp30B=Cyc_Tcutil_rgns_of(
_tmp305);void*_tmp30D;_LL1D9: {struct Cyc_Absyn_RgnsEff_struct*_tmp30C=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp30B;if(_tmp30C->tag != 25)goto _LL1DB;else{_tmp30D=(void*)_tmp30C->f1;}}_LL1DA:
if(!may_constrain_evars)return 0;{void*_tmp30E=Cyc_Tcutil_compress(_tmp30D);
struct Cyc_Core_Opt*_tmp310;struct Cyc_Core_Opt*_tmp311;struct Cyc_Core_Opt**
_tmp312;struct Cyc_Core_Opt*_tmp313;_LL1DE: {struct Cyc_Absyn_Evar_struct*_tmp30F=(
struct Cyc_Absyn_Evar_struct*)_tmp30E;if(_tmp30F->tag != 1)goto _LL1E0;else{_tmp310=
_tmp30F->f1;_tmp311=_tmp30F->f2;_tmp312=(struct Cyc_Core_Opt**)& _tmp30F->f2;
_tmp313=_tmp30F->f4;}}_LL1DF: {void*_tmp314=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_ek,_tmp313);if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct
Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp313))->v,v))return 0;{
struct Cyc_Absyn_JoinEff_struct _tmpFC4;struct Cyc_Absyn_VarType_struct*_tmpFC3;
struct Cyc_Absyn_VarType_struct _tmpFC2;void*_tmpFC1[2];struct Cyc_Absyn_JoinEff_struct*
_tmpFC0;void*_tmp315=Cyc_Tcutil_dummy_fntype((void*)((_tmpFC0=_cycalloc(sizeof(*
_tmpFC0)),((_tmpFC0[0]=((_tmpFC4.tag=24,((_tmpFC4.f1=((_tmpFC1[1]=(void*)((
_tmpFC3=_cycalloc(sizeof(*_tmpFC3)),((_tmpFC3[0]=((_tmpFC2.tag=2,((_tmpFC2.f1=v,
_tmpFC2)))),_tmpFC3)))),((_tmpFC1[0]=_tmp314,((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpFC1,sizeof(void*),2)))))),
_tmpFC4)))),_tmpFC0)))));{struct Cyc_Core_Opt*_tmpFC5;*_tmp312=((_tmpFC5=
_cycalloc(sizeof(*_tmpFC5)),((_tmpFC5->v=_tmp315,_tmpFC5))));}return 1;}}_LL1E0:;
_LL1E1: return 0;_LL1DD:;}_LL1DB:;_LL1DC: return Cyc_Tcutil_variable_in_effect(
may_constrain_evars,v,_tmp30B);_LL1D8:;}_LL1D4: {struct Cyc_Absyn_Evar_struct*
_tmp306=(struct Cyc_Absyn_Evar_struct*)_tmp2FF;if(_tmp306->tag != 1)goto _LL1D6;
else{_tmp307=_tmp306->f1;_tmp308=_tmp306->f2;_tmp309=(struct Cyc_Core_Opt**)&
_tmp306->f2;_tmp30A=_tmp306->f4;}}_LL1D5: if(_tmp307 == 0  || (enum Cyc_Absyn_Kind)
_tmp307->v != Cyc_Absyn_EffKind){const char*_tmpFC8;void*_tmpFC7;(_tmpFC7=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpFC8="effect evar has wrong kind",
_tag_dyneither(_tmpFC8,sizeof(char),27))),_tag_dyneither(_tmpFC7,sizeof(void*),0)));}{
void*_tmp31E=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,_tmp30A);if(
!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*
l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp30A))->v,v))return 0;{struct Cyc_Absyn_JoinEff_struct
_tmpFDD;struct Cyc_List_List*_tmpFDC;struct Cyc_Absyn_VarType_struct _tmpFDB;struct
Cyc_Absyn_VarType_struct*_tmpFDA;struct Cyc_List_List*_tmpFD9;struct Cyc_Absyn_JoinEff_struct*
_tmpFD8;struct Cyc_Absyn_JoinEff_struct*_tmp31F=(_tmpFD8=_cycalloc(sizeof(*
_tmpFD8)),((_tmpFD8[0]=((_tmpFDD.tag=24,((_tmpFDD.f1=((_tmpFD9=_cycalloc(sizeof(*
_tmpFD9)),((_tmpFD9->hd=_tmp31E,((_tmpFD9->tl=((_tmpFDC=_cycalloc(sizeof(*
_tmpFDC)),((_tmpFDC->hd=(void*)((_tmpFDA=_cycalloc(sizeof(*_tmpFDA)),((_tmpFDA[0]=((
_tmpFDB.tag=2,((_tmpFDB.f1=v,_tmpFDB)))),_tmpFDA)))),((_tmpFDC->tl=0,_tmpFDC)))))),
_tmpFD9)))))),_tmpFDD)))),_tmpFD8)));{struct Cyc_Core_Opt*_tmpFDE;*_tmp309=((
_tmpFDE=_cycalloc(sizeof(*_tmpFDE)),((_tmpFDE->v=(void*)_tmp31F,_tmpFDE))));}
return 1;}}_LL1D6:;_LL1D7: return 0;_LL1CD:;}}static int Cyc_Tcutil_evar_in_effect(
void*evar,void*e);static int Cyc_Tcutil_evar_in_effect(void*evar,void*e){e=Cyc_Tcutil_compress(
e);{void*_tmp327=e;struct Cyc_List_List*_tmp329;void*_tmp32B;_LL1E3: {struct Cyc_Absyn_JoinEff_struct*
_tmp328=(struct Cyc_Absyn_JoinEff_struct*)_tmp327;if(_tmp328->tag != 24)goto _LL1E5;
else{_tmp329=_tmp328->f1;}}_LL1E4: for(0;_tmp329 != 0;_tmp329=_tmp329->tl){if(Cyc_Tcutil_evar_in_effect(
evar,(void*)_tmp329->hd))return 1;}return 0;_LL1E5: {struct Cyc_Absyn_RgnsEff_struct*
_tmp32A=(struct Cyc_Absyn_RgnsEff_struct*)_tmp327;if(_tmp32A->tag != 25)goto _LL1E7;
else{_tmp32B=(void*)_tmp32A->f1;}}_LL1E6: {void*_tmp32D=Cyc_Tcutil_rgns_of(
_tmp32B);void*_tmp32F;_LL1EC: {struct Cyc_Absyn_RgnsEff_struct*_tmp32E=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp32D;if(_tmp32E->tag != 25)goto _LL1EE;else{_tmp32F=(void*)_tmp32E->f1;}}_LL1ED:
return 0;_LL1EE:;_LL1EF: return Cyc_Tcutil_evar_in_effect(evar,_tmp32D);_LL1EB:;}
_LL1E7: {struct Cyc_Absyn_Evar_struct*_tmp32C=(struct Cyc_Absyn_Evar_struct*)
_tmp327;if(_tmp32C->tag != 1)goto _LL1E9;}_LL1E8: return evar == e;_LL1E9:;_LL1EA:
return 0;_LL1E2:;}}int Cyc_Tcutil_subset_effect(int may_constrain_evars,void*e1,
void*e2);int Cyc_Tcutil_subset_effect(int may_constrain_evars,void*e1,void*e2){
void*_tmp330=Cyc_Tcutil_compress(e1);struct Cyc_List_List*_tmp332;void*_tmp334;
struct Cyc_Absyn_Tvar*_tmp336;void*_tmp338;struct Cyc_Core_Opt*_tmp33A;struct Cyc_Core_Opt**
_tmp33B;struct Cyc_Core_Opt*_tmp33C;_LL1F1: {struct Cyc_Absyn_JoinEff_struct*
_tmp331=(struct Cyc_Absyn_JoinEff_struct*)_tmp330;if(_tmp331->tag != 24)goto _LL1F3;
else{_tmp332=_tmp331->f1;}}_LL1F2: for(0;_tmp332 != 0;_tmp332=_tmp332->tl){if(!Cyc_Tcutil_subset_effect(
may_constrain_evars,(void*)_tmp332->hd,e2))return 0;}return 1;_LL1F3: {struct Cyc_Absyn_AccessEff_struct*
_tmp333=(struct Cyc_Absyn_AccessEff_struct*)_tmp330;if(_tmp333->tag != 23)goto
_LL1F5;else{_tmp334=(void*)_tmp333->f1;}}_LL1F4: return Cyc_Tcutil_region_in_effect(
0,_tmp334,e2) || may_constrain_evars  && Cyc_Tcutil_unify(_tmp334,(void*)& Cyc_Absyn_HeapRgn_val);
_LL1F5: {struct Cyc_Absyn_VarType_struct*_tmp335=(struct Cyc_Absyn_VarType_struct*)
_tmp330;if(_tmp335->tag != 2)goto _LL1F7;else{_tmp336=_tmp335->f1;}}_LL1F6: return
Cyc_Tcutil_variable_in_effect(may_constrain_evars,_tmp336,e2);_LL1F7: {struct Cyc_Absyn_RgnsEff_struct*
_tmp337=(struct Cyc_Absyn_RgnsEff_struct*)_tmp330;if(_tmp337->tag != 25)goto _LL1F9;
else{_tmp338=(void*)_tmp337->f1;}}_LL1F8: {void*_tmp33D=Cyc_Tcutil_rgns_of(
_tmp338);void*_tmp33F;_LL1FE: {struct Cyc_Absyn_RgnsEff_struct*_tmp33E=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp33D;if(_tmp33E->tag != 25)goto _LL200;else{_tmp33F=(void*)_tmp33E->f1;}}_LL1FF:
return Cyc_Tcutil_type_in_effect(may_constrain_evars,_tmp33F,e2) || 
may_constrain_evars  && Cyc_Tcutil_unify(_tmp33F,Cyc_Absyn_sint_typ);_LL200:;
_LL201: return Cyc_Tcutil_subset_effect(may_constrain_evars,_tmp33D,e2);_LL1FD:;}
_LL1F9: {struct Cyc_Absyn_Evar_struct*_tmp339=(struct Cyc_Absyn_Evar_struct*)
_tmp330;if(_tmp339->tag != 1)goto _LL1FB;else{_tmp33A=_tmp339->f2;_tmp33B=(struct
Cyc_Core_Opt**)& _tmp339->f2;_tmp33C=_tmp339->f4;}}_LL1FA: if(!Cyc_Tcutil_evar_in_effect(
e1,e2)){struct Cyc_Core_Opt*_tmpFDF;*_tmp33B=((_tmpFDF=_cycalloc(sizeof(*_tmpFDF)),((
_tmpFDF->v=Cyc_Absyn_empty_effect,_tmpFDF))));}return 1;_LL1FB:;_LL1FC: {const
char*_tmpFE3;void*_tmpFE2[1];struct Cyc_String_pa_struct _tmpFE1;(_tmpFE1.tag=0,((
_tmpFE1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
e1)),((_tmpFE2[0]=& _tmpFE1,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap))Cyc_Tcutil_impos)(((_tmpFE3="subset_effect: bad effect: %s",_tag_dyneither(
_tmpFE3,sizeof(char),30))),_tag_dyneither(_tmpFE2,sizeof(void*),1)))))));}_LL1F0:;}
static int Cyc_Tcutil_unify_effect(void*e1,void*e2);static int Cyc_Tcutil_unify_effect(
void*e1,void*e2){e1=Cyc_Tcutil_normalize_effect(e1);e2=Cyc_Tcutil_normalize_effect(
e2);if(Cyc_Tcutil_subset_effect(0,e1,e2) && Cyc_Tcutil_subset_effect(0,e2,e1))
return 1;if(Cyc_Tcutil_subset_effect(1,e1,e2) && Cyc_Tcutil_subset_effect(1,e2,e1))
return 1;return 0;}static int Cyc_Tcutil_sub_rgnpo(struct Cyc_List_List*rpo1,struct
Cyc_List_List*rpo2);static int Cyc_Tcutil_sub_rgnpo(struct Cyc_List_List*rpo1,
struct Cyc_List_List*rpo2){{struct Cyc_List_List*r1=rpo1;for(0;r1 != 0;r1=r1->tl){
struct _tuple0 _tmp345;void*_tmp346;void*_tmp347;struct _tuple0*_tmp344=(struct
_tuple0*)r1->hd;_tmp345=*_tmp344;_tmp346=_tmp345.f1;_tmp347=_tmp345.f2;{int found=
_tmp346 == (void*)& Cyc_Absyn_HeapRgn_val;{struct Cyc_List_List*r2=rpo2;for(0;r2 != 
0  && !found;r2=r2->tl){struct _tuple0 _tmp349;void*_tmp34A;void*_tmp34B;struct
_tuple0*_tmp348=(struct _tuple0*)r2->hd;_tmp349=*_tmp348;_tmp34A=_tmp349.f1;
_tmp34B=_tmp349.f2;if(Cyc_Tcutil_unify(_tmp346,_tmp34A) && Cyc_Tcutil_unify(
_tmp347,_tmp34B)){found=1;break;}}}if(!found)return 0;}}}return 1;}static int Cyc_Tcutil_same_rgn_po(
struct Cyc_List_List*rpo1,struct Cyc_List_List*rpo2);static int Cyc_Tcutil_same_rgn_po(
struct Cyc_List_List*rpo1,struct Cyc_List_List*rpo2){return Cyc_Tcutil_sub_rgnpo(
rpo1,rpo2) && Cyc_Tcutil_sub_rgnpo(rpo2,rpo1);}struct _tuple18{struct Cyc_Absyn_VarargInfo*
f1;struct Cyc_Absyn_VarargInfo*f2;};struct _tuple19{struct Cyc_Core_Opt*f1;struct
Cyc_Core_Opt*f2;};void Cyc_Tcutil_unify_it(void*t1,void*t2);void Cyc_Tcutil_unify_it(
void*t1,void*t2){Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;Cyc_Tcutil_failure_reason=(
struct _dyneither_ptr)_tag_dyneither(0,0,0);t1=Cyc_Tcutil_compress(t1);t2=Cyc_Tcutil_compress(
t2);if(t1 == t2)return;{void*_tmp34C=t1;struct Cyc_Core_Opt*_tmp34E;struct Cyc_Core_Opt*
_tmp34F;struct Cyc_Core_Opt**_tmp350;struct Cyc_Core_Opt*_tmp351;_LL203: {struct
Cyc_Absyn_Evar_struct*_tmp34D=(struct Cyc_Absyn_Evar_struct*)_tmp34C;if(_tmp34D->tag
!= 1)goto _LL205;else{_tmp34E=_tmp34D->f1;_tmp34F=_tmp34D->f2;_tmp350=(struct Cyc_Core_Opt**)&
_tmp34D->f2;_tmp351=_tmp34D->f4;}}_LL204: Cyc_Tcutil_occurs(t1,Cyc_Core_heap_region,(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp351))->v,t2);{enum 
Cyc_Absyn_Kind _tmp352=Cyc_Tcutil_typ_kind(t2);if(Cyc_Tcutil_kind_leq(_tmp352,(
enum Cyc_Absyn_Kind)((struct Cyc_Core_Opt*)_check_null(_tmp34E))->v)){{struct Cyc_Core_Opt*
_tmpFE4;*_tmp350=((_tmpFE4=_cycalloc(sizeof(*_tmpFE4)),((_tmpFE4->v=t2,_tmpFE4))));}
return;}else{{void*_tmp354=t2;struct Cyc_Core_Opt*_tmp356;struct Cyc_Core_Opt**
_tmp357;struct Cyc_Core_Opt*_tmp358;struct Cyc_Absyn_PtrInfo _tmp35A;_LL208: {
struct Cyc_Absyn_Evar_struct*_tmp355=(struct Cyc_Absyn_Evar_struct*)_tmp354;if(
_tmp355->tag != 1)goto _LL20A;else{_tmp356=_tmp355->f2;_tmp357=(struct Cyc_Core_Opt**)&
_tmp355->f2;_tmp358=_tmp355->f4;}}_LL209: {struct Cyc_List_List*_tmp35B=(struct
Cyc_List_List*)_tmp351->v;{struct Cyc_List_List*s2=(struct Cyc_List_List*)((struct
Cyc_Core_Opt*)_check_null(_tmp358))->v;for(0;s2 != 0;s2=s2->tl){if(!((int(*)(int(*
compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,
struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,_tmp35B,(struct
Cyc_Absyn_Tvar*)s2->hd)){{const char*_tmpFE5;Cyc_Tcutil_failure_reason=((_tmpFE5="(type variable would escape scope)",
_tag_dyneither(_tmpFE5,sizeof(char),35)));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}}}
if(Cyc_Tcutil_kind_leq((enum Cyc_Absyn_Kind)_tmp34E->v,_tmp352)){{struct Cyc_Core_Opt*
_tmpFE6;*_tmp357=((_tmpFE6=_cycalloc(sizeof(*_tmpFE6)),((_tmpFE6->v=t1,_tmpFE6))));}
return;}{const char*_tmpFE7;Cyc_Tcutil_failure_reason=((_tmpFE7="(kinds are incompatible)",
_tag_dyneither(_tmpFE7,sizeof(char),25)));}goto _LL207;}_LL20A:{struct Cyc_Absyn_PointerType_struct*
_tmp359=(struct Cyc_Absyn_PointerType_struct*)_tmp354;if(_tmp359->tag != 5)goto
_LL20C;else{_tmp35A=_tmp359->f1;}}if(!((enum Cyc_Absyn_Kind)_tmp34E->v == Cyc_Absyn_BoxKind))
goto _LL20C;_LL20B: {union Cyc_Absyn_Constraint*_tmp35F=((union Cyc_Absyn_Constraint*(*)(
union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)((_tmp35A.ptr_atts).bounds);{
union Cyc_Absyn_Constraint*_tmp360=_tmp35F;union Cyc_Absyn_Constraint _tmp361;int
_tmp362;_LL20F: _tmp361=*_tmp360;if((_tmp361.No_constr).tag != 3)goto _LL211;
_tmp362=(int)(_tmp361.No_constr).val;_LL210:{struct _union_Constraint_Eq_constr*
_tmpFE8;(_tmpFE8=& _tmp35F->Eq_constr,((_tmpFE8->tag=1,_tmpFE8->val=Cyc_Absyn_bounds_one)));}{
struct Cyc_Core_Opt*_tmpFE9;*_tmp350=((_tmpFE9=_cycalloc(sizeof(*_tmpFE9)),((
_tmpFE9->v=t2,_tmpFE9))));}return;_LL211:;_LL212: goto _LL20E;_LL20E:;}goto _LL207;}
_LL20C:;_LL20D: goto _LL207;_LL207:;}{const char*_tmpFEA;Cyc_Tcutil_failure_reason=((
_tmpFEA="(kinds are incompatible)",_tag_dyneither(_tmpFEA,sizeof(char),25)));}(
int)_throw((void*)& Cyc_Tcutil_Unify_val);}}_LL205:;_LL206: goto _LL202;_LL202:;}{
struct _tuple0 _tmpFEB;struct _tuple0 _tmp367=(_tmpFEB.f1=t2,((_tmpFEB.f2=t1,_tmpFEB)));
void*_tmp368;void*_tmp36A;void*_tmp36C;void*_tmp36E;struct Cyc_Absyn_Tvar*_tmp370;
void*_tmp371;struct Cyc_Absyn_Tvar*_tmp373;void*_tmp374;struct Cyc_Absyn_AggrInfo
_tmp376;union Cyc_Absyn_AggrInfoU _tmp377;struct Cyc_List_List*_tmp378;void*_tmp379;
struct Cyc_Absyn_AggrInfo _tmp37B;union Cyc_Absyn_AggrInfoU _tmp37C;struct Cyc_List_List*
_tmp37D;void*_tmp37E;struct _tuple2*_tmp380;void*_tmp381;struct _tuple2*_tmp383;
void*_tmp384;struct Cyc_List_List*_tmp386;void*_tmp387;struct Cyc_List_List*
_tmp389;void*_tmp38A;struct Cyc_Absyn_DatatypeInfo _tmp38C;union Cyc_Absyn_DatatypeInfoU
_tmp38D;struct Cyc_Absyn_Datatypedecl**_tmp38E;struct Cyc_Absyn_Datatypedecl*
_tmp38F;struct Cyc_List_List*_tmp390;void*_tmp391;struct Cyc_Absyn_DatatypeInfo
_tmp393;union Cyc_Absyn_DatatypeInfoU _tmp394;struct Cyc_Absyn_Datatypedecl**
_tmp395;struct Cyc_Absyn_Datatypedecl*_tmp396;struct Cyc_List_List*_tmp397;void*
_tmp398;struct Cyc_Absyn_DatatypeFieldInfo _tmp39A;union Cyc_Absyn_DatatypeFieldInfoU
_tmp39B;struct _tuple3 _tmp39C;struct Cyc_Absyn_Datatypedecl*_tmp39D;struct Cyc_Absyn_Datatypefield*
_tmp39E;struct Cyc_List_List*_tmp39F;void*_tmp3A0;struct Cyc_Absyn_DatatypeFieldInfo
_tmp3A2;union Cyc_Absyn_DatatypeFieldInfoU _tmp3A3;struct _tuple3 _tmp3A4;struct Cyc_Absyn_Datatypedecl*
_tmp3A5;struct Cyc_Absyn_Datatypefield*_tmp3A6;struct Cyc_List_List*_tmp3A7;void*
_tmp3A8;struct Cyc_Absyn_PtrInfo _tmp3AA;void*_tmp3AB;struct Cyc_Absyn_Tqual _tmp3AC;
struct Cyc_Absyn_PtrAtts _tmp3AD;void*_tmp3AE;union Cyc_Absyn_Constraint*_tmp3AF;
union Cyc_Absyn_Constraint*_tmp3B0;union Cyc_Absyn_Constraint*_tmp3B1;void*_tmp3B2;
struct Cyc_Absyn_PtrInfo _tmp3B4;void*_tmp3B5;struct Cyc_Absyn_Tqual _tmp3B6;struct
Cyc_Absyn_PtrAtts _tmp3B7;void*_tmp3B8;union Cyc_Absyn_Constraint*_tmp3B9;union Cyc_Absyn_Constraint*
_tmp3BA;union Cyc_Absyn_Constraint*_tmp3BB;void*_tmp3BC;enum Cyc_Absyn_Sign
_tmp3BE;enum Cyc_Absyn_Size_of _tmp3BF;void*_tmp3C0;enum Cyc_Absyn_Sign _tmp3C2;
enum Cyc_Absyn_Size_of _tmp3C3;void*_tmp3C4;void*_tmp3C6;void*_tmp3C8;int _tmp3CA;
void*_tmp3CB;int _tmp3CD;void*_tmp3CE;void*_tmp3D0;void*_tmp3D1;void*_tmp3D3;void*
_tmp3D4;struct Cyc_Absyn_Exp*_tmp3D6;void*_tmp3D7;struct Cyc_Absyn_Exp*_tmp3D9;
void*_tmp3DA;struct Cyc_Absyn_ArrayInfo _tmp3DC;void*_tmp3DD;struct Cyc_Absyn_Tqual
_tmp3DE;struct Cyc_Absyn_Exp*_tmp3DF;union Cyc_Absyn_Constraint*_tmp3E0;void*
_tmp3E1;struct Cyc_Absyn_ArrayInfo _tmp3E3;void*_tmp3E4;struct Cyc_Absyn_Tqual
_tmp3E5;struct Cyc_Absyn_Exp*_tmp3E6;union Cyc_Absyn_Constraint*_tmp3E7;void*
_tmp3E8;struct Cyc_Absyn_FnInfo _tmp3EA;struct Cyc_List_List*_tmp3EB;struct Cyc_Core_Opt*
_tmp3EC;void*_tmp3ED;struct Cyc_List_List*_tmp3EE;int _tmp3EF;struct Cyc_Absyn_VarargInfo*
_tmp3F0;struct Cyc_List_List*_tmp3F1;struct Cyc_List_List*_tmp3F2;void*_tmp3F3;
struct Cyc_Absyn_FnInfo _tmp3F5;struct Cyc_List_List*_tmp3F6;struct Cyc_Core_Opt*
_tmp3F7;void*_tmp3F8;struct Cyc_List_List*_tmp3F9;int _tmp3FA;struct Cyc_Absyn_VarargInfo*
_tmp3FB;struct Cyc_List_List*_tmp3FC;struct Cyc_List_List*_tmp3FD;void*_tmp3FE;
struct Cyc_List_List*_tmp400;void*_tmp401;struct Cyc_List_List*_tmp403;void*
_tmp404;enum Cyc_Absyn_AggrKind _tmp406;struct Cyc_List_List*_tmp407;void*_tmp408;
enum Cyc_Absyn_AggrKind _tmp40A;struct Cyc_List_List*_tmp40B;void*_tmp40C;void*
_tmp40E;void*_tmp410;void*_tmp412;void*_tmp414;void*_tmp416;void*_tmp417;void*
_tmp419;void*_tmp41A;void*_tmp41C;void*_tmp41D;void*_tmp41E;void*_tmp420;void*
_tmp421;void*_tmp422;void*_tmp424;void*_tmp426;void*_tmp428;void*_tmp42A;void*
_tmp42C;_LL214: _tmp368=_tmp367.f1;{struct Cyc_Absyn_Evar_struct*_tmp369=(struct
Cyc_Absyn_Evar_struct*)_tmp368;if(_tmp369->tag != 1)goto _LL216;}_LL215: Cyc_Tcutil_unify_it(
t2,t1);return;_LL216: _tmp36A=_tmp367.f1;{struct Cyc_Absyn_VoidType_struct*_tmp36B=(
struct Cyc_Absyn_VoidType_struct*)_tmp36A;if(_tmp36B->tag != 0)goto _LL218;}_tmp36C=
_tmp367.f2;{struct Cyc_Absyn_VoidType_struct*_tmp36D=(struct Cyc_Absyn_VoidType_struct*)
_tmp36C;if(_tmp36D->tag != 0)goto _LL218;}_LL217: return;_LL218: _tmp36E=_tmp367.f1;{
struct Cyc_Absyn_VarType_struct*_tmp36F=(struct Cyc_Absyn_VarType_struct*)_tmp36E;
if(_tmp36F->tag != 2)goto _LL21A;else{_tmp370=_tmp36F->f1;}}_tmp371=_tmp367.f2;{
struct Cyc_Absyn_VarType_struct*_tmp372=(struct Cyc_Absyn_VarType_struct*)_tmp371;
if(_tmp372->tag != 2)goto _LL21A;else{_tmp373=_tmp372->f1;}}_LL219: {struct
_dyneither_ptr*_tmp42E=_tmp370->name;struct _dyneither_ptr*_tmp42F=_tmp373->name;
int _tmp430=_tmp370->identity;int _tmp431=_tmp373->identity;if(_tmp431 == _tmp430)
return;{const char*_tmpFEC;Cyc_Tcutil_failure_reason=((_tmpFEC="(variable types are not the same)",
_tag_dyneither(_tmpFEC,sizeof(char),34)));}goto _LL213;}_LL21A: _tmp374=_tmp367.f1;{
struct Cyc_Absyn_AggrType_struct*_tmp375=(struct Cyc_Absyn_AggrType_struct*)
_tmp374;if(_tmp375->tag != 12)goto _LL21C;else{_tmp376=_tmp375->f1;_tmp377=_tmp376.aggr_info;
_tmp378=_tmp376.targs;}}_tmp379=_tmp367.f2;{struct Cyc_Absyn_AggrType_struct*
_tmp37A=(struct Cyc_Absyn_AggrType_struct*)_tmp379;if(_tmp37A->tag != 12)goto
_LL21C;else{_tmp37B=_tmp37A->f1;_tmp37C=_tmp37B.aggr_info;_tmp37D=_tmp37B.targs;}}
_LL21B: {enum Cyc_Absyn_AggrKind _tmp434;struct _tuple2*_tmp435;struct _tuple12
_tmp433=Cyc_Absyn_aggr_kinded_name(_tmp37C);_tmp434=_tmp433.f1;_tmp435=_tmp433.f2;{
enum Cyc_Absyn_AggrKind _tmp437;struct _tuple2*_tmp438;struct _tuple12 _tmp436=Cyc_Absyn_aggr_kinded_name(
_tmp377);_tmp437=_tmp436.f1;_tmp438=_tmp436.f2;if(_tmp434 != _tmp437){{const char*
_tmpFED;Cyc_Tcutil_failure_reason=((_tmpFED="(struct and union type)",
_tag_dyneither(_tmpFED,sizeof(char),24)));}goto _LL213;}if(Cyc_Absyn_qvar_cmp(
_tmp435,_tmp438)!= 0){{const char*_tmpFEE;Cyc_Tcutil_failure_reason=((_tmpFEE="(different type name)",
_tag_dyneither(_tmpFEE,sizeof(char),22)));}goto _LL213;}Cyc_Tcutil_unify_list(
_tmp37D,_tmp378);return;}}_LL21C: _tmp37E=_tmp367.f1;{struct Cyc_Absyn_EnumType_struct*
_tmp37F=(struct Cyc_Absyn_EnumType_struct*)_tmp37E;if(_tmp37F->tag != 14)goto
_LL21E;else{_tmp380=_tmp37F->f1;}}_tmp381=_tmp367.f2;{struct Cyc_Absyn_EnumType_struct*
_tmp382=(struct Cyc_Absyn_EnumType_struct*)_tmp381;if(_tmp382->tag != 14)goto
_LL21E;else{_tmp383=_tmp382->f1;}}_LL21D: if(Cyc_Absyn_qvar_cmp(_tmp380,_tmp383)
== 0)return;{const char*_tmpFEF;Cyc_Tcutil_failure_reason=((_tmpFEF="(different enum types)",
_tag_dyneither(_tmpFEF,sizeof(char),23)));}goto _LL213;_LL21E: _tmp384=_tmp367.f1;{
struct Cyc_Absyn_AnonEnumType_struct*_tmp385=(struct Cyc_Absyn_AnonEnumType_struct*)
_tmp384;if(_tmp385->tag != 15)goto _LL220;else{_tmp386=_tmp385->f1;}}_tmp387=
_tmp367.f2;{struct Cyc_Absyn_AnonEnumType_struct*_tmp388=(struct Cyc_Absyn_AnonEnumType_struct*)
_tmp387;if(_tmp388->tag != 15)goto _LL220;else{_tmp389=_tmp388->f1;}}_LL21F: {int
bad=0;for(0;_tmp386 != 0  && _tmp389 != 0;(_tmp386=_tmp386->tl,_tmp389=_tmp389->tl)){
struct Cyc_Absyn_Enumfield*_tmp43C=(struct Cyc_Absyn_Enumfield*)_tmp386->hd;struct
Cyc_Absyn_Enumfield*_tmp43D=(struct Cyc_Absyn_Enumfield*)_tmp389->hd;if(Cyc_Absyn_qvar_cmp(
_tmp43C->name,_tmp43D->name)!= 0){{const char*_tmpFF0;Cyc_Tcutil_failure_reason=((
_tmpFF0="(different names for enum fields)",_tag_dyneither(_tmpFF0,sizeof(char),
34)));}bad=1;break;}if(_tmp43C->tag == _tmp43D->tag)continue;if(_tmp43C->tag == 0
 || _tmp43D->tag == 0){{const char*_tmpFF1;Cyc_Tcutil_failure_reason=((_tmpFF1="(different tag values for enum fields)",
_tag_dyneither(_tmpFF1,sizeof(char),39)));}bad=1;break;}if(!Cyc_Evexp_same_const_exp((
struct Cyc_Absyn_Exp*)_check_null(_tmp43C->tag),(struct Cyc_Absyn_Exp*)_check_null(
_tmp43D->tag))){{const char*_tmpFF2;Cyc_Tcutil_failure_reason=((_tmpFF2="(different tag values for enum fields)",
_tag_dyneither(_tmpFF2,sizeof(char),39)));}bad=1;break;}}if(bad)goto _LL213;if(
_tmp386 == 0  && _tmp389 == 0)return;{const char*_tmpFF3;Cyc_Tcutil_failure_reason=((
_tmpFF3="(different number of fields for enums)",_tag_dyneither(_tmpFF3,sizeof(
char),39)));}goto _LL213;}_LL220: _tmp38A=_tmp367.f1;{struct Cyc_Absyn_DatatypeType_struct*
_tmp38B=(struct Cyc_Absyn_DatatypeType_struct*)_tmp38A;if(_tmp38B->tag != 3)goto
_LL222;else{_tmp38C=_tmp38B->f1;_tmp38D=_tmp38C.datatype_info;if((_tmp38D.KnownDatatype).tag
!= 2)goto _LL222;_tmp38E=(struct Cyc_Absyn_Datatypedecl**)(_tmp38D.KnownDatatype).val;
_tmp38F=*_tmp38E;_tmp390=_tmp38C.targs;}}_tmp391=_tmp367.f2;{struct Cyc_Absyn_DatatypeType_struct*
_tmp392=(struct Cyc_Absyn_DatatypeType_struct*)_tmp391;if(_tmp392->tag != 3)goto
_LL222;else{_tmp393=_tmp392->f1;_tmp394=_tmp393.datatype_info;if((_tmp394.KnownDatatype).tag
!= 2)goto _LL222;_tmp395=(struct Cyc_Absyn_Datatypedecl**)(_tmp394.KnownDatatype).val;
_tmp396=*_tmp395;_tmp397=_tmp393.targs;}}_LL221: if(_tmp38F == _tmp396  || Cyc_Absyn_qvar_cmp(
_tmp38F->name,_tmp396->name)== 0){Cyc_Tcutil_unify_list(_tmp397,_tmp390);return;}
goto _LL213;_LL222: _tmp398=_tmp367.f1;{struct Cyc_Absyn_DatatypeFieldType_struct*
_tmp399=(struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp398;if(_tmp399->tag != 4)
goto _LL224;else{_tmp39A=_tmp399->f1;_tmp39B=_tmp39A.field_info;if((_tmp39B.KnownDatatypefield).tag
!= 2)goto _LL224;_tmp39C=(struct _tuple3)(_tmp39B.KnownDatatypefield).val;_tmp39D=
_tmp39C.f1;_tmp39E=_tmp39C.f2;_tmp39F=_tmp39A.targs;}}_tmp3A0=_tmp367.f2;{struct
Cyc_Absyn_DatatypeFieldType_struct*_tmp3A1=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp3A0;if(_tmp3A1->tag != 4)goto _LL224;else{_tmp3A2=_tmp3A1->f1;_tmp3A3=_tmp3A2.field_info;
if((_tmp3A3.KnownDatatypefield).tag != 2)goto _LL224;_tmp3A4=(struct _tuple3)(
_tmp3A3.KnownDatatypefield).val;_tmp3A5=_tmp3A4.f1;_tmp3A6=_tmp3A4.f2;_tmp3A7=
_tmp3A2.targs;}}_LL223: if((_tmp39D == _tmp3A5  || Cyc_Absyn_qvar_cmp(_tmp39D->name,
_tmp3A5->name)== 0) && (_tmp39E == _tmp3A6  || Cyc_Absyn_qvar_cmp(_tmp39E->name,
_tmp3A6->name)== 0)){Cyc_Tcutil_unify_list(_tmp3A7,_tmp39F);return;}{const char*
_tmpFF4;Cyc_Tcutil_failure_reason=((_tmpFF4="(different datatype field types)",
_tag_dyneither(_tmpFF4,sizeof(char),33)));}goto _LL213;_LL224: _tmp3A8=_tmp367.f1;{
struct Cyc_Absyn_PointerType_struct*_tmp3A9=(struct Cyc_Absyn_PointerType_struct*)
_tmp3A8;if(_tmp3A9->tag != 5)goto _LL226;else{_tmp3AA=_tmp3A9->f1;_tmp3AB=_tmp3AA.elt_typ;
_tmp3AC=_tmp3AA.elt_tq;_tmp3AD=_tmp3AA.ptr_atts;_tmp3AE=_tmp3AD.rgn;_tmp3AF=
_tmp3AD.nullable;_tmp3B0=_tmp3AD.bounds;_tmp3B1=_tmp3AD.zero_term;}}_tmp3B2=
_tmp367.f2;{struct Cyc_Absyn_PointerType_struct*_tmp3B3=(struct Cyc_Absyn_PointerType_struct*)
_tmp3B2;if(_tmp3B3->tag != 5)goto _LL226;else{_tmp3B4=_tmp3B3->f1;_tmp3B5=_tmp3B4.elt_typ;
_tmp3B6=_tmp3B4.elt_tq;_tmp3B7=_tmp3B4.ptr_atts;_tmp3B8=_tmp3B7.rgn;_tmp3B9=
_tmp3B7.nullable;_tmp3BA=_tmp3B7.bounds;_tmp3BB=_tmp3B7.zero_term;}}_LL225: Cyc_Tcutil_unify_it(
_tmp3B5,_tmp3AB);Cyc_Tcutil_unify_it(_tmp3AE,_tmp3B8);Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;{const char*_tmpFF5;((void(*)(int(*cmp)(int,int),union
Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason))
Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,_tmp3BB,_tmp3B1,((_tmpFF5="(not both zero terminated)",
_tag_dyneither(_tmpFF5,sizeof(char),27))));}Cyc_Tcutil_unify_tqual(_tmp3B6,
_tmp3B5,_tmp3AC,_tmp3AB);{const char*_tmpFF6;((void(*)(int(*cmp)(void*,void*),
union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr
reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp3BA,_tmp3B0,((
_tmpFF6="(different pointer bounds)",_tag_dyneither(_tmpFF6,sizeof(char),27))));}{
void*_tmp445=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,_tmp3BA);_LL24F: {struct Cyc_Absyn_DynEither_b_struct*
_tmp446=(struct Cyc_Absyn_DynEither_b_struct*)_tmp445;if(_tmp446->tag != 0)goto
_LL251;}_LL250: return;_LL251:;_LL252: goto _LL24E;_LL24E:;}{const char*_tmpFF7;((
void(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*
y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,
_tmp3B9,_tmp3AF,((_tmpFF7="(incompatible pointer types)",_tag_dyneither(_tmpFF7,
sizeof(char),29))));}return;_LL226: _tmp3BC=_tmp367.f1;{struct Cyc_Absyn_IntType_struct*
_tmp3BD=(struct Cyc_Absyn_IntType_struct*)_tmp3BC;if(_tmp3BD->tag != 6)goto _LL228;
else{_tmp3BE=_tmp3BD->f1;_tmp3BF=_tmp3BD->f2;}}_tmp3C0=_tmp367.f2;{struct Cyc_Absyn_IntType_struct*
_tmp3C1=(struct Cyc_Absyn_IntType_struct*)_tmp3C0;if(_tmp3C1->tag != 6)goto _LL228;
else{_tmp3C2=_tmp3C1->f1;_tmp3C3=_tmp3C1->f2;}}_LL227: if(_tmp3C2 == _tmp3BE  && ((
_tmp3C3 == _tmp3BF  || _tmp3C3 == Cyc_Absyn_Int_sz  && _tmp3BF == Cyc_Absyn_Long_sz)
 || _tmp3C3 == Cyc_Absyn_Long_sz  && _tmp3BF == Cyc_Absyn_Int_sz))return;{const char*
_tmpFF8;Cyc_Tcutil_failure_reason=((_tmpFF8="(different integral types)",
_tag_dyneither(_tmpFF8,sizeof(char),27)));}goto _LL213;_LL228: _tmp3C4=_tmp367.f1;{
struct Cyc_Absyn_FloatType_struct*_tmp3C5=(struct Cyc_Absyn_FloatType_struct*)
_tmp3C4;if(_tmp3C5->tag != 7)goto _LL22A;}_tmp3C6=_tmp367.f2;{struct Cyc_Absyn_FloatType_struct*
_tmp3C7=(struct Cyc_Absyn_FloatType_struct*)_tmp3C6;if(_tmp3C7->tag != 7)goto
_LL22A;}_LL229: return;_LL22A: _tmp3C8=_tmp367.f1;{struct Cyc_Absyn_DoubleType_struct*
_tmp3C9=(struct Cyc_Absyn_DoubleType_struct*)_tmp3C8;if(_tmp3C9->tag != 8)goto
_LL22C;else{_tmp3CA=_tmp3C9->f1;}}_tmp3CB=_tmp367.f2;{struct Cyc_Absyn_DoubleType_struct*
_tmp3CC=(struct Cyc_Absyn_DoubleType_struct*)_tmp3CB;if(_tmp3CC->tag != 8)goto
_LL22C;else{_tmp3CD=_tmp3CC->f1;}}_LL22B: if(_tmp3CA == _tmp3CD)return;goto _LL213;
_LL22C: _tmp3CE=_tmp367.f1;{struct Cyc_Absyn_TagType_struct*_tmp3CF=(struct Cyc_Absyn_TagType_struct*)
_tmp3CE;if(_tmp3CF->tag != 20)goto _LL22E;else{_tmp3D0=(void*)_tmp3CF->f1;}}
_tmp3D1=_tmp367.f2;{struct Cyc_Absyn_TagType_struct*_tmp3D2=(struct Cyc_Absyn_TagType_struct*)
_tmp3D1;if(_tmp3D2->tag != 20)goto _LL22E;else{_tmp3D3=(void*)_tmp3D2->f1;}}_LL22D:
Cyc_Tcutil_unify_it(_tmp3D0,_tmp3D3);return;_LL22E: _tmp3D4=_tmp367.f1;{struct Cyc_Absyn_ValueofType_struct*
_tmp3D5=(struct Cyc_Absyn_ValueofType_struct*)_tmp3D4;if(_tmp3D5->tag != 19)goto
_LL230;else{_tmp3D6=_tmp3D5->f1;}}_tmp3D7=_tmp367.f2;{struct Cyc_Absyn_ValueofType_struct*
_tmp3D8=(struct Cyc_Absyn_ValueofType_struct*)_tmp3D7;if(_tmp3D8->tag != 19)goto
_LL230;else{_tmp3D9=_tmp3D8->f1;}}_LL22F: if(!Cyc_Evexp_same_const_exp(_tmp3D6,
_tmp3D9)){{const char*_tmpFF9;Cyc_Tcutil_failure_reason=((_tmpFF9="(cannot prove expressions are the same)",
_tag_dyneither(_tmpFF9,sizeof(char),40)));}goto _LL213;}return;_LL230: _tmp3DA=
_tmp367.f1;{struct Cyc_Absyn_ArrayType_struct*_tmp3DB=(struct Cyc_Absyn_ArrayType_struct*)
_tmp3DA;if(_tmp3DB->tag != 9)goto _LL232;else{_tmp3DC=_tmp3DB->f1;_tmp3DD=_tmp3DC.elt_type;
_tmp3DE=_tmp3DC.tq;_tmp3DF=_tmp3DC.num_elts;_tmp3E0=_tmp3DC.zero_term;}}_tmp3E1=
_tmp367.f2;{struct Cyc_Absyn_ArrayType_struct*_tmp3E2=(struct Cyc_Absyn_ArrayType_struct*)
_tmp3E1;if(_tmp3E2->tag != 9)goto _LL232;else{_tmp3E3=_tmp3E2->f1;_tmp3E4=_tmp3E3.elt_type;
_tmp3E5=_tmp3E3.tq;_tmp3E6=_tmp3E3.num_elts;_tmp3E7=_tmp3E3.zero_term;}}_LL231:
Cyc_Tcutil_unify_it(_tmp3E4,_tmp3DD);Cyc_Tcutil_unify_tqual(_tmp3E5,_tmp3E4,
_tmp3DE,_tmp3DD);Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;{const char*
_tmpFFA;((void(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*
y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,
_tmp3E0,_tmp3E7,((_tmpFFA="(not both zero terminated)",_tag_dyneither(_tmpFFA,
sizeof(char),27))));}if(_tmp3DF == _tmp3E6)return;if(_tmp3DF == 0  || _tmp3E6 == 0)
goto _LL213;if(Cyc_Evexp_same_const_exp((struct Cyc_Absyn_Exp*)_tmp3DF,(struct Cyc_Absyn_Exp*)
_tmp3E6))return;{const char*_tmpFFB;Cyc_Tcutil_failure_reason=((_tmpFFB="(different array sizes)",
_tag_dyneither(_tmpFFB,sizeof(char),24)));}goto _LL213;_LL232: _tmp3E8=_tmp367.f1;{
struct Cyc_Absyn_FnType_struct*_tmp3E9=(struct Cyc_Absyn_FnType_struct*)_tmp3E8;
if(_tmp3E9->tag != 10)goto _LL234;else{_tmp3EA=_tmp3E9->f1;_tmp3EB=_tmp3EA.tvars;
_tmp3EC=_tmp3EA.effect;_tmp3ED=_tmp3EA.ret_typ;_tmp3EE=_tmp3EA.args;_tmp3EF=
_tmp3EA.c_varargs;_tmp3F0=_tmp3EA.cyc_varargs;_tmp3F1=_tmp3EA.rgn_po;_tmp3F2=
_tmp3EA.attributes;}}_tmp3F3=_tmp367.f2;{struct Cyc_Absyn_FnType_struct*_tmp3F4=(
struct Cyc_Absyn_FnType_struct*)_tmp3F3;if(_tmp3F4->tag != 10)goto _LL234;else{
_tmp3F5=_tmp3F4->f1;_tmp3F6=_tmp3F5.tvars;_tmp3F7=_tmp3F5.effect;_tmp3F8=_tmp3F5.ret_typ;
_tmp3F9=_tmp3F5.args;_tmp3FA=_tmp3F5.c_varargs;_tmp3FB=_tmp3F5.cyc_varargs;
_tmp3FC=_tmp3F5.rgn_po;_tmp3FD=_tmp3F5.attributes;}}_LL233: {int done=0;struct
_RegionHandle _tmp44C=_new_region("rgn");struct _RegionHandle*rgn=& _tmp44C;
_push_region(rgn);{struct Cyc_List_List*inst=0;while(_tmp3F6 != 0){if(_tmp3EB == 0){{
const char*_tmpFFC;Cyc_Tcutil_failure_reason=((_tmpFFC="(second function type has too few type variables)",
_tag_dyneither(_tmpFFC,sizeof(char),50)));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}{
void*_tmp44E=((struct Cyc_Absyn_Tvar*)_tmp3F6->hd)->kind;void*_tmp44F=((struct Cyc_Absyn_Tvar*)
_tmp3EB->hd)->kind;if(!Cyc_Tcutil_unify_kindbound(_tmp44E,_tmp44F)){{const char*
_tmp1002;void*_tmp1001[3];struct Cyc_String_pa_struct _tmp1000;struct Cyc_String_pa_struct
_tmpFFF;struct Cyc_String_pa_struct _tmpFFE;Cyc_Tcutil_failure_reason=(struct
_dyneither_ptr)((_tmpFFE.tag=0,((_tmpFFE.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_kind2string(Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)
_tmp3EB->hd,Cyc_Absyn_BoxKind))),((_tmpFFF.tag=0,((_tmpFFF.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(Cyc_Tcutil_tvar_kind((
struct Cyc_Absyn_Tvar*)_tmp3F6->hd,Cyc_Absyn_BoxKind))),((_tmp1000.tag=0,((
_tmp1000.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_tvar2string((
struct Cyc_Absyn_Tvar*)_tmp3F6->hd)),((_tmp1001[0]=& _tmp1000,((_tmp1001[1]=&
_tmpFFF,((_tmp1001[2]=& _tmpFFE,Cyc_aprintf(((_tmp1002="(type var %s has different kinds %s and %s)",
_tag_dyneither(_tmp1002,sizeof(char),44))),_tag_dyneither(_tmp1001,sizeof(void*),
3))))))))))))))))))));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}{struct
_tuple14*_tmp100C;struct Cyc_Absyn_VarType_struct _tmp100B;struct Cyc_Absyn_VarType_struct*
_tmp100A;struct Cyc_List_List*_tmp1009;inst=((_tmp1009=_region_malloc(rgn,sizeof(*
_tmp1009)),((_tmp1009->hd=((_tmp100C=_region_malloc(rgn,sizeof(*_tmp100C)),((
_tmp100C->f1=(struct Cyc_Absyn_Tvar*)_tmp3EB->hd,((_tmp100C->f2=(void*)((_tmp100A=
_cycalloc(sizeof(*_tmp100A)),((_tmp100A[0]=((_tmp100B.tag=2,((_tmp100B.f1=(
struct Cyc_Absyn_Tvar*)_tmp3F6->hd,_tmp100B)))),_tmp100A)))),_tmp100C)))))),((
_tmp1009->tl=inst,_tmp1009))))));}_tmp3F6=_tmp3F6->tl;_tmp3EB=_tmp3EB->tl;}}if(
_tmp3EB != 0){{const char*_tmp100D;Cyc_Tcutil_failure_reason=((_tmp100D="(second function type has too many type variables)",
_tag_dyneither(_tmp100D,sizeof(char),51)));}_npop_handler(0);goto _LL213;}if(inst
!= 0){{struct Cyc_Absyn_FnType_struct _tmp1019;struct Cyc_Absyn_FnInfo _tmp1018;
struct Cyc_Absyn_FnType_struct*_tmp1017;struct Cyc_Absyn_FnType_struct _tmp1013;
struct Cyc_Absyn_FnInfo _tmp1012;struct Cyc_Absyn_FnType_struct*_tmp1011;Cyc_Tcutil_unify_it((
void*)((_tmp1011=_cycalloc(sizeof(*_tmp1011)),((_tmp1011[0]=((_tmp1013.tag=10,((
_tmp1013.f1=((_tmp1012.tvars=0,((_tmp1012.effect=_tmp3F7,((_tmp1012.ret_typ=
_tmp3F8,((_tmp1012.args=_tmp3F9,((_tmp1012.c_varargs=_tmp3FA,((_tmp1012.cyc_varargs=
_tmp3FB,((_tmp1012.rgn_po=_tmp3FC,((_tmp1012.attributes=_tmp3FD,_tmp1012)))))))))))))))),
_tmp1013)))),_tmp1011)))),Cyc_Tcutil_rsubstitute(rgn,inst,(void*)((_tmp1017=
_cycalloc(sizeof(*_tmp1017)),((_tmp1017[0]=((_tmp1019.tag=10,((_tmp1019.f1=((
_tmp1018.tvars=0,((_tmp1018.effect=_tmp3EC,((_tmp1018.ret_typ=_tmp3ED,((_tmp1018.args=
_tmp3EE,((_tmp1018.c_varargs=_tmp3EF,((_tmp1018.cyc_varargs=_tmp3F0,((_tmp1018.rgn_po=
_tmp3F1,((_tmp1018.attributes=_tmp3F2,_tmp1018)))))))))))))))),_tmp1019)))),
_tmp1017))))));}done=1;}}if(done){_npop_handler(0);return;}Cyc_Tcutil_unify_it(
_tmp3F8,_tmp3ED);for(0;_tmp3F9 != 0  && _tmp3EE != 0;(_tmp3F9=_tmp3F9->tl,_tmp3EE=
_tmp3EE->tl)){struct Cyc_Absyn_Tqual _tmp461;void*_tmp462;struct _tuple9 _tmp460=*((
struct _tuple9*)_tmp3F9->hd);_tmp461=_tmp460.f2;_tmp462=_tmp460.f3;{struct Cyc_Absyn_Tqual
_tmp464;void*_tmp465;struct _tuple9 _tmp463=*((struct _tuple9*)_tmp3EE->hd);_tmp464=
_tmp463.f2;_tmp465=_tmp463.f3;Cyc_Tcutil_unify_it(_tmp462,_tmp465);Cyc_Tcutil_unify_tqual(
_tmp461,_tmp462,_tmp464,_tmp465);}}Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=
t2;if(_tmp3F9 != 0  || _tmp3EE != 0){{const char*_tmp101A;Cyc_Tcutil_failure_reason=((
_tmp101A="(function types have different number of arguments)",_tag_dyneither(
_tmp101A,sizeof(char),52)));}_npop_handler(0);goto _LL213;}if(_tmp3FA != _tmp3EF){{
const char*_tmp101B;Cyc_Tcutil_failure_reason=((_tmp101B="(only one function type takes C varargs)",
_tag_dyneither(_tmp101B,sizeof(char),41)));}_npop_handler(0);goto _LL213;}{int
bad_cyc_vararg=0;{struct _tuple18 _tmp101C;struct _tuple18 _tmp469=(_tmp101C.f1=
_tmp3FB,((_tmp101C.f2=_tmp3F0,_tmp101C)));struct Cyc_Absyn_VarargInfo*_tmp46A;
struct Cyc_Absyn_VarargInfo*_tmp46B;struct Cyc_Absyn_VarargInfo*_tmp46C;struct Cyc_Absyn_VarargInfo*
_tmp46D;struct Cyc_Absyn_VarargInfo*_tmp46E;struct Cyc_Absyn_VarargInfo _tmp46F;
struct Cyc_Core_Opt*_tmp470;struct Cyc_Absyn_Tqual _tmp471;void*_tmp472;int _tmp473;
struct Cyc_Absyn_VarargInfo*_tmp474;struct Cyc_Absyn_VarargInfo _tmp475;struct Cyc_Core_Opt*
_tmp476;struct Cyc_Absyn_Tqual _tmp477;void*_tmp478;int _tmp479;_LL254: _tmp46A=
_tmp469.f1;if(_tmp46A != 0)goto _LL256;_tmp46B=_tmp469.f2;if(_tmp46B != 0)goto
_LL256;_LL255: goto _LL253;_LL256: _tmp46C=_tmp469.f1;if(_tmp46C != 0)goto _LL258;
_LL257: goto _LL259;_LL258: _tmp46D=_tmp469.f2;if(_tmp46D != 0)goto _LL25A;_LL259:
bad_cyc_vararg=1;{const char*_tmp101D;Cyc_Tcutil_failure_reason=((_tmp101D="(only one function type takes varargs)",
_tag_dyneither(_tmp101D,sizeof(char),39)));}goto _LL253;_LL25A: _tmp46E=_tmp469.f1;
if(_tmp46E == 0)goto _LL253;_tmp46F=*_tmp46E;_tmp470=_tmp46F.name;_tmp471=_tmp46F.tq;
_tmp472=_tmp46F.type;_tmp473=_tmp46F.inject;_tmp474=_tmp469.f2;if(_tmp474 == 0)
goto _LL253;_tmp475=*_tmp474;_tmp476=_tmp475.name;_tmp477=_tmp475.tq;_tmp478=
_tmp475.type;_tmp479=_tmp475.inject;_LL25B: Cyc_Tcutil_unify_it(_tmp472,_tmp478);
Cyc_Tcutil_unify_tqual(_tmp471,_tmp472,_tmp477,_tmp478);if(_tmp473 != _tmp479){
bad_cyc_vararg=1;{const char*_tmp101E;Cyc_Tcutil_failure_reason=((_tmp101E="(only one function type injects varargs)",
_tag_dyneither(_tmp101E,sizeof(char),41)));}}goto _LL253;_LL253:;}if(
bad_cyc_vararg){_npop_handler(0);goto _LL213;}{int bad_effect=0;{struct _tuple19
_tmp101F;struct _tuple19 _tmp47D=(_tmp101F.f1=_tmp3F7,((_tmp101F.f2=_tmp3EC,
_tmp101F)));struct Cyc_Core_Opt*_tmp47E;struct Cyc_Core_Opt*_tmp47F;struct Cyc_Core_Opt*
_tmp480;struct Cyc_Core_Opt*_tmp481;_LL25D: _tmp47E=_tmp47D.f1;if(_tmp47E != 0)goto
_LL25F;_tmp47F=_tmp47D.f2;if(_tmp47F != 0)goto _LL25F;_LL25E: goto _LL25C;_LL25F:
_tmp480=_tmp47D.f1;if(_tmp480 != 0)goto _LL261;_LL260: goto _LL262;_LL261: _tmp481=
_tmp47D.f2;if(_tmp481 != 0)goto _LL263;_LL262: bad_effect=1;goto _LL25C;_LL263:;
_LL264: bad_effect=!Cyc_Tcutil_unify_effect((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp3F7))->v,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp3EC))->v);
goto _LL25C;_LL25C:;}Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;if(
bad_effect){{const char*_tmp1020;Cyc_Tcutil_failure_reason=((_tmp1020="(function type effects do not unify)",
_tag_dyneither(_tmp1020,sizeof(char),37)));}_npop_handler(0);goto _LL213;}if(!Cyc_Tcutil_same_atts(
_tmp3F2,_tmp3FD)){{const char*_tmp1021;Cyc_Tcutil_failure_reason=((_tmp1021="(function types have different attributes)",
_tag_dyneither(_tmp1021,sizeof(char),43)));}_npop_handler(0);goto _LL213;}if(!Cyc_Tcutil_same_rgn_po(
_tmp3F1,_tmp3FC)){{const char*_tmp1022;Cyc_Tcutil_failure_reason=((_tmp1022="(function types have different region lifetime orderings)",
_tag_dyneither(_tmp1022,sizeof(char),58)));}_npop_handler(0);goto _LL213;}
_npop_handler(0);return;}};_pop_region(rgn);}_LL234: _tmp3FE=_tmp367.f1;{struct
Cyc_Absyn_TupleType_struct*_tmp3FF=(struct Cyc_Absyn_TupleType_struct*)_tmp3FE;
if(_tmp3FF->tag != 11)goto _LL236;else{_tmp400=_tmp3FF->f1;}}_tmp401=_tmp367.f2;{
struct Cyc_Absyn_TupleType_struct*_tmp402=(struct Cyc_Absyn_TupleType_struct*)
_tmp401;if(_tmp402->tag != 11)goto _LL236;else{_tmp403=_tmp402->f1;}}_LL235: for(0;
_tmp403 != 0  && _tmp400 != 0;(_tmp403=_tmp403->tl,_tmp400=_tmp400->tl)){struct Cyc_Absyn_Tqual
_tmp486;void*_tmp487;struct _tuple11 _tmp485=*((struct _tuple11*)_tmp403->hd);
_tmp486=_tmp485.f1;_tmp487=_tmp485.f2;{struct Cyc_Absyn_Tqual _tmp489;void*_tmp48A;
struct _tuple11 _tmp488=*((struct _tuple11*)_tmp400->hd);_tmp489=_tmp488.f1;_tmp48A=
_tmp488.f2;Cyc_Tcutil_unify_it(_tmp487,_tmp48A);Cyc_Tcutil_unify_tqual(_tmp486,
_tmp487,_tmp489,_tmp48A);}}if(_tmp403 == 0  && _tmp400 == 0)return;Cyc_Tcutil_t1_failure=
t1;Cyc_Tcutil_t2_failure=t2;{const char*_tmp1023;Cyc_Tcutil_failure_reason=((
_tmp1023="(tuple types have different numbers of components)",_tag_dyneither(
_tmp1023,sizeof(char),51)));}goto _LL213;_LL236: _tmp404=_tmp367.f1;{struct Cyc_Absyn_AnonAggrType_struct*
_tmp405=(struct Cyc_Absyn_AnonAggrType_struct*)_tmp404;if(_tmp405->tag != 13)goto
_LL238;else{_tmp406=_tmp405->f1;_tmp407=_tmp405->f2;}}_tmp408=_tmp367.f2;{struct
Cyc_Absyn_AnonAggrType_struct*_tmp409=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp408;if(_tmp409->tag != 13)goto _LL238;else{_tmp40A=_tmp409->f1;_tmp40B=_tmp409->f2;}}
_LL237: if(_tmp40A != _tmp406){{const char*_tmp1024;Cyc_Tcutil_failure_reason=((
_tmp1024="(struct and union type)",_tag_dyneither(_tmp1024,sizeof(char),24)));}
goto _LL213;}for(0;_tmp40B != 0  && _tmp407 != 0;(_tmp40B=_tmp40B->tl,_tmp407=
_tmp407->tl)){struct Cyc_Absyn_Aggrfield*_tmp48D=(struct Cyc_Absyn_Aggrfield*)
_tmp40B->hd;struct Cyc_Absyn_Aggrfield*_tmp48E=(struct Cyc_Absyn_Aggrfield*)
_tmp407->hd;if(Cyc_strptrcmp(_tmp48D->name,_tmp48E->name)!= 0){{const char*
_tmp1025;Cyc_Tcutil_failure_reason=((_tmp1025="(different member names)",
_tag_dyneither(_tmp1025,sizeof(char),25)));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
Cyc_Tcutil_unify_it(_tmp48D->type,_tmp48E->type);Cyc_Tcutil_unify_tqual(_tmp48D->tq,
_tmp48D->type,_tmp48E->tq,_tmp48E->type);if(!Cyc_Tcutil_same_atts(_tmp48D->attributes,
_tmp48E->attributes)){Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;{const
char*_tmp1026;Cyc_Tcutil_failure_reason=((_tmp1026="(different attributes on member)",
_tag_dyneither(_tmp1026,sizeof(char),33)));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
if((_tmp48D->width != 0  && _tmp48E->width == 0  || _tmp48E->width != 0  && _tmp48D->width
== 0) || (_tmp48D->width != 0  && _tmp48E->width != 0) && !Cyc_Evexp_same_const_exp((
struct Cyc_Absyn_Exp*)_check_null(_tmp48D->width),(struct Cyc_Absyn_Exp*)
_check_null(_tmp48E->width))){Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;{
const char*_tmp1027;Cyc_Tcutil_failure_reason=((_tmp1027="(different bitfield widths on member)",
_tag_dyneither(_tmp1027,sizeof(char),38)));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}}
if(_tmp40B == 0  && _tmp407 == 0)return;Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=
t2;{const char*_tmp1028;Cyc_Tcutil_failure_reason=((_tmp1028="(different number of members)",
_tag_dyneither(_tmp1028,sizeof(char),30)));}goto _LL213;_LL238: _tmp40C=_tmp367.f1;{
struct Cyc_Absyn_HeapRgn_struct*_tmp40D=(struct Cyc_Absyn_HeapRgn_struct*)_tmp40C;
if(_tmp40D->tag != 21)goto _LL23A;}_tmp40E=_tmp367.f2;{struct Cyc_Absyn_HeapRgn_struct*
_tmp40F=(struct Cyc_Absyn_HeapRgn_struct*)_tmp40E;if(_tmp40F->tag != 21)goto _LL23A;}
_LL239: return;_LL23A: _tmp410=_tmp367.f1;{struct Cyc_Absyn_UniqueRgn_struct*
_tmp411=(struct Cyc_Absyn_UniqueRgn_struct*)_tmp410;if(_tmp411->tag != 22)goto
_LL23C;}_tmp412=_tmp367.f2;{struct Cyc_Absyn_UniqueRgn_struct*_tmp413=(struct Cyc_Absyn_UniqueRgn_struct*)
_tmp412;if(_tmp413->tag != 22)goto _LL23C;}_LL23B: return;_LL23C: _tmp414=_tmp367.f1;{
struct Cyc_Absyn_RgnHandleType_struct*_tmp415=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp414;if(_tmp415->tag != 16)goto _LL23E;else{_tmp416=(void*)_tmp415->f1;}}
_tmp417=_tmp367.f2;{struct Cyc_Absyn_RgnHandleType_struct*_tmp418=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp417;if(_tmp418->tag != 16)goto _LL23E;else{_tmp419=(void*)_tmp418->f1;}}_LL23D:
Cyc_Tcutil_unify_it(_tmp416,_tmp419);return;_LL23E: _tmp41A=_tmp367.f1;{struct Cyc_Absyn_DynRgnType_struct*
_tmp41B=(struct Cyc_Absyn_DynRgnType_struct*)_tmp41A;if(_tmp41B->tag != 17)goto
_LL240;else{_tmp41C=(void*)_tmp41B->f1;_tmp41D=(void*)_tmp41B->f2;}}_tmp41E=
_tmp367.f2;{struct Cyc_Absyn_DynRgnType_struct*_tmp41F=(struct Cyc_Absyn_DynRgnType_struct*)
_tmp41E;if(_tmp41F->tag != 17)goto _LL240;else{_tmp420=(void*)_tmp41F->f1;_tmp421=(
void*)_tmp41F->f2;}}_LL23F: Cyc_Tcutil_unify_it(_tmp41C,_tmp420);Cyc_Tcutil_unify_it(
_tmp41D,_tmp421);return;_LL240: _tmp422=_tmp367.f1;{struct Cyc_Absyn_JoinEff_struct*
_tmp423=(struct Cyc_Absyn_JoinEff_struct*)_tmp422;if(_tmp423->tag != 24)goto _LL242;}
_LL241: goto _LL243;_LL242: _tmp424=_tmp367.f2;{struct Cyc_Absyn_JoinEff_struct*
_tmp425=(struct Cyc_Absyn_JoinEff_struct*)_tmp424;if(_tmp425->tag != 24)goto _LL244;}
_LL243: goto _LL245;_LL244: _tmp426=_tmp367.f1;{struct Cyc_Absyn_AccessEff_struct*
_tmp427=(struct Cyc_Absyn_AccessEff_struct*)_tmp426;if(_tmp427->tag != 23)goto
_LL246;}_LL245: goto _LL247;_LL246: _tmp428=_tmp367.f1;{struct Cyc_Absyn_RgnsEff_struct*
_tmp429=(struct Cyc_Absyn_RgnsEff_struct*)_tmp428;if(_tmp429->tag != 25)goto _LL248;}
_LL247: goto _LL249;_LL248: _tmp42A=_tmp367.f2;{struct Cyc_Absyn_RgnsEff_struct*
_tmp42B=(struct Cyc_Absyn_RgnsEff_struct*)_tmp42A;if(_tmp42B->tag != 25)goto _LL24A;}
_LL249: goto _LL24B;_LL24A: _tmp42C=_tmp367.f2;{struct Cyc_Absyn_AccessEff_struct*
_tmp42D=(struct Cyc_Absyn_AccessEff_struct*)_tmp42C;if(_tmp42D->tag != 23)goto
_LL24C;}_LL24B: if(Cyc_Tcutil_unify_effect(t1,t2))return;{const char*_tmp1029;Cyc_Tcutil_failure_reason=((
_tmp1029="(effects don't unify)",_tag_dyneither(_tmp1029,sizeof(char),22)));}
goto _LL213;_LL24C:;_LL24D: goto _LL213;_LL213:;}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
int Cyc_Tcutil_star_cmp(int(*cmp)(void*,void*),void*a1,void*a2);int Cyc_Tcutil_star_cmp(
int(*cmp)(void*,void*),void*a1,void*a2){if(a1 == a2)return 0;if(a1 == 0  && a2 != 0)
return - 1;if(a1 != 0  && a2 == 0)return 1;return cmp((void*)_check_null(a1),(void*)
_check_null(a2));}static int Cyc_Tcutil_tqual_cmp(struct Cyc_Absyn_Tqual tq1,struct
Cyc_Absyn_Tqual tq2);static int Cyc_Tcutil_tqual_cmp(struct Cyc_Absyn_Tqual tq1,
struct Cyc_Absyn_Tqual tq2){int _tmp494=(tq1.real_const + (tq1.q_volatile << 1))+ (
tq1.q_restrict << 2);int _tmp495=(tq2.real_const + (tq2.q_volatile << 1))+ (tq2.q_restrict
<< 2);return Cyc_Core_intcmp(_tmp494,_tmp495);}static int Cyc_Tcutil_conrefs_cmp(
int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y);
static int Cyc_Tcutil_conrefs_cmp(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*
x,union Cyc_Absyn_Constraint*y){x=Cyc_Absyn_compress_conref(x);y=Cyc_Absyn_compress_conref(
y);if(x == y)return 0;{union Cyc_Absyn_Constraint*_tmp496=x;union Cyc_Absyn_Constraint
_tmp497;int _tmp498;union Cyc_Absyn_Constraint _tmp499;void*_tmp49A;union Cyc_Absyn_Constraint
_tmp49B;union Cyc_Absyn_Constraint*_tmp49C;_LL266: _tmp497=*_tmp496;if((_tmp497.No_constr).tag
!= 3)goto _LL268;_tmp498=(int)(_tmp497.No_constr).val;_LL267: return - 1;_LL268:
_tmp499=*_tmp496;if((_tmp499.Eq_constr).tag != 1)goto _LL26A;_tmp49A=(void*)(
_tmp499.Eq_constr).val;_LL269: {union Cyc_Absyn_Constraint*_tmp49D=y;union Cyc_Absyn_Constraint
_tmp49E;int _tmp49F;union Cyc_Absyn_Constraint _tmp4A0;void*_tmp4A1;union Cyc_Absyn_Constraint
_tmp4A2;union Cyc_Absyn_Constraint*_tmp4A3;_LL26D: _tmp49E=*_tmp49D;if((_tmp49E.No_constr).tag
!= 3)goto _LL26F;_tmp49F=(int)(_tmp49E.No_constr).val;_LL26E: return 1;_LL26F:
_tmp4A0=*_tmp49D;if((_tmp4A0.Eq_constr).tag != 1)goto _LL271;_tmp4A1=(void*)(
_tmp4A0.Eq_constr).val;_LL270: return cmp(_tmp49A,_tmp4A1);_LL271: _tmp4A2=*_tmp49D;
if((_tmp4A2.Forward_constr).tag != 2)goto _LL26C;_tmp4A3=(union Cyc_Absyn_Constraint*)(
_tmp4A2.Forward_constr).val;_LL272: {const char*_tmp102C;void*_tmp102B;(_tmp102B=
0,Cyc_Tcutil_impos(((_tmp102C="unify_conref: forward after compress(2)",
_tag_dyneither(_tmp102C,sizeof(char),40))),_tag_dyneither(_tmp102B,sizeof(void*),
0)));}_LL26C:;}_LL26A: _tmp49B=*_tmp496;if((_tmp49B.Forward_constr).tag != 2)goto
_LL265;_tmp49C=(union Cyc_Absyn_Constraint*)(_tmp49B.Forward_constr).val;_LL26B: {
const char*_tmp102F;void*_tmp102E;(_tmp102E=0,Cyc_Tcutil_impos(((_tmp102F="unify_conref: forward after compress",
_tag_dyneither(_tmp102F,sizeof(char),37))),_tag_dyneither(_tmp102E,sizeof(void*),
0)));}_LL265:;}}static int Cyc_Tcutil_tqual_type_cmp(struct _tuple11*tqt1,struct
_tuple11*tqt2);static int Cyc_Tcutil_tqual_type_cmp(struct _tuple11*tqt1,struct
_tuple11*tqt2){struct _tuple11 _tmp4A9;struct Cyc_Absyn_Tqual _tmp4AA;void*_tmp4AB;
struct _tuple11*_tmp4A8=tqt1;_tmp4A9=*_tmp4A8;_tmp4AA=_tmp4A9.f1;_tmp4AB=_tmp4A9.f2;{
struct _tuple11 _tmp4AD;struct Cyc_Absyn_Tqual _tmp4AE;void*_tmp4AF;struct _tuple11*
_tmp4AC=tqt2;_tmp4AD=*_tmp4AC;_tmp4AE=_tmp4AD.f1;_tmp4AF=_tmp4AD.f2;{int _tmp4B0=
Cyc_Tcutil_tqual_cmp(_tmp4AA,_tmp4AE);if(_tmp4B0 != 0)return _tmp4B0;return Cyc_Tcutil_typecmp(
_tmp4AB,_tmp4AF);}}}static int Cyc_Tcutil_aggrfield_cmp(struct Cyc_Absyn_Aggrfield*
f1,struct Cyc_Absyn_Aggrfield*f2);static int Cyc_Tcutil_aggrfield_cmp(struct Cyc_Absyn_Aggrfield*
f1,struct Cyc_Absyn_Aggrfield*f2){int _tmp4B1=Cyc_strptrcmp(f1->name,f2->name);if(
_tmp4B1 != 0)return _tmp4B1;{int _tmp4B2=Cyc_Tcutil_tqual_cmp(f1->tq,f2->tq);if(
_tmp4B2 != 0)return _tmp4B2;{int _tmp4B3=Cyc_Tcutil_typecmp(f1->type,f2->type);if(
_tmp4B3 != 0)return _tmp4B3;{int _tmp4B4=((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*
l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_attribute_cmp,f1->attributes,
f2->attributes);if(_tmp4B4 != 0)return _tmp4B4;return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(
Cyc_Evexp_const_exp_cmp,f1->width,f2->width);}}}}static int Cyc_Tcutil_enumfield_cmp(
struct Cyc_Absyn_Enumfield*e1,struct Cyc_Absyn_Enumfield*e2);static int Cyc_Tcutil_enumfield_cmp(
struct Cyc_Absyn_Enumfield*e1,struct Cyc_Absyn_Enumfield*e2){int _tmp4B5=Cyc_Absyn_qvar_cmp(
e1->name,e2->name);if(_tmp4B5 != 0)return _tmp4B5;return((int(*)(int(*cmp)(struct
Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*
a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,e1->tag,e2->tag);}static int Cyc_Tcutil_type_case_number(
void*t);static int Cyc_Tcutil_type_case_number(void*t){void*_tmp4B6=t;_LL274: {
struct Cyc_Absyn_VoidType_struct*_tmp4B7=(struct Cyc_Absyn_VoidType_struct*)
_tmp4B6;if(_tmp4B7->tag != 0)goto _LL276;}_LL275: return 0;_LL276: {struct Cyc_Absyn_Evar_struct*
_tmp4B8=(struct Cyc_Absyn_Evar_struct*)_tmp4B6;if(_tmp4B8->tag != 1)goto _LL278;}
_LL277: return 1;_LL278: {struct Cyc_Absyn_VarType_struct*_tmp4B9=(struct Cyc_Absyn_VarType_struct*)
_tmp4B6;if(_tmp4B9->tag != 2)goto _LL27A;}_LL279: return 2;_LL27A: {struct Cyc_Absyn_DatatypeType_struct*
_tmp4BA=(struct Cyc_Absyn_DatatypeType_struct*)_tmp4B6;if(_tmp4BA->tag != 3)goto
_LL27C;}_LL27B: return 3;_LL27C: {struct Cyc_Absyn_DatatypeFieldType_struct*_tmp4BB=(
struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp4B6;if(_tmp4BB->tag != 4)goto _LL27E;}
_LL27D: return 4;_LL27E: {struct Cyc_Absyn_PointerType_struct*_tmp4BC=(struct Cyc_Absyn_PointerType_struct*)
_tmp4B6;if(_tmp4BC->tag != 5)goto _LL280;}_LL27F: return 5;_LL280: {struct Cyc_Absyn_IntType_struct*
_tmp4BD=(struct Cyc_Absyn_IntType_struct*)_tmp4B6;if(_tmp4BD->tag != 6)goto _LL282;}
_LL281: return 6;_LL282: {struct Cyc_Absyn_FloatType_struct*_tmp4BE=(struct Cyc_Absyn_FloatType_struct*)
_tmp4B6;if(_tmp4BE->tag != 7)goto _LL284;}_LL283: return 7;_LL284: {struct Cyc_Absyn_DoubleType_struct*
_tmp4BF=(struct Cyc_Absyn_DoubleType_struct*)_tmp4B6;if(_tmp4BF->tag != 8)goto
_LL286;}_LL285: return 8;_LL286: {struct Cyc_Absyn_ArrayType_struct*_tmp4C0=(struct
Cyc_Absyn_ArrayType_struct*)_tmp4B6;if(_tmp4C0->tag != 9)goto _LL288;}_LL287:
return 9;_LL288: {struct Cyc_Absyn_FnType_struct*_tmp4C1=(struct Cyc_Absyn_FnType_struct*)
_tmp4B6;if(_tmp4C1->tag != 10)goto _LL28A;}_LL289: return 10;_LL28A: {struct Cyc_Absyn_TupleType_struct*
_tmp4C2=(struct Cyc_Absyn_TupleType_struct*)_tmp4B6;if(_tmp4C2->tag != 11)goto
_LL28C;}_LL28B: return 11;_LL28C: {struct Cyc_Absyn_AggrType_struct*_tmp4C3=(struct
Cyc_Absyn_AggrType_struct*)_tmp4B6;if(_tmp4C3->tag != 12)goto _LL28E;}_LL28D:
return 12;_LL28E: {struct Cyc_Absyn_AnonAggrType_struct*_tmp4C4=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp4B6;if(_tmp4C4->tag != 13)goto _LL290;}_LL28F: return 14;_LL290: {struct Cyc_Absyn_EnumType_struct*
_tmp4C5=(struct Cyc_Absyn_EnumType_struct*)_tmp4B6;if(_tmp4C5->tag != 14)goto
_LL292;}_LL291: return 16;_LL292: {struct Cyc_Absyn_AnonEnumType_struct*_tmp4C6=(
struct Cyc_Absyn_AnonEnumType_struct*)_tmp4B6;if(_tmp4C6->tag != 15)goto _LL294;}
_LL293: return 17;_LL294: {struct Cyc_Absyn_RgnHandleType_struct*_tmp4C7=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp4B6;if(_tmp4C7->tag != 16)goto _LL296;}_LL295: return 18;_LL296: {struct Cyc_Absyn_TypedefType_struct*
_tmp4C8=(struct Cyc_Absyn_TypedefType_struct*)_tmp4B6;if(_tmp4C8->tag != 18)goto
_LL298;}_LL297: return 19;_LL298: {struct Cyc_Absyn_UniqueRgn_struct*_tmp4C9=(
struct Cyc_Absyn_UniqueRgn_struct*)_tmp4B6;if(_tmp4C9->tag != 22)goto _LL29A;}
_LL299: return 20;_LL29A: {struct Cyc_Absyn_HeapRgn_struct*_tmp4CA=(struct Cyc_Absyn_HeapRgn_struct*)
_tmp4B6;if(_tmp4CA->tag != 21)goto _LL29C;}_LL29B: return 21;_LL29C: {struct Cyc_Absyn_AccessEff_struct*
_tmp4CB=(struct Cyc_Absyn_AccessEff_struct*)_tmp4B6;if(_tmp4CB->tag != 23)goto
_LL29E;}_LL29D: return 22;_LL29E: {struct Cyc_Absyn_JoinEff_struct*_tmp4CC=(struct
Cyc_Absyn_JoinEff_struct*)_tmp4B6;if(_tmp4CC->tag != 24)goto _LL2A0;}_LL29F: return
23;_LL2A0: {struct Cyc_Absyn_RgnsEff_struct*_tmp4CD=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp4B6;if(_tmp4CD->tag != 25)goto _LL2A2;}_LL2A1: return 24;_LL2A2: {struct Cyc_Absyn_TagType_struct*
_tmp4CE=(struct Cyc_Absyn_TagType_struct*)_tmp4B6;if(_tmp4CE->tag != 20)goto _LL2A4;}
_LL2A3: return 26;_LL2A4: {struct Cyc_Absyn_DynRgnType_struct*_tmp4CF=(struct Cyc_Absyn_DynRgnType_struct*)
_tmp4B6;if(_tmp4CF->tag != 17)goto _LL2A6;}_LL2A5: return 27;_LL2A6: {struct Cyc_Absyn_ValueofType_struct*
_tmp4D0=(struct Cyc_Absyn_ValueofType_struct*)_tmp4B6;if(_tmp4D0->tag != 19)goto
_LL273;}_LL2A7: return 28;_LL273:;}int Cyc_Tcutil_typecmp(void*t1,void*t2);int Cyc_Tcutil_typecmp(
void*t1,void*t2){t1=Cyc_Tcutil_compress(t1);t2=Cyc_Tcutil_compress(t2);if(t1 == 
t2)return 0;{int _tmp4D1=Cyc_Core_intcmp(Cyc_Tcutil_type_case_number(t1),Cyc_Tcutil_type_case_number(
t2));if(_tmp4D1 != 0)return _tmp4D1;{struct _tuple0 _tmp1030;struct _tuple0 _tmp4D3=(
_tmp1030.f1=t2,((_tmp1030.f2=t1,_tmp1030)));void*_tmp4D4;void*_tmp4D6;void*
_tmp4D8;struct Cyc_Absyn_Tvar*_tmp4DA;void*_tmp4DB;struct Cyc_Absyn_Tvar*_tmp4DD;
void*_tmp4DE;struct Cyc_Absyn_AggrInfo _tmp4E0;union Cyc_Absyn_AggrInfoU _tmp4E1;
struct Cyc_List_List*_tmp4E2;void*_tmp4E3;struct Cyc_Absyn_AggrInfo _tmp4E5;union
Cyc_Absyn_AggrInfoU _tmp4E6;struct Cyc_List_List*_tmp4E7;void*_tmp4E8;struct
_tuple2*_tmp4EA;void*_tmp4EB;struct _tuple2*_tmp4ED;void*_tmp4EE;struct Cyc_List_List*
_tmp4F0;void*_tmp4F1;struct Cyc_List_List*_tmp4F3;void*_tmp4F4;struct Cyc_Absyn_DatatypeInfo
_tmp4F6;union Cyc_Absyn_DatatypeInfoU _tmp4F7;struct Cyc_Absyn_Datatypedecl**
_tmp4F8;struct Cyc_Absyn_Datatypedecl*_tmp4F9;struct Cyc_List_List*_tmp4FA;void*
_tmp4FB;struct Cyc_Absyn_DatatypeInfo _tmp4FD;union Cyc_Absyn_DatatypeInfoU _tmp4FE;
struct Cyc_Absyn_Datatypedecl**_tmp4FF;struct Cyc_Absyn_Datatypedecl*_tmp500;
struct Cyc_List_List*_tmp501;void*_tmp502;struct Cyc_Absyn_DatatypeFieldInfo
_tmp504;union Cyc_Absyn_DatatypeFieldInfoU _tmp505;struct _tuple3 _tmp506;struct Cyc_Absyn_Datatypedecl*
_tmp507;struct Cyc_Absyn_Datatypefield*_tmp508;struct Cyc_List_List*_tmp509;void*
_tmp50A;struct Cyc_Absyn_DatatypeFieldInfo _tmp50C;union Cyc_Absyn_DatatypeFieldInfoU
_tmp50D;struct _tuple3 _tmp50E;struct Cyc_Absyn_Datatypedecl*_tmp50F;struct Cyc_Absyn_Datatypefield*
_tmp510;struct Cyc_List_List*_tmp511;void*_tmp512;struct Cyc_Absyn_PtrInfo _tmp514;
void*_tmp515;struct Cyc_Absyn_Tqual _tmp516;struct Cyc_Absyn_PtrAtts _tmp517;void*
_tmp518;union Cyc_Absyn_Constraint*_tmp519;union Cyc_Absyn_Constraint*_tmp51A;
union Cyc_Absyn_Constraint*_tmp51B;void*_tmp51C;struct Cyc_Absyn_PtrInfo _tmp51E;
void*_tmp51F;struct Cyc_Absyn_Tqual _tmp520;struct Cyc_Absyn_PtrAtts _tmp521;void*
_tmp522;union Cyc_Absyn_Constraint*_tmp523;union Cyc_Absyn_Constraint*_tmp524;
union Cyc_Absyn_Constraint*_tmp525;void*_tmp526;enum Cyc_Absyn_Sign _tmp528;enum 
Cyc_Absyn_Size_of _tmp529;void*_tmp52A;enum Cyc_Absyn_Sign _tmp52C;enum Cyc_Absyn_Size_of
_tmp52D;void*_tmp52E;void*_tmp530;void*_tmp532;int _tmp534;void*_tmp535;int
_tmp537;void*_tmp538;struct Cyc_Absyn_ArrayInfo _tmp53A;void*_tmp53B;struct Cyc_Absyn_Tqual
_tmp53C;struct Cyc_Absyn_Exp*_tmp53D;union Cyc_Absyn_Constraint*_tmp53E;void*
_tmp53F;struct Cyc_Absyn_ArrayInfo _tmp541;void*_tmp542;struct Cyc_Absyn_Tqual
_tmp543;struct Cyc_Absyn_Exp*_tmp544;union Cyc_Absyn_Constraint*_tmp545;void*
_tmp546;struct Cyc_Absyn_FnInfo _tmp548;struct Cyc_List_List*_tmp549;struct Cyc_Core_Opt*
_tmp54A;void*_tmp54B;struct Cyc_List_List*_tmp54C;int _tmp54D;struct Cyc_Absyn_VarargInfo*
_tmp54E;struct Cyc_List_List*_tmp54F;struct Cyc_List_List*_tmp550;void*_tmp551;
struct Cyc_Absyn_FnInfo _tmp553;struct Cyc_List_List*_tmp554;struct Cyc_Core_Opt*
_tmp555;void*_tmp556;struct Cyc_List_List*_tmp557;int _tmp558;struct Cyc_Absyn_VarargInfo*
_tmp559;struct Cyc_List_List*_tmp55A;struct Cyc_List_List*_tmp55B;void*_tmp55C;
struct Cyc_List_List*_tmp55E;void*_tmp55F;struct Cyc_List_List*_tmp561;void*
_tmp562;enum Cyc_Absyn_AggrKind _tmp564;struct Cyc_List_List*_tmp565;void*_tmp566;
enum Cyc_Absyn_AggrKind _tmp568;struct Cyc_List_List*_tmp569;void*_tmp56A;void*
_tmp56C;void*_tmp56D;void*_tmp56F;void*_tmp570;void*_tmp572;void*_tmp573;void*
_tmp574;void*_tmp576;void*_tmp577;void*_tmp578;void*_tmp57A;void*_tmp57B;void*
_tmp57D;void*_tmp57E;struct Cyc_Absyn_Exp*_tmp580;void*_tmp581;struct Cyc_Absyn_Exp*
_tmp583;void*_tmp584;void*_tmp586;void*_tmp588;void*_tmp58A;void*_tmp58C;void*
_tmp58E;_LL2A9: _tmp4D4=_tmp4D3.f1;{struct Cyc_Absyn_Evar_struct*_tmp4D5=(struct
Cyc_Absyn_Evar_struct*)_tmp4D4;if(_tmp4D5->tag != 1)goto _LL2AB;}_tmp4D6=_tmp4D3.f2;{
struct Cyc_Absyn_Evar_struct*_tmp4D7=(struct Cyc_Absyn_Evar_struct*)_tmp4D6;if(
_tmp4D7->tag != 1)goto _LL2AB;}_LL2AA: {const char*_tmp1033;void*_tmp1032;(_tmp1032=
0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp1033="typecmp: can only compare closed types",_tag_dyneither(_tmp1033,
sizeof(char),39))),_tag_dyneither(_tmp1032,sizeof(void*),0)));}_LL2AB: _tmp4D8=
_tmp4D3.f1;{struct Cyc_Absyn_VarType_struct*_tmp4D9=(struct Cyc_Absyn_VarType_struct*)
_tmp4D8;if(_tmp4D9->tag != 2)goto _LL2AD;else{_tmp4DA=_tmp4D9->f1;}}_tmp4DB=
_tmp4D3.f2;{struct Cyc_Absyn_VarType_struct*_tmp4DC=(struct Cyc_Absyn_VarType_struct*)
_tmp4DB;if(_tmp4DC->tag != 2)goto _LL2AD;else{_tmp4DD=_tmp4DC->f1;}}_LL2AC: return
Cyc_Core_intcmp(_tmp4DD->identity,_tmp4DA->identity);_LL2AD: _tmp4DE=_tmp4D3.f1;{
struct Cyc_Absyn_AggrType_struct*_tmp4DF=(struct Cyc_Absyn_AggrType_struct*)
_tmp4DE;if(_tmp4DF->tag != 12)goto _LL2AF;else{_tmp4E0=_tmp4DF->f1;_tmp4E1=_tmp4E0.aggr_info;
_tmp4E2=_tmp4E0.targs;}}_tmp4E3=_tmp4D3.f2;{struct Cyc_Absyn_AggrType_struct*
_tmp4E4=(struct Cyc_Absyn_AggrType_struct*)_tmp4E3;if(_tmp4E4->tag != 12)goto
_LL2AF;else{_tmp4E5=_tmp4E4->f1;_tmp4E6=_tmp4E5.aggr_info;_tmp4E7=_tmp4E5.targs;}}
_LL2AE: {struct _tuple2*_tmp593;struct _tuple12 _tmp592=Cyc_Absyn_aggr_kinded_name(
_tmp4E1);_tmp593=_tmp592.f2;{struct _tuple2*_tmp595;struct _tuple12 _tmp594=Cyc_Absyn_aggr_kinded_name(
_tmp4E6);_tmp595=_tmp594.f2;{int _tmp596=Cyc_Absyn_qvar_cmp(_tmp593,_tmp595);if(
_tmp596 != 0)return _tmp596;else{return((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*
l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_typecmp,_tmp4E2,_tmp4E7);}}}}
_LL2AF: _tmp4E8=_tmp4D3.f1;{struct Cyc_Absyn_EnumType_struct*_tmp4E9=(struct Cyc_Absyn_EnumType_struct*)
_tmp4E8;if(_tmp4E9->tag != 14)goto _LL2B1;else{_tmp4EA=_tmp4E9->f1;}}_tmp4EB=
_tmp4D3.f2;{struct Cyc_Absyn_EnumType_struct*_tmp4EC=(struct Cyc_Absyn_EnumType_struct*)
_tmp4EB;if(_tmp4EC->tag != 14)goto _LL2B1;else{_tmp4ED=_tmp4EC->f1;}}_LL2B0: return
Cyc_Absyn_qvar_cmp(_tmp4EA,_tmp4ED);_LL2B1: _tmp4EE=_tmp4D3.f1;{struct Cyc_Absyn_AnonEnumType_struct*
_tmp4EF=(struct Cyc_Absyn_AnonEnumType_struct*)_tmp4EE;if(_tmp4EF->tag != 15)goto
_LL2B3;else{_tmp4F0=_tmp4EF->f1;}}_tmp4F1=_tmp4D3.f2;{struct Cyc_Absyn_AnonEnumType_struct*
_tmp4F2=(struct Cyc_Absyn_AnonEnumType_struct*)_tmp4F1;if(_tmp4F2->tag != 15)goto
_LL2B3;else{_tmp4F3=_tmp4F2->f1;}}_LL2B2: return((int(*)(int(*cmp)(struct Cyc_Absyn_Enumfield*,
struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(
Cyc_Tcutil_enumfield_cmp,_tmp4F0,_tmp4F3);_LL2B3: _tmp4F4=_tmp4D3.f1;{struct Cyc_Absyn_DatatypeType_struct*
_tmp4F5=(struct Cyc_Absyn_DatatypeType_struct*)_tmp4F4;if(_tmp4F5->tag != 3)goto
_LL2B5;else{_tmp4F6=_tmp4F5->f1;_tmp4F7=_tmp4F6.datatype_info;if((_tmp4F7.KnownDatatype).tag
!= 2)goto _LL2B5;_tmp4F8=(struct Cyc_Absyn_Datatypedecl**)(_tmp4F7.KnownDatatype).val;
_tmp4F9=*_tmp4F8;_tmp4FA=_tmp4F6.targs;}}_tmp4FB=_tmp4D3.f2;{struct Cyc_Absyn_DatatypeType_struct*
_tmp4FC=(struct Cyc_Absyn_DatatypeType_struct*)_tmp4FB;if(_tmp4FC->tag != 3)goto
_LL2B5;else{_tmp4FD=_tmp4FC->f1;_tmp4FE=_tmp4FD.datatype_info;if((_tmp4FE.KnownDatatype).tag
!= 2)goto _LL2B5;_tmp4FF=(struct Cyc_Absyn_Datatypedecl**)(_tmp4FE.KnownDatatype).val;
_tmp500=*_tmp4FF;_tmp501=_tmp4FD.targs;}}_LL2B4: if(_tmp500 == _tmp4F9)return 0;{
int _tmp597=Cyc_Absyn_qvar_cmp(_tmp500->name,_tmp4F9->name);if(_tmp597 != 0)return
_tmp597;return((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*
l2))Cyc_List_list_cmp)(Cyc_Tcutil_typecmp,_tmp501,_tmp4FA);}_LL2B5: _tmp502=
_tmp4D3.f1;{struct Cyc_Absyn_DatatypeFieldType_struct*_tmp503=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp502;if(_tmp503->tag != 4)goto _LL2B7;else{_tmp504=_tmp503->f1;_tmp505=_tmp504.field_info;
if((_tmp505.KnownDatatypefield).tag != 2)goto _LL2B7;_tmp506=(struct _tuple3)(
_tmp505.KnownDatatypefield).val;_tmp507=_tmp506.f1;_tmp508=_tmp506.f2;_tmp509=
_tmp504.targs;}}_tmp50A=_tmp4D3.f2;{struct Cyc_Absyn_DatatypeFieldType_struct*
_tmp50B=(struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp50A;if(_tmp50B->tag != 4)
goto _LL2B7;else{_tmp50C=_tmp50B->f1;_tmp50D=_tmp50C.field_info;if((_tmp50D.KnownDatatypefield).tag
!= 2)goto _LL2B7;_tmp50E=(struct _tuple3)(_tmp50D.KnownDatatypefield).val;_tmp50F=
_tmp50E.f1;_tmp510=_tmp50E.f2;_tmp511=_tmp50C.targs;}}_LL2B6: if(_tmp50F == 
_tmp507)return 0;{int _tmp598=Cyc_Absyn_qvar_cmp(_tmp507->name,_tmp50F->name);if(
_tmp598 != 0)return _tmp598;{int _tmp599=Cyc_Absyn_qvar_cmp(_tmp508->name,_tmp510->name);
if(_tmp599 != 0)return _tmp599;return((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*
l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_typecmp,_tmp511,_tmp509);}}
_LL2B7: _tmp512=_tmp4D3.f1;{struct Cyc_Absyn_PointerType_struct*_tmp513=(struct Cyc_Absyn_PointerType_struct*)
_tmp512;if(_tmp513->tag != 5)goto _LL2B9;else{_tmp514=_tmp513->f1;_tmp515=_tmp514.elt_typ;
_tmp516=_tmp514.elt_tq;_tmp517=_tmp514.ptr_atts;_tmp518=_tmp517.rgn;_tmp519=
_tmp517.nullable;_tmp51A=_tmp517.bounds;_tmp51B=_tmp517.zero_term;}}_tmp51C=
_tmp4D3.f2;{struct Cyc_Absyn_PointerType_struct*_tmp51D=(struct Cyc_Absyn_PointerType_struct*)
_tmp51C;if(_tmp51D->tag != 5)goto _LL2B9;else{_tmp51E=_tmp51D->f1;_tmp51F=_tmp51E.elt_typ;
_tmp520=_tmp51E.elt_tq;_tmp521=_tmp51E.ptr_atts;_tmp522=_tmp521.rgn;_tmp523=
_tmp521.nullable;_tmp524=_tmp521.bounds;_tmp525=_tmp521.zero_term;}}_LL2B8: {int
_tmp59A=Cyc_Tcutil_typecmp(_tmp51F,_tmp515);if(_tmp59A != 0)return _tmp59A;{int
_tmp59B=Cyc_Tcutil_typecmp(_tmp522,_tmp518);if(_tmp59B != 0)return _tmp59B;{int
_tmp59C=Cyc_Tcutil_tqual_cmp(_tmp520,_tmp516);if(_tmp59C != 0)return _tmp59C;{int
_tmp59D=((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*
y))Cyc_Tcutil_conrefs_cmp)(Cyc_Tcutil_boundscmp,_tmp524,_tmp51A);if(_tmp59D != 0)
return _tmp59D;{int _tmp59E=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*
x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_conrefs_cmp)(Cyc_Core_intcmp,_tmp525,
_tmp51B);if(_tmp59E != 0)return _tmp59E;{void*_tmp59F=((void*(*)(void*y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp524);_LL2DE: {struct Cyc_Absyn_DynEither_b_struct*
_tmp5A0=(struct Cyc_Absyn_DynEither_b_struct*)_tmp59F;if(_tmp5A0->tag != 0)goto
_LL2E0;}_LL2DF: return 0;_LL2E0:;_LL2E1: goto _LL2DD;_LL2DD:;}return((int(*)(int(*
cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_conrefs_cmp)(
Cyc_Core_intcmp,_tmp523,_tmp519);}}}}}_LL2B9: _tmp526=_tmp4D3.f1;{struct Cyc_Absyn_IntType_struct*
_tmp527=(struct Cyc_Absyn_IntType_struct*)_tmp526;if(_tmp527->tag != 6)goto _LL2BB;
else{_tmp528=_tmp527->f1;_tmp529=_tmp527->f2;}}_tmp52A=_tmp4D3.f2;{struct Cyc_Absyn_IntType_struct*
_tmp52B=(struct Cyc_Absyn_IntType_struct*)_tmp52A;if(_tmp52B->tag != 6)goto _LL2BB;
else{_tmp52C=_tmp52B->f1;_tmp52D=_tmp52B->f2;}}_LL2BA: if(_tmp52C != _tmp528)
return Cyc_Core_intcmp((int)((unsigned int)_tmp52C),(int)((unsigned int)_tmp528));
if(_tmp52D != _tmp529)return Cyc_Core_intcmp((int)((unsigned int)_tmp52D),(int)((
unsigned int)_tmp529));return 0;_LL2BB: _tmp52E=_tmp4D3.f1;{struct Cyc_Absyn_FloatType_struct*
_tmp52F=(struct Cyc_Absyn_FloatType_struct*)_tmp52E;if(_tmp52F->tag != 7)goto
_LL2BD;}_tmp530=_tmp4D3.f2;{struct Cyc_Absyn_FloatType_struct*_tmp531=(struct Cyc_Absyn_FloatType_struct*)
_tmp530;if(_tmp531->tag != 7)goto _LL2BD;}_LL2BC: return 0;_LL2BD: _tmp532=_tmp4D3.f1;{
struct Cyc_Absyn_DoubleType_struct*_tmp533=(struct Cyc_Absyn_DoubleType_struct*)
_tmp532;if(_tmp533->tag != 8)goto _LL2BF;else{_tmp534=_tmp533->f1;}}_tmp535=
_tmp4D3.f2;{struct Cyc_Absyn_DoubleType_struct*_tmp536=(struct Cyc_Absyn_DoubleType_struct*)
_tmp535;if(_tmp536->tag != 8)goto _LL2BF;else{_tmp537=_tmp536->f1;}}_LL2BE: if(
_tmp534 == _tmp537)return 0;else{if(_tmp534)return - 1;else{return 1;}}_LL2BF: _tmp538=
_tmp4D3.f1;{struct Cyc_Absyn_ArrayType_struct*_tmp539=(struct Cyc_Absyn_ArrayType_struct*)
_tmp538;if(_tmp539->tag != 9)goto _LL2C1;else{_tmp53A=_tmp539->f1;_tmp53B=_tmp53A.elt_type;
_tmp53C=_tmp53A.tq;_tmp53D=_tmp53A.num_elts;_tmp53E=_tmp53A.zero_term;}}_tmp53F=
_tmp4D3.f2;{struct Cyc_Absyn_ArrayType_struct*_tmp540=(struct Cyc_Absyn_ArrayType_struct*)
_tmp53F;if(_tmp540->tag != 9)goto _LL2C1;else{_tmp541=_tmp540->f1;_tmp542=_tmp541.elt_type;
_tmp543=_tmp541.tq;_tmp544=_tmp541.num_elts;_tmp545=_tmp541.zero_term;}}_LL2C0: {
int _tmp5A1=Cyc_Tcutil_tqual_cmp(_tmp543,_tmp53C);if(_tmp5A1 != 0)return _tmp5A1;{
int _tmp5A2=Cyc_Tcutil_typecmp(_tmp542,_tmp53B);if(_tmp5A2 != 0)return _tmp5A2;{int
_tmp5A3=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*
y))Cyc_Tcutil_conrefs_cmp)(Cyc_Core_intcmp,_tmp53E,_tmp545);if(_tmp5A3 != 0)
return _tmp5A3;if(_tmp53D == _tmp544)return 0;if(_tmp53D == 0  || _tmp544 == 0){const
char*_tmp1036;void*_tmp1035;(_tmp1035=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1036="missing expression in array index",
_tag_dyneither(_tmp1036,sizeof(char),34))),_tag_dyneither(_tmp1035,sizeof(void*),
0)));}return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct
Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,
_tmp53D,_tmp544);}}}_LL2C1: _tmp546=_tmp4D3.f1;{struct Cyc_Absyn_FnType_struct*
_tmp547=(struct Cyc_Absyn_FnType_struct*)_tmp546;if(_tmp547->tag != 10)goto _LL2C3;
else{_tmp548=_tmp547->f1;_tmp549=_tmp548.tvars;_tmp54A=_tmp548.effect;_tmp54B=
_tmp548.ret_typ;_tmp54C=_tmp548.args;_tmp54D=_tmp548.c_varargs;_tmp54E=_tmp548.cyc_varargs;
_tmp54F=_tmp548.rgn_po;_tmp550=_tmp548.attributes;}}_tmp551=_tmp4D3.f2;{struct
Cyc_Absyn_FnType_struct*_tmp552=(struct Cyc_Absyn_FnType_struct*)_tmp551;if(
_tmp552->tag != 10)goto _LL2C3;else{_tmp553=_tmp552->f1;_tmp554=_tmp553.tvars;
_tmp555=_tmp553.effect;_tmp556=_tmp553.ret_typ;_tmp557=_tmp553.args;_tmp558=
_tmp553.c_varargs;_tmp559=_tmp553.cyc_varargs;_tmp55A=_tmp553.rgn_po;_tmp55B=
_tmp553.attributes;}}_LL2C2: {const char*_tmp1039;void*_tmp1038;(_tmp1038=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1039="typecmp: function types not handled",
_tag_dyneither(_tmp1039,sizeof(char),36))),_tag_dyneither(_tmp1038,sizeof(void*),
0)));}_LL2C3: _tmp55C=_tmp4D3.f1;{struct Cyc_Absyn_TupleType_struct*_tmp55D=(
struct Cyc_Absyn_TupleType_struct*)_tmp55C;if(_tmp55D->tag != 11)goto _LL2C5;else{
_tmp55E=_tmp55D->f1;}}_tmp55F=_tmp4D3.f2;{struct Cyc_Absyn_TupleType_struct*
_tmp560=(struct Cyc_Absyn_TupleType_struct*)_tmp55F;if(_tmp560->tag != 11)goto
_LL2C5;else{_tmp561=_tmp560->f1;}}_LL2C4: return((int(*)(int(*cmp)(struct _tuple11*,
struct _tuple11*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(
Cyc_Tcutil_tqual_type_cmp,_tmp561,_tmp55E);_LL2C5: _tmp562=_tmp4D3.f1;{struct Cyc_Absyn_AnonAggrType_struct*
_tmp563=(struct Cyc_Absyn_AnonAggrType_struct*)_tmp562;if(_tmp563->tag != 13)goto
_LL2C7;else{_tmp564=_tmp563->f1;_tmp565=_tmp563->f2;}}_tmp566=_tmp4D3.f2;{struct
Cyc_Absyn_AnonAggrType_struct*_tmp567=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp566;if(_tmp567->tag != 13)goto _LL2C7;else{_tmp568=_tmp567->f1;_tmp569=_tmp567->f2;}}
_LL2C6: if(_tmp568 != _tmp564){if(_tmp568 == Cyc_Absyn_StructA)return - 1;else{return
1;}}return((int(*)(int(*cmp)(struct Cyc_Absyn_Aggrfield*,struct Cyc_Absyn_Aggrfield*),
struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_aggrfield_cmp,
_tmp569,_tmp565);_LL2C7: _tmp56A=_tmp4D3.f1;{struct Cyc_Absyn_RgnHandleType_struct*
_tmp56B=(struct Cyc_Absyn_RgnHandleType_struct*)_tmp56A;if(_tmp56B->tag != 16)goto
_LL2C9;else{_tmp56C=(void*)_tmp56B->f1;}}_tmp56D=_tmp4D3.f2;{struct Cyc_Absyn_RgnHandleType_struct*
_tmp56E=(struct Cyc_Absyn_RgnHandleType_struct*)_tmp56D;if(_tmp56E->tag != 16)goto
_LL2C9;else{_tmp56F=(void*)_tmp56E->f1;}}_LL2C8: return Cyc_Tcutil_typecmp(_tmp56C,
_tmp56F);_LL2C9: _tmp570=_tmp4D3.f1;{struct Cyc_Absyn_DynRgnType_struct*_tmp571=(
struct Cyc_Absyn_DynRgnType_struct*)_tmp570;if(_tmp571->tag != 17)goto _LL2CB;else{
_tmp572=(void*)_tmp571->f1;_tmp573=(void*)_tmp571->f2;}}_tmp574=_tmp4D3.f2;{
struct Cyc_Absyn_DynRgnType_struct*_tmp575=(struct Cyc_Absyn_DynRgnType_struct*)
_tmp574;if(_tmp575->tag != 17)goto _LL2CB;else{_tmp576=(void*)_tmp575->f1;_tmp577=(
void*)_tmp575->f2;}}_LL2CA: {int _tmp5A8=Cyc_Tcutil_typecmp(_tmp572,_tmp576);if(
_tmp5A8 != 0)return _tmp5A8;else{return Cyc_Tcutil_typecmp(_tmp573,_tmp577);}}
_LL2CB: _tmp578=_tmp4D3.f1;{struct Cyc_Absyn_TagType_struct*_tmp579=(struct Cyc_Absyn_TagType_struct*)
_tmp578;if(_tmp579->tag != 20)goto _LL2CD;else{_tmp57A=(void*)_tmp579->f1;}}
_tmp57B=_tmp4D3.f2;{struct Cyc_Absyn_TagType_struct*_tmp57C=(struct Cyc_Absyn_TagType_struct*)
_tmp57B;if(_tmp57C->tag != 20)goto _LL2CD;else{_tmp57D=(void*)_tmp57C->f1;}}_LL2CC:
return Cyc_Tcutil_typecmp(_tmp57A,_tmp57D);_LL2CD: _tmp57E=_tmp4D3.f1;{struct Cyc_Absyn_ValueofType_struct*
_tmp57F=(struct Cyc_Absyn_ValueofType_struct*)_tmp57E;if(_tmp57F->tag != 19)goto
_LL2CF;else{_tmp580=_tmp57F->f1;}}_tmp581=_tmp4D3.f2;{struct Cyc_Absyn_ValueofType_struct*
_tmp582=(struct Cyc_Absyn_ValueofType_struct*)_tmp581;if(_tmp582->tag != 19)goto
_LL2CF;else{_tmp583=_tmp582->f1;}}_LL2CE: return Cyc_Evexp_const_exp_cmp(_tmp580,
_tmp583);_LL2CF: _tmp584=_tmp4D3.f1;{struct Cyc_Absyn_JoinEff_struct*_tmp585=(
struct Cyc_Absyn_JoinEff_struct*)_tmp584;if(_tmp585->tag != 24)goto _LL2D1;}_LL2D0:
goto _LL2D2;_LL2D1: _tmp586=_tmp4D3.f2;{struct Cyc_Absyn_JoinEff_struct*_tmp587=(
struct Cyc_Absyn_JoinEff_struct*)_tmp586;if(_tmp587->tag != 24)goto _LL2D3;}_LL2D2:
goto _LL2D4;_LL2D3: _tmp588=_tmp4D3.f1;{struct Cyc_Absyn_AccessEff_struct*_tmp589=(
struct Cyc_Absyn_AccessEff_struct*)_tmp588;if(_tmp589->tag != 23)goto _LL2D5;}
_LL2D4: goto _LL2D6;_LL2D5: _tmp58A=_tmp4D3.f1;{struct Cyc_Absyn_RgnsEff_struct*
_tmp58B=(struct Cyc_Absyn_RgnsEff_struct*)_tmp58A;if(_tmp58B->tag != 25)goto _LL2D7;}
_LL2D6: goto _LL2D8;_LL2D7: _tmp58C=_tmp4D3.f2;{struct Cyc_Absyn_RgnsEff_struct*
_tmp58D=(struct Cyc_Absyn_RgnsEff_struct*)_tmp58C;if(_tmp58D->tag != 25)goto _LL2D9;}
_LL2D8: goto _LL2DA;_LL2D9: _tmp58E=_tmp4D3.f2;{struct Cyc_Absyn_AccessEff_struct*
_tmp58F=(struct Cyc_Absyn_AccessEff_struct*)_tmp58E;if(_tmp58F->tag != 23)goto
_LL2DB;}_LL2DA: {const char*_tmp103C;void*_tmp103B;(_tmp103B=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp103C="typecmp: effects not handled",
_tag_dyneither(_tmp103C,sizeof(char),29))),_tag_dyneither(_tmp103B,sizeof(void*),
0)));}_LL2DB:;_LL2DC: {const char*_tmp103F;void*_tmp103E;(_tmp103E=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp103F="Unmatched case in typecmp",
_tag_dyneither(_tmp103F,sizeof(char),26))),_tag_dyneither(_tmp103E,sizeof(void*),
0)));}_LL2A8:;}}}int Cyc_Tcutil_is_arithmetic_type(void*t);int Cyc_Tcutil_is_arithmetic_type(
void*t){void*_tmp5AD=Cyc_Tcutil_compress(t);_LL2E3: {struct Cyc_Absyn_IntType_struct*
_tmp5AE=(struct Cyc_Absyn_IntType_struct*)_tmp5AD;if(_tmp5AE->tag != 6)goto _LL2E5;}
_LL2E4: goto _LL2E6;_LL2E5: {struct Cyc_Absyn_FloatType_struct*_tmp5AF=(struct Cyc_Absyn_FloatType_struct*)
_tmp5AD;if(_tmp5AF->tag != 7)goto _LL2E7;}_LL2E6: goto _LL2E8;_LL2E7: {struct Cyc_Absyn_DoubleType_struct*
_tmp5B0=(struct Cyc_Absyn_DoubleType_struct*)_tmp5AD;if(_tmp5B0->tag != 8)goto
_LL2E9;}_LL2E8: goto _LL2EA;_LL2E9: {struct Cyc_Absyn_EnumType_struct*_tmp5B1=(
struct Cyc_Absyn_EnumType_struct*)_tmp5AD;if(_tmp5B1->tag != 14)goto _LL2EB;}_LL2EA:
goto _LL2EC;_LL2EB: {struct Cyc_Absyn_AnonEnumType_struct*_tmp5B2=(struct Cyc_Absyn_AnonEnumType_struct*)
_tmp5AD;if(_tmp5B2->tag != 15)goto _LL2ED;}_LL2EC: return 1;_LL2ED:;_LL2EE: return 0;
_LL2E2:;}int Cyc_Tcutil_will_lose_precision(void*t1,void*t2);int Cyc_Tcutil_will_lose_precision(
void*t1,void*t2){t1=Cyc_Tcutil_compress(t1);t2=Cyc_Tcutil_compress(t2);{struct
_tuple0 _tmp1040;struct _tuple0 _tmp5B4=(_tmp1040.f1=t1,((_tmp1040.f2=t2,_tmp1040)));
void*_tmp5B5;int _tmp5B7;void*_tmp5B8;int _tmp5BA;void*_tmp5BB;void*_tmp5BD;void*
_tmp5BF;void*_tmp5C1;void*_tmp5C3;void*_tmp5C5;void*_tmp5C7;void*_tmp5C9;void*
_tmp5CB;void*_tmp5CD;void*_tmp5CF;enum Cyc_Absyn_Size_of _tmp5D1;void*_tmp5D2;
enum Cyc_Absyn_Size_of _tmp5D4;void*_tmp5D5;enum Cyc_Absyn_Size_of _tmp5D7;void*
_tmp5D8;enum Cyc_Absyn_Size_of _tmp5DA;void*_tmp5DB;enum Cyc_Absyn_Size_of _tmp5DD;
void*_tmp5DE;enum Cyc_Absyn_Size_of _tmp5E0;void*_tmp5E1;enum Cyc_Absyn_Size_of
_tmp5E3;void*_tmp5E4;enum Cyc_Absyn_Size_of _tmp5E6;void*_tmp5E7;void*_tmp5E9;
enum Cyc_Absyn_Size_of _tmp5EB;void*_tmp5EC;void*_tmp5EE;enum Cyc_Absyn_Size_of
_tmp5F0;void*_tmp5F1;enum Cyc_Absyn_Size_of _tmp5F3;void*_tmp5F4;enum Cyc_Absyn_Size_of
_tmp5F6;void*_tmp5F7;enum Cyc_Absyn_Size_of _tmp5F9;void*_tmp5FA;void*_tmp5FC;
enum Cyc_Absyn_Size_of _tmp5FE;void*_tmp5FF;enum Cyc_Absyn_Size_of _tmp601;void*
_tmp602;enum Cyc_Absyn_Size_of _tmp604;void*_tmp605;enum Cyc_Absyn_Size_of _tmp607;
void*_tmp608;enum Cyc_Absyn_Size_of _tmp60A;void*_tmp60B;enum Cyc_Absyn_Size_of
_tmp60D;void*_tmp60E;enum Cyc_Absyn_Size_of _tmp610;void*_tmp611;void*_tmp613;
enum Cyc_Absyn_Size_of _tmp615;_LL2F0: _tmp5B5=_tmp5B4.f1;{struct Cyc_Absyn_DoubleType_struct*
_tmp5B6=(struct Cyc_Absyn_DoubleType_struct*)_tmp5B5;if(_tmp5B6->tag != 8)goto
_LL2F2;else{_tmp5B7=_tmp5B6->f1;}}_tmp5B8=_tmp5B4.f2;{struct Cyc_Absyn_DoubleType_struct*
_tmp5B9=(struct Cyc_Absyn_DoubleType_struct*)_tmp5B8;if(_tmp5B9->tag != 8)goto
_LL2F2;else{_tmp5BA=_tmp5B9->f1;}}_LL2F1: return !_tmp5BA  && _tmp5B7;_LL2F2:
_tmp5BB=_tmp5B4.f1;{struct Cyc_Absyn_DoubleType_struct*_tmp5BC=(struct Cyc_Absyn_DoubleType_struct*)
_tmp5BB;if(_tmp5BC->tag != 8)goto _LL2F4;}_tmp5BD=_tmp5B4.f2;{struct Cyc_Absyn_FloatType_struct*
_tmp5BE=(struct Cyc_Absyn_FloatType_struct*)_tmp5BD;if(_tmp5BE->tag != 7)goto
_LL2F4;}_LL2F3: goto _LL2F5;_LL2F4: _tmp5BF=_tmp5B4.f1;{struct Cyc_Absyn_DoubleType_struct*
_tmp5C0=(struct Cyc_Absyn_DoubleType_struct*)_tmp5BF;if(_tmp5C0->tag != 8)goto
_LL2F6;}_tmp5C1=_tmp5B4.f2;{struct Cyc_Absyn_IntType_struct*_tmp5C2=(struct Cyc_Absyn_IntType_struct*)
_tmp5C1;if(_tmp5C2->tag != 6)goto _LL2F6;}_LL2F5: goto _LL2F7;_LL2F6: _tmp5C3=_tmp5B4.f1;{
struct Cyc_Absyn_DoubleType_struct*_tmp5C4=(struct Cyc_Absyn_DoubleType_struct*)
_tmp5C3;if(_tmp5C4->tag != 8)goto _LL2F8;}_tmp5C5=_tmp5B4.f2;{struct Cyc_Absyn_TagType_struct*
_tmp5C6=(struct Cyc_Absyn_TagType_struct*)_tmp5C5;if(_tmp5C6->tag != 20)goto _LL2F8;}
_LL2F7: goto _LL2F9;_LL2F8: _tmp5C7=_tmp5B4.f1;{struct Cyc_Absyn_FloatType_struct*
_tmp5C8=(struct Cyc_Absyn_FloatType_struct*)_tmp5C7;if(_tmp5C8->tag != 7)goto
_LL2FA;}_tmp5C9=_tmp5B4.f2;{struct Cyc_Absyn_TagType_struct*_tmp5CA=(struct Cyc_Absyn_TagType_struct*)
_tmp5C9;if(_tmp5CA->tag != 20)goto _LL2FA;}_LL2F9: goto _LL2FB;_LL2FA: _tmp5CB=
_tmp5B4.f1;{struct Cyc_Absyn_FloatType_struct*_tmp5CC=(struct Cyc_Absyn_FloatType_struct*)
_tmp5CB;if(_tmp5CC->tag != 7)goto _LL2FC;}_tmp5CD=_tmp5B4.f2;{struct Cyc_Absyn_IntType_struct*
_tmp5CE=(struct Cyc_Absyn_IntType_struct*)_tmp5CD;if(_tmp5CE->tag != 6)goto _LL2FC;}
_LL2FB: return 1;_LL2FC: _tmp5CF=_tmp5B4.f1;{struct Cyc_Absyn_IntType_struct*_tmp5D0=(
struct Cyc_Absyn_IntType_struct*)_tmp5CF;if(_tmp5D0->tag != 6)goto _LL2FE;else{
_tmp5D1=_tmp5D0->f2;if(_tmp5D1 != Cyc_Absyn_LongLong_sz)goto _LL2FE;}}_tmp5D2=
_tmp5B4.f2;{struct Cyc_Absyn_IntType_struct*_tmp5D3=(struct Cyc_Absyn_IntType_struct*)
_tmp5D2;if(_tmp5D3->tag != 6)goto _LL2FE;else{_tmp5D4=_tmp5D3->f2;if(_tmp5D4 != Cyc_Absyn_LongLong_sz)
goto _LL2FE;}}_LL2FD: return 0;_LL2FE: _tmp5D5=_tmp5B4.f1;{struct Cyc_Absyn_IntType_struct*
_tmp5D6=(struct Cyc_Absyn_IntType_struct*)_tmp5D5;if(_tmp5D6->tag != 6)goto _LL300;
else{_tmp5D7=_tmp5D6->f2;if(_tmp5D7 != Cyc_Absyn_LongLong_sz)goto _LL300;}}_LL2FF:
return 1;_LL300: _tmp5D8=_tmp5B4.f1;{struct Cyc_Absyn_IntType_struct*_tmp5D9=(
struct Cyc_Absyn_IntType_struct*)_tmp5D8;if(_tmp5D9->tag != 6)goto _LL302;else{
_tmp5DA=_tmp5D9->f2;if(_tmp5DA != Cyc_Absyn_Long_sz)goto _LL302;}}_tmp5DB=_tmp5B4.f2;{
struct Cyc_Absyn_IntType_struct*_tmp5DC=(struct Cyc_Absyn_IntType_struct*)_tmp5DB;
if(_tmp5DC->tag != 6)goto _LL302;else{_tmp5DD=_tmp5DC->f2;if(_tmp5DD != Cyc_Absyn_Int_sz)
goto _LL302;}}_LL301: goto _LL303;_LL302: _tmp5DE=_tmp5B4.f1;{struct Cyc_Absyn_IntType_struct*
_tmp5DF=(struct Cyc_Absyn_IntType_struct*)_tmp5DE;if(_tmp5DF->tag != 6)goto _LL304;
else{_tmp5E0=_tmp5DF->f2;if(_tmp5E0 != Cyc_Absyn_Int_sz)goto _LL304;}}_tmp5E1=
_tmp5B4.f2;{struct Cyc_Absyn_IntType_struct*_tmp5E2=(struct Cyc_Absyn_IntType_struct*)
_tmp5E1;if(_tmp5E2->tag != 6)goto _LL304;else{_tmp5E3=_tmp5E2->f2;if(_tmp5E3 != Cyc_Absyn_Long_sz)
goto _LL304;}}_LL303: return 0;_LL304: _tmp5E4=_tmp5B4.f1;{struct Cyc_Absyn_IntType_struct*
_tmp5E5=(struct Cyc_Absyn_IntType_struct*)_tmp5E4;if(_tmp5E5->tag != 6)goto _LL306;
else{_tmp5E6=_tmp5E5->f2;if(_tmp5E6 != Cyc_Absyn_Long_sz)goto _LL306;}}_tmp5E7=
_tmp5B4.f2;{struct Cyc_Absyn_FloatType_struct*_tmp5E8=(struct Cyc_Absyn_FloatType_struct*)
_tmp5E7;if(_tmp5E8->tag != 7)goto _LL306;}_LL305: goto _LL307;_LL306: _tmp5E9=_tmp5B4.f1;{
struct Cyc_Absyn_IntType_struct*_tmp5EA=(struct Cyc_Absyn_IntType_struct*)_tmp5E9;
if(_tmp5EA->tag != 6)goto _LL308;else{_tmp5EB=_tmp5EA->f2;if(_tmp5EB != Cyc_Absyn_Int_sz)
goto _LL308;}}_tmp5EC=_tmp5B4.f2;{struct Cyc_Absyn_FloatType_struct*_tmp5ED=(
struct Cyc_Absyn_FloatType_struct*)_tmp5EC;if(_tmp5ED->tag != 7)goto _LL308;}_LL307:
goto _LL309;_LL308: _tmp5EE=_tmp5B4.f1;{struct Cyc_Absyn_IntType_struct*_tmp5EF=(
struct Cyc_Absyn_IntType_struct*)_tmp5EE;if(_tmp5EF->tag != 6)goto _LL30A;else{
_tmp5F0=_tmp5EF->f2;if(_tmp5F0 != Cyc_Absyn_Long_sz)goto _LL30A;}}_tmp5F1=_tmp5B4.f2;{
struct Cyc_Absyn_IntType_struct*_tmp5F2=(struct Cyc_Absyn_IntType_struct*)_tmp5F1;
if(_tmp5F2->tag != 6)goto _LL30A;else{_tmp5F3=_tmp5F2->f2;if(_tmp5F3 != Cyc_Absyn_Short_sz)
goto _LL30A;}}_LL309: goto _LL30B;_LL30A: _tmp5F4=_tmp5B4.f1;{struct Cyc_Absyn_IntType_struct*
_tmp5F5=(struct Cyc_Absyn_IntType_struct*)_tmp5F4;if(_tmp5F5->tag != 6)goto _LL30C;
else{_tmp5F6=_tmp5F5->f2;if(_tmp5F6 != Cyc_Absyn_Int_sz)goto _LL30C;}}_tmp5F7=
_tmp5B4.f2;{struct Cyc_Absyn_IntType_struct*_tmp5F8=(struct Cyc_Absyn_IntType_struct*)
_tmp5F7;if(_tmp5F8->tag != 6)goto _LL30C;else{_tmp5F9=_tmp5F8->f2;if(_tmp5F9 != Cyc_Absyn_Short_sz)
goto _LL30C;}}_LL30B: goto _LL30D;_LL30C: _tmp5FA=_tmp5B4.f1;{struct Cyc_Absyn_TagType_struct*
_tmp5FB=(struct Cyc_Absyn_TagType_struct*)_tmp5FA;if(_tmp5FB->tag != 20)goto _LL30E;}
_tmp5FC=_tmp5B4.f2;{struct Cyc_Absyn_IntType_struct*_tmp5FD=(struct Cyc_Absyn_IntType_struct*)
_tmp5FC;if(_tmp5FD->tag != 6)goto _LL30E;else{_tmp5FE=_tmp5FD->f2;if(_tmp5FE != Cyc_Absyn_Short_sz)
goto _LL30E;}}_LL30D: goto _LL30F;_LL30E: _tmp5FF=_tmp5B4.f1;{struct Cyc_Absyn_IntType_struct*
_tmp600=(struct Cyc_Absyn_IntType_struct*)_tmp5FF;if(_tmp600->tag != 6)goto _LL310;
else{_tmp601=_tmp600->f2;if(_tmp601 != Cyc_Absyn_Long_sz)goto _LL310;}}_tmp602=
_tmp5B4.f2;{struct Cyc_Absyn_IntType_struct*_tmp603=(struct Cyc_Absyn_IntType_struct*)
_tmp602;if(_tmp603->tag != 6)goto _LL310;else{_tmp604=_tmp603->f2;if(_tmp604 != Cyc_Absyn_Char_sz)
goto _LL310;}}_LL30F: goto _LL311;_LL310: _tmp605=_tmp5B4.f1;{struct Cyc_Absyn_IntType_struct*
_tmp606=(struct Cyc_Absyn_IntType_struct*)_tmp605;if(_tmp606->tag != 6)goto _LL312;
else{_tmp607=_tmp606->f2;if(_tmp607 != Cyc_Absyn_Int_sz)goto _LL312;}}_tmp608=
_tmp5B4.f2;{struct Cyc_Absyn_IntType_struct*_tmp609=(struct Cyc_Absyn_IntType_struct*)
_tmp608;if(_tmp609->tag != 6)goto _LL312;else{_tmp60A=_tmp609->f2;if(_tmp60A != Cyc_Absyn_Char_sz)
goto _LL312;}}_LL311: goto _LL313;_LL312: _tmp60B=_tmp5B4.f1;{struct Cyc_Absyn_IntType_struct*
_tmp60C=(struct Cyc_Absyn_IntType_struct*)_tmp60B;if(_tmp60C->tag != 6)goto _LL314;
else{_tmp60D=_tmp60C->f2;if(_tmp60D != Cyc_Absyn_Short_sz)goto _LL314;}}_tmp60E=
_tmp5B4.f2;{struct Cyc_Absyn_IntType_struct*_tmp60F=(struct Cyc_Absyn_IntType_struct*)
_tmp60E;if(_tmp60F->tag != 6)goto _LL314;else{_tmp610=_tmp60F->f2;if(_tmp610 != Cyc_Absyn_Char_sz)
goto _LL314;}}_LL313: goto _LL315;_LL314: _tmp611=_tmp5B4.f1;{struct Cyc_Absyn_TagType_struct*
_tmp612=(struct Cyc_Absyn_TagType_struct*)_tmp611;if(_tmp612->tag != 20)goto _LL316;}
_tmp613=_tmp5B4.f2;{struct Cyc_Absyn_IntType_struct*_tmp614=(struct Cyc_Absyn_IntType_struct*)
_tmp613;if(_tmp614->tag != 6)goto _LL316;else{_tmp615=_tmp614->f2;if(_tmp615 != Cyc_Absyn_Char_sz)
goto _LL316;}}_LL315: return 1;_LL316:;_LL317: return 0;_LL2EF:;}}int Cyc_Tcutil_coerce_list(
struct Cyc_Tcenv_Tenv*te,void*t,struct Cyc_List_List*es);int Cyc_Tcutil_coerce_list(
struct Cyc_Tcenv_Tenv*te,void*t,struct Cyc_List_List*es){struct _RegionHandle*
_tmp616=Cyc_Tcenv_get_fnrgn(te);{struct Cyc_Core_Opt*max_arith_type=0;{struct Cyc_List_List*
el=es;for(0;el != 0;el=el->tl){void*t1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt))->v);if(Cyc_Tcutil_is_arithmetic_type(
t1)){if(max_arith_type == 0  || Cyc_Tcutil_will_lose_precision(t1,(void*)
max_arith_type->v)){struct Cyc_Core_Opt*_tmp1041;max_arith_type=((_tmp1041=
_region_malloc(_tmp616,sizeof(*_tmp1041)),((_tmp1041->v=t1,_tmp1041))));}}}}if(
max_arith_type != 0){if(!Cyc_Tcutil_unify(t,(void*)max_arith_type->v))return 0;}}{
struct Cyc_List_List*el=es;for(0;el != 0;el=el->tl){if(!Cyc_Tcutil_coerce_assign(
te,(struct Cyc_Absyn_Exp*)el->hd,t)){{const char*_tmp1046;void*_tmp1045[2];struct
Cyc_String_pa_struct _tmp1044;struct Cyc_String_pa_struct _tmp1043;(_tmp1043.tag=0,((
_tmp1043.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt))->v)),((
_tmp1044.tag=0,((_tmp1044.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t)),((_tmp1045[0]=& _tmp1044,((_tmp1045[1]=& _tmp1043,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)
el->hd)->loc,((_tmp1046="type mismatch: expecting %s but found %s",
_tag_dyneither(_tmp1046,sizeof(char),41))),_tag_dyneither(_tmp1045,sizeof(void*),
2)))))))))))));}return 0;}}}return 1;}int Cyc_Tcutil_coerce_to_bool(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e);int Cyc_Tcutil_coerce_to_bool(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Absyn_Exp*e){if(!Cyc_Tcutil_coerce_sint_typ(te,e)){void*_tmp61C=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);_LL319: {struct Cyc_Absyn_PointerType_struct*
_tmp61D=(struct Cyc_Absyn_PointerType_struct*)_tmp61C;if(_tmp61D->tag != 5)goto
_LL31B;}_LL31A: Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_uint_typ,Cyc_Absyn_Other_coercion);
goto _LL318;_LL31B:;_LL31C: return 0;_LL318:;}return 1;}int Cyc_Tcutil_is_integral_type(
void*t);int Cyc_Tcutil_is_integral_type(void*t){void*_tmp61E=Cyc_Tcutil_compress(
t);_LL31E: {struct Cyc_Absyn_IntType_struct*_tmp61F=(struct Cyc_Absyn_IntType_struct*)
_tmp61E;if(_tmp61F->tag != 6)goto _LL320;}_LL31F: goto _LL321;_LL320: {struct Cyc_Absyn_TagType_struct*
_tmp620=(struct Cyc_Absyn_TagType_struct*)_tmp61E;if(_tmp620->tag != 20)goto _LL322;}
_LL321: goto _LL323;_LL322: {struct Cyc_Absyn_EnumType_struct*_tmp621=(struct Cyc_Absyn_EnumType_struct*)
_tmp61E;if(_tmp621->tag != 14)goto _LL324;}_LL323: goto _LL325;_LL324: {struct Cyc_Absyn_AnonEnumType_struct*
_tmp622=(struct Cyc_Absyn_AnonEnumType_struct*)_tmp61E;if(_tmp622->tag != 15)goto
_LL326;}_LL325: return 1;_LL326:;_LL327: return 0;_LL31D:;}int Cyc_Tcutil_coerce_uint_typ(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e);int Cyc_Tcutil_coerce_uint_typ(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){if(Cyc_Tcutil_unify((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_Absyn_uint_typ))return 1;if(Cyc_Tcutil_is_integral_type((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)){if(Cyc_Tcutil_will_lose_precision((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_Absyn_uint_typ)){const
char*_tmp1049;void*_tmp1048;(_tmp1048=0,Cyc_Tcutil_warn(e->loc,((_tmp1049="integral size mismatch; conversion supplied",
_tag_dyneither(_tmp1049,sizeof(char),44))),_tag_dyneither(_tmp1048,sizeof(void*),
0)));}Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_uint_typ,Cyc_Absyn_No_coercion);
return 1;}return 0;}int Cyc_Tcutil_coerce_sint_typ(struct Cyc_Tcenv_Tenv*te,struct
Cyc_Absyn_Exp*e);int Cyc_Tcutil_coerce_sint_typ(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*
e){if(Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_Absyn_sint_typ))
return 1;if(Cyc_Tcutil_is_integral_type((void*)((struct Cyc_Core_Opt*)_check_null(
e->topt))->v)){if(Cyc_Tcutil_will_lose_precision((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,Cyc_Absyn_sint_typ)){const char*_tmp104C;void*_tmp104B;(
_tmp104B=0,Cyc_Tcutil_warn(e->loc,((_tmp104C="integral size mismatch; conversion supplied",
_tag_dyneither(_tmp104C,sizeof(char),44))),_tag_dyneither(_tmp104B,sizeof(void*),
0)));}Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_sint_typ,Cyc_Absyn_No_coercion);
return 1;}return 0;}static int Cyc_Tcutil_ptrsubtype(struct Cyc_Tcenv_Tenv*te,struct
Cyc_List_List*assume,void*t1,void*t2);int Cyc_Tcutil_silent_castable(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void*t1,void*t2);int Cyc_Tcutil_silent_castable(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void*t1,void*t2){t1=Cyc_Tcutil_compress(
t1);t2=Cyc_Tcutil_compress(t2);{struct _tuple0 _tmp104D;struct _tuple0 _tmp628=(
_tmp104D.f1=t1,((_tmp104D.f2=t2,_tmp104D)));void*_tmp629;struct Cyc_Absyn_PtrInfo
_tmp62B;void*_tmp62C;struct Cyc_Absyn_PtrInfo _tmp62E;void*_tmp62F;struct Cyc_Absyn_ArrayInfo
_tmp631;void*_tmp632;struct Cyc_Absyn_Tqual _tmp633;struct Cyc_Absyn_Exp*_tmp634;
union Cyc_Absyn_Constraint*_tmp635;void*_tmp636;struct Cyc_Absyn_ArrayInfo _tmp638;
void*_tmp639;struct Cyc_Absyn_Tqual _tmp63A;struct Cyc_Absyn_Exp*_tmp63B;union Cyc_Absyn_Constraint*
_tmp63C;void*_tmp63D;void*_tmp63F;_LL329: _tmp629=_tmp628.f1;{struct Cyc_Absyn_PointerType_struct*
_tmp62A=(struct Cyc_Absyn_PointerType_struct*)_tmp629;if(_tmp62A->tag != 5)goto
_LL32B;else{_tmp62B=_tmp62A->f1;}}_tmp62C=_tmp628.f2;{struct Cyc_Absyn_PointerType_struct*
_tmp62D=(struct Cyc_Absyn_PointerType_struct*)_tmp62C;if(_tmp62D->tag != 5)goto
_LL32B;else{_tmp62E=_tmp62D->f1;}}_LL32A: {int okay=1;if(!((int(*)(int(*cmp)(int,
int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(
Cyc_Core_intcmp,(_tmp62B.ptr_atts).nullable,(_tmp62E.ptr_atts).nullable))okay=!((
int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,(_tmp62B.ptr_atts).nullable);
if(!((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*
y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,(_tmp62B.ptr_atts).bounds,(
_tmp62E.ptr_atts).bounds)){struct _tuple0 _tmp104E;struct _tuple0 _tmp642=(_tmp104E.f1=((
void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,(
_tmp62B.ptr_atts).bounds),((_tmp104E.f2=((void*(*)(void*y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,(_tmp62E.ptr_atts).bounds),
_tmp104E)));void*_tmp643;void*_tmp645;void*_tmp647;void*_tmp649;void*_tmp64B;
struct Cyc_Absyn_Exp*_tmp64D;void*_tmp64E;struct Cyc_Absyn_Exp*_tmp650;void*
_tmp651;void*_tmp653;struct Cyc_Absyn_Exp*_tmp655;_LL332: _tmp643=_tmp642.f1;{
struct Cyc_Absyn_Upper_b_struct*_tmp644=(struct Cyc_Absyn_Upper_b_struct*)_tmp643;
if(_tmp644->tag != 1)goto _LL334;}_tmp645=_tmp642.f2;{struct Cyc_Absyn_DynEither_b_struct*
_tmp646=(struct Cyc_Absyn_DynEither_b_struct*)_tmp645;if(_tmp646->tag != 0)goto
_LL334;}_LL333: goto _LL335;_LL334: _tmp647=_tmp642.f1;{struct Cyc_Absyn_DynEither_b_struct*
_tmp648=(struct Cyc_Absyn_DynEither_b_struct*)_tmp647;if(_tmp648->tag != 0)goto
_LL336;}_tmp649=_tmp642.f2;{struct Cyc_Absyn_DynEither_b_struct*_tmp64A=(struct
Cyc_Absyn_DynEither_b_struct*)_tmp649;if(_tmp64A->tag != 0)goto _LL336;}_LL335:
okay=1;goto _LL331;_LL336: _tmp64B=_tmp642.f1;{struct Cyc_Absyn_Upper_b_struct*
_tmp64C=(struct Cyc_Absyn_Upper_b_struct*)_tmp64B;if(_tmp64C->tag != 1)goto _LL338;
else{_tmp64D=_tmp64C->f1;}}_tmp64E=_tmp642.f2;{struct Cyc_Absyn_Upper_b_struct*
_tmp64F=(struct Cyc_Absyn_Upper_b_struct*)_tmp64E;if(_tmp64F->tag != 1)goto _LL338;
else{_tmp650=_tmp64F->f1;}}_LL337: okay=okay  && Cyc_Evexp_lte_const_exp(_tmp650,
_tmp64D);if(!((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(
0,(_tmp62E.ptr_atts).zero_term)){const char*_tmp1051;void*_tmp1050;(_tmp1050=0,
Cyc_Tcutil_warn(loc,((_tmp1051="implicit cast to shorter array",_tag_dyneither(
_tmp1051,sizeof(char),31))),_tag_dyneither(_tmp1050,sizeof(void*),0)));}goto
_LL331;_LL338: _tmp651=_tmp642.f1;{struct Cyc_Absyn_DynEither_b_struct*_tmp652=(
struct Cyc_Absyn_DynEither_b_struct*)_tmp651;if(_tmp652->tag != 0)goto _LL331;}
_tmp653=_tmp642.f2;{struct Cyc_Absyn_Upper_b_struct*_tmp654=(struct Cyc_Absyn_Upper_b_struct*)
_tmp653;if(_tmp654->tag != 1)goto _LL331;else{_tmp655=_tmp654->f1;}}_LL339: if(((
int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,(_tmp62B.ptr_atts).zero_term)
 && Cyc_Tcutil_is_bound_one((_tmp62E.ptr_atts).bounds))goto _LL331;okay=0;goto
_LL331;_LL331:;}okay=okay  && (!(_tmp62B.elt_tq).real_const  || (_tmp62E.elt_tq).real_const);
okay=okay  && (Cyc_Tcutil_unify((_tmp62B.ptr_atts).rgn,(_tmp62E.ptr_atts).rgn)
 || Cyc_Tcenv_region_outlives(te,(_tmp62B.ptr_atts).rgn,(_tmp62E.ptr_atts).rgn));
okay=okay  && (((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*
y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,(_tmp62B.ptr_atts).zero_term,(
_tmp62E.ptr_atts).zero_term) || ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(
1,(_tmp62B.ptr_atts).zero_term) && (_tmp62E.elt_tq).real_const);okay=okay  && (
Cyc_Tcutil_unify(_tmp62B.elt_typ,_tmp62E.elt_typ) || Cyc_Tcutil_ptrsubtype(te,0,
_tmp62B.elt_typ,_tmp62E.elt_typ));return okay;}_LL32B: _tmp62F=_tmp628.f1;{struct
Cyc_Absyn_ArrayType_struct*_tmp630=(struct Cyc_Absyn_ArrayType_struct*)_tmp62F;
if(_tmp630->tag != 9)goto _LL32D;else{_tmp631=_tmp630->f1;_tmp632=_tmp631.elt_type;
_tmp633=_tmp631.tq;_tmp634=_tmp631.num_elts;_tmp635=_tmp631.zero_term;}}_tmp636=
_tmp628.f2;{struct Cyc_Absyn_ArrayType_struct*_tmp637=(struct Cyc_Absyn_ArrayType_struct*)
_tmp636;if(_tmp637->tag != 9)goto _LL32D;else{_tmp638=_tmp637->f1;_tmp639=_tmp638.elt_type;
_tmp63A=_tmp638.tq;_tmp63B=_tmp638.num_elts;_tmp63C=_tmp638.zero_term;}}_LL32C: {
int okay;okay=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*
y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp635,_tmp63C) && ((_tmp634 != 0
 && _tmp63B != 0) && Cyc_Evexp_same_const_exp((struct Cyc_Absyn_Exp*)_tmp634,(
struct Cyc_Absyn_Exp*)_tmp63B));return(okay  && Cyc_Tcutil_unify(_tmp632,_tmp639))
 && (!_tmp633.real_const  || _tmp63A.real_const);}_LL32D: _tmp63D=_tmp628.f1;{
struct Cyc_Absyn_TagType_struct*_tmp63E=(struct Cyc_Absyn_TagType_struct*)_tmp63D;
if(_tmp63E->tag != 20)goto _LL32F;}_tmp63F=_tmp628.f2;{struct Cyc_Absyn_IntType_struct*
_tmp640=(struct Cyc_Absyn_IntType_struct*)_tmp63F;if(_tmp640->tag != 6)goto _LL32F;}
_LL32E: return 0;_LL32F:;_LL330: return Cyc_Tcutil_unify(t1,t2);_LL328:;}}int Cyc_Tcutil_is_pointer_type(
void*t);int Cyc_Tcutil_is_pointer_type(void*t){void*_tmp658=Cyc_Tcutil_compress(t);
_LL33B: {struct Cyc_Absyn_PointerType_struct*_tmp659=(struct Cyc_Absyn_PointerType_struct*)
_tmp658;if(_tmp659->tag != 5)goto _LL33D;}_LL33C: return 1;_LL33D:;_LL33E: return 0;
_LL33A:;}void*Cyc_Tcutil_pointer_elt_type(void*t);void*Cyc_Tcutil_pointer_elt_type(
void*t){void*_tmp65A=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp65C;void*
_tmp65D;_LL340: {struct Cyc_Absyn_PointerType_struct*_tmp65B=(struct Cyc_Absyn_PointerType_struct*)
_tmp65A;if(_tmp65B->tag != 5)goto _LL342;else{_tmp65C=_tmp65B->f1;_tmp65D=_tmp65C.elt_typ;}}
_LL341: return _tmp65D;_LL342:;_LL343: {const char*_tmp1054;void*_tmp1053;(_tmp1053=
0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp1054="pointer_elt_type",_tag_dyneither(_tmp1054,sizeof(char),17))),
_tag_dyneither(_tmp1053,sizeof(void*),0)));}_LL33F:;}void*Cyc_Tcutil_pointer_region(
void*t);void*Cyc_Tcutil_pointer_region(void*t){void*_tmp660=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_PtrInfo _tmp662;struct Cyc_Absyn_PtrAtts _tmp663;struct Cyc_Absyn_PtrAtts*
_tmp664;_LL345: {struct Cyc_Absyn_PointerType_struct*_tmp661=(struct Cyc_Absyn_PointerType_struct*)
_tmp660;if(_tmp661->tag != 5)goto _LL347;else{_tmp662=_tmp661->f1;_tmp663=_tmp662.ptr_atts;
_tmp664=(struct Cyc_Absyn_PtrAtts*)&(_tmp661->f1).ptr_atts;}}_LL346: return _tmp664->rgn;
_LL347:;_LL348: {const char*_tmp1057;void*_tmp1056;(_tmp1056=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1057="pointer_elt_type",
_tag_dyneither(_tmp1057,sizeof(char),17))),_tag_dyneither(_tmp1056,sizeof(void*),
0)));}_LL344:;}int Cyc_Tcutil_rgn_of_pointer(void*t,void**rgn);int Cyc_Tcutil_rgn_of_pointer(
void*t,void**rgn){void*_tmp667=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo
_tmp669;struct Cyc_Absyn_PtrAtts _tmp66A;void*_tmp66B;_LL34A: {struct Cyc_Absyn_PointerType_struct*
_tmp668=(struct Cyc_Absyn_PointerType_struct*)_tmp667;if(_tmp668->tag != 5)goto
_LL34C;else{_tmp669=_tmp668->f1;_tmp66A=_tmp669.ptr_atts;_tmp66B=_tmp66A.rgn;}}
_LL34B:*rgn=_tmp66B;return 1;_LL34C:;_LL34D: return 0;_LL349:;}int Cyc_Tcutil_is_pointer_or_boxed(
void*t,int*is_dyneither_ptr);int Cyc_Tcutil_is_pointer_or_boxed(void*t,int*
is_dyneither_ptr){void*_tmp66C=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo
_tmp66E;struct Cyc_Absyn_PtrAtts _tmp66F;union Cyc_Absyn_Constraint*_tmp670;_LL34F: {
struct Cyc_Absyn_PointerType_struct*_tmp66D=(struct Cyc_Absyn_PointerType_struct*)
_tmp66C;if(_tmp66D->tag != 5)goto _LL351;else{_tmp66E=_tmp66D->f1;_tmp66F=_tmp66E.ptr_atts;
_tmp670=_tmp66F.bounds;}}_LL350:*is_dyneither_ptr=((void*(*)(void*y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,_tmp670)== (void*)& Cyc_Absyn_DynEither_b_val;
return 1;_LL351:;_LL352: return Cyc_Tcutil_typ_kind(t)== Cyc_Absyn_BoxKind;_LL34E:;}
int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*e);int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*
e){void*_tmp671=e->r;union Cyc_Absyn_Cnst _tmp673;struct _tuple7 _tmp674;int _tmp675;
union Cyc_Absyn_Cnst _tmp677;struct _tuple5 _tmp678;char _tmp679;union Cyc_Absyn_Cnst
_tmp67B;struct _tuple6 _tmp67C;short _tmp67D;union Cyc_Absyn_Cnst _tmp67F;struct
_tuple8 _tmp680;long long _tmp681;void*_tmp683;struct Cyc_Absyn_Exp*_tmp684;_LL354: {
struct Cyc_Absyn_Const_e_struct*_tmp672=(struct Cyc_Absyn_Const_e_struct*)_tmp671;
if(_tmp672->tag != 0)goto _LL356;else{_tmp673=_tmp672->f1;if((_tmp673.Int_c).tag != 
4)goto _LL356;_tmp674=(struct _tuple7)(_tmp673.Int_c).val;_tmp675=_tmp674.f2;if(
_tmp675 != 0)goto _LL356;}}_LL355: goto _LL357;_LL356: {struct Cyc_Absyn_Const_e_struct*
_tmp676=(struct Cyc_Absyn_Const_e_struct*)_tmp671;if(_tmp676->tag != 0)goto _LL358;
else{_tmp677=_tmp676->f1;if((_tmp677.Char_c).tag != 2)goto _LL358;_tmp678=(struct
_tuple5)(_tmp677.Char_c).val;_tmp679=_tmp678.f2;if(_tmp679 != 0)goto _LL358;}}
_LL357: goto _LL359;_LL358: {struct Cyc_Absyn_Const_e_struct*_tmp67A=(struct Cyc_Absyn_Const_e_struct*)
_tmp671;if(_tmp67A->tag != 0)goto _LL35A;else{_tmp67B=_tmp67A->f1;if((_tmp67B.Short_c).tag
!= 3)goto _LL35A;_tmp67C=(struct _tuple6)(_tmp67B.Short_c).val;_tmp67D=_tmp67C.f2;
if(_tmp67D != 0)goto _LL35A;}}_LL359: goto _LL35B;_LL35A: {struct Cyc_Absyn_Const_e_struct*
_tmp67E=(struct Cyc_Absyn_Const_e_struct*)_tmp671;if(_tmp67E->tag != 0)goto _LL35C;
else{_tmp67F=_tmp67E->f1;if((_tmp67F.LongLong_c).tag != 5)goto _LL35C;_tmp680=(
struct _tuple8)(_tmp67F.LongLong_c).val;_tmp681=_tmp680.f2;if(_tmp681 != 0)goto
_LL35C;}}_LL35B: return 1;_LL35C: {struct Cyc_Absyn_Cast_e_struct*_tmp682=(struct
Cyc_Absyn_Cast_e_struct*)_tmp671;if(_tmp682->tag != 15)goto _LL35E;else{_tmp683=(
void*)_tmp682->f1;_tmp684=_tmp682->f2;}}_LL35D: return Cyc_Tcutil_is_zero(_tmp684)
 && Cyc_Tcutil_admits_zero(_tmp683);_LL35E:;_LL35F: return 0;_LL353:;}struct Cyc_Core_Opt
Cyc_Tcutil_trk={(void*)Cyc_Absyn_TopRgnKind};struct Cyc_Core_Opt Cyc_Tcutil_urk={(
void*)Cyc_Absyn_UniqueRgnKind};struct Cyc_Core_Opt Cyc_Tcutil_rk={(void*)Cyc_Absyn_RgnKind};
struct Cyc_Core_Opt Cyc_Tcutil_ak={(void*)Cyc_Absyn_AnyKind};struct Cyc_Core_Opt Cyc_Tcutil_bk={(
void*)Cyc_Absyn_BoxKind};struct Cyc_Core_Opt Cyc_Tcutil_mk={(void*)Cyc_Absyn_MemKind};
struct Cyc_Core_Opt Cyc_Tcutil_ik={(void*)Cyc_Absyn_IntKind};struct Cyc_Core_Opt Cyc_Tcutil_ek={(
void*)Cyc_Absyn_EffKind};struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_opt(enum Cyc_Absyn_Kind
k);struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_opt(enum Cyc_Absyn_Kind k){switch(k){
case Cyc_Absyn_AnyKind: _LL360: return(struct Cyc_Core_Opt*)& Cyc_Tcutil_ak;case Cyc_Absyn_MemKind:
_LL361: return(struct Cyc_Core_Opt*)& Cyc_Tcutil_mk;case Cyc_Absyn_BoxKind: _LL362:
return(struct Cyc_Core_Opt*)& Cyc_Tcutil_bk;case Cyc_Absyn_RgnKind: _LL363: return(
struct Cyc_Core_Opt*)& Cyc_Tcutil_rk;case Cyc_Absyn_UniqueRgnKind: _LL364: return(
struct Cyc_Core_Opt*)& Cyc_Tcutil_urk;case Cyc_Absyn_TopRgnKind: _LL365: return(
struct Cyc_Core_Opt*)& Cyc_Tcutil_trk;case Cyc_Absyn_EffKind: _LL366: return(struct
Cyc_Core_Opt*)& Cyc_Tcutil_ek;case Cyc_Absyn_IntKind: _LL367: return(struct Cyc_Core_Opt*)&
Cyc_Tcutil_ik;}}static void**Cyc_Tcutil_kind_to_b(enum Cyc_Absyn_Kind k);static
void**Cyc_Tcutil_kind_to_b(enum Cyc_Absyn_Kind k){static struct Cyc_Absyn_Eq_kb_struct
ab_v={0,Cyc_Absyn_AnyKind};static struct Cyc_Absyn_Eq_kb_struct mb_v={0,Cyc_Absyn_MemKind};
static struct Cyc_Absyn_Eq_kb_struct bb_v={0,Cyc_Absyn_BoxKind};static struct Cyc_Absyn_Eq_kb_struct
rb_v={0,Cyc_Absyn_RgnKind};static struct Cyc_Absyn_Eq_kb_struct ub_v={0,Cyc_Absyn_UniqueRgnKind};
static struct Cyc_Absyn_Eq_kb_struct tb_v={0,Cyc_Absyn_TopRgnKind};static struct Cyc_Absyn_Eq_kb_struct
eb_v={0,Cyc_Absyn_EffKind};static struct Cyc_Absyn_Eq_kb_struct ib_v={0,Cyc_Absyn_IntKind};
static void*ab=(void*)& ab_v;static void*mb=(void*)& mb_v;static void*bb=(void*)& bb_v;
static void*rb=(void*)& rb_v;static void*ub=(void*)& ub_v;static void*tb=(void*)& tb_v;
static void*eb=(void*)& eb_v;static void*ib=(void*)& ib_v;switch(k){case Cyc_Absyn_AnyKind:
_LL369: return& ab;case Cyc_Absyn_MemKind: _LL36A: return& mb;case Cyc_Absyn_BoxKind:
_LL36B: return& bb;case Cyc_Absyn_RgnKind: _LL36C: return& rb;case Cyc_Absyn_UniqueRgnKind:
_LL36D: return& ub;case Cyc_Absyn_TopRgnKind: _LL36E: return& tb;case Cyc_Absyn_EffKind:
_LL36F: return& eb;case Cyc_Absyn_IntKind: _LL370: return& ib;}}void*Cyc_Tcutil_kind_to_bound(
enum Cyc_Absyn_Kind k);void*Cyc_Tcutil_kind_to_bound(enum Cyc_Absyn_Kind k){return*
Cyc_Tcutil_kind_to_b(k);}struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_bound_opt(enum 
Cyc_Absyn_Kind k);struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_bound_opt(enum Cyc_Absyn_Kind
k){return(struct Cyc_Core_Opt*)Cyc_Tcutil_kind_to_b(k);}int Cyc_Tcutil_zero_to_null(
struct Cyc_Tcenv_Tenv*te,void*t2,struct Cyc_Absyn_Exp*e1);int Cyc_Tcutil_zero_to_null(
struct Cyc_Tcenv_Tenv*te,void*t2,struct Cyc_Absyn_Exp*e1){if(Cyc_Tcutil_is_pointer_type(
t2) && Cyc_Tcutil_is_zero(e1)){{struct Cyc_Absyn_Const_e_struct _tmp105A;struct Cyc_Absyn_Const_e_struct*
_tmp1059;e1->r=(void*)((_tmp1059=_cycalloc(sizeof(*_tmp1059)),((_tmp1059[0]=((
_tmp105A.tag=0,((_tmp105A.f1=Cyc_Absyn_Null_c,_tmp105A)))),_tmp1059))));}{struct
Cyc_Core_Opt*_tmp68F=Cyc_Tcenv_lookup_opt_type_vars(te);struct Cyc_Absyn_PointerType_struct
_tmp1064;struct Cyc_Absyn_PtrAtts _tmp1063;struct Cyc_Absyn_PtrInfo _tmp1062;struct
Cyc_Absyn_PointerType_struct*_tmp1061;struct Cyc_Absyn_PointerType_struct*_tmp690=(
_tmp1061=_cycalloc(sizeof(*_tmp1061)),((_tmp1061[0]=((_tmp1064.tag=5,((_tmp1064.f1=((
_tmp1062.elt_typ=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ak,_tmp68F),((
_tmp1062.elt_tq=Cyc_Absyn_empty_tqual(0),((_tmp1062.ptr_atts=((_tmp1063.rgn=Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_trk,_tmp68F),((_tmp1063.nullable=Cyc_Absyn_true_conref,((
_tmp1063.bounds=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((
_tmp1063.zero_term=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((
_tmp1063.ptrloc=0,_tmp1063)))))))))),_tmp1062)))))),_tmp1064)))),_tmp1061)));((
struct Cyc_Core_Opt*)_check_null(e1->topt))->v=(void*)((void*)_tmp690);return Cyc_Tcutil_coerce_arg(
te,e1,t2);}}return 0;}struct _dyneither_ptr Cyc_Tcutil_coercion2string(enum Cyc_Absyn_Coercion
c);struct _dyneither_ptr Cyc_Tcutil_coercion2string(enum Cyc_Absyn_Coercion c){
switch(c){case Cyc_Absyn_Unknown_coercion: _LL372: {const char*_tmp1065;return(
_tmp1065="unknown",_tag_dyneither(_tmp1065,sizeof(char),8));}case Cyc_Absyn_No_coercion:
_LL373: {const char*_tmp1066;return(_tmp1066="no coercion",_tag_dyneither(
_tmp1066,sizeof(char),12));}case Cyc_Absyn_NonNull_to_Null: _LL374: {const char*
_tmp1067;return(_tmp1067="null check",_tag_dyneither(_tmp1067,sizeof(char),11));}
case Cyc_Absyn_Other_coercion: _LL375: {const char*_tmp1068;return(_tmp1068="other coercion",
_tag_dyneither(_tmp1068,sizeof(char),15));}}}int Cyc_Tcutil_coerce_arg(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e,void*t2);int Cyc_Tcutil_coerce_arg(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e,void*t2){void*t1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);enum Cyc_Absyn_Coercion c;if(Cyc_Tcutil_unify(t1,t2))
return 1;if(Cyc_Tcutil_is_arithmetic_type(t2) && Cyc_Tcutil_is_arithmetic_type(t1)){
if(Cyc_Tcutil_will_lose_precision(t1,t2)){const char*_tmp106D;void*_tmp106C[2];
struct Cyc_String_pa_struct _tmp106B;struct Cyc_String_pa_struct _tmp106A;(_tmp106A.tag=
0,((_tmp106A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t2)),((_tmp106B.tag=0,((_tmp106B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t1)),((_tmp106C[0]=& _tmp106B,((_tmp106C[1]=& _tmp106A,Cyc_Tcutil_warn(
e->loc,((_tmp106D="integral size mismatch; %s -> %s conversion supplied",
_tag_dyneither(_tmp106D,sizeof(char),53))),_tag_dyneither(_tmp106C,sizeof(void*),
2)))))))))))));}Cyc_Tcutil_unchecked_cast(te,e,t2,Cyc_Absyn_No_coercion);return 1;}
else{if(Cyc_Tcutil_silent_castable(te,e->loc,t1,t2)){Cyc_Tcutil_unchecked_cast(
te,e,t2,Cyc_Absyn_Other_coercion);return 1;}else{if(Cyc_Tcutil_zero_to_null(te,t2,
e))return 1;else{if((c=Cyc_Tcutil_castable(te,e->loc,t1,t2))!= Cyc_Absyn_Unknown_coercion){
Cyc_Tcutil_unchecked_cast(te,e,t2,c);if(c != Cyc_Absyn_NonNull_to_Null){const char*
_tmp1072;void*_tmp1071[2];struct Cyc_String_pa_struct _tmp1070;struct Cyc_String_pa_struct
_tmp106F;(_tmp106F.tag=0,((_tmp106F.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(t2)),((_tmp1070.tag=0,((_tmp1070.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((
_tmp1071[0]=& _tmp1070,((_tmp1071[1]=& _tmp106F,Cyc_Tcutil_warn(e->loc,((_tmp1072="implicit cast from %s to %s",
_tag_dyneither(_tmp1072,sizeof(char),28))),_tag_dyneither(_tmp1071,sizeof(void*),
2)))))))))))));}return 1;}else{return 0;}}}}}int Cyc_Tcutil_coerce_assign(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e,void*t);int Cyc_Tcutil_coerce_assign(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e,void*t){return Cyc_Tcutil_coerce_arg(te,e,t);}int Cyc_Tcutil_coerceable(
void*t);int Cyc_Tcutil_coerceable(void*t){void*_tmp6A1=Cyc_Tcutil_compress(t);
_LL378: {struct Cyc_Absyn_IntType_struct*_tmp6A2=(struct Cyc_Absyn_IntType_struct*)
_tmp6A1;if(_tmp6A2->tag != 6)goto _LL37A;}_LL379: goto _LL37B;_LL37A: {struct Cyc_Absyn_FloatType_struct*
_tmp6A3=(struct Cyc_Absyn_FloatType_struct*)_tmp6A1;if(_tmp6A3->tag != 7)goto
_LL37C;}_LL37B: goto _LL37D;_LL37C: {struct Cyc_Absyn_DoubleType_struct*_tmp6A4=(
struct Cyc_Absyn_DoubleType_struct*)_tmp6A1;if(_tmp6A4->tag != 8)goto _LL37E;}
_LL37D: return 1;_LL37E:;_LL37F: return 0;_LL377:;}static struct _tuple11*Cyc_Tcutil_flatten_typ_f(
struct _tuple15*env,struct Cyc_Absyn_Aggrfield*x);static struct _tuple11*Cyc_Tcutil_flatten_typ_f(
struct _tuple15*env,struct Cyc_Absyn_Aggrfield*x){struct Cyc_List_List*_tmp6A6;
struct _RegionHandle*_tmp6A7;struct _tuple15 _tmp6A5=*env;_tmp6A6=_tmp6A5.f1;
_tmp6A7=_tmp6A5.f2;{struct _tuple11*_tmp1073;return(_tmp1073=_region_malloc(
_tmp6A7,sizeof(*_tmp1073)),((_tmp1073->f1=x->tq,((_tmp1073->f2=Cyc_Tcutil_rsubstitute(
_tmp6A7,_tmp6A6,x->type),_tmp1073)))));}}static struct _tuple11*Cyc_Tcutil_rcopy_tqt(
struct _RegionHandle*r,struct _tuple11*x);static struct _tuple11*Cyc_Tcutil_rcopy_tqt(
struct _RegionHandle*r,struct _tuple11*x){struct Cyc_Absyn_Tqual _tmp6AA;void*
_tmp6AB;struct _tuple11 _tmp6A9=*x;_tmp6AA=_tmp6A9.f1;_tmp6AB=_tmp6A9.f2;{struct
_tuple11*_tmp1074;return(_tmp1074=_region_malloc(r,sizeof(*_tmp1074)),((_tmp1074->f1=
_tmp6AA,((_tmp1074->f2=_tmp6AB,_tmp1074)))));}}static struct Cyc_List_List*Cyc_Tcutil_flatten_typ(
struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,void*t1);static struct Cyc_List_List*
Cyc_Tcutil_flatten_typ(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,void*t1){t1=
Cyc_Tcutil_compress(t1);{void*_tmp6AD=t1;struct Cyc_List_List*_tmp6B0;struct Cyc_Absyn_AggrInfo
_tmp6B2;union Cyc_Absyn_AggrInfoU _tmp6B3;struct Cyc_Absyn_Aggrdecl**_tmp6B4;struct
Cyc_Absyn_Aggrdecl*_tmp6B5;struct Cyc_List_List*_tmp6B6;enum Cyc_Absyn_AggrKind
_tmp6B8;struct Cyc_List_List*_tmp6B9;_LL381: {struct Cyc_Absyn_VoidType_struct*
_tmp6AE=(struct Cyc_Absyn_VoidType_struct*)_tmp6AD;if(_tmp6AE->tag != 0)goto _LL383;}
_LL382: return 0;_LL383: {struct Cyc_Absyn_TupleType_struct*_tmp6AF=(struct Cyc_Absyn_TupleType_struct*)
_tmp6AD;if(_tmp6AF->tag != 11)goto _LL385;else{_tmp6B0=_tmp6AF->f1;}}_LL384: return((
struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple11*(*f)(struct
_RegionHandle*,struct _tuple11*),struct _RegionHandle*env,struct Cyc_List_List*x))
Cyc_List_rmap_c)(r,Cyc_Tcutil_rcopy_tqt,r,_tmp6B0);_LL385: {struct Cyc_Absyn_AggrType_struct*
_tmp6B1=(struct Cyc_Absyn_AggrType_struct*)_tmp6AD;if(_tmp6B1->tag != 12)goto
_LL387;else{_tmp6B2=_tmp6B1->f1;_tmp6B3=_tmp6B2.aggr_info;if((_tmp6B3.KnownAggr).tag
!= 2)goto _LL387;_tmp6B4=(struct Cyc_Absyn_Aggrdecl**)(_tmp6B3.KnownAggr).val;
_tmp6B5=*_tmp6B4;_tmp6B6=_tmp6B2.targs;}}_LL386: if(((_tmp6B5->kind == Cyc_Absyn_UnionA
 || _tmp6B5->impl == 0) || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp6B5->impl))->exist_vars
!= 0) || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp6B5->impl))->rgn_po != 0){
struct _tuple11*_tmp1077;struct Cyc_List_List*_tmp1076;return(_tmp1076=
_region_malloc(r,sizeof(*_tmp1076)),((_tmp1076->hd=((_tmp1077=_region_malloc(r,
sizeof(*_tmp1077)),((_tmp1077->f1=Cyc_Absyn_const_tqual(0),((_tmp1077->f2=t1,
_tmp1077)))))),((_tmp1076->tl=0,_tmp1076)))));}{struct Cyc_List_List*_tmp6BC=((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp6B5->tvs,_tmp6B6);struct _tuple15
_tmp1078;struct _tuple15 env=(_tmp1078.f1=_tmp6BC,((_tmp1078.f2=r,_tmp1078)));
return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple11*(*f)(struct
_tuple15*,struct Cyc_Absyn_Aggrfield*),struct _tuple15*env,struct Cyc_List_List*x))
Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_typ_f,& env,((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp6B5->impl))->fields);}_LL387: {struct Cyc_Absyn_AnonAggrType_struct*
_tmp6B7=(struct Cyc_Absyn_AnonAggrType_struct*)_tmp6AD;if(_tmp6B7->tag != 13)goto
_LL389;else{_tmp6B8=_tmp6B7->f1;if(_tmp6B8 != Cyc_Absyn_StructA)goto _LL389;
_tmp6B9=_tmp6B7->f2;}}_LL388: {struct _tuple15 _tmp1079;struct _tuple15 env=(
_tmp1079.f1=0,((_tmp1079.f2=r,_tmp1079)));return((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _tuple11*(*f)(struct _tuple15*,struct Cyc_Absyn_Aggrfield*),
struct _tuple15*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_typ_f,&
env,_tmp6B9);}_LL389:;_LL38A: {struct _tuple11*_tmp107C;struct Cyc_List_List*
_tmp107B;return(_tmp107B=_region_malloc(r,sizeof(*_tmp107B)),((_tmp107B->hd=((
_tmp107C=_region_malloc(r,sizeof(*_tmp107C)),((_tmp107C->f1=Cyc_Absyn_const_tqual(
0),((_tmp107C->f2=t1,_tmp107C)))))),((_tmp107B->tl=0,_tmp107B)))));}_LL380:;}}
static int Cyc_Tcutil_sub_attributes(struct Cyc_List_List*a1,struct Cyc_List_List*a2);
static int Cyc_Tcutil_sub_attributes(struct Cyc_List_List*a1,struct Cyc_List_List*a2){{
struct Cyc_List_List*t=a1;for(0;t != 0;t=t->tl){void*_tmp6C1=(void*)t->hd;_LL38C: {
struct Cyc_Absyn_Pure_att_struct*_tmp6C2=(struct Cyc_Absyn_Pure_att_struct*)
_tmp6C1;if(_tmp6C2->tag != 21)goto _LL38E;}_LL38D: goto _LL38F;_LL38E: {struct Cyc_Absyn_Noreturn_att_struct*
_tmp6C3=(struct Cyc_Absyn_Noreturn_att_struct*)_tmp6C1;if(_tmp6C3->tag != 4)goto
_LL390;}_LL38F: goto _LL391;_LL390: {struct Cyc_Absyn_Initializes_att_struct*
_tmp6C4=(struct Cyc_Absyn_Initializes_att_struct*)_tmp6C1;if(_tmp6C4->tag != 20)
goto _LL392;}_LL391: continue;_LL392:;_LL393: if(!((int(*)(int(*pred)(void*,void*),
void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)t->hd,
a2))return 0;_LL38B:;}}for(0;a2 != 0;a2=a2->tl){if(!((int(*)(int(*pred)(void*,void*),
void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)
a2->hd,a1))return 0;}return 1;}static int Cyc_Tcutil_subtype(struct Cyc_Tcenv_Tenv*te,
struct Cyc_List_List*assume,void*t1,void*t2);static int Cyc_Tcutil_subtype(struct
Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2){if(Cyc_Tcutil_unify(
t1,t2))return 1;{struct Cyc_List_List*a=assume;for(0;a != 0;a=a->tl){if(Cyc_Tcutil_unify(
t1,(*((struct _tuple0*)a->hd)).f1) && Cyc_Tcutil_unify(t2,(*((struct _tuple0*)a->hd)).f2))
return 1;}}t1=Cyc_Tcutil_compress(t1);t2=Cyc_Tcutil_compress(t2);{struct _tuple0
_tmp107D;struct _tuple0 _tmp6C6=(_tmp107D.f1=t1,((_tmp107D.f2=t2,_tmp107D)));void*
_tmp6C7;struct Cyc_Absyn_PtrInfo _tmp6C9;void*_tmp6CA;struct Cyc_Absyn_Tqual _tmp6CB;
struct Cyc_Absyn_PtrAtts _tmp6CC;void*_tmp6CD;union Cyc_Absyn_Constraint*_tmp6CE;
union Cyc_Absyn_Constraint*_tmp6CF;union Cyc_Absyn_Constraint*_tmp6D0;void*_tmp6D1;
struct Cyc_Absyn_PtrInfo _tmp6D3;void*_tmp6D4;struct Cyc_Absyn_Tqual _tmp6D5;struct
Cyc_Absyn_PtrAtts _tmp6D6;void*_tmp6D7;union Cyc_Absyn_Constraint*_tmp6D8;union Cyc_Absyn_Constraint*
_tmp6D9;union Cyc_Absyn_Constraint*_tmp6DA;void*_tmp6DB;struct Cyc_Absyn_DatatypeFieldInfo
_tmp6DD;union Cyc_Absyn_DatatypeFieldInfoU _tmp6DE;struct _tuple3 _tmp6DF;struct Cyc_Absyn_Datatypedecl*
_tmp6E0;struct Cyc_Absyn_Datatypefield*_tmp6E1;struct Cyc_List_List*_tmp6E2;void*
_tmp6E3;struct Cyc_Absyn_DatatypeInfo _tmp6E5;union Cyc_Absyn_DatatypeInfoU _tmp6E6;
struct Cyc_Absyn_Datatypedecl**_tmp6E7;struct Cyc_Absyn_Datatypedecl*_tmp6E8;
struct Cyc_List_List*_tmp6E9;void*_tmp6EA;struct Cyc_Absyn_FnInfo _tmp6EC;void*
_tmp6ED;struct Cyc_Absyn_FnInfo _tmp6EF;_LL395: _tmp6C7=_tmp6C6.f1;{struct Cyc_Absyn_PointerType_struct*
_tmp6C8=(struct Cyc_Absyn_PointerType_struct*)_tmp6C7;if(_tmp6C8->tag != 5)goto
_LL397;else{_tmp6C9=_tmp6C8->f1;_tmp6CA=_tmp6C9.elt_typ;_tmp6CB=_tmp6C9.elt_tq;
_tmp6CC=_tmp6C9.ptr_atts;_tmp6CD=_tmp6CC.rgn;_tmp6CE=_tmp6CC.nullable;_tmp6CF=
_tmp6CC.bounds;_tmp6D0=_tmp6CC.zero_term;}}_tmp6D1=_tmp6C6.f2;{struct Cyc_Absyn_PointerType_struct*
_tmp6D2=(struct Cyc_Absyn_PointerType_struct*)_tmp6D1;if(_tmp6D2->tag != 5)goto
_LL397;else{_tmp6D3=_tmp6D2->f1;_tmp6D4=_tmp6D3.elt_typ;_tmp6D5=_tmp6D3.elt_tq;
_tmp6D6=_tmp6D3.ptr_atts;_tmp6D7=_tmp6D6.rgn;_tmp6D8=_tmp6D6.nullable;_tmp6D9=
_tmp6D6.bounds;_tmp6DA=_tmp6D6.zero_term;}}_LL396: if(_tmp6CB.real_const  && !
_tmp6D5.real_const)return 0;if((!((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*
x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp6CE,
_tmp6D8) && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmp6CE)) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmp6D8))return 0;if((!((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,
union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp6D0,
_tmp6DA) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmp6D0)) && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmp6DA))return 0;if(!Cyc_Tcutil_unify(_tmp6CD,_tmp6D7) && !Cyc_Tcenv_region_outlives(
te,_tmp6CD,_tmp6D7))return 0;if(!((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*
x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,
_tmp6CF,_tmp6D9)){struct _tuple0 _tmp107E;struct _tuple0 _tmp6F1=(_tmp107E.f1=((void*(*)(
union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp6CF),((_tmp107E.f2=((void*(*)(
union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp6D9),_tmp107E)));void*
_tmp6F2;void*_tmp6F4;void*_tmp6F6;struct Cyc_Absyn_Exp*_tmp6F8;void*_tmp6F9;
struct Cyc_Absyn_Exp*_tmp6FB;_LL39E: _tmp6F2=_tmp6F1.f1;{struct Cyc_Absyn_Upper_b_struct*
_tmp6F3=(struct Cyc_Absyn_Upper_b_struct*)_tmp6F2;if(_tmp6F3->tag != 1)goto _LL3A0;}
_tmp6F4=_tmp6F1.f2;{struct Cyc_Absyn_DynEither_b_struct*_tmp6F5=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp6F4;if(_tmp6F5->tag != 0)goto _LL3A0;}_LL39F: goto _LL39D;_LL3A0: _tmp6F6=_tmp6F1.f1;{
struct Cyc_Absyn_Upper_b_struct*_tmp6F7=(struct Cyc_Absyn_Upper_b_struct*)_tmp6F6;
if(_tmp6F7->tag != 1)goto _LL3A2;else{_tmp6F8=_tmp6F7->f1;}}_tmp6F9=_tmp6F1.f2;{
struct Cyc_Absyn_Upper_b_struct*_tmp6FA=(struct Cyc_Absyn_Upper_b_struct*)_tmp6F9;
if(_tmp6FA->tag != 1)goto _LL3A2;else{_tmp6FB=_tmp6FA->f1;}}_LL3A1: if(!Cyc_Evexp_lte_const_exp(
_tmp6FB,_tmp6F8))return 0;goto _LL39D;_LL3A2:;_LL3A3: return 0;_LL39D:;}{struct
_tuple0*_tmp1081;struct Cyc_List_List*_tmp1080;return Cyc_Tcutil_ptrsubtype(te,((
_tmp1080=_cycalloc(sizeof(*_tmp1080)),((_tmp1080->hd=((_tmp1081=_cycalloc(
sizeof(*_tmp1081)),((_tmp1081->f1=t1,((_tmp1081->f2=t2,_tmp1081)))))),((_tmp1080->tl=
assume,_tmp1080)))))),_tmp6CA,_tmp6D4);}_LL397: _tmp6DB=_tmp6C6.f1;{struct Cyc_Absyn_DatatypeFieldType_struct*
_tmp6DC=(struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp6DB;if(_tmp6DC->tag != 4)
goto _LL399;else{_tmp6DD=_tmp6DC->f1;_tmp6DE=_tmp6DD.field_info;if((_tmp6DE.KnownDatatypefield).tag
!= 2)goto _LL399;_tmp6DF=(struct _tuple3)(_tmp6DE.KnownDatatypefield).val;_tmp6E0=
_tmp6DF.f1;_tmp6E1=_tmp6DF.f2;_tmp6E2=_tmp6DD.targs;}}_tmp6E3=_tmp6C6.f2;{struct
Cyc_Absyn_DatatypeType_struct*_tmp6E4=(struct Cyc_Absyn_DatatypeType_struct*)
_tmp6E3;if(_tmp6E4->tag != 3)goto _LL399;else{_tmp6E5=_tmp6E4->f1;_tmp6E6=_tmp6E5.datatype_info;
if((_tmp6E6.KnownDatatype).tag != 2)goto _LL399;_tmp6E7=(struct Cyc_Absyn_Datatypedecl**)(
_tmp6E6.KnownDatatype).val;_tmp6E8=*_tmp6E7;_tmp6E9=_tmp6E5.targs;}}_LL398: if(
_tmp6E0 != _tmp6E8  && Cyc_Absyn_qvar_cmp(_tmp6E0->name,_tmp6E8->name)!= 0)return 0;
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp6E2)!= ((int(*)(struct
Cyc_List_List*x))Cyc_List_length)(_tmp6E9))return 0;for(0;_tmp6E2 != 0;(_tmp6E2=
_tmp6E2->tl,_tmp6E9=_tmp6E9->tl)){if(!Cyc_Tcutil_unify((void*)_tmp6E2->hd,(void*)((
struct Cyc_List_List*)_check_null(_tmp6E9))->hd))return 0;}return 1;_LL399: _tmp6EA=
_tmp6C6.f1;{struct Cyc_Absyn_FnType_struct*_tmp6EB=(struct Cyc_Absyn_FnType_struct*)
_tmp6EA;if(_tmp6EB->tag != 10)goto _LL39B;else{_tmp6EC=_tmp6EB->f1;}}_tmp6ED=
_tmp6C6.f2;{struct Cyc_Absyn_FnType_struct*_tmp6EE=(struct Cyc_Absyn_FnType_struct*)
_tmp6ED;if(_tmp6EE->tag != 10)goto _LL39B;else{_tmp6EF=_tmp6EE->f1;}}_LL39A: if(
_tmp6EC.tvars != 0  || _tmp6EF.tvars != 0){struct Cyc_List_List*_tmp6FE=_tmp6EC.tvars;
struct Cyc_List_List*_tmp6FF=_tmp6EF.tvars;if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp6FE)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp6FF))return 0;{
struct _RegionHandle*_tmp700=Cyc_Tcenv_get_fnrgn(te);struct Cyc_List_List*inst=0;
while(_tmp6FE != 0){if(!Cyc_Tcutil_unify_kindbound(((struct Cyc_Absyn_Tvar*)
_tmp6FE->hd)->kind,((struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)_check_null(
_tmp6FF))->hd)->kind))return 0;{struct _tuple14*_tmp108B;struct Cyc_Absyn_VarType_struct
_tmp108A;struct Cyc_Absyn_VarType_struct*_tmp1089;struct Cyc_List_List*_tmp1088;
inst=((_tmp1088=_region_malloc(_tmp700,sizeof(*_tmp1088)),((_tmp1088->hd=((
_tmp108B=_region_malloc(_tmp700,sizeof(*_tmp108B)),((_tmp108B->f1=(struct Cyc_Absyn_Tvar*)
_tmp6FF->hd,((_tmp108B->f2=(void*)((_tmp1089=_cycalloc(sizeof(*_tmp1089)),((
_tmp1089[0]=((_tmp108A.tag=2,((_tmp108A.f1=(struct Cyc_Absyn_Tvar*)_tmp6FE->hd,
_tmp108A)))),_tmp1089)))),_tmp108B)))))),((_tmp1088->tl=inst,_tmp1088))))));}
_tmp6FE=_tmp6FE->tl;_tmp6FF=_tmp6FF->tl;}if(inst != 0){_tmp6EC.tvars=0;_tmp6EF.tvars=
0;{struct Cyc_Absyn_FnType_struct _tmp1091;struct Cyc_Absyn_FnType_struct*_tmp1090;
struct Cyc_Absyn_FnType_struct _tmp108E;struct Cyc_Absyn_FnType_struct*_tmp108D;
return Cyc_Tcutil_subtype(te,assume,(void*)((_tmp108D=_cycalloc(sizeof(*_tmp108D)),((
_tmp108D[0]=((_tmp108E.tag=10,((_tmp108E.f1=_tmp6EC,_tmp108E)))),_tmp108D)))),(
void*)((_tmp1090=_cycalloc(sizeof(*_tmp1090)),((_tmp1090[0]=((_tmp1091.tag=10,((
_tmp1091.f1=_tmp6EF,_tmp1091)))),_tmp1090)))));}}}}if(!Cyc_Tcutil_subtype(te,
assume,_tmp6EC.ret_typ,_tmp6EF.ret_typ))return 0;{struct Cyc_List_List*_tmp709=
_tmp6EC.args;struct Cyc_List_List*_tmp70A=_tmp6EF.args;if(((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(_tmp709)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp70A))return 0;for(0;_tmp709 != 0;(_tmp709=_tmp709->tl,_tmp70A=_tmp70A->tl)){
struct Cyc_Absyn_Tqual _tmp70C;void*_tmp70D;struct _tuple9 _tmp70B=*((struct _tuple9*)
_tmp709->hd);_tmp70C=_tmp70B.f2;_tmp70D=_tmp70B.f3;{struct Cyc_Absyn_Tqual _tmp70F;
void*_tmp710;struct _tuple9 _tmp70E=*((struct _tuple9*)((struct Cyc_List_List*)
_check_null(_tmp70A))->hd);_tmp70F=_tmp70E.f2;_tmp710=_tmp70E.f3;if(_tmp70F.real_const
 && !_tmp70C.real_const  || !Cyc_Tcutil_subtype(te,assume,_tmp710,_tmp70D))return
0;}}if(_tmp6EC.c_varargs != _tmp6EF.c_varargs)return 0;if(_tmp6EC.cyc_varargs != 0
 && _tmp6EF.cyc_varargs != 0){struct Cyc_Absyn_VarargInfo _tmp711=*_tmp6EC.cyc_varargs;
struct Cyc_Absyn_VarargInfo _tmp712=*_tmp6EF.cyc_varargs;if((_tmp712.tq).real_const
 && !(_tmp711.tq).real_const  || !Cyc_Tcutil_subtype(te,assume,_tmp712.type,
_tmp711.type))return 0;}else{if(_tmp6EC.cyc_varargs != 0  || _tmp6EF.cyc_varargs != 
0)return 0;}if(!Cyc_Tcutil_subset_effect(1,(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp6EC.effect))->v,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp6EF.effect))->v))
return 0;if(!Cyc_Tcutil_sub_rgnpo(_tmp6EC.rgn_po,_tmp6EF.rgn_po))return 0;if(!Cyc_Tcutil_sub_attributes(
_tmp6EC.attributes,_tmp6EF.attributes))return 0;return 1;}_LL39B:;_LL39C: return 0;
_LL394:;}}static int Cyc_Tcutil_isomorphic(void*t1,void*t2);static int Cyc_Tcutil_isomorphic(
void*t1,void*t2){struct _tuple0 _tmp1092;struct _tuple0 _tmp714=(_tmp1092.f1=Cyc_Tcutil_compress(
t1),((_tmp1092.f2=Cyc_Tcutil_compress(t2),_tmp1092)));void*_tmp715;enum Cyc_Absyn_Size_of
_tmp717;void*_tmp718;enum Cyc_Absyn_Size_of _tmp71A;_LL3A5: _tmp715=_tmp714.f1;{
struct Cyc_Absyn_IntType_struct*_tmp716=(struct Cyc_Absyn_IntType_struct*)_tmp715;
if(_tmp716->tag != 6)goto _LL3A7;else{_tmp717=_tmp716->f2;}}_tmp718=_tmp714.f2;{
struct Cyc_Absyn_IntType_struct*_tmp719=(struct Cyc_Absyn_IntType_struct*)_tmp718;
if(_tmp719->tag != 6)goto _LL3A7;else{_tmp71A=_tmp719->f2;}}_LL3A6: return(_tmp717
== _tmp71A  || _tmp717 == Cyc_Absyn_Int_sz  && _tmp71A == Cyc_Absyn_Long_sz) || 
_tmp717 == Cyc_Absyn_Long_sz  && _tmp71A == Cyc_Absyn_Int_sz;_LL3A7:;_LL3A8: return 0;
_LL3A4:;}static int Cyc_Tcutil_ptrsubtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*
assume,void*t1,void*t2);static int Cyc_Tcutil_ptrsubtype(struct Cyc_Tcenv_Tenv*te,
struct Cyc_List_List*assume,void*t1,void*t2){struct _RegionHandle*_tmp71B=Cyc_Tcenv_get_fnrgn(
te);struct Cyc_List_List*tqs1=Cyc_Tcutil_flatten_typ(_tmp71B,te,t1);struct Cyc_List_List*
tqs2=Cyc_Tcutil_flatten_typ(_tmp71B,te,t2);for(0;tqs2 != 0;(tqs2=tqs2->tl,tqs1=
tqs1->tl)){if(tqs1 == 0)return 0;{struct _tuple11 _tmp71D;struct Cyc_Absyn_Tqual
_tmp71E;void*_tmp71F;struct _tuple11*_tmp71C=(struct _tuple11*)tqs1->hd;_tmp71D=*
_tmp71C;_tmp71E=_tmp71D.f1;_tmp71F=_tmp71D.f2;{struct _tuple11 _tmp721;struct Cyc_Absyn_Tqual
_tmp722;void*_tmp723;struct _tuple11*_tmp720=(struct _tuple11*)tqs2->hd;_tmp721=*
_tmp720;_tmp722=_tmp721.f1;_tmp723=_tmp721.f2;if(_tmp722.real_const  && Cyc_Tcutil_subtype(
te,assume,_tmp71F,_tmp723))continue;else{if(Cyc_Tcutil_unify(_tmp71F,_tmp723))
continue;else{if(Cyc_Tcutil_isomorphic(_tmp71F,_tmp723))continue;else{return 0;}}}}}}
return 1;}static int Cyc_Tcutil_is_char_type(void*t);static int Cyc_Tcutil_is_char_type(
void*t){void*_tmp724=Cyc_Tcutil_compress(t);enum Cyc_Absyn_Size_of _tmp726;_LL3AA: {
struct Cyc_Absyn_IntType_struct*_tmp725=(struct Cyc_Absyn_IntType_struct*)_tmp724;
if(_tmp725->tag != 6)goto _LL3AC;else{_tmp726=_tmp725->f2;if(_tmp726 != Cyc_Absyn_Char_sz)
goto _LL3AC;}}_LL3AB: return 1;_LL3AC:;_LL3AD: return 0;_LL3A9:;}enum Cyc_Absyn_Coercion
Cyc_Tcutil_castable(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void*
t1,void*t2);enum Cyc_Absyn_Coercion Cyc_Tcutil_castable(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,void*t1,void*t2){if(Cyc_Tcutil_unify(t1,t2))
return Cyc_Absyn_No_coercion;t1=Cyc_Tcutil_compress(t1);t2=Cyc_Tcutil_compress(t2);
if(t2 == (void*)& Cyc_Absyn_VoidType_val)return Cyc_Absyn_No_coercion;{void*_tmp727=
t2;enum Cyc_Absyn_Size_of _tmp729;enum Cyc_Absyn_Size_of _tmp72B;_LL3AF: {struct
Cyc_Absyn_IntType_struct*_tmp728=(struct Cyc_Absyn_IntType_struct*)_tmp727;if(
_tmp728->tag != 6)goto _LL3B1;else{_tmp729=_tmp728->f2;if(_tmp729 != Cyc_Absyn_Int_sz)
goto _LL3B1;}}_LL3B0: goto _LL3B2;_LL3B1: {struct Cyc_Absyn_IntType_struct*_tmp72A=(
struct Cyc_Absyn_IntType_struct*)_tmp727;if(_tmp72A->tag != 6)goto _LL3B3;else{
_tmp72B=_tmp72A->f2;if(_tmp72B != Cyc_Absyn_Long_sz)goto _LL3B3;}}_LL3B2: if(Cyc_Tcutil_typ_kind(
t1)== Cyc_Absyn_BoxKind)return Cyc_Absyn_No_coercion;goto _LL3AE;_LL3B3:;_LL3B4:
goto _LL3AE;_LL3AE:;}{void*_tmp72C=t1;struct Cyc_Absyn_PtrInfo _tmp72E;void*_tmp72F;
struct Cyc_Absyn_Tqual _tmp730;struct Cyc_Absyn_PtrAtts _tmp731;void*_tmp732;union
Cyc_Absyn_Constraint*_tmp733;union Cyc_Absyn_Constraint*_tmp734;union Cyc_Absyn_Constraint*
_tmp735;struct Cyc_Absyn_ArrayInfo _tmp737;void*_tmp738;struct Cyc_Absyn_Tqual
_tmp739;struct Cyc_Absyn_Exp*_tmp73A;union Cyc_Absyn_Constraint*_tmp73B;struct Cyc_Absyn_Enumdecl*
_tmp73D;void*_tmp742;_LL3B6: {struct Cyc_Absyn_PointerType_struct*_tmp72D=(struct
Cyc_Absyn_PointerType_struct*)_tmp72C;if(_tmp72D->tag != 5)goto _LL3B8;else{
_tmp72E=_tmp72D->f1;_tmp72F=_tmp72E.elt_typ;_tmp730=_tmp72E.elt_tq;_tmp731=
_tmp72E.ptr_atts;_tmp732=_tmp731.rgn;_tmp733=_tmp731.nullable;_tmp734=_tmp731.bounds;
_tmp735=_tmp731.zero_term;}}_LL3B7:{void*_tmp743=t2;struct Cyc_Absyn_PtrInfo
_tmp745;void*_tmp746;struct Cyc_Absyn_Tqual _tmp747;struct Cyc_Absyn_PtrAtts _tmp748;
void*_tmp749;union Cyc_Absyn_Constraint*_tmp74A;union Cyc_Absyn_Constraint*_tmp74B;
union Cyc_Absyn_Constraint*_tmp74C;_LL3C7: {struct Cyc_Absyn_PointerType_struct*
_tmp744=(struct Cyc_Absyn_PointerType_struct*)_tmp743;if(_tmp744->tag != 5)goto
_LL3C9;else{_tmp745=_tmp744->f1;_tmp746=_tmp745.elt_typ;_tmp747=_tmp745.elt_tq;
_tmp748=_tmp745.ptr_atts;_tmp749=_tmp748.rgn;_tmp74A=_tmp748.nullable;_tmp74B=
_tmp748.bounds;_tmp74C=_tmp748.zero_term;}}_LL3C8: {enum Cyc_Absyn_Coercion
coercion=Cyc_Absyn_Other_coercion;struct _tuple0*_tmp1095;struct Cyc_List_List*
_tmp1094;struct Cyc_List_List*_tmp74D=(_tmp1094=_cycalloc(sizeof(*_tmp1094)),((
_tmp1094->hd=((_tmp1095=_cycalloc(sizeof(*_tmp1095)),((_tmp1095->f1=t1,((
_tmp1095->f2=t2,_tmp1095)))))),((_tmp1094->tl=0,_tmp1094)))));int _tmp74E=Cyc_Tcutil_ptrsubtype(
te,_tmp74D,_tmp72F,_tmp746) && (!_tmp730.real_const  || _tmp747.real_const);Cyc_Tcutil_t1_failure=
t1;Cyc_Tcutil_t2_failure=t2;{int zeroterm_ok=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*
x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp735,
_tmp74C) || !((int(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp74C);
int _tmp74F=_tmp74E?0:((Cyc_Tcutil_bits_only(_tmp72F) && Cyc_Tcutil_is_char_type(
_tmp746)) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmp74C)) && (_tmp747.real_const  || !_tmp730.real_const);int bounds_ok=((int(*)(
int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))
Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp734,_tmp74B);if(!
bounds_ok  && !_tmp74F){struct _tuple0 _tmp1096;struct _tuple0 _tmp751=(_tmp1096.f1=((
void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp734),((_tmp1096.f2=((
void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp74B),_tmp1096)));
void*_tmp752;struct Cyc_Absyn_Exp*_tmp754;void*_tmp755;struct Cyc_Absyn_Exp*
_tmp757;_LL3CC: _tmp752=_tmp751.f1;{struct Cyc_Absyn_Upper_b_struct*_tmp753=(
struct Cyc_Absyn_Upper_b_struct*)_tmp752;if(_tmp753->tag != 1)goto _LL3CE;else{
_tmp754=_tmp753->f1;}}_tmp755=_tmp751.f2;{struct Cyc_Absyn_Upper_b_struct*_tmp756=(
struct Cyc_Absyn_Upper_b_struct*)_tmp755;if(_tmp756->tag != 1)goto _LL3CE;else{
_tmp757=_tmp756->f1;}}_LL3CD: if(Cyc_Evexp_lte_const_exp(_tmp757,_tmp754))
bounds_ok=1;goto _LL3CB;_LL3CE:;_LL3CF: bounds_ok=1;goto _LL3CB;_LL3CB:;}if(((int(*)(
int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp733) && !((int(*)(
int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp74A))coercion=Cyc_Absyn_NonNull_to_Null;
if(((bounds_ok  && zeroterm_ok) && (_tmp74E  || _tmp74F)) && (Cyc_Tcutil_unify(
_tmp732,_tmp749) || Cyc_Tcenv_region_outlives(te,_tmp732,_tmp749)))return
coercion;else{return Cyc_Absyn_Unknown_coercion;}}}_LL3C9:;_LL3CA: goto _LL3C6;
_LL3C6:;}return Cyc_Absyn_Unknown_coercion;_LL3B8: {struct Cyc_Absyn_ArrayType_struct*
_tmp736=(struct Cyc_Absyn_ArrayType_struct*)_tmp72C;if(_tmp736->tag != 9)goto
_LL3BA;else{_tmp737=_tmp736->f1;_tmp738=_tmp737.elt_type;_tmp739=_tmp737.tq;
_tmp73A=_tmp737.num_elts;_tmp73B=_tmp737.zero_term;}}_LL3B9:{void*_tmp75A=t2;
struct Cyc_Absyn_ArrayInfo _tmp75C;void*_tmp75D;struct Cyc_Absyn_Tqual _tmp75E;
struct Cyc_Absyn_Exp*_tmp75F;union Cyc_Absyn_Constraint*_tmp760;_LL3D1: {struct Cyc_Absyn_ArrayType_struct*
_tmp75B=(struct Cyc_Absyn_ArrayType_struct*)_tmp75A;if(_tmp75B->tag != 9)goto
_LL3D3;else{_tmp75C=_tmp75B->f1;_tmp75D=_tmp75C.elt_type;_tmp75E=_tmp75C.tq;
_tmp75F=_tmp75C.num_elts;_tmp760=_tmp75C.zero_term;}}_LL3D2: {int okay;okay=((
_tmp73A != 0  && _tmp75F != 0) && ((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*
x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp73B,
_tmp760)) && Cyc_Evexp_lte_const_exp((struct Cyc_Absyn_Exp*)_tmp75F,(struct Cyc_Absyn_Exp*)
_tmp73A);return(okay  && Cyc_Tcutil_unify(_tmp738,_tmp75D)) && (!_tmp739.real_const
 || _tmp75E.real_const)?Cyc_Absyn_Other_coercion: Cyc_Absyn_Unknown_coercion;}
_LL3D3:;_LL3D4: return Cyc_Absyn_Unknown_coercion;_LL3D0:;}return Cyc_Absyn_Unknown_coercion;
_LL3BA: {struct Cyc_Absyn_EnumType_struct*_tmp73C=(struct Cyc_Absyn_EnumType_struct*)
_tmp72C;if(_tmp73C->tag != 14)goto _LL3BC;else{_tmp73D=_tmp73C->f2;}}_LL3BB:{void*
_tmp761=t2;struct Cyc_Absyn_Enumdecl*_tmp763;_LL3D6: {struct Cyc_Absyn_EnumType_struct*
_tmp762=(struct Cyc_Absyn_EnumType_struct*)_tmp761;if(_tmp762->tag != 14)goto
_LL3D8;else{_tmp763=_tmp762->f2;}}_LL3D7: if((_tmp73D->fields != 0  && _tmp763->fields
!= 0) && ((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp73D->fields))->v)>= ((int(*)(struct Cyc_List_List*
x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(
_tmp763->fields))->v))return Cyc_Absyn_No_coercion;goto _LL3D5;_LL3D8:;_LL3D9: goto
_LL3D5;_LL3D5:;}goto _LL3BD;_LL3BC: {struct Cyc_Absyn_IntType_struct*_tmp73E=(
struct Cyc_Absyn_IntType_struct*)_tmp72C;if(_tmp73E->tag != 6)goto _LL3BE;}_LL3BD:
goto _LL3BF;_LL3BE: {struct Cyc_Absyn_FloatType_struct*_tmp73F=(struct Cyc_Absyn_FloatType_struct*)
_tmp72C;if(_tmp73F->tag != 7)goto _LL3C0;}_LL3BF: goto _LL3C1;_LL3C0: {struct Cyc_Absyn_DoubleType_struct*
_tmp740=(struct Cyc_Absyn_DoubleType_struct*)_tmp72C;if(_tmp740->tag != 8)goto
_LL3C2;}_LL3C1: return Cyc_Tcutil_coerceable(t2)?Cyc_Absyn_No_coercion: Cyc_Absyn_Unknown_coercion;
_LL3C2: {struct Cyc_Absyn_RgnHandleType_struct*_tmp741=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp72C;if(_tmp741->tag != 16)goto _LL3C4;else{_tmp742=(void*)_tmp741->f1;}}_LL3C3:{
void*_tmp764=t2;void*_tmp766;_LL3DB: {struct Cyc_Absyn_RgnHandleType_struct*
_tmp765=(struct Cyc_Absyn_RgnHandleType_struct*)_tmp764;if(_tmp765->tag != 16)goto
_LL3DD;else{_tmp766=(void*)_tmp765->f1;}}_LL3DC: if(Cyc_Tcenv_region_outlives(te,
_tmp742,_tmp766))return Cyc_Absyn_No_coercion;goto _LL3DA;_LL3DD:;_LL3DE: goto
_LL3DA;_LL3DA:;}return Cyc_Absyn_Unknown_coercion;_LL3C4:;_LL3C5: return Cyc_Absyn_Unknown_coercion;
_LL3B5:;}}void Cyc_Tcutil_unchecked_cast(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*
e,void*t,enum Cyc_Absyn_Coercion c);void Cyc_Tcutil_unchecked_cast(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e,void*t,enum Cyc_Absyn_Coercion c){if(!Cyc_Tcutil_unify((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,t)){struct Cyc_Absyn_Exp*
_tmp767=Cyc_Absyn_copy_exp(e);{struct Cyc_Absyn_Cast_e_struct _tmp1099;struct Cyc_Absyn_Cast_e_struct*
_tmp1098;e->r=(void*)((_tmp1098=_cycalloc(sizeof(*_tmp1098)),((_tmp1098[0]=((
_tmp1099.tag=15,((_tmp1099.f1=(void*)t,((_tmp1099.f2=_tmp767,((_tmp1099.f3=0,((
_tmp1099.f4=c,_tmp1099)))))))))),_tmp1098))));}{struct Cyc_Core_Opt*_tmp109A;e->topt=((
_tmp109A=_cycalloc(sizeof(*_tmp109A)),((_tmp109A->v=t,_tmp109A))));}}}int Cyc_Tcutil_is_integral(
struct Cyc_Absyn_Exp*e);int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*e){void*
_tmp76B=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
_LL3E0: {struct Cyc_Absyn_IntType_struct*_tmp76C=(struct Cyc_Absyn_IntType_struct*)
_tmp76B;if(_tmp76C->tag != 6)goto _LL3E2;}_LL3E1: goto _LL3E3;_LL3E2: {struct Cyc_Absyn_EnumType_struct*
_tmp76D=(struct Cyc_Absyn_EnumType_struct*)_tmp76B;if(_tmp76D->tag != 14)goto
_LL3E4;}_LL3E3: goto _LL3E5;_LL3E4: {struct Cyc_Absyn_AnonEnumType_struct*_tmp76E=(
struct Cyc_Absyn_AnonEnumType_struct*)_tmp76B;if(_tmp76E->tag != 15)goto _LL3E6;}
_LL3E5: goto _LL3E7;_LL3E6: {struct Cyc_Absyn_TagType_struct*_tmp76F=(struct Cyc_Absyn_TagType_struct*)
_tmp76B;if(_tmp76F->tag != 20)goto _LL3E8;}_LL3E7: return 1;_LL3E8: {struct Cyc_Absyn_Evar_struct*
_tmp770=(struct Cyc_Absyn_Evar_struct*)_tmp76B;if(_tmp770->tag != 1)goto _LL3EA;}
_LL3E9: return Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,
Cyc_Absyn_sint_typ);_LL3EA:;_LL3EB: return 0;_LL3DF:;}int Cyc_Tcutil_is_numeric(
struct Cyc_Absyn_Exp*e);int Cyc_Tcutil_is_numeric(struct Cyc_Absyn_Exp*e){if(Cyc_Tcutil_is_integral(
e))return 1;{void*_tmp771=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);_LL3ED: {struct Cyc_Absyn_FloatType_struct*_tmp772=(
struct Cyc_Absyn_FloatType_struct*)_tmp771;if(_tmp772->tag != 7)goto _LL3EF;}_LL3EE:
goto _LL3F0;_LL3EF: {struct Cyc_Absyn_DoubleType_struct*_tmp773=(struct Cyc_Absyn_DoubleType_struct*)
_tmp771;if(_tmp773->tag != 8)goto _LL3F1;}_LL3F0: return 1;_LL3F1:;_LL3F2: return 0;
_LL3EC:;}}int Cyc_Tcutil_is_function_type(void*t);int Cyc_Tcutil_is_function_type(
void*t){void*_tmp774=Cyc_Tcutil_compress(t);_LL3F4: {struct Cyc_Absyn_FnType_struct*
_tmp775=(struct Cyc_Absyn_FnType_struct*)_tmp774;if(_tmp775->tag != 10)goto _LL3F6;}
_LL3F5: return 1;_LL3F6:;_LL3F7: return 0;_LL3F3:;}void*Cyc_Tcutil_max_arithmetic_type(
void*t1,void*t2);void*Cyc_Tcutil_max_arithmetic_type(void*t1,void*t2){struct
_tuple0 _tmp109B;struct _tuple0 _tmp777=(_tmp109B.f1=t1,((_tmp109B.f2=t2,_tmp109B)));
void*_tmp778;int _tmp77A;void*_tmp77B;int _tmp77D;void*_tmp77E;void*_tmp780;void*
_tmp782;void*_tmp784;void*_tmp786;enum Cyc_Absyn_Sign _tmp788;enum Cyc_Absyn_Size_of
_tmp789;void*_tmp78A;enum Cyc_Absyn_Sign _tmp78C;enum Cyc_Absyn_Size_of _tmp78D;
void*_tmp78E;enum Cyc_Absyn_Size_of _tmp790;void*_tmp791;enum Cyc_Absyn_Size_of
_tmp793;void*_tmp794;enum Cyc_Absyn_Sign _tmp796;enum Cyc_Absyn_Size_of _tmp797;
void*_tmp798;enum Cyc_Absyn_Sign _tmp79A;enum Cyc_Absyn_Size_of _tmp79B;void*
_tmp79C;enum Cyc_Absyn_Sign _tmp79E;enum Cyc_Absyn_Size_of _tmp79F;void*_tmp7A0;
enum Cyc_Absyn_Sign _tmp7A2;enum Cyc_Absyn_Size_of _tmp7A3;void*_tmp7A4;enum Cyc_Absyn_Size_of
_tmp7A6;void*_tmp7A7;enum Cyc_Absyn_Size_of _tmp7A9;_LL3F9: _tmp778=_tmp777.f1;{
struct Cyc_Absyn_DoubleType_struct*_tmp779=(struct Cyc_Absyn_DoubleType_struct*)
_tmp778;if(_tmp779->tag != 8)goto _LL3FB;else{_tmp77A=_tmp779->f1;}}_tmp77B=
_tmp777.f2;{struct Cyc_Absyn_DoubleType_struct*_tmp77C=(struct Cyc_Absyn_DoubleType_struct*)
_tmp77B;if(_tmp77C->tag != 8)goto _LL3FB;else{_tmp77D=_tmp77C->f1;}}_LL3FA: if(
_tmp77A)return t1;else{return t2;}_LL3FB: _tmp77E=_tmp777.f1;{struct Cyc_Absyn_DoubleType_struct*
_tmp77F=(struct Cyc_Absyn_DoubleType_struct*)_tmp77E;if(_tmp77F->tag != 8)goto
_LL3FD;}_LL3FC: return t1;_LL3FD: _tmp780=_tmp777.f2;{struct Cyc_Absyn_DoubleType_struct*
_tmp781=(struct Cyc_Absyn_DoubleType_struct*)_tmp780;if(_tmp781->tag != 8)goto
_LL3FF;}_LL3FE: return t2;_LL3FF: _tmp782=_tmp777.f1;{struct Cyc_Absyn_FloatType_struct*
_tmp783=(struct Cyc_Absyn_FloatType_struct*)_tmp782;if(_tmp783->tag != 7)goto
_LL401;}_LL400: goto _LL402;_LL401: _tmp784=_tmp777.f2;{struct Cyc_Absyn_FloatType_struct*
_tmp785=(struct Cyc_Absyn_FloatType_struct*)_tmp784;if(_tmp785->tag != 7)goto
_LL403;}_LL402: return(void*)& Cyc_Absyn_FloatType_val;_LL403: _tmp786=_tmp777.f1;{
struct Cyc_Absyn_IntType_struct*_tmp787=(struct Cyc_Absyn_IntType_struct*)_tmp786;
if(_tmp787->tag != 6)goto _LL405;else{_tmp788=_tmp787->f1;if(_tmp788 != Cyc_Absyn_Unsigned)
goto _LL405;_tmp789=_tmp787->f2;if(_tmp789 != Cyc_Absyn_LongLong_sz)goto _LL405;}}
_LL404: goto _LL406;_LL405: _tmp78A=_tmp777.f2;{struct Cyc_Absyn_IntType_struct*
_tmp78B=(struct Cyc_Absyn_IntType_struct*)_tmp78A;if(_tmp78B->tag != 6)goto _LL407;
else{_tmp78C=_tmp78B->f1;if(_tmp78C != Cyc_Absyn_Unsigned)goto _LL407;_tmp78D=
_tmp78B->f2;if(_tmp78D != Cyc_Absyn_LongLong_sz)goto _LL407;}}_LL406: return Cyc_Absyn_ulonglong_typ;
_LL407: _tmp78E=_tmp777.f1;{struct Cyc_Absyn_IntType_struct*_tmp78F=(struct Cyc_Absyn_IntType_struct*)
_tmp78E;if(_tmp78F->tag != 6)goto _LL409;else{_tmp790=_tmp78F->f2;if(_tmp790 != Cyc_Absyn_LongLong_sz)
goto _LL409;}}_LL408: goto _LL40A;_LL409: _tmp791=_tmp777.f2;{struct Cyc_Absyn_IntType_struct*
_tmp792=(struct Cyc_Absyn_IntType_struct*)_tmp791;if(_tmp792->tag != 6)goto _LL40B;
else{_tmp793=_tmp792->f2;if(_tmp793 != Cyc_Absyn_LongLong_sz)goto _LL40B;}}_LL40A:
return Cyc_Absyn_slonglong_typ;_LL40B: _tmp794=_tmp777.f1;{struct Cyc_Absyn_IntType_struct*
_tmp795=(struct Cyc_Absyn_IntType_struct*)_tmp794;if(_tmp795->tag != 6)goto _LL40D;
else{_tmp796=_tmp795->f1;if(_tmp796 != Cyc_Absyn_Unsigned)goto _LL40D;_tmp797=
_tmp795->f2;if(_tmp797 != Cyc_Absyn_Long_sz)goto _LL40D;}}_LL40C: goto _LL40E;_LL40D:
_tmp798=_tmp777.f2;{struct Cyc_Absyn_IntType_struct*_tmp799=(struct Cyc_Absyn_IntType_struct*)
_tmp798;if(_tmp799->tag != 6)goto _LL40F;else{_tmp79A=_tmp799->f1;if(_tmp79A != Cyc_Absyn_Unsigned)
goto _LL40F;_tmp79B=_tmp799->f2;if(_tmp79B != Cyc_Absyn_Long_sz)goto _LL40F;}}
_LL40E: return Cyc_Absyn_ulong_typ;_LL40F: _tmp79C=_tmp777.f1;{struct Cyc_Absyn_IntType_struct*
_tmp79D=(struct Cyc_Absyn_IntType_struct*)_tmp79C;if(_tmp79D->tag != 6)goto _LL411;
else{_tmp79E=_tmp79D->f1;if(_tmp79E != Cyc_Absyn_Unsigned)goto _LL411;_tmp79F=
_tmp79D->f2;if(_tmp79F != Cyc_Absyn_Int_sz)goto _LL411;}}_LL410: goto _LL412;_LL411:
_tmp7A0=_tmp777.f2;{struct Cyc_Absyn_IntType_struct*_tmp7A1=(struct Cyc_Absyn_IntType_struct*)
_tmp7A0;if(_tmp7A1->tag != 6)goto _LL413;else{_tmp7A2=_tmp7A1->f1;if(_tmp7A2 != Cyc_Absyn_Unsigned)
goto _LL413;_tmp7A3=_tmp7A1->f2;if(_tmp7A3 != Cyc_Absyn_Int_sz)goto _LL413;}}_LL412:
return Cyc_Absyn_uint_typ;_LL413: _tmp7A4=_tmp777.f1;{struct Cyc_Absyn_IntType_struct*
_tmp7A5=(struct Cyc_Absyn_IntType_struct*)_tmp7A4;if(_tmp7A5->tag != 6)goto _LL415;
else{_tmp7A6=_tmp7A5->f2;if(_tmp7A6 != Cyc_Absyn_Long_sz)goto _LL415;}}_LL414: goto
_LL416;_LL415: _tmp7A7=_tmp777.f2;{struct Cyc_Absyn_IntType_struct*_tmp7A8=(struct
Cyc_Absyn_IntType_struct*)_tmp7A7;if(_tmp7A8->tag != 6)goto _LL417;else{_tmp7A9=
_tmp7A8->f2;if(_tmp7A9 != Cyc_Absyn_Long_sz)goto _LL417;}}_LL416: return Cyc_Absyn_slong_typ;
_LL417:;_LL418: return Cyc_Absyn_sint_typ;_LL3F8:;}void Cyc_Tcutil_check_contains_assign(
struct Cyc_Absyn_Exp*e);void Cyc_Tcutil_check_contains_assign(struct Cyc_Absyn_Exp*
e){void*_tmp7AA=e->r;struct Cyc_Core_Opt*_tmp7AC;_LL41A: {struct Cyc_Absyn_AssignOp_e_struct*
_tmp7AB=(struct Cyc_Absyn_AssignOp_e_struct*)_tmp7AA;if(_tmp7AB->tag != 4)goto
_LL41C;else{_tmp7AC=_tmp7AB->f2;if(_tmp7AC != 0)goto _LL41C;}}_LL41B:{const char*
_tmp109E;void*_tmp109D;(_tmp109D=0,Cyc_Tcutil_warn(e->loc,((_tmp109E="assignment in test",
_tag_dyneither(_tmp109E,sizeof(char),19))),_tag_dyneither(_tmp109D,sizeof(void*),
0)));}goto _LL419;_LL41C:;_LL41D: goto _LL419;_LL419:;}static int Cyc_Tcutil_constrain_kinds(
void*c1,void*c2);static int Cyc_Tcutil_constrain_kinds(void*c1,void*c2){c1=Cyc_Absyn_compress_kb(
c1);c2=Cyc_Absyn_compress_kb(c2);{struct _tuple0 _tmp109F;struct _tuple0 _tmp7B0=(
_tmp109F.f1=c1,((_tmp109F.f2=c2,_tmp109F)));void*_tmp7B1;enum Cyc_Absyn_Kind
_tmp7B3;void*_tmp7B4;enum Cyc_Absyn_Kind _tmp7B6;void*_tmp7B7;struct Cyc_Core_Opt*
_tmp7B9;struct Cyc_Core_Opt**_tmp7BA;void*_tmp7BB;struct Cyc_Core_Opt*_tmp7BD;
struct Cyc_Core_Opt**_tmp7BE;void*_tmp7BF;struct Cyc_Core_Opt*_tmp7C1;struct Cyc_Core_Opt**
_tmp7C2;enum Cyc_Absyn_Kind _tmp7C3;void*_tmp7C4;enum Cyc_Absyn_Kind _tmp7C6;void*
_tmp7C7;enum Cyc_Absyn_Kind _tmp7C9;void*_tmp7CA;struct Cyc_Core_Opt*_tmp7CC;
struct Cyc_Core_Opt**_tmp7CD;enum Cyc_Absyn_Kind _tmp7CE;void*_tmp7CF;struct Cyc_Core_Opt*
_tmp7D1;struct Cyc_Core_Opt**_tmp7D2;enum Cyc_Absyn_Kind _tmp7D3;void*_tmp7D4;
struct Cyc_Core_Opt*_tmp7D6;struct Cyc_Core_Opt**_tmp7D7;enum Cyc_Absyn_Kind
_tmp7D8;_LL41F: _tmp7B1=_tmp7B0.f1;{struct Cyc_Absyn_Eq_kb_struct*_tmp7B2=(struct
Cyc_Absyn_Eq_kb_struct*)_tmp7B1;if(_tmp7B2->tag != 0)goto _LL421;else{_tmp7B3=
_tmp7B2->f1;}}_tmp7B4=_tmp7B0.f2;{struct Cyc_Absyn_Eq_kb_struct*_tmp7B5=(struct
Cyc_Absyn_Eq_kb_struct*)_tmp7B4;if(_tmp7B5->tag != 0)goto _LL421;else{_tmp7B6=
_tmp7B5->f1;}}_LL420: return _tmp7B3 == _tmp7B6;_LL421: _tmp7B7=_tmp7B0.f2;{struct
Cyc_Absyn_Unknown_kb_struct*_tmp7B8=(struct Cyc_Absyn_Unknown_kb_struct*)_tmp7B7;
if(_tmp7B8->tag != 1)goto _LL423;else{_tmp7B9=_tmp7B8->f1;_tmp7BA=(struct Cyc_Core_Opt**)&
_tmp7B8->f1;}}_LL422:{struct Cyc_Core_Opt*_tmp10A0;*_tmp7BA=((_tmp10A0=_cycalloc(
sizeof(*_tmp10A0)),((_tmp10A0->v=c1,_tmp10A0))));}return 1;_LL423: _tmp7BB=_tmp7B0.f1;{
struct Cyc_Absyn_Unknown_kb_struct*_tmp7BC=(struct Cyc_Absyn_Unknown_kb_struct*)
_tmp7BB;if(_tmp7BC->tag != 1)goto _LL425;else{_tmp7BD=_tmp7BC->f1;_tmp7BE=(struct
Cyc_Core_Opt**)& _tmp7BC->f1;}}_LL424:{struct Cyc_Core_Opt*_tmp10A1;*_tmp7BE=((
_tmp10A1=_cycalloc(sizeof(*_tmp10A1)),((_tmp10A1->v=c2,_tmp10A1))));}return 1;
_LL425: _tmp7BF=_tmp7B0.f1;{struct Cyc_Absyn_Less_kb_struct*_tmp7C0=(struct Cyc_Absyn_Less_kb_struct*)
_tmp7BF;if(_tmp7C0->tag != 2)goto _LL427;else{_tmp7C1=_tmp7C0->f1;_tmp7C2=(struct
Cyc_Core_Opt**)& _tmp7C0->f1;_tmp7C3=_tmp7C0->f2;}}_tmp7C4=_tmp7B0.f2;{struct Cyc_Absyn_Eq_kb_struct*
_tmp7C5=(struct Cyc_Absyn_Eq_kb_struct*)_tmp7C4;if(_tmp7C5->tag != 0)goto _LL427;
else{_tmp7C6=_tmp7C5->f1;}}_LL426: if(Cyc_Tcutil_kind_leq(_tmp7C6,_tmp7C3)){{
struct Cyc_Core_Opt*_tmp10A2;*_tmp7C2=((_tmp10A2=_cycalloc(sizeof(*_tmp10A2)),((
_tmp10A2->v=c2,_tmp10A2))));}return 1;}else{return 0;}_LL427: _tmp7C7=_tmp7B0.f1;{
struct Cyc_Absyn_Eq_kb_struct*_tmp7C8=(struct Cyc_Absyn_Eq_kb_struct*)_tmp7C7;if(
_tmp7C8->tag != 0)goto _LL429;else{_tmp7C9=_tmp7C8->f1;}}_tmp7CA=_tmp7B0.f2;{
struct Cyc_Absyn_Less_kb_struct*_tmp7CB=(struct Cyc_Absyn_Less_kb_struct*)_tmp7CA;
if(_tmp7CB->tag != 2)goto _LL429;else{_tmp7CC=_tmp7CB->f1;_tmp7CD=(struct Cyc_Core_Opt**)&
_tmp7CB->f1;_tmp7CE=_tmp7CB->f2;}}_LL428: if(Cyc_Tcutil_kind_leq(_tmp7C9,_tmp7CE)){{
struct Cyc_Core_Opt*_tmp10A3;*_tmp7CD=((_tmp10A3=_cycalloc(sizeof(*_tmp10A3)),((
_tmp10A3->v=c1,_tmp10A3))));}return 1;}else{return 0;}_LL429: _tmp7CF=_tmp7B0.f1;{
struct Cyc_Absyn_Less_kb_struct*_tmp7D0=(struct Cyc_Absyn_Less_kb_struct*)_tmp7CF;
if(_tmp7D0->tag != 2)goto _LL41E;else{_tmp7D1=_tmp7D0->f1;_tmp7D2=(struct Cyc_Core_Opt**)&
_tmp7D0->f1;_tmp7D3=_tmp7D0->f2;}}_tmp7D4=_tmp7B0.f2;{struct Cyc_Absyn_Less_kb_struct*
_tmp7D5=(struct Cyc_Absyn_Less_kb_struct*)_tmp7D4;if(_tmp7D5->tag != 2)goto _LL41E;
else{_tmp7D6=_tmp7D5->f1;_tmp7D7=(struct Cyc_Core_Opt**)& _tmp7D5->f1;_tmp7D8=
_tmp7D5->f2;}}_LL42A: if(Cyc_Tcutil_kind_leq(_tmp7D3,_tmp7D8)){{struct Cyc_Core_Opt*
_tmp10A4;*_tmp7D7=((_tmp10A4=_cycalloc(sizeof(*_tmp10A4)),((_tmp10A4->v=c1,
_tmp10A4))));}return 1;}else{if(Cyc_Tcutil_kind_leq(_tmp7D8,_tmp7D3)){{struct Cyc_Core_Opt*
_tmp10A5;*_tmp7D2=((_tmp10A5=_cycalloc(sizeof(*_tmp10A5)),((_tmp10A5->v=c2,
_tmp10A5))));}return 1;}else{return 0;}}_LL41E:;}}static int Cyc_Tcutil_tvar_id_counter=
0;int Cyc_Tcutil_new_tvar_id();int Cyc_Tcutil_new_tvar_id(){return Cyc_Tcutil_tvar_id_counter
++;}static int Cyc_Tcutil_tvar_counter=0;struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(
void*k);struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k){int i=Cyc_Tcutil_tvar_counter
++;const char*_tmp10A9;void*_tmp10A8[1];struct Cyc_Int_pa_struct _tmp10A7;struct
_dyneither_ptr s=(struct _dyneither_ptr)((_tmp10A7.tag=1,((_tmp10A7.f1=(
unsigned long)i,((_tmp10A8[0]=& _tmp10A7,Cyc_aprintf(((_tmp10A9="#%d",
_tag_dyneither(_tmp10A9,sizeof(char),4))),_tag_dyneither(_tmp10A8,sizeof(void*),
1))))))));struct _dyneither_ptr*_tmp10AC;struct Cyc_Absyn_Tvar*_tmp10AB;return(
_tmp10AB=_cycalloc(sizeof(*_tmp10AB)),((_tmp10AB->name=((_tmp10AC=_cycalloc(
sizeof(struct _dyneither_ptr)* 1),((_tmp10AC[0]=s,_tmp10AC)))),((_tmp10AB->identity=
- 1,((_tmp10AB->kind=k,_tmp10AB)))))));}int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*
t);int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*t){struct _dyneither_ptr
_tmp7E4=*t->name;return*((const char*)_check_dyneither_subscript(_tmp7E4,sizeof(
char),0))== '#';}void Cyc_Tcutil_rewrite_temp_tvar(struct Cyc_Absyn_Tvar*t);void
Cyc_Tcutil_rewrite_temp_tvar(struct Cyc_Absyn_Tvar*t){{const char*_tmp10B0;void*
_tmp10AF[1];struct Cyc_String_pa_struct _tmp10AE;(_tmp10AE.tag=0,((_tmp10AE.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*t->name),((_tmp10AF[0]=& _tmp10AE,
Cyc_printf(((_tmp10B0="%s",_tag_dyneither(_tmp10B0,sizeof(char),3))),
_tag_dyneither(_tmp10AF,sizeof(void*),1)))))));}if(!Cyc_Tcutil_is_temp_tvar(t))
return;{const char*_tmp10B1;struct _dyneither_ptr _tmp7E8=Cyc_strconcat(((_tmp10B1="`",
_tag_dyneither(_tmp10B1,sizeof(char),2))),(struct _dyneither_ptr)*t->name);{char
_tmp10B4;char _tmp10B3;struct _dyneither_ptr _tmp10B2;(_tmp10B2=_dyneither_ptr_plus(
_tmp7E8,sizeof(char),1),((_tmp10B3=*((char*)_check_dyneither_subscript(_tmp10B2,
sizeof(char),0)),((_tmp10B4='t',((_get_dyneither_size(_tmp10B2,sizeof(char))== 1
 && (_tmp10B3 == '\000'  && _tmp10B4 != '\000')?_throw_arraybounds(): 1,*((char*)
_tmp10B2.curr)=_tmp10B4)))))));}{struct _dyneither_ptr*_tmp10B5;t->name=((
_tmp10B5=_cycalloc(sizeof(struct _dyneither_ptr)* 1),((_tmp10B5[0]=(struct
_dyneither_ptr)_tmp7E8,_tmp10B5))));}}}struct _tuple20{struct _dyneither_ptr*f1;
struct Cyc_Absyn_Tqual f2;void*f3;};static struct _tuple9*Cyc_Tcutil_fndecl2typ_f(
struct _tuple20*x);static struct _tuple9*Cyc_Tcutil_fndecl2typ_f(struct _tuple20*x){
struct Cyc_Core_Opt*_tmp10B8;struct _tuple9*_tmp10B7;return(_tmp10B7=_cycalloc(
sizeof(*_tmp10B7)),((_tmp10B7->f1=(struct Cyc_Core_Opt*)((_tmp10B8=_cycalloc(
sizeof(*_tmp10B8)),((_tmp10B8->v=(*x).f1,_tmp10B8)))),((_tmp10B7->f2=(*x).f2,((
_tmp10B7->f3=(*x).f3,_tmp10B7)))))));}void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*
fd);void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*fd){if(fd->cached_typ == 0){
struct Cyc_List_List*_tmp7F0=0;{struct Cyc_List_List*atts=fd->attributes;for(0;
atts != 0;atts=atts->tl){if(Cyc_Absyn_fntype_att((void*)atts->hd)){struct Cyc_List_List*
_tmp10B9;_tmp7F0=((_tmp10B9=_cycalloc(sizeof(*_tmp10B9)),((_tmp10B9->hd=(void*)
atts->hd,((_tmp10B9->tl=_tmp7F0,_tmp10B9))))));}}}{struct Cyc_Absyn_FnType_struct
_tmp10BF;struct Cyc_Absyn_FnInfo _tmp10BE;struct Cyc_Absyn_FnType_struct*_tmp10BD;
return(void*)((_tmp10BD=_cycalloc(sizeof(*_tmp10BD)),((_tmp10BD[0]=((_tmp10BF.tag=
10,((_tmp10BF.f1=((_tmp10BE.tvars=fd->tvs,((_tmp10BE.effect=fd->effect,((
_tmp10BE.ret_typ=fd->ret_type,((_tmp10BE.args=((struct Cyc_List_List*(*)(struct
_tuple9*(*f)(struct _tuple20*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_fndecl2typ_f,
fd->args),((_tmp10BE.c_varargs=fd->c_varargs,((_tmp10BE.cyc_varargs=fd->cyc_varargs,((
_tmp10BE.rgn_po=fd->rgn_po,((_tmp10BE.attributes=_tmp7F0,_tmp10BE)))))))))))))))),
_tmp10BF)))),_tmp10BD))));}}return(void*)((struct Cyc_Core_Opt*)_check_null(fd->cached_typ))->v;}
struct _tuple21{void*f1;struct Cyc_Absyn_Tqual f2;void*f3;};static void*Cyc_Tcutil_fst_fdarg(
struct _tuple21*t);static void*Cyc_Tcutil_fst_fdarg(struct _tuple21*t){return(*t).f1;}
void*Cyc_Tcutil_snd_tqt(struct _tuple11*t);void*Cyc_Tcutil_snd_tqt(struct _tuple11*
t){return(*t).f2;}static struct _tuple11*Cyc_Tcutil_map2_tq(struct _tuple11*pr,void*
t);static struct _tuple11*Cyc_Tcutil_map2_tq(struct _tuple11*pr,void*t){struct
_tuple11*_tmp10C0;return(_tmp10C0=_cycalloc(sizeof(*_tmp10C0)),((_tmp10C0->f1=(*
pr).f1,((_tmp10C0->f2=t,_tmp10C0)))));}struct _tuple22{struct Cyc_Core_Opt*f1;
struct Cyc_Absyn_Tqual f2;};struct _tuple23{struct _tuple22*f1;void*f2;};static
struct _tuple23*Cyc_Tcutil_substitute_f1(struct _RegionHandle*rgn,struct _tuple9*y);
static struct _tuple23*Cyc_Tcutil_substitute_f1(struct _RegionHandle*rgn,struct
_tuple9*y){struct _tuple22*_tmp10C3;struct _tuple23*_tmp10C2;return(_tmp10C2=
_region_malloc(rgn,sizeof(*_tmp10C2)),((_tmp10C2->f1=((_tmp10C3=_region_malloc(
rgn,sizeof(*_tmp10C3)),((_tmp10C3->f1=(*y).f1,((_tmp10C3->f2=(*y).f2,_tmp10C3)))))),((
_tmp10C2->f2=(*y).f3,_tmp10C2)))));}static struct _tuple9*Cyc_Tcutil_substitute_f2(
struct _tuple23*w);static struct _tuple9*Cyc_Tcutil_substitute_f2(struct _tuple23*w){
struct _tuple22*_tmp7F9;void*_tmp7FA;struct _tuple23 _tmp7F8=*w;_tmp7F9=_tmp7F8.f1;
_tmp7FA=_tmp7F8.f2;{struct Cyc_Core_Opt*_tmp7FC;struct Cyc_Absyn_Tqual _tmp7FD;
struct _tuple22 _tmp7FB=*_tmp7F9;_tmp7FC=_tmp7FB.f1;_tmp7FD=_tmp7FB.f2;{struct
_tuple9*_tmp10C4;return(_tmp10C4=_cycalloc(sizeof(*_tmp10C4)),((_tmp10C4->f1=
_tmp7FC,((_tmp10C4->f2=_tmp7FD,((_tmp10C4->f3=_tmp7FA,_tmp10C4)))))));}}}static
void*Cyc_Tcutil_field_type(struct Cyc_Absyn_Aggrfield*f);static void*Cyc_Tcutil_field_type(
struct Cyc_Absyn_Aggrfield*f){return f->type;}static struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_zip_field_type(
struct Cyc_Absyn_Aggrfield*f,void*t);static struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_zip_field_type(
struct Cyc_Absyn_Aggrfield*f,void*t){struct Cyc_Absyn_Aggrfield*_tmp10C5;return(
_tmp10C5=_cycalloc(sizeof(*_tmp10C5)),((_tmp10C5->name=f->name,((_tmp10C5->tq=f->tq,((
_tmp10C5->type=t,((_tmp10C5->width=f->width,((_tmp10C5->attributes=f->attributes,
_tmp10C5)))))))))));}static struct Cyc_List_List*Cyc_Tcutil_substs(struct
_RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*ts);static struct
Cyc_Absyn_Exp*Cyc_Tcutil_copye(struct Cyc_Absyn_Exp*old,void*r);static struct Cyc_Absyn_Exp*
Cyc_Tcutil_copye(struct Cyc_Absyn_Exp*old,void*r){struct Cyc_Absyn_Exp*_tmp10C6;
return(_tmp10C6=_cycalloc(sizeof(*_tmp10C6)),((_tmp10C6->topt=old->topt,((
_tmp10C6->r=r,((_tmp10C6->loc=old->loc,((_tmp10C6->annot=old->annot,_tmp10C6)))))))));}
static struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubsexp(struct _RegionHandle*r,struct Cyc_List_List*
inst,struct Cyc_Absyn_Exp*e);static struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubsexp(struct
_RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_Absyn_Exp*e){void*_tmp801=e->r;
enum Cyc_Absyn_Primop _tmp807;struct Cyc_List_List*_tmp808;struct Cyc_Absyn_Exp*
_tmp80A;struct Cyc_Absyn_Exp*_tmp80B;struct Cyc_Absyn_Exp*_tmp80C;struct Cyc_Absyn_Exp*
_tmp80E;struct Cyc_Absyn_Exp*_tmp80F;struct Cyc_Absyn_Exp*_tmp811;struct Cyc_Absyn_Exp*
_tmp812;struct Cyc_Absyn_Exp*_tmp814;struct Cyc_Absyn_Exp*_tmp815;void*_tmp817;
struct Cyc_Absyn_Exp*_tmp818;int _tmp819;enum Cyc_Absyn_Coercion _tmp81A;void*
_tmp81C;struct Cyc_Absyn_Exp*_tmp81E;void*_tmp820;void*_tmp821;void*_tmp823;
_LL42C: {struct Cyc_Absyn_Const_e_struct*_tmp802=(struct Cyc_Absyn_Const_e_struct*)
_tmp801;if(_tmp802->tag != 0)goto _LL42E;}_LL42D: goto _LL42F;_LL42E: {struct Cyc_Absyn_Enum_e_struct*
_tmp803=(struct Cyc_Absyn_Enum_e_struct*)_tmp801;if(_tmp803->tag != 33)goto _LL430;}
_LL42F: goto _LL431;_LL430: {struct Cyc_Absyn_AnonEnum_e_struct*_tmp804=(struct Cyc_Absyn_AnonEnum_e_struct*)
_tmp801;if(_tmp804->tag != 34)goto _LL432;}_LL431: goto _LL433;_LL432: {struct Cyc_Absyn_Var_e_struct*
_tmp805=(struct Cyc_Absyn_Var_e_struct*)_tmp801;if(_tmp805->tag != 1)goto _LL434;}
_LL433: return e;_LL434: {struct Cyc_Absyn_Primop_e_struct*_tmp806=(struct Cyc_Absyn_Primop_e_struct*)
_tmp801;if(_tmp806->tag != 3)goto _LL436;else{_tmp807=_tmp806->f1;_tmp808=_tmp806->f2;}}
_LL435: if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp808)== 1){struct
Cyc_Absyn_Exp*_tmp824=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(
_tmp808))->hd;struct Cyc_Absyn_Exp*_tmp825=Cyc_Tcutil_rsubsexp(r,inst,_tmp824);
if(_tmp825 == _tmp824)return e;{struct Cyc_Absyn_Primop_e_struct _tmp10CC;struct Cyc_Absyn_Exp*
_tmp10CB[1];struct Cyc_Absyn_Primop_e_struct*_tmp10CA;return Cyc_Tcutil_copye(e,(
void*)((_tmp10CA=_cycalloc(sizeof(*_tmp10CA)),((_tmp10CA[0]=((_tmp10CC.tag=3,((
_tmp10CC.f1=_tmp807,((_tmp10CC.f2=((_tmp10CB[0]=_tmp825,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp10CB,sizeof(struct Cyc_Absyn_Exp*),
1)))),_tmp10CC)))))),_tmp10CA)))));}}else{if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp808)== 2){struct Cyc_Absyn_Exp*_tmp829=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp808))->hd;struct Cyc_Absyn_Exp*_tmp82A=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp808->tl))->hd;struct Cyc_Absyn_Exp*_tmp82B=
Cyc_Tcutil_rsubsexp(r,inst,_tmp829);struct Cyc_Absyn_Exp*_tmp82C=Cyc_Tcutil_rsubsexp(
r,inst,_tmp82A);if(_tmp82B == _tmp829  && _tmp82C == _tmp82A)return e;{struct Cyc_Absyn_Primop_e_struct
_tmp10D2;struct Cyc_Absyn_Exp*_tmp10D1[2];struct Cyc_Absyn_Primop_e_struct*
_tmp10D0;return Cyc_Tcutil_copye(e,(void*)((_tmp10D0=_cycalloc(sizeof(*_tmp10D0)),((
_tmp10D0[0]=((_tmp10D2.tag=3,((_tmp10D2.f1=_tmp807,((_tmp10D2.f2=((_tmp10D1[1]=
_tmp82C,((_tmp10D1[0]=_tmp82B,((struct Cyc_List_List*(*)(struct _dyneither_ptr))
Cyc_List_list)(_tag_dyneither(_tmp10D1,sizeof(struct Cyc_Absyn_Exp*),2)))))),
_tmp10D2)))))),_tmp10D0)))));}}else{const char*_tmp10D5;void*_tmp10D4;return(
_tmp10D4=0,((struct Cyc_Absyn_Exp*(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp10D5="primop does not have 1 or 2 args!",
_tag_dyneither(_tmp10D5,sizeof(char),34))),_tag_dyneither(_tmp10D4,sizeof(void*),
0)));}}_LL436: {struct Cyc_Absyn_Conditional_e_struct*_tmp809=(struct Cyc_Absyn_Conditional_e_struct*)
_tmp801;if(_tmp809->tag != 6)goto _LL438;else{_tmp80A=_tmp809->f1;_tmp80B=_tmp809->f2;
_tmp80C=_tmp809->f3;}}_LL437: {struct Cyc_Absyn_Exp*_tmp832=Cyc_Tcutil_rsubsexp(r,
inst,_tmp80A);struct Cyc_Absyn_Exp*_tmp833=Cyc_Tcutil_rsubsexp(r,inst,_tmp80B);
struct Cyc_Absyn_Exp*_tmp834=Cyc_Tcutil_rsubsexp(r,inst,_tmp80C);if((_tmp832 == 
_tmp80A  && _tmp833 == _tmp80B) && _tmp834 == _tmp80C)return e;{struct Cyc_Absyn_Conditional_e_struct
_tmp10D8;struct Cyc_Absyn_Conditional_e_struct*_tmp10D7;return Cyc_Tcutil_copye(e,(
void*)((_tmp10D7=_cycalloc(sizeof(*_tmp10D7)),((_tmp10D7[0]=((_tmp10D8.tag=6,((
_tmp10D8.f1=_tmp832,((_tmp10D8.f2=_tmp833,((_tmp10D8.f3=_tmp834,_tmp10D8)))))))),
_tmp10D7)))));}}_LL438: {struct Cyc_Absyn_And_e_struct*_tmp80D=(struct Cyc_Absyn_And_e_struct*)
_tmp801;if(_tmp80D->tag != 7)goto _LL43A;else{_tmp80E=_tmp80D->f1;_tmp80F=_tmp80D->f2;}}
_LL439: {struct Cyc_Absyn_Exp*_tmp837=Cyc_Tcutil_rsubsexp(r,inst,_tmp80E);struct
Cyc_Absyn_Exp*_tmp838=Cyc_Tcutil_rsubsexp(r,inst,_tmp80F);if(_tmp837 == _tmp80E
 && _tmp838 == _tmp80F)return e;{struct Cyc_Absyn_And_e_struct _tmp10DB;struct Cyc_Absyn_And_e_struct*
_tmp10DA;return Cyc_Tcutil_copye(e,(void*)((_tmp10DA=_cycalloc(sizeof(*_tmp10DA)),((
_tmp10DA[0]=((_tmp10DB.tag=7,((_tmp10DB.f1=_tmp837,((_tmp10DB.f2=_tmp838,
_tmp10DB)))))),_tmp10DA)))));}}_LL43A: {struct Cyc_Absyn_Or_e_struct*_tmp810=(
struct Cyc_Absyn_Or_e_struct*)_tmp801;if(_tmp810->tag != 8)goto _LL43C;else{_tmp811=
_tmp810->f1;_tmp812=_tmp810->f2;}}_LL43B: {struct Cyc_Absyn_Exp*_tmp83B=Cyc_Tcutil_rsubsexp(
r,inst,_tmp811);struct Cyc_Absyn_Exp*_tmp83C=Cyc_Tcutil_rsubsexp(r,inst,_tmp812);
if(_tmp83B == _tmp811  && _tmp83C == _tmp812)return e;{struct Cyc_Absyn_Or_e_struct
_tmp10DE;struct Cyc_Absyn_Or_e_struct*_tmp10DD;return Cyc_Tcutil_copye(e,(void*)((
_tmp10DD=_cycalloc(sizeof(*_tmp10DD)),((_tmp10DD[0]=((_tmp10DE.tag=8,((_tmp10DE.f1=
_tmp83B,((_tmp10DE.f2=_tmp83C,_tmp10DE)))))),_tmp10DD)))));}}_LL43C: {struct Cyc_Absyn_SeqExp_e_struct*
_tmp813=(struct Cyc_Absyn_SeqExp_e_struct*)_tmp801;if(_tmp813->tag != 9)goto _LL43E;
else{_tmp814=_tmp813->f1;_tmp815=_tmp813->f2;}}_LL43D: {struct Cyc_Absyn_Exp*
_tmp83F=Cyc_Tcutil_rsubsexp(r,inst,_tmp814);struct Cyc_Absyn_Exp*_tmp840=Cyc_Tcutil_rsubsexp(
r,inst,_tmp815);if(_tmp83F == _tmp814  && _tmp840 == _tmp815)return e;{struct Cyc_Absyn_SeqExp_e_struct
_tmp10E1;struct Cyc_Absyn_SeqExp_e_struct*_tmp10E0;return Cyc_Tcutil_copye(e,(void*)((
_tmp10E0=_cycalloc(sizeof(*_tmp10E0)),((_tmp10E0[0]=((_tmp10E1.tag=9,((_tmp10E1.f1=
_tmp83F,((_tmp10E1.f2=_tmp840,_tmp10E1)))))),_tmp10E0)))));}}_LL43E: {struct Cyc_Absyn_Cast_e_struct*
_tmp816=(struct Cyc_Absyn_Cast_e_struct*)_tmp801;if(_tmp816->tag != 15)goto _LL440;
else{_tmp817=(void*)_tmp816->f1;_tmp818=_tmp816->f2;_tmp819=_tmp816->f3;_tmp81A=
_tmp816->f4;}}_LL43F: {struct Cyc_Absyn_Exp*_tmp843=Cyc_Tcutil_rsubsexp(r,inst,
_tmp818);void*_tmp844=Cyc_Tcutil_rsubstitute(r,inst,_tmp817);if(_tmp843 == 
_tmp818  && _tmp844 == _tmp817)return e;{struct Cyc_Absyn_Cast_e_struct _tmp10E4;
struct Cyc_Absyn_Cast_e_struct*_tmp10E3;return Cyc_Tcutil_copye(e,(void*)((
_tmp10E3=_cycalloc(sizeof(*_tmp10E3)),((_tmp10E3[0]=((_tmp10E4.tag=15,((_tmp10E4.f1=(
void*)_tmp844,((_tmp10E4.f2=_tmp843,((_tmp10E4.f3=_tmp819,((_tmp10E4.f4=_tmp81A,
_tmp10E4)))))))))),_tmp10E3)))));}}_LL440: {struct Cyc_Absyn_Sizeoftyp_e_struct*
_tmp81B=(struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp801;if(_tmp81B->tag != 18)goto
_LL442;else{_tmp81C=(void*)_tmp81B->f1;}}_LL441: {void*_tmp847=Cyc_Tcutil_rsubstitute(
r,inst,_tmp81C);if(_tmp847 == _tmp81C)return e;{struct Cyc_Absyn_Sizeoftyp_e_struct
_tmp10E7;struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp10E6;return Cyc_Tcutil_copye(e,(
void*)((_tmp10E6=_cycalloc(sizeof(*_tmp10E6)),((_tmp10E6[0]=((_tmp10E7.tag=18,((
_tmp10E7.f1=(void*)_tmp847,_tmp10E7)))),_tmp10E6)))));}}_LL442: {struct Cyc_Absyn_Sizeofexp_e_struct*
_tmp81D=(struct Cyc_Absyn_Sizeofexp_e_struct*)_tmp801;if(_tmp81D->tag != 19)goto
_LL444;else{_tmp81E=_tmp81D->f1;}}_LL443: {struct Cyc_Absyn_Exp*_tmp84A=Cyc_Tcutil_rsubsexp(
r,inst,_tmp81E);if(_tmp84A == _tmp81E)return e;{struct Cyc_Absyn_Sizeofexp_e_struct
_tmp10EA;struct Cyc_Absyn_Sizeofexp_e_struct*_tmp10E9;return Cyc_Tcutil_copye(e,(
void*)((_tmp10E9=_cycalloc(sizeof(*_tmp10E9)),((_tmp10E9[0]=((_tmp10EA.tag=19,((
_tmp10EA.f1=_tmp84A,_tmp10EA)))),_tmp10E9)))));}}_LL444: {struct Cyc_Absyn_Offsetof_e_struct*
_tmp81F=(struct Cyc_Absyn_Offsetof_e_struct*)_tmp801;if(_tmp81F->tag != 20)goto
_LL446;else{_tmp820=(void*)_tmp81F->f1;_tmp821=(void*)_tmp81F->f2;}}_LL445: {
void*_tmp84D=Cyc_Tcutil_rsubstitute(r,inst,_tmp820);if(_tmp84D == _tmp820)return e;{
struct Cyc_Absyn_Offsetof_e_struct _tmp10ED;struct Cyc_Absyn_Offsetof_e_struct*
_tmp10EC;return Cyc_Tcutil_copye(e,(void*)((_tmp10EC=_cycalloc(sizeof(*_tmp10EC)),((
_tmp10EC[0]=((_tmp10ED.tag=20,((_tmp10ED.f1=(void*)_tmp84D,((_tmp10ED.f2=(void*)
_tmp821,_tmp10ED)))))),_tmp10EC)))));}}_LL446: {struct Cyc_Absyn_Valueof_e_struct*
_tmp822=(struct Cyc_Absyn_Valueof_e_struct*)_tmp801;if(_tmp822->tag != 40)goto
_LL448;else{_tmp823=(void*)_tmp822->f1;}}_LL447: {void*_tmp850=Cyc_Tcutil_rsubstitute(
r,inst,_tmp823);if(_tmp850 == _tmp823)return e;{struct Cyc_Absyn_Valueof_e_struct
_tmp10F0;struct Cyc_Absyn_Valueof_e_struct*_tmp10EF;return Cyc_Tcutil_copye(e,(
void*)((_tmp10EF=_cycalloc(sizeof(*_tmp10EF)),((_tmp10EF[0]=((_tmp10F0.tag=40,((
_tmp10F0.f1=(void*)_tmp850,_tmp10F0)))),_tmp10EF)))));}}_LL448:;_LL449: {const
char*_tmp10F3;void*_tmp10F2;return(_tmp10F2=0,((struct Cyc_Absyn_Exp*(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp10F3="non-type-level-expression in Tcutil::rsubsexp",
_tag_dyneither(_tmp10F3,sizeof(char),46))),_tag_dyneither(_tmp10F2,sizeof(void*),
0)));}_LL42B:;}void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*rgn,struct Cyc_List_List*
inst,void*t);void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*rgn,struct Cyc_List_List*
inst,void*t){void*_tmp855=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*_tmp857;
struct Cyc_Absyn_AggrInfo _tmp859;union Cyc_Absyn_AggrInfoU _tmp85A;struct Cyc_List_List*
_tmp85B;struct Cyc_Absyn_DatatypeInfo _tmp85D;union Cyc_Absyn_DatatypeInfoU _tmp85E;
struct Cyc_List_List*_tmp85F;struct Cyc_Absyn_DatatypeFieldInfo _tmp861;union Cyc_Absyn_DatatypeFieldInfoU
_tmp862;struct Cyc_List_List*_tmp863;struct _tuple2*_tmp865;struct Cyc_List_List*
_tmp866;struct Cyc_Absyn_Typedefdecl*_tmp867;void**_tmp868;struct Cyc_Absyn_ArrayInfo
_tmp86A;void*_tmp86B;struct Cyc_Absyn_Tqual _tmp86C;struct Cyc_Absyn_Exp*_tmp86D;
union Cyc_Absyn_Constraint*_tmp86E;struct Cyc_Position_Segment*_tmp86F;struct Cyc_Absyn_PtrInfo
_tmp871;void*_tmp872;struct Cyc_Absyn_Tqual _tmp873;struct Cyc_Absyn_PtrAtts _tmp874;
void*_tmp875;union Cyc_Absyn_Constraint*_tmp876;union Cyc_Absyn_Constraint*_tmp877;
union Cyc_Absyn_Constraint*_tmp878;struct Cyc_Absyn_FnInfo _tmp87A;struct Cyc_List_List*
_tmp87B;struct Cyc_Core_Opt*_tmp87C;void*_tmp87D;struct Cyc_List_List*_tmp87E;int
_tmp87F;struct Cyc_Absyn_VarargInfo*_tmp880;struct Cyc_List_List*_tmp881;struct Cyc_List_List*
_tmp882;struct Cyc_List_List*_tmp884;enum Cyc_Absyn_AggrKind _tmp886;struct Cyc_List_List*
_tmp887;struct Cyc_Core_Opt*_tmp889;void*_tmp88B;void*_tmp88D;void*_tmp88E;void*
_tmp890;struct Cyc_Absyn_Exp*_tmp892;void*_tmp89C;void*_tmp89E;struct Cyc_List_List*
_tmp8A0;_LL44B: {struct Cyc_Absyn_VarType_struct*_tmp856=(struct Cyc_Absyn_VarType_struct*)
_tmp855;if(_tmp856->tag != 2)goto _LL44D;else{_tmp857=_tmp856->f1;}}_LL44C: {
struct _handler_cons _tmp8A1;_push_handler(& _tmp8A1);{int _tmp8A3=0;if(setjmp(
_tmp8A1.handler))_tmp8A3=1;if(!_tmp8A3){{void*_tmp8A4=((void*(*)(int(*cmp)(
struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*
x))Cyc_List_assoc_cmp)(Cyc_Absyn_tvar_cmp,inst,_tmp857);_npop_handler(0);return
_tmp8A4;};_pop_handler();}else{void*_tmp8A2=(void*)_exn_thrown;void*_tmp8A6=
_tmp8A2;_LL480: {struct Cyc_Core_Not_found_struct*_tmp8A7=(struct Cyc_Core_Not_found_struct*)
_tmp8A6;if(_tmp8A7->tag != Cyc_Core_Not_found)goto _LL482;}_LL481: return t;_LL482:;
_LL483:(void)_throw(_tmp8A6);_LL47F:;}}}_LL44D: {struct Cyc_Absyn_AggrType_struct*
_tmp858=(struct Cyc_Absyn_AggrType_struct*)_tmp855;if(_tmp858->tag != 12)goto
_LL44F;else{_tmp859=_tmp858->f1;_tmp85A=_tmp859.aggr_info;_tmp85B=_tmp859.targs;}}
_LL44E: {struct Cyc_List_List*_tmp8A8=Cyc_Tcutil_substs(rgn,inst,_tmp85B);struct
Cyc_Absyn_AggrType_struct _tmp10F9;struct Cyc_Absyn_AggrInfo _tmp10F8;struct Cyc_Absyn_AggrType_struct*
_tmp10F7;return _tmp8A8 == _tmp85B?t:(void*)((_tmp10F7=_cycalloc(sizeof(*_tmp10F7)),((
_tmp10F7[0]=((_tmp10F9.tag=12,((_tmp10F9.f1=((_tmp10F8.aggr_info=_tmp85A,((
_tmp10F8.targs=_tmp8A8,_tmp10F8)))),_tmp10F9)))),_tmp10F7))));}_LL44F: {struct
Cyc_Absyn_DatatypeType_struct*_tmp85C=(struct Cyc_Absyn_DatatypeType_struct*)
_tmp855;if(_tmp85C->tag != 3)goto _LL451;else{_tmp85D=_tmp85C->f1;_tmp85E=_tmp85D.datatype_info;
_tmp85F=_tmp85D.targs;}}_LL450: {struct Cyc_List_List*_tmp8AC=Cyc_Tcutil_substs(
rgn,inst,_tmp85F);struct Cyc_Absyn_DatatypeType_struct _tmp10FF;struct Cyc_Absyn_DatatypeInfo
_tmp10FE;struct Cyc_Absyn_DatatypeType_struct*_tmp10FD;return _tmp8AC == _tmp85F?t:(
void*)((_tmp10FD=_cycalloc(sizeof(*_tmp10FD)),((_tmp10FD[0]=((_tmp10FF.tag=3,((
_tmp10FF.f1=((_tmp10FE.datatype_info=_tmp85E,((_tmp10FE.targs=_tmp8AC,_tmp10FE)))),
_tmp10FF)))),_tmp10FD))));}_LL451: {struct Cyc_Absyn_DatatypeFieldType_struct*
_tmp860=(struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp855;if(_tmp860->tag != 4)
goto _LL453;else{_tmp861=_tmp860->f1;_tmp862=_tmp861.field_info;_tmp863=_tmp861.targs;}}
_LL452: {struct Cyc_List_List*_tmp8B0=Cyc_Tcutil_substs(rgn,inst,_tmp863);struct
Cyc_Absyn_DatatypeFieldType_struct _tmp1105;struct Cyc_Absyn_DatatypeFieldInfo
_tmp1104;struct Cyc_Absyn_DatatypeFieldType_struct*_tmp1103;return _tmp8B0 == 
_tmp863?t:(void*)((_tmp1103=_cycalloc(sizeof(*_tmp1103)),((_tmp1103[0]=((
_tmp1105.tag=4,((_tmp1105.f1=((_tmp1104.field_info=_tmp862,((_tmp1104.targs=
_tmp8B0,_tmp1104)))),_tmp1105)))),_tmp1103))));}_LL453: {struct Cyc_Absyn_TypedefType_struct*
_tmp864=(struct Cyc_Absyn_TypedefType_struct*)_tmp855;if(_tmp864->tag != 18)goto
_LL455;else{_tmp865=_tmp864->f1;_tmp866=_tmp864->f2;_tmp867=_tmp864->f3;_tmp868=
_tmp864->f4;}}_LL454: {struct Cyc_List_List*_tmp8B4=Cyc_Tcutil_substs(rgn,inst,
_tmp866);struct Cyc_Absyn_TypedefType_struct _tmp1108;struct Cyc_Absyn_TypedefType_struct*
_tmp1107;return _tmp8B4 == _tmp866?t:(void*)((_tmp1107=_cycalloc(sizeof(*_tmp1107)),((
_tmp1107[0]=((_tmp1108.tag=18,((_tmp1108.f1=_tmp865,((_tmp1108.f2=_tmp8B4,((
_tmp1108.f3=_tmp867,((_tmp1108.f4=_tmp868,_tmp1108)))))))))),_tmp1107))));}
_LL455: {struct Cyc_Absyn_ArrayType_struct*_tmp869=(struct Cyc_Absyn_ArrayType_struct*)
_tmp855;if(_tmp869->tag != 9)goto _LL457;else{_tmp86A=_tmp869->f1;_tmp86B=_tmp86A.elt_type;
_tmp86C=_tmp86A.tq;_tmp86D=_tmp86A.num_elts;_tmp86E=_tmp86A.zero_term;_tmp86F=
_tmp86A.zt_loc;}}_LL456: {void*_tmp8B7=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp86B);
struct Cyc_Absyn_ArrayType_struct _tmp110E;struct Cyc_Absyn_ArrayInfo _tmp110D;
struct Cyc_Absyn_ArrayType_struct*_tmp110C;return _tmp8B7 == _tmp86B?t:(void*)((
_tmp110C=_cycalloc(sizeof(*_tmp110C)),((_tmp110C[0]=((_tmp110E.tag=9,((_tmp110E.f1=((
_tmp110D.elt_type=_tmp8B7,((_tmp110D.tq=_tmp86C,((_tmp110D.num_elts=_tmp86D,((
_tmp110D.zero_term=_tmp86E,((_tmp110D.zt_loc=_tmp86F,_tmp110D)))))))))),_tmp110E)))),
_tmp110C))));}_LL457: {struct Cyc_Absyn_PointerType_struct*_tmp870=(struct Cyc_Absyn_PointerType_struct*)
_tmp855;if(_tmp870->tag != 5)goto _LL459;else{_tmp871=_tmp870->f1;_tmp872=_tmp871.elt_typ;
_tmp873=_tmp871.elt_tq;_tmp874=_tmp871.ptr_atts;_tmp875=_tmp874.rgn;_tmp876=
_tmp874.nullable;_tmp877=_tmp874.bounds;_tmp878=_tmp874.zero_term;}}_LL458: {
void*_tmp8BB=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp872);void*_tmp8BC=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmp875);union Cyc_Absyn_Constraint*_tmp8BD=_tmp877;{void*_tmp8BE=((void*(*)(
void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,
_tmp877);struct Cyc_Absyn_Exp*_tmp8C0;_LL485: {struct Cyc_Absyn_Upper_b_struct*
_tmp8BF=(struct Cyc_Absyn_Upper_b_struct*)_tmp8BE;if(_tmp8BF->tag != 1)goto _LL487;
else{_tmp8C0=_tmp8BF->f1;}}_LL486: {struct Cyc_Absyn_Exp*_tmp8C1=Cyc_Tcutil_rsubsexp(
rgn,inst,_tmp8C0);if(_tmp8C1 != _tmp8C0){struct Cyc_Absyn_Upper_b_struct _tmp1111;
struct Cyc_Absyn_Upper_b_struct*_tmp1110;_tmp8BD=((union Cyc_Absyn_Constraint*(*)(
void*x))Cyc_Absyn_new_conref)((void*)((_tmp1110=_cycalloc(sizeof(*_tmp1110)),((
_tmp1110[0]=((_tmp1111.tag=1,((_tmp1111.f1=_tmp8C1,_tmp1111)))),_tmp1110)))));}
goto _LL484;}_LL487:;_LL488: goto _LL484;_LL484:;}if((_tmp8BB == _tmp872  && _tmp8BC
== _tmp875) && _tmp8BD == _tmp877)return t;{struct Cyc_Absyn_PointerType_struct
_tmp111B;struct Cyc_Absyn_PtrAtts _tmp111A;struct Cyc_Absyn_PtrInfo _tmp1119;struct
Cyc_Absyn_PointerType_struct*_tmp1118;return(void*)((_tmp1118=_cycalloc(sizeof(*
_tmp1118)),((_tmp1118[0]=((_tmp111B.tag=5,((_tmp111B.f1=((_tmp1119.elt_typ=
_tmp8BB,((_tmp1119.elt_tq=_tmp873,((_tmp1119.ptr_atts=((_tmp111A.rgn=_tmp8BC,((
_tmp111A.nullable=_tmp876,((_tmp111A.bounds=_tmp8BD,((_tmp111A.zero_term=_tmp878,((
_tmp111A.ptrloc=0,_tmp111A)))))))))),_tmp1119)))))),_tmp111B)))),_tmp1118))));}}
_LL459: {struct Cyc_Absyn_FnType_struct*_tmp879=(struct Cyc_Absyn_FnType_struct*)
_tmp855;if(_tmp879->tag != 10)goto _LL45B;else{_tmp87A=_tmp879->f1;_tmp87B=_tmp87A.tvars;
_tmp87C=_tmp87A.effect;_tmp87D=_tmp87A.ret_typ;_tmp87E=_tmp87A.args;_tmp87F=
_tmp87A.c_varargs;_tmp880=_tmp87A.cyc_varargs;_tmp881=_tmp87A.rgn_po;_tmp882=
_tmp87A.attributes;}}_LL45A:{struct Cyc_List_List*_tmp8C8=_tmp87B;for(0;_tmp8C8 != 
0;_tmp8C8=_tmp8C8->tl){struct _tuple14*_tmp1125;struct Cyc_Absyn_VarType_struct
_tmp1124;struct Cyc_Absyn_VarType_struct*_tmp1123;struct Cyc_List_List*_tmp1122;
inst=((_tmp1122=_region_malloc(rgn,sizeof(*_tmp1122)),((_tmp1122->hd=((_tmp1125=
_region_malloc(rgn,sizeof(*_tmp1125)),((_tmp1125->f1=(struct Cyc_Absyn_Tvar*)
_tmp8C8->hd,((_tmp1125->f2=(void*)((_tmp1123=_cycalloc(sizeof(*_tmp1123)),((
_tmp1123[0]=((_tmp1124.tag=2,((_tmp1124.f1=(struct Cyc_Absyn_Tvar*)_tmp8C8->hd,
_tmp1124)))),_tmp1123)))),_tmp1125)))))),((_tmp1122->tl=inst,_tmp1122))))));}}{
struct Cyc_List_List*_tmp8CE;struct Cyc_List_List*_tmp8CF;struct _tuple1 _tmp8CD=((
struct _tuple1(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x))Cyc_List_rsplit)(rgn,rgn,((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct _tuple23*(*f)(struct _RegionHandle*,struct _tuple9*),struct _RegionHandle*env,
struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_substitute_f1,rgn,_tmp87E));
_tmp8CE=_tmp8CD.f1;_tmp8CF=_tmp8CD.f2;{struct Cyc_List_List*_tmp8D0=Cyc_Tcutil_substs(
rgn,inst,_tmp8CF);struct Cyc_List_List*_tmp8D1=((struct Cyc_List_List*(*)(struct
_tuple9*(*f)(struct _tuple23*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_substitute_f2,((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmp8CE,_tmp8D0));struct Cyc_Core_Opt*
eff2;if(_tmp87C == 0)eff2=0;else{void*_tmp8D2=Cyc_Tcutil_rsubstitute(rgn,inst,(
void*)_tmp87C->v);if(_tmp8D2 == (void*)_tmp87C->v)eff2=_tmp87C;else{struct Cyc_Core_Opt*
_tmp1126;eff2=((_tmp1126=_cycalloc(sizeof(*_tmp1126)),((_tmp1126->v=_tmp8D2,
_tmp1126))));}}{struct Cyc_Absyn_VarargInfo*cyc_varargs2;if(_tmp880 == 0)
cyc_varargs2=0;else{struct Cyc_Core_Opt*_tmp8D5;struct Cyc_Absyn_Tqual _tmp8D6;void*
_tmp8D7;int _tmp8D8;struct Cyc_Absyn_VarargInfo _tmp8D4=*_tmp880;_tmp8D5=_tmp8D4.name;
_tmp8D6=_tmp8D4.tq;_tmp8D7=_tmp8D4.type;_tmp8D8=_tmp8D4.inject;{void*_tmp8D9=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmp8D7);if(_tmp8D9 == _tmp8D7)cyc_varargs2=_tmp880;else{struct Cyc_Absyn_VarargInfo*
_tmp1127;cyc_varargs2=((_tmp1127=_cycalloc(sizeof(*_tmp1127)),((_tmp1127->name=
_tmp8D5,((_tmp1127->tq=_tmp8D6,((_tmp1127->type=_tmp8D9,((_tmp1127->inject=
_tmp8D8,_tmp1127))))))))));}}}{struct Cyc_List_List*rgn_po2;struct Cyc_List_List*
_tmp8DC;struct Cyc_List_List*_tmp8DD;struct _tuple1 _tmp8DB=((struct _tuple1(*)(
struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(
rgn,rgn,_tmp881);_tmp8DC=_tmp8DB.f1;_tmp8DD=_tmp8DB.f2;{struct Cyc_List_List*
_tmp8DE=Cyc_Tcutil_substs(rgn,inst,_tmp8DC);struct Cyc_List_List*_tmp8DF=Cyc_Tcutil_substs(
rgn,inst,_tmp8DD);if(_tmp8DE == _tmp8DC  && _tmp8DF == _tmp8DD)rgn_po2=_tmp881;
else{rgn_po2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_zip)(_tmp8DE,_tmp8DF);}{struct Cyc_Absyn_FnType_struct _tmp112D;struct
Cyc_Absyn_FnInfo _tmp112C;struct Cyc_Absyn_FnType_struct*_tmp112B;return(void*)((
_tmp112B=_cycalloc(sizeof(*_tmp112B)),((_tmp112B[0]=((_tmp112D.tag=10,((_tmp112D.f1=((
_tmp112C.tvars=_tmp87B,((_tmp112C.effect=eff2,((_tmp112C.ret_typ=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmp87D),((_tmp112C.args=_tmp8D1,((_tmp112C.c_varargs=_tmp87F,((
_tmp112C.cyc_varargs=cyc_varargs2,((_tmp112C.rgn_po=rgn_po2,((_tmp112C.attributes=
_tmp882,_tmp112C)))))))))))))))),_tmp112D)))),_tmp112B))));}}}}}}_LL45B: {struct
Cyc_Absyn_TupleType_struct*_tmp883=(struct Cyc_Absyn_TupleType_struct*)_tmp855;
if(_tmp883->tag != 11)goto _LL45D;else{_tmp884=_tmp883->f1;}}_LL45C: {struct Cyc_List_List*
_tmp8E3=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _tuple11*),
struct Cyc_List_List*x))Cyc_List_rmap)(rgn,Cyc_Tcutil_snd_tqt,_tmp884);struct Cyc_List_List*
_tmp8E4=Cyc_Tcutil_substs(rgn,inst,_tmp8E3);if(_tmp8E4 == _tmp8E3)return t;{struct
Cyc_List_List*_tmp8E5=((struct Cyc_List_List*(*)(struct _tuple11*(*f)(struct
_tuple11*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_map2_tq,
_tmp884,_tmp8E4);struct Cyc_Absyn_TupleType_struct _tmp1130;struct Cyc_Absyn_TupleType_struct*
_tmp112F;return(void*)((_tmp112F=_cycalloc(sizeof(*_tmp112F)),((_tmp112F[0]=((
_tmp1130.tag=11,((_tmp1130.f1=_tmp8E5,_tmp1130)))),_tmp112F))));}}_LL45D: {
struct Cyc_Absyn_AnonAggrType_struct*_tmp885=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp855;if(_tmp885->tag != 13)goto _LL45F;else{_tmp886=_tmp885->f1;_tmp887=_tmp885->f2;}}
_LL45E: {struct Cyc_List_List*_tmp8E8=((struct Cyc_List_List*(*)(struct
_RegionHandle*,void*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_rmap)(
rgn,Cyc_Tcutil_field_type,_tmp887);struct Cyc_List_List*_tmp8E9=Cyc_Tcutil_substs(
rgn,inst,_tmp8E8);if(_tmp8E9 == _tmp8E8)return t;{struct Cyc_List_List*_tmp8EA=((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*,
void*),struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_zip_field_type,
_tmp887,_tmp8E9);struct Cyc_Absyn_AnonAggrType_struct _tmp1133;struct Cyc_Absyn_AnonAggrType_struct*
_tmp1132;return(void*)((_tmp1132=_cycalloc(sizeof(*_tmp1132)),((_tmp1132[0]=((
_tmp1133.tag=13,((_tmp1133.f1=_tmp886,((_tmp1133.f2=_tmp8EA,_tmp1133)))))),
_tmp1132))));}}_LL45F: {struct Cyc_Absyn_Evar_struct*_tmp888=(struct Cyc_Absyn_Evar_struct*)
_tmp855;if(_tmp888->tag != 1)goto _LL461;else{_tmp889=_tmp888->f2;}}_LL460: if(
_tmp889 != 0)return Cyc_Tcutil_rsubstitute(rgn,inst,(void*)_tmp889->v);else{return
t;}_LL461: {struct Cyc_Absyn_RgnHandleType_struct*_tmp88A=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp855;if(_tmp88A->tag != 16)goto _LL463;else{_tmp88B=(void*)_tmp88A->f1;}}_LL462: {
void*_tmp8ED=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp88B);struct Cyc_Absyn_RgnHandleType_struct
_tmp1136;struct Cyc_Absyn_RgnHandleType_struct*_tmp1135;return _tmp8ED == _tmp88B?t:(
void*)((_tmp1135=_cycalloc(sizeof(*_tmp1135)),((_tmp1135[0]=((_tmp1136.tag=16,((
_tmp1136.f1=(void*)_tmp8ED,_tmp1136)))),_tmp1135))));}_LL463: {struct Cyc_Absyn_DynRgnType_struct*
_tmp88C=(struct Cyc_Absyn_DynRgnType_struct*)_tmp855;if(_tmp88C->tag != 17)goto
_LL465;else{_tmp88D=(void*)_tmp88C->f1;_tmp88E=(void*)_tmp88C->f2;}}_LL464: {
void*_tmp8F0=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp88D);void*_tmp8F1=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmp88E);struct Cyc_Absyn_DynRgnType_struct _tmp1139;struct Cyc_Absyn_DynRgnType_struct*
_tmp1138;return _tmp8F0 == _tmp88D  && _tmp8F1 == _tmp88E?t:(void*)((_tmp1138=
_cycalloc(sizeof(*_tmp1138)),((_tmp1138[0]=((_tmp1139.tag=17,((_tmp1139.f1=(void*)
_tmp8F0,((_tmp1139.f2=(void*)_tmp8F1,_tmp1139)))))),_tmp1138))));}_LL465: {
struct Cyc_Absyn_TagType_struct*_tmp88F=(struct Cyc_Absyn_TagType_struct*)_tmp855;
if(_tmp88F->tag != 20)goto _LL467;else{_tmp890=(void*)_tmp88F->f1;}}_LL466: {void*
_tmp8F4=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp890);struct Cyc_Absyn_TagType_struct
_tmp113C;struct Cyc_Absyn_TagType_struct*_tmp113B;return _tmp8F4 == _tmp890?t:(void*)((
_tmp113B=_cycalloc(sizeof(*_tmp113B)),((_tmp113B[0]=((_tmp113C.tag=20,((_tmp113C.f1=(
void*)_tmp8F4,_tmp113C)))),_tmp113B))));}_LL467: {struct Cyc_Absyn_ValueofType_struct*
_tmp891=(struct Cyc_Absyn_ValueofType_struct*)_tmp855;if(_tmp891->tag != 19)goto
_LL469;else{_tmp892=_tmp891->f1;}}_LL468: {struct Cyc_Absyn_Exp*_tmp8F7=Cyc_Tcutil_rsubsexp(
rgn,inst,_tmp892);struct Cyc_Absyn_ValueofType_struct _tmp113F;struct Cyc_Absyn_ValueofType_struct*
_tmp113E;return _tmp8F7 == _tmp892?t:(void*)((_tmp113E=_cycalloc(sizeof(*_tmp113E)),((
_tmp113E[0]=((_tmp113F.tag=19,((_tmp113F.f1=_tmp8F7,_tmp113F)))),_tmp113E))));}
_LL469: {struct Cyc_Absyn_EnumType_struct*_tmp893=(struct Cyc_Absyn_EnumType_struct*)
_tmp855;if(_tmp893->tag != 14)goto _LL46B;}_LL46A: goto _LL46C;_LL46B: {struct Cyc_Absyn_AnonEnumType_struct*
_tmp894=(struct Cyc_Absyn_AnonEnumType_struct*)_tmp855;if(_tmp894->tag != 15)goto
_LL46D;}_LL46C: goto _LL46E;_LL46D: {struct Cyc_Absyn_VoidType_struct*_tmp895=(
struct Cyc_Absyn_VoidType_struct*)_tmp855;if(_tmp895->tag != 0)goto _LL46F;}_LL46E:
goto _LL470;_LL46F: {struct Cyc_Absyn_IntType_struct*_tmp896=(struct Cyc_Absyn_IntType_struct*)
_tmp855;if(_tmp896->tag != 6)goto _LL471;}_LL470: goto _LL472;_LL471: {struct Cyc_Absyn_FloatType_struct*
_tmp897=(struct Cyc_Absyn_FloatType_struct*)_tmp855;if(_tmp897->tag != 7)goto
_LL473;}_LL472: goto _LL474;_LL473: {struct Cyc_Absyn_DoubleType_struct*_tmp898=(
struct Cyc_Absyn_DoubleType_struct*)_tmp855;if(_tmp898->tag != 8)goto _LL475;}
_LL474: goto _LL476;_LL475: {struct Cyc_Absyn_UniqueRgn_struct*_tmp899=(struct Cyc_Absyn_UniqueRgn_struct*)
_tmp855;if(_tmp899->tag != 22)goto _LL477;}_LL476: goto _LL478;_LL477: {struct Cyc_Absyn_HeapRgn_struct*
_tmp89A=(struct Cyc_Absyn_HeapRgn_struct*)_tmp855;if(_tmp89A->tag != 21)goto _LL479;}
_LL478: return t;_LL479: {struct Cyc_Absyn_RgnsEff_struct*_tmp89B=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp855;if(_tmp89B->tag != 25)goto _LL47B;else{_tmp89C=(void*)_tmp89B->f1;}}_LL47A: {
void*_tmp8FA=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp89C);struct Cyc_Absyn_RgnsEff_struct
_tmp1142;struct Cyc_Absyn_RgnsEff_struct*_tmp1141;return _tmp8FA == _tmp89C?t:(void*)((
_tmp1141=_cycalloc(sizeof(*_tmp1141)),((_tmp1141[0]=((_tmp1142.tag=25,((_tmp1142.f1=(
void*)_tmp8FA,_tmp1142)))),_tmp1141))));}_LL47B: {struct Cyc_Absyn_AccessEff_struct*
_tmp89D=(struct Cyc_Absyn_AccessEff_struct*)_tmp855;if(_tmp89D->tag != 23)goto
_LL47D;else{_tmp89E=(void*)_tmp89D->f1;}}_LL47C: {void*_tmp8FD=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmp89E);struct Cyc_Absyn_AccessEff_struct _tmp1145;struct Cyc_Absyn_AccessEff_struct*
_tmp1144;return _tmp8FD == _tmp89E?t:(void*)((_tmp1144=_cycalloc(sizeof(*_tmp1144)),((
_tmp1144[0]=((_tmp1145.tag=23,((_tmp1145.f1=(void*)_tmp8FD,_tmp1145)))),_tmp1144))));}
_LL47D: {struct Cyc_Absyn_JoinEff_struct*_tmp89F=(struct Cyc_Absyn_JoinEff_struct*)
_tmp855;if(_tmp89F->tag != 24)goto _LL44A;else{_tmp8A0=_tmp89F->f1;}}_LL47E: {
struct Cyc_List_List*_tmp900=Cyc_Tcutil_substs(rgn,inst,_tmp8A0);struct Cyc_Absyn_JoinEff_struct
_tmp1148;struct Cyc_Absyn_JoinEff_struct*_tmp1147;return _tmp900 == _tmp8A0?t:(void*)((
_tmp1147=_cycalloc(sizeof(*_tmp1147)),((_tmp1147[0]=((_tmp1148.tag=24,((_tmp1148.f1=
_tmp900,_tmp1148)))),_tmp1147))));}_LL44A:;}static struct Cyc_List_List*Cyc_Tcutil_substs(
struct _RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*ts);static
struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*rgn,struct Cyc_List_List*
inst,struct Cyc_List_List*ts){if(ts == 0)return 0;{void*_tmp903=(void*)ts->hd;
struct Cyc_List_List*_tmp904=ts->tl;void*_tmp905=Cyc_Tcutil_rsubstitute(rgn,inst,
_tmp903);struct Cyc_List_List*_tmp906=Cyc_Tcutil_substs(rgn,inst,_tmp904);if(
_tmp903 == _tmp905  && _tmp904 == _tmp906)return ts;{struct Cyc_List_List*_tmp1149;
return(struct Cyc_List_List*)((struct Cyc_List_List*)((_tmp1149=_cycalloc(sizeof(*
_tmp1149)),((_tmp1149->hd=_tmp905,((_tmp1149->tl=_tmp906,_tmp1149)))))));}}}void*
Cyc_Tcutil_substitute(struct Cyc_List_List*inst,void*t);extern void*Cyc_Tcutil_substitute(
struct Cyc_List_List*inst,void*t){return Cyc_Tcutil_rsubstitute(Cyc_Core_heap_region,
inst,t);}struct _tuple14*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*s,struct Cyc_Absyn_Tvar*
tv);struct _tuple14*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*s,struct Cyc_Absyn_Tvar*
tv){struct Cyc_Core_Opt*_tmp908=Cyc_Tcutil_kind_to_opt(Cyc_Tcutil_tvar_kind(tv,
Cyc_Absyn_BoxKind));struct Cyc_Core_Opt*_tmp114C;struct _tuple14*_tmp114B;return(
_tmp114B=_cycalloc(sizeof(*_tmp114B)),((_tmp114B->f1=tv,((_tmp114B->f2=Cyc_Absyn_new_evar(
_tmp908,((_tmp114C=_cycalloc(sizeof(*_tmp114C)),((_tmp114C->v=s,_tmp114C))))),
_tmp114B)))));}struct _tuple14*Cyc_Tcutil_r_make_inst_var(struct _tuple15*env,
struct Cyc_Absyn_Tvar*tv);struct _tuple14*Cyc_Tcutil_r_make_inst_var(struct
_tuple15*env,struct Cyc_Absyn_Tvar*tv){struct _tuple15 _tmp90C;struct Cyc_List_List*
_tmp90D;struct _RegionHandle*_tmp90E;struct _tuple15*_tmp90B=env;_tmp90C=*_tmp90B;
_tmp90D=_tmp90C.f1;_tmp90E=_tmp90C.f2;{struct Cyc_Core_Opt*_tmp90F=Cyc_Tcutil_kind_to_opt(
Cyc_Tcutil_tvar_kind(tv,Cyc_Absyn_BoxKind));struct Cyc_Core_Opt*_tmp114F;struct
_tuple14*_tmp114E;return(_tmp114E=_region_malloc(_tmp90E,sizeof(*_tmp114E)),((
_tmp114E->f1=tv,((_tmp114E->f2=Cyc_Absyn_new_evar(_tmp90F,((_tmp114F=_cycalloc(
sizeof(*_tmp114F)),((_tmp114F->v=_tmp90D,_tmp114F))))),_tmp114E)))));}}static
struct Cyc_List_List*Cyc_Tcutil_add_free_tvar(struct Cyc_Position_Segment*loc,
struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv);static struct Cyc_List_List*Cyc_Tcutil_add_free_tvar(
struct Cyc_Position_Segment*loc,struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){{
struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){if(Cyc_strptrcmp(((
struct Cyc_Absyn_Tvar*)tvs2->hd)->name,tv->name)== 0){void*k1=((struct Cyc_Absyn_Tvar*)
tvs2->hd)->kind;void*k2=tv->kind;if(!Cyc_Tcutil_constrain_kinds(k1,k2)){const
char*_tmp1155;void*_tmp1154[3];struct Cyc_String_pa_struct _tmp1153;struct Cyc_String_pa_struct
_tmp1152;struct Cyc_String_pa_struct _tmp1151;(_tmp1151.tag=0,((_tmp1151.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kindbound2string(k2)),((
_tmp1152.tag=0,((_tmp1152.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kindbound2string(
k1)),((_tmp1153.tag=0,((_tmp1153.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*
tv->name),((_tmp1154[0]=& _tmp1153,((_tmp1154[1]=& _tmp1152,((_tmp1154[2]=&
_tmp1151,Cyc_Tcutil_terr(loc,((_tmp1155="type variable %s is used with inconsistent kinds %s and %s",
_tag_dyneither(_tmp1155,sizeof(char),59))),_tag_dyneither(_tmp1154,sizeof(void*),
3)))))))))))))))))));}if(tv->identity == - 1)tv->identity=((struct Cyc_Absyn_Tvar*)
tvs2->hd)->identity;else{if(tv->identity != ((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity){
const char*_tmp1158;void*_tmp1157;(_tmp1157=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1158="same type variable has different identity!",
_tag_dyneither(_tmp1158,sizeof(char),43))),_tag_dyneither(_tmp1157,sizeof(void*),
0)));}}return tvs;}}}tv->identity=Cyc_Tcutil_new_tvar_id();{struct Cyc_List_List*
_tmp1159;return(_tmp1159=_cycalloc(sizeof(*_tmp1159)),((_tmp1159->hd=tv,((
_tmp1159->tl=tvs,_tmp1159)))));}}static struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar(
struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv);static struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar(
struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){if(tv->identity == - 1){const char*
_tmp115C;void*_tmp115B;(_tmp115B=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp115C="fast_add_free_tvar: bad identity in tv",
_tag_dyneither(_tmp115C,sizeof(char),39))),_tag_dyneither(_tmp115B,sizeof(void*),
0)));}{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){struct Cyc_Absyn_Tvar*
_tmp91C=(struct Cyc_Absyn_Tvar*)tvs2->hd;if(_tmp91C->identity == - 1){const char*
_tmp115F;void*_tmp115E;(_tmp115E=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp115F="fast_add_free_tvar: bad identity in tvs2",
_tag_dyneither(_tmp115F,sizeof(char),41))),_tag_dyneither(_tmp115E,sizeof(void*),
0)));}if(_tmp91C->identity == tv->identity)return tvs;}}{struct Cyc_List_List*
_tmp1160;return(_tmp1160=_cycalloc(sizeof(*_tmp1160)),((_tmp1160->hd=tv,((
_tmp1160->tl=tvs,_tmp1160)))));}}struct _tuple24{struct Cyc_Absyn_Tvar*f1;int f2;};
static struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar_bool(struct _RegionHandle*
r,struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv,int b);static struct Cyc_List_List*
Cyc_Tcutil_fast_add_free_tvar_bool(struct _RegionHandle*r,struct Cyc_List_List*tvs,
struct Cyc_Absyn_Tvar*tv,int b){if(tv->identity == - 1){const char*_tmp1163;void*
_tmp1162;(_tmp1162=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmp1163="fast_add_free_tvar_bool: bad identity in tv",
_tag_dyneither(_tmp1163,sizeof(char),44))),_tag_dyneither(_tmp1162,sizeof(void*),
0)));}{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){struct _tuple24
_tmp923;struct Cyc_Absyn_Tvar*_tmp924;int _tmp925;int*_tmp926;struct _tuple24*
_tmp922=(struct _tuple24*)tvs2->hd;_tmp923=*_tmp922;_tmp924=_tmp923.f1;_tmp925=
_tmp923.f2;_tmp926=(int*)&(*_tmp922).f2;if(_tmp924->identity == - 1){const char*
_tmp1166;void*_tmp1165;(_tmp1165=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1166="fast_add_free_tvar_bool: bad identity in tvs2",
_tag_dyneither(_tmp1166,sizeof(char),46))),_tag_dyneither(_tmp1165,sizeof(void*),
0)));}if(_tmp924->identity == tv->identity){*_tmp926=*_tmp926  || b;return tvs;}}}{
struct _tuple24*_tmp1169;struct Cyc_List_List*_tmp1168;return(_tmp1168=
_region_malloc(r,sizeof(*_tmp1168)),((_tmp1168->hd=((_tmp1169=_region_malloc(r,
sizeof(*_tmp1169)),((_tmp1169->f1=tv,((_tmp1169->f2=b,_tmp1169)))))),((_tmp1168->tl=
tvs,_tmp1168)))));}}static struct Cyc_List_List*Cyc_Tcutil_add_bound_tvar(struct
Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv);static struct Cyc_List_List*Cyc_Tcutil_add_bound_tvar(
struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){if(tv->identity == - 1){const char*
_tmp116D;void*_tmp116C[1];struct Cyc_String_pa_struct _tmp116B;(_tmp116B.tag=0,((
_tmp116B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_tvar2string(
tv)),((_tmp116C[0]=& _tmp116B,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp116D="bound tvar id for %s is NULL",
_tag_dyneither(_tmp116D,sizeof(char),29))),_tag_dyneither(_tmp116C,sizeof(void*),
1)))))));}{struct Cyc_List_List*_tmp116E;return(_tmp116E=_cycalloc(sizeof(*
_tmp116E)),((_tmp116E->hd=tv,((_tmp116E->tl=tvs,_tmp116E)))));}}struct _tuple25{
void*f1;int f2;};static struct Cyc_List_List*Cyc_Tcutil_add_free_evar(struct
_RegionHandle*r,struct Cyc_List_List*es,void*e,int b);static struct Cyc_List_List*
Cyc_Tcutil_add_free_evar(struct _RegionHandle*r,struct Cyc_List_List*es,void*e,int
b){void*_tmp92F=Cyc_Tcutil_compress(e);int _tmp931;_LL48A: {struct Cyc_Absyn_Evar_struct*
_tmp930=(struct Cyc_Absyn_Evar_struct*)_tmp92F;if(_tmp930->tag != 1)goto _LL48C;
else{_tmp931=_tmp930->f3;}}_LL48B:{struct Cyc_List_List*es2=es;for(0;es2 != 0;es2=
es2->tl){struct _tuple25 _tmp933;void*_tmp934;int _tmp935;int*_tmp936;struct
_tuple25*_tmp932=(struct _tuple25*)es2->hd;_tmp933=*_tmp932;_tmp934=_tmp933.f1;
_tmp935=_tmp933.f2;_tmp936=(int*)&(*_tmp932).f2;{void*_tmp937=Cyc_Tcutil_compress(
_tmp934);int _tmp939;_LL48F: {struct Cyc_Absyn_Evar_struct*_tmp938=(struct Cyc_Absyn_Evar_struct*)
_tmp937;if(_tmp938->tag != 1)goto _LL491;else{_tmp939=_tmp938->f3;}}_LL490: if(
_tmp931 == _tmp939){if(b != *_tmp936)*_tmp936=1;return es;}goto _LL48E;_LL491:;
_LL492: goto _LL48E;_LL48E:;}}}{struct _tuple25*_tmp1171;struct Cyc_List_List*
_tmp1170;return(_tmp1170=_region_malloc(r,sizeof(*_tmp1170)),((_tmp1170->hd=((
_tmp1171=_region_malloc(r,sizeof(*_tmp1171)),((_tmp1171->f1=e,((_tmp1171->f2=b,
_tmp1171)))))),((_tmp1170->tl=es,_tmp1170)))));}_LL48C:;_LL48D: return es;_LL489:;}
static struct Cyc_List_List*Cyc_Tcutil_remove_bound_tvars(struct _RegionHandle*rgn,
struct Cyc_List_List*tvs,struct Cyc_List_List*btvs);static struct Cyc_List_List*Cyc_Tcutil_remove_bound_tvars(
struct _RegionHandle*rgn,struct Cyc_List_List*tvs,struct Cyc_List_List*btvs){struct
Cyc_List_List*r=0;for(0;tvs != 0;tvs=tvs->tl){int present=0;{struct Cyc_List_List*b=
btvs;for(0;b != 0;b=b->tl){if(((struct Cyc_Absyn_Tvar*)tvs->hd)->identity == ((
struct Cyc_Absyn_Tvar*)b->hd)->identity){present=1;break;}}}if(!present){struct
Cyc_List_List*_tmp1172;r=((_tmp1172=_region_malloc(rgn,sizeof(*_tmp1172)),((
_tmp1172->hd=(struct Cyc_Absyn_Tvar*)tvs->hd,((_tmp1172->tl=r,_tmp1172))))));}}r=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(r);return r;}
static struct Cyc_List_List*Cyc_Tcutil_remove_bound_tvars_bool(struct _RegionHandle*
r,struct Cyc_List_List*tvs,struct Cyc_List_List*btvs);static struct Cyc_List_List*
Cyc_Tcutil_remove_bound_tvars_bool(struct _RegionHandle*r,struct Cyc_List_List*tvs,
struct Cyc_List_List*btvs){struct Cyc_List_List*res=0;for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_Absyn_Tvar*_tmp93E;int _tmp93F;struct _tuple24 _tmp93D=*((struct _tuple24*)
tvs->hd);_tmp93E=_tmp93D.f1;_tmp93F=_tmp93D.f2;{int present=0;{struct Cyc_List_List*
b=btvs;for(0;b != 0;b=b->tl){if(_tmp93E->identity == ((struct Cyc_Absyn_Tvar*)b->hd)->identity){
present=1;break;}}}if(!present){struct Cyc_List_List*_tmp1173;res=((_tmp1173=
_region_malloc(r,sizeof(*_tmp1173)),((_tmp1173->hd=(struct _tuple24*)tvs->hd,((
_tmp1173->tl=res,_tmp1173))))));}}}res=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(res);return res;}void Cyc_Tcutil_check_bitfield(struct Cyc_Position_Segment*
loc,struct Cyc_Tcenv_Tenv*te,void*field_typ,struct Cyc_Absyn_Exp*width,struct
_dyneither_ptr*fn);void Cyc_Tcutil_check_bitfield(struct Cyc_Position_Segment*loc,
struct Cyc_Tcenv_Tenv*te,void*field_typ,struct Cyc_Absyn_Exp*width,struct
_dyneither_ptr*fn){if(width != 0){unsigned int w=0;if(!Cyc_Tcutil_is_const_exp(te,(
struct Cyc_Absyn_Exp*)width)){const char*_tmp1177;void*_tmp1176[1];struct Cyc_String_pa_struct
_tmp1175;(_tmp1175.tag=0,((_tmp1175.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*fn),((_tmp1176[0]=& _tmp1175,Cyc_Tcutil_terr(loc,((_tmp1177="bitfield %s does not have constant width",
_tag_dyneither(_tmp1177,sizeof(char),41))),_tag_dyneither(_tmp1176,sizeof(void*),
1)))))));}else{unsigned int _tmp945;int _tmp946;struct _tuple13 _tmp944=Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)width);_tmp945=_tmp944.f1;_tmp946=_tmp944.f2;if(!_tmp946){
const char*_tmp117A;void*_tmp1179;(_tmp1179=0,Cyc_Tcutil_terr(loc,((_tmp117A="bitfield width cannot use sizeof or offsetof",
_tag_dyneither(_tmp117A,sizeof(char),45))),_tag_dyneither(_tmp1179,sizeof(void*),
0)));}w=_tmp945;}{void*_tmp949=Cyc_Tcutil_compress(field_typ);enum Cyc_Absyn_Size_of
_tmp94B;_LL494: {struct Cyc_Absyn_IntType_struct*_tmp94A=(struct Cyc_Absyn_IntType_struct*)
_tmp949;if(_tmp94A->tag != 6)goto _LL496;else{_tmp94B=_tmp94A->f2;}}_LL495: switch(
_tmp94B){case Cyc_Absyn_Char_sz: _LL498: if(w > 8){const char*_tmp117D;void*_tmp117C;(
_tmp117C=0,Cyc_Tcutil_terr(loc,((_tmp117D="bitfield larger than type",
_tag_dyneither(_tmp117D,sizeof(char),26))),_tag_dyneither(_tmp117C,sizeof(void*),
0)));}break;case Cyc_Absyn_Short_sz: _LL499: if(w > 16){const char*_tmp1180;void*
_tmp117F;(_tmp117F=0,Cyc_Tcutil_terr(loc,((_tmp1180="bitfield larger than type",
_tag_dyneither(_tmp1180,sizeof(char),26))),_tag_dyneither(_tmp117F,sizeof(void*),
0)));}break;case Cyc_Absyn_Long_sz: _LL49A: goto _LL49B;case Cyc_Absyn_Int_sz: _LL49B:
if(w > 32){const char*_tmp1183;void*_tmp1182;(_tmp1182=0,Cyc_Tcutil_terr(loc,((
_tmp1183="bitfield larger than type",_tag_dyneither(_tmp1183,sizeof(char),26))),
_tag_dyneither(_tmp1182,sizeof(void*),0)));}break;case Cyc_Absyn_LongLong_sz:
_LL49C: if(w > 64){const char*_tmp1186;void*_tmp1185;(_tmp1185=0,Cyc_Tcutil_terr(
loc,((_tmp1186="bitfield larger than type",_tag_dyneither(_tmp1186,sizeof(char),
26))),_tag_dyneither(_tmp1185,sizeof(void*),0)));}break;}goto _LL493;_LL496:;
_LL497:{const char*_tmp118B;void*_tmp118A[2];struct Cyc_String_pa_struct _tmp1189;
struct Cyc_String_pa_struct _tmp1188;(_tmp1188.tag=0,((_tmp1188.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(field_typ)),((
_tmp1189.tag=0,((_tmp1189.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*fn),((
_tmp118A[0]=& _tmp1189,((_tmp118A[1]=& _tmp1188,Cyc_Tcutil_terr(loc,((_tmp118B="bitfield %s must have integral type but has type %s",
_tag_dyneither(_tmp118B,sizeof(char),52))),_tag_dyneither(_tmp118A,sizeof(void*),
2)))))))))))));}goto _LL493;_LL493:;}}}static void Cyc_Tcutil_check_field_atts(
struct Cyc_Position_Segment*loc,struct _dyneither_ptr*fn,struct Cyc_List_List*atts);
static void Cyc_Tcutil_check_field_atts(struct Cyc_Position_Segment*loc,struct
_dyneither_ptr*fn,struct Cyc_List_List*atts){for(0;atts != 0;atts=atts->tl){void*
_tmp958=(void*)atts->hd;_LL49F: {struct Cyc_Absyn_Packed_att_struct*_tmp959=(
struct Cyc_Absyn_Packed_att_struct*)_tmp958;if(_tmp959->tag != 7)goto _LL4A1;}
_LL4A0: continue;_LL4A1: {struct Cyc_Absyn_Aligned_att_struct*_tmp95A=(struct Cyc_Absyn_Aligned_att_struct*)
_tmp958;if(_tmp95A->tag != 6)goto _LL4A3;}_LL4A2: continue;_LL4A3:;_LL4A4: {const
char*_tmp1190;void*_tmp118F[2];struct Cyc_String_pa_struct _tmp118E;struct Cyc_String_pa_struct
_tmp118D;(_tmp118D.tag=0,((_tmp118D.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*fn),((_tmp118E.tag=0,((_tmp118E.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absyn_attribute2string((void*)atts->hd)),((_tmp118F[0]=&
_tmp118E,((_tmp118F[1]=& _tmp118D,Cyc_Tcutil_terr(loc,((_tmp1190="bad attribute %s on member %s",
_tag_dyneither(_tmp1190,sizeof(char),30))),_tag_dyneither(_tmp118F,sizeof(void*),
2)))))))))))));}_LL49E:;}}struct Cyc_Tcutil_CVTEnv{struct _RegionHandle*r;struct
Cyc_List_List*kind_env;struct Cyc_List_List*free_vars;struct Cyc_List_List*
free_evars;int generalize_evars;int fn_result;};int Cyc_Tcutil_extract_const_from_typedef(
struct Cyc_Position_Segment*loc,int declared_const,void*t);int Cyc_Tcutil_extract_const_from_typedef(
struct Cyc_Position_Segment*loc,int declared_const,void*t){void*_tmp95F=t;struct
Cyc_Absyn_Typedefdecl*_tmp961;void**_tmp962;_LL4A6: {struct Cyc_Absyn_TypedefType_struct*
_tmp960=(struct Cyc_Absyn_TypedefType_struct*)_tmp95F;if(_tmp960->tag != 18)goto
_LL4A8;else{_tmp961=_tmp960->f3;_tmp962=_tmp960->f4;}}_LL4A7: if((((struct Cyc_Absyn_Typedefdecl*)
_check_null(_tmp961))->tq).real_const  || (_tmp961->tq).print_const){if(
declared_const){const char*_tmp1193;void*_tmp1192;(_tmp1192=0,Cyc_Tcutil_warn(loc,((
_tmp1193="extra const",_tag_dyneither(_tmp1193,sizeof(char),12))),_tag_dyneither(
_tmp1192,sizeof(void*),0)));}return 1;}if((unsigned int)_tmp962)return Cyc_Tcutil_extract_const_from_typedef(
loc,declared_const,*_tmp962);else{return declared_const;}_LL4A8:;_LL4A9: return
declared_const;_LL4A5:;}static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type_level_exp(
struct Cyc_Absyn_Exp*e,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv);
struct _tuple26{enum Cyc_Absyn_Format_Type f1;void*f2;};static struct Cyc_Tcutil_CVTEnv
Cyc_Tcutil_i_check_valid_type(struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*
te,struct Cyc_Tcutil_CVTEnv cvtenv,enum Cyc_Absyn_Kind expected_kind,void*t,int
put_in_effect);static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type(struct
Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv,
enum Cyc_Absyn_Kind expected_kind,void*t,int put_in_effect){static struct Cyc_Core_Opt
urgn={(void*)((void*)& Cyc_Absyn_UniqueRgn_val)};static struct Cyc_Core_Opt hrgn={(
void*)((void*)& Cyc_Absyn_HeapRgn_val)};{void*_tmp965=Cyc_Tcutil_compress(t);
struct Cyc_Core_Opt*_tmp968;struct Cyc_Core_Opt**_tmp969;struct Cyc_Core_Opt*
_tmp96A;struct Cyc_Core_Opt**_tmp96B;struct Cyc_Absyn_Tvar*_tmp96D;struct Cyc_List_List*
_tmp96F;struct _tuple2*_tmp971;struct Cyc_Absyn_Enumdecl*_tmp972;struct Cyc_Absyn_Enumdecl**
_tmp973;struct Cyc_Absyn_DatatypeInfo _tmp975;union Cyc_Absyn_DatatypeInfoU _tmp976;
union Cyc_Absyn_DatatypeInfoU*_tmp977;struct Cyc_List_List*_tmp978;struct Cyc_List_List**
_tmp979;struct Cyc_Absyn_DatatypeFieldInfo _tmp97B;union Cyc_Absyn_DatatypeFieldInfoU
_tmp97C;union Cyc_Absyn_DatatypeFieldInfoU*_tmp97D;struct Cyc_List_List*_tmp97E;
struct Cyc_Absyn_PtrInfo _tmp980;void*_tmp981;struct Cyc_Absyn_Tqual _tmp982;struct
Cyc_Absyn_Tqual*_tmp983;struct Cyc_Absyn_PtrAtts _tmp984;void*_tmp985;union Cyc_Absyn_Constraint*
_tmp986;union Cyc_Absyn_Constraint*_tmp987;union Cyc_Absyn_Constraint*_tmp988;void*
_tmp98A;struct Cyc_Absyn_Exp*_tmp98C;struct Cyc_Absyn_ArrayInfo _tmp991;void*
_tmp992;struct Cyc_Absyn_Tqual _tmp993;struct Cyc_Absyn_Tqual*_tmp994;struct Cyc_Absyn_Exp*
_tmp995;struct Cyc_Absyn_Exp**_tmp996;union Cyc_Absyn_Constraint*_tmp997;struct Cyc_Position_Segment*
_tmp998;struct Cyc_Absyn_FnInfo _tmp99A;struct Cyc_List_List*_tmp99B;struct Cyc_List_List**
_tmp99C;struct Cyc_Core_Opt*_tmp99D;struct Cyc_Core_Opt**_tmp99E;void*_tmp99F;
struct Cyc_List_List*_tmp9A0;int _tmp9A1;struct Cyc_Absyn_VarargInfo*_tmp9A2;struct
Cyc_List_List*_tmp9A3;struct Cyc_List_List*_tmp9A4;struct Cyc_List_List*_tmp9A6;
enum Cyc_Absyn_AggrKind _tmp9A8;struct Cyc_List_List*_tmp9A9;struct Cyc_Absyn_AggrInfo
_tmp9AB;union Cyc_Absyn_AggrInfoU _tmp9AC;union Cyc_Absyn_AggrInfoU*_tmp9AD;struct
Cyc_List_List*_tmp9AE;struct Cyc_List_List**_tmp9AF;struct _tuple2*_tmp9B1;struct
Cyc_List_List*_tmp9B2;struct Cyc_List_List**_tmp9B3;struct Cyc_Absyn_Typedefdecl*
_tmp9B4;struct Cyc_Absyn_Typedefdecl**_tmp9B5;void**_tmp9B6;void***_tmp9B7;void*
_tmp9BB;void*_tmp9BD;void*_tmp9BE;void*_tmp9C0;void*_tmp9C2;struct Cyc_List_List*
_tmp9C4;_LL4AB: {struct Cyc_Absyn_VoidType_struct*_tmp966=(struct Cyc_Absyn_VoidType_struct*)
_tmp965;if(_tmp966->tag != 0)goto _LL4AD;}_LL4AC: goto _LL4AA;_LL4AD: {struct Cyc_Absyn_Evar_struct*
_tmp967=(struct Cyc_Absyn_Evar_struct*)_tmp965;if(_tmp967->tag != 1)goto _LL4AF;
else{_tmp968=_tmp967->f1;_tmp969=(struct Cyc_Core_Opt**)& _tmp967->f1;_tmp96A=
_tmp967->f2;_tmp96B=(struct Cyc_Core_Opt**)& _tmp967->f2;}}_LL4AE: if(*_tmp969 == 0)*
_tmp969=Cyc_Tcutil_kind_to_opt(expected_kind);if((cvtenv.fn_result  && cvtenv.generalize_evars)
 && Cyc_Tcutil_is_region_kind(expected_kind)){if(expected_kind == Cyc_Absyn_UniqueRgnKind)*
_tmp96B=(struct Cyc_Core_Opt*)& urgn;else{*_tmp96B=(struct Cyc_Core_Opt*)& hrgn;}}
else{if(cvtenv.generalize_evars){struct Cyc_Absyn_Less_kb_struct _tmp1196;struct
Cyc_Absyn_Less_kb_struct*_tmp1195;struct Cyc_Absyn_Tvar*_tmp9C5=Cyc_Tcutil_new_tvar((
void*)((_tmp1195=_cycalloc(sizeof(*_tmp1195)),((_tmp1195[0]=((_tmp1196.tag=2,((
_tmp1196.f1=0,((_tmp1196.f2=expected_kind,_tmp1196)))))),_tmp1195)))));{struct
Cyc_Absyn_VarType_struct*_tmp119C;struct Cyc_Absyn_VarType_struct _tmp119B;struct
Cyc_Core_Opt*_tmp119A;*_tmp96B=((_tmp119A=_cycalloc(sizeof(*_tmp119A)),((
_tmp119A->v=(void*)((_tmp119C=_cycalloc(sizeof(*_tmp119C)),((_tmp119C[0]=((
_tmp119B.tag=2,((_tmp119B.f1=_tmp9C5,_tmp119B)))),_tmp119C)))),_tmp119A))));}
_tmp96D=_tmp9C5;goto _LL4B0;}else{cvtenv.free_evars=Cyc_Tcutil_add_free_evar(
cvtenv.r,cvtenv.free_evars,t,put_in_effect);}}goto _LL4AA;_LL4AF: {struct Cyc_Absyn_VarType_struct*
_tmp96C=(struct Cyc_Absyn_VarType_struct*)_tmp965;if(_tmp96C->tag != 2)goto _LL4B1;
else{_tmp96D=_tmp96C->f1;}}_LL4B0:{void*_tmp9CB=Cyc_Absyn_compress_kb(_tmp96D->kind);
struct Cyc_Core_Opt*_tmp9CD;struct Cyc_Core_Opt**_tmp9CE;_LL4E0: {struct Cyc_Absyn_Unknown_kb_struct*
_tmp9CC=(struct Cyc_Absyn_Unknown_kb_struct*)_tmp9CB;if(_tmp9CC->tag != 1)goto
_LL4E2;else{_tmp9CD=_tmp9CC->f1;_tmp9CE=(struct Cyc_Core_Opt**)& _tmp9CC->f1;}}
_LL4E1:{struct Cyc_Absyn_Less_kb_struct*_tmp11A2;struct Cyc_Absyn_Less_kb_struct
_tmp11A1;struct Cyc_Core_Opt*_tmp11A0;*_tmp9CE=((_tmp11A0=_cycalloc(sizeof(*
_tmp11A0)),((_tmp11A0->v=(void*)((_tmp11A2=_cycalloc(sizeof(*_tmp11A2)),((
_tmp11A2[0]=((_tmp11A1.tag=2,((_tmp11A1.f1=0,((_tmp11A1.f2=expected_kind,
_tmp11A1)))))),_tmp11A2)))),_tmp11A0))));}goto _LL4DF;_LL4E2:;_LL4E3: goto _LL4DF;
_LL4DF:;}cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmp96D);
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,
_tmp96D,put_in_effect);{void*_tmp9D2=Cyc_Absyn_compress_kb(_tmp96D->kind);struct
Cyc_Core_Opt*_tmp9D4;struct Cyc_Core_Opt**_tmp9D5;enum Cyc_Absyn_Kind _tmp9D6;
_LL4E5: {struct Cyc_Absyn_Less_kb_struct*_tmp9D3=(struct Cyc_Absyn_Less_kb_struct*)
_tmp9D2;if(_tmp9D3->tag != 2)goto _LL4E7;else{_tmp9D4=_tmp9D3->f1;_tmp9D5=(struct
Cyc_Core_Opt**)& _tmp9D3->f1;_tmp9D6=_tmp9D3->f2;}}_LL4E6: if(Cyc_Tcutil_kind_leq(
expected_kind,_tmp9D6)){struct Cyc_Absyn_Less_kb_struct*_tmp11A8;struct Cyc_Absyn_Less_kb_struct
_tmp11A7;struct Cyc_Core_Opt*_tmp11A6;*_tmp9D5=((_tmp11A6=_cycalloc(sizeof(*
_tmp11A6)),((_tmp11A6->v=(void*)((_tmp11A8=_cycalloc(sizeof(*_tmp11A8)),((
_tmp11A8[0]=((_tmp11A7.tag=2,((_tmp11A7.f1=0,((_tmp11A7.f2=expected_kind,
_tmp11A7)))))),_tmp11A8)))),_tmp11A6))));}goto _LL4E4;_LL4E7:;_LL4E8: goto _LL4E4;
_LL4E4:;}goto _LL4AA;_LL4B1: {struct Cyc_Absyn_AnonEnumType_struct*_tmp96E=(struct
Cyc_Absyn_AnonEnumType_struct*)_tmp965;if(_tmp96E->tag != 15)goto _LL4B3;else{
_tmp96F=_tmp96E->f1;}}_LL4B2: {struct Cyc_Tcenv_Genv*ge=((struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct _RegionHandle*_tmp9DA=Cyc_Tcenv_get_fnrgn(te);{struct Cyc_List_List*
prev_fields=0;unsigned int tag_count=0;for(0;_tmp96F != 0;_tmp96F=_tmp96F->tl){
struct Cyc_Absyn_Enumfield*_tmp9DB=(struct Cyc_Absyn_Enumfield*)_tmp96F->hd;if(((
int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*
l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,(*_tmp9DB->name).f2)){
const char*_tmp11AC;void*_tmp11AB[1];struct Cyc_String_pa_struct _tmp11AA;(_tmp11AA.tag=
0,((_tmp11AA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp9DB->name).f2),((
_tmp11AB[0]=& _tmp11AA,Cyc_Tcutil_terr(_tmp9DB->loc,((_tmp11AC="duplicate enum field name %s",
_tag_dyneither(_tmp11AC,sizeof(char),29))),_tag_dyneither(_tmp11AB,sizeof(void*),
1)))))));}else{struct Cyc_List_List*_tmp11AD;prev_fields=((_tmp11AD=
_region_malloc(_tmp9DA,sizeof(*_tmp11AD)),((_tmp11AD->hd=(*_tmp9DB->name).f2,((
_tmp11AD->tl=prev_fields,_tmp11AD))))));}if(_tmp9DB->tag == 0)_tmp9DB->tag=(
struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(tag_count,_tmp9DB->loc);else{if(!Cyc_Tcutil_is_const_exp(
te,(struct Cyc_Absyn_Exp*)_check_null(_tmp9DB->tag))){const char*_tmp11B1;void*
_tmp11B0[1];struct Cyc_String_pa_struct _tmp11AF;(_tmp11AF.tag=0,((_tmp11AF.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp9DB->name).f2),((_tmp11B0[0]=&
_tmp11AF,Cyc_Tcutil_terr(loc,((_tmp11B1="enum field %s: expression is not constant",
_tag_dyneither(_tmp11B1,sizeof(char),42))),_tag_dyneither(_tmp11B0,sizeof(void*),
1)))))));}}{unsigned int t1=(Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)
_check_null(_tmp9DB->tag))).f1;tag_count=t1 + 1;(*_tmp9DB->name).f1=Cyc_Absyn_Abs_n(
te->ns);{struct Cyc_Tcenv_AnonEnumRes_struct*_tmp11B7;struct Cyc_Tcenv_AnonEnumRes_struct
_tmp11B6;struct _tuple25*_tmp11B5;ge->ordinaries=((struct Cyc_Dict_Dict(*)(struct
Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct _tuple25*v))Cyc_Dict_insert)(ge->ordinaries,(*
_tmp9DB->name).f2,(struct _tuple25*)((_tmp11B5=_cycalloc(sizeof(*_tmp11B5)),((
_tmp11B5->f1=(void*)((_tmp11B7=_cycalloc(sizeof(*_tmp11B7)),((_tmp11B7[0]=((
_tmp11B6.tag=4,((_tmp11B6.f1=(void*)t,((_tmp11B6.f2=_tmp9DB,_tmp11B6)))))),
_tmp11B7)))),((_tmp11B5->f2=1,_tmp11B5)))))));}}}}goto _LL4AA;}_LL4B3: {struct Cyc_Absyn_EnumType_struct*
_tmp970=(struct Cyc_Absyn_EnumType_struct*)_tmp965;if(_tmp970->tag != 14)goto
_LL4B5;else{_tmp971=_tmp970->f1;_tmp972=_tmp970->f2;_tmp973=(struct Cyc_Absyn_Enumdecl**)&
_tmp970->f2;}}_LL4B4: if(*_tmp973 == 0  || ((struct Cyc_Absyn_Enumdecl*)_check_null(*
_tmp973))->fields == 0){struct _handler_cons _tmp9E6;_push_handler(& _tmp9E6);{int
_tmp9E8=0;if(setjmp(_tmp9E6.handler))_tmp9E8=1;if(!_tmp9E8){{struct Cyc_Absyn_Enumdecl**
ed=Cyc_Tcenv_lookup_enumdecl(te,loc,_tmp971);*_tmp973=(struct Cyc_Absyn_Enumdecl*)*
ed;};_pop_handler();}else{void*_tmp9E7=(void*)_exn_thrown;void*_tmp9EA=_tmp9E7;
_LL4EA: {struct Cyc_Dict_Absent_struct*_tmp9EB=(struct Cyc_Dict_Absent_struct*)
_tmp9EA;if(_tmp9EB->tag != Cyc_Dict_Absent)goto _LL4EC;}_LL4EB: {struct Cyc_Tcenv_Genv*
_tmp9EC=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))
Cyc_Dict_lookup)(te->ae,te->ns);struct Cyc_Absyn_Enumdecl*_tmp11B8;struct Cyc_Absyn_Enumdecl*
_tmp9ED=(_tmp11B8=_cycalloc(sizeof(*_tmp11B8)),((_tmp11B8->sc=Cyc_Absyn_Extern,((
_tmp11B8->name=_tmp971,((_tmp11B8->fields=0,_tmp11B8)))))));Cyc_Tc_tcEnumdecl(te,
_tmp9EC,loc,_tmp9ED);{struct Cyc_Absyn_Enumdecl**ed=Cyc_Tcenv_lookup_enumdecl(te,
loc,_tmp971);*_tmp973=(struct Cyc_Absyn_Enumdecl*)*ed;goto _LL4E9;}}_LL4EC:;_LL4ED:(
void)_throw(_tmp9EA);_LL4E9:;}}}{struct Cyc_Absyn_Enumdecl*ed=(struct Cyc_Absyn_Enumdecl*)
_check_null(*_tmp973);*_tmp971=(ed->name)[_check_known_subscript_notnull(1,0)];
goto _LL4AA;}_LL4B5: {struct Cyc_Absyn_DatatypeType_struct*_tmp974=(struct Cyc_Absyn_DatatypeType_struct*)
_tmp965;if(_tmp974->tag != 3)goto _LL4B7;else{_tmp975=_tmp974->f1;_tmp976=_tmp975.datatype_info;
_tmp977=(union Cyc_Absyn_DatatypeInfoU*)&(_tmp974->f1).datatype_info;_tmp978=
_tmp975.targs;_tmp979=(struct Cyc_List_List**)&(_tmp974->f1).targs;}}_LL4B6: {
struct Cyc_List_List*_tmp9EF=*_tmp979;{union Cyc_Absyn_DatatypeInfoU _tmp9F0=*
_tmp977;struct Cyc_Absyn_UnknownDatatypeInfo _tmp9F1;struct _tuple2*_tmp9F2;int
_tmp9F3;struct Cyc_Absyn_Datatypedecl**_tmp9F4;struct Cyc_Absyn_Datatypedecl*
_tmp9F5;_LL4EF: if((_tmp9F0.UnknownDatatype).tag != 1)goto _LL4F1;_tmp9F1=(struct
Cyc_Absyn_UnknownDatatypeInfo)(_tmp9F0.UnknownDatatype).val;_tmp9F2=_tmp9F1.name;
_tmp9F3=_tmp9F1.is_extensible;_LL4F0: {struct Cyc_Absyn_Datatypedecl**tudp;{
struct _handler_cons _tmp9F6;_push_handler(& _tmp9F6);{int _tmp9F8=0;if(setjmp(
_tmp9F6.handler))_tmp9F8=1;if(!_tmp9F8){tudp=Cyc_Tcenv_lookup_datatypedecl(te,
loc,_tmp9F2);;_pop_handler();}else{void*_tmp9F7=(void*)_exn_thrown;void*_tmp9FA=
_tmp9F7;_LL4F4: {struct Cyc_Dict_Absent_struct*_tmp9FB=(struct Cyc_Dict_Absent_struct*)
_tmp9FA;if(_tmp9FB->tag != Cyc_Dict_Absent)goto _LL4F6;}_LL4F5: {struct Cyc_Tcenv_Genv*
_tmp9FC=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))
Cyc_Dict_lookup)(te->ae,te->ns);struct Cyc_Absyn_Datatypedecl*_tmp11B9;struct Cyc_Absyn_Datatypedecl*
_tmp9FD=(_tmp11B9=_cycalloc(sizeof(*_tmp11B9)),((_tmp11B9->sc=Cyc_Absyn_Extern,((
_tmp11B9->name=_tmp9F2,((_tmp11B9->tvs=0,((_tmp11B9->fields=0,((_tmp11B9->is_extensible=
_tmp9F3,_tmp11B9)))))))))));Cyc_Tc_tcDatatypedecl(te,_tmp9FC,loc,_tmp9FD);tudp=
Cyc_Tcenv_lookup_datatypedecl(te,loc,_tmp9F2);if(_tmp9EF != 0){{const char*
_tmp11BD;void*_tmp11BC[1];struct Cyc_String_pa_struct _tmp11BB;(_tmp11BB.tag=0,((
_tmp11BB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp9F2)),((_tmp11BC[0]=& _tmp11BB,Cyc_Tcutil_terr(loc,((_tmp11BD="declare parameterized datatype %s before using",
_tag_dyneither(_tmp11BD,sizeof(char),47))),_tag_dyneither(_tmp11BC,sizeof(void*),
1)))))));}return cvtenv;}goto _LL4F3;}_LL4F6:;_LL4F7:(void)_throw(_tmp9FA);_LL4F3:;}}}
if(_tmp9F3  && !(*tudp)->is_extensible){const char*_tmp11C1;void*_tmp11C0[1];
struct Cyc_String_pa_struct _tmp11BF;(_tmp11BF.tag=0,((_tmp11BF.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp9F2)),((
_tmp11C0[0]=& _tmp11BF,Cyc_Tcutil_terr(loc,((_tmp11C1="datatype %s was not declared @extensible",
_tag_dyneither(_tmp11C1,sizeof(char),41))),_tag_dyneither(_tmp11C0,sizeof(void*),
1)))))));}*_tmp977=Cyc_Absyn_KnownDatatype(tudp);_tmp9F5=*tudp;goto _LL4F2;}
_LL4F1: if((_tmp9F0.KnownDatatype).tag != 2)goto _LL4EE;_tmp9F4=(struct Cyc_Absyn_Datatypedecl**)(
_tmp9F0.KnownDatatype).val;_tmp9F5=*_tmp9F4;_LL4F2: {struct Cyc_List_List*tvs=
_tmp9F5->tvs;for(0;_tmp9EF != 0  && tvs != 0;(_tmp9EF=_tmp9EF->tl,tvs=tvs->tl)){
void*t=(void*)_tmp9EF->hd;struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)tvs->hd;{
struct _tuple0 _tmp11C2;struct _tuple0 _tmpA06=(_tmp11C2.f1=Cyc_Absyn_compress_kb(tv->kind),((
_tmp11C2.f2=t,_tmp11C2)));void*_tmpA07;void*_tmpA09;struct Cyc_Absyn_Tvar*_tmpA0B;
_LL4F9: _tmpA07=_tmpA06.f1;{struct Cyc_Absyn_Unknown_kb_struct*_tmpA08=(struct Cyc_Absyn_Unknown_kb_struct*)
_tmpA07;if(_tmpA08->tag != 1)goto _LL4FB;}_tmpA09=_tmpA06.f2;{struct Cyc_Absyn_VarType_struct*
_tmpA0A=(struct Cyc_Absyn_VarType_struct*)_tmpA09;if(_tmpA0A->tag != 2)goto _LL4FB;
else{_tmpA0B=_tmpA0A->f1;}}_LL4FA: cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(loc,
cvtenv.kind_env,_tmpA0B);cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(
cvtenv.r,cvtenv.free_vars,_tmpA0B,1);continue;_LL4FB:;_LL4FC: goto _LL4F8;_LL4F8:;}{
enum Cyc_Absyn_Kind k=Cyc_Tcutil_tvar_kind(tv,Cyc_Absyn_BoxKind);cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,k,t,1);}}if(_tmp9EF != 0){const char*_tmp11C6;void*_tmp11C5[1];
struct Cyc_String_pa_struct _tmp11C4;(_tmp11C4.tag=0,((_tmp11C4.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp9F5->name)),((
_tmp11C5[0]=& _tmp11C4,Cyc_Tcutil_terr(loc,((_tmp11C6="too many type arguments for datatype %s",
_tag_dyneither(_tmp11C6,sizeof(char),40))),_tag_dyneither(_tmp11C5,sizeof(void*),
1)))))));}if(tvs != 0){struct Cyc_List_List*hidden_ts=0;for(0;tvs != 0;tvs=tvs->tl){
enum Cyc_Absyn_Kind k1=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd,Cyc_Absyn_BoxKind);
void*e=Cyc_Absyn_new_evar(0,0);{struct Cyc_List_List*_tmp11C7;hidden_ts=((
_tmp11C7=_cycalloc(sizeof(*_tmp11C7)),((_tmp11C7->hd=e,((_tmp11C7->tl=hidden_ts,
_tmp11C7))))));}cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k1,e,1);}*
_tmp979=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_imp_append)(*_tmp979,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(hidden_ts));}goto _LL4EE;}_LL4EE:;}goto _LL4AA;}_LL4B7: {struct
Cyc_Absyn_DatatypeFieldType_struct*_tmp97A=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp965;if(_tmp97A->tag != 4)goto _LL4B9;else{_tmp97B=_tmp97A->f1;_tmp97C=_tmp97B.field_info;
_tmp97D=(union Cyc_Absyn_DatatypeFieldInfoU*)&(_tmp97A->f1).field_info;_tmp97E=
_tmp97B.targs;}}_LL4B8:{union Cyc_Absyn_DatatypeFieldInfoU _tmpA10=*_tmp97D;struct
Cyc_Absyn_UnknownDatatypeFieldInfo _tmpA11;struct _tuple2*_tmpA12;struct _tuple2*
_tmpA13;int _tmpA14;struct _tuple3 _tmpA15;struct Cyc_Absyn_Datatypedecl*_tmpA16;
struct Cyc_Absyn_Datatypefield*_tmpA17;_LL4FE: if((_tmpA10.UnknownDatatypefield).tag
!= 1)goto _LL500;_tmpA11=(struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmpA10.UnknownDatatypefield).val;
_tmpA12=_tmpA11.datatype_name;_tmpA13=_tmpA11.field_name;_tmpA14=_tmpA11.is_extensible;
_LL4FF: {struct Cyc_Absyn_Datatypedecl*tud;struct Cyc_Absyn_Datatypefield*tuf;{
struct _handler_cons _tmpA18;_push_handler(& _tmpA18);{int _tmpA1A=0;if(setjmp(
_tmpA18.handler))_tmpA1A=1;if(!_tmpA1A){*Cyc_Tcenv_lookup_datatypedecl(te,loc,
_tmpA12);;_pop_handler();}else{void*_tmpA19=(void*)_exn_thrown;void*_tmpA1C=
_tmpA19;_LL503: {struct Cyc_Dict_Absent_struct*_tmpA1D=(struct Cyc_Dict_Absent_struct*)
_tmpA1C;if(_tmpA1D->tag != Cyc_Dict_Absent)goto _LL505;}_LL504:{const char*_tmp11CB;
void*_tmp11CA[1];struct Cyc_String_pa_struct _tmp11C9;(_tmp11C9.tag=0,((_tmp11C9.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpA12)),((
_tmp11CA[0]=& _tmp11C9,Cyc_Tcutil_terr(loc,((_tmp11CB="unbound datatype %s",
_tag_dyneither(_tmp11CB,sizeof(char),20))),_tag_dyneither(_tmp11CA,sizeof(void*),
1)))))));}return cvtenv;_LL505:;_LL506:(void)_throw(_tmpA1C);_LL502:;}}}{struct
_handler_cons _tmpA21;_push_handler(& _tmpA21);{int _tmpA23=0;if(setjmp(_tmpA21.handler))
_tmpA23=1;if(!_tmpA23){{struct _RegionHandle*_tmpA24=Cyc_Tcenv_get_fnrgn(te);void*
_tmpA25=Cyc_Tcenv_lookup_ordinary(_tmpA24,te,loc,_tmpA13);struct Cyc_Absyn_Datatypedecl*
_tmpA27;struct Cyc_Absyn_Datatypefield*_tmpA28;_LL508: {struct Cyc_Tcenv_DatatypeRes_struct*
_tmpA26=(struct Cyc_Tcenv_DatatypeRes_struct*)_tmpA25;if(_tmpA26->tag != 2)goto
_LL50A;else{_tmpA27=_tmpA26->f1;_tmpA28=_tmpA26->f2;}}_LL509: tuf=_tmpA28;tud=
_tmpA27;if(_tmpA14  && !tud->is_extensible){const char*_tmp11CF;void*_tmp11CE[1];
struct Cyc_String_pa_struct _tmp11CD;(_tmp11CD.tag=0,((_tmp11CD.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpA12)),((
_tmp11CE[0]=& _tmp11CD,Cyc_Tcutil_terr(loc,((_tmp11CF="datatype %s was not declared @extensible",
_tag_dyneither(_tmp11CF,sizeof(char),41))),_tag_dyneither(_tmp11CE,sizeof(void*),
1)))))));}goto _LL507;_LL50A:;_LL50B:{const char*_tmp11D4;void*_tmp11D3[2];struct
Cyc_String_pa_struct _tmp11D2;struct Cyc_String_pa_struct _tmp11D1;(_tmp11D1.tag=0,((
_tmp11D1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpA12)),((_tmp11D2.tag=0,((_tmp11D2.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpA13)),((_tmp11D3[0]=& _tmp11D2,((
_tmp11D3[1]=& _tmp11D1,Cyc_Tcutil_terr(loc,((_tmp11D4="unbound field %s in type datatype %s",
_tag_dyneither(_tmp11D4,sizeof(char),37))),_tag_dyneither(_tmp11D3,sizeof(void*),
2)))))))))))));}{struct Cyc_Tcutil_CVTEnv _tmpA30=cvtenv;_npop_handler(0);return
_tmpA30;}_LL507:;};_pop_handler();}else{void*_tmpA22=(void*)_exn_thrown;void*
_tmpA32=_tmpA22;_LL50D: {struct Cyc_Dict_Absent_struct*_tmpA33=(struct Cyc_Dict_Absent_struct*)
_tmpA32;if(_tmpA33->tag != Cyc_Dict_Absent)goto _LL50F;}_LL50E:{const char*_tmp11D9;
void*_tmp11D8[2];struct Cyc_String_pa_struct _tmp11D7;struct Cyc_String_pa_struct
_tmp11D6;(_tmp11D6.tag=0,((_tmp11D6.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpA12)),((_tmp11D7.tag=0,((_tmp11D7.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpA13)),((
_tmp11D8[0]=& _tmp11D7,((_tmp11D8[1]=& _tmp11D6,Cyc_Tcutil_terr(loc,((_tmp11D9="unbound field %s in type datatype %s",
_tag_dyneither(_tmp11D9,sizeof(char),37))),_tag_dyneither(_tmp11D8,sizeof(void*),
2)))))))))))));}return cvtenv;_LL50F:;_LL510:(void)_throw(_tmpA32);_LL50C:;}}}*
_tmp97D=Cyc_Absyn_KnownDatatypefield(tud,tuf);_tmpA16=tud;_tmpA17=tuf;goto _LL501;}
_LL500: if((_tmpA10.KnownDatatypefield).tag != 2)goto _LL4FD;_tmpA15=(struct _tuple3)(
_tmpA10.KnownDatatypefield).val;_tmpA16=_tmpA15.f1;_tmpA17=_tmpA15.f2;_LL501: {
struct Cyc_List_List*tvs=_tmpA16->tvs;for(0;_tmp97E != 0  && tvs != 0;(_tmp97E=
_tmp97E->tl,tvs=tvs->tl)){void*t=(void*)_tmp97E->hd;struct Cyc_Absyn_Tvar*tv=(
struct Cyc_Absyn_Tvar*)tvs->hd;{struct _tuple0 _tmp11DA;struct _tuple0 _tmpA39=(
_tmp11DA.f1=Cyc_Absyn_compress_kb(tv->kind),((_tmp11DA.f2=t,_tmp11DA)));void*
_tmpA3A;void*_tmpA3C;struct Cyc_Absyn_Tvar*_tmpA3E;_LL512: _tmpA3A=_tmpA39.f1;{
struct Cyc_Absyn_Unknown_kb_struct*_tmpA3B=(struct Cyc_Absyn_Unknown_kb_struct*)
_tmpA3A;if(_tmpA3B->tag != 1)goto _LL514;}_tmpA3C=_tmpA39.f2;{struct Cyc_Absyn_VarType_struct*
_tmpA3D=(struct Cyc_Absyn_VarType_struct*)_tmpA3C;if(_tmpA3D->tag != 2)goto _LL514;
else{_tmpA3E=_tmpA3D->f1;}}_LL513: cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(loc,
cvtenv.kind_env,_tmpA3E);cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(
cvtenv.r,cvtenv.free_vars,_tmpA3E,1);continue;_LL514:;_LL515: goto _LL511;_LL511:;}{
enum Cyc_Absyn_Kind k=Cyc_Tcutil_tvar_kind(tv,Cyc_Absyn_BoxKind);cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,k,t,1);}}if(_tmp97E != 0){const char*_tmp11DF;void*_tmp11DE[2];
struct Cyc_String_pa_struct _tmp11DD;struct Cyc_String_pa_struct _tmp11DC;(_tmp11DC.tag=
0,((_tmp11DC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpA17->name)),((_tmp11DD.tag=0,((_tmp11DD.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpA16->name)),((_tmp11DE[0]=& _tmp11DD,((
_tmp11DE[1]=& _tmp11DC,Cyc_Tcutil_terr(loc,((_tmp11DF="too many type arguments for datatype %s.%s",
_tag_dyneither(_tmp11DF,sizeof(char),43))),_tag_dyneither(_tmp11DE,sizeof(void*),
2)))))))))))));}if(tvs != 0){const char*_tmp11E4;void*_tmp11E3[2];struct Cyc_String_pa_struct
_tmp11E2;struct Cyc_String_pa_struct _tmp11E1;(_tmp11E1.tag=0,((_tmp11E1.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpA17->name)),((
_tmp11E2.tag=0,((_tmp11E2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpA16->name)),((_tmp11E3[0]=& _tmp11E2,((_tmp11E3[1]=& _tmp11E1,Cyc_Tcutil_terr(
loc,((_tmp11E4="too few type arguments for datatype %s.%s",_tag_dyneither(
_tmp11E4,sizeof(char),42))),_tag_dyneither(_tmp11E3,sizeof(void*),2)))))))))))));}
goto _LL4FD;}_LL4FD:;}goto _LL4AA;_LL4B9: {struct Cyc_Absyn_PointerType_struct*
_tmp97F=(struct Cyc_Absyn_PointerType_struct*)_tmp965;if(_tmp97F->tag != 5)goto
_LL4BB;else{_tmp980=_tmp97F->f1;_tmp981=_tmp980.elt_typ;_tmp982=_tmp980.elt_tq;
_tmp983=(struct Cyc_Absyn_Tqual*)&(_tmp97F->f1).elt_tq;_tmp984=_tmp980.ptr_atts;
_tmp985=_tmp984.rgn;_tmp986=_tmp984.nullable;_tmp987=_tmp984.bounds;_tmp988=
_tmp984.zero_term;}}_LL4BA: {int is_zero_terminated;cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,Cyc_Absyn_AnyKind,_tmp981,1);_tmp983->real_const=Cyc_Tcutil_extract_const_from_typedef(
loc,_tmp983->print_const,_tmp981);cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,
cvtenv,Cyc_Absyn_TopRgnKind,_tmp985,1);{union Cyc_Absyn_Constraint*_tmpA47=((
union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)(
_tmp988);union Cyc_Absyn_Constraint _tmpA48;int _tmpA49;union Cyc_Absyn_Constraint
_tmpA4A;int _tmpA4B;_LL517: _tmpA48=*_tmpA47;if((_tmpA48.No_constr).tag != 3)goto
_LL519;_tmpA49=(int)(_tmpA48.No_constr).val;_LL518:{void*_tmpA4C=Cyc_Tcutil_compress(
_tmp981);enum Cyc_Absyn_Size_of _tmpA4E;_LL51E: {struct Cyc_Absyn_IntType_struct*
_tmpA4D=(struct Cyc_Absyn_IntType_struct*)_tmpA4C;if(_tmpA4D->tag != 6)goto _LL520;
else{_tmpA4E=_tmpA4D->f2;if(_tmpA4E != Cyc_Absyn_Char_sz)goto _LL520;}}_LL51F:((
int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*
y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp988,Cyc_Absyn_true_conref);
is_zero_terminated=1;goto _LL51D;_LL520:;_LL521:((int(*)(int(*cmp)(int,int),union
Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(
Cyc_Core_intcmp,_tmp988,Cyc_Absyn_false_conref);is_zero_terminated=0;goto _LL51D;
_LL51D:;}goto _LL516;_LL519: _tmpA4A=*_tmpA47;if((_tmpA4A.Eq_constr).tag != 1)goto
_LL51B;_tmpA4B=(int)(_tmpA4A.Eq_constr).val;if(_tmpA4B != 1)goto _LL51B;_LL51A: if(
!Cyc_Tcutil_admits_zero(_tmp981)){const char*_tmp11E8;void*_tmp11E7[1];struct Cyc_String_pa_struct
_tmp11E6;(_tmp11E6.tag=0,((_tmp11E6.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(_tmp981)),((_tmp11E7[0]=& _tmp11E6,Cyc_Tcutil_terr(
loc,((_tmp11E8="cannot have a pointer to zero-terminated %s elements",
_tag_dyneither(_tmp11E8,sizeof(char),53))),_tag_dyneither(_tmp11E7,sizeof(void*),
1)))))));}is_zero_terminated=1;goto _LL516;_LL51B:;_LL51C: is_zero_terminated=0;
goto _LL516;_LL516:;}{void*_tmpA52=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))
Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,_tmp987);struct Cyc_Absyn_Exp*
_tmpA55;_LL523: {struct Cyc_Absyn_DynEither_b_struct*_tmpA53=(struct Cyc_Absyn_DynEither_b_struct*)
_tmpA52;if(_tmpA53->tag != 0)goto _LL525;}_LL524: goto _LL522;_LL525: {struct Cyc_Absyn_Upper_b_struct*
_tmpA54=(struct Cyc_Absyn_Upper_b_struct*)_tmpA52;if(_tmpA54->tag != 1)goto _LL522;
else{_tmpA55=_tmpA54->f1;}}_LL526: {struct _RegionHandle*_tmpA56=Cyc_Tcenv_get_fnrgn(
te);{struct Cyc_Tcenv_Tenv*_tmpA57=Cyc_Tcenv_allow_valueof(_tmpA56,te);Cyc_Tcexp_tcExp(
_tmpA57,0,_tmpA55);}cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpA55,te,
cvtenv);if(!Cyc_Tcutil_coerce_uint_typ(te,_tmpA55)){const char*_tmp11EB;void*
_tmp11EA;(_tmp11EA=0,Cyc_Tcutil_terr(loc,((_tmp11EB="pointer bounds expression is not an unsigned int",
_tag_dyneither(_tmp11EB,sizeof(char),49))),_tag_dyneither(_tmp11EA,sizeof(void*),
0)));}{unsigned int _tmpA5B;int _tmpA5C;struct _tuple13 _tmpA5A=Cyc_Evexp_eval_const_uint_exp(
_tmpA55);_tmpA5B=_tmpA5A.f1;_tmpA5C=_tmpA5A.f2;if(is_zero_terminated  && (!
_tmpA5C  || _tmpA5B < 1)){const char*_tmp11EE;void*_tmp11ED;(_tmp11ED=0,Cyc_Tcutil_terr(
loc,((_tmp11EE="zero-terminated pointer cannot point to empty sequence",
_tag_dyneither(_tmp11EE,sizeof(char),55))),_tag_dyneither(_tmp11ED,sizeof(void*),
0)));}goto _LL522;}}_LL522:;}goto _LL4AA;}_LL4BB: {struct Cyc_Absyn_TagType_struct*
_tmp989=(struct Cyc_Absyn_TagType_struct*)_tmp965;if(_tmp989->tag != 20)goto _LL4BD;
else{_tmp98A=(void*)_tmp989->f1;}}_LL4BC: cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,Cyc_Absyn_IntKind,_tmp98A,1);goto _LL4AA;_LL4BD: {struct Cyc_Absyn_ValueofType_struct*
_tmp98B=(struct Cyc_Absyn_ValueofType_struct*)_tmp965;if(_tmp98B->tag != 19)goto
_LL4BF;else{_tmp98C=_tmp98B->f1;}}_LL4BE: {struct _RegionHandle*_tmpA5F=Cyc_Tcenv_get_fnrgn(
te);{struct Cyc_Tcenv_Tenv*_tmpA60=Cyc_Tcenv_allow_valueof(_tmpA5F,te);Cyc_Tcexp_tcExp(
_tmpA60,0,_tmp98C);}if(!Cyc_Tcutil_coerce_uint_typ(te,_tmp98C)){const char*
_tmp11F1;void*_tmp11F0;(_tmp11F0=0,Cyc_Tcutil_terr(loc,((_tmp11F1="valueof_t requires an int expression",
_tag_dyneither(_tmp11F1,sizeof(char),37))),_tag_dyneither(_tmp11F0,sizeof(void*),
0)));}cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp98C,te,cvtenv);goto
_LL4AA;}_LL4BF: {struct Cyc_Absyn_IntType_struct*_tmp98D=(struct Cyc_Absyn_IntType_struct*)
_tmp965;if(_tmp98D->tag != 6)goto _LL4C1;}_LL4C0: goto _LL4C2;_LL4C1: {struct Cyc_Absyn_FloatType_struct*
_tmp98E=(struct Cyc_Absyn_FloatType_struct*)_tmp965;if(_tmp98E->tag != 7)goto
_LL4C3;}_LL4C2: goto _LL4C4;_LL4C3: {struct Cyc_Absyn_DoubleType_struct*_tmp98F=(
struct Cyc_Absyn_DoubleType_struct*)_tmp965;if(_tmp98F->tag != 8)goto _LL4C5;}
_LL4C4: goto _LL4AA;_LL4C5: {struct Cyc_Absyn_ArrayType_struct*_tmp990=(struct Cyc_Absyn_ArrayType_struct*)
_tmp965;if(_tmp990->tag != 9)goto _LL4C7;else{_tmp991=_tmp990->f1;_tmp992=_tmp991.elt_type;
_tmp993=_tmp991.tq;_tmp994=(struct Cyc_Absyn_Tqual*)&(_tmp990->f1).tq;_tmp995=
_tmp991.num_elts;_tmp996=(struct Cyc_Absyn_Exp**)&(_tmp990->f1).num_elts;_tmp997=
_tmp991.zero_term;_tmp998=_tmp991.zt_loc;}}_LL4C6: {struct Cyc_Absyn_Exp*_tmpA63=*
_tmp996;cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,Cyc_Absyn_MemKind,
_tmp992,1);_tmp994->real_const=Cyc_Tcutil_extract_const_from_typedef(loc,_tmp994->print_const,
_tmp992);{int is_zero_terminated;{union Cyc_Absyn_Constraint*_tmpA64=((union Cyc_Absyn_Constraint*(*)(
union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)(_tmp997);union Cyc_Absyn_Constraint
_tmpA65;int _tmpA66;union Cyc_Absyn_Constraint _tmpA67;int _tmpA68;_LL528: _tmpA65=*
_tmpA64;if((_tmpA65.No_constr).tag != 3)goto _LL52A;_tmpA66=(int)(_tmpA65.No_constr).val;
_LL529:((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*
y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp997,Cyc_Absyn_false_conref);
is_zero_terminated=0;goto _LL527;_LL52A: _tmpA67=*_tmpA64;if((_tmpA67.Eq_constr).tag
!= 1)goto _LL52C;_tmpA68=(int)(_tmpA67.Eq_constr).val;if(_tmpA68 != 1)goto _LL52C;
_LL52B: if(!Cyc_Tcutil_admits_zero(_tmp992)){const char*_tmp11F5;void*_tmp11F4[1];
struct Cyc_String_pa_struct _tmp11F3;(_tmp11F3.tag=0,((_tmp11F3.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp992)),((
_tmp11F4[0]=& _tmp11F3,Cyc_Tcutil_terr(loc,((_tmp11F5="cannot have a zero-terminated array of %s elements",
_tag_dyneither(_tmp11F5,sizeof(char),51))),_tag_dyneither(_tmp11F4,sizeof(void*),
1)))))));}is_zero_terminated=1;goto _LL527;_LL52C:;_LL52D: is_zero_terminated=0;
goto _LL527;_LL527:;}if(_tmpA63 == 0){if(is_zero_terminated)*_tmp996=(_tmpA63=(
struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(1,0));else{{const char*_tmp11F8;void*
_tmp11F7;(_tmp11F7=0,Cyc_Tcutil_warn(loc,((_tmp11F8="array bound defaults to 1 here",
_tag_dyneither(_tmp11F8,sizeof(char),31))),_tag_dyneither(_tmp11F7,sizeof(void*),
0)));}*_tmp996=(_tmpA63=(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(1,0));}}Cyc_Tcexp_tcExp(
te,0,(struct Cyc_Absyn_Exp*)_tmpA63);if(!Cyc_Tcutil_is_const_exp(te,(struct Cyc_Absyn_Exp*)
_tmpA63)){const char*_tmp11FB;void*_tmp11FA;(_tmp11FA=0,Cyc_Tcutil_terr(loc,((
_tmp11FB="array bounds expression is not constant",_tag_dyneither(_tmp11FB,
sizeof(char),40))),_tag_dyneither(_tmp11FA,sizeof(void*),0)));}if(!Cyc_Tcutil_coerce_uint_typ(
te,(struct Cyc_Absyn_Exp*)_tmpA63)){const char*_tmp11FE;void*_tmp11FD;(_tmp11FD=0,
Cyc_Tcutil_terr(loc,((_tmp11FE="array bounds expression is not an unsigned int",
_tag_dyneither(_tmp11FE,sizeof(char),47))),_tag_dyneither(_tmp11FD,sizeof(void*),
0)));}{unsigned int _tmpA73;int _tmpA74;struct _tuple13 _tmpA72=Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)_tmpA63);_tmpA73=_tmpA72.f1;_tmpA74=_tmpA72.f2;if((
is_zero_terminated  && _tmpA74) && _tmpA73 < 1){const char*_tmp1201;void*_tmp1200;(
_tmp1200=0,Cyc_Tcutil_warn(loc,((_tmp1201="zero terminated array cannot have zero elements",
_tag_dyneither(_tmp1201,sizeof(char),48))),_tag_dyneither(_tmp1200,sizeof(void*),
0)));}if((_tmpA74  && _tmpA73 < 1) && Cyc_Cyclone_tovc_r){{const char*_tmp1204;void*
_tmp1203;(_tmp1203=0,Cyc_Tcutil_warn(loc,((_tmp1204="arrays with 0 elements are not supported except with gcc -- changing to 1.",
_tag_dyneither(_tmp1204,sizeof(char),75))),_tag_dyneither(_tmp1203,sizeof(void*),
0)));}*_tmp996=(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(1,0);}goto _LL4AA;}}}
_LL4C7: {struct Cyc_Absyn_FnType_struct*_tmp999=(struct Cyc_Absyn_FnType_struct*)
_tmp965;if(_tmp999->tag != 10)goto _LL4C9;else{_tmp99A=_tmp999->f1;_tmp99B=_tmp99A.tvars;
_tmp99C=(struct Cyc_List_List**)&(_tmp999->f1).tvars;_tmp99D=_tmp99A.effect;
_tmp99E=(struct Cyc_Core_Opt**)&(_tmp999->f1).effect;_tmp99F=_tmp99A.ret_typ;
_tmp9A0=_tmp99A.args;_tmp9A1=_tmp99A.c_varargs;_tmp9A2=_tmp99A.cyc_varargs;
_tmp9A3=_tmp99A.rgn_po;_tmp9A4=_tmp99A.attributes;}}_LL4C8: {int num_convs=0;int
seen_cdecl=0;int seen_stdcall=0;int seen_fastcall=0;int seen_format=0;enum Cyc_Absyn_Format_Type
ft=Cyc_Absyn_Printf_ft;int fmt_desc_arg=- 1;int fmt_arg_start=- 1;for(0;_tmp9A4 != 0;
_tmp9A4=_tmp9A4->tl){if(!Cyc_Absyn_fntype_att((void*)_tmp9A4->hd)){const char*
_tmp1208;void*_tmp1207[1];struct Cyc_String_pa_struct _tmp1206;(_tmp1206.tag=0,((
_tmp1206.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((
void*)_tmp9A4->hd)),((_tmp1207[0]=& _tmp1206,Cyc_Tcutil_terr(loc,((_tmp1208="bad function type attribute %s",
_tag_dyneither(_tmp1208,sizeof(char),31))),_tag_dyneither(_tmp1207,sizeof(void*),
1)))))));}{void*_tmpA7C=(void*)_tmp9A4->hd;enum Cyc_Absyn_Format_Type _tmpA81;int
_tmpA82;int _tmpA83;_LL52F: {struct Cyc_Absyn_Stdcall_att_struct*_tmpA7D=(struct
Cyc_Absyn_Stdcall_att_struct*)_tmpA7C;if(_tmpA7D->tag != 1)goto _LL531;}_LL530: if(
!seen_stdcall){seen_stdcall=1;++ num_convs;}goto _LL52E;_LL531: {struct Cyc_Absyn_Cdecl_att_struct*
_tmpA7E=(struct Cyc_Absyn_Cdecl_att_struct*)_tmpA7C;if(_tmpA7E->tag != 2)goto
_LL533;}_LL532: if(!seen_cdecl){seen_cdecl=1;++ num_convs;}goto _LL52E;_LL533: {
struct Cyc_Absyn_Fastcall_att_struct*_tmpA7F=(struct Cyc_Absyn_Fastcall_att_struct*)
_tmpA7C;if(_tmpA7F->tag != 3)goto _LL535;}_LL534: if(!seen_fastcall){seen_fastcall=
1;++ num_convs;}goto _LL52E;_LL535: {struct Cyc_Absyn_Format_att_struct*_tmpA80=(
struct Cyc_Absyn_Format_att_struct*)_tmpA7C;if(_tmpA80->tag != 19)goto _LL537;else{
_tmpA81=_tmpA80->f1;_tmpA82=_tmpA80->f2;_tmpA83=_tmpA80->f3;}}_LL536: if(!
seen_format){seen_format=1;ft=_tmpA81;fmt_desc_arg=_tmpA82;fmt_arg_start=_tmpA83;}
else{const char*_tmp120B;void*_tmp120A;(_tmp120A=0,Cyc_Tcutil_terr(loc,((_tmp120B="function can't have multiple format attributes",
_tag_dyneither(_tmp120B,sizeof(char),47))),_tag_dyneither(_tmp120A,sizeof(void*),
0)));}goto _LL52E;_LL537:;_LL538: goto _LL52E;_LL52E:;}}if(num_convs > 1){const char*
_tmp120E;void*_tmp120D;(_tmp120D=0,Cyc_Tcutil_terr(loc,((_tmp120E="function can't have multiple calling conventions",
_tag_dyneither(_tmp120E,sizeof(char),49))),_tag_dyneither(_tmp120D,sizeof(void*),
0)));}Cyc_Tcutil_check_unique_tvars(loc,*_tmp99C);{struct Cyc_List_List*b=*
_tmp99C;for(0;b != 0;b=b->tl){((struct Cyc_Absyn_Tvar*)b->hd)->identity=Cyc_Tcutil_new_tvar_id();
cvtenv.kind_env=Cyc_Tcutil_add_bound_tvar(cvtenv.kind_env,(struct Cyc_Absyn_Tvar*)
b->hd);{void*_tmpA88=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)b->hd)->kind);
enum Cyc_Absyn_Kind _tmpA8A;_LL53A: {struct Cyc_Absyn_Eq_kb_struct*_tmpA89=(struct
Cyc_Absyn_Eq_kb_struct*)_tmpA88;if(_tmpA89->tag != 0)goto _LL53C;else{_tmpA8A=
_tmpA89->f1;if(_tmpA8A != Cyc_Absyn_MemKind)goto _LL53C;}}_LL53B:{const char*
_tmp1212;void*_tmp1211[1];struct Cyc_String_pa_struct _tmp1210;(_tmp1210.tag=0,((
_tmp1210.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)
b->hd)->name),((_tmp1211[0]=& _tmp1210,Cyc_Tcutil_terr(loc,((_tmp1212="function attempts to abstract Mem type variable %s",
_tag_dyneither(_tmp1212,sizeof(char),51))),_tag_dyneither(_tmp1211,sizeof(void*),
1)))))));}goto _LL539;_LL53C:;_LL53D: goto _LL539;_LL539:;}}}{struct _RegionHandle*
_tmpA8E=Cyc_Tcenv_get_fnrgn(te);{struct Cyc_Tcutil_CVTEnv _tmp1213;struct Cyc_Tcutil_CVTEnv
_tmpA8F=(_tmp1213.r=_tmpA8E,((_tmp1213.kind_env=cvtenv.kind_env,((_tmp1213.free_vars=
0,((_tmp1213.free_evars=0,((_tmp1213.generalize_evars=cvtenv.generalize_evars,((
_tmp1213.fn_result=1,_tmp1213)))))))))));_tmpA8F=Cyc_Tcutil_i_check_valid_type(
loc,te,_tmpA8F,Cyc_Absyn_MemKind,_tmp99F,1);_tmpA8F.fn_result=0;{struct Cyc_List_List*
a=_tmp9A0;for(0;a != 0;a=a->tl){struct _tuple9*_tmpA90=(struct _tuple9*)a->hd;void*
_tmpA91=(*_tmpA90).f3;_tmpA8F=Cyc_Tcutil_i_check_valid_type(loc,te,_tmpA8F,Cyc_Absyn_MemKind,
_tmpA91,1);((*_tmpA90).f2).real_const=Cyc_Tcutil_extract_const_from_typedef(loc,((*
_tmpA90).f2).print_const,_tmpA91);}}if(_tmp9A2 != 0){if(_tmp9A1){const char*
_tmp1216;void*_tmp1215;(_tmp1215=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1216="both c_vararg and cyc_vararg",
_tag_dyneither(_tmp1216,sizeof(char),29))),_tag_dyneither(_tmp1215,sizeof(void*),
0)));}{void*_tmpA95;int _tmpA96;struct Cyc_Absyn_VarargInfo _tmpA94=*_tmp9A2;
_tmpA95=_tmpA94.type;_tmpA96=_tmpA94.inject;_tmpA8F=Cyc_Tcutil_i_check_valid_type(
loc,te,_tmpA8F,Cyc_Absyn_MemKind,_tmpA95,1);(_tmp9A2->tq).real_const=Cyc_Tcutil_extract_const_from_typedef(
loc,(_tmp9A2->tq).print_const,_tmpA95);if(_tmpA96){void*_tmpA97=Cyc_Tcutil_compress(
_tmpA95);struct Cyc_Absyn_PtrInfo _tmpA99;void*_tmpA9A;struct Cyc_Absyn_PtrAtts
_tmpA9B;union Cyc_Absyn_Constraint*_tmpA9C;union Cyc_Absyn_Constraint*_tmpA9D;
_LL53F: {struct Cyc_Absyn_PointerType_struct*_tmpA98=(struct Cyc_Absyn_PointerType_struct*)
_tmpA97;if(_tmpA98->tag != 5)goto _LL541;else{_tmpA99=_tmpA98->f1;_tmpA9A=_tmpA99.elt_typ;
_tmpA9B=_tmpA99.ptr_atts;_tmpA9C=_tmpA9B.bounds;_tmpA9D=_tmpA9B.zero_term;}}
_LL540:{void*_tmpA9E=Cyc_Tcutil_compress(_tmpA9A);_LL544: {struct Cyc_Absyn_DatatypeType_struct*
_tmpA9F=(struct Cyc_Absyn_DatatypeType_struct*)_tmpA9E;if(_tmpA9F->tag != 3)goto
_LL546;}_LL545: if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(
0,_tmpA9D)){const char*_tmp1219;void*_tmp1218;(_tmp1218=0,Cyc_Tcutil_terr(loc,((
_tmp1219="can't inject into a zeroterm pointer",_tag_dyneither(_tmp1219,sizeof(
char),37))),_tag_dyneither(_tmp1218,sizeof(void*),0)));}{void*_tmpAA2=((void*(*)(
void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,
_tmpA9C);_LL549: {struct Cyc_Absyn_DynEither_b_struct*_tmpAA3=(struct Cyc_Absyn_DynEither_b_struct*)
_tmpAA2;if(_tmpAA3->tag != 0)goto _LL54B;}_LL54A:{const char*_tmp121C;void*_tmp121B;(
_tmp121B=0,Cyc_Tcutil_terr(loc,((_tmp121C="can't inject into a fat pointer to datatype",
_tag_dyneither(_tmp121C,sizeof(char),44))),_tag_dyneither(_tmp121B,sizeof(void*),
0)));}goto _LL548;_LL54B:;_LL54C: goto _LL548;_LL548:;}goto _LL543;_LL546:;_LL547:{
const char*_tmp121F;void*_tmp121E;(_tmp121E=0,Cyc_Tcutil_terr(loc,((_tmp121F="can't inject a non-datatype type",
_tag_dyneither(_tmp121F,sizeof(char),33))),_tag_dyneither(_tmp121E,sizeof(void*),
0)));}goto _LL543;_LL543:;}goto _LL53E;_LL541:;_LL542:{const char*_tmp1222;void*
_tmp1221;(_tmp1221=0,Cyc_Tcutil_terr(loc,((_tmp1222="expecting a datatype pointer type",
_tag_dyneither(_tmp1222,sizeof(char),34))),_tag_dyneither(_tmp1221,sizeof(void*),
0)));}goto _LL53E;_LL53E:;}}}if(seen_format){int _tmpAAA=((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(_tmp9A0);if((((fmt_desc_arg < 0  || fmt_desc_arg > _tmpAAA) || 
fmt_arg_start < 0) || _tmp9A2 == 0  && fmt_arg_start != 0) || _tmp9A2 != 0  && 
fmt_arg_start != _tmpAAA + 1){const char*_tmp1225;void*_tmp1224;(_tmp1224=0,Cyc_Tcutil_terr(
loc,((_tmp1225="bad format descriptor",_tag_dyneither(_tmp1225,sizeof(char),22))),
_tag_dyneither(_tmp1224,sizeof(void*),0)));}else{void*_tmpAAE;struct _tuple9
_tmpAAD=*((struct _tuple9*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp9A0,
fmt_desc_arg - 1);_tmpAAE=_tmpAAD.f3;{void*_tmpAAF=Cyc_Tcutil_compress(_tmpAAE);
struct Cyc_Absyn_PtrInfo _tmpAB1;void*_tmpAB2;struct Cyc_Absyn_PtrAtts _tmpAB3;union
Cyc_Absyn_Constraint*_tmpAB4;union Cyc_Absyn_Constraint*_tmpAB5;_LL54E: {struct
Cyc_Absyn_PointerType_struct*_tmpAB0=(struct Cyc_Absyn_PointerType_struct*)
_tmpAAF;if(_tmpAB0->tag != 5)goto _LL550;else{_tmpAB1=_tmpAB0->f1;_tmpAB2=_tmpAB1.elt_typ;
_tmpAB3=_tmpAB1.ptr_atts;_tmpAB4=_tmpAB3.bounds;_tmpAB5=_tmpAB3.zero_term;}}
_LL54F:{struct _tuple0 _tmp1226;struct _tuple0 _tmpAB7=(_tmp1226.f1=Cyc_Tcutil_compress(
_tmpAB2),((_tmp1226.f2=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((
void*)& Cyc_Absyn_DynEither_b_val,_tmpAB4),_tmp1226)));void*_tmpAB8;enum Cyc_Absyn_Sign
_tmpABA;enum Cyc_Absyn_Size_of _tmpABB;void*_tmpABC;_LL553: _tmpAB8=_tmpAB7.f1;{
struct Cyc_Absyn_IntType_struct*_tmpAB9=(struct Cyc_Absyn_IntType_struct*)_tmpAB8;
if(_tmpAB9->tag != 6)goto _LL555;else{_tmpABA=_tmpAB9->f1;if(_tmpABA != Cyc_Absyn_None)
goto _LL555;_tmpABB=_tmpAB9->f2;if(_tmpABB != Cyc_Absyn_Char_sz)goto _LL555;}}
_tmpABC=_tmpAB7.f2;{struct Cyc_Absyn_DynEither_b_struct*_tmpABD=(struct Cyc_Absyn_DynEither_b_struct*)
_tmpABC;if(_tmpABD->tag != 0)goto _LL555;}_LL554: goto _LL552;_LL555:;_LL556:{const
char*_tmp1229;void*_tmp1228;(_tmp1228=0,Cyc_Tcutil_terr(loc,((_tmp1229="format descriptor is not a char ? type",
_tag_dyneither(_tmp1229,sizeof(char),39))),_tag_dyneither(_tmp1228,sizeof(void*),
0)));}goto _LL552;_LL552:;}goto _LL54D;_LL550:;_LL551:{const char*_tmp122C;void*
_tmp122B;(_tmp122B=0,Cyc_Tcutil_terr(loc,((_tmp122C="format descriptor is not a char ? type",
_tag_dyneither(_tmp122C,sizeof(char),39))),_tag_dyneither(_tmp122B,sizeof(void*),
0)));}goto _LL54D;_LL54D:;}if(fmt_arg_start != 0){int problem;{struct _tuple26
_tmp122D;struct _tuple26 _tmpAC3=(_tmp122D.f1=ft,((_tmp122D.f2=Cyc_Tcutil_compress(
Cyc_Tcutil_pointer_elt_type(((struct Cyc_Absyn_VarargInfo*)_check_null(_tmp9A2))->type)),
_tmp122D)));enum Cyc_Absyn_Format_Type _tmpAC4;void*_tmpAC5;struct Cyc_Absyn_DatatypeInfo
_tmpAC7;union Cyc_Absyn_DatatypeInfoU _tmpAC8;struct Cyc_Absyn_Datatypedecl**
_tmpAC9;struct Cyc_Absyn_Datatypedecl*_tmpACA;enum Cyc_Absyn_Format_Type _tmpACB;
void*_tmpACC;struct Cyc_Absyn_DatatypeInfo _tmpACE;union Cyc_Absyn_DatatypeInfoU
_tmpACF;struct Cyc_Absyn_Datatypedecl**_tmpAD0;struct Cyc_Absyn_Datatypedecl*
_tmpAD1;_LL558: _tmpAC4=_tmpAC3.f1;if(_tmpAC4 != Cyc_Absyn_Printf_ft)goto _LL55A;
_tmpAC5=_tmpAC3.f2;{struct Cyc_Absyn_DatatypeType_struct*_tmpAC6=(struct Cyc_Absyn_DatatypeType_struct*)
_tmpAC5;if(_tmpAC6->tag != 3)goto _LL55A;else{_tmpAC7=_tmpAC6->f1;_tmpAC8=_tmpAC7.datatype_info;
if((_tmpAC8.KnownDatatype).tag != 2)goto _LL55A;_tmpAC9=(struct Cyc_Absyn_Datatypedecl**)(
_tmpAC8.KnownDatatype).val;_tmpACA=*_tmpAC9;}}_LL559: problem=Cyc_Absyn_qvar_cmp(
_tmpACA->name,Cyc_Absyn_datatype_print_arg_qvar)!= 0;goto _LL557;_LL55A: _tmpACB=
_tmpAC3.f1;if(_tmpACB != Cyc_Absyn_Scanf_ft)goto _LL55C;_tmpACC=_tmpAC3.f2;{struct
Cyc_Absyn_DatatypeType_struct*_tmpACD=(struct Cyc_Absyn_DatatypeType_struct*)
_tmpACC;if(_tmpACD->tag != 3)goto _LL55C;else{_tmpACE=_tmpACD->f1;_tmpACF=_tmpACE.datatype_info;
if((_tmpACF.KnownDatatype).tag != 2)goto _LL55C;_tmpAD0=(struct Cyc_Absyn_Datatypedecl**)(
_tmpACF.KnownDatatype).val;_tmpAD1=*_tmpAD0;}}_LL55B: problem=Cyc_Absyn_qvar_cmp(
_tmpAD1->name,Cyc_Absyn_datatype_scanf_arg_qvar)!= 0;goto _LL557;_LL55C:;_LL55D:
problem=1;goto _LL557;_LL557:;}if(problem){const char*_tmp1230;void*_tmp122F;(
_tmp122F=0,Cyc_Tcutil_terr(loc,((_tmp1230="format attribute and vararg types don't match",
_tag_dyneither(_tmp1230,sizeof(char),46))),_tag_dyneither(_tmp122F,sizeof(void*),
0)));}}}}{struct Cyc_List_List*rpo=_tmp9A3;for(0;rpo != 0;rpo=rpo->tl){struct
_tuple0 _tmpAD5;void*_tmpAD6;void*_tmpAD7;struct _tuple0*_tmpAD4=(struct _tuple0*)
rpo->hd;_tmpAD5=*_tmpAD4;_tmpAD6=_tmpAD5.f1;_tmpAD7=_tmpAD5.f2;_tmpA8F=Cyc_Tcutil_i_check_valid_type(
loc,te,_tmpA8F,Cyc_Absyn_EffKind,_tmpAD6,1);_tmpA8F=Cyc_Tcutil_i_check_valid_type(
loc,te,_tmpA8F,Cyc_Absyn_TopRgnKind,_tmpAD7,1);}}if(*_tmp99E != 0)_tmpA8F=Cyc_Tcutil_i_check_valid_type(
loc,te,_tmpA8F,Cyc_Absyn_EffKind,(void*)((struct Cyc_Core_Opt*)_check_null(*
_tmp99E))->v,1);else{struct Cyc_List_List*effect=0;{struct Cyc_List_List*tvs=
_tmpA8F.free_vars;for(0;tvs != 0;tvs=tvs->tl){struct Cyc_Absyn_Tvar*_tmpAD9;int
_tmpADA;struct _tuple24 _tmpAD8=*((struct _tuple24*)tvs->hd);_tmpAD9=_tmpAD8.f1;
_tmpADA=_tmpAD8.f2;if(!_tmpADA)continue;{void*_tmpADB=Cyc_Absyn_compress_kb(
_tmpAD9->kind);struct Cyc_Core_Opt*_tmpADD;struct Cyc_Core_Opt**_tmpADE;enum Cyc_Absyn_Kind
_tmpADF;struct Cyc_Core_Opt*_tmpAE1;struct Cyc_Core_Opt**_tmpAE2;enum Cyc_Absyn_Kind
_tmpAE3;enum Cyc_Absyn_Kind _tmpAE5;enum Cyc_Absyn_Kind _tmpAE7;enum Cyc_Absyn_Kind
_tmpAE9;struct Cyc_Core_Opt*_tmpAEB;struct Cyc_Core_Opt**_tmpAEC;enum Cyc_Absyn_Kind
_tmpAED;enum Cyc_Absyn_Kind _tmpAEF;struct Cyc_Core_Opt*_tmpAF1;struct Cyc_Core_Opt**
_tmpAF2;_LL55F: {struct Cyc_Absyn_Less_kb_struct*_tmpADC=(struct Cyc_Absyn_Less_kb_struct*)
_tmpADB;if(_tmpADC->tag != 2)goto _LL561;else{_tmpADD=_tmpADC->f1;_tmpADE=(struct
Cyc_Core_Opt**)& _tmpADC->f1;_tmpADF=_tmpADC->f2;if(_tmpADF != Cyc_Absyn_TopRgnKind)
goto _LL561;}}_LL560: _tmpAE2=_tmpADE;_tmpAE3=Cyc_Absyn_RgnKind;goto _LL562;_LL561:{
struct Cyc_Absyn_Less_kb_struct*_tmpAE0=(struct Cyc_Absyn_Less_kb_struct*)_tmpADB;
if(_tmpAE0->tag != 2)goto _LL563;else{_tmpAE1=_tmpAE0->f1;_tmpAE2=(struct Cyc_Core_Opt**)&
_tmpAE0->f1;_tmpAE3=_tmpAE0->f2;}}if(!(_tmpAE3 == Cyc_Absyn_RgnKind  || _tmpAE3 == 
Cyc_Absyn_UniqueRgnKind))goto _LL563;_LL562:*_tmpAE2=Cyc_Tcutil_kind_to_bound_opt(
_tmpAE3);_tmpAE5=_tmpAE3;goto _LL564;_LL563:{struct Cyc_Absyn_Eq_kb_struct*_tmpAE4=(
struct Cyc_Absyn_Eq_kb_struct*)_tmpADB;if(_tmpAE4->tag != 0)goto _LL565;else{
_tmpAE5=_tmpAE4->f1;}}if(!((_tmpAE5 == Cyc_Absyn_RgnKind  || _tmpAE5 == Cyc_Absyn_UniqueRgnKind)
 || _tmpAE5 == Cyc_Absyn_TopRgnKind))goto _LL565;_LL564:{struct Cyc_Absyn_AccessEff_struct*
_tmp123F;struct Cyc_Absyn_VarType_struct*_tmp123E;struct Cyc_Absyn_VarType_struct
_tmp123D;struct Cyc_Absyn_AccessEff_struct _tmp123C;struct Cyc_List_List*_tmp123B;
effect=((_tmp123B=_cycalloc(sizeof(*_tmp123B)),((_tmp123B->hd=(void*)((_tmp123F=
_cycalloc(sizeof(*_tmp123F)),((_tmp123F[0]=((_tmp123C.tag=23,((_tmp123C.f1=(void*)((
void*)((_tmp123E=_cycalloc(sizeof(*_tmp123E)),((_tmp123E[0]=((_tmp123D.tag=2,((
_tmp123D.f1=_tmpAD9,_tmp123D)))),_tmp123E))))),_tmp123C)))),_tmp123F)))),((
_tmp123B->tl=effect,_tmp123B))))));}goto _LL55E;_LL565: {struct Cyc_Absyn_Less_kb_struct*
_tmpAE6=(struct Cyc_Absyn_Less_kb_struct*)_tmpADB;if(_tmpAE6->tag != 2)goto _LL567;
else{_tmpAE7=_tmpAE6->f2;if(_tmpAE7 != Cyc_Absyn_IntKind)goto _LL567;}}_LL566: goto
_LL568;_LL567: {struct Cyc_Absyn_Eq_kb_struct*_tmpAE8=(struct Cyc_Absyn_Eq_kb_struct*)
_tmpADB;if(_tmpAE8->tag != 0)goto _LL569;else{_tmpAE9=_tmpAE8->f1;if(_tmpAE9 != Cyc_Absyn_IntKind)
goto _LL569;}}_LL568: goto _LL55E;_LL569: {struct Cyc_Absyn_Less_kb_struct*_tmpAEA=(
struct Cyc_Absyn_Less_kb_struct*)_tmpADB;if(_tmpAEA->tag != 2)goto _LL56B;else{
_tmpAEB=_tmpAEA->f1;_tmpAEC=(struct Cyc_Core_Opt**)& _tmpAEA->f1;_tmpAED=_tmpAEA->f2;
if(_tmpAED != Cyc_Absyn_EffKind)goto _LL56B;}}_LL56A:*_tmpAEC=Cyc_Tcutil_kind_to_bound_opt(
Cyc_Absyn_EffKind);goto _LL56C;_LL56B: {struct Cyc_Absyn_Eq_kb_struct*_tmpAEE=(
struct Cyc_Absyn_Eq_kb_struct*)_tmpADB;if(_tmpAEE->tag != 0)goto _LL56D;else{
_tmpAEF=_tmpAEE->f1;if(_tmpAEF != Cyc_Absyn_EffKind)goto _LL56D;}}_LL56C:{struct
Cyc_Absyn_VarType_struct*_tmp1245;struct Cyc_Absyn_VarType_struct _tmp1244;struct
Cyc_List_List*_tmp1243;effect=((_tmp1243=_cycalloc(sizeof(*_tmp1243)),((_tmp1243->hd=(
void*)((_tmp1245=_cycalloc(sizeof(*_tmp1245)),((_tmp1245[0]=((_tmp1244.tag=2,((
_tmp1244.f1=_tmpAD9,_tmp1244)))),_tmp1245)))),((_tmp1243->tl=effect,_tmp1243))))));}
goto _LL55E;_LL56D: {struct Cyc_Absyn_Unknown_kb_struct*_tmpAF0=(struct Cyc_Absyn_Unknown_kb_struct*)
_tmpADB;if(_tmpAF0->tag != 1)goto _LL56F;else{_tmpAF1=_tmpAF0->f1;_tmpAF2=(struct
Cyc_Core_Opt**)& _tmpAF0->f1;}}_LL56E:{struct Cyc_Absyn_Less_kb_struct*_tmp124B;
struct Cyc_Absyn_Less_kb_struct _tmp124A;struct Cyc_Core_Opt*_tmp1249;*_tmpAF2=((
_tmp1249=_cycalloc(sizeof(*_tmp1249)),((_tmp1249->v=(void*)((_tmp124B=_cycalloc(
sizeof(*_tmp124B)),((_tmp124B[0]=((_tmp124A.tag=2,((_tmp124A.f1=0,((_tmp124A.f2=
Cyc_Absyn_AnyKind,_tmp124A)))))),_tmp124B)))),_tmp1249))));}goto _LL570;_LL56F:;
_LL570:{struct Cyc_Absyn_RgnsEff_struct*_tmp125A;struct Cyc_Absyn_VarType_struct*
_tmp1259;struct Cyc_Absyn_VarType_struct _tmp1258;struct Cyc_Absyn_RgnsEff_struct
_tmp1257;struct Cyc_List_List*_tmp1256;effect=((_tmp1256=_cycalloc(sizeof(*
_tmp1256)),((_tmp1256->hd=(void*)((_tmp125A=_cycalloc(sizeof(*_tmp125A)),((
_tmp125A[0]=((_tmp1257.tag=25,((_tmp1257.f1=(void*)((void*)((_tmp1259=_cycalloc(
sizeof(*_tmp1259)),((_tmp1259[0]=((_tmp1258.tag=2,((_tmp1258.f1=_tmpAD9,_tmp1258)))),
_tmp1259))))),_tmp1257)))),_tmp125A)))),((_tmp1256->tl=effect,_tmp1256))))));}
goto _LL55E;_LL55E:;}}}{struct Cyc_List_List*ts=_tmpA8F.free_evars;for(0;ts != 0;ts=
ts->tl){void*_tmpB04;int _tmpB05;struct _tuple25 _tmpB03=*((struct _tuple25*)ts->hd);
_tmpB04=_tmpB03.f1;_tmpB05=_tmpB03.f2;if(!_tmpB05)continue;switch(Cyc_Tcutil_typ_kind(
_tmpB04)){case Cyc_Absyn_TopRgnKind: _LL571: goto _LL572;case Cyc_Absyn_UniqueRgnKind:
_LL572: goto _LL573;case Cyc_Absyn_RgnKind: _LL573:{struct Cyc_Absyn_AccessEff_struct*
_tmp1260;struct Cyc_Absyn_AccessEff_struct _tmp125F;struct Cyc_List_List*_tmp125E;
effect=((_tmp125E=_cycalloc(sizeof(*_tmp125E)),((_tmp125E->hd=(void*)((_tmp1260=
_cycalloc(sizeof(*_tmp1260)),((_tmp1260[0]=((_tmp125F.tag=23,((_tmp125F.f1=(void*)
_tmpB04,_tmp125F)))),_tmp1260)))),((_tmp125E->tl=effect,_tmp125E))))));}break;
case Cyc_Absyn_EffKind: _LL574:{struct Cyc_List_List*_tmp1261;effect=((_tmp1261=
_cycalloc(sizeof(*_tmp1261)),((_tmp1261->hd=_tmpB04,((_tmp1261->tl=effect,
_tmp1261))))));}break;case Cyc_Absyn_IntKind: _LL575: break;default: _LL576:{struct
Cyc_Absyn_RgnsEff_struct*_tmp1267;struct Cyc_Absyn_RgnsEff_struct _tmp1266;struct
Cyc_List_List*_tmp1265;effect=((_tmp1265=_cycalloc(sizeof(*_tmp1265)),((_tmp1265->hd=(
void*)((_tmp1267=_cycalloc(sizeof(*_tmp1267)),((_tmp1267[0]=((_tmp1266.tag=25,((
_tmp1266.f1=(void*)_tmpB04,_tmp1266)))),_tmp1267)))),((_tmp1265->tl=effect,
_tmp1265))))));}break;}}}{struct Cyc_Absyn_JoinEff_struct*_tmp126D;struct Cyc_Absyn_JoinEff_struct
_tmp126C;struct Cyc_Core_Opt*_tmp126B;*_tmp99E=((_tmp126B=_cycalloc(sizeof(*
_tmp126B)),((_tmp126B->v=(void*)((_tmp126D=_cycalloc(sizeof(*_tmp126D)),((
_tmp126D[0]=((_tmp126C.tag=24,((_tmp126C.f1=effect,_tmp126C)))),_tmp126D)))),
_tmp126B))));}}if(*_tmp99C != 0){struct Cyc_List_List*bs=*_tmp99C;for(0;bs != 0;bs=
bs->tl){void*_tmpB10=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)bs->hd)->kind);
struct Cyc_Core_Opt*_tmpB12;struct Cyc_Core_Opt**_tmpB13;struct Cyc_Core_Opt*
_tmpB15;struct Cyc_Core_Opt**_tmpB16;enum Cyc_Absyn_Kind _tmpB17;struct Cyc_Core_Opt*
_tmpB19;struct Cyc_Core_Opt**_tmpB1A;enum Cyc_Absyn_Kind _tmpB1B;struct Cyc_Core_Opt*
_tmpB1D;struct Cyc_Core_Opt**_tmpB1E;enum Cyc_Absyn_Kind _tmpB1F;struct Cyc_Core_Opt*
_tmpB21;struct Cyc_Core_Opt**_tmpB22;enum Cyc_Absyn_Kind _tmpB23;enum Cyc_Absyn_Kind
_tmpB25;_LL579: {struct Cyc_Absyn_Unknown_kb_struct*_tmpB11=(struct Cyc_Absyn_Unknown_kb_struct*)
_tmpB10;if(_tmpB11->tag != 1)goto _LL57B;else{_tmpB12=_tmpB11->f1;_tmpB13=(struct
Cyc_Core_Opt**)& _tmpB11->f1;}}_LL57A:{const char*_tmp1271;void*_tmp1270[1];struct
Cyc_String_pa_struct _tmp126F;(_tmp126F.tag=0,((_tmp126F.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)bs->hd)->name),((_tmp1270[0]=&
_tmp126F,Cyc_Tcutil_warn(loc,((_tmp1271="Type variable %s unconstrained, assuming boxed",
_tag_dyneither(_tmp1271,sizeof(char),47))),_tag_dyneither(_tmp1270,sizeof(void*),
1)))))));}_tmpB16=_tmpB13;goto _LL57C;_LL57B: {struct Cyc_Absyn_Less_kb_struct*
_tmpB14=(struct Cyc_Absyn_Less_kb_struct*)_tmpB10;if(_tmpB14->tag != 2)goto _LL57D;
else{_tmpB15=_tmpB14->f1;_tmpB16=(struct Cyc_Core_Opt**)& _tmpB14->f1;_tmpB17=
_tmpB14->f2;if(_tmpB17 != Cyc_Absyn_MemKind)goto _LL57D;}}_LL57C: _tmpB1A=_tmpB16;
goto _LL57E;_LL57D: {struct Cyc_Absyn_Less_kb_struct*_tmpB18=(struct Cyc_Absyn_Less_kb_struct*)
_tmpB10;if(_tmpB18->tag != 2)goto _LL57F;else{_tmpB19=_tmpB18->f1;_tmpB1A=(struct
Cyc_Core_Opt**)& _tmpB18->f1;_tmpB1B=_tmpB18->f2;if(_tmpB1B != Cyc_Absyn_AnyKind)
goto _LL57F;}}_LL57E:*_tmpB1A=Cyc_Tcutil_kind_to_bound_opt(Cyc_Absyn_BoxKind);
goto _LL578;_LL57F: {struct Cyc_Absyn_Less_kb_struct*_tmpB1C=(struct Cyc_Absyn_Less_kb_struct*)
_tmpB10;if(_tmpB1C->tag != 2)goto _LL581;else{_tmpB1D=_tmpB1C->f1;_tmpB1E=(struct
Cyc_Core_Opt**)& _tmpB1C->f1;_tmpB1F=_tmpB1C->f2;if(_tmpB1F != Cyc_Absyn_TopRgnKind)
goto _LL581;}}_LL580:*_tmpB1E=Cyc_Tcutil_kind_to_bound_opt(Cyc_Absyn_RgnKind);
goto _LL578;_LL581: {struct Cyc_Absyn_Less_kb_struct*_tmpB20=(struct Cyc_Absyn_Less_kb_struct*)
_tmpB10;if(_tmpB20->tag != 2)goto _LL583;else{_tmpB21=_tmpB20->f1;_tmpB22=(struct
Cyc_Core_Opt**)& _tmpB20->f1;_tmpB23=_tmpB20->f2;}}_LL582:*_tmpB22=Cyc_Tcutil_kind_to_bound_opt(
_tmpB23);goto _LL578;_LL583: {struct Cyc_Absyn_Eq_kb_struct*_tmpB24=(struct Cyc_Absyn_Eq_kb_struct*)
_tmpB10;if(_tmpB24->tag != 0)goto _LL585;else{_tmpB25=_tmpB24->f1;if(_tmpB25 != Cyc_Absyn_MemKind)
goto _LL585;}}_LL584:{const char*_tmp1274;void*_tmp1273;(_tmp1273=0,Cyc_Tcutil_terr(
loc,((_tmp1274="functions can't abstract M types",_tag_dyneither(_tmp1274,
sizeof(char),33))),_tag_dyneither(_tmp1273,sizeof(void*),0)));}goto _LL578;_LL585:;
_LL586: goto _LL578;_LL578:;}}cvtenv.kind_env=Cyc_Tcutil_remove_bound_tvars(Cyc_Core_heap_region,
_tmpA8F.kind_env,*_tmp99C);_tmpA8F.free_vars=Cyc_Tcutil_remove_bound_tvars_bool(
_tmpA8F.r,_tmpA8F.free_vars,*_tmp99C);{struct Cyc_List_List*tvs=_tmpA8F.free_vars;
for(0;tvs != 0;tvs=tvs->tl){struct Cyc_Absyn_Tvar*_tmpB2C;int _tmpB2D;struct
_tuple24 _tmpB2B=*((struct _tuple24*)tvs->hd);_tmpB2C=_tmpB2B.f1;_tmpB2D=_tmpB2B.f2;
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,
_tmpB2C,_tmpB2D);}}{struct Cyc_List_List*evs=_tmpA8F.free_evars;for(0;evs != 0;evs=
evs->tl){void*_tmpB2F;int _tmpB30;struct _tuple25 _tmpB2E=*((struct _tuple25*)evs->hd);
_tmpB2F=_tmpB2E.f1;_tmpB30=_tmpB2E.f2;cvtenv.free_evars=Cyc_Tcutil_add_free_evar(
cvtenv.r,cvtenv.free_evars,_tmpB2F,_tmpB30);}}}goto _LL4AA;}}_LL4C9: {struct Cyc_Absyn_TupleType_struct*
_tmp9A5=(struct Cyc_Absyn_TupleType_struct*)_tmp965;if(_tmp9A5->tag != 11)goto
_LL4CB;else{_tmp9A6=_tmp9A5->f1;}}_LL4CA: for(0;_tmp9A6 != 0;_tmp9A6=_tmp9A6->tl){
struct _tuple11*_tmpB32=(struct _tuple11*)_tmp9A6->hd;cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,Cyc_Absyn_MemKind,(*_tmpB32).f2,1);((*_tmpB32).f1).real_const=Cyc_Tcutil_extract_const_from_typedef(
loc,((*_tmpB32).f1).print_const,(*_tmpB32).f2);}goto _LL4AA;_LL4CB: {struct Cyc_Absyn_AnonAggrType_struct*
_tmp9A7=(struct Cyc_Absyn_AnonAggrType_struct*)_tmp965;if(_tmp9A7->tag != 13)goto
_LL4CD;else{_tmp9A8=_tmp9A7->f1;_tmp9A9=_tmp9A7->f2;}}_LL4CC: {struct
_RegionHandle*_tmpB33=Cyc_Tcenv_get_fnrgn(te);{struct Cyc_List_List*prev_fields=0;
for(0;_tmp9A9 != 0;_tmp9A9=_tmp9A9->tl){struct Cyc_Absyn_Aggrfield _tmpB35;struct
_dyneither_ptr*_tmpB36;struct Cyc_Absyn_Tqual _tmpB37;struct Cyc_Absyn_Tqual*
_tmpB38;void*_tmpB39;struct Cyc_Absyn_Exp*_tmpB3A;struct Cyc_List_List*_tmpB3B;
struct Cyc_Absyn_Aggrfield*_tmpB34=(struct Cyc_Absyn_Aggrfield*)_tmp9A9->hd;
_tmpB35=*_tmpB34;_tmpB36=_tmpB35.name;_tmpB37=_tmpB35.tq;_tmpB38=(struct Cyc_Absyn_Tqual*)&(*
_tmpB34).tq;_tmpB39=_tmpB35.type;_tmpB3A=_tmpB35.width;_tmpB3B=_tmpB35.attributes;
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*
l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,_tmpB36)){
const char*_tmp1278;void*_tmp1277[1];struct Cyc_String_pa_struct _tmp1276;(_tmp1276.tag=
0,((_tmp1276.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpB36),((
_tmp1277[0]=& _tmp1276,Cyc_Tcutil_terr(loc,((_tmp1278="duplicate field %s",
_tag_dyneither(_tmp1278,sizeof(char),19))),_tag_dyneither(_tmp1277,sizeof(void*),
1)))))));}{const char*_tmp1279;if(Cyc_strcmp((struct _dyneither_ptr)*_tmpB36,((
_tmp1279="",_tag_dyneither(_tmp1279,sizeof(char),1))))!= 0){struct Cyc_List_List*
_tmp127A;prev_fields=((_tmp127A=_region_malloc(_tmpB33,sizeof(*_tmp127A)),((
_tmp127A->hd=_tmpB36,((_tmp127A->tl=prev_fields,_tmp127A))))));}}cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,Cyc_Absyn_MemKind,_tmpB39,1);_tmpB38->real_const=Cyc_Tcutil_extract_const_from_typedef(
loc,_tmpB38->print_const,_tmpB39);if(_tmp9A8 == Cyc_Absyn_UnionA  && !Cyc_Tcutil_bits_only(
_tmpB39)){const char*_tmp127E;void*_tmp127D[1];struct Cyc_String_pa_struct _tmp127C;(
_tmp127C.tag=0,((_tmp127C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*
_tmpB36),((_tmp127D[0]=& _tmp127C,Cyc_Tcutil_warn(loc,((_tmp127E="union member %s is not `bits-only' so it can only be written and not read",
_tag_dyneither(_tmp127E,sizeof(char),74))),_tag_dyneither(_tmp127D,sizeof(void*),
1)))))));}Cyc_Tcutil_check_bitfield(loc,te,_tmpB39,_tmpB3A,_tmpB36);Cyc_Tcutil_check_field_atts(
loc,_tmpB36,_tmpB3B);}}goto _LL4AA;}_LL4CD: {struct Cyc_Absyn_AggrType_struct*
_tmp9AA=(struct Cyc_Absyn_AggrType_struct*)_tmp965;if(_tmp9AA->tag != 12)goto
_LL4CF;else{_tmp9AB=_tmp9AA->f1;_tmp9AC=_tmp9AB.aggr_info;_tmp9AD=(union Cyc_Absyn_AggrInfoU*)&(
_tmp9AA->f1).aggr_info;_tmp9AE=_tmp9AB.targs;_tmp9AF=(struct Cyc_List_List**)&(
_tmp9AA->f1).targs;}}_LL4CE:{union Cyc_Absyn_AggrInfoU _tmpB44=*_tmp9AD;struct
_tuple4 _tmpB45;enum Cyc_Absyn_AggrKind _tmpB46;struct _tuple2*_tmpB47;struct Cyc_Core_Opt*
_tmpB48;struct Cyc_Absyn_Aggrdecl**_tmpB49;struct Cyc_Absyn_Aggrdecl*_tmpB4A;
_LL588: if((_tmpB44.UnknownAggr).tag != 1)goto _LL58A;_tmpB45=(struct _tuple4)(
_tmpB44.UnknownAggr).val;_tmpB46=_tmpB45.f1;_tmpB47=_tmpB45.f2;_tmpB48=_tmpB45.f3;
_LL589: {struct Cyc_Absyn_Aggrdecl**adp;{struct _handler_cons _tmpB4B;_push_handler(&
_tmpB4B);{int _tmpB4D=0;if(setjmp(_tmpB4B.handler))_tmpB4D=1;if(!_tmpB4D){adp=Cyc_Tcenv_lookup_aggrdecl(
te,loc,_tmpB47);{struct Cyc_Absyn_Aggrdecl*_tmpB4E=*adp;if(_tmpB4E->kind != 
_tmpB46){if(_tmpB4E->kind == Cyc_Absyn_StructA){const char*_tmp1283;void*_tmp1282[
2];struct Cyc_String_pa_struct _tmp1281;struct Cyc_String_pa_struct _tmp1280;(
_tmp1280.tag=0,((_tmp1280.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpB47)),((_tmp1281.tag=0,((_tmp1281.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpB47)),((_tmp1282[0]=& _tmp1281,((
_tmp1282[1]=& _tmp1280,Cyc_Tcutil_terr(loc,((_tmp1283="expecting struct %s instead of union %s",
_tag_dyneither(_tmp1283,sizeof(char),40))),_tag_dyneither(_tmp1282,sizeof(void*),
2)))))))))))));}else{const char*_tmp1288;void*_tmp1287[2];struct Cyc_String_pa_struct
_tmp1286;struct Cyc_String_pa_struct _tmp1285;(_tmp1285.tag=0,((_tmp1285.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpB47)),((
_tmp1286.tag=0,((_tmp1286.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpB47)),((_tmp1287[0]=& _tmp1286,((_tmp1287[1]=& _tmp1285,Cyc_Tcutil_terr(loc,((
_tmp1288="expecting union %s instead of struct %s",_tag_dyneither(_tmp1288,
sizeof(char),40))),_tag_dyneither(_tmp1287,sizeof(void*),2)))))))))))));}}if((
unsigned int)_tmpB48  && (int)_tmpB48->v){if(!((unsigned int)_tmpB4E->impl) || !((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpB4E->impl))->tagged){const char*
_tmp128C;void*_tmp128B[1];struct Cyc_String_pa_struct _tmp128A;(_tmp128A.tag=0,((
_tmp128A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpB47)),((_tmp128B[0]=& _tmp128A,Cyc_Tcutil_terr(loc,((_tmp128C="@tagged qualfiers don't agree on union %s",
_tag_dyneither(_tmp128C,sizeof(char),42))),_tag_dyneither(_tmp128B,sizeof(void*),
1)))))));}}*_tmp9AD=Cyc_Absyn_KnownAggr(adp);};_pop_handler();}else{void*_tmpB4C=(
void*)_exn_thrown;void*_tmpB5B=_tmpB4C;_LL58D: {struct Cyc_Dict_Absent_struct*
_tmpB5C=(struct Cyc_Dict_Absent_struct*)_tmpB5B;if(_tmpB5C->tag != Cyc_Dict_Absent)
goto _LL58F;}_LL58E: {struct Cyc_Tcenv_Genv*_tmpB5D=((struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct Cyc_Absyn_Aggrdecl*_tmp128D;struct Cyc_Absyn_Aggrdecl*_tmpB5E=(_tmp128D=
_cycalloc(sizeof(*_tmp128D)),((_tmp128D->kind=_tmpB46,((_tmp128D->sc=Cyc_Absyn_Extern,((
_tmp128D->name=_tmpB47,((_tmp128D->tvs=0,((_tmp128D->impl=0,((_tmp128D->attributes=
0,_tmp128D)))))))))))));Cyc_Tc_tcAggrdecl(te,_tmpB5D,loc,_tmpB5E);adp=Cyc_Tcenv_lookup_aggrdecl(
te,loc,_tmpB47);*_tmp9AD=Cyc_Absyn_KnownAggr(adp);if(*_tmp9AF != 0){{const char*
_tmp1291;void*_tmp1290[1];struct Cyc_String_pa_struct _tmp128F;(_tmp128F.tag=0,((
_tmp128F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpB47)),((_tmp1290[0]=& _tmp128F,Cyc_Tcutil_terr(loc,((_tmp1291="declare parameterized type %s before using",
_tag_dyneither(_tmp1291,sizeof(char),43))),_tag_dyneither(_tmp1290,sizeof(void*),
1)))))));}return cvtenv;}goto _LL58C;}_LL58F:;_LL590:(void)_throw(_tmpB5B);_LL58C:;}}}
_tmpB4A=*adp;goto _LL58B;}_LL58A: if((_tmpB44.KnownAggr).tag != 2)goto _LL587;
_tmpB49=(struct Cyc_Absyn_Aggrdecl**)(_tmpB44.KnownAggr).val;_tmpB4A=*_tmpB49;
_LL58B: {struct Cyc_List_List*tvs=_tmpB4A->tvs;struct Cyc_List_List*ts=*_tmp9AF;
for(0;ts != 0  && tvs != 0;(ts=ts->tl,tvs=tvs->tl)){struct Cyc_Absyn_Tvar*_tmpB63=(
struct Cyc_Absyn_Tvar*)tvs->hd;void*_tmpB64=(void*)ts->hd;{struct _tuple0 _tmp1292;
struct _tuple0 _tmpB66=(_tmp1292.f1=Cyc_Absyn_compress_kb(_tmpB63->kind),((
_tmp1292.f2=_tmpB64,_tmp1292)));void*_tmpB67;void*_tmpB69;struct Cyc_Absyn_Tvar*
_tmpB6B;_LL592: _tmpB67=_tmpB66.f1;{struct Cyc_Absyn_Unknown_kb_struct*_tmpB68=(
struct Cyc_Absyn_Unknown_kb_struct*)_tmpB67;if(_tmpB68->tag != 1)goto _LL594;}
_tmpB69=_tmpB66.f2;{struct Cyc_Absyn_VarType_struct*_tmpB6A=(struct Cyc_Absyn_VarType_struct*)
_tmpB69;if(_tmpB6A->tag != 2)goto _LL594;else{_tmpB6B=_tmpB6A->f1;}}_LL593: cvtenv.kind_env=
Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmpB6B);cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(
cvtenv.r,cvtenv.free_vars,_tmpB6B,1);continue;_LL594:;_LL595: goto _LL591;_LL591:;}{
enum Cyc_Absyn_Kind k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd,Cyc_Absyn_BoxKind);
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,(void*)ts->hd,1);}}if(ts != 
0){const char*_tmp1296;void*_tmp1295[1];struct Cyc_String_pa_struct _tmp1294;(
_tmp1294.tag=0,((_tmp1294.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpB4A->name)),((_tmp1295[0]=& _tmp1294,Cyc_Tcutil_terr(loc,((_tmp1296="too many parameters for type %s",
_tag_dyneither(_tmp1296,sizeof(char),32))),_tag_dyneither(_tmp1295,sizeof(void*),
1)))))));}if(tvs != 0){struct Cyc_List_List*hidden_ts=0;for(0;tvs != 0;tvs=tvs->tl){
enum Cyc_Absyn_Kind k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd,Cyc_Absyn_BoxKind);
void*e=Cyc_Absyn_new_evar(0,0);{struct Cyc_List_List*_tmp1297;hidden_ts=((
_tmp1297=_cycalloc(sizeof(*_tmp1297)),((_tmp1297->hd=e,((_tmp1297->tl=hidden_ts,
_tmp1297))))));}cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,e,1);}*
_tmp9AF=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_imp_append)(*_tmp9AF,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(hidden_ts));}}_LL587:;}goto _LL4AA;_LL4CF: {struct Cyc_Absyn_TypedefType_struct*
_tmp9B0=(struct Cyc_Absyn_TypedefType_struct*)_tmp965;if(_tmp9B0->tag != 18)goto
_LL4D1;else{_tmp9B1=_tmp9B0->f1;_tmp9B2=_tmp9B0->f2;_tmp9B3=(struct Cyc_List_List**)&
_tmp9B0->f2;_tmp9B4=_tmp9B0->f3;_tmp9B5=(struct Cyc_Absyn_Typedefdecl**)& _tmp9B0->f3;
_tmp9B6=_tmp9B0->f4;_tmp9B7=(void***)& _tmp9B0->f4;}}_LL4D0: {struct Cyc_List_List*
targs=*_tmp9B3;struct Cyc_Absyn_Typedefdecl*td;{struct _handler_cons _tmpB70;
_push_handler(& _tmpB70);{int _tmpB72=0;if(setjmp(_tmpB70.handler))_tmpB72=1;if(!
_tmpB72){td=Cyc_Tcenv_lookup_typedefdecl(te,loc,_tmp9B1);;_pop_handler();}else{
void*_tmpB71=(void*)_exn_thrown;void*_tmpB74=_tmpB71;_LL597: {struct Cyc_Dict_Absent_struct*
_tmpB75=(struct Cyc_Dict_Absent_struct*)_tmpB74;if(_tmpB75->tag != Cyc_Dict_Absent)
goto _LL599;}_LL598:{const char*_tmp129B;void*_tmp129A[1];struct Cyc_String_pa_struct
_tmp1299;(_tmp1299.tag=0,((_tmp1299.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp9B1)),((_tmp129A[0]=& _tmp1299,Cyc_Tcutil_terr(
loc,((_tmp129B="unbound typedef name %s",_tag_dyneither(_tmp129B,sizeof(char),24))),
_tag_dyneither(_tmp129A,sizeof(void*),1)))))));}return cvtenv;_LL599:;_LL59A:(
void)_throw(_tmpB74);_LL596:;}}}*_tmp9B5=(struct Cyc_Absyn_Typedefdecl*)td;
_tmp9B1[0]=(td->name)[_check_known_subscript_notnull(1,0)];{struct Cyc_List_List*
tvs=td->tvs;struct Cyc_List_List*ts=targs;struct _RegionHandle*_tmpB79=Cyc_Tcenv_get_fnrgn(
te);{struct Cyc_List_List*inst=0;for(0;ts != 0  && tvs != 0;(ts=ts->tl,tvs=tvs->tl)){
enum Cyc_Absyn_Kind k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd,Cyc_Absyn_BoxKind);
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,(void*)ts->hd,1);{struct
_tuple14*_tmp129E;struct Cyc_List_List*_tmp129D;inst=((_tmp129D=_region_malloc(
_tmpB79,sizeof(*_tmp129D)),((_tmp129D->hd=((_tmp129E=_region_malloc(_tmpB79,
sizeof(*_tmp129E)),((_tmp129E->f1=(struct Cyc_Absyn_Tvar*)tvs->hd,((_tmp129E->f2=(
void*)ts->hd,_tmp129E)))))),((_tmp129D->tl=inst,_tmp129D))))));}}if(ts != 0){
const char*_tmp12A2;void*_tmp12A1[1];struct Cyc_String_pa_struct _tmp12A0;(_tmp12A0.tag=
0,((_tmp12A0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp9B1)),((_tmp12A1[0]=& _tmp12A0,Cyc_Tcutil_terr(loc,((_tmp12A2="too many parameters for typedef %s",
_tag_dyneither(_tmp12A2,sizeof(char),35))),_tag_dyneither(_tmp12A1,sizeof(void*),
1)))))));}if(tvs != 0){struct Cyc_List_List*hidden_ts=0;for(0;tvs != 0;tvs=tvs->tl){
enum Cyc_Absyn_Kind k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd,Cyc_Absyn_BoxKind);
void*e=Cyc_Absyn_new_evar(0,0);{struct Cyc_List_List*_tmp12A3;hidden_ts=((
_tmp12A3=_cycalloc(sizeof(*_tmp12A3)),((_tmp12A3->hd=e,((_tmp12A3->tl=hidden_ts,
_tmp12A3))))));}cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,e,1);{
struct _tuple14*_tmp12A6;struct Cyc_List_List*_tmp12A5;inst=(struct Cyc_List_List*)((
_tmp12A5=_cycalloc(sizeof(*_tmp12A5)),((_tmp12A5->hd=(struct _tuple14*)((_tmp12A6=
_cycalloc(sizeof(*_tmp12A6)),((_tmp12A6->f1=(struct Cyc_Absyn_Tvar*)tvs->hd,((
_tmp12A6->f2=e,_tmp12A6)))))),((_tmp12A5->tl=inst,_tmp12A5))))));}}*_tmp9B3=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(
targs,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
hidden_ts));}if(td->defn != 0){void*new_typ=Cyc_Tcutil_rsubstitute(_tmpB79,inst,(
void*)((struct Cyc_Core_Opt*)_check_null(td->defn))->v);void**_tmp12A7;*_tmp9B7=((
_tmp12A7=_cycalloc(sizeof(*_tmp12A7)),((_tmp12A7[0]=new_typ,_tmp12A7))));}}goto
_LL4AA;}}_LL4D1: {struct Cyc_Absyn_UniqueRgn_struct*_tmp9B8=(struct Cyc_Absyn_UniqueRgn_struct*)
_tmp965;if(_tmp9B8->tag != 22)goto _LL4D3;}_LL4D2: goto _LL4D4;_LL4D3: {struct Cyc_Absyn_HeapRgn_struct*
_tmp9B9=(struct Cyc_Absyn_HeapRgn_struct*)_tmp965;if(_tmp9B9->tag != 21)goto _LL4D5;}
_LL4D4: goto _LL4AA;_LL4D5: {struct Cyc_Absyn_RgnHandleType_struct*_tmp9BA=(struct
Cyc_Absyn_RgnHandleType_struct*)_tmp965;if(_tmp9BA->tag != 16)goto _LL4D7;else{
_tmp9BB=(void*)_tmp9BA->f1;}}_LL4D6: cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,
cvtenv,Cyc_Absyn_TopRgnKind,_tmp9BB,1);goto _LL4AA;_LL4D7: {struct Cyc_Absyn_DynRgnType_struct*
_tmp9BC=(struct Cyc_Absyn_DynRgnType_struct*)_tmp965;if(_tmp9BC->tag != 17)goto
_LL4D9;else{_tmp9BD=(void*)_tmp9BC->f1;_tmp9BE=(void*)_tmp9BC->f2;}}_LL4D8:
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,Cyc_Absyn_RgnKind,_tmp9BD,0);
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,Cyc_Absyn_RgnKind,_tmp9BE,1);
goto _LL4AA;_LL4D9: {struct Cyc_Absyn_AccessEff_struct*_tmp9BF=(struct Cyc_Absyn_AccessEff_struct*)
_tmp965;if(_tmp9BF->tag != 23)goto _LL4DB;else{_tmp9C0=(void*)_tmp9BF->f1;}}_LL4DA:
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,Cyc_Absyn_TopRgnKind,_tmp9C0,
1);goto _LL4AA;_LL4DB: {struct Cyc_Absyn_RgnsEff_struct*_tmp9C1=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp965;if(_tmp9C1->tag != 25)goto _LL4DD;else{_tmp9C2=(void*)_tmp9C1->f1;}}_LL4DC:
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,Cyc_Absyn_AnyKind,_tmp9C2,1);
goto _LL4AA;_LL4DD: {struct Cyc_Absyn_JoinEff_struct*_tmp9C3=(struct Cyc_Absyn_JoinEff_struct*)
_tmp965;if(_tmp9C3->tag != 24)goto _LL4AA;else{_tmp9C4=_tmp9C3->f1;}}_LL4DE: for(0;
_tmp9C4 != 0;_tmp9C4=_tmp9C4->tl){cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,
cvtenv,Cyc_Absyn_EffKind,(void*)_tmp9C4->hd,1);}goto _LL4AA;_LL4AA:;}if(!Cyc_Tcutil_kind_leq(
Cyc_Tcutil_typ_kind(t),expected_kind)){{void*_tmpB83=t;struct Cyc_Core_Opt*
_tmpB85;struct Cyc_Core_Opt*_tmpB86;_LL59C: {struct Cyc_Absyn_Evar_struct*_tmpB84=(
struct Cyc_Absyn_Evar_struct*)_tmpB83;if(_tmpB84->tag != 1)goto _LL59E;else{_tmpB85=
_tmpB84->f1;_tmpB86=_tmpB84->f2;}}_LL59D: {struct _dyneither_ptr s;{struct Cyc_Core_Opt*
_tmpB87=_tmpB85;struct Cyc_Core_Opt _tmpB88;enum Cyc_Absyn_Kind _tmpB89;_LL5A1: if(
_tmpB87 != 0)goto _LL5A3;_LL5A2:{const char*_tmp12A8;s=((_tmp12A8="kind=NULL ",
_tag_dyneither(_tmp12A8,sizeof(char),11)));}goto _LL5A0;_LL5A3: if(_tmpB87 == 0)
goto _LL5A0;_tmpB88=*_tmpB87;_tmpB89=(enum Cyc_Absyn_Kind)_tmpB88.v;_LL5A4:{const
char*_tmp12AC;void*_tmp12AB[1];struct Cyc_String_pa_struct _tmp12AA;s=(struct
_dyneither_ptr)((_tmp12AA.tag=0,((_tmp12AA.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_kind2string(_tmpB89)),((_tmp12AB[0]=& _tmp12AA,Cyc_aprintf(((
_tmp12AC="kind=%s ",_tag_dyneither(_tmp12AC,sizeof(char),9))),_tag_dyneither(
_tmp12AB,sizeof(void*),1))))))));}goto _LL5A0;_LL5A0:;}{struct Cyc_Core_Opt*
_tmpB8E=_tmpB86;struct Cyc_Core_Opt _tmpB8F;void*_tmpB90;_LL5A6: if(_tmpB8E != 0)
goto _LL5A8;_LL5A7:{const char*_tmp12B0;void*_tmp12AF[1];struct Cyc_String_pa_struct
_tmp12AE;s=(struct _dyneither_ptr)((_tmp12AE.tag=0,((_tmp12AE.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)s),((_tmp12AF[0]=& _tmp12AE,Cyc_aprintf(((
_tmp12B0="%s ref=NULL",_tag_dyneither(_tmp12B0,sizeof(char),12))),_tag_dyneither(
_tmp12AF,sizeof(void*),1))))))));}goto _LL5A5;_LL5A8: if(_tmpB8E == 0)goto _LL5A5;
_tmpB8F=*_tmpB8E;_tmpB90=(void*)_tmpB8F.v;_LL5A9:{const char*_tmp12B5;void*
_tmp12B4[2];struct Cyc_String_pa_struct _tmp12B3;struct Cyc_String_pa_struct
_tmp12B2;s=(struct _dyneither_ptr)((_tmp12B2.tag=0,((_tmp12B2.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmpB90)),((
_tmp12B3.tag=0,((_tmp12B3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((
_tmp12B4[0]=& _tmp12B3,((_tmp12B4[1]=& _tmp12B2,Cyc_aprintf(((_tmp12B5="%s ref=%s",
_tag_dyneither(_tmp12B5,sizeof(char),10))),_tag_dyneither(_tmp12B4,sizeof(void*),
2))))))))))))));}goto _LL5A5;_LL5A5:;}{const char*_tmp12B9;void*_tmp12B8[1];struct
Cyc_String_pa_struct _tmp12B7;(_tmp12B7.tag=0,((_tmp12B7.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)s),((_tmp12B8[0]=& _tmp12B7,Cyc_fprintf(Cyc_stderr,((
_tmp12B9="evar info: %s\n",_tag_dyneither(_tmp12B9,sizeof(char),15))),
_tag_dyneither(_tmp12B8,sizeof(void*),1)))))));}goto _LL59B;}_LL59E:;_LL59F: goto
_LL59B;_LL59B:;}{const char*_tmp12BF;void*_tmp12BE[3];struct Cyc_String_pa_struct
_tmp12BD;struct Cyc_String_pa_struct _tmp12BC;struct Cyc_String_pa_struct _tmp12BB;(
_tmp12BB.tag=0,((_tmp12BB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(
expected_kind)),((_tmp12BC.tag=0,((_tmp12BC.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_kind2string(Cyc_Tcutil_typ_kind(t))),((_tmp12BD.tag=0,((
_tmp12BD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t)),((_tmp12BE[0]=& _tmp12BD,((_tmp12BE[1]=& _tmp12BC,((_tmp12BE[2]=& _tmp12BB,Cyc_Tcutil_terr(
loc,((_tmp12BF="type %s has kind %s but as used here needs kind %s",
_tag_dyneither(_tmp12BF,sizeof(char),51))),_tag_dyneither(_tmp12BE,sizeof(void*),
3)))))))))))))))))));}}return cvtenv;}static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type_level_exp(
struct Cyc_Absyn_Exp*e,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv);
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type_level_exp(struct Cyc_Absyn_Exp*
e,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv){{void*_tmpBA0=e->r;
struct Cyc_List_List*_tmpBA6;struct Cyc_Absyn_Exp*_tmpBA8;struct Cyc_Absyn_Exp*
_tmpBA9;struct Cyc_Absyn_Exp*_tmpBAA;struct Cyc_Absyn_Exp*_tmpBAC;struct Cyc_Absyn_Exp*
_tmpBAD;struct Cyc_Absyn_Exp*_tmpBAF;struct Cyc_Absyn_Exp*_tmpBB0;struct Cyc_Absyn_Exp*
_tmpBB2;struct Cyc_Absyn_Exp*_tmpBB3;void*_tmpBB5;struct Cyc_Absyn_Exp*_tmpBB6;
void*_tmpBB8;void*_tmpBBA;void*_tmpBBC;struct Cyc_Absyn_Exp*_tmpBBE;_LL5AB: {
struct Cyc_Absyn_Const_e_struct*_tmpBA1=(struct Cyc_Absyn_Const_e_struct*)_tmpBA0;
if(_tmpBA1->tag != 0)goto _LL5AD;}_LL5AC: goto _LL5AE;_LL5AD: {struct Cyc_Absyn_Enum_e_struct*
_tmpBA2=(struct Cyc_Absyn_Enum_e_struct*)_tmpBA0;if(_tmpBA2->tag != 33)goto _LL5AF;}
_LL5AE: goto _LL5B0;_LL5AF: {struct Cyc_Absyn_AnonEnum_e_struct*_tmpBA3=(struct Cyc_Absyn_AnonEnum_e_struct*)
_tmpBA0;if(_tmpBA3->tag != 34)goto _LL5B1;}_LL5B0: goto _LL5B2;_LL5B1: {struct Cyc_Absyn_Var_e_struct*
_tmpBA4=(struct Cyc_Absyn_Var_e_struct*)_tmpBA0;if(_tmpBA4->tag != 1)goto _LL5B3;}
_LL5B2: goto _LL5AA;_LL5B3: {struct Cyc_Absyn_Primop_e_struct*_tmpBA5=(struct Cyc_Absyn_Primop_e_struct*)
_tmpBA0;if(_tmpBA5->tag != 3)goto _LL5B5;else{_tmpBA6=_tmpBA5->f2;}}_LL5B4: for(0;
_tmpBA6 != 0;_tmpBA6=_tmpBA6->tl){cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp((
struct Cyc_Absyn_Exp*)_tmpBA6->hd,te,cvtenv);}goto _LL5AA;_LL5B5: {struct Cyc_Absyn_Conditional_e_struct*
_tmpBA7=(struct Cyc_Absyn_Conditional_e_struct*)_tmpBA0;if(_tmpBA7->tag != 6)goto
_LL5B7;else{_tmpBA8=_tmpBA7->f1;_tmpBA9=_tmpBA7->f2;_tmpBAA=_tmpBA7->f3;}}_LL5B6:
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpBA8,te,cvtenv);cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(
_tmpBA9,te,cvtenv);cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpBAA,te,
cvtenv);goto _LL5AA;_LL5B7: {struct Cyc_Absyn_And_e_struct*_tmpBAB=(struct Cyc_Absyn_And_e_struct*)
_tmpBA0;if(_tmpBAB->tag != 7)goto _LL5B9;else{_tmpBAC=_tmpBAB->f1;_tmpBAD=_tmpBAB->f2;}}
_LL5B8: _tmpBAF=_tmpBAC;_tmpBB0=_tmpBAD;goto _LL5BA;_LL5B9: {struct Cyc_Absyn_Or_e_struct*
_tmpBAE=(struct Cyc_Absyn_Or_e_struct*)_tmpBA0;if(_tmpBAE->tag != 8)goto _LL5BB;
else{_tmpBAF=_tmpBAE->f1;_tmpBB0=_tmpBAE->f2;}}_LL5BA: _tmpBB2=_tmpBAF;_tmpBB3=
_tmpBB0;goto _LL5BC;_LL5BB: {struct Cyc_Absyn_SeqExp_e_struct*_tmpBB1=(struct Cyc_Absyn_SeqExp_e_struct*)
_tmpBA0;if(_tmpBB1->tag != 9)goto _LL5BD;else{_tmpBB2=_tmpBB1->f1;_tmpBB3=_tmpBB1->f2;}}
_LL5BC: cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpBB2,te,cvtenv);cvtenv=
Cyc_Tcutil_i_check_valid_type_level_exp(_tmpBB3,te,cvtenv);goto _LL5AA;_LL5BD: {
struct Cyc_Absyn_Cast_e_struct*_tmpBB4=(struct Cyc_Absyn_Cast_e_struct*)_tmpBA0;
if(_tmpBB4->tag != 15)goto _LL5BF;else{_tmpBB5=(void*)_tmpBB4->f1;_tmpBB6=_tmpBB4->f2;}}
_LL5BE: cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpBB6,te,cvtenv);cvtenv=
Cyc_Tcutil_i_check_valid_type(e->loc,te,cvtenv,Cyc_Absyn_AnyKind,_tmpBB5,1);goto
_LL5AA;_LL5BF: {struct Cyc_Absyn_Offsetof_e_struct*_tmpBB7=(struct Cyc_Absyn_Offsetof_e_struct*)
_tmpBA0;if(_tmpBB7->tag != 20)goto _LL5C1;else{_tmpBB8=(void*)_tmpBB7->f1;}}_LL5C0:
_tmpBBA=_tmpBB8;goto _LL5C2;_LL5C1: {struct Cyc_Absyn_Sizeoftyp_e_struct*_tmpBB9=(
struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmpBA0;if(_tmpBB9->tag != 18)goto _LL5C3;
else{_tmpBBA=(void*)_tmpBB9->f1;}}_LL5C2: cvtenv=Cyc_Tcutil_i_check_valid_type(e->loc,
te,cvtenv,Cyc_Absyn_AnyKind,_tmpBBA,1);goto _LL5AA;_LL5C3: {struct Cyc_Absyn_Valueof_e_struct*
_tmpBBB=(struct Cyc_Absyn_Valueof_e_struct*)_tmpBA0;if(_tmpBBB->tag != 40)goto
_LL5C5;else{_tmpBBC=(void*)_tmpBBB->f1;}}_LL5C4: cvtenv=Cyc_Tcutil_i_check_valid_type(
e->loc,te,cvtenv,Cyc_Absyn_IntKind,_tmpBBC,1);goto _LL5AA;_LL5C5: {struct Cyc_Absyn_Sizeofexp_e_struct*
_tmpBBD=(struct Cyc_Absyn_Sizeofexp_e_struct*)_tmpBA0;if(_tmpBBD->tag != 19)goto
_LL5C7;else{_tmpBBE=_tmpBBD->f1;}}_LL5C6: cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(
_tmpBBE,te,cvtenv);goto _LL5AA;_LL5C7:;_LL5C8: {const char*_tmp12C2;void*_tmp12C1;(
_tmp12C1=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp12C2="non-type-level-expression in Tcutil::i_check_valid_type_level_exp",
_tag_dyneither(_tmp12C2,sizeof(char),66))),_tag_dyneither(_tmp12C1,sizeof(void*),
0)));}_LL5AA:;}return cvtenv;}static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_check_valid_type(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv
cvt,enum Cyc_Absyn_Kind expected_kind,void*t);static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_check_valid_type(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv
cvt,enum Cyc_Absyn_Kind expected_kind,void*t){struct Cyc_List_List*_tmpBC1=cvt.kind_env;
cvt=Cyc_Tcutil_i_check_valid_type(loc,te,cvt,expected_kind,t,1);{struct Cyc_List_List*
vs=cvt.free_vars;for(0;vs != 0;vs=vs->tl){struct Cyc_Absyn_Tvar*_tmpBC3;struct
_tuple24 _tmpBC2=*((struct _tuple24*)vs->hd);_tmpBC3=_tmpBC2.f1;cvt.kind_env=Cyc_Tcutil_fast_add_free_tvar(
_tmpBC1,_tmpBC3);}}{struct Cyc_List_List*evs=cvt.free_evars;for(0;evs != 0;evs=evs->tl){
void*_tmpBC5;struct _tuple25 _tmpBC4=*((struct _tuple25*)evs->hd);_tmpBC5=_tmpBC4.f1;{
void*_tmpBC6=Cyc_Tcutil_compress(_tmpBC5);struct Cyc_Core_Opt*_tmpBC8;struct Cyc_Core_Opt**
_tmpBC9;_LL5CA: {struct Cyc_Absyn_Evar_struct*_tmpBC7=(struct Cyc_Absyn_Evar_struct*)
_tmpBC6;if(_tmpBC7->tag != 1)goto _LL5CC;else{_tmpBC8=_tmpBC7->f4;_tmpBC9=(struct
Cyc_Core_Opt**)& _tmpBC7->f4;}}_LL5CB: if(*_tmpBC9 == 0){struct Cyc_Core_Opt*
_tmp12C3;*_tmpBC9=((_tmp12C3=_cycalloc(sizeof(*_tmp12C3)),((_tmp12C3->v=_tmpBC1,
_tmp12C3))));}else{struct Cyc_List_List*_tmpBCB=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(*_tmpBC9))->v;struct Cyc_List_List*_tmpBCC=0;for(0;_tmpBCB != 0;
_tmpBCB=_tmpBCB->tl){if(((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,
_tmpBC1,(struct Cyc_Absyn_Tvar*)_tmpBCB->hd)){struct Cyc_List_List*_tmp12C4;
_tmpBCC=((_tmp12C4=_cycalloc(sizeof(*_tmp12C4)),((_tmp12C4->hd=(struct Cyc_Absyn_Tvar*)
_tmpBCB->hd,((_tmp12C4->tl=_tmpBCC,_tmp12C4))))));}}{struct Cyc_Core_Opt*_tmp12C5;*
_tmpBC9=((_tmp12C5=_cycalloc(sizeof(*_tmp12C5)),((_tmp12C5->v=_tmpBCC,_tmp12C5))));}}
goto _LL5C9;_LL5CC:;_LL5CD: goto _LL5C9;_LL5C9:;}}}return cvt;}void Cyc_Tcutil_check_valid_toplevel_type(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,void*t);void Cyc_Tcutil_check_valid_toplevel_type(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,void*t){int
generalize_evars=Cyc_Tcutil_is_function_type(t);struct Cyc_List_List*_tmpBCF=Cyc_Tcenv_lookup_type_vars(
te);struct _RegionHandle*_tmpBD0=Cyc_Tcenv_get_fnrgn(te);struct Cyc_Tcutil_CVTEnv
_tmp12C6;struct Cyc_Tcutil_CVTEnv _tmpBD1=Cyc_Tcutil_check_valid_type(loc,te,((
_tmp12C6.r=_tmpBD0,((_tmp12C6.kind_env=_tmpBCF,((_tmp12C6.free_vars=0,((_tmp12C6.free_evars=
0,((_tmp12C6.generalize_evars=generalize_evars,((_tmp12C6.fn_result=0,_tmp12C6)))))))))))),
Cyc_Absyn_MemKind,t);struct Cyc_List_List*_tmpBD2=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_Absyn_Tvar*(*f)(struct _tuple24*),struct Cyc_List_List*
x))Cyc_List_rmap)(_tmpBD0,(struct Cyc_Absyn_Tvar*(*)(struct _tuple24*))Cyc_Core_fst,
_tmpBD1.free_vars);struct Cyc_List_List*_tmpBD3=((struct Cyc_List_List*(*)(struct
_RegionHandle*,void*(*f)(struct _tuple25*),struct Cyc_List_List*x))Cyc_List_rmap)(
_tmpBD0,(void*(*)(struct _tuple25*))Cyc_Core_fst,_tmpBD1.free_evars);if(_tmpBCF != 
0){struct Cyc_List_List*_tmpBD4=0;{struct Cyc_List_List*_tmpBD5=_tmpBD2;for(0;(
unsigned int)_tmpBD5;_tmpBD5=_tmpBD5->tl){struct Cyc_Absyn_Tvar*_tmpBD6=(struct
Cyc_Absyn_Tvar*)_tmpBD5->hd;int found=0;{struct Cyc_List_List*_tmpBD7=_tmpBCF;for(
0;(unsigned int)_tmpBD7;_tmpBD7=_tmpBD7->tl){if(Cyc_Absyn_tvar_cmp(_tmpBD6,(
struct Cyc_Absyn_Tvar*)_tmpBD7->hd)== 0){found=1;break;}}}if(!found){struct Cyc_List_List*
_tmp12C7;_tmpBD4=((_tmp12C7=_region_malloc(_tmpBD0,sizeof(*_tmp12C7)),((_tmp12C7->hd=(
struct Cyc_Absyn_Tvar*)_tmpBD5->hd,((_tmp12C7->tl=_tmpBD4,_tmp12C7))))));}}}
_tmpBD2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmpBD4);}{struct Cyc_List_List*x=_tmpBD2;for(0;x != 0;x=x->tl){void*_tmpBD9=Cyc_Absyn_compress_kb(((
struct Cyc_Absyn_Tvar*)x->hd)->kind);struct Cyc_Core_Opt*_tmpBDB;struct Cyc_Core_Opt**
_tmpBDC;struct Cyc_Core_Opt*_tmpBDE;struct Cyc_Core_Opt**_tmpBDF;enum Cyc_Absyn_Kind
_tmpBE0;struct Cyc_Core_Opt*_tmpBE2;struct Cyc_Core_Opt**_tmpBE3;enum Cyc_Absyn_Kind
_tmpBE4;struct Cyc_Core_Opt*_tmpBE6;struct Cyc_Core_Opt**_tmpBE7;enum Cyc_Absyn_Kind
_tmpBE8;enum Cyc_Absyn_Kind _tmpBEA;_LL5CF: {struct Cyc_Absyn_Unknown_kb_struct*
_tmpBDA=(struct Cyc_Absyn_Unknown_kb_struct*)_tmpBD9;if(_tmpBDA->tag != 1)goto
_LL5D1;else{_tmpBDB=_tmpBDA->f1;_tmpBDC=(struct Cyc_Core_Opt**)& _tmpBDA->f1;}}
_LL5D0: _tmpBDF=_tmpBDC;goto _LL5D2;_LL5D1: {struct Cyc_Absyn_Less_kb_struct*
_tmpBDD=(struct Cyc_Absyn_Less_kb_struct*)_tmpBD9;if(_tmpBDD->tag != 2)goto _LL5D3;
else{_tmpBDE=_tmpBDD->f1;_tmpBDF=(struct Cyc_Core_Opt**)& _tmpBDD->f1;_tmpBE0=
_tmpBDD->f2;if(_tmpBE0 != Cyc_Absyn_MemKind)goto _LL5D3;}}_LL5D2:*_tmpBDF=Cyc_Tcutil_kind_to_bound_opt(
Cyc_Absyn_BoxKind);goto _LL5CE;_LL5D3: {struct Cyc_Absyn_Less_kb_struct*_tmpBE1=(
struct Cyc_Absyn_Less_kb_struct*)_tmpBD9;if(_tmpBE1->tag != 2)goto _LL5D5;else{
_tmpBE2=_tmpBE1->f1;_tmpBE3=(struct Cyc_Core_Opt**)& _tmpBE1->f1;_tmpBE4=_tmpBE1->f2;
if(_tmpBE4 != Cyc_Absyn_TopRgnKind)goto _LL5D5;}}_LL5D4:*_tmpBE3=Cyc_Tcutil_kind_to_bound_opt(
Cyc_Absyn_RgnKind);goto _LL5CE;_LL5D5: {struct Cyc_Absyn_Less_kb_struct*_tmpBE5=(
struct Cyc_Absyn_Less_kb_struct*)_tmpBD9;if(_tmpBE5->tag != 2)goto _LL5D7;else{
_tmpBE6=_tmpBE5->f1;_tmpBE7=(struct Cyc_Core_Opt**)& _tmpBE5->f1;_tmpBE8=_tmpBE5->f2;}}
_LL5D6:*_tmpBE7=Cyc_Tcutil_kind_to_bound_opt(_tmpBE8);goto _LL5CE;_LL5D7: {struct
Cyc_Absyn_Eq_kb_struct*_tmpBE9=(struct Cyc_Absyn_Eq_kb_struct*)_tmpBD9;if(_tmpBE9->tag
!= 0)goto _LL5D9;else{_tmpBEA=_tmpBE9->f1;if(_tmpBEA != Cyc_Absyn_MemKind)goto
_LL5D9;}}_LL5D8:{const char*_tmp12CB;void*_tmp12CA[1];struct Cyc_String_pa_struct
_tmp12C9;(_tmp12C9.tag=0,((_tmp12C9.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Tcutil_tvar2string((struct Cyc_Absyn_Tvar*)x->hd)),((_tmp12CA[
0]=& _tmp12C9,Cyc_Tcutil_terr(loc,((_tmp12CB="type variable %s cannot have kind M",
_tag_dyneither(_tmp12CB,sizeof(char),36))),_tag_dyneither(_tmp12CA,sizeof(void*),
1)))))));}goto _LL5CE;_LL5D9:;_LL5DA: goto _LL5CE;_LL5CE:;}}if(_tmpBD2 != 0  || 
_tmpBD3 != 0){{void*_tmpBEE=Cyc_Tcutil_compress(t);struct Cyc_Absyn_FnInfo _tmpBF0;
struct Cyc_List_List*_tmpBF1;struct Cyc_List_List**_tmpBF2;struct Cyc_Core_Opt*
_tmpBF3;void*_tmpBF4;struct Cyc_List_List*_tmpBF5;int _tmpBF6;struct Cyc_Absyn_VarargInfo*
_tmpBF7;struct Cyc_List_List*_tmpBF8;struct Cyc_List_List*_tmpBF9;_LL5DC: {struct
Cyc_Absyn_FnType_struct*_tmpBEF=(struct Cyc_Absyn_FnType_struct*)_tmpBEE;if(
_tmpBEF->tag != 10)goto _LL5DE;else{_tmpBF0=_tmpBEF->f1;_tmpBF1=_tmpBF0.tvars;
_tmpBF2=(struct Cyc_List_List**)&(_tmpBEF->f1).tvars;_tmpBF3=_tmpBF0.effect;
_tmpBF4=_tmpBF0.ret_typ;_tmpBF5=_tmpBF0.args;_tmpBF6=_tmpBF0.c_varargs;_tmpBF7=
_tmpBF0.cyc_varargs;_tmpBF8=_tmpBF0.rgn_po;_tmpBF9=_tmpBF0.attributes;}}_LL5DD:
if(*_tmpBF2 == 0){*_tmpBF2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_copy)(
_tmpBD2);_tmpBD2=0;}goto _LL5DB;_LL5DE:;_LL5DF: goto _LL5DB;_LL5DB:;}if(_tmpBD2 != 0){
const char*_tmp12CF;void*_tmp12CE[1];struct Cyc_String_pa_struct _tmp12CD;(_tmp12CD.tag=
0,((_tmp12CD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)
_tmpBD2->hd)->name),((_tmp12CE[0]=& _tmp12CD,Cyc_Tcutil_terr(loc,((_tmp12CF="unbound type variable %s",
_tag_dyneither(_tmp12CF,sizeof(char),25))),_tag_dyneither(_tmp12CE,sizeof(void*),
1)))))));}if(_tmpBD3 != 0)for(0;_tmpBD3 != 0;_tmpBD3=_tmpBD3->tl){void*e=(void*)
_tmpBD3->hd;switch(Cyc_Tcutil_typ_kind(e)){case Cyc_Absyn_UniqueRgnKind: _LL5E0:
if(!Cyc_Tcutil_unify(e,(void*)& Cyc_Absyn_UniqueRgn_val)){const char*_tmp12D2;void*
_tmp12D1;(_tmp12D1=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmp12D2="can't unify evar with unique region!",
_tag_dyneither(_tmp12D2,sizeof(char),37))),_tag_dyneither(_tmp12D1,sizeof(void*),
0)));}break;case Cyc_Absyn_TopRgnKind: _LL5E1: goto _LL5E2;case Cyc_Absyn_RgnKind:
_LL5E2: if(!Cyc_Tcutil_unify(e,(void*)& Cyc_Absyn_HeapRgn_val)){const char*_tmp12D5;
void*_tmp12D4;(_tmp12D4=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap))Cyc_Tcutil_impos)(((_tmp12D5="can't unify evar with heap!",_tag_dyneither(
_tmp12D5,sizeof(char),28))),_tag_dyneither(_tmp12D4,sizeof(void*),0)));}break;
case Cyc_Absyn_EffKind: _LL5E3: if(!Cyc_Tcutil_unify(e,Cyc_Absyn_empty_effect)){
const char*_tmp12D8;void*_tmp12D7;(_tmp12D7=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12D8="can't unify evar with {}!",
_tag_dyneither(_tmp12D8,sizeof(char),26))),_tag_dyneither(_tmp12D7,sizeof(void*),
0)));}break;default: _LL5E4:{const char*_tmp12DD;void*_tmp12DC[2];struct Cyc_String_pa_struct
_tmp12DB;struct Cyc_String_pa_struct _tmp12DA;(_tmp12DA.tag=0,((_tmp12DA.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((
_tmp12DB.tag=0,((_tmp12DB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
e)),((_tmp12DC[0]=& _tmp12DB,((_tmp12DC[1]=& _tmp12DA,Cyc_Tcutil_terr(loc,((
_tmp12DD="hidden type variable %s isn't abstracted in type %s",_tag_dyneither(
_tmp12DD,sizeof(char),52))),_tag_dyneither(_tmp12DC,sizeof(void*),2)))))))))))));}
break;}}}}void Cyc_Tcutil_check_fndecl_valid_type(struct Cyc_Position_Segment*loc,
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Fndecl*fd);void Cyc_Tcutil_check_fndecl_valid_type(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Fndecl*fd){
void*t=Cyc_Tcutil_fndecl2typ(fd);Cyc_Tcutil_check_valid_toplevel_type(loc,te,t);{
void*_tmpC08=Cyc_Tcutil_compress(t);struct Cyc_Absyn_FnInfo _tmpC0A;struct Cyc_List_List*
_tmpC0B;struct Cyc_Core_Opt*_tmpC0C;void*_tmpC0D;_LL5E7: {struct Cyc_Absyn_FnType_struct*
_tmpC09=(struct Cyc_Absyn_FnType_struct*)_tmpC08;if(_tmpC09->tag != 10)goto _LL5E9;
else{_tmpC0A=_tmpC09->f1;_tmpC0B=_tmpC0A.tvars;_tmpC0C=_tmpC0A.effect;_tmpC0D=
_tmpC0A.ret_typ;}}_LL5E8: fd->tvs=_tmpC0B;fd->effect=_tmpC0C;goto _LL5E6;_LL5E9:;
_LL5EA: {const char*_tmp12E0;void*_tmp12DF;(_tmp12DF=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12E0="check_fndecl_valid_type: not a FnType",
_tag_dyneither(_tmp12E0,sizeof(char),38))),_tag_dyneither(_tmp12DF,sizeof(void*),
0)));}_LL5E6:;}{struct _RegionHandle*_tmpC10=Cyc_Tcenv_get_fnrgn(te);{const char*
_tmp12E1;Cyc_Tcutil_check_unique_vars(((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _dyneither_ptr*(*f)(struct _tuple20*),struct Cyc_List_List*x))
Cyc_List_rmap)(_tmpC10,(struct _dyneither_ptr*(*)(struct _tuple20*t))Cyc_Tcutil_fst_fdarg,
fd->args),loc,((_tmp12E1="function declaration has repeated parameter",
_tag_dyneither(_tmp12E1,sizeof(char),44))));}{struct Cyc_Core_Opt*_tmp12E2;fd->cached_typ=((
_tmp12E2=_cycalloc(sizeof(*_tmp12E2)),((_tmp12E2->v=t,_tmp12E2))));}}}void Cyc_Tcutil_check_type(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*
bound_tvars,enum Cyc_Absyn_Kind expected_kind,int allow_evars,void*t);void Cyc_Tcutil_check_type(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*
bound_tvars,enum Cyc_Absyn_Kind expected_kind,int allow_evars,void*t){struct
_RegionHandle*_tmpC13=Cyc_Tcenv_get_fnrgn(te);struct Cyc_Tcutil_CVTEnv _tmp12E3;
struct Cyc_Tcutil_CVTEnv _tmpC14=Cyc_Tcutil_check_valid_type(loc,te,((_tmp12E3.r=
_tmpC13,((_tmp12E3.kind_env=bound_tvars,((_tmp12E3.free_vars=0,((_tmp12E3.free_evars=
0,((_tmp12E3.generalize_evars=0,((_tmp12E3.fn_result=0,_tmp12E3)))))))))))),
expected_kind,t);struct Cyc_List_List*_tmpC15=Cyc_Tcutil_remove_bound_tvars(
_tmpC13,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Tvar*(*f)(
struct _tuple24*),struct Cyc_List_List*x))Cyc_List_rmap)(_tmpC13,(struct Cyc_Absyn_Tvar*(*)(
struct _tuple24*))Cyc_Core_fst,_tmpC14.free_vars),bound_tvars);struct Cyc_List_List*
_tmpC16=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _tuple25*),
struct Cyc_List_List*x))Cyc_List_rmap)(_tmpC13,(void*(*)(struct _tuple25*))Cyc_Core_fst,
_tmpC14.free_evars);{struct Cyc_List_List*fs=_tmpC15;for(0;fs != 0;fs=fs->tl){
struct _dyneither_ptr*_tmpC17=((struct Cyc_Absyn_Tvar*)fs->hd)->name;const char*
_tmp12E8;void*_tmp12E7[2];struct Cyc_String_pa_struct _tmp12E6;struct Cyc_String_pa_struct
_tmp12E5;(_tmp12E5.tag=0,((_tmp12E5.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp12E6.tag=0,((_tmp12E6.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*_tmpC17),((_tmp12E7[0]=& _tmp12E6,((
_tmp12E7[1]=& _tmp12E5,Cyc_Tcutil_terr(loc,((_tmp12E8="unbound type variable %s in type %s",
_tag_dyneither(_tmp12E8,sizeof(char),36))),_tag_dyneither(_tmp12E7,sizeof(void*),
2)))))))))))));}}if(!allow_evars  && _tmpC16 != 0)for(0;_tmpC16 != 0;_tmpC16=
_tmpC16->tl){void*e=(void*)_tmpC16->hd;switch(Cyc_Tcutil_typ_kind(e)){case Cyc_Absyn_UniqueRgnKind:
_LL5EB: if(!Cyc_Tcutil_unify(e,(void*)& Cyc_Absyn_UniqueRgn_val)){const char*
_tmp12EB;void*_tmp12EA;(_tmp12EA=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12EB="can't unify evar with unique region!",
_tag_dyneither(_tmp12EB,sizeof(char),37))),_tag_dyneither(_tmp12EA,sizeof(void*),
0)));}break;case Cyc_Absyn_TopRgnKind: _LL5EC: goto _LL5ED;case Cyc_Absyn_RgnKind:
_LL5ED: if(!Cyc_Tcutil_unify(e,(void*)& Cyc_Absyn_HeapRgn_val)){const char*_tmp12EE;
void*_tmp12ED;(_tmp12ED=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap))Cyc_Tcutil_impos)(((_tmp12EE="can't unify evar with heap!",_tag_dyneither(
_tmp12EE,sizeof(char),28))),_tag_dyneither(_tmp12ED,sizeof(void*),0)));}break;
case Cyc_Absyn_EffKind: _LL5EE: if(!Cyc_Tcutil_unify(e,Cyc_Absyn_empty_effect)){
const char*_tmp12F1;void*_tmp12F0;(_tmp12F0=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12F1="can't unify evar with {}!",
_tag_dyneither(_tmp12F1,sizeof(char),26))),_tag_dyneither(_tmp12F0,sizeof(void*),
0)));}break;default: _LL5EF:{const char*_tmp12F6;void*_tmp12F5[2];struct Cyc_String_pa_struct
_tmp12F4;struct Cyc_String_pa_struct _tmp12F3;(_tmp12F3.tag=0,((_tmp12F3.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((
_tmp12F4.tag=0,((_tmp12F4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
e)),((_tmp12F5[0]=& _tmp12F4,((_tmp12F5[1]=& _tmp12F3,Cyc_Tcutil_terr(loc,((
_tmp12F6="hidden type variable %s isn't abstracted in type %s",_tag_dyneither(
_tmp12F6,sizeof(char),52))),_tag_dyneither(_tmp12F5,sizeof(void*),2)))))))))))));}
break;}}}void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*tv);void Cyc_Tcutil_add_tvar_identity(
struct Cyc_Absyn_Tvar*tv){if(tv->identity == - 1)tv->identity=Cyc_Tcutil_new_tvar_id();}
void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*tvs);void Cyc_Tcutil_add_tvar_identities(
struct Cyc_List_List*tvs){((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*
x))Cyc_List_iter)(Cyc_Tcutil_add_tvar_identity,tvs);}static void Cyc_Tcutil_check_unique_unsorted(
int(*cmp)(void*,void*),struct Cyc_List_List*vs,struct Cyc_Position_Segment*loc,
struct _dyneither_ptr(*a2string)(void*),struct _dyneither_ptr msg);static void Cyc_Tcutil_check_unique_unsorted(
int(*cmp)(void*,void*),struct Cyc_List_List*vs,struct Cyc_Position_Segment*loc,
struct _dyneither_ptr(*a2string)(void*),struct _dyneither_ptr msg){for(0;vs != 0;vs=
vs->tl){struct Cyc_List_List*vs2=vs->tl;for(0;vs2 != 0;vs2=vs2->tl){if(cmp((void*)
vs->hd,(void*)vs2->hd)== 0){const char*_tmp12FB;void*_tmp12FA[2];struct Cyc_String_pa_struct
_tmp12F9;struct Cyc_String_pa_struct _tmp12F8;(_tmp12F8.tag=0,((_tmp12F8.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)a2string((void*)vs->hd)),((_tmp12F9.tag=
0,((_tmp12F9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg),((_tmp12FA[0]=&
_tmp12F9,((_tmp12FA[1]=& _tmp12F8,Cyc_Tcutil_terr(loc,((_tmp12FB="%s: %s",
_tag_dyneither(_tmp12FB,sizeof(char),7))),_tag_dyneither(_tmp12FA,sizeof(void*),
2)))))))))))));}}}}static struct _dyneither_ptr Cyc_Tcutil_strptr2string(struct
_dyneither_ptr*s);static struct _dyneither_ptr Cyc_Tcutil_strptr2string(struct
_dyneither_ptr*s){return*s;}void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*
vs,struct Cyc_Position_Segment*loc,struct _dyneither_ptr msg);void Cyc_Tcutil_check_unique_vars(
struct Cyc_List_List*vs,struct Cyc_Position_Segment*loc,struct _dyneither_ptr msg){((
void(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*
vs,struct Cyc_Position_Segment*loc,struct _dyneither_ptr(*a2string)(struct
_dyneither_ptr*),struct _dyneither_ptr msg))Cyc_Tcutil_check_unique_unsorted)(Cyc_strptrcmp,
vs,loc,Cyc_Tcutil_strptr2string,msg);}void Cyc_Tcutil_check_unique_tvars(struct
Cyc_Position_Segment*loc,struct Cyc_List_List*tvs);void Cyc_Tcutil_check_unique_tvars(
struct Cyc_Position_Segment*loc,struct Cyc_List_List*tvs){const char*_tmp12FC;((
void(*)(int(*cmp)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*
vs,struct Cyc_Position_Segment*loc,struct _dyneither_ptr(*a2string)(struct Cyc_Absyn_Tvar*),
struct _dyneither_ptr msg))Cyc_Tcutil_check_unique_unsorted)(Cyc_Absyn_tvar_cmp,
tvs,loc,Cyc_Tcutil_tvar2string,((_tmp12FC="duplicate type variable",
_tag_dyneither(_tmp12FC,sizeof(char),24))));}struct _tuple27{struct Cyc_Absyn_Aggrfield*
f1;int f2;};struct _tuple28{struct Cyc_List_List*f1;void*f2;};struct _tuple29{struct
Cyc_Absyn_Aggrfield*f1;void*f2;};struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(
struct _RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,
enum Cyc_Absyn_AggrKind aggr_kind,struct Cyc_List_List*sdfields);struct Cyc_List_List*
Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*rgn,struct Cyc_Position_Segment*
loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind aggr_kind,struct Cyc_List_List*
sdfields){struct Cyc_List_List*fields=0;{struct Cyc_List_List*sd_fields=sdfields;
for(0;sd_fields != 0;sd_fields=sd_fields->tl){const char*_tmp12FD;if(Cyc_strcmp((
struct _dyneither_ptr)*((struct Cyc_Absyn_Aggrfield*)sd_fields->hd)->name,((
_tmp12FD="",_tag_dyneither(_tmp12FD,sizeof(char),1))))!= 0){struct _tuple27*
_tmp1300;struct Cyc_List_List*_tmp12FF;fields=((_tmp12FF=_cycalloc(sizeof(*
_tmp12FF)),((_tmp12FF->hd=((_tmp1300=_cycalloc(sizeof(*_tmp1300)),((_tmp1300->f1=(
struct Cyc_Absyn_Aggrfield*)sd_fields->hd,((_tmp1300->f2=0,_tmp1300)))))),((
_tmp12FF->tl=fields,_tmp12FF))))));}}}fields=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(fields);{const char*_tmp1302;const char*_tmp1301;struct
_dyneither_ptr aggr_str=aggr_kind == Cyc_Absyn_StructA?(_tmp1302="struct",
_tag_dyneither(_tmp1302,sizeof(char),7)):((_tmp1301="union",_tag_dyneither(
_tmp1301,sizeof(char),6)));struct Cyc_List_List*ans=0;for(0;des != 0;des=des->tl){
struct _tuple28 _tmpC30;struct Cyc_List_List*_tmpC31;void*_tmpC32;struct _tuple28*
_tmpC2F=(struct _tuple28*)des->hd;_tmpC30=*_tmpC2F;_tmpC31=_tmpC30.f1;_tmpC32=
_tmpC30.f2;if(_tmpC31 == 0){struct Cyc_List_List*_tmpC33=fields;for(0;_tmpC33 != 0;
_tmpC33=_tmpC33->tl){if(!(*((struct _tuple27*)_tmpC33->hd)).f2){(*((struct
_tuple27*)_tmpC33->hd)).f2=1;{struct Cyc_Absyn_FieldName_struct*_tmp1308;struct
Cyc_Absyn_FieldName_struct _tmp1307;struct Cyc_List_List*_tmp1306;(*((struct
_tuple28*)des->hd)).f1=(struct Cyc_List_List*)((_tmp1306=_cycalloc(sizeof(*
_tmp1306)),((_tmp1306->hd=(void*)((_tmp1308=_cycalloc(sizeof(*_tmp1308)),((
_tmp1308[0]=((_tmp1307.tag=1,((_tmp1307.f1=((*((struct _tuple27*)_tmpC33->hd)).f1)->name,
_tmp1307)))),_tmp1308)))),((_tmp1306->tl=0,_tmp1306))))));}{struct _tuple29*
_tmp130B;struct Cyc_List_List*_tmp130A;ans=((_tmp130A=_region_malloc(rgn,sizeof(*
_tmp130A)),((_tmp130A->hd=((_tmp130B=_region_malloc(rgn,sizeof(*_tmp130B)),((
_tmp130B->f1=(*((struct _tuple27*)_tmpC33->hd)).f1,((_tmp130B->f2=_tmpC32,
_tmp130B)))))),((_tmp130A->tl=ans,_tmp130A))))));}break;}}if(_tmpC33 == 0){const
char*_tmp130F;void*_tmp130E[1];struct Cyc_String_pa_struct _tmp130D;(_tmp130D.tag=
0,((_tmp130D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)aggr_str),((
_tmp130E[0]=& _tmp130D,Cyc_Tcutil_terr(loc,((_tmp130F="too many arguments to %s",
_tag_dyneither(_tmp130F,sizeof(char),25))),_tag_dyneither(_tmp130E,sizeof(void*),
1)))))));}}else{if(_tmpC31->tl != 0){const char*_tmp1312;void*_tmp1311;(_tmp1311=0,
Cyc_Tcutil_terr(loc,((_tmp1312="multiple designators are not yet supported",
_tag_dyneither(_tmp1312,sizeof(char),43))),_tag_dyneither(_tmp1311,sizeof(void*),
0)));}else{void*_tmpC3E=(void*)_tmpC31->hd;struct _dyneither_ptr*_tmpC41;_LL5F2: {
struct Cyc_Absyn_ArrayElement_struct*_tmpC3F=(struct Cyc_Absyn_ArrayElement_struct*)
_tmpC3E;if(_tmpC3F->tag != 0)goto _LL5F4;}_LL5F3:{const char*_tmp1316;void*_tmp1315[
1];struct Cyc_String_pa_struct _tmp1314;(_tmp1314.tag=0,((_tmp1314.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)aggr_str),((_tmp1315[0]=& _tmp1314,Cyc_Tcutil_terr(
loc,((_tmp1316="array designator used in argument to %s",_tag_dyneither(_tmp1316,
sizeof(char),40))),_tag_dyneither(_tmp1315,sizeof(void*),1)))))));}goto _LL5F1;
_LL5F4: {struct Cyc_Absyn_FieldName_struct*_tmpC40=(struct Cyc_Absyn_FieldName_struct*)
_tmpC3E;if(_tmpC40->tag != 1)goto _LL5F1;else{_tmpC41=_tmpC40->f1;}}_LL5F5: {
struct Cyc_List_List*_tmpC45=fields;for(0;_tmpC45 != 0;_tmpC45=_tmpC45->tl){if(Cyc_strptrcmp(
_tmpC41,((*((struct _tuple27*)_tmpC45->hd)).f1)->name)== 0){if((*((struct _tuple27*)
_tmpC45->hd)).f2){const char*_tmp131A;void*_tmp1319[1];struct Cyc_String_pa_struct
_tmp1318;(_tmp1318.tag=0,((_tmp1318.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*_tmpC41),((_tmp1319[0]=& _tmp1318,Cyc_Tcutil_terr(loc,((_tmp131A="member %s has already been used as an argument",
_tag_dyneither(_tmp131A,sizeof(char),47))),_tag_dyneither(_tmp1319,sizeof(void*),
1)))))));}(*((struct _tuple27*)_tmpC45->hd)).f2=1;{struct _tuple29*_tmp131D;struct
Cyc_List_List*_tmp131C;ans=((_tmp131C=_region_malloc(rgn,sizeof(*_tmp131C)),((
_tmp131C->hd=((_tmp131D=_region_malloc(rgn,sizeof(*_tmp131D)),((_tmp131D->f1=(*((
struct _tuple27*)_tmpC45->hd)).f1,((_tmp131D->f2=_tmpC32,_tmp131D)))))),((
_tmp131C->tl=ans,_tmp131C))))));}break;}}if(_tmpC45 == 0){const char*_tmp1321;void*
_tmp1320[1];struct Cyc_String_pa_struct _tmp131F;(_tmp131F.tag=0,((_tmp131F.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpC41),((_tmp1320[0]=& _tmp131F,
Cyc_Tcutil_terr(loc,((_tmp1321="bad field designator %s",_tag_dyneither(_tmp1321,
sizeof(char),24))),_tag_dyneither(_tmp1320,sizeof(void*),1)))))));}goto _LL5F1;}
_LL5F1:;}}}if(aggr_kind == Cyc_Absyn_StructA)for(0;fields != 0;fields=fields->tl){
if(!(*((struct _tuple27*)fields->hd)).f2){{const char*_tmp1324;void*_tmp1323;(
_tmp1323=0,Cyc_Tcutil_terr(loc,((_tmp1324="too few arguments to struct",
_tag_dyneither(_tmp1324,sizeof(char),28))),_tag_dyneither(_tmp1323,sizeof(void*),
0)));}break;}}else{int found=0;for(0;fields != 0;fields=fields->tl){if((*((struct
_tuple27*)fields->hd)).f2){if(found){const char*_tmp1327;void*_tmp1326;(_tmp1326=
0,Cyc_Tcutil_terr(loc,((_tmp1327="only one member of a union is allowed",
_tag_dyneither(_tmp1327,sizeof(char),38))),_tag_dyneither(_tmp1326,sizeof(void*),
0)));}found=1;}}if(!found){const char*_tmp132A;void*_tmp1329;(_tmp1329=0,Cyc_Tcutil_terr(
loc,((_tmp132A="missing member for union",_tag_dyneither(_tmp132A,sizeof(char),
25))),_tag_dyneither(_tmp1329,sizeof(void*),0)));}}return((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(ans);}}int Cyc_Tcutil_is_tagged_pointer_typ_elt(
void*t,void**elt_typ_dest);int Cyc_Tcutil_is_tagged_pointer_typ_elt(void*t,void**
elt_typ_dest){void*_tmpC56=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmpC58;
void*_tmpC59;struct Cyc_Absyn_PtrAtts _tmpC5A;union Cyc_Absyn_Constraint*_tmpC5B;
_LL5F7: {struct Cyc_Absyn_PointerType_struct*_tmpC57=(struct Cyc_Absyn_PointerType_struct*)
_tmpC56;if(_tmpC57->tag != 5)goto _LL5F9;else{_tmpC58=_tmpC57->f1;_tmpC59=_tmpC58.elt_typ;
_tmpC5A=_tmpC58.ptr_atts;_tmpC5B=_tmpC5A.bounds;}}_LL5F8: {void*_tmpC5C=((void*(*)(
void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)((void*)& Cyc_Absyn_DynEither_b_val,
_tmpC5B);_LL5FC: {struct Cyc_Absyn_DynEither_b_struct*_tmpC5D=(struct Cyc_Absyn_DynEither_b_struct*)
_tmpC5C;if(_tmpC5D->tag != 0)goto _LL5FE;}_LL5FD:*elt_typ_dest=_tmpC59;return 1;
_LL5FE:;_LL5FF: return 0;_LL5FB:;}_LL5F9:;_LL5FA: return 0;_LL5F6:;}int Cyc_Tcutil_is_zero_pointer_typ_elt(
void*t,void**elt_typ_dest);int Cyc_Tcutil_is_zero_pointer_typ_elt(void*t,void**
elt_typ_dest){void*_tmpC5E=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmpC60;
void*_tmpC61;struct Cyc_Absyn_PtrAtts _tmpC62;union Cyc_Absyn_Constraint*_tmpC63;
_LL601: {struct Cyc_Absyn_PointerType_struct*_tmpC5F=(struct Cyc_Absyn_PointerType_struct*)
_tmpC5E;if(_tmpC5F->tag != 5)goto _LL603;else{_tmpC60=_tmpC5F->f1;_tmpC61=_tmpC60.elt_typ;
_tmpC62=_tmpC60.ptr_atts;_tmpC63=_tmpC62.zero_term;}}_LL602:*elt_typ_dest=
_tmpC61;return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmpC63);_LL603:;_LL604: return 0;_LL600:;}int Cyc_Tcutil_is_zero_ptr_type(void*t,
void**ptr_type,int*is_dyneither,void**elt_type);int Cyc_Tcutil_is_zero_ptr_type(
void*t,void**ptr_type,int*is_dyneither,void**elt_type){void*_tmpC64=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_PtrInfo _tmpC66;void*_tmpC67;struct Cyc_Absyn_PtrAtts _tmpC68;
union Cyc_Absyn_Constraint*_tmpC69;union Cyc_Absyn_Constraint*_tmpC6A;struct Cyc_Absyn_ArrayInfo
_tmpC6C;void*_tmpC6D;struct Cyc_Absyn_Tqual _tmpC6E;struct Cyc_Absyn_Exp*_tmpC6F;
union Cyc_Absyn_Constraint*_tmpC70;_LL606: {struct Cyc_Absyn_PointerType_struct*
_tmpC65=(struct Cyc_Absyn_PointerType_struct*)_tmpC64;if(_tmpC65->tag != 5)goto
_LL608;else{_tmpC66=_tmpC65->f1;_tmpC67=_tmpC66.elt_typ;_tmpC68=_tmpC66.ptr_atts;
_tmpC69=_tmpC68.bounds;_tmpC6A=_tmpC68.zero_term;}}_LL607: if(((int(*)(int y,union
Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpC6A)){*ptr_type=t;*elt_type=
_tmpC67;{void*_tmpC71=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,_tmpC69);_LL60D: {struct Cyc_Absyn_DynEither_b_struct*
_tmpC72=(struct Cyc_Absyn_DynEither_b_struct*)_tmpC71;if(_tmpC72->tag != 0)goto
_LL60F;}_LL60E:*is_dyneither=1;goto _LL60C;_LL60F:;_LL610:*is_dyneither=0;goto
_LL60C;_LL60C:;}return 1;}else{return 0;}_LL608: {struct Cyc_Absyn_ArrayType_struct*
_tmpC6B=(struct Cyc_Absyn_ArrayType_struct*)_tmpC64;if(_tmpC6B->tag != 9)goto
_LL60A;else{_tmpC6C=_tmpC6B->f1;_tmpC6D=_tmpC6C.elt_type;_tmpC6E=_tmpC6C.tq;
_tmpC6F=_tmpC6C.num_elts;_tmpC70=_tmpC6C.zero_term;}}_LL609: if(((int(*)(int y,
union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpC70)){*elt_type=_tmpC6D;*
is_dyneither=0;{struct Cyc_Absyn_PointerType_struct _tmp133F;struct Cyc_Absyn_PtrAtts
_tmp133E;struct Cyc_Absyn_Upper_b_struct _tmp133D;struct Cyc_Absyn_Upper_b_struct*
_tmp133C;struct Cyc_Absyn_PtrInfo _tmp133B;struct Cyc_Absyn_PointerType_struct*
_tmp133A;*ptr_type=(void*)((_tmp133A=_cycalloc(sizeof(*_tmp133A)),((_tmp133A[0]=((
_tmp133F.tag=5,((_tmp133F.f1=((_tmp133B.elt_typ=_tmpC6D,((_tmp133B.elt_tq=
_tmpC6E,((_tmp133B.ptr_atts=((_tmp133E.rgn=(void*)& Cyc_Absyn_HeapRgn_val,((
_tmp133E.nullable=Cyc_Absyn_false_conref,((_tmp133E.bounds=((union Cyc_Absyn_Constraint*(*)(
void*x))Cyc_Absyn_new_conref)((void*)((_tmp133C=_cycalloc(sizeof(*_tmp133C)),((
_tmp133C[0]=((_tmp133D.tag=1,((_tmp133D.f1=(struct Cyc_Absyn_Exp*)_check_null(
_tmpC6F),_tmp133D)))),_tmp133C))))),((_tmp133E.zero_term=_tmpC70,((_tmp133E.ptrloc=
0,_tmp133E)))))))))),_tmp133B)))))),_tmp133F)))),_tmp133A))));}return 1;}else{
return 0;}_LL60A:;_LL60B: return 0;_LL605:;}int Cyc_Tcutil_is_zero_ptr_deref(struct
Cyc_Absyn_Exp*e1,void**ptr_type,int*is_dyneither,void**elt_type);int Cyc_Tcutil_is_zero_ptr_deref(
struct Cyc_Absyn_Exp*e1,void**ptr_type,int*is_dyneither,void**elt_type){void*
_tmpC79=e1->r;struct Cyc_Absyn_Exp*_tmpC7C;struct Cyc_Absyn_Exp*_tmpC7E;struct Cyc_Absyn_Exp*
_tmpC80;struct Cyc_Absyn_Exp*_tmpC82;struct Cyc_Absyn_Exp*_tmpC84;struct Cyc_Absyn_Exp*
_tmpC86;_LL612: {struct Cyc_Absyn_Cast_e_struct*_tmpC7A=(struct Cyc_Absyn_Cast_e_struct*)
_tmpC79;if(_tmpC7A->tag != 15)goto _LL614;}_LL613: {const char*_tmp1343;void*
_tmp1342[1];struct Cyc_String_pa_struct _tmp1341;(_tmp1341.tag=0,((_tmp1341.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e1)),((
_tmp1342[0]=& _tmp1341,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmp1343="we have a cast in a lhs:  %s",_tag_dyneither(
_tmp1343,sizeof(char),29))),_tag_dyneither(_tmp1342,sizeof(void*),1)))))));}
_LL614: {struct Cyc_Absyn_Deref_e_struct*_tmpC7B=(struct Cyc_Absyn_Deref_e_struct*)
_tmpC79;if(_tmpC7B->tag != 22)goto _LL616;else{_tmpC7C=_tmpC7B->f1;}}_LL615:
_tmpC7E=_tmpC7C;goto _LL617;_LL616: {struct Cyc_Absyn_Subscript_e_struct*_tmpC7D=(
struct Cyc_Absyn_Subscript_e_struct*)_tmpC79;if(_tmpC7D->tag != 25)goto _LL618;
else{_tmpC7E=_tmpC7D->f1;}}_LL617: return Cyc_Tcutil_is_zero_ptr_type((void*)((
struct Cyc_Core_Opt*)_check_null(_tmpC7E->topt))->v,ptr_type,is_dyneither,
elt_type);_LL618: {struct Cyc_Absyn_AggrArrow_e_struct*_tmpC7F=(struct Cyc_Absyn_AggrArrow_e_struct*)
_tmpC79;if(_tmpC7F->tag != 24)goto _LL61A;else{_tmpC80=_tmpC7F->f1;}}_LL619:
_tmpC82=_tmpC80;goto _LL61B;_LL61A: {struct Cyc_Absyn_AggrMember_e_struct*_tmpC81=(
struct Cyc_Absyn_AggrMember_e_struct*)_tmpC79;if(_tmpC81->tag != 23)goto _LL61C;
else{_tmpC82=_tmpC81->f1;}}_LL61B: if(Cyc_Tcutil_is_zero_ptr_type((void*)((struct
Cyc_Core_Opt*)_check_null(_tmpC82->topt))->v,ptr_type,is_dyneither,elt_type)){
const char*_tmp1347;void*_tmp1346[1];struct Cyc_String_pa_struct _tmp1345;(_tmp1345.tag=
0,((_tmp1345.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(
e1)),((_tmp1346[0]=& _tmp1345,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1347="found zero pointer aggregate member assignment: %s",
_tag_dyneither(_tmp1347,sizeof(char),51))),_tag_dyneither(_tmp1346,sizeof(void*),
1)))))));}return 0;_LL61C: {struct Cyc_Absyn_Instantiate_e_struct*_tmpC83=(struct
Cyc_Absyn_Instantiate_e_struct*)_tmpC79;if(_tmpC83->tag != 14)goto _LL61E;else{
_tmpC84=_tmpC83->f1;}}_LL61D: _tmpC86=_tmpC84;goto _LL61F;_LL61E: {struct Cyc_Absyn_NoInstantiate_e_struct*
_tmpC85=(struct Cyc_Absyn_NoInstantiate_e_struct*)_tmpC79;if(_tmpC85->tag != 13)
goto _LL620;else{_tmpC86=_tmpC85->f1;}}_LL61F: if(Cyc_Tcutil_is_zero_ptr_type((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpC86->topt))->v,ptr_type,is_dyneither,
elt_type)){const char*_tmp134B;void*_tmp134A[1];struct Cyc_String_pa_struct
_tmp1349;(_tmp1349.tag=0,((_tmp1349.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_exp2string(e1)),((_tmp134A[0]=& _tmp1349,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp134B="found zero pointer instantiate/noinstantiate: %s",
_tag_dyneither(_tmp134B,sizeof(char),49))),_tag_dyneither(_tmp134A,sizeof(void*),
1)))))));}return 0;_LL620: {struct Cyc_Absyn_Var_e_struct*_tmpC87=(struct Cyc_Absyn_Var_e_struct*)
_tmpC79;if(_tmpC87->tag != 1)goto _LL622;}_LL621: return 0;_LL622:;_LL623: {const
char*_tmp134F;void*_tmp134E[1];struct Cyc_String_pa_struct _tmp134D;(_tmp134D.tag=
0,((_tmp134D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(
e1)),((_tmp134E[0]=& _tmp134D,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp134F="found bad lhs in is_zero_ptr_deref: %s",
_tag_dyneither(_tmp134F,sizeof(char),39))),_tag_dyneither(_tmp134E,sizeof(void*),
1)))))));}_LL611:;}int Cyc_Tcutil_is_tagged_pointer_typ(void*t);int Cyc_Tcutil_is_tagged_pointer_typ(
void*t){void*ignore=(void*)& Cyc_Absyn_VoidType_val;return Cyc_Tcutil_is_tagged_pointer_typ_elt(
t,& ignore);}static int Cyc_Tcutil_is_noalias_region(void*r);static int Cyc_Tcutil_is_noalias_region(
void*r){void*_tmpC94=Cyc_Tcutil_compress(r);struct Cyc_Absyn_Tvar*_tmpC97;_LL625: {
struct Cyc_Absyn_UniqueRgn_struct*_tmpC95=(struct Cyc_Absyn_UniqueRgn_struct*)
_tmpC94;if(_tmpC95->tag != 22)goto _LL627;}_LL626: return 1;_LL627: {struct Cyc_Absyn_VarType_struct*
_tmpC96=(struct Cyc_Absyn_VarType_struct*)_tmpC94;if(_tmpC96->tag != 2)goto _LL629;
else{_tmpC97=_tmpC96->f1;}}_LL628: if(Cyc_Tcutil_tvar_kind(_tmpC97,Cyc_Absyn_RgnKind)
== Cyc_Absyn_UniqueRgnKind  || Cyc_Tcutil_tvar_kind(_tmpC97,Cyc_Absyn_RgnKind)== 
Cyc_Absyn_TopRgnKind){void*_tmpC98=Cyc_Absyn_compress_kb(_tmpC97->kind);struct
Cyc_Core_Opt*_tmpC9A;struct Cyc_Core_Opt**_tmpC9B;enum Cyc_Absyn_Kind _tmpC9C;
_LL62C: {struct Cyc_Absyn_Less_kb_struct*_tmpC99=(struct Cyc_Absyn_Less_kb_struct*)
_tmpC98;if(_tmpC99->tag != 2)goto _LL62E;else{_tmpC9A=_tmpC99->f1;_tmpC9B=(struct
Cyc_Core_Opt**)& _tmpC99->f1;_tmpC9C=_tmpC99->f2;if(_tmpC9C != Cyc_Absyn_TopRgnKind)
goto _LL62E;}}_LL62D:{struct Cyc_Absyn_Less_kb_struct*_tmp1355;struct Cyc_Absyn_Less_kb_struct
_tmp1354;struct Cyc_Core_Opt*_tmp1353;*_tmpC9B=((_tmp1353=_cycalloc(sizeof(*
_tmp1353)),((_tmp1353->v=(void*)((_tmp1355=_cycalloc(sizeof(*_tmp1355)),((
_tmp1355[0]=((_tmp1354.tag=2,((_tmp1354.f1=0,((_tmp1354.f2=Cyc_Absyn_RgnKind,
_tmp1354)))))),_tmp1355)))),_tmp1353))));}return 0;_LL62E:;_LL62F: return 1;_LL62B:;}
return 0;_LL629:;_LL62A: return 0;_LL624:;}int Cyc_Tcutil_is_noalias_pointer(void*t);
int Cyc_Tcutil_is_noalias_pointer(void*t){void*_tmpCA0=Cyc_Tcutil_compress(t);
struct Cyc_Absyn_PtrInfo _tmpCA2;struct Cyc_Absyn_PtrAtts _tmpCA3;void*_tmpCA4;
_LL631: {struct Cyc_Absyn_PointerType_struct*_tmpCA1=(struct Cyc_Absyn_PointerType_struct*)
_tmpCA0;if(_tmpCA1->tag != 5)goto _LL633;else{_tmpCA2=_tmpCA1->f1;_tmpCA3=_tmpCA2.ptr_atts;
_tmpCA4=_tmpCA3.rgn;}}_LL632: return Cyc_Tcutil_is_noalias_region(_tmpCA4);_LL633:;
_LL634: return 0;_LL630:;}int Cyc_Tcutil_is_noalias_pointer_or_aggr(struct
_RegionHandle*rgn,void*t);int Cyc_Tcutil_is_noalias_pointer_or_aggr(struct
_RegionHandle*rgn,void*t){void*_tmpCA5=Cyc_Tcutil_compress(t);if(Cyc_Tcutil_is_noalias_pointer(
_tmpCA5))return 1;{void*_tmpCA6=_tmpCA5;struct Cyc_List_List*_tmpCA8;struct Cyc_Absyn_AggrInfo
_tmpCAA;union Cyc_Absyn_AggrInfoU _tmpCAB;struct Cyc_Absyn_Aggrdecl**_tmpCAC;struct
Cyc_List_List*_tmpCAD;struct Cyc_List_List*_tmpCAF;struct Cyc_Absyn_AggrInfo
_tmpCB1;union Cyc_Absyn_AggrInfoU _tmpCB2;struct _tuple4 _tmpCB3;struct Cyc_Absyn_DatatypeInfo
_tmpCB5;union Cyc_Absyn_DatatypeInfoU _tmpCB6;struct Cyc_List_List*_tmpCB7;struct
Cyc_Absyn_DatatypeFieldInfo _tmpCB9;union Cyc_Absyn_DatatypeFieldInfoU _tmpCBA;
struct Cyc_List_List*_tmpCBB;_LL636: {struct Cyc_Absyn_TupleType_struct*_tmpCA7=(
struct Cyc_Absyn_TupleType_struct*)_tmpCA6;if(_tmpCA7->tag != 11)goto _LL638;else{
_tmpCA8=_tmpCA7->f1;}}_LL637: while(_tmpCA8 != 0){if(Cyc_Tcutil_is_noalias_pointer_or_aggr(
rgn,(*((struct _tuple11*)_tmpCA8->hd)).f2))return 1;_tmpCA8=_tmpCA8->tl;}return 0;
_LL638: {struct Cyc_Absyn_AggrType_struct*_tmpCA9=(struct Cyc_Absyn_AggrType_struct*)
_tmpCA6;if(_tmpCA9->tag != 12)goto _LL63A;else{_tmpCAA=_tmpCA9->f1;_tmpCAB=_tmpCAA.aggr_info;
if((_tmpCAB.KnownAggr).tag != 2)goto _LL63A;_tmpCAC=(struct Cyc_Absyn_Aggrdecl**)(
_tmpCAB.KnownAggr).val;_tmpCAD=_tmpCAA.targs;}}_LL639: if((*_tmpCAC)->impl == 0)
return 0;else{struct Cyc_List_List*_tmpCBC=((struct Cyc_List_List*(*)(struct
_RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(rgn,rgn,(*_tmpCAC)->tvs,_tmpCAD);struct Cyc_List_List*_tmpCBD=((
struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmpCAC)->impl))->fields;void*t;
while(_tmpCBD != 0){t=Cyc_Tcutil_rsubstitute(rgn,_tmpCBC,((struct Cyc_Absyn_Aggrfield*)
_tmpCBD->hd)->type);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,t))return 1;
_tmpCBD=_tmpCBD->tl;}return 0;}_LL63A: {struct Cyc_Absyn_AnonAggrType_struct*
_tmpCAE=(struct Cyc_Absyn_AnonAggrType_struct*)_tmpCA6;if(_tmpCAE->tag != 13)goto
_LL63C;else{_tmpCAF=_tmpCAE->f2;}}_LL63B: while(_tmpCAF != 0){if(Cyc_Tcutil_is_noalias_pointer_or_aggr(
rgn,((struct Cyc_Absyn_Aggrfield*)_tmpCAF->hd)->type))return 1;_tmpCAF=_tmpCAF->tl;}
return 0;_LL63C: {struct Cyc_Absyn_AggrType_struct*_tmpCB0=(struct Cyc_Absyn_AggrType_struct*)
_tmpCA6;if(_tmpCB0->tag != 12)goto _LL63E;else{_tmpCB1=_tmpCB0->f1;_tmpCB2=_tmpCB1.aggr_info;
if((_tmpCB2.UnknownAggr).tag != 1)goto _LL63E;_tmpCB3=(struct _tuple4)(_tmpCB2.UnknownAggr).val;}}
_LL63D: {const char*_tmp1358;void*_tmp1357;(_tmp1357=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1358="got unknown aggr in is_noalias_aggr",
_tag_dyneither(_tmp1358,sizeof(char),36))),_tag_dyneither(_tmp1357,sizeof(void*),
0)));}_LL63E: {struct Cyc_Absyn_DatatypeType_struct*_tmpCB4=(struct Cyc_Absyn_DatatypeType_struct*)
_tmpCA6;if(_tmpCB4->tag != 3)goto _LL640;else{_tmpCB5=_tmpCB4->f1;_tmpCB6=_tmpCB5.datatype_info;
_tmpCB7=_tmpCB5.targs;}}_LL63F: {union Cyc_Absyn_DatatypeInfoU _tmpCC0=_tmpCB6;
struct Cyc_Absyn_UnknownDatatypeInfo _tmpCC1;struct _tuple2*_tmpCC2;int _tmpCC3;
struct Cyc_Absyn_Datatypedecl**_tmpCC4;struct Cyc_Absyn_Datatypedecl*_tmpCC5;
struct Cyc_Absyn_Datatypedecl _tmpCC6;struct Cyc_List_List*_tmpCC7;struct Cyc_Core_Opt*
_tmpCC8;_LL645: if((_tmpCC0.UnknownDatatype).tag != 1)goto _LL647;_tmpCC1=(struct
Cyc_Absyn_UnknownDatatypeInfo)(_tmpCC0.UnknownDatatype).val;_tmpCC2=_tmpCC1.name;
_tmpCC3=_tmpCC1.is_extensible;_LL646: {const char*_tmp135B;void*_tmp135A;(
_tmp135A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp135B="got unknown datatype in is_noalias_aggr",_tag_dyneither(_tmp135B,
sizeof(char),40))),_tag_dyneither(_tmp135A,sizeof(void*),0)));}_LL647: if((
_tmpCC0.KnownDatatype).tag != 2)goto _LL644;_tmpCC4=(struct Cyc_Absyn_Datatypedecl**)(
_tmpCC0.KnownDatatype).val;_tmpCC5=*_tmpCC4;_tmpCC6=*_tmpCC5;_tmpCC7=_tmpCC6.tvs;
_tmpCC8=_tmpCC6.fields;_LL648: return 0;_LL644:;}_LL640: {struct Cyc_Absyn_DatatypeFieldType_struct*
_tmpCB8=(struct Cyc_Absyn_DatatypeFieldType_struct*)_tmpCA6;if(_tmpCB8->tag != 4)
goto _LL642;else{_tmpCB9=_tmpCB8->f1;_tmpCBA=_tmpCB9.field_info;_tmpCBB=_tmpCB9.targs;}}
_LL641: {union Cyc_Absyn_DatatypeFieldInfoU _tmpCCB=_tmpCBA;struct Cyc_Absyn_UnknownDatatypeFieldInfo
_tmpCCC;struct _tuple3 _tmpCCD;struct Cyc_Absyn_Datatypedecl*_tmpCCE;struct Cyc_Absyn_Datatypefield*
_tmpCCF;_LL64A: if((_tmpCCB.UnknownDatatypefield).tag != 1)goto _LL64C;_tmpCCC=(
struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmpCCB.UnknownDatatypefield).val;
_LL64B: {const char*_tmp135E;void*_tmp135D;(_tmp135D=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp135E="got unknown datatype field in is_noalias_aggr",
_tag_dyneither(_tmp135E,sizeof(char),46))),_tag_dyneither(_tmp135D,sizeof(void*),
0)));}_LL64C: if((_tmpCCB.KnownDatatypefield).tag != 2)goto _LL649;_tmpCCD=(struct
_tuple3)(_tmpCCB.KnownDatatypefield).val;_tmpCCE=_tmpCCD.f1;_tmpCCF=_tmpCCD.f2;
_LL64D: {struct Cyc_List_List*_tmpCD2=((struct Cyc_List_List*(*)(struct
_RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(rgn,rgn,_tmpCCE->tvs,_tmpCBB);struct Cyc_List_List*_tmpCD3=
_tmpCCF->typs;while(_tmpCD3 != 0){_tmpCA5=Cyc_Tcutil_rsubstitute(rgn,_tmpCD2,(*((
struct _tuple11*)_tmpCD3->hd)).f2);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,
_tmpCA5))return 1;_tmpCD3=_tmpCD3->tl;}return 0;}_LL649:;}_LL642:;_LL643: return 0;
_LL635:;}}static int Cyc_Tcutil_is_noalias_field(struct _RegionHandle*r,void*t,
struct _dyneither_ptr*f);static int Cyc_Tcutil_is_noalias_field(struct _RegionHandle*
r,void*t,struct _dyneither_ptr*f){void*_tmpCD4=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo
_tmpCD6;union Cyc_Absyn_AggrInfoU _tmpCD7;struct Cyc_Absyn_Aggrdecl**_tmpCD8;struct
Cyc_Absyn_Aggrdecl*_tmpCD9;struct Cyc_List_List*_tmpCDA;struct Cyc_List_List*
_tmpCDC;_LL64F: {struct Cyc_Absyn_AggrType_struct*_tmpCD5=(struct Cyc_Absyn_AggrType_struct*)
_tmpCD4;if(_tmpCD5->tag != 12)goto _LL651;else{_tmpCD6=_tmpCD5->f1;_tmpCD7=_tmpCD6.aggr_info;
if((_tmpCD7.KnownAggr).tag != 2)goto _LL651;_tmpCD8=(struct Cyc_Absyn_Aggrdecl**)(
_tmpCD7.KnownAggr).val;_tmpCD9=*_tmpCD8;_tmpCDA=_tmpCD6.targs;}}_LL650: _tmpCDC=
_tmpCD9->impl == 0?0:((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpCD9->impl))->fields;
goto _LL652;_LL651: {struct Cyc_Absyn_AnonAggrType_struct*_tmpCDB=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmpCD4;if(_tmpCDB->tag != 13)goto _LL653;else{_tmpCDC=_tmpCDB->f2;}}_LL652: {
struct Cyc_Absyn_Aggrfield*_tmpCDD=Cyc_Absyn_lookup_field(_tmpCDC,f);{struct Cyc_Absyn_Aggrfield*
_tmpCDE=_tmpCDD;struct Cyc_Absyn_Aggrfield _tmpCDF;void*_tmpCE0;_LL656: if(_tmpCDE
!= 0)goto _LL658;_LL657: {const char*_tmp1361;void*_tmp1360;(_tmp1360=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1361="is_noalias_field: missing field",
_tag_dyneither(_tmp1361,sizeof(char),32))),_tag_dyneither(_tmp1360,sizeof(void*),
0)));}_LL658: if(_tmpCDE == 0)goto _LL655;_tmpCDF=*_tmpCDE;_tmpCE0=_tmpCDF.type;
_LL659: return Cyc_Tcutil_is_noalias_pointer_or_aggr(r,_tmpCE0);_LL655:;}return 0;}
_LL653:;_LL654: {const char*_tmp1365;void*_tmp1364[1];struct Cyc_String_pa_struct
_tmp1363;(_tmp1363.tag=0,((_tmp1363.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp1364[0]=& _tmp1363,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1365="is_noalias_field: invalid type |%s|",
_tag_dyneither(_tmp1365,sizeof(char),36))),_tag_dyneither(_tmp1364,sizeof(void*),
1)))))));}_LL64E:;}static int Cyc_Tcutil_is_noalias_path_aux(struct _RegionHandle*r,
struct Cyc_Absyn_Exp*e,int ignore_leaf);static int Cyc_Tcutil_is_noalias_path_aux(
struct _RegionHandle*r,struct Cyc_Absyn_Exp*e,int ignore_leaf){void*_tmpCE6=e->r;
void*_tmpCF7;struct Cyc_Absyn_Exp*_tmpCFD;struct _dyneither_ptr*_tmpCFE;struct Cyc_Absyn_Exp*
_tmpD00;struct Cyc_Absyn_Exp*_tmpD01;void*_tmpD0A;void*_tmpD0D;void*_tmpD10;
struct Cyc_Absyn_Exp*_tmpD13;struct Cyc_Absyn_Exp*_tmpD15;struct Cyc_Absyn_Exp*
_tmpD17;struct Cyc_Absyn_Exp*_tmpD19;void*_tmpD1B;struct Cyc_Absyn_Exp*_tmpD1C;
struct Cyc_Absyn_Stmt*_tmpD1E;_LL65B: {struct Cyc_Absyn_Increment_e_struct*_tmpCE7=(
struct Cyc_Absyn_Increment_e_struct*)_tmpCE6;if(_tmpCE7->tag != 5)goto _LL65D;}
_LL65C: goto _LL65E;_LL65D: {struct Cyc_Absyn_And_e_struct*_tmpCE8=(struct Cyc_Absyn_And_e_struct*)
_tmpCE6;if(_tmpCE8->tag != 7)goto _LL65F;}_LL65E: goto _LL660;_LL65F: {struct Cyc_Absyn_Or_e_struct*
_tmpCE9=(struct Cyc_Absyn_Or_e_struct*)_tmpCE6;if(_tmpCE9->tag != 8)goto _LL661;}
_LL660: goto _LL662;_LL661: {struct Cyc_Absyn_Throw_e_struct*_tmpCEA=(struct Cyc_Absyn_Throw_e_struct*)
_tmpCE6;if(_tmpCEA->tag != 12)goto _LL663;}_LL662: goto _LL664;_LL663: {struct Cyc_Absyn_Address_e_struct*
_tmpCEB=(struct Cyc_Absyn_Address_e_struct*)_tmpCE6;if(_tmpCEB->tag != 16)goto
_LL665;}_LL664: goto _LL666;_LL665: {struct Cyc_Absyn_Sizeoftyp_e_struct*_tmpCEC=(
struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmpCE6;if(_tmpCEC->tag != 18)goto _LL667;}
_LL666: goto _LL668;_LL667: {struct Cyc_Absyn_Sizeofexp_e_struct*_tmpCED=(struct Cyc_Absyn_Sizeofexp_e_struct*)
_tmpCE6;if(_tmpCED->tag != 19)goto _LL669;}_LL668: goto _LL66A;_LL669: {struct Cyc_Absyn_Offsetof_e_struct*
_tmpCEE=(struct Cyc_Absyn_Offsetof_e_struct*)_tmpCE6;if(_tmpCEE->tag != 20)goto
_LL66B;}_LL66A: goto _LL66C;_LL66B: {struct Cyc_Absyn_Gentyp_e_struct*_tmpCEF=(
struct Cyc_Absyn_Gentyp_e_struct*)_tmpCE6;if(_tmpCEF->tag != 21)goto _LL66D;}_LL66C:
goto _LL66E;_LL66D: {struct Cyc_Absyn_CompoundLit_e_struct*_tmpCF0=(struct Cyc_Absyn_CompoundLit_e_struct*)
_tmpCE6;if(_tmpCF0->tag != 27)goto _LL66F;}_LL66E: goto _LL670;_LL66F: {struct Cyc_Absyn_Comprehension_e_struct*
_tmpCF1=(struct Cyc_Absyn_Comprehension_e_struct*)_tmpCE6;if(_tmpCF1->tag != 29)
goto _LL671;}_LL670: goto _LL672;_LL671: {struct Cyc_Absyn_Array_e_struct*_tmpCF2=(
struct Cyc_Absyn_Array_e_struct*)_tmpCE6;if(_tmpCF2->tag != 28)goto _LL673;}_LL672:
goto _LL674;_LL673: {struct Cyc_Absyn_Enum_e_struct*_tmpCF3=(struct Cyc_Absyn_Enum_e_struct*)
_tmpCE6;if(_tmpCF3->tag != 33)goto _LL675;}_LL674: goto _LL676;_LL675: {struct Cyc_Absyn_AnonEnum_e_struct*
_tmpCF4=(struct Cyc_Absyn_AnonEnum_e_struct*)_tmpCE6;if(_tmpCF4->tag != 34)goto
_LL677;}_LL676: goto _LL678;_LL677: {struct Cyc_Absyn_Swap_e_struct*_tmpCF5=(struct
Cyc_Absyn_Swap_e_struct*)_tmpCE6;if(_tmpCF5->tag != 36)goto _LL679;}_LL678: goto
_LL67A;_LL679: {struct Cyc_Absyn_Var_e_struct*_tmpCF6=(struct Cyc_Absyn_Var_e_struct*)
_tmpCE6;if(_tmpCF6->tag != 1)goto _LL67B;else{_tmpCF7=(void*)_tmpCF6->f2;{struct
Cyc_Absyn_Global_b_struct*_tmpCF8=(struct Cyc_Absyn_Global_b_struct*)_tmpCF7;if(
_tmpCF8->tag != 1)goto _LL67B;}}}_LL67A: goto _LL67C;_LL67B: {struct Cyc_Absyn_Primop_e_struct*
_tmpCF9=(struct Cyc_Absyn_Primop_e_struct*)_tmpCE6;if(_tmpCF9->tag != 3)goto _LL67D;}
_LL67C: return 0;_LL67D: {struct Cyc_Absyn_Deref_e_struct*_tmpCFA=(struct Cyc_Absyn_Deref_e_struct*)
_tmpCE6;if(_tmpCFA->tag != 22)goto _LL67F;}_LL67E: goto _LL680;_LL67F: {struct Cyc_Absyn_AggrArrow_e_struct*
_tmpCFB=(struct Cyc_Absyn_AggrArrow_e_struct*)_tmpCE6;if(_tmpCFB->tag != 24)goto
_LL681;}_LL680: return 0;_LL681: {struct Cyc_Absyn_AggrMember_e_struct*_tmpCFC=(
struct Cyc_Absyn_AggrMember_e_struct*)_tmpCE6;if(_tmpCFC->tag != 23)goto _LL683;
else{_tmpCFD=_tmpCFC->f1;_tmpCFE=_tmpCFC->f2;}}_LL682: return(ignore_leaf  || Cyc_Tcutil_is_noalias_field(
r,(void*)((struct Cyc_Core_Opt*)_check_null(_tmpCFD->topt))->v,_tmpCFE)) && Cyc_Tcutil_is_noalias_path_aux(
r,_tmpCFD,0);_LL683: {struct Cyc_Absyn_Subscript_e_struct*_tmpCFF=(struct Cyc_Absyn_Subscript_e_struct*)
_tmpCE6;if(_tmpCFF->tag != 25)goto _LL685;else{_tmpD00=_tmpCFF->f1;_tmpD01=_tmpCFF->f2;}}
_LL684: {void*_tmpD1F=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmpD00->topt))->v);struct Cyc_Absyn_PtrInfo _tmpD21;void*_tmpD22;
struct Cyc_List_List*_tmpD24;_LL6A8: {struct Cyc_Absyn_PointerType_struct*_tmpD20=(
struct Cyc_Absyn_PointerType_struct*)_tmpD1F;if(_tmpD20->tag != 5)goto _LL6AA;else{
_tmpD21=_tmpD20->f1;_tmpD22=_tmpD21.elt_typ;}}_LL6A9: return 0;_LL6AA: {struct Cyc_Absyn_TupleType_struct*
_tmpD23=(struct Cyc_Absyn_TupleType_struct*)_tmpD1F;if(_tmpD23->tag != 11)goto
_LL6AC;else{_tmpD24=_tmpD23->f1;}}_LL6AB: {unsigned int _tmpD26;int _tmpD27;struct
_tuple13 _tmpD25=Cyc_Evexp_eval_const_uint_exp(_tmpD01);_tmpD26=_tmpD25.f1;
_tmpD27=_tmpD25.f2;if(!_tmpD27){const char*_tmp1368;void*_tmp1367;(_tmp1367=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp1368="is_noalias_path_aux: non-constant subscript",_tag_dyneither(_tmp1368,
sizeof(char),44))),_tag_dyneither(_tmp1367,sizeof(void*),0)));}{struct
_handler_cons _tmpD2A;_push_handler(& _tmpD2A);{int _tmpD2C=0;if(setjmp(_tmpD2A.handler))
_tmpD2C=1;if(!_tmpD2C){{void*_tmpD2D=(*((struct _tuple11*(*)(struct Cyc_List_List*
x,int n))Cyc_List_nth)(_tmpD24,(int)_tmpD26)).f2;int _tmpD2E=(ignore_leaf  || Cyc_Tcutil_is_noalias_pointer_or_aggr(
r,_tmpD2D)) && Cyc_Tcutil_is_noalias_path_aux(r,_tmpD00,0);_npop_handler(0);
return _tmpD2E;};_pop_handler();}else{void*_tmpD2B=(void*)_exn_thrown;void*
_tmpD30=_tmpD2B;_LL6AF: {struct Cyc_List_Nth_struct*_tmpD31=(struct Cyc_List_Nth_struct*)
_tmpD30;if(_tmpD31->tag != Cyc_List_Nth)goto _LL6B1;}_LL6B0: {const char*_tmp136B;
void*_tmp136A;return(_tmp136A=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp136B="is_noalias_path_aux: out-of-bounds subscript",
_tag_dyneither(_tmp136B,sizeof(char),45))),_tag_dyneither(_tmp136A,sizeof(void*),
0)));}_LL6B1:;_LL6B2:(void)_throw(_tmpD30);_LL6AE:;}}}}_LL6AC:;_LL6AD: {const
char*_tmp136E;void*_tmp136D;(_tmp136D=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp136E="is_noalias_path_aux: subscript on non-pointer/tuple",
_tag_dyneither(_tmp136E,sizeof(char),52))),_tag_dyneither(_tmp136D,sizeof(void*),
0)));}_LL6A7:;}_LL685: {struct Cyc_Absyn_Datatype_e_struct*_tmpD02=(struct Cyc_Absyn_Datatype_e_struct*)
_tmpCE6;if(_tmpD02->tag != 32)goto _LL687;}_LL686: goto _LL688;_LL687: {struct Cyc_Absyn_Tuple_e_struct*
_tmpD03=(struct Cyc_Absyn_Tuple_e_struct*)_tmpCE6;if(_tmpD03->tag != 26)goto _LL689;}
_LL688: goto _LL68A;_LL689: {struct Cyc_Absyn_Aggregate_e_struct*_tmpD04=(struct Cyc_Absyn_Aggregate_e_struct*)
_tmpCE6;if(_tmpD04->tag != 30)goto _LL68B;}_LL68A: goto _LL68C;_LL68B: {struct Cyc_Absyn_AnonStruct_e_struct*
_tmpD05=(struct Cyc_Absyn_AnonStruct_e_struct*)_tmpCE6;if(_tmpD05->tag != 31)goto
_LL68D;}_LL68C: goto _LL68E;_LL68D: {struct Cyc_Absyn_Const_e_struct*_tmpD06=(
struct Cyc_Absyn_Const_e_struct*)_tmpCE6;if(_tmpD06->tag != 0)goto _LL68F;}_LL68E:
goto _LL690;_LL68F: {struct Cyc_Absyn_Malloc_e_struct*_tmpD07=(struct Cyc_Absyn_Malloc_e_struct*)
_tmpCE6;if(_tmpD07->tag != 35)goto _LL691;}_LL690: goto _LL692;_LL691: {struct Cyc_Absyn_New_e_struct*
_tmpD08=(struct Cyc_Absyn_New_e_struct*)_tmpCE6;if(_tmpD08->tag != 17)goto _LL693;}
_LL692: goto _LL694;_LL693: {struct Cyc_Absyn_Var_e_struct*_tmpD09=(struct Cyc_Absyn_Var_e_struct*)
_tmpCE6;if(_tmpD09->tag != 1)goto _LL695;else{_tmpD0A=(void*)_tmpD09->f2;{struct
Cyc_Absyn_Local_b_struct*_tmpD0B=(struct Cyc_Absyn_Local_b_struct*)_tmpD0A;if(
_tmpD0B->tag != 4)goto _LL695;}}}_LL694: goto _LL696;_LL695: {struct Cyc_Absyn_Var_e_struct*
_tmpD0C=(struct Cyc_Absyn_Var_e_struct*)_tmpCE6;if(_tmpD0C->tag != 1)goto _LL697;
else{_tmpD0D=(void*)_tmpD0C->f2;{struct Cyc_Absyn_Pat_b_struct*_tmpD0E=(struct Cyc_Absyn_Pat_b_struct*)
_tmpD0D;if(_tmpD0E->tag != 5)goto _LL697;}}}_LL696: goto _LL698;_LL697: {struct Cyc_Absyn_Var_e_struct*
_tmpD0F=(struct Cyc_Absyn_Var_e_struct*)_tmpCE6;if(_tmpD0F->tag != 1)goto _LL699;
else{_tmpD10=(void*)_tmpD0F->f2;{struct Cyc_Absyn_Param_b_struct*_tmpD11=(struct
Cyc_Absyn_Param_b_struct*)_tmpD10;if(_tmpD11->tag != 3)goto _LL699;}}}_LL698: {int
_tmpD36=ignore_leaf  || Cyc_Tcutil_is_noalias_pointer_or_aggr(r,(void*)((struct
Cyc_Core_Opt*)_check_null(e->topt))->v);return _tmpD36;}_LL699: {struct Cyc_Absyn_Conditional_e_struct*
_tmpD12=(struct Cyc_Absyn_Conditional_e_struct*)_tmpCE6;if(_tmpD12->tag != 6)goto
_LL69B;else{_tmpD13=_tmpD12->f2;}}_LL69A: _tmpD15=_tmpD13;goto _LL69C;_LL69B: {
struct Cyc_Absyn_SeqExp_e_struct*_tmpD14=(struct Cyc_Absyn_SeqExp_e_struct*)
_tmpCE6;if(_tmpD14->tag != 9)goto _LL69D;else{_tmpD15=_tmpD14->f2;}}_LL69C: _tmpD17=
_tmpD15;goto _LL69E;_LL69D: {struct Cyc_Absyn_AssignOp_e_struct*_tmpD16=(struct Cyc_Absyn_AssignOp_e_struct*)
_tmpCE6;if(_tmpD16->tag != 4)goto _LL69F;else{_tmpD17=_tmpD16->f1;}}_LL69E: return
Cyc_Tcutil_is_noalias_path_aux(r,_tmpD17,ignore_leaf);_LL69F: {struct Cyc_Absyn_FnCall_e_struct*
_tmpD18=(struct Cyc_Absyn_FnCall_e_struct*)_tmpCE6;if(_tmpD18->tag != 11)goto
_LL6A1;else{_tmpD19=_tmpD18->f1;}}_LL6A0: {void*_tmpD37=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpD19->topt))->v);struct Cyc_Absyn_FnInfo
_tmpD39;void*_tmpD3A;struct Cyc_Absyn_PtrInfo _tmpD3C;void*_tmpD3D;_LL6B4: {struct
Cyc_Absyn_FnType_struct*_tmpD38=(struct Cyc_Absyn_FnType_struct*)_tmpD37;if(
_tmpD38->tag != 10)goto _LL6B6;else{_tmpD39=_tmpD38->f1;_tmpD3A=_tmpD39.ret_typ;}}
_LL6B5: return ignore_leaf  || Cyc_Tcutil_is_noalias_pointer_or_aggr(r,_tmpD3A);
_LL6B6: {struct Cyc_Absyn_PointerType_struct*_tmpD3B=(struct Cyc_Absyn_PointerType_struct*)
_tmpD37;if(_tmpD3B->tag != 5)goto _LL6B8;else{_tmpD3C=_tmpD3B->f1;_tmpD3D=_tmpD3C.elt_typ;}}
_LL6B7:{void*_tmpD3E=Cyc_Tcutil_compress(_tmpD3D);struct Cyc_Absyn_FnInfo _tmpD40;
void*_tmpD41;_LL6BB: {struct Cyc_Absyn_FnType_struct*_tmpD3F=(struct Cyc_Absyn_FnType_struct*)
_tmpD3E;if(_tmpD3F->tag != 10)goto _LL6BD;else{_tmpD40=_tmpD3F->f1;_tmpD41=_tmpD40.ret_typ;}}
_LL6BC: return ignore_leaf  || Cyc_Tcutil_is_noalias_pointer_or_aggr(r,_tmpD41);
_LL6BD:;_LL6BE: goto _LL6BA;_LL6BA:;}goto _LL6B9;_LL6B8:;_LL6B9: {const char*
_tmp1372;void*_tmp1371[1];struct Cyc_String_pa_struct _tmp1370;(_tmp1370.tag=0,((
_tmp1370.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpD19->topt))->v)),((_tmp1371[0]=&
_tmp1370,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp1372="Fncall has non function type %s",_tag_dyneither(_tmp1372,sizeof(char),
32))),_tag_dyneither(_tmp1371,sizeof(void*),1)))))));}_LL6B3:;}_LL6A1: {struct
Cyc_Absyn_Cast_e_struct*_tmpD1A=(struct Cyc_Absyn_Cast_e_struct*)_tmpCE6;if(
_tmpD1A->tag != 15)goto _LL6A3;else{_tmpD1B=(void*)_tmpD1A->f1;_tmpD1C=_tmpD1A->f2;}}
_LL6A2: return Cyc_Tcutil_is_noalias_pointer_or_aggr(r,_tmpD1B) && Cyc_Tcutil_is_noalias_path_aux(
r,_tmpD1C,1);_LL6A3: {struct Cyc_Absyn_StmtExp_e_struct*_tmpD1D=(struct Cyc_Absyn_StmtExp_e_struct*)
_tmpCE6;if(_tmpD1D->tag != 38)goto _LL6A5;else{_tmpD1E=_tmpD1D->f1;}}_LL6A4: while(
1){void*_tmpD45=_tmpD1E->r;struct Cyc_Absyn_Stmt*_tmpD47;struct Cyc_Absyn_Stmt*
_tmpD48;struct Cyc_Absyn_Decl*_tmpD4A;struct Cyc_Absyn_Stmt*_tmpD4B;struct Cyc_Absyn_Exp*
_tmpD4D;_LL6C0: {struct Cyc_Absyn_Seq_s_struct*_tmpD46=(struct Cyc_Absyn_Seq_s_struct*)
_tmpD45;if(_tmpD46->tag != 2)goto _LL6C2;else{_tmpD47=_tmpD46->f1;_tmpD48=_tmpD46->f2;}}
_LL6C1: _tmpD1E=_tmpD48;goto _LL6BF;_LL6C2: {struct Cyc_Absyn_Decl_s_struct*_tmpD49=(
struct Cyc_Absyn_Decl_s_struct*)_tmpD45;if(_tmpD49->tag != 12)goto _LL6C4;else{
_tmpD4A=_tmpD49->f1;_tmpD4B=_tmpD49->f2;}}_LL6C3: _tmpD1E=_tmpD4B;goto _LL6BF;
_LL6C4: {struct Cyc_Absyn_Exp_s_struct*_tmpD4C=(struct Cyc_Absyn_Exp_s_struct*)
_tmpD45;if(_tmpD4C->tag != 1)goto _LL6C6;else{_tmpD4D=_tmpD4C->f1;}}_LL6C5: return
Cyc_Tcutil_is_noalias_path_aux(r,_tmpD4D,ignore_leaf);_LL6C6:;_LL6C7: {const char*
_tmp1375;void*_tmp1374;(_tmp1374=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1375="is_noalias_stmt_exp: ill-formed StmtExp",
_tag_dyneither(_tmp1375,sizeof(char),40))),_tag_dyneither(_tmp1374,sizeof(void*),
0)));}_LL6BF:;}_LL6A5:;_LL6A6: return 0;_LL65A:;}int Cyc_Tcutil_is_noalias_path(
struct _RegionHandle*r,struct Cyc_Absyn_Exp*e);int Cyc_Tcutil_is_noalias_path(
struct _RegionHandle*r,struct Cyc_Absyn_Exp*e){return Cyc_Tcutil_is_noalias_path_aux(
r,e,0);}struct _tuple16 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*te,struct
Cyc_Absyn_Exp*e);struct _tuple16 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e){struct _tuple16 _tmp1376;struct _tuple16 bogus_ans=(
_tmp1376.f1=0,((_tmp1376.f2=(void*)& Cyc_Absyn_HeapRgn_val,_tmp1376)));void*
_tmpD50=e->r;struct _tuple2*_tmpD52;void*_tmpD53;struct Cyc_Absyn_Exp*_tmpD55;
struct _dyneither_ptr*_tmpD56;int _tmpD57;struct Cyc_Absyn_Exp*_tmpD59;struct
_dyneither_ptr*_tmpD5A;int _tmpD5B;struct Cyc_Absyn_Exp*_tmpD5D;struct Cyc_Absyn_Exp*
_tmpD5F;struct Cyc_Absyn_Exp*_tmpD60;_LL6C9: {struct Cyc_Absyn_Var_e_struct*
_tmpD51=(struct Cyc_Absyn_Var_e_struct*)_tmpD50;if(_tmpD51->tag != 1)goto _LL6CB;
else{_tmpD52=_tmpD51->f1;_tmpD53=(void*)_tmpD51->f2;}}_LL6CA: {void*_tmpD61=
_tmpD53;struct Cyc_Absyn_Vardecl*_tmpD65;struct Cyc_Absyn_Vardecl*_tmpD67;struct
Cyc_Absyn_Vardecl*_tmpD69;struct Cyc_Absyn_Vardecl*_tmpD6B;_LL6D6: {struct Cyc_Absyn_Unresolved_b_struct*
_tmpD62=(struct Cyc_Absyn_Unresolved_b_struct*)_tmpD61;if(_tmpD62->tag != 0)goto
_LL6D8;}_LL6D7: goto _LL6D9;_LL6D8: {struct Cyc_Absyn_Funname_b_struct*_tmpD63=(
struct Cyc_Absyn_Funname_b_struct*)_tmpD61;if(_tmpD63->tag != 2)goto _LL6DA;}_LL6D9:
return bogus_ans;_LL6DA: {struct Cyc_Absyn_Global_b_struct*_tmpD64=(struct Cyc_Absyn_Global_b_struct*)
_tmpD61;if(_tmpD64->tag != 1)goto _LL6DC;else{_tmpD65=_tmpD64->f1;}}_LL6DB: {void*
_tmpD6C=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
_LL6E3: {struct Cyc_Absyn_ArrayType_struct*_tmpD6D=(struct Cyc_Absyn_ArrayType_struct*)
_tmpD6C;if(_tmpD6D->tag != 9)goto _LL6E5;}_LL6E4: {struct _tuple16 _tmp1377;return(
_tmp1377.f1=1,((_tmp1377.f2=(void*)& Cyc_Absyn_HeapRgn_val,_tmp1377)));}_LL6E5:;
_LL6E6: {struct _tuple16 _tmp1378;return(_tmp1378.f1=(_tmpD65->tq).real_const,((
_tmp1378.f2=(void*)& Cyc_Absyn_HeapRgn_val,_tmp1378)));}_LL6E2:;}_LL6DC: {struct
Cyc_Absyn_Local_b_struct*_tmpD66=(struct Cyc_Absyn_Local_b_struct*)_tmpD61;if(
_tmpD66->tag != 4)goto _LL6DE;else{_tmpD67=_tmpD66->f1;}}_LL6DD: {void*_tmpD70=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);_LL6E8: {struct Cyc_Absyn_ArrayType_struct*
_tmpD71=(struct Cyc_Absyn_ArrayType_struct*)_tmpD70;if(_tmpD71->tag != 9)goto
_LL6EA;}_LL6E9: {struct _tuple16 _tmp1379;return(_tmp1379.f1=1,((_tmp1379.f2=(void*)((
struct Cyc_Core_Opt*)_check_null(_tmpD67->rgn))->v,_tmp1379)));}_LL6EA:;_LL6EB:
_tmpD67->escapes=1;{struct _tuple16 _tmp137A;return(_tmp137A.f1=(_tmpD67->tq).real_const,((
_tmp137A.f2=(void*)((struct Cyc_Core_Opt*)_check_null(_tmpD67->rgn))->v,_tmp137A)));}
_LL6E7:;}_LL6DE: {struct Cyc_Absyn_Pat_b_struct*_tmpD68=(struct Cyc_Absyn_Pat_b_struct*)
_tmpD61;if(_tmpD68->tag != 5)goto _LL6E0;else{_tmpD69=_tmpD68->f1;}}_LL6DF: _tmpD6B=
_tmpD69;goto _LL6E1;_LL6E0: {struct Cyc_Absyn_Param_b_struct*_tmpD6A=(struct Cyc_Absyn_Param_b_struct*)
_tmpD61;if(_tmpD6A->tag != 3)goto _LL6D5;else{_tmpD6B=_tmpD6A->f1;}}_LL6E1: _tmpD6B->escapes=
1;{struct _tuple16 _tmp137B;return(_tmp137B.f1=(_tmpD6B->tq).real_const,((_tmp137B.f2=(
void*)((struct Cyc_Core_Opt*)_check_null(_tmpD6B->rgn))->v,_tmp137B)));}_LL6D5:;}
_LL6CB: {struct Cyc_Absyn_AggrMember_e_struct*_tmpD54=(struct Cyc_Absyn_AggrMember_e_struct*)
_tmpD50;if(_tmpD54->tag != 23)goto _LL6CD;else{_tmpD55=_tmpD54->f1;_tmpD56=_tmpD54->f2;
_tmpD57=_tmpD54->f3;}}_LL6CC: if(_tmpD57)return bogus_ans;{void*_tmpD75=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpD55->topt))->v);struct Cyc_List_List*
_tmpD77;struct Cyc_Absyn_AggrInfo _tmpD79;union Cyc_Absyn_AggrInfoU _tmpD7A;struct
Cyc_Absyn_Aggrdecl**_tmpD7B;struct Cyc_Absyn_Aggrdecl*_tmpD7C;_LL6ED: {struct Cyc_Absyn_AnonAggrType_struct*
_tmpD76=(struct Cyc_Absyn_AnonAggrType_struct*)_tmpD75;if(_tmpD76->tag != 13)goto
_LL6EF;else{_tmpD77=_tmpD76->f2;}}_LL6EE: {struct Cyc_Absyn_Aggrfield*_tmpD7D=Cyc_Absyn_lookup_field(
_tmpD77,_tmpD56);if(_tmpD7D != 0  && _tmpD7D->width != 0){struct _tuple16 _tmp137C;
return(_tmp137C.f1=(_tmpD7D->tq).real_const,((_tmp137C.f2=(Cyc_Tcutil_addressof_props(
te,_tmpD55)).f2,_tmp137C)));}return bogus_ans;}_LL6EF: {struct Cyc_Absyn_AggrType_struct*
_tmpD78=(struct Cyc_Absyn_AggrType_struct*)_tmpD75;if(_tmpD78->tag != 12)goto
_LL6F1;else{_tmpD79=_tmpD78->f1;_tmpD7A=_tmpD79.aggr_info;if((_tmpD7A.KnownAggr).tag
!= 2)goto _LL6F1;_tmpD7B=(struct Cyc_Absyn_Aggrdecl**)(_tmpD7A.KnownAggr).val;
_tmpD7C=*_tmpD7B;}}_LL6F0: {struct Cyc_Absyn_Aggrfield*_tmpD7F=Cyc_Absyn_lookup_decl_field(
_tmpD7C,_tmpD56);if(_tmpD7F != 0  && _tmpD7F->width != 0){struct _tuple16 _tmp137D;
return(_tmp137D.f1=(_tmpD7F->tq).real_const,((_tmp137D.f2=(Cyc_Tcutil_addressof_props(
te,_tmpD55)).f2,_tmp137D)));}return bogus_ans;}_LL6F1:;_LL6F2: return bogus_ans;
_LL6EC:;}_LL6CD: {struct Cyc_Absyn_AggrArrow_e_struct*_tmpD58=(struct Cyc_Absyn_AggrArrow_e_struct*)
_tmpD50;if(_tmpD58->tag != 24)goto _LL6CF;else{_tmpD59=_tmpD58->f1;_tmpD5A=_tmpD58->f2;
_tmpD5B=_tmpD58->f3;}}_LL6CE: if(_tmpD5B)return bogus_ans;{void*_tmpD81=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpD59->topt))->v);struct Cyc_Absyn_PtrInfo
_tmpD83;void*_tmpD84;struct Cyc_Absyn_PtrAtts _tmpD85;void*_tmpD86;_LL6F4: {struct
Cyc_Absyn_PointerType_struct*_tmpD82=(struct Cyc_Absyn_PointerType_struct*)
_tmpD81;if(_tmpD82->tag != 5)goto _LL6F6;else{_tmpD83=_tmpD82->f1;_tmpD84=_tmpD83.elt_typ;
_tmpD85=_tmpD83.ptr_atts;_tmpD86=_tmpD85.rgn;}}_LL6F5: {struct Cyc_Absyn_Aggrfield*
finfo;{void*_tmpD87=Cyc_Tcutil_compress(_tmpD84);struct Cyc_List_List*_tmpD89;
struct Cyc_Absyn_AggrInfo _tmpD8B;union Cyc_Absyn_AggrInfoU _tmpD8C;struct Cyc_Absyn_Aggrdecl**
_tmpD8D;struct Cyc_Absyn_Aggrdecl*_tmpD8E;_LL6F9: {struct Cyc_Absyn_AnonAggrType_struct*
_tmpD88=(struct Cyc_Absyn_AnonAggrType_struct*)_tmpD87;if(_tmpD88->tag != 13)goto
_LL6FB;else{_tmpD89=_tmpD88->f2;}}_LL6FA: finfo=Cyc_Absyn_lookup_field(_tmpD89,
_tmpD5A);goto _LL6F8;_LL6FB: {struct Cyc_Absyn_AggrType_struct*_tmpD8A=(struct Cyc_Absyn_AggrType_struct*)
_tmpD87;if(_tmpD8A->tag != 12)goto _LL6FD;else{_tmpD8B=_tmpD8A->f1;_tmpD8C=_tmpD8B.aggr_info;
if((_tmpD8C.KnownAggr).tag != 2)goto _LL6FD;_tmpD8D=(struct Cyc_Absyn_Aggrdecl**)(
_tmpD8C.KnownAggr).val;_tmpD8E=*_tmpD8D;}}_LL6FC: finfo=Cyc_Absyn_lookup_decl_field(
_tmpD8E,_tmpD5A);goto _LL6F8;_LL6FD:;_LL6FE: return bogus_ans;_LL6F8:;}if(finfo != 0
 && finfo->width != 0){struct _tuple16 _tmp137E;return(_tmp137E.f1=(finfo->tq).real_const,((
_tmp137E.f2=_tmpD86,_tmp137E)));}return bogus_ans;}_LL6F6:;_LL6F7: return bogus_ans;
_LL6F3:;}_LL6CF: {struct Cyc_Absyn_Deref_e_struct*_tmpD5C=(struct Cyc_Absyn_Deref_e_struct*)
_tmpD50;if(_tmpD5C->tag != 22)goto _LL6D1;else{_tmpD5D=_tmpD5C->f1;}}_LL6D0: {void*
_tmpD90=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmpD5D->topt))->v);
struct Cyc_Absyn_PtrInfo _tmpD92;struct Cyc_Absyn_Tqual _tmpD93;struct Cyc_Absyn_PtrAtts
_tmpD94;void*_tmpD95;_LL700: {struct Cyc_Absyn_PointerType_struct*_tmpD91=(struct
Cyc_Absyn_PointerType_struct*)_tmpD90;if(_tmpD91->tag != 5)goto _LL702;else{
_tmpD92=_tmpD91->f1;_tmpD93=_tmpD92.elt_tq;_tmpD94=_tmpD92.ptr_atts;_tmpD95=
_tmpD94.rgn;}}_LL701: {struct _tuple16 _tmp137F;return(_tmp137F.f1=_tmpD93.real_const,((
_tmp137F.f2=_tmpD95,_tmp137F)));}_LL702:;_LL703: return bogus_ans;_LL6FF:;}_LL6D1: {
struct Cyc_Absyn_Subscript_e_struct*_tmpD5E=(struct Cyc_Absyn_Subscript_e_struct*)
_tmpD50;if(_tmpD5E->tag != 25)goto _LL6D3;else{_tmpD5F=_tmpD5E->f1;_tmpD60=_tmpD5E->f2;}}
_LL6D2: {void*t=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmpD5F->topt))->v);void*_tmpD97=t;struct Cyc_List_List*_tmpD99;struct Cyc_Absyn_PtrInfo
_tmpD9B;struct Cyc_Absyn_Tqual _tmpD9C;struct Cyc_Absyn_PtrAtts _tmpD9D;void*_tmpD9E;
struct Cyc_Absyn_ArrayInfo _tmpDA0;struct Cyc_Absyn_Tqual _tmpDA1;_LL705: {struct Cyc_Absyn_TupleType_struct*
_tmpD98=(struct Cyc_Absyn_TupleType_struct*)_tmpD97;if(_tmpD98->tag != 11)goto
_LL707;else{_tmpD99=_tmpD98->f1;}}_LL706: {unsigned int _tmpDA3;int _tmpDA4;struct
_tuple13 _tmpDA2=Cyc_Evexp_eval_const_uint_exp(_tmpD60);_tmpDA3=_tmpDA2.f1;
_tmpDA4=_tmpDA2.f2;if(!_tmpDA4)return bogus_ans;{struct _tuple11*_tmpDA5=Cyc_Absyn_lookup_tuple_field(
_tmpD99,(int)_tmpDA3);if(_tmpDA5 != 0){struct _tuple16 _tmp1380;return(_tmp1380.f1=((*
_tmpDA5).f1).real_const,((_tmp1380.f2=(Cyc_Tcutil_addressof_props(te,_tmpD5F)).f2,
_tmp1380)));}return bogus_ans;}}_LL707: {struct Cyc_Absyn_PointerType_struct*
_tmpD9A=(struct Cyc_Absyn_PointerType_struct*)_tmpD97;if(_tmpD9A->tag != 5)goto
_LL709;else{_tmpD9B=_tmpD9A->f1;_tmpD9C=_tmpD9B.elt_tq;_tmpD9D=_tmpD9B.ptr_atts;
_tmpD9E=_tmpD9D.rgn;}}_LL708: {struct _tuple16 _tmp1381;return(_tmp1381.f1=_tmpD9C.real_const,((
_tmp1381.f2=_tmpD9E,_tmp1381)));}_LL709: {struct Cyc_Absyn_ArrayType_struct*
_tmpD9F=(struct Cyc_Absyn_ArrayType_struct*)_tmpD97;if(_tmpD9F->tag != 9)goto
_LL70B;else{_tmpDA0=_tmpD9F->f1;_tmpDA1=_tmpDA0.tq;}}_LL70A: {struct _tuple16
_tmp1382;return(_tmp1382.f1=_tmpDA1.real_const,((_tmp1382.f2=(Cyc_Tcutil_addressof_props(
te,_tmpD5F)).f2,_tmp1382)));}_LL70B:;_LL70C: return bogus_ans;_LL704:;}_LL6D3:;
_LL6D4:{const char*_tmp1385;void*_tmp1384;(_tmp1384=0,Cyc_Tcutil_terr(e->loc,((
_tmp1385="unary & applied to non-lvalue",_tag_dyneither(_tmp1385,sizeof(char),30))),
_tag_dyneither(_tmp1384,sizeof(void*),0)));}return bogus_ans;_LL6C8:;}void*Cyc_Tcutil_array_to_ptr(
struct Cyc_Tcenv_Tenv*te,void*e_typ,struct Cyc_Absyn_Exp*e);void*Cyc_Tcutil_array_to_ptr(
struct Cyc_Tcenv_Tenv*te,void*e_typ,struct Cyc_Absyn_Exp*e){void*_tmpDAC=Cyc_Tcutil_compress(
e_typ);struct Cyc_Absyn_ArrayInfo _tmpDAE;void*_tmpDAF;struct Cyc_Absyn_Tqual
_tmpDB0;union Cyc_Absyn_Constraint*_tmpDB1;_LL70E: {struct Cyc_Absyn_ArrayType_struct*
_tmpDAD=(struct Cyc_Absyn_ArrayType_struct*)_tmpDAC;if(_tmpDAD->tag != 9)goto
_LL710;else{_tmpDAE=_tmpDAD->f1;_tmpDAF=_tmpDAE.elt_type;_tmpDB0=_tmpDAE.tq;
_tmpDB1=_tmpDAE.zero_term;}}_LL70F: {void*_tmpDB3;struct _tuple16 _tmpDB2=Cyc_Tcutil_addressof_props(
te,e);_tmpDB3=_tmpDB2.f2;{struct Cyc_Absyn_Upper_b_struct _tmp1388;struct Cyc_Absyn_Upper_b_struct*
_tmp1387;return Cyc_Absyn_atb_typ(_tmpDAF,_tmpDB3,_tmpDB0,(void*)((_tmp1387=
_cycalloc(sizeof(*_tmp1387)),((_tmp1387[0]=((_tmp1388.tag=1,((_tmp1388.f1=e,
_tmp1388)))),_tmp1387)))),_tmpDB1);}}_LL710:;_LL711: return e_typ;_LL70D:;}void Cyc_Tcutil_check_bound(
struct Cyc_Position_Segment*loc,unsigned int i,union Cyc_Absyn_Constraint*b);void
Cyc_Tcutil_check_bound(struct Cyc_Position_Segment*loc,unsigned int i,union Cyc_Absyn_Constraint*
b){b=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)(
b);{void*_tmpDB6=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(
Cyc_Absyn_bounds_one,b);struct Cyc_Absyn_Exp*_tmpDB9;_LL713: {struct Cyc_Absyn_DynEither_b_struct*
_tmpDB7=(struct Cyc_Absyn_DynEither_b_struct*)_tmpDB6;if(_tmpDB7->tag != 0)goto
_LL715;}_LL714: return;_LL715: {struct Cyc_Absyn_Upper_b_struct*_tmpDB8=(struct Cyc_Absyn_Upper_b_struct*)
_tmpDB6;if(_tmpDB8->tag != 1)goto _LL712;else{_tmpDB9=_tmpDB8->f1;}}_LL716: {
unsigned int _tmpDBB;int _tmpDBC;struct _tuple13 _tmpDBA=Cyc_Evexp_eval_const_uint_exp(
_tmpDB9);_tmpDBB=_tmpDBA.f1;_tmpDBC=_tmpDBA.f2;if(_tmpDBC  && _tmpDBB <= i){const
char*_tmp138D;void*_tmp138C[2];struct Cyc_Int_pa_struct _tmp138B;struct Cyc_Int_pa_struct
_tmp138A;(_tmp138A.tag=1,((_tmp138A.f1=(unsigned long)((int)i),((_tmp138B.tag=1,((
_tmp138B.f1=(unsigned long)((int)_tmpDBB),((_tmp138C[0]=& _tmp138B,((_tmp138C[1]=&
_tmp138A,Cyc_Tcutil_terr(loc,((_tmp138D="dereference is out of bounds: %d <= %d",
_tag_dyneither(_tmp138D,sizeof(char),39))),_tag_dyneither(_tmp138C,sizeof(void*),
2)))))))))))));}return;}_LL712:;}}void Cyc_Tcutil_check_nonzero_bound(struct Cyc_Position_Segment*
loc,union Cyc_Absyn_Constraint*b);void Cyc_Tcutil_check_nonzero_bound(struct Cyc_Position_Segment*
loc,union Cyc_Absyn_Constraint*b){Cyc_Tcutil_check_bound(loc,0,b);}int Cyc_Tcutil_is_bound_one(
union Cyc_Absyn_Constraint*b);int Cyc_Tcutil_is_bound_one(union Cyc_Absyn_Constraint*
b){void*_tmpDC1=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((
void*)& Cyc_Absyn_DynEither_b_val,b);struct Cyc_Absyn_Exp*_tmpDC3;_LL718: {struct
Cyc_Absyn_Upper_b_struct*_tmpDC2=(struct Cyc_Absyn_Upper_b_struct*)_tmpDC1;if(
_tmpDC2->tag != 1)goto _LL71A;else{_tmpDC3=_tmpDC2->f1;}}_LL719: {unsigned int
_tmpDC5;int _tmpDC6;struct _tuple13 _tmpDC4=Cyc_Evexp_eval_const_uint_exp(_tmpDC3);
_tmpDC5=_tmpDC4.f1;_tmpDC6=_tmpDC4.f2;return _tmpDC6  && _tmpDC5 == 1;}_LL71A:;
_LL71B: return 0;_LL717:;}int Cyc_Tcutil_bits_only(void*t);int Cyc_Tcutil_bits_only(
void*t){void*_tmpDC7=Cyc_Tcutil_compress(t);struct Cyc_Absyn_ArrayInfo _tmpDCF;
void*_tmpDD0;union Cyc_Absyn_Constraint*_tmpDD1;struct Cyc_List_List*_tmpDD3;
struct Cyc_Absyn_AggrInfo _tmpDD5;union Cyc_Absyn_AggrInfoU _tmpDD6;struct _tuple4
_tmpDD7;struct Cyc_Absyn_AggrInfo _tmpDD9;union Cyc_Absyn_AggrInfoU _tmpDDA;struct
Cyc_Absyn_Aggrdecl**_tmpDDB;struct Cyc_Absyn_Aggrdecl*_tmpDDC;struct Cyc_List_List*
_tmpDDD;struct Cyc_List_List*_tmpDDF;_LL71D: {struct Cyc_Absyn_VoidType_struct*
_tmpDC8=(struct Cyc_Absyn_VoidType_struct*)_tmpDC7;if(_tmpDC8->tag != 0)goto _LL71F;}
_LL71E: goto _LL720;_LL71F: {struct Cyc_Absyn_IntType_struct*_tmpDC9=(struct Cyc_Absyn_IntType_struct*)
_tmpDC7;if(_tmpDC9->tag != 6)goto _LL721;}_LL720: goto _LL722;_LL721: {struct Cyc_Absyn_FloatType_struct*
_tmpDCA=(struct Cyc_Absyn_FloatType_struct*)_tmpDC7;if(_tmpDCA->tag != 7)goto
_LL723;}_LL722: goto _LL724;_LL723: {struct Cyc_Absyn_DoubleType_struct*_tmpDCB=(
struct Cyc_Absyn_DoubleType_struct*)_tmpDC7;if(_tmpDCB->tag != 8)goto _LL725;}
_LL724: return 1;_LL725: {struct Cyc_Absyn_EnumType_struct*_tmpDCC=(struct Cyc_Absyn_EnumType_struct*)
_tmpDC7;if(_tmpDCC->tag != 14)goto _LL727;}_LL726: goto _LL728;_LL727: {struct Cyc_Absyn_AnonEnumType_struct*
_tmpDCD=(struct Cyc_Absyn_AnonEnumType_struct*)_tmpDC7;if(_tmpDCD->tag != 15)goto
_LL729;}_LL728: return 0;_LL729: {struct Cyc_Absyn_ArrayType_struct*_tmpDCE=(struct
Cyc_Absyn_ArrayType_struct*)_tmpDC7;if(_tmpDCE->tag != 9)goto _LL72B;else{_tmpDCF=
_tmpDCE->f1;_tmpDD0=_tmpDCF.elt_type;_tmpDD1=_tmpDCF.zero_term;}}_LL72A: return !((
int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpDD1) && Cyc_Tcutil_bits_only(
_tmpDD0);_LL72B: {struct Cyc_Absyn_TupleType_struct*_tmpDD2=(struct Cyc_Absyn_TupleType_struct*)
_tmpDC7;if(_tmpDD2->tag != 11)goto _LL72D;else{_tmpDD3=_tmpDD2->f1;}}_LL72C: for(0;
_tmpDD3 != 0;_tmpDD3=_tmpDD3->tl){if(!Cyc_Tcutil_bits_only((*((struct _tuple11*)
_tmpDD3->hd)).f2))return 0;}return 1;_LL72D: {struct Cyc_Absyn_AggrType_struct*
_tmpDD4=(struct Cyc_Absyn_AggrType_struct*)_tmpDC7;if(_tmpDD4->tag != 12)goto
_LL72F;else{_tmpDD5=_tmpDD4->f1;_tmpDD6=_tmpDD5.aggr_info;if((_tmpDD6.UnknownAggr).tag
!= 1)goto _LL72F;_tmpDD7=(struct _tuple4)(_tmpDD6.UnknownAggr).val;}}_LL72E: return
0;_LL72F: {struct Cyc_Absyn_AggrType_struct*_tmpDD8=(struct Cyc_Absyn_AggrType_struct*)
_tmpDC7;if(_tmpDD8->tag != 12)goto _LL731;else{_tmpDD9=_tmpDD8->f1;_tmpDDA=_tmpDD9.aggr_info;
if((_tmpDDA.KnownAggr).tag != 2)goto _LL731;_tmpDDB=(struct Cyc_Absyn_Aggrdecl**)(
_tmpDDA.KnownAggr).val;_tmpDDC=*_tmpDDB;_tmpDDD=_tmpDD9.targs;}}_LL730: if(
_tmpDDC->impl == 0)return 0;{struct _RegionHandle _tmpDE0=_new_region("rgn");struct
_RegionHandle*rgn=& _tmpDE0;_push_region(rgn);{struct Cyc_List_List*_tmpDE1=((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmpDDC->tvs,_tmpDDD);{struct Cyc_List_List*
fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpDDC->impl))->fields;for(0;fs
!= 0;fs=fs->tl){if(!Cyc_Tcutil_bits_only(Cyc_Tcutil_rsubstitute(rgn,_tmpDE1,((
struct Cyc_Absyn_Aggrfield*)fs->hd)->type))){int _tmpDE2=0;_npop_handler(0);return
_tmpDE2;}}}{int _tmpDE3=1;_npop_handler(0);return _tmpDE3;}};_pop_region(rgn);}
_LL731: {struct Cyc_Absyn_AnonAggrType_struct*_tmpDDE=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmpDC7;if(_tmpDDE->tag != 13)goto _LL733;else{_tmpDDF=_tmpDDE->f2;}}_LL732: for(0;
_tmpDDF != 0;_tmpDDF=_tmpDDF->tl){if(!Cyc_Tcutil_bits_only(((struct Cyc_Absyn_Aggrfield*)
_tmpDDF->hd)->type))return 0;}return 1;_LL733:;_LL734: return 0;_LL71C:;}struct
_tuple30{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};static int Cyc_Tcutil_cnst_exp(
struct Cyc_Tcenv_Tenv*te,int var_okay,struct Cyc_Absyn_Exp*e);static int Cyc_Tcutil_cnst_exp(
struct Cyc_Tcenv_Tenv*te,int var_okay,struct Cyc_Absyn_Exp*e){void*_tmpDE4=e->r;
struct _tuple2*_tmpDED;void*_tmpDEE;struct Cyc_Absyn_Exp*_tmpDF0;struct Cyc_Absyn_Exp*
_tmpDF1;struct Cyc_Absyn_Exp*_tmpDF2;struct Cyc_Absyn_Exp*_tmpDF4;struct Cyc_Absyn_Exp*
_tmpDF5;struct Cyc_Absyn_Exp*_tmpDF7;struct Cyc_Absyn_Exp*_tmpDF9;void*_tmpDFB;
struct Cyc_Absyn_Exp*_tmpDFC;enum Cyc_Absyn_Coercion _tmpDFD;void*_tmpDFF;struct
Cyc_Absyn_Exp*_tmpE00;struct Cyc_Absyn_Exp*_tmpE02;struct Cyc_Absyn_Exp*_tmpE04;
struct Cyc_Absyn_Exp*_tmpE05;struct Cyc_List_List*_tmpE07;struct Cyc_List_List*
_tmpE09;struct Cyc_List_List*_tmpE0B;enum Cyc_Absyn_Primop _tmpE0D;struct Cyc_List_List*
_tmpE0E;struct Cyc_List_List*_tmpE10;struct Cyc_List_List*_tmpE12;_LL736: {struct
Cyc_Absyn_Const_e_struct*_tmpDE5=(struct Cyc_Absyn_Const_e_struct*)_tmpDE4;if(
_tmpDE5->tag != 0)goto _LL738;}_LL737: goto _LL739;_LL738: {struct Cyc_Absyn_Sizeoftyp_e_struct*
_tmpDE6=(struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmpDE4;if(_tmpDE6->tag != 18)goto
_LL73A;}_LL739: goto _LL73B;_LL73A: {struct Cyc_Absyn_Sizeofexp_e_struct*_tmpDE7=(
struct Cyc_Absyn_Sizeofexp_e_struct*)_tmpDE4;if(_tmpDE7->tag != 19)goto _LL73C;}
_LL73B: goto _LL73D;_LL73C: {struct Cyc_Absyn_Offsetof_e_struct*_tmpDE8=(struct Cyc_Absyn_Offsetof_e_struct*)
_tmpDE4;if(_tmpDE8->tag != 20)goto _LL73E;}_LL73D: goto _LL73F;_LL73E: {struct Cyc_Absyn_Gentyp_e_struct*
_tmpDE9=(struct Cyc_Absyn_Gentyp_e_struct*)_tmpDE4;if(_tmpDE9->tag != 21)goto
_LL740;}_LL73F: goto _LL741;_LL740: {struct Cyc_Absyn_Enum_e_struct*_tmpDEA=(struct
Cyc_Absyn_Enum_e_struct*)_tmpDE4;if(_tmpDEA->tag != 33)goto _LL742;}_LL741: goto
_LL743;_LL742: {struct Cyc_Absyn_AnonEnum_e_struct*_tmpDEB=(struct Cyc_Absyn_AnonEnum_e_struct*)
_tmpDE4;if(_tmpDEB->tag != 34)goto _LL744;}_LL743: return 1;_LL744: {struct Cyc_Absyn_Var_e_struct*
_tmpDEC=(struct Cyc_Absyn_Var_e_struct*)_tmpDE4;if(_tmpDEC->tag != 1)goto _LL746;
else{_tmpDED=_tmpDEC->f1;_tmpDEE=(void*)_tmpDEC->f2;}}_LL745: {void*_tmpE13=
_tmpDEE;struct Cyc_Absyn_Vardecl*_tmpE16;_LL765: {struct Cyc_Absyn_Funname_b_struct*
_tmpE14=(struct Cyc_Absyn_Funname_b_struct*)_tmpE13;if(_tmpE14->tag != 2)goto
_LL767;}_LL766: return 1;_LL767: {struct Cyc_Absyn_Global_b_struct*_tmpE15=(struct
Cyc_Absyn_Global_b_struct*)_tmpE13;if(_tmpE15->tag != 1)goto _LL769;else{_tmpE16=
_tmpE15->f1;}}_LL768: {void*_tmpE18=Cyc_Tcutil_compress(_tmpE16->type);_LL76E: {
struct Cyc_Absyn_ArrayType_struct*_tmpE19=(struct Cyc_Absyn_ArrayType_struct*)
_tmpE18;if(_tmpE19->tag != 9)goto _LL770;}_LL76F: goto _LL771;_LL770: {struct Cyc_Absyn_FnType_struct*
_tmpE1A=(struct Cyc_Absyn_FnType_struct*)_tmpE18;if(_tmpE1A->tag != 10)goto _LL772;}
_LL771: return 1;_LL772:;_LL773: return var_okay;_LL76D:;}_LL769: {struct Cyc_Absyn_Unresolved_b_struct*
_tmpE17=(struct Cyc_Absyn_Unresolved_b_struct*)_tmpE13;if(_tmpE17->tag != 0)goto
_LL76B;}_LL76A: return 0;_LL76B:;_LL76C: return var_okay;_LL764:;}_LL746: {struct Cyc_Absyn_Conditional_e_struct*
_tmpDEF=(struct Cyc_Absyn_Conditional_e_struct*)_tmpDE4;if(_tmpDEF->tag != 6)goto
_LL748;else{_tmpDF0=_tmpDEF->f1;_tmpDF1=_tmpDEF->f2;_tmpDF2=_tmpDEF->f3;}}_LL747:
return(Cyc_Tcutil_cnst_exp(te,0,_tmpDF0) && Cyc_Tcutil_cnst_exp(te,0,_tmpDF1))
 && Cyc_Tcutil_cnst_exp(te,0,_tmpDF2);_LL748: {struct Cyc_Absyn_SeqExp_e_struct*
_tmpDF3=(struct Cyc_Absyn_SeqExp_e_struct*)_tmpDE4;if(_tmpDF3->tag != 9)goto _LL74A;
else{_tmpDF4=_tmpDF3->f1;_tmpDF5=_tmpDF3->f2;}}_LL749: return Cyc_Tcutil_cnst_exp(
te,0,_tmpDF4) && Cyc_Tcutil_cnst_exp(te,0,_tmpDF5);_LL74A: {struct Cyc_Absyn_NoInstantiate_e_struct*
_tmpDF6=(struct Cyc_Absyn_NoInstantiate_e_struct*)_tmpDE4;if(_tmpDF6->tag != 13)
goto _LL74C;else{_tmpDF7=_tmpDF6->f1;}}_LL74B: _tmpDF9=_tmpDF7;goto _LL74D;_LL74C: {
struct Cyc_Absyn_Instantiate_e_struct*_tmpDF8=(struct Cyc_Absyn_Instantiate_e_struct*)
_tmpDE4;if(_tmpDF8->tag != 14)goto _LL74E;else{_tmpDF9=_tmpDF8->f1;}}_LL74D: return
Cyc_Tcutil_cnst_exp(te,var_okay,_tmpDF9);_LL74E: {struct Cyc_Absyn_Cast_e_struct*
_tmpDFA=(struct Cyc_Absyn_Cast_e_struct*)_tmpDE4;if(_tmpDFA->tag != 15)goto _LL750;
else{_tmpDFB=(void*)_tmpDFA->f1;_tmpDFC=_tmpDFA->f2;_tmpDFD=_tmpDFA->f4;if(
_tmpDFD != Cyc_Absyn_No_coercion)goto _LL750;}}_LL74F: return Cyc_Tcutil_cnst_exp(te,
var_okay,_tmpDFC);_LL750: {struct Cyc_Absyn_Cast_e_struct*_tmpDFE=(struct Cyc_Absyn_Cast_e_struct*)
_tmpDE4;if(_tmpDFE->tag != 15)goto _LL752;else{_tmpDFF=(void*)_tmpDFE->f1;_tmpE00=
_tmpDFE->f2;}}_LL751: return Cyc_Tcutil_cnst_exp(te,var_okay,_tmpE00);_LL752: {
struct Cyc_Absyn_Address_e_struct*_tmpE01=(struct Cyc_Absyn_Address_e_struct*)
_tmpDE4;if(_tmpE01->tag != 16)goto _LL754;else{_tmpE02=_tmpE01->f1;}}_LL753: return
Cyc_Tcutil_cnst_exp(te,1,_tmpE02);_LL754: {struct Cyc_Absyn_Comprehension_e_struct*
_tmpE03=(struct Cyc_Absyn_Comprehension_e_struct*)_tmpDE4;if(_tmpE03->tag != 29)
goto _LL756;else{_tmpE04=_tmpE03->f2;_tmpE05=_tmpE03->f3;}}_LL755: return Cyc_Tcutil_cnst_exp(
te,0,_tmpE04) && Cyc_Tcutil_cnst_exp(te,0,_tmpE05);_LL756: {struct Cyc_Absyn_Array_e_struct*
_tmpE06=(struct Cyc_Absyn_Array_e_struct*)_tmpDE4;if(_tmpE06->tag != 28)goto _LL758;
else{_tmpE07=_tmpE06->f1;}}_LL757: _tmpE09=_tmpE07;goto _LL759;_LL758: {struct Cyc_Absyn_AnonStruct_e_struct*
_tmpE08=(struct Cyc_Absyn_AnonStruct_e_struct*)_tmpDE4;if(_tmpE08->tag != 31)goto
_LL75A;else{_tmpE09=_tmpE08->f2;}}_LL759: _tmpE0B=_tmpE09;goto _LL75B;_LL75A: {
struct Cyc_Absyn_Aggregate_e_struct*_tmpE0A=(struct Cyc_Absyn_Aggregate_e_struct*)
_tmpDE4;if(_tmpE0A->tag != 30)goto _LL75C;else{_tmpE0B=_tmpE0A->f3;}}_LL75B: for(0;
_tmpE0B != 0;_tmpE0B=_tmpE0B->tl){if(!Cyc_Tcutil_cnst_exp(te,0,(*((struct _tuple30*)
_tmpE0B->hd)).f2))return 0;}return 1;_LL75C: {struct Cyc_Absyn_Primop_e_struct*
_tmpE0C=(struct Cyc_Absyn_Primop_e_struct*)_tmpDE4;if(_tmpE0C->tag != 3)goto _LL75E;
else{_tmpE0D=_tmpE0C->f1;_tmpE0E=_tmpE0C->f2;}}_LL75D: _tmpE10=_tmpE0E;goto _LL75F;
_LL75E: {struct Cyc_Absyn_Tuple_e_struct*_tmpE0F=(struct Cyc_Absyn_Tuple_e_struct*)
_tmpDE4;if(_tmpE0F->tag != 26)goto _LL760;else{_tmpE10=_tmpE0F->f1;}}_LL75F:
_tmpE12=_tmpE10;goto _LL761;_LL760: {struct Cyc_Absyn_Datatype_e_struct*_tmpE11=(
struct Cyc_Absyn_Datatype_e_struct*)_tmpDE4;if(_tmpE11->tag != 32)goto _LL762;else{
_tmpE12=_tmpE11->f1;}}_LL761: for(0;_tmpE12 != 0;_tmpE12=_tmpE12->tl){if(!Cyc_Tcutil_cnst_exp(
te,0,(struct Cyc_Absyn_Exp*)_tmpE12->hd))return 0;}return 1;_LL762:;_LL763: return 0;
_LL735:;}int Cyc_Tcutil_is_const_exp(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*
e);int Cyc_Tcutil_is_const_exp(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
return Cyc_Tcutil_cnst_exp(te,0,e);}static int Cyc_Tcutil_fields_support_default(
struct Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs);int Cyc_Tcutil_supports_default(
void*t);int Cyc_Tcutil_supports_default(void*t){void*_tmpE1B=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_PtrInfo _tmpE21;struct Cyc_Absyn_PtrAtts _tmpE22;union Cyc_Absyn_Constraint*
_tmpE23;union Cyc_Absyn_Constraint*_tmpE24;struct Cyc_Absyn_ArrayInfo _tmpE26;void*
_tmpE27;struct Cyc_List_List*_tmpE29;struct Cyc_Absyn_AggrInfo _tmpE2B;union Cyc_Absyn_AggrInfoU
_tmpE2C;struct Cyc_List_List*_tmpE2D;struct Cyc_List_List*_tmpE2F;_LL775: {struct
Cyc_Absyn_VoidType_struct*_tmpE1C=(struct Cyc_Absyn_VoidType_struct*)_tmpE1B;if(
_tmpE1C->tag != 0)goto _LL777;}_LL776: goto _LL778;_LL777: {struct Cyc_Absyn_IntType_struct*
_tmpE1D=(struct Cyc_Absyn_IntType_struct*)_tmpE1B;if(_tmpE1D->tag != 6)goto _LL779;}
_LL778: goto _LL77A;_LL779: {struct Cyc_Absyn_FloatType_struct*_tmpE1E=(struct Cyc_Absyn_FloatType_struct*)
_tmpE1B;if(_tmpE1E->tag != 7)goto _LL77B;}_LL77A: goto _LL77C;_LL77B: {struct Cyc_Absyn_DoubleType_struct*
_tmpE1F=(struct Cyc_Absyn_DoubleType_struct*)_tmpE1B;if(_tmpE1F->tag != 8)goto
_LL77D;}_LL77C: return 1;_LL77D: {struct Cyc_Absyn_PointerType_struct*_tmpE20=(
struct Cyc_Absyn_PointerType_struct*)_tmpE1B;if(_tmpE20->tag != 5)goto _LL77F;else{
_tmpE21=_tmpE20->f1;_tmpE22=_tmpE21.ptr_atts;_tmpE23=_tmpE22.nullable;_tmpE24=
_tmpE22.bounds;}}_LL77E: {void*_tmpE32=((void*(*)(void*y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmpE24);_LL78E: {
struct Cyc_Absyn_DynEither_b_struct*_tmpE33=(struct Cyc_Absyn_DynEither_b_struct*)
_tmpE32;if(_tmpE33->tag != 0)goto _LL790;}_LL78F: return 1;_LL790:;_LL791: return((
int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(1,_tmpE23);_LL78D:;}
_LL77F: {struct Cyc_Absyn_ArrayType_struct*_tmpE25=(struct Cyc_Absyn_ArrayType_struct*)
_tmpE1B;if(_tmpE25->tag != 9)goto _LL781;else{_tmpE26=_tmpE25->f1;_tmpE27=_tmpE26.elt_type;}}
_LL780: return Cyc_Tcutil_supports_default(_tmpE27);_LL781: {struct Cyc_Absyn_TupleType_struct*
_tmpE28=(struct Cyc_Absyn_TupleType_struct*)_tmpE1B;if(_tmpE28->tag != 11)goto
_LL783;else{_tmpE29=_tmpE28->f1;}}_LL782: for(0;_tmpE29 != 0;_tmpE29=_tmpE29->tl){
if(!Cyc_Tcutil_supports_default((*((struct _tuple11*)_tmpE29->hd)).f2))return 0;}
return 1;_LL783: {struct Cyc_Absyn_AggrType_struct*_tmpE2A=(struct Cyc_Absyn_AggrType_struct*)
_tmpE1B;if(_tmpE2A->tag != 12)goto _LL785;else{_tmpE2B=_tmpE2A->f1;_tmpE2C=_tmpE2B.aggr_info;
_tmpE2D=_tmpE2B.targs;}}_LL784: {struct Cyc_Absyn_Aggrdecl*_tmpE34=Cyc_Absyn_get_known_aggrdecl(
_tmpE2C);if(_tmpE34->impl == 0)return 0;if(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmpE34->impl))->exist_vars != 0)return 0;return Cyc_Tcutil_fields_support_default(
_tmpE34->tvs,_tmpE2D,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpE34->impl))->fields);}
_LL785: {struct Cyc_Absyn_AnonAggrType_struct*_tmpE2E=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmpE1B;if(_tmpE2E->tag != 13)goto _LL787;else{_tmpE2F=_tmpE2E->f2;}}_LL786: return
Cyc_Tcutil_fields_support_default(0,0,_tmpE2F);_LL787: {struct Cyc_Absyn_AnonEnumType_struct*
_tmpE30=(struct Cyc_Absyn_AnonEnumType_struct*)_tmpE1B;if(_tmpE30->tag != 15)goto
_LL789;}_LL788: goto _LL78A;_LL789: {struct Cyc_Absyn_EnumType_struct*_tmpE31=(
struct Cyc_Absyn_EnumType_struct*)_tmpE1B;if(_tmpE31->tag != 14)goto _LL78B;}_LL78A:
return 1;_LL78B:;_LL78C: return 0;_LL774:;}static int Cyc_Tcutil_fields_support_default(
struct Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs);static int
Cyc_Tcutil_fields_support_default(struct Cyc_List_List*tvs,struct Cyc_List_List*ts,
struct Cyc_List_List*fs){struct _RegionHandle _tmpE35=_new_region("rgn");struct
_RegionHandle*rgn=& _tmpE35;_push_region(rgn);{struct Cyc_List_List*_tmpE36=((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,tvs,ts);for(0;fs != 0;fs=fs->tl){
void*t=Cyc_Tcutil_rsubstitute(rgn,_tmpE36,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type);
if(!Cyc_Tcutil_supports_default(t)){int _tmpE37=0;_npop_handler(0);return _tmpE37;}}}{
int _tmpE38=1;_npop_handler(0);return _tmpE38;};_pop_region(rgn);}int Cyc_Tcutil_admits_zero(
void*t);int Cyc_Tcutil_admits_zero(void*t){void*_tmpE39=Cyc_Tcutil_compress(t);
struct Cyc_Absyn_PtrInfo _tmpE3E;struct Cyc_Absyn_PtrAtts _tmpE3F;union Cyc_Absyn_Constraint*
_tmpE40;union Cyc_Absyn_Constraint*_tmpE41;_LL793: {struct Cyc_Absyn_IntType_struct*
_tmpE3A=(struct Cyc_Absyn_IntType_struct*)_tmpE39;if(_tmpE3A->tag != 6)goto _LL795;}
_LL794: goto _LL796;_LL795: {struct Cyc_Absyn_FloatType_struct*_tmpE3B=(struct Cyc_Absyn_FloatType_struct*)
_tmpE39;if(_tmpE3B->tag != 7)goto _LL797;}_LL796: goto _LL798;_LL797: {struct Cyc_Absyn_DoubleType_struct*
_tmpE3C=(struct Cyc_Absyn_DoubleType_struct*)_tmpE39;if(_tmpE3C->tag != 8)goto
_LL799;}_LL798: return 1;_LL799: {struct Cyc_Absyn_PointerType_struct*_tmpE3D=(
struct Cyc_Absyn_PointerType_struct*)_tmpE39;if(_tmpE3D->tag != 5)goto _LL79B;else{
_tmpE3E=_tmpE3D->f1;_tmpE3F=_tmpE3E.ptr_atts;_tmpE40=_tmpE3F.nullable;_tmpE41=
_tmpE3F.bounds;}}_LL79A: {void*_tmpE42=((void*(*)(void*y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmpE41);_LL79E: {
struct Cyc_Absyn_DynEither_b_struct*_tmpE43=(struct Cyc_Absyn_DynEither_b_struct*)
_tmpE42;if(_tmpE43->tag != 0)goto _LL7A0;}_LL79F: return 0;_LL7A0:;_LL7A1: return((
int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpE40);_LL79D:;}
_LL79B:;_LL79C: return 0;_LL792:;}int Cyc_Tcutil_is_noreturn(void*t);int Cyc_Tcutil_is_noreturn(
void*t){{void*_tmpE44=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmpE46;void*
_tmpE47;struct Cyc_Absyn_FnInfo _tmpE49;struct Cyc_List_List*_tmpE4A;_LL7A3: {
struct Cyc_Absyn_PointerType_struct*_tmpE45=(struct Cyc_Absyn_PointerType_struct*)
_tmpE44;if(_tmpE45->tag != 5)goto _LL7A5;else{_tmpE46=_tmpE45->f1;_tmpE47=_tmpE46.elt_typ;}}
_LL7A4: return Cyc_Tcutil_is_noreturn(_tmpE47);_LL7A5: {struct Cyc_Absyn_FnType_struct*
_tmpE48=(struct Cyc_Absyn_FnType_struct*)_tmpE44;if(_tmpE48->tag != 10)goto _LL7A7;
else{_tmpE49=_tmpE48->f1;_tmpE4A=_tmpE49.attributes;}}_LL7A6: for(0;_tmpE4A != 0;
_tmpE4A=_tmpE4A->tl){void*_tmpE4B=(void*)_tmpE4A->hd;_LL7AA: {struct Cyc_Absyn_Noreturn_att_struct*
_tmpE4C=(struct Cyc_Absyn_Noreturn_att_struct*)_tmpE4B;if(_tmpE4C->tag != 4)goto
_LL7AC;}_LL7AB: return 1;_LL7AC:;_LL7AD: continue;_LL7A9:;}goto _LL7A2;_LL7A7:;
_LL7A8: goto _LL7A2;_LL7A2:;}return 0;}struct Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(
void*t,struct Cyc_List_List*atts);struct Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(
void*t,struct Cyc_List_List*atts){void*_tmpE4D=Cyc_Tcutil_compress(t);struct Cyc_Absyn_FnInfo
_tmpE4F;struct Cyc_List_List*_tmpE50;struct Cyc_List_List**_tmpE51;_LL7AF: {struct
Cyc_Absyn_FnType_struct*_tmpE4E=(struct Cyc_Absyn_FnType_struct*)_tmpE4D;if(
_tmpE4E->tag != 10)goto _LL7B1;else{_tmpE4F=_tmpE4E->f1;_tmpE50=_tmpE4F.attributes;
_tmpE51=(struct Cyc_List_List**)&(_tmpE4E->f1).attributes;}}_LL7B0: {struct Cyc_List_List*
_tmpE52=0;for(0;atts != 0;atts=atts->tl){if(Cyc_Absyn_fntype_att((void*)atts->hd)){
struct Cyc_List_List*_tmp138E;*_tmpE51=((_tmp138E=_cycalloc(sizeof(*_tmp138E)),((
_tmp138E->hd=(void*)atts->hd,((_tmp138E->tl=*_tmpE51,_tmp138E))))));}else{struct
Cyc_List_List*_tmp138F;_tmpE52=((_tmp138F=_cycalloc(sizeof(*_tmp138F)),((
_tmp138F->hd=(void*)atts->hd,((_tmp138F->tl=_tmpE52,_tmp138F))))));}}return
_tmpE52;}_LL7B1:;_LL7B2: {const char*_tmp1392;void*_tmp1391;(_tmp1391=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1392="transfer_fn_type_atts",
_tag_dyneither(_tmp1392,sizeof(char),22))),_tag_dyneither(_tmp1391,sizeof(void*),
0)));}_LL7AE:;}
