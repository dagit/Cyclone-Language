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
Cyc_Absyn_UnknownDatatypeInfo{struct _tuple2*name;int is_extensible;int is_flat;};
struct Cyc_Absyn_UnknownDatatype_struct{int tag;struct Cyc_Absyn_UnknownDatatypeInfo
f1;};struct Cyc_Absyn_KnownDatatype_struct{int tag;struct Cyc_Absyn_Datatypedecl**
f1;};union Cyc_Absyn_DatatypeInfoU_union{struct Cyc_Absyn_UnknownDatatype_struct
UnknownDatatype;struct Cyc_Absyn_KnownDatatype_struct KnownDatatype;};struct Cyc_Absyn_DatatypeInfo{
union Cyc_Absyn_DatatypeInfoU_union datatype_info;struct Cyc_List_List*targs;struct
Cyc_Core_Opt*rgn;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple2*
datatype_name;struct _tuple2*field_name;int is_extensible;};struct Cyc_Absyn_UnknownDatatypefield_struct{
int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo f1;};struct Cyc_Absyn_KnownDatatypefield_struct{
int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};union
Cyc_Absyn_DatatypeFieldInfoU_union{struct Cyc_Absyn_UnknownDatatypefield_struct
UnknownDatatypefield;struct Cyc_Absyn_KnownDatatypefield_struct KnownDatatypefield;
};struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU_union
field_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownAggr_struct{int tag;
void*f1;struct _tuple2*f2;};struct Cyc_Absyn_KnownAggr_struct{int tag;struct Cyc_Absyn_Aggrdecl**
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
int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_struct{int tag;
struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*
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
struct Cyc_Core_Opt*fields;int is_extensible;int is_flat;};struct Cyc_Absyn_Enumfield{
struct _tuple2*name;struct Cyc_Absyn_Exp*tag;struct Cyc_Position_Segment*loc;};
struct Cyc_Absyn_Enumdecl{void*sc;struct _tuple2*name;struct Cyc_Core_Opt*fields;};
struct Cyc_Absyn_Typedefdecl{struct _tuple2*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*
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
extern struct _tuple2*Cyc_Absyn_datatype_print_arg_qvar;extern struct _tuple2*Cyc_Absyn_datatype_scanf_arg_qvar;
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
struct Cyc_Absyn_Aggrdecl*);void Cyc_Tc_tcDatatypedecl(struct Cyc_Tcenv_Tenv*,
struct Cyc_Tcenv_Genv*,struct Cyc_Position_Segment*,struct Cyc_Absyn_Datatypedecl*);
void Cyc_Tc_tcEnumdecl(struct Cyc_Tcenv_Tenv*,struct Cyc_Tcenv_Genv*,struct Cyc_Position_Segment*,
struct Cyc_Absyn_Enumdecl*);extern int Cyc_Cyclone_tovc_r;char Cyc_Tcutil_Unify[10]="\000\000\000\000Unify\000";
void Cyc_Tcutil_unify_it(void*t1,void*t2);void*Cyc_Tcutil_t1_failure=(void*)0;int
Cyc_Tcutil_tq1_const=0;void*Cyc_Tcutil_t2_failure=(void*)0;int Cyc_Tcutil_tq2_const=
0;struct _dyneither_ptr Cyc_Tcutil_failure_reason=(struct _dyneither_ptr){(void*)0,(
void*)0,(void*)(0 + 0)};void Cyc_Tcutil_explain_failure();void Cyc_Tcutil_explain_failure(){
if(Cyc_Position_num_errors >= Cyc_Position_max_errors)return;Cyc_fflush((struct
Cyc___cycFILE*)Cyc_stderr);{const char*_tmpB5E;const char*_tmpB5D;const char*
_tmpB5C;void*_tmpB5B[2];struct Cyc_String_pa_struct _tmpB5A;struct Cyc_String_pa_struct
_tmpB59;struct _dyneither_ptr s1=(struct _dyneither_ptr)((_tmpB59.tag=0,((_tmpB59.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(Cyc_Tcutil_t1_failure)),((
_tmpB5A.tag=0,((_tmpB5A.f1=(struct _dyneither_ptr)(Cyc_Tcutil_tq1_const?(_tmpB5D="const ",
_tag_dyneither(_tmpB5D,sizeof(char),7)):((_tmpB5E="",_tag_dyneither(_tmpB5E,
sizeof(char),1)))),((_tmpB5B[0]=& _tmpB5A,((_tmpB5B[1]=& _tmpB59,Cyc_aprintf(((
_tmpB5C="%s%s",_tag_dyneither(_tmpB5C,sizeof(char),5))),_tag_dyneither(_tmpB5B,
sizeof(void*),2))))))))))))));const char*_tmpB67;const char*_tmpB66;const char*
_tmpB65;void*_tmpB64[2];struct Cyc_String_pa_struct _tmpB63;struct Cyc_String_pa_struct
_tmpB62;struct _dyneither_ptr s2=(struct _dyneither_ptr)((_tmpB62.tag=0,((_tmpB62.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(Cyc_Tcutil_t2_failure)),((
_tmpB63.tag=0,((_tmpB63.f1=(struct _dyneither_ptr)(Cyc_Tcutil_tq2_const?(_tmpB66="const ",
_tag_dyneither(_tmpB66,sizeof(char),7)):((_tmpB67="",_tag_dyneither(_tmpB67,
sizeof(char),1)))),((_tmpB64[0]=& _tmpB63,((_tmpB64[1]=& _tmpB62,Cyc_aprintf(((
_tmpB65="%s%s",_tag_dyneither(_tmpB65,sizeof(char),5))),_tag_dyneither(_tmpB64,
sizeof(void*),2))))))))))))));int pos=2;{const char*_tmpB6B;void*_tmpB6A[1];struct
Cyc_String_pa_struct _tmpB69;(_tmpB69.tag=0,((_tmpB69.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)s1),((_tmpB6A[0]=& _tmpB69,Cyc_fprintf(Cyc_stderr,((_tmpB6B="  %s",
_tag_dyneither(_tmpB6B,sizeof(char),5))),_tag_dyneither(_tmpB6A,sizeof(void*),1)))))));}
pos +=_get_dyneither_size(s1,sizeof(char));if(pos + 5 >= 80){{const char*_tmpB6E;
void*_tmpB6D;(_tmpB6D=0,Cyc_fprintf(Cyc_stderr,((_tmpB6E="\n\t",_tag_dyneither(
_tmpB6E,sizeof(char),3))),_tag_dyneither(_tmpB6D,sizeof(void*),0)));}pos=8;}
else{{const char*_tmpB71;void*_tmpB70;(_tmpB70=0,Cyc_fprintf(Cyc_stderr,((_tmpB71=" ",
_tag_dyneither(_tmpB71,sizeof(char),2))),_tag_dyneither(_tmpB70,sizeof(void*),0)));}
++ pos;}{const char*_tmpB74;void*_tmpB73;(_tmpB73=0,Cyc_fprintf(Cyc_stderr,((
_tmpB74="and ",_tag_dyneither(_tmpB74,sizeof(char),5))),_tag_dyneither(_tmpB73,
sizeof(void*),0)));}pos +=4;if(pos + _get_dyneither_size(s2,sizeof(char))>= 80){{
const char*_tmpB77;void*_tmpB76;(_tmpB76=0,Cyc_fprintf(Cyc_stderr,((_tmpB77="\n\t",
_tag_dyneither(_tmpB77,sizeof(char),3))),_tag_dyneither(_tmpB76,sizeof(void*),0)));}
pos=8;}{const char*_tmpB7B;void*_tmpB7A[1];struct Cyc_String_pa_struct _tmpB79;(
_tmpB79.tag=0,((_tmpB79.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s2),((
_tmpB7A[0]=& _tmpB79,Cyc_fprintf(Cyc_stderr,((_tmpB7B="%s ",_tag_dyneither(
_tmpB7B,sizeof(char),4))),_tag_dyneither(_tmpB7A,sizeof(void*),1)))))));}pos +=
_get_dyneither_size(s2,sizeof(char))+ 1;if(pos + 17 >= 80){{const char*_tmpB7E;void*
_tmpB7D;(_tmpB7D=0,Cyc_fprintf(Cyc_stderr,((_tmpB7E="\n\t",_tag_dyneither(
_tmpB7E,sizeof(char),3))),_tag_dyneither(_tmpB7D,sizeof(void*),0)));}pos=8;}{
const char*_tmpB81;void*_tmpB80;(_tmpB80=0,Cyc_fprintf(Cyc_stderr,((_tmpB81="are not compatible. ",
_tag_dyneither(_tmpB81,sizeof(char),21))),_tag_dyneither(_tmpB80,sizeof(void*),0)));}
pos +=17;if(Cyc_Tcutil_failure_reason.curr != (_tag_dyneither(0,0,0)).curr){if(pos
+ Cyc_strlen((struct _dyneither_ptr)Cyc_Tcutil_failure_reason)>= 80){const char*
_tmpB84;void*_tmpB83;(_tmpB83=0,Cyc_fprintf(Cyc_stderr,((_tmpB84="\n\t",
_tag_dyneither(_tmpB84,sizeof(char),3))),_tag_dyneither(_tmpB83,sizeof(void*),0)));}{
const char*_tmpB88;void*_tmpB87[1];struct Cyc_String_pa_struct _tmpB86;(_tmpB86.tag=
0,((_tmpB86.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_failure_reason),((
_tmpB87[0]=& _tmpB86,Cyc_fprintf(Cyc_stderr,((_tmpB88="%s",_tag_dyneither(_tmpB88,
sizeof(char),3))),_tag_dyneither(_tmpB87,sizeof(void*),1)))))));}}{const char*
_tmpB8B;void*_tmpB8A;(_tmpB8A=0,Cyc_fprintf(Cyc_stderr,((_tmpB8B="\n",
_tag_dyneither(_tmpB8B,sizeof(char),2))),_tag_dyneither(_tmpB8A,sizeof(void*),0)));}
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);}}void Cyc_Tcutil_terr(struct Cyc_Position_Segment*
loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);void Cyc_Tcutil_terr(struct
Cyc_Position_Segment*loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){Cyc_Position_post_error(
Cyc_Position_mk_err_elab(loc,(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,
fmt,ap)));}void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){struct
_dyneither_ptr msg=(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap);{
const char*_tmpB8F;void*_tmpB8E[1];struct Cyc_String_pa_struct _tmpB8D;(_tmpB8D.tag=
0,((_tmpB8D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg),((_tmpB8E[0]=&
_tmpB8D,Cyc_fprintf(Cyc_stderr,((_tmpB8F="Compiler Error (Tcutil::impos): %s\n",
_tag_dyneither(_tmpB8F,sizeof(char),36))),_tag_dyneither(_tmpB8E,sizeof(void*),1)))))));}
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);{struct Cyc_Core_Impossible_struct
_tmpB92;struct Cyc_Core_Impossible_struct*_tmpB91;(int)_throw((void*)((_tmpB91=
_cycalloc(sizeof(*_tmpB91)),((_tmpB91[0]=((_tmpB92.tag=Cyc_Core_Impossible,((
_tmpB92.f1=msg,_tmpB92)))),_tmpB91)))));}}static struct _dyneither_ptr Cyc_Tcutil_tvar2string(
struct Cyc_Absyn_Tvar*tv);static struct _dyneither_ptr Cyc_Tcutil_tvar2string(struct
Cyc_Absyn_Tvar*tv){return*tv->name;}void Cyc_Tcutil_print_tvars(struct Cyc_List_List*
tvs);void Cyc_Tcutil_print_tvars(struct Cyc_List_List*tvs){for(0;tvs != 0;tvs=tvs->tl){
const char*_tmpB97;void*_tmpB96[2];struct Cyc_String_pa_struct _tmpB95;struct Cyc_String_pa_struct
_tmpB94;(_tmpB94.tag=0,((_tmpB94.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kindbound2string((void*)((struct Cyc_Absyn_Tvar*)tvs->hd)->kind)),((
_tmpB95.tag=0,((_tmpB95.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_tvar2string((
struct Cyc_Absyn_Tvar*)tvs->hd)),((_tmpB96[0]=& _tmpB95,((_tmpB96[1]=& _tmpB94,Cyc_fprintf(
Cyc_stderr,((_tmpB97="%s::%s ",_tag_dyneither(_tmpB97,sizeof(char),8))),
_tag_dyneither(_tmpB96,sizeof(void*),2)))))))))))));}{const char*_tmpB9A;void*
_tmpB99;(_tmpB99=0,Cyc_fprintf(Cyc_stderr,((_tmpB9A="\n",_tag_dyneither(_tmpB9A,
sizeof(char),2))),_tag_dyneither(_tmpB99,sizeof(void*),0)));}Cyc_fflush((struct
Cyc___cycFILE*)Cyc_stderr);}static struct Cyc_List_List*Cyc_Tcutil_warning_segs=0;
static struct Cyc_List_List*Cyc_Tcutil_warning_msgs=0;void Cyc_Tcutil_warn(struct
Cyc_Position_Segment*sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);void Cyc_Tcutil_warn(
struct Cyc_Position_Segment*sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
struct _dyneither_ptr msg=(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,
fmt,ap);{struct Cyc_List_List*_tmpB9B;Cyc_Tcutil_warning_segs=((_tmpB9B=_cycalloc(
sizeof(*_tmpB9B)),((_tmpB9B->hd=sg,((_tmpB9B->tl=Cyc_Tcutil_warning_segs,_tmpB9B))))));}{
struct _dyneither_ptr*_tmpB9E;struct Cyc_List_List*_tmpB9D;Cyc_Tcutil_warning_msgs=((
_tmpB9D=_cycalloc(sizeof(*_tmpB9D)),((_tmpB9D->hd=((_tmpB9E=_cycalloc(sizeof(*
_tmpB9E)),((_tmpB9E[0]=msg,_tmpB9E)))),((_tmpB9D->tl=Cyc_Tcutil_warning_msgs,
_tmpB9D))))));}}void Cyc_Tcutil_flush_warnings();void Cyc_Tcutil_flush_warnings(){
if(Cyc_Tcutil_warning_segs == 0)return;{const char*_tmpBA1;void*_tmpBA0;(_tmpBA0=0,
Cyc_fprintf(Cyc_stderr,((_tmpBA1="***Warnings***\n",_tag_dyneither(_tmpBA1,
sizeof(char),16))),_tag_dyneither(_tmpBA0,sizeof(void*),0)));}{struct Cyc_List_List*
_tmp35=Cyc_Position_strings_of_segments(Cyc_Tcutil_warning_segs);Cyc_Tcutil_warning_segs=
0;Cyc_Tcutil_warning_msgs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_Tcutil_warning_msgs);while(Cyc_Tcutil_warning_msgs != 0){{const char*_tmpBA6;
void*_tmpBA5[2];struct Cyc_String_pa_struct _tmpBA4;struct Cyc_String_pa_struct
_tmpBA3;(_tmpBA3.tag=0,((_tmpBA3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((
struct _dyneither_ptr*)((struct Cyc_List_List*)_check_null(Cyc_Tcutil_warning_msgs))->hd)),((
_tmpBA4.tag=0,((_tmpBA4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((
struct _dyneither_ptr*)((struct Cyc_List_List*)_check_null(_tmp35))->hd)),((
_tmpBA5[0]=& _tmpBA4,((_tmpBA5[1]=& _tmpBA3,Cyc_fprintf(Cyc_stderr,((_tmpBA6="%s: %s\n",
_tag_dyneither(_tmpBA6,sizeof(char),8))),_tag_dyneither(_tmpBA5,sizeof(void*),2)))))))))))));}
_tmp35=_tmp35->tl;Cyc_Tcutil_warning_msgs=((struct Cyc_List_List*)_check_null(Cyc_Tcutil_warning_msgs))->tl;}{
const char*_tmpBA9;void*_tmpBA8;(_tmpBA8=0,Cyc_fprintf(Cyc_stderr,((_tmpBA9="**************\n",
_tag_dyneither(_tmpBA9,sizeof(char),16))),_tag_dyneither(_tmpBA8,sizeof(void*),0)));}
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
_tmpBAA;*_tmp40=((_tmpBAA=_cycalloc(sizeof(*_tmpBAA)),((_tmpBAA[0]=t2,_tmpBAA))));}
return t2;}_LL7: if(*((int*)_tmp3C)!= 0)goto _LL9;_tmp41=((struct Cyc_Absyn_Evar_struct*)
_tmp3C)->f2;_tmp42=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)_tmp3C)->f2;
_LL8: {void*t2=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(*
_tmp42))->v);if(t2 != (void*)((struct Cyc_Core_Opt*)_check_null(*_tmp42))->v){
struct Cyc_Core_Opt*_tmpBAB;*_tmp42=((_tmpBAB=_cycalloc(sizeof(*_tmpBAB)),((
_tmpBAB->v=(void*)t2,_tmpBAB))));}return t2;}_LL9:;_LLA: return t;_LL0:;}void*Cyc_Tcutil_copy_type(
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
_tmpBAE;struct Cyc_Absyn_Unknown_kb_struct*_tmpBAD;return(void*)((_tmpBAD=
_cycalloc(sizeof(*_tmpBAD)),((_tmpBAD[0]=((_tmpBAE.tag=1,((_tmpBAE.f1=0,_tmpBAE)))),
_tmpBAD))));}_LL15: if(*((int*)_tmp48)!= 2)goto _LL17;_tmp49=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp48)->f2;_LL16: {struct Cyc_Absyn_Less_kb_struct _tmpBB1;struct Cyc_Absyn_Less_kb_struct*
_tmpBB0;return(void*)((_tmpBB0=_cycalloc(sizeof(*_tmpBB0)),((_tmpBB0[0]=((
_tmpBB1.tag=2,((_tmpBB1.f1=0,((_tmpBB1.f2=(void*)_tmp49,_tmpBB1)))))),_tmpBB0))));}
_LL17:;_LL18: return kb;_LL12:;}static struct Cyc_Absyn_Tvar*Cyc_Tcutil_copy_tvar(
struct Cyc_Absyn_Tvar*tv);static struct Cyc_Absyn_Tvar*Cyc_Tcutil_copy_tvar(struct
Cyc_Absyn_Tvar*tv){struct Cyc_Absyn_Tvar*_tmpBB2;return(_tmpBB2=_cycalloc(sizeof(*
_tmpBB2)),((_tmpBB2->name=tv->name,((_tmpBB2->identity=- 1,((_tmpBB2->kind=(void*)
Cyc_Tcutil_copy_kindbound((void*)tv->kind),_tmpBB2)))))));}static struct _tuple3*
Cyc_Tcutil_copy_arg(struct _tuple3*arg);static struct _tuple3*Cyc_Tcutil_copy_arg(
struct _tuple3*arg){struct _tuple3 _tmp50;struct Cyc_Core_Opt*_tmp51;struct Cyc_Absyn_Tqual
_tmp52;void*_tmp53;struct _tuple3*_tmp4F=arg;_tmp50=*_tmp4F;_tmp51=_tmp50.f1;
_tmp52=_tmp50.f2;_tmp53=_tmp50.f3;{struct _tuple3*_tmpBB3;return(_tmpBB3=
_cycalloc(sizeof(*_tmpBB3)),((_tmpBB3->f1=_tmp51,((_tmpBB3->f2=_tmp52,((_tmpBB3->f3=
Cyc_Tcutil_copy_type(_tmp53),_tmpBB3)))))));}}static struct _tuple5*Cyc_Tcutil_copy_tqt(
struct _tuple5*arg);static struct _tuple5*Cyc_Tcutil_copy_tqt(struct _tuple5*arg){
struct _tuple5 _tmp56;struct Cyc_Absyn_Tqual _tmp57;void*_tmp58;struct _tuple5*_tmp55=
arg;_tmp56=*_tmp55;_tmp57=_tmp56.f1;_tmp58=_tmp56.f2;{struct _tuple5*_tmpBB4;
return(_tmpBB4=_cycalloc(sizeof(*_tmpBB4)),((_tmpBB4->f1=_tmp57,((_tmpBB4->f2=
Cyc_Tcutil_copy_type(_tmp58),_tmpBB4)))));}}static struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_copy_field(
struct Cyc_Absyn_Aggrfield*f);static struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_copy_field(
struct Cyc_Absyn_Aggrfield*f){struct Cyc_Absyn_Aggrfield*_tmpBB5;return(_tmpBB5=
_cycalloc(sizeof(*_tmpBB5)),((_tmpBB5->name=f->name,((_tmpBB5->tq=f->tq,((
_tmpBB5->type=(void*)Cyc_Tcutil_copy_type((void*)f->type),((_tmpBB5->width=f->width,((
_tmpBB5->attributes=f->attributes,_tmpBB5)))))))))));}static struct _tuple0*Cyc_Tcutil_copy_rgncmp(
struct _tuple0*x);static struct _tuple0*Cyc_Tcutil_copy_rgncmp(struct _tuple0*x){
struct _tuple0 _tmp5C;void*_tmp5D;void*_tmp5E;struct _tuple0*_tmp5B=x;_tmp5C=*
_tmp5B;_tmp5D=_tmp5C.f1;_tmp5E=_tmp5C.f2;{struct _tuple0*_tmpBB6;return(_tmpBB6=
_cycalloc(sizeof(*_tmpBB6)),((_tmpBB6->f1=Cyc_Tcutil_copy_type(_tmp5D),((_tmpBB6->f2=
Cyc_Tcutil_copy_type(_tmp5E),_tmpBB6)))));}}static struct Cyc_Absyn_Enumfield*Cyc_Tcutil_copy_enumfield(
struct Cyc_Absyn_Enumfield*f);static struct Cyc_Absyn_Enumfield*Cyc_Tcutil_copy_enumfield(
struct Cyc_Absyn_Enumfield*f){struct Cyc_Absyn_Enumfield*_tmpBB7;return(_tmpBB7=
_cycalloc(sizeof(*_tmpBB7)),((_tmpBB7->name=f->name,((_tmpBB7->tag=f->tag,((
_tmpBB7->loc=f->loc,_tmpBB7)))))));}void*Cyc_Tcutil_copy_type(void*t);void*Cyc_Tcutil_copy_type(
void*t){void*_tmp61=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*_tmp62;struct Cyc_Absyn_DatatypeInfo
_tmp63;union Cyc_Absyn_DatatypeInfoU_union _tmp64;struct Cyc_List_List*_tmp65;
struct Cyc_Core_Opt*_tmp66;struct Cyc_Absyn_DatatypeFieldInfo _tmp67;union Cyc_Absyn_DatatypeFieldInfoU_union
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
_tmp61)->f1;_LL1F: {struct Cyc_Absyn_VarType_struct _tmpBBA;struct Cyc_Absyn_VarType_struct*
_tmpBB9;return(void*)((_tmpBB9=_cycalloc(sizeof(*_tmpBB9)),((_tmpBB9[0]=((
_tmpBBA.tag=1,((_tmpBBA.f1=Cyc_Tcutil_copy_tvar(_tmp62),_tmpBBA)))),_tmpBB9))));}
_LL20: if(*((int*)_tmp61)!= 2)goto _LL22;_tmp63=((struct Cyc_Absyn_DatatypeType_struct*)
_tmp61)->f1;_tmp64=_tmp63.datatype_info;_tmp65=_tmp63.targs;_tmp66=_tmp63.rgn;
_LL21: {struct Cyc_Core_Opt*_tmpBBB;struct Cyc_Core_Opt*_tmp9E=(unsigned int)
_tmp66?(_tmpBBB=_cycalloc(sizeof(*_tmpBBB)),((_tmpBBB->v=(void*)Cyc_Tcutil_copy_type((
void*)_tmp66->v),_tmpBBB))): 0;struct Cyc_Absyn_DatatypeType_struct _tmpBC1;struct
Cyc_Absyn_DatatypeInfo _tmpBC0;struct Cyc_Absyn_DatatypeType_struct*_tmpBBF;return(
void*)((_tmpBBF=_cycalloc(sizeof(*_tmpBBF)),((_tmpBBF[0]=((_tmpBC1.tag=2,((
_tmpBC1.f1=((_tmpBC0.datatype_info=_tmp64,((_tmpBC0.targs=Cyc_Tcutil_copy_types(
_tmp65),((_tmpBC0.rgn=_tmp9E,_tmpBC0)))))),_tmpBC1)))),_tmpBBF))));}_LL22: if(*((
int*)_tmp61)!= 3)goto _LL24;_tmp67=((struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp61)->f1;_tmp68=_tmp67.field_info;_tmp69=_tmp67.targs;_LL23: {struct Cyc_Absyn_DatatypeFieldType_struct
_tmpBC7;struct Cyc_Absyn_DatatypeFieldInfo _tmpBC6;struct Cyc_Absyn_DatatypeFieldType_struct*
_tmpBC5;return(void*)((_tmpBC5=_cycalloc(sizeof(*_tmpBC5)),((_tmpBC5[0]=((
_tmpBC7.tag=3,((_tmpBC7.f1=((_tmpBC6.field_info=_tmp68,((_tmpBC6.targs=Cyc_Tcutil_copy_types(
_tmp69),_tmpBC6)))),_tmpBC7)))),_tmpBC5))));}_LL24: if(*((int*)_tmp61)!= 4)goto
_LL26;_tmp6A=((struct Cyc_Absyn_PointerType_struct*)_tmp61)->f1;_tmp6B=(void*)
_tmp6A.elt_typ;_tmp6C=_tmp6A.elt_tq;_tmp6D=_tmp6A.ptr_atts;_tmp6E=(void*)_tmp6D.rgn;
_tmp6F=_tmp6D.nullable;_tmp70=_tmp6D.bounds;_tmp71=_tmp6D.zero_term;_tmp72=
_tmp6D.ptrloc;_LL25: {void*_tmpA6=Cyc_Tcutil_copy_type(_tmp6B);void*_tmpA7=Cyc_Tcutil_copy_type(
_tmp6E);struct Cyc_Absyn_Conref*_tmpA8=((struct Cyc_Absyn_Conref*(*)(struct Cyc_Absyn_Conref*
c))Cyc_Tcutil_copy_conref)(_tmp6F);struct Cyc_Absyn_Tqual _tmpA9=_tmp6C;struct Cyc_Absyn_Conref*
_tmpAA=Cyc_Tcutil_copy_conref(_tmp70);struct Cyc_Absyn_Conref*_tmpAB=((struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref*c))Cyc_Tcutil_copy_conref)(_tmp71);struct Cyc_Absyn_PointerType_struct
_tmpBD1;struct Cyc_Absyn_PtrAtts _tmpBD0;struct Cyc_Absyn_PtrInfo _tmpBCF;struct Cyc_Absyn_PointerType_struct*
_tmpBCE;return(void*)((_tmpBCE=_cycalloc(sizeof(*_tmpBCE)),((_tmpBCE[0]=((
_tmpBD1.tag=4,((_tmpBD1.f1=((_tmpBCF.elt_typ=(void*)_tmpA6,((_tmpBCF.elt_tq=
_tmpA9,((_tmpBCF.ptr_atts=((_tmpBD0.rgn=(void*)_tmpA7,((_tmpBD0.nullable=_tmpA8,((
_tmpBD0.bounds=_tmpAA,((_tmpBD0.zero_term=_tmpAB,((_tmpBD0.ptrloc=_tmp72,_tmpBD0)))))))))),
_tmpBCF)))))),_tmpBD1)))),_tmpBCE))));}_LL26: if(*((int*)_tmp61)!= 5)goto _LL28;
_LL27: goto _LL29;_LL28: if((int)_tmp61 != 1)goto _LL2A;_LL29: goto _LL2B;_LL2A: if(
_tmp61 <= (void*)4)goto _LL46;if(*((int*)_tmp61)!= 6)goto _LL2C;_LL2B: return t;_LL2C:
if(*((int*)_tmp61)!= 7)goto _LL2E;_tmp73=((struct Cyc_Absyn_ArrayType_struct*)
_tmp61)->f1;_tmp74=(void*)_tmp73.elt_type;_tmp75=_tmp73.tq;_tmp76=_tmp73.num_elts;
_tmp77=_tmp73.zero_term;_tmp78=_tmp73.zt_loc;_LL2D: {struct Cyc_Absyn_ArrayType_struct
_tmpBD7;struct Cyc_Absyn_ArrayInfo _tmpBD6;struct Cyc_Absyn_ArrayType_struct*
_tmpBD5;return(void*)((_tmpBD5=_cycalloc(sizeof(*_tmpBD5)),((_tmpBD5[0]=((
_tmpBD7.tag=7,((_tmpBD7.f1=((_tmpBD6.elt_type=(void*)Cyc_Tcutil_copy_type(_tmp74),((
_tmpBD6.tq=_tmp75,((_tmpBD6.num_elts=_tmp76,((_tmpBD6.zero_term=((struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref*c))Cyc_Tcutil_copy_conref)(_tmp77),((_tmpBD6.zt_loc=
_tmp78,_tmpBD6)))))))))),_tmpBD7)))),_tmpBD5))));}_LL2E: if(*((int*)_tmp61)!= 8)
goto _LL30;_tmp79=((struct Cyc_Absyn_FnType_struct*)_tmp61)->f1;_tmp7A=_tmp79.tvars;
_tmp7B=_tmp79.effect;_tmp7C=(void*)_tmp79.ret_typ;_tmp7D=_tmp79.args;_tmp7E=
_tmp79.c_varargs;_tmp7F=_tmp79.cyc_varargs;_tmp80=_tmp79.rgn_po;_tmp81=_tmp79.attributes;
_LL2F: {struct Cyc_List_List*_tmpB3=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*
f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_tvar,
_tmp7A);struct Cyc_Core_Opt*_tmpBD8;struct Cyc_Core_Opt*_tmpB4=_tmp7B == 0?0:((
_tmpBD8=_cycalloc(sizeof(*_tmpBD8)),((_tmpBD8->v=(void*)Cyc_Tcutil_copy_type((
void*)_tmp7B->v),_tmpBD8))));void*_tmpB5=Cyc_Tcutil_copy_type(_tmp7C);struct Cyc_List_List*
_tmpB6=((struct Cyc_List_List*(*)(struct _tuple3*(*f)(struct _tuple3*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Tcutil_copy_arg,_tmp7D);int _tmpB7=_tmp7E;struct Cyc_Absyn_VarargInfo*
cyc_varargs2=0;if(_tmp7F != 0){struct Cyc_Absyn_VarargInfo*cv=(struct Cyc_Absyn_VarargInfo*)
_tmp7F;struct Cyc_Absyn_VarargInfo*_tmpBD9;cyc_varargs2=((_tmpBD9=_cycalloc(
sizeof(*_tmpBD9)),((_tmpBD9->name=cv->name,((_tmpBD9->tq=cv->tq,((_tmpBD9->type=(
void*)Cyc_Tcutil_copy_type((void*)cv->type),((_tmpBD9->inject=cv->inject,_tmpBD9))))))))));}{
struct Cyc_List_List*_tmpB9=((struct Cyc_List_List*(*)(struct _tuple0*(*f)(struct
_tuple0*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_rgncmp,_tmp80);
struct Cyc_List_List*_tmpBA=_tmp81;struct Cyc_Absyn_FnType_struct _tmpBDF;struct Cyc_Absyn_FnInfo
_tmpBDE;struct Cyc_Absyn_FnType_struct*_tmpBDD;return(void*)((_tmpBDD=_cycalloc(
sizeof(*_tmpBDD)),((_tmpBDD[0]=((_tmpBDF.tag=8,((_tmpBDF.f1=((_tmpBDE.tvars=
_tmpB3,((_tmpBDE.effect=_tmpB4,((_tmpBDE.ret_typ=(void*)_tmpB5,((_tmpBDE.args=
_tmpB6,((_tmpBDE.c_varargs=_tmpB7,((_tmpBDE.cyc_varargs=cyc_varargs2,((_tmpBDE.rgn_po=
_tmpB9,((_tmpBDE.attributes=_tmpBA,_tmpBDE)))))))))))))))),_tmpBDF)))),_tmpBDD))));}}
_LL30: if(*((int*)_tmp61)!= 9)goto _LL32;_tmp82=((struct Cyc_Absyn_TupleType_struct*)
_tmp61)->f1;_LL31: {struct Cyc_Absyn_TupleType_struct _tmpBE2;struct Cyc_Absyn_TupleType_struct*
_tmpBE1;return(void*)((_tmpBE1=_cycalloc(sizeof(*_tmpBE1)),((_tmpBE1[0]=((
_tmpBE2.tag=9,((_tmpBE2.f1=((struct Cyc_List_List*(*)(struct _tuple5*(*f)(struct
_tuple5*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_tqt,_tmp82),
_tmpBE2)))),_tmpBE1))));}_LL32: if(*((int*)_tmp61)!= 10)goto _LL34;_tmp83=((struct
Cyc_Absyn_AggrType_struct*)_tmp61)->f1;_tmp84=_tmp83.aggr_info;if((((((struct Cyc_Absyn_AggrType_struct*)
_tmp61)->f1).aggr_info).UnknownAggr).tag != 0)goto _LL34;_tmp85=(_tmp84.UnknownAggr).f1;
_tmp86=(_tmp84.UnknownAggr).f2;_tmp87=_tmp83.targs;_LL33: {struct Cyc_Absyn_AggrType_struct
_tmpBEC;union Cyc_Absyn_AggrInfoU_union _tmpBEB;struct Cyc_Absyn_AggrInfo _tmpBEA;
struct Cyc_Absyn_AggrType_struct*_tmpBE9;return(void*)((_tmpBE9=_cycalloc(sizeof(*
_tmpBE9)),((_tmpBE9[0]=((_tmpBEC.tag=10,((_tmpBEC.f1=((_tmpBEA.aggr_info=(union
Cyc_Absyn_AggrInfoU_union)(((_tmpBEB.UnknownAggr).tag=0,(((_tmpBEB.UnknownAggr).f1=(
void*)_tmp85,(((_tmpBEB.UnknownAggr).f2=_tmp86,_tmpBEB)))))),((_tmpBEA.targs=Cyc_Tcutil_copy_types(
_tmp87),_tmpBEA)))),_tmpBEC)))),_tmpBE9))));}_LL34: if(*((int*)_tmp61)!= 10)goto
_LL36;_tmp88=((struct Cyc_Absyn_AggrType_struct*)_tmp61)->f1;_tmp89=_tmp88.aggr_info;
if((((((struct Cyc_Absyn_AggrType_struct*)_tmp61)->f1).aggr_info).KnownAggr).tag
!= 1)goto _LL36;_tmp8A=(_tmp89.KnownAggr).f1;_tmp8B=_tmp88.targs;_LL35: {struct
Cyc_Absyn_AggrType_struct _tmpBF6;union Cyc_Absyn_AggrInfoU_union _tmpBF5;struct Cyc_Absyn_AggrInfo
_tmpBF4;struct Cyc_Absyn_AggrType_struct*_tmpBF3;return(void*)((_tmpBF3=_cycalloc(
sizeof(*_tmpBF3)),((_tmpBF3[0]=((_tmpBF6.tag=10,((_tmpBF6.f1=((_tmpBF4.aggr_info=(
union Cyc_Absyn_AggrInfoU_union)(((_tmpBF5.KnownAggr).tag=1,(((_tmpBF5.KnownAggr).f1=
_tmp8A,_tmpBF5)))),((_tmpBF4.targs=Cyc_Tcutil_copy_types(_tmp8B),_tmpBF4)))),
_tmpBF6)))),_tmpBF3))));}_LL36: if(*((int*)_tmp61)!= 11)goto _LL38;_tmp8C=(void*)((
struct Cyc_Absyn_AnonAggrType_struct*)_tmp61)->f1;_tmp8D=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp61)->f2;_LL37: {struct Cyc_Absyn_AnonAggrType_struct _tmpBF9;struct Cyc_Absyn_AnonAggrType_struct*
_tmpBF8;return(void*)((_tmpBF8=_cycalloc(sizeof(*_tmpBF8)),((_tmpBF8[0]=((
_tmpBF9.tag=11,((_tmpBF9.f1=(void*)_tmp8C,((_tmpBF9.f2=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Tcutil_copy_field,_tmp8D),_tmpBF9)))))),_tmpBF8))));}_LL38: if(*((
int*)_tmp61)!= 12)goto _LL3A;_tmp8E=((struct Cyc_Absyn_EnumType_struct*)_tmp61)->f1;
_tmp8F=((struct Cyc_Absyn_EnumType_struct*)_tmp61)->f2;_LL39: {struct Cyc_Absyn_EnumType_struct
_tmpBFC;struct Cyc_Absyn_EnumType_struct*_tmpBFB;return(void*)((_tmpBFB=_cycalloc(
sizeof(*_tmpBFB)),((_tmpBFB[0]=((_tmpBFC.tag=12,((_tmpBFC.f1=_tmp8E,((_tmpBFC.f2=
_tmp8F,_tmpBFC)))))),_tmpBFB))));}_LL3A: if(*((int*)_tmp61)!= 13)goto _LL3C;_tmp90=((
struct Cyc_Absyn_AnonEnumType_struct*)_tmp61)->f1;_LL3B: {struct Cyc_Absyn_AnonEnumType_struct
_tmpBFF;struct Cyc_Absyn_AnonEnumType_struct*_tmpBFE;return(void*)((_tmpBFE=
_cycalloc(sizeof(*_tmpBFE)),((_tmpBFE[0]=((_tmpBFF.tag=13,((_tmpBFF.f1=((struct
Cyc_List_List*(*)(struct Cyc_Absyn_Enumfield*(*f)(struct Cyc_Absyn_Enumfield*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_enumfield,_tmp90),_tmpBFF)))),
_tmpBFE))));}_LL3C: if(*((int*)_tmp61)!= 18)goto _LL3E;_tmp91=(void*)((struct Cyc_Absyn_TagType_struct*)
_tmp61)->f1;_LL3D: {struct Cyc_Absyn_TagType_struct _tmpC02;struct Cyc_Absyn_TagType_struct*
_tmpC01;return(void*)((_tmpC01=_cycalloc(sizeof(*_tmpC01)),((_tmpC01[0]=((
_tmpC02.tag=18,((_tmpC02.f1=(void*)Cyc_Tcutil_copy_type(_tmp91),_tmpC02)))),
_tmpC01))));}_LL3E: if(*((int*)_tmp61)!= 17)goto _LL40;_tmp92=((struct Cyc_Absyn_ValueofType_struct*)
_tmp61)->f1;_LL3F: {struct Cyc_Absyn_ValueofType_struct _tmpC05;struct Cyc_Absyn_ValueofType_struct*
_tmpC04;return(void*)((_tmpC04=_cycalloc(sizeof(*_tmpC04)),((_tmpC04[0]=((
_tmpC05.tag=17,((_tmpC05.f1=_tmp92,_tmpC05)))),_tmpC04))));}_LL40: if(*((int*)
_tmp61)!= 14)goto _LL42;_tmp93=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp61)->f1;_LL41: {struct Cyc_Absyn_RgnHandleType_struct _tmpC08;struct Cyc_Absyn_RgnHandleType_struct*
_tmpC07;return(void*)((_tmpC07=_cycalloc(sizeof(*_tmpC07)),((_tmpC07[0]=((
_tmpC08.tag=14,((_tmpC08.f1=(void*)Cyc_Tcutil_copy_type(_tmp93),_tmpC08)))),
_tmpC07))));}_LL42: if(*((int*)_tmp61)!= 15)goto _LL44;_tmp94=(void*)((struct Cyc_Absyn_DynRgnType_struct*)
_tmp61)->f1;_tmp95=(void*)((struct Cyc_Absyn_DynRgnType_struct*)_tmp61)->f2;_LL43: {
struct Cyc_Absyn_DynRgnType_struct _tmpC0B;struct Cyc_Absyn_DynRgnType_struct*
_tmpC0A;return(void*)((_tmpC0A=_cycalloc(sizeof(*_tmpC0A)),((_tmpC0A[0]=((
_tmpC0B.tag=15,((_tmpC0B.f1=(void*)Cyc_Tcutil_copy_type(_tmp94),((_tmpC0B.f2=(
void*)Cyc_Tcutil_copy_type(_tmp95),_tmpC0B)))))),_tmpC0A))));}_LL44: if(*((int*)
_tmp61)!= 16)goto _LL46;_tmp96=((struct Cyc_Absyn_TypedefType_struct*)_tmp61)->f1;
_tmp97=((struct Cyc_Absyn_TypedefType_struct*)_tmp61)->f2;_tmp98=((struct Cyc_Absyn_TypedefType_struct*)
_tmp61)->f3;_LL45: {struct Cyc_Absyn_TypedefType_struct _tmpC0E;struct Cyc_Absyn_TypedefType_struct*
_tmpC0D;return(void*)((_tmpC0D=_cycalloc(sizeof(*_tmpC0D)),((_tmpC0D[0]=((
_tmpC0E.tag=16,((_tmpC0E.f1=_tmp96,((_tmpC0E.f2=Cyc_Tcutil_copy_types(_tmp97),((
_tmpC0E.f3=_tmp98,((_tmpC0E.f4=0,_tmpC0E)))))))))),_tmpC0D))));}_LL46: if((int)
_tmp61 != 3)goto _LL48;_LL47: goto _LL49;_LL48: if((int)_tmp61 != 2)goto _LL4A;_LL49:
return t;_LL4A: if(_tmp61 <= (void*)4)goto _LL4C;if(*((int*)_tmp61)!= 19)goto _LL4C;
_tmp99=(void*)((struct Cyc_Absyn_AccessEff_struct*)_tmp61)->f1;_LL4B: {struct Cyc_Absyn_AccessEff_struct
_tmpC11;struct Cyc_Absyn_AccessEff_struct*_tmpC10;return(void*)((_tmpC10=
_cycalloc(sizeof(*_tmpC10)),((_tmpC10[0]=((_tmpC11.tag=19,((_tmpC11.f1=(void*)
Cyc_Tcutil_copy_type(_tmp99),_tmpC11)))),_tmpC10))));}_LL4C: if(_tmp61 <= (void*)4)
goto _LL4E;if(*((int*)_tmp61)!= 20)goto _LL4E;_tmp9A=((struct Cyc_Absyn_JoinEff_struct*)
_tmp61)->f1;_LL4D: {struct Cyc_Absyn_JoinEff_struct _tmpC14;struct Cyc_Absyn_JoinEff_struct*
_tmpC13;return(void*)((_tmpC13=_cycalloc(sizeof(*_tmpC13)),((_tmpC13[0]=((
_tmpC14.tag=20,((_tmpC14.f1=Cyc_Tcutil_copy_types(_tmp9A),_tmpC14)))),_tmpC13))));}
_LL4E: if(_tmp61 <= (void*)4)goto _LL19;if(*((int*)_tmp61)!= 21)goto _LL19;_tmp9B=(
void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp61)->f1;_LL4F: {struct Cyc_Absyn_RgnsEff_struct
_tmpC17;struct Cyc_Absyn_RgnsEff_struct*_tmpC16;return(void*)((_tmpC16=_cycalloc(
sizeof(*_tmpC16)),((_tmpC16[0]=((_tmpC17.tag=21,((_tmpC17.f1=(void*)Cyc_Tcutil_copy_type(
_tmp9B),_tmpC17)))),_tmpC16))));}_LL19:;}int Cyc_Tcutil_kind_leq(void*k1,void*k2);
int Cyc_Tcutil_kind_leq(void*k1,void*k2){if(k1 == k2)return 1;{struct _tuple0 _tmpC18;
struct _tuple0 _tmpE0=(_tmpC18.f1=k1,((_tmpC18.f2=k2,_tmpC18)));void*_tmpE1;void*
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
_tmpEC)->f2;_LL6A: return _tmpEE;_LL6B:;_LL6C: {const char*_tmpC1B;void*_tmpC1A;(
_tmpC1A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmpC1B="kind not suitably constrained!",_tag_dyneither(_tmpC1B,sizeof(char),31))),
_tag_dyneither(_tmpC1A,sizeof(void*),0)));}_LL66:;}struct _tuple8 Cyc_Tcutil_swap_kind(
void*t,void*kb);struct _tuple8 Cyc_Tcutil_swap_kind(void*t,void*kb){void*_tmpF1=
Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*_tmpF2;_LL6E: if(_tmpF1 <= (void*)4)
goto _LL70;if(*((int*)_tmpF1)!= 1)goto _LL70;_tmpF2=((struct Cyc_Absyn_VarType_struct*)
_tmpF1)->f1;_LL6F: {void*_tmpF3=(void*)_tmpF2->kind;(void*)(_tmpF2->kind=(void*)
kb);{struct _tuple8 _tmpC1C;return(_tmpC1C.f1=_tmpF2,((_tmpC1C.f2=_tmpF3,_tmpC1C)));}}
_LL70:;_LL71: {const char*_tmpC20;void*_tmpC1F[1];struct Cyc_String_pa_struct
_tmpC1E;(_tmpC1E.tag=0,((_tmpC1E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmpC1F[0]=& _tmpC1E,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpC20="swap_kind: cannot update the kind of %s",
_tag_dyneither(_tmpC20,sizeof(char),40))),_tag_dyneither(_tmpC1F,sizeof(void*),1)))))));}
_LL6D:;}void*Cyc_Tcutil_typ_kind(void*t);void*Cyc_Tcutil_typ_kind(void*t){void*
_tmpF8=Cyc_Tcutil_compress(t);struct Cyc_Core_Opt*_tmpF9;struct Cyc_Absyn_Tvar*
_tmpFA;void*_tmpFB;struct Cyc_Absyn_DatatypeInfo _tmpFC;union Cyc_Absyn_DatatypeInfoU_union
_tmpFD;struct Cyc_Absyn_UnknownDatatypeInfo _tmpFE;struct Cyc_Absyn_DatatypeInfo
_tmpFF;union Cyc_Absyn_DatatypeInfoU_union _tmp100;struct Cyc_Absyn_Datatypedecl**
_tmp101;struct Cyc_Absyn_Datatypedecl*_tmp102;struct Cyc_Absyn_DatatypeFieldInfo
_tmp103;union Cyc_Absyn_DatatypeFieldInfoU_union _tmp104;struct Cyc_Absyn_Datatypedecl*
_tmp105;struct Cyc_Absyn_Datatypefield*_tmp106;struct Cyc_Absyn_DatatypeFieldInfo
_tmp107;union Cyc_Absyn_DatatypeFieldInfoU_union _tmp108;struct Cyc_Absyn_Enumdecl*
_tmp109;struct Cyc_Absyn_AggrInfo _tmp10A;union Cyc_Absyn_AggrInfoU_union _tmp10B;
struct Cyc_Absyn_AggrInfo _tmp10C;union Cyc_Absyn_AggrInfoU_union _tmp10D;struct Cyc_Absyn_Aggrdecl**
_tmp10E;struct Cyc_Absyn_Aggrdecl*_tmp10F;struct Cyc_Absyn_Aggrdecl _tmp110;struct
Cyc_Absyn_AggrdeclImpl*_tmp111;struct Cyc_Absyn_Enumdecl*_tmp112;struct Cyc_Absyn_PtrInfo
_tmp113;struct Cyc_Absyn_Typedefdecl*_tmp114;_LL73: if(_tmpF8 <= (void*)4)goto _LL77;
if(*((int*)_tmpF8)!= 0)goto _LL75;_tmpF9=((struct Cyc_Absyn_Evar_struct*)_tmpF8)->f1;
_LL74: return(void*)((struct Cyc_Core_Opt*)_check_null(_tmpF9))->v;_LL75: if(*((int*)
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
_tmpF8)!= 2)goto _LL8B;_tmpFC=((struct Cyc_Absyn_DatatypeType_struct*)_tmpF8)->f1;
_tmpFD=_tmpFC.datatype_info;if((((((struct Cyc_Absyn_DatatypeType_struct*)_tmpF8)->f1).datatype_info).UnknownDatatype).tag
!= 0)goto _LL8B;_tmpFE=(_tmpFD.UnknownDatatype).f1;_LL8A: if(_tmpFE.is_flat)return(
void*)1;else{return(void*)2;}_LL8B: if(_tmpF8 <= (void*)4)goto _LL8D;if(*((int*)
_tmpF8)!= 2)goto _LL8D;_tmpFF=((struct Cyc_Absyn_DatatypeType_struct*)_tmpF8)->f1;
_tmp100=_tmpFF.datatype_info;if((((((struct Cyc_Absyn_DatatypeType_struct*)_tmpF8)->f1).datatype_info).KnownDatatype).tag
!= 1)goto _LL8D;_tmp101=(_tmp100.KnownDatatype).f1;_tmp102=*_tmp101;_LL8C: if(
_tmp102->is_flat)return(void*)1;else{return(void*)2;}_LL8D: if(_tmpF8 <= (void*)4)
goto _LL8F;if(*((int*)_tmpF8)!= 3)goto _LL8F;_tmp103=((struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmpF8)->f1;_tmp104=_tmp103.field_info;if((((((struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmpF8)->f1).field_info).KnownDatatypefield).tag != 1)goto _LL8F;_tmp105=(_tmp104.KnownDatatypefield).f1;
_tmp106=(_tmp104.KnownDatatypefield).f2;_LL8E: if(_tmp105->is_flat)return(void*)1;
else{if(_tmp106->typs == 0)return(void*)2;else{return(void*)1;}}_LL8F: if(_tmpF8 <= (
void*)4)goto _LL91;if(*((int*)_tmpF8)!= 3)goto _LL91;_tmp107=((struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmpF8)->f1;_tmp108=_tmp107.field_info;if((((((struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmpF8)->f1).field_info).UnknownDatatypefield).tag != 0)goto _LL91;_LL90: {const
char*_tmpC23;void*_tmpC22;(_tmpC22=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpC23="typ_kind: Unresolved DatatypeFieldType",
_tag_dyneither(_tmpC23,sizeof(char),39))),_tag_dyneither(_tmpC22,sizeof(void*),0)));}
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
_tmpC26;void*_tmpC25;(_tmpC25=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpC26="typ_kind: forward constr in ptr bounds",
_tag_dyneither(_tmpC26,sizeof(char),39))),_tag_dyneither(_tmpC25,sizeof(void*),0)));}
_LLAF:;}_LL9F: if(_tmpF8 <= (void*)4)goto _LLA1;if(*((int*)_tmpF8)!= 17)goto _LLA1;
_LLA0: return(void*)7;_LLA1: if(_tmpF8 <= (void*)4)goto _LLA3;if(*((int*)_tmpF8)!= 
18)goto _LLA3;_LLA2: return(void*)2;_LLA3: if(_tmpF8 <= (void*)4)goto _LLA5;if(*((int*)
_tmpF8)!= 7)goto _LLA5;_LLA4: goto _LLA6;_LLA5: if(_tmpF8 <= (void*)4)goto _LLA7;if(*((
int*)_tmpF8)!= 9)goto _LLA7;_LLA6: return(void*)1;_LLA7: if(_tmpF8 <= (void*)4)goto
_LLA9;if(*((int*)_tmpF8)!= 16)goto _LLA9;_tmp114=((struct Cyc_Absyn_TypedefType_struct*)
_tmpF8)->f3;_LLA8: if(_tmp114 == 0  || _tmp114->kind == 0){const char*_tmpC2A;void*
_tmpC29[1];struct Cyc_String_pa_struct _tmpC28;(_tmpC28.tag=0,((_tmpC28.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpC29[0]=&
_tmpC28,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmpC2A="typ_kind: typedef found: %s",_tag_dyneither(_tmpC2A,sizeof(char),28))),
_tag_dyneither(_tmpC29,sizeof(void*),1)))))));}return(void*)((struct Cyc_Core_Opt*)
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
struct Cyc_Absyn_DatatypeInfo _tmp137;struct Cyc_List_List*_tmp138;struct Cyc_Core_Opt*
_tmp139;struct Cyc_List_List*_tmp13A;struct Cyc_Absyn_DatatypeFieldInfo _tmp13B;
struct Cyc_List_List*_tmp13C;struct Cyc_Absyn_AggrInfo _tmp13D;struct Cyc_List_List*
_tmp13E;struct Cyc_List_List*_tmp13F;void*_tmp140;void*_tmp141;void*_tmp142;void*
_tmp143;struct Cyc_List_List*_tmp144;_LLBE: if(_tmp125 <= (void*)4)goto _LLDE;if(*((
int*)_tmp125)!= 1)goto _LLC0;_tmp126=((struct Cyc_Absyn_VarType_struct*)_tmp125)->f1;
_LLBF: if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,
env,_tmp126)){{const char*_tmpC2B;Cyc_Tcutil_failure_reason=((_tmpC2B="(type variable would escape scope)",
_tag_dyneither(_tmpC2B,sizeof(char),35)));}(int)_throw((void*)Cyc_Tcutil_Unify);}
goto _LLBD;_LLC0: if(*((int*)_tmp125)!= 0)goto _LLC2;_tmp127=((struct Cyc_Absyn_Evar_struct*)
_tmp125)->f2;_tmp128=((struct Cyc_Absyn_Evar_struct*)_tmp125)->f4;_tmp129=(struct
Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)_tmp125)->f4;_LLC1: if(t == evar){{
const char*_tmpC2C;Cyc_Tcutil_failure_reason=((_tmpC2C="(occurs check)",
_tag_dyneither(_tmpC2C,sizeof(char),15)));}(int)_throw((void*)Cyc_Tcutil_Unify);}
else{if(_tmp127 != 0)Cyc_Tcutil_occurs(evar,r,env,(void*)_tmp127->v);else{int
problem=0;{struct Cyc_List_List*s=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(*_tmp129))->v;for(0;s != 0;s=s->tl){if(!((int(*)(int(*compare)(struct
Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*
x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,(struct Cyc_Absyn_Tvar*)s->hd)){
problem=1;break;}}}if(problem){struct Cyc_List_List*_tmp147=0;{struct Cyc_List_List*
s=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*_tmp129))->v;for(0;s
!= 0;s=s->tl){if(((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,
env,(struct Cyc_Absyn_Tvar*)s->hd)){struct Cyc_List_List*_tmpC2D;_tmp147=((_tmpC2D=
_cycalloc(sizeof(*_tmpC2D)),((_tmpC2D->hd=(struct Cyc_Absyn_Tvar*)s->hd,((_tmpC2D->tl=
_tmp147,_tmpC2D))))));}}}{struct Cyc_Core_Opt*_tmpC2E;*_tmp129=((_tmpC2E=
_cycalloc(sizeof(*_tmpC2E)),((_tmpC2E->v=_tmp147,_tmpC2E))));}}}}goto _LLBD;_LLC2:
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
_LLCC;_tmp137=((struct Cyc_Absyn_DatatypeType_struct*)_tmp125)->f1;_tmp138=
_tmp137.targs;_tmp139=_tmp137.rgn;_LLCB: if((unsigned int)_tmp139)Cyc_Tcutil_occurs(
evar,r,env,(void*)_tmp139->v);Cyc_Tcutil_occurslist(evar,r,env,_tmp138);goto
_LLBD;_LLCC: if(*((int*)_tmp125)!= 16)goto _LLCE;_tmp13A=((struct Cyc_Absyn_TypedefType_struct*)
_tmp125)->f2;_LLCD: _tmp13C=_tmp13A;goto _LLCF;_LLCE: if(*((int*)_tmp125)!= 3)goto
_LLD0;_tmp13B=((struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp125)->f1;_tmp13C=
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
union Cyc_Absyn_Constraint_union _tmp153=x->v;void*_tmp154;_LLE1: if((_tmp153.No_constr).tag
!= 2)goto _LLE3;_LLE2:{union Cyc_Absyn_Constraint_union _tmpC36;x->v=(union Cyc_Absyn_Constraint_union)(((
_tmpC36.Forward_constr).tag=1,(((_tmpC36.Forward_constr).f1=y,_tmpC36))));}
return;_LLE3: if((_tmp153.Eq_constr).tag != 0)goto _LLE5;_tmp154=(_tmp153.Eq_constr).f1;
_LLE4: {union Cyc_Absyn_Constraint_union _tmp156=y->v;void*_tmp157;_LLE8: if((
_tmp156.No_constr).tag != 2)goto _LLEA;_LLE9: y->v=x->v;return;_LLEA: if((_tmp156.Eq_constr).tag
!= 0)goto _LLEC;_tmp157=(_tmp156.Eq_constr).f1;_LLEB: if(cmp(_tmp154,_tmp157)!= 0){
Cyc_Tcutil_failure_reason=reason;(int)_throw((void*)Cyc_Tcutil_Unify);}return;
_LLEC: if((_tmp156.Forward_constr).tag != 1)goto _LLE7;_LLED: {const char*_tmpC39;
void*_tmpC38;(_tmpC38=0,Cyc_Tcutil_impos(((_tmpC39="unify_conref: forward after compress(2)",
_tag_dyneither(_tmpC39,sizeof(char),40))),_tag_dyneither(_tmpC38,sizeof(void*),0)));}
_LLE7:;}_LLE5: if((_tmp153.Forward_constr).tag != 1)goto _LLE0;_LLE6: {const char*
_tmpC3C;void*_tmpC3B;(_tmpC3B=0,Cyc_Tcutil_impos(((_tmpC3C="unify_conref: forward after compress",
_tag_dyneither(_tmpC3C,sizeof(char),37))),_tag_dyneither(_tmpC3B,sizeof(void*),0)));}
_LLE0:;}}static int Cyc_Tcutil_unify_conrefs(int(*cmp)(void*,void*),struct Cyc_Absyn_Conref*
x,struct Cyc_Absyn_Conref*y);static int Cyc_Tcutil_unify_conrefs(int(*cmp)(void*,
void*),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y){struct _handler_cons
_tmp15C;_push_handler(& _tmp15C);{int _tmp15E=0;if(setjmp(_tmp15C.handler))_tmp15E=
1;if(!_tmp15E){Cyc_Tcutil_unify_it_conrefs(cmp,x,y,(struct _dyneither_ptr)
_tag_dyneither(0,0,0));{int _tmp15F=1;_npop_handler(0);return _tmp15F;};
_pop_handler();}else{void*_tmp15D=(void*)_exn_thrown;void*_tmp161=_tmp15D;_LLEF:
if(_tmp161 != Cyc_Tcutil_Unify)goto _LLF1;_LLF0: return 0;_LLF1:;_LLF2:(void)_throw(
_tmp161);_LLEE:;}}}static int Cyc_Tcutil_boundscmp(void*b1,void*b2);static int Cyc_Tcutil_boundscmp(
void*b1,void*b2){struct _tuple0 _tmpC3D;struct _tuple0 _tmp163=(_tmpC3D.f1=b1,((
_tmpC3D.f2=b2,_tmpC3D)));void*_tmp164;void*_tmp165;void*_tmp166;void*_tmp167;
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
_tuple0 _tmpC3E;struct _tuple0 _tmp16D=(_tmpC3E.f1=b1,((_tmpC3E.f2=b2,_tmpC3E)));
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
_tuple0 _tmpC3F;struct _tuple0 _tmp178=(_tmpC3F.f1=att1,((_tmpC3F.f2=att2,_tmpC3F)));
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
_LL147:{struct Cyc_Absyn_ValueofType_struct _tmpC42;struct Cyc_Absyn_ValueofType_struct*
_tmpC41;t=(void*)((_tmpC41=_cycalloc(sizeof(*_tmpC41)),((_tmpC41[0]=((_tmpC42.tag=
17,((_tmpC42.f1=Cyc_Absyn_uint_exp(0,0),_tmpC42)))),_tmpC41))));}goto _LL13F;
_LL148:;_LL149: t=Cyc_Absyn_sint_typ;goto _LL13F;_LL13F:;}{struct _tuple8*_tmpC43;
return(_tmpC43=_cycalloc(sizeof(*_tmpC43)),((_tmpC43->f1=tv,((_tmpC43->f2=t,
_tmpC43)))));}}static void*Cyc_Tcutil_rgns_of(void*t);static void*Cyc_Tcutil_rgns_of(
void*t){void*_tmp197=Cyc_Tcutil_compress(t);void*_tmp198;void*_tmp199;void*
_tmp19A;struct Cyc_Absyn_DatatypeInfo _tmp19B;struct Cyc_List_List*_tmp19C;struct
Cyc_Core_Opt*_tmp19D;struct Cyc_Absyn_PtrInfo _tmp19E;void*_tmp19F;struct Cyc_Absyn_PtrAtts
_tmp1A0;void*_tmp1A1;struct Cyc_Absyn_ArrayInfo _tmp1A2;void*_tmp1A3;struct Cyc_List_List*
_tmp1A4;struct Cyc_Absyn_DatatypeFieldInfo _tmp1A5;struct Cyc_List_List*_tmp1A6;
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
_tmpC46;struct Cyc_Absyn_AccessEff_struct*_tmpC45;return(void*)((_tmpC45=
_cycalloc(sizeof(*_tmpC45)),((_tmpC45[0]=((_tmpC46.tag=19,((_tmpC46.f1=(void*)t,
_tmpC46)))),_tmpC45))));}_LL186: if((int)_tmp1B3 != 6)goto _LL188;_LL187: return t;
_LL188: if((int)_tmp1B3 != 7)goto _LL18A;_LL189: return Cyc_Absyn_empty_effect;_LL18A:;
_LL18B: {struct Cyc_Absyn_RgnsEff_struct _tmpC49;struct Cyc_Absyn_RgnsEff_struct*
_tmpC48;return(void*)((_tmpC48=_cycalloc(sizeof(*_tmpC48)),((_tmpC48[0]=((
_tmpC49.tag=21,((_tmpC49.f1=(void*)t,_tmpC49)))),_tmpC48))));}_LL17F:;}_LL15D:
if(*((int*)_tmp197)!= 14)goto _LL15F;_tmp198=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp197)->f1;_LL15E: {struct Cyc_Absyn_AccessEff_struct _tmpC4C;struct Cyc_Absyn_AccessEff_struct*
_tmpC4B;return(void*)((_tmpC4B=_cycalloc(sizeof(*_tmpC4B)),((_tmpC4B[0]=((
_tmpC4C.tag=19,((_tmpC4C.f1=(void*)_tmp198,_tmpC4C)))),_tmpC4B))));}_LL15F: if(*((
int*)_tmp197)!= 15)goto _LL161;_tmp199=(void*)((struct Cyc_Absyn_DynRgnType_struct*)
_tmp197)->f1;_tmp19A=(void*)((struct Cyc_Absyn_DynRgnType_struct*)_tmp197)->f2;
_LL160: {struct Cyc_Absyn_AccessEff_struct _tmpC4F;struct Cyc_Absyn_AccessEff_struct*
_tmpC4E;return(void*)((_tmpC4E=_cycalloc(sizeof(*_tmpC4E)),((_tmpC4E[0]=((
_tmpC4F.tag=19,((_tmpC4F.f1=(void*)_tmp19A,_tmpC4F)))),_tmpC4E))));}_LL161: if(*((
int*)_tmp197)!= 2)goto _LL163;_tmp19B=((struct Cyc_Absyn_DatatypeType_struct*)
_tmp197)->f1;_tmp19C=_tmp19B.targs;_tmp19D=_tmp19B.rgn;_LL162: {struct Cyc_List_List*
ts=Cyc_List_map(Cyc_Tcutil_rgns_of,_tmp19C);if((unsigned int)_tmp19D){struct Cyc_Absyn_AccessEff_struct*
_tmpC55;struct Cyc_Absyn_AccessEff_struct _tmpC54;struct Cyc_List_List*_tmpC53;ts=((
_tmpC53=_cycalloc(sizeof(*_tmpC53)),((_tmpC53->hd=(void*)((void*)((_tmpC55=
_cycalloc(sizeof(*_tmpC55)),((_tmpC55[0]=((_tmpC54.tag=19,((_tmpC54.f1=(void*)((
void*)_tmp19D->v),_tmpC54)))),_tmpC55))))),((_tmpC53->tl=ts,_tmpC53))))));}{
struct Cyc_Absyn_JoinEff_struct _tmpC58;struct Cyc_Absyn_JoinEff_struct*_tmpC57;
return Cyc_Tcutil_normalize_effect((void*)((_tmpC57=_cycalloc(sizeof(*_tmpC57)),((
_tmpC57[0]=((_tmpC58.tag=20,((_tmpC58.f1=ts,_tmpC58)))),_tmpC57)))));}}_LL163:
if(*((int*)_tmp197)!= 4)goto _LL165;_tmp19E=((struct Cyc_Absyn_PointerType_struct*)
_tmp197)->f1;_tmp19F=(void*)_tmp19E.elt_typ;_tmp1A0=_tmp19E.ptr_atts;_tmp1A1=(
void*)_tmp1A0.rgn;_LL164: {struct Cyc_Absyn_JoinEff_struct _tmpC67;struct Cyc_Absyn_AccessEff_struct*
_tmpC66;struct Cyc_Absyn_AccessEff_struct _tmpC65;void*_tmpC64[2];struct Cyc_Absyn_JoinEff_struct*
_tmpC63;return Cyc_Tcutil_normalize_effect((void*)((_tmpC63=_cycalloc(sizeof(*
_tmpC63)),((_tmpC63[0]=((_tmpC67.tag=20,((_tmpC67.f1=((_tmpC64[1]=Cyc_Tcutil_rgns_of(
_tmp19F),((_tmpC64[0]=(void*)((_tmpC66=_cycalloc(sizeof(*_tmpC66)),((_tmpC66[0]=((
_tmpC65.tag=19,((_tmpC65.f1=(void*)_tmp1A1,_tmpC65)))),_tmpC66)))),Cyc_List_list(
_tag_dyneither(_tmpC64,sizeof(void*),2)))))),_tmpC67)))),_tmpC63)))));}_LL165:
if(*((int*)_tmp197)!= 7)goto _LL167;_tmp1A2=((struct Cyc_Absyn_ArrayType_struct*)
_tmp197)->f1;_tmp1A3=(void*)_tmp1A2.elt_type;_LL166: return Cyc_Tcutil_normalize_effect(
Cyc_Tcutil_rgns_of(_tmp1A3));_LL167: if(*((int*)_tmp197)!= 9)goto _LL169;_tmp1A4=((
struct Cyc_Absyn_TupleType_struct*)_tmp197)->f1;_LL168: {struct Cyc_List_List*
_tmp1C6=0;for(0;_tmp1A4 != 0;_tmp1A4=_tmp1A4->tl){struct Cyc_List_List*_tmpC68;
_tmp1C6=((_tmpC68=_cycalloc(sizeof(*_tmpC68)),((_tmpC68->hd=(void*)(*((struct
_tuple5*)_tmp1A4->hd)).f2,((_tmpC68->tl=_tmp1C6,_tmpC68))))));}_tmp1A6=_tmp1C6;
goto _LL16A;}_LL169: if(*((int*)_tmp197)!= 3)goto _LL16B;_tmp1A5=((struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp197)->f1;_tmp1A6=_tmp1A5.targs;_LL16A: _tmp1A8=_tmp1A6;goto _LL16C;_LL16B: if(*((
int*)_tmp197)!= 10)goto _LL16D;_tmp1A7=((struct Cyc_Absyn_AggrType_struct*)_tmp197)->f1;
_tmp1A8=_tmp1A7.targs;_LL16C: {struct Cyc_Absyn_JoinEff_struct _tmpC6B;struct Cyc_Absyn_JoinEff_struct*
_tmpC6A;return Cyc_Tcutil_normalize_effect((void*)((_tmpC6A=_cycalloc(sizeof(*
_tmpC6A)),((_tmpC6A[0]=((_tmpC6B.tag=20,((_tmpC6B.f1=Cyc_List_map(Cyc_Tcutil_rgns_of,
_tmp1A8),_tmpC6B)))),_tmpC6A)))));}_LL16D: if(*((int*)_tmp197)!= 11)goto _LL16F;
_tmp1A9=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp197)->f2;_LL16E: {struct Cyc_Absyn_JoinEff_struct
_tmpC6E;struct Cyc_Absyn_JoinEff_struct*_tmpC6D;return Cyc_Tcutil_normalize_effect((
void*)((_tmpC6D=_cycalloc(sizeof(*_tmpC6D)),((_tmpC6D[0]=((_tmpC6E.tag=20,((
_tmpC6E.f1=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Aggrfield*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of_field,_tmp1A9),_tmpC6E)))),
_tmpC6D)))));}_LL16F: if(*((int*)_tmp197)!= 18)goto _LL171;_LL170: return Cyc_Absyn_empty_effect;
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
_tmp197)->f2;_LL17E: {struct Cyc_Absyn_JoinEff_struct _tmpC71;struct Cyc_Absyn_JoinEff_struct*
_tmpC70;return Cyc_Tcutil_normalize_effect((void*)((_tmpC70=_cycalloc(sizeof(*
_tmpC70)),((_tmpC70[0]=((_tmpC71.tag=20,((_tmpC71.f1=Cyc_List_map(Cyc_Tcutil_rgns_of,
_tmp1B2),_tmpC71)))),_tmpC70)))));}_LL14A:;}void*Cyc_Tcutil_normalize_effect(
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
struct Cyc_List_List*_tmpC72;effects=((_tmpC72=_cycalloc(sizeof(*_tmpC72)),((
_tmpC72->hd=(void*)_tmp1D6,((_tmpC72->tl=effects,_tmpC72))))));}goto _LL19A;
_LL19A:;}}*_tmp1D1=Cyc_List_imp_rev(effects);return e;}}_LL18F: if(*((int*)_tmp1CF)
!= 21)goto _LL191;_tmp1D2=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp1CF)->f1;
_LL190: {void*_tmp1DA=Cyc_Tcutil_compress(_tmp1D2);_LL1A2: if(_tmp1DA <= (void*)4)
goto _LL1A6;if(*((int*)_tmp1DA)!= 0)goto _LL1A4;_LL1A3: goto _LL1A5;_LL1A4: if(*((int*)
_tmp1DA)!= 1)goto _LL1A6;_LL1A5: return e;_LL1A6:;_LL1A7: return Cyc_Tcutil_rgns_of(
_tmp1D2);_LL1A1:;}_LL191:;_LL192: return e;_LL18C:;}}static void*Cyc_Tcutil_dummy_fntype(
void*eff);static void*Cyc_Tcutil_dummy_fntype(void*eff){struct Cyc_Absyn_FnType_struct
_tmpC7C;struct Cyc_Core_Opt*_tmpC7B;struct Cyc_Absyn_FnInfo _tmpC7A;struct Cyc_Absyn_FnType_struct*
_tmpC79;struct Cyc_Absyn_FnType_struct*_tmp1DB=(_tmpC79=_cycalloc(sizeof(*_tmpC79)),((
_tmpC79[0]=((_tmpC7C.tag=8,((_tmpC7C.f1=((_tmpC7A.tvars=0,((_tmpC7A.effect=((
_tmpC7B=_cycalloc(sizeof(*_tmpC7B)),((_tmpC7B->v=(void*)eff,_tmpC7B)))),((
_tmpC7A.ret_typ=(void*)((void*)0),((_tmpC7A.args=0,((_tmpC7A.c_varargs=0,((
_tmpC7A.cyc_varargs=0,((_tmpC7A.rgn_po=0,((_tmpC7A.attributes=0,_tmpC7A)))))))))))))))),
_tmpC7C)))),_tmpC79)));return Cyc_Absyn_atb_typ((void*)_tmp1DB,(void*)2,Cyc_Absyn_empty_tqual(
0),Cyc_Absyn_bounds_one,Cyc_Absyn_false_conref);}int Cyc_Tcutil_region_in_effect(
int constrain,void*r,void*e);int Cyc_Tcutil_region_in_effect(int constrain,void*r,
void*e){r=Cyc_Tcutil_compress(r);if(r == (void*)2  || r == (void*)3)return 1;{void*
_tmp1E0=Cyc_Tcutil_compress(e);void*_tmp1E1;struct Cyc_List_List*_tmp1E2;void*
_tmp1E3;struct Cyc_Core_Opt*_tmp1E4;struct Cyc_Core_Opt*_tmp1E5;struct Cyc_Core_Opt**
_tmp1E6;struct Cyc_Core_Opt*_tmp1E7;_LL1A9: if(_tmp1E0 <= (void*)4)goto _LL1B1;if(*((
int*)_tmp1E0)!= 19)goto _LL1AB;_tmp1E1=(void*)((struct Cyc_Absyn_AccessEff_struct*)
_tmp1E0)->f1;_LL1AA: if(constrain)return Cyc_Tcutil_unify(r,_tmp1E1);_tmp1E1=Cyc_Tcutil_compress(
_tmp1E1);if(r == _tmp1E1)return 1;{struct _tuple0 _tmpC7D;struct _tuple0 _tmp1E9=(
_tmpC7D.f1=r,((_tmpC7D.f2=_tmp1E1,_tmpC7D)));void*_tmp1EA;struct Cyc_Absyn_Tvar*
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
_tmpC8C;struct Cyc_Absyn_AccessEff_struct*_tmpC8B;struct Cyc_Absyn_AccessEff_struct
_tmpC8A;void*_tmpC89[2];struct Cyc_Absyn_JoinEff_struct*_tmpC88;void*_tmp1F6=Cyc_Tcutil_dummy_fntype((
void*)((_tmpC88=_cycalloc(sizeof(*_tmpC88)),((_tmpC88[0]=((_tmpC8C.tag=20,((
_tmpC8C.f1=((_tmpC89[1]=(void*)((_tmpC8B=_cycalloc(sizeof(*_tmpC8B)),((_tmpC8B[0]=((
_tmpC8A.tag=19,((_tmpC8A.f1=(void*)r,_tmpC8A)))),_tmpC8B)))),((_tmpC89[0]=
_tmp1F5,Cyc_List_list(_tag_dyneither(_tmpC89,sizeof(void*),2)))))),_tmpC8C)))),
_tmpC88)))));{struct Cyc_Core_Opt*_tmpC8D;*_tmp1F3=((_tmpC8D=_cycalloc(sizeof(*
_tmpC8D)),((_tmpC8D->v=(void*)_tmp1F6,_tmpC8D))));}return 1;}}_LL1C0:;_LL1C1:
return 0;_LL1BD:;}_LL1BB:;_LL1BC: return Cyc_Tcutil_region_in_effect(constrain,r,
_tmp1EE);_LL1B8:;}_LL1AF: if(*((int*)_tmp1E0)!= 0)goto _LL1B1;_tmp1E4=((struct Cyc_Absyn_Evar_struct*)
_tmp1E0)->f1;_tmp1E5=((struct Cyc_Absyn_Evar_struct*)_tmp1E0)->f2;_tmp1E6=(struct
Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)_tmp1E0)->f2;_tmp1E7=((struct Cyc_Absyn_Evar_struct*)
_tmp1E0)->f4;_LL1B0: if(_tmp1E4 == 0  || (void*)_tmp1E4->v != (void*)6){const char*
_tmpC90;void*_tmpC8F;(_tmpC8F=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpC90="effect evar has wrong kind",
_tag_dyneither(_tmpC90,sizeof(char),27))),_tag_dyneither(_tmpC8F,sizeof(void*),0)));}
if(!constrain)return 0;{void*_tmp1FF=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,
_tmp1E7);Cyc_Tcutil_occurs(_tmp1FF,Cyc_Core_heap_region,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp1E7))->v,r);{struct Cyc_Absyn_JoinEff_struct
_tmpCA5;struct Cyc_List_List*_tmpCA4;struct Cyc_Absyn_AccessEff_struct _tmpCA3;
struct Cyc_Absyn_AccessEff_struct*_tmpCA2;struct Cyc_List_List*_tmpCA1;struct Cyc_Absyn_JoinEff_struct*
_tmpCA0;struct Cyc_Absyn_JoinEff_struct*_tmp200=(_tmpCA0=_cycalloc(sizeof(*
_tmpCA0)),((_tmpCA0[0]=((_tmpCA5.tag=20,((_tmpCA5.f1=((_tmpCA1=_cycalloc(sizeof(*
_tmpCA1)),((_tmpCA1->hd=(void*)_tmp1FF,((_tmpCA1->tl=((_tmpCA4=_cycalloc(sizeof(*
_tmpCA4)),((_tmpCA4->hd=(void*)((void*)((_tmpCA2=_cycalloc(sizeof(*_tmpCA2)),((
_tmpCA2[0]=((_tmpCA3.tag=19,((_tmpCA3.f1=(void*)r,_tmpCA3)))),_tmpCA2))))),((
_tmpCA4->tl=0,_tmpCA4)))))),_tmpCA1)))))),_tmpCA5)))),_tmpCA0)));{struct Cyc_Core_Opt*
_tmpCA6;*_tmp1E6=((_tmpCA6=_cycalloc(sizeof(*_tmpCA6)),((_tmpCA6->v=(void*)((
void*)_tmp200),_tmpCA6))));}return 1;}}_LL1B1:;_LL1B2: return 0;_LL1A8:;}}static int
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
Cyc_Absyn_RgnsEff_struct*)_tmp20F)->f1;_LL1CF: {struct _tuple0 _tmpCA7;struct
_tuple0 _tmp212=(_tmpCA7.f1=Cyc_Tcutil_compress(_tmp210),((_tmpCA7.f2=_tmp20A,
_tmpCA7)));void*_tmp213;struct Cyc_Absyn_Tvar*_tmp214;void*_tmp215;struct Cyc_Absyn_Tvar*
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
_tmpCAA;void*_tmpCA9;(_tmpCA9=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpCAA="effect evar has wrong kind",
_tag_dyneither(_tmpCAA,sizeof(char),27))),_tag_dyneither(_tmpCA9,sizeof(void*),0)));}
if(!may_constrain_evars)return 0;{void*_tmp219=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_ek,_tmp20E);Cyc_Tcutil_occurs(_tmp219,Cyc_Core_heap_region,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp20E))->v,t);{struct Cyc_Absyn_JoinEff_struct
_tmpCBF;struct Cyc_List_List*_tmpCBE;struct Cyc_Absyn_RgnsEff_struct _tmpCBD;struct
Cyc_Absyn_RgnsEff_struct*_tmpCBC;struct Cyc_List_List*_tmpCBB;struct Cyc_Absyn_JoinEff_struct*
_tmpCBA;struct Cyc_Absyn_JoinEff_struct*_tmp21A=(_tmpCBA=_cycalloc(sizeof(*
_tmpCBA)),((_tmpCBA[0]=((_tmpCBF.tag=20,((_tmpCBF.f1=((_tmpCBB=_cycalloc(sizeof(*
_tmpCBB)),((_tmpCBB->hd=(void*)_tmp219,((_tmpCBB->tl=((_tmpCBE=_cycalloc(sizeof(*
_tmpCBE)),((_tmpCBE->hd=(void*)((void*)((_tmpCBC=_cycalloc(sizeof(*_tmpCBC)),((
_tmpCBC[0]=((_tmpCBD.tag=21,((_tmpCBD.f1=(void*)t,_tmpCBD)))),_tmpCBC))))),((
_tmpCBE->tl=0,_tmpCBE)))))),_tmpCBB)))))),_tmpCBF)))),_tmpCBA)));{struct Cyc_Core_Opt*
_tmpCC0;*_tmp20D=((_tmpCC0=_cycalloc(sizeof(*_tmpCC0)),((_tmpCC0->v=(void*)((
void*)_tmp21A),_tmpCC0))));}return 1;}}_LL1CB:;_LL1CC: return 0;_LL1C2:;}}static int
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
_tmpCCF;struct Cyc_Absyn_VarType_struct*_tmpCCE;struct Cyc_Absyn_VarType_struct
_tmpCCD;void*_tmpCCC[2];struct Cyc_Absyn_JoinEff_struct*_tmpCCB;void*_tmp232=Cyc_Tcutil_dummy_fntype((
void*)((_tmpCCB=_cycalloc(sizeof(*_tmpCCB)),((_tmpCCB[0]=((_tmpCCF.tag=20,((
_tmpCCF.f1=((_tmpCCC[1]=(void*)((_tmpCCE=_cycalloc(sizeof(*_tmpCCE)),((_tmpCCE[0]=((
_tmpCCD.tag=1,((_tmpCCD.f1=v,_tmpCCD)))),_tmpCCE)))),((_tmpCCC[0]=_tmp231,Cyc_List_list(
_tag_dyneither(_tmpCCC,sizeof(void*),2)))))),_tmpCCF)))),_tmpCCB)))));{struct Cyc_Core_Opt*
_tmpCD0;*_tmp22F=((_tmpCD0=_cycalloc(sizeof(*_tmpCD0)),((_tmpCD0->v=(void*)
_tmp232,_tmpCD0))));}return 1;}}_LL1EA:;_LL1EB: return 0;_LL1E7:;}_LL1E5:;_LL1E6:
return Cyc_Tcutil_variable_in_effect(may_constrain_evars,v,_tmp22A);_LL1E2:;}
_LL1DE: if(*((int*)_tmp222)!= 0)goto _LL1E0;_tmp226=((struct Cyc_Absyn_Evar_struct*)
_tmp222)->f1;_tmp227=((struct Cyc_Absyn_Evar_struct*)_tmp222)->f2;_tmp228=(struct
Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)_tmp222)->f2;_tmp229=((struct Cyc_Absyn_Evar_struct*)
_tmp222)->f4;_LL1DF: if(_tmp226 == 0  || (void*)_tmp226->v != (void*)6){const char*
_tmpCD3;void*_tmpCD2;(_tmpCD2=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpCD3="effect evar has wrong kind",
_tag_dyneither(_tmpCD3,sizeof(char),27))),_tag_dyneither(_tmpCD2,sizeof(void*),0)));}{
void*_tmp23B=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,_tmp229);if(
!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*
l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp229))->v,v))return 0;{struct Cyc_Absyn_JoinEff_struct
_tmpCE8;struct Cyc_List_List*_tmpCE7;struct Cyc_Absyn_VarType_struct _tmpCE6;struct
Cyc_Absyn_VarType_struct*_tmpCE5;struct Cyc_List_List*_tmpCE4;struct Cyc_Absyn_JoinEff_struct*
_tmpCE3;struct Cyc_Absyn_JoinEff_struct*_tmp23C=(_tmpCE3=_cycalloc(sizeof(*
_tmpCE3)),((_tmpCE3[0]=((_tmpCE8.tag=20,((_tmpCE8.f1=((_tmpCE4=_cycalloc(sizeof(*
_tmpCE4)),((_tmpCE4->hd=(void*)_tmp23B,((_tmpCE4->tl=((_tmpCE7=_cycalloc(sizeof(*
_tmpCE7)),((_tmpCE7->hd=(void*)((void*)((_tmpCE5=_cycalloc(sizeof(*_tmpCE5)),((
_tmpCE5[0]=((_tmpCE6.tag=1,((_tmpCE6.f1=v,_tmpCE6)))),_tmpCE5))))),((_tmpCE7->tl=
0,_tmpCE7)))))),_tmpCE4)))))),_tmpCE8)))),_tmpCE3)));{struct Cyc_Core_Opt*_tmpCE9;*
_tmp228=((_tmpCE9=_cycalloc(sizeof(*_tmpCE9)),((_tmpCE9->v=(void*)((void*)
_tmp23C),_tmpCE9))));}return 1;}}_LL1E0:;_LL1E1: return 0;_LL1D7:;}}static int Cyc_Tcutil_evar_in_effect(
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
_tmpCEA;*_tmp24F=((_tmpCEA=_cycalloc(sizeof(*_tmpCEA)),((_tmpCEA->v=(void*)Cyc_Absyn_empty_effect,
_tmpCEA))));}return 1;_LL205:;_LL206: {const char*_tmpCEE;void*_tmpCED[1];struct
Cyc_String_pa_struct _tmpCEC;(_tmpCEC.tag=0,((_tmpCEC.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string(e1)),((_tmpCED[0]=& _tmpCEC,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpCEE="subset_effect: bad effect: %s",
_tag_dyneither(_tmpCEE,sizeof(char),30))),_tag_dyneither(_tmpCED,sizeof(void*),1)))))));}
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
_tmp260))->v)){{struct Cyc_Core_Opt*_tmpCEF;*_tmp262=((_tmpCEF=_cycalloc(sizeof(*
_tmpCEF)),((_tmpCEF->v=(void*)t2,_tmpCEF))));}return;}else{{void*_tmp266=t2;
struct Cyc_Core_Opt*_tmp267;struct Cyc_Core_Opt**_tmp268;struct Cyc_Core_Opt*
_tmp269;struct Cyc_Absyn_PtrInfo _tmp26A;_LL212: if(_tmp266 <= (void*)4)goto _LL216;
if(*((int*)_tmp266)!= 0)goto _LL214;_tmp267=((struct Cyc_Absyn_Evar_struct*)
_tmp266)->f2;_tmp268=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)
_tmp266)->f2;_tmp269=((struct Cyc_Absyn_Evar_struct*)_tmp266)->f4;_LL213: {struct
Cyc_List_List*_tmp26B=(struct Cyc_List_List*)_tmp263->v;{struct Cyc_List_List*s2=(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp269))->v;for(0;s2 != 0;
s2=s2->tl){if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,
_tmp26B,(struct Cyc_Absyn_Tvar*)s2->hd)){{const char*_tmpCF0;Cyc_Tcutil_failure_reason=((
_tmpCF0="(type variable would escape scope)",_tag_dyneither(_tmpCF0,sizeof(char),
35)));}(int)_throw((void*)Cyc_Tcutil_Unify);}}}if(Cyc_Tcutil_kind_leq((void*)
_tmp260->v,_tmp264)){{struct Cyc_Core_Opt*_tmpCF1;*_tmp268=((_tmpCF1=_cycalloc(
sizeof(*_tmpCF1)),((_tmpCF1->v=(void*)t1,_tmpCF1))));}return;}{const char*_tmpCF2;
Cyc_Tcutil_failure_reason=((_tmpCF2="(kinds are incompatible)",_tag_dyneither(
_tmpCF2,sizeof(char),25)));}goto _LL211;}_LL214: if(*((int*)_tmp266)!= 4)goto
_LL216;_tmp26A=((struct Cyc_Absyn_PointerType_struct*)_tmp266)->f1;if(!((void*)
_tmp260->v == (void*)2))goto _LL216;_LL215: {struct Cyc_Absyn_Conref*_tmp26F=Cyc_Absyn_compress_conref((
_tmp26A.ptr_atts).bounds);{union Cyc_Absyn_Constraint_union _tmp270=_tmp26F->v;
_LL219: if((_tmp270.No_constr).tag != 2)goto _LL21B;_LL21A:{union Cyc_Absyn_Constraint_union
_tmpCF3;_tmp26F->v=(union Cyc_Absyn_Constraint_union)(((_tmpCF3.Eq_constr).tag=0,(((
_tmpCF3.Eq_constr).f1=(void*)Cyc_Absyn_bounds_one,_tmpCF3))));}{struct Cyc_Core_Opt*
_tmpCF4;*_tmp262=((_tmpCF4=_cycalloc(sizeof(*_tmpCF4)),((_tmpCF4->v=(void*)t2,
_tmpCF4))));}return;_LL21B:;_LL21C: goto _LL218;_LL218:;}goto _LL211;}_LL216:;
_LL217: goto _LL211;_LL211:;}{const char*_tmpCF5;Cyc_Tcutil_failure_reason=((
_tmpCF5="(kinds are incompatible)",_tag_dyneither(_tmpCF5,sizeof(char),25)));}(
int)_throw((void*)Cyc_Tcutil_Unify);}}_LL20F:;_LL210: goto _LL20C;_LL20C:;}{struct
_tuple0 _tmpCF6;struct _tuple0 _tmp275=(_tmpCF6.f1=t2,((_tmpCF6.f2=t1,_tmpCF6)));
void*_tmp276;void*_tmp277;void*_tmp278;void*_tmp279;struct Cyc_Absyn_Tvar*_tmp27A;
void*_tmp27B;struct Cyc_Absyn_Tvar*_tmp27C;void*_tmp27D;struct Cyc_Absyn_AggrInfo
_tmp27E;union Cyc_Absyn_AggrInfoU_union _tmp27F;struct Cyc_List_List*_tmp280;void*
_tmp281;struct Cyc_Absyn_AggrInfo _tmp282;union Cyc_Absyn_AggrInfoU_union _tmp283;
struct Cyc_List_List*_tmp284;void*_tmp285;struct _tuple2*_tmp286;void*_tmp287;
struct _tuple2*_tmp288;void*_tmp289;struct Cyc_List_List*_tmp28A;void*_tmp28B;
struct Cyc_List_List*_tmp28C;void*_tmp28D;struct Cyc_Absyn_DatatypeInfo _tmp28E;
union Cyc_Absyn_DatatypeInfoU_union _tmp28F;struct Cyc_Absyn_Datatypedecl**_tmp290;
struct Cyc_Absyn_Datatypedecl*_tmp291;struct Cyc_List_List*_tmp292;struct Cyc_Core_Opt*
_tmp293;void*_tmp294;struct Cyc_Absyn_DatatypeInfo _tmp295;union Cyc_Absyn_DatatypeInfoU_union
_tmp296;struct Cyc_Absyn_Datatypedecl**_tmp297;struct Cyc_Absyn_Datatypedecl*
_tmp298;struct Cyc_List_List*_tmp299;struct Cyc_Core_Opt*_tmp29A;void*_tmp29B;
struct Cyc_Absyn_DatatypeFieldInfo _tmp29C;union Cyc_Absyn_DatatypeFieldInfoU_union
_tmp29D;struct Cyc_Absyn_Datatypedecl*_tmp29E;struct Cyc_Absyn_Datatypefield*
_tmp29F;struct Cyc_List_List*_tmp2A0;void*_tmp2A1;struct Cyc_Absyn_DatatypeFieldInfo
_tmp2A2;union Cyc_Absyn_DatatypeFieldInfoU_union _tmp2A3;struct Cyc_Absyn_Datatypedecl*
_tmp2A4;struct Cyc_Absyn_Datatypefield*_tmp2A5;struct Cyc_List_List*_tmp2A6;void*
_tmp2A7;struct Cyc_Absyn_PtrInfo _tmp2A8;void*_tmp2A9;struct Cyc_Absyn_Tqual _tmp2AA;
struct Cyc_Absyn_PtrAtts _tmp2AB;void*_tmp2AC;struct Cyc_Absyn_Conref*_tmp2AD;
struct Cyc_Absyn_Conref*_tmp2AE;struct Cyc_Absyn_Conref*_tmp2AF;void*_tmp2B0;
struct Cyc_Absyn_PtrInfo _tmp2B1;void*_tmp2B2;struct Cyc_Absyn_Tqual _tmp2B3;struct
Cyc_Absyn_PtrAtts _tmp2B4;void*_tmp2B5;struct Cyc_Absyn_Conref*_tmp2B6;struct Cyc_Absyn_Conref*
_tmp2B7;struct Cyc_Absyn_Conref*_tmp2B8;void*_tmp2B9;void*_tmp2BA;void*_tmp2BB;
void*_tmp2BC;void*_tmp2BD;void*_tmp2BE;void*_tmp2BF;void*_tmp2C0;void*_tmp2C1;
int _tmp2C2;void*_tmp2C3;int _tmp2C4;void*_tmp2C5;void*_tmp2C6;void*_tmp2C7;void*
_tmp2C8;void*_tmp2C9;struct Cyc_Absyn_Exp*_tmp2CA;void*_tmp2CB;struct Cyc_Absyn_Exp*
_tmp2CC;void*_tmp2CD;struct Cyc_Absyn_ArrayInfo _tmp2CE;void*_tmp2CF;struct Cyc_Absyn_Tqual
_tmp2D0;struct Cyc_Absyn_Exp*_tmp2D1;struct Cyc_Absyn_Conref*_tmp2D2;void*_tmp2D3;
struct Cyc_Absyn_ArrayInfo _tmp2D4;void*_tmp2D5;struct Cyc_Absyn_Tqual _tmp2D6;
struct Cyc_Absyn_Exp*_tmp2D7;struct Cyc_Absyn_Conref*_tmp2D8;void*_tmp2D9;struct
Cyc_Absyn_FnInfo _tmp2DA;struct Cyc_List_List*_tmp2DB;struct Cyc_Core_Opt*_tmp2DC;
void*_tmp2DD;struct Cyc_List_List*_tmp2DE;int _tmp2DF;struct Cyc_Absyn_VarargInfo*
_tmp2E0;struct Cyc_List_List*_tmp2E1;struct Cyc_List_List*_tmp2E2;void*_tmp2E3;
struct Cyc_Absyn_FnInfo _tmp2E4;struct Cyc_List_List*_tmp2E5;struct Cyc_Core_Opt*
_tmp2E6;void*_tmp2E7;struct Cyc_List_List*_tmp2E8;int _tmp2E9;struct Cyc_Absyn_VarargInfo*
_tmp2EA;struct Cyc_List_List*_tmp2EB;struct Cyc_List_List*_tmp2EC;void*_tmp2ED;
struct Cyc_List_List*_tmp2EE;void*_tmp2EF;struct Cyc_List_List*_tmp2F0;void*
_tmp2F1;void*_tmp2F2;struct Cyc_List_List*_tmp2F3;void*_tmp2F4;void*_tmp2F5;
struct Cyc_List_List*_tmp2F6;void*_tmp2F7;void*_tmp2F8;void*_tmp2F9;void*_tmp2FA;
void*_tmp2FB;void*_tmp2FC;void*_tmp2FD;void*_tmp2FE;void*_tmp2FF;void*_tmp300;
void*_tmp301;void*_tmp302;void*_tmp303;void*_tmp304;void*_tmp305;void*_tmp306;
void*_tmp307;void*_tmp308;void*_tmp309;void*_tmp30A;_LL21E: _tmp276=_tmp275.f1;
if(_tmp276 <= (void*)4)goto _LL220;if(*((int*)_tmp276)!= 0)goto _LL220;_LL21F: Cyc_Tcutil_unify_it(
t2,t1);return;_LL220: _tmp277=_tmp275.f1;if((int)_tmp277 != 0)goto _LL222;_tmp278=
_tmp275.f2;if((int)_tmp278 != 0)goto _LL222;_LL221: return;_LL222: _tmp279=_tmp275.f1;
if(_tmp279 <= (void*)4)goto _LL224;if(*((int*)_tmp279)!= 1)goto _LL224;_tmp27A=((
struct Cyc_Absyn_VarType_struct*)_tmp279)->f1;_tmp27B=_tmp275.f2;if(_tmp27B <= (
void*)4)goto _LL224;if(*((int*)_tmp27B)!= 1)goto _LL224;_tmp27C=((struct Cyc_Absyn_VarType_struct*)
_tmp27B)->f1;_LL223: {struct _dyneither_ptr*_tmp30B=_tmp27A->name;struct
_dyneither_ptr*_tmp30C=_tmp27C->name;int _tmp30D=_tmp27A->identity;int _tmp30E=
_tmp27C->identity;void*_tmp30F=Cyc_Tcutil_tvar_kind(_tmp27A);void*_tmp310=Cyc_Tcutil_tvar_kind(
_tmp27C);if(_tmp30E == _tmp30D  && Cyc_strptrcmp(_tmp30B,_tmp30C)== 0){if(_tmp30F
!= _tmp310){const char*_tmpCFC;void*_tmpCFB[3];struct Cyc_String_pa_struct _tmpCFA;
struct Cyc_String_pa_struct _tmpCF9;struct Cyc_String_pa_struct _tmpCF8;(_tmpCF8.tag=
0,((_tmpCF8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(
_tmp310)),((_tmpCF9.tag=0,((_tmpCF9.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_kind2string(_tmp30F)),((_tmpCFA.tag=0,((_tmpCFA.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp30B),((_tmpCFB[0]=& _tmpCFA,((
_tmpCFB[1]=& _tmpCF9,((_tmpCFB[2]=& _tmpCF8,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpCFC="same type variable %s has kinds %s and %s",
_tag_dyneither(_tmpCFC,sizeof(char),42))),_tag_dyneither(_tmpCFB,sizeof(void*),3)))))))))))))))))));}
return;}{const char*_tmpCFD;Cyc_Tcutil_failure_reason=((_tmpCFD="(variable types are not the same)",
_tag_dyneither(_tmpCFD,sizeof(char),34)));}goto _LL21D;}_LL224: _tmp27D=_tmp275.f1;
if(_tmp27D <= (void*)4)goto _LL226;if(*((int*)_tmp27D)!= 10)goto _LL226;_tmp27E=((
struct Cyc_Absyn_AggrType_struct*)_tmp27D)->f1;_tmp27F=_tmp27E.aggr_info;_tmp280=
_tmp27E.targs;_tmp281=_tmp275.f2;if(_tmp281 <= (void*)4)goto _LL226;if(*((int*)
_tmp281)!= 10)goto _LL226;_tmp282=((struct Cyc_Absyn_AggrType_struct*)_tmp281)->f1;
_tmp283=_tmp282.aggr_info;_tmp284=_tmp282.targs;_LL225: {void*_tmp318;struct
_tuple2*_tmp319;struct _tuple6 _tmp317=Cyc_Absyn_aggr_kinded_name(_tmp283);_tmp318=
_tmp317.f1;_tmp319=_tmp317.f2;{void*_tmp31B;struct _tuple2*_tmp31C;struct _tuple6
_tmp31A=Cyc_Absyn_aggr_kinded_name(_tmp27F);_tmp31B=_tmp31A.f1;_tmp31C=_tmp31A.f2;
if(_tmp318 != _tmp31B){{const char*_tmpCFE;Cyc_Tcutil_failure_reason=((_tmpCFE="(struct and union type)",
_tag_dyneither(_tmpCFE,sizeof(char),24)));}goto _LL21D;}if(Cyc_Absyn_qvar_cmp(
_tmp319,_tmp31C)!= 0){{const char*_tmpCFF;Cyc_Tcutil_failure_reason=((_tmpCFF="(different type name)",
_tag_dyneither(_tmpCFF,sizeof(char),22)));}goto _LL21D;}Cyc_Tcutil_unify_list(
_tmp284,_tmp280);return;}}_LL226: _tmp285=_tmp275.f1;if(_tmp285 <= (void*)4)goto
_LL228;if(*((int*)_tmp285)!= 12)goto _LL228;_tmp286=((struct Cyc_Absyn_EnumType_struct*)
_tmp285)->f1;_tmp287=_tmp275.f2;if(_tmp287 <= (void*)4)goto _LL228;if(*((int*)
_tmp287)!= 12)goto _LL228;_tmp288=((struct Cyc_Absyn_EnumType_struct*)_tmp287)->f1;
_LL227: if(Cyc_Absyn_qvar_cmp(_tmp286,_tmp288)== 0)return;{const char*_tmpD00;Cyc_Tcutil_failure_reason=((
_tmpD00="(different enum types)",_tag_dyneither(_tmpD00,sizeof(char),23)));}goto
_LL21D;_LL228: _tmp289=_tmp275.f1;if(_tmp289 <= (void*)4)goto _LL22A;if(*((int*)
_tmp289)!= 13)goto _LL22A;_tmp28A=((struct Cyc_Absyn_AnonEnumType_struct*)_tmp289)->f1;
_tmp28B=_tmp275.f2;if(_tmp28B <= (void*)4)goto _LL22A;if(*((int*)_tmp28B)!= 13)
goto _LL22A;_tmp28C=((struct Cyc_Absyn_AnonEnumType_struct*)_tmp28B)->f1;_LL229: {
int bad=0;for(0;_tmp28A != 0  && _tmp28C != 0;(_tmp28A=_tmp28A->tl,_tmp28C=_tmp28C->tl)){
struct Cyc_Absyn_Enumfield*_tmp320=(struct Cyc_Absyn_Enumfield*)_tmp28A->hd;struct
Cyc_Absyn_Enumfield*_tmp321=(struct Cyc_Absyn_Enumfield*)_tmp28C->hd;if(Cyc_Absyn_qvar_cmp(
_tmp320->name,_tmp321->name)!= 0){{const char*_tmpD01;Cyc_Tcutil_failure_reason=((
_tmpD01="(different names for enum fields)",_tag_dyneither(_tmpD01,sizeof(char),
34)));}bad=1;break;}if(_tmp320->tag == _tmp321->tag)continue;if(_tmp320->tag == 0
 || _tmp321->tag == 0){{const char*_tmpD02;Cyc_Tcutil_failure_reason=((_tmpD02="(different tag values for enum fields)",
_tag_dyneither(_tmpD02,sizeof(char),39)));}bad=1;break;}if(!Cyc_Evexp_same_const_exp((
struct Cyc_Absyn_Exp*)_check_null(_tmp320->tag),(struct Cyc_Absyn_Exp*)_check_null(
_tmp321->tag))){{const char*_tmpD03;Cyc_Tcutil_failure_reason=((_tmpD03="(different tag values for enum fields)",
_tag_dyneither(_tmpD03,sizeof(char),39)));}bad=1;break;}}if(bad)goto _LL21D;if(
_tmp28A == 0  && _tmp28C == 0)return;{const char*_tmpD04;Cyc_Tcutil_failure_reason=((
_tmpD04="(different number of fields for enums)",_tag_dyneither(_tmpD04,sizeof(
char),39)));}goto _LL21D;}_LL22A: _tmp28D=_tmp275.f1;if(_tmp28D <= (void*)4)goto
_LL22C;if(*((int*)_tmp28D)!= 2)goto _LL22C;_tmp28E=((struct Cyc_Absyn_DatatypeType_struct*)
_tmp28D)->f1;_tmp28F=_tmp28E.datatype_info;if((((((struct Cyc_Absyn_DatatypeType_struct*)
_tmp28D)->f1).datatype_info).KnownDatatype).tag != 1)goto _LL22C;_tmp290=(_tmp28F.KnownDatatype).f1;
_tmp291=*_tmp290;_tmp292=_tmp28E.targs;_tmp293=_tmp28E.rgn;_tmp294=_tmp275.f2;
if(_tmp294 <= (void*)4)goto _LL22C;if(*((int*)_tmp294)!= 2)goto _LL22C;_tmp295=((
struct Cyc_Absyn_DatatypeType_struct*)_tmp294)->f1;_tmp296=_tmp295.datatype_info;
if((((((struct Cyc_Absyn_DatatypeType_struct*)_tmp294)->f1).datatype_info).KnownDatatype).tag
!= 1)goto _LL22C;_tmp297=(_tmp296.KnownDatatype).f1;_tmp298=*_tmp297;_tmp299=
_tmp295.targs;_tmp29A=_tmp295.rgn;_LL22B: if(_tmp291 == _tmp298  || Cyc_Absyn_qvar_cmp(
_tmp291->name,_tmp298->name)== 0){if(_tmp29A != 0  && _tmp293 != 0)Cyc_Tcutil_unify_it((
void*)_tmp29A->v,(void*)_tmp293->v);else{if(_tmp29A != 0  || _tmp293 != 0)goto
Datatype_fail;}Cyc_Tcutil_unify_list(_tmp299,_tmp292);return;}Datatype_fail: {
const char*_tmpD05;Cyc_Tcutil_failure_reason=((_tmpD05="(different datatype types)",
_tag_dyneither(_tmpD05,sizeof(char),27)));}goto _LL21D;_LL22C: _tmp29B=_tmp275.f1;
if(_tmp29B <= (void*)4)goto _LL22E;if(*((int*)_tmp29B)!= 3)goto _LL22E;_tmp29C=((
struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp29B)->f1;_tmp29D=_tmp29C.field_info;
if((((((struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp29B)->f1).field_info).KnownDatatypefield).tag
!= 1)goto _LL22E;_tmp29E=(_tmp29D.KnownDatatypefield).f1;_tmp29F=(_tmp29D.KnownDatatypefield).f2;
_tmp2A0=_tmp29C.targs;_tmp2A1=_tmp275.f2;if(_tmp2A1 <= (void*)4)goto _LL22E;if(*((
int*)_tmp2A1)!= 3)goto _LL22E;_tmp2A2=((struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp2A1)->f1;_tmp2A3=_tmp2A2.field_info;if((((((struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp2A1)->f1).field_info).KnownDatatypefield).tag != 1)goto _LL22E;_tmp2A4=(
_tmp2A3.KnownDatatypefield).f1;_tmp2A5=(_tmp2A3.KnownDatatypefield).f2;_tmp2A6=
_tmp2A2.targs;_LL22D: if((_tmp29E == _tmp2A4  || Cyc_Absyn_qvar_cmp(_tmp29E->name,
_tmp2A4->name)== 0) && (_tmp29F == _tmp2A5  || Cyc_Absyn_qvar_cmp(_tmp29F->name,
_tmp2A5->name)== 0)){Cyc_Tcutil_unify_list(_tmp2A6,_tmp2A0);return;}{const char*
_tmpD06;Cyc_Tcutil_failure_reason=((_tmpD06="(different datatype field types)",
_tag_dyneither(_tmpD06,sizeof(char),33)));}goto _LL21D;_LL22E: _tmp2A7=_tmp275.f1;
if(_tmp2A7 <= (void*)4)goto _LL230;if(*((int*)_tmp2A7)!= 4)goto _LL230;_tmp2A8=((
struct Cyc_Absyn_PointerType_struct*)_tmp2A7)->f1;_tmp2A9=(void*)_tmp2A8.elt_typ;
_tmp2AA=_tmp2A8.elt_tq;_tmp2AB=_tmp2A8.ptr_atts;_tmp2AC=(void*)_tmp2AB.rgn;
_tmp2AD=_tmp2AB.nullable;_tmp2AE=_tmp2AB.bounds;_tmp2AF=_tmp2AB.zero_term;
_tmp2B0=_tmp275.f2;if(_tmp2B0 <= (void*)4)goto _LL230;if(*((int*)_tmp2B0)!= 4)goto
_LL230;_tmp2B1=((struct Cyc_Absyn_PointerType_struct*)_tmp2B0)->f1;_tmp2B2=(void*)
_tmp2B1.elt_typ;_tmp2B3=_tmp2B1.elt_tq;_tmp2B4=_tmp2B1.ptr_atts;_tmp2B5=(void*)
_tmp2B4.rgn;_tmp2B6=_tmp2B4.nullable;_tmp2B7=_tmp2B4.bounds;_tmp2B8=_tmp2B4.zero_term;
_LL22F: Cyc_Tcutil_unify_it(_tmp2B2,_tmp2A9);Cyc_Tcutil_unify_it(_tmp2AC,_tmp2B5);
Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;{const char*_tmpD07;((void(*)(
int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y,struct
_dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,_tmp2B8,
_tmp2AF,((_tmpD07="(not both zero terminated)",_tag_dyneither(_tmpD07,sizeof(
char),27))));}Cyc_Tcutil_unify_tqual(_tmp2B3,_tmp2B2,_tmp2AA,_tmp2A9);{const char*
_tmpD08;Cyc_Tcutil_unify_it_conrefs(Cyc_Tcutil_unify_it_bounds,_tmp2B7,_tmp2AE,((
_tmpD08="(different pointer bounds)",_tag_dyneither(_tmpD08,sizeof(char),27))));}{
union Cyc_Absyn_Constraint_union _tmp32A=(Cyc_Absyn_compress_conref(_tmp2B7))->v;
void*_tmp32B;_LL259: if((_tmp32A.Eq_constr).tag != 0)goto _LL25B;_tmp32B=(_tmp32A.Eq_constr).f1;
if((int)_tmp32B != 0)goto _LL25B;_LL25A: return;_LL25B:;_LL25C: goto _LL258;_LL258:;}{
const char*_tmpD09;((void(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct
Cyc_Absyn_Conref*y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,
_tmp2B6,_tmp2AD,((_tmpD09="(incompatible pointer types)",_tag_dyneither(_tmpD09,
sizeof(char),29))));}return;_LL230: _tmp2B9=_tmp275.f1;if(_tmp2B9 <= (void*)4)goto
_LL232;if(*((int*)_tmp2B9)!= 5)goto _LL232;_tmp2BA=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp2B9)->f1;_tmp2BB=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp2B9)->f2;
_tmp2BC=_tmp275.f2;if(_tmp2BC <= (void*)4)goto _LL232;if(*((int*)_tmp2BC)!= 5)goto
_LL232;_tmp2BD=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp2BC)->f1;_tmp2BE=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp2BC)->f2;_LL231: if(_tmp2BD == _tmp2BA
 && ((_tmp2BE == _tmp2BB  || _tmp2BE == (void*)2  && _tmp2BB == (void*)3) || _tmp2BE
== (void*)3  && _tmp2BB == (void*)2))return;{const char*_tmpD0A;Cyc_Tcutil_failure_reason=((
_tmpD0A="(different integral types)",_tag_dyneither(_tmpD0A,sizeof(char),27)));}
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
_LL239: if(!Cyc_Evexp_same_const_exp(_tmp2CA,_tmp2CC)){{const char*_tmpD0B;Cyc_Tcutil_failure_reason=((
_tmpD0B="(cannot prove expressions are the same)",_tag_dyneither(_tmpD0B,sizeof(
char),40)));}goto _LL21D;}return;_LL23A: _tmp2CD=_tmp275.f1;if(_tmp2CD <= (void*)4)
goto _LL23C;if(*((int*)_tmp2CD)!= 7)goto _LL23C;_tmp2CE=((struct Cyc_Absyn_ArrayType_struct*)
_tmp2CD)->f1;_tmp2CF=(void*)_tmp2CE.elt_type;_tmp2D0=_tmp2CE.tq;_tmp2D1=_tmp2CE.num_elts;
_tmp2D2=_tmp2CE.zero_term;_tmp2D3=_tmp275.f2;if(_tmp2D3 <= (void*)4)goto _LL23C;
if(*((int*)_tmp2D3)!= 7)goto _LL23C;_tmp2D4=((struct Cyc_Absyn_ArrayType_struct*)
_tmp2D3)->f1;_tmp2D5=(void*)_tmp2D4.elt_type;_tmp2D6=_tmp2D4.tq;_tmp2D7=_tmp2D4.num_elts;
_tmp2D8=_tmp2D4.zero_term;_LL23B: Cyc_Tcutil_unify_it(_tmp2D5,_tmp2CF);Cyc_Tcutil_unify_tqual(
_tmp2D6,_tmp2D5,_tmp2D0,_tmp2CF);Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=
t2;{const char*_tmpD0C;((void(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,
struct Cyc_Absyn_Conref*y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(
Cyc_Core_intcmp,_tmp2D2,_tmp2D8,((_tmpD0C="(not both zero terminated)",
_tag_dyneither(_tmpD0C,sizeof(char),27))));}if(_tmp2D1 == _tmp2D7)return;if(
_tmp2D1 == 0  || _tmp2D7 == 0)goto _LL21D;if(Cyc_Evexp_same_const_exp((struct Cyc_Absyn_Exp*)
_tmp2D1,(struct Cyc_Absyn_Exp*)_tmp2D7))return;{const char*_tmpD0D;Cyc_Tcutil_failure_reason=((
_tmpD0D="(different array sizes)",_tag_dyneither(_tmpD0D,sizeof(char),24)));}
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
const char*_tmpD0E;Cyc_Tcutil_failure_reason=((_tmpD0E="(second function type has too few type variables)",
_tag_dyneither(_tmpD0E,sizeof(char),50)));}(int)_throw((void*)Cyc_Tcutil_Unify);}{
void*_tmp333=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp2E5->hd);void*
_tmp334=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp2DB->hd);if(_tmp333 != 
_tmp334){{const char*_tmpD14;void*_tmpD13[3];struct Cyc_String_pa_struct _tmpD12;
struct Cyc_String_pa_struct _tmpD11;struct Cyc_String_pa_struct _tmpD10;Cyc_Tcutil_failure_reason=(
struct _dyneither_ptr)((_tmpD10.tag=0,((_tmpD10.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_kind2string(_tmp334)),((_tmpD11.tag=0,((_tmpD11.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(_tmp333)),((
_tmpD12.tag=0,((_tmpD12.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_tvar2string((
struct Cyc_Absyn_Tvar*)_tmp2E5->hd)),((_tmpD13[0]=& _tmpD12,((_tmpD13[1]=& _tmpD11,((
_tmpD13[2]=& _tmpD10,Cyc_aprintf(((_tmpD14="(type var %s has different kinds %s and %s)",
_tag_dyneither(_tmpD14,sizeof(char),44))),_tag_dyneither(_tmpD13,sizeof(void*),3))))))))))))))))))));}(
int)_throw((void*)Cyc_Tcutil_Unify);}{struct _tuple8*_tmpD1E;struct Cyc_Absyn_VarType_struct
_tmpD1D;struct Cyc_Absyn_VarType_struct*_tmpD1C;struct Cyc_List_List*_tmpD1B;inst=((
_tmpD1B=_region_malloc(rgn,sizeof(*_tmpD1B)),((_tmpD1B->hd=((_tmpD1E=
_region_malloc(rgn,sizeof(*_tmpD1E)),((_tmpD1E->f1=(struct Cyc_Absyn_Tvar*)
_tmp2DB->hd,((_tmpD1E->f2=(void*)((_tmpD1C=_cycalloc(sizeof(*_tmpD1C)),((_tmpD1C[
0]=((_tmpD1D.tag=1,((_tmpD1D.f1=(struct Cyc_Absyn_Tvar*)_tmp2E5->hd,_tmpD1D)))),
_tmpD1C)))),_tmpD1E)))))),((_tmpD1B->tl=inst,_tmpD1B))))));}_tmp2E5=_tmp2E5->tl;
_tmp2DB=_tmp2DB->tl;}}if(_tmp2DB != 0){{const char*_tmpD1F;Cyc_Tcutil_failure_reason=((
_tmpD1F="(second function type has too many type variables)",_tag_dyneither(
_tmpD1F,sizeof(char),51)));}_npop_handler(0);goto _LL21D;}if(inst != 0){{struct Cyc_Absyn_FnType_struct
_tmpD2B;struct Cyc_Absyn_FnInfo _tmpD2A;struct Cyc_Absyn_FnType_struct*_tmpD29;
struct Cyc_Absyn_FnType_struct _tmpD25;struct Cyc_Absyn_FnInfo _tmpD24;struct Cyc_Absyn_FnType_struct*
_tmpD23;Cyc_Tcutil_unify_it((void*)((_tmpD23=_cycalloc(sizeof(*_tmpD23)),((
_tmpD23[0]=((_tmpD25.tag=8,((_tmpD25.f1=((_tmpD24.tvars=0,((_tmpD24.effect=
_tmp2E6,((_tmpD24.ret_typ=(void*)_tmp2E7,((_tmpD24.args=_tmp2E8,((_tmpD24.c_varargs=
_tmp2E9,((_tmpD24.cyc_varargs=_tmp2EA,((_tmpD24.rgn_po=_tmp2EB,((_tmpD24.attributes=
_tmp2EC,_tmpD24)))))))))))))))),_tmpD25)))),_tmpD23)))),Cyc_Tcutil_rsubstitute(
rgn,inst,(void*)((_tmpD29=_cycalloc(sizeof(*_tmpD29)),((_tmpD29[0]=((_tmpD2B.tag=
8,((_tmpD2B.f1=((_tmpD2A.tvars=0,((_tmpD2A.effect=_tmp2DC,((_tmpD2A.ret_typ=(
void*)_tmp2DD,((_tmpD2A.args=_tmp2DE,((_tmpD2A.c_varargs=_tmp2DF,((_tmpD2A.cyc_varargs=
_tmp2E0,((_tmpD2A.rgn_po=_tmp2E1,((_tmpD2A.attributes=_tmp2E2,_tmpD2A)))))))))))))))),
_tmpD2B)))),_tmpD29))))));}done=1;}}if(done){_npop_handler(0);return;}Cyc_Tcutil_unify_it(
_tmp2E7,_tmp2DD);for(0;_tmp2E8 != 0  && _tmp2DE != 0;(_tmp2E8=_tmp2E8->tl,_tmp2DE=
_tmp2DE->tl)){struct Cyc_Absyn_Tqual _tmp346;void*_tmp347;struct _tuple3 _tmp345=*((
struct _tuple3*)_tmp2E8->hd);_tmp346=_tmp345.f2;_tmp347=_tmp345.f3;{struct Cyc_Absyn_Tqual
_tmp349;void*_tmp34A;struct _tuple3 _tmp348=*((struct _tuple3*)_tmp2DE->hd);_tmp349=
_tmp348.f2;_tmp34A=_tmp348.f3;Cyc_Tcutil_unify_it(_tmp347,_tmp34A);Cyc_Tcutil_unify_tqual(
_tmp346,_tmp347,_tmp349,_tmp34A);}}Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=
t2;if(_tmp2E8 != 0  || _tmp2DE != 0){{const char*_tmpD2C;Cyc_Tcutil_failure_reason=((
_tmpD2C="(function types have different number of arguments)",_tag_dyneither(
_tmpD2C,sizeof(char),52)));}_npop_handler(0);goto _LL21D;}if(_tmp2E9 != _tmp2DF){{
const char*_tmpD2D;Cyc_Tcutil_failure_reason=((_tmpD2D="(only one function type takes C varargs)",
_tag_dyneither(_tmpD2D,sizeof(char),41)));}_npop_handler(0);goto _LL21D;}{int
bad_cyc_vararg=0;{struct _tuple11 _tmpD2E;struct _tuple11 _tmp34E=(_tmpD2E.f1=
_tmp2EA,((_tmpD2E.f2=_tmp2E0,_tmpD2E)));struct Cyc_Absyn_VarargInfo*_tmp34F;
struct Cyc_Absyn_VarargInfo*_tmp350;struct Cyc_Absyn_VarargInfo*_tmp351;struct Cyc_Absyn_VarargInfo*
_tmp352;struct Cyc_Absyn_VarargInfo*_tmp353;struct Cyc_Absyn_VarargInfo _tmp354;
struct Cyc_Core_Opt*_tmp355;struct Cyc_Absyn_Tqual _tmp356;void*_tmp357;int _tmp358;
struct Cyc_Absyn_VarargInfo*_tmp359;struct Cyc_Absyn_VarargInfo _tmp35A;struct Cyc_Core_Opt*
_tmp35B;struct Cyc_Absyn_Tqual _tmp35C;void*_tmp35D;int _tmp35E;_LL25E: _tmp34F=
_tmp34E.f1;if(_tmp34F != 0)goto _LL260;_tmp350=_tmp34E.f2;if(_tmp350 != 0)goto
_LL260;_LL25F: goto _LL25D;_LL260: _tmp351=_tmp34E.f1;if(_tmp351 != 0)goto _LL262;
_LL261: goto _LL263;_LL262: _tmp352=_tmp34E.f2;if(_tmp352 != 0)goto _LL264;_LL263:
bad_cyc_vararg=1;{const char*_tmpD2F;Cyc_Tcutil_failure_reason=((_tmpD2F="(only one function type takes varargs)",
_tag_dyneither(_tmpD2F,sizeof(char),39)));}goto _LL25D;_LL264: _tmp353=_tmp34E.f1;
if(_tmp353 == 0)goto _LL25D;_tmp354=*_tmp353;_tmp355=_tmp354.name;_tmp356=_tmp354.tq;
_tmp357=(void*)_tmp354.type;_tmp358=_tmp354.inject;_tmp359=_tmp34E.f2;if(_tmp359
== 0)goto _LL25D;_tmp35A=*_tmp359;_tmp35B=_tmp35A.name;_tmp35C=_tmp35A.tq;_tmp35D=(
void*)_tmp35A.type;_tmp35E=_tmp35A.inject;_LL265: Cyc_Tcutil_unify_it(_tmp357,
_tmp35D);Cyc_Tcutil_unify_tqual(_tmp356,_tmp357,_tmp35C,_tmp35D);if(_tmp358 != 
_tmp35E){bad_cyc_vararg=1;{const char*_tmpD30;Cyc_Tcutil_failure_reason=((_tmpD30="(only one function type injects varargs)",
_tag_dyneither(_tmpD30,sizeof(char),41)));}}goto _LL25D;_LL25D:;}if(
bad_cyc_vararg){_npop_handler(0);goto _LL21D;}{int bad_effect=0;{struct _tuple12
_tmpD31;struct _tuple12 _tmp362=(_tmpD31.f1=_tmp2E6,((_tmpD31.f2=_tmp2DC,_tmpD31)));
struct Cyc_Core_Opt*_tmp363;struct Cyc_Core_Opt*_tmp364;struct Cyc_Core_Opt*_tmp365;
struct Cyc_Core_Opt*_tmp366;_LL267: _tmp363=_tmp362.f1;if(_tmp363 != 0)goto _LL269;
_tmp364=_tmp362.f2;if(_tmp364 != 0)goto _LL269;_LL268: goto _LL266;_LL269: _tmp365=
_tmp362.f1;if(_tmp365 != 0)goto _LL26B;_LL26A: goto _LL26C;_LL26B: _tmp366=_tmp362.f2;
if(_tmp366 != 0)goto _LL26D;_LL26C: bad_effect=1;goto _LL266;_LL26D:;_LL26E:
bad_effect=!Cyc_Tcutil_unify_effect((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp2E6))->v,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp2DC))->v);goto _LL266;
_LL266:;}Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;if(bad_effect){{const
char*_tmpD32;Cyc_Tcutil_failure_reason=((_tmpD32="(function type effects do not unify)",
_tag_dyneither(_tmpD32,sizeof(char),37)));}_npop_handler(0);goto _LL21D;}if(!Cyc_Tcutil_same_atts(
_tmp2E2,_tmp2EC)){{const char*_tmpD33;Cyc_Tcutil_failure_reason=((_tmpD33="(function types have different attributes)",
_tag_dyneither(_tmpD33,sizeof(char),43)));}_npop_handler(0);goto _LL21D;}if(!Cyc_Tcutil_same_rgn_po(
_tmp2E1,_tmp2EB)){{const char*_tmpD34;Cyc_Tcutil_failure_reason=((_tmpD34="(function types have different region lifetime orderings)",
_tag_dyneither(_tmpD34,sizeof(char),58)));}_npop_handler(0);goto _LL21D;}
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
char*_tmpD35;Cyc_Tcutil_failure_reason=((_tmpD35="(tuple types have different numbers of components)",
_tag_dyneither(_tmpD35,sizeof(char),51)));}goto _LL21D;_LL240: _tmp2F1=_tmp275.f1;
if(_tmp2F1 <= (void*)4)goto _LL242;if(*((int*)_tmp2F1)!= 11)goto _LL242;_tmp2F2=(
void*)((struct Cyc_Absyn_AnonAggrType_struct*)_tmp2F1)->f1;_tmp2F3=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp2F1)->f2;_tmp2F4=_tmp275.f2;if(_tmp2F4 <= (void*)4)goto _LL242;if(*((int*)
_tmp2F4)!= 11)goto _LL242;_tmp2F5=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp2F4)->f1;_tmp2F6=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp2F4)->f2;_LL241:
if(_tmp2F5 != _tmp2F2){{const char*_tmpD36;Cyc_Tcutil_failure_reason=((_tmpD36="(struct and union type)",
_tag_dyneither(_tmpD36,sizeof(char),24)));}goto _LL21D;}for(0;_tmp2F6 != 0  && 
_tmp2F3 != 0;(_tmp2F6=_tmp2F6->tl,_tmp2F3=_tmp2F3->tl)){struct Cyc_Absyn_Aggrfield*
_tmp372=(struct Cyc_Absyn_Aggrfield*)_tmp2F6->hd;struct Cyc_Absyn_Aggrfield*
_tmp373=(struct Cyc_Absyn_Aggrfield*)_tmp2F3->hd;if(Cyc_strptrcmp(_tmp372->name,
_tmp373->name)!= 0){{const char*_tmpD37;Cyc_Tcutil_failure_reason=((_tmpD37="(different member names)",
_tag_dyneither(_tmpD37,sizeof(char),25)));}(int)_throw((void*)Cyc_Tcutil_Unify);}
Cyc_Tcutil_unify_it((void*)_tmp372->type,(void*)_tmp373->type);Cyc_Tcutil_unify_tqual(
_tmp372->tq,(void*)_tmp372->type,_tmp373->tq,(void*)_tmp373->type);if(!Cyc_Tcutil_same_atts(
_tmp372->attributes,_tmp373->attributes)){Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=
t2;{const char*_tmpD38;Cyc_Tcutil_failure_reason=((_tmpD38="(different attributes on member)",
_tag_dyneither(_tmpD38,sizeof(char),33)));}(int)_throw((void*)Cyc_Tcutil_Unify);}
if((_tmp372->width != 0  && _tmp373->width == 0  || _tmp373->width != 0  && _tmp372->width
== 0) || (_tmp372->width != 0  && _tmp373->width != 0) && !Cyc_Evexp_same_const_exp((
struct Cyc_Absyn_Exp*)_check_null(_tmp372->width),(struct Cyc_Absyn_Exp*)
_check_null(_tmp373->width))){Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;{
const char*_tmpD39;Cyc_Tcutil_failure_reason=((_tmpD39="(different bitfield widths on member)",
_tag_dyneither(_tmpD39,sizeof(char),38)));}(int)_throw((void*)Cyc_Tcutil_Unify);}}
if(_tmp2F6 == 0  && _tmp2F3 == 0)return;Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=
t2;{const char*_tmpD3A;Cyc_Tcutil_failure_reason=((_tmpD3A="(different number of members)",
_tag_dyneither(_tmpD3A,sizeof(char),30)));}goto _LL21D;_LL242: _tmp2F7=_tmp275.f1;
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
19)goto _LL256;_LL255: if(Cyc_Tcutil_unify_effect(t1,t2))return;{const char*_tmpD3B;
Cyc_Tcutil_failure_reason=((_tmpD3B="(effects don't unify)",_tag_dyneither(
_tmpD3B,sizeof(char),22)));}goto _LL21D;_LL256:;_LL257: goto _LL21D;_LL21D:;}(int)
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
_tmp37D.Forward_constr).tag != 1)goto _LL276;_LL27C: {const char*_tmpD3E;void*
_tmpD3D;(_tmpD3D=0,Cyc_Tcutil_impos(((_tmpD3E="unify_conref: forward after compress(2)",
_tag_dyneither(_tmpD3E,sizeof(char),40))),_tag_dyneither(_tmpD3D,sizeof(void*),0)));}
_LL276:;}_LL274: if((_tmp37B.Forward_constr).tag != 1)goto _LL26F;_LL275: {const
char*_tmpD41;void*_tmpD40;(_tmpD40=0,Cyc_Tcutil_impos(((_tmpD41="unify_conref: forward after compress",
_tag_dyneither(_tmpD41,sizeof(char),37))),_tag_dyneither(_tmpD40,sizeof(void*),0)));}
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
return _tmp392;{struct _tuple0 _tmpD42;struct _tuple0 _tmp394=(_tmpD42.f1=t2,((
_tmpD42.f2=t1,_tmpD42)));void*_tmp395;void*_tmp396;void*_tmp397;struct Cyc_Absyn_Tvar*
_tmp398;void*_tmp399;struct Cyc_Absyn_Tvar*_tmp39A;void*_tmp39B;struct Cyc_Absyn_AggrInfo
_tmp39C;union Cyc_Absyn_AggrInfoU_union _tmp39D;struct Cyc_List_List*_tmp39E;void*
_tmp39F;struct Cyc_Absyn_AggrInfo _tmp3A0;union Cyc_Absyn_AggrInfoU_union _tmp3A1;
struct Cyc_List_List*_tmp3A2;void*_tmp3A3;struct _tuple2*_tmp3A4;void*_tmp3A5;
struct _tuple2*_tmp3A6;void*_tmp3A7;struct Cyc_List_List*_tmp3A8;void*_tmp3A9;
struct Cyc_List_List*_tmp3AA;void*_tmp3AB;struct Cyc_Absyn_DatatypeInfo _tmp3AC;
union Cyc_Absyn_DatatypeInfoU_union _tmp3AD;struct Cyc_Absyn_Datatypedecl**_tmp3AE;
struct Cyc_Absyn_Datatypedecl*_tmp3AF;struct Cyc_List_List*_tmp3B0;struct Cyc_Core_Opt*
_tmp3B1;void*_tmp3B2;struct Cyc_Absyn_DatatypeInfo _tmp3B3;union Cyc_Absyn_DatatypeInfoU_union
_tmp3B4;struct Cyc_Absyn_Datatypedecl**_tmp3B5;struct Cyc_Absyn_Datatypedecl*
_tmp3B6;struct Cyc_List_List*_tmp3B7;struct Cyc_Core_Opt*_tmp3B8;void*_tmp3B9;
struct Cyc_Absyn_DatatypeFieldInfo _tmp3BA;union Cyc_Absyn_DatatypeFieldInfoU_union
_tmp3BB;struct Cyc_Absyn_Datatypedecl*_tmp3BC;struct Cyc_Absyn_Datatypefield*
_tmp3BD;struct Cyc_List_List*_tmp3BE;void*_tmp3BF;struct Cyc_Absyn_DatatypeFieldInfo
_tmp3C0;union Cyc_Absyn_DatatypeFieldInfoU_union _tmp3C1;struct Cyc_Absyn_Datatypedecl*
_tmp3C2;struct Cyc_Absyn_Datatypefield*_tmp3C3;struct Cyc_List_List*_tmp3C4;void*
_tmp3C5;struct Cyc_Absyn_PtrInfo _tmp3C6;void*_tmp3C7;struct Cyc_Absyn_Tqual _tmp3C8;
struct Cyc_Absyn_PtrAtts _tmp3C9;void*_tmp3CA;struct Cyc_Absyn_Conref*_tmp3CB;
struct Cyc_Absyn_Conref*_tmp3CC;struct Cyc_Absyn_Conref*_tmp3CD;void*_tmp3CE;
struct Cyc_Absyn_PtrInfo _tmp3CF;void*_tmp3D0;struct Cyc_Absyn_Tqual _tmp3D1;struct
Cyc_Absyn_PtrAtts _tmp3D2;void*_tmp3D3;struct Cyc_Absyn_Conref*_tmp3D4;struct Cyc_Absyn_Conref*
_tmp3D5;struct Cyc_Absyn_Conref*_tmp3D6;void*_tmp3D7;void*_tmp3D8;void*_tmp3D9;
void*_tmp3DA;void*_tmp3DB;void*_tmp3DC;void*_tmp3DD;void*_tmp3DE;void*_tmp3DF;
int _tmp3E0;void*_tmp3E1;int _tmp3E2;void*_tmp3E3;struct Cyc_Absyn_ArrayInfo _tmp3E4;
void*_tmp3E5;struct Cyc_Absyn_Tqual _tmp3E6;struct Cyc_Absyn_Exp*_tmp3E7;struct Cyc_Absyn_Conref*
_tmp3E8;void*_tmp3E9;struct Cyc_Absyn_ArrayInfo _tmp3EA;void*_tmp3EB;struct Cyc_Absyn_Tqual
_tmp3EC;struct Cyc_Absyn_Exp*_tmp3ED;struct Cyc_Absyn_Conref*_tmp3EE;void*_tmp3EF;
struct Cyc_Absyn_FnInfo _tmp3F0;struct Cyc_List_List*_tmp3F1;struct Cyc_Core_Opt*
_tmp3F2;void*_tmp3F3;struct Cyc_List_List*_tmp3F4;int _tmp3F5;struct Cyc_Absyn_VarargInfo*
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
const char*_tmpD45;void*_tmpD44;(_tmpD44=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpD45="typecmp: can only compare closed types",
_tag_dyneither(_tmpD45,sizeof(char),39))),_tag_dyneither(_tmpD44,sizeof(void*),0)));}
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
void*)4)goto _LL2BF;if(*((int*)_tmp3AB)!= 2)goto _LL2BF;_tmp3AC=((struct Cyc_Absyn_DatatypeType_struct*)
_tmp3AB)->f1;_tmp3AD=_tmp3AC.datatype_info;if((((((struct Cyc_Absyn_DatatypeType_struct*)
_tmp3AB)->f1).datatype_info).KnownDatatype).tag != 1)goto _LL2BF;_tmp3AE=(_tmp3AD.KnownDatatype).f1;
_tmp3AF=*_tmp3AE;_tmp3B0=_tmp3AC.targs;_tmp3B1=_tmp3AC.rgn;_tmp3B2=_tmp394.f2;
if(_tmp3B2 <= (void*)4)goto _LL2BF;if(*((int*)_tmp3B2)!= 2)goto _LL2BF;_tmp3B3=((
struct Cyc_Absyn_DatatypeType_struct*)_tmp3B2)->f1;_tmp3B4=_tmp3B3.datatype_info;
if((((((struct Cyc_Absyn_DatatypeType_struct*)_tmp3B2)->f1).datatype_info).KnownDatatype).tag
!= 1)goto _LL2BF;_tmp3B5=(_tmp3B4.KnownDatatype).f1;_tmp3B6=*_tmp3B5;_tmp3B7=
_tmp3B3.targs;_tmp3B8=_tmp3B3.rgn;_LL2BE: if(_tmp3B6 == _tmp3AF)return 0;{int
_tmp42C=Cyc_Absyn_qvar_cmp(_tmp3B6->name,_tmp3AF->name);if(_tmp42C != 0)return
_tmp42C;if((unsigned int)_tmp3B8  && (unsigned int)_tmp3B1){int _tmp42D=Cyc_Tcutil_typecmp((
void*)_tmp3B8->v,(void*)_tmp3B1->v);if(_tmp42D != 0)return _tmp42D;}else{if((
unsigned int)_tmp3B8)return - 1;else{return 1;}}return Cyc_List_list_cmp(Cyc_Tcutil_typecmp,
_tmp3B7,_tmp3B0);}_LL2BF: _tmp3B9=_tmp394.f1;if(_tmp3B9 <= (void*)4)goto _LL2C1;if(*((
int*)_tmp3B9)!= 3)goto _LL2C1;_tmp3BA=((struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp3B9)->f1;_tmp3BB=_tmp3BA.field_info;if((((((struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp3B9)->f1).field_info).KnownDatatypefield).tag != 1)goto _LL2C1;_tmp3BC=(
_tmp3BB.KnownDatatypefield).f1;_tmp3BD=(_tmp3BB.KnownDatatypefield).f2;_tmp3BE=
_tmp3BA.targs;_tmp3BF=_tmp394.f2;if(_tmp3BF <= (void*)4)goto _LL2C1;if(*((int*)
_tmp3BF)!= 3)goto _LL2C1;_tmp3C0=((struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp3BF)->f1;_tmp3C1=_tmp3C0.field_info;if((((((struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp3BF)->f1).field_info).KnownDatatypefield).tag != 1)goto _LL2C1;_tmp3C2=(
_tmp3C1.KnownDatatypefield).f1;_tmp3C3=(_tmp3C1.KnownDatatypefield).f2;_tmp3C4=
_tmp3C0.targs;_LL2C0: if(_tmp3C2 == _tmp3BC)return 0;{int _tmp42E=Cyc_Absyn_qvar_cmp(
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
char*_tmpD48;void*_tmpD47;(_tmpD47=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpD48="missing expression in array index",
_tag_dyneither(_tmpD48,sizeof(char),34))),_tag_dyneither(_tmpD47,sizeof(void*),0)));}
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
_tmp401=_tmp3FA.rgn_po;_tmp402=_tmp3FA.attributes;_LL2CC: {const char*_tmpD4B;
void*_tmpD4A;(_tmpD4A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmpD4B="typecmp: function types not handled",_tag_dyneither(
_tmpD4B,sizeof(char),36))),_tag_dyneither(_tmpD4A,sizeof(void*),0)));}_LL2CD:
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
_LL2E4: {const char*_tmpD4E;void*_tmpD4D;(_tmpD4D=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpD4E="typecmp: effects not handled",
_tag_dyneither(_tmpD4E,sizeof(char),29))),_tag_dyneither(_tmpD4D,sizeof(void*),0)));}
_LL2E5:;_LL2E6: {const char*_tmpD51;void*_tmpD50;(_tmpD50=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpD51="Unmatched case in typecmp",
_tag_dyneither(_tmpD51,sizeof(char),26))),_tag_dyneither(_tmpD50,sizeof(void*),0)));}
_LL2B2:;}}}int Cyc_Tcutil_is_arithmetic_type(void*t);int Cyc_Tcutil_is_arithmetic_type(
void*t){void*_tmp443=Cyc_Tcutil_compress(t);_LL2ED: if(_tmp443 <= (void*)4)goto
_LL2EF;if(*((int*)_tmp443)!= 5)goto _LL2EF;_LL2EE: goto _LL2F0;_LL2EF: if((int)
_tmp443 != 1)goto _LL2F1;_LL2F0: goto _LL2F2;_LL2F1: if(_tmp443 <= (void*)4)goto _LL2F7;
if(*((int*)_tmp443)!= 6)goto _LL2F3;_LL2F2: goto _LL2F4;_LL2F3: if(*((int*)_tmp443)
!= 12)goto _LL2F5;_LL2F4: goto _LL2F6;_LL2F5: if(*((int*)_tmp443)!= 13)goto _LL2F7;
_LL2F6: return 1;_LL2F7:;_LL2F8: return 0;_LL2EC:;}int Cyc_Tcutil_will_lose_precision(
void*t1,void*t2);int Cyc_Tcutil_will_lose_precision(void*t1,void*t2){t1=Cyc_Tcutil_compress(
t1);t2=Cyc_Tcutil_compress(t2);{struct _tuple0 _tmpD52;struct _tuple0 _tmp445=(
_tmpD52.f1=t1,((_tmpD52.f2=t2,_tmpD52)));void*_tmp446;int _tmp447;void*_tmp448;
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
max_arith_type->v)){struct Cyc_Core_Opt*_tmpD53;max_arith_type=((_tmpD53=
_region_malloc(_tmp482,sizeof(*_tmpD53)),((_tmpD53->v=(void*)t1,_tmpD53))));}}}}
if(max_arith_type != 0){if(!Cyc_Tcutil_unify(t,(void*)max_arith_type->v))return 0;}}{
struct Cyc_List_List*el=es;for(0;el != 0;el=el->tl){if(!Cyc_Tcutil_coerce_assign(
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
t1);t2=Cyc_Tcutil_compress(t2);{struct _tuple0 _tmpD5F;struct _tuple0 _tmp48F=(
_tmpD5F.f1=t1,((_tmpD5F.f2=t2,_tmpD5F)));void*_tmp490;struct Cyc_Absyn_PtrInfo
_tmp491;void*_tmp492;struct Cyc_Absyn_PtrInfo _tmp493;void*_tmp494;struct Cyc_Absyn_DatatypeInfo
_tmp495;union Cyc_Absyn_DatatypeInfoU_union _tmp496;struct Cyc_Absyn_Datatypedecl**
_tmp497;struct Cyc_Absyn_Datatypedecl*_tmp498;struct Cyc_List_List*_tmp499;struct
Cyc_Core_Opt*_tmp49A;struct Cyc_Core_Opt _tmp49B;void*_tmp49C;void*_tmp49D;struct
Cyc_Absyn_DatatypeInfo _tmp49E;union Cyc_Absyn_DatatypeInfoU_union _tmp49F;struct
Cyc_Absyn_Datatypedecl**_tmp4A0;struct Cyc_Absyn_Datatypedecl*_tmp4A1;struct Cyc_List_List*
_tmp4A2;struct Cyc_Core_Opt*_tmp4A3;struct Cyc_Core_Opt _tmp4A4;void*_tmp4A5;void*
_tmp4A6;struct Cyc_Absyn_ArrayInfo _tmp4A7;void*_tmp4A8;struct Cyc_Absyn_Tqual
_tmp4A9;struct Cyc_Absyn_Exp*_tmp4AA;struct Cyc_Absyn_Conref*_tmp4AB;void*_tmp4AC;
struct Cyc_Absyn_ArrayInfo _tmp4AD;void*_tmp4AE;struct Cyc_Absyn_Tqual _tmp4AF;
struct Cyc_Absyn_Exp*_tmp4B0;struct Cyc_Absyn_Conref*_tmp4B1;void*_tmp4B2;struct
Cyc_Absyn_DatatypeFieldInfo _tmp4B3;union Cyc_Absyn_DatatypeFieldInfoU_union
_tmp4B4;struct Cyc_Absyn_Datatypedecl*_tmp4B5;struct Cyc_Absyn_Datatypefield*
_tmp4B6;struct Cyc_List_List*_tmp4B7;void*_tmp4B8;struct Cyc_Absyn_DatatypeInfo
_tmp4B9;union Cyc_Absyn_DatatypeInfoU_union _tmp4BA;struct Cyc_Absyn_Datatypedecl**
_tmp4BB;struct Cyc_Absyn_Datatypedecl*_tmp4BC;struct Cyc_List_List*_tmp4BD;void*
_tmp4BE;struct Cyc_Absyn_PtrInfo _tmp4BF;void*_tmp4C0;struct Cyc_Absyn_Tqual _tmp4C1;
struct Cyc_Absyn_PtrAtts _tmp4C2;void*_tmp4C3;struct Cyc_Absyn_Conref*_tmp4C4;
struct Cyc_Absyn_Conref*_tmp4C5;struct Cyc_Absyn_Conref*_tmp4C6;void*_tmp4C7;
struct Cyc_Absyn_DatatypeInfo _tmp4C8;union Cyc_Absyn_DatatypeInfoU_union _tmp4C9;
struct Cyc_Absyn_Datatypedecl**_tmp4CA;struct Cyc_Absyn_Datatypedecl*_tmp4CB;
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
_LL343: okay=!_tmp4D1;goto _LL341;_LL344:;_LL345: {const char*_tmpD62;void*_tmpD61;(
_tmpD61=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmpD62="silent_castable conref not eq",_tag_dyneither(_tmpD62,sizeof(char),30))),
_tag_dyneither(_tmpD61,sizeof(void*),0)));}_LL341:;}if(!Cyc_Tcutil_unify_conrefs(
Cyc_Tcutil_unify_it_bounds,(_tmp491.ptr_atts).bounds,(_tmp493.ptr_atts).bounds)){
struct _tuple13 _tmpD63;struct _tuple13 _tmp4D5=(_tmpD63.f1=(Cyc_Absyn_compress_conref((
_tmp491.ptr_atts).bounds))->v,((_tmpD63.f2=(Cyc_Absyn_compress_conref((_tmp493.ptr_atts).bounds))->v,
_tmpD63)));union Cyc_Absyn_Constraint_union _tmp4D6;void*_tmp4D7;union Cyc_Absyn_Constraint_union
_tmp4D8;void*_tmp4D9;union Cyc_Absyn_Constraint_union _tmp4DA;_LL347: _tmp4D6=
_tmp4D5.f1;if(((_tmp4D5.f1).Eq_constr).tag != 0)goto _LL349;_tmp4D7=(_tmp4D6.Eq_constr).f1;
_tmp4D8=_tmp4D5.f2;if(((_tmp4D5.f2).Eq_constr).tag != 0)goto _LL349;_tmp4D9=(
_tmp4D8.Eq_constr).f1;_LL348:{struct _tuple0 _tmpD64;struct _tuple0 _tmp4DC=(_tmpD64.f1=
_tmp4D7,((_tmpD64.f2=_tmp4D9,_tmpD64)));void*_tmp4DD;void*_tmp4DE;void*_tmp4DF;
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
const char*_tmpD67;void*_tmpD66;(_tmpD66=0,Cyc_Tcutil_warn(loc,((_tmpD67="implicit cast to shorter array",
_tag_dyneither(_tmpD67,sizeof(char),31))),_tag_dyneither(_tmpD66,sizeof(void*),0)));}
goto _LL34D;_LL354: _tmp4E5=_tmp4DC.f1;if((int)_tmp4E5 != 0)goto _LL34D;_tmp4E6=
_tmp4DC.f2;if(_tmp4E6 <= (void*)1)goto _LL34D;if(*((int*)_tmp4E6)!= 0)goto _LL34D;
_tmp4E7=((struct Cyc_Absyn_Upper_b_struct*)_tmp4E6)->f1;_LL355: if(((int(*)(int,
struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,(_tmp491.ptr_atts).zero_term)
 && Cyc_Tcutil_is_bound_one((_tmp493.ptr_atts).bounds))goto _LL34D;okay=0;goto
_LL34D;_LL34D:;}goto _LL346;_LL349: _tmp4DA=_tmp4D5.f1;if(((_tmp4D5.f1).No_constr).tag
!= 2)goto _LL34B;_LL34A: okay=0;goto _LL346;_LL34B:;_LL34C: {const char*_tmpD6A;void*
_tmpD69;(_tmpD69=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmpD6A="silent_castable conrefs didn't unify",_tag_dyneither(_tmpD6A,sizeof(
char),37))),_tag_dyneither(_tmpD69,sizeof(void*),0)));}_LL346:;}okay=okay  && Cyc_Tcutil_unify((
void*)_tmp491.elt_typ,(void*)_tmp493.elt_typ);okay=okay  && (Cyc_Tcutil_unify((
void*)(_tmp491.ptr_atts).rgn,(void*)(_tmp493.ptr_atts).rgn) || Cyc_Tcenv_region_outlives(
te,(void*)(_tmp491.ptr_atts).rgn,(void*)(_tmp493.ptr_atts).rgn));okay=okay  && (!(
_tmp491.elt_tq).real_const  || (_tmp493.elt_tq).real_const);okay=okay  && (((int(*)(
int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(
Cyc_Core_intcmp,(_tmp491.ptr_atts).zero_term,(_tmp493.ptr_atts).zero_term) || ((
int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(1,(_tmp491.ptr_atts).zero_term)
 && (_tmp493.elt_tq).real_const);return okay;}_LL335: _tmp494=_tmp48F.f1;if(
_tmp494 <= (void*)4)goto _LL337;if(*((int*)_tmp494)!= 2)goto _LL337;_tmp495=((
struct Cyc_Absyn_DatatypeType_struct*)_tmp494)->f1;_tmp496=_tmp495.datatype_info;
if((((((struct Cyc_Absyn_DatatypeType_struct*)_tmp494)->f1).datatype_info).KnownDatatype).tag
!= 1)goto _LL337;_tmp497=(_tmp496.KnownDatatype).f1;_tmp498=*_tmp497;_tmp499=
_tmp495.targs;_tmp49A=_tmp495.rgn;if(_tmp49A == 0)goto _LL337;_tmp49B=*_tmp49A;
_tmp49C=(void*)_tmp49B.v;_tmp49D=_tmp48F.f2;if(_tmp49D <= (void*)4)goto _LL337;if(*((
int*)_tmp49D)!= 2)goto _LL337;_tmp49E=((struct Cyc_Absyn_DatatypeType_struct*)
_tmp49D)->f1;_tmp49F=_tmp49E.datatype_info;if((((((struct Cyc_Absyn_DatatypeType_struct*)
_tmp49D)->f1).datatype_info).KnownDatatype).tag != 1)goto _LL337;_tmp4A0=(_tmp49F.KnownDatatype).f1;
_tmp4A1=*_tmp4A0;_tmp4A2=_tmp49E.targs;_tmp4A3=_tmp49E.rgn;if(_tmp4A3 == 0)goto
_LL337;_tmp4A4=*_tmp4A3;_tmp4A5=(void*)_tmp4A4.v;_LL336: if(_tmp498 != _tmp4A1  || 
!Cyc_Tcenv_region_outlives(te,_tmp49C,_tmp4A5))return 0;for(0;_tmp499 != 0  && 
_tmp4A2 != 0;(_tmp499=_tmp499->tl,_tmp4A2=_tmp4A2->tl)){if(!Cyc_Tcutil_unify((
void*)_tmp499->hd,(void*)_tmp4A2->hd))return 0;}if(_tmp499 != 0  || _tmp4A2 != 0)
return 0;return 1;_LL337: _tmp4A6=_tmp48F.f1;if(_tmp4A6 <= (void*)4)goto _LL339;if(*((
int*)_tmp4A6)!= 7)goto _LL339;_tmp4A7=((struct Cyc_Absyn_ArrayType_struct*)_tmp4A6)->f1;
_tmp4A8=(void*)_tmp4A7.elt_type;_tmp4A9=_tmp4A7.tq;_tmp4AA=_tmp4A7.num_elts;
_tmp4AB=_tmp4A7.zero_term;_tmp4AC=_tmp48F.f2;if(_tmp4AC <= (void*)4)goto _LL339;
if(*((int*)_tmp4AC)!= 7)goto _LL339;_tmp4AD=((struct Cyc_Absyn_ArrayType_struct*)
_tmp4AC)->f1;_tmp4AE=(void*)_tmp4AD.elt_type;_tmp4AF=_tmp4AD.tq;_tmp4B0=_tmp4AD.num_elts;
_tmp4B1=_tmp4AD.zero_term;_LL338: {int okay;okay=((int(*)(int(*cmp)(int,int),
struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,
_tmp4AB,_tmp4B1) && ((_tmp4AA != 0  && _tmp4B0 != 0) && Cyc_Evexp_same_const_exp((
struct Cyc_Absyn_Exp*)_tmp4AA,(struct Cyc_Absyn_Exp*)_tmp4B0));return(okay  && Cyc_Tcutil_unify(
_tmp4A8,_tmp4AE)) && (!_tmp4A9.real_const  || _tmp4AF.real_const);}_LL339: _tmp4B2=
_tmp48F.f1;if(_tmp4B2 <= (void*)4)goto _LL33B;if(*((int*)_tmp4B2)!= 3)goto _LL33B;
_tmp4B3=((struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp4B2)->f1;_tmp4B4=_tmp4B3.field_info;
if((((((struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp4B2)->f1).field_info).KnownDatatypefield).tag
!= 1)goto _LL33B;_tmp4B5=(_tmp4B4.KnownDatatypefield).f1;_tmp4B6=(_tmp4B4.KnownDatatypefield).f2;
_tmp4B7=_tmp4B3.targs;_tmp4B8=_tmp48F.f2;if(_tmp4B8 <= (void*)4)goto _LL33B;if(*((
int*)_tmp4B8)!= 2)goto _LL33B;_tmp4B9=((struct Cyc_Absyn_DatatypeType_struct*)
_tmp4B8)->f1;_tmp4BA=_tmp4B9.datatype_info;if((((((struct Cyc_Absyn_DatatypeType_struct*)
_tmp4B8)->f1).datatype_info).KnownDatatype).tag != 1)goto _LL33B;_tmp4BB=(_tmp4BA.KnownDatatype).f1;
_tmp4BC=*_tmp4BB;_tmp4BD=_tmp4B9.targs;_LL33A: if((_tmp4B5 == _tmp4BC  || Cyc_Absyn_qvar_cmp(
_tmp4B5->name,_tmp4BC->name)== 0) && (_tmp4B6->typs == 0  || _tmp4B5->is_flat)){
for(0;_tmp4B7 != 0  && _tmp4BD != 0;(_tmp4B7=_tmp4B7->tl,_tmp4BD=_tmp4BD->tl)){if(!
Cyc_Tcutil_unify((void*)_tmp4B7->hd,(void*)_tmp4BD->hd))break;}if(_tmp4B7 == 0
 && _tmp4BD == 0)return 1;}return 0;_LL33B: _tmp4BE=_tmp48F.f1;if(_tmp4BE <= (void*)4)
goto _LL33D;if(*((int*)_tmp4BE)!= 4)goto _LL33D;_tmp4BF=((struct Cyc_Absyn_PointerType_struct*)
_tmp4BE)->f1;_tmp4C0=(void*)_tmp4BF.elt_typ;_tmp4C1=_tmp4BF.elt_tq;_tmp4C2=
_tmp4BF.ptr_atts;_tmp4C3=(void*)_tmp4C2.rgn;_tmp4C4=_tmp4C2.nullable;_tmp4C5=
_tmp4C2.bounds;_tmp4C6=_tmp4C2.zero_term;_tmp4C7=_tmp48F.f2;if(_tmp4C7 <= (void*)
4)goto _LL33D;if(*((int*)_tmp4C7)!= 2)goto _LL33D;_tmp4C8=((struct Cyc_Absyn_DatatypeType_struct*)
_tmp4C7)->f1;_tmp4C9=_tmp4C8.datatype_info;if((((((struct Cyc_Absyn_DatatypeType_struct*)
_tmp4C7)->f1).datatype_info).KnownDatatype).tag != 1)goto _LL33D;_tmp4CA=(_tmp4C9.KnownDatatype).f1;
_tmp4CB=*_tmp4CA;_tmp4CC=_tmp4C8.targs;_tmp4CD=_tmp4C8.rgn;if(!(!_tmp4CB->is_flat))
goto _LL33D;_LL33C:{void*_tmp4EC=Cyc_Tcutil_compress(_tmp4C0);struct Cyc_Absyn_DatatypeFieldInfo
_tmp4ED;union Cyc_Absyn_DatatypeFieldInfoU_union _tmp4EE;struct Cyc_Absyn_Datatypedecl*
_tmp4EF;struct Cyc_Absyn_Datatypefield*_tmp4F0;struct Cyc_List_List*_tmp4F1;_LL357:
if(_tmp4EC <= (void*)4)goto _LL359;if(*((int*)_tmp4EC)!= 3)goto _LL359;_tmp4ED=((
struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp4EC)->f1;_tmp4EE=_tmp4ED.field_info;
if((((((struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp4EC)->f1).field_info).KnownDatatypefield).tag
!= 1)goto _LL359;_tmp4EF=(_tmp4EE.KnownDatatypefield).f1;_tmp4F0=(_tmp4EE.KnownDatatypefield).f2;
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
t2) && Cyc_Tcutil_is_zero(e1)){{struct Cyc_Absyn_Const_e_struct _tmpD70;union Cyc_Absyn_Cnst_union
_tmpD6F;struct Cyc_Absyn_Const_e_struct*_tmpD6E;(void*)(e1->r=(void*)((void*)((
_tmpD6E=_cycalloc(sizeof(*_tmpD6E)),((_tmpD6E[0]=((_tmpD70.tag=0,((_tmpD70.f1=(
union Cyc_Absyn_Cnst_union)(((_tmpD6F.Null_c).tag=6,_tmpD6F)),_tmpD70)))),_tmpD6E))))));}{
struct Cyc_Core_Opt*_tmp513=Cyc_Tcenv_lookup_opt_type_vars(te);struct Cyc_Absyn_PointerType_struct
_tmpD7A;struct Cyc_Absyn_PtrAtts _tmpD79;struct Cyc_Absyn_PtrInfo _tmpD78;struct Cyc_Absyn_PointerType_struct*
_tmpD77;struct Cyc_Absyn_PointerType_struct*_tmp514=(_tmpD77=_cycalloc(sizeof(*
_tmpD77)),((_tmpD77[0]=((_tmpD7A.tag=4,((_tmpD7A.f1=((_tmpD78.elt_typ=(void*)Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_ak,_tmp513),((_tmpD78.elt_tq=Cyc_Absyn_empty_tqual(
0),((_tmpD78.ptr_atts=((_tmpD79.rgn=(void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_trk,_tmp513),((_tmpD79.nullable=Cyc_Absyn_true_conref,((_tmpD79.bounds=
Cyc_Absyn_empty_conref(),((_tmpD79.zero_term=((struct Cyc_Absyn_Conref*(*)())Cyc_Absyn_empty_conref)(),((
_tmpD79.ptrloc=0,_tmpD79)))))))))),_tmpD78)))))),_tmpD7A)))),_tmpD77)));(void*)(((
struct Cyc_Core_Opt*)_check_null(e1->topt))->v=(void*)((void*)_tmp514));return Cyc_Tcutil_coerce_arg(
te,e1,t2);}}return 0;}struct _dyneither_ptr Cyc_Tcutil_coercion2string(void*c);
struct _dyneither_ptr Cyc_Tcutil_coercion2string(void*c){void*_tmp519=c;_LL39A: if((
int)_tmp519 != 0)goto _LL39C;_LL39B: {const char*_tmpD7B;return(_tmpD7B="unknown",
_tag_dyneither(_tmpD7B,sizeof(char),8));}_LL39C: if((int)_tmp519 != 1)goto _LL39E;
_LL39D: {const char*_tmpD7C;return(_tmpD7C="no coercion",_tag_dyneither(_tmpD7C,
sizeof(char),12));}_LL39E: if((int)_tmp519 != 2)goto _LL3A0;_LL39F: {const char*
_tmpD7D;return(_tmpD7D="null check",_tag_dyneither(_tmpD7D,sizeof(char),11));}
_LL3A0: if((int)_tmp519 != 3)goto _LL399;_LL3A1: {const char*_tmpD7E;return(_tmpD7E="other coercion",
_tag_dyneither(_tmpD7E,sizeof(char),15));}_LL399:;}int Cyc_Tcutil_coerce_arg(
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
void*t);int Cyc_Tcutil_coerceable(void*t){void*_tmp526=Cyc_Tcutil_compress(t);
_LL3A3: if(_tmp526 <= (void*)4)goto _LL3A5;if(*((int*)_tmp526)!= 5)goto _LL3A5;
_LL3A4: goto _LL3A6;_LL3A5: if((int)_tmp526 != 1)goto _LL3A7;_LL3A6: goto _LL3A8;_LL3A7:
if(_tmp526 <= (void*)4)goto _LL3A9;if(*((int*)_tmp526)!= 6)goto _LL3A9;_LL3A8:
return 1;_LL3A9:;_LL3AA: return 0;_LL3A2:;}static struct _tuple5*Cyc_Tcutil_flatten_typ_f(
struct _tuple9*env,struct Cyc_Absyn_Aggrfield*x);static struct _tuple5*Cyc_Tcutil_flatten_typ_f(
struct _tuple9*env,struct Cyc_Absyn_Aggrfield*x){struct Cyc_List_List*_tmp528;
struct _RegionHandle*_tmp529;struct _tuple9 _tmp527=*env;_tmp528=_tmp527.f1;_tmp529=
_tmp527.f2;{struct _tuple5*_tmpD89;return(_tmpD89=_region_malloc(_tmp529,sizeof(*
_tmpD89)),((_tmpD89->f1=x->tq,((_tmpD89->f2=Cyc_Tcutil_rsubstitute(_tmp529,
_tmp528,(void*)x->type),_tmpD89)))));}}static struct _tuple5*Cyc_Tcutil_rcopy_tqt(
struct _RegionHandle*r,struct _tuple5*x);static struct _tuple5*Cyc_Tcutil_rcopy_tqt(
struct _RegionHandle*r,struct _tuple5*x){struct Cyc_Absyn_Tqual _tmp52C;void*_tmp52D;
struct _tuple5 _tmp52B=*x;_tmp52C=_tmp52B.f1;_tmp52D=_tmp52B.f2;{struct _tuple5*
_tmpD8A;return(_tmpD8A=_region_malloc(r,sizeof(*_tmpD8A)),((_tmpD8A->f1=_tmp52C,((
_tmpD8A->f2=_tmp52D,_tmpD8A)))));}}static struct Cyc_List_List*Cyc_Tcutil_flatten_typ(
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
_check_null(_tmp534->impl))->rgn_po != 0){struct _tuple5*_tmpD8D;struct Cyc_List_List*
_tmpD8C;return(_tmpD8C=_region_malloc(r,sizeof(*_tmpD8C)),((_tmpD8C->hd=((
_tmpD8D=_region_malloc(r,sizeof(*_tmpD8D)),((_tmpD8D->f1=Cyc_Absyn_empty_tqual(0),((
_tmpD8D->f2=t1,_tmpD8D)))))),((_tmpD8C->tl=0,_tmpD8C)))));}{struct Cyc_List_List*
_tmp53B=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp534->tvs,
_tmp535);struct _tuple9 _tmpD8E;struct _tuple9 env=(_tmpD8E.f1=_tmp53B,((_tmpD8E.f2=
r,_tmpD8E)));return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple5*(*
f)(struct _tuple9*,struct Cyc_Absyn_Aggrfield*),struct _tuple9*env,struct Cyc_List_List*
x))Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_typ_f,& env,((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp534->impl))->fields);}_LL3B2: if(*((int*)_tmp52F)!= 11)goto _LL3B4;
_tmp536=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)_tmp52F)->f1;if((int)
_tmp536 != 0)goto _LL3B4;_tmp537=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp52F)->f2;
_LL3B3: {struct _tuple9 _tmpD8F;struct _tuple9 env=(_tmpD8F.f1=0,((_tmpD8F.f2=r,
_tmpD8F)));return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple5*(*
f)(struct _tuple9*,struct Cyc_Absyn_Aggrfield*),struct _tuple9*env,struct Cyc_List_List*
x))Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_typ_f,& env,_tmp537);}_LL3B4: if(*((int*)
_tmp52F)!= 8)goto _LL3B6;_tmp538=((struct Cyc_Absyn_FnType_struct*)_tmp52F)->f1;
_LL3B5: {struct _tuple5*_tmpD92;struct Cyc_List_List*_tmpD91;return(_tmpD91=
_region_malloc(r,sizeof(*_tmpD91)),((_tmpD91->hd=((_tmpD92=_region_malloc(r,
sizeof(*_tmpD92)),((_tmpD92->f1=Cyc_Absyn_const_tqual(0),((_tmpD92->f2=t1,
_tmpD92)))))),((_tmpD91->tl=0,_tmpD91)))));}_LL3B6:;_LL3B7: {struct _tuple5*
_tmpD95;struct Cyc_List_List*_tmpD94;return(_tmpD94=_region_malloc(r,sizeof(*
_tmpD94)),((_tmpD94->hd=((_tmpD95=_region_malloc(r,sizeof(*_tmpD95)),((_tmpD95->f1=
Cyc_Absyn_empty_tqual(0),((_tmpD95->f2=t1,_tmpD95)))))),((_tmpD94->tl=0,_tmpD94)))));}
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
_tmpD96;struct _tuple0 _tmp544=(_tmpD96.f1=t1,((_tmpD96.f2=t2,_tmpD96)));void*
_tmp545;struct Cyc_Absyn_PtrInfo _tmp546;void*_tmp547;struct Cyc_Absyn_Tqual _tmp548;
struct Cyc_Absyn_PtrAtts _tmp549;void*_tmp54A;struct Cyc_Absyn_Conref*_tmp54B;
struct Cyc_Absyn_Conref*_tmp54C;struct Cyc_Absyn_Conref*_tmp54D;void*_tmp54E;
struct Cyc_Absyn_PtrInfo _tmp54F;void*_tmp550;struct Cyc_Absyn_Tqual _tmp551;struct
Cyc_Absyn_PtrAtts _tmp552;void*_tmp553;struct Cyc_Absyn_Conref*_tmp554;struct Cyc_Absyn_Conref*
_tmp555;struct Cyc_Absyn_Conref*_tmp556;void*_tmp557;struct Cyc_Absyn_DatatypeInfo
_tmp558;union Cyc_Absyn_DatatypeInfoU_union _tmp559;struct Cyc_Absyn_Datatypedecl**
_tmp55A;struct Cyc_Absyn_Datatypedecl*_tmp55B;struct Cyc_List_List*_tmp55C;struct
Cyc_Core_Opt*_tmp55D;struct Cyc_Core_Opt _tmp55E;void*_tmp55F;void*_tmp560;struct
Cyc_Absyn_DatatypeInfo _tmp561;union Cyc_Absyn_DatatypeInfoU_union _tmp562;struct
Cyc_Absyn_Datatypedecl**_tmp563;struct Cyc_Absyn_Datatypedecl*_tmp564;struct Cyc_List_List*
_tmp565;struct Cyc_Core_Opt*_tmp566;struct Cyc_Core_Opt _tmp567;void*_tmp568;void*
_tmp569;struct Cyc_Absyn_FnInfo _tmp56A;void*_tmp56B;struct Cyc_Absyn_FnInfo _tmp56C;
_LL3C2: _tmp545=_tmp544.f1;if(_tmp545 <= (void*)4)goto _LL3C4;if(*((int*)_tmp545)!= 
4)goto _LL3C4;_tmp546=((struct Cyc_Absyn_PointerType_struct*)_tmp545)->f1;_tmp547=(
void*)_tmp546.elt_typ;_tmp548=_tmp546.elt_tq;_tmp549=_tmp546.ptr_atts;_tmp54A=(
void*)_tmp549.rgn;_tmp54B=_tmp549.nullable;_tmp54C=_tmp549.bounds;_tmp54D=
_tmp549.zero_term;_tmp54E=_tmp544.f2;if(_tmp54E <= (void*)4)goto _LL3C4;if(*((int*)
_tmp54E)!= 4)goto _LL3C4;_tmp54F=((struct Cyc_Absyn_PointerType_struct*)_tmp54E)->f1;
_tmp550=(void*)_tmp54F.elt_typ;_tmp551=_tmp54F.elt_tq;_tmp552=_tmp54F.ptr_atts;
_tmp553=(void*)_tmp552.rgn;_tmp554=_tmp552.nullable;_tmp555=_tmp552.bounds;
_tmp556=_tmp552.zero_term;_LL3C3: if(_tmp548.real_const  && !_tmp551.real_const)
return 0;if((!((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*
y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp54B,_tmp554) && ((int(*)(int,
struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp54B)) && !((int(*)(int,
struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp554))return 0;if((!((int(*)(
int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(
Cyc_Core_intcmp,_tmp54D,_tmp556) && !((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,_tmp54D)) && ((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,
_tmp556))return 0;if(!Cyc_Tcutil_unify(_tmp54A,_tmp553) && !Cyc_Tcenv_region_outlives(
te,_tmp54A,_tmp553))return 0;if(!Cyc_Tcutil_unify_conrefs(Cyc_Tcutil_unify_it_bounds,
_tmp54C,_tmp555)){struct _tuple0 _tmpD97;struct _tuple0 _tmp56E=(_tmpD97.f1=Cyc_Absyn_conref_val(
_tmp54C),((_tmpD97.f2=Cyc_Absyn_conref_val(_tmp555),_tmpD97)));void*_tmp56F;void*
_tmp570;void*_tmp571;struct Cyc_Absyn_Exp*_tmp572;void*_tmp573;struct Cyc_Absyn_Exp*
_tmp574;_LL3CB: _tmp56F=_tmp56E.f1;if(_tmp56F <= (void*)1)goto _LL3CD;if(*((int*)
_tmp56F)!= 0)goto _LL3CD;_tmp570=_tmp56E.f2;if((int)_tmp570 != 0)goto _LL3CD;_LL3CC:
goto _LL3CA;_LL3CD: _tmp571=_tmp56E.f1;if(_tmp571 <= (void*)1)goto _LL3CF;if(*((int*)
_tmp571)!= 0)goto _LL3CF;_tmp572=((struct Cyc_Absyn_Upper_b_struct*)_tmp571)->f1;
_tmp573=_tmp56E.f2;if(_tmp573 <= (void*)1)goto _LL3CF;if(*((int*)_tmp573)!= 0)goto
_LL3CF;_tmp574=((struct Cyc_Absyn_Upper_b_struct*)_tmp573)->f1;_LL3CE: if(!Cyc_Evexp_lte_const_exp(
_tmp574,_tmp572))return 0;goto _LL3CA;_LL3CF:;_LL3D0: return 0;_LL3CA:;}{struct
_tuple0*_tmpD9A;struct Cyc_List_List*_tmpD99;return Cyc_Tcutil_ptrsubtype(te,((
_tmpD99=_cycalloc(sizeof(*_tmpD99)),((_tmpD99->hd=((_tmpD9A=_cycalloc(sizeof(*
_tmpD9A)),((_tmpD9A->f1=t1,((_tmpD9A->f2=t2,_tmpD9A)))))),((_tmpD99->tl=assume,
_tmpD99)))))),_tmp547,_tmp550);}_LL3C4: _tmp557=_tmp544.f1;if(_tmp557 <= (void*)4)
goto _LL3C6;if(*((int*)_tmp557)!= 2)goto _LL3C6;_tmp558=((struct Cyc_Absyn_DatatypeType_struct*)
_tmp557)->f1;_tmp559=_tmp558.datatype_info;if((((((struct Cyc_Absyn_DatatypeType_struct*)
_tmp557)->f1).datatype_info).KnownDatatype).tag != 1)goto _LL3C6;_tmp55A=(_tmp559.KnownDatatype).f1;
_tmp55B=*_tmp55A;_tmp55C=_tmp558.targs;_tmp55D=_tmp558.rgn;if(_tmp55D == 0)goto
_LL3C6;_tmp55E=*_tmp55D;_tmp55F=(void*)_tmp55E.v;_tmp560=_tmp544.f2;if(_tmp560 <= (
void*)4)goto _LL3C6;if(*((int*)_tmp560)!= 2)goto _LL3C6;_tmp561=((struct Cyc_Absyn_DatatypeType_struct*)
_tmp560)->f1;_tmp562=_tmp561.datatype_info;if((((((struct Cyc_Absyn_DatatypeType_struct*)
_tmp560)->f1).datatype_info).KnownDatatype).tag != 1)goto _LL3C6;_tmp563=(_tmp562.KnownDatatype).f1;
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
_tmp578))->hd))return 0;{struct _tuple8*_tmpDA4;struct Cyc_Absyn_VarType_struct
_tmpDA3;struct Cyc_Absyn_VarType_struct*_tmpDA2;struct Cyc_List_List*_tmpDA1;inst=((
_tmpDA1=_region_malloc(_tmp579,sizeof(*_tmpDA1)),((_tmpDA1->hd=((_tmpDA4=
_region_malloc(_tmp579,sizeof(*_tmpDA4)),((_tmpDA4->f1=(struct Cyc_Absyn_Tvar*)
_tmp578->hd,((_tmpDA4->f2=(void*)((_tmpDA2=_cycalloc(sizeof(*_tmpDA2)),((_tmpDA2[
0]=((_tmpDA3.tag=1,((_tmpDA3.f1=(struct Cyc_Absyn_Tvar*)_tmp577->hd,_tmpDA3)))),
_tmpDA2)))),_tmpDA4)))))),((_tmpDA1->tl=inst,_tmpDA1))))));}_tmp577=_tmp577->tl;
_tmp578=_tmp578->tl;}if(inst != 0){_tmp56A.tvars=0;_tmp56C.tvars=0;{struct Cyc_Absyn_FnType_struct
_tmpDAA;struct Cyc_Absyn_FnType_struct*_tmpDA9;struct Cyc_Absyn_FnType_struct
_tmpDA7;struct Cyc_Absyn_FnType_struct*_tmpDA6;return Cyc_Tcutil_subtype(te,assume,(
void*)((_tmpDA6=_cycalloc(sizeof(*_tmpDA6)),((_tmpDA6[0]=((_tmpDA7.tag=8,((
_tmpDA7.f1=_tmp56A,_tmpDA7)))),_tmpDA6)))),(void*)((_tmpDA9=_cycalloc(sizeof(*
_tmpDA9)),((_tmpDA9[0]=((_tmpDAA.tag=8,((_tmpDAA.f1=_tmp56C,_tmpDAA)))),_tmpDA9)))));}}}}
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
void*t2){struct _tuple0 _tmpDAB;struct _tuple0 _tmp58D=(_tmpDAB.f1=Cyc_Tcutil_compress(
t1),((_tmpDAB.f2=Cyc_Tcutil_compress(t2),_tmpDAB)));void*_tmp58E;void*_tmp58F;
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
_tmp5B8=_tmp5B4.zero_term;_LL3F5: {void*coercion=(void*)3;struct _tuple0*_tmpDAE;
struct Cyc_List_List*_tmpDAD;struct Cyc_List_List*_tmp5B9=(_tmpDAD=_cycalloc(
sizeof(*_tmpDAD)),((_tmpDAD->hd=((_tmpDAE=_cycalloc(sizeof(*_tmpDAE)),((_tmpDAE->f1=
t1,((_tmpDAE->f2=t2,_tmpDAE)))))),((_tmpDAD->tl=0,_tmpDAD)))));int _tmp5BA=Cyc_Tcutil_ptrsubtype(
te,_tmp5B9,_tmp5A2,_tmp5B2) && (!_tmp5A3.real_const  || _tmp5B3.real_const);Cyc_Tcutil_t1_failure=
t1;Cyc_Tcutil_t2_failure=t2;{int zeroterm_ok=((int(*)(int(*cmp)(int,int),struct
Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,
_tmp5A8,_tmp5B8) || !((int(*)(struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_val)(
_tmp5B8);int _tmp5BB=_tmp5BA?0:((Cyc_Tcutil_bits_only(_tmp5A2) && Cyc_Tcutil_is_char_type(
_tmp5B2)) && !((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,
_tmp5B8)) && (_tmp5B3.real_const  || !_tmp5A3.real_const);int bounds_ok=Cyc_Tcutil_unify_conrefs(
Cyc_Tcutil_unify_it_bounds,_tmp5A7,_tmp5B7);if(!bounds_ok  && !_tmp5BB){struct
_tuple0 _tmpDAF;struct _tuple0 _tmp5BD=(_tmpDAF.f1=Cyc_Absyn_conref_val(_tmp5A7),((
_tmpDAF.f2=Cyc_Absyn_conref_val(_tmp5B7),_tmpDAF)));void*_tmp5BE;struct Cyc_Absyn_Exp*
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
struct Cyc_Absyn_Cast_e_struct _tmpDB2;struct Cyc_Absyn_Cast_e_struct*_tmpDB1;(void*)(
e->r=(void*)((void*)((_tmpDB1=_cycalloc(sizeof(*_tmpDB1)),((_tmpDB1[0]=((_tmpDB2.tag=
15,((_tmpDB2.f1=(void*)t,((_tmpDB2.f2=_tmp5CE,((_tmpDB2.f3=0,((_tmpDB2.f4=(void*)
c,_tmpDB2)))))))))),_tmpDB1))))));}{struct Cyc_Core_Opt*_tmpDB3;e->topt=((_tmpDB3=
_cycalloc(sizeof(*_tmpDB3)),((_tmpDB3->v=(void*)t,_tmpDB3))));}}}int Cyc_Tcutil_is_integral(
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
_tuple0 _tmpDB4;struct _tuple0 _tmp5D6=(_tmpDB4.f1=t1,((_tmpDB4.f2=t2,_tmpDB4)));
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
_tmp5FA != 0)goto _LL449;_LL448:{const char*_tmpDB7;void*_tmpDB6;(_tmpDB6=0,Cyc_Tcutil_warn(
e->loc,((_tmpDB7="assignment in test",_tag_dyneither(_tmpDB7,sizeof(char),19))),
_tag_dyneither(_tmpDB6,sizeof(void*),0)));}goto _LL446;_LL449:;_LL44A: goto _LL446;
_LL446:;}static int Cyc_Tcutil_constrain_kinds(void*c1,void*c2);static int Cyc_Tcutil_constrain_kinds(
void*c1,void*c2){c1=Cyc_Absyn_compress_kb(c1);c2=Cyc_Absyn_compress_kb(c2);{
struct _tuple0 _tmpDB8;struct _tuple0 _tmp5FE=(_tmpDB8.f1=c1,((_tmpDB8.f2=c2,_tmpDB8)));
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
struct Cyc_Core_Opt*_tmpDB9;*_tmp605=((_tmpDB9=_cycalloc(sizeof(*_tmpDB9)),((
_tmpDB9->v=(void*)c1,_tmpDB9))));}return 1;_LL450: _tmp606=_tmp5FE.f1;if(*((int*)
_tmp606)!= 1)goto _LL452;_tmp607=((struct Cyc_Absyn_Unknown_kb_struct*)_tmp606)->f1;
_tmp608=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)_tmp606)->f1;
_LL451:{struct Cyc_Core_Opt*_tmpDBA;*_tmp608=((_tmpDBA=_cycalloc(sizeof(*_tmpDBA)),((
_tmpDBA->v=(void*)c2,_tmpDBA))));}return 1;_LL452: _tmp609=_tmp5FE.f1;if(*((int*)
_tmp609)!= 2)goto _LL454;_tmp60A=((struct Cyc_Absyn_Less_kb_struct*)_tmp609)->f1;
_tmp60B=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)_tmp609)->f1;
_tmp60C=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp609)->f2;_tmp60D=_tmp5FE.f2;
if(*((int*)_tmp60D)!= 0)goto _LL454;_tmp60E=(void*)((struct Cyc_Absyn_Eq_kb_struct*)
_tmp60D)->f1;_LL453: if(Cyc_Tcutil_kind_leq(_tmp60E,_tmp60C)){{struct Cyc_Core_Opt*
_tmpDBB;*_tmp60B=((_tmpDBB=_cycalloc(sizeof(*_tmpDBB)),((_tmpDBB->v=(void*)c2,
_tmpDBB))));}return 1;}else{return 0;}_LL454: _tmp60F=_tmp5FE.f1;if(*((int*)_tmp60F)
!= 0)goto _LL456;_tmp610=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp60F)->f1;
_tmp611=_tmp5FE.f2;if(*((int*)_tmp611)!= 2)goto _LL456;_tmp612=((struct Cyc_Absyn_Less_kb_struct*)
_tmp611)->f1;_tmp613=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp611)->f1;_tmp614=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp611)->f2;
_LL455: if(Cyc_Tcutil_kind_leq(_tmp610,_tmp614)){{struct Cyc_Core_Opt*_tmpDBC;*
_tmp613=((_tmpDBC=_cycalloc(sizeof(*_tmpDBC)),((_tmpDBC->v=(void*)c1,_tmpDBC))));}
return 1;}else{return 0;}_LL456: _tmp615=_tmp5FE.f1;if(*((int*)_tmp615)!= 2)goto
_LL44B;_tmp616=((struct Cyc_Absyn_Less_kb_struct*)_tmp615)->f1;_tmp617=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*)_tmp615)->f1;_tmp618=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp615)->f2;_tmp619=_tmp5FE.f2;if(*((int*)_tmp619)!= 2)goto _LL44B;_tmp61A=((
struct Cyc_Absyn_Less_kb_struct*)_tmp619)->f1;_tmp61B=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*)_tmp619)->f1;_tmp61C=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp619)->f2;_LL457: if(Cyc_Tcutil_kind_leq(_tmp618,_tmp61C)){{struct Cyc_Core_Opt*
_tmpDBD;*_tmp61B=((_tmpDBD=_cycalloc(sizeof(*_tmpDBD)),((_tmpDBD->v=(void*)c1,
_tmpDBD))));}return 1;}else{if(Cyc_Tcutil_kind_leq(_tmp61C,_tmp618)){{struct Cyc_Core_Opt*
_tmpDBE;*_tmp617=((_tmpDBE=_cycalloc(sizeof(*_tmpDBE)),((_tmpDBE->v=(void*)c2,
_tmpDBE))));}return 1;}else{return 0;}}_LL44B:;}}static int Cyc_Tcutil_tvar_id_counter=
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
Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*t){struct _dyneither_ptr _tmp628=*t->name;
return*((const char*)_check_dyneither_subscript(_tmp628,sizeof(char),0))== '#';}
void Cyc_Tcutil_rewrite_temp_tvar(struct Cyc_Absyn_Tvar*t);void Cyc_Tcutil_rewrite_temp_tvar(
struct Cyc_Absyn_Tvar*t){{const char*_tmpDC9;void*_tmpDC8[1];struct Cyc_String_pa_struct
_tmpDC7;(_tmpDC7.tag=0,((_tmpDC7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*
t->name),((_tmpDC8[0]=& _tmpDC7,Cyc_printf(((_tmpDC9="%s",_tag_dyneither(_tmpDC9,
sizeof(char),3))),_tag_dyneither(_tmpDC8,sizeof(void*),1)))))));}if(!Cyc_Tcutil_is_temp_tvar(
t))return;{const char*_tmpDCA;struct _dyneither_ptr _tmp62C=Cyc_strconcat(((_tmpDCA="`",
_tag_dyneither(_tmpDCA,sizeof(char),2))),(struct _dyneither_ptr)*t->name);{char
_tmpDCD;char _tmpDCC;struct _dyneither_ptr _tmpDCB;(_tmpDCB=_dyneither_ptr_plus(
_tmp62C,sizeof(char),1),((_tmpDCC=*((char*)_check_dyneither_subscript(_tmpDCB,
sizeof(char),0)),((_tmpDCD='t',((_get_dyneither_size(_tmpDCB,sizeof(char))== 1
 && (_tmpDCC == '\000'  && _tmpDCD != '\000')?_throw_arraybounds(): 1,*((char*)
_tmpDCB.curr)=_tmpDCD)))))));}{struct _dyneither_ptr*_tmpDCE;t->name=((_tmpDCE=
_cycalloc(sizeof(struct _dyneither_ptr)* 1),((_tmpDCE[0]=(struct _dyneither_ptr)
_tmp62C,_tmpDCE))));}}}struct _tuple14{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual
f2;void*f3;};static struct _tuple3*Cyc_Tcutil_fndecl2typ_f(struct _tuple14*x);
static struct _tuple3*Cyc_Tcutil_fndecl2typ_f(struct _tuple14*x){struct Cyc_Core_Opt*
_tmpDD1;struct _tuple3*_tmpDD0;return(_tmpDD0=_cycalloc(sizeof(*_tmpDD0)),((
_tmpDD0->f1=(struct Cyc_Core_Opt*)((_tmpDD1=_cycalloc(sizeof(*_tmpDD1)),((_tmpDD1->v=(*
x).f1,_tmpDD1)))),((_tmpDD0->f2=(*x).f2,((_tmpDD0->f3=(*x).f3,_tmpDD0)))))));}
void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*fd);void*Cyc_Tcutil_fndecl2typ(
struct Cyc_Absyn_Fndecl*fd){if(fd->cached_typ == 0){struct Cyc_List_List*_tmp634=0;{
struct Cyc_List_List*atts=fd->attributes;for(0;atts != 0;atts=atts->tl){if(Cyc_Absyn_fntype_att((
void*)atts->hd)){struct Cyc_List_List*_tmpDD2;_tmp634=((_tmpDD2=_cycalloc(sizeof(*
_tmpDD2)),((_tmpDD2->hd=(void*)((void*)atts->hd),((_tmpDD2->tl=_tmp634,_tmpDD2))))));}}}{
struct Cyc_Absyn_FnType_struct _tmpDD8;struct Cyc_Absyn_FnInfo _tmpDD7;struct Cyc_Absyn_FnType_struct*
_tmpDD6;return(void*)((_tmpDD6=_cycalloc(sizeof(*_tmpDD6)),((_tmpDD6[0]=((
_tmpDD8.tag=8,((_tmpDD8.f1=((_tmpDD7.tvars=fd->tvs,((_tmpDD7.effect=fd->effect,((
_tmpDD7.ret_typ=(void*)((void*)fd->ret_type),((_tmpDD7.args=((struct Cyc_List_List*(*)(
struct _tuple3*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_fndecl2typ_f,
fd->args),((_tmpDD7.c_varargs=fd->c_varargs,((_tmpDD7.cyc_varargs=fd->cyc_varargs,((
_tmpDD7.rgn_po=fd->rgn_po,((_tmpDD7.attributes=_tmp634,_tmpDD7)))))))))))))))),
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
struct _tuple16*_tmp63D;void*_tmp63E;struct _tuple17 _tmp63C=*w;_tmp63D=_tmp63C.f1;
_tmp63E=_tmp63C.f2;{struct Cyc_Core_Opt*_tmp640;struct Cyc_Absyn_Tqual _tmp641;
struct _tuple16 _tmp63F=*_tmp63D;_tmp640=_tmp63F.f1;_tmp641=_tmp63F.f2;{struct
_tuple3*_tmpDDD;return(_tmpDDD=_cycalloc(sizeof(*_tmpDDD)),((_tmpDDD->f1=_tmp640,((
_tmpDDD->f2=_tmp641,((_tmpDDD->f3=_tmp63E,_tmpDDD)))))));}}}static void*Cyc_Tcutil_field_type(
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
_tmp65A)return e;{struct Cyc_Absyn_Primop_e_struct _tmpDE5;struct Cyc_Absyn_Exp*
_tmpDE4[1];struct Cyc_Absyn_Primop_e_struct*_tmpDE3;return Cyc_Tcutil_copye(e,(
void*)((_tmpDE3=_cycalloc(sizeof(*_tmpDE3)),((_tmpDE3[0]=((_tmpDE5.tag=3,((
_tmpDE5.f1=(void*)_tmp646,((_tmpDE5.f2=((_tmpDE4[0]=_tmp65B,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpDE4,sizeof(struct Cyc_Absyn_Exp*),
1)))),_tmpDE5)))))),_tmpDE3)))));}}else{if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp647)== 2){struct Cyc_Absyn_Exp*_tmp65F=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp647))->hd;struct Cyc_Absyn_Exp*_tmp660=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp647->tl))->hd;struct Cyc_Absyn_Exp*_tmp661=
Cyc_Tcutil_rsubsexp(r,inst,_tmp65F);struct Cyc_Absyn_Exp*_tmp662=Cyc_Tcutil_rsubsexp(
r,inst,_tmp660);if(_tmp661 == _tmp65F  && _tmp662 == _tmp660)return e;{struct Cyc_Absyn_Primop_e_struct
_tmpDEB;struct Cyc_Absyn_Exp*_tmpDEA[2];struct Cyc_Absyn_Primop_e_struct*_tmpDE9;
return Cyc_Tcutil_copye(e,(void*)((_tmpDE9=_cycalloc(sizeof(*_tmpDE9)),((_tmpDE9[
0]=((_tmpDEB.tag=3,((_tmpDEB.f1=(void*)_tmp646,((_tmpDEB.f2=((_tmpDEA[1]=_tmp662,((
_tmpDEA[0]=_tmp661,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpDEA,sizeof(struct Cyc_Absyn_Exp*),2)))))),_tmpDEB)))))),
_tmpDE9)))));}}else{const char*_tmpDEE;void*_tmpDED;return(_tmpDED=0,((struct Cyc_Absyn_Exp*(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpDEE="primop does not have 1 or 2 args!",
_tag_dyneither(_tmpDEE,sizeof(char),34))),_tag_dyneither(_tmpDED,sizeof(void*),0)));}}
_LL463: if(*((int*)_tmp645)!= 6)goto _LL465;_tmp648=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp645)->f1;_tmp649=((struct Cyc_Absyn_Conditional_e_struct*)_tmp645)->f2;
_tmp64A=((struct Cyc_Absyn_Conditional_e_struct*)_tmp645)->f3;_LL464: {struct Cyc_Absyn_Exp*
_tmp668=Cyc_Tcutil_rsubsexp(r,inst,_tmp648);struct Cyc_Absyn_Exp*_tmp669=Cyc_Tcutil_rsubsexp(
r,inst,_tmp649);struct Cyc_Absyn_Exp*_tmp66A=Cyc_Tcutil_rsubsexp(r,inst,_tmp64A);
if((_tmp668 == _tmp648  && _tmp669 == _tmp649) && _tmp66A == _tmp64A)return e;{struct
Cyc_Absyn_Conditional_e_struct _tmpDF1;struct Cyc_Absyn_Conditional_e_struct*
_tmpDF0;return Cyc_Tcutil_copye(e,(void*)((_tmpDF0=_cycalloc(sizeof(*_tmpDF0)),((
_tmpDF0[0]=((_tmpDF1.tag=6,((_tmpDF1.f1=_tmp668,((_tmpDF1.f2=_tmp669,((_tmpDF1.f3=
_tmp66A,_tmpDF1)))))))),_tmpDF0)))));}}_LL465: if(*((int*)_tmp645)!= 7)goto _LL467;
_tmp64B=((struct Cyc_Absyn_And_e_struct*)_tmp645)->f1;_tmp64C=((struct Cyc_Absyn_And_e_struct*)
_tmp645)->f2;_LL466: {struct Cyc_Absyn_Exp*_tmp66D=Cyc_Tcutil_rsubsexp(r,inst,
_tmp64B);struct Cyc_Absyn_Exp*_tmp66E=Cyc_Tcutil_rsubsexp(r,inst,_tmp64C);if(
_tmp66D == _tmp64B  && _tmp66E == _tmp64C)return e;{struct Cyc_Absyn_And_e_struct
_tmpDF4;struct Cyc_Absyn_And_e_struct*_tmpDF3;return Cyc_Tcutil_copye(e,(void*)((
_tmpDF3=_cycalloc(sizeof(*_tmpDF3)),((_tmpDF3[0]=((_tmpDF4.tag=7,((_tmpDF4.f1=
_tmp66D,((_tmpDF4.f2=_tmp66E,_tmpDF4)))))),_tmpDF3)))));}}_LL467: if(*((int*)
_tmp645)!= 8)goto _LL469;_tmp64D=((struct Cyc_Absyn_Or_e_struct*)_tmp645)->f1;
_tmp64E=((struct Cyc_Absyn_Or_e_struct*)_tmp645)->f2;_LL468: {struct Cyc_Absyn_Exp*
_tmp671=Cyc_Tcutil_rsubsexp(r,inst,_tmp64D);struct Cyc_Absyn_Exp*_tmp672=Cyc_Tcutil_rsubsexp(
r,inst,_tmp64E);if(_tmp671 == _tmp64D  && _tmp672 == _tmp64E)return e;{struct Cyc_Absyn_Or_e_struct
_tmpDF7;struct Cyc_Absyn_Or_e_struct*_tmpDF6;return Cyc_Tcutil_copye(e,(void*)((
_tmpDF6=_cycalloc(sizeof(*_tmpDF6)),((_tmpDF6[0]=((_tmpDF7.tag=8,((_tmpDF7.f1=
_tmp671,((_tmpDF7.f2=_tmp672,_tmpDF7)))))),_tmpDF6)))));}}_LL469: if(*((int*)
_tmp645)!= 9)goto _LL46B;_tmp64F=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp645)->f1;
_tmp650=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp645)->f2;_LL46A: {struct Cyc_Absyn_Exp*
_tmp675=Cyc_Tcutil_rsubsexp(r,inst,_tmp64F);struct Cyc_Absyn_Exp*_tmp676=Cyc_Tcutil_rsubsexp(
r,inst,_tmp650);if(_tmp675 == _tmp64F  && _tmp676 == _tmp650)return e;{struct Cyc_Absyn_SeqExp_e_struct
_tmpDFA;struct Cyc_Absyn_SeqExp_e_struct*_tmpDF9;return Cyc_Tcutil_copye(e,(void*)((
_tmpDF9=_cycalloc(sizeof(*_tmpDF9)),((_tmpDF9[0]=((_tmpDFA.tag=9,((_tmpDFA.f1=
_tmp675,((_tmpDFA.f2=_tmp676,_tmpDFA)))))),_tmpDF9)))));}}_LL46B: if(*((int*)
_tmp645)!= 15)goto _LL46D;_tmp651=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp645)->f1;
_tmp652=((struct Cyc_Absyn_Cast_e_struct*)_tmp645)->f2;_tmp653=((struct Cyc_Absyn_Cast_e_struct*)
_tmp645)->f3;_tmp654=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp645)->f4;_LL46C: {
struct Cyc_Absyn_Exp*_tmp679=Cyc_Tcutil_rsubsexp(r,inst,_tmp652);void*_tmp67A=Cyc_Tcutil_rsubstitute(
r,inst,_tmp651);if(_tmp679 == _tmp652  && _tmp67A == _tmp651)return e;{struct Cyc_Absyn_Cast_e_struct
_tmpDFD;struct Cyc_Absyn_Cast_e_struct*_tmpDFC;return Cyc_Tcutil_copye(e,(void*)((
_tmpDFC=_cycalloc(sizeof(*_tmpDFC)),((_tmpDFC[0]=((_tmpDFD.tag=15,((_tmpDFD.f1=(
void*)_tmp67A,((_tmpDFD.f2=_tmp679,((_tmpDFD.f3=_tmp653,((_tmpDFD.f4=(void*)
_tmp654,_tmpDFD)))))))))),_tmpDFC)))));}}_LL46D: if(*((int*)_tmp645)!= 18)goto
_LL46F;_tmp655=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp645)->f1;_LL46E: {
void*_tmp67D=Cyc_Tcutil_rsubstitute(r,inst,_tmp655);if(_tmp67D == _tmp655)return e;{
struct Cyc_Absyn_Sizeoftyp_e_struct _tmpE00;struct Cyc_Absyn_Sizeoftyp_e_struct*
_tmpDFF;return Cyc_Tcutil_copye(e,(void*)((_tmpDFF=_cycalloc(sizeof(*_tmpDFF)),((
_tmpDFF[0]=((_tmpE00.tag=18,((_tmpE00.f1=(void*)_tmp67D,_tmpE00)))),_tmpDFF)))));}}
_LL46F: if(*((int*)_tmp645)!= 19)goto _LL471;_tmp656=((struct Cyc_Absyn_Sizeofexp_e_struct*)
_tmp645)->f1;_LL470: {struct Cyc_Absyn_Exp*_tmp680=Cyc_Tcutil_rsubsexp(r,inst,
_tmp656);if(_tmp680 == _tmp656)return e;{struct Cyc_Absyn_Sizeofexp_e_struct _tmpE03;
struct Cyc_Absyn_Sizeofexp_e_struct*_tmpE02;return Cyc_Tcutil_copye(e,(void*)((
_tmpE02=_cycalloc(sizeof(*_tmpE02)),((_tmpE02[0]=((_tmpE03.tag=19,((_tmpE03.f1=
_tmp680,_tmpE03)))),_tmpE02)))));}}_LL471: if(*((int*)_tmp645)!= 20)goto _LL473;
_tmp657=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp645)->f1;_tmp658=(void*)((
struct Cyc_Absyn_Offsetof_e_struct*)_tmp645)->f2;_LL472: {void*_tmp683=Cyc_Tcutil_rsubstitute(
r,inst,_tmp657);if(_tmp683 == _tmp657)return e;{struct Cyc_Absyn_Offsetof_e_struct
_tmpE06;struct Cyc_Absyn_Offsetof_e_struct*_tmpE05;return Cyc_Tcutil_copye(e,(void*)((
_tmpE05=_cycalloc(sizeof(*_tmpE05)),((_tmpE05[0]=((_tmpE06.tag=20,((_tmpE06.f1=(
void*)_tmp683,((_tmpE06.f2=(void*)_tmp658,_tmpE06)))))),_tmpE05)))));}}_LL473:
if(*((int*)_tmp645)!= 39)goto _LL475;_tmp659=(void*)((struct Cyc_Absyn_Valueof_e_struct*)
_tmp645)->f1;_LL474: {void*_tmp686=Cyc_Tcutil_rsubstitute(r,inst,_tmp659);if(
_tmp686 == _tmp659)return e;{struct Cyc_Absyn_Valueof_e_struct _tmpE09;struct Cyc_Absyn_Valueof_e_struct*
_tmpE08;return Cyc_Tcutil_copye(e,(void*)((_tmpE08=_cycalloc(sizeof(*_tmpE08)),((
_tmpE08[0]=((_tmpE09.tag=39,((_tmpE09.f1=(void*)_tmp686,_tmpE09)))),_tmpE08)))));}}
_LL475:;_LL476: {const char*_tmpE0C;void*_tmpE0B;return(_tmpE0B=0,((struct Cyc_Absyn_Exp*(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpE0C="non-type-level-expression in Tcutil::rsubsexp",
_tag_dyneither(_tmpE0C,sizeof(char),46))),_tag_dyneither(_tmpE0B,sizeof(void*),0)));}
_LL458:;}void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*rgn,struct Cyc_List_List*
inst,void*t);void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*rgn,struct Cyc_List_List*
inst,void*t){void*_tmp68B=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*_tmp68C;
struct Cyc_Absyn_AggrInfo _tmp68D;union Cyc_Absyn_AggrInfoU_union _tmp68E;struct Cyc_List_List*
_tmp68F;struct Cyc_Absyn_DatatypeInfo _tmp690;union Cyc_Absyn_DatatypeInfoU_union
_tmp691;struct Cyc_List_List*_tmp692;struct Cyc_Core_Opt*_tmp693;struct Cyc_Absyn_DatatypeFieldInfo
_tmp694;union Cyc_Absyn_DatatypeFieldInfoU_union _tmp695;struct Cyc_List_List*
_tmp696;struct _tuple2*_tmp697;struct Cyc_List_List*_tmp698;struct Cyc_Absyn_Typedefdecl*
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
_tmpE12;struct Cyc_Absyn_AggrInfo _tmpE11;struct Cyc_Absyn_AggrType_struct*_tmpE10;
return _tmp6C4 == _tmp68F?t:(void*)((_tmpE10=_cycalloc(sizeof(*_tmpE10)),((_tmpE10[
0]=((_tmpE12.tag=10,((_tmpE12.f1=((_tmpE11.aggr_info=_tmp68E,((_tmpE11.targs=
_tmp6C4,_tmpE11)))),_tmpE12)))),_tmpE10))));}_LL47C: if(*((int*)_tmp68B)!= 2)goto
_LL47E;_tmp690=((struct Cyc_Absyn_DatatypeType_struct*)_tmp68B)->f1;_tmp691=
_tmp690.datatype_info;_tmp692=_tmp690.targs;_tmp693=_tmp690.rgn;_LL47D: {struct
Cyc_List_List*_tmp6C8=Cyc_Tcutil_substs(rgn,inst,_tmp692);struct Cyc_Core_Opt*
new_r;if((unsigned int)_tmp693){void*_tmp6C9=Cyc_Tcutil_rsubstitute(rgn,inst,(
void*)_tmp693->v);if(_tmp6C9 == (void*)_tmp693->v)new_r=_tmp693;else{struct Cyc_Core_Opt*
_tmpE13;new_r=((_tmpE13=_cycalloc(sizeof(*_tmpE13)),((_tmpE13->v=(void*)_tmp6C9,
_tmpE13))));}}else{new_r=_tmp693;}{struct Cyc_Absyn_DatatypeType_struct _tmpE19;
struct Cyc_Absyn_DatatypeInfo _tmpE18;struct Cyc_Absyn_DatatypeType_struct*_tmpE17;
return _tmp6C8 == _tmp692  && new_r == _tmp693?t:(void*)((_tmpE17=_cycalloc(sizeof(*
_tmpE17)),((_tmpE17[0]=((_tmpE19.tag=2,((_tmpE19.f1=((_tmpE18.datatype_info=
_tmp691,((_tmpE18.targs=_tmp6C8,((_tmpE18.rgn=new_r,_tmpE18)))))),_tmpE19)))),
_tmpE17))));}}_LL47E: if(*((int*)_tmp68B)!= 3)goto _LL480;_tmp694=((struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp68B)->f1;_tmp695=_tmp694.field_info;_tmp696=_tmp694.targs;_LL47F: {struct Cyc_List_List*
_tmp6CE=Cyc_Tcutil_substs(rgn,inst,_tmp696);struct Cyc_Absyn_DatatypeFieldType_struct
_tmpE1F;struct Cyc_Absyn_DatatypeFieldInfo _tmpE1E;struct Cyc_Absyn_DatatypeFieldType_struct*
_tmpE1D;return _tmp6CE == _tmp696?t:(void*)((_tmpE1D=_cycalloc(sizeof(*_tmpE1D)),((
_tmpE1D[0]=((_tmpE1F.tag=3,((_tmpE1F.f1=((_tmpE1E.field_info=_tmp695,((_tmpE1E.targs=
_tmp6CE,_tmpE1E)))),_tmpE1F)))),_tmpE1D))));}_LL480: if(*((int*)_tmp68B)!= 16)
goto _LL482;_tmp697=((struct Cyc_Absyn_TypedefType_struct*)_tmp68B)->f1;_tmp698=((
struct Cyc_Absyn_TypedefType_struct*)_tmp68B)->f2;_tmp699=((struct Cyc_Absyn_TypedefType_struct*)
_tmp68B)->f3;_tmp69A=((struct Cyc_Absyn_TypedefType_struct*)_tmp68B)->f4;_LL481: {
struct Cyc_List_List*_tmp6D2=Cyc_Tcutil_substs(rgn,inst,_tmp698);struct Cyc_Absyn_TypedefType_struct
_tmpE22;struct Cyc_Absyn_TypedefType_struct*_tmpE21;return _tmp6D2 == _tmp698?t:(
void*)((_tmpE21=_cycalloc(sizeof(*_tmpE21)),((_tmpE21[0]=((_tmpE22.tag=16,((
_tmpE22.f1=_tmp697,((_tmpE22.f2=_tmp6D2,((_tmpE22.f3=_tmp699,((_tmpE22.f4=
_tmp69A,_tmpE22)))))))))),_tmpE21))));}_LL482: if(*((int*)_tmp68B)!= 7)goto _LL484;
_tmp69B=((struct Cyc_Absyn_ArrayType_struct*)_tmp68B)->f1;_tmp69C=(void*)_tmp69B.elt_type;
_tmp69D=_tmp69B.tq;_tmp69E=_tmp69B.num_elts;_tmp69F=_tmp69B.zero_term;_tmp6A0=
_tmp69B.zt_loc;_LL483: {void*_tmp6D5=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp69C);
struct Cyc_Absyn_ArrayType_struct _tmpE28;struct Cyc_Absyn_ArrayInfo _tmpE27;struct
Cyc_Absyn_ArrayType_struct*_tmpE26;return _tmp6D5 == _tmp69C?t:(void*)((_tmpE26=
_cycalloc(sizeof(*_tmpE26)),((_tmpE26[0]=((_tmpE28.tag=7,((_tmpE28.f1=((_tmpE27.elt_type=(
void*)_tmp6D5,((_tmpE27.tq=_tmp69D,((_tmpE27.num_elts=_tmp69E,((_tmpE27.zero_term=
_tmp69F,((_tmpE27.zt_loc=_tmp6A0,_tmpE27)))))))))),_tmpE28)))),_tmpE26))));}
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
_tmp6DE);if(_tmp6DF != _tmp6DE){struct Cyc_Absyn_Upper_b_struct _tmpE2B;struct Cyc_Absyn_Upper_b_struct*
_tmpE2A;_tmp6DB=Cyc_Absyn_new_conref((void*)((_tmpE2A=_cycalloc(sizeof(*_tmpE2A)),((
_tmpE2A[0]=((_tmpE2B.tag=0,((_tmpE2B.f1=_tmp6DF,_tmpE2B)))),_tmpE2A)))));}goto
_LL4B1;}_LL4B4:;_LL4B5: goto _LL4B1;_LL4B1:;}if((_tmp6D9 == _tmp6A2  && _tmp6DA == 
_tmp6A5) && _tmp6DB == _tmp6A7)return t;{struct Cyc_Absyn_PointerType_struct _tmpE35;
struct Cyc_Absyn_PtrAtts _tmpE34;struct Cyc_Absyn_PtrInfo _tmpE33;struct Cyc_Absyn_PointerType_struct*
_tmpE32;return(void*)((_tmpE32=_cycalloc(sizeof(*_tmpE32)),((_tmpE32[0]=((
_tmpE35.tag=4,((_tmpE35.f1=((_tmpE33.elt_typ=(void*)_tmp6D9,((_tmpE33.elt_tq=
_tmp6A3,((_tmpE33.ptr_atts=((_tmpE34.rgn=(void*)_tmp6DA,((_tmpE34.nullable=
_tmp6A6,((_tmpE34.bounds=_tmp6DB,((_tmpE34.zero_term=_tmp6A8,((_tmpE34.ptrloc=0,
_tmpE34)))))))))),_tmpE33)))))),_tmpE35)))),_tmpE32))));}}_LL486: if(*((int*)
_tmp68B)!= 8)goto _LL488;_tmp6A9=((struct Cyc_Absyn_FnType_struct*)_tmp68B)->f1;
_tmp6AA=_tmp6A9.tvars;_tmp6AB=_tmp6A9.effect;_tmp6AC=(void*)_tmp6A9.ret_typ;
_tmp6AD=_tmp6A9.args;_tmp6AE=_tmp6A9.c_varargs;_tmp6AF=_tmp6A9.cyc_varargs;
_tmp6B0=_tmp6A9.rgn_po;_tmp6B1=_tmp6A9.attributes;_LL487:{struct Cyc_List_List*
_tmp6E6=_tmp6AA;for(0;_tmp6E6 != 0;_tmp6E6=_tmp6E6->tl){struct _tuple8*_tmpE3F;
struct Cyc_Absyn_VarType_struct _tmpE3E;struct Cyc_Absyn_VarType_struct*_tmpE3D;
struct Cyc_List_List*_tmpE3C;inst=((_tmpE3C=_region_malloc(rgn,sizeof(*_tmpE3C)),((
_tmpE3C->hd=((_tmpE3F=_region_malloc(rgn,sizeof(*_tmpE3F)),((_tmpE3F->f1=(struct
Cyc_Absyn_Tvar*)_tmp6E6->hd,((_tmpE3F->f2=(void*)((_tmpE3D=_cycalloc(sizeof(*
_tmpE3D)),((_tmpE3D[0]=((_tmpE3E.tag=1,((_tmpE3E.f1=(struct Cyc_Absyn_Tvar*)
_tmp6E6->hd,_tmpE3E)))),_tmpE3D)))),_tmpE3F)))))),((_tmpE3C->tl=inst,_tmpE3C))))));}}{
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
_tmpE40;eff2=((_tmpE40=_cycalloc(sizeof(*_tmpE40)),((_tmpE40->v=(void*)_tmp6F0,
_tmpE40))));}}{struct Cyc_Absyn_VarargInfo*cyc_varargs2;if(_tmp6AF == 0)
cyc_varargs2=0;else{struct Cyc_Core_Opt*_tmp6F3;struct Cyc_Absyn_Tqual _tmp6F4;void*
_tmp6F5;int _tmp6F6;struct Cyc_Absyn_VarargInfo _tmp6F2=*_tmp6AF;_tmp6F3=_tmp6F2.name;
_tmp6F4=_tmp6F2.tq;_tmp6F5=(void*)_tmp6F2.type;_tmp6F6=_tmp6F2.inject;{void*
_tmp6F7=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6F5);if(_tmp6F7 == _tmp6F5)
cyc_varargs2=_tmp6AF;else{struct Cyc_Absyn_VarargInfo*_tmpE41;cyc_varargs2=((
_tmpE41=_cycalloc(sizeof(*_tmpE41)),((_tmpE41->name=_tmp6F3,((_tmpE41->tq=
_tmp6F4,((_tmpE41->type=(void*)_tmp6F7,((_tmpE41->inject=_tmp6F6,_tmpE41))))))))));}}}{
struct Cyc_List_List*rgn_po2;struct Cyc_List_List*_tmp6FA;struct Cyc_List_List*
_tmp6FB;struct _tuple1 _tmp6F9=Cyc_List_rsplit(rgn,rgn,_tmp6B0);_tmp6FA=_tmp6F9.f1;
_tmp6FB=_tmp6F9.f2;{struct Cyc_List_List*_tmp6FC=Cyc_Tcutil_substs(rgn,inst,
_tmp6FA);struct Cyc_List_List*_tmp6FD=Cyc_Tcutil_substs(rgn,inst,_tmp6FB);if(
_tmp6FC == _tmp6FA  && _tmp6FD == _tmp6FB)rgn_po2=_tmp6B0;else{rgn_po2=Cyc_List_zip(
_tmp6FC,_tmp6FD);}{struct Cyc_Absyn_FnType_struct _tmpE47;struct Cyc_Absyn_FnInfo
_tmpE46;struct Cyc_Absyn_FnType_struct*_tmpE45;return(void*)((_tmpE45=_cycalloc(
sizeof(*_tmpE45)),((_tmpE45[0]=((_tmpE47.tag=8,((_tmpE47.f1=((_tmpE46.tvars=
_tmp6AA,((_tmpE46.effect=eff2,((_tmpE46.ret_typ=(void*)Cyc_Tcutil_rsubstitute(
rgn,inst,_tmp6AC),((_tmpE46.args=_tmp6EF,((_tmpE46.c_varargs=_tmp6AE,((_tmpE46.cyc_varargs=
cyc_varargs2,((_tmpE46.rgn_po=rgn_po2,((_tmpE46.attributes=_tmp6B1,_tmpE46)))))))))))))))),
_tmpE47)))),_tmpE45))));}}}}}}_LL488: if(*((int*)_tmp68B)!= 9)goto _LL48A;_tmp6B2=((
struct Cyc_Absyn_TupleType_struct*)_tmp68B)->f1;_LL489: {struct Cyc_List_List*
_tmp701=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _tuple5*),
struct Cyc_List_List*x))Cyc_List_rmap)(rgn,Cyc_Tcutil_snd_tqt,_tmp6B2);struct Cyc_List_List*
_tmp702=Cyc_Tcutil_substs(rgn,inst,_tmp701);if(_tmp702 == _tmp701)return t;{struct
Cyc_List_List*_tmp703=((struct Cyc_List_List*(*)(struct _tuple5*(*f)(struct _tuple5*,
void*),struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_map2_tq,
_tmp6B2,_tmp702);struct Cyc_Absyn_TupleType_struct _tmpE4A;struct Cyc_Absyn_TupleType_struct*
_tmpE49;return(void*)((_tmpE49=_cycalloc(sizeof(*_tmpE49)),((_tmpE49[0]=((
_tmpE4A.tag=9,((_tmpE4A.f1=_tmp703,_tmpE4A)))),_tmpE49))));}}_LL48A: if(*((int*)
_tmp68B)!= 11)goto _LL48C;_tmp6B3=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp68B)->f1;_tmp6B4=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp68B)->f2;_LL48B: {
struct Cyc_List_List*_tmp706=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*
f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_rmap)(rgn,Cyc_Tcutil_field_type,
_tmp6B4);struct Cyc_List_List*_tmp707=Cyc_Tcutil_substs(rgn,inst,_tmp706);if(
_tmp707 == _tmp706)return t;{struct Cyc_List_List*_tmp708=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*,void*),struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_zip_field_type,_tmp6B4,
_tmp707);struct Cyc_Absyn_AnonAggrType_struct _tmpE4D;struct Cyc_Absyn_AnonAggrType_struct*
_tmpE4C;return(void*)((_tmpE4C=_cycalloc(sizeof(*_tmpE4C)),((_tmpE4C[0]=((
_tmpE4D.tag=11,((_tmpE4D.f1=(void*)_tmp6B3,((_tmpE4D.f2=_tmp708,_tmpE4D)))))),
_tmpE4C))));}}_LL48C: if(*((int*)_tmp68B)!= 0)goto _LL48E;_tmp6B5=((struct Cyc_Absyn_Evar_struct*)
_tmp68B)->f2;_LL48D: if(_tmp6B5 != 0)return Cyc_Tcutil_rsubstitute(rgn,inst,(void*)
_tmp6B5->v);else{return t;}_LL48E: if(*((int*)_tmp68B)!= 14)goto _LL490;_tmp6B6=(
void*)((struct Cyc_Absyn_RgnHandleType_struct*)_tmp68B)->f1;_LL48F: {void*_tmp70B=
Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6B6);struct Cyc_Absyn_RgnHandleType_struct
_tmpE50;struct Cyc_Absyn_RgnHandleType_struct*_tmpE4F;return _tmp70B == _tmp6B6?t:(
void*)((_tmpE4F=_cycalloc(sizeof(*_tmpE4F)),((_tmpE4F[0]=((_tmpE50.tag=14,((
_tmpE50.f1=(void*)_tmp70B,_tmpE50)))),_tmpE4F))));}_LL490: if(*((int*)_tmp68B)!= 
15)goto _LL492;_tmp6B7=(void*)((struct Cyc_Absyn_DynRgnType_struct*)_tmp68B)->f1;
_tmp6B8=(void*)((struct Cyc_Absyn_DynRgnType_struct*)_tmp68B)->f2;_LL491: {void*
_tmp70E=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6B7);void*_tmp70F=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmp6B8);struct Cyc_Absyn_DynRgnType_struct _tmpE53;struct Cyc_Absyn_DynRgnType_struct*
_tmpE52;return _tmp70E == _tmp6B7  && _tmp70F == _tmp6B8?t:(void*)((_tmpE52=
_cycalloc(sizeof(*_tmpE52)),((_tmpE52[0]=((_tmpE53.tag=15,((_tmpE53.f1=(void*)
_tmp70E,((_tmpE53.f2=(void*)_tmp70F,_tmpE53)))))),_tmpE52))));}_LL492: if(*((int*)
_tmp68B)!= 18)goto _LL494;_tmp6B9=(void*)((struct Cyc_Absyn_TagType_struct*)
_tmp68B)->f1;_LL493: {void*_tmp712=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6B9);
struct Cyc_Absyn_TagType_struct _tmpE56;struct Cyc_Absyn_TagType_struct*_tmpE55;
return _tmp712 == _tmp6B9?t:(void*)((_tmpE55=_cycalloc(sizeof(*_tmpE55)),((_tmpE55[
0]=((_tmpE56.tag=18,((_tmpE56.f1=(void*)_tmp712,_tmpE56)))),_tmpE55))));}_LL494:
if(*((int*)_tmp68B)!= 17)goto _LL496;_tmp6BA=((struct Cyc_Absyn_ValueofType_struct*)
_tmp68B)->f1;_LL495: {struct Cyc_Absyn_Exp*_tmp715=Cyc_Tcutil_rsubsexp(rgn,inst,
_tmp6BA);struct Cyc_Absyn_ValueofType_struct _tmpE59;struct Cyc_Absyn_ValueofType_struct*
_tmpE58;return _tmp715 == _tmp6BA?t:(void*)((_tmpE58=_cycalloc(sizeof(*_tmpE58)),((
_tmpE58[0]=((_tmpE59.tag=17,((_tmpE59.f1=_tmp715,_tmpE59)))),_tmpE58))));}_LL496:
if(*((int*)_tmp68B)!= 12)goto _LL498;_LL497: goto _LL499;_LL498: if(*((int*)_tmp68B)
!= 13)goto _LL49A;_LL499: goto _LL49B;_LL49A: if((int)_tmp68B != 0)goto _LL49C;_LL49B:
goto _LL49D;_LL49C: if(_tmp68B <= (void*)4)goto _LL49E;if(*((int*)_tmp68B)!= 5)goto
_LL49E;_LL49D: goto _LL49F;_LL49E: if((int)_tmp68B != 1)goto _LL4A0;_LL49F: goto _LL4A1;
_LL4A0: if(_tmp68B <= (void*)4)goto _LL4A2;if(*((int*)_tmp68B)!= 6)goto _LL4A2;
_LL4A1: goto _LL4A3;_LL4A2: if((int)_tmp68B != 3)goto _LL4A4;_LL4A3: goto _LL4A5;_LL4A4:
if((int)_tmp68B != 2)goto _LL4A6;_LL4A5: return t;_LL4A6: if(_tmp68B <= (void*)4)goto
_LL4A8;if(*((int*)_tmp68B)!= 21)goto _LL4A8;_tmp6BB=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp68B)->f1;_LL4A7: {void*_tmp718=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6BB);
struct Cyc_Absyn_RgnsEff_struct _tmpE5C;struct Cyc_Absyn_RgnsEff_struct*_tmpE5B;
return _tmp718 == _tmp6BB?t:(void*)((_tmpE5B=_cycalloc(sizeof(*_tmpE5B)),((_tmpE5B[
0]=((_tmpE5C.tag=21,((_tmpE5C.f1=(void*)_tmp718,_tmpE5C)))),_tmpE5B))));}_LL4A8:
if(_tmp68B <= (void*)4)goto _LL4AA;if(*((int*)_tmp68B)!= 19)goto _LL4AA;_tmp6BC=(
void*)((struct Cyc_Absyn_AccessEff_struct*)_tmp68B)->f1;_LL4A9: {void*_tmp71B=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmp6BC);struct Cyc_Absyn_AccessEff_struct _tmpE5F;struct Cyc_Absyn_AccessEff_struct*
_tmpE5E;return _tmp71B == _tmp6BC?t:(void*)((_tmpE5E=_cycalloc(sizeof(*_tmpE5E)),((
_tmpE5E[0]=((_tmpE5F.tag=19,((_tmpE5F.f1=(void*)_tmp71B,_tmpE5F)))),_tmpE5E))));}
_LL4AA: if(_tmp68B <= (void*)4)goto _LL477;if(*((int*)_tmp68B)!= 20)goto _LL477;
_tmp6BD=((struct Cyc_Absyn_JoinEff_struct*)_tmp68B)->f1;_LL4AB: {struct Cyc_List_List*
_tmp71E=Cyc_Tcutil_substs(rgn,inst,_tmp6BD);struct Cyc_Absyn_JoinEff_struct
_tmpE62;struct Cyc_Absyn_JoinEff_struct*_tmpE61;return _tmp71E == _tmp6BD?t:(void*)((
_tmpE61=_cycalloc(sizeof(*_tmpE61)),((_tmpE61[0]=((_tmpE62.tag=20,((_tmpE62.f1=
_tmp71E,_tmpE62)))),_tmpE61))));}_LL477:;}static struct Cyc_List_List*Cyc_Tcutil_substs(
struct _RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*ts);static
struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*rgn,struct Cyc_List_List*
inst,struct Cyc_List_List*ts){if(ts == 0)return 0;{void*_tmp721=(void*)ts->hd;
struct Cyc_List_List*_tmp722=ts->tl;void*_tmp723=Cyc_Tcutil_rsubstitute(rgn,inst,
_tmp721);struct Cyc_List_List*_tmp724=Cyc_Tcutil_substs(rgn,inst,_tmp722);if(
_tmp721 == _tmp723  && _tmp722 == _tmp724)return ts;{struct Cyc_List_List*_tmpE63;
return(struct Cyc_List_List*)((struct Cyc_List_List*)((_tmpE63=_cycalloc(sizeof(*
_tmpE63)),((_tmpE63->hd=(void*)_tmp723,((_tmpE63->tl=_tmp724,_tmpE63)))))));}}}
void*Cyc_Tcutil_substitute(struct Cyc_List_List*inst,void*t);extern void*Cyc_Tcutil_substitute(
struct Cyc_List_List*inst,void*t){return Cyc_Tcutil_rsubstitute(Cyc_Core_heap_region,
inst,t);}struct _tuple8*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*s,struct Cyc_Absyn_Tvar*
tv);struct _tuple8*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*s,struct Cyc_Absyn_Tvar*
tv){struct Cyc_Core_Opt*_tmp726=Cyc_Tcutil_kind_to_opt(Cyc_Tcutil_tvar_kind(tv));
struct Cyc_Core_Opt*_tmpE66;struct _tuple8*_tmpE65;return(_tmpE65=_cycalloc(
sizeof(*_tmpE65)),((_tmpE65->f1=tv,((_tmpE65->f2=Cyc_Absyn_new_evar(_tmp726,((
_tmpE66=_cycalloc(sizeof(*_tmpE66)),((_tmpE66->v=s,_tmpE66))))),_tmpE65)))));}
struct _tuple8*Cyc_Tcutil_r_make_inst_var(struct _tuple9*env,struct Cyc_Absyn_Tvar*
tv);struct _tuple8*Cyc_Tcutil_r_make_inst_var(struct _tuple9*env,struct Cyc_Absyn_Tvar*
tv){struct _tuple9 _tmp72A;struct Cyc_List_List*_tmp72B;struct _RegionHandle*_tmp72C;
struct _tuple9*_tmp729=env;_tmp72A=*_tmp729;_tmp72B=_tmp72A.f1;_tmp72C=_tmp72A.f2;{
struct Cyc_Core_Opt*_tmp72D=Cyc_Tcutil_kind_to_opt(Cyc_Tcutil_tvar_kind(tv));
struct Cyc_Core_Opt*_tmpE69;struct _tuple8*_tmpE68;return(_tmpE68=_region_malloc(
_tmp72C,sizeof(*_tmpE68)),((_tmpE68->f1=tv,((_tmpE68->f2=Cyc_Absyn_new_evar(
_tmp72D,((_tmpE69=_cycalloc(sizeof(*_tmpE69)),((_tmpE69->v=_tmp72B,_tmpE69))))),
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
_tmp73A=(struct Cyc_Absyn_Tvar*)tvs2->hd;if(_tmp73A->identity == - 1){const char*
_tmpE79;void*_tmpE78;(_tmpE78=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpE79="fast_add_free_tvar: bad identity in tvs2",
_tag_dyneither(_tmpE79,sizeof(char),41))),_tag_dyneither(_tmpE78,sizeof(void*),0)));}
if(_tmp73A->identity == tv->identity)return tvs;}}{struct Cyc_List_List*_tmpE7A;
return(_tmpE7A=_cycalloc(sizeof(*_tmpE7A)),((_tmpE7A->hd=tv,((_tmpE7A->tl=tvs,
_tmpE7A)))));}}struct _tuple18{struct Cyc_Absyn_Tvar*f1;int f2;};static struct Cyc_List_List*
Cyc_Tcutil_fast_add_free_tvar_bool(struct _RegionHandle*r,struct Cyc_List_List*tvs,
struct Cyc_Absyn_Tvar*tv,int b);static struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar_bool(
struct _RegionHandle*r,struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv,int b){if(
tv->identity == - 1){const char*_tmpE7D;void*_tmpE7C;(_tmpE7C=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpE7D="fast_add_free_tvar_bool: bad identity in tv",
_tag_dyneither(_tmpE7D,sizeof(char),44))),_tag_dyneither(_tmpE7C,sizeof(void*),0)));}{
struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){struct _tuple18 _tmp741;
struct Cyc_Absyn_Tvar*_tmp742;int _tmp743;int*_tmp744;struct _tuple18*_tmp740=(
struct _tuple18*)tvs2->hd;_tmp741=*_tmp740;_tmp742=_tmp741.f1;_tmp743=_tmp741.f2;
_tmp744=(int*)&(*_tmp740).f2;if(_tmp742->identity == - 1){const char*_tmpE80;void*
_tmpE7F;(_tmpE7F=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmpE80="fast_add_free_tvar_bool: bad identity in tvs2",_tag_dyneither(_tmpE80,
sizeof(char),46))),_tag_dyneither(_tmpE7F,sizeof(void*),0)));}if(_tmp742->identity
== tv->identity){*_tmp744=*_tmp744  || b;return tvs;}}}{struct _tuple18*_tmpE83;
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
b){void*_tmp74D=Cyc_Tcutil_compress(e);int _tmp74E;_LL4B7: if(_tmp74D <= (void*)4)
goto _LL4B9;if(*((int*)_tmp74D)!= 0)goto _LL4B9;_tmp74E=((struct Cyc_Absyn_Evar_struct*)
_tmp74D)->f3;_LL4B8:{struct Cyc_List_List*es2=es;for(0;es2 != 0;es2=es2->tl){
struct _tuple19 _tmp750;void*_tmp751;int _tmp752;int*_tmp753;struct _tuple19*_tmp74F=(
struct _tuple19*)es2->hd;_tmp750=*_tmp74F;_tmp751=_tmp750.f1;_tmp752=_tmp750.f2;
_tmp753=(int*)&(*_tmp74F).f2;{void*_tmp754=Cyc_Tcutil_compress(_tmp751);int
_tmp755;_LL4BC: if(_tmp754 <= (void*)4)goto _LL4BE;if(*((int*)_tmp754)!= 0)goto
_LL4BE;_tmp755=((struct Cyc_Absyn_Evar_struct*)_tmp754)->f3;_LL4BD: if(_tmp74E == 
_tmp755){if(b != *_tmp753)*_tmp753=1;return es;}goto _LL4BB;_LL4BE:;_LL4BF: goto
_LL4BB;_LL4BB:;}}}{struct _tuple19*_tmpE8B;struct Cyc_List_List*_tmpE8A;return(
_tmpE8A=_region_malloc(r,sizeof(*_tmpE8A)),((_tmpE8A->hd=((_tmpE8B=
_region_malloc(r,sizeof(*_tmpE8B)),((_tmpE8B->f1=e,((_tmpE8B->f2=b,_tmpE8B)))))),((
_tmpE8A->tl=es,_tmpE8A)))));}_LL4B9:;_LL4BA: return es;_LL4B6:;}static struct Cyc_List_List*
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
Cyc_List_List*res=0;for(0;tvs != 0;tvs=tvs->tl){struct Cyc_Absyn_Tvar*_tmp75A;int
_tmp75B;struct _tuple18 _tmp759=*((struct _tuple18*)tvs->hd);_tmp75A=_tmp759.f1;
_tmp75B=_tmp759.f2;{int present=0;{struct Cyc_List_List*b=btvs;for(0;b != 0;b=b->tl){
if(_tmp75A->identity == ((struct Cyc_Absyn_Tvar*)b->hd)->identity){present=1;
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
else{unsigned int _tmp761;int _tmp762;struct _tuple7 _tmp760=Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)width);_tmp761=_tmp760.f1;_tmp762=_tmp760.f2;if(!_tmp762){
const char*_tmpE94;void*_tmpE93;(_tmpE93=0,Cyc_Tcutil_terr(loc,((_tmpE94="bitfield width cannot use sizeof or offsetof",
_tag_dyneither(_tmpE94,sizeof(char),45))),_tag_dyneither(_tmpE93,sizeof(void*),0)));}
w=_tmp761;}{void*_tmp765=Cyc_Tcutil_compress(field_typ);void*_tmp766;_LL4C1: if(
_tmp765 <= (void*)4)goto _LL4C3;if(*((int*)_tmp765)!= 5)goto _LL4C3;_tmp766=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp765)->f2;_LL4C2:{void*_tmp767=_tmp766;_LL4C6:
if((int)_tmp767 != 0)goto _LL4C8;_LL4C7: if(w > 8){const char*_tmpE97;void*_tmpE96;(
_tmpE96=0,Cyc_Tcutil_terr(loc,((_tmpE97="bitfield larger than type",
_tag_dyneither(_tmpE97,sizeof(char),26))),_tag_dyneither(_tmpE96,sizeof(void*),0)));}
goto _LL4C5;_LL4C8: if((int)_tmp767 != 1)goto _LL4CA;_LL4C9: if(w > 16){const char*
_tmpE9A;void*_tmpE99;(_tmpE99=0,Cyc_Tcutil_terr(loc,((_tmpE9A="bitfield larger than type",
_tag_dyneither(_tmpE9A,sizeof(char),26))),_tag_dyneither(_tmpE99,sizeof(void*),0)));}
goto _LL4C5;_LL4CA: if((int)_tmp767 != 3)goto _LL4CC;_LL4CB: goto _LL4CD;_LL4CC: if((
int)_tmp767 != 2)goto _LL4CE;_LL4CD: if(w > 32){const char*_tmpE9D;void*_tmpE9C;(
_tmpE9C=0,Cyc_Tcutil_terr(loc,((_tmpE9D="bitfield larger than type",
_tag_dyneither(_tmpE9D,sizeof(char),26))),_tag_dyneither(_tmpE9C,sizeof(void*),0)));}
goto _LL4C5;_LL4CE: if((int)_tmp767 != 4)goto _LL4C5;_LL4CF: if(w > 64){const char*
_tmpEA0;void*_tmpE9F;(_tmpE9F=0,Cyc_Tcutil_terr(loc,((_tmpEA0="bitfield larger than type",
_tag_dyneither(_tmpEA0,sizeof(char),26))),_tag_dyneither(_tmpE9F,sizeof(void*),0)));}
goto _LL4C5;_LL4C5:;}goto _LL4C0;_LL4C3:;_LL4C4:{const char*_tmpEA5;void*_tmpEA4[2];
struct Cyc_String_pa_struct _tmpEA3;struct Cyc_String_pa_struct _tmpEA2;(_tmpEA2.tag=
0,((_tmpEA2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
field_typ)),((_tmpEA3.tag=0,((_tmpEA3.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*fn),((_tmpEA4[0]=& _tmpEA3,((_tmpEA4[1]=& _tmpEA2,Cyc_Tcutil_terr(
loc,((_tmpEA5="bitfield %s must have integral type but has type %s",
_tag_dyneither(_tmpEA5,sizeof(char),52))),_tag_dyneither(_tmpEA4,sizeof(void*),2)))))))))))));}
goto _LL4C0;_LL4C0:;}}}static void Cyc_Tcutil_check_field_atts(struct Cyc_Position_Segment*
loc,struct _dyneither_ptr*fn,struct Cyc_List_List*atts);static void Cyc_Tcutil_check_field_atts(
struct Cyc_Position_Segment*loc,struct _dyneither_ptr*fn,struct Cyc_List_List*atts){
for(0;atts != 0;atts=atts->tl){void*_tmp774=(void*)atts->hd;_LL4D1: if((int)
_tmp774 != 5)goto _LL4D3;_LL4D2: continue;_LL4D3: if(_tmp774 <= (void*)17)goto _LL4D5;
if(*((int*)_tmp774)!= 1)goto _LL4D5;_LL4D4: continue;_LL4D5:;_LL4D6: {const char*
_tmpEAA;void*_tmpEA9[2];struct Cyc_String_pa_struct _tmpEA8;struct Cyc_String_pa_struct
_tmpEA7;(_tmpEA7.tag=0,((_tmpEA7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*
fn),((_tmpEA8.tag=0,((_tmpEA8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absyn_attribute2string((void*)atts->hd)),((_tmpEA9[0]=& _tmpEA8,((_tmpEA9[1]=&
_tmpEA7,Cyc_Tcutil_terr(loc,((_tmpEAA="bad attribute %s on member %s",
_tag_dyneither(_tmpEAA,sizeof(char),30))),_tag_dyneither(_tmpEA9,sizeof(void*),2)))))))))))));}
_LL4D0:;}}struct Cyc_Tcutil_CVTEnv{struct _RegionHandle*r;struct Cyc_List_List*
kind_env;struct Cyc_List_List*free_vars;struct Cyc_List_List*free_evars;int
generalize_evars;int fn_result;};int Cyc_Tcutil_extract_const_from_typedef(struct
Cyc_Position_Segment*loc,int declared_const,void*t);int Cyc_Tcutil_extract_const_from_typedef(
struct Cyc_Position_Segment*loc,int declared_const,void*t){void*_tmp779=t;struct
Cyc_Absyn_Typedefdecl*_tmp77A;void**_tmp77B;_LL4D8: if(_tmp779 <= (void*)4)goto
_LL4DA;if(*((int*)_tmp779)!= 16)goto _LL4DA;_tmp77A=((struct Cyc_Absyn_TypedefType_struct*)
_tmp779)->f3;_tmp77B=((struct Cyc_Absyn_TypedefType_struct*)_tmp779)->f4;_LL4D9:
if((((struct Cyc_Absyn_Typedefdecl*)_check_null(_tmp77A))->tq).real_const  || (
_tmp77A->tq).print_const){if(declared_const){const char*_tmpEAD;void*_tmpEAC;(
_tmpEAC=0,Cyc_Tcutil_warn(loc,((_tmpEAD="extra const",_tag_dyneither(_tmpEAD,
sizeof(char),12))),_tag_dyneither(_tmpEAC,sizeof(void*),0)));}return 1;}if((
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
_tmp786;struct Cyc_Absyn_Enumdecl**_tmp787;struct Cyc_Absyn_DatatypeInfo _tmp788;
union Cyc_Absyn_DatatypeInfoU_union _tmp789;union Cyc_Absyn_DatatypeInfoU_union*
_tmp78A;struct Cyc_List_List*_tmp78B;struct Cyc_List_List**_tmp78C;struct Cyc_Core_Opt*
_tmp78D;struct Cyc_Core_Opt**_tmp78E;struct Cyc_Absyn_DatatypeFieldInfo _tmp78F;
union Cyc_Absyn_DatatypeFieldInfoU_union _tmp790;union Cyc_Absyn_DatatypeFieldInfoU_union*
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
struct Cyc_Absyn_Less_kb_struct _tmpEB0;struct Cyc_Absyn_Less_kb_struct*_tmpEAF;
struct Cyc_Absyn_Tvar*_tmp7C6=Cyc_Tcutil_new_tvar((void*)((_tmpEAF=_cycalloc(
sizeof(*_tmpEAF)),((_tmpEAF[0]=((_tmpEB0.tag=2,((_tmpEB0.f1=0,((_tmpEB0.f2=(void*)
expected_kind,_tmpEB0)))))),_tmpEAF)))));{struct Cyc_Absyn_VarType_struct*_tmpEB6;
struct Cyc_Absyn_VarType_struct _tmpEB5;struct Cyc_Core_Opt*_tmpEB4;*_tmp782=((
_tmpEB4=_cycalloc(sizeof(*_tmpEB4)),((_tmpEB4->v=(void*)((void*)((_tmpEB6=
_cycalloc(sizeof(*_tmpEB6)),((_tmpEB6[0]=((_tmpEB5.tag=1,((_tmpEB5.f1=_tmp7C6,
_tmpEB5)))),_tmpEB6))))),_tmpEB4))));}_tmp783=_tmp7C6;goto _LL4E2;}else{cvtenv.free_evars=
Cyc_Tcutil_add_free_evar(cvtenv.r,cvtenv.free_evars,t,put_in_effect);}}goto
_LL4DC;_LL4E1: if(*((int*)_tmp77E)!= 1)goto _LL4E3;_tmp783=((struct Cyc_Absyn_VarType_struct*)
_tmp77E)->f1;_LL4E2:{void*_tmp7CC=Cyc_Absyn_compress_kb((void*)_tmp783->kind);
struct Cyc_Core_Opt*_tmp7CD;struct Cyc_Core_Opt**_tmp7CE;_LL512: if(*((int*)_tmp7CC)
!= 1)goto _LL514;_tmp7CD=((struct Cyc_Absyn_Unknown_kb_struct*)_tmp7CC)->f1;
_tmp7CE=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)_tmp7CC)->f1;
_LL513:{struct Cyc_Absyn_Less_kb_struct*_tmpEBC;struct Cyc_Absyn_Less_kb_struct
_tmpEBB;struct Cyc_Core_Opt*_tmpEBA;*_tmp7CE=((_tmpEBA=_cycalloc(sizeof(*_tmpEBA)),((
_tmpEBA->v=(void*)((void*)((_tmpEBC=_cycalloc(sizeof(*_tmpEBC)),((_tmpEBC[0]=((
_tmpEBB.tag=2,((_tmpEBB.f1=0,((_tmpEBB.f2=(void*)expected_kind,_tmpEBB)))))),
_tmpEBC))))),_tmpEBA))));}goto _LL511;_LL514:;_LL515: goto _LL511;_LL511:;}cvtenv.kind_env=
Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmp783);cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(
cvtenv.r,cvtenv.free_vars,_tmp783,put_in_effect);{void*_tmp7D2=Cyc_Absyn_compress_kb((
void*)_tmp783->kind);struct Cyc_Core_Opt*_tmp7D3;struct Cyc_Core_Opt**_tmp7D4;void*
_tmp7D5;_LL517: if(*((int*)_tmp7D2)!= 2)goto _LL519;_tmp7D3=((struct Cyc_Absyn_Less_kb_struct*)
_tmp7D2)->f1;_tmp7D4=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp7D2)->f1;_tmp7D5=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp7D2)->f2;
_LL518: if(Cyc_Tcutil_kind_leq(expected_kind,_tmp7D5)){struct Cyc_Absyn_Less_kb_struct*
_tmpEC2;struct Cyc_Absyn_Less_kb_struct _tmpEC1;struct Cyc_Core_Opt*_tmpEC0;*
_tmp7D4=((_tmpEC0=_cycalloc(sizeof(*_tmpEC0)),((_tmpEC0->v=(void*)((void*)((
_tmpEC2=_cycalloc(sizeof(*_tmpEC2)),((_tmpEC2[0]=((_tmpEC1.tag=2,((_tmpEC1.f1=0,((
_tmpEC1.f2=(void*)expected_kind,_tmpEC1)))))),_tmpEC2))))),_tmpEC0))));}goto
_LL516;_LL519:;_LL51A: goto _LL516;_LL516:;}goto _LL4DC;_LL4E3: if(*((int*)_tmp77E)
!= 13)goto _LL4E5;_tmp784=((struct Cyc_Absyn_AnonEnumType_struct*)_tmp77E)->f1;
_LL4E4: {struct Cyc_Tcenv_Genv*ge=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,
struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);struct _RegionHandle*
_tmp7D9=Cyc_Tcenv_get_fnrgn(te);{struct Cyc_List_List*prev_fields=0;unsigned int
tag_count=0;for(0;_tmp784 != 0;_tmp784=_tmp784->tl){struct Cyc_Absyn_Enumfield*
_tmp7DA=(struct Cyc_Absyn_Enumfield*)_tmp784->hd;if(((int(*)(int(*compare)(struct
_dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct
_dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,(*_tmp7DA->name).f2)){
const char*_tmpEC6;void*_tmpEC5[1];struct Cyc_String_pa_struct _tmpEC4;(_tmpEC4.tag=
0,((_tmpEC4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp7DA->name).f2),((
_tmpEC5[0]=& _tmpEC4,Cyc_Tcutil_terr(_tmp7DA->loc,((_tmpEC6="duplicate enum field name %s",
_tag_dyneither(_tmpEC6,sizeof(char),29))),_tag_dyneither(_tmpEC5,sizeof(void*),1)))))));}
else{struct Cyc_List_List*_tmpEC7;prev_fields=((_tmpEC7=_region_malloc(_tmp7D9,
sizeof(*_tmpEC7)),((_tmpEC7->hd=(*_tmp7DA->name).f2,((_tmpEC7->tl=prev_fields,
_tmpEC7))))));}if(_tmp7DA->tag == 0)_tmp7DA->tag=(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(
tag_count,_tmp7DA->loc);else{if(!Cyc_Tcutil_is_const_exp(te,(struct Cyc_Absyn_Exp*)
_check_null(_tmp7DA->tag))){const char*_tmpECB;void*_tmpECA[1];struct Cyc_String_pa_struct
_tmpEC9;(_tmpEC9.tag=0,((_tmpEC9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*
_tmp7DA->name).f2),((_tmpECA[0]=& _tmpEC9,Cyc_Tcutil_terr(loc,((_tmpECB="enum field %s: expression is not constant",
_tag_dyneither(_tmpECB,sizeof(char),42))),_tag_dyneither(_tmpECA,sizeof(void*),1)))))));}}{
unsigned int t1=(Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)_check_null(
_tmp7DA->tag))).f1;tag_count=t1 + 1;{union Cyc_Absyn_Nmspace_union _tmpECC;(*
_tmp7DA->name).f1=(union Cyc_Absyn_Nmspace_union)(((_tmpECC.Abs_n).tag=2,(((
_tmpECC.Abs_n).f1=te->ns,_tmpECC))));}{struct Cyc_Tcenv_AnonEnumRes_struct*
_tmpED2;struct Cyc_Tcenv_AnonEnumRes_struct _tmpED1;struct _tuple19*_tmpED0;ge->ordinaries=((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct
_tuple19*v))Cyc_Dict_insert)(ge->ordinaries,(*_tmp7DA->name).f2,(struct _tuple19*)((
_tmpED0=_cycalloc(sizeof(*_tmpED0)),((_tmpED0->f1=(void*)((_tmpED2=_cycalloc(
sizeof(*_tmpED2)),((_tmpED2[0]=((_tmpED1.tag=4,((_tmpED1.f1=(void*)t,((_tmpED1.f2=
_tmp7DA,_tmpED1)))))),_tmpED2)))),((_tmpED0->f2=1,_tmpED0)))))));}}}}goto _LL4DC;}
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
struct Cyc_Absyn_Enumdecl*_tmpED3;struct Cyc_Absyn_Enumdecl*_tmp7EC=(_tmpED3=
_cycalloc(sizeof(*_tmpED3)),((_tmpED3->sc=(void*)((void*)3),((_tmpED3->name=
_tmp785,((_tmpED3->fields=0,_tmpED3)))))));Cyc_Tc_tcEnumdecl(te,_tmp7EB,loc,
_tmp7EC);{struct Cyc_Absyn_Enumdecl**ed=Cyc_Tcenv_lookup_enumdecl(te,loc,_tmp785);*
_tmp787=(struct Cyc_Absyn_Enumdecl*)*ed;goto _LL51B;}}_LL51E:;_LL51F:(void)_throw(
_tmp7EA);_LL51B:;}}}{struct Cyc_Absyn_Enumdecl*ed=(struct Cyc_Absyn_Enumdecl*)
_check_null(*_tmp787);*_tmp785=(ed->name)[_check_known_subscript_notnull(1,0)];
goto _LL4DC;}_LL4E7: if(*((int*)_tmp77E)!= 2)goto _LL4E9;_tmp788=((struct Cyc_Absyn_DatatypeType_struct*)
_tmp77E)->f1;_tmp789=_tmp788.datatype_info;_tmp78A=(union Cyc_Absyn_DatatypeInfoU_union*)&(((
struct Cyc_Absyn_DatatypeType_struct*)_tmp77E)->f1).datatype_info;_tmp78B=_tmp788.targs;
_tmp78C=(struct Cyc_List_List**)&(((struct Cyc_Absyn_DatatypeType_struct*)_tmp77E)->f1).targs;
_tmp78D=_tmp788.rgn;_tmp78E=(struct Cyc_Core_Opt**)&(((struct Cyc_Absyn_DatatypeType_struct*)
_tmp77E)->f1).rgn;_LL4E8: {struct Cyc_List_List*_tmp7EE=*_tmp78C;{union Cyc_Absyn_DatatypeInfoU_union
_tmp7EF=*_tmp78A;struct Cyc_Absyn_UnknownDatatypeInfo _tmp7F0;struct _tuple2*
_tmp7F1;int _tmp7F2;int _tmp7F3;struct Cyc_Absyn_Datatypedecl**_tmp7F4;struct Cyc_Absyn_Datatypedecl*
_tmp7F5;_LL521: if((_tmp7EF.UnknownDatatype).tag != 0)goto _LL523;_tmp7F0=(_tmp7EF.UnknownDatatype).f1;
_tmp7F1=_tmp7F0.name;_tmp7F2=_tmp7F0.is_extensible;_tmp7F3=_tmp7F0.is_flat;
_LL522: {struct Cyc_Absyn_Datatypedecl**tudp;{struct _handler_cons _tmp7F6;
_push_handler(& _tmp7F6);{int _tmp7F8=0;if(setjmp(_tmp7F6.handler))_tmp7F8=1;if(!
_tmp7F8){tudp=Cyc_Tcenv_lookup_datatypedecl(te,loc,_tmp7F1);;_pop_handler();}
else{void*_tmp7F7=(void*)_exn_thrown;void*_tmp7FA=_tmp7F7;_LL526: if(_tmp7FA != 
Cyc_Dict_Absent)goto _LL528;_LL527: {struct Cyc_Tcenv_Genv*_tmp7FB=((struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct Cyc_Absyn_Datatypedecl*_tmpED4;struct Cyc_Absyn_Datatypedecl*_tmp7FC=(
_tmpED4=_cycalloc(sizeof(*_tmpED4)),((_tmpED4->sc=(void*)((void*)3),((_tmpED4->name=
_tmp7F1,((_tmpED4->tvs=0,((_tmpED4->fields=0,((_tmpED4->is_extensible=_tmp7F2,((
_tmpED4->is_flat=_tmp7F3,_tmpED4)))))))))))));Cyc_Tc_tcDatatypedecl(te,_tmp7FB,
loc,_tmp7FC);tudp=Cyc_Tcenv_lookup_datatypedecl(te,loc,_tmp7F1);if(_tmp7EE != 0){{
const char*_tmpED8;void*_tmpED7[1];struct Cyc_String_pa_struct _tmpED6;(_tmpED6.tag=
0,((_tmpED6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp7F1)),((_tmpED7[0]=& _tmpED6,Cyc_Tcutil_terr(loc,((_tmpED8="declare parameterized datatype %s before using",
_tag_dyneither(_tmpED8,sizeof(char),47))),_tag_dyneither(_tmpED7,sizeof(void*),1)))))));}
return cvtenv;}goto _LL525;}_LL528:;_LL529:(void)_throw(_tmp7FA);_LL525:;}}}if(
_tmp7F2  && !(*tudp)->is_extensible){const char*_tmpEDC;void*_tmpEDB[1];struct Cyc_String_pa_struct
_tmpEDA;(_tmpEDA.tag=0,((_tmpEDA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp7F1)),((_tmpEDB[0]=& _tmpEDA,Cyc_Tcutil_terr(loc,((
_tmpEDC="datatype %s was not declared @extensible",_tag_dyneither(_tmpEDC,
sizeof(char),41))),_tag_dyneither(_tmpEDB,sizeof(void*),1)))))));}{union Cyc_Absyn_DatatypeInfoU_union
_tmpEDD;*_tmp78A=(union Cyc_Absyn_DatatypeInfoU_union)(((_tmpEDD.KnownDatatype).tag=
1,(((_tmpEDD.KnownDatatype).f1=tudp,_tmpEDD))));}_tmp7F5=*tudp;goto _LL524;}
_LL523: if((_tmp7EF.KnownDatatype).tag != 1)goto _LL520;_tmp7F4=(_tmp7EF.KnownDatatype).f1;
_tmp7F5=*_tmp7F4;_LL524: if(_tmp7F5->is_flat  && (unsigned int)*_tmp78E){const char*
_tmpEE1;void*_tmpEE0[1];struct Cyc_String_pa_struct _tmpEDF;(_tmpEDF.tag=0,((
_tmpEDF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(*_tmp78E))->v)),((_tmpEE0[0]=& _tmpEDF,
Cyc_Tcutil_terr(loc,((_tmpEE1="flat datatype has region %s",_tag_dyneither(
_tmpEE1,sizeof(char),28))),_tag_dyneither(_tmpEE0,sizeof(void*),1)))))));}if(!
_tmp7F5->is_flat  && !((unsigned int)*_tmp78E)){struct Cyc_Core_Opt*_tmpEE2;*
_tmp78E=((_tmpEE2=_cycalloc(sizeof(*_tmpEE2)),((_tmpEE2->v=(void*)Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,0),_tmpEE2))));}if((unsigned int)*_tmp78E){
void*_tmp809=(void*)((struct Cyc_Core_Opt*)_check_null(*_tmp78E))->v;cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,(void*)3,_tmp809,1);}{struct Cyc_List_List*tvs=_tmp7F5->tvs;for(0;
_tmp7EE != 0  && tvs != 0;(_tmp7EE=_tmp7EE->tl,tvs=tvs->tl)){void*t1=(void*)_tmp7EE->hd;
void*k1=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd);cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,k1,t1,1);}if(_tmp7EE != 0){const char*_tmpEE6;void*_tmpEE5[1];struct
Cyc_String_pa_struct _tmpEE4;(_tmpEE4.tag=0,((_tmpEE4.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp7F5->name)),((_tmpEE5[0]=&
_tmpEE4,Cyc_Tcutil_terr(loc,((_tmpEE6="too many type arguments for datatype %s",
_tag_dyneither(_tmpEE6,sizeof(char),40))),_tag_dyneither(_tmpEE5,sizeof(void*),1)))))));}
if(tvs != 0){struct Cyc_List_List*hidden_ts=0;for(0;tvs != 0;tvs=tvs->tl){void*k1=
Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd);void*e=Cyc_Absyn_new_evar(0,
0);{struct Cyc_List_List*_tmpEE7;hidden_ts=((_tmpEE7=_cycalloc(sizeof(*_tmpEE7)),((
_tmpEE7->hd=(void*)e,((_tmpEE7->tl=hidden_ts,_tmpEE7))))));}cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,k1,e,1);}*_tmp78C=Cyc_List_imp_append(*_tmp78C,Cyc_List_imp_rev(
hidden_ts));}goto _LL520;}_LL520:;}goto _LL4DC;}_LL4E9: if(*((int*)_tmp77E)!= 3)
goto _LL4EB;_tmp78F=((struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp77E)->f1;
_tmp790=_tmp78F.field_info;_tmp791=(union Cyc_Absyn_DatatypeFieldInfoU_union*)&(((
struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp77E)->f1).field_info;_tmp792=
_tmp78F.targs;_LL4EA:{union Cyc_Absyn_DatatypeFieldInfoU_union _tmp80E=*_tmp791;
struct Cyc_Absyn_UnknownDatatypeFieldInfo _tmp80F;struct _tuple2*_tmp810;struct
_tuple2*_tmp811;int _tmp812;struct Cyc_Absyn_Datatypedecl*_tmp813;struct Cyc_Absyn_Datatypefield*
_tmp814;_LL52B: if((_tmp80E.UnknownDatatypefield).tag != 0)goto _LL52D;_tmp80F=(
_tmp80E.UnknownDatatypefield).f1;_tmp810=_tmp80F.datatype_name;_tmp811=_tmp80F.field_name;
_tmp812=_tmp80F.is_extensible;_LL52C: {struct Cyc_Absyn_Datatypedecl*tud;struct
Cyc_Absyn_Datatypefield*tuf;{struct _handler_cons _tmp815;_push_handler(& _tmp815);{
int _tmp817=0;if(setjmp(_tmp815.handler))_tmp817=1;if(!_tmp817){*Cyc_Tcenv_lookup_datatypedecl(
te,loc,_tmp810);;_pop_handler();}else{void*_tmp816=(void*)_exn_thrown;void*
_tmp819=_tmp816;_LL530: if(_tmp819 != Cyc_Dict_Absent)goto _LL532;_LL531:{const char*
_tmpEEB;void*_tmpEEA[1];struct Cyc_String_pa_struct _tmpEE9;(_tmpEE9.tag=0,((
_tmpEE9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp810)),((_tmpEEA[0]=& _tmpEE9,Cyc_Tcutil_terr(loc,((_tmpEEB="unbound datatype %s",
_tag_dyneither(_tmpEEB,sizeof(char),20))),_tag_dyneither(_tmpEEA,sizeof(void*),1)))))));}
return cvtenv;_LL532:;_LL533:(void)_throw(_tmp819);_LL52F:;}}}{struct
_handler_cons _tmp81D;_push_handler(& _tmp81D);{int _tmp81F=0;if(setjmp(_tmp81D.handler))
_tmp81F=1;if(!_tmp81F){{struct _RegionHandle*_tmp820=Cyc_Tcenv_get_fnrgn(te);void*
_tmp821=Cyc_Tcenv_lookup_ordinary(_tmp820,te,loc,_tmp811);struct Cyc_Absyn_Datatypedecl*
_tmp822;struct Cyc_Absyn_Datatypefield*_tmp823;_LL535: if(*((int*)_tmp821)!= 2)
goto _LL537;_tmp822=((struct Cyc_Tcenv_DatatypeRes_struct*)_tmp821)->f1;_tmp823=((
struct Cyc_Tcenv_DatatypeRes_struct*)_tmp821)->f2;_LL536: tuf=_tmp823;tud=_tmp822;
if(_tmp812  && !tud->is_extensible){const char*_tmpEEF;void*_tmpEEE[1];struct Cyc_String_pa_struct
_tmpEED;(_tmpEED.tag=0,((_tmpEED.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp810)),((_tmpEEE[0]=& _tmpEED,Cyc_Tcutil_terr(loc,((
_tmpEEF="datatype %s was not declared @extensible",_tag_dyneither(_tmpEEF,
sizeof(char),41))),_tag_dyneither(_tmpEEE,sizeof(void*),1)))))));}goto _LL534;
_LL537:;_LL538:{const char*_tmpEF4;void*_tmpEF3[2];struct Cyc_String_pa_struct
_tmpEF2;struct Cyc_String_pa_struct _tmpEF1;(_tmpEF1.tag=0,((_tmpEF1.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp810)),((
_tmpEF2.tag=0,((_tmpEF2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp811)),((_tmpEF3[0]=& _tmpEF2,((_tmpEF3[1]=& _tmpEF1,Cyc_Tcutil_terr(loc,((
_tmpEF4="unbound field %s in type datatype %s",_tag_dyneither(_tmpEF4,sizeof(
char),37))),_tag_dyneither(_tmpEF3,sizeof(void*),2)))))))))))));}{struct Cyc_Tcutil_CVTEnv
_tmp82B=cvtenv;_npop_handler(0);return _tmp82B;}_LL534:;};_pop_handler();}else{
void*_tmp81E=(void*)_exn_thrown;void*_tmp82D=_tmp81E;_LL53A: if(_tmp82D != Cyc_Dict_Absent)
goto _LL53C;_LL53B:{const char*_tmpEF9;void*_tmpEF8[2];struct Cyc_String_pa_struct
_tmpEF7;struct Cyc_String_pa_struct _tmpEF6;(_tmpEF6.tag=0,((_tmpEF6.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp810)),((
_tmpEF7.tag=0,((_tmpEF7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp811)),((_tmpEF8[0]=& _tmpEF7,((_tmpEF8[1]=& _tmpEF6,Cyc_Tcutil_terr(loc,((
_tmpEF9="unbound field %s in type datatype %s",_tag_dyneither(_tmpEF9,sizeof(
char),37))),_tag_dyneither(_tmpEF8,sizeof(void*),2)))))))))))));}return cvtenv;
_LL53C:;_LL53D:(void)_throw(_tmp82D);_LL539:;}}}{union Cyc_Absyn_DatatypeFieldInfoU_union
_tmpEFA;*_tmp791=(union Cyc_Absyn_DatatypeFieldInfoU_union)(((_tmpEFA.KnownDatatypefield).tag=
1,(((_tmpEFA.KnownDatatypefield).f1=tud,(((_tmpEFA.KnownDatatypefield).f2=tuf,
_tmpEFA))))));}_tmp813=tud;_tmp814=tuf;goto _LL52E;}_LL52D: if((_tmp80E.KnownDatatypefield).tag
!= 1)goto _LL52A;_tmp813=(_tmp80E.KnownDatatypefield).f1;_tmp814=(_tmp80E.KnownDatatypefield).f2;
_LL52E: {struct Cyc_List_List*tvs=_tmp813->tvs;for(0;_tmp792 != 0  && tvs != 0;(
_tmp792=_tmp792->tl,tvs=tvs->tl)){void*t1=(void*)_tmp792->hd;void*k1=Cyc_Tcutil_tvar_kind((
struct Cyc_Absyn_Tvar*)tvs->hd);cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,
cvtenv,k1,t1,1);}if(_tmp792 != 0){const char*_tmpEFF;void*_tmpEFE[2];struct Cyc_String_pa_struct
_tmpEFD;struct Cyc_String_pa_struct _tmpEFC;(_tmpEFC.tag=0,((_tmpEFC.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp814->name)),((
_tmpEFD.tag=0,((_tmpEFD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp813->name)),((_tmpEFE[0]=& _tmpEFD,((_tmpEFE[1]=& _tmpEFC,Cyc_Tcutil_terr(loc,((
_tmpEFF="too many type arguments for datatype %s.%s",_tag_dyneither(_tmpEFF,
sizeof(char),43))),_tag_dyneither(_tmpEFE,sizeof(void*),2)))))))))))));}if(tvs != 
0){const char*_tmpF04;void*_tmpF03[2];struct Cyc_String_pa_struct _tmpF02;struct Cyc_String_pa_struct
_tmpF01;(_tmpF01.tag=0,((_tmpF01.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp814->name)),((_tmpF02.tag=0,((_tmpF02.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp813->name)),((
_tmpF03[0]=& _tmpF02,((_tmpF03[1]=& _tmpF01,Cyc_Tcutil_terr(loc,((_tmpF04="too few type arguments for datatype %s.%s",
_tag_dyneither(_tmpF04,sizeof(char),42))),_tag_dyneither(_tmpF03,sizeof(void*),2)))))))))))));}
goto _LL52A;}_LL52A:;}goto _LL4DC;_LL4EB: if(*((int*)_tmp77E)!= 4)goto _LL4ED;
_tmp793=((struct Cyc_Absyn_PointerType_struct*)_tmp77E)->f1;_tmp794=(void*)
_tmp793.elt_typ;_tmp795=_tmp793.elt_tq;_tmp796=(struct Cyc_Absyn_Tqual*)&(((
struct Cyc_Absyn_PointerType_struct*)_tmp77E)->f1).elt_tq;_tmp797=_tmp793.ptr_atts;
_tmp798=(void*)_tmp797.rgn;_tmp799=_tmp797.nullable;_tmp79A=_tmp797.bounds;
_tmp79B=_tmp797.zero_term;_LL4EC: {int is_zero_terminated;cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,(void*)0,_tmp794,1);_tmp796->real_const=Cyc_Tcutil_extract_const_from_typedef(
loc,_tmp796->print_const,_tmp794);cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,
cvtenv,(void*)5,_tmp798,1);{union Cyc_Absyn_Constraint_union _tmp83B=(((struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref*x))Cyc_Absyn_compress_conref)(_tmp79B))->v;int _tmp83C;
_LL53F: if((_tmp83B.No_constr).tag != 2)goto _LL541;_LL540:{void*_tmp83D=Cyc_Tcutil_compress(
_tmp794);void*_tmp83E;_LL546: if(_tmp83D <= (void*)4)goto _LL548;if(*((int*)_tmp83D)
!= 5)goto _LL548;_tmp83E=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp83D)->f2;
if((int)_tmp83E != 0)goto _LL548;_LL547:((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*
x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp79B,
Cyc_Absyn_true_conref);is_zero_terminated=1;goto _LL545;_LL548:;_LL549:((int(*)(
int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(
Cyc_Core_intcmp,_tmp79B,Cyc_Absyn_false_conref);is_zero_terminated=0;goto _LL545;
_LL545:;}goto _LL53E;_LL541: if((_tmp83B.Eq_constr).tag != 0)goto _LL543;_tmp83C=(
_tmp83B.Eq_constr).f1;if(_tmp83C != 1)goto _LL543;_LL542: if(!Cyc_Tcutil_admits_zero(
_tmp794)){const char*_tmpF08;void*_tmpF07[1];struct Cyc_String_pa_struct _tmpF06;(
_tmpF06.tag=0,((_tmpF06.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
_tmp794)),((_tmpF07[0]=& _tmpF06,Cyc_Tcutil_terr(loc,((_tmpF08="cannot have a pointer to zero-terminated %s elements",
_tag_dyneither(_tmpF08,sizeof(char),53))),_tag_dyneither(_tmpF07,sizeof(void*),1)))))));}
is_zero_terminated=1;goto _LL53E;_LL543:;_LL544: is_zero_terminated=0;goto _LL53E;
_LL53E:;}{union Cyc_Absyn_Constraint_union _tmp842=(Cyc_Absyn_compress_conref(
_tmp79A))->v;void*_tmp843;void*_tmp844;struct Cyc_Absyn_Exp*_tmp845;_LL54B: if((
_tmp842.No_constr).tag != 2)goto _LL54D;_LL54C: goto _LL54E;_LL54D: if((_tmp842.Eq_constr).tag
!= 0)goto _LL54F;_tmp843=(_tmp842.Eq_constr).f1;if((int)_tmp843 != 0)goto _LL54F;
_LL54E: goto _LL54A;_LL54F: if((_tmp842.Eq_constr).tag != 0)goto _LL551;_tmp844=(
_tmp842.Eq_constr).f1;if(_tmp844 <= (void*)1)goto _LL551;if(*((int*)_tmp844)!= 0)
goto _LL551;_tmp845=((struct Cyc_Absyn_Upper_b_struct*)_tmp844)->f1;_LL550: {
struct _RegionHandle*_tmp846=Cyc_Tcenv_get_fnrgn(te);{struct Cyc_Tcenv_Tenv*
_tmp847=Cyc_Tcenv_allow_valueof(_tmp846,te);Cyc_Tcexp_tcExp(_tmp847,0,_tmp845);}
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp845,te,cvtenv);if(!Cyc_Tcutil_coerce_uint_typ(
te,_tmp845)){const char*_tmpF0B;void*_tmpF0A;(_tmpF0A=0,Cyc_Tcutil_terr(loc,((
_tmpF0B="pointer bounds expression is not an unsigned int",_tag_dyneither(
_tmpF0B,sizeof(char),49))),_tag_dyneither(_tmpF0A,sizeof(void*),0)));}{
unsigned int _tmp84B;int _tmp84C;struct _tuple7 _tmp84A=Cyc_Evexp_eval_const_uint_exp(
_tmp845);_tmp84B=_tmp84A.f1;_tmp84C=_tmp84A.f2;if(is_zero_terminated  && (!
_tmp84C  || _tmp84B < 1)){const char*_tmpF0E;void*_tmpF0D;(_tmpF0D=0,Cyc_Tcutil_terr(
loc,((_tmpF0E="zero-terminated pointer cannot point to empty sequence",
_tag_dyneither(_tmpF0E,sizeof(char),55))),_tag_dyneither(_tmpF0D,sizeof(void*),0)));}
goto _LL54A;}}_LL551: if((_tmp842.Forward_constr).tag != 1)goto _LL54A;_LL552: {
const char*_tmpF11;void*_tmpF10;(_tmpF10=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpF11="forward constraint",
_tag_dyneither(_tmpF11,sizeof(char),19))),_tag_dyneither(_tmpF10,sizeof(void*),0)));}
_LL54A:;}goto _LL4DC;}_LL4ED: if(*((int*)_tmp77E)!= 18)goto _LL4EF;_tmp79C=(void*)((
struct Cyc_Absyn_TagType_struct*)_tmp77E)->f1;_LL4EE: cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,(void*)7,_tmp79C,1);goto _LL4DC;_LL4EF: if(*((int*)_tmp77E)!= 17)
goto _LL4F1;_tmp79D=((struct Cyc_Absyn_ValueofType_struct*)_tmp77E)->f1;_LL4F0: {
struct _RegionHandle*_tmp851=Cyc_Tcenv_get_fnrgn(te);{struct Cyc_Tcenv_Tenv*
_tmp852=Cyc_Tcenv_allow_valueof(_tmp851,te);Cyc_Tcexp_tcExp(_tmp852,0,_tmp79D);}
if(!Cyc_Tcutil_coerce_uint_typ(te,_tmp79D)){const char*_tmpF14;void*_tmpF13;(
_tmpF13=0,Cyc_Tcutil_terr(loc,((_tmpF14="valueof_t requires an int expression",
_tag_dyneither(_tmpF14,sizeof(char),37))),_tag_dyneither(_tmpF13,sizeof(void*),0)));}
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp79D,te,cvtenv);goto _LL4DC;}
_LL4F1: if(*((int*)_tmp77E)!= 5)goto _LL4F3;_LL4F2: goto _LL4F4;_LL4F3: if((int)
_tmp77E != 1)goto _LL4F5;_LL4F4: goto _LL4F6;_LL4F5: if(_tmp77E <= (void*)4)goto _LL503;
if(*((int*)_tmp77E)!= 6)goto _LL4F7;_LL4F6: goto _LL4DC;_LL4F7: if(*((int*)_tmp77E)
!= 7)goto _LL4F9;_tmp79E=((struct Cyc_Absyn_ArrayType_struct*)_tmp77E)->f1;_tmp79F=(
void*)_tmp79E.elt_type;_tmp7A0=_tmp79E.tq;_tmp7A1=(struct Cyc_Absyn_Tqual*)&(((
struct Cyc_Absyn_ArrayType_struct*)_tmp77E)->f1).tq;_tmp7A2=_tmp79E.num_elts;
_tmp7A3=(struct Cyc_Absyn_Exp**)&(((struct Cyc_Absyn_ArrayType_struct*)_tmp77E)->f1).num_elts;
_tmp7A4=_tmp79E.zero_term;_tmp7A5=_tmp79E.zt_loc;_LL4F8: {struct Cyc_Absyn_Exp*
_tmp855=*_tmp7A3;cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)1,
_tmp79F,1);_tmp7A1->real_const=Cyc_Tcutil_extract_const_from_typedef(loc,_tmp7A1->print_const,
_tmp79F);{int is_zero_terminated;{union Cyc_Absyn_Constraint_union _tmp856=(((
struct Cyc_Absyn_Conref*(*)(struct Cyc_Absyn_Conref*x))Cyc_Absyn_compress_conref)(
_tmp7A4))->v;int _tmp857;_LL554: if((_tmp856.No_constr).tag != 2)goto _LL556;_LL555:((
int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(
Cyc_Core_intcmp,_tmp7A4,Cyc_Absyn_false_conref);is_zero_terminated=0;goto _LL553;
_LL556: if((_tmp856.Eq_constr).tag != 0)goto _LL558;_tmp857=(_tmp856.Eq_constr).f1;
if(_tmp857 != 1)goto _LL558;_LL557: if(!Cyc_Tcutil_admits_zero(_tmp79F)){const char*
_tmpF18;void*_tmpF17[1];struct Cyc_String_pa_struct _tmpF16;(_tmpF16.tag=0,((
_tmpF16.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
_tmp79F)),((_tmpF17[0]=& _tmpF16,Cyc_Tcutil_terr(loc,((_tmpF18="cannot have a zero-terminated array of %s elements",
_tag_dyneither(_tmpF18,sizeof(char),51))),_tag_dyneither(_tmpF17,sizeof(void*),1)))))));}
is_zero_terminated=1;goto _LL553;_LL558:;_LL559: is_zero_terminated=0;goto _LL553;
_LL553:;}if(_tmp855 == 0){if(is_zero_terminated)*_tmp7A3=(_tmp855=(struct Cyc_Absyn_Exp*)
Cyc_Absyn_uint_exp(1,0));else{{const char*_tmpF1B;void*_tmpF1A;(_tmpF1A=0,Cyc_Tcutil_warn(
loc,((_tmpF1B="array bound defaults to 1 here",_tag_dyneither(_tmpF1B,sizeof(
char),31))),_tag_dyneither(_tmpF1A,sizeof(void*),0)));}*_tmp7A3=(_tmp855=(struct
Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(1,0));}}Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)
_tmp855);if(!Cyc_Tcutil_is_const_exp(te,(struct Cyc_Absyn_Exp*)_tmp855)){const
char*_tmpF1E;void*_tmpF1D;(_tmpF1D=0,Cyc_Tcutil_terr(loc,((_tmpF1E="array bounds expression is not constant",
_tag_dyneither(_tmpF1E,sizeof(char),40))),_tag_dyneither(_tmpF1D,sizeof(void*),0)));}
if(!Cyc_Tcutil_coerce_uint_typ(te,(struct Cyc_Absyn_Exp*)_tmp855)){const char*
_tmpF21;void*_tmpF20;(_tmpF20=0,Cyc_Tcutil_terr(loc,((_tmpF21="array bounds expression is not an unsigned int",
_tag_dyneither(_tmpF21,sizeof(char),47))),_tag_dyneither(_tmpF20,sizeof(void*),0)));}{
unsigned int _tmp862;int _tmp863;struct _tuple7 _tmp861=Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)_tmp855);_tmp862=_tmp861.f1;_tmp863=_tmp861.f2;if((
is_zero_terminated  && _tmp863) && _tmp862 < 1){const char*_tmpF24;void*_tmpF23;(
_tmpF23=0,Cyc_Tcutil_warn(loc,((_tmpF24="zero terminated array cannot have zero elements",
_tag_dyneither(_tmpF24,sizeof(char),48))),_tag_dyneither(_tmpF23,sizeof(void*),0)));}
if((_tmp863  && _tmp862 < 1) && Cyc_Cyclone_tovc_r){{const char*_tmpF27;void*
_tmpF26;(_tmpF26=0,Cyc_Tcutil_warn(loc,((_tmpF27="arrays with 0 elements are not supported except with gcc -- changing to 1.",
_tag_dyneither(_tmpF27,sizeof(char),75))),_tag_dyneither(_tmpF26,sizeof(void*),0)));}*
_tmp7A3=(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(1,0);}goto _LL4DC;}}}_LL4F9: if(*((
int*)_tmp77E)!= 8)goto _LL4FB;_tmp7A6=((struct Cyc_Absyn_FnType_struct*)_tmp77E)->f1;
_tmp7A7=_tmp7A6.tvars;_tmp7A8=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_struct*)
_tmp77E)->f1).tvars;_tmp7A9=_tmp7A6.effect;_tmp7AA=(struct Cyc_Core_Opt**)&(((
struct Cyc_Absyn_FnType_struct*)_tmp77E)->f1).effect;_tmp7AB=(void*)_tmp7A6.ret_typ;
_tmp7AC=_tmp7A6.args;_tmp7AD=_tmp7A6.c_varargs;_tmp7AE=_tmp7A6.cyc_varargs;
_tmp7AF=_tmp7A6.rgn_po;_tmp7B0=_tmp7A6.attributes;_LL4FA: {int num_convs=0;int
seen_cdecl=0;int seen_stdcall=0;int seen_fastcall=0;int seen_format=0;void*ft=(void*)
0;int fmt_desc_arg=- 1;int fmt_arg_start=- 1;for(0;_tmp7B0 != 0;_tmp7B0=_tmp7B0->tl){
if(!Cyc_Absyn_fntype_att((void*)_tmp7B0->hd)){const char*_tmpF2B;void*_tmpF2A[1];
struct Cyc_String_pa_struct _tmpF29;(_tmpF29.tag=0,((_tmpF29.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((void*)_tmp7B0->hd)),((
_tmpF2A[0]=& _tmpF29,Cyc_Tcutil_terr(loc,((_tmpF2B="bad function type attribute %s",
_tag_dyneither(_tmpF2B,sizeof(char),31))),_tag_dyneither(_tmpF2A,sizeof(void*),1)))))));}{
void*_tmp86B=(void*)_tmp7B0->hd;void*_tmp86C;int _tmp86D;int _tmp86E;_LL55B: if((
int)_tmp86B != 0)goto _LL55D;_LL55C: if(!seen_stdcall){seen_stdcall=1;++ num_convs;}
goto _LL55A;_LL55D: if((int)_tmp86B != 1)goto _LL55F;_LL55E: if(!seen_cdecl){
seen_cdecl=1;++ num_convs;}goto _LL55A;_LL55F: if((int)_tmp86B != 2)goto _LL561;
_LL560: if(!seen_fastcall){seen_fastcall=1;++ num_convs;}goto _LL55A;_LL561: if(
_tmp86B <= (void*)17)goto _LL563;if(*((int*)_tmp86B)!= 3)goto _LL563;_tmp86C=(void*)((
struct Cyc_Absyn_Format_att_struct*)_tmp86B)->f1;_tmp86D=((struct Cyc_Absyn_Format_att_struct*)
_tmp86B)->f2;_tmp86E=((struct Cyc_Absyn_Format_att_struct*)_tmp86B)->f3;_LL562:
if(!seen_format){seen_format=1;ft=_tmp86C;fmt_desc_arg=_tmp86D;fmt_arg_start=
_tmp86E;}else{const char*_tmpF2E;void*_tmpF2D;(_tmpF2D=0,Cyc_Tcutil_terr(loc,((
_tmpF2E="function can't have multiple format attributes",_tag_dyneither(_tmpF2E,
sizeof(char),47))),_tag_dyneither(_tmpF2D,sizeof(void*),0)));}goto _LL55A;_LL563:;
_LL564: goto _LL55A;_LL55A:;}}if(num_convs > 1){const char*_tmpF31;void*_tmpF30;(
_tmpF30=0,Cyc_Tcutil_terr(loc,((_tmpF31="function can't have multiple calling conventions",
_tag_dyneither(_tmpF31,sizeof(char),49))),_tag_dyneither(_tmpF30,sizeof(void*),0)));}
Cyc_Tcutil_check_unique_tvars(loc,*_tmp7A8);{struct Cyc_List_List*b=*_tmp7A8;for(
0;b != 0;b=b->tl){((struct Cyc_Absyn_Tvar*)b->hd)->identity=Cyc_Tcutil_new_tvar_id();
cvtenv.kind_env=Cyc_Tcutil_add_bound_tvar(cvtenv.kind_env,(struct Cyc_Absyn_Tvar*)
b->hd);{void*_tmp873=Cyc_Absyn_compress_kb((void*)((struct Cyc_Absyn_Tvar*)b->hd)->kind);
void*_tmp874;_LL566: if(*((int*)_tmp873)!= 0)goto _LL568;_tmp874=(void*)((struct
Cyc_Absyn_Eq_kb_struct*)_tmp873)->f1;if((int)_tmp874 != 1)goto _LL568;_LL567:{
const char*_tmpF35;void*_tmpF34[1];struct Cyc_String_pa_struct _tmpF33;(_tmpF33.tag=
0,((_tmpF33.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)
b->hd)->name),((_tmpF34[0]=& _tmpF33,Cyc_Tcutil_terr(loc,((_tmpF35="function attempts to abstract Mem type variable %s",
_tag_dyneither(_tmpF35,sizeof(char),51))),_tag_dyneither(_tmpF34,sizeof(void*),1)))))));}
goto _LL565;_LL568:;_LL569: goto _LL565;_LL565:;}}}{struct _RegionHandle*_tmp878=Cyc_Tcenv_get_fnrgn(
te);{struct Cyc_Tcutil_CVTEnv _tmpF36;struct Cyc_Tcutil_CVTEnv _tmp879=(_tmpF36.r=
_tmp878,((_tmpF36.kind_env=cvtenv.kind_env,((_tmpF36.free_vars=0,((_tmpF36.free_evars=
0,((_tmpF36.generalize_evars=cvtenv.generalize_evars,((_tmpF36.fn_result=1,
_tmpF36)))))))))));_tmp879=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp879,(void*)1,
_tmp7AB,1);_tmp879.fn_result=0;{struct Cyc_List_List*a=_tmp7AC;for(0;a != 0;a=a->tl){
struct _tuple3*_tmp87A=(struct _tuple3*)a->hd;void*_tmp87B=(*_tmp87A).f3;_tmp879=
Cyc_Tcutil_i_check_valid_type(loc,te,_tmp879,(void*)1,_tmp87B,1);((*_tmp87A).f2).real_const=
Cyc_Tcutil_extract_const_from_typedef(loc,((*_tmp87A).f2).print_const,_tmp87B);}}
if(_tmp7AE != 0){if(_tmp7AD){const char*_tmpF39;void*_tmpF38;(_tmpF38=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpF39="both c_vararg and cyc_vararg",
_tag_dyneither(_tmpF39,sizeof(char),29))),_tag_dyneither(_tmpF38,sizeof(void*),0)));}{
void*_tmp87F;int _tmp880;struct Cyc_Absyn_VarargInfo _tmp87E=*_tmp7AE;_tmp87F=(void*)
_tmp87E.type;_tmp880=_tmp87E.inject;_tmp879=Cyc_Tcutil_i_check_valid_type(loc,te,
_tmp879,(void*)1,_tmp87F,1);(_tmp7AE->tq).real_const=Cyc_Tcutil_extract_const_from_typedef(
loc,(_tmp7AE->tq).print_const,_tmp87F);if(_tmp880){void*_tmp881=Cyc_Tcutil_compress(
_tmp87F);struct Cyc_Absyn_DatatypeInfo _tmp882;union Cyc_Absyn_DatatypeInfoU_union
_tmp883;struct Cyc_Absyn_Datatypedecl**_tmp884;_LL56B: if(_tmp881 <= (void*)4)goto
_LL56D;if(*((int*)_tmp881)!= 2)goto _LL56D;_tmp882=((struct Cyc_Absyn_DatatypeType_struct*)
_tmp881)->f1;_tmp883=_tmp882.datatype_info;if((((((struct Cyc_Absyn_DatatypeType_struct*)
_tmp881)->f1).datatype_info).KnownDatatype).tag != 1)goto _LL56D;_tmp884=(_tmp883.KnownDatatype).f1;
_LL56C: if((*_tmp884)->is_flat){const char*_tmpF3C;void*_tmpF3B;(_tmpF3B=0,Cyc_Tcutil_terr(
loc,((_tmpF3C="cant inject into a flat datatype",_tag_dyneither(_tmpF3C,sizeof(
char),33))),_tag_dyneither(_tmpF3B,sizeof(void*),0)));}goto _LL56A;_LL56D:;_LL56E:{
const char*_tmpF3F;void*_tmpF3E;(_tmpF3E=0,Cyc_Tcutil_terr(loc,((_tmpF3F="can't inject a non-datatype type",
_tag_dyneither(_tmpF3F,sizeof(char),33))),_tag_dyneither(_tmpF3E,sizeof(void*),0)));}
goto _LL56A;_LL56A:;}}}if(seen_format){int _tmp889=((int(*)(struct Cyc_List_List*x))
Cyc_List_length)(_tmp7AC);if((((fmt_desc_arg < 0  || fmt_desc_arg > _tmp889) || 
fmt_arg_start < 0) || _tmp7AE == 0  && fmt_arg_start != 0) || _tmp7AE != 0  && 
fmt_arg_start != _tmp889 + 1){const char*_tmpF42;void*_tmpF41;(_tmpF41=0,Cyc_Tcutil_terr(
loc,((_tmpF42="bad format descriptor",_tag_dyneither(_tmpF42,sizeof(char),22))),
_tag_dyneither(_tmpF41,sizeof(void*),0)));}else{void*_tmp88D;struct _tuple3
_tmp88C=*((struct _tuple3*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp7AC,
fmt_desc_arg - 1);_tmp88D=_tmp88C.f3;{void*_tmp88E=Cyc_Tcutil_compress(_tmp88D);
struct Cyc_Absyn_PtrInfo _tmp88F;void*_tmp890;struct Cyc_Absyn_PtrAtts _tmp891;
struct Cyc_Absyn_Conref*_tmp892;struct Cyc_Absyn_Conref*_tmp893;_LL570: if(_tmp88E
<= (void*)4)goto _LL572;if(*((int*)_tmp88E)!= 4)goto _LL572;_tmp88F=((struct Cyc_Absyn_PointerType_struct*)
_tmp88E)->f1;_tmp890=(void*)_tmp88F.elt_typ;_tmp891=_tmp88F.ptr_atts;_tmp892=
_tmp891.bounds;_tmp893=_tmp891.zero_term;_LL571:{struct _tuple0 _tmpF43;struct
_tuple0 _tmp895=(_tmpF43.f1=Cyc_Tcutil_compress(_tmp890),((_tmpF43.f2=Cyc_Absyn_conref_def((
void*)((void*)0),_tmp892),_tmpF43)));void*_tmp896;void*_tmp897;void*_tmp898;void*
_tmp899;_LL575: _tmp896=_tmp895.f1;if(_tmp896 <= (void*)4)goto _LL577;if(*((int*)
_tmp896)!= 5)goto _LL577;_tmp897=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp896)->f1;
if((int)_tmp897 != 2)goto _LL577;_tmp898=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp896)->f2;if((int)_tmp898 != 0)goto _LL577;_tmp899=_tmp895.f2;if((int)_tmp899 != 
0)goto _LL577;_LL576: goto _LL574;_LL577:;_LL578:{const char*_tmpF46;void*_tmpF45;(
_tmpF45=0,Cyc_Tcutil_terr(loc,((_tmpF46="format descriptor is not a char ? type",
_tag_dyneither(_tmpF46,sizeof(char),39))),_tag_dyneither(_tmpF45,sizeof(void*),0)));}
goto _LL574;_LL574:;}goto _LL56F;_LL572:;_LL573:{const char*_tmpF49;void*_tmpF48;(
_tmpF48=0,Cyc_Tcutil_terr(loc,((_tmpF49="format descriptor is not a char ? type",
_tag_dyneither(_tmpF49,sizeof(char),39))),_tag_dyneither(_tmpF48,sizeof(void*),0)));}
goto _LL56F;_LL56F:;}if(fmt_arg_start != 0){int problem;{struct _tuple0 _tmpF4A;
struct _tuple0 _tmp89F=(_tmpF4A.f1=ft,((_tmpF4A.f2=Cyc_Tcutil_compress((void*)((
struct Cyc_Absyn_VarargInfo*)_check_null(_tmp7AE))->type),_tmpF4A)));void*_tmp8A0;
void*_tmp8A1;struct Cyc_Absyn_DatatypeInfo _tmp8A2;union Cyc_Absyn_DatatypeInfoU_union
_tmp8A3;struct Cyc_Absyn_Datatypedecl**_tmp8A4;struct Cyc_Absyn_Datatypedecl*
_tmp8A5;void*_tmp8A6;void*_tmp8A7;struct Cyc_Absyn_DatatypeInfo _tmp8A8;union Cyc_Absyn_DatatypeInfoU_union
_tmp8A9;struct Cyc_Absyn_Datatypedecl**_tmp8AA;struct Cyc_Absyn_Datatypedecl*
_tmp8AB;_LL57A: _tmp8A0=_tmp89F.f1;if((int)_tmp8A0 != 0)goto _LL57C;_tmp8A1=_tmp89F.f2;
if(_tmp8A1 <= (void*)4)goto _LL57C;if(*((int*)_tmp8A1)!= 2)goto _LL57C;_tmp8A2=((
struct Cyc_Absyn_DatatypeType_struct*)_tmp8A1)->f1;_tmp8A3=_tmp8A2.datatype_info;
if((((((struct Cyc_Absyn_DatatypeType_struct*)_tmp8A1)->f1).datatype_info).KnownDatatype).tag
!= 1)goto _LL57C;_tmp8A4=(_tmp8A3.KnownDatatype).f1;_tmp8A5=*_tmp8A4;_LL57B:
problem=Cyc_Absyn_qvar_cmp(_tmp8A5->name,Cyc_Absyn_datatype_print_arg_qvar)!= 0;
goto _LL579;_LL57C: _tmp8A6=_tmp89F.f1;if((int)_tmp8A6 != 1)goto _LL57E;_tmp8A7=
_tmp89F.f2;if(_tmp8A7 <= (void*)4)goto _LL57E;if(*((int*)_tmp8A7)!= 2)goto _LL57E;
_tmp8A8=((struct Cyc_Absyn_DatatypeType_struct*)_tmp8A7)->f1;_tmp8A9=_tmp8A8.datatype_info;
if((((((struct Cyc_Absyn_DatatypeType_struct*)_tmp8A7)->f1).datatype_info).KnownDatatype).tag
!= 1)goto _LL57E;_tmp8AA=(_tmp8A9.KnownDatatype).f1;_tmp8AB=*_tmp8AA;_LL57D:
problem=Cyc_Absyn_qvar_cmp(_tmp8AB->name,Cyc_Absyn_datatype_scanf_arg_qvar)!= 0;
goto _LL579;_LL57E:;_LL57F: problem=1;goto _LL579;_LL579:;}if(problem){const char*
_tmpF4D;void*_tmpF4C;(_tmpF4C=0,Cyc_Tcutil_terr(loc,((_tmpF4D="format attribute and vararg types don't match",
_tag_dyneither(_tmpF4D,sizeof(char),46))),_tag_dyneither(_tmpF4C,sizeof(void*),0)));}}}}{
struct Cyc_List_List*rpo=_tmp7AF;for(0;rpo != 0;rpo=rpo->tl){struct _tuple0 _tmp8AF;
void*_tmp8B0;void*_tmp8B1;struct _tuple0*_tmp8AE=(struct _tuple0*)rpo->hd;_tmp8AF=*
_tmp8AE;_tmp8B0=_tmp8AF.f1;_tmp8B1=_tmp8AF.f2;_tmp879=Cyc_Tcutil_i_check_valid_type(
loc,te,_tmp879,(void*)6,_tmp8B0,1);_tmp879=Cyc_Tcutil_i_check_valid_type(loc,te,
_tmp879,(void*)5,_tmp8B1,1);}}if(*_tmp7AA != 0)_tmp879=Cyc_Tcutil_i_check_valid_type(
loc,te,_tmp879,(void*)6,(void*)((struct Cyc_Core_Opt*)_check_null(*_tmp7AA))->v,1);
else{struct Cyc_List_List*effect=0;{struct Cyc_List_List*tvs=_tmp879.free_vars;
for(0;tvs != 0;tvs=tvs->tl){struct Cyc_Absyn_Tvar*_tmp8B3;int _tmp8B4;struct
_tuple18 _tmp8B2=*((struct _tuple18*)tvs->hd);_tmp8B3=_tmp8B2.f1;_tmp8B4=_tmp8B2.f2;
if(!_tmp8B4)continue;{void*_tmp8B5=Cyc_Absyn_compress_kb((void*)_tmp8B3->kind);
struct Cyc_Core_Opt*_tmp8B6;struct Cyc_Core_Opt**_tmp8B7;void*_tmp8B8;struct Cyc_Core_Opt*
_tmp8B9;struct Cyc_Core_Opt**_tmp8BA;void*_tmp8BB;void*_tmp8BC;void*_tmp8BD;void*
_tmp8BE;struct Cyc_Core_Opt*_tmp8BF;struct Cyc_Core_Opt**_tmp8C0;void*_tmp8C1;void*
_tmp8C2;struct Cyc_Core_Opt*_tmp8C3;struct Cyc_Core_Opt**_tmp8C4;_LL581: if(*((int*)
_tmp8B5)!= 2)goto _LL583;_tmp8B6=((struct Cyc_Absyn_Less_kb_struct*)_tmp8B5)->f1;
_tmp8B7=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)_tmp8B5)->f1;
_tmp8B8=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp8B5)->f2;if((int)_tmp8B8 != 
5)goto _LL583;_LL582: _tmp8BA=_tmp8B7;_tmp8BB=(void*)3;goto _LL584;_LL583: if(*((int*)
_tmp8B5)!= 2)goto _LL585;_tmp8B9=((struct Cyc_Absyn_Less_kb_struct*)_tmp8B5)->f1;
_tmp8BA=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)_tmp8B5)->f1;
_tmp8BB=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp8B5)->f2;if(!(_tmp8BB == (
void*)3  || _tmp8BB == (void*)4))goto _LL585;_LL584:*_tmp8BA=Cyc_Tcutil_kind_to_bound_opt(
_tmp8BB);_tmp8BC=_tmp8BB;goto _LL586;_LL585: if(*((int*)_tmp8B5)!= 0)goto _LL587;
_tmp8BC=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp8B5)->f1;if(!((_tmp8BC == (
void*)3  || _tmp8BC == (void*)4) || _tmp8BC == (void*)5))goto _LL587;_LL586:{struct
Cyc_Absyn_AccessEff_struct*_tmpF5C;struct Cyc_Absyn_VarType_struct*_tmpF5B;struct
Cyc_Absyn_VarType_struct _tmpF5A;struct Cyc_Absyn_AccessEff_struct _tmpF59;struct
Cyc_List_List*_tmpF58;effect=((_tmpF58=_cycalloc(sizeof(*_tmpF58)),((_tmpF58->hd=(
void*)((void*)((_tmpF5C=_cycalloc(sizeof(*_tmpF5C)),((_tmpF5C[0]=((_tmpF59.tag=
19,((_tmpF59.f1=(void*)((void*)((_tmpF5B=_cycalloc(sizeof(*_tmpF5B)),((_tmpF5B[0]=((
_tmpF5A.tag=1,((_tmpF5A.f1=_tmp8B3,_tmpF5A)))),_tmpF5B))))),_tmpF59)))),_tmpF5C))))),((
_tmpF58->tl=effect,_tmpF58))))));}goto _LL580;_LL587: if(*((int*)_tmp8B5)!= 2)goto
_LL589;_tmp8BD=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp8B5)->f2;if((int)
_tmp8BD != 7)goto _LL589;_LL588: goto _LL58A;_LL589: if(*((int*)_tmp8B5)!= 0)goto
_LL58B;_tmp8BE=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp8B5)->f1;if((int)
_tmp8BE != 7)goto _LL58B;_LL58A: goto _LL580;_LL58B: if(*((int*)_tmp8B5)!= 2)goto
_LL58D;_tmp8BF=((struct Cyc_Absyn_Less_kb_struct*)_tmp8B5)->f1;_tmp8C0=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*)_tmp8B5)->f1;_tmp8C1=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp8B5)->f2;if((int)_tmp8C1 != 6)goto _LL58D;_LL58C:*_tmp8C0=Cyc_Tcutil_kind_to_bound_opt((
void*)6);goto _LL58E;_LL58D: if(*((int*)_tmp8B5)!= 0)goto _LL58F;_tmp8C2=(void*)((
struct Cyc_Absyn_Eq_kb_struct*)_tmp8B5)->f1;if((int)_tmp8C2 != 6)goto _LL58F;_LL58E:{
struct Cyc_Absyn_VarType_struct*_tmpF62;struct Cyc_Absyn_VarType_struct _tmpF61;
struct Cyc_List_List*_tmpF60;effect=((_tmpF60=_cycalloc(sizeof(*_tmpF60)),((
_tmpF60->hd=(void*)((void*)((_tmpF62=_cycalloc(sizeof(*_tmpF62)),((_tmpF62[0]=((
_tmpF61.tag=1,((_tmpF61.f1=_tmp8B3,_tmpF61)))),_tmpF62))))),((_tmpF60->tl=effect,
_tmpF60))))));}goto _LL580;_LL58F: if(*((int*)_tmp8B5)!= 1)goto _LL591;_tmp8C3=((
struct Cyc_Absyn_Unknown_kb_struct*)_tmp8B5)->f1;_tmp8C4=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Unknown_kb_struct*)_tmp8B5)->f1;_LL590:{struct Cyc_Absyn_Less_kb_struct*
_tmpF68;struct Cyc_Absyn_Less_kb_struct _tmpF67;struct Cyc_Core_Opt*_tmpF66;*
_tmp8C4=((_tmpF66=_cycalloc(sizeof(*_tmpF66)),((_tmpF66->v=(void*)((void*)((
_tmpF68=_cycalloc(sizeof(*_tmpF68)),((_tmpF68[0]=((_tmpF67.tag=2,((_tmpF67.f1=0,((
_tmpF67.f2=(void*)((void*)0),_tmpF67)))))),_tmpF68))))),_tmpF66))));}goto _LL592;
_LL591:;_LL592:{struct Cyc_Absyn_RgnsEff_struct*_tmpF77;struct Cyc_Absyn_VarType_struct*
_tmpF76;struct Cyc_Absyn_VarType_struct _tmpF75;struct Cyc_Absyn_RgnsEff_struct
_tmpF74;struct Cyc_List_List*_tmpF73;effect=((_tmpF73=_cycalloc(sizeof(*_tmpF73)),((
_tmpF73->hd=(void*)((void*)((_tmpF77=_cycalloc(sizeof(*_tmpF77)),((_tmpF77[0]=((
_tmpF74.tag=21,((_tmpF74.f1=(void*)((void*)((_tmpF76=_cycalloc(sizeof(*_tmpF76)),((
_tmpF76[0]=((_tmpF75.tag=1,((_tmpF75.f1=_tmp8B3,_tmpF75)))),_tmpF76))))),_tmpF74)))),
_tmpF77))))),((_tmpF73->tl=effect,_tmpF73))))));}goto _LL580;_LL580:;}}}{struct
Cyc_List_List*ts=_tmp879.free_evars;for(0;ts != 0;ts=ts->tl){void*_tmp8D6;int
_tmp8D7;struct _tuple19 _tmp8D5=*((struct _tuple19*)ts->hd);_tmp8D6=_tmp8D5.f1;
_tmp8D7=_tmp8D5.f2;if(!_tmp8D7)continue;{void*_tmp8D8=Cyc_Tcutil_typ_kind(
_tmp8D6);_LL594: if((int)_tmp8D8 != 5)goto _LL596;_LL595: goto _LL597;_LL596: if((int)
_tmp8D8 != 4)goto _LL598;_LL597: goto _LL599;_LL598: if((int)_tmp8D8 != 3)goto _LL59A;
_LL599:{struct Cyc_Absyn_AccessEff_struct*_tmpF7D;struct Cyc_Absyn_AccessEff_struct
_tmpF7C;struct Cyc_List_List*_tmpF7B;effect=((_tmpF7B=_cycalloc(sizeof(*_tmpF7B)),((
_tmpF7B->hd=(void*)((void*)((_tmpF7D=_cycalloc(sizeof(*_tmpF7D)),((_tmpF7D[0]=((
_tmpF7C.tag=19,((_tmpF7C.f1=(void*)_tmp8D6,_tmpF7C)))),_tmpF7D))))),((_tmpF7B->tl=
effect,_tmpF7B))))));}goto _LL593;_LL59A: if((int)_tmp8D8 != 6)goto _LL59C;_LL59B:{
struct Cyc_List_List*_tmpF7E;effect=((_tmpF7E=_cycalloc(sizeof(*_tmpF7E)),((
_tmpF7E->hd=(void*)_tmp8D6,((_tmpF7E->tl=effect,_tmpF7E))))));}goto _LL593;_LL59C:
if((int)_tmp8D8 != 7)goto _LL59E;_LL59D: goto _LL593;_LL59E:;_LL59F:{struct Cyc_Absyn_RgnsEff_struct*
_tmpF84;struct Cyc_Absyn_RgnsEff_struct _tmpF83;struct Cyc_List_List*_tmpF82;effect=((
_tmpF82=_cycalloc(sizeof(*_tmpF82)),((_tmpF82->hd=(void*)((void*)((_tmpF84=
_cycalloc(sizeof(*_tmpF84)),((_tmpF84[0]=((_tmpF83.tag=21,((_tmpF83.f1=(void*)
_tmp8D6,_tmpF83)))),_tmpF84))))),((_tmpF82->tl=effect,_tmpF82))))));}goto _LL593;
_LL593:;}}}{struct Cyc_Absyn_JoinEff_struct*_tmpF8A;struct Cyc_Absyn_JoinEff_struct
_tmpF89;struct Cyc_Core_Opt*_tmpF88;*_tmp7AA=((_tmpF88=_cycalloc(sizeof(*_tmpF88)),((
_tmpF88->v=(void*)((void*)((_tmpF8A=_cycalloc(sizeof(*_tmpF8A)),((_tmpF8A[0]=((
_tmpF89.tag=20,((_tmpF89.f1=effect,_tmpF89)))),_tmpF8A))))),_tmpF88))));}}if(*
_tmp7A8 != 0){struct Cyc_List_List*bs=*_tmp7A8;for(0;bs != 0;bs=bs->tl){void*
_tmp8E3=Cyc_Absyn_compress_kb((void*)((struct Cyc_Absyn_Tvar*)bs->hd)->kind);
struct Cyc_Core_Opt*_tmp8E4;struct Cyc_Core_Opt**_tmp8E5;struct Cyc_Core_Opt*
_tmp8E6;struct Cyc_Core_Opt**_tmp8E7;void*_tmp8E8;struct Cyc_Core_Opt*_tmp8E9;
struct Cyc_Core_Opt**_tmp8EA;void*_tmp8EB;struct Cyc_Core_Opt*_tmp8EC;struct Cyc_Core_Opt**
_tmp8ED;void*_tmp8EE;struct Cyc_Core_Opt*_tmp8EF;struct Cyc_Core_Opt**_tmp8F0;void*
_tmp8F1;void*_tmp8F2;_LL5A1: if(*((int*)_tmp8E3)!= 1)goto _LL5A3;_tmp8E4=((struct
Cyc_Absyn_Unknown_kb_struct*)_tmp8E3)->f1;_tmp8E5=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Unknown_kb_struct*)_tmp8E3)->f1;_LL5A2:{const char*_tmpF8E;void*
_tmpF8D[1];struct Cyc_String_pa_struct _tmpF8C;(_tmpF8C.tag=0,((_tmpF8C.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)bs->hd)->name),((
_tmpF8D[0]=& _tmpF8C,Cyc_Tcutil_warn(loc,((_tmpF8E="Type variable %s unconstrained, assuming boxed",
_tag_dyneither(_tmpF8E,sizeof(char),47))),_tag_dyneither(_tmpF8D,sizeof(void*),1)))))));}
_tmp8E7=_tmp8E5;goto _LL5A4;_LL5A3: if(*((int*)_tmp8E3)!= 2)goto _LL5A5;_tmp8E6=((
struct Cyc_Absyn_Less_kb_struct*)_tmp8E3)->f1;_tmp8E7=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*)_tmp8E3)->f1;_tmp8E8=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp8E3)->f2;if((int)_tmp8E8 != 1)goto _LL5A5;_LL5A4: _tmp8EA=_tmp8E7;goto _LL5A6;
_LL5A5: if(*((int*)_tmp8E3)!= 2)goto _LL5A7;_tmp8E9=((struct Cyc_Absyn_Less_kb_struct*)
_tmp8E3)->f1;_tmp8EA=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp8E3)->f1;_tmp8EB=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp8E3)->f2;if((
int)_tmp8EB != 0)goto _LL5A7;_LL5A6:*_tmp8EA=Cyc_Tcutil_kind_to_bound_opt((void*)2);
goto _LL5A0;_LL5A7: if(*((int*)_tmp8E3)!= 2)goto _LL5A9;_tmp8EC=((struct Cyc_Absyn_Less_kb_struct*)
_tmp8E3)->f1;_tmp8ED=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp8E3)->f1;_tmp8EE=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp8E3)->f2;if((
int)_tmp8EE != 5)goto _LL5A9;_LL5A8:*_tmp8ED=Cyc_Tcutil_kind_to_bound_opt((void*)3);
goto _LL5A0;_LL5A9: if(*((int*)_tmp8E3)!= 2)goto _LL5AB;_tmp8EF=((struct Cyc_Absyn_Less_kb_struct*)
_tmp8E3)->f1;_tmp8F0=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp8E3)->f1;_tmp8F1=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp8E3)->f2;
_LL5AA:*_tmp8F0=Cyc_Tcutil_kind_to_bound_opt(_tmp8F1);goto _LL5A0;_LL5AB: if(*((
int*)_tmp8E3)!= 0)goto _LL5AD;_tmp8F2=(void*)((struct Cyc_Absyn_Eq_kb_struct*)
_tmp8E3)->f1;if((int)_tmp8F2 != 1)goto _LL5AD;_LL5AC:{const char*_tmpF91;void*
_tmpF90;(_tmpF90=0,Cyc_Tcutil_terr(loc,((_tmpF91="functions can't abstract M types",
_tag_dyneither(_tmpF91,sizeof(char),33))),_tag_dyneither(_tmpF90,sizeof(void*),0)));}
goto _LL5A0;_LL5AD:;_LL5AE: goto _LL5A0;_LL5A0:;}}cvtenv.kind_env=Cyc_Tcutil_remove_bound_tvars(
Cyc_Core_heap_region,_tmp879.kind_env,*_tmp7A8);_tmp879.free_vars=Cyc_Tcutil_remove_bound_tvars_bool(
_tmp879.r,_tmp879.free_vars,*_tmp7A8);{struct Cyc_List_List*tvs=_tmp879.free_vars;
for(0;tvs != 0;tvs=tvs->tl){struct Cyc_Absyn_Tvar*_tmp8F9;int _tmp8FA;struct
_tuple18 _tmp8F8=*((struct _tuple18*)tvs->hd);_tmp8F9=_tmp8F8.f1;_tmp8FA=_tmp8F8.f2;
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,
_tmp8F9,_tmp8FA);}}{struct Cyc_List_List*evs=_tmp879.free_evars;for(0;evs != 0;evs=
evs->tl){void*_tmp8FC;int _tmp8FD;struct _tuple19 _tmp8FB=*((struct _tuple19*)evs->hd);
_tmp8FC=_tmp8FB.f1;_tmp8FD=_tmp8FB.f2;cvtenv.free_evars=Cyc_Tcutil_add_free_evar(
cvtenv.r,cvtenv.free_evars,_tmp8FC,_tmp8FD);}}}goto _LL4DC;}}_LL4FB: if(*((int*)
_tmp77E)!= 9)goto _LL4FD;_tmp7B1=((struct Cyc_Absyn_TupleType_struct*)_tmp77E)->f1;
_LL4FC: for(0;_tmp7B1 != 0;_tmp7B1=_tmp7B1->tl){struct _tuple5*_tmp8FF=(struct
_tuple5*)_tmp7B1->hd;cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)1,(*
_tmp8FF).f2,1);((*_tmp8FF).f1).real_const=Cyc_Tcutil_extract_const_from_typedef(
loc,((*_tmp8FF).f1).print_const,(*_tmp8FF).f2);}goto _LL4DC;_LL4FD: if(*((int*)
_tmp77E)!= 11)goto _LL4FF;_tmp7B2=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp77E)->f1;_tmp7B3=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp77E)->f2;_LL4FE: {
struct _RegionHandle*_tmp900=Cyc_Tcenv_get_fnrgn(te);{struct Cyc_List_List*
prev_fields=0;for(0;_tmp7B3 != 0;_tmp7B3=_tmp7B3->tl){struct Cyc_Absyn_Aggrfield
_tmp902;struct _dyneither_ptr*_tmp903;struct Cyc_Absyn_Tqual _tmp904;struct Cyc_Absyn_Tqual*
_tmp905;void*_tmp906;struct Cyc_Absyn_Exp*_tmp907;struct Cyc_List_List*_tmp908;
struct Cyc_Absyn_Aggrfield*_tmp901=(struct Cyc_Absyn_Aggrfield*)_tmp7B3->hd;
_tmp902=*_tmp901;_tmp903=_tmp902.name;_tmp904=_tmp902.tq;_tmp905=(struct Cyc_Absyn_Tqual*)&(*
_tmp901).tq;_tmp906=(void*)_tmp902.type;_tmp907=_tmp902.width;_tmp908=_tmp902.attributes;
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*
l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,_tmp903)){
const char*_tmpF95;void*_tmpF94[1];struct Cyc_String_pa_struct _tmpF93;(_tmpF93.tag=
0,((_tmpF93.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp903),((_tmpF94[
0]=& _tmpF93,Cyc_Tcutil_terr(loc,((_tmpF95="duplicate field %s",_tag_dyneither(
_tmpF95,sizeof(char),19))),_tag_dyneither(_tmpF94,sizeof(void*),1)))))));}{const
char*_tmpF96;if(Cyc_strcmp((struct _dyneither_ptr)*_tmp903,((_tmpF96="",
_tag_dyneither(_tmpF96,sizeof(char),1))))!= 0){struct Cyc_List_List*_tmpF97;
prev_fields=((_tmpF97=_region_malloc(_tmp900,sizeof(*_tmpF97)),((_tmpF97->hd=
_tmp903,((_tmpF97->tl=prev_fields,_tmpF97))))));}}cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,(void*)1,_tmp906,1);_tmp905->real_const=Cyc_Tcutil_extract_const_from_typedef(
loc,_tmp905->print_const,_tmp906);if(_tmp7B2 == (void*)1  && !Cyc_Tcutil_bits_only(
_tmp906)){const char*_tmpF9B;void*_tmpF9A[1];struct Cyc_String_pa_struct _tmpF99;(
_tmpF99.tag=0,((_tmpF99.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp903),((
_tmpF9A[0]=& _tmpF99,Cyc_Tcutil_warn(loc,((_tmpF9B="union member %s is not `bits-only' so it can only be written and not read",
_tag_dyneither(_tmpF9B,sizeof(char),74))),_tag_dyneither(_tmpF9A,sizeof(void*),1)))))));}
Cyc_Tcutil_check_bitfield(loc,te,_tmp906,_tmp907,_tmp903);Cyc_Tcutil_check_field_atts(
loc,_tmp903,_tmp908);}}goto _LL4DC;}_LL4FF: if(*((int*)_tmp77E)!= 10)goto _LL501;
_tmp7B4=((struct Cyc_Absyn_AggrType_struct*)_tmp77E)->f1;_tmp7B5=_tmp7B4.aggr_info;
_tmp7B6=(union Cyc_Absyn_AggrInfoU_union*)&(((struct Cyc_Absyn_AggrType_struct*)
_tmp77E)->f1).aggr_info;_tmp7B7=_tmp7B4.targs;_tmp7B8=(struct Cyc_List_List**)&(((
struct Cyc_Absyn_AggrType_struct*)_tmp77E)->f1).targs;_LL500:{union Cyc_Absyn_AggrInfoU_union
_tmp911=*_tmp7B6;void*_tmp912;struct _tuple2*_tmp913;struct Cyc_Absyn_Aggrdecl**
_tmp914;struct Cyc_Absyn_Aggrdecl*_tmp915;_LL5B0: if((_tmp911.UnknownAggr).tag != 0)
goto _LL5B2;_tmp912=(_tmp911.UnknownAggr).f1;_tmp913=(_tmp911.UnknownAggr).f2;
_LL5B1: {struct Cyc_Absyn_Aggrdecl**adp;{struct _handler_cons _tmp916;_push_handler(&
_tmp916);{int _tmp918=0;if(setjmp(_tmp916.handler))_tmp918=1;if(!_tmp918){adp=Cyc_Tcenv_lookup_aggrdecl(
te,loc,_tmp913);{union Cyc_Absyn_AggrInfoU_union _tmpF9C;*_tmp7B6=(union Cyc_Absyn_AggrInfoU_union)(((
_tmpF9C.KnownAggr).tag=1,(((_tmpF9C.KnownAggr).f1=adp,_tmpF9C))));};_pop_handler();}
else{void*_tmp917=(void*)_exn_thrown;void*_tmp91B=_tmp917;_LL5B5: if(_tmp91B != 
Cyc_Dict_Absent)goto _LL5B7;_LL5B6: {struct Cyc_Tcenv_Genv*_tmp91C=((struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct Cyc_Absyn_Aggrdecl*_tmpF9D;struct Cyc_Absyn_Aggrdecl*_tmp91D=(_tmpF9D=
_cycalloc(sizeof(*_tmpF9D)),((_tmpF9D->kind=(void*)_tmp912,((_tmpF9D->sc=(void*)((
void*)3),((_tmpF9D->name=_tmp913,((_tmpF9D->tvs=0,((_tmpF9D->impl=0,((_tmpF9D->attributes=
0,_tmpF9D)))))))))))));Cyc_Tc_tcAggrdecl(te,_tmp91C,loc,_tmp91D);adp=Cyc_Tcenv_lookup_aggrdecl(
te,loc,_tmp913);{union Cyc_Absyn_AggrInfoU_union _tmpF9E;*_tmp7B6=(union Cyc_Absyn_AggrInfoU_union)(((
_tmpF9E.KnownAggr).tag=1,(((_tmpF9E.KnownAggr).f1=adp,_tmpF9E))));}if(*_tmp7B8 != 
0){{const char*_tmpFA2;void*_tmpFA1[1];struct Cyc_String_pa_struct _tmpFA0;(_tmpFA0.tag=
0,((_tmpFA0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp913)),((_tmpFA1[0]=& _tmpFA0,Cyc_Tcutil_terr(loc,((_tmpFA2="declare parameterized type %s before using",
_tag_dyneither(_tmpFA2,sizeof(char),43))),_tag_dyneither(_tmpFA1,sizeof(void*),1)))))));}
return cvtenv;}goto _LL5B4;}_LL5B7:;_LL5B8:(void)_throw(_tmp91B);_LL5B4:;}}}
_tmp915=*adp;goto _LL5B3;}_LL5B2: if((_tmp911.KnownAggr).tag != 1)goto _LL5AF;
_tmp914=(_tmp911.KnownAggr).f1;_tmp915=*_tmp914;_LL5B3: {struct Cyc_List_List*tvs=
_tmp915->tvs;struct Cyc_List_List*ts=*_tmp7B8;for(0;ts != 0  && tvs != 0;(ts=ts->tl,
tvs=tvs->tl)){void*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd);cvtenv=
Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,(void*)ts->hd,1);}if(ts != 0){const
char*_tmpFA6;void*_tmpFA5[1];struct Cyc_String_pa_struct _tmpFA4;(_tmpFA4.tag=0,((
_tmpFA4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp915->name)),((_tmpFA5[0]=& _tmpFA4,Cyc_Tcutil_terr(loc,((_tmpFA6="too many parameters for type %s",
_tag_dyneither(_tmpFA6,sizeof(char),32))),_tag_dyneither(_tmpFA5,sizeof(void*),1)))))));}
if(tvs != 0){struct Cyc_List_List*hidden_ts=0;for(0;tvs != 0;tvs=tvs->tl){void*k=
Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd);void*e=Cyc_Absyn_new_evar(0,
0);{struct Cyc_List_List*_tmpFA7;hidden_ts=((_tmpFA7=_cycalloc(sizeof(*_tmpFA7)),((
_tmpFA7->hd=(void*)e,((_tmpFA7->tl=hidden_ts,_tmpFA7))))));}cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,k,e,1);}*_tmp7B8=Cyc_List_imp_append(*_tmp7B8,Cyc_List_imp_rev(
hidden_ts));}}_LL5AF:;}goto _LL4DC;_LL501: if(*((int*)_tmp77E)!= 16)goto _LL503;
_tmp7B9=((struct Cyc_Absyn_TypedefType_struct*)_tmp77E)->f1;_tmp7BA=((struct Cyc_Absyn_TypedefType_struct*)
_tmp77E)->f2;_tmp7BB=(struct Cyc_List_List**)&((struct Cyc_Absyn_TypedefType_struct*)
_tmp77E)->f2;_tmp7BC=((struct Cyc_Absyn_TypedefType_struct*)_tmp77E)->f3;_tmp7BD=(
struct Cyc_Absyn_Typedefdecl**)&((struct Cyc_Absyn_TypedefType_struct*)_tmp77E)->f3;
_tmp7BE=((struct Cyc_Absyn_TypedefType_struct*)_tmp77E)->f4;_tmp7BF=(void***)&((
struct Cyc_Absyn_TypedefType_struct*)_tmp77E)->f4;_LL502: {struct Cyc_List_List*
targs=*_tmp7BB;struct Cyc_Absyn_Typedefdecl*td;{struct _handler_cons _tmp927;
_push_handler(& _tmp927);{int _tmp929=0;if(setjmp(_tmp927.handler))_tmp929=1;if(!
_tmp929){td=Cyc_Tcenv_lookup_typedefdecl(te,loc,_tmp7B9);;_pop_handler();}else{
void*_tmp928=(void*)_exn_thrown;void*_tmp92B=_tmp928;_LL5BA: if(_tmp92B != Cyc_Dict_Absent)
goto _LL5BC;_LL5BB:{const char*_tmpFAB;void*_tmpFAA[1];struct Cyc_String_pa_struct
_tmpFA9;(_tmpFA9.tag=0,((_tmpFA9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp7B9)),((_tmpFAA[0]=& _tmpFA9,Cyc_Tcutil_terr(loc,((
_tmpFAB="unbound typedef name %s",_tag_dyneither(_tmpFAB,sizeof(char),24))),
_tag_dyneither(_tmpFAA,sizeof(void*),1)))))));}return cvtenv;_LL5BC:;_LL5BD:(void)
_throw(_tmp92B);_LL5B9:;}}}*_tmp7BD=(struct Cyc_Absyn_Typedefdecl*)td;_tmp7B9[0]=(
td->name)[_check_known_subscript_notnull(1,0)];{struct Cyc_List_List*tvs=td->tvs;
struct Cyc_List_List*ts=targs;struct _RegionHandle*_tmp92F=Cyc_Tcenv_get_fnrgn(te);{
struct Cyc_List_List*inst=0;for(0;ts != 0  && tvs != 0;(ts=ts->tl,tvs=tvs->tl)){void*
k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd);cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,k,(void*)ts->hd,1);{struct _tuple8*_tmpFAE;struct Cyc_List_List*
_tmpFAD;inst=((_tmpFAD=_region_malloc(_tmp92F,sizeof(*_tmpFAD)),((_tmpFAD->hd=((
_tmpFAE=_region_malloc(_tmp92F,sizeof(*_tmpFAE)),((_tmpFAE->f1=(struct Cyc_Absyn_Tvar*)
tvs->hd,((_tmpFAE->f2=(void*)ts->hd,_tmpFAE)))))),((_tmpFAD->tl=inst,_tmpFAD))))));}}
if(ts != 0){const char*_tmpFB2;void*_tmpFB1[1];struct Cyc_String_pa_struct _tmpFB0;(
_tmpFB0.tag=0,((_tmpFB0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp7B9)),((_tmpFB1[0]=& _tmpFB0,Cyc_Tcutil_terr(loc,((_tmpFB2="too many parameters for typedef %s",
_tag_dyneither(_tmpFB2,sizeof(char),35))),_tag_dyneither(_tmpFB1,sizeof(void*),1)))))));}
if(tvs != 0){struct Cyc_List_List*hidden_ts=0;for(0;tvs != 0;tvs=tvs->tl){void*k=
Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd);void*e=Cyc_Absyn_new_evar(0,
0);{struct Cyc_List_List*_tmpFB3;hidden_ts=((_tmpFB3=_cycalloc(sizeof(*_tmpFB3)),((
_tmpFB3->hd=(void*)e,((_tmpFB3->tl=hidden_ts,_tmpFB3))))));}cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,k,e,1);{struct _tuple8*_tmpFB6;struct Cyc_List_List*_tmpFB5;inst=(
struct Cyc_List_List*)((_tmpFB5=_cycalloc(sizeof(*_tmpFB5)),((_tmpFB5->hd=(struct
_tuple8*)((_tmpFB6=_cycalloc(sizeof(*_tmpFB6)),((_tmpFB6->f1=(struct Cyc_Absyn_Tvar*)
tvs->hd,((_tmpFB6->f2=e,_tmpFB6)))))),((_tmpFB5->tl=inst,_tmpFB5))))));}}*
_tmp7BB=Cyc_List_imp_append(targs,Cyc_List_imp_rev(hidden_ts));}if(td->defn != 0){
void*new_typ=Cyc_Tcutil_rsubstitute(_tmp92F,inst,(void*)((struct Cyc_Core_Opt*)
_check_null(td->defn))->v);void**_tmpFB7;*_tmp7BF=((_tmpFB7=_cycalloc(sizeof(*
_tmpFB7)),((_tmpFB7[0]=new_typ,_tmpFB7))));}}goto _LL4DC;}}_LL503: if((int)_tmp77E
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
Cyc_Tcutil_typ_kind(t),expected_kind)){{void*_tmp939=t;struct Cyc_Core_Opt*
_tmp93A;struct Cyc_Core_Opt*_tmp93B;_LL5BF: if(_tmp939 <= (void*)4)goto _LL5C1;if(*((
int*)_tmp939)!= 0)goto _LL5C1;_tmp93A=((struct Cyc_Absyn_Evar_struct*)_tmp939)->f1;
_tmp93B=((struct Cyc_Absyn_Evar_struct*)_tmp939)->f2;_LL5C0: {struct
_dyneither_ptr s;{struct Cyc_Core_Opt*_tmp93C=_tmp93A;struct Cyc_Core_Opt _tmp93D;
void*_tmp93E;_LL5C4: if(_tmp93C != 0)goto _LL5C6;_LL5C5:{const char*_tmpFB8;s=((
_tmpFB8="kind=NULL ",_tag_dyneither(_tmpFB8,sizeof(char),11)));}goto _LL5C3;
_LL5C6: if(_tmp93C == 0)goto _LL5C3;_tmp93D=*_tmp93C;_tmp93E=(void*)_tmp93D.v;
_LL5C7:{const char*_tmpFBC;void*_tmpFBB[1];struct Cyc_String_pa_struct _tmpFBA;s=(
struct _dyneither_ptr)((_tmpFBA.tag=0,((_tmpFBA.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_kind2string(_tmp93E)),((_tmpFBB[0]=& _tmpFBA,Cyc_aprintf(((
_tmpFBC="kind=%s ",_tag_dyneither(_tmpFBC,sizeof(char),9))),_tag_dyneither(
_tmpFBB,sizeof(void*),1))))))));}goto _LL5C3;_LL5C3:;}{struct Cyc_Core_Opt*_tmp943=
_tmp93B;struct Cyc_Core_Opt _tmp944;void*_tmp945;_LL5C9: if(_tmp943 != 0)goto _LL5CB;
_LL5CA:{const char*_tmpFC0;void*_tmpFBF[1];struct Cyc_String_pa_struct _tmpFBE;s=(
struct _dyneither_ptr)((_tmpFBE.tag=0,((_tmpFBE.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)s),((_tmpFBF[0]=& _tmpFBE,Cyc_aprintf(((_tmpFC0="%s ref=NULL",
_tag_dyneither(_tmpFC0,sizeof(char),12))),_tag_dyneither(_tmpFBF,sizeof(void*),1))))))));}
goto _LL5C8;_LL5CB: if(_tmp943 == 0)goto _LL5C8;_tmp944=*_tmp943;_tmp945=(void*)
_tmp944.v;_LL5CC:{const char*_tmpFC5;void*_tmpFC4[2];struct Cyc_String_pa_struct
_tmpFC3;struct Cyc_String_pa_struct _tmpFC2;s=(struct _dyneither_ptr)((_tmpFC2.tag=
0,((_tmpFC2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
_tmp945)),((_tmpFC3.tag=0,((_tmpFC3.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)s),((_tmpFC4[0]=& _tmpFC3,((_tmpFC4[1]=& _tmpFC2,Cyc_aprintf(((
_tmpFC5="%s ref=%s",_tag_dyneither(_tmpFC5,sizeof(char),10))),_tag_dyneither(
_tmpFC4,sizeof(void*),2))))))))))))));}goto _LL5C8;_LL5C8:;}{const char*_tmpFC9;
void*_tmpFC8[1];struct Cyc_String_pa_struct _tmpFC7;(_tmpFC7.tag=0,((_tmpFC7.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmpFC8[0]=& _tmpFC7,Cyc_fprintf(
Cyc_stderr,((_tmpFC9="evar info: %s\n",_tag_dyneither(_tmpFC9,sizeof(char),15))),
_tag_dyneither(_tmpFC8,sizeof(void*),1)))))));}goto _LL5BE;}_LL5C1:;_LL5C2: goto
_LL5BE;_LL5BE:;}{const char*_tmpFCF;void*_tmpFCE[3];struct Cyc_String_pa_struct
_tmpFCD;struct Cyc_String_pa_struct _tmpFCC;struct Cyc_String_pa_struct _tmpFCB;(
_tmpFCB.tag=0,((_tmpFCB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(
expected_kind)),((_tmpFCC.tag=0,((_tmpFCC.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_kind2string(Cyc_Tcutil_typ_kind(t))),((_tmpFCD.tag=0,((
_tmpFCD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((
_tmpFCE[0]=& _tmpFCD,((_tmpFCE[1]=& _tmpFCC,((_tmpFCE[2]=& _tmpFCB,Cyc_Tcutil_terr(
loc,((_tmpFCF="type %s has kind %s but as used here needs kind %s",
_tag_dyneither(_tmpFCF,sizeof(char),51))),_tag_dyneither(_tmpFCE,sizeof(void*),3)))))))))))))))))));}}
return cvtenv;}static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type_level_exp(
struct Cyc_Absyn_Exp*e,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv);
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type_level_exp(struct Cyc_Absyn_Exp*
e,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv){{void*_tmp955=(void*)e->r;
struct Cyc_List_List*_tmp956;struct Cyc_Absyn_Exp*_tmp957;struct Cyc_Absyn_Exp*
_tmp958;struct Cyc_Absyn_Exp*_tmp959;struct Cyc_Absyn_Exp*_tmp95A;struct Cyc_Absyn_Exp*
_tmp95B;struct Cyc_Absyn_Exp*_tmp95C;struct Cyc_Absyn_Exp*_tmp95D;struct Cyc_Absyn_Exp*
_tmp95E;struct Cyc_Absyn_Exp*_tmp95F;void*_tmp960;struct Cyc_Absyn_Exp*_tmp961;
void*_tmp962;void*_tmp963;void*_tmp964;struct Cyc_Absyn_Exp*_tmp965;_LL5CE: if(*((
int*)_tmp955)!= 0)goto _LL5D0;_LL5CF: goto _LL5D1;_LL5D0: if(*((int*)_tmp955)!= 33)
goto _LL5D2;_LL5D1: goto _LL5D3;_LL5D2: if(*((int*)_tmp955)!= 34)goto _LL5D4;_LL5D3:
goto _LL5D5;_LL5D4: if(*((int*)_tmp955)!= 1)goto _LL5D6;_LL5D5: goto _LL5CD;_LL5D6:
if(*((int*)_tmp955)!= 3)goto _LL5D8;_tmp956=((struct Cyc_Absyn_Primop_e_struct*)
_tmp955)->f2;_LL5D7: for(0;_tmp956 != 0;_tmp956=_tmp956->tl){cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp((
struct Cyc_Absyn_Exp*)_tmp956->hd,te,cvtenv);}goto _LL5CD;_LL5D8: if(*((int*)
_tmp955)!= 6)goto _LL5DA;_tmp957=((struct Cyc_Absyn_Conditional_e_struct*)_tmp955)->f1;
_tmp958=((struct Cyc_Absyn_Conditional_e_struct*)_tmp955)->f2;_tmp959=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp955)->f3;_LL5D9: cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp957,te,
cvtenv);cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp958,te,cvtenv);cvtenv=
Cyc_Tcutil_i_check_valid_type_level_exp(_tmp959,te,cvtenv);goto _LL5CD;_LL5DA: if(*((
int*)_tmp955)!= 7)goto _LL5DC;_tmp95A=((struct Cyc_Absyn_And_e_struct*)_tmp955)->f1;
_tmp95B=((struct Cyc_Absyn_And_e_struct*)_tmp955)->f2;_LL5DB: _tmp95C=_tmp95A;
_tmp95D=_tmp95B;goto _LL5DD;_LL5DC: if(*((int*)_tmp955)!= 8)goto _LL5DE;_tmp95C=((
struct Cyc_Absyn_Or_e_struct*)_tmp955)->f1;_tmp95D=((struct Cyc_Absyn_Or_e_struct*)
_tmp955)->f2;_LL5DD: _tmp95E=_tmp95C;_tmp95F=_tmp95D;goto _LL5DF;_LL5DE: if(*((int*)
_tmp955)!= 9)goto _LL5E0;_tmp95E=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp955)->f1;
_tmp95F=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp955)->f2;_LL5DF: cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(
_tmp95E,te,cvtenv);cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp95F,te,
cvtenv);goto _LL5CD;_LL5E0: if(*((int*)_tmp955)!= 15)goto _LL5E2;_tmp960=(void*)((
struct Cyc_Absyn_Cast_e_struct*)_tmp955)->f1;_tmp961=((struct Cyc_Absyn_Cast_e_struct*)
_tmp955)->f2;_LL5E1: cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp961,te,
cvtenv);cvtenv=Cyc_Tcutil_i_check_valid_type(e->loc,te,cvtenv,(void*)0,_tmp960,1);
goto _LL5CD;_LL5E2: if(*((int*)_tmp955)!= 20)goto _LL5E4;_tmp962=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)
_tmp955)->f1;_LL5E3: _tmp963=_tmp962;goto _LL5E5;_LL5E4: if(*((int*)_tmp955)!= 18)
goto _LL5E6;_tmp963=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp955)->f1;
_LL5E5: cvtenv=Cyc_Tcutil_i_check_valid_type(e->loc,te,cvtenv,(void*)0,_tmp963,1);
goto _LL5CD;_LL5E6: if(*((int*)_tmp955)!= 39)goto _LL5E8;_tmp964=(void*)((struct Cyc_Absyn_Valueof_e_struct*)
_tmp955)->f1;_LL5E7: cvtenv=Cyc_Tcutil_i_check_valid_type(e->loc,te,cvtenv,(void*)
7,_tmp964,1);goto _LL5CD;_LL5E8: if(*((int*)_tmp955)!= 19)goto _LL5EA;_tmp965=((
struct Cyc_Absyn_Sizeofexp_e_struct*)_tmp955)->f1;_LL5E9: cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(
_tmp965,te,cvtenv);goto _LL5CD;_LL5EA:;_LL5EB: {const char*_tmpFD2;void*_tmpFD1;(
_tmpFD1=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmpFD2="non-type-level-expression in Tcutil::i_check_valid_type_level_exp",
_tag_dyneither(_tmpFD2,sizeof(char),66))),_tag_dyneither(_tmpFD1,sizeof(void*),0)));}
_LL5CD:;}return cvtenv;}static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_check_valid_type(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv
cvt,void*expected_kind,void*t);static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_check_valid_type(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv
cvt,void*expected_kind,void*t){struct Cyc_List_List*_tmp968=cvt.kind_env;cvt=Cyc_Tcutil_i_check_valid_type(
loc,te,cvt,expected_kind,t,1);{struct Cyc_List_List*vs=cvt.free_vars;for(0;vs != 0;
vs=vs->tl){struct Cyc_Absyn_Tvar*_tmp96A;struct _tuple18 _tmp969=*((struct _tuple18*)
vs->hd);_tmp96A=_tmp969.f1;cvt.kind_env=Cyc_Tcutil_fast_add_free_tvar(_tmp968,
_tmp96A);}}{struct Cyc_List_List*evs=cvt.free_evars;for(0;evs != 0;evs=evs->tl){
void*_tmp96C;struct _tuple19 _tmp96B=*((struct _tuple19*)evs->hd);_tmp96C=_tmp96B.f1;{
void*_tmp96D=Cyc_Tcutil_compress(_tmp96C);struct Cyc_Core_Opt*_tmp96E;struct Cyc_Core_Opt**
_tmp96F;_LL5ED: if(_tmp96D <= (void*)4)goto _LL5EF;if(*((int*)_tmp96D)!= 0)goto
_LL5EF;_tmp96E=((struct Cyc_Absyn_Evar_struct*)_tmp96D)->f4;_tmp96F=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Evar_struct*)_tmp96D)->f4;_LL5EE: if(*_tmp96F == 0){struct Cyc_Core_Opt*
_tmpFD3;*_tmp96F=((_tmpFD3=_cycalloc(sizeof(*_tmpFD3)),((_tmpFD3->v=_tmp968,
_tmpFD3))));}else{struct Cyc_List_List*_tmp971=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(*_tmp96F))->v;struct Cyc_List_List*_tmp972=0;for(0;_tmp971 != 0;
_tmp971=_tmp971->tl){if(((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,
_tmp968,(struct Cyc_Absyn_Tvar*)_tmp971->hd)){struct Cyc_List_List*_tmpFD4;_tmp972=((
_tmpFD4=_cycalloc(sizeof(*_tmpFD4)),((_tmpFD4->hd=(struct Cyc_Absyn_Tvar*)_tmp971->hd,((
_tmpFD4->tl=_tmp972,_tmpFD4))))));}}{struct Cyc_Core_Opt*_tmpFD5;*_tmp96F=((
_tmpFD5=_cycalloc(sizeof(*_tmpFD5)),((_tmpFD5->v=_tmp972,_tmpFD5))));}}goto
_LL5EC;_LL5EF:;_LL5F0: goto _LL5EC;_LL5EC:;}}}return cvt;}void Cyc_Tcutil_check_valid_toplevel_type(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,void*t);void Cyc_Tcutil_check_valid_toplevel_type(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,void*t){int
generalize_evars=Cyc_Tcutil_is_function_type(t);struct Cyc_List_List*_tmp975=Cyc_Tcenv_lookup_type_vars(
te);struct _RegionHandle*_tmp976=Cyc_Tcenv_get_fnrgn(te);struct Cyc_Tcutil_CVTEnv
_tmpFD6;struct Cyc_Tcutil_CVTEnv _tmp977=Cyc_Tcutil_check_valid_type(loc,te,((
_tmpFD6.r=_tmp976,((_tmpFD6.kind_env=_tmp975,((_tmpFD6.free_vars=0,((_tmpFD6.free_evars=
0,((_tmpFD6.generalize_evars=generalize_evars,((_tmpFD6.fn_result=0,_tmpFD6)))))))))))),(
void*)1,t);struct Cyc_List_List*_tmp978=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct Cyc_Absyn_Tvar*(*f)(struct _tuple18*),struct Cyc_List_List*x))
Cyc_List_rmap)(_tmp976,(struct Cyc_Absyn_Tvar*(*)(struct _tuple18*))Cyc_Core_fst,
_tmp977.free_vars);struct Cyc_List_List*_tmp979=((struct Cyc_List_List*(*)(struct
_RegionHandle*,void*(*f)(struct _tuple19*),struct Cyc_List_List*x))Cyc_List_rmap)(
_tmp976,(void*(*)(struct _tuple19*))Cyc_Core_fst,_tmp977.free_evars);if(_tmp975 != 
0){struct Cyc_List_List*_tmp97A=0;{struct Cyc_List_List*_tmp97B=_tmp978;for(0;(
unsigned int)_tmp97B;_tmp97B=_tmp97B->tl){struct Cyc_Absyn_Tvar*_tmp97C=(struct
Cyc_Absyn_Tvar*)_tmp97B->hd;int found=0;{struct Cyc_List_List*_tmp97D=_tmp975;for(
0;(unsigned int)_tmp97D;_tmp97D=_tmp97D->tl){if(Cyc_Absyn_tvar_cmp(_tmp97C,(
struct Cyc_Absyn_Tvar*)_tmp97D->hd)== 0){found=1;break;}}}if(!found){struct Cyc_List_List*
_tmpFD7;_tmp97A=((_tmpFD7=_region_malloc(_tmp976,sizeof(*_tmpFD7)),((_tmpFD7->hd=(
struct Cyc_Absyn_Tvar*)_tmp97B->hd,((_tmpFD7->tl=_tmp97A,_tmpFD7))))));}}}_tmp978=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp97A);}{
struct Cyc_List_List*x=_tmp978;for(0;x != 0;x=x->tl){void*_tmp97F=Cyc_Absyn_compress_kb((
void*)((struct Cyc_Absyn_Tvar*)x->hd)->kind);struct Cyc_Core_Opt*_tmp980;struct Cyc_Core_Opt**
_tmp981;struct Cyc_Core_Opt*_tmp982;struct Cyc_Core_Opt**_tmp983;void*_tmp984;
struct Cyc_Core_Opt*_tmp985;struct Cyc_Core_Opt**_tmp986;void*_tmp987;struct Cyc_Core_Opt*
_tmp988;struct Cyc_Core_Opt**_tmp989;void*_tmp98A;void*_tmp98B;_LL5F2: if(*((int*)
_tmp97F)!= 1)goto _LL5F4;_tmp980=((struct Cyc_Absyn_Unknown_kb_struct*)_tmp97F)->f1;
_tmp981=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)_tmp97F)->f1;
_LL5F3: _tmp983=_tmp981;goto _LL5F5;_LL5F4: if(*((int*)_tmp97F)!= 2)goto _LL5F6;
_tmp982=((struct Cyc_Absyn_Less_kb_struct*)_tmp97F)->f1;_tmp983=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*)_tmp97F)->f1;_tmp984=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp97F)->f2;if((int)_tmp984 != 1)goto _LL5F6;_LL5F5:*_tmp983=Cyc_Tcutil_kind_to_bound_opt((
void*)2);goto _LL5F1;_LL5F6: if(*((int*)_tmp97F)!= 2)goto _LL5F8;_tmp985=((struct
Cyc_Absyn_Less_kb_struct*)_tmp97F)->f1;_tmp986=(struct Cyc_Core_Opt**)&((struct
Cyc_Absyn_Less_kb_struct*)_tmp97F)->f1;_tmp987=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp97F)->f2;if((int)_tmp987 != 5)goto _LL5F8;_LL5F7:*_tmp986=Cyc_Tcutil_kind_to_bound_opt((
void*)3);goto _LL5F1;_LL5F8: if(*((int*)_tmp97F)!= 2)goto _LL5FA;_tmp988=((struct
Cyc_Absyn_Less_kb_struct*)_tmp97F)->f1;_tmp989=(struct Cyc_Core_Opt**)&((struct
Cyc_Absyn_Less_kb_struct*)_tmp97F)->f1;_tmp98A=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp97F)->f2;_LL5F9:*_tmp989=Cyc_Tcutil_kind_to_bound_opt(_tmp98A);goto _LL5F1;
_LL5FA: if(*((int*)_tmp97F)!= 0)goto _LL5FC;_tmp98B=(void*)((struct Cyc_Absyn_Eq_kb_struct*)
_tmp97F)->f1;if((int)_tmp98B != 1)goto _LL5FC;_LL5FB:{const char*_tmpFDB;void*
_tmpFDA[1];struct Cyc_String_pa_struct _tmpFD9;(_tmpFD9.tag=0,((_tmpFD9.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_tvar2string((struct Cyc_Absyn_Tvar*)
x->hd)),((_tmpFDA[0]=& _tmpFD9,Cyc_Tcutil_terr(loc,((_tmpFDB="type variable %s cannot have kind M",
_tag_dyneither(_tmpFDB,sizeof(char),36))),_tag_dyneither(_tmpFDA,sizeof(void*),1)))))));}
goto _LL5F1;_LL5FC:;_LL5FD: goto _LL5F1;_LL5F1:;}}if(_tmp978 != 0  || _tmp979 != 0){{
void*_tmp98F=Cyc_Tcutil_compress(t);struct Cyc_Absyn_FnInfo _tmp990;struct Cyc_List_List*
_tmp991;struct Cyc_List_List**_tmp992;struct Cyc_Core_Opt*_tmp993;void*_tmp994;
struct Cyc_List_List*_tmp995;int _tmp996;struct Cyc_Absyn_VarargInfo*_tmp997;struct
Cyc_List_List*_tmp998;struct Cyc_List_List*_tmp999;_LL5FF: if(_tmp98F <= (void*)4)
goto _LL601;if(*((int*)_tmp98F)!= 8)goto _LL601;_tmp990=((struct Cyc_Absyn_FnType_struct*)
_tmp98F)->f1;_tmp991=_tmp990.tvars;_tmp992=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_struct*)
_tmp98F)->f1).tvars;_tmp993=_tmp990.effect;_tmp994=(void*)_tmp990.ret_typ;
_tmp995=_tmp990.args;_tmp996=_tmp990.c_varargs;_tmp997=_tmp990.cyc_varargs;
_tmp998=_tmp990.rgn_po;_tmp999=_tmp990.attributes;_LL600: if(*_tmp992 == 0){*
_tmp992=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_copy)(_tmp978);
_tmp978=0;}goto _LL5FE;_LL601:;_LL602: goto _LL5FE;_LL5FE:;}if(_tmp978 != 0){const
char*_tmpFDF;void*_tmpFDE[1];struct Cyc_String_pa_struct _tmpFDD;(_tmpFDD.tag=0,((
_tmpFDD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)
_tmp978->hd)->name),((_tmpFDE[0]=& _tmpFDD,Cyc_Tcutil_terr(loc,((_tmpFDF="unbound type variable %s",
_tag_dyneither(_tmpFDF,sizeof(char),25))),_tag_dyneither(_tmpFDE,sizeof(void*),1)))))));}
if(_tmp979 != 0)for(0;_tmp979 != 0;_tmp979=_tmp979->tl){void*e=(void*)_tmp979->hd;
void*_tmp99D=Cyc_Tcutil_typ_kind(e);_LL604: if((int)_tmp99D != 4)goto _LL606;_LL605:
if(!Cyc_Tcutil_unify(e,(void*)3)){const char*_tmpFE2;void*_tmpFE1;(_tmpFE1=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmpFE2="can't unify evar with unique region!",_tag_dyneither(_tmpFE2,sizeof(
char),37))),_tag_dyneither(_tmpFE1,sizeof(void*),0)));}goto _LL603;_LL606: if((int)
_tmp99D != 5)goto _LL608;_LL607: goto _LL609;_LL608: if((int)_tmp99D != 3)goto _LL60A;
_LL609: if(!Cyc_Tcutil_unify(e,(void*)2)){const char*_tmpFE5;void*_tmpFE4;(_tmpFE4=
0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmpFE5="can't unify evar with heap!",_tag_dyneither(_tmpFE5,sizeof(char),28))),
_tag_dyneither(_tmpFE4,sizeof(void*),0)));}goto _LL603;_LL60A: if((int)_tmp99D != 6)
goto _LL60C;_LL60B: if(!Cyc_Tcutil_unify(e,Cyc_Absyn_empty_effect)){const char*
_tmpFE8;void*_tmpFE7;(_tmpFE7=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpFE8="can't unify evar with {}!",
_tag_dyneither(_tmpFE8,sizeof(char),26))),_tag_dyneither(_tmpFE7,sizeof(void*),0)));}
goto _LL603;_LL60C:;_LL60D:{const char*_tmpFED;void*_tmpFEC[2];struct Cyc_String_pa_struct
_tmpFEB;struct Cyc_String_pa_struct _tmpFEA;(_tmpFEA.tag=0,((_tmpFEA.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpFEB.tag=0,((
_tmpFEB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(e)),((
_tmpFEC[0]=& _tmpFEB,((_tmpFEC[1]=& _tmpFEA,Cyc_Tcutil_terr(loc,((_tmpFED="hidden type variable %s isn't abstracted in type %s",
_tag_dyneither(_tmpFED,sizeof(char),52))),_tag_dyneither(_tmpFEC,sizeof(void*),2)))))))))))));}
goto _LL603;_LL603:;}}}void Cyc_Tcutil_check_fndecl_valid_type(struct Cyc_Position_Segment*
loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Fndecl*fd);void Cyc_Tcutil_check_fndecl_valid_type(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Fndecl*fd){
void*t=Cyc_Tcutil_fndecl2typ(fd);Cyc_Tcutil_check_valid_toplevel_type(loc,te,t);{
void*_tmp9A9=Cyc_Tcutil_compress(t);struct Cyc_Absyn_FnInfo _tmp9AA;struct Cyc_List_List*
_tmp9AB;struct Cyc_Core_Opt*_tmp9AC;void*_tmp9AD;_LL60F: if(_tmp9A9 <= (void*)4)
goto _LL611;if(*((int*)_tmp9A9)!= 8)goto _LL611;_tmp9AA=((struct Cyc_Absyn_FnType_struct*)
_tmp9A9)->f1;_tmp9AB=_tmp9AA.tvars;_tmp9AC=_tmp9AA.effect;_tmp9AD=(void*)_tmp9AA.ret_typ;
_LL610: fd->tvs=_tmp9AB;fd->effect=_tmp9AC;goto _LL60E;_LL611:;_LL612: {const char*
_tmpFF0;void*_tmpFEF;(_tmpFEF=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpFF0="check_fndecl_valid_type: not a FnType",
_tag_dyneither(_tmpFF0,sizeof(char),38))),_tag_dyneither(_tmpFEF,sizeof(void*),0)));}
_LL60E:;}{struct _RegionHandle*_tmp9B0=Cyc_Tcenv_get_fnrgn(te);{const char*_tmpFF1;
Cyc_Tcutil_check_unique_vars(((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct _dyneither_ptr*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_rmap)(
_tmp9B0,(struct _dyneither_ptr*(*)(struct _tuple14*t))Cyc_Tcutil_fst_fdarg,fd->args),
loc,((_tmpFF1="function declaration has repeated parameter",_tag_dyneither(
_tmpFF1,sizeof(char),44))));}{struct Cyc_Core_Opt*_tmpFF2;fd->cached_typ=((
_tmpFF2=_cycalloc(sizeof(*_tmpFF2)),((_tmpFF2->v=(void*)t,_tmpFF2))));}}}void Cyc_Tcutil_check_type(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*
bound_tvars,void*expected_kind,int allow_evars,void*t);void Cyc_Tcutil_check_type(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*
bound_tvars,void*expected_kind,int allow_evars,void*t){struct _RegionHandle*
_tmp9B3=Cyc_Tcenv_get_fnrgn(te);struct Cyc_Tcutil_CVTEnv _tmpFF3;struct Cyc_Tcutil_CVTEnv
_tmp9B4=Cyc_Tcutil_check_valid_type(loc,te,((_tmpFF3.r=_tmp9B3,((_tmpFF3.kind_env=
bound_tvars,((_tmpFF3.free_vars=0,((_tmpFF3.free_evars=0,((_tmpFF3.generalize_evars=
0,((_tmpFF3.fn_result=0,_tmpFF3)))))))))))),expected_kind,t);struct Cyc_List_List*
_tmp9B5=Cyc_Tcutil_remove_bound_tvars(_tmp9B3,((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct Cyc_Absyn_Tvar*(*f)(struct _tuple18*),struct Cyc_List_List*x))
Cyc_List_rmap)(_tmp9B3,(struct Cyc_Absyn_Tvar*(*)(struct _tuple18*))Cyc_Core_fst,
_tmp9B4.free_vars),bound_tvars);struct Cyc_List_List*_tmp9B6=((struct Cyc_List_List*(*)(
struct _RegionHandle*,void*(*f)(struct _tuple19*),struct Cyc_List_List*x))Cyc_List_rmap)(
_tmp9B3,(void*(*)(struct _tuple19*))Cyc_Core_fst,_tmp9B4.free_evars);{struct Cyc_List_List*
fs=_tmp9B5;for(0;fs != 0;fs=fs->tl){struct _dyneither_ptr*_tmp9B7=((struct Cyc_Absyn_Tvar*)
fs->hd)->name;const char*_tmpFF8;void*_tmpFF7[2];struct Cyc_String_pa_struct
_tmpFF6;struct Cyc_String_pa_struct _tmpFF5;(_tmpFF5.tag=0,((_tmpFF5.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpFF6.tag=0,((
_tmpFF6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp9B7),((_tmpFF7[0]=&
_tmpFF6,((_tmpFF7[1]=& _tmpFF5,Cyc_Tcutil_terr(loc,((_tmpFF8="unbound type variable %s in type %s",
_tag_dyneither(_tmpFF8,sizeof(char),36))),_tag_dyneither(_tmpFF7,sizeof(void*),2)))))))))))));}}
if(!allow_evars  && _tmp9B6 != 0)for(0;_tmp9B6 != 0;_tmp9B6=_tmp9B6->tl){void*e=(
void*)_tmp9B6->hd;void*_tmp9BC=Cyc_Tcutil_typ_kind(e);_LL614: if((int)_tmp9BC != 4)
goto _LL616;_LL615: if(!Cyc_Tcutil_unify(e,(void*)3)){const char*_tmpFFB;void*
_tmpFFA;(_tmpFFA=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmpFFB="can't unify evar with unique region!",_tag_dyneither(_tmpFFB,sizeof(
char),37))),_tag_dyneither(_tmpFFA,sizeof(void*),0)));}goto _LL613;_LL616: if((int)
_tmp9BC != 5)goto _LL618;_LL617: goto _LL619;_LL618: if((int)_tmp9BC != 3)goto _LL61A;
_LL619: if(!Cyc_Tcutil_unify(e,(void*)2)){const char*_tmpFFE;void*_tmpFFD;(_tmpFFD=
0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmpFFE="can't unify evar with heap!",_tag_dyneither(_tmpFFE,sizeof(char),28))),
_tag_dyneither(_tmpFFD,sizeof(void*),0)));}goto _LL613;_LL61A: if((int)_tmp9BC != 6)
goto _LL61C;_LL61B: if(!Cyc_Tcutil_unify(e,Cyc_Absyn_empty_effect)){const char*
_tmp1001;void*_tmp1000;(_tmp1000=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1001="can't unify evar with {}!",
_tag_dyneither(_tmp1001,sizeof(char),26))),_tag_dyneither(_tmp1000,sizeof(void*),
0)));}goto _LL613;_LL61C:;_LL61D:{const char*_tmp1006;void*_tmp1005[2];struct Cyc_String_pa_struct
_tmp1004;struct Cyc_String_pa_struct _tmp1003;(_tmp1003.tag=0,((_tmp1003.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((
_tmp1004.tag=0,((_tmp1004.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
e)),((_tmp1005[0]=& _tmp1004,((_tmp1005[1]=& _tmp1003,Cyc_Tcutil_terr(loc,((
_tmp1006="hidden type variable %s isn't abstracted in type %s",_tag_dyneither(
_tmp1006,sizeof(char),52))),_tag_dyneither(_tmp1005,sizeof(void*),2)))))))))))));}
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
vs->hd,(void*)vs2->hd)== 0){const char*_tmp100B;void*_tmp100A[2];struct Cyc_String_pa_struct
_tmp1009;struct Cyc_String_pa_struct _tmp1008;(_tmp1008.tag=0,((_tmp1008.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)a2string((void*)vs->hd)),((_tmp1009.tag=
0,((_tmp1009.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg),((_tmp100A[0]=&
_tmp1009,((_tmp100A[1]=& _tmp1008,Cyc_Tcutil_terr(loc,((_tmp100B="%s: %s",
_tag_dyneither(_tmp100B,sizeof(char),7))),_tag_dyneither(_tmp100A,sizeof(void*),
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
struct Cyc_Position_Segment*loc,struct Cyc_List_List*tvs){const char*_tmp100C;((
void(*)(int(*cmp)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*
vs,struct Cyc_Position_Segment*loc,struct _dyneither_ptr(*a2string)(struct Cyc_Absyn_Tvar*),
struct _dyneither_ptr msg))Cyc_Tcutil_check_unique_unsorted)(Cyc_Absyn_tvar_cmp,
tvs,loc,Cyc_Tcutil_tvar2string,((_tmp100C="duplicate type variable",
_tag_dyneither(_tmp100C,sizeof(char),24))));}struct _tuple20{struct Cyc_Absyn_Aggrfield*
f1;int f2;};struct _tuple21{struct Cyc_List_List*f1;void*f2;};struct _tuple22{struct
Cyc_Absyn_Aggrfield*f1;void*f2;};struct Cyc_List_List*Cyc_Tcutil_resolve_struct_designators(
struct _RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,
struct Cyc_List_List*sdfields);struct Cyc_List_List*Cyc_Tcutil_resolve_struct_designators(
struct _RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,
struct Cyc_List_List*sdfields){struct Cyc_List_List*fields=0;{struct Cyc_List_List*
sd_fields=sdfields;for(0;sd_fields != 0;sd_fields=sd_fields->tl){const char*
_tmp100D;if(Cyc_strcmp((struct _dyneither_ptr)*((struct Cyc_Absyn_Aggrfield*)
sd_fields->hd)->name,((_tmp100D="",_tag_dyneither(_tmp100D,sizeof(char),1))))!= 
0){struct _tuple20*_tmp1010;struct Cyc_List_List*_tmp100F;fields=((_tmp100F=
_cycalloc(sizeof(*_tmp100F)),((_tmp100F->hd=((_tmp1010=_cycalloc(sizeof(*
_tmp1010)),((_tmp1010->f1=(struct Cyc_Absyn_Aggrfield*)sd_fields->hd,((_tmp1010->f2=
0,_tmp1010)))))),((_tmp100F->tl=fields,_tmp100F))))));}}}fields=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(fields);{struct Cyc_List_List*ans=0;for(
0;des != 0;des=des->tl){struct _tuple21 _tmp9D1;struct Cyc_List_List*_tmp9D2;void*
_tmp9D3;struct _tuple21*_tmp9D0=(struct _tuple21*)des->hd;_tmp9D1=*_tmp9D0;_tmp9D2=
_tmp9D1.f1;_tmp9D3=_tmp9D1.f2;if(_tmp9D2 == 0){struct Cyc_List_List*_tmp9D4=fields;
for(0;_tmp9D4 != 0;_tmp9D4=_tmp9D4->tl){if(!(*((struct _tuple20*)_tmp9D4->hd)).f2){(*((
struct _tuple20*)_tmp9D4->hd)).f2=1;{struct Cyc_Absyn_FieldName_struct*_tmp1016;
struct Cyc_Absyn_FieldName_struct _tmp1015;struct Cyc_List_List*_tmp1014;(*((struct
_tuple21*)des->hd)).f1=(struct Cyc_List_List*)((_tmp1014=_cycalloc(sizeof(*
_tmp1014)),((_tmp1014->hd=(void*)((void*)((_tmp1016=_cycalloc(sizeof(*_tmp1016)),((
_tmp1016[0]=((_tmp1015.tag=1,((_tmp1015.f1=((*((struct _tuple20*)_tmp9D4->hd)).f1)->name,
_tmp1015)))),_tmp1016))))),((_tmp1014->tl=0,_tmp1014))))));}{struct _tuple22*
_tmp1019;struct Cyc_List_List*_tmp1018;ans=((_tmp1018=_region_malloc(rgn,sizeof(*
_tmp1018)),((_tmp1018->hd=((_tmp1019=_region_malloc(rgn,sizeof(*_tmp1019)),((
_tmp1019->f1=(*((struct _tuple20*)_tmp9D4->hd)).f1,((_tmp1019->f2=_tmp9D3,
_tmp1019)))))),((_tmp1018->tl=ans,_tmp1018))))));}break;}}if(_tmp9D4 == 0){const
char*_tmp101C;void*_tmp101B;(_tmp101B=0,Cyc_Tcutil_terr(loc,((_tmp101C="too many arguments to struct",
_tag_dyneither(_tmp101C,sizeof(char),29))),_tag_dyneither(_tmp101B,sizeof(void*),
0)));}}else{if(_tmp9D2->tl != 0){const char*_tmp101F;void*_tmp101E;(_tmp101E=0,Cyc_Tcutil_terr(
loc,((_tmp101F="multiple designators are not supported",_tag_dyneither(_tmp101F,
sizeof(char),39))),_tag_dyneither(_tmp101E,sizeof(void*),0)));}else{void*_tmp9DE=(
void*)_tmp9D2->hd;struct _dyneither_ptr*_tmp9DF;_LL61F: if(*((int*)_tmp9DE)!= 0)
goto _LL621;_LL620:{const char*_tmp1022;void*_tmp1021;(_tmp1021=0,Cyc_Tcutil_terr(
loc,((_tmp1022="array designator used in argument to struct",_tag_dyneither(
_tmp1022,sizeof(char),44))),_tag_dyneither(_tmp1021,sizeof(void*),0)));}goto
_LL61E;_LL621: if(*((int*)_tmp9DE)!= 1)goto _LL61E;_tmp9DF=((struct Cyc_Absyn_FieldName_struct*)
_tmp9DE)->f1;_LL622: {struct Cyc_List_List*_tmp9E2=fields;for(0;_tmp9E2 != 0;
_tmp9E2=_tmp9E2->tl){if(Cyc_strptrcmp(_tmp9DF,((*((struct _tuple20*)_tmp9E2->hd)).f1)->name)
== 0){if((*((struct _tuple20*)_tmp9E2->hd)).f2){const char*_tmp1026;void*_tmp1025[
1];struct Cyc_String_pa_struct _tmp1024;(_tmp1024.tag=0,((_tmp1024.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*_tmp9DF),((_tmp1025[0]=& _tmp1024,Cyc_Tcutil_terr(
loc,((_tmp1026="field %s has already been used as an argument",_tag_dyneither(
_tmp1026,sizeof(char),46))),_tag_dyneither(_tmp1025,sizeof(void*),1)))))));}(*((
struct _tuple20*)_tmp9E2->hd)).f2=1;{struct _tuple22*_tmp1029;struct Cyc_List_List*
_tmp1028;ans=((_tmp1028=_region_malloc(rgn,sizeof(*_tmp1028)),((_tmp1028->hd=((
_tmp1029=_region_malloc(rgn,sizeof(*_tmp1029)),((_tmp1029->f1=(*((struct _tuple20*)
_tmp9E2->hd)).f1,((_tmp1029->f2=_tmp9D3,_tmp1029)))))),((_tmp1028->tl=ans,
_tmp1028))))));}break;}}if(_tmp9E2 == 0){const char*_tmp102D;void*_tmp102C[1];
struct Cyc_String_pa_struct _tmp102B;(_tmp102B.tag=0,((_tmp102B.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*_tmp9DF),((_tmp102C[0]=& _tmp102B,Cyc_Tcutil_terr(
loc,((_tmp102D="bad field designator %s",_tag_dyneither(_tmp102D,sizeof(char),24))),
_tag_dyneither(_tmp102C,sizeof(void*),1)))))));}goto _LL61E;}_LL61E:;}}}for(0;
fields != 0;fields=fields->tl){if(!(*((struct _tuple20*)fields->hd)).f2){{const
char*_tmp1030;void*_tmp102F;(_tmp102F=0,Cyc_Tcutil_terr(loc,((_tmp1030="too few arguments to struct",
_tag_dyneither(_tmp1030,sizeof(char),28))),_tag_dyneither(_tmp102F,sizeof(void*),
0)));}break;}}return((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
ans);}}int Cyc_Tcutil_is_tagged_pointer_typ_elt(void*t,void**elt_typ_dest);int Cyc_Tcutil_is_tagged_pointer_typ_elt(
void*t,void**elt_typ_dest){void*_tmp9ED=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo
_tmp9EE;void*_tmp9EF;struct Cyc_Absyn_PtrAtts _tmp9F0;struct Cyc_Absyn_Conref*
_tmp9F1;_LL624: if(_tmp9ED <= (void*)4)goto _LL626;if(*((int*)_tmp9ED)!= 4)goto
_LL626;_tmp9EE=((struct Cyc_Absyn_PointerType_struct*)_tmp9ED)->f1;_tmp9EF=(void*)
_tmp9EE.elt_typ;_tmp9F0=_tmp9EE.ptr_atts;_tmp9F1=_tmp9F0.bounds;_LL625: {struct
Cyc_Absyn_Conref*_tmp9F2=Cyc_Absyn_compress_conref(_tmp9F1);union Cyc_Absyn_Constraint_union
_tmp9F3=(Cyc_Absyn_compress_conref(_tmp9F2))->v;void*_tmp9F4;_LL629: if((_tmp9F3.Eq_constr).tag
!= 0)goto _LL62B;_tmp9F4=(_tmp9F3.Eq_constr).f1;if((int)_tmp9F4 != 0)goto _LL62B;
_LL62A:*elt_typ_dest=_tmp9EF;return 1;_LL62B: if((_tmp9F3.No_constr).tag != 2)goto
_LL62D;_LL62C:{union Cyc_Absyn_Constraint_union _tmp1031;_tmp9F2->v=(union Cyc_Absyn_Constraint_union)(((
_tmp1031.Eq_constr).tag=0,(((_tmp1031.Eq_constr).f1=(void*)((void*)0),_tmp1031))));}*
elt_typ_dest=_tmp9EF;return 1;_LL62D:;_LL62E: return 0;_LL628:;}_LL626:;_LL627:
return 0;_LL623:;}int Cyc_Tcutil_is_zero_pointer_typ_elt(void*t,void**elt_typ_dest);
int Cyc_Tcutil_is_zero_pointer_typ_elt(void*t,void**elt_typ_dest){void*_tmp9F6=
Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp9F7;void*_tmp9F8;struct Cyc_Absyn_PtrAtts
_tmp9F9;struct Cyc_Absyn_Conref*_tmp9FA;_LL630: if(_tmp9F6 <= (void*)4)goto _LL632;
if(*((int*)_tmp9F6)!= 4)goto _LL632;_tmp9F7=((struct Cyc_Absyn_PointerType_struct*)
_tmp9F6)->f1;_tmp9F8=(void*)_tmp9F7.elt_typ;_tmp9F9=_tmp9F7.ptr_atts;_tmp9FA=
_tmp9F9.zero_term;_LL631:*elt_typ_dest=_tmp9F8;return((int(*)(int,struct Cyc_Absyn_Conref*
x))Cyc_Absyn_conref_def)(0,_tmp9FA);_LL632:;_LL633: return 0;_LL62F:;}int Cyc_Tcutil_is_zero_ptr_type(
void*t,void**ptr_type,int*is_dyneither,void**elt_type);int Cyc_Tcutil_is_zero_ptr_type(
void*t,void**ptr_type,int*is_dyneither,void**elt_type){void*_tmp9FB=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_PtrInfo _tmp9FC;void*_tmp9FD;struct Cyc_Absyn_PtrAtts _tmp9FE;
struct Cyc_Absyn_Conref*_tmp9FF;struct Cyc_Absyn_Conref*_tmpA00;struct Cyc_Absyn_ArrayInfo
_tmpA01;void*_tmpA02;struct Cyc_Absyn_Tqual _tmpA03;struct Cyc_Absyn_Exp*_tmpA04;
struct Cyc_Absyn_Conref*_tmpA05;_LL635: if(_tmp9FB <= (void*)4)goto _LL639;if(*((int*)
_tmp9FB)!= 4)goto _LL637;_tmp9FC=((struct Cyc_Absyn_PointerType_struct*)_tmp9FB)->f1;
_tmp9FD=(void*)_tmp9FC.elt_typ;_tmp9FE=_tmp9FC.ptr_atts;_tmp9FF=_tmp9FE.bounds;
_tmpA00=_tmp9FE.zero_term;_LL636: if(((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,_tmpA00)){*ptr_type=t;*elt_type=_tmp9FD;{void*_tmpA06=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,
_tmp9FF);_LL63C: if((int)_tmpA06 != 0)goto _LL63E;_LL63D:*is_dyneither=1;goto _LL63B;
_LL63E:;_LL63F:*is_dyneither=0;goto _LL63B;_LL63B:;}return 1;}else{return 0;}_LL637:
if(*((int*)_tmp9FB)!= 7)goto _LL639;_tmpA01=((struct Cyc_Absyn_ArrayType_struct*)
_tmp9FB)->f1;_tmpA02=(void*)_tmpA01.elt_type;_tmpA03=_tmpA01.tq;_tmpA04=_tmpA01.num_elts;
_tmpA05=_tmpA01.zero_term;_LL638: if(((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,_tmpA05)){*elt_type=_tmpA02;*is_dyneither=0;{struct Cyc_Absyn_PointerType_struct
_tmp1046;struct Cyc_Absyn_PtrAtts _tmp1045;struct Cyc_Absyn_Upper_b_struct _tmp1044;
struct Cyc_Absyn_Upper_b_struct*_tmp1043;struct Cyc_Absyn_PtrInfo _tmp1042;struct
Cyc_Absyn_PointerType_struct*_tmp1041;*ptr_type=(void*)((_tmp1041=_cycalloc(
sizeof(*_tmp1041)),((_tmp1041[0]=((_tmp1046.tag=4,((_tmp1046.f1=((_tmp1042.elt_typ=(
void*)_tmpA02,((_tmp1042.elt_tq=_tmpA03,((_tmp1042.ptr_atts=((_tmp1045.rgn=(void*)((
void*)2),((_tmp1045.nullable=Cyc_Absyn_false_conref,((_tmp1045.bounds=Cyc_Absyn_new_conref((
void*)((_tmp1043=_cycalloc(sizeof(*_tmp1043)),((_tmp1043[0]=((_tmp1044.tag=0,((
_tmp1044.f1=(struct Cyc_Absyn_Exp*)_check_null(_tmpA04),_tmp1044)))),_tmp1043))))),((
_tmp1045.zero_term=_tmpA05,((_tmp1045.ptrloc=0,_tmp1045)))))))))),_tmp1042)))))),
_tmp1046)))),_tmp1041))));}return 1;}else{return 0;}_LL639:;_LL63A: return 0;_LL634:;}
int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*e1,void**ptr_type,int*
is_dyneither,void**elt_type);int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*
e1,void**ptr_type,int*is_dyneither,void**elt_type){void*_tmpA0D=(void*)e1->r;
struct Cyc_Absyn_Exp*_tmpA0E;struct Cyc_Absyn_Exp*_tmpA0F;struct Cyc_Absyn_Exp*
_tmpA10;struct Cyc_Absyn_Exp*_tmpA11;struct Cyc_Absyn_Exp*_tmpA12;struct Cyc_Absyn_Exp*
_tmpA13;_LL641: if(*((int*)_tmpA0D)!= 15)goto _LL643;_LL642: {const char*_tmp104A;
void*_tmp1049[1];struct Cyc_String_pa_struct _tmp1048;(_tmp1048.tag=0,((_tmp1048.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e1)),((
_tmp1049[0]=& _tmp1048,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmp104A="we have a cast in a lhs:  %s",_tag_dyneither(
_tmp104A,sizeof(char),29))),_tag_dyneither(_tmp1049,sizeof(void*),1)))))));}
_LL643: if(*((int*)_tmpA0D)!= 22)goto _LL645;_tmpA0E=((struct Cyc_Absyn_Deref_e_struct*)
_tmpA0D)->f1;_LL644: _tmpA0F=_tmpA0E;goto _LL646;_LL645: if(*((int*)_tmpA0D)!= 25)
goto _LL647;_tmpA0F=((struct Cyc_Absyn_Subscript_e_struct*)_tmpA0D)->f1;_LL646:
return Cyc_Tcutil_is_zero_ptr_type((void*)((struct Cyc_Core_Opt*)_check_null(
_tmpA0F->topt))->v,ptr_type,is_dyneither,elt_type);_LL647: if(*((int*)_tmpA0D)!= 
24)goto _LL649;_tmpA10=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmpA0D)->f1;_LL648:
_tmpA11=_tmpA10;goto _LL64A;_LL649: if(*((int*)_tmpA0D)!= 23)goto _LL64B;_tmpA11=((
struct Cyc_Absyn_AggrMember_e_struct*)_tmpA0D)->f1;_LL64A: if(Cyc_Tcutil_is_zero_ptr_type((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpA11->topt))->v,ptr_type,is_dyneither,
elt_type)){const char*_tmp104E;void*_tmp104D[1];struct Cyc_String_pa_struct
_tmp104C;(_tmp104C.tag=0,((_tmp104C.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_exp2string(e1)),((_tmp104D[0]=& _tmp104C,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp104E="found zero pointer aggregate member assignment: %s",
_tag_dyneither(_tmp104E,sizeof(char),51))),_tag_dyneither(_tmp104D,sizeof(void*),
1)))))));}return 0;_LL64B: if(*((int*)_tmpA0D)!= 14)goto _LL64D;_tmpA12=((struct Cyc_Absyn_Instantiate_e_struct*)
_tmpA0D)->f1;_LL64C: _tmpA13=_tmpA12;goto _LL64E;_LL64D: if(*((int*)_tmpA0D)!= 13)
goto _LL64F;_tmpA13=((struct Cyc_Absyn_NoInstantiate_e_struct*)_tmpA0D)->f1;_LL64E:
if(Cyc_Tcutil_is_zero_ptr_type((void*)((struct Cyc_Core_Opt*)_check_null(_tmpA13->topt))->v,
ptr_type,is_dyneither,elt_type)){const char*_tmp1052;void*_tmp1051[1];struct Cyc_String_pa_struct
_tmp1050;(_tmp1050.tag=0,((_tmp1050.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_exp2string(e1)),((_tmp1051[0]=& _tmp1050,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1052="found zero pointer instantiate/noinstantiate: %s",
_tag_dyneither(_tmp1052,sizeof(char),49))),_tag_dyneither(_tmp1051,sizeof(void*),
1)))))));}return 0;_LL64F: if(*((int*)_tmpA0D)!= 1)goto _LL651;_LL650: return 0;
_LL651:;_LL652: {const char*_tmp1056;void*_tmp1055[1];struct Cyc_String_pa_struct
_tmp1054;(_tmp1054.tag=0,((_tmp1054.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_exp2string(e1)),((_tmp1055[0]=& _tmp1054,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1056="found bad lhs in is_zero_ptr_deref: %s",
_tag_dyneither(_tmp1056,sizeof(char),39))),_tag_dyneither(_tmp1055,sizeof(void*),
1)))))));}_LL640:;}int Cyc_Tcutil_is_tagged_pointer_typ(void*t);int Cyc_Tcutil_is_tagged_pointer_typ(
void*t){void*ignore=(void*)0;return Cyc_Tcutil_is_tagged_pointer_typ_elt(t,&
ignore);}static int Cyc_Tcutil_is_noalias_region(void*r);static int Cyc_Tcutil_is_noalias_region(
void*r){void*_tmpA20=Cyc_Tcutil_compress(r);struct Cyc_Absyn_Tvar*_tmpA21;_LL654:
if((int)_tmpA20 != 3)goto _LL656;_LL655: return 1;_LL656: if(_tmpA20 <= (void*)4)goto
_LL658;if(*((int*)_tmpA20)!= 1)goto _LL658;_tmpA21=((struct Cyc_Absyn_VarType_struct*)
_tmpA20)->f1;_LL657: return Cyc_Tcutil_tvar_kind(_tmpA21)== (void*)4  || Cyc_Tcutil_tvar_kind(
_tmpA21)== (void*)5;_LL658:;_LL659: return 0;_LL653:;}int Cyc_Tcutil_is_noalias_pointer(
void*t);int Cyc_Tcutil_is_noalias_pointer(void*t){void*_tmpA22=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_PtrInfo _tmpA23;struct Cyc_Absyn_PtrAtts _tmpA24;void*_tmpA25;
_LL65B: if(_tmpA22 <= (void*)4)goto _LL65D;if(*((int*)_tmpA22)!= 4)goto _LL65D;
_tmpA23=((struct Cyc_Absyn_PointerType_struct*)_tmpA22)->f1;_tmpA24=_tmpA23.ptr_atts;
_tmpA25=(void*)_tmpA24.rgn;_LL65C: return Cyc_Tcutil_is_noalias_region(_tmpA25);
_LL65D:;_LL65E: return 0;_LL65A:;}int Cyc_Tcutil_is_noalias_pointer_or_aggr(struct
_RegionHandle*rgn,void*t);int Cyc_Tcutil_is_noalias_pointer_or_aggr(struct
_RegionHandle*rgn,void*t){void*_tmpA26=Cyc_Tcutil_compress(t);if(Cyc_Tcutil_is_noalias_pointer(
_tmpA26))return 1;{void*_tmpA27=_tmpA26;struct Cyc_List_List*_tmpA28;struct Cyc_Absyn_AggrInfo
_tmpA29;union Cyc_Absyn_AggrInfoU_union _tmpA2A;struct Cyc_Absyn_Aggrdecl**_tmpA2B;
struct Cyc_List_List*_tmpA2C;struct Cyc_List_List*_tmpA2D;struct Cyc_Absyn_AggrInfo
_tmpA2E;union Cyc_Absyn_AggrInfoU_union _tmpA2F;struct Cyc_Absyn_DatatypeInfo
_tmpA30;union Cyc_Absyn_DatatypeInfoU_union _tmpA31;struct Cyc_List_List*_tmpA32;
struct Cyc_Core_Opt*_tmpA33;struct Cyc_Absyn_DatatypeFieldInfo _tmpA34;union Cyc_Absyn_DatatypeFieldInfoU_union
_tmpA35;struct Cyc_List_List*_tmpA36;_LL660: if(_tmpA27 <= (void*)4)goto _LL66C;if(*((
int*)_tmpA27)!= 9)goto _LL662;_tmpA28=((struct Cyc_Absyn_TupleType_struct*)_tmpA27)->f1;
_LL661: while(_tmpA28 != 0){if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,(*((
struct _tuple5*)_tmpA28->hd)).f2))return 1;_tmpA28=_tmpA28->tl;}return 0;_LL662: if(*((
int*)_tmpA27)!= 10)goto _LL664;_tmpA29=((struct Cyc_Absyn_AggrType_struct*)_tmpA27)->f1;
_tmpA2A=_tmpA29.aggr_info;if((((((struct Cyc_Absyn_AggrType_struct*)_tmpA27)->f1).aggr_info).KnownAggr).tag
!= 1)goto _LL664;_tmpA2B=(_tmpA2A.KnownAggr).f1;_tmpA2C=_tmpA29.targs;_LL663: if((*
_tmpA2B)->impl == 0)return 0;else{struct Cyc_List_List*_tmpA37=((struct Cyc_List_List*(*)(
struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(rgn,rgn,(*_tmpA2B)->tvs,_tmpA2C);struct Cyc_List_List*_tmpA38=((
struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmpA2B)->impl))->fields;void*t;
while(_tmpA38 != 0){t=Cyc_Tcutil_rsubstitute(rgn,_tmpA37,(void*)((struct Cyc_Absyn_Aggrfield*)
_tmpA38->hd)->type);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,t))return 1;
_tmpA38=_tmpA38->tl;}return 0;}_LL664: if(*((int*)_tmpA27)!= 11)goto _LL666;_tmpA2D=((
struct Cyc_Absyn_AnonAggrType_struct*)_tmpA27)->f2;_LL665: while(_tmpA2D != 0){if(
Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,(void*)((struct Cyc_Absyn_Aggrfield*)
_tmpA2D->hd)->type))return 1;_tmpA2D=_tmpA2D->tl;}return 0;_LL666: if(*((int*)
_tmpA27)!= 10)goto _LL668;_tmpA2E=((struct Cyc_Absyn_AggrType_struct*)_tmpA27)->f1;
_tmpA2F=_tmpA2E.aggr_info;if((((((struct Cyc_Absyn_AggrType_struct*)_tmpA27)->f1).aggr_info).UnknownAggr).tag
!= 0)goto _LL668;_LL667: {const char*_tmp1059;void*_tmp1058;(_tmp1058=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1059="got unknown aggr in is_noalias_aggr",
_tag_dyneither(_tmp1059,sizeof(char),36))),_tag_dyneither(_tmp1058,sizeof(void*),
0)));}_LL668: if(*((int*)_tmpA27)!= 2)goto _LL66A;_tmpA30=((struct Cyc_Absyn_DatatypeType_struct*)
_tmpA27)->f1;_tmpA31=_tmpA30.datatype_info;_tmpA32=_tmpA30.targs;_tmpA33=_tmpA30.rgn;
_LL669: {union Cyc_Absyn_DatatypeInfoU_union _tmpA3B=_tmpA31;struct Cyc_Absyn_UnknownDatatypeInfo
_tmpA3C;struct _tuple2*_tmpA3D;int _tmpA3E;int _tmpA3F;struct Cyc_Absyn_Datatypedecl**
_tmpA40;struct Cyc_Absyn_Datatypedecl*_tmpA41;struct Cyc_Absyn_Datatypedecl _tmpA42;
struct Cyc_List_List*_tmpA43;struct Cyc_Core_Opt*_tmpA44;int _tmpA45;_LL66F: if((
_tmpA3B.UnknownDatatype).tag != 0)goto _LL671;_tmpA3C=(_tmpA3B.UnknownDatatype).f1;
_tmpA3D=_tmpA3C.name;_tmpA3E=_tmpA3C.is_extensible;_tmpA3F=_tmpA3C.is_flat;
_LL670: {const char*_tmp105C;void*_tmp105B;(_tmp105B=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp105C="got unknown datatype in is_noalias_aggr",
_tag_dyneither(_tmp105C,sizeof(char),40))),_tag_dyneither(_tmp105B,sizeof(void*),
0)));}_LL671: if((_tmpA3B.KnownDatatype).tag != 1)goto _LL66E;_tmpA40=(_tmpA3B.KnownDatatype).f1;
_tmpA41=*_tmpA40;_tmpA42=*_tmpA41;_tmpA43=_tmpA42.tvs;_tmpA44=_tmpA42.fields;
_tmpA45=_tmpA42.is_flat;_LL672: if(!_tmpA45){if(_tmpA33 == 0)return 0;return Cyc_Tcutil_is_noalias_region((
void*)_tmpA33->v);}else{if(_tmpA44 == 0){const char*_tmp105F;void*_tmp105E;(
_tmp105E=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp105F="got datatype with no fields",_tag_dyneither(_tmp105F,sizeof(char),28))),
_tag_dyneither(_tmp105E,sizeof(void*),0)));}else{struct Cyc_List_List*_tmpA4A=((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmpA43,_tmpA32);struct Cyc_List_List*
_tmpA4B=(struct Cyc_List_List*)_tmpA44->v;while(_tmpA4B != 0){struct Cyc_List_List*
_tmpA4C=((struct Cyc_Absyn_Datatypefield*)_tmpA4B->hd)->typs;while(_tmpA4C != 0){
_tmpA26=Cyc_Tcutil_rsubstitute(rgn,_tmpA4A,(*((struct _tuple5*)_tmpA4C->hd)).f2);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,_tmpA26))return 1;_tmpA4C=_tmpA4C->tl;}
_tmpA4B=_tmpA4B->tl;}return 0;}}_LL66E:;}_LL66A: if(*((int*)_tmpA27)!= 3)goto
_LL66C;_tmpA34=((struct Cyc_Absyn_DatatypeFieldType_struct*)_tmpA27)->f1;_tmpA35=
_tmpA34.field_info;_tmpA36=_tmpA34.targs;_LL66B: {union Cyc_Absyn_DatatypeFieldInfoU_union
_tmpA4D=_tmpA35;struct Cyc_Absyn_Datatypedecl*_tmpA4E;struct Cyc_Absyn_Datatypefield*
_tmpA4F;_LL674: if((_tmpA4D.UnknownDatatypefield).tag != 0)goto _LL676;_LL675: {
const char*_tmp1062;void*_tmp1061;(_tmp1061=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1062="got unknown datatype field in is_noalias_aggr",
_tag_dyneither(_tmp1062,sizeof(char),46))),_tag_dyneither(_tmp1061,sizeof(void*),
0)));}_LL676: if((_tmpA4D.KnownDatatypefield).tag != 1)goto _LL673;_tmpA4E=(_tmpA4D.KnownDatatypefield).f1;
_tmpA4F=(_tmpA4D.KnownDatatypefield).f2;_LL677: {struct Cyc_List_List*_tmpA52=((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmpA4E->tvs,_tmpA36);struct Cyc_List_List*
_tmpA53=_tmpA4F->typs;while(_tmpA53 != 0){_tmpA26=Cyc_Tcutil_rsubstitute(rgn,
_tmpA52,(*((struct _tuple5*)_tmpA53->hd)).f2);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(
rgn,_tmpA26))return 1;_tmpA53=_tmpA53->tl;}return 0;}_LL673:;}_LL66C:;_LL66D:
return 0;_LL65F:;}}static int Cyc_Tcutil_is_noalias_field(struct _RegionHandle*r,
void*t,struct _dyneither_ptr*f);static int Cyc_Tcutil_is_noalias_field(struct
_RegionHandle*r,void*t,struct _dyneither_ptr*f){void*_tmpA54=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_AggrInfo _tmpA55;union Cyc_Absyn_AggrInfoU_union _tmpA56;struct
Cyc_Absyn_Aggrdecl**_tmpA57;struct Cyc_Absyn_Aggrdecl*_tmpA58;struct Cyc_List_List*
_tmpA59;struct Cyc_List_List*_tmpA5A;_LL679: if(_tmpA54 <= (void*)4)goto _LL67D;if(*((
int*)_tmpA54)!= 10)goto _LL67B;_tmpA55=((struct Cyc_Absyn_AggrType_struct*)_tmpA54)->f1;
_tmpA56=_tmpA55.aggr_info;if((((((struct Cyc_Absyn_AggrType_struct*)_tmpA54)->f1).aggr_info).KnownAggr).tag
!= 1)goto _LL67B;_tmpA57=(_tmpA56.KnownAggr).f1;_tmpA58=*_tmpA57;_tmpA59=_tmpA55.targs;
_LL67A: _tmpA5A=_tmpA58->impl == 0?0:((struct Cyc_Absyn_AggrdeclImpl*)_check_null(
_tmpA58->impl))->fields;goto _LL67C;_LL67B: if(*((int*)_tmpA54)!= 11)goto _LL67D;
_tmpA5A=((struct Cyc_Absyn_AnonAggrType_struct*)_tmpA54)->f2;_LL67C: {struct Cyc_Absyn_Aggrfield*
_tmpA5B=Cyc_Absyn_lookup_field(_tmpA5A,f);{struct Cyc_Absyn_Aggrfield*_tmpA5C=
_tmpA5B;struct Cyc_Absyn_Aggrfield _tmpA5D;void*_tmpA5E;_LL680: if(_tmpA5C != 0)goto
_LL682;_LL681: {const char*_tmp1065;void*_tmp1064;(_tmp1064=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1065="is_noalias_field: missing field",
_tag_dyneither(_tmp1065,sizeof(char),32))),_tag_dyneither(_tmp1064,sizeof(void*),
0)));}_LL682: if(_tmpA5C == 0)goto _LL67F;_tmpA5D=*_tmpA5C;_tmpA5E=(void*)_tmpA5D.type;
_LL683: return Cyc_Tcutil_is_noalias_pointer_or_aggr(r,_tmpA5E);_LL67F:;}return 0;}
_LL67D:;_LL67E: {const char*_tmp1069;void*_tmp1068[1];struct Cyc_String_pa_struct
_tmp1067;(_tmp1067.tag=0,((_tmp1067.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp1068[0]=& _tmp1067,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1069="is_noalias_field: invalid type |%s|",
_tag_dyneither(_tmp1069,sizeof(char),36))),_tag_dyneither(_tmp1068,sizeof(void*),
1)))))));}_LL678:;}static int Cyc_Tcutil_is_noalias_path_aux(struct _RegionHandle*r,
struct Cyc_Absyn_Exp*e,int ignore_leaf);static int Cyc_Tcutil_is_noalias_path_aux(
struct _RegionHandle*r,struct Cyc_Absyn_Exp*e,int ignore_leaf){void*_tmpA64=(void*)
e->r;void*_tmpA65;struct Cyc_Absyn_Exp*_tmpA66;struct _dyneither_ptr*_tmpA67;
struct Cyc_Absyn_Exp*_tmpA68;struct Cyc_Absyn_Exp*_tmpA69;void*_tmpA6A;void*
_tmpA6B;void*_tmpA6C;struct Cyc_Absyn_Exp*_tmpA6D;struct Cyc_Absyn_Exp*_tmpA6E;
struct Cyc_Absyn_Exp*_tmpA6F;struct Cyc_Absyn_Exp*_tmpA70;void*_tmpA71;struct Cyc_Absyn_Exp*
_tmpA72;struct Cyc_Absyn_Stmt*_tmpA73;_LL685: if(*((int*)_tmpA64)!= 5)goto _LL687;
_LL686: goto _LL688;_LL687: if(*((int*)_tmpA64)!= 7)goto _LL689;_LL688: goto _LL68A;
_LL689: if(*((int*)_tmpA64)!= 8)goto _LL68B;_LL68A: goto _LL68C;_LL68B: if(*((int*)
_tmpA64)!= 12)goto _LL68D;_LL68C: goto _LL68E;_LL68D: if(*((int*)_tmpA64)!= 16)goto
_LL68F;_LL68E: goto _LL690;_LL68F: if(*((int*)_tmpA64)!= 18)goto _LL691;_LL690: goto
_LL692;_LL691: if(*((int*)_tmpA64)!= 19)goto _LL693;_LL692: goto _LL694;_LL693: if(*((
int*)_tmpA64)!= 20)goto _LL695;_LL694: goto _LL696;_LL695: if(*((int*)_tmpA64)!= 21)
goto _LL697;_LL696: goto _LL698;_LL697: if(*((int*)_tmpA64)!= 27)goto _LL699;_LL698:
goto _LL69A;_LL699: if(*((int*)_tmpA64)!= 29)goto _LL69B;_LL69A: goto _LL69C;_LL69B:
if(*((int*)_tmpA64)!= 28)goto _LL69D;_LL69C: goto _LL69E;_LL69D: if(*((int*)_tmpA64)
!= 33)goto _LL69F;_LL69E: goto _LL6A0;_LL69F: if(*((int*)_tmpA64)!= 34)goto _LL6A1;
_LL6A0: goto _LL6A2;_LL6A1: if(*((int*)_tmpA64)!= 36)goto _LL6A3;_LL6A2: goto _LL6A4;
_LL6A3: if(*((int*)_tmpA64)!= 1)goto _LL6A5;_tmpA65=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmpA64)->f2;if(_tmpA65 <= (void*)1)goto _LL6A5;if(*((int*)_tmpA65)!= 0)goto _LL6A5;
_LL6A4: goto _LL6A6;_LL6A5: if(*((int*)_tmpA64)!= 3)goto _LL6A7;_LL6A6: return 0;
_LL6A7: if(*((int*)_tmpA64)!= 22)goto _LL6A9;_LL6A8: goto _LL6AA;_LL6A9: if(*((int*)
_tmpA64)!= 24)goto _LL6AB;_LL6AA: return 0;_LL6AB: if(*((int*)_tmpA64)!= 23)goto
_LL6AD;_tmpA66=((struct Cyc_Absyn_AggrMember_e_struct*)_tmpA64)->f1;_tmpA67=((
struct Cyc_Absyn_AggrMember_e_struct*)_tmpA64)->f2;_LL6AC: return(ignore_leaf  || 
Cyc_Tcutil_is_noalias_field(r,(void*)((struct Cyc_Core_Opt*)_check_null(_tmpA66->topt))->v,
_tmpA67)) && Cyc_Tcutil_is_noalias_path_aux(r,_tmpA66,0);_LL6AD: if(*((int*)
_tmpA64)!= 25)goto _LL6AF;_tmpA68=((struct Cyc_Absyn_Subscript_e_struct*)_tmpA64)->f1;
_tmpA69=((struct Cyc_Absyn_Subscript_e_struct*)_tmpA64)->f2;_LL6AE: {void*_tmpA74=
Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmpA68->topt))->v);
struct Cyc_Absyn_PtrInfo _tmpA75;void*_tmpA76;struct Cyc_List_List*_tmpA77;_LL6D2:
if(_tmpA74 <= (void*)4)goto _LL6D6;if(*((int*)_tmpA74)!= 4)goto _LL6D4;_tmpA75=((
struct Cyc_Absyn_PointerType_struct*)_tmpA74)->f1;_tmpA76=(void*)_tmpA75.elt_typ;
_LL6D3: return 0;_LL6D4: if(*((int*)_tmpA74)!= 9)goto _LL6D6;_tmpA77=((struct Cyc_Absyn_TupleType_struct*)
_tmpA74)->f1;_LL6D5: {unsigned int _tmpA79;int _tmpA7A;struct _tuple7 _tmpA78=Cyc_Evexp_eval_const_uint_exp(
_tmpA69);_tmpA79=_tmpA78.f1;_tmpA7A=_tmpA78.f2;if(!_tmpA7A){const char*_tmp106C;
void*_tmp106B;(_tmp106B=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap))Cyc_Tcutil_impos)(((_tmp106C="is_noalias_path_aux: non-constant subscript",
_tag_dyneither(_tmp106C,sizeof(char),44))),_tag_dyneither(_tmp106B,sizeof(void*),
0)));}{struct _handler_cons _tmpA7D;_push_handler(& _tmpA7D);{int _tmpA7F=0;if(
setjmp(_tmpA7D.handler))_tmpA7F=1;if(!_tmpA7F){{void*_tmpA80=(*((struct _tuple5*(*)(
struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmpA77,(int)_tmpA79)).f2;int _tmpA81=(
ignore_leaf  || Cyc_Tcutil_is_noalias_pointer_or_aggr(r,_tmpA80)) && Cyc_Tcutil_is_noalias_path_aux(
r,_tmpA68,0);_npop_handler(0);return _tmpA81;};_pop_handler();}else{void*_tmpA7E=(
void*)_exn_thrown;void*_tmpA83=_tmpA7E;_LL6D9: if(_tmpA83 != Cyc_List_Nth)goto
_LL6DB;_LL6DA: {const char*_tmp106F;void*_tmp106E;return(_tmp106E=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp106F="is_noalias_path_aux: out-of-bounds subscript",
_tag_dyneither(_tmp106F,sizeof(char),45))),_tag_dyneither(_tmp106E,sizeof(void*),
0)));}_LL6DB:;_LL6DC:(void)_throw(_tmpA83);_LL6D8:;}}}}_LL6D6:;_LL6D7: {const
char*_tmp1072;void*_tmp1071;(_tmp1071=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1072="is_noalias_path_aux: subscript on non-pointer/tuple",
_tag_dyneither(_tmp1072,sizeof(char),52))),_tag_dyneither(_tmp1071,sizeof(void*),
0)));}_LL6D1:;}_LL6AF: if(*((int*)_tmpA64)!= 32)goto _LL6B1;_LL6B0: goto _LL6B2;
_LL6B1: if(*((int*)_tmpA64)!= 26)goto _LL6B3;_LL6B2: goto _LL6B4;_LL6B3: if(*((int*)
_tmpA64)!= 30)goto _LL6B5;_LL6B4: goto _LL6B6;_LL6B5: if(*((int*)_tmpA64)!= 31)goto
_LL6B7;_LL6B6: goto _LL6B8;_LL6B7: if(*((int*)_tmpA64)!= 0)goto _LL6B9;_LL6B8: goto
_LL6BA;_LL6B9: if(*((int*)_tmpA64)!= 35)goto _LL6BB;_LL6BA: goto _LL6BC;_LL6BB: if(*((
int*)_tmpA64)!= 17)goto _LL6BD;_LL6BC: goto _LL6BE;_LL6BD: if(*((int*)_tmpA64)!= 1)
goto _LL6BF;_tmpA6A=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmpA64)->f2;if(
_tmpA6A <= (void*)1)goto _LL6BF;if(*((int*)_tmpA6A)!= 3)goto _LL6BF;_LL6BE: goto
_LL6C0;_LL6BF: if(*((int*)_tmpA64)!= 1)goto _LL6C1;_tmpA6B=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmpA64)->f2;if(_tmpA6B <= (void*)1)goto _LL6C1;if(*((int*)_tmpA6B)!= 4)goto _LL6C1;
_LL6C0: goto _LL6C2;_LL6C1: if(*((int*)_tmpA64)!= 1)goto _LL6C3;_tmpA6C=(void*)((
struct Cyc_Absyn_Var_e_struct*)_tmpA64)->f2;if(_tmpA6C <= (void*)1)goto _LL6C3;if(*((
int*)_tmpA6C)!= 2)goto _LL6C3;_LL6C2: {int _tmpA88=ignore_leaf  || Cyc_Tcutil_is_noalias_pointer_or_aggr(
r,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);return _tmpA88;}_LL6C3:
if(*((int*)_tmpA64)!= 6)goto _LL6C5;_tmpA6D=((struct Cyc_Absyn_Conditional_e_struct*)
_tmpA64)->f2;_LL6C4: _tmpA6E=_tmpA6D;goto _LL6C6;_LL6C5: if(*((int*)_tmpA64)!= 9)
goto _LL6C7;_tmpA6E=((struct Cyc_Absyn_SeqExp_e_struct*)_tmpA64)->f2;_LL6C6:
_tmpA6F=_tmpA6E;goto _LL6C8;_LL6C7: if(*((int*)_tmpA64)!= 4)goto _LL6C9;_tmpA6F=((
struct Cyc_Absyn_AssignOp_e_struct*)_tmpA64)->f1;_LL6C8: return Cyc_Tcutil_is_noalias_path_aux(
r,_tmpA6F,ignore_leaf);_LL6C9: if(*((int*)_tmpA64)!= 11)goto _LL6CB;_tmpA70=((
struct Cyc_Absyn_FnCall_e_struct*)_tmpA64)->f1;_LL6CA: {void*_tmpA89=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpA70->topt))->v);struct Cyc_Absyn_FnInfo
_tmpA8A;void*_tmpA8B;struct Cyc_Absyn_PtrInfo _tmpA8C;void*_tmpA8D;_LL6DE: if(
_tmpA89 <= (void*)4)goto _LL6E2;if(*((int*)_tmpA89)!= 8)goto _LL6E0;_tmpA8A=((
struct Cyc_Absyn_FnType_struct*)_tmpA89)->f1;_tmpA8B=(void*)_tmpA8A.ret_typ;
_LL6DF: return ignore_leaf  || Cyc_Tcutil_is_noalias_pointer_or_aggr(r,_tmpA8B);
_LL6E0: if(*((int*)_tmpA89)!= 4)goto _LL6E2;_tmpA8C=((struct Cyc_Absyn_PointerType_struct*)
_tmpA89)->f1;_tmpA8D=(void*)_tmpA8C.elt_typ;_LL6E1:{void*_tmpA8E=Cyc_Tcutil_compress(
_tmpA8D);struct Cyc_Absyn_FnInfo _tmpA8F;void*_tmpA90;_LL6E5: if(_tmpA8E <= (void*)4)
goto _LL6E7;if(*((int*)_tmpA8E)!= 8)goto _LL6E7;_tmpA8F=((struct Cyc_Absyn_FnType_struct*)
_tmpA8E)->f1;_tmpA90=(void*)_tmpA8F.ret_typ;_LL6E6: return ignore_leaf  || Cyc_Tcutil_is_noalias_pointer_or_aggr(
r,_tmpA90);_LL6E7:;_LL6E8: goto _LL6E4;_LL6E4:;}goto _LL6E3;_LL6E2:;_LL6E3: {const
char*_tmp1076;void*_tmp1075[1];struct Cyc_String_pa_struct _tmp1074;(_tmp1074.tag=
0,((_tmp1074.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpA70->topt))->v)),((_tmp1075[0]=&
_tmp1074,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp1076="Fncall has non function type %s",_tag_dyneither(_tmp1076,sizeof(char),
32))),_tag_dyneither(_tmp1075,sizeof(void*),1)))))));}_LL6DD:;}_LL6CB: if(*((int*)
_tmpA64)!= 15)goto _LL6CD;_tmpA71=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmpA64)->f1;
_tmpA72=((struct Cyc_Absyn_Cast_e_struct*)_tmpA64)->f2;_LL6CC: return Cyc_Tcutil_is_noalias_pointer_or_aggr(
r,_tmpA71) && Cyc_Tcutil_is_noalias_path_aux(r,_tmpA72,1);_LL6CD: if(*((int*)
_tmpA64)!= 38)goto _LL6CF;_tmpA73=((struct Cyc_Absyn_StmtExp_e_struct*)_tmpA64)->f1;
_LL6CE: while(1){void*_tmpA94=(void*)_tmpA73->r;struct Cyc_Absyn_Stmt*_tmpA95;
struct Cyc_Absyn_Stmt*_tmpA96;struct Cyc_Absyn_Decl*_tmpA97;struct Cyc_Absyn_Stmt*
_tmpA98;struct Cyc_Absyn_Exp*_tmpA99;_LL6EA: if(_tmpA94 <= (void*)1)goto _LL6F0;if(*((
int*)_tmpA94)!= 1)goto _LL6EC;_tmpA95=((struct Cyc_Absyn_Seq_s_struct*)_tmpA94)->f1;
_tmpA96=((struct Cyc_Absyn_Seq_s_struct*)_tmpA94)->f2;_LL6EB: _tmpA73=_tmpA96;goto
_LL6E9;_LL6EC: if(*((int*)_tmpA94)!= 11)goto _LL6EE;_tmpA97=((struct Cyc_Absyn_Decl_s_struct*)
_tmpA94)->f1;_tmpA98=((struct Cyc_Absyn_Decl_s_struct*)_tmpA94)->f2;_LL6ED:
_tmpA73=_tmpA98;goto _LL6E9;_LL6EE: if(*((int*)_tmpA94)!= 0)goto _LL6F0;_tmpA99=((
struct Cyc_Absyn_Exp_s_struct*)_tmpA94)->f1;_LL6EF: return Cyc_Tcutil_is_noalias_path_aux(
r,_tmpA99,ignore_leaf);_LL6F0:;_LL6F1: {const char*_tmp1079;void*_tmp1078;(
_tmp1078=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp1079="is_noalias_stmt_exp: ill-formed StmtExp",_tag_dyneither(_tmp1079,
sizeof(char),40))),_tag_dyneither(_tmp1078,sizeof(void*),0)));}_LL6E9:;}_LL6CF:;
_LL6D0: return 0;_LL684:;}int Cyc_Tcutil_is_noalias_path(struct _RegionHandle*r,
struct Cyc_Absyn_Exp*e);int Cyc_Tcutil_is_noalias_path(struct _RegionHandle*r,
struct Cyc_Absyn_Exp*e){return Cyc_Tcutil_is_noalias_path_aux(r,e,0);}struct
_tuple10 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e);
struct _tuple10 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*
e){struct _tuple10 _tmp107A;struct _tuple10 bogus_ans=(_tmp107A.f1=0,((_tmp107A.f2=(
void*)2,_tmp107A)));void*_tmpA9C=(void*)e->r;struct _tuple2*_tmpA9D;void*_tmpA9E;
struct Cyc_Absyn_Exp*_tmpA9F;struct _dyneither_ptr*_tmpAA0;struct Cyc_Absyn_Exp*
_tmpAA1;struct _dyneither_ptr*_tmpAA2;struct Cyc_Absyn_Exp*_tmpAA3;struct Cyc_Absyn_Exp*
_tmpAA4;struct Cyc_Absyn_Exp*_tmpAA5;_LL6F3: if(*((int*)_tmpA9C)!= 1)goto _LL6F5;
_tmpA9D=((struct Cyc_Absyn_Var_e_struct*)_tmpA9C)->f1;_tmpA9E=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmpA9C)->f2;_LL6F4: {void*_tmpAA6=_tmpA9E;struct Cyc_Absyn_Vardecl*_tmpAA7;
struct Cyc_Absyn_Vardecl*_tmpAA8;struct Cyc_Absyn_Vardecl*_tmpAA9;struct Cyc_Absyn_Vardecl*
_tmpAAA;_LL700: if((int)_tmpAA6 != 0)goto _LL702;_LL701: goto _LL703;_LL702: if(
_tmpAA6 <= (void*)1)goto _LL704;if(*((int*)_tmpAA6)!= 1)goto _LL704;_LL703: return
bogus_ans;_LL704: if(_tmpAA6 <= (void*)1)goto _LL706;if(*((int*)_tmpAA6)!= 0)goto
_LL706;_tmpAA7=((struct Cyc_Absyn_Global_b_struct*)_tmpAA6)->f1;_LL705: {void*
_tmpAAB=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
_LL70D: if(_tmpAAB <= (void*)4)goto _LL70F;if(*((int*)_tmpAAB)!= 7)goto _LL70F;
_LL70E: {struct _tuple10 _tmp107B;return(_tmp107B.f1=1,((_tmp107B.f2=(void*)2,
_tmp107B)));}_LL70F:;_LL710: {struct _tuple10 _tmp107C;return(_tmp107C.f1=(_tmpAA7->tq).real_const,((
_tmp107C.f2=(void*)2,_tmp107C)));}_LL70C:;}_LL706: if(_tmpAA6 <= (void*)1)goto
_LL708;if(*((int*)_tmpAA6)!= 3)goto _LL708;_tmpAA8=((struct Cyc_Absyn_Local_b_struct*)
_tmpAA6)->f1;_LL707: {void*_tmpAAE=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);_LL712: if(_tmpAAE <= (void*)4)goto _LL714;if(*((int*)
_tmpAAE)!= 7)goto _LL714;_LL713: {struct _tuple10 _tmp107D;return(_tmp107D.f1=1,((
_tmp107D.f2=(void*)((struct Cyc_Core_Opt*)_check_null(_tmpAA8->rgn))->v,_tmp107D)));}
_LL714:;_LL715: _tmpAA8->escapes=1;{struct _tuple10 _tmp107E;return(_tmp107E.f1=(
_tmpAA8->tq).real_const,((_tmp107E.f2=(void*)((struct Cyc_Core_Opt*)_check_null(
_tmpAA8->rgn))->v,_tmp107E)));}_LL711:;}_LL708: if(_tmpAA6 <= (void*)1)goto _LL70A;
if(*((int*)_tmpAA6)!= 4)goto _LL70A;_tmpAA9=((struct Cyc_Absyn_Pat_b_struct*)
_tmpAA6)->f1;_LL709: _tmpAAA=_tmpAA9;goto _LL70B;_LL70A: if(_tmpAA6 <= (void*)1)goto
_LL6FF;if(*((int*)_tmpAA6)!= 2)goto _LL6FF;_tmpAAA=((struct Cyc_Absyn_Param_b_struct*)
_tmpAA6)->f1;_LL70B: _tmpAAA->escapes=1;{struct _tuple10 _tmp107F;return(_tmp107F.f1=(
_tmpAAA->tq).real_const,((_tmp107F.f2=(void*)((struct Cyc_Core_Opt*)_check_null(
_tmpAAA->rgn))->v,_tmp107F)));}_LL6FF:;}_LL6F5: if(*((int*)_tmpA9C)!= 23)goto
_LL6F7;_tmpA9F=((struct Cyc_Absyn_AggrMember_e_struct*)_tmpA9C)->f1;_tmpAA0=((
struct Cyc_Absyn_AggrMember_e_struct*)_tmpA9C)->f2;_LL6F6: {void*_tmpAB2=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpA9F->topt))->v);struct Cyc_List_List*
_tmpAB3;struct Cyc_Absyn_AggrInfo _tmpAB4;union Cyc_Absyn_AggrInfoU_union _tmpAB5;
struct Cyc_Absyn_Aggrdecl**_tmpAB6;struct Cyc_Absyn_Aggrdecl*_tmpAB7;_LL717: if(
_tmpAB2 <= (void*)4)goto _LL71B;if(*((int*)_tmpAB2)!= 11)goto _LL719;_tmpAB3=((
struct Cyc_Absyn_AnonAggrType_struct*)_tmpAB2)->f2;_LL718: {struct Cyc_Absyn_Aggrfield*
_tmpAB8=Cyc_Absyn_lookup_field(_tmpAB3,_tmpAA0);if(_tmpAB8 != 0  && _tmpAB8->width
!= 0){struct _tuple10 _tmp1080;return(_tmp1080.f1=(_tmpAB8->tq).real_const,((
_tmp1080.f2=(Cyc_Tcutil_addressof_props(te,_tmpA9F)).f2,_tmp1080)));}return
bogus_ans;}_LL719: if(*((int*)_tmpAB2)!= 10)goto _LL71B;_tmpAB4=((struct Cyc_Absyn_AggrType_struct*)
_tmpAB2)->f1;_tmpAB5=_tmpAB4.aggr_info;if((((((struct Cyc_Absyn_AggrType_struct*)
_tmpAB2)->f1).aggr_info).KnownAggr).tag != 1)goto _LL71B;_tmpAB6=(_tmpAB5.KnownAggr).f1;
_tmpAB7=*_tmpAB6;_LL71A: {struct Cyc_Absyn_Aggrfield*_tmpABA=Cyc_Absyn_lookup_decl_field(
_tmpAB7,_tmpAA0);if(_tmpABA != 0  && _tmpABA->width != 0){struct _tuple10 _tmp1081;
return(_tmp1081.f1=(_tmpABA->tq).real_const,((_tmp1081.f2=(Cyc_Tcutil_addressof_props(
te,_tmpA9F)).f2,_tmp1081)));}return bogus_ans;}_LL71B:;_LL71C: return bogus_ans;
_LL716:;}_LL6F7: if(*((int*)_tmpA9C)!= 24)goto _LL6F9;_tmpAA1=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmpA9C)->f1;_tmpAA2=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmpA9C)->f2;_LL6F8: {
void*_tmpABC=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmpAA1->topt))->v);struct Cyc_Absyn_PtrInfo _tmpABD;void*_tmpABE;struct Cyc_Absyn_PtrAtts
_tmpABF;void*_tmpAC0;_LL71E: if(_tmpABC <= (void*)4)goto _LL720;if(*((int*)_tmpABC)
!= 4)goto _LL720;_tmpABD=((struct Cyc_Absyn_PointerType_struct*)_tmpABC)->f1;
_tmpABE=(void*)_tmpABD.elt_typ;_tmpABF=_tmpABD.ptr_atts;_tmpAC0=(void*)_tmpABF.rgn;
_LL71F: {struct Cyc_Absyn_Aggrfield*finfo;{void*_tmpAC1=Cyc_Tcutil_compress(
_tmpABE);struct Cyc_List_List*_tmpAC2;struct Cyc_Absyn_AggrInfo _tmpAC3;union Cyc_Absyn_AggrInfoU_union
_tmpAC4;struct Cyc_Absyn_Aggrdecl**_tmpAC5;struct Cyc_Absyn_Aggrdecl*_tmpAC6;
_LL723: if(_tmpAC1 <= (void*)4)goto _LL727;if(*((int*)_tmpAC1)!= 11)goto _LL725;
_tmpAC2=((struct Cyc_Absyn_AnonAggrType_struct*)_tmpAC1)->f2;_LL724: finfo=Cyc_Absyn_lookup_field(
_tmpAC2,_tmpAA2);goto _LL722;_LL725: if(*((int*)_tmpAC1)!= 10)goto _LL727;_tmpAC3=((
struct Cyc_Absyn_AggrType_struct*)_tmpAC1)->f1;_tmpAC4=_tmpAC3.aggr_info;if((((((
struct Cyc_Absyn_AggrType_struct*)_tmpAC1)->f1).aggr_info).KnownAggr).tag != 1)
goto _LL727;_tmpAC5=(_tmpAC4.KnownAggr).f1;_tmpAC6=*_tmpAC5;_LL726: finfo=Cyc_Absyn_lookup_decl_field(
_tmpAC6,_tmpAA2);goto _LL722;_LL727:;_LL728: return bogus_ans;_LL722:;}if(finfo != 0
 && finfo->width != 0){struct _tuple10 _tmp1082;return(_tmp1082.f1=(finfo->tq).real_const,((
_tmp1082.f2=_tmpAC0,_tmp1082)));}return bogus_ans;}_LL720:;_LL721: return bogus_ans;
_LL71D:;}_LL6F9: if(*((int*)_tmpA9C)!= 22)goto _LL6FB;_tmpAA3=((struct Cyc_Absyn_Deref_e_struct*)
_tmpA9C)->f1;_LL6FA: {void*_tmpAC8=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmpAA3->topt))->v);struct Cyc_Absyn_PtrInfo _tmpAC9;struct Cyc_Absyn_Tqual
_tmpACA;struct Cyc_Absyn_PtrAtts _tmpACB;void*_tmpACC;_LL72A: if(_tmpAC8 <= (void*)4)
goto _LL72C;if(*((int*)_tmpAC8)!= 4)goto _LL72C;_tmpAC9=((struct Cyc_Absyn_PointerType_struct*)
_tmpAC8)->f1;_tmpACA=_tmpAC9.elt_tq;_tmpACB=_tmpAC9.ptr_atts;_tmpACC=(void*)
_tmpACB.rgn;_LL72B: {struct _tuple10 _tmp1083;return(_tmp1083.f1=_tmpACA.real_const,((
_tmp1083.f2=_tmpACC,_tmp1083)));}_LL72C:;_LL72D: return bogus_ans;_LL729:;}_LL6FB:
if(*((int*)_tmpA9C)!= 25)goto _LL6FD;_tmpAA4=((struct Cyc_Absyn_Subscript_e_struct*)
_tmpA9C)->f1;_tmpAA5=((struct Cyc_Absyn_Subscript_e_struct*)_tmpA9C)->f2;_LL6FC: {
void*t=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmpAA4->topt))->v);
void*_tmpACE=t;struct Cyc_List_List*_tmpACF;struct Cyc_Absyn_PtrInfo _tmpAD0;struct
Cyc_Absyn_Tqual _tmpAD1;struct Cyc_Absyn_PtrAtts _tmpAD2;void*_tmpAD3;struct Cyc_Absyn_ArrayInfo
_tmpAD4;struct Cyc_Absyn_Tqual _tmpAD5;_LL72F: if(_tmpACE <= (void*)4)goto _LL735;if(*((
int*)_tmpACE)!= 9)goto _LL731;_tmpACF=((struct Cyc_Absyn_TupleType_struct*)_tmpACE)->f1;
_LL730: {unsigned int _tmpAD7;int _tmpAD8;struct _tuple7 _tmpAD6=Cyc_Evexp_eval_const_uint_exp(
_tmpAA5);_tmpAD7=_tmpAD6.f1;_tmpAD8=_tmpAD6.f2;if(!_tmpAD8)return bogus_ans;{
struct _tuple5*_tmpAD9=Cyc_Absyn_lookup_tuple_field(_tmpACF,(int)_tmpAD7);if(
_tmpAD9 != 0){struct _tuple10 _tmp1084;return(_tmp1084.f1=((*_tmpAD9).f1).real_const,((
_tmp1084.f2=(Cyc_Tcutil_addressof_props(te,_tmpAA4)).f2,_tmp1084)));}return
bogus_ans;}}_LL731: if(*((int*)_tmpACE)!= 4)goto _LL733;_tmpAD0=((struct Cyc_Absyn_PointerType_struct*)
_tmpACE)->f1;_tmpAD1=_tmpAD0.elt_tq;_tmpAD2=_tmpAD0.ptr_atts;_tmpAD3=(void*)
_tmpAD2.rgn;_LL732: {struct _tuple10 _tmp1085;return(_tmp1085.f1=_tmpAD1.real_const,((
_tmp1085.f2=_tmpAD3,_tmp1085)));}_LL733: if(*((int*)_tmpACE)!= 7)goto _LL735;
_tmpAD4=((struct Cyc_Absyn_ArrayType_struct*)_tmpACE)->f1;_tmpAD5=_tmpAD4.tq;
_LL734: {struct _tuple10 _tmp1086;return(_tmp1086.f1=_tmpAD5.real_const,((_tmp1086.f2=(
Cyc_Tcutil_addressof_props(te,_tmpAA4)).f2,_tmp1086)));}_LL735:;_LL736: return
bogus_ans;_LL72E:;}_LL6FD:;_LL6FE:{const char*_tmp1089;void*_tmp1088;(_tmp1088=0,
Cyc_Tcutil_terr(e->loc,((_tmp1089="unary & applied to non-lvalue",_tag_dyneither(
_tmp1089,sizeof(char),30))),_tag_dyneither(_tmp1088,sizeof(void*),0)));}return
bogus_ans;_LL6F2:;}void*Cyc_Tcutil_array_to_ptr(struct Cyc_Tcenv_Tenv*te,void*
e_typ,struct Cyc_Absyn_Exp*e);void*Cyc_Tcutil_array_to_ptr(struct Cyc_Tcenv_Tenv*
te,void*e_typ,struct Cyc_Absyn_Exp*e){void*_tmpAE0=Cyc_Tcutil_compress(e_typ);
struct Cyc_Absyn_ArrayInfo _tmpAE1;void*_tmpAE2;struct Cyc_Absyn_Tqual _tmpAE3;
struct Cyc_Absyn_Conref*_tmpAE4;_LL738: if(_tmpAE0 <= (void*)4)goto _LL73A;if(*((int*)
_tmpAE0)!= 7)goto _LL73A;_tmpAE1=((struct Cyc_Absyn_ArrayType_struct*)_tmpAE0)->f1;
_tmpAE2=(void*)_tmpAE1.elt_type;_tmpAE3=_tmpAE1.tq;_tmpAE4=_tmpAE1.zero_term;
_LL739: {void*_tmpAE6;struct _tuple10 _tmpAE5=Cyc_Tcutil_addressof_props(te,e);
_tmpAE6=_tmpAE5.f2;{struct Cyc_Absyn_Upper_b_struct _tmp108C;struct Cyc_Absyn_Upper_b_struct*
_tmp108B;return Cyc_Absyn_atb_typ(_tmpAE2,_tmpAE6,_tmpAE3,(void*)((_tmp108B=
_cycalloc(sizeof(*_tmp108B)),((_tmp108B[0]=((_tmp108C.tag=0,((_tmp108C.f1=e,
_tmp108C)))),_tmp108B)))),_tmpAE4);}}_LL73A:;_LL73B: return e_typ;_LL737:;}void Cyc_Tcutil_check_bound(
struct Cyc_Position_Segment*loc,unsigned int i,struct Cyc_Absyn_Conref*b);void Cyc_Tcutil_check_bound(
struct Cyc_Position_Segment*loc,unsigned int i,struct Cyc_Absyn_Conref*b){b=Cyc_Absyn_compress_conref(
b);{union Cyc_Absyn_Constraint_union _tmpAE9=b->v;void*_tmpAEA;void*_tmpAEB;struct
Cyc_Absyn_Exp*_tmpAEC;_LL73D: if((_tmpAE9.Eq_constr).tag != 0)goto _LL73F;_tmpAEA=(
_tmpAE9.Eq_constr).f1;if((int)_tmpAEA != 0)goto _LL73F;_LL73E: return;_LL73F: if((
_tmpAE9.Eq_constr).tag != 0)goto _LL741;_tmpAEB=(_tmpAE9.Eq_constr).f1;if(_tmpAEB
<= (void*)1)goto _LL741;if(*((int*)_tmpAEB)!= 0)goto _LL741;_tmpAEC=((struct Cyc_Absyn_Upper_b_struct*)
_tmpAEB)->f1;_LL740: {unsigned int _tmpAEE;int _tmpAEF;struct _tuple7 _tmpAED=Cyc_Evexp_eval_const_uint_exp(
_tmpAEC);_tmpAEE=_tmpAED.f1;_tmpAEF=_tmpAED.f2;if(_tmpAEF  && _tmpAEE <= i){const
char*_tmp1091;void*_tmp1090[2];struct Cyc_Int_pa_struct _tmp108F;struct Cyc_Int_pa_struct
_tmp108E;(_tmp108E.tag=1,((_tmp108E.f1=(unsigned long)((int)i),((_tmp108F.tag=1,((
_tmp108F.f1=(unsigned long)((int)_tmpAEE),((_tmp1090[0]=& _tmp108F,((_tmp1090[1]=&
_tmp108E,Cyc_Tcutil_terr(loc,((_tmp1091="dereference is out of bounds: %d <= %d",
_tag_dyneither(_tmp1091,sizeof(char),39))),_tag_dyneither(_tmp1090,sizeof(void*),
2)))))))))))));}return;}_LL741: if((_tmpAE9.No_constr).tag != 2)goto _LL743;_LL742:{
struct Cyc_Absyn_Upper_b_struct*_tmp1097;struct Cyc_Absyn_Upper_b_struct _tmp1096;
union Cyc_Absyn_Constraint_union _tmp1095;b->v=(union Cyc_Absyn_Constraint_union)(((
_tmp1095.Eq_constr).tag=0,(((_tmp1095.Eq_constr).f1=(void*)((void*)((void*)((
_tmp1097=_cycalloc(sizeof(*_tmp1097)),((_tmp1097[0]=((_tmp1096.tag=0,((_tmp1096.f1=
Cyc_Absyn_uint_exp(i + 1,0),_tmp1096)))),_tmp1097)))))),_tmp1095))));}return;
_LL743:;_LL744: {const char*_tmp109A;void*_tmp1099;(_tmp1099=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp109A="check_bound -- bad compressed conref",
_tag_dyneither(_tmp109A,sizeof(char),37))),_tag_dyneither(_tmp1099,sizeof(void*),
0)));}_LL73C:;}}void Cyc_Tcutil_check_nonzero_bound(struct Cyc_Position_Segment*
loc,struct Cyc_Absyn_Conref*b);void Cyc_Tcutil_check_nonzero_bound(struct Cyc_Position_Segment*
loc,struct Cyc_Absyn_Conref*b){Cyc_Tcutil_check_bound(loc,0,b);}int Cyc_Tcutil_is_bound_one(
struct Cyc_Absyn_Conref*b);int Cyc_Tcutil_is_bound_one(struct Cyc_Absyn_Conref*b){
union Cyc_Absyn_Constraint_union _tmpAF9=(Cyc_Absyn_compress_conref(b))->v;void*
_tmpAFA;struct Cyc_Absyn_Exp*_tmpAFB;_LL746: if((_tmpAF9.Eq_constr).tag != 0)goto
_LL748;_tmpAFA=(_tmpAF9.Eq_constr).f1;if(_tmpAFA <= (void*)1)goto _LL748;if(*((int*)
_tmpAFA)!= 0)goto _LL748;_tmpAFB=((struct Cyc_Absyn_Upper_b_struct*)_tmpAFA)->f1;
_LL747: {unsigned int _tmpAFD;int _tmpAFE;struct _tuple7 _tmpAFC=Cyc_Evexp_eval_const_uint_exp(
_tmpAFB);_tmpAFD=_tmpAFC.f1;_tmpAFE=_tmpAFC.f2;return _tmpAFE  && _tmpAFD == 1;}
_LL748:;_LL749: return 0;_LL745:;}int Cyc_Tcutil_bits_only(void*t);int Cyc_Tcutil_bits_only(
void*t){void*_tmpAFF=Cyc_Tcutil_compress(t);struct Cyc_Absyn_ArrayInfo _tmpB00;
void*_tmpB01;struct Cyc_Absyn_Conref*_tmpB02;struct Cyc_List_List*_tmpB03;struct
Cyc_Absyn_AggrInfo _tmpB04;union Cyc_Absyn_AggrInfoU_union _tmpB05;struct Cyc_Absyn_AggrInfo
_tmpB06;union Cyc_Absyn_AggrInfoU_union _tmpB07;struct Cyc_Absyn_Aggrdecl**_tmpB08;
struct Cyc_Absyn_Aggrdecl*_tmpB09;struct Cyc_List_List*_tmpB0A;struct Cyc_List_List*
_tmpB0B;_LL74B: if((int)_tmpAFF != 0)goto _LL74D;_LL74C: goto _LL74E;_LL74D: if(
_tmpAFF <= (void*)4)goto _LL74F;if(*((int*)_tmpAFF)!= 5)goto _LL74F;_LL74E: goto
_LL750;_LL74F: if((int)_tmpAFF != 1)goto _LL751;_LL750: goto _LL752;_LL751: if(_tmpAFF
<= (void*)4)goto _LL761;if(*((int*)_tmpAFF)!= 6)goto _LL753;_LL752: return 1;_LL753:
if(*((int*)_tmpAFF)!= 12)goto _LL755;_LL754: goto _LL756;_LL755: if(*((int*)_tmpAFF)
!= 13)goto _LL757;_LL756: return 0;_LL757: if(*((int*)_tmpAFF)!= 7)goto _LL759;
_tmpB00=((struct Cyc_Absyn_ArrayType_struct*)_tmpAFF)->f1;_tmpB01=(void*)_tmpB00.elt_type;
_tmpB02=_tmpB00.zero_term;_LL758: return !((int(*)(int,struct Cyc_Absyn_Conref*x))
Cyc_Absyn_conref_def)(0,_tmpB02) && Cyc_Tcutil_bits_only(_tmpB01);_LL759: if(*((
int*)_tmpAFF)!= 9)goto _LL75B;_tmpB03=((struct Cyc_Absyn_TupleType_struct*)_tmpAFF)->f1;
_LL75A: for(0;_tmpB03 != 0;_tmpB03=_tmpB03->tl){if(!Cyc_Tcutil_bits_only((*((
struct _tuple5*)_tmpB03->hd)).f2))return 0;}return 1;_LL75B: if(*((int*)_tmpAFF)!= 
10)goto _LL75D;_tmpB04=((struct Cyc_Absyn_AggrType_struct*)_tmpAFF)->f1;_tmpB05=
_tmpB04.aggr_info;if((((((struct Cyc_Absyn_AggrType_struct*)_tmpAFF)->f1).aggr_info).UnknownAggr).tag
!= 0)goto _LL75D;_LL75C: return 0;_LL75D: if(*((int*)_tmpAFF)!= 10)goto _LL75F;
_tmpB06=((struct Cyc_Absyn_AggrType_struct*)_tmpAFF)->f1;_tmpB07=_tmpB06.aggr_info;
if((((((struct Cyc_Absyn_AggrType_struct*)_tmpAFF)->f1).aggr_info).KnownAggr).tag
!= 1)goto _LL75F;_tmpB08=(_tmpB07.KnownAggr).f1;_tmpB09=*_tmpB08;_tmpB0A=_tmpB06.targs;
_LL75E: if(_tmpB09->impl == 0)return 0;{struct _RegionHandle _tmpB0C=_new_region("rgn");
struct _RegionHandle*rgn=& _tmpB0C;_push_region(rgn);{struct Cyc_List_List*_tmpB0D=((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmpB09->tvs,_tmpB0A);{struct Cyc_List_List*
fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpB09->impl))->fields;for(0;fs
!= 0;fs=fs->tl){if(!Cyc_Tcutil_bits_only(Cyc_Tcutil_rsubstitute(rgn,_tmpB0D,(
void*)((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))){int _tmpB0E=0;_npop_handler(0);
return _tmpB0E;}}}{int _tmpB0F=1;_npop_handler(0);return _tmpB0F;}};_pop_region(rgn);}
_LL75F: if(*((int*)_tmpAFF)!= 11)goto _LL761;_tmpB0B=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmpAFF)->f2;_LL760: for(0;_tmpB0B != 0;_tmpB0B=_tmpB0B->tl){if(!Cyc_Tcutil_bits_only((
void*)((struct Cyc_Absyn_Aggrfield*)_tmpB0B->hd)->type))return 0;}return 1;_LL761:;
_LL762: return 0;_LL74A:;}struct _tuple23{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*
f2;};static int Cyc_Tcutil_cnst_exp(struct Cyc_Tcenv_Tenv*te,int var_okay,struct Cyc_Absyn_Exp*
e);static int Cyc_Tcutil_cnst_exp(struct Cyc_Tcenv_Tenv*te,int var_okay,struct Cyc_Absyn_Exp*
e){void*_tmpB10=(void*)e->r;struct _tuple2*_tmpB11;void*_tmpB12;struct Cyc_Absyn_Exp*
_tmpB13;struct Cyc_Absyn_Exp*_tmpB14;struct Cyc_Absyn_Exp*_tmpB15;struct Cyc_Absyn_Exp*
_tmpB16;struct Cyc_Absyn_Exp*_tmpB17;struct Cyc_Absyn_Exp*_tmpB18;struct Cyc_Absyn_Exp*
_tmpB19;void*_tmpB1A;struct Cyc_Absyn_Exp*_tmpB1B;void*_tmpB1C;void*_tmpB1D;
struct Cyc_Absyn_Exp*_tmpB1E;struct Cyc_Absyn_Exp*_tmpB1F;struct Cyc_Absyn_Exp*
_tmpB20;struct Cyc_Absyn_Exp*_tmpB21;struct Cyc_List_List*_tmpB22;struct Cyc_List_List*
_tmpB23;struct Cyc_List_List*_tmpB24;void*_tmpB25;struct Cyc_List_List*_tmpB26;
struct Cyc_List_List*_tmpB27;struct Cyc_List_List*_tmpB28;_LL764: if(*((int*)
_tmpB10)!= 0)goto _LL766;_LL765: goto _LL767;_LL766: if(*((int*)_tmpB10)!= 18)goto
_LL768;_LL767: goto _LL769;_LL768: if(*((int*)_tmpB10)!= 19)goto _LL76A;_LL769: goto
_LL76B;_LL76A: if(*((int*)_tmpB10)!= 20)goto _LL76C;_LL76B: goto _LL76D;_LL76C: if(*((
int*)_tmpB10)!= 21)goto _LL76E;_LL76D: goto _LL76F;_LL76E: if(*((int*)_tmpB10)!= 33)
goto _LL770;_LL76F: goto _LL771;_LL770: if(*((int*)_tmpB10)!= 34)goto _LL772;_LL771:
return 1;_LL772: if(*((int*)_tmpB10)!= 1)goto _LL774;_tmpB11=((struct Cyc_Absyn_Var_e_struct*)
_tmpB10)->f1;_tmpB12=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmpB10)->f2;_LL773: {
void*_tmpB29=_tmpB12;struct Cyc_Absyn_Vardecl*_tmpB2A;_LL793: if(_tmpB29 <= (void*)
1)goto _LL797;if(*((int*)_tmpB29)!= 1)goto _LL795;_LL794: return 1;_LL795: if(*((int*)
_tmpB29)!= 0)goto _LL797;_tmpB2A=((struct Cyc_Absyn_Global_b_struct*)_tmpB29)->f1;
_LL796: {void*_tmpB2B=Cyc_Tcutil_compress((void*)_tmpB2A->type);_LL79C: if(
_tmpB2B <= (void*)4)goto _LL7A0;if(*((int*)_tmpB2B)!= 7)goto _LL79E;_LL79D: goto
_LL79F;_LL79E: if(*((int*)_tmpB2B)!= 8)goto _LL7A0;_LL79F: return 1;_LL7A0:;_LL7A1:
return var_okay;_LL79B:;}_LL797: if((int)_tmpB29 != 0)goto _LL799;_LL798: return 0;
_LL799:;_LL79A: return var_okay;_LL792:;}_LL774: if(*((int*)_tmpB10)!= 6)goto _LL776;
_tmpB13=((struct Cyc_Absyn_Conditional_e_struct*)_tmpB10)->f1;_tmpB14=((struct Cyc_Absyn_Conditional_e_struct*)
_tmpB10)->f2;_tmpB15=((struct Cyc_Absyn_Conditional_e_struct*)_tmpB10)->f3;_LL775:
return(Cyc_Tcutil_cnst_exp(te,0,_tmpB13) && Cyc_Tcutil_cnst_exp(te,0,_tmpB14))
 && Cyc_Tcutil_cnst_exp(te,0,_tmpB15);_LL776: if(*((int*)_tmpB10)!= 9)goto _LL778;
_tmpB16=((struct Cyc_Absyn_SeqExp_e_struct*)_tmpB10)->f1;_tmpB17=((struct Cyc_Absyn_SeqExp_e_struct*)
_tmpB10)->f2;_LL777: return Cyc_Tcutil_cnst_exp(te,0,_tmpB16) && Cyc_Tcutil_cnst_exp(
te,0,_tmpB17);_LL778: if(*((int*)_tmpB10)!= 13)goto _LL77A;_tmpB18=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmpB10)->f1;_LL779: _tmpB19=_tmpB18;goto _LL77B;_LL77A: if(*((int*)_tmpB10)!= 14)
goto _LL77C;_tmpB19=((struct Cyc_Absyn_Instantiate_e_struct*)_tmpB10)->f1;_LL77B:
return Cyc_Tcutil_cnst_exp(te,var_okay,_tmpB19);_LL77C: if(*((int*)_tmpB10)!= 15)
goto _LL77E;_tmpB1A=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmpB10)->f1;_tmpB1B=((
struct Cyc_Absyn_Cast_e_struct*)_tmpB10)->f2;_tmpB1C=(void*)((struct Cyc_Absyn_Cast_e_struct*)
_tmpB10)->f4;if((int)_tmpB1C != 1)goto _LL77E;_LL77D: return Cyc_Tcutil_cnst_exp(te,
var_okay,_tmpB1B);_LL77E: if(*((int*)_tmpB10)!= 15)goto _LL780;_tmpB1D=(void*)((
struct Cyc_Absyn_Cast_e_struct*)_tmpB10)->f1;_tmpB1E=((struct Cyc_Absyn_Cast_e_struct*)
_tmpB10)->f2;_LL77F: return Cyc_Tcutil_cnst_exp(te,var_okay,_tmpB1E);_LL780: if(*((
int*)_tmpB10)!= 16)goto _LL782;_tmpB1F=((struct Cyc_Absyn_Address_e_struct*)
_tmpB10)->f1;_LL781: return Cyc_Tcutil_cnst_exp(te,1,_tmpB1F);_LL782: if(*((int*)
_tmpB10)!= 29)goto _LL784;_tmpB20=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmpB10)->f2;_tmpB21=((struct Cyc_Absyn_Comprehension_e_struct*)_tmpB10)->f3;
_LL783: return Cyc_Tcutil_cnst_exp(te,0,_tmpB20) && Cyc_Tcutil_cnst_exp(te,0,
_tmpB21);_LL784: if(*((int*)_tmpB10)!= 28)goto _LL786;_tmpB22=((struct Cyc_Absyn_Array_e_struct*)
_tmpB10)->f1;_LL785: _tmpB23=_tmpB22;goto _LL787;_LL786: if(*((int*)_tmpB10)!= 31)
goto _LL788;_tmpB23=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmpB10)->f2;_LL787:
_tmpB24=_tmpB23;goto _LL789;_LL788: if(*((int*)_tmpB10)!= 30)goto _LL78A;_tmpB24=((
struct Cyc_Absyn_Struct_e_struct*)_tmpB10)->f3;_LL789: for(0;_tmpB24 != 0;_tmpB24=
_tmpB24->tl){if(!Cyc_Tcutil_cnst_exp(te,0,(*((struct _tuple23*)_tmpB24->hd)).f2))
return 0;}return 1;_LL78A: if(*((int*)_tmpB10)!= 3)goto _LL78C;_tmpB25=(void*)((
struct Cyc_Absyn_Primop_e_struct*)_tmpB10)->f1;_tmpB26=((struct Cyc_Absyn_Primop_e_struct*)
_tmpB10)->f2;_LL78B: _tmpB27=_tmpB26;goto _LL78D;_LL78C: if(*((int*)_tmpB10)!= 26)
goto _LL78E;_tmpB27=((struct Cyc_Absyn_Tuple_e_struct*)_tmpB10)->f1;_LL78D: _tmpB28=
_tmpB27;goto _LL78F;_LL78E: if(*((int*)_tmpB10)!= 32)goto _LL790;_tmpB28=((struct
Cyc_Absyn_Datatype_e_struct*)_tmpB10)->f1;_LL78F: for(0;_tmpB28 != 0;_tmpB28=
_tmpB28->tl){if(!Cyc_Tcutil_cnst_exp(te,0,(struct Cyc_Absyn_Exp*)_tmpB28->hd))
return 0;}return 1;_LL790:;_LL791: return 0;_LL763:;}int Cyc_Tcutil_is_const_exp(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e);int Cyc_Tcutil_is_const_exp(struct
Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){return Cyc_Tcutil_cnst_exp(te,0,e);}
static int Cyc_Tcutil_fields_support_default(struct Cyc_List_List*tvs,struct Cyc_List_List*
ts,struct Cyc_List_List*fs);int Cyc_Tcutil_supports_default(void*t);int Cyc_Tcutil_supports_default(
void*t){void*_tmpB2C=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmpB2D;
struct Cyc_Absyn_PtrAtts _tmpB2E;struct Cyc_Absyn_Conref*_tmpB2F;struct Cyc_Absyn_Conref*
_tmpB30;struct Cyc_Absyn_ArrayInfo _tmpB31;void*_tmpB32;struct Cyc_List_List*
_tmpB33;struct Cyc_Absyn_AggrInfo _tmpB34;union Cyc_Absyn_AggrInfoU_union _tmpB35;
struct Cyc_List_List*_tmpB36;struct Cyc_List_List*_tmpB37;_LL7A3: if((int)_tmpB2C != 
0)goto _LL7A5;_LL7A4: goto _LL7A6;_LL7A5: if(_tmpB2C <= (void*)4)goto _LL7A7;if(*((int*)
_tmpB2C)!= 5)goto _LL7A7;_LL7A6: goto _LL7A8;_LL7A7: if((int)_tmpB2C != 1)goto _LL7A9;
_LL7A8: goto _LL7AA;_LL7A9: if(_tmpB2C <= (void*)4)goto _LL7B9;if(*((int*)_tmpB2C)!= 
6)goto _LL7AB;_LL7AA: return 1;_LL7AB: if(*((int*)_tmpB2C)!= 4)goto _LL7AD;_tmpB2D=((
struct Cyc_Absyn_PointerType_struct*)_tmpB2C)->f1;_tmpB2E=_tmpB2D.ptr_atts;
_tmpB2F=_tmpB2E.nullable;_tmpB30=_tmpB2E.bounds;_LL7AC: {void*_tmpB38=Cyc_Absyn_conref_def((
void*)((void*)0),_tmpB30);_LL7BC: if((int)_tmpB38 != 0)goto _LL7BE;_LL7BD: return 1;
_LL7BE:;_LL7BF: return((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
1,_tmpB2F);_LL7BB:;}_LL7AD: if(*((int*)_tmpB2C)!= 7)goto _LL7AF;_tmpB31=((struct
Cyc_Absyn_ArrayType_struct*)_tmpB2C)->f1;_tmpB32=(void*)_tmpB31.elt_type;_LL7AE:
return Cyc_Tcutil_supports_default(_tmpB32);_LL7AF: if(*((int*)_tmpB2C)!= 9)goto
_LL7B1;_tmpB33=((struct Cyc_Absyn_TupleType_struct*)_tmpB2C)->f1;_LL7B0: for(0;
_tmpB33 != 0;_tmpB33=_tmpB33->tl){if(!Cyc_Tcutil_supports_default((*((struct
_tuple5*)_tmpB33->hd)).f2))return 0;}return 1;_LL7B1: if(*((int*)_tmpB2C)!= 10)goto
_LL7B3;_tmpB34=((struct Cyc_Absyn_AggrType_struct*)_tmpB2C)->f1;_tmpB35=_tmpB34.aggr_info;
_tmpB36=_tmpB34.targs;_LL7B2: {struct Cyc_Absyn_Aggrdecl*_tmpB39=Cyc_Absyn_get_known_aggrdecl(
_tmpB35);if(_tmpB39->impl == 0)return 0;if(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmpB39->impl))->exist_vars != 0)return 0;return Cyc_Tcutil_fields_support_default(
_tmpB39->tvs,_tmpB36,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpB39->impl))->fields);}
_LL7B3: if(*((int*)_tmpB2C)!= 11)goto _LL7B5;_tmpB37=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmpB2C)->f2;_LL7B4: return Cyc_Tcutil_fields_support_default(0,0,_tmpB37);_LL7B5:
if(*((int*)_tmpB2C)!= 13)goto _LL7B7;_LL7B6: goto _LL7B8;_LL7B7: if(*((int*)_tmpB2C)
!= 12)goto _LL7B9;_LL7B8: return 1;_LL7B9:;_LL7BA: return 0;_LL7A2:;}static int Cyc_Tcutil_fields_support_default(
struct Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs);static int
Cyc_Tcutil_fields_support_default(struct Cyc_List_List*tvs,struct Cyc_List_List*ts,
struct Cyc_List_List*fs){struct _RegionHandle _tmpB3A=_new_region("rgn");struct
_RegionHandle*rgn=& _tmpB3A;_push_region(rgn);{struct Cyc_List_List*_tmpB3B=((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,tvs,ts);for(0;fs != 0;fs=fs->tl){
void*t=Cyc_Tcutil_rsubstitute(rgn,_tmpB3B,(void*)((struct Cyc_Absyn_Aggrfield*)fs->hd)->type);
if(!Cyc_Tcutil_supports_default(t)){int _tmpB3C=0;_npop_handler(0);return _tmpB3C;}}}{
int _tmpB3D=1;_npop_handler(0);return _tmpB3D;};_pop_region(rgn);}int Cyc_Tcutil_admits_zero(
void*t);int Cyc_Tcutil_admits_zero(void*t){void*_tmpB3E=Cyc_Tcutil_compress(t);
struct Cyc_Absyn_PtrInfo _tmpB3F;struct Cyc_Absyn_PtrAtts _tmpB40;struct Cyc_Absyn_Conref*
_tmpB41;struct Cyc_Absyn_Conref*_tmpB42;_LL7C1: if(_tmpB3E <= (void*)4)goto _LL7C3;
if(*((int*)_tmpB3E)!= 5)goto _LL7C3;_LL7C2: goto _LL7C4;_LL7C3: if((int)_tmpB3E != 1)
goto _LL7C5;_LL7C4: goto _LL7C6;_LL7C5: if(_tmpB3E <= (void*)4)goto _LL7C9;if(*((int*)
_tmpB3E)!= 6)goto _LL7C7;_LL7C6: return 1;_LL7C7: if(*((int*)_tmpB3E)!= 4)goto _LL7C9;
_tmpB3F=((struct Cyc_Absyn_PointerType_struct*)_tmpB3E)->f1;_tmpB40=_tmpB3F.ptr_atts;
_tmpB41=_tmpB40.nullable;_tmpB42=_tmpB40.bounds;_LL7C8: {union Cyc_Absyn_Constraint_union
_tmpB43=(Cyc_Absyn_compress_conref(_tmpB42))->v;void*_tmpB44;_LL7CC: if((_tmpB43.Eq_constr).tag
!= 0)goto _LL7CE;_tmpB44=(_tmpB43.Eq_constr).f1;if((int)_tmpB44 != 0)goto _LL7CE;
_LL7CD: return 0;_LL7CE: if((_tmpB43.Eq_constr).tag != 0)goto _LL7D0;_LL7CF: {union
Cyc_Absyn_Constraint_union _tmpB45=(((struct Cyc_Absyn_Conref*(*)(struct Cyc_Absyn_Conref*
x))Cyc_Absyn_compress_conref)(_tmpB41))->v;int _tmpB46;_LL7D3: if((_tmpB45.Eq_constr).tag
!= 0)goto _LL7D5;_tmpB46=(_tmpB45.Eq_constr).f1;_LL7D4: return _tmpB46;_LL7D5:;
_LL7D6: return 0;_LL7D2:;}_LL7D0:;_LL7D1: return 0;_LL7CB:;}_LL7C9:;_LL7CA: return 0;
_LL7C0:;}int Cyc_Tcutil_is_noreturn(void*t);int Cyc_Tcutil_is_noreturn(void*t){{
void*_tmpB47=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmpB48;void*_tmpB49;
struct Cyc_Absyn_FnInfo _tmpB4A;struct Cyc_List_List*_tmpB4B;_LL7D8: if(_tmpB47 <= (
void*)4)goto _LL7DC;if(*((int*)_tmpB47)!= 4)goto _LL7DA;_tmpB48=((struct Cyc_Absyn_PointerType_struct*)
_tmpB47)->f1;_tmpB49=(void*)_tmpB48.elt_typ;_LL7D9: return Cyc_Tcutil_is_noreturn(
_tmpB49);_LL7DA: if(*((int*)_tmpB47)!= 8)goto _LL7DC;_tmpB4A=((struct Cyc_Absyn_FnType_struct*)
_tmpB47)->f1;_tmpB4B=_tmpB4A.attributes;_LL7DB: for(0;_tmpB4B != 0;_tmpB4B=_tmpB4B->tl){
void*_tmpB4C=(void*)_tmpB4B->hd;_LL7DF: if((int)_tmpB4C != 3)goto _LL7E1;_LL7E0:
return 1;_LL7E1:;_LL7E2: continue;_LL7DE:;}goto _LL7D7;_LL7DC:;_LL7DD: goto _LL7D7;
_LL7D7:;}return 0;}struct Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(void*t,
struct Cyc_List_List*atts);struct Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(
void*t,struct Cyc_List_List*atts){void*_tmpB4D=Cyc_Tcutil_compress(t);struct Cyc_Absyn_FnInfo
_tmpB4E;struct Cyc_List_List*_tmpB4F;struct Cyc_List_List**_tmpB50;_LL7E4: if(
_tmpB4D <= (void*)4)goto _LL7E6;if(*((int*)_tmpB4D)!= 8)goto _LL7E6;_tmpB4E=((
struct Cyc_Absyn_FnType_struct*)_tmpB4D)->f1;_tmpB4F=_tmpB4E.attributes;_tmpB50=(
struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_struct*)_tmpB4D)->f1).attributes;
_LL7E5: {struct Cyc_List_List*_tmpB51=0;for(0;atts != 0;atts=atts->tl){if(Cyc_Absyn_fntype_att((
void*)atts->hd)){struct Cyc_List_List*_tmp109B;*_tmpB50=((_tmp109B=_cycalloc(
sizeof(*_tmp109B)),((_tmp109B->hd=(void*)((void*)atts->hd),((_tmp109B->tl=*
_tmpB50,_tmp109B))))));}else{struct Cyc_List_List*_tmp109C;_tmpB51=((_tmp109C=
_cycalloc(sizeof(*_tmp109C)),((_tmp109C->hd=(void*)((void*)atts->hd),((_tmp109C->tl=
_tmpB51,_tmp109C))))));}}return _tmpB51;}_LL7E6:;_LL7E7: {const char*_tmp109F;void*
_tmp109E;(_tmp109E=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmp109F="transfer_fn_type_atts",_tag_dyneither(_tmp109F,
sizeof(char),22))),_tag_dyneither(_tmp109E,sizeof(void*),0)));}_LL7E3:;}
