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
f1;};struct Cyc_Absyn_Region_d_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*
f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Alias_d_struct{int tag;struct
Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Tvar*f2;struct Cyc_Absyn_Vardecl*f3;};struct Cyc_Absyn_Aggr_d_struct{
int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Tunion_d_struct{int tag;
struct Cyc_Absyn_Tuniondecl*f1;};struct Cyc_Absyn_Enum_d_struct{int tag;struct Cyc_Absyn_Enumdecl*
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
le;int allow_valueof;};struct _RegionHandle*Cyc_Tcenv_get_fnrgn(struct Cyc_Tcenv_Tenv*);
void*Cyc_Tcenv_lookup_ordinary(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,struct
Cyc_Position_Segment*,struct _tuple2*);struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl(
struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,struct _tuple2*);struct Cyc_Absyn_Tuniondecl**
Cyc_Tcenv_lookup_tuniondecl(struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,
struct _tuple2*);struct Cyc_Absyn_Enumdecl**Cyc_Tcenv_lookup_enumdecl(struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*,struct _tuple2*);struct Cyc_Absyn_Typedefdecl*Cyc_Tcenv_lookup_typedefdecl(
struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,struct _tuple2*);struct Cyc_Tcenv_Tenv*
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
void*k);void*Cyc_Tcutil_kind_to_bound(void*k);struct _tuple8{struct Cyc_Absyn_Tvar*
f1;void*f2;};struct _tuple8 Cyc_Tcutil_swap_kind(void*t,void*kb);int Cyc_Tcutil_zero_to_null(
struct Cyc_Tcenv_Tenv*,void*t,struct Cyc_Absyn_Exp*e);void*Cyc_Tcutil_max_arithmetic_type(
void*,void*);void Cyc_Tcutil_explain_failure();int Cyc_Tcutil_unify(void*,void*);
int Cyc_Tcutil_typecmp(void*,void*);void*Cyc_Tcutil_substitute(struct Cyc_List_List*,
void*);void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,
void*);int Cyc_Tcutil_subset_effect(int may_constrain_evars,void*e1,void*e2);int
Cyc_Tcutil_region_in_effect(int constrain,void*r,void*e);void*Cyc_Tcutil_fndecl2typ(
struct Cyc_Absyn_Fndecl*);struct _tuple8*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*,
struct Cyc_Absyn_Tvar*);struct _tuple9{struct Cyc_List_List*f1;struct _RegionHandle*
f2;};struct _tuple8*Cyc_Tcutil_r_make_inst_var(struct _tuple9*,struct Cyc_Absyn_Tvar*);
void Cyc_Tcutil_check_bitfield(struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*
te,void*field_typ,struct Cyc_Absyn_Exp*width,struct _dyneither_ptr*fn);void Cyc_Tcutil_check_contains_assign(
struct Cyc_Absyn_Exp*);void Cyc_Tcutil_check_valid_toplevel_type(struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Tenv*,void*);void Cyc_Tcutil_check_fndecl_valid_type(struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Fndecl*);void Cyc_Tcutil_check_type(struct
Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*bound_tvars,void*
k,int allow_evars,void*);void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*vs,
struct Cyc_Position_Segment*loc,struct _dyneither_ptr err_msg);void Cyc_Tcutil_check_unique_tvars(
struct Cyc_Position_Segment*,struct Cyc_List_List*);void Cyc_Tcutil_check_nonzero_bound(
struct Cyc_Position_Segment*,struct Cyc_Absyn_Conref*);void Cyc_Tcutil_check_bound(
struct Cyc_Position_Segment*,unsigned int i,struct Cyc_Absyn_Conref*);int Cyc_Tcutil_is_bound_one(
struct Cyc_Absyn_Conref*b);int Cyc_Tcutil_equal_tqual(struct Cyc_Absyn_Tqual tq1,
struct Cyc_Absyn_Tqual tq2);struct Cyc_List_List*Cyc_Tcutil_resolve_struct_designators(
struct _RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,
struct Cyc_List_List*fields);int Cyc_Tcutil_is_tagged_pointer_typ(void*);int Cyc_Tcutil_is_tagged_pointer_typ_elt(
void*t,void**elt_typ_dest);int Cyc_Tcutil_is_zero_pointer_typ_elt(void*t,void**
elt_typ_dest);int Cyc_Tcutil_is_zero_ptr_type(void*t,void**ptr_type,int*
is_dyneither,void**elt_type);int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*
e1,void**ptr_type,int*is_dyneither,void**elt_type);int Cyc_Tcutil_is_noalias_pointer(
void*t);int Cyc_Tcutil_is_noalias_path(struct _RegionHandle*,struct Cyc_Absyn_Exp*e);
int Cyc_Tcutil_is_noalias_pointer_or_aggr(struct _RegionHandle*,void*t);void*Cyc_Tcutil_array_to_ptr(
struct Cyc_Tcenv_Tenv*,void*t,struct Cyc_Absyn_Exp*e);struct _tuple10{int f1;void*f2;
};struct _tuple10 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*
e);void*Cyc_Tcutil_normalize_effect(void*e);struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(
void*k);int Cyc_Tcutil_new_tvar_id();void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*);
void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*);int Cyc_Tcutil_is_temp_tvar(
struct Cyc_Absyn_Tvar*);void Cyc_Tcutil_rewrite_temp_tvar(struct Cyc_Absyn_Tvar*);
int Cyc_Tcutil_same_atts(struct Cyc_List_List*,struct Cyc_List_List*);int Cyc_Tcutil_bits_only(
void*t);int Cyc_Tcutil_is_const_exp(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e);
void*Cyc_Tcutil_snd_tqt(struct _tuple5*);int Cyc_Tcutil_supports_default(void*);
int Cyc_Tcutil_admits_zero(void*t);int Cyc_Tcutil_is_noreturn(void*);int Cyc_Tcutil_extract_const_from_typedef(
struct Cyc_Position_Segment*,int declared_const,void*);struct Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(
void*t,struct Cyc_List_List*atts);int Cyc_Tcutil_rgn_of_pointer(void*t,void**rgn);
void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);struct
Cyc_Tcexp_TestEnv{struct _tuple0*eq;int isTrue;};struct Cyc_Tcexp_TestEnv Cyc_Tcexp_tcTest(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr msg_part);void
Cyc_Tc_tcAggrdecl(struct Cyc_Tcenv_Tenv*,struct Cyc_Tcenv_Genv*,struct Cyc_Position_Segment*,
struct Cyc_Absyn_Aggrdecl*);void Cyc_Tc_tcTuniondecl(struct Cyc_Tcenv_Tenv*,struct
Cyc_Tcenv_Genv*,struct Cyc_Position_Segment*,struct Cyc_Absyn_Tuniondecl*);void Cyc_Tc_tcEnumdecl(
struct Cyc_Tcenv_Tenv*,struct Cyc_Tcenv_Genv*,struct Cyc_Position_Segment*,struct
Cyc_Absyn_Enumdecl*);extern int Cyc_Cyclone_tovc_r;char Cyc_Tcutil_Unify[10]="\000\000\000\000Unify\000";
void Cyc_Tcutil_unify_it(void*t1,void*t2);void*Cyc_Tcutil_t1_failure=(void*)0;int
Cyc_Tcutil_tq1_const=0;void*Cyc_Tcutil_t2_failure=(void*)0;int Cyc_Tcutil_tq2_const=
0;struct _dyneither_ptr Cyc_Tcutil_failure_reason=(struct _dyneither_ptr){(void*)0,(
void*)0,(void*)(0 + 0)};void Cyc_Tcutil_explain_failure();void Cyc_Tcutil_explain_failure(){
if(Cyc_Position_num_errors >= Cyc_Position_max_errors)return;Cyc_fflush((struct
Cyc___cycFILE*)Cyc_stderr);{const char*_tmpB61;const char*_tmpB60;const char*
_tmpB5F;void*_tmpB5E[2];struct Cyc_String_pa_struct _tmpB5D;struct Cyc_String_pa_struct
_tmpB5C;struct _dyneither_ptr s1=(struct _dyneither_ptr)((_tmpB5C.tag=0,((_tmpB5C.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(Cyc_Tcutil_t1_failure)),((
_tmpB5D.tag=0,((_tmpB5D.f1=(struct _dyneither_ptr)(Cyc_Tcutil_tq1_const?(_tmpB60="const ",
_tag_dyneither(_tmpB60,sizeof(char),7)):((_tmpB61="",_tag_dyneither(_tmpB61,
sizeof(char),1)))),((_tmpB5E[0]=& _tmpB5D,((_tmpB5E[1]=& _tmpB5C,Cyc_aprintf(((
_tmpB5F="%s%s",_tag_dyneither(_tmpB5F,sizeof(char),5))),_tag_dyneither(_tmpB5E,
sizeof(void*),2))))))))))))));const char*_tmpB6A;const char*_tmpB69;const char*
_tmpB68;void*_tmpB67[2];struct Cyc_String_pa_struct _tmpB66;struct Cyc_String_pa_struct
_tmpB65;struct _dyneither_ptr s2=(struct _dyneither_ptr)((_tmpB65.tag=0,((_tmpB65.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(Cyc_Tcutil_t2_failure)),((
_tmpB66.tag=0,((_tmpB66.f1=(struct _dyneither_ptr)(Cyc_Tcutil_tq2_const?(_tmpB69="const ",
_tag_dyneither(_tmpB69,sizeof(char),7)):((_tmpB6A="",_tag_dyneither(_tmpB6A,
sizeof(char),1)))),((_tmpB67[0]=& _tmpB66,((_tmpB67[1]=& _tmpB65,Cyc_aprintf(((
_tmpB68="%s%s",_tag_dyneither(_tmpB68,sizeof(char),5))),_tag_dyneither(_tmpB67,
sizeof(void*),2))))))))))))));int pos=2;{const char*_tmpB6E;void*_tmpB6D[1];struct
Cyc_String_pa_struct _tmpB6C;(_tmpB6C.tag=0,((_tmpB6C.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)s1),((_tmpB6D[0]=& _tmpB6C,Cyc_fprintf(Cyc_stderr,((_tmpB6E="  %s",
_tag_dyneither(_tmpB6E,sizeof(char),5))),_tag_dyneither(_tmpB6D,sizeof(void*),1)))))));}
pos +=_get_dyneither_size(s1,sizeof(char));if(pos + 5 >= 80){{const char*_tmpB71;
void*_tmpB70;(_tmpB70=0,Cyc_fprintf(Cyc_stderr,((_tmpB71="\n\t",_tag_dyneither(
_tmpB71,sizeof(char),3))),_tag_dyneither(_tmpB70,sizeof(void*),0)));}pos=8;}
else{{const char*_tmpB74;void*_tmpB73;(_tmpB73=0,Cyc_fprintf(Cyc_stderr,((_tmpB74=" ",
_tag_dyneither(_tmpB74,sizeof(char),2))),_tag_dyneither(_tmpB73,sizeof(void*),0)));}
++ pos;}{const char*_tmpB77;void*_tmpB76;(_tmpB76=0,Cyc_fprintf(Cyc_stderr,((
_tmpB77="and ",_tag_dyneither(_tmpB77,sizeof(char),5))),_tag_dyneither(_tmpB76,
sizeof(void*),0)));}pos +=4;if(pos + _get_dyneither_size(s2,sizeof(char))>= 80){{
const char*_tmpB7A;void*_tmpB79;(_tmpB79=0,Cyc_fprintf(Cyc_stderr,((_tmpB7A="\n\t",
_tag_dyneither(_tmpB7A,sizeof(char),3))),_tag_dyneither(_tmpB79,sizeof(void*),0)));}
pos=8;}{const char*_tmpB7E;void*_tmpB7D[1];struct Cyc_String_pa_struct _tmpB7C;(
_tmpB7C.tag=0,((_tmpB7C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s2),((
_tmpB7D[0]=& _tmpB7C,Cyc_fprintf(Cyc_stderr,((_tmpB7E="%s ",_tag_dyneither(
_tmpB7E,sizeof(char),4))),_tag_dyneither(_tmpB7D,sizeof(void*),1)))))));}pos +=
_get_dyneither_size(s2,sizeof(char))+ 1;if(pos + 17 >= 80){{const char*_tmpB81;void*
_tmpB80;(_tmpB80=0,Cyc_fprintf(Cyc_stderr,((_tmpB81="\n\t",_tag_dyneither(
_tmpB81,sizeof(char),3))),_tag_dyneither(_tmpB80,sizeof(void*),0)));}pos=8;}{
const char*_tmpB84;void*_tmpB83;(_tmpB83=0,Cyc_fprintf(Cyc_stderr,((_tmpB84="are not compatible. ",
_tag_dyneither(_tmpB84,sizeof(char),21))),_tag_dyneither(_tmpB83,sizeof(void*),0)));}
pos +=17;if(Cyc_Tcutil_failure_reason.curr != (_tag_dyneither(0,0,0)).curr){if(pos
+ Cyc_strlen((struct _dyneither_ptr)Cyc_Tcutil_failure_reason)>= 80){const char*
_tmpB87;void*_tmpB86;(_tmpB86=0,Cyc_fprintf(Cyc_stderr,((_tmpB87="\n\t",
_tag_dyneither(_tmpB87,sizeof(char),3))),_tag_dyneither(_tmpB86,sizeof(void*),0)));}{
const char*_tmpB8B;void*_tmpB8A[1];struct Cyc_String_pa_struct _tmpB89;(_tmpB89.tag=
0,((_tmpB89.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_failure_reason),((
_tmpB8A[0]=& _tmpB89,Cyc_fprintf(Cyc_stderr,((_tmpB8B="%s",_tag_dyneither(_tmpB8B,
sizeof(char),3))),_tag_dyneither(_tmpB8A,sizeof(void*),1)))))));}}{const char*
_tmpB8E;void*_tmpB8D;(_tmpB8D=0,Cyc_fprintf(Cyc_stderr,((_tmpB8E="\n",
_tag_dyneither(_tmpB8E,sizeof(char),2))),_tag_dyneither(_tmpB8D,sizeof(void*),0)));}
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);}}void Cyc_Tcutil_terr(struct Cyc_Position_Segment*
loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);void Cyc_Tcutil_terr(struct
Cyc_Position_Segment*loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){Cyc_Position_post_error(
Cyc_Position_mk_err_elab(loc,(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,
fmt,ap)));}void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){struct
_dyneither_ptr msg=(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap);{
const char*_tmpB92;void*_tmpB91[1];struct Cyc_String_pa_struct _tmpB90;(_tmpB90.tag=
0,((_tmpB90.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg),((_tmpB91[0]=&
_tmpB90,Cyc_fprintf(Cyc_stderr,((_tmpB92="Compiler Error (Tcutil::impos): %s\n",
_tag_dyneither(_tmpB92,sizeof(char),36))),_tag_dyneither(_tmpB91,sizeof(void*),1)))))));}
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);{struct Cyc_Core_Impossible_struct
_tmpB95;struct Cyc_Core_Impossible_struct*_tmpB94;(int)_throw((void*)((_tmpB94=
_cycalloc(sizeof(*_tmpB94)),((_tmpB94[0]=((_tmpB95.tag=Cyc_Core_Impossible,((
_tmpB95.f1=msg,_tmpB95)))),_tmpB94)))));}}static struct _dyneither_ptr Cyc_Tcutil_tvar2string(
struct Cyc_Absyn_Tvar*tv);static struct _dyneither_ptr Cyc_Tcutil_tvar2string(struct
Cyc_Absyn_Tvar*tv){return*tv->name;}void Cyc_Tcutil_print_tvars(struct Cyc_List_List*
tvs);void Cyc_Tcutil_print_tvars(struct Cyc_List_List*tvs){for(0;tvs != 0;tvs=tvs->tl){
const char*_tmpB9A;void*_tmpB99[2];struct Cyc_String_pa_struct _tmpB98;struct Cyc_String_pa_struct
_tmpB97;(_tmpB97.tag=0,((_tmpB97.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kindbound2string((void*)((struct Cyc_Absyn_Tvar*)tvs->hd)->kind)),((
_tmpB98.tag=0,((_tmpB98.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_tvar2string((
struct Cyc_Absyn_Tvar*)tvs->hd)),((_tmpB99[0]=& _tmpB98,((_tmpB99[1]=& _tmpB97,Cyc_fprintf(
Cyc_stderr,((_tmpB9A="%s::%s ",_tag_dyneither(_tmpB9A,sizeof(char),8))),
_tag_dyneither(_tmpB99,sizeof(void*),2)))))))))))));}{const char*_tmpB9D;void*
_tmpB9C;(_tmpB9C=0,Cyc_fprintf(Cyc_stderr,((_tmpB9D="\n",_tag_dyneither(_tmpB9D,
sizeof(char),2))),_tag_dyneither(_tmpB9C,sizeof(void*),0)));}Cyc_fflush((struct
Cyc___cycFILE*)Cyc_stderr);}static struct Cyc_List_List*Cyc_Tcutil_warning_segs=0;
static struct Cyc_List_List*Cyc_Tcutil_warning_msgs=0;void Cyc_Tcutil_warn(struct
Cyc_Position_Segment*sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);void Cyc_Tcutil_warn(
struct Cyc_Position_Segment*sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
struct _dyneither_ptr msg=(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,
fmt,ap);{struct Cyc_List_List*_tmpB9E;Cyc_Tcutil_warning_segs=((_tmpB9E=_cycalloc(
sizeof(*_tmpB9E)),((_tmpB9E->hd=sg,((_tmpB9E->tl=Cyc_Tcutil_warning_segs,_tmpB9E))))));}{
struct _dyneither_ptr*_tmpBA1;struct Cyc_List_List*_tmpBA0;Cyc_Tcutil_warning_msgs=((
_tmpBA0=_cycalloc(sizeof(*_tmpBA0)),((_tmpBA0->hd=((_tmpBA1=_cycalloc(sizeof(*
_tmpBA1)),((_tmpBA1[0]=msg,_tmpBA1)))),((_tmpBA0->tl=Cyc_Tcutil_warning_msgs,
_tmpBA0))))));}}void Cyc_Tcutil_flush_warnings();void Cyc_Tcutil_flush_warnings(){
if(Cyc_Tcutil_warning_segs == 0)return;{const char*_tmpBA4;void*_tmpBA3;(_tmpBA3=0,
Cyc_fprintf(Cyc_stderr,((_tmpBA4="***Warnings***\n",_tag_dyneither(_tmpBA4,
sizeof(char),16))),_tag_dyneither(_tmpBA3,sizeof(void*),0)));}{struct Cyc_List_List*
_tmp35=Cyc_Position_strings_of_segments(Cyc_Tcutil_warning_segs);Cyc_Tcutil_warning_segs=
0;Cyc_Tcutil_warning_msgs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_Tcutil_warning_msgs);while(Cyc_Tcutil_warning_msgs != 0){{const char*_tmpBA9;
void*_tmpBA8[2];struct Cyc_String_pa_struct _tmpBA7;struct Cyc_String_pa_struct
_tmpBA6;(_tmpBA6.tag=0,((_tmpBA6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((
struct _dyneither_ptr*)((struct Cyc_List_List*)_check_null(Cyc_Tcutil_warning_msgs))->hd)),((
_tmpBA7.tag=0,((_tmpBA7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((
struct _dyneither_ptr*)((struct Cyc_List_List*)_check_null(_tmp35))->hd)),((
_tmpBA8[0]=& _tmpBA7,((_tmpBA8[1]=& _tmpBA6,Cyc_fprintf(Cyc_stderr,((_tmpBA9="%s: %s\n",
_tag_dyneither(_tmpBA9,sizeof(char),8))),_tag_dyneither(_tmpBA8,sizeof(void*),2)))))))))))));}
_tmp35=_tmp35->tl;Cyc_Tcutil_warning_msgs=((struct Cyc_List_List*)_check_null(Cyc_Tcutil_warning_msgs))->tl;}{
const char*_tmpBAC;void*_tmpBAB;(_tmpBAB=0,Cyc_fprintf(Cyc_stderr,((_tmpBAC="**************\n",
_tag_dyneither(_tmpBAC,sizeof(char),16))),_tag_dyneither(_tmpBAB,sizeof(void*),0)));}
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
_tmpBAD;*_tmp40=((_tmpBAD=_cycalloc(sizeof(*_tmpBAD)),((_tmpBAD[0]=t2,_tmpBAD))));}
return t2;}_LL7: if(*((int*)_tmp3C)!= 0)goto _LL9;_tmp41=((struct Cyc_Absyn_Evar_struct*)
_tmp3C)->f2;_tmp42=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)_tmp3C)->f2;
_LL8: {void*t2=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(*
_tmp42))->v);if(t2 != (void*)((struct Cyc_Core_Opt*)_check_null(*_tmp42))->v){
struct Cyc_Core_Opt*_tmpBAE;*_tmp42=((_tmpBAE=_cycalloc(sizeof(*_tmpBAE)),((
_tmpBAE->v=(void*)t2,_tmpBAE))));}return t2;}_LL9:;_LLA: return t;_LL0:;}void*Cyc_Tcutil_copy_type(
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
_tmpBB1;struct Cyc_Absyn_Unknown_kb_struct*_tmpBB0;return(void*)((_tmpBB0=
_cycalloc(sizeof(*_tmpBB0)),((_tmpBB0[0]=((_tmpBB1.tag=1,((_tmpBB1.f1=0,_tmpBB1)))),
_tmpBB0))));}_LL15: if(*((int*)_tmp48)!= 2)goto _LL17;_tmp49=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp48)->f2;_LL16: {struct Cyc_Absyn_Less_kb_struct _tmpBB4;struct Cyc_Absyn_Less_kb_struct*
_tmpBB3;return(void*)((_tmpBB3=_cycalloc(sizeof(*_tmpBB3)),((_tmpBB3[0]=((
_tmpBB4.tag=2,((_tmpBB4.f1=0,((_tmpBB4.f2=(void*)_tmp49,_tmpBB4)))))),_tmpBB3))));}
_LL17:;_LL18: return kb;_LL12:;}static struct Cyc_Absyn_Tvar*Cyc_Tcutil_copy_tvar(
struct Cyc_Absyn_Tvar*tv);static struct Cyc_Absyn_Tvar*Cyc_Tcutil_copy_tvar(struct
Cyc_Absyn_Tvar*tv){struct Cyc_Absyn_Tvar*_tmpBB5;return(_tmpBB5=_cycalloc(sizeof(*
_tmpBB5)),((_tmpBB5->name=tv->name,((_tmpBB5->identity=- 1,((_tmpBB5->kind=(void*)
Cyc_Tcutil_copy_kindbound((void*)tv->kind),_tmpBB5)))))));}static struct _tuple3*
Cyc_Tcutil_copy_arg(struct _tuple3*arg);static struct _tuple3*Cyc_Tcutil_copy_arg(
struct _tuple3*arg){struct _tuple3 _tmp50;struct Cyc_Core_Opt*_tmp51;struct Cyc_Absyn_Tqual
_tmp52;void*_tmp53;struct _tuple3*_tmp4F=arg;_tmp50=*_tmp4F;_tmp51=_tmp50.f1;
_tmp52=_tmp50.f2;_tmp53=_tmp50.f3;{struct _tuple3*_tmpBB6;return(_tmpBB6=
_cycalloc(sizeof(*_tmpBB6)),((_tmpBB6->f1=_tmp51,((_tmpBB6->f2=_tmp52,((_tmpBB6->f3=
Cyc_Tcutil_copy_type(_tmp53),_tmpBB6)))))));}}static struct _tuple5*Cyc_Tcutil_copy_tqt(
struct _tuple5*arg);static struct _tuple5*Cyc_Tcutil_copy_tqt(struct _tuple5*arg){
struct _tuple5 _tmp56;struct Cyc_Absyn_Tqual _tmp57;void*_tmp58;struct _tuple5*_tmp55=
arg;_tmp56=*_tmp55;_tmp57=_tmp56.f1;_tmp58=_tmp56.f2;{struct _tuple5*_tmpBB7;
return(_tmpBB7=_cycalloc(sizeof(*_tmpBB7)),((_tmpBB7->f1=_tmp57,((_tmpBB7->f2=
Cyc_Tcutil_copy_type(_tmp58),_tmpBB7)))));}}static struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_copy_field(
struct Cyc_Absyn_Aggrfield*f);static struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_copy_field(
struct Cyc_Absyn_Aggrfield*f){struct Cyc_Absyn_Aggrfield*_tmpBB8;return(_tmpBB8=
_cycalloc(sizeof(*_tmpBB8)),((_tmpBB8->name=f->name,((_tmpBB8->tq=f->tq,((
_tmpBB8->type=(void*)Cyc_Tcutil_copy_type((void*)f->type),((_tmpBB8->width=f->width,((
_tmpBB8->attributes=f->attributes,_tmpBB8)))))))))));}static struct _tuple0*Cyc_Tcutil_copy_rgncmp(
struct _tuple0*x);static struct _tuple0*Cyc_Tcutil_copy_rgncmp(struct _tuple0*x){
struct _tuple0 _tmp5C;void*_tmp5D;void*_tmp5E;struct _tuple0*_tmp5B=x;_tmp5C=*
_tmp5B;_tmp5D=_tmp5C.f1;_tmp5E=_tmp5C.f2;{struct _tuple0*_tmpBB9;return(_tmpBB9=
_cycalloc(sizeof(*_tmpBB9)),((_tmpBB9->f1=Cyc_Tcutil_copy_type(_tmp5D),((_tmpBB9->f2=
Cyc_Tcutil_copy_type(_tmp5E),_tmpBB9)))));}}static struct Cyc_Absyn_Enumfield*Cyc_Tcutil_copy_enumfield(
struct Cyc_Absyn_Enumfield*f);static struct Cyc_Absyn_Enumfield*Cyc_Tcutil_copy_enumfield(
struct Cyc_Absyn_Enumfield*f){struct Cyc_Absyn_Enumfield*_tmpBBA;return(_tmpBBA=
_cycalloc(sizeof(*_tmpBBA)),((_tmpBBA->name=f->name,((_tmpBBA->tag=f->tag,((
_tmpBBA->loc=f->loc,_tmpBBA)))))));}void*Cyc_Tcutil_copy_type(void*t);void*Cyc_Tcutil_copy_type(
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
_tmp61)->f1;_LL1F: {struct Cyc_Absyn_VarType_struct _tmpBBD;struct Cyc_Absyn_VarType_struct*
_tmpBBC;return(void*)((_tmpBBC=_cycalloc(sizeof(*_tmpBBC)),((_tmpBBC[0]=((
_tmpBBD.tag=1,((_tmpBBD.f1=Cyc_Tcutil_copy_tvar(_tmp62),_tmpBBD)))),_tmpBBC))));}
_LL20: if(*((int*)_tmp61)!= 2)goto _LL22;_tmp63=((struct Cyc_Absyn_TunionType_struct*)
_tmp61)->f1;_tmp64=_tmp63.tunion_info;_tmp65=_tmp63.targs;_tmp66=_tmp63.rgn;
_LL21: {struct Cyc_Core_Opt*_tmpBBE;struct Cyc_Core_Opt*_tmp9E=(unsigned int)
_tmp66?(_tmpBBE=_cycalloc(sizeof(*_tmpBBE)),((_tmpBBE->v=(void*)Cyc_Tcutil_copy_type((
void*)_tmp66->v),_tmpBBE))): 0;struct Cyc_Absyn_TunionType_struct _tmpBC4;struct Cyc_Absyn_TunionInfo
_tmpBC3;struct Cyc_Absyn_TunionType_struct*_tmpBC2;return(void*)((_tmpBC2=
_cycalloc(sizeof(*_tmpBC2)),((_tmpBC2[0]=((_tmpBC4.tag=2,((_tmpBC4.f1=((_tmpBC3.tunion_info=
_tmp64,((_tmpBC3.targs=Cyc_Tcutil_copy_types(_tmp65),((_tmpBC3.rgn=_tmp9E,
_tmpBC3)))))),_tmpBC4)))),_tmpBC2))));}_LL22: if(*((int*)_tmp61)!= 3)goto _LL24;
_tmp67=((struct Cyc_Absyn_TunionFieldType_struct*)_tmp61)->f1;_tmp68=_tmp67.field_info;
_tmp69=_tmp67.targs;_LL23: {struct Cyc_Absyn_TunionFieldType_struct _tmpBCA;struct
Cyc_Absyn_TunionFieldInfo _tmpBC9;struct Cyc_Absyn_TunionFieldType_struct*_tmpBC8;
return(void*)((_tmpBC8=_cycalloc(sizeof(*_tmpBC8)),((_tmpBC8[0]=((_tmpBCA.tag=3,((
_tmpBCA.f1=((_tmpBC9.field_info=_tmp68,((_tmpBC9.targs=Cyc_Tcutil_copy_types(
_tmp69),_tmpBC9)))),_tmpBCA)))),_tmpBC8))));}_LL24: if(*((int*)_tmp61)!= 4)goto
_LL26;_tmp6A=((struct Cyc_Absyn_PointerType_struct*)_tmp61)->f1;_tmp6B=(void*)
_tmp6A.elt_typ;_tmp6C=_tmp6A.elt_tq;_tmp6D=_tmp6A.ptr_atts;_tmp6E=(void*)_tmp6D.rgn;
_tmp6F=_tmp6D.nullable;_tmp70=_tmp6D.bounds;_tmp71=_tmp6D.zero_term;_tmp72=
_tmp6D.ptrloc;_LL25: {void*_tmpA6=Cyc_Tcutil_copy_type(_tmp6B);void*_tmpA7=Cyc_Tcutil_copy_type(
_tmp6E);struct Cyc_Absyn_Conref*_tmpA8=((struct Cyc_Absyn_Conref*(*)(struct Cyc_Absyn_Conref*
c))Cyc_Tcutil_copy_conref)(_tmp6F);struct Cyc_Absyn_Tqual _tmpA9=_tmp6C;struct Cyc_Absyn_Conref*
_tmpAA=Cyc_Tcutil_copy_conref(_tmp70);struct Cyc_Absyn_Conref*_tmpAB=((struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref*c))Cyc_Tcutil_copy_conref)(_tmp71);struct Cyc_Absyn_PointerType_struct
_tmpBD4;struct Cyc_Absyn_PtrAtts _tmpBD3;struct Cyc_Absyn_PtrInfo _tmpBD2;struct Cyc_Absyn_PointerType_struct*
_tmpBD1;return(void*)((_tmpBD1=_cycalloc(sizeof(*_tmpBD1)),((_tmpBD1[0]=((
_tmpBD4.tag=4,((_tmpBD4.f1=((_tmpBD2.elt_typ=(void*)_tmpA6,((_tmpBD2.elt_tq=
_tmpA9,((_tmpBD2.ptr_atts=((_tmpBD3.rgn=(void*)_tmpA7,((_tmpBD3.nullable=_tmpA8,((
_tmpBD3.bounds=_tmpAA,((_tmpBD3.zero_term=_tmpAB,((_tmpBD3.ptrloc=_tmp72,_tmpBD3)))))))))),
_tmpBD2)))))),_tmpBD4)))),_tmpBD1))));}_LL26: if(*((int*)_tmp61)!= 5)goto _LL28;
_LL27: goto _LL29;_LL28: if((int)_tmp61 != 1)goto _LL2A;_LL29: goto _LL2B;_LL2A: if(
_tmp61 <= (void*)4)goto _LL46;if(*((int*)_tmp61)!= 6)goto _LL2C;_LL2B: return t;_LL2C:
if(*((int*)_tmp61)!= 7)goto _LL2E;_tmp73=((struct Cyc_Absyn_ArrayType_struct*)
_tmp61)->f1;_tmp74=(void*)_tmp73.elt_type;_tmp75=_tmp73.tq;_tmp76=_tmp73.num_elts;
_tmp77=_tmp73.zero_term;_tmp78=_tmp73.zt_loc;_LL2D: {struct Cyc_Absyn_ArrayType_struct
_tmpBDA;struct Cyc_Absyn_ArrayInfo _tmpBD9;struct Cyc_Absyn_ArrayType_struct*
_tmpBD8;return(void*)((_tmpBD8=_cycalloc(sizeof(*_tmpBD8)),((_tmpBD8[0]=((
_tmpBDA.tag=7,((_tmpBDA.f1=((_tmpBD9.elt_type=(void*)Cyc_Tcutil_copy_type(_tmp74),((
_tmpBD9.tq=_tmp75,((_tmpBD9.num_elts=_tmp76,((_tmpBD9.zero_term=((struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref*c))Cyc_Tcutil_copy_conref)(_tmp77),((_tmpBD9.zt_loc=
_tmp78,_tmpBD9)))))))))),_tmpBDA)))),_tmpBD8))));}_LL2E: if(*((int*)_tmp61)!= 8)
goto _LL30;_tmp79=((struct Cyc_Absyn_FnType_struct*)_tmp61)->f1;_tmp7A=_tmp79.tvars;
_tmp7B=_tmp79.effect;_tmp7C=(void*)_tmp79.ret_typ;_tmp7D=_tmp79.args;_tmp7E=
_tmp79.c_varargs;_tmp7F=_tmp79.cyc_varargs;_tmp80=_tmp79.rgn_po;_tmp81=_tmp79.attributes;
_LL2F: {struct Cyc_List_List*_tmpB3=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*
f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_tvar,
_tmp7A);struct Cyc_Core_Opt*_tmpBDB;struct Cyc_Core_Opt*_tmpB4=_tmp7B == 0?0:((
_tmpBDB=_cycalloc(sizeof(*_tmpBDB)),((_tmpBDB->v=(void*)Cyc_Tcutil_copy_type((
void*)_tmp7B->v),_tmpBDB))));void*_tmpB5=Cyc_Tcutil_copy_type(_tmp7C);struct Cyc_List_List*
_tmpB6=((struct Cyc_List_List*(*)(struct _tuple3*(*f)(struct _tuple3*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Tcutil_copy_arg,_tmp7D);int _tmpB7=_tmp7E;struct Cyc_Absyn_VarargInfo*
cyc_varargs2=0;if(_tmp7F != 0){struct Cyc_Absyn_VarargInfo*cv=(struct Cyc_Absyn_VarargInfo*)
_tmp7F;struct Cyc_Absyn_VarargInfo*_tmpBDC;cyc_varargs2=((_tmpBDC=_cycalloc(
sizeof(*_tmpBDC)),((_tmpBDC->name=cv->name,((_tmpBDC->tq=cv->tq,((_tmpBDC->type=(
void*)Cyc_Tcutil_copy_type((void*)cv->type),((_tmpBDC->inject=cv->inject,_tmpBDC))))))))));}{
struct Cyc_List_List*_tmpB9=((struct Cyc_List_List*(*)(struct _tuple0*(*f)(struct
_tuple0*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_rgncmp,_tmp80);
struct Cyc_List_List*_tmpBA=_tmp81;struct Cyc_Absyn_FnType_struct _tmpBE2;struct Cyc_Absyn_FnInfo
_tmpBE1;struct Cyc_Absyn_FnType_struct*_tmpBE0;return(void*)((_tmpBE0=_cycalloc(
sizeof(*_tmpBE0)),((_tmpBE0[0]=((_tmpBE2.tag=8,((_tmpBE2.f1=((_tmpBE1.tvars=
_tmpB3,((_tmpBE1.effect=_tmpB4,((_tmpBE1.ret_typ=(void*)_tmpB5,((_tmpBE1.args=
_tmpB6,((_tmpBE1.c_varargs=_tmpB7,((_tmpBE1.cyc_varargs=cyc_varargs2,((_tmpBE1.rgn_po=
_tmpB9,((_tmpBE1.attributes=_tmpBA,_tmpBE1)))))))))))))))),_tmpBE2)))),_tmpBE0))));}}
_LL30: if(*((int*)_tmp61)!= 9)goto _LL32;_tmp82=((struct Cyc_Absyn_TupleType_struct*)
_tmp61)->f1;_LL31: {struct Cyc_Absyn_TupleType_struct _tmpBE5;struct Cyc_Absyn_TupleType_struct*
_tmpBE4;return(void*)((_tmpBE4=_cycalloc(sizeof(*_tmpBE4)),((_tmpBE4[0]=((
_tmpBE5.tag=9,((_tmpBE5.f1=((struct Cyc_List_List*(*)(struct _tuple5*(*f)(struct
_tuple5*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_tqt,_tmp82),
_tmpBE5)))),_tmpBE4))));}_LL32: if(*((int*)_tmp61)!= 10)goto _LL34;_tmp83=((struct
Cyc_Absyn_AggrType_struct*)_tmp61)->f1;_tmp84=_tmp83.aggr_info;if((((((struct Cyc_Absyn_AggrType_struct*)
_tmp61)->f1).aggr_info).UnknownAggr).tag != 0)goto _LL34;_tmp85=(_tmp84.UnknownAggr).f1;
_tmp86=(_tmp84.UnknownAggr).f2;_tmp87=_tmp83.targs;_LL33: {struct Cyc_Absyn_AggrType_struct
_tmpBEF;union Cyc_Absyn_AggrInfoU_union _tmpBEE;struct Cyc_Absyn_AggrInfo _tmpBED;
struct Cyc_Absyn_AggrType_struct*_tmpBEC;return(void*)((_tmpBEC=_cycalloc(sizeof(*
_tmpBEC)),((_tmpBEC[0]=((_tmpBEF.tag=10,((_tmpBEF.f1=((_tmpBED.aggr_info=(union
Cyc_Absyn_AggrInfoU_union)(((_tmpBEE.UnknownAggr).tag=0,(((_tmpBEE.UnknownAggr).f1=(
void*)_tmp85,(((_tmpBEE.UnknownAggr).f2=_tmp86,_tmpBEE)))))),((_tmpBED.targs=Cyc_Tcutil_copy_types(
_tmp87),_tmpBED)))),_tmpBEF)))),_tmpBEC))));}_LL34: if(*((int*)_tmp61)!= 10)goto
_LL36;_tmp88=((struct Cyc_Absyn_AggrType_struct*)_tmp61)->f1;_tmp89=_tmp88.aggr_info;
if((((((struct Cyc_Absyn_AggrType_struct*)_tmp61)->f1).aggr_info).KnownAggr).tag
!= 1)goto _LL36;_tmp8A=(_tmp89.KnownAggr).f1;_tmp8B=_tmp88.targs;_LL35: {struct
Cyc_Absyn_AggrType_struct _tmpBF9;union Cyc_Absyn_AggrInfoU_union _tmpBF8;struct Cyc_Absyn_AggrInfo
_tmpBF7;struct Cyc_Absyn_AggrType_struct*_tmpBF6;return(void*)((_tmpBF6=_cycalloc(
sizeof(*_tmpBF6)),((_tmpBF6[0]=((_tmpBF9.tag=10,((_tmpBF9.f1=((_tmpBF7.aggr_info=(
union Cyc_Absyn_AggrInfoU_union)(((_tmpBF8.KnownAggr).tag=1,(((_tmpBF8.KnownAggr).f1=
_tmp8A,_tmpBF8)))),((_tmpBF7.targs=Cyc_Tcutil_copy_types(_tmp8B),_tmpBF7)))),
_tmpBF9)))),_tmpBF6))));}_LL36: if(*((int*)_tmp61)!= 11)goto _LL38;_tmp8C=(void*)((
struct Cyc_Absyn_AnonAggrType_struct*)_tmp61)->f1;_tmp8D=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp61)->f2;_LL37: {struct Cyc_Absyn_AnonAggrType_struct _tmpBFC;struct Cyc_Absyn_AnonAggrType_struct*
_tmpBFB;return(void*)((_tmpBFB=_cycalloc(sizeof(*_tmpBFB)),((_tmpBFB[0]=((
_tmpBFC.tag=11,((_tmpBFC.f1=(void*)_tmp8C,((_tmpBFC.f2=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Tcutil_copy_field,_tmp8D),_tmpBFC)))))),_tmpBFB))));}_LL38: if(*((
int*)_tmp61)!= 12)goto _LL3A;_tmp8E=((struct Cyc_Absyn_EnumType_struct*)_tmp61)->f1;
_tmp8F=((struct Cyc_Absyn_EnumType_struct*)_tmp61)->f2;_LL39: {struct Cyc_Absyn_EnumType_struct
_tmpBFF;struct Cyc_Absyn_EnumType_struct*_tmpBFE;return(void*)((_tmpBFE=_cycalloc(
sizeof(*_tmpBFE)),((_tmpBFE[0]=((_tmpBFF.tag=12,((_tmpBFF.f1=_tmp8E,((_tmpBFF.f2=
_tmp8F,_tmpBFF)))))),_tmpBFE))));}_LL3A: if(*((int*)_tmp61)!= 13)goto _LL3C;_tmp90=((
struct Cyc_Absyn_AnonEnumType_struct*)_tmp61)->f1;_LL3B: {struct Cyc_Absyn_AnonEnumType_struct
_tmpC02;struct Cyc_Absyn_AnonEnumType_struct*_tmpC01;return(void*)((_tmpC01=
_cycalloc(sizeof(*_tmpC01)),((_tmpC01[0]=((_tmpC02.tag=13,((_tmpC02.f1=((struct
Cyc_List_List*(*)(struct Cyc_Absyn_Enumfield*(*f)(struct Cyc_Absyn_Enumfield*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_enumfield,_tmp90),_tmpC02)))),
_tmpC01))));}_LL3C: if(*((int*)_tmp61)!= 18)goto _LL3E;_tmp91=(void*)((struct Cyc_Absyn_TagType_struct*)
_tmp61)->f1;_LL3D: {struct Cyc_Absyn_TagType_struct _tmpC05;struct Cyc_Absyn_TagType_struct*
_tmpC04;return(void*)((_tmpC04=_cycalloc(sizeof(*_tmpC04)),((_tmpC04[0]=((
_tmpC05.tag=18,((_tmpC05.f1=(void*)Cyc_Tcutil_copy_type(_tmp91),_tmpC05)))),
_tmpC04))));}_LL3E: if(*((int*)_tmp61)!= 17)goto _LL40;_tmp92=((struct Cyc_Absyn_ValueofType_struct*)
_tmp61)->f1;_LL3F: {struct Cyc_Absyn_ValueofType_struct _tmpC08;struct Cyc_Absyn_ValueofType_struct*
_tmpC07;return(void*)((_tmpC07=_cycalloc(sizeof(*_tmpC07)),((_tmpC07[0]=((
_tmpC08.tag=17,((_tmpC08.f1=_tmp92,_tmpC08)))),_tmpC07))));}_LL40: if(*((int*)
_tmp61)!= 14)goto _LL42;_tmp93=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp61)->f1;_LL41: {struct Cyc_Absyn_RgnHandleType_struct _tmpC0B;struct Cyc_Absyn_RgnHandleType_struct*
_tmpC0A;return(void*)((_tmpC0A=_cycalloc(sizeof(*_tmpC0A)),((_tmpC0A[0]=((
_tmpC0B.tag=14,((_tmpC0B.f1=(void*)Cyc_Tcutil_copy_type(_tmp93),_tmpC0B)))),
_tmpC0A))));}_LL42: if(*((int*)_tmp61)!= 15)goto _LL44;_tmp94=(void*)((struct Cyc_Absyn_DynRgnType_struct*)
_tmp61)->f1;_tmp95=(void*)((struct Cyc_Absyn_DynRgnType_struct*)_tmp61)->f2;_LL43: {
struct Cyc_Absyn_DynRgnType_struct _tmpC0E;struct Cyc_Absyn_DynRgnType_struct*
_tmpC0D;return(void*)((_tmpC0D=_cycalloc(sizeof(*_tmpC0D)),((_tmpC0D[0]=((
_tmpC0E.tag=15,((_tmpC0E.f1=(void*)Cyc_Tcutil_copy_type(_tmp94),((_tmpC0E.f2=(
void*)Cyc_Tcutil_copy_type(_tmp95),_tmpC0E)))))),_tmpC0D))));}_LL44: if(*((int*)
_tmp61)!= 16)goto _LL46;_tmp96=((struct Cyc_Absyn_TypedefType_struct*)_tmp61)->f1;
_tmp97=((struct Cyc_Absyn_TypedefType_struct*)_tmp61)->f2;_tmp98=((struct Cyc_Absyn_TypedefType_struct*)
_tmp61)->f3;_LL45: {struct Cyc_Absyn_TypedefType_struct _tmpC11;struct Cyc_Absyn_TypedefType_struct*
_tmpC10;return(void*)((_tmpC10=_cycalloc(sizeof(*_tmpC10)),((_tmpC10[0]=((
_tmpC11.tag=16,((_tmpC11.f1=_tmp96,((_tmpC11.f2=Cyc_Tcutil_copy_types(_tmp97),((
_tmpC11.f3=_tmp98,((_tmpC11.f4=0,_tmpC11)))))))))),_tmpC10))));}_LL46: if((int)
_tmp61 != 3)goto _LL48;_LL47: goto _LL49;_LL48: if((int)_tmp61 != 2)goto _LL4A;_LL49:
return t;_LL4A: if(_tmp61 <= (void*)4)goto _LL4C;if(*((int*)_tmp61)!= 19)goto _LL4C;
_tmp99=(void*)((struct Cyc_Absyn_AccessEff_struct*)_tmp61)->f1;_LL4B: {struct Cyc_Absyn_AccessEff_struct
_tmpC14;struct Cyc_Absyn_AccessEff_struct*_tmpC13;return(void*)((_tmpC13=
_cycalloc(sizeof(*_tmpC13)),((_tmpC13[0]=((_tmpC14.tag=19,((_tmpC14.f1=(void*)
Cyc_Tcutil_copy_type(_tmp99),_tmpC14)))),_tmpC13))));}_LL4C: if(_tmp61 <= (void*)4)
goto _LL4E;if(*((int*)_tmp61)!= 20)goto _LL4E;_tmp9A=((struct Cyc_Absyn_JoinEff_struct*)
_tmp61)->f1;_LL4D: {struct Cyc_Absyn_JoinEff_struct _tmpC17;struct Cyc_Absyn_JoinEff_struct*
_tmpC16;return(void*)((_tmpC16=_cycalloc(sizeof(*_tmpC16)),((_tmpC16[0]=((
_tmpC17.tag=20,((_tmpC17.f1=Cyc_Tcutil_copy_types(_tmp9A),_tmpC17)))),_tmpC16))));}
_LL4E: if(_tmp61 <= (void*)4)goto _LL19;if(*((int*)_tmp61)!= 21)goto _LL19;_tmp9B=(
void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp61)->f1;_LL4F: {struct Cyc_Absyn_RgnsEff_struct
_tmpC1A;struct Cyc_Absyn_RgnsEff_struct*_tmpC19;return(void*)((_tmpC19=_cycalloc(
sizeof(*_tmpC19)),((_tmpC19[0]=((_tmpC1A.tag=21,((_tmpC1A.f1=(void*)Cyc_Tcutil_copy_type(
_tmp9B),_tmpC1A)))),_tmpC19))));}_LL19:;}int Cyc_Tcutil_kind_leq(void*k1,void*k2);
int Cyc_Tcutil_kind_leq(void*k1,void*k2){if(k1 == k2)return 1;{struct _tuple0 _tmpC1B;
struct _tuple0 _tmpE0=(_tmpC1B.f1=k1,((_tmpC1B.f2=k2,_tmpC1B)));void*_tmpE1;void*
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
_tmpEC)->f2;_LL6A: return _tmpEE;_LL6B:;_LL6C: {const char*_tmpC1E;void*_tmpC1D;(
_tmpC1D=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmpC1E="kind not suitably constrained!",_tag_dyneither(_tmpC1E,sizeof(char),31))),
_tag_dyneither(_tmpC1D,sizeof(void*),0)));}_LL66:;}struct _tuple8 Cyc_Tcutil_swap_kind(
void*t,void*kb);struct _tuple8 Cyc_Tcutil_swap_kind(void*t,void*kb){void*_tmpF1=
Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*_tmpF2;_LL6E: if(_tmpF1 <= (void*)4)
goto _LL70;if(*((int*)_tmpF1)!= 1)goto _LL70;_tmpF2=((struct Cyc_Absyn_VarType_struct*)
_tmpF1)->f1;_LL6F: {void*_tmpF3=(void*)_tmpF2->kind;(void*)(_tmpF2->kind=(void*)
kb);{struct _tuple8 _tmpC1F;return(_tmpC1F.f1=_tmpF2,((_tmpC1F.f2=_tmpF3,_tmpC1F)));}}
_LL70:;_LL71: {const char*_tmpC23;void*_tmpC22[1];struct Cyc_String_pa_struct
_tmpC21;(_tmpC21.tag=0,((_tmpC21.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmpC22[0]=& _tmpC21,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpC23="swap_kind: cannot update the kind of %s",
_tag_dyneither(_tmpC23,sizeof(char),40))),_tag_dyneither(_tmpC22,sizeof(void*),1)))))));}
_LL6D:;}void*Cyc_Tcutil_typ_kind(void*t);void*Cyc_Tcutil_typ_kind(void*t){void*
_tmpF8=Cyc_Tcutil_compress(t);struct Cyc_Core_Opt*_tmpF9;struct Cyc_Absyn_Tvar*
_tmpFA;void*_tmpFB;struct Cyc_Absyn_TunionInfo _tmpFC;union Cyc_Absyn_TunionInfoU_union
_tmpFD;struct Cyc_Absyn_UnknownTunionInfo _tmpFE;struct Cyc_Absyn_TunionInfo _tmpFF;
union Cyc_Absyn_TunionInfoU_union _tmp100;struct Cyc_Absyn_Tuniondecl**_tmp101;
struct Cyc_Absyn_Tuniondecl*_tmp102;struct Cyc_Absyn_TunionFieldInfo _tmp103;union
Cyc_Absyn_TunionFieldInfoU_union _tmp104;struct Cyc_Absyn_Tuniondecl*_tmp105;
struct Cyc_Absyn_Tunionfield*_tmp106;struct Cyc_Absyn_TunionFieldInfo _tmp107;union
Cyc_Absyn_TunionFieldInfoU_union _tmp108;struct Cyc_Absyn_Enumdecl*_tmp109;struct
Cyc_Absyn_AggrInfo _tmp10A;union Cyc_Absyn_AggrInfoU_union _tmp10B;struct Cyc_Absyn_AggrInfo
_tmp10C;union Cyc_Absyn_AggrInfoU_union _tmp10D;struct Cyc_Absyn_Aggrdecl**_tmp10E;
struct Cyc_Absyn_Aggrdecl*_tmp10F;struct Cyc_Absyn_Aggrdecl _tmp110;struct Cyc_Absyn_AggrdeclImpl*
_tmp111;struct Cyc_Absyn_Enumdecl*_tmp112;struct Cyc_Absyn_PtrInfo _tmp113;struct
Cyc_Absyn_Typedefdecl*_tmp114;_LL73: if(_tmpF8 <= (void*)4)goto _LL77;if(*((int*)
_tmpF8)!= 0)goto _LL75;_tmpF9=((struct Cyc_Absyn_Evar_struct*)_tmpF8)->f1;_LL74:
return(void*)((struct Cyc_Core_Opt*)_check_null(_tmpF9))->v;_LL75: if(*((int*)
_tmpF8)!= 1)goto _LL77;_tmpFA=((struct Cyc_Absyn_VarType_struct*)_tmpF8)->f1;_LL76:
return Cyc_Tcutil_tvar_kind(_tmpFA);_LL77: if((int)_tmpF8 != 0)goto _LL79;_LL78:
return(void*)1;_LL79: if(_tmpF8 <= (void*)4)goto _LL7B;if(*((int*)_tmpF8)!= 5)goto
_LL7B;_tmpFB=(void*)((struct Cyc_Absyn_IntType_struct*)_tmpF8)->f2;_LL7A: return(
_tmpFB == (void*)((void*)2) || _tmpFB == (void*)((void*)3))?(void*)2:(void*)1;
_LL7B: if((int)_tmpF8 != 1)goto _LL7D;_LL7C: goto _LL7E;_LL7D: if(_tmpF8 <= (void*)4)
goto _LL85;if(*((int*)_tmpF8)!= 6)goto _LL7F;_LL7E: goto _LL80;_LL7F: if(*((int*)
_tmpF8)!= 8)goto _LL81;_LL80: return(void*)1;_LL81: if(*((int*)_tmpF8)!= 15)goto
_LL83;_LL82: goto _LL84;_LL83: if(*((int*)_tmpF8)!= 14)goto _LL85;_LL84: return(void*)
2;_LL85: if((int)_tmpF8 != 3)goto _LL87;_LL86: return(void*)4;_LL87: if((int)_tmpF8 != 
2)goto _LL89;_LL88: return(void*)3;_LL89: if(_tmpF8 <= (void*)4)goto _LL8B;if(*((int*)
_tmpF8)!= 2)goto _LL8B;_tmpFC=((struct Cyc_Absyn_TunionType_struct*)_tmpF8)->f1;
_tmpFD=_tmpFC.tunion_info;if((((((struct Cyc_Absyn_TunionType_struct*)_tmpF8)->f1).tunion_info).UnknownTunion).tag
!= 0)goto _LL8B;_tmpFE=(_tmpFD.UnknownTunion).f1;_LL8A: if(_tmpFE.is_flat)return(
void*)1;else{return(void*)2;}_LL8B: if(_tmpF8 <= (void*)4)goto _LL8D;if(*((int*)
_tmpF8)!= 2)goto _LL8D;_tmpFF=((struct Cyc_Absyn_TunionType_struct*)_tmpF8)->f1;
_tmp100=_tmpFF.tunion_info;if((((((struct Cyc_Absyn_TunionType_struct*)_tmpF8)->f1).tunion_info).KnownTunion).tag
!= 1)goto _LL8D;_tmp101=(_tmp100.KnownTunion).f1;_tmp102=*_tmp101;_LL8C: if(
_tmp102->is_flat)return(void*)1;else{return(void*)2;}_LL8D: if(_tmpF8 <= (void*)4)
goto _LL8F;if(*((int*)_tmpF8)!= 3)goto _LL8F;_tmp103=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmpF8)->f1;_tmp104=_tmp103.field_info;if((((((struct Cyc_Absyn_TunionFieldType_struct*)
_tmpF8)->f1).field_info).KnownTunionfield).tag != 1)goto _LL8F;_tmp105=(_tmp104.KnownTunionfield).f1;
_tmp106=(_tmp104.KnownTunionfield).f2;_LL8E: if(_tmp105->is_flat)return(void*)1;
else{if(_tmp106->typs == 0)return(void*)2;else{return(void*)1;}}_LL8F: if(_tmpF8 <= (
void*)4)goto _LL91;if(*((int*)_tmpF8)!= 3)goto _LL91;_tmp107=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmpF8)->f1;_tmp108=_tmp107.field_info;if((((((struct Cyc_Absyn_TunionFieldType_struct*)
_tmpF8)->f1).field_info).UnknownTunionfield).tag != 0)goto _LL91;_LL90: {const char*
_tmpC26;void*_tmpC25;(_tmpC25=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpC26="typ_kind: Unresolved TunionFieldType",
_tag_dyneither(_tmpC26,sizeof(char),37))),_tag_dyneither(_tmpC25,sizeof(void*),0)));}
_LL91: if(_tmpF8 <= (void*)4)goto _LL93;if(*((int*)_tmpF8)!= 12)goto _LL93;_tmp109=((
struct Cyc_Absyn_EnumType_struct*)_tmpF8)->f2;if(_tmp109 != 0)goto _LL93;_LL92: goto
_LL94;_LL93: if(_tmpF8 <= (void*)4)goto _LL95;if(*((int*)_tmpF8)!= 10)goto _LL95;
_tmp10A=((struct Cyc_Absyn_AggrType_struct*)_tmpF8)->f1;_tmp10B=_tmp10A.aggr_info;
if((((((struct Cyc_Absyn_AggrType_struct*)_tmpF8)->f1).aggr_info).UnknownAggr).tag
!= 0)goto _LL95;_LL94: return(void*)0;_LL95: if(_tmpF8 <= (void*)4)goto _LL97;if(*((
int*)_tmpF8)!= 10)goto _LL97;_tmp10C=((struct Cyc_Absyn_AggrType_struct*)_tmpF8)->f1;
_tmp10D=_tmp10C.aggr_info;if((((((struct Cyc_Absyn_AggrType_struct*)_tmpF8)->f1).aggr_info).KnownAggr).tag
!= 1)goto _LL97;_tmp10E=(_tmp10D.KnownAggr).f1;_tmp10F=*_tmp10E;_tmp110=*_tmp10F;
_tmp111=_tmp110.impl;_LL96: return _tmp111 == 0?(void*)0:(void*)1;_LL97: if(_tmpF8 <= (
void*)4)goto _LL99;if(*((int*)_tmpF8)!= 11)goto _LL99;_LL98: goto _LL9A;_LL99: if(
_tmpF8 <= (void*)4)goto _LL9B;if(*((int*)_tmpF8)!= 13)goto _LL9B;_LL9A: return(void*)
1;_LL9B: if(_tmpF8 <= (void*)4)goto _LL9D;if(*((int*)_tmpF8)!= 12)goto _LL9D;_tmp112=((
struct Cyc_Absyn_EnumType_struct*)_tmpF8)->f2;_LL9C: if(_tmp112->fields == 0)return(
void*)0;else{return(void*)1;}_LL9D: if(_tmpF8 <= (void*)4)goto _LL9F;if(*((int*)
_tmpF8)!= 4)goto _LL9F;_tmp113=((struct Cyc_Absyn_PointerType_struct*)_tmpF8)->f1;
_LL9E: {union Cyc_Absyn_Constraint_union _tmp117=(Cyc_Absyn_compress_conref((
_tmp113.ptr_atts).bounds))->v;void*_tmp118;void*_tmp119;_LLB0: if((_tmp117.No_constr).tag
!= 2)goto _LLB2;_LLB1: goto _LLB3;_LLB2: if((_tmp117.Eq_constr).tag != 0)goto _LLB4;
_tmp118=(_tmp117.Eq_constr).f1;if((int)_tmp118 != 0)goto _LLB4;_LLB3: return(void*)
1;_LLB4: if((_tmp117.Eq_constr).tag != 0)goto _LLB6;_tmp119=(_tmp117.Eq_constr).f1;
if(_tmp119 <= (void*)1)goto _LLB6;if(*((int*)_tmp119)!= 0)goto _LLB6;_LLB5: return(
void*)2;_LLB6: if((_tmp117.Forward_constr).tag != 1)goto _LLAF;_LLB7: {const char*
_tmpC29;void*_tmpC28;(_tmpC28=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpC29="typ_kind: forward constr in ptr bounds",
_tag_dyneither(_tmpC29,sizeof(char),39))),_tag_dyneither(_tmpC28,sizeof(void*),0)));}
_LLAF:;}_LL9F: if(_tmpF8 <= (void*)4)goto _LLA1;if(*((int*)_tmpF8)!= 17)goto _LLA1;
_LLA0: return(void*)7;_LLA1: if(_tmpF8 <= (void*)4)goto _LLA3;if(*((int*)_tmpF8)!= 
18)goto _LLA3;_LLA2: return(void*)2;_LLA3: if(_tmpF8 <= (void*)4)goto _LLA5;if(*((int*)
_tmpF8)!= 7)goto _LLA5;_LLA4: goto _LLA6;_LLA5: if(_tmpF8 <= (void*)4)goto _LLA7;if(*((
int*)_tmpF8)!= 9)goto _LLA7;_LLA6: return(void*)1;_LLA7: if(_tmpF8 <= (void*)4)goto
_LLA9;if(*((int*)_tmpF8)!= 16)goto _LLA9;_tmp114=((struct Cyc_Absyn_TypedefType_struct*)
_tmpF8)->f3;_LLA8: if(_tmp114 == 0  || _tmp114->kind == 0){const char*_tmpC2D;void*
_tmpC2C[1];struct Cyc_String_pa_struct _tmpC2B;(_tmpC2B.tag=0,((_tmpC2B.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpC2C[0]=&
_tmpC2B,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmpC2D="typ_kind: typedef found: %s",_tag_dyneither(_tmpC2D,sizeof(char),28))),
_tag_dyneither(_tmpC2C,sizeof(void*),1)))))));}return(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp114->kind))->v;_LLA9: if(_tmpF8 <= (void*)4)goto _LLAB;if(*((int*)
_tmpF8)!= 19)goto _LLAB;_LLAA: goto _LLAC;_LLAB: if(_tmpF8 <= (void*)4)goto _LLAD;if(*((
int*)_tmpF8)!= 20)goto _LLAD;_LLAC: goto _LLAE;_LLAD: if(_tmpF8 <= (void*)4)goto _LL72;
if(*((int*)_tmpF8)!= 21)goto _LL72;_LLAE: return(void*)6;_LL72:;}int Cyc_Tcutil_unify(
void*t1,void*t2);int Cyc_Tcutil_unify(void*t1,void*t2){struct _handler_cons _tmp11F;
_push_handler(& _tmp11F);{int _tmp121=0;if(setjmp(_tmp11F.handler))_tmp121=1;if(!
_tmp121){Cyc_Tcutil_unify_it(t1,t2);{int _tmp122=1;_npop_handler(0);return _tmp122;};
_pop_handler();}else{void*_tmp120=(void*)_exn_thrown;void*_tmp124=_tmp120;_LLB9:
if(_tmp124 != Cyc_Tcutil_Unify)goto _LLBB;_LLBA: return 0;_LLBB:;_LLBC:(void)_throw(
_tmp124);_LLB8:;}}}static void Cyc_Tcutil_occurslist(void*evar,struct _RegionHandle*
r,struct Cyc_List_List*env,struct Cyc_List_List*ts);static void Cyc_Tcutil_occurs(
void*evar,struct _RegionHandle*r,struct Cyc_List_List*env,void*t);static void Cyc_Tcutil_occurs(
void*evar,struct _RegionHandle*r,struct Cyc_List_List*env,void*t){t=Cyc_Tcutil_compress(
t);{void*_tmp125=t;struct Cyc_Absyn_Tvar*_tmp126;struct Cyc_Core_Opt*_tmp127;
struct Cyc_Core_Opt*_tmp128;struct Cyc_Core_Opt**_tmp129;struct Cyc_Absyn_PtrInfo
_tmp12A;struct Cyc_Absyn_ArrayInfo _tmp12B;void*_tmp12C;struct Cyc_Absyn_FnInfo
_tmp12D;struct Cyc_List_List*_tmp12E;struct Cyc_Core_Opt*_tmp12F;void*_tmp130;
struct Cyc_List_List*_tmp131;int _tmp132;struct Cyc_Absyn_VarargInfo*_tmp133;struct
Cyc_List_List*_tmp134;struct Cyc_List_List*_tmp135;struct Cyc_List_List*_tmp136;
struct Cyc_Absyn_TunionInfo _tmp137;struct Cyc_List_List*_tmp138;struct Cyc_Core_Opt*
_tmp139;struct Cyc_List_List*_tmp13A;struct Cyc_Absyn_TunionFieldInfo _tmp13B;
struct Cyc_List_List*_tmp13C;struct Cyc_Absyn_AggrInfo _tmp13D;struct Cyc_List_List*
_tmp13E;struct Cyc_List_List*_tmp13F;void*_tmp140;void*_tmp141;void*_tmp142;void*
_tmp143;struct Cyc_List_List*_tmp144;_LLBE: if(_tmp125 <= (void*)4)goto _LLDE;if(*((
int*)_tmp125)!= 1)goto _LLC0;_tmp126=((struct Cyc_Absyn_VarType_struct*)_tmp125)->f1;
_LLBF: if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,
env,_tmp126)){{const char*_tmpC2E;Cyc_Tcutil_failure_reason=((_tmpC2E="(type variable would escape scope)",
_tag_dyneither(_tmpC2E,sizeof(char),35)));}(int)_throw((void*)Cyc_Tcutil_Unify);}
goto _LLBD;_LLC0: if(*((int*)_tmp125)!= 0)goto _LLC2;_tmp127=((struct Cyc_Absyn_Evar_struct*)
_tmp125)->f2;_tmp128=((struct Cyc_Absyn_Evar_struct*)_tmp125)->f4;_tmp129=(struct
Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)_tmp125)->f4;_LLC1: if(t == evar){{
const char*_tmpC2F;Cyc_Tcutil_failure_reason=((_tmpC2F="(occurs check)",
_tag_dyneither(_tmpC2F,sizeof(char),15)));}(int)_throw((void*)Cyc_Tcutil_Unify);}
else{if(_tmp127 != 0)Cyc_Tcutil_occurs(evar,r,env,(void*)_tmp127->v);else{int
problem=0;{struct Cyc_List_List*s=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(*_tmp129))->v;for(0;s != 0;s=s->tl){if(!((int(*)(int(*compare)(struct
Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*
x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,(struct Cyc_Absyn_Tvar*)s->hd)){
problem=1;break;}}}if(problem){struct Cyc_List_List*_tmp147=0;{struct Cyc_List_List*
s=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*_tmp129))->v;for(0;s
!= 0;s=s->tl){if(((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,
env,(struct Cyc_Absyn_Tvar*)s->hd)){struct Cyc_List_List*_tmpC30;_tmp147=((_tmpC30=
_cycalloc(sizeof(*_tmpC30)),((_tmpC30->hd=(struct Cyc_Absyn_Tvar*)s->hd,((_tmpC30->tl=
_tmp147,_tmpC30))))));}}}{struct Cyc_Core_Opt*_tmpC31;*_tmp129=((_tmpC31=
_cycalloc(sizeof(*_tmpC31)),((_tmpC31->v=_tmp147,_tmpC31))));}}}}goto _LLBD;_LLC2:
if(*((int*)_tmp125)!= 4)goto _LLC4;_tmp12A=((struct Cyc_Absyn_PointerType_struct*)
_tmp125)->f1;_LLC3: Cyc_Tcutil_occurs(evar,r,env,(void*)_tmp12A.elt_typ);Cyc_Tcutil_occurs(
evar,r,env,(void*)(_tmp12A.ptr_atts).rgn);goto _LLBD;_LLC4: if(*((int*)_tmp125)!= 
7)goto _LLC6;_tmp12B=((struct Cyc_Absyn_ArrayType_struct*)_tmp125)->f1;_tmp12C=(
void*)_tmp12B.elt_type;_LLC5: Cyc_Tcutil_occurs(evar,r,env,_tmp12C);goto _LLBD;
_LLC6: if(*((int*)_tmp125)!= 8)goto _LLC8;_tmp12D=((struct Cyc_Absyn_FnType_struct*)
_tmp125)->f1;_tmp12E=_tmp12D.tvars;_tmp12F=_tmp12D.effect;_tmp130=(void*)_tmp12D.ret_typ;
_tmp131=_tmp12D.args;_tmp132=_tmp12D.c_varargs;_tmp133=_tmp12D.cyc_varargs;
_tmp134=_tmp12D.rgn_po;_tmp135=_tmp12D.attributes;_LLC7: env=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(
r,_tmp12E,env);if(_tmp12F != 0)Cyc_Tcutil_occurs(evar,r,env,(void*)_tmp12F->v);
Cyc_Tcutil_occurs(evar,r,env,_tmp130);for(0;_tmp131 != 0;_tmp131=_tmp131->tl){Cyc_Tcutil_occurs(
evar,r,env,(*((struct _tuple3*)_tmp131->hd)).f3);}if(_tmp133 != 0)Cyc_Tcutil_occurs(
evar,r,env,(void*)_tmp133->type);for(0;_tmp134 != 0;_tmp134=_tmp134->tl){struct
_tuple0 _tmp14B;void*_tmp14C;void*_tmp14D;struct _tuple0*_tmp14A=(struct _tuple0*)
_tmp134->hd;_tmp14B=*_tmp14A;_tmp14C=_tmp14B.f1;_tmp14D=_tmp14B.f2;Cyc_Tcutil_occurs(
evar,r,env,_tmp14C);Cyc_Tcutil_occurs(evar,r,env,_tmp14D);}goto _LLBD;_LLC8: if(*((
int*)_tmp125)!= 9)goto _LLCA;_tmp136=((struct Cyc_Absyn_TupleType_struct*)_tmp125)->f1;
_LLC9: for(0;_tmp136 != 0;_tmp136=_tmp136->tl){Cyc_Tcutil_occurs(evar,r,env,(*((
struct _tuple5*)_tmp136->hd)).f2);}goto _LLBD;_LLCA: if(*((int*)_tmp125)!= 2)goto
_LLCC;_tmp137=((struct Cyc_Absyn_TunionType_struct*)_tmp125)->f1;_tmp138=_tmp137.targs;
_tmp139=_tmp137.rgn;_LLCB: if((unsigned int)_tmp139)Cyc_Tcutil_occurs(evar,r,env,(
void*)_tmp139->v);Cyc_Tcutil_occurslist(evar,r,env,_tmp138);goto _LLBD;_LLCC: if(*((
int*)_tmp125)!= 16)goto _LLCE;_tmp13A=((struct Cyc_Absyn_TypedefType_struct*)
_tmp125)->f2;_LLCD: _tmp13C=_tmp13A;goto _LLCF;_LLCE: if(*((int*)_tmp125)!= 3)goto
_LLD0;_tmp13B=((struct Cyc_Absyn_TunionFieldType_struct*)_tmp125)->f1;_tmp13C=
_tmp13B.targs;_LLCF: _tmp13E=_tmp13C;goto _LLD1;_LLD0: if(*((int*)_tmp125)!= 10)
goto _LLD2;_tmp13D=((struct Cyc_Absyn_AggrType_struct*)_tmp125)->f1;_tmp13E=
_tmp13D.targs;_LLD1: Cyc_Tcutil_occurslist(evar,r,env,_tmp13E);goto _LLBD;_LLD2:
if(*((int*)_tmp125)!= 11)goto _LLD4;_tmp13F=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp125)->f2;_LLD3: for(0;_tmp13F != 0;_tmp13F=_tmp13F->tl){Cyc_Tcutil_occurs(evar,
r,env,(void*)((struct Cyc_Absyn_Aggrfield*)_tmp13F->hd)->type);}goto _LLBD;_LLD4:
if(*((int*)_tmp125)!= 18)goto _LLD6;_tmp140=(void*)((struct Cyc_Absyn_TagType_struct*)
_tmp125)->f1;_LLD5: _tmp141=_tmp140;goto _LLD7;_LLD6: if(*((int*)_tmp125)!= 19)goto
_LLD8;_tmp141=(void*)((struct Cyc_Absyn_AccessEff_struct*)_tmp125)->f1;_LLD7:
_tmp142=_tmp141;goto _LLD9;_LLD8: if(*((int*)_tmp125)!= 14)goto _LLDA;_tmp142=(void*)((
struct Cyc_Absyn_RgnHandleType_struct*)_tmp125)->f1;_LLD9: _tmp143=_tmp142;goto
_LLDB;_LLDA: if(*((int*)_tmp125)!= 21)goto _LLDC;_tmp143=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp125)->f1;_LLDB: Cyc_Tcutil_occurs(evar,r,env,_tmp143);goto _LLBD;_LLDC: if(*((
int*)_tmp125)!= 20)goto _LLDE;_tmp144=((struct Cyc_Absyn_JoinEff_struct*)_tmp125)->f1;
_LLDD: Cyc_Tcutil_occurslist(evar,r,env,_tmp144);goto _LLBD;_LLDE:;_LLDF: goto _LLBD;
_LLBD:;}}static void Cyc_Tcutil_occurslist(void*evar,struct _RegionHandle*r,struct
Cyc_List_List*env,struct Cyc_List_List*ts);static void Cyc_Tcutil_occurslist(void*
evar,struct _RegionHandle*r,struct Cyc_List_List*env,struct Cyc_List_List*ts){for(0;
ts != 0;ts=ts->tl){Cyc_Tcutil_occurs(evar,r,env,(void*)ts->hd);}}static void Cyc_Tcutil_unify_list(
struct Cyc_List_List*t1,struct Cyc_List_List*t2);static void Cyc_Tcutil_unify_list(
struct Cyc_List_List*t1,struct Cyc_List_List*t2){for(0;t1 != 0  && t2 != 0;(t1=t1->tl,
t2=t2->tl)){Cyc_Tcutil_unify_it((void*)t1->hd,(void*)t2->hd);}if(t1 != 0  || t2 != 
0)(int)_throw((void*)Cyc_Tcutil_Unify);}static void Cyc_Tcutil_unify_tqual(struct
Cyc_Absyn_Tqual tq1,void*t1,struct Cyc_Absyn_Tqual tq2,void*t2);static void Cyc_Tcutil_unify_tqual(
struct Cyc_Absyn_Tqual tq1,void*t1,struct Cyc_Absyn_Tqual tq2,void*t2){if(tq1.print_const
 && !tq1.real_const){const char*_tmpC34;void*_tmpC33;(_tmpC33=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpC34="tq1 real_const not set.",
_tag_dyneither(_tmpC34,sizeof(char),24))),_tag_dyneither(_tmpC33,sizeof(void*),0)));}
if(tq2.print_const  && !tq2.real_const){const char*_tmpC37;void*_tmpC36;(_tmpC36=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmpC37="tq2 real_const not set.",_tag_dyneither(_tmpC37,sizeof(char),24))),
_tag_dyneither(_tmpC36,sizeof(void*),0)));}if((tq1.real_const != tq2.real_const
 || tq1.q_volatile != tq2.q_volatile) || tq1.q_restrict != tq2.q_restrict){Cyc_Tcutil_t1_failure=
t1;Cyc_Tcutil_t2_failure=t2;Cyc_Tcutil_tq1_const=tq1.real_const;Cyc_Tcutil_tq2_const=
tq2.real_const;{const char*_tmpC38;Cyc_Tcutil_failure_reason=((_tmpC38="(qualifiers don't match)",
_tag_dyneither(_tmpC38,sizeof(char),25)));}(int)_throw((void*)Cyc_Tcutil_Unify);}
Cyc_Tcutil_tq1_const=0;Cyc_Tcutil_tq2_const=0;}int Cyc_Tcutil_equal_tqual(struct
Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2);int Cyc_Tcutil_equal_tqual(struct Cyc_Absyn_Tqual
tq1,struct Cyc_Absyn_Tqual tq2){return(tq1.real_const == tq2.real_const  && tq1.q_volatile
== tq2.q_volatile) && tq1.q_restrict == tq2.q_restrict;}static void Cyc_Tcutil_unify_it_conrefs(
int(*cmp)(void*,void*),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y,struct
_dyneither_ptr reason);static void Cyc_Tcutil_unify_it_conrefs(int(*cmp)(void*,void*),
struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y,struct _dyneither_ptr reason){x=
Cyc_Absyn_compress_conref(x);y=Cyc_Absyn_compress_conref(y);if(x == y)return;{
union Cyc_Absyn_Constraint_union _tmp153=x->v;void*_tmp154;_LLE1: if((_tmp153.No_constr).tag
!= 2)goto _LLE3;_LLE2:{union Cyc_Absyn_Constraint_union _tmpC39;x->v=(union Cyc_Absyn_Constraint_union)(((
_tmpC39.Forward_constr).tag=1,(((_tmpC39.Forward_constr).f1=y,_tmpC39))));}
return;_LLE3: if((_tmp153.Eq_constr).tag != 0)goto _LLE5;_tmp154=(_tmp153.Eq_constr).f1;
_LLE4: {union Cyc_Absyn_Constraint_union _tmp156=y->v;void*_tmp157;_LLE8: if((
_tmp156.No_constr).tag != 2)goto _LLEA;_LLE9: y->v=x->v;return;_LLEA: if((_tmp156.Eq_constr).tag
!= 0)goto _LLEC;_tmp157=(_tmp156.Eq_constr).f1;_LLEB: if(cmp(_tmp154,_tmp157)!= 0){
Cyc_Tcutil_failure_reason=reason;(int)_throw((void*)Cyc_Tcutil_Unify);}return;
_LLEC: if((_tmp156.Forward_constr).tag != 1)goto _LLE7;_LLED: {const char*_tmpC3C;
void*_tmpC3B;(_tmpC3B=0,Cyc_Tcutil_impos(((_tmpC3C="unify_conref: forward after compress(2)",
_tag_dyneither(_tmpC3C,sizeof(char),40))),_tag_dyneither(_tmpC3B,sizeof(void*),0)));}
_LLE7:;}_LLE5: if((_tmp153.Forward_constr).tag != 1)goto _LLE0;_LLE6: {const char*
_tmpC3F;void*_tmpC3E;(_tmpC3E=0,Cyc_Tcutil_impos(((_tmpC3F="unify_conref: forward after compress",
_tag_dyneither(_tmpC3F,sizeof(char),37))),_tag_dyneither(_tmpC3E,sizeof(void*),0)));}
_LLE0:;}}static int Cyc_Tcutil_unify_conrefs(int(*cmp)(void*,void*),struct Cyc_Absyn_Conref*
x,struct Cyc_Absyn_Conref*y);static int Cyc_Tcutil_unify_conrefs(int(*cmp)(void*,
void*),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y){struct _handler_cons
_tmp15C;_push_handler(& _tmp15C);{int _tmp15E=0;if(setjmp(_tmp15C.handler))_tmp15E=
1;if(!_tmp15E){Cyc_Tcutil_unify_it_conrefs(cmp,x,y,(struct _dyneither_ptr)
_tag_dyneither(0,0,0));{int _tmp15F=1;_npop_handler(0);return _tmp15F;};
_pop_handler();}else{void*_tmp15D=(void*)_exn_thrown;void*_tmp161=_tmp15D;_LLEF:
if(_tmp161 != Cyc_Tcutil_Unify)goto _LLF1;_LLF0: return 0;_LLF1:;_LLF2:(void)_throw(
_tmp161);_LLEE:;}}}static int Cyc_Tcutil_boundscmp(void*b1,void*b2);static int Cyc_Tcutil_boundscmp(
void*b1,void*b2){struct _tuple0 _tmpC40;struct _tuple0 _tmp163=(_tmpC40.f1=b1,((
_tmpC40.f2=b2,_tmpC40)));void*_tmp164;void*_tmp165;void*_tmp166;void*_tmp167;
void*_tmp168;struct Cyc_Absyn_Exp*_tmp169;void*_tmp16A;struct Cyc_Absyn_Exp*
_tmp16B;_LLF4: _tmp164=_tmp163.f1;if((int)_tmp164 != 0)goto _LLF6;_tmp165=_tmp163.f2;
if((int)_tmp165 != 0)goto _LLF6;_LLF5: return 0;_LLF6: _tmp166=_tmp163.f1;if((int)
_tmp166 != 0)goto _LLF8;_LLF7: return - 1;_LLF8: _tmp167=_tmp163.f2;if((int)_tmp167 != 
0)goto _LLFA;_LLF9: return 1;_LLFA: _tmp168=_tmp163.f1;if(_tmp168 <= (void*)1)goto
_LLF3;if(*((int*)_tmp168)!= 0)goto _LLF3;_tmp169=((struct Cyc_Absyn_Upper_b_struct*)
_tmp168)->f1;_tmp16A=_tmp163.f2;if(_tmp16A <= (void*)1)goto _LLF3;if(*((int*)
_tmp16A)!= 0)goto _LLF3;_tmp16B=((struct Cyc_Absyn_Upper_b_struct*)_tmp16A)->f1;
_LLFB: return Cyc_Evexp_const_exp_cmp(_tmp169,_tmp16B);_LLF3:;}static int Cyc_Tcutil_unify_it_bounds(
void*b1,void*b2);static int Cyc_Tcutil_unify_it_bounds(void*b1,void*b2){struct
_tuple0 _tmpC41;struct _tuple0 _tmp16D=(_tmpC41.f1=b1,((_tmpC41.f2=b2,_tmpC41)));
void*_tmp16E;void*_tmp16F;void*_tmp170;void*_tmp171;void*_tmp172;struct Cyc_Absyn_Exp*
_tmp173;void*_tmp174;struct Cyc_Absyn_Exp*_tmp175;_LLFD: _tmp16E=_tmp16D.f1;if((
int)_tmp16E != 0)goto _LLFF;_tmp16F=_tmp16D.f2;if((int)_tmp16F != 0)goto _LLFF;_LLFE:
return 0;_LLFF: _tmp170=_tmp16D.f1;if((int)_tmp170 != 0)goto _LL101;_LL100: return - 1;
_LL101: _tmp171=_tmp16D.f2;if((int)_tmp171 != 0)goto _LL103;_LL102: return 1;_LL103:
_tmp172=_tmp16D.f1;if(_tmp172 <= (void*)1)goto _LLFC;if(*((int*)_tmp172)!= 0)goto
_LLFC;_tmp173=((struct Cyc_Absyn_Upper_b_struct*)_tmp172)->f1;_tmp174=_tmp16D.f2;
if(_tmp174 <= (void*)1)goto _LLFC;if(*((int*)_tmp174)!= 0)goto _LLFC;_tmp175=((
struct Cyc_Absyn_Upper_b_struct*)_tmp174)->f1;_LL104: return Cyc_Evexp_const_exp_cmp(
_tmp173,_tmp175);_LLFC:;}static int Cyc_Tcutil_attribute_case_number(void*att);
static int Cyc_Tcutil_attribute_case_number(void*att){void*_tmp176=att;_LL106: if(
_tmp176 <= (void*)17)goto _LL108;if(*((int*)_tmp176)!= 0)goto _LL108;_LL107: return 0;
_LL108: if((int)_tmp176 != 0)goto _LL10A;_LL109: return 1;_LL10A: if((int)_tmp176 != 1)
goto _LL10C;_LL10B: return 2;_LL10C: if((int)_tmp176 != 2)goto _LL10E;_LL10D: return 3;
_LL10E: if((int)_tmp176 != 3)goto _LL110;_LL10F: return 4;_LL110: if((int)_tmp176 != 4)
goto _LL112;_LL111: return 5;_LL112: if(_tmp176 <= (void*)17)goto _LL114;if(*((int*)
_tmp176)!= 1)goto _LL114;_LL113: return 6;_LL114: if((int)_tmp176 != 5)goto _LL116;
_LL115: return 7;_LL116: if(_tmp176 <= (void*)17)goto _LL118;if(*((int*)_tmp176)!= 2)
goto _LL118;_LL117: return 8;_LL118: if((int)_tmp176 != 6)goto _LL11A;_LL119: return 9;
_LL11A: if((int)_tmp176 != 7)goto _LL11C;_LL11B: return 10;_LL11C: if((int)_tmp176 != 8)
goto _LL11E;_LL11D: return 11;_LL11E: if((int)_tmp176 != 9)goto _LL120;_LL11F: return 12;
_LL120: if((int)_tmp176 != 10)goto _LL122;_LL121: return 13;_LL122: if((int)_tmp176 != 
11)goto _LL124;_LL123: return 14;_LL124: if((int)_tmp176 != 12)goto _LL126;_LL125:
return 15;_LL126: if((int)_tmp176 != 13)goto _LL128;_LL127: return 16;_LL128: if((int)
_tmp176 != 14)goto _LL12A;_LL129: return 17;_LL12A: if((int)_tmp176 != 15)goto _LL12C;
_LL12B: return 18;_LL12C: if(_tmp176 <= (void*)17)goto _LL130;if(*((int*)_tmp176)!= 3)
goto _LL12E;_LL12D: return 19;_LL12E: if(*((int*)_tmp176)!= 4)goto _LL130;_LL12F:
return 20;_LL130:;_LL131: return 21;_LL105:;}static int Cyc_Tcutil_attribute_cmp(void*
att1,void*att2);static int Cyc_Tcutil_attribute_cmp(void*att1,void*att2){struct
_tuple0 _tmpC42;struct _tuple0 _tmp178=(_tmpC42.f1=att1,((_tmpC42.f2=att2,_tmpC42)));
void*_tmp179;int _tmp17A;void*_tmp17B;int _tmp17C;void*_tmp17D;int _tmp17E;void*
_tmp17F;int _tmp180;void*_tmp181;int _tmp182;void*_tmp183;int _tmp184;void*_tmp185;
struct _dyneither_ptr _tmp186;void*_tmp187;struct _dyneither_ptr _tmp188;void*
_tmp189;void*_tmp18A;int _tmp18B;int _tmp18C;void*_tmp18D;void*_tmp18E;int _tmp18F;
int _tmp190;_LL133: _tmp179=_tmp178.f1;if(_tmp179 <= (void*)17)goto _LL135;if(*((int*)
_tmp179)!= 0)goto _LL135;_tmp17A=((struct Cyc_Absyn_Regparm_att_struct*)_tmp179)->f1;
_tmp17B=_tmp178.f2;if(_tmp17B <= (void*)17)goto _LL135;if(*((int*)_tmp17B)!= 0)
goto _LL135;_tmp17C=((struct Cyc_Absyn_Regparm_att_struct*)_tmp17B)->f1;_LL134:
_tmp17E=_tmp17A;_tmp180=_tmp17C;goto _LL136;_LL135: _tmp17D=_tmp178.f1;if(_tmp17D
<= (void*)17)goto _LL137;if(*((int*)_tmp17D)!= 4)goto _LL137;_tmp17E=((struct Cyc_Absyn_Initializes_att_struct*)
_tmp17D)->f1;_tmp17F=_tmp178.f2;if(_tmp17F <= (void*)17)goto _LL137;if(*((int*)
_tmp17F)!= 4)goto _LL137;_tmp180=((struct Cyc_Absyn_Initializes_att_struct*)
_tmp17F)->f1;_LL136: _tmp182=_tmp17E;_tmp184=_tmp180;goto _LL138;_LL137: _tmp181=
_tmp178.f1;if(_tmp181 <= (void*)17)goto _LL139;if(*((int*)_tmp181)!= 1)goto _LL139;
_tmp182=((struct Cyc_Absyn_Aligned_att_struct*)_tmp181)->f1;_tmp183=_tmp178.f2;
if(_tmp183 <= (void*)17)goto _LL139;if(*((int*)_tmp183)!= 1)goto _LL139;_tmp184=((
struct Cyc_Absyn_Aligned_att_struct*)_tmp183)->f1;_LL138: return Cyc_Core_intcmp(
_tmp182,_tmp184);_LL139: _tmp185=_tmp178.f1;if(_tmp185 <= (void*)17)goto _LL13B;if(*((
int*)_tmp185)!= 2)goto _LL13B;_tmp186=((struct Cyc_Absyn_Section_att_struct*)
_tmp185)->f1;_tmp187=_tmp178.f2;if(_tmp187 <= (void*)17)goto _LL13B;if(*((int*)
_tmp187)!= 2)goto _LL13B;_tmp188=((struct Cyc_Absyn_Section_att_struct*)_tmp187)->f1;
_LL13A: return Cyc_strcmp((struct _dyneither_ptr)_tmp186,(struct _dyneither_ptr)
_tmp188);_LL13B: _tmp189=_tmp178.f1;if(_tmp189 <= (void*)17)goto _LL13D;if(*((int*)
_tmp189)!= 3)goto _LL13D;_tmp18A=(void*)((struct Cyc_Absyn_Format_att_struct*)
_tmp189)->f1;_tmp18B=((struct Cyc_Absyn_Format_att_struct*)_tmp189)->f2;_tmp18C=((
struct Cyc_Absyn_Format_att_struct*)_tmp189)->f3;_tmp18D=_tmp178.f2;if(_tmp18D <= (
void*)17)goto _LL13D;if(*((int*)_tmp18D)!= 3)goto _LL13D;_tmp18E=(void*)((struct
Cyc_Absyn_Format_att_struct*)_tmp18D)->f1;_tmp18F=((struct Cyc_Absyn_Format_att_struct*)
_tmp18D)->f2;_tmp190=((struct Cyc_Absyn_Format_att_struct*)_tmp18D)->f3;_LL13C: {
int _tmp191=Cyc_Core_intcmp((int)((unsigned int)_tmp18A),(int)((unsigned int)
_tmp18E));if(_tmp191 != 0)return _tmp191;{int _tmp192=Cyc_Core_intcmp(_tmp18B,
_tmp18F);if(_tmp192 != 0)return _tmp192;return Cyc_Core_intcmp(_tmp18C,_tmp190);}}
_LL13D:;_LL13E: return Cyc_Core_intcmp(Cyc_Tcutil_attribute_case_number(att1),Cyc_Tcutil_attribute_case_number(
att2));_LL132:;}static int Cyc_Tcutil_equal_att(void*a1,void*a2);static int Cyc_Tcutil_equal_att(
void*a1,void*a2){return Cyc_Tcutil_attribute_cmp(a1,a2)== 0;}int Cyc_Tcutil_same_atts(
struct Cyc_List_List*a1,struct Cyc_List_List*a2);int Cyc_Tcutil_same_atts(struct Cyc_List_List*
a1,struct Cyc_List_List*a2){{struct Cyc_List_List*a=a1;for(0;a != 0;a=a->tl){if(!
Cyc_List_exists_c(Cyc_Tcutil_equal_att,(void*)a->hd,a2))return 0;}}{struct Cyc_List_List*
a=a2;for(0;a != 0;a=a->tl){if(!Cyc_List_exists_c(Cyc_Tcutil_equal_att,(void*)a->hd,
a1))return 0;}}return 1;}static void*Cyc_Tcutil_rgns_of(void*t);static void*Cyc_Tcutil_rgns_of_field(
struct Cyc_Absyn_Aggrfield*af);static void*Cyc_Tcutil_rgns_of_field(struct Cyc_Absyn_Aggrfield*
af){return Cyc_Tcutil_rgns_of((void*)af->type);}static struct _tuple8*Cyc_Tcutil_region_free_subst(
struct Cyc_Absyn_Tvar*tv);static struct _tuple8*Cyc_Tcutil_region_free_subst(struct
Cyc_Absyn_Tvar*tv){void*t;{void*_tmp193=Cyc_Tcutil_tvar_kind(tv);_LL140: if((int)
_tmp193 != 4)goto _LL142;_LL141: t=(void*)3;goto _LL13F;_LL142: if((int)_tmp193 != 3)
goto _LL144;_LL143: t=(void*)2;goto _LL13F;_LL144: if((int)_tmp193 != 6)goto _LL146;
_LL145: t=Cyc_Absyn_empty_effect;goto _LL13F;_LL146: if((int)_tmp193 != 7)goto _LL148;
_LL147:{struct Cyc_Absyn_ValueofType_struct _tmpC45;struct Cyc_Absyn_ValueofType_struct*
_tmpC44;t=(void*)((_tmpC44=_cycalloc(sizeof(*_tmpC44)),((_tmpC44[0]=((_tmpC45.tag=
17,((_tmpC45.f1=Cyc_Absyn_uint_exp(0,0),_tmpC45)))),_tmpC44))));}goto _LL13F;
_LL148:;_LL149: t=Cyc_Absyn_sint_typ;goto _LL13F;_LL13F:;}{struct _tuple8*_tmpC46;
return(_tmpC46=_cycalloc(sizeof(*_tmpC46)),((_tmpC46->f1=tv,((_tmpC46->f2=t,
_tmpC46)))));}}static void*Cyc_Tcutil_rgns_of(void*t);static void*Cyc_Tcutil_rgns_of(
void*t){void*_tmp197=Cyc_Tcutil_compress(t);void*_tmp198;void*_tmp199;void*
_tmp19A;struct Cyc_Absyn_TunionInfo _tmp19B;struct Cyc_List_List*_tmp19C;struct Cyc_Core_Opt*
_tmp19D;struct Cyc_Absyn_PtrInfo _tmp19E;void*_tmp19F;struct Cyc_Absyn_PtrAtts
_tmp1A0;void*_tmp1A1;struct Cyc_Absyn_ArrayInfo _tmp1A2;void*_tmp1A3;struct Cyc_List_List*
_tmp1A4;struct Cyc_Absyn_TunionFieldInfo _tmp1A5;struct Cyc_List_List*_tmp1A6;
struct Cyc_Absyn_AggrInfo _tmp1A7;struct Cyc_List_List*_tmp1A8;struct Cyc_List_List*
_tmp1A9;struct Cyc_Absyn_FnInfo _tmp1AA;struct Cyc_List_List*_tmp1AB;struct Cyc_Core_Opt*
_tmp1AC;void*_tmp1AD;struct Cyc_List_List*_tmp1AE;struct Cyc_Absyn_VarargInfo*
_tmp1AF;struct Cyc_List_List*_tmp1B0;void*_tmp1B1;struct Cyc_List_List*_tmp1B2;
_LL14B: if((int)_tmp197 != 0)goto _LL14D;_LL14C: goto _LL14E;_LL14D: if((int)_tmp197 != 
1)goto _LL14F;_LL14E: goto _LL150;_LL14F: if(_tmp197 <= (void*)4)goto _LL173;if(*((int*)
_tmp197)!= 6)goto _LL151;_LL150: goto _LL152;_LL151: if(*((int*)_tmp197)!= 12)goto
_LL153;_LL152: goto _LL154;_LL153: if(*((int*)_tmp197)!= 13)goto _LL155;_LL154: goto
_LL156;_LL155: if(*((int*)_tmp197)!= 17)goto _LL157;_LL156: goto _LL158;_LL157: if(*((
int*)_tmp197)!= 5)goto _LL159;_LL158: return Cyc_Absyn_empty_effect;_LL159: if(*((
int*)_tmp197)!= 0)goto _LL15B;_LL15A: goto _LL15C;_LL15B: if(*((int*)_tmp197)!= 1)
goto _LL15D;_LL15C: {void*_tmp1B3=Cyc_Tcutil_typ_kind(t);_LL180: if((int)_tmp1B3 != 
3)goto _LL182;_LL181: goto _LL183;_LL182: if((int)_tmp1B3 != 4)goto _LL184;_LL183: goto
_LL185;_LL184: if((int)_tmp1B3 != 5)goto _LL186;_LL185: {struct Cyc_Absyn_AccessEff_struct
_tmpC49;struct Cyc_Absyn_AccessEff_struct*_tmpC48;return(void*)((_tmpC48=
_cycalloc(sizeof(*_tmpC48)),((_tmpC48[0]=((_tmpC49.tag=19,((_tmpC49.f1=(void*)t,
_tmpC49)))),_tmpC48))));}_LL186: if((int)_tmp1B3 != 6)goto _LL188;_LL187: return t;
_LL188: if((int)_tmp1B3 != 7)goto _LL18A;_LL189: return Cyc_Absyn_empty_effect;_LL18A:;
_LL18B: {struct Cyc_Absyn_RgnsEff_struct _tmpC4C;struct Cyc_Absyn_RgnsEff_struct*
_tmpC4B;return(void*)((_tmpC4B=_cycalloc(sizeof(*_tmpC4B)),((_tmpC4B[0]=((
_tmpC4C.tag=21,((_tmpC4C.f1=(void*)t,_tmpC4C)))),_tmpC4B))));}_LL17F:;}_LL15D:
if(*((int*)_tmp197)!= 14)goto _LL15F;_tmp198=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp197)->f1;_LL15E: {struct Cyc_Absyn_AccessEff_struct _tmpC4F;struct Cyc_Absyn_AccessEff_struct*
_tmpC4E;return(void*)((_tmpC4E=_cycalloc(sizeof(*_tmpC4E)),((_tmpC4E[0]=((
_tmpC4F.tag=19,((_tmpC4F.f1=(void*)_tmp198,_tmpC4F)))),_tmpC4E))));}_LL15F: if(*((
int*)_tmp197)!= 15)goto _LL161;_tmp199=(void*)((struct Cyc_Absyn_DynRgnType_struct*)
_tmp197)->f1;_tmp19A=(void*)((struct Cyc_Absyn_DynRgnType_struct*)_tmp197)->f2;
_LL160: {struct Cyc_Absyn_AccessEff_struct _tmpC52;struct Cyc_Absyn_AccessEff_struct*
_tmpC51;return(void*)((_tmpC51=_cycalloc(sizeof(*_tmpC51)),((_tmpC51[0]=((
_tmpC52.tag=19,((_tmpC52.f1=(void*)_tmp19A,_tmpC52)))),_tmpC51))));}_LL161: if(*((
int*)_tmp197)!= 2)goto _LL163;_tmp19B=((struct Cyc_Absyn_TunionType_struct*)
_tmp197)->f1;_tmp19C=_tmp19B.targs;_tmp19D=_tmp19B.rgn;_LL162: {struct Cyc_List_List*
ts=Cyc_List_map(Cyc_Tcutil_rgns_of,_tmp19C);if((unsigned int)_tmp19D){struct Cyc_Absyn_AccessEff_struct*
_tmpC58;struct Cyc_Absyn_AccessEff_struct _tmpC57;struct Cyc_List_List*_tmpC56;ts=((
_tmpC56=_cycalloc(sizeof(*_tmpC56)),((_tmpC56->hd=(void*)((void*)((_tmpC58=
_cycalloc(sizeof(*_tmpC58)),((_tmpC58[0]=((_tmpC57.tag=19,((_tmpC57.f1=(void*)((
void*)_tmp19D->v),_tmpC57)))),_tmpC58))))),((_tmpC56->tl=ts,_tmpC56))))));}{
struct Cyc_Absyn_JoinEff_struct _tmpC5B;struct Cyc_Absyn_JoinEff_struct*_tmpC5A;
return Cyc_Tcutil_normalize_effect((void*)((_tmpC5A=_cycalloc(sizeof(*_tmpC5A)),((
_tmpC5A[0]=((_tmpC5B.tag=20,((_tmpC5B.f1=ts,_tmpC5B)))),_tmpC5A)))));}}_LL163:
if(*((int*)_tmp197)!= 4)goto _LL165;_tmp19E=((struct Cyc_Absyn_PointerType_struct*)
_tmp197)->f1;_tmp19F=(void*)_tmp19E.elt_typ;_tmp1A0=_tmp19E.ptr_atts;_tmp1A1=(
void*)_tmp1A0.rgn;_LL164: {struct Cyc_Absyn_JoinEff_struct _tmpC6A;struct Cyc_Absyn_AccessEff_struct*
_tmpC69;struct Cyc_Absyn_AccessEff_struct _tmpC68;void*_tmpC67[2];struct Cyc_Absyn_JoinEff_struct*
_tmpC66;return Cyc_Tcutil_normalize_effect((void*)((_tmpC66=_cycalloc(sizeof(*
_tmpC66)),((_tmpC66[0]=((_tmpC6A.tag=20,((_tmpC6A.f1=((_tmpC67[1]=Cyc_Tcutil_rgns_of(
_tmp19F),((_tmpC67[0]=(void*)((_tmpC69=_cycalloc(sizeof(*_tmpC69)),((_tmpC69[0]=((
_tmpC68.tag=19,((_tmpC68.f1=(void*)_tmp1A1,_tmpC68)))),_tmpC69)))),Cyc_List_list(
_tag_dyneither(_tmpC67,sizeof(void*),2)))))),_tmpC6A)))),_tmpC66)))));}_LL165:
if(*((int*)_tmp197)!= 7)goto _LL167;_tmp1A2=((struct Cyc_Absyn_ArrayType_struct*)
_tmp197)->f1;_tmp1A3=(void*)_tmp1A2.elt_type;_LL166: return Cyc_Tcutil_normalize_effect(
Cyc_Tcutil_rgns_of(_tmp1A3));_LL167: if(*((int*)_tmp197)!= 9)goto _LL169;_tmp1A4=((
struct Cyc_Absyn_TupleType_struct*)_tmp197)->f1;_LL168: {struct Cyc_List_List*
_tmp1C6=0;for(0;_tmp1A4 != 0;_tmp1A4=_tmp1A4->tl){struct Cyc_List_List*_tmpC6B;
_tmp1C6=((_tmpC6B=_cycalloc(sizeof(*_tmpC6B)),((_tmpC6B->hd=(void*)(*((struct
_tuple5*)_tmp1A4->hd)).f2,((_tmpC6B->tl=_tmp1C6,_tmpC6B))))));}_tmp1A6=_tmp1C6;
goto _LL16A;}_LL169: if(*((int*)_tmp197)!= 3)goto _LL16B;_tmp1A5=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp197)->f1;_tmp1A6=_tmp1A5.targs;_LL16A: _tmp1A8=_tmp1A6;goto _LL16C;_LL16B: if(*((
int*)_tmp197)!= 10)goto _LL16D;_tmp1A7=((struct Cyc_Absyn_AggrType_struct*)_tmp197)->f1;
_tmp1A8=_tmp1A7.targs;_LL16C: {struct Cyc_Absyn_JoinEff_struct _tmpC6E;struct Cyc_Absyn_JoinEff_struct*
_tmpC6D;return Cyc_Tcutil_normalize_effect((void*)((_tmpC6D=_cycalloc(sizeof(*
_tmpC6D)),((_tmpC6D[0]=((_tmpC6E.tag=20,((_tmpC6E.f1=Cyc_List_map(Cyc_Tcutil_rgns_of,
_tmp1A8),_tmpC6E)))),_tmpC6D)))));}_LL16D: if(*((int*)_tmp197)!= 11)goto _LL16F;
_tmp1A9=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp197)->f2;_LL16E: {struct Cyc_Absyn_JoinEff_struct
_tmpC71;struct Cyc_Absyn_JoinEff_struct*_tmpC70;return Cyc_Tcutil_normalize_effect((
void*)((_tmpC70=_cycalloc(sizeof(*_tmpC70)),((_tmpC70[0]=((_tmpC71.tag=20,((
_tmpC71.f1=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Aggrfield*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of_field,_tmp1A9),_tmpC71)))),
_tmpC70)))));}_LL16F: if(*((int*)_tmp197)!= 18)goto _LL171;_LL170: return Cyc_Absyn_empty_effect;
_LL171: if(*((int*)_tmp197)!= 8)goto _LL173;_tmp1AA=((struct Cyc_Absyn_FnType_struct*)
_tmp197)->f1;_tmp1AB=_tmp1AA.tvars;_tmp1AC=_tmp1AA.effect;_tmp1AD=(void*)_tmp1AA.ret_typ;
_tmp1AE=_tmp1AA.args;_tmp1AF=_tmp1AA.cyc_varargs;_tmp1B0=_tmp1AA.rgn_po;_LL172: {
void*_tmp1CC=Cyc_Tcutil_substitute(((struct Cyc_List_List*(*)(struct _tuple8*(*f)(
struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_region_free_subst,
_tmp1AB),(void*)((struct Cyc_Core_Opt*)_check_null(_tmp1AC))->v);return Cyc_Tcutil_normalize_effect(
_tmp1CC);}_LL173: if((int)_tmp197 != 3)goto _LL175;_LL174: goto _LL176;_LL175: if((int)
_tmp197 != 2)goto _LL177;_LL176: return Cyc_Absyn_empty_effect;_LL177: if(_tmp197 <= (
void*)4)goto _LL179;if(*((int*)_tmp197)!= 19)goto _LL179;_LL178: goto _LL17A;_LL179:
if(_tmp197 <= (void*)4)goto _LL17B;if(*((int*)_tmp197)!= 20)goto _LL17B;_LL17A:
return t;_LL17B: if(_tmp197 <= (void*)4)goto _LL17D;if(*((int*)_tmp197)!= 21)goto
_LL17D;_tmp1B1=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp197)->f1;_LL17C:
return Cyc_Tcutil_rgns_of(_tmp1B1);_LL17D: if(_tmp197 <= (void*)4)goto _LL14A;if(*((
int*)_tmp197)!= 16)goto _LL14A;_tmp1B2=((struct Cyc_Absyn_TypedefType_struct*)
_tmp197)->f2;_LL17E: {struct Cyc_Absyn_JoinEff_struct _tmpC74;struct Cyc_Absyn_JoinEff_struct*
_tmpC73;return Cyc_Tcutil_normalize_effect((void*)((_tmpC73=_cycalloc(sizeof(*
_tmpC73)),((_tmpC73[0]=((_tmpC74.tag=20,((_tmpC74.f1=Cyc_List_map(Cyc_Tcutil_rgns_of,
_tmp1B2),_tmpC74)))),_tmpC73)))));}_LL14A:;}void*Cyc_Tcutil_normalize_effect(
void*e);void*Cyc_Tcutil_normalize_effect(void*e){e=Cyc_Tcutil_compress(e);{void*
_tmp1CF=e;struct Cyc_List_List*_tmp1D0;struct Cyc_List_List**_tmp1D1;void*_tmp1D2;
_LL18D: if(_tmp1CF <= (void*)4)goto _LL191;if(*((int*)_tmp1CF)!= 20)goto _LL18F;
_tmp1D0=((struct Cyc_Absyn_JoinEff_struct*)_tmp1CF)->f1;_tmp1D1=(struct Cyc_List_List**)&((
struct Cyc_Absyn_JoinEff_struct*)_tmp1CF)->f1;_LL18E: {int redo_join=0;{struct Cyc_List_List*
effs=*_tmp1D1;for(0;effs != 0;effs=effs->tl){void*_tmp1D3=(void*)effs->hd;(void*)(
effs->hd=(void*)Cyc_Tcutil_compress(Cyc_Tcutil_normalize_effect(_tmp1D3)));{void*
_tmp1D4=(void*)effs->hd;void*_tmp1D5;_LL194: if(_tmp1D4 <= (void*)4)goto _LL198;if(*((
int*)_tmp1D4)!= 20)goto _LL196;_LL195: goto _LL197;_LL196: if(*((int*)_tmp1D4)!= 19)
goto _LL198;_tmp1D5=(void*)((struct Cyc_Absyn_AccessEff_struct*)_tmp1D4)->f1;if((
int)_tmp1D5 != 2)goto _LL198;_LL197: redo_join=1;goto _LL193;_LL198:;_LL199: goto
_LL193;_LL193:;}}}if(!redo_join)return e;{struct Cyc_List_List*effects=0;{struct
Cyc_List_List*effs=*_tmp1D1;for(0;effs != 0;effs=effs->tl){void*_tmp1D6=Cyc_Tcutil_compress((
void*)effs->hd);struct Cyc_List_List*_tmp1D7;void*_tmp1D8;_LL19B: if(_tmp1D6 <= (
void*)4)goto _LL19F;if(*((int*)_tmp1D6)!= 20)goto _LL19D;_tmp1D7=((struct Cyc_Absyn_JoinEff_struct*)
_tmp1D6)->f1;_LL19C: effects=Cyc_List_revappend(_tmp1D7,effects);goto _LL19A;
_LL19D: if(*((int*)_tmp1D6)!= 19)goto _LL19F;_tmp1D8=(void*)((struct Cyc_Absyn_AccessEff_struct*)
_tmp1D6)->f1;if((int)_tmp1D8 != 2)goto _LL19F;_LL19E: goto _LL19A;_LL19F:;_LL1A0:{
struct Cyc_List_List*_tmpC75;effects=((_tmpC75=_cycalloc(sizeof(*_tmpC75)),((
_tmpC75->hd=(void*)_tmp1D6,((_tmpC75->tl=effects,_tmpC75))))));}goto _LL19A;
_LL19A:;}}*_tmp1D1=Cyc_List_imp_rev(effects);return e;}}_LL18F: if(*((int*)_tmp1CF)
!= 21)goto _LL191;_tmp1D2=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp1CF)->f1;
_LL190: {void*_tmp1DA=Cyc_Tcutil_compress(_tmp1D2);_LL1A2: if(_tmp1DA <= (void*)4)
goto _LL1A6;if(*((int*)_tmp1DA)!= 0)goto _LL1A4;_LL1A3: goto _LL1A5;_LL1A4: if(*((int*)
_tmp1DA)!= 1)goto _LL1A6;_LL1A5: return e;_LL1A6:;_LL1A7: return Cyc_Tcutil_rgns_of(
_tmp1D2);_LL1A1:;}_LL191:;_LL192: return e;_LL18C:;}}static void*Cyc_Tcutil_dummy_fntype(
void*eff);static void*Cyc_Tcutil_dummy_fntype(void*eff){struct Cyc_Absyn_FnType_struct
_tmpC7F;struct Cyc_Core_Opt*_tmpC7E;struct Cyc_Absyn_FnInfo _tmpC7D;struct Cyc_Absyn_FnType_struct*
_tmpC7C;struct Cyc_Absyn_FnType_struct*_tmp1DB=(_tmpC7C=_cycalloc(sizeof(*_tmpC7C)),((
_tmpC7C[0]=((_tmpC7F.tag=8,((_tmpC7F.f1=((_tmpC7D.tvars=0,((_tmpC7D.effect=((
_tmpC7E=_cycalloc(sizeof(*_tmpC7E)),((_tmpC7E->v=(void*)eff,_tmpC7E)))),((
_tmpC7D.ret_typ=(void*)((void*)0),((_tmpC7D.args=0,((_tmpC7D.c_varargs=0,((
_tmpC7D.cyc_varargs=0,((_tmpC7D.rgn_po=0,((_tmpC7D.attributes=0,_tmpC7D)))))))))))))))),
_tmpC7F)))),_tmpC7C)));return Cyc_Absyn_atb_typ((void*)_tmp1DB,(void*)2,Cyc_Absyn_empty_tqual(
0),Cyc_Absyn_bounds_one,Cyc_Absyn_false_conref);}int Cyc_Tcutil_region_in_effect(
int constrain,void*r,void*e);int Cyc_Tcutil_region_in_effect(int constrain,void*r,
void*e){r=Cyc_Tcutil_compress(r);if(r == (void*)2  || r == (void*)3)return 1;{void*
_tmp1E0=Cyc_Tcutil_compress(e);void*_tmp1E1;struct Cyc_List_List*_tmp1E2;void*
_tmp1E3;struct Cyc_Core_Opt*_tmp1E4;struct Cyc_Core_Opt*_tmp1E5;struct Cyc_Core_Opt**
_tmp1E6;struct Cyc_Core_Opt*_tmp1E7;_LL1A9: if(_tmp1E0 <= (void*)4)goto _LL1B1;if(*((
int*)_tmp1E0)!= 19)goto _LL1AB;_tmp1E1=(void*)((struct Cyc_Absyn_AccessEff_struct*)
_tmp1E0)->f1;_LL1AA: if(constrain)return Cyc_Tcutil_unify(r,_tmp1E1);_tmp1E1=Cyc_Tcutil_compress(
_tmp1E1);if(r == _tmp1E1)return 1;{struct _tuple0 _tmpC80;struct _tuple0 _tmp1E9=(
_tmpC80.f1=r,((_tmpC80.f2=_tmp1E1,_tmpC80)));void*_tmp1EA;struct Cyc_Absyn_Tvar*
_tmp1EB;void*_tmp1EC;struct Cyc_Absyn_Tvar*_tmp1ED;_LL1B4: _tmp1EA=_tmp1E9.f1;if(
_tmp1EA <= (void*)4)goto _LL1B6;if(*((int*)_tmp1EA)!= 1)goto _LL1B6;_tmp1EB=((
struct Cyc_Absyn_VarType_struct*)_tmp1EA)->f1;_tmp1EC=_tmp1E9.f2;if(_tmp1EC <= (
void*)4)goto _LL1B6;if(*((int*)_tmp1EC)!= 1)goto _LL1B6;_tmp1ED=((struct Cyc_Absyn_VarType_struct*)
_tmp1EC)->f1;_LL1B5: return Cyc_Absyn_tvar_cmp(_tmp1EB,_tmp1ED)== 0;_LL1B6:;_LL1B7:
return 0;_LL1B3:;}_LL1AB: if(*((int*)_tmp1E0)!= 20)goto _LL1AD;_tmp1E2=((struct Cyc_Absyn_JoinEff_struct*)
_tmp1E0)->f1;_LL1AC: for(0;_tmp1E2 != 0;_tmp1E2=_tmp1E2->tl){if(Cyc_Tcutil_region_in_effect(
constrain,r,(void*)_tmp1E2->hd))return 1;}return 0;_LL1AD: if(*((int*)_tmp1E0)!= 21)
goto _LL1AF;_tmp1E3=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp1E0)->f1;_LL1AE: {
void*_tmp1EE=Cyc_Tcutil_rgns_of(_tmp1E3);void*_tmp1EF;_LL1B9: if(_tmp1EE <= (void*)
4)goto _LL1BB;if(*((int*)_tmp1EE)!= 21)goto _LL1BB;_tmp1EF=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp1EE)->f1;_LL1BA: if(!constrain)return 0;{void*_tmp1F0=Cyc_Tcutil_compress(
_tmp1EF);struct Cyc_Core_Opt*_tmp1F1;struct Cyc_Core_Opt*_tmp1F2;struct Cyc_Core_Opt**
_tmp1F3;struct Cyc_Core_Opt*_tmp1F4;_LL1BE: if(_tmp1F0 <= (void*)4)goto _LL1C0;if(*((
int*)_tmp1F0)!= 0)goto _LL1C0;_tmp1F1=((struct Cyc_Absyn_Evar_struct*)_tmp1F0)->f1;
_tmp1F2=((struct Cyc_Absyn_Evar_struct*)_tmp1F0)->f2;_tmp1F3=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Evar_struct*)_tmp1F0)->f2;_tmp1F4=((struct Cyc_Absyn_Evar_struct*)
_tmp1F0)->f4;_LL1BF: {void*_tmp1F5=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,
_tmp1F4);Cyc_Tcutil_occurs(_tmp1F5,Cyc_Core_heap_region,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp1F4))->v,r);{struct Cyc_Absyn_JoinEff_struct
_tmpC8F;struct Cyc_Absyn_AccessEff_struct*_tmpC8E;struct Cyc_Absyn_AccessEff_struct
_tmpC8D;void*_tmpC8C[2];struct Cyc_Absyn_JoinEff_struct*_tmpC8B;void*_tmp1F6=Cyc_Tcutil_dummy_fntype((
void*)((_tmpC8B=_cycalloc(sizeof(*_tmpC8B)),((_tmpC8B[0]=((_tmpC8F.tag=20,((
_tmpC8F.f1=((_tmpC8C[1]=(void*)((_tmpC8E=_cycalloc(sizeof(*_tmpC8E)),((_tmpC8E[0]=((
_tmpC8D.tag=19,((_tmpC8D.f1=(void*)r,_tmpC8D)))),_tmpC8E)))),((_tmpC8C[0]=
_tmp1F5,Cyc_List_list(_tag_dyneither(_tmpC8C,sizeof(void*),2)))))),_tmpC8F)))),
_tmpC8B)))));{struct Cyc_Core_Opt*_tmpC90;*_tmp1F3=((_tmpC90=_cycalloc(sizeof(*
_tmpC90)),((_tmpC90->v=(void*)_tmp1F6,_tmpC90))));}return 1;}}_LL1C0:;_LL1C1:
return 0;_LL1BD:;}_LL1BB:;_LL1BC: return Cyc_Tcutil_region_in_effect(constrain,r,
_tmp1EE);_LL1B8:;}_LL1AF: if(*((int*)_tmp1E0)!= 0)goto _LL1B1;_tmp1E4=((struct Cyc_Absyn_Evar_struct*)
_tmp1E0)->f1;_tmp1E5=((struct Cyc_Absyn_Evar_struct*)_tmp1E0)->f2;_tmp1E6=(struct
Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)_tmp1E0)->f2;_tmp1E7=((struct Cyc_Absyn_Evar_struct*)
_tmp1E0)->f4;_LL1B0: if(_tmp1E4 == 0  || (void*)_tmp1E4->v != (void*)6){const char*
_tmpC93;void*_tmpC92;(_tmpC92=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpC93="effect evar has wrong kind",
_tag_dyneither(_tmpC93,sizeof(char),27))),_tag_dyneither(_tmpC92,sizeof(void*),0)));}
if(!constrain)return 0;{void*_tmp1FF=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,
_tmp1E7);Cyc_Tcutil_occurs(_tmp1FF,Cyc_Core_heap_region,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp1E7))->v,r);{struct Cyc_Absyn_JoinEff_struct
_tmpCA8;struct Cyc_List_List*_tmpCA7;struct Cyc_Absyn_AccessEff_struct _tmpCA6;
struct Cyc_Absyn_AccessEff_struct*_tmpCA5;struct Cyc_List_List*_tmpCA4;struct Cyc_Absyn_JoinEff_struct*
_tmpCA3;struct Cyc_Absyn_JoinEff_struct*_tmp200=(_tmpCA3=_cycalloc(sizeof(*
_tmpCA3)),((_tmpCA3[0]=((_tmpCA8.tag=20,((_tmpCA8.f1=((_tmpCA4=_cycalloc(sizeof(*
_tmpCA4)),((_tmpCA4->hd=(void*)_tmp1FF,((_tmpCA4->tl=((_tmpCA7=_cycalloc(sizeof(*
_tmpCA7)),((_tmpCA7->hd=(void*)((void*)((_tmpCA5=_cycalloc(sizeof(*_tmpCA5)),((
_tmpCA5[0]=((_tmpCA6.tag=19,((_tmpCA6.f1=(void*)r,_tmpCA6)))),_tmpCA5))))),((
_tmpCA7->tl=0,_tmpCA7)))))),_tmpCA4)))))),_tmpCA8)))),_tmpCA3)));{struct Cyc_Core_Opt*
_tmpCA9;*_tmp1E6=((_tmpCA9=_cycalloc(sizeof(*_tmpCA9)),((_tmpCA9->v=(void*)((
void*)_tmp200),_tmpCA9))));}return 1;}}_LL1B1:;_LL1B2: return 0;_LL1A8:;}}static int
Cyc_Tcutil_type_in_effect(int may_constrain_evars,void*t,void*e);static int Cyc_Tcutil_type_in_effect(
int may_constrain_evars,void*t,void*e){t=Cyc_Tcutil_compress(t);{void*_tmp208=Cyc_Tcutil_normalize_effect(
Cyc_Tcutil_compress(e));struct Cyc_List_List*_tmp209;void*_tmp20A;struct Cyc_Core_Opt*
_tmp20B;struct Cyc_Core_Opt*_tmp20C;struct Cyc_Core_Opt**_tmp20D;struct Cyc_Core_Opt*
_tmp20E;_LL1C3: if(_tmp208 <= (void*)4)goto _LL1CB;if(*((int*)_tmp208)!= 19)goto
_LL1C5;_LL1C4: return 0;_LL1C5: if(*((int*)_tmp208)!= 20)goto _LL1C7;_tmp209=((
struct Cyc_Absyn_JoinEff_struct*)_tmp208)->f1;_LL1C6: for(0;_tmp209 != 0;_tmp209=
_tmp209->tl){if(Cyc_Tcutil_type_in_effect(may_constrain_evars,t,(void*)_tmp209->hd))
return 1;}return 0;_LL1C7: if(*((int*)_tmp208)!= 21)goto _LL1C9;_tmp20A=(void*)((
struct Cyc_Absyn_RgnsEff_struct*)_tmp208)->f1;_LL1C8: _tmp20A=Cyc_Tcutil_compress(
_tmp20A);if(t == _tmp20A)return 1;if(may_constrain_evars)return Cyc_Tcutil_unify(t,
_tmp20A);{void*_tmp20F=Cyc_Tcutil_rgns_of(t);void*_tmp210;_LL1CE: if(_tmp20F <= (
void*)4)goto _LL1D0;if(*((int*)_tmp20F)!= 21)goto _LL1D0;_tmp210=(void*)((struct
Cyc_Absyn_RgnsEff_struct*)_tmp20F)->f1;_LL1CF: {struct _tuple0 _tmpCAA;struct
_tuple0 _tmp212=(_tmpCAA.f1=Cyc_Tcutil_compress(_tmp210),((_tmpCAA.f2=_tmp20A,
_tmpCAA)));void*_tmp213;struct Cyc_Absyn_Tvar*_tmp214;void*_tmp215;struct Cyc_Absyn_Tvar*
_tmp216;_LL1D3: _tmp213=_tmp212.f1;if(_tmp213 <= (void*)4)goto _LL1D5;if(*((int*)
_tmp213)!= 1)goto _LL1D5;_tmp214=((struct Cyc_Absyn_VarType_struct*)_tmp213)->f1;
_tmp215=_tmp212.f2;if(_tmp215 <= (void*)4)goto _LL1D5;if(*((int*)_tmp215)!= 1)goto
_LL1D5;_tmp216=((struct Cyc_Absyn_VarType_struct*)_tmp215)->f1;_LL1D4: return Cyc_Tcutil_unify(
t,_tmp20A);_LL1D5:;_LL1D6: return _tmp210 == _tmp20A;_LL1D2:;}_LL1D0:;_LL1D1: return
Cyc_Tcutil_type_in_effect(may_constrain_evars,t,_tmp20F);_LL1CD:;}_LL1C9: if(*((
int*)_tmp208)!= 0)goto _LL1CB;_tmp20B=((struct Cyc_Absyn_Evar_struct*)_tmp208)->f1;
_tmp20C=((struct Cyc_Absyn_Evar_struct*)_tmp208)->f2;_tmp20D=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Evar_struct*)_tmp208)->f2;_tmp20E=((struct Cyc_Absyn_Evar_struct*)
_tmp208)->f4;_LL1CA: if(_tmp20B == 0  || (void*)_tmp20B->v != (void*)6){const char*
_tmpCAD;void*_tmpCAC;(_tmpCAC=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpCAD="effect evar has wrong kind",
_tag_dyneither(_tmpCAD,sizeof(char),27))),_tag_dyneither(_tmpCAC,sizeof(void*),0)));}
if(!may_constrain_evars)return 0;{void*_tmp219=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_ek,_tmp20E);Cyc_Tcutil_occurs(_tmp219,Cyc_Core_heap_region,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp20E))->v,t);{struct Cyc_Absyn_JoinEff_struct
_tmpCC2;struct Cyc_List_List*_tmpCC1;struct Cyc_Absyn_RgnsEff_struct _tmpCC0;struct
Cyc_Absyn_RgnsEff_struct*_tmpCBF;struct Cyc_List_List*_tmpCBE;struct Cyc_Absyn_JoinEff_struct*
_tmpCBD;struct Cyc_Absyn_JoinEff_struct*_tmp21A=(_tmpCBD=_cycalloc(sizeof(*
_tmpCBD)),((_tmpCBD[0]=((_tmpCC2.tag=20,((_tmpCC2.f1=((_tmpCBE=_cycalloc(sizeof(*
_tmpCBE)),((_tmpCBE->hd=(void*)_tmp219,((_tmpCBE->tl=((_tmpCC1=_cycalloc(sizeof(*
_tmpCC1)),((_tmpCC1->hd=(void*)((void*)((_tmpCBF=_cycalloc(sizeof(*_tmpCBF)),((
_tmpCBF[0]=((_tmpCC0.tag=21,((_tmpCC0.f1=(void*)t,_tmpCC0)))),_tmpCBF))))),((
_tmpCC1->tl=0,_tmpCC1)))))),_tmpCBE)))))),_tmpCC2)))),_tmpCBD)));{struct Cyc_Core_Opt*
_tmpCC3;*_tmp20D=((_tmpCC3=_cycalloc(sizeof(*_tmpCC3)),((_tmpCC3->v=(void*)((
void*)_tmp21A),_tmpCC3))));}return 1;}}_LL1CB:;_LL1CC: return 0;_LL1C2:;}}static int
Cyc_Tcutil_variable_in_effect(int may_constrain_evars,struct Cyc_Absyn_Tvar*v,void*
e);static int Cyc_Tcutil_variable_in_effect(int may_constrain_evars,struct Cyc_Absyn_Tvar*
v,void*e){e=Cyc_Tcutil_compress(e);{void*_tmp222=e;struct Cyc_Absyn_Tvar*_tmp223;
struct Cyc_List_List*_tmp224;void*_tmp225;struct Cyc_Core_Opt*_tmp226;struct Cyc_Core_Opt*
_tmp227;struct Cyc_Core_Opt**_tmp228;struct Cyc_Core_Opt*_tmp229;_LL1D8: if(_tmp222
<= (void*)4)goto _LL1E0;if(*((int*)_tmp222)!= 1)goto _LL1DA;_tmp223=((struct Cyc_Absyn_VarType_struct*)
_tmp222)->f1;_LL1D9: return Cyc_Absyn_tvar_cmp(v,_tmp223)== 0;_LL1DA: if(*((int*)
_tmp222)!= 20)goto _LL1DC;_tmp224=((struct Cyc_Absyn_JoinEff_struct*)_tmp222)->f1;
_LL1DB: for(0;_tmp224 != 0;_tmp224=_tmp224->tl){if(Cyc_Tcutil_variable_in_effect(
may_constrain_evars,v,(void*)_tmp224->hd))return 1;}return 0;_LL1DC: if(*((int*)
_tmp222)!= 21)goto _LL1DE;_tmp225=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp222)->f1;_LL1DD: {void*_tmp22A=Cyc_Tcutil_rgns_of(_tmp225);void*_tmp22B;
_LL1E3: if(_tmp22A <= (void*)4)goto _LL1E5;if(*((int*)_tmp22A)!= 21)goto _LL1E5;
_tmp22B=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp22A)->f1;_LL1E4: if(!
may_constrain_evars)return 0;{void*_tmp22C=Cyc_Tcutil_compress(_tmp22B);struct Cyc_Core_Opt*
_tmp22D;struct Cyc_Core_Opt*_tmp22E;struct Cyc_Core_Opt**_tmp22F;struct Cyc_Core_Opt*
_tmp230;_LL1E8: if(_tmp22C <= (void*)4)goto _LL1EA;if(*((int*)_tmp22C)!= 0)goto
_LL1EA;_tmp22D=((struct Cyc_Absyn_Evar_struct*)_tmp22C)->f1;_tmp22E=((struct Cyc_Absyn_Evar_struct*)
_tmp22C)->f2;_tmp22F=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)
_tmp22C)->f2;_tmp230=((struct Cyc_Absyn_Evar_struct*)_tmp22C)->f4;_LL1E9: {void*
_tmp231=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,_tmp230);if(!((
int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*
l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp230))->v,v))return 0;{struct Cyc_Absyn_JoinEff_struct
_tmpCD2;struct Cyc_Absyn_VarType_struct*_tmpCD1;struct Cyc_Absyn_VarType_struct
_tmpCD0;void*_tmpCCF[2];struct Cyc_Absyn_JoinEff_struct*_tmpCCE;void*_tmp232=Cyc_Tcutil_dummy_fntype((
void*)((_tmpCCE=_cycalloc(sizeof(*_tmpCCE)),((_tmpCCE[0]=((_tmpCD2.tag=20,((
_tmpCD2.f1=((_tmpCCF[1]=(void*)((_tmpCD1=_cycalloc(sizeof(*_tmpCD1)),((_tmpCD1[0]=((
_tmpCD0.tag=1,((_tmpCD0.f1=v,_tmpCD0)))),_tmpCD1)))),((_tmpCCF[0]=_tmp231,Cyc_List_list(
_tag_dyneither(_tmpCCF,sizeof(void*),2)))))),_tmpCD2)))),_tmpCCE)))));{struct Cyc_Core_Opt*
_tmpCD3;*_tmp22F=((_tmpCD3=_cycalloc(sizeof(*_tmpCD3)),((_tmpCD3->v=(void*)
_tmp232,_tmpCD3))));}return 1;}}_LL1EA:;_LL1EB: return 0;_LL1E7:;}_LL1E5:;_LL1E6:
return Cyc_Tcutil_variable_in_effect(may_constrain_evars,v,_tmp22A);_LL1E2:;}
_LL1DE: if(*((int*)_tmp222)!= 0)goto _LL1E0;_tmp226=((struct Cyc_Absyn_Evar_struct*)
_tmp222)->f1;_tmp227=((struct Cyc_Absyn_Evar_struct*)_tmp222)->f2;_tmp228=(struct
Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)_tmp222)->f2;_tmp229=((struct Cyc_Absyn_Evar_struct*)
_tmp222)->f4;_LL1DF: if(_tmp226 == 0  || (void*)_tmp226->v != (void*)6){const char*
_tmpCD6;void*_tmpCD5;(_tmpCD5=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpCD6="effect evar has wrong kind",
_tag_dyneither(_tmpCD6,sizeof(char),27))),_tag_dyneither(_tmpCD5,sizeof(void*),0)));}{
void*_tmp23B=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,_tmp229);if(
!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*
l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp229))->v,v))return 0;{struct Cyc_Absyn_JoinEff_struct
_tmpCEB;struct Cyc_List_List*_tmpCEA;struct Cyc_Absyn_VarType_struct _tmpCE9;struct
Cyc_Absyn_VarType_struct*_tmpCE8;struct Cyc_List_List*_tmpCE7;struct Cyc_Absyn_JoinEff_struct*
_tmpCE6;struct Cyc_Absyn_JoinEff_struct*_tmp23C=(_tmpCE6=_cycalloc(sizeof(*
_tmpCE6)),((_tmpCE6[0]=((_tmpCEB.tag=20,((_tmpCEB.f1=((_tmpCE7=_cycalloc(sizeof(*
_tmpCE7)),((_tmpCE7->hd=(void*)_tmp23B,((_tmpCE7->tl=((_tmpCEA=_cycalloc(sizeof(*
_tmpCEA)),((_tmpCEA->hd=(void*)((void*)((_tmpCE8=_cycalloc(sizeof(*_tmpCE8)),((
_tmpCE8[0]=((_tmpCE9.tag=1,((_tmpCE9.f1=v,_tmpCE9)))),_tmpCE8))))),((_tmpCEA->tl=
0,_tmpCEA)))))),_tmpCE7)))))),_tmpCEB)))),_tmpCE6)));{struct Cyc_Core_Opt*_tmpCEC;*
_tmp228=((_tmpCEC=_cycalloc(sizeof(*_tmpCEC)),((_tmpCEC->v=(void*)((void*)
_tmp23C),_tmpCEC))));}return 1;}}_LL1E0:;_LL1E1: return 0;_LL1D7:;}}static int Cyc_Tcutil_evar_in_effect(
void*evar,void*e);static int Cyc_Tcutil_evar_in_effect(void*evar,void*e){e=Cyc_Tcutil_compress(
e);{void*_tmp244=e;struct Cyc_List_List*_tmp245;void*_tmp246;_LL1ED: if(_tmp244 <= (
void*)4)goto _LL1F3;if(*((int*)_tmp244)!= 20)goto _LL1EF;_tmp245=((struct Cyc_Absyn_JoinEff_struct*)
_tmp244)->f1;_LL1EE: for(0;_tmp245 != 0;_tmp245=_tmp245->tl){if(Cyc_Tcutil_evar_in_effect(
evar,(void*)_tmp245->hd))return 1;}return 0;_LL1EF: if(*((int*)_tmp244)!= 21)goto
_LL1F1;_tmp246=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp244)->f1;_LL1F0: {
void*_tmp247=Cyc_Tcutil_rgns_of(_tmp246);void*_tmp248;_LL1F6: if(_tmp247 <= (void*)
4)goto _LL1F8;if(*((int*)_tmp247)!= 21)goto _LL1F8;_tmp248=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp247)->f1;_LL1F7: return 0;_LL1F8:;_LL1F9: return Cyc_Tcutil_evar_in_effect(evar,
_tmp247);_LL1F5:;}_LL1F1: if(*((int*)_tmp244)!= 0)goto _LL1F3;_LL1F2: return evar == 
e;_LL1F3:;_LL1F4: return 0;_LL1EC:;}}int Cyc_Tcutil_subset_effect(int
may_constrain_evars,void*e1,void*e2);int Cyc_Tcutil_subset_effect(int
may_constrain_evars,void*e1,void*e2){void*_tmp249=Cyc_Tcutil_compress(e1);struct
Cyc_List_List*_tmp24A;void*_tmp24B;struct Cyc_Absyn_Tvar*_tmp24C;void*_tmp24D;
struct Cyc_Core_Opt*_tmp24E;struct Cyc_Core_Opt**_tmp24F;struct Cyc_Core_Opt*
_tmp250;_LL1FB: if(_tmp249 <= (void*)4)goto _LL205;if(*((int*)_tmp249)!= 20)goto
_LL1FD;_tmp24A=((struct Cyc_Absyn_JoinEff_struct*)_tmp249)->f1;_LL1FC: for(0;
_tmp24A != 0;_tmp24A=_tmp24A->tl){if(!Cyc_Tcutil_subset_effect(
may_constrain_evars,(void*)_tmp24A->hd,e2))return 0;}return 1;_LL1FD: if(*((int*)
_tmp249)!= 19)goto _LL1FF;_tmp24B=(void*)((struct Cyc_Absyn_AccessEff_struct*)
_tmp249)->f1;_LL1FE: return Cyc_Tcutil_region_in_effect(0,_tmp24B,e2) || 
may_constrain_evars  && Cyc_Tcutil_unify(_tmp24B,(void*)2);_LL1FF: if(*((int*)
_tmp249)!= 1)goto _LL201;_tmp24C=((struct Cyc_Absyn_VarType_struct*)_tmp249)->f1;
_LL200: return Cyc_Tcutil_variable_in_effect(may_constrain_evars,_tmp24C,e2);
_LL201: if(*((int*)_tmp249)!= 21)goto _LL203;_tmp24D=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp249)->f1;_LL202: {void*_tmp251=Cyc_Tcutil_rgns_of(_tmp24D);void*_tmp252;
_LL208: if(_tmp251 <= (void*)4)goto _LL20A;if(*((int*)_tmp251)!= 21)goto _LL20A;
_tmp252=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp251)->f1;_LL209: return Cyc_Tcutil_type_in_effect(
may_constrain_evars,_tmp252,e2) || may_constrain_evars  && Cyc_Tcutil_unify(
_tmp252,Cyc_Absyn_sint_typ);_LL20A:;_LL20B: return Cyc_Tcutil_subset_effect(
may_constrain_evars,_tmp251,e2);_LL207:;}_LL203: if(*((int*)_tmp249)!= 0)goto
_LL205;_tmp24E=((struct Cyc_Absyn_Evar_struct*)_tmp249)->f2;_tmp24F=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Evar_struct*)_tmp249)->f2;_tmp250=((struct Cyc_Absyn_Evar_struct*)
_tmp249)->f4;_LL204: if(!Cyc_Tcutil_evar_in_effect(e1,e2)){struct Cyc_Core_Opt*
_tmpCED;*_tmp24F=((_tmpCED=_cycalloc(sizeof(*_tmpCED)),((_tmpCED->v=(void*)Cyc_Absyn_empty_effect,
_tmpCED))));}return 1;_LL205:;_LL206: {const char*_tmpCF1;void*_tmpCF0[1];struct
Cyc_String_pa_struct _tmpCEF;(_tmpCEF.tag=0,((_tmpCEF.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string(e1)),((_tmpCF0[0]=& _tmpCEF,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpCF1="subset_effect: bad effect: %s",
_tag_dyneither(_tmpCF1,sizeof(char),30))),_tag_dyneither(_tmpCF0,sizeof(void*),1)))))));}
_LL1FA:;}static int Cyc_Tcutil_unify_effect(void*e1,void*e2);static int Cyc_Tcutil_unify_effect(
void*e1,void*e2){e1=Cyc_Tcutil_normalize_effect(e1);e2=Cyc_Tcutil_normalize_effect(
e2);if(Cyc_Tcutil_subset_effect(0,e1,e2) && Cyc_Tcutil_subset_effect(0,e2,e1))
return 1;if(Cyc_Tcutil_subset_effect(1,e1,e2) && Cyc_Tcutil_subset_effect(1,e2,e1))
return 1;return 0;}static int Cyc_Tcutil_sub_rgnpo(struct Cyc_List_List*rpo1,struct
Cyc_List_List*rpo2);static int Cyc_Tcutil_sub_rgnpo(struct Cyc_List_List*rpo1,
struct Cyc_List_List*rpo2){{struct Cyc_List_List*r1=rpo1;for(0;r1 != 0;r1=r1->tl){
struct _tuple0 _tmp258;void*_tmp259;void*_tmp25A;struct _tuple0*_tmp257=(struct
_tuple0*)r1->hd;_tmp258=*_tmp257;_tmp259=_tmp258.f1;_tmp25A=_tmp258.f2;{int found=
_tmp259 == (void*)2;{struct Cyc_List_List*r2=rpo2;for(0;r2 != 0  && !found;r2=r2->tl){
struct _tuple0 _tmp25C;void*_tmp25D;void*_tmp25E;struct _tuple0*_tmp25B=(struct
_tuple0*)r2->hd;_tmp25C=*_tmp25B;_tmp25D=_tmp25C.f1;_tmp25E=_tmp25C.f2;if(Cyc_Tcutil_unify(
_tmp259,_tmp25D) && Cyc_Tcutil_unify(_tmp25A,_tmp25E)){found=1;break;}}}if(!
found)return 0;}}}return 1;}static int Cyc_Tcutil_same_rgn_po(struct Cyc_List_List*
rpo1,struct Cyc_List_List*rpo2);static int Cyc_Tcutil_same_rgn_po(struct Cyc_List_List*
rpo1,struct Cyc_List_List*rpo2){return Cyc_Tcutil_sub_rgnpo(rpo1,rpo2) && Cyc_Tcutil_sub_rgnpo(
rpo2,rpo1);}struct _tuple11{struct Cyc_Absyn_VarargInfo*f1;struct Cyc_Absyn_VarargInfo*
f2;};struct _tuple12{struct Cyc_Core_Opt*f1;struct Cyc_Core_Opt*f2;};void Cyc_Tcutil_unify_it(
void*t1,void*t2);void Cyc_Tcutil_unify_it(void*t1,void*t2){Cyc_Tcutil_t1_failure=
t1;Cyc_Tcutil_t2_failure=t2;Cyc_Tcutil_failure_reason=(struct _dyneither_ptr)
_tag_dyneither(0,0,0);t1=Cyc_Tcutil_compress(t1);t2=Cyc_Tcutil_compress(t2);if(
t1 == t2)return;{void*_tmp25F=t1;struct Cyc_Core_Opt*_tmp260;struct Cyc_Core_Opt*
_tmp261;struct Cyc_Core_Opt**_tmp262;struct Cyc_Core_Opt*_tmp263;_LL20D: if(_tmp25F
<= (void*)4)goto _LL20F;if(*((int*)_tmp25F)!= 0)goto _LL20F;_tmp260=((struct Cyc_Absyn_Evar_struct*)
_tmp25F)->f1;_tmp261=((struct Cyc_Absyn_Evar_struct*)_tmp25F)->f2;_tmp262=(struct
Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)_tmp25F)->f2;_tmp263=((struct Cyc_Absyn_Evar_struct*)
_tmp25F)->f4;_LL20E: Cyc_Tcutil_occurs(t1,Cyc_Core_heap_region,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp263))->v,t2);{void*_tmp264=Cyc_Tcutil_typ_kind(
t2);if(Cyc_Tcutil_kind_leq(_tmp264,(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp260))->v)){{struct Cyc_Core_Opt*_tmpCF2;*_tmp262=((_tmpCF2=_cycalloc(sizeof(*
_tmpCF2)),((_tmpCF2->v=(void*)t2,_tmpCF2))));}return;}else{{void*_tmp266=t2;
struct Cyc_Core_Opt*_tmp267;struct Cyc_Core_Opt**_tmp268;struct Cyc_Core_Opt*
_tmp269;struct Cyc_Absyn_PtrInfo _tmp26A;_LL212: if(_tmp266 <= (void*)4)goto _LL216;
if(*((int*)_tmp266)!= 0)goto _LL214;_tmp267=((struct Cyc_Absyn_Evar_struct*)
_tmp266)->f2;_tmp268=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)
_tmp266)->f2;_tmp269=((struct Cyc_Absyn_Evar_struct*)_tmp266)->f4;_LL213: {struct
Cyc_List_List*_tmp26B=(struct Cyc_List_List*)_tmp263->v;{struct Cyc_List_List*s2=(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp269))->v;for(0;s2 != 0;
s2=s2->tl){if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,
_tmp26B,(struct Cyc_Absyn_Tvar*)s2->hd)){{const char*_tmpCF3;Cyc_Tcutil_failure_reason=((
_tmpCF3="(type variable would escape scope)",_tag_dyneither(_tmpCF3,sizeof(char),
35)));}(int)_throw((void*)Cyc_Tcutil_Unify);}}}if(Cyc_Tcutil_kind_leq((void*)
_tmp260->v,_tmp264)){{struct Cyc_Core_Opt*_tmpCF4;*_tmp268=((_tmpCF4=_cycalloc(
sizeof(*_tmpCF4)),((_tmpCF4->v=(void*)t1,_tmpCF4))));}return;}{const char*_tmpCF5;
Cyc_Tcutil_failure_reason=((_tmpCF5="(kinds are incompatible)",_tag_dyneither(
_tmpCF5,sizeof(char),25)));}goto _LL211;}_LL214: if(*((int*)_tmp266)!= 4)goto
_LL216;_tmp26A=((struct Cyc_Absyn_PointerType_struct*)_tmp266)->f1;if(!((void*)
_tmp260->v == (void*)2))goto _LL216;_LL215: {struct Cyc_Absyn_Conref*_tmp26F=Cyc_Absyn_compress_conref((
_tmp26A.ptr_atts).bounds);{union Cyc_Absyn_Constraint_union _tmp270=_tmp26F->v;
_LL219: if((_tmp270.No_constr).tag != 2)goto _LL21B;_LL21A:{union Cyc_Absyn_Constraint_union
_tmpCF6;_tmp26F->v=(union Cyc_Absyn_Constraint_union)(((_tmpCF6.Eq_constr).tag=0,(((
_tmpCF6.Eq_constr).f1=(void*)Cyc_Absyn_bounds_one,_tmpCF6))));}{struct Cyc_Core_Opt*
_tmpCF7;*_tmp262=((_tmpCF7=_cycalloc(sizeof(*_tmpCF7)),((_tmpCF7->v=(void*)t2,
_tmpCF7))));}return;_LL21B:;_LL21C: goto _LL218;_LL218:;}goto _LL211;}_LL216:;
_LL217: goto _LL211;_LL211:;}{const char*_tmpCF8;Cyc_Tcutil_failure_reason=((
_tmpCF8="(kinds are incompatible)",_tag_dyneither(_tmpCF8,sizeof(char),25)));}(
int)_throw((void*)Cyc_Tcutil_Unify);}}_LL20F:;_LL210: goto _LL20C;_LL20C:;}{struct
_tuple0 _tmpCF9;struct _tuple0 _tmp275=(_tmpCF9.f1=t2,((_tmpCF9.f2=t1,_tmpCF9)));
void*_tmp276;void*_tmp277;void*_tmp278;void*_tmp279;struct Cyc_Absyn_Tvar*_tmp27A;
void*_tmp27B;struct Cyc_Absyn_Tvar*_tmp27C;void*_tmp27D;struct Cyc_Absyn_AggrInfo
_tmp27E;union Cyc_Absyn_AggrInfoU_union _tmp27F;struct Cyc_List_List*_tmp280;void*
_tmp281;struct Cyc_Absyn_AggrInfo _tmp282;union Cyc_Absyn_AggrInfoU_union _tmp283;
struct Cyc_List_List*_tmp284;void*_tmp285;struct _tuple2*_tmp286;void*_tmp287;
struct _tuple2*_tmp288;void*_tmp289;struct Cyc_List_List*_tmp28A;void*_tmp28B;
struct Cyc_List_List*_tmp28C;void*_tmp28D;struct Cyc_Absyn_TunionInfo _tmp28E;union
Cyc_Absyn_TunionInfoU_union _tmp28F;struct Cyc_Absyn_Tuniondecl**_tmp290;struct Cyc_Absyn_Tuniondecl*
_tmp291;struct Cyc_List_List*_tmp292;struct Cyc_Core_Opt*_tmp293;void*_tmp294;
struct Cyc_Absyn_TunionInfo _tmp295;union Cyc_Absyn_TunionInfoU_union _tmp296;struct
Cyc_Absyn_Tuniondecl**_tmp297;struct Cyc_Absyn_Tuniondecl*_tmp298;struct Cyc_List_List*
_tmp299;struct Cyc_Core_Opt*_tmp29A;void*_tmp29B;struct Cyc_Absyn_TunionFieldInfo
_tmp29C;union Cyc_Absyn_TunionFieldInfoU_union _tmp29D;struct Cyc_Absyn_Tuniondecl*
_tmp29E;struct Cyc_Absyn_Tunionfield*_tmp29F;struct Cyc_List_List*_tmp2A0;void*
_tmp2A1;struct Cyc_Absyn_TunionFieldInfo _tmp2A2;union Cyc_Absyn_TunionFieldInfoU_union
_tmp2A3;struct Cyc_Absyn_Tuniondecl*_tmp2A4;struct Cyc_Absyn_Tunionfield*_tmp2A5;
struct Cyc_List_List*_tmp2A6;void*_tmp2A7;struct Cyc_Absyn_PtrInfo _tmp2A8;void*
_tmp2A9;struct Cyc_Absyn_Tqual _tmp2AA;struct Cyc_Absyn_PtrAtts _tmp2AB;void*_tmp2AC;
struct Cyc_Absyn_Conref*_tmp2AD;struct Cyc_Absyn_Conref*_tmp2AE;struct Cyc_Absyn_Conref*
_tmp2AF;void*_tmp2B0;struct Cyc_Absyn_PtrInfo _tmp2B1;void*_tmp2B2;struct Cyc_Absyn_Tqual
_tmp2B3;struct Cyc_Absyn_PtrAtts _tmp2B4;void*_tmp2B5;struct Cyc_Absyn_Conref*
_tmp2B6;struct Cyc_Absyn_Conref*_tmp2B7;struct Cyc_Absyn_Conref*_tmp2B8;void*
_tmp2B9;void*_tmp2BA;void*_tmp2BB;void*_tmp2BC;void*_tmp2BD;void*_tmp2BE;void*
_tmp2BF;void*_tmp2C0;void*_tmp2C1;int _tmp2C2;void*_tmp2C3;int _tmp2C4;void*
_tmp2C5;void*_tmp2C6;void*_tmp2C7;void*_tmp2C8;void*_tmp2C9;struct Cyc_Absyn_Exp*
_tmp2CA;void*_tmp2CB;struct Cyc_Absyn_Exp*_tmp2CC;void*_tmp2CD;struct Cyc_Absyn_ArrayInfo
_tmp2CE;void*_tmp2CF;struct Cyc_Absyn_Tqual _tmp2D0;struct Cyc_Absyn_Exp*_tmp2D1;
struct Cyc_Absyn_Conref*_tmp2D2;void*_tmp2D3;struct Cyc_Absyn_ArrayInfo _tmp2D4;
void*_tmp2D5;struct Cyc_Absyn_Tqual _tmp2D6;struct Cyc_Absyn_Exp*_tmp2D7;struct Cyc_Absyn_Conref*
_tmp2D8;void*_tmp2D9;struct Cyc_Absyn_FnInfo _tmp2DA;struct Cyc_List_List*_tmp2DB;
struct Cyc_Core_Opt*_tmp2DC;void*_tmp2DD;struct Cyc_List_List*_tmp2DE;int _tmp2DF;
struct Cyc_Absyn_VarargInfo*_tmp2E0;struct Cyc_List_List*_tmp2E1;struct Cyc_List_List*
_tmp2E2;void*_tmp2E3;struct Cyc_Absyn_FnInfo _tmp2E4;struct Cyc_List_List*_tmp2E5;
struct Cyc_Core_Opt*_tmp2E6;void*_tmp2E7;struct Cyc_List_List*_tmp2E8;int _tmp2E9;
struct Cyc_Absyn_VarargInfo*_tmp2EA;struct Cyc_List_List*_tmp2EB;struct Cyc_List_List*
_tmp2EC;void*_tmp2ED;struct Cyc_List_List*_tmp2EE;void*_tmp2EF;struct Cyc_List_List*
_tmp2F0;void*_tmp2F1;void*_tmp2F2;struct Cyc_List_List*_tmp2F3;void*_tmp2F4;void*
_tmp2F5;struct Cyc_List_List*_tmp2F6;void*_tmp2F7;void*_tmp2F8;void*_tmp2F9;void*
_tmp2FA;void*_tmp2FB;void*_tmp2FC;void*_tmp2FD;void*_tmp2FE;void*_tmp2FF;void*
_tmp300;void*_tmp301;void*_tmp302;void*_tmp303;void*_tmp304;void*_tmp305;void*
_tmp306;void*_tmp307;void*_tmp308;void*_tmp309;void*_tmp30A;_LL21E: _tmp276=
_tmp275.f1;if(_tmp276 <= (void*)4)goto _LL220;if(*((int*)_tmp276)!= 0)goto _LL220;
_LL21F: Cyc_Tcutil_unify_it(t2,t1);return;_LL220: _tmp277=_tmp275.f1;if((int)
_tmp277 != 0)goto _LL222;_tmp278=_tmp275.f2;if((int)_tmp278 != 0)goto _LL222;_LL221:
return;_LL222: _tmp279=_tmp275.f1;if(_tmp279 <= (void*)4)goto _LL224;if(*((int*)
_tmp279)!= 1)goto _LL224;_tmp27A=((struct Cyc_Absyn_VarType_struct*)_tmp279)->f1;
_tmp27B=_tmp275.f2;if(_tmp27B <= (void*)4)goto _LL224;if(*((int*)_tmp27B)!= 1)goto
_LL224;_tmp27C=((struct Cyc_Absyn_VarType_struct*)_tmp27B)->f1;_LL223: {struct
_dyneither_ptr*_tmp30B=_tmp27A->name;struct _dyneither_ptr*_tmp30C=_tmp27C->name;
int _tmp30D=_tmp27A->identity;int _tmp30E=_tmp27C->identity;void*_tmp30F=Cyc_Tcutil_tvar_kind(
_tmp27A);void*_tmp310=Cyc_Tcutil_tvar_kind(_tmp27C);if(_tmp30E == _tmp30D  && Cyc_strptrcmp(
_tmp30B,_tmp30C)== 0){if(_tmp30F != _tmp310){const char*_tmpCFF;void*_tmpCFE[3];
struct Cyc_String_pa_struct _tmpCFD;struct Cyc_String_pa_struct _tmpCFC;struct Cyc_String_pa_struct
_tmpCFB;(_tmpCFB.tag=0,((_tmpCFB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kind2string(_tmp310)),((_tmpCFC.tag=0,((_tmpCFC.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(_tmp30F)),((
_tmpCFD.tag=0,((_tmpCFD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp30B),((
_tmpCFE[0]=& _tmpCFD,((_tmpCFE[1]=& _tmpCFC,((_tmpCFE[2]=& _tmpCFB,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpCFF="same type variable %s has kinds %s and %s",
_tag_dyneither(_tmpCFF,sizeof(char),42))),_tag_dyneither(_tmpCFE,sizeof(void*),3)))))))))))))))))));}
return;}{const char*_tmpD00;Cyc_Tcutil_failure_reason=((_tmpD00="(variable types are not the same)",
_tag_dyneither(_tmpD00,sizeof(char),34)));}goto _LL21D;}_LL224: _tmp27D=_tmp275.f1;
if(_tmp27D <= (void*)4)goto _LL226;if(*((int*)_tmp27D)!= 10)goto _LL226;_tmp27E=((
struct Cyc_Absyn_AggrType_struct*)_tmp27D)->f1;_tmp27F=_tmp27E.aggr_info;_tmp280=
_tmp27E.targs;_tmp281=_tmp275.f2;if(_tmp281 <= (void*)4)goto _LL226;if(*((int*)
_tmp281)!= 10)goto _LL226;_tmp282=((struct Cyc_Absyn_AggrType_struct*)_tmp281)->f1;
_tmp283=_tmp282.aggr_info;_tmp284=_tmp282.targs;_LL225: {void*_tmp318;struct
_tuple2*_tmp319;struct _tuple6 _tmp317=Cyc_Absyn_aggr_kinded_name(_tmp283);_tmp318=
_tmp317.f1;_tmp319=_tmp317.f2;{void*_tmp31B;struct _tuple2*_tmp31C;struct _tuple6
_tmp31A=Cyc_Absyn_aggr_kinded_name(_tmp27F);_tmp31B=_tmp31A.f1;_tmp31C=_tmp31A.f2;
if(_tmp318 != _tmp31B){{const char*_tmpD01;Cyc_Tcutil_failure_reason=((_tmpD01="(struct and union type)",
_tag_dyneither(_tmpD01,sizeof(char),24)));}goto _LL21D;}if(Cyc_Absyn_qvar_cmp(
_tmp319,_tmp31C)!= 0){{const char*_tmpD02;Cyc_Tcutil_failure_reason=((_tmpD02="(different type name)",
_tag_dyneither(_tmpD02,sizeof(char),22)));}goto _LL21D;}Cyc_Tcutil_unify_list(
_tmp284,_tmp280);return;}}_LL226: _tmp285=_tmp275.f1;if(_tmp285 <= (void*)4)goto
_LL228;if(*((int*)_tmp285)!= 12)goto _LL228;_tmp286=((struct Cyc_Absyn_EnumType_struct*)
_tmp285)->f1;_tmp287=_tmp275.f2;if(_tmp287 <= (void*)4)goto _LL228;if(*((int*)
_tmp287)!= 12)goto _LL228;_tmp288=((struct Cyc_Absyn_EnumType_struct*)_tmp287)->f1;
_LL227: if(Cyc_Absyn_qvar_cmp(_tmp286,_tmp288)== 0)return;{const char*_tmpD03;Cyc_Tcutil_failure_reason=((
_tmpD03="(different enum types)",_tag_dyneither(_tmpD03,sizeof(char),23)));}goto
_LL21D;_LL228: _tmp289=_tmp275.f1;if(_tmp289 <= (void*)4)goto _LL22A;if(*((int*)
_tmp289)!= 13)goto _LL22A;_tmp28A=((struct Cyc_Absyn_AnonEnumType_struct*)_tmp289)->f1;
_tmp28B=_tmp275.f2;if(_tmp28B <= (void*)4)goto _LL22A;if(*((int*)_tmp28B)!= 13)
goto _LL22A;_tmp28C=((struct Cyc_Absyn_AnonEnumType_struct*)_tmp28B)->f1;_LL229: {
int bad=0;for(0;_tmp28A != 0  && _tmp28C != 0;(_tmp28A=_tmp28A->tl,_tmp28C=_tmp28C->tl)){
struct Cyc_Absyn_Enumfield*_tmp320=(struct Cyc_Absyn_Enumfield*)_tmp28A->hd;struct
Cyc_Absyn_Enumfield*_tmp321=(struct Cyc_Absyn_Enumfield*)_tmp28C->hd;if(Cyc_Absyn_qvar_cmp(
_tmp320->name,_tmp321->name)!= 0){{const char*_tmpD04;Cyc_Tcutil_failure_reason=((
_tmpD04="(different names for enum fields)",_tag_dyneither(_tmpD04,sizeof(char),
34)));}bad=1;break;}if(_tmp320->tag == _tmp321->tag)continue;if(_tmp320->tag == 0
 || _tmp321->tag == 0){{const char*_tmpD05;Cyc_Tcutil_failure_reason=((_tmpD05="(different tag values for enum fields)",
_tag_dyneither(_tmpD05,sizeof(char),39)));}bad=1;break;}if(!Cyc_Evexp_same_const_exp((
struct Cyc_Absyn_Exp*)_check_null(_tmp320->tag),(struct Cyc_Absyn_Exp*)_check_null(
_tmp321->tag))){{const char*_tmpD06;Cyc_Tcutil_failure_reason=((_tmpD06="(different tag values for enum fields)",
_tag_dyneither(_tmpD06,sizeof(char),39)));}bad=1;break;}}if(bad)goto _LL21D;if(
_tmp28A == 0  && _tmp28C == 0)return;{const char*_tmpD07;Cyc_Tcutil_failure_reason=((
_tmpD07="(different number of fields for enums)",_tag_dyneither(_tmpD07,sizeof(
char),39)));}goto _LL21D;}_LL22A: _tmp28D=_tmp275.f1;if(_tmp28D <= (void*)4)goto
_LL22C;if(*((int*)_tmp28D)!= 2)goto _LL22C;_tmp28E=((struct Cyc_Absyn_TunionType_struct*)
_tmp28D)->f1;_tmp28F=_tmp28E.tunion_info;if((((((struct Cyc_Absyn_TunionType_struct*)
_tmp28D)->f1).tunion_info).KnownTunion).tag != 1)goto _LL22C;_tmp290=(_tmp28F.KnownTunion).f1;
_tmp291=*_tmp290;_tmp292=_tmp28E.targs;_tmp293=_tmp28E.rgn;_tmp294=_tmp275.f2;
if(_tmp294 <= (void*)4)goto _LL22C;if(*((int*)_tmp294)!= 2)goto _LL22C;_tmp295=((
struct Cyc_Absyn_TunionType_struct*)_tmp294)->f1;_tmp296=_tmp295.tunion_info;if((((((
struct Cyc_Absyn_TunionType_struct*)_tmp294)->f1).tunion_info).KnownTunion).tag != 
1)goto _LL22C;_tmp297=(_tmp296.KnownTunion).f1;_tmp298=*_tmp297;_tmp299=_tmp295.targs;
_tmp29A=_tmp295.rgn;_LL22B: if(_tmp291 == _tmp298  || Cyc_Absyn_qvar_cmp(_tmp291->name,
_tmp298->name)== 0){if(_tmp29A != 0  && _tmp293 != 0)Cyc_Tcutil_unify_it((void*)
_tmp29A->v,(void*)_tmp293->v);else{if(_tmp29A != 0  || _tmp293 != 0)goto Tunion_fail;}
Cyc_Tcutil_unify_list(_tmp299,_tmp292);return;}Tunion_fail: {const char*_tmpD08;
Cyc_Tcutil_failure_reason=((_tmpD08="(different tunion types)",_tag_dyneither(
_tmpD08,sizeof(char),25)));}goto _LL21D;_LL22C: _tmp29B=_tmp275.f1;if(_tmp29B <= (
void*)4)goto _LL22E;if(*((int*)_tmp29B)!= 3)goto _LL22E;_tmp29C=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp29B)->f1;_tmp29D=_tmp29C.field_info;if((((((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp29B)->f1).field_info).KnownTunionfield).tag != 1)goto _LL22E;_tmp29E=(_tmp29D.KnownTunionfield).f1;
_tmp29F=(_tmp29D.KnownTunionfield).f2;_tmp2A0=_tmp29C.targs;_tmp2A1=_tmp275.f2;
if(_tmp2A1 <= (void*)4)goto _LL22E;if(*((int*)_tmp2A1)!= 3)goto _LL22E;_tmp2A2=((
struct Cyc_Absyn_TunionFieldType_struct*)_tmp2A1)->f1;_tmp2A3=_tmp2A2.field_info;
if((((((struct Cyc_Absyn_TunionFieldType_struct*)_tmp2A1)->f1).field_info).KnownTunionfield).tag
!= 1)goto _LL22E;_tmp2A4=(_tmp2A3.KnownTunionfield).f1;_tmp2A5=(_tmp2A3.KnownTunionfield).f2;
_tmp2A6=_tmp2A2.targs;_LL22D: if((_tmp29E == _tmp2A4  || Cyc_Absyn_qvar_cmp(_tmp29E->name,
_tmp2A4->name)== 0) && (_tmp29F == _tmp2A5  || Cyc_Absyn_qvar_cmp(_tmp29F->name,
_tmp2A5->name)== 0)){Cyc_Tcutil_unify_list(_tmp2A6,_tmp2A0);return;}{const char*
_tmpD09;Cyc_Tcutil_failure_reason=((_tmpD09="(different tunion field types)",
_tag_dyneither(_tmpD09,sizeof(char),31)));}goto _LL21D;_LL22E: _tmp2A7=_tmp275.f1;
if(_tmp2A7 <= (void*)4)goto _LL230;if(*((int*)_tmp2A7)!= 4)goto _LL230;_tmp2A8=((
struct Cyc_Absyn_PointerType_struct*)_tmp2A7)->f1;_tmp2A9=(void*)_tmp2A8.elt_typ;
_tmp2AA=_tmp2A8.elt_tq;_tmp2AB=_tmp2A8.ptr_atts;_tmp2AC=(void*)_tmp2AB.rgn;
_tmp2AD=_tmp2AB.nullable;_tmp2AE=_tmp2AB.bounds;_tmp2AF=_tmp2AB.zero_term;
_tmp2B0=_tmp275.f2;if(_tmp2B0 <= (void*)4)goto _LL230;if(*((int*)_tmp2B0)!= 4)goto
_LL230;_tmp2B1=((struct Cyc_Absyn_PointerType_struct*)_tmp2B0)->f1;_tmp2B2=(void*)
_tmp2B1.elt_typ;_tmp2B3=_tmp2B1.elt_tq;_tmp2B4=_tmp2B1.ptr_atts;_tmp2B5=(void*)
_tmp2B4.rgn;_tmp2B6=_tmp2B4.nullable;_tmp2B7=_tmp2B4.bounds;_tmp2B8=_tmp2B4.zero_term;
_LL22F: Cyc_Tcutil_unify_it(_tmp2B2,_tmp2A9);Cyc_Tcutil_unify_it(_tmp2AC,_tmp2B5);
Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;{const char*_tmpD0A;((void(*)(
int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y,struct
_dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,_tmp2B8,
_tmp2AF,((_tmpD0A="(not both zero terminated)",_tag_dyneither(_tmpD0A,sizeof(
char),27))));}Cyc_Tcutil_unify_tqual(_tmp2B3,_tmp2B2,_tmp2AA,_tmp2A9);{const char*
_tmpD0B;Cyc_Tcutil_unify_it_conrefs(Cyc_Tcutil_unify_it_bounds,_tmp2B7,_tmp2AE,((
_tmpD0B="(different pointer bounds)",_tag_dyneither(_tmpD0B,sizeof(char),27))));}{
union Cyc_Absyn_Constraint_union _tmp32A=(Cyc_Absyn_compress_conref(_tmp2B7))->v;
void*_tmp32B;_LL259: if((_tmp32A.Eq_constr).tag != 0)goto _LL25B;_tmp32B=(_tmp32A.Eq_constr).f1;
if((int)_tmp32B != 0)goto _LL25B;_LL25A: return;_LL25B:;_LL25C: goto _LL258;_LL258:;}{
const char*_tmpD0C;((void(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct
Cyc_Absyn_Conref*y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,
_tmp2B6,_tmp2AD,((_tmpD0C="(incompatible pointer types)",_tag_dyneither(_tmpD0C,
sizeof(char),29))));}return;_LL230: _tmp2B9=_tmp275.f1;if(_tmp2B9 <= (void*)4)goto
_LL232;if(*((int*)_tmp2B9)!= 5)goto _LL232;_tmp2BA=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp2B9)->f1;_tmp2BB=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp2B9)->f2;
_tmp2BC=_tmp275.f2;if(_tmp2BC <= (void*)4)goto _LL232;if(*((int*)_tmp2BC)!= 5)goto
_LL232;_tmp2BD=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp2BC)->f1;_tmp2BE=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp2BC)->f2;_LL231: if(_tmp2BD == _tmp2BA
 && ((_tmp2BE == _tmp2BB  || _tmp2BE == (void*)2  && _tmp2BB == (void*)3) || _tmp2BE
== (void*)3  && _tmp2BB == (void*)2))return;{const char*_tmpD0D;Cyc_Tcutil_failure_reason=((
_tmpD0D="(different integral types)",_tag_dyneither(_tmpD0D,sizeof(char),27)));}
goto _LL21D;_LL232: _tmp2BF=_tmp275.f1;if((int)_tmp2BF != 1)goto _LL234;_tmp2C0=
_tmp275.f2;if((int)_tmp2C0 != 1)goto _LL234;_LL233: return;_LL234: _tmp2C1=_tmp275.f1;
if(_tmp2C1 <= (void*)4)goto _LL236;if(*((int*)_tmp2C1)!= 6)goto _LL236;_tmp2C2=((
struct Cyc_Absyn_DoubleType_struct*)_tmp2C1)->f1;_tmp2C3=_tmp275.f2;if(_tmp2C3 <= (
void*)4)goto _LL236;if(*((int*)_tmp2C3)!= 6)goto _LL236;_tmp2C4=((struct Cyc_Absyn_DoubleType_struct*)
_tmp2C3)->f1;_LL235: if(_tmp2C2 == _tmp2C4)return;goto _LL21D;_LL236: _tmp2C5=
_tmp275.f1;if(_tmp2C5 <= (void*)4)goto _LL238;if(*((int*)_tmp2C5)!= 18)goto _LL238;
_tmp2C6=(void*)((struct Cyc_Absyn_TagType_struct*)_tmp2C5)->f1;_tmp2C7=_tmp275.f2;
if(_tmp2C7 <= (void*)4)goto _LL238;if(*((int*)_tmp2C7)!= 18)goto _LL238;_tmp2C8=(
void*)((struct Cyc_Absyn_TagType_struct*)_tmp2C7)->f1;_LL237: Cyc_Tcutil_unify_it(
_tmp2C6,_tmp2C8);return;_LL238: _tmp2C9=_tmp275.f1;if(_tmp2C9 <= (void*)4)goto
_LL23A;if(*((int*)_tmp2C9)!= 17)goto _LL23A;_tmp2CA=((struct Cyc_Absyn_ValueofType_struct*)
_tmp2C9)->f1;_tmp2CB=_tmp275.f2;if(_tmp2CB <= (void*)4)goto _LL23A;if(*((int*)
_tmp2CB)!= 17)goto _LL23A;_tmp2CC=((struct Cyc_Absyn_ValueofType_struct*)_tmp2CB)->f1;
_LL239: if(!Cyc_Evexp_same_const_exp(_tmp2CA,_tmp2CC)){{const char*_tmpD0E;Cyc_Tcutil_failure_reason=((
_tmpD0E="(cannot prove expressions are the same)",_tag_dyneither(_tmpD0E,sizeof(
char),40)));}goto _LL21D;}return;_LL23A: _tmp2CD=_tmp275.f1;if(_tmp2CD <= (void*)4)
goto _LL23C;if(*((int*)_tmp2CD)!= 7)goto _LL23C;_tmp2CE=((struct Cyc_Absyn_ArrayType_struct*)
_tmp2CD)->f1;_tmp2CF=(void*)_tmp2CE.elt_type;_tmp2D0=_tmp2CE.tq;_tmp2D1=_tmp2CE.num_elts;
_tmp2D2=_tmp2CE.zero_term;_tmp2D3=_tmp275.f2;if(_tmp2D3 <= (void*)4)goto _LL23C;
if(*((int*)_tmp2D3)!= 7)goto _LL23C;_tmp2D4=((struct Cyc_Absyn_ArrayType_struct*)
_tmp2D3)->f1;_tmp2D5=(void*)_tmp2D4.elt_type;_tmp2D6=_tmp2D4.tq;_tmp2D7=_tmp2D4.num_elts;
_tmp2D8=_tmp2D4.zero_term;_LL23B: Cyc_Tcutil_unify_it(_tmp2D5,_tmp2CF);Cyc_Tcutil_unify_tqual(
_tmp2D6,_tmp2D5,_tmp2D0,_tmp2CF);Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=
t2;{const char*_tmpD0F;((void(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,
struct Cyc_Absyn_Conref*y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(
Cyc_Core_intcmp,_tmp2D2,_tmp2D8,((_tmpD0F="(not both zero terminated)",
_tag_dyneither(_tmpD0F,sizeof(char),27))));}if(_tmp2D1 == _tmp2D7)return;if(
_tmp2D1 == 0  || _tmp2D7 == 0)goto _LL21D;if(Cyc_Evexp_same_const_exp((struct Cyc_Absyn_Exp*)
_tmp2D1,(struct Cyc_Absyn_Exp*)_tmp2D7))return;{const char*_tmpD10;Cyc_Tcutil_failure_reason=((
_tmpD10="(different array sizes)",_tag_dyneither(_tmpD10,sizeof(char),24)));}
goto _LL21D;_LL23C: _tmp2D9=_tmp275.f1;if(_tmp2D9 <= (void*)4)goto _LL23E;if(*((int*)
_tmp2D9)!= 8)goto _LL23E;_tmp2DA=((struct Cyc_Absyn_FnType_struct*)_tmp2D9)->f1;
_tmp2DB=_tmp2DA.tvars;_tmp2DC=_tmp2DA.effect;_tmp2DD=(void*)_tmp2DA.ret_typ;
_tmp2DE=_tmp2DA.args;_tmp2DF=_tmp2DA.c_varargs;_tmp2E0=_tmp2DA.cyc_varargs;
_tmp2E1=_tmp2DA.rgn_po;_tmp2E2=_tmp2DA.attributes;_tmp2E3=_tmp275.f2;if(_tmp2E3
<= (void*)4)goto _LL23E;if(*((int*)_tmp2E3)!= 8)goto _LL23E;_tmp2E4=((struct Cyc_Absyn_FnType_struct*)
_tmp2E3)->f1;_tmp2E5=_tmp2E4.tvars;_tmp2E6=_tmp2E4.effect;_tmp2E7=(void*)_tmp2E4.ret_typ;
_tmp2E8=_tmp2E4.args;_tmp2E9=_tmp2E4.c_varargs;_tmp2EA=_tmp2E4.cyc_varargs;
_tmp2EB=_tmp2E4.rgn_po;_tmp2EC=_tmp2E4.attributes;_LL23D: {int done=0;struct
_RegionHandle _tmp331=_new_region("rgn");struct _RegionHandle*rgn=& _tmp331;
_push_region(rgn);{struct Cyc_List_List*inst=0;while(_tmp2E5 != 0){if(_tmp2DB == 0){{
const char*_tmpD11;Cyc_Tcutil_failure_reason=((_tmpD11="(second function type has too few type variables)",
_tag_dyneither(_tmpD11,sizeof(char),50)));}(int)_throw((void*)Cyc_Tcutil_Unify);}{
void*_tmp333=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp2E5->hd);void*
_tmp334=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp2DB->hd);if(_tmp333 != 
_tmp334){{const char*_tmpD17;void*_tmpD16[3];struct Cyc_String_pa_struct _tmpD15;
struct Cyc_String_pa_struct _tmpD14;struct Cyc_String_pa_struct _tmpD13;Cyc_Tcutil_failure_reason=(
struct _dyneither_ptr)((_tmpD13.tag=0,((_tmpD13.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_kind2string(_tmp334)),((_tmpD14.tag=0,((_tmpD14.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(_tmp333)),((
_tmpD15.tag=0,((_tmpD15.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_tvar2string((
struct Cyc_Absyn_Tvar*)_tmp2E5->hd)),((_tmpD16[0]=& _tmpD15,((_tmpD16[1]=& _tmpD14,((
_tmpD16[2]=& _tmpD13,Cyc_aprintf(((_tmpD17="(type var %s has different kinds %s and %s)",
_tag_dyneither(_tmpD17,sizeof(char),44))),_tag_dyneither(_tmpD16,sizeof(void*),3))))))))))))))))))));}(
int)_throw((void*)Cyc_Tcutil_Unify);}{struct _tuple8*_tmpD21;struct Cyc_Absyn_VarType_struct
_tmpD20;struct Cyc_Absyn_VarType_struct*_tmpD1F;struct Cyc_List_List*_tmpD1E;inst=((
_tmpD1E=_region_malloc(rgn,sizeof(*_tmpD1E)),((_tmpD1E->hd=((_tmpD21=
_region_malloc(rgn,sizeof(*_tmpD21)),((_tmpD21->f1=(struct Cyc_Absyn_Tvar*)
_tmp2DB->hd,((_tmpD21->f2=(void*)((_tmpD1F=_cycalloc(sizeof(*_tmpD1F)),((_tmpD1F[
0]=((_tmpD20.tag=1,((_tmpD20.f1=(struct Cyc_Absyn_Tvar*)_tmp2E5->hd,_tmpD20)))),
_tmpD1F)))),_tmpD21)))))),((_tmpD1E->tl=inst,_tmpD1E))))));}_tmp2E5=_tmp2E5->tl;
_tmp2DB=_tmp2DB->tl;}}if(_tmp2DB != 0){{const char*_tmpD22;Cyc_Tcutil_failure_reason=((
_tmpD22="(second function type has too many type variables)",_tag_dyneither(
_tmpD22,sizeof(char),51)));}_npop_handler(0);goto _LL21D;}if(inst != 0){{struct Cyc_Absyn_FnType_struct
_tmpD2E;struct Cyc_Absyn_FnInfo _tmpD2D;struct Cyc_Absyn_FnType_struct*_tmpD2C;
struct Cyc_Absyn_FnType_struct _tmpD28;struct Cyc_Absyn_FnInfo _tmpD27;struct Cyc_Absyn_FnType_struct*
_tmpD26;Cyc_Tcutil_unify_it((void*)((_tmpD26=_cycalloc(sizeof(*_tmpD26)),((
_tmpD26[0]=((_tmpD28.tag=8,((_tmpD28.f1=((_tmpD27.tvars=0,((_tmpD27.effect=
_tmp2E6,((_tmpD27.ret_typ=(void*)_tmp2E7,((_tmpD27.args=_tmp2E8,((_tmpD27.c_varargs=
_tmp2E9,((_tmpD27.cyc_varargs=_tmp2EA,((_tmpD27.rgn_po=_tmp2EB,((_tmpD27.attributes=
_tmp2EC,_tmpD27)))))))))))))))),_tmpD28)))),_tmpD26)))),Cyc_Tcutil_rsubstitute(
rgn,inst,(void*)((_tmpD2C=_cycalloc(sizeof(*_tmpD2C)),((_tmpD2C[0]=((_tmpD2E.tag=
8,((_tmpD2E.f1=((_tmpD2D.tvars=0,((_tmpD2D.effect=_tmp2DC,((_tmpD2D.ret_typ=(
void*)_tmp2DD,((_tmpD2D.args=_tmp2DE,((_tmpD2D.c_varargs=_tmp2DF,((_tmpD2D.cyc_varargs=
_tmp2E0,((_tmpD2D.rgn_po=_tmp2E1,((_tmpD2D.attributes=_tmp2E2,_tmpD2D)))))))))))))))),
_tmpD2E)))),_tmpD2C))))));}done=1;}}if(done){_npop_handler(0);return;}Cyc_Tcutil_unify_it(
_tmp2E7,_tmp2DD);for(0;_tmp2E8 != 0  && _tmp2DE != 0;(_tmp2E8=_tmp2E8->tl,_tmp2DE=
_tmp2DE->tl)){struct Cyc_Absyn_Tqual _tmp346;void*_tmp347;struct _tuple3 _tmp345=*((
struct _tuple3*)_tmp2E8->hd);_tmp346=_tmp345.f2;_tmp347=_tmp345.f3;{struct Cyc_Absyn_Tqual
_tmp349;void*_tmp34A;struct _tuple3 _tmp348=*((struct _tuple3*)_tmp2DE->hd);_tmp349=
_tmp348.f2;_tmp34A=_tmp348.f3;Cyc_Tcutil_unify_it(_tmp347,_tmp34A);Cyc_Tcutil_unify_tqual(
_tmp346,_tmp347,_tmp349,_tmp34A);}}Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=
t2;if(_tmp2E8 != 0  || _tmp2DE != 0){{const char*_tmpD2F;Cyc_Tcutil_failure_reason=((
_tmpD2F="(function types have different number of arguments)",_tag_dyneither(
_tmpD2F,sizeof(char),52)));}_npop_handler(0);goto _LL21D;}if(_tmp2E9 != _tmp2DF){{
const char*_tmpD30;Cyc_Tcutil_failure_reason=((_tmpD30="(only one function type takes C varargs)",
_tag_dyneither(_tmpD30,sizeof(char),41)));}_npop_handler(0);goto _LL21D;}{int
bad_cyc_vararg=0;{struct _tuple11 _tmpD31;struct _tuple11 _tmp34E=(_tmpD31.f1=
_tmp2EA,((_tmpD31.f2=_tmp2E0,_tmpD31)));struct Cyc_Absyn_VarargInfo*_tmp34F;
struct Cyc_Absyn_VarargInfo*_tmp350;struct Cyc_Absyn_VarargInfo*_tmp351;struct Cyc_Absyn_VarargInfo*
_tmp352;struct Cyc_Absyn_VarargInfo*_tmp353;struct Cyc_Absyn_VarargInfo _tmp354;
struct Cyc_Core_Opt*_tmp355;struct Cyc_Absyn_Tqual _tmp356;void*_tmp357;int _tmp358;
struct Cyc_Absyn_VarargInfo*_tmp359;struct Cyc_Absyn_VarargInfo _tmp35A;struct Cyc_Core_Opt*
_tmp35B;struct Cyc_Absyn_Tqual _tmp35C;void*_tmp35D;int _tmp35E;_LL25E: _tmp34F=
_tmp34E.f1;if(_tmp34F != 0)goto _LL260;_tmp350=_tmp34E.f2;if(_tmp350 != 0)goto
_LL260;_LL25F: goto _LL25D;_LL260: _tmp351=_tmp34E.f1;if(_tmp351 != 0)goto _LL262;
_LL261: goto _LL263;_LL262: _tmp352=_tmp34E.f2;if(_tmp352 != 0)goto _LL264;_LL263:
bad_cyc_vararg=1;{const char*_tmpD32;Cyc_Tcutil_failure_reason=((_tmpD32="(only one function type takes varargs)",
_tag_dyneither(_tmpD32,sizeof(char),39)));}goto _LL25D;_LL264: _tmp353=_tmp34E.f1;
if(_tmp353 == 0)goto _LL25D;_tmp354=*_tmp353;_tmp355=_tmp354.name;_tmp356=_tmp354.tq;
_tmp357=(void*)_tmp354.type;_tmp358=_tmp354.inject;_tmp359=_tmp34E.f2;if(_tmp359
== 0)goto _LL25D;_tmp35A=*_tmp359;_tmp35B=_tmp35A.name;_tmp35C=_tmp35A.tq;_tmp35D=(
void*)_tmp35A.type;_tmp35E=_tmp35A.inject;_LL265: Cyc_Tcutil_unify_it(_tmp357,
_tmp35D);Cyc_Tcutil_unify_tqual(_tmp356,_tmp357,_tmp35C,_tmp35D);if(_tmp358 != 
_tmp35E){bad_cyc_vararg=1;{const char*_tmpD33;Cyc_Tcutil_failure_reason=((_tmpD33="(only one function type injects varargs)",
_tag_dyneither(_tmpD33,sizeof(char),41)));}}goto _LL25D;_LL25D:;}if(
bad_cyc_vararg){_npop_handler(0);goto _LL21D;}{int bad_effect=0;{struct _tuple12
_tmpD34;struct _tuple12 _tmp362=(_tmpD34.f1=_tmp2E6,((_tmpD34.f2=_tmp2DC,_tmpD34)));
struct Cyc_Core_Opt*_tmp363;struct Cyc_Core_Opt*_tmp364;struct Cyc_Core_Opt*_tmp365;
struct Cyc_Core_Opt*_tmp366;_LL267: _tmp363=_tmp362.f1;if(_tmp363 != 0)goto _LL269;
_tmp364=_tmp362.f2;if(_tmp364 != 0)goto _LL269;_LL268: goto _LL266;_LL269: _tmp365=
_tmp362.f1;if(_tmp365 != 0)goto _LL26B;_LL26A: goto _LL26C;_LL26B: _tmp366=_tmp362.f2;
if(_tmp366 != 0)goto _LL26D;_LL26C: bad_effect=1;goto _LL266;_LL26D:;_LL26E:
bad_effect=!Cyc_Tcutil_unify_effect((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp2E6))->v,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp2DC))->v);goto _LL266;
_LL266:;}Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;if(bad_effect){{const
char*_tmpD35;Cyc_Tcutil_failure_reason=((_tmpD35="(function type effects do not unify)",
_tag_dyneither(_tmpD35,sizeof(char),37)));}_npop_handler(0);goto _LL21D;}if(!Cyc_Tcutil_same_atts(
_tmp2E2,_tmp2EC)){{const char*_tmpD36;Cyc_Tcutil_failure_reason=((_tmpD36="(function types have different attributes)",
_tag_dyneither(_tmpD36,sizeof(char),43)));}_npop_handler(0);goto _LL21D;}if(!Cyc_Tcutil_same_rgn_po(
_tmp2E1,_tmp2EB)){{const char*_tmpD37;Cyc_Tcutil_failure_reason=((_tmpD37="(function types have different region lifetime orderings)",
_tag_dyneither(_tmpD37,sizeof(char),58)));}_npop_handler(0);goto _LL21D;}
_npop_handler(0);return;}};_pop_region(rgn);}_LL23E: _tmp2ED=_tmp275.f1;if(
_tmp2ED <= (void*)4)goto _LL240;if(*((int*)_tmp2ED)!= 9)goto _LL240;_tmp2EE=((
struct Cyc_Absyn_TupleType_struct*)_tmp2ED)->f1;_tmp2EF=_tmp275.f2;if(_tmp2EF <= (
void*)4)goto _LL240;if(*((int*)_tmp2EF)!= 9)goto _LL240;_tmp2F0=((struct Cyc_Absyn_TupleType_struct*)
_tmp2EF)->f1;_LL23F: for(0;_tmp2F0 != 0  && _tmp2EE != 0;(_tmp2F0=_tmp2F0->tl,
_tmp2EE=_tmp2EE->tl)){struct Cyc_Absyn_Tqual _tmp36B;void*_tmp36C;struct _tuple5
_tmp36A=*((struct _tuple5*)_tmp2F0->hd);_tmp36B=_tmp36A.f1;_tmp36C=_tmp36A.f2;{
struct Cyc_Absyn_Tqual _tmp36E;void*_tmp36F;struct _tuple5 _tmp36D=*((struct _tuple5*)
_tmp2EE->hd);_tmp36E=_tmp36D.f1;_tmp36F=_tmp36D.f2;Cyc_Tcutil_unify_it(_tmp36C,
_tmp36F);Cyc_Tcutil_unify_tqual(_tmp36B,_tmp36C,_tmp36E,_tmp36F);}}if(_tmp2F0 == 
0  && _tmp2EE == 0)return;Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;{const
char*_tmpD38;Cyc_Tcutil_failure_reason=((_tmpD38="(tuple types have different numbers of components)",
_tag_dyneither(_tmpD38,sizeof(char),51)));}goto _LL21D;_LL240: _tmp2F1=_tmp275.f1;
if(_tmp2F1 <= (void*)4)goto _LL242;if(*((int*)_tmp2F1)!= 11)goto _LL242;_tmp2F2=(
void*)((struct Cyc_Absyn_AnonAggrType_struct*)_tmp2F1)->f1;_tmp2F3=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp2F1)->f2;_tmp2F4=_tmp275.f2;if(_tmp2F4 <= (void*)4)goto _LL242;if(*((int*)
_tmp2F4)!= 11)goto _LL242;_tmp2F5=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp2F4)->f1;_tmp2F6=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp2F4)->f2;_LL241:
if(_tmp2F5 != _tmp2F2){{const char*_tmpD39;Cyc_Tcutil_failure_reason=((_tmpD39="(struct and union type)",
_tag_dyneither(_tmpD39,sizeof(char),24)));}goto _LL21D;}for(0;_tmp2F6 != 0  && 
_tmp2F3 != 0;(_tmp2F6=_tmp2F6->tl,_tmp2F3=_tmp2F3->tl)){struct Cyc_Absyn_Aggrfield*
_tmp372=(struct Cyc_Absyn_Aggrfield*)_tmp2F6->hd;struct Cyc_Absyn_Aggrfield*
_tmp373=(struct Cyc_Absyn_Aggrfield*)_tmp2F3->hd;if(Cyc_strptrcmp(_tmp372->name,
_tmp373->name)!= 0){{const char*_tmpD3A;Cyc_Tcutil_failure_reason=((_tmpD3A="(different member names)",
_tag_dyneither(_tmpD3A,sizeof(char),25)));}(int)_throw((void*)Cyc_Tcutil_Unify);}
Cyc_Tcutil_unify_it((void*)_tmp372->type,(void*)_tmp373->type);Cyc_Tcutil_unify_tqual(
_tmp372->tq,(void*)_tmp372->type,_tmp373->tq,(void*)_tmp373->type);if(!Cyc_Tcutil_same_atts(
_tmp372->attributes,_tmp373->attributes)){Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=
t2;{const char*_tmpD3B;Cyc_Tcutil_failure_reason=((_tmpD3B="(different attributes on member)",
_tag_dyneither(_tmpD3B,sizeof(char),33)));}(int)_throw((void*)Cyc_Tcutil_Unify);}
if((_tmp372->width != 0  && _tmp373->width == 0  || _tmp373->width != 0  && _tmp372->width
== 0) || (_tmp372->width != 0  && _tmp373->width != 0) && !Cyc_Evexp_same_const_exp((
struct Cyc_Absyn_Exp*)_check_null(_tmp372->width),(struct Cyc_Absyn_Exp*)
_check_null(_tmp373->width))){Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;{
const char*_tmpD3C;Cyc_Tcutil_failure_reason=((_tmpD3C="(different bitfield widths on member)",
_tag_dyneither(_tmpD3C,sizeof(char),38)));}(int)_throw((void*)Cyc_Tcutil_Unify);}}
if(_tmp2F6 == 0  && _tmp2F3 == 0)return;Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=
t2;{const char*_tmpD3D;Cyc_Tcutil_failure_reason=((_tmpD3D="(different number of members)",
_tag_dyneither(_tmpD3D,sizeof(char),30)));}goto _LL21D;_LL242: _tmp2F7=_tmp275.f1;
if((int)_tmp2F7 != 2)goto _LL244;_tmp2F8=_tmp275.f2;if((int)_tmp2F8 != 2)goto _LL244;
_LL243: return;_LL244: _tmp2F9=_tmp275.f1;if((int)_tmp2F9 != 3)goto _LL246;_tmp2FA=
_tmp275.f2;if((int)_tmp2FA != 3)goto _LL246;_LL245: return;_LL246: _tmp2FB=_tmp275.f1;
if(_tmp2FB <= (void*)4)goto _LL248;if(*((int*)_tmp2FB)!= 14)goto _LL248;_tmp2FC=(
void*)((struct Cyc_Absyn_RgnHandleType_struct*)_tmp2FB)->f1;_tmp2FD=_tmp275.f2;
if(_tmp2FD <= (void*)4)goto _LL248;if(*((int*)_tmp2FD)!= 14)goto _LL248;_tmp2FE=(
void*)((struct Cyc_Absyn_RgnHandleType_struct*)_tmp2FD)->f1;_LL247: Cyc_Tcutil_unify_it(
_tmp2FC,_tmp2FE);return;_LL248: _tmp2FF=_tmp275.f1;if(_tmp2FF <= (void*)4)goto
_LL24A;if(*((int*)_tmp2FF)!= 15)goto _LL24A;_tmp300=(void*)((struct Cyc_Absyn_DynRgnType_struct*)
_tmp2FF)->f1;_tmp301=(void*)((struct Cyc_Absyn_DynRgnType_struct*)_tmp2FF)->f2;
_tmp302=_tmp275.f2;if(_tmp302 <= (void*)4)goto _LL24A;if(*((int*)_tmp302)!= 15)
goto _LL24A;_tmp303=(void*)((struct Cyc_Absyn_DynRgnType_struct*)_tmp302)->f1;
_tmp304=(void*)((struct Cyc_Absyn_DynRgnType_struct*)_tmp302)->f2;_LL249: Cyc_Tcutil_unify_it(
_tmp300,_tmp303);Cyc_Tcutil_unify_it(_tmp301,_tmp304);return;_LL24A: _tmp305=
_tmp275.f1;if(_tmp305 <= (void*)4)goto _LL24C;if(*((int*)_tmp305)!= 20)goto _LL24C;
_LL24B: goto _LL24D;_LL24C: _tmp306=_tmp275.f2;if(_tmp306 <= (void*)4)goto _LL24E;if(*((
int*)_tmp306)!= 20)goto _LL24E;_LL24D: goto _LL24F;_LL24E: _tmp307=_tmp275.f1;if(
_tmp307 <= (void*)4)goto _LL250;if(*((int*)_tmp307)!= 19)goto _LL250;_LL24F: goto
_LL251;_LL250: _tmp308=_tmp275.f1;if(_tmp308 <= (void*)4)goto _LL252;if(*((int*)
_tmp308)!= 21)goto _LL252;_LL251: goto _LL253;_LL252: _tmp309=_tmp275.f2;if(_tmp309
<= (void*)4)goto _LL254;if(*((int*)_tmp309)!= 21)goto _LL254;_LL253: goto _LL255;
_LL254: _tmp30A=_tmp275.f2;if(_tmp30A <= (void*)4)goto _LL256;if(*((int*)_tmp30A)!= 
19)goto _LL256;_LL255: if(Cyc_Tcutil_unify_effect(t1,t2))return;{const char*_tmpD3E;
Cyc_Tcutil_failure_reason=((_tmpD3E="(effects don't unify)",_tag_dyneither(
_tmpD3E,sizeof(char),22)));}goto _LL21D;_LL256:;_LL257: goto _LL21D;_LL21D:;}(int)
_throw((void*)Cyc_Tcutil_Unify);}int Cyc_Tcutil_star_cmp(int(*cmp)(void*,void*),
void*a1,void*a2);int Cyc_Tcutil_star_cmp(int(*cmp)(void*,void*),void*a1,void*a2){
if(a1 == a2)return 0;if(a1 == 0  && a2 != 0)return - 1;if(a1 != 0  && a2 == 0)return 1;
return cmp((void*)_check_null(a1),(void*)_check_null(a2));}static int Cyc_Tcutil_tqual_cmp(
struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2);static int Cyc_Tcutil_tqual_cmp(
struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2){int _tmp379=(tq1.real_const + (
tq1.q_volatile << 1))+ (tq1.q_restrict << 2);int _tmp37A=(tq2.real_const + (tq2.q_volatile
<< 1))+ (tq2.q_restrict << 2);return Cyc_Core_intcmp(_tmp379,_tmp37A);}static int
Cyc_Tcutil_conrefs_cmp(int(*cmp)(void*,void*),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*
y);static int Cyc_Tcutil_conrefs_cmp(int(*cmp)(void*,void*),struct Cyc_Absyn_Conref*
x,struct Cyc_Absyn_Conref*y){x=Cyc_Absyn_compress_conref(x);y=Cyc_Absyn_compress_conref(
y);if(x == y)return 0;{union Cyc_Absyn_Constraint_union _tmp37B=x->v;void*_tmp37C;
_LL270: if((_tmp37B.No_constr).tag != 2)goto _LL272;_LL271: return - 1;_LL272: if((
_tmp37B.Eq_constr).tag != 0)goto _LL274;_tmp37C=(_tmp37B.Eq_constr).f1;_LL273: {
union Cyc_Absyn_Constraint_union _tmp37D=y->v;void*_tmp37E;_LL277: if((_tmp37D.No_constr).tag
!= 2)goto _LL279;_LL278: return 1;_LL279: if((_tmp37D.Eq_constr).tag != 0)goto _LL27B;
_tmp37E=(_tmp37D.Eq_constr).f1;_LL27A: return cmp(_tmp37C,_tmp37E);_LL27B: if((
_tmp37D.Forward_constr).tag != 1)goto _LL276;_LL27C: {const char*_tmpD41;void*
_tmpD40;(_tmpD40=0,Cyc_Tcutil_impos(((_tmpD41="unify_conref: forward after compress(2)",
_tag_dyneither(_tmpD41,sizeof(char),40))),_tag_dyneither(_tmpD40,sizeof(void*),0)));}
_LL276:;}_LL274: if((_tmp37B.Forward_constr).tag != 1)goto _LL26F;_LL275: {const
char*_tmpD44;void*_tmpD43;(_tmpD43=0,Cyc_Tcutil_impos(((_tmpD44="unify_conref: forward after compress",
_tag_dyneither(_tmpD44,sizeof(char),37))),_tag_dyneither(_tmpD43,sizeof(void*),0)));}
_LL26F:;}}static int Cyc_Tcutil_tqual_type_cmp(struct _tuple5*tqt1,struct _tuple5*
tqt2);static int Cyc_Tcutil_tqual_type_cmp(struct _tuple5*tqt1,struct _tuple5*tqt2){
struct _tuple5 _tmp384;struct Cyc_Absyn_Tqual _tmp385;void*_tmp386;struct _tuple5*
_tmp383=tqt1;_tmp384=*_tmp383;_tmp385=_tmp384.f1;_tmp386=_tmp384.f2;{struct
_tuple5 _tmp388;struct Cyc_Absyn_Tqual _tmp389;void*_tmp38A;struct _tuple5*_tmp387=
tqt2;_tmp388=*_tmp387;_tmp389=_tmp388.f1;_tmp38A=_tmp388.f2;{int _tmp38B=Cyc_Tcutil_tqual_cmp(
_tmp385,_tmp389);if(_tmp38B != 0)return _tmp38B;return Cyc_Tcutil_typecmp(_tmp386,
_tmp38A);}}}static int Cyc_Tcutil_aggrfield_cmp(struct Cyc_Absyn_Aggrfield*f1,
struct Cyc_Absyn_Aggrfield*f2);static int Cyc_Tcutil_aggrfield_cmp(struct Cyc_Absyn_Aggrfield*
f1,struct Cyc_Absyn_Aggrfield*f2){int _tmp38C=Cyc_strptrcmp(f1->name,f2->name);if(
_tmp38C != 0)return _tmp38C;{int _tmp38D=Cyc_Tcutil_tqual_cmp(f1->tq,f2->tq);if(
_tmp38D != 0)return _tmp38D;{int _tmp38E=Cyc_Tcutil_typecmp((void*)f1->type,(void*)
f2->type);if(_tmp38E != 0)return _tmp38E;{int _tmp38F=Cyc_List_list_cmp(Cyc_Tcutil_attribute_cmp,
f1->attributes,f2->attributes);if(_tmp38F != 0)return _tmp38F;return((int(*)(int(*
cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*
a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,f1->width,f2->width);}}}}static
int Cyc_Tcutil_enumfield_cmp(struct Cyc_Absyn_Enumfield*e1,struct Cyc_Absyn_Enumfield*
e2);static int Cyc_Tcutil_enumfield_cmp(struct Cyc_Absyn_Enumfield*e1,struct Cyc_Absyn_Enumfield*
e2){int _tmp390=Cyc_Absyn_qvar_cmp(e1->name,e2->name);if(_tmp390 != 0)return
_tmp390;return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),
struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,
e1->tag,e2->tag);}static int Cyc_Tcutil_type_case_number(void*t);static int Cyc_Tcutil_type_case_number(
void*t){void*_tmp391=t;_LL27E: if((int)_tmp391 != 0)goto _LL280;_LL27F: return 0;
_LL280: if(_tmp391 <= (void*)4)goto _LL28C;if(*((int*)_tmp391)!= 0)goto _LL282;
_LL281: return 1;_LL282: if(*((int*)_tmp391)!= 1)goto _LL284;_LL283: return 2;_LL284:
if(*((int*)_tmp391)!= 2)goto _LL286;_LL285: return 3;_LL286: if(*((int*)_tmp391)!= 3)
goto _LL288;_LL287: return 4;_LL288: if(*((int*)_tmp391)!= 4)goto _LL28A;_LL289:
return 5;_LL28A: if(*((int*)_tmp391)!= 5)goto _LL28C;_LL28B: return 6;_LL28C: if((int)
_tmp391 != 1)goto _LL28E;_LL28D: return 7;_LL28E: if(_tmp391 <= (void*)4)goto _LL2A2;
if(*((int*)_tmp391)!= 6)goto _LL290;_LL28F: return 8;_LL290: if(*((int*)_tmp391)!= 7)
goto _LL292;_LL291: return 9;_LL292: if(*((int*)_tmp391)!= 8)goto _LL294;_LL293:
return 10;_LL294: if(*((int*)_tmp391)!= 9)goto _LL296;_LL295: return 11;_LL296: if(*((
int*)_tmp391)!= 10)goto _LL298;_LL297: return 12;_LL298: if(*((int*)_tmp391)!= 11)
goto _LL29A;_LL299: return 14;_LL29A: if(*((int*)_tmp391)!= 12)goto _LL29C;_LL29B:
return 16;_LL29C: if(*((int*)_tmp391)!= 13)goto _LL29E;_LL29D: return 17;_LL29E: if(*((
int*)_tmp391)!= 14)goto _LL2A0;_LL29F: return 18;_LL2A0: if(*((int*)_tmp391)!= 16)
goto _LL2A2;_LL2A1: return 19;_LL2A2: if((int)_tmp391 != 3)goto _LL2A4;_LL2A3: return 20;
_LL2A4: if((int)_tmp391 != 2)goto _LL2A6;_LL2A5: return 21;_LL2A6: if(_tmp391 <= (void*)
4)goto _LL2A8;if(*((int*)_tmp391)!= 19)goto _LL2A8;_LL2A7: return 22;_LL2A8: if(
_tmp391 <= (void*)4)goto _LL2AA;if(*((int*)_tmp391)!= 20)goto _LL2AA;_LL2A9: return
23;_LL2AA: if(_tmp391 <= (void*)4)goto _LL2AC;if(*((int*)_tmp391)!= 21)goto _LL2AC;
_LL2AB: return 24;_LL2AC: if(_tmp391 <= (void*)4)goto _LL2AE;if(*((int*)_tmp391)!= 18)
goto _LL2AE;_LL2AD: return 26;_LL2AE: if(_tmp391 <= (void*)4)goto _LL2B0;if(*((int*)
_tmp391)!= 15)goto _LL2B0;_LL2AF: return 27;_LL2B0: if(_tmp391 <= (void*)4)goto _LL27D;
if(*((int*)_tmp391)!= 17)goto _LL27D;_LL2B1: return 28;_LL27D:;}int Cyc_Tcutil_typecmp(
void*t1,void*t2);int Cyc_Tcutil_typecmp(void*t1,void*t2){t1=Cyc_Tcutil_compress(
t1);t2=Cyc_Tcutil_compress(t2);if(t1 == t2)return 0;{int _tmp392=Cyc_Core_intcmp(
Cyc_Tcutil_type_case_number(t1),Cyc_Tcutil_type_case_number(t2));if(_tmp392 != 0)
return _tmp392;{struct _tuple0 _tmpD45;struct _tuple0 _tmp394=(_tmpD45.f1=t2,((
_tmpD45.f2=t1,_tmpD45)));void*_tmp395;void*_tmp396;void*_tmp397;struct Cyc_Absyn_Tvar*
_tmp398;void*_tmp399;struct Cyc_Absyn_Tvar*_tmp39A;void*_tmp39B;struct Cyc_Absyn_AggrInfo
_tmp39C;union Cyc_Absyn_AggrInfoU_union _tmp39D;struct Cyc_List_List*_tmp39E;void*
_tmp39F;struct Cyc_Absyn_AggrInfo _tmp3A0;union Cyc_Absyn_AggrInfoU_union _tmp3A1;
struct Cyc_List_List*_tmp3A2;void*_tmp3A3;struct _tuple2*_tmp3A4;void*_tmp3A5;
struct _tuple2*_tmp3A6;void*_tmp3A7;struct Cyc_List_List*_tmp3A8;void*_tmp3A9;
struct Cyc_List_List*_tmp3AA;void*_tmp3AB;struct Cyc_Absyn_TunionInfo _tmp3AC;union
Cyc_Absyn_TunionInfoU_union _tmp3AD;struct Cyc_Absyn_Tuniondecl**_tmp3AE;struct Cyc_Absyn_Tuniondecl*
_tmp3AF;struct Cyc_List_List*_tmp3B0;struct Cyc_Core_Opt*_tmp3B1;void*_tmp3B2;
struct Cyc_Absyn_TunionInfo _tmp3B3;union Cyc_Absyn_TunionInfoU_union _tmp3B4;struct
Cyc_Absyn_Tuniondecl**_tmp3B5;struct Cyc_Absyn_Tuniondecl*_tmp3B6;struct Cyc_List_List*
_tmp3B7;struct Cyc_Core_Opt*_tmp3B8;void*_tmp3B9;struct Cyc_Absyn_TunionFieldInfo
_tmp3BA;union Cyc_Absyn_TunionFieldInfoU_union _tmp3BB;struct Cyc_Absyn_Tuniondecl*
_tmp3BC;struct Cyc_Absyn_Tunionfield*_tmp3BD;struct Cyc_List_List*_tmp3BE;void*
_tmp3BF;struct Cyc_Absyn_TunionFieldInfo _tmp3C0;union Cyc_Absyn_TunionFieldInfoU_union
_tmp3C1;struct Cyc_Absyn_Tuniondecl*_tmp3C2;struct Cyc_Absyn_Tunionfield*_tmp3C3;
struct Cyc_List_List*_tmp3C4;void*_tmp3C5;struct Cyc_Absyn_PtrInfo _tmp3C6;void*
_tmp3C7;struct Cyc_Absyn_Tqual _tmp3C8;struct Cyc_Absyn_PtrAtts _tmp3C9;void*_tmp3CA;
struct Cyc_Absyn_Conref*_tmp3CB;struct Cyc_Absyn_Conref*_tmp3CC;struct Cyc_Absyn_Conref*
_tmp3CD;void*_tmp3CE;struct Cyc_Absyn_PtrInfo _tmp3CF;void*_tmp3D0;struct Cyc_Absyn_Tqual
_tmp3D1;struct Cyc_Absyn_PtrAtts _tmp3D2;void*_tmp3D3;struct Cyc_Absyn_Conref*
_tmp3D4;struct Cyc_Absyn_Conref*_tmp3D5;struct Cyc_Absyn_Conref*_tmp3D6;void*
_tmp3D7;void*_tmp3D8;void*_tmp3D9;void*_tmp3DA;void*_tmp3DB;void*_tmp3DC;void*
_tmp3DD;void*_tmp3DE;void*_tmp3DF;int _tmp3E0;void*_tmp3E1;int _tmp3E2;void*
_tmp3E3;struct Cyc_Absyn_ArrayInfo _tmp3E4;void*_tmp3E5;struct Cyc_Absyn_Tqual
_tmp3E6;struct Cyc_Absyn_Exp*_tmp3E7;struct Cyc_Absyn_Conref*_tmp3E8;void*_tmp3E9;
struct Cyc_Absyn_ArrayInfo _tmp3EA;void*_tmp3EB;struct Cyc_Absyn_Tqual _tmp3EC;
struct Cyc_Absyn_Exp*_tmp3ED;struct Cyc_Absyn_Conref*_tmp3EE;void*_tmp3EF;struct
Cyc_Absyn_FnInfo _tmp3F0;struct Cyc_List_List*_tmp3F1;struct Cyc_Core_Opt*_tmp3F2;
void*_tmp3F3;struct Cyc_List_List*_tmp3F4;int _tmp3F5;struct Cyc_Absyn_VarargInfo*
_tmp3F6;struct Cyc_List_List*_tmp3F7;struct Cyc_List_List*_tmp3F8;void*_tmp3F9;
struct Cyc_Absyn_FnInfo _tmp3FA;struct Cyc_List_List*_tmp3FB;struct Cyc_Core_Opt*
_tmp3FC;void*_tmp3FD;struct Cyc_List_List*_tmp3FE;int _tmp3FF;struct Cyc_Absyn_VarargInfo*
_tmp400;struct Cyc_List_List*_tmp401;struct Cyc_List_List*_tmp402;void*_tmp403;
struct Cyc_List_List*_tmp404;void*_tmp405;struct Cyc_List_List*_tmp406;void*
_tmp407;void*_tmp408;struct Cyc_List_List*_tmp409;void*_tmp40A;void*_tmp40B;
struct Cyc_List_List*_tmp40C;void*_tmp40D;void*_tmp40E;void*_tmp40F;void*_tmp410;
void*_tmp411;void*_tmp412;void*_tmp413;void*_tmp414;void*_tmp415;void*_tmp416;
void*_tmp417;void*_tmp418;void*_tmp419;void*_tmp41A;void*_tmp41B;struct Cyc_Absyn_Exp*
_tmp41C;void*_tmp41D;struct Cyc_Absyn_Exp*_tmp41E;void*_tmp41F;void*_tmp420;void*
_tmp421;void*_tmp422;void*_tmp423;void*_tmp424;_LL2B3: _tmp395=_tmp394.f1;if(
_tmp395 <= (void*)4)goto _LL2B5;if(*((int*)_tmp395)!= 0)goto _LL2B5;_tmp396=_tmp394.f2;
if(_tmp396 <= (void*)4)goto _LL2B5;if(*((int*)_tmp396)!= 0)goto _LL2B5;_LL2B4: {
const char*_tmpD48;void*_tmpD47;(_tmpD47=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpD48="typecmp: can only compare closed types",
_tag_dyneither(_tmpD48,sizeof(char),39))),_tag_dyneither(_tmpD47,sizeof(void*),0)));}
_LL2B5: _tmp397=_tmp394.f1;if(_tmp397 <= (void*)4)goto _LL2B7;if(*((int*)_tmp397)!= 
1)goto _LL2B7;_tmp398=((struct Cyc_Absyn_VarType_struct*)_tmp397)->f1;_tmp399=
_tmp394.f2;if(_tmp399 <= (void*)4)goto _LL2B7;if(*((int*)_tmp399)!= 1)goto _LL2B7;
_tmp39A=((struct Cyc_Absyn_VarType_struct*)_tmp399)->f1;_LL2B6: return Cyc_Core_intcmp(
_tmp39A->identity,_tmp398->identity);_LL2B7: _tmp39B=_tmp394.f1;if(_tmp39B <= (
void*)4)goto _LL2B9;if(*((int*)_tmp39B)!= 10)goto _LL2B9;_tmp39C=((struct Cyc_Absyn_AggrType_struct*)
_tmp39B)->f1;_tmp39D=_tmp39C.aggr_info;_tmp39E=_tmp39C.targs;_tmp39F=_tmp394.f2;
if(_tmp39F <= (void*)4)goto _LL2B9;if(*((int*)_tmp39F)!= 10)goto _LL2B9;_tmp3A0=((
struct Cyc_Absyn_AggrType_struct*)_tmp39F)->f1;_tmp3A1=_tmp3A0.aggr_info;_tmp3A2=
_tmp3A0.targs;_LL2B8: {struct _tuple2*_tmp428;struct _tuple6 _tmp427=Cyc_Absyn_aggr_kinded_name(
_tmp39D);_tmp428=_tmp427.f2;{struct _tuple2*_tmp42A;struct _tuple6 _tmp429=Cyc_Absyn_aggr_kinded_name(
_tmp3A1);_tmp42A=_tmp429.f2;{int _tmp42B=Cyc_Absyn_qvar_cmp(_tmp428,_tmp42A);if(
_tmp42B != 0)return _tmp42B;else{return Cyc_List_list_cmp(Cyc_Tcutil_typecmp,
_tmp39E,_tmp3A2);}}}}_LL2B9: _tmp3A3=_tmp394.f1;if(_tmp3A3 <= (void*)4)goto _LL2BB;
if(*((int*)_tmp3A3)!= 12)goto _LL2BB;_tmp3A4=((struct Cyc_Absyn_EnumType_struct*)
_tmp3A3)->f1;_tmp3A5=_tmp394.f2;if(_tmp3A5 <= (void*)4)goto _LL2BB;if(*((int*)
_tmp3A5)!= 12)goto _LL2BB;_tmp3A6=((struct Cyc_Absyn_EnumType_struct*)_tmp3A5)->f1;
_LL2BA: return Cyc_Absyn_qvar_cmp(_tmp3A4,_tmp3A6);_LL2BB: _tmp3A7=_tmp394.f1;if(
_tmp3A7 <= (void*)4)goto _LL2BD;if(*((int*)_tmp3A7)!= 13)goto _LL2BD;_tmp3A8=((
struct Cyc_Absyn_AnonEnumType_struct*)_tmp3A7)->f1;_tmp3A9=_tmp394.f2;if(_tmp3A9
<= (void*)4)goto _LL2BD;if(*((int*)_tmp3A9)!= 13)goto _LL2BD;_tmp3AA=((struct Cyc_Absyn_AnonEnumType_struct*)
_tmp3A9)->f1;_LL2BC: return((int(*)(int(*cmp)(struct Cyc_Absyn_Enumfield*,struct
Cyc_Absyn_Enumfield*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(
Cyc_Tcutil_enumfield_cmp,_tmp3A8,_tmp3AA);_LL2BD: _tmp3AB=_tmp394.f1;if(_tmp3AB <= (
void*)4)goto _LL2BF;if(*((int*)_tmp3AB)!= 2)goto _LL2BF;_tmp3AC=((struct Cyc_Absyn_TunionType_struct*)
_tmp3AB)->f1;_tmp3AD=_tmp3AC.tunion_info;if((((((struct Cyc_Absyn_TunionType_struct*)
_tmp3AB)->f1).tunion_info).KnownTunion).tag != 1)goto _LL2BF;_tmp3AE=(_tmp3AD.KnownTunion).f1;
_tmp3AF=*_tmp3AE;_tmp3B0=_tmp3AC.targs;_tmp3B1=_tmp3AC.rgn;_tmp3B2=_tmp394.f2;
if(_tmp3B2 <= (void*)4)goto _LL2BF;if(*((int*)_tmp3B2)!= 2)goto _LL2BF;_tmp3B3=((
struct Cyc_Absyn_TunionType_struct*)_tmp3B2)->f1;_tmp3B4=_tmp3B3.tunion_info;if((((((
struct Cyc_Absyn_TunionType_struct*)_tmp3B2)->f1).tunion_info).KnownTunion).tag != 
1)goto _LL2BF;_tmp3B5=(_tmp3B4.KnownTunion).f1;_tmp3B6=*_tmp3B5;_tmp3B7=_tmp3B3.targs;
_tmp3B8=_tmp3B3.rgn;_LL2BE: if(_tmp3B6 == _tmp3AF)return 0;{int _tmp42C=Cyc_Absyn_qvar_cmp(
_tmp3B6->name,_tmp3AF->name);if(_tmp42C != 0)return _tmp42C;if((unsigned int)
_tmp3B8  && (unsigned int)_tmp3B1){int _tmp42D=Cyc_Tcutil_typecmp((void*)_tmp3B8->v,(
void*)_tmp3B1->v);if(_tmp42D != 0)return _tmp42D;}else{if((unsigned int)_tmp3B8)
return - 1;else{return 1;}}return Cyc_List_list_cmp(Cyc_Tcutil_typecmp,_tmp3B7,
_tmp3B0);}_LL2BF: _tmp3B9=_tmp394.f1;if(_tmp3B9 <= (void*)4)goto _LL2C1;if(*((int*)
_tmp3B9)!= 3)goto _LL2C1;_tmp3BA=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp3B9)->f1;_tmp3BB=_tmp3BA.field_info;if((((((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp3B9)->f1).field_info).KnownTunionfield).tag != 1)goto _LL2C1;_tmp3BC=(_tmp3BB.KnownTunionfield).f1;
_tmp3BD=(_tmp3BB.KnownTunionfield).f2;_tmp3BE=_tmp3BA.targs;_tmp3BF=_tmp394.f2;
if(_tmp3BF <= (void*)4)goto _LL2C1;if(*((int*)_tmp3BF)!= 3)goto _LL2C1;_tmp3C0=((
struct Cyc_Absyn_TunionFieldType_struct*)_tmp3BF)->f1;_tmp3C1=_tmp3C0.field_info;
if((((((struct Cyc_Absyn_TunionFieldType_struct*)_tmp3BF)->f1).field_info).KnownTunionfield).tag
!= 1)goto _LL2C1;_tmp3C2=(_tmp3C1.KnownTunionfield).f1;_tmp3C3=(_tmp3C1.KnownTunionfield).f2;
_tmp3C4=_tmp3C0.targs;_LL2C0: if(_tmp3C2 == _tmp3BC)return 0;{int _tmp42E=Cyc_Absyn_qvar_cmp(
_tmp3BC->name,_tmp3C2->name);if(_tmp42E != 0)return _tmp42E;{int _tmp42F=Cyc_Absyn_qvar_cmp(
_tmp3BD->name,_tmp3C3->name);if(_tmp42F != 0)return _tmp42F;return Cyc_List_list_cmp(
Cyc_Tcutil_typecmp,_tmp3C4,_tmp3BE);}}_LL2C1: _tmp3C5=_tmp394.f1;if(_tmp3C5 <= (
void*)4)goto _LL2C3;if(*((int*)_tmp3C5)!= 4)goto _LL2C3;_tmp3C6=((struct Cyc_Absyn_PointerType_struct*)
_tmp3C5)->f1;_tmp3C7=(void*)_tmp3C6.elt_typ;_tmp3C8=_tmp3C6.elt_tq;_tmp3C9=
_tmp3C6.ptr_atts;_tmp3CA=(void*)_tmp3C9.rgn;_tmp3CB=_tmp3C9.nullable;_tmp3CC=
_tmp3C9.bounds;_tmp3CD=_tmp3C9.zero_term;_tmp3CE=_tmp394.f2;if(_tmp3CE <= (void*)
4)goto _LL2C3;if(*((int*)_tmp3CE)!= 4)goto _LL2C3;_tmp3CF=((struct Cyc_Absyn_PointerType_struct*)
_tmp3CE)->f1;_tmp3D0=(void*)_tmp3CF.elt_typ;_tmp3D1=_tmp3CF.elt_tq;_tmp3D2=
_tmp3CF.ptr_atts;_tmp3D3=(void*)_tmp3D2.rgn;_tmp3D4=_tmp3D2.nullable;_tmp3D5=
_tmp3D2.bounds;_tmp3D6=_tmp3D2.zero_term;_LL2C2: {int _tmp430=Cyc_Tcutil_typecmp(
_tmp3D0,_tmp3C7);if(_tmp430 != 0)return _tmp430;{int _tmp431=Cyc_Tcutil_typecmp(
_tmp3D3,_tmp3CA);if(_tmp431 != 0)return _tmp431;{int _tmp432=Cyc_Tcutil_tqual_cmp(
_tmp3D1,_tmp3C8);if(_tmp432 != 0)return _tmp432;{int _tmp433=Cyc_Tcutil_conrefs_cmp(
Cyc_Tcutil_boundscmp,_tmp3D5,_tmp3CC);if(_tmp433 != 0)return _tmp433;{int _tmp434=((
int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_conrefs_cmp)(
Cyc_Core_intcmp,_tmp3D6,_tmp3CD);if(_tmp434 != 0)return _tmp434;{union Cyc_Absyn_Constraint_union
_tmp435=(Cyc_Absyn_compress_conref(_tmp3D5))->v;void*_tmp436;_LL2E8: if((_tmp435.Eq_constr).tag
!= 0)goto _LL2EA;_tmp436=(_tmp435.Eq_constr).f1;if((int)_tmp436 != 0)goto _LL2EA;
_LL2E9: return 0;_LL2EA:;_LL2EB: goto _LL2E7;_LL2E7:;}return((int(*)(int(*cmp)(int,
int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_conrefs_cmp)(
Cyc_Core_intcmp,_tmp3D4,_tmp3CB);}}}}}_LL2C3: _tmp3D7=_tmp394.f1;if(_tmp3D7 <= (
void*)4)goto _LL2C5;if(*((int*)_tmp3D7)!= 5)goto _LL2C5;_tmp3D8=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp3D7)->f1;_tmp3D9=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp3D7)->f2;
_tmp3DA=_tmp394.f2;if(_tmp3DA <= (void*)4)goto _LL2C5;if(*((int*)_tmp3DA)!= 5)goto
_LL2C5;_tmp3DB=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp3DA)->f1;_tmp3DC=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp3DA)->f2;_LL2C4: if(_tmp3DB != _tmp3D8)
return Cyc_Core_intcmp((int)((unsigned int)_tmp3DB),(int)((unsigned int)_tmp3D8));
if(_tmp3DC != _tmp3D9)return Cyc_Core_intcmp((int)((unsigned int)_tmp3DC),(int)((
unsigned int)_tmp3D9));return 0;_LL2C5: _tmp3DD=_tmp394.f1;if((int)_tmp3DD != 1)
goto _LL2C7;_tmp3DE=_tmp394.f2;if((int)_tmp3DE != 1)goto _LL2C7;_LL2C6: return 0;
_LL2C7: _tmp3DF=_tmp394.f1;if(_tmp3DF <= (void*)4)goto _LL2C9;if(*((int*)_tmp3DF)!= 
6)goto _LL2C9;_tmp3E0=((struct Cyc_Absyn_DoubleType_struct*)_tmp3DF)->f1;_tmp3E1=
_tmp394.f2;if(_tmp3E1 <= (void*)4)goto _LL2C9;if(*((int*)_tmp3E1)!= 6)goto _LL2C9;
_tmp3E2=((struct Cyc_Absyn_DoubleType_struct*)_tmp3E1)->f1;_LL2C8: if(_tmp3E0 == 
_tmp3E2)return 0;else{if(_tmp3E0)return - 1;else{return 1;}}_LL2C9: _tmp3E3=_tmp394.f1;
if(_tmp3E3 <= (void*)4)goto _LL2CB;if(*((int*)_tmp3E3)!= 7)goto _LL2CB;_tmp3E4=((
struct Cyc_Absyn_ArrayType_struct*)_tmp3E3)->f1;_tmp3E5=(void*)_tmp3E4.elt_type;
_tmp3E6=_tmp3E4.tq;_tmp3E7=_tmp3E4.num_elts;_tmp3E8=_tmp3E4.zero_term;_tmp3E9=
_tmp394.f2;if(_tmp3E9 <= (void*)4)goto _LL2CB;if(*((int*)_tmp3E9)!= 7)goto _LL2CB;
_tmp3EA=((struct Cyc_Absyn_ArrayType_struct*)_tmp3E9)->f1;_tmp3EB=(void*)_tmp3EA.elt_type;
_tmp3EC=_tmp3EA.tq;_tmp3ED=_tmp3EA.num_elts;_tmp3EE=_tmp3EA.zero_term;_LL2CA: {
int _tmp437=Cyc_Tcutil_tqual_cmp(_tmp3EC,_tmp3E6);if(_tmp437 != 0)return _tmp437;{
int _tmp438=Cyc_Tcutil_typecmp(_tmp3EB,_tmp3E5);if(_tmp438 != 0)return _tmp438;{int
_tmp439=((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*
y))Cyc_Tcutil_conrefs_cmp)(Cyc_Core_intcmp,_tmp3E8,_tmp3EE);if(_tmp439 != 0)
return _tmp439;if(_tmp3E7 == _tmp3ED)return 0;if(_tmp3E7 == 0  || _tmp3ED == 0){const
char*_tmpD4B;void*_tmpD4A;(_tmpD4A=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpD4B="missing expression in array index",
_tag_dyneither(_tmpD4B,sizeof(char),34))),_tag_dyneither(_tmpD4A,sizeof(void*),0)));}
return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*
a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,_tmp3E7,
_tmp3ED);}}}_LL2CB: _tmp3EF=_tmp394.f1;if(_tmp3EF <= (void*)4)goto _LL2CD;if(*((int*)
_tmp3EF)!= 8)goto _LL2CD;_tmp3F0=((struct Cyc_Absyn_FnType_struct*)_tmp3EF)->f1;
_tmp3F1=_tmp3F0.tvars;_tmp3F2=_tmp3F0.effect;_tmp3F3=(void*)_tmp3F0.ret_typ;
_tmp3F4=_tmp3F0.args;_tmp3F5=_tmp3F0.c_varargs;_tmp3F6=_tmp3F0.cyc_varargs;
_tmp3F7=_tmp3F0.rgn_po;_tmp3F8=_tmp3F0.attributes;_tmp3F9=_tmp394.f2;if(_tmp3F9
<= (void*)4)goto _LL2CD;if(*((int*)_tmp3F9)!= 8)goto _LL2CD;_tmp3FA=((struct Cyc_Absyn_FnType_struct*)
_tmp3F9)->f1;_tmp3FB=_tmp3FA.tvars;_tmp3FC=_tmp3FA.effect;_tmp3FD=(void*)_tmp3FA.ret_typ;
_tmp3FE=_tmp3FA.args;_tmp3FF=_tmp3FA.c_varargs;_tmp400=_tmp3FA.cyc_varargs;
_tmp401=_tmp3FA.rgn_po;_tmp402=_tmp3FA.attributes;_LL2CC: {const char*_tmpD4E;
void*_tmpD4D;(_tmpD4D=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmpD4E="typecmp: function types not handled",_tag_dyneither(
_tmpD4E,sizeof(char),36))),_tag_dyneither(_tmpD4D,sizeof(void*),0)));}_LL2CD:
_tmp403=_tmp394.f1;if(_tmp403 <= (void*)4)goto _LL2CF;if(*((int*)_tmp403)!= 9)goto
_LL2CF;_tmp404=((struct Cyc_Absyn_TupleType_struct*)_tmp403)->f1;_tmp405=_tmp394.f2;
if(_tmp405 <= (void*)4)goto _LL2CF;if(*((int*)_tmp405)!= 9)goto _LL2CF;_tmp406=((
struct Cyc_Absyn_TupleType_struct*)_tmp405)->f1;_LL2CE: return((int(*)(int(*cmp)(
struct _tuple5*,struct _tuple5*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))
Cyc_List_list_cmp)(Cyc_Tcutil_tqual_type_cmp,_tmp406,_tmp404);_LL2CF: _tmp407=
_tmp394.f1;if(_tmp407 <= (void*)4)goto _LL2D1;if(*((int*)_tmp407)!= 11)goto _LL2D1;
_tmp408=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)_tmp407)->f1;_tmp409=((
struct Cyc_Absyn_AnonAggrType_struct*)_tmp407)->f2;_tmp40A=_tmp394.f2;if(_tmp40A
<= (void*)4)goto _LL2D1;if(*((int*)_tmp40A)!= 11)goto _LL2D1;_tmp40B=(void*)((
struct Cyc_Absyn_AnonAggrType_struct*)_tmp40A)->f1;_tmp40C=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp40A)->f2;_LL2D0: if(_tmp40B != _tmp408){if(_tmp40B == (void*)0)return - 1;else{
return 1;}}return((int(*)(int(*cmp)(struct Cyc_Absyn_Aggrfield*,struct Cyc_Absyn_Aggrfield*),
struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_aggrfield_cmp,
_tmp40C,_tmp409);_LL2D1: _tmp40D=_tmp394.f1;if(_tmp40D <= (void*)4)goto _LL2D3;if(*((
int*)_tmp40D)!= 14)goto _LL2D3;_tmp40E=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp40D)->f1;_tmp40F=_tmp394.f2;if(_tmp40F <= (void*)4)goto _LL2D3;if(*((int*)
_tmp40F)!= 14)goto _LL2D3;_tmp410=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp40F)->f1;_LL2D2: return Cyc_Tcutil_typecmp(_tmp40E,_tmp410);_LL2D3: _tmp411=
_tmp394.f1;if(_tmp411 <= (void*)4)goto _LL2D5;if(*((int*)_tmp411)!= 15)goto _LL2D5;
_tmp412=(void*)((struct Cyc_Absyn_DynRgnType_struct*)_tmp411)->f1;_tmp413=(void*)((
struct Cyc_Absyn_DynRgnType_struct*)_tmp411)->f2;_tmp414=_tmp394.f2;if(_tmp414 <= (
void*)4)goto _LL2D5;if(*((int*)_tmp414)!= 15)goto _LL2D5;_tmp415=(void*)((struct
Cyc_Absyn_DynRgnType_struct*)_tmp414)->f1;_tmp416=(void*)((struct Cyc_Absyn_DynRgnType_struct*)
_tmp414)->f2;_LL2D4: {int _tmp43E=Cyc_Tcutil_typecmp(_tmp412,_tmp415);if(_tmp43E
!= 0)return _tmp43E;else{return Cyc_Tcutil_typecmp(_tmp413,_tmp416);}}_LL2D5:
_tmp417=_tmp394.f1;if(_tmp417 <= (void*)4)goto _LL2D7;if(*((int*)_tmp417)!= 18)
goto _LL2D7;_tmp418=(void*)((struct Cyc_Absyn_TagType_struct*)_tmp417)->f1;_tmp419=
_tmp394.f2;if(_tmp419 <= (void*)4)goto _LL2D7;if(*((int*)_tmp419)!= 18)goto _LL2D7;
_tmp41A=(void*)((struct Cyc_Absyn_TagType_struct*)_tmp419)->f1;_LL2D6: return Cyc_Tcutil_typecmp(
_tmp418,_tmp41A);_LL2D7: _tmp41B=_tmp394.f1;if(_tmp41B <= (void*)4)goto _LL2D9;if(*((
int*)_tmp41B)!= 17)goto _LL2D9;_tmp41C=((struct Cyc_Absyn_ValueofType_struct*)
_tmp41B)->f1;_tmp41D=_tmp394.f2;if(_tmp41D <= (void*)4)goto _LL2D9;if(*((int*)
_tmp41D)!= 17)goto _LL2D9;_tmp41E=((struct Cyc_Absyn_ValueofType_struct*)_tmp41D)->f1;
_LL2D8: return Cyc_Evexp_const_exp_cmp(_tmp41C,_tmp41E);_LL2D9: _tmp41F=_tmp394.f1;
if(_tmp41F <= (void*)4)goto _LL2DB;if(*((int*)_tmp41F)!= 20)goto _LL2DB;_LL2DA: goto
_LL2DC;_LL2DB: _tmp420=_tmp394.f2;if(_tmp420 <= (void*)4)goto _LL2DD;if(*((int*)
_tmp420)!= 20)goto _LL2DD;_LL2DC: goto _LL2DE;_LL2DD: _tmp421=_tmp394.f1;if(_tmp421
<= (void*)4)goto _LL2DF;if(*((int*)_tmp421)!= 19)goto _LL2DF;_LL2DE: goto _LL2E0;
_LL2DF: _tmp422=_tmp394.f1;if(_tmp422 <= (void*)4)goto _LL2E1;if(*((int*)_tmp422)!= 
21)goto _LL2E1;_LL2E0: goto _LL2E2;_LL2E1: _tmp423=_tmp394.f2;if(_tmp423 <= (void*)4)
goto _LL2E3;if(*((int*)_tmp423)!= 21)goto _LL2E3;_LL2E2: goto _LL2E4;_LL2E3: _tmp424=
_tmp394.f2;if(_tmp424 <= (void*)4)goto _LL2E5;if(*((int*)_tmp424)!= 19)goto _LL2E5;
_LL2E4: {const char*_tmpD51;void*_tmpD50;(_tmpD50=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpD51="typecmp: effects not handled",
_tag_dyneither(_tmpD51,sizeof(char),29))),_tag_dyneither(_tmpD50,sizeof(void*),0)));}
_LL2E5:;_LL2E6: {const char*_tmpD54;void*_tmpD53;(_tmpD53=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpD54="Unmatched case in typecmp",
_tag_dyneither(_tmpD54,sizeof(char),26))),_tag_dyneither(_tmpD53,sizeof(void*),0)));}
_LL2B2:;}}}int Cyc_Tcutil_is_arithmetic_type(void*t);int Cyc_Tcutil_is_arithmetic_type(
void*t){void*_tmp443=Cyc_Tcutil_compress(t);_LL2ED: if(_tmp443 <= (void*)4)goto
_LL2EF;if(*((int*)_tmp443)!= 5)goto _LL2EF;_LL2EE: goto _LL2F0;_LL2EF: if((int)
_tmp443 != 1)goto _LL2F1;_LL2F0: goto _LL2F2;_LL2F1: if(_tmp443 <= (void*)4)goto _LL2F7;
if(*((int*)_tmp443)!= 6)goto _LL2F3;_LL2F2: goto _LL2F4;_LL2F3: if(*((int*)_tmp443)
!= 12)goto _LL2F5;_LL2F4: goto _LL2F6;_LL2F5: if(*((int*)_tmp443)!= 13)goto _LL2F7;
_LL2F6: return 1;_LL2F7:;_LL2F8: return 0;_LL2EC:;}int Cyc_Tcutil_will_lose_precision(
void*t1,void*t2);int Cyc_Tcutil_will_lose_precision(void*t1,void*t2){t1=Cyc_Tcutil_compress(
t1);t2=Cyc_Tcutil_compress(t2);{struct _tuple0 _tmpD55;struct _tuple0 _tmp445=(
_tmpD55.f1=t1,((_tmpD55.f2=t2,_tmpD55)));void*_tmp446;int _tmp447;void*_tmp448;
int _tmp449;void*_tmp44A;void*_tmp44B;void*_tmp44C;void*_tmp44D;void*_tmp44E;void*
_tmp44F;void*_tmp450;void*_tmp451;void*_tmp452;void*_tmp453;void*_tmp454;void*
_tmp455;void*_tmp456;void*_tmp457;void*_tmp458;void*_tmp459;void*_tmp45A;void*
_tmp45B;void*_tmp45C;void*_tmp45D;void*_tmp45E;void*_tmp45F;void*_tmp460;void*
_tmp461;void*_tmp462;void*_tmp463;void*_tmp464;void*_tmp465;void*_tmp466;void*
_tmp467;void*_tmp468;void*_tmp469;void*_tmp46A;void*_tmp46B;void*_tmp46C;void*
_tmp46D;void*_tmp46E;void*_tmp46F;void*_tmp470;void*_tmp471;void*_tmp472;void*
_tmp473;void*_tmp474;void*_tmp475;void*_tmp476;void*_tmp477;void*_tmp478;void*
_tmp479;void*_tmp47A;void*_tmp47B;void*_tmp47C;void*_tmp47D;void*_tmp47E;void*
_tmp47F;void*_tmp480;void*_tmp481;_LL2FA: _tmp446=_tmp445.f1;if(_tmp446 <= (void*)
4)goto _LL2FC;if(*((int*)_tmp446)!= 6)goto _LL2FC;_tmp447=((struct Cyc_Absyn_DoubleType_struct*)
_tmp446)->f1;_tmp448=_tmp445.f2;if(_tmp448 <= (void*)4)goto _LL2FC;if(*((int*)
_tmp448)!= 6)goto _LL2FC;_tmp449=((struct Cyc_Absyn_DoubleType_struct*)_tmp448)->f1;
_LL2FB: return !_tmp449  && _tmp447;_LL2FC: _tmp44A=_tmp445.f1;if(_tmp44A <= (void*)4)
goto _LL2FE;if(*((int*)_tmp44A)!= 6)goto _LL2FE;_tmp44B=_tmp445.f2;if((int)_tmp44B
!= 1)goto _LL2FE;_LL2FD: goto _LL2FF;_LL2FE: _tmp44C=_tmp445.f1;if(_tmp44C <= (void*)
4)goto _LL300;if(*((int*)_tmp44C)!= 6)goto _LL300;_tmp44D=_tmp445.f2;if(_tmp44D <= (
void*)4)goto _LL300;if(*((int*)_tmp44D)!= 5)goto _LL300;_LL2FF: goto _LL301;_LL300:
_tmp44E=_tmp445.f1;if(_tmp44E <= (void*)4)goto _LL302;if(*((int*)_tmp44E)!= 6)goto
_LL302;_tmp44F=_tmp445.f2;if(_tmp44F <= (void*)4)goto _LL302;if(*((int*)_tmp44F)!= 
18)goto _LL302;_LL301: goto _LL303;_LL302: _tmp450=_tmp445.f1;if((int)_tmp450 != 1)
goto _LL304;_tmp451=_tmp445.f2;if(_tmp451 <= (void*)4)goto _LL304;if(*((int*)
_tmp451)!= 18)goto _LL304;_LL303: goto _LL305;_LL304: _tmp452=_tmp445.f1;if((int)
_tmp452 != 1)goto _LL306;_tmp453=_tmp445.f2;if(_tmp453 <= (void*)4)goto _LL306;if(*((
int*)_tmp453)!= 5)goto _LL306;_LL305: return 1;_LL306: _tmp454=_tmp445.f1;if(_tmp454
<= (void*)4)goto _LL308;if(*((int*)_tmp454)!= 5)goto _LL308;_tmp455=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp454)->f2;if((int)_tmp455 != 4)goto _LL308;
_tmp456=_tmp445.f2;if(_tmp456 <= (void*)4)goto _LL308;if(*((int*)_tmp456)!= 5)goto
_LL308;_tmp457=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp456)->f2;if((int)
_tmp457 != 4)goto _LL308;_LL307: return 0;_LL308: _tmp458=_tmp445.f1;if(_tmp458 <= (
void*)4)goto _LL30A;if(*((int*)_tmp458)!= 5)goto _LL30A;_tmp459=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp458)->f2;if((int)_tmp459 != 4)goto _LL30A;_LL309: return 1;_LL30A: _tmp45A=
_tmp445.f1;if(_tmp45A <= (void*)4)goto _LL30C;if(*((int*)_tmp45A)!= 5)goto _LL30C;
_tmp45B=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp45A)->f2;if((int)_tmp45B != 
3)goto _LL30C;_tmp45C=_tmp445.f2;if(_tmp45C <= (void*)4)goto _LL30C;if(*((int*)
_tmp45C)!= 5)goto _LL30C;_tmp45D=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp45C)->f2;
if((int)_tmp45D != 2)goto _LL30C;_LL30B: goto _LL30D;_LL30C: _tmp45E=_tmp445.f1;if(
_tmp45E <= (void*)4)goto _LL30E;if(*((int*)_tmp45E)!= 5)goto _LL30E;_tmp45F=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp45E)->f2;if((int)_tmp45F != 2)goto _LL30E;
_tmp460=_tmp445.f2;if(_tmp460 <= (void*)4)goto _LL30E;if(*((int*)_tmp460)!= 5)goto
_LL30E;_tmp461=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp460)->f2;if((int)
_tmp461 != 3)goto _LL30E;_LL30D: return 0;_LL30E: _tmp462=_tmp445.f1;if(_tmp462 <= (
void*)4)goto _LL310;if(*((int*)_tmp462)!= 5)goto _LL310;_tmp463=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp462)->f2;if((int)_tmp463 != 3)goto _LL310;_tmp464=_tmp445.f2;if((int)_tmp464 != 
1)goto _LL310;_LL30F: goto _LL311;_LL310: _tmp465=_tmp445.f1;if(_tmp465 <= (void*)4)
goto _LL312;if(*((int*)_tmp465)!= 5)goto _LL312;_tmp466=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp465)->f2;if((int)_tmp466 != 2)goto _LL312;_tmp467=_tmp445.f2;if((int)_tmp467 != 
1)goto _LL312;_LL311: goto _LL313;_LL312: _tmp468=_tmp445.f1;if(_tmp468 <= (void*)4)
goto _LL314;if(*((int*)_tmp468)!= 5)goto _LL314;_tmp469=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp468)->f2;if((int)_tmp469 != 3)goto _LL314;_tmp46A=_tmp445.f2;if(_tmp46A <= (
void*)4)goto _LL314;if(*((int*)_tmp46A)!= 5)goto _LL314;_tmp46B=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp46A)->f2;if((int)_tmp46B != 1)goto _LL314;_LL313: goto _LL315;_LL314: _tmp46C=
_tmp445.f1;if(_tmp46C <= (void*)4)goto _LL316;if(*((int*)_tmp46C)!= 5)goto _LL316;
_tmp46D=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp46C)->f2;if((int)_tmp46D != 
2)goto _LL316;_tmp46E=_tmp445.f2;if(_tmp46E <= (void*)4)goto _LL316;if(*((int*)
_tmp46E)!= 5)goto _LL316;_tmp46F=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp46E)->f2;
if((int)_tmp46F != 1)goto _LL316;_LL315: goto _LL317;_LL316: _tmp470=_tmp445.f1;if(
_tmp470 <= (void*)4)goto _LL318;if(*((int*)_tmp470)!= 18)goto _LL318;_tmp471=
_tmp445.f2;if(_tmp471 <= (void*)4)goto _LL318;if(*((int*)_tmp471)!= 5)goto _LL318;
_tmp472=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp471)->f2;if((int)_tmp472 != 
1)goto _LL318;_LL317: goto _LL319;_LL318: _tmp473=_tmp445.f1;if(_tmp473 <= (void*)4)
goto _LL31A;if(*((int*)_tmp473)!= 5)goto _LL31A;_tmp474=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp473)->f2;if((int)_tmp474 != 3)goto _LL31A;_tmp475=_tmp445.f2;if(_tmp475 <= (
void*)4)goto _LL31A;if(*((int*)_tmp475)!= 5)goto _LL31A;_tmp476=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp475)->f2;if((int)_tmp476 != 0)goto _LL31A;_LL319: goto _LL31B;_LL31A: _tmp477=
_tmp445.f1;if(_tmp477 <= (void*)4)goto _LL31C;if(*((int*)_tmp477)!= 5)goto _LL31C;
_tmp478=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp477)->f2;if((int)_tmp478 != 
2)goto _LL31C;_tmp479=_tmp445.f2;if(_tmp479 <= (void*)4)goto _LL31C;if(*((int*)
_tmp479)!= 5)goto _LL31C;_tmp47A=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp479)->f2;
if((int)_tmp47A != 0)goto _LL31C;_LL31B: goto _LL31D;_LL31C: _tmp47B=_tmp445.f1;if(
_tmp47B <= (void*)4)goto _LL31E;if(*((int*)_tmp47B)!= 5)goto _LL31E;_tmp47C=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp47B)->f2;if((int)_tmp47C != 1)goto _LL31E;
_tmp47D=_tmp445.f2;if(_tmp47D <= (void*)4)goto _LL31E;if(*((int*)_tmp47D)!= 5)goto
_LL31E;_tmp47E=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp47D)->f2;if((int)
_tmp47E != 0)goto _LL31E;_LL31D: goto _LL31F;_LL31E: _tmp47F=_tmp445.f1;if(_tmp47F <= (
void*)4)goto _LL320;if(*((int*)_tmp47F)!= 18)goto _LL320;_tmp480=_tmp445.f2;if(
_tmp480 <= (void*)4)goto _LL320;if(*((int*)_tmp480)!= 5)goto _LL320;_tmp481=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp480)->f2;if((int)_tmp481 != 0)goto _LL320;
_LL31F: return 1;_LL320:;_LL321: return 0;_LL2F9:;}}int Cyc_Tcutil_coerce_list(struct
Cyc_Tcenv_Tenv*te,void*t,struct Cyc_List_List*es);int Cyc_Tcutil_coerce_list(
struct Cyc_Tcenv_Tenv*te,void*t,struct Cyc_List_List*es){struct _RegionHandle*
_tmp482=Cyc_Tcenv_get_fnrgn(te);{struct Cyc_Core_Opt*max_arith_type=0;{struct Cyc_List_List*
el=es;for(0;el != 0;el=el->tl){void*t1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt))->v);if(Cyc_Tcutil_is_arithmetic_type(
t1)){if(max_arith_type == 0  || Cyc_Tcutil_will_lose_precision(t1,(void*)
max_arith_type->v)){struct Cyc_Core_Opt*_tmpD56;max_arith_type=((_tmpD56=
_region_malloc(_tmp482,sizeof(*_tmpD56)),((_tmpD56->v=(void*)t1,_tmpD56))));}}}}
if(max_arith_type != 0){if(!Cyc_Tcutil_unify(t,(void*)max_arith_type->v))return 0;}}{
struct Cyc_List_List*el=es;for(0;el != 0;el=el->tl){if(!Cyc_Tcutil_coerce_assign(
te,(struct Cyc_Absyn_Exp*)el->hd,t)){{const char*_tmpD5B;void*_tmpD5A[2];struct Cyc_String_pa_struct
_tmpD59;struct Cyc_String_pa_struct _tmpD58;(_tmpD58.tag=0,((_tmpD58.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt))->v)),((_tmpD59.tag=0,((
_tmpD59.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((
_tmpD5A[0]=& _tmpD59,((_tmpD5A[1]=& _tmpD58,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)
el->hd)->loc,((_tmpD5B="type mismatch: expecting %s but found %s",_tag_dyneither(
_tmpD5B,sizeof(char),41))),_tag_dyneither(_tmpD5A,sizeof(void*),2)))))))))))));}
return 0;}}}return 1;}int Cyc_Tcutil_coerce_to_bool(struct Cyc_Tcenv_Tenv*te,struct
Cyc_Absyn_Exp*e);int Cyc_Tcutil_coerce_to_bool(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*
e){if(!Cyc_Tcutil_coerce_sint_typ(te,e)){void*_tmp488=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v);_LL323: if(_tmp488 <= (void*)4)goto
_LL325;if(*((int*)_tmp488)!= 4)goto _LL325;_LL324: Cyc_Tcutil_unchecked_cast(te,e,
Cyc_Absyn_uint_typ,(void*)3);goto _LL322;_LL325:;_LL326: return 0;_LL322:;}return 1;}
int Cyc_Tcutil_is_integral_type(void*t);int Cyc_Tcutil_is_integral_type(void*t){
void*_tmp489=Cyc_Tcutil_compress(t);_LL328: if(_tmp489 <= (void*)4)goto _LL330;if(*((
int*)_tmp489)!= 5)goto _LL32A;_LL329: goto _LL32B;_LL32A: if(*((int*)_tmp489)!= 18)
goto _LL32C;_LL32B: goto _LL32D;_LL32C: if(*((int*)_tmp489)!= 12)goto _LL32E;_LL32D:
goto _LL32F;_LL32E: if(*((int*)_tmp489)!= 13)goto _LL330;_LL32F: return 1;_LL330:;
_LL331: return 0;_LL327:;}int Cyc_Tcutil_coerce_uint_typ(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Absyn_Exp*e);int Cyc_Tcutil_coerce_uint_typ(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Absyn_Exp*e){if(Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,Cyc_Absyn_uint_typ))return 1;if(Cyc_Tcutil_is_integral_type((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)){if(Cyc_Tcutil_will_lose_precision((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_Absyn_uint_typ)){const
char*_tmpD5E;void*_tmpD5D;(_tmpD5D=0,Cyc_Tcutil_warn(e->loc,((_tmpD5E="integral size mismatch; conversion supplied",
_tag_dyneither(_tmpD5E,sizeof(char),44))),_tag_dyneither(_tmpD5D,sizeof(void*),0)));}
Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_uint_typ,(void*)1);return 1;}return 0;}
int Cyc_Tcutil_coerce_sint_typ(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e);int
Cyc_Tcutil_coerce_sint_typ(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){if(Cyc_Tcutil_unify((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_Absyn_sint_typ))return 1;
if(Cyc_Tcutil_is_integral_type((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)){
if(Cyc_Tcutil_will_lose_precision((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,
Cyc_Absyn_sint_typ)){const char*_tmpD61;void*_tmpD60;(_tmpD60=0,Cyc_Tcutil_warn(e->loc,((
_tmpD61="integral size mismatch; conversion supplied",_tag_dyneither(_tmpD61,
sizeof(char),44))),_tag_dyneither(_tmpD60,sizeof(void*),0)));}Cyc_Tcutil_unchecked_cast(
te,e,Cyc_Absyn_sint_typ,(void*)1);return 1;}return 0;}struct _tuple13{union Cyc_Absyn_Constraint_union
f1;union Cyc_Absyn_Constraint_union f2;};int Cyc_Tcutil_silent_castable(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void*t1,void*t2);int Cyc_Tcutil_silent_castable(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void*t1,void*t2){t1=Cyc_Tcutil_compress(
t1);t2=Cyc_Tcutil_compress(t2);{struct _tuple0 _tmpD62;struct _tuple0 _tmp48F=(
_tmpD62.f1=t1,((_tmpD62.f2=t2,_tmpD62)));void*_tmp490;struct Cyc_Absyn_PtrInfo
_tmp491;void*_tmp492;struct Cyc_Absyn_PtrInfo _tmp493;void*_tmp494;struct Cyc_Absyn_TunionInfo
_tmp495;union Cyc_Absyn_TunionInfoU_union _tmp496;struct Cyc_Absyn_Tuniondecl**
_tmp497;struct Cyc_Absyn_Tuniondecl*_tmp498;struct Cyc_List_List*_tmp499;struct Cyc_Core_Opt*
_tmp49A;struct Cyc_Core_Opt _tmp49B;void*_tmp49C;void*_tmp49D;struct Cyc_Absyn_TunionInfo
_tmp49E;union Cyc_Absyn_TunionInfoU_union _tmp49F;struct Cyc_Absyn_Tuniondecl**
_tmp4A0;struct Cyc_Absyn_Tuniondecl*_tmp4A1;struct Cyc_List_List*_tmp4A2;struct Cyc_Core_Opt*
_tmp4A3;struct Cyc_Core_Opt _tmp4A4;void*_tmp4A5;void*_tmp4A6;struct Cyc_Absyn_ArrayInfo
_tmp4A7;void*_tmp4A8;struct Cyc_Absyn_Tqual _tmp4A9;struct Cyc_Absyn_Exp*_tmp4AA;
struct Cyc_Absyn_Conref*_tmp4AB;void*_tmp4AC;struct Cyc_Absyn_ArrayInfo _tmp4AD;
void*_tmp4AE;struct Cyc_Absyn_Tqual _tmp4AF;struct Cyc_Absyn_Exp*_tmp4B0;struct Cyc_Absyn_Conref*
_tmp4B1;void*_tmp4B2;struct Cyc_Absyn_TunionFieldInfo _tmp4B3;union Cyc_Absyn_TunionFieldInfoU_union
_tmp4B4;struct Cyc_Absyn_Tuniondecl*_tmp4B5;struct Cyc_Absyn_Tunionfield*_tmp4B6;
struct Cyc_List_List*_tmp4B7;void*_tmp4B8;struct Cyc_Absyn_TunionInfo _tmp4B9;union
Cyc_Absyn_TunionInfoU_union _tmp4BA;struct Cyc_Absyn_Tuniondecl**_tmp4BB;struct Cyc_Absyn_Tuniondecl*
_tmp4BC;struct Cyc_List_List*_tmp4BD;void*_tmp4BE;struct Cyc_Absyn_PtrInfo _tmp4BF;
void*_tmp4C0;struct Cyc_Absyn_Tqual _tmp4C1;struct Cyc_Absyn_PtrAtts _tmp4C2;void*
_tmp4C3;struct Cyc_Absyn_Conref*_tmp4C4;struct Cyc_Absyn_Conref*_tmp4C5;struct Cyc_Absyn_Conref*
_tmp4C6;void*_tmp4C7;struct Cyc_Absyn_TunionInfo _tmp4C8;union Cyc_Absyn_TunionInfoU_union
_tmp4C9;struct Cyc_Absyn_Tuniondecl**_tmp4CA;struct Cyc_Absyn_Tuniondecl*_tmp4CB;
struct Cyc_List_List*_tmp4CC;struct Cyc_Core_Opt*_tmp4CD;void*_tmp4CE;void*_tmp4CF;
_LL333: _tmp490=_tmp48F.f1;if(_tmp490 <= (void*)4)goto _LL335;if(*((int*)_tmp490)!= 
4)goto _LL335;_tmp491=((struct Cyc_Absyn_PointerType_struct*)_tmp490)->f1;_tmp492=
_tmp48F.f2;if(_tmp492 <= (void*)4)goto _LL335;if(*((int*)_tmp492)!= 4)goto _LL335;
_tmp493=((struct Cyc_Absyn_PointerType_struct*)_tmp492)->f1;_LL334: {int okay=1;
if(!((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*
y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,(_tmp491.ptr_atts).nullable,(
_tmp493.ptr_atts).nullable)){union Cyc_Absyn_Constraint_union _tmp4D0=(((struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref*x))Cyc_Absyn_compress_conref)((_tmp491.ptr_atts).nullable))->v;
int _tmp4D1;_LL342: if((_tmp4D0.Eq_constr).tag != 0)goto _LL344;_tmp4D1=(_tmp4D0.Eq_constr).f1;
_LL343: okay=!_tmp4D1;goto _LL341;_LL344:;_LL345: {const char*_tmpD65;void*_tmpD64;(
_tmpD64=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmpD65="silent_castable conref not eq",_tag_dyneither(_tmpD65,sizeof(char),30))),
_tag_dyneither(_tmpD64,sizeof(void*),0)));}_LL341:;}if(!Cyc_Tcutil_unify_conrefs(
Cyc_Tcutil_unify_it_bounds,(_tmp491.ptr_atts).bounds,(_tmp493.ptr_atts).bounds)){
struct _tuple13 _tmpD66;struct _tuple13 _tmp4D5=(_tmpD66.f1=(Cyc_Absyn_compress_conref((
_tmp491.ptr_atts).bounds))->v,((_tmpD66.f2=(Cyc_Absyn_compress_conref((_tmp493.ptr_atts).bounds))->v,
_tmpD66)));union Cyc_Absyn_Constraint_union _tmp4D6;void*_tmp4D7;union Cyc_Absyn_Constraint_union
_tmp4D8;void*_tmp4D9;union Cyc_Absyn_Constraint_union _tmp4DA;_LL347: _tmp4D6=
_tmp4D5.f1;if(((_tmp4D5.f1).Eq_constr).tag != 0)goto _LL349;_tmp4D7=(_tmp4D6.Eq_constr).f1;
_tmp4D8=_tmp4D5.f2;if(((_tmp4D5.f2).Eq_constr).tag != 0)goto _LL349;_tmp4D9=(
_tmp4D8.Eq_constr).f1;_LL348:{struct _tuple0 _tmpD67;struct _tuple0 _tmp4DC=(_tmpD67.f1=
_tmp4D7,((_tmpD67.f2=_tmp4D9,_tmpD67)));void*_tmp4DD;void*_tmp4DE;void*_tmp4DF;
void*_tmp4E0;void*_tmp4E1;struct Cyc_Absyn_Exp*_tmp4E2;void*_tmp4E3;struct Cyc_Absyn_Exp*
_tmp4E4;void*_tmp4E5;void*_tmp4E6;struct Cyc_Absyn_Exp*_tmp4E7;_LL34E: _tmp4DD=
_tmp4DC.f1;if(_tmp4DD <= (void*)1)goto _LL350;if(*((int*)_tmp4DD)!= 0)goto _LL350;
_tmp4DE=_tmp4DC.f2;if((int)_tmp4DE != 0)goto _LL350;_LL34F: goto _LL351;_LL350:
_tmp4DF=_tmp4DC.f1;if((int)_tmp4DF != 0)goto _LL352;_tmp4E0=_tmp4DC.f2;if((int)
_tmp4E0 != 0)goto _LL352;_LL351: okay=1;goto _LL34D;_LL352: _tmp4E1=_tmp4DC.f1;if(
_tmp4E1 <= (void*)1)goto _LL354;if(*((int*)_tmp4E1)!= 0)goto _LL354;_tmp4E2=((
struct Cyc_Absyn_Upper_b_struct*)_tmp4E1)->f1;_tmp4E3=_tmp4DC.f2;if(_tmp4E3 <= (
void*)1)goto _LL354;if(*((int*)_tmp4E3)!= 0)goto _LL354;_tmp4E4=((struct Cyc_Absyn_Upper_b_struct*)
_tmp4E3)->f1;_LL353: okay=okay  && Cyc_Evexp_lte_const_exp(_tmp4E4,_tmp4E2);if(!((
int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,(_tmp493.ptr_atts).zero_term)){
const char*_tmpD6A;void*_tmpD69;(_tmpD69=0,Cyc_Tcutil_warn(loc,((_tmpD6A="implicit cast to shorter array",
_tag_dyneither(_tmpD6A,sizeof(char),31))),_tag_dyneither(_tmpD69,sizeof(void*),0)));}
goto _LL34D;_LL354: _tmp4E5=_tmp4DC.f1;if((int)_tmp4E5 != 0)goto _LL34D;_tmp4E6=
_tmp4DC.f2;if(_tmp4E6 <= (void*)1)goto _LL34D;if(*((int*)_tmp4E6)!= 0)goto _LL34D;
_tmp4E7=((struct Cyc_Absyn_Upper_b_struct*)_tmp4E6)->f1;_LL355: if(((int(*)(int,
struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,(_tmp491.ptr_atts).zero_term)
 && Cyc_Tcutil_is_bound_one((_tmp493.ptr_atts).bounds))goto _LL34D;okay=0;goto
_LL34D;_LL34D:;}goto _LL346;_LL349: _tmp4DA=_tmp4D5.f1;if(((_tmp4D5.f1).No_constr).tag
!= 2)goto _LL34B;_LL34A: okay=0;goto _LL346;_LL34B:;_LL34C: {const char*_tmpD6D;void*
_tmpD6C;(_tmpD6C=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmpD6D="silent_castable conrefs didn't unify",_tag_dyneither(_tmpD6D,sizeof(
char),37))),_tag_dyneither(_tmpD6C,sizeof(void*),0)));}_LL346:;}okay=okay  && Cyc_Tcutil_unify((
void*)_tmp491.elt_typ,(void*)_tmp493.elt_typ);okay=okay  && (Cyc_Tcutil_unify((
void*)(_tmp491.ptr_atts).rgn,(void*)(_tmp493.ptr_atts).rgn) || Cyc_Tcenv_region_outlives(
te,(void*)(_tmp491.ptr_atts).rgn,(void*)(_tmp493.ptr_atts).rgn));okay=okay  && (!(
_tmp491.elt_tq).real_const  || (_tmp493.elt_tq).real_const);okay=okay  && (((int(*)(
int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(
Cyc_Core_intcmp,(_tmp491.ptr_atts).zero_term,(_tmp493.ptr_atts).zero_term) || ((
int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(1,(_tmp491.ptr_atts).zero_term)
 && (_tmp493.elt_tq).real_const);return okay;}_LL335: _tmp494=_tmp48F.f1;if(
_tmp494 <= (void*)4)goto _LL337;if(*((int*)_tmp494)!= 2)goto _LL337;_tmp495=((
struct Cyc_Absyn_TunionType_struct*)_tmp494)->f1;_tmp496=_tmp495.tunion_info;if((((((
struct Cyc_Absyn_TunionType_struct*)_tmp494)->f1).tunion_info).KnownTunion).tag != 
1)goto _LL337;_tmp497=(_tmp496.KnownTunion).f1;_tmp498=*_tmp497;_tmp499=_tmp495.targs;
_tmp49A=_tmp495.rgn;if(_tmp49A == 0)goto _LL337;_tmp49B=*_tmp49A;_tmp49C=(void*)
_tmp49B.v;_tmp49D=_tmp48F.f2;if(_tmp49D <= (void*)4)goto _LL337;if(*((int*)_tmp49D)
!= 2)goto _LL337;_tmp49E=((struct Cyc_Absyn_TunionType_struct*)_tmp49D)->f1;
_tmp49F=_tmp49E.tunion_info;if((((((struct Cyc_Absyn_TunionType_struct*)_tmp49D)->f1).tunion_info).KnownTunion).tag
!= 1)goto _LL337;_tmp4A0=(_tmp49F.KnownTunion).f1;_tmp4A1=*_tmp4A0;_tmp4A2=
_tmp49E.targs;_tmp4A3=_tmp49E.rgn;if(_tmp4A3 == 0)goto _LL337;_tmp4A4=*_tmp4A3;
_tmp4A5=(void*)_tmp4A4.v;_LL336: if(_tmp498 != _tmp4A1  || !Cyc_Tcenv_region_outlives(
te,_tmp49C,_tmp4A5))return 0;for(0;_tmp499 != 0  && _tmp4A2 != 0;(_tmp499=_tmp499->tl,
_tmp4A2=_tmp4A2->tl)){if(!Cyc_Tcutil_unify((void*)_tmp499->hd,(void*)_tmp4A2->hd))
return 0;}if(_tmp499 != 0  || _tmp4A2 != 0)return 0;return 1;_LL337: _tmp4A6=_tmp48F.f1;
if(_tmp4A6 <= (void*)4)goto _LL339;if(*((int*)_tmp4A6)!= 7)goto _LL339;_tmp4A7=((
struct Cyc_Absyn_ArrayType_struct*)_tmp4A6)->f1;_tmp4A8=(void*)_tmp4A7.elt_type;
_tmp4A9=_tmp4A7.tq;_tmp4AA=_tmp4A7.num_elts;_tmp4AB=_tmp4A7.zero_term;_tmp4AC=
_tmp48F.f2;if(_tmp4AC <= (void*)4)goto _LL339;if(*((int*)_tmp4AC)!= 7)goto _LL339;
_tmp4AD=((struct Cyc_Absyn_ArrayType_struct*)_tmp4AC)->f1;_tmp4AE=(void*)_tmp4AD.elt_type;
_tmp4AF=_tmp4AD.tq;_tmp4B0=_tmp4AD.num_elts;_tmp4B1=_tmp4AD.zero_term;_LL338: {
int okay;okay=((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*
y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp4AB,_tmp4B1) && ((_tmp4AA != 0
 && _tmp4B0 != 0) && Cyc_Evexp_same_const_exp((struct Cyc_Absyn_Exp*)_tmp4AA,(
struct Cyc_Absyn_Exp*)_tmp4B0));return(okay  && Cyc_Tcutil_unify(_tmp4A8,_tmp4AE))
 && (!_tmp4A9.real_const  || _tmp4AF.real_const);}_LL339: _tmp4B2=_tmp48F.f1;if(
_tmp4B2 <= (void*)4)goto _LL33B;if(*((int*)_tmp4B2)!= 3)goto _LL33B;_tmp4B3=((
struct Cyc_Absyn_TunionFieldType_struct*)_tmp4B2)->f1;_tmp4B4=_tmp4B3.field_info;
if((((((struct Cyc_Absyn_TunionFieldType_struct*)_tmp4B2)->f1).field_info).KnownTunionfield).tag
!= 1)goto _LL33B;_tmp4B5=(_tmp4B4.KnownTunionfield).f1;_tmp4B6=(_tmp4B4.KnownTunionfield).f2;
_tmp4B7=_tmp4B3.targs;_tmp4B8=_tmp48F.f2;if(_tmp4B8 <= (void*)4)goto _LL33B;if(*((
int*)_tmp4B8)!= 2)goto _LL33B;_tmp4B9=((struct Cyc_Absyn_TunionType_struct*)
_tmp4B8)->f1;_tmp4BA=_tmp4B9.tunion_info;if((((((struct Cyc_Absyn_TunionType_struct*)
_tmp4B8)->f1).tunion_info).KnownTunion).tag != 1)goto _LL33B;_tmp4BB=(_tmp4BA.KnownTunion).f1;
_tmp4BC=*_tmp4BB;_tmp4BD=_tmp4B9.targs;_LL33A: if((_tmp4B5 == _tmp4BC  || Cyc_Absyn_qvar_cmp(
_tmp4B5->name,_tmp4BC->name)== 0) && (_tmp4B6->typs == 0  || _tmp4B5->is_flat)){
for(0;_tmp4B7 != 0  && _tmp4BD != 0;(_tmp4B7=_tmp4B7->tl,_tmp4BD=_tmp4BD->tl)){if(!
Cyc_Tcutil_unify((void*)_tmp4B7->hd,(void*)_tmp4BD->hd))break;}if(_tmp4B7 == 0
 && _tmp4BD == 0)return 1;}return 0;_LL33B: _tmp4BE=_tmp48F.f1;if(_tmp4BE <= (void*)4)
goto _LL33D;if(*((int*)_tmp4BE)!= 4)goto _LL33D;_tmp4BF=((struct Cyc_Absyn_PointerType_struct*)
_tmp4BE)->f1;_tmp4C0=(void*)_tmp4BF.elt_typ;_tmp4C1=_tmp4BF.elt_tq;_tmp4C2=
_tmp4BF.ptr_atts;_tmp4C3=(void*)_tmp4C2.rgn;_tmp4C4=_tmp4C2.nullable;_tmp4C5=
_tmp4C2.bounds;_tmp4C6=_tmp4C2.zero_term;_tmp4C7=_tmp48F.f2;if(_tmp4C7 <= (void*)
4)goto _LL33D;if(*((int*)_tmp4C7)!= 2)goto _LL33D;_tmp4C8=((struct Cyc_Absyn_TunionType_struct*)
_tmp4C7)->f1;_tmp4C9=_tmp4C8.tunion_info;if((((((struct Cyc_Absyn_TunionType_struct*)
_tmp4C7)->f1).tunion_info).KnownTunion).tag != 1)goto _LL33D;_tmp4CA=(_tmp4C9.KnownTunion).f1;
_tmp4CB=*_tmp4CA;_tmp4CC=_tmp4C8.targs;_tmp4CD=_tmp4C8.rgn;if(!(!_tmp4CB->is_flat))
goto _LL33D;_LL33C:{void*_tmp4EC=Cyc_Tcutil_compress(_tmp4C0);struct Cyc_Absyn_TunionFieldInfo
_tmp4ED;union Cyc_Absyn_TunionFieldInfoU_union _tmp4EE;struct Cyc_Absyn_Tuniondecl*
_tmp4EF;struct Cyc_Absyn_Tunionfield*_tmp4F0;struct Cyc_List_List*_tmp4F1;_LL357:
if(_tmp4EC <= (void*)4)goto _LL359;if(*((int*)_tmp4EC)!= 3)goto _LL359;_tmp4ED=((
struct Cyc_Absyn_TunionFieldType_struct*)_tmp4EC)->f1;_tmp4EE=_tmp4ED.field_info;
if((((((struct Cyc_Absyn_TunionFieldType_struct*)_tmp4EC)->f1).field_info).KnownTunionfield).tag
!= 1)goto _LL359;_tmp4EF=(_tmp4EE.KnownTunionfield).f1;_tmp4F0=(_tmp4EE.KnownTunionfield).f2;
_tmp4F1=_tmp4ED.targs;_LL358: if(!Cyc_Tcutil_unify(_tmp4C3,(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp4CD))->v) && !Cyc_Tcenv_region_outlives(te,_tmp4C3,(void*)
_tmp4CD->v))return 0;if(!((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,
struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp4C4,Cyc_Absyn_false_conref))
return 0;if(!Cyc_Tcutil_unify_conrefs(Cyc_Tcutil_unify_it_bounds,_tmp4C5,Cyc_Absyn_bounds_one_conref))
return 0;if(!((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*
y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp4C6,Cyc_Absyn_false_conref))
return 0;if(Cyc_Absyn_qvar_cmp(_tmp4CB->name,_tmp4EF->name)== 0  && _tmp4F0->typs
!= 0){int okay=1;for(0;_tmp4F1 != 0  && _tmp4CC != 0;(_tmp4F1=_tmp4F1->tl,_tmp4CC=
_tmp4CC->tl)){if(!Cyc_Tcutil_unify((void*)_tmp4F1->hd,(void*)_tmp4CC->hd)){okay=
0;break;}}if((!okay  || _tmp4F1 != 0) || _tmp4CC != 0)return 0;return 1;}goto _LL356;
_LL359:;_LL35A: goto _LL356;_LL356:;}return 0;_LL33D: _tmp4CE=_tmp48F.f1;if(_tmp4CE
<= (void*)4)goto _LL33F;if(*((int*)_tmp4CE)!= 18)goto _LL33F;_tmp4CF=_tmp48F.f2;
if(_tmp4CF <= (void*)4)goto _LL33F;if(*((int*)_tmp4CF)!= 5)goto _LL33F;_LL33E:
return 0;_LL33F:;_LL340: return Cyc_Tcutil_unify(t1,t2);_LL332:;}}int Cyc_Tcutil_is_pointer_type(
void*t);int Cyc_Tcutil_is_pointer_type(void*t){void*_tmp4F2=Cyc_Tcutil_compress(t);
_LL35C: if(_tmp4F2 <= (void*)4)goto _LL35E;if(*((int*)_tmp4F2)!= 4)goto _LL35E;
_LL35D: return 1;_LL35E:;_LL35F: return 0;_LL35B:;}int Cyc_Tcutil_rgn_of_pointer(void*
t,void**rgn);int Cyc_Tcutil_rgn_of_pointer(void*t,void**rgn){void*_tmp4F3=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_PtrInfo _tmp4F4;struct Cyc_Absyn_PtrAtts _tmp4F5;void*_tmp4F6;
_LL361: if(_tmp4F3 <= (void*)4)goto _LL363;if(*((int*)_tmp4F3)!= 4)goto _LL363;
_tmp4F4=((struct Cyc_Absyn_PointerType_struct*)_tmp4F3)->f1;_tmp4F5=_tmp4F4.ptr_atts;
_tmp4F6=(void*)_tmp4F5.rgn;_LL362:*rgn=_tmp4F6;return 1;_LL363:;_LL364: return 0;
_LL360:;}int Cyc_Tcutil_is_pointer_or_boxed(void*t,int*is_dyneither_ptr);int Cyc_Tcutil_is_pointer_or_boxed(
void*t,int*is_dyneither_ptr){void*_tmp4F7=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo
_tmp4F8;struct Cyc_Absyn_PtrAtts _tmp4F9;struct Cyc_Absyn_Conref*_tmp4FA;_LL366: if(
_tmp4F7 <= (void*)4)goto _LL368;if(*((int*)_tmp4F7)!= 4)goto _LL368;_tmp4F8=((
struct Cyc_Absyn_PointerType_struct*)_tmp4F7)->f1;_tmp4F9=_tmp4F8.ptr_atts;
_tmp4FA=_tmp4F9.bounds;_LL367:*is_dyneither_ptr=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,
_tmp4FA)== (void*)0;return 1;_LL368:;_LL369: return Cyc_Tcutil_typ_kind(t)== (void*)
2;_LL365:;}int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*e);int Cyc_Tcutil_is_zero(
struct Cyc_Absyn_Exp*e){void*_tmp4FB=(void*)e->r;union Cyc_Absyn_Cnst_union _tmp4FC;
int _tmp4FD;union Cyc_Absyn_Cnst_union _tmp4FE;char _tmp4FF;union Cyc_Absyn_Cnst_union
_tmp500;short _tmp501;union Cyc_Absyn_Cnst_union _tmp502;long long _tmp503;void*
_tmp504;struct Cyc_Absyn_Exp*_tmp505;_LL36B: if(*((int*)_tmp4FB)!= 0)goto _LL36D;
_tmp4FC=((struct Cyc_Absyn_Const_e_struct*)_tmp4FB)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)
_tmp4FB)->f1).Int_c).tag != 2)goto _LL36D;_tmp4FD=(_tmp4FC.Int_c).f2;if(_tmp4FD != 
0)goto _LL36D;_LL36C: goto _LL36E;_LL36D: if(*((int*)_tmp4FB)!= 0)goto _LL36F;_tmp4FE=((
struct Cyc_Absyn_Const_e_struct*)_tmp4FB)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)
_tmp4FB)->f1).Char_c).tag != 0)goto _LL36F;_tmp4FF=(_tmp4FE.Char_c).f2;if(_tmp4FF
!= 0)goto _LL36F;_LL36E: goto _LL370;_LL36F: if(*((int*)_tmp4FB)!= 0)goto _LL371;
_tmp500=((struct Cyc_Absyn_Const_e_struct*)_tmp4FB)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)
_tmp4FB)->f1).Short_c).tag != 1)goto _LL371;_tmp501=(_tmp500.Short_c).f2;if(
_tmp501 != 0)goto _LL371;_LL370: goto _LL372;_LL371: if(*((int*)_tmp4FB)!= 0)goto
_LL373;_tmp502=((struct Cyc_Absyn_Const_e_struct*)_tmp4FB)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)
_tmp4FB)->f1).LongLong_c).tag != 3)goto _LL373;_tmp503=(_tmp502.LongLong_c).f2;if(
_tmp503 != 0)goto _LL373;_LL372: return 1;_LL373: if(*((int*)_tmp4FB)!= 15)goto _LL375;
_tmp504=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp4FB)->f1;_tmp505=((struct Cyc_Absyn_Cast_e_struct*)
_tmp4FB)->f2;_LL374: return Cyc_Tcutil_is_zero(_tmp505) && Cyc_Tcutil_admits_zero(
_tmp504);_LL375:;_LL376: return 0;_LL36A:;}struct Cyc_Core_Opt Cyc_Tcutil_trk={(void*)((
void*)5)};struct Cyc_Core_Opt Cyc_Tcutil_urk={(void*)((void*)4)};struct Cyc_Core_Opt
Cyc_Tcutil_rk={(void*)((void*)3)};struct Cyc_Core_Opt Cyc_Tcutil_ak={(void*)((void*)
0)};struct Cyc_Core_Opt Cyc_Tcutil_bk={(void*)((void*)2)};struct Cyc_Core_Opt Cyc_Tcutil_mk={(
void*)((void*)1)};struct Cyc_Core_Opt Cyc_Tcutil_ik={(void*)((void*)7)};struct Cyc_Core_Opt
Cyc_Tcutil_ek={(void*)((void*)6)};struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_opt(void*
k);struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_opt(void*k){void*_tmp506=k;_LL378: if((
int)_tmp506 != 0)goto _LL37A;_LL379: return(struct Cyc_Core_Opt*)& Cyc_Tcutil_ak;
_LL37A: if((int)_tmp506 != 1)goto _LL37C;_LL37B: return(struct Cyc_Core_Opt*)& Cyc_Tcutil_mk;
_LL37C: if((int)_tmp506 != 2)goto _LL37E;_LL37D: return(struct Cyc_Core_Opt*)& Cyc_Tcutil_bk;
_LL37E: if((int)_tmp506 != 3)goto _LL380;_LL37F: return(struct Cyc_Core_Opt*)& Cyc_Tcutil_rk;
_LL380: if((int)_tmp506 != 4)goto _LL382;_LL381: return(struct Cyc_Core_Opt*)& Cyc_Tcutil_urk;
_LL382: if((int)_tmp506 != 5)goto _LL384;_LL383: return(struct Cyc_Core_Opt*)& Cyc_Tcutil_trk;
_LL384: if((int)_tmp506 != 6)goto _LL386;_LL385: return(struct Cyc_Core_Opt*)& Cyc_Tcutil_ek;
_LL386: if((int)_tmp506 != 7)goto _LL377;_LL387: return(struct Cyc_Core_Opt*)& Cyc_Tcutil_ik;
_LL377:;}static void**Cyc_Tcutil_kind_to_b(void*k);static void**Cyc_Tcutil_kind_to_b(
void*k){static struct Cyc_Absyn_Eq_kb_struct ab_v={0,(void*)((void*)0)};static
struct Cyc_Absyn_Eq_kb_struct mb_v={0,(void*)((void*)1)};static struct Cyc_Absyn_Eq_kb_struct
bb_v={0,(void*)((void*)2)};static struct Cyc_Absyn_Eq_kb_struct rb_v={0,(void*)((
void*)3)};static struct Cyc_Absyn_Eq_kb_struct ub_v={0,(void*)((void*)4)};static
struct Cyc_Absyn_Eq_kb_struct tb_v={0,(void*)((void*)5)};static struct Cyc_Absyn_Eq_kb_struct
eb_v={0,(void*)((void*)6)};static struct Cyc_Absyn_Eq_kb_struct ib_v={0,(void*)((
void*)7)};static void*ab=(void*)& ab_v;static void*mb=(void*)& mb_v;static void*bb=(
void*)& bb_v;static void*rb=(void*)& rb_v;static void*ub=(void*)& ub_v;static void*tb=(
void*)& tb_v;static void*eb=(void*)& eb_v;static void*ib=(void*)& ib_v;void*_tmp507=k;
_LL389: if((int)_tmp507 != 0)goto _LL38B;_LL38A: return& ab;_LL38B: if((int)_tmp507 != 
1)goto _LL38D;_LL38C: return& mb;_LL38D: if((int)_tmp507 != 2)goto _LL38F;_LL38E:
return& bb;_LL38F: if((int)_tmp507 != 3)goto _LL391;_LL390: return& rb;_LL391: if((int)
_tmp507 != 4)goto _LL393;_LL392: return& ub;_LL393: if((int)_tmp507 != 5)goto _LL395;
_LL394: return& tb;_LL395: if((int)_tmp507 != 6)goto _LL397;_LL396: return& eb;_LL397:
if((int)_tmp507 != 7)goto _LL388;_LL398: return& ib;_LL388:;}void*Cyc_Tcutil_kind_to_bound(
void*k);void*Cyc_Tcutil_kind_to_bound(void*k){return*Cyc_Tcutil_kind_to_b(k);}
struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_bound_opt(void*k);struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_bound_opt(
void*k){return(struct Cyc_Core_Opt*)Cyc_Tcutil_kind_to_b(k);}int Cyc_Tcutil_zero_to_null(
struct Cyc_Tcenv_Tenv*te,void*t2,struct Cyc_Absyn_Exp*e1);int Cyc_Tcutil_zero_to_null(
struct Cyc_Tcenv_Tenv*te,void*t2,struct Cyc_Absyn_Exp*e1){if(Cyc_Tcutil_is_pointer_type(
t2) && Cyc_Tcutil_is_zero(e1)){{struct Cyc_Absyn_Const_e_struct _tmpD73;union Cyc_Absyn_Cnst_union
_tmpD72;struct Cyc_Absyn_Const_e_struct*_tmpD71;(void*)(e1->r=(void*)((void*)((
_tmpD71=_cycalloc(sizeof(*_tmpD71)),((_tmpD71[0]=((_tmpD73.tag=0,((_tmpD73.f1=(
union Cyc_Absyn_Cnst_union)(((_tmpD72.Null_c).tag=6,_tmpD72)),_tmpD73)))),_tmpD71))))));}{
struct Cyc_Core_Opt*_tmp513=Cyc_Tcenv_lookup_opt_type_vars(te);struct Cyc_Absyn_PointerType_struct
_tmpD7D;struct Cyc_Absyn_PtrAtts _tmpD7C;struct Cyc_Absyn_PtrInfo _tmpD7B;struct Cyc_Absyn_PointerType_struct*
_tmpD7A;struct Cyc_Absyn_PointerType_struct*_tmp514=(_tmpD7A=_cycalloc(sizeof(*
_tmpD7A)),((_tmpD7A[0]=((_tmpD7D.tag=4,((_tmpD7D.f1=((_tmpD7B.elt_typ=(void*)Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_ak,_tmp513),((_tmpD7B.elt_tq=Cyc_Absyn_empty_tqual(
0),((_tmpD7B.ptr_atts=((_tmpD7C.rgn=(void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_trk,_tmp513),((_tmpD7C.nullable=Cyc_Absyn_true_conref,((_tmpD7C.bounds=
Cyc_Absyn_empty_conref(),((_tmpD7C.zero_term=((struct Cyc_Absyn_Conref*(*)())Cyc_Absyn_empty_conref)(),((
_tmpD7C.ptrloc=0,_tmpD7C)))))))))),_tmpD7B)))))),_tmpD7D)))),_tmpD7A)));(void*)(((
struct Cyc_Core_Opt*)_check_null(e1->topt))->v=(void*)((void*)_tmp514));return Cyc_Tcutil_coerce_arg(
te,e1,t2);}}return 0;}struct _dyneither_ptr Cyc_Tcutil_coercion2string(void*c);
struct _dyneither_ptr Cyc_Tcutil_coercion2string(void*c){void*_tmp519=c;_LL39A: if((
int)_tmp519 != 0)goto _LL39C;_LL39B: {const char*_tmpD7E;return(_tmpD7E="unknown",
_tag_dyneither(_tmpD7E,sizeof(char),8));}_LL39C: if((int)_tmp519 != 1)goto _LL39E;
_LL39D: {const char*_tmpD7F;return(_tmpD7F="no coercion",_tag_dyneither(_tmpD7F,
sizeof(char),12));}_LL39E: if((int)_tmp519 != 2)goto _LL3A0;_LL39F: {const char*
_tmpD80;return(_tmpD80="null check",_tag_dyneither(_tmpD80,sizeof(char),11));}
_LL3A0: if((int)_tmp519 != 3)goto _LL399;_LL3A1: {const char*_tmpD81;return(_tmpD81="other coercion",
_tag_dyneither(_tmpD81,sizeof(char),15));}_LL399:;}int Cyc_Tcutil_coerce_arg(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t2);int Cyc_Tcutil_coerce_arg(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t2){void*t1=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);void*c;if(Cyc_Tcutil_unify(
t1,t2))return 1;if(Cyc_Tcutil_is_arithmetic_type(t2) && Cyc_Tcutil_is_arithmetic_type(
t1)){if(Cyc_Tcutil_will_lose_precision(t1,t2)){const char*_tmpD86;void*_tmpD85[2];
struct Cyc_String_pa_struct _tmpD84;struct Cyc_String_pa_struct _tmpD83;(_tmpD83.tag=
0,((_tmpD83.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t2)),((_tmpD84.tag=0,((_tmpD84.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t1)),((_tmpD85[0]=& _tmpD84,((_tmpD85[1]=& _tmpD83,Cyc_Tcutil_warn(
e->loc,((_tmpD86="integral size mismatch; %s -> %s conversion supplied",
_tag_dyneither(_tmpD86,sizeof(char),53))),_tag_dyneither(_tmpD85,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_unchecked_cast(te,e,t2,(void*)1);return 1;}else{if(Cyc_Tcutil_silent_castable(
te,e->loc,t1,t2)){Cyc_Tcutil_unchecked_cast(te,e,t2,(void*)3);return 1;}else{if(
Cyc_Tcutil_zero_to_null(te,t2,e))return 1;else{if((c=Cyc_Tcutil_castable(te,e->loc,
t1,t2))!= (void*)0){Cyc_Tcutil_unchecked_cast(te,e,t2,c);if(c != (void*)2){const
char*_tmpD8B;void*_tmpD8A[2];struct Cyc_String_pa_struct _tmpD89;struct Cyc_String_pa_struct
_tmpD88;(_tmpD88.tag=0,((_tmpD88.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2)),((_tmpD89.tag=0,((_tmpD89.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmpD8A[0]=& _tmpD89,((_tmpD8A[
1]=& _tmpD88,Cyc_Tcutil_warn(e->loc,((_tmpD8B="implicit cast from %s to %s",
_tag_dyneither(_tmpD8B,sizeof(char),28))),_tag_dyneither(_tmpD8A,sizeof(void*),2)))))))))))));}
return 1;}else{return 0;}}}}}int Cyc_Tcutil_coerce_assign(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Absyn_Exp*e,void*t);int Cyc_Tcutil_coerce_assign(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e,void*t){return Cyc_Tcutil_coerce_arg(te,e,t);}int Cyc_Tcutil_coerceable(
void*t);int Cyc_Tcutil_coerceable(void*t){void*_tmp526=Cyc_Tcutil_compress(t);
_LL3A3: if(_tmp526 <= (void*)4)goto _LL3A5;if(*((int*)_tmp526)!= 5)goto _LL3A5;
_LL3A4: goto _LL3A6;_LL3A5: if((int)_tmp526 != 1)goto _LL3A7;_LL3A6: goto _LL3A8;_LL3A7:
if(_tmp526 <= (void*)4)goto _LL3A9;if(*((int*)_tmp526)!= 6)goto _LL3A9;_LL3A8:
return 1;_LL3A9:;_LL3AA: return 0;_LL3A2:;}static struct _tuple5*Cyc_Tcutil_flatten_typ_f(
struct _tuple9*env,struct Cyc_Absyn_Aggrfield*x);static struct _tuple5*Cyc_Tcutil_flatten_typ_f(
struct _tuple9*env,struct Cyc_Absyn_Aggrfield*x){struct Cyc_List_List*_tmp528;
struct _RegionHandle*_tmp529;struct _tuple9 _tmp527=*env;_tmp528=_tmp527.f1;_tmp529=
_tmp527.f2;{struct _tuple5*_tmpD8C;return(_tmpD8C=_region_malloc(_tmp529,sizeof(*
_tmpD8C)),((_tmpD8C->f1=x->tq,((_tmpD8C->f2=Cyc_Tcutil_rsubstitute(_tmp529,
_tmp528,(void*)x->type),_tmpD8C)))));}}static struct _tuple5*Cyc_Tcutil_rcopy_tqt(
struct _RegionHandle*r,struct _tuple5*x);static struct _tuple5*Cyc_Tcutil_rcopy_tqt(
struct _RegionHandle*r,struct _tuple5*x){struct Cyc_Absyn_Tqual _tmp52C;void*_tmp52D;
struct _tuple5 _tmp52B=*x;_tmp52C=_tmp52B.f1;_tmp52D=_tmp52B.f2;{struct _tuple5*
_tmpD8D;return(_tmpD8D=_region_malloc(r,sizeof(*_tmpD8D)),((_tmpD8D->f1=_tmp52C,((
_tmpD8D->f2=_tmp52D,_tmpD8D)))));}}static struct Cyc_List_List*Cyc_Tcutil_flatten_typ(
struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,void*t1);static struct Cyc_List_List*
Cyc_Tcutil_flatten_typ(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,void*t1){t1=
Cyc_Tcutil_compress(t1);{void*_tmp52F=t1;struct Cyc_List_List*_tmp530;struct Cyc_Absyn_AggrInfo
_tmp531;union Cyc_Absyn_AggrInfoU_union _tmp532;struct Cyc_Absyn_Aggrdecl**_tmp533;
struct Cyc_Absyn_Aggrdecl*_tmp534;struct Cyc_List_List*_tmp535;void*_tmp536;struct
Cyc_List_List*_tmp537;struct Cyc_Absyn_FnInfo _tmp538;_LL3AC: if((int)_tmp52F != 0)
goto _LL3AE;_LL3AD: return 0;_LL3AE: if(_tmp52F <= (void*)4)goto _LL3B6;if(*((int*)
_tmp52F)!= 9)goto _LL3B0;_tmp530=((struct Cyc_Absyn_TupleType_struct*)_tmp52F)->f1;
_LL3AF: return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple5*(*f)(
struct _RegionHandle*,struct _tuple5*),struct _RegionHandle*env,struct Cyc_List_List*
x))Cyc_List_rmap_c)(r,Cyc_Tcutil_rcopy_tqt,r,_tmp530);_LL3B0: if(*((int*)_tmp52F)
!= 10)goto _LL3B2;_tmp531=((struct Cyc_Absyn_AggrType_struct*)_tmp52F)->f1;_tmp532=
_tmp531.aggr_info;if((((((struct Cyc_Absyn_AggrType_struct*)_tmp52F)->f1).aggr_info).KnownAggr).tag
!= 1)goto _LL3B2;_tmp533=(_tmp532.KnownAggr).f1;_tmp534=*_tmp533;_tmp535=_tmp531.targs;
_LL3B1: if((((void*)_tmp534->kind == (void*)1  || _tmp534->impl == 0) || ((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp534->impl))->exist_vars != 0) || ((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp534->impl))->rgn_po != 0){struct _tuple5*_tmpD90;struct Cyc_List_List*
_tmpD8F;return(_tmpD8F=_region_malloc(r,sizeof(*_tmpD8F)),((_tmpD8F->hd=((
_tmpD90=_region_malloc(r,sizeof(*_tmpD90)),((_tmpD90->f1=Cyc_Absyn_empty_tqual(0),((
_tmpD90->f2=t1,_tmpD90)))))),((_tmpD8F->tl=0,_tmpD8F)))));}{struct Cyc_List_List*
_tmp53B=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp534->tvs,
_tmp535);struct _tuple9 _tmpD91;struct _tuple9 env=(_tmpD91.f1=_tmp53B,((_tmpD91.f2=
r,_tmpD91)));return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple5*(*
f)(struct _tuple9*,struct Cyc_Absyn_Aggrfield*),struct _tuple9*env,struct Cyc_List_List*
x))Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_typ_f,& env,((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp534->impl))->fields);}_LL3B2: if(*((int*)_tmp52F)!= 11)goto _LL3B4;
_tmp536=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)_tmp52F)->f1;if((int)
_tmp536 != 0)goto _LL3B4;_tmp537=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp52F)->f2;
_LL3B3: {struct _tuple9 _tmpD92;struct _tuple9 env=(_tmpD92.f1=0,((_tmpD92.f2=r,
_tmpD92)));return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple5*(*
f)(struct _tuple9*,struct Cyc_Absyn_Aggrfield*),struct _tuple9*env,struct Cyc_List_List*
x))Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_typ_f,& env,_tmp537);}_LL3B4: if(*((int*)
_tmp52F)!= 8)goto _LL3B6;_tmp538=((struct Cyc_Absyn_FnType_struct*)_tmp52F)->f1;
_LL3B5: {struct _tuple5*_tmpD95;struct Cyc_List_List*_tmpD94;return(_tmpD94=
_region_malloc(r,sizeof(*_tmpD94)),((_tmpD94->hd=((_tmpD95=_region_malloc(r,
sizeof(*_tmpD95)),((_tmpD95->f1=Cyc_Absyn_const_tqual(0),((_tmpD95->f2=t1,
_tmpD95)))))),((_tmpD94->tl=0,_tmpD94)))));}_LL3B6:;_LL3B7: {struct _tuple5*
_tmpD98;struct Cyc_List_List*_tmpD97;return(_tmpD97=_region_malloc(r,sizeof(*
_tmpD97)),((_tmpD97->hd=((_tmpD98=_region_malloc(r,sizeof(*_tmpD98)),((_tmpD98->f1=
Cyc_Absyn_empty_tqual(0),((_tmpD98->f2=t1,_tmpD98)))))),((_tmpD97->tl=0,_tmpD97)))));}
_LL3AB:;}}static int Cyc_Tcutil_sub_attributes(struct Cyc_List_List*a1,struct Cyc_List_List*
a2);static int Cyc_Tcutil_sub_attributes(struct Cyc_List_List*a1,struct Cyc_List_List*
a2){{struct Cyc_List_List*t=a1;for(0;t != 0;t=t->tl){void*_tmp542=(void*)t->hd;
_LL3B9: if((int)_tmp542 != 16)goto _LL3BB;_LL3BA: goto _LL3BC;_LL3BB: if((int)_tmp542
!= 3)goto _LL3BD;_LL3BC: goto _LL3BE;_LL3BD: if(_tmp542 <= (void*)17)goto _LL3BF;if(*((
int*)_tmp542)!= 4)goto _LL3BF;_LL3BE: continue;_LL3BF:;_LL3C0: if(!Cyc_List_exists_c(
Cyc_Tcutil_equal_att,(void*)t->hd,a2))return 0;_LL3B8:;}}for(0;a2 != 0;a2=a2->tl){
if(!Cyc_List_exists_c(Cyc_Tcutil_equal_att,(void*)a2->hd,a1))return 0;}return 1;}
static int Cyc_Tcutil_ptrsubtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*
assume,void*t1,void*t2);static int Cyc_Tcutil_subtype(struct Cyc_Tcenv_Tenv*te,
struct Cyc_List_List*assume,void*t1,void*t2);static int Cyc_Tcutil_subtype(struct
Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2){if(Cyc_Tcutil_unify(
t1,t2))return 1;{struct Cyc_List_List*a=assume;for(0;a != 0;a=a->tl){if(Cyc_Tcutil_unify(
t1,(*((struct _tuple0*)a->hd)).f1) && Cyc_Tcutil_unify(t2,(*((struct _tuple0*)a->hd)).f2))
return 1;}}t1=Cyc_Tcutil_compress(t1);t2=Cyc_Tcutil_compress(t2);{struct _tuple0
_tmpD99;struct _tuple0 _tmp544=(_tmpD99.f1=t1,((_tmpD99.f2=t2,_tmpD99)));void*
_tmp545;struct Cyc_Absyn_PtrInfo _tmp546;void*_tmp547;struct Cyc_Absyn_Tqual _tmp548;
struct Cyc_Absyn_PtrAtts _tmp549;void*_tmp54A;struct Cyc_Absyn_Conref*_tmp54B;
struct Cyc_Absyn_Conref*_tmp54C;struct Cyc_Absyn_Conref*_tmp54D;void*_tmp54E;
struct Cyc_Absyn_PtrInfo _tmp54F;void*_tmp550;struct Cyc_Absyn_Tqual _tmp551;struct
Cyc_Absyn_PtrAtts _tmp552;void*_tmp553;struct Cyc_Absyn_Conref*_tmp554;struct Cyc_Absyn_Conref*
_tmp555;struct Cyc_Absyn_Conref*_tmp556;void*_tmp557;struct Cyc_Absyn_TunionInfo
_tmp558;union Cyc_Absyn_TunionInfoU_union _tmp559;struct Cyc_Absyn_Tuniondecl**
_tmp55A;struct Cyc_Absyn_Tuniondecl*_tmp55B;struct Cyc_List_List*_tmp55C;struct Cyc_Core_Opt*
_tmp55D;struct Cyc_Core_Opt _tmp55E;void*_tmp55F;void*_tmp560;struct Cyc_Absyn_TunionInfo
_tmp561;union Cyc_Absyn_TunionInfoU_union _tmp562;struct Cyc_Absyn_Tuniondecl**
_tmp563;struct Cyc_Absyn_Tuniondecl*_tmp564;struct Cyc_List_List*_tmp565;struct Cyc_Core_Opt*
_tmp566;struct Cyc_Core_Opt _tmp567;void*_tmp568;void*_tmp569;struct Cyc_Absyn_FnInfo
_tmp56A;void*_tmp56B;struct Cyc_Absyn_FnInfo _tmp56C;_LL3C2: _tmp545=_tmp544.f1;if(
_tmp545 <= (void*)4)goto _LL3C4;if(*((int*)_tmp545)!= 4)goto _LL3C4;_tmp546=((
struct Cyc_Absyn_PointerType_struct*)_tmp545)->f1;_tmp547=(void*)_tmp546.elt_typ;
_tmp548=_tmp546.elt_tq;_tmp549=_tmp546.ptr_atts;_tmp54A=(void*)_tmp549.rgn;
_tmp54B=_tmp549.nullable;_tmp54C=_tmp549.bounds;_tmp54D=_tmp549.zero_term;
_tmp54E=_tmp544.f2;if(_tmp54E <= (void*)4)goto _LL3C4;if(*((int*)_tmp54E)!= 4)goto
_LL3C4;_tmp54F=((struct Cyc_Absyn_PointerType_struct*)_tmp54E)->f1;_tmp550=(void*)
_tmp54F.elt_typ;_tmp551=_tmp54F.elt_tq;_tmp552=_tmp54F.ptr_atts;_tmp553=(void*)
_tmp552.rgn;_tmp554=_tmp552.nullable;_tmp555=_tmp552.bounds;_tmp556=_tmp552.zero_term;
_LL3C3: if(_tmp548.real_const  && !_tmp551.real_const)return 0;if((!((int(*)(int(*
cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(
Cyc_Core_intcmp,_tmp54B,_tmp554) && ((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,_tmp54B)) && !((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,
_tmp554))return 0;if((!((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,
struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp54D,
_tmp556) && !((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,
_tmp54D)) && ((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,
_tmp556))return 0;if(!Cyc_Tcutil_unify(_tmp54A,_tmp553) && !Cyc_Tcenv_region_outlives(
te,_tmp54A,_tmp553))return 0;if(!Cyc_Tcutil_unify_conrefs(Cyc_Tcutil_unify_it_bounds,
_tmp54C,_tmp555)){struct _tuple0 _tmpD9A;struct _tuple0 _tmp56E=(_tmpD9A.f1=Cyc_Absyn_conref_val(
_tmp54C),((_tmpD9A.f2=Cyc_Absyn_conref_val(_tmp555),_tmpD9A)));void*_tmp56F;void*
_tmp570;void*_tmp571;struct Cyc_Absyn_Exp*_tmp572;void*_tmp573;struct Cyc_Absyn_Exp*
_tmp574;_LL3CB: _tmp56F=_tmp56E.f1;if(_tmp56F <= (void*)1)goto _LL3CD;if(*((int*)
_tmp56F)!= 0)goto _LL3CD;_tmp570=_tmp56E.f2;if((int)_tmp570 != 0)goto _LL3CD;_LL3CC:
goto _LL3CA;_LL3CD: _tmp571=_tmp56E.f1;if(_tmp571 <= (void*)1)goto _LL3CF;if(*((int*)
_tmp571)!= 0)goto _LL3CF;_tmp572=((struct Cyc_Absyn_Upper_b_struct*)_tmp571)->f1;
_tmp573=_tmp56E.f2;if(_tmp573 <= (void*)1)goto _LL3CF;if(*((int*)_tmp573)!= 0)goto
_LL3CF;_tmp574=((struct Cyc_Absyn_Upper_b_struct*)_tmp573)->f1;_LL3CE: if(!Cyc_Evexp_lte_const_exp(
_tmp574,_tmp572))return 0;goto _LL3CA;_LL3CF:;_LL3D0: return 0;_LL3CA:;}{struct
_tuple0*_tmpD9D;struct Cyc_List_List*_tmpD9C;return Cyc_Tcutil_ptrsubtype(te,((
_tmpD9C=_cycalloc(sizeof(*_tmpD9C)),((_tmpD9C->hd=((_tmpD9D=_cycalloc(sizeof(*
_tmpD9D)),((_tmpD9D->f1=t1,((_tmpD9D->f2=t2,_tmpD9D)))))),((_tmpD9C->tl=assume,
_tmpD9C)))))),_tmp547,_tmp550);}_LL3C4: _tmp557=_tmp544.f1;if(_tmp557 <= (void*)4)
goto _LL3C6;if(*((int*)_tmp557)!= 2)goto _LL3C6;_tmp558=((struct Cyc_Absyn_TunionType_struct*)
_tmp557)->f1;_tmp559=_tmp558.tunion_info;if((((((struct Cyc_Absyn_TunionType_struct*)
_tmp557)->f1).tunion_info).KnownTunion).tag != 1)goto _LL3C6;_tmp55A=(_tmp559.KnownTunion).f1;
_tmp55B=*_tmp55A;_tmp55C=_tmp558.targs;_tmp55D=_tmp558.rgn;if(_tmp55D == 0)goto
_LL3C6;_tmp55E=*_tmp55D;_tmp55F=(void*)_tmp55E.v;_tmp560=_tmp544.f2;if(_tmp560 <= (
void*)4)goto _LL3C6;if(*((int*)_tmp560)!= 2)goto _LL3C6;_tmp561=((struct Cyc_Absyn_TunionType_struct*)
_tmp560)->f1;_tmp562=_tmp561.tunion_info;if((((((struct Cyc_Absyn_TunionType_struct*)
_tmp560)->f1).tunion_info).KnownTunion).tag != 1)goto _LL3C6;_tmp563=(_tmp562.KnownTunion).f1;
_tmp564=*_tmp563;_tmp565=_tmp561.targs;_tmp566=_tmp561.rgn;if(_tmp566 == 0)goto
_LL3C6;_tmp567=*_tmp566;_tmp568=(void*)_tmp567.v;_LL3C5: if(_tmp55B != _tmp564  || 
!Cyc_Tcenv_region_outlives(te,_tmp55F,_tmp568))return 0;for(0;_tmp55C != 0  && 
_tmp565 != 0;(_tmp55C=_tmp55C->tl,_tmp565=_tmp565->tl)){if(!Cyc_Tcutil_unify((
void*)_tmp55C->hd,(void*)_tmp565->hd))return 0;}if(_tmp55C != 0  || _tmp565 != 0)
return 0;return 1;_LL3C6: _tmp569=_tmp544.f1;if(_tmp569 <= (void*)4)goto _LL3C8;if(*((
int*)_tmp569)!= 8)goto _LL3C8;_tmp56A=((struct Cyc_Absyn_FnType_struct*)_tmp569)->f1;
_tmp56B=_tmp544.f2;if(_tmp56B <= (void*)4)goto _LL3C8;if(*((int*)_tmp56B)!= 8)goto
_LL3C8;_tmp56C=((struct Cyc_Absyn_FnType_struct*)_tmp56B)->f1;_LL3C7: if(_tmp56A.tvars
!= 0  || _tmp56C.tvars != 0){struct Cyc_List_List*_tmp577=_tmp56A.tvars;struct Cyc_List_List*
_tmp578=_tmp56C.tvars;if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp577)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp578))return 0;{
struct _RegionHandle*_tmp579=Cyc_Tcenv_get_fnrgn(te);struct Cyc_List_List*inst=0;
while(_tmp577 != 0){if(Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp577->hd)!= 
Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)_check_null(
_tmp578))->hd))return 0;{struct _tuple8*_tmpDA7;struct Cyc_Absyn_VarType_struct
_tmpDA6;struct Cyc_Absyn_VarType_struct*_tmpDA5;struct Cyc_List_List*_tmpDA4;inst=((
_tmpDA4=_region_malloc(_tmp579,sizeof(*_tmpDA4)),((_tmpDA4->hd=((_tmpDA7=
_region_malloc(_tmp579,sizeof(*_tmpDA7)),((_tmpDA7->f1=(struct Cyc_Absyn_Tvar*)
_tmp578->hd,((_tmpDA7->f2=(void*)((_tmpDA5=_cycalloc(sizeof(*_tmpDA5)),((_tmpDA5[
0]=((_tmpDA6.tag=1,((_tmpDA6.f1=(struct Cyc_Absyn_Tvar*)_tmp577->hd,_tmpDA6)))),
_tmpDA5)))),_tmpDA7)))))),((_tmpDA4->tl=inst,_tmpDA4))))));}_tmp577=_tmp577->tl;
_tmp578=_tmp578->tl;}if(inst != 0){_tmp56A.tvars=0;_tmp56C.tvars=0;{struct Cyc_Absyn_FnType_struct
_tmpDAD;struct Cyc_Absyn_FnType_struct*_tmpDAC;struct Cyc_Absyn_FnType_struct
_tmpDAA;struct Cyc_Absyn_FnType_struct*_tmpDA9;return Cyc_Tcutil_subtype(te,assume,(
void*)((_tmpDA9=_cycalloc(sizeof(*_tmpDA9)),((_tmpDA9[0]=((_tmpDAA.tag=8,((
_tmpDAA.f1=_tmp56A,_tmpDAA)))),_tmpDA9)))),(void*)((_tmpDAC=_cycalloc(sizeof(*
_tmpDAC)),((_tmpDAC[0]=((_tmpDAD.tag=8,((_tmpDAD.f1=_tmp56C,_tmpDAD)))),_tmpDAC)))));}}}}
if(!Cyc_Tcutil_subtype(te,assume,(void*)_tmp56A.ret_typ,(void*)_tmp56C.ret_typ))
return 0;{struct Cyc_List_List*_tmp582=_tmp56A.args;struct Cyc_List_List*_tmp583=
_tmp56C.args;if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp582)!= ((
int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp583))return 0;for(0;_tmp582 != 
0;(_tmp582=_tmp582->tl,_tmp583=_tmp583->tl)){struct Cyc_Absyn_Tqual _tmp585;void*
_tmp586;struct _tuple3 _tmp584=*((struct _tuple3*)_tmp582->hd);_tmp585=_tmp584.f2;
_tmp586=_tmp584.f3;{struct Cyc_Absyn_Tqual _tmp588;void*_tmp589;struct _tuple3
_tmp587=*((struct _tuple3*)((struct Cyc_List_List*)_check_null(_tmp583))->hd);
_tmp588=_tmp587.f2;_tmp589=_tmp587.f3;if(_tmp588.real_const  && !_tmp585.real_const
 || !Cyc_Tcutil_subtype(te,assume,_tmp589,_tmp586))return 0;}}if(_tmp56A.c_varargs
!= _tmp56C.c_varargs)return 0;if(_tmp56A.cyc_varargs != 0  && _tmp56C.cyc_varargs != 
0){struct Cyc_Absyn_VarargInfo _tmp58A=*_tmp56A.cyc_varargs;struct Cyc_Absyn_VarargInfo
_tmp58B=*_tmp56C.cyc_varargs;if((_tmp58B.tq).real_const  && !(_tmp58A.tq).real_const
 || !Cyc_Tcutil_subtype(te,assume,(void*)_tmp58B.type,(void*)_tmp58A.type))
return 0;}else{if(_tmp56A.cyc_varargs != 0  || _tmp56C.cyc_varargs != 0)return 0;}if(
!Cyc_Tcutil_subset_effect(1,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp56A.effect))->v,(
void*)((struct Cyc_Core_Opt*)_check_null(_tmp56C.effect))->v))return 0;if(!Cyc_Tcutil_sub_rgnpo(
_tmp56A.rgn_po,_tmp56C.rgn_po))return 0;if(!Cyc_Tcutil_sub_attributes(_tmp56A.attributes,
_tmp56C.attributes))return 0;return 1;}_LL3C8:;_LL3C9: return 0;_LL3C1:;}}static int
Cyc_Tcutil_isomorphic(void*t1,void*t2);static int Cyc_Tcutil_isomorphic(void*t1,
void*t2){struct _tuple0 _tmpDAE;struct _tuple0 _tmp58D=(_tmpDAE.f1=Cyc_Tcutil_compress(
t1),((_tmpDAE.f2=Cyc_Tcutil_compress(t2),_tmpDAE)));void*_tmp58E;void*_tmp58F;
void*_tmp590;void*_tmp591;_LL3D2: _tmp58E=_tmp58D.f1;if(_tmp58E <= (void*)4)goto
_LL3D4;if(*((int*)_tmp58E)!= 5)goto _LL3D4;_tmp58F=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp58E)->f2;_tmp590=_tmp58D.f2;if(_tmp590 <= (void*)4)goto _LL3D4;if(*((int*)
_tmp590)!= 5)goto _LL3D4;_tmp591=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp590)->f2;
_LL3D3: return(_tmp58F == _tmp591  || _tmp58F == (void*)2  && _tmp591 == (void*)3) || 
_tmp58F == (void*)3  && _tmp591 == (void*)2;_LL3D4:;_LL3D5: return 0;_LL3D1:;}static
int Cyc_Tcutil_ptrsubtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*
t1,void*t2);static int Cyc_Tcutil_ptrsubtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*
assume,void*t1,void*t2){struct _RegionHandle*_tmp592=Cyc_Tcenv_get_fnrgn(te);
struct Cyc_List_List*tqs1=Cyc_Tcutil_flatten_typ(_tmp592,te,t1);struct Cyc_List_List*
tqs2=Cyc_Tcutil_flatten_typ(_tmp592,te,t2);for(0;tqs2 != 0;(tqs2=tqs2->tl,tqs1=
tqs1->tl)){if(tqs1 == 0)return 0;{struct _tuple5 _tmp594;struct Cyc_Absyn_Tqual
_tmp595;void*_tmp596;struct _tuple5*_tmp593=(struct _tuple5*)tqs1->hd;_tmp594=*
_tmp593;_tmp595=_tmp594.f1;_tmp596=_tmp594.f2;{struct _tuple5 _tmp598;struct Cyc_Absyn_Tqual
_tmp599;void*_tmp59A;struct _tuple5*_tmp597=(struct _tuple5*)tqs2->hd;_tmp598=*
_tmp597;_tmp599=_tmp598.f1;_tmp59A=_tmp598.f2;if(_tmp599.real_const  && Cyc_Tcutil_subtype(
te,assume,_tmp596,_tmp59A))continue;else{if(Cyc_Tcutil_unify(_tmp596,_tmp59A))
continue;else{if(Cyc_Tcutil_isomorphic(_tmp596,_tmp59A))continue;else{return 0;}}}}}}
return 1;}static int Cyc_Tcutil_is_char_type(void*t);static int Cyc_Tcutil_is_char_type(
void*t){void*_tmp59B=Cyc_Tcutil_compress(t);void*_tmp59C;_LL3D7: if(_tmp59B <= (
void*)4)goto _LL3D9;if(*((int*)_tmp59B)!= 5)goto _LL3D9;_tmp59C=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp59B)->f2;if((int)_tmp59C != 0)goto _LL3D9;_LL3D8: return 1;_LL3D9:;_LL3DA: return
0;_LL3D6:;}void*Cyc_Tcutil_castable(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void*t1,void*t2);void*Cyc_Tcutil_castable(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void*t1,void*t2){if(Cyc_Tcutil_unify(t1,t2))return(void*)1;t1=Cyc_Tcutil_compress(
t1);t2=Cyc_Tcutil_compress(t2);if(t2 == (void*)0)return(void*)1;{void*_tmp59D=t2;
void*_tmp59E;void*_tmp59F;_LL3DC: if(_tmp59D <= (void*)4)goto _LL3E0;if(*((int*)
_tmp59D)!= 5)goto _LL3DE;_tmp59E=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp59D)->f2;
if((int)_tmp59E != 2)goto _LL3DE;_LL3DD: goto _LL3DF;_LL3DE: if(*((int*)_tmp59D)!= 5)
goto _LL3E0;_tmp59F=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp59D)->f2;if((int)
_tmp59F != 3)goto _LL3E0;_LL3DF: if(Cyc_Tcutil_typ_kind(t1)== (void*)2)return(void*)
1;goto _LL3DB;_LL3E0:;_LL3E1: goto _LL3DB;_LL3DB:;}{void*_tmp5A0=t1;struct Cyc_Absyn_PtrInfo
_tmp5A1;void*_tmp5A2;struct Cyc_Absyn_Tqual _tmp5A3;struct Cyc_Absyn_PtrAtts _tmp5A4;
void*_tmp5A5;struct Cyc_Absyn_Conref*_tmp5A6;struct Cyc_Absyn_Conref*_tmp5A7;
struct Cyc_Absyn_Conref*_tmp5A8;struct Cyc_Absyn_ArrayInfo _tmp5A9;void*_tmp5AA;
struct Cyc_Absyn_Tqual _tmp5AB;struct Cyc_Absyn_Exp*_tmp5AC;struct Cyc_Absyn_Conref*
_tmp5AD;struct Cyc_Absyn_Enumdecl*_tmp5AE;void*_tmp5AF;_LL3E3: if(_tmp5A0 <= (void*)
4)goto _LL3EB;if(*((int*)_tmp5A0)!= 4)goto _LL3E5;_tmp5A1=((struct Cyc_Absyn_PointerType_struct*)
_tmp5A0)->f1;_tmp5A2=(void*)_tmp5A1.elt_typ;_tmp5A3=_tmp5A1.elt_tq;_tmp5A4=
_tmp5A1.ptr_atts;_tmp5A5=(void*)_tmp5A4.rgn;_tmp5A6=_tmp5A4.nullable;_tmp5A7=
_tmp5A4.bounds;_tmp5A8=_tmp5A4.zero_term;_LL3E4:{void*_tmp5B0=t2;struct Cyc_Absyn_PtrInfo
_tmp5B1;void*_tmp5B2;struct Cyc_Absyn_Tqual _tmp5B3;struct Cyc_Absyn_PtrAtts _tmp5B4;
void*_tmp5B5;struct Cyc_Absyn_Conref*_tmp5B6;struct Cyc_Absyn_Conref*_tmp5B7;
struct Cyc_Absyn_Conref*_tmp5B8;_LL3F4: if(_tmp5B0 <= (void*)4)goto _LL3F6;if(*((int*)
_tmp5B0)!= 4)goto _LL3F6;_tmp5B1=((struct Cyc_Absyn_PointerType_struct*)_tmp5B0)->f1;
_tmp5B2=(void*)_tmp5B1.elt_typ;_tmp5B3=_tmp5B1.elt_tq;_tmp5B4=_tmp5B1.ptr_atts;
_tmp5B5=(void*)_tmp5B4.rgn;_tmp5B6=_tmp5B4.nullable;_tmp5B7=_tmp5B4.bounds;
_tmp5B8=_tmp5B4.zero_term;_LL3F5: {void*coercion=(void*)3;struct _tuple0*_tmpDB1;
struct Cyc_List_List*_tmpDB0;struct Cyc_List_List*_tmp5B9=(_tmpDB0=_cycalloc(
sizeof(*_tmpDB0)),((_tmpDB0->hd=((_tmpDB1=_cycalloc(sizeof(*_tmpDB1)),((_tmpDB1->f1=
t1,((_tmpDB1->f2=t2,_tmpDB1)))))),((_tmpDB0->tl=0,_tmpDB0)))));int _tmp5BA=Cyc_Tcutil_ptrsubtype(
te,_tmp5B9,_tmp5A2,_tmp5B2) && (!_tmp5A3.real_const  || _tmp5B3.real_const);Cyc_Tcutil_t1_failure=
t1;Cyc_Tcutil_t2_failure=t2;{int zeroterm_ok=((int(*)(int(*cmp)(int,int),struct
Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,
_tmp5A8,_tmp5B8) || !((int(*)(struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_val)(
_tmp5B8);int _tmp5BB=_tmp5BA?0:((Cyc_Tcutil_bits_only(_tmp5A2) && Cyc_Tcutil_is_char_type(
_tmp5B2)) && !((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,
_tmp5B8)) && (_tmp5B3.real_const  || !_tmp5A3.real_const);int bounds_ok=Cyc_Tcutil_unify_conrefs(
Cyc_Tcutil_unify_it_bounds,_tmp5A7,_tmp5B7);if(!bounds_ok  && !_tmp5BB){struct
_tuple0 _tmpDB2;struct _tuple0 _tmp5BD=(_tmpDB2.f1=Cyc_Absyn_conref_val(_tmp5A7),((
_tmpDB2.f2=Cyc_Absyn_conref_val(_tmp5B7),_tmpDB2)));void*_tmp5BE;struct Cyc_Absyn_Exp*
_tmp5BF;void*_tmp5C0;struct Cyc_Absyn_Exp*_tmp5C1;_LL3F9: _tmp5BE=_tmp5BD.f1;if(
_tmp5BE <= (void*)1)goto _LL3FB;if(*((int*)_tmp5BE)!= 0)goto _LL3FB;_tmp5BF=((
struct Cyc_Absyn_Upper_b_struct*)_tmp5BE)->f1;_tmp5C0=_tmp5BD.f2;if(_tmp5C0 <= (
void*)1)goto _LL3FB;if(*((int*)_tmp5C0)!= 0)goto _LL3FB;_tmp5C1=((struct Cyc_Absyn_Upper_b_struct*)
_tmp5C0)->f1;_LL3FA: if(Cyc_Evexp_lte_const_exp(_tmp5C1,_tmp5BF))bounds_ok=1;goto
_LL3F8;_LL3FB:;_LL3FC: bounds_ok=1;goto _LL3F8;_LL3F8:;}if(((int(*)(int,struct Cyc_Absyn_Conref*
x))Cyc_Absyn_conref_def)(0,_tmp5A6) && !((int(*)(int,struct Cyc_Absyn_Conref*x))
Cyc_Absyn_conref_def)(0,_tmp5B6))coercion=(void*)2;if(((bounds_ok  && zeroterm_ok)
 && (_tmp5BA  || _tmp5BB)) && (Cyc_Tcutil_unify(_tmp5A5,_tmp5B5) || Cyc_Tcenv_region_outlives(
te,_tmp5A5,_tmp5B5)))return coercion;else{return(void*)0;}}}_LL3F6:;_LL3F7: goto
_LL3F3;_LL3F3:;}return(void*)0;_LL3E5: if(*((int*)_tmp5A0)!= 7)goto _LL3E7;_tmp5A9=((
struct Cyc_Absyn_ArrayType_struct*)_tmp5A0)->f1;_tmp5AA=(void*)_tmp5A9.elt_type;
_tmp5AB=_tmp5A9.tq;_tmp5AC=_tmp5A9.num_elts;_tmp5AD=_tmp5A9.zero_term;_LL3E6:{
void*_tmp5C4=t2;struct Cyc_Absyn_ArrayInfo _tmp5C5;void*_tmp5C6;struct Cyc_Absyn_Tqual
_tmp5C7;struct Cyc_Absyn_Exp*_tmp5C8;struct Cyc_Absyn_Conref*_tmp5C9;_LL3FE: if(
_tmp5C4 <= (void*)4)goto _LL400;if(*((int*)_tmp5C4)!= 7)goto _LL400;_tmp5C5=((
struct Cyc_Absyn_ArrayType_struct*)_tmp5C4)->f1;_tmp5C6=(void*)_tmp5C5.elt_type;
_tmp5C7=_tmp5C5.tq;_tmp5C8=_tmp5C5.num_elts;_tmp5C9=_tmp5C5.zero_term;_LL3FF: {
int okay;okay=((_tmp5AC != 0  && _tmp5C8 != 0) && ((int(*)(int(*cmp)(int,int),struct
Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,
_tmp5AD,_tmp5C9)) && Cyc_Evexp_lte_const_exp((struct Cyc_Absyn_Exp*)_tmp5C8,(
struct Cyc_Absyn_Exp*)_tmp5AC);return(okay  && Cyc_Tcutil_unify(_tmp5AA,_tmp5C6))
 && (!_tmp5AB.real_const  || _tmp5C7.real_const)?(void*)3:(void*)0;}_LL400:;
_LL401: return(void*)0;_LL3FD:;}return(void*)0;_LL3E7: if(*((int*)_tmp5A0)!= 12)
goto _LL3E9;_tmp5AE=((struct Cyc_Absyn_EnumType_struct*)_tmp5A0)->f2;_LL3E8:{void*
_tmp5CA=t2;struct Cyc_Absyn_Enumdecl*_tmp5CB;_LL403: if(_tmp5CA <= (void*)4)goto
_LL405;if(*((int*)_tmp5CA)!= 12)goto _LL405;_tmp5CB=((struct Cyc_Absyn_EnumType_struct*)
_tmp5CA)->f2;_LL404: if((_tmp5AE->fields != 0  && _tmp5CB->fields != 0) && ((int(*)(
struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp5AE->fields))->v)>= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)((
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp5CB->fields))->v))
return(void*)1;goto _LL402;_LL405:;_LL406: goto _LL402;_LL402:;}goto _LL3EA;_LL3E9:
if(*((int*)_tmp5A0)!= 5)goto _LL3EB;_LL3EA: goto _LL3EC;_LL3EB: if((int)_tmp5A0 != 1)
goto _LL3ED;_LL3EC: goto _LL3EE;_LL3ED: if(_tmp5A0 <= (void*)4)goto _LL3F1;if(*((int*)
_tmp5A0)!= 6)goto _LL3EF;_LL3EE: return Cyc_Tcutil_coerceable(t2)?(void*)1:(void*)0;
_LL3EF: if(*((int*)_tmp5A0)!= 14)goto _LL3F1;_tmp5AF=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp5A0)->f1;_LL3F0:{void*_tmp5CC=t2;void*_tmp5CD;_LL408: if(_tmp5CC <= (void*)4)
goto _LL40A;if(*((int*)_tmp5CC)!= 14)goto _LL40A;_tmp5CD=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp5CC)->f1;_LL409: if(Cyc_Tcenv_region_outlives(te,_tmp5AF,_tmp5CD))return(void*)
1;goto _LL407;_LL40A:;_LL40B: goto _LL407;_LL407:;}return(void*)0;_LL3F1:;_LL3F2:
return(void*)0;_LL3E2:;}}void Cyc_Tcutil_unchecked_cast(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Absyn_Exp*e,void*t,void*c);void Cyc_Tcutil_unchecked_cast(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e,void*t,void*c){if(!Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,t)){struct Cyc_Absyn_Exp*_tmp5CE=Cyc_Absyn_copy_exp(e);{
struct Cyc_Absyn_Cast_e_struct _tmpDB5;struct Cyc_Absyn_Cast_e_struct*_tmpDB4;(void*)(
e->r=(void*)((void*)((_tmpDB4=_cycalloc(sizeof(*_tmpDB4)),((_tmpDB4[0]=((_tmpDB5.tag=
15,((_tmpDB5.f1=(void*)t,((_tmpDB5.f2=_tmp5CE,((_tmpDB5.f3=0,((_tmpDB5.f4=(void*)
c,_tmpDB5)))))))))),_tmpDB4))))));}{struct Cyc_Core_Opt*_tmpDB6;e->topt=((_tmpDB6=
_cycalloc(sizeof(*_tmpDB6)),((_tmpDB6->v=(void*)t,_tmpDB6))));}}}int Cyc_Tcutil_is_integral(
struct Cyc_Absyn_Exp*e);int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*e){void*
_tmp5D2=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
_LL40D: if(_tmp5D2 <= (void*)4)goto _LL417;if(*((int*)_tmp5D2)!= 5)goto _LL40F;
_LL40E: goto _LL410;_LL40F: if(*((int*)_tmp5D2)!= 12)goto _LL411;_LL410: goto _LL412;
_LL411: if(*((int*)_tmp5D2)!= 13)goto _LL413;_LL412: goto _LL414;_LL413: if(*((int*)
_tmp5D2)!= 18)goto _LL415;_LL414: return 1;_LL415: if(*((int*)_tmp5D2)!= 0)goto
_LL417;_LL416: return Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,
Cyc_Absyn_sint_typ);_LL417:;_LL418: return 0;_LL40C:;}int Cyc_Tcutil_is_numeric(
struct Cyc_Absyn_Exp*e);int Cyc_Tcutil_is_numeric(struct Cyc_Absyn_Exp*e){if(Cyc_Tcutil_is_integral(
e))return 1;{void*_tmp5D3=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);_LL41A: if((int)_tmp5D3 != 1)goto _LL41C;_LL41B: goto
_LL41D;_LL41C: if(_tmp5D3 <= (void*)4)goto _LL41E;if(*((int*)_tmp5D3)!= 6)goto
_LL41E;_LL41D: return 1;_LL41E:;_LL41F: return 0;_LL419:;}}int Cyc_Tcutil_is_function_type(
void*t);int Cyc_Tcutil_is_function_type(void*t){void*_tmp5D4=Cyc_Tcutil_compress(
t);_LL421: if(_tmp5D4 <= (void*)4)goto _LL423;if(*((int*)_tmp5D4)!= 8)goto _LL423;
_LL422: return 1;_LL423:;_LL424: return 0;_LL420:;}void*Cyc_Tcutil_max_arithmetic_type(
void*t1,void*t2);void*Cyc_Tcutil_max_arithmetic_type(void*t1,void*t2){struct
_tuple0 _tmpDB7;struct _tuple0 _tmp5D6=(_tmpDB7.f1=t1,((_tmpDB7.f2=t2,_tmpDB7)));
void*_tmp5D7;int _tmp5D8;void*_tmp5D9;int _tmp5DA;void*_tmp5DB;void*_tmp5DC;void*
_tmp5DD;void*_tmp5DE;void*_tmp5DF;void*_tmp5E0;void*_tmp5E1;void*_tmp5E2;void*
_tmp5E3;void*_tmp5E4;void*_tmp5E5;void*_tmp5E6;void*_tmp5E7;void*_tmp5E8;void*
_tmp5E9;void*_tmp5EA;void*_tmp5EB;void*_tmp5EC;void*_tmp5ED;void*_tmp5EE;void*
_tmp5EF;void*_tmp5F0;void*_tmp5F1;void*_tmp5F2;void*_tmp5F3;void*_tmp5F4;void*
_tmp5F5;void*_tmp5F6;void*_tmp5F7;void*_tmp5F8;_LL426: _tmp5D7=_tmp5D6.f1;if(
_tmp5D7 <= (void*)4)goto _LL428;if(*((int*)_tmp5D7)!= 6)goto _LL428;_tmp5D8=((
struct Cyc_Absyn_DoubleType_struct*)_tmp5D7)->f1;_tmp5D9=_tmp5D6.f2;if(_tmp5D9 <= (
void*)4)goto _LL428;if(*((int*)_tmp5D9)!= 6)goto _LL428;_tmp5DA=((struct Cyc_Absyn_DoubleType_struct*)
_tmp5D9)->f1;_LL427: if(_tmp5D8)return t1;else{return t2;}_LL428: _tmp5DB=_tmp5D6.f1;
if(_tmp5DB <= (void*)4)goto _LL42A;if(*((int*)_tmp5DB)!= 6)goto _LL42A;_LL429:
return t1;_LL42A: _tmp5DC=_tmp5D6.f2;if(_tmp5DC <= (void*)4)goto _LL42C;if(*((int*)
_tmp5DC)!= 6)goto _LL42C;_LL42B: return t2;_LL42C: _tmp5DD=_tmp5D6.f1;if((int)
_tmp5DD != 1)goto _LL42E;_LL42D: goto _LL42F;_LL42E: _tmp5DE=_tmp5D6.f2;if((int)
_tmp5DE != 1)goto _LL430;_LL42F: return(void*)1;_LL430: _tmp5DF=_tmp5D6.f1;if(
_tmp5DF <= (void*)4)goto _LL432;if(*((int*)_tmp5DF)!= 5)goto _LL432;_tmp5E0=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp5DF)->f1;if((int)_tmp5E0 != 1)goto _LL432;
_tmp5E1=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp5DF)->f2;if((int)_tmp5E1 != 
4)goto _LL432;_LL431: goto _LL433;_LL432: _tmp5E2=_tmp5D6.f2;if(_tmp5E2 <= (void*)4)
goto _LL434;if(*((int*)_tmp5E2)!= 5)goto _LL434;_tmp5E3=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp5E2)->f1;if((int)_tmp5E3 != 1)goto _LL434;_tmp5E4=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp5E2)->f2;if((int)_tmp5E4 != 4)goto _LL434;_LL433: return Cyc_Absyn_ulonglong_typ;
_LL434: _tmp5E5=_tmp5D6.f1;if(_tmp5E5 <= (void*)4)goto _LL436;if(*((int*)_tmp5E5)!= 
5)goto _LL436;_tmp5E6=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp5E5)->f2;if((
int)_tmp5E6 != 4)goto _LL436;_LL435: goto _LL437;_LL436: _tmp5E7=_tmp5D6.f2;if(
_tmp5E7 <= (void*)4)goto _LL438;if(*((int*)_tmp5E7)!= 5)goto _LL438;_tmp5E8=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp5E7)->f2;if((int)_tmp5E8 != 4)goto _LL438;
_LL437: return Cyc_Absyn_slonglong_typ;_LL438: _tmp5E9=_tmp5D6.f1;if(_tmp5E9 <= (
void*)4)goto _LL43A;if(*((int*)_tmp5E9)!= 5)goto _LL43A;_tmp5EA=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp5E9)->f1;if((int)_tmp5EA != 1)goto _LL43A;_tmp5EB=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp5E9)->f2;if((int)_tmp5EB != 3)goto _LL43A;_LL439: goto _LL43B;_LL43A: _tmp5EC=
_tmp5D6.f2;if(_tmp5EC <= (void*)4)goto _LL43C;if(*((int*)_tmp5EC)!= 5)goto _LL43C;
_tmp5ED=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp5EC)->f1;if((int)_tmp5ED != 
1)goto _LL43C;_tmp5EE=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp5EC)->f2;if((
int)_tmp5EE != 3)goto _LL43C;_LL43B: return Cyc_Absyn_ulong_typ;_LL43C: _tmp5EF=
_tmp5D6.f1;if(_tmp5EF <= (void*)4)goto _LL43E;if(*((int*)_tmp5EF)!= 5)goto _LL43E;
_tmp5F0=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp5EF)->f1;if((int)_tmp5F0 != 
1)goto _LL43E;_tmp5F1=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp5EF)->f2;if((
int)_tmp5F1 != 2)goto _LL43E;_LL43D: goto _LL43F;_LL43E: _tmp5F2=_tmp5D6.f2;if(
_tmp5F2 <= (void*)4)goto _LL440;if(*((int*)_tmp5F2)!= 5)goto _LL440;_tmp5F3=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp5F2)->f1;if((int)_tmp5F3 != 1)goto _LL440;
_tmp5F4=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp5F2)->f2;if((int)_tmp5F4 != 
2)goto _LL440;_LL43F: return Cyc_Absyn_uint_typ;_LL440: _tmp5F5=_tmp5D6.f1;if(
_tmp5F5 <= (void*)4)goto _LL442;if(*((int*)_tmp5F5)!= 5)goto _LL442;_tmp5F6=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp5F5)->f2;if((int)_tmp5F6 != 3)goto _LL442;
_LL441: goto _LL443;_LL442: _tmp5F7=_tmp5D6.f2;if(_tmp5F7 <= (void*)4)goto _LL444;if(*((
int*)_tmp5F7)!= 5)goto _LL444;_tmp5F8=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp5F7)->f2;if((int)_tmp5F8 != 3)goto _LL444;_LL443: return Cyc_Absyn_slong_typ;
_LL444:;_LL445: return Cyc_Absyn_sint_typ;_LL425:;}void Cyc_Tcutil_check_contains_assign(
struct Cyc_Absyn_Exp*e);void Cyc_Tcutil_check_contains_assign(struct Cyc_Absyn_Exp*
e){void*_tmp5F9=(void*)e->r;struct Cyc_Core_Opt*_tmp5FA;_LL447: if(*((int*)_tmp5F9)
!= 4)goto _LL449;_tmp5FA=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp5F9)->f2;if(
_tmp5FA != 0)goto _LL449;_LL448:{const char*_tmpDBA;void*_tmpDB9;(_tmpDB9=0,Cyc_Tcutil_warn(
e->loc,((_tmpDBA="assignment in test",_tag_dyneither(_tmpDBA,sizeof(char),19))),
_tag_dyneither(_tmpDB9,sizeof(void*),0)));}goto _LL446;_LL449:;_LL44A: goto _LL446;
_LL446:;}static int Cyc_Tcutil_constrain_kinds(void*c1,void*c2);static int Cyc_Tcutil_constrain_kinds(
void*c1,void*c2){c1=Cyc_Absyn_compress_kb(c1);c2=Cyc_Absyn_compress_kb(c2);{
struct _tuple0 _tmpDBB;struct _tuple0 _tmp5FE=(_tmpDBB.f1=c1,((_tmpDBB.f2=c2,_tmpDBB)));
void*_tmp5FF;void*_tmp600;void*_tmp601;void*_tmp602;void*_tmp603;struct Cyc_Core_Opt*
_tmp604;struct Cyc_Core_Opt**_tmp605;void*_tmp606;struct Cyc_Core_Opt*_tmp607;
struct Cyc_Core_Opt**_tmp608;void*_tmp609;struct Cyc_Core_Opt*_tmp60A;struct Cyc_Core_Opt**
_tmp60B;void*_tmp60C;void*_tmp60D;void*_tmp60E;void*_tmp60F;void*_tmp610;void*
_tmp611;struct Cyc_Core_Opt*_tmp612;struct Cyc_Core_Opt**_tmp613;void*_tmp614;void*
_tmp615;struct Cyc_Core_Opt*_tmp616;struct Cyc_Core_Opt**_tmp617;void*_tmp618;void*
_tmp619;struct Cyc_Core_Opt*_tmp61A;struct Cyc_Core_Opt**_tmp61B;void*_tmp61C;
_LL44C: _tmp5FF=_tmp5FE.f1;if(*((int*)_tmp5FF)!= 0)goto _LL44E;_tmp600=(void*)((
struct Cyc_Absyn_Eq_kb_struct*)_tmp5FF)->f1;_tmp601=_tmp5FE.f2;if(*((int*)_tmp601)
!= 0)goto _LL44E;_tmp602=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp601)->f1;
_LL44D: return _tmp600 == _tmp602;_LL44E: _tmp603=_tmp5FE.f2;if(*((int*)_tmp603)!= 1)
goto _LL450;_tmp604=((struct Cyc_Absyn_Unknown_kb_struct*)_tmp603)->f1;_tmp605=(
struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)_tmp603)->f1;_LL44F:{
struct Cyc_Core_Opt*_tmpDBC;*_tmp605=((_tmpDBC=_cycalloc(sizeof(*_tmpDBC)),((
_tmpDBC->v=(void*)c1,_tmpDBC))));}return 1;_LL450: _tmp606=_tmp5FE.f1;if(*((int*)
_tmp606)!= 1)goto _LL452;_tmp607=((struct Cyc_Absyn_Unknown_kb_struct*)_tmp606)->f1;
_tmp608=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)_tmp606)->f1;
_LL451:{struct Cyc_Core_Opt*_tmpDBD;*_tmp608=((_tmpDBD=_cycalloc(sizeof(*_tmpDBD)),((
_tmpDBD->v=(void*)c2,_tmpDBD))));}return 1;_LL452: _tmp609=_tmp5FE.f1;if(*((int*)
_tmp609)!= 2)goto _LL454;_tmp60A=((struct Cyc_Absyn_Less_kb_struct*)_tmp609)->f1;
_tmp60B=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)_tmp609)->f1;
_tmp60C=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp609)->f2;_tmp60D=_tmp5FE.f2;
if(*((int*)_tmp60D)!= 0)goto _LL454;_tmp60E=(void*)((struct Cyc_Absyn_Eq_kb_struct*)
_tmp60D)->f1;_LL453: if(Cyc_Tcutil_kind_leq(_tmp60E,_tmp60C)){{struct Cyc_Core_Opt*
_tmpDBE;*_tmp60B=((_tmpDBE=_cycalloc(sizeof(*_tmpDBE)),((_tmpDBE->v=(void*)c2,
_tmpDBE))));}return 1;}else{return 0;}_LL454: _tmp60F=_tmp5FE.f1;if(*((int*)_tmp60F)
!= 0)goto _LL456;_tmp610=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp60F)->f1;
_tmp611=_tmp5FE.f2;if(*((int*)_tmp611)!= 2)goto _LL456;_tmp612=((struct Cyc_Absyn_Less_kb_struct*)
_tmp611)->f1;_tmp613=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp611)->f1;_tmp614=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp611)->f2;
_LL455: if(Cyc_Tcutil_kind_leq(_tmp610,_tmp614)){{struct Cyc_Core_Opt*_tmpDBF;*
_tmp613=((_tmpDBF=_cycalloc(sizeof(*_tmpDBF)),((_tmpDBF->v=(void*)c1,_tmpDBF))));}
return 1;}else{return 0;}_LL456: _tmp615=_tmp5FE.f1;if(*((int*)_tmp615)!= 2)goto
_LL44B;_tmp616=((struct Cyc_Absyn_Less_kb_struct*)_tmp615)->f1;_tmp617=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*)_tmp615)->f1;_tmp618=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp615)->f2;_tmp619=_tmp5FE.f2;if(*((int*)_tmp619)!= 2)goto _LL44B;_tmp61A=((
struct Cyc_Absyn_Less_kb_struct*)_tmp619)->f1;_tmp61B=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*)_tmp619)->f1;_tmp61C=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp619)->f2;_LL457: if(Cyc_Tcutil_kind_leq(_tmp618,_tmp61C)){{struct Cyc_Core_Opt*
_tmpDC0;*_tmp61B=((_tmpDC0=_cycalloc(sizeof(*_tmpDC0)),((_tmpDC0->v=(void*)c1,
_tmpDC0))));}return 1;}else{if(Cyc_Tcutil_kind_leq(_tmp61C,_tmp618)){{struct Cyc_Core_Opt*
_tmpDC1;*_tmp617=((_tmpDC1=_cycalloc(sizeof(*_tmpDC1)),((_tmpDC1->v=(void*)c2,
_tmpDC1))));}return 1;}else{return 0;}}_LL44B:;}}static int Cyc_Tcutil_tvar_id_counter=
0;int Cyc_Tcutil_new_tvar_id();int Cyc_Tcutil_new_tvar_id(){return Cyc_Tcutil_tvar_id_counter
++;}static int Cyc_Tcutil_tvar_counter=0;struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(
void*k);struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k){int i=Cyc_Tcutil_tvar_counter
++;const char*_tmpDC5;void*_tmpDC4[1];struct Cyc_Int_pa_struct _tmpDC3;struct
_dyneither_ptr s=(struct _dyneither_ptr)((_tmpDC3.tag=1,((_tmpDC3.f1=(
unsigned long)i,((_tmpDC4[0]=& _tmpDC3,Cyc_aprintf(((_tmpDC5="#%d",_tag_dyneither(
_tmpDC5,sizeof(char),4))),_tag_dyneither(_tmpDC4,sizeof(void*),1))))))));struct
_dyneither_ptr*_tmpDC8;struct Cyc_Absyn_Tvar*_tmpDC7;return(_tmpDC7=_cycalloc(
sizeof(*_tmpDC7)),((_tmpDC7->name=((_tmpDC8=_cycalloc(sizeof(struct
_dyneither_ptr)* 1),((_tmpDC8[0]=s,_tmpDC8)))),((_tmpDC7->identity=- 1,((_tmpDC7->kind=(
void*)k,_tmpDC7)))))));}int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*t);int
Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*t){struct _dyneither_ptr _tmp628=*t->name;
return*((const char*)_check_dyneither_subscript(_tmp628,sizeof(char),0))== '#';}
void Cyc_Tcutil_rewrite_temp_tvar(struct Cyc_Absyn_Tvar*t);void Cyc_Tcutil_rewrite_temp_tvar(
struct Cyc_Absyn_Tvar*t){{const char*_tmpDCC;void*_tmpDCB[1];struct Cyc_String_pa_struct
_tmpDCA;(_tmpDCA.tag=0,((_tmpDCA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*
t->name),((_tmpDCB[0]=& _tmpDCA,Cyc_printf(((_tmpDCC="%s",_tag_dyneither(_tmpDCC,
sizeof(char),3))),_tag_dyneither(_tmpDCB,sizeof(void*),1)))))));}if(!Cyc_Tcutil_is_temp_tvar(
t))return;{const char*_tmpDCD;struct _dyneither_ptr _tmp62C=Cyc_strconcat(((_tmpDCD="`",
_tag_dyneither(_tmpDCD,sizeof(char),2))),(struct _dyneither_ptr)*t->name);{char
_tmpDD0;char _tmpDCF;struct _dyneither_ptr _tmpDCE;(_tmpDCE=_dyneither_ptr_plus(
_tmp62C,sizeof(char),1),((_tmpDCF=*((char*)_check_dyneither_subscript(_tmpDCE,
sizeof(char),0)),((_tmpDD0='t',((_get_dyneither_size(_tmpDCE,sizeof(char))== 1
 && (_tmpDCF == '\000'  && _tmpDD0 != '\000')?_throw_arraybounds(): 1,*((char*)
_tmpDCE.curr)=_tmpDD0)))))));}{struct _dyneither_ptr*_tmpDD1;t->name=((_tmpDD1=
_cycalloc(sizeof(struct _dyneither_ptr)* 1),((_tmpDD1[0]=(struct _dyneither_ptr)
_tmp62C,_tmpDD1))));}}}struct _tuple14{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual
f2;void*f3;};static struct _tuple3*Cyc_Tcutil_fndecl2typ_f(struct _tuple14*x);
static struct _tuple3*Cyc_Tcutil_fndecl2typ_f(struct _tuple14*x){struct Cyc_Core_Opt*
_tmpDD4;struct _tuple3*_tmpDD3;return(_tmpDD3=_cycalloc(sizeof(*_tmpDD3)),((
_tmpDD3->f1=(struct Cyc_Core_Opt*)((_tmpDD4=_cycalloc(sizeof(*_tmpDD4)),((_tmpDD4->v=(*
x).f1,_tmpDD4)))),((_tmpDD3->f2=(*x).f2,((_tmpDD3->f3=(*x).f3,_tmpDD3)))))));}
void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*fd);void*Cyc_Tcutil_fndecl2typ(
struct Cyc_Absyn_Fndecl*fd){if(fd->cached_typ == 0){struct Cyc_List_List*_tmp634=0;{
struct Cyc_List_List*atts=fd->attributes;for(0;atts != 0;atts=atts->tl){if(Cyc_Absyn_fntype_att((
void*)atts->hd)){struct Cyc_List_List*_tmpDD5;_tmp634=((_tmpDD5=_cycalloc(sizeof(*
_tmpDD5)),((_tmpDD5->hd=(void*)((void*)atts->hd),((_tmpDD5->tl=_tmp634,_tmpDD5))))));}}}{
struct Cyc_Absyn_FnType_struct _tmpDDB;struct Cyc_Absyn_FnInfo _tmpDDA;struct Cyc_Absyn_FnType_struct*
_tmpDD9;return(void*)((_tmpDD9=_cycalloc(sizeof(*_tmpDD9)),((_tmpDD9[0]=((
_tmpDDB.tag=8,((_tmpDDB.f1=((_tmpDDA.tvars=fd->tvs,((_tmpDDA.effect=fd->effect,((
_tmpDDA.ret_typ=(void*)((void*)fd->ret_type),((_tmpDDA.args=((struct Cyc_List_List*(*)(
struct _tuple3*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_fndecl2typ_f,
fd->args),((_tmpDDA.c_varargs=fd->c_varargs,((_tmpDDA.cyc_varargs=fd->cyc_varargs,((
_tmpDDA.rgn_po=fd->rgn_po,((_tmpDDA.attributes=_tmp634,_tmpDDA)))))))))))))))),
_tmpDDB)))),_tmpDD9))));}}return(void*)((struct Cyc_Core_Opt*)_check_null(fd->cached_typ))->v;}
struct _tuple15{void*f1;struct Cyc_Absyn_Tqual f2;void*f3;};static void*Cyc_Tcutil_fst_fdarg(
struct _tuple15*t);static void*Cyc_Tcutil_fst_fdarg(struct _tuple15*t){return(*t).f1;}
void*Cyc_Tcutil_snd_tqt(struct _tuple5*t);void*Cyc_Tcutil_snd_tqt(struct _tuple5*t){
return(*t).f2;}static struct _tuple5*Cyc_Tcutil_map2_tq(struct _tuple5*pr,void*t);
static struct _tuple5*Cyc_Tcutil_map2_tq(struct _tuple5*pr,void*t){struct _tuple5*
_tmpDDC;return(_tmpDDC=_cycalloc(sizeof(*_tmpDDC)),((_tmpDDC->f1=(*pr).f1,((
_tmpDDC->f2=t,_tmpDDC)))));}struct _tuple16{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Tqual
f2;};struct _tuple17{struct _tuple16*f1;void*f2;};static struct _tuple17*Cyc_Tcutil_substitute_f1(
struct _RegionHandle*rgn,struct _tuple3*y);static struct _tuple17*Cyc_Tcutil_substitute_f1(
struct _RegionHandle*rgn,struct _tuple3*y){struct _tuple16*_tmpDDF;struct _tuple17*
_tmpDDE;return(_tmpDDE=_region_malloc(rgn,sizeof(*_tmpDDE)),((_tmpDDE->f1=((
_tmpDDF=_region_malloc(rgn,sizeof(*_tmpDDF)),((_tmpDDF->f1=(*y).f1,((_tmpDDF->f2=(*
y).f2,_tmpDDF)))))),((_tmpDDE->f2=(*y).f3,_tmpDDE)))));}static struct _tuple3*Cyc_Tcutil_substitute_f2(
struct _tuple17*w);static struct _tuple3*Cyc_Tcutil_substitute_f2(struct _tuple17*w){
struct _tuple16*_tmp63D;void*_tmp63E;struct _tuple17 _tmp63C=*w;_tmp63D=_tmp63C.f1;
_tmp63E=_tmp63C.f2;{struct Cyc_Core_Opt*_tmp640;struct Cyc_Absyn_Tqual _tmp641;
struct _tuple16 _tmp63F=*_tmp63D;_tmp640=_tmp63F.f1;_tmp641=_tmp63F.f2;{struct
_tuple3*_tmpDE0;return(_tmpDE0=_cycalloc(sizeof(*_tmpDE0)),((_tmpDE0->f1=_tmp640,((
_tmpDE0->f2=_tmp641,((_tmpDE0->f3=_tmp63E,_tmpDE0)))))));}}}static void*Cyc_Tcutil_field_type(
struct Cyc_Absyn_Aggrfield*f);static void*Cyc_Tcutil_field_type(struct Cyc_Absyn_Aggrfield*
f){return(void*)f->type;}static struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_zip_field_type(
struct Cyc_Absyn_Aggrfield*f,void*t);static struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_zip_field_type(
struct Cyc_Absyn_Aggrfield*f,void*t){struct Cyc_Absyn_Aggrfield*_tmpDE1;return(
_tmpDE1=_cycalloc(sizeof(*_tmpDE1)),((_tmpDE1->name=f->name,((_tmpDE1->tq=f->tq,((
_tmpDE1->type=(void*)t,((_tmpDE1->width=f->width,((_tmpDE1->attributes=f->attributes,
_tmpDE1)))))))))));}static struct Cyc_List_List*Cyc_Tcutil_substs(struct
_RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*ts);static struct
Cyc_Absyn_Exp*Cyc_Tcutil_copye(struct Cyc_Absyn_Exp*old,void*r);static struct Cyc_Absyn_Exp*
Cyc_Tcutil_copye(struct Cyc_Absyn_Exp*old,void*r){struct Cyc_Absyn_Exp*_tmpDE2;
return(_tmpDE2=_cycalloc(sizeof(*_tmpDE2)),((_tmpDE2->topt=old->topt,((_tmpDE2->r=(
void*)r,((_tmpDE2->loc=old->loc,((_tmpDE2->annot=(void*)((void*)old->annot),
_tmpDE2)))))))));}static struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubsexp(struct
_RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_Absyn_Exp*e);static struct Cyc_Absyn_Exp*
Cyc_Tcutil_rsubsexp(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_Absyn_Exp*
e){void*_tmp645=(void*)e->r;void*_tmp646;struct Cyc_List_List*_tmp647;struct Cyc_Absyn_Exp*
_tmp648;struct Cyc_Absyn_Exp*_tmp649;struct Cyc_Absyn_Exp*_tmp64A;struct Cyc_Absyn_Exp*
_tmp64B;struct Cyc_Absyn_Exp*_tmp64C;struct Cyc_Absyn_Exp*_tmp64D;struct Cyc_Absyn_Exp*
_tmp64E;struct Cyc_Absyn_Exp*_tmp64F;struct Cyc_Absyn_Exp*_tmp650;void*_tmp651;
struct Cyc_Absyn_Exp*_tmp652;int _tmp653;void*_tmp654;void*_tmp655;struct Cyc_Absyn_Exp*
_tmp656;void*_tmp657;void*_tmp658;void*_tmp659;_LL459: if(*((int*)_tmp645)!= 0)
goto _LL45B;_LL45A: goto _LL45C;_LL45B: if(*((int*)_tmp645)!= 33)goto _LL45D;_LL45C:
goto _LL45E;_LL45D: if(*((int*)_tmp645)!= 34)goto _LL45F;_LL45E: goto _LL460;_LL45F:
if(*((int*)_tmp645)!= 1)goto _LL461;_LL460: return e;_LL461: if(*((int*)_tmp645)!= 3)
goto _LL463;_tmp646=(void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp645)->f1;
_tmp647=((struct Cyc_Absyn_Primop_e_struct*)_tmp645)->f2;_LL462: if(((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmp647)== 1){struct Cyc_Absyn_Exp*
_tmp65A=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp647))->hd;
struct Cyc_Absyn_Exp*_tmp65B=Cyc_Tcutil_rsubsexp(r,inst,_tmp65A);if(_tmp65B == 
_tmp65A)return e;{struct Cyc_Absyn_Primop_e_struct _tmpDE8;struct Cyc_Absyn_Exp*
_tmpDE7[1];struct Cyc_Absyn_Primop_e_struct*_tmpDE6;return Cyc_Tcutil_copye(e,(
void*)((_tmpDE6=_cycalloc(sizeof(*_tmpDE6)),((_tmpDE6[0]=((_tmpDE8.tag=3,((
_tmpDE8.f1=(void*)_tmp646,((_tmpDE8.f2=((_tmpDE7[0]=_tmp65B,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpDE7,sizeof(struct Cyc_Absyn_Exp*),
1)))),_tmpDE8)))))),_tmpDE6)))));}}else{if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp647)== 2){struct Cyc_Absyn_Exp*_tmp65F=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp647))->hd;struct Cyc_Absyn_Exp*_tmp660=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp647->tl))->hd;struct Cyc_Absyn_Exp*_tmp661=
Cyc_Tcutil_rsubsexp(r,inst,_tmp65F);struct Cyc_Absyn_Exp*_tmp662=Cyc_Tcutil_rsubsexp(
r,inst,_tmp660);if(_tmp661 == _tmp65F  && _tmp662 == _tmp660)return e;{struct Cyc_Absyn_Primop_e_struct
_tmpDEE;struct Cyc_Absyn_Exp*_tmpDED[2];struct Cyc_Absyn_Primop_e_struct*_tmpDEC;
return Cyc_Tcutil_copye(e,(void*)((_tmpDEC=_cycalloc(sizeof(*_tmpDEC)),((_tmpDEC[
0]=((_tmpDEE.tag=3,((_tmpDEE.f1=(void*)_tmp646,((_tmpDEE.f2=((_tmpDED[1]=_tmp662,((
_tmpDED[0]=_tmp661,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpDED,sizeof(struct Cyc_Absyn_Exp*),2)))))),_tmpDEE)))))),
_tmpDEC)))));}}else{const char*_tmpDF1;void*_tmpDF0;return(_tmpDF0=0,((struct Cyc_Absyn_Exp*(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpDF1="primop does not have 1 or 2 args!",
_tag_dyneither(_tmpDF1,sizeof(char),34))),_tag_dyneither(_tmpDF0,sizeof(void*),0)));}}
_LL463: if(*((int*)_tmp645)!= 6)goto _LL465;_tmp648=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp645)->f1;_tmp649=((struct Cyc_Absyn_Conditional_e_struct*)_tmp645)->f2;
_tmp64A=((struct Cyc_Absyn_Conditional_e_struct*)_tmp645)->f3;_LL464: {struct Cyc_Absyn_Exp*
_tmp668=Cyc_Tcutil_rsubsexp(r,inst,_tmp648);struct Cyc_Absyn_Exp*_tmp669=Cyc_Tcutil_rsubsexp(
r,inst,_tmp649);struct Cyc_Absyn_Exp*_tmp66A=Cyc_Tcutil_rsubsexp(r,inst,_tmp64A);
if((_tmp668 == _tmp648  && _tmp669 == _tmp649) && _tmp66A == _tmp64A)return e;{struct
Cyc_Absyn_Conditional_e_struct _tmpDF4;struct Cyc_Absyn_Conditional_e_struct*
_tmpDF3;return Cyc_Tcutil_copye(e,(void*)((_tmpDF3=_cycalloc(sizeof(*_tmpDF3)),((
_tmpDF3[0]=((_tmpDF4.tag=6,((_tmpDF4.f1=_tmp668,((_tmpDF4.f2=_tmp669,((_tmpDF4.f3=
_tmp66A,_tmpDF4)))))))),_tmpDF3)))));}}_LL465: if(*((int*)_tmp645)!= 7)goto _LL467;
_tmp64B=((struct Cyc_Absyn_And_e_struct*)_tmp645)->f1;_tmp64C=((struct Cyc_Absyn_And_e_struct*)
_tmp645)->f2;_LL466: {struct Cyc_Absyn_Exp*_tmp66D=Cyc_Tcutil_rsubsexp(r,inst,
_tmp64B);struct Cyc_Absyn_Exp*_tmp66E=Cyc_Tcutil_rsubsexp(r,inst,_tmp64C);if(
_tmp66D == _tmp64B  && _tmp66E == _tmp64C)return e;{struct Cyc_Absyn_And_e_struct
_tmpDF7;struct Cyc_Absyn_And_e_struct*_tmpDF6;return Cyc_Tcutil_copye(e,(void*)((
_tmpDF6=_cycalloc(sizeof(*_tmpDF6)),((_tmpDF6[0]=((_tmpDF7.tag=7,((_tmpDF7.f1=
_tmp66D,((_tmpDF7.f2=_tmp66E,_tmpDF7)))))),_tmpDF6)))));}}_LL467: if(*((int*)
_tmp645)!= 8)goto _LL469;_tmp64D=((struct Cyc_Absyn_Or_e_struct*)_tmp645)->f1;
_tmp64E=((struct Cyc_Absyn_Or_e_struct*)_tmp645)->f2;_LL468: {struct Cyc_Absyn_Exp*
_tmp671=Cyc_Tcutil_rsubsexp(r,inst,_tmp64D);struct Cyc_Absyn_Exp*_tmp672=Cyc_Tcutil_rsubsexp(
r,inst,_tmp64E);if(_tmp671 == _tmp64D  && _tmp672 == _tmp64E)return e;{struct Cyc_Absyn_Or_e_struct
_tmpDFA;struct Cyc_Absyn_Or_e_struct*_tmpDF9;return Cyc_Tcutil_copye(e,(void*)((
_tmpDF9=_cycalloc(sizeof(*_tmpDF9)),((_tmpDF9[0]=((_tmpDFA.tag=8,((_tmpDFA.f1=
_tmp671,((_tmpDFA.f2=_tmp672,_tmpDFA)))))),_tmpDF9)))));}}_LL469: if(*((int*)
_tmp645)!= 9)goto _LL46B;_tmp64F=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp645)->f1;
_tmp650=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp645)->f2;_LL46A: {struct Cyc_Absyn_Exp*
_tmp675=Cyc_Tcutil_rsubsexp(r,inst,_tmp64F);struct Cyc_Absyn_Exp*_tmp676=Cyc_Tcutil_rsubsexp(
r,inst,_tmp650);if(_tmp675 == _tmp64F  && _tmp676 == _tmp650)return e;{struct Cyc_Absyn_SeqExp_e_struct
_tmpDFD;struct Cyc_Absyn_SeqExp_e_struct*_tmpDFC;return Cyc_Tcutil_copye(e,(void*)((
_tmpDFC=_cycalloc(sizeof(*_tmpDFC)),((_tmpDFC[0]=((_tmpDFD.tag=9,((_tmpDFD.f1=
_tmp675,((_tmpDFD.f2=_tmp676,_tmpDFD)))))),_tmpDFC)))));}}_LL46B: if(*((int*)
_tmp645)!= 15)goto _LL46D;_tmp651=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp645)->f1;
_tmp652=((struct Cyc_Absyn_Cast_e_struct*)_tmp645)->f2;_tmp653=((struct Cyc_Absyn_Cast_e_struct*)
_tmp645)->f3;_tmp654=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp645)->f4;_LL46C: {
struct Cyc_Absyn_Exp*_tmp679=Cyc_Tcutil_rsubsexp(r,inst,_tmp652);void*_tmp67A=Cyc_Tcutil_rsubstitute(
r,inst,_tmp651);if(_tmp679 == _tmp652  && _tmp67A == _tmp651)return e;{struct Cyc_Absyn_Cast_e_struct
_tmpE00;struct Cyc_Absyn_Cast_e_struct*_tmpDFF;return Cyc_Tcutil_copye(e,(void*)((
_tmpDFF=_cycalloc(sizeof(*_tmpDFF)),((_tmpDFF[0]=((_tmpE00.tag=15,((_tmpE00.f1=(
void*)_tmp67A,((_tmpE00.f2=_tmp679,((_tmpE00.f3=_tmp653,((_tmpE00.f4=(void*)
_tmp654,_tmpE00)))))))))),_tmpDFF)))));}}_LL46D: if(*((int*)_tmp645)!= 18)goto
_LL46F;_tmp655=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp645)->f1;_LL46E: {
void*_tmp67D=Cyc_Tcutil_rsubstitute(r,inst,_tmp655);if(_tmp67D == _tmp655)return e;{
struct Cyc_Absyn_Sizeoftyp_e_struct _tmpE03;struct Cyc_Absyn_Sizeoftyp_e_struct*
_tmpE02;return Cyc_Tcutil_copye(e,(void*)((_tmpE02=_cycalloc(sizeof(*_tmpE02)),((
_tmpE02[0]=((_tmpE03.tag=18,((_tmpE03.f1=(void*)_tmp67D,_tmpE03)))),_tmpE02)))));}}
_LL46F: if(*((int*)_tmp645)!= 19)goto _LL471;_tmp656=((struct Cyc_Absyn_Sizeofexp_e_struct*)
_tmp645)->f1;_LL470: {struct Cyc_Absyn_Exp*_tmp680=Cyc_Tcutil_rsubsexp(r,inst,
_tmp656);if(_tmp680 == _tmp656)return e;{struct Cyc_Absyn_Sizeofexp_e_struct _tmpE06;
struct Cyc_Absyn_Sizeofexp_e_struct*_tmpE05;return Cyc_Tcutil_copye(e,(void*)((
_tmpE05=_cycalloc(sizeof(*_tmpE05)),((_tmpE05[0]=((_tmpE06.tag=19,((_tmpE06.f1=
_tmp680,_tmpE06)))),_tmpE05)))));}}_LL471: if(*((int*)_tmp645)!= 20)goto _LL473;
_tmp657=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp645)->f1;_tmp658=(void*)((
struct Cyc_Absyn_Offsetof_e_struct*)_tmp645)->f2;_LL472: {void*_tmp683=Cyc_Tcutil_rsubstitute(
r,inst,_tmp657);if(_tmp683 == _tmp657)return e;{struct Cyc_Absyn_Offsetof_e_struct
_tmpE09;struct Cyc_Absyn_Offsetof_e_struct*_tmpE08;return Cyc_Tcutil_copye(e,(void*)((
_tmpE08=_cycalloc(sizeof(*_tmpE08)),((_tmpE08[0]=((_tmpE09.tag=20,((_tmpE09.f1=(
void*)_tmp683,((_tmpE09.f2=(void*)_tmp658,_tmpE09)))))),_tmpE08)))));}}_LL473:
if(*((int*)_tmp645)!= 39)goto _LL475;_tmp659=(void*)((struct Cyc_Absyn_Valueof_e_struct*)
_tmp645)->f1;_LL474: {void*_tmp686=Cyc_Tcutil_rsubstitute(r,inst,_tmp659);if(
_tmp686 == _tmp659)return e;{struct Cyc_Absyn_Valueof_e_struct _tmpE0C;struct Cyc_Absyn_Valueof_e_struct*
_tmpE0B;return Cyc_Tcutil_copye(e,(void*)((_tmpE0B=_cycalloc(sizeof(*_tmpE0B)),((
_tmpE0B[0]=((_tmpE0C.tag=39,((_tmpE0C.f1=(void*)_tmp686,_tmpE0C)))),_tmpE0B)))));}}
_LL475:;_LL476: {const char*_tmpE0F;void*_tmpE0E;return(_tmpE0E=0,((struct Cyc_Absyn_Exp*(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpE0F="non-type-level-expression in Tcutil::rsubsexp",
_tag_dyneither(_tmpE0F,sizeof(char),46))),_tag_dyneither(_tmpE0E,sizeof(void*),0)));}
_LL458:;}void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*rgn,struct Cyc_List_List*
inst,void*t);void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*rgn,struct Cyc_List_List*
inst,void*t){void*_tmp68B=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*_tmp68C;
struct Cyc_Absyn_AggrInfo _tmp68D;union Cyc_Absyn_AggrInfoU_union _tmp68E;struct Cyc_List_List*
_tmp68F;struct Cyc_Absyn_TunionInfo _tmp690;union Cyc_Absyn_TunionInfoU_union
_tmp691;struct Cyc_List_List*_tmp692;struct Cyc_Core_Opt*_tmp693;struct Cyc_Absyn_TunionFieldInfo
_tmp694;union Cyc_Absyn_TunionFieldInfoU_union _tmp695;struct Cyc_List_List*_tmp696;
struct _tuple2*_tmp697;struct Cyc_List_List*_tmp698;struct Cyc_Absyn_Typedefdecl*
_tmp699;void**_tmp69A;struct Cyc_Absyn_ArrayInfo _tmp69B;void*_tmp69C;struct Cyc_Absyn_Tqual
_tmp69D;struct Cyc_Absyn_Exp*_tmp69E;struct Cyc_Absyn_Conref*_tmp69F;struct Cyc_Position_Segment*
_tmp6A0;struct Cyc_Absyn_PtrInfo _tmp6A1;void*_tmp6A2;struct Cyc_Absyn_Tqual _tmp6A3;
struct Cyc_Absyn_PtrAtts _tmp6A4;void*_tmp6A5;struct Cyc_Absyn_Conref*_tmp6A6;
struct Cyc_Absyn_Conref*_tmp6A7;struct Cyc_Absyn_Conref*_tmp6A8;struct Cyc_Absyn_FnInfo
_tmp6A9;struct Cyc_List_List*_tmp6AA;struct Cyc_Core_Opt*_tmp6AB;void*_tmp6AC;
struct Cyc_List_List*_tmp6AD;int _tmp6AE;struct Cyc_Absyn_VarargInfo*_tmp6AF;struct
Cyc_List_List*_tmp6B0;struct Cyc_List_List*_tmp6B1;struct Cyc_List_List*_tmp6B2;
void*_tmp6B3;struct Cyc_List_List*_tmp6B4;struct Cyc_Core_Opt*_tmp6B5;void*_tmp6B6;
void*_tmp6B7;void*_tmp6B8;void*_tmp6B9;struct Cyc_Absyn_Exp*_tmp6BA;void*_tmp6BB;
void*_tmp6BC;struct Cyc_List_List*_tmp6BD;_LL478: if(_tmp68B <= (void*)4)goto _LL49A;
if(*((int*)_tmp68B)!= 1)goto _LL47A;_tmp68C=((struct Cyc_Absyn_VarType_struct*)
_tmp68B)->f1;_LL479: {struct _handler_cons _tmp6BE;_push_handler(& _tmp6BE);{int
_tmp6C0=0;if(setjmp(_tmp6BE.handler))_tmp6C0=1;if(!_tmp6C0){{void*_tmp6C1=((void*(*)(
int(*cmp)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,
struct Cyc_Absyn_Tvar*x))Cyc_List_assoc_cmp)(Cyc_Absyn_tvar_cmp,inst,_tmp68C);
_npop_handler(0);return _tmp6C1;};_pop_handler();}else{void*_tmp6BF=(void*)
_exn_thrown;void*_tmp6C3=_tmp6BF;_LL4AD: if(_tmp6C3 != Cyc_Core_Not_found)goto
_LL4AF;_LL4AE: return t;_LL4AF:;_LL4B0:(void)_throw(_tmp6C3);_LL4AC:;}}}_LL47A: if(*((
int*)_tmp68B)!= 10)goto _LL47C;_tmp68D=((struct Cyc_Absyn_AggrType_struct*)_tmp68B)->f1;
_tmp68E=_tmp68D.aggr_info;_tmp68F=_tmp68D.targs;_LL47B: {struct Cyc_List_List*
_tmp6C4=Cyc_Tcutil_substs(rgn,inst,_tmp68F);struct Cyc_Absyn_AggrType_struct
_tmpE15;struct Cyc_Absyn_AggrInfo _tmpE14;struct Cyc_Absyn_AggrType_struct*_tmpE13;
return _tmp6C4 == _tmp68F?t:(void*)((_tmpE13=_cycalloc(sizeof(*_tmpE13)),((_tmpE13[
0]=((_tmpE15.tag=10,((_tmpE15.f1=((_tmpE14.aggr_info=_tmp68E,((_tmpE14.targs=
_tmp6C4,_tmpE14)))),_tmpE15)))),_tmpE13))));}_LL47C: if(*((int*)_tmp68B)!= 2)goto
_LL47E;_tmp690=((struct Cyc_Absyn_TunionType_struct*)_tmp68B)->f1;_tmp691=_tmp690.tunion_info;
_tmp692=_tmp690.targs;_tmp693=_tmp690.rgn;_LL47D: {struct Cyc_List_List*_tmp6C8=
Cyc_Tcutil_substs(rgn,inst,_tmp692);struct Cyc_Core_Opt*new_r;if((unsigned int)
_tmp693){void*_tmp6C9=Cyc_Tcutil_rsubstitute(rgn,inst,(void*)_tmp693->v);if(
_tmp6C9 == (void*)_tmp693->v)new_r=_tmp693;else{struct Cyc_Core_Opt*_tmpE16;new_r=((
_tmpE16=_cycalloc(sizeof(*_tmpE16)),((_tmpE16->v=(void*)_tmp6C9,_tmpE16))));}}
else{new_r=_tmp693;}{struct Cyc_Absyn_TunionType_struct _tmpE1C;struct Cyc_Absyn_TunionInfo
_tmpE1B;struct Cyc_Absyn_TunionType_struct*_tmpE1A;return _tmp6C8 == _tmp692  && 
new_r == _tmp693?t:(void*)((_tmpE1A=_cycalloc(sizeof(*_tmpE1A)),((_tmpE1A[0]=((
_tmpE1C.tag=2,((_tmpE1C.f1=((_tmpE1B.tunion_info=_tmp691,((_tmpE1B.targs=_tmp6C8,((
_tmpE1B.rgn=new_r,_tmpE1B)))))),_tmpE1C)))),_tmpE1A))));}}_LL47E: if(*((int*)
_tmp68B)!= 3)goto _LL480;_tmp694=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp68B)->f1;_tmp695=_tmp694.field_info;_tmp696=_tmp694.targs;_LL47F: {struct Cyc_List_List*
_tmp6CE=Cyc_Tcutil_substs(rgn,inst,_tmp696);struct Cyc_Absyn_TunionFieldType_struct
_tmpE22;struct Cyc_Absyn_TunionFieldInfo _tmpE21;struct Cyc_Absyn_TunionFieldType_struct*
_tmpE20;return _tmp6CE == _tmp696?t:(void*)((_tmpE20=_cycalloc(sizeof(*_tmpE20)),((
_tmpE20[0]=((_tmpE22.tag=3,((_tmpE22.f1=((_tmpE21.field_info=_tmp695,((_tmpE21.targs=
_tmp6CE,_tmpE21)))),_tmpE22)))),_tmpE20))));}_LL480: if(*((int*)_tmp68B)!= 16)
goto _LL482;_tmp697=((struct Cyc_Absyn_TypedefType_struct*)_tmp68B)->f1;_tmp698=((
struct Cyc_Absyn_TypedefType_struct*)_tmp68B)->f2;_tmp699=((struct Cyc_Absyn_TypedefType_struct*)
_tmp68B)->f3;_tmp69A=((struct Cyc_Absyn_TypedefType_struct*)_tmp68B)->f4;_LL481: {
struct Cyc_List_List*_tmp6D2=Cyc_Tcutil_substs(rgn,inst,_tmp698);struct Cyc_Absyn_TypedefType_struct
_tmpE25;struct Cyc_Absyn_TypedefType_struct*_tmpE24;return _tmp6D2 == _tmp698?t:(
void*)((_tmpE24=_cycalloc(sizeof(*_tmpE24)),((_tmpE24[0]=((_tmpE25.tag=16,((
_tmpE25.f1=_tmp697,((_tmpE25.f2=_tmp6D2,((_tmpE25.f3=_tmp699,((_tmpE25.f4=
_tmp69A,_tmpE25)))))))))),_tmpE24))));}_LL482: if(*((int*)_tmp68B)!= 7)goto _LL484;
_tmp69B=((struct Cyc_Absyn_ArrayType_struct*)_tmp68B)->f1;_tmp69C=(void*)_tmp69B.elt_type;
_tmp69D=_tmp69B.tq;_tmp69E=_tmp69B.num_elts;_tmp69F=_tmp69B.zero_term;_tmp6A0=
_tmp69B.zt_loc;_LL483: {void*_tmp6D5=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp69C);
struct Cyc_Absyn_ArrayType_struct _tmpE2B;struct Cyc_Absyn_ArrayInfo _tmpE2A;struct
Cyc_Absyn_ArrayType_struct*_tmpE29;return _tmp6D5 == _tmp69C?t:(void*)((_tmpE29=
_cycalloc(sizeof(*_tmpE29)),((_tmpE29[0]=((_tmpE2B.tag=7,((_tmpE2B.f1=((_tmpE2A.elt_type=(
void*)_tmp6D5,((_tmpE2A.tq=_tmp69D,((_tmpE2A.num_elts=_tmp69E,((_tmpE2A.zero_term=
_tmp69F,((_tmpE2A.zt_loc=_tmp6A0,_tmpE2A)))))))))),_tmpE2B)))),_tmpE29))));}
_LL484: if(*((int*)_tmp68B)!= 4)goto _LL486;_tmp6A1=((struct Cyc_Absyn_PointerType_struct*)
_tmp68B)->f1;_tmp6A2=(void*)_tmp6A1.elt_typ;_tmp6A3=_tmp6A1.elt_tq;_tmp6A4=
_tmp6A1.ptr_atts;_tmp6A5=(void*)_tmp6A4.rgn;_tmp6A6=_tmp6A4.nullable;_tmp6A7=
_tmp6A4.bounds;_tmp6A8=_tmp6A4.zero_term;_LL485: {void*_tmp6D9=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmp6A2);void*_tmp6DA=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6A5);struct
Cyc_Absyn_Conref*_tmp6DB=_tmp6A7;{union Cyc_Absyn_Constraint_union _tmp6DC=(Cyc_Absyn_compress_conref(
_tmp6A7))->v;void*_tmp6DD;struct Cyc_Absyn_Exp*_tmp6DE;_LL4B2: if((_tmp6DC.Eq_constr).tag
!= 0)goto _LL4B4;_tmp6DD=(_tmp6DC.Eq_constr).f1;if(_tmp6DD <= (void*)1)goto _LL4B4;
if(*((int*)_tmp6DD)!= 0)goto _LL4B4;_tmp6DE=((struct Cyc_Absyn_Upper_b_struct*)
_tmp6DD)->f1;_LL4B3: {struct Cyc_Absyn_Exp*_tmp6DF=Cyc_Tcutil_rsubsexp(rgn,inst,
_tmp6DE);if(_tmp6DF != _tmp6DE){struct Cyc_Absyn_Upper_b_struct _tmpE2E;struct Cyc_Absyn_Upper_b_struct*
_tmpE2D;_tmp6DB=Cyc_Absyn_new_conref((void*)((_tmpE2D=_cycalloc(sizeof(*_tmpE2D)),((
_tmpE2D[0]=((_tmpE2E.tag=0,((_tmpE2E.f1=_tmp6DF,_tmpE2E)))),_tmpE2D)))));}goto
_LL4B1;}_LL4B4:;_LL4B5: goto _LL4B1;_LL4B1:;}if((_tmp6D9 == _tmp6A2  && _tmp6DA == 
_tmp6A5) && _tmp6DB == _tmp6A7)return t;{struct Cyc_Absyn_PointerType_struct _tmpE38;
struct Cyc_Absyn_PtrAtts _tmpE37;struct Cyc_Absyn_PtrInfo _tmpE36;struct Cyc_Absyn_PointerType_struct*
_tmpE35;return(void*)((_tmpE35=_cycalloc(sizeof(*_tmpE35)),((_tmpE35[0]=((
_tmpE38.tag=4,((_tmpE38.f1=((_tmpE36.elt_typ=(void*)_tmp6D9,((_tmpE36.elt_tq=
_tmp6A3,((_tmpE36.ptr_atts=((_tmpE37.rgn=(void*)_tmp6DA,((_tmpE37.nullable=
_tmp6A6,((_tmpE37.bounds=_tmp6DB,((_tmpE37.zero_term=_tmp6A8,((_tmpE37.ptrloc=0,
_tmpE37)))))))))),_tmpE36)))))),_tmpE38)))),_tmpE35))));}}_LL486: if(*((int*)
_tmp68B)!= 8)goto _LL488;_tmp6A9=((struct Cyc_Absyn_FnType_struct*)_tmp68B)->f1;
_tmp6AA=_tmp6A9.tvars;_tmp6AB=_tmp6A9.effect;_tmp6AC=(void*)_tmp6A9.ret_typ;
_tmp6AD=_tmp6A9.args;_tmp6AE=_tmp6A9.c_varargs;_tmp6AF=_tmp6A9.cyc_varargs;
_tmp6B0=_tmp6A9.rgn_po;_tmp6B1=_tmp6A9.attributes;_LL487:{struct Cyc_List_List*
_tmp6E6=_tmp6AA;for(0;_tmp6E6 != 0;_tmp6E6=_tmp6E6->tl){struct _tuple8*_tmpE42;
struct Cyc_Absyn_VarType_struct _tmpE41;struct Cyc_Absyn_VarType_struct*_tmpE40;
struct Cyc_List_List*_tmpE3F;inst=((_tmpE3F=_region_malloc(rgn,sizeof(*_tmpE3F)),((
_tmpE3F->hd=((_tmpE42=_region_malloc(rgn,sizeof(*_tmpE42)),((_tmpE42->f1=(struct
Cyc_Absyn_Tvar*)_tmp6E6->hd,((_tmpE42->f2=(void*)((_tmpE40=_cycalloc(sizeof(*
_tmpE40)),((_tmpE40[0]=((_tmpE41.tag=1,((_tmpE41.f1=(struct Cyc_Absyn_Tvar*)
_tmp6E6->hd,_tmpE41)))),_tmpE40)))),_tmpE42)))))),((_tmpE3F->tl=inst,_tmpE3F))))));}}{
struct Cyc_List_List*_tmp6EC;struct Cyc_List_List*_tmp6ED;struct _tuple1 _tmp6EB=((
struct _tuple1(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x))Cyc_List_rsplit)(rgn,rgn,((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct _tuple17*(*f)(struct _RegionHandle*,struct _tuple3*),struct _RegionHandle*env,
struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_substitute_f1,rgn,_tmp6AD));
_tmp6EC=_tmp6EB.f1;_tmp6ED=_tmp6EB.f2;{struct Cyc_List_List*_tmp6EE=Cyc_Tcutil_substs(
rgn,inst,_tmp6ED);struct Cyc_List_List*_tmp6EF=((struct Cyc_List_List*(*)(struct
_tuple3*(*f)(struct _tuple17*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_substitute_f2,((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmp6EC,_tmp6EE));struct Cyc_Core_Opt*
eff2;if(_tmp6AB == 0)eff2=0;else{void*_tmp6F0=Cyc_Tcutil_rsubstitute(rgn,inst,(
void*)_tmp6AB->v);if(_tmp6F0 == (void*)_tmp6AB->v)eff2=_tmp6AB;else{struct Cyc_Core_Opt*
_tmpE43;eff2=((_tmpE43=_cycalloc(sizeof(*_tmpE43)),((_tmpE43->v=(void*)_tmp6F0,
_tmpE43))));}}{struct Cyc_Absyn_VarargInfo*cyc_varargs2;if(_tmp6AF == 0)
cyc_varargs2=0;else{struct Cyc_Core_Opt*_tmp6F3;struct Cyc_Absyn_Tqual _tmp6F4;void*
_tmp6F5;int _tmp6F6;struct Cyc_Absyn_VarargInfo _tmp6F2=*_tmp6AF;_tmp6F3=_tmp6F2.name;
_tmp6F4=_tmp6F2.tq;_tmp6F5=(void*)_tmp6F2.type;_tmp6F6=_tmp6F2.inject;{void*
_tmp6F7=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6F5);if(_tmp6F7 == _tmp6F5)
cyc_varargs2=_tmp6AF;else{struct Cyc_Absyn_VarargInfo*_tmpE44;cyc_varargs2=((
_tmpE44=_cycalloc(sizeof(*_tmpE44)),((_tmpE44->name=_tmp6F3,((_tmpE44->tq=
_tmp6F4,((_tmpE44->type=(void*)_tmp6F7,((_tmpE44->inject=_tmp6F6,_tmpE44))))))))));}}}{
struct Cyc_List_List*rgn_po2;struct Cyc_List_List*_tmp6FA;struct Cyc_List_List*
_tmp6FB;struct _tuple1 _tmp6F9=Cyc_List_rsplit(rgn,rgn,_tmp6B0);_tmp6FA=_tmp6F9.f1;
_tmp6FB=_tmp6F9.f2;{struct Cyc_List_List*_tmp6FC=Cyc_Tcutil_substs(rgn,inst,
_tmp6FA);struct Cyc_List_List*_tmp6FD=Cyc_Tcutil_substs(rgn,inst,_tmp6FB);if(
_tmp6FC == _tmp6FA  && _tmp6FD == _tmp6FB)rgn_po2=_tmp6B0;else{rgn_po2=Cyc_List_zip(
_tmp6FC,_tmp6FD);}{struct Cyc_Absyn_FnType_struct _tmpE4A;struct Cyc_Absyn_FnInfo
_tmpE49;struct Cyc_Absyn_FnType_struct*_tmpE48;return(void*)((_tmpE48=_cycalloc(
sizeof(*_tmpE48)),((_tmpE48[0]=((_tmpE4A.tag=8,((_tmpE4A.f1=((_tmpE49.tvars=
_tmp6AA,((_tmpE49.effect=eff2,((_tmpE49.ret_typ=(void*)Cyc_Tcutil_rsubstitute(
rgn,inst,_tmp6AC),((_tmpE49.args=_tmp6EF,((_tmpE49.c_varargs=_tmp6AE,((_tmpE49.cyc_varargs=
cyc_varargs2,((_tmpE49.rgn_po=rgn_po2,((_tmpE49.attributes=_tmp6B1,_tmpE49)))))))))))))))),
_tmpE4A)))),_tmpE48))));}}}}}}_LL488: if(*((int*)_tmp68B)!= 9)goto _LL48A;_tmp6B2=((
struct Cyc_Absyn_TupleType_struct*)_tmp68B)->f1;_LL489: {struct Cyc_List_List*
_tmp701=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _tuple5*),
struct Cyc_List_List*x))Cyc_List_rmap)(rgn,Cyc_Tcutil_snd_tqt,_tmp6B2);struct Cyc_List_List*
_tmp702=Cyc_Tcutil_substs(rgn,inst,_tmp701);if(_tmp702 == _tmp701)return t;{struct
Cyc_List_List*_tmp703=((struct Cyc_List_List*(*)(struct _tuple5*(*f)(struct _tuple5*,
void*),struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_map2_tq,
_tmp6B2,_tmp702);struct Cyc_Absyn_TupleType_struct _tmpE4D;struct Cyc_Absyn_TupleType_struct*
_tmpE4C;return(void*)((_tmpE4C=_cycalloc(sizeof(*_tmpE4C)),((_tmpE4C[0]=((
_tmpE4D.tag=9,((_tmpE4D.f1=_tmp703,_tmpE4D)))),_tmpE4C))));}}_LL48A: if(*((int*)
_tmp68B)!= 11)goto _LL48C;_tmp6B3=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp68B)->f1;_tmp6B4=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp68B)->f2;_LL48B: {
struct Cyc_List_List*_tmp706=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*
f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_rmap)(rgn,Cyc_Tcutil_field_type,
_tmp6B4);struct Cyc_List_List*_tmp707=Cyc_Tcutil_substs(rgn,inst,_tmp706);if(
_tmp707 == _tmp706)return t;{struct Cyc_List_List*_tmp708=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*,void*),struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_zip_field_type,_tmp6B4,
_tmp707);struct Cyc_Absyn_AnonAggrType_struct _tmpE50;struct Cyc_Absyn_AnonAggrType_struct*
_tmpE4F;return(void*)((_tmpE4F=_cycalloc(sizeof(*_tmpE4F)),((_tmpE4F[0]=((
_tmpE50.tag=11,((_tmpE50.f1=(void*)_tmp6B3,((_tmpE50.f2=_tmp708,_tmpE50)))))),
_tmpE4F))));}}_LL48C: if(*((int*)_tmp68B)!= 0)goto _LL48E;_tmp6B5=((struct Cyc_Absyn_Evar_struct*)
_tmp68B)->f2;_LL48D: if(_tmp6B5 != 0)return Cyc_Tcutil_rsubstitute(rgn,inst,(void*)
_tmp6B5->v);else{return t;}_LL48E: if(*((int*)_tmp68B)!= 14)goto _LL490;_tmp6B6=(
void*)((struct Cyc_Absyn_RgnHandleType_struct*)_tmp68B)->f1;_LL48F: {void*_tmp70B=
Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6B6);struct Cyc_Absyn_RgnHandleType_struct
_tmpE53;struct Cyc_Absyn_RgnHandleType_struct*_tmpE52;return _tmp70B == _tmp6B6?t:(
void*)((_tmpE52=_cycalloc(sizeof(*_tmpE52)),((_tmpE52[0]=((_tmpE53.tag=14,((
_tmpE53.f1=(void*)_tmp70B,_tmpE53)))),_tmpE52))));}_LL490: if(*((int*)_tmp68B)!= 
15)goto _LL492;_tmp6B7=(void*)((struct Cyc_Absyn_DynRgnType_struct*)_tmp68B)->f1;
_tmp6B8=(void*)((struct Cyc_Absyn_DynRgnType_struct*)_tmp68B)->f2;_LL491: {void*
_tmp70E=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6B7);void*_tmp70F=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmp6B8);struct Cyc_Absyn_DynRgnType_struct _tmpE56;struct Cyc_Absyn_DynRgnType_struct*
_tmpE55;return _tmp70E == _tmp6B7  && _tmp70F == _tmp6B8?t:(void*)((_tmpE55=
_cycalloc(sizeof(*_tmpE55)),((_tmpE55[0]=((_tmpE56.tag=15,((_tmpE56.f1=(void*)
_tmp70E,((_tmpE56.f2=(void*)_tmp70F,_tmpE56)))))),_tmpE55))));}_LL492: if(*((int*)
_tmp68B)!= 18)goto _LL494;_tmp6B9=(void*)((struct Cyc_Absyn_TagType_struct*)
_tmp68B)->f1;_LL493: {void*_tmp712=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6B9);
struct Cyc_Absyn_TagType_struct _tmpE59;struct Cyc_Absyn_TagType_struct*_tmpE58;
return _tmp712 == _tmp6B9?t:(void*)((_tmpE58=_cycalloc(sizeof(*_tmpE58)),((_tmpE58[
0]=((_tmpE59.tag=18,((_tmpE59.f1=(void*)_tmp712,_tmpE59)))),_tmpE58))));}_LL494:
if(*((int*)_tmp68B)!= 17)goto _LL496;_tmp6BA=((struct Cyc_Absyn_ValueofType_struct*)
_tmp68B)->f1;_LL495: {struct Cyc_Absyn_Exp*_tmp715=Cyc_Tcutil_rsubsexp(rgn,inst,
_tmp6BA);struct Cyc_Absyn_ValueofType_struct _tmpE5C;struct Cyc_Absyn_ValueofType_struct*
_tmpE5B;return _tmp715 == _tmp6BA?t:(void*)((_tmpE5B=_cycalloc(sizeof(*_tmpE5B)),((
_tmpE5B[0]=((_tmpE5C.tag=17,((_tmpE5C.f1=_tmp715,_tmpE5C)))),_tmpE5B))));}_LL496:
if(*((int*)_tmp68B)!= 12)goto _LL498;_LL497: goto _LL499;_LL498: if(*((int*)_tmp68B)
!= 13)goto _LL49A;_LL499: goto _LL49B;_LL49A: if((int)_tmp68B != 0)goto _LL49C;_LL49B:
goto _LL49D;_LL49C: if(_tmp68B <= (void*)4)goto _LL49E;if(*((int*)_tmp68B)!= 5)goto
_LL49E;_LL49D: goto _LL49F;_LL49E: if((int)_tmp68B != 1)goto _LL4A0;_LL49F: goto _LL4A1;
_LL4A0: if(_tmp68B <= (void*)4)goto _LL4A2;if(*((int*)_tmp68B)!= 6)goto _LL4A2;
_LL4A1: goto _LL4A3;_LL4A2: if((int)_tmp68B != 3)goto _LL4A4;_LL4A3: goto _LL4A5;_LL4A4:
if((int)_tmp68B != 2)goto _LL4A6;_LL4A5: return t;_LL4A6: if(_tmp68B <= (void*)4)goto
_LL4A8;if(*((int*)_tmp68B)!= 21)goto _LL4A8;_tmp6BB=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp68B)->f1;_LL4A7: {void*_tmp718=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6BB);
struct Cyc_Absyn_RgnsEff_struct _tmpE5F;struct Cyc_Absyn_RgnsEff_struct*_tmpE5E;
return _tmp718 == _tmp6BB?t:(void*)((_tmpE5E=_cycalloc(sizeof(*_tmpE5E)),((_tmpE5E[
0]=((_tmpE5F.tag=21,((_tmpE5F.f1=(void*)_tmp718,_tmpE5F)))),_tmpE5E))));}_LL4A8:
if(_tmp68B <= (void*)4)goto _LL4AA;if(*((int*)_tmp68B)!= 19)goto _LL4AA;_tmp6BC=(
void*)((struct Cyc_Absyn_AccessEff_struct*)_tmp68B)->f1;_LL4A9: {void*_tmp71B=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmp6BC);struct Cyc_Absyn_AccessEff_struct _tmpE62;struct Cyc_Absyn_AccessEff_struct*
_tmpE61;return _tmp71B == _tmp6BC?t:(void*)((_tmpE61=_cycalloc(sizeof(*_tmpE61)),((
_tmpE61[0]=((_tmpE62.tag=19,((_tmpE62.f1=(void*)_tmp71B,_tmpE62)))),_tmpE61))));}
_LL4AA: if(_tmp68B <= (void*)4)goto _LL477;if(*((int*)_tmp68B)!= 20)goto _LL477;
_tmp6BD=((struct Cyc_Absyn_JoinEff_struct*)_tmp68B)->f1;_LL4AB: {struct Cyc_List_List*
_tmp71E=Cyc_Tcutil_substs(rgn,inst,_tmp6BD);struct Cyc_Absyn_JoinEff_struct
_tmpE65;struct Cyc_Absyn_JoinEff_struct*_tmpE64;return _tmp71E == _tmp6BD?t:(void*)((
_tmpE64=_cycalloc(sizeof(*_tmpE64)),((_tmpE64[0]=((_tmpE65.tag=20,((_tmpE65.f1=
_tmp71E,_tmpE65)))),_tmpE64))));}_LL477:;}static struct Cyc_List_List*Cyc_Tcutil_substs(
struct _RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*ts);static
struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*rgn,struct Cyc_List_List*
inst,struct Cyc_List_List*ts){if(ts == 0)return 0;{void*_tmp721=(void*)ts->hd;
struct Cyc_List_List*_tmp722=ts->tl;void*_tmp723=Cyc_Tcutil_rsubstitute(rgn,inst,
_tmp721);struct Cyc_List_List*_tmp724=Cyc_Tcutil_substs(rgn,inst,_tmp722);if(
_tmp721 == _tmp723  && _tmp722 == _tmp724)return ts;{struct Cyc_List_List*_tmpE66;
return(struct Cyc_List_List*)((struct Cyc_List_List*)((_tmpE66=_cycalloc(sizeof(*
_tmpE66)),((_tmpE66->hd=(void*)_tmp723,((_tmpE66->tl=_tmp724,_tmpE66)))))));}}}
void*Cyc_Tcutil_substitute(struct Cyc_List_List*inst,void*t);extern void*Cyc_Tcutil_substitute(
struct Cyc_List_List*inst,void*t){return Cyc_Tcutil_rsubstitute(Cyc_Core_heap_region,
inst,t);}struct _tuple8*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*s,struct Cyc_Absyn_Tvar*
tv);struct _tuple8*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*s,struct Cyc_Absyn_Tvar*
tv){struct Cyc_Core_Opt*_tmp726=Cyc_Tcutil_kind_to_opt(Cyc_Tcutil_tvar_kind(tv));
struct Cyc_Core_Opt*_tmpE69;struct _tuple8*_tmpE68;return(_tmpE68=_cycalloc(
sizeof(*_tmpE68)),((_tmpE68->f1=tv,((_tmpE68->f2=Cyc_Absyn_new_evar(_tmp726,((
_tmpE69=_cycalloc(sizeof(*_tmpE69)),((_tmpE69->v=s,_tmpE69))))),_tmpE68)))));}
struct _tuple8*Cyc_Tcutil_r_make_inst_var(struct _tuple9*env,struct Cyc_Absyn_Tvar*
tv);struct _tuple8*Cyc_Tcutil_r_make_inst_var(struct _tuple9*env,struct Cyc_Absyn_Tvar*
tv){struct _tuple9 _tmp72A;struct Cyc_List_List*_tmp72B;struct _RegionHandle*_tmp72C;
struct _tuple9*_tmp729=env;_tmp72A=*_tmp729;_tmp72B=_tmp72A.f1;_tmp72C=_tmp72A.f2;{
struct Cyc_Core_Opt*_tmp72D=Cyc_Tcutil_kind_to_opt(Cyc_Tcutil_tvar_kind(tv));
struct Cyc_Core_Opt*_tmpE6C;struct _tuple8*_tmpE6B;return(_tmpE6B=_region_malloc(
_tmp72C,sizeof(*_tmpE6B)),((_tmpE6B->f1=tv,((_tmpE6B->f2=Cyc_Absyn_new_evar(
_tmp72D,((_tmpE6C=_cycalloc(sizeof(*_tmpE6C)),((_tmpE6C->v=_tmp72B,_tmpE6C))))),
_tmpE6B)))));}}static struct Cyc_List_List*Cyc_Tcutil_add_free_tvar(struct Cyc_Position_Segment*
loc,struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv);static struct Cyc_List_List*
Cyc_Tcutil_add_free_tvar(struct Cyc_Position_Segment*loc,struct Cyc_List_List*tvs,
struct Cyc_Absyn_Tvar*tv){{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
if(Cyc_strptrcmp(((struct Cyc_Absyn_Tvar*)tvs2->hd)->name,tv->name)== 0){void*k1=(
void*)((struct Cyc_Absyn_Tvar*)tvs2->hd)->kind;void*k2=(void*)tv->kind;if(!Cyc_Tcutil_constrain_kinds(
k1,k2)){const char*_tmpE72;void*_tmpE71[3];struct Cyc_String_pa_struct _tmpE70;
struct Cyc_String_pa_struct _tmpE6F;struct Cyc_String_pa_struct _tmpE6E;(_tmpE6E.tag=
0,((_tmpE6E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kindbound2string(
k2)),((_tmpE6F.tag=0,((_tmpE6F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kindbound2string(k1)),((_tmpE70.tag=0,((_tmpE70.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*tv->name),((_tmpE71[0]=& _tmpE70,((_tmpE71[
1]=& _tmpE6F,((_tmpE71[2]=& _tmpE6E,Cyc_Tcutil_terr(loc,((_tmpE72="type variable %s is used with inconsistent kinds %s and %s",
_tag_dyneither(_tmpE72,sizeof(char),59))),_tag_dyneither(_tmpE71,sizeof(void*),3)))))))))))))))))));}
if(tv->identity == - 1)tv->identity=((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity;
else{if(tv->identity != ((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity){const char*
_tmpE75;void*_tmpE74;(_tmpE74=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpE75="same type variable has different identity!",
_tag_dyneither(_tmpE75,sizeof(char),43))),_tag_dyneither(_tmpE74,sizeof(void*),0)));}}
return tvs;}}}tv->identity=Cyc_Tcutil_new_tvar_id();{struct Cyc_List_List*_tmpE76;
return(_tmpE76=_cycalloc(sizeof(*_tmpE76)),((_tmpE76->hd=tv,((_tmpE76->tl=tvs,
_tmpE76)))));}}static struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar(struct Cyc_List_List*
tvs,struct Cyc_Absyn_Tvar*tv);static struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar(
struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){if(tv->identity == - 1){const char*
_tmpE79;void*_tmpE78;(_tmpE78=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpE79="fast_add_free_tvar: bad identity in tv",
_tag_dyneither(_tmpE79,sizeof(char),39))),_tag_dyneither(_tmpE78,sizeof(void*),0)));}{
struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){struct Cyc_Absyn_Tvar*
_tmp73A=(struct Cyc_Absyn_Tvar*)tvs2->hd;if(_tmp73A->identity == - 1){const char*
_tmpE7C;void*_tmpE7B;(_tmpE7B=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpE7C="fast_add_free_tvar: bad identity in tvs2",
_tag_dyneither(_tmpE7C,sizeof(char),41))),_tag_dyneither(_tmpE7B,sizeof(void*),0)));}
if(_tmp73A->identity == tv->identity)return tvs;}}{struct Cyc_List_List*_tmpE7D;
return(_tmpE7D=_cycalloc(sizeof(*_tmpE7D)),((_tmpE7D->hd=tv,((_tmpE7D->tl=tvs,
_tmpE7D)))));}}struct _tuple18{struct Cyc_Absyn_Tvar*f1;int f2;};static struct Cyc_List_List*
Cyc_Tcutil_fast_add_free_tvar_bool(struct _RegionHandle*r,struct Cyc_List_List*tvs,
struct Cyc_Absyn_Tvar*tv,int b);static struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar_bool(
struct _RegionHandle*r,struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv,int b){if(
tv->identity == - 1){const char*_tmpE80;void*_tmpE7F;(_tmpE7F=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpE80="fast_add_free_tvar_bool: bad identity in tv",
_tag_dyneither(_tmpE80,sizeof(char),44))),_tag_dyneither(_tmpE7F,sizeof(void*),0)));}{
struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){struct _tuple18 _tmp741;
struct Cyc_Absyn_Tvar*_tmp742;int _tmp743;int*_tmp744;struct _tuple18*_tmp740=(
struct _tuple18*)tvs2->hd;_tmp741=*_tmp740;_tmp742=_tmp741.f1;_tmp743=_tmp741.f2;
_tmp744=(int*)&(*_tmp740).f2;if(_tmp742->identity == - 1){const char*_tmpE83;void*
_tmpE82;(_tmpE82=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmpE83="fast_add_free_tvar_bool: bad identity in tvs2",_tag_dyneither(_tmpE83,
sizeof(char),46))),_tag_dyneither(_tmpE82,sizeof(void*),0)));}if(_tmp742->identity
== tv->identity){*_tmp744=*_tmp744  || b;return tvs;}}}{struct _tuple18*_tmpE86;
struct Cyc_List_List*_tmpE85;return(_tmpE85=_region_malloc(r,sizeof(*_tmpE85)),((
_tmpE85->hd=((_tmpE86=_region_malloc(r,sizeof(*_tmpE86)),((_tmpE86->f1=tv,((
_tmpE86->f2=b,_tmpE86)))))),((_tmpE85->tl=tvs,_tmpE85)))));}}static struct Cyc_List_List*
Cyc_Tcutil_add_bound_tvar(struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv);
static struct Cyc_List_List*Cyc_Tcutil_add_bound_tvar(struct Cyc_List_List*tvs,
struct Cyc_Absyn_Tvar*tv){if(tv->identity == - 1){const char*_tmpE8A;void*_tmpE89[1];
struct Cyc_String_pa_struct _tmpE88;(_tmpE88.tag=0,((_tmpE88.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_tvar2string(tv)),((_tmpE89[0]=&
_tmpE88,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmpE8A="bound tvar id for %s is NULL",_tag_dyneither(_tmpE8A,sizeof(char),29))),
_tag_dyneither(_tmpE89,sizeof(void*),1)))))));}{struct Cyc_List_List*_tmpE8B;
return(_tmpE8B=_cycalloc(sizeof(*_tmpE8B)),((_tmpE8B->hd=tv,((_tmpE8B->tl=tvs,
_tmpE8B)))));}}struct _tuple19{void*f1;int f2;};static struct Cyc_List_List*Cyc_Tcutil_add_free_evar(
struct _RegionHandle*r,struct Cyc_List_List*es,void*e,int b);static struct Cyc_List_List*
Cyc_Tcutil_add_free_evar(struct _RegionHandle*r,struct Cyc_List_List*es,void*e,int
b){void*_tmp74D=Cyc_Tcutil_compress(e);int _tmp74E;_LL4B7: if(_tmp74D <= (void*)4)
goto _LL4B9;if(*((int*)_tmp74D)!= 0)goto _LL4B9;_tmp74E=((struct Cyc_Absyn_Evar_struct*)
_tmp74D)->f3;_LL4B8:{struct Cyc_List_List*es2=es;for(0;es2 != 0;es2=es2->tl){
struct _tuple19 _tmp750;void*_tmp751;int _tmp752;int*_tmp753;struct _tuple19*_tmp74F=(
struct _tuple19*)es2->hd;_tmp750=*_tmp74F;_tmp751=_tmp750.f1;_tmp752=_tmp750.f2;
_tmp753=(int*)&(*_tmp74F).f2;{void*_tmp754=Cyc_Tcutil_compress(_tmp751);int
_tmp755;_LL4BC: if(_tmp754 <= (void*)4)goto _LL4BE;if(*((int*)_tmp754)!= 0)goto
_LL4BE;_tmp755=((struct Cyc_Absyn_Evar_struct*)_tmp754)->f3;_LL4BD: if(_tmp74E == 
_tmp755){if(b != *_tmp753)*_tmp753=1;return es;}goto _LL4BB;_LL4BE:;_LL4BF: goto
_LL4BB;_LL4BB:;}}}{struct _tuple19*_tmpE8E;struct Cyc_List_List*_tmpE8D;return(
_tmpE8D=_region_malloc(r,sizeof(*_tmpE8D)),((_tmpE8D->hd=((_tmpE8E=
_region_malloc(r,sizeof(*_tmpE8E)),((_tmpE8E->f1=e,((_tmpE8E->f2=b,_tmpE8E)))))),((
_tmpE8D->tl=es,_tmpE8D)))));}_LL4B9:;_LL4BA: return es;_LL4B6:;}static struct Cyc_List_List*
Cyc_Tcutil_remove_bound_tvars(struct _RegionHandle*rgn,struct Cyc_List_List*tvs,
struct Cyc_List_List*btvs);static struct Cyc_List_List*Cyc_Tcutil_remove_bound_tvars(
struct _RegionHandle*rgn,struct Cyc_List_List*tvs,struct Cyc_List_List*btvs){struct
Cyc_List_List*r=0;for(0;tvs != 0;tvs=tvs->tl){int present=0;{struct Cyc_List_List*b=
btvs;for(0;b != 0;b=b->tl){if(((struct Cyc_Absyn_Tvar*)tvs->hd)->identity == ((
struct Cyc_Absyn_Tvar*)b->hd)->identity){present=1;break;}}}if(!present){struct
Cyc_List_List*_tmpE8F;r=((_tmpE8F=_region_malloc(rgn,sizeof(*_tmpE8F)),((_tmpE8F->hd=(
struct Cyc_Absyn_Tvar*)tvs->hd,((_tmpE8F->tl=r,_tmpE8F))))));}}r=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(r);return r;}static struct Cyc_List_List*
Cyc_Tcutil_remove_bound_tvars_bool(struct _RegionHandle*r,struct Cyc_List_List*tvs,
struct Cyc_List_List*btvs);static struct Cyc_List_List*Cyc_Tcutil_remove_bound_tvars_bool(
struct _RegionHandle*r,struct Cyc_List_List*tvs,struct Cyc_List_List*btvs){struct
Cyc_List_List*res=0;for(0;tvs != 0;tvs=tvs->tl){struct Cyc_Absyn_Tvar*_tmp75A;int
_tmp75B;struct _tuple18 _tmp759=*((struct _tuple18*)tvs->hd);_tmp75A=_tmp759.f1;
_tmp75B=_tmp759.f2;{int present=0;{struct Cyc_List_List*b=btvs;for(0;b != 0;b=b->tl){
if(_tmp75A->identity == ((struct Cyc_Absyn_Tvar*)b->hd)->identity){present=1;
break;}}}if(!present){struct Cyc_List_List*_tmpE90;res=((_tmpE90=_region_malloc(r,
sizeof(*_tmpE90)),((_tmpE90->hd=(struct _tuple18*)tvs->hd,((_tmpE90->tl=res,
_tmpE90))))));}}}res=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
res);return res;}void Cyc_Tcutil_check_bitfield(struct Cyc_Position_Segment*loc,
struct Cyc_Tcenv_Tenv*te,void*field_typ,struct Cyc_Absyn_Exp*width,struct
_dyneither_ptr*fn);void Cyc_Tcutil_check_bitfield(struct Cyc_Position_Segment*loc,
struct Cyc_Tcenv_Tenv*te,void*field_typ,struct Cyc_Absyn_Exp*width,struct
_dyneither_ptr*fn){if(width != 0){unsigned int w=0;if(!Cyc_Tcutil_is_const_exp(te,(
struct Cyc_Absyn_Exp*)width)){const char*_tmpE94;void*_tmpE93[1];struct Cyc_String_pa_struct
_tmpE92;(_tmpE92.tag=0,((_tmpE92.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*
fn),((_tmpE93[0]=& _tmpE92,Cyc_Tcutil_terr(loc,((_tmpE94="bitfield %s does not have constant width",
_tag_dyneither(_tmpE94,sizeof(char),41))),_tag_dyneither(_tmpE93,sizeof(void*),1)))))));}
else{unsigned int _tmp761;int _tmp762;struct _tuple7 _tmp760=Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)width);_tmp761=_tmp760.f1;_tmp762=_tmp760.f2;if(!_tmp762){
const char*_tmpE97;void*_tmpE96;(_tmpE96=0,Cyc_Tcutil_terr(loc,((_tmpE97="bitfield width cannot use sizeof or offsetof",
_tag_dyneither(_tmpE97,sizeof(char),45))),_tag_dyneither(_tmpE96,sizeof(void*),0)));}
w=_tmp761;}{void*_tmp765=Cyc_Tcutil_compress(field_typ);void*_tmp766;_LL4C1: if(
_tmp765 <= (void*)4)goto _LL4C3;if(*((int*)_tmp765)!= 5)goto _LL4C3;_tmp766=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp765)->f2;_LL4C2:{void*_tmp767=_tmp766;_LL4C6:
if((int)_tmp767 != 0)goto _LL4C8;_LL4C7: if(w > 8){const char*_tmpE9A;void*_tmpE99;(
_tmpE99=0,Cyc_Tcutil_terr(loc,((_tmpE9A="bitfield larger than type",
_tag_dyneither(_tmpE9A,sizeof(char),26))),_tag_dyneither(_tmpE99,sizeof(void*),0)));}
goto _LL4C5;_LL4C8: if((int)_tmp767 != 1)goto _LL4CA;_LL4C9: if(w > 16){const char*
_tmpE9D;void*_tmpE9C;(_tmpE9C=0,Cyc_Tcutil_terr(loc,((_tmpE9D="bitfield larger than type",
_tag_dyneither(_tmpE9D,sizeof(char),26))),_tag_dyneither(_tmpE9C,sizeof(void*),0)));}
goto _LL4C5;_LL4CA: if((int)_tmp767 != 3)goto _LL4CC;_LL4CB: goto _LL4CD;_LL4CC: if((
int)_tmp767 != 2)goto _LL4CE;_LL4CD: if(w > 32){const char*_tmpEA0;void*_tmpE9F;(
_tmpE9F=0,Cyc_Tcutil_terr(loc,((_tmpEA0="bitfield larger than type",
_tag_dyneither(_tmpEA0,sizeof(char),26))),_tag_dyneither(_tmpE9F,sizeof(void*),0)));}
goto _LL4C5;_LL4CE: if((int)_tmp767 != 4)goto _LL4C5;_LL4CF: if(w > 64){const char*
_tmpEA3;void*_tmpEA2;(_tmpEA2=0,Cyc_Tcutil_terr(loc,((_tmpEA3="bitfield larger than type",
_tag_dyneither(_tmpEA3,sizeof(char),26))),_tag_dyneither(_tmpEA2,sizeof(void*),0)));}
goto _LL4C5;_LL4C5:;}goto _LL4C0;_LL4C3:;_LL4C4:{const char*_tmpEA8;void*_tmpEA7[2];
struct Cyc_String_pa_struct _tmpEA6;struct Cyc_String_pa_struct _tmpEA5;(_tmpEA5.tag=
0,((_tmpEA5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
field_typ)),((_tmpEA6.tag=0,((_tmpEA6.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*fn),((_tmpEA7[0]=& _tmpEA6,((_tmpEA7[1]=& _tmpEA5,Cyc_Tcutil_terr(
loc,((_tmpEA8="bitfield %s must have integral type but has type %s",
_tag_dyneither(_tmpEA8,sizeof(char),52))),_tag_dyneither(_tmpEA7,sizeof(void*),2)))))))))))));}
goto _LL4C0;_LL4C0:;}}}static void Cyc_Tcutil_check_field_atts(struct Cyc_Position_Segment*
loc,struct _dyneither_ptr*fn,struct Cyc_List_List*atts);static void Cyc_Tcutil_check_field_atts(
struct Cyc_Position_Segment*loc,struct _dyneither_ptr*fn,struct Cyc_List_List*atts){
for(0;atts != 0;atts=atts->tl){void*_tmp774=(void*)atts->hd;_LL4D1: if((int)
_tmp774 != 5)goto _LL4D3;_LL4D2: continue;_LL4D3: if(_tmp774 <= (void*)17)goto _LL4D5;
if(*((int*)_tmp774)!= 1)goto _LL4D5;_LL4D4: continue;_LL4D5:;_LL4D6: {const char*
_tmpEAD;void*_tmpEAC[2];struct Cyc_String_pa_struct _tmpEAB;struct Cyc_String_pa_struct
_tmpEAA;(_tmpEAA.tag=0,((_tmpEAA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*
fn),((_tmpEAB.tag=0,((_tmpEAB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absyn_attribute2string((void*)atts->hd)),((_tmpEAC[0]=& _tmpEAB,((_tmpEAC[1]=&
_tmpEAA,Cyc_Tcutil_terr(loc,((_tmpEAD="bad attribute %s on member %s",
_tag_dyneither(_tmpEAD,sizeof(char),30))),_tag_dyneither(_tmpEAC,sizeof(void*),2)))))))))))));}
_LL4D0:;}}struct Cyc_Tcutil_CVTEnv{struct _RegionHandle*r;struct Cyc_List_List*
kind_env;struct Cyc_List_List*free_vars;struct Cyc_List_List*free_evars;int
generalize_evars;int fn_result;};int Cyc_Tcutil_extract_const_from_typedef(struct
Cyc_Position_Segment*loc,int declared_const,void*t);int Cyc_Tcutil_extract_const_from_typedef(
struct Cyc_Position_Segment*loc,int declared_const,void*t){void*_tmp779=t;struct
Cyc_Absyn_Typedefdecl*_tmp77A;void**_tmp77B;_LL4D8: if(_tmp779 <= (void*)4)goto
_LL4DA;if(*((int*)_tmp779)!= 16)goto _LL4DA;_tmp77A=((struct Cyc_Absyn_TypedefType_struct*)
_tmp779)->f3;_tmp77B=((struct Cyc_Absyn_TypedefType_struct*)_tmp779)->f4;_LL4D9:
if((((struct Cyc_Absyn_Typedefdecl*)_check_null(_tmp77A))->tq).real_const  || (
_tmp77A->tq).print_const){if(declared_const){const char*_tmpEB0;void*_tmpEAF;(
_tmpEAF=0,Cyc_Tcutil_warn(loc,((_tmpEB0="extra const",_tag_dyneither(_tmpEB0,
sizeof(char),12))),_tag_dyneither(_tmpEAF,sizeof(void*),0)));}return 1;}if((
unsigned int)_tmp77B)return Cyc_Tcutil_extract_const_from_typedef(loc,
declared_const,*_tmp77B);else{return declared_const;}_LL4DA:;_LL4DB: return
declared_const;_LL4D7:;}static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type_level_exp(
struct Cyc_Absyn_Exp*e,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv);
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type(struct Cyc_Position_Segment*
loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv,void*expected_kind,
void*t,int put_in_effect);static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv
cvtenv,void*expected_kind,void*t,int put_in_effect){static struct Cyc_Core_Opt urgn={(
void*)((void*)3)};static struct Cyc_Core_Opt hrgn={(void*)((void*)2)};{void*_tmp77E=
Cyc_Tcutil_compress(t);struct Cyc_Core_Opt*_tmp77F;struct Cyc_Core_Opt**_tmp780;
struct Cyc_Core_Opt*_tmp781;struct Cyc_Core_Opt**_tmp782;struct Cyc_Absyn_Tvar*
_tmp783;struct Cyc_List_List*_tmp784;struct _tuple2*_tmp785;struct Cyc_Absyn_Enumdecl*
_tmp786;struct Cyc_Absyn_Enumdecl**_tmp787;struct Cyc_Absyn_TunionInfo _tmp788;
union Cyc_Absyn_TunionInfoU_union _tmp789;union Cyc_Absyn_TunionInfoU_union*_tmp78A;
struct Cyc_List_List*_tmp78B;struct Cyc_List_List**_tmp78C;struct Cyc_Core_Opt*
_tmp78D;struct Cyc_Core_Opt**_tmp78E;struct Cyc_Absyn_TunionFieldInfo _tmp78F;union
Cyc_Absyn_TunionFieldInfoU_union _tmp790;union Cyc_Absyn_TunionFieldInfoU_union*
_tmp791;struct Cyc_List_List*_tmp792;struct Cyc_Absyn_PtrInfo _tmp793;void*_tmp794;
struct Cyc_Absyn_Tqual _tmp795;struct Cyc_Absyn_Tqual*_tmp796;struct Cyc_Absyn_PtrAtts
_tmp797;void*_tmp798;struct Cyc_Absyn_Conref*_tmp799;struct Cyc_Absyn_Conref*
_tmp79A;struct Cyc_Absyn_Conref*_tmp79B;void*_tmp79C;struct Cyc_Absyn_Exp*_tmp79D;
struct Cyc_Absyn_ArrayInfo _tmp79E;void*_tmp79F;struct Cyc_Absyn_Tqual _tmp7A0;
struct Cyc_Absyn_Tqual*_tmp7A1;struct Cyc_Absyn_Exp*_tmp7A2;struct Cyc_Absyn_Exp**
_tmp7A3;struct Cyc_Absyn_Conref*_tmp7A4;struct Cyc_Position_Segment*_tmp7A5;struct
Cyc_Absyn_FnInfo _tmp7A6;struct Cyc_List_List*_tmp7A7;struct Cyc_List_List**_tmp7A8;
struct Cyc_Core_Opt*_tmp7A9;struct Cyc_Core_Opt**_tmp7AA;void*_tmp7AB;struct Cyc_List_List*
_tmp7AC;int _tmp7AD;struct Cyc_Absyn_VarargInfo*_tmp7AE;struct Cyc_List_List*
_tmp7AF;struct Cyc_List_List*_tmp7B0;struct Cyc_List_List*_tmp7B1;void*_tmp7B2;
struct Cyc_List_List*_tmp7B3;struct Cyc_Absyn_AggrInfo _tmp7B4;union Cyc_Absyn_AggrInfoU_union
_tmp7B5;union Cyc_Absyn_AggrInfoU_union*_tmp7B6;struct Cyc_List_List*_tmp7B7;
struct Cyc_List_List**_tmp7B8;struct _tuple2*_tmp7B9;struct Cyc_List_List*_tmp7BA;
struct Cyc_List_List**_tmp7BB;struct Cyc_Absyn_Typedefdecl*_tmp7BC;struct Cyc_Absyn_Typedefdecl**
_tmp7BD;void**_tmp7BE;void***_tmp7BF;void*_tmp7C0;void*_tmp7C1;void*_tmp7C2;void*
_tmp7C3;void*_tmp7C4;struct Cyc_List_List*_tmp7C5;_LL4DD: if((int)_tmp77E != 0)goto
_LL4DF;_LL4DE: goto _LL4DC;_LL4DF: if(_tmp77E <= (void*)4)goto _LL4F3;if(*((int*)
_tmp77E)!= 0)goto _LL4E1;_tmp77F=((struct Cyc_Absyn_Evar_struct*)_tmp77E)->f1;
_tmp780=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)_tmp77E)->f1;
_tmp781=((struct Cyc_Absyn_Evar_struct*)_tmp77E)->f2;_tmp782=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Evar_struct*)_tmp77E)->f2;_LL4E0: if(*_tmp780 == 0)*_tmp780=Cyc_Tcutil_kind_to_opt(
expected_kind);if((cvtenv.fn_result  && cvtenv.generalize_evars) && Cyc_Tcutil_is_region_kind(
expected_kind)){if(expected_kind == (void*)4)*_tmp782=(struct Cyc_Core_Opt*)& urgn;
else{*_tmp782=(struct Cyc_Core_Opt*)& hrgn;}}else{if(cvtenv.generalize_evars){
struct Cyc_Absyn_Less_kb_struct _tmpEB3;struct Cyc_Absyn_Less_kb_struct*_tmpEB2;
struct Cyc_Absyn_Tvar*_tmp7C6=Cyc_Tcutil_new_tvar((void*)((_tmpEB2=_cycalloc(
sizeof(*_tmpEB2)),((_tmpEB2[0]=((_tmpEB3.tag=2,((_tmpEB3.f1=0,((_tmpEB3.f2=(void*)
expected_kind,_tmpEB3)))))),_tmpEB2)))));{struct Cyc_Absyn_VarType_struct*_tmpEB9;
struct Cyc_Absyn_VarType_struct _tmpEB8;struct Cyc_Core_Opt*_tmpEB7;*_tmp782=((
_tmpEB7=_cycalloc(sizeof(*_tmpEB7)),((_tmpEB7->v=(void*)((void*)((_tmpEB9=
_cycalloc(sizeof(*_tmpEB9)),((_tmpEB9[0]=((_tmpEB8.tag=1,((_tmpEB8.f1=_tmp7C6,
_tmpEB8)))),_tmpEB9))))),_tmpEB7))));}_tmp783=_tmp7C6;goto _LL4E2;}else{cvtenv.free_evars=
Cyc_Tcutil_add_free_evar(cvtenv.r,cvtenv.free_evars,t,put_in_effect);}}goto
_LL4DC;_LL4E1: if(*((int*)_tmp77E)!= 1)goto _LL4E3;_tmp783=((struct Cyc_Absyn_VarType_struct*)
_tmp77E)->f1;_LL4E2:{void*_tmp7CC=Cyc_Absyn_compress_kb((void*)_tmp783->kind);
struct Cyc_Core_Opt*_tmp7CD;struct Cyc_Core_Opt**_tmp7CE;_LL512: if(*((int*)_tmp7CC)
!= 1)goto _LL514;_tmp7CD=((struct Cyc_Absyn_Unknown_kb_struct*)_tmp7CC)->f1;
_tmp7CE=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)_tmp7CC)->f1;
_LL513:{struct Cyc_Absyn_Less_kb_struct*_tmpEBF;struct Cyc_Absyn_Less_kb_struct
_tmpEBE;struct Cyc_Core_Opt*_tmpEBD;*_tmp7CE=((_tmpEBD=_cycalloc(sizeof(*_tmpEBD)),((
_tmpEBD->v=(void*)((void*)((_tmpEBF=_cycalloc(sizeof(*_tmpEBF)),((_tmpEBF[0]=((
_tmpEBE.tag=2,((_tmpEBE.f1=0,((_tmpEBE.f2=(void*)expected_kind,_tmpEBE)))))),
_tmpEBF))))),_tmpEBD))));}goto _LL511;_LL514:;_LL515: goto _LL511;_LL511:;}cvtenv.kind_env=
Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmp783);cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(
cvtenv.r,cvtenv.free_vars,_tmp783,put_in_effect);{void*_tmp7D2=Cyc_Absyn_compress_kb((
void*)_tmp783->kind);struct Cyc_Core_Opt*_tmp7D3;struct Cyc_Core_Opt**_tmp7D4;void*
_tmp7D5;_LL517: if(*((int*)_tmp7D2)!= 2)goto _LL519;_tmp7D3=((struct Cyc_Absyn_Less_kb_struct*)
_tmp7D2)->f1;_tmp7D4=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp7D2)->f1;_tmp7D5=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp7D2)->f2;
_LL518: if(Cyc_Tcutil_kind_leq(expected_kind,_tmp7D5)){struct Cyc_Absyn_Less_kb_struct*
_tmpEC5;struct Cyc_Absyn_Less_kb_struct _tmpEC4;struct Cyc_Core_Opt*_tmpEC3;*
_tmp7D4=((_tmpEC3=_cycalloc(sizeof(*_tmpEC3)),((_tmpEC3->v=(void*)((void*)((
_tmpEC5=_cycalloc(sizeof(*_tmpEC5)),((_tmpEC5[0]=((_tmpEC4.tag=2,((_tmpEC4.f1=0,((
_tmpEC4.f2=(void*)expected_kind,_tmpEC4)))))),_tmpEC5))))),_tmpEC3))));}goto
_LL516;_LL519:;_LL51A: goto _LL516;_LL516:;}goto _LL4DC;_LL4E3: if(*((int*)_tmp77E)
!= 13)goto _LL4E5;_tmp784=((struct Cyc_Absyn_AnonEnumType_struct*)_tmp77E)->f1;
_LL4E4: {struct Cyc_Tcenv_Genv*ge=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,
struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);struct _RegionHandle*
_tmp7D9=Cyc_Tcenv_get_fnrgn(te);{struct Cyc_List_List*prev_fields=0;unsigned int
tag_count=0;for(0;_tmp784 != 0;_tmp784=_tmp784->tl){struct Cyc_Absyn_Enumfield*
_tmp7DA=(struct Cyc_Absyn_Enumfield*)_tmp784->hd;if(((int(*)(int(*compare)(struct
_dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct
_dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,(*_tmp7DA->name).f2)){
const char*_tmpEC9;void*_tmpEC8[1];struct Cyc_String_pa_struct _tmpEC7;(_tmpEC7.tag=
0,((_tmpEC7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp7DA->name).f2),((
_tmpEC8[0]=& _tmpEC7,Cyc_Tcutil_terr(_tmp7DA->loc,((_tmpEC9="duplicate enum field name %s",
_tag_dyneither(_tmpEC9,sizeof(char),29))),_tag_dyneither(_tmpEC8,sizeof(void*),1)))))));}
else{struct Cyc_List_List*_tmpECA;prev_fields=((_tmpECA=_region_malloc(_tmp7D9,
sizeof(*_tmpECA)),((_tmpECA->hd=(*_tmp7DA->name).f2,((_tmpECA->tl=prev_fields,
_tmpECA))))));}if(_tmp7DA->tag == 0)_tmp7DA->tag=(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(
tag_count,_tmp7DA->loc);else{if(!Cyc_Tcutil_is_const_exp(te,(struct Cyc_Absyn_Exp*)
_check_null(_tmp7DA->tag))){const char*_tmpECE;void*_tmpECD[1];struct Cyc_String_pa_struct
_tmpECC;(_tmpECC.tag=0,((_tmpECC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*
_tmp7DA->name).f2),((_tmpECD[0]=& _tmpECC,Cyc_Tcutil_terr(loc,((_tmpECE="enum field %s: expression is not constant",
_tag_dyneither(_tmpECE,sizeof(char),42))),_tag_dyneither(_tmpECD,sizeof(void*),1)))))));}}{
unsigned int t1=(Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)_check_null(
_tmp7DA->tag))).f1;tag_count=t1 + 1;{union Cyc_Absyn_Nmspace_union _tmpECF;(*
_tmp7DA->name).f1=(union Cyc_Absyn_Nmspace_union)(((_tmpECF.Abs_n).tag=2,(((
_tmpECF.Abs_n).f1=te->ns,_tmpECF))));}{struct Cyc_Tcenv_AnonEnumRes_struct*
_tmpED5;struct Cyc_Tcenv_AnonEnumRes_struct _tmpED4;struct _tuple19*_tmpED3;ge->ordinaries=((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct
_tuple19*v))Cyc_Dict_insert)(ge->ordinaries,(*_tmp7DA->name).f2,(struct _tuple19*)((
_tmpED3=_cycalloc(sizeof(*_tmpED3)),((_tmpED3->f1=(void*)((_tmpED5=_cycalloc(
sizeof(*_tmpED5)),((_tmpED5[0]=((_tmpED4.tag=4,((_tmpED4.f1=(void*)t,((_tmpED4.f2=
_tmp7DA,_tmpED4)))))),_tmpED5)))),((_tmpED3->f2=1,_tmpED3)))))));}}}}goto _LL4DC;}
_LL4E5: if(*((int*)_tmp77E)!= 12)goto _LL4E7;_tmp785=((struct Cyc_Absyn_EnumType_struct*)
_tmp77E)->f1;_tmp786=((struct Cyc_Absyn_EnumType_struct*)_tmp77E)->f2;_tmp787=(
struct Cyc_Absyn_Enumdecl**)&((struct Cyc_Absyn_EnumType_struct*)_tmp77E)->f2;
_LL4E6: if(*_tmp787 == 0  || ((struct Cyc_Absyn_Enumdecl*)_check_null(*_tmp787))->fields
== 0){struct _handler_cons _tmp7E6;_push_handler(& _tmp7E6);{int _tmp7E8=0;if(setjmp(
_tmp7E6.handler))_tmp7E8=1;if(!_tmp7E8){{struct Cyc_Absyn_Enumdecl**ed=Cyc_Tcenv_lookup_enumdecl(
te,loc,_tmp785);*_tmp787=(struct Cyc_Absyn_Enumdecl*)*ed;};_pop_handler();}else{
void*_tmp7E7=(void*)_exn_thrown;void*_tmp7EA=_tmp7E7;_LL51C: if(_tmp7EA != Cyc_Dict_Absent)
goto _LL51E;_LL51D: {struct Cyc_Tcenv_Genv*_tmp7EB=((struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct Cyc_Absyn_Enumdecl*_tmpED6;struct Cyc_Absyn_Enumdecl*_tmp7EC=(_tmpED6=
_cycalloc(sizeof(*_tmpED6)),((_tmpED6->sc=(void*)((void*)3),((_tmpED6->name=
_tmp785,((_tmpED6->fields=0,_tmpED6)))))));Cyc_Tc_tcEnumdecl(te,_tmp7EB,loc,
_tmp7EC);{struct Cyc_Absyn_Enumdecl**ed=Cyc_Tcenv_lookup_enumdecl(te,loc,_tmp785);*
_tmp787=(struct Cyc_Absyn_Enumdecl*)*ed;goto _LL51B;}}_LL51E:;_LL51F:(void)_throw(
_tmp7EA);_LL51B:;}}}{struct Cyc_Absyn_Enumdecl*ed=(struct Cyc_Absyn_Enumdecl*)
_check_null(*_tmp787);*_tmp785=(ed->name)[_check_known_subscript_notnull(1,0)];
goto _LL4DC;}_LL4E7: if(*((int*)_tmp77E)!= 2)goto _LL4E9;_tmp788=((struct Cyc_Absyn_TunionType_struct*)
_tmp77E)->f1;_tmp789=_tmp788.tunion_info;_tmp78A=(union Cyc_Absyn_TunionInfoU_union*)&(((
struct Cyc_Absyn_TunionType_struct*)_tmp77E)->f1).tunion_info;_tmp78B=_tmp788.targs;
_tmp78C=(struct Cyc_List_List**)&(((struct Cyc_Absyn_TunionType_struct*)_tmp77E)->f1).targs;
_tmp78D=_tmp788.rgn;_tmp78E=(struct Cyc_Core_Opt**)&(((struct Cyc_Absyn_TunionType_struct*)
_tmp77E)->f1).rgn;_LL4E8: {struct Cyc_List_List*_tmp7EE=*_tmp78C;{union Cyc_Absyn_TunionInfoU_union
_tmp7EF=*_tmp78A;struct Cyc_Absyn_UnknownTunionInfo _tmp7F0;struct _tuple2*_tmp7F1;
int _tmp7F2;int _tmp7F3;struct Cyc_Absyn_Tuniondecl**_tmp7F4;struct Cyc_Absyn_Tuniondecl*
_tmp7F5;_LL521: if((_tmp7EF.UnknownTunion).tag != 0)goto _LL523;_tmp7F0=(_tmp7EF.UnknownTunion).f1;
_tmp7F1=_tmp7F0.name;_tmp7F2=_tmp7F0.is_xtunion;_tmp7F3=_tmp7F0.is_flat;_LL522: {
struct Cyc_Absyn_Tuniondecl**tudp;{struct _handler_cons _tmp7F6;_push_handler(&
_tmp7F6);{int _tmp7F8=0;if(setjmp(_tmp7F6.handler))_tmp7F8=1;if(!_tmp7F8){tudp=
Cyc_Tcenv_lookup_tuniondecl(te,loc,_tmp7F1);;_pop_handler();}else{void*_tmp7F7=(
void*)_exn_thrown;void*_tmp7FA=_tmp7F7;_LL526: if(_tmp7FA != Cyc_Dict_Absent)goto
_LL528;_LL527: {struct Cyc_Tcenv_Genv*_tmp7FB=((struct Cyc_Tcenv_Genv*(*)(struct
Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);struct Cyc_Absyn_Tuniondecl*
_tmpED7;struct Cyc_Absyn_Tuniondecl*_tmp7FC=(_tmpED7=_cycalloc(sizeof(*_tmpED7)),((
_tmpED7->sc=(void*)((void*)3),((_tmpED7->name=_tmp7F1,((_tmpED7->tvs=0,((_tmpED7->fields=
0,((_tmpED7->is_xtunion=_tmp7F2,((_tmpED7->is_flat=_tmp7F3,_tmpED7)))))))))))));
Cyc_Tc_tcTuniondecl(te,_tmp7FB,loc,_tmp7FC);tudp=Cyc_Tcenv_lookup_tuniondecl(te,
loc,_tmp7F1);if(_tmp7EE != 0){{const char*_tmpEE0;const char*_tmpEDF;const char*
_tmpEDE;void*_tmpEDD[2];struct Cyc_String_pa_struct _tmpEDC;struct Cyc_String_pa_struct
_tmpEDB;(_tmpEDB.tag=0,((_tmpEDB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp7F1)),((_tmpEDC.tag=0,((_tmpEDC.f1=(struct
_dyneither_ptr)(_tmp7F2?(struct _dyneither_ptr)((struct _dyneither_ptr)((_tmpEDF="xtunion",
_tag_dyneither(_tmpEDF,sizeof(char),8)))):(struct _dyneither_ptr)((_tmpEE0="tunion",
_tag_dyneither(_tmpEE0,sizeof(char),7)))),((_tmpEDD[0]=& _tmpEDC,((_tmpEDD[1]=&
_tmpEDB,Cyc_Tcutil_terr(loc,((_tmpEDE="declare parameterized %s %s before using",
_tag_dyneither(_tmpEDE,sizeof(char),41))),_tag_dyneither(_tmpEDD,sizeof(void*),2)))))))))))));}
return cvtenv;}goto _LL525;}_LL528:;_LL529:(void)_throw(_tmp7FA);_LL525:;}}}if((*
tudp)->is_xtunion != _tmp7F2){const char*_tmpEE4;void*_tmpEE3[1];struct Cyc_String_pa_struct
_tmpEE2;(_tmpEE2.tag=0,((_tmpEE2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp7F1)),((_tmpEE3[0]=& _tmpEE2,Cyc_Tcutil_terr(loc,((
_tmpEE4="[x]tunion is different from type declaration %s",_tag_dyneither(_tmpEE4,
sizeof(char),48))),_tag_dyneither(_tmpEE3,sizeof(void*),1)))))));}{union Cyc_Absyn_TunionInfoU_union
_tmpEE5;*_tmp78A=(union Cyc_Absyn_TunionInfoU_union)(((_tmpEE5.KnownTunion).tag=1,(((
_tmpEE5.KnownTunion).f1=tudp,_tmpEE5))));}_tmp7F5=*tudp;goto _LL524;}_LL523: if((
_tmp7EF.KnownTunion).tag != 1)goto _LL520;_tmp7F4=(_tmp7EF.KnownTunion).f1;_tmp7F5=*
_tmp7F4;_LL524: if(_tmp7F5->is_flat  && (unsigned int)*_tmp78E){const char*_tmpEE9;
void*_tmpEE8[1];struct Cyc_String_pa_struct _tmpEE7;(_tmpEE7.tag=0,((_tmpEE7.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)((
struct Cyc_Core_Opt*)_check_null(*_tmp78E))->v)),((_tmpEE8[0]=& _tmpEE7,Cyc_Tcutil_terr(
loc,((_tmpEE9="flat tunion has region %s",_tag_dyneither(_tmpEE9,sizeof(char),26))),
_tag_dyneither(_tmpEE8,sizeof(void*),1)))))));}if(!_tmp7F5->is_flat  && !((
unsigned int)*_tmp78E)){struct Cyc_Core_Opt*_tmpEEA;*_tmp78E=((_tmpEEA=_cycalloc(
sizeof(*_tmpEEA)),((_tmpEEA->v=(void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_rk,0),_tmpEEA))));}if((unsigned int)*_tmp78E){void*_tmp80C=(void*)((
struct Cyc_Core_Opt*)_check_null(*_tmp78E))->v;cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,(void*)3,_tmp80C,1);}{struct Cyc_List_List*tvs=_tmp7F5->tvs;for(0;
_tmp7EE != 0  && tvs != 0;(_tmp7EE=_tmp7EE->tl,tvs=tvs->tl)){void*t1=(void*)_tmp7EE->hd;
void*k1=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd);cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,k1,t1,1);}if(_tmp7EE != 0){const char*_tmpEEE;void*_tmpEED[1];struct
Cyc_String_pa_struct _tmpEEC;(_tmpEEC.tag=0,((_tmpEEC.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp7F5->name)),((_tmpEED[0]=&
_tmpEEC,Cyc_Tcutil_terr(loc,((_tmpEEE="too many type arguments for tunion %s",
_tag_dyneither(_tmpEEE,sizeof(char),38))),_tag_dyneither(_tmpEED,sizeof(void*),1)))))));}
if(tvs != 0){struct Cyc_List_List*hidden_ts=0;for(0;tvs != 0;tvs=tvs->tl){void*k1=
Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd);void*e=Cyc_Absyn_new_evar(0,
0);{struct Cyc_List_List*_tmpEEF;hidden_ts=((_tmpEEF=_cycalloc(sizeof(*_tmpEEF)),((
_tmpEEF->hd=(void*)e,((_tmpEEF->tl=hidden_ts,_tmpEEF))))));}cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,k1,e,1);}*_tmp78C=Cyc_List_imp_append(*_tmp78C,Cyc_List_imp_rev(
hidden_ts));}goto _LL520;}_LL520:;}goto _LL4DC;}_LL4E9: if(*((int*)_tmp77E)!= 3)
goto _LL4EB;_tmp78F=((struct Cyc_Absyn_TunionFieldType_struct*)_tmp77E)->f1;
_tmp790=_tmp78F.field_info;_tmp791=(union Cyc_Absyn_TunionFieldInfoU_union*)&(((
struct Cyc_Absyn_TunionFieldType_struct*)_tmp77E)->f1).field_info;_tmp792=_tmp78F.targs;
_LL4EA:{union Cyc_Absyn_TunionFieldInfoU_union _tmp811=*_tmp791;struct Cyc_Absyn_UnknownTunionFieldInfo
_tmp812;struct _tuple2*_tmp813;struct _tuple2*_tmp814;int _tmp815;struct Cyc_Absyn_Tuniondecl*
_tmp816;struct Cyc_Absyn_Tunionfield*_tmp817;_LL52B: if((_tmp811.UnknownTunionfield).tag
!= 0)goto _LL52D;_tmp812=(_tmp811.UnknownTunionfield).f1;_tmp813=_tmp812.tunion_name;
_tmp814=_tmp812.field_name;_tmp815=_tmp812.is_xtunion;_LL52C: {struct Cyc_Absyn_Tuniondecl*
tud;struct Cyc_Absyn_Tunionfield*tuf;{struct _handler_cons _tmp818;_push_handler(&
_tmp818);{int _tmp81A=0;if(setjmp(_tmp818.handler))_tmp81A=1;if(!_tmp81A){*Cyc_Tcenv_lookup_tuniondecl(
te,loc,_tmp813);;_pop_handler();}else{void*_tmp819=(void*)_exn_thrown;void*
_tmp81C=_tmp819;_LL530: if(_tmp81C != Cyc_Dict_Absent)goto _LL532;_LL531:{const char*
_tmpEF3;void*_tmpEF2[1];struct Cyc_String_pa_struct _tmpEF1;(_tmpEF1.tag=0,((
_tmpEF1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp813)),((_tmpEF2[0]=& _tmpEF1,Cyc_Tcutil_terr(loc,((_tmpEF3="unbound type tunion %s",
_tag_dyneither(_tmpEF3,sizeof(char),23))),_tag_dyneither(_tmpEF2,sizeof(void*),1)))))));}
return cvtenv;_LL532:;_LL533:(void)_throw(_tmp81C);_LL52F:;}}}{struct
_handler_cons _tmp820;_push_handler(& _tmp820);{int _tmp822=0;if(setjmp(_tmp820.handler))
_tmp822=1;if(!_tmp822){{struct _RegionHandle*_tmp823=Cyc_Tcenv_get_fnrgn(te);void*
_tmp824=Cyc_Tcenv_lookup_ordinary(_tmp823,te,loc,_tmp814);struct Cyc_Absyn_Tuniondecl*
_tmp825;struct Cyc_Absyn_Tunionfield*_tmp826;_LL535: if(*((int*)_tmp824)!= 2)goto
_LL537;_tmp825=((struct Cyc_Tcenv_TunionRes_struct*)_tmp824)->f1;_tmp826=((struct
Cyc_Tcenv_TunionRes_struct*)_tmp824)->f2;_LL536: tuf=_tmp826;tud=_tmp825;if(tud->is_xtunion
!= _tmp815){const char*_tmpEF7;void*_tmpEF6[1];struct Cyc_String_pa_struct _tmpEF5;(
_tmpEF5.tag=0,((_tmpEF5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp813)),((_tmpEF6[0]=& _tmpEF5,Cyc_Tcutil_terr(loc,((_tmpEF7="[x]tunion is different from type declaration %s",
_tag_dyneither(_tmpEF7,sizeof(char),48))),_tag_dyneither(_tmpEF6,sizeof(void*),1)))))));}
goto _LL534;_LL537:;_LL538:{const char*_tmpEFC;void*_tmpEFB[2];struct Cyc_String_pa_struct
_tmpEFA;struct Cyc_String_pa_struct _tmpEF9;(_tmpEF9.tag=0,((_tmpEF9.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp813)),((
_tmpEFA.tag=0,((_tmpEFA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp814)),((_tmpEFB[0]=& _tmpEFA,((_tmpEFB[1]=& _tmpEF9,Cyc_Tcutil_terr(loc,((
_tmpEFC="unbound field %s in type tunion %s",_tag_dyneither(_tmpEFC,sizeof(char),
35))),_tag_dyneither(_tmpEFB,sizeof(void*),2)))))))))))));}{struct Cyc_Tcutil_CVTEnv
_tmp82E=cvtenv;_npop_handler(0);return _tmp82E;}_LL534:;};_pop_handler();}else{
void*_tmp821=(void*)_exn_thrown;void*_tmp830=_tmp821;_LL53A: if(_tmp830 != Cyc_Dict_Absent)
goto _LL53C;_LL53B:{const char*_tmpF01;void*_tmpF00[2];struct Cyc_String_pa_struct
_tmpEFF;struct Cyc_String_pa_struct _tmpEFE;(_tmpEFE.tag=0,((_tmpEFE.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp813)),((
_tmpEFF.tag=0,((_tmpEFF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp814)),((_tmpF00[0]=& _tmpEFF,((_tmpF00[1]=& _tmpEFE,Cyc_Tcutil_terr(loc,((
_tmpF01="unbound field %s in type tunion %s",_tag_dyneither(_tmpF01,sizeof(char),
35))),_tag_dyneither(_tmpF00,sizeof(void*),2)))))))))))));}return cvtenv;_LL53C:;
_LL53D:(void)_throw(_tmp830);_LL539:;}}}{union Cyc_Absyn_TunionFieldInfoU_union
_tmpF02;*_tmp791=(union Cyc_Absyn_TunionFieldInfoU_union)(((_tmpF02.KnownTunionfield).tag=
1,(((_tmpF02.KnownTunionfield).f1=tud,(((_tmpF02.KnownTunionfield).f2=tuf,
_tmpF02))))));}_tmp816=tud;_tmp817=tuf;goto _LL52E;}_LL52D: if((_tmp811.KnownTunionfield).tag
!= 1)goto _LL52A;_tmp816=(_tmp811.KnownTunionfield).f1;_tmp817=(_tmp811.KnownTunionfield).f2;
_LL52E: {struct Cyc_List_List*tvs=_tmp816->tvs;for(0;_tmp792 != 0  && tvs != 0;(
_tmp792=_tmp792->tl,tvs=tvs->tl)){void*t1=(void*)_tmp792->hd;void*k1=Cyc_Tcutil_tvar_kind((
struct Cyc_Absyn_Tvar*)tvs->hd);cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,
cvtenv,k1,t1,1);}if(_tmp792 != 0){const char*_tmpF07;void*_tmpF06[2];struct Cyc_String_pa_struct
_tmpF05;struct Cyc_String_pa_struct _tmpF04;(_tmpF04.tag=0,((_tmpF04.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp817->name)),((
_tmpF05.tag=0,((_tmpF05.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp816->name)),((_tmpF06[0]=& _tmpF05,((_tmpF06[1]=& _tmpF04,Cyc_Tcutil_terr(loc,((
_tmpF07="too many type arguments for tunion %s.%s",_tag_dyneither(_tmpF07,
sizeof(char),41))),_tag_dyneither(_tmpF06,sizeof(void*),2)))))))))))));}if(tvs != 
0){const char*_tmpF0C;void*_tmpF0B[2];struct Cyc_String_pa_struct _tmpF0A;struct Cyc_String_pa_struct
_tmpF09;(_tmpF09.tag=0,((_tmpF09.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp817->name)),((_tmpF0A.tag=0,((_tmpF0A.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp816->name)),((
_tmpF0B[0]=& _tmpF0A,((_tmpF0B[1]=& _tmpF09,Cyc_Tcutil_terr(loc,((_tmpF0C="too few type arguments for tunion %s.%s",
_tag_dyneither(_tmpF0C,sizeof(char),40))),_tag_dyneither(_tmpF0B,sizeof(void*),2)))))))))))));}
goto _LL52A;}_LL52A:;}goto _LL4DC;_LL4EB: if(*((int*)_tmp77E)!= 4)goto _LL4ED;
_tmp793=((struct Cyc_Absyn_PointerType_struct*)_tmp77E)->f1;_tmp794=(void*)
_tmp793.elt_typ;_tmp795=_tmp793.elt_tq;_tmp796=(struct Cyc_Absyn_Tqual*)&(((
struct Cyc_Absyn_PointerType_struct*)_tmp77E)->f1).elt_tq;_tmp797=_tmp793.ptr_atts;
_tmp798=(void*)_tmp797.rgn;_tmp799=_tmp797.nullable;_tmp79A=_tmp797.bounds;
_tmp79B=_tmp797.zero_term;_LL4EC: {int is_zero_terminated;cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,(void*)0,_tmp794,1);_tmp796->real_const=Cyc_Tcutil_extract_const_from_typedef(
loc,_tmp796->print_const,_tmp794);cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,
cvtenv,(void*)5,_tmp798,1);{union Cyc_Absyn_Constraint_union _tmp83E=(((struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref*x))Cyc_Absyn_compress_conref)(_tmp79B))->v;int _tmp83F;
_LL53F: if((_tmp83E.No_constr).tag != 2)goto _LL541;_LL540:{void*_tmp840=Cyc_Tcutil_compress(
_tmp794);void*_tmp841;_LL546: if(_tmp840 <= (void*)4)goto _LL548;if(*((int*)_tmp840)
!= 5)goto _LL548;_tmp841=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp840)->f2;
if((int)_tmp841 != 0)goto _LL548;_LL547:((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*
x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp79B,
Cyc_Absyn_true_conref);is_zero_terminated=1;goto _LL545;_LL548:;_LL549:((int(*)(
int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(
Cyc_Core_intcmp,_tmp79B,Cyc_Absyn_false_conref);is_zero_terminated=0;goto _LL545;
_LL545:;}goto _LL53E;_LL541: if((_tmp83E.Eq_constr).tag != 0)goto _LL543;_tmp83F=(
_tmp83E.Eq_constr).f1;if(_tmp83F != 1)goto _LL543;_LL542: if(!Cyc_Tcutil_admits_zero(
_tmp794)){const char*_tmpF10;void*_tmpF0F[1];struct Cyc_String_pa_struct _tmpF0E;(
_tmpF0E.tag=0,((_tmpF0E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
_tmp794)),((_tmpF0F[0]=& _tmpF0E,Cyc_Tcutil_terr(loc,((_tmpF10="cannot have a pointer to zero-terminated %s elements",
_tag_dyneither(_tmpF10,sizeof(char),53))),_tag_dyneither(_tmpF0F,sizeof(void*),1)))))));}
is_zero_terminated=1;goto _LL53E;_LL543:;_LL544: is_zero_terminated=0;goto _LL53E;
_LL53E:;}{union Cyc_Absyn_Constraint_union _tmp845=(Cyc_Absyn_compress_conref(
_tmp79A))->v;void*_tmp846;void*_tmp847;struct Cyc_Absyn_Exp*_tmp848;_LL54B: if((
_tmp845.No_constr).tag != 2)goto _LL54D;_LL54C: goto _LL54E;_LL54D: if((_tmp845.Eq_constr).tag
!= 0)goto _LL54F;_tmp846=(_tmp845.Eq_constr).f1;if((int)_tmp846 != 0)goto _LL54F;
_LL54E: goto _LL54A;_LL54F: if((_tmp845.Eq_constr).tag != 0)goto _LL551;_tmp847=(
_tmp845.Eq_constr).f1;if(_tmp847 <= (void*)1)goto _LL551;if(*((int*)_tmp847)!= 0)
goto _LL551;_tmp848=((struct Cyc_Absyn_Upper_b_struct*)_tmp847)->f1;_LL550: {
struct _RegionHandle*_tmp849=Cyc_Tcenv_get_fnrgn(te);{struct Cyc_Tcenv_Tenv*
_tmp84A=Cyc_Tcenv_allow_valueof(_tmp849,te);Cyc_Tcexp_tcExp(_tmp84A,0,_tmp848);}
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp848,te,cvtenv);if(!Cyc_Tcutil_coerce_uint_typ(
te,_tmp848)){const char*_tmpF13;void*_tmpF12;(_tmpF12=0,Cyc_Tcutil_terr(loc,((
_tmpF13="pointer bounds expression is not an unsigned int",_tag_dyneither(
_tmpF13,sizeof(char),49))),_tag_dyneither(_tmpF12,sizeof(void*),0)));}{
unsigned int _tmp84E;int _tmp84F;struct _tuple7 _tmp84D=Cyc_Evexp_eval_const_uint_exp(
_tmp848);_tmp84E=_tmp84D.f1;_tmp84F=_tmp84D.f2;if(is_zero_terminated  && (!
_tmp84F  || _tmp84E < 1)){const char*_tmpF16;void*_tmpF15;(_tmpF15=0,Cyc_Tcutil_terr(
loc,((_tmpF16="zero-terminated pointer cannot point to empty sequence",
_tag_dyneither(_tmpF16,sizeof(char),55))),_tag_dyneither(_tmpF15,sizeof(void*),0)));}
goto _LL54A;}}_LL551: if((_tmp845.Forward_constr).tag != 1)goto _LL54A;_LL552: {
const char*_tmpF19;void*_tmpF18;(_tmpF18=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpF19="forward constraint",
_tag_dyneither(_tmpF19,sizeof(char),19))),_tag_dyneither(_tmpF18,sizeof(void*),0)));}
_LL54A:;}goto _LL4DC;}_LL4ED: if(*((int*)_tmp77E)!= 18)goto _LL4EF;_tmp79C=(void*)((
struct Cyc_Absyn_TagType_struct*)_tmp77E)->f1;_LL4EE: cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,(void*)7,_tmp79C,1);goto _LL4DC;_LL4EF: if(*((int*)_tmp77E)!= 17)
goto _LL4F1;_tmp79D=((struct Cyc_Absyn_ValueofType_struct*)_tmp77E)->f1;_LL4F0: {
struct _RegionHandle*_tmp854=Cyc_Tcenv_get_fnrgn(te);{struct Cyc_Tcenv_Tenv*
_tmp855=Cyc_Tcenv_allow_valueof(_tmp854,te);Cyc_Tcexp_tcExp(_tmp855,0,_tmp79D);}
if(!Cyc_Tcutil_coerce_uint_typ(te,_tmp79D)){const char*_tmpF1C;void*_tmpF1B;(
_tmpF1B=0,Cyc_Tcutil_terr(loc,((_tmpF1C="valueof_t requires an int expression",
_tag_dyneither(_tmpF1C,sizeof(char),37))),_tag_dyneither(_tmpF1B,sizeof(void*),0)));}
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp79D,te,cvtenv);goto _LL4DC;}
_LL4F1: if(*((int*)_tmp77E)!= 5)goto _LL4F3;_LL4F2: goto _LL4F4;_LL4F3: if((int)
_tmp77E != 1)goto _LL4F5;_LL4F4: goto _LL4F6;_LL4F5: if(_tmp77E <= (void*)4)goto _LL503;
if(*((int*)_tmp77E)!= 6)goto _LL4F7;_LL4F6: goto _LL4DC;_LL4F7: if(*((int*)_tmp77E)
!= 7)goto _LL4F9;_tmp79E=((struct Cyc_Absyn_ArrayType_struct*)_tmp77E)->f1;_tmp79F=(
void*)_tmp79E.elt_type;_tmp7A0=_tmp79E.tq;_tmp7A1=(struct Cyc_Absyn_Tqual*)&(((
struct Cyc_Absyn_ArrayType_struct*)_tmp77E)->f1).tq;_tmp7A2=_tmp79E.num_elts;
_tmp7A3=(struct Cyc_Absyn_Exp**)&(((struct Cyc_Absyn_ArrayType_struct*)_tmp77E)->f1).num_elts;
_tmp7A4=_tmp79E.zero_term;_tmp7A5=_tmp79E.zt_loc;_LL4F8: {struct Cyc_Absyn_Exp*
_tmp858=*_tmp7A3;cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)1,
_tmp79F,1);_tmp7A1->real_const=Cyc_Tcutil_extract_const_from_typedef(loc,_tmp7A1->print_const,
_tmp79F);{int is_zero_terminated;{union Cyc_Absyn_Constraint_union _tmp859=(((
struct Cyc_Absyn_Conref*(*)(struct Cyc_Absyn_Conref*x))Cyc_Absyn_compress_conref)(
_tmp7A4))->v;int _tmp85A;_LL554: if((_tmp859.No_constr).tag != 2)goto _LL556;_LL555:((
int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(
Cyc_Core_intcmp,_tmp7A4,Cyc_Absyn_false_conref);is_zero_terminated=0;goto _LL553;
_LL556: if((_tmp859.Eq_constr).tag != 0)goto _LL558;_tmp85A=(_tmp859.Eq_constr).f1;
if(_tmp85A != 1)goto _LL558;_LL557: if(!Cyc_Tcutil_admits_zero(_tmp79F)){const char*
_tmpF20;void*_tmpF1F[1];struct Cyc_String_pa_struct _tmpF1E;(_tmpF1E.tag=0,((
_tmpF1E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
_tmp79F)),((_tmpF1F[0]=& _tmpF1E,Cyc_Tcutil_terr(loc,((_tmpF20="cannot have a zero-terminated array of %s elements",
_tag_dyneither(_tmpF20,sizeof(char),51))),_tag_dyneither(_tmpF1F,sizeof(void*),1)))))));}
is_zero_terminated=1;goto _LL553;_LL558:;_LL559: is_zero_terminated=0;goto _LL553;
_LL553:;}if(_tmp858 == 0){if(is_zero_terminated)*_tmp7A3=(_tmp858=(struct Cyc_Absyn_Exp*)
Cyc_Absyn_uint_exp(1,0));else{{const char*_tmpF23;void*_tmpF22;(_tmpF22=0,Cyc_Tcutil_warn(
loc,((_tmpF23="array bound defaults to 1 here",_tag_dyneither(_tmpF23,sizeof(
char),31))),_tag_dyneither(_tmpF22,sizeof(void*),0)));}*_tmp7A3=(_tmp858=(struct
Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(1,0));}}Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)
_tmp858);if(!Cyc_Tcutil_is_const_exp(te,(struct Cyc_Absyn_Exp*)_tmp858)){const
char*_tmpF26;void*_tmpF25;(_tmpF25=0,Cyc_Tcutil_terr(loc,((_tmpF26="array bounds expression is not constant",
_tag_dyneither(_tmpF26,sizeof(char),40))),_tag_dyneither(_tmpF25,sizeof(void*),0)));}
if(!Cyc_Tcutil_coerce_uint_typ(te,(struct Cyc_Absyn_Exp*)_tmp858)){const char*
_tmpF29;void*_tmpF28;(_tmpF28=0,Cyc_Tcutil_terr(loc,((_tmpF29="array bounds expression is not an unsigned int",
_tag_dyneither(_tmpF29,sizeof(char),47))),_tag_dyneither(_tmpF28,sizeof(void*),0)));}{
unsigned int _tmp865;int _tmp866;struct _tuple7 _tmp864=Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)_tmp858);_tmp865=_tmp864.f1;_tmp866=_tmp864.f2;if((
is_zero_terminated  && _tmp866) && _tmp865 < 1){const char*_tmpF2C;void*_tmpF2B;(
_tmpF2B=0,Cyc_Tcutil_warn(loc,((_tmpF2C="zero terminated array cannot have zero elements",
_tag_dyneither(_tmpF2C,sizeof(char),48))),_tag_dyneither(_tmpF2B,sizeof(void*),0)));}
if((_tmp866  && _tmp865 < 1) && Cyc_Cyclone_tovc_r){{const char*_tmpF2F;void*
_tmpF2E;(_tmpF2E=0,Cyc_Tcutil_warn(loc,((_tmpF2F="arrays with 0 elements are not supported except with gcc -- changing to 1.",
_tag_dyneither(_tmpF2F,sizeof(char),75))),_tag_dyneither(_tmpF2E,sizeof(void*),0)));}*
_tmp7A3=(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(1,0);}goto _LL4DC;}}}_LL4F9: if(*((
int*)_tmp77E)!= 8)goto _LL4FB;_tmp7A6=((struct Cyc_Absyn_FnType_struct*)_tmp77E)->f1;
_tmp7A7=_tmp7A6.tvars;_tmp7A8=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_struct*)
_tmp77E)->f1).tvars;_tmp7A9=_tmp7A6.effect;_tmp7AA=(struct Cyc_Core_Opt**)&(((
struct Cyc_Absyn_FnType_struct*)_tmp77E)->f1).effect;_tmp7AB=(void*)_tmp7A6.ret_typ;
_tmp7AC=_tmp7A6.args;_tmp7AD=_tmp7A6.c_varargs;_tmp7AE=_tmp7A6.cyc_varargs;
_tmp7AF=_tmp7A6.rgn_po;_tmp7B0=_tmp7A6.attributes;_LL4FA: {int num_convs=0;int
seen_cdecl=0;int seen_stdcall=0;int seen_fastcall=0;int seen_format=0;void*ft=(void*)
0;int fmt_desc_arg=- 1;int fmt_arg_start=- 1;for(0;_tmp7B0 != 0;_tmp7B0=_tmp7B0->tl){
if(!Cyc_Absyn_fntype_att((void*)_tmp7B0->hd)){const char*_tmpF33;void*_tmpF32[1];
struct Cyc_String_pa_struct _tmpF31;(_tmpF31.tag=0,((_tmpF31.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((void*)_tmp7B0->hd)),((
_tmpF32[0]=& _tmpF31,Cyc_Tcutil_terr(loc,((_tmpF33="bad function type attribute %s",
_tag_dyneither(_tmpF33,sizeof(char),31))),_tag_dyneither(_tmpF32,sizeof(void*),1)))))));}{
void*_tmp86E=(void*)_tmp7B0->hd;void*_tmp86F;int _tmp870;int _tmp871;_LL55B: if((
int)_tmp86E != 0)goto _LL55D;_LL55C: if(!seen_stdcall){seen_stdcall=1;++ num_convs;}
goto _LL55A;_LL55D: if((int)_tmp86E != 1)goto _LL55F;_LL55E: if(!seen_cdecl){
seen_cdecl=1;++ num_convs;}goto _LL55A;_LL55F: if((int)_tmp86E != 2)goto _LL561;
_LL560: if(!seen_fastcall){seen_fastcall=1;++ num_convs;}goto _LL55A;_LL561: if(
_tmp86E <= (void*)17)goto _LL563;if(*((int*)_tmp86E)!= 3)goto _LL563;_tmp86F=(void*)((
struct Cyc_Absyn_Format_att_struct*)_tmp86E)->f1;_tmp870=((struct Cyc_Absyn_Format_att_struct*)
_tmp86E)->f2;_tmp871=((struct Cyc_Absyn_Format_att_struct*)_tmp86E)->f3;_LL562:
if(!seen_format){seen_format=1;ft=_tmp86F;fmt_desc_arg=_tmp870;fmt_arg_start=
_tmp871;}else{const char*_tmpF36;void*_tmpF35;(_tmpF35=0,Cyc_Tcutil_terr(loc,((
_tmpF36="function can't have multiple format attributes",_tag_dyneither(_tmpF36,
sizeof(char),47))),_tag_dyneither(_tmpF35,sizeof(void*),0)));}goto _LL55A;_LL563:;
_LL564: goto _LL55A;_LL55A:;}}if(num_convs > 1){const char*_tmpF39;void*_tmpF38;(
_tmpF38=0,Cyc_Tcutil_terr(loc,((_tmpF39="function can't have multiple calling conventions",
_tag_dyneither(_tmpF39,sizeof(char),49))),_tag_dyneither(_tmpF38,sizeof(void*),0)));}
Cyc_Tcutil_check_unique_tvars(loc,*_tmp7A8);{struct Cyc_List_List*b=*_tmp7A8;for(
0;b != 0;b=b->tl){((struct Cyc_Absyn_Tvar*)b->hd)->identity=Cyc_Tcutil_new_tvar_id();
cvtenv.kind_env=Cyc_Tcutil_add_bound_tvar(cvtenv.kind_env,(struct Cyc_Absyn_Tvar*)
b->hd);{void*_tmp876=Cyc_Absyn_compress_kb((void*)((struct Cyc_Absyn_Tvar*)b->hd)->kind);
void*_tmp877;_LL566: if(*((int*)_tmp876)!= 0)goto _LL568;_tmp877=(void*)((struct
Cyc_Absyn_Eq_kb_struct*)_tmp876)->f1;if((int)_tmp877 != 1)goto _LL568;_LL567:{
const char*_tmpF3D;void*_tmpF3C[1];struct Cyc_String_pa_struct _tmpF3B;(_tmpF3B.tag=
0,((_tmpF3B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)
b->hd)->name),((_tmpF3C[0]=& _tmpF3B,Cyc_Tcutil_terr(loc,((_tmpF3D="function attempts to abstract Mem type variable %s",
_tag_dyneither(_tmpF3D,sizeof(char),51))),_tag_dyneither(_tmpF3C,sizeof(void*),1)))))));}
goto _LL565;_LL568:;_LL569: goto _LL565;_LL565:;}}}{struct _RegionHandle*_tmp87B=Cyc_Tcenv_get_fnrgn(
te);{struct Cyc_Tcutil_CVTEnv _tmpF3E;struct Cyc_Tcutil_CVTEnv _tmp87C=(_tmpF3E.r=
_tmp87B,((_tmpF3E.kind_env=cvtenv.kind_env,((_tmpF3E.free_vars=0,((_tmpF3E.free_evars=
0,((_tmpF3E.generalize_evars=cvtenv.generalize_evars,((_tmpF3E.fn_result=1,
_tmpF3E)))))))))));_tmp87C=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp87C,(void*)1,
_tmp7AB,1);_tmp87C.fn_result=0;{struct Cyc_List_List*a=_tmp7AC;for(0;a != 0;a=a->tl){
struct _tuple3*_tmp87D=(struct _tuple3*)a->hd;void*_tmp87E=(*_tmp87D).f3;_tmp87C=
Cyc_Tcutil_i_check_valid_type(loc,te,_tmp87C,(void*)1,_tmp87E,1);((*_tmp87D).f2).real_const=
Cyc_Tcutil_extract_const_from_typedef(loc,((*_tmp87D).f2).print_const,_tmp87E);}}
if(_tmp7AE != 0){if(_tmp7AD){const char*_tmpF41;void*_tmpF40;(_tmpF40=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpF41="both c_vararg and cyc_vararg",
_tag_dyneither(_tmpF41,sizeof(char),29))),_tag_dyneither(_tmpF40,sizeof(void*),0)));}{
void*_tmp882;int _tmp883;struct Cyc_Absyn_VarargInfo _tmp881=*_tmp7AE;_tmp882=(void*)
_tmp881.type;_tmp883=_tmp881.inject;_tmp87C=Cyc_Tcutil_i_check_valid_type(loc,te,
_tmp87C,(void*)1,_tmp882,1);(_tmp7AE->tq).real_const=Cyc_Tcutil_extract_const_from_typedef(
loc,(_tmp7AE->tq).print_const,_tmp882);if(_tmp883){void*_tmp884=Cyc_Tcutil_compress(
_tmp882);struct Cyc_Absyn_TunionInfo _tmp885;union Cyc_Absyn_TunionInfoU_union
_tmp886;struct Cyc_Absyn_Tuniondecl**_tmp887;_LL56B: if(_tmp884 <= (void*)4)goto
_LL56D;if(*((int*)_tmp884)!= 2)goto _LL56D;_tmp885=((struct Cyc_Absyn_TunionType_struct*)
_tmp884)->f1;_tmp886=_tmp885.tunion_info;if((((((struct Cyc_Absyn_TunionType_struct*)
_tmp884)->f1).tunion_info).KnownTunion).tag != 1)goto _LL56D;_tmp887=(_tmp886.KnownTunion).f1;
_LL56C: if((*_tmp887)->is_flat){const char*_tmpF44;void*_tmpF43;(_tmpF43=0,Cyc_Tcutil_terr(
loc,((_tmpF44="cant inject into a flat tunion",_tag_dyneither(_tmpF44,sizeof(
char),31))),_tag_dyneither(_tmpF43,sizeof(void*),0)));}goto _LL56A;_LL56D:;_LL56E:{
const char*_tmpF47;void*_tmpF46;(_tmpF46=0,Cyc_Tcutil_terr(loc,((_tmpF47="can't inject a non-[x]tunion type",
_tag_dyneither(_tmpF47,sizeof(char),34))),_tag_dyneither(_tmpF46,sizeof(void*),0)));}
goto _LL56A;_LL56A:;}}}if(seen_format){int _tmp88C=((int(*)(struct Cyc_List_List*x))
Cyc_List_length)(_tmp7AC);if((((fmt_desc_arg < 0  || fmt_desc_arg > _tmp88C) || 
fmt_arg_start < 0) || _tmp7AE == 0  && fmt_arg_start != 0) || _tmp7AE != 0  && 
fmt_arg_start != _tmp88C + 1){const char*_tmpF4A;void*_tmpF49;(_tmpF49=0,Cyc_Tcutil_terr(
loc,((_tmpF4A="bad format descriptor",_tag_dyneither(_tmpF4A,sizeof(char),22))),
_tag_dyneither(_tmpF49,sizeof(void*),0)));}else{void*_tmp890;struct _tuple3
_tmp88F=*((struct _tuple3*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp7AC,
fmt_desc_arg - 1);_tmp890=_tmp88F.f3;{void*_tmp891=Cyc_Tcutil_compress(_tmp890);
struct Cyc_Absyn_PtrInfo _tmp892;void*_tmp893;struct Cyc_Absyn_PtrAtts _tmp894;
struct Cyc_Absyn_Conref*_tmp895;struct Cyc_Absyn_Conref*_tmp896;_LL570: if(_tmp891
<= (void*)4)goto _LL572;if(*((int*)_tmp891)!= 4)goto _LL572;_tmp892=((struct Cyc_Absyn_PointerType_struct*)
_tmp891)->f1;_tmp893=(void*)_tmp892.elt_typ;_tmp894=_tmp892.ptr_atts;_tmp895=
_tmp894.bounds;_tmp896=_tmp894.zero_term;_LL571:{struct _tuple0 _tmpF4B;struct
_tuple0 _tmp898=(_tmpF4B.f1=Cyc_Tcutil_compress(_tmp893),((_tmpF4B.f2=Cyc_Absyn_conref_def((
void*)((void*)0),_tmp895),_tmpF4B)));void*_tmp899;void*_tmp89A;void*_tmp89B;void*
_tmp89C;_LL575: _tmp899=_tmp898.f1;if(_tmp899 <= (void*)4)goto _LL577;if(*((int*)
_tmp899)!= 5)goto _LL577;_tmp89A=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp899)->f1;
if((int)_tmp89A != 2)goto _LL577;_tmp89B=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp899)->f2;if((int)_tmp89B != 0)goto _LL577;_tmp89C=_tmp898.f2;if((int)_tmp89C != 
0)goto _LL577;_LL576: goto _LL574;_LL577:;_LL578:{const char*_tmpF4E;void*_tmpF4D;(
_tmpF4D=0,Cyc_Tcutil_terr(loc,((_tmpF4E="format descriptor is not a char ? type",
_tag_dyneither(_tmpF4E,sizeof(char),39))),_tag_dyneither(_tmpF4D,sizeof(void*),0)));}
goto _LL574;_LL574:;}goto _LL56F;_LL572:;_LL573:{const char*_tmpF51;void*_tmpF50;(
_tmpF50=0,Cyc_Tcutil_terr(loc,((_tmpF51="format descriptor is not a char ? type",
_tag_dyneither(_tmpF51,sizeof(char),39))),_tag_dyneither(_tmpF50,sizeof(void*),0)));}
goto _LL56F;_LL56F:;}if(fmt_arg_start != 0){int problem;{struct _tuple0 _tmpF52;
struct _tuple0 _tmp8A2=(_tmpF52.f1=ft,((_tmpF52.f2=Cyc_Tcutil_compress((void*)((
struct Cyc_Absyn_VarargInfo*)_check_null(_tmp7AE))->type),_tmpF52)));void*_tmp8A3;
void*_tmp8A4;struct Cyc_Absyn_TunionInfo _tmp8A5;union Cyc_Absyn_TunionInfoU_union
_tmp8A6;struct Cyc_Absyn_Tuniondecl**_tmp8A7;struct Cyc_Absyn_Tuniondecl*_tmp8A8;
void*_tmp8A9;void*_tmp8AA;struct Cyc_Absyn_TunionInfo _tmp8AB;union Cyc_Absyn_TunionInfoU_union
_tmp8AC;struct Cyc_Absyn_Tuniondecl**_tmp8AD;struct Cyc_Absyn_Tuniondecl*_tmp8AE;
_LL57A: _tmp8A3=_tmp8A2.f1;if((int)_tmp8A3 != 0)goto _LL57C;_tmp8A4=_tmp8A2.f2;if(
_tmp8A4 <= (void*)4)goto _LL57C;if(*((int*)_tmp8A4)!= 2)goto _LL57C;_tmp8A5=((
struct Cyc_Absyn_TunionType_struct*)_tmp8A4)->f1;_tmp8A6=_tmp8A5.tunion_info;if((((((
struct Cyc_Absyn_TunionType_struct*)_tmp8A4)->f1).tunion_info).KnownTunion).tag != 
1)goto _LL57C;_tmp8A7=(_tmp8A6.KnownTunion).f1;_tmp8A8=*_tmp8A7;_LL57B: problem=
Cyc_Absyn_qvar_cmp(_tmp8A8->name,Cyc_Absyn_tunion_print_arg_qvar)!= 0;goto _LL579;
_LL57C: _tmp8A9=_tmp8A2.f1;if((int)_tmp8A9 != 1)goto _LL57E;_tmp8AA=_tmp8A2.f2;if(
_tmp8AA <= (void*)4)goto _LL57E;if(*((int*)_tmp8AA)!= 2)goto _LL57E;_tmp8AB=((
struct Cyc_Absyn_TunionType_struct*)_tmp8AA)->f1;_tmp8AC=_tmp8AB.tunion_info;if((((((
struct Cyc_Absyn_TunionType_struct*)_tmp8AA)->f1).tunion_info).KnownTunion).tag != 
1)goto _LL57E;_tmp8AD=(_tmp8AC.KnownTunion).f1;_tmp8AE=*_tmp8AD;_LL57D: problem=
Cyc_Absyn_qvar_cmp(_tmp8AE->name,Cyc_Absyn_tunion_scanf_arg_qvar)!= 0;goto _LL579;
_LL57E:;_LL57F: problem=1;goto _LL579;_LL579:;}if(problem){const char*_tmpF55;void*
_tmpF54;(_tmpF54=0,Cyc_Tcutil_terr(loc,((_tmpF55="format attribute and vararg types don't match",
_tag_dyneither(_tmpF55,sizeof(char),46))),_tag_dyneither(_tmpF54,sizeof(void*),0)));}}}}{
struct Cyc_List_List*rpo=_tmp7AF;for(0;rpo != 0;rpo=rpo->tl){struct _tuple0 _tmp8B2;
void*_tmp8B3;void*_tmp8B4;struct _tuple0*_tmp8B1=(struct _tuple0*)rpo->hd;_tmp8B2=*
_tmp8B1;_tmp8B3=_tmp8B2.f1;_tmp8B4=_tmp8B2.f2;_tmp87C=Cyc_Tcutil_i_check_valid_type(
loc,te,_tmp87C,(void*)6,_tmp8B3,1);_tmp87C=Cyc_Tcutil_i_check_valid_type(loc,te,
_tmp87C,(void*)5,_tmp8B4,1);}}if(*_tmp7AA != 0)_tmp87C=Cyc_Tcutil_i_check_valid_type(
loc,te,_tmp87C,(void*)6,(void*)((struct Cyc_Core_Opt*)_check_null(*_tmp7AA))->v,1);
else{struct Cyc_List_List*effect=0;{struct Cyc_List_List*tvs=_tmp87C.free_vars;
for(0;tvs != 0;tvs=tvs->tl){struct Cyc_Absyn_Tvar*_tmp8B6;int _tmp8B7;struct
_tuple18 _tmp8B5=*((struct _tuple18*)tvs->hd);_tmp8B6=_tmp8B5.f1;_tmp8B7=_tmp8B5.f2;
if(!_tmp8B7)continue;{void*_tmp8B8=Cyc_Absyn_compress_kb((void*)_tmp8B6->kind);
struct Cyc_Core_Opt*_tmp8B9;struct Cyc_Core_Opt**_tmp8BA;void*_tmp8BB;struct Cyc_Core_Opt*
_tmp8BC;struct Cyc_Core_Opt**_tmp8BD;void*_tmp8BE;void*_tmp8BF;void*_tmp8C0;void*
_tmp8C1;struct Cyc_Core_Opt*_tmp8C2;struct Cyc_Core_Opt**_tmp8C3;void*_tmp8C4;void*
_tmp8C5;struct Cyc_Core_Opt*_tmp8C6;struct Cyc_Core_Opt**_tmp8C7;_LL581: if(*((int*)
_tmp8B8)!= 2)goto _LL583;_tmp8B9=((struct Cyc_Absyn_Less_kb_struct*)_tmp8B8)->f1;
_tmp8BA=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)_tmp8B8)->f1;
_tmp8BB=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp8B8)->f2;if((int)_tmp8BB != 
5)goto _LL583;_LL582: _tmp8BD=_tmp8BA;_tmp8BE=(void*)3;goto _LL584;_LL583: if(*((int*)
_tmp8B8)!= 2)goto _LL585;_tmp8BC=((struct Cyc_Absyn_Less_kb_struct*)_tmp8B8)->f1;
_tmp8BD=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)_tmp8B8)->f1;
_tmp8BE=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp8B8)->f2;if(!(_tmp8BE == (
void*)3  || _tmp8BE == (void*)4))goto _LL585;_LL584:*_tmp8BD=Cyc_Tcutil_kind_to_bound_opt(
_tmp8BE);_tmp8BF=_tmp8BE;goto _LL586;_LL585: if(*((int*)_tmp8B8)!= 0)goto _LL587;
_tmp8BF=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp8B8)->f1;if(!((_tmp8BF == (
void*)3  || _tmp8BF == (void*)4) || _tmp8BF == (void*)5))goto _LL587;_LL586:{struct
Cyc_Absyn_AccessEff_struct*_tmpF64;struct Cyc_Absyn_VarType_struct*_tmpF63;struct
Cyc_Absyn_VarType_struct _tmpF62;struct Cyc_Absyn_AccessEff_struct _tmpF61;struct
Cyc_List_List*_tmpF60;effect=((_tmpF60=_cycalloc(sizeof(*_tmpF60)),((_tmpF60->hd=(
void*)((void*)((_tmpF64=_cycalloc(sizeof(*_tmpF64)),((_tmpF64[0]=((_tmpF61.tag=
19,((_tmpF61.f1=(void*)((void*)((_tmpF63=_cycalloc(sizeof(*_tmpF63)),((_tmpF63[0]=((
_tmpF62.tag=1,((_tmpF62.f1=_tmp8B6,_tmpF62)))),_tmpF63))))),_tmpF61)))),_tmpF64))))),((
_tmpF60->tl=effect,_tmpF60))))));}goto _LL580;_LL587: if(*((int*)_tmp8B8)!= 2)goto
_LL589;_tmp8C0=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp8B8)->f2;if((int)
_tmp8C0 != 7)goto _LL589;_LL588: goto _LL58A;_LL589: if(*((int*)_tmp8B8)!= 0)goto
_LL58B;_tmp8C1=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp8B8)->f1;if((int)
_tmp8C1 != 7)goto _LL58B;_LL58A: goto _LL580;_LL58B: if(*((int*)_tmp8B8)!= 2)goto
_LL58D;_tmp8C2=((struct Cyc_Absyn_Less_kb_struct*)_tmp8B8)->f1;_tmp8C3=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*)_tmp8B8)->f1;_tmp8C4=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp8B8)->f2;if((int)_tmp8C4 != 6)goto _LL58D;_LL58C:*_tmp8C3=Cyc_Tcutil_kind_to_bound_opt((
void*)6);goto _LL58E;_LL58D: if(*((int*)_tmp8B8)!= 0)goto _LL58F;_tmp8C5=(void*)((
struct Cyc_Absyn_Eq_kb_struct*)_tmp8B8)->f1;if((int)_tmp8C5 != 6)goto _LL58F;_LL58E:{
struct Cyc_Absyn_VarType_struct*_tmpF6A;struct Cyc_Absyn_VarType_struct _tmpF69;
struct Cyc_List_List*_tmpF68;effect=((_tmpF68=_cycalloc(sizeof(*_tmpF68)),((
_tmpF68->hd=(void*)((void*)((_tmpF6A=_cycalloc(sizeof(*_tmpF6A)),((_tmpF6A[0]=((
_tmpF69.tag=1,((_tmpF69.f1=_tmp8B6,_tmpF69)))),_tmpF6A))))),((_tmpF68->tl=effect,
_tmpF68))))));}goto _LL580;_LL58F: if(*((int*)_tmp8B8)!= 1)goto _LL591;_tmp8C6=((
struct Cyc_Absyn_Unknown_kb_struct*)_tmp8B8)->f1;_tmp8C7=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Unknown_kb_struct*)_tmp8B8)->f1;_LL590:{struct Cyc_Absyn_Less_kb_struct*
_tmpF70;struct Cyc_Absyn_Less_kb_struct _tmpF6F;struct Cyc_Core_Opt*_tmpF6E;*
_tmp8C7=((_tmpF6E=_cycalloc(sizeof(*_tmpF6E)),((_tmpF6E->v=(void*)((void*)((
_tmpF70=_cycalloc(sizeof(*_tmpF70)),((_tmpF70[0]=((_tmpF6F.tag=2,((_tmpF6F.f1=0,((
_tmpF6F.f2=(void*)((void*)0),_tmpF6F)))))),_tmpF70))))),_tmpF6E))));}goto _LL592;
_LL591:;_LL592:{struct Cyc_Absyn_RgnsEff_struct*_tmpF7F;struct Cyc_Absyn_VarType_struct*
_tmpF7E;struct Cyc_Absyn_VarType_struct _tmpF7D;struct Cyc_Absyn_RgnsEff_struct
_tmpF7C;struct Cyc_List_List*_tmpF7B;effect=((_tmpF7B=_cycalloc(sizeof(*_tmpF7B)),((
_tmpF7B->hd=(void*)((void*)((_tmpF7F=_cycalloc(sizeof(*_tmpF7F)),((_tmpF7F[0]=((
_tmpF7C.tag=21,((_tmpF7C.f1=(void*)((void*)((_tmpF7E=_cycalloc(sizeof(*_tmpF7E)),((
_tmpF7E[0]=((_tmpF7D.tag=1,((_tmpF7D.f1=_tmp8B6,_tmpF7D)))),_tmpF7E))))),_tmpF7C)))),
_tmpF7F))))),((_tmpF7B->tl=effect,_tmpF7B))))));}goto _LL580;_LL580:;}}}{struct
Cyc_List_List*ts=_tmp87C.free_evars;for(0;ts != 0;ts=ts->tl){void*_tmp8D9;int
_tmp8DA;struct _tuple19 _tmp8D8=*((struct _tuple19*)ts->hd);_tmp8D9=_tmp8D8.f1;
_tmp8DA=_tmp8D8.f2;if(!_tmp8DA)continue;{void*_tmp8DB=Cyc_Tcutil_typ_kind(
_tmp8D9);_LL594: if((int)_tmp8DB != 5)goto _LL596;_LL595: goto _LL597;_LL596: if((int)
_tmp8DB != 4)goto _LL598;_LL597: goto _LL599;_LL598: if((int)_tmp8DB != 3)goto _LL59A;
_LL599:{struct Cyc_Absyn_AccessEff_struct*_tmpF85;struct Cyc_Absyn_AccessEff_struct
_tmpF84;struct Cyc_List_List*_tmpF83;effect=((_tmpF83=_cycalloc(sizeof(*_tmpF83)),((
_tmpF83->hd=(void*)((void*)((_tmpF85=_cycalloc(sizeof(*_tmpF85)),((_tmpF85[0]=((
_tmpF84.tag=19,((_tmpF84.f1=(void*)_tmp8D9,_tmpF84)))),_tmpF85))))),((_tmpF83->tl=
effect,_tmpF83))))));}goto _LL593;_LL59A: if((int)_tmp8DB != 6)goto _LL59C;_LL59B:{
struct Cyc_List_List*_tmpF86;effect=((_tmpF86=_cycalloc(sizeof(*_tmpF86)),((
_tmpF86->hd=(void*)_tmp8D9,((_tmpF86->tl=effect,_tmpF86))))));}goto _LL593;_LL59C:
if((int)_tmp8DB != 7)goto _LL59E;_LL59D: goto _LL593;_LL59E:;_LL59F:{struct Cyc_Absyn_RgnsEff_struct*
_tmpF8C;struct Cyc_Absyn_RgnsEff_struct _tmpF8B;struct Cyc_List_List*_tmpF8A;effect=((
_tmpF8A=_cycalloc(sizeof(*_tmpF8A)),((_tmpF8A->hd=(void*)((void*)((_tmpF8C=
_cycalloc(sizeof(*_tmpF8C)),((_tmpF8C[0]=((_tmpF8B.tag=21,((_tmpF8B.f1=(void*)
_tmp8D9,_tmpF8B)))),_tmpF8C))))),((_tmpF8A->tl=effect,_tmpF8A))))));}goto _LL593;
_LL593:;}}}{struct Cyc_Absyn_JoinEff_struct*_tmpF92;struct Cyc_Absyn_JoinEff_struct
_tmpF91;struct Cyc_Core_Opt*_tmpF90;*_tmp7AA=((_tmpF90=_cycalloc(sizeof(*_tmpF90)),((
_tmpF90->v=(void*)((void*)((_tmpF92=_cycalloc(sizeof(*_tmpF92)),((_tmpF92[0]=((
_tmpF91.tag=20,((_tmpF91.f1=effect,_tmpF91)))),_tmpF92))))),_tmpF90))));}}if(*
_tmp7A8 != 0){struct Cyc_List_List*bs=*_tmp7A8;for(0;bs != 0;bs=bs->tl){void*
_tmp8E6=Cyc_Absyn_compress_kb((void*)((struct Cyc_Absyn_Tvar*)bs->hd)->kind);
struct Cyc_Core_Opt*_tmp8E7;struct Cyc_Core_Opt**_tmp8E8;struct Cyc_Core_Opt*
_tmp8E9;struct Cyc_Core_Opt**_tmp8EA;void*_tmp8EB;struct Cyc_Core_Opt*_tmp8EC;
struct Cyc_Core_Opt**_tmp8ED;void*_tmp8EE;struct Cyc_Core_Opt*_tmp8EF;struct Cyc_Core_Opt**
_tmp8F0;void*_tmp8F1;struct Cyc_Core_Opt*_tmp8F2;struct Cyc_Core_Opt**_tmp8F3;void*
_tmp8F4;void*_tmp8F5;_LL5A1: if(*((int*)_tmp8E6)!= 1)goto _LL5A3;_tmp8E7=((struct
Cyc_Absyn_Unknown_kb_struct*)_tmp8E6)->f1;_tmp8E8=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Unknown_kb_struct*)_tmp8E6)->f1;_LL5A2:{const char*_tmpF96;void*
_tmpF95[1];struct Cyc_String_pa_struct _tmpF94;(_tmpF94.tag=0,((_tmpF94.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)bs->hd)->name),((
_tmpF95[0]=& _tmpF94,Cyc_Tcutil_warn(loc,((_tmpF96="Type variable %s unconstrained, assuming boxed",
_tag_dyneither(_tmpF96,sizeof(char),47))),_tag_dyneither(_tmpF95,sizeof(void*),1)))))));}
_tmp8EA=_tmp8E8;goto _LL5A4;_LL5A3: if(*((int*)_tmp8E6)!= 2)goto _LL5A5;_tmp8E9=((
struct Cyc_Absyn_Less_kb_struct*)_tmp8E6)->f1;_tmp8EA=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*)_tmp8E6)->f1;_tmp8EB=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp8E6)->f2;if((int)_tmp8EB != 1)goto _LL5A5;_LL5A4: _tmp8ED=_tmp8EA;goto _LL5A6;
_LL5A5: if(*((int*)_tmp8E6)!= 2)goto _LL5A7;_tmp8EC=((struct Cyc_Absyn_Less_kb_struct*)
_tmp8E6)->f1;_tmp8ED=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp8E6)->f1;_tmp8EE=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp8E6)->f2;if((
int)_tmp8EE != 0)goto _LL5A7;_LL5A6:*_tmp8ED=Cyc_Tcutil_kind_to_bound_opt((void*)2);
goto _LL5A0;_LL5A7: if(*((int*)_tmp8E6)!= 2)goto _LL5A9;_tmp8EF=((struct Cyc_Absyn_Less_kb_struct*)
_tmp8E6)->f1;_tmp8F0=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp8E6)->f1;_tmp8F1=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp8E6)->f2;if((
int)_tmp8F1 != 5)goto _LL5A9;_LL5A8:*_tmp8F0=Cyc_Tcutil_kind_to_bound_opt((void*)3);
goto _LL5A0;_LL5A9: if(*((int*)_tmp8E6)!= 2)goto _LL5AB;_tmp8F2=((struct Cyc_Absyn_Less_kb_struct*)
_tmp8E6)->f1;_tmp8F3=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp8E6)->f1;_tmp8F4=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp8E6)->f2;
_LL5AA:*_tmp8F3=Cyc_Tcutil_kind_to_bound_opt(_tmp8F4);goto _LL5A0;_LL5AB: if(*((
int*)_tmp8E6)!= 0)goto _LL5AD;_tmp8F5=(void*)((struct Cyc_Absyn_Eq_kb_struct*)
_tmp8E6)->f1;if((int)_tmp8F5 != 1)goto _LL5AD;_LL5AC:{const char*_tmpF99;void*
_tmpF98;(_tmpF98=0,Cyc_Tcutil_terr(loc,((_tmpF99="functions can't abstract M types",
_tag_dyneither(_tmpF99,sizeof(char),33))),_tag_dyneither(_tmpF98,sizeof(void*),0)));}
goto _LL5A0;_LL5AD:;_LL5AE: goto _LL5A0;_LL5A0:;}}cvtenv.kind_env=Cyc_Tcutil_remove_bound_tvars(
Cyc_Core_heap_region,_tmp87C.kind_env,*_tmp7A8);_tmp87C.free_vars=Cyc_Tcutil_remove_bound_tvars_bool(
_tmp87C.r,_tmp87C.free_vars,*_tmp7A8);{struct Cyc_List_List*tvs=_tmp87C.free_vars;
for(0;tvs != 0;tvs=tvs->tl){struct Cyc_Absyn_Tvar*_tmp8FC;int _tmp8FD;struct
_tuple18 _tmp8FB=*((struct _tuple18*)tvs->hd);_tmp8FC=_tmp8FB.f1;_tmp8FD=_tmp8FB.f2;
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,
_tmp8FC,_tmp8FD);}}{struct Cyc_List_List*evs=_tmp87C.free_evars;for(0;evs != 0;evs=
evs->tl){void*_tmp8FF;int _tmp900;struct _tuple19 _tmp8FE=*((struct _tuple19*)evs->hd);
_tmp8FF=_tmp8FE.f1;_tmp900=_tmp8FE.f2;cvtenv.free_evars=Cyc_Tcutil_add_free_evar(
cvtenv.r,cvtenv.free_evars,_tmp8FF,_tmp900);}}}goto _LL4DC;}}_LL4FB: if(*((int*)
_tmp77E)!= 9)goto _LL4FD;_tmp7B1=((struct Cyc_Absyn_TupleType_struct*)_tmp77E)->f1;
_LL4FC: for(0;_tmp7B1 != 0;_tmp7B1=_tmp7B1->tl){struct _tuple5*_tmp902=(struct
_tuple5*)_tmp7B1->hd;cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)1,(*
_tmp902).f2,1);((*_tmp902).f1).real_const=Cyc_Tcutil_extract_const_from_typedef(
loc,((*_tmp902).f1).print_const,(*_tmp902).f2);}goto _LL4DC;_LL4FD: if(*((int*)
_tmp77E)!= 11)goto _LL4FF;_tmp7B2=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp77E)->f1;_tmp7B3=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp77E)->f2;_LL4FE: {
struct _RegionHandle*_tmp903=Cyc_Tcenv_get_fnrgn(te);{struct Cyc_List_List*
prev_fields=0;for(0;_tmp7B3 != 0;_tmp7B3=_tmp7B3->tl){struct Cyc_Absyn_Aggrfield
_tmp905;struct _dyneither_ptr*_tmp906;struct Cyc_Absyn_Tqual _tmp907;struct Cyc_Absyn_Tqual*
_tmp908;void*_tmp909;struct Cyc_Absyn_Exp*_tmp90A;struct Cyc_List_List*_tmp90B;
struct Cyc_Absyn_Aggrfield*_tmp904=(struct Cyc_Absyn_Aggrfield*)_tmp7B3->hd;
_tmp905=*_tmp904;_tmp906=_tmp905.name;_tmp907=_tmp905.tq;_tmp908=(struct Cyc_Absyn_Tqual*)&(*
_tmp904).tq;_tmp909=(void*)_tmp905.type;_tmp90A=_tmp905.width;_tmp90B=_tmp905.attributes;
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*
l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,_tmp906)){
const char*_tmpF9D;void*_tmpF9C[1];struct Cyc_String_pa_struct _tmpF9B;(_tmpF9B.tag=
0,((_tmpF9B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp906),((_tmpF9C[
0]=& _tmpF9B,Cyc_Tcutil_terr(loc,((_tmpF9D="duplicate field %s",_tag_dyneither(
_tmpF9D,sizeof(char),19))),_tag_dyneither(_tmpF9C,sizeof(void*),1)))))));}{const
char*_tmpF9E;if(Cyc_strcmp((struct _dyneither_ptr)*_tmp906,((_tmpF9E="",
_tag_dyneither(_tmpF9E,sizeof(char),1))))!= 0){struct Cyc_List_List*_tmpF9F;
prev_fields=((_tmpF9F=_region_malloc(_tmp903,sizeof(*_tmpF9F)),((_tmpF9F->hd=
_tmp906,((_tmpF9F->tl=prev_fields,_tmpF9F))))));}}cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,(void*)1,_tmp909,1);_tmp908->real_const=Cyc_Tcutil_extract_const_from_typedef(
loc,_tmp908->print_const,_tmp909);if(_tmp7B2 == (void*)1  && !Cyc_Tcutil_bits_only(
_tmp909)){const char*_tmpFA3;void*_tmpFA2[1];struct Cyc_String_pa_struct _tmpFA1;(
_tmpFA1.tag=0,((_tmpFA1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp906),((
_tmpFA2[0]=& _tmpFA1,Cyc_Tcutil_warn(loc,((_tmpFA3="union member %s is not `bits-only' so it can only be written and not read",
_tag_dyneither(_tmpFA3,sizeof(char),74))),_tag_dyneither(_tmpFA2,sizeof(void*),1)))))));}
Cyc_Tcutil_check_bitfield(loc,te,_tmp909,_tmp90A,_tmp906);Cyc_Tcutil_check_field_atts(
loc,_tmp906,_tmp90B);}}goto _LL4DC;}_LL4FF: if(*((int*)_tmp77E)!= 10)goto _LL501;
_tmp7B4=((struct Cyc_Absyn_AggrType_struct*)_tmp77E)->f1;_tmp7B5=_tmp7B4.aggr_info;
_tmp7B6=(union Cyc_Absyn_AggrInfoU_union*)&(((struct Cyc_Absyn_AggrType_struct*)
_tmp77E)->f1).aggr_info;_tmp7B7=_tmp7B4.targs;_tmp7B8=(struct Cyc_List_List**)&(((
struct Cyc_Absyn_AggrType_struct*)_tmp77E)->f1).targs;_LL500:{union Cyc_Absyn_AggrInfoU_union
_tmp914=*_tmp7B6;void*_tmp915;struct _tuple2*_tmp916;struct Cyc_Absyn_Aggrdecl**
_tmp917;struct Cyc_Absyn_Aggrdecl*_tmp918;_LL5B0: if((_tmp914.UnknownAggr).tag != 0)
goto _LL5B2;_tmp915=(_tmp914.UnknownAggr).f1;_tmp916=(_tmp914.UnknownAggr).f2;
_LL5B1: {struct Cyc_Absyn_Aggrdecl**adp;{struct _handler_cons _tmp919;_push_handler(&
_tmp919);{int _tmp91B=0;if(setjmp(_tmp919.handler))_tmp91B=1;if(!_tmp91B){adp=Cyc_Tcenv_lookup_aggrdecl(
te,loc,_tmp916);{union Cyc_Absyn_AggrInfoU_union _tmpFA4;*_tmp7B6=(union Cyc_Absyn_AggrInfoU_union)(((
_tmpFA4.KnownAggr).tag=1,(((_tmpFA4.KnownAggr).f1=adp,_tmpFA4))));};_pop_handler();}
else{void*_tmp91A=(void*)_exn_thrown;void*_tmp91E=_tmp91A;_LL5B5: if(_tmp91E != 
Cyc_Dict_Absent)goto _LL5B7;_LL5B6: {struct Cyc_Tcenv_Genv*_tmp91F=((struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct Cyc_Absyn_Aggrdecl*_tmpFA5;struct Cyc_Absyn_Aggrdecl*_tmp920=(_tmpFA5=
_cycalloc(sizeof(*_tmpFA5)),((_tmpFA5->kind=(void*)_tmp915,((_tmpFA5->sc=(void*)((
void*)3),((_tmpFA5->name=_tmp916,((_tmpFA5->tvs=0,((_tmpFA5->impl=0,((_tmpFA5->attributes=
0,_tmpFA5)))))))))))));Cyc_Tc_tcAggrdecl(te,_tmp91F,loc,_tmp920);adp=Cyc_Tcenv_lookup_aggrdecl(
te,loc,_tmp916);{union Cyc_Absyn_AggrInfoU_union _tmpFA6;*_tmp7B6=(union Cyc_Absyn_AggrInfoU_union)(((
_tmpFA6.KnownAggr).tag=1,(((_tmpFA6.KnownAggr).f1=adp,_tmpFA6))));}if(*_tmp7B8 != 
0){{const char*_tmpFAA;void*_tmpFA9[1];struct Cyc_String_pa_struct _tmpFA8;(_tmpFA8.tag=
0,((_tmpFA8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp916)),((_tmpFA9[0]=& _tmpFA8,Cyc_Tcutil_terr(loc,((_tmpFAA="declare parameterized type %s before using",
_tag_dyneither(_tmpFAA,sizeof(char),43))),_tag_dyneither(_tmpFA9,sizeof(void*),1)))))));}
return cvtenv;}goto _LL5B4;}_LL5B7:;_LL5B8:(void)_throw(_tmp91E);_LL5B4:;}}}
_tmp918=*adp;goto _LL5B3;}_LL5B2: if((_tmp914.KnownAggr).tag != 1)goto _LL5AF;
_tmp917=(_tmp914.KnownAggr).f1;_tmp918=*_tmp917;_LL5B3: {struct Cyc_List_List*tvs=
_tmp918->tvs;struct Cyc_List_List*ts=*_tmp7B8;for(0;ts != 0  && tvs != 0;(ts=ts->tl,
tvs=tvs->tl)){void*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd);cvtenv=
Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,(void*)ts->hd,1);}if(ts != 0){const
char*_tmpFAE;void*_tmpFAD[1];struct Cyc_String_pa_struct _tmpFAC;(_tmpFAC.tag=0,((
_tmpFAC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp918->name)),((_tmpFAD[0]=& _tmpFAC,Cyc_Tcutil_terr(loc,((_tmpFAE="too many parameters for type %s",
_tag_dyneither(_tmpFAE,sizeof(char),32))),_tag_dyneither(_tmpFAD,sizeof(void*),1)))))));}
if(tvs != 0){struct Cyc_List_List*hidden_ts=0;for(0;tvs != 0;tvs=tvs->tl){void*k=
Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd);void*e=Cyc_Absyn_new_evar(0,
0);{struct Cyc_List_List*_tmpFAF;hidden_ts=((_tmpFAF=_cycalloc(sizeof(*_tmpFAF)),((
_tmpFAF->hd=(void*)e,((_tmpFAF->tl=hidden_ts,_tmpFAF))))));}cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,k,e,1);}*_tmp7B8=Cyc_List_imp_append(*_tmp7B8,Cyc_List_imp_rev(
hidden_ts));}}_LL5AF:;}goto _LL4DC;_LL501: if(*((int*)_tmp77E)!= 16)goto _LL503;
_tmp7B9=((struct Cyc_Absyn_TypedefType_struct*)_tmp77E)->f1;_tmp7BA=((struct Cyc_Absyn_TypedefType_struct*)
_tmp77E)->f2;_tmp7BB=(struct Cyc_List_List**)&((struct Cyc_Absyn_TypedefType_struct*)
_tmp77E)->f2;_tmp7BC=((struct Cyc_Absyn_TypedefType_struct*)_tmp77E)->f3;_tmp7BD=(
struct Cyc_Absyn_Typedefdecl**)&((struct Cyc_Absyn_TypedefType_struct*)_tmp77E)->f3;
_tmp7BE=((struct Cyc_Absyn_TypedefType_struct*)_tmp77E)->f4;_tmp7BF=(void***)&((
struct Cyc_Absyn_TypedefType_struct*)_tmp77E)->f4;_LL502: {struct Cyc_List_List*
targs=*_tmp7BB;struct Cyc_Absyn_Typedefdecl*td;{struct _handler_cons _tmp92A;
_push_handler(& _tmp92A);{int _tmp92C=0;if(setjmp(_tmp92A.handler))_tmp92C=1;if(!
_tmp92C){td=Cyc_Tcenv_lookup_typedefdecl(te,loc,_tmp7B9);;_pop_handler();}else{
void*_tmp92B=(void*)_exn_thrown;void*_tmp92E=_tmp92B;_LL5BA: if(_tmp92E != Cyc_Dict_Absent)
goto _LL5BC;_LL5BB:{const char*_tmpFB3;void*_tmpFB2[1];struct Cyc_String_pa_struct
_tmpFB1;(_tmpFB1.tag=0,((_tmpFB1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp7B9)),((_tmpFB2[0]=& _tmpFB1,Cyc_Tcutil_terr(loc,((
_tmpFB3="unbound typedef name %s",_tag_dyneither(_tmpFB3,sizeof(char),24))),
_tag_dyneither(_tmpFB2,sizeof(void*),1)))))));}return cvtenv;_LL5BC:;_LL5BD:(void)
_throw(_tmp92E);_LL5B9:;}}}*_tmp7BD=(struct Cyc_Absyn_Typedefdecl*)td;_tmp7B9[0]=(
td->name)[_check_known_subscript_notnull(1,0)];{struct Cyc_List_List*tvs=td->tvs;
struct Cyc_List_List*ts=targs;struct _RegionHandle*_tmp932=Cyc_Tcenv_get_fnrgn(te);{
struct Cyc_List_List*inst=0;for(0;ts != 0  && tvs != 0;(ts=ts->tl,tvs=tvs->tl)){void*
k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd);cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,k,(void*)ts->hd,1);{struct _tuple8*_tmpFB6;struct Cyc_List_List*
_tmpFB5;inst=((_tmpFB5=_region_malloc(_tmp932,sizeof(*_tmpFB5)),((_tmpFB5->hd=((
_tmpFB6=_region_malloc(_tmp932,sizeof(*_tmpFB6)),((_tmpFB6->f1=(struct Cyc_Absyn_Tvar*)
tvs->hd,((_tmpFB6->f2=(void*)ts->hd,_tmpFB6)))))),((_tmpFB5->tl=inst,_tmpFB5))))));}}
if(ts != 0){const char*_tmpFBA;void*_tmpFB9[1];struct Cyc_String_pa_struct _tmpFB8;(
_tmpFB8.tag=0,((_tmpFB8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp7B9)),((_tmpFB9[0]=& _tmpFB8,Cyc_Tcutil_terr(loc,((_tmpFBA="too many parameters for typedef %s",
_tag_dyneither(_tmpFBA,sizeof(char),35))),_tag_dyneither(_tmpFB9,sizeof(void*),1)))))));}
if(tvs != 0){struct Cyc_List_List*hidden_ts=0;for(0;tvs != 0;tvs=tvs->tl){void*k=
Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd);void*e=Cyc_Absyn_new_evar(0,
0);{struct Cyc_List_List*_tmpFBB;hidden_ts=((_tmpFBB=_cycalloc(sizeof(*_tmpFBB)),((
_tmpFBB->hd=(void*)e,((_tmpFBB->tl=hidden_ts,_tmpFBB))))));}cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,k,e,1);{struct _tuple8*_tmpFBE;struct Cyc_List_List*_tmpFBD;inst=(
struct Cyc_List_List*)((_tmpFBD=_cycalloc(sizeof(*_tmpFBD)),((_tmpFBD->hd=(struct
_tuple8*)((_tmpFBE=_cycalloc(sizeof(*_tmpFBE)),((_tmpFBE->f1=(struct Cyc_Absyn_Tvar*)
tvs->hd,((_tmpFBE->f2=e,_tmpFBE)))))),((_tmpFBD->tl=inst,_tmpFBD))))));}}*
_tmp7BB=Cyc_List_imp_append(targs,Cyc_List_imp_rev(hidden_ts));}if(td->defn != 0){
void*new_typ=Cyc_Tcutil_rsubstitute(_tmp932,inst,(void*)((struct Cyc_Core_Opt*)
_check_null(td->defn))->v);void**_tmpFBF;*_tmp7BF=((_tmpFBF=_cycalloc(sizeof(*
_tmpFBF)),((_tmpFBF[0]=new_typ,_tmpFBF))));}}goto _LL4DC;}}_LL503: if((int)_tmp77E
!= 3)goto _LL505;_LL504: goto _LL506;_LL505: if((int)_tmp77E != 2)goto _LL507;_LL506:
goto _LL4DC;_LL507: if(_tmp77E <= (void*)4)goto _LL509;if(*((int*)_tmp77E)!= 14)goto
_LL509;_tmp7C0=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)_tmp77E)->f1;
_LL508: cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)5,_tmp7C0,1);
goto _LL4DC;_LL509: if(_tmp77E <= (void*)4)goto _LL50B;if(*((int*)_tmp77E)!= 15)goto
_LL50B;_tmp7C1=(void*)((struct Cyc_Absyn_DynRgnType_struct*)_tmp77E)->f1;_tmp7C2=(
void*)((struct Cyc_Absyn_DynRgnType_struct*)_tmp77E)->f2;_LL50A: cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,(void*)3,_tmp7C1,0);cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,
cvtenv,(void*)3,_tmp7C2,1);goto _LL4DC;_LL50B: if(_tmp77E <= (void*)4)goto _LL50D;
if(*((int*)_tmp77E)!= 19)goto _LL50D;_tmp7C3=(void*)((struct Cyc_Absyn_AccessEff_struct*)
_tmp77E)->f1;_LL50C: cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)5,
_tmp7C3,1);goto _LL4DC;_LL50D: if(_tmp77E <= (void*)4)goto _LL50F;if(*((int*)_tmp77E)
!= 21)goto _LL50F;_tmp7C4=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp77E)->f1;
_LL50E: cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)0,_tmp7C4,1);
goto _LL4DC;_LL50F: if(_tmp77E <= (void*)4)goto _LL4DC;if(*((int*)_tmp77E)!= 20)goto
_LL4DC;_tmp7C5=((struct Cyc_Absyn_JoinEff_struct*)_tmp77E)->f1;_LL510: for(0;
_tmp7C5 != 0;_tmp7C5=_tmp7C5->tl){cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,
cvtenv,(void*)6,(void*)_tmp7C5->hd,1);}goto _LL4DC;_LL4DC:;}if(!Cyc_Tcutil_kind_leq(
Cyc_Tcutil_typ_kind(t),expected_kind)){{void*_tmp93C=t;struct Cyc_Core_Opt*
_tmp93D;struct Cyc_Core_Opt*_tmp93E;_LL5BF: if(_tmp93C <= (void*)4)goto _LL5C1;if(*((
int*)_tmp93C)!= 0)goto _LL5C1;_tmp93D=((struct Cyc_Absyn_Evar_struct*)_tmp93C)->f1;
_tmp93E=((struct Cyc_Absyn_Evar_struct*)_tmp93C)->f2;_LL5C0: {struct
_dyneither_ptr s;{struct Cyc_Core_Opt*_tmp93F=_tmp93D;struct Cyc_Core_Opt _tmp940;
void*_tmp941;_LL5C4: if(_tmp93F != 0)goto _LL5C6;_LL5C5:{const char*_tmpFC0;s=((
_tmpFC0="kind=NULL ",_tag_dyneither(_tmpFC0,sizeof(char),11)));}goto _LL5C3;
_LL5C6: if(_tmp93F == 0)goto _LL5C3;_tmp940=*_tmp93F;_tmp941=(void*)_tmp940.v;
_LL5C7:{const char*_tmpFC4;void*_tmpFC3[1];struct Cyc_String_pa_struct _tmpFC2;s=(
struct _dyneither_ptr)((_tmpFC2.tag=0,((_tmpFC2.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_kind2string(_tmp941)),((_tmpFC3[0]=& _tmpFC2,Cyc_aprintf(((
_tmpFC4="kind=%s ",_tag_dyneither(_tmpFC4,sizeof(char),9))),_tag_dyneither(
_tmpFC3,sizeof(void*),1))))))));}goto _LL5C3;_LL5C3:;}{struct Cyc_Core_Opt*_tmp946=
_tmp93E;struct Cyc_Core_Opt _tmp947;void*_tmp948;_LL5C9: if(_tmp946 != 0)goto _LL5CB;
_LL5CA:{const char*_tmpFC8;void*_tmpFC7[1];struct Cyc_String_pa_struct _tmpFC6;s=(
struct _dyneither_ptr)((_tmpFC6.tag=0,((_tmpFC6.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)s),((_tmpFC7[0]=& _tmpFC6,Cyc_aprintf(((_tmpFC8="%s ref=NULL",
_tag_dyneither(_tmpFC8,sizeof(char),12))),_tag_dyneither(_tmpFC7,sizeof(void*),1))))))));}
goto _LL5C8;_LL5CB: if(_tmp946 == 0)goto _LL5C8;_tmp947=*_tmp946;_tmp948=(void*)
_tmp947.v;_LL5CC:{const char*_tmpFCD;void*_tmpFCC[2];struct Cyc_String_pa_struct
_tmpFCB;struct Cyc_String_pa_struct _tmpFCA;s=(struct _dyneither_ptr)((_tmpFCA.tag=
0,((_tmpFCA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
_tmp948)),((_tmpFCB.tag=0,((_tmpFCB.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)s),((_tmpFCC[0]=& _tmpFCB,((_tmpFCC[1]=& _tmpFCA,Cyc_aprintf(((
_tmpFCD="%s ref=%s",_tag_dyneither(_tmpFCD,sizeof(char),10))),_tag_dyneither(
_tmpFCC,sizeof(void*),2))))))))))))));}goto _LL5C8;_LL5C8:;}{const char*_tmpFD1;
void*_tmpFD0[1];struct Cyc_String_pa_struct _tmpFCF;(_tmpFCF.tag=0,((_tmpFCF.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmpFD0[0]=& _tmpFCF,Cyc_fprintf(
Cyc_stderr,((_tmpFD1="evar info: %s\n",_tag_dyneither(_tmpFD1,sizeof(char),15))),
_tag_dyneither(_tmpFD0,sizeof(void*),1)))))));}goto _LL5BE;}_LL5C1:;_LL5C2: goto
_LL5BE;_LL5BE:;}{const char*_tmpFD7;void*_tmpFD6[3];struct Cyc_String_pa_struct
_tmpFD5;struct Cyc_String_pa_struct _tmpFD4;struct Cyc_String_pa_struct _tmpFD3;(
_tmpFD3.tag=0,((_tmpFD3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(
expected_kind)),((_tmpFD4.tag=0,((_tmpFD4.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_kind2string(Cyc_Tcutil_typ_kind(t))),((_tmpFD5.tag=0,((
_tmpFD5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((
_tmpFD6[0]=& _tmpFD5,((_tmpFD6[1]=& _tmpFD4,((_tmpFD6[2]=& _tmpFD3,Cyc_Tcutil_terr(
loc,((_tmpFD7="type %s has kind %s but as used here needs kind %s",
_tag_dyneither(_tmpFD7,sizeof(char),51))),_tag_dyneither(_tmpFD6,sizeof(void*),3)))))))))))))))))));}}
return cvtenv;}static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type_level_exp(
struct Cyc_Absyn_Exp*e,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv);
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type_level_exp(struct Cyc_Absyn_Exp*
e,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv){{void*_tmp958=(void*)e->r;
struct Cyc_List_List*_tmp959;struct Cyc_Absyn_Exp*_tmp95A;struct Cyc_Absyn_Exp*
_tmp95B;struct Cyc_Absyn_Exp*_tmp95C;struct Cyc_Absyn_Exp*_tmp95D;struct Cyc_Absyn_Exp*
_tmp95E;struct Cyc_Absyn_Exp*_tmp95F;struct Cyc_Absyn_Exp*_tmp960;struct Cyc_Absyn_Exp*
_tmp961;struct Cyc_Absyn_Exp*_tmp962;void*_tmp963;struct Cyc_Absyn_Exp*_tmp964;
void*_tmp965;void*_tmp966;void*_tmp967;struct Cyc_Absyn_Exp*_tmp968;_LL5CE: if(*((
int*)_tmp958)!= 0)goto _LL5D0;_LL5CF: goto _LL5D1;_LL5D0: if(*((int*)_tmp958)!= 33)
goto _LL5D2;_LL5D1: goto _LL5D3;_LL5D2: if(*((int*)_tmp958)!= 34)goto _LL5D4;_LL5D3:
goto _LL5D5;_LL5D4: if(*((int*)_tmp958)!= 1)goto _LL5D6;_LL5D5: goto _LL5CD;_LL5D6:
if(*((int*)_tmp958)!= 3)goto _LL5D8;_tmp959=((struct Cyc_Absyn_Primop_e_struct*)
_tmp958)->f2;_LL5D7: for(0;_tmp959 != 0;_tmp959=_tmp959->tl){cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp((
struct Cyc_Absyn_Exp*)_tmp959->hd,te,cvtenv);}goto _LL5CD;_LL5D8: if(*((int*)
_tmp958)!= 6)goto _LL5DA;_tmp95A=((struct Cyc_Absyn_Conditional_e_struct*)_tmp958)->f1;
_tmp95B=((struct Cyc_Absyn_Conditional_e_struct*)_tmp958)->f2;_tmp95C=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp958)->f3;_LL5D9: cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp95A,te,
cvtenv);cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp95B,te,cvtenv);cvtenv=
Cyc_Tcutil_i_check_valid_type_level_exp(_tmp95C,te,cvtenv);goto _LL5CD;_LL5DA: if(*((
int*)_tmp958)!= 7)goto _LL5DC;_tmp95D=((struct Cyc_Absyn_And_e_struct*)_tmp958)->f1;
_tmp95E=((struct Cyc_Absyn_And_e_struct*)_tmp958)->f2;_LL5DB: _tmp95F=_tmp95D;
_tmp960=_tmp95E;goto _LL5DD;_LL5DC: if(*((int*)_tmp958)!= 8)goto _LL5DE;_tmp95F=((
struct Cyc_Absyn_Or_e_struct*)_tmp958)->f1;_tmp960=((struct Cyc_Absyn_Or_e_struct*)
_tmp958)->f2;_LL5DD: _tmp961=_tmp95F;_tmp962=_tmp960;goto _LL5DF;_LL5DE: if(*((int*)
_tmp958)!= 9)goto _LL5E0;_tmp961=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp958)->f1;
_tmp962=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp958)->f2;_LL5DF: cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(
_tmp961,te,cvtenv);cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp962,te,
cvtenv);goto _LL5CD;_LL5E0: if(*((int*)_tmp958)!= 15)goto _LL5E2;_tmp963=(void*)((
struct Cyc_Absyn_Cast_e_struct*)_tmp958)->f1;_tmp964=((struct Cyc_Absyn_Cast_e_struct*)
_tmp958)->f2;_LL5E1: cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp964,te,
cvtenv);cvtenv=Cyc_Tcutil_i_check_valid_type(e->loc,te,cvtenv,(void*)0,_tmp963,1);
goto _LL5CD;_LL5E2: if(*((int*)_tmp958)!= 20)goto _LL5E4;_tmp965=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)
_tmp958)->f1;_LL5E3: _tmp966=_tmp965;goto _LL5E5;_LL5E4: if(*((int*)_tmp958)!= 18)
goto _LL5E6;_tmp966=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp958)->f1;
_LL5E5: cvtenv=Cyc_Tcutil_i_check_valid_type(e->loc,te,cvtenv,(void*)0,_tmp966,1);
goto _LL5CD;_LL5E6: if(*((int*)_tmp958)!= 39)goto _LL5E8;_tmp967=(void*)((struct Cyc_Absyn_Valueof_e_struct*)
_tmp958)->f1;_LL5E7: cvtenv=Cyc_Tcutil_i_check_valid_type(e->loc,te,cvtenv,(void*)
7,_tmp967,1);goto _LL5CD;_LL5E8: if(*((int*)_tmp958)!= 19)goto _LL5EA;_tmp968=((
struct Cyc_Absyn_Sizeofexp_e_struct*)_tmp958)->f1;_LL5E9: cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(
_tmp968,te,cvtenv);goto _LL5CD;_LL5EA:;_LL5EB: {const char*_tmpFDA;void*_tmpFD9;(
_tmpFD9=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmpFDA="non-type-level-expression in Tcutil::i_check_valid_type_level_exp",
_tag_dyneither(_tmpFDA,sizeof(char),66))),_tag_dyneither(_tmpFD9,sizeof(void*),0)));}
_LL5CD:;}return cvtenv;}static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_check_valid_type(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv
cvt,void*expected_kind,void*t);static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_check_valid_type(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv
cvt,void*expected_kind,void*t){struct Cyc_List_List*_tmp96B=cvt.kind_env;cvt=Cyc_Tcutil_i_check_valid_type(
loc,te,cvt,expected_kind,t,1);{struct Cyc_List_List*vs=cvt.free_vars;for(0;vs != 0;
vs=vs->tl){struct Cyc_Absyn_Tvar*_tmp96D;struct _tuple18 _tmp96C=*((struct _tuple18*)
vs->hd);_tmp96D=_tmp96C.f1;cvt.kind_env=Cyc_Tcutil_fast_add_free_tvar(_tmp96B,
_tmp96D);}}{struct Cyc_List_List*evs=cvt.free_evars;for(0;evs != 0;evs=evs->tl){
void*_tmp96F;struct _tuple19 _tmp96E=*((struct _tuple19*)evs->hd);_tmp96F=_tmp96E.f1;{
void*_tmp970=Cyc_Tcutil_compress(_tmp96F);struct Cyc_Core_Opt*_tmp971;struct Cyc_Core_Opt**
_tmp972;_LL5ED: if(_tmp970 <= (void*)4)goto _LL5EF;if(*((int*)_tmp970)!= 0)goto
_LL5EF;_tmp971=((struct Cyc_Absyn_Evar_struct*)_tmp970)->f4;_tmp972=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Evar_struct*)_tmp970)->f4;_LL5EE: if(*_tmp972 == 0){struct Cyc_Core_Opt*
_tmpFDB;*_tmp972=((_tmpFDB=_cycalloc(sizeof(*_tmpFDB)),((_tmpFDB->v=_tmp96B,
_tmpFDB))));}else{struct Cyc_List_List*_tmp974=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(*_tmp972))->v;struct Cyc_List_List*_tmp975=0;for(0;_tmp974 != 0;
_tmp974=_tmp974->tl){if(((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,
_tmp96B,(struct Cyc_Absyn_Tvar*)_tmp974->hd)){struct Cyc_List_List*_tmpFDC;_tmp975=((
_tmpFDC=_cycalloc(sizeof(*_tmpFDC)),((_tmpFDC->hd=(struct Cyc_Absyn_Tvar*)_tmp974->hd,((
_tmpFDC->tl=_tmp975,_tmpFDC))))));}}{struct Cyc_Core_Opt*_tmpFDD;*_tmp972=((
_tmpFDD=_cycalloc(sizeof(*_tmpFDD)),((_tmpFDD->v=_tmp975,_tmpFDD))));}}goto
_LL5EC;_LL5EF:;_LL5F0: goto _LL5EC;_LL5EC:;}}}return cvt;}void Cyc_Tcutil_check_valid_toplevel_type(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,void*t);void Cyc_Tcutil_check_valid_toplevel_type(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,void*t){int
generalize_evars=Cyc_Tcutil_is_function_type(t);struct Cyc_List_List*_tmp978=Cyc_Tcenv_lookup_type_vars(
te);struct _RegionHandle*_tmp979=Cyc_Tcenv_get_fnrgn(te);struct Cyc_Tcutil_CVTEnv
_tmpFDE;struct Cyc_Tcutil_CVTEnv _tmp97A=Cyc_Tcutil_check_valid_type(loc,te,((
_tmpFDE.r=_tmp979,((_tmpFDE.kind_env=_tmp978,((_tmpFDE.free_vars=0,((_tmpFDE.free_evars=
0,((_tmpFDE.generalize_evars=generalize_evars,((_tmpFDE.fn_result=0,_tmpFDE)))))))))))),(
void*)1,t);struct Cyc_List_List*_tmp97B=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct Cyc_Absyn_Tvar*(*f)(struct _tuple18*),struct Cyc_List_List*x))
Cyc_List_rmap)(_tmp979,(struct Cyc_Absyn_Tvar*(*)(struct _tuple18*))Cyc_Core_fst,
_tmp97A.free_vars);struct Cyc_List_List*_tmp97C=((struct Cyc_List_List*(*)(struct
_RegionHandle*,void*(*f)(struct _tuple19*),struct Cyc_List_List*x))Cyc_List_rmap)(
_tmp979,(void*(*)(struct _tuple19*))Cyc_Core_fst,_tmp97A.free_evars);if(_tmp978 != 
0){struct Cyc_List_List*_tmp97D=0;{struct Cyc_List_List*_tmp97E=_tmp97B;for(0;(
unsigned int)_tmp97E;_tmp97E=_tmp97E->tl){struct Cyc_Absyn_Tvar*_tmp97F=(struct
Cyc_Absyn_Tvar*)_tmp97E->hd;int found=0;{struct Cyc_List_List*_tmp980=_tmp978;for(
0;(unsigned int)_tmp980;_tmp980=_tmp980->tl){if(Cyc_Absyn_tvar_cmp(_tmp97F,(
struct Cyc_Absyn_Tvar*)_tmp980->hd)== 0){found=1;break;}}}if(!found){struct Cyc_List_List*
_tmpFDF;_tmp97D=((_tmpFDF=_region_malloc(_tmp979,sizeof(*_tmpFDF)),((_tmpFDF->hd=(
struct Cyc_Absyn_Tvar*)_tmp97E->hd,((_tmpFDF->tl=_tmp97D,_tmpFDF))))));}}}_tmp97B=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp97D);}{
struct Cyc_List_List*x=_tmp97B;for(0;x != 0;x=x->tl){void*_tmp982=Cyc_Absyn_compress_kb((
void*)((struct Cyc_Absyn_Tvar*)x->hd)->kind);struct Cyc_Core_Opt*_tmp983;struct Cyc_Core_Opt**
_tmp984;struct Cyc_Core_Opt*_tmp985;struct Cyc_Core_Opt**_tmp986;void*_tmp987;
struct Cyc_Core_Opt*_tmp988;struct Cyc_Core_Opt**_tmp989;void*_tmp98A;struct Cyc_Core_Opt*
_tmp98B;struct Cyc_Core_Opt**_tmp98C;void*_tmp98D;void*_tmp98E;_LL5F2: if(*((int*)
_tmp982)!= 1)goto _LL5F4;_tmp983=((struct Cyc_Absyn_Unknown_kb_struct*)_tmp982)->f1;
_tmp984=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)_tmp982)->f1;
_LL5F3: _tmp986=_tmp984;goto _LL5F5;_LL5F4: if(*((int*)_tmp982)!= 2)goto _LL5F6;
_tmp985=((struct Cyc_Absyn_Less_kb_struct*)_tmp982)->f1;_tmp986=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*)_tmp982)->f1;_tmp987=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp982)->f2;if((int)_tmp987 != 1)goto _LL5F6;_LL5F5:*_tmp986=Cyc_Tcutil_kind_to_bound_opt((
void*)2);goto _LL5F1;_LL5F6: if(*((int*)_tmp982)!= 2)goto _LL5F8;_tmp988=((struct
Cyc_Absyn_Less_kb_struct*)_tmp982)->f1;_tmp989=(struct Cyc_Core_Opt**)&((struct
Cyc_Absyn_Less_kb_struct*)_tmp982)->f1;_tmp98A=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp982)->f2;if((int)_tmp98A != 5)goto _LL5F8;_LL5F7:*_tmp989=Cyc_Tcutil_kind_to_bound_opt((
void*)3);goto _LL5F1;_LL5F8: if(*((int*)_tmp982)!= 2)goto _LL5FA;_tmp98B=((struct
Cyc_Absyn_Less_kb_struct*)_tmp982)->f1;_tmp98C=(struct Cyc_Core_Opt**)&((struct
Cyc_Absyn_Less_kb_struct*)_tmp982)->f1;_tmp98D=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp982)->f2;_LL5F9:*_tmp98C=Cyc_Tcutil_kind_to_bound_opt(_tmp98D);goto _LL5F1;
_LL5FA: if(*((int*)_tmp982)!= 0)goto _LL5FC;_tmp98E=(void*)((struct Cyc_Absyn_Eq_kb_struct*)
_tmp982)->f1;if((int)_tmp98E != 1)goto _LL5FC;_LL5FB:{const char*_tmpFE3;void*
_tmpFE2[1];struct Cyc_String_pa_struct _tmpFE1;(_tmpFE1.tag=0,((_tmpFE1.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_tvar2string((struct Cyc_Absyn_Tvar*)
x->hd)),((_tmpFE2[0]=& _tmpFE1,Cyc_Tcutil_terr(loc,((_tmpFE3="type variable %s cannot have kind M",
_tag_dyneither(_tmpFE3,sizeof(char),36))),_tag_dyneither(_tmpFE2,sizeof(void*),1)))))));}
goto _LL5F1;_LL5FC:;_LL5FD: goto _LL5F1;_LL5F1:;}}if(_tmp97B != 0  || _tmp97C != 0){{
void*_tmp992=Cyc_Tcutil_compress(t);struct Cyc_Absyn_FnInfo _tmp993;struct Cyc_List_List*
_tmp994;struct Cyc_List_List**_tmp995;struct Cyc_Core_Opt*_tmp996;void*_tmp997;
struct Cyc_List_List*_tmp998;int _tmp999;struct Cyc_Absyn_VarargInfo*_tmp99A;struct
Cyc_List_List*_tmp99B;struct Cyc_List_List*_tmp99C;_LL5FF: if(_tmp992 <= (void*)4)
goto _LL601;if(*((int*)_tmp992)!= 8)goto _LL601;_tmp993=((struct Cyc_Absyn_FnType_struct*)
_tmp992)->f1;_tmp994=_tmp993.tvars;_tmp995=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_struct*)
_tmp992)->f1).tvars;_tmp996=_tmp993.effect;_tmp997=(void*)_tmp993.ret_typ;
_tmp998=_tmp993.args;_tmp999=_tmp993.c_varargs;_tmp99A=_tmp993.cyc_varargs;
_tmp99B=_tmp993.rgn_po;_tmp99C=_tmp993.attributes;_LL600: if(*_tmp995 == 0){*
_tmp995=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_copy)(_tmp97B);
_tmp97B=0;}goto _LL5FE;_LL601:;_LL602: goto _LL5FE;_LL5FE:;}if(_tmp97B != 0){const
char*_tmpFE7;void*_tmpFE6[1];struct Cyc_String_pa_struct _tmpFE5;(_tmpFE5.tag=0,((
_tmpFE5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)
_tmp97B->hd)->name),((_tmpFE6[0]=& _tmpFE5,Cyc_Tcutil_terr(loc,((_tmpFE7="unbound type variable %s",
_tag_dyneither(_tmpFE7,sizeof(char),25))),_tag_dyneither(_tmpFE6,sizeof(void*),1)))))));}
if(_tmp97C != 0)for(0;_tmp97C != 0;_tmp97C=_tmp97C->tl){void*e=(void*)_tmp97C->hd;
void*_tmp9A0=Cyc_Tcutil_typ_kind(e);_LL604: if((int)_tmp9A0 != 4)goto _LL606;_LL605:
if(!Cyc_Tcutil_unify(e,(void*)3)){const char*_tmpFEA;void*_tmpFE9;(_tmpFE9=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmpFEA="can't unify evar with unique region!",_tag_dyneither(_tmpFEA,sizeof(
char),37))),_tag_dyneither(_tmpFE9,sizeof(void*),0)));}goto _LL603;_LL606: if((int)
_tmp9A0 != 5)goto _LL608;_LL607: goto _LL609;_LL608: if((int)_tmp9A0 != 3)goto _LL60A;
_LL609: if(!Cyc_Tcutil_unify(e,(void*)2)){const char*_tmpFED;void*_tmpFEC;(_tmpFEC=
0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmpFED="can't unify evar with heap!",_tag_dyneither(_tmpFED,sizeof(char),28))),
_tag_dyneither(_tmpFEC,sizeof(void*),0)));}goto _LL603;_LL60A: if((int)_tmp9A0 != 6)
goto _LL60C;_LL60B: if(!Cyc_Tcutil_unify(e,Cyc_Absyn_empty_effect)){const char*
_tmpFF0;void*_tmpFEF;(_tmpFEF=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpFF0="can't unify evar with {}!",
_tag_dyneither(_tmpFF0,sizeof(char),26))),_tag_dyneither(_tmpFEF,sizeof(void*),0)));}
goto _LL603;_LL60C:;_LL60D:{const char*_tmpFF5;void*_tmpFF4[2];struct Cyc_String_pa_struct
_tmpFF3;struct Cyc_String_pa_struct _tmpFF2;(_tmpFF2.tag=0,((_tmpFF2.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpFF3.tag=0,((
_tmpFF3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(e)),((
_tmpFF4[0]=& _tmpFF3,((_tmpFF4[1]=& _tmpFF2,Cyc_Tcutil_terr(loc,((_tmpFF5="hidden type variable %s isn't abstracted in type %s",
_tag_dyneither(_tmpFF5,sizeof(char),52))),_tag_dyneither(_tmpFF4,sizeof(void*),2)))))))))))));}
goto _LL603;_LL603:;}}}void Cyc_Tcutil_check_fndecl_valid_type(struct Cyc_Position_Segment*
loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Fndecl*fd);void Cyc_Tcutil_check_fndecl_valid_type(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Fndecl*fd){
void*t=Cyc_Tcutil_fndecl2typ(fd);Cyc_Tcutil_check_valid_toplevel_type(loc,te,t);{
void*_tmp9AC=Cyc_Tcutil_compress(t);struct Cyc_Absyn_FnInfo _tmp9AD;struct Cyc_List_List*
_tmp9AE;struct Cyc_Core_Opt*_tmp9AF;void*_tmp9B0;_LL60F: if(_tmp9AC <= (void*)4)
goto _LL611;if(*((int*)_tmp9AC)!= 8)goto _LL611;_tmp9AD=((struct Cyc_Absyn_FnType_struct*)
_tmp9AC)->f1;_tmp9AE=_tmp9AD.tvars;_tmp9AF=_tmp9AD.effect;_tmp9B0=(void*)_tmp9AD.ret_typ;
_LL610: fd->tvs=_tmp9AE;fd->effect=_tmp9AF;goto _LL60E;_LL611:;_LL612: {const char*
_tmpFF8;void*_tmpFF7;(_tmpFF7=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpFF8="check_fndecl_valid_type: not a FnType",
_tag_dyneither(_tmpFF8,sizeof(char),38))),_tag_dyneither(_tmpFF7,sizeof(void*),0)));}
_LL60E:;}{struct _RegionHandle*_tmp9B3=Cyc_Tcenv_get_fnrgn(te);{const char*_tmpFF9;
Cyc_Tcutil_check_unique_vars(((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct _dyneither_ptr*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_rmap)(
_tmp9B3,(struct _dyneither_ptr*(*)(struct _tuple14*t))Cyc_Tcutil_fst_fdarg,fd->args),
loc,((_tmpFF9="function declaration has repeated parameter",_tag_dyneither(
_tmpFF9,sizeof(char),44))));}{struct Cyc_Core_Opt*_tmpFFA;fd->cached_typ=((
_tmpFFA=_cycalloc(sizeof(*_tmpFFA)),((_tmpFFA->v=(void*)t,_tmpFFA))));}}}void Cyc_Tcutil_check_type(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*
bound_tvars,void*expected_kind,int allow_evars,void*t);void Cyc_Tcutil_check_type(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*
bound_tvars,void*expected_kind,int allow_evars,void*t){struct _RegionHandle*
_tmp9B6=Cyc_Tcenv_get_fnrgn(te);struct Cyc_Tcutil_CVTEnv _tmpFFB;struct Cyc_Tcutil_CVTEnv
_tmp9B7=Cyc_Tcutil_check_valid_type(loc,te,((_tmpFFB.r=_tmp9B6,((_tmpFFB.kind_env=
bound_tvars,((_tmpFFB.free_vars=0,((_tmpFFB.free_evars=0,((_tmpFFB.generalize_evars=
0,((_tmpFFB.fn_result=0,_tmpFFB)))))))))))),expected_kind,t);struct Cyc_List_List*
_tmp9B8=Cyc_Tcutil_remove_bound_tvars(_tmp9B6,((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct Cyc_Absyn_Tvar*(*f)(struct _tuple18*),struct Cyc_List_List*x))
Cyc_List_rmap)(_tmp9B6,(struct Cyc_Absyn_Tvar*(*)(struct _tuple18*))Cyc_Core_fst,
_tmp9B7.free_vars),bound_tvars);struct Cyc_List_List*_tmp9B9=((struct Cyc_List_List*(*)(
struct _RegionHandle*,void*(*f)(struct _tuple19*),struct Cyc_List_List*x))Cyc_List_rmap)(
_tmp9B6,(void*(*)(struct _tuple19*))Cyc_Core_fst,_tmp9B7.free_evars);{struct Cyc_List_List*
fs=_tmp9B8;for(0;fs != 0;fs=fs->tl){struct _dyneither_ptr*_tmp9BA=((struct Cyc_Absyn_Tvar*)
fs->hd)->name;const char*_tmp1000;void*_tmpFFF[2];struct Cyc_String_pa_struct
_tmpFFE;struct Cyc_String_pa_struct _tmpFFD;(_tmpFFD.tag=0,((_tmpFFD.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpFFE.tag=0,((
_tmpFFE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp9BA),((_tmpFFF[0]=&
_tmpFFE,((_tmpFFF[1]=& _tmpFFD,Cyc_Tcutil_terr(loc,((_tmp1000="unbound type variable %s in type %s",
_tag_dyneither(_tmp1000,sizeof(char),36))),_tag_dyneither(_tmpFFF,sizeof(void*),
2)))))))))))));}}if(!allow_evars  && _tmp9B9 != 0)for(0;_tmp9B9 != 0;_tmp9B9=
_tmp9B9->tl){void*e=(void*)_tmp9B9->hd;void*_tmp9BF=Cyc_Tcutil_typ_kind(e);
_LL614: if((int)_tmp9BF != 4)goto _LL616;_LL615: if(!Cyc_Tcutil_unify(e,(void*)3)){
const char*_tmp1003;void*_tmp1002;(_tmp1002=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1003="can't unify evar with unique region!",
_tag_dyneither(_tmp1003,sizeof(char),37))),_tag_dyneither(_tmp1002,sizeof(void*),
0)));}goto _LL613;_LL616: if((int)_tmp9BF != 5)goto _LL618;_LL617: goto _LL619;_LL618:
if((int)_tmp9BF != 3)goto _LL61A;_LL619: if(!Cyc_Tcutil_unify(e,(void*)2)){const
char*_tmp1006;void*_tmp1005;(_tmp1005=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1006="can't unify evar with heap!",
_tag_dyneither(_tmp1006,sizeof(char),28))),_tag_dyneither(_tmp1005,sizeof(void*),
0)));}goto _LL613;_LL61A: if((int)_tmp9BF != 6)goto _LL61C;_LL61B: if(!Cyc_Tcutil_unify(
e,Cyc_Absyn_empty_effect)){const char*_tmp1009;void*_tmp1008;(_tmp1008=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1009="can't unify evar with {}!",
_tag_dyneither(_tmp1009,sizeof(char),26))),_tag_dyneither(_tmp1008,sizeof(void*),
0)));}goto _LL613;_LL61C:;_LL61D:{const char*_tmp100E;void*_tmp100D[2];struct Cyc_String_pa_struct
_tmp100C;struct Cyc_String_pa_struct _tmp100B;(_tmp100B.tag=0,((_tmp100B.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((
_tmp100C.tag=0,((_tmp100C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
e)),((_tmp100D[0]=& _tmp100C,((_tmp100D[1]=& _tmp100B,Cyc_Tcutil_terr(loc,((
_tmp100E="hidden type variable %s isn't abstracted in type %s",_tag_dyneither(
_tmp100E,sizeof(char),52))),_tag_dyneither(_tmp100D,sizeof(void*),2)))))))))))));}
goto _LL613;_LL613:;}}void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*tv);
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
vs->hd,(void*)vs2->hd)== 0){const char*_tmp1013;void*_tmp1012[2];struct Cyc_String_pa_struct
_tmp1011;struct Cyc_String_pa_struct _tmp1010;(_tmp1010.tag=0,((_tmp1010.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)a2string((void*)vs->hd)),((_tmp1011.tag=
0,((_tmp1011.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg),((_tmp1012[0]=&
_tmp1011,((_tmp1012[1]=& _tmp1010,Cyc_Tcutil_terr(loc,((_tmp1013="%s: %s",
_tag_dyneither(_tmp1013,sizeof(char),7))),_tag_dyneither(_tmp1012,sizeof(void*),
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
struct Cyc_Position_Segment*loc,struct Cyc_List_List*tvs){const char*_tmp1014;((
void(*)(int(*cmp)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*
vs,struct Cyc_Position_Segment*loc,struct _dyneither_ptr(*a2string)(struct Cyc_Absyn_Tvar*),
struct _dyneither_ptr msg))Cyc_Tcutil_check_unique_unsorted)(Cyc_Absyn_tvar_cmp,
tvs,loc,Cyc_Tcutil_tvar2string,((_tmp1014="duplicate type variable",
_tag_dyneither(_tmp1014,sizeof(char),24))));}struct _tuple20{struct Cyc_Absyn_Aggrfield*
f1;int f2;};struct _tuple21{struct Cyc_List_List*f1;void*f2;};struct _tuple22{struct
Cyc_Absyn_Aggrfield*f1;void*f2;};struct Cyc_List_List*Cyc_Tcutil_resolve_struct_designators(
struct _RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,
struct Cyc_List_List*sdfields);struct Cyc_List_List*Cyc_Tcutil_resolve_struct_designators(
struct _RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,
struct Cyc_List_List*sdfields){struct Cyc_List_List*fields=0;{struct Cyc_List_List*
sd_fields=sdfields;for(0;sd_fields != 0;sd_fields=sd_fields->tl){const char*
_tmp1015;if(Cyc_strcmp((struct _dyneither_ptr)*((struct Cyc_Absyn_Aggrfield*)
sd_fields->hd)->name,((_tmp1015="",_tag_dyneither(_tmp1015,sizeof(char),1))))!= 
0){struct _tuple20*_tmp1018;struct Cyc_List_List*_tmp1017;fields=((_tmp1017=
_cycalloc(sizeof(*_tmp1017)),((_tmp1017->hd=((_tmp1018=_cycalloc(sizeof(*
_tmp1018)),((_tmp1018->f1=(struct Cyc_Absyn_Aggrfield*)sd_fields->hd,((_tmp1018->f2=
0,_tmp1018)))))),((_tmp1017->tl=fields,_tmp1017))))));}}}fields=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(fields);{struct Cyc_List_List*ans=0;for(
0;des != 0;des=des->tl){struct _tuple21 _tmp9D4;struct Cyc_List_List*_tmp9D5;void*
_tmp9D6;struct _tuple21*_tmp9D3=(struct _tuple21*)des->hd;_tmp9D4=*_tmp9D3;_tmp9D5=
_tmp9D4.f1;_tmp9D6=_tmp9D4.f2;if(_tmp9D5 == 0){struct Cyc_List_List*_tmp9D7=fields;
for(0;_tmp9D7 != 0;_tmp9D7=_tmp9D7->tl){if(!(*((struct _tuple20*)_tmp9D7->hd)).f2){(*((
struct _tuple20*)_tmp9D7->hd)).f2=1;{struct Cyc_Absyn_FieldName_struct*_tmp101E;
struct Cyc_Absyn_FieldName_struct _tmp101D;struct Cyc_List_List*_tmp101C;(*((struct
_tuple21*)des->hd)).f1=(struct Cyc_List_List*)((_tmp101C=_cycalloc(sizeof(*
_tmp101C)),((_tmp101C->hd=(void*)((void*)((_tmp101E=_cycalloc(sizeof(*_tmp101E)),((
_tmp101E[0]=((_tmp101D.tag=1,((_tmp101D.f1=((*((struct _tuple20*)_tmp9D7->hd)).f1)->name,
_tmp101D)))),_tmp101E))))),((_tmp101C->tl=0,_tmp101C))))));}{struct _tuple22*
_tmp1021;struct Cyc_List_List*_tmp1020;ans=((_tmp1020=_region_malloc(rgn,sizeof(*
_tmp1020)),((_tmp1020->hd=((_tmp1021=_region_malloc(rgn,sizeof(*_tmp1021)),((
_tmp1021->f1=(*((struct _tuple20*)_tmp9D7->hd)).f1,((_tmp1021->f2=_tmp9D6,
_tmp1021)))))),((_tmp1020->tl=ans,_tmp1020))))));}break;}}if(_tmp9D7 == 0){const
char*_tmp1024;void*_tmp1023;(_tmp1023=0,Cyc_Tcutil_terr(loc,((_tmp1024="too many arguments to struct",
_tag_dyneither(_tmp1024,sizeof(char),29))),_tag_dyneither(_tmp1023,sizeof(void*),
0)));}}else{if(_tmp9D5->tl != 0){const char*_tmp1027;void*_tmp1026;(_tmp1026=0,Cyc_Tcutil_terr(
loc,((_tmp1027="multiple designators are not supported",_tag_dyneither(_tmp1027,
sizeof(char),39))),_tag_dyneither(_tmp1026,sizeof(void*),0)));}else{void*_tmp9E1=(
void*)_tmp9D5->hd;struct _dyneither_ptr*_tmp9E2;_LL61F: if(*((int*)_tmp9E1)!= 0)
goto _LL621;_LL620:{const char*_tmp102A;void*_tmp1029;(_tmp1029=0,Cyc_Tcutil_terr(
loc,((_tmp102A="array designator used in argument to struct",_tag_dyneither(
_tmp102A,sizeof(char),44))),_tag_dyneither(_tmp1029,sizeof(void*),0)));}goto
_LL61E;_LL621: if(*((int*)_tmp9E1)!= 1)goto _LL61E;_tmp9E2=((struct Cyc_Absyn_FieldName_struct*)
_tmp9E1)->f1;_LL622: {struct Cyc_List_List*_tmp9E5=fields;for(0;_tmp9E5 != 0;
_tmp9E5=_tmp9E5->tl){if(Cyc_strptrcmp(_tmp9E2,((*((struct _tuple20*)_tmp9E5->hd)).f1)->name)
== 0){if((*((struct _tuple20*)_tmp9E5->hd)).f2){const char*_tmp102E;void*_tmp102D[
1];struct Cyc_String_pa_struct _tmp102C;(_tmp102C.tag=0,((_tmp102C.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*_tmp9E2),((_tmp102D[0]=& _tmp102C,Cyc_Tcutil_terr(
loc,((_tmp102E="field %s has already been used as an argument",_tag_dyneither(
_tmp102E,sizeof(char),46))),_tag_dyneither(_tmp102D,sizeof(void*),1)))))));}(*((
struct _tuple20*)_tmp9E5->hd)).f2=1;{struct _tuple22*_tmp1031;struct Cyc_List_List*
_tmp1030;ans=((_tmp1030=_region_malloc(rgn,sizeof(*_tmp1030)),((_tmp1030->hd=((
_tmp1031=_region_malloc(rgn,sizeof(*_tmp1031)),((_tmp1031->f1=(*((struct _tuple20*)
_tmp9E5->hd)).f1,((_tmp1031->f2=_tmp9D6,_tmp1031)))))),((_tmp1030->tl=ans,
_tmp1030))))));}break;}}if(_tmp9E5 == 0){const char*_tmp1035;void*_tmp1034[1];
struct Cyc_String_pa_struct _tmp1033;(_tmp1033.tag=0,((_tmp1033.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*_tmp9E2),((_tmp1034[0]=& _tmp1033,Cyc_Tcutil_terr(
loc,((_tmp1035="bad field designator %s",_tag_dyneither(_tmp1035,sizeof(char),24))),
_tag_dyneither(_tmp1034,sizeof(void*),1)))))));}goto _LL61E;}_LL61E:;}}}for(0;
fields != 0;fields=fields->tl){if(!(*((struct _tuple20*)fields->hd)).f2){{const
char*_tmp1038;void*_tmp1037;(_tmp1037=0,Cyc_Tcutil_terr(loc,((_tmp1038="too few arguments to struct",
_tag_dyneither(_tmp1038,sizeof(char),28))),_tag_dyneither(_tmp1037,sizeof(void*),
0)));}break;}}return((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
ans);}}int Cyc_Tcutil_is_tagged_pointer_typ_elt(void*t,void**elt_typ_dest);int Cyc_Tcutil_is_tagged_pointer_typ_elt(
void*t,void**elt_typ_dest){void*_tmp9F0=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo
_tmp9F1;void*_tmp9F2;struct Cyc_Absyn_PtrAtts _tmp9F3;struct Cyc_Absyn_Conref*
_tmp9F4;_LL624: if(_tmp9F0 <= (void*)4)goto _LL626;if(*((int*)_tmp9F0)!= 4)goto
_LL626;_tmp9F1=((struct Cyc_Absyn_PointerType_struct*)_tmp9F0)->f1;_tmp9F2=(void*)
_tmp9F1.elt_typ;_tmp9F3=_tmp9F1.ptr_atts;_tmp9F4=_tmp9F3.bounds;_LL625: {struct
Cyc_Absyn_Conref*_tmp9F5=Cyc_Absyn_compress_conref(_tmp9F4);union Cyc_Absyn_Constraint_union
_tmp9F6=(Cyc_Absyn_compress_conref(_tmp9F5))->v;void*_tmp9F7;_LL629: if((_tmp9F6.Eq_constr).tag
!= 0)goto _LL62B;_tmp9F7=(_tmp9F6.Eq_constr).f1;if((int)_tmp9F7 != 0)goto _LL62B;
_LL62A:*elt_typ_dest=_tmp9F2;return 1;_LL62B: if((_tmp9F6.No_constr).tag != 2)goto
_LL62D;_LL62C:{union Cyc_Absyn_Constraint_union _tmp1039;_tmp9F5->v=(union Cyc_Absyn_Constraint_union)(((
_tmp1039.Eq_constr).tag=0,(((_tmp1039.Eq_constr).f1=(void*)((void*)0),_tmp1039))));}*
elt_typ_dest=_tmp9F2;return 1;_LL62D:;_LL62E: return 0;_LL628:;}_LL626:;_LL627:
return 0;_LL623:;}int Cyc_Tcutil_is_zero_pointer_typ_elt(void*t,void**elt_typ_dest);
int Cyc_Tcutil_is_zero_pointer_typ_elt(void*t,void**elt_typ_dest){void*_tmp9F9=
Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp9FA;void*_tmp9FB;struct Cyc_Absyn_PtrAtts
_tmp9FC;struct Cyc_Absyn_Conref*_tmp9FD;_LL630: if(_tmp9F9 <= (void*)4)goto _LL632;
if(*((int*)_tmp9F9)!= 4)goto _LL632;_tmp9FA=((struct Cyc_Absyn_PointerType_struct*)
_tmp9F9)->f1;_tmp9FB=(void*)_tmp9FA.elt_typ;_tmp9FC=_tmp9FA.ptr_atts;_tmp9FD=
_tmp9FC.zero_term;_LL631:*elt_typ_dest=_tmp9FB;return((int(*)(int,struct Cyc_Absyn_Conref*
x))Cyc_Absyn_conref_def)(0,_tmp9FD);_LL632:;_LL633: return 0;_LL62F:;}int Cyc_Tcutil_is_zero_ptr_type(
void*t,void**ptr_type,int*is_dyneither,void**elt_type);int Cyc_Tcutil_is_zero_ptr_type(
void*t,void**ptr_type,int*is_dyneither,void**elt_type){void*_tmp9FE=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_PtrInfo _tmp9FF;void*_tmpA00;struct Cyc_Absyn_PtrAtts _tmpA01;
struct Cyc_Absyn_Conref*_tmpA02;struct Cyc_Absyn_Conref*_tmpA03;struct Cyc_Absyn_ArrayInfo
_tmpA04;void*_tmpA05;struct Cyc_Absyn_Tqual _tmpA06;struct Cyc_Absyn_Exp*_tmpA07;
struct Cyc_Absyn_Conref*_tmpA08;_LL635: if(_tmp9FE <= (void*)4)goto _LL639;if(*((int*)
_tmp9FE)!= 4)goto _LL637;_tmp9FF=((struct Cyc_Absyn_PointerType_struct*)_tmp9FE)->f1;
_tmpA00=(void*)_tmp9FF.elt_typ;_tmpA01=_tmp9FF.ptr_atts;_tmpA02=_tmpA01.bounds;
_tmpA03=_tmpA01.zero_term;_LL636: if(((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,_tmpA03)){*ptr_type=t;*elt_type=_tmpA00;{void*_tmpA09=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,
_tmpA02);_LL63C: if((int)_tmpA09 != 0)goto _LL63E;_LL63D:*is_dyneither=1;goto _LL63B;
_LL63E:;_LL63F:*is_dyneither=0;goto _LL63B;_LL63B:;}return 1;}else{return 0;}_LL637:
if(*((int*)_tmp9FE)!= 7)goto _LL639;_tmpA04=((struct Cyc_Absyn_ArrayType_struct*)
_tmp9FE)->f1;_tmpA05=(void*)_tmpA04.elt_type;_tmpA06=_tmpA04.tq;_tmpA07=_tmpA04.num_elts;
_tmpA08=_tmpA04.zero_term;_LL638: if(((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,_tmpA08)){*elt_type=_tmpA05;*is_dyneither=0;{struct Cyc_Absyn_PointerType_struct
_tmp104E;struct Cyc_Absyn_PtrAtts _tmp104D;struct Cyc_Absyn_Upper_b_struct _tmp104C;
struct Cyc_Absyn_Upper_b_struct*_tmp104B;struct Cyc_Absyn_PtrInfo _tmp104A;struct
Cyc_Absyn_PointerType_struct*_tmp1049;*ptr_type=(void*)((_tmp1049=_cycalloc(
sizeof(*_tmp1049)),((_tmp1049[0]=((_tmp104E.tag=4,((_tmp104E.f1=((_tmp104A.elt_typ=(
void*)_tmpA05,((_tmp104A.elt_tq=_tmpA06,((_tmp104A.ptr_atts=((_tmp104D.rgn=(void*)((
void*)2),((_tmp104D.nullable=Cyc_Absyn_false_conref,((_tmp104D.bounds=Cyc_Absyn_new_conref((
void*)((_tmp104B=_cycalloc(sizeof(*_tmp104B)),((_tmp104B[0]=((_tmp104C.tag=0,((
_tmp104C.f1=(struct Cyc_Absyn_Exp*)_check_null(_tmpA07),_tmp104C)))),_tmp104B))))),((
_tmp104D.zero_term=_tmpA08,((_tmp104D.ptrloc=0,_tmp104D)))))))))),_tmp104A)))))),
_tmp104E)))),_tmp1049))));}return 1;}else{return 0;}_LL639:;_LL63A: return 0;_LL634:;}
int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*e1,void**ptr_type,int*
is_dyneither,void**elt_type);int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*
e1,void**ptr_type,int*is_dyneither,void**elt_type){void*_tmpA10=(void*)e1->r;
struct Cyc_Absyn_Exp*_tmpA11;struct Cyc_Absyn_Exp*_tmpA12;struct Cyc_Absyn_Exp*
_tmpA13;struct Cyc_Absyn_Exp*_tmpA14;struct Cyc_Absyn_Exp*_tmpA15;struct Cyc_Absyn_Exp*
_tmpA16;_LL641: if(*((int*)_tmpA10)!= 15)goto _LL643;_LL642: {const char*_tmp1052;
void*_tmp1051[1];struct Cyc_String_pa_struct _tmp1050;(_tmp1050.tag=0,((_tmp1050.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e1)),((
_tmp1051[0]=& _tmp1050,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmp1052="we have a cast in a lhs:  %s",_tag_dyneither(
_tmp1052,sizeof(char),29))),_tag_dyneither(_tmp1051,sizeof(void*),1)))))));}
_LL643: if(*((int*)_tmpA10)!= 22)goto _LL645;_tmpA11=((struct Cyc_Absyn_Deref_e_struct*)
_tmpA10)->f1;_LL644: _tmpA12=_tmpA11;goto _LL646;_LL645: if(*((int*)_tmpA10)!= 25)
goto _LL647;_tmpA12=((struct Cyc_Absyn_Subscript_e_struct*)_tmpA10)->f1;_LL646:
return Cyc_Tcutil_is_zero_ptr_type((void*)((struct Cyc_Core_Opt*)_check_null(
_tmpA12->topt))->v,ptr_type,is_dyneither,elt_type);_LL647: if(*((int*)_tmpA10)!= 
24)goto _LL649;_tmpA13=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmpA10)->f1;_LL648:
_tmpA14=_tmpA13;goto _LL64A;_LL649: if(*((int*)_tmpA10)!= 23)goto _LL64B;_tmpA14=((
struct Cyc_Absyn_AggrMember_e_struct*)_tmpA10)->f1;_LL64A: if(Cyc_Tcutil_is_zero_ptr_type((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpA14->topt))->v,ptr_type,is_dyneither,
elt_type)){const char*_tmp1056;void*_tmp1055[1];struct Cyc_String_pa_struct
_tmp1054;(_tmp1054.tag=0,((_tmp1054.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_exp2string(e1)),((_tmp1055[0]=& _tmp1054,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1056="found zero pointer aggregate member assignment: %s",
_tag_dyneither(_tmp1056,sizeof(char),51))),_tag_dyneither(_tmp1055,sizeof(void*),
1)))))));}return 0;_LL64B: if(*((int*)_tmpA10)!= 14)goto _LL64D;_tmpA15=((struct Cyc_Absyn_Instantiate_e_struct*)
_tmpA10)->f1;_LL64C: _tmpA16=_tmpA15;goto _LL64E;_LL64D: if(*((int*)_tmpA10)!= 13)
goto _LL64F;_tmpA16=((struct Cyc_Absyn_NoInstantiate_e_struct*)_tmpA10)->f1;_LL64E:
if(Cyc_Tcutil_is_zero_ptr_type((void*)((struct Cyc_Core_Opt*)_check_null(_tmpA16->topt))->v,
ptr_type,is_dyneither,elt_type)){const char*_tmp105A;void*_tmp1059[1];struct Cyc_String_pa_struct
_tmp1058;(_tmp1058.tag=0,((_tmp1058.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_exp2string(e1)),((_tmp1059[0]=& _tmp1058,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp105A="found zero pointer instantiate/noinstantiate: %s",
_tag_dyneither(_tmp105A,sizeof(char),49))),_tag_dyneither(_tmp1059,sizeof(void*),
1)))))));}return 0;_LL64F: if(*((int*)_tmpA10)!= 1)goto _LL651;_LL650: return 0;
_LL651:;_LL652: {const char*_tmp105E;void*_tmp105D[1];struct Cyc_String_pa_struct
_tmp105C;(_tmp105C.tag=0,((_tmp105C.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_exp2string(e1)),((_tmp105D[0]=& _tmp105C,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp105E="found bad lhs in is_zero_ptr_deref: %s",
_tag_dyneither(_tmp105E,sizeof(char),39))),_tag_dyneither(_tmp105D,sizeof(void*),
1)))))));}_LL640:;}int Cyc_Tcutil_is_tagged_pointer_typ(void*t);int Cyc_Tcutil_is_tagged_pointer_typ(
void*t){void*ignore=(void*)0;return Cyc_Tcutil_is_tagged_pointer_typ_elt(t,&
ignore);}static int Cyc_Tcutil_is_noalias_region(void*r);static int Cyc_Tcutil_is_noalias_region(
void*r){void*_tmpA23=Cyc_Tcutil_compress(r);struct Cyc_Absyn_Tvar*_tmpA24;_LL654:
if((int)_tmpA23 != 3)goto _LL656;_LL655: return 1;_LL656: if(_tmpA23 <= (void*)4)goto
_LL658;if(*((int*)_tmpA23)!= 1)goto _LL658;_tmpA24=((struct Cyc_Absyn_VarType_struct*)
_tmpA23)->f1;_LL657: return Cyc_Tcutil_tvar_kind(_tmpA24)== (void*)4  || Cyc_Tcutil_tvar_kind(
_tmpA24)== (void*)5;_LL658:;_LL659: return 0;_LL653:;}int Cyc_Tcutil_is_noalias_pointer(
void*t);int Cyc_Tcutil_is_noalias_pointer(void*t){void*_tmpA25=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_PtrInfo _tmpA26;struct Cyc_Absyn_PtrAtts _tmpA27;void*_tmpA28;
_LL65B: if(_tmpA25 <= (void*)4)goto _LL65D;if(*((int*)_tmpA25)!= 4)goto _LL65D;
_tmpA26=((struct Cyc_Absyn_PointerType_struct*)_tmpA25)->f1;_tmpA27=_tmpA26.ptr_atts;
_tmpA28=(void*)_tmpA27.rgn;_LL65C: return Cyc_Tcutil_is_noalias_region(_tmpA28);
_LL65D:;_LL65E: return 0;_LL65A:;}int Cyc_Tcutil_is_noalias_pointer_or_aggr(struct
_RegionHandle*rgn,void*t);int Cyc_Tcutil_is_noalias_pointer_or_aggr(struct
_RegionHandle*rgn,void*t){void*_tmpA29=Cyc_Tcutil_compress(t);if(Cyc_Tcutil_is_noalias_pointer(
_tmpA29))return 1;{void*_tmpA2A=_tmpA29;struct Cyc_List_List*_tmpA2B;struct Cyc_Absyn_AggrInfo
_tmpA2C;union Cyc_Absyn_AggrInfoU_union _tmpA2D;struct Cyc_Absyn_Aggrdecl**_tmpA2E;
struct Cyc_List_List*_tmpA2F;struct Cyc_List_List*_tmpA30;struct Cyc_Absyn_AggrInfo
_tmpA31;union Cyc_Absyn_AggrInfoU_union _tmpA32;struct Cyc_Absyn_TunionInfo _tmpA33;
union Cyc_Absyn_TunionInfoU_union _tmpA34;struct Cyc_List_List*_tmpA35;struct Cyc_Core_Opt*
_tmpA36;struct Cyc_Absyn_TunionFieldInfo _tmpA37;union Cyc_Absyn_TunionFieldInfoU_union
_tmpA38;struct Cyc_List_List*_tmpA39;_LL660: if(_tmpA2A <= (void*)4)goto _LL66C;if(*((
int*)_tmpA2A)!= 9)goto _LL662;_tmpA2B=((struct Cyc_Absyn_TupleType_struct*)_tmpA2A)->f1;
_LL661: while(_tmpA2B != 0){if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,(*((
struct _tuple5*)_tmpA2B->hd)).f2))return 1;_tmpA2B=_tmpA2B->tl;}return 0;_LL662: if(*((
int*)_tmpA2A)!= 10)goto _LL664;_tmpA2C=((struct Cyc_Absyn_AggrType_struct*)_tmpA2A)->f1;
_tmpA2D=_tmpA2C.aggr_info;if((((((struct Cyc_Absyn_AggrType_struct*)_tmpA2A)->f1).aggr_info).KnownAggr).tag
!= 1)goto _LL664;_tmpA2E=(_tmpA2D.KnownAggr).f1;_tmpA2F=_tmpA2C.targs;_LL663: if((*
_tmpA2E)->impl == 0)return 0;else{struct Cyc_List_List*_tmpA3A=((struct Cyc_List_List*(*)(
struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(rgn,rgn,(*_tmpA2E)->tvs,_tmpA2F);struct Cyc_List_List*_tmpA3B=((
struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmpA2E)->impl))->fields;void*t;
while(_tmpA3B != 0){t=Cyc_Tcutil_rsubstitute(rgn,_tmpA3A,(void*)((struct Cyc_Absyn_Aggrfield*)
_tmpA3B->hd)->type);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,t))return 1;
_tmpA3B=_tmpA3B->tl;}return 0;}_LL664: if(*((int*)_tmpA2A)!= 11)goto _LL666;_tmpA30=((
struct Cyc_Absyn_AnonAggrType_struct*)_tmpA2A)->f2;_LL665: while(_tmpA30 != 0){if(
Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,(void*)((struct Cyc_Absyn_Aggrfield*)
_tmpA30->hd)->type))return 1;_tmpA30=_tmpA30->tl;}return 0;_LL666: if(*((int*)
_tmpA2A)!= 10)goto _LL668;_tmpA31=((struct Cyc_Absyn_AggrType_struct*)_tmpA2A)->f1;
_tmpA32=_tmpA31.aggr_info;if((((((struct Cyc_Absyn_AggrType_struct*)_tmpA2A)->f1).aggr_info).UnknownAggr).tag
!= 0)goto _LL668;_LL667: {const char*_tmp1061;void*_tmp1060;(_tmp1060=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1061="got unknown aggr in is_noalias_aggr",
_tag_dyneither(_tmp1061,sizeof(char),36))),_tag_dyneither(_tmp1060,sizeof(void*),
0)));}_LL668: if(*((int*)_tmpA2A)!= 2)goto _LL66A;_tmpA33=((struct Cyc_Absyn_TunionType_struct*)
_tmpA2A)->f1;_tmpA34=_tmpA33.tunion_info;_tmpA35=_tmpA33.targs;_tmpA36=_tmpA33.rgn;
_LL669: {union Cyc_Absyn_TunionInfoU_union _tmpA3E=_tmpA34;struct Cyc_Absyn_UnknownTunionInfo
_tmpA3F;struct _tuple2*_tmpA40;int _tmpA41;int _tmpA42;struct Cyc_Absyn_Tuniondecl**
_tmpA43;struct Cyc_Absyn_Tuniondecl*_tmpA44;struct Cyc_Absyn_Tuniondecl _tmpA45;
struct Cyc_List_List*_tmpA46;struct Cyc_Core_Opt*_tmpA47;int _tmpA48;_LL66F: if((
_tmpA3E.UnknownTunion).tag != 0)goto _LL671;_tmpA3F=(_tmpA3E.UnknownTunion).f1;
_tmpA40=_tmpA3F.name;_tmpA41=_tmpA3F.is_xtunion;_tmpA42=_tmpA3F.is_flat;_LL670: {
const char*_tmp1064;void*_tmp1063;(_tmp1063=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1064="got unknown tunion in is_noalias_aggr",
_tag_dyneither(_tmp1064,sizeof(char),38))),_tag_dyneither(_tmp1063,sizeof(void*),
0)));}_LL671: if((_tmpA3E.KnownTunion).tag != 1)goto _LL66E;_tmpA43=(_tmpA3E.KnownTunion).f1;
_tmpA44=*_tmpA43;_tmpA45=*_tmpA44;_tmpA46=_tmpA45.tvs;_tmpA47=_tmpA45.fields;
_tmpA48=_tmpA45.is_flat;_LL672: if(!_tmpA48){if(_tmpA36 == 0)return 0;return Cyc_Tcutil_is_noalias_region((
void*)_tmpA36->v);}else{if(_tmpA47 == 0){const char*_tmp1067;void*_tmp1066;(
_tmp1066=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp1067="got tunion with no fields",_tag_dyneither(_tmp1067,sizeof(char),26))),
_tag_dyneither(_tmp1066,sizeof(void*),0)));}else{struct Cyc_List_List*_tmpA4D=((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmpA46,_tmpA35);struct Cyc_List_List*
_tmpA4E=(struct Cyc_List_List*)_tmpA47->v;while(_tmpA4E != 0){struct Cyc_List_List*
_tmpA4F=((struct Cyc_Absyn_Tunionfield*)_tmpA4E->hd)->typs;while(_tmpA4F != 0){
_tmpA29=Cyc_Tcutil_rsubstitute(rgn,_tmpA4D,(*((struct _tuple5*)_tmpA4F->hd)).f2);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,_tmpA29))return 1;_tmpA4F=_tmpA4F->tl;}
_tmpA4E=_tmpA4E->tl;}return 0;}}_LL66E:;}_LL66A: if(*((int*)_tmpA2A)!= 3)goto
_LL66C;_tmpA37=((struct Cyc_Absyn_TunionFieldType_struct*)_tmpA2A)->f1;_tmpA38=
_tmpA37.field_info;_tmpA39=_tmpA37.targs;_LL66B: {union Cyc_Absyn_TunionFieldInfoU_union
_tmpA50=_tmpA38;struct Cyc_Absyn_Tuniondecl*_tmpA51;struct Cyc_Absyn_Tunionfield*
_tmpA52;_LL674: if((_tmpA50.UnknownTunionfield).tag != 0)goto _LL676;_LL675: {const
char*_tmp106A;void*_tmp1069;(_tmp1069=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp106A="got unknown tunion field in is_noalias_aggr",
_tag_dyneither(_tmp106A,sizeof(char),44))),_tag_dyneither(_tmp1069,sizeof(void*),
0)));}_LL676: if((_tmpA50.KnownTunionfield).tag != 1)goto _LL673;_tmpA51=(_tmpA50.KnownTunionfield).f1;
_tmpA52=(_tmpA50.KnownTunionfield).f2;_LL677: {struct Cyc_List_List*_tmpA55=((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmpA51->tvs,_tmpA39);struct Cyc_List_List*
_tmpA56=_tmpA52->typs;while(_tmpA56 != 0){_tmpA29=Cyc_Tcutil_rsubstitute(rgn,
_tmpA55,(*((struct _tuple5*)_tmpA56->hd)).f2);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(
rgn,_tmpA29))return 1;_tmpA56=_tmpA56->tl;}return 0;}_LL673:;}_LL66C:;_LL66D:
return 0;_LL65F:;}}static int Cyc_Tcutil_is_noalias_field(struct _RegionHandle*r,
void*t,struct _dyneither_ptr*f);static int Cyc_Tcutil_is_noalias_field(struct
_RegionHandle*r,void*t,struct _dyneither_ptr*f){void*_tmpA57=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_AggrInfo _tmpA58;union Cyc_Absyn_AggrInfoU_union _tmpA59;struct
Cyc_Absyn_Aggrdecl**_tmpA5A;struct Cyc_Absyn_Aggrdecl*_tmpA5B;struct Cyc_List_List*
_tmpA5C;struct Cyc_List_List*_tmpA5D;_LL679: if(_tmpA57 <= (void*)4)goto _LL67D;if(*((
int*)_tmpA57)!= 10)goto _LL67B;_tmpA58=((struct Cyc_Absyn_AggrType_struct*)_tmpA57)->f1;
_tmpA59=_tmpA58.aggr_info;if((((((struct Cyc_Absyn_AggrType_struct*)_tmpA57)->f1).aggr_info).KnownAggr).tag
!= 1)goto _LL67B;_tmpA5A=(_tmpA59.KnownAggr).f1;_tmpA5B=*_tmpA5A;_tmpA5C=_tmpA58.targs;
_LL67A: _tmpA5D=_tmpA5B->impl == 0?0:((struct Cyc_Absyn_AggrdeclImpl*)_check_null(
_tmpA5B->impl))->fields;goto _LL67C;_LL67B: if(*((int*)_tmpA57)!= 11)goto _LL67D;
_tmpA5D=((struct Cyc_Absyn_AnonAggrType_struct*)_tmpA57)->f2;_LL67C: {struct Cyc_Absyn_Aggrfield*
_tmpA5E=Cyc_Absyn_lookup_field(_tmpA5D,f);{struct Cyc_Absyn_Aggrfield*_tmpA5F=
_tmpA5E;struct Cyc_Absyn_Aggrfield _tmpA60;void*_tmpA61;_LL680: if(_tmpA5F != 0)goto
_LL682;_LL681: {const char*_tmp106D;void*_tmp106C;(_tmp106C=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp106D="is_noalias_field: missing field",
_tag_dyneither(_tmp106D,sizeof(char),32))),_tag_dyneither(_tmp106C,sizeof(void*),
0)));}_LL682: if(_tmpA5F == 0)goto _LL67F;_tmpA60=*_tmpA5F;_tmpA61=(void*)_tmpA60.type;
_LL683: return Cyc_Tcutil_is_noalias_pointer_or_aggr(r,_tmpA61);_LL67F:;}return 0;}
_LL67D:;_LL67E: {const char*_tmp1071;void*_tmp1070[1];struct Cyc_String_pa_struct
_tmp106F;(_tmp106F.tag=0,((_tmp106F.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp1070[0]=& _tmp106F,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1071="is_noalias_field: invalid type |%s|",
_tag_dyneither(_tmp1071,sizeof(char),36))),_tag_dyneither(_tmp1070,sizeof(void*),
1)))))));}_LL678:;}static int Cyc_Tcutil_is_noalias_path_aux(struct _RegionHandle*r,
struct Cyc_Absyn_Exp*e,int ignore_leaf);static int Cyc_Tcutil_is_noalias_path_aux(
struct _RegionHandle*r,struct Cyc_Absyn_Exp*e,int ignore_leaf){void*_tmpA67=(void*)
e->r;void*_tmpA68;struct Cyc_Absyn_Exp*_tmpA69;struct _dyneither_ptr*_tmpA6A;
struct Cyc_Absyn_Exp*_tmpA6B;struct Cyc_Absyn_Exp*_tmpA6C;void*_tmpA6D;void*
_tmpA6E;void*_tmpA6F;struct Cyc_Absyn_Exp*_tmpA70;struct Cyc_Absyn_Exp*_tmpA71;
struct Cyc_Absyn_Exp*_tmpA72;struct Cyc_Absyn_Exp*_tmpA73;void*_tmpA74;struct Cyc_Absyn_Exp*
_tmpA75;struct Cyc_Absyn_Stmt*_tmpA76;_LL685: if(*((int*)_tmpA67)!= 5)goto _LL687;
_LL686: goto _LL688;_LL687: if(*((int*)_tmpA67)!= 7)goto _LL689;_LL688: goto _LL68A;
_LL689: if(*((int*)_tmpA67)!= 8)goto _LL68B;_LL68A: goto _LL68C;_LL68B: if(*((int*)
_tmpA67)!= 12)goto _LL68D;_LL68C: goto _LL68E;_LL68D: if(*((int*)_tmpA67)!= 16)goto
_LL68F;_LL68E: goto _LL690;_LL68F: if(*((int*)_tmpA67)!= 18)goto _LL691;_LL690: goto
_LL692;_LL691: if(*((int*)_tmpA67)!= 19)goto _LL693;_LL692: goto _LL694;_LL693: if(*((
int*)_tmpA67)!= 20)goto _LL695;_LL694: goto _LL696;_LL695: if(*((int*)_tmpA67)!= 21)
goto _LL697;_LL696: goto _LL698;_LL697: if(*((int*)_tmpA67)!= 27)goto _LL699;_LL698:
goto _LL69A;_LL699: if(*((int*)_tmpA67)!= 29)goto _LL69B;_LL69A: goto _LL69C;_LL69B:
if(*((int*)_tmpA67)!= 28)goto _LL69D;_LL69C: goto _LL69E;_LL69D: if(*((int*)_tmpA67)
!= 33)goto _LL69F;_LL69E: goto _LL6A0;_LL69F: if(*((int*)_tmpA67)!= 34)goto _LL6A1;
_LL6A0: goto _LL6A2;_LL6A1: if(*((int*)_tmpA67)!= 36)goto _LL6A3;_LL6A2: goto _LL6A4;
_LL6A3: if(*((int*)_tmpA67)!= 1)goto _LL6A5;_tmpA68=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmpA67)->f2;if(_tmpA68 <= (void*)1)goto _LL6A5;if(*((int*)_tmpA68)!= 0)goto _LL6A5;
_LL6A4: goto _LL6A6;_LL6A5: if(*((int*)_tmpA67)!= 3)goto _LL6A7;_LL6A6: return 0;
_LL6A7: if(*((int*)_tmpA67)!= 22)goto _LL6A9;_LL6A8: goto _LL6AA;_LL6A9: if(*((int*)
_tmpA67)!= 24)goto _LL6AB;_LL6AA: return 0;_LL6AB: if(*((int*)_tmpA67)!= 23)goto
_LL6AD;_tmpA69=((struct Cyc_Absyn_AggrMember_e_struct*)_tmpA67)->f1;_tmpA6A=((
struct Cyc_Absyn_AggrMember_e_struct*)_tmpA67)->f2;_LL6AC: return(ignore_leaf  || 
Cyc_Tcutil_is_noalias_field(r,(void*)((struct Cyc_Core_Opt*)_check_null(_tmpA69->topt))->v,
_tmpA6A)) && Cyc_Tcutil_is_noalias_path_aux(r,_tmpA69,0);_LL6AD: if(*((int*)
_tmpA67)!= 25)goto _LL6AF;_tmpA6B=((struct Cyc_Absyn_Subscript_e_struct*)_tmpA67)->f1;
_tmpA6C=((struct Cyc_Absyn_Subscript_e_struct*)_tmpA67)->f2;_LL6AE: {void*_tmpA77=
Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmpA6B->topt))->v);
struct Cyc_Absyn_PtrInfo _tmpA78;void*_tmpA79;struct Cyc_List_List*_tmpA7A;_LL6D2:
if(_tmpA77 <= (void*)4)goto _LL6D6;if(*((int*)_tmpA77)!= 4)goto _LL6D4;_tmpA78=((
struct Cyc_Absyn_PointerType_struct*)_tmpA77)->f1;_tmpA79=(void*)_tmpA78.elt_typ;
_LL6D3: return 0;_LL6D4: if(*((int*)_tmpA77)!= 9)goto _LL6D6;_tmpA7A=((struct Cyc_Absyn_TupleType_struct*)
_tmpA77)->f1;_LL6D5: {unsigned int _tmpA7C;int _tmpA7D;struct _tuple7 _tmpA7B=Cyc_Evexp_eval_const_uint_exp(
_tmpA6C);_tmpA7C=_tmpA7B.f1;_tmpA7D=_tmpA7B.f2;if(!_tmpA7D){const char*_tmp1074;
void*_tmp1073;(_tmp1073=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap))Cyc_Tcutil_impos)(((_tmp1074="is_noalias_path_aux: non-constant subscript",
_tag_dyneither(_tmp1074,sizeof(char),44))),_tag_dyneither(_tmp1073,sizeof(void*),
0)));}{struct _handler_cons _tmpA80;_push_handler(& _tmpA80);{int _tmpA82=0;if(
setjmp(_tmpA80.handler))_tmpA82=1;if(!_tmpA82){{void*_tmpA83=(*((struct _tuple5*(*)(
struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmpA7A,(int)_tmpA7C)).f2;int _tmpA84=(
ignore_leaf  || Cyc_Tcutil_is_noalias_pointer_or_aggr(r,_tmpA83)) && Cyc_Tcutil_is_noalias_path_aux(
r,_tmpA6B,0);_npop_handler(0);return _tmpA84;};_pop_handler();}else{void*_tmpA81=(
void*)_exn_thrown;void*_tmpA86=_tmpA81;_LL6D9: if(_tmpA86 != Cyc_List_Nth)goto
_LL6DB;_LL6DA: {const char*_tmp1077;void*_tmp1076;return(_tmp1076=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1077="is_noalias_path_aux: out-of-bounds subscript",
_tag_dyneither(_tmp1077,sizeof(char),45))),_tag_dyneither(_tmp1076,sizeof(void*),
0)));}_LL6DB:;_LL6DC:(void)_throw(_tmpA86);_LL6D8:;}}}}_LL6D6:;_LL6D7: {const
char*_tmp107A;void*_tmp1079;(_tmp1079=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp107A="is_noalias_path_aux: subscript on non-pointer/tuple",
_tag_dyneither(_tmp107A,sizeof(char),52))),_tag_dyneither(_tmp1079,sizeof(void*),
0)));}_LL6D1:;}_LL6AF: if(*((int*)_tmpA67)!= 32)goto _LL6B1;_LL6B0: goto _LL6B2;
_LL6B1: if(*((int*)_tmpA67)!= 26)goto _LL6B3;_LL6B2: goto _LL6B4;_LL6B3: if(*((int*)
_tmpA67)!= 30)goto _LL6B5;_LL6B4: goto _LL6B6;_LL6B5: if(*((int*)_tmpA67)!= 31)goto
_LL6B7;_LL6B6: goto _LL6B8;_LL6B7: if(*((int*)_tmpA67)!= 0)goto _LL6B9;_LL6B8: goto
_LL6BA;_LL6B9: if(*((int*)_tmpA67)!= 35)goto _LL6BB;_LL6BA: goto _LL6BC;_LL6BB: if(*((
int*)_tmpA67)!= 17)goto _LL6BD;_LL6BC: goto _LL6BE;_LL6BD: if(*((int*)_tmpA67)!= 1)
goto _LL6BF;_tmpA6D=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmpA67)->f2;if(
_tmpA6D <= (void*)1)goto _LL6BF;if(*((int*)_tmpA6D)!= 3)goto _LL6BF;_LL6BE: goto
_LL6C0;_LL6BF: if(*((int*)_tmpA67)!= 1)goto _LL6C1;_tmpA6E=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmpA67)->f2;if(_tmpA6E <= (void*)1)goto _LL6C1;if(*((int*)_tmpA6E)!= 4)goto _LL6C1;
_LL6C0: goto _LL6C2;_LL6C1: if(*((int*)_tmpA67)!= 1)goto _LL6C3;_tmpA6F=(void*)((
struct Cyc_Absyn_Var_e_struct*)_tmpA67)->f2;if(_tmpA6F <= (void*)1)goto _LL6C3;if(*((
int*)_tmpA6F)!= 2)goto _LL6C3;_LL6C2: {int _tmpA8B=ignore_leaf  || Cyc_Tcutil_is_noalias_pointer_or_aggr(
r,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);return _tmpA8B;}_LL6C3:
if(*((int*)_tmpA67)!= 6)goto _LL6C5;_tmpA70=((struct Cyc_Absyn_Conditional_e_struct*)
_tmpA67)->f2;_LL6C4: _tmpA71=_tmpA70;goto _LL6C6;_LL6C5: if(*((int*)_tmpA67)!= 9)
goto _LL6C7;_tmpA71=((struct Cyc_Absyn_SeqExp_e_struct*)_tmpA67)->f2;_LL6C6:
_tmpA72=_tmpA71;goto _LL6C8;_LL6C7: if(*((int*)_tmpA67)!= 4)goto _LL6C9;_tmpA72=((
struct Cyc_Absyn_AssignOp_e_struct*)_tmpA67)->f1;_LL6C8: return Cyc_Tcutil_is_noalias_path_aux(
r,_tmpA72,ignore_leaf);_LL6C9: if(*((int*)_tmpA67)!= 11)goto _LL6CB;_tmpA73=((
struct Cyc_Absyn_FnCall_e_struct*)_tmpA67)->f1;_LL6CA: {void*_tmpA8C=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpA73->topt))->v);struct Cyc_Absyn_FnInfo
_tmpA8D;void*_tmpA8E;struct Cyc_Absyn_PtrInfo _tmpA8F;void*_tmpA90;_LL6DE: if(
_tmpA8C <= (void*)4)goto _LL6E2;if(*((int*)_tmpA8C)!= 8)goto _LL6E0;_tmpA8D=((
struct Cyc_Absyn_FnType_struct*)_tmpA8C)->f1;_tmpA8E=(void*)_tmpA8D.ret_typ;
_LL6DF: return ignore_leaf  || Cyc_Tcutil_is_noalias_pointer_or_aggr(r,_tmpA8E);
_LL6E0: if(*((int*)_tmpA8C)!= 4)goto _LL6E2;_tmpA8F=((struct Cyc_Absyn_PointerType_struct*)
_tmpA8C)->f1;_tmpA90=(void*)_tmpA8F.elt_typ;_LL6E1:{void*_tmpA91=Cyc_Tcutil_compress(
_tmpA90);struct Cyc_Absyn_FnInfo _tmpA92;void*_tmpA93;_LL6E5: if(_tmpA91 <= (void*)4)
goto _LL6E7;if(*((int*)_tmpA91)!= 8)goto _LL6E7;_tmpA92=((struct Cyc_Absyn_FnType_struct*)
_tmpA91)->f1;_tmpA93=(void*)_tmpA92.ret_typ;_LL6E6: return ignore_leaf  || Cyc_Tcutil_is_noalias_pointer_or_aggr(
r,_tmpA93);_LL6E7:;_LL6E8: goto _LL6E4;_LL6E4:;}goto _LL6E3;_LL6E2:;_LL6E3: {const
char*_tmp107E;void*_tmp107D[1];struct Cyc_String_pa_struct _tmp107C;(_tmp107C.tag=
0,((_tmp107C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpA73->topt))->v)),((_tmp107D[0]=&
_tmp107C,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp107E="Fncall has non function type %s",_tag_dyneither(_tmp107E,sizeof(char),
32))),_tag_dyneither(_tmp107D,sizeof(void*),1)))))));}_LL6DD:;}_LL6CB: if(*((int*)
_tmpA67)!= 15)goto _LL6CD;_tmpA74=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmpA67)->f1;
_tmpA75=((struct Cyc_Absyn_Cast_e_struct*)_tmpA67)->f2;_LL6CC: return Cyc_Tcutil_is_noalias_pointer_or_aggr(
r,_tmpA74) && Cyc_Tcutil_is_noalias_path_aux(r,_tmpA75,1);_LL6CD: if(*((int*)
_tmpA67)!= 38)goto _LL6CF;_tmpA76=((struct Cyc_Absyn_StmtExp_e_struct*)_tmpA67)->f1;
_LL6CE: while(1){void*_tmpA97=(void*)_tmpA76->r;struct Cyc_Absyn_Stmt*_tmpA98;
struct Cyc_Absyn_Stmt*_tmpA99;struct Cyc_Absyn_Decl*_tmpA9A;struct Cyc_Absyn_Stmt*
_tmpA9B;struct Cyc_Absyn_Exp*_tmpA9C;_LL6EA: if(_tmpA97 <= (void*)1)goto _LL6F0;if(*((
int*)_tmpA97)!= 1)goto _LL6EC;_tmpA98=((struct Cyc_Absyn_Seq_s_struct*)_tmpA97)->f1;
_tmpA99=((struct Cyc_Absyn_Seq_s_struct*)_tmpA97)->f2;_LL6EB: _tmpA76=_tmpA99;goto
_LL6E9;_LL6EC: if(*((int*)_tmpA97)!= 11)goto _LL6EE;_tmpA9A=((struct Cyc_Absyn_Decl_s_struct*)
_tmpA97)->f1;_tmpA9B=((struct Cyc_Absyn_Decl_s_struct*)_tmpA97)->f2;_LL6ED:
_tmpA76=_tmpA9B;goto _LL6E9;_LL6EE: if(*((int*)_tmpA97)!= 0)goto _LL6F0;_tmpA9C=((
struct Cyc_Absyn_Exp_s_struct*)_tmpA97)->f1;_LL6EF: return Cyc_Tcutil_is_noalias_path_aux(
r,_tmpA9C,ignore_leaf);_LL6F0:;_LL6F1: {const char*_tmp1081;void*_tmp1080;(
_tmp1080=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp1081="is_noalias_stmt_exp: ill-formed StmtExp",_tag_dyneither(_tmp1081,
sizeof(char),40))),_tag_dyneither(_tmp1080,sizeof(void*),0)));}_LL6E9:;}_LL6CF:;
_LL6D0: return 0;_LL684:;}int Cyc_Tcutil_is_noalias_path(struct _RegionHandle*r,
struct Cyc_Absyn_Exp*e);int Cyc_Tcutil_is_noalias_path(struct _RegionHandle*r,
struct Cyc_Absyn_Exp*e){return Cyc_Tcutil_is_noalias_path_aux(r,e,0);}struct
_tuple10 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e);
struct _tuple10 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*
e){struct _tuple10 _tmp1082;struct _tuple10 bogus_ans=(_tmp1082.f1=0,((_tmp1082.f2=(
void*)2,_tmp1082)));void*_tmpA9F=(void*)e->r;struct _tuple2*_tmpAA0;void*_tmpAA1;
struct Cyc_Absyn_Exp*_tmpAA2;struct _dyneither_ptr*_tmpAA3;struct Cyc_Absyn_Exp*
_tmpAA4;struct _dyneither_ptr*_tmpAA5;struct Cyc_Absyn_Exp*_tmpAA6;struct Cyc_Absyn_Exp*
_tmpAA7;struct Cyc_Absyn_Exp*_tmpAA8;_LL6F3: if(*((int*)_tmpA9F)!= 1)goto _LL6F5;
_tmpAA0=((struct Cyc_Absyn_Var_e_struct*)_tmpA9F)->f1;_tmpAA1=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmpA9F)->f2;_LL6F4: {void*_tmpAA9=_tmpAA1;struct Cyc_Absyn_Vardecl*_tmpAAA;
struct Cyc_Absyn_Vardecl*_tmpAAB;struct Cyc_Absyn_Vardecl*_tmpAAC;struct Cyc_Absyn_Vardecl*
_tmpAAD;_LL700: if((int)_tmpAA9 != 0)goto _LL702;_LL701: goto _LL703;_LL702: if(
_tmpAA9 <= (void*)1)goto _LL704;if(*((int*)_tmpAA9)!= 1)goto _LL704;_LL703: return
bogus_ans;_LL704: if(_tmpAA9 <= (void*)1)goto _LL706;if(*((int*)_tmpAA9)!= 0)goto
_LL706;_tmpAAA=((struct Cyc_Absyn_Global_b_struct*)_tmpAA9)->f1;_LL705: {void*
_tmpAAE=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
_LL70D: if(_tmpAAE <= (void*)4)goto _LL70F;if(*((int*)_tmpAAE)!= 7)goto _LL70F;
_LL70E: {struct _tuple10 _tmp1083;return(_tmp1083.f1=1,((_tmp1083.f2=(void*)2,
_tmp1083)));}_LL70F:;_LL710: {struct _tuple10 _tmp1084;return(_tmp1084.f1=(_tmpAAA->tq).real_const,((
_tmp1084.f2=(void*)2,_tmp1084)));}_LL70C:;}_LL706: if(_tmpAA9 <= (void*)1)goto
_LL708;if(*((int*)_tmpAA9)!= 3)goto _LL708;_tmpAAB=((struct Cyc_Absyn_Local_b_struct*)
_tmpAA9)->f1;_LL707: {void*_tmpAB1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);_LL712: if(_tmpAB1 <= (void*)4)goto _LL714;if(*((int*)
_tmpAB1)!= 7)goto _LL714;_LL713: {struct _tuple10 _tmp1085;return(_tmp1085.f1=1,((
_tmp1085.f2=(void*)((struct Cyc_Core_Opt*)_check_null(_tmpAAB->rgn))->v,_tmp1085)));}
_LL714:;_LL715: _tmpAAB->escapes=1;{struct _tuple10 _tmp1086;return(_tmp1086.f1=(
_tmpAAB->tq).real_const,((_tmp1086.f2=(void*)((struct Cyc_Core_Opt*)_check_null(
_tmpAAB->rgn))->v,_tmp1086)));}_LL711:;}_LL708: if(_tmpAA9 <= (void*)1)goto _LL70A;
if(*((int*)_tmpAA9)!= 4)goto _LL70A;_tmpAAC=((struct Cyc_Absyn_Pat_b_struct*)
_tmpAA9)->f1;_LL709: _tmpAAD=_tmpAAC;goto _LL70B;_LL70A: if(_tmpAA9 <= (void*)1)goto
_LL6FF;if(*((int*)_tmpAA9)!= 2)goto _LL6FF;_tmpAAD=((struct Cyc_Absyn_Param_b_struct*)
_tmpAA9)->f1;_LL70B: _tmpAAD->escapes=1;{struct _tuple10 _tmp1087;return(_tmp1087.f1=(
_tmpAAD->tq).real_const,((_tmp1087.f2=(void*)((struct Cyc_Core_Opt*)_check_null(
_tmpAAD->rgn))->v,_tmp1087)));}_LL6FF:;}_LL6F5: if(*((int*)_tmpA9F)!= 23)goto
_LL6F7;_tmpAA2=((struct Cyc_Absyn_AggrMember_e_struct*)_tmpA9F)->f1;_tmpAA3=((
struct Cyc_Absyn_AggrMember_e_struct*)_tmpA9F)->f2;_LL6F6: {void*_tmpAB5=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpAA2->topt))->v);struct Cyc_List_List*
_tmpAB6;struct Cyc_Absyn_AggrInfo _tmpAB7;union Cyc_Absyn_AggrInfoU_union _tmpAB8;
struct Cyc_Absyn_Aggrdecl**_tmpAB9;struct Cyc_Absyn_Aggrdecl*_tmpABA;_LL717: if(
_tmpAB5 <= (void*)4)goto _LL71B;if(*((int*)_tmpAB5)!= 11)goto _LL719;_tmpAB6=((
struct Cyc_Absyn_AnonAggrType_struct*)_tmpAB5)->f2;_LL718: {struct Cyc_Absyn_Aggrfield*
_tmpABB=Cyc_Absyn_lookup_field(_tmpAB6,_tmpAA3);if(_tmpABB != 0  && _tmpABB->width
!= 0){struct _tuple10 _tmp1088;return(_tmp1088.f1=(_tmpABB->tq).real_const,((
_tmp1088.f2=(Cyc_Tcutil_addressof_props(te,_tmpAA2)).f2,_tmp1088)));}return
bogus_ans;}_LL719: if(*((int*)_tmpAB5)!= 10)goto _LL71B;_tmpAB7=((struct Cyc_Absyn_AggrType_struct*)
_tmpAB5)->f1;_tmpAB8=_tmpAB7.aggr_info;if((((((struct Cyc_Absyn_AggrType_struct*)
_tmpAB5)->f1).aggr_info).KnownAggr).tag != 1)goto _LL71B;_tmpAB9=(_tmpAB8.KnownAggr).f1;
_tmpABA=*_tmpAB9;_LL71A: {struct Cyc_Absyn_Aggrfield*_tmpABD=Cyc_Absyn_lookup_decl_field(
_tmpABA,_tmpAA3);if(_tmpABD != 0  && _tmpABD->width != 0){struct _tuple10 _tmp1089;
return(_tmp1089.f1=(_tmpABD->tq).real_const,((_tmp1089.f2=(Cyc_Tcutil_addressof_props(
te,_tmpAA2)).f2,_tmp1089)));}return bogus_ans;}_LL71B:;_LL71C: return bogus_ans;
_LL716:;}_LL6F7: if(*((int*)_tmpA9F)!= 24)goto _LL6F9;_tmpAA4=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmpA9F)->f1;_tmpAA5=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmpA9F)->f2;_LL6F8: {
void*_tmpABF=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmpAA4->topt))->v);struct Cyc_Absyn_PtrInfo _tmpAC0;void*_tmpAC1;struct Cyc_Absyn_PtrAtts
_tmpAC2;void*_tmpAC3;_LL71E: if(_tmpABF <= (void*)4)goto _LL720;if(*((int*)_tmpABF)
!= 4)goto _LL720;_tmpAC0=((struct Cyc_Absyn_PointerType_struct*)_tmpABF)->f1;
_tmpAC1=(void*)_tmpAC0.elt_typ;_tmpAC2=_tmpAC0.ptr_atts;_tmpAC3=(void*)_tmpAC2.rgn;
_LL71F: {struct Cyc_Absyn_Aggrfield*finfo;{void*_tmpAC4=Cyc_Tcutil_compress(
_tmpAC1);struct Cyc_List_List*_tmpAC5;struct Cyc_Absyn_AggrInfo _tmpAC6;union Cyc_Absyn_AggrInfoU_union
_tmpAC7;struct Cyc_Absyn_Aggrdecl**_tmpAC8;struct Cyc_Absyn_Aggrdecl*_tmpAC9;
_LL723: if(_tmpAC4 <= (void*)4)goto _LL727;if(*((int*)_tmpAC4)!= 11)goto _LL725;
_tmpAC5=((struct Cyc_Absyn_AnonAggrType_struct*)_tmpAC4)->f2;_LL724: finfo=Cyc_Absyn_lookup_field(
_tmpAC5,_tmpAA5);goto _LL722;_LL725: if(*((int*)_tmpAC4)!= 10)goto _LL727;_tmpAC6=((
struct Cyc_Absyn_AggrType_struct*)_tmpAC4)->f1;_tmpAC7=_tmpAC6.aggr_info;if((((((
struct Cyc_Absyn_AggrType_struct*)_tmpAC4)->f1).aggr_info).KnownAggr).tag != 1)
goto _LL727;_tmpAC8=(_tmpAC7.KnownAggr).f1;_tmpAC9=*_tmpAC8;_LL726: finfo=Cyc_Absyn_lookup_decl_field(
_tmpAC9,_tmpAA5);goto _LL722;_LL727:;_LL728: return bogus_ans;_LL722:;}if(finfo != 0
 && finfo->width != 0){struct _tuple10 _tmp108A;return(_tmp108A.f1=(finfo->tq).real_const,((
_tmp108A.f2=_tmpAC3,_tmp108A)));}return bogus_ans;}_LL720:;_LL721: return bogus_ans;
_LL71D:;}_LL6F9: if(*((int*)_tmpA9F)!= 22)goto _LL6FB;_tmpAA6=((struct Cyc_Absyn_Deref_e_struct*)
_tmpA9F)->f1;_LL6FA: {void*_tmpACB=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmpAA6->topt))->v);struct Cyc_Absyn_PtrInfo _tmpACC;struct Cyc_Absyn_Tqual
_tmpACD;struct Cyc_Absyn_PtrAtts _tmpACE;void*_tmpACF;_LL72A: if(_tmpACB <= (void*)4)
goto _LL72C;if(*((int*)_tmpACB)!= 4)goto _LL72C;_tmpACC=((struct Cyc_Absyn_PointerType_struct*)
_tmpACB)->f1;_tmpACD=_tmpACC.elt_tq;_tmpACE=_tmpACC.ptr_atts;_tmpACF=(void*)
_tmpACE.rgn;_LL72B: {struct _tuple10 _tmp108B;return(_tmp108B.f1=_tmpACD.real_const,((
_tmp108B.f2=_tmpACF,_tmp108B)));}_LL72C:;_LL72D: return bogus_ans;_LL729:;}_LL6FB:
if(*((int*)_tmpA9F)!= 25)goto _LL6FD;_tmpAA7=((struct Cyc_Absyn_Subscript_e_struct*)
_tmpA9F)->f1;_tmpAA8=((struct Cyc_Absyn_Subscript_e_struct*)_tmpA9F)->f2;_LL6FC: {
void*t=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmpAA7->topt))->v);
void*_tmpAD1=t;struct Cyc_List_List*_tmpAD2;struct Cyc_Absyn_PtrInfo _tmpAD3;struct
Cyc_Absyn_Tqual _tmpAD4;struct Cyc_Absyn_PtrAtts _tmpAD5;void*_tmpAD6;struct Cyc_Absyn_ArrayInfo
_tmpAD7;struct Cyc_Absyn_Tqual _tmpAD8;_LL72F: if(_tmpAD1 <= (void*)4)goto _LL735;if(*((
int*)_tmpAD1)!= 9)goto _LL731;_tmpAD2=((struct Cyc_Absyn_TupleType_struct*)_tmpAD1)->f1;
_LL730: {unsigned int _tmpADA;int _tmpADB;struct _tuple7 _tmpAD9=Cyc_Evexp_eval_const_uint_exp(
_tmpAA8);_tmpADA=_tmpAD9.f1;_tmpADB=_tmpAD9.f2;if(!_tmpADB)return bogus_ans;{
struct _tuple5*_tmpADC=Cyc_Absyn_lookup_tuple_field(_tmpAD2,(int)_tmpADA);if(
_tmpADC != 0){struct _tuple10 _tmp108C;return(_tmp108C.f1=((*_tmpADC).f1).real_const,((
_tmp108C.f2=(Cyc_Tcutil_addressof_props(te,_tmpAA7)).f2,_tmp108C)));}return
bogus_ans;}}_LL731: if(*((int*)_tmpAD1)!= 4)goto _LL733;_tmpAD3=((struct Cyc_Absyn_PointerType_struct*)
_tmpAD1)->f1;_tmpAD4=_tmpAD3.elt_tq;_tmpAD5=_tmpAD3.ptr_atts;_tmpAD6=(void*)
_tmpAD5.rgn;_LL732: {struct _tuple10 _tmp108D;return(_tmp108D.f1=_tmpAD4.real_const,((
_tmp108D.f2=_tmpAD6,_tmp108D)));}_LL733: if(*((int*)_tmpAD1)!= 7)goto _LL735;
_tmpAD7=((struct Cyc_Absyn_ArrayType_struct*)_tmpAD1)->f1;_tmpAD8=_tmpAD7.tq;
_LL734: {struct _tuple10 _tmp108E;return(_tmp108E.f1=_tmpAD8.real_const,((_tmp108E.f2=(
Cyc_Tcutil_addressof_props(te,_tmpAA7)).f2,_tmp108E)));}_LL735:;_LL736: return
bogus_ans;_LL72E:;}_LL6FD:;_LL6FE:{const char*_tmp1091;void*_tmp1090;(_tmp1090=0,
Cyc_Tcutil_terr(e->loc,((_tmp1091="unary & applied to non-lvalue",_tag_dyneither(
_tmp1091,sizeof(char),30))),_tag_dyneither(_tmp1090,sizeof(void*),0)));}return
bogus_ans;_LL6F2:;}void*Cyc_Tcutil_array_to_ptr(struct Cyc_Tcenv_Tenv*te,void*
e_typ,struct Cyc_Absyn_Exp*e);void*Cyc_Tcutil_array_to_ptr(struct Cyc_Tcenv_Tenv*
te,void*e_typ,struct Cyc_Absyn_Exp*e){void*_tmpAE3=Cyc_Tcutil_compress(e_typ);
struct Cyc_Absyn_ArrayInfo _tmpAE4;void*_tmpAE5;struct Cyc_Absyn_Tqual _tmpAE6;
struct Cyc_Absyn_Conref*_tmpAE7;_LL738: if(_tmpAE3 <= (void*)4)goto _LL73A;if(*((int*)
_tmpAE3)!= 7)goto _LL73A;_tmpAE4=((struct Cyc_Absyn_ArrayType_struct*)_tmpAE3)->f1;
_tmpAE5=(void*)_tmpAE4.elt_type;_tmpAE6=_tmpAE4.tq;_tmpAE7=_tmpAE4.zero_term;
_LL739: {void*_tmpAE9;struct _tuple10 _tmpAE8=Cyc_Tcutil_addressof_props(te,e);
_tmpAE9=_tmpAE8.f2;{struct Cyc_Absyn_Upper_b_struct _tmp1094;struct Cyc_Absyn_Upper_b_struct*
_tmp1093;return Cyc_Absyn_atb_typ(_tmpAE5,_tmpAE9,_tmpAE6,(void*)((_tmp1093=
_cycalloc(sizeof(*_tmp1093)),((_tmp1093[0]=((_tmp1094.tag=0,((_tmp1094.f1=e,
_tmp1094)))),_tmp1093)))),_tmpAE7);}}_LL73A:;_LL73B: return e_typ;_LL737:;}void Cyc_Tcutil_check_bound(
struct Cyc_Position_Segment*loc,unsigned int i,struct Cyc_Absyn_Conref*b);void Cyc_Tcutil_check_bound(
struct Cyc_Position_Segment*loc,unsigned int i,struct Cyc_Absyn_Conref*b){b=Cyc_Absyn_compress_conref(
b);{union Cyc_Absyn_Constraint_union _tmpAEC=b->v;void*_tmpAED;void*_tmpAEE;struct
Cyc_Absyn_Exp*_tmpAEF;_LL73D: if((_tmpAEC.Eq_constr).tag != 0)goto _LL73F;_tmpAED=(
_tmpAEC.Eq_constr).f1;if((int)_tmpAED != 0)goto _LL73F;_LL73E: return;_LL73F: if((
_tmpAEC.Eq_constr).tag != 0)goto _LL741;_tmpAEE=(_tmpAEC.Eq_constr).f1;if(_tmpAEE
<= (void*)1)goto _LL741;if(*((int*)_tmpAEE)!= 0)goto _LL741;_tmpAEF=((struct Cyc_Absyn_Upper_b_struct*)
_tmpAEE)->f1;_LL740: {unsigned int _tmpAF1;int _tmpAF2;struct _tuple7 _tmpAF0=Cyc_Evexp_eval_const_uint_exp(
_tmpAEF);_tmpAF1=_tmpAF0.f1;_tmpAF2=_tmpAF0.f2;if(_tmpAF2  && _tmpAF1 <= i){const
char*_tmp1099;void*_tmp1098[2];struct Cyc_Int_pa_struct _tmp1097;struct Cyc_Int_pa_struct
_tmp1096;(_tmp1096.tag=1,((_tmp1096.f1=(unsigned long)((int)i),((_tmp1097.tag=1,((
_tmp1097.f1=(unsigned long)((int)_tmpAF1),((_tmp1098[0]=& _tmp1097,((_tmp1098[1]=&
_tmp1096,Cyc_Tcutil_terr(loc,((_tmp1099="dereference is out of bounds: %d <= %d",
_tag_dyneither(_tmp1099,sizeof(char),39))),_tag_dyneither(_tmp1098,sizeof(void*),
2)))))))))))));}return;}_LL741: if((_tmpAEC.No_constr).tag != 2)goto _LL743;_LL742:{
struct Cyc_Absyn_Upper_b_struct*_tmp109F;struct Cyc_Absyn_Upper_b_struct _tmp109E;
union Cyc_Absyn_Constraint_union _tmp109D;b->v=(union Cyc_Absyn_Constraint_union)(((
_tmp109D.Eq_constr).tag=0,(((_tmp109D.Eq_constr).f1=(void*)((void*)((void*)((
_tmp109F=_cycalloc(sizeof(*_tmp109F)),((_tmp109F[0]=((_tmp109E.tag=0,((_tmp109E.f1=
Cyc_Absyn_uint_exp(i + 1,0),_tmp109E)))),_tmp109F)))))),_tmp109D))));}return;
_LL743:;_LL744: {const char*_tmp10A2;void*_tmp10A1;(_tmp10A1=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp10A2="check_bound -- bad compressed conref",
_tag_dyneither(_tmp10A2,sizeof(char),37))),_tag_dyneither(_tmp10A1,sizeof(void*),
0)));}_LL73C:;}}void Cyc_Tcutil_check_nonzero_bound(struct Cyc_Position_Segment*
loc,struct Cyc_Absyn_Conref*b);void Cyc_Tcutil_check_nonzero_bound(struct Cyc_Position_Segment*
loc,struct Cyc_Absyn_Conref*b){Cyc_Tcutil_check_bound(loc,0,b);}int Cyc_Tcutil_is_bound_one(
struct Cyc_Absyn_Conref*b);int Cyc_Tcutil_is_bound_one(struct Cyc_Absyn_Conref*b){
union Cyc_Absyn_Constraint_union _tmpAFC=(Cyc_Absyn_compress_conref(b))->v;void*
_tmpAFD;struct Cyc_Absyn_Exp*_tmpAFE;_LL746: if((_tmpAFC.Eq_constr).tag != 0)goto
_LL748;_tmpAFD=(_tmpAFC.Eq_constr).f1;if(_tmpAFD <= (void*)1)goto _LL748;if(*((int*)
_tmpAFD)!= 0)goto _LL748;_tmpAFE=((struct Cyc_Absyn_Upper_b_struct*)_tmpAFD)->f1;
_LL747: {unsigned int _tmpB00;int _tmpB01;struct _tuple7 _tmpAFF=Cyc_Evexp_eval_const_uint_exp(
_tmpAFE);_tmpB00=_tmpAFF.f1;_tmpB01=_tmpAFF.f2;return _tmpB01  && _tmpB00 == 1;}
_LL748:;_LL749: return 0;_LL745:;}int Cyc_Tcutil_bits_only(void*t);int Cyc_Tcutil_bits_only(
void*t){void*_tmpB02=Cyc_Tcutil_compress(t);struct Cyc_Absyn_ArrayInfo _tmpB03;
void*_tmpB04;struct Cyc_Absyn_Conref*_tmpB05;struct Cyc_List_List*_tmpB06;struct
Cyc_Absyn_AggrInfo _tmpB07;union Cyc_Absyn_AggrInfoU_union _tmpB08;struct Cyc_Absyn_AggrInfo
_tmpB09;union Cyc_Absyn_AggrInfoU_union _tmpB0A;struct Cyc_Absyn_Aggrdecl**_tmpB0B;
struct Cyc_Absyn_Aggrdecl*_tmpB0C;struct Cyc_List_List*_tmpB0D;struct Cyc_List_List*
_tmpB0E;_LL74B: if((int)_tmpB02 != 0)goto _LL74D;_LL74C: goto _LL74E;_LL74D: if(
_tmpB02 <= (void*)4)goto _LL74F;if(*((int*)_tmpB02)!= 5)goto _LL74F;_LL74E: goto
_LL750;_LL74F: if((int)_tmpB02 != 1)goto _LL751;_LL750: goto _LL752;_LL751: if(_tmpB02
<= (void*)4)goto _LL761;if(*((int*)_tmpB02)!= 6)goto _LL753;_LL752: return 1;_LL753:
if(*((int*)_tmpB02)!= 12)goto _LL755;_LL754: goto _LL756;_LL755: if(*((int*)_tmpB02)
!= 13)goto _LL757;_LL756: return 0;_LL757: if(*((int*)_tmpB02)!= 7)goto _LL759;
_tmpB03=((struct Cyc_Absyn_ArrayType_struct*)_tmpB02)->f1;_tmpB04=(void*)_tmpB03.elt_type;
_tmpB05=_tmpB03.zero_term;_LL758: return !((int(*)(int,struct Cyc_Absyn_Conref*x))
Cyc_Absyn_conref_def)(0,_tmpB05) && Cyc_Tcutil_bits_only(_tmpB04);_LL759: if(*((
int*)_tmpB02)!= 9)goto _LL75B;_tmpB06=((struct Cyc_Absyn_TupleType_struct*)_tmpB02)->f1;
_LL75A: for(0;_tmpB06 != 0;_tmpB06=_tmpB06->tl){if(!Cyc_Tcutil_bits_only((*((
struct _tuple5*)_tmpB06->hd)).f2))return 0;}return 1;_LL75B: if(*((int*)_tmpB02)!= 
10)goto _LL75D;_tmpB07=((struct Cyc_Absyn_AggrType_struct*)_tmpB02)->f1;_tmpB08=
_tmpB07.aggr_info;if((((((struct Cyc_Absyn_AggrType_struct*)_tmpB02)->f1).aggr_info).UnknownAggr).tag
!= 0)goto _LL75D;_LL75C: return 0;_LL75D: if(*((int*)_tmpB02)!= 10)goto _LL75F;
_tmpB09=((struct Cyc_Absyn_AggrType_struct*)_tmpB02)->f1;_tmpB0A=_tmpB09.aggr_info;
if((((((struct Cyc_Absyn_AggrType_struct*)_tmpB02)->f1).aggr_info).KnownAggr).tag
!= 1)goto _LL75F;_tmpB0B=(_tmpB0A.KnownAggr).f1;_tmpB0C=*_tmpB0B;_tmpB0D=_tmpB09.targs;
_LL75E: if(_tmpB0C->impl == 0)return 0;{struct _RegionHandle _tmpB0F=_new_region("rgn");
struct _RegionHandle*rgn=& _tmpB0F;_push_region(rgn);{struct Cyc_List_List*_tmpB10=((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmpB0C->tvs,_tmpB0D);{struct Cyc_List_List*
fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpB0C->impl))->fields;for(0;fs
!= 0;fs=fs->tl){if(!Cyc_Tcutil_bits_only(Cyc_Tcutil_rsubstitute(rgn,_tmpB10,(
void*)((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))){int _tmpB11=0;_npop_handler(0);
return _tmpB11;}}}{int _tmpB12=1;_npop_handler(0);return _tmpB12;}};_pop_region(rgn);}
_LL75F: if(*((int*)_tmpB02)!= 11)goto _LL761;_tmpB0E=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmpB02)->f2;_LL760: for(0;_tmpB0E != 0;_tmpB0E=_tmpB0E->tl){if(!Cyc_Tcutil_bits_only((
void*)((struct Cyc_Absyn_Aggrfield*)_tmpB0E->hd)->type))return 0;}return 1;_LL761:;
_LL762: return 0;_LL74A:;}struct _tuple23{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*
f2;};static int Cyc_Tcutil_cnst_exp(struct Cyc_Tcenv_Tenv*te,int var_okay,struct Cyc_Absyn_Exp*
e);static int Cyc_Tcutil_cnst_exp(struct Cyc_Tcenv_Tenv*te,int var_okay,struct Cyc_Absyn_Exp*
e){void*_tmpB13=(void*)e->r;struct _tuple2*_tmpB14;void*_tmpB15;struct Cyc_Absyn_Exp*
_tmpB16;struct Cyc_Absyn_Exp*_tmpB17;struct Cyc_Absyn_Exp*_tmpB18;struct Cyc_Absyn_Exp*
_tmpB19;struct Cyc_Absyn_Exp*_tmpB1A;struct Cyc_Absyn_Exp*_tmpB1B;struct Cyc_Absyn_Exp*
_tmpB1C;void*_tmpB1D;struct Cyc_Absyn_Exp*_tmpB1E;void*_tmpB1F;void*_tmpB20;
struct Cyc_Absyn_Exp*_tmpB21;struct Cyc_Absyn_Exp*_tmpB22;struct Cyc_Absyn_Exp*
_tmpB23;struct Cyc_Absyn_Exp*_tmpB24;struct Cyc_List_List*_tmpB25;struct Cyc_List_List*
_tmpB26;struct Cyc_List_List*_tmpB27;void*_tmpB28;struct Cyc_List_List*_tmpB29;
struct Cyc_List_List*_tmpB2A;struct Cyc_List_List*_tmpB2B;_LL764: if(*((int*)
_tmpB13)!= 0)goto _LL766;_LL765: goto _LL767;_LL766: if(*((int*)_tmpB13)!= 18)goto
_LL768;_LL767: goto _LL769;_LL768: if(*((int*)_tmpB13)!= 19)goto _LL76A;_LL769: goto
_LL76B;_LL76A: if(*((int*)_tmpB13)!= 20)goto _LL76C;_LL76B: goto _LL76D;_LL76C: if(*((
int*)_tmpB13)!= 21)goto _LL76E;_LL76D: goto _LL76F;_LL76E: if(*((int*)_tmpB13)!= 33)
goto _LL770;_LL76F: goto _LL771;_LL770: if(*((int*)_tmpB13)!= 34)goto _LL772;_LL771:
return 1;_LL772: if(*((int*)_tmpB13)!= 1)goto _LL774;_tmpB14=((struct Cyc_Absyn_Var_e_struct*)
_tmpB13)->f1;_tmpB15=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmpB13)->f2;_LL773: {
void*_tmpB2C=_tmpB15;struct Cyc_Absyn_Vardecl*_tmpB2D;_LL793: if(_tmpB2C <= (void*)
1)goto _LL797;if(*((int*)_tmpB2C)!= 1)goto _LL795;_LL794: return 1;_LL795: if(*((int*)
_tmpB2C)!= 0)goto _LL797;_tmpB2D=((struct Cyc_Absyn_Global_b_struct*)_tmpB2C)->f1;
_LL796: {void*_tmpB2E=Cyc_Tcutil_compress((void*)_tmpB2D->type);_LL79C: if(
_tmpB2E <= (void*)4)goto _LL7A0;if(*((int*)_tmpB2E)!= 7)goto _LL79E;_LL79D: goto
_LL79F;_LL79E: if(*((int*)_tmpB2E)!= 8)goto _LL7A0;_LL79F: return 1;_LL7A0:;_LL7A1:
return var_okay;_LL79B:;}_LL797: if((int)_tmpB2C != 0)goto _LL799;_LL798: return 0;
_LL799:;_LL79A: return var_okay;_LL792:;}_LL774: if(*((int*)_tmpB13)!= 6)goto _LL776;
_tmpB16=((struct Cyc_Absyn_Conditional_e_struct*)_tmpB13)->f1;_tmpB17=((struct Cyc_Absyn_Conditional_e_struct*)
_tmpB13)->f2;_tmpB18=((struct Cyc_Absyn_Conditional_e_struct*)_tmpB13)->f3;_LL775:
return(Cyc_Tcutil_cnst_exp(te,0,_tmpB16) && Cyc_Tcutil_cnst_exp(te,0,_tmpB17))
 && Cyc_Tcutil_cnst_exp(te,0,_tmpB18);_LL776: if(*((int*)_tmpB13)!= 9)goto _LL778;
_tmpB19=((struct Cyc_Absyn_SeqExp_e_struct*)_tmpB13)->f1;_tmpB1A=((struct Cyc_Absyn_SeqExp_e_struct*)
_tmpB13)->f2;_LL777: return Cyc_Tcutil_cnst_exp(te,0,_tmpB19) && Cyc_Tcutil_cnst_exp(
te,0,_tmpB1A);_LL778: if(*((int*)_tmpB13)!= 13)goto _LL77A;_tmpB1B=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmpB13)->f1;_LL779: _tmpB1C=_tmpB1B;goto _LL77B;_LL77A: if(*((int*)_tmpB13)!= 14)
goto _LL77C;_tmpB1C=((struct Cyc_Absyn_Instantiate_e_struct*)_tmpB13)->f1;_LL77B:
return Cyc_Tcutil_cnst_exp(te,var_okay,_tmpB1C);_LL77C: if(*((int*)_tmpB13)!= 15)
goto _LL77E;_tmpB1D=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmpB13)->f1;_tmpB1E=((
struct Cyc_Absyn_Cast_e_struct*)_tmpB13)->f2;_tmpB1F=(void*)((struct Cyc_Absyn_Cast_e_struct*)
_tmpB13)->f4;if((int)_tmpB1F != 1)goto _LL77E;_LL77D: return Cyc_Tcutil_cnst_exp(te,
var_okay,_tmpB1E);_LL77E: if(*((int*)_tmpB13)!= 15)goto _LL780;_tmpB20=(void*)((
struct Cyc_Absyn_Cast_e_struct*)_tmpB13)->f1;_tmpB21=((struct Cyc_Absyn_Cast_e_struct*)
_tmpB13)->f2;_LL77F: return Cyc_Tcutil_cnst_exp(te,var_okay,_tmpB21);_LL780: if(*((
int*)_tmpB13)!= 16)goto _LL782;_tmpB22=((struct Cyc_Absyn_Address_e_struct*)
_tmpB13)->f1;_LL781: return Cyc_Tcutil_cnst_exp(te,1,_tmpB22);_LL782: if(*((int*)
_tmpB13)!= 29)goto _LL784;_tmpB23=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmpB13)->f2;_tmpB24=((struct Cyc_Absyn_Comprehension_e_struct*)_tmpB13)->f3;
_LL783: return Cyc_Tcutil_cnst_exp(te,0,_tmpB23) && Cyc_Tcutil_cnst_exp(te,0,
_tmpB24);_LL784: if(*((int*)_tmpB13)!= 28)goto _LL786;_tmpB25=((struct Cyc_Absyn_Array_e_struct*)
_tmpB13)->f1;_LL785: _tmpB26=_tmpB25;goto _LL787;_LL786: if(*((int*)_tmpB13)!= 31)
goto _LL788;_tmpB26=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmpB13)->f2;_LL787:
_tmpB27=_tmpB26;goto _LL789;_LL788: if(*((int*)_tmpB13)!= 30)goto _LL78A;_tmpB27=((
struct Cyc_Absyn_Struct_e_struct*)_tmpB13)->f3;_LL789: for(0;_tmpB27 != 0;_tmpB27=
_tmpB27->tl){if(!Cyc_Tcutil_cnst_exp(te,0,(*((struct _tuple23*)_tmpB27->hd)).f2))
return 0;}return 1;_LL78A: if(*((int*)_tmpB13)!= 3)goto _LL78C;_tmpB28=(void*)((
struct Cyc_Absyn_Primop_e_struct*)_tmpB13)->f1;_tmpB29=((struct Cyc_Absyn_Primop_e_struct*)
_tmpB13)->f2;_LL78B: _tmpB2A=_tmpB29;goto _LL78D;_LL78C: if(*((int*)_tmpB13)!= 26)
goto _LL78E;_tmpB2A=((struct Cyc_Absyn_Tuple_e_struct*)_tmpB13)->f1;_LL78D: _tmpB2B=
_tmpB2A;goto _LL78F;_LL78E: if(*((int*)_tmpB13)!= 32)goto _LL790;_tmpB2B=((struct
Cyc_Absyn_Tunion_e_struct*)_tmpB13)->f1;_LL78F: for(0;_tmpB2B != 0;_tmpB2B=_tmpB2B->tl){
if(!Cyc_Tcutil_cnst_exp(te,0,(struct Cyc_Absyn_Exp*)_tmpB2B->hd))return 0;}return 1;
_LL790:;_LL791: return 0;_LL763:;}int Cyc_Tcutil_is_const_exp(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e);int Cyc_Tcutil_is_const_exp(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Absyn_Exp*e){return Cyc_Tcutil_cnst_exp(te,0,e);}static int Cyc_Tcutil_fields_support_default(
struct Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs);int Cyc_Tcutil_supports_default(
void*t);int Cyc_Tcutil_supports_default(void*t){void*_tmpB2F=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_PtrInfo _tmpB30;struct Cyc_Absyn_PtrAtts _tmpB31;struct Cyc_Absyn_Conref*
_tmpB32;struct Cyc_Absyn_Conref*_tmpB33;struct Cyc_Absyn_ArrayInfo _tmpB34;void*
_tmpB35;struct Cyc_List_List*_tmpB36;struct Cyc_Absyn_AggrInfo _tmpB37;union Cyc_Absyn_AggrInfoU_union
_tmpB38;struct Cyc_List_List*_tmpB39;struct Cyc_List_List*_tmpB3A;_LL7A3: if((int)
_tmpB2F != 0)goto _LL7A5;_LL7A4: goto _LL7A6;_LL7A5: if(_tmpB2F <= (void*)4)goto _LL7A7;
if(*((int*)_tmpB2F)!= 5)goto _LL7A7;_LL7A6: goto _LL7A8;_LL7A7: if((int)_tmpB2F != 1)
goto _LL7A9;_LL7A8: goto _LL7AA;_LL7A9: if(_tmpB2F <= (void*)4)goto _LL7B9;if(*((int*)
_tmpB2F)!= 6)goto _LL7AB;_LL7AA: return 1;_LL7AB: if(*((int*)_tmpB2F)!= 4)goto _LL7AD;
_tmpB30=((struct Cyc_Absyn_PointerType_struct*)_tmpB2F)->f1;_tmpB31=_tmpB30.ptr_atts;
_tmpB32=_tmpB31.nullable;_tmpB33=_tmpB31.bounds;_LL7AC: {void*_tmpB3B=Cyc_Absyn_conref_def((
void*)((void*)0),_tmpB33);_LL7BC: if((int)_tmpB3B != 0)goto _LL7BE;_LL7BD: return 1;
_LL7BE:;_LL7BF: return((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
1,_tmpB32);_LL7BB:;}_LL7AD: if(*((int*)_tmpB2F)!= 7)goto _LL7AF;_tmpB34=((struct
Cyc_Absyn_ArrayType_struct*)_tmpB2F)->f1;_tmpB35=(void*)_tmpB34.elt_type;_LL7AE:
return Cyc_Tcutil_supports_default(_tmpB35);_LL7AF: if(*((int*)_tmpB2F)!= 9)goto
_LL7B1;_tmpB36=((struct Cyc_Absyn_TupleType_struct*)_tmpB2F)->f1;_LL7B0: for(0;
_tmpB36 != 0;_tmpB36=_tmpB36->tl){if(!Cyc_Tcutil_supports_default((*((struct
_tuple5*)_tmpB36->hd)).f2))return 0;}return 1;_LL7B1: if(*((int*)_tmpB2F)!= 10)goto
_LL7B3;_tmpB37=((struct Cyc_Absyn_AggrType_struct*)_tmpB2F)->f1;_tmpB38=_tmpB37.aggr_info;
_tmpB39=_tmpB37.targs;_LL7B2: {struct Cyc_Absyn_Aggrdecl*_tmpB3C=Cyc_Absyn_get_known_aggrdecl(
_tmpB38);if(_tmpB3C->impl == 0)return 0;if(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmpB3C->impl))->exist_vars != 0)return 0;return Cyc_Tcutil_fields_support_default(
_tmpB3C->tvs,_tmpB39,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpB3C->impl))->fields);}
_LL7B3: if(*((int*)_tmpB2F)!= 11)goto _LL7B5;_tmpB3A=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmpB2F)->f2;_LL7B4: return Cyc_Tcutil_fields_support_default(0,0,_tmpB3A);_LL7B5:
if(*((int*)_tmpB2F)!= 13)goto _LL7B7;_LL7B6: goto _LL7B8;_LL7B7: if(*((int*)_tmpB2F)
!= 12)goto _LL7B9;_LL7B8: return 1;_LL7B9:;_LL7BA: return 0;_LL7A2:;}static int Cyc_Tcutil_fields_support_default(
struct Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs);static int
Cyc_Tcutil_fields_support_default(struct Cyc_List_List*tvs,struct Cyc_List_List*ts,
struct Cyc_List_List*fs){struct _RegionHandle _tmpB3D=_new_region("rgn");struct
_RegionHandle*rgn=& _tmpB3D;_push_region(rgn);{struct Cyc_List_List*_tmpB3E=((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,tvs,ts);for(0;fs != 0;fs=fs->tl){
void*t=Cyc_Tcutil_rsubstitute(rgn,_tmpB3E,(void*)((struct Cyc_Absyn_Aggrfield*)fs->hd)->type);
if(!Cyc_Tcutil_supports_default(t)){int _tmpB3F=0;_npop_handler(0);return _tmpB3F;}}}{
int _tmpB40=1;_npop_handler(0);return _tmpB40;};_pop_region(rgn);}int Cyc_Tcutil_admits_zero(
void*t);int Cyc_Tcutil_admits_zero(void*t){void*_tmpB41=Cyc_Tcutil_compress(t);
struct Cyc_Absyn_PtrInfo _tmpB42;struct Cyc_Absyn_PtrAtts _tmpB43;struct Cyc_Absyn_Conref*
_tmpB44;struct Cyc_Absyn_Conref*_tmpB45;_LL7C1: if(_tmpB41 <= (void*)4)goto _LL7C3;
if(*((int*)_tmpB41)!= 5)goto _LL7C3;_LL7C2: goto _LL7C4;_LL7C3: if((int)_tmpB41 != 1)
goto _LL7C5;_LL7C4: goto _LL7C6;_LL7C5: if(_tmpB41 <= (void*)4)goto _LL7C9;if(*((int*)
_tmpB41)!= 6)goto _LL7C7;_LL7C6: return 1;_LL7C7: if(*((int*)_tmpB41)!= 4)goto _LL7C9;
_tmpB42=((struct Cyc_Absyn_PointerType_struct*)_tmpB41)->f1;_tmpB43=_tmpB42.ptr_atts;
_tmpB44=_tmpB43.nullable;_tmpB45=_tmpB43.bounds;_LL7C8: {union Cyc_Absyn_Constraint_union
_tmpB46=(Cyc_Absyn_compress_conref(_tmpB45))->v;void*_tmpB47;_LL7CC: if((_tmpB46.Eq_constr).tag
!= 0)goto _LL7CE;_tmpB47=(_tmpB46.Eq_constr).f1;if((int)_tmpB47 != 0)goto _LL7CE;
_LL7CD: return 0;_LL7CE: if((_tmpB46.Eq_constr).tag != 0)goto _LL7D0;_LL7CF: {union
Cyc_Absyn_Constraint_union _tmpB48=(((struct Cyc_Absyn_Conref*(*)(struct Cyc_Absyn_Conref*
x))Cyc_Absyn_compress_conref)(_tmpB44))->v;int _tmpB49;_LL7D3: if((_tmpB48.Eq_constr).tag
!= 0)goto _LL7D5;_tmpB49=(_tmpB48.Eq_constr).f1;_LL7D4: return _tmpB49;_LL7D5:;
_LL7D6: return 0;_LL7D2:;}_LL7D0:;_LL7D1: return 0;_LL7CB:;}_LL7C9:;_LL7CA: return 0;
_LL7C0:;}int Cyc_Tcutil_is_noreturn(void*t);int Cyc_Tcutil_is_noreturn(void*t){{
void*_tmpB4A=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmpB4B;void*_tmpB4C;
struct Cyc_Absyn_FnInfo _tmpB4D;struct Cyc_List_List*_tmpB4E;_LL7D8: if(_tmpB4A <= (
void*)4)goto _LL7DC;if(*((int*)_tmpB4A)!= 4)goto _LL7DA;_tmpB4B=((struct Cyc_Absyn_PointerType_struct*)
_tmpB4A)->f1;_tmpB4C=(void*)_tmpB4B.elt_typ;_LL7D9: return Cyc_Tcutil_is_noreturn(
_tmpB4C);_LL7DA: if(*((int*)_tmpB4A)!= 8)goto _LL7DC;_tmpB4D=((struct Cyc_Absyn_FnType_struct*)
_tmpB4A)->f1;_tmpB4E=_tmpB4D.attributes;_LL7DB: for(0;_tmpB4E != 0;_tmpB4E=_tmpB4E->tl){
void*_tmpB4F=(void*)_tmpB4E->hd;_LL7DF: if((int)_tmpB4F != 3)goto _LL7E1;_LL7E0:
return 1;_LL7E1:;_LL7E2: continue;_LL7DE:;}goto _LL7D7;_LL7DC:;_LL7DD: goto _LL7D7;
_LL7D7:;}return 0;}struct Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(void*t,
struct Cyc_List_List*atts);struct Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(
void*t,struct Cyc_List_List*atts){void*_tmpB50=Cyc_Tcutil_compress(t);struct Cyc_Absyn_FnInfo
_tmpB51;struct Cyc_List_List*_tmpB52;struct Cyc_List_List**_tmpB53;_LL7E4: if(
_tmpB50 <= (void*)4)goto _LL7E6;if(*((int*)_tmpB50)!= 8)goto _LL7E6;_tmpB51=((
struct Cyc_Absyn_FnType_struct*)_tmpB50)->f1;_tmpB52=_tmpB51.attributes;_tmpB53=(
struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_struct*)_tmpB50)->f1).attributes;
_LL7E5: {struct Cyc_List_List*_tmpB54=0;for(0;atts != 0;atts=atts->tl){if(Cyc_Absyn_fntype_att((
void*)atts->hd)){struct Cyc_List_List*_tmp10A3;*_tmpB53=((_tmp10A3=_cycalloc(
sizeof(*_tmp10A3)),((_tmp10A3->hd=(void*)((void*)atts->hd),((_tmp10A3->tl=*
_tmpB53,_tmp10A3))))));}else{struct Cyc_List_List*_tmp10A4;_tmpB54=((_tmp10A4=
_cycalloc(sizeof(*_tmp10A4)),((_tmp10A4->hd=(void*)((void*)atts->hd),((_tmp10A4->tl=
_tmpB54,_tmp10A4))))));}}return _tmpB54;}_LL7E6:;_LL7E7: {const char*_tmp10A7;void*
_tmp10A6;(_tmp10A6=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmp10A7="transfer_fn_type_atts",_tag_dyneither(_tmp10A7,
sizeof(char),22))),_tag_dyneither(_tmp10A6,sizeof(void*),0)));}_LL7E3:;}
