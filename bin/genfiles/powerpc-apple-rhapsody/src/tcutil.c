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
struct Cyc_Absyn_Loc_n_struct{int tag;};struct Cyc_Absyn_Rel_n_struct{int tag;struct
Cyc_List_List*f1;};struct Cyc_Absyn_Abs_n_struct{int tag;struct Cyc_List_List*f1;};
union Cyc_Absyn_Nmspace_union{struct Cyc_Absyn_Loc_n_struct Loc_n;struct Cyc_Absyn_Rel_n_struct
Rel_n;struct Cyc_Absyn_Abs_n_struct Abs_n;};struct _tuple2{union Cyc_Absyn_Nmspace_union
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
Cyc_Absyn_UnknownTunionInfo{struct _tuple2*name;int is_xtunion;int is_flat;};struct
Cyc_Absyn_UnknownTunion_struct{int tag;struct Cyc_Absyn_UnknownTunionInfo f1;};
struct Cyc_Absyn_KnownTunion_struct{int tag;struct Cyc_Absyn_Tuniondecl**f1;};union
Cyc_Absyn_TunionInfoU_union{struct Cyc_Absyn_UnknownTunion_struct UnknownTunion;
struct Cyc_Absyn_KnownTunion_struct KnownTunion;};struct Cyc_Absyn_TunionInfo{union
Cyc_Absyn_TunionInfoU_union tunion_info;struct Cyc_List_List*targs;struct Cyc_Core_Opt*
rgn;};struct Cyc_Absyn_UnknownTunionFieldInfo{struct _tuple2*tunion_name;struct
_tuple2*field_name;int is_xtunion;};struct Cyc_Absyn_UnknownTunionfield_struct{int
tag;struct Cyc_Absyn_UnknownTunionFieldInfo f1;};struct Cyc_Absyn_KnownTunionfield_struct{
int tag;struct Cyc_Absyn_Tuniondecl*f1;struct Cyc_Absyn_Tunionfield*f2;};union Cyc_Absyn_TunionFieldInfoU_union{
struct Cyc_Absyn_UnknownTunionfield_struct UnknownTunionfield;struct Cyc_Absyn_KnownTunionfield_struct
KnownTunionfield;};struct Cyc_Absyn_TunionFieldInfo{union Cyc_Absyn_TunionFieldInfoU_union
field_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownAggr_struct{int tag;
void*f1;struct _tuple2*f2;};struct Cyc_Absyn_KnownAggr_struct{int tag;struct Cyc_Absyn_Aggrdecl**
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
int tag;struct _tuple2*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_struct{int tag;void*
f1;};struct Cyc_Absyn_DynRgnType_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_struct{
int tag;struct _tuple2*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;
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
_tuple2*f1;void*f2;};struct Cyc_Absyn_UnknownId_e_struct{int tag;struct _tuple2*f1;
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
f1;};struct _tuple3{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Tqual f2;void*f3;};
struct Cyc_Absyn_CompoundLit_e_struct{int tag;struct _tuple3*f1;struct Cyc_List_List*
f2;};struct Cyc_Absyn_Array_e_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;
int f4;};struct Cyc_Absyn_Struct_e_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*
f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_struct{
int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Tunion_e_struct{int tag;
struct Cyc_List_List*f1;struct Cyc_Absyn_Tuniondecl*f2;struct Cyc_Absyn_Tunionfield*
f3;};struct Cyc_Absyn_Enum_e_struct{int tag;struct _tuple2*f1;struct Cyc_Absyn_Enumdecl*
f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_struct{int tag;
struct _tuple2*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_struct{
int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_struct{int tag;
struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_struct{
int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_struct{
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Valueof_e_struct{int tag;void*f1;
};struct Cyc_Absyn_Exp{struct Cyc_Core_Opt*topt;void*r;struct Cyc_Position_Segment*
loc;void*annot;};struct Cyc_Absyn_Exp_s_struct{int tag;struct Cyc_Absyn_Exp*f1;};
struct Cyc_Absyn_Seq_s_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*
f2;};struct Cyc_Absyn_Return_s_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};
struct _tuple4{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_struct{
int tag;struct _tuple4 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_struct{
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_struct{int tag;struct
Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_struct{int tag;struct _dyneither_ptr*f1;
struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_struct{int tag;struct Cyc_Absyn_Exp*
f1;struct _tuple4 f2;struct _tuple4 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Fallthru_s_struct{
int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_struct{
int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_struct{
int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_struct{
int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple4 f2;};struct Cyc_Absyn_TryCatch_s_struct{
int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Region_s_struct{
int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*
f4;struct Cyc_Absyn_Stmt*f5;};struct Cyc_Absyn_ResetRegion_s_struct{int tag;struct
Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Alias_s_struct{int tag;struct Cyc_Absyn_Exp*f1;
struct Cyc_Absyn_Tvar*f2;struct Cyc_Absyn_Vardecl*f3;struct Cyc_Absyn_Stmt*f4;};
struct Cyc_Absyn_Stmt{void*r;struct Cyc_Position_Segment*loc;struct Cyc_List_List*
non_local_preds;int try_depth;void*annot;};struct Cyc_Absyn_Var_p_struct{int tag;
struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_Reference_p_struct{
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
f2;};struct Cyc_Absyn_UnknownId_p_struct{int tag;struct _tuple2*f1;};struct Cyc_Absyn_UnknownCall_p_struct{
int tag;struct _tuple2*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;struct Cyc_Core_Opt*
topt;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*
pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*
body;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Global_b_struct{int tag;
struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_struct{int tag;struct Cyc_Absyn_Fndecl*
f1;};struct Cyc_Absyn_Param_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct
Cyc_Absyn_Local_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{void*sc;struct
_tuple2*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;
struct Cyc_Core_Opt*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{
void*sc;int is_inline;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*
effect;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*
cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;struct Cyc_Core_Opt*
cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;
struct Cyc_List_List*attributes;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*
name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*
attributes;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct
Cyc_List_List*rgn_po;struct Cyc_List_List*fields;};struct Cyc_Absyn_Aggrdecl{void*
kind;void*sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*
impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Tunionfield{struct _tuple2*
name;struct Cyc_List_List*typs;struct Cyc_Position_Segment*loc;void*sc;};struct Cyc_Absyn_Tuniondecl{
void*sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int
is_xtunion;int is_flat;};struct Cyc_Absyn_Enumfield{struct _tuple2*name;struct Cyc_Absyn_Exp*
tag;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Enumdecl{void*sc;struct
_tuple2*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct
_tuple2*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*
kind;struct Cyc_Core_Opt*defn;struct Cyc_List_List*atts;};struct Cyc_Absyn_Var_d_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_struct{int tag;struct Cyc_Absyn_Fndecl*
f1;};struct Cyc_Absyn_Let_d_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*
f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Letv_d_struct{int tag;struct Cyc_List_List*
f1;};struct Cyc_Absyn_Aggr_d_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct
Cyc_Absyn_Tunion_d_struct{int tag;struct Cyc_Absyn_Tuniondecl*f1;};struct Cyc_Absyn_Enum_d_struct{
int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_struct{int tag;
struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_struct{int tag;
struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_struct{
int tag;struct _tuple2*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_struct{int tag;
struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Decl{void*r;
struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_ArrayElement_struct{int tag;
struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_struct{int tag;struct
_dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];int Cyc_Absyn_qvar_cmp(
struct _tuple2*,struct _tuple2*);int Cyc_Absyn_tvar_cmp(struct Cyc_Absyn_Tvar*,
struct Cyc_Absyn_Tvar*);struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(struct Cyc_Position_Segment*);
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(struct Cyc_Position_Segment*);struct
Cyc_Absyn_Conref*Cyc_Absyn_new_conref(void*x);struct Cyc_Absyn_Conref*Cyc_Absyn_empty_conref();
struct Cyc_Absyn_Conref*Cyc_Absyn_compress_conref(struct Cyc_Absyn_Conref*x);void*
Cyc_Absyn_conref_val(struct Cyc_Absyn_Conref*x);void*Cyc_Absyn_conref_def(void*,
struct Cyc_Absyn_Conref*x);extern struct Cyc_Absyn_Conref*Cyc_Absyn_true_conref;
extern struct Cyc_Absyn_Conref*Cyc_Absyn_false_conref;extern struct Cyc_Absyn_Conref*
Cyc_Absyn_bounds_one_conref;void*Cyc_Absyn_compress_kb(void*);void*Cyc_Absyn_new_evar(
struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);extern void*Cyc_Absyn_uint_typ;
extern void*Cyc_Absyn_ulong_typ;extern void*Cyc_Absyn_ulonglong_typ;extern void*Cyc_Absyn_sint_typ;
extern void*Cyc_Absyn_slong_typ;extern void*Cyc_Absyn_slonglong_typ;extern void*Cyc_Absyn_empty_effect;
extern struct _tuple2*Cyc_Absyn_tunion_print_arg_qvar;extern struct _tuple2*Cyc_Absyn_tunion_scanf_arg_qvar;
extern void*Cyc_Absyn_bounds_one;void*Cyc_Absyn_atb_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual
tq,void*b,struct Cyc_Absyn_Conref*zero_term);struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(
struct Cyc_Absyn_Exp*);struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,struct
Cyc_Position_Segment*);struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct
Cyc_List_List*,struct _dyneither_ptr*);struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(
struct Cyc_Absyn_Aggrdecl*,struct _dyneither_ptr*);struct _tuple5{struct Cyc_Absyn_Tqual
f1;void*f2;};struct _tuple5*Cyc_Absyn_lookup_tuple_field(struct Cyc_List_List*,int);
struct _dyneither_ptr Cyc_Absyn_attribute2string(void*);int Cyc_Absyn_fntype_att(
void*a);struct _tuple6{void*f1;struct _tuple2*f2;};struct _tuple6 Cyc_Absyn_aggr_kinded_name(
union Cyc_Absyn_AggrInfoU_union);struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(
union Cyc_Absyn_AggrInfoU_union info);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct
Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs: 1;int qvar_to_Cids: 1;int
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
int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Tcenv_TunionRes_struct{int tag;
struct Cyc_Absyn_Tuniondecl*f1;struct Cyc_Absyn_Tunionfield*f2;};struct Cyc_Tcenv_EnumRes_struct{
int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcenv_AnonEnumRes_struct{
int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcenv_Genv{struct
_RegionHandle*grgn;struct Cyc_Set_Set*namespaces;struct Cyc_Dict_Dict aggrdecls;
struct Cyc_Dict_Dict tuniondecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict
typedefs;struct Cyc_Dict_Dict ordinaries;struct Cyc_List_List*availables;};struct
Cyc_Tcenv_Fenv;struct Cyc_Tcenv_Stmt_j_struct{int tag;struct Cyc_Absyn_Stmt*f1;};
struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Dict_Dict ae;struct Cyc_Tcenv_Fenv*
le;int allow_valueof;};void*Cyc_Tcenv_lookup_ordinary(struct _RegionHandle*,struct
Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,struct _tuple2*);struct Cyc_Absyn_Aggrdecl**
Cyc_Tcenv_lookup_aggrdecl(struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,
struct _tuple2*);struct Cyc_Absyn_Tuniondecl**Cyc_Tcenv_lookup_tuniondecl(struct
Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,struct _tuple2*);struct Cyc_Absyn_Enumdecl**
Cyc_Tcenv_lookup_enumdecl(struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,
struct _tuple2*);struct Cyc_Absyn_Typedefdecl*Cyc_Tcenv_lookup_typedefdecl(struct
Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,struct _tuple2*);struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_allow_valueof(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*);struct Cyc_List_List*
Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*);struct Cyc_Core_Opt*Cyc_Tcenv_lookup_opt_type_vars(
struct Cyc_Tcenv_Tenv*te);int Cyc_Tcenv_region_outlives(struct Cyc_Tcenv_Tenv*,void*
r1,void*r2);unsigned long Cyc_strlen(struct _dyneither_ptr s);int Cyc_strcmp(struct
_dyneither_ptr s1,struct _dyneither_ptr s2);int Cyc_strptrcmp(struct _dyneither_ptr*
s1,struct _dyneither_ptr*s2);struct _dyneither_ptr Cyc_strconcat(struct
_dyneither_ptr,struct _dyneither_ptr);struct _tuple7{unsigned int f1;int f2;};struct
_tuple7 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);int Cyc_Evexp_same_const_exp(
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
void*k);void*Cyc_Tcutil_kind_to_bound(void*k);int Cyc_Tcutil_zero_to_null(struct
Cyc_Tcenv_Tenv*,void*t,struct Cyc_Absyn_Exp*e);void*Cyc_Tcutil_max_arithmetic_type(
void*,void*);void Cyc_Tcutil_explain_failure();int Cyc_Tcutil_unify(void*,void*);
int Cyc_Tcutil_typecmp(void*,void*);void*Cyc_Tcutil_substitute(struct Cyc_List_List*,
void*);void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,
void*);int Cyc_Tcutil_subset_effect(int may_constrain_evars,void*e1,void*e2);int
Cyc_Tcutil_region_in_effect(int constrain,void*r,void*e);void*Cyc_Tcutil_fndecl2typ(
struct Cyc_Absyn_Fndecl*);struct _tuple8{struct Cyc_Absyn_Tvar*f1;void*f2;};struct
_tuple8*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*,struct Cyc_Absyn_Tvar*);
struct _tuple9{struct Cyc_List_List*f1;struct _RegionHandle*f2;};struct _tuple8*Cyc_Tcutil_r_make_inst_var(
struct _tuple9*,struct Cyc_Absyn_Tvar*);void Cyc_Tcutil_check_bitfield(struct Cyc_Position_Segment*
loc,struct Cyc_Tcenv_Tenv*te,void*field_typ,struct Cyc_Absyn_Exp*width,struct
_dyneither_ptr*fn);void Cyc_Tcutil_check_contains_assign(struct Cyc_Absyn_Exp*);
void Cyc_Tcutil_check_valid_toplevel_type(struct Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*,
void*);void Cyc_Tcutil_check_fndecl_valid_type(struct Cyc_Position_Segment*,struct
Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Fndecl*);void Cyc_Tcutil_check_type(struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*bound_tvars,void*k,int allow_evars,void*);
void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*vs,struct Cyc_Position_Segment*
loc,struct _dyneither_ptr err_msg);void Cyc_Tcutil_check_unique_tvars(struct Cyc_Position_Segment*,
struct Cyc_List_List*);void Cyc_Tcutil_check_nonzero_bound(struct Cyc_Position_Segment*,
struct Cyc_Absyn_Conref*);void Cyc_Tcutil_check_bound(struct Cyc_Position_Segment*,
unsigned int i,struct Cyc_Absyn_Conref*);int Cyc_Tcutil_is_bound_one(struct Cyc_Absyn_Conref*
b);int Cyc_Tcutil_equal_tqual(struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2);
struct Cyc_List_List*Cyc_Tcutil_resolve_struct_designators(struct _RegionHandle*
rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,struct Cyc_List_List*
fields);int Cyc_Tcutil_is_tagged_pointer_typ(void*);int Cyc_Tcutil_is_tagged_pointer_typ_elt(
void*t,void**elt_typ_dest);int Cyc_Tcutil_is_zero_pointer_typ_elt(void*t,void**
elt_typ_dest);int Cyc_Tcutil_is_zero_ptr_type(void*t,void**ptr_type,int*
is_dyneither,void**elt_type);int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*
e1,void**ptr_type,int*is_dyneither,void**elt_type);int Cyc_Tcutil_is_noalias_pointer(
void*t);int Cyc_Tcutil_is_noalias_path(struct Cyc_Absyn_Exp*e);int Cyc_Tcutil_is_noalias_pointer_or_aggr(
void*t);void*Cyc_Tcutil_array_to_ptr(struct Cyc_Tcenv_Tenv*,void*t,struct Cyc_Absyn_Exp*
e);struct _tuple10{int f1;void*f2;};struct _tuple10 Cyc_Tcutil_addressof_props(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e);void*Cyc_Tcutil_normalize_effect(
void*e);struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k);int Cyc_Tcutil_new_tvar_id();
void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*);void Cyc_Tcutil_add_tvar_identities(
struct Cyc_List_List*);int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*);void Cyc_Tcutil_rewrite_temp_tvar(
struct Cyc_Absyn_Tvar*);int Cyc_Tcutil_same_atts(struct Cyc_List_List*,struct Cyc_List_List*);
int Cyc_Tcutil_bits_only(void*t);int Cyc_Tcutil_is_const_exp(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e);void*Cyc_Tcutil_snd_tqt(struct _tuple5*);int Cyc_Tcutil_supports_default(
void*);int Cyc_Tcutil_admits_zero(void*t);int Cyc_Tcutil_is_noreturn(void*);int Cyc_Tcutil_extract_const_from_typedef(
struct Cyc_Position_Segment*,int declared_const,void*);struct Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(
void*t,struct Cyc_List_List*atts);void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*,void**,
struct Cyc_Absyn_Exp*);void Cyc_Tc_tcAggrdecl(struct Cyc_Tcenv_Tenv*,struct Cyc_Tcenv_Genv*,
struct Cyc_Position_Segment*,struct Cyc_Absyn_Aggrdecl*);void Cyc_Tc_tcTuniondecl(
struct Cyc_Tcenv_Tenv*,struct Cyc_Tcenv_Genv*,struct Cyc_Position_Segment*,struct
Cyc_Absyn_Tuniondecl*);void Cyc_Tc_tcEnumdecl(struct Cyc_Tcenv_Tenv*,struct Cyc_Tcenv_Genv*,
struct Cyc_Position_Segment*,struct Cyc_Absyn_Enumdecl*);extern int Cyc_Cyclone_tovc_r;
char Cyc_Tcutil_Unify[10]="\000\000\000\000Unify\000";void Cyc_Tcutil_unify_it(
void*t1,void*t2);void*Cyc_Tcutil_t1_failure=(void*)0;int Cyc_Tcutil_tq1_const=0;
void*Cyc_Tcutil_t2_failure=(void*)0;int Cyc_Tcutil_tq2_const=0;struct
_dyneither_ptr Cyc_Tcutil_failure_reason=(struct _dyneither_ptr){(void*)0,(void*)0,(
void*)(0 + 0)};void Cyc_Tcutil_explain_failure();void Cyc_Tcutil_explain_failure(){
if(Cyc_Position_num_errors >= Cyc_Position_max_errors)return;Cyc_fflush((struct
Cyc___cycFILE*)Cyc_stderr);{const char*_tmpB63;const char*_tmpB62;const char*
_tmpB61;void*_tmpB60[2];struct Cyc_String_pa_struct _tmpB5F;struct Cyc_String_pa_struct
_tmpB5E;struct _dyneither_ptr s1=(struct _dyneither_ptr)((_tmpB5E.tag=0,((_tmpB5E.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(Cyc_Tcutil_t1_failure)),((
_tmpB5F.tag=0,((_tmpB5F.f1=(struct _dyneither_ptr)(Cyc_Tcutil_tq1_const?(_tmpB62="const ",
_tag_dyneither(_tmpB62,sizeof(char),7)):((_tmpB63="",_tag_dyneither(_tmpB63,
sizeof(char),1)))),((_tmpB60[0]=& _tmpB5F,((_tmpB60[1]=& _tmpB5E,Cyc_aprintf(((
_tmpB61="%s%s",_tag_dyneither(_tmpB61,sizeof(char),5))),_tag_dyneither(_tmpB60,
sizeof(void*),2))))))))))))));const char*_tmpB6C;const char*_tmpB6B;const char*
_tmpB6A;void*_tmpB69[2];struct Cyc_String_pa_struct _tmpB68;struct Cyc_String_pa_struct
_tmpB67;struct _dyneither_ptr s2=(struct _dyneither_ptr)((_tmpB67.tag=0,((_tmpB67.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(Cyc_Tcutil_t2_failure)),((
_tmpB68.tag=0,((_tmpB68.f1=(struct _dyneither_ptr)(Cyc_Tcutil_tq2_const?(_tmpB6B="const ",
_tag_dyneither(_tmpB6B,sizeof(char),7)):((_tmpB6C="",_tag_dyneither(_tmpB6C,
sizeof(char),1)))),((_tmpB69[0]=& _tmpB68,((_tmpB69[1]=& _tmpB67,Cyc_aprintf(((
_tmpB6A="%s%s",_tag_dyneither(_tmpB6A,sizeof(char),5))),_tag_dyneither(_tmpB69,
sizeof(void*),2))))))))))))));int pos=2;{const char*_tmpB70;void*_tmpB6F[1];struct
Cyc_String_pa_struct _tmpB6E;(_tmpB6E.tag=0,((_tmpB6E.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)s1),((_tmpB6F[0]=& _tmpB6E,Cyc_fprintf(Cyc_stderr,((_tmpB70="  %s",
_tag_dyneither(_tmpB70,sizeof(char),5))),_tag_dyneither(_tmpB6F,sizeof(void*),1)))))));}
pos +=_get_dyneither_size(s1,sizeof(char));if(pos + 5 >= 80){{const char*_tmpB73;
void*_tmpB72;(_tmpB72=0,Cyc_fprintf(Cyc_stderr,((_tmpB73="\n\t",_tag_dyneither(
_tmpB73,sizeof(char),3))),_tag_dyneither(_tmpB72,sizeof(void*),0)));}pos=8;}
else{{const char*_tmpB76;void*_tmpB75;(_tmpB75=0,Cyc_fprintf(Cyc_stderr,((_tmpB76=" ",
_tag_dyneither(_tmpB76,sizeof(char),2))),_tag_dyneither(_tmpB75,sizeof(void*),0)));}
++ pos;}{const char*_tmpB79;void*_tmpB78;(_tmpB78=0,Cyc_fprintf(Cyc_stderr,((
_tmpB79="and ",_tag_dyneither(_tmpB79,sizeof(char),5))),_tag_dyneither(_tmpB78,
sizeof(void*),0)));}pos +=4;if(pos + _get_dyneither_size(s2,sizeof(char))>= 80){{
const char*_tmpB7C;void*_tmpB7B;(_tmpB7B=0,Cyc_fprintf(Cyc_stderr,((_tmpB7C="\n\t",
_tag_dyneither(_tmpB7C,sizeof(char),3))),_tag_dyneither(_tmpB7B,sizeof(void*),0)));}
pos=8;}{const char*_tmpB80;void*_tmpB7F[1];struct Cyc_String_pa_struct _tmpB7E;(
_tmpB7E.tag=0,((_tmpB7E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s2),((
_tmpB7F[0]=& _tmpB7E,Cyc_fprintf(Cyc_stderr,((_tmpB80="%s ",_tag_dyneither(
_tmpB80,sizeof(char),4))),_tag_dyneither(_tmpB7F,sizeof(void*),1)))))));}pos +=
_get_dyneither_size(s2,sizeof(char))+ 1;if(pos + 17 >= 80){{const char*_tmpB83;void*
_tmpB82;(_tmpB82=0,Cyc_fprintf(Cyc_stderr,((_tmpB83="\n\t",_tag_dyneither(
_tmpB83,sizeof(char),3))),_tag_dyneither(_tmpB82,sizeof(void*),0)));}pos=8;}{
const char*_tmpB86;void*_tmpB85;(_tmpB85=0,Cyc_fprintf(Cyc_stderr,((_tmpB86="are not compatible. ",
_tag_dyneither(_tmpB86,sizeof(char),21))),_tag_dyneither(_tmpB85,sizeof(void*),0)));}
pos +=17;if(Cyc_Tcutil_failure_reason.curr != ((struct _dyneither_ptr)
_tag_dyneither(0,0,0)).curr){if(pos + Cyc_strlen((struct _dyneither_ptr)Cyc_Tcutil_failure_reason)
>= 80){const char*_tmpB89;void*_tmpB88;(_tmpB88=0,Cyc_fprintf(Cyc_stderr,((
_tmpB89="\n\t",_tag_dyneither(_tmpB89,sizeof(char),3))),_tag_dyneither(_tmpB88,
sizeof(void*),0)));}{const char*_tmpB8D;void*_tmpB8C[1];struct Cyc_String_pa_struct
_tmpB8B;(_tmpB8B.tag=0,((_tmpB8B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Tcutil_failure_reason),((_tmpB8C[0]=& _tmpB8B,Cyc_fprintf(Cyc_stderr,((
_tmpB8D="%s",_tag_dyneither(_tmpB8D,sizeof(char),3))),_tag_dyneither(_tmpB8C,
sizeof(void*),1)))))));}}{const char*_tmpB90;void*_tmpB8F;(_tmpB8F=0,Cyc_fprintf(
Cyc_stderr,((_tmpB90="\n",_tag_dyneither(_tmpB90,sizeof(char),2))),
_tag_dyneither(_tmpB8F,sizeof(void*),0)));}Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);}}
void Cyc_Tcutil_terr(struct Cyc_Position_Segment*loc,struct _dyneither_ptr fmt,
struct _dyneither_ptr ap);void Cyc_Tcutil_terr(struct Cyc_Position_Segment*loc,
struct _dyneither_ptr fmt,struct _dyneither_ptr ap){Cyc_Position_post_error(Cyc_Position_mk_err_elab(
loc,(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap)));}void*Cyc_Tcutil_impos(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap);void*Cyc_Tcutil_impos(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap){struct _dyneither_ptr msg=(struct
_dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap);{const char*_tmpB94;void*
_tmpB93[1];struct Cyc_String_pa_struct _tmpB92;(_tmpB92.tag=0,((_tmpB92.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)msg),((_tmpB93[0]=& _tmpB92,Cyc_fprintf(Cyc_stderr,((
_tmpB94="Compiler Error (Tcutil::impos): %s\n",_tag_dyneither(_tmpB94,sizeof(
char),36))),_tag_dyneither(_tmpB93,sizeof(void*),1)))))));}Cyc_fflush((struct Cyc___cycFILE*)
Cyc_stderr);{struct Cyc_Core_Impossible_struct _tmpB97;struct Cyc_Core_Impossible_struct*
_tmpB96;(int)_throw((void*)((_tmpB96=_cycalloc(sizeof(*_tmpB96)),((_tmpB96[0]=((
_tmpB97.tag=Cyc_Core_Impossible,((_tmpB97.f1=msg,_tmpB97)))),_tmpB96)))));}}
static struct _dyneither_ptr Cyc_Tcutil_tvar2string(struct Cyc_Absyn_Tvar*tv);static
struct _dyneither_ptr Cyc_Tcutil_tvar2string(struct Cyc_Absyn_Tvar*tv){return*tv->name;}
void Cyc_Tcutil_print_tvars(struct Cyc_List_List*tvs);void Cyc_Tcutil_print_tvars(
struct Cyc_List_List*tvs){for(0;tvs != 0;tvs=tvs->tl){const char*_tmpB9C;void*
_tmpB9B[2];struct Cyc_String_pa_struct _tmpB9A;struct Cyc_String_pa_struct _tmpB99;(
_tmpB99.tag=0,((_tmpB99.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kindbound2string((
void*)((struct Cyc_Absyn_Tvar*)tvs->hd)->kind)),((_tmpB9A.tag=0,((_tmpB9A.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_tvar2string((struct Cyc_Absyn_Tvar*)
tvs->hd)),((_tmpB9B[0]=& _tmpB9A,((_tmpB9B[1]=& _tmpB99,Cyc_fprintf(Cyc_stderr,((
_tmpB9C="%s::%s ",_tag_dyneither(_tmpB9C,sizeof(char),8))),_tag_dyneither(
_tmpB9B,sizeof(void*),2)))))))))))));}{const char*_tmpB9F;void*_tmpB9E;(_tmpB9E=0,
Cyc_fprintf(Cyc_stderr,((_tmpB9F="\n",_tag_dyneither(_tmpB9F,sizeof(char),2))),
_tag_dyneither(_tmpB9E,sizeof(void*),0)));}Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);}
static struct Cyc_List_List*Cyc_Tcutil_warning_segs=0;static struct Cyc_List_List*
Cyc_Tcutil_warning_msgs=0;void Cyc_Tcutil_warn(struct Cyc_Position_Segment*sg,
struct _dyneither_ptr fmt,struct _dyneither_ptr ap);void Cyc_Tcutil_warn(struct Cyc_Position_Segment*
sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){struct _dyneither_ptr msg=(
struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap);{struct Cyc_List_List*
_tmpBA0;Cyc_Tcutil_warning_segs=((_tmpBA0=_cycalloc(sizeof(*_tmpBA0)),((_tmpBA0->hd=
sg,((_tmpBA0->tl=Cyc_Tcutil_warning_segs,_tmpBA0))))));}{struct _dyneither_ptr*
_tmpBA3;struct Cyc_List_List*_tmpBA2;Cyc_Tcutil_warning_msgs=((_tmpBA2=_cycalloc(
sizeof(*_tmpBA2)),((_tmpBA2->hd=((_tmpBA3=_cycalloc(sizeof(*_tmpBA3)),((_tmpBA3[
0]=msg,_tmpBA3)))),((_tmpBA2->tl=Cyc_Tcutil_warning_msgs,_tmpBA2))))));}}void Cyc_Tcutil_flush_warnings();
void Cyc_Tcutil_flush_warnings(){if(Cyc_Tcutil_warning_segs == 0)return;{const char*
_tmpBA6;void*_tmpBA5;(_tmpBA5=0,Cyc_fprintf(Cyc_stderr,((_tmpBA6="***Warnings***\n",
_tag_dyneither(_tmpBA6,sizeof(char),16))),_tag_dyneither(_tmpBA5,sizeof(void*),0)));}{
struct Cyc_List_List*_tmp35=Cyc_Position_strings_of_segments(Cyc_Tcutil_warning_segs);
Cyc_Tcutil_warning_segs=0;Cyc_Tcutil_warning_msgs=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_Tcutil_warning_msgs);while(Cyc_Tcutil_warning_msgs
!= 0){{const char*_tmpBAB;void*_tmpBAA[2];struct Cyc_String_pa_struct _tmpBA9;
struct Cyc_String_pa_struct _tmpBA8;(_tmpBA8.tag=0,((_tmpBA8.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)((struct Cyc_List_List*)
_check_null(Cyc_Tcutil_warning_msgs))->hd)),((_tmpBA9.tag=0,((_tmpBA9.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)((struct Cyc_List_List*)
_check_null(_tmp35))->hd)),((_tmpBAA[0]=& _tmpBA9,((_tmpBAA[1]=& _tmpBA8,Cyc_fprintf(
Cyc_stderr,((_tmpBAB="%s: %s\n",_tag_dyneither(_tmpBAB,sizeof(char),8))),
_tag_dyneither(_tmpBAA,sizeof(void*),2)))))))))))));}_tmp35=_tmp35->tl;Cyc_Tcutil_warning_msgs=((
struct Cyc_List_List*)_check_null(Cyc_Tcutil_warning_msgs))->tl;}{const char*
_tmpBAE;void*_tmpBAD;(_tmpBAD=0,Cyc_fprintf(Cyc_stderr,((_tmpBAE="**************\n",
_tag_dyneither(_tmpBAE,sizeof(char),16))),_tag_dyneither(_tmpBAD,sizeof(void*),0)));}
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
_tmpBAF;*_tmp40=((_tmpBAF=_cycalloc(sizeof(*_tmpBAF)),((_tmpBAF[0]=t2,_tmpBAF))));}
return t2;}_LL7: if(*((int*)_tmp3C)!= 0)goto _LL9;_tmp41=((struct Cyc_Absyn_Evar_struct*)
_tmp3C)->f2;_tmp42=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)_tmp3C)->f2;
_LL8: {void*t2=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(*
_tmp42))->v);if(t2 != (void*)((struct Cyc_Core_Opt*)_check_null(*_tmp42))->v){
struct Cyc_Core_Opt*_tmpBB0;*_tmp42=((_tmpBB0=_cycalloc(sizeof(*_tmpBB0)),((
_tmpBB0->v=(void*)t2,_tmpBB0))));}return t2;}_LL9:;_LLA: return t;_LL0:;}void*Cyc_Tcutil_copy_type(
void*t);static struct Cyc_List_List*Cyc_Tcutil_copy_types(struct Cyc_List_List*ts);
static struct Cyc_List_List*Cyc_Tcutil_copy_types(struct Cyc_List_List*ts){return
Cyc_List_map(Cyc_Tcutil_copy_type,ts);}static struct Cyc_Absyn_Conref*Cyc_Tcutil_copy_conref(
struct Cyc_Absyn_Conref*c);static struct Cyc_Absyn_Conref*Cyc_Tcutil_copy_conref(
struct Cyc_Absyn_Conref*c){union Cyc_Absyn_Constraint_union _tmp45=c->v;void*_tmp46;
struct Cyc_Absyn_Conref*_tmp47;_LLC: if((_tmp45.No_constr).tag != 2)goto _LLE;_LLD:
return Cyc_Absyn_empty_conref();_LLE: if((_tmp45.Eq_constr).tag != 0)goto _LL10;
_tmp46=(_tmp45.Eq_constr).f1;_LLF: return Cyc_Absyn_new_conref(_tmp46);_LL10: if((
_tmp45.Forward_constr).tag != 1)goto _LLB;_tmp47=(_tmp45.Forward_constr).f1;_LL11:
return Cyc_Tcutil_copy_conref(_tmp47);_LLB:;}static void*Cyc_Tcutil_copy_kindbound(
void*kb);static void*Cyc_Tcutil_copy_kindbound(void*kb){void*_tmp48=Cyc_Absyn_compress_kb(
kb);void*_tmp49;_LL13: if(*((int*)_tmp48)!= 1)goto _LL15;_LL14: {struct Cyc_Absyn_Unknown_kb_struct
_tmpBB3;struct Cyc_Absyn_Unknown_kb_struct*_tmpBB2;return(void*)((_tmpBB2=
_cycalloc(sizeof(*_tmpBB2)),((_tmpBB2[0]=((_tmpBB3.tag=1,((_tmpBB3.f1=0,_tmpBB3)))),
_tmpBB2))));}_LL15: if(*((int*)_tmp48)!= 2)goto _LL17;_tmp49=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp48)->f2;_LL16: {struct Cyc_Absyn_Less_kb_struct _tmpBB6;struct Cyc_Absyn_Less_kb_struct*
_tmpBB5;return(void*)((_tmpBB5=_cycalloc(sizeof(*_tmpBB5)),((_tmpBB5[0]=((
_tmpBB6.tag=2,((_tmpBB6.f1=0,((_tmpBB6.f2=(void*)_tmp49,_tmpBB6)))))),_tmpBB5))));}
_LL17:;_LL18: return kb;_LL12:;}static struct Cyc_Absyn_Tvar*Cyc_Tcutil_copy_tvar(
struct Cyc_Absyn_Tvar*tv);static struct Cyc_Absyn_Tvar*Cyc_Tcutil_copy_tvar(struct
Cyc_Absyn_Tvar*tv){struct Cyc_Absyn_Tvar*_tmpBB7;return(_tmpBB7=_cycalloc(sizeof(*
_tmpBB7)),((_tmpBB7->name=tv->name,((_tmpBB7->identity=- 1,((_tmpBB7->kind=(void*)
Cyc_Tcutil_copy_kindbound((void*)tv->kind),_tmpBB7)))))));}static struct _tuple3*
Cyc_Tcutil_copy_arg(struct _tuple3*arg);static struct _tuple3*Cyc_Tcutil_copy_arg(
struct _tuple3*arg){struct _tuple3 _tmp50;struct Cyc_Core_Opt*_tmp51;struct Cyc_Absyn_Tqual
_tmp52;void*_tmp53;struct _tuple3*_tmp4F=arg;_tmp50=*_tmp4F;_tmp51=_tmp50.f1;
_tmp52=_tmp50.f2;_tmp53=_tmp50.f3;{struct _tuple3*_tmpBB8;return(_tmpBB8=
_cycalloc(sizeof(*_tmpBB8)),((_tmpBB8->f1=_tmp51,((_tmpBB8->f2=_tmp52,((_tmpBB8->f3=
Cyc_Tcutil_copy_type(_tmp53),_tmpBB8)))))));}}static struct _tuple5*Cyc_Tcutil_copy_tqt(
struct _tuple5*arg);static struct _tuple5*Cyc_Tcutil_copy_tqt(struct _tuple5*arg){
struct _tuple5 _tmp56;struct Cyc_Absyn_Tqual _tmp57;void*_tmp58;struct _tuple5*_tmp55=
arg;_tmp56=*_tmp55;_tmp57=_tmp56.f1;_tmp58=_tmp56.f2;{struct _tuple5*_tmpBB9;
return(_tmpBB9=_cycalloc(sizeof(*_tmpBB9)),((_tmpBB9->f1=_tmp57,((_tmpBB9->f2=
Cyc_Tcutil_copy_type(_tmp58),_tmpBB9)))));}}static struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_copy_field(
struct Cyc_Absyn_Aggrfield*f);static struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_copy_field(
struct Cyc_Absyn_Aggrfield*f){struct Cyc_Absyn_Aggrfield*_tmpBBA;return(_tmpBBA=
_cycalloc(sizeof(*_tmpBBA)),((_tmpBBA->name=f->name,((_tmpBBA->tq=f->tq,((
_tmpBBA->type=(void*)Cyc_Tcutil_copy_type((void*)f->type),((_tmpBBA->width=f->width,((
_tmpBBA->attributes=f->attributes,_tmpBBA)))))))))));}static struct _tuple0*Cyc_Tcutil_copy_rgncmp(
struct _tuple0*x);static struct _tuple0*Cyc_Tcutil_copy_rgncmp(struct _tuple0*x){
struct _tuple0 _tmp5C;void*_tmp5D;void*_tmp5E;struct _tuple0*_tmp5B=x;_tmp5C=*
_tmp5B;_tmp5D=_tmp5C.f1;_tmp5E=_tmp5C.f2;{struct _tuple0*_tmpBBB;return(_tmpBBB=
_cycalloc(sizeof(*_tmpBBB)),((_tmpBBB->f1=Cyc_Tcutil_copy_type(_tmp5D),((_tmpBBB->f2=
Cyc_Tcutil_copy_type(_tmp5E),_tmpBBB)))));}}static struct Cyc_Absyn_Enumfield*Cyc_Tcutil_copy_enumfield(
struct Cyc_Absyn_Enumfield*f);static struct Cyc_Absyn_Enumfield*Cyc_Tcutil_copy_enumfield(
struct Cyc_Absyn_Enumfield*f){struct Cyc_Absyn_Enumfield*_tmpBBC;return(_tmpBBC=
_cycalloc(sizeof(*_tmpBBC)),((_tmpBBC->name=f->name,((_tmpBBC->tag=f->tag,((
_tmpBBC->loc=f->loc,_tmpBBC)))))));}void*Cyc_Tcutil_copy_type(void*t);void*Cyc_Tcutil_copy_type(
void*t){void*_tmp61=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*_tmp62;struct Cyc_Absyn_TunionInfo
_tmp63;union Cyc_Absyn_TunionInfoU_union _tmp64;struct Cyc_List_List*_tmp65;struct
Cyc_Core_Opt*_tmp66;struct Cyc_Absyn_TunionFieldInfo _tmp67;union Cyc_Absyn_TunionFieldInfoU_union
_tmp68;struct Cyc_List_List*_tmp69;struct Cyc_Absyn_PtrInfo _tmp6A;void*_tmp6B;
struct Cyc_Absyn_Tqual _tmp6C;struct Cyc_Absyn_PtrAtts _tmp6D;void*_tmp6E;struct Cyc_Absyn_Conref*
_tmp6F;struct Cyc_Absyn_Conref*_tmp70;struct Cyc_Absyn_Conref*_tmp71;struct Cyc_Absyn_PtrLoc*
_tmp72;struct Cyc_Absyn_ArrayInfo _tmp73;void*_tmp74;struct Cyc_Absyn_Tqual _tmp75;
struct Cyc_Absyn_Exp*_tmp76;struct Cyc_Absyn_Conref*_tmp77;struct Cyc_Position_Segment*
_tmp78;struct Cyc_Absyn_FnInfo _tmp79;struct Cyc_List_List*_tmp7A;struct Cyc_Core_Opt*
_tmp7B;void*_tmp7C;struct Cyc_List_List*_tmp7D;int _tmp7E;struct Cyc_Absyn_VarargInfo*
_tmp7F;struct Cyc_List_List*_tmp80;struct Cyc_List_List*_tmp81;struct Cyc_List_List*
_tmp82;struct Cyc_Absyn_AggrInfo _tmp83;union Cyc_Absyn_AggrInfoU_union _tmp84;void*
_tmp85;struct _tuple2*_tmp86;struct Cyc_List_List*_tmp87;struct Cyc_Absyn_AggrInfo
_tmp88;union Cyc_Absyn_AggrInfoU_union _tmp89;struct Cyc_Absyn_Aggrdecl**_tmp8A;
struct Cyc_List_List*_tmp8B;void*_tmp8C;struct Cyc_List_List*_tmp8D;struct _tuple2*
_tmp8E;struct Cyc_Absyn_Enumdecl*_tmp8F;struct Cyc_List_List*_tmp90;void*_tmp91;
struct Cyc_Absyn_Exp*_tmp92;void*_tmp93;void*_tmp94;void*_tmp95;struct _tuple2*
_tmp96;struct Cyc_List_List*_tmp97;struct Cyc_Absyn_Typedefdecl*_tmp98;void*_tmp99;
struct Cyc_List_List*_tmp9A;void*_tmp9B;_LL1A: if((int)_tmp61 != 0)goto _LL1C;_LL1B:
goto _LL1D;_LL1C: if(_tmp61 <= (void*)4)goto _LL28;if(*((int*)_tmp61)!= 0)goto _LL1E;
_LL1D: return t;_LL1E: if(*((int*)_tmp61)!= 1)goto _LL20;_tmp62=((struct Cyc_Absyn_VarType_struct*)
_tmp61)->f1;_LL1F: {struct Cyc_Absyn_VarType_struct _tmpBBF;struct Cyc_Absyn_VarType_struct*
_tmpBBE;return(void*)((_tmpBBE=_cycalloc(sizeof(*_tmpBBE)),((_tmpBBE[0]=((
_tmpBBF.tag=1,((_tmpBBF.f1=Cyc_Tcutil_copy_tvar(_tmp62),_tmpBBF)))),_tmpBBE))));}
_LL20: if(*((int*)_tmp61)!= 2)goto _LL22;_tmp63=((struct Cyc_Absyn_TunionType_struct*)
_tmp61)->f1;_tmp64=_tmp63.tunion_info;_tmp65=_tmp63.targs;_tmp66=_tmp63.rgn;
_LL21: {struct Cyc_Core_Opt*_tmpBC0;struct Cyc_Core_Opt*_tmp9E=(unsigned int)
_tmp66?(_tmpBC0=_cycalloc(sizeof(*_tmpBC0)),((_tmpBC0->v=(void*)Cyc_Tcutil_copy_type((
void*)_tmp66->v),_tmpBC0))): 0;struct Cyc_Absyn_TunionType_struct _tmpBC6;struct Cyc_Absyn_TunionInfo
_tmpBC5;struct Cyc_Absyn_TunionType_struct*_tmpBC4;return(void*)((_tmpBC4=
_cycalloc(sizeof(*_tmpBC4)),((_tmpBC4[0]=((_tmpBC6.tag=2,((_tmpBC6.f1=((_tmpBC5.tunion_info=
_tmp64,((_tmpBC5.targs=Cyc_Tcutil_copy_types(_tmp65),((_tmpBC5.rgn=_tmp9E,
_tmpBC5)))))),_tmpBC6)))),_tmpBC4))));}_LL22: if(*((int*)_tmp61)!= 3)goto _LL24;
_tmp67=((struct Cyc_Absyn_TunionFieldType_struct*)_tmp61)->f1;_tmp68=_tmp67.field_info;
_tmp69=_tmp67.targs;_LL23: {struct Cyc_Absyn_TunionFieldType_struct _tmpBCC;struct
Cyc_Absyn_TunionFieldInfo _tmpBCB;struct Cyc_Absyn_TunionFieldType_struct*_tmpBCA;
return(void*)((_tmpBCA=_cycalloc(sizeof(*_tmpBCA)),((_tmpBCA[0]=((_tmpBCC.tag=3,((
_tmpBCC.f1=((_tmpBCB.field_info=_tmp68,((_tmpBCB.targs=Cyc_Tcutil_copy_types(
_tmp69),_tmpBCB)))),_tmpBCC)))),_tmpBCA))));}_LL24: if(*((int*)_tmp61)!= 4)goto
_LL26;_tmp6A=((struct Cyc_Absyn_PointerType_struct*)_tmp61)->f1;_tmp6B=(void*)
_tmp6A.elt_typ;_tmp6C=_tmp6A.elt_tq;_tmp6D=_tmp6A.ptr_atts;_tmp6E=(void*)_tmp6D.rgn;
_tmp6F=_tmp6D.nullable;_tmp70=_tmp6D.bounds;_tmp71=_tmp6D.zero_term;_tmp72=
_tmp6D.ptrloc;_LL25: {void*_tmpA6=Cyc_Tcutil_copy_type(_tmp6B);void*_tmpA7=Cyc_Tcutil_copy_type(
_tmp6E);struct Cyc_Absyn_Conref*_tmpA8=((struct Cyc_Absyn_Conref*(*)(struct Cyc_Absyn_Conref*
c))Cyc_Tcutil_copy_conref)(_tmp6F);struct Cyc_Absyn_Tqual _tmpA9=_tmp6C;struct Cyc_Absyn_Conref*
_tmpAA=Cyc_Tcutil_copy_conref(_tmp70);struct Cyc_Absyn_Conref*_tmpAB=((struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref*c))Cyc_Tcutil_copy_conref)(_tmp71);struct Cyc_Absyn_PointerType_struct
_tmpBD6;struct Cyc_Absyn_PtrAtts _tmpBD5;struct Cyc_Absyn_PtrInfo _tmpBD4;struct Cyc_Absyn_PointerType_struct*
_tmpBD3;return(void*)((_tmpBD3=_cycalloc(sizeof(*_tmpBD3)),((_tmpBD3[0]=((
_tmpBD6.tag=4,((_tmpBD6.f1=((_tmpBD4.elt_typ=(void*)_tmpA6,((_tmpBD4.elt_tq=
_tmpA9,((_tmpBD4.ptr_atts=((_tmpBD5.rgn=(void*)_tmpA7,((_tmpBD5.nullable=_tmpA8,((
_tmpBD5.bounds=_tmpAA,((_tmpBD5.zero_term=_tmpAB,((_tmpBD5.ptrloc=_tmp72,_tmpBD5)))))))))),
_tmpBD4)))))),_tmpBD6)))),_tmpBD3))));}_LL26: if(*((int*)_tmp61)!= 5)goto _LL28;
_LL27: goto _LL29;_LL28: if((int)_tmp61 != 1)goto _LL2A;_LL29: goto _LL2B;_LL2A: if(
_tmp61 <= (void*)4)goto _LL46;if(*((int*)_tmp61)!= 6)goto _LL2C;_LL2B: return t;_LL2C:
if(*((int*)_tmp61)!= 7)goto _LL2E;_tmp73=((struct Cyc_Absyn_ArrayType_struct*)
_tmp61)->f1;_tmp74=(void*)_tmp73.elt_type;_tmp75=_tmp73.tq;_tmp76=_tmp73.num_elts;
_tmp77=_tmp73.zero_term;_tmp78=_tmp73.zt_loc;_LL2D: {struct Cyc_Absyn_ArrayType_struct
_tmpBDC;struct Cyc_Absyn_ArrayInfo _tmpBDB;struct Cyc_Absyn_ArrayType_struct*
_tmpBDA;return(void*)((_tmpBDA=_cycalloc(sizeof(*_tmpBDA)),((_tmpBDA[0]=((
_tmpBDC.tag=7,((_tmpBDC.f1=((_tmpBDB.elt_type=(void*)Cyc_Tcutil_copy_type(_tmp74),((
_tmpBDB.tq=_tmp75,((_tmpBDB.num_elts=_tmp76,((_tmpBDB.zero_term=((struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref*c))Cyc_Tcutil_copy_conref)(_tmp77),((_tmpBDB.zt_loc=
_tmp78,_tmpBDB)))))))))),_tmpBDC)))),_tmpBDA))));}_LL2E: if(*((int*)_tmp61)!= 8)
goto _LL30;_tmp79=((struct Cyc_Absyn_FnType_struct*)_tmp61)->f1;_tmp7A=_tmp79.tvars;
_tmp7B=_tmp79.effect;_tmp7C=(void*)_tmp79.ret_typ;_tmp7D=_tmp79.args;_tmp7E=
_tmp79.c_varargs;_tmp7F=_tmp79.cyc_varargs;_tmp80=_tmp79.rgn_po;_tmp81=_tmp79.attributes;
_LL2F: {struct Cyc_List_List*_tmpB3=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*
f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_tvar,
_tmp7A);struct Cyc_Core_Opt*_tmpBDD;struct Cyc_Core_Opt*_tmpB4=_tmp7B == 0?0:((
_tmpBDD=_cycalloc(sizeof(*_tmpBDD)),((_tmpBDD->v=(void*)Cyc_Tcutil_copy_type((
void*)_tmp7B->v),_tmpBDD))));void*_tmpB5=Cyc_Tcutil_copy_type(_tmp7C);struct Cyc_List_List*
_tmpB6=((struct Cyc_List_List*(*)(struct _tuple3*(*f)(struct _tuple3*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Tcutil_copy_arg,_tmp7D);int _tmpB7=_tmp7E;struct Cyc_Absyn_VarargInfo*
cyc_varargs2=0;if(_tmp7F != 0){struct Cyc_Absyn_VarargInfo*cv=(struct Cyc_Absyn_VarargInfo*)
_tmp7F;struct Cyc_Absyn_VarargInfo*_tmpBDE;cyc_varargs2=((_tmpBDE=_cycalloc(
sizeof(*_tmpBDE)),((_tmpBDE->name=cv->name,((_tmpBDE->tq=cv->tq,((_tmpBDE->type=(
void*)Cyc_Tcutil_copy_type((void*)cv->type),((_tmpBDE->inject=cv->inject,_tmpBDE))))))))));}{
struct Cyc_List_List*_tmpB9=((struct Cyc_List_List*(*)(struct _tuple0*(*f)(struct
_tuple0*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_rgncmp,_tmp80);
struct Cyc_List_List*_tmpBA=_tmp81;struct Cyc_Absyn_FnType_struct _tmpBE4;struct Cyc_Absyn_FnInfo
_tmpBE3;struct Cyc_Absyn_FnType_struct*_tmpBE2;return(void*)((_tmpBE2=_cycalloc(
sizeof(*_tmpBE2)),((_tmpBE2[0]=((_tmpBE4.tag=8,((_tmpBE4.f1=((_tmpBE3.tvars=
_tmpB3,((_tmpBE3.effect=_tmpB4,((_tmpBE3.ret_typ=(void*)_tmpB5,((_tmpBE3.args=
_tmpB6,((_tmpBE3.c_varargs=_tmpB7,((_tmpBE3.cyc_varargs=cyc_varargs2,((_tmpBE3.rgn_po=
_tmpB9,((_tmpBE3.attributes=_tmpBA,_tmpBE3)))))))))))))))),_tmpBE4)))),_tmpBE2))));}}
_LL30: if(*((int*)_tmp61)!= 9)goto _LL32;_tmp82=((struct Cyc_Absyn_TupleType_struct*)
_tmp61)->f1;_LL31: {struct Cyc_Absyn_TupleType_struct _tmpBE7;struct Cyc_Absyn_TupleType_struct*
_tmpBE6;return(void*)((_tmpBE6=_cycalloc(sizeof(*_tmpBE6)),((_tmpBE6[0]=((
_tmpBE7.tag=9,((_tmpBE7.f1=((struct Cyc_List_List*(*)(struct _tuple5*(*f)(struct
_tuple5*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_tqt,_tmp82),
_tmpBE7)))),_tmpBE6))));}_LL32: if(*((int*)_tmp61)!= 10)goto _LL34;_tmp83=((struct
Cyc_Absyn_AggrType_struct*)_tmp61)->f1;_tmp84=_tmp83.aggr_info;if((((((struct Cyc_Absyn_AggrType_struct*)
_tmp61)->f1).aggr_info).UnknownAggr).tag != 0)goto _LL34;_tmp85=(_tmp84.UnknownAggr).f1;
_tmp86=(_tmp84.UnknownAggr).f2;_tmp87=_tmp83.targs;_LL33: {struct Cyc_Absyn_AggrType_struct
_tmpBF1;union Cyc_Absyn_AggrInfoU_union _tmpBF0;struct Cyc_Absyn_AggrInfo _tmpBEF;
struct Cyc_Absyn_AggrType_struct*_tmpBEE;return(void*)((_tmpBEE=_cycalloc(sizeof(*
_tmpBEE)),((_tmpBEE[0]=((_tmpBF1.tag=10,((_tmpBF1.f1=((_tmpBEF.aggr_info=(union
Cyc_Absyn_AggrInfoU_union)(((_tmpBF0.UnknownAggr).tag=0,(((_tmpBF0.UnknownAggr).f1=(
void*)_tmp85,(((_tmpBF0.UnknownAggr).f2=_tmp86,_tmpBF0)))))),((_tmpBEF.targs=Cyc_Tcutil_copy_types(
_tmp87),_tmpBEF)))),_tmpBF1)))),_tmpBEE))));}_LL34: if(*((int*)_tmp61)!= 10)goto
_LL36;_tmp88=((struct Cyc_Absyn_AggrType_struct*)_tmp61)->f1;_tmp89=_tmp88.aggr_info;
if((((((struct Cyc_Absyn_AggrType_struct*)_tmp61)->f1).aggr_info).KnownAggr).tag
!= 1)goto _LL36;_tmp8A=(_tmp89.KnownAggr).f1;_tmp8B=_tmp88.targs;_LL35: {struct
Cyc_Absyn_AggrType_struct _tmpBFB;union Cyc_Absyn_AggrInfoU_union _tmpBFA;struct Cyc_Absyn_AggrInfo
_tmpBF9;struct Cyc_Absyn_AggrType_struct*_tmpBF8;return(void*)((_tmpBF8=_cycalloc(
sizeof(*_tmpBF8)),((_tmpBF8[0]=((_tmpBFB.tag=10,((_tmpBFB.f1=((_tmpBF9.aggr_info=(
union Cyc_Absyn_AggrInfoU_union)(((_tmpBFA.KnownAggr).tag=1,(((_tmpBFA.KnownAggr).f1=
_tmp8A,_tmpBFA)))),((_tmpBF9.targs=Cyc_Tcutil_copy_types(_tmp8B),_tmpBF9)))),
_tmpBFB)))),_tmpBF8))));}_LL36: if(*((int*)_tmp61)!= 11)goto _LL38;_tmp8C=(void*)((
struct Cyc_Absyn_AnonAggrType_struct*)_tmp61)->f1;_tmp8D=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp61)->f2;_LL37: {struct Cyc_Absyn_AnonAggrType_struct _tmpBFE;struct Cyc_Absyn_AnonAggrType_struct*
_tmpBFD;return(void*)((_tmpBFD=_cycalloc(sizeof(*_tmpBFD)),((_tmpBFD[0]=((
_tmpBFE.tag=11,((_tmpBFE.f1=(void*)_tmp8C,((_tmpBFE.f2=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Tcutil_copy_field,_tmp8D),_tmpBFE)))))),_tmpBFD))));}_LL38: if(*((
int*)_tmp61)!= 12)goto _LL3A;_tmp8E=((struct Cyc_Absyn_EnumType_struct*)_tmp61)->f1;
_tmp8F=((struct Cyc_Absyn_EnumType_struct*)_tmp61)->f2;_LL39: {struct Cyc_Absyn_EnumType_struct
_tmpC01;struct Cyc_Absyn_EnumType_struct*_tmpC00;return(void*)((_tmpC00=_cycalloc(
sizeof(*_tmpC00)),((_tmpC00[0]=((_tmpC01.tag=12,((_tmpC01.f1=_tmp8E,((_tmpC01.f2=
_tmp8F,_tmpC01)))))),_tmpC00))));}_LL3A: if(*((int*)_tmp61)!= 13)goto _LL3C;_tmp90=((
struct Cyc_Absyn_AnonEnumType_struct*)_tmp61)->f1;_LL3B: {struct Cyc_Absyn_AnonEnumType_struct
_tmpC04;struct Cyc_Absyn_AnonEnumType_struct*_tmpC03;return(void*)((_tmpC03=
_cycalloc(sizeof(*_tmpC03)),((_tmpC03[0]=((_tmpC04.tag=13,((_tmpC04.f1=((struct
Cyc_List_List*(*)(struct Cyc_Absyn_Enumfield*(*f)(struct Cyc_Absyn_Enumfield*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_enumfield,_tmp90),_tmpC04)))),
_tmpC03))));}_LL3C: if(*((int*)_tmp61)!= 18)goto _LL3E;_tmp91=(void*)((struct Cyc_Absyn_TagType_struct*)
_tmp61)->f1;_LL3D: {struct Cyc_Absyn_TagType_struct _tmpC07;struct Cyc_Absyn_TagType_struct*
_tmpC06;return(void*)((_tmpC06=_cycalloc(sizeof(*_tmpC06)),((_tmpC06[0]=((
_tmpC07.tag=18,((_tmpC07.f1=(void*)Cyc_Tcutil_copy_type(_tmp91),_tmpC07)))),
_tmpC06))));}_LL3E: if(*((int*)_tmp61)!= 17)goto _LL40;_tmp92=((struct Cyc_Absyn_ValueofType_struct*)
_tmp61)->f1;_LL3F: {struct Cyc_Absyn_ValueofType_struct _tmpC0A;struct Cyc_Absyn_ValueofType_struct*
_tmpC09;return(void*)((_tmpC09=_cycalloc(sizeof(*_tmpC09)),((_tmpC09[0]=((
_tmpC0A.tag=17,((_tmpC0A.f1=_tmp92,_tmpC0A)))),_tmpC09))));}_LL40: if(*((int*)
_tmp61)!= 14)goto _LL42;_tmp93=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp61)->f1;_LL41: {struct Cyc_Absyn_RgnHandleType_struct _tmpC0D;struct Cyc_Absyn_RgnHandleType_struct*
_tmpC0C;return(void*)((_tmpC0C=_cycalloc(sizeof(*_tmpC0C)),((_tmpC0C[0]=((
_tmpC0D.tag=14,((_tmpC0D.f1=(void*)Cyc_Tcutil_copy_type(_tmp93),_tmpC0D)))),
_tmpC0C))));}_LL42: if(*((int*)_tmp61)!= 15)goto _LL44;_tmp94=(void*)((struct Cyc_Absyn_DynRgnType_struct*)
_tmp61)->f1;_tmp95=(void*)((struct Cyc_Absyn_DynRgnType_struct*)_tmp61)->f2;_LL43: {
struct Cyc_Absyn_DynRgnType_struct _tmpC10;struct Cyc_Absyn_DynRgnType_struct*
_tmpC0F;return(void*)((_tmpC0F=_cycalloc(sizeof(*_tmpC0F)),((_tmpC0F[0]=((
_tmpC10.tag=15,((_tmpC10.f1=(void*)Cyc_Tcutil_copy_type(_tmp94),((_tmpC10.f2=(
void*)Cyc_Tcutil_copy_type(_tmp95),_tmpC10)))))),_tmpC0F))));}_LL44: if(*((int*)
_tmp61)!= 16)goto _LL46;_tmp96=((struct Cyc_Absyn_TypedefType_struct*)_tmp61)->f1;
_tmp97=((struct Cyc_Absyn_TypedefType_struct*)_tmp61)->f2;_tmp98=((struct Cyc_Absyn_TypedefType_struct*)
_tmp61)->f3;_LL45: {struct Cyc_Absyn_TypedefType_struct _tmpC13;struct Cyc_Absyn_TypedefType_struct*
_tmpC12;return(void*)((_tmpC12=_cycalloc(sizeof(*_tmpC12)),((_tmpC12[0]=((
_tmpC13.tag=16,((_tmpC13.f1=_tmp96,((_tmpC13.f2=Cyc_Tcutil_copy_types(_tmp97),((
_tmpC13.f3=_tmp98,((_tmpC13.f4=0,_tmpC13)))))))))),_tmpC12))));}_LL46: if((int)
_tmp61 != 3)goto _LL48;_LL47: goto _LL49;_LL48: if((int)_tmp61 != 2)goto _LL4A;_LL49:
return t;_LL4A: if(_tmp61 <= (void*)4)goto _LL4C;if(*((int*)_tmp61)!= 19)goto _LL4C;
_tmp99=(void*)((struct Cyc_Absyn_AccessEff_struct*)_tmp61)->f1;_LL4B: {struct Cyc_Absyn_AccessEff_struct
_tmpC16;struct Cyc_Absyn_AccessEff_struct*_tmpC15;return(void*)((_tmpC15=
_cycalloc(sizeof(*_tmpC15)),((_tmpC15[0]=((_tmpC16.tag=19,((_tmpC16.f1=(void*)
Cyc_Tcutil_copy_type(_tmp99),_tmpC16)))),_tmpC15))));}_LL4C: if(_tmp61 <= (void*)4)
goto _LL4E;if(*((int*)_tmp61)!= 20)goto _LL4E;_tmp9A=((struct Cyc_Absyn_JoinEff_struct*)
_tmp61)->f1;_LL4D: {struct Cyc_Absyn_JoinEff_struct _tmpC19;struct Cyc_Absyn_JoinEff_struct*
_tmpC18;return(void*)((_tmpC18=_cycalloc(sizeof(*_tmpC18)),((_tmpC18[0]=((
_tmpC19.tag=20,((_tmpC19.f1=Cyc_Tcutil_copy_types(_tmp9A),_tmpC19)))),_tmpC18))));}
_LL4E: if(_tmp61 <= (void*)4)goto _LL19;if(*((int*)_tmp61)!= 21)goto _LL19;_tmp9B=(
void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp61)->f1;_LL4F: {struct Cyc_Absyn_RgnsEff_struct
_tmpC1C;struct Cyc_Absyn_RgnsEff_struct*_tmpC1B;return(void*)((_tmpC1B=_cycalloc(
sizeof(*_tmpC1B)),((_tmpC1B[0]=((_tmpC1C.tag=21,((_tmpC1C.f1=(void*)Cyc_Tcutil_copy_type(
_tmp9B),_tmpC1C)))),_tmpC1B))));}_LL19:;}int Cyc_Tcutil_kind_leq(void*k1,void*k2);
int Cyc_Tcutil_kind_leq(void*k1,void*k2){if(k1 == k2)return 1;{struct _tuple0 _tmpC1D;
struct _tuple0 _tmpE0=(_tmpC1D.f1=k1,((_tmpC1D.f2=k2,_tmpC1D)));void*_tmpE1;void*
_tmpE2;void*_tmpE3;void*_tmpE4;void*_tmpE5;void*_tmpE6;void*_tmpE7;void*_tmpE8;
void*_tmpE9;void*_tmpEA;_LL51: _tmpE1=_tmpE0.f1;if((int)_tmpE1 != 2)goto _LL53;
_tmpE2=_tmpE0.f2;if((int)_tmpE2 != 1)goto _LL53;_LL52: goto _LL54;_LL53: _tmpE3=
_tmpE0.f1;if((int)_tmpE3 != 2)goto _LL55;_tmpE4=_tmpE0.f2;if((int)_tmpE4 != 0)goto
_LL55;_LL54: goto _LL56;_LL55: _tmpE5=_tmpE0.f1;if((int)_tmpE5 != 1)goto _LL57;_tmpE6=
_tmpE0.f2;if((int)_tmpE6 != 0)goto _LL57;_LL56: goto _LL58;_LL57: _tmpE7=_tmpE0.f1;
if((int)_tmpE7 != 3)goto _LL59;_tmpE8=_tmpE0.f2;if((int)_tmpE8 != 5)goto _LL59;_LL58:
goto _LL5A;_LL59: _tmpE9=_tmpE0.f1;if((int)_tmpE9 != 4)goto _LL5B;_tmpEA=_tmpE0.f2;
if((int)_tmpEA != 5)goto _LL5B;_LL5A: return 1;_LL5B:;_LL5C: return 0;_LL50:;}}static
int Cyc_Tcutil_is_region_kind(void*k);static int Cyc_Tcutil_is_region_kind(void*k){
void*_tmpEB=k;_LL5E: if((int)_tmpEB != 3)goto _LL60;_LL5F: goto _LL61;_LL60: if((int)
_tmpEB != 5)goto _LL62;_LL61: goto _LL63;_LL62: if((int)_tmpEB != 4)goto _LL64;_LL63:
return 1;_LL64:;_LL65: return 0;_LL5D:;}void*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*
tv);void*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*tv){void*_tmpEC=Cyc_Absyn_compress_kb((
void*)tv->kind);void*_tmpED;void*_tmpEE;_LL67: if(*((int*)_tmpEC)!= 0)goto _LL69;
_tmpED=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmpEC)->f1;_LL68: return _tmpED;
_LL69: if(*((int*)_tmpEC)!= 2)goto _LL6B;_tmpEE=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmpEC)->f2;_LL6A: return _tmpEE;_LL6B:;_LL6C: {const char*_tmpC20;void*_tmpC1F;(
_tmpC1F=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmpC20="kind not suitably constrained!",_tag_dyneither(_tmpC20,sizeof(char),31))),
_tag_dyneither(_tmpC1F,sizeof(void*),0)));}_LL66:;}void*Cyc_Tcutil_typ_kind(void*
t);void*Cyc_Tcutil_typ_kind(void*t){void*_tmpF1=Cyc_Tcutil_compress(t);struct Cyc_Core_Opt*
_tmpF2;struct Cyc_Absyn_Tvar*_tmpF3;void*_tmpF4;struct Cyc_Absyn_TunionInfo _tmpF5;
union Cyc_Absyn_TunionInfoU_union _tmpF6;struct Cyc_Absyn_UnknownTunionInfo _tmpF7;
struct Cyc_Absyn_TunionInfo _tmpF8;union Cyc_Absyn_TunionInfoU_union _tmpF9;struct
Cyc_Absyn_Tuniondecl**_tmpFA;struct Cyc_Absyn_Tuniondecl*_tmpFB;struct Cyc_Absyn_TunionFieldInfo
_tmpFC;union Cyc_Absyn_TunionFieldInfoU_union _tmpFD;struct Cyc_Absyn_Tuniondecl*
_tmpFE;struct Cyc_Absyn_Tunionfield*_tmpFF;struct Cyc_Absyn_TunionFieldInfo _tmp100;
union Cyc_Absyn_TunionFieldInfoU_union _tmp101;struct Cyc_Absyn_Enumdecl*_tmp102;
struct Cyc_Absyn_AggrInfo _tmp103;union Cyc_Absyn_AggrInfoU_union _tmp104;struct Cyc_Absyn_AggrInfo
_tmp105;union Cyc_Absyn_AggrInfoU_union _tmp106;struct Cyc_Absyn_Aggrdecl**_tmp107;
struct Cyc_Absyn_Aggrdecl*_tmp108;struct Cyc_Absyn_Aggrdecl _tmp109;struct Cyc_Absyn_AggrdeclImpl*
_tmp10A;struct Cyc_Absyn_Enumdecl*_tmp10B;struct Cyc_Absyn_PtrInfo _tmp10C;struct
Cyc_Absyn_Typedefdecl*_tmp10D;_LL6E: if(_tmpF1 <= (void*)4)goto _LL72;if(*((int*)
_tmpF1)!= 0)goto _LL70;_tmpF2=((struct Cyc_Absyn_Evar_struct*)_tmpF1)->f1;_LL6F:
return(void*)((struct Cyc_Core_Opt*)_check_null(_tmpF2))->v;_LL70: if(*((int*)
_tmpF1)!= 1)goto _LL72;_tmpF3=((struct Cyc_Absyn_VarType_struct*)_tmpF1)->f1;_LL71:
return Cyc_Tcutil_tvar_kind(_tmpF3);_LL72: if((int)_tmpF1 != 0)goto _LL74;_LL73:
return(void*)1;_LL74: if(_tmpF1 <= (void*)4)goto _LL76;if(*((int*)_tmpF1)!= 5)goto
_LL76;_tmpF4=(void*)((struct Cyc_Absyn_IntType_struct*)_tmpF1)->f2;_LL75: return(
_tmpF4 == (void*)((void*)2) || _tmpF4 == (void*)((void*)3))?(void*)2:(void*)1;
_LL76: if((int)_tmpF1 != 1)goto _LL78;_LL77: goto _LL79;_LL78: if(_tmpF1 <= (void*)4)
goto _LL80;if(*((int*)_tmpF1)!= 6)goto _LL7A;_LL79: goto _LL7B;_LL7A: if(*((int*)
_tmpF1)!= 8)goto _LL7C;_LL7B: return(void*)1;_LL7C: if(*((int*)_tmpF1)!= 15)goto
_LL7E;_LL7D: goto _LL7F;_LL7E: if(*((int*)_tmpF1)!= 14)goto _LL80;_LL7F: return(void*)
2;_LL80: if((int)_tmpF1 != 3)goto _LL82;_LL81: return(void*)4;_LL82: if((int)_tmpF1 != 
2)goto _LL84;_LL83: return(void*)3;_LL84: if(_tmpF1 <= (void*)4)goto _LL86;if(*((int*)
_tmpF1)!= 2)goto _LL86;_tmpF5=((struct Cyc_Absyn_TunionType_struct*)_tmpF1)->f1;
_tmpF6=_tmpF5.tunion_info;if((((((struct Cyc_Absyn_TunionType_struct*)_tmpF1)->f1).tunion_info).UnknownTunion).tag
!= 0)goto _LL86;_tmpF7=(_tmpF6.UnknownTunion).f1;_LL85: if(_tmpF7.is_flat)return(
void*)1;else{return(void*)2;}_LL86: if(_tmpF1 <= (void*)4)goto _LL88;if(*((int*)
_tmpF1)!= 2)goto _LL88;_tmpF8=((struct Cyc_Absyn_TunionType_struct*)_tmpF1)->f1;
_tmpF9=_tmpF8.tunion_info;if((((((struct Cyc_Absyn_TunionType_struct*)_tmpF1)->f1).tunion_info).KnownTunion).tag
!= 1)goto _LL88;_tmpFA=(_tmpF9.KnownTunion).f1;_tmpFB=*_tmpFA;_LL87: if(_tmpFB->is_flat)
return(void*)1;else{return(void*)2;}_LL88: if(_tmpF1 <= (void*)4)goto _LL8A;if(*((
int*)_tmpF1)!= 3)goto _LL8A;_tmpFC=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmpF1)->f1;_tmpFD=_tmpFC.field_info;if((((((struct Cyc_Absyn_TunionFieldType_struct*)
_tmpF1)->f1).field_info).KnownTunionfield).tag != 1)goto _LL8A;_tmpFE=(_tmpFD.KnownTunionfield).f1;
_tmpFF=(_tmpFD.KnownTunionfield).f2;_LL89: if(_tmpFE->is_flat)return(void*)1;
else{if(_tmpFF->typs == 0)return(void*)2;else{return(void*)1;}}_LL8A: if(_tmpF1 <= (
void*)4)goto _LL8C;if(*((int*)_tmpF1)!= 3)goto _LL8C;_tmp100=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmpF1)->f1;_tmp101=_tmp100.field_info;if((((((struct Cyc_Absyn_TunionFieldType_struct*)
_tmpF1)->f1).field_info).UnknownTunionfield).tag != 0)goto _LL8C;_LL8B: {const char*
_tmpC23;void*_tmpC22;(_tmpC22=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpC23="typ_kind: Unresolved TunionFieldType",
_tag_dyneither(_tmpC23,sizeof(char),37))),_tag_dyneither(_tmpC22,sizeof(void*),0)));}
_LL8C: if(_tmpF1 <= (void*)4)goto _LL8E;if(*((int*)_tmpF1)!= 12)goto _LL8E;_tmp102=((
struct Cyc_Absyn_EnumType_struct*)_tmpF1)->f2;if(_tmp102 != 0)goto _LL8E;_LL8D: goto
_LL8F;_LL8E: if(_tmpF1 <= (void*)4)goto _LL90;if(*((int*)_tmpF1)!= 10)goto _LL90;
_tmp103=((struct Cyc_Absyn_AggrType_struct*)_tmpF1)->f1;_tmp104=_tmp103.aggr_info;
if((((((struct Cyc_Absyn_AggrType_struct*)_tmpF1)->f1).aggr_info).UnknownAggr).tag
!= 0)goto _LL90;_LL8F: return(void*)0;_LL90: if(_tmpF1 <= (void*)4)goto _LL92;if(*((
int*)_tmpF1)!= 10)goto _LL92;_tmp105=((struct Cyc_Absyn_AggrType_struct*)_tmpF1)->f1;
_tmp106=_tmp105.aggr_info;if((((((struct Cyc_Absyn_AggrType_struct*)_tmpF1)->f1).aggr_info).KnownAggr).tag
!= 1)goto _LL92;_tmp107=(_tmp106.KnownAggr).f1;_tmp108=*_tmp107;_tmp109=*_tmp108;
_tmp10A=_tmp109.impl;_LL91: return _tmp10A == 0?(void*)0:(void*)1;_LL92: if(_tmpF1 <= (
void*)4)goto _LL94;if(*((int*)_tmpF1)!= 11)goto _LL94;_LL93: goto _LL95;_LL94: if(
_tmpF1 <= (void*)4)goto _LL96;if(*((int*)_tmpF1)!= 13)goto _LL96;_LL95: return(void*)
1;_LL96: if(_tmpF1 <= (void*)4)goto _LL98;if(*((int*)_tmpF1)!= 12)goto _LL98;_tmp10B=((
struct Cyc_Absyn_EnumType_struct*)_tmpF1)->f2;_LL97: if(_tmp10B->fields == 0)return(
void*)0;else{return(void*)1;}_LL98: if(_tmpF1 <= (void*)4)goto _LL9A;if(*((int*)
_tmpF1)!= 4)goto _LL9A;_tmp10C=((struct Cyc_Absyn_PointerType_struct*)_tmpF1)->f1;
_LL99: {union Cyc_Absyn_Constraint_union _tmp110=(Cyc_Absyn_compress_conref((
_tmp10C.ptr_atts).bounds))->v;void*_tmp111;void*_tmp112;_LLAB: if((_tmp110.No_constr).tag
!= 2)goto _LLAD;_LLAC: goto _LLAE;_LLAD: if((_tmp110.Eq_constr).tag != 0)goto _LLAF;
_tmp111=(_tmp110.Eq_constr).f1;if((int)_tmp111 != 0)goto _LLAF;_LLAE: return(void*)
1;_LLAF: if((_tmp110.Eq_constr).tag != 0)goto _LLB1;_tmp112=(_tmp110.Eq_constr).f1;
if(_tmp112 <= (void*)1)goto _LLB1;if(*((int*)_tmp112)!= 0)goto _LLB1;_LLB0: return(
void*)2;_LLB1: if((_tmp110.Forward_constr).tag != 1)goto _LLAA;_LLB2: {const char*
_tmpC26;void*_tmpC25;(_tmpC25=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpC26="typ_kind: forward constr in ptr bounds",
_tag_dyneither(_tmpC26,sizeof(char),39))),_tag_dyneither(_tmpC25,sizeof(void*),0)));}
_LLAA:;}_LL9A: if(_tmpF1 <= (void*)4)goto _LL9C;if(*((int*)_tmpF1)!= 17)goto _LL9C;
_LL9B: return(void*)7;_LL9C: if(_tmpF1 <= (void*)4)goto _LL9E;if(*((int*)_tmpF1)!= 
18)goto _LL9E;_LL9D: return(void*)2;_LL9E: if(_tmpF1 <= (void*)4)goto _LLA0;if(*((int*)
_tmpF1)!= 7)goto _LLA0;_LL9F: goto _LLA1;_LLA0: if(_tmpF1 <= (void*)4)goto _LLA2;if(*((
int*)_tmpF1)!= 9)goto _LLA2;_LLA1: return(void*)1;_LLA2: if(_tmpF1 <= (void*)4)goto
_LLA4;if(*((int*)_tmpF1)!= 16)goto _LLA4;_tmp10D=((struct Cyc_Absyn_TypedefType_struct*)
_tmpF1)->f3;_LLA3: if(_tmp10D == 0  || _tmp10D->kind == 0){const char*_tmpC2A;void*
_tmpC29[1];struct Cyc_String_pa_struct _tmpC28;(_tmpC28.tag=0,((_tmpC28.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpC29[0]=&
_tmpC28,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmpC2A="typ_kind: typedef found: %s",_tag_dyneither(_tmpC2A,sizeof(char),28))),
_tag_dyneither(_tmpC29,sizeof(void*),1)))))));}return(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp10D->kind))->v;_LLA4: if(_tmpF1 <= (void*)4)goto _LLA6;if(*((int*)
_tmpF1)!= 19)goto _LLA6;_LLA5: goto _LLA7;_LLA6: if(_tmpF1 <= (void*)4)goto _LLA8;if(*((
int*)_tmpF1)!= 20)goto _LLA8;_LLA7: goto _LLA9;_LLA8: if(_tmpF1 <= (void*)4)goto _LL6D;
if(*((int*)_tmpF1)!= 21)goto _LL6D;_LLA9: return(void*)6;_LL6D:;}int Cyc_Tcutil_unify(
void*t1,void*t2);int Cyc_Tcutil_unify(void*t1,void*t2){struct _handler_cons _tmp118;
_push_handler(& _tmp118);{int _tmp11A=0;if(setjmp(_tmp118.handler))_tmp11A=1;if(!
_tmp11A){Cyc_Tcutil_unify_it(t1,t2);{int _tmp11B=1;_npop_handler(0);return _tmp11B;};
_pop_handler();}else{void*_tmp119=(void*)_exn_thrown;void*_tmp11D=_tmp119;_LLB4:
if(_tmp11D != Cyc_Tcutil_Unify)goto _LLB6;_LLB5: return 0;_LLB6:;_LLB7:(void)_throw(
_tmp11D);_LLB3:;}}}static void Cyc_Tcutil_occurslist(void*evar,struct _RegionHandle*
r,struct Cyc_List_List*env,struct Cyc_List_List*ts);static void Cyc_Tcutil_occurs(
void*evar,struct _RegionHandle*r,struct Cyc_List_List*env,void*t);static void Cyc_Tcutil_occurs(
void*evar,struct _RegionHandle*r,struct Cyc_List_List*env,void*t){t=Cyc_Tcutil_compress(
t);{void*_tmp11E=t;struct Cyc_Absyn_Tvar*_tmp11F;struct Cyc_Core_Opt*_tmp120;
struct Cyc_Core_Opt*_tmp121;struct Cyc_Core_Opt**_tmp122;struct Cyc_Absyn_PtrInfo
_tmp123;struct Cyc_Absyn_ArrayInfo _tmp124;void*_tmp125;struct Cyc_Absyn_FnInfo
_tmp126;struct Cyc_List_List*_tmp127;struct Cyc_Core_Opt*_tmp128;void*_tmp129;
struct Cyc_List_List*_tmp12A;int _tmp12B;struct Cyc_Absyn_VarargInfo*_tmp12C;struct
Cyc_List_List*_tmp12D;struct Cyc_List_List*_tmp12E;struct Cyc_List_List*_tmp12F;
struct Cyc_Absyn_TunionInfo _tmp130;struct Cyc_List_List*_tmp131;struct Cyc_Core_Opt*
_tmp132;struct Cyc_List_List*_tmp133;struct Cyc_Absyn_TunionFieldInfo _tmp134;
struct Cyc_List_List*_tmp135;struct Cyc_Absyn_AggrInfo _tmp136;struct Cyc_List_List*
_tmp137;struct Cyc_List_List*_tmp138;void*_tmp139;void*_tmp13A;void*_tmp13B;void*
_tmp13C;struct Cyc_List_List*_tmp13D;_LLB9: if(_tmp11E <= (void*)4)goto _LLD9;if(*((
int*)_tmp11E)!= 1)goto _LLBB;_tmp11F=((struct Cyc_Absyn_VarType_struct*)_tmp11E)->f1;
_LLBA: if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,
env,_tmp11F)){{const char*_tmpC2B;Cyc_Tcutil_failure_reason=((_tmpC2B="(type variable would escape scope)",
_tag_dyneither(_tmpC2B,sizeof(char),35)));}(int)_throw((void*)Cyc_Tcutil_Unify);}
goto _LLB8;_LLBB: if(*((int*)_tmp11E)!= 0)goto _LLBD;_tmp120=((struct Cyc_Absyn_Evar_struct*)
_tmp11E)->f2;_tmp121=((struct Cyc_Absyn_Evar_struct*)_tmp11E)->f4;_tmp122=(struct
Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)_tmp11E)->f4;_LLBC: if(t == evar){{
const char*_tmpC2C;Cyc_Tcutil_failure_reason=((_tmpC2C="(occurs check)",
_tag_dyneither(_tmpC2C,sizeof(char),15)));}(int)_throw((void*)Cyc_Tcutil_Unify);}
else{if(_tmp120 != 0)Cyc_Tcutil_occurs(evar,r,env,(void*)_tmp120->v);else{int
problem=0;{struct Cyc_List_List*s=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(*_tmp122))->v;for(0;s != 0;s=s->tl){if(!((int(*)(int(*compare)(struct
Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*
x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,(struct Cyc_Absyn_Tvar*)s->hd)){
problem=1;break;}}}if(problem){struct Cyc_List_List*_tmp140=0;{struct Cyc_List_List*
s=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*_tmp122))->v;for(0;s
!= 0;s=s->tl){if(((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,
env,(struct Cyc_Absyn_Tvar*)s->hd)){struct Cyc_List_List*_tmpC2D;_tmp140=((_tmpC2D=
_cycalloc(sizeof(*_tmpC2D)),((_tmpC2D->hd=(struct Cyc_Absyn_Tvar*)s->hd,((_tmpC2D->tl=
_tmp140,_tmpC2D))))));}}}{struct Cyc_Core_Opt*_tmpC2E;*_tmp122=((_tmpC2E=
_cycalloc(sizeof(*_tmpC2E)),((_tmpC2E->v=_tmp140,_tmpC2E))));}}}}goto _LLB8;_LLBD:
if(*((int*)_tmp11E)!= 4)goto _LLBF;_tmp123=((struct Cyc_Absyn_PointerType_struct*)
_tmp11E)->f1;_LLBE: Cyc_Tcutil_occurs(evar,r,env,(void*)_tmp123.elt_typ);Cyc_Tcutil_occurs(
evar,r,env,(void*)(_tmp123.ptr_atts).rgn);goto _LLB8;_LLBF: if(*((int*)_tmp11E)!= 
7)goto _LLC1;_tmp124=((struct Cyc_Absyn_ArrayType_struct*)_tmp11E)->f1;_tmp125=(
void*)_tmp124.elt_type;_LLC0: Cyc_Tcutil_occurs(evar,r,env,_tmp125);goto _LLB8;
_LLC1: if(*((int*)_tmp11E)!= 8)goto _LLC3;_tmp126=((struct Cyc_Absyn_FnType_struct*)
_tmp11E)->f1;_tmp127=_tmp126.tvars;_tmp128=_tmp126.effect;_tmp129=(void*)_tmp126.ret_typ;
_tmp12A=_tmp126.args;_tmp12B=_tmp126.c_varargs;_tmp12C=_tmp126.cyc_varargs;
_tmp12D=_tmp126.rgn_po;_tmp12E=_tmp126.attributes;_LLC2: env=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(
r,_tmp127,env);if(_tmp128 != 0)Cyc_Tcutil_occurs(evar,r,env,(void*)_tmp128->v);
Cyc_Tcutil_occurs(evar,r,env,_tmp129);for(0;_tmp12A != 0;_tmp12A=_tmp12A->tl){Cyc_Tcutil_occurs(
evar,r,env,(*((struct _tuple3*)_tmp12A->hd)).f3);}if(_tmp12C != 0)Cyc_Tcutil_occurs(
evar,r,env,(void*)_tmp12C->type);for(0;_tmp12D != 0;_tmp12D=_tmp12D->tl){struct
_tuple0 _tmp144;void*_tmp145;void*_tmp146;struct _tuple0*_tmp143=(struct _tuple0*)
_tmp12D->hd;_tmp144=*_tmp143;_tmp145=_tmp144.f1;_tmp146=_tmp144.f2;Cyc_Tcutil_occurs(
evar,r,env,_tmp145);Cyc_Tcutil_occurs(evar,r,env,_tmp146);}goto _LLB8;_LLC3: if(*((
int*)_tmp11E)!= 9)goto _LLC5;_tmp12F=((struct Cyc_Absyn_TupleType_struct*)_tmp11E)->f1;
_LLC4: for(0;_tmp12F != 0;_tmp12F=_tmp12F->tl){Cyc_Tcutil_occurs(evar,r,env,(*((
struct _tuple5*)_tmp12F->hd)).f2);}goto _LLB8;_LLC5: if(*((int*)_tmp11E)!= 2)goto
_LLC7;_tmp130=((struct Cyc_Absyn_TunionType_struct*)_tmp11E)->f1;_tmp131=_tmp130.targs;
_tmp132=_tmp130.rgn;_LLC6: if((unsigned int)_tmp132)Cyc_Tcutil_occurs(evar,r,env,(
void*)_tmp132->v);Cyc_Tcutil_occurslist(evar,r,env,_tmp131);goto _LLB8;_LLC7: if(*((
int*)_tmp11E)!= 16)goto _LLC9;_tmp133=((struct Cyc_Absyn_TypedefType_struct*)
_tmp11E)->f2;_LLC8: _tmp135=_tmp133;goto _LLCA;_LLC9: if(*((int*)_tmp11E)!= 3)goto
_LLCB;_tmp134=((struct Cyc_Absyn_TunionFieldType_struct*)_tmp11E)->f1;_tmp135=
_tmp134.targs;_LLCA: _tmp137=_tmp135;goto _LLCC;_LLCB: if(*((int*)_tmp11E)!= 10)
goto _LLCD;_tmp136=((struct Cyc_Absyn_AggrType_struct*)_tmp11E)->f1;_tmp137=
_tmp136.targs;_LLCC: Cyc_Tcutil_occurslist(evar,r,env,_tmp137);goto _LLB8;_LLCD:
if(*((int*)_tmp11E)!= 11)goto _LLCF;_tmp138=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp11E)->f2;_LLCE: for(0;_tmp138 != 0;_tmp138=_tmp138->tl){Cyc_Tcutil_occurs(evar,
r,env,(void*)((struct Cyc_Absyn_Aggrfield*)_tmp138->hd)->type);}goto _LLB8;_LLCF:
if(*((int*)_tmp11E)!= 18)goto _LLD1;_tmp139=(void*)((struct Cyc_Absyn_TagType_struct*)
_tmp11E)->f1;_LLD0: _tmp13A=_tmp139;goto _LLD2;_LLD1: if(*((int*)_tmp11E)!= 19)goto
_LLD3;_tmp13A=(void*)((struct Cyc_Absyn_AccessEff_struct*)_tmp11E)->f1;_LLD2:
_tmp13B=_tmp13A;goto _LLD4;_LLD3: if(*((int*)_tmp11E)!= 14)goto _LLD5;_tmp13B=(void*)((
struct Cyc_Absyn_RgnHandleType_struct*)_tmp11E)->f1;_LLD4: _tmp13C=_tmp13B;goto
_LLD6;_LLD5: if(*((int*)_tmp11E)!= 21)goto _LLD7;_tmp13C=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp11E)->f1;_LLD6: Cyc_Tcutil_occurs(evar,r,env,_tmp13C);goto _LLB8;_LLD7: if(*((
int*)_tmp11E)!= 20)goto _LLD9;_tmp13D=((struct Cyc_Absyn_JoinEff_struct*)_tmp11E)->f1;
_LLD8: Cyc_Tcutil_occurslist(evar,r,env,_tmp13D);goto _LLB8;_LLD9:;_LLDA: goto _LLB8;
_LLB8:;}}static void Cyc_Tcutil_occurslist(void*evar,struct _RegionHandle*r,struct
Cyc_List_List*env,struct Cyc_List_List*ts);static void Cyc_Tcutil_occurslist(void*
evar,struct _RegionHandle*r,struct Cyc_List_List*env,struct Cyc_List_List*ts){for(0;
ts != 0;ts=ts->tl){Cyc_Tcutil_occurs(evar,r,env,(void*)ts->hd);}}static void Cyc_Tcutil_unify_list(
struct Cyc_List_List*t1,struct Cyc_List_List*t2);static void Cyc_Tcutil_unify_list(
struct Cyc_List_List*t1,struct Cyc_List_List*t2){for(0;t1 != 0  && t2 != 0;(t1=t1->tl,
t2=t2->tl)){Cyc_Tcutil_unify_it((void*)t1->hd,(void*)t2->hd);}if(t1 != 0  || t2 != 
0)(int)_throw((void*)Cyc_Tcutil_Unify);}static void Cyc_Tcutil_unify_tqual(struct
Cyc_Absyn_Tqual tq1,void*t1,struct Cyc_Absyn_Tqual tq2,void*t2);static void Cyc_Tcutil_unify_tqual(
struct Cyc_Absyn_Tqual tq1,void*t1,struct Cyc_Absyn_Tqual tq2,void*t2){if(tq1.print_const
 && !tq1.real_const){const char*_tmpC31;void*_tmpC30;(_tmpC30=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpC31="tq1 real_const not set.",
_tag_dyneither(_tmpC31,sizeof(char),24))),_tag_dyneither(_tmpC30,sizeof(void*),0)));}
if(tq2.print_const  && !tq2.real_const){const char*_tmpC34;void*_tmpC33;(_tmpC33=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmpC34="tq2 real_const not set.",_tag_dyneither(_tmpC34,sizeof(char),24))),
_tag_dyneither(_tmpC33,sizeof(void*),0)));}if((tq1.real_const != tq2.real_const
 || tq1.q_volatile != tq2.q_volatile) || tq1.q_restrict != tq2.q_restrict){Cyc_Tcutil_t1_failure=
t1;Cyc_Tcutil_t2_failure=t2;Cyc_Tcutil_tq1_const=tq1.real_const;Cyc_Tcutil_tq2_const=
tq2.real_const;{const char*_tmpC35;Cyc_Tcutil_failure_reason=((_tmpC35="(qualifiers don't match)",
_tag_dyneither(_tmpC35,sizeof(char),25)));}(int)_throw((void*)Cyc_Tcutil_Unify);}
Cyc_Tcutil_tq1_const=0;Cyc_Tcutil_tq2_const=0;}int Cyc_Tcutil_equal_tqual(struct
Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2);int Cyc_Tcutil_equal_tqual(struct Cyc_Absyn_Tqual
tq1,struct Cyc_Absyn_Tqual tq2){return(tq1.real_const == tq2.real_const  && tq1.q_volatile
== tq2.q_volatile) && tq1.q_restrict == tq2.q_restrict;}static void Cyc_Tcutil_unify_it_conrefs(
int(*cmp)(void*,void*),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y,struct
_dyneither_ptr reason);static void Cyc_Tcutil_unify_it_conrefs(int(*cmp)(void*,void*),
struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y,struct _dyneither_ptr reason){x=
Cyc_Absyn_compress_conref(x);y=Cyc_Absyn_compress_conref(y);if(x == y)return;{
union Cyc_Absyn_Constraint_union _tmp14C=x->v;void*_tmp14D;_LLDC: if((_tmp14C.No_constr).tag
!= 2)goto _LLDE;_LLDD:{union Cyc_Absyn_Constraint_union _tmpC36;x->v=(union Cyc_Absyn_Constraint_union)(((
_tmpC36.Forward_constr).tag=1,(((_tmpC36.Forward_constr).f1=y,_tmpC36))));}
return;_LLDE: if((_tmp14C.Eq_constr).tag != 0)goto _LLE0;_tmp14D=(_tmp14C.Eq_constr).f1;
_LLDF: {union Cyc_Absyn_Constraint_union _tmp14F=y->v;void*_tmp150;_LLE3: if((
_tmp14F.No_constr).tag != 2)goto _LLE5;_LLE4: y->v=x->v;return;_LLE5: if((_tmp14F.Eq_constr).tag
!= 0)goto _LLE7;_tmp150=(_tmp14F.Eq_constr).f1;_LLE6: if(cmp(_tmp14D,_tmp150)!= 0){
Cyc_Tcutil_failure_reason=reason;(int)_throw((void*)Cyc_Tcutil_Unify);}return;
_LLE7: if((_tmp14F.Forward_constr).tag != 1)goto _LLE2;_LLE8: {const char*_tmpC39;
void*_tmpC38;(_tmpC38=0,Cyc_Tcutil_impos(((_tmpC39="unify_conref: forward after compress(2)",
_tag_dyneither(_tmpC39,sizeof(char),40))),_tag_dyneither(_tmpC38,sizeof(void*),0)));}
_LLE2:;}_LLE0: if((_tmp14C.Forward_constr).tag != 1)goto _LLDB;_LLE1: {const char*
_tmpC3C;void*_tmpC3B;(_tmpC3B=0,Cyc_Tcutil_impos(((_tmpC3C="unify_conref: forward after compress",
_tag_dyneither(_tmpC3C,sizeof(char),37))),_tag_dyneither(_tmpC3B,sizeof(void*),0)));}
_LLDB:;}}static int Cyc_Tcutil_unify_conrefs(int(*cmp)(void*,void*),struct Cyc_Absyn_Conref*
x,struct Cyc_Absyn_Conref*y);static int Cyc_Tcutil_unify_conrefs(int(*cmp)(void*,
void*),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y){struct _handler_cons
_tmp155;_push_handler(& _tmp155);{int _tmp157=0;if(setjmp(_tmp155.handler))_tmp157=
1;if(!_tmp157){Cyc_Tcutil_unify_it_conrefs(cmp,x,y,(struct _dyneither_ptr)
_tag_dyneither(0,0,0));{int _tmp158=1;_npop_handler(0);return _tmp158;};
_pop_handler();}else{void*_tmp156=(void*)_exn_thrown;void*_tmp15A=_tmp156;_LLEA:
if(_tmp15A != Cyc_Tcutil_Unify)goto _LLEC;_LLEB: return 0;_LLEC:;_LLED:(void)_throw(
_tmp15A);_LLE9:;}}}static int Cyc_Tcutil_boundscmp(void*b1,void*b2);static int Cyc_Tcutil_boundscmp(
void*b1,void*b2){struct _tuple0 _tmpC3D;struct _tuple0 _tmp15C=(_tmpC3D.f1=b1,((
_tmpC3D.f2=b2,_tmpC3D)));void*_tmp15D;void*_tmp15E;void*_tmp15F;void*_tmp160;
void*_tmp161;struct Cyc_Absyn_Exp*_tmp162;void*_tmp163;struct Cyc_Absyn_Exp*
_tmp164;_LLEF: _tmp15D=_tmp15C.f1;if((int)_tmp15D != 0)goto _LLF1;_tmp15E=_tmp15C.f2;
if((int)_tmp15E != 0)goto _LLF1;_LLF0: return 0;_LLF1: _tmp15F=_tmp15C.f1;if((int)
_tmp15F != 0)goto _LLF3;_LLF2: return - 1;_LLF3: _tmp160=_tmp15C.f2;if((int)_tmp160 != 
0)goto _LLF5;_LLF4: return 1;_LLF5: _tmp161=_tmp15C.f1;if(_tmp161 <= (void*)1)goto
_LLEE;if(*((int*)_tmp161)!= 0)goto _LLEE;_tmp162=((struct Cyc_Absyn_Upper_b_struct*)
_tmp161)->f1;_tmp163=_tmp15C.f2;if(_tmp163 <= (void*)1)goto _LLEE;if(*((int*)
_tmp163)!= 0)goto _LLEE;_tmp164=((struct Cyc_Absyn_Upper_b_struct*)_tmp163)->f1;
_LLF6: return Cyc_Evexp_const_exp_cmp(_tmp162,_tmp164);_LLEE:;}static int Cyc_Tcutil_unify_it_bounds(
void*b1,void*b2);static int Cyc_Tcutil_unify_it_bounds(void*b1,void*b2){struct
_tuple0 _tmpC3E;struct _tuple0 _tmp166=(_tmpC3E.f1=b1,((_tmpC3E.f2=b2,_tmpC3E)));
void*_tmp167;void*_tmp168;void*_tmp169;void*_tmp16A;void*_tmp16B;struct Cyc_Absyn_Exp*
_tmp16C;void*_tmp16D;struct Cyc_Absyn_Exp*_tmp16E;_LLF8: _tmp167=_tmp166.f1;if((
int)_tmp167 != 0)goto _LLFA;_tmp168=_tmp166.f2;if((int)_tmp168 != 0)goto _LLFA;_LLF9:
return 0;_LLFA: _tmp169=_tmp166.f1;if((int)_tmp169 != 0)goto _LLFC;_LLFB: return - 1;
_LLFC: _tmp16A=_tmp166.f2;if((int)_tmp16A != 0)goto _LLFE;_LLFD: return 1;_LLFE:
_tmp16B=_tmp166.f1;if(_tmp16B <= (void*)1)goto _LLF7;if(*((int*)_tmp16B)!= 0)goto
_LLF7;_tmp16C=((struct Cyc_Absyn_Upper_b_struct*)_tmp16B)->f1;_tmp16D=_tmp166.f2;
if(_tmp16D <= (void*)1)goto _LLF7;if(*((int*)_tmp16D)!= 0)goto _LLF7;_tmp16E=((
struct Cyc_Absyn_Upper_b_struct*)_tmp16D)->f1;_LLFF: return Cyc_Evexp_const_exp_cmp(
_tmp16C,_tmp16E);_LLF7:;}static int Cyc_Tcutil_attribute_case_number(void*att);
static int Cyc_Tcutil_attribute_case_number(void*att){void*_tmp16F=att;_LL101: if(
_tmp16F <= (void*)17)goto _LL103;if(*((int*)_tmp16F)!= 0)goto _LL103;_LL102: return 0;
_LL103: if((int)_tmp16F != 0)goto _LL105;_LL104: return 1;_LL105: if((int)_tmp16F != 1)
goto _LL107;_LL106: return 2;_LL107: if((int)_tmp16F != 2)goto _LL109;_LL108: return 3;
_LL109: if((int)_tmp16F != 3)goto _LL10B;_LL10A: return 4;_LL10B: if((int)_tmp16F != 4)
goto _LL10D;_LL10C: return 5;_LL10D: if(_tmp16F <= (void*)17)goto _LL10F;if(*((int*)
_tmp16F)!= 1)goto _LL10F;_LL10E: return 6;_LL10F: if((int)_tmp16F != 5)goto _LL111;
_LL110: return 7;_LL111: if(_tmp16F <= (void*)17)goto _LL113;if(*((int*)_tmp16F)!= 2)
goto _LL113;_LL112: return 8;_LL113: if((int)_tmp16F != 6)goto _LL115;_LL114: return 9;
_LL115: if((int)_tmp16F != 7)goto _LL117;_LL116: return 10;_LL117: if((int)_tmp16F != 8)
goto _LL119;_LL118: return 11;_LL119: if((int)_tmp16F != 9)goto _LL11B;_LL11A: return 12;
_LL11B: if((int)_tmp16F != 10)goto _LL11D;_LL11C: return 13;_LL11D: if((int)_tmp16F != 
11)goto _LL11F;_LL11E: return 14;_LL11F: if((int)_tmp16F != 12)goto _LL121;_LL120:
return 15;_LL121: if((int)_tmp16F != 13)goto _LL123;_LL122: return 16;_LL123: if((int)
_tmp16F != 14)goto _LL125;_LL124: return 17;_LL125: if((int)_tmp16F != 15)goto _LL127;
_LL126: return 18;_LL127: if(_tmp16F <= (void*)17)goto _LL12B;if(*((int*)_tmp16F)!= 3)
goto _LL129;_LL128: return 19;_LL129: if(*((int*)_tmp16F)!= 4)goto _LL12B;_LL12A:
return 20;_LL12B:;_LL12C: return 21;_LL100:;}static int Cyc_Tcutil_attribute_cmp(void*
att1,void*att2);static int Cyc_Tcutil_attribute_cmp(void*att1,void*att2){struct
_tuple0 _tmpC3F;struct _tuple0 _tmp171=(_tmpC3F.f1=att1,((_tmpC3F.f2=att2,_tmpC3F)));
void*_tmp172;int _tmp173;void*_tmp174;int _tmp175;void*_tmp176;int _tmp177;void*
_tmp178;int _tmp179;void*_tmp17A;int _tmp17B;void*_tmp17C;int _tmp17D;void*_tmp17E;
struct _dyneither_ptr _tmp17F;void*_tmp180;struct _dyneither_ptr _tmp181;void*
_tmp182;void*_tmp183;int _tmp184;int _tmp185;void*_tmp186;void*_tmp187;int _tmp188;
int _tmp189;_LL12E: _tmp172=_tmp171.f1;if(_tmp172 <= (void*)17)goto _LL130;if(*((int*)
_tmp172)!= 0)goto _LL130;_tmp173=((struct Cyc_Absyn_Regparm_att_struct*)_tmp172)->f1;
_tmp174=_tmp171.f2;if(_tmp174 <= (void*)17)goto _LL130;if(*((int*)_tmp174)!= 0)
goto _LL130;_tmp175=((struct Cyc_Absyn_Regparm_att_struct*)_tmp174)->f1;_LL12F:
_tmp177=_tmp173;_tmp179=_tmp175;goto _LL131;_LL130: _tmp176=_tmp171.f1;if(_tmp176
<= (void*)17)goto _LL132;if(*((int*)_tmp176)!= 4)goto _LL132;_tmp177=((struct Cyc_Absyn_Initializes_att_struct*)
_tmp176)->f1;_tmp178=_tmp171.f2;if(_tmp178 <= (void*)17)goto _LL132;if(*((int*)
_tmp178)!= 4)goto _LL132;_tmp179=((struct Cyc_Absyn_Initializes_att_struct*)
_tmp178)->f1;_LL131: _tmp17B=_tmp177;_tmp17D=_tmp179;goto _LL133;_LL132: _tmp17A=
_tmp171.f1;if(_tmp17A <= (void*)17)goto _LL134;if(*((int*)_tmp17A)!= 1)goto _LL134;
_tmp17B=((struct Cyc_Absyn_Aligned_att_struct*)_tmp17A)->f1;_tmp17C=_tmp171.f2;
if(_tmp17C <= (void*)17)goto _LL134;if(*((int*)_tmp17C)!= 1)goto _LL134;_tmp17D=((
struct Cyc_Absyn_Aligned_att_struct*)_tmp17C)->f1;_LL133: return Cyc_Core_intcmp(
_tmp17B,_tmp17D);_LL134: _tmp17E=_tmp171.f1;if(_tmp17E <= (void*)17)goto _LL136;if(*((
int*)_tmp17E)!= 2)goto _LL136;_tmp17F=((struct Cyc_Absyn_Section_att_struct*)
_tmp17E)->f1;_tmp180=_tmp171.f2;if(_tmp180 <= (void*)17)goto _LL136;if(*((int*)
_tmp180)!= 2)goto _LL136;_tmp181=((struct Cyc_Absyn_Section_att_struct*)_tmp180)->f1;
_LL135: return Cyc_strcmp((struct _dyneither_ptr)_tmp17F,(struct _dyneither_ptr)
_tmp181);_LL136: _tmp182=_tmp171.f1;if(_tmp182 <= (void*)17)goto _LL138;if(*((int*)
_tmp182)!= 3)goto _LL138;_tmp183=(void*)((struct Cyc_Absyn_Format_att_struct*)
_tmp182)->f1;_tmp184=((struct Cyc_Absyn_Format_att_struct*)_tmp182)->f2;_tmp185=((
struct Cyc_Absyn_Format_att_struct*)_tmp182)->f3;_tmp186=_tmp171.f2;if(_tmp186 <= (
void*)17)goto _LL138;if(*((int*)_tmp186)!= 3)goto _LL138;_tmp187=(void*)((struct
Cyc_Absyn_Format_att_struct*)_tmp186)->f1;_tmp188=((struct Cyc_Absyn_Format_att_struct*)
_tmp186)->f2;_tmp189=((struct Cyc_Absyn_Format_att_struct*)_tmp186)->f3;_LL137: {
int _tmp18A=Cyc_Core_intcmp((int)((unsigned int)_tmp183),(int)((unsigned int)
_tmp187));if(_tmp18A != 0)return _tmp18A;{int _tmp18B=Cyc_Core_intcmp(_tmp184,
_tmp188);if(_tmp18B != 0)return _tmp18B;return Cyc_Core_intcmp(_tmp185,_tmp189);}}
_LL138:;_LL139: return Cyc_Core_intcmp(Cyc_Tcutil_attribute_case_number(att1),Cyc_Tcutil_attribute_case_number(
att2));_LL12D:;}static int Cyc_Tcutil_equal_att(void*a1,void*a2);static int Cyc_Tcutil_equal_att(
void*a1,void*a2){return Cyc_Tcutil_attribute_cmp(a1,a2)== 0;}int Cyc_Tcutil_same_atts(
struct Cyc_List_List*a1,struct Cyc_List_List*a2);int Cyc_Tcutil_same_atts(struct Cyc_List_List*
a1,struct Cyc_List_List*a2){{struct Cyc_List_List*a=a1;for(0;a != 0;a=a->tl){if(!
Cyc_List_exists_c(Cyc_Tcutil_equal_att,(void*)a->hd,a2))return 0;}}{struct Cyc_List_List*
a=a2;for(0;a != 0;a=a->tl){if(!Cyc_List_exists_c(Cyc_Tcutil_equal_att,(void*)a->hd,
a1))return 0;}}return 1;}static void*Cyc_Tcutil_rgns_of(void*t);static void*Cyc_Tcutil_rgns_of_field(
struct Cyc_Absyn_Aggrfield*af);static void*Cyc_Tcutil_rgns_of_field(struct Cyc_Absyn_Aggrfield*
af){return Cyc_Tcutil_rgns_of((void*)af->type);}static struct _tuple8*Cyc_Tcutil_region_free_subst(
struct Cyc_Absyn_Tvar*tv);static struct _tuple8*Cyc_Tcutil_region_free_subst(struct
Cyc_Absyn_Tvar*tv){void*t;{void*_tmp18C=Cyc_Tcutil_tvar_kind(tv);_LL13B: if((int)
_tmp18C != 4)goto _LL13D;_LL13C: t=(void*)3;goto _LL13A;_LL13D: if((int)_tmp18C != 3)
goto _LL13F;_LL13E: t=(void*)2;goto _LL13A;_LL13F: if((int)_tmp18C != 6)goto _LL141;
_LL140: t=Cyc_Absyn_empty_effect;goto _LL13A;_LL141: if((int)_tmp18C != 7)goto _LL143;
_LL142:{struct Cyc_Absyn_ValueofType_struct _tmpC42;struct Cyc_Absyn_ValueofType_struct*
_tmpC41;t=(void*)((_tmpC41=_cycalloc(sizeof(*_tmpC41)),((_tmpC41[0]=((_tmpC42.tag=
17,((_tmpC42.f1=Cyc_Absyn_uint_exp(0,0),_tmpC42)))),_tmpC41))));}goto _LL13A;
_LL143:;_LL144: t=Cyc_Absyn_sint_typ;goto _LL13A;_LL13A:;}{struct _tuple8*_tmpC43;
return(_tmpC43=_cycalloc(sizeof(*_tmpC43)),((_tmpC43->f1=tv,((_tmpC43->f2=t,
_tmpC43)))));}}static void*Cyc_Tcutil_rgns_of(void*t);static void*Cyc_Tcutil_rgns_of(
void*t){void*_tmp190=Cyc_Tcutil_compress(t);void*_tmp191;void*_tmp192;void*
_tmp193;struct Cyc_Absyn_TunionInfo _tmp194;struct Cyc_List_List*_tmp195;struct Cyc_Core_Opt*
_tmp196;struct Cyc_Absyn_PtrInfo _tmp197;void*_tmp198;struct Cyc_Absyn_PtrAtts
_tmp199;void*_tmp19A;struct Cyc_Absyn_ArrayInfo _tmp19B;void*_tmp19C;struct Cyc_List_List*
_tmp19D;struct Cyc_Absyn_TunionFieldInfo _tmp19E;struct Cyc_List_List*_tmp19F;
struct Cyc_Absyn_AggrInfo _tmp1A0;struct Cyc_List_List*_tmp1A1;struct Cyc_List_List*
_tmp1A2;struct Cyc_Absyn_FnInfo _tmp1A3;struct Cyc_List_List*_tmp1A4;struct Cyc_Core_Opt*
_tmp1A5;void*_tmp1A6;struct Cyc_List_List*_tmp1A7;struct Cyc_Absyn_VarargInfo*
_tmp1A8;struct Cyc_List_List*_tmp1A9;void*_tmp1AA;struct Cyc_List_List*_tmp1AB;
_LL146: if((int)_tmp190 != 0)goto _LL148;_LL147: goto _LL149;_LL148: if((int)_tmp190 != 
1)goto _LL14A;_LL149: goto _LL14B;_LL14A: if(_tmp190 <= (void*)4)goto _LL16E;if(*((int*)
_tmp190)!= 6)goto _LL14C;_LL14B: goto _LL14D;_LL14C: if(*((int*)_tmp190)!= 12)goto
_LL14E;_LL14D: goto _LL14F;_LL14E: if(*((int*)_tmp190)!= 13)goto _LL150;_LL14F: goto
_LL151;_LL150: if(*((int*)_tmp190)!= 17)goto _LL152;_LL151: goto _LL153;_LL152: if(*((
int*)_tmp190)!= 5)goto _LL154;_LL153: return Cyc_Absyn_empty_effect;_LL154: if(*((
int*)_tmp190)!= 0)goto _LL156;_LL155: goto _LL157;_LL156: if(*((int*)_tmp190)!= 1)
goto _LL158;_LL157: {void*_tmp1AC=Cyc_Tcutil_typ_kind(t);_LL17B: if((int)_tmp1AC != 
3)goto _LL17D;_LL17C: goto _LL17E;_LL17D: if((int)_tmp1AC != 4)goto _LL17F;_LL17E: goto
_LL180;_LL17F: if((int)_tmp1AC != 5)goto _LL181;_LL180: {struct Cyc_Absyn_AccessEff_struct
_tmpC46;struct Cyc_Absyn_AccessEff_struct*_tmpC45;return(void*)((_tmpC45=
_cycalloc(sizeof(*_tmpC45)),((_tmpC45[0]=((_tmpC46.tag=19,((_tmpC46.f1=(void*)t,
_tmpC46)))),_tmpC45))));}_LL181: if((int)_tmp1AC != 6)goto _LL183;_LL182: return t;
_LL183: if((int)_tmp1AC != 7)goto _LL185;_LL184: return Cyc_Absyn_empty_effect;_LL185:;
_LL186: {struct Cyc_Absyn_RgnsEff_struct _tmpC49;struct Cyc_Absyn_RgnsEff_struct*
_tmpC48;return(void*)((_tmpC48=_cycalloc(sizeof(*_tmpC48)),((_tmpC48[0]=((
_tmpC49.tag=21,((_tmpC49.f1=(void*)t,_tmpC49)))),_tmpC48))));}_LL17A:;}_LL158:
if(*((int*)_tmp190)!= 14)goto _LL15A;_tmp191=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp190)->f1;_LL159: {struct Cyc_Absyn_AccessEff_struct _tmpC4C;struct Cyc_Absyn_AccessEff_struct*
_tmpC4B;return(void*)((_tmpC4B=_cycalloc(sizeof(*_tmpC4B)),((_tmpC4B[0]=((
_tmpC4C.tag=19,((_tmpC4C.f1=(void*)_tmp191,_tmpC4C)))),_tmpC4B))));}_LL15A: if(*((
int*)_tmp190)!= 15)goto _LL15C;_tmp192=(void*)((struct Cyc_Absyn_DynRgnType_struct*)
_tmp190)->f1;_tmp193=(void*)((struct Cyc_Absyn_DynRgnType_struct*)_tmp190)->f2;
_LL15B: {struct Cyc_Absyn_AccessEff_struct _tmpC4F;struct Cyc_Absyn_AccessEff_struct*
_tmpC4E;return(void*)((_tmpC4E=_cycalloc(sizeof(*_tmpC4E)),((_tmpC4E[0]=((
_tmpC4F.tag=19,((_tmpC4F.f1=(void*)_tmp193,_tmpC4F)))),_tmpC4E))));}_LL15C: if(*((
int*)_tmp190)!= 2)goto _LL15E;_tmp194=((struct Cyc_Absyn_TunionType_struct*)
_tmp190)->f1;_tmp195=_tmp194.targs;_tmp196=_tmp194.rgn;_LL15D: {struct Cyc_List_List*
ts=Cyc_List_map(Cyc_Tcutil_rgns_of,_tmp195);if((unsigned int)_tmp196){struct Cyc_Absyn_AccessEff_struct*
_tmpC55;struct Cyc_Absyn_AccessEff_struct _tmpC54;struct Cyc_List_List*_tmpC53;ts=((
_tmpC53=_cycalloc(sizeof(*_tmpC53)),((_tmpC53->hd=(void*)((void*)((_tmpC55=
_cycalloc(sizeof(*_tmpC55)),((_tmpC55[0]=((_tmpC54.tag=19,((_tmpC54.f1=(void*)((
void*)_tmp196->v),_tmpC54)))),_tmpC55))))),((_tmpC53->tl=ts,_tmpC53))))));}{
struct Cyc_Absyn_JoinEff_struct _tmpC58;struct Cyc_Absyn_JoinEff_struct*_tmpC57;
return Cyc_Tcutil_normalize_effect((void*)((_tmpC57=_cycalloc(sizeof(*_tmpC57)),((
_tmpC57[0]=((_tmpC58.tag=20,((_tmpC58.f1=ts,_tmpC58)))),_tmpC57)))));}}_LL15E:
if(*((int*)_tmp190)!= 4)goto _LL160;_tmp197=((struct Cyc_Absyn_PointerType_struct*)
_tmp190)->f1;_tmp198=(void*)_tmp197.elt_typ;_tmp199=_tmp197.ptr_atts;_tmp19A=(
void*)_tmp199.rgn;_LL15F: {struct Cyc_Absyn_JoinEff_struct _tmpC67;struct Cyc_Absyn_AccessEff_struct*
_tmpC66;struct Cyc_Absyn_AccessEff_struct _tmpC65;void*_tmpC64[2];struct Cyc_Absyn_JoinEff_struct*
_tmpC63;return Cyc_Tcutil_normalize_effect((void*)((_tmpC63=_cycalloc(sizeof(*
_tmpC63)),((_tmpC63[0]=((_tmpC67.tag=20,((_tmpC67.f1=((_tmpC64[1]=Cyc_Tcutil_rgns_of(
_tmp198),((_tmpC64[0]=(void*)((_tmpC66=_cycalloc(sizeof(*_tmpC66)),((_tmpC66[0]=((
_tmpC65.tag=19,((_tmpC65.f1=(void*)_tmp19A,_tmpC65)))),_tmpC66)))),Cyc_List_list(
_tag_dyneither(_tmpC64,sizeof(void*),2)))))),_tmpC67)))),_tmpC63)))));}_LL160:
if(*((int*)_tmp190)!= 7)goto _LL162;_tmp19B=((struct Cyc_Absyn_ArrayType_struct*)
_tmp190)->f1;_tmp19C=(void*)_tmp19B.elt_type;_LL161: return Cyc_Tcutil_normalize_effect(
Cyc_Tcutil_rgns_of(_tmp19C));_LL162: if(*((int*)_tmp190)!= 9)goto _LL164;_tmp19D=((
struct Cyc_Absyn_TupleType_struct*)_tmp190)->f1;_LL163: {struct Cyc_List_List*
_tmp1BF=0;for(0;_tmp19D != 0;_tmp19D=_tmp19D->tl){struct Cyc_List_List*_tmpC68;
_tmp1BF=((_tmpC68=_cycalloc(sizeof(*_tmpC68)),((_tmpC68->hd=(void*)(*((struct
_tuple5*)_tmp19D->hd)).f2,((_tmpC68->tl=_tmp1BF,_tmpC68))))));}_tmp19F=_tmp1BF;
goto _LL165;}_LL164: if(*((int*)_tmp190)!= 3)goto _LL166;_tmp19E=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp190)->f1;_tmp19F=_tmp19E.targs;_LL165: _tmp1A1=_tmp19F;goto _LL167;_LL166: if(*((
int*)_tmp190)!= 10)goto _LL168;_tmp1A0=((struct Cyc_Absyn_AggrType_struct*)_tmp190)->f1;
_tmp1A1=_tmp1A0.targs;_LL167: {struct Cyc_Absyn_JoinEff_struct _tmpC6B;struct Cyc_Absyn_JoinEff_struct*
_tmpC6A;return Cyc_Tcutil_normalize_effect((void*)((_tmpC6A=_cycalloc(sizeof(*
_tmpC6A)),((_tmpC6A[0]=((_tmpC6B.tag=20,((_tmpC6B.f1=Cyc_List_map(Cyc_Tcutil_rgns_of,
_tmp1A1),_tmpC6B)))),_tmpC6A)))));}_LL168: if(*((int*)_tmp190)!= 11)goto _LL16A;
_tmp1A2=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp190)->f2;_LL169: {struct Cyc_Absyn_JoinEff_struct
_tmpC6E;struct Cyc_Absyn_JoinEff_struct*_tmpC6D;return Cyc_Tcutil_normalize_effect((
void*)((_tmpC6D=_cycalloc(sizeof(*_tmpC6D)),((_tmpC6D[0]=((_tmpC6E.tag=20,((
_tmpC6E.f1=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Aggrfield*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of_field,_tmp1A2),_tmpC6E)))),
_tmpC6D)))));}_LL16A: if(*((int*)_tmp190)!= 18)goto _LL16C;_LL16B: return Cyc_Absyn_empty_effect;
_LL16C: if(*((int*)_tmp190)!= 8)goto _LL16E;_tmp1A3=((struct Cyc_Absyn_FnType_struct*)
_tmp190)->f1;_tmp1A4=_tmp1A3.tvars;_tmp1A5=_tmp1A3.effect;_tmp1A6=(void*)_tmp1A3.ret_typ;
_tmp1A7=_tmp1A3.args;_tmp1A8=_tmp1A3.cyc_varargs;_tmp1A9=_tmp1A3.rgn_po;_LL16D: {
void*_tmp1C5=Cyc_Tcutil_substitute(((struct Cyc_List_List*(*)(struct _tuple8*(*f)(
struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_region_free_subst,
_tmp1A4),(void*)((struct Cyc_Core_Opt*)_check_null(_tmp1A5))->v);return Cyc_Tcutil_normalize_effect(
_tmp1C5);}_LL16E: if((int)_tmp190 != 3)goto _LL170;_LL16F: goto _LL171;_LL170: if((int)
_tmp190 != 2)goto _LL172;_LL171: return Cyc_Absyn_empty_effect;_LL172: if(_tmp190 <= (
void*)4)goto _LL174;if(*((int*)_tmp190)!= 19)goto _LL174;_LL173: goto _LL175;_LL174:
if(_tmp190 <= (void*)4)goto _LL176;if(*((int*)_tmp190)!= 20)goto _LL176;_LL175:
return t;_LL176: if(_tmp190 <= (void*)4)goto _LL178;if(*((int*)_tmp190)!= 21)goto
_LL178;_tmp1AA=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp190)->f1;_LL177:
return Cyc_Tcutil_rgns_of(_tmp1AA);_LL178: if(_tmp190 <= (void*)4)goto _LL145;if(*((
int*)_tmp190)!= 16)goto _LL145;_tmp1AB=((struct Cyc_Absyn_TypedefType_struct*)
_tmp190)->f2;_LL179: {struct Cyc_Absyn_JoinEff_struct _tmpC71;struct Cyc_Absyn_JoinEff_struct*
_tmpC70;return Cyc_Tcutil_normalize_effect((void*)((_tmpC70=_cycalloc(sizeof(*
_tmpC70)),((_tmpC70[0]=((_tmpC71.tag=20,((_tmpC71.f1=Cyc_List_map(Cyc_Tcutil_rgns_of,
_tmp1AB),_tmpC71)))),_tmpC70)))));}_LL145:;}void*Cyc_Tcutil_normalize_effect(
void*e);void*Cyc_Tcutil_normalize_effect(void*e){e=Cyc_Tcutil_compress(e);{void*
_tmp1C8=e;struct Cyc_List_List*_tmp1C9;struct Cyc_List_List**_tmp1CA;void*_tmp1CB;
_LL188: if(_tmp1C8 <= (void*)4)goto _LL18C;if(*((int*)_tmp1C8)!= 20)goto _LL18A;
_tmp1C9=((struct Cyc_Absyn_JoinEff_struct*)_tmp1C8)->f1;_tmp1CA=(struct Cyc_List_List**)&((
struct Cyc_Absyn_JoinEff_struct*)_tmp1C8)->f1;_LL189: {int redo_join=0;{struct Cyc_List_List*
effs=*_tmp1CA;for(0;effs != 0;effs=effs->tl){void*_tmp1CC=(void*)effs->hd;(void*)(
effs->hd=(void*)Cyc_Tcutil_compress(Cyc_Tcutil_normalize_effect(_tmp1CC)));{void*
_tmp1CD=(void*)effs->hd;void*_tmp1CE;_LL18F: if(_tmp1CD <= (void*)4)goto _LL193;if(*((
int*)_tmp1CD)!= 20)goto _LL191;_LL190: goto _LL192;_LL191: if(*((int*)_tmp1CD)!= 19)
goto _LL193;_tmp1CE=(void*)((struct Cyc_Absyn_AccessEff_struct*)_tmp1CD)->f1;if((
int)_tmp1CE != 2)goto _LL193;_LL192: redo_join=1;goto _LL18E;_LL193:;_LL194: goto
_LL18E;_LL18E:;}}}if(!redo_join)return e;{struct Cyc_List_List*effects=0;{struct
Cyc_List_List*effs=*_tmp1CA;for(0;effs != 0;effs=effs->tl){void*_tmp1CF=Cyc_Tcutil_compress((
void*)effs->hd);struct Cyc_List_List*_tmp1D0;void*_tmp1D1;_LL196: if(_tmp1CF <= (
void*)4)goto _LL19A;if(*((int*)_tmp1CF)!= 20)goto _LL198;_tmp1D0=((struct Cyc_Absyn_JoinEff_struct*)
_tmp1CF)->f1;_LL197: effects=Cyc_List_revappend(_tmp1D0,effects);goto _LL195;
_LL198: if(*((int*)_tmp1CF)!= 19)goto _LL19A;_tmp1D1=(void*)((struct Cyc_Absyn_AccessEff_struct*)
_tmp1CF)->f1;if((int)_tmp1D1 != 2)goto _LL19A;_LL199: goto _LL195;_LL19A:;_LL19B:{
struct Cyc_List_List*_tmpC72;effects=((_tmpC72=_cycalloc(sizeof(*_tmpC72)),((
_tmpC72->hd=(void*)_tmp1CF,((_tmpC72->tl=effects,_tmpC72))))));}goto _LL195;
_LL195:;}}*_tmp1CA=Cyc_List_imp_rev(effects);return e;}}_LL18A: if(*((int*)_tmp1C8)
!= 21)goto _LL18C;_tmp1CB=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp1C8)->f1;
_LL18B: {void*_tmp1D3=Cyc_Tcutil_compress(_tmp1CB);_LL19D: if(_tmp1D3 <= (void*)4)
goto _LL1A1;if(*((int*)_tmp1D3)!= 0)goto _LL19F;_LL19E: goto _LL1A0;_LL19F: if(*((int*)
_tmp1D3)!= 1)goto _LL1A1;_LL1A0: return e;_LL1A1:;_LL1A2: return Cyc_Tcutil_rgns_of(
_tmp1CB);_LL19C:;}_LL18C:;_LL18D: return e;_LL187:;}}static void*Cyc_Tcutil_dummy_fntype(
void*eff);static void*Cyc_Tcutil_dummy_fntype(void*eff){struct Cyc_Absyn_FnType_struct
_tmpC7C;struct Cyc_Core_Opt*_tmpC7B;struct Cyc_Absyn_FnInfo _tmpC7A;struct Cyc_Absyn_FnType_struct*
_tmpC79;struct Cyc_Absyn_FnType_struct*_tmp1D4=(_tmpC79=_cycalloc(sizeof(*_tmpC79)),((
_tmpC79[0]=((_tmpC7C.tag=8,((_tmpC7C.f1=((_tmpC7A.tvars=0,((_tmpC7A.effect=((
_tmpC7B=_cycalloc(sizeof(*_tmpC7B)),((_tmpC7B->v=(void*)eff,_tmpC7B)))),((
_tmpC7A.ret_typ=(void*)((void*)0),((_tmpC7A.args=0,((_tmpC7A.c_varargs=0,((
_tmpC7A.cyc_varargs=0,((_tmpC7A.rgn_po=0,((_tmpC7A.attributes=0,_tmpC7A)))))))))))))))),
_tmpC7C)))),_tmpC79)));return Cyc_Absyn_atb_typ((void*)_tmp1D4,(void*)2,Cyc_Absyn_empty_tqual(
0),Cyc_Absyn_bounds_one,Cyc_Absyn_false_conref);}int Cyc_Tcutil_region_in_effect(
int constrain,void*r,void*e);int Cyc_Tcutil_region_in_effect(int constrain,void*r,
void*e){r=Cyc_Tcutil_compress(r);if(r == (void*)2  || r == (void*)3)return 1;{void*
_tmp1D9=Cyc_Tcutil_compress(e);void*_tmp1DA;struct Cyc_List_List*_tmp1DB;void*
_tmp1DC;struct Cyc_Core_Opt*_tmp1DD;struct Cyc_Core_Opt*_tmp1DE;struct Cyc_Core_Opt**
_tmp1DF;struct Cyc_Core_Opt*_tmp1E0;_LL1A4: if(_tmp1D9 <= (void*)4)goto _LL1AC;if(*((
int*)_tmp1D9)!= 19)goto _LL1A6;_tmp1DA=(void*)((struct Cyc_Absyn_AccessEff_struct*)
_tmp1D9)->f1;_LL1A5: if(constrain)return Cyc_Tcutil_unify(r,_tmp1DA);_tmp1DA=Cyc_Tcutil_compress(
_tmp1DA);if(r == _tmp1DA)return 1;{struct _tuple0 _tmpC7D;struct _tuple0 _tmp1E2=(
_tmpC7D.f1=r,((_tmpC7D.f2=_tmp1DA,_tmpC7D)));void*_tmp1E3;struct Cyc_Absyn_Tvar*
_tmp1E4;void*_tmp1E5;struct Cyc_Absyn_Tvar*_tmp1E6;_LL1AF: _tmp1E3=_tmp1E2.f1;if(
_tmp1E3 <= (void*)4)goto _LL1B1;if(*((int*)_tmp1E3)!= 1)goto _LL1B1;_tmp1E4=((
struct Cyc_Absyn_VarType_struct*)_tmp1E3)->f1;_tmp1E5=_tmp1E2.f2;if(_tmp1E5 <= (
void*)4)goto _LL1B1;if(*((int*)_tmp1E5)!= 1)goto _LL1B1;_tmp1E6=((struct Cyc_Absyn_VarType_struct*)
_tmp1E5)->f1;_LL1B0: return Cyc_Absyn_tvar_cmp(_tmp1E4,_tmp1E6)== 0;_LL1B1:;_LL1B2:
return 0;_LL1AE:;}_LL1A6: if(*((int*)_tmp1D9)!= 20)goto _LL1A8;_tmp1DB=((struct Cyc_Absyn_JoinEff_struct*)
_tmp1D9)->f1;_LL1A7: for(0;_tmp1DB != 0;_tmp1DB=_tmp1DB->tl){if(Cyc_Tcutil_region_in_effect(
constrain,r,(void*)_tmp1DB->hd))return 1;}return 0;_LL1A8: if(*((int*)_tmp1D9)!= 21)
goto _LL1AA;_tmp1DC=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp1D9)->f1;_LL1A9: {
void*_tmp1E7=Cyc_Tcutil_rgns_of(_tmp1DC);void*_tmp1E8;_LL1B4: if(_tmp1E7 <= (void*)
4)goto _LL1B6;if(*((int*)_tmp1E7)!= 21)goto _LL1B6;_tmp1E8=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp1E7)->f1;_LL1B5: if(!constrain)return 0;{void*_tmp1E9=Cyc_Tcutil_compress(
_tmp1E8);struct Cyc_Core_Opt*_tmp1EA;struct Cyc_Core_Opt*_tmp1EB;struct Cyc_Core_Opt**
_tmp1EC;struct Cyc_Core_Opt*_tmp1ED;_LL1B9: if(_tmp1E9 <= (void*)4)goto _LL1BB;if(*((
int*)_tmp1E9)!= 0)goto _LL1BB;_tmp1EA=((struct Cyc_Absyn_Evar_struct*)_tmp1E9)->f1;
_tmp1EB=((struct Cyc_Absyn_Evar_struct*)_tmp1E9)->f2;_tmp1EC=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Evar_struct*)_tmp1E9)->f2;_tmp1ED=((struct Cyc_Absyn_Evar_struct*)
_tmp1E9)->f4;_LL1BA: {void*_tmp1EE=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,
_tmp1ED);Cyc_Tcutil_occurs(_tmp1EE,Cyc_Core_heap_region,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp1ED))->v,r);{struct Cyc_Absyn_JoinEff_struct
_tmpC8C;struct Cyc_Absyn_AccessEff_struct*_tmpC8B;struct Cyc_Absyn_AccessEff_struct
_tmpC8A;void*_tmpC89[2];struct Cyc_Absyn_JoinEff_struct*_tmpC88;void*_tmp1EF=Cyc_Tcutil_dummy_fntype((
void*)((_tmpC88=_cycalloc(sizeof(*_tmpC88)),((_tmpC88[0]=((_tmpC8C.tag=20,((
_tmpC8C.f1=((_tmpC89[1]=(void*)((_tmpC8B=_cycalloc(sizeof(*_tmpC8B)),((_tmpC8B[0]=((
_tmpC8A.tag=19,((_tmpC8A.f1=(void*)r,_tmpC8A)))),_tmpC8B)))),((_tmpC89[0]=
_tmp1EE,Cyc_List_list(_tag_dyneither(_tmpC89,sizeof(void*),2)))))),_tmpC8C)))),
_tmpC88)))));{struct Cyc_Core_Opt*_tmpC8D;*_tmp1EC=((_tmpC8D=_cycalloc(sizeof(*
_tmpC8D)),((_tmpC8D->v=(void*)_tmp1EF,_tmpC8D))));}return 1;}}_LL1BB:;_LL1BC:
return 0;_LL1B8:;}_LL1B6:;_LL1B7: return Cyc_Tcutil_region_in_effect(constrain,r,
_tmp1E7);_LL1B3:;}_LL1AA: if(*((int*)_tmp1D9)!= 0)goto _LL1AC;_tmp1DD=((struct Cyc_Absyn_Evar_struct*)
_tmp1D9)->f1;_tmp1DE=((struct Cyc_Absyn_Evar_struct*)_tmp1D9)->f2;_tmp1DF=(struct
Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)_tmp1D9)->f2;_tmp1E0=((struct Cyc_Absyn_Evar_struct*)
_tmp1D9)->f4;_LL1AB: if(_tmp1DD == 0  || (void*)_tmp1DD->v != (void*)6){const char*
_tmpC90;void*_tmpC8F;(_tmpC8F=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpC90="effect evar has wrong kind",
_tag_dyneither(_tmpC90,sizeof(char),27))),_tag_dyneither(_tmpC8F,sizeof(void*),0)));}
if(!constrain)return 0;{void*_tmp1F8=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,
_tmp1E0);Cyc_Tcutil_occurs(_tmp1F8,Cyc_Core_heap_region,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp1E0))->v,r);{struct Cyc_Absyn_JoinEff_struct
_tmpCA5;struct Cyc_List_List*_tmpCA4;struct Cyc_Absyn_AccessEff_struct _tmpCA3;
struct Cyc_Absyn_AccessEff_struct*_tmpCA2;struct Cyc_List_List*_tmpCA1;struct Cyc_Absyn_JoinEff_struct*
_tmpCA0;struct Cyc_Absyn_JoinEff_struct*_tmp1F9=(_tmpCA0=_cycalloc(sizeof(*
_tmpCA0)),((_tmpCA0[0]=((_tmpCA5.tag=20,((_tmpCA5.f1=((_tmpCA1=_cycalloc(sizeof(*
_tmpCA1)),((_tmpCA1->hd=(void*)_tmp1F8,((_tmpCA1->tl=((_tmpCA4=_cycalloc(sizeof(*
_tmpCA4)),((_tmpCA4->hd=(void*)((void*)((_tmpCA2=_cycalloc(sizeof(*_tmpCA2)),((
_tmpCA2[0]=((_tmpCA3.tag=19,((_tmpCA3.f1=(void*)r,_tmpCA3)))),_tmpCA2))))),((
_tmpCA4->tl=0,_tmpCA4)))))),_tmpCA1)))))),_tmpCA5)))),_tmpCA0)));{struct Cyc_Core_Opt*
_tmpCA6;*_tmp1DF=((_tmpCA6=_cycalloc(sizeof(*_tmpCA6)),((_tmpCA6->v=(void*)((
void*)_tmp1F9),_tmpCA6))));}return 1;}}_LL1AC:;_LL1AD: return 0;_LL1A3:;}}static int
Cyc_Tcutil_type_in_effect(int may_constrain_evars,void*t,void*e);static int Cyc_Tcutil_type_in_effect(
int may_constrain_evars,void*t,void*e){t=Cyc_Tcutil_compress(t);{void*_tmp201=Cyc_Tcutil_normalize_effect(
Cyc_Tcutil_compress(e));struct Cyc_List_List*_tmp202;void*_tmp203;struct Cyc_Core_Opt*
_tmp204;struct Cyc_Core_Opt*_tmp205;struct Cyc_Core_Opt**_tmp206;struct Cyc_Core_Opt*
_tmp207;_LL1BE: if(_tmp201 <= (void*)4)goto _LL1C6;if(*((int*)_tmp201)!= 19)goto
_LL1C0;_LL1BF: return 0;_LL1C0: if(*((int*)_tmp201)!= 20)goto _LL1C2;_tmp202=((
struct Cyc_Absyn_JoinEff_struct*)_tmp201)->f1;_LL1C1: for(0;_tmp202 != 0;_tmp202=
_tmp202->tl){if(Cyc_Tcutil_type_in_effect(may_constrain_evars,t,(void*)_tmp202->hd))
return 1;}return 0;_LL1C2: if(*((int*)_tmp201)!= 21)goto _LL1C4;_tmp203=(void*)((
struct Cyc_Absyn_RgnsEff_struct*)_tmp201)->f1;_LL1C3: _tmp203=Cyc_Tcutil_compress(
_tmp203);if(t == _tmp203)return 1;if(may_constrain_evars)return Cyc_Tcutil_unify(t,
_tmp203);{void*_tmp208=Cyc_Tcutil_rgns_of(t);void*_tmp209;_LL1C9: if(_tmp208 <= (
void*)4)goto _LL1CB;if(*((int*)_tmp208)!= 21)goto _LL1CB;_tmp209=(void*)((struct
Cyc_Absyn_RgnsEff_struct*)_tmp208)->f1;_LL1CA: {struct _tuple0 _tmpCA7;struct
_tuple0 _tmp20B=(_tmpCA7.f1=Cyc_Tcutil_compress(_tmp209),((_tmpCA7.f2=_tmp203,
_tmpCA7)));void*_tmp20C;struct Cyc_Absyn_Tvar*_tmp20D;void*_tmp20E;struct Cyc_Absyn_Tvar*
_tmp20F;_LL1CE: _tmp20C=_tmp20B.f1;if(_tmp20C <= (void*)4)goto _LL1D0;if(*((int*)
_tmp20C)!= 1)goto _LL1D0;_tmp20D=((struct Cyc_Absyn_VarType_struct*)_tmp20C)->f1;
_tmp20E=_tmp20B.f2;if(_tmp20E <= (void*)4)goto _LL1D0;if(*((int*)_tmp20E)!= 1)goto
_LL1D0;_tmp20F=((struct Cyc_Absyn_VarType_struct*)_tmp20E)->f1;_LL1CF: return Cyc_Tcutil_unify(
t,_tmp203);_LL1D0:;_LL1D1: return _tmp209 == _tmp203;_LL1CD:;}_LL1CB:;_LL1CC: return
Cyc_Tcutil_type_in_effect(may_constrain_evars,t,_tmp208);_LL1C8:;}_LL1C4: if(*((
int*)_tmp201)!= 0)goto _LL1C6;_tmp204=((struct Cyc_Absyn_Evar_struct*)_tmp201)->f1;
_tmp205=((struct Cyc_Absyn_Evar_struct*)_tmp201)->f2;_tmp206=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Evar_struct*)_tmp201)->f2;_tmp207=((struct Cyc_Absyn_Evar_struct*)
_tmp201)->f4;_LL1C5: if(_tmp204 == 0  || (void*)_tmp204->v != (void*)6){const char*
_tmpCAA;void*_tmpCA9;(_tmpCA9=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpCAA="effect evar has wrong kind",
_tag_dyneither(_tmpCAA,sizeof(char),27))),_tag_dyneither(_tmpCA9,sizeof(void*),0)));}
if(!may_constrain_evars)return 0;{void*_tmp212=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_ek,_tmp207);Cyc_Tcutil_occurs(_tmp212,Cyc_Core_heap_region,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp207))->v,t);{struct Cyc_Absyn_JoinEff_struct
_tmpCBF;struct Cyc_List_List*_tmpCBE;struct Cyc_Absyn_RgnsEff_struct _tmpCBD;struct
Cyc_Absyn_RgnsEff_struct*_tmpCBC;struct Cyc_List_List*_tmpCBB;struct Cyc_Absyn_JoinEff_struct*
_tmpCBA;struct Cyc_Absyn_JoinEff_struct*_tmp213=(_tmpCBA=_cycalloc(sizeof(*
_tmpCBA)),((_tmpCBA[0]=((_tmpCBF.tag=20,((_tmpCBF.f1=((_tmpCBB=_cycalloc(sizeof(*
_tmpCBB)),((_tmpCBB->hd=(void*)_tmp212,((_tmpCBB->tl=((_tmpCBE=_cycalloc(sizeof(*
_tmpCBE)),((_tmpCBE->hd=(void*)((void*)((_tmpCBC=_cycalloc(sizeof(*_tmpCBC)),((
_tmpCBC[0]=((_tmpCBD.tag=21,((_tmpCBD.f1=(void*)t,_tmpCBD)))),_tmpCBC))))),((
_tmpCBE->tl=0,_tmpCBE)))))),_tmpCBB)))))),_tmpCBF)))),_tmpCBA)));{struct Cyc_Core_Opt*
_tmpCC0;*_tmp206=((_tmpCC0=_cycalloc(sizeof(*_tmpCC0)),((_tmpCC0->v=(void*)((
void*)_tmp213),_tmpCC0))));}return 1;}}_LL1C6:;_LL1C7: return 0;_LL1BD:;}}static int
Cyc_Tcutil_variable_in_effect(int may_constrain_evars,struct Cyc_Absyn_Tvar*v,void*
e);static int Cyc_Tcutil_variable_in_effect(int may_constrain_evars,struct Cyc_Absyn_Tvar*
v,void*e){e=Cyc_Tcutil_compress(e);{void*_tmp21B=e;struct Cyc_Absyn_Tvar*_tmp21C;
struct Cyc_List_List*_tmp21D;void*_tmp21E;struct Cyc_Core_Opt*_tmp21F;struct Cyc_Core_Opt*
_tmp220;struct Cyc_Core_Opt**_tmp221;struct Cyc_Core_Opt*_tmp222;_LL1D3: if(_tmp21B
<= (void*)4)goto _LL1DB;if(*((int*)_tmp21B)!= 1)goto _LL1D5;_tmp21C=((struct Cyc_Absyn_VarType_struct*)
_tmp21B)->f1;_LL1D4: return Cyc_Absyn_tvar_cmp(v,_tmp21C)== 0;_LL1D5: if(*((int*)
_tmp21B)!= 20)goto _LL1D7;_tmp21D=((struct Cyc_Absyn_JoinEff_struct*)_tmp21B)->f1;
_LL1D6: for(0;_tmp21D != 0;_tmp21D=_tmp21D->tl){if(Cyc_Tcutil_variable_in_effect(
may_constrain_evars,v,(void*)_tmp21D->hd))return 1;}return 0;_LL1D7: if(*((int*)
_tmp21B)!= 21)goto _LL1D9;_tmp21E=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp21B)->f1;_LL1D8: {void*_tmp223=Cyc_Tcutil_rgns_of(_tmp21E);void*_tmp224;
_LL1DE: if(_tmp223 <= (void*)4)goto _LL1E0;if(*((int*)_tmp223)!= 21)goto _LL1E0;
_tmp224=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp223)->f1;_LL1DF: if(!
may_constrain_evars)return 0;{void*_tmp225=Cyc_Tcutil_compress(_tmp224);struct Cyc_Core_Opt*
_tmp226;struct Cyc_Core_Opt*_tmp227;struct Cyc_Core_Opt**_tmp228;struct Cyc_Core_Opt*
_tmp229;_LL1E3: if(_tmp225 <= (void*)4)goto _LL1E5;if(*((int*)_tmp225)!= 0)goto
_LL1E5;_tmp226=((struct Cyc_Absyn_Evar_struct*)_tmp225)->f1;_tmp227=((struct Cyc_Absyn_Evar_struct*)
_tmp225)->f2;_tmp228=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)
_tmp225)->f2;_tmp229=((struct Cyc_Absyn_Evar_struct*)_tmp225)->f4;_LL1E4: {void*
_tmp22A=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,_tmp229);if(!((
int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*
l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp229))->v,v))return 0;{struct Cyc_Absyn_JoinEff_struct
_tmpCCF;struct Cyc_Absyn_VarType_struct*_tmpCCE;struct Cyc_Absyn_VarType_struct
_tmpCCD;void*_tmpCCC[2];struct Cyc_Absyn_JoinEff_struct*_tmpCCB;void*_tmp22B=Cyc_Tcutil_dummy_fntype((
void*)((_tmpCCB=_cycalloc(sizeof(*_tmpCCB)),((_tmpCCB[0]=((_tmpCCF.tag=20,((
_tmpCCF.f1=((_tmpCCC[1]=(void*)((_tmpCCE=_cycalloc(sizeof(*_tmpCCE)),((_tmpCCE[0]=((
_tmpCCD.tag=1,((_tmpCCD.f1=v,_tmpCCD)))),_tmpCCE)))),((_tmpCCC[0]=_tmp22A,Cyc_List_list(
_tag_dyneither(_tmpCCC,sizeof(void*),2)))))),_tmpCCF)))),_tmpCCB)))));{struct Cyc_Core_Opt*
_tmpCD0;*_tmp228=((_tmpCD0=_cycalloc(sizeof(*_tmpCD0)),((_tmpCD0->v=(void*)
_tmp22B,_tmpCD0))));}return 1;}}_LL1E5:;_LL1E6: return 0;_LL1E2:;}_LL1E0:;_LL1E1:
return Cyc_Tcutil_variable_in_effect(may_constrain_evars,v,_tmp223);_LL1DD:;}
_LL1D9: if(*((int*)_tmp21B)!= 0)goto _LL1DB;_tmp21F=((struct Cyc_Absyn_Evar_struct*)
_tmp21B)->f1;_tmp220=((struct Cyc_Absyn_Evar_struct*)_tmp21B)->f2;_tmp221=(struct
Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)_tmp21B)->f2;_tmp222=((struct Cyc_Absyn_Evar_struct*)
_tmp21B)->f4;_LL1DA: if(_tmp21F == 0  || (void*)_tmp21F->v != (void*)6){const char*
_tmpCD3;void*_tmpCD2;(_tmpCD2=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpCD3="effect evar has wrong kind",
_tag_dyneither(_tmpCD3,sizeof(char),27))),_tag_dyneither(_tmpCD2,sizeof(void*),0)));}{
void*_tmp234=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,_tmp222);if(
!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*
l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp222))->v,v))return 0;{struct Cyc_Absyn_JoinEff_struct
_tmpCE8;struct Cyc_List_List*_tmpCE7;struct Cyc_Absyn_VarType_struct _tmpCE6;struct
Cyc_Absyn_VarType_struct*_tmpCE5;struct Cyc_List_List*_tmpCE4;struct Cyc_Absyn_JoinEff_struct*
_tmpCE3;struct Cyc_Absyn_JoinEff_struct*_tmp235=(_tmpCE3=_cycalloc(sizeof(*
_tmpCE3)),((_tmpCE3[0]=((_tmpCE8.tag=20,((_tmpCE8.f1=((_tmpCE4=_cycalloc(sizeof(*
_tmpCE4)),((_tmpCE4->hd=(void*)_tmp234,((_tmpCE4->tl=((_tmpCE7=_cycalloc(sizeof(*
_tmpCE7)),((_tmpCE7->hd=(void*)((void*)((_tmpCE5=_cycalloc(sizeof(*_tmpCE5)),((
_tmpCE5[0]=((_tmpCE6.tag=1,((_tmpCE6.f1=v,_tmpCE6)))),_tmpCE5))))),((_tmpCE7->tl=
0,_tmpCE7)))))),_tmpCE4)))))),_tmpCE8)))),_tmpCE3)));{struct Cyc_Core_Opt*_tmpCE9;*
_tmp221=((_tmpCE9=_cycalloc(sizeof(*_tmpCE9)),((_tmpCE9->v=(void*)((void*)
_tmp235),_tmpCE9))));}return 1;}}_LL1DB:;_LL1DC: return 0;_LL1D2:;}}static int Cyc_Tcutil_evar_in_effect(
void*evar,void*e);static int Cyc_Tcutil_evar_in_effect(void*evar,void*e){e=Cyc_Tcutil_compress(
e);{void*_tmp23D=e;struct Cyc_List_List*_tmp23E;void*_tmp23F;_LL1E8: if(_tmp23D <= (
void*)4)goto _LL1EE;if(*((int*)_tmp23D)!= 20)goto _LL1EA;_tmp23E=((struct Cyc_Absyn_JoinEff_struct*)
_tmp23D)->f1;_LL1E9: for(0;_tmp23E != 0;_tmp23E=_tmp23E->tl){if(Cyc_Tcutil_evar_in_effect(
evar,(void*)_tmp23E->hd))return 1;}return 0;_LL1EA: if(*((int*)_tmp23D)!= 21)goto
_LL1EC;_tmp23F=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp23D)->f1;_LL1EB: {
void*_tmp240=Cyc_Tcutil_rgns_of(_tmp23F);void*_tmp241;_LL1F1: if(_tmp240 <= (void*)
4)goto _LL1F3;if(*((int*)_tmp240)!= 21)goto _LL1F3;_tmp241=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp240)->f1;_LL1F2: return 0;_LL1F3:;_LL1F4: return Cyc_Tcutil_evar_in_effect(evar,
_tmp240);_LL1F0:;}_LL1EC: if(*((int*)_tmp23D)!= 0)goto _LL1EE;_LL1ED: return evar == 
e;_LL1EE:;_LL1EF: return 0;_LL1E7:;}}int Cyc_Tcutil_subset_effect(int
may_constrain_evars,void*e1,void*e2);int Cyc_Tcutil_subset_effect(int
may_constrain_evars,void*e1,void*e2){void*_tmp242=Cyc_Tcutil_compress(e1);struct
Cyc_List_List*_tmp243;void*_tmp244;struct Cyc_Absyn_Tvar*_tmp245;void*_tmp246;
struct Cyc_Core_Opt*_tmp247;struct Cyc_Core_Opt**_tmp248;struct Cyc_Core_Opt*
_tmp249;_LL1F6: if(_tmp242 <= (void*)4)goto _LL200;if(*((int*)_tmp242)!= 20)goto
_LL1F8;_tmp243=((struct Cyc_Absyn_JoinEff_struct*)_tmp242)->f1;_LL1F7: for(0;
_tmp243 != 0;_tmp243=_tmp243->tl){if(!Cyc_Tcutil_subset_effect(
may_constrain_evars,(void*)_tmp243->hd,e2))return 0;}return 1;_LL1F8: if(*((int*)
_tmp242)!= 19)goto _LL1FA;_tmp244=(void*)((struct Cyc_Absyn_AccessEff_struct*)
_tmp242)->f1;_LL1F9: return Cyc_Tcutil_region_in_effect(0,_tmp244,e2) || 
may_constrain_evars  && Cyc_Tcutil_unify(_tmp244,(void*)2);_LL1FA: if(*((int*)
_tmp242)!= 1)goto _LL1FC;_tmp245=((struct Cyc_Absyn_VarType_struct*)_tmp242)->f1;
_LL1FB: return Cyc_Tcutil_variable_in_effect(may_constrain_evars,_tmp245,e2);
_LL1FC: if(*((int*)_tmp242)!= 21)goto _LL1FE;_tmp246=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp242)->f1;_LL1FD: {void*_tmp24A=Cyc_Tcutil_rgns_of(_tmp246);void*_tmp24B;
_LL203: if(_tmp24A <= (void*)4)goto _LL205;if(*((int*)_tmp24A)!= 21)goto _LL205;
_tmp24B=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp24A)->f1;_LL204: return Cyc_Tcutil_type_in_effect(
may_constrain_evars,_tmp24B,e2) || may_constrain_evars  && Cyc_Tcutil_unify(
_tmp24B,Cyc_Absyn_sint_typ);_LL205:;_LL206: return Cyc_Tcutil_subset_effect(
may_constrain_evars,_tmp24A,e2);_LL202:;}_LL1FE: if(*((int*)_tmp242)!= 0)goto
_LL200;_tmp247=((struct Cyc_Absyn_Evar_struct*)_tmp242)->f2;_tmp248=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Evar_struct*)_tmp242)->f2;_tmp249=((struct Cyc_Absyn_Evar_struct*)
_tmp242)->f4;_LL1FF: if(!Cyc_Tcutil_evar_in_effect(e1,e2)){struct Cyc_Core_Opt*
_tmpCEA;*_tmp248=((_tmpCEA=_cycalloc(sizeof(*_tmpCEA)),((_tmpCEA->v=(void*)Cyc_Absyn_empty_effect,
_tmpCEA))));}return 1;_LL200:;_LL201: {const char*_tmpCEE;void*_tmpCED[1];struct
Cyc_String_pa_struct _tmpCEC;(_tmpCEC.tag=0,((_tmpCEC.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string(e1)),((_tmpCED[0]=& _tmpCEC,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpCEE="subset_effect: bad effect: %s",
_tag_dyneither(_tmpCEE,sizeof(char),30))),_tag_dyneither(_tmpCED,sizeof(void*),1)))))));}
_LL1F5:;}static int Cyc_Tcutil_unify_effect(void*e1,void*e2);static int Cyc_Tcutil_unify_effect(
void*e1,void*e2){e1=Cyc_Tcutil_normalize_effect(e1);e2=Cyc_Tcutil_normalize_effect(
e2);if(Cyc_Tcutil_subset_effect(0,e1,e2) && Cyc_Tcutil_subset_effect(0,e2,e1))
return 1;if(Cyc_Tcutil_subset_effect(1,e1,e2) && Cyc_Tcutil_subset_effect(1,e2,e1))
return 1;return 0;}static int Cyc_Tcutil_sub_rgnpo(struct Cyc_List_List*rpo1,struct
Cyc_List_List*rpo2);static int Cyc_Tcutil_sub_rgnpo(struct Cyc_List_List*rpo1,
struct Cyc_List_List*rpo2){{struct Cyc_List_List*r1=rpo1;for(0;r1 != 0;r1=r1->tl){
struct _tuple0 _tmp251;void*_tmp252;void*_tmp253;struct _tuple0*_tmp250=(struct
_tuple0*)r1->hd;_tmp251=*_tmp250;_tmp252=_tmp251.f1;_tmp253=_tmp251.f2;{int found=
_tmp252 == (void*)2;{struct Cyc_List_List*r2=rpo2;for(0;r2 != 0  && !found;r2=r2->tl){
struct _tuple0 _tmp255;void*_tmp256;void*_tmp257;struct _tuple0*_tmp254=(struct
_tuple0*)r2->hd;_tmp255=*_tmp254;_tmp256=_tmp255.f1;_tmp257=_tmp255.f2;if(Cyc_Tcutil_unify(
_tmp252,_tmp256) && Cyc_Tcutil_unify(_tmp253,_tmp257)){found=1;break;}}}if(!
found)return 0;}}}return 1;}static int Cyc_Tcutil_same_rgn_po(struct Cyc_List_List*
rpo1,struct Cyc_List_List*rpo2);static int Cyc_Tcutil_same_rgn_po(struct Cyc_List_List*
rpo1,struct Cyc_List_List*rpo2){return Cyc_Tcutil_sub_rgnpo(rpo1,rpo2) && Cyc_Tcutil_sub_rgnpo(
rpo2,rpo1);}struct _tuple11{struct Cyc_Absyn_VarargInfo*f1;struct Cyc_Absyn_VarargInfo*
f2;};struct _tuple12{struct Cyc_Core_Opt*f1;struct Cyc_Core_Opt*f2;};void Cyc_Tcutil_unify_it(
void*t1,void*t2);void Cyc_Tcutil_unify_it(void*t1,void*t2){Cyc_Tcutil_t1_failure=
t1;Cyc_Tcutil_t2_failure=t2;Cyc_Tcutil_failure_reason=(struct _dyneither_ptr)
_tag_dyneither(0,0,0);t1=Cyc_Tcutil_compress(t1);t2=Cyc_Tcutil_compress(t2);if(
t1 == t2)return;{void*_tmp258=t1;struct Cyc_Core_Opt*_tmp259;struct Cyc_Core_Opt*
_tmp25A;struct Cyc_Core_Opt**_tmp25B;struct Cyc_Core_Opt*_tmp25C;_LL208: if(_tmp258
<= (void*)4)goto _LL20A;if(*((int*)_tmp258)!= 0)goto _LL20A;_tmp259=((struct Cyc_Absyn_Evar_struct*)
_tmp258)->f1;_tmp25A=((struct Cyc_Absyn_Evar_struct*)_tmp258)->f2;_tmp25B=(struct
Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)_tmp258)->f2;_tmp25C=((struct Cyc_Absyn_Evar_struct*)
_tmp258)->f4;_LL209: Cyc_Tcutil_occurs(t1,Cyc_Core_heap_region,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp25C))->v,t2);{void*_tmp25D=Cyc_Tcutil_typ_kind(
t2);if(Cyc_Tcutil_kind_leq(_tmp25D,(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp259))->v)){{struct Cyc_Core_Opt*_tmpCEF;*_tmp25B=((_tmpCEF=_cycalloc(sizeof(*
_tmpCEF)),((_tmpCEF->v=(void*)t2,_tmpCEF))));}return;}else{{void*_tmp25F=t2;
struct Cyc_Core_Opt*_tmp260;struct Cyc_Core_Opt**_tmp261;struct Cyc_Core_Opt*
_tmp262;struct Cyc_Absyn_PtrInfo _tmp263;_LL20D: if(_tmp25F <= (void*)4)goto _LL211;
if(*((int*)_tmp25F)!= 0)goto _LL20F;_tmp260=((struct Cyc_Absyn_Evar_struct*)
_tmp25F)->f2;_tmp261=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)
_tmp25F)->f2;_tmp262=((struct Cyc_Absyn_Evar_struct*)_tmp25F)->f4;_LL20E: {struct
Cyc_List_List*_tmp264=(struct Cyc_List_List*)_tmp25C->v;{struct Cyc_List_List*s2=(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp262))->v;for(0;s2 != 0;
s2=s2->tl){if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,
_tmp264,(struct Cyc_Absyn_Tvar*)s2->hd)){{const char*_tmpCF0;Cyc_Tcutil_failure_reason=((
_tmpCF0="(type variable would escape scope)",_tag_dyneither(_tmpCF0,sizeof(char),
35)));}(int)_throw((void*)Cyc_Tcutil_Unify);}}}if(Cyc_Tcutil_kind_leq((void*)
_tmp259->v,_tmp25D)){{struct Cyc_Core_Opt*_tmpCF1;*_tmp261=((_tmpCF1=_cycalloc(
sizeof(*_tmpCF1)),((_tmpCF1->v=(void*)t1,_tmpCF1))));}return;}{const char*_tmpCF2;
Cyc_Tcutil_failure_reason=((_tmpCF2="(kinds are incompatible)",_tag_dyneither(
_tmpCF2,sizeof(char),25)));}goto _LL20C;}_LL20F: if(*((int*)_tmp25F)!= 4)goto
_LL211;_tmp263=((struct Cyc_Absyn_PointerType_struct*)_tmp25F)->f1;if(!((void*)
_tmp259->v == (void*)2))goto _LL211;_LL210: {struct Cyc_Absyn_Conref*_tmp268=Cyc_Absyn_compress_conref((
_tmp263.ptr_atts).bounds);{union Cyc_Absyn_Constraint_union _tmp269=_tmp268->v;
_LL214: if((_tmp269.No_constr).tag != 2)goto _LL216;_LL215:{union Cyc_Absyn_Constraint_union
_tmpCF3;_tmp268->v=(union Cyc_Absyn_Constraint_union)(((_tmpCF3.Eq_constr).tag=0,(((
_tmpCF3.Eq_constr).f1=(void*)Cyc_Absyn_bounds_one,_tmpCF3))));}{struct Cyc_Core_Opt*
_tmpCF4;*_tmp25B=((_tmpCF4=_cycalloc(sizeof(*_tmpCF4)),((_tmpCF4->v=(void*)t2,
_tmpCF4))));}return;_LL216:;_LL217: goto _LL213;_LL213:;}goto _LL20C;}_LL211:;
_LL212: goto _LL20C;_LL20C:;}{const char*_tmpCF5;Cyc_Tcutil_failure_reason=((
_tmpCF5="(kinds are incompatible)",_tag_dyneither(_tmpCF5,sizeof(char),25)));}(
int)_throw((void*)Cyc_Tcutil_Unify);}}_LL20A:;_LL20B: goto _LL207;_LL207:;}{struct
_tuple0 _tmpCF6;struct _tuple0 _tmp26E=(_tmpCF6.f1=t2,((_tmpCF6.f2=t1,_tmpCF6)));
void*_tmp26F;void*_tmp270;void*_tmp271;void*_tmp272;struct Cyc_Absyn_Tvar*_tmp273;
void*_tmp274;struct Cyc_Absyn_Tvar*_tmp275;void*_tmp276;struct Cyc_Absyn_AggrInfo
_tmp277;union Cyc_Absyn_AggrInfoU_union _tmp278;struct Cyc_List_List*_tmp279;void*
_tmp27A;struct Cyc_Absyn_AggrInfo _tmp27B;union Cyc_Absyn_AggrInfoU_union _tmp27C;
struct Cyc_List_List*_tmp27D;void*_tmp27E;struct _tuple2*_tmp27F;void*_tmp280;
struct _tuple2*_tmp281;void*_tmp282;struct Cyc_List_List*_tmp283;void*_tmp284;
struct Cyc_List_List*_tmp285;void*_tmp286;struct Cyc_Absyn_TunionInfo _tmp287;union
Cyc_Absyn_TunionInfoU_union _tmp288;struct Cyc_Absyn_Tuniondecl**_tmp289;struct Cyc_Absyn_Tuniondecl*
_tmp28A;struct Cyc_List_List*_tmp28B;struct Cyc_Core_Opt*_tmp28C;void*_tmp28D;
struct Cyc_Absyn_TunionInfo _tmp28E;union Cyc_Absyn_TunionInfoU_union _tmp28F;struct
Cyc_Absyn_Tuniondecl**_tmp290;struct Cyc_Absyn_Tuniondecl*_tmp291;struct Cyc_List_List*
_tmp292;struct Cyc_Core_Opt*_tmp293;void*_tmp294;struct Cyc_Absyn_TunionFieldInfo
_tmp295;union Cyc_Absyn_TunionFieldInfoU_union _tmp296;struct Cyc_Absyn_Tuniondecl*
_tmp297;struct Cyc_Absyn_Tunionfield*_tmp298;struct Cyc_List_List*_tmp299;void*
_tmp29A;struct Cyc_Absyn_TunionFieldInfo _tmp29B;union Cyc_Absyn_TunionFieldInfoU_union
_tmp29C;struct Cyc_Absyn_Tuniondecl*_tmp29D;struct Cyc_Absyn_Tunionfield*_tmp29E;
struct Cyc_List_List*_tmp29F;void*_tmp2A0;struct Cyc_Absyn_PtrInfo _tmp2A1;void*
_tmp2A2;struct Cyc_Absyn_Tqual _tmp2A3;struct Cyc_Absyn_PtrAtts _tmp2A4;void*_tmp2A5;
struct Cyc_Absyn_Conref*_tmp2A6;struct Cyc_Absyn_Conref*_tmp2A7;struct Cyc_Absyn_Conref*
_tmp2A8;void*_tmp2A9;struct Cyc_Absyn_PtrInfo _tmp2AA;void*_tmp2AB;struct Cyc_Absyn_Tqual
_tmp2AC;struct Cyc_Absyn_PtrAtts _tmp2AD;void*_tmp2AE;struct Cyc_Absyn_Conref*
_tmp2AF;struct Cyc_Absyn_Conref*_tmp2B0;struct Cyc_Absyn_Conref*_tmp2B1;void*
_tmp2B2;void*_tmp2B3;void*_tmp2B4;void*_tmp2B5;void*_tmp2B6;void*_tmp2B7;void*
_tmp2B8;void*_tmp2B9;void*_tmp2BA;int _tmp2BB;void*_tmp2BC;int _tmp2BD;void*
_tmp2BE;void*_tmp2BF;void*_tmp2C0;void*_tmp2C1;void*_tmp2C2;struct Cyc_Absyn_Exp*
_tmp2C3;void*_tmp2C4;struct Cyc_Absyn_Exp*_tmp2C5;void*_tmp2C6;struct Cyc_Absyn_ArrayInfo
_tmp2C7;void*_tmp2C8;struct Cyc_Absyn_Tqual _tmp2C9;struct Cyc_Absyn_Exp*_tmp2CA;
struct Cyc_Absyn_Conref*_tmp2CB;void*_tmp2CC;struct Cyc_Absyn_ArrayInfo _tmp2CD;
void*_tmp2CE;struct Cyc_Absyn_Tqual _tmp2CF;struct Cyc_Absyn_Exp*_tmp2D0;struct Cyc_Absyn_Conref*
_tmp2D1;void*_tmp2D2;struct Cyc_Absyn_FnInfo _tmp2D3;struct Cyc_List_List*_tmp2D4;
struct Cyc_Core_Opt*_tmp2D5;void*_tmp2D6;struct Cyc_List_List*_tmp2D7;int _tmp2D8;
struct Cyc_Absyn_VarargInfo*_tmp2D9;struct Cyc_List_List*_tmp2DA;struct Cyc_List_List*
_tmp2DB;void*_tmp2DC;struct Cyc_Absyn_FnInfo _tmp2DD;struct Cyc_List_List*_tmp2DE;
struct Cyc_Core_Opt*_tmp2DF;void*_tmp2E0;struct Cyc_List_List*_tmp2E1;int _tmp2E2;
struct Cyc_Absyn_VarargInfo*_tmp2E3;struct Cyc_List_List*_tmp2E4;struct Cyc_List_List*
_tmp2E5;void*_tmp2E6;struct Cyc_List_List*_tmp2E7;void*_tmp2E8;struct Cyc_List_List*
_tmp2E9;void*_tmp2EA;void*_tmp2EB;struct Cyc_List_List*_tmp2EC;void*_tmp2ED;void*
_tmp2EE;struct Cyc_List_List*_tmp2EF;void*_tmp2F0;void*_tmp2F1;void*_tmp2F2;void*
_tmp2F3;void*_tmp2F4;void*_tmp2F5;void*_tmp2F6;void*_tmp2F7;void*_tmp2F8;void*
_tmp2F9;void*_tmp2FA;void*_tmp2FB;void*_tmp2FC;void*_tmp2FD;void*_tmp2FE;void*
_tmp2FF;void*_tmp300;void*_tmp301;void*_tmp302;void*_tmp303;_LL219: _tmp26F=
_tmp26E.f1;if(_tmp26F <= (void*)4)goto _LL21B;if(*((int*)_tmp26F)!= 0)goto _LL21B;
_LL21A: Cyc_Tcutil_unify_it(t2,t1);return;_LL21B: _tmp270=_tmp26E.f1;if((int)
_tmp270 != 0)goto _LL21D;_tmp271=_tmp26E.f2;if((int)_tmp271 != 0)goto _LL21D;_LL21C:
return;_LL21D: _tmp272=_tmp26E.f1;if(_tmp272 <= (void*)4)goto _LL21F;if(*((int*)
_tmp272)!= 1)goto _LL21F;_tmp273=((struct Cyc_Absyn_VarType_struct*)_tmp272)->f1;
_tmp274=_tmp26E.f2;if(_tmp274 <= (void*)4)goto _LL21F;if(*((int*)_tmp274)!= 1)goto
_LL21F;_tmp275=((struct Cyc_Absyn_VarType_struct*)_tmp274)->f1;_LL21E: {struct
_dyneither_ptr*_tmp304=_tmp273->name;struct _dyneither_ptr*_tmp305=_tmp275->name;
int _tmp306=_tmp273->identity;int _tmp307=_tmp275->identity;void*_tmp308=Cyc_Tcutil_tvar_kind(
_tmp273);void*_tmp309=Cyc_Tcutil_tvar_kind(_tmp275);if(_tmp307 == _tmp306  && Cyc_strptrcmp(
_tmp304,_tmp305)== 0){if(_tmp308 != _tmp309){const char*_tmpCFC;void*_tmpCFB[3];
struct Cyc_String_pa_struct _tmpCFA;struct Cyc_String_pa_struct _tmpCF9;struct Cyc_String_pa_struct
_tmpCF8;(_tmpCF8.tag=0,((_tmpCF8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kind2string(_tmp309)),((_tmpCF9.tag=0,((_tmpCF9.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(_tmp308)),((
_tmpCFA.tag=0,((_tmpCFA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp304),((
_tmpCFB[0]=& _tmpCFA,((_tmpCFB[1]=& _tmpCF9,((_tmpCFB[2]=& _tmpCF8,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpCFC="same type variable %s has kinds %s and %s",
_tag_dyneither(_tmpCFC,sizeof(char),42))),_tag_dyneither(_tmpCFB,sizeof(void*),3)))))))))))))))))));}
return;}{const char*_tmpCFD;Cyc_Tcutil_failure_reason=((_tmpCFD="(variable types are not the same)",
_tag_dyneither(_tmpCFD,sizeof(char),34)));}goto _LL218;}_LL21F: _tmp276=_tmp26E.f1;
if(_tmp276 <= (void*)4)goto _LL221;if(*((int*)_tmp276)!= 10)goto _LL221;_tmp277=((
struct Cyc_Absyn_AggrType_struct*)_tmp276)->f1;_tmp278=_tmp277.aggr_info;_tmp279=
_tmp277.targs;_tmp27A=_tmp26E.f2;if(_tmp27A <= (void*)4)goto _LL221;if(*((int*)
_tmp27A)!= 10)goto _LL221;_tmp27B=((struct Cyc_Absyn_AggrType_struct*)_tmp27A)->f1;
_tmp27C=_tmp27B.aggr_info;_tmp27D=_tmp27B.targs;_LL220: {void*_tmp311;struct
_tuple2*_tmp312;struct _tuple6 _tmp310=Cyc_Absyn_aggr_kinded_name(_tmp27C);_tmp311=
_tmp310.f1;_tmp312=_tmp310.f2;{void*_tmp314;struct _tuple2*_tmp315;struct _tuple6
_tmp313=Cyc_Absyn_aggr_kinded_name(_tmp278);_tmp314=_tmp313.f1;_tmp315=_tmp313.f2;
if(_tmp311 != _tmp314){{const char*_tmpCFE;Cyc_Tcutil_failure_reason=((_tmpCFE="(struct and union type)",
_tag_dyneither(_tmpCFE,sizeof(char),24)));}goto _LL218;}if(Cyc_Absyn_qvar_cmp(
_tmp312,_tmp315)!= 0){{const char*_tmpCFF;Cyc_Tcutil_failure_reason=((_tmpCFF="(different type name)",
_tag_dyneither(_tmpCFF,sizeof(char),22)));}goto _LL218;}Cyc_Tcutil_unify_list(
_tmp27D,_tmp279);return;}}_LL221: _tmp27E=_tmp26E.f1;if(_tmp27E <= (void*)4)goto
_LL223;if(*((int*)_tmp27E)!= 12)goto _LL223;_tmp27F=((struct Cyc_Absyn_EnumType_struct*)
_tmp27E)->f1;_tmp280=_tmp26E.f2;if(_tmp280 <= (void*)4)goto _LL223;if(*((int*)
_tmp280)!= 12)goto _LL223;_tmp281=((struct Cyc_Absyn_EnumType_struct*)_tmp280)->f1;
_LL222: if(Cyc_Absyn_qvar_cmp(_tmp27F,_tmp281)== 0)return;{const char*_tmpD00;Cyc_Tcutil_failure_reason=((
_tmpD00="(different enum types)",_tag_dyneither(_tmpD00,sizeof(char),23)));}goto
_LL218;_LL223: _tmp282=_tmp26E.f1;if(_tmp282 <= (void*)4)goto _LL225;if(*((int*)
_tmp282)!= 13)goto _LL225;_tmp283=((struct Cyc_Absyn_AnonEnumType_struct*)_tmp282)->f1;
_tmp284=_tmp26E.f2;if(_tmp284 <= (void*)4)goto _LL225;if(*((int*)_tmp284)!= 13)
goto _LL225;_tmp285=((struct Cyc_Absyn_AnonEnumType_struct*)_tmp284)->f1;_LL224: {
int bad=0;for(0;_tmp283 != 0  && _tmp285 != 0;(_tmp283=_tmp283->tl,_tmp285=_tmp285->tl)){
struct Cyc_Absyn_Enumfield*_tmp319=(struct Cyc_Absyn_Enumfield*)_tmp283->hd;struct
Cyc_Absyn_Enumfield*_tmp31A=(struct Cyc_Absyn_Enumfield*)_tmp285->hd;if(Cyc_Absyn_qvar_cmp(
_tmp319->name,_tmp31A->name)!= 0){{const char*_tmpD01;Cyc_Tcutil_failure_reason=((
_tmpD01="(different names for enum fields)",_tag_dyneither(_tmpD01,sizeof(char),
34)));}bad=1;break;}if(_tmp319->tag == _tmp31A->tag)continue;if(_tmp319->tag == 0
 || _tmp31A->tag == 0){{const char*_tmpD02;Cyc_Tcutil_failure_reason=((_tmpD02="(different tag values for enum fields)",
_tag_dyneither(_tmpD02,sizeof(char),39)));}bad=1;break;}if(!Cyc_Evexp_same_const_exp((
struct Cyc_Absyn_Exp*)_check_null(_tmp319->tag),(struct Cyc_Absyn_Exp*)_check_null(
_tmp31A->tag))){{const char*_tmpD03;Cyc_Tcutil_failure_reason=((_tmpD03="(different tag values for enum fields)",
_tag_dyneither(_tmpD03,sizeof(char),39)));}bad=1;break;}}if(bad)goto _LL218;if(
_tmp283 == 0  && _tmp285 == 0)return;{const char*_tmpD04;Cyc_Tcutil_failure_reason=((
_tmpD04="(different number of fields for enums)",_tag_dyneither(_tmpD04,sizeof(
char),39)));}goto _LL218;}_LL225: _tmp286=_tmp26E.f1;if(_tmp286 <= (void*)4)goto
_LL227;if(*((int*)_tmp286)!= 2)goto _LL227;_tmp287=((struct Cyc_Absyn_TunionType_struct*)
_tmp286)->f1;_tmp288=_tmp287.tunion_info;if((((((struct Cyc_Absyn_TunionType_struct*)
_tmp286)->f1).tunion_info).KnownTunion).tag != 1)goto _LL227;_tmp289=(_tmp288.KnownTunion).f1;
_tmp28A=*_tmp289;_tmp28B=_tmp287.targs;_tmp28C=_tmp287.rgn;_tmp28D=_tmp26E.f2;
if(_tmp28D <= (void*)4)goto _LL227;if(*((int*)_tmp28D)!= 2)goto _LL227;_tmp28E=((
struct Cyc_Absyn_TunionType_struct*)_tmp28D)->f1;_tmp28F=_tmp28E.tunion_info;if((((((
struct Cyc_Absyn_TunionType_struct*)_tmp28D)->f1).tunion_info).KnownTunion).tag != 
1)goto _LL227;_tmp290=(_tmp28F.KnownTunion).f1;_tmp291=*_tmp290;_tmp292=_tmp28E.targs;
_tmp293=_tmp28E.rgn;_LL226: if(_tmp28A == _tmp291  || Cyc_Absyn_qvar_cmp(_tmp28A->name,
_tmp291->name)== 0){if(_tmp293 != 0  && _tmp28C != 0)Cyc_Tcutil_unify_it((void*)
_tmp293->v,(void*)_tmp28C->v);else{if(_tmp293 != 0  || _tmp28C != 0)goto Tunion_fail;}
Cyc_Tcutil_unify_list(_tmp292,_tmp28B);return;}Tunion_fail: {const char*_tmpD05;
Cyc_Tcutil_failure_reason=((_tmpD05="(different tunion types)",_tag_dyneither(
_tmpD05,sizeof(char),25)));}goto _LL218;_LL227: _tmp294=_tmp26E.f1;if(_tmp294 <= (
void*)4)goto _LL229;if(*((int*)_tmp294)!= 3)goto _LL229;_tmp295=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp294)->f1;_tmp296=_tmp295.field_info;if((((((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp294)->f1).field_info).KnownTunionfield).tag != 1)goto _LL229;_tmp297=(_tmp296.KnownTunionfield).f1;
_tmp298=(_tmp296.KnownTunionfield).f2;_tmp299=_tmp295.targs;_tmp29A=_tmp26E.f2;
if(_tmp29A <= (void*)4)goto _LL229;if(*((int*)_tmp29A)!= 3)goto _LL229;_tmp29B=((
struct Cyc_Absyn_TunionFieldType_struct*)_tmp29A)->f1;_tmp29C=_tmp29B.field_info;
if((((((struct Cyc_Absyn_TunionFieldType_struct*)_tmp29A)->f1).field_info).KnownTunionfield).tag
!= 1)goto _LL229;_tmp29D=(_tmp29C.KnownTunionfield).f1;_tmp29E=(_tmp29C.KnownTunionfield).f2;
_tmp29F=_tmp29B.targs;_LL228: if((_tmp297 == _tmp29D  || Cyc_Absyn_qvar_cmp(_tmp297->name,
_tmp29D->name)== 0) && (_tmp298 == _tmp29E  || Cyc_Absyn_qvar_cmp(_tmp298->name,
_tmp29E->name)== 0)){Cyc_Tcutil_unify_list(_tmp29F,_tmp299);return;}{const char*
_tmpD06;Cyc_Tcutil_failure_reason=((_tmpD06="(different tunion field types)",
_tag_dyneither(_tmpD06,sizeof(char),31)));}goto _LL218;_LL229: _tmp2A0=_tmp26E.f1;
if(_tmp2A0 <= (void*)4)goto _LL22B;if(*((int*)_tmp2A0)!= 4)goto _LL22B;_tmp2A1=((
struct Cyc_Absyn_PointerType_struct*)_tmp2A0)->f1;_tmp2A2=(void*)_tmp2A1.elt_typ;
_tmp2A3=_tmp2A1.elt_tq;_tmp2A4=_tmp2A1.ptr_atts;_tmp2A5=(void*)_tmp2A4.rgn;
_tmp2A6=_tmp2A4.nullable;_tmp2A7=_tmp2A4.bounds;_tmp2A8=_tmp2A4.zero_term;
_tmp2A9=_tmp26E.f2;if(_tmp2A9 <= (void*)4)goto _LL22B;if(*((int*)_tmp2A9)!= 4)goto
_LL22B;_tmp2AA=((struct Cyc_Absyn_PointerType_struct*)_tmp2A9)->f1;_tmp2AB=(void*)
_tmp2AA.elt_typ;_tmp2AC=_tmp2AA.elt_tq;_tmp2AD=_tmp2AA.ptr_atts;_tmp2AE=(void*)
_tmp2AD.rgn;_tmp2AF=_tmp2AD.nullable;_tmp2B0=_tmp2AD.bounds;_tmp2B1=_tmp2AD.zero_term;
_LL22A: Cyc_Tcutil_unify_it(_tmp2AB,_tmp2A2);Cyc_Tcutil_unify_it(_tmp2A5,_tmp2AE);
Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;{const char*_tmpD07;((void(*)(
int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y,struct
_dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,_tmp2B1,
_tmp2A8,((_tmpD07="(not both zero terminated)",_tag_dyneither(_tmpD07,sizeof(
char),27))));}Cyc_Tcutil_unify_tqual(_tmp2AC,_tmp2AB,_tmp2A3,_tmp2A2);{const char*
_tmpD08;Cyc_Tcutil_unify_it_conrefs(Cyc_Tcutil_unify_it_bounds,_tmp2B0,_tmp2A7,((
_tmpD08="(different pointer bounds)",_tag_dyneither(_tmpD08,sizeof(char),27))));}{
union Cyc_Absyn_Constraint_union _tmp323=(Cyc_Absyn_compress_conref(_tmp2B0))->v;
void*_tmp324;_LL254: if((_tmp323.Eq_constr).tag != 0)goto _LL256;_tmp324=(_tmp323.Eq_constr).f1;
if((int)_tmp324 != 0)goto _LL256;_LL255: return;_LL256:;_LL257: goto _LL253;_LL253:;}{
const char*_tmpD09;((void(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct
Cyc_Absyn_Conref*y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,
_tmp2AF,_tmp2A6,((_tmpD09="(incompatible pointer types)",_tag_dyneither(_tmpD09,
sizeof(char),29))));}return;_LL22B: _tmp2B2=_tmp26E.f1;if(_tmp2B2 <= (void*)4)goto
_LL22D;if(*((int*)_tmp2B2)!= 5)goto _LL22D;_tmp2B3=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp2B2)->f1;_tmp2B4=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp2B2)->f2;
_tmp2B5=_tmp26E.f2;if(_tmp2B5 <= (void*)4)goto _LL22D;if(*((int*)_tmp2B5)!= 5)goto
_LL22D;_tmp2B6=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp2B5)->f1;_tmp2B7=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp2B5)->f2;_LL22C: if(_tmp2B6 == _tmp2B3
 && ((_tmp2B7 == _tmp2B4  || _tmp2B7 == (void*)2  && _tmp2B4 == (void*)3) || _tmp2B7
== (void*)3  && _tmp2B4 == (void*)2))return;{const char*_tmpD0A;Cyc_Tcutil_failure_reason=((
_tmpD0A="(different integral types)",_tag_dyneither(_tmpD0A,sizeof(char),27)));}
goto _LL218;_LL22D: _tmp2B8=_tmp26E.f1;if((int)_tmp2B8 != 1)goto _LL22F;_tmp2B9=
_tmp26E.f2;if((int)_tmp2B9 != 1)goto _LL22F;_LL22E: return;_LL22F: _tmp2BA=_tmp26E.f1;
if(_tmp2BA <= (void*)4)goto _LL231;if(*((int*)_tmp2BA)!= 6)goto _LL231;_tmp2BB=((
struct Cyc_Absyn_DoubleType_struct*)_tmp2BA)->f1;_tmp2BC=_tmp26E.f2;if(_tmp2BC <= (
void*)4)goto _LL231;if(*((int*)_tmp2BC)!= 6)goto _LL231;_tmp2BD=((struct Cyc_Absyn_DoubleType_struct*)
_tmp2BC)->f1;_LL230: if(_tmp2BB == _tmp2BD)return;goto _LL218;_LL231: _tmp2BE=
_tmp26E.f1;if(_tmp2BE <= (void*)4)goto _LL233;if(*((int*)_tmp2BE)!= 18)goto _LL233;
_tmp2BF=(void*)((struct Cyc_Absyn_TagType_struct*)_tmp2BE)->f1;_tmp2C0=_tmp26E.f2;
if(_tmp2C0 <= (void*)4)goto _LL233;if(*((int*)_tmp2C0)!= 18)goto _LL233;_tmp2C1=(
void*)((struct Cyc_Absyn_TagType_struct*)_tmp2C0)->f1;_LL232: Cyc_Tcutil_unify_it(
_tmp2BF,_tmp2C1);return;_LL233: _tmp2C2=_tmp26E.f1;if(_tmp2C2 <= (void*)4)goto
_LL235;if(*((int*)_tmp2C2)!= 17)goto _LL235;_tmp2C3=((struct Cyc_Absyn_ValueofType_struct*)
_tmp2C2)->f1;_tmp2C4=_tmp26E.f2;if(_tmp2C4 <= (void*)4)goto _LL235;if(*((int*)
_tmp2C4)!= 17)goto _LL235;_tmp2C5=((struct Cyc_Absyn_ValueofType_struct*)_tmp2C4)->f1;
_LL234: if(!Cyc_Evexp_same_const_exp(_tmp2C3,_tmp2C5)){{const char*_tmpD0B;Cyc_Tcutil_failure_reason=((
_tmpD0B="(cannot prove expressions are the same)",_tag_dyneither(_tmpD0B,sizeof(
char),40)));}goto _LL218;}return;_LL235: _tmp2C6=_tmp26E.f1;if(_tmp2C6 <= (void*)4)
goto _LL237;if(*((int*)_tmp2C6)!= 7)goto _LL237;_tmp2C7=((struct Cyc_Absyn_ArrayType_struct*)
_tmp2C6)->f1;_tmp2C8=(void*)_tmp2C7.elt_type;_tmp2C9=_tmp2C7.tq;_tmp2CA=_tmp2C7.num_elts;
_tmp2CB=_tmp2C7.zero_term;_tmp2CC=_tmp26E.f2;if(_tmp2CC <= (void*)4)goto _LL237;
if(*((int*)_tmp2CC)!= 7)goto _LL237;_tmp2CD=((struct Cyc_Absyn_ArrayType_struct*)
_tmp2CC)->f1;_tmp2CE=(void*)_tmp2CD.elt_type;_tmp2CF=_tmp2CD.tq;_tmp2D0=_tmp2CD.num_elts;
_tmp2D1=_tmp2CD.zero_term;_LL236: Cyc_Tcutil_unify_it(_tmp2CE,_tmp2C8);Cyc_Tcutil_unify_tqual(
_tmp2CF,_tmp2CE,_tmp2C9,_tmp2C8);Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=
t2;{const char*_tmpD0C;((void(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,
struct Cyc_Absyn_Conref*y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(
Cyc_Core_intcmp,_tmp2CB,_tmp2D1,((_tmpD0C="(not both zero terminated)",
_tag_dyneither(_tmpD0C,sizeof(char),27))));}if(_tmp2CA == _tmp2D0)return;if(
_tmp2CA == 0  || _tmp2D0 == 0)goto _LL218;if(Cyc_Evexp_same_const_exp((struct Cyc_Absyn_Exp*)
_tmp2CA,(struct Cyc_Absyn_Exp*)_tmp2D0))return;{const char*_tmpD0D;Cyc_Tcutil_failure_reason=((
_tmpD0D="(different array sizes)",_tag_dyneither(_tmpD0D,sizeof(char),24)));}
goto _LL218;_LL237: _tmp2D2=_tmp26E.f1;if(_tmp2D2 <= (void*)4)goto _LL239;if(*((int*)
_tmp2D2)!= 8)goto _LL239;_tmp2D3=((struct Cyc_Absyn_FnType_struct*)_tmp2D2)->f1;
_tmp2D4=_tmp2D3.tvars;_tmp2D5=_tmp2D3.effect;_tmp2D6=(void*)_tmp2D3.ret_typ;
_tmp2D7=_tmp2D3.args;_tmp2D8=_tmp2D3.c_varargs;_tmp2D9=_tmp2D3.cyc_varargs;
_tmp2DA=_tmp2D3.rgn_po;_tmp2DB=_tmp2D3.attributes;_tmp2DC=_tmp26E.f2;if(_tmp2DC
<= (void*)4)goto _LL239;if(*((int*)_tmp2DC)!= 8)goto _LL239;_tmp2DD=((struct Cyc_Absyn_FnType_struct*)
_tmp2DC)->f1;_tmp2DE=_tmp2DD.tvars;_tmp2DF=_tmp2DD.effect;_tmp2E0=(void*)_tmp2DD.ret_typ;
_tmp2E1=_tmp2DD.args;_tmp2E2=_tmp2DD.c_varargs;_tmp2E3=_tmp2DD.cyc_varargs;
_tmp2E4=_tmp2DD.rgn_po;_tmp2E5=_tmp2DD.attributes;_LL238: {int done=0;{struct
_RegionHandle _tmp32A=_new_region("rgn");struct _RegionHandle*rgn=& _tmp32A;
_push_region(rgn);{struct Cyc_List_List*inst=0;while(_tmp2DE != 0){if(_tmp2D4 == 0){{
const char*_tmpD0E;Cyc_Tcutil_failure_reason=((_tmpD0E="(second function type has too few type variables)",
_tag_dyneither(_tmpD0E,sizeof(char),50)));}(int)_throw((void*)Cyc_Tcutil_Unify);}{
void*_tmp32C=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp2DE->hd);void*
_tmp32D=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp2D4->hd);if(_tmp32C != 
_tmp32D){{const char*_tmpD14;void*_tmpD13[3];struct Cyc_String_pa_struct _tmpD12;
struct Cyc_String_pa_struct _tmpD11;struct Cyc_String_pa_struct _tmpD10;Cyc_Tcutil_failure_reason=(
struct _dyneither_ptr)((_tmpD10.tag=0,((_tmpD10.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_kind2string(_tmp32D)),((_tmpD11.tag=0,((_tmpD11.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(_tmp32C)),((
_tmpD12.tag=0,((_tmpD12.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_tvar2string((
struct Cyc_Absyn_Tvar*)_tmp2DE->hd)),((_tmpD13[0]=& _tmpD12,((_tmpD13[1]=& _tmpD11,((
_tmpD13[2]=& _tmpD10,Cyc_aprintf(((_tmpD14="(type var %s has different kinds %s and %s)",
_tag_dyneither(_tmpD14,sizeof(char),44))),_tag_dyneither(_tmpD13,sizeof(void*),3))))))))))))))))))));}(
int)_throw((void*)Cyc_Tcutil_Unify);}{struct _tuple8*_tmpD1E;struct Cyc_Absyn_VarType_struct
_tmpD1D;struct Cyc_Absyn_VarType_struct*_tmpD1C;struct Cyc_List_List*_tmpD1B;inst=((
_tmpD1B=_region_malloc(rgn,sizeof(*_tmpD1B)),((_tmpD1B->hd=((_tmpD1E=
_region_malloc(rgn,sizeof(*_tmpD1E)),((_tmpD1E->f1=(struct Cyc_Absyn_Tvar*)
_tmp2D4->hd,((_tmpD1E->f2=(void*)((_tmpD1C=_cycalloc(sizeof(*_tmpD1C)),((_tmpD1C[
0]=((_tmpD1D.tag=1,((_tmpD1D.f1=(struct Cyc_Absyn_Tvar*)_tmp2DE->hd,_tmpD1D)))),
_tmpD1C)))),_tmpD1E)))))),((_tmpD1B->tl=inst,_tmpD1B))))));}_tmp2DE=_tmp2DE->tl;
_tmp2D4=_tmp2D4->tl;}}if(_tmp2D4 != 0){{const char*_tmpD1F;Cyc_Tcutil_failure_reason=((
_tmpD1F="(second function type has too many type variables)",_tag_dyneither(
_tmpD1F,sizeof(char),51)));}_npop_handler(0);goto _LL218;}if(inst != 0){{struct Cyc_Absyn_FnType_struct
_tmpD2B;struct Cyc_Absyn_FnInfo _tmpD2A;struct Cyc_Absyn_FnType_struct*_tmpD29;
struct Cyc_Absyn_FnType_struct _tmpD25;struct Cyc_Absyn_FnInfo _tmpD24;struct Cyc_Absyn_FnType_struct*
_tmpD23;Cyc_Tcutil_unify_it((void*)((_tmpD23=_cycalloc(sizeof(*_tmpD23)),((
_tmpD23[0]=((_tmpD25.tag=8,((_tmpD25.f1=((_tmpD24.tvars=0,((_tmpD24.effect=
_tmp2DF,((_tmpD24.ret_typ=(void*)_tmp2E0,((_tmpD24.args=_tmp2E1,((_tmpD24.c_varargs=
_tmp2E2,((_tmpD24.cyc_varargs=_tmp2E3,((_tmpD24.rgn_po=_tmp2E4,((_tmpD24.attributes=
_tmp2E5,_tmpD24)))))))))))))))),_tmpD25)))),_tmpD23)))),Cyc_Tcutil_rsubstitute(
rgn,inst,(void*)((_tmpD29=_cycalloc(sizeof(*_tmpD29)),((_tmpD29[0]=((_tmpD2B.tag=
8,((_tmpD2B.f1=((_tmpD2A.tvars=0,((_tmpD2A.effect=_tmp2D5,((_tmpD2A.ret_typ=(
void*)_tmp2D6,((_tmpD2A.args=_tmp2D7,((_tmpD2A.c_varargs=_tmp2D8,((_tmpD2A.cyc_varargs=
_tmp2D9,((_tmpD2A.rgn_po=_tmp2DA,((_tmpD2A.attributes=_tmp2DB,_tmpD2A)))))))))))))))),
_tmpD2B)))),_tmpD29))))));}done=1;}};_pop_region(rgn);}if(done)return;Cyc_Tcutil_unify_it(
_tmp2E0,_tmp2D6);for(0;_tmp2E1 != 0  && _tmp2D7 != 0;(_tmp2E1=_tmp2E1->tl,_tmp2D7=
_tmp2D7->tl)){struct Cyc_Absyn_Tqual _tmp33F;void*_tmp340;struct _tuple3 _tmp33E=*((
struct _tuple3*)_tmp2E1->hd);_tmp33F=_tmp33E.f2;_tmp340=_tmp33E.f3;{struct Cyc_Absyn_Tqual
_tmp342;void*_tmp343;struct _tuple3 _tmp341=*((struct _tuple3*)_tmp2D7->hd);_tmp342=
_tmp341.f2;_tmp343=_tmp341.f3;Cyc_Tcutil_unify_it(_tmp340,_tmp343);Cyc_Tcutil_unify_tqual(
_tmp33F,_tmp340,_tmp342,_tmp343);}}Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=
t2;if(_tmp2E1 != 0  || _tmp2D7 != 0){{const char*_tmpD2C;Cyc_Tcutil_failure_reason=((
_tmpD2C="(function types have different number of arguments)",_tag_dyneither(
_tmpD2C,sizeof(char),52)));}goto _LL218;}if(_tmp2E2 != _tmp2D8){{const char*_tmpD2D;
Cyc_Tcutil_failure_reason=((_tmpD2D="(only one function type takes C varargs)",
_tag_dyneither(_tmpD2D,sizeof(char),41)));}goto _LL218;}{int bad_cyc_vararg=0;{
struct _tuple11 _tmpD2E;struct _tuple11 _tmp347=(_tmpD2E.f1=_tmp2E3,((_tmpD2E.f2=
_tmp2D9,_tmpD2E)));struct Cyc_Absyn_VarargInfo*_tmp348;struct Cyc_Absyn_VarargInfo*
_tmp349;struct Cyc_Absyn_VarargInfo*_tmp34A;struct Cyc_Absyn_VarargInfo*_tmp34B;
struct Cyc_Absyn_VarargInfo*_tmp34C;struct Cyc_Absyn_VarargInfo _tmp34D;struct Cyc_Core_Opt*
_tmp34E;struct Cyc_Absyn_Tqual _tmp34F;void*_tmp350;int _tmp351;struct Cyc_Absyn_VarargInfo*
_tmp352;struct Cyc_Absyn_VarargInfo _tmp353;struct Cyc_Core_Opt*_tmp354;struct Cyc_Absyn_Tqual
_tmp355;void*_tmp356;int _tmp357;_LL259: _tmp348=_tmp347.f1;if(_tmp348 != 0)goto
_LL25B;_tmp349=_tmp347.f2;if(_tmp349 != 0)goto _LL25B;_LL25A: goto _LL258;_LL25B:
_tmp34A=_tmp347.f1;if(_tmp34A != 0)goto _LL25D;_LL25C: goto _LL25E;_LL25D: _tmp34B=
_tmp347.f2;if(_tmp34B != 0)goto _LL25F;_LL25E: bad_cyc_vararg=1;{const char*_tmpD2F;
Cyc_Tcutil_failure_reason=((_tmpD2F="(only one function type takes varargs)",
_tag_dyneither(_tmpD2F,sizeof(char),39)));}goto _LL258;_LL25F: _tmp34C=_tmp347.f1;
if(_tmp34C == 0)goto _LL258;_tmp34D=*_tmp34C;_tmp34E=_tmp34D.name;_tmp34F=_tmp34D.tq;
_tmp350=(void*)_tmp34D.type;_tmp351=_tmp34D.inject;_tmp352=_tmp347.f2;if(_tmp352
== 0)goto _LL258;_tmp353=*_tmp352;_tmp354=_tmp353.name;_tmp355=_tmp353.tq;_tmp356=(
void*)_tmp353.type;_tmp357=_tmp353.inject;_LL260: Cyc_Tcutil_unify_it(_tmp350,
_tmp356);Cyc_Tcutil_unify_tqual(_tmp34F,_tmp350,_tmp355,_tmp356);if(_tmp351 != 
_tmp357){bad_cyc_vararg=1;{const char*_tmpD30;Cyc_Tcutil_failure_reason=((_tmpD30="(only one function type injects varargs)",
_tag_dyneither(_tmpD30,sizeof(char),41)));}}goto _LL258;_LL258:;}if(
bad_cyc_vararg)goto _LL218;{int bad_effect=0;{struct _tuple12 _tmpD31;struct _tuple12
_tmp35B=(_tmpD31.f1=_tmp2DF,((_tmpD31.f2=_tmp2D5,_tmpD31)));struct Cyc_Core_Opt*
_tmp35C;struct Cyc_Core_Opt*_tmp35D;struct Cyc_Core_Opt*_tmp35E;struct Cyc_Core_Opt*
_tmp35F;_LL262: _tmp35C=_tmp35B.f1;if(_tmp35C != 0)goto _LL264;_tmp35D=_tmp35B.f2;
if(_tmp35D != 0)goto _LL264;_LL263: goto _LL261;_LL264: _tmp35E=_tmp35B.f1;if(_tmp35E
!= 0)goto _LL266;_LL265: goto _LL267;_LL266: _tmp35F=_tmp35B.f2;if(_tmp35F != 0)goto
_LL268;_LL267: bad_effect=1;goto _LL261;_LL268:;_LL269: bad_effect=!Cyc_Tcutil_unify_effect((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp2DF))->v,(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp2D5))->v);goto _LL261;_LL261:;}Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=
t2;if(bad_effect){{const char*_tmpD32;Cyc_Tcutil_failure_reason=((_tmpD32="(function type effects do not unify)",
_tag_dyneither(_tmpD32,sizeof(char),37)));}goto _LL218;}if(!Cyc_Tcutil_same_atts(
_tmp2DB,_tmp2E5)){{const char*_tmpD33;Cyc_Tcutil_failure_reason=((_tmpD33="(function types have different attributes)",
_tag_dyneither(_tmpD33,sizeof(char),43)));}goto _LL218;}if(!Cyc_Tcutil_same_rgn_po(
_tmp2DA,_tmp2E4)){{const char*_tmpD34;Cyc_Tcutil_failure_reason=((_tmpD34="(function types have different region lifetime orderings)",
_tag_dyneither(_tmpD34,sizeof(char),58)));}goto _LL218;}return;}}}_LL239: _tmp2E6=
_tmp26E.f1;if(_tmp2E6 <= (void*)4)goto _LL23B;if(*((int*)_tmp2E6)!= 9)goto _LL23B;
_tmp2E7=((struct Cyc_Absyn_TupleType_struct*)_tmp2E6)->f1;_tmp2E8=_tmp26E.f2;if(
_tmp2E8 <= (void*)4)goto _LL23B;if(*((int*)_tmp2E8)!= 9)goto _LL23B;_tmp2E9=((
struct Cyc_Absyn_TupleType_struct*)_tmp2E8)->f1;_LL23A: for(0;_tmp2E9 != 0  && 
_tmp2E7 != 0;(_tmp2E9=_tmp2E9->tl,_tmp2E7=_tmp2E7->tl)){struct Cyc_Absyn_Tqual
_tmp364;void*_tmp365;struct _tuple5 _tmp363=*((struct _tuple5*)_tmp2E9->hd);_tmp364=
_tmp363.f1;_tmp365=_tmp363.f2;{struct Cyc_Absyn_Tqual _tmp367;void*_tmp368;struct
_tuple5 _tmp366=*((struct _tuple5*)_tmp2E7->hd);_tmp367=_tmp366.f1;_tmp368=_tmp366.f2;
Cyc_Tcutil_unify_it(_tmp365,_tmp368);Cyc_Tcutil_unify_tqual(_tmp364,_tmp365,
_tmp367,_tmp368);}}if(_tmp2E9 == 0  && _tmp2E7 == 0)return;Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;{const char*_tmpD35;Cyc_Tcutil_failure_reason=((_tmpD35="(tuple types have different numbers of components)",
_tag_dyneither(_tmpD35,sizeof(char),51)));}goto _LL218;_LL23B: _tmp2EA=_tmp26E.f1;
if(_tmp2EA <= (void*)4)goto _LL23D;if(*((int*)_tmp2EA)!= 11)goto _LL23D;_tmp2EB=(
void*)((struct Cyc_Absyn_AnonAggrType_struct*)_tmp2EA)->f1;_tmp2EC=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp2EA)->f2;_tmp2ED=_tmp26E.f2;if(_tmp2ED <= (void*)4)goto _LL23D;if(*((int*)
_tmp2ED)!= 11)goto _LL23D;_tmp2EE=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp2ED)->f1;_tmp2EF=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp2ED)->f2;_LL23C:
if(_tmp2EE != _tmp2EB){{const char*_tmpD36;Cyc_Tcutil_failure_reason=((_tmpD36="(struct and union type)",
_tag_dyneither(_tmpD36,sizeof(char),24)));}goto _LL218;}for(0;_tmp2EF != 0  && 
_tmp2EC != 0;(_tmp2EF=_tmp2EF->tl,_tmp2EC=_tmp2EC->tl)){struct Cyc_Absyn_Aggrfield*
_tmp36B=(struct Cyc_Absyn_Aggrfield*)_tmp2EF->hd;struct Cyc_Absyn_Aggrfield*
_tmp36C=(struct Cyc_Absyn_Aggrfield*)_tmp2EC->hd;if(Cyc_strptrcmp(_tmp36B->name,
_tmp36C->name)!= 0){{const char*_tmpD37;Cyc_Tcutil_failure_reason=((_tmpD37="(different member names)",
_tag_dyneither(_tmpD37,sizeof(char),25)));}(int)_throw((void*)Cyc_Tcutil_Unify);}
Cyc_Tcutil_unify_it((void*)_tmp36B->type,(void*)_tmp36C->type);Cyc_Tcutil_unify_tqual(
_tmp36B->tq,(void*)_tmp36B->type,_tmp36C->tq,(void*)_tmp36C->type);if(!Cyc_Tcutil_same_atts(
_tmp36B->attributes,_tmp36C->attributes)){Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=
t2;{const char*_tmpD38;Cyc_Tcutil_failure_reason=((_tmpD38="(different attributes on member)",
_tag_dyneither(_tmpD38,sizeof(char),33)));}(int)_throw((void*)Cyc_Tcutil_Unify);}
if((_tmp36B->width != 0  && _tmp36C->width == 0  || _tmp36C->width != 0  && _tmp36B->width
== 0) || (_tmp36B->width != 0  && _tmp36C->width != 0) && !Cyc_Evexp_same_const_exp((
struct Cyc_Absyn_Exp*)_check_null(_tmp36B->width),(struct Cyc_Absyn_Exp*)
_check_null(_tmp36C->width))){Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;{
const char*_tmpD39;Cyc_Tcutil_failure_reason=((_tmpD39="(different bitfield widths on member)",
_tag_dyneither(_tmpD39,sizeof(char),38)));}(int)_throw((void*)Cyc_Tcutil_Unify);}}
if(_tmp2EF == 0  && _tmp2EC == 0)return;Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=
t2;{const char*_tmpD3A;Cyc_Tcutil_failure_reason=((_tmpD3A="(different number of members)",
_tag_dyneither(_tmpD3A,sizeof(char),30)));}goto _LL218;_LL23D: _tmp2F0=_tmp26E.f1;
if((int)_tmp2F0 != 2)goto _LL23F;_tmp2F1=_tmp26E.f2;if((int)_tmp2F1 != 2)goto _LL23F;
_LL23E: return;_LL23F: _tmp2F2=_tmp26E.f1;if((int)_tmp2F2 != 3)goto _LL241;_tmp2F3=
_tmp26E.f2;if((int)_tmp2F3 != 3)goto _LL241;_LL240: return;_LL241: _tmp2F4=_tmp26E.f1;
if(_tmp2F4 <= (void*)4)goto _LL243;if(*((int*)_tmp2F4)!= 14)goto _LL243;_tmp2F5=(
void*)((struct Cyc_Absyn_RgnHandleType_struct*)_tmp2F4)->f1;_tmp2F6=_tmp26E.f2;
if(_tmp2F6 <= (void*)4)goto _LL243;if(*((int*)_tmp2F6)!= 14)goto _LL243;_tmp2F7=(
void*)((struct Cyc_Absyn_RgnHandleType_struct*)_tmp2F6)->f1;_LL242: Cyc_Tcutil_unify_it(
_tmp2F5,_tmp2F7);return;_LL243: _tmp2F8=_tmp26E.f1;if(_tmp2F8 <= (void*)4)goto
_LL245;if(*((int*)_tmp2F8)!= 15)goto _LL245;_tmp2F9=(void*)((struct Cyc_Absyn_DynRgnType_struct*)
_tmp2F8)->f1;_tmp2FA=(void*)((struct Cyc_Absyn_DynRgnType_struct*)_tmp2F8)->f2;
_tmp2FB=_tmp26E.f2;if(_tmp2FB <= (void*)4)goto _LL245;if(*((int*)_tmp2FB)!= 15)
goto _LL245;_tmp2FC=(void*)((struct Cyc_Absyn_DynRgnType_struct*)_tmp2FB)->f1;
_tmp2FD=(void*)((struct Cyc_Absyn_DynRgnType_struct*)_tmp2FB)->f2;_LL244: Cyc_Tcutil_unify_it(
_tmp2F9,_tmp2FC);Cyc_Tcutil_unify_it(_tmp2FA,_tmp2FD);return;_LL245: _tmp2FE=
_tmp26E.f1;if(_tmp2FE <= (void*)4)goto _LL247;if(*((int*)_tmp2FE)!= 20)goto _LL247;
_LL246: goto _LL248;_LL247: _tmp2FF=_tmp26E.f2;if(_tmp2FF <= (void*)4)goto _LL249;if(*((
int*)_tmp2FF)!= 20)goto _LL249;_LL248: goto _LL24A;_LL249: _tmp300=_tmp26E.f1;if(
_tmp300 <= (void*)4)goto _LL24B;if(*((int*)_tmp300)!= 19)goto _LL24B;_LL24A: goto
_LL24C;_LL24B: _tmp301=_tmp26E.f1;if(_tmp301 <= (void*)4)goto _LL24D;if(*((int*)
_tmp301)!= 21)goto _LL24D;_LL24C: goto _LL24E;_LL24D: _tmp302=_tmp26E.f2;if(_tmp302
<= (void*)4)goto _LL24F;if(*((int*)_tmp302)!= 21)goto _LL24F;_LL24E: goto _LL250;
_LL24F: _tmp303=_tmp26E.f2;if(_tmp303 <= (void*)4)goto _LL251;if(*((int*)_tmp303)!= 
19)goto _LL251;_LL250: if(Cyc_Tcutil_unify_effect(t1,t2))return;{const char*_tmpD3B;
Cyc_Tcutil_failure_reason=((_tmpD3B="(effects don't unify)",_tag_dyneither(
_tmpD3B,sizeof(char),22)));}goto _LL218;_LL251:;_LL252: goto _LL218;_LL218:;}(int)
_throw((void*)Cyc_Tcutil_Unify);}int Cyc_Tcutil_star_cmp(int(*cmp)(void*,void*),
void*a1,void*a2);int Cyc_Tcutil_star_cmp(int(*cmp)(void*,void*),void*a1,void*a2){
if(a1 == a2)return 0;if(a1 == 0  && a2 != 0)return - 1;if(a1 != 0  && a2 == 0)return 1;
return cmp((void*)_check_null(a1),(void*)_check_null(a2));}static int Cyc_Tcutil_tqual_cmp(
struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2);static int Cyc_Tcutil_tqual_cmp(
struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2){int _tmp372=(tq1.real_const + (
tq1.q_volatile << 1))+ (tq1.q_restrict << 2);int _tmp373=(tq2.real_const + (tq2.q_volatile
<< 1))+ (tq2.q_restrict << 2);return Cyc_Core_intcmp(_tmp372,_tmp373);}static int
Cyc_Tcutil_conrefs_cmp(int(*cmp)(void*,void*),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*
y);static int Cyc_Tcutil_conrefs_cmp(int(*cmp)(void*,void*),struct Cyc_Absyn_Conref*
x,struct Cyc_Absyn_Conref*y){x=Cyc_Absyn_compress_conref(x);y=Cyc_Absyn_compress_conref(
y);if(x == y)return 0;{union Cyc_Absyn_Constraint_union _tmp374=x->v;void*_tmp375;
_LL26B: if((_tmp374.No_constr).tag != 2)goto _LL26D;_LL26C: return - 1;_LL26D: if((
_tmp374.Eq_constr).tag != 0)goto _LL26F;_tmp375=(_tmp374.Eq_constr).f1;_LL26E: {
union Cyc_Absyn_Constraint_union _tmp376=y->v;void*_tmp377;_LL272: if((_tmp376.No_constr).tag
!= 2)goto _LL274;_LL273: return 1;_LL274: if((_tmp376.Eq_constr).tag != 0)goto _LL276;
_tmp377=(_tmp376.Eq_constr).f1;_LL275: return cmp(_tmp375,_tmp377);_LL276: if((
_tmp376.Forward_constr).tag != 1)goto _LL271;_LL277: {const char*_tmpD3E;void*
_tmpD3D;(_tmpD3D=0,Cyc_Tcutil_impos(((_tmpD3E="unify_conref: forward after compress(2)",
_tag_dyneither(_tmpD3E,sizeof(char),40))),_tag_dyneither(_tmpD3D,sizeof(void*),0)));}
_LL271:;}_LL26F: if((_tmp374.Forward_constr).tag != 1)goto _LL26A;_LL270: {const
char*_tmpD41;void*_tmpD40;(_tmpD40=0,Cyc_Tcutil_impos(((_tmpD41="unify_conref: forward after compress",
_tag_dyneither(_tmpD41,sizeof(char),37))),_tag_dyneither(_tmpD40,sizeof(void*),0)));}
_LL26A:;}}static int Cyc_Tcutil_tqual_type_cmp(struct _tuple5*tqt1,struct _tuple5*
tqt2);static int Cyc_Tcutil_tqual_type_cmp(struct _tuple5*tqt1,struct _tuple5*tqt2){
struct _tuple5 _tmp37D;struct Cyc_Absyn_Tqual _tmp37E;void*_tmp37F;struct _tuple5*
_tmp37C=tqt1;_tmp37D=*_tmp37C;_tmp37E=_tmp37D.f1;_tmp37F=_tmp37D.f2;{struct
_tuple5 _tmp381;struct Cyc_Absyn_Tqual _tmp382;void*_tmp383;struct _tuple5*_tmp380=
tqt2;_tmp381=*_tmp380;_tmp382=_tmp381.f1;_tmp383=_tmp381.f2;{int _tmp384=Cyc_Tcutil_tqual_cmp(
_tmp37E,_tmp382);if(_tmp384 != 0)return _tmp384;return Cyc_Tcutil_typecmp(_tmp37F,
_tmp383);}}}static int Cyc_Tcutil_aggrfield_cmp(struct Cyc_Absyn_Aggrfield*f1,
struct Cyc_Absyn_Aggrfield*f2);static int Cyc_Tcutil_aggrfield_cmp(struct Cyc_Absyn_Aggrfield*
f1,struct Cyc_Absyn_Aggrfield*f2){int _tmp385=Cyc_strptrcmp(f1->name,f2->name);if(
_tmp385 != 0)return _tmp385;{int _tmp386=Cyc_Tcutil_tqual_cmp(f1->tq,f2->tq);if(
_tmp386 != 0)return _tmp386;{int _tmp387=Cyc_Tcutil_typecmp((void*)f1->type,(void*)
f2->type);if(_tmp387 != 0)return _tmp387;{int _tmp388=Cyc_List_list_cmp(Cyc_Tcutil_attribute_cmp,
f1->attributes,f2->attributes);if(_tmp388 != 0)return _tmp388;return((int(*)(int(*
cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*
a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,f1->width,f2->width);}}}}static
int Cyc_Tcutil_enumfield_cmp(struct Cyc_Absyn_Enumfield*e1,struct Cyc_Absyn_Enumfield*
e2);static int Cyc_Tcutil_enumfield_cmp(struct Cyc_Absyn_Enumfield*e1,struct Cyc_Absyn_Enumfield*
e2){int _tmp389=Cyc_Absyn_qvar_cmp(e1->name,e2->name);if(_tmp389 != 0)return
_tmp389;return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),
struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,
e1->tag,e2->tag);}static int Cyc_Tcutil_type_case_number(void*t);static int Cyc_Tcutil_type_case_number(
void*t){void*_tmp38A=t;_LL279: if((int)_tmp38A != 0)goto _LL27B;_LL27A: return 0;
_LL27B: if(_tmp38A <= (void*)4)goto _LL287;if(*((int*)_tmp38A)!= 0)goto _LL27D;
_LL27C: return 1;_LL27D: if(*((int*)_tmp38A)!= 1)goto _LL27F;_LL27E: return 2;_LL27F:
if(*((int*)_tmp38A)!= 2)goto _LL281;_LL280: return 3;_LL281: if(*((int*)_tmp38A)!= 3)
goto _LL283;_LL282: return 4;_LL283: if(*((int*)_tmp38A)!= 4)goto _LL285;_LL284:
return 5;_LL285: if(*((int*)_tmp38A)!= 5)goto _LL287;_LL286: return 6;_LL287: if((int)
_tmp38A != 1)goto _LL289;_LL288: return 7;_LL289: if(_tmp38A <= (void*)4)goto _LL29D;
if(*((int*)_tmp38A)!= 6)goto _LL28B;_LL28A: return 8;_LL28B: if(*((int*)_tmp38A)!= 7)
goto _LL28D;_LL28C: return 9;_LL28D: if(*((int*)_tmp38A)!= 8)goto _LL28F;_LL28E:
return 10;_LL28F: if(*((int*)_tmp38A)!= 9)goto _LL291;_LL290: return 11;_LL291: if(*((
int*)_tmp38A)!= 10)goto _LL293;_LL292: return 12;_LL293: if(*((int*)_tmp38A)!= 11)
goto _LL295;_LL294: return 14;_LL295: if(*((int*)_tmp38A)!= 12)goto _LL297;_LL296:
return 16;_LL297: if(*((int*)_tmp38A)!= 13)goto _LL299;_LL298: return 17;_LL299: if(*((
int*)_tmp38A)!= 14)goto _LL29B;_LL29A: return 18;_LL29B: if(*((int*)_tmp38A)!= 16)
goto _LL29D;_LL29C: return 19;_LL29D: if((int)_tmp38A != 3)goto _LL29F;_LL29E: return 20;
_LL29F: if((int)_tmp38A != 2)goto _LL2A1;_LL2A0: return 21;_LL2A1: if(_tmp38A <= (void*)
4)goto _LL2A3;if(*((int*)_tmp38A)!= 19)goto _LL2A3;_LL2A2: return 22;_LL2A3: if(
_tmp38A <= (void*)4)goto _LL2A5;if(*((int*)_tmp38A)!= 20)goto _LL2A5;_LL2A4: return
23;_LL2A5: if(_tmp38A <= (void*)4)goto _LL2A7;if(*((int*)_tmp38A)!= 21)goto _LL2A7;
_LL2A6: return 24;_LL2A7: if(_tmp38A <= (void*)4)goto _LL2A9;if(*((int*)_tmp38A)!= 18)
goto _LL2A9;_LL2A8: return 26;_LL2A9: if(_tmp38A <= (void*)4)goto _LL2AB;if(*((int*)
_tmp38A)!= 15)goto _LL2AB;_LL2AA: return 27;_LL2AB: if(_tmp38A <= (void*)4)goto _LL278;
if(*((int*)_tmp38A)!= 17)goto _LL278;_LL2AC: return 28;_LL278:;}int Cyc_Tcutil_typecmp(
void*t1,void*t2);int Cyc_Tcutil_typecmp(void*t1,void*t2){t1=Cyc_Tcutil_compress(
t1);t2=Cyc_Tcutil_compress(t2);if(t1 == t2)return 0;{int _tmp38B=Cyc_Core_intcmp(
Cyc_Tcutil_type_case_number(t1),Cyc_Tcutil_type_case_number(t2));if(_tmp38B != 0)
return _tmp38B;{struct _tuple0 _tmpD42;struct _tuple0 _tmp38D=(_tmpD42.f1=t2,((
_tmpD42.f2=t1,_tmpD42)));void*_tmp38E;void*_tmp38F;void*_tmp390;struct Cyc_Absyn_Tvar*
_tmp391;void*_tmp392;struct Cyc_Absyn_Tvar*_tmp393;void*_tmp394;struct Cyc_Absyn_AggrInfo
_tmp395;union Cyc_Absyn_AggrInfoU_union _tmp396;struct Cyc_List_List*_tmp397;void*
_tmp398;struct Cyc_Absyn_AggrInfo _tmp399;union Cyc_Absyn_AggrInfoU_union _tmp39A;
struct Cyc_List_List*_tmp39B;void*_tmp39C;struct _tuple2*_tmp39D;void*_tmp39E;
struct _tuple2*_tmp39F;void*_tmp3A0;struct Cyc_List_List*_tmp3A1;void*_tmp3A2;
struct Cyc_List_List*_tmp3A3;void*_tmp3A4;struct Cyc_Absyn_TunionInfo _tmp3A5;union
Cyc_Absyn_TunionInfoU_union _tmp3A6;struct Cyc_Absyn_Tuniondecl**_tmp3A7;struct Cyc_Absyn_Tuniondecl*
_tmp3A8;struct Cyc_List_List*_tmp3A9;struct Cyc_Core_Opt*_tmp3AA;void*_tmp3AB;
struct Cyc_Absyn_TunionInfo _tmp3AC;union Cyc_Absyn_TunionInfoU_union _tmp3AD;struct
Cyc_Absyn_Tuniondecl**_tmp3AE;struct Cyc_Absyn_Tuniondecl*_tmp3AF;struct Cyc_List_List*
_tmp3B0;struct Cyc_Core_Opt*_tmp3B1;void*_tmp3B2;struct Cyc_Absyn_TunionFieldInfo
_tmp3B3;union Cyc_Absyn_TunionFieldInfoU_union _tmp3B4;struct Cyc_Absyn_Tuniondecl*
_tmp3B5;struct Cyc_Absyn_Tunionfield*_tmp3B6;struct Cyc_List_List*_tmp3B7;void*
_tmp3B8;struct Cyc_Absyn_TunionFieldInfo _tmp3B9;union Cyc_Absyn_TunionFieldInfoU_union
_tmp3BA;struct Cyc_Absyn_Tuniondecl*_tmp3BB;struct Cyc_Absyn_Tunionfield*_tmp3BC;
struct Cyc_List_List*_tmp3BD;void*_tmp3BE;struct Cyc_Absyn_PtrInfo _tmp3BF;void*
_tmp3C0;struct Cyc_Absyn_Tqual _tmp3C1;struct Cyc_Absyn_PtrAtts _tmp3C2;void*_tmp3C3;
struct Cyc_Absyn_Conref*_tmp3C4;struct Cyc_Absyn_Conref*_tmp3C5;struct Cyc_Absyn_Conref*
_tmp3C6;void*_tmp3C7;struct Cyc_Absyn_PtrInfo _tmp3C8;void*_tmp3C9;struct Cyc_Absyn_Tqual
_tmp3CA;struct Cyc_Absyn_PtrAtts _tmp3CB;void*_tmp3CC;struct Cyc_Absyn_Conref*
_tmp3CD;struct Cyc_Absyn_Conref*_tmp3CE;struct Cyc_Absyn_Conref*_tmp3CF;void*
_tmp3D0;void*_tmp3D1;void*_tmp3D2;void*_tmp3D3;void*_tmp3D4;void*_tmp3D5;void*
_tmp3D6;void*_tmp3D7;void*_tmp3D8;int _tmp3D9;void*_tmp3DA;int _tmp3DB;void*
_tmp3DC;struct Cyc_Absyn_ArrayInfo _tmp3DD;void*_tmp3DE;struct Cyc_Absyn_Tqual
_tmp3DF;struct Cyc_Absyn_Exp*_tmp3E0;struct Cyc_Absyn_Conref*_tmp3E1;void*_tmp3E2;
struct Cyc_Absyn_ArrayInfo _tmp3E3;void*_tmp3E4;struct Cyc_Absyn_Tqual _tmp3E5;
struct Cyc_Absyn_Exp*_tmp3E6;struct Cyc_Absyn_Conref*_tmp3E7;void*_tmp3E8;struct
Cyc_Absyn_FnInfo _tmp3E9;struct Cyc_List_List*_tmp3EA;struct Cyc_Core_Opt*_tmp3EB;
void*_tmp3EC;struct Cyc_List_List*_tmp3ED;int _tmp3EE;struct Cyc_Absyn_VarargInfo*
_tmp3EF;struct Cyc_List_List*_tmp3F0;struct Cyc_List_List*_tmp3F1;void*_tmp3F2;
struct Cyc_Absyn_FnInfo _tmp3F3;struct Cyc_List_List*_tmp3F4;struct Cyc_Core_Opt*
_tmp3F5;void*_tmp3F6;struct Cyc_List_List*_tmp3F7;int _tmp3F8;struct Cyc_Absyn_VarargInfo*
_tmp3F9;struct Cyc_List_List*_tmp3FA;struct Cyc_List_List*_tmp3FB;void*_tmp3FC;
struct Cyc_List_List*_tmp3FD;void*_tmp3FE;struct Cyc_List_List*_tmp3FF;void*
_tmp400;void*_tmp401;struct Cyc_List_List*_tmp402;void*_tmp403;void*_tmp404;
struct Cyc_List_List*_tmp405;void*_tmp406;void*_tmp407;void*_tmp408;void*_tmp409;
void*_tmp40A;void*_tmp40B;void*_tmp40C;void*_tmp40D;void*_tmp40E;void*_tmp40F;
void*_tmp410;void*_tmp411;void*_tmp412;void*_tmp413;void*_tmp414;struct Cyc_Absyn_Exp*
_tmp415;void*_tmp416;struct Cyc_Absyn_Exp*_tmp417;void*_tmp418;void*_tmp419;void*
_tmp41A;void*_tmp41B;void*_tmp41C;void*_tmp41D;_LL2AE: _tmp38E=_tmp38D.f1;if(
_tmp38E <= (void*)4)goto _LL2B0;if(*((int*)_tmp38E)!= 0)goto _LL2B0;_tmp38F=_tmp38D.f2;
if(_tmp38F <= (void*)4)goto _LL2B0;if(*((int*)_tmp38F)!= 0)goto _LL2B0;_LL2AF: {
const char*_tmpD45;void*_tmpD44;(_tmpD44=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpD45="typecmp: can only compare closed types",
_tag_dyneither(_tmpD45,sizeof(char),39))),_tag_dyneither(_tmpD44,sizeof(void*),0)));}
_LL2B0: _tmp390=_tmp38D.f1;if(_tmp390 <= (void*)4)goto _LL2B2;if(*((int*)_tmp390)!= 
1)goto _LL2B2;_tmp391=((struct Cyc_Absyn_VarType_struct*)_tmp390)->f1;_tmp392=
_tmp38D.f2;if(_tmp392 <= (void*)4)goto _LL2B2;if(*((int*)_tmp392)!= 1)goto _LL2B2;
_tmp393=((struct Cyc_Absyn_VarType_struct*)_tmp392)->f1;_LL2B1: return Cyc_Core_intcmp(
_tmp393->identity,_tmp391->identity);_LL2B2: _tmp394=_tmp38D.f1;if(_tmp394 <= (
void*)4)goto _LL2B4;if(*((int*)_tmp394)!= 10)goto _LL2B4;_tmp395=((struct Cyc_Absyn_AggrType_struct*)
_tmp394)->f1;_tmp396=_tmp395.aggr_info;_tmp397=_tmp395.targs;_tmp398=_tmp38D.f2;
if(_tmp398 <= (void*)4)goto _LL2B4;if(*((int*)_tmp398)!= 10)goto _LL2B4;_tmp399=((
struct Cyc_Absyn_AggrType_struct*)_tmp398)->f1;_tmp39A=_tmp399.aggr_info;_tmp39B=
_tmp399.targs;_LL2B3: {struct _tuple2*_tmp421;struct _tuple6 _tmp420=Cyc_Absyn_aggr_kinded_name(
_tmp396);_tmp421=_tmp420.f2;{struct _tuple2*_tmp423;struct _tuple6 _tmp422=Cyc_Absyn_aggr_kinded_name(
_tmp39A);_tmp423=_tmp422.f2;{int _tmp424=Cyc_Absyn_qvar_cmp(_tmp421,_tmp423);if(
_tmp424 != 0)return _tmp424;else{return Cyc_List_list_cmp(Cyc_Tcutil_typecmp,
_tmp397,_tmp39B);}}}}_LL2B4: _tmp39C=_tmp38D.f1;if(_tmp39C <= (void*)4)goto _LL2B6;
if(*((int*)_tmp39C)!= 12)goto _LL2B6;_tmp39D=((struct Cyc_Absyn_EnumType_struct*)
_tmp39C)->f1;_tmp39E=_tmp38D.f2;if(_tmp39E <= (void*)4)goto _LL2B6;if(*((int*)
_tmp39E)!= 12)goto _LL2B6;_tmp39F=((struct Cyc_Absyn_EnumType_struct*)_tmp39E)->f1;
_LL2B5: return Cyc_Absyn_qvar_cmp(_tmp39D,_tmp39F);_LL2B6: _tmp3A0=_tmp38D.f1;if(
_tmp3A0 <= (void*)4)goto _LL2B8;if(*((int*)_tmp3A0)!= 13)goto _LL2B8;_tmp3A1=((
struct Cyc_Absyn_AnonEnumType_struct*)_tmp3A0)->f1;_tmp3A2=_tmp38D.f2;if(_tmp3A2
<= (void*)4)goto _LL2B8;if(*((int*)_tmp3A2)!= 13)goto _LL2B8;_tmp3A3=((struct Cyc_Absyn_AnonEnumType_struct*)
_tmp3A2)->f1;_LL2B7: return((int(*)(int(*cmp)(struct Cyc_Absyn_Enumfield*,struct
Cyc_Absyn_Enumfield*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(
Cyc_Tcutil_enumfield_cmp,_tmp3A1,_tmp3A3);_LL2B8: _tmp3A4=_tmp38D.f1;if(_tmp3A4 <= (
void*)4)goto _LL2BA;if(*((int*)_tmp3A4)!= 2)goto _LL2BA;_tmp3A5=((struct Cyc_Absyn_TunionType_struct*)
_tmp3A4)->f1;_tmp3A6=_tmp3A5.tunion_info;if((((((struct Cyc_Absyn_TunionType_struct*)
_tmp3A4)->f1).tunion_info).KnownTunion).tag != 1)goto _LL2BA;_tmp3A7=(_tmp3A6.KnownTunion).f1;
_tmp3A8=*_tmp3A7;_tmp3A9=_tmp3A5.targs;_tmp3AA=_tmp3A5.rgn;_tmp3AB=_tmp38D.f2;
if(_tmp3AB <= (void*)4)goto _LL2BA;if(*((int*)_tmp3AB)!= 2)goto _LL2BA;_tmp3AC=((
struct Cyc_Absyn_TunionType_struct*)_tmp3AB)->f1;_tmp3AD=_tmp3AC.tunion_info;if((((((
struct Cyc_Absyn_TunionType_struct*)_tmp3AB)->f1).tunion_info).KnownTunion).tag != 
1)goto _LL2BA;_tmp3AE=(_tmp3AD.KnownTunion).f1;_tmp3AF=*_tmp3AE;_tmp3B0=_tmp3AC.targs;
_tmp3B1=_tmp3AC.rgn;_LL2B9: if(_tmp3AF == _tmp3A8)return 0;{int _tmp425=Cyc_Absyn_qvar_cmp(
_tmp3AF->name,_tmp3A8->name);if(_tmp425 != 0)return _tmp425;if((unsigned int)
_tmp3B1  && (unsigned int)_tmp3AA){int _tmp426=Cyc_Tcutil_typecmp((void*)_tmp3B1->v,(
void*)_tmp3AA->v);if(_tmp426 != 0)return _tmp426;}else{if((unsigned int)_tmp3B1)
return - 1;else{return 1;}}return Cyc_List_list_cmp(Cyc_Tcutil_typecmp,_tmp3B0,
_tmp3A9);}_LL2BA: _tmp3B2=_tmp38D.f1;if(_tmp3B2 <= (void*)4)goto _LL2BC;if(*((int*)
_tmp3B2)!= 3)goto _LL2BC;_tmp3B3=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp3B2)->f1;_tmp3B4=_tmp3B3.field_info;if((((((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp3B2)->f1).field_info).KnownTunionfield).tag != 1)goto _LL2BC;_tmp3B5=(_tmp3B4.KnownTunionfield).f1;
_tmp3B6=(_tmp3B4.KnownTunionfield).f2;_tmp3B7=_tmp3B3.targs;_tmp3B8=_tmp38D.f2;
if(_tmp3B8 <= (void*)4)goto _LL2BC;if(*((int*)_tmp3B8)!= 3)goto _LL2BC;_tmp3B9=((
struct Cyc_Absyn_TunionFieldType_struct*)_tmp3B8)->f1;_tmp3BA=_tmp3B9.field_info;
if((((((struct Cyc_Absyn_TunionFieldType_struct*)_tmp3B8)->f1).field_info).KnownTunionfield).tag
!= 1)goto _LL2BC;_tmp3BB=(_tmp3BA.KnownTunionfield).f1;_tmp3BC=(_tmp3BA.KnownTunionfield).f2;
_tmp3BD=_tmp3B9.targs;_LL2BB: if(_tmp3BB == _tmp3B5)return 0;{int _tmp427=Cyc_Absyn_qvar_cmp(
_tmp3B5->name,_tmp3BB->name);if(_tmp427 != 0)return _tmp427;{int _tmp428=Cyc_Absyn_qvar_cmp(
_tmp3B6->name,_tmp3BC->name);if(_tmp428 != 0)return _tmp428;return Cyc_List_list_cmp(
Cyc_Tcutil_typecmp,_tmp3BD,_tmp3B7);}}_LL2BC: _tmp3BE=_tmp38D.f1;if(_tmp3BE <= (
void*)4)goto _LL2BE;if(*((int*)_tmp3BE)!= 4)goto _LL2BE;_tmp3BF=((struct Cyc_Absyn_PointerType_struct*)
_tmp3BE)->f1;_tmp3C0=(void*)_tmp3BF.elt_typ;_tmp3C1=_tmp3BF.elt_tq;_tmp3C2=
_tmp3BF.ptr_atts;_tmp3C3=(void*)_tmp3C2.rgn;_tmp3C4=_tmp3C2.nullable;_tmp3C5=
_tmp3C2.bounds;_tmp3C6=_tmp3C2.zero_term;_tmp3C7=_tmp38D.f2;if(_tmp3C7 <= (void*)
4)goto _LL2BE;if(*((int*)_tmp3C7)!= 4)goto _LL2BE;_tmp3C8=((struct Cyc_Absyn_PointerType_struct*)
_tmp3C7)->f1;_tmp3C9=(void*)_tmp3C8.elt_typ;_tmp3CA=_tmp3C8.elt_tq;_tmp3CB=
_tmp3C8.ptr_atts;_tmp3CC=(void*)_tmp3CB.rgn;_tmp3CD=_tmp3CB.nullable;_tmp3CE=
_tmp3CB.bounds;_tmp3CF=_tmp3CB.zero_term;_LL2BD: {int _tmp429=Cyc_Tcutil_typecmp(
_tmp3C9,_tmp3C0);if(_tmp429 != 0)return _tmp429;{int _tmp42A=Cyc_Tcutil_typecmp(
_tmp3CC,_tmp3C3);if(_tmp42A != 0)return _tmp42A;{int _tmp42B=Cyc_Tcutil_tqual_cmp(
_tmp3CA,_tmp3C1);if(_tmp42B != 0)return _tmp42B;{int _tmp42C=Cyc_Tcutil_conrefs_cmp(
Cyc_Tcutil_boundscmp,_tmp3CE,_tmp3C5);if(_tmp42C != 0)return _tmp42C;{int _tmp42D=((
int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_conrefs_cmp)(
Cyc_Core_intcmp,_tmp3CF,_tmp3C6);if(_tmp42D != 0)return _tmp42D;{union Cyc_Absyn_Constraint_union
_tmp42E=(Cyc_Absyn_compress_conref(_tmp3CE))->v;void*_tmp42F;_LL2E3: if((_tmp42E.Eq_constr).tag
!= 0)goto _LL2E5;_tmp42F=(_tmp42E.Eq_constr).f1;if((int)_tmp42F != 0)goto _LL2E5;
_LL2E4: return 0;_LL2E5:;_LL2E6: goto _LL2E2;_LL2E2:;}return((int(*)(int(*cmp)(int,
int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_conrefs_cmp)(
Cyc_Core_intcmp,_tmp3CD,_tmp3C4);}}}}}_LL2BE: _tmp3D0=_tmp38D.f1;if(_tmp3D0 <= (
void*)4)goto _LL2C0;if(*((int*)_tmp3D0)!= 5)goto _LL2C0;_tmp3D1=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp3D0)->f1;_tmp3D2=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp3D0)->f2;
_tmp3D3=_tmp38D.f2;if(_tmp3D3 <= (void*)4)goto _LL2C0;if(*((int*)_tmp3D3)!= 5)goto
_LL2C0;_tmp3D4=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp3D3)->f1;_tmp3D5=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp3D3)->f2;_LL2BF: if(_tmp3D4 != _tmp3D1)
return Cyc_Core_intcmp((int)((unsigned int)_tmp3D4),(int)((unsigned int)_tmp3D1));
if(_tmp3D5 != _tmp3D2)return Cyc_Core_intcmp((int)((unsigned int)_tmp3D5),(int)((
unsigned int)_tmp3D2));return 0;_LL2C0: _tmp3D6=_tmp38D.f1;if((int)_tmp3D6 != 1)
goto _LL2C2;_tmp3D7=_tmp38D.f2;if((int)_tmp3D7 != 1)goto _LL2C2;_LL2C1: return 0;
_LL2C2: _tmp3D8=_tmp38D.f1;if(_tmp3D8 <= (void*)4)goto _LL2C4;if(*((int*)_tmp3D8)!= 
6)goto _LL2C4;_tmp3D9=((struct Cyc_Absyn_DoubleType_struct*)_tmp3D8)->f1;_tmp3DA=
_tmp38D.f2;if(_tmp3DA <= (void*)4)goto _LL2C4;if(*((int*)_tmp3DA)!= 6)goto _LL2C4;
_tmp3DB=((struct Cyc_Absyn_DoubleType_struct*)_tmp3DA)->f1;_LL2C3: if(_tmp3D9 == 
_tmp3DB)return 0;else{if(_tmp3D9)return - 1;else{return 1;}}_LL2C4: _tmp3DC=_tmp38D.f1;
if(_tmp3DC <= (void*)4)goto _LL2C6;if(*((int*)_tmp3DC)!= 7)goto _LL2C6;_tmp3DD=((
struct Cyc_Absyn_ArrayType_struct*)_tmp3DC)->f1;_tmp3DE=(void*)_tmp3DD.elt_type;
_tmp3DF=_tmp3DD.tq;_tmp3E0=_tmp3DD.num_elts;_tmp3E1=_tmp3DD.zero_term;_tmp3E2=
_tmp38D.f2;if(_tmp3E2 <= (void*)4)goto _LL2C6;if(*((int*)_tmp3E2)!= 7)goto _LL2C6;
_tmp3E3=((struct Cyc_Absyn_ArrayType_struct*)_tmp3E2)->f1;_tmp3E4=(void*)_tmp3E3.elt_type;
_tmp3E5=_tmp3E3.tq;_tmp3E6=_tmp3E3.num_elts;_tmp3E7=_tmp3E3.zero_term;_LL2C5: {
int _tmp430=Cyc_Tcutil_tqual_cmp(_tmp3E5,_tmp3DF);if(_tmp430 != 0)return _tmp430;{
int _tmp431=Cyc_Tcutil_typecmp(_tmp3E4,_tmp3DE);if(_tmp431 != 0)return _tmp431;{int
_tmp432=((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*
y))Cyc_Tcutil_conrefs_cmp)(Cyc_Core_intcmp,_tmp3E1,_tmp3E7);if(_tmp432 != 0)
return _tmp432;if(_tmp3E0 == _tmp3E6)return 0;if(_tmp3E0 == 0  || _tmp3E6 == 0){const
char*_tmpD48;void*_tmpD47;(_tmpD47=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpD48="missing expression in array index",
_tag_dyneither(_tmpD48,sizeof(char),34))),_tag_dyneither(_tmpD47,sizeof(void*),0)));}
return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*
a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,_tmp3E0,
_tmp3E6);}}}_LL2C6: _tmp3E8=_tmp38D.f1;if(_tmp3E8 <= (void*)4)goto _LL2C8;if(*((int*)
_tmp3E8)!= 8)goto _LL2C8;_tmp3E9=((struct Cyc_Absyn_FnType_struct*)_tmp3E8)->f1;
_tmp3EA=_tmp3E9.tvars;_tmp3EB=_tmp3E9.effect;_tmp3EC=(void*)_tmp3E9.ret_typ;
_tmp3ED=_tmp3E9.args;_tmp3EE=_tmp3E9.c_varargs;_tmp3EF=_tmp3E9.cyc_varargs;
_tmp3F0=_tmp3E9.rgn_po;_tmp3F1=_tmp3E9.attributes;_tmp3F2=_tmp38D.f2;if(_tmp3F2
<= (void*)4)goto _LL2C8;if(*((int*)_tmp3F2)!= 8)goto _LL2C8;_tmp3F3=((struct Cyc_Absyn_FnType_struct*)
_tmp3F2)->f1;_tmp3F4=_tmp3F3.tvars;_tmp3F5=_tmp3F3.effect;_tmp3F6=(void*)_tmp3F3.ret_typ;
_tmp3F7=_tmp3F3.args;_tmp3F8=_tmp3F3.c_varargs;_tmp3F9=_tmp3F3.cyc_varargs;
_tmp3FA=_tmp3F3.rgn_po;_tmp3FB=_tmp3F3.attributes;_LL2C7: {const char*_tmpD4B;
void*_tmpD4A;(_tmpD4A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmpD4B="typecmp: function types not handled",_tag_dyneither(
_tmpD4B,sizeof(char),36))),_tag_dyneither(_tmpD4A,sizeof(void*),0)));}_LL2C8:
_tmp3FC=_tmp38D.f1;if(_tmp3FC <= (void*)4)goto _LL2CA;if(*((int*)_tmp3FC)!= 9)goto
_LL2CA;_tmp3FD=((struct Cyc_Absyn_TupleType_struct*)_tmp3FC)->f1;_tmp3FE=_tmp38D.f2;
if(_tmp3FE <= (void*)4)goto _LL2CA;if(*((int*)_tmp3FE)!= 9)goto _LL2CA;_tmp3FF=((
struct Cyc_Absyn_TupleType_struct*)_tmp3FE)->f1;_LL2C9: return((int(*)(int(*cmp)(
struct _tuple5*,struct _tuple5*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))
Cyc_List_list_cmp)(Cyc_Tcutil_tqual_type_cmp,_tmp3FF,_tmp3FD);_LL2CA: _tmp400=
_tmp38D.f1;if(_tmp400 <= (void*)4)goto _LL2CC;if(*((int*)_tmp400)!= 11)goto _LL2CC;
_tmp401=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)_tmp400)->f1;_tmp402=((
struct Cyc_Absyn_AnonAggrType_struct*)_tmp400)->f2;_tmp403=_tmp38D.f2;if(_tmp403
<= (void*)4)goto _LL2CC;if(*((int*)_tmp403)!= 11)goto _LL2CC;_tmp404=(void*)((
struct Cyc_Absyn_AnonAggrType_struct*)_tmp403)->f1;_tmp405=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp403)->f2;_LL2CB: if(_tmp404 != _tmp401){if(_tmp404 == (void*)0)return - 1;else{
return 1;}}return((int(*)(int(*cmp)(struct Cyc_Absyn_Aggrfield*,struct Cyc_Absyn_Aggrfield*),
struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_aggrfield_cmp,
_tmp405,_tmp402);_LL2CC: _tmp406=_tmp38D.f1;if(_tmp406 <= (void*)4)goto _LL2CE;if(*((
int*)_tmp406)!= 14)goto _LL2CE;_tmp407=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp406)->f1;_tmp408=_tmp38D.f2;if(_tmp408 <= (void*)4)goto _LL2CE;if(*((int*)
_tmp408)!= 14)goto _LL2CE;_tmp409=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp408)->f1;_LL2CD: return Cyc_Tcutil_typecmp(_tmp407,_tmp409);_LL2CE: _tmp40A=
_tmp38D.f1;if(_tmp40A <= (void*)4)goto _LL2D0;if(*((int*)_tmp40A)!= 15)goto _LL2D0;
_tmp40B=(void*)((struct Cyc_Absyn_DynRgnType_struct*)_tmp40A)->f1;_tmp40C=(void*)((
struct Cyc_Absyn_DynRgnType_struct*)_tmp40A)->f2;_tmp40D=_tmp38D.f2;if(_tmp40D <= (
void*)4)goto _LL2D0;if(*((int*)_tmp40D)!= 15)goto _LL2D0;_tmp40E=(void*)((struct
Cyc_Absyn_DynRgnType_struct*)_tmp40D)->f1;_tmp40F=(void*)((struct Cyc_Absyn_DynRgnType_struct*)
_tmp40D)->f2;_LL2CF: {int _tmp437=Cyc_Tcutil_typecmp(_tmp40B,_tmp40E);if(_tmp437
!= 0)return _tmp437;else{return Cyc_Tcutil_typecmp(_tmp40C,_tmp40F);}}_LL2D0:
_tmp410=_tmp38D.f1;if(_tmp410 <= (void*)4)goto _LL2D2;if(*((int*)_tmp410)!= 18)
goto _LL2D2;_tmp411=(void*)((struct Cyc_Absyn_TagType_struct*)_tmp410)->f1;_tmp412=
_tmp38D.f2;if(_tmp412 <= (void*)4)goto _LL2D2;if(*((int*)_tmp412)!= 18)goto _LL2D2;
_tmp413=(void*)((struct Cyc_Absyn_TagType_struct*)_tmp412)->f1;_LL2D1: return Cyc_Tcutil_typecmp(
_tmp411,_tmp413);_LL2D2: _tmp414=_tmp38D.f1;if(_tmp414 <= (void*)4)goto _LL2D4;if(*((
int*)_tmp414)!= 17)goto _LL2D4;_tmp415=((struct Cyc_Absyn_ValueofType_struct*)
_tmp414)->f1;_tmp416=_tmp38D.f2;if(_tmp416 <= (void*)4)goto _LL2D4;if(*((int*)
_tmp416)!= 17)goto _LL2D4;_tmp417=((struct Cyc_Absyn_ValueofType_struct*)_tmp416)->f1;
_LL2D3: return Cyc_Evexp_const_exp_cmp(_tmp415,_tmp417);_LL2D4: _tmp418=_tmp38D.f1;
if(_tmp418 <= (void*)4)goto _LL2D6;if(*((int*)_tmp418)!= 20)goto _LL2D6;_LL2D5: goto
_LL2D7;_LL2D6: _tmp419=_tmp38D.f2;if(_tmp419 <= (void*)4)goto _LL2D8;if(*((int*)
_tmp419)!= 20)goto _LL2D8;_LL2D7: goto _LL2D9;_LL2D8: _tmp41A=_tmp38D.f1;if(_tmp41A
<= (void*)4)goto _LL2DA;if(*((int*)_tmp41A)!= 19)goto _LL2DA;_LL2D9: goto _LL2DB;
_LL2DA: _tmp41B=_tmp38D.f1;if(_tmp41B <= (void*)4)goto _LL2DC;if(*((int*)_tmp41B)!= 
21)goto _LL2DC;_LL2DB: goto _LL2DD;_LL2DC: _tmp41C=_tmp38D.f2;if(_tmp41C <= (void*)4)
goto _LL2DE;if(*((int*)_tmp41C)!= 21)goto _LL2DE;_LL2DD: goto _LL2DF;_LL2DE: _tmp41D=
_tmp38D.f2;if(_tmp41D <= (void*)4)goto _LL2E0;if(*((int*)_tmp41D)!= 19)goto _LL2E0;
_LL2DF: {const char*_tmpD4E;void*_tmpD4D;(_tmpD4D=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpD4E="typecmp: effects not handled",
_tag_dyneither(_tmpD4E,sizeof(char),29))),_tag_dyneither(_tmpD4D,sizeof(void*),0)));}
_LL2E0:;_LL2E1: {const char*_tmpD51;void*_tmpD50;(_tmpD50=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpD51="Unmatched case in typecmp",
_tag_dyneither(_tmpD51,sizeof(char),26))),_tag_dyneither(_tmpD50,sizeof(void*),0)));}
_LL2AD:;}}}int Cyc_Tcutil_is_arithmetic_type(void*t);int Cyc_Tcutil_is_arithmetic_type(
void*t){void*_tmp43C=Cyc_Tcutil_compress(t);_LL2E8: if(_tmp43C <= (void*)4)goto
_LL2EA;if(*((int*)_tmp43C)!= 5)goto _LL2EA;_LL2E9: goto _LL2EB;_LL2EA: if((int)
_tmp43C != 1)goto _LL2EC;_LL2EB: goto _LL2ED;_LL2EC: if(_tmp43C <= (void*)4)goto _LL2F2;
if(*((int*)_tmp43C)!= 6)goto _LL2EE;_LL2ED: goto _LL2EF;_LL2EE: if(*((int*)_tmp43C)
!= 12)goto _LL2F0;_LL2EF: goto _LL2F1;_LL2F0: if(*((int*)_tmp43C)!= 13)goto _LL2F2;
_LL2F1: return 1;_LL2F2:;_LL2F3: return 0;_LL2E7:;}int Cyc_Tcutil_will_lose_precision(
void*t1,void*t2);int Cyc_Tcutil_will_lose_precision(void*t1,void*t2){t1=Cyc_Tcutil_compress(
t1);t2=Cyc_Tcutil_compress(t2);{struct _tuple0 _tmpD52;struct _tuple0 _tmp43E=(
_tmpD52.f1=t1,((_tmpD52.f2=t2,_tmpD52)));void*_tmp43F;int _tmp440;void*_tmp441;
int _tmp442;void*_tmp443;void*_tmp444;void*_tmp445;void*_tmp446;void*_tmp447;void*
_tmp448;void*_tmp449;void*_tmp44A;void*_tmp44B;void*_tmp44C;void*_tmp44D;void*
_tmp44E;void*_tmp44F;void*_tmp450;void*_tmp451;void*_tmp452;void*_tmp453;void*
_tmp454;void*_tmp455;void*_tmp456;void*_tmp457;void*_tmp458;void*_tmp459;void*
_tmp45A;void*_tmp45B;void*_tmp45C;void*_tmp45D;void*_tmp45E;void*_tmp45F;void*
_tmp460;void*_tmp461;void*_tmp462;void*_tmp463;void*_tmp464;void*_tmp465;void*
_tmp466;void*_tmp467;void*_tmp468;void*_tmp469;void*_tmp46A;void*_tmp46B;void*
_tmp46C;void*_tmp46D;void*_tmp46E;void*_tmp46F;void*_tmp470;void*_tmp471;void*
_tmp472;void*_tmp473;void*_tmp474;void*_tmp475;void*_tmp476;void*_tmp477;void*
_tmp478;void*_tmp479;void*_tmp47A;_LL2F5: _tmp43F=_tmp43E.f1;if(_tmp43F <= (void*)
4)goto _LL2F7;if(*((int*)_tmp43F)!= 6)goto _LL2F7;_tmp440=((struct Cyc_Absyn_DoubleType_struct*)
_tmp43F)->f1;_tmp441=_tmp43E.f2;if(_tmp441 <= (void*)4)goto _LL2F7;if(*((int*)
_tmp441)!= 6)goto _LL2F7;_tmp442=((struct Cyc_Absyn_DoubleType_struct*)_tmp441)->f1;
_LL2F6: return !_tmp442  && _tmp440;_LL2F7: _tmp443=_tmp43E.f1;if(_tmp443 <= (void*)4)
goto _LL2F9;if(*((int*)_tmp443)!= 6)goto _LL2F9;_tmp444=_tmp43E.f2;if((int)_tmp444
!= 1)goto _LL2F9;_LL2F8: goto _LL2FA;_LL2F9: _tmp445=_tmp43E.f1;if(_tmp445 <= (void*)
4)goto _LL2FB;if(*((int*)_tmp445)!= 6)goto _LL2FB;_tmp446=_tmp43E.f2;if(_tmp446 <= (
void*)4)goto _LL2FB;if(*((int*)_tmp446)!= 5)goto _LL2FB;_LL2FA: goto _LL2FC;_LL2FB:
_tmp447=_tmp43E.f1;if(_tmp447 <= (void*)4)goto _LL2FD;if(*((int*)_tmp447)!= 6)goto
_LL2FD;_tmp448=_tmp43E.f2;if(_tmp448 <= (void*)4)goto _LL2FD;if(*((int*)_tmp448)!= 
18)goto _LL2FD;_LL2FC: goto _LL2FE;_LL2FD: _tmp449=_tmp43E.f1;if((int)_tmp449 != 1)
goto _LL2FF;_tmp44A=_tmp43E.f2;if(_tmp44A <= (void*)4)goto _LL2FF;if(*((int*)
_tmp44A)!= 18)goto _LL2FF;_LL2FE: goto _LL300;_LL2FF: _tmp44B=_tmp43E.f1;if((int)
_tmp44B != 1)goto _LL301;_tmp44C=_tmp43E.f2;if(_tmp44C <= (void*)4)goto _LL301;if(*((
int*)_tmp44C)!= 5)goto _LL301;_LL300: return 1;_LL301: _tmp44D=_tmp43E.f1;if(_tmp44D
<= (void*)4)goto _LL303;if(*((int*)_tmp44D)!= 5)goto _LL303;_tmp44E=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp44D)->f2;if((int)_tmp44E != 4)goto _LL303;
_tmp44F=_tmp43E.f2;if(_tmp44F <= (void*)4)goto _LL303;if(*((int*)_tmp44F)!= 5)goto
_LL303;_tmp450=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp44F)->f2;if((int)
_tmp450 != 4)goto _LL303;_LL302: return 0;_LL303: _tmp451=_tmp43E.f1;if(_tmp451 <= (
void*)4)goto _LL305;if(*((int*)_tmp451)!= 5)goto _LL305;_tmp452=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp451)->f2;if((int)_tmp452 != 4)goto _LL305;_LL304: return 1;_LL305: _tmp453=
_tmp43E.f1;if(_tmp453 <= (void*)4)goto _LL307;if(*((int*)_tmp453)!= 5)goto _LL307;
_tmp454=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp453)->f2;if((int)_tmp454 != 
3)goto _LL307;_tmp455=_tmp43E.f2;if(_tmp455 <= (void*)4)goto _LL307;if(*((int*)
_tmp455)!= 5)goto _LL307;_tmp456=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp455)->f2;
if((int)_tmp456 != 2)goto _LL307;_LL306: goto _LL308;_LL307: _tmp457=_tmp43E.f1;if(
_tmp457 <= (void*)4)goto _LL309;if(*((int*)_tmp457)!= 5)goto _LL309;_tmp458=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp457)->f2;if((int)_tmp458 != 2)goto _LL309;
_tmp459=_tmp43E.f2;if(_tmp459 <= (void*)4)goto _LL309;if(*((int*)_tmp459)!= 5)goto
_LL309;_tmp45A=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp459)->f2;if((int)
_tmp45A != 3)goto _LL309;_LL308: return 0;_LL309: _tmp45B=_tmp43E.f1;if(_tmp45B <= (
void*)4)goto _LL30B;if(*((int*)_tmp45B)!= 5)goto _LL30B;_tmp45C=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp45B)->f2;if((int)_tmp45C != 3)goto _LL30B;_tmp45D=_tmp43E.f2;if((int)_tmp45D != 
1)goto _LL30B;_LL30A: goto _LL30C;_LL30B: _tmp45E=_tmp43E.f1;if(_tmp45E <= (void*)4)
goto _LL30D;if(*((int*)_tmp45E)!= 5)goto _LL30D;_tmp45F=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp45E)->f2;if((int)_tmp45F != 2)goto _LL30D;_tmp460=_tmp43E.f2;if((int)_tmp460 != 
1)goto _LL30D;_LL30C: goto _LL30E;_LL30D: _tmp461=_tmp43E.f1;if(_tmp461 <= (void*)4)
goto _LL30F;if(*((int*)_tmp461)!= 5)goto _LL30F;_tmp462=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp461)->f2;if((int)_tmp462 != 3)goto _LL30F;_tmp463=_tmp43E.f2;if(_tmp463 <= (
void*)4)goto _LL30F;if(*((int*)_tmp463)!= 5)goto _LL30F;_tmp464=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp463)->f2;if((int)_tmp464 != 1)goto _LL30F;_LL30E: goto _LL310;_LL30F: _tmp465=
_tmp43E.f1;if(_tmp465 <= (void*)4)goto _LL311;if(*((int*)_tmp465)!= 5)goto _LL311;
_tmp466=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp465)->f2;if((int)_tmp466 != 
2)goto _LL311;_tmp467=_tmp43E.f2;if(_tmp467 <= (void*)4)goto _LL311;if(*((int*)
_tmp467)!= 5)goto _LL311;_tmp468=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp467)->f2;
if((int)_tmp468 != 1)goto _LL311;_LL310: goto _LL312;_LL311: _tmp469=_tmp43E.f1;if(
_tmp469 <= (void*)4)goto _LL313;if(*((int*)_tmp469)!= 18)goto _LL313;_tmp46A=
_tmp43E.f2;if(_tmp46A <= (void*)4)goto _LL313;if(*((int*)_tmp46A)!= 5)goto _LL313;
_tmp46B=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp46A)->f2;if((int)_tmp46B != 
1)goto _LL313;_LL312: goto _LL314;_LL313: _tmp46C=_tmp43E.f1;if(_tmp46C <= (void*)4)
goto _LL315;if(*((int*)_tmp46C)!= 5)goto _LL315;_tmp46D=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp46C)->f2;if((int)_tmp46D != 3)goto _LL315;_tmp46E=_tmp43E.f2;if(_tmp46E <= (
void*)4)goto _LL315;if(*((int*)_tmp46E)!= 5)goto _LL315;_tmp46F=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp46E)->f2;if((int)_tmp46F != 0)goto _LL315;_LL314: goto _LL316;_LL315: _tmp470=
_tmp43E.f1;if(_tmp470 <= (void*)4)goto _LL317;if(*((int*)_tmp470)!= 5)goto _LL317;
_tmp471=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp470)->f2;if((int)_tmp471 != 
2)goto _LL317;_tmp472=_tmp43E.f2;if(_tmp472 <= (void*)4)goto _LL317;if(*((int*)
_tmp472)!= 5)goto _LL317;_tmp473=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp472)->f2;
if((int)_tmp473 != 0)goto _LL317;_LL316: goto _LL318;_LL317: _tmp474=_tmp43E.f1;if(
_tmp474 <= (void*)4)goto _LL319;if(*((int*)_tmp474)!= 5)goto _LL319;_tmp475=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp474)->f2;if((int)_tmp475 != 1)goto _LL319;
_tmp476=_tmp43E.f2;if(_tmp476 <= (void*)4)goto _LL319;if(*((int*)_tmp476)!= 5)goto
_LL319;_tmp477=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp476)->f2;if((int)
_tmp477 != 0)goto _LL319;_LL318: goto _LL31A;_LL319: _tmp478=_tmp43E.f1;if(_tmp478 <= (
void*)4)goto _LL31B;if(*((int*)_tmp478)!= 18)goto _LL31B;_tmp479=_tmp43E.f2;if(
_tmp479 <= (void*)4)goto _LL31B;if(*((int*)_tmp479)!= 5)goto _LL31B;_tmp47A=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp479)->f2;if((int)_tmp47A != 0)goto _LL31B;
_LL31A: return 1;_LL31B:;_LL31C: return 0;_LL2F4:;}}int Cyc_Tcutil_coerce_list(struct
Cyc_Tcenv_Tenv*te,void*t,struct Cyc_List_List*es);int Cyc_Tcutil_coerce_list(
struct Cyc_Tcenv_Tenv*te,void*t,struct Cyc_List_List*es){{struct _RegionHandle
_tmp47B=_new_region("r");struct _RegionHandle*r=& _tmp47B;_push_region(r);{struct
Cyc_Core_Opt*max_arith_type=0;{struct Cyc_List_List*el=es;for(0;el != 0;el=el->tl){
void*t1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Exp*)
el->hd)->topt))->v);if(Cyc_Tcutil_is_arithmetic_type(t1)){if(max_arith_type == 0
 || Cyc_Tcutil_will_lose_precision(t1,(void*)max_arith_type->v)){struct Cyc_Core_Opt*
_tmpD53;max_arith_type=((_tmpD53=_region_malloc(r,sizeof(*_tmpD53)),((_tmpD53->v=(
void*)t1,_tmpD53))));}}}}if(max_arith_type != 0){if(!Cyc_Tcutil_unify(t,(void*)
max_arith_type->v)){int _tmp47D=0;_npop_handler(0);return _tmp47D;}}};_pop_region(
r);}{struct Cyc_List_List*el=es;for(0;el != 0;el=el->tl){if(!Cyc_Tcutil_coerce_assign(
te,(struct Cyc_Absyn_Exp*)el->hd,t)){{const char*_tmpD58;void*_tmpD57[2];struct Cyc_String_pa_struct
_tmpD56;struct Cyc_String_pa_struct _tmpD55;(_tmpD55.tag=0,((_tmpD55.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt))->v)),((_tmpD56.tag=0,((
_tmpD56.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((
_tmpD57[0]=& _tmpD56,((_tmpD57[1]=& _tmpD55,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)
el->hd)->loc,((_tmpD58="type mismatch: expecting %s but found %s",_tag_dyneither(
_tmpD58,sizeof(char),41))),_tag_dyneither(_tmpD57,sizeof(void*),2)))))))))))));}
return 0;}}}return 1;}int Cyc_Tcutil_coerce_to_bool(struct Cyc_Tcenv_Tenv*te,struct
Cyc_Absyn_Exp*e);int Cyc_Tcutil_coerce_to_bool(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*
e){if(!Cyc_Tcutil_coerce_sint_typ(te,e)){void*_tmp482=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v);_LL31E: if(_tmp482 <= (void*)4)goto
_LL320;if(*((int*)_tmp482)!= 4)goto _LL320;_LL31F: Cyc_Tcutil_unchecked_cast(te,e,
Cyc_Absyn_uint_typ,(void*)3);goto _LL31D;_LL320:;_LL321: return 0;_LL31D:;}return 1;}
int Cyc_Tcutil_is_integral_type(void*t);int Cyc_Tcutil_is_integral_type(void*t){
void*_tmp483=Cyc_Tcutil_compress(t);_LL323: if(_tmp483 <= (void*)4)goto _LL32B;if(*((
int*)_tmp483)!= 5)goto _LL325;_LL324: goto _LL326;_LL325: if(*((int*)_tmp483)!= 18)
goto _LL327;_LL326: goto _LL328;_LL327: if(*((int*)_tmp483)!= 12)goto _LL329;_LL328:
goto _LL32A;_LL329: if(*((int*)_tmp483)!= 13)goto _LL32B;_LL32A: return 1;_LL32B:;
_LL32C: return 0;_LL322:;}int Cyc_Tcutil_coerce_uint_typ(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Absyn_Exp*e);int Cyc_Tcutil_coerce_uint_typ(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Absyn_Exp*e){if(Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,Cyc_Absyn_uint_typ))return 1;if(Cyc_Tcutil_is_integral_type((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)){if(Cyc_Tcutil_will_lose_precision((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_Absyn_uint_typ)){const
char*_tmpD5B;void*_tmpD5A;(_tmpD5A=0,Cyc_Tcutil_warn(e->loc,((_tmpD5B="integral size mismatch; conversion supplied",
_tag_dyneither(_tmpD5B,sizeof(char),44))),_tag_dyneither(_tmpD5A,sizeof(void*),0)));}
Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_uint_typ,(void*)1);return 1;}return 0;}
int Cyc_Tcutil_coerce_sint_typ(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e);int
Cyc_Tcutil_coerce_sint_typ(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){if(Cyc_Tcutil_unify((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_Absyn_sint_typ))return 1;
if(Cyc_Tcutil_is_integral_type((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)){
if(Cyc_Tcutil_will_lose_precision((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,
Cyc_Absyn_sint_typ)){const char*_tmpD5E;void*_tmpD5D;(_tmpD5D=0,Cyc_Tcutil_warn(e->loc,((
_tmpD5E="integral size mismatch; conversion supplied",_tag_dyneither(_tmpD5E,
sizeof(char),44))),_tag_dyneither(_tmpD5D,sizeof(void*),0)));}Cyc_Tcutil_unchecked_cast(
te,e,Cyc_Absyn_sint_typ,(void*)1);return 1;}return 0;}struct _tuple13{union Cyc_Absyn_Constraint_union
f1;union Cyc_Absyn_Constraint_union f2;};int Cyc_Tcutil_silent_castable(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void*t1,void*t2);int Cyc_Tcutil_silent_castable(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void*t1,void*t2){t1=Cyc_Tcutil_compress(
t1);t2=Cyc_Tcutil_compress(t2);{struct _tuple0 _tmpD5F;struct _tuple0 _tmp489=(
_tmpD5F.f1=t1,((_tmpD5F.f2=t2,_tmpD5F)));void*_tmp48A;struct Cyc_Absyn_PtrInfo
_tmp48B;void*_tmp48C;struct Cyc_Absyn_PtrInfo _tmp48D;void*_tmp48E;struct Cyc_Absyn_TunionInfo
_tmp48F;union Cyc_Absyn_TunionInfoU_union _tmp490;struct Cyc_Absyn_Tuniondecl**
_tmp491;struct Cyc_Absyn_Tuniondecl*_tmp492;struct Cyc_List_List*_tmp493;struct Cyc_Core_Opt*
_tmp494;struct Cyc_Core_Opt _tmp495;void*_tmp496;void*_tmp497;struct Cyc_Absyn_TunionInfo
_tmp498;union Cyc_Absyn_TunionInfoU_union _tmp499;struct Cyc_Absyn_Tuniondecl**
_tmp49A;struct Cyc_Absyn_Tuniondecl*_tmp49B;struct Cyc_List_List*_tmp49C;struct Cyc_Core_Opt*
_tmp49D;struct Cyc_Core_Opt _tmp49E;void*_tmp49F;void*_tmp4A0;struct Cyc_Absyn_ArrayInfo
_tmp4A1;void*_tmp4A2;struct Cyc_Absyn_Tqual _tmp4A3;struct Cyc_Absyn_Exp*_tmp4A4;
struct Cyc_Absyn_Conref*_tmp4A5;void*_tmp4A6;struct Cyc_Absyn_ArrayInfo _tmp4A7;
void*_tmp4A8;struct Cyc_Absyn_Tqual _tmp4A9;struct Cyc_Absyn_Exp*_tmp4AA;struct Cyc_Absyn_Conref*
_tmp4AB;void*_tmp4AC;struct Cyc_Absyn_TunionFieldInfo _tmp4AD;union Cyc_Absyn_TunionFieldInfoU_union
_tmp4AE;struct Cyc_Absyn_Tuniondecl*_tmp4AF;struct Cyc_Absyn_Tunionfield*_tmp4B0;
struct Cyc_List_List*_tmp4B1;void*_tmp4B2;struct Cyc_Absyn_TunionInfo _tmp4B3;union
Cyc_Absyn_TunionInfoU_union _tmp4B4;struct Cyc_Absyn_Tuniondecl**_tmp4B5;struct Cyc_Absyn_Tuniondecl*
_tmp4B6;struct Cyc_List_List*_tmp4B7;void*_tmp4B8;struct Cyc_Absyn_PtrInfo _tmp4B9;
void*_tmp4BA;struct Cyc_Absyn_Tqual _tmp4BB;struct Cyc_Absyn_PtrAtts _tmp4BC;void*
_tmp4BD;struct Cyc_Absyn_Conref*_tmp4BE;struct Cyc_Absyn_Conref*_tmp4BF;struct Cyc_Absyn_Conref*
_tmp4C0;void*_tmp4C1;struct Cyc_Absyn_TunionInfo _tmp4C2;union Cyc_Absyn_TunionInfoU_union
_tmp4C3;struct Cyc_Absyn_Tuniondecl**_tmp4C4;struct Cyc_Absyn_Tuniondecl*_tmp4C5;
struct Cyc_List_List*_tmp4C6;struct Cyc_Core_Opt*_tmp4C7;void*_tmp4C8;void*_tmp4C9;
_LL32E: _tmp48A=_tmp489.f1;if(_tmp48A <= (void*)4)goto _LL330;if(*((int*)_tmp48A)!= 
4)goto _LL330;_tmp48B=((struct Cyc_Absyn_PointerType_struct*)_tmp48A)->f1;_tmp48C=
_tmp489.f2;if(_tmp48C <= (void*)4)goto _LL330;if(*((int*)_tmp48C)!= 4)goto _LL330;
_tmp48D=((struct Cyc_Absyn_PointerType_struct*)_tmp48C)->f1;_LL32F: {int okay=1;
if(!((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*
y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,(_tmp48B.ptr_atts).nullable,(
_tmp48D.ptr_atts).nullable)){union Cyc_Absyn_Constraint_union _tmp4CA=(((struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref*x))Cyc_Absyn_compress_conref)((_tmp48B.ptr_atts).nullable))->v;
int _tmp4CB;_LL33D: if((_tmp4CA.Eq_constr).tag != 0)goto _LL33F;_tmp4CB=(_tmp4CA.Eq_constr).f1;
_LL33E: okay=!_tmp4CB;goto _LL33C;_LL33F:;_LL340: {const char*_tmpD62;void*_tmpD61;(
_tmpD61=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmpD62="silent_castable conref not eq",_tag_dyneither(_tmpD62,sizeof(char),30))),
_tag_dyneither(_tmpD61,sizeof(void*),0)));}_LL33C:;}if(!Cyc_Tcutil_unify_conrefs(
Cyc_Tcutil_unify_it_bounds,(_tmp48B.ptr_atts).bounds,(_tmp48D.ptr_atts).bounds)){
struct _tuple13 _tmpD63;struct _tuple13 _tmp4CF=(_tmpD63.f1=(Cyc_Absyn_compress_conref((
_tmp48B.ptr_atts).bounds))->v,((_tmpD63.f2=(Cyc_Absyn_compress_conref((_tmp48D.ptr_atts).bounds))->v,
_tmpD63)));union Cyc_Absyn_Constraint_union _tmp4D0;void*_tmp4D1;union Cyc_Absyn_Constraint_union
_tmp4D2;void*_tmp4D3;union Cyc_Absyn_Constraint_union _tmp4D4;_LL342: _tmp4D0=
_tmp4CF.f1;if(((_tmp4CF.f1).Eq_constr).tag != 0)goto _LL344;_tmp4D1=(_tmp4D0.Eq_constr).f1;
_tmp4D2=_tmp4CF.f2;if(((_tmp4CF.f2).Eq_constr).tag != 0)goto _LL344;_tmp4D3=(
_tmp4D2.Eq_constr).f1;_LL343:{struct _tuple0 _tmpD64;struct _tuple0 _tmp4D6=(_tmpD64.f1=
_tmp4D1,((_tmpD64.f2=_tmp4D3,_tmpD64)));void*_tmp4D7;void*_tmp4D8;void*_tmp4D9;
void*_tmp4DA;void*_tmp4DB;struct Cyc_Absyn_Exp*_tmp4DC;void*_tmp4DD;struct Cyc_Absyn_Exp*
_tmp4DE;void*_tmp4DF;void*_tmp4E0;struct Cyc_Absyn_Exp*_tmp4E1;_LL349: _tmp4D7=
_tmp4D6.f1;if(_tmp4D7 <= (void*)1)goto _LL34B;if(*((int*)_tmp4D7)!= 0)goto _LL34B;
_tmp4D8=_tmp4D6.f2;if((int)_tmp4D8 != 0)goto _LL34B;_LL34A: goto _LL34C;_LL34B:
_tmp4D9=_tmp4D6.f1;if((int)_tmp4D9 != 0)goto _LL34D;_tmp4DA=_tmp4D6.f2;if((int)
_tmp4DA != 0)goto _LL34D;_LL34C: okay=1;goto _LL348;_LL34D: _tmp4DB=_tmp4D6.f1;if(
_tmp4DB <= (void*)1)goto _LL34F;if(*((int*)_tmp4DB)!= 0)goto _LL34F;_tmp4DC=((
struct Cyc_Absyn_Upper_b_struct*)_tmp4DB)->f1;_tmp4DD=_tmp4D6.f2;if(_tmp4DD <= (
void*)1)goto _LL34F;if(*((int*)_tmp4DD)!= 0)goto _LL34F;_tmp4DE=((struct Cyc_Absyn_Upper_b_struct*)
_tmp4DD)->f1;_LL34E: okay=okay  && Cyc_Evexp_lte_const_exp(_tmp4DE,_tmp4DC);if(!((
int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,(_tmp48D.ptr_atts).zero_term)){
const char*_tmpD67;void*_tmpD66;(_tmpD66=0,Cyc_Tcutil_warn(loc,((_tmpD67="implicit cast to shorter array",
_tag_dyneither(_tmpD67,sizeof(char),31))),_tag_dyneither(_tmpD66,sizeof(void*),0)));}
goto _LL348;_LL34F: _tmp4DF=_tmp4D6.f1;if((int)_tmp4DF != 0)goto _LL348;_tmp4E0=
_tmp4D6.f2;if(_tmp4E0 <= (void*)1)goto _LL348;if(*((int*)_tmp4E0)!= 0)goto _LL348;
_tmp4E1=((struct Cyc_Absyn_Upper_b_struct*)_tmp4E0)->f1;_LL350: if(((int(*)(int,
struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,(_tmp48B.ptr_atts).zero_term)
 && Cyc_Tcutil_is_bound_one((_tmp48D.ptr_atts).bounds))goto _LL348;okay=0;goto
_LL348;_LL348:;}goto _LL341;_LL344: _tmp4D4=_tmp4CF.f1;if(((_tmp4CF.f1).No_constr).tag
!= 2)goto _LL346;_LL345: okay=0;goto _LL341;_LL346:;_LL347: {const char*_tmpD6A;void*
_tmpD69;(_tmpD69=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmpD6A="silent_castable conrefs didn't unify",_tag_dyneither(_tmpD6A,sizeof(
char),37))),_tag_dyneither(_tmpD69,sizeof(void*),0)));}_LL341:;}okay=okay  && Cyc_Tcutil_unify((
void*)_tmp48B.elt_typ,(void*)_tmp48D.elt_typ);okay=okay  && (Cyc_Tcutil_unify((
void*)(_tmp48B.ptr_atts).rgn,(void*)(_tmp48D.ptr_atts).rgn) || Cyc_Tcenv_region_outlives(
te,(void*)(_tmp48B.ptr_atts).rgn,(void*)(_tmp48D.ptr_atts).rgn));okay=okay  && (!(
_tmp48B.elt_tq).real_const  || (_tmp48D.elt_tq).real_const);okay=okay  && (((int(*)(
int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(
Cyc_Core_intcmp,(_tmp48B.ptr_atts).zero_term,(_tmp48D.ptr_atts).zero_term) || ((
int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(1,(_tmp48B.ptr_atts).zero_term)
 && (_tmp48D.elt_tq).real_const);return okay;}_LL330: _tmp48E=_tmp489.f1;if(
_tmp48E <= (void*)4)goto _LL332;if(*((int*)_tmp48E)!= 2)goto _LL332;_tmp48F=((
struct Cyc_Absyn_TunionType_struct*)_tmp48E)->f1;_tmp490=_tmp48F.tunion_info;if((((((
struct Cyc_Absyn_TunionType_struct*)_tmp48E)->f1).tunion_info).KnownTunion).tag != 
1)goto _LL332;_tmp491=(_tmp490.KnownTunion).f1;_tmp492=*_tmp491;_tmp493=_tmp48F.targs;
_tmp494=_tmp48F.rgn;if(_tmp494 == 0)goto _LL332;_tmp495=*_tmp494;_tmp496=(void*)
_tmp495.v;_tmp497=_tmp489.f2;if(_tmp497 <= (void*)4)goto _LL332;if(*((int*)_tmp497)
!= 2)goto _LL332;_tmp498=((struct Cyc_Absyn_TunionType_struct*)_tmp497)->f1;
_tmp499=_tmp498.tunion_info;if((((((struct Cyc_Absyn_TunionType_struct*)_tmp497)->f1).tunion_info).KnownTunion).tag
!= 1)goto _LL332;_tmp49A=(_tmp499.KnownTunion).f1;_tmp49B=*_tmp49A;_tmp49C=
_tmp498.targs;_tmp49D=_tmp498.rgn;if(_tmp49D == 0)goto _LL332;_tmp49E=*_tmp49D;
_tmp49F=(void*)_tmp49E.v;_LL331: if(_tmp492 != _tmp49B  || !Cyc_Tcenv_region_outlives(
te,_tmp496,_tmp49F))return 0;for(0;_tmp493 != 0  && _tmp49C != 0;(_tmp493=_tmp493->tl,
_tmp49C=_tmp49C->tl)){if(!Cyc_Tcutil_unify((void*)_tmp493->hd,(void*)_tmp49C->hd))
return 0;}if(_tmp493 != 0  || _tmp49C != 0)return 0;return 1;_LL332: _tmp4A0=_tmp489.f1;
if(_tmp4A0 <= (void*)4)goto _LL334;if(*((int*)_tmp4A0)!= 7)goto _LL334;_tmp4A1=((
struct Cyc_Absyn_ArrayType_struct*)_tmp4A0)->f1;_tmp4A2=(void*)_tmp4A1.elt_type;
_tmp4A3=_tmp4A1.tq;_tmp4A4=_tmp4A1.num_elts;_tmp4A5=_tmp4A1.zero_term;_tmp4A6=
_tmp489.f2;if(_tmp4A6 <= (void*)4)goto _LL334;if(*((int*)_tmp4A6)!= 7)goto _LL334;
_tmp4A7=((struct Cyc_Absyn_ArrayType_struct*)_tmp4A6)->f1;_tmp4A8=(void*)_tmp4A7.elt_type;
_tmp4A9=_tmp4A7.tq;_tmp4AA=_tmp4A7.num_elts;_tmp4AB=_tmp4A7.zero_term;_LL333: {
int okay;okay=((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*
y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp4A5,_tmp4AB) && ((_tmp4A4 != 0
 && _tmp4AA != 0) && Cyc_Evexp_same_const_exp((struct Cyc_Absyn_Exp*)_tmp4A4,(
struct Cyc_Absyn_Exp*)_tmp4AA));return(okay  && Cyc_Tcutil_unify(_tmp4A2,_tmp4A8))
 && (!_tmp4A3.real_const  || _tmp4A9.real_const);}_LL334: _tmp4AC=_tmp489.f1;if(
_tmp4AC <= (void*)4)goto _LL336;if(*((int*)_tmp4AC)!= 3)goto _LL336;_tmp4AD=((
struct Cyc_Absyn_TunionFieldType_struct*)_tmp4AC)->f1;_tmp4AE=_tmp4AD.field_info;
if((((((struct Cyc_Absyn_TunionFieldType_struct*)_tmp4AC)->f1).field_info).KnownTunionfield).tag
!= 1)goto _LL336;_tmp4AF=(_tmp4AE.KnownTunionfield).f1;_tmp4B0=(_tmp4AE.KnownTunionfield).f2;
_tmp4B1=_tmp4AD.targs;_tmp4B2=_tmp489.f2;if(_tmp4B2 <= (void*)4)goto _LL336;if(*((
int*)_tmp4B2)!= 2)goto _LL336;_tmp4B3=((struct Cyc_Absyn_TunionType_struct*)
_tmp4B2)->f1;_tmp4B4=_tmp4B3.tunion_info;if((((((struct Cyc_Absyn_TunionType_struct*)
_tmp4B2)->f1).tunion_info).KnownTunion).tag != 1)goto _LL336;_tmp4B5=(_tmp4B4.KnownTunion).f1;
_tmp4B6=*_tmp4B5;_tmp4B7=_tmp4B3.targs;_LL335: if((_tmp4AF == _tmp4B6  || Cyc_Absyn_qvar_cmp(
_tmp4AF->name,_tmp4B6->name)== 0) && (_tmp4B0->typs == 0  || _tmp4AF->is_flat)){
for(0;_tmp4B1 != 0  && _tmp4B7 != 0;(_tmp4B1=_tmp4B1->tl,_tmp4B7=_tmp4B7->tl)){if(!
Cyc_Tcutil_unify((void*)_tmp4B1->hd,(void*)_tmp4B7->hd))break;}if(_tmp4B1 == 0
 && _tmp4B7 == 0)return 1;}return 0;_LL336: _tmp4B8=_tmp489.f1;if(_tmp4B8 <= (void*)4)
goto _LL338;if(*((int*)_tmp4B8)!= 4)goto _LL338;_tmp4B9=((struct Cyc_Absyn_PointerType_struct*)
_tmp4B8)->f1;_tmp4BA=(void*)_tmp4B9.elt_typ;_tmp4BB=_tmp4B9.elt_tq;_tmp4BC=
_tmp4B9.ptr_atts;_tmp4BD=(void*)_tmp4BC.rgn;_tmp4BE=_tmp4BC.nullable;_tmp4BF=
_tmp4BC.bounds;_tmp4C0=_tmp4BC.zero_term;_tmp4C1=_tmp489.f2;if(_tmp4C1 <= (void*)
4)goto _LL338;if(*((int*)_tmp4C1)!= 2)goto _LL338;_tmp4C2=((struct Cyc_Absyn_TunionType_struct*)
_tmp4C1)->f1;_tmp4C3=_tmp4C2.tunion_info;if((((((struct Cyc_Absyn_TunionType_struct*)
_tmp4C1)->f1).tunion_info).KnownTunion).tag != 1)goto _LL338;_tmp4C4=(_tmp4C3.KnownTunion).f1;
_tmp4C5=*_tmp4C4;_tmp4C6=_tmp4C2.targs;_tmp4C7=_tmp4C2.rgn;if(!(!_tmp4C5->is_flat))
goto _LL338;_LL337:{void*_tmp4E6=Cyc_Tcutil_compress(_tmp4BA);struct Cyc_Absyn_TunionFieldInfo
_tmp4E7;union Cyc_Absyn_TunionFieldInfoU_union _tmp4E8;struct Cyc_Absyn_Tuniondecl*
_tmp4E9;struct Cyc_Absyn_Tunionfield*_tmp4EA;struct Cyc_List_List*_tmp4EB;_LL352:
if(_tmp4E6 <= (void*)4)goto _LL354;if(*((int*)_tmp4E6)!= 3)goto _LL354;_tmp4E7=((
struct Cyc_Absyn_TunionFieldType_struct*)_tmp4E6)->f1;_tmp4E8=_tmp4E7.field_info;
if((((((struct Cyc_Absyn_TunionFieldType_struct*)_tmp4E6)->f1).field_info).KnownTunionfield).tag
!= 1)goto _LL354;_tmp4E9=(_tmp4E8.KnownTunionfield).f1;_tmp4EA=(_tmp4E8.KnownTunionfield).f2;
_tmp4EB=_tmp4E7.targs;_LL353: if(!Cyc_Tcutil_unify(_tmp4BD,(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp4C7))->v) && !Cyc_Tcenv_region_outlives(te,_tmp4BD,(void*)
_tmp4C7->v))return 0;if(!((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,
struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp4BE,Cyc_Absyn_false_conref))
return 0;if(!Cyc_Tcutil_unify_conrefs(Cyc_Tcutil_unify_it_bounds,_tmp4BF,Cyc_Absyn_bounds_one_conref))
return 0;if(!((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*
y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp4C0,Cyc_Absyn_false_conref))
return 0;if(Cyc_Absyn_qvar_cmp(_tmp4C5->name,_tmp4E9->name)== 0  && _tmp4EA->typs
!= 0){int okay=1;for(0;_tmp4EB != 0  && _tmp4C6 != 0;(_tmp4EB=_tmp4EB->tl,_tmp4C6=
_tmp4C6->tl)){if(!Cyc_Tcutil_unify((void*)_tmp4EB->hd,(void*)_tmp4C6->hd)){okay=
0;break;}}if((!okay  || _tmp4EB != 0) || _tmp4C6 != 0)return 0;return 1;}goto _LL351;
_LL354:;_LL355: goto _LL351;_LL351:;}return 0;_LL338: _tmp4C8=_tmp489.f1;if(_tmp4C8
<= (void*)4)goto _LL33A;if(*((int*)_tmp4C8)!= 18)goto _LL33A;_tmp4C9=_tmp489.f2;
if(_tmp4C9 <= (void*)4)goto _LL33A;if(*((int*)_tmp4C9)!= 5)goto _LL33A;_LL339:
return 0;_LL33A:;_LL33B: return Cyc_Tcutil_unify(t1,t2);_LL32D:;}}int Cyc_Tcutil_is_pointer_type(
void*t);int Cyc_Tcutil_is_pointer_type(void*t){void*_tmp4EC=Cyc_Tcutil_compress(t);
_LL357: if(_tmp4EC <= (void*)4)goto _LL359;if(*((int*)_tmp4EC)!= 4)goto _LL359;
_LL358: return 1;_LL359:;_LL35A: return 0;_LL356:;}int Cyc_Tcutil_is_pointer_or_boxed(
void*t,int*is_dyneither_ptr);int Cyc_Tcutil_is_pointer_or_boxed(void*t,int*
is_dyneither_ptr){void*_tmp4ED=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo
_tmp4EE;struct Cyc_Absyn_PtrAtts _tmp4EF;struct Cyc_Absyn_Conref*_tmp4F0;_LL35C: if(
_tmp4ED <= (void*)4)goto _LL35E;if(*((int*)_tmp4ED)!= 4)goto _LL35E;_tmp4EE=((
struct Cyc_Absyn_PointerType_struct*)_tmp4ED)->f1;_tmp4EF=_tmp4EE.ptr_atts;
_tmp4F0=_tmp4EF.bounds;_LL35D:*is_dyneither_ptr=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,
_tmp4F0)== (void*)0;return 1;_LL35E:;_LL35F: return Cyc_Tcutil_typ_kind(t)== (void*)
2;_LL35B:;}int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*e);int Cyc_Tcutil_is_zero(
struct Cyc_Absyn_Exp*e){void*_tmp4F1=(void*)e->r;union Cyc_Absyn_Cnst_union _tmp4F2;
int _tmp4F3;union Cyc_Absyn_Cnst_union _tmp4F4;char _tmp4F5;union Cyc_Absyn_Cnst_union
_tmp4F6;short _tmp4F7;union Cyc_Absyn_Cnst_union _tmp4F8;long long _tmp4F9;void*
_tmp4FA;struct Cyc_Absyn_Exp*_tmp4FB;_LL361: if(*((int*)_tmp4F1)!= 0)goto _LL363;
_tmp4F2=((struct Cyc_Absyn_Const_e_struct*)_tmp4F1)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)
_tmp4F1)->f1).Int_c).tag != 2)goto _LL363;_tmp4F3=(_tmp4F2.Int_c).f2;if(_tmp4F3 != 
0)goto _LL363;_LL362: goto _LL364;_LL363: if(*((int*)_tmp4F1)!= 0)goto _LL365;_tmp4F4=((
struct Cyc_Absyn_Const_e_struct*)_tmp4F1)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)
_tmp4F1)->f1).Char_c).tag != 0)goto _LL365;_tmp4F5=(_tmp4F4.Char_c).f2;if(_tmp4F5
!= 0)goto _LL365;_LL364: goto _LL366;_LL365: if(*((int*)_tmp4F1)!= 0)goto _LL367;
_tmp4F6=((struct Cyc_Absyn_Const_e_struct*)_tmp4F1)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)
_tmp4F1)->f1).Short_c).tag != 1)goto _LL367;_tmp4F7=(_tmp4F6.Short_c).f2;if(
_tmp4F7 != 0)goto _LL367;_LL366: goto _LL368;_LL367: if(*((int*)_tmp4F1)!= 0)goto
_LL369;_tmp4F8=((struct Cyc_Absyn_Const_e_struct*)_tmp4F1)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)
_tmp4F1)->f1).LongLong_c).tag != 3)goto _LL369;_tmp4F9=(_tmp4F8.LongLong_c).f2;if(
_tmp4F9 != 0)goto _LL369;_LL368: return 1;_LL369: if(*((int*)_tmp4F1)!= 15)goto _LL36B;
_tmp4FA=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp4F1)->f1;_tmp4FB=((struct Cyc_Absyn_Cast_e_struct*)
_tmp4F1)->f2;_LL36A: return Cyc_Tcutil_is_zero(_tmp4FB) && Cyc_Tcutil_admits_zero(
_tmp4FA);_LL36B:;_LL36C: return 0;_LL360:;}struct Cyc_Core_Opt Cyc_Tcutil_trk={(void*)((
void*)5)};struct Cyc_Core_Opt Cyc_Tcutil_urk={(void*)((void*)4)};struct Cyc_Core_Opt
Cyc_Tcutil_rk={(void*)((void*)3)};struct Cyc_Core_Opt Cyc_Tcutil_ak={(void*)((void*)
0)};struct Cyc_Core_Opt Cyc_Tcutil_bk={(void*)((void*)2)};struct Cyc_Core_Opt Cyc_Tcutil_mk={(
void*)((void*)1)};struct Cyc_Core_Opt Cyc_Tcutil_ik={(void*)((void*)7)};struct Cyc_Core_Opt
Cyc_Tcutil_ek={(void*)((void*)6)};struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_opt(void*
k);struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_opt(void*k){void*_tmp4FC=k;_LL36E: if((
int)_tmp4FC != 0)goto _LL370;_LL36F: return(struct Cyc_Core_Opt*)& Cyc_Tcutil_ak;
_LL370: if((int)_tmp4FC != 1)goto _LL372;_LL371: return(struct Cyc_Core_Opt*)& Cyc_Tcutil_mk;
_LL372: if((int)_tmp4FC != 2)goto _LL374;_LL373: return(struct Cyc_Core_Opt*)& Cyc_Tcutil_bk;
_LL374: if((int)_tmp4FC != 3)goto _LL376;_LL375: return(struct Cyc_Core_Opt*)& Cyc_Tcutil_rk;
_LL376: if((int)_tmp4FC != 4)goto _LL378;_LL377: return(struct Cyc_Core_Opt*)& Cyc_Tcutil_urk;
_LL378: if((int)_tmp4FC != 5)goto _LL37A;_LL379: return(struct Cyc_Core_Opt*)& Cyc_Tcutil_trk;
_LL37A: if((int)_tmp4FC != 6)goto _LL37C;_LL37B: return(struct Cyc_Core_Opt*)& Cyc_Tcutil_ek;
_LL37C: if((int)_tmp4FC != 7)goto _LL36D;_LL37D: return(struct Cyc_Core_Opt*)& Cyc_Tcutil_ik;
_LL36D:;}static void**Cyc_Tcutil_kind_to_b(void*k);static void**Cyc_Tcutil_kind_to_b(
void*k){static struct Cyc_Absyn_Eq_kb_struct ab_v={0,(void*)((void*)0)};static
struct Cyc_Absyn_Eq_kb_struct mb_v={0,(void*)((void*)1)};static struct Cyc_Absyn_Eq_kb_struct
bb_v={0,(void*)((void*)2)};static struct Cyc_Absyn_Eq_kb_struct rb_v={0,(void*)((
void*)3)};static struct Cyc_Absyn_Eq_kb_struct ub_v={0,(void*)((void*)4)};static
struct Cyc_Absyn_Eq_kb_struct tb_v={0,(void*)((void*)5)};static struct Cyc_Absyn_Eq_kb_struct
eb_v={0,(void*)((void*)6)};static struct Cyc_Absyn_Eq_kb_struct ib_v={0,(void*)((
void*)7)};static void*ab=(void*)& ab_v;static void*mb=(void*)& mb_v;static void*bb=(
void*)& bb_v;static void*rb=(void*)& rb_v;static void*ub=(void*)& ub_v;static void*tb=(
void*)& tb_v;static void*eb=(void*)& eb_v;static void*ib=(void*)& ib_v;void*_tmp4FD=k;
_LL37F: if((int)_tmp4FD != 0)goto _LL381;_LL380: return& ab;_LL381: if((int)_tmp4FD != 
1)goto _LL383;_LL382: return& mb;_LL383: if((int)_tmp4FD != 2)goto _LL385;_LL384:
return& bb;_LL385: if((int)_tmp4FD != 3)goto _LL387;_LL386: return& rb;_LL387: if((int)
_tmp4FD != 4)goto _LL389;_LL388: return& ub;_LL389: if((int)_tmp4FD != 5)goto _LL38B;
_LL38A: return& tb;_LL38B: if((int)_tmp4FD != 6)goto _LL38D;_LL38C: return& eb;_LL38D:
if((int)_tmp4FD != 7)goto _LL37E;_LL38E: return& ib;_LL37E:;}void*Cyc_Tcutil_kind_to_bound(
void*k);void*Cyc_Tcutil_kind_to_bound(void*k){return*Cyc_Tcutil_kind_to_b(k);}
struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_bound_opt(void*k);struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_bound_opt(
void*k){return(struct Cyc_Core_Opt*)Cyc_Tcutil_kind_to_b(k);}int Cyc_Tcutil_zero_to_null(
struct Cyc_Tcenv_Tenv*te,void*t2,struct Cyc_Absyn_Exp*e1);int Cyc_Tcutil_zero_to_null(
struct Cyc_Tcenv_Tenv*te,void*t2,struct Cyc_Absyn_Exp*e1){if(Cyc_Tcutil_is_pointer_type(
t2) && Cyc_Tcutil_is_zero(e1)){{struct Cyc_Absyn_Const_e_struct _tmpD70;union Cyc_Absyn_Cnst_union
_tmpD6F;struct Cyc_Absyn_Const_e_struct*_tmpD6E;(void*)(e1->r=(void*)((void*)((
_tmpD6E=_cycalloc(sizeof(*_tmpD6E)),((_tmpD6E[0]=((_tmpD70.tag=0,((_tmpD70.f1=(
union Cyc_Absyn_Cnst_union)(((_tmpD6F.Null_c).tag=6,_tmpD6F)),_tmpD70)))),_tmpD6E))))));}{
struct Cyc_Core_Opt*_tmp509=Cyc_Tcenv_lookup_opt_type_vars(te);struct Cyc_Absyn_PointerType_struct
_tmpD7A;struct Cyc_Absyn_PtrAtts _tmpD79;struct Cyc_Absyn_PtrInfo _tmpD78;struct Cyc_Absyn_PointerType_struct*
_tmpD77;struct Cyc_Absyn_PointerType_struct*_tmp50A=(_tmpD77=_cycalloc(sizeof(*
_tmpD77)),((_tmpD77[0]=((_tmpD7A.tag=4,((_tmpD7A.f1=((_tmpD78.elt_typ=(void*)Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_ak,_tmp509),((_tmpD78.elt_tq=Cyc_Absyn_empty_tqual(
0),((_tmpD78.ptr_atts=((_tmpD79.rgn=(void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_trk,_tmp509),((_tmpD79.nullable=Cyc_Absyn_true_conref,((_tmpD79.bounds=
Cyc_Absyn_empty_conref(),((_tmpD79.zero_term=((struct Cyc_Absyn_Conref*(*)())Cyc_Absyn_empty_conref)(),((
_tmpD79.ptrloc=0,_tmpD79)))))))))),_tmpD78)))))),_tmpD7A)))),_tmpD77)));(void*)(((
struct Cyc_Core_Opt*)_check_null(e1->topt))->v=(void*)((void*)_tmp50A));return Cyc_Tcutil_coerce_arg(
te,e1,t2);}}return 0;}struct _dyneither_ptr Cyc_Tcutil_coercion2string(void*c);
struct _dyneither_ptr Cyc_Tcutil_coercion2string(void*c){void*_tmp50F=c;_LL390: if((
int)_tmp50F != 0)goto _LL392;_LL391: {const char*_tmpD7B;return(_tmpD7B="unknown",
_tag_dyneither(_tmpD7B,sizeof(char),8));}_LL392: if((int)_tmp50F != 1)goto _LL394;
_LL393: {const char*_tmpD7C;return(_tmpD7C="no coercion",_tag_dyneither(_tmpD7C,
sizeof(char),12));}_LL394: if((int)_tmp50F != 2)goto _LL396;_LL395: {const char*
_tmpD7D;return(_tmpD7D="null check",_tag_dyneither(_tmpD7D,sizeof(char),11));}
_LL396: if((int)_tmp50F != 3)goto _LL38F;_LL397: {const char*_tmpD7E;return(_tmpD7E="other coercion",
_tag_dyneither(_tmpD7E,sizeof(char),15));}_LL38F:;}int Cyc_Tcutil_coerce_arg(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t2);int Cyc_Tcutil_coerce_arg(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t2){void*t1=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);void*c;if(Cyc_Tcutil_unify(
t1,t2))return 1;if(Cyc_Tcutil_is_arithmetic_type(t2) && Cyc_Tcutil_is_arithmetic_type(
t1)){if(Cyc_Tcutil_will_lose_precision(t1,t2)){const char*_tmpD83;void*_tmpD82[2];
struct Cyc_String_pa_struct _tmpD81;struct Cyc_String_pa_struct _tmpD80;(_tmpD80.tag=
0,((_tmpD80.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t2)),((_tmpD81.tag=0,((_tmpD81.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t1)),((_tmpD82[0]=& _tmpD81,((_tmpD82[1]=& _tmpD80,Cyc_Tcutil_warn(
e->loc,((_tmpD83="integral size mismatch; %s -> %s conversion supplied",
_tag_dyneither(_tmpD83,sizeof(char),53))),_tag_dyneither(_tmpD82,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_unchecked_cast(te,e,t2,(void*)1);return 1;}else{if(Cyc_Tcutil_silent_castable(
te,e->loc,t1,t2)){Cyc_Tcutil_unchecked_cast(te,e,t2,(void*)3);return 1;}else{if(
Cyc_Tcutil_zero_to_null(te,t2,e))return 1;else{if((c=Cyc_Tcutil_castable(te,e->loc,
t1,t2))!= (void*)0){Cyc_Tcutil_unchecked_cast(te,e,t2,c);if(c != (void*)2){const
char*_tmpD88;void*_tmpD87[2];struct Cyc_String_pa_struct _tmpD86;struct Cyc_String_pa_struct
_tmpD85;(_tmpD85.tag=0,((_tmpD85.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2)),((_tmpD86.tag=0,((_tmpD86.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmpD87[0]=& _tmpD86,((_tmpD87[
1]=& _tmpD85,Cyc_Tcutil_warn(e->loc,((_tmpD88="implicit cast from %s to %s",
_tag_dyneither(_tmpD88,sizeof(char),28))),_tag_dyneither(_tmpD87,sizeof(void*),2)))))))))))));}
return 1;}else{return 0;}}}}}int Cyc_Tcutil_coerce_assign(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Absyn_Exp*e,void*t);int Cyc_Tcutil_coerce_assign(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e,void*t){return Cyc_Tcutil_coerce_arg(te,e,t);}int Cyc_Tcutil_coerceable(
void*t);int Cyc_Tcutil_coerceable(void*t){void*_tmp51C=Cyc_Tcutil_compress(t);
_LL399: if(_tmp51C <= (void*)4)goto _LL39B;if(*((int*)_tmp51C)!= 5)goto _LL39B;
_LL39A: goto _LL39C;_LL39B: if((int)_tmp51C != 1)goto _LL39D;_LL39C: goto _LL39E;_LL39D:
if(_tmp51C <= (void*)4)goto _LL39F;if(*((int*)_tmp51C)!= 6)goto _LL39F;_LL39E:
return 1;_LL39F:;_LL3A0: return 0;_LL398:;}static struct _tuple5*Cyc_Tcutil_flatten_typ_f(
struct _tuple9*env,struct Cyc_Absyn_Aggrfield*x);static struct _tuple5*Cyc_Tcutil_flatten_typ_f(
struct _tuple9*env,struct Cyc_Absyn_Aggrfield*x){struct Cyc_List_List*_tmp51E;
struct _RegionHandle*_tmp51F;struct _tuple9 _tmp51D=*env;_tmp51E=_tmp51D.f1;_tmp51F=
_tmp51D.f2;{struct _tuple5*_tmpD89;return(_tmpD89=_region_malloc(_tmp51F,sizeof(*
_tmpD89)),((_tmpD89->f1=x->tq,((_tmpD89->f2=Cyc_Tcutil_rsubstitute(_tmp51F,
_tmp51E,(void*)x->type),_tmpD89)))));}}static struct _tuple5*Cyc_Tcutil_rcopy_tqt(
struct _RegionHandle*r,struct _tuple5*x);static struct _tuple5*Cyc_Tcutil_rcopy_tqt(
struct _RegionHandle*r,struct _tuple5*x){struct Cyc_Absyn_Tqual _tmp522;void*_tmp523;
struct _tuple5 _tmp521=*x;_tmp522=_tmp521.f1;_tmp523=_tmp521.f2;{struct _tuple5*
_tmpD8A;return(_tmpD8A=_region_malloc(r,sizeof(*_tmpD8A)),((_tmpD8A->f1=_tmp522,((
_tmpD8A->f2=_tmp523,_tmpD8A)))));}}static struct Cyc_List_List*Cyc_Tcutil_flatten_typ(
struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,void*t1);static struct Cyc_List_List*
Cyc_Tcutil_flatten_typ(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,void*t1){t1=
Cyc_Tcutil_compress(t1);{void*_tmp525=t1;struct Cyc_List_List*_tmp526;struct Cyc_Absyn_AggrInfo
_tmp527;union Cyc_Absyn_AggrInfoU_union _tmp528;struct Cyc_Absyn_Aggrdecl**_tmp529;
struct Cyc_Absyn_Aggrdecl*_tmp52A;struct Cyc_List_List*_tmp52B;void*_tmp52C;struct
Cyc_List_List*_tmp52D;struct Cyc_Absyn_FnInfo _tmp52E;_LL3A2: if((int)_tmp525 != 0)
goto _LL3A4;_LL3A3: return 0;_LL3A4: if(_tmp525 <= (void*)4)goto _LL3AC;if(*((int*)
_tmp525)!= 9)goto _LL3A6;_tmp526=((struct Cyc_Absyn_TupleType_struct*)_tmp525)->f1;
_LL3A5: return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple5*(*f)(
struct _RegionHandle*,struct _tuple5*),struct _RegionHandle*env,struct Cyc_List_List*
x))Cyc_List_rmap_c)(r,Cyc_Tcutil_rcopy_tqt,r,_tmp526);_LL3A6: if(*((int*)_tmp525)
!= 10)goto _LL3A8;_tmp527=((struct Cyc_Absyn_AggrType_struct*)_tmp525)->f1;_tmp528=
_tmp527.aggr_info;if((((((struct Cyc_Absyn_AggrType_struct*)_tmp525)->f1).aggr_info).KnownAggr).tag
!= 1)goto _LL3A8;_tmp529=(_tmp528.KnownAggr).f1;_tmp52A=*_tmp529;_tmp52B=_tmp527.targs;
_LL3A7: if((((void*)_tmp52A->kind == (void*)1  || _tmp52A->impl == 0) || ((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp52A->impl))->exist_vars != 0) || ((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp52A->impl))->rgn_po != 0){struct _tuple5*_tmpD8D;struct Cyc_List_List*
_tmpD8C;return(_tmpD8C=_region_malloc(r,sizeof(*_tmpD8C)),((_tmpD8C->hd=((
_tmpD8D=_region_malloc(r,sizeof(*_tmpD8D)),((_tmpD8D->f1=Cyc_Absyn_empty_tqual(0),((
_tmpD8D->f2=t1,_tmpD8D)))))),((_tmpD8C->tl=0,_tmpD8C)))));}{struct Cyc_List_List*
_tmp531=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp52A->tvs,
_tmp52B);struct _tuple9 _tmpD8E;struct _tuple9 env=(_tmpD8E.f1=_tmp531,((_tmpD8E.f2=
r,_tmpD8E)));return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple5*(*
f)(struct _tuple9*,struct Cyc_Absyn_Aggrfield*),struct _tuple9*env,struct Cyc_List_List*
x))Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_typ_f,& env,((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp52A->impl))->fields);}_LL3A8: if(*((int*)_tmp525)!= 11)goto _LL3AA;
_tmp52C=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)_tmp525)->f1;if((int)
_tmp52C != 0)goto _LL3AA;_tmp52D=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp525)->f2;
_LL3A9: {struct _tuple9 _tmpD8F;struct _tuple9 env=(_tmpD8F.f1=0,((_tmpD8F.f2=r,
_tmpD8F)));return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple5*(*
f)(struct _tuple9*,struct Cyc_Absyn_Aggrfield*),struct _tuple9*env,struct Cyc_List_List*
x))Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_typ_f,& env,_tmp52D);}_LL3AA: if(*((int*)
_tmp525)!= 8)goto _LL3AC;_tmp52E=((struct Cyc_Absyn_FnType_struct*)_tmp525)->f1;
_LL3AB: {struct _tuple5*_tmpD92;struct Cyc_List_List*_tmpD91;return(_tmpD91=
_region_malloc(r,sizeof(*_tmpD91)),((_tmpD91->hd=((_tmpD92=_region_malloc(r,
sizeof(*_tmpD92)),((_tmpD92->f1=Cyc_Absyn_const_tqual(0),((_tmpD92->f2=t1,
_tmpD92)))))),((_tmpD91->tl=0,_tmpD91)))));}_LL3AC:;_LL3AD: {struct _tuple5*
_tmpD95;struct Cyc_List_List*_tmpD94;return(_tmpD94=_region_malloc(r,sizeof(*
_tmpD94)),((_tmpD94->hd=((_tmpD95=_region_malloc(r,sizeof(*_tmpD95)),((_tmpD95->f1=
Cyc_Absyn_empty_tqual(0),((_tmpD95->f2=t1,_tmpD95)))))),((_tmpD94->tl=0,_tmpD94)))));}
_LL3A1:;}}static int Cyc_Tcutil_sub_attributes(struct Cyc_List_List*a1,struct Cyc_List_List*
a2);static int Cyc_Tcutil_sub_attributes(struct Cyc_List_List*a1,struct Cyc_List_List*
a2){{struct Cyc_List_List*t=a1;for(0;t != 0;t=t->tl){void*_tmp538=(void*)t->hd;
_LL3AF: if((int)_tmp538 != 16)goto _LL3B1;_LL3B0: goto _LL3B2;_LL3B1: if((int)_tmp538
!= 3)goto _LL3B3;_LL3B2: goto _LL3B4;_LL3B3: if(_tmp538 <= (void*)17)goto _LL3B5;if(*((
int*)_tmp538)!= 4)goto _LL3B5;_LL3B4: continue;_LL3B5:;_LL3B6: if(!Cyc_List_exists_c(
Cyc_Tcutil_equal_att,(void*)t->hd,a2))return 0;_LL3AE:;}}for(0;a2 != 0;a2=a2->tl){
if(!Cyc_List_exists_c(Cyc_Tcutil_equal_att,(void*)a2->hd,a1))return 0;}return 1;}
static int Cyc_Tcutil_ptrsubtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*
assume,void*t1,void*t2);static int Cyc_Tcutil_subtype(struct Cyc_Tcenv_Tenv*te,
struct Cyc_List_List*assume,void*t1,void*t2);static int Cyc_Tcutil_subtype(struct
Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2){if(Cyc_Tcutil_unify(
t1,t2))return 1;{struct Cyc_List_List*a=assume;for(0;a != 0;a=a->tl){if(Cyc_Tcutil_unify(
t1,(*((struct _tuple0*)a->hd)).f1) && Cyc_Tcutil_unify(t2,(*((struct _tuple0*)a->hd)).f2))
return 1;}}t1=Cyc_Tcutil_compress(t1);t2=Cyc_Tcutil_compress(t2);{struct _tuple0
_tmpD96;struct _tuple0 _tmp53A=(_tmpD96.f1=t1,((_tmpD96.f2=t2,_tmpD96)));void*
_tmp53B;struct Cyc_Absyn_PtrInfo _tmp53C;void*_tmp53D;struct Cyc_Absyn_Tqual _tmp53E;
struct Cyc_Absyn_PtrAtts _tmp53F;void*_tmp540;struct Cyc_Absyn_Conref*_tmp541;
struct Cyc_Absyn_Conref*_tmp542;struct Cyc_Absyn_Conref*_tmp543;void*_tmp544;
struct Cyc_Absyn_PtrInfo _tmp545;void*_tmp546;struct Cyc_Absyn_Tqual _tmp547;struct
Cyc_Absyn_PtrAtts _tmp548;void*_tmp549;struct Cyc_Absyn_Conref*_tmp54A;struct Cyc_Absyn_Conref*
_tmp54B;struct Cyc_Absyn_Conref*_tmp54C;void*_tmp54D;struct Cyc_Absyn_TunionInfo
_tmp54E;union Cyc_Absyn_TunionInfoU_union _tmp54F;struct Cyc_Absyn_Tuniondecl**
_tmp550;struct Cyc_Absyn_Tuniondecl*_tmp551;struct Cyc_List_List*_tmp552;struct Cyc_Core_Opt*
_tmp553;struct Cyc_Core_Opt _tmp554;void*_tmp555;void*_tmp556;struct Cyc_Absyn_TunionInfo
_tmp557;union Cyc_Absyn_TunionInfoU_union _tmp558;struct Cyc_Absyn_Tuniondecl**
_tmp559;struct Cyc_Absyn_Tuniondecl*_tmp55A;struct Cyc_List_List*_tmp55B;struct Cyc_Core_Opt*
_tmp55C;struct Cyc_Core_Opt _tmp55D;void*_tmp55E;void*_tmp55F;struct Cyc_Absyn_FnInfo
_tmp560;void*_tmp561;struct Cyc_Absyn_FnInfo _tmp562;_LL3B8: _tmp53B=_tmp53A.f1;if(
_tmp53B <= (void*)4)goto _LL3BA;if(*((int*)_tmp53B)!= 4)goto _LL3BA;_tmp53C=((
struct Cyc_Absyn_PointerType_struct*)_tmp53B)->f1;_tmp53D=(void*)_tmp53C.elt_typ;
_tmp53E=_tmp53C.elt_tq;_tmp53F=_tmp53C.ptr_atts;_tmp540=(void*)_tmp53F.rgn;
_tmp541=_tmp53F.nullable;_tmp542=_tmp53F.bounds;_tmp543=_tmp53F.zero_term;
_tmp544=_tmp53A.f2;if(_tmp544 <= (void*)4)goto _LL3BA;if(*((int*)_tmp544)!= 4)goto
_LL3BA;_tmp545=((struct Cyc_Absyn_PointerType_struct*)_tmp544)->f1;_tmp546=(void*)
_tmp545.elt_typ;_tmp547=_tmp545.elt_tq;_tmp548=_tmp545.ptr_atts;_tmp549=(void*)
_tmp548.rgn;_tmp54A=_tmp548.nullable;_tmp54B=_tmp548.bounds;_tmp54C=_tmp548.zero_term;
_LL3B9: if(_tmp53E.real_const  && !_tmp547.real_const)return 0;if((!((int(*)(int(*
cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(
Cyc_Core_intcmp,_tmp541,_tmp54A) && ((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,_tmp541)) && !((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,
_tmp54A))return 0;if((!((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,
struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp543,
_tmp54C) && !((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,
_tmp543)) && ((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,
_tmp54C))return 0;if(!Cyc_Tcutil_unify(_tmp540,_tmp549) && !Cyc_Tcenv_region_outlives(
te,_tmp540,_tmp549))return 0;if(!Cyc_Tcutil_unify_conrefs(Cyc_Tcutil_unify_it_bounds,
_tmp542,_tmp54B)){struct _tuple0 _tmpD97;struct _tuple0 _tmp564=(_tmpD97.f1=Cyc_Absyn_conref_val(
_tmp542),((_tmpD97.f2=Cyc_Absyn_conref_val(_tmp54B),_tmpD97)));void*_tmp565;void*
_tmp566;void*_tmp567;struct Cyc_Absyn_Exp*_tmp568;void*_tmp569;struct Cyc_Absyn_Exp*
_tmp56A;_LL3C1: _tmp565=_tmp564.f1;if(_tmp565 <= (void*)1)goto _LL3C3;if(*((int*)
_tmp565)!= 0)goto _LL3C3;_tmp566=_tmp564.f2;if((int)_tmp566 != 0)goto _LL3C3;_LL3C2:
goto _LL3C0;_LL3C3: _tmp567=_tmp564.f1;if(_tmp567 <= (void*)1)goto _LL3C5;if(*((int*)
_tmp567)!= 0)goto _LL3C5;_tmp568=((struct Cyc_Absyn_Upper_b_struct*)_tmp567)->f1;
_tmp569=_tmp564.f2;if(_tmp569 <= (void*)1)goto _LL3C5;if(*((int*)_tmp569)!= 0)goto
_LL3C5;_tmp56A=((struct Cyc_Absyn_Upper_b_struct*)_tmp569)->f1;_LL3C4: if(!Cyc_Evexp_lte_const_exp(
_tmp56A,_tmp568))return 0;goto _LL3C0;_LL3C5:;_LL3C6: return 0;_LL3C0:;}{struct
_tuple0*_tmpD9A;struct Cyc_List_List*_tmpD99;return Cyc_Tcutil_ptrsubtype(te,((
_tmpD99=_cycalloc(sizeof(*_tmpD99)),((_tmpD99->hd=((_tmpD9A=_cycalloc(sizeof(*
_tmpD9A)),((_tmpD9A->f1=t1,((_tmpD9A->f2=t2,_tmpD9A)))))),((_tmpD99->tl=assume,
_tmpD99)))))),_tmp53D,_tmp546);}_LL3BA: _tmp54D=_tmp53A.f1;if(_tmp54D <= (void*)4)
goto _LL3BC;if(*((int*)_tmp54D)!= 2)goto _LL3BC;_tmp54E=((struct Cyc_Absyn_TunionType_struct*)
_tmp54D)->f1;_tmp54F=_tmp54E.tunion_info;if((((((struct Cyc_Absyn_TunionType_struct*)
_tmp54D)->f1).tunion_info).KnownTunion).tag != 1)goto _LL3BC;_tmp550=(_tmp54F.KnownTunion).f1;
_tmp551=*_tmp550;_tmp552=_tmp54E.targs;_tmp553=_tmp54E.rgn;if(_tmp553 == 0)goto
_LL3BC;_tmp554=*_tmp553;_tmp555=(void*)_tmp554.v;_tmp556=_tmp53A.f2;if(_tmp556 <= (
void*)4)goto _LL3BC;if(*((int*)_tmp556)!= 2)goto _LL3BC;_tmp557=((struct Cyc_Absyn_TunionType_struct*)
_tmp556)->f1;_tmp558=_tmp557.tunion_info;if((((((struct Cyc_Absyn_TunionType_struct*)
_tmp556)->f1).tunion_info).KnownTunion).tag != 1)goto _LL3BC;_tmp559=(_tmp558.KnownTunion).f1;
_tmp55A=*_tmp559;_tmp55B=_tmp557.targs;_tmp55C=_tmp557.rgn;if(_tmp55C == 0)goto
_LL3BC;_tmp55D=*_tmp55C;_tmp55E=(void*)_tmp55D.v;_LL3BB: if(_tmp551 != _tmp55A  || 
!Cyc_Tcenv_region_outlives(te,_tmp555,_tmp55E))return 0;for(0;_tmp552 != 0  && 
_tmp55B != 0;(_tmp552=_tmp552->tl,_tmp55B=_tmp55B->tl)){if(!Cyc_Tcutil_unify((
void*)_tmp552->hd,(void*)_tmp55B->hd))return 0;}if(_tmp552 != 0  || _tmp55B != 0)
return 0;return 1;_LL3BC: _tmp55F=_tmp53A.f1;if(_tmp55F <= (void*)4)goto _LL3BE;if(*((
int*)_tmp55F)!= 8)goto _LL3BE;_tmp560=((struct Cyc_Absyn_FnType_struct*)_tmp55F)->f1;
_tmp561=_tmp53A.f2;if(_tmp561 <= (void*)4)goto _LL3BE;if(*((int*)_tmp561)!= 8)goto
_LL3BE;_tmp562=((struct Cyc_Absyn_FnType_struct*)_tmp561)->f1;_LL3BD: if(_tmp560.tvars
!= 0  || _tmp562.tvars != 0){struct Cyc_List_List*_tmp56D=_tmp560.tvars;struct Cyc_List_List*
_tmp56E=_tmp562.tvars;if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp56D)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp56E))return 0;{
struct _RegionHandle _tmp56F=_new_region("r");struct _RegionHandle*r=& _tmp56F;
_push_region(r);{struct Cyc_List_List*inst=0;while(_tmp56D != 0){if(Cyc_Tcutil_tvar_kind((
struct Cyc_Absyn_Tvar*)_tmp56D->hd)!= Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*)_check_null(_tmp56E))->hd)){int _tmp570=0;_npop_handler(0);
return _tmp570;}{struct _tuple8*_tmpDA4;struct Cyc_Absyn_VarType_struct _tmpDA3;
struct Cyc_Absyn_VarType_struct*_tmpDA2;struct Cyc_List_List*_tmpDA1;inst=((
_tmpDA1=_region_malloc(r,sizeof(*_tmpDA1)),((_tmpDA1->hd=((_tmpDA4=
_region_malloc(r,sizeof(*_tmpDA4)),((_tmpDA4->f1=(struct Cyc_Absyn_Tvar*)_tmp56E->hd,((
_tmpDA4->f2=(void*)((_tmpDA2=_cycalloc(sizeof(*_tmpDA2)),((_tmpDA2[0]=((_tmpDA3.tag=
1,((_tmpDA3.f1=(struct Cyc_Absyn_Tvar*)_tmp56D->hd,_tmpDA3)))),_tmpDA2)))),
_tmpDA4)))))),((_tmpDA1->tl=inst,_tmpDA1))))));}_tmp56D=_tmp56D->tl;_tmp56E=
_tmp56E->tl;}if(inst != 0){_tmp560.tvars=0;_tmp562.tvars=0;{struct Cyc_Absyn_FnType_struct
_tmpDAA;struct Cyc_Absyn_FnType_struct*_tmpDA9;struct Cyc_Absyn_FnType_struct
_tmpDA7;struct Cyc_Absyn_FnType_struct*_tmpDA6;int _tmp579=Cyc_Tcutil_subtype(te,
assume,(void*)((_tmpDA6=_cycalloc(sizeof(*_tmpDA6)),((_tmpDA6[0]=((_tmpDA7.tag=8,((
_tmpDA7.f1=_tmp560,_tmpDA7)))),_tmpDA6)))),(void*)((_tmpDA9=_cycalloc(sizeof(*
_tmpDA9)),((_tmpDA9[0]=((_tmpDAA.tag=8,((_tmpDAA.f1=_tmp562,_tmpDAA)))),_tmpDA9)))));
_npop_handler(0);return _tmp579;}}};_pop_region(r);}}if(!Cyc_Tcutil_subtype(te,
assume,(void*)_tmp560.ret_typ,(void*)_tmp562.ret_typ))return 0;{struct Cyc_List_List*
_tmp57A=_tmp560.args;struct Cyc_List_List*_tmp57B=_tmp562.args;if(((int(*)(struct
Cyc_List_List*x))Cyc_List_length)(_tmp57A)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp57B))return 0;for(0;_tmp57A != 0;(_tmp57A=_tmp57A->tl,_tmp57B=_tmp57B->tl)){
struct Cyc_Absyn_Tqual _tmp57D;void*_tmp57E;struct _tuple3 _tmp57C=*((struct _tuple3*)
_tmp57A->hd);_tmp57D=_tmp57C.f2;_tmp57E=_tmp57C.f3;{struct Cyc_Absyn_Tqual _tmp580;
void*_tmp581;struct _tuple3 _tmp57F=*((struct _tuple3*)((struct Cyc_List_List*)
_check_null(_tmp57B))->hd);_tmp580=_tmp57F.f2;_tmp581=_tmp57F.f3;if(_tmp580.real_const
 && !_tmp57D.real_const  || !Cyc_Tcutil_subtype(te,assume,_tmp581,_tmp57E))return
0;}}if(_tmp560.c_varargs != _tmp562.c_varargs)return 0;if(_tmp560.cyc_varargs != 0
 && _tmp562.cyc_varargs != 0){struct Cyc_Absyn_VarargInfo _tmp582=*_tmp560.cyc_varargs;
struct Cyc_Absyn_VarargInfo _tmp583=*_tmp562.cyc_varargs;if((_tmp583.tq).real_const
 && !(_tmp582.tq).real_const  || !Cyc_Tcutil_subtype(te,assume,(void*)_tmp583.type,(
void*)_tmp582.type))return 0;}else{if(_tmp560.cyc_varargs != 0  || _tmp562.cyc_varargs
!= 0)return 0;}if(!Cyc_Tcutil_subset_effect(1,(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp560.effect))->v,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp562.effect))->v))
return 0;if(!Cyc_Tcutil_sub_rgnpo(_tmp560.rgn_po,_tmp562.rgn_po))return 0;if(!Cyc_Tcutil_sub_attributes(
_tmp560.attributes,_tmp562.attributes))return 0;return 1;}_LL3BE:;_LL3BF: return 0;
_LL3B7:;}}static int Cyc_Tcutil_isomorphic(void*t1,void*t2);static int Cyc_Tcutil_isomorphic(
void*t1,void*t2){struct _tuple0 _tmpDAB;struct _tuple0 _tmp585=(_tmpDAB.f1=Cyc_Tcutil_compress(
t1),((_tmpDAB.f2=Cyc_Tcutil_compress(t2),_tmpDAB)));void*_tmp586;void*_tmp587;
void*_tmp588;void*_tmp589;_LL3C8: _tmp586=_tmp585.f1;if(_tmp586 <= (void*)4)goto
_LL3CA;if(*((int*)_tmp586)!= 5)goto _LL3CA;_tmp587=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp586)->f2;_tmp588=_tmp585.f2;if(_tmp588 <= (void*)4)goto _LL3CA;if(*((int*)
_tmp588)!= 5)goto _LL3CA;_tmp589=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp588)->f2;
_LL3C9: return(_tmp587 == _tmp589  || _tmp587 == (void*)2  && _tmp589 == (void*)3) || 
_tmp587 == (void*)3  && _tmp589 == (void*)2;_LL3CA:;_LL3CB: return 0;_LL3C7:;}static
int Cyc_Tcutil_ptrsubtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*
t1,void*t2);static int Cyc_Tcutil_ptrsubtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*
assume,void*t1,void*t2){struct _RegionHandle _tmp58A=_new_region("temp");struct
_RegionHandle*temp=& _tmp58A;_push_region(temp);{struct Cyc_List_List*tqs1=Cyc_Tcutil_flatten_typ(
temp,te,t1);struct Cyc_List_List*tqs2=Cyc_Tcutil_flatten_typ(temp,te,t2);for(0;
tqs2 != 0;(tqs2=tqs2->tl,tqs1=tqs1->tl)){if(tqs1 == 0){int _tmp58B=0;_npop_handler(
0);return _tmp58B;}{struct _tuple5 _tmp58D;struct Cyc_Absyn_Tqual _tmp58E;void*
_tmp58F;struct _tuple5*_tmp58C=(struct _tuple5*)tqs1->hd;_tmp58D=*_tmp58C;_tmp58E=
_tmp58D.f1;_tmp58F=_tmp58D.f2;{struct _tuple5 _tmp591;struct Cyc_Absyn_Tqual _tmp592;
void*_tmp593;struct _tuple5*_tmp590=(struct _tuple5*)tqs2->hd;_tmp591=*_tmp590;
_tmp592=_tmp591.f1;_tmp593=_tmp591.f2;if(_tmp592.real_const  && Cyc_Tcutil_subtype(
te,assume,_tmp58F,_tmp593))continue;else{if(Cyc_Tcutil_unify(_tmp58F,_tmp593))
continue;else{if(Cyc_Tcutil_isomorphic(_tmp58F,_tmp593))continue;else{int _tmp594=
0;_npop_handler(0);return _tmp594;}}}}}}{int _tmp595=1;_npop_handler(0);return
_tmp595;}};_pop_region(temp);}static int Cyc_Tcutil_is_char_type(void*t);static int
Cyc_Tcutil_is_char_type(void*t){void*_tmp596=Cyc_Tcutil_compress(t);void*_tmp597;
_LL3CD: if(_tmp596 <= (void*)4)goto _LL3CF;if(*((int*)_tmp596)!= 5)goto _LL3CF;
_tmp597=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp596)->f2;if((int)_tmp597 != 
0)goto _LL3CF;_LL3CE: return 1;_LL3CF:;_LL3D0: return 0;_LL3CC:;}void*Cyc_Tcutil_castable(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void*t1,void*t2);void*Cyc_Tcutil_castable(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void*t1,void*t2){if(Cyc_Tcutil_unify(
t1,t2))return(void*)1;t1=Cyc_Tcutil_compress(t1);t2=Cyc_Tcutil_compress(t2);if(
t2 == (void*)0)return(void*)1;{void*_tmp598=t2;void*_tmp599;void*_tmp59A;_LL3D2:
if(_tmp598 <= (void*)4)goto _LL3D6;if(*((int*)_tmp598)!= 5)goto _LL3D4;_tmp599=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp598)->f2;if((int)_tmp599 != 2)goto
_LL3D4;_LL3D3: goto _LL3D5;_LL3D4: if(*((int*)_tmp598)!= 5)goto _LL3D6;_tmp59A=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp598)->f2;if((int)_tmp59A != 3)goto _LL3D6;
_LL3D5: if(Cyc_Tcutil_typ_kind(t1)== (void*)2)return(void*)1;goto _LL3D1;_LL3D6:;
_LL3D7: goto _LL3D1;_LL3D1:;}{void*_tmp59B=t1;struct Cyc_Absyn_PtrInfo _tmp59C;void*
_tmp59D;struct Cyc_Absyn_Tqual _tmp59E;struct Cyc_Absyn_PtrAtts _tmp59F;void*_tmp5A0;
struct Cyc_Absyn_Conref*_tmp5A1;struct Cyc_Absyn_Conref*_tmp5A2;struct Cyc_Absyn_Conref*
_tmp5A3;struct Cyc_Absyn_ArrayInfo _tmp5A4;void*_tmp5A5;struct Cyc_Absyn_Tqual
_tmp5A6;struct Cyc_Absyn_Exp*_tmp5A7;struct Cyc_Absyn_Conref*_tmp5A8;struct Cyc_Absyn_Enumdecl*
_tmp5A9;_LL3D9: if(_tmp59B <= (void*)4)goto _LL3E1;if(*((int*)_tmp59B)!= 4)goto
_LL3DB;_tmp59C=((struct Cyc_Absyn_PointerType_struct*)_tmp59B)->f1;_tmp59D=(void*)
_tmp59C.elt_typ;_tmp59E=_tmp59C.elt_tq;_tmp59F=_tmp59C.ptr_atts;_tmp5A0=(void*)
_tmp59F.rgn;_tmp5A1=_tmp59F.nullable;_tmp5A2=_tmp59F.bounds;_tmp5A3=_tmp59F.zero_term;
_LL3DA:{void*_tmp5AA=t2;struct Cyc_Absyn_PtrInfo _tmp5AB;void*_tmp5AC;struct Cyc_Absyn_Tqual
_tmp5AD;struct Cyc_Absyn_PtrAtts _tmp5AE;void*_tmp5AF;struct Cyc_Absyn_Conref*
_tmp5B0;struct Cyc_Absyn_Conref*_tmp5B1;struct Cyc_Absyn_Conref*_tmp5B2;_LL3E8: if(
_tmp5AA <= (void*)4)goto _LL3EA;if(*((int*)_tmp5AA)!= 4)goto _LL3EA;_tmp5AB=((
struct Cyc_Absyn_PointerType_struct*)_tmp5AA)->f1;_tmp5AC=(void*)_tmp5AB.elt_typ;
_tmp5AD=_tmp5AB.elt_tq;_tmp5AE=_tmp5AB.ptr_atts;_tmp5AF=(void*)_tmp5AE.rgn;
_tmp5B0=_tmp5AE.nullable;_tmp5B1=_tmp5AE.bounds;_tmp5B2=_tmp5AE.zero_term;_LL3E9: {
void*coercion=(void*)3;struct _tuple0*_tmpDAE;struct Cyc_List_List*_tmpDAD;struct
Cyc_List_List*_tmp5B3=(_tmpDAD=_cycalloc(sizeof(*_tmpDAD)),((_tmpDAD->hd=((
_tmpDAE=_cycalloc(sizeof(*_tmpDAE)),((_tmpDAE->f1=t1,((_tmpDAE->f2=t2,_tmpDAE)))))),((
_tmpDAD->tl=0,_tmpDAD)))));int _tmp5B4=Cyc_Tcutil_ptrsubtype(te,_tmp5B3,_tmp59D,
_tmp5AC) && (!_tmp59E.real_const  || _tmp5AD.real_const);Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;{int zeroterm_ok=((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*
x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp5A3,
_tmp5B2) || !((int(*)(struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_val)(_tmp5B2);
int _tmp5B5=_tmp5B4?0:((Cyc_Tcutil_bits_only(_tmp59D) && Cyc_Tcutil_is_char_type(
_tmp5AC)) && !((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,
_tmp5B2)) && (_tmp5AD.real_const  || !_tmp59E.real_const);int bounds_ok=Cyc_Tcutil_unify_conrefs(
Cyc_Tcutil_unify_it_bounds,_tmp5A2,_tmp5B1);if(!bounds_ok  && !_tmp5B5){struct
_tuple0 _tmpDAF;struct _tuple0 _tmp5B7=(_tmpDAF.f1=Cyc_Absyn_conref_val(_tmp5A2),((
_tmpDAF.f2=Cyc_Absyn_conref_val(_tmp5B1),_tmpDAF)));void*_tmp5B8;struct Cyc_Absyn_Exp*
_tmp5B9;void*_tmp5BA;struct Cyc_Absyn_Exp*_tmp5BB;_LL3ED: _tmp5B8=_tmp5B7.f1;if(
_tmp5B8 <= (void*)1)goto _LL3EF;if(*((int*)_tmp5B8)!= 0)goto _LL3EF;_tmp5B9=((
struct Cyc_Absyn_Upper_b_struct*)_tmp5B8)->f1;_tmp5BA=_tmp5B7.f2;if(_tmp5BA <= (
void*)1)goto _LL3EF;if(*((int*)_tmp5BA)!= 0)goto _LL3EF;_tmp5BB=((struct Cyc_Absyn_Upper_b_struct*)
_tmp5BA)->f1;_LL3EE: if(Cyc_Evexp_lte_const_exp(_tmp5BB,_tmp5B9))bounds_ok=1;goto
_LL3EC;_LL3EF:;_LL3F0: bounds_ok=1;goto _LL3EC;_LL3EC:;}if(((int(*)(int,struct Cyc_Absyn_Conref*
x))Cyc_Absyn_conref_def)(0,_tmp5A1) && !((int(*)(int,struct Cyc_Absyn_Conref*x))
Cyc_Absyn_conref_def)(0,_tmp5B0))coercion=(void*)2;if(((bounds_ok  && zeroterm_ok)
 && (_tmp5B4  || _tmp5B5)) && (Cyc_Tcutil_unify(_tmp5A0,_tmp5AF) || Cyc_Tcenv_region_outlives(
te,_tmp5A0,_tmp5AF)))return coercion;else{return(void*)0;}}}_LL3EA:;_LL3EB: goto
_LL3E7;_LL3E7:;}return(void*)0;_LL3DB: if(*((int*)_tmp59B)!= 7)goto _LL3DD;_tmp5A4=((
struct Cyc_Absyn_ArrayType_struct*)_tmp59B)->f1;_tmp5A5=(void*)_tmp5A4.elt_type;
_tmp5A6=_tmp5A4.tq;_tmp5A7=_tmp5A4.num_elts;_tmp5A8=_tmp5A4.zero_term;_LL3DC:{
void*_tmp5BE=t2;struct Cyc_Absyn_ArrayInfo _tmp5BF;void*_tmp5C0;struct Cyc_Absyn_Tqual
_tmp5C1;struct Cyc_Absyn_Exp*_tmp5C2;struct Cyc_Absyn_Conref*_tmp5C3;_LL3F2: if(
_tmp5BE <= (void*)4)goto _LL3F4;if(*((int*)_tmp5BE)!= 7)goto _LL3F4;_tmp5BF=((
struct Cyc_Absyn_ArrayType_struct*)_tmp5BE)->f1;_tmp5C0=(void*)_tmp5BF.elt_type;
_tmp5C1=_tmp5BF.tq;_tmp5C2=_tmp5BF.num_elts;_tmp5C3=_tmp5BF.zero_term;_LL3F3: {
int okay;okay=((_tmp5A7 != 0  && _tmp5C2 != 0) && ((int(*)(int(*cmp)(int,int),struct
Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,
_tmp5A8,_tmp5C3)) && Cyc_Evexp_lte_const_exp((struct Cyc_Absyn_Exp*)_tmp5C2,(
struct Cyc_Absyn_Exp*)_tmp5A7);return(okay  && Cyc_Tcutil_unify(_tmp5A5,_tmp5C0))
 && (!_tmp5A6.real_const  || _tmp5C1.real_const)?(void*)3:(void*)0;}_LL3F4:;
_LL3F5: return(void*)0;_LL3F1:;}return(void*)0;_LL3DD: if(*((int*)_tmp59B)!= 12)
goto _LL3DF;_tmp5A9=((struct Cyc_Absyn_EnumType_struct*)_tmp59B)->f2;_LL3DE:{void*
_tmp5C4=t2;struct Cyc_Absyn_Enumdecl*_tmp5C5;_LL3F7: if(_tmp5C4 <= (void*)4)goto
_LL3F9;if(*((int*)_tmp5C4)!= 12)goto _LL3F9;_tmp5C5=((struct Cyc_Absyn_EnumType_struct*)
_tmp5C4)->f2;_LL3F8: if((_tmp5A9->fields != 0  && _tmp5C5->fields != 0) && ((int(*)(
struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp5A9->fields))->v)>= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)((
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp5C5->fields))->v))
return(void*)1;goto _LL3F6;_LL3F9:;_LL3FA: goto _LL3F6;_LL3F6:;}goto _LL3E0;_LL3DF:
if(*((int*)_tmp59B)!= 5)goto _LL3E1;_LL3E0: goto _LL3E2;_LL3E1: if((int)_tmp59B != 1)
goto _LL3E3;_LL3E2: goto _LL3E4;_LL3E3: if(_tmp59B <= (void*)4)goto _LL3E5;if(*((int*)
_tmp59B)!= 6)goto _LL3E5;_LL3E4: return Cyc_Tcutil_coerceable(t2)?(void*)1:(void*)0;
_LL3E5:;_LL3E6: return(void*)0;_LL3D8:;}}void Cyc_Tcutil_unchecked_cast(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e,void*t,void*c);void Cyc_Tcutil_unchecked_cast(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e,void*t,void*c){if(!Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,t)){struct Cyc_Absyn_Exp*_tmp5C6=Cyc_Absyn_copy_exp(e);{
struct Cyc_Absyn_Cast_e_struct _tmpDB2;struct Cyc_Absyn_Cast_e_struct*_tmpDB1;(void*)(
e->r=(void*)((void*)((_tmpDB1=_cycalloc(sizeof(*_tmpDB1)),((_tmpDB1[0]=((_tmpDB2.tag=
15,((_tmpDB2.f1=(void*)t,((_tmpDB2.f2=_tmp5C6,((_tmpDB2.f3=0,((_tmpDB2.f4=(void*)
c,_tmpDB2)))))))))),_tmpDB1))))));}{struct Cyc_Core_Opt*_tmpDB3;e->topt=((_tmpDB3=
_cycalloc(sizeof(*_tmpDB3)),((_tmpDB3->v=(void*)t,_tmpDB3))));}}}int Cyc_Tcutil_is_integral(
struct Cyc_Absyn_Exp*e);int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*e){void*
_tmp5CA=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
_LL3FC: if(_tmp5CA <= (void*)4)goto _LL406;if(*((int*)_tmp5CA)!= 5)goto _LL3FE;
_LL3FD: goto _LL3FF;_LL3FE: if(*((int*)_tmp5CA)!= 12)goto _LL400;_LL3FF: goto _LL401;
_LL400: if(*((int*)_tmp5CA)!= 13)goto _LL402;_LL401: goto _LL403;_LL402: if(*((int*)
_tmp5CA)!= 18)goto _LL404;_LL403: return 1;_LL404: if(*((int*)_tmp5CA)!= 0)goto
_LL406;_LL405: return Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,
Cyc_Absyn_sint_typ);_LL406:;_LL407: return 0;_LL3FB:;}int Cyc_Tcutil_is_numeric(
struct Cyc_Absyn_Exp*e);int Cyc_Tcutil_is_numeric(struct Cyc_Absyn_Exp*e){if(Cyc_Tcutil_is_integral(
e))return 1;{void*_tmp5CB=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);_LL409: if((int)_tmp5CB != 1)goto _LL40B;_LL40A: goto
_LL40C;_LL40B: if(_tmp5CB <= (void*)4)goto _LL40D;if(*((int*)_tmp5CB)!= 6)goto
_LL40D;_LL40C: return 1;_LL40D:;_LL40E: return 0;_LL408:;}}int Cyc_Tcutil_is_function_type(
void*t);int Cyc_Tcutil_is_function_type(void*t){void*_tmp5CC=Cyc_Tcutil_compress(
t);_LL410: if(_tmp5CC <= (void*)4)goto _LL412;if(*((int*)_tmp5CC)!= 8)goto _LL412;
_LL411: return 1;_LL412:;_LL413: return 0;_LL40F:;}void*Cyc_Tcutil_max_arithmetic_type(
void*t1,void*t2);void*Cyc_Tcutil_max_arithmetic_type(void*t1,void*t2){struct
_tuple0 _tmpDB4;struct _tuple0 _tmp5CE=(_tmpDB4.f1=t1,((_tmpDB4.f2=t2,_tmpDB4)));
void*_tmp5CF;int _tmp5D0;void*_tmp5D1;int _tmp5D2;void*_tmp5D3;void*_tmp5D4;void*
_tmp5D5;void*_tmp5D6;void*_tmp5D7;void*_tmp5D8;void*_tmp5D9;void*_tmp5DA;void*
_tmp5DB;void*_tmp5DC;void*_tmp5DD;void*_tmp5DE;void*_tmp5DF;void*_tmp5E0;void*
_tmp5E1;void*_tmp5E2;void*_tmp5E3;void*_tmp5E4;void*_tmp5E5;void*_tmp5E6;void*
_tmp5E7;void*_tmp5E8;void*_tmp5E9;void*_tmp5EA;void*_tmp5EB;void*_tmp5EC;void*
_tmp5ED;void*_tmp5EE;void*_tmp5EF;void*_tmp5F0;_LL415: _tmp5CF=_tmp5CE.f1;if(
_tmp5CF <= (void*)4)goto _LL417;if(*((int*)_tmp5CF)!= 6)goto _LL417;_tmp5D0=((
struct Cyc_Absyn_DoubleType_struct*)_tmp5CF)->f1;_tmp5D1=_tmp5CE.f2;if(_tmp5D1 <= (
void*)4)goto _LL417;if(*((int*)_tmp5D1)!= 6)goto _LL417;_tmp5D2=((struct Cyc_Absyn_DoubleType_struct*)
_tmp5D1)->f1;_LL416: if(_tmp5D0)return t1;else{return t2;}_LL417: _tmp5D3=_tmp5CE.f1;
if(_tmp5D3 <= (void*)4)goto _LL419;if(*((int*)_tmp5D3)!= 6)goto _LL419;_LL418:
return t1;_LL419: _tmp5D4=_tmp5CE.f2;if(_tmp5D4 <= (void*)4)goto _LL41B;if(*((int*)
_tmp5D4)!= 6)goto _LL41B;_LL41A: return t2;_LL41B: _tmp5D5=_tmp5CE.f1;if((int)
_tmp5D5 != 1)goto _LL41D;_LL41C: goto _LL41E;_LL41D: _tmp5D6=_tmp5CE.f2;if((int)
_tmp5D6 != 1)goto _LL41F;_LL41E: return(void*)1;_LL41F: _tmp5D7=_tmp5CE.f1;if(
_tmp5D7 <= (void*)4)goto _LL421;if(*((int*)_tmp5D7)!= 5)goto _LL421;_tmp5D8=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp5D7)->f1;if((int)_tmp5D8 != 1)goto _LL421;
_tmp5D9=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp5D7)->f2;if((int)_tmp5D9 != 
4)goto _LL421;_LL420: goto _LL422;_LL421: _tmp5DA=_tmp5CE.f2;if(_tmp5DA <= (void*)4)
goto _LL423;if(*((int*)_tmp5DA)!= 5)goto _LL423;_tmp5DB=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp5DA)->f1;if((int)_tmp5DB != 1)goto _LL423;_tmp5DC=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp5DA)->f2;if((int)_tmp5DC != 4)goto _LL423;_LL422: return Cyc_Absyn_ulonglong_typ;
_LL423: _tmp5DD=_tmp5CE.f1;if(_tmp5DD <= (void*)4)goto _LL425;if(*((int*)_tmp5DD)!= 
5)goto _LL425;_tmp5DE=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp5DD)->f2;if((
int)_tmp5DE != 4)goto _LL425;_LL424: goto _LL426;_LL425: _tmp5DF=_tmp5CE.f2;if(
_tmp5DF <= (void*)4)goto _LL427;if(*((int*)_tmp5DF)!= 5)goto _LL427;_tmp5E0=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp5DF)->f2;if((int)_tmp5E0 != 4)goto _LL427;
_LL426: return Cyc_Absyn_slonglong_typ;_LL427: _tmp5E1=_tmp5CE.f1;if(_tmp5E1 <= (
void*)4)goto _LL429;if(*((int*)_tmp5E1)!= 5)goto _LL429;_tmp5E2=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp5E1)->f1;if((int)_tmp5E2 != 1)goto _LL429;_tmp5E3=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp5E1)->f2;if((int)_tmp5E3 != 3)goto _LL429;_LL428: goto _LL42A;_LL429: _tmp5E4=
_tmp5CE.f2;if(_tmp5E4 <= (void*)4)goto _LL42B;if(*((int*)_tmp5E4)!= 5)goto _LL42B;
_tmp5E5=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp5E4)->f1;if((int)_tmp5E5 != 
1)goto _LL42B;_tmp5E6=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp5E4)->f2;if((
int)_tmp5E6 != 3)goto _LL42B;_LL42A: return Cyc_Absyn_ulong_typ;_LL42B: _tmp5E7=
_tmp5CE.f1;if(_tmp5E7 <= (void*)4)goto _LL42D;if(*((int*)_tmp5E7)!= 5)goto _LL42D;
_tmp5E8=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp5E7)->f1;if((int)_tmp5E8 != 
1)goto _LL42D;_tmp5E9=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp5E7)->f2;if((
int)_tmp5E9 != 2)goto _LL42D;_LL42C: goto _LL42E;_LL42D: _tmp5EA=_tmp5CE.f2;if(
_tmp5EA <= (void*)4)goto _LL42F;if(*((int*)_tmp5EA)!= 5)goto _LL42F;_tmp5EB=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp5EA)->f1;if((int)_tmp5EB != 1)goto _LL42F;
_tmp5EC=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp5EA)->f2;if((int)_tmp5EC != 
2)goto _LL42F;_LL42E: return Cyc_Absyn_uint_typ;_LL42F: _tmp5ED=_tmp5CE.f1;if(
_tmp5ED <= (void*)4)goto _LL431;if(*((int*)_tmp5ED)!= 5)goto _LL431;_tmp5EE=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp5ED)->f2;if((int)_tmp5EE != 3)goto _LL431;
_LL430: goto _LL432;_LL431: _tmp5EF=_tmp5CE.f2;if(_tmp5EF <= (void*)4)goto _LL433;if(*((
int*)_tmp5EF)!= 5)goto _LL433;_tmp5F0=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp5EF)->f2;if((int)_tmp5F0 != 3)goto _LL433;_LL432: return Cyc_Absyn_slong_typ;
_LL433:;_LL434: return Cyc_Absyn_sint_typ;_LL414:;}void Cyc_Tcutil_check_contains_assign(
struct Cyc_Absyn_Exp*e);void Cyc_Tcutil_check_contains_assign(struct Cyc_Absyn_Exp*
e){void*_tmp5F1=(void*)e->r;struct Cyc_Core_Opt*_tmp5F2;_LL436: if(*((int*)_tmp5F1)
!= 4)goto _LL438;_tmp5F2=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp5F1)->f2;if(
_tmp5F2 != 0)goto _LL438;_LL437:{const char*_tmpDB7;void*_tmpDB6;(_tmpDB6=0,Cyc_Tcutil_warn(
e->loc,((_tmpDB7="assignment in test",_tag_dyneither(_tmpDB7,sizeof(char),19))),
_tag_dyneither(_tmpDB6,sizeof(void*),0)));}goto _LL435;_LL438:;_LL439: goto _LL435;
_LL435:;}static int Cyc_Tcutil_constrain_kinds(void*c1,void*c2);static int Cyc_Tcutil_constrain_kinds(
void*c1,void*c2){c1=Cyc_Absyn_compress_kb(c1);c2=Cyc_Absyn_compress_kb(c2);{
struct _tuple0 _tmpDB8;struct _tuple0 _tmp5F6=(_tmpDB8.f1=c1,((_tmpDB8.f2=c2,_tmpDB8)));
void*_tmp5F7;void*_tmp5F8;void*_tmp5F9;void*_tmp5FA;void*_tmp5FB;struct Cyc_Core_Opt*
_tmp5FC;struct Cyc_Core_Opt**_tmp5FD;void*_tmp5FE;struct Cyc_Core_Opt*_tmp5FF;
struct Cyc_Core_Opt**_tmp600;void*_tmp601;struct Cyc_Core_Opt*_tmp602;struct Cyc_Core_Opt**
_tmp603;void*_tmp604;void*_tmp605;void*_tmp606;void*_tmp607;void*_tmp608;void*
_tmp609;struct Cyc_Core_Opt*_tmp60A;struct Cyc_Core_Opt**_tmp60B;void*_tmp60C;void*
_tmp60D;struct Cyc_Core_Opt*_tmp60E;struct Cyc_Core_Opt**_tmp60F;void*_tmp610;void*
_tmp611;struct Cyc_Core_Opt*_tmp612;struct Cyc_Core_Opt**_tmp613;void*_tmp614;
_LL43B: _tmp5F7=_tmp5F6.f1;if(*((int*)_tmp5F7)!= 0)goto _LL43D;_tmp5F8=(void*)((
struct Cyc_Absyn_Eq_kb_struct*)_tmp5F7)->f1;_tmp5F9=_tmp5F6.f2;if(*((int*)_tmp5F9)
!= 0)goto _LL43D;_tmp5FA=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp5F9)->f1;
_LL43C: return _tmp5F8 == _tmp5FA;_LL43D: _tmp5FB=_tmp5F6.f2;if(*((int*)_tmp5FB)!= 1)
goto _LL43F;_tmp5FC=((struct Cyc_Absyn_Unknown_kb_struct*)_tmp5FB)->f1;_tmp5FD=(
struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)_tmp5FB)->f1;_LL43E:{
struct Cyc_Core_Opt*_tmpDB9;*_tmp5FD=((_tmpDB9=_cycalloc(sizeof(*_tmpDB9)),((
_tmpDB9->v=(void*)c1,_tmpDB9))));}return 1;_LL43F: _tmp5FE=_tmp5F6.f1;if(*((int*)
_tmp5FE)!= 1)goto _LL441;_tmp5FF=((struct Cyc_Absyn_Unknown_kb_struct*)_tmp5FE)->f1;
_tmp600=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)_tmp5FE)->f1;
_LL440:{struct Cyc_Core_Opt*_tmpDBA;*_tmp600=((_tmpDBA=_cycalloc(sizeof(*_tmpDBA)),((
_tmpDBA->v=(void*)c2,_tmpDBA))));}return 1;_LL441: _tmp601=_tmp5F6.f1;if(*((int*)
_tmp601)!= 2)goto _LL443;_tmp602=((struct Cyc_Absyn_Less_kb_struct*)_tmp601)->f1;
_tmp603=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)_tmp601)->f1;
_tmp604=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp601)->f2;_tmp605=_tmp5F6.f2;
if(*((int*)_tmp605)!= 0)goto _LL443;_tmp606=(void*)((struct Cyc_Absyn_Eq_kb_struct*)
_tmp605)->f1;_LL442: if(Cyc_Tcutil_kind_leq(_tmp606,_tmp604)){{struct Cyc_Core_Opt*
_tmpDBB;*_tmp603=((_tmpDBB=_cycalloc(sizeof(*_tmpDBB)),((_tmpDBB->v=(void*)c2,
_tmpDBB))));}return 1;}else{return 0;}_LL443: _tmp607=_tmp5F6.f1;if(*((int*)_tmp607)
!= 0)goto _LL445;_tmp608=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp607)->f1;
_tmp609=_tmp5F6.f2;if(*((int*)_tmp609)!= 2)goto _LL445;_tmp60A=((struct Cyc_Absyn_Less_kb_struct*)
_tmp609)->f1;_tmp60B=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp609)->f1;_tmp60C=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp609)->f2;
_LL444: if(Cyc_Tcutil_kind_leq(_tmp608,_tmp60C)){{struct Cyc_Core_Opt*_tmpDBC;*
_tmp60B=((_tmpDBC=_cycalloc(sizeof(*_tmpDBC)),((_tmpDBC->v=(void*)c1,_tmpDBC))));}
return 1;}else{return 0;}_LL445: _tmp60D=_tmp5F6.f1;if(*((int*)_tmp60D)!= 2)goto
_LL43A;_tmp60E=((struct Cyc_Absyn_Less_kb_struct*)_tmp60D)->f1;_tmp60F=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*)_tmp60D)->f1;_tmp610=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp60D)->f2;_tmp611=_tmp5F6.f2;if(*((int*)_tmp611)!= 2)goto _LL43A;_tmp612=((
struct Cyc_Absyn_Less_kb_struct*)_tmp611)->f1;_tmp613=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*)_tmp611)->f1;_tmp614=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp611)->f2;_LL446: if(Cyc_Tcutil_kind_leq(_tmp610,_tmp614)){{struct Cyc_Core_Opt*
_tmpDBD;*_tmp613=((_tmpDBD=_cycalloc(sizeof(*_tmpDBD)),((_tmpDBD->v=(void*)c1,
_tmpDBD))));}return 1;}else{if(Cyc_Tcutil_kind_leq(_tmp614,_tmp610)){{struct Cyc_Core_Opt*
_tmpDBE;*_tmp60F=((_tmpDBE=_cycalloc(sizeof(*_tmpDBE)),((_tmpDBE->v=(void*)c2,
_tmpDBE))));}return 1;}else{return 0;}}_LL43A:;}}static int Cyc_Tcutil_tvar_id_counter=
0;int Cyc_Tcutil_new_tvar_id();int Cyc_Tcutil_new_tvar_id(){return Cyc_Tcutil_tvar_id_counter
++;}static int Cyc_Tcutil_tvar_counter=0;struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(
void*k);struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k){int i=Cyc_Tcutil_tvar_counter
++;const char*_tmpDC2;void*_tmpDC1[1];struct Cyc_Int_pa_struct _tmpDC0;struct
_dyneither_ptr s=(struct _dyneither_ptr)((_tmpDC0.tag=1,((_tmpDC0.f1=(
unsigned long)i,((_tmpDC1[0]=& _tmpDC0,Cyc_aprintf(((_tmpDC2="#%d",_tag_dyneither(
_tmpDC2,sizeof(char),4))),_tag_dyneither(_tmpDC1,sizeof(void*),1))))))));struct
_dyneither_ptr*_tmpDC5;struct Cyc_Absyn_Tvar*_tmpDC4;return(_tmpDC4=_cycalloc(
sizeof(*_tmpDC4)),((_tmpDC4->name=((_tmpDC5=_cycalloc(sizeof(struct
_dyneither_ptr)* 1),((_tmpDC5[0]=s,_tmpDC5)))),((_tmpDC4->identity=- 1,((_tmpDC4->kind=(
void*)k,_tmpDC4)))))));}int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*t);int
Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*t){struct _dyneither_ptr _tmp620=*t->name;
return*((const char*)_check_dyneither_subscript(_tmp620,sizeof(char),0))== '#';}
void Cyc_Tcutil_rewrite_temp_tvar(struct Cyc_Absyn_Tvar*t);void Cyc_Tcutil_rewrite_temp_tvar(
struct Cyc_Absyn_Tvar*t){{const char*_tmpDC9;void*_tmpDC8[1];struct Cyc_String_pa_struct
_tmpDC7;(_tmpDC7.tag=0,((_tmpDC7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*
t->name),((_tmpDC8[0]=& _tmpDC7,Cyc_printf(((_tmpDC9="%s",_tag_dyneither(_tmpDC9,
sizeof(char),3))),_tag_dyneither(_tmpDC8,sizeof(void*),1)))))));}if(!Cyc_Tcutil_is_temp_tvar(
t))return;{const char*_tmpDCA;struct _dyneither_ptr _tmp624=Cyc_strconcat(((_tmpDCA="`",
_tag_dyneither(_tmpDCA,sizeof(char),2))),(struct _dyneither_ptr)*t->name);{char
_tmpDCD;char _tmpDCC;struct _dyneither_ptr _tmpDCB;(_tmpDCB=_dyneither_ptr_plus(
_tmp624,sizeof(char),1),((_tmpDCC=*((char*)_check_dyneither_subscript(_tmpDCB,
sizeof(char),0)),((_tmpDCD='t',((_get_dyneither_size(_tmpDCB,sizeof(char))== 1
 && (_tmpDCC == '\000'  && _tmpDCD != '\000')?_throw_arraybounds(): 1,*((char*)
_tmpDCB.curr)=_tmpDCD)))))));}{struct _dyneither_ptr*_tmpDCE;t->name=((_tmpDCE=
_cycalloc(sizeof(struct _dyneither_ptr)* 1),((_tmpDCE[0]=(struct _dyneither_ptr)
_tmp624,_tmpDCE))));}}}struct _tuple14{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual
f2;void*f3;};static struct _tuple3*Cyc_Tcutil_fndecl2typ_f(struct _tuple14*x);
static struct _tuple3*Cyc_Tcutil_fndecl2typ_f(struct _tuple14*x){struct Cyc_Core_Opt*
_tmpDD1;struct _tuple3*_tmpDD0;return(_tmpDD0=_cycalloc(sizeof(*_tmpDD0)),((
_tmpDD0->f1=(struct Cyc_Core_Opt*)((_tmpDD1=_cycalloc(sizeof(*_tmpDD1)),((_tmpDD1->v=(*
x).f1,_tmpDD1)))),((_tmpDD0->f2=(*x).f2,((_tmpDD0->f3=(*x).f3,_tmpDD0)))))));}
void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*fd);void*Cyc_Tcutil_fndecl2typ(
struct Cyc_Absyn_Fndecl*fd){if(fd->cached_typ == 0){struct Cyc_List_List*_tmp62C=0;{
struct Cyc_List_List*atts=fd->attributes;for(0;atts != 0;atts=atts->tl){if(Cyc_Absyn_fntype_att((
void*)atts->hd)){struct Cyc_List_List*_tmpDD2;_tmp62C=((_tmpDD2=_cycalloc(sizeof(*
_tmpDD2)),((_tmpDD2->hd=(void*)((void*)atts->hd),((_tmpDD2->tl=_tmp62C,_tmpDD2))))));}}}{
struct Cyc_Absyn_FnType_struct _tmpDD8;struct Cyc_Absyn_FnInfo _tmpDD7;struct Cyc_Absyn_FnType_struct*
_tmpDD6;return(void*)((_tmpDD6=_cycalloc(sizeof(*_tmpDD6)),((_tmpDD6[0]=((
_tmpDD8.tag=8,((_tmpDD8.f1=((_tmpDD7.tvars=fd->tvs,((_tmpDD7.effect=fd->effect,((
_tmpDD7.ret_typ=(void*)((void*)fd->ret_type),((_tmpDD7.args=((struct Cyc_List_List*(*)(
struct _tuple3*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_fndecl2typ_f,
fd->args),((_tmpDD7.c_varargs=fd->c_varargs,((_tmpDD7.cyc_varargs=fd->cyc_varargs,((
_tmpDD7.rgn_po=fd->rgn_po,((_tmpDD7.attributes=_tmp62C,_tmpDD7)))))))))))))))),
_tmpDD8)))),_tmpDD6))));}}return(void*)((struct Cyc_Core_Opt*)_check_null(fd->cached_typ))->v;}
struct _tuple15{void*f1;struct Cyc_Absyn_Tqual f2;void*f3;};static void*Cyc_Tcutil_fst_fdarg(
struct _tuple15*t);static void*Cyc_Tcutil_fst_fdarg(struct _tuple15*t){return(*t).f1;}
void*Cyc_Tcutil_snd_tqt(struct _tuple5*t);void*Cyc_Tcutil_snd_tqt(struct _tuple5*t){
return(*t).f2;}static struct _tuple5*Cyc_Tcutil_map2_tq(struct _tuple5*pr,void*t);
static struct _tuple5*Cyc_Tcutil_map2_tq(struct _tuple5*pr,void*t){struct _tuple5*
_tmpDD9;return(_tmpDD9=_cycalloc(sizeof(*_tmpDD9)),((_tmpDD9->f1=(*pr).f1,((
_tmpDD9->f2=t,_tmpDD9)))));}struct _tuple16{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Tqual
f2;};struct _tuple17{struct _tuple16*f1;void*f2;};static struct _tuple17*Cyc_Tcutil_substitute_f1(
struct _RegionHandle*rgn,struct _tuple3*y);static struct _tuple17*Cyc_Tcutil_substitute_f1(
struct _RegionHandle*rgn,struct _tuple3*y){struct _tuple16*_tmpDDC;struct _tuple17*
_tmpDDB;return(_tmpDDB=_region_malloc(rgn,sizeof(*_tmpDDB)),((_tmpDDB->f1=((
_tmpDDC=_region_malloc(rgn,sizeof(*_tmpDDC)),((_tmpDDC->f1=(*y).f1,((_tmpDDC->f2=(*
y).f2,_tmpDDC)))))),((_tmpDDB->f2=(*y).f3,_tmpDDB)))));}static struct _tuple3*Cyc_Tcutil_substitute_f2(
struct _tuple17*w);static struct _tuple3*Cyc_Tcutil_substitute_f2(struct _tuple17*w){
struct _tuple16*_tmp635;void*_tmp636;struct _tuple17 _tmp634=*w;_tmp635=_tmp634.f1;
_tmp636=_tmp634.f2;{struct Cyc_Core_Opt*_tmp638;struct Cyc_Absyn_Tqual _tmp639;
struct _tuple16 _tmp637=*_tmp635;_tmp638=_tmp637.f1;_tmp639=_tmp637.f2;{struct
_tuple3*_tmpDDD;return(_tmpDDD=_cycalloc(sizeof(*_tmpDDD)),((_tmpDDD->f1=_tmp638,((
_tmpDDD->f2=_tmp639,((_tmpDDD->f3=_tmp636,_tmpDDD)))))));}}}static void*Cyc_Tcutil_field_type(
struct Cyc_Absyn_Aggrfield*f);static void*Cyc_Tcutil_field_type(struct Cyc_Absyn_Aggrfield*
f){return(void*)f->type;}static struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_zip_field_type(
struct Cyc_Absyn_Aggrfield*f,void*t);static struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_zip_field_type(
struct Cyc_Absyn_Aggrfield*f,void*t){struct Cyc_Absyn_Aggrfield*_tmpDDE;return(
_tmpDDE=_cycalloc(sizeof(*_tmpDDE)),((_tmpDDE->name=f->name,((_tmpDDE->tq=f->tq,((
_tmpDDE->type=(void*)t,((_tmpDDE->width=f->width,((_tmpDDE->attributes=f->attributes,
_tmpDDE)))))))))));}static struct Cyc_List_List*Cyc_Tcutil_substs(struct
_RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*ts);static struct
Cyc_Absyn_Exp*Cyc_Tcutil_copye(struct Cyc_Absyn_Exp*old,void*r);static struct Cyc_Absyn_Exp*
Cyc_Tcutil_copye(struct Cyc_Absyn_Exp*old,void*r){struct Cyc_Absyn_Exp*_tmpDDF;
return(_tmpDDF=_cycalloc(sizeof(*_tmpDDF)),((_tmpDDF->topt=old->topt,((_tmpDDF->r=(
void*)r,((_tmpDDF->loc=old->loc,((_tmpDDF->annot=(void*)((void*)old->annot),
_tmpDDF)))))))));}static struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubsexp(struct
_RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_Absyn_Exp*e);static struct Cyc_Absyn_Exp*
Cyc_Tcutil_rsubsexp(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_Absyn_Exp*
e){void*_tmp63D=(void*)e->r;void*_tmp63E;struct Cyc_List_List*_tmp63F;struct Cyc_Absyn_Exp*
_tmp640;struct Cyc_Absyn_Exp*_tmp641;struct Cyc_Absyn_Exp*_tmp642;struct Cyc_Absyn_Exp*
_tmp643;struct Cyc_Absyn_Exp*_tmp644;struct Cyc_Absyn_Exp*_tmp645;struct Cyc_Absyn_Exp*
_tmp646;struct Cyc_Absyn_Exp*_tmp647;struct Cyc_Absyn_Exp*_tmp648;void*_tmp649;
struct Cyc_Absyn_Exp*_tmp64A;int _tmp64B;void*_tmp64C;void*_tmp64D;struct Cyc_Absyn_Exp*
_tmp64E;void*_tmp64F;void*_tmp650;void*_tmp651;_LL448: if(*((int*)_tmp63D)!= 0)
goto _LL44A;_LL449: goto _LL44B;_LL44A: if(*((int*)_tmp63D)!= 33)goto _LL44C;_LL44B:
goto _LL44D;_LL44C: if(*((int*)_tmp63D)!= 34)goto _LL44E;_LL44D: goto _LL44F;_LL44E:
if(*((int*)_tmp63D)!= 1)goto _LL450;_LL44F: return e;_LL450: if(*((int*)_tmp63D)!= 3)
goto _LL452;_tmp63E=(void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp63D)->f1;
_tmp63F=((struct Cyc_Absyn_Primop_e_struct*)_tmp63D)->f2;_LL451: if(((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmp63F)== 1){struct Cyc_Absyn_Exp*
_tmp652=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp63F))->hd;
struct Cyc_Absyn_Exp*_tmp653=Cyc_Tcutil_rsubsexp(r,inst,_tmp652);if(_tmp653 == 
_tmp652)return e;{struct Cyc_Absyn_Primop_e_struct _tmpDE5;struct Cyc_Absyn_Exp*
_tmpDE4[1];struct Cyc_Absyn_Primop_e_struct*_tmpDE3;return Cyc_Tcutil_copye(e,(
void*)((_tmpDE3=_cycalloc(sizeof(*_tmpDE3)),((_tmpDE3[0]=((_tmpDE5.tag=3,((
_tmpDE5.f1=(void*)_tmp63E,((_tmpDE5.f2=((_tmpDE4[0]=_tmp653,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpDE4,sizeof(struct Cyc_Absyn_Exp*),
1)))),_tmpDE5)))))),_tmpDE3)))));}}else{if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp63F)== 2){struct Cyc_Absyn_Exp*_tmp657=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp63F))->hd;struct Cyc_Absyn_Exp*_tmp658=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp63F->tl))->hd;struct Cyc_Absyn_Exp*_tmp659=
Cyc_Tcutil_rsubsexp(r,inst,_tmp657);struct Cyc_Absyn_Exp*_tmp65A=Cyc_Tcutil_rsubsexp(
r,inst,_tmp658);if(_tmp659 == _tmp657  && _tmp65A == _tmp658)return e;{struct Cyc_Absyn_Primop_e_struct
_tmpDEB;struct Cyc_Absyn_Exp*_tmpDEA[2];struct Cyc_Absyn_Primop_e_struct*_tmpDE9;
return Cyc_Tcutil_copye(e,(void*)((_tmpDE9=_cycalloc(sizeof(*_tmpDE9)),((_tmpDE9[
0]=((_tmpDEB.tag=3,((_tmpDEB.f1=(void*)_tmp63E,((_tmpDEB.f2=((_tmpDEA[1]=_tmp65A,((
_tmpDEA[0]=_tmp659,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpDEA,sizeof(struct Cyc_Absyn_Exp*),2)))))),_tmpDEB)))))),
_tmpDE9)))));}}else{const char*_tmpDEE;void*_tmpDED;return(_tmpDED=0,((struct Cyc_Absyn_Exp*(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpDEE="primop does not have 1 or 2 args!",
_tag_dyneither(_tmpDEE,sizeof(char),34))),_tag_dyneither(_tmpDED,sizeof(void*),0)));}}
_LL452: if(*((int*)_tmp63D)!= 6)goto _LL454;_tmp640=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp63D)->f1;_tmp641=((struct Cyc_Absyn_Conditional_e_struct*)_tmp63D)->f2;
_tmp642=((struct Cyc_Absyn_Conditional_e_struct*)_tmp63D)->f3;_LL453: {struct Cyc_Absyn_Exp*
_tmp660=Cyc_Tcutil_rsubsexp(r,inst,_tmp640);struct Cyc_Absyn_Exp*_tmp661=Cyc_Tcutil_rsubsexp(
r,inst,_tmp641);struct Cyc_Absyn_Exp*_tmp662=Cyc_Tcutil_rsubsexp(r,inst,_tmp642);
if((_tmp660 == _tmp640  && _tmp661 == _tmp641) && _tmp662 == _tmp642)return e;{struct
Cyc_Absyn_Conditional_e_struct _tmpDF1;struct Cyc_Absyn_Conditional_e_struct*
_tmpDF0;return Cyc_Tcutil_copye(e,(void*)((_tmpDF0=_cycalloc(sizeof(*_tmpDF0)),((
_tmpDF0[0]=((_tmpDF1.tag=6,((_tmpDF1.f1=_tmp660,((_tmpDF1.f2=_tmp661,((_tmpDF1.f3=
_tmp662,_tmpDF1)))))))),_tmpDF0)))));}}_LL454: if(*((int*)_tmp63D)!= 7)goto _LL456;
_tmp643=((struct Cyc_Absyn_And_e_struct*)_tmp63D)->f1;_tmp644=((struct Cyc_Absyn_And_e_struct*)
_tmp63D)->f2;_LL455: {struct Cyc_Absyn_Exp*_tmp665=Cyc_Tcutil_rsubsexp(r,inst,
_tmp643);struct Cyc_Absyn_Exp*_tmp666=Cyc_Tcutil_rsubsexp(r,inst,_tmp644);if(
_tmp665 == _tmp643  && _tmp666 == _tmp644)return e;{struct Cyc_Absyn_And_e_struct
_tmpDF4;struct Cyc_Absyn_And_e_struct*_tmpDF3;return Cyc_Tcutil_copye(e,(void*)((
_tmpDF3=_cycalloc(sizeof(*_tmpDF3)),((_tmpDF3[0]=((_tmpDF4.tag=7,((_tmpDF4.f1=
_tmp665,((_tmpDF4.f2=_tmp666,_tmpDF4)))))),_tmpDF3)))));}}_LL456: if(*((int*)
_tmp63D)!= 8)goto _LL458;_tmp645=((struct Cyc_Absyn_Or_e_struct*)_tmp63D)->f1;
_tmp646=((struct Cyc_Absyn_Or_e_struct*)_tmp63D)->f2;_LL457: {struct Cyc_Absyn_Exp*
_tmp669=Cyc_Tcutil_rsubsexp(r,inst,_tmp645);struct Cyc_Absyn_Exp*_tmp66A=Cyc_Tcutil_rsubsexp(
r,inst,_tmp646);if(_tmp669 == _tmp645  && _tmp66A == _tmp646)return e;{struct Cyc_Absyn_Or_e_struct
_tmpDF7;struct Cyc_Absyn_Or_e_struct*_tmpDF6;return Cyc_Tcutil_copye(e,(void*)((
_tmpDF6=_cycalloc(sizeof(*_tmpDF6)),((_tmpDF6[0]=((_tmpDF7.tag=8,((_tmpDF7.f1=
_tmp669,((_tmpDF7.f2=_tmp66A,_tmpDF7)))))),_tmpDF6)))));}}_LL458: if(*((int*)
_tmp63D)!= 9)goto _LL45A;_tmp647=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp63D)->f1;
_tmp648=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp63D)->f2;_LL459: {struct Cyc_Absyn_Exp*
_tmp66D=Cyc_Tcutil_rsubsexp(r,inst,_tmp647);struct Cyc_Absyn_Exp*_tmp66E=Cyc_Tcutil_rsubsexp(
r,inst,_tmp648);if(_tmp66D == _tmp647  && _tmp66E == _tmp648)return e;{struct Cyc_Absyn_SeqExp_e_struct
_tmpDFA;struct Cyc_Absyn_SeqExp_e_struct*_tmpDF9;return Cyc_Tcutil_copye(e,(void*)((
_tmpDF9=_cycalloc(sizeof(*_tmpDF9)),((_tmpDF9[0]=((_tmpDFA.tag=9,((_tmpDFA.f1=
_tmp66D,((_tmpDFA.f2=_tmp66E,_tmpDFA)))))),_tmpDF9)))));}}_LL45A: if(*((int*)
_tmp63D)!= 15)goto _LL45C;_tmp649=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp63D)->f1;
_tmp64A=((struct Cyc_Absyn_Cast_e_struct*)_tmp63D)->f2;_tmp64B=((struct Cyc_Absyn_Cast_e_struct*)
_tmp63D)->f3;_tmp64C=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp63D)->f4;_LL45B: {
struct Cyc_Absyn_Exp*_tmp671=Cyc_Tcutil_rsubsexp(r,inst,_tmp64A);void*_tmp672=Cyc_Tcutil_rsubstitute(
r,inst,_tmp649);if(_tmp671 == _tmp64A  && _tmp672 == _tmp649)return e;{struct Cyc_Absyn_Cast_e_struct
_tmpDFD;struct Cyc_Absyn_Cast_e_struct*_tmpDFC;return Cyc_Tcutil_copye(e,(void*)((
_tmpDFC=_cycalloc(sizeof(*_tmpDFC)),((_tmpDFC[0]=((_tmpDFD.tag=15,((_tmpDFD.f1=(
void*)_tmp672,((_tmpDFD.f2=_tmp671,((_tmpDFD.f3=_tmp64B,((_tmpDFD.f4=(void*)
_tmp64C,_tmpDFD)))))))))),_tmpDFC)))));}}_LL45C: if(*((int*)_tmp63D)!= 18)goto
_LL45E;_tmp64D=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp63D)->f1;_LL45D: {
void*_tmp675=Cyc_Tcutil_rsubstitute(r,inst,_tmp64D);if(_tmp675 == _tmp64D)return e;{
struct Cyc_Absyn_Sizeoftyp_e_struct _tmpE00;struct Cyc_Absyn_Sizeoftyp_e_struct*
_tmpDFF;return Cyc_Tcutil_copye(e,(void*)((_tmpDFF=_cycalloc(sizeof(*_tmpDFF)),((
_tmpDFF[0]=((_tmpE00.tag=18,((_tmpE00.f1=(void*)_tmp675,_tmpE00)))),_tmpDFF)))));}}
_LL45E: if(*((int*)_tmp63D)!= 19)goto _LL460;_tmp64E=((struct Cyc_Absyn_Sizeofexp_e_struct*)
_tmp63D)->f1;_LL45F: {struct Cyc_Absyn_Exp*_tmp678=Cyc_Tcutil_rsubsexp(r,inst,
_tmp64E);if(_tmp678 == _tmp64E)return e;{struct Cyc_Absyn_Sizeofexp_e_struct _tmpE03;
struct Cyc_Absyn_Sizeofexp_e_struct*_tmpE02;return Cyc_Tcutil_copye(e,(void*)((
_tmpE02=_cycalloc(sizeof(*_tmpE02)),((_tmpE02[0]=((_tmpE03.tag=19,((_tmpE03.f1=
_tmp678,_tmpE03)))),_tmpE02)))));}}_LL460: if(*((int*)_tmp63D)!= 20)goto _LL462;
_tmp64F=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp63D)->f1;_tmp650=(void*)((
struct Cyc_Absyn_Offsetof_e_struct*)_tmp63D)->f2;_LL461: {void*_tmp67B=Cyc_Tcutil_rsubstitute(
r,inst,_tmp64F);if(_tmp67B == _tmp64F)return e;{struct Cyc_Absyn_Offsetof_e_struct
_tmpE06;struct Cyc_Absyn_Offsetof_e_struct*_tmpE05;return Cyc_Tcutil_copye(e,(void*)((
_tmpE05=_cycalloc(sizeof(*_tmpE05)),((_tmpE05[0]=((_tmpE06.tag=20,((_tmpE06.f1=(
void*)_tmp67B,((_tmpE06.f2=(void*)_tmp650,_tmpE06)))))),_tmpE05)))));}}_LL462:
if(*((int*)_tmp63D)!= 39)goto _LL464;_tmp651=(void*)((struct Cyc_Absyn_Valueof_e_struct*)
_tmp63D)->f1;_LL463: {void*_tmp67E=Cyc_Tcutil_rsubstitute(r,inst,_tmp651);if(
_tmp67E == _tmp651)return e;{struct Cyc_Absyn_Valueof_e_struct _tmpE09;struct Cyc_Absyn_Valueof_e_struct*
_tmpE08;return Cyc_Tcutil_copye(e,(void*)((_tmpE08=_cycalloc(sizeof(*_tmpE08)),((
_tmpE08[0]=((_tmpE09.tag=39,((_tmpE09.f1=(void*)_tmp67E,_tmpE09)))),_tmpE08)))));}}
_LL464:;_LL465: {const char*_tmpE0C;void*_tmpE0B;return(_tmpE0B=0,((struct Cyc_Absyn_Exp*(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpE0C="non-type-level-expression in Tcutil::rsubsexp",
_tag_dyneither(_tmpE0C,sizeof(char),46))),_tag_dyneither(_tmpE0B,sizeof(void*),0)));}
_LL447:;}void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*rgn,struct Cyc_List_List*
inst,void*t);void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*rgn,struct Cyc_List_List*
inst,void*t){void*_tmp683=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*_tmp684;
struct Cyc_Absyn_AggrInfo _tmp685;union Cyc_Absyn_AggrInfoU_union _tmp686;struct Cyc_List_List*
_tmp687;struct Cyc_Absyn_TunionInfo _tmp688;union Cyc_Absyn_TunionInfoU_union
_tmp689;struct Cyc_List_List*_tmp68A;struct Cyc_Core_Opt*_tmp68B;struct Cyc_Absyn_TunionFieldInfo
_tmp68C;union Cyc_Absyn_TunionFieldInfoU_union _tmp68D;struct Cyc_List_List*_tmp68E;
struct _tuple2*_tmp68F;struct Cyc_List_List*_tmp690;struct Cyc_Absyn_Typedefdecl*
_tmp691;void**_tmp692;struct Cyc_Absyn_ArrayInfo _tmp693;void*_tmp694;struct Cyc_Absyn_Tqual
_tmp695;struct Cyc_Absyn_Exp*_tmp696;struct Cyc_Absyn_Conref*_tmp697;struct Cyc_Position_Segment*
_tmp698;struct Cyc_Absyn_PtrInfo _tmp699;void*_tmp69A;struct Cyc_Absyn_Tqual _tmp69B;
struct Cyc_Absyn_PtrAtts _tmp69C;void*_tmp69D;struct Cyc_Absyn_Conref*_tmp69E;
struct Cyc_Absyn_Conref*_tmp69F;struct Cyc_Absyn_Conref*_tmp6A0;struct Cyc_Absyn_FnInfo
_tmp6A1;struct Cyc_List_List*_tmp6A2;struct Cyc_Core_Opt*_tmp6A3;void*_tmp6A4;
struct Cyc_List_List*_tmp6A5;int _tmp6A6;struct Cyc_Absyn_VarargInfo*_tmp6A7;struct
Cyc_List_List*_tmp6A8;struct Cyc_List_List*_tmp6A9;struct Cyc_List_List*_tmp6AA;
void*_tmp6AB;struct Cyc_List_List*_tmp6AC;struct Cyc_Core_Opt*_tmp6AD;void*_tmp6AE;
void*_tmp6AF;void*_tmp6B0;void*_tmp6B1;struct Cyc_Absyn_Exp*_tmp6B2;void*_tmp6B3;
void*_tmp6B4;struct Cyc_List_List*_tmp6B5;_LL467: if(_tmp683 <= (void*)4)goto _LL489;
if(*((int*)_tmp683)!= 1)goto _LL469;_tmp684=((struct Cyc_Absyn_VarType_struct*)
_tmp683)->f1;_LL468: {struct _handler_cons _tmp6B6;_push_handler(& _tmp6B6);{int
_tmp6B8=0;if(setjmp(_tmp6B6.handler))_tmp6B8=1;if(!_tmp6B8){{void*_tmp6B9=((void*(*)(
int(*cmp)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,
struct Cyc_Absyn_Tvar*x))Cyc_List_assoc_cmp)(Cyc_Absyn_tvar_cmp,inst,_tmp684);
_npop_handler(0);return _tmp6B9;};_pop_handler();}else{void*_tmp6B7=(void*)
_exn_thrown;void*_tmp6BB=_tmp6B7;_LL49C: if(_tmp6BB != Cyc_Core_Not_found)goto
_LL49E;_LL49D: return t;_LL49E:;_LL49F:(void)_throw(_tmp6BB);_LL49B:;}}}_LL469: if(*((
int*)_tmp683)!= 10)goto _LL46B;_tmp685=((struct Cyc_Absyn_AggrType_struct*)_tmp683)->f1;
_tmp686=_tmp685.aggr_info;_tmp687=_tmp685.targs;_LL46A: {struct Cyc_List_List*
_tmp6BC=Cyc_Tcutil_substs(rgn,inst,_tmp687);struct Cyc_Absyn_AggrType_struct
_tmpE12;struct Cyc_Absyn_AggrInfo _tmpE11;struct Cyc_Absyn_AggrType_struct*_tmpE10;
return _tmp6BC == _tmp687?t:(void*)((_tmpE10=_cycalloc(sizeof(*_tmpE10)),((_tmpE10[
0]=((_tmpE12.tag=10,((_tmpE12.f1=((_tmpE11.aggr_info=_tmp686,((_tmpE11.targs=
_tmp6BC,_tmpE11)))),_tmpE12)))),_tmpE10))));}_LL46B: if(*((int*)_tmp683)!= 2)goto
_LL46D;_tmp688=((struct Cyc_Absyn_TunionType_struct*)_tmp683)->f1;_tmp689=_tmp688.tunion_info;
_tmp68A=_tmp688.targs;_tmp68B=_tmp688.rgn;_LL46C: {struct Cyc_List_List*_tmp6C0=
Cyc_Tcutil_substs(rgn,inst,_tmp68A);struct Cyc_Core_Opt*new_r;if((unsigned int)
_tmp68B){void*_tmp6C1=Cyc_Tcutil_rsubstitute(rgn,inst,(void*)_tmp68B->v);if(
_tmp6C1 == (void*)_tmp68B->v)new_r=_tmp68B;else{struct Cyc_Core_Opt*_tmpE13;new_r=((
_tmpE13=_cycalloc(sizeof(*_tmpE13)),((_tmpE13->v=(void*)_tmp6C1,_tmpE13))));}}
else{new_r=_tmp68B;}{struct Cyc_Absyn_TunionType_struct _tmpE19;struct Cyc_Absyn_TunionInfo
_tmpE18;struct Cyc_Absyn_TunionType_struct*_tmpE17;return _tmp6C0 == _tmp68A  && 
new_r == _tmp68B?t:(void*)((_tmpE17=_cycalloc(sizeof(*_tmpE17)),((_tmpE17[0]=((
_tmpE19.tag=2,((_tmpE19.f1=((_tmpE18.tunion_info=_tmp689,((_tmpE18.targs=_tmp6C0,((
_tmpE18.rgn=new_r,_tmpE18)))))),_tmpE19)))),_tmpE17))));}}_LL46D: if(*((int*)
_tmp683)!= 3)goto _LL46F;_tmp68C=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp683)->f1;_tmp68D=_tmp68C.field_info;_tmp68E=_tmp68C.targs;_LL46E: {struct Cyc_List_List*
_tmp6C6=Cyc_Tcutil_substs(rgn,inst,_tmp68E);struct Cyc_Absyn_TunionFieldType_struct
_tmpE1F;struct Cyc_Absyn_TunionFieldInfo _tmpE1E;struct Cyc_Absyn_TunionFieldType_struct*
_tmpE1D;return _tmp6C6 == _tmp68E?t:(void*)((_tmpE1D=_cycalloc(sizeof(*_tmpE1D)),((
_tmpE1D[0]=((_tmpE1F.tag=3,((_tmpE1F.f1=((_tmpE1E.field_info=_tmp68D,((_tmpE1E.targs=
_tmp6C6,_tmpE1E)))),_tmpE1F)))),_tmpE1D))));}_LL46F: if(*((int*)_tmp683)!= 16)
goto _LL471;_tmp68F=((struct Cyc_Absyn_TypedefType_struct*)_tmp683)->f1;_tmp690=((
struct Cyc_Absyn_TypedefType_struct*)_tmp683)->f2;_tmp691=((struct Cyc_Absyn_TypedefType_struct*)
_tmp683)->f3;_tmp692=((struct Cyc_Absyn_TypedefType_struct*)_tmp683)->f4;_LL470: {
struct Cyc_List_List*_tmp6CA=Cyc_Tcutil_substs(rgn,inst,_tmp690);struct Cyc_Absyn_TypedefType_struct
_tmpE22;struct Cyc_Absyn_TypedefType_struct*_tmpE21;return _tmp6CA == _tmp690?t:(
void*)((_tmpE21=_cycalloc(sizeof(*_tmpE21)),((_tmpE21[0]=((_tmpE22.tag=16,((
_tmpE22.f1=_tmp68F,((_tmpE22.f2=_tmp6CA,((_tmpE22.f3=_tmp691,((_tmpE22.f4=
_tmp692,_tmpE22)))))))))),_tmpE21))));}_LL471: if(*((int*)_tmp683)!= 7)goto _LL473;
_tmp693=((struct Cyc_Absyn_ArrayType_struct*)_tmp683)->f1;_tmp694=(void*)_tmp693.elt_type;
_tmp695=_tmp693.tq;_tmp696=_tmp693.num_elts;_tmp697=_tmp693.zero_term;_tmp698=
_tmp693.zt_loc;_LL472: {void*_tmp6CD=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp694);
struct Cyc_Absyn_ArrayType_struct _tmpE28;struct Cyc_Absyn_ArrayInfo _tmpE27;struct
Cyc_Absyn_ArrayType_struct*_tmpE26;return _tmp6CD == _tmp694?t:(void*)((_tmpE26=
_cycalloc(sizeof(*_tmpE26)),((_tmpE26[0]=((_tmpE28.tag=7,((_tmpE28.f1=((_tmpE27.elt_type=(
void*)_tmp6CD,((_tmpE27.tq=_tmp695,((_tmpE27.num_elts=_tmp696,((_tmpE27.zero_term=
_tmp697,((_tmpE27.zt_loc=_tmp698,_tmpE27)))))))))),_tmpE28)))),_tmpE26))));}
_LL473: if(*((int*)_tmp683)!= 4)goto _LL475;_tmp699=((struct Cyc_Absyn_PointerType_struct*)
_tmp683)->f1;_tmp69A=(void*)_tmp699.elt_typ;_tmp69B=_tmp699.elt_tq;_tmp69C=
_tmp699.ptr_atts;_tmp69D=(void*)_tmp69C.rgn;_tmp69E=_tmp69C.nullable;_tmp69F=
_tmp69C.bounds;_tmp6A0=_tmp69C.zero_term;_LL474: {void*_tmp6D1=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmp69A);void*_tmp6D2=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp69D);struct
Cyc_Absyn_Conref*_tmp6D3=_tmp69F;{union Cyc_Absyn_Constraint_union _tmp6D4=(Cyc_Absyn_compress_conref(
_tmp69F))->v;void*_tmp6D5;struct Cyc_Absyn_Exp*_tmp6D6;_LL4A1: if((_tmp6D4.Eq_constr).tag
!= 0)goto _LL4A3;_tmp6D5=(_tmp6D4.Eq_constr).f1;if(_tmp6D5 <= (void*)1)goto _LL4A3;
if(*((int*)_tmp6D5)!= 0)goto _LL4A3;_tmp6D6=((struct Cyc_Absyn_Upper_b_struct*)
_tmp6D5)->f1;_LL4A2: {struct Cyc_Absyn_Exp*_tmp6D7=Cyc_Tcutil_rsubsexp(rgn,inst,
_tmp6D6);if(_tmp6D7 != _tmp6D6){struct Cyc_Absyn_Upper_b_struct _tmpE2B;struct Cyc_Absyn_Upper_b_struct*
_tmpE2A;_tmp6D3=Cyc_Absyn_new_conref((void*)((_tmpE2A=_cycalloc(sizeof(*_tmpE2A)),((
_tmpE2A[0]=((_tmpE2B.tag=0,((_tmpE2B.f1=_tmp6D7,_tmpE2B)))),_tmpE2A)))));}goto
_LL4A0;}_LL4A3:;_LL4A4: goto _LL4A0;_LL4A0:;}if((_tmp6D1 == _tmp69A  && _tmp6D2 == 
_tmp69D) && _tmp6D3 == _tmp69F)return t;{struct Cyc_Absyn_PointerType_struct _tmpE35;
struct Cyc_Absyn_PtrAtts _tmpE34;struct Cyc_Absyn_PtrInfo _tmpE33;struct Cyc_Absyn_PointerType_struct*
_tmpE32;return(void*)((_tmpE32=_cycalloc(sizeof(*_tmpE32)),((_tmpE32[0]=((
_tmpE35.tag=4,((_tmpE35.f1=((_tmpE33.elt_typ=(void*)_tmp6D1,((_tmpE33.elt_tq=
_tmp69B,((_tmpE33.ptr_atts=((_tmpE34.rgn=(void*)_tmp6D2,((_tmpE34.nullable=
_tmp69E,((_tmpE34.bounds=_tmp6D3,((_tmpE34.zero_term=_tmp6A0,((_tmpE34.ptrloc=0,
_tmpE34)))))))))),_tmpE33)))))),_tmpE35)))),_tmpE32))));}}_LL475: if(*((int*)
_tmp683)!= 8)goto _LL477;_tmp6A1=((struct Cyc_Absyn_FnType_struct*)_tmp683)->f1;
_tmp6A2=_tmp6A1.tvars;_tmp6A3=_tmp6A1.effect;_tmp6A4=(void*)_tmp6A1.ret_typ;
_tmp6A5=_tmp6A1.args;_tmp6A6=_tmp6A1.c_varargs;_tmp6A7=_tmp6A1.cyc_varargs;
_tmp6A8=_tmp6A1.rgn_po;_tmp6A9=_tmp6A1.attributes;_LL476:{struct Cyc_List_List*
_tmp6DE=_tmp6A2;for(0;_tmp6DE != 0;_tmp6DE=_tmp6DE->tl){struct _tuple8*_tmpE3F;
struct Cyc_Absyn_VarType_struct _tmpE3E;struct Cyc_Absyn_VarType_struct*_tmpE3D;
struct Cyc_List_List*_tmpE3C;inst=((_tmpE3C=_region_malloc(rgn,sizeof(*_tmpE3C)),((
_tmpE3C->hd=((_tmpE3F=_region_malloc(rgn,sizeof(*_tmpE3F)),((_tmpE3F->f1=(struct
Cyc_Absyn_Tvar*)_tmp6DE->hd,((_tmpE3F->f2=(void*)((_tmpE3D=_cycalloc(sizeof(*
_tmpE3D)),((_tmpE3D[0]=((_tmpE3E.tag=1,((_tmpE3E.f1=(struct Cyc_Absyn_Tvar*)
_tmp6DE->hd,_tmpE3E)))),_tmpE3D)))),_tmpE3F)))))),((_tmpE3C->tl=inst,_tmpE3C))))));}}{
struct Cyc_List_List*_tmp6E4;struct Cyc_List_List*_tmp6E5;struct _tuple1 _tmp6E3=((
struct _tuple1(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x))Cyc_List_rsplit)(rgn,rgn,((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct _tuple17*(*f)(struct _RegionHandle*,struct _tuple3*),struct _RegionHandle*env,
struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_substitute_f1,rgn,_tmp6A5));
_tmp6E4=_tmp6E3.f1;_tmp6E5=_tmp6E3.f2;{struct Cyc_List_List*_tmp6E6=Cyc_Tcutil_substs(
rgn,inst,_tmp6E5);struct Cyc_List_List*_tmp6E7=((struct Cyc_List_List*(*)(struct
_tuple3*(*f)(struct _tuple17*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_substitute_f2,((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmp6E4,_tmp6E6));struct Cyc_Core_Opt*
eff2;if(_tmp6A3 == 0)eff2=0;else{void*_tmp6E8=Cyc_Tcutil_rsubstitute(rgn,inst,(
void*)_tmp6A3->v);if(_tmp6E8 == (void*)_tmp6A3->v)eff2=_tmp6A3;else{struct Cyc_Core_Opt*
_tmpE40;eff2=((_tmpE40=_cycalloc(sizeof(*_tmpE40)),((_tmpE40->v=(void*)_tmp6E8,
_tmpE40))));}}{struct Cyc_Absyn_VarargInfo*cyc_varargs2;if(_tmp6A7 == 0)
cyc_varargs2=0;else{struct Cyc_Core_Opt*_tmp6EB;struct Cyc_Absyn_Tqual _tmp6EC;void*
_tmp6ED;int _tmp6EE;struct Cyc_Absyn_VarargInfo _tmp6EA=*_tmp6A7;_tmp6EB=_tmp6EA.name;
_tmp6EC=_tmp6EA.tq;_tmp6ED=(void*)_tmp6EA.type;_tmp6EE=_tmp6EA.inject;{void*
_tmp6EF=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6ED);if(_tmp6EF == _tmp6ED)
cyc_varargs2=_tmp6A7;else{struct Cyc_Absyn_VarargInfo*_tmpE41;cyc_varargs2=((
_tmpE41=_cycalloc(sizeof(*_tmpE41)),((_tmpE41->name=_tmp6EB,((_tmpE41->tq=
_tmp6EC,((_tmpE41->type=(void*)_tmp6EF,((_tmpE41->inject=_tmp6EE,_tmpE41))))))))));}}}{
struct Cyc_List_List*rgn_po2;struct Cyc_List_List*_tmp6F2;struct Cyc_List_List*
_tmp6F3;struct _tuple1 _tmp6F1=Cyc_List_rsplit(rgn,rgn,_tmp6A8);_tmp6F2=_tmp6F1.f1;
_tmp6F3=_tmp6F1.f2;{struct Cyc_List_List*_tmp6F4=Cyc_Tcutil_substs(rgn,inst,
_tmp6F2);struct Cyc_List_List*_tmp6F5=Cyc_Tcutil_substs(rgn,inst,_tmp6F3);if(
_tmp6F4 == _tmp6F2  && _tmp6F5 == _tmp6F3)rgn_po2=_tmp6A8;else{rgn_po2=Cyc_List_zip(
_tmp6F4,_tmp6F5);}{struct Cyc_Absyn_FnType_struct _tmpE47;struct Cyc_Absyn_FnInfo
_tmpE46;struct Cyc_Absyn_FnType_struct*_tmpE45;return(void*)((_tmpE45=_cycalloc(
sizeof(*_tmpE45)),((_tmpE45[0]=((_tmpE47.tag=8,((_tmpE47.f1=((_tmpE46.tvars=
_tmp6A2,((_tmpE46.effect=eff2,((_tmpE46.ret_typ=(void*)Cyc_Tcutil_rsubstitute(
rgn,inst,_tmp6A4),((_tmpE46.args=_tmp6E7,((_tmpE46.c_varargs=_tmp6A6,((_tmpE46.cyc_varargs=
cyc_varargs2,((_tmpE46.rgn_po=rgn_po2,((_tmpE46.attributes=_tmp6A9,_tmpE46)))))))))))))))),
_tmpE47)))),_tmpE45))));}}}}}}_LL477: if(*((int*)_tmp683)!= 9)goto _LL479;_tmp6AA=((
struct Cyc_Absyn_TupleType_struct*)_tmp683)->f1;_LL478: {struct Cyc_List_List*
_tmp6F9=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _tuple5*),
struct Cyc_List_List*x))Cyc_List_rmap)(rgn,Cyc_Tcutil_snd_tqt,_tmp6AA);struct Cyc_List_List*
_tmp6FA=Cyc_Tcutil_substs(rgn,inst,_tmp6F9);if(_tmp6FA == _tmp6F9)return t;{struct
Cyc_List_List*_tmp6FB=((struct Cyc_List_List*(*)(struct _tuple5*(*f)(struct _tuple5*,
void*),struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_map2_tq,
_tmp6AA,_tmp6FA);struct Cyc_Absyn_TupleType_struct _tmpE4A;struct Cyc_Absyn_TupleType_struct*
_tmpE49;return(void*)((_tmpE49=_cycalloc(sizeof(*_tmpE49)),((_tmpE49[0]=((
_tmpE4A.tag=9,((_tmpE4A.f1=_tmp6FB,_tmpE4A)))),_tmpE49))));}}_LL479: if(*((int*)
_tmp683)!= 11)goto _LL47B;_tmp6AB=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp683)->f1;_tmp6AC=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp683)->f2;_LL47A: {
struct Cyc_List_List*_tmp6FE=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*
f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_rmap)(rgn,Cyc_Tcutil_field_type,
_tmp6AC);struct Cyc_List_List*_tmp6FF=Cyc_Tcutil_substs(rgn,inst,_tmp6FE);if(
_tmp6FF == _tmp6FE)return t;{struct Cyc_List_List*_tmp700=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*,void*),struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_zip_field_type,_tmp6AC,
_tmp6FF);struct Cyc_Absyn_AnonAggrType_struct _tmpE4D;struct Cyc_Absyn_AnonAggrType_struct*
_tmpE4C;return(void*)((_tmpE4C=_cycalloc(sizeof(*_tmpE4C)),((_tmpE4C[0]=((
_tmpE4D.tag=11,((_tmpE4D.f1=(void*)_tmp6AB,((_tmpE4D.f2=_tmp700,_tmpE4D)))))),
_tmpE4C))));}}_LL47B: if(*((int*)_tmp683)!= 0)goto _LL47D;_tmp6AD=((struct Cyc_Absyn_Evar_struct*)
_tmp683)->f2;_LL47C: if(_tmp6AD != 0)return Cyc_Tcutil_rsubstitute(rgn,inst,(void*)
_tmp6AD->v);else{return t;}_LL47D: if(*((int*)_tmp683)!= 14)goto _LL47F;_tmp6AE=(
void*)((struct Cyc_Absyn_RgnHandleType_struct*)_tmp683)->f1;_LL47E: {void*_tmp703=
Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6AE);struct Cyc_Absyn_RgnHandleType_struct
_tmpE50;struct Cyc_Absyn_RgnHandleType_struct*_tmpE4F;return _tmp703 == _tmp6AE?t:(
void*)((_tmpE4F=_cycalloc(sizeof(*_tmpE4F)),((_tmpE4F[0]=((_tmpE50.tag=14,((
_tmpE50.f1=(void*)_tmp703,_tmpE50)))),_tmpE4F))));}_LL47F: if(*((int*)_tmp683)!= 
15)goto _LL481;_tmp6AF=(void*)((struct Cyc_Absyn_DynRgnType_struct*)_tmp683)->f1;
_tmp6B0=(void*)((struct Cyc_Absyn_DynRgnType_struct*)_tmp683)->f2;_LL480: {void*
_tmp706=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6AF);void*_tmp707=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmp6B0);struct Cyc_Absyn_DynRgnType_struct _tmpE53;struct Cyc_Absyn_DynRgnType_struct*
_tmpE52;return _tmp706 == _tmp6AF  && _tmp707 == _tmp6B0?t:(void*)((_tmpE52=
_cycalloc(sizeof(*_tmpE52)),((_tmpE52[0]=((_tmpE53.tag=15,((_tmpE53.f1=(void*)
_tmp706,((_tmpE53.f2=(void*)_tmp707,_tmpE53)))))),_tmpE52))));}_LL481: if(*((int*)
_tmp683)!= 18)goto _LL483;_tmp6B1=(void*)((struct Cyc_Absyn_TagType_struct*)
_tmp683)->f1;_LL482: {void*_tmp70A=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6B1);
struct Cyc_Absyn_TagType_struct _tmpE56;struct Cyc_Absyn_TagType_struct*_tmpE55;
return _tmp70A == _tmp6B1?t:(void*)((_tmpE55=_cycalloc(sizeof(*_tmpE55)),((_tmpE55[
0]=((_tmpE56.tag=18,((_tmpE56.f1=(void*)_tmp70A,_tmpE56)))),_tmpE55))));}_LL483:
if(*((int*)_tmp683)!= 17)goto _LL485;_tmp6B2=((struct Cyc_Absyn_ValueofType_struct*)
_tmp683)->f1;_LL484: {struct Cyc_Absyn_Exp*_tmp70D=Cyc_Tcutil_rsubsexp(rgn,inst,
_tmp6B2);struct Cyc_Absyn_ValueofType_struct _tmpE59;struct Cyc_Absyn_ValueofType_struct*
_tmpE58;return _tmp70D == _tmp6B2?t:(void*)((_tmpE58=_cycalloc(sizeof(*_tmpE58)),((
_tmpE58[0]=((_tmpE59.tag=17,((_tmpE59.f1=_tmp70D,_tmpE59)))),_tmpE58))));}_LL485:
if(*((int*)_tmp683)!= 12)goto _LL487;_LL486: goto _LL488;_LL487: if(*((int*)_tmp683)
!= 13)goto _LL489;_LL488: goto _LL48A;_LL489: if((int)_tmp683 != 0)goto _LL48B;_LL48A:
goto _LL48C;_LL48B: if(_tmp683 <= (void*)4)goto _LL48D;if(*((int*)_tmp683)!= 5)goto
_LL48D;_LL48C: goto _LL48E;_LL48D: if((int)_tmp683 != 1)goto _LL48F;_LL48E: goto _LL490;
_LL48F: if(_tmp683 <= (void*)4)goto _LL491;if(*((int*)_tmp683)!= 6)goto _LL491;
_LL490: goto _LL492;_LL491: if((int)_tmp683 != 3)goto _LL493;_LL492: goto _LL494;_LL493:
if((int)_tmp683 != 2)goto _LL495;_LL494: return t;_LL495: if(_tmp683 <= (void*)4)goto
_LL497;if(*((int*)_tmp683)!= 21)goto _LL497;_tmp6B3=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp683)->f1;_LL496: {void*_tmp710=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6B3);
struct Cyc_Absyn_RgnsEff_struct _tmpE5C;struct Cyc_Absyn_RgnsEff_struct*_tmpE5B;
return _tmp710 == _tmp6B3?t:(void*)((_tmpE5B=_cycalloc(sizeof(*_tmpE5B)),((_tmpE5B[
0]=((_tmpE5C.tag=21,((_tmpE5C.f1=(void*)_tmp710,_tmpE5C)))),_tmpE5B))));}_LL497:
if(_tmp683 <= (void*)4)goto _LL499;if(*((int*)_tmp683)!= 19)goto _LL499;_tmp6B4=(
void*)((struct Cyc_Absyn_AccessEff_struct*)_tmp683)->f1;_LL498: {void*_tmp713=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmp6B4);struct Cyc_Absyn_AccessEff_struct _tmpE5F;struct Cyc_Absyn_AccessEff_struct*
_tmpE5E;return _tmp713 == _tmp6B4?t:(void*)((_tmpE5E=_cycalloc(sizeof(*_tmpE5E)),((
_tmpE5E[0]=((_tmpE5F.tag=19,((_tmpE5F.f1=(void*)_tmp713,_tmpE5F)))),_tmpE5E))));}
_LL499: if(_tmp683 <= (void*)4)goto _LL466;if(*((int*)_tmp683)!= 20)goto _LL466;
_tmp6B5=((struct Cyc_Absyn_JoinEff_struct*)_tmp683)->f1;_LL49A: {struct Cyc_List_List*
_tmp716=Cyc_Tcutil_substs(rgn,inst,_tmp6B5);struct Cyc_Absyn_JoinEff_struct
_tmpE62;struct Cyc_Absyn_JoinEff_struct*_tmpE61;return _tmp716 == _tmp6B5?t:(void*)((
_tmpE61=_cycalloc(sizeof(*_tmpE61)),((_tmpE61[0]=((_tmpE62.tag=20,((_tmpE62.f1=
_tmp716,_tmpE62)))),_tmpE61))));}_LL466:;}static struct Cyc_List_List*Cyc_Tcutil_substs(
struct _RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*ts);static
struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*rgn,struct Cyc_List_List*
inst,struct Cyc_List_List*ts){if(ts == 0)return 0;{void*_tmp719=(void*)ts->hd;
struct Cyc_List_List*_tmp71A=ts->tl;void*_tmp71B=Cyc_Tcutil_rsubstitute(rgn,inst,
_tmp719);struct Cyc_List_List*_tmp71C=Cyc_Tcutil_substs(rgn,inst,_tmp71A);if(
_tmp719 == _tmp71B  && _tmp71A == _tmp71C)return ts;{struct Cyc_List_List*_tmpE63;
return(struct Cyc_List_List*)((struct Cyc_List_List*)((_tmpE63=_cycalloc(sizeof(*
_tmpE63)),((_tmpE63->hd=(void*)_tmp71B,((_tmpE63->tl=_tmp71C,_tmpE63)))))));}}}
void*Cyc_Tcutil_substitute(struct Cyc_List_List*inst,void*t);extern void*Cyc_Tcutil_substitute(
struct Cyc_List_List*inst,void*t){return Cyc_Tcutil_rsubstitute(Cyc_Core_heap_region,
inst,t);}struct _tuple8*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*s,struct Cyc_Absyn_Tvar*
tv);struct _tuple8*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*s,struct Cyc_Absyn_Tvar*
tv){struct Cyc_Core_Opt*_tmp71E=Cyc_Tcutil_kind_to_opt(Cyc_Tcutil_tvar_kind(tv));
struct Cyc_Core_Opt*_tmpE66;struct _tuple8*_tmpE65;return(_tmpE65=_cycalloc(
sizeof(*_tmpE65)),((_tmpE65->f1=tv,((_tmpE65->f2=Cyc_Absyn_new_evar(_tmp71E,((
_tmpE66=_cycalloc(sizeof(*_tmpE66)),((_tmpE66->v=s,_tmpE66))))),_tmpE65)))));}
struct _tuple8*Cyc_Tcutil_r_make_inst_var(struct _tuple9*env,struct Cyc_Absyn_Tvar*
tv);struct _tuple8*Cyc_Tcutil_r_make_inst_var(struct _tuple9*env,struct Cyc_Absyn_Tvar*
tv){struct _tuple9 _tmp722;struct Cyc_List_List*_tmp723;struct _RegionHandle*_tmp724;
struct _tuple9*_tmp721=env;_tmp722=*_tmp721;_tmp723=_tmp722.f1;_tmp724=_tmp722.f2;{
struct Cyc_Core_Opt*_tmp725=Cyc_Tcutil_kind_to_opt(Cyc_Tcutil_tvar_kind(tv));
struct Cyc_Core_Opt*_tmpE69;struct _tuple8*_tmpE68;return(_tmpE68=_region_malloc(
_tmp724,sizeof(*_tmpE68)),((_tmpE68->f1=tv,((_tmpE68->f2=Cyc_Absyn_new_evar(
_tmp725,((_tmpE69=_cycalloc(sizeof(*_tmpE69)),((_tmpE69->v=_tmp723,_tmpE69))))),
_tmpE68)))));}}static struct Cyc_List_List*Cyc_Tcutil_add_free_tvar(struct Cyc_Position_Segment*
loc,struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv);static struct Cyc_List_List*
Cyc_Tcutil_add_free_tvar(struct Cyc_Position_Segment*loc,struct Cyc_List_List*tvs,
struct Cyc_Absyn_Tvar*tv){{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
if(Cyc_strptrcmp(((struct Cyc_Absyn_Tvar*)tvs2->hd)->name,tv->name)== 0){void*k1=(
void*)((struct Cyc_Absyn_Tvar*)tvs2->hd)->kind;void*k2=(void*)tv->kind;if(!Cyc_Tcutil_constrain_kinds(
k1,k2)){const char*_tmpE6F;void*_tmpE6E[3];struct Cyc_String_pa_struct _tmpE6D;
struct Cyc_String_pa_struct _tmpE6C;struct Cyc_String_pa_struct _tmpE6B;(_tmpE6B.tag=
0,((_tmpE6B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kindbound2string(
k2)),((_tmpE6C.tag=0,((_tmpE6C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kindbound2string(k1)),((_tmpE6D.tag=0,((_tmpE6D.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*tv->name),((_tmpE6E[0]=& _tmpE6D,((_tmpE6E[
1]=& _tmpE6C,((_tmpE6E[2]=& _tmpE6B,Cyc_Tcutil_terr(loc,((_tmpE6F="type variable %s is used with inconsistent kinds %s and %s",
_tag_dyneither(_tmpE6F,sizeof(char),59))),_tag_dyneither(_tmpE6E,sizeof(void*),3)))))))))))))))))));}
if(tv->identity == - 1)tv->identity=((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity;
else{if(tv->identity != ((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity){const char*
_tmpE72;void*_tmpE71;(_tmpE71=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpE72="same type variable has different identity!",
_tag_dyneither(_tmpE72,sizeof(char),43))),_tag_dyneither(_tmpE71,sizeof(void*),0)));}}
return tvs;}}}tv->identity=Cyc_Tcutil_new_tvar_id();{struct Cyc_List_List*_tmpE73;
return(_tmpE73=_cycalloc(sizeof(*_tmpE73)),((_tmpE73->hd=tv,((_tmpE73->tl=tvs,
_tmpE73)))));}}static struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar(struct Cyc_List_List*
tvs,struct Cyc_Absyn_Tvar*tv);static struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar(
struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){if(tv->identity == - 1){const char*
_tmpE76;void*_tmpE75;(_tmpE75=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpE76="fast_add_free_tvar: bad identity in tv",
_tag_dyneither(_tmpE76,sizeof(char),39))),_tag_dyneither(_tmpE75,sizeof(void*),0)));}{
struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){struct Cyc_Absyn_Tvar*
_tmp732=(struct Cyc_Absyn_Tvar*)tvs2->hd;if(_tmp732->identity == - 1){const char*
_tmpE79;void*_tmpE78;(_tmpE78=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpE79="fast_add_free_tvar: bad identity in tvs2",
_tag_dyneither(_tmpE79,sizeof(char),41))),_tag_dyneither(_tmpE78,sizeof(void*),0)));}
if(_tmp732->identity == tv->identity)return tvs;}}{struct Cyc_List_List*_tmpE7A;
return(_tmpE7A=_cycalloc(sizeof(*_tmpE7A)),((_tmpE7A->hd=tv,((_tmpE7A->tl=tvs,
_tmpE7A)))));}}struct _tuple18{struct Cyc_Absyn_Tvar*f1;int f2;};static struct Cyc_List_List*
Cyc_Tcutil_fast_add_free_tvar_bool(struct _RegionHandle*r,struct Cyc_List_List*tvs,
struct Cyc_Absyn_Tvar*tv,int b);static struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar_bool(
struct _RegionHandle*r,struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv,int b){if(
tv->identity == - 1){const char*_tmpE7D;void*_tmpE7C;(_tmpE7C=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpE7D="fast_add_free_tvar_bool: bad identity in tv",
_tag_dyneither(_tmpE7D,sizeof(char),44))),_tag_dyneither(_tmpE7C,sizeof(void*),0)));}{
struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){struct _tuple18 _tmp739;
struct Cyc_Absyn_Tvar*_tmp73A;int _tmp73B;int*_tmp73C;struct _tuple18*_tmp738=(
struct _tuple18*)tvs2->hd;_tmp739=*_tmp738;_tmp73A=_tmp739.f1;_tmp73B=_tmp739.f2;
_tmp73C=(int*)&(*_tmp738).f2;if(_tmp73A->identity == - 1){const char*_tmpE80;void*
_tmpE7F;(_tmpE7F=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmpE80="fast_add_free_tvar_bool: bad identity in tvs2",_tag_dyneither(_tmpE80,
sizeof(char),46))),_tag_dyneither(_tmpE7F,sizeof(void*),0)));}if(_tmp73A->identity
== tv->identity){*_tmp73C=*_tmp73C  || b;return tvs;}}}{struct _tuple18*_tmpE83;
struct Cyc_List_List*_tmpE82;return(_tmpE82=_region_malloc(r,sizeof(*_tmpE82)),((
_tmpE82->hd=((_tmpE83=_region_malloc(r,sizeof(*_tmpE83)),((_tmpE83->f1=tv,((
_tmpE83->f2=b,_tmpE83)))))),((_tmpE82->tl=tvs,_tmpE82)))));}}static struct Cyc_List_List*
Cyc_Tcutil_add_bound_tvar(struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv);
static struct Cyc_List_List*Cyc_Tcutil_add_bound_tvar(struct Cyc_List_List*tvs,
struct Cyc_Absyn_Tvar*tv){if(tv->identity == - 1){const char*_tmpE87;void*_tmpE86[1];
struct Cyc_String_pa_struct _tmpE85;(_tmpE85.tag=0,((_tmpE85.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_tvar2string(tv)),((_tmpE86[0]=&
_tmpE85,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmpE87="bound tvar id for %s is NULL",_tag_dyneither(_tmpE87,sizeof(char),29))),
_tag_dyneither(_tmpE86,sizeof(void*),1)))))));}{struct Cyc_List_List*_tmpE88;
return(_tmpE88=_cycalloc(sizeof(*_tmpE88)),((_tmpE88->hd=tv,((_tmpE88->tl=tvs,
_tmpE88)))));}}struct _tuple19{void*f1;int f2;};static struct Cyc_List_List*Cyc_Tcutil_add_free_evar(
struct _RegionHandle*r,struct Cyc_List_List*es,void*e,int b);static struct Cyc_List_List*
Cyc_Tcutil_add_free_evar(struct _RegionHandle*r,struct Cyc_List_List*es,void*e,int
b){void*_tmp745=Cyc_Tcutil_compress(e);int _tmp746;_LL4A6: if(_tmp745 <= (void*)4)
goto _LL4A8;if(*((int*)_tmp745)!= 0)goto _LL4A8;_tmp746=((struct Cyc_Absyn_Evar_struct*)
_tmp745)->f3;_LL4A7:{struct Cyc_List_List*es2=es;for(0;es2 != 0;es2=es2->tl){
struct _tuple19 _tmp748;void*_tmp749;int _tmp74A;int*_tmp74B;struct _tuple19*_tmp747=(
struct _tuple19*)es2->hd;_tmp748=*_tmp747;_tmp749=_tmp748.f1;_tmp74A=_tmp748.f2;
_tmp74B=(int*)&(*_tmp747).f2;{void*_tmp74C=Cyc_Tcutil_compress(_tmp749);int
_tmp74D;_LL4AB: if(_tmp74C <= (void*)4)goto _LL4AD;if(*((int*)_tmp74C)!= 0)goto
_LL4AD;_tmp74D=((struct Cyc_Absyn_Evar_struct*)_tmp74C)->f3;_LL4AC: if(_tmp746 == 
_tmp74D){if(b != *_tmp74B)*_tmp74B=1;return es;}goto _LL4AA;_LL4AD:;_LL4AE: goto
_LL4AA;_LL4AA:;}}}{struct _tuple19*_tmpE8B;struct Cyc_List_List*_tmpE8A;return(
_tmpE8A=_region_malloc(r,sizeof(*_tmpE8A)),((_tmpE8A->hd=((_tmpE8B=
_region_malloc(r,sizeof(*_tmpE8B)),((_tmpE8B->f1=e,((_tmpE8B->f2=b,_tmpE8B)))))),((
_tmpE8A->tl=es,_tmpE8A)))));}_LL4A8:;_LL4A9: return es;_LL4A5:;}static struct Cyc_List_List*
Cyc_Tcutil_remove_bound_tvars(struct _RegionHandle*rgn,struct Cyc_List_List*tvs,
struct Cyc_List_List*btvs);static struct Cyc_List_List*Cyc_Tcutil_remove_bound_tvars(
struct _RegionHandle*rgn,struct Cyc_List_List*tvs,struct Cyc_List_List*btvs){struct
Cyc_List_List*r=0;for(0;tvs != 0;tvs=tvs->tl){int present=0;{struct Cyc_List_List*b=
btvs;for(0;b != 0;b=b->tl){if(((struct Cyc_Absyn_Tvar*)tvs->hd)->identity == ((
struct Cyc_Absyn_Tvar*)b->hd)->identity){present=1;break;}}}if(!present){struct
Cyc_List_List*_tmpE8C;r=((_tmpE8C=_region_malloc(rgn,sizeof(*_tmpE8C)),((_tmpE8C->hd=(
struct Cyc_Absyn_Tvar*)tvs->hd,((_tmpE8C->tl=r,_tmpE8C))))));}}r=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(r);return r;}static struct Cyc_List_List*
Cyc_Tcutil_remove_bound_tvars_bool(struct _RegionHandle*r,struct Cyc_List_List*tvs,
struct Cyc_List_List*btvs);static struct Cyc_List_List*Cyc_Tcutil_remove_bound_tvars_bool(
struct _RegionHandle*r,struct Cyc_List_List*tvs,struct Cyc_List_List*btvs){struct
Cyc_List_List*res=0;for(0;tvs != 0;tvs=tvs->tl){struct Cyc_Absyn_Tvar*_tmp752;int
_tmp753;struct _tuple18 _tmp751=*((struct _tuple18*)tvs->hd);_tmp752=_tmp751.f1;
_tmp753=_tmp751.f2;{int present=0;{struct Cyc_List_List*b=btvs;for(0;b != 0;b=b->tl){
if(_tmp752->identity == ((struct Cyc_Absyn_Tvar*)b->hd)->identity){present=1;
break;}}}if(!present){struct Cyc_List_List*_tmpE8D;res=((_tmpE8D=_region_malloc(r,
sizeof(*_tmpE8D)),((_tmpE8D->hd=(struct _tuple18*)tvs->hd,((_tmpE8D->tl=res,
_tmpE8D))))));}}}res=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
res);return res;}void Cyc_Tcutil_check_bitfield(struct Cyc_Position_Segment*loc,
struct Cyc_Tcenv_Tenv*te,void*field_typ,struct Cyc_Absyn_Exp*width,struct
_dyneither_ptr*fn);void Cyc_Tcutil_check_bitfield(struct Cyc_Position_Segment*loc,
struct Cyc_Tcenv_Tenv*te,void*field_typ,struct Cyc_Absyn_Exp*width,struct
_dyneither_ptr*fn){if(width != 0){unsigned int w=0;if(!Cyc_Tcutil_is_const_exp(te,(
struct Cyc_Absyn_Exp*)width)){const char*_tmpE91;void*_tmpE90[1];struct Cyc_String_pa_struct
_tmpE8F;(_tmpE8F.tag=0,((_tmpE8F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*
fn),((_tmpE90[0]=& _tmpE8F,Cyc_Tcutil_terr(loc,((_tmpE91="bitfield %s does not have constant width",
_tag_dyneither(_tmpE91,sizeof(char),41))),_tag_dyneither(_tmpE90,sizeof(void*),1)))))));}
else{unsigned int _tmp759;int _tmp75A;struct _tuple7 _tmp758=Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)width);_tmp759=_tmp758.f1;_tmp75A=_tmp758.f2;if(!_tmp75A){
const char*_tmpE94;void*_tmpE93;(_tmpE93=0,Cyc_Tcutil_terr(loc,((_tmpE94="bitfield width cannot use sizeof or offsetof",
_tag_dyneither(_tmpE94,sizeof(char),45))),_tag_dyneither(_tmpE93,sizeof(void*),0)));}
w=_tmp759;}{void*_tmp75D=Cyc_Tcutil_compress(field_typ);void*_tmp75E;_LL4B0: if(
_tmp75D <= (void*)4)goto _LL4B2;if(*((int*)_tmp75D)!= 5)goto _LL4B2;_tmp75E=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp75D)->f2;_LL4B1:{void*_tmp75F=_tmp75E;_LL4B5:
if((int)_tmp75F != 0)goto _LL4B7;_LL4B6: if(w > 8){const char*_tmpE97;void*_tmpE96;(
_tmpE96=0,Cyc_Tcutil_terr(loc,((_tmpE97="bitfield larger than type",
_tag_dyneither(_tmpE97,sizeof(char),26))),_tag_dyneither(_tmpE96,sizeof(void*),0)));}
goto _LL4B4;_LL4B7: if((int)_tmp75F != 1)goto _LL4B9;_LL4B8: if(w > 16){const char*
_tmpE9A;void*_tmpE99;(_tmpE99=0,Cyc_Tcutil_terr(loc,((_tmpE9A="bitfield larger than type",
_tag_dyneither(_tmpE9A,sizeof(char),26))),_tag_dyneither(_tmpE99,sizeof(void*),0)));}
goto _LL4B4;_LL4B9: if((int)_tmp75F != 3)goto _LL4BB;_LL4BA: goto _LL4BC;_LL4BB: if((
int)_tmp75F != 2)goto _LL4BD;_LL4BC: if(w > 32){const char*_tmpE9D;void*_tmpE9C;(
_tmpE9C=0,Cyc_Tcutil_terr(loc,((_tmpE9D="bitfield larger than type",
_tag_dyneither(_tmpE9D,sizeof(char),26))),_tag_dyneither(_tmpE9C,sizeof(void*),0)));}
goto _LL4B4;_LL4BD: if((int)_tmp75F != 4)goto _LL4B4;_LL4BE: if(w > 64){const char*
_tmpEA0;void*_tmpE9F;(_tmpE9F=0,Cyc_Tcutil_terr(loc,((_tmpEA0="bitfield larger than type",
_tag_dyneither(_tmpEA0,sizeof(char),26))),_tag_dyneither(_tmpE9F,sizeof(void*),0)));}
goto _LL4B4;_LL4B4:;}goto _LL4AF;_LL4B2:;_LL4B3:{const char*_tmpEA5;void*_tmpEA4[2];
struct Cyc_String_pa_struct _tmpEA3;struct Cyc_String_pa_struct _tmpEA2;(_tmpEA2.tag=
0,((_tmpEA2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
field_typ)),((_tmpEA3.tag=0,((_tmpEA3.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*fn),((_tmpEA4[0]=& _tmpEA3,((_tmpEA4[1]=& _tmpEA2,Cyc_Tcutil_terr(
loc,((_tmpEA5="bitfield %s must have integral type but has type %s",
_tag_dyneither(_tmpEA5,sizeof(char),52))),_tag_dyneither(_tmpEA4,sizeof(void*),2)))))))))))));}
goto _LL4AF;_LL4AF:;}}}static void Cyc_Tcutil_check_field_atts(struct Cyc_Position_Segment*
loc,struct _dyneither_ptr*fn,struct Cyc_List_List*atts);static void Cyc_Tcutil_check_field_atts(
struct Cyc_Position_Segment*loc,struct _dyneither_ptr*fn,struct Cyc_List_List*atts){
for(0;atts != 0;atts=atts->tl){void*_tmp76C=(void*)atts->hd;_LL4C0: if((int)
_tmp76C != 5)goto _LL4C2;_LL4C1: continue;_LL4C2: if(_tmp76C <= (void*)17)goto _LL4C4;
if(*((int*)_tmp76C)!= 1)goto _LL4C4;_LL4C3: continue;_LL4C4:;_LL4C5: {const char*
_tmpEAA;void*_tmpEA9[2];struct Cyc_String_pa_struct _tmpEA8;struct Cyc_String_pa_struct
_tmpEA7;(_tmpEA7.tag=0,((_tmpEA7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*
fn),((_tmpEA8.tag=0,((_tmpEA8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absyn_attribute2string((void*)atts->hd)),((_tmpEA9[0]=& _tmpEA8,((_tmpEA9[1]=&
_tmpEA7,Cyc_Tcutil_terr(loc,((_tmpEAA="bad attribute %s on member %s",
_tag_dyneither(_tmpEAA,sizeof(char),30))),_tag_dyneither(_tmpEA9,sizeof(void*),2)))))))))))));}
_LL4BF:;}}struct Cyc_Tcutil_CVTEnv{struct _RegionHandle*r;struct Cyc_List_List*
kind_env;struct Cyc_List_List*free_vars;struct Cyc_List_List*free_evars;int
generalize_evars;int fn_result;};int Cyc_Tcutil_extract_const_from_typedef(struct
Cyc_Position_Segment*loc,int declared_const,void*t);int Cyc_Tcutil_extract_const_from_typedef(
struct Cyc_Position_Segment*loc,int declared_const,void*t){void*_tmp771=t;struct
Cyc_Absyn_Typedefdecl*_tmp772;void**_tmp773;_LL4C7: if(_tmp771 <= (void*)4)goto
_LL4C9;if(*((int*)_tmp771)!= 16)goto _LL4C9;_tmp772=((struct Cyc_Absyn_TypedefType_struct*)
_tmp771)->f3;_tmp773=((struct Cyc_Absyn_TypedefType_struct*)_tmp771)->f4;_LL4C8:
if((((struct Cyc_Absyn_Typedefdecl*)_check_null(_tmp772))->tq).real_const  || (
_tmp772->tq).print_const){if(declared_const){const char*_tmpEAD;void*_tmpEAC;(
_tmpEAC=0,Cyc_Tcutil_warn(loc,((_tmpEAD="extra const",_tag_dyneither(_tmpEAD,
sizeof(char),12))),_tag_dyneither(_tmpEAC,sizeof(void*),0)));}return 1;}if((
unsigned int)_tmp773)return Cyc_Tcutil_extract_const_from_typedef(loc,
declared_const,*_tmp773);else{return declared_const;}_LL4C9:;_LL4CA: return
declared_const;_LL4C6:;}static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type_level_exp(
struct Cyc_Absyn_Exp*e,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv);
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type(struct Cyc_Position_Segment*
loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv,void*expected_kind,
void*t,int put_in_effect);static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv
cvtenv,void*expected_kind,void*t,int put_in_effect){static struct Cyc_Core_Opt urgn={(
void*)((void*)3)};static struct Cyc_Core_Opt hrgn={(void*)((void*)2)};{void*_tmp776=
Cyc_Tcutil_compress(t);struct Cyc_Core_Opt*_tmp777;struct Cyc_Core_Opt**_tmp778;
struct Cyc_Core_Opt*_tmp779;struct Cyc_Core_Opt**_tmp77A;struct Cyc_Absyn_Tvar*
_tmp77B;struct Cyc_List_List*_tmp77C;struct _tuple2*_tmp77D;struct Cyc_Absyn_Enumdecl*
_tmp77E;struct Cyc_Absyn_Enumdecl**_tmp77F;struct Cyc_Absyn_TunionInfo _tmp780;
union Cyc_Absyn_TunionInfoU_union _tmp781;union Cyc_Absyn_TunionInfoU_union*_tmp782;
struct Cyc_List_List*_tmp783;struct Cyc_List_List**_tmp784;struct Cyc_Core_Opt*
_tmp785;struct Cyc_Core_Opt**_tmp786;struct Cyc_Absyn_TunionFieldInfo _tmp787;union
Cyc_Absyn_TunionFieldInfoU_union _tmp788;union Cyc_Absyn_TunionFieldInfoU_union*
_tmp789;struct Cyc_List_List*_tmp78A;struct Cyc_Absyn_PtrInfo _tmp78B;void*_tmp78C;
struct Cyc_Absyn_Tqual _tmp78D;struct Cyc_Absyn_Tqual*_tmp78E;struct Cyc_Absyn_PtrAtts
_tmp78F;void*_tmp790;struct Cyc_Absyn_Conref*_tmp791;struct Cyc_Absyn_Conref*
_tmp792;struct Cyc_Absyn_Conref*_tmp793;void*_tmp794;struct Cyc_Absyn_Exp*_tmp795;
struct Cyc_Absyn_ArrayInfo _tmp796;void*_tmp797;struct Cyc_Absyn_Tqual _tmp798;
struct Cyc_Absyn_Tqual*_tmp799;struct Cyc_Absyn_Exp*_tmp79A;struct Cyc_Absyn_Exp**
_tmp79B;struct Cyc_Absyn_Conref*_tmp79C;struct Cyc_Position_Segment*_tmp79D;struct
Cyc_Absyn_FnInfo _tmp79E;struct Cyc_List_List*_tmp79F;struct Cyc_List_List**_tmp7A0;
struct Cyc_Core_Opt*_tmp7A1;struct Cyc_Core_Opt**_tmp7A2;void*_tmp7A3;struct Cyc_List_List*
_tmp7A4;int _tmp7A5;struct Cyc_Absyn_VarargInfo*_tmp7A6;struct Cyc_List_List*
_tmp7A7;struct Cyc_List_List*_tmp7A8;struct Cyc_List_List*_tmp7A9;void*_tmp7AA;
struct Cyc_List_List*_tmp7AB;struct Cyc_Absyn_AggrInfo _tmp7AC;union Cyc_Absyn_AggrInfoU_union
_tmp7AD;union Cyc_Absyn_AggrInfoU_union*_tmp7AE;struct Cyc_List_List*_tmp7AF;
struct Cyc_List_List**_tmp7B0;struct _tuple2*_tmp7B1;struct Cyc_List_List*_tmp7B2;
struct Cyc_List_List**_tmp7B3;struct Cyc_Absyn_Typedefdecl*_tmp7B4;struct Cyc_Absyn_Typedefdecl**
_tmp7B5;void**_tmp7B6;void***_tmp7B7;void*_tmp7B8;void*_tmp7B9;void*_tmp7BA;void*
_tmp7BB;void*_tmp7BC;struct Cyc_List_List*_tmp7BD;_LL4CC: if((int)_tmp776 != 0)goto
_LL4CE;_LL4CD: goto _LL4CB;_LL4CE: if(_tmp776 <= (void*)4)goto _LL4E2;if(*((int*)
_tmp776)!= 0)goto _LL4D0;_tmp777=((struct Cyc_Absyn_Evar_struct*)_tmp776)->f1;
_tmp778=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)_tmp776)->f1;
_tmp779=((struct Cyc_Absyn_Evar_struct*)_tmp776)->f2;_tmp77A=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Evar_struct*)_tmp776)->f2;_LL4CF: if(*_tmp778 == 0)*_tmp778=Cyc_Tcutil_kind_to_opt(
expected_kind);if((cvtenv.fn_result  && cvtenv.generalize_evars) && Cyc_Tcutil_is_region_kind(
expected_kind)){if(expected_kind == (void*)4)*_tmp77A=(struct Cyc_Core_Opt*)& urgn;
else{*_tmp77A=(struct Cyc_Core_Opt*)& hrgn;}}else{if(cvtenv.generalize_evars){
struct Cyc_Absyn_Less_kb_struct _tmpEB0;struct Cyc_Absyn_Less_kb_struct*_tmpEAF;
struct Cyc_Absyn_Tvar*_tmp7BE=Cyc_Tcutil_new_tvar((void*)((_tmpEAF=_cycalloc(
sizeof(*_tmpEAF)),((_tmpEAF[0]=((_tmpEB0.tag=2,((_tmpEB0.f1=0,((_tmpEB0.f2=(void*)
expected_kind,_tmpEB0)))))),_tmpEAF)))));{struct Cyc_Absyn_VarType_struct*_tmpEB6;
struct Cyc_Absyn_VarType_struct _tmpEB5;struct Cyc_Core_Opt*_tmpEB4;*_tmp77A=((
_tmpEB4=_cycalloc(sizeof(*_tmpEB4)),((_tmpEB4->v=(void*)((void*)((_tmpEB6=
_cycalloc(sizeof(*_tmpEB6)),((_tmpEB6[0]=((_tmpEB5.tag=1,((_tmpEB5.f1=_tmp7BE,
_tmpEB5)))),_tmpEB6))))),_tmpEB4))));}_tmp77B=_tmp7BE;goto _LL4D1;}else{cvtenv.free_evars=
Cyc_Tcutil_add_free_evar(cvtenv.r,cvtenv.free_evars,t,put_in_effect);}}goto
_LL4CB;_LL4D0: if(*((int*)_tmp776)!= 1)goto _LL4D2;_tmp77B=((struct Cyc_Absyn_VarType_struct*)
_tmp776)->f1;_LL4D1:{void*_tmp7C4=Cyc_Absyn_compress_kb((void*)_tmp77B->kind);
struct Cyc_Core_Opt*_tmp7C5;struct Cyc_Core_Opt**_tmp7C6;_LL501: if(*((int*)_tmp7C4)
!= 1)goto _LL503;_tmp7C5=((struct Cyc_Absyn_Unknown_kb_struct*)_tmp7C4)->f1;
_tmp7C6=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)_tmp7C4)->f1;
_LL502:{struct Cyc_Absyn_Less_kb_struct*_tmpEBC;struct Cyc_Absyn_Less_kb_struct
_tmpEBB;struct Cyc_Core_Opt*_tmpEBA;*_tmp7C6=((_tmpEBA=_cycalloc(sizeof(*_tmpEBA)),((
_tmpEBA->v=(void*)((void*)((_tmpEBC=_cycalloc(sizeof(*_tmpEBC)),((_tmpEBC[0]=((
_tmpEBB.tag=2,((_tmpEBB.f1=0,((_tmpEBB.f2=(void*)expected_kind,_tmpEBB)))))),
_tmpEBC))))),_tmpEBA))));}goto _LL500;_LL503:;_LL504: goto _LL500;_LL500:;}cvtenv.kind_env=
Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmp77B);cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(
cvtenv.r,cvtenv.free_vars,_tmp77B,put_in_effect);{void*_tmp7CA=Cyc_Absyn_compress_kb((
void*)_tmp77B->kind);struct Cyc_Core_Opt*_tmp7CB;struct Cyc_Core_Opt**_tmp7CC;void*
_tmp7CD;_LL506: if(*((int*)_tmp7CA)!= 2)goto _LL508;_tmp7CB=((struct Cyc_Absyn_Less_kb_struct*)
_tmp7CA)->f1;_tmp7CC=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp7CA)->f1;_tmp7CD=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp7CA)->f2;
_LL507: if(Cyc_Tcutil_kind_leq(expected_kind,_tmp7CD)){struct Cyc_Absyn_Less_kb_struct*
_tmpEC2;struct Cyc_Absyn_Less_kb_struct _tmpEC1;struct Cyc_Core_Opt*_tmpEC0;*
_tmp7CC=((_tmpEC0=_cycalloc(sizeof(*_tmpEC0)),((_tmpEC0->v=(void*)((void*)((
_tmpEC2=_cycalloc(sizeof(*_tmpEC2)),((_tmpEC2[0]=((_tmpEC1.tag=2,((_tmpEC1.f1=0,((
_tmpEC1.f2=(void*)expected_kind,_tmpEC1)))))),_tmpEC2))))),_tmpEC0))));}goto
_LL505;_LL508:;_LL509: goto _LL505;_LL505:;}goto _LL4CB;_LL4D2: if(*((int*)_tmp776)
!= 13)goto _LL4D4;_tmp77C=((struct Cyc_Absyn_AnonEnumType_struct*)_tmp776)->f1;
_LL4D3: {struct Cyc_Tcenv_Genv*ge=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,
struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);{struct _RegionHandle
_tmp7D1=_new_region("uprev_rgn");struct _RegionHandle*uprev_rgn=& _tmp7D1;
_push_region(uprev_rgn);{struct Cyc_List_List*prev_fields=0;unsigned int tag_count=
0;for(0;_tmp77C != 0;_tmp77C=_tmp77C->tl){struct Cyc_Absyn_Enumfield*_tmp7D2=(
struct Cyc_Absyn_Enumfield*)_tmp77C->hd;if(((int(*)(int(*compare)(struct
_dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct
_dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,(*_tmp7D2->name).f2)){
const char*_tmpEC6;void*_tmpEC5[1];struct Cyc_String_pa_struct _tmpEC4;(_tmpEC4.tag=
0,((_tmpEC4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp7D2->name).f2),((
_tmpEC5[0]=& _tmpEC4,Cyc_Tcutil_terr(_tmp7D2->loc,((_tmpEC6="duplicate enum field name %s",
_tag_dyneither(_tmpEC6,sizeof(char),29))),_tag_dyneither(_tmpEC5,sizeof(void*),1)))))));}
else{struct Cyc_List_List*_tmpEC7;prev_fields=((_tmpEC7=_region_malloc(uprev_rgn,
sizeof(*_tmpEC7)),((_tmpEC7->hd=(*_tmp7D2->name).f2,((_tmpEC7->tl=prev_fields,
_tmpEC7))))));}if(_tmp7D2->tag == 0)_tmp7D2->tag=(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(
tag_count,_tmp7D2->loc);else{if(!Cyc_Tcutil_is_const_exp(te,(struct Cyc_Absyn_Exp*)
_check_null(_tmp7D2->tag))){const char*_tmpECB;void*_tmpECA[1];struct Cyc_String_pa_struct
_tmpEC9;(_tmpEC9.tag=0,((_tmpEC9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*
_tmp7D2->name).f2),((_tmpECA[0]=& _tmpEC9,Cyc_Tcutil_terr(loc,((_tmpECB="enum field %s: expression is not constant",
_tag_dyneither(_tmpECB,sizeof(char),42))),_tag_dyneither(_tmpECA,sizeof(void*),1)))))));}}{
unsigned int t1=(Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)_check_null(
_tmp7D2->tag))).f1;tag_count=t1 + 1;{union Cyc_Absyn_Nmspace_union _tmpECC;(*
_tmp7D2->name).f1=(union Cyc_Absyn_Nmspace_union)(((_tmpECC.Abs_n).tag=2,(((
_tmpECC.Abs_n).f1=te->ns,_tmpECC))));}{struct Cyc_Tcenv_AnonEnumRes_struct*
_tmpED2;struct Cyc_Tcenv_AnonEnumRes_struct _tmpED1;struct _tuple19*_tmpED0;ge->ordinaries=((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct
_tuple19*v))Cyc_Dict_insert)(ge->ordinaries,(*_tmp7D2->name).f2,(struct _tuple19*)((
_tmpED0=_cycalloc(sizeof(*_tmpED0)),((_tmpED0->f1=(void*)((_tmpED2=_cycalloc(
sizeof(*_tmpED2)),((_tmpED2[0]=((_tmpED1.tag=4,((_tmpED1.f1=(void*)t,((_tmpED1.f2=
_tmp7D2,_tmpED1)))))),_tmpED2)))),((_tmpED0->f2=1,_tmpED0)))))));}}}};
_pop_region(uprev_rgn);}goto _LL4CB;}_LL4D4: if(*((int*)_tmp776)!= 12)goto _LL4D6;
_tmp77D=((struct Cyc_Absyn_EnumType_struct*)_tmp776)->f1;_tmp77E=((struct Cyc_Absyn_EnumType_struct*)
_tmp776)->f2;_tmp77F=(struct Cyc_Absyn_Enumdecl**)&((struct Cyc_Absyn_EnumType_struct*)
_tmp776)->f2;_LL4D5: if(*_tmp77F == 0  || ((struct Cyc_Absyn_Enumdecl*)_check_null(*
_tmp77F))->fields == 0){struct _handler_cons _tmp7DE;_push_handler(& _tmp7DE);{int
_tmp7E0=0;if(setjmp(_tmp7DE.handler))_tmp7E0=1;if(!_tmp7E0){{struct Cyc_Absyn_Enumdecl**
ed=Cyc_Tcenv_lookup_enumdecl(te,loc,_tmp77D);*_tmp77F=(struct Cyc_Absyn_Enumdecl*)*
ed;};_pop_handler();}else{void*_tmp7DF=(void*)_exn_thrown;void*_tmp7E2=_tmp7DF;
_LL50B: if(_tmp7E2 != Cyc_Dict_Absent)goto _LL50D;_LL50C: {struct Cyc_Tcenv_Genv*
_tmp7E3=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))
Cyc_Dict_lookup)(te->ae,te->ns);struct Cyc_Absyn_Enumdecl*_tmpED3;struct Cyc_Absyn_Enumdecl*
_tmp7E4=(_tmpED3=_cycalloc(sizeof(*_tmpED3)),((_tmpED3->sc=(void*)((void*)3),((
_tmpED3->name=_tmp77D,((_tmpED3->fields=0,_tmpED3)))))));Cyc_Tc_tcEnumdecl(te,
_tmp7E3,loc,_tmp7E4);{struct Cyc_Absyn_Enumdecl**ed=Cyc_Tcenv_lookup_enumdecl(te,
loc,_tmp77D);*_tmp77F=(struct Cyc_Absyn_Enumdecl*)*ed;goto _LL50A;}}_LL50D:;_LL50E:(
void)_throw(_tmp7E2);_LL50A:;}}}{struct Cyc_Absyn_Enumdecl*ed=(struct Cyc_Absyn_Enumdecl*)
_check_null(*_tmp77F);*_tmp77D=(ed->name)[_check_known_subscript_notnull(1,0)];
goto _LL4CB;}_LL4D6: if(*((int*)_tmp776)!= 2)goto _LL4D8;_tmp780=((struct Cyc_Absyn_TunionType_struct*)
_tmp776)->f1;_tmp781=_tmp780.tunion_info;_tmp782=(union Cyc_Absyn_TunionInfoU_union*)&(((
struct Cyc_Absyn_TunionType_struct*)_tmp776)->f1).tunion_info;_tmp783=_tmp780.targs;
_tmp784=(struct Cyc_List_List**)&(((struct Cyc_Absyn_TunionType_struct*)_tmp776)->f1).targs;
_tmp785=_tmp780.rgn;_tmp786=(struct Cyc_Core_Opt**)&(((struct Cyc_Absyn_TunionType_struct*)
_tmp776)->f1).rgn;_LL4D7: {struct Cyc_List_List*_tmp7E6=*_tmp784;{union Cyc_Absyn_TunionInfoU_union
_tmp7E7=*_tmp782;struct Cyc_Absyn_UnknownTunionInfo _tmp7E8;struct _tuple2*_tmp7E9;
int _tmp7EA;int _tmp7EB;struct Cyc_Absyn_Tuniondecl**_tmp7EC;struct Cyc_Absyn_Tuniondecl*
_tmp7ED;_LL510: if((_tmp7E7.UnknownTunion).tag != 0)goto _LL512;_tmp7E8=(_tmp7E7.UnknownTunion).f1;
_tmp7E9=_tmp7E8.name;_tmp7EA=_tmp7E8.is_xtunion;_tmp7EB=_tmp7E8.is_flat;_LL511: {
struct Cyc_Absyn_Tuniondecl**tudp;{struct _handler_cons _tmp7EE;_push_handler(&
_tmp7EE);{int _tmp7F0=0;if(setjmp(_tmp7EE.handler))_tmp7F0=1;if(!_tmp7F0){tudp=
Cyc_Tcenv_lookup_tuniondecl(te,loc,_tmp7E9);;_pop_handler();}else{void*_tmp7EF=(
void*)_exn_thrown;void*_tmp7F2=_tmp7EF;_LL515: if(_tmp7F2 != Cyc_Dict_Absent)goto
_LL517;_LL516: {struct Cyc_Tcenv_Genv*_tmp7F3=((struct Cyc_Tcenv_Genv*(*)(struct
Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);struct Cyc_Absyn_Tuniondecl*
_tmpED4;struct Cyc_Absyn_Tuniondecl*_tmp7F4=(_tmpED4=_cycalloc(sizeof(*_tmpED4)),((
_tmpED4->sc=(void*)((void*)3),((_tmpED4->name=_tmp7E9,((_tmpED4->tvs=0,((_tmpED4->fields=
0,((_tmpED4->is_xtunion=_tmp7EA,((_tmpED4->is_flat=_tmp7EB,_tmpED4)))))))))))));
Cyc_Tc_tcTuniondecl(te,_tmp7F3,loc,_tmp7F4);tudp=Cyc_Tcenv_lookup_tuniondecl(te,
loc,_tmp7E9);if(_tmp7E6 != 0){{const char*_tmpEDD;const char*_tmpEDC;const char*
_tmpEDB;void*_tmpEDA[2];struct Cyc_String_pa_struct _tmpED9;struct Cyc_String_pa_struct
_tmpED8;(_tmpED8.tag=0,((_tmpED8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp7E9)),((_tmpED9.tag=0,((_tmpED9.f1=(struct
_dyneither_ptr)(_tmp7EA?(struct _dyneither_ptr)((struct _dyneither_ptr)((_tmpEDC="xtunion",
_tag_dyneither(_tmpEDC,sizeof(char),8)))):(struct _dyneither_ptr)((_tmpEDD="tunion",
_tag_dyneither(_tmpEDD,sizeof(char),7)))),((_tmpEDA[0]=& _tmpED9,((_tmpEDA[1]=&
_tmpED8,Cyc_Tcutil_terr(loc,((_tmpEDB="declare parameterized %s %s before using",
_tag_dyneither(_tmpEDB,sizeof(char),41))),_tag_dyneither(_tmpEDA,sizeof(void*),2)))))))))))));}
return cvtenv;}goto _LL514;}_LL517:;_LL518:(void)_throw(_tmp7F2);_LL514:;}}}if((*
tudp)->is_xtunion != _tmp7EA){const char*_tmpEE1;void*_tmpEE0[1];struct Cyc_String_pa_struct
_tmpEDF;(_tmpEDF.tag=0,((_tmpEDF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp7E9)),((_tmpEE0[0]=& _tmpEDF,Cyc_Tcutil_terr(loc,((
_tmpEE1="[x]tunion is different from type declaration %s",_tag_dyneither(_tmpEE1,
sizeof(char),48))),_tag_dyneither(_tmpEE0,sizeof(void*),1)))))));}{union Cyc_Absyn_TunionInfoU_union
_tmpEE2;*_tmp782=(union Cyc_Absyn_TunionInfoU_union)(((_tmpEE2.KnownTunion).tag=1,(((
_tmpEE2.KnownTunion).f1=tudp,_tmpEE2))));}_tmp7ED=*tudp;goto _LL513;}_LL512: if((
_tmp7E7.KnownTunion).tag != 1)goto _LL50F;_tmp7EC=(_tmp7E7.KnownTunion).f1;_tmp7ED=*
_tmp7EC;_LL513: if(_tmp7ED->is_flat  && (unsigned int)*_tmp786){const char*_tmpEE6;
void*_tmpEE5[1];struct Cyc_String_pa_struct _tmpEE4;(_tmpEE4.tag=0,((_tmpEE4.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)((
struct Cyc_Core_Opt*)_check_null(*_tmp786))->v)),((_tmpEE5[0]=& _tmpEE4,Cyc_Tcutil_terr(
loc,((_tmpEE6="flat tunion has region %s",_tag_dyneither(_tmpEE6,sizeof(char),26))),
_tag_dyneither(_tmpEE5,sizeof(void*),1)))))));}if(!_tmp7ED->is_flat  && !((
unsigned int)*_tmp786)){struct Cyc_Core_Opt*_tmpEE7;*_tmp786=((_tmpEE7=_cycalloc(
sizeof(*_tmpEE7)),((_tmpEE7->v=(void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_rk,0),_tmpEE7))));}if((unsigned int)*_tmp786){void*_tmp804=(void*)((
struct Cyc_Core_Opt*)_check_null(*_tmp786))->v;cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,(void*)3,_tmp804,1);}{struct Cyc_List_List*tvs=_tmp7ED->tvs;for(0;
_tmp7E6 != 0  && tvs != 0;(_tmp7E6=_tmp7E6->tl,tvs=tvs->tl)){void*t1=(void*)_tmp7E6->hd;
void*k1=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd);cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,k1,t1,1);}if(_tmp7E6 != 0){const char*_tmpEEB;void*_tmpEEA[1];struct
Cyc_String_pa_struct _tmpEE9;(_tmpEE9.tag=0,((_tmpEE9.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp7ED->name)),((_tmpEEA[0]=&
_tmpEE9,Cyc_Tcutil_terr(loc,((_tmpEEB="too many type arguments for tunion %s",
_tag_dyneither(_tmpEEB,sizeof(char),38))),_tag_dyneither(_tmpEEA,sizeof(void*),1)))))));}
if(tvs != 0){struct Cyc_List_List*hidden_ts=0;for(0;tvs != 0;tvs=tvs->tl){void*k1=
Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd);void*e=Cyc_Absyn_new_evar(0,
0);{struct Cyc_List_List*_tmpEEC;hidden_ts=((_tmpEEC=_cycalloc(sizeof(*_tmpEEC)),((
_tmpEEC->hd=(void*)e,((_tmpEEC->tl=hidden_ts,_tmpEEC))))));}cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,k1,e,1);}*_tmp784=Cyc_List_imp_append(*_tmp784,Cyc_List_imp_rev(
hidden_ts));}goto _LL50F;}_LL50F:;}goto _LL4CB;}_LL4D8: if(*((int*)_tmp776)!= 3)
goto _LL4DA;_tmp787=((struct Cyc_Absyn_TunionFieldType_struct*)_tmp776)->f1;
_tmp788=_tmp787.field_info;_tmp789=(union Cyc_Absyn_TunionFieldInfoU_union*)&(((
struct Cyc_Absyn_TunionFieldType_struct*)_tmp776)->f1).field_info;_tmp78A=_tmp787.targs;
_LL4D9:{union Cyc_Absyn_TunionFieldInfoU_union _tmp809=*_tmp789;struct Cyc_Absyn_UnknownTunionFieldInfo
_tmp80A;struct _tuple2*_tmp80B;struct _tuple2*_tmp80C;int _tmp80D;struct Cyc_Absyn_Tuniondecl*
_tmp80E;struct Cyc_Absyn_Tunionfield*_tmp80F;_LL51A: if((_tmp809.UnknownTunionfield).tag
!= 0)goto _LL51C;_tmp80A=(_tmp809.UnknownTunionfield).f1;_tmp80B=_tmp80A.tunion_name;
_tmp80C=_tmp80A.field_name;_tmp80D=_tmp80A.is_xtunion;_LL51B: {struct Cyc_Absyn_Tuniondecl*
tud;struct Cyc_Absyn_Tunionfield*tuf;{struct _handler_cons _tmp810;_push_handler(&
_tmp810);{int _tmp812=0;if(setjmp(_tmp810.handler))_tmp812=1;if(!_tmp812){*Cyc_Tcenv_lookup_tuniondecl(
te,loc,_tmp80B);;_pop_handler();}else{void*_tmp811=(void*)_exn_thrown;void*
_tmp814=_tmp811;_LL51F: if(_tmp814 != Cyc_Dict_Absent)goto _LL521;_LL520:{const char*
_tmpEF0;void*_tmpEEF[1];struct Cyc_String_pa_struct _tmpEEE;(_tmpEEE.tag=0,((
_tmpEEE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp80B)),((_tmpEEF[0]=& _tmpEEE,Cyc_Tcutil_terr(loc,((_tmpEF0="unbound type tunion %s",
_tag_dyneither(_tmpEF0,sizeof(char),23))),_tag_dyneither(_tmpEEF,sizeof(void*),1)))))));}
return cvtenv;_LL521:;_LL522:(void)_throw(_tmp814);_LL51E:;}}}{struct
_handler_cons _tmp818;_push_handler(& _tmp818);{int _tmp81A=0;if(setjmp(_tmp818.handler))
_tmp81A=1;if(!_tmp81A){{struct _RegionHandle _tmp81B=_new_region("r");struct
_RegionHandle*r=& _tmp81B;_push_region(r);{void*_tmp81C=Cyc_Tcenv_lookup_ordinary(
r,te,loc,_tmp80C);struct Cyc_Absyn_Tuniondecl*_tmp81D;struct Cyc_Absyn_Tunionfield*
_tmp81E;_LL524: if(*((int*)_tmp81C)!= 2)goto _LL526;_tmp81D=((struct Cyc_Tcenv_TunionRes_struct*)
_tmp81C)->f1;_tmp81E=((struct Cyc_Tcenv_TunionRes_struct*)_tmp81C)->f2;_LL525: tuf=
_tmp81E;tud=_tmp81D;if(tud->is_xtunion != _tmp80D){const char*_tmpEF4;void*_tmpEF3[
1];struct Cyc_String_pa_struct _tmpEF2;(_tmpEF2.tag=0,((_tmpEF2.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp80B)),((
_tmpEF3[0]=& _tmpEF2,Cyc_Tcutil_terr(loc,((_tmpEF4="[x]tunion is different from type declaration %s",
_tag_dyneither(_tmpEF4,sizeof(char),48))),_tag_dyneither(_tmpEF3,sizeof(void*),1)))))));}
goto _LL523;_LL526:;_LL527:{const char*_tmpEF9;void*_tmpEF8[2];struct Cyc_String_pa_struct
_tmpEF7;struct Cyc_String_pa_struct _tmpEF6;(_tmpEF6.tag=0,((_tmpEF6.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp80B)),((
_tmpEF7.tag=0,((_tmpEF7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp80C)),((_tmpEF8[0]=& _tmpEF7,((_tmpEF8[1]=& _tmpEF6,Cyc_Tcutil_terr(loc,((
_tmpEF9="unbound field %s in type tunion %s",_tag_dyneither(_tmpEF9,sizeof(char),
35))),_tag_dyneither(_tmpEF8,sizeof(void*),2)))))))))))));}{struct Cyc_Tcutil_CVTEnv
_tmp826=cvtenv;_npop_handler(1);return _tmp826;}_LL523:;};_pop_region(r);};
_pop_handler();}else{void*_tmp819=(void*)_exn_thrown;void*_tmp828=_tmp819;_LL529:
if(_tmp828 != Cyc_Dict_Absent)goto _LL52B;_LL52A:{const char*_tmpEFE;void*_tmpEFD[2];
struct Cyc_String_pa_struct _tmpEFC;struct Cyc_String_pa_struct _tmpEFB;(_tmpEFB.tag=
0,((_tmpEFB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp80B)),((_tmpEFC.tag=0,((_tmpEFC.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp80C)),((_tmpEFD[0]=& _tmpEFC,((_tmpEFD[
1]=& _tmpEFB,Cyc_Tcutil_terr(loc,((_tmpEFE="unbound field %s in type tunion %s",
_tag_dyneither(_tmpEFE,sizeof(char),35))),_tag_dyneither(_tmpEFD,sizeof(void*),2)))))))))))));}
return cvtenv;_LL52B:;_LL52C:(void)_throw(_tmp828);_LL528:;}}}{union Cyc_Absyn_TunionFieldInfoU_union
_tmpEFF;*_tmp789=(union Cyc_Absyn_TunionFieldInfoU_union)(((_tmpEFF.KnownTunionfield).tag=
1,(((_tmpEFF.KnownTunionfield).f1=tud,(((_tmpEFF.KnownTunionfield).f2=tuf,
_tmpEFF))))));}_tmp80E=tud;_tmp80F=tuf;goto _LL51D;}_LL51C: if((_tmp809.KnownTunionfield).tag
!= 1)goto _LL519;_tmp80E=(_tmp809.KnownTunionfield).f1;_tmp80F=(_tmp809.KnownTunionfield).f2;
_LL51D: {struct Cyc_List_List*tvs=_tmp80E->tvs;for(0;_tmp78A != 0  && tvs != 0;(
_tmp78A=_tmp78A->tl,tvs=tvs->tl)){void*t1=(void*)_tmp78A->hd;void*k1=Cyc_Tcutil_tvar_kind((
struct Cyc_Absyn_Tvar*)tvs->hd);cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,
cvtenv,k1,t1,1);}if(_tmp78A != 0){const char*_tmpF04;void*_tmpF03[2];struct Cyc_String_pa_struct
_tmpF02;struct Cyc_String_pa_struct _tmpF01;(_tmpF01.tag=0,((_tmpF01.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp80F->name)),((
_tmpF02.tag=0,((_tmpF02.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp80E->name)),((_tmpF03[0]=& _tmpF02,((_tmpF03[1]=& _tmpF01,Cyc_Tcutil_terr(loc,((
_tmpF04="too many type arguments for tunion %s.%s",_tag_dyneither(_tmpF04,
sizeof(char),41))),_tag_dyneither(_tmpF03,sizeof(void*),2)))))))))))));}if(tvs != 
0){const char*_tmpF09;void*_tmpF08[2];struct Cyc_String_pa_struct _tmpF07;struct Cyc_String_pa_struct
_tmpF06;(_tmpF06.tag=0,((_tmpF06.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp80F->name)),((_tmpF07.tag=0,((_tmpF07.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp80E->name)),((
_tmpF08[0]=& _tmpF07,((_tmpF08[1]=& _tmpF06,Cyc_Tcutil_terr(loc,((_tmpF09="too few type arguments for tunion %s.%s",
_tag_dyneither(_tmpF09,sizeof(char),40))),_tag_dyneither(_tmpF08,sizeof(void*),2)))))))))))));}
goto _LL519;}_LL519:;}goto _LL4CB;_LL4DA: if(*((int*)_tmp776)!= 4)goto _LL4DC;
_tmp78B=((struct Cyc_Absyn_PointerType_struct*)_tmp776)->f1;_tmp78C=(void*)
_tmp78B.elt_typ;_tmp78D=_tmp78B.elt_tq;_tmp78E=(struct Cyc_Absyn_Tqual*)&(((
struct Cyc_Absyn_PointerType_struct*)_tmp776)->f1).elt_tq;_tmp78F=_tmp78B.ptr_atts;
_tmp790=(void*)_tmp78F.rgn;_tmp791=_tmp78F.nullable;_tmp792=_tmp78F.bounds;
_tmp793=_tmp78F.zero_term;_LL4DB: {int is_zero_terminated;cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,(void*)0,_tmp78C,1);_tmp78E->real_const=Cyc_Tcutil_extract_const_from_typedef(
loc,_tmp78E->print_const,_tmp78C);cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,
cvtenv,(void*)5,_tmp790,1);{union Cyc_Absyn_Constraint_union _tmp836=(((struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref*x))Cyc_Absyn_compress_conref)(_tmp793))->v;int _tmp837;
_LL52E: if((_tmp836.No_constr).tag != 2)goto _LL530;_LL52F:{void*_tmp838=Cyc_Tcutil_compress(
_tmp78C);void*_tmp839;_LL535: if(_tmp838 <= (void*)4)goto _LL537;if(*((int*)_tmp838)
!= 5)goto _LL537;_tmp839=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp838)->f2;
if((int)_tmp839 != 0)goto _LL537;_LL536:((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*
x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp793,
Cyc_Absyn_true_conref);is_zero_terminated=1;goto _LL534;_LL537:;_LL538:((int(*)(
int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(
Cyc_Core_intcmp,_tmp793,Cyc_Absyn_false_conref);is_zero_terminated=0;goto _LL534;
_LL534:;}goto _LL52D;_LL530: if((_tmp836.Eq_constr).tag != 0)goto _LL532;_tmp837=(
_tmp836.Eq_constr).f1;if(_tmp837 != 1)goto _LL532;_LL531: if(!Cyc_Tcutil_admits_zero(
_tmp78C)){const char*_tmpF0D;void*_tmpF0C[1];struct Cyc_String_pa_struct _tmpF0B;(
_tmpF0B.tag=0,((_tmpF0B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
_tmp78C)),((_tmpF0C[0]=& _tmpF0B,Cyc_Tcutil_terr(loc,((_tmpF0D="cannot have a pointer to zero-terminated %s elements",
_tag_dyneither(_tmpF0D,sizeof(char),53))),_tag_dyneither(_tmpF0C,sizeof(void*),1)))))));}
is_zero_terminated=1;goto _LL52D;_LL532:;_LL533: is_zero_terminated=0;goto _LL52D;
_LL52D:;}{union Cyc_Absyn_Constraint_union _tmp83D=(Cyc_Absyn_compress_conref(
_tmp792))->v;void*_tmp83E;void*_tmp83F;struct Cyc_Absyn_Exp*_tmp840;_LL53A: if((
_tmp83D.No_constr).tag != 2)goto _LL53C;_LL53B: goto _LL53D;_LL53C: if((_tmp83D.Eq_constr).tag
!= 0)goto _LL53E;_tmp83E=(_tmp83D.Eq_constr).f1;if((int)_tmp83E != 0)goto _LL53E;
_LL53D: goto _LL539;_LL53E: if((_tmp83D.Eq_constr).tag != 0)goto _LL540;_tmp83F=(
_tmp83D.Eq_constr).f1;if(_tmp83F <= (void*)1)goto _LL540;if(*((int*)_tmp83F)!= 0)
goto _LL540;_tmp840=((struct Cyc_Absyn_Upper_b_struct*)_tmp83F)->f1;_LL53F:{struct
_RegionHandle _tmp841=_new_region("temp");struct _RegionHandle*temp=& _tmp841;
_push_region(temp);{struct Cyc_Tcenv_Tenv*_tmp842=Cyc_Tcenv_allow_valueof(temp,te);
Cyc_Tcexp_tcExp(_tmp842,0,_tmp840);};_pop_region(temp);}cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(
_tmp840,te,cvtenv);if(!Cyc_Tcutil_coerce_uint_typ(te,_tmp840)){const char*_tmpF10;
void*_tmpF0F;(_tmpF0F=0,Cyc_Tcutil_terr(loc,((_tmpF10="pointer bounds expression is not an unsigned int",
_tag_dyneither(_tmpF10,sizeof(char),49))),_tag_dyneither(_tmpF0F,sizeof(void*),0)));}{
unsigned int _tmp846;int _tmp847;struct _tuple7 _tmp845=Cyc_Evexp_eval_const_uint_exp(
_tmp840);_tmp846=_tmp845.f1;_tmp847=_tmp845.f2;if(is_zero_terminated  && (!
_tmp847  || _tmp846 < 1)){const char*_tmpF13;void*_tmpF12;(_tmpF12=0,Cyc_Tcutil_terr(
loc,((_tmpF13="zero-terminated pointer cannot point to empty sequence",
_tag_dyneither(_tmpF13,sizeof(char),55))),_tag_dyneither(_tmpF12,sizeof(void*),0)));}
goto _LL539;}_LL540: if((_tmp83D.Forward_constr).tag != 1)goto _LL539;_LL541: {const
char*_tmpF16;void*_tmpF15;(_tmpF15=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpF16="forward constraint",
_tag_dyneither(_tmpF16,sizeof(char),19))),_tag_dyneither(_tmpF15,sizeof(void*),0)));}
_LL539:;}goto _LL4CB;}_LL4DC: if(*((int*)_tmp776)!= 18)goto _LL4DE;_tmp794=(void*)((
struct Cyc_Absyn_TagType_struct*)_tmp776)->f1;_LL4DD: cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,(void*)7,_tmp794,1);goto _LL4CB;_LL4DE: if(*((int*)_tmp776)!= 17)
goto _LL4E0;_tmp795=((struct Cyc_Absyn_ValueofType_struct*)_tmp776)->f1;_LL4DF:{
struct _RegionHandle _tmp84C=_new_region("temp");struct _RegionHandle*temp=& _tmp84C;
_push_region(temp);{struct Cyc_Tcenv_Tenv*_tmp84D=Cyc_Tcenv_allow_valueof(temp,te);
Cyc_Tcexp_tcExp(_tmp84D,0,_tmp795);};_pop_region(temp);}if(!Cyc_Tcutil_coerce_uint_typ(
te,_tmp795)){const char*_tmpF19;void*_tmpF18;(_tmpF18=0,Cyc_Tcutil_terr(loc,((
_tmpF19="valueof_t requires an int expression",_tag_dyneither(_tmpF19,sizeof(
char),37))),_tag_dyneither(_tmpF18,sizeof(void*),0)));}cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(
_tmp795,te,cvtenv);goto _LL4CB;_LL4E0: if(*((int*)_tmp776)!= 5)goto _LL4E2;_LL4E1:
goto _LL4E3;_LL4E2: if((int)_tmp776 != 1)goto _LL4E4;_LL4E3: goto _LL4E5;_LL4E4: if(
_tmp776 <= (void*)4)goto _LL4F2;if(*((int*)_tmp776)!= 6)goto _LL4E6;_LL4E5: goto
_LL4CB;_LL4E6: if(*((int*)_tmp776)!= 7)goto _LL4E8;_tmp796=((struct Cyc_Absyn_ArrayType_struct*)
_tmp776)->f1;_tmp797=(void*)_tmp796.elt_type;_tmp798=_tmp796.tq;_tmp799=(struct
Cyc_Absyn_Tqual*)&(((struct Cyc_Absyn_ArrayType_struct*)_tmp776)->f1).tq;_tmp79A=
_tmp796.num_elts;_tmp79B=(struct Cyc_Absyn_Exp**)&(((struct Cyc_Absyn_ArrayType_struct*)
_tmp776)->f1).num_elts;_tmp79C=_tmp796.zero_term;_tmp79D=_tmp796.zt_loc;_LL4E7: {
struct Cyc_Absyn_Exp*_tmp850=*_tmp79B;cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,
cvtenv,(void*)1,_tmp797,1);_tmp799->real_const=Cyc_Tcutil_extract_const_from_typedef(
loc,_tmp799->print_const,_tmp797);{int is_zero_terminated;{union Cyc_Absyn_Constraint_union
_tmp851=(((struct Cyc_Absyn_Conref*(*)(struct Cyc_Absyn_Conref*x))Cyc_Absyn_compress_conref)(
_tmp79C))->v;int _tmp852;_LL543: if((_tmp851.No_constr).tag != 2)goto _LL545;_LL544:((
int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(
Cyc_Core_intcmp,_tmp79C,Cyc_Absyn_false_conref);is_zero_terminated=0;goto _LL542;
_LL545: if((_tmp851.Eq_constr).tag != 0)goto _LL547;_tmp852=(_tmp851.Eq_constr).f1;
if(_tmp852 != 1)goto _LL547;_LL546: if(!Cyc_Tcutil_admits_zero(_tmp797)){const char*
_tmpF1D;void*_tmpF1C[1];struct Cyc_String_pa_struct _tmpF1B;(_tmpF1B.tag=0,((
_tmpF1B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
_tmp797)),((_tmpF1C[0]=& _tmpF1B,Cyc_Tcutil_terr(loc,((_tmpF1D="cannot have a zero-terminated array of %s elements",
_tag_dyneither(_tmpF1D,sizeof(char),51))),_tag_dyneither(_tmpF1C,sizeof(void*),1)))))));}
is_zero_terminated=1;goto _LL542;_LL547:;_LL548: is_zero_terminated=0;goto _LL542;
_LL542:;}if(_tmp850 == 0){if(is_zero_terminated)*_tmp79B=(_tmp850=(struct Cyc_Absyn_Exp*)
Cyc_Absyn_uint_exp(1,0));else{{const char*_tmpF20;void*_tmpF1F;(_tmpF1F=0,Cyc_Tcutil_warn(
loc,((_tmpF20="array bound defaults to 1 here",_tag_dyneither(_tmpF20,sizeof(
char),31))),_tag_dyneither(_tmpF1F,sizeof(void*),0)));}*_tmp79B=(_tmp850=(struct
Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(1,0));}}Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)
_tmp850);if(!Cyc_Tcutil_is_const_exp(te,(struct Cyc_Absyn_Exp*)_tmp850)){const
char*_tmpF23;void*_tmpF22;(_tmpF22=0,Cyc_Tcutil_terr(loc,((_tmpF23="array bounds expression is not constant",
_tag_dyneither(_tmpF23,sizeof(char),40))),_tag_dyneither(_tmpF22,sizeof(void*),0)));}
if(!Cyc_Tcutil_coerce_uint_typ(te,(struct Cyc_Absyn_Exp*)_tmp850)){const char*
_tmpF26;void*_tmpF25;(_tmpF25=0,Cyc_Tcutil_terr(loc,((_tmpF26="array bounds expression is not an unsigned int",
_tag_dyneither(_tmpF26,sizeof(char),47))),_tag_dyneither(_tmpF25,sizeof(void*),0)));}{
unsigned int _tmp85D;int _tmp85E;struct _tuple7 _tmp85C=Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)_tmp850);_tmp85D=_tmp85C.f1;_tmp85E=_tmp85C.f2;if((
is_zero_terminated  && _tmp85E) && _tmp85D < 1){const char*_tmpF29;void*_tmpF28;(
_tmpF28=0,Cyc_Tcutil_warn(loc,((_tmpF29="zero terminated array cannot have zero elements",
_tag_dyneither(_tmpF29,sizeof(char),48))),_tag_dyneither(_tmpF28,sizeof(void*),0)));}
if((_tmp85E  && _tmp85D < 1) && Cyc_Cyclone_tovc_r){{const char*_tmpF2C;void*
_tmpF2B;(_tmpF2B=0,Cyc_Tcutil_warn(loc,((_tmpF2C="arrays with 0 elements are not supported except with gcc -- changing to 1.",
_tag_dyneither(_tmpF2C,sizeof(char),75))),_tag_dyneither(_tmpF2B,sizeof(void*),0)));}*
_tmp79B=(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(1,0);}goto _LL4CB;}}}_LL4E8: if(*((
int*)_tmp776)!= 8)goto _LL4EA;_tmp79E=((struct Cyc_Absyn_FnType_struct*)_tmp776)->f1;
_tmp79F=_tmp79E.tvars;_tmp7A0=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_struct*)
_tmp776)->f1).tvars;_tmp7A1=_tmp79E.effect;_tmp7A2=(struct Cyc_Core_Opt**)&(((
struct Cyc_Absyn_FnType_struct*)_tmp776)->f1).effect;_tmp7A3=(void*)_tmp79E.ret_typ;
_tmp7A4=_tmp79E.args;_tmp7A5=_tmp79E.c_varargs;_tmp7A6=_tmp79E.cyc_varargs;
_tmp7A7=_tmp79E.rgn_po;_tmp7A8=_tmp79E.attributes;_LL4E9: {int num_convs=0;int
seen_cdecl=0;int seen_stdcall=0;int seen_fastcall=0;int seen_format=0;void*ft=(void*)
0;int fmt_desc_arg=- 1;int fmt_arg_start=- 1;for(0;_tmp7A8 != 0;_tmp7A8=_tmp7A8->tl){
if(!Cyc_Absyn_fntype_att((void*)_tmp7A8->hd)){const char*_tmpF30;void*_tmpF2F[1];
struct Cyc_String_pa_struct _tmpF2E;(_tmpF2E.tag=0,((_tmpF2E.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((void*)_tmp7A8->hd)),((
_tmpF2F[0]=& _tmpF2E,Cyc_Tcutil_terr(loc,((_tmpF30="bad function type attribute %s",
_tag_dyneither(_tmpF30,sizeof(char),31))),_tag_dyneither(_tmpF2F,sizeof(void*),1)))))));}{
void*_tmp866=(void*)_tmp7A8->hd;void*_tmp867;int _tmp868;int _tmp869;_LL54A: if((
int)_tmp866 != 0)goto _LL54C;_LL54B: if(!seen_stdcall){seen_stdcall=1;++ num_convs;}
goto _LL549;_LL54C: if((int)_tmp866 != 1)goto _LL54E;_LL54D: if(!seen_cdecl){
seen_cdecl=1;++ num_convs;}goto _LL549;_LL54E: if((int)_tmp866 != 2)goto _LL550;
_LL54F: if(!seen_fastcall){seen_fastcall=1;++ num_convs;}goto _LL549;_LL550: if(
_tmp866 <= (void*)17)goto _LL552;if(*((int*)_tmp866)!= 3)goto _LL552;_tmp867=(void*)((
struct Cyc_Absyn_Format_att_struct*)_tmp866)->f1;_tmp868=((struct Cyc_Absyn_Format_att_struct*)
_tmp866)->f2;_tmp869=((struct Cyc_Absyn_Format_att_struct*)_tmp866)->f3;_LL551:
if(!seen_format){seen_format=1;ft=_tmp867;fmt_desc_arg=_tmp868;fmt_arg_start=
_tmp869;}else{const char*_tmpF33;void*_tmpF32;(_tmpF32=0,Cyc_Tcutil_terr(loc,((
_tmpF33="function can't have multiple format attributes",_tag_dyneither(_tmpF33,
sizeof(char),47))),_tag_dyneither(_tmpF32,sizeof(void*),0)));}goto _LL549;_LL552:;
_LL553: goto _LL549;_LL549:;}}if(num_convs > 1){const char*_tmpF36;void*_tmpF35;(
_tmpF35=0,Cyc_Tcutil_terr(loc,((_tmpF36="function can't have multiple calling conventions",
_tag_dyneither(_tmpF36,sizeof(char),49))),_tag_dyneither(_tmpF35,sizeof(void*),0)));}
Cyc_Tcutil_check_unique_tvars(loc,*_tmp7A0);{struct Cyc_List_List*b=*_tmp7A0;for(
0;b != 0;b=b->tl){((struct Cyc_Absyn_Tvar*)b->hd)->identity=Cyc_Tcutil_new_tvar_id();
cvtenv.kind_env=Cyc_Tcutil_add_bound_tvar(cvtenv.kind_env,(struct Cyc_Absyn_Tvar*)
b->hd);{void*_tmp86E=Cyc_Absyn_compress_kb((void*)((struct Cyc_Absyn_Tvar*)b->hd)->kind);
void*_tmp86F;_LL555: if(*((int*)_tmp86E)!= 0)goto _LL557;_tmp86F=(void*)((struct
Cyc_Absyn_Eq_kb_struct*)_tmp86E)->f1;if((int)_tmp86F != 1)goto _LL557;_LL556:{
const char*_tmpF3A;void*_tmpF39[1];struct Cyc_String_pa_struct _tmpF38;(_tmpF38.tag=
0,((_tmpF38.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)
b->hd)->name),((_tmpF39[0]=& _tmpF38,Cyc_Tcutil_terr(loc,((_tmpF3A="function attempts to abstract Mem type variable %s",
_tag_dyneither(_tmpF3A,sizeof(char),51))),_tag_dyneither(_tmpF39,sizeof(void*),1)))))));}
goto _LL554;_LL557:;_LL558: goto _LL554;_LL554:;}}}{struct _RegionHandle _tmp873=
_new_region("newr");struct _RegionHandle*newr=& _tmp873;_push_region(newr);{struct
Cyc_Tcutil_CVTEnv _tmpF3B;struct Cyc_Tcutil_CVTEnv _tmp874=(_tmpF3B.r=newr,((
_tmpF3B.kind_env=cvtenv.kind_env,((_tmpF3B.free_vars=0,((_tmpF3B.free_evars=0,((
_tmpF3B.generalize_evars=cvtenv.generalize_evars,((_tmpF3B.fn_result=1,_tmpF3B)))))))))));
_tmp874=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp874,(void*)1,_tmp7A3,1);_tmp874.fn_result=
0;{struct Cyc_List_List*a=_tmp7A4;for(0;a != 0;a=a->tl){struct _tuple3*_tmp875=(
struct _tuple3*)a->hd;void*_tmp876=(*_tmp875).f3;_tmp874=Cyc_Tcutil_i_check_valid_type(
loc,te,_tmp874,(void*)1,_tmp876,1);((*_tmp875).f2).real_const=Cyc_Tcutil_extract_const_from_typedef(
loc,((*_tmp875).f2).print_const,_tmp876);}}if(_tmp7A6 != 0){if(_tmp7A5){const char*
_tmpF3E;void*_tmpF3D;(_tmpF3D=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpF3E="both c_vararg and cyc_vararg",
_tag_dyneither(_tmpF3E,sizeof(char),29))),_tag_dyneither(_tmpF3D,sizeof(void*),0)));}{
void*_tmp87A;int _tmp87B;struct Cyc_Absyn_VarargInfo _tmp879=*_tmp7A6;_tmp87A=(void*)
_tmp879.type;_tmp87B=_tmp879.inject;_tmp874=Cyc_Tcutil_i_check_valid_type(loc,te,
_tmp874,(void*)1,_tmp87A,1);(_tmp7A6->tq).real_const=Cyc_Tcutil_extract_const_from_typedef(
loc,(_tmp7A6->tq).print_const,_tmp87A);if(_tmp87B){void*_tmp87C=Cyc_Tcutil_compress(
_tmp87A);struct Cyc_Absyn_TunionInfo _tmp87D;union Cyc_Absyn_TunionInfoU_union
_tmp87E;struct Cyc_Absyn_Tuniondecl**_tmp87F;_LL55A: if(_tmp87C <= (void*)4)goto
_LL55C;if(*((int*)_tmp87C)!= 2)goto _LL55C;_tmp87D=((struct Cyc_Absyn_TunionType_struct*)
_tmp87C)->f1;_tmp87E=_tmp87D.tunion_info;if((((((struct Cyc_Absyn_TunionType_struct*)
_tmp87C)->f1).tunion_info).KnownTunion).tag != 1)goto _LL55C;_tmp87F=(_tmp87E.KnownTunion).f1;
_LL55B: if((*_tmp87F)->is_flat){const char*_tmpF41;void*_tmpF40;(_tmpF40=0,Cyc_Tcutil_terr(
loc,((_tmpF41="cant inject into a flat tunion",_tag_dyneither(_tmpF41,sizeof(
char),31))),_tag_dyneither(_tmpF40,sizeof(void*),0)));}goto _LL559;_LL55C:;_LL55D:{
const char*_tmpF44;void*_tmpF43;(_tmpF43=0,Cyc_Tcutil_terr(loc,((_tmpF44="can't inject a non-[x]tunion type",
_tag_dyneither(_tmpF44,sizeof(char),34))),_tag_dyneither(_tmpF43,sizeof(void*),0)));}
goto _LL559;_LL559:;}}}if(seen_format){int _tmp884=((int(*)(struct Cyc_List_List*x))
Cyc_List_length)(_tmp7A4);if((((fmt_desc_arg < 0  || fmt_desc_arg > _tmp884) || 
fmt_arg_start < 0) || _tmp7A6 == 0  && fmt_arg_start != 0) || _tmp7A6 != 0  && 
fmt_arg_start != _tmp884 + 1){const char*_tmpF47;void*_tmpF46;(_tmpF46=0,Cyc_Tcutil_terr(
loc,((_tmpF47="bad format descriptor",_tag_dyneither(_tmpF47,sizeof(char),22))),
_tag_dyneither(_tmpF46,sizeof(void*),0)));}else{void*_tmp888;struct _tuple3
_tmp887=*((struct _tuple3*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp7A4,
fmt_desc_arg - 1);_tmp888=_tmp887.f3;{void*_tmp889=Cyc_Tcutil_compress(_tmp888);
struct Cyc_Absyn_PtrInfo _tmp88A;void*_tmp88B;struct Cyc_Absyn_PtrAtts _tmp88C;
struct Cyc_Absyn_Conref*_tmp88D;struct Cyc_Absyn_Conref*_tmp88E;_LL55F: if(_tmp889
<= (void*)4)goto _LL561;if(*((int*)_tmp889)!= 4)goto _LL561;_tmp88A=((struct Cyc_Absyn_PointerType_struct*)
_tmp889)->f1;_tmp88B=(void*)_tmp88A.elt_typ;_tmp88C=_tmp88A.ptr_atts;_tmp88D=
_tmp88C.bounds;_tmp88E=_tmp88C.zero_term;_LL560:{struct _tuple0 _tmpF48;struct
_tuple0 _tmp890=(_tmpF48.f1=Cyc_Tcutil_compress(_tmp88B),((_tmpF48.f2=Cyc_Absyn_conref_def((
void*)((void*)0),_tmp88D),_tmpF48)));void*_tmp891;void*_tmp892;void*_tmp893;void*
_tmp894;_LL564: _tmp891=_tmp890.f1;if(_tmp891 <= (void*)4)goto _LL566;if(*((int*)
_tmp891)!= 5)goto _LL566;_tmp892=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp891)->f1;
if((int)_tmp892 != 2)goto _LL566;_tmp893=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp891)->f2;if((int)_tmp893 != 0)goto _LL566;_tmp894=_tmp890.f2;if((int)_tmp894 != 
0)goto _LL566;_LL565: goto _LL563;_LL566:;_LL567:{const char*_tmpF4B;void*_tmpF4A;(
_tmpF4A=0,Cyc_Tcutil_terr(loc,((_tmpF4B="format descriptor is not a char ? type",
_tag_dyneither(_tmpF4B,sizeof(char),39))),_tag_dyneither(_tmpF4A,sizeof(void*),0)));}
goto _LL563;_LL563:;}goto _LL55E;_LL561:;_LL562:{const char*_tmpF4E;void*_tmpF4D;(
_tmpF4D=0,Cyc_Tcutil_terr(loc,((_tmpF4E="format descriptor is not a char ? type",
_tag_dyneither(_tmpF4E,sizeof(char),39))),_tag_dyneither(_tmpF4D,sizeof(void*),0)));}
goto _LL55E;_LL55E:;}if(fmt_arg_start != 0){int problem;{struct _tuple0 _tmpF4F;
struct _tuple0 _tmp89A=(_tmpF4F.f1=ft,((_tmpF4F.f2=Cyc_Tcutil_compress((void*)((
struct Cyc_Absyn_VarargInfo*)_check_null(_tmp7A6))->type),_tmpF4F)));void*_tmp89B;
void*_tmp89C;struct Cyc_Absyn_TunionInfo _tmp89D;union Cyc_Absyn_TunionInfoU_union
_tmp89E;struct Cyc_Absyn_Tuniondecl**_tmp89F;struct Cyc_Absyn_Tuniondecl*_tmp8A0;
void*_tmp8A1;void*_tmp8A2;struct Cyc_Absyn_TunionInfo _tmp8A3;union Cyc_Absyn_TunionInfoU_union
_tmp8A4;struct Cyc_Absyn_Tuniondecl**_tmp8A5;struct Cyc_Absyn_Tuniondecl*_tmp8A6;
_LL569: _tmp89B=_tmp89A.f1;if((int)_tmp89B != 0)goto _LL56B;_tmp89C=_tmp89A.f2;if(
_tmp89C <= (void*)4)goto _LL56B;if(*((int*)_tmp89C)!= 2)goto _LL56B;_tmp89D=((
struct Cyc_Absyn_TunionType_struct*)_tmp89C)->f1;_tmp89E=_tmp89D.tunion_info;if((((((
struct Cyc_Absyn_TunionType_struct*)_tmp89C)->f1).tunion_info).KnownTunion).tag != 
1)goto _LL56B;_tmp89F=(_tmp89E.KnownTunion).f1;_tmp8A0=*_tmp89F;_LL56A: problem=
Cyc_Absyn_qvar_cmp(_tmp8A0->name,Cyc_Absyn_tunion_print_arg_qvar)!= 0;goto _LL568;
_LL56B: _tmp8A1=_tmp89A.f1;if((int)_tmp8A1 != 1)goto _LL56D;_tmp8A2=_tmp89A.f2;if(
_tmp8A2 <= (void*)4)goto _LL56D;if(*((int*)_tmp8A2)!= 2)goto _LL56D;_tmp8A3=((
struct Cyc_Absyn_TunionType_struct*)_tmp8A2)->f1;_tmp8A4=_tmp8A3.tunion_info;if((((((
struct Cyc_Absyn_TunionType_struct*)_tmp8A2)->f1).tunion_info).KnownTunion).tag != 
1)goto _LL56D;_tmp8A5=(_tmp8A4.KnownTunion).f1;_tmp8A6=*_tmp8A5;_LL56C: problem=
Cyc_Absyn_qvar_cmp(_tmp8A6->name,Cyc_Absyn_tunion_scanf_arg_qvar)!= 0;goto _LL568;
_LL56D:;_LL56E: problem=1;goto _LL568;_LL568:;}if(problem){const char*_tmpF52;void*
_tmpF51;(_tmpF51=0,Cyc_Tcutil_terr(loc,((_tmpF52="format attribute and vararg types don't match",
_tag_dyneither(_tmpF52,sizeof(char),46))),_tag_dyneither(_tmpF51,sizeof(void*),0)));}}}}{
struct Cyc_List_List*rpo=_tmp7A7;for(0;rpo != 0;rpo=rpo->tl){struct _tuple0 _tmp8AA;
void*_tmp8AB;void*_tmp8AC;struct _tuple0*_tmp8A9=(struct _tuple0*)rpo->hd;_tmp8AA=*
_tmp8A9;_tmp8AB=_tmp8AA.f1;_tmp8AC=_tmp8AA.f2;_tmp874=Cyc_Tcutil_i_check_valid_type(
loc,te,_tmp874,(void*)6,_tmp8AB,1);_tmp874=Cyc_Tcutil_i_check_valid_type(loc,te,
_tmp874,(void*)5,_tmp8AC,1);}}if(*_tmp7A2 != 0)_tmp874=Cyc_Tcutil_i_check_valid_type(
loc,te,_tmp874,(void*)6,(void*)((struct Cyc_Core_Opt*)_check_null(*_tmp7A2))->v,1);
else{struct Cyc_List_List*effect=0;{struct Cyc_List_List*tvs=_tmp874.free_vars;
for(0;tvs != 0;tvs=tvs->tl){struct Cyc_Absyn_Tvar*_tmp8AE;int _tmp8AF;struct
_tuple18 _tmp8AD=*((struct _tuple18*)tvs->hd);_tmp8AE=_tmp8AD.f1;_tmp8AF=_tmp8AD.f2;
if(!_tmp8AF)continue;{void*_tmp8B0=Cyc_Absyn_compress_kb((void*)_tmp8AE->kind);
struct Cyc_Core_Opt*_tmp8B1;struct Cyc_Core_Opt**_tmp8B2;void*_tmp8B3;struct Cyc_Core_Opt*
_tmp8B4;struct Cyc_Core_Opt**_tmp8B5;void*_tmp8B6;void*_tmp8B7;void*_tmp8B8;void*
_tmp8B9;struct Cyc_Core_Opt*_tmp8BA;struct Cyc_Core_Opt**_tmp8BB;void*_tmp8BC;void*
_tmp8BD;struct Cyc_Core_Opt*_tmp8BE;struct Cyc_Core_Opt**_tmp8BF;_LL570: if(*((int*)
_tmp8B0)!= 2)goto _LL572;_tmp8B1=((struct Cyc_Absyn_Less_kb_struct*)_tmp8B0)->f1;
_tmp8B2=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)_tmp8B0)->f1;
_tmp8B3=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp8B0)->f2;if((int)_tmp8B3 != 
5)goto _LL572;_LL571: _tmp8B5=_tmp8B2;_tmp8B6=(void*)3;goto _LL573;_LL572: if(*((int*)
_tmp8B0)!= 2)goto _LL574;_tmp8B4=((struct Cyc_Absyn_Less_kb_struct*)_tmp8B0)->f1;
_tmp8B5=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)_tmp8B0)->f1;
_tmp8B6=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp8B0)->f2;if(!(_tmp8B6 == (
void*)3  || _tmp8B6 == (void*)4))goto _LL574;_LL573:*_tmp8B5=Cyc_Tcutil_kind_to_bound_opt(
_tmp8B6);_tmp8B7=_tmp8B6;goto _LL575;_LL574: if(*((int*)_tmp8B0)!= 0)goto _LL576;
_tmp8B7=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp8B0)->f1;if(!((_tmp8B7 == (
void*)3  || _tmp8B7 == (void*)4) || _tmp8B7 == (void*)5))goto _LL576;_LL575:{struct
Cyc_Absyn_AccessEff_struct*_tmpF61;struct Cyc_Absyn_VarType_struct*_tmpF60;struct
Cyc_Absyn_VarType_struct _tmpF5F;struct Cyc_Absyn_AccessEff_struct _tmpF5E;struct
Cyc_List_List*_tmpF5D;effect=((_tmpF5D=_cycalloc(sizeof(*_tmpF5D)),((_tmpF5D->hd=(
void*)((void*)((_tmpF61=_cycalloc(sizeof(*_tmpF61)),((_tmpF61[0]=((_tmpF5E.tag=
19,((_tmpF5E.f1=(void*)((void*)((_tmpF60=_cycalloc(sizeof(*_tmpF60)),((_tmpF60[0]=((
_tmpF5F.tag=1,((_tmpF5F.f1=_tmp8AE,_tmpF5F)))),_tmpF60))))),_tmpF5E)))),_tmpF61))))),((
_tmpF5D->tl=effect,_tmpF5D))))));}goto _LL56F;_LL576: if(*((int*)_tmp8B0)!= 2)goto
_LL578;_tmp8B8=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp8B0)->f2;if((int)
_tmp8B8 != 7)goto _LL578;_LL577: goto _LL579;_LL578: if(*((int*)_tmp8B0)!= 0)goto
_LL57A;_tmp8B9=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp8B0)->f1;if((int)
_tmp8B9 != 7)goto _LL57A;_LL579: goto _LL56F;_LL57A: if(*((int*)_tmp8B0)!= 2)goto
_LL57C;_tmp8BA=((struct Cyc_Absyn_Less_kb_struct*)_tmp8B0)->f1;_tmp8BB=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*)_tmp8B0)->f1;_tmp8BC=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp8B0)->f2;if((int)_tmp8BC != 6)goto _LL57C;_LL57B:*_tmp8BB=Cyc_Tcutil_kind_to_bound_opt((
void*)6);goto _LL57D;_LL57C: if(*((int*)_tmp8B0)!= 0)goto _LL57E;_tmp8BD=(void*)((
struct Cyc_Absyn_Eq_kb_struct*)_tmp8B0)->f1;if((int)_tmp8BD != 6)goto _LL57E;_LL57D:{
struct Cyc_Absyn_VarType_struct*_tmpF67;struct Cyc_Absyn_VarType_struct _tmpF66;
struct Cyc_List_List*_tmpF65;effect=((_tmpF65=_cycalloc(sizeof(*_tmpF65)),((
_tmpF65->hd=(void*)((void*)((_tmpF67=_cycalloc(sizeof(*_tmpF67)),((_tmpF67[0]=((
_tmpF66.tag=1,((_tmpF66.f1=_tmp8AE,_tmpF66)))),_tmpF67))))),((_tmpF65->tl=effect,
_tmpF65))))));}goto _LL56F;_LL57E: if(*((int*)_tmp8B0)!= 1)goto _LL580;_tmp8BE=((
struct Cyc_Absyn_Unknown_kb_struct*)_tmp8B0)->f1;_tmp8BF=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Unknown_kb_struct*)_tmp8B0)->f1;_LL57F:{struct Cyc_Absyn_Less_kb_struct*
_tmpF6D;struct Cyc_Absyn_Less_kb_struct _tmpF6C;struct Cyc_Core_Opt*_tmpF6B;*
_tmp8BF=((_tmpF6B=_cycalloc(sizeof(*_tmpF6B)),((_tmpF6B->v=(void*)((void*)((
_tmpF6D=_cycalloc(sizeof(*_tmpF6D)),((_tmpF6D[0]=((_tmpF6C.tag=2,((_tmpF6C.f1=0,((
_tmpF6C.f2=(void*)((void*)0),_tmpF6C)))))),_tmpF6D))))),_tmpF6B))));}goto _LL581;
_LL580:;_LL581:{struct Cyc_Absyn_RgnsEff_struct*_tmpF7C;struct Cyc_Absyn_VarType_struct*
_tmpF7B;struct Cyc_Absyn_VarType_struct _tmpF7A;struct Cyc_Absyn_RgnsEff_struct
_tmpF79;struct Cyc_List_List*_tmpF78;effect=((_tmpF78=_cycalloc(sizeof(*_tmpF78)),((
_tmpF78->hd=(void*)((void*)((_tmpF7C=_cycalloc(sizeof(*_tmpF7C)),((_tmpF7C[0]=((
_tmpF79.tag=21,((_tmpF79.f1=(void*)((void*)((_tmpF7B=_cycalloc(sizeof(*_tmpF7B)),((
_tmpF7B[0]=((_tmpF7A.tag=1,((_tmpF7A.f1=_tmp8AE,_tmpF7A)))),_tmpF7B))))),_tmpF79)))),
_tmpF7C))))),((_tmpF78->tl=effect,_tmpF78))))));}goto _LL56F;_LL56F:;}}}{struct
Cyc_List_List*ts=_tmp874.free_evars;for(0;ts != 0;ts=ts->tl){void*_tmp8D1;int
_tmp8D2;struct _tuple19 _tmp8D0=*((struct _tuple19*)ts->hd);_tmp8D1=_tmp8D0.f1;
_tmp8D2=_tmp8D0.f2;if(!_tmp8D2)continue;{void*_tmp8D3=Cyc_Tcutil_typ_kind(
_tmp8D1);_LL583: if((int)_tmp8D3 != 5)goto _LL585;_LL584: goto _LL586;_LL585: if((int)
_tmp8D3 != 4)goto _LL587;_LL586: goto _LL588;_LL587: if((int)_tmp8D3 != 3)goto _LL589;
_LL588:{struct Cyc_Absyn_AccessEff_struct*_tmpF82;struct Cyc_Absyn_AccessEff_struct
_tmpF81;struct Cyc_List_List*_tmpF80;effect=((_tmpF80=_cycalloc(sizeof(*_tmpF80)),((
_tmpF80->hd=(void*)((void*)((_tmpF82=_cycalloc(sizeof(*_tmpF82)),((_tmpF82[0]=((
_tmpF81.tag=19,((_tmpF81.f1=(void*)_tmp8D1,_tmpF81)))),_tmpF82))))),((_tmpF80->tl=
effect,_tmpF80))))));}goto _LL582;_LL589: if((int)_tmp8D3 != 6)goto _LL58B;_LL58A:{
struct Cyc_List_List*_tmpF83;effect=((_tmpF83=_cycalloc(sizeof(*_tmpF83)),((
_tmpF83->hd=(void*)_tmp8D1,((_tmpF83->tl=effect,_tmpF83))))));}goto _LL582;_LL58B:
if((int)_tmp8D3 != 7)goto _LL58D;_LL58C: goto _LL582;_LL58D:;_LL58E:{struct Cyc_Absyn_RgnsEff_struct*
_tmpF89;struct Cyc_Absyn_RgnsEff_struct _tmpF88;struct Cyc_List_List*_tmpF87;effect=((
_tmpF87=_cycalloc(sizeof(*_tmpF87)),((_tmpF87->hd=(void*)((void*)((_tmpF89=
_cycalloc(sizeof(*_tmpF89)),((_tmpF89[0]=((_tmpF88.tag=21,((_tmpF88.f1=(void*)
_tmp8D1,_tmpF88)))),_tmpF89))))),((_tmpF87->tl=effect,_tmpF87))))));}goto _LL582;
_LL582:;}}}{struct Cyc_Absyn_JoinEff_struct*_tmpF8F;struct Cyc_Absyn_JoinEff_struct
_tmpF8E;struct Cyc_Core_Opt*_tmpF8D;*_tmp7A2=((_tmpF8D=_cycalloc(sizeof(*_tmpF8D)),((
_tmpF8D->v=(void*)((void*)((_tmpF8F=_cycalloc(sizeof(*_tmpF8F)),((_tmpF8F[0]=((
_tmpF8E.tag=20,((_tmpF8E.f1=effect,_tmpF8E)))),_tmpF8F))))),_tmpF8D))));}}if(*
_tmp7A0 != 0){struct Cyc_List_List*bs=*_tmp7A0;for(0;bs != 0;bs=bs->tl){void*
_tmp8DE=Cyc_Absyn_compress_kb((void*)((struct Cyc_Absyn_Tvar*)bs->hd)->kind);
struct Cyc_Core_Opt*_tmp8DF;struct Cyc_Core_Opt**_tmp8E0;struct Cyc_Core_Opt*
_tmp8E1;struct Cyc_Core_Opt**_tmp8E2;void*_tmp8E3;struct Cyc_Core_Opt*_tmp8E4;
struct Cyc_Core_Opt**_tmp8E5;void*_tmp8E6;struct Cyc_Core_Opt*_tmp8E7;struct Cyc_Core_Opt**
_tmp8E8;void*_tmp8E9;struct Cyc_Core_Opt*_tmp8EA;struct Cyc_Core_Opt**_tmp8EB;void*
_tmp8EC;void*_tmp8ED;_LL590: if(*((int*)_tmp8DE)!= 1)goto _LL592;_tmp8DF=((struct
Cyc_Absyn_Unknown_kb_struct*)_tmp8DE)->f1;_tmp8E0=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Unknown_kb_struct*)_tmp8DE)->f1;_LL591:{const char*_tmpF93;void*
_tmpF92[1];struct Cyc_String_pa_struct _tmpF91;(_tmpF91.tag=0,((_tmpF91.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)bs->hd)->name),((
_tmpF92[0]=& _tmpF91,Cyc_Tcutil_warn(loc,((_tmpF93="Type variable %s unconstrained, assuming boxed",
_tag_dyneither(_tmpF93,sizeof(char),47))),_tag_dyneither(_tmpF92,sizeof(void*),1)))))));}
_tmp8E2=_tmp8E0;goto _LL593;_LL592: if(*((int*)_tmp8DE)!= 2)goto _LL594;_tmp8E1=((
struct Cyc_Absyn_Less_kb_struct*)_tmp8DE)->f1;_tmp8E2=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*)_tmp8DE)->f1;_tmp8E3=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp8DE)->f2;if((int)_tmp8E3 != 1)goto _LL594;_LL593: _tmp8E5=_tmp8E2;goto _LL595;
_LL594: if(*((int*)_tmp8DE)!= 2)goto _LL596;_tmp8E4=((struct Cyc_Absyn_Less_kb_struct*)
_tmp8DE)->f1;_tmp8E5=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp8DE)->f1;_tmp8E6=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp8DE)->f2;if((
int)_tmp8E6 != 0)goto _LL596;_LL595:*_tmp8E5=Cyc_Tcutil_kind_to_bound_opt((void*)2);
goto _LL58F;_LL596: if(*((int*)_tmp8DE)!= 2)goto _LL598;_tmp8E7=((struct Cyc_Absyn_Less_kb_struct*)
_tmp8DE)->f1;_tmp8E8=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp8DE)->f1;_tmp8E9=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp8DE)->f2;if((
int)_tmp8E9 != 5)goto _LL598;_LL597:*_tmp8E8=Cyc_Tcutil_kind_to_bound_opt((void*)3);
goto _LL58F;_LL598: if(*((int*)_tmp8DE)!= 2)goto _LL59A;_tmp8EA=((struct Cyc_Absyn_Less_kb_struct*)
_tmp8DE)->f1;_tmp8EB=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp8DE)->f1;_tmp8EC=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp8DE)->f2;
_LL599:*_tmp8EB=Cyc_Tcutil_kind_to_bound_opt(_tmp8EC);goto _LL58F;_LL59A: if(*((
int*)_tmp8DE)!= 0)goto _LL59C;_tmp8ED=(void*)((struct Cyc_Absyn_Eq_kb_struct*)
_tmp8DE)->f1;if((int)_tmp8ED != 1)goto _LL59C;_LL59B:{const char*_tmpF96;void*
_tmpF95;(_tmpF95=0,Cyc_Tcutil_terr(loc,((_tmpF96="functions can't abstract M types",
_tag_dyneither(_tmpF96,sizeof(char),33))),_tag_dyneither(_tmpF95,sizeof(void*),0)));}
goto _LL58F;_LL59C:;_LL59D: goto _LL58F;_LL58F:;}}cvtenv.kind_env=Cyc_Tcutil_remove_bound_tvars(
Cyc_Core_heap_region,_tmp874.kind_env,*_tmp7A0);_tmp874.free_vars=Cyc_Tcutil_remove_bound_tvars_bool(
_tmp874.r,_tmp874.free_vars,*_tmp7A0);{struct Cyc_List_List*tvs=_tmp874.free_vars;
for(0;tvs != 0;tvs=tvs->tl){struct Cyc_Absyn_Tvar*_tmp8F4;int _tmp8F5;struct
_tuple18 _tmp8F3=*((struct _tuple18*)tvs->hd);_tmp8F4=_tmp8F3.f1;_tmp8F5=_tmp8F3.f2;
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,
_tmp8F4,_tmp8F5);}}{struct Cyc_List_List*evs=_tmp874.free_evars;for(0;evs != 0;evs=
evs->tl){void*_tmp8F7;int _tmp8F8;struct _tuple19 _tmp8F6=*((struct _tuple19*)evs->hd);
_tmp8F7=_tmp8F6.f1;_tmp8F8=_tmp8F6.f2;cvtenv.free_evars=Cyc_Tcutil_add_free_evar(
cvtenv.r,cvtenv.free_evars,_tmp8F7,_tmp8F8);}}};_pop_region(newr);}goto _LL4CB;}
_LL4EA: if(*((int*)_tmp776)!= 9)goto _LL4EC;_tmp7A9=((struct Cyc_Absyn_TupleType_struct*)
_tmp776)->f1;_LL4EB: for(0;_tmp7A9 != 0;_tmp7A9=_tmp7A9->tl){struct _tuple5*_tmp8FA=(
struct _tuple5*)_tmp7A9->hd;cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(
void*)1,(*_tmp8FA).f2,1);((*_tmp8FA).f1).real_const=Cyc_Tcutil_extract_const_from_typedef(
loc,((*_tmp8FA).f1).print_const,(*_tmp8FA).f2);}goto _LL4CB;_LL4EC: if(*((int*)
_tmp776)!= 11)goto _LL4EE;_tmp7AA=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp776)->f1;_tmp7AB=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp776)->f2;_LL4ED:{
struct _RegionHandle _tmp8FB=_new_region("aprev_rgn");struct _RegionHandle*
aprev_rgn=& _tmp8FB;_push_region(aprev_rgn);{struct Cyc_List_List*prev_fields=0;
for(0;_tmp7AB != 0;_tmp7AB=_tmp7AB->tl){struct Cyc_Absyn_Aggrfield _tmp8FD;struct
_dyneither_ptr*_tmp8FE;struct Cyc_Absyn_Tqual _tmp8FF;struct Cyc_Absyn_Tqual*
_tmp900;void*_tmp901;struct Cyc_Absyn_Exp*_tmp902;struct Cyc_List_List*_tmp903;
struct Cyc_Absyn_Aggrfield*_tmp8FC=(struct Cyc_Absyn_Aggrfield*)_tmp7AB->hd;
_tmp8FD=*_tmp8FC;_tmp8FE=_tmp8FD.name;_tmp8FF=_tmp8FD.tq;_tmp900=(struct Cyc_Absyn_Tqual*)&(*
_tmp8FC).tq;_tmp901=(void*)_tmp8FD.type;_tmp902=_tmp8FD.width;_tmp903=_tmp8FD.attributes;
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*
l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,_tmp8FE)){
const char*_tmpF9A;void*_tmpF99[1];struct Cyc_String_pa_struct _tmpF98;(_tmpF98.tag=
0,((_tmpF98.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp8FE),((_tmpF99[
0]=& _tmpF98,Cyc_Tcutil_terr(loc,((_tmpF9A="duplicate field %s",_tag_dyneither(
_tmpF9A,sizeof(char),19))),_tag_dyneither(_tmpF99,sizeof(void*),1)))))));}{const
char*_tmpF9B;if(Cyc_strcmp((struct _dyneither_ptr)*_tmp8FE,((_tmpF9B="",
_tag_dyneither(_tmpF9B,sizeof(char),1))))!= 0){struct Cyc_List_List*_tmpF9C;
prev_fields=((_tmpF9C=_region_malloc(aprev_rgn,sizeof(*_tmpF9C)),((_tmpF9C->hd=
_tmp8FE,((_tmpF9C->tl=prev_fields,_tmpF9C))))));}}cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,(void*)1,_tmp901,1);_tmp900->real_const=Cyc_Tcutil_extract_const_from_typedef(
loc,_tmp900->print_const,_tmp901);if(_tmp7AA == (void*)1  && !Cyc_Tcutil_bits_only(
_tmp901)){const char*_tmpFA0;void*_tmpF9F[1];struct Cyc_String_pa_struct _tmpF9E;(
_tmpF9E.tag=0,((_tmpF9E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp8FE),((
_tmpF9F[0]=& _tmpF9E,Cyc_Tcutil_warn(loc,((_tmpFA0="union member %s is not `bits-only' so it can only be written and not read",
_tag_dyneither(_tmpFA0,sizeof(char),74))),_tag_dyneither(_tmpF9F,sizeof(void*),1)))))));}
Cyc_Tcutil_check_bitfield(loc,te,_tmp901,_tmp902,_tmp8FE);Cyc_Tcutil_check_field_atts(
loc,_tmp8FE,_tmp903);}};_pop_region(aprev_rgn);}goto _LL4CB;_LL4EE: if(*((int*)
_tmp776)!= 10)goto _LL4F0;_tmp7AC=((struct Cyc_Absyn_AggrType_struct*)_tmp776)->f1;
_tmp7AD=_tmp7AC.aggr_info;_tmp7AE=(union Cyc_Absyn_AggrInfoU_union*)&(((struct Cyc_Absyn_AggrType_struct*)
_tmp776)->f1).aggr_info;_tmp7AF=_tmp7AC.targs;_tmp7B0=(struct Cyc_List_List**)&(((
struct Cyc_Absyn_AggrType_struct*)_tmp776)->f1).targs;_LL4EF:{union Cyc_Absyn_AggrInfoU_union
_tmp90C=*_tmp7AE;void*_tmp90D;struct _tuple2*_tmp90E;struct Cyc_Absyn_Aggrdecl**
_tmp90F;struct Cyc_Absyn_Aggrdecl*_tmp910;_LL59F: if((_tmp90C.UnknownAggr).tag != 0)
goto _LL5A1;_tmp90D=(_tmp90C.UnknownAggr).f1;_tmp90E=(_tmp90C.UnknownAggr).f2;
_LL5A0: {struct Cyc_Absyn_Aggrdecl**adp;{struct _handler_cons _tmp911;_push_handler(&
_tmp911);{int _tmp913=0;if(setjmp(_tmp911.handler))_tmp913=1;if(!_tmp913){adp=Cyc_Tcenv_lookup_aggrdecl(
te,loc,_tmp90E);{union Cyc_Absyn_AggrInfoU_union _tmpFA1;*_tmp7AE=(union Cyc_Absyn_AggrInfoU_union)(((
_tmpFA1.KnownAggr).tag=1,(((_tmpFA1.KnownAggr).f1=adp,_tmpFA1))));};_pop_handler();}
else{void*_tmp912=(void*)_exn_thrown;void*_tmp916=_tmp912;_LL5A4: if(_tmp916 != 
Cyc_Dict_Absent)goto _LL5A6;_LL5A5: {struct Cyc_Tcenv_Genv*_tmp917=((struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct Cyc_Absyn_Aggrdecl*_tmpFA2;struct Cyc_Absyn_Aggrdecl*_tmp918=(_tmpFA2=
_cycalloc(sizeof(*_tmpFA2)),((_tmpFA2->kind=(void*)_tmp90D,((_tmpFA2->sc=(void*)((
void*)3),((_tmpFA2->name=_tmp90E,((_tmpFA2->tvs=0,((_tmpFA2->impl=0,((_tmpFA2->attributes=
0,_tmpFA2)))))))))))));Cyc_Tc_tcAggrdecl(te,_tmp917,loc,_tmp918);adp=Cyc_Tcenv_lookup_aggrdecl(
te,loc,_tmp90E);{union Cyc_Absyn_AggrInfoU_union _tmpFA3;*_tmp7AE=(union Cyc_Absyn_AggrInfoU_union)(((
_tmpFA3.KnownAggr).tag=1,(((_tmpFA3.KnownAggr).f1=adp,_tmpFA3))));}if(*_tmp7B0 != 
0){{const char*_tmpFA7;void*_tmpFA6[1];struct Cyc_String_pa_struct _tmpFA5;(_tmpFA5.tag=
0,((_tmpFA5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp90E)),((_tmpFA6[0]=& _tmpFA5,Cyc_Tcutil_terr(loc,((_tmpFA7="declare parameterized type %s before using",
_tag_dyneither(_tmpFA7,sizeof(char),43))),_tag_dyneither(_tmpFA6,sizeof(void*),1)))))));}
return cvtenv;}goto _LL5A3;}_LL5A6:;_LL5A7:(void)_throw(_tmp916);_LL5A3:;}}}
_tmp910=*adp;goto _LL5A2;}_LL5A1: if((_tmp90C.KnownAggr).tag != 1)goto _LL59E;
_tmp90F=(_tmp90C.KnownAggr).f1;_tmp910=*_tmp90F;_LL5A2: {struct Cyc_List_List*tvs=
_tmp910->tvs;struct Cyc_List_List*ts=*_tmp7B0;for(0;ts != 0  && tvs != 0;(ts=ts->tl,
tvs=tvs->tl)){void*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd);cvtenv=
Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,(void*)ts->hd,1);}if(ts != 0){const
char*_tmpFAB;void*_tmpFAA[1];struct Cyc_String_pa_struct _tmpFA9;(_tmpFA9.tag=0,((
_tmpFA9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp910->name)),((_tmpFAA[0]=& _tmpFA9,Cyc_Tcutil_terr(loc,((_tmpFAB="too many parameters for type %s",
_tag_dyneither(_tmpFAB,sizeof(char),32))),_tag_dyneither(_tmpFAA,sizeof(void*),1)))))));}
if(tvs != 0){struct Cyc_List_List*hidden_ts=0;for(0;tvs != 0;tvs=tvs->tl){void*k=
Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd);void*e=Cyc_Absyn_new_evar(0,
0);{struct Cyc_List_List*_tmpFAC;hidden_ts=((_tmpFAC=_cycalloc(sizeof(*_tmpFAC)),((
_tmpFAC->hd=(void*)e,((_tmpFAC->tl=hidden_ts,_tmpFAC))))));}cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,k,e,1);}*_tmp7B0=Cyc_List_imp_append(*_tmp7B0,Cyc_List_imp_rev(
hidden_ts));}}_LL59E:;}goto _LL4CB;_LL4F0: if(*((int*)_tmp776)!= 16)goto _LL4F2;
_tmp7B1=((struct Cyc_Absyn_TypedefType_struct*)_tmp776)->f1;_tmp7B2=((struct Cyc_Absyn_TypedefType_struct*)
_tmp776)->f2;_tmp7B3=(struct Cyc_List_List**)&((struct Cyc_Absyn_TypedefType_struct*)
_tmp776)->f2;_tmp7B4=((struct Cyc_Absyn_TypedefType_struct*)_tmp776)->f3;_tmp7B5=(
struct Cyc_Absyn_Typedefdecl**)&((struct Cyc_Absyn_TypedefType_struct*)_tmp776)->f3;
_tmp7B6=((struct Cyc_Absyn_TypedefType_struct*)_tmp776)->f4;_tmp7B7=(void***)&((
struct Cyc_Absyn_TypedefType_struct*)_tmp776)->f4;_LL4F1: {struct Cyc_List_List*
targs=*_tmp7B3;struct Cyc_Absyn_Typedefdecl*td;{struct _handler_cons _tmp922;
_push_handler(& _tmp922);{int _tmp924=0;if(setjmp(_tmp922.handler))_tmp924=1;if(!
_tmp924){td=Cyc_Tcenv_lookup_typedefdecl(te,loc,_tmp7B1);;_pop_handler();}else{
void*_tmp923=(void*)_exn_thrown;void*_tmp926=_tmp923;_LL5A9: if(_tmp926 != Cyc_Dict_Absent)
goto _LL5AB;_LL5AA:{const char*_tmpFB0;void*_tmpFAF[1];struct Cyc_String_pa_struct
_tmpFAE;(_tmpFAE.tag=0,((_tmpFAE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp7B1)),((_tmpFAF[0]=& _tmpFAE,Cyc_Tcutil_terr(loc,((
_tmpFB0="unbound typedef name %s",_tag_dyneither(_tmpFB0,sizeof(char),24))),
_tag_dyneither(_tmpFAF,sizeof(void*),1)))))));}return cvtenv;_LL5AB:;_LL5AC:(void)
_throw(_tmp926);_LL5A8:;}}}*_tmp7B5=(struct Cyc_Absyn_Typedefdecl*)td;_tmp7B1[0]=(
td->name)[_check_known_subscript_notnull(1,0)];{struct Cyc_List_List*tvs=td->tvs;
struct Cyc_List_List*ts=targs;{struct _RegionHandle _tmp92A=_new_region("temp");
struct _RegionHandle*temp=& _tmp92A;_push_region(temp);{struct Cyc_List_List*inst=0;
for(0;ts != 0  && tvs != 0;(ts=ts->tl,tvs=tvs->tl)){void*k=Cyc_Tcutil_tvar_kind((
struct Cyc_Absyn_Tvar*)tvs->hd);cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,
cvtenv,k,(void*)ts->hd,1);{struct _tuple8*_tmpFB3;struct Cyc_List_List*_tmpFB2;
inst=((_tmpFB2=_region_malloc(temp,sizeof(*_tmpFB2)),((_tmpFB2->hd=((_tmpFB3=
_region_malloc(temp,sizeof(*_tmpFB3)),((_tmpFB3->f1=(struct Cyc_Absyn_Tvar*)tvs->hd,((
_tmpFB3->f2=(void*)ts->hd,_tmpFB3)))))),((_tmpFB2->tl=inst,_tmpFB2))))));}}if(ts
!= 0){const char*_tmpFB7;void*_tmpFB6[1];struct Cyc_String_pa_struct _tmpFB5;(
_tmpFB5.tag=0,((_tmpFB5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp7B1)),((_tmpFB6[0]=& _tmpFB5,Cyc_Tcutil_terr(loc,((_tmpFB7="too many parameters for typedef %s",
_tag_dyneither(_tmpFB7,sizeof(char),35))),_tag_dyneither(_tmpFB6,sizeof(void*),1)))))));}
if(tvs != 0){struct Cyc_List_List*hidden_ts=0;for(0;tvs != 0;tvs=tvs->tl){void*k=
Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd);void*e=Cyc_Absyn_new_evar(0,
0);{struct Cyc_List_List*_tmpFB8;hidden_ts=((_tmpFB8=_cycalloc(sizeof(*_tmpFB8)),((
_tmpFB8->hd=(void*)e,((_tmpFB8->tl=hidden_ts,_tmpFB8))))));}cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,k,e,1);{struct _tuple8*_tmpFBB;struct Cyc_List_List*_tmpFBA;inst=(
struct Cyc_List_List*)((_tmpFBA=_cycalloc(sizeof(*_tmpFBA)),((_tmpFBA->hd=(struct
_tuple8*)((_tmpFBB=_cycalloc(sizeof(*_tmpFBB)),((_tmpFBB->f1=(struct Cyc_Absyn_Tvar*)
tvs->hd,((_tmpFBB->f2=e,_tmpFBB)))))),((_tmpFBA->tl=inst,_tmpFBA))))));}}*
_tmp7B3=Cyc_List_imp_append(targs,Cyc_List_imp_rev(hidden_ts));}if(td->defn != 0){
void*new_typ=Cyc_Tcutil_rsubstitute(temp,inst,(void*)((struct Cyc_Core_Opt*)
_check_null(td->defn))->v);void**_tmpFBC;*_tmp7B7=((_tmpFBC=_cycalloc(sizeof(*
_tmpFBC)),((_tmpFBC[0]=new_typ,_tmpFBC))));}};_pop_region(temp);}goto _LL4CB;}}
_LL4F2: if((int)_tmp776 != 3)goto _LL4F4;_LL4F3: goto _LL4F5;_LL4F4: if((int)_tmp776 != 
2)goto _LL4F6;_LL4F5: goto _LL4CB;_LL4F6: if(_tmp776 <= (void*)4)goto _LL4F8;if(*((int*)
_tmp776)!= 14)goto _LL4F8;_tmp7B8=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp776)->f1;_LL4F7: cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)5,
_tmp7B8,1);goto _LL4CB;_LL4F8: if(_tmp776 <= (void*)4)goto _LL4FA;if(*((int*)_tmp776)
!= 15)goto _LL4FA;_tmp7B9=(void*)((struct Cyc_Absyn_DynRgnType_struct*)_tmp776)->f1;
_tmp7BA=(void*)((struct Cyc_Absyn_DynRgnType_struct*)_tmp776)->f2;_LL4F9: cvtenv=
Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)3,_tmp7B9,0);cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,(void*)3,_tmp7BA,1);goto _LL4CB;_LL4FA: if(_tmp776 <= (void*)4)goto
_LL4FC;if(*((int*)_tmp776)!= 19)goto _LL4FC;_tmp7BB=(void*)((struct Cyc_Absyn_AccessEff_struct*)
_tmp776)->f1;_LL4FB: cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)5,
_tmp7BB,1);goto _LL4CB;_LL4FC: if(_tmp776 <= (void*)4)goto _LL4FE;if(*((int*)_tmp776)
!= 21)goto _LL4FE;_tmp7BC=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp776)->f1;
_LL4FD: cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)0,_tmp7BC,1);
goto _LL4CB;_LL4FE: if(_tmp776 <= (void*)4)goto _LL4CB;if(*((int*)_tmp776)!= 20)goto
_LL4CB;_tmp7BD=((struct Cyc_Absyn_JoinEff_struct*)_tmp776)->f1;_LL4FF: for(0;
_tmp7BD != 0;_tmp7BD=_tmp7BD->tl){cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,
cvtenv,(void*)6,(void*)_tmp7BD->hd,1);}goto _LL4CB;_LL4CB:;}if(!Cyc_Tcutil_kind_leq(
Cyc_Tcutil_typ_kind(t),expected_kind)){{void*_tmp934=t;struct Cyc_Core_Opt*
_tmp935;struct Cyc_Core_Opt*_tmp936;_LL5AE: if(_tmp934 <= (void*)4)goto _LL5B0;if(*((
int*)_tmp934)!= 0)goto _LL5B0;_tmp935=((struct Cyc_Absyn_Evar_struct*)_tmp934)->f1;
_tmp936=((struct Cyc_Absyn_Evar_struct*)_tmp934)->f2;_LL5AF: {struct
_dyneither_ptr s;{struct Cyc_Core_Opt*_tmp937=_tmp935;struct Cyc_Core_Opt _tmp938;
void*_tmp939;_LL5B3: if(_tmp937 != 0)goto _LL5B5;_LL5B4:{const char*_tmpFBD;s=((
_tmpFBD="kind=NULL ",_tag_dyneither(_tmpFBD,sizeof(char),11)));}goto _LL5B2;
_LL5B5: if(_tmp937 == 0)goto _LL5B2;_tmp938=*_tmp937;_tmp939=(void*)_tmp938.v;
_LL5B6:{const char*_tmpFC1;void*_tmpFC0[1];struct Cyc_String_pa_struct _tmpFBF;s=(
struct _dyneither_ptr)((_tmpFBF.tag=0,((_tmpFBF.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_kind2string(_tmp939)),((_tmpFC0[0]=& _tmpFBF,Cyc_aprintf(((
_tmpFC1="kind=%s ",_tag_dyneither(_tmpFC1,sizeof(char),9))),_tag_dyneither(
_tmpFC0,sizeof(void*),1))))))));}goto _LL5B2;_LL5B2:;}{struct Cyc_Core_Opt*_tmp93E=
_tmp936;struct Cyc_Core_Opt _tmp93F;void*_tmp940;_LL5B8: if(_tmp93E != 0)goto _LL5BA;
_LL5B9:{const char*_tmpFC5;void*_tmpFC4[1];struct Cyc_String_pa_struct _tmpFC3;s=(
struct _dyneither_ptr)((_tmpFC3.tag=0,((_tmpFC3.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)s),((_tmpFC4[0]=& _tmpFC3,Cyc_aprintf(((_tmpFC5="%s ref=NULL",
_tag_dyneither(_tmpFC5,sizeof(char),12))),_tag_dyneither(_tmpFC4,sizeof(void*),1))))))));}
goto _LL5B7;_LL5BA: if(_tmp93E == 0)goto _LL5B7;_tmp93F=*_tmp93E;_tmp940=(void*)
_tmp93F.v;_LL5BB:{const char*_tmpFCA;void*_tmpFC9[2];struct Cyc_String_pa_struct
_tmpFC8;struct Cyc_String_pa_struct _tmpFC7;s=(struct _dyneither_ptr)((_tmpFC7.tag=
0,((_tmpFC7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
_tmp940)),((_tmpFC8.tag=0,((_tmpFC8.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)s),((_tmpFC9[0]=& _tmpFC8,((_tmpFC9[1]=& _tmpFC7,Cyc_aprintf(((
_tmpFCA="%s ref=%s",_tag_dyneither(_tmpFCA,sizeof(char),10))),_tag_dyneither(
_tmpFC9,sizeof(void*),2))))))))))))));}goto _LL5B7;_LL5B7:;}{const char*_tmpFCE;
void*_tmpFCD[1];struct Cyc_String_pa_struct _tmpFCC;(_tmpFCC.tag=0,((_tmpFCC.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmpFCD[0]=& _tmpFCC,Cyc_fprintf(
Cyc_stderr,((_tmpFCE="evar info: %s\n",_tag_dyneither(_tmpFCE,sizeof(char),15))),
_tag_dyneither(_tmpFCD,sizeof(void*),1)))))));}goto _LL5AD;}_LL5B0:;_LL5B1: goto
_LL5AD;_LL5AD:;}{const char*_tmpFD4;void*_tmpFD3[3];struct Cyc_String_pa_struct
_tmpFD2;struct Cyc_String_pa_struct _tmpFD1;struct Cyc_String_pa_struct _tmpFD0;(
_tmpFD0.tag=0,((_tmpFD0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(
expected_kind)),((_tmpFD1.tag=0,((_tmpFD1.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_kind2string(Cyc_Tcutil_typ_kind(t))),((_tmpFD2.tag=0,((
_tmpFD2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((
_tmpFD3[0]=& _tmpFD2,((_tmpFD3[1]=& _tmpFD1,((_tmpFD3[2]=& _tmpFD0,Cyc_Tcutil_terr(
loc,((_tmpFD4="type %s has kind %s but as used here needs kind %s",
_tag_dyneither(_tmpFD4,sizeof(char),51))),_tag_dyneither(_tmpFD3,sizeof(void*),3)))))))))))))))))));}}
return cvtenv;}static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type_level_exp(
struct Cyc_Absyn_Exp*e,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv);
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type_level_exp(struct Cyc_Absyn_Exp*
e,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv){{void*_tmp950=(void*)e->r;
struct Cyc_List_List*_tmp951;struct Cyc_Absyn_Exp*_tmp952;struct Cyc_Absyn_Exp*
_tmp953;struct Cyc_Absyn_Exp*_tmp954;struct Cyc_Absyn_Exp*_tmp955;struct Cyc_Absyn_Exp*
_tmp956;struct Cyc_Absyn_Exp*_tmp957;struct Cyc_Absyn_Exp*_tmp958;struct Cyc_Absyn_Exp*
_tmp959;struct Cyc_Absyn_Exp*_tmp95A;void*_tmp95B;struct Cyc_Absyn_Exp*_tmp95C;
void*_tmp95D;void*_tmp95E;void*_tmp95F;struct Cyc_Absyn_Exp*_tmp960;_LL5BD: if(*((
int*)_tmp950)!= 0)goto _LL5BF;_LL5BE: goto _LL5C0;_LL5BF: if(*((int*)_tmp950)!= 33)
goto _LL5C1;_LL5C0: goto _LL5C2;_LL5C1: if(*((int*)_tmp950)!= 34)goto _LL5C3;_LL5C2:
goto _LL5C4;_LL5C3: if(*((int*)_tmp950)!= 1)goto _LL5C5;_LL5C4: goto _LL5BC;_LL5C5:
if(*((int*)_tmp950)!= 3)goto _LL5C7;_tmp951=((struct Cyc_Absyn_Primop_e_struct*)
_tmp950)->f2;_LL5C6: for(0;_tmp951 != 0;_tmp951=_tmp951->tl){cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp((
struct Cyc_Absyn_Exp*)_tmp951->hd,te,cvtenv);}goto _LL5BC;_LL5C7: if(*((int*)
_tmp950)!= 6)goto _LL5C9;_tmp952=((struct Cyc_Absyn_Conditional_e_struct*)_tmp950)->f1;
_tmp953=((struct Cyc_Absyn_Conditional_e_struct*)_tmp950)->f2;_tmp954=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp950)->f3;_LL5C8: cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp952,te,
cvtenv);cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp953,te,cvtenv);cvtenv=
Cyc_Tcutil_i_check_valid_type_level_exp(_tmp954,te,cvtenv);goto _LL5BC;_LL5C9: if(*((
int*)_tmp950)!= 7)goto _LL5CB;_tmp955=((struct Cyc_Absyn_And_e_struct*)_tmp950)->f1;
_tmp956=((struct Cyc_Absyn_And_e_struct*)_tmp950)->f2;_LL5CA: _tmp957=_tmp955;
_tmp958=_tmp956;goto _LL5CC;_LL5CB: if(*((int*)_tmp950)!= 8)goto _LL5CD;_tmp957=((
struct Cyc_Absyn_Or_e_struct*)_tmp950)->f1;_tmp958=((struct Cyc_Absyn_Or_e_struct*)
_tmp950)->f2;_LL5CC: _tmp959=_tmp957;_tmp95A=_tmp958;goto _LL5CE;_LL5CD: if(*((int*)
_tmp950)!= 9)goto _LL5CF;_tmp959=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp950)->f1;
_tmp95A=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp950)->f2;_LL5CE: cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(
_tmp959,te,cvtenv);cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp95A,te,
cvtenv);goto _LL5BC;_LL5CF: if(*((int*)_tmp950)!= 15)goto _LL5D1;_tmp95B=(void*)((
struct Cyc_Absyn_Cast_e_struct*)_tmp950)->f1;_tmp95C=((struct Cyc_Absyn_Cast_e_struct*)
_tmp950)->f2;_LL5D0: cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp95C,te,
cvtenv);cvtenv=Cyc_Tcutil_i_check_valid_type(e->loc,te,cvtenv,(void*)0,_tmp95B,1);
goto _LL5BC;_LL5D1: if(*((int*)_tmp950)!= 20)goto _LL5D3;_tmp95D=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)
_tmp950)->f1;_LL5D2: _tmp95E=_tmp95D;goto _LL5D4;_LL5D3: if(*((int*)_tmp950)!= 18)
goto _LL5D5;_tmp95E=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp950)->f1;
_LL5D4: cvtenv=Cyc_Tcutil_i_check_valid_type(e->loc,te,cvtenv,(void*)0,_tmp95E,1);
goto _LL5BC;_LL5D5: if(*((int*)_tmp950)!= 39)goto _LL5D7;_tmp95F=(void*)((struct Cyc_Absyn_Valueof_e_struct*)
_tmp950)->f1;_LL5D6: cvtenv=Cyc_Tcutil_i_check_valid_type(e->loc,te,cvtenv,(void*)
7,_tmp95F,1);goto _LL5BC;_LL5D7: if(*((int*)_tmp950)!= 19)goto _LL5D9;_tmp960=((
struct Cyc_Absyn_Sizeofexp_e_struct*)_tmp950)->f1;_LL5D8: cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(
_tmp960,te,cvtenv);goto _LL5BC;_LL5D9:;_LL5DA: {const char*_tmpFD7;void*_tmpFD6;(
_tmpFD6=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmpFD7="non-type-level-expression in Tcutil::i_check_valid_type_level_exp",
_tag_dyneither(_tmpFD7,sizeof(char),66))),_tag_dyneither(_tmpFD6,sizeof(void*),0)));}
_LL5BC:;}return cvtenv;}static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_check_valid_type(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv
cvt,void*expected_kind,void*t);static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_check_valid_type(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv
cvt,void*expected_kind,void*t){struct Cyc_List_List*_tmp963=cvt.kind_env;cvt=Cyc_Tcutil_i_check_valid_type(
loc,te,cvt,expected_kind,t,1);{struct Cyc_List_List*vs=cvt.free_vars;for(0;vs != 0;
vs=vs->tl){struct Cyc_Absyn_Tvar*_tmp965;struct _tuple18 _tmp964=*((struct _tuple18*)
vs->hd);_tmp965=_tmp964.f1;cvt.kind_env=Cyc_Tcutil_fast_add_free_tvar(_tmp963,
_tmp965);}}{struct Cyc_List_List*evs=cvt.free_evars;for(0;evs != 0;evs=evs->tl){
void*_tmp967;struct _tuple19 _tmp966=*((struct _tuple19*)evs->hd);_tmp967=_tmp966.f1;{
void*_tmp968=Cyc_Tcutil_compress(_tmp967);struct Cyc_Core_Opt*_tmp969;struct Cyc_Core_Opt**
_tmp96A;_LL5DC: if(_tmp968 <= (void*)4)goto _LL5DE;if(*((int*)_tmp968)!= 0)goto
_LL5DE;_tmp969=((struct Cyc_Absyn_Evar_struct*)_tmp968)->f4;_tmp96A=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Evar_struct*)_tmp968)->f4;_LL5DD: if(*_tmp96A == 0){struct Cyc_Core_Opt*
_tmpFD8;*_tmp96A=((_tmpFD8=_cycalloc(sizeof(*_tmpFD8)),((_tmpFD8->v=_tmp963,
_tmpFD8))));}else{struct Cyc_List_List*_tmp96C=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(*_tmp96A))->v;struct Cyc_List_List*_tmp96D=0;for(0;_tmp96C != 0;
_tmp96C=_tmp96C->tl){if(((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,
_tmp963,(struct Cyc_Absyn_Tvar*)_tmp96C->hd)){struct Cyc_List_List*_tmpFD9;_tmp96D=((
_tmpFD9=_cycalloc(sizeof(*_tmpFD9)),((_tmpFD9->hd=(struct Cyc_Absyn_Tvar*)_tmp96C->hd,((
_tmpFD9->tl=_tmp96D,_tmpFD9))))));}}{struct Cyc_Core_Opt*_tmpFDA;*_tmp96A=((
_tmpFDA=_cycalloc(sizeof(*_tmpFDA)),((_tmpFDA->v=_tmp96D,_tmpFDA))));}}goto
_LL5DB;_LL5DE:;_LL5DF: goto _LL5DB;_LL5DB:;}}}return cvt;}void Cyc_Tcutil_check_valid_toplevel_type(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,void*t);void Cyc_Tcutil_check_valid_toplevel_type(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,void*t){int
generalize_evars=Cyc_Tcutil_is_function_type(t);struct Cyc_List_List*_tmp970=Cyc_Tcenv_lookup_type_vars(
te);struct _RegionHandle _tmp971=_new_region("temp");struct _RegionHandle*temp=&
_tmp971;_push_region(temp);{struct Cyc_Tcutil_CVTEnv _tmpFDB;struct Cyc_Tcutil_CVTEnv
_tmp972=Cyc_Tcutil_check_valid_type(loc,te,((_tmpFDB.r=temp,((_tmpFDB.kind_env=
_tmp970,((_tmpFDB.free_vars=0,((_tmpFDB.free_evars=0,((_tmpFDB.generalize_evars=
generalize_evars,((_tmpFDB.fn_result=0,_tmpFDB)))))))))))),(void*)1,t);struct Cyc_List_List*
_tmp973=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Tvar*(*f)(
struct _tuple18*),struct Cyc_List_List*x))Cyc_List_rmap)(temp,(struct Cyc_Absyn_Tvar*(*)(
struct _tuple18*))Cyc_Core_fst,_tmp972.free_vars);struct Cyc_List_List*_tmp974=((
struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _tuple19*),struct
Cyc_List_List*x))Cyc_List_rmap)(temp,(void*(*)(struct _tuple19*))Cyc_Core_fst,
_tmp972.free_evars);if(_tmp970 != 0){struct Cyc_List_List*_tmp975=0;{struct Cyc_List_List*
_tmp976=_tmp973;for(0;(unsigned int)_tmp976;_tmp976=_tmp976->tl){struct Cyc_Absyn_Tvar*
_tmp977=(struct Cyc_Absyn_Tvar*)_tmp976->hd;int found=0;{struct Cyc_List_List*
_tmp978=_tmp970;for(0;(unsigned int)_tmp978;_tmp978=_tmp978->tl){if(Cyc_Absyn_tvar_cmp(
_tmp977,(struct Cyc_Absyn_Tvar*)_tmp978->hd)== 0){found=1;break;}}}if(!found){
struct Cyc_List_List*_tmpFDC;_tmp975=((_tmpFDC=_region_malloc(temp,sizeof(*
_tmpFDC)),((_tmpFDC->hd=(struct Cyc_Absyn_Tvar*)_tmp976->hd,((_tmpFDC->tl=_tmp975,
_tmpFDC))))));}}}_tmp973=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmp975);}{struct Cyc_List_List*x=_tmp973;for(0;x != 0;x=x->tl){void*_tmp97A=Cyc_Absyn_compress_kb((
void*)((struct Cyc_Absyn_Tvar*)x->hd)->kind);struct Cyc_Core_Opt*_tmp97B;struct Cyc_Core_Opt**
_tmp97C;struct Cyc_Core_Opt*_tmp97D;struct Cyc_Core_Opt**_tmp97E;void*_tmp97F;
struct Cyc_Core_Opt*_tmp980;struct Cyc_Core_Opt**_tmp981;void*_tmp982;struct Cyc_Core_Opt*
_tmp983;struct Cyc_Core_Opt**_tmp984;void*_tmp985;void*_tmp986;_LL5E1: if(*((int*)
_tmp97A)!= 1)goto _LL5E3;_tmp97B=((struct Cyc_Absyn_Unknown_kb_struct*)_tmp97A)->f1;
_tmp97C=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)_tmp97A)->f1;
_LL5E2: _tmp97E=_tmp97C;goto _LL5E4;_LL5E3: if(*((int*)_tmp97A)!= 2)goto _LL5E5;
_tmp97D=((struct Cyc_Absyn_Less_kb_struct*)_tmp97A)->f1;_tmp97E=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*)_tmp97A)->f1;_tmp97F=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp97A)->f2;if((int)_tmp97F != 1)goto _LL5E5;_LL5E4:*_tmp97E=Cyc_Tcutil_kind_to_bound_opt((
void*)2);goto _LL5E0;_LL5E5: if(*((int*)_tmp97A)!= 2)goto _LL5E7;_tmp980=((struct
Cyc_Absyn_Less_kb_struct*)_tmp97A)->f1;_tmp981=(struct Cyc_Core_Opt**)&((struct
Cyc_Absyn_Less_kb_struct*)_tmp97A)->f1;_tmp982=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp97A)->f2;if((int)_tmp982 != 5)goto _LL5E7;_LL5E6:*_tmp981=Cyc_Tcutil_kind_to_bound_opt((
void*)3);goto _LL5E0;_LL5E7: if(*((int*)_tmp97A)!= 2)goto _LL5E9;_tmp983=((struct
Cyc_Absyn_Less_kb_struct*)_tmp97A)->f1;_tmp984=(struct Cyc_Core_Opt**)&((struct
Cyc_Absyn_Less_kb_struct*)_tmp97A)->f1;_tmp985=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp97A)->f2;_LL5E8:*_tmp984=Cyc_Tcutil_kind_to_bound_opt(_tmp985);goto _LL5E0;
_LL5E9: if(*((int*)_tmp97A)!= 0)goto _LL5EB;_tmp986=(void*)((struct Cyc_Absyn_Eq_kb_struct*)
_tmp97A)->f1;if((int)_tmp986 != 1)goto _LL5EB;_LL5EA:{const char*_tmpFE0;void*
_tmpFDF[1];struct Cyc_String_pa_struct _tmpFDE;(_tmpFDE.tag=0,((_tmpFDE.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_tvar2string((struct Cyc_Absyn_Tvar*)
x->hd)),((_tmpFDF[0]=& _tmpFDE,Cyc_Tcutil_terr(loc,((_tmpFE0="type variable %s cannot have kind M",
_tag_dyneither(_tmpFE0,sizeof(char),36))),_tag_dyneither(_tmpFDF,sizeof(void*),1)))))));}
goto _LL5E0;_LL5EB:;_LL5EC: goto _LL5E0;_LL5E0:;}}if(_tmp973 != 0  || _tmp974 != 0){{
void*_tmp98A=Cyc_Tcutil_compress(t);struct Cyc_Absyn_FnInfo _tmp98B;struct Cyc_List_List*
_tmp98C;struct Cyc_List_List**_tmp98D;struct Cyc_Core_Opt*_tmp98E;void*_tmp98F;
struct Cyc_List_List*_tmp990;int _tmp991;struct Cyc_Absyn_VarargInfo*_tmp992;struct
Cyc_List_List*_tmp993;struct Cyc_List_List*_tmp994;_LL5EE: if(_tmp98A <= (void*)4)
goto _LL5F0;if(*((int*)_tmp98A)!= 8)goto _LL5F0;_tmp98B=((struct Cyc_Absyn_FnType_struct*)
_tmp98A)->f1;_tmp98C=_tmp98B.tvars;_tmp98D=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_struct*)
_tmp98A)->f1).tvars;_tmp98E=_tmp98B.effect;_tmp98F=(void*)_tmp98B.ret_typ;
_tmp990=_tmp98B.args;_tmp991=_tmp98B.c_varargs;_tmp992=_tmp98B.cyc_varargs;
_tmp993=_tmp98B.rgn_po;_tmp994=_tmp98B.attributes;_LL5EF: if(*_tmp98D == 0){*
_tmp98D=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_copy)(_tmp973);
_tmp973=0;}goto _LL5ED;_LL5F0:;_LL5F1: goto _LL5ED;_LL5ED:;}if(_tmp973 != 0){const
char*_tmpFE4;void*_tmpFE3[1];struct Cyc_String_pa_struct _tmpFE2;(_tmpFE2.tag=0,((
_tmpFE2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)
_tmp973->hd)->name),((_tmpFE3[0]=& _tmpFE2,Cyc_Tcutil_terr(loc,((_tmpFE4="unbound type variable %s",
_tag_dyneither(_tmpFE4,sizeof(char),25))),_tag_dyneither(_tmpFE3,sizeof(void*),1)))))));}
if(_tmp974 != 0)for(0;_tmp974 != 0;_tmp974=_tmp974->tl){void*e=(void*)_tmp974->hd;
void*_tmp998=Cyc_Tcutil_typ_kind(e);_LL5F3: if((int)_tmp998 != 4)goto _LL5F5;_LL5F4:
if(!Cyc_Tcutil_unify(e,(void*)3)){const char*_tmpFE7;void*_tmpFE6;(_tmpFE6=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmpFE7="can't unify evar with unique region!",_tag_dyneither(_tmpFE7,sizeof(
char),37))),_tag_dyneither(_tmpFE6,sizeof(void*),0)));}goto _LL5F2;_LL5F5: if((int)
_tmp998 != 5)goto _LL5F7;_LL5F6: goto _LL5F8;_LL5F7: if((int)_tmp998 != 3)goto _LL5F9;
_LL5F8: if(!Cyc_Tcutil_unify(e,(void*)2)){const char*_tmpFEA;void*_tmpFE9;(_tmpFE9=
0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmpFEA="can't unify evar with heap!",_tag_dyneither(_tmpFEA,sizeof(char),28))),
_tag_dyneither(_tmpFE9,sizeof(void*),0)));}goto _LL5F2;_LL5F9: if((int)_tmp998 != 6)
goto _LL5FB;_LL5FA: if(!Cyc_Tcutil_unify(e,Cyc_Absyn_empty_effect)){const char*
_tmpFED;void*_tmpFEC;(_tmpFEC=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpFED="can't unify evar with {}!",
_tag_dyneither(_tmpFED,sizeof(char),26))),_tag_dyneither(_tmpFEC,sizeof(void*),0)));}
goto _LL5F2;_LL5FB:;_LL5FC:{const char*_tmpFF2;void*_tmpFF1[2];struct Cyc_String_pa_struct
_tmpFF0;struct Cyc_String_pa_struct _tmpFEF;(_tmpFEF.tag=0,((_tmpFEF.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpFF0.tag=0,((
_tmpFF0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(e)),((
_tmpFF1[0]=& _tmpFF0,((_tmpFF1[1]=& _tmpFEF,Cyc_Tcutil_terr(loc,((_tmpFF2="hidden type variable %s isn't abstracted in type %s",
_tag_dyneither(_tmpFF2,sizeof(char),52))),_tag_dyneither(_tmpFF1,sizeof(void*),2)))))))))))));}
goto _LL5F2;_LL5F2:;}}};_pop_region(temp);}void Cyc_Tcutil_check_fndecl_valid_type(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Fndecl*fd);
void Cyc_Tcutil_check_fndecl_valid_type(struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Fndecl*fd){void*t=Cyc_Tcutil_fndecl2typ(fd);Cyc_Tcutil_check_valid_toplevel_type(
loc,te,t);{void*_tmp9A4=Cyc_Tcutil_compress(t);struct Cyc_Absyn_FnInfo _tmp9A5;
struct Cyc_List_List*_tmp9A6;struct Cyc_Core_Opt*_tmp9A7;void*_tmp9A8;_LL5FE: if(
_tmp9A4 <= (void*)4)goto _LL600;if(*((int*)_tmp9A4)!= 8)goto _LL600;_tmp9A5=((
struct Cyc_Absyn_FnType_struct*)_tmp9A4)->f1;_tmp9A6=_tmp9A5.tvars;_tmp9A7=
_tmp9A5.effect;_tmp9A8=(void*)_tmp9A5.ret_typ;_LL5FF: fd->tvs=_tmp9A6;fd->effect=
_tmp9A7;goto _LL5FD;_LL600:;_LL601: {const char*_tmpFF5;void*_tmpFF4;(_tmpFF4=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmpFF5="check_fndecl_valid_type: not a FnType",_tag_dyneither(_tmpFF5,sizeof(
char),38))),_tag_dyneither(_tmpFF4,sizeof(void*),0)));}_LL5FD:;}{struct
_RegionHandle _tmp9AB=_new_region("r");struct _RegionHandle*r=& _tmp9AB;
_push_region(r);{const char*_tmpFF6;Cyc_Tcutil_check_unique_vars(((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _dyneither_ptr*(*f)(struct _tuple14*),struct Cyc_List_List*
x))Cyc_List_rmap)(r,(struct _dyneither_ptr*(*)(struct _tuple14*t))Cyc_Tcutil_fst_fdarg,
fd->args),loc,((_tmpFF6="function declaration has repeated parameter",
_tag_dyneither(_tmpFF6,sizeof(char),44))));};_pop_region(r);}{struct Cyc_Core_Opt*
_tmpFF7;fd->cached_typ=((_tmpFF7=_cycalloc(sizeof(*_tmpFF7)),((_tmpFF7->v=(void*)
t,_tmpFF7))));}}void Cyc_Tcutil_check_type(struct Cyc_Position_Segment*loc,struct
Cyc_Tcenv_Tenv*te,struct Cyc_List_List*bound_tvars,void*expected_kind,int
allow_evars,void*t);void Cyc_Tcutil_check_type(struct Cyc_Position_Segment*loc,
struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*bound_tvars,void*expected_kind,int
allow_evars,void*t){struct _RegionHandle _tmp9AE=_new_region("r");struct
_RegionHandle*r=& _tmp9AE;_push_region(r);{struct Cyc_Tcutil_CVTEnv _tmpFF8;struct
Cyc_Tcutil_CVTEnv _tmp9AF=Cyc_Tcutil_check_valid_type(loc,te,((_tmpFF8.r=r,((
_tmpFF8.kind_env=bound_tvars,((_tmpFF8.free_vars=0,((_tmpFF8.free_evars=0,((
_tmpFF8.generalize_evars=0,((_tmpFF8.fn_result=0,_tmpFF8)))))))))))),
expected_kind,t);struct Cyc_List_List*_tmp9B0=Cyc_Tcutil_remove_bound_tvars(r,((
struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Tvar*(*f)(struct
_tuple18*),struct Cyc_List_List*x))Cyc_List_rmap)(r,(struct Cyc_Absyn_Tvar*(*)(
struct _tuple18*))Cyc_Core_fst,_tmp9AF.free_vars),bound_tvars);struct Cyc_List_List*
_tmp9B1=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _tuple19*),
struct Cyc_List_List*x))Cyc_List_rmap)(r,(void*(*)(struct _tuple19*))Cyc_Core_fst,
_tmp9AF.free_evars);{struct Cyc_List_List*fs=_tmp9B0;for(0;fs != 0;fs=fs->tl){
struct _dyneither_ptr*_tmp9B2=((struct Cyc_Absyn_Tvar*)fs->hd)->name;const char*
_tmpFFD;void*_tmpFFC[2];struct Cyc_String_pa_struct _tmpFFB;struct Cyc_String_pa_struct
_tmpFFA;(_tmpFFA.tag=0,((_tmpFFA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmpFFB.tag=0,((_tmpFFB.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)*_tmp9B2),((_tmpFFC[0]=& _tmpFFB,((_tmpFFC[1]=& _tmpFFA,Cyc_Tcutil_terr(
loc,((_tmpFFD="unbound type variable %s in type %s",_tag_dyneither(_tmpFFD,
sizeof(char),36))),_tag_dyneither(_tmpFFC,sizeof(void*),2)))))))))))));}}if(!
allow_evars  && _tmp9B1 != 0)for(0;_tmp9B1 != 0;_tmp9B1=_tmp9B1->tl){void*e=(void*)
_tmp9B1->hd;void*_tmp9B7=Cyc_Tcutil_typ_kind(e);_LL603: if((int)_tmp9B7 != 4)goto
_LL605;_LL604: if(!Cyc_Tcutil_unify(e,(void*)3)){const char*_tmp1000;void*_tmpFFF;(
_tmpFFF=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp1000="can't unify evar with unique region!",_tag_dyneither(_tmp1000,sizeof(
char),37))),_tag_dyneither(_tmpFFF,sizeof(void*),0)));}goto _LL602;_LL605: if((int)
_tmp9B7 != 5)goto _LL607;_LL606: goto _LL608;_LL607: if((int)_tmp9B7 != 3)goto _LL609;
_LL608: if(!Cyc_Tcutil_unify(e,(void*)2)){const char*_tmp1003;void*_tmp1002;(
_tmp1002=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp1003="can't unify evar with heap!",_tag_dyneither(_tmp1003,sizeof(char),28))),
_tag_dyneither(_tmp1002,sizeof(void*),0)));}goto _LL602;_LL609: if((int)_tmp9B7 != 
6)goto _LL60B;_LL60A: if(!Cyc_Tcutil_unify(e,Cyc_Absyn_empty_effect)){const char*
_tmp1006;void*_tmp1005;(_tmp1005=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1006="can't unify evar with {}!",
_tag_dyneither(_tmp1006,sizeof(char),26))),_tag_dyneither(_tmp1005,sizeof(void*),
0)));}goto _LL602;_LL60B:;_LL60C:{const char*_tmp100B;void*_tmp100A[2];struct Cyc_String_pa_struct
_tmp1009;struct Cyc_String_pa_struct _tmp1008;(_tmp1008.tag=0,((_tmp1008.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((
_tmp1009.tag=0,((_tmp1009.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
e)),((_tmp100A[0]=& _tmp1009,((_tmp100A[1]=& _tmp1008,Cyc_Tcutil_terr(loc,((
_tmp100B="hidden type variable %s isn't abstracted in type %s",_tag_dyneither(
_tmp100B,sizeof(char),52))),_tag_dyneither(_tmp100A,sizeof(void*),2)))))))))))));}
goto _LL602;_LL602:;}};_pop_region(r);}void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*
tv);void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*tv){if(tv->identity == 
- 1)tv->identity=Cyc_Tcutil_new_tvar_id();}void Cyc_Tcutil_add_tvar_identities(
struct Cyc_List_List*tvs);void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*
tvs){((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_iter)(
Cyc_Tcutil_add_tvar_identity,tvs);}static void Cyc_Tcutil_check_unique_unsorted(
int(*cmp)(void*,void*),struct Cyc_List_List*vs,struct Cyc_Position_Segment*loc,
struct _dyneither_ptr(*a2string)(void*),struct _dyneither_ptr msg);static void Cyc_Tcutil_check_unique_unsorted(
int(*cmp)(void*,void*),struct Cyc_List_List*vs,struct Cyc_Position_Segment*loc,
struct _dyneither_ptr(*a2string)(void*),struct _dyneither_ptr msg){for(0;vs != 0;vs=
vs->tl){struct Cyc_List_List*vs2=vs->tl;for(0;vs2 != 0;vs2=vs2->tl){if(cmp((void*)
vs->hd,(void*)vs2->hd)== 0){const char*_tmp1010;void*_tmp100F[2];struct Cyc_String_pa_struct
_tmp100E;struct Cyc_String_pa_struct _tmp100D;(_tmp100D.tag=0,((_tmp100D.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)a2string((void*)vs->hd)),((_tmp100E.tag=
0,((_tmp100E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg),((_tmp100F[0]=&
_tmp100E,((_tmp100F[1]=& _tmp100D,Cyc_Tcutil_terr(loc,((_tmp1010="%s: %s",
_tag_dyneither(_tmp1010,sizeof(char),7))),_tag_dyneither(_tmp100F,sizeof(void*),
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
struct Cyc_Position_Segment*loc,struct Cyc_List_List*tvs){const char*_tmp1011;((
void(*)(int(*cmp)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*
vs,struct Cyc_Position_Segment*loc,struct _dyneither_ptr(*a2string)(struct Cyc_Absyn_Tvar*),
struct _dyneither_ptr msg))Cyc_Tcutil_check_unique_unsorted)(Cyc_Absyn_tvar_cmp,
tvs,loc,Cyc_Tcutil_tvar2string,((_tmp1011="duplicate type variable",
_tag_dyneither(_tmp1011,sizeof(char),24))));}struct _tuple20{struct Cyc_Absyn_Aggrfield*
f1;int f2;};struct _tuple21{struct Cyc_List_List*f1;void*f2;};struct _tuple22{struct
Cyc_Absyn_Aggrfield*f1;void*f2;};struct Cyc_List_List*Cyc_Tcutil_resolve_struct_designators(
struct _RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,
struct Cyc_List_List*sdfields);struct Cyc_List_List*Cyc_Tcutil_resolve_struct_designators(
struct _RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,
struct Cyc_List_List*sdfields){struct Cyc_List_List*fields=0;{struct Cyc_List_List*
sd_fields=sdfields;for(0;sd_fields != 0;sd_fields=sd_fields->tl){const char*
_tmp1012;if(Cyc_strcmp((struct _dyneither_ptr)*((struct Cyc_Absyn_Aggrfield*)
sd_fields->hd)->name,((_tmp1012="",_tag_dyneither(_tmp1012,sizeof(char),1))))!= 
0){struct _tuple20*_tmp1015;struct Cyc_List_List*_tmp1014;fields=((_tmp1014=
_cycalloc(sizeof(*_tmp1014)),((_tmp1014->hd=((_tmp1015=_cycalloc(sizeof(*
_tmp1015)),((_tmp1015->f1=(struct Cyc_Absyn_Aggrfield*)sd_fields->hd,((_tmp1015->f2=
0,_tmp1015)))))),((_tmp1014->tl=fields,_tmp1014))))));}}}fields=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(fields);{struct Cyc_List_List*ans=0;for(
0;des != 0;des=des->tl){struct _tuple21 _tmp9CC;struct Cyc_List_List*_tmp9CD;void*
_tmp9CE;struct _tuple21*_tmp9CB=(struct _tuple21*)des->hd;_tmp9CC=*_tmp9CB;_tmp9CD=
_tmp9CC.f1;_tmp9CE=_tmp9CC.f2;if(_tmp9CD == 0){struct Cyc_List_List*_tmp9CF=fields;
for(0;_tmp9CF != 0;_tmp9CF=_tmp9CF->tl){if(!(*((struct _tuple20*)_tmp9CF->hd)).f2){(*((
struct _tuple20*)_tmp9CF->hd)).f2=1;{struct Cyc_Absyn_FieldName_struct*_tmp101B;
struct Cyc_Absyn_FieldName_struct _tmp101A;struct Cyc_List_List*_tmp1019;(*((struct
_tuple21*)des->hd)).f1=(struct Cyc_List_List*)((_tmp1019=_cycalloc(sizeof(*
_tmp1019)),((_tmp1019->hd=(void*)((void*)((_tmp101B=_cycalloc(sizeof(*_tmp101B)),((
_tmp101B[0]=((_tmp101A.tag=1,((_tmp101A.f1=((*((struct _tuple20*)_tmp9CF->hd)).f1)->name,
_tmp101A)))),_tmp101B))))),((_tmp1019->tl=0,_tmp1019))))));}{struct _tuple22*
_tmp101E;struct Cyc_List_List*_tmp101D;ans=((_tmp101D=_region_malloc(rgn,sizeof(*
_tmp101D)),((_tmp101D->hd=((_tmp101E=_region_malloc(rgn,sizeof(*_tmp101E)),((
_tmp101E->f1=(*((struct _tuple20*)_tmp9CF->hd)).f1,((_tmp101E->f2=_tmp9CE,
_tmp101E)))))),((_tmp101D->tl=ans,_tmp101D))))));}break;}}if(_tmp9CF == 0){const
char*_tmp1021;void*_tmp1020;(_tmp1020=0,Cyc_Tcutil_terr(loc,((_tmp1021="too many arguments to struct",
_tag_dyneither(_tmp1021,sizeof(char),29))),_tag_dyneither(_tmp1020,sizeof(void*),
0)));}}else{if(_tmp9CD->tl != 0){const char*_tmp1024;void*_tmp1023;(_tmp1023=0,Cyc_Tcutil_terr(
loc,((_tmp1024="multiple designators are not supported",_tag_dyneither(_tmp1024,
sizeof(char),39))),_tag_dyneither(_tmp1023,sizeof(void*),0)));}else{void*_tmp9D9=(
void*)_tmp9CD->hd;struct _dyneither_ptr*_tmp9DA;_LL60E: if(*((int*)_tmp9D9)!= 0)
goto _LL610;_LL60F:{const char*_tmp1027;void*_tmp1026;(_tmp1026=0,Cyc_Tcutil_terr(
loc,((_tmp1027="array designator used in argument to struct",_tag_dyneither(
_tmp1027,sizeof(char),44))),_tag_dyneither(_tmp1026,sizeof(void*),0)));}goto
_LL60D;_LL610: if(*((int*)_tmp9D9)!= 1)goto _LL60D;_tmp9DA=((struct Cyc_Absyn_FieldName_struct*)
_tmp9D9)->f1;_LL611: {struct Cyc_List_List*_tmp9DD=fields;for(0;_tmp9DD != 0;
_tmp9DD=_tmp9DD->tl){if(Cyc_strptrcmp(_tmp9DA,((*((struct _tuple20*)_tmp9DD->hd)).f1)->name)
== 0){if((*((struct _tuple20*)_tmp9DD->hd)).f2){const char*_tmp102B;void*_tmp102A[
1];struct Cyc_String_pa_struct _tmp1029;(_tmp1029.tag=0,((_tmp1029.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*_tmp9DA),((_tmp102A[0]=& _tmp1029,Cyc_Tcutil_terr(
loc,((_tmp102B="field %s has already been used as an argument",_tag_dyneither(
_tmp102B,sizeof(char),46))),_tag_dyneither(_tmp102A,sizeof(void*),1)))))));}(*((
struct _tuple20*)_tmp9DD->hd)).f2=1;{struct _tuple22*_tmp102E;struct Cyc_List_List*
_tmp102D;ans=((_tmp102D=_region_malloc(rgn,sizeof(*_tmp102D)),((_tmp102D->hd=((
_tmp102E=_region_malloc(rgn,sizeof(*_tmp102E)),((_tmp102E->f1=(*((struct _tuple20*)
_tmp9DD->hd)).f1,((_tmp102E->f2=_tmp9CE,_tmp102E)))))),((_tmp102D->tl=ans,
_tmp102D))))));}break;}}if(_tmp9DD == 0){const char*_tmp1032;void*_tmp1031[1];
struct Cyc_String_pa_struct _tmp1030;(_tmp1030.tag=0,((_tmp1030.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*_tmp9DA),((_tmp1031[0]=& _tmp1030,Cyc_Tcutil_terr(
loc,((_tmp1032="bad field designator %s",_tag_dyneither(_tmp1032,sizeof(char),24))),
_tag_dyneither(_tmp1031,sizeof(void*),1)))))));}goto _LL60D;}_LL60D:;}}}for(0;
fields != 0;fields=fields->tl){if(!(*((struct _tuple20*)fields->hd)).f2){{const
char*_tmp1035;void*_tmp1034;(_tmp1034=0,Cyc_Tcutil_terr(loc,((_tmp1035="too few arguments to struct",
_tag_dyneither(_tmp1035,sizeof(char),28))),_tag_dyneither(_tmp1034,sizeof(void*),
0)));}break;}}return((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
ans);}}int Cyc_Tcutil_is_tagged_pointer_typ_elt(void*t,void**elt_typ_dest);int Cyc_Tcutil_is_tagged_pointer_typ_elt(
void*t,void**elt_typ_dest){void*_tmp9E8=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo
_tmp9E9;void*_tmp9EA;struct Cyc_Absyn_PtrAtts _tmp9EB;struct Cyc_Absyn_Conref*
_tmp9EC;_LL613: if(_tmp9E8 <= (void*)4)goto _LL615;if(*((int*)_tmp9E8)!= 4)goto
_LL615;_tmp9E9=((struct Cyc_Absyn_PointerType_struct*)_tmp9E8)->f1;_tmp9EA=(void*)
_tmp9E9.elt_typ;_tmp9EB=_tmp9E9.ptr_atts;_tmp9EC=_tmp9EB.bounds;_LL614: {struct
Cyc_Absyn_Conref*_tmp9ED=Cyc_Absyn_compress_conref(_tmp9EC);union Cyc_Absyn_Constraint_union
_tmp9EE=(Cyc_Absyn_compress_conref(_tmp9ED))->v;void*_tmp9EF;_LL618: if((_tmp9EE.Eq_constr).tag
!= 0)goto _LL61A;_tmp9EF=(_tmp9EE.Eq_constr).f1;if((int)_tmp9EF != 0)goto _LL61A;
_LL619:*elt_typ_dest=_tmp9EA;return 1;_LL61A: if((_tmp9EE.No_constr).tag != 2)goto
_LL61C;_LL61B:{union Cyc_Absyn_Constraint_union _tmp1036;_tmp9ED->v=(union Cyc_Absyn_Constraint_union)(((
_tmp1036.Eq_constr).tag=0,(((_tmp1036.Eq_constr).f1=(void*)((void*)0),_tmp1036))));}*
elt_typ_dest=_tmp9EA;return 1;_LL61C:;_LL61D: return 0;_LL617:;}_LL615:;_LL616:
return 0;_LL612:;}int Cyc_Tcutil_is_zero_pointer_typ_elt(void*t,void**elt_typ_dest);
int Cyc_Tcutil_is_zero_pointer_typ_elt(void*t,void**elt_typ_dest){void*_tmp9F1=
Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp9F2;void*_tmp9F3;struct Cyc_Absyn_PtrAtts
_tmp9F4;struct Cyc_Absyn_Conref*_tmp9F5;_LL61F: if(_tmp9F1 <= (void*)4)goto _LL621;
if(*((int*)_tmp9F1)!= 4)goto _LL621;_tmp9F2=((struct Cyc_Absyn_PointerType_struct*)
_tmp9F1)->f1;_tmp9F3=(void*)_tmp9F2.elt_typ;_tmp9F4=_tmp9F2.ptr_atts;_tmp9F5=
_tmp9F4.zero_term;_LL620:*elt_typ_dest=_tmp9F3;return((int(*)(int,struct Cyc_Absyn_Conref*
x))Cyc_Absyn_conref_def)(0,_tmp9F5);_LL621:;_LL622: return 0;_LL61E:;}int Cyc_Tcutil_is_zero_ptr_type(
void*t,void**ptr_type,int*is_dyneither,void**elt_type);int Cyc_Tcutil_is_zero_ptr_type(
void*t,void**ptr_type,int*is_dyneither,void**elt_type){void*_tmp9F6=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_PtrInfo _tmp9F7;void*_tmp9F8;struct Cyc_Absyn_PtrAtts _tmp9F9;
struct Cyc_Absyn_Conref*_tmp9FA;struct Cyc_Absyn_Conref*_tmp9FB;struct Cyc_Absyn_ArrayInfo
_tmp9FC;void*_tmp9FD;struct Cyc_Absyn_Tqual _tmp9FE;struct Cyc_Absyn_Exp*_tmp9FF;
struct Cyc_Absyn_Conref*_tmpA00;_LL624: if(_tmp9F6 <= (void*)4)goto _LL628;if(*((int*)
_tmp9F6)!= 4)goto _LL626;_tmp9F7=((struct Cyc_Absyn_PointerType_struct*)_tmp9F6)->f1;
_tmp9F8=(void*)_tmp9F7.elt_typ;_tmp9F9=_tmp9F7.ptr_atts;_tmp9FA=_tmp9F9.bounds;
_tmp9FB=_tmp9F9.zero_term;_LL625: if(((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,_tmp9FB)){*ptr_type=t;*elt_type=_tmp9F8;{void*_tmpA01=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,
_tmp9FA);_LL62B: if((int)_tmpA01 != 0)goto _LL62D;_LL62C:*is_dyneither=1;goto _LL62A;
_LL62D:;_LL62E:*is_dyneither=0;goto _LL62A;_LL62A:;}return 1;}else{return 0;}_LL626:
if(*((int*)_tmp9F6)!= 7)goto _LL628;_tmp9FC=((struct Cyc_Absyn_ArrayType_struct*)
_tmp9F6)->f1;_tmp9FD=(void*)_tmp9FC.elt_type;_tmp9FE=_tmp9FC.tq;_tmp9FF=_tmp9FC.num_elts;
_tmpA00=_tmp9FC.zero_term;_LL627: if(((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,_tmpA00)){*elt_type=_tmp9FD;*is_dyneither=0;{struct Cyc_Absyn_PointerType_struct
_tmp104B;struct Cyc_Absyn_PtrAtts _tmp104A;struct Cyc_Absyn_Upper_b_struct _tmp1049;
struct Cyc_Absyn_Upper_b_struct*_tmp1048;struct Cyc_Absyn_PtrInfo _tmp1047;struct
Cyc_Absyn_PointerType_struct*_tmp1046;*ptr_type=(void*)((_tmp1046=_cycalloc(
sizeof(*_tmp1046)),((_tmp1046[0]=((_tmp104B.tag=4,((_tmp104B.f1=((_tmp1047.elt_typ=(
void*)_tmp9FD,((_tmp1047.elt_tq=_tmp9FE,((_tmp1047.ptr_atts=((_tmp104A.rgn=(void*)((
void*)2),((_tmp104A.nullable=Cyc_Absyn_false_conref,((_tmp104A.bounds=Cyc_Absyn_new_conref((
void*)((_tmp1048=_cycalloc(sizeof(*_tmp1048)),((_tmp1048[0]=((_tmp1049.tag=0,((
_tmp1049.f1=(struct Cyc_Absyn_Exp*)_check_null(_tmp9FF),_tmp1049)))),_tmp1048))))),((
_tmp104A.zero_term=_tmpA00,((_tmp104A.ptrloc=0,_tmp104A)))))))))),_tmp1047)))))),
_tmp104B)))),_tmp1046))));}return 1;}else{return 0;}_LL628:;_LL629: return 0;_LL623:;}
int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*e1,void**ptr_type,int*
is_dyneither,void**elt_type);int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*
e1,void**ptr_type,int*is_dyneither,void**elt_type){void*_tmpA08=(void*)e1->r;
struct Cyc_Absyn_Exp*_tmpA09;struct Cyc_Absyn_Exp*_tmpA0A;struct Cyc_Absyn_Exp*
_tmpA0B;struct Cyc_Absyn_Exp*_tmpA0C;struct Cyc_Absyn_Exp*_tmpA0D;struct Cyc_Absyn_Exp*
_tmpA0E;_LL630: if(*((int*)_tmpA08)!= 15)goto _LL632;_LL631: {const char*_tmp104F;
void*_tmp104E[1];struct Cyc_String_pa_struct _tmp104D;(_tmp104D.tag=0,((_tmp104D.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e1)),((
_tmp104E[0]=& _tmp104D,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmp104F="we have a cast in a lhs:  %s",_tag_dyneither(
_tmp104F,sizeof(char),29))),_tag_dyneither(_tmp104E,sizeof(void*),1)))))));}
_LL632: if(*((int*)_tmpA08)!= 22)goto _LL634;_tmpA09=((struct Cyc_Absyn_Deref_e_struct*)
_tmpA08)->f1;_LL633: _tmpA0A=_tmpA09;goto _LL635;_LL634: if(*((int*)_tmpA08)!= 25)
goto _LL636;_tmpA0A=((struct Cyc_Absyn_Subscript_e_struct*)_tmpA08)->f1;_LL635:
return Cyc_Tcutil_is_zero_ptr_type((void*)((struct Cyc_Core_Opt*)_check_null(
_tmpA0A->topt))->v,ptr_type,is_dyneither,elt_type);_LL636: if(*((int*)_tmpA08)!= 
24)goto _LL638;_tmpA0B=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmpA08)->f1;_LL637:
_tmpA0C=_tmpA0B;goto _LL639;_LL638: if(*((int*)_tmpA08)!= 23)goto _LL63A;_tmpA0C=((
struct Cyc_Absyn_AggrMember_e_struct*)_tmpA08)->f1;_LL639: if(Cyc_Tcutil_is_zero_ptr_type((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpA0C->topt))->v,ptr_type,is_dyneither,
elt_type)){const char*_tmp1053;void*_tmp1052[1];struct Cyc_String_pa_struct
_tmp1051;(_tmp1051.tag=0,((_tmp1051.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_exp2string(e1)),((_tmp1052[0]=& _tmp1051,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1053="found zero pointer aggregate member assignment: %s",
_tag_dyneither(_tmp1053,sizeof(char),51))),_tag_dyneither(_tmp1052,sizeof(void*),
1)))))));}return 0;_LL63A: if(*((int*)_tmpA08)!= 14)goto _LL63C;_tmpA0D=((struct Cyc_Absyn_Instantiate_e_struct*)
_tmpA08)->f1;_LL63B: _tmpA0E=_tmpA0D;goto _LL63D;_LL63C: if(*((int*)_tmpA08)!= 13)
goto _LL63E;_tmpA0E=((struct Cyc_Absyn_NoInstantiate_e_struct*)_tmpA08)->f1;_LL63D:
if(Cyc_Tcutil_is_zero_ptr_type((void*)((struct Cyc_Core_Opt*)_check_null(_tmpA0E->topt))->v,
ptr_type,is_dyneither,elt_type)){const char*_tmp1057;void*_tmp1056[1];struct Cyc_String_pa_struct
_tmp1055;(_tmp1055.tag=0,((_tmp1055.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_exp2string(e1)),((_tmp1056[0]=& _tmp1055,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1057="found zero pointer instantiate/noinstantiate: %s",
_tag_dyneither(_tmp1057,sizeof(char),49))),_tag_dyneither(_tmp1056,sizeof(void*),
1)))))));}return 0;_LL63E: if(*((int*)_tmpA08)!= 1)goto _LL640;_LL63F: return 0;
_LL640:;_LL641: {const char*_tmp105B;void*_tmp105A[1];struct Cyc_String_pa_struct
_tmp1059;(_tmp1059.tag=0,((_tmp1059.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_exp2string(e1)),((_tmp105A[0]=& _tmp1059,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp105B="found bad lhs in is_zero_ptr_deref: %s",
_tag_dyneither(_tmp105B,sizeof(char),39))),_tag_dyneither(_tmp105A,sizeof(void*),
1)))))));}_LL62F:;}int Cyc_Tcutil_is_tagged_pointer_typ(void*t);int Cyc_Tcutil_is_tagged_pointer_typ(
void*t){void*ignore=(void*)0;return Cyc_Tcutil_is_tagged_pointer_typ_elt(t,&
ignore);}static int Cyc_Tcutil_is_noalias_region(void*r);static int Cyc_Tcutil_is_noalias_region(
void*r){void*_tmpA1B=Cyc_Tcutil_compress(r);struct Cyc_Absyn_Tvar*_tmpA1C;_LL643:
if((int)_tmpA1B != 3)goto _LL645;_LL644: return 1;_LL645: if(_tmpA1B <= (void*)4)goto
_LL647;if(*((int*)_tmpA1B)!= 1)goto _LL647;_tmpA1C=((struct Cyc_Absyn_VarType_struct*)
_tmpA1B)->f1;_LL646: return Cyc_Tcutil_tvar_kind(_tmpA1C)== (void*)4  || Cyc_Tcutil_tvar_kind(
_tmpA1C)== (void*)5;_LL647:;_LL648: return 0;_LL642:;}int Cyc_Tcutil_is_noalias_pointer(
void*t);int Cyc_Tcutil_is_noalias_pointer(void*t){void*_tmpA1D=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_PtrInfo _tmpA1E;struct Cyc_Absyn_PtrAtts _tmpA1F;void*_tmpA20;
_LL64A: if(_tmpA1D <= (void*)4)goto _LL64C;if(*((int*)_tmpA1D)!= 4)goto _LL64C;
_tmpA1E=((struct Cyc_Absyn_PointerType_struct*)_tmpA1D)->f1;_tmpA1F=_tmpA1E.ptr_atts;
_tmpA20=(void*)_tmpA1F.rgn;_LL64B: return Cyc_Tcutil_is_noalias_region(_tmpA20);
_LL64C:;_LL64D: return 0;_LL649:;}int Cyc_Tcutil_is_noalias_pointer_or_aggr(void*t);
int Cyc_Tcutil_is_noalias_pointer_or_aggr(void*t){void*_tmpA21=Cyc_Tcutil_compress(
t);if(Cyc_Tcutil_is_noalias_pointer(_tmpA21))return 1;{void*_tmpA22=_tmpA21;
struct Cyc_List_List*_tmpA23;struct Cyc_Absyn_AggrInfo _tmpA24;union Cyc_Absyn_AggrInfoU_union
_tmpA25;struct Cyc_Absyn_Aggrdecl**_tmpA26;struct Cyc_List_List*_tmpA27;struct Cyc_List_List*
_tmpA28;struct Cyc_Absyn_AggrInfo _tmpA29;union Cyc_Absyn_AggrInfoU_union _tmpA2A;
struct Cyc_Absyn_TunionInfo _tmpA2B;union Cyc_Absyn_TunionInfoU_union _tmpA2C;struct
Cyc_List_List*_tmpA2D;struct Cyc_Core_Opt*_tmpA2E;struct Cyc_Absyn_TunionFieldInfo
_tmpA2F;union Cyc_Absyn_TunionFieldInfoU_union _tmpA30;struct Cyc_List_List*_tmpA31;
_LL64F: if(_tmpA22 <= (void*)4)goto _LL65B;if(*((int*)_tmpA22)!= 9)goto _LL651;
_tmpA23=((struct Cyc_Absyn_TupleType_struct*)_tmpA22)->f1;_LL650: while(_tmpA23 != 
0){if(Cyc_Tcutil_is_noalias_pointer_or_aggr((*((struct _tuple5*)_tmpA23->hd)).f2))
return 1;_tmpA23=_tmpA23->tl;}return 0;_LL651: if(*((int*)_tmpA22)!= 10)goto _LL653;
_tmpA24=((struct Cyc_Absyn_AggrType_struct*)_tmpA22)->f1;_tmpA25=_tmpA24.aggr_info;
if((((((struct Cyc_Absyn_AggrType_struct*)_tmpA22)->f1).aggr_info).KnownAggr).tag
!= 1)goto _LL653;_tmpA26=(_tmpA25.KnownAggr).f1;_tmpA27=_tmpA24.targs;_LL652: if((*
_tmpA26)->impl == 0)return 0;else{{struct _RegionHandle _tmpA32=_new_region("rgn");
struct _RegionHandle*rgn=& _tmpA32;_push_region(rgn);{struct Cyc_List_List*_tmpA33=((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,(*_tmpA26)->tvs,_tmpA27);struct
Cyc_List_List*_tmpA34=((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmpA26)->impl))->fields;
void*t;while(_tmpA34 != 0){t=Cyc_Tcutil_rsubstitute(rgn,_tmpA33,(void*)((struct
Cyc_Absyn_Aggrfield*)_tmpA34->hd)->type);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(
t)){int _tmpA35=1;_npop_handler(0);return _tmpA35;}_tmpA34=_tmpA34->tl;}};
_pop_region(rgn);}return 0;}_LL653: if(*((int*)_tmpA22)!= 11)goto _LL655;_tmpA28=((
struct Cyc_Absyn_AnonAggrType_struct*)_tmpA22)->f2;_LL654: while(_tmpA28 != 0){if(
Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)((struct Cyc_Absyn_Aggrfield*)
_tmpA28->hd)->type))return 1;_tmpA28=_tmpA28->tl;}return 0;_LL655: if(*((int*)
_tmpA22)!= 10)goto _LL657;_tmpA29=((struct Cyc_Absyn_AggrType_struct*)_tmpA22)->f1;
_tmpA2A=_tmpA29.aggr_info;if((((((struct Cyc_Absyn_AggrType_struct*)_tmpA22)->f1).aggr_info).UnknownAggr).tag
!= 0)goto _LL657;_LL656: {const char*_tmp105E;void*_tmp105D;(_tmp105D=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp105E="got unknown aggr in is_noalias_aggr",
_tag_dyneither(_tmp105E,sizeof(char),36))),_tag_dyneither(_tmp105D,sizeof(void*),
0)));}_LL657: if(*((int*)_tmpA22)!= 2)goto _LL659;_tmpA2B=((struct Cyc_Absyn_TunionType_struct*)
_tmpA22)->f1;_tmpA2C=_tmpA2B.tunion_info;_tmpA2D=_tmpA2B.targs;_tmpA2E=_tmpA2B.rgn;
_LL658: {union Cyc_Absyn_TunionInfoU_union _tmpA38=_tmpA2C;struct Cyc_Absyn_UnknownTunionInfo
_tmpA39;struct _tuple2*_tmpA3A;int _tmpA3B;int _tmpA3C;struct Cyc_Absyn_Tuniondecl**
_tmpA3D;struct Cyc_Absyn_Tuniondecl*_tmpA3E;struct Cyc_Absyn_Tuniondecl _tmpA3F;
struct Cyc_List_List*_tmpA40;struct Cyc_Core_Opt*_tmpA41;int _tmpA42;_LL65E: if((
_tmpA38.UnknownTunion).tag != 0)goto _LL660;_tmpA39=(_tmpA38.UnknownTunion).f1;
_tmpA3A=_tmpA39.name;_tmpA3B=_tmpA39.is_xtunion;_tmpA3C=_tmpA39.is_flat;_LL65F: {
const char*_tmp1061;void*_tmp1060;(_tmp1060=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1061="got unknown tunion in is_noalias_aggr",
_tag_dyneither(_tmp1061,sizeof(char),38))),_tag_dyneither(_tmp1060,sizeof(void*),
0)));}_LL660: if((_tmpA38.KnownTunion).tag != 1)goto _LL65D;_tmpA3D=(_tmpA38.KnownTunion).f1;
_tmpA3E=*_tmpA3D;_tmpA3F=*_tmpA3E;_tmpA40=_tmpA3F.tvs;_tmpA41=_tmpA3F.fields;
_tmpA42=_tmpA3F.is_flat;_LL661: if(!_tmpA42){if(_tmpA2E == 0){const char*_tmp1064;
void*_tmp1063;(_tmp1063=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap))Cyc_Tcutil_impos)(((_tmp1064="got tunion with no region",_tag_dyneither(
_tmp1064,sizeof(char),26))),_tag_dyneither(_tmp1063,sizeof(void*),0)));}return
Cyc_Tcutil_is_noalias_region((void*)_tmpA2E->v);}else{if(_tmpA41 == 0){const char*
_tmp1067;void*_tmp1066;(_tmp1066=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1067="got tunion with no fields",
_tag_dyneither(_tmp1067,sizeof(char),26))),_tag_dyneither(_tmp1066,sizeof(void*),
0)));}else{{struct _RegionHandle _tmpA49=_new_region("rgn");struct _RegionHandle*
rgn=& _tmpA49;_push_region(rgn);{struct Cyc_List_List*_tmpA4A=((struct Cyc_List_List*(*)(
struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(rgn,rgn,_tmpA40,_tmpA2D);struct Cyc_List_List*_tmpA4B=(struct
Cyc_List_List*)_tmpA41->v;while(_tmpA4B != 0){struct Cyc_List_List*_tmpA4C=((
struct Cyc_Absyn_Tunionfield*)_tmpA4B->hd)->typs;while(_tmpA4C != 0){_tmpA21=Cyc_Tcutil_rsubstitute(
rgn,_tmpA4A,(*((struct _tuple5*)_tmpA4C->hd)).f2);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(
_tmpA21)){int _tmpA4D=1;_npop_handler(0);return _tmpA4D;}_tmpA4C=_tmpA4C->tl;}
_tmpA4B=_tmpA4B->tl;}};_pop_region(rgn);}return 0;}}_LL65D:;}_LL659: if(*((int*)
_tmpA22)!= 3)goto _LL65B;_tmpA2F=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmpA22)->f1;_tmpA30=_tmpA2F.field_info;_tmpA31=_tmpA2F.targs;_LL65A: {union Cyc_Absyn_TunionFieldInfoU_union
_tmpA4E=_tmpA30;struct Cyc_Absyn_Tuniondecl*_tmpA4F;struct Cyc_Absyn_Tunionfield*
_tmpA50;_LL663: if((_tmpA4E.UnknownTunionfield).tag != 0)goto _LL665;_LL664: {const
char*_tmp106A;void*_tmp1069;(_tmp1069=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp106A="got unknown tunion field in is_noalias_aggr",
_tag_dyneither(_tmp106A,sizeof(char),44))),_tag_dyneither(_tmp1069,sizeof(void*),
0)));}_LL665: if((_tmpA4E.KnownTunionfield).tag != 1)goto _LL662;_tmpA4F=(_tmpA4E.KnownTunionfield).f1;
_tmpA50=(_tmpA4E.KnownTunionfield).f2;_LL666:{struct _RegionHandle _tmpA53=
_new_region("rgn");struct _RegionHandle*rgn=& _tmpA53;_push_region(rgn);{struct Cyc_List_List*
_tmpA54=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmpA4F->tvs,
_tmpA31);struct Cyc_List_List*_tmpA55=_tmpA50->typs;while(_tmpA55 != 0){_tmpA21=
Cyc_Tcutil_rsubstitute(rgn,_tmpA54,(*((struct _tuple5*)_tmpA55->hd)).f2);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(
_tmpA21)){int _tmpA56=1;_npop_handler(0);return _tmpA56;}_tmpA55=_tmpA55->tl;}};
_pop_region(rgn);}return 0;_LL662:;}_LL65B:;_LL65C: return 0;_LL64E:;}}static int Cyc_Tcutil_is_noalias_field(
void*t,struct _dyneither_ptr*f);static int Cyc_Tcutil_is_noalias_field(void*t,
struct _dyneither_ptr*f){void*_tmpA57=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo
_tmpA58;union Cyc_Absyn_AggrInfoU_union _tmpA59;struct Cyc_Absyn_Aggrdecl**_tmpA5A;
struct Cyc_Absyn_Aggrdecl*_tmpA5B;struct Cyc_List_List*_tmpA5C;struct Cyc_List_List*
_tmpA5D;_LL668: if(_tmpA57 <= (void*)4)goto _LL66C;if(*((int*)_tmpA57)!= 10)goto
_LL66A;_tmpA58=((struct Cyc_Absyn_AggrType_struct*)_tmpA57)->f1;_tmpA59=_tmpA58.aggr_info;
if((((((struct Cyc_Absyn_AggrType_struct*)_tmpA57)->f1).aggr_info).KnownAggr).tag
!= 1)goto _LL66A;_tmpA5A=(_tmpA59.KnownAggr).f1;_tmpA5B=*_tmpA5A;_tmpA5C=_tmpA58.targs;
_LL669: _tmpA5D=_tmpA5B->impl == 0?0:((struct Cyc_Absyn_AggrdeclImpl*)_check_null(
_tmpA5B->impl))->fields;goto _LL66B;_LL66A: if(*((int*)_tmpA57)!= 11)goto _LL66C;
_tmpA5D=((struct Cyc_Absyn_AnonAggrType_struct*)_tmpA57)->f2;_LL66B: {struct Cyc_Absyn_Aggrfield*
_tmpA5E=Cyc_Absyn_lookup_field(_tmpA5D,f);{struct Cyc_Absyn_Aggrfield*_tmpA5F=
_tmpA5E;struct Cyc_Absyn_Aggrfield _tmpA60;void*_tmpA61;_LL66F: if(_tmpA5F != 0)goto
_LL671;_LL670: {const char*_tmp106D;void*_tmp106C;(_tmp106C=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp106D="is_noalias_field: missing field",
_tag_dyneither(_tmp106D,sizeof(char),32))),_tag_dyneither(_tmp106C,sizeof(void*),
0)));}_LL671: if(_tmpA5F == 0)goto _LL66E;_tmpA60=*_tmpA5F;_tmpA61=(void*)_tmpA60.type;
_LL672: return Cyc_Tcutil_is_noalias_pointer_or_aggr(_tmpA61);_LL66E:;}return 0;}
_LL66C:;_LL66D: {const char*_tmp1071;void*_tmp1070[1];struct Cyc_String_pa_struct
_tmp106F;(_tmp106F.tag=0,((_tmp106F.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp1070[0]=& _tmp106F,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1071="is_noalias_field: invalid type |%s|",
_tag_dyneither(_tmp1071,sizeof(char),36))),_tag_dyneither(_tmp1070,sizeof(void*),
1)))))));}_LL667:;}static int Cyc_Tcutil_is_noalias_path_aux(struct Cyc_Absyn_Exp*e,
int ignore_leaf);static int Cyc_Tcutil_is_noalias_path_aux(struct Cyc_Absyn_Exp*e,
int ignore_leaf){void*_tmpA67=(void*)e->r;void*_tmpA68;struct Cyc_Absyn_Exp*
_tmpA69;struct _dyneither_ptr*_tmpA6A;struct Cyc_Absyn_Exp*_tmpA6B;struct Cyc_Absyn_Exp*
_tmpA6C;void*_tmpA6D;void*_tmpA6E;void*_tmpA6F;struct Cyc_Absyn_Exp*_tmpA70;
struct Cyc_Absyn_Exp*_tmpA71;struct Cyc_Absyn_Exp*_tmpA72;struct Cyc_Absyn_Exp*
_tmpA73;void*_tmpA74;struct Cyc_Absyn_Exp*_tmpA75;struct Cyc_Absyn_Stmt*_tmpA76;
_LL674: if(*((int*)_tmpA67)!= 5)goto _LL676;_LL675: goto _LL677;_LL676: if(*((int*)
_tmpA67)!= 7)goto _LL678;_LL677: goto _LL679;_LL678: if(*((int*)_tmpA67)!= 8)goto
_LL67A;_LL679: goto _LL67B;_LL67A: if(*((int*)_tmpA67)!= 12)goto _LL67C;_LL67B: goto
_LL67D;_LL67C: if(*((int*)_tmpA67)!= 16)goto _LL67E;_LL67D: goto _LL67F;_LL67E: if(*((
int*)_tmpA67)!= 18)goto _LL680;_LL67F: goto _LL681;_LL680: if(*((int*)_tmpA67)!= 19)
goto _LL682;_LL681: goto _LL683;_LL682: if(*((int*)_tmpA67)!= 20)goto _LL684;_LL683:
goto _LL685;_LL684: if(*((int*)_tmpA67)!= 21)goto _LL686;_LL685: goto _LL687;_LL686:
if(*((int*)_tmpA67)!= 27)goto _LL688;_LL687: goto _LL689;_LL688: if(*((int*)_tmpA67)
!= 29)goto _LL68A;_LL689: goto _LL68B;_LL68A: if(*((int*)_tmpA67)!= 28)goto _LL68C;
_LL68B: goto _LL68D;_LL68C: if(*((int*)_tmpA67)!= 33)goto _LL68E;_LL68D: goto _LL68F;
_LL68E: if(*((int*)_tmpA67)!= 34)goto _LL690;_LL68F: goto _LL691;_LL690: if(*((int*)
_tmpA67)!= 36)goto _LL692;_LL691: goto _LL693;_LL692: if(*((int*)_tmpA67)!= 1)goto
_LL694;_tmpA68=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmpA67)->f2;if(_tmpA68 <= (
void*)1)goto _LL694;if(*((int*)_tmpA68)!= 0)goto _LL694;_LL693: goto _LL695;_LL694:
if(*((int*)_tmpA67)!= 3)goto _LL696;_LL695: return 0;_LL696: if(*((int*)_tmpA67)!= 
22)goto _LL698;_LL697: goto _LL699;_LL698: if(*((int*)_tmpA67)!= 24)goto _LL69A;
_LL699: return 0;_LL69A: if(*((int*)_tmpA67)!= 23)goto _LL69C;_tmpA69=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmpA67)->f1;_tmpA6A=((struct Cyc_Absyn_AggrMember_e_struct*)_tmpA67)->f2;_LL69B:
return(ignore_leaf  || Cyc_Tcutil_is_noalias_field((void*)((struct Cyc_Core_Opt*)
_check_null(_tmpA69->topt))->v,_tmpA6A)) && Cyc_Tcutil_is_noalias_path_aux(
_tmpA69,0);_LL69C: if(*((int*)_tmpA67)!= 25)goto _LL69E;_tmpA6B=((struct Cyc_Absyn_Subscript_e_struct*)
_tmpA67)->f1;_tmpA6C=((struct Cyc_Absyn_Subscript_e_struct*)_tmpA67)->f2;_LL69D: {
void*_tmpA77=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmpA6B->topt))->v);struct Cyc_Absyn_PtrInfo _tmpA78;void*_tmpA79;struct Cyc_List_List*
_tmpA7A;_LL6C1: if(_tmpA77 <= (void*)4)goto _LL6C5;if(*((int*)_tmpA77)!= 4)goto
_LL6C3;_tmpA78=((struct Cyc_Absyn_PointerType_struct*)_tmpA77)->f1;_tmpA79=(void*)
_tmpA78.elt_typ;_LL6C2: return 0;_LL6C3: if(*((int*)_tmpA77)!= 9)goto _LL6C5;_tmpA7A=((
struct Cyc_Absyn_TupleType_struct*)_tmpA77)->f1;_LL6C4: {unsigned int _tmpA7C;int
_tmpA7D;struct _tuple7 _tmpA7B=Cyc_Evexp_eval_const_uint_exp(_tmpA6C);_tmpA7C=
_tmpA7B.f1;_tmpA7D=_tmpA7B.f2;if(!_tmpA7D){const char*_tmp1074;void*_tmp1073;(
_tmp1073=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp1074="is_noalias_path_aux: non-constant subscript",_tag_dyneither(_tmp1074,
sizeof(char),44))),_tag_dyneither(_tmp1073,sizeof(void*),0)));}{struct
_handler_cons _tmpA80;_push_handler(& _tmpA80);{int _tmpA82=0;if(setjmp(_tmpA80.handler))
_tmpA82=1;if(!_tmpA82){{void*_tmpA83=(*((struct _tuple5*(*)(struct Cyc_List_List*x,
int n))Cyc_List_nth)(_tmpA7A,(int)_tmpA7C)).f2;int _tmpA84=(ignore_leaf  || Cyc_Tcutil_is_noalias_pointer_or_aggr(
_tmpA83)) && Cyc_Tcutil_is_noalias_path_aux(_tmpA6B,0);_npop_handler(0);return
_tmpA84;};_pop_handler();}else{void*_tmpA81=(void*)_exn_thrown;void*_tmpA86=
_tmpA81;_LL6C8: if(_tmpA86 != Cyc_List_Nth)goto _LL6CA;_LL6C9: {const char*_tmp1077;
void*_tmp1076;return(_tmp1076=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1077="is_noalias_path_aux: out-of-bounds subscript",
_tag_dyneither(_tmp1077,sizeof(char),45))),_tag_dyneither(_tmp1076,sizeof(void*),
0)));}_LL6CA:;_LL6CB:(void)_throw(_tmpA86);_LL6C7:;}}}}_LL6C5:;_LL6C6: {const
char*_tmp107A;void*_tmp1079;(_tmp1079=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp107A="is_noalias_path_aux: subscript on non-pointer/tuple",
_tag_dyneither(_tmp107A,sizeof(char),52))),_tag_dyneither(_tmp1079,sizeof(void*),
0)));}_LL6C0:;}_LL69E: if(*((int*)_tmpA67)!= 32)goto _LL6A0;_LL69F: goto _LL6A1;
_LL6A0: if(*((int*)_tmpA67)!= 26)goto _LL6A2;_LL6A1: goto _LL6A3;_LL6A2: if(*((int*)
_tmpA67)!= 30)goto _LL6A4;_LL6A3: goto _LL6A5;_LL6A4: if(*((int*)_tmpA67)!= 31)goto
_LL6A6;_LL6A5: goto _LL6A7;_LL6A6: if(*((int*)_tmpA67)!= 0)goto _LL6A8;_LL6A7: goto
_LL6A9;_LL6A8: if(*((int*)_tmpA67)!= 35)goto _LL6AA;_LL6A9: goto _LL6AB;_LL6AA: if(*((
int*)_tmpA67)!= 17)goto _LL6AC;_LL6AB: goto _LL6AD;_LL6AC: if(*((int*)_tmpA67)!= 1)
goto _LL6AE;_tmpA6D=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmpA67)->f2;if(
_tmpA6D <= (void*)1)goto _LL6AE;if(*((int*)_tmpA6D)!= 3)goto _LL6AE;_LL6AD: goto
_LL6AF;_LL6AE: if(*((int*)_tmpA67)!= 1)goto _LL6B0;_tmpA6E=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmpA67)->f2;if(_tmpA6E <= (void*)1)goto _LL6B0;if(*((int*)_tmpA6E)!= 4)goto _LL6B0;
_LL6AF: goto _LL6B1;_LL6B0: if(*((int*)_tmpA67)!= 1)goto _LL6B2;_tmpA6F=(void*)((
struct Cyc_Absyn_Var_e_struct*)_tmpA67)->f2;if(_tmpA6F <= (void*)1)goto _LL6B2;if(*((
int*)_tmpA6F)!= 2)goto _LL6B2;_LL6B1: {int _tmpA8B=ignore_leaf  || Cyc_Tcutil_is_noalias_pointer_or_aggr((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);return _tmpA8B;}_LL6B2: if(*((
int*)_tmpA67)!= 6)goto _LL6B4;_tmpA70=((struct Cyc_Absyn_Conditional_e_struct*)
_tmpA67)->f2;_LL6B3: _tmpA71=_tmpA70;goto _LL6B5;_LL6B4: if(*((int*)_tmpA67)!= 9)
goto _LL6B6;_tmpA71=((struct Cyc_Absyn_SeqExp_e_struct*)_tmpA67)->f2;_LL6B5:
_tmpA72=_tmpA71;goto _LL6B7;_LL6B6: if(*((int*)_tmpA67)!= 4)goto _LL6B8;_tmpA72=((
struct Cyc_Absyn_AssignOp_e_struct*)_tmpA67)->f1;_LL6B7: return Cyc_Tcutil_is_noalias_path_aux(
_tmpA72,ignore_leaf);_LL6B8: if(*((int*)_tmpA67)!= 11)goto _LL6BA;_tmpA73=((struct
Cyc_Absyn_FnCall_e_struct*)_tmpA67)->f1;_LL6B9: {void*_tmpA8C=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpA73->topt))->v);struct Cyc_Absyn_FnInfo
_tmpA8D;void*_tmpA8E;struct Cyc_Absyn_PtrInfo _tmpA8F;void*_tmpA90;_LL6CD: if(
_tmpA8C <= (void*)4)goto _LL6D1;if(*((int*)_tmpA8C)!= 8)goto _LL6CF;_tmpA8D=((
struct Cyc_Absyn_FnType_struct*)_tmpA8C)->f1;_tmpA8E=(void*)_tmpA8D.ret_typ;
_LL6CE: return ignore_leaf  || Cyc_Tcutil_is_noalias_pointer_or_aggr(_tmpA8E);
_LL6CF: if(*((int*)_tmpA8C)!= 4)goto _LL6D1;_tmpA8F=((struct Cyc_Absyn_PointerType_struct*)
_tmpA8C)->f1;_tmpA90=(void*)_tmpA8F.elt_typ;_LL6D0:{void*_tmpA91=Cyc_Tcutil_compress(
_tmpA90);struct Cyc_Absyn_FnInfo _tmpA92;void*_tmpA93;_LL6D4: if(_tmpA91 <= (void*)4)
goto _LL6D6;if(*((int*)_tmpA91)!= 8)goto _LL6D6;_tmpA92=((struct Cyc_Absyn_FnType_struct*)
_tmpA91)->f1;_tmpA93=(void*)_tmpA92.ret_typ;_LL6D5: return ignore_leaf  || Cyc_Tcutil_is_noalias_pointer_or_aggr(
_tmpA93);_LL6D6:;_LL6D7: goto _LL6D3;_LL6D3:;}goto _LL6D2;_LL6D1:;_LL6D2: {const
char*_tmp107E;void*_tmp107D[1];struct Cyc_String_pa_struct _tmp107C;(_tmp107C.tag=
0,((_tmp107C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpA73->topt))->v)),((_tmp107D[0]=&
_tmp107C,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp107E="Fncall has non function type %s",_tag_dyneither(_tmp107E,sizeof(char),
32))),_tag_dyneither(_tmp107D,sizeof(void*),1)))))));}_LL6CC:;}_LL6BA: if(*((int*)
_tmpA67)!= 15)goto _LL6BC;_tmpA74=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmpA67)->f1;
_tmpA75=((struct Cyc_Absyn_Cast_e_struct*)_tmpA67)->f2;_LL6BB: return Cyc_Tcutil_is_noalias_pointer_or_aggr(
_tmpA74) && Cyc_Tcutil_is_noalias_path_aux(_tmpA75,1);_LL6BC: if(*((int*)_tmpA67)
!= 38)goto _LL6BE;_tmpA76=((struct Cyc_Absyn_StmtExp_e_struct*)_tmpA67)->f1;_LL6BD:
while(1){void*_tmpA97=(void*)_tmpA76->r;struct Cyc_Absyn_Stmt*_tmpA98;struct Cyc_Absyn_Stmt*
_tmpA99;struct Cyc_Absyn_Decl*_tmpA9A;struct Cyc_Absyn_Stmt*_tmpA9B;struct Cyc_Absyn_Exp*
_tmpA9C;_LL6D9: if(_tmpA97 <= (void*)1)goto _LL6DF;if(*((int*)_tmpA97)!= 1)goto
_LL6DB;_tmpA98=((struct Cyc_Absyn_Seq_s_struct*)_tmpA97)->f1;_tmpA99=((struct Cyc_Absyn_Seq_s_struct*)
_tmpA97)->f2;_LL6DA: _tmpA76=_tmpA99;goto _LL6D8;_LL6DB: if(*((int*)_tmpA97)!= 11)
goto _LL6DD;_tmpA9A=((struct Cyc_Absyn_Decl_s_struct*)_tmpA97)->f1;_tmpA9B=((
struct Cyc_Absyn_Decl_s_struct*)_tmpA97)->f2;_LL6DC: _tmpA76=_tmpA9B;goto _LL6D8;
_LL6DD: if(*((int*)_tmpA97)!= 0)goto _LL6DF;_tmpA9C=((struct Cyc_Absyn_Exp_s_struct*)
_tmpA97)->f1;_LL6DE: return Cyc_Tcutil_is_noalias_path_aux(_tmpA9C,ignore_leaf);
_LL6DF:;_LL6E0: {const char*_tmp1081;void*_tmp1080;(_tmp1080=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1081="is_noalias_stmt_exp: ill-formed StmtExp",
_tag_dyneither(_tmp1081,sizeof(char),40))),_tag_dyneither(_tmp1080,sizeof(void*),
0)));}_LL6D8:;}_LL6BE:;_LL6BF: {const char*_tmp1085;void*_tmp1084[1];struct Cyc_String_pa_struct
_tmp1083;(_tmp1083.tag=0,((_tmp1083.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmp1084[0]=& _tmp1083,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1085="Called is_noalias_path_aux with bogus exp |%s|\n",
_tag_dyneither(_tmp1085,sizeof(char),48))),_tag_dyneither(_tmp1084,sizeof(void*),
1)))))));}_LL673:;}int Cyc_Tcutil_is_noalias_path(struct Cyc_Absyn_Exp*e);int Cyc_Tcutil_is_noalias_path(
struct Cyc_Absyn_Exp*e){return Cyc_Tcutil_is_noalias_path_aux(e,0);}struct _tuple10
Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e);struct
_tuple10 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
struct _tuple10 _tmp1086;struct _tuple10 bogus_ans=(_tmp1086.f1=0,((_tmp1086.f2=(
void*)2,_tmp1086)));void*_tmpAA2=(void*)e->r;struct _tuple2*_tmpAA3;void*_tmpAA4;
struct Cyc_Absyn_Exp*_tmpAA5;struct _dyneither_ptr*_tmpAA6;struct Cyc_Absyn_Exp*
_tmpAA7;struct _dyneither_ptr*_tmpAA8;struct Cyc_Absyn_Exp*_tmpAA9;struct Cyc_Absyn_Exp*
_tmpAAA;struct Cyc_Absyn_Exp*_tmpAAB;_LL6E2: if(*((int*)_tmpAA2)!= 1)goto _LL6E4;
_tmpAA3=((struct Cyc_Absyn_Var_e_struct*)_tmpAA2)->f1;_tmpAA4=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmpAA2)->f2;_LL6E3: {void*_tmpAAC=_tmpAA4;struct Cyc_Absyn_Vardecl*_tmpAAD;
struct Cyc_Absyn_Vardecl*_tmpAAE;struct Cyc_Absyn_Vardecl*_tmpAAF;struct Cyc_Absyn_Vardecl*
_tmpAB0;_LL6EF: if((int)_tmpAAC != 0)goto _LL6F1;_LL6F0: goto _LL6F2;_LL6F1: if(
_tmpAAC <= (void*)1)goto _LL6F3;if(*((int*)_tmpAAC)!= 1)goto _LL6F3;_LL6F2: return
bogus_ans;_LL6F3: if(_tmpAAC <= (void*)1)goto _LL6F5;if(*((int*)_tmpAAC)!= 0)goto
_LL6F5;_tmpAAD=((struct Cyc_Absyn_Global_b_struct*)_tmpAAC)->f1;_LL6F4: {void*
_tmpAB1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
_LL6FC: if(_tmpAB1 <= (void*)4)goto _LL6FE;if(*((int*)_tmpAB1)!= 7)goto _LL6FE;
_LL6FD: {struct _tuple10 _tmp1087;return(_tmp1087.f1=1,((_tmp1087.f2=(void*)2,
_tmp1087)));}_LL6FE:;_LL6FF: {struct _tuple10 _tmp1088;return(_tmp1088.f1=(_tmpAAD->tq).real_const,((
_tmp1088.f2=(void*)2,_tmp1088)));}_LL6FB:;}_LL6F5: if(_tmpAAC <= (void*)1)goto
_LL6F7;if(*((int*)_tmpAAC)!= 3)goto _LL6F7;_tmpAAE=((struct Cyc_Absyn_Local_b_struct*)
_tmpAAC)->f1;_LL6F6: {void*_tmpAB4=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);_LL701: if(_tmpAB4 <= (void*)4)goto _LL703;if(*((int*)
_tmpAB4)!= 7)goto _LL703;_LL702: {struct _tuple10 _tmp1089;return(_tmp1089.f1=1,((
_tmp1089.f2=(void*)((struct Cyc_Core_Opt*)_check_null(_tmpAAE->rgn))->v,_tmp1089)));}
_LL703:;_LL704: _tmpAAE->escapes=1;{struct _tuple10 _tmp108A;return(_tmp108A.f1=(
_tmpAAE->tq).real_const,((_tmp108A.f2=(void*)((struct Cyc_Core_Opt*)_check_null(
_tmpAAE->rgn))->v,_tmp108A)));}_LL700:;}_LL6F7: if(_tmpAAC <= (void*)1)goto _LL6F9;
if(*((int*)_tmpAAC)!= 4)goto _LL6F9;_tmpAAF=((struct Cyc_Absyn_Pat_b_struct*)
_tmpAAC)->f1;_LL6F8: _tmpAB0=_tmpAAF;goto _LL6FA;_LL6F9: if(_tmpAAC <= (void*)1)goto
_LL6EE;if(*((int*)_tmpAAC)!= 2)goto _LL6EE;_tmpAB0=((struct Cyc_Absyn_Param_b_struct*)
_tmpAAC)->f1;_LL6FA: _tmpAB0->escapes=1;{struct _tuple10 _tmp108B;return(_tmp108B.f1=(
_tmpAB0->tq).real_const,((_tmp108B.f2=(void*)((struct Cyc_Core_Opt*)_check_null(
_tmpAB0->rgn))->v,_tmp108B)));}_LL6EE:;}_LL6E4: if(*((int*)_tmpAA2)!= 23)goto
_LL6E6;_tmpAA5=((struct Cyc_Absyn_AggrMember_e_struct*)_tmpAA2)->f1;_tmpAA6=((
struct Cyc_Absyn_AggrMember_e_struct*)_tmpAA2)->f2;_LL6E5: {void*_tmpAB8=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpAA5->topt))->v);struct Cyc_List_List*
_tmpAB9;struct Cyc_Absyn_AggrInfo _tmpABA;union Cyc_Absyn_AggrInfoU_union _tmpABB;
struct Cyc_Absyn_Aggrdecl**_tmpABC;struct Cyc_Absyn_Aggrdecl*_tmpABD;_LL706: if(
_tmpAB8 <= (void*)4)goto _LL70A;if(*((int*)_tmpAB8)!= 11)goto _LL708;_tmpAB9=((
struct Cyc_Absyn_AnonAggrType_struct*)_tmpAB8)->f2;_LL707: {struct Cyc_Absyn_Aggrfield*
_tmpABE=Cyc_Absyn_lookup_field(_tmpAB9,_tmpAA6);if(_tmpABE != 0  && _tmpABE->width
!= 0){struct _tuple10 _tmp108C;return(_tmp108C.f1=(_tmpABE->tq).real_const,((
_tmp108C.f2=(Cyc_Tcutil_addressof_props(te,_tmpAA5)).f2,_tmp108C)));}return
bogus_ans;}_LL708: if(*((int*)_tmpAB8)!= 10)goto _LL70A;_tmpABA=((struct Cyc_Absyn_AggrType_struct*)
_tmpAB8)->f1;_tmpABB=_tmpABA.aggr_info;if((((((struct Cyc_Absyn_AggrType_struct*)
_tmpAB8)->f1).aggr_info).KnownAggr).tag != 1)goto _LL70A;_tmpABC=(_tmpABB.KnownAggr).f1;
_tmpABD=*_tmpABC;_LL709: {struct Cyc_Absyn_Aggrfield*_tmpAC0=Cyc_Absyn_lookup_decl_field(
_tmpABD,_tmpAA6);if(_tmpAC0 != 0  && _tmpAC0->width != 0){struct _tuple10 _tmp108D;
return(_tmp108D.f1=(_tmpAC0->tq).real_const,((_tmp108D.f2=(Cyc_Tcutil_addressof_props(
te,_tmpAA5)).f2,_tmp108D)));}return bogus_ans;}_LL70A:;_LL70B: return bogus_ans;
_LL705:;}_LL6E6: if(*((int*)_tmpAA2)!= 24)goto _LL6E8;_tmpAA7=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmpAA2)->f1;_tmpAA8=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmpAA2)->f2;_LL6E7: {
void*_tmpAC2=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmpAA7->topt))->v);struct Cyc_Absyn_PtrInfo _tmpAC3;void*_tmpAC4;struct Cyc_Absyn_PtrAtts
_tmpAC5;void*_tmpAC6;_LL70D: if(_tmpAC2 <= (void*)4)goto _LL70F;if(*((int*)_tmpAC2)
!= 4)goto _LL70F;_tmpAC3=((struct Cyc_Absyn_PointerType_struct*)_tmpAC2)->f1;
_tmpAC4=(void*)_tmpAC3.elt_typ;_tmpAC5=_tmpAC3.ptr_atts;_tmpAC6=(void*)_tmpAC5.rgn;
_LL70E: {struct Cyc_Absyn_Aggrfield*finfo;{void*_tmpAC7=Cyc_Tcutil_compress(
_tmpAC4);struct Cyc_List_List*_tmpAC8;struct Cyc_Absyn_AggrInfo _tmpAC9;union Cyc_Absyn_AggrInfoU_union
_tmpACA;struct Cyc_Absyn_Aggrdecl**_tmpACB;struct Cyc_Absyn_Aggrdecl*_tmpACC;
_LL712: if(_tmpAC7 <= (void*)4)goto _LL716;if(*((int*)_tmpAC7)!= 11)goto _LL714;
_tmpAC8=((struct Cyc_Absyn_AnonAggrType_struct*)_tmpAC7)->f2;_LL713: finfo=Cyc_Absyn_lookup_field(
_tmpAC8,_tmpAA8);goto _LL711;_LL714: if(*((int*)_tmpAC7)!= 10)goto _LL716;_tmpAC9=((
struct Cyc_Absyn_AggrType_struct*)_tmpAC7)->f1;_tmpACA=_tmpAC9.aggr_info;if((((((
struct Cyc_Absyn_AggrType_struct*)_tmpAC7)->f1).aggr_info).KnownAggr).tag != 1)
goto _LL716;_tmpACB=(_tmpACA.KnownAggr).f1;_tmpACC=*_tmpACB;_LL715: finfo=Cyc_Absyn_lookup_decl_field(
_tmpACC,_tmpAA8);goto _LL711;_LL716:;_LL717: return bogus_ans;_LL711:;}if(finfo != 0
 && finfo->width != 0){struct _tuple10 _tmp108E;return(_tmp108E.f1=(finfo->tq).real_const,((
_tmp108E.f2=_tmpAC6,_tmp108E)));}return bogus_ans;}_LL70F:;_LL710: return bogus_ans;
_LL70C:;}_LL6E8: if(*((int*)_tmpAA2)!= 22)goto _LL6EA;_tmpAA9=((struct Cyc_Absyn_Deref_e_struct*)
_tmpAA2)->f1;_LL6E9: {void*_tmpACE=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmpAA9->topt))->v);struct Cyc_Absyn_PtrInfo _tmpACF;struct Cyc_Absyn_Tqual
_tmpAD0;struct Cyc_Absyn_PtrAtts _tmpAD1;void*_tmpAD2;_LL719: if(_tmpACE <= (void*)4)
goto _LL71B;if(*((int*)_tmpACE)!= 4)goto _LL71B;_tmpACF=((struct Cyc_Absyn_PointerType_struct*)
_tmpACE)->f1;_tmpAD0=_tmpACF.elt_tq;_tmpAD1=_tmpACF.ptr_atts;_tmpAD2=(void*)
_tmpAD1.rgn;_LL71A: {struct _tuple10 _tmp108F;return(_tmp108F.f1=_tmpAD0.real_const,((
_tmp108F.f2=_tmpAD2,_tmp108F)));}_LL71B:;_LL71C: return bogus_ans;_LL718:;}_LL6EA:
if(*((int*)_tmpAA2)!= 25)goto _LL6EC;_tmpAAA=((struct Cyc_Absyn_Subscript_e_struct*)
_tmpAA2)->f1;_tmpAAB=((struct Cyc_Absyn_Subscript_e_struct*)_tmpAA2)->f2;_LL6EB: {
void*t=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmpAAA->topt))->v);
void*_tmpAD4=t;struct Cyc_List_List*_tmpAD5;struct Cyc_Absyn_PtrInfo _tmpAD6;struct
Cyc_Absyn_Tqual _tmpAD7;struct Cyc_Absyn_PtrAtts _tmpAD8;void*_tmpAD9;struct Cyc_Absyn_ArrayInfo
_tmpADA;struct Cyc_Absyn_Tqual _tmpADB;_LL71E: if(_tmpAD4 <= (void*)4)goto _LL724;if(*((
int*)_tmpAD4)!= 9)goto _LL720;_tmpAD5=((struct Cyc_Absyn_TupleType_struct*)_tmpAD4)->f1;
_LL71F: {unsigned int _tmpADD;int _tmpADE;struct _tuple7 _tmpADC=Cyc_Evexp_eval_const_uint_exp(
_tmpAAB);_tmpADD=_tmpADC.f1;_tmpADE=_tmpADC.f2;if(!_tmpADE)return bogus_ans;{
struct _tuple5*_tmpADF=Cyc_Absyn_lookup_tuple_field(_tmpAD5,(int)_tmpADD);if(
_tmpADF != 0){struct _tuple10 _tmp1090;return(_tmp1090.f1=((*_tmpADF).f1).real_const,((
_tmp1090.f2=(Cyc_Tcutil_addressof_props(te,_tmpAAA)).f2,_tmp1090)));}return
bogus_ans;}}_LL720: if(*((int*)_tmpAD4)!= 4)goto _LL722;_tmpAD6=((struct Cyc_Absyn_PointerType_struct*)
_tmpAD4)->f1;_tmpAD7=_tmpAD6.elt_tq;_tmpAD8=_tmpAD6.ptr_atts;_tmpAD9=(void*)
_tmpAD8.rgn;_LL721: {struct _tuple10 _tmp1091;return(_tmp1091.f1=_tmpAD7.real_const,((
_tmp1091.f2=_tmpAD9,_tmp1091)));}_LL722: if(*((int*)_tmpAD4)!= 7)goto _LL724;
_tmpADA=((struct Cyc_Absyn_ArrayType_struct*)_tmpAD4)->f1;_tmpADB=_tmpADA.tq;
_LL723: {struct _tuple10 _tmp1092;return(_tmp1092.f1=_tmpADB.real_const,((_tmp1092.f2=(
Cyc_Tcutil_addressof_props(te,_tmpAAA)).f2,_tmp1092)));}_LL724:;_LL725: return
bogus_ans;_LL71D:;}_LL6EC:;_LL6ED:{const char*_tmp1095;void*_tmp1094;(_tmp1094=0,
Cyc_Tcutil_terr(e->loc,((_tmp1095="unary & applied to non-lvalue",_tag_dyneither(
_tmp1095,sizeof(char),30))),_tag_dyneither(_tmp1094,sizeof(void*),0)));}return
bogus_ans;_LL6E1:;}void*Cyc_Tcutil_array_to_ptr(struct Cyc_Tcenv_Tenv*te,void*
e_typ,struct Cyc_Absyn_Exp*e);void*Cyc_Tcutil_array_to_ptr(struct Cyc_Tcenv_Tenv*
te,void*e_typ,struct Cyc_Absyn_Exp*e){void*_tmpAE6=Cyc_Tcutil_compress(e_typ);
struct Cyc_Absyn_ArrayInfo _tmpAE7;void*_tmpAE8;struct Cyc_Absyn_Tqual _tmpAE9;
struct Cyc_Absyn_Conref*_tmpAEA;_LL727: if(_tmpAE6 <= (void*)4)goto _LL729;if(*((int*)
_tmpAE6)!= 7)goto _LL729;_tmpAE7=((struct Cyc_Absyn_ArrayType_struct*)_tmpAE6)->f1;
_tmpAE8=(void*)_tmpAE7.elt_type;_tmpAE9=_tmpAE7.tq;_tmpAEA=_tmpAE7.zero_term;
_LL728: {void*_tmpAEC;struct _tuple10 _tmpAEB=Cyc_Tcutil_addressof_props(te,e);
_tmpAEC=_tmpAEB.f2;{struct Cyc_Absyn_Upper_b_struct _tmp1098;struct Cyc_Absyn_Upper_b_struct*
_tmp1097;return Cyc_Absyn_atb_typ(_tmpAE8,_tmpAEC,_tmpAE9,(void*)((_tmp1097=
_cycalloc(sizeof(*_tmp1097)),((_tmp1097[0]=((_tmp1098.tag=0,((_tmp1098.f1=e,
_tmp1098)))),_tmp1097)))),_tmpAEA);}}_LL729:;_LL72A: return e_typ;_LL726:;}void Cyc_Tcutil_check_bound(
struct Cyc_Position_Segment*loc,unsigned int i,struct Cyc_Absyn_Conref*b);void Cyc_Tcutil_check_bound(
struct Cyc_Position_Segment*loc,unsigned int i,struct Cyc_Absyn_Conref*b){b=Cyc_Absyn_compress_conref(
b);{union Cyc_Absyn_Constraint_union _tmpAEF=b->v;void*_tmpAF0;void*_tmpAF1;struct
Cyc_Absyn_Exp*_tmpAF2;_LL72C: if((_tmpAEF.Eq_constr).tag != 0)goto _LL72E;_tmpAF0=(
_tmpAEF.Eq_constr).f1;if((int)_tmpAF0 != 0)goto _LL72E;_LL72D: return;_LL72E: if((
_tmpAEF.Eq_constr).tag != 0)goto _LL730;_tmpAF1=(_tmpAEF.Eq_constr).f1;if(_tmpAF1
<= (void*)1)goto _LL730;if(*((int*)_tmpAF1)!= 0)goto _LL730;_tmpAF2=((struct Cyc_Absyn_Upper_b_struct*)
_tmpAF1)->f1;_LL72F: {unsigned int _tmpAF4;int _tmpAF5;struct _tuple7 _tmpAF3=Cyc_Evexp_eval_const_uint_exp(
_tmpAF2);_tmpAF4=_tmpAF3.f1;_tmpAF5=_tmpAF3.f2;if(_tmpAF5  && _tmpAF4 <= i){const
char*_tmp109D;void*_tmp109C[2];struct Cyc_Int_pa_struct _tmp109B;struct Cyc_Int_pa_struct
_tmp109A;(_tmp109A.tag=1,((_tmp109A.f1=(unsigned long)((int)i),((_tmp109B.tag=1,((
_tmp109B.f1=(unsigned long)((int)_tmpAF4),((_tmp109C[0]=& _tmp109B,((_tmp109C[1]=&
_tmp109A,Cyc_Tcutil_terr(loc,((_tmp109D="dereference is out of bounds: %d <= %d",
_tag_dyneither(_tmp109D,sizeof(char),39))),_tag_dyneither(_tmp109C,sizeof(void*),
2)))))))))))));}return;}_LL730: if((_tmpAEF.No_constr).tag != 2)goto _LL732;_LL731:{
struct Cyc_Absyn_Upper_b_struct*_tmp10A3;struct Cyc_Absyn_Upper_b_struct _tmp10A2;
union Cyc_Absyn_Constraint_union _tmp10A1;b->v=(union Cyc_Absyn_Constraint_union)(((
_tmp10A1.Eq_constr).tag=0,(((_tmp10A1.Eq_constr).f1=(void*)((void*)((void*)((
_tmp10A3=_cycalloc(sizeof(*_tmp10A3)),((_tmp10A3[0]=((_tmp10A2.tag=0,((_tmp10A2.f1=
Cyc_Absyn_uint_exp(i + 1,0),_tmp10A2)))),_tmp10A3)))))),_tmp10A1))));}return;
_LL732:;_LL733: {const char*_tmp10A6;void*_tmp10A5;(_tmp10A5=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp10A6="check_bound -- bad compressed conref",
_tag_dyneither(_tmp10A6,sizeof(char),37))),_tag_dyneither(_tmp10A5,sizeof(void*),
0)));}_LL72B:;}}void Cyc_Tcutil_check_nonzero_bound(struct Cyc_Position_Segment*
loc,struct Cyc_Absyn_Conref*b);void Cyc_Tcutil_check_nonzero_bound(struct Cyc_Position_Segment*
loc,struct Cyc_Absyn_Conref*b){Cyc_Tcutil_check_bound(loc,0,b);}int Cyc_Tcutil_is_bound_one(
struct Cyc_Absyn_Conref*b);int Cyc_Tcutil_is_bound_one(struct Cyc_Absyn_Conref*b){
union Cyc_Absyn_Constraint_union _tmpAFF=(Cyc_Absyn_compress_conref(b))->v;void*
_tmpB00;struct Cyc_Absyn_Exp*_tmpB01;_LL735: if((_tmpAFF.Eq_constr).tag != 0)goto
_LL737;_tmpB00=(_tmpAFF.Eq_constr).f1;if(_tmpB00 <= (void*)1)goto _LL737;if(*((int*)
_tmpB00)!= 0)goto _LL737;_tmpB01=((struct Cyc_Absyn_Upper_b_struct*)_tmpB00)->f1;
_LL736: {unsigned int _tmpB03;int _tmpB04;struct _tuple7 _tmpB02=Cyc_Evexp_eval_const_uint_exp(
_tmpB01);_tmpB03=_tmpB02.f1;_tmpB04=_tmpB02.f2;return _tmpB04  && _tmpB03 == 1;}
_LL737:;_LL738: return 0;_LL734:;}int Cyc_Tcutil_bits_only(void*t);int Cyc_Tcutil_bits_only(
void*t){void*_tmpB05=Cyc_Tcutil_compress(t);struct Cyc_Absyn_ArrayInfo _tmpB06;
void*_tmpB07;struct Cyc_Absyn_Conref*_tmpB08;struct Cyc_List_List*_tmpB09;struct
Cyc_Absyn_AggrInfo _tmpB0A;union Cyc_Absyn_AggrInfoU_union _tmpB0B;struct Cyc_Absyn_AggrInfo
_tmpB0C;union Cyc_Absyn_AggrInfoU_union _tmpB0D;struct Cyc_Absyn_Aggrdecl**_tmpB0E;
struct Cyc_Absyn_Aggrdecl*_tmpB0F;struct Cyc_List_List*_tmpB10;struct Cyc_List_List*
_tmpB11;_LL73A: if((int)_tmpB05 != 0)goto _LL73C;_LL73B: goto _LL73D;_LL73C: if(
_tmpB05 <= (void*)4)goto _LL73E;if(*((int*)_tmpB05)!= 5)goto _LL73E;_LL73D: goto
_LL73F;_LL73E: if((int)_tmpB05 != 1)goto _LL740;_LL73F: goto _LL741;_LL740: if(_tmpB05
<= (void*)4)goto _LL750;if(*((int*)_tmpB05)!= 6)goto _LL742;_LL741: return 1;_LL742:
if(*((int*)_tmpB05)!= 12)goto _LL744;_LL743: goto _LL745;_LL744: if(*((int*)_tmpB05)
!= 13)goto _LL746;_LL745: return 0;_LL746: if(*((int*)_tmpB05)!= 7)goto _LL748;
_tmpB06=((struct Cyc_Absyn_ArrayType_struct*)_tmpB05)->f1;_tmpB07=(void*)_tmpB06.elt_type;
_tmpB08=_tmpB06.zero_term;_LL747: return !((int(*)(int,struct Cyc_Absyn_Conref*x))
Cyc_Absyn_conref_def)(0,_tmpB08) && Cyc_Tcutil_bits_only(_tmpB07);_LL748: if(*((
int*)_tmpB05)!= 9)goto _LL74A;_tmpB09=((struct Cyc_Absyn_TupleType_struct*)_tmpB05)->f1;
_LL749: for(0;_tmpB09 != 0;_tmpB09=_tmpB09->tl){if(!Cyc_Tcutil_bits_only((*((
struct _tuple5*)_tmpB09->hd)).f2))return 0;}return 1;_LL74A: if(*((int*)_tmpB05)!= 
10)goto _LL74C;_tmpB0A=((struct Cyc_Absyn_AggrType_struct*)_tmpB05)->f1;_tmpB0B=
_tmpB0A.aggr_info;if((((((struct Cyc_Absyn_AggrType_struct*)_tmpB05)->f1).aggr_info).UnknownAggr).tag
!= 0)goto _LL74C;_LL74B: return 0;_LL74C: if(*((int*)_tmpB05)!= 10)goto _LL74E;
_tmpB0C=((struct Cyc_Absyn_AggrType_struct*)_tmpB05)->f1;_tmpB0D=_tmpB0C.aggr_info;
if((((((struct Cyc_Absyn_AggrType_struct*)_tmpB05)->f1).aggr_info).KnownAggr).tag
!= 1)goto _LL74E;_tmpB0E=(_tmpB0D.KnownAggr).f1;_tmpB0F=*_tmpB0E;_tmpB10=_tmpB0C.targs;
_LL74D: if(_tmpB0F->impl == 0)return 0;{struct _RegionHandle _tmpB12=_new_region("rgn");
struct _RegionHandle*rgn=& _tmpB12;_push_region(rgn);{struct Cyc_List_List*_tmpB13=((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmpB0F->tvs,_tmpB10);{struct Cyc_List_List*
fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpB0F->impl))->fields;for(0;fs
!= 0;fs=fs->tl){if(!Cyc_Tcutil_bits_only(Cyc_Tcutil_rsubstitute(rgn,_tmpB13,(
void*)((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))){int _tmpB14=0;_npop_handler(0);
return _tmpB14;}}}{int _tmpB15=1;_npop_handler(0);return _tmpB15;}};_pop_region(rgn);}
_LL74E: if(*((int*)_tmpB05)!= 11)goto _LL750;_tmpB11=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmpB05)->f2;_LL74F: for(0;_tmpB11 != 0;_tmpB11=_tmpB11->tl){if(!Cyc_Tcutil_bits_only((
void*)((struct Cyc_Absyn_Aggrfield*)_tmpB11->hd)->type))return 0;}return 1;_LL750:;
_LL751: return 0;_LL739:;}struct _tuple23{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*
f2;};static int Cyc_Tcutil_cnst_exp(struct Cyc_Tcenv_Tenv*te,int var_okay,struct Cyc_Absyn_Exp*
e);static int Cyc_Tcutil_cnst_exp(struct Cyc_Tcenv_Tenv*te,int var_okay,struct Cyc_Absyn_Exp*
e){void*_tmpB16=(void*)e->r;struct _tuple2*_tmpB17;void*_tmpB18;struct Cyc_Absyn_Exp*
_tmpB19;struct Cyc_Absyn_Exp*_tmpB1A;struct Cyc_Absyn_Exp*_tmpB1B;struct Cyc_Absyn_Exp*
_tmpB1C;struct Cyc_Absyn_Exp*_tmpB1D;struct Cyc_Absyn_Exp*_tmpB1E;struct Cyc_Absyn_Exp*
_tmpB1F;void*_tmpB20;struct Cyc_Absyn_Exp*_tmpB21;void*_tmpB22;void*_tmpB23;
struct Cyc_Absyn_Exp*_tmpB24;struct Cyc_Absyn_Exp*_tmpB25;struct Cyc_Absyn_Exp*
_tmpB26;struct Cyc_Absyn_Exp*_tmpB27;struct Cyc_List_List*_tmpB28;struct Cyc_List_List*
_tmpB29;struct Cyc_List_List*_tmpB2A;void*_tmpB2B;struct Cyc_List_List*_tmpB2C;
struct Cyc_List_List*_tmpB2D;struct Cyc_List_List*_tmpB2E;_LL753: if(*((int*)
_tmpB16)!= 0)goto _LL755;_LL754: goto _LL756;_LL755: if(*((int*)_tmpB16)!= 18)goto
_LL757;_LL756: goto _LL758;_LL757: if(*((int*)_tmpB16)!= 19)goto _LL759;_LL758: goto
_LL75A;_LL759: if(*((int*)_tmpB16)!= 20)goto _LL75B;_LL75A: goto _LL75C;_LL75B: if(*((
int*)_tmpB16)!= 21)goto _LL75D;_LL75C: goto _LL75E;_LL75D: if(*((int*)_tmpB16)!= 33)
goto _LL75F;_LL75E: goto _LL760;_LL75F: if(*((int*)_tmpB16)!= 34)goto _LL761;_LL760:
return 1;_LL761: if(*((int*)_tmpB16)!= 1)goto _LL763;_tmpB17=((struct Cyc_Absyn_Var_e_struct*)
_tmpB16)->f1;_tmpB18=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmpB16)->f2;_LL762: {
void*_tmpB2F=_tmpB18;struct Cyc_Absyn_Vardecl*_tmpB30;_LL782: if(_tmpB2F <= (void*)
1)goto _LL786;if(*((int*)_tmpB2F)!= 1)goto _LL784;_LL783: return 1;_LL784: if(*((int*)
_tmpB2F)!= 0)goto _LL786;_tmpB30=((struct Cyc_Absyn_Global_b_struct*)_tmpB2F)->f1;
_LL785: {void*_tmpB31=Cyc_Tcutil_compress((void*)_tmpB30->type);_LL78B: if(
_tmpB31 <= (void*)4)goto _LL78F;if(*((int*)_tmpB31)!= 7)goto _LL78D;_LL78C: goto
_LL78E;_LL78D: if(*((int*)_tmpB31)!= 8)goto _LL78F;_LL78E: return 1;_LL78F:;_LL790:
return var_okay;_LL78A:;}_LL786: if((int)_tmpB2F != 0)goto _LL788;_LL787: return 0;
_LL788:;_LL789: return var_okay;_LL781:;}_LL763: if(*((int*)_tmpB16)!= 6)goto _LL765;
_tmpB19=((struct Cyc_Absyn_Conditional_e_struct*)_tmpB16)->f1;_tmpB1A=((struct Cyc_Absyn_Conditional_e_struct*)
_tmpB16)->f2;_tmpB1B=((struct Cyc_Absyn_Conditional_e_struct*)_tmpB16)->f3;_LL764:
return(Cyc_Tcutil_cnst_exp(te,0,_tmpB19) && Cyc_Tcutil_cnst_exp(te,0,_tmpB1A))
 && Cyc_Tcutil_cnst_exp(te,0,_tmpB1B);_LL765: if(*((int*)_tmpB16)!= 9)goto _LL767;
_tmpB1C=((struct Cyc_Absyn_SeqExp_e_struct*)_tmpB16)->f1;_tmpB1D=((struct Cyc_Absyn_SeqExp_e_struct*)
_tmpB16)->f2;_LL766: return Cyc_Tcutil_cnst_exp(te,0,_tmpB1C) && Cyc_Tcutil_cnst_exp(
te,0,_tmpB1D);_LL767: if(*((int*)_tmpB16)!= 13)goto _LL769;_tmpB1E=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmpB16)->f1;_LL768: _tmpB1F=_tmpB1E;goto _LL76A;_LL769: if(*((int*)_tmpB16)!= 14)
goto _LL76B;_tmpB1F=((struct Cyc_Absyn_Instantiate_e_struct*)_tmpB16)->f1;_LL76A:
return Cyc_Tcutil_cnst_exp(te,var_okay,_tmpB1F);_LL76B: if(*((int*)_tmpB16)!= 15)
goto _LL76D;_tmpB20=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmpB16)->f1;_tmpB21=((
struct Cyc_Absyn_Cast_e_struct*)_tmpB16)->f2;_tmpB22=(void*)((struct Cyc_Absyn_Cast_e_struct*)
_tmpB16)->f4;if((int)_tmpB22 != 1)goto _LL76D;_LL76C: return Cyc_Tcutil_cnst_exp(te,
var_okay,_tmpB21);_LL76D: if(*((int*)_tmpB16)!= 15)goto _LL76F;_tmpB23=(void*)((
struct Cyc_Absyn_Cast_e_struct*)_tmpB16)->f1;_tmpB24=((struct Cyc_Absyn_Cast_e_struct*)
_tmpB16)->f2;_LL76E: return Cyc_Tcutil_cnst_exp(te,var_okay,_tmpB24);_LL76F: if(*((
int*)_tmpB16)!= 16)goto _LL771;_tmpB25=((struct Cyc_Absyn_Address_e_struct*)
_tmpB16)->f1;_LL770: return Cyc_Tcutil_cnst_exp(te,1,_tmpB25);_LL771: if(*((int*)
_tmpB16)!= 29)goto _LL773;_tmpB26=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmpB16)->f2;_tmpB27=((struct Cyc_Absyn_Comprehension_e_struct*)_tmpB16)->f3;
_LL772: return Cyc_Tcutil_cnst_exp(te,0,_tmpB26) && Cyc_Tcutil_cnst_exp(te,0,
_tmpB27);_LL773: if(*((int*)_tmpB16)!= 28)goto _LL775;_tmpB28=((struct Cyc_Absyn_Array_e_struct*)
_tmpB16)->f1;_LL774: _tmpB29=_tmpB28;goto _LL776;_LL775: if(*((int*)_tmpB16)!= 31)
goto _LL777;_tmpB29=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmpB16)->f2;_LL776:
_tmpB2A=_tmpB29;goto _LL778;_LL777: if(*((int*)_tmpB16)!= 30)goto _LL779;_tmpB2A=((
struct Cyc_Absyn_Struct_e_struct*)_tmpB16)->f3;_LL778: for(0;_tmpB2A != 0;_tmpB2A=
_tmpB2A->tl){if(!Cyc_Tcutil_cnst_exp(te,0,(*((struct _tuple23*)_tmpB2A->hd)).f2))
return 0;}return 1;_LL779: if(*((int*)_tmpB16)!= 3)goto _LL77B;_tmpB2B=(void*)((
struct Cyc_Absyn_Primop_e_struct*)_tmpB16)->f1;_tmpB2C=((struct Cyc_Absyn_Primop_e_struct*)
_tmpB16)->f2;_LL77A: _tmpB2D=_tmpB2C;goto _LL77C;_LL77B: if(*((int*)_tmpB16)!= 26)
goto _LL77D;_tmpB2D=((struct Cyc_Absyn_Tuple_e_struct*)_tmpB16)->f1;_LL77C: _tmpB2E=
_tmpB2D;goto _LL77E;_LL77D: if(*((int*)_tmpB16)!= 32)goto _LL77F;_tmpB2E=((struct
Cyc_Absyn_Tunion_e_struct*)_tmpB16)->f1;_LL77E: for(0;_tmpB2E != 0;_tmpB2E=_tmpB2E->tl){
if(!Cyc_Tcutil_cnst_exp(te,0,(struct Cyc_Absyn_Exp*)_tmpB2E->hd))return 0;}return 1;
_LL77F:;_LL780: return 0;_LL752:;}int Cyc_Tcutil_is_const_exp(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e);int Cyc_Tcutil_is_const_exp(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Absyn_Exp*e){return Cyc_Tcutil_cnst_exp(te,0,e);}static int Cyc_Tcutil_fields_support_default(
struct Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs);int Cyc_Tcutil_supports_default(
void*t);int Cyc_Tcutil_supports_default(void*t){void*_tmpB32=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_PtrInfo _tmpB33;struct Cyc_Absyn_PtrAtts _tmpB34;struct Cyc_Absyn_Conref*
_tmpB35;struct Cyc_Absyn_Conref*_tmpB36;struct Cyc_Absyn_ArrayInfo _tmpB37;void*
_tmpB38;struct Cyc_List_List*_tmpB39;struct Cyc_Absyn_AggrInfo _tmpB3A;union Cyc_Absyn_AggrInfoU_union
_tmpB3B;struct Cyc_List_List*_tmpB3C;struct Cyc_List_List*_tmpB3D;_LL792: if((int)
_tmpB32 != 0)goto _LL794;_LL793: goto _LL795;_LL794: if(_tmpB32 <= (void*)4)goto _LL796;
if(*((int*)_tmpB32)!= 5)goto _LL796;_LL795: goto _LL797;_LL796: if((int)_tmpB32 != 1)
goto _LL798;_LL797: goto _LL799;_LL798: if(_tmpB32 <= (void*)4)goto _LL7A8;if(*((int*)
_tmpB32)!= 6)goto _LL79A;_LL799: return 1;_LL79A: if(*((int*)_tmpB32)!= 4)goto _LL79C;
_tmpB33=((struct Cyc_Absyn_PointerType_struct*)_tmpB32)->f1;_tmpB34=_tmpB33.ptr_atts;
_tmpB35=_tmpB34.nullable;_tmpB36=_tmpB34.bounds;_LL79B: {void*_tmpB3E=Cyc_Absyn_conref_def((
void*)((void*)0),_tmpB36);_LL7AB: if((int)_tmpB3E != 0)goto _LL7AD;_LL7AC: return 1;
_LL7AD:;_LL7AE: return((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
1,_tmpB35);_LL7AA:;}_LL79C: if(*((int*)_tmpB32)!= 7)goto _LL79E;_tmpB37=((struct
Cyc_Absyn_ArrayType_struct*)_tmpB32)->f1;_tmpB38=(void*)_tmpB37.elt_type;_LL79D:
return Cyc_Tcutil_supports_default(_tmpB38);_LL79E: if(*((int*)_tmpB32)!= 9)goto
_LL7A0;_tmpB39=((struct Cyc_Absyn_TupleType_struct*)_tmpB32)->f1;_LL79F: for(0;
_tmpB39 != 0;_tmpB39=_tmpB39->tl){if(!Cyc_Tcutil_supports_default((*((struct
_tuple5*)_tmpB39->hd)).f2))return 0;}return 1;_LL7A0: if(*((int*)_tmpB32)!= 10)goto
_LL7A2;_tmpB3A=((struct Cyc_Absyn_AggrType_struct*)_tmpB32)->f1;_tmpB3B=_tmpB3A.aggr_info;
_tmpB3C=_tmpB3A.targs;_LL7A1: {struct Cyc_Absyn_Aggrdecl*_tmpB3F=Cyc_Absyn_get_known_aggrdecl(
_tmpB3B);if(_tmpB3F->impl == 0)return 0;if(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmpB3F->impl))->exist_vars != 0)return 0;return Cyc_Tcutil_fields_support_default(
_tmpB3F->tvs,_tmpB3C,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpB3F->impl))->fields);}
_LL7A2: if(*((int*)_tmpB32)!= 11)goto _LL7A4;_tmpB3D=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmpB32)->f2;_LL7A3: return Cyc_Tcutil_fields_support_default(0,0,_tmpB3D);_LL7A4:
if(*((int*)_tmpB32)!= 13)goto _LL7A6;_LL7A5: goto _LL7A7;_LL7A6: if(*((int*)_tmpB32)
!= 12)goto _LL7A8;_LL7A7: return 1;_LL7A8:;_LL7A9: return 0;_LL791:;}static int Cyc_Tcutil_fields_support_default(
struct Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs);static int
Cyc_Tcutil_fields_support_default(struct Cyc_List_List*tvs,struct Cyc_List_List*ts,
struct Cyc_List_List*fs){{struct _RegionHandle _tmpB40=_new_region("rgn");struct
_RegionHandle*rgn=& _tmpB40;_push_region(rgn);{struct Cyc_List_List*_tmpB41=((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,tvs,ts);for(0;fs != 0;fs=fs->tl){
void*t=Cyc_Tcutil_rsubstitute(rgn,_tmpB41,(void*)((struct Cyc_Absyn_Aggrfield*)fs->hd)->type);
if(!Cyc_Tcutil_supports_default(t)){int _tmpB42=0;_npop_handler(0);return _tmpB42;}}};
_pop_region(rgn);}return 1;}int Cyc_Tcutil_admits_zero(void*t);int Cyc_Tcutil_admits_zero(
void*t){void*_tmpB43=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmpB44;
struct Cyc_Absyn_PtrAtts _tmpB45;struct Cyc_Absyn_Conref*_tmpB46;struct Cyc_Absyn_Conref*
_tmpB47;_LL7B0: if(_tmpB43 <= (void*)4)goto _LL7B2;if(*((int*)_tmpB43)!= 5)goto
_LL7B2;_LL7B1: goto _LL7B3;_LL7B2: if((int)_tmpB43 != 1)goto _LL7B4;_LL7B3: goto _LL7B5;
_LL7B4: if(_tmpB43 <= (void*)4)goto _LL7B8;if(*((int*)_tmpB43)!= 6)goto _LL7B6;
_LL7B5: return 1;_LL7B6: if(*((int*)_tmpB43)!= 4)goto _LL7B8;_tmpB44=((struct Cyc_Absyn_PointerType_struct*)
_tmpB43)->f1;_tmpB45=_tmpB44.ptr_atts;_tmpB46=_tmpB45.nullable;_tmpB47=_tmpB45.bounds;
_LL7B7: {union Cyc_Absyn_Constraint_union _tmpB48=(Cyc_Absyn_compress_conref(
_tmpB47))->v;void*_tmpB49;_LL7BB: if((_tmpB48.Eq_constr).tag != 0)goto _LL7BD;
_tmpB49=(_tmpB48.Eq_constr).f1;if((int)_tmpB49 != 0)goto _LL7BD;_LL7BC: return 0;
_LL7BD: if((_tmpB48.Eq_constr).tag != 0)goto _LL7BF;_LL7BE: {union Cyc_Absyn_Constraint_union
_tmpB4A=(((struct Cyc_Absyn_Conref*(*)(struct Cyc_Absyn_Conref*x))Cyc_Absyn_compress_conref)(
_tmpB46))->v;int _tmpB4B;_LL7C2: if((_tmpB4A.Eq_constr).tag != 0)goto _LL7C4;_tmpB4B=(
_tmpB4A.Eq_constr).f1;_LL7C3: return _tmpB4B;_LL7C4:;_LL7C5: return 0;_LL7C1:;}
_LL7BF:;_LL7C0: return 0;_LL7BA:;}_LL7B8:;_LL7B9: return 0;_LL7AF:;}int Cyc_Tcutil_is_noreturn(
void*t);int Cyc_Tcutil_is_noreturn(void*t){{void*_tmpB4C=Cyc_Tcutil_compress(t);
struct Cyc_Absyn_PtrInfo _tmpB4D;void*_tmpB4E;struct Cyc_Absyn_FnInfo _tmpB4F;struct
Cyc_List_List*_tmpB50;_LL7C7: if(_tmpB4C <= (void*)4)goto _LL7CB;if(*((int*)_tmpB4C)
!= 4)goto _LL7C9;_tmpB4D=((struct Cyc_Absyn_PointerType_struct*)_tmpB4C)->f1;
_tmpB4E=(void*)_tmpB4D.elt_typ;_LL7C8: return Cyc_Tcutil_is_noreturn(_tmpB4E);
_LL7C9: if(*((int*)_tmpB4C)!= 8)goto _LL7CB;_tmpB4F=((struct Cyc_Absyn_FnType_struct*)
_tmpB4C)->f1;_tmpB50=_tmpB4F.attributes;_LL7CA: for(0;_tmpB50 != 0;_tmpB50=_tmpB50->tl){
void*_tmpB51=(void*)_tmpB50->hd;_LL7CE: if((int)_tmpB51 != 3)goto _LL7D0;_LL7CF:
return 1;_LL7D0:;_LL7D1: continue;_LL7CD:;}goto _LL7C6;_LL7CB:;_LL7CC: goto _LL7C6;
_LL7C6:;}return 0;}struct Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(void*t,
struct Cyc_List_List*atts);struct Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(
void*t,struct Cyc_List_List*atts){void*_tmpB52=Cyc_Tcutil_compress(t);struct Cyc_Absyn_FnInfo
_tmpB53;struct Cyc_List_List*_tmpB54;struct Cyc_List_List**_tmpB55;_LL7D3: if(
_tmpB52 <= (void*)4)goto _LL7D5;if(*((int*)_tmpB52)!= 8)goto _LL7D5;_tmpB53=((
struct Cyc_Absyn_FnType_struct*)_tmpB52)->f1;_tmpB54=_tmpB53.attributes;_tmpB55=(
struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_struct*)_tmpB52)->f1).attributes;
_LL7D4: {struct Cyc_List_List*_tmpB56=0;for(0;atts != 0;atts=atts->tl){if(Cyc_Absyn_fntype_att((
void*)atts->hd)){struct Cyc_List_List*_tmp10A7;*_tmpB55=((_tmp10A7=_cycalloc(
sizeof(*_tmp10A7)),((_tmp10A7->hd=(void*)((void*)atts->hd),((_tmp10A7->tl=*
_tmpB55,_tmp10A7))))));}else{struct Cyc_List_List*_tmp10A8;_tmpB56=((_tmp10A8=
_cycalloc(sizeof(*_tmp10A8)),((_tmp10A8->hd=(void*)((void*)atts->hd),((_tmp10A8->tl=
_tmpB56,_tmp10A8))))));}}return _tmpB56;}_LL7D5:;_LL7D6: {const char*_tmp10AB;void*
_tmp10AA;(_tmp10AA=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmp10AB="transfer_fn_type_atts",_tag_dyneither(_tmp10AB,
sizeof(char),22))),_tag_dyneither(_tmp10AA,sizeof(void*),0)));}_LL7D2:;}
