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
void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);void Cyc_Tc_tcAggrdecl(
struct Cyc_Tcenv_Tenv*,struct Cyc_Tcenv_Genv*,struct Cyc_Position_Segment*,struct
Cyc_Absyn_Aggrdecl*);void Cyc_Tc_tcTuniondecl(struct Cyc_Tcenv_Tenv*,struct Cyc_Tcenv_Genv*,
struct Cyc_Position_Segment*,struct Cyc_Absyn_Tuniondecl*);void Cyc_Tc_tcEnumdecl(
struct Cyc_Tcenv_Tenv*,struct Cyc_Tcenv_Genv*,struct Cyc_Position_Segment*,struct
Cyc_Absyn_Enumdecl*);extern int Cyc_Cyclone_tovc_r;char Cyc_Tcutil_Unify[10]="\000\000\000\000Unify\000";
void Cyc_Tcutil_unify_it(void*t1,void*t2);void*Cyc_Tcutil_t1_failure=(void*)0;int
Cyc_Tcutil_tq1_const=0;void*Cyc_Tcutil_t2_failure=(void*)0;int Cyc_Tcutil_tq2_const=
0;struct _dyneither_ptr Cyc_Tcutil_failure_reason=(struct _dyneither_ptr){(void*)0,(
void*)0,(void*)(0 + 0)};void Cyc_Tcutil_explain_failure();void Cyc_Tcutil_explain_failure(){
if(Cyc_Position_num_errors >= Cyc_Position_max_errors)return;Cyc_fflush((struct
Cyc___cycFILE*)Cyc_stderr);{const char*_tmpB5A;const char*_tmpB59;const char*
_tmpB58;void*_tmpB57[2];struct Cyc_String_pa_struct _tmpB56;struct Cyc_String_pa_struct
_tmpB55;struct _dyneither_ptr s1=(struct _dyneither_ptr)((_tmpB55.tag=0,((_tmpB55.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(Cyc_Tcutil_t1_failure)),((
_tmpB56.tag=0,((_tmpB56.f1=(struct _dyneither_ptr)(Cyc_Tcutil_tq1_const?(_tmpB59="const ",
_tag_dyneither(_tmpB59,sizeof(char),7)):((_tmpB5A="",_tag_dyneither(_tmpB5A,
sizeof(char),1)))),((_tmpB57[0]=& _tmpB56,((_tmpB57[1]=& _tmpB55,Cyc_aprintf(((
_tmpB58="%s%s",_tag_dyneither(_tmpB58,sizeof(char),5))),_tag_dyneither(_tmpB57,
sizeof(void*),2))))))))))))));const char*_tmpB63;const char*_tmpB62;const char*
_tmpB61;void*_tmpB60[2];struct Cyc_String_pa_struct _tmpB5F;struct Cyc_String_pa_struct
_tmpB5E;struct _dyneither_ptr s2=(struct _dyneither_ptr)((_tmpB5E.tag=0,((_tmpB5E.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(Cyc_Tcutil_t2_failure)),((
_tmpB5F.tag=0,((_tmpB5F.f1=(struct _dyneither_ptr)(Cyc_Tcutil_tq2_const?(_tmpB62="const ",
_tag_dyneither(_tmpB62,sizeof(char),7)):((_tmpB63="",_tag_dyneither(_tmpB63,
sizeof(char),1)))),((_tmpB60[0]=& _tmpB5F,((_tmpB60[1]=& _tmpB5E,Cyc_aprintf(((
_tmpB61="%s%s",_tag_dyneither(_tmpB61,sizeof(char),5))),_tag_dyneither(_tmpB60,
sizeof(void*),2))))))))))))));int pos=2;{const char*_tmpB67;void*_tmpB66[1];struct
Cyc_String_pa_struct _tmpB65;(_tmpB65.tag=0,((_tmpB65.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)s1),((_tmpB66[0]=& _tmpB65,Cyc_fprintf(Cyc_stderr,((_tmpB67="  %s",
_tag_dyneither(_tmpB67,sizeof(char),5))),_tag_dyneither(_tmpB66,sizeof(void*),1)))))));}
pos +=_get_dyneither_size(s1,sizeof(char));if(pos + 5 >= 80){{const char*_tmpB6A;
void*_tmpB69;(_tmpB69=0,Cyc_fprintf(Cyc_stderr,((_tmpB6A="\n\t",_tag_dyneither(
_tmpB6A,sizeof(char),3))),_tag_dyneither(_tmpB69,sizeof(void*),0)));}pos=8;}
else{{const char*_tmpB6D;void*_tmpB6C;(_tmpB6C=0,Cyc_fprintf(Cyc_stderr,((_tmpB6D=" ",
_tag_dyneither(_tmpB6D,sizeof(char),2))),_tag_dyneither(_tmpB6C,sizeof(void*),0)));}
++ pos;}{const char*_tmpB70;void*_tmpB6F;(_tmpB6F=0,Cyc_fprintf(Cyc_stderr,((
_tmpB70="and ",_tag_dyneither(_tmpB70,sizeof(char),5))),_tag_dyneither(_tmpB6F,
sizeof(void*),0)));}pos +=4;if(pos + _get_dyneither_size(s2,sizeof(char))>= 80){{
const char*_tmpB73;void*_tmpB72;(_tmpB72=0,Cyc_fprintf(Cyc_stderr,((_tmpB73="\n\t",
_tag_dyneither(_tmpB73,sizeof(char),3))),_tag_dyneither(_tmpB72,sizeof(void*),0)));}
pos=8;}{const char*_tmpB77;void*_tmpB76[1];struct Cyc_String_pa_struct _tmpB75;(
_tmpB75.tag=0,((_tmpB75.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s2),((
_tmpB76[0]=& _tmpB75,Cyc_fprintf(Cyc_stderr,((_tmpB77="%s ",_tag_dyneither(
_tmpB77,sizeof(char),4))),_tag_dyneither(_tmpB76,sizeof(void*),1)))))));}pos +=
_get_dyneither_size(s2,sizeof(char))+ 1;if(pos + 17 >= 80){{const char*_tmpB7A;void*
_tmpB79;(_tmpB79=0,Cyc_fprintf(Cyc_stderr,((_tmpB7A="\n\t",_tag_dyneither(
_tmpB7A,sizeof(char),3))),_tag_dyneither(_tmpB79,sizeof(void*),0)));}pos=8;}{
const char*_tmpB7D;void*_tmpB7C;(_tmpB7C=0,Cyc_fprintf(Cyc_stderr,((_tmpB7D="are not compatible. ",
_tag_dyneither(_tmpB7D,sizeof(char),21))),_tag_dyneither(_tmpB7C,sizeof(void*),0)));}
pos +=17;if(Cyc_Tcutil_failure_reason.curr != (_tag_dyneither(0,0,0)).curr){if(pos
+ Cyc_strlen((struct _dyneither_ptr)Cyc_Tcutil_failure_reason)>= 80){const char*
_tmpB80;void*_tmpB7F;(_tmpB7F=0,Cyc_fprintf(Cyc_stderr,((_tmpB80="\n\t",
_tag_dyneither(_tmpB80,sizeof(char),3))),_tag_dyneither(_tmpB7F,sizeof(void*),0)));}{
const char*_tmpB84;void*_tmpB83[1];struct Cyc_String_pa_struct _tmpB82;(_tmpB82.tag=
0,((_tmpB82.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_failure_reason),((
_tmpB83[0]=& _tmpB82,Cyc_fprintf(Cyc_stderr,((_tmpB84="%s",_tag_dyneither(_tmpB84,
sizeof(char),3))),_tag_dyneither(_tmpB83,sizeof(void*),1)))))));}}{const char*
_tmpB87;void*_tmpB86;(_tmpB86=0,Cyc_fprintf(Cyc_stderr,((_tmpB87="\n",
_tag_dyneither(_tmpB87,sizeof(char),2))),_tag_dyneither(_tmpB86,sizeof(void*),0)));}
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);}}void Cyc_Tcutil_terr(struct Cyc_Position_Segment*
loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);void Cyc_Tcutil_terr(struct
Cyc_Position_Segment*loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){Cyc_Position_post_error(
Cyc_Position_mk_err_elab(loc,(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,
fmt,ap)));}void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){struct
_dyneither_ptr msg=(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap);{
const char*_tmpB8B;void*_tmpB8A[1];struct Cyc_String_pa_struct _tmpB89;(_tmpB89.tag=
0,((_tmpB89.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg),((_tmpB8A[0]=&
_tmpB89,Cyc_fprintf(Cyc_stderr,((_tmpB8B="Compiler Error (Tcutil::impos): %s\n",
_tag_dyneither(_tmpB8B,sizeof(char),36))),_tag_dyneither(_tmpB8A,sizeof(void*),1)))))));}
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);{struct Cyc_Core_Impossible_struct
_tmpB8E;struct Cyc_Core_Impossible_struct*_tmpB8D;(int)_throw((void*)((_tmpB8D=
_cycalloc(sizeof(*_tmpB8D)),((_tmpB8D[0]=((_tmpB8E.tag=Cyc_Core_Impossible,((
_tmpB8E.f1=msg,_tmpB8E)))),_tmpB8D)))));}}static struct _dyneither_ptr Cyc_Tcutil_tvar2string(
struct Cyc_Absyn_Tvar*tv);static struct _dyneither_ptr Cyc_Tcutil_tvar2string(struct
Cyc_Absyn_Tvar*tv){return*tv->name;}void Cyc_Tcutil_print_tvars(struct Cyc_List_List*
tvs);void Cyc_Tcutil_print_tvars(struct Cyc_List_List*tvs){for(0;tvs != 0;tvs=tvs->tl){
const char*_tmpB93;void*_tmpB92[2];struct Cyc_String_pa_struct _tmpB91;struct Cyc_String_pa_struct
_tmpB90;(_tmpB90.tag=0,((_tmpB90.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kindbound2string((void*)((struct Cyc_Absyn_Tvar*)tvs->hd)->kind)),((
_tmpB91.tag=0,((_tmpB91.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_tvar2string((
struct Cyc_Absyn_Tvar*)tvs->hd)),((_tmpB92[0]=& _tmpB91,((_tmpB92[1]=& _tmpB90,Cyc_fprintf(
Cyc_stderr,((_tmpB93="%s::%s ",_tag_dyneither(_tmpB93,sizeof(char),8))),
_tag_dyneither(_tmpB92,sizeof(void*),2)))))))))))));}{const char*_tmpB96;void*
_tmpB95;(_tmpB95=0,Cyc_fprintf(Cyc_stderr,((_tmpB96="\n",_tag_dyneither(_tmpB96,
sizeof(char),2))),_tag_dyneither(_tmpB95,sizeof(void*),0)));}Cyc_fflush((struct
Cyc___cycFILE*)Cyc_stderr);}static struct Cyc_List_List*Cyc_Tcutil_warning_segs=0;
static struct Cyc_List_List*Cyc_Tcutil_warning_msgs=0;void Cyc_Tcutil_warn(struct
Cyc_Position_Segment*sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);void Cyc_Tcutil_warn(
struct Cyc_Position_Segment*sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
struct _dyneither_ptr msg=(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,
fmt,ap);{struct Cyc_List_List*_tmpB97;Cyc_Tcutil_warning_segs=((_tmpB97=_cycalloc(
sizeof(*_tmpB97)),((_tmpB97->hd=sg,((_tmpB97->tl=Cyc_Tcutil_warning_segs,_tmpB97))))));}{
struct _dyneither_ptr*_tmpB9A;struct Cyc_List_List*_tmpB99;Cyc_Tcutil_warning_msgs=((
_tmpB99=_cycalloc(sizeof(*_tmpB99)),((_tmpB99->hd=((_tmpB9A=_cycalloc(sizeof(*
_tmpB9A)),((_tmpB9A[0]=msg,_tmpB9A)))),((_tmpB99->tl=Cyc_Tcutil_warning_msgs,
_tmpB99))))));}}void Cyc_Tcutil_flush_warnings();void Cyc_Tcutil_flush_warnings(){
if(Cyc_Tcutil_warning_segs == 0)return;{const char*_tmpB9D;void*_tmpB9C;(_tmpB9C=0,
Cyc_fprintf(Cyc_stderr,((_tmpB9D="***Warnings***\n",_tag_dyneither(_tmpB9D,
sizeof(char),16))),_tag_dyneither(_tmpB9C,sizeof(void*),0)));}{struct Cyc_List_List*
_tmp35=Cyc_Position_strings_of_segments(Cyc_Tcutil_warning_segs);Cyc_Tcutil_warning_segs=
0;Cyc_Tcutil_warning_msgs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_Tcutil_warning_msgs);while(Cyc_Tcutil_warning_msgs != 0){{const char*_tmpBA2;
void*_tmpBA1[2];struct Cyc_String_pa_struct _tmpBA0;struct Cyc_String_pa_struct
_tmpB9F;(_tmpB9F.tag=0,((_tmpB9F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((
struct _dyneither_ptr*)((struct Cyc_List_List*)_check_null(Cyc_Tcutil_warning_msgs))->hd)),((
_tmpBA0.tag=0,((_tmpBA0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((
struct _dyneither_ptr*)((struct Cyc_List_List*)_check_null(_tmp35))->hd)),((
_tmpBA1[0]=& _tmpBA0,((_tmpBA1[1]=& _tmpB9F,Cyc_fprintf(Cyc_stderr,((_tmpBA2="%s: %s\n",
_tag_dyneither(_tmpBA2,sizeof(char),8))),_tag_dyneither(_tmpBA1,sizeof(void*),2)))))))))))));}
_tmp35=_tmp35->tl;Cyc_Tcutil_warning_msgs=((struct Cyc_List_List*)_check_null(Cyc_Tcutil_warning_msgs))->tl;}{
const char*_tmpBA5;void*_tmpBA4;(_tmpBA4=0,Cyc_fprintf(Cyc_stderr,((_tmpBA5="**************\n",
_tag_dyneither(_tmpBA5,sizeof(char),16))),_tag_dyneither(_tmpBA4,sizeof(void*),0)));}
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
_tmpBA6;*_tmp40=((_tmpBA6=_cycalloc(sizeof(*_tmpBA6)),((_tmpBA6[0]=t2,_tmpBA6))));}
return t2;}_LL7: if(*((int*)_tmp3C)!= 0)goto _LL9;_tmp41=((struct Cyc_Absyn_Evar_struct*)
_tmp3C)->f2;_tmp42=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)_tmp3C)->f2;
_LL8: {void*t2=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(*
_tmp42))->v);if(t2 != (void*)((struct Cyc_Core_Opt*)_check_null(*_tmp42))->v){
struct Cyc_Core_Opt*_tmpBA7;*_tmp42=((_tmpBA7=_cycalloc(sizeof(*_tmpBA7)),((
_tmpBA7->v=(void*)t2,_tmpBA7))));}return t2;}_LL9:;_LLA: return t;_LL0:;}void*Cyc_Tcutil_copy_type(
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
_tmpBAA;struct Cyc_Absyn_Unknown_kb_struct*_tmpBA9;return(void*)((_tmpBA9=
_cycalloc(sizeof(*_tmpBA9)),((_tmpBA9[0]=((_tmpBAA.tag=1,((_tmpBAA.f1=0,_tmpBAA)))),
_tmpBA9))));}_LL15: if(*((int*)_tmp48)!= 2)goto _LL17;_tmp49=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp48)->f2;_LL16: {struct Cyc_Absyn_Less_kb_struct _tmpBAD;struct Cyc_Absyn_Less_kb_struct*
_tmpBAC;return(void*)((_tmpBAC=_cycalloc(sizeof(*_tmpBAC)),((_tmpBAC[0]=((
_tmpBAD.tag=2,((_tmpBAD.f1=0,((_tmpBAD.f2=(void*)_tmp49,_tmpBAD)))))),_tmpBAC))));}
_LL17:;_LL18: return kb;_LL12:;}static struct Cyc_Absyn_Tvar*Cyc_Tcutil_copy_tvar(
struct Cyc_Absyn_Tvar*tv);static struct Cyc_Absyn_Tvar*Cyc_Tcutil_copy_tvar(struct
Cyc_Absyn_Tvar*tv){struct Cyc_Absyn_Tvar*_tmpBAE;return(_tmpBAE=_cycalloc(sizeof(*
_tmpBAE)),((_tmpBAE->name=tv->name,((_tmpBAE->identity=- 1,((_tmpBAE->kind=(void*)
Cyc_Tcutil_copy_kindbound((void*)tv->kind),_tmpBAE)))))));}static struct _tuple3*
Cyc_Tcutil_copy_arg(struct _tuple3*arg);static struct _tuple3*Cyc_Tcutil_copy_arg(
struct _tuple3*arg){struct _tuple3 _tmp50;struct Cyc_Core_Opt*_tmp51;struct Cyc_Absyn_Tqual
_tmp52;void*_tmp53;struct _tuple3*_tmp4F=arg;_tmp50=*_tmp4F;_tmp51=_tmp50.f1;
_tmp52=_tmp50.f2;_tmp53=_tmp50.f3;{struct _tuple3*_tmpBAF;return(_tmpBAF=
_cycalloc(sizeof(*_tmpBAF)),((_tmpBAF->f1=_tmp51,((_tmpBAF->f2=_tmp52,((_tmpBAF->f3=
Cyc_Tcutil_copy_type(_tmp53),_tmpBAF)))))));}}static struct _tuple5*Cyc_Tcutil_copy_tqt(
struct _tuple5*arg);static struct _tuple5*Cyc_Tcutil_copy_tqt(struct _tuple5*arg){
struct _tuple5 _tmp56;struct Cyc_Absyn_Tqual _tmp57;void*_tmp58;struct _tuple5*_tmp55=
arg;_tmp56=*_tmp55;_tmp57=_tmp56.f1;_tmp58=_tmp56.f2;{struct _tuple5*_tmpBB0;
return(_tmpBB0=_cycalloc(sizeof(*_tmpBB0)),((_tmpBB0->f1=_tmp57,((_tmpBB0->f2=
Cyc_Tcutil_copy_type(_tmp58),_tmpBB0)))));}}static struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_copy_field(
struct Cyc_Absyn_Aggrfield*f);static struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_copy_field(
struct Cyc_Absyn_Aggrfield*f){struct Cyc_Absyn_Aggrfield*_tmpBB1;return(_tmpBB1=
_cycalloc(sizeof(*_tmpBB1)),((_tmpBB1->name=f->name,((_tmpBB1->tq=f->tq,((
_tmpBB1->type=(void*)Cyc_Tcutil_copy_type((void*)f->type),((_tmpBB1->width=f->width,((
_tmpBB1->attributes=f->attributes,_tmpBB1)))))))))));}static struct _tuple0*Cyc_Tcutil_copy_rgncmp(
struct _tuple0*x);static struct _tuple0*Cyc_Tcutil_copy_rgncmp(struct _tuple0*x){
struct _tuple0 _tmp5C;void*_tmp5D;void*_tmp5E;struct _tuple0*_tmp5B=x;_tmp5C=*
_tmp5B;_tmp5D=_tmp5C.f1;_tmp5E=_tmp5C.f2;{struct _tuple0*_tmpBB2;return(_tmpBB2=
_cycalloc(sizeof(*_tmpBB2)),((_tmpBB2->f1=Cyc_Tcutil_copy_type(_tmp5D),((_tmpBB2->f2=
Cyc_Tcutil_copy_type(_tmp5E),_tmpBB2)))));}}static struct Cyc_Absyn_Enumfield*Cyc_Tcutil_copy_enumfield(
struct Cyc_Absyn_Enumfield*f);static struct Cyc_Absyn_Enumfield*Cyc_Tcutil_copy_enumfield(
struct Cyc_Absyn_Enumfield*f){struct Cyc_Absyn_Enumfield*_tmpBB3;return(_tmpBB3=
_cycalloc(sizeof(*_tmpBB3)),((_tmpBB3->name=f->name,((_tmpBB3->tag=f->tag,((
_tmpBB3->loc=f->loc,_tmpBB3)))))));}void*Cyc_Tcutil_copy_type(void*t);void*Cyc_Tcutil_copy_type(
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
_tmp61)->f1;_LL1F: {struct Cyc_Absyn_VarType_struct _tmpBB6;struct Cyc_Absyn_VarType_struct*
_tmpBB5;return(void*)((_tmpBB5=_cycalloc(sizeof(*_tmpBB5)),((_tmpBB5[0]=((
_tmpBB6.tag=1,((_tmpBB6.f1=Cyc_Tcutil_copy_tvar(_tmp62),_tmpBB6)))),_tmpBB5))));}
_LL20: if(*((int*)_tmp61)!= 2)goto _LL22;_tmp63=((struct Cyc_Absyn_TunionType_struct*)
_tmp61)->f1;_tmp64=_tmp63.tunion_info;_tmp65=_tmp63.targs;_tmp66=_tmp63.rgn;
_LL21: {struct Cyc_Core_Opt*_tmpBB7;struct Cyc_Core_Opt*_tmp9E=(unsigned int)
_tmp66?(_tmpBB7=_cycalloc(sizeof(*_tmpBB7)),((_tmpBB7->v=(void*)Cyc_Tcutil_copy_type((
void*)_tmp66->v),_tmpBB7))): 0;struct Cyc_Absyn_TunionType_struct _tmpBBD;struct Cyc_Absyn_TunionInfo
_tmpBBC;struct Cyc_Absyn_TunionType_struct*_tmpBBB;return(void*)((_tmpBBB=
_cycalloc(sizeof(*_tmpBBB)),((_tmpBBB[0]=((_tmpBBD.tag=2,((_tmpBBD.f1=((_tmpBBC.tunion_info=
_tmp64,((_tmpBBC.targs=Cyc_Tcutil_copy_types(_tmp65),((_tmpBBC.rgn=_tmp9E,
_tmpBBC)))))),_tmpBBD)))),_tmpBBB))));}_LL22: if(*((int*)_tmp61)!= 3)goto _LL24;
_tmp67=((struct Cyc_Absyn_TunionFieldType_struct*)_tmp61)->f1;_tmp68=_tmp67.field_info;
_tmp69=_tmp67.targs;_LL23: {struct Cyc_Absyn_TunionFieldType_struct _tmpBC3;struct
Cyc_Absyn_TunionFieldInfo _tmpBC2;struct Cyc_Absyn_TunionFieldType_struct*_tmpBC1;
return(void*)((_tmpBC1=_cycalloc(sizeof(*_tmpBC1)),((_tmpBC1[0]=((_tmpBC3.tag=3,((
_tmpBC3.f1=((_tmpBC2.field_info=_tmp68,((_tmpBC2.targs=Cyc_Tcutil_copy_types(
_tmp69),_tmpBC2)))),_tmpBC3)))),_tmpBC1))));}_LL24: if(*((int*)_tmp61)!= 4)goto
_LL26;_tmp6A=((struct Cyc_Absyn_PointerType_struct*)_tmp61)->f1;_tmp6B=(void*)
_tmp6A.elt_typ;_tmp6C=_tmp6A.elt_tq;_tmp6D=_tmp6A.ptr_atts;_tmp6E=(void*)_tmp6D.rgn;
_tmp6F=_tmp6D.nullable;_tmp70=_tmp6D.bounds;_tmp71=_tmp6D.zero_term;_tmp72=
_tmp6D.ptrloc;_LL25: {void*_tmpA6=Cyc_Tcutil_copy_type(_tmp6B);void*_tmpA7=Cyc_Tcutil_copy_type(
_tmp6E);struct Cyc_Absyn_Conref*_tmpA8=((struct Cyc_Absyn_Conref*(*)(struct Cyc_Absyn_Conref*
c))Cyc_Tcutil_copy_conref)(_tmp6F);struct Cyc_Absyn_Tqual _tmpA9=_tmp6C;struct Cyc_Absyn_Conref*
_tmpAA=Cyc_Tcutil_copy_conref(_tmp70);struct Cyc_Absyn_Conref*_tmpAB=((struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref*c))Cyc_Tcutil_copy_conref)(_tmp71);struct Cyc_Absyn_PointerType_struct
_tmpBCD;struct Cyc_Absyn_PtrAtts _tmpBCC;struct Cyc_Absyn_PtrInfo _tmpBCB;struct Cyc_Absyn_PointerType_struct*
_tmpBCA;return(void*)((_tmpBCA=_cycalloc(sizeof(*_tmpBCA)),((_tmpBCA[0]=((
_tmpBCD.tag=4,((_tmpBCD.f1=((_tmpBCB.elt_typ=(void*)_tmpA6,((_tmpBCB.elt_tq=
_tmpA9,((_tmpBCB.ptr_atts=((_tmpBCC.rgn=(void*)_tmpA7,((_tmpBCC.nullable=_tmpA8,((
_tmpBCC.bounds=_tmpAA,((_tmpBCC.zero_term=_tmpAB,((_tmpBCC.ptrloc=_tmp72,_tmpBCC)))))))))),
_tmpBCB)))))),_tmpBCD)))),_tmpBCA))));}_LL26: if(*((int*)_tmp61)!= 5)goto _LL28;
_LL27: goto _LL29;_LL28: if((int)_tmp61 != 1)goto _LL2A;_LL29: goto _LL2B;_LL2A: if(
_tmp61 <= (void*)4)goto _LL46;if(*((int*)_tmp61)!= 6)goto _LL2C;_LL2B: return t;_LL2C:
if(*((int*)_tmp61)!= 7)goto _LL2E;_tmp73=((struct Cyc_Absyn_ArrayType_struct*)
_tmp61)->f1;_tmp74=(void*)_tmp73.elt_type;_tmp75=_tmp73.tq;_tmp76=_tmp73.num_elts;
_tmp77=_tmp73.zero_term;_tmp78=_tmp73.zt_loc;_LL2D: {struct Cyc_Absyn_ArrayType_struct
_tmpBD3;struct Cyc_Absyn_ArrayInfo _tmpBD2;struct Cyc_Absyn_ArrayType_struct*
_tmpBD1;return(void*)((_tmpBD1=_cycalloc(sizeof(*_tmpBD1)),((_tmpBD1[0]=((
_tmpBD3.tag=7,((_tmpBD3.f1=((_tmpBD2.elt_type=(void*)Cyc_Tcutil_copy_type(_tmp74),((
_tmpBD2.tq=_tmp75,((_tmpBD2.num_elts=_tmp76,((_tmpBD2.zero_term=((struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref*c))Cyc_Tcutil_copy_conref)(_tmp77),((_tmpBD2.zt_loc=
_tmp78,_tmpBD2)))))))))),_tmpBD3)))),_tmpBD1))));}_LL2E: if(*((int*)_tmp61)!= 8)
goto _LL30;_tmp79=((struct Cyc_Absyn_FnType_struct*)_tmp61)->f1;_tmp7A=_tmp79.tvars;
_tmp7B=_tmp79.effect;_tmp7C=(void*)_tmp79.ret_typ;_tmp7D=_tmp79.args;_tmp7E=
_tmp79.c_varargs;_tmp7F=_tmp79.cyc_varargs;_tmp80=_tmp79.rgn_po;_tmp81=_tmp79.attributes;
_LL2F: {struct Cyc_List_List*_tmpB3=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*
f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_tvar,
_tmp7A);struct Cyc_Core_Opt*_tmpBD4;struct Cyc_Core_Opt*_tmpB4=_tmp7B == 0?0:((
_tmpBD4=_cycalloc(sizeof(*_tmpBD4)),((_tmpBD4->v=(void*)Cyc_Tcutil_copy_type((
void*)_tmp7B->v),_tmpBD4))));void*_tmpB5=Cyc_Tcutil_copy_type(_tmp7C);struct Cyc_List_List*
_tmpB6=((struct Cyc_List_List*(*)(struct _tuple3*(*f)(struct _tuple3*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Tcutil_copy_arg,_tmp7D);int _tmpB7=_tmp7E;struct Cyc_Absyn_VarargInfo*
cyc_varargs2=0;if(_tmp7F != 0){struct Cyc_Absyn_VarargInfo*cv=(struct Cyc_Absyn_VarargInfo*)
_tmp7F;struct Cyc_Absyn_VarargInfo*_tmpBD5;cyc_varargs2=((_tmpBD5=_cycalloc(
sizeof(*_tmpBD5)),((_tmpBD5->name=cv->name,((_tmpBD5->tq=cv->tq,((_tmpBD5->type=(
void*)Cyc_Tcutil_copy_type((void*)cv->type),((_tmpBD5->inject=cv->inject,_tmpBD5))))))))));}{
struct Cyc_List_List*_tmpB9=((struct Cyc_List_List*(*)(struct _tuple0*(*f)(struct
_tuple0*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_rgncmp,_tmp80);
struct Cyc_List_List*_tmpBA=_tmp81;struct Cyc_Absyn_FnType_struct _tmpBDB;struct Cyc_Absyn_FnInfo
_tmpBDA;struct Cyc_Absyn_FnType_struct*_tmpBD9;return(void*)((_tmpBD9=_cycalloc(
sizeof(*_tmpBD9)),((_tmpBD9[0]=((_tmpBDB.tag=8,((_tmpBDB.f1=((_tmpBDA.tvars=
_tmpB3,((_tmpBDA.effect=_tmpB4,((_tmpBDA.ret_typ=(void*)_tmpB5,((_tmpBDA.args=
_tmpB6,((_tmpBDA.c_varargs=_tmpB7,((_tmpBDA.cyc_varargs=cyc_varargs2,((_tmpBDA.rgn_po=
_tmpB9,((_tmpBDA.attributes=_tmpBA,_tmpBDA)))))))))))))))),_tmpBDB)))),_tmpBD9))));}}
_LL30: if(*((int*)_tmp61)!= 9)goto _LL32;_tmp82=((struct Cyc_Absyn_TupleType_struct*)
_tmp61)->f1;_LL31: {struct Cyc_Absyn_TupleType_struct _tmpBDE;struct Cyc_Absyn_TupleType_struct*
_tmpBDD;return(void*)((_tmpBDD=_cycalloc(sizeof(*_tmpBDD)),((_tmpBDD[0]=((
_tmpBDE.tag=9,((_tmpBDE.f1=((struct Cyc_List_List*(*)(struct _tuple5*(*f)(struct
_tuple5*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_tqt,_tmp82),
_tmpBDE)))),_tmpBDD))));}_LL32: if(*((int*)_tmp61)!= 10)goto _LL34;_tmp83=((struct
Cyc_Absyn_AggrType_struct*)_tmp61)->f1;_tmp84=_tmp83.aggr_info;if((((((struct Cyc_Absyn_AggrType_struct*)
_tmp61)->f1).aggr_info).UnknownAggr).tag != 0)goto _LL34;_tmp85=(_tmp84.UnknownAggr).f1;
_tmp86=(_tmp84.UnknownAggr).f2;_tmp87=_tmp83.targs;_LL33: {struct Cyc_Absyn_AggrType_struct
_tmpBE8;union Cyc_Absyn_AggrInfoU_union _tmpBE7;struct Cyc_Absyn_AggrInfo _tmpBE6;
struct Cyc_Absyn_AggrType_struct*_tmpBE5;return(void*)((_tmpBE5=_cycalloc(sizeof(*
_tmpBE5)),((_tmpBE5[0]=((_tmpBE8.tag=10,((_tmpBE8.f1=((_tmpBE6.aggr_info=(union
Cyc_Absyn_AggrInfoU_union)(((_tmpBE7.UnknownAggr).tag=0,(((_tmpBE7.UnknownAggr).f1=(
void*)_tmp85,(((_tmpBE7.UnknownAggr).f2=_tmp86,_tmpBE7)))))),((_tmpBE6.targs=Cyc_Tcutil_copy_types(
_tmp87),_tmpBE6)))),_tmpBE8)))),_tmpBE5))));}_LL34: if(*((int*)_tmp61)!= 10)goto
_LL36;_tmp88=((struct Cyc_Absyn_AggrType_struct*)_tmp61)->f1;_tmp89=_tmp88.aggr_info;
if((((((struct Cyc_Absyn_AggrType_struct*)_tmp61)->f1).aggr_info).KnownAggr).tag
!= 1)goto _LL36;_tmp8A=(_tmp89.KnownAggr).f1;_tmp8B=_tmp88.targs;_LL35: {struct
Cyc_Absyn_AggrType_struct _tmpBF2;union Cyc_Absyn_AggrInfoU_union _tmpBF1;struct Cyc_Absyn_AggrInfo
_tmpBF0;struct Cyc_Absyn_AggrType_struct*_tmpBEF;return(void*)((_tmpBEF=_cycalloc(
sizeof(*_tmpBEF)),((_tmpBEF[0]=((_tmpBF2.tag=10,((_tmpBF2.f1=((_tmpBF0.aggr_info=(
union Cyc_Absyn_AggrInfoU_union)(((_tmpBF1.KnownAggr).tag=1,(((_tmpBF1.KnownAggr).f1=
_tmp8A,_tmpBF1)))),((_tmpBF0.targs=Cyc_Tcutil_copy_types(_tmp8B),_tmpBF0)))),
_tmpBF2)))),_tmpBEF))));}_LL36: if(*((int*)_tmp61)!= 11)goto _LL38;_tmp8C=(void*)((
struct Cyc_Absyn_AnonAggrType_struct*)_tmp61)->f1;_tmp8D=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp61)->f2;_LL37: {struct Cyc_Absyn_AnonAggrType_struct _tmpBF5;struct Cyc_Absyn_AnonAggrType_struct*
_tmpBF4;return(void*)((_tmpBF4=_cycalloc(sizeof(*_tmpBF4)),((_tmpBF4[0]=((
_tmpBF5.tag=11,((_tmpBF5.f1=(void*)_tmp8C,((_tmpBF5.f2=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Tcutil_copy_field,_tmp8D),_tmpBF5)))))),_tmpBF4))));}_LL38: if(*((
int*)_tmp61)!= 12)goto _LL3A;_tmp8E=((struct Cyc_Absyn_EnumType_struct*)_tmp61)->f1;
_tmp8F=((struct Cyc_Absyn_EnumType_struct*)_tmp61)->f2;_LL39: {struct Cyc_Absyn_EnumType_struct
_tmpBF8;struct Cyc_Absyn_EnumType_struct*_tmpBF7;return(void*)((_tmpBF7=_cycalloc(
sizeof(*_tmpBF7)),((_tmpBF7[0]=((_tmpBF8.tag=12,((_tmpBF8.f1=_tmp8E,((_tmpBF8.f2=
_tmp8F,_tmpBF8)))))),_tmpBF7))));}_LL3A: if(*((int*)_tmp61)!= 13)goto _LL3C;_tmp90=((
struct Cyc_Absyn_AnonEnumType_struct*)_tmp61)->f1;_LL3B: {struct Cyc_Absyn_AnonEnumType_struct
_tmpBFB;struct Cyc_Absyn_AnonEnumType_struct*_tmpBFA;return(void*)((_tmpBFA=
_cycalloc(sizeof(*_tmpBFA)),((_tmpBFA[0]=((_tmpBFB.tag=13,((_tmpBFB.f1=((struct
Cyc_List_List*(*)(struct Cyc_Absyn_Enumfield*(*f)(struct Cyc_Absyn_Enumfield*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_enumfield,_tmp90),_tmpBFB)))),
_tmpBFA))));}_LL3C: if(*((int*)_tmp61)!= 18)goto _LL3E;_tmp91=(void*)((struct Cyc_Absyn_TagType_struct*)
_tmp61)->f1;_LL3D: {struct Cyc_Absyn_TagType_struct _tmpBFE;struct Cyc_Absyn_TagType_struct*
_tmpBFD;return(void*)((_tmpBFD=_cycalloc(sizeof(*_tmpBFD)),((_tmpBFD[0]=((
_tmpBFE.tag=18,((_tmpBFE.f1=(void*)Cyc_Tcutil_copy_type(_tmp91),_tmpBFE)))),
_tmpBFD))));}_LL3E: if(*((int*)_tmp61)!= 17)goto _LL40;_tmp92=((struct Cyc_Absyn_ValueofType_struct*)
_tmp61)->f1;_LL3F: {struct Cyc_Absyn_ValueofType_struct _tmpC01;struct Cyc_Absyn_ValueofType_struct*
_tmpC00;return(void*)((_tmpC00=_cycalloc(sizeof(*_tmpC00)),((_tmpC00[0]=((
_tmpC01.tag=17,((_tmpC01.f1=_tmp92,_tmpC01)))),_tmpC00))));}_LL40: if(*((int*)
_tmp61)!= 14)goto _LL42;_tmp93=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp61)->f1;_LL41: {struct Cyc_Absyn_RgnHandleType_struct _tmpC04;struct Cyc_Absyn_RgnHandleType_struct*
_tmpC03;return(void*)((_tmpC03=_cycalloc(sizeof(*_tmpC03)),((_tmpC03[0]=((
_tmpC04.tag=14,((_tmpC04.f1=(void*)Cyc_Tcutil_copy_type(_tmp93),_tmpC04)))),
_tmpC03))));}_LL42: if(*((int*)_tmp61)!= 15)goto _LL44;_tmp94=(void*)((struct Cyc_Absyn_DynRgnType_struct*)
_tmp61)->f1;_tmp95=(void*)((struct Cyc_Absyn_DynRgnType_struct*)_tmp61)->f2;_LL43: {
struct Cyc_Absyn_DynRgnType_struct _tmpC07;struct Cyc_Absyn_DynRgnType_struct*
_tmpC06;return(void*)((_tmpC06=_cycalloc(sizeof(*_tmpC06)),((_tmpC06[0]=((
_tmpC07.tag=15,((_tmpC07.f1=(void*)Cyc_Tcutil_copy_type(_tmp94),((_tmpC07.f2=(
void*)Cyc_Tcutil_copy_type(_tmp95),_tmpC07)))))),_tmpC06))));}_LL44: if(*((int*)
_tmp61)!= 16)goto _LL46;_tmp96=((struct Cyc_Absyn_TypedefType_struct*)_tmp61)->f1;
_tmp97=((struct Cyc_Absyn_TypedefType_struct*)_tmp61)->f2;_tmp98=((struct Cyc_Absyn_TypedefType_struct*)
_tmp61)->f3;_LL45: {struct Cyc_Absyn_TypedefType_struct _tmpC0A;struct Cyc_Absyn_TypedefType_struct*
_tmpC09;return(void*)((_tmpC09=_cycalloc(sizeof(*_tmpC09)),((_tmpC09[0]=((
_tmpC0A.tag=16,((_tmpC0A.f1=_tmp96,((_tmpC0A.f2=Cyc_Tcutil_copy_types(_tmp97),((
_tmpC0A.f3=_tmp98,((_tmpC0A.f4=0,_tmpC0A)))))))))),_tmpC09))));}_LL46: if((int)
_tmp61 != 3)goto _LL48;_LL47: goto _LL49;_LL48: if((int)_tmp61 != 2)goto _LL4A;_LL49:
return t;_LL4A: if(_tmp61 <= (void*)4)goto _LL4C;if(*((int*)_tmp61)!= 19)goto _LL4C;
_tmp99=(void*)((struct Cyc_Absyn_AccessEff_struct*)_tmp61)->f1;_LL4B: {struct Cyc_Absyn_AccessEff_struct
_tmpC0D;struct Cyc_Absyn_AccessEff_struct*_tmpC0C;return(void*)((_tmpC0C=
_cycalloc(sizeof(*_tmpC0C)),((_tmpC0C[0]=((_tmpC0D.tag=19,((_tmpC0D.f1=(void*)
Cyc_Tcutil_copy_type(_tmp99),_tmpC0D)))),_tmpC0C))));}_LL4C: if(_tmp61 <= (void*)4)
goto _LL4E;if(*((int*)_tmp61)!= 20)goto _LL4E;_tmp9A=((struct Cyc_Absyn_JoinEff_struct*)
_tmp61)->f1;_LL4D: {struct Cyc_Absyn_JoinEff_struct _tmpC10;struct Cyc_Absyn_JoinEff_struct*
_tmpC0F;return(void*)((_tmpC0F=_cycalloc(sizeof(*_tmpC0F)),((_tmpC0F[0]=((
_tmpC10.tag=20,((_tmpC10.f1=Cyc_Tcutil_copy_types(_tmp9A),_tmpC10)))),_tmpC0F))));}
_LL4E: if(_tmp61 <= (void*)4)goto _LL19;if(*((int*)_tmp61)!= 21)goto _LL19;_tmp9B=(
void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp61)->f1;_LL4F: {struct Cyc_Absyn_RgnsEff_struct
_tmpC13;struct Cyc_Absyn_RgnsEff_struct*_tmpC12;return(void*)((_tmpC12=_cycalloc(
sizeof(*_tmpC12)),((_tmpC12[0]=((_tmpC13.tag=21,((_tmpC13.f1=(void*)Cyc_Tcutil_copy_type(
_tmp9B),_tmpC13)))),_tmpC12))));}_LL19:;}int Cyc_Tcutil_kind_leq(void*k1,void*k2);
int Cyc_Tcutil_kind_leq(void*k1,void*k2){if(k1 == k2)return 1;{struct _tuple0 _tmpC14;
struct _tuple0 _tmpE0=(_tmpC14.f1=k1,((_tmpC14.f2=k2,_tmpC14)));void*_tmpE1;void*
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
_tmpEC)->f2;_LL6A: return _tmpEE;_LL6B:;_LL6C: {const char*_tmpC17;void*_tmpC16;(
_tmpC16=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmpC17="kind not suitably constrained!",_tag_dyneither(_tmpC17,sizeof(char),31))),
_tag_dyneither(_tmpC16,sizeof(void*),0)));}_LL66:;}void*Cyc_Tcutil_typ_kind(void*
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
_tmpC1A;void*_tmpC19;(_tmpC19=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpC1A="typ_kind: Unresolved TunionFieldType",
_tag_dyneither(_tmpC1A,sizeof(char),37))),_tag_dyneither(_tmpC19,sizeof(void*),0)));}
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
_tmpC1D;void*_tmpC1C;(_tmpC1C=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpC1D="typ_kind: forward constr in ptr bounds",
_tag_dyneither(_tmpC1D,sizeof(char),39))),_tag_dyneither(_tmpC1C,sizeof(void*),0)));}
_LLAA:;}_LL9A: if(_tmpF1 <= (void*)4)goto _LL9C;if(*((int*)_tmpF1)!= 17)goto _LL9C;
_LL9B: return(void*)7;_LL9C: if(_tmpF1 <= (void*)4)goto _LL9E;if(*((int*)_tmpF1)!= 
18)goto _LL9E;_LL9D: return(void*)2;_LL9E: if(_tmpF1 <= (void*)4)goto _LLA0;if(*((int*)
_tmpF1)!= 7)goto _LLA0;_LL9F: goto _LLA1;_LLA0: if(_tmpF1 <= (void*)4)goto _LLA2;if(*((
int*)_tmpF1)!= 9)goto _LLA2;_LLA1: return(void*)1;_LLA2: if(_tmpF1 <= (void*)4)goto
_LLA4;if(*((int*)_tmpF1)!= 16)goto _LLA4;_tmp10D=((struct Cyc_Absyn_TypedefType_struct*)
_tmpF1)->f3;_LLA3: if(_tmp10D == 0  || _tmp10D->kind == 0){const char*_tmpC21;void*
_tmpC20[1];struct Cyc_String_pa_struct _tmpC1F;(_tmpC1F.tag=0,((_tmpC1F.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpC20[0]=&
_tmpC1F,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmpC21="typ_kind: typedef found: %s",_tag_dyneither(_tmpC21,sizeof(char),28))),
_tag_dyneither(_tmpC20,sizeof(void*),1)))))));}return(void*)((struct Cyc_Core_Opt*)
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
env,_tmp11F)){{const char*_tmpC22;Cyc_Tcutil_failure_reason=((_tmpC22="(type variable would escape scope)",
_tag_dyneither(_tmpC22,sizeof(char),35)));}(int)_throw((void*)Cyc_Tcutil_Unify);}
goto _LLB8;_LLBB: if(*((int*)_tmp11E)!= 0)goto _LLBD;_tmp120=((struct Cyc_Absyn_Evar_struct*)
_tmp11E)->f2;_tmp121=((struct Cyc_Absyn_Evar_struct*)_tmp11E)->f4;_tmp122=(struct
Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)_tmp11E)->f4;_LLBC: if(t == evar){{
const char*_tmpC23;Cyc_Tcutil_failure_reason=((_tmpC23="(occurs check)",
_tag_dyneither(_tmpC23,sizeof(char),15)));}(int)_throw((void*)Cyc_Tcutil_Unify);}
else{if(_tmp120 != 0)Cyc_Tcutil_occurs(evar,r,env,(void*)_tmp120->v);else{int
problem=0;{struct Cyc_List_List*s=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(*_tmp122))->v;for(0;s != 0;s=s->tl){if(!((int(*)(int(*compare)(struct
Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*
x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,(struct Cyc_Absyn_Tvar*)s->hd)){
problem=1;break;}}}if(problem){struct Cyc_List_List*_tmp140=0;{struct Cyc_List_List*
s=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*_tmp122))->v;for(0;s
!= 0;s=s->tl){if(((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,
env,(struct Cyc_Absyn_Tvar*)s->hd)){struct Cyc_List_List*_tmpC24;_tmp140=((_tmpC24=
_cycalloc(sizeof(*_tmpC24)),((_tmpC24->hd=(struct Cyc_Absyn_Tvar*)s->hd,((_tmpC24->tl=
_tmp140,_tmpC24))))));}}}{struct Cyc_Core_Opt*_tmpC25;*_tmp122=((_tmpC25=
_cycalloc(sizeof(*_tmpC25)),((_tmpC25->v=_tmp140,_tmpC25))));}}}}goto _LLB8;_LLBD:
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
 && !tq1.real_const){const char*_tmpC28;void*_tmpC27;(_tmpC27=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpC28="tq1 real_const not set.",
_tag_dyneither(_tmpC28,sizeof(char),24))),_tag_dyneither(_tmpC27,sizeof(void*),0)));}
if(tq2.print_const  && !tq2.real_const){const char*_tmpC2B;void*_tmpC2A;(_tmpC2A=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmpC2B="tq2 real_const not set.",_tag_dyneither(_tmpC2B,sizeof(char),24))),
_tag_dyneither(_tmpC2A,sizeof(void*),0)));}if((tq1.real_const != tq2.real_const
 || tq1.q_volatile != tq2.q_volatile) || tq1.q_restrict != tq2.q_restrict){Cyc_Tcutil_t1_failure=
t1;Cyc_Tcutil_t2_failure=t2;Cyc_Tcutil_tq1_const=tq1.real_const;Cyc_Tcutil_tq2_const=
tq2.real_const;{const char*_tmpC2C;Cyc_Tcutil_failure_reason=((_tmpC2C="(qualifiers don't match)",
_tag_dyneither(_tmpC2C,sizeof(char),25)));}(int)_throw((void*)Cyc_Tcutil_Unify);}
Cyc_Tcutil_tq1_const=0;Cyc_Tcutil_tq2_const=0;}int Cyc_Tcutil_equal_tqual(struct
Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2);int Cyc_Tcutil_equal_tqual(struct Cyc_Absyn_Tqual
tq1,struct Cyc_Absyn_Tqual tq2){return(tq1.real_const == tq2.real_const  && tq1.q_volatile
== tq2.q_volatile) && tq1.q_restrict == tq2.q_restrict;}static void Cyc_Tcutil_unify_it_conrefs(
int(*cmp)(void*,void*),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y,struct
_dyneither_ptr reason);static void Cyc_Tcutil_unify_it_conrefs(int(*cmp)(void*,void*),
struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y,struct _dyneither_ptr reason){x=
Cyc_Absyn_compress_conref(x);y=Cyc_Absyn_compress_conref(y);if(x == y)return;{
union Cyc_Absyn_Constraint_union _tmp14C=x->v;void*_tmp14D;_LLDC: if((_tmp14C.No_constr).tag
!= 2)goto _LLDE;_LLDD:{union Cyc_Absyn_Constraint_union _tmpC2D;x->v=(union Cyc_Absyn_Constraint_union)(((
_tmpC2D.Forward_constr).tag=1,(((_tmpC2D.Forward_constr).f1=y,_tmpC2D))));}
return;_LLDE: if((_tmp14C.Eq_constr).tag != 0)goto _LLE0;_tmp14D=(_tmp14C.Eq_constr).f1;
_LLDF: {union Cyc_Absyn_Constraint_union _tmp14F=y->v;void*_tmp150;_LLE3: if((
_tmp14F.No_constr).tag != 2)goto _LLE5;_LLE4: y->v=x->v;return;_LLE5: if((_tmp14F.Eq_constr).tag
!= 0)goto _LLE7;_tmp150=(_tmp14F.Eq_constr).f1;_LLE6: if(cmp(_tmp14D,_tmp150)!= 0){
Cyc_Tcutil_failure_reason=reason;(int)_throw((void*)Cyc_Tcutil_Unify);}return;
_LLE7: if((_tmp14F.Forward_constr).tag != 1)goto _LLE2;_LLE8: {const char*_tmpC30;
void*_tmpC2F;(_tmpC2F=0,Cyc_Tcutil_impos(((_tmpC30="unify_conref: forward after compress(2)",
_tag_dyneither(_tmpC30,sizeof(char),40))),_tag_dyneither(_tmpC2F,sizeof(void*),0)));}
_LLE2:;}_LLE0: if((_tmp14C.Forward_constr).tag != 1)goto _LLDB;_LLE1: {const char*
_tmpC33;void*_tmpC32;(_tmpC32=0,Cyc_Tcutil_impos(((_tmpC33="unify_conref: forward after compress",
_tag_dyneither(_tmpC33,sizeof(char),37))),_tag_dyneither(_tmpC32,sizeof(void*),0)));}
_LLDB:;}}static int Cyc_Tcutil_unify_conrefs(int(*cmp)(void*,void*),struct Cyc_Absyn_Conref*
x,struct Cyc_Absyn_Conref*y);static int Cyc_Tcutil_unify_conrefs(int(*cmp)(void*,
void*),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y){struct _handler_cons
_tmp155;_push_handler(& _tmp155);{int _tmp157=0;if(setjmp(_tmp155.handler))_tmp157=
1;if(!_tmp157){Cyc_Tcutil_unify_it_conrefs(cmp,x,y,(struct _dyneither_ptr)
_tag_dyneither(0,0,0));{int _tmp158=1;_npop_handler(0);return _tmp158;};
_pop_handler();}else{void*_tmp156=(void*)_exn_thrown;void*_tmp15A=_tmp156;_LLEA:
if(_tmp15A != Cyc_Tcutil_Unify)goto _LLEC;_LLEB: return 0;_LLEC:;_LLED:(void)_throw(
_tmp15A);_LLE9:;}}}static int Cyc_Tcutil_boundscmp(void*b1,void*b2);static int Cyc_Tcutil_boundscmp(
void*b1,void*b2){struct _tuple0 _tmpC34;struct _tuple0 _tmp15C=(_tmpC34.f1=b1,((
_tmpC34.f2=b2,_tmpC34)));void*_tmp15D;void*_tmp15E;void*_tmp15F;void*_tmp160;
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
_tuple0 _tmpC35;struct _tuple0 _tmp166=(_tmpC35.f1=b1,((_tmpC35.f2=b2,_tmpC35)));
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
_tuple0 _tmpC36;struct _tuple0 _tmp171=(_tmpC36.f1=att1,((_tmpC36.f2=att2,_tmpC36)));
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
_LL142:{struct Cyc_Absyn_ValueofType_struct _tmpC39;struct Cyc_Absyn_ValueofType_struct*
_tmpC38;t=(void*)((_tmpC38=_cycalloc(sizeof(*_tmpC38)),((_tmpC38[0]=((_tmpC39.tag=
17,((_tmpC39.f1=Cyc_Absyn_uint_exp(0,0),_tmpC39)))),_tmpC38))));}goto _LL13A;
_LL143:;_LL144: t=Cyc_Absyn_sint_typ;goto _LL13A;_LL13A:;}{struct _tuple8*_tmpC3A;
return(_tmpC3A=_cycalloc(sizeof(*_tmpC3A)),((_tmpC3A->f1=tv,((_tmpC3A->f2=t,
_tmpC3A)))));}}static void*Cyc_Tcutil_rgns_of(void*t);static void*Cyc_Tcutil_rgns_of(
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
_tmpC3D;struct Cyc_Absyn_AccessEff_struct*_tmpC3C;return(void*)((_tmpC3C=
_cycalloc(sizeof(*_tmpC3C)),((_tmpC3C[0]=((_tmpC3D.tag=19,((_tmpC3D.f1=(void*)t,
_tmpC3D)))),_tmpC3C))));}_LL181: if((int)_tmp1AC != 6)goto _LL183;_LL182: return t;
_LL183: if((int)_tmp1AC != 7)goto _LL185;_LL184: return Cyc_Absyn_empty_effect;_LL185:;
_LL186: {struct Cyc_Absyn_RgnsEff_struct _tmpC40;struct Cyc_Absyn_RgnsEff_struct*
_tmpC3F;return(void*)((_tmpC3F=_cycalloc(sizeof(*_tmpC3F)),((_tmpC3F[0]=((
_tmpC40.tag=21,((_tmpC40.f1=(void*)t,_tmpC40)))),_tmpC3F))));}_LL17A:;}_LL158:
if(*((int*)_tmp190)!= 14)goto _LL15A;_tmp191=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp190)->f1;_LL159: {struct Cyc_Absyn_AccessEff_struct _tmpC43;struct Cyc_Absyn_AccessEff_struct*
_tmpC42;return(void*)((_tmpC42=_cycalloc(sizeof(*_tmpC42)),((_tmpC42[0]=((
_tmpC43.tag=19,((_tmpC43.f1=(void*)_tmp191,_tmpC43)))),_tmpC42))));}_LL15A: if(*((
int*)_tmp190)!= 15)goto _LL15C;_tmp192=(void*)((struct Cyc_Absyn_DynRgnType_struct*)
_tmp190)->f1;_tmp193=(void*)((struct Cyc_Absyn_DynRgnType_struct*)_tmp190)->f2;
_LL15B: {struct Cyc_Absyn_AccessEff_struct _tmpC46;struct Cyc_Absyn_AccessEff_struct*
_tmpC45;return(void*)((_tmpC45=_cycalloc(sizeof(*_tmpC45)),((_tmpC45[0]=((
_tmpC46.tag=19,((_tmpC46.f1=(void*)_tmp193,_tmpC46)))),_tmpC45))));}_LL15C: if(*((
int*)_tmp190)!= 2)goto _LL15E;_tmp194=((struct Cyc_Absyn_TunionType_struct*)
_tmp190)->f1;_tmp195=_tmp194.targs;_tmp196=_tmp194.rgn;_LL15D: {struct Cyc_List_List*
ts=Cyc_List_map(Cyc_Tcutil_rgns_of,_tmp195);if((unsigned int)_tmp196){struct Cyc_Absyn_AccessEff_struct*
_tmpC4C;struct Cyc_Absyn_AccessEff_struct _tmpC4B;struct Cyc_List_List*_tmpC4A;ts=((
_tmpC4A=_cycalloc(sizeof(*_tmpC4A)),((_tmpC4A->hd=(void*)((void*)((_tmpC4C=
_cycalloc(sizeof(*_tmpC4C)),((_tmpC4C[0]=((_tmpC4B.tag=19,((_tmpC4B.f1=(void*)((
void*)_tmp196->v),_tmpC4B)))),_tmpC4C))))),((_tmpC4A->tl=ts,_tmpC4A))))));}{
struct Cyc_Absyn_JoinEff_struct _tmpC4F;struct Cyc_Absyn_JoinEff_struct*_tmpC4E;
return Cyc_Tcutil_normalize_effect((void*)((_tmpC4E=_cycalloc(sizeof(*_tmpC4E)),((
_tmpC4E[0]=((_tmpC4F.tag=20,((_tmpC4F.f1=ts,_tmpC4F)))),_tmpC4E)))));}}_LL15E:
if(*((int*)_tmp190)!= 4)goto _LL160;_tmp197=((struct Cyc_Absyn_PointerType_struct*)
_tmp190)->f1;_tmp198=(void*)_tmp197.elt_typ;_tmp199=_tmp197.ptr_atts;_tmp19A=(
void*)_tmp199.rgn;_LL15F: {struct Cyc_Absyn_JoinEff_struct _tmpC5E;struct Cyc_Absyn_AccessEff_struct*
_tmpC5D;struct Cyc_Absyn_AccessEff_struct _tmpC5C;void*_tmpC5B[2];struct Cyc_Absyn_JoinEff_struct*
_tmpC5A;return Cyc_Tcutil_normalize_effect((void*)((_tmpC5A=_cycalloc(sizeof(*
_tmpC5A)),((_tmpC5A[0]=((_tmpC5E.tag=20,((_tmpC5E.f1=((_tmpC5B[1]=Cyc_Tcutil_rgns_of(
_tmp198),((_tmpC5B[0]=(void*)((_tmpC5D=_cycalloc(sizeof(*_tmpC5D)),((_tmpC5D[0]=((
_tmpC5C.tag=19,((_tmpC5C.f1=(void*)_tmp19A,_tmpC5C)))),_tmpC5D)))),Cyc_List_list(
_tag_dyneither(_tmpC5B,sizeof(void*),2)))))),_tmpC5E)))),_tmpC5A)))));}_LL160:
if(*((int*)_tmp190)!= 7)goto _LL162;_tmp19B=((struct Cyc_Absyn_ArrayType_struct*)
_tmp190)->f1;_tmp19C=(void*)_tmp19B.elt_type;_LL161: return Cyc_Tcutil_normalize_effect(
Cyc_Tcutil_rgns_of(_tmp19C));_LL162: if(*((int*)_tmp190)!= 9)goto _LL164;_tmp19D=((
struct Cyc_Absyn_TupleType_struct*)_tmp190)->f1;_LL163: {struct Cyc_List_List*
_tmp1BF=0;for(0;_tmp19D != 0;_tmp19D=_tmp19D->tl){struct Cyc_List_List*_tmpC5F;
_tmp1BF=((_tmpC5F=_cycalloc(sizeof(*_tmpC5F)),((_tmpC5F->hd=(void*)(*((struct
_tuple5*)_tmp19D->hd)).f2,((_tmpC5F->tl=_tmp1BF,_tmpC5F))))));}_tmp19F=_tmp1BF;
goto _LL165;}_LL164: if(*((int*)_tmp190)!= 3)goto _LL166;_tmp19E=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp190)->f1;_tmp19F=_tmp19E.targs;_LL165: _tmp1A1=_tmp19F;goto _LL167;_LL166: if(*((
int*)_tmp190)!= 10)goto _LL168;_tmp1A0=((struct Cyc_Absyn_AggrType_struct*)_tmp190)->f1;
_tmp1A1=_tmp1A0.targs;_LL167: {struct Cyc_Absyn_JoinEff_struct _tmpC62;struct Cyc_Absyn_JoinEff_struct*
_tmpC61;return Cyc_Tcutil_normalize_effect((void*)((_tmpC61=_cycalloc(sizeof(*
_tmpC61)),((_tmpC61[0]=((_tmpC62.tag=20,((_tmpC62.f1=Cyc_List_map(Cyc_Tcutil_rgns_of,
_tmp1A1),_tmpC62)))),_tmpC61)))));}_LL168: if(*((int*)_tmp190)!= 11)goto _LL16A;
_tmp1A2=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp190)->f2;_LL169: {struct Cyc_Absyn_JoinEff_struct
_tmpC65;struct Cyc_Absyn_JoinEff_struct*_tmpC64;return Cyc_Tcutil_normalize_effect((
void*)((_tmpC64=_cycalloc(sizeof(*_tmpC64)),((_tmpC64[0]=((_tmpC65.tag=20,((
_tmpC65.f1=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Aggrfield*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of_field,_tmp1A2),_tmpC65)))),
_tmpC64)))));}_LL16A: if(*((int*)_tmp190)!= 18)goto _LL16C;_LL16B: return Cyc_Absyn_empty_effect;
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
_tmp190)->f2;_LL179: {struct Cyc_Absyn_JoinEff_struct _tmpC68;struct Cyc_Absyn_JoinEff_struct*
_tmpC67;return Cyc_Tcutil_normalize_effect((void*)((_tmpC67=_cycalloc(sizeof(*
_tmpC67)),((_tmpC67[0]=((_tmpC68.tag=20,((_tmpC68.f1=Cyc_List_map(Cyc_Tcutil_rgns_of,
_tmp1AB),_tmpC68)))),_tmpC67)))));}_LL145:;}void*Cyc_Tcutil_normalize_effect(
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
struct Cyc_List_List*_tmpC69;effects=((_tmpC69=_cycalloc(sizeof(*_tmpC69)),((
_tmpC69->hd=(void*)_tmp1CF,((_tmpC69->tl=effects,_tmpC69))))));}goto _LL195;
_LL195:;}}*_tmp1CA=Cyc_List_imp_rev(effects);return e;}}_LL18A: if(*((int*)_tmp1C8)
!= 21)goto _LL18C;_tmp1CB=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp1C8)->f1;
_LL18B: {void*_tmp1D3=Cyc_Tcutil_compress(_tmp1CB);_LL19D: if(_tmp1D3 <= (void*)4)
goto _LL1A1;if(*((int*)_tmp1D3)!= 0)goto _LL19F;_LL19E: goto _LL1A0;_LL19F: if(*((int*)
_tmp1D3)!= 1)goto _LL1A1;_LL1A0: return e;_LL1A1:;_LL1A2: return Cyc_Tcutil_rgns_of(
_tmp1CB);_LL19C:;}_LL18C:;_LL18D: return e;_LL187:;}}static void*Cyc_Tcutil_dummy_fntype(
void*eff);static void*Cyc_Tcutil_dummy_fntype(void*eff){struct Cyc_Absyn_FnType_struct
_tmpC73;struct Cyc_Core_Opt*_tmpC72;struct Cyc_Absyn_FnInfo _tmpC71;struct Cyc_Absyn_FnType_struct*
_tmpC70;struct Cyc_Absyn_FnType_struct*_tmp1D4=(_tmpC70=_cycalloc(sizeof(*_tmpC70)),((
_tmpC70[0]=((_tmpC73.tag=8,((_tmpC73.f1=((_tmpC71.tvars=0,((_tmpC71.effect=((
_tmpC72=_cycalloc(sizeof(*_tmpC72)),((_tmpC72->v=(void*)eff,_tmpC72)))),((
_tmpC71.ret_typ=(void*)((void*)0),((_tmpC71.args=0,((_tmpC71.c_varargs=0,((
_tmpC71.cyc_varargs=0,((_tmpC71.rgn_po=0,((_tmpC71.attributes=0,_tmpC71)))))))))))))))),
_tmpC73)))),_tmpC70)));return Cyc_Absyn_atb_typ((void*)_tmp1D4,(void*)2,Cyc_Absyn_empty_tqual(
0),Cyc_Absyn_bounds_one,Cyc_Absyn_false_conref);}int Cyc_Tcutil_region_in_effect(
int constrain,void*r,void*e);int Cyc_Tcutil_region_in_effect(int constrain,void*r,
void*e){r=Cyc_Tcutil_compress(r);if(r == (void*)2  || r == (void*)3)return 1;{void*
_tmp1D9=Cyc_Tcutil_compress(e);void*_tmp1DA;struct Cyc_List_List*_tmp1DB;void*
_tmp1DC;struct Cyc_Core_Opt*_tmp1DD;struct Cyc_Core_Opt*_tmp1DE;struct Cyc_Core_Opt**
_tmp1DF;struct Cyc_Core_Opt*_tmp1E0;_LL1A4: if(_tmp1D9 <= (void*)4)goto _LL1AC;if(*((
int*)_tmp1D9)!= 19)goto _LL1A6;_tmp1DA=(void*)((struct Cyc_Absyn_AccessEff_struct*)
_tmp1D9)->f1;_LL1A5: if(constrain)return Cyc_Tcutil_unify(r,_tmp1DA);_tmp1DA=Cyc_Tcutil_compress(
_tmp1DA);if(r == _tmp1DA)return 1;{struct _tuple0 _tmpC74;struct _tuple0 _tmp1E2=(
_tmpC74.f1=r,((_tmpC74.f2=_tmp1DA,_tmpC74)));void*_tmp1E3;struct Cyc_Absyn_Tvar*
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
_tmpC83;struct Cyc_Absyn_AccessEff_struct*_tmpC82;struct Cyc_Absyn_AccessEff_struct
_tmpC81;void*_tmpC80[2];struct Cyc_Absyn_JoinEff_struct*_tmpC7F;void*_tmp1EF=Cyc_Tcutil_dummy_fntype((
void*)((_tmpC7F=_cycalloc(sizeof(*_tmpC7F)),((_tmpC7F[0]=((_tmpC83.tag=20,((
_tmpC83.f1=((_tmpC80[1]=(void*)((_tmpC82=_cycalloc(sizeof(*_tmpC82)),((_tmpC82[0]=((
_tmpC81.tag=19,((_tmpC81.f1=(void*)r,_tmpC81)))),_tmpC82)))),((_tmpC80[0]=
_tmp1EE,Cyc_List_list(_tag_dyneither(_tmpC80,sizeof(void*),2)))))),_tmpC83)))),
_tmpC7F)))));{struct Cyc_Core_Opt*_tmpC84;*_tmp1EC=((_tmpC84=_cycalloc(sizeof(*
_tmpC84)),((_tmpC84->v=(void*)_tmp1EF,_tmpC84))));}return 1;}}_LL1BB:;_LL1BC:
return 0;_LL1B8:;}_LL1B6:;_LL1B7: return Cyc_Tcutil_region_in_effect(constrain,r,
_tmp1E7);_LL1B3:;}_LL1AA: if(*((int*)_tmp1D9)!= 0)goto _LL1AC;_tmp1DD=((struct Cyc_Absyn_Evar_struct*)
_tmp1D9)->f1;_tmp1DE=((struct Cyc_Absyn_Evar_struct*)_tmp1D9)->f2;_tmp1DF=(struct
Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)_tmp1D9)->f2;_tmp1E0=((struct Cyc_Absyn_Evar_struct*)
_tmp1D9)->f4;_LL1AB: if(_tmp1DD == 0  || (void*)_tmp1DD->v != (void*)6){const char*
_tmpC87;void*_tmpC86;(_tmpC86=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpC87="effect evar has wrong kind",
_tag_dyneither(_tmpC87,sizeof(char),27))),_tag_dyneither(_tmpC86,sizeof(void*),0)));}
if(!constrain)return 0;{void*_tmp1F8=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,
_tmp1E0);Cyc_Tcutil_occurs(_tmp1F8,Cyc_Core_heap_region,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp1E0))->v,r);{struct Cyc_Absyn_JoinEff_struct
_tmpC9C;struct Cyc_List_List*_tmpC9B;struct Cyc_Absyn_AccessEff_struct _tmpC9A;
struct Cyc_Absyn_AccessEff_struct*_tmpC99;struct Cyc_List_List*_tmpC98;struct Cyc_Absyn_JoinEff_struct*
_tmpC97;struct Cyc_Absyn_JoinEff_struct*_tmp1F9=(_tmpC97=_cycalloc(sizeof(*
_tmpC97)),((_tmpC97[0]=((_tmpC9C.tag=20,((_tmpC9C.f1=((_tmpC98=_cycalloc(sizeof(*
_tmpC98)),((_tmpC98->hd=(void*)_tmp1F8,((_tmpC98->tl=((_tmpC9B=_cycalloc(sizeof(*
_tmpC9B)),((_tmpC9B->hd=(void*)((void*)((_tmpC99=_cycalloc(sizeof(*_tmpC99)),((
_tmpC99[0]=((_tmpC9A.tag=19,((_tmpC9A.f1=(void*)r,_tmpC9A)))),_tmpC99))))),((
_tmpC9B->tl=0,_tmpC9B)))))),_tmpC98)))))),_tmpC9C)))),_tmpC97)));{struct Cyc_Core_Opt*
_tmpC9D;*_tmp1DF=((_tmpC9D=_cycalloc(sizeof(*_tmpC9D)),((_tmpC9D->v=(void*)((
void*)_tmp1F9),_tmpC9D))));}return 1;}}_LL1AC:;_LL1AD: return 0;_LL1A3:;}}static int
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
Cyc_Absyn_RgnsEff_struct*)_tmp208)->f1;_LL1CA: {struct _tuple0 _tmpC9E;struct
_tuple0 _tmp20B=(_tmpC9E.f1=Cyc_Tcutil_compress(_tmp209),((_tmpC9E.f2=_tmp203,
_tmpC9E)));void*_tmp20C;struct Cyc_Absyn_Tvar*_tmp20D;void*_tmp20E;struct Cyc_Absyn_Tvar*
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
_tmpCA1;void*_tmpCA0;(_tmpCA0=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpCA1="effect evar has wrong kind",
_tag_dyneither(_tmpCA1,sizeof(char),27))),_tag_dyneither(_tmpCA0,sizeof(void*),0)));}
if(!may_constrain_evars)return 0;{void*_tmp212=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_ek,_tmp207);Cyc_Tcutil_occurs(_tmp212,Cyc_Core_heap_region,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp207))->v,t);{struct Cyc_Absyn_JoinEff_struct
_tmpCB6;struct Cyc_List_List*_tmpCB5;struct Cyc_Absyn_RgnsEff_struct _tmpCB4;struct
Cyc_Absyn_RgnsEff_struct*_tmpCB3;struct Cyc_List_List*_tmpCB2;struct Cyc_Absyn_JoinEff_struct*
_tmpCB1;struct Cyc_Absyn_JoinEff_struct*_tmp213=(_tmpCB1=_cycalloc(sizeof(*
_tmpCB1)),((_tmpCB1[0]=((_tmpCB6.tag=20,((_tmpCB6.f1=((_tmpCB2=_cycalloc(sizeof(*
_tmpCB2)),((_tmpCB2->hd=(void*)_tmp212,((_tmpCB2->tl=((_tmpCB5=_cycalloc(sizeof(*
_tmpCB5)),((_tmpCB5->hd=(void*)((void*)((_tmpCB3=_cycalloc(sizeof(*_tmpCB3)),((
_tmpCB3[0]=((_tmpCB4.tag=21,((_tmpCB4.f1=(void*)t,_tmpCB4)))),_tmpCB3))))),((
_tmpCB5->tl=0,_tmpCB5)))))),_tmpCB2)))))),_tmpCB6)))),_tmpCB1)));{struct Cyc_Core_Opt*
_tmpCB7;*_tmp206=((_tmpCB7=_cycalloc(sizeof(*_tmpCB7)),((_tmpCB7->v=(void*)((
void*)_tmp213),_tmpCB7))));}return 1;}}_LL1C6:;_LL1C7: return 0;_LL1BD:;}}static int
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
_tmpCC6;struct Cyc_Absyn_VarType_struct*_tmpCC5;struct Cyc_Absyn_VarType_struct
_tmpCC4;void*_tmpCC3[2];struct Cyc_Absyn_JoinEff_struct*_tmpCC2;void*_tmp22B=Cyc_Tcutil_dummy_fntype((
void*)((_tmpCC2=_cycalloc(sizeof(*_tmpCC2)),((_tmpCC2[0]=((_tmpCC6.tag=20,((
_tmpCC6.f1=((_tmpCC3[1]=(void*)((_tmpCC5=_cycalloc(sizeof(*_tmpCC5)),((_tmpCC5[0]=((
_tmpCC4.tag=1,((_tmpCC4.f1=v,_tmpCC4)))),_tmpCC5)))),((_tmpCC3[0]=_tmp22A,Cyc_List_list(
_tag_dyneither(_tmpCC3,sizeof(void*),2)))))),_tmpCC6)))),_tmpCC2)))));{struct Cyc_Core_Opt*
_tmpCC7;*_tmp228=((_tmpCC7=_cycalloc(sizeof(*_tmpCC7)),((_tmpCC7->v=(void*)
_tmp22B,_tmpCC7))));}return 1;}}_LL1E5:;_LL1E6: return 0;_LL1E2:;}_LL1E0:;_LL1E1:
return Cyc_Tcutil_variable_in_effect(may_constrain_evars,v,_tmp223);_LL1DD:;}
_LL1D9: if(*((int*)_tmp21B)!= 0)goto _LL1DB;_tmp21F=((struct Cyc_Absyn_Evar_struct*)
_tmp21B)->f1;_tmp220=((struct Cyc_Absyn_Evar_struct*)_tmp21B)->f2;_tmp221=(struct
Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)_tmp21B)->f2;_tmp222=((struct Cyc_Absyn_Evar_struct*)
_tmp21B)->f4;_LL1DA: if(_tmp21F == 0  || (void*)_tmp21F->v != (void*)6){const char*
_tmpCCA;void*_tmpCC9;(_tmpCC9=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpCCA="effect evar has wrong kind",
_tag_dyneither(_tmpCCA,sizeof(char),27))),_tag_dyneither(_tmpCC9,sizeof(void*),0)));}{
void*_tmp234=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,_tmp222);if(
!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*
l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp222))->v,v))return 0;{struct Cyc_Absyn_JoinEff_struct
_tmpCDF;struct Cyc_List_List*_tmpCDE;struct Cyc_Absyn_VarType_struct _tmpCDD;struct
Cyc_Absyn_VarType_struct*_tmpCDC;struct Cyc_List_List*_tmpCDB;struct Cyc_Absyn_JoinEff_struct*
_tmpCDA;struct Cyc_Absyn_JoinEff_struct*_tmp235=(_tmpCDA=_cycalloc(sizeof(*
_tmpCDA)),((_tmpCDA[0]=((_tmpCDF.tag=20,((_tmpCDF.f1=((_tmpCDB=_cycalloc(sizeof(*
_tmpCDB)),((_tmpCDB->hd=(void*)_tmp234,((_tmpCDB->tl=((_tmpCDE=_cycalloc(sizeof(*
_tmpCDE)),((_tmpCDE->hd=(void*)((void*)((_tmpCDC=_cycalloc(sizeof(*_tmpCDC)),((
_tmpCDC[0]=((_tmpCDD.tag=1,((_tmpCDD.f1=v,_tmpCDD)))),_tmpCDC))))),((_tmpCDE->tl=
0,_tmpCDE)))))),_tmpCDB)))))),_tmpCDF)))),_tmpCDA)));{struct Cyc_Core_Opt*_tmpCE0;*
_tmp221=((_tmpCE0=_cycalloc(sizeof(*_tmpCE0)),((_tmpCE0->v=(void*)((void*)
_tmp235),_tmpCE0))));}return 1;}}_LL1DB:;_LL1DC: return 0;_LL1D2:;}}static int Cyc_Tcutil_evar_in_effect(
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
_tmpCE1;*_tmp248=((_tmpCE1=_cycalloc(sizeof(*_tmpCE1)),((_tmpCE1->v=(void*)Cyc_Absyn_empty_effect,
_tmpCE1))));}return 1;_LL200:;_LL201: {const char*_tmpCE5;void*_tmpCE4[1];struct
Cyc_String_pa_struct _tmpCE3;(_tmpCE3.tag=0,((_tmpCE3.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string(e1)),((_tmpCE4[0]=& _tmpCE3,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpCE5="subset_effect: bad effect: %s",
_tag_dyneither(_tmpCE5,sizeof(char),30))),_tag_dyneither(_tmpCE4,sizeof(void*),1)))))));}
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
_tmp259))->v)){{struct Cyc_Core_Opt*_tmpCE6;*_tmp25B=((_tmpCE6=_cycalloc(sizeof(*
_tmpCE6)),((_tmpCE6->v=(void*)t2,_tmpCE6))));}return;}else{{void*_tmp25F=t2;
struct Cyc_Core_Opt*_tmp260;struct Cyc_Core_Opt**_tmp261;struct Cyc_Core_Opt*
_tmp262;struct Cyc_Absyn_PtrInfo _tmp263;_LL20D: if(_tmp25F <= (void*)4)goto _LL211;
if(*((int*)_tmp25F)!= 0)goto _LL20F;_tmp260=((struct Cyc_Absyn_Evar_struct*)
_tmp25F)->f2;_tmp261=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)
_tmp25F)->f2;_tmp262=((struct Cyc_Absyn_Evar_struct*)_tmp25F)->f4;_LL20E: {struct
Cyc_List_List*_tmp264=(struct Cyc_List_List*)_tmp25C->v;{struct Cyc_List_List*s2=(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp262))->v;for(0;s2 != 0;
s2=s2->tl){if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,
_tmp264,(struct Cyc_Absyn_Tvar*)s2->hd)){{const char*_tmpCE7;Cyc_Tcutil_failure_reason=((
_tmpCE7="(type variable would escape scope)",_tag_dyneither(_tmpCE7,sizeof(char),
35)));}(int)_throw((void*)Cyc_Tcutil_Unify);}}}if(Cyc_Tcutil_kind_leq((void*)
_tmp259->v,_tmp25D)){{struct Cyc_Core_Opt*_tmpCE8;*_tmp261=((_tmpCE8=_cycalloc(
sizeof(*_tmpCE8)),((_tmpCE8->v=(void*)t1,_tmpCE8))));}return;}{const char*_tmpCE9;
Cyc_Tcutil_failure_reason=((_tmpCE9="(kinds are incompatible)",_tag_dyneither(
_tmpCE9,sizeof(char),25)));}goto _LL20C;}_LL20F: if(*((int*)_tmp25F)!= 4)goto
_LL211;_tmp263=((struct Cyc_Absyn_PointerType_struct*)_tmp25F)->f1;if(!((void*)
_tmp259->v == (void*)2))goto _LL211;_LL210: {struct Cyc_Absyn_Conref*_tmp268=Cyc_Absyn_compress_conref((
_tmp263.ptr_atts).bounds);{union Cyc_Absyn_Constraint_union _tmp269=_tmp268->v;
_LL214: if((_tmp269.No_constr).tag != 2)goto _LL216;_LL215:{union Cyc_Absyn_Constraint_union
_tmpCEA;_tmp268->v=(union Cyc_Absyn_Constraint_union)(((_tmpCEA.Eq_constr).tag=0,(((
_tmpCEA.Eq_constr).f1=(void*)Cyc_Absyn_bounds_one,_tmpCEA))));}{struct Cyc_Core_Opt*
_tmpCEB;*_tmp25B=((_tmpCEB=_cycalloc(sizeof(*_tmpCEB)),((_tmpCEB->v=(void*)t2,
_tmpCEB))));}return;_LL216:;_LL217: goto _LL213;_LL213:;}goto _LL20C;}_LL211:;
_LL212: goto _LL20C;_LL20C:;}{const char*_tmpCEC;Cyc_Tcutil_failure_reason=((
_tmpCEC="(kinds are incompatible)",_tag_dyneither(_tmpCEC,sizeof(char),25)));}(
int)_throw((void*)Cyc_Tcutil_Unify);}}_LL20A:;_LL20B: goto _LL207;_LL207:;}{struct
_tuple0 _tmpCED;struct _tuple0 _tmp26E=(_tmpCED.f1=t2,((_tmpCED.f2=t1,_tmpCED)));
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
_tmp304,_tmp305)== 0){if(_tmp308 != _tmp309){const char*_tmpCF3;void*_tmpCF2[3];
struct Cyc_String_pa_struct _tmpCF1;struct Cyc_String_pa_struct _tmpCF0;struct Cyc_String_pa_struct
_tmpCEF;(_tmpCEF.tag=0,((_tmpCEF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kind2string(_tmp309)),((_tmpCF0.tag=0,((_tmpCF0.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(_tmp308)),((
_tmpCF1.tag=0,((_tmpCF1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp304),((
_tmpCF2[0]=& _tmpCF1,((_tmpCF2[1]=& _tmpCF0,((_tmpCF2[2]=& _tmpCEF,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpCF3="same type variable %s has kinds %s and %s",
_tag_dyneither(_tmpCF3,sizeof(char),42))),_tag_dyneither(_tmpCF2,sizeof(void*),3)))))))))))))))))));}
return;}{const char*_tmpCF4;Cyc_Tcutil_failure_reason=((_tmpCF4="(variable types are not the same)",
_tag_dyneither(_tmpCF4,sizeof(char),34)));}goto _LL218;}_LL21F: _tmp276=_tmp26E.f1;
if(_tmp276 <= (void*)4)goto _LL221;if(*((int*)_tmp276)!= 10)goto _LL221;_tmp277=((
struct Cyc_Absyn_AggrType_struct*)_tmp276)->f1;_tmp278=_tmp277.aggr_info;_tmp279=
_tmp277.targs;_tmp27A=_tmp26E.f2;if(_tmp27A <= (void*)4)goto _LL221;if(*((int*)
_tmp27A)!= 10)goto _LL221;_tmp27B=((struct Cyc_Absyn_AggrType_struct*)_tmp27A)->f1;
_tmp27C=_tmp27B.aggr_info;_tmp27D=_tmp27B.targs;_LL220: {void*_tmp311;struct
_tuple2*_tmp312;struct _tuple6 _tmp310=Cyc_Absyn_aggr_kinded_name(_tmp27C);_tmp311=
_tmp310.f1;_tmp312=_tmp310.f2;{void*_tmp314;struct _tuple2*_tmp315;struct _tuple6
_tmp313=Cyc_Absyn_aggr_kinded_name(_tmp278);_tmp314=_tmp313.f1;_tmp315=_tmp313.f2;
if(_tmp311 != _tmp314){{const char*_tmpCF5;Cyc_Tcutil_failure_reason=((_tmpCF5="(struct and union type)",
_tag_dyneither(_tmpCF5,sizeof(char),24)));}goto _LL218;}if(Cyc_Absyn_qvar_cmp(
_tmp312,_tmp315)!= 0){{const char*_tmpCF6;Cyc_Tcutil_failure_reason=((_tmpCF6="(different type name)",
_tag_dyneither(_tmpCF6,sizeof(char),22)));}goto _LL218;}Cyc_Tcutil_unify_list(
_tmp27D,_tmp279);return;}}_LL221: _tmp27E=_tmp26E.f1;if(_tmp27E <= (void*)4)goto
_LL223;if(*((int*)_tmp27E)!= 12)goto _LL223;_tmp27F=((struct Cyc_Absyn_EnumType_struct*)
_tmp27E)->f1;_tmp280=_tmp26E.f2;if(_tmp280 <= (void*)4)goto _LL223;if(*((int*)
_tmp280)!= 12)goto _LL223;_tmp281=((struct Cyc_Absyn_EnumType_struct*)_tmp280)->f1;
_LL222: if(Cyc_Absyn_qvar_cmp(_tmp27F,_tmp281)== 0)return;{const char*_tmpCF7;Cyc_Tcutil_failure_reason=((
_tmpCF7="(different enum types)",_tag_dyneither(_tmpCF7,sizeof(char),23)));}goto
_LL218;_LL223: _tmp282=_tmp26E.f1;if(_tmp282 <= (void*)4)goto _LL225;if(*((int*)
_tmp282)!= 13)goto _LL225;_tmp283=((struct Cyc_Absyn_AnonEnumType_struct*)_tmp282)->f1;
_tmp284=_tmp26E.f2;if(_tmp284 <= (void*)4)goto _LL225;if(*((int*)_tmp284)!= 13)
goto _LL225;_tmp285=((struct Cyc_Absyn_AnonEnumType_struct*)_tmp284)->f1;_LL224: {
int bad=0;for(0;_tmp283 != 0  && _tmp285 != 0;(_tmp283=_tmp283->tl,_tmp285=_tmp285->tl)){
struct Cyc_Absyn_Enumfield*_tmp319=(struct Cyc_Absyn_Enumfield*)_tmp283->hd;struct
Cyc_Absyn_Enumfield*_tmp31A=(struct Cyc_Absyn_Enumfield*)_tmp285->hd;if(Cyc_Absyn_qvar_cmp(
_tmp319->name,_tmp31A->name)!= 0){{const char*_tmpCF8;Cyc_Tcutil_failure_reason=((
_tmpCF8="(different names for enum fields)",_tag_dyneither(_tmpCF8,sizeof(char),
34)));}bad=1;break;}if(_tmp319->tag == _tmp31A->tag)continue;if(_tmp319->tag == 0
 || _tmp31A->tag == 0){{const char*_tmpCF9;Cyc_Tcutil_failure_reason=((_tmpCF9="(different tag values for enum fields)",
_tag_dyneither(_tmpCF9,sizeof(char),39)));}bad=1;break;}if(!Cyc_Evexp_same_const_exp((
struct Cyc_Absyn_Exp*)_check_null(_tmp319->tag),(struct Cyc_Absyn_Exp*)_check_null(
_tmp31A->tag))){{const char*_tmpCFA;Cyc_Tcutil_failure_reason=((_tmpCFA="(different tag values for enum fields)",
_tag_dyneither(_tmpCFA,sizeof(char),39)));}bad=1;break;}}if(bad)goto _LL218;if(
_tmp283 == 0  && _tmp285 == 0)return;{const char*_tmpCFB;Cyc_Tcutil_failure_reason=((
_tmpCFB="(different number of fields for enums)",_tag_dyneither(_tmpCFB,sizeof(
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
Cyc_Tcutil_unify_list(_tmp292,_tmp28B);return;}Tunion_fail: {const char*_tmpCFC;
Cyc_Tcutil_failure_reason=((_tmpCFC="(different tunion types)",_tag_dyneither(
_tmpCFC,sizeof(char),25)));}goto _LL218;_LL227: _tmp294=_tmp26E.f1;if(_tmp294 <= (
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
_tmpCFD;Cyc_Tcutil_failure_reason=((_tmpCFD="(different tunion field types)",
_tag_dyneither(_tmpCFD,sizeof(char),31)));}goto _LL218;_LL229: _tmp2A0=_tmp26E.f1;
if(_tmp2A0 <= (void*)4)goto _LL22B;if(*((int*)_tmp2A0)!= 4)goto _LL22B;_tmp2A1=((
struct Cyc_Absyn_PointerType_struct*)_tmp2A0)->f1;_tmp2A2=(void*)_tmp2A1.elt_typ;
_tmp2A3=_tmp2A1.elt_tq;_tmp2A4=_tmp2A1.ptr_atts;_tmp2A5=(void*)_tmp2A4.rgn;
_tmp2A6=_tmp2A4.nullable;_tmp2A7=_tmp2A4.bounds;_tmp2A8=_tmp2A4.zero_term;
_tmp2A9=_tmp26E.f2;if(_tmp2A9 <= (void*)4)goto _LL22B;if(*((int*)_tmp2A9)!= 4)goto
_LL22B;_tmp2AA=((struct Cyc_Absyn_PointerType_struct*)_tmp2A9)->f1;_tmp2AB=(void*)
_tmp2AA.elt_typ;_tmp2AC=_tmp2AA.elt_tq;_tmp2AD=_tmp2AA.ptr_atts;_tmp2AE=(void*)
_tmp2AD.rgn;_tmp2AF=_tmp2AD.nullable;_tmp2B0=_tmp2AD.bounds;_tmp2B1=_tmp2AD.zero_term;
_LL22A: Cyc_Tcutil_unify_it(_tmp2AB,_tmp2A2);Cyc_Tcutil_unify_it(_tmp2A5,_tmp2AE);
Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;{const char*_tmpCFE;((void(*)(
int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y,struct
_dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,_tmp2B1,
_tmp2A8,((_tmpCFE="(not both zero terminated)",_tag_dyneither(_tmpCFE,sizeof(
char),27))));}Cyc_Tcutil_unify_tqual(_tmp2AC,_tmp2AB,_tmp2A3,_tmp2A2);{const char*
_tmpCFF;Cyc_Tcutil_unify_it_conrefs(Cyc_Tcutil_unify_it_bounds,_tmp2B0,_tmp2A7,((
_tmpCFF="(different pointer bounds)",_tag_dyneither(_tmpCFF,sizeof(char),27))));}{
union Cyc_Absyn_Constraint_union _tmp323=(Cyc_Absyn_compress_conref(_tmp2B0))->v;
void*_tmp324;_LL254: if((_tmp323.Eq_constr).tag != 0)goto _LL256;_tmp324=(_tmp323.Eq_constr).f1;
if((int)_tmp324 != 0)goto _LL256;_LL255: return;_LL256:;_LL257: goto _LL253;_LL253:;}{
const char*_tmpD00;((void(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct
Cyc_Absyn_Conref*y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,
_tmp2AF,_tmp2A6,((_tmpD00="(incompatible pointer types)",_tag_dyneither(_tmpD00,
sizeof(char),29))));}return;_LL22B: _tmp2B2=_tmp26E.f1;if(_tmp2B2 <= (void*)4)goto
_LL22D;if(*((int*)_tmp2B2)!= 5)goto _LL22D;_tmp2B3=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp2B2)->f1;_tmp2B4=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp2B2)->f2;
_tmp2B5=_tmp26E.f2;if(_tmp2B5 <= (void*)4)goto _LL22D;if(*((int*)_tmp2B5)!= 5)goto
_LL22D;_tmp2B6=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp2B5)->f1;_tmp2B7=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp2B5)->f2;_LL22C: if(_tmp2B6 == _tmp2B3
 && ((_tmp2B7 == _tmp2B4  || _tmp2B7 == (void*)2  && _tmp2B4 == (void*)3) || _tmp2B7
== (void*)3  && _tmp2B4 == (void*)2))return;{const char*_tmpD01;Cyc_Tcutil_failure_reason=((
_tmpD01="(different integral types)",_tag_dyneither(_tmpD01,sizeof(char),27)));}
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
_LL234: if(!Cyc_Evexp_same_const_exp(_tmp2C3,_tmp2C5)){{const char*_tmpD02;Cyc_Tcutil_failure_reason=((
_tmpD02="(cannot prove expressions are the same)",_tag_dyneither(_tmpD02,sizeof(
char),40)));}goto _LL218;}return;_LL235: _tmp2C6=_tmp26E.f1;if(_tmp2C6 <= (void*)4)
goto _LL237;if(*((int*)_tmp2C6)!= 7)goto _LL237;_tmp2C7=((struct Cyc_Absyn_ArrayType_struct*)
_tmp2C6)->f1;_tmp2C8=(void*)_tmp2C7.elt_type;_tmp2C9=_tmp2C7.tq;_tmp2CA=_tmp2C7.num_elts;
_tmp2CB=_tmp2C7.zero_term;_tmp2CC=_tmp26E.f2;if(_tmp2CC <= (void*)4)goto _LL237;
if(*((int*)_tmp2CC)!= 7)goto _LL237;_tmp2CD=((struct Cyc_Absyn_ArrayType_struct*)
_tmp2CC)->f1;_tmp2CE=(void*)_tmp2CD.elt_type;_tmp2CF=_tmp2CD.tq;_tmp2D0=_tmp2CD.num_elts;
_tmp2D1=_tmp2CD.zero_term;_LL236: Cyc_Tcutil_unify_it(_tmp2CE,_tmp2C8);Cyc_Tcutil_unify_tqual(
_tmp2CF,_tmp2CE,_tmp2C9,_tmp2C8);Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=
t2;{const char*_tmpD03;((void(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,
struct Cyc_Absyn_Conref*y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(
Cyc_Core_intcmp,_tmp2CB,_tmp2D1,((_tmpD03="(not both zero terminated)",
_tag_dyneither(_tmpD03,sizeof(char),27))));}if(_tmp2CA == _tmp2D0)return;if(
_tmp2CA == 0  || _tmp2D0 == 0)goto _LL218;if(Cyc_Evexp_same_const_exp((struct Cyc_Absyn_Exp*)
_tmp2CA,(struct Cyc_Absyn_Exp*)_tmp2D0))return;{const char*_tmpD04;Cyc_Tcutil_failure_reason=((
_tmpD04="(different array sizes)",_tag_dyneither(_tmpD04,sizeof(char),24)));}
goto _LL218;_LL237: _tmp2D2=_tmp26E.f1;if(_tmp2D2 <= (void*)4)goto _LL239;if(*((int*)
_tmp2D2)!= 8)goto _LL239;_tmp2D3=((struct Cyc_Absyn_FnType_struct*)_tmp2D2)->f1;
_tmp2D4=_tmp2D3.tvars;_tmp2D5=_tmp2D3.effect;_tmp2D6=(void*)_tmp2D3.ret_typ;
_tmp2D7=_tmp2D3.args;_tmp2D8=_tmp2D3.c_varargs;_tmp2D9=_tmp2D3.cyc_varargs;
_tmp2DA=_tmp2D3.rgn_po;_tmp2DB=_tmp2D3.attributes;_tmp2DC=_tmp26E.f2;if(_tmp2DC
<= (void*)4)goto _LL239;if(*((int*)_tmp2DC)!= 8)goto _LL239;_tmp2DD=((struct Cyc_Absyn_FnType_struct*)
_tmp2DC)->f1;_tmp2DE=_tmp2DD.tvars;_tmp2DF=_tmp2DD.effect;_tmp2E0=(void*)_tmp2DD.ret_typ;
_tmp2E1=_tmp2DD.args;_tmp2E2=_tmp2DD.c_varargs;_tmp2E3=_tmp2DD.cyc_varargs;
_tmp2E4=_tmp2DD.rgn_po;_tmp2E5=_tmp2DD.attributes;_LL238: {int done=0;struct
_RegionHandle _tmp32A=_new_region("rgn");struct _RegionHandle*rgn=& _tmp32A;
_push_region(rgn);{struct Cyc_List_List*inst=0;while(_tmp2DE != 0){if(_tmp2D4 == 0){{
const char*_tmpD05;Cyc_Tcutil_failure_reason=((_tmpD05="(second function type has too few type variables)",
_tag_dyneither(_tmpD05,sizeof(char),50)));}(int)_throw((void*)Cyc_Tcutil_Unify);}{
void*_tmp32C=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp2DE->hd);void*
_tmp32D=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp2D4->hd);if(_tmp32C != 
_tmp32D){{const char*_tmpD0B;void*_tmpD0A[3];struct Cyc_String_pa_struct _tmpD09;
struct Cyc_String_pa_struct _tmpD08;struct Cyc_String_pa_struct _tmpD07;Cyc_Tcutil_failure_reason=(
struct _dyneither_ptr)((_tmpD07.tag=0,((_tmpD07.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_kind2string(_tmp32D)),((_tmpD08.tag=0,((_tmpD08.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(_tmp32C)),((
_tmpD09.tag=0,((_tmpD09.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_tvar2string((
struct Cyc_Absyn_Tvar*)_tmp2DE->hd)),((_tmpD0A[0]=& _tmpD09,((_tmpD0A[1]=& _tmpD08,((
_tmpD0A[2]=& _tmpD07,Cyc_aprintf(((_tmpD0B="(type var %s has different kinds %s and %s)",
_tag_dyneither(_tmpD0B,sizeof(char),44))),_tag_dyneither(_tmpD0A,sizeof(void*),3))))))))))))))))))));}(
int)_throw((void*)Cyc_Tcutil_Unify);}{struct _tuple8*_tmpD15;struct Cyc_Absyn_VarType_struct
_tmpD14;struct Cyc_Absyn_VarType_struct*_tmpD13;struct Cyc_List_List*_tmpD12;inst=((
_tmpD12=_region_malloc(rgn,sizeof(*_tmpD12)),((_tmpD12->hd=((_tmpD15=
_region_malloc(rgn,sizeof(*_tmpD15)),((_tmpD15->f1=(struct Cyc_Absyn_Tvar*)
_tmp2D4->hd,((_tmpD15->f2=(void*)((_tmpD13=_cycalloc(sizeof(*_tmpD13)),((_tmpD13[
0]=((_tmpD14.tag=1,((_tmpD14.f1=(struct Cyc_Absyn_Tvar*)_tmp2DE->hd,_tmpD14)))),
_tmpD13)))),_tmpD15)))))),((_tmpD12->tl=inst,_tmpD12))))));}_tmp2DE=_tmp2DE->tl;
_tmp2D4=_tmp2D4->tl;}}if(_tmp2D4 != 0){{const char*_tmpD16;Cyc_Tcutil_failure_reason=((
_tmpD16="(second function type has too many type variables)",_tag_dyneither(
_tmpD16,sizeof(char),51)));}_npop_handler(0);goto _LL218;}if(inst != 0){{struct Cyc_Absyn_FnType_struct
_tmpD22;struct Cyc_Absyn_FnInfo _tmpD21;struct Cyc_Absyn_FnType_struct*_tmpD20;
struct Cyc_Absyn_FnType_struct _tmpD1C;struct Cyc_Absyn_FnInfo _tmpD1B;struct Cyc_Absyn_FnType_struct*
_tmpD1A;Cyc_Tcutil_unify_it((void*)((_tmpD1A=_cycalloc(sizeof(*_tmpD1A)),((
_tmpD1A[0]=((_tmpD1C.tag=8,((_tmpD1C.f1=((_tmpD1B.tvars=0,((_tmpD1B.effect=
_tmp2DF,((_tmpD1B.ret_typ=(void*)_tmp2E0,((_tmpD1B.args=_tmp2E1,((_tmpD1B.c_varargs=
_tmp2E2,((_tmpD1B.cyc_varargs=_tmp2E3,((_tmpD1B.rgn_po=_tmp2E4,((_tmpD1B.attributes=
_tmp2E5,_tmpD1B)))))))))))))))),_tmpD1C)))),_tmpD1A)))),Cyc_Tcutil_rsubstitute(
rgn,inst,(void*)((_tmpD20=_cycalloc(sizeof(*_tmpD20)),((_tmpD20[0]=((_tmpD22.tag=
8,((_tmpD22.f1=((_tmpD21.tvars=0,((_tmpD21.effect=_tmp2D5,((_tmpD21.ret_typ=(
void*)_tmp2D6,((_tmpD21.args=_tmp2D7,((_tmpD21.c_varargs=_tmp2D8,((_tmpD21.cyc_varargs=
_tmp2D9,((_tmpD21.rgn_po=_tmp2DA,((_tmpD21.attributes=_tmp2DB,_tmpD21)))))))))))))))),
_tmpD22)))),_tmpD20))))));}done=1;}}if(done){_npop_handler(0);return;}Cyc_Tcutil_unify_it(
_tmp2E0,_tmp2D6);for(0;_tmp2E1 != 0  && _tmp2D7 != 0;(_tmp2E1=_tmp2E1->tl,_tmp2D7=
_tmp2D7->tl)){struct Cyc_Absyn_Tqual _tmp33F;void*_tmp340;struct _tuple3 _tmp33E=*((
struct _tuple3*)_tmp2E1->hd);_tmp33F=_tmp33E.f2;_tmp340=_tmp33E.f3;{struct Cyc_Absyn_Tqual
_tmp342;void*_tmp343;struct _tuple3 _tmp341=*((struct _tuple3*)_tmp2D7->hd);_tmp342=
_tmp341.f2;_tmp343=_tmp341.f3;Cyc_Tcutil_unify_it(_tmp340,_tmp343);Cyc_Tcutil_unify_tqual(
_tmp33F,_tmp340,_tmp342,_tmp343);}}Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=
t2;if(_tmp2E1 != 0  || _tmp2D7 != 0){{const char*_tmpD23;Cyc_Tcutil_failure_reason=((
_tmpD23="(function types have different number of arguments)",_tag_dyneither(
_tmpD23,sizeof(char),52)));}_npop_handler(0);goto _LL218;}if(_tmp2E2 != _tmp2D8){{
const char*_tmpD24;Cyc_Tcutil_failure_reason=((_tmpD24="(only one function type takes C varargs)",
_tag_dyneither(_tmpD24,sizeof(char),41)));}_npop_handler(0);goto _LL218;}{int
bad_cyc_vararg=0;{struct _tuple11 _tmpD25;struct _tuple11 _tmp347=(_tmpD25.f1=
_tmp2E3,((_tmpD25.f2=_tmp2D9,_tmpD25)));struct Cyc_Absyn_VarargInfo*_tmp348;
struct Cyc_Absyn_VarargInfo*_tmp349;struct Cyc_Absyn_VarargInfo*_tmp34A;struct Cyc_Absyn_VarargInfo*
_tmp34B;struct Cyc_Absyn_VarargInfo*_tmp34C;struct Cyc_Absyn_VarargInfo _tmp34D;
struct Cyc_Core_Opt*_tmp34E;struct Cyc_Absyn_Tqual _tmp34F;void*_tmp350;int _tmp351;
struct Cyc_Absyn_VarargInfo*_tmp352;struct Cyc_Absyn_VarargInfo _tmp353;struct Cyc_Core_Opt*
_tmp354;struct Cyc_Absyn_Tqual _tmp355;void*_tmp356;int _tmp357;_LL259: _tmp348=
_tmp347.f1;if(_tmp348 != 0)goto _LL25B;_tmp349=_tmp347.f2;if(_tmp349 != 0)goto
_LL25B;_LL25A: goto _LL258;_LL25B: _tmp34A=_tmp347.f1;if(_tmp34A != 0)goto _LL25D;
_LL25C: goto _LL25E;_LL25D: _tmp34B=_tmp347.f2;if(_tmp34B != 0)goto _LL25F;_LL25E:
bad_cyc_vararg=1;{const char*_tmpD26;Cyc_Tcutil_failure_reason=((_tmpD26="(only one function type takes varargs)",
_tag_dyneither(_tmpD26,sizeof(char),39)));}goto _LL258;_LL25F: _tmp34C=_tmp347.f1;
if(_tmp34C == 0)goto _LL258;_tmp34D=*_tmp34C;_tmp34E=_tmp34D.name;_tmp34F=_tmp34D.tq;
_tmp350=(void*)_tmp34D.type;_tmp351=_tmp34D.inject;_tmp352=_tmp347.f2;if(_tmp352
== 0)goto _LL258;_tmp353=*_tmp352;_tmp354=_tmp353.name;_tmp355=_tmp353.tq;_tmp356=(
void*)_tmp353.type;_tmp357=_tmp353.inject;_LL260: Cyc_Tcutil_unify_it(_tmp350,
_tmp356);Cyc_Tcutil_unify_tqual(_tmp34F,_tmp350,_tmp355,_tmp356);if(_tmp351 != 
_tmp357){bad_cyc_vararg=1;{const char*_tmpD27;Cyc_Tcutil_failure_reason=((_tmpD27="(only one function type injects varargs)",
_tag_dyneither(_tmpD27,sizeof(char),41)));}}goto _LL258;_LL258:;}if(
bad_cyc_vararg){_npop_handler(0);goto _LL218;}{int bad_effect=0;{struct _tuple12
_tmpD28;struct _tuple12 _tmp35B=(_tmpD28.f1=_tmp2DF,((_tmpD28.f2=_tmp2D5,_tmpD28)));
struct Cyc_Core_Opt*_tmp35C;struct Cyc_Core_Opt*_tmp35D;struct Cyc_Core_Opt*_tmp35E;
struct Cyc_Core_Opt*_tmp35F;_LL262: _tmp35C=_tmp35B.f1;if(_tmp35C != 0)goto _LL264;
_tmp35D=_tmp35B.f2;if(_tmp35D != 0)goto _LL264;_LL263: goto _LL261;_LL264: _tmp35E=
_tmp35B.f1;if(_tmp35E != 0)goto _LL266;_LL265: goto _LL267;_LL266: _tmp35F=_tmp35B.f2;
if(_tmp35F != 0)goto _LL268;_LL267: bad_effect=1;goto _LL261;_LL268:;_LL269:
bad_effect=!Cyc_Tcutil_unify_effect((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp2DF))->v,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp2D5))->v);goto _LL261;
_LL261:;}Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;if(bad_effect){{const
char*_tmpD29;Cyc_Tcutil_failure_reason=((_tmpD29="(function type effects do not unify)",
_tag_dyneither(_tmpD29,sizeof(char),37)));}_npop_handler(0);goto _LL218;}if(!Cyc_Tcutil_same_atts(
_tmp2DB,_tmp2E5)){{const char*_tmpD2A;Cyc_Tcutil_failure_reason=((_tmpD2A="(function types have different attributes)",
_tag_dyneither(_tmpD2A,sizeof(char),43)));}_npop_handler(0);goto _LL218;}if(!Cyc_Tcutil_same_rgn_po(
_tmp2DA,_tmp2E4)){{const char*_tmpD2B;Cyc_Tcutil_failure_reason=((_tmpD2B="(function types have different region lifetime orderings)",
_tag_dyneither(_tmpD2B,sizeof(char),58)));}_npop_handler(0);goto _LL218;}
_npop_handler(0);return;}};_pop_region(rgn);}_LL239: _tmp2E6=_tmp26E.f1;if(
_tmp2E6 <= (void*)4)goto _LL23B;if(*((int*)_tmp2E6)!= 9)goto _LL23B;_tmp2E7=((
struct Cyc_Absyn_TupleType_struct*)_tmp2E6)->f1;_tmp2E8=_tmp26E.f2;if(_tmp2E8 <= (
void*)4)goto _LL23B;if(*((int*)_tmp2E8)!= 9)goto _LL23B;_tmp2E9=((struct Cyc_Absyn_TupleType_struct*)
_tmp2E8)->f1;_LL23A: for(0;_tmp2E9 != 0  && _tmp2E7 != 0;(_tmp2E9=_tmp2E9->tl,
_tmp2E7=_tmp2E7->tl)){struct Cyc_Absyn_Tqual _tmp364;void*_tmp365;struct _tuple5
_tmp363=*((struct _tuple5*)_tmp2E9->hd);_tmp364=_tmp363.f1;_tmp365=_tmp363.f2;{
struct Cyc_Absyn_Tqual _tmp367;void*_tmp368;struct _tuple5 _tmp366=*((struct _tuple5*)
_tmp2E7->hd);_tmp367=_tmp366.f1;_tmp368=_tmp366.f2;Cyc_Tcutil_unify_it(_tmp365,
_tmp368);Cyc_Tcutil_unify_tqual(_tmp364,_tmp365,_tmp367,_tmp368);}}if(_tmp2E9 == 
0  && _tmp2E7 == 0)return;Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;{const
char*_tmpD2C;Cyc_Tcutil_failure_reason=((_tmpD2C="(tuple types have different numbers of components)",
_tag_dyneither(_tmpD2C,sizeof(char),51)));}goto _LL218;_LL23B: _tmp2EA=_tmp26E.f1;
if(_tmp2EA <= (void*)4)goto _LL23D;if(*((int*)_tmp2EA)!= 11)goto _LL23D;_tmp2EB=(
void*)((struct Cyc_Absyn_AnonAggrType_struct*)_tmp2EA)->f1;_tmp2EC=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp2EA)->f2;_tmp2ED=_tmp26E.f2;if(_tmp2ED <= (void*)4)goto _LL23D;if(*((int*)
_tmp2ED)!= 11)goto _LL23D;_tmp2EE=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp2ED)->f1;_tmp2EF=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp2ED)->f2;_LL23C:
if(_tmp2EE != _tmp2EB){{const char*_tmpD2D;Cyc_Tcutil_failure_reason=((_tmpD2D="(struct and union type)",
_tag_dyneither(_tmpD2D,sizeof(char),24)));}goto _LL218;}for(0;_tmp2EF != 0  && 
_tmp2EC != 0;(_tmp2EF=_tmp2EF->tl,_tmp2EC=_tmp2EC->tl)){struct Cyc_Absyn_Aggrfield*
_tmp36B=(struct Cyc_Absyn_Aggrfield*)_tmp2EF->hd;struct Cyc_Absyn_Aggrfield*
_tmp36C=(struct Cyc_Absyn_Aggrfield*)_tmp2EC->hd;if(Cyc_strptrcmp(_tmp36B->name,
_tmp36C->name)!= 0){{const char*_tmpD2E;Cyc_Tcutil_failure_reason=((_tmpD2E="(different member names)",
_tag_dyneither(_tmpD2E,sizeof(char),25)));}(int)_throw((void*)Cyc_Tcutil_Unify);}
Cyc_Tcutil_unify_it((void*)_tmp36B->type,(void*)_tmp36C->type);Cyc_Tcutil_unify_tqual(
_tmp36B->tq,(void*)_tmp36B->type,_tmp36C->tq,(void*)_tmp36C->type);if(!Cyc_Tcutil_same_atts(
_tmp36B->attributes,_tmp36C->attributes)){Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=
t2;{const char*_tmpD2F;Cyc_Tcutil_failure_reason=((_tmpD2F="(different attributes on member)",
_tag_dyneither(_tmpD2F,sizeof(char),33)));}(int)_throw((void*)Cyc_Tcutil_Unify);}
if((_tmp36B->width != 0  && _tmp36C->width == 0  || _tmp36C->width != 0  && _tmp36B->width
== 0) || (_tmp36B->width != 0  && _tmp36C->width != 0) && !Cyc_Evexp_same_const_exp((
struct Cyc_Absyn_Exp*)_check_null(_tmp36B->width),(struct Cyc_Absyn_Exp*)
_check_null(_tmp36C->width))){Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;{
const char*_tmpD30;Cyc_Tcutil_failure_reason=((_tmpD30="(different bitfield widths on member)",
_tag_dyneither(_tmpD30,sizeof(char),38)));}(int)_throw((void*)Cyc_Tcutil_Unify);}}
if(_tmp2EF == 0  && _tmp2EC == 0)return;Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=
t2;{const char*_tmpD31;Cyc_Tcutil_failure_reason=((_tmpD31="(different number of members)",
_tag_dyneither(_tmpD31,sizeof(char),30)));}goto _LL218;_LL23D: _tmp2F0=_tmp26E.f1;
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
19)goto _LL251;_LL250: if(Cyc_Tcutil_unify_effect(t1,t2))return;{const char*_tmpD32;
Cyc_Tcutil_failure_reason=((_tmpD32="(effects don't unify)",_tag_dyneither(
_tmpD32,sizeof(char),22)));}goto _LL218;_LL251:;_LL252: goto _LL218;_LL218:;}(int)
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
_tmp376.Forward_constr).tag != 1)goto _LL271;_LL277: {const char*_tmpD35;void*
_tmpD34;(_tmpD34=0,Cyc_Tcutil_impos(((_tmpD35="unify_conref: forward after compress(2)",
_tag_dyneither(_tmpD35,sizeof(char),40))),_tag_dyneither(_tmpD34,sizeof(void*),0)));}
_LL271:;}_LL26F: if((_tmp374.Forward_constr).tag != 1)goto _LL26A;_LL270: {const
char*_tmpD38;void*_tmpD37;(_tmpD37=0,Cyc_Tcutil_impos(((_tmpD38="unify_conref: forward after compress",
_tag_dyneither(_tmpD38,sizeof(char),37))),_tag_dyneither(_tmpD37,sizeof(void*),0)));}
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
return _tmp38B;{struct _tuple0 _tmpD39;struct _tuple0 _tmp38D=(_tmpD39.f1=t2,((
_tmpD39.f2=t1,_tmpD39)));void*_tmp38E;void*_tmp38F;void*_tmp390;struct Cyc_Absyn_Tvar*
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
const char*_tmpD3C;void*_tmpD3B;(_tmpD3B=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpD3C="typecmp: can only compare closed types",
_tag_dyneither(_tmpD3C,sizeof(char),39))),_tag_dyneither(_tmpD3B,sizeof(void*),0)));}
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
char*_tmpD3F;void*_tmpD3E;(_tmpD3E=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpD3F="missing expression in array index",
_tag_dyneither(_tmpD3F,sizeof(char),34))),_tag_dyneither(_tmpD3E,sizeof(void*),0)));}
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
_tmp3FA=_tmp3F3.rgn_po;_tmp3FB=_tmp3F3.attributes;_LL2C7: {const char*_tmpD42;
void*_tmpD41;(_tmpD41=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmpD42="typecmp: function types not handled",_tag_dyneither(
_tmpD42,sizeof(char),36))),_tag_dyneither(_tmpD41,sizeof(void*),0)));}_LL2C8:
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
_LL2DF: {const char*_tmpD45;void*_tmpD44;(_tmpD44=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpD45="typecmp: effects not handled",
_tag_dyneither(_tmpD45,sizeof(char),29))),_tag_dyneither(_tmpD44,sizeof(void*),0)));}
_LL2E0:;_LL2E1: {const char*_tmpD48;void*_tmpD47;(_tmpD47=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpD48="Unmatched case in typecmp",
_tag_dyneither(_tmpD48,sizeof(char),26))),_tag_dyneither(_tmpD47,sizeof(void*),0)));}
_LL2AD:;}}}int Cyc_Tcutil_is_arithmetic_type(void*t);int Cyc_Tcutil_is_arithmetic_type(
void*t){void*_tmp43C=Cyc_Tcutil_compress(t);_LL2E8: if(_tmp43C <= (void*)4)goto
_LL2EA;if(*((int*)_tmp43C)!= 5)goto _LL2EA;_LL2E9: goto _LL2EB;_LL2EA: if((int)
_tmp43C != 1)goto _LL2EC;_LL2EB: goto _LL2ED;_LL2EC: if(_tmp43C <= (void*)4)goto _LL2F2;
if(*((int*)_tmp43C)!= 6)goto _LL2EE;_LL2ED: goto _LL2EF;_LL2EE: if(*((int*)_tmp43C)
!= 12)goto _LL2F0;_LL2EF: goto _LL2F1;_LL2F0: if(*((int*)_tmp43C)!= 13)goto _LL2F2;
_LL2F1: return 1;_LL2F2:;_LL2F3: return 0;_LL2E7:;}int Cyc_Tcutil_will_lose_precision(
void*t1,void*t2);int Cyc_Tcutil_will_lose_precision(void*t1,void*t2){t1=Cyc_Tcutil_compress(
t1);t2=Cyc_Tcutil_compress(t2);{struct _tuple0 _tmpD49;struct _tuple0 _tmp43E=(
_tmpD49.f1=t1,((_tmpD49.f2=t2,_tmpD49)));void*_tmp43F;int _tmp440;void*_tmp441;
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
struct Cyc_Tcenv_Tenv*te,void*t,struct Cyc_List_List*es){struct _RegionHandle*
_tmp47B=Cyc_Tcenv_get_fnrgn(te);{struct Cyc_Core_Opt*max_arith_type=0;{struct Cyc_List_List*
el=es;for(0;el != 0;el=el->tl){void*t1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt))->v);if(Cyc_Tcutil_is_arithmetic_type(
t1)){if(max_arith_type == 0  || Cyc_Tcutil_will_lose_precision(t1,(void*)
max_arith_type->v)){struct Cyc_Core_Opt*_tmpD4A;max_arith_type=((_tmpD4A=
_region_malloc(_tmp47B,sizeof(*_tmpD4A)),((_tmpD4A->v=(void*)t1,_tmpD4A))));}}}}
if(max_arith_type != 0){if(!Cyc_Tcutil_unify(t,(void*)max_arith_type->v))return 0;}}{
struct Cyc_List_List*el=es;for(0;el != 0;el=el->tl){if(!Cyc_Tcutil_coerce_assign(
te,(struct Cyc_Absyn_Exp*)el->hd,t)){{const char*_tmpD4F;void*_tmpD4E[2];struct Cyc_String_pa_struct
_tmpD4D;struct Cyc_String_pa_struct _tmpD4C;(_tmpD4C.tag=0,((_tmpD4C.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt))->v)),((_tmpD4D.tag=0,((
_tmpD4D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((
_tmpD4E[0]=& _tmpD4D,((_tmpD4E[1]=& _tmpD4C,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)
el->hd)->loc,((_tmpD4F="type mismatch: expecting %s but found %s",_tag_dyneither(
_tmpD4F,sizeof(char),41))),_tag_dyneither(_tmpD4E,sizeof(void*),2)))))))))))));}
return 0;}}}return 1;}int Cyc_Tcutil_coerce_to_bool(struct Cyc_Tcenv_Tenv*te,struct
Cyc_Absyn_Exp*e);int Cyc_Tcutil_coerce_to_bool(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*
e){if(!Cyc_Tcutil_coerce_sint_typ(te,e)){void*_tmp481=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v);_LL31E: if(_tmp481 <= (void*)4)goto
_LL320;if(*((int*)_tmp481)!= 4)goto _LL320;_LL31F: Cyc_Tcutil_unchecked_cast(te,e,
Cyc_Absyn_uint_typ,(void*)3);goto _LL31D;_LL320:;_LL321: return 0;_LL31D:;}return 1;}
int Cyc_Tcutil_is_integral_type(void*t);int Cyc_Tcutil_is_integral_type(void*t){
void*_tmp482=Cyc_Tcutil_compress(t);_LL323: if(_tmp482 <= (void*)4)goto _LL32B;if(*((
int*)_tmp482)!= 5)goto _LL325;_LL324: goto _LL326;_LL325: if(*((int*)_tmp482)!= 18)
goto _LL327;_LL326: goto _LL328;_LL327: if(*((int*)_tmp482)!= 12)goto _LL329;_LL328:
goto _LL32A;_LL329: if(*((int*)_tmp482)!= 13)goto _LL32B;_LL32A: return 1;_LL32B:;
_LL32C: return 0;_LL322:;}int Cyc_Tcutil_coerce_uint_typ(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Absyn_Exp*e);int Cyc_Tcutil_coerce_uint_typ(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Absyn_Exp*e){if(Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,Cyc_Absyn_uint_typ))return 1;if(Cyc_Tcutil_is_integral_type((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)){if(Cyc_Tcutil_will_lose_precision((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_Absyn_uint_typ)){const
char*_tmpD52;void*_tmpD51;(_tmpD51=0,Cyc_Tcutil_warn(e->loc,((_tmpD52="integral size mismatch; conversion supplied",
_tag_dyneither(_tmpD52,sizeof(char),44))),_tag_dyneither(_tmpD51,sizeof(void*),0)));}
Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_uint_typ,(void*)1);return 1;}return 0;}
int Cyc_Tcutil_coerce_sint_typ(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e);int
Cyc_Tcutil_coerce_sint_typ(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){if(Cyc_Tcutil_unify((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_Absyn_sint_typ))return 1;
if(Cyc_Tcutil_is_integral_type((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)){
if(Cyc_Tcutil_will_lose_precision((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,
Cyc_Absyn_sint_typ)){const char*_tmpD55;void*_tmpD54;(_tmpD54=0,Cyc_Tcutil_warn(e->loc,((
_tmpD55="integral size mismatch; conversion supplied",_tag_dyneither(_tmpD55,
sizeof(char),44))),_tag_dyneither(_tmpD54,sizeof(void*),0)));}Cyc_Tcutil_unchecked_cast(
te,e,Cyc_Absyn_sint_typ,(void*)1);return 1;}return 0;}struct _tuple13{union Cyc_Absyn_Constraint_union
f1;union Cyc_Absyn_Constraint_union f2;};int Cyc_Tcutil_silent_castable(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void*t1,void*t2);int Cyc_Tcutil_silent_castable(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void*t1,void*t2){t1=Cyc_Tcutil_compress(
t1);t2=Cyc_Tcutil_compress(t2);{struct _tuple0 _tmpD56;struct _tuple0 _tmp488=(
_tmpD56.f1=t1,((_tmpD56.f2=t2,_tmpD56)));void*_tmp489;struct Cyc_Absyn_PtrInfo
_tmp48A;void*_tmp48B;struct Cyc_Absyn_PtrInfo _tmp48C;void*_tmp48D;struct Cyc_Absyn_TunionInfo
_tmp48E;union Cyc_Absyn_TunionInfoU_union _tmp48F;struct Cyc_Absyn_Tuniondecl**
_tmp490;struct Cyc_Absyn_Tuniondecl*_tmp491;struct Cyc_List_List*_tmp492;struct Cyc_Core_Opt*
_tmp493;struct Cyc_Core_Opt _tmp494;void*_tmp495;void*_tmp496;struct Cyc_Absyn_TunionInfo
_tmp497;union Cyc_Absyn_TunionInfoU_union _tmp498;struct Cyc_Absyn_Tuniondecl**
_tmp499;struct Cyc_Absyn_Tuniondecl*_tmp49A;struct Cyc_List_List*_tmp49B;struct Cyc_Core_Opt*
_tmp49C;struct Cyc_Core_Opt _tmp49D;void*_tmp49E;void*_tmp49F;struct Cyc_Absyn_ArrayInfo
_tmp4A0;void*_tmp4A1;struct Cyc_Absyn_Tqual _tmp4A2;struct Cyc_Absyn_Exp*_tmp4A3;
struct Cyc_Absyn_Conref*_tmp4A4;void*_tmp4A5;struct Cyc_Absyn_ArrayInfo _tmp4A6;
void*_tmp4A7;struct Cyc_Absyn_Tqual _tmp4A8;struct Cyc_Absyn_Exp*_tmp4A9;struct Cyc_Absyn_Conref*
_tmp4AA;void*_tmp4AB;struct Cyc_Absyn_TunionFieldInfo _tmp4AC;union Cyc_Absyn_TunionFieldInfoU_union
_tmp4AD;struct Cyc_Absyn_Tuniondecl*_tmp4AE;struct Cyc_Absyn_Tunionfield*_tmp4AF;
struct Cyc_List_List*_tmp4B0;void*_tmp4B1;struct Cyc_Absyn_TunionInfo _tmp4B2;union
Cyc_Absyn_TunionInfoU_union _tmp4B3;struct Cyc_Absyn_Tuniondecl**_tmp4B4;struct Cyc_Absyn_Tuniondecl*
_tmp4B5;struct Cyc_List_List*_tmp4B6;void*_tmp4B7;struct Cyc_Absyn_PtrInfo _tmp4B8;
void*_tmp4B9;struct Cyc_Absyn_Tqual _tmp4BA;struct Cyc_Absyn_PtrAtts _tmp4BB;void*
_tmp4BC;struct Cyc_Absyn_Conref*_tmp4BD;struct Cyc_Absyn_Conref*_tmp4BE;struct Cyc_Absyn_Conref*
_tmp4BF;void*_tmp4C0;struct Cyc_Absyn_TunionInfo _tmp4C1;union Cyc_Absyn_TunionInfoU_union
_tmp4C2;struct Cyc_Absyn_Tuniondecl**_tmp4C3;struct Cyc_Absyn_Tuniondecl*_tmp4C4;
struct Cyc_List_List*_tmp4C5;struct Cyc_Core_Opt*_tmp4C6;void*_tmp4C7;void*_tmp4C8;
_LL32E: _tmp489=_tmp488.f1;if(_tmp489 <= (void*)4)goto _LL330;if(*((int*)_tmp489)!= 
4)goto _LL330;_tmp48A=((struct Cyc_Absyn_PointerType_struct*)_tmp489)->f1;_tmp48B=
_tmp488.f2;if(_tmp48B <= (void*)4)goto _LL330;if(*((int*)_tmp48B)!= 4)goto _LL330;
_tmp48C=((struct Cyc_Absyn_PointerType_struct*)_tmp48B)->f1;_LL32F: {int okay=1;
if(!((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*
y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,(_tmp48A.ptr_atts).nullable,(
_tmp48C.ptr_atts).nullable)){union Cyc_Absyn_Constraint_union _tmp4C9=(((struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref*x))Cyc_Absyn_compress_conref)((_tmp48A.ptr_atts).nullable))->v;
int _tmp4CA;_LL33D: if((_tmp4C9.Eq_constr).tag != 0)goto _LL33F;_tmp4CA=(_tmp4C9.Eq_constr).f1;
_LL33E: okay=!_tmp4CA;goto _LL33C;_LL33F:;_LL340: {const char*_tmpD59;void*_tmpD58;(
_tmpD58=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmpD59="silent_castable conref not eq",_tag_dyneither(_tmpD59,sizeof(char),30))),
_tag_dyneither(_tmpD58,sizeof(void*),0)));}_LL33C:;}if(!Cyc_Tcutil_unify_conrefs(
Cyc_Tcutil_unify_it_bounds,(_tmp48A.ptr_atts).bounds,(_tmp48C.ptr_atts).bounds)){
struct _tuple13 _tmpD5A;struct _tuple13 _tmp4CE=(_tmpD5A.f1=(Cyc_Absyn_compress_conref((
_tmp48A.ptr_atts).bounds))->v,((_tmpD5A.f2=(Cyc_Absyn_compress_conref((_tmp48C.ptr_atts).bounds))->v,
_tmpD5A)));union Cyc_Absyn_Constraint_union _tmp4CF;void*_tmp4D0;union Cyc_Absyn_Constraint_union
_tmp4D1;void*_tmp4D2;union Cyc_Absyn_Constraint_union _tmp4D3;_LL342: _tmp4CF=
_tmp4CE.f1;if(((_tmp4CE.f1).Eq_constr).tag != 0)goto _LL344;_tmp4D0=(_tmp4CF.Eq_constr).f1;
_tmp4D1=_tmp4CE.f2;if(((_tmp4CE.f2).Eq_constr).tag != 0)goto _LL344;_tmp4D2=(
_tmp4D1.Eq_constr).f1;_LL343:{struct _tuple0 _tmpD5B;struct _tuple0 _tmp4D5=(_tmpD5B.f1=
_tmp4D0,((_tmpD5B.f2=_tmp4D2,_tmpD5B)));void*_tmp4D6;void*_tmp4D7;void*_tmp4D8;
void*_tmp4D9;void*_tmp4DA;struct Cyc_Absyn_Exp*_tmp4DB;void*_tmp4DC;struct Cyc_Absyn_Exp*
_tmp4DD;void*_tmp4DE;void*_tmp4DF;struct Cyc_Absyn_Exp*_tmp4E0;_LL349: _tmp4D6=
_tmp4D5.f1;if(_tmp4D6 <= (void*)1)goto _LL34B;if(*((int*)_tmp4D6)!= 0)goto _LL34B;
_tmp4D7=_tmp4D5.f2;if((int)_tmp4D7 != 0)goto _LL34B;_LL34A: goto _LL34C;_LL34B:
_tmp4D8=_tmp4D5.f1;if((int)_tmp4D8 != 0)goto _LL34D;_tmp4D9=_tmp4D5.f2;if((int)
_tmp4D9 != 0)goto _LL34D;_LL34C: okay=1;goto _LL348;_LL34D: _tmp4DA=_tmp4D5.f1;if(
_tmp4DA <= (void*)1)goto _LL34F;if(*((int*)_tmp4DA)!= 0)goto _LL34F;_tmp4DB=((
struct Cyc_Absyn_Upper_b_struct*)_tmp4DA)->f1;_tmp4DC=_tmp4D5.f2;if(_tmp4DC <= (
void*)1)goto _LL34F;if(*((int*)_tmp4DC)!= 0)goto _LL34F;_tmp4DD=((struct Cyc_Absyn_Upper_b_struct*)
_tmp4DC)->f1;_LL34E: okay=okay  && Cyc_Evexp_lte_const_exp(_tmp4DD,_tmp4DB);if(!((
int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,(_tmp48C.ptr_atts).zero_term)){
const char*_tmpD5E;void*_tmpD5D;(_tmpD5D=0,Cyc_Tcutil_warn(loc,((_tmpD5E="implicit cast to shorter array",
_tag_dyneither(_tmpD5E,sizeof(char),31))),_tag_dyneither(_tmpD5D,sizeof(void*),0)));}
goto _LL348;_LL34F: _tmp4DE=_tmp4D5.f1;if((int)_tmp4DE != 0)goto _LL348;_tmp4DF=
_tmp4D5.f2;if(_tmp4DF <= (void*)1)goto _LL348;if(*((int*)_tmp4DF)!= 0)goto _LL348;
_tmp4E0=((struct Cyc_Absyn_Upper_b_struct*)_tmp4DF)->f1;_LL350: if(((int(*)(int,
struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,(_tmp48A.ptr_atts).zero_term)
 && Cyc_Tcutil_is_bound_one((_tmp48C.ptr_atts).bounds))goto _LL348;okay=0;goto
_LL348;_LL348:;}goto _LL341;_LL344: _tmp4D3=_tmp4CE.f1;if(((_tmp4CE.f1).No_constr).tag
!= 2)goto _LL346;_LL345: okay=0;goto _LL341;_LL346:;_LL347: {const char*_tmpD61;void*
_tmpD60;(_tmpD60=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmpD61="silent_castable conrefs didn't unify",_tag_dyneither(_tmpD61,sizeof(
char),37))),_tag_dyneither(_tmpD60,sizeof(void*),0)));}_LL341:;}okay=okay  && Cyc_Tcutil_unify((
void*)_tmp48A.elt_typ,(void*)_tmp48C.elt_typ);okay=okay  && (Cyc_Tcutil_unify((
void*)(_tmp48A.ptr_atts).rgn,(void*)(_tmp48C.ptr_atts).rgn) || Cyc_Tcenv_region_outlives(
te,(void*)(_tmp48A.ptr_atts).rgn,(void*)(_tmp48C.ptr_atts).rgn));okay=okay  && (!(
_tmp48A.elt_tq).real_const  || (_tmp48C.elt_tq).real_const);okay=okay  && (((int(*)(
int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(
Cyc_Core_intcmp,(_tmp48A.ptr_atts).zero_term,(_tmp48C.ptr_atts).zero_term) || ((
int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(1,(_tmp48A.ptr_atts).zero_term)
 && (_tmp48C.elt_tq).real_const);return okay;}_LL330: _tmp48D=_tmp488.f1;if(
_tmp48D <= (void*)4)goto _LL332;if(*((int*)_tmp48D)!= 2)goto _LL332;_tmp48E=((
struct Cyc_Absyn_TunionType_struct*)_tmp48D)->f1;_tmp48F=_tmp48E.tunion_info;if((((((
struct Cyc_Absyn_TunionType_struct*)_tmp48D)->f1).tunion_info).KnownTunion).tag != 
1)goto _LL332;_tmp490=(_tmp48F.KnownTunion).f1;_tmp491=*_tmp490;_tmp492=_tmp48E.targs;
_tmp493=_tmp48E.rgn;if(_tmp493 == 0)goto _LL332;_tmp494=*_tmp493;_tmp495=(void*)
_tmp494.v;_tmp496=_tmp488.f2;if(_tmp496 <= (void*)4)goto _LL332;if(*((int*)_tmp496)
!= 2)goto _LL332;_tmp497=((struct Cyc_Absyn_TunionType_struct*)_tmp496)->f1;
_tmp498=_tmp497.tunion_info;if((((((struct Cyc_Absyn_TunionType_struct*)_tmp496)->f1).tunion_info).KnownTunion).tag
!= 1)goto _LL332;_tmp499=(_tmp498.KnownTunion).f1;_tmp49A=*_tmp499;_tmp49B=
_tmp497.targs;_tmp49C=_tmp497.rgn;if(_tmp49C == 0)goto _LL332;_tmp49D=*_tmp49C;
_tmp49E=(void*)_tmp49D.v;_LL331: if(_tmp491 != _tmp49A  || !Cyc_Tcenv_region_outlives(
te,_tmp495,_tmp49E))return 0;for(0;_tmp492 != 0  && _tmp49B != 0;(_tmp492=_tmp492->tl,
_tmp49B=_tmp49B->tl)){if(!Cyc_Tcutil_unify((void*)_tmp492->hd,(void*)_tmp49B->hd))
return 0;}if(_tmp492 != 0  || _tmp49B != 0)return 0;return 1;_LL332: _tmp49F=_tmp488.f1;
if(_tmp49F <= (void*)4)goto _LL334;if(*((int*)_tmp49F)!= 7)goto _LL334;_tmp4A0=((
struct Cyc_Absyn_ArrayType_struct*)_tmp49F)->f1;_tmp4A1=(void*)_tmp4A0.elt_type;
_tmp4A2=_tmp4A0.tq;_tmp4A3=_tmp4A0.num_elts;_tmp4A4=_tmp4A0.zero_term;_tmp4A5=
_tmp488.f2;if(_tmp4A5 <= (void*)4)goto _LL334;if(*((int*)_tmp4A5)!= 7)goto _LL334;
_tmp4A6=((struct Cyc_Absyn_ArrayType_struct*)_tmp4A5)->f1;_tmp4A7=(void*)_tmp4A6.elt_type;
_tmp4A8=_tmp4A6.tq;_tmp4A9=_tmp4A6.num_elts;_tmp4AA=_tmp4A6.zero_term;_LL333: {
int okay;okay=((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*
y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp4A4,_tmp4AA) && ((_tmp4A3 != 0
 && _tmp4A9 != 0) && Cyc_Evexp_same_const_exp((struct Cyc_Absyn_Exp*)_tmp4A3,(
struct Cyc_Absyn_Exp*)_tmp4A9));return(okay  && Cyc_Tcutil_unify(_tmp4A1,_tmp4A7))
 && (!_tmp4A2.real_const  || _tmp4A8.real_const);}_LL334: _tmp4AB=_tmp488.f1;if(
_tmp4AB <= (void*)4)goto _LL336;if(*((int*)_tmp4AB)!= 3)goto _LL336;_tmp4AC=((
struct Cyc_Absyn_TunionFieldType_struct*)_tmp4AB)->f1;_tmp4AD=_tmp4AC.field_info;
if((((((struct Cyc_Absyn_TunionFieldType_struct*)_tmp4AB)->f1).field_info).KnownTunionfield).tag
!= 1)goto _LL336;_tmp4AE=(_tmp4AD.KnownTunionfield).f1;_tmp4AF=(_tmp4AD.KnownTunionfield).f2;
_tmp4B0=_tmp4AC.targs;_tmp4B1=_tmp488.f2;if(_tmp4B1 <= (void*)4)goto _LL336;if(*((
int*)_tmp4B1)!= 2)goto _LL336;_tmp4B2=((struct Cyc_Absyn_TunionType_struct*)
_tmp4B1)->f1;_tmp4B3=_tmp4B2.tunion_info;if((((((struct Cyc_Absyn_TunionType_struct*)
_tmp4B1)->f1).tunion_info).KnownTunion).tag != 1)goto _LL336;_tmp4B4=(_tmp4B3.KnownTunion).f1;
_tmp4B5=*_tmp4B4;_tmp4B6=_tmp4B2.targs;_LL335: if((_tmp4AE == _tmp4B5  || Cyc_Absyn_qvar_cmp(
_tmp4AE->name,_tmp4B5->name)== 0) && (_tmp4AF->typs == 0  || _tmp4AE->is_flat)){
for(0;_tmp4B0 != 0  && _tmp4B6 != 0;(_tmp4B0=_tmp4B0->tl,_tmp4B6=_tmp4B6->tl)){if(!
Cyc_Tcutil_unify((void*)_tmp4B0->hd,(void*)_tmp4B6->hd))break;}if(_tmp4B0 == 0
 && _tmp4B6 == 0)return 1;}return 0;_LL336: _tmp4B7=_tmp488.f1;if(_tmp4B7 <= (void*)4)
goto _LL338;if(*((int*)_tmp4B7)!= 4)goto _LL338;_tmp4B8=((struct Cyc_Absyn_PointerType_struct*)
_tmp4B7)->f1;_tmp4B9=(void*)_tmp4B8.elt_typ;_tmp4BA=_tmp4B8.elt_tq;_tmp4BB=
_tmp4B8.ptr_atts;_tmp4BC=(void*)_tmp4BB.rgn;_tmp4BD=_tmp4BB.nullable;_tmp4BE=
_tmp4BB.bounds;_tmp4BF=_tmp4BB.zero_term;_tmp4C0=_tmp488.f2;if(_tmp4C0 <= (void*)
4)goto _LL338;if(*((int*)_tmp4C0)!= 2)goto _LL338;_tmp4C1=((struct Cyc_Absyn_TunionType_struct*)
_tmp4C0)->f1;_tmp4C2=_tmp4C1.tunion_info;if((((((struct Cyc_Absyn_TunionType_struct*)
_tmp4C0)->f1).tunion_info).KnownTunion).tag != 1)goto _LL338;_tmp4C3=(_tmp4C2.KnownTunion).f1;
_tmp4C4=*_tmp4C3;_tmp4C5=_tmp4C1.targs;_tmp4C6=_tmp4C1.rgn;if(!(!_tmp4C4->is_flat))
goto _LL338;_LL337:{void*_tmp4E5=Cyc_Tcutil_compress(_tmp4B9);struct Cyc_Absyn_TunionFieldInfo
_tmp4E6;union Cyc_Absyn_TunionFieldInfoU_union _tmp4E7;struct Cyc_Absyn_Tuniondecl*
_tmp4E8;struct Cyc_Absyn_Tunionfield*_tmp4E9;struct Cyc_List_List*_tmp4EA;_LL352:
if(_tmp4E5 <= (void*)4)goto _LL354;if(*((int*)_tmp4E5)!= 3)goto _LL354;_tmp4E6=((
struct Cyc_Absyn_TunionFieldType_struct*)_tmp4E5)->f1;_tmp4E7=_tmp4E6.field_info;
if((((((struct Cyc_Absyn_TunionFieldType_struct*)_tmp4E5)->f1).field_info).KnownTunionfield).tag
!= 1)goto _LL354;_tmp4E8=(_tmp4E7.KnownTunionfield).f1;_tmp4E9=(_tmp4E7.KnownTunionfield).f2;
_tmp4EA=_tmp4E6.targs;_LL353: if(!Cyc_Tcutil_unify(_tmp4BC,(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp4C6))->v) && !Cyc_Tcenv_region_outlives(te,_tmp4BC,(void*)
_tmp4C6->v))return 0;if(!((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,
struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp4BD,Cyc_Absyn_false_conref))
return 0;if(!Cyc_Tcutil_unify_conrefs(Cyc_Tcutil_unify_it_bounds,_tmp4BE,Cyc_Absyn_bounds_one_conref))
return 0;if(!((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*
y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp4BF,Cyc_Absyn_false_conref))
return 0;if(Cyc_Absyn_qvar_cmp(_tmp4C4->name,_tmp4E8->name)== 0  && _tmp4E9->typs
!= 0){int okay=1;for(0;_tmp4EA != 0  && _tmp4C5 != 0;(_tmp4EA=_tmp4EA->tl,_tmp4C5=
_tmp4C5->tl)){if(!Cyc_Tcutil_unify((void*)_tmp4EA->hd,(void*)_tmp4C5->hd)){okay=
0;break;}}if((!okay  || _tmp4EA != 0) || _tmp4C5 != 0)return 0;return 1;}goto _LL351;
_LL354:;_LL355: goto _LL351;_LL351:;}return 0;_LL338: _tmp4C7=_tmp488.f1;if(_tmp4C7
<= (void*)4)goto _LL33A;if(*((int*)_tmp4C7)!= 18)goto _LL33A;_tmp4C8=_tmp488.f2;
if(_tmp4C8 <= (void*)4)goto _LL33A;if(*((int*)_tmp4C8)!= 5)goto _LL33A;_LL339:
return 0;_LL33A:;_LL33B: return Cyc_Tcutil_unify(t1,t2);_LL32D:;}}int Cyc_Tcutil_is_pointer_type(
void*t);int Cyc_Tcutil_is_pointer_type(void*t){void*_tmp4EB=Cyc_Tcutil_compress(t);
_LL357: if(_tmp4EB <= (void*)4)goto _LL359;if(*((int*)_tmp4EB)!= 4)goto _LL359;
_LL358: return 1;_LL359:;_LL35A: return 0;_LL356:;}int Cyc_Tcutil_rgn_of_pointer(void*
t,void**rgn);int Cyc_Tcutil_rgn_of_pointer(void*t,void**rgn){void*_tmp4EC=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_PtrInfo _tmp4ED;struct Cyc_Absyn_PtrAtts _tmp4EE;void*_tmp4EF;
_LL35C: if(_tmp4EC <= (void*)4)goto _LL35E;if(*((int*)_tmp4EC)!= 4)goto _LL35E;
_tmp4ED=((struct Cyc_Absyn_PointerType_struct*)_tmp4EC)->f1;_tmp4EE=_tmp4ED.ptr_atts;
_tmp4EF=(void*)_tmp4EE.rgn;_LL35D:*rgn=_tmp4EF;return 1;_LL35E:;_LL35F: return 0;
_LL35B:;}int Cyc_Tcutil_is_pointer_or_boxed(void*t,int*is_dyneither_ptr);int Cyc_Tcutil_is_pointer_or_boxed(
void*t,int*is_dyneither_ptr){void*_tmp4F0=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo
_tmp4F1;struct Cyc_Absyn_PtrAtts _tmp4F2;struct Cyc_Absyn_Conref*_tmp4F3;_LL361: if(
_tmp4F0 <= (void*)4)goto _LL363;if(*((int*)_tmp4F0)!= 4)goto _LL363;_tmp4F1=((
struct Cyc_Absyn_PointerType_struct*)_tmp4F0)->f1;_tmp4F2=_tmp4F1.ptr_atts;
_tmp4F3=_tmp4F2.bounds;_LL362:*is_dyneither_ptr=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,
_tmp4F3)== (void*)0;return 1;_LL363:;_LL364: return Cyc_Tcutil_typ_kind(t)== (void*)
2;_LL360:;}int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*e);int Cyc_Tcutil_is_zero(
struct Cyc_Absyn_Exp*e){void*_tmp4F4=(void*)e->r;union Cyc_Absyn_Cnst_union _tmp4F5;
int _tmp4F6;union Cyc_Absyn_Cnst_union _tmp4F7;char _tmp4F8;union Cyc_Absyn_Cnst_union
_tmp4F9;short _tmp4FA;union Cyc_Absyn_Cnst_union _tmp4FB;long long _tmp4FC;void*
_tmp4FD;struct Cyc_Absyn_Exp*_tmp4FE;_LL366: if(*((int*)_tmp4F4)!= 0)goto _LL368;
_tmp4F5=((struct Cyc_Absyn_Const_e_struct*)_tmp4F4)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)
_tmp4F4)->f1).Int_c).tag != 2)goto _LL368;_tmp4F6=(_tmp4F5.Int_c).f2;if(_tmp4F6 != 
0)goto _LL368;_LL367: goto _LL369;_LL368: if(*((int*)_tmp4F4)!= 0)goto _LL36A;_tmp4F7=((
struct Cyc_Absyn_Const_e_struct*)_tmp4F4)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)
_tmp4F4)->f1).Char_c).tag != 0)goto _LL36A;_tmp4F8=(_tmp4F7.Char_c).f2;if(_tmp4F8
!= 0)goto _LL36A;_LL369: goto _LL36B;_LL36A: if(*((int*)_tmp4F4)!= 0)goto _LL36C;
_tmp4F9=((struct Cyc_Absyn_Const_e_struct*)_tmp4F4)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)
_tmp4F4)->f1).Short_c).tag != 1)goto _LL36C;_tmp4FA=(_tmp4F9.Short_c).f2;if(
_tmp4FA != 0)goto _LL36C;_LL36B: goto _LL36D;_LL36C: if(*((int*)_tmp4F4)!= 0)goto
_LL36E;_tmp4FB=((struct Cyc_Absyn_Const_e_struct*)_tmp4F4)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)
_tmp4F4)->f1).LongLong_c).tag != 3)goto _LL36E;_tmp4FC=(_tmp4FB.LongLong_c).f2;if(
_tmp4FC != 0)goto _LL36E;_LL36D: return 1;_LL36E: if(*((int*)_tmp4F4)!= 15)goto _LL370;
_tmp4FD=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp4F4)->f1;_tmp4FE=((struct Cyc_Absyn_Cast_e_struct*)
_tmp4F4)->f2;_LL36F: return Cyc_Tcutil_is_zero(_tmp4FE) && Cyc_Tcutil_admits_zero(
_tmp4FD);_LL370:;_LL371: return 0;_LL365:;}struct Cyc_Core_Opt Cyc_Tcutil_trk={(void*)((
void*)5)};struct Cyc_Core_Opt Cyc_Tcutil_urk={(void*)((void*)4)};struct Cyc_Core_Opt
Cyc_Tcutil_rk={(void*)((void*)3)};struct Cyc_Core_Opt Cyc_Tcutil_ak={(void*)((void*)
0)};struct Cyc_Core_Opt Cyc_Tcutil_bk={(void*)((void*)2)};struct Cyc_Core_Opt Cyc_Tcutil_mk={(
void*)((void*)1)};struct Cyc_Core_Opt Cyc_Tcutil_ik={(void*)((void*)7)};struct Cyc_Core_Opt
Cyc_Tcutil_ek={(void*)((void*)6)};struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_opt(void*
k);struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_opt(void*k){void*_tmp4FF=k;_LL373: if((
int)_tmp4FF != 0)goto _LL375;_LL374: return(struct Cyc_Core_Opt*)& Cyc_Tcutil_ak;
_LL375: if((int)_tmp4FF != 1)goto _LL377;_LL376: return(struct Cyc_Core_Opt*)& Cyc_Tcutil_mk;
_LL377: if((int)_tmp4FF != 2)goto _LL379;_LL378: return(struct Cyc_Core_Opt*)& Cyc_Tcutil_bk;
_LL379: if((int)_tmp4FF != 3)goto _LL37B;_LL37A: return(struct Cyc_Core_Opt*)& Cyc_Tcutil_rk;
_LL37B: if((int)_tmp4FF != 4)goto _LL37D;_LL37C: return(struct Cyc_Core_Opt*)& Cyc_Tcutil_urk;
_LL37D: if((int)_tmp4FF != 5)goto _LL37F;_LL37E: return(struct Cyc_Core_Opt*)& Cyc_Tcutil_trk;
_LL37F: if((int)_tmp4FF != 6)goto _LL381;_LL380: return(struct Cyc_Core_Opt*)& Cyc_Tcutil_ek;
_LL381: if((int)_tmp4FF != 7)goto _LL372;_LL382: return(struct Cyc_Core_Opt*)& Cyc_Tcutil_ik;
_LL372:;}static void**Cyc_Tcutil_kind_to_b(void*k);static void**Cyc_Tcutil_kind_to_b(
void*k){static struct Cyc_Absyn_Eq_kb_struct ab_v={0,(void*)((void*)0)};static
struct Cyc_Absyn_Eq_kb_struct mb_v={0,(void*)((void*)1)};static struct Cyc_Absyn_Eq_kb_struct
bb_v={0,(void*)((void*)2)};static struct Cyc_Absyn_Eq_kb_struct rb_v={0,(void*)((
void*)3)};static struct Cyc_Absyn_Eq_kb_struct ub_v={0,(void*)((void*)4)};static
struct Cyc_Absyn_Eq_kb_struct tb_v={0,(void*)((void*)5)};static struct Cyc_Absyn_Eq_kb_struct
eb_v={0,(void*)((void*)6)};static struct Cyc_Absyn_Eq_kb_struct ib_v={0,(void*)((
void*)7)};static void*ab=(void*)& ab_v;static void*mb=(void*)& mb_v;static void*bb=(
void*)& bb_v;static void*rb=(void*)& rb_v;static void*ub=(void*)& ub_v;static void*tb=(
void*)& tb_v;static void*eb=(void*)& eb_v;static void*ib=(void*)& ib_v;void*_tmp500=k;
_LL384: if((int)_tmp500 != 0)goto _LL386;_LL385: return& ab;_LL386: if((int)_tmp500 != 
1)goto _LL388;_LL387: return& mb;_LL388: if((int)_tmp500 != 2)goto _LL38A;_LL389:
return& bb;_LL38A: if((int)_tmp500 != 3)goto _LL38C;_LL38B: return& rb;_LL38C: if((int)
_tmp500 != 4)goto _LL38E;_LL38D: return& ub;_LL38E: if((int)_tmp500 != 5)goto _LL390;
_LL38F: return& tb;_LL390: if((int)_tmp500 != 6)goto _LL392;_LL391: return& eb;_LL392:
if((int)_tmp500 != 7)goto _LL383;_LL393: return& ib;_LL383:;}void*Cyc_Tcutil_kind_to_bound(
void*k);void*Cyc_Tcutil_kind_to_bound(void*k){return*Cyc_Tcutil_kind_to_b(k);}
struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_bound_opt(void*k);struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_bound_opt(
void*k){return(struct Cyc_Core_Opt*)Cyc_Tcutil_kind_to_b(k);}int Cyc_Tcutil_zero_to_null(
struct Cyc_Tcenv_Tenv*te,void*t2,struct Cyc_Absyn_Exp*e1);int Cyc_Tcutil_zero_to_null(
struct Cyc_Tcenv_Tenv*te,void*t2,struct Cyc_Absyn_Exp*e1){if(Cyc_Tcutil_is_pointer_type(
t2) && Cyc_Tcutil_is_zero(e1)){{struct Cyc_Absyn_Const_e_struct _tmpD67;union Cyc_Absyn_Cnst_union
_tmpD66;struct Cyc_Absyn_Const_e_struct*_tmpD65;(void*)(e1->r=(void*)((void*)((
_tmpD65=_cycalloc(sizeof(*_tmpD65)),((_tmpD65[0]=((_tmpD67.tag=0,((_tmpD67.f1=(
union Cyc_Absyn_Cnst_union)(((_tmpD66.Null_c).tag=6,_tmpD66)),_tmpD67)))),_tmpD65))))));}{
struct Cyc_Core_Opt*_tmp50C=Cyc_Tcenv_lookup_opt_type_vars(te);struct Cyc_Absyn_PointerType_struct
_tmpD71;struct Cyc_Absyn_PtrAtts _tmpD70;struct Cyc_Absyn_PtrInfo _tmpD6F;struct Cyc_Absyn_PointerType_struct*
_tmpD6E;struct Cyc_Absyn_PointerType_struct*_tmp50D=(_tmpD6E=_cycalloc(sizeof(*
_tmpD6E)),((_tmpD6E[0]=((_tmpD71.tag=4,((_tmpD71.f1=((_tmpD6F.elt_typ=(void*)Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_ak,_tmp50C),((_tmpD6F.elt_tq=Cyc_Absyn_empty_tqual(
0),((_tmpD6F.ptr_atts=((_tmpD70.rgn=(void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_trk,_tmp50C),((_tmpD70.nullable=Cyc_Absyn_true_conref,((_tmpD70.bounds=
Cyc_Absyn_empty_conref(),((_tmpD70.zero_term=((struct Cyc_Absyn_Conref*(*)())Cyc_Absyn_empty_conref)(),((
_tmpD70.ptrloc=0,_tmpD70)))))))))),_tmpD6F)))))),_tmpD71)))),_tmpD6E)));(void*)(((
struct Cyc_Core_Opt*)_check_null(e1->topt))->v=(void*)((void*)_tmp50D));return Cyc_Tcutil_coerce_arg(
te,e1,t2);}}return 0;}struct _dyneither_ptr Cyc_Tcutil_coercion2string(void*c);
struct _dyneither_ptr Cyc_Tcutil_coercion2string(void*c){void*_tmp512=c;_LL395: if((
int)_tmp512 != 0)goto _LL397;_LL396: {const char*_tmpD72;return(_tmpD72="unknown",
_tag_dyneither(_tmpD72,sizeof(char),8));}_LL397: if((int)_tmp512 != 1)goto _LL399;
_LL398: {const char*_tmpD73;return(_tmpD73="no coercion",_tag_dyneither(_tmpD73,
sizeof(char),12));}_LL399: if((int)_tmp512 != 2)goto _LL39B;_LL39A: {const char*
_tmpD74;return(_tmpD74="null check",_tag_dyneither(_tmpD74,sizeof(char),11));}
_LL39B: if((int)_tmp512 != 3)goto _LL394;_LL39C: {const char*_tmpD75;return(_tmpD75="other coercion",
_tag_dyneither(_tmpD75,sizeof(char),15));}_LL394:;}int Cyc_Tcutil_coerce_arg(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t2);int Cyc_Tcutil_coerce_arg(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t2){void*t1=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);void*c;if(Cyc_Tcutil_unify(
t1,t2))return 1;if(Cyc_Tcutil_is_arithmetic_type(t2) && Cyc_Tcutil_is_arithmetic_type(
t1)){if(Cyc_Tcutil_will_lose_precision(t1,t2)){const char*_tmpD7A;void*_tmpD79[2];
struct Cyc_String_pa_struct _tmpD78;struct Cyc_String_pa_struct _tmpD77;(_tmpD77.tag=
0,((_tmpD77.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t2)),((_tmpD78.tag=0,((_tmpD78.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t1)),((_tmpD79[0]=& _tmpD78,((_tmpD79[1]=& _tmpD77,Cyc_Tcutil_warn(
e->loc,((_tmpD7A="integral size mismatch; %s -> %s conversion supplied",
_tag_dyneither(_tmpD7A,sizeof(char),53))),_tag_dyneither(_tmpD79,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_unchecked_cast(te,e,t2,(void*)1);return 1;}else{if(Cyc_Tcutil_silent_castable(
te,e->loc,t1,t2)){Cyc_Tcutil_unchecked_cast(te,e,t2,(void*)3);return 1;}else{if(
Cyc_Tcutil_zero_to_null(te,t2,e))return 1;else{if((c=Cyc_Tcutil_castable(te,e->loc,
t1,t2))!= (void*)0){Cyc_Tcutil_unchecked_cast(te,e,t2,c);if(c != (void*)2){const
char*_tmpD7F;void*_tmpD7E[2];struct Cyc_String_pa_struct _tmpD7D;struct Cyc_String_pa_struct
_tmpD7C;(_tmpD7C.tag=0,((_tmpD7C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2)),((_tmpD7D.tag=0,((_tmpD7D.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmpD7E[0]=& _tmpD7D,((_tmpD7E[
1]=& _tmpD7C,Cyc_Tcutil_warn(e->loc,((_tmpD7F="implicit cast from %s to %s",
_tag_dyneither(_tmpD7F,sizeof(char),28))),_tag_dyneither(_tmpD7E,sizeof(void*),2)))))))))))));}
return 1;}else{return 0;}}}}}int Cyc_Tcutil_coerce_assign(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Absyn_Exp*e,void*t);int Cyc_Tcutil_coerce_assign(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e,void*t){return Cyc_Tcutil_coerce_arg(te,e,t);}int Cyc_Tcutil_coerceable(
void*t);int Cyc_Tcutil_coerceable(void*t){void*_tmp51F=Cyc_Tcutil_compress(t);
_LL39E: if(_tmp51F <= (void*)4)goto _LL3A0;if(*((int*)_tmp51F)!= 5)goto _LL3A0;
_LL39F: goto _LL3A1;_LL3A0: if((int)_tmp51F != 1)goto _LL3A2;_LL3A1: goto _LL3A3;_LL3A2:
if(_tmp51F <= (void*)4)goto _LL3A4;if(*((int*)_tmp51F)!= 6)goto _LL3A4;_LL3A3:
return 1;_LL3A4:;_LL3A5: return 0;_LL39D:;}static struct _tuple5*Cyc_Tcutil_flatten_typ_f(
struct _tuple9*env,struct Cyc_Absyn_Aggrfield*x);static struct _tuple5*Cyc_Tcutil_flatten_typ_f(
struct _tuple9*env,struct Cyc_Absyn_Aggrfield*x){struct Cyc_List_List*_tmp521;
struct _RegionHandle*_tmp522;struct _tuple9 _tmp520=*env;_tmp521=_tmp520.f1;_tmp522=
_tmp520.f2;{struct _tuple5*_tmpD80;return(_tmpD80=_region_malloc(_tmp522,sizeof(*
_tmpD80)),((_tmpD80->f1=x->tq,((_tmpD80->f2=Cyc_Tcutil_rsubstitute(_tmp522,
_tmp521,(void*)x->type),_tmpD80)))));}}static struct _tuple5*Cyc_Tcutil_rcopy_tqt(
struct _RegionHandle*r,struct _tuple5*x);static struct _tuple5*Cyc_Tcutil_rcopy_tqt(
struct _RegionHandle*r,struct _tuple5*x){struct Cyc_Absyn_Tqual _tmp525;void*_tmp526;
struct _tuple5 _tmp524=*x;_tmp525=_tmp524.f1;_tmp526=_tmp524.f2;{struct _tuple5*
_tmpD81;return(_tmpD81=_region_malloc(r,sizeof(*_tmpD81)),((_tmpD81->f1=_tmp525,((
_tmpD81->f2=_tmp526,_tmpD81)))));}}static struct Cyc_List_List*Cyc_Tcutil_flatten_typ(
struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,void*t1);static struct Cyc_List_List*
Cyc_Tcutil_flatten_typ(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,void*t1){t1=
Cyc_Tcutil_compress(t1);{void*_tmp528=t1;struct Cyc_List_List*_tmp529;struct Cyc_Absyn_AggrInfo
_tmp52A;union Cyc_Absyn_AggrInfoU_union _tmp52B;struct Cyc_Absyn_Aggrdecl**_tmp52C;
struct Cyc_Absyn_Aggrdecl*_tmp52D;struct Cyc_List_List*_tmp52E;void*_tmp52F;struct
Cyc_List_List*_tmp530;struct Cyc_Absyn_FnInfo _tmp531;_LL3A7: if((int)_tmp528 != 0)
goto _LL3A9;_LL3A8: return 0;_LL3A9: if(_tmp528 <= (void*)4)goto _LL3B1;if(*((int*)
_tmp528)!= 9)goto _LL3AB;_tmp529=((struct Cyc_Absyn_TupleType_struct*)_tmp528)->f1;
_LL3AA: return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple5*(*f)(
struct _RegionHandle*,struct _tuple5*),struct _RegionHandle*env,struct Cyc_List_List*
x))Cyc_List_rmap_c)(r,Cyc_Tcutil_rcopy_tqt,r,_tmp529);_LL3AB: if(*((int*)_tmp528)
!= 10)goto _LL3AD;_tmp52A=((struct Cyc_Absyn_AggrType_struct*)_tmp528)->f1;_tmp52B=
_tmp52A.aggr_info;if((((((struct Cyc_Absyn_AggrType_struct*)_tmp528)->f1).aggr_info).KnownAggr).tag
!= 1)goto _LL3AD;_tmp52C=(_tmp52B.KnownAggr).f1;_tmp52D=*_tmp52C;_tmp52E=_tmp52A.targs;
_LL3AC: if((((void*)_tmp52D->kind == (void*)1  || _tmp52D->impl == 0) || ((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp52D->impl))->exist_vars != 0) || ((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp52D->impl))->rgn_po != 0){struct _tuple5*_tmpD84;struct Cyc_List_List*
_tmpD83;return(_tmpD83=_region_malloc(r,sizeof(*_tmpD83)),((_tmpD83->hd=((
_tmpD84=_region_malloc(r,sizeof(*_tmpD84)),((_tmpD84->f1=Cyc_Absyn_empty_tqual(0),((
_tmpD84->f2=t1,_tmpD84)))))),((_tmpD83->tl=0,_tmpD83)))));}{struct Cyc_List_List*
_tmp534=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp52D->tvs,
_tmp52E);struct _tuple9 _tmpD85;struct _tuple9 env=(_tmpD85.f1=_tmp534,((_tmpD85.f2=
r,_tmpD85)));return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple5*(*
f)(struct _tuple9*,struct Cyc_Absyn_Aggrfield*),struct _tuple9*env,struct Cyc_List_List*
x))Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_typ_f,& env,((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp52D->impl))->fields);}_LL3AD: if(*((int*)_tmp528)!= 11)goto _LL3AF;
_tmp52F=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)_tmp528)->f1;if((int)
_tmp52F != 0)goto _LL3AF;_tmp530=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp528)->f2;
_LL3AE: {struct _tuple9 _tmpD86;struct _tuple9 env=(_tmpD86.f1=0,((_tmpD86.f2=r,
_tmpD86)));return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple5*(*
f)(struct _tuple9*,struct Cyc_Absyn_Aggrfield*),struct _tuple9*env,struct Cyc_List_List*
x))Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_typ_f,& env,_tmp530);}_LL3AF: if(*((int*)
_tmp528)!= 8)goto _LL3B1;_tmp531=((struct Cyc_Absyn_FnType_struct*)_tmp528)->f1;
_LL3B0: {struct _tuple5*_tmpD89;struct Cyc_List_List*_tmpD88;return(_tmpD88=
_region_malloc(r,sizeof(*_tmpD88)),((_tmpD88->hd=((_tmpD89=_region_malloc(r,
sizeof(*_tmpD89)),((_tmpD89->f1=Cyc_Absyn_const_tqual(0),((_tmpD89->f2=t1,
_tmpD89)))))),((_tmpD88->tl=0,_tmpD88)))));}_LL3B1:;_LL3B2: {struct _tuple5*
_tmpD8C;struct Cyc_List_List*_tmpD8B;return(_tmpD8B=_region_malloc(r,sizeof(*
_tmpD8B)),((_tmpD8B->hd=((_tmpD8C=_region_malloc(r,sizeof(*_tmpD8C)),((_tmpD8C->f1=
Cyc_Absyn_empty_tqual(0),((_tmpD8C->f2=t1,_tmpD8C)))))),((_tmpD8B->tl=0,_tmpD8B)))));}
_LL3A6:;}}static int Cyc_Tcutil_sub_attributes(struct Cyc_List_List*a1,struct Cyc_List_List*
a2);static int Cyc_Tcutil_sub_attributes(struct Cyc_List_List*a1,struct Cyc_List_List*
a2){{struct Cyc_List_List*t=a1;for(0;t != 0;t=t->tl){void*_tmp53B=(void*)t->hd;
_LL3B4: if((int)_tmp53B != 16)goto _LL3B6;_LL3B5: goto _LL3B7;_LL3B6: if((int)_tmp53B
!= 3)goto _LL3B8;_LL3B7: goto _LL3B9;_LL3B8: if(_tmp53B <= (void*)17)goto _LL3BA;if(*((
int*)_tmp53B)!= 4)goto _LL3BA;_LL3B9: continue;_LL3BA:;_LL3BB: if(!Cyc_List_exists_c(
Cyc_Tcutil_equal_att,(void*)t->hd,a2))return 0;_LL3B3:;}}for(0;a2 != 0;a2=a2->tl){
if(!Cyc_List_exists_c(Cyc_Tcutil_equal_att,(void*)a2->hd,a1))return 0;}return 1;}
static int Cyc_Tcutil_ptrsubtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*
assume,void*t1,void*t2);static int Cyc_Tcutil_subtype(struct Cyc_Tcenv_Tenv*te,
struct Cyc_List_List*assume,void*t1,void*t2);static int Cyc_Tcutil_subtype(struct
Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2){if(Cyc_Tcutil_unify(
t1,t2))return 1;{struct Cyc_List_List*a=assume;for(0;a != 0;a=a->tl){if(Cyc_Tcutil_unify(
t1,(*((struct _tuple0*)a->hd)).f1) && Cyc_Tcutil_unify(t2,(*((struct _tuple0*)a->hd)).f2))
return 1;}}t1=Cyc_Tcutil_compress(t1);t2=Cyc_Tcutil_compress(t2);{struct _tuple0
_tmpD8D;struct _tuple0 _tmp53D=(_tmpD8D.f1=t1,((_tmpD8D.f2=t2,_tmpD8D)));void*
_tmp53E;struct Cyc_Absyn_PtrInfo _tmp53F;void*_tmp540;struct Cyc_Absyn_Tqual _tmp541;
struct Cyc_Absyn_PtrAtts _tmp542;void*_tmp543;struct Cyc_Absyn_Conref*_tmp544;
struct Cyc_Absyn_Conref*_tmp545;struct Cyc_Absyn_Conref*_tmp546;void*_tmp547;
struct Cyc_Absyn_PtrInfo _tmp548;void*_tmp549;struct Cyc_Absyn_Tqual _tmp54A;struct
Cyc_Absyn_PtrAtts _tmp54B;void*_tmp54C;struct Cyc_Absyn_Conref*_tmp54D;struct Cyc_Absyn_Conref*
_tmp54E;struct Cyc_Absyn_Conref*_tmp54F;void*_tmp550;struct Cyc_Absyn_TunionInfo
_tmp551;union Cyc_Absyn_TunionInfoU_union _tmp552;struct Cyc_Absyn_Tuniondecl**
_tmp553;struct Cyc_Absyn_Tuniondecl*_tmp554;struct Cyc_List_List*_tmp555;struct Cyc_Core_Opt*
_tmp556;struct Cyc_Core_Opt _tmp557;void*_tmp558;void*_tmp559;struct Cyc_Absyn_TunionInfo
_tmp55A;union Cyc_Absyn_TunionInfoU_union _tmp55B;struct Cyc_Absyn_Tuniondecl**
_tmp55C;struct Cyc_Absyn_Tuniondecl*_tmp55D;struct Cyc_List_List*_tmp55E;struct Cyc_Core_Opt*
_tmp55F;struct Cyc_Core_Opt _tmp560;void*_tmp561;void*_tmp562;struct Cyc_Absyn_FnInfo
_tmp563;void*_tmp564;struct Cyc_Absyn_FnInfo _tmp565;_LL3BD: _tmp53E=_tmp53D.f1;if(
_tmp53E <= (void*)4)goto _LL3BF;if(*((int*)_tmp53E)!= 4)goto _LL3BF;_tmp53F=((
struct Cyc_Absyn_PointerType_struct*)_tmp53E)->f1;_tmp540=(void*)_tmp53F.elt_typ;
_tmp541=_tmp53F.elt_tq;_tmp542=_tmp53F.ptr_atts;_tmp543=(void*)_tmp542.rgn;
_tmp544=_tmp542.nullable;_tmp545=_tmp542.bounds;_tmp546=_tmp542.zero_term;
_tmp547=_tmp53D.f2;if(_tmp547 <= (void*)4)goto _LL3BF;if(*((int*)_tmp547)!= 4)goto
_LL3BF;_tmp548=((struct Cyc_Absyn_PointerType_struct*)_tmp547)->f1;_tmp549=(void*)
_tmp548.elt_typ;_tmp54A=_tmp548.elt_tq;_tmp54B=_tmp548.ptr_atts;_tmp54C=(void*)
_tmp54B.rgn;_tmp54D=_tmp54B.nullable;_tmp54E=_tmp54B.bounds;_tmp54F=_tmp54B.zero_term;
_LL3BE: if(_tmp541.real_const  && !_tmp54A.real_const)return 0;if((!((int(*)(int(*
cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(
Cyc_Core_intcmp,_tmp544,_tmp54D) && ((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,_tmp544)) && !((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,
_tmp54D))return 0;if((!((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,
struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp546,
_tmp54F) && !((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,
_tmp546)) && ((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,
_tmp54F))return 0;if(!Cyc_Tcutil_unify(_tmp543,_tmp54C) && !Cyc_Tcenv_region_outlives(
te,_tmp543,_tmp54C))return 0;if(!Cyc_Tcutil_unify_conrefs(Cyc_Tcutil_unify_it_bounds,
_tmp545,_tmp54E)){struct _tuple0 _tmpD8E;struct _tuple0 _tmp567=(_tmpD8E.f1=Cyc_Absyn_conref_val(
_tmp545),((_tmpD8E.f2=Cyc_Absyn_conref_val(_tmp54E),_tmpD8E)));void*_tmp568;void*
_tmp569;void*_tmp56A;struct Cyc_Absyn_Exp*_tmp56B;void*_tmp56C;struct Cyc_Absyn_Exp*
_tmp56D;_LL3C6: _tmp568=_tmp567.f1;if(_tmp568 <= (void*)1)goto _LL3C8;if(*((int*)
_tmp568)!= 0)goto _LL3C8;_tmp569=_tmp567.f2;if((int)_tmp569 != 0)goto _LL3C8;_LL3C7:
goto _LL3C5;_LL3C8: _tmp56A=_tmp567.f1;if(_tmp56A <= (void*)1)goto _LL3CA;if(*((int*)
_tmp56A)!= 0)goto _LL3CA;_tmp56B=((struct Cyc_Absyn_Upper_b_struct*)_tmp56A)->f1;
_tmp56C=_tmp567.f2;if(_tmp56C <= (void*)1)goto _LL3CA;if(*((int*)_tmp56C)!= 0)goto
_LL3CA;_tmp56D=((struct Cyc_Absyn_Upper_b_struct*)_tmp56C)->f1;_LL3C9: if(!Cyc_Evexp_lte_const_exp(
_tmp56D,_tmp56B))return 0;goto _LL3C5;_LL3CA:;_LL3CB: return 0;_LL3C5:;}{struct
_tuple0*_tmpD91;struct Cyc_List_List*_tmpD90;return Cyc_Tcutil_ptrsubtype(te,((
_tmpD90=_cycalloc(sizeof(*_tmpD90)),((_tmpD90->hd=((_tmpD91=_cycalloc(sizeof(*
_tmpD91)),((_tmpD91->f1=t1,((_tmpD91->f2=t2,_tmpD91)))))),((_tmpD90->tl=assume,
_tmpD90)))))),_tmp540,_tmp549);}_LL3BF: _tmp550=_tmp53D.f1;if(_tmp550 <= (void*)4)
goto _LL3C1;if(*((int*)_tmp550)!= 2)goto _LL3C1;_tmp551=((struct Cyc_Absyn_TunionType_struct*)
_tmp550)->f1;_tmp552=_tmp551.tunion_info;if((((((struct Cyc_Absyn_TunionType_struct*)
_tmp550)->f1).tunion_info).KnownTunion).tag != 1)goto _LL3C1;_tmp553=(_tmp552.KnownTunion).f1;
_tmp554=*_tmp553;_tmp555=_tmp551.targs;_tmp556=_tmp551.rgn;if(_tmp556 == 0)goto
_LL3C1;_tmp557=*_tmp556;_tmp558=(void*)_tmp557.v;_tmp559=_tmp53D.f2;if(_tmp559 <= (
void*)4)goto _LL3C1;if(*((int*)_tmp559)!= 2)goto _LL3C1;_tmp55A=((struct Cyc_Absyn_TunionType_struct*)
_tmp559)->f1;_tmp55B=_tmp55A.tunion_info;if((((((struct Cyc_Absyn_TunionType_struct*)
_tmp559)->f1).tunion_info).KnownTunion).tag != 1)goto _LL3C1;_tmp55C=(_tmp55B.KnownTunion).f1;
_tmp55D=*_tmp55C;_tmp55E=_tmp55A.targs;_tmp55F=_tmp55A.rgn;if(_tmp55F == 0)goto
_LL3C1;_tmp560=*_tmp55F;_tmp561=(void*)_tmp560.v;_LL3C0: if(_tmp554 != _tmp55D  || 
!Cyc_Tcenv_region_outlives(te,_tmp558,_tmp561))return 0;for(0;_tmp555 != 0  && 
_tmp55E != 0;(_tmp555=_tmp555->tl,_tmp55E=_tmp55E->tl)){if(!Cyc_Tcutil_unify((
void*)_tmp555->hd,(void*)_tmp55E->hd))return 0;}if(_tmp555 != 0  || _tmp55E != 0)
return 0;return 1;_LL3C1: _tmp562=_tmp53D.f1;if(_tmp562 <= (void*)4)goto _LL3C3;if(*((
int*)_tmp562)!= 8)goto _LL3C3;_tmp563=((struct Cyc_Absyn_FnType_struct*)_tmp562)->f1;
_tmp564=_tmp53D.f2;if(_tmp564 <= (void*)4)goto _LL3C3;if(*((int*)_tmp564)!= 8)goto
_LL3C3;_tmp565=((struct Cyc_Absyn_FnType_struct*)_tmp564)->f1;_LL3C2: if(_tmp563.tvars
!= 0  || _tmp565.tvars != 0){struct Cyc_List_List*_tmp570=_tmp563.tvars;struct Cyc_List_List*
_tmp571=_tmp565.tvars;if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp570)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp571))return 0;{
struct _RegionHandle*_tmp572=Cyc_Tcenv_get_fnrgn(te);struct Cyc_List_List*inst=0;
while(_tmp570 != 0){if(Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp570->hd)!= 
Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)_check_null(
_tmp571))->hd))return 0;{struct _tuple8*_tmpD9B;struct Cyc_Absyn_VarType_struct
_tmpD9A;struct Cyc_Absyn_VarType_struct*_tmpD99;struct Cyc_List_List*_tmpD98;inst=((
_tmpD98=_region_malloc(_tmp572,sizeof(*_tmpD98)),((_tmpD98->hd=((_tmpD9B=
_region_malloc(_tmp572,sizeof(*_tmpD9B)),((_tmpD9B->f1=(struct Cyc_Absyn_Tvar*)
_tmp571->hd,((_tmpD9B->f2=(void*)((_tmpD99=_cycalloc(sizeof(*_tmpD99)),((_tmpD99[
0]=((_tmpD9A.tag=1,((_tmpD9A.f1=(struct Cyc_Absyn_Tvar*)_tmp570->hd,_tmpD9A)))),
_tmpD99)))),_tmpD9B)))))),((_tmpD98->tl=inst,_tmpD98))))));}_tmp570=_tmp570->tl;
_tmp571=_tmp571->tl;}if(inst != 0){_tmp563.tvars=0;_tmp565.tvars=0;{struct Cyc_Absyn_FnType_struct
_tmpDA1;struct Cyc_Absyn_FnType_struct*_tmpDA0;struct Cyc_Absyn_FnType_struct
_tmpD9E;struct Cyc_Absyn_FnType_struct*_tmpD9D;return Cyc_Tcutil_subtype(te,assume,(
void*)((_tmpD9D=_cycalloc(sizeof(*_tmpD9D)),((_tmpD9D[0]=((_tmpD9E.tag=8,((
_tmpD9E.f1=_tmp563,_tmpD9E)))),_tmpD9D)))),(void*)((_tmpDA0=_cycalloc(sizeof(*
_tmpDA0)),((_tmpDA0[0]=((_tmpDA1.tag=8,((_tmpDA1.f1=_tmp565,_tmpDA1)))),_tmpDA0)))));}}}}
if(!Cyc_Tcutil_subtype(te,assume,(void*)_tmp563.ret_typ,(void*)_tmp565.ret_typ))
return 0;{struct Cyc_List_List*_tmp57B=_tmp563.args;struct Cyc_List_List*_tmp57C=
_tmp565.args;if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp57B)!= ((
int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp57C))return 0;for(0;_tmp57B != 
0;(_tmp57B=_tmp57B->tl,_tmp57C=_tmp57C->tl)){struct Cyc_Absyn_Tqual _tmp57E;void*
_tmp57F;struct _tuple3 _tmp57D=*((struct _tuple3*)_tmp57B->hd);_tmp57E=_tmp57D.f2;
_tmp57F=_tmp57D.f3;{struct Cyc_Absyn_Tqual _tmp581;void*_tmp582;struct _tuple3
_tmp580=*((struct _tuple3*)((struct Cyc_List_List*)_check_null(_tmp57C))->hd);
_tmp581=_tmp580.f2;_tmp582=_tmp580.f3;if(_tmp581.real_const  && !_tmp57E.real_const
 || !Cyc_Tcutil_subtype(te,assume,_tmp582,_tmp57F))return 0;}}if(_tmp563.c_varargs
!= _tmp565.c_varargs)return 0;if(_tmp563.cyc_varargs != 0  && _tmp565.cyc_varargs != 
0){struct Cyc_Absyn_VarargInfo _tmp583=*_tmp563.cyc_varargs;struct Cyc_Absyn_VarargInfo
_tmp584=*_tmp565.cyc_varargs;if((_tmp584.tq).real_const  && !(_tmp583.tq).real_const
 || !Cyc_Tcutil_subtype(te,assume,(void*)_tmp584.type,(void*)_tmp583.type))
return 0;}else{if(_tmp563.cyc_varargs != 0  || _tmp565.cyc_varargs != 0)return 0;}if(
!Cyc_Tcutil_subset_effect(1,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp563.effect))->v,(
void*)((struct Cyc_Core_Opt*)_check_null(_tmp565.effect))->v))return 0;if(!Cyc_Tcutil_sub_rgnpo(
_tmp563.rgn_po,_tmp565.rgn_po))return 0;if(!Cyc_Tcutil_sub_attributes(_tmp563.attributes,
_tmp565.attributes))return 0;return 1;}_LL3C3:;_LL3C4: return 0;_LL3BC:;}}static int
Cyc_Tcutil_isomorphic(void*t1,void*t2);static int Cyc_Tcutil_isomorphic(void*t1,
void*t2){struct _tuple0 _tmpDA2;struct _tuple0 _tmp586=(_tmpDA2.f1=Cyc_Tcutil_compress(
t1),((_tmpDA2.f2=Cyc_Tcutil_compress(t2),_tmpDA2)));void*_tmp587;void*_tmp588;
void*_tmp589;void*_tmp58A;_LL3CD: _tmp587=_tmp586.f1;if(_tmp587 <= (void*)4)goto
_LL3CF;if(*((int*)_tmp587)!= 5)goto _LL3CF;_tmp588=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp587)->f2;_tmp589=_tmp586.f2;if(_tmp589 <= (void*)4)goto _LL3CF;if(*((int*)
_tmp589)!= 5)goto _LL3CF;_tmp58A=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp589)->f2;
_LL3CE: return(_tmp588 == _tmp58A  || _tmp588 == (void*)2  && _tmp58A == (void*)3) || 
_tmp588 == (void*)3  && _tmp58A == (void*)2;_LL3CF:;_LL3D0: return 0;_LL3CC:;}static
int Cyc_Tcutil_ptrsubtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*
t1,void*t2);static int Cyc_Tcutil_ptrsubtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*
assume,void*t1,void*t2){struct _RegionHandle*_tmp58B=Cyc_Tcenv_get_fnrgn(te);
struct Cyc_List_List*tqs1=Cyc_Tcutil_flatten_typ(_tmp58B,te,t1);struct Cyc_List_List*
tqs2=Cyc_Tcutil_flatten_typ(_tmp58B,te,t2);for(0;tqs2 != 0;(tqs2=tqs2->tl,tqs1=
tqs1->tl)){if(tqs1 == 0)return 0;{struct _tuple5 _tmp58D;struct Cyc_Absyn_Tqual
_tmp58E;void*_tmp58F;struct _tuple5*_tmp58C=(struct _tuple5*)tqs1->hd;_tmp58D=*
_tmp58C;_tmp58E=_tmp58D.f1;_tmp58F=_tmp58D.f2;{struct _tuple5 _tmp591;struct Cyc_Absyn_Tqual
_tmp592;void*_tmp593;struct _tuple5*_tmp590=(struct _tuple5*)tqs2->hd;_tmp591=*
_tmp590;_tmp592=_tmp591.f1;_tmp593=_tmp591.f2;if(_tmp592.real_const  && Cyc_Tcutil_subtype(
te,assume,_tmp58F,_tmp593))continue;else{if(Cyc_Tcutil_unify(_tmp58F,_tmp593))
continue;else{if(Cyc_Tcutil_isomorphic(_tmp58F,_tmp593))continue;else{return 0;}}}}}}
return 1;}static int Cyc_Tcutil_is_char_type(void*t);static int Cyc_Tcutil_is_char_type(
void*t){void*_tmp594=Cyc_Tcutil_compress(t);void*_tmp595;_LL3D2: if(_tmp594 <= (
void*)4)goto _LL3D4;if(*((int*)_tmp594)!= 5)goto _LL3D4;_tmp595=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp594)->f2;if((int)_tmp595 != 0)goto _LL3D4;_LL3D3: return 1;_LL3D4:;_LL3D5: return
0;_LL3D1:;}void*Cyc_Tcutil_castable(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void*t1,void*t2);void*Cyc_Tcutil_castable(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void*t1,void*t2){if(Cyc_Tcutil_unify(t1,t2))return(void*)1;t1=Cyc_Tcutil_compress(
t1);t2=Cyc_Tcutil_compress(t2);if(t2 == (void*)0)return(void*)1;{void*_tmp596=t2;
void*_tmp597;void*_tmp598;_LL3D7: if(_tmp596 <= (void*)4)goto _LL3DB;if(*((int*)
_tmp596)!= 5)goto _LL3D9;_tmp597=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp596)->f2;
if((int)_tmp597 != 2)goto _LL3D9;_LL3D8: goto _LL3DA;_LL3D9: if(*((int*)_tmp596)!= 5)
goto _LL3DB;_tmp598=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp596)->f2;if((int)
_tmp598 != 3)goto _LL3DB;_LL3DA: if(Cyc_Tcutil_typ_kind(t1)== (void*)2)return(void*)
1;goto _LL3D6;_LL3DB:;_LL3DC: goto _LL3D6;_LL3D6:;}{void*_tmp599=t1;struct Cyc_Absyn_PtrInfo
_tmp59A;void*_tmp59B;struct Cyc_Absyn_Tqual _tmp59C;struct Cyc_Absyn_PtrAtts _tmp59D;
void*_tmp59E;struct Cyc_Absyn_Conref*_tmp59F;struct Cyc_Absyn_Conref*_tmp5A0;
struct Cyc_Absyn_Conref*_tmp5A1;struct Cyc_Absyn_ArrayInfo _tmp5A2;void*_tmp5A3;
struct Cyc_Absyn_Tqual _tmp5A4;struct Cyc_Absyn_Exp*_tmp5A5;struct Cyc_Absyn_Conref*
_tmp5A6;struct Cyc_Absyn_Enumdecl*_tmp5A7;void*_tmp5A8;_LL3DE: if(_tmp599 <= (void*)
4)goto _LL3E6;if(*((int*)_tmp599)!= 4)goto _LL3E0;_tmp59A=((struct Cyc_Absyn_PointerType_struct*)
_tmp599)->f1;_tmp59B=(void*)_tmp59A.elt_typ;_tmp59C=_tmp59A.elt_tq;_tmp59D=
_tmp59A.ptr_atts;_tmp59E=(void*)_tmp59D.rgn;_tmp59F=_tmp59D.nullable;_tmp5A0=
_tmp59D.bounds;_tmp5A1=_tmp59D.zero_term;_LL3DF:{void*_tmp5A9=t2;struct Cyc_Absyn_PtrInfo
_tmp5AA;void*_tmp5AB;struct Cyc_Absyn_Tqual _tmp5AC;struct Cyc_Absyn_PtrAtts _tmp5AD;
void*_tmp5AE;struct Cyc_Absyn_Conref*_tmp5AF;struct Cyc_Absyn_Conref*_tmp5B0;
struct Cyc_Absyn_Conref*_tmp5B1;_LL3EF: if(_tmp5A9 <= (void*)4)goto _LL3F1;if(*((int*)
_tmp5A9)!= 4)goto _LL3F1;_tmp5AA=((struct Cyc_Absyn_PointerType_struct*)_tmp5A9)->f1;
_tmp5AB=(void*)_tmp5AA.elt_typ;_tmp5AC=_tmp5AA.elt_tq;_tmp5AD=_tmp5AA.ptr_atts;
_tmp5AE=(void*)_tmp5AD.rgn;_tmp5AF=_tmp5AD.nullable;_tmp5B0=_tmp5AD.bounds;
_tmp5B1=_tmp5AD.zero_term;_LL3F0: {void*coercion=(void*)3;struct _tuple0*_tmpDA5;
struct Cyc_List_List*_tmpDA4;struct Cyc_List_List*_tmp5B2=(_tmpDA4=_cycalloc(
sizeof(*_tmpDA4)),((_tmpDA4->hd=((_tmpDA5=_cycalloc(sizeof(*_tmpDA5)),((_tmpDA5->f1=
t1,((_tmpDA5->f2=t2,_tmpDA5)))))),((_tmpDA4->tl=0,_tmpDA4)))));int _tmp5B3=Cyc_Tcutil_ptrsubtype(
te,_tmp5B2,_tmp59B,_tmp5AB) && (!_tmp59C.real_const  || _tmp5AC.real_const);Cyc_Tcutil_t1_failure=
t1;Cyc_Tcutil_t2_failure=t2;{int zeroterm_ok=((int(*)(int(*cmp)(int,int),struct
Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,
_tmp5A1,_tmp5B1) || !((int(*)(struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_val)(
_tmp5B1);int _tmp5B4=_tmp5B3?0:((Cyc_Tcutil_bits_only(_tmp59B) && Cyc_Tcutil_is_char_type(
_tmp5AB)) && !((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,
_tmp5B1)) && (_tmp5AC.real_const  || !_tmp59C.real_const);int bounds_ok=Cyc_Tcutil_unify_conrefs(
Cyc_Tcutil_unify_it_bounds,_tmp5A0,_tmp5B0);if(!bounds_ok  && !_tmp5B4){struct
_tuple0 _tmpDA6;struct _tuple0 _tmp5B6=(_tmpDA6.f1=Cyc_Absyn_conref_val(_tmp5A0),((
_tmpDA6.f2=Cyc_Absyn_conref_val(_tmp5B0),_tmpDA6)));void*_tmp5B7;struct Cyc_Absyn_Exp*
_tmp5B8;void*_tmp5B9;struct Cyc_Absyn_Exp*_tmp5BA;_LL3F4: _tmp5B7=_tmp5B6.f1;if(
_tmp5B7 <= (void*)1)goto _LL3F6;if(*((int*)_tmp5B7)!= 0)goto _LL3F6;_tmp5B8=((
struct Cyc_Absyn_Upper_b_struct*)_tmp5B7)->f1;_tmp5B9=_tmp5B6.f2;if(_tmp5B9 <= (
void*)1)goto _LL3F6;if(*((int*)_tmp5B9)!= 0)goto _LL3F6;_tmp5BA=((struct Cyc_Absyn_Upper_b_struct*)
_tmp5B9)->f1;_LL3F5: if(Cyc_Evexp_lte_const_exp(_tmp5BA,_tmp5B8))bounds_ok=1;goto
_LL3F3;_LL3F6:;_LL3F7: bounds_ok=1;goto _LL3F3;_LL3F3:;}if(((int(*)(int,struct Cyc_Absyn_Conref*
x))Cyc_Absyn_conref_def)(0,_tmp59F) && !((int(*)(int,struct Cyc_Absyn_Conref*x))
Cyc_Absyn_conref_def)(0,_tmp5AF))coercion=(void*)2;if(((bounds_ok  && zeroterm_ok)
 && (_tmp5B3  || _tmp5B4)) && (Cyc_Tcutil_unify(_tmp59E,_tmp5AE) || Cyc_Tcenv_region_outlives(
te,_tmp59E,_tmp5AE)))return coercion;else{return(void*)0;}}}_LL3F1:;_LL3F2: goto
_LL3EE;_LL3EE:;}return(void*)0;_LL3E0: if(*((int*)_tmp599)!= 7)goto _LL3E2;_tmp5A2=((
struct Cyc_Absyn_ArrayType_struct*)_tmp599)->f1;_tmp5A3=(void*)_tmp5A2.elt_type;
_tmp5A4=_tmp5A2.tq;_tmp5A5=_tmp5A2.num_elts;_tmp5A6=_tmp5A2.zero_term;_LL3E1:{
void*_tmp5BD=t2;struct Cyc_Absyn_ArrayInfo _tmp5BE;void*_tmp5BF;struct Cyc_Absyn_Tqual
_tmp5C0;struct Cyc_Absyn_Exp*_tmp5C1;struct Cyc_Absyn_Conref*_tmp5C2;_LL3F9: if(
_tmp5BD <= (void*)4)goto _LL3FB;if(*((int*)_tmp5BD)!= 7)goto _LL3FB;_tmp5BE=((
struct Cyc_Absyn_ArrayType_struct*)_tmp5BD)->f1;_tmp5BF=(void*)_tmp5BE.elt_type;
_tmp5C0=_tmp5BE.tq;_tmp5C1=_tmp5BE.num_elts;_tmp5C2=_tmp5BE.zero_term;_LL3FA: {
int okay;okay=((_tmp5A5 != 0  && _tmp5C1 != 0) && ((int(*)(int(*cmp)(int,int),struct
Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,
_tmp5A6,_tmp5C2)) && Cyc_Evexp_lte_const_exp((struct Cyc_Absyn_Exp*)_tmp5C1,(
struct Cyc_Absyn_Exp*)_tmp5A5);return(okay  && Cyc_Tcutil_unify(_tmp5A3,_tmp5BF))
 && (!_tmp5A4.real_const  || _tmp5C0.real_const)?(void*)3:(void*)0;}_LL3FB:;
_LL3FC: return(void*)0;_LL3F8:;}return(void*)0;_LL3E2: if(*((int*)_tmp599)!= 12)
goto _LL3E4;_tmp5A7=((struct Cyc_Absyn_EnumType_struct*)_tmp599)->f2;_LL3E3:{void*
_tmp5C3=t2;struct Cyc_Absyn_Enumdecl*_tmp5C4;_LL3FE: if(_tmp5C3 <= (void*)4)goto
_LL400;if(*((int*)_tmp5C3)!= 12)goto _LL400;_tmp5C4=((struct Cyc_Absyn_EnumType_struct*)
_tmp5C3)->f2;_LL3FF: if((_tmp5A7->fields != 0  && _tmp5C4->fields != 0) && ((int(*)(
struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp5A7->fields))->v)>= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)((
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp5C4->fields))->v))
return(void*)1;goto _LL3FD;_LL400:;_LL401: goto _LL3FD;_LL3FD:;}goto _LL3E5;_LL3E4:
if(*((int*)_tmp599)!= 5)goto _LL3E6;_LL3E5: goto _LL3E7;_LL3E6: if((int)_tmp599 != 1)
goto _LL3E8;_LL3E7: goto _LL3E9;_LL3E8: if(_tmp599 <= (void*)4)goto _LL3EC;if(*((int*)
_tmp599)!= 6)goto _LL3EA;_LL3E9: return Cyc_Tcutil_coerceable(t2)?(void*)1:(void*)0;
_LL3EA: if(*((int*)_tmp599)!= 14)goto _LL3EC;_tmp5A8=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp599)->f1;_LL3EB:{void*_tmp5C5=t2;void*_tmp5C6;_LL403: if(_tmp5C5 <= (void*)4)
goto _LL405;if(*((int*)_tmp5C5)!= 14)goto _LL405;_tmp5C6=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp5C5)->f1;_LL404: if(Cyc_Tcenv_region_outlives(te,_tmp5A8,_tmp5C6))return(void*)
1;goto _LL402;_LL405:;_LL406: goto _LL402;_LL402:;}return(void*)0;_LL3EC:;_LL3ED:
return(void*)0;_LL3DD:;}}void Cyc_Tcutil_unchecked_cast(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Absyn_Exp*e,void*t,void*c);void Cyc_Tcutil_unchecked_cast(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e,void*t,void*c){if(!Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,t)){struct Cyc_Absyn_Exp*_tmp5C7=Cyc_Absyn_copy_exp(e);{
struct Cyc_Absyn_Cast_e_struct _tmpDA9;struct Cyc_Absyn_Cast_e_struct*_tmpDA8;(void*)(
e->r=(void*)((void*)((_tmpDA8=_cycalloc(sizeof(*_tmpDA8)),((_tmpDA8[0]=((_tmpDA9.tag=
15,((_tmpDA9.f1=(void*)t,((_tmpDA9.f2=_tmp5C7,((_tmpDA9.f3=0,((_tmpDA9.f4=(void*)
c,_tmpDA9)))))))))),_tmpDA8))))));}{struct Cyc_Core_Opt*_tmpDAA;e->topt=((_tmpDAA=
_cycalloc(sizeof(*_tmpDAA)),((_tmpDAA->v=(void*)t,_tmpDAA))));}}}int Cyc_Tcutil_is_integral(
struct Cyc_Absyn_Exp*e);int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*e){void*
_tmp5CB=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
_LL408: if(_tmp5CB <= (void*)4)goto _LL412;if(*((int*)_tmp5CB)!= 5)goto _LL40A;
_LL409: goto _LL40B;_LL40A: if(*((int*)_tmp5CB)!= 12)goto _LL40C;_LL40B: goto _LL40D;
_LL40C: if(*((int*)_tmp5CB)!= 13)goto _LL40E;_LL40D: goto _LL40F;_LL40E: if(*((int*)
_tmp5CB)!= 18)goto _LL410;_LL40F: return 1;_LL410: if(*((int*)_tmp5CB)!= 0)goto
_LL412;_LL411: return Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,
Cyc_Absyn_sint_typ);_LL412:;_LL413: return 0;_LL407:;}int Cyc_Tcutil_is_numeric(
struct Cyc_Absyn_Exp*e);int Cyc_Tcutil_is_numeric(struct Cyc_Absyn_Exp*e){if(Cyc_Tcutil_is_integral(
e))return 1;{void*_tmp5CC=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);_LL415: if((int)_tmp5CC != 1)goto _LL417;_LL416: goto
_LL418;_LL417: if(_tmp5CC <= (void*)4)goto _LL419;if(*((int*)_tmp5CC)!= 6)goto
_LL419;_LL418: return 1;_LL419:;_LL41A: return 0;_LL414:;}}int Cyc_Tcutil_is_function_type(
void*t);int Cyc_Tcutil_is_function_type(void*t){void*_tmp5CD=Cyc_Tcutil_compress(
t);_LL41C: if(_tmp5CD <= (void*)4)goto _LL41E;if(*((int*)_tmp5CD)!= 8)goto _LL41E;
_LL41D: return 1;_LL41E:;_LL41F: return 0;_LL41B:;}void*Cyc_Tcutil_max_arithmetic_type(
void*t1,void*t2);void*Cyc_Tcutil_max_arithmetic_type(void*t1,void*t2){struct
_tuple0 _tmpDAB;struct _tuple0 _tmp5CF=(_tmpDAB.f1=t1,((_tmpDAB.f2=t2,_tmpDAB)));
void*_tmp5D0;int _tmp5D1;void*_tmp5D2;int _tmp5D3;void*_tmp5D4;void*_tmp5D5;void*
_tmp5D6;void*_tmp5D7;void*_tmp5D8;void*_tmp5D9;void*_tmp5DA;void*_tmp5DB;void*
_tmp5DC;void*_tmp5DD;void*_tmp5DE;void*_tmp5DF;void*_tmp5E0;void*_tmp5E1;void*
_tmp5E2;void*_tmp5E3;void*_tmp5E4;void*_tmp5E5;void*_tmp5E6;void*_tmp5E7;void*
_tmp5E8;void*_tmp5E9;void*_tmp5EA;void*_tmp5EB;void*_tmp5EC;void*_tmp5ED;void*
_tmp5EE;void*_tmp5EF;void*_tmp5F0;void*_tmp5F1;_LL421: _tmp5D0=_tmp5CF.f1;if(
_tmp5D0 <= (void*)4)goto _LL423;if(*((int*)_tmp5D0)!= 6)goto _LL423;_tmp5D1=((
struct Cyc_Absyn_DoubleType_struct*)_tmp5D0)->f1;_tmp5D2=_tmp5CF.f2;if(_tmp5D2 <= (
void*)4)goto _LL423;if(*((int*)_tmp5D2)!= 6)goto _LL423;_tmp5D3=((struct Cyc_Absyn_DoubleType_struct*)
_tmp5D2)->f1;_LL422: if(_tmp5D1)return t1;else{return t2;}_LL423: _tmp5D4=_tmp5CF.f1;
if(_tmp5D4 <= (void*)4)goto _LL425;if(*((int*)_tmp5D4)!= 6)goto _LL425;_LL424:
return t1;_LL425: _tmp5D5=_tmp5CF.f2;if(_tmp5D5 <= (void*)4)goto _LL427;if(*((int*)
_tmp5D5)!= 6)goto _LL427;_LL426: return t2;_LL427: _tmp5D6=_tmp5CF.f1;if((int)
_tmp5D6 != 1)goto _LL429;_LL428: goto _LL42A;_LL429: _tmp5D7=_tmp5CF.f2;if((int)
_tmp5D7 != 1)goto _LL42B;_LL42A: return(void*)1;_LL42B: _tmp5D8=_tmp5CF.f1;if(
_tmp5D8 <= (void*)4)goto _LL42D;if(*((int*)_tmp5D8)!= 5)goto _LL42D;_tmp5D9=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp5D8)->f1;if((int)_tmp5D9 != 1)goto _LL42D;
_tmp5DA=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp5D8)->f2;if((int)_tmp5DA != 
4)goto _LL42D;_LL42C: goto _LL42E;_LL42D: _tmp5DB=_tmp5CF.f2;if(_tmp5DB <= (void*)4)
goto _LL42F;if(*((int*)_tmp5DB)!= 5)goto _LL42F;_tmp5DC=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp5DB)->f1;if((int)_tmp5DC != 1)goto _LL42F;_tmp5DD=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp5DB)->f2;if((int)_tmp5DD != 4)goto _LL42F;_LL42E: return Cyc_Absyn_ulonglong_typ;
_LL42F: _tmp5DE=_tmp5CF.f1;if(_tmp5DE <= (void*)4)goto _LL431;if(*((int*)_tmp5DE)!= 
5)goto _LL431;_tmp5DF=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp5DE)->f2;if((
int)_tmp5DF != 4)goto _LL431;_LL430: goto _LL432;_LL431: _tmp5E0=_tmp5CF.f2;if(
_tmp5E0 <= (void*)4)goto _LL433;if(*((int*)_tmp5E0)!= 5)goto _LL433;_tmp5E1=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp5E0)->f2;if((int)_tmp5E1 != 4)goto _LL433;
_LL432: return Cyc_Absyn_slonglong_typ;_LL433: _tmp5E2=_tmp5CF.f1;if(_tmp5E2 <= (
void*)4)goto _LL435;if(*((int*)_tmp5E2)!= 5)goto _LL435;_tmp5E3=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp5E2)->f1;if((int)_tmp5E3 != 1)goto _LL435;_tmp5E4=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp5E2)->f2;if((int)_tmp5E4 != 3)goto _LL435;_LL434: goto _LL436;_LL435: _tmp5E5=
_tmp5CF.f2;if(_tmp5E5 <= (void*)4)goto _LL437;if(*((int*)_tmp5E5)!= 5)goto _LL437;
_tmp5E6=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp5E5)->f1;if((int)_tmp5E6 != 
1)goto _LL437;_tmp5E7=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp5E5)->f2;if((
int)_tmp5E7 != 3)goto _LL437;_LL436: return Cyc_Absyn_ulong_typ;_LL437: _tmp5E8=
_tmp5CF.f1;if(_tmp5E8 <= (void*)4)goto _LL439;if(*((int*)_tmp5E8)!= 5)goto _LL439;
_tmp5E9=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp5E8)->f1;if((int)_tmp5E9 != 
1)goto _LL439;_tmp5EA=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp5E8)->f2;if((
int)_tmp5EA != 2)goto _LL439;_LL438: goto _LL43A;_LL439: _tmp5EB=_tmp5CF.f2;if(
_tmp5EB <= (void*)4)goto _LL43B;if(*((int*)_tmp5EB)!= 5)goto _LL43B;_tmp5EC=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp5EB)->f1;if((int)_tmp5EC != 1)goto _LL43B;
_tmp5ED=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp5EB)->f2;if((int)_tmp5ED != 
2)goto _LL43B;_LL43A: return Cyc_Absyn_uint_typ;_LL43B: _tmp5EE=_tmp5CF.f1;if(
_tmp5EE <= (void*)4)goto _LL43D;if(*((int*)_tmp5EE)!= 5)goto _LL43D;_tmp5EF=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp5EE)->f2;if((int)_tmp5EF != 3)goto _LL43D;
_LL43C: goto _LL43E;_LL43D: _tmp5F0=_tmp5CF.f2;if(_tmp5F0 <= (void*)4)goto _LL43F;if(*((
int*)_tmp5F0)!= 5)goto _LL43F;_tmp5F1=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp5F0)->f2;if((int)_tmp5F1 != 3)goto _LL43F;_LL43E: return Cyc_Absyn_slong_typ;
_LL43F:;_LL440: return Cyc_Absyn_sint_typ;_LL420:;}void Cyc_Tcutil_check_contains_assign(
struct Cyc_Absyn_Exp*e);void Cyc_Tcutil_check_contains_assign(struct Cyc_Absyn_Exp*
e){void*_tmp5F2=(void*)e->r;struct Cyc_Core_Opt*_tmp5F3;_LL442: if(*((int*)_tmp5F2)
!= 4)goto _LL444;_tmp5F3=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp5F2)->f2;if(
_tmp5F3 != 0)goto _LL444;_LL443:{const char*_tmpDAE;void*_tmpDAD;(_tmpDAD=0,Cyc_Tcutil_warn(
e->loc,((_tmpDAE="assignment in test",_tag_dyneither(_tmpDAE,sizeof(char),19))),
_tag_dyneither(_tmpDAD,sizeof(void*),0)));}goto _LL441;_LL444:;_LL445: goto _LL441;
_LL441:;}static int Cyc_Tcutil_constrain_kinds(void*c1,void*c2);static int Cyc_Tcutil_constrain_kinds(
void*c1,void*c2){c1=Cyc_Absyn_compress_kb(c1);c2=Cyc_Absyn_compress_kb(c2);{
struct _tuple0 _tmpDAF;struct _tuple0 _tmp5F7=(_tmpDAF.f1=c1,((_tmpDAF.f2=c2,_tmpDAF)));
void*_tmp5F8;void*_tmp5F9;void*_tmp5FA;void*_tmp5FB;void*_tmp5FC;struct Cyc_Core_Opt*
_tmp5FD;struct Cyc_Core_Opt**_tmp5FE;void*_tmp5FF;struct Cyc_Core_Opt*_tmp600;
struct Cyc_Core_Opt**_tmp601;void*_tmp602;struct Cyc_Core_Opt*_tmp603;struct Cyc_Core_Opt**
_tmp604;void*_tmp605;void*_tmp606;void*_tmp607;void*_tmp608;void*_tmp609;void*
_tmp60A;struct Cyc_Core_Opt*_tmp60B;struct Cyc_Core_Opt**_tmp60C;void*_tmp60D;void*
_tmp60E;struct Cyc_Core_Opt*_tmp60F;struct Cyc_Core_Opt**_tmp610;void*_tmp611;void*
_tmp612;struct Cyc_Core_Opt*_tmp613;struct Cyc_Core_Opt**_tmp614;void*_tmp615;
_LL447: _tmp5F8=_tmp5F7.f1;if(*((int*)_tmp5F8)!= 0)goto _LL449;_tmp5F9=(void*)((
struct Cyc_Absyn_Eq_kb_struct*)_tmp5F8)->f1;_tmp5FA=_tmp5F7.f2;if(*((int*)_tmp5FA)
!= 0)goto _LL449;_tmp5FB=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp5FA)->f1;
_LL448: return _tmp5F9 == _tmp5FB;_LL449: _tmp5FC=_tmp5F7.f2;if(*((int*)_tmp5FC)!= 1)
goto _LL44B;_tmp5FD=((struct Cyc_Absyn_Unknown_kb_struct*)_tmp5FC)->f1;_tmp5FE=(
struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)_tmp5FC)->f1;_LL44A:{
struct Cyc_Core_Opt*_tmpDB0;*_tmp5FE=((_tmpDB0=_cycalloc(sizeof(*_tmpDB0)),((
_tmpDB0->v=(void*)c1,_tmpDB0))));}return 1;_LL44B: _tmp5FF=_tmp5F7.f1;if(*((int*)
_tmp5FF)!= 1)goto _LL44D;_tmp600=((struct Cyc_Absyn_Unknown_kb_struct*)_tmp5FF)->f1;
_tmp601=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)_tmp5FF)->f1;
_LL44C:{struct Cyc_Core_Opt*_tmpDB1;*_tmp601=((_tmpDB1=_cycalloc(sizeof(*_tmpDB1)),((
_tmpDB1->v=(void*)c2,_tmpDB1))));}return 1;_LL44D: _tmp602=_tmp5F7.f1;if(*((int*)
_tmp602)!= 2)goto _LL44F;_tmp603=((struct Cyc_Absyn_Less_kb_struct*)_tmp602)->f1;
_tmp604=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)_tmp602)->f1;
_tmp605=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp602)->f2;_tmp606=_tmp5F7.f2;
if(*((int*)_tmp606)!= 0)goto _LL44F;_tmp607=(void*)((struct Cyc_Absyn_Eq_kb_struct*)
_tmp606)->f1;_LL44E: if(Cyc_Tcutil_kind_leq(_tmp607,_tmp605)){{struct Cyc_Core_Opt*
_tmpDB2;*_tmp604=((_tmpDB2=_cycalloc(sizeof(*_tmpDB2)),((_tmpDB2->v=(void*)c2,
_tmpDB2))));}return 1;}else{return 0;}_LL44F: _tmp608=_tmp5F7.f1;if(*((int*)_tmp608)
!= 0)goto _LL451;_tmp609=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp608)->f1;
_tmp60A=_tmp5F7.f2;if(*((int*)_tmp60A)!= 2)goto _LL451;_tmp60B=((struct Cyc_Absyn_Less_kb_struct*)
_tmp60A)->f1;_tmp60C=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp60A)->f1;_tmp60D=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp60A)->f2;
_LL450: if(Cyc_Tcutil_kind_leq(_tmp609,_tmp60D)){{struct Cyc_Core_Opt*_tmpDB3;*
_tmp60C=((_tmpDB3=_cycalloc(sizeof(*_tmpDB3)),((_tmpDB3->v=(void*)c1,_tmpDB3))));}
return 1;}else{return 0;}_LL451: _tmp60E=_tmp5F7.f1;if(*((int*)_tmp60E)!= 2)goto
_LL446;_tmp60F=((struct Cyc_Absyn_Less_kb_struct*)_tmp60E)->f1;_tmp610=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*)_tmp60E)->f1;_tmp611=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp60E)->f2;_tmp612=_tmp5F7.f2;if(*((int*)_tmp612)!= 2)goto _LL446;_tmp613=((
struct Cyc_Absyn_Less_kb_struct*)_tmp612)->f1;_tmp614=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*)_tmp612)->f1;_tmp615=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp612)->f2;_LL452: if(Cyc_Tcutil_kind_leq(_tmp611,_tmp615)){{struct Cyc_Core_Opt*
_tmpDB4;*_tmp614=((_tmpDB4=_cycalloc(sizeof(*_tmpDB4)),((_tmpDB4->v=(void*)c1,
_tmpDB4))));}return 1;}else{if(Cyc_Tcutil_kind_leq(_tmp615,_tmp611)){{struct Cyc_Core_Opt*
_tmpDB5;*_tmp610=((_tmpDB5=_cycalloc(sizeof(*_tmpDB5)),((_tmpDB5->v=(void*)c2,
_tmpDB5))));}return 1;}else{return 0;}}_LL446:;}}static int Cyc_Tcutil_tvar_id_counter=
0;int Cyc_Tcutil_new_tvar_id();int Cyc_Tcutil_new_tvar_id(){return Cyc_Tcutil_tvar_id_counter
++;}static int Cyc_Tcutil_tvar_counter=0;struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(
void*k);struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k){int i=Cyc_Tcutil_tvar_counter
++;const char*_tmpDB9;void*_tmpDB8[1];struct Cyc_Int_pa_struct _tmpDB7;struct
_dyneither_ptr s=(struct _dyneither_ptr)((_tmpDB7.tag=1,((_tmpDB7.f1=(
unsigned long)i,((_tmpDB8[0]=& _tmpDB7,Cyc_aprintf(((_tmpDB9="#%d",_tag_dyneither(
_tmpDB9,sizeof(char),4))),_tag_dyneither(_tmpDB8,sizeof(void*),1))))))));struct
_dyneither_ptr*_tmpDBC;struct Cyc_Absyn_Tvar*_tmpDBB;return(_tmpDBB=_cycalloc(
sizeof(*_tmpDBB)),((_tmpDBB->name=((_tmpDBC=_cycalloc(sizeof(struct
_dyneither_ptr)* 1),((_tmpDBC[0]=s,_tmpDBC)))),((_tmpDBB->identity=- 1,((_tmpDBB->kind=(
void*)k,_tmpDBB)))))));}int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*t);int
Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*t){struct _dyneither_ptr _tmp621=*t->name;
return*((const char*)_check_dyneither_subscript(_tmp621,sizeof(char),0))== '#';}
void Cyc_Tcutil_rewrite_temp_tvar(struct Cyc_Absyn_Tvar*t);void Cyc_Tcutil_rewrite_temp_tvar(
struct Cyc_Absyn_Tvar*t){{const char*_tmpDC0;void*_tmpDBF[1];struct Cyc_String_pa_struct
_tmpDBE;(_tmpDBE.tag=0,((_tmpDBE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*
t->name),((_tmpDBF[0]=& _tmpDBE,Cyc_printf(((_tmpDC0="%s",_tag_dyneither(_tmpDC0,
sizeof(char),3))),_tag_dyneither(_tmpDBF,sizeof(void*),1)))))));}if(!Cyc_Tcutil_is_temp_tvar(
t))return;{const char*_tmpDC1;struct _dyneither_ptr _tmp625=Cyc_strconcat(((_tmpDC1="`",
_tag_dyneither(_tmpDC1,sizeof(char),2))),(struct _dyneither_ptr)*t->name);{char
_tmpDC4;char _tmpDC3;struct _dyneither_ptr _tmpDC2;(_tmpDC2=_dyneither_ptr_plus(
_tmp625,sizeof(char),1),((_tmpDC3=*((char*)_check_dyneither_subscript(_tmpDC2,
sizeof(char),0)),((_tmpDC4='t',((_get_dyneither_size(_tmpDC2,sizeof(char))== 1
 && (_tmpDC3 == '\000'  && _tmpDC4 != '\000')?_throw_arraybounds(): 1,*((char*)
_tmpDC2.curr)=_tmpDC4)))))));}{struct _dyneither_ptr*_tmpDC5;t->name=((_tmpDC5=
_cycalloc(sizeof(struct _dyneither_ptr)* 1),((_tmpDC5[0]=(struct _dyneither_ptr)
_tmp625,_tmpDC5))));}}}struct _tuple14{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual
f2;void*f3;};static struct _tuple3*Cyc_Tcutil_fndecl2typ_f(struct _tuple14*x);
static struct _tuple3*Cyc_Tcutil_fndecl2typ_f(struct _tuple14*x){struct Cyc_Core_Opt*
_tmpDC8;struct _tuple3*_tmpDC7;return(_tmpDC7=_cycalloc(sizeof(*_tmpDC7)),((
_tmpDC7->f1=(struct Cyc_Core_Opt*)((_tmpDC8=_cycalloc(sizeof(*_tmpDC8)),((_tmpDC8->v=(*
x).f1,_tmpDC8)))),((_tmpDC7->f2=(*x).f2,((_tmpDC7->f3=(*x).f3,_tmpDC7)))))));}
void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*fd);void*Cyc_Tcutil_fndecl2typ(
struct Cyc_Absyn_Fndecl*fd){if(fd->cached_typ == 0){struct Cyc_List_List*_tmp62D=0;{
struct Cyc_List_List*atts=fd->attributes;for(0;atts != 0;atts=atts->tl){if(Cyc_Absyn_fntype_att((
void*)atts->hd)){struct Cyc_List_List*_tmpDC9;_tmp62D=((_tmpDC9=_cycalloc(sizeof(*
_tmpDC9)),((_tmpDC9->hd=(void*)((void*)atts->hd),((_tmpDC9->tl=_tmp62D,_tmpDC9))))));}}}{
struct Cyc_Absyn_FnType_struct _tmpDCF;struct Cyc_Absyn_FnInfo _tmpDCE;struct Cyc_Absyn_FnType_struct*
_tmpDCD;return(void*)((_tmpDCD=_cycalloc(sizeof(*_tmpDCD)),((_tmpDCD[0]=((
_tmpDCF.tag=8,((_tmpDCF.f1=((_tmpDCE.tvars=fd->tvs,((_tmpDCE.effect=fd->effect,((
_tmpDCE.ret_typ=(void*)((void*)fd->ret_type),((_tmpDCE.args=((struct Cyc_List_List*(*)(
struct _tuple3*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_fndecl2typ_f,
fd->args),((_tmpDCE.c_varargs=fd->c_varargs,((_tmpDCE.cyc_varargs=fd->cyc_varargs,((
_tmpDCE.rgn_po=fd->rgn_po,((_tmpDCE.attributes=_tmp62D,_tmpDCE)))))))))))))))),
_tmpDCF)))),_tmpDCD))));}}return(void*)((struct Cyc_Core_Opt*)_check_null(fd->cached_typ))->v;}
struct _tuple15{void*f1;struct Cyc_Absyn_Tqual f2;void*f3;};static void*Cyc_Tcutil_fst_fdarg(
struct _tuple15*t);static void*Cyc_Tcutil_fst_fdarg(struct _tuple15*t){return(*t).f1;}
void*Cyc_Tcutil_snd_tqt(struct _tuple5*t);void*Cyc_Tcutil_snd_tqt(struct _tuple5*t){
return(*t).f2;}static struct _tuple5*Cyc_Tcutil_map2_tq(struct _tuple5*pr,void*t);
static struct _tuple5*Cyc_Tcutil_map2_tq(struct _tuple5*pr,void*t){struct _tuple5*
_tmpDD0;return(_tmpDD0=_cycalloc(sizeof(*_tmpDD0)),((_tmpDD0->f1=(*pr).f1,((
_tmpDD0->f2=t,_tmpDD0)))));}struct _tuple16{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Tqual
f2;};struct _tuple17{struct _tuple16*f1;void*f2;};static struct _tuple17*Cyc_Tcutil_substitute_f1(
struct _RegionHandle*rgn,struct _tuple3*y);static struct _tuple17*Cyc_Tcutil_substitute_f1(
struct _RegionHandle*rgn,struct _tuple3*y){struct _tuple16*_tmpDD3;struct _tuple17*
_tmpDD2;return(_tmpDD2=_region_malloc(rgn,sizeof(*_tmpDD2)),((_tmpDD2->f1=((
_tmpDD3=_region_malloc(rgn,sizeof(*_tmpDD3)),((_tmpDD3->f1=(*y).f1,((_tmpDD3->f2=(*
y).f2,_tmpDD3)))))),((_tmpDD2->f2=(*y).f3,_tmpDD2)))));}static struct _tuple3*Cyc_Tcutil_substitute_f2(
struct _tuple17*w);static struct _tuple3*Cyc_Tcutil_substitute_f2(struct _tuple17*w){
struct _tuple16*_tmp636;void*_tmp637;struct _tuple17 _tmp635=*w;_tmp636=_tmp635.f1;
_tmp637=_tmp635.f2;{struct Cyc_Core_Opt*_tmp639;struct Cyc_Absyn_Tqual _tmp63A;
struct _tuple16 _tmp638=*_tmp636;_tmp639=_tmp638.f1;_tmp63A=_tmp638.f2;{struct
_tuple3*_tmpDD4;return(_tmpDD4=_cycalloc(sizeof(*_tmpDD4)),((_tmpDD4->f1=_tmp639,((
_tmpDD4->f2=_tmp63A,((_tmpDD4->f3=_tmp637,_tmpDD4)))))));}}}static void*Cyc_Tcutil_field_type(
struct Cyc_Absyn_Aggrfield*f);static void*Cyc_Tcutil_field_type(struct Cyc_Absyn_Aggrfield*
f){return(void*)f->type;}static struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_zip_field_type(
struct Cyc_Absyn_Aggrfield*f,void*t);static struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_zip_field_type(
struct Cyc_Absyn_Aggrfield*f,void*t){struct Cyc_Absyn_Aggrfield*_tmpDD5;return(
_tmpDD5=_cycalloc(sizeof(*_tmpDD5)),((_tmpDD5->name=f->name,((_tmpDD5->tq=f->tq,((
_tmpDD5->type=(void*)t,((_tmpDD5->width=f->width,((_tmpDD5->attributes=f->attributes,
_tmpDD5)))))))))));}static struct Cyc_List_List*Cyc_Tcutil_substs(struct
_RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*ts);static struct
Cyc_Absyn_Exp*Cyc_Tcutil_copye(struct Cyc_Absyn_Exp*old,void*r);static struct Cyc_Absyn_Exp*
Cyc_Tcutil_copye(struct Cyc_Absyn_Exp*old,void*r){struct Cyc_Absyn_Exp*_tmpDD6;
return(_tmpDD6=_cycalloc(sizeof(*_tmpDD6)),((_tmpDD6->topt=old->topt,((_tmpDD6->r=(
void*)r,((_tmpDD6->loc=old->loc,((_tmpDD6->annot=(void*)((void*)old->annot),
_tmpDD6)))))))));}static struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubsexp(struct
_RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_Absyn_Exp*e);static struct Cyc_Absyn_Exp*
Cyc_Tcutil_rsubsexp(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_Absyn_Exp*
e){void*_tmp63E=(void*)e->r;void*_tmp63F;struct Cyc_List_List*_tmp640;struct Cyc_Absyn_Exp*
_tmp641;struct Cyc_Absyn_Exp*_tmp642;struct Cyc_Absyn_Exp*_tmp643;struct Cyc_Absyn_Exp*
_tmp644;struct Cyc_Absyn_Exp*_tmp645;struct Cyc_Absyn_Exp*_tmp646;struct Cyc_Absyn_Exp*
_tmp647;struct Cyc_Absyn_Exp*_tmp648;struct Cyc_Absyn_Exp*_tmp649;void*_tmp64A;
struct Cyc_Absyn_Exp*_tmp64B;int _tmp64C;void*_tmp64D;void*_tmp64E;struct Cyc_Absyn_Exp*
_tmp64F;void*_tmp650;void*_tmp651;void*_tmp652;_LL454: if(*((int*)_tmp63E)!= 0)
goto _LL456;_LL455: goto _LL457;_LL456: if(*((int*)_tmp63E)!= 33)goto _LL458;_LL457:
goto _LL459;_LL458: if(*((int*)_tmp63E)!= 34)goto _LL45A;_LL459: goto _LL45B;_LL45A:
if(*((int*)_tmp63E)!= 1)goto _LL45C;_LL45B: return e;_LL45C: if(*((int*)_tmp63E)!= 3)
goto _LL45E;_tmp63F=(void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp63E)->f1;
_tmp640=((struct Cyc_Absyn_Primop_e_struct*)_tmp63E)->f2;_LL45D: if(((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmp640)== 1){struct Cyc_Absyn_Exp*
_tmp653=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp640))->hd;
struct Cyc_Absyn_Exp*_tmp654=Cyc_Tcutil_rsubsexp(r,inst,_tmp653);if(_tmp654 == 
_tmp653)return e;{struct Cyc_Absyn_Primop_e_struct _tmpDDC;struct Cyc_Absyn_Exp*
_tmpDDB[1];struct Cyc_Absyn_Primop_e_struct*_tmpDDA;return Cyc_Tcutil_copye(e,(
void*)((_tmpDDA=_cycalloc(sizeof(*_tmpDDA)),((_tmpDDA[0]=((_tmpDDC.tag=3,((
_tmpDDC.f1=(void*)_tmp63F,((_tmpDDC.f2=((_tmpDDB[0]=_tmp654,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpDDB,sizeof(struct Cyc_Absyn_Exp*),
1)))),_tmpDDC)))))),_tmpDDA)))));}}else{if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp640)== 2){struct Cyc_Absyn_Exp*_tmp658=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp640))->hd;struct Cyc_Absyn_Exp*_tmp659=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp640->tl))->hd;struct Cyc_Absyn_Exp*_tmp65A=
Cyc_Tcutil_rsubsexp(r,inst,_tmp658);struct Cyc_Absyn_Exp*_tmp65B=Cyc_Tcutil_rsubsexp(
r,inst,_tmp659);if(_tmp65A == _tmp658  && _tmp65B == _tmp659)return e;{struct Cyc_Absyn_Primop_e_struct
_tmpDE2;struct Cyc_Absyn_Exp*_tmpDE1[2];struct Cyc_Absyn_Primop_e_struct*_tmpDE0;
return Cyc_Tcutil_copye(e,(void*)((_tmpDE0=_cycalloc(sizeof(*_tmpDE0)),((_tmpDE0[
0]=((_tmpDE2.tag=3,((_tmpDE2.f1=(void*)_tmp63F,((_tmpDE2.f2=((_tmpDE1[1]=_tmp65B,((
_tmpDE1[0]=_tmp65A,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpDE1,sizeof(struct Cyc_Absyn_Exp*),2)))))),_tmpDE2)))))),
_tmpDE0)))));}}else{const char*_tmpDE5;void*_tmpDE4;return(_tmpDE4=0,((struct Cyc_Absyn_Exp*(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpDE5="primop does not have 1 or 2 args!",
_tag_dyneither(_tmpDE5,sizeof(char),34))),_tag_dyneither(_tmpDE4,sizeof(void*),0)));}}
_LL45E: if(*((int*)_tmp63E)!= 6)goto _LL460;_tmp641=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp63E)->f1;_tmp642=((struct Cyc_Absyn_Conditional_e_struct*)_tmp63E)->f2;
_tmp643=((struct Cyc_Absyn_Conditional_e_struct*)_tmp63E)->f3;_LL45F: {struct Cyc_Absyn_Exp*
_tmp661=Cyc_Tcutil_rsubsexp(r,inst,_tmp641);struct Cyc_Absyn_Exp*_tmp662=Cyc_Tcutil_rsubsexp(
r,inst,_tmp642);struct Cyc_Absyn_Exp*_tmp663=Cyc_Tcutil_rsubsexp(r,inst,_tmp643);
if((_tmp661 == _tmp641  && _tmp662 == _tmp642) && _tmp663 == _tmp643)return e;{struct
Cyc_Absyn_Conditional_e_struct _tmpDE8;struct Cyc_Absyn_Conditional_e_struct*
_tmpDE7;return Cyc_Tcutil_copye(e,(void*)((_tmpDE7=_cycalloc(sizeof(*_tmpDE7)),((
_tmpDE7[0]=((_tmpDE8.tag=6,((_tmpDE8.f1=_tmp661,((_tmpDE8.f2=_tmp662,((_tmpDE8.f3=
_tmp663,_tmpDE8)))))))),_tmpDE7)))));}}_LL460: if(*((int*)_tmp63E)!= 7)goto _LL462;
_tmp644=((struct Cyc_Absyn_And_e_struct*)_tmp63E)->f1;_tmp645=((struct Cyc_Absyn_And_e_struct*)
_tmp63E)->f2;_LL461: {struct Cyc_Absyn_Exp*_tmp666=Cyc_Tcutil_rsubsexp(r,inst,
_tmp644);struct Cyc_Absyn_Exp*_tmp667=Cyc_Tcutil_rsubsexp(r,inst,_tmp645);if(
_tmp666 == _tmp644  && _tmp667 == _tmp645)return e;{struct Cyc_Absyn_And_e_struct
_tmpDEB;struct Cyc_Absyn_And_e_struct*_tmpDEA;return Cyc_Tcutil_copye(e,(void*)((
_tmpDEA=_cycalloc(sizeof(*_tmpDEA)),((_tmpDEA[0]=((_tmpDEB.tag=7,((_tmpDEB.f1=
_tmp666,((_tmpDEB.f2=_tmp667,_tmpDEB)))))),_tmpDEA)))));}}_LL462: if(*((int*)
_tmp63E)!= 8)goto _LL464;_tmp646=((struct Cyc_Absyn_Or_e_struct*)_tmp63E)->f1;
_tmp647=((struct Cyc_Absyn_Or_e_struct*)_tmp63E)->f2;_LL463: {struct Cyc_Absyn_Exp*
_tmp66A=Cyc_Tcutil_rsubsexp(r,inst,_tmp646);struct Cyc_Absyn_Exp*_tmp66B=Cyc_Tcutil_rsubsexp(
r,inst,_tmp647);if(_tmp66A == _tmp646  && _tmp66B == _tmp647)return e;{struct Cyc_Absyn_Or_e_struct
_tmpDEE;struct Cyc_Absyn_Or_e_struct*_tmpDED;return Cyc_Tcutil_copye(e,(void*)((
_tmpDED=_cycalloc(sizeof(*_tmpDED)),((_tmpDED[0]=((_tmpDEE.tag=8,((_tmpDEE.f1=
_tmp66A,((_tmpDEE.f2=_tmp66B,_tmpDEE)))))),_tmpDED)))));}}_LL464: if(*((int*)
_tmp63E)!= 9)goto _LL466;_tmp648=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp63E)->f1;
_tmp649=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp63E)->f2;_LL465: {struct Cyc_Absyn_Exp*
_tmp66E=Cyc_Tcutil_rsubsexp(r,inst,_tmp648);struct Cyc_Absyn_Exp*_tmp66F=Cyc_Tcutil_rsubsexp(
r,inst,_tmp649);if(_tmp66E == _tmp648  && _tmp66F == _tmp649)return e;{struct Cyc_Absyn_SeqExp_e_struct
_tmpDF1;struct Cyc_Absyn_SeqExp_e_struct*_tmpDF0;return Cyc_Tcutil_copye(e,(void*)((
_tmpDF0=_cycalloc(sizeof(*_tmpDF0)),((_tmpDF0[0]=((_tmpDF1.tag=9,((_tmpDF1.f1=
_tmp66E,((_tmpDF1.f2=_tmp66F,_tmpDF1)))))),_tmpDF0)))));}}_LL466: if(*((int*)
_tmp63E)!= 15)goto _LL468;_tmp64A=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp63E)->f1;
_tmp64B=((struct Cyc_Absyn_Cast_e_struct*)_tmp63E)->f2;_tmp64C=((struct Cyc_Absyn_Cast_e_struct*)
_tmp63E)->f3;_tmp64D=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp63E)->f4;_LL467: {
struct Cyc_Absyn_Exp*_tmp672=Cyc_Tcutil_rsubsexp(r,inst,_tmp64B);void*_tmp673=Cyc_Tcutil_rsubstitute(
r,inst,_tmp64A);if(_tmp672 == _tmp64B  && _tmp673 == _tmp64A)return e;{struct Cyc_Absyn_Cast_e_struct
_tmpDF4;struct Cyc_Absyn_Cast_e_struct*_tmpDF3;return Cyc_Tcutil_copye(e,(void*)((
_tmpDF3=_cycalloc(sizeof(*_tmpDF3)),((_tmpDF3[0]=((_tmpDF4.tag=15,((_tmpDF4.f1=(
void*)_tmp673,((_tmpDF4.f2=_tmp672,((_tmpDF4.f3=_tmp64C,((_tmpDF4.f4=(void*)
_tmp64D,_tmpDF4)))))))))),_tmpDF3)))));}}_LL468: if(*((int*)_tmp63E)!= 18)goto
_LL46A;_tmp64E=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp63E)->f1;_LL469: {
void*_tmp676=Cyc_Tcutil_rsubstitute(r,inst,_tmp64E);if(_tmp676 == _tmp64E)return e;{
struct Cyc_Absyn_Sizeoftyp_e_struct _tmpDF7;struct Cyc_Absyn_Sizeoftyp_e_struct*
_tmpDF6;return Cyc_Tcutil_copye(e,(void*)((_tmpDF6=_cycalloc(sizeof(*_tmpDF6)),((
_tmpDF6[0]=((_tmpDF7.tag=18,((_tmpDF7.f1=(void*)_tmp676,_tmpDF7)))),_tmpDF6)))));}}
_LL46A: if(*((int*)_tmp63E)!= 19)goto _LL46C;_tmp64F=((struct Cyc_Absyn_Sizeofexp_e_struct*)
_tmp63E)->f1;_LL46B: {struct Cyc_Absyn_Exp*_tmp679=Cyc_Tcutil_rsubsexp(r,inst,
_tmp64F);if(_tmp679 == _tmp64F)return e;{struct Cyc_Absyn_Sizeofexp_e_struct _tmpDFA;
struct Cyc_Absyn_Sizeofexp_e_struct*_tmpDF9;return Cyc_Tcutil_copye(e,(void*)((
_tmpDF9=_cycalloc(sizeof(*_tmpDF9)),((_tmpDF9[0]=((_tmpDFA.tag=19,((_tmpDFA.f1=
_tmp679,_tmpDFA)))),_tmpDF9)))));}}_LL46C: if(*((int*)_tmp63E)!= 20)goto _LL46E;
_tmp650=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp63E)->f1;_tmp651=(void*)((
struct Cyc_Absyn_Offsetof_e_struct*)_tmp63E)->f2;_LL46D: {void*_tmp67C=Cyc_Tcutil_rsubstitute(
r,inst,_tmp650);if(_tmp67C == _tmp650)return e;{struct Cyc_Absyn_Offsetof_e_struct
_tmpDFD;struct Cyc_Absyn_Offsetof_e_struct*_tmpDFC;return Cyc_Tcutil_copye(e,(void*)((
_tmpDFC=_cycalloc(sizeof(*_tmpDFC)),((_tmpDFC[0]=((_tmpDFD.tag=20,((_tmpDFD.f1=(
void*)_tmp67C,((_tmpDFD.f2=(void*)_tmp651,_tmpDFD)))))),_tmpDFC)))));}}_LL46E:
if(*((int*)_tmp63E)!= 39)goto _LL470;_tmp652=(void*)((struct Cyc_Absyn_Valueof_e_struct*)
_tmp63E)->f1;_LL46F: {void*_tmp67F=Cyc_Tcutil_rsubstitute(r,inst,_tmp652);if(
_tmp67F == _tmp652)return e;{struct Cyc_Absyn_Valueof_e_struct _tmpE00;struct Cyc_Absyn_Valueof_e_struct*
_tmpDFF;return Cyc_Tcutil_copye(e,(void*)((_tmpDFF=_cycalloc(sizeof(*_tmpDFF)),((
_tmpDFF[0]=((_tmpE00.tag=39,((_tmpE00.f1=(void*)_tmp67F,_tmpE00)))),_tmpDFF)))));}}
_LL470:;_LL471: {const char*_tmpE03;void*_tmpE02;return(_tmpE02=0,((struct Cyc_Absyn_Exp*(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpE03="non-type-level-expression in Tcutil::rsubsexp",
_tag_dyneither(_tmpE03,sizeof(char),46))),_tag_dyneither(_tmpE02,sizeof(void*),0)));}
_LL453:;}void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*rgn,struct Cyc_List_List*
inst,void*t);void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*rgn,struct Cyc_List_List*
inst,void*t){void*_tmp684=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*_tmp685;
struct Cyc_Absyn_AggrInfo _tmp686;union Cyc_Absyn_AggrInfoU_union _tmp687;struct Cyc_List_List*
_tmp688;struct Cyc_Absyn_TunionInfo _tmp689;union Cyc_Absyn_TunionInfoU_union
_tmp68A;struct Cyc_List_List*_tmp68B;struct Cyc_Core_Opt*_tmp68C;struct Cyc_Absyn_TunionFieldInfo
_tmp68D;union Cyc_Absyn_TunionFieldInfoU_union _tmp68E;struct Cyc_List_List*_tmp68F;
struct _tuple2*_tmp690;struct Cyc_List_List*_tmp691;struct Cyc_Absyn_Typedefdecl*
_tmp692;void**_tmp693;struct Cyc_Absyn_ArrayInfo _tmp694;void*_tmp695;struct Cyc_Absyn_Tqual
_tmp696;struct Cyc_Absyn_Exp*_tmp697;struct Cyc_Absyn_Conref*_tmp698;struct Cyc_Position_Segment*
_tmp699;struct Cyc_Absyn_PtrInfo _tmp69A;void*_tmp69B;struct Cyc_Absyn_Tqual _tmp69C;
struct Cyc_Absyn_PtrAtts _tmp69D;void*_tmp69E;struct Cyc_Absyn_Conref*_tmp69F;
struct Cyc_Absyn_Conref*_tmp6A0;struct Cyc_Absyn_Conref*_tmp6A1;struct Cyc_Absyn_FnInfo
_tmp6A2;struct Cyc_List_List*_tmp6A3;struct Cyc_Core_Opt*_tmp6A4;void*_tmp6A5;
struct Cyc_List_List*_tmp6A6;int _tmp6A7;struct Cyc_Absyn_VarargInfo*_tmp6A8;struct
Cyc_List_List*_tmp6A9;struct Cyc_List_List*_tmp6AA;struct Cyc_List_List*_tmp6AB;
void*_tmp6AC;struct Cyc_List_List*_tmp6AD;struct Cyc_Core_Opt*_tmp6AE;void*_tmp6AF;
void*_tmp6B0;void*_tmp6B1;void*_tmp6B2;struct Cyc_Absyn_Exp*_tmp6B3;void*_tmp6B4;
void*_tmp6B5;struct Cyc_List_List*_tmp6B6;_LL473: if(_tmp684 <= (void*)4)goto _LL495;
if(*((int*)_tmp684)!= 1)goto _LL475;_tmp685=((struct Cyc_Absyn_VarType_struct*)
_tmp684)->f1;_LL474: {struct _handler_cons _tmp6B7;_push_handler(& _tmp6B7);{int
_tmp6B9=0;if(setjmp(_tmp6B7.handler))_tmp6B9=1;if(!_tmp6B9){{void*_tmp6BA=((void*(*)(
int(*cmp)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,
struct Cyc_Absyn_Tvar*x))Cyc_List_assoc_cmp)(Cyc_Absyn_tvar_cmp,inst,_tmp685);
_npop_handler(0);return _tmp6BA;};_pop_handler();}else{void*_tmp6B8=(void*)
_exn_thrown;void*_tmp6BC=_tmp6B8;_LL4A8: if(_tmp6BC != Cyc_Core_Not_found)goto
_LL4AA;_LL4A9: return t;_LL4AA:;_LL4AB:(void)_throw(_tmp6BC);_LL4A7:;}}}_LL475: if(*((
int*)_tmp684)!= 10)goto _LL477;_tmp686=((struct Cyc_Absyn_AggrType_struct*)_tmp684)->f1;
_tmp687=_tmp686.aggr_info;_tmp688=_tmp686.targs;_LL476: {struct Cyc_List_List*
_tmp6BD=Cyc_Tcutil_substs(rgn,inst,_tmp688);struct Cyc_Absyn_AggrType_struct
_tmpE09;struct Cyc_Absyn_AggrInfo _tmpE08;struct Cyc_Absyn_AggrType_struct*_tmpE07;
return _tmp6BD == _tmp688?t:(void*)((_tmpE07=_cycalloc(sizeof(*_tmpE07)),((_tmpE07[
0]=((_tmpE09.tag=10,((_tmpE09.f1=((_tmpE08.aggr_info=_tmp687,((_tmpE08.targs=
_tmp6BD,_tmpE08)))),_tmpE09)))),_tmpE07))));}_LL477: if(*((int*)_tmp684)!= 2)goto
_LL479;_tmp689=((struct Cyc_Absyn_TunionType_struct*)_tmp684)->f1;_tmp68A=_tmp689.tunion_info;
_tmp68B=_tmp689.targs;_tmp68C=_tmp689.rgn;_LL478: {struct Cyc_List_List*_tmp6C1=
Cyc_Tcutil_substs(rgn,inst,_tmp68B);struct Cyc_Core_Opt*new_r;if((unsigned int)
_tmp68C){void*_tmp6C2=Cyc_Tcutil_rsubstitute(rgn,inst,(void*)_tmp68C->v);if(
_tmp6C2 == (void*)_tmp68C->v)new_r=_tmp68C;else{struct Cyc_Core_Opt*_tmpE0A;new_r=((
_tmpE0A=_cycalloc(sizeof(*_tmpE0A)),((_tmpE0A->v=(void*)_tmp6C2,_tmpE0A))));}}
else{new_r=_tmp68C;}{struct Cyc_Absyn_TunionType_struct _tmpE10;struct Cyc_Absyn_TunionInfo
_tmpE0F;struct Cyc_Absyn_TunionType_struct*_tmpE0E;return _tmp6C1 == _tmp68B  && 
new_r == _tmp68C?t:(void*)((_tmpE0E=_cycalloc(sizeof(*_tmpE0E)),((_tmpE0E[0]=((
_tmpE10.tag=2,((_tmpE10.f1=((_tmpE0F.tunion_info=_tmp68A,((_tmpE0F.targs=_tmp6C1,((
_tmpE0F.rgn=new_r,_tmpE0F)))))),_tmpE10)))),_tmpE0E))));}}_LL479: if(*((int*)
_tmp684)!= 3)goto _LL47B;_tmp68D=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp684)->f1;_tmp68E=_tmp68D.field_info;_tmp68F=_tmp68D.targs;_LL47A: {struct Cyc_List_List*
_tmp6C7=Cyc_Tcutil_substs(rgn,inst,_tmp68F);struct Cyc_Absyn_TunionFieldType_struct
_tmpE16;struct Cyc_Absyn_TunionFieldInfo _tmpE15;struct Cyc_Absyn_TunionFieldType_struct*
_tmpE14;return _tmp6C7 == _tmp68F?t:(void*)((_tmpE14=_cycalloc(sizeof(*_tmpE14)),((
_tmpE14[0]=((_tmpE16.tag=3,((_tmpE16.f1=((_tmpE15.field_info=_tmp68E,((_tmpE15.targs=
_tmp6C7,_tmpE15)))),_tmpE16)))),_tmpE14))));}_LL47B: if(*((int*)_tmp684)!= 16)
goto _LL47D;_tmp690=((struct Cyc_Absyn_TypedefType_struct*)_tmp684)->f1;_tmp691=((
struct Cyc_Absyn_TypedefType_struct*)_tmp684)->f2;_tmp692=((struct Cyc_Absyn_TypedefType_struct*)
_tmp684)->f3;_tmp693=((struct Cyc_Absyn_TypedefType_struct*)_tmp684)->f4;_LL47C: {
struct Cyc_List_List*_tmp6CB=Cyc_Tcutil_substs(rgn,inst,_tmp691);struct Cyc_Absyn_TypedefType_struct
_tmpE19;struct Cyc_Absyn_TypedefType_struct*_tmpE18;return _tmp6CB == _tmp691?t:(
void*)((_tmpE18=_cycalloc(sizeof(*_tmpE18)),((_tmpE18[0]=((_tmpE19.tag=16,((
_tmpE19.f1=_tmp690,((_tmpE19.f2=_tmp6CB,((_tmpE19.f3=_tmp692,((_tmpE19.f4=
_tmp693,_tmpE19)))))))))),_tmpE18))));}_LL47D: if(*((int*)_tmp684)!= 7)goto _LL47F;
_tmp694=((struct Cyc_Absyn_ArrayType_struct*)_tmp684)->f1;_tmp695=(void*)_tmp694.elt_type;
_tmp696=_tmp694.tq;_tmp697=_tmp694.num_elts;_tmp698=_tmp694.zero_term;_tmp699=
_tmp694.zt_loc;_LL47E: {void*_tmp6CE=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp695);
struct Cyc_Absyn_ArrayType_struct _tmpE1F;struct Cyc_Absyn_ArrayInfo _tmpE1E;struct
Cyc_Absyn_ArrayType_struct*_tmpE1D;return _tmp6CE == _tmp695?t:(void*)((_tmpE1D=
_cycalloc(sizeof(*_tmpE1D)),((_tmpE1D[0]=((_tmpE1F.tag=7,((_tmpE1F.f1=((_tmpE1E.elt_type=(
void*)_tmp6CE,((_tmpE1E.tq=_tmp696,((_tmpE1E.num_elts=_tmp697,((_tmpE1E.zero_term=
_tmp698,((_tmpE1E.zt_loc=_tmp699,_tmpE1E)))))))))),_tmpE1F)))),_tmpE1D))));}
_LL47F: if(*((int*)_tmp684)!= 4)goto _LL481;_tmp69A=((struct Cyc_Absyn_PointerType_struct*)
_tmp684)->f1;_tmp69B=(void*)_tmp69A.elt_typ;_tmp69C=_tmp69A.elt_tq;_tmp69D=
_tmp69A.ptr_atts;_tmp69E=(void*)_tmp69D.rgn;_tmp69F=_tmp69D.nullable;_tmp6A0=
_tmp69D.bounds;_tmp6A1=_tmp69D.zero_term;_LL480: {void*_tmp6D2=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmp69B);void*_tmp6D3=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp69E);struct
Cyc_Absyn_Conref*_tmp6D4=_tmp6A0;{union Cyc_Absyn_Constraint_union _tmp6D5=(Cyc_Absyn_compress_conref(
_tmp6A0))->v;void*_tmp6D6;struct Cyc_Absyn_Exp*_tmp6D7;_LL4AD: if((_tmp6D5.Eq_constr).tag
!= 0)goto _LL4AF;_tmp6D6=(_tmp6D5.Eq_constr).f1;if(_tmp6D6 <= (void*)1)goto _LL4AF;
if(*((int*)_tmp6D6)!= 0)goto _LL4AF;_tmp6D7=((struct Cyc_Absyn_Upper_b_struct*)
_tmp6D6)->f1;_LL4AE: {struct Cyc_Absyn_Exp*_tmp6D8=Cyc_Tcutil_rsubsexp(rgn,inst,
_tmp6D7);if(_tmp6D8 != _tmp6D7){struct Cyc_Absyn_Upper_b_struct _tmpE22;struct Cyc_Absyn_Upper_b_struct*
_tmpE21;_tmp6D4=Cyc_Absyn_new_conref((void*)((_tmpE21=_cycalloc(sizeof(*_tmpE21)),((
_tmpE21[0]=((_tmpE22.tag=0,((_tmpE22.f1=_tmp6D8,_tmpE22)))),_tmpE21)))));}goto
_LL4AC;}_LL4AF:;_LL4B0: goto _LL4AC;_LL4AC:;}if((_tmp6D2 == _tmp69B  && _tmp6D3 == 
_tmp69E) && _tmp6D4 == _tmp6A0)return t;{struct Cyc_Absyn_PointerType_struct _tmpE2C;
struct Cyc_Absyn_PtrAtts _tmpE2B;struct Cyc_Absyn_PtrInfo _tmpE2A;struct Cyc_Absyn_PointerType_struct*
_tmpE29;return(void*)((_tmpE29=_cycalloc(sizeof(*_tmpE29)),((_tmpE29[0]=((
_tmpE2C.tag=4,((_tmpE2C.f1=((_tmpE2A.elt_typ=(void*)_tmp6D2,((_tmpE2A.elt_tq=
_tmp69C,((_tmpE2A.ptr_atts=((_tmpE2B.rgn=(void*)_tmp6D3,((_tmpE2B.nullable=
_tmp69F,((_tmpE2B.bounds=_tmp6D4,((_tmpE2B.zero_term=_tmp6A1,((_tmpE2B.ptrloc=0,
_tmpE2B)))))))))),_tmpE2A)))))),_tmpE2C)))),_tmpE29))));}}_LL481: if(*((int*)
_tmp684)!= 8)goto _LL483;_tmp6A2=((struct Cyc_Absyn_FnType_struct*)_tmp684)->f1;
_tmp6A3=_tmp6A2.tvars;_tmp6A4=_tmp6A2.effect;_tmp6A5=(void*)_tmp6A2.ret_typ;
_tmp6A6=_tmp6A2.args;_tmp6A7=_tmp6A2.c_varargs;_tmp6A8=_tmp6A2.cyc_varargs;
_tmp6A9=_tmp6A2.rgn_po;_tmp6AA=_tmp6A2.attributes;_LL482:{struct Cyc_List_List*
_tmp6DF=_tmp6A3;for(0;_tmp6DF != 0;_tmp6DF=_tmp6DF->tl){struct _tuple8*_tmpE36;
struct Cyc_Absyn_VarType_struct _tmpE35;struct Cyc_Absyn_VarType_struct*_tmpE34;
struct Cyc_List_List*_tmpE33;inst=((_tmpE33=_region_malloc(rgn,sizeof(*_tmpE33)),((
_tmpE33->hd=((_tmpE36=_region_malloc(rgn,sizeof(*_tmpE36)),((_tmpE36->f1=(struct
Cyc_Absyn_Tvar*)_tmp6DF->hd,((_tmpE36->f2=(void*)((_tmpE34=_cycalloc(sizeof(*
_tmpE34)),((_tmpE34[0]=((_tmpE35.tag=1,((_tmpE35.f1=(struct Cyc_Absyn_Tvar*)
_tmp6DF->hd,_tmpE35)))),_tmpE34)))),_tmpE36)))))),((_tmpE33->tl=inst,_tmpE33))))));}}{
struct Cyc_List_List*_tmp6E5;struct Cyc_List_List*_tmp6E6;struct _tuple1 _tmp6E4=((
struct _tuple1(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x))Cyc_List_rsplit)(rgn,rgn,((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct _tuple17*(*f)(struct _RegionHandle*,struct _tuple3*),struct _RegionHandle*env,
struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_substitute_f1,rgn,_tmp6A6));
_tmp6E5=_tmp6E4.f1;_tmp6E6=_tmp6E4.f2;{struct Cyc_List_List*_tmp6E7=Cyc_Tcutil_substs(
rgn,inst,_tmp6E6);struct Cyc_List_List*_tmp6E8=((struct Cyc_List_List*(*)(struct
_tuple3*(*f)(struct _tuple17*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_substitute_f2,((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmp6E5,_tmp6E7));struct Cyc_Core_Opt*
eff2;if(_tmp6A4 == 0)eff2=0;else{void*_tmp6E9=Cyc_Tcutil_rsubstitute(rgn,inst,(
void*)_tmp6A4->v);if(_tmp6E9 == (void*)_tmp6A4->v)eff2=_tmp6A4;else{struct Cyc_Core_Opt*
_tmpE37;eff2=((_tmpE37=_cycalloc(sizeof(*_tmpE37)),((_tmpE37->v=(void*)_tmp6E9,
_tmpE37))));}}{struct Cyc_Absyn_VarargInfo*cyc_varargs2;if(_tmp6A8 == 0)
cyc_varargs2=0;else{struct Cyc_Core_Opt*_tmp6EC;struct Cyc_Absyn_Tqual _tmp6ED;void*
_tmp6EE;int _tmp6EF;struct Cyc_Absyn_VarargInfo _tmp6EB=*_tmp6A8;_tmp6EC=_tmp6EB.name;
_tmp6ED=_tmp6EB.tq;_tmp6EE=(void*)_tmp6EB.type;_tmp6EF=_tmp6EB.inject;{void*
_tmp6F0=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6EE);if(_tmp6F0 == _tmp6EE)
cyc_varargs2=_tmp6A8;else{struct Cyc_Absyn_VarargInfo*_tmpE38;cyc_varargs2=((
_tmpE38=_cycalloc(sizeof(*_tmpE38)),((_tmpE38->name=_tmp6EC,((_tmpE38->tq=
_tmp6ED,((_tmpE38->type=(void*)_tmp6F0,((_tmpE38->inject=_tmp6EF,_tmpE38))))))))));}}}{
struct Cyc_List_List*rgn_po2;struct Cyc_List_List*_tmp6F3;struct Cyc_List_List*
_tmp6F4;struct _tuple1 _tmp6F2=Cyc_List_rsplit(rgn,rgn,_tmp6A9);_tmp6F3=_tmp6F2.f1;
_tmp6F4=_tmp6F2.f2;{struct Cyc_List_List*_tmp6F5=Cyc_Tcutil_substs(rgn,inst,
_tmp6F3);struct Cyc_List_List*_tmp6F6=Cyc_Tcutil_substs(rgn,inst,_tmp6F4);if(
_tmp6F5 == _tmp6F3  && _tmp6F6 == _tmp6F4)rgn_po2=_tmp6A9;else{rgn_po2=Cyc_List_zip(
_tmp6F5,_tmp6F6);}{struct Cyc_Absyn_FnType_struct _tmpE3E;struct Cyc_Absyn_FnInfo
_tmpE3D;struct Cyc_Absyn_FnType_struct*_tmpE3C;return(void*)((_tmpE3C=_cycalloc(
sizeof(*_tmpE3C)),((_tmpE3C[0]=((_tmpE3E.tag=8,((_tmpE3E.f1=((_tmpE3D.tvars=
_tmp6A3,((_tmpE3D.effect=eff2,((_tmpE3D.ret_typ=(void*)Cyc_Tcutil_rsubstitute(
rgn,inst,_tmp6A5),((_tmpE3D.args=_tmp6E8,((_tmpE3D.c_varargs=_tmp6A7,((_tmpE3D.cyc_varargs=
cyc_varargs2,((_tmpE3D.rgn_po=rgn_po2,((_tmpE3D.attributes=_tmp6AA,_tmpE3D)))))))))))))))),
_tmpE3E)))),_tmpE3C))));}}}}}}_LL483: if(*((int*)_tmp684)!= 9)goto _LL485;_tmp6AB=((
struct Cyc_Absyn_TupleType_struct*)_tmp684)->f1;_LL484: {struct Cyc_List_List*
_tmp6FA=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _tuple5*),
struct Cyc_List_List*x))Cyc_List_rmap)(rgn,Cyc_Tcutil_snd_tqt,_tmp6AB);struct Cyc_List_List*
_tmp6FB=Cyc_Tcutil_substs(rgn,inst,_tmp6FA);if(_tmp6FB == _tmp6FA)return t;{struct
Cyc_List_List*_tmp6FC=((struct Cyc_List_List*(*)(struct _tuple5*(*f)(struct _tuple5*,
void*),struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_map2_tq,
_tmp6AB,_tmp6FB);struct Cyc_Absyn_TupleType_struct _tmpE41;struct Cyc_Absyn_TupleType_struct*
_tmpE40;return(void*)((_tmpE40=_cycalloc(sizeof(*_tmpE40)),((_tmpE40[0]=((
_tmpE41.tag=9,((_tmpE41.f1=_tmp6FC,_tmpE41)))),_tmpE40))));}}_LL485: if(*((int*)
_tmp684)!= 11)goto _LL487;_tmp6AC=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp684)->f1;_tmp6AD=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp684)->f2;_LL486: {
struct Cyc_List_List*_tmp6FF=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*
f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_rmap)(rgn,Cyc_Tcutil_field_type,
_tmp6AD);struct Cyc_List_List*_tmp700=Cyc_Tcutil_substs(rgn,inst,_tmp6FF);if(
_tmp700 == _tmp6FF)return t;{struct Cyc_List_List*_tmp701=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*,void*),struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_zip_field_type,_tmp6AD,
_tmp700);struct Cyc_Absyn_AnonAggrType_struct _tmpE44;struct Cyc_Absyn_AnonAggrType_struct*
_tmpE43;return(void*)((_tmpE43=_cycalloc(sizeof(*_tmpE43)),((_tmpE43[0]=((
_tmpE44.tag=11,((_tmpE44.f1=(void*)_tmp6AC,((_tmpE44.f2=_tmp701,_tmpE44)))))),
_tmpE43))));}}_LL487: if(*((int*)_tmp684)!= 0)goto _LL489;_tmp6AE=((struct Cyc_Absyn_Evar_struct*)
_tmp684)->f2;_LL488: if(_tmp6AE != 0)return Cyc_Tcutil_rsubstitute(rgn,inst,(void*)
_tmp6AE->v);else{return t;}_LL489: if(*((int*)_tmp684)!= 14)goto _LL48B;_tmp6AF=(
void*)((struct Cyc_Absyn_RgnHandleType_struct*)_tmp684)->f1;_LL48A: {void*_tmp704=
Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6AF);struct Cyc_Absyn_RgnHandleType_struct
_tmpE47;struct Cyc_Absyn_RgnHandleType_struct*_tmpE46;return _tmp704 == _tmp6AF?t:(
void*)((_tmpE46=_cycalloc(sizeof(*_tmpE46)),((_tmpE46[0]=((_tmpE47.tag=14,((
_tmpE47.f1=(void*)_tmp704,_tmpE47)))),_tmpE46))));}_LL48B: if(*((int*)_tmp684)!= 
15)goto _LL48D;_tmp6B0=(void*)((struct Cyc_Absyn_DynRgnType_struct*)_tmp684)->f1;
_tmp6B1=(void*)((struct Cyc_Absyn_DynRgnType_struct*)_tmp684)->f2;_LL48C: {void*
_tmp707=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6B0);void*_tmp708=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmp6B1);struct Cyc_Absyn_DynRgnType_struct _tmpE4A;struct Cyc_Absyn_DynRgnType_struct*
_tmpE49;return _tmp707 == _tmp6B0  && _tmp708 == _tmp6B1?t:(void*)((_tmpE49=
_cycalloc(sizeof(*_tmpE49)),((_tmpE49[0]=((_tmpE4A.tag=15,((_tmpE4A.f1=(void*)
_tmp707,((_tmpE4A.f2=(void*)_tmp708,_tmpE4A)))))),_tmpE49))));}_LL48D: if(*((int*)
_tmp684)!= 18)goto _LL48F;_tmp6B2=(void*)((struct Cyc_Absyn_TagType_struct*)
_tmp684)->f1;_LL48E: {void*_tmp70B=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6B2);
struct Cyc_Absyn_TagType_struct _tmpE4D;struct Cyc_Absyn_TagType_struct*_tmpE4C;
return _tmp70B == _tmp6B2?t:(void*)((_tmpE4C=_cycalloc(sizeof(*_tmpE4C)),((_tmpE4C[
0]=((_tmpE4D.tag=18,((_tmpE4D.f1=(void*)_tmp70B,_tmpE4D)))),_tmpE4C))));}_LL48F:
if(*((int*)_tmp684)!= 17)goto _LL491;_tmp6B3=((struct Cyc_Absyn_ValueofType_struct*)
_tmp684)->f1;_LL490: {struct Cyc_Absyn_Exp*_tmp70E=Cyc_Tcutil_rsubsexp(rgn,inst,
_tmp6B3);struct Cyc_Absyn_ValueofType_struct _tmpE50;struct Cyc_Absyn_ValueofType_struct*
_tmpE4F;return _tmp70E == _tmp6B3?t:(void*)((_tmpE4F=_cycalloc(sizeof(*_tmpE4F)),((
_tmpE4F[0]=((_tmpE50.tag=17,((_tmpE50.f1=_tmp70E,_tmpE50)))),_tmpE4F))));}_LL491:
if(*((int*)_tmp684)!= 12)goto _LL493;_LL492: goto _LL494;_LL493: if(*((int*)_tmp684)
!= 13)goto _LL495;_LL494: goto _LL496;_LL495: if((int)_tmp684 != 0)goto _LL497;_LL496:
goto _LL498;_LL497: if(_tmp684 <= (void*)4)goto _LL499;if(*((int*)_tmp684)!= 5)goto
_LL499;_LL498: goto _LL49A;_LL499: if((int)_tmp684 != 1)goto _LL49B;_LL49A: goto _LL49C;
_LL49B: if(_tmp684 <= (void*)4)goto _LL49D;if(*((int*)_tmp684)!= 6)goto _LL49D;
_LL49C: goto _LL49E;_LL49D: if((int)_tmp684 != 3)goto _LL49F;_LL49E: goto _LL4A0;_LL49F:
if((int)_tmp684 != 2)goto _LL4A1;_LL4A0: return t;_LL4A1: if(_tmp684 <= (void*)4)goto
_LL4A3;if(*((int*)_tmp684)!= 21)goto _LL4A3;_tmp6B4=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp684)->f1;_LL4A2: {void*_tmp711=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6B4);
struct Cyc_Absyn_RgnsEff_struct _tmpE53;struct Cyc_Absyn_RgnsEff_struct*_tmpE52;
return _tmp711 == _tmp6B4?t:(void*)((_tmpE52=_cycalloc(sizeof(*_tmpE52)),((_tmpE52[
0]=((_tmpE53.tag=21,((_tmpE53.f1=(void*)_tmp711,_tmpE53)))),_tmpE52))));}_LL4A3:
if(_tmp684 <= (void*)4)goto _LL4A5;if(*((int*)_tmp684)!= 19)goto _LL4A5;_tmp6B5=(
void*)((struct Cyc_Absyn_AccessEff_struct*)_tmp684)->f1;_LL4A4: {void*_tmp714=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmp6B5);struct Cyc_Absyn_AccessEff_struct _tmpE56;struct Cyc_Absyn_AccessEff_struct*
_tmpE55;return _tmp714 == _tmp6B5?t:(void*)((_tmpE55=_cycalloc(sizeof(*_tmpE55)),((
_tmpE55[0]=((_tmpE56.tag=19,((_tmpE56.f1=(void*)_tmp714,_tmpE56)))),_tmpE55))));}
_LL4A5: if(_tmp684 <= (void*)4)goto _LL472;if(*((int*)_tmp684)!= 20)goto _LL472;
_tmp6B6=((struct Cyc_Absyn_JoinEff_struct*)_tmp684)->f1;_LL4A6: {struct Cyc_List_List*
_tmp717=Cyc_Tcutil_substs(rgn,inst,_tmp6B6);struct Cyc_Absyn_JoinEff_struct
_tmpE59;struct Cyc_Absyn_JoinEff_struct*_tmpE58;return _tmp717 == _tmp6B6?t:(void*)((
_tmpE58=_cycalloc(sizeof(*_tmpE58)),((_tmpE58[0]=((_tmpE59.tag=20,((_tmpE59.f1=
_tmp717,_tmpE59)))),_tmpE58))));}_LL472:;}static struct Cyc_List_List*Cyc_Tcutil_substs(
struct _RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*ts);static
struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*rgn,struct Cyc_List_List*
inst,struct Cyc_List_List*ts){if(ts == 0)return 0;{void*_tmp71A=(void*)ts->hd;
struct Cyc_List_List*_tmp71B=ts->tl;void*_tmp71C=Cyc_Tcutil_rsubstitute(rgn,inst,
_tmp71A);struct Cyc_List_List*_tmp71D=Cyc_Tcutil_substs(rgn,inst,_tmp71B);if(
_tmp71A == _tmp71C  && _tmp71B == _tmp71D)return ts;{struct Cyc_List_List*_tmpE5A;
return(struct Cyc_List_List*)((struct Cyc_List_List*)((_tmpE5A=_cycalloc(sizeof(*
_tmpE5A)),((_tmpE5A->hd=(void*)_tmp71C,((_tmpE5A->tl=_tmp71D,_tmpE5A)))))));}}}
void*Cyc_Tcutil_substitute(struct Cyc_List_List*inst,void*t);extern void*Cyc_Tcutil_substitute(
struct Cyc_List_List*inst,void*t){return Cyc_Tcutil_rsubstitute(Cyc_Core_heap_region,
inst,t);}struct _tuple8*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*s,struct Cyc_Absyn_Tvar*
tv);struct _tuple8*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*s,struct Cyc_Absyn_Tvar*
tv){struct Cyc_Core_Opt*_tmp71F=Cyc_Tcutil_kind_to_opt(Cyc_Tcutil_tvar_kind(tv));
struct Cyc_Core_Opt*_tmpE5D;struct _tuple8*_tmpE5C;return(_tmpE5C=_cycalloc(
sizeof(*_tmpE5C)),((_tmpE5C->f1=tv,((_tmpE5C->f2=Cyc_Absyn_new_evar(_tmp71F,((
_tmpE5D=_cycalloc(sizeof(*_tmpE5D)),((_tmpE5D->v=s,_tmpE5D))))),_tmpE5C)))));}
struct _tuple8*Cyc_Tcutil_r_make_inst_var(struct _tuple9*env,struct Cyc_Absyn_Tvar*
tv);struct _tuple8*Cyc_Tcutil_r_make_inst_var(struct _tuple9*env,struct Cyc_Absyn_Tvar*
tv){struct _tuple9 _tmp723;struct Cyc_List_List*_tmp724;struct _RegionHandle*_tmp725;
struct _tuple9*_tmp722=env;_tmp723=*_tmp722;_tmp724=_tmp723.f1;_tmp725=_tmp723.f2;{
struct Cyc_Core_Opt*_tmp726=Cyc_Tcutil_kind_to_opt(Cyc_Tcutil_tvar_kind(tv));
struct Cyc_Core_Opt*_tmpE60;struct _tuple8*_tmpE5F;return(_tmpE5F=_region_malloc(
_tmp725,sizeof(*_tmpE5F)),((_tmpE5F->f1=tv,((_tmpE5F->f2=Cyc_Absyn_new_evar(
_tmp726,((_tmpE60=_cycalloc(sizeof(*_tmpE60)),((_tmpE60->v=_tmp724,_tmpE60))))),
_tmpE5F)))));}}static struct Cyc_List_List*Cyc_Tcutil_add_free_tvar(struct Cyc_Position_Segment*
loc,struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv);static struct Cyc_List_List*
Cyc_Tcutil_add_free_tvar(struct Cyc_Position_Segment*loc,struct Cyc_List_List*tvs,
struct Cyc_Absyn_Tvar*tv){{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
if(Cyc_strptrcmp(((struct Cyc_Absyn_Tvar*)tvs2->hd)->name,tv->name)== 0){void*k1=(
void*)((struct Cyc_Absyn_Tvar*)tvs2->hd)->kind;void*k2=(void*)tv->kind;if(!Cyc_Tcutil_constrain_kinds(
k1,k2)){const char*_tmpE66;void*_tmpE65[3];struct Cyc_String_pa_struct _tmpE64;
struct Cyc_String_pa_struct _tmpE63;struct Cyc_String_pa_struct _tmpE62;(_tmpE62.tag=
0,((_tmpE62.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kindbound2string(
k2)),((_tmpE63.tag=0,((_tmpE63.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kindbound2string(k1)),((_tmpE64.tag=0,((_tmpE64.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*tv->name),((_tmpE65[0]=& _tmpE64,((_tmpE65[
1]=& _tmpE63,((_tmpE65[2]=& _tmpE62,Cyc_Tcutil_terr(loc,((_tmpE66="type variable %s is used with inconsistent kinds %s and %s",
_tag_dyneither(_tmpE66,sizeof(char),59))),_tag_dyneither(_tmpE65,sizeof(void*),3)))))))))))))))))));}
if(tv->identity == - 1)tv->identity=((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity;
else{if(tv->identity != ((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity){const char*
_tmpE69;void*_tmpE68;(_tmpE68=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpE69="same type variable has different identity!",
_tag_dyneither(_tmpE69,sizeof(char),43))),_tag_dyneither(_tmpE68,sizeof(void*),0)));}}
return tvs;}}}tv->identity=Cyc_Tcutil_new_tvar_id();{struct Cyc_List_List*_tmpE6A;
return(_tmpE6A=_cycalloc(sizeof(*_tmpE6A)),((_tmpE6A->hd=tv,((_tmpE6A->tl=tvs,
_tmpE6A)))));}}static struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar(struct Cyc_List_List*
tvs,struct Cyc_Absyn_Tvar*tv);static struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar(
struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){if(tv->identity == - 1){const char*
_tmpE6D;void*_tmpE6C;(_tmpE6C=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpE6D="fast_add_free_tvar: bad identity in tv",
_tag_dyneither(_tmpE6D,sizeof(char),39))),_tag_dyneither(_tmpE6C,sizeof(void*),0)));}{
struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){struct Cyc_Absyn_Tvar*
_tmp733=(struct Cyc_Absyn_Tvar*)tvs2->hd;if(_tmp733->identity == - 1){const char*
_tmpE70;void*_tmpE6F;(_tmpE6F=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpE70="fast_add_free_tvar: bad identity in tvs2",
_tag_dyneither(_tmpE70,sizeof(char),41))),_tag_dyneither(_tmpE6F,sizeof(void*),0)));}
if(_tmp733->identity == tv->identity)return tvs;}}{struct Cyc_List_List*_tmpE71;
return(_tmpE71=_cycalloc(sizeof(*_tmpE71)),((_tmpE71->hd=tv,((_tmpE71->tl=tvs,
_tmpE71)))));}}struct _tuple18{struct Cyc_Absyn_Tvar*f1;int f2;};static struct Cyc_List_List*
Cyc_Tcutil_fast_add_free_tvar_bool(struct _RegionHandle*r,struct Cyc_List_List*tvs,
struct Cyc_Absyn_Tvar*tv,int b);static struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar_bool(
struct _RegionHandle*r,struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv,int b){if(
tv->identity == - 1){const char*_tmpE74;void*_tmpE73;(_tmpE73=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpE74="fast_add_free_tvar_bool: bad identity in tv",
_tag_dyneither(_tmpE74,sizeof(char),44))),_tag_dyneither(_tmpE73,sizeof(void*),0)));}{
struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){struct _tuple18 _tmp73A;
struct Cyc_Absyn_Tvar*_tmp73B;int _tmp73C;int*_tmp73D;struct _tuple18*_tmp739=(
struct _tuple18*)tvs2->hd;_tmp73A=*_tmp739;_tmp73B=_tmp73A.f1;_tmp73C=_tmp73A.f2;
_tmp73D=(int*)&(*_tmp739).f2;if(_tmp73B->identity == - 1){const char*_tmpE77;void*
_tmpE76;(_tmpE76=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmpE77="fast_add_free_tvar_bool: bad identity in tvs2",_tag_dyneither(_tmpE77,
sizeof(char),46))),_tag_dyneither(_tmpE76,sizeof(void*),0)));}if(_tmp73B->identity
== tv->identity){*_tmp73D=*_tmp73D  || b;return tvs;}}}{struct _tuple18*_tmpE7A;
struct Cyc_List_List*_tmpE79;return(_tmpE79=_region_malloc(r,sizeof(*_tmpE79)),((
_tmpE79->hd=((_tmpE7A=_region_malloc(r,sizeof(*_tmpE7A)),((_tmpE7A->f1=tv,((
_tmpE7A->f2=b,_tmpE7A)))))),((_tmpE79->tl=tvs,_tmpE79)))));}}static struct Cyc_List_List*
Cyc_Tcutil_add_bound_tvar(struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv);
static struct Cyc_List_List*Cyc_Tcutil_add_bound_tvar(struct Cyc_List_List*tvs,
struct Cyc_Absyn_Tvar*tv){if(tv->identity == - 1){const char*_tmpE7E;void*_tmpE7D[1];
struct Cyc_String_pa_struct _tmpE7C;(_tmpE7C.tag=0,((_tmpE7C.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_tvar2string(tv)),((_tmpE7D[0]=&
_tmpE7C,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmpE7E="bound tvar id for %s is NULL",_tag_dyneither(_tmpE7E,sizeof(char),29))),
_tag_dyneither(_tmpE7D,sizeof(void*),1)))))));}{struct Cyc_List_List*_tmpE7F;
return(_tmpE7F=_cycalloc(sizeof(*_tmpE7F)),((_tmpE7F->hd=tv,((_tmpE7F->tl=tvs,
_tmpE7F)))));}}struct _tuple19{void*f1;int f2;};static struct Cyc_List_List*Cyc_Tcutil_add_free_evar(
struct _RegionHandle*r,struct Cyc_List_List*es,void*e,int b);static struct Cyc_List_List*
Cyc_Tcutil_add_free_evar(struct _RegionHandle*r,struct Cyc_List_List*es,void*e,int
b){void*_tmp746=Cyc_Tcutil_compress(e);int _tmp747;_LL4B2: if(_tmp746 <= (void*)4)
goto _LL4B4;if(*((int*)_tmp746)!= 0)goto _LL4B4;_tmp747=((struct Cyc_Absyn_Evar_struct*)
_tmp746)->f3;_LL4B3:{struct Cyc_List_List*es2=es;for(0;es2 != 0;es2=es2->tl){
struct _tuple19 _tmp749;void*_tmp74A;int _tmp74B;int*_tmp74C;struct _tuple19*_tmp748=(
struct _tuple19*)es2->hd;_tmp749=*_tmp748;_tmp74A=_tmp749.f1;_tmp74B=_tmp749.f2;
_tmp74C=(int*)&(*_tmp748).f2;{void*_tmp74D=Cyc_Tcutil_compress(_tmp74A);int
_tmp74E;_LL4B7: if(_tmp74D <= (void*)4)goto _LL4B9;if(*((int*)_tmp74D)!= 0)goto
_LL4B9;_tmp74E=((struct Cyc_Absyn_Evar_struct*)_tmp74D)->f3;_LL4B8: if(_tmp747 == 
_tmp74E){if(b != *_tmp74C)*_tmp74C=1;return es;}goto _LL4B6;_LL4B9:;_LL4BA: goto
_LL4B6;_LL4B6:;}}}{struct _tuple19*_tmpE82;struct Cyc_List_List*_tmpE81;return(
_tmpE81=_region_malloc(r,sizeof(*_tmpE81)),((_tmpE81->hd=((_tmpE82=
_region_malloc(r,sizeof(*_tmpE82)),((_tmpE82->f1=e,((_tmpE82->f2=b,_tmpE82)))))),((
_tmpE81->tl=es,_tmpE81)))));}_LL4B4:;_LL4B5: return es;_LL4B1:;}static struct Cyc_List_List*
Cyc_Tcutil_remove_bound_tvars(struct _RegionHandle*rgn,struct Cyc_List_List*tvs,
struct Cyc_List_List*btvs);static struct Cyc_List_List*Cyc_Tcutil_remove_bound_tvars(
struct _RegionHandle*rgn,struct Cyc_List_List*tvs,struct Cyc_List_List*btvs){struct
Cyc_List_List*r=0;for(0;tvs != 0;tvs=tvs->tl){int present=0;{struct Cyc_List_List*b=
btvs;for(0;b != 0;b=b->tl){if(((struct Cyc_Absyn_Tvar*)tvs->hd)->identity == ((
struct Cyc_Absyn_Tvar*)b->hd)->identity){present=1;break;}}}if(!present){struct
Cyc_List_List*_tmpE83;r=((_tmpE83=_region_malloc(rgn,sizeof(*_tmpE83)),((_tmpE83->hd=(
struct Cyc_Absyn_Tvar*)tvs->hd,((_tmpE83->tl=r,_tmpE83))))));}}r=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(r);return r;}static struct Cyc_List_List*
Cyc_Tcutil_remove_bound_tvars_bool(struct _RegionHandle*r,struct Cyc_List_List*tvs,
struct Cyc_List_List*btvs);static struct Cyc_List_List*Cyc_Tcutil_remove_bound_tvars_bool(
struct _RegionHandle*r,struct Cyc_List_List*tvs,struct Cyc_List_List*btvs){struct
Cyc_List_List*res=0;for(0;tvs != 0;tvs=tvs->tl){struct Cyc_Absyn_Tvar*_tmp753;int
_tmp754;struct _tuple18 _tmp752=*((struct _tuple18*)tvs->hd);_tmp753=_tmp752.f1;
_tmp754=_tmp752.f2;{int present=0;{struct Cyc_List_List*b=btvs;for(0;b != 0;b=b->tl){
if(_tmp753->identity == ((struct Cyc_Absyn_Tvar*)b->hd)->identity){present=1;
break;}}}if(!present){struct Cyc_List_List*_tmpE84;res=((_tmpE84=_region_malloc(r,
sizeof(*_tmpE84)),((_tmpE84->hd=(struct _tuple18*)tvs->hd,((_tmpE84->tl=res,
_tmpE84))))));}}}res=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
res);return res;}void Cyc_Tcutil_check_bitfield(struct Cyc_Position_Segment*loc,
struct Cyc_Tcenv_Tenv*te,void*field_typ,struct Cyc_Absyn_Exp*width,struct
_dyneither_ptr*fn);void Cyc_Tcutil_check_bitfield(struct Cyc_Position_Segment*loc,
struct Cyc_Tcenv_Tenv*te,void*field_typ,struct Cyc_Absyn_Exp*width,struct
_dyneither_ptr*fn){if(width != 0){unsigned int w=0;if(!Cyc_Tcutil_is_const_exp(te,(
struct Cyc_Absyn_Exp*)width)){const char*_tmpE88;void*_tmpE87[1];struct Cyc_String_pa_struct
_tmpE86;(_tmpE86.tag=0,((_tmpE86.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*
fn),((_tmpE87[0]=& _tmpE86,Cyc_Tcutil_terr(loc,((_tmpE88="bitfield %s does not have constant width",
_tag_dyneither(_tmpE88,sizeof(char),41))),_tag_dyneither(_tmpE87,sizeof(void*),1)))))));}
else{unsigned int _tmp75A;int _tmp75B;struct _tuple7 _tmp759=Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)width);_tmp75A=_tmp759.f1;_tmp75B=_tmp759.f2;if(!_tmp75B){
const char*_tmpE8B;void*_tmpE8A;(_tmpE8A=0,Cyc_Tcutil_terr(loc,((_tmpE8B="bitfield width cannot use sizeof or offsetof",
_tag_dyneither(_tmpE8B,sizeof(char),45))),_tag_dyneither(_tmpE8A,sizeof(void*),0)));}
w=_tmp75A;}{void*_tmp75E=Cyc_Tcutil_compress(field_typ);void*_tmp75F;_LL4BC: if(
_tmp75E <= (void*)4)goto _LL4BE;if(*((int*)_tmp75E)!= 5)goto _LL4BE;_tmp75F=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp75E)->f2;_LL4BD:{void*_tmp760=_tmp75F;_LL4C1:
if((int)_tmp760 != 0)goto _LL4C3;_LL4C2: if(w > 8){const char*_tmpE8E;void*_tmpE8D;(
_tmpE8D=0,Cyc_Tcutil_terr(loc,((_tmpE8E="bitfield larger than type",
_tag_dyneither(_tmpE8E,sizeof(char),26))),_tag_dyneither(_tmpE8D,sizeof(void*),0)));}
goto _LL4C0;_LL4C3: if((int)_tmp760 != 1)goto _LL4C5;_LL4C4: if(w > 16){const char*
_tmpE91;void*_tmpE90;(_tmpE90=0,Cyc_Tcutil_terr(loc,((_tmpE91="bitfield larger than type",
_tag_dyneither(_tmpE91,sizeof(char),26))),_tag_dyneither(_tmpE90,sizeof(void*),0)));}
goto _LL4C0;_LL4C5: if((int)_tmp760 != 3)goto _LL4C7;_LL4C6: goto _LL4C8;_LL4C7: if((
int)_tmp760 != 2)goto _LL4C9;_LL4C8: if(w > 32){const char*_tmpE94;void*_tmpE93;(
_tmpE93=0,Cyc_Tcutil_terr(loc,((_tmpE94="bitfield larger than type",
_tag_dyneither(_tmpE94,sizeof(char),26))),_tag_dyneither(_tmpE93,sizeof(void*),0)));}
goto _LL4C0;_LL4C9: if((int)_tmp760 != 4)goto _LL4C0;_LL4CA: if(w > 64){const char*
_tmpE97;void*_tmpE96;(_tmpE96=0,Cyc_Tcutil_terr(loc,((_tmpE97="bitfield larger than type",
_tag_dyneither(_tmpE97,sizeof(char),26))),_tag_dyneither(_tmpE96,sizeof(void*),0)));}
goto _LL4C0;_LL4C0:;}goto _LL4BB;_LL4BE:;_LL4BF:{const char*_tmpE9C;void*_tmpE9B[2];
struct Cyc_String_pa_struct _tmpE9A;struct Cyc_String_pa_struct _tmpE99;(_tmpE99.tag=
0,((_tmpE99.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
field_typ)),((_tmpE9A.tag=0,((_tmpE9A.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*fn),((_tmpE9B[0]=& _tmpE9A,((_tmpE9B[1]=& _tmpE99,Cyc_Tcutil_terr(
loc,((_tmpE9C="bitfield %s must have integral type but has type %s",
_tag_dyneither(_tmpE9C,sizeof(char),52))),_tag_dyneither(_tmpE9B,sizeof(void*),2)))))))))))));}
goto _LL4BB;_LL4BB:;}}}static void Cyc_Tcutil_check_field_atts(struct Cyc_Position_Segment*
loc,struct _dyneither_ptr*fn,struct Cyc_List_List*atts);static void Cyc_Tcutil_check_field_atts(
struct Cyc_Position_Segment*loc,struct _dyneither_ptr*fn,struct Cyc_List_List*atts){
for(0;atts != 0;atts=atts->tl){void*_tmp76D=(void*)atts->hd;_LL4CC: if((int)
_tmp76D != 5)goto _LL4CE;_LL4CD: continue;_LL4CE: if(_tmp76D <= (void*)17)goto _LL4D0;
if(*((int*)_tmp76D)!= 1)goto _LL4D0;_LL4CF: continue;_LL4D0:;_LL4D1: {const char*
_tmpEA1;void*_tmpEA0[2];struct Cyc_String_pa_struct _tmpE9F;struct Cyc_String_pa_struct
_tmpE9E;(_tmpE9E.tag=0,((_tmpE9E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*
fn),((_tmpE9F.tag=0,((_tmpE9F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absyn_attribute2string((void*)atts->hd)),((_tmpEA0[0]=& _tmpE9F,((_tmpEA0[1]=&
_tmpE9E,Cyc_Tcutil_terr(loc,((_tmpEA1="bad attribute %s on member %s",
_tag_dyneither(_tmpEA1,sizeof(char),30))),_tag_dyneither(_tmpEA0,sizeof(void*),2)))))))))))));}
_LL4CB:;}}struct Cyc_Tcutil_CVTEnv{struct _RegionHandle*r;struct Cyc_List_List*
kind_env;struct Cyc_List_List*free_vars;struct Cyc_List_List*free_evars;int
generalize_evars;int fn_result;};int Cyc_Tcutil_extract_const_from_typedef(struct
Cyc_Position_Segment*loc,int declared_const,void*t);int Cyc_Tcutil_extract_const_from_typedef(
struct Cyc_Position_Segment*loc,int declared_const,void*t){void*_tmp772=t;struct
Cyc_Absyn_Typedefdecl*_tmp773;void**_tmp774;_LL4D3: if(_tmp772 <= (void*)4)goto
_LL4D5;if(*((int*)_tmp772)!= 16)goto _LL4D5;_tmp773=((struct Cyc_Absyn_TypedefType_struct*)
_tmp772)->f3;_tmp774=((struct Cyc_Absyn_TypedefType_struct*)_tmp772)->f4;_LL4D4:
if((((struct Cyc_Absyn_Typedefdecl*)_check_null(_tmp773))->tq).real_const  || (
_tmp773->tq).print_const){if(declared_const){const char*_tmpEA4;void*_tmpEA3;(
_tmpEA3=0,Cyc_Tcutil_warn(loc,((_tmpEA4="extra const",_tag_dyneither(_tmpEA4,
sizeof(char),12))),_tag_dyneither(_tmpEA3,sizeof(void*),0)));}return 1;}if((
unsigned int)_tmp774)return Cyc_Tcutil_extract_const_from_typedef(loc,
declared_const,*_tmp774);else{return declared_const;}_LL4D5:;_LL4D6: return
declared_const;_LL4D2:;}static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type_level_exp(
struct Cyc_Absyn_Exp*e,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv);
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type(struct Cyc_Position_Segment*
loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv,void*expected_kind,
void*t,int put_in_effect);static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv
cvtenv,void*expected_kind,void*t,int put_in_effect){static struct Cyc_Core_Opt urgn={(
void*)((void*)3)};static struct Cyc_Core_Opt hrgn={(void*)((void*)2)};{void*_tmp777=
Cyc_Tcutil_compress(t);struct Cyc_Core_Opt*_tmp778;struct Cyc_Core_Opt**_tmp779;
struct Cyc_Core_Opt*_tmp77A;struct Cyc_Core_Opt**_tmp77B;struct Cyc_Absyn_Tvar*
_tmp77C;struct Cyc_List_List*_tmp77D;struct _tuple2*_tmp77E;struct Cyc_Absyn_Enumdecl*
_tmp77F;struct Cyc_Absyn_Enumdecl**_tmp780;struct Cyc_Absyn_TunionInfo _tmp781;
union Cyc_Absyn_TunionInfoU_union _tmp782;union Cyc_Absyn_TunionInfoU_union*_tmp783;
struct Cyc_List_List*_tmp784;struct Cyc_List_List**_tmp785;struct Cyc_Core_Opt*
_tmp786;struct Cyc_Core_Opt**_tmp787;struct Cyc_Absyn_TunionFieldInfo _tmp788;union
Cyc_Absyn_TunionFieldInfoU_union _tmp789;union Cyc_Absyn_TunionFieldInfoU_union*
_tmp78A;struct Cyc_List_List*_tmp78B;struct Cyc_Absyn_PtrInfo _tmp78C;void*_tmp78D;
struct Cyc_Absyn_Tqual _tmp78E;struct Cyc_Absyn_Tqual*_tmp78F;struct Cyc_Absyn_PtrAtts
_tmp790;void*_tmp791;struct Cyc_Absyn_Conref*_tmp792;struct Cyc_Absyn_Conref*
_tmp793;struct Cyc_Absyn_Conref*_tmp794;void*_tmp795;struct Cyc_Absyn_Exp*_tmp796;
struct Cyc_Absyn_ArrayInfo _tmp797;void*_tmp798;struct Cyc_Absyn_Tqual _tmp799;
struct Cyc_Absyn_Tqual*_tmp79A;struct Cyc_Absyn_Exp*_tmp79B;struct Cyc_Absyn_Exp**
_tmp79C;struct Cyc_Absyn_Conref*_tmp79D;struct Cyc_Position_Segment*_tmp79E;struct
Cyc_Absyn_FnInfo _tmp79F;struct Cyc_List_List*_tmp7A0;struct Cyc_List_List**_tmp7A1;
struct Cyc_Core_Opt*_tmp7A2;struct Cyc_Core_Opt**_tmp7A3;void*_tmp7A4;struct Cyc_List_List*
_tmp7A5;int _tmp7A6;struct Cyc_Absyn_VarargInfo*_tmp7A7;struct Cyc_List_List*
_tmp7A8;struct Cyc_List_List*_tmp7A9;struct Cyc_List_List*_tmp7AA;void*_tmp7AB;
struct Cyc_List_List*_tmp7AC;struct Cyc_Absyn_AggrInfo _tmp7AD;union Cyc_Absyn_AggrInfoU_union
_tmp7AE;union Cyc_Absyn_AggrInfoU_union*_tmp7AF;struct Cyc_List_List*_tmp7B0;
struct Cyc_List_List**_tmp7B1;struct _tuple2*_tmp7B2;struct Cyc_List_List*_tmp7B3;
struct Cyc_List_List**_tmp7B4;struct Cyc_Absyn_Typedefdecl*_tmp7B5;struct Cyc_Absyn_Typedefdecl**
_tmp7B6;void**_tmp7B7;void***_tmp7B8;void*_tmp7B9;void*_tmp7BA;void*_tmp7BB;void*
_tmp7BC;void*_tmp7BD;struct Cyc_List_List*_tmp7BE;_LL4D8: if((int)_tmp777 != 0)goto
_LL4DA;_LL4D9: goto _LL4D7;_LL4DA: if(_tmp777 <= (void*)4)goto _LL4EE;if(*((int*)
_tmp777)!= 0)goto _LL4DC;_tmp778=((struct Cyc_Absyn_Evar_struct*)_tmp777)->f1;
_tmp779=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)_tmp777)->f1;
_tmp77A=((struct Cyc_Absyn_Evar_struct*)_tmp777)->f2;_tmp77B=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Evar_struct*)_tmp777)->f2;_LL4DB: if(*_tmp779 == 0)*_tmp779=Cyc_Tcutil_kind_to_opt(
expected_kind);if((cvtenv.fn_result  && cvtenv.generalize_evars) && Cyc_Tcutil_is_region_kind(
expected_kind)){if(expected_kind == (void*)4)*_tmp77B=(struct Cyc_Core_Opt*)& urgn;
else{*_tmp77B=(struct Cyc_Core_Opt*)& hrgn;}}else{if(cvtenv.generalize_evars){
struct Cyc_Absyn_Less_kb_struct _tmpEA7;struct Cyc_Absyn_Less_kb_struct*_tmpEA6;
struct Cyc_Absyn_Tvar*_tmp7BF=Cyc_Tcutil_new_tvar((void*)((_tmpEA6=_cycalloc(
sizeof(*_tmpEA6)),((_tmpEA6[0]=((_tmpEA7.tag=2,((_tmpEA7.f1=0,((_tmpEA7.f2=(void*)
expected_kind,_tmpEA7)))))),_tmpEA6)))));{struct Cyc_Absyn_VarType_struct*_tmpEAD;
struct Cyc_Absyn_VarType_struct _tmpEAC;struct Cyc_Core_Opt*_tmpEAB;*_tmp77B=((
_tmpEAB=_cycalloc(sizeof(*_tmpEAB)),((_tmpEAB->v=(void*)((void*)((_tmpEAD=
_cycalloc(sizeof(*_tmpEAD)),((_tmpEAD[0]=((_tmpEAC.tag=1,((_tmpEAC.f1=_tmp7BF,
_tmpEAC)))),_tmpEAD))))),_tmpEAB))));}_tmp77C=_tmp7BF;goto _LL4DD;}else{cvtenv.free_evars=
Cyc_Tcutil_add_free_evar(cvtenv.r,cvtenv.free_evars,t,put_in_effect);}}goto
_LL4D7;_LL4DC: if(*((int*)_tmp777)!= 1)goto _LL4DE;_tmp77C=((struct Cyc_Absyn_VarType_struct*)
_tmp777)->f1;_LL4DD:{void*_tmp7C5=Cyc_Absyn_compress_kb((void*)_tmp77C->kind);
struct Cyc_Core_Opt*_tmp7C6;struct Cyc_Core_Opt**_tmp7C7;_LL50D: if(*((int*)_tmp7C5)
!= 1)goto _LL50F;_tmp7C6=((struct Cyc_Absyn_Unknown_kb_struct*)_tmp7C5)->f1;
_tmp7C7=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)_tmp7C5)->f1;
_LL50E:{struct Cyc_Absyn_Less_kb_struct*_tmpEB3;struct Cyc_Absyn_Less_kb_struct
_tmpEB2;struct Cyc_Core_Opt*_tmpEB1;*_tmp7C7=((_tmpEB1=_cycalloc(sizeof(*_tmpEB1)),((
_tmpEB1->v=(void*)((void*)((_tmpEB3=_cycalloc(sizeof(*_tmpEB3)),((_tmpEB3[0]=((
_tmpEB2.tag=2,((_tmpEB2.f1=0,((_tmpEB2.f2=(void*)expected_kind,_tmpEB2)))))),
_tmpEB3))))),_tmpEB1))));}goto _LL50C;_LL50F:;_LL510: goto _LL50C;_LL50C:;}cvtenv.kind_env=
Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmp77C);cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(
cvtenv.r,cvtenv.free_vars,_tmp77C,put_in_effect);{void*_tmp7CB=Cyc_Absyn_compress_kb((
void*)_tmp77C->kind);struct Cyc_Core_Opt*_tmp7CC;struct Cyc_Core_Opt**_tmp7CD;void*
_tmp7CE;_LL512: if(*((int*)_tmp7CB)!= 2)goto _LL514;_tmp7CC=((struct Cyc_Absyn_Less_kb_struct*)
_tmp7CB)->f1;_tmp7CD=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp7CB)->f1;_tmp7CE=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp7CB)->f2;
_LL513: if(Cyc_Tcutil_kind_leq(expected_kind,_tmp7CE)){struct Cyc_Absyn_Less_kb_struct*
_tmpEB9;struct Cyc_Absyn_Less_kb_struct _tmpEB8;struct Cyc_Core_Opt*_tmpEB7;*
_tmp7CD=((_tmpEB7=_cycalloc(sizeof(*_tmpEB7)),((_tmpEB7->v=(void*)((void*)((
_tmpEB9=_cycalloc(sizeof(*_tmpEB9)),((_tmpEB9[0]=((_tmpEB8.tag=2,((_tmpEB8.f1=0,((
_tmpEB8.f2=(void*)expected_kind,_tmpEB8)))))),_tmpEB9))))),_tmpEB7))));}goto
_LL511;_LL514:;_LL515: goto _LL511;_LL511:;}goto _LL4D7;_LL4DE: if(*((int*)_tmp777)
!= 13)goto _LL4E0;_tmp77D=((struct Cyc_Absyn_AnonEnumType_struct*)_tmp777)->f1;
_LL4DF: {struct Cyc_Tcenv_Genv*ge=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,
struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);struct _RegionHandle*
_tmp7D2=Cyc_Tcenv_get_fnrgn(te);{struct Cyc_List_List*prev_fields=0;unsigned int
tag_count=0;for(0;_tmp77D != 0;_tmp77D=_tmp77D->tl){struct Cyc_Absyn_Enumfield*
_tmp7D3=(struct Cyc_Absyn_Enumfield*)_tmp77D->hd;if(((int(*)(int(*compare)(struct
_dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct
_dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,(*_tmp7D3->name).f2)){
const char*_tmpEBD;void*_tmpEBC[1];struct Cyc_String_pa_struct _tmpEBB;(_tmpEBB.tag=
0,((_tmpEBB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp7D3->name).f2),((
_tmpEBC[0]=& _tmpEBB,Cyc_Tcutil_terr(_tmp7D3->loc,((_tmpEBD="duplicate enum field name %s",
_tag_dyneither(_tmpEBD,sizeof(char),29))),_tag_dyneither(_tmpEBC,sizeof(void*),1)))))));}
else{struct Cyc_List_List*_tmpEBE;prev_fields=((_tmpEBE=_region_malloc(_tmp7D2,
sizeof(*_tmpEBE)),((_tmpEBE->hd=(*_tmp7D3->name).f2,((_tmpEBE->tl=prev_fields,
_tmpEBE))))));}if(_tmp7D3->tag == 0)_tmp7D3->tag=(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(
tag_count,_tmp7D3->loc);else{if(!Cyc_Tcutil_is_const_exp(te,(struct Cyc_Absyn_Exp*)
_check_null(_tmp7D3->tag))){const char*_tmpEC2;void*_tmpEC1[1];struct Cyc_String_pa_struct
_tmpEC0;(_tmpEC0.tag=0,((_tmpEC0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*
_tmp7D3->name).f2),((_tmpEC1[0]=& _tmpEC0,Cyc_Tcutil_terr(loc,((_tmpEC2="enum field %s: expression is not constant",
_tag_dyneither(_tmpEC2,sizeof(char),42))),_tag_dyneither(_tmpEC1,sizeof(void*),1)))))));}}{
unsigned int t1=(Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)_check_null(
_tmp7D3->tag))).f1;tag_count=t1 + 1;{union Cyc_Absyn_Nmspace_union _tmpEC3;(*
_tmp7D3->name).f1=(union Cyc_Absyn_Nmspace_union)(((_tmpEC3.Abs_n).tag=2,(((
_tmpEC3.Abs_n).f1=te->ns,_tmpEC3))));}{struct Cyc_Tcenv_AnonEnumRes_struct*
_tmpEC9;struct Cyc_Tcenv_AnonEnumRes_struct _tmpEC8;struct _tuple19*_tmpEC7;ge->ordinaries=((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct
_tuple19*v))Cyc_Dict_insert)(ge->ordinaries,(*_tmp7D3->name).f2,(struct _tuple19*)((
_tmpEC7=_cycalloc(sizeof(*_tmpEC7)),((_tmpEC7->f1=(void*)((_tmpEC9=_cycalloc(
sizeof(*_tmpEC9)),((_tmpEC9[0]=((_tmpEC8.tag=4,((_tmpEC8.f1=(void*)t,((_tmpEC8.f2=
_tmp7D3,_tmpEC8)))))),_tmpEC9)))),((_tmpEC7->f2=1,_tmpEC7)))))));}}}}goto _LL4D7;}
_LL4E0: if(*((int*)_tmp777)!= 12)goto _LL4E2;_tmp77E=((struct Cyc_Absyn_EnumType_struct*)
_tmp777)->f1;_tmp77F=((struct Cyc_Absyn_EnumType_struct*)_tmp777)->f2;_tmp780=(
struct Cyc_Absyn_Enumdecl**)&((struct Cyc_Absyn_EnumType_struct*)_tmp777)->f2;
_LL4E1: if(*_tmp780 == 0  || ((struct Cyc_Absyn_Enumdecl*)_check_null(*_tmp780))->fields
== 0){struct _handler_cons _tmp7DF;_push_handler(& _tmp7DF);{int _tmp7E1=0;if(setjmp(
_tmp7DF.handler))_tmp7E1=1;if(!_tmp7E1){{struct Cyc_Absyn_Enumdecl**ed=Cyc_Tcenv_lookup_enumdecl(
te,loc,_tmp77E);*_tmp780=(struct Cyc_Absyn_Enumdecl*)*ed;};_pop_handler();}else{
void*_tmp7E0=(void*)_exn_thrown;void*_tmp7E3=_tmp7E0;_LL517: if(_tmp7E3 != Cyc_Dict_Absent)
goto _LL519;_LL518: {struct Cyc_Tcenv_Genv*_tmp7E4=((struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct Cyc_Absyn_Enumdecl*_tmpECA;struct Cyc_Absyn_Enumdecl*_tmp7E5=(_tmpECA=
_cycalloc(sizeof(*_tmpECA)),((_tmpECA->sc=(void*)((void*)3),((_tmpECA->name=
_tmp77E,((_tmpECA->fields=0,_tmpECA)))))));Cyc_Tc_tcEnumdecl(te,_tmp7E4,loc,
_tmp7E5);{struct Cyc_Absyn_Enumdecl**ed=Cyc_Tcenv_lookup_enumdecl(te,loc,_tmp77E);*
_tmp780=(struct Cyc_Absyn_Enumdecl*)*ed;goto _LL516;}}_LL519:;_LL51A:(void)_throw(
_tmp7E3);_LL516:;}}}{struct Cyc_Absyn_Enumdecl*ed=(struct Cyc_Absyn_Enumdecl*)
_check_null(*_tmp780);*_tmp77E=(ed->name)[_check_known_subscript_notnull(1,0)];
goto _LL4D7;}_LL4E2: if(*((int*)_tmp777)!= 2)goto _LL4E4;_tmp781=((struct Cyc_Absyn_TunionType_struct*)
_tmp777)->f1;_tmp782=_tmp781.tunion_info;_tmp783=(union Cyc_Absyn_TunionInfoU_union*)&(((
struct Cyc_Absyn_TunionType_struct*)_tmp777)->f1).tunion_info;_tmp784=_tmp781.targs;
_tmp785=(struct Cyc_List_List**)&(((struct Cyc_Absyn_TunionType_struct*)_tmp777)->f1).targs;
_tmp786=_tmp781.rgn;_tmp787=(struct Cyc_Core_Opt**)&(((struct Cyc_Absyn_TunionType_struct*)
_tmp777)->f1).rgn;_LL4E3: {struct Cyc_List_List*_tmp7E7=*_tmp785;{union Cyc_Absyn_TunionInfoU_union
_tmp7E8=*_tmp783;struct Cyc_Absyn_UnknownTunionInfo _tmp7E9;struct _tuple2*_tmp7EA;
int _tmp7EB;int _tmp7EC;struct Cyc_Absyn_Tuniondecl**_tmp7ED;struct Cyc_Absyn_Tuniondecl*
_tmp7EE;_LL51C: if((_tmp7E8.UnknownTunion).tag != 0)goto _LL51E;_tmp7E9=(_tmp7E8.UnknownTunion).f1;
_tmp7EA=_tmp7E9.name;_tmp7EB=_tmp7E9.is_xtunion;_tmp7EC=_tmp7E9.is_flat;_LL51D: {
struct Cyc_Absyn_Tuniondecl**tudp;{struct _handler_cons _tmp7EF;_push_handler(&
_tmp7EF);{int _tmp7F1=0;if(setjmp(_tmp7EF.handler))_tmp7F1=1;if(!_tmp7F1){tudp=
Cyc_Tcenv_lookup_tuniondecl(te,loc,_tmp7EA);;_pop_handler();}else{void*_tmp7F0=(
void*)_exn_thrown;void*_tmp7F3=_tmp7F0;_LL521: if(_tmp7F3 != Cyc_Dict_Absent)goto
_LL523;_LL522: {struct Cyc_Tcenv_Genv*_tmp7F4=((struct Cyc_Tcenv_Genv*(*)(struct
Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);struct Cyc_Absyn_Tuniondecl*
_tmpECB;struct Cyc_Absyn_Tuniondecl*_tmp7F5=(_tmpECB=_cycalloc(sizeof(*_tmpECB)),((
_tmpECB->sc=(void*)((void*)3),((_tmpECB->name=_tmp7EA,((_tmpECB->tvs=0,((_tmpECB->fields=
0,((_tmpECB->is_xtunion=_tmp7EB,((_tmpECB->is_flat=_tmp7EC,_tmpECB)))))))))))));
Cyc_Tc_tcTuniondecl(te,_tmp7F4,loc,_tmp7F5);tudp=Cyc_Tcenv_lookup_tuniondecl(te,
loc,_tmp7EA);if(_tmp7E7 != 0){{const char*_tmpED4;const char*_tmpED3;const char*
_tmpED2;void*_tmpED1[2];struct Cyc_String_pa_struct _tmpED0;struct Cyc_String_pa_struct
_tmpECF;(_tmpECF.tag=0,((_tmpECF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp7EA)),((_tmpED0.tag=0,((_tmpED0.f1=(struct
_dyneither_ptr)(_tmp7EB?(struct _dyneither_ptr)((struct _dyneither_ptr)((_tmpED3="xtunion",
_tag_dyneither(_tmpED3,sizeof(char),8)))):(struct _dyneither_ptr)((_tmpED4="tunion",
_tag_dyneither(_tmpED4,sizeof(char),7)))),((_tmpED1[0]=& _tmpED0,((_tmpED1[1]=&
_tmpECF,Cyc_Tcutil_terr(loc,((_tmpED2="declare parameterized %s %s before using",
_tag_dyneither(_tmpED2,sizeof(char),41))),_tag_dyneither(_tmpED1,sizeof(void*),2)))))))))))));}
return cvtenv;}goto _LL520;}_LL523:;_LL524:(void)_throw(_tmp7F3);_LL520:;}}}if((*
tudp)->is_xtunion != _tmp7EB){const char*_tmpED8;void*_tmpED7[1];struct Cyc_String_pa_struct
_tmpED6;(_tmpED6.tag=0,((_tmpED6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp7EA)),((_tmpED7[0]=& _tmpED6,Cyc_Tcutil_terr(loc,((
_tmpED8="[x]tunion is different from type declaration %s",_tag_dyneither(_tmpED8,
sizeof(char),48))),_tag_dyneither(_tmpED7,sizeof(void*),1)))))));}{union Cyc_Absyn_TunionInfoU_union
_tmpED9;*_tmp783=(union Cyc_Absyn_TunionInfoU_union)(((_tmpED9.KnownTunion).tag=1,(((
_tmpED9.KnownTunion).f1=tudp,_tmpED9))));}_tmp7EE=*tudp;goto _LL51F;}_LL51E: if((
_tmp7E8.KnownTunion).tag != 1)goto _LL51B;_tmp7ED=(_tmp7E8.KnownTunion).f1;_tmp7EE=*
_tmp7ED;_LL51F: if(_tmp7EE->is_flat  && (unsigned int)*_tmp787){const char*_tmpEDD;
void*_tmpEDC[1];struct Cyc_String_pa_struct _tmpEDB;(_tmpEDB.tag=0,((_tmpEDB.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)((
struct Cyc_Core_Opt*)_check_null(*_tmp787))->v)),((_tmpEDC[0]=& _tmpEDB,Cyc_Tcutil_terr(
loc,((_tmpEDD="flat tunion has region %s",_tag_dyneither(_tmpEDD,sizeof(char),26))),
_tag_dyneither(_tmpEDC,sizeof(void*),1)))))));}if(!_tmp7EE->is_flat  && !((
unsigned int)*_tmp787)){struct Cyc_Core_Opt*_tmpEDE;*_tmp787=((_tmpEDE=_cycalloc(
sizeof(*_tmpEDE)),((_tmpEDE->v=(void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_rk,0),_tmpEDE))));}if((unsigned int)*_tmp787){void*_tmp805=(void*)((
struct Cyc_Core_Opt*)_check_null(*_tmp787))->v;cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,(void*)3,_tmp805,1);}{struct Cyc_List_List*tvs=_tmp7EE->tvs;for(0;
_tmp7E7 != 0  && tvs != 0;(_tmp7E7=_tmp7E7->tl,tvs=tvs->tl)){void*t1=(void*)_tmp7E7->hd;
void*k1=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd);cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,k1,t1,1);}if(_tmp7E7 != 0){const char*_tmpEE2;void*_tmpEE1[1];struct
Cyc_String_pa_struct _tmpEE0;(_tmpEE0.tag=0,((_tmpEE0.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp7EE->name)),((_tmpEE1[0]=&
_tmpEE0,Cyc_Tcutil_terr(loc,((_tmpEE2="too many type arguments for tunion %s",
_tag_dyneither(_tmpEE2,sizeof(char),38))),_tag_dyneither(_tmpEE1,sizeof(void*),1)))))));}
if(tvs != 0){struct Cyc_List_List*hidden_ts=0;for(0;tvs != 0;tvs=tvs->tl){void*k1=
Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd);void*e=Cyc_Absyn_new_evar(0,
0);{struct Cyc_List_List*_tmpEE3;hidden_ts=((_tmpEE3=_cycalloc(sizeof(*_tmpEE3)),((
_tmpEE3->hd=(void*)e,((_tmpEE3->tl=hidden_ts,_tmpEE3))))));}cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,k1,e,1);}*_tmp785=Cyc_List_imp_append(*_tmp785,Cyc_List_imp_rev(
hidden_ts));}goto _LL51B;}_LL51B:;}goto _LL4D7;}_LL4E4: if(*((int*)_tmp777)!= 3)
goto _LL4E6;_tmp788=((struct Cyc_Absyn_TunionFieldType_struct*)_tmp777)->f1;
_tmp789=_tmp788.field_info;_tmp78A=(union Cyc_Absyn_TunionFieldInfoU_union*)&(((
struct Cyc_Absyn_TunionFieldType_struct*)_tmp777)->f1).field_info;_tmp78B=_tmp788.targs;
_LL4E5:{union Cyc_Absyn_TunionFieldInfoU_union _tmp80A=*_tmp78A;struct Cyc_Absyn_UnknownTunionFieldInfo
_tmp80B;struct _tuple2*_tmp80C;struct _tuple2*_tmp80D;int _tmp80E;struct Cyc_Absyn_Tuniondecl*
_tmp80F;struct Cyc_Absyn_Tunionfield*_tmp810;_LL526: if((_tmp80A.UnknownTunionfield).tag
!= 0)goto _LL528;_tmp80B=(_tmp80A.UnknownTunionfield).f1;_tmp80C=_tmp80B.tunion_name;
_tmp80D=_tmp80B.field_name;_tmp80E=_tmp80B.is_xtunion;_LL527: {struct Cyc_Absyn_Tuniondecl*
tud;struct Cyc_Absyn_Tunionfield*tuf;{struct _handler_cons _tmp811;_push_handler(&
_tmp811);{int _tmp813=0;if(setjmp(_tmp811.handler))_tmp813=1;if(!_tmp813){*Cyc_Tcenv_lookup_tuniondecl(
te,loc,_tmp80C);;_pop_handler();}else{void*_tmp812=(void*)_exn_thrown;void*
_tmp815=_tmp812;_LL52B: if(_tmp815 != Cyc_Dict_Absent)goto _LL52D;_LL52C:{const char*
_tmpEE7;void*_tmpEE6[1];struct Cyc_String_pa_struct _tmpEE5;(_tmpEE5.tag=0,((
_tmpEE5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp80C)),((_tmpEE6[0]=& _tmpEE5,Cyc_Tcutil_terr(loc,((_tmpEE7="unbound type tunion %s",
_tag_dyneither(_tmpEE7,sizeof(char),23))),_tag_dyneither(_tmpEE6,sizeof(void*),1)))))));}
return cvtenv;_LL52D:;_LL52E:(void)_throw(_tmp815);_LL52A:;}}}{struct
_handler_cons _tmp819;_push_handler(& _tmp819);{int _tmp81B=0;if(setjmp(_tmp819.handler))
_tmp81B=1;if(!_tmp81B){{struct _RegionHandle*_tmp81C=Cyc_Tcenv_get_fnrgn(te);void*
_tmp81D=Cyc_Tcenv_lookup_ordinary(_tmp81C,te,loc,_tmp80D);struct Cyc_Absyn_Tuniondecl*
_tmp81E;struct Cyc_Absyn_Tunionfield*_tmp81F;_LL530: if(*((int*)_tmp81D)!= 2)goto
_LL532;_tmp81E=((struct Cyc_Tcenv_TunionRes_struct*)_tmp81D)->f1;_tmp81F=((struct
Cyc_Tcenv_TunionRes_struct*)_tmp81D)->f2;_LL531: tuf=_tmp81F;tud=_tmp81E;if(tud->is_xtunion
!= _tmp80E){const char*_tmpEEB;void*_tmpEEA[1];struct Cyc_String_pa_struct _tmpEE9;(
_tmpEE9.tag=0,((_tmpEE9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp80C)),((_tmpEEA[0]=& _tmpEE9,Cyc_Tcutil_terr(loc,((_tmpEEB="[x]tunion is different from type declaration %s",
_tag_dyneither(_tmpEEB,sizeof(char),48))),_tag_dyneither(_tmpEEA,sizeof(void*),1)))))));}
goto _LL52F;_LL532:;_LL533:{const char*_tmpEF0;void*_tmpEEF[2];struct Cyc_String_pa_struct
_tmpEEE;struct Cyc_String_pa_struct _tmpEED;(_tmpEED.tag=0,((_tmpEED.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp80C)),((
_tmpEEE.tag=0,((_tmpEEE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp80D)),((_tmpEEF[0]=& _tmpEEE,((_tmpEEF[1]=& _tmpEED,Cyc_Tcutil_terr(loc,((
_tmpEF0="unbound field %s in type tunion %s",_tag_dyneither(_tmpEF0,sizeof(char),
35))),_tag_dyneither(_tmpEEF,sizeof(void*),2)))))))))))));}{struct Cyc_Tcutil_CVTEnv
_tmp827=cvtenv;_npop_handler(0);return _tmp827;}_LL52F:;};_pop_handler();}else{
void*_tmp81A=(void*)_exn_thrown;void*_tmp829=_tmp81A;_LL535: if(_tmp829 != Cyc_Dict_Absent)
goto _LL537;_LL536:{const char*_tmpEF5;void*_tmpEF4[2];struct Cyc_String_pa_struct
_tmpEF3;struct Cyc_String_pa_struct _tmpEF2;(_tmpEF2.tag=0,((_tmpEF2.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp80C)),((
_tmpEF3.tag=0,((_tmpEF3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp80D)),((_tmpEF4[0]=& _tmpEF3,((_tmpEF4[1]=& _tmpEF2,Cyc_Tcutil_terr(loc,((
_tmpEF5="unbound field %s in type tunion %s",_tag_dyneither(_tmpEF5,sizeof(char),
35))),_tag_dyneither(_tmpEF4,sizeof(void*),2)))))))))))));}return cvtenv;_LL537:;
_LL538:(void)_throw(_tmp829);_LL534:;}}}{union Cyc_Absyn_TunionFieldInfoU_union
_tmpEF6;*_tmp78A=(union Cyc_Absyn_TunionFieldInfoU_union)(((_tmpEF6.KnownTunionfield).tag=
1,(((_tmpEF6.KnownTunionfield).f1=tud,(((_tmpEF6.KnownTunionfield).f2=tuf,
_tmpEF6))))));}_tmp80F=tud;_tmp810=tuf;goto _LL529;}_LL528: if((_tmp80A.KnownTunionfield).tag
!= 1)goto _LL525;_tmp80F=(_tmp80A.KnownTunionfield).f1;_tmp810=(_tmp80A.KnownTunionfield).f2;
_LL529: {struct Cyc_List_List*tvs=_tmp80F->tvs;for(0;_tmp78B != 0  && tvs != 0;(
_tmp78B=_tmp78B->tl,tvs=tvs->tl)){void*t1=(void*)_tmp78B->hd;void*k1=Cyc_Tcutil_tvar_kind((
struct Cyc_Absyn_Tvar*)tvs->hd);cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,
cvtenv,k1,t1,1);}if(_tmp78B != 0){const char*_tmpEFB;void*_tmpEFA[2];struct Cyc_String_pa_struct
_tmpEF9;struct Cyc_String_pa_struct _tmpEF8;(_tmpEF8.tag=0,((_tmpEF8.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp810->name)),((
_tmpEF9.tag=0,((_tmpEF9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp80F->name)),((_tmpEFA[0]=& _tmpEF9,((_tmpEFA[1]=& _tmpEF8,Cyc_Tcutil_terr(loc,((
_tmpEFB="too many type arguments for tunion %s.%s",_tag_dyneither(_tmpEFB,
sizeof(char),41))),_tag_dyneither(_tmpEFA,sizeof(void*),2)))))))))))));}if(tvs != 
0){const char*_tmpF00;void*_tmpEFF[2];struct Cyc_String_pa_struct _tmpEFE;struct Cyc_String_pa_struct
_tmpEFD;(_tmpEFD.tag=0,((_tmpEFD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp810->name)),((_tmpEFE.tag=0,((_tmpEFE.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp80F->name)),((
_tmpEFF[0]=& _tmpEFE,((_tmpEFF[1]=& _tmpEFD,Cyc_Tcutil_terr(loc,((_tmpF00="too few type arguments for tunion %s.%s",
_tag_dyneither(_tmpF00,sizeof(char),40))),_tag_dyneither(_tmpEFF,sizeof(void*),2)))))))))))));}
goto _LL525;}_LL525:;}goto _LL4D7;_LL4E6: if(*((int*)_tmp777)!= 4)goto _LL4E8;
_tmp78C=((struct Cyc_Absyn_PointerType_struct*)_tmp777)->f1;_tmp78D=(void*)
_tmp78C.elt_typ;_tmp78E=_tmp78C.elt_tq;_tmp78F=(struct Cyc_Absyn_Tqual*)&(((
struct Cyc_Absyn_PointerType_struct*)_tmp777)->f1).elt_tq;_tmp790=_tmp78C.ptr_atts;
_tmp791=(void*)_tmp790.rgn;_tmp792=_tmp790.nullable;_tmp793=_tmp790.bounds;
_tmp794=_tmp790.zero_term;_LL4E7: {int is_zero_terminated;cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,(void*)0,_tmp78D,1);_tmp78F->real_const=Cyc_Tcutil_extract_const_from_typedef(
loc,_tmp78F->print_const,_tmp78D);cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,
cvtenv,(void*)5,_tmp791,1);{union Cyc_Absyn_Constraint_union _tmp837=(((struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref*x))Cyc_Absyn_compress_conref)(_tmp794))->v;int _tmp838;
_LL53A: if((_tmp837.No_constr).tag != 2)goto _LL53C;_LL53B:{void*_tmp839=Cyc_Tcutil_compress(
_tmp78D);void*_tmp83A;_LL541: if(_tmp839 <= (void*)4)goto _LL543;if(*((int*)_tmp839)
!= 5)goto _LL543;_tmp83A=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp839)->f2;
if((int)_tmp83A != 0)goto _LL543;_LL542:((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*
x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp794,
Cyc_Absyn_true_conref);is_zero_terminated=1;goto _LL540;_LL543:;_LL544:((int(*)(
int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(
Cyc_Core_intcmp,_tmp794,Cyc_Absyn_false_conref);is_zero_terminated=0;goto _LL540;
_LL540:;}goto _LL539;_LL53C: if((_tmp837.Eq_constr).tag != 0)goto _LL53E;_tmp838=(
_tmp837.Eq_constr).f1;if(_tmp838 != 1)goto _LL53E;_LL53D: if(!Cyc_Tcutil_admits_zero(
_tmp78D)){const char*_tmpF04;void*_tmpF03[1];struct Cyc_String_pa_struct _tmpF02;(
_tmpF02.tag=0,((_tmpF02.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
_tmp78D)),((_tmpF03[0]=& _tmpF02,Cyc_Tcutil_terr(loc,((_tmpF04="cannot have a pointer to zero-terminated %s elements",
_tag_dyneither(_tmpF04,sizeof(char),53))),_tag_dyneither(_tmpF03,sizeof(void*),1)))))));}
is_zero_terminated=1;goto _LL539;_LL53E:;_LL53F: is_zero_terminated=0;goto _LL539;
_LL539:;}{union Cyc_Absyn_Constraint_union _tmp83E=(Cyc_Absyn_compress_conref(
_tmp793))->v;void*_tmp83F;void*_tmp840;struct Cyc_Absyn_Exp*_tmp841;_LL546: if((
_tmp83E.No_constr).tag != 2)goto _LL548;_LL547: goto _LL549;_LL548: if((_tmp83E.Eq_constr).tag
!= 0)goto _LL54A;_tmp83F=(_tmp83E.Eq_constr).f1;if((int)_tmp83F != 0)goto _LL54A;
_LL549: goto _LL545;_LL54A: if((_tmp83E.Eq_constr).tag != 0)goto _LL54C;_tmp840=(
_tmp83E.Eq_constr).f1;if(_tmp840 <= (void*)1)goto _LL54C;if(*((int*)_tmp840)!= 0)
goto _LL54C;_tmp841=((struct Cyc_Absyn_Upper_b_struct*)_tmp840)->f1;_LL54B: {
struct _RegionHandle*_tmp842=Cyc_Tcenv_get_fnrgn(te);{struct Cyc_Tcenv_Tenv*
_tmp843=Cyc_Tcenv_allow_valueof(_tmp842,te);Cyc_Tcexp_tcExp(_tmp843,0,_tmp841);}
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp841,te,cvtenv);if(!Cyc_Tcutil_coerce_uint_typ(
te,_tmp841)){const char*_tmpF07;void*_tmpF06;(_tmpF06=0,Cyc_Tcutil_terr(loc,((
_tmpF07="pointer bounds expression is not an unsigned int",_tag_dyneither(
_tmpF07,sizeof(char),49))),_tag_dyneither(_tmpF06,sizeof(void*),0)));}{
unsigned int _tmp847;int _tmp848;struct _tuple7 _tmp846=Cyc_Evexp_eval_const_uint_exp(
_tmp841);_tmp847=_tmp846.f1;_tmp848=_tmp846.f2;if(is_zero_terminated  && (!
_tmp848  || _tmp847 < 1)){const char*_tmpF0A;void*_tmpF09;(_tmpF09=0,Cyc_Tcutil_terr(
loc,((_tmpF0A="zero-terminated pointer cannot point to empty sequence",
_tag_dyneither(_tmpF0A,sizeof(char),55))),_tag_dyneither(_tmpF09,sizeof(void*),0)));}
goto _LL545;}}_LL54C: if((_tmp83E.Forward_constr).tag != 1)goto _LL545;_LL54D: {
const char*_tmpF0D;void*_tmpF0C;(_tmpF0C=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpF0D="forward constraint",
_tag_dyneither(_tmpF0D,sizeof(char),19))),_tag_dyneither(_tmpF0C,sizeof(void*),0)));}
_LL545:;}goto _LL4D7;}_LL4E8: if(*((int*)_tmp777)!= 18)goto _LL4EA;_tmp795=(void*)((
struct Cyc_Absyn_TagType_struct*)_tmp777)->f1;_LL4E9: cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,(void*)7,_tmp795,1);goto _LL4D7;_LL4EA: if(*((int*)_tmp777)!= 17)
goto _LL4EC;_tmp796=((struct Cyc_Absyn_ValueofType_struct*)_tmp777)->f1;_LL4EB: {
struct _RegionHandle*_tmp84D=Cyc_Tcenv_get_fnrgn(te);{struct Cyc_Tcenv_Tenv*
_tmp84E=Cyc_Tcenv_allow_valueof(_tmp84D,te);Cyc_Tcexp_tcExp(_tmp84E,0,_tmp796);}
if(!Cyc_Tcutil_coerce_uint_typ(te,_tmp796)){const char*_tmpF10;void*_tmpF0F;(
_tmpF0F=0,Cyc_Tcutil_terr(loc,((_tmpF10="valueof_t requires an int expression",
_tag_dyneither(_tmpF10,sizeof(char),37))),_tag_dyneither(_tmpF0F,sizeof(void*),0)));}
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp796,te,cvtenv);goto _LL4D7;}
_LL4EC: if(*((int*)_tmp777)!= 5)goto _LL4EE;_LL4ED: goto _LL4EF;_LL4EE: if((int)
_tmp777 != 1)goto _LL4F0;_LL4EF: goto _LL4F1;_LL4F0: if(_tmp777 <= (void*)4)goto _LL4FE;
if(*((int*)_tmp777)!= 6)goto _LL4F2;_LL4F1: goto _LL4D7;_LL4F2: if(*((int*)_tmp777)
!= 7)goto _LL4F4;_tmp797=((struct Cyc_Absyn_ArrayType_struct*)_tmp777)->f1;_tmp798=(
void*)_tmp797.elt_type;_tmp799=_tmp797.tq;_tmp79A=(struct Cyc_Absyn_Tqual*)&(((
struct Cyc_Absyn_ArrayType_struct*)_tmp777)->f1).tq;_tmp79B=_tmp797.num_elts;
_tmp79C=(struct Cyc_Absyn_Exp**)&(((struct Cyc_Absyn_ArrayType_struct*)_tmp777)->f1).num_elts;
_tmp79D=_tmp797.zero_term;_tmp79E=_tmp797.zt_loc;_LL4F3: {struct Cyc_Absyn_Exp*
_tmp851=*_tmp79C;cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)1,
_tmp798,1);_tmp79A->real_const=Cyc_Tcutil_extract_const_from_typedef(loc,_tmp79A->print_const,
_tmp798);{int is_zero_terminated;{union Cyc_Absyn_Constraint_union _tmp852=(((
struct Cyc_Absyn_Conref*(*)(struct Cyc_Absyn_Conref*x))Cyc_Absyn_compress_conref)(
_tmp79D))->v;int _tmp853;_LL54F: if((_tmp852.No_constr).tag != 2)goto _LL551;_LL550:((
int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(
Cyc_Core_intcmp,_tmp79D,Cyc_Absyn_false_conref);is_zero_terminated=0;goto _LL54E;
_LL551: if((_tmp852.Eq_constr).tag != 0)goto _LL553;_tmp853=(_tmp852.Eq_constr).f1;
if(_tmp853 != 1)goto _LL553;_LL552: if(!Cyc_Tcutil_admits_zero(_tmp798)){const char*
_tmpF14;void*_tmpF13[1];struct Cyc_String_pa_struct _tmpF12;(_tmpF12.tag=0,((
_tmpF12.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
_tmp798)),((_tmpF13[0]=& _tmpF12,Cyc_Tcutil_terr(loc,((_tmpF14="cannot have a zero-terminated array of %s elements",
_tag_dyneither(_tmpF14,sizeof(char),51))),_tag_dyneither(_tmpF13,sizeof(void*),1)))))));}
is_zero_terminated=1;goto _LL54E;_LL553:;_LL554: is_zero_terminated=0;goto _LL54E;
_LL54E:;}if(_tmp851 == 0){if(is_zero_terminated)*_tmp79C=(_tmp851=(struct Cyc_Absyn_Exp*)
Cyc_Absyn_uint_exp(1,0));else{{const char*_tmpF17;void*_tmpF16;(_tmpF16=0,Cyc_Tcutil_warn(
loc,((_tmpF17="array bound defaults to 1 here",_tag_dyneither(_tmpF17,sizeof(
char),31))),_tag_dyneither(_tmpF16,sizeof(void*),0)));}*_tmp79C=(_tmp851=(struct
Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(1,0));}}Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)
_tmp851);if(!Cyc_Tcutil_is_const_exp(te,(struct Cyc_Absyn_Exp*)_tmp851)){const
char*_tmpF1A;void*_tmpF19;(_tmpF19=0,Cyc_Tcutil_terr(loc,((_tmpF1A="array bounds expression is not constant",
_tag_dyneither(_tmpF1A,sizeof(char),40))),_tag_dyneither(_tmpF19,sizeof(void*),0)));}
if(!Cyc_Tcutil_coerce_uint_typ(te,(struct Cyc_Absyn_Exp*)_tmp851)){const char*
_tmpF1D;void*_tmpF1C;(_tmpF1C=0,Cyc_Tcutil_terr(loc,((_tmpF1D="array bounds expression is not an unsigned int",
_tag_dyneither(_tmpF1D,sizeof(char),47))),_tag_dyneither(_tmpF1C,sizeof(void*),0)));}{
unsigned int _tmp85E;int _tmp85F;struct _tuple7 _tmp85D=Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)_tmp851);_tmp85E=_tmp85D.f1;_tmp85F=_tmp85D.f2;if((
is_zero_terminated  && _tmp85F) && _tmp85E < 1){const char*_tmpF20;void*_tmpF1F;(
_tmpF1F=0,Cyc_Tcutil_warn(loc,((_tmpF20="zero terminated array cannot have zero elements",
_tag_dyneither(_tmpF20,sizeof(char),48))),_tag_dyneither(_tmpF1F,sizeof(void*),0)));}
if((_tmp85F  && _tmp85E < 1) && Cyc_Cyclone_tovc_r){{const char*_tmpF23;void*
_tmpF22;(_tmpF22=0,Cyc_Tcutil_warn(loc,((_tmpF23="arrays with 0 elements are not supported except with gcc -- changing to 1.",
_tag_dyneither(_tmpF23,sizeof(char),75))),_tag_dyneither(_tmpF22,sizeof(void*),0)));}*
_tmp79C=(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(1,0);}goto _LL4D7;}}}_LL4F4: if(*((
int*)_tmp777)!= 8)goto _LL4F6;_tmp79F=((struct Cyc_Absyn_FnType_struct*)_tmp777)->f1;
_tmp7A0=_tmp79F.tvars;_tmp7A1=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_struct*)
_tmp777)->f1).tvars;_tmp7A2=_tmp79F.effect;_tmp7A3=(struct Cyc_Core_Opt**)&(((
struct Cyc_Absyn_FnType_struct*)_tmp777)->f1).effect;_tmp7A4=(void*)_tmp79F.ret_typ;
_tmp7A5=_tmp79F.args;_tmp7A6=_tmp79F.c_varargs;_tmp7A7=_tmp79F.cyc_varargs;
_tmp7A8=_tmp79F.rgn_po;_tmp7A9=_tmp79F.attributes;_LL4F5: {int num_convs=0;int
seen_cdecl=0;int seen_stdcall=0;int seen_fastcall=0;int seen_format=0;void*ft=(void*)
0;int fmt_desc_arg=- 1;int fmt_arg_start=- 1;for(0;_tmp7A9 != 0;_tmp7A9=_tmp7A9->tl){
if(!Cyc_Absyn_fntype_att((void*)_tmp7A9->hd)){const char*_tmpF27;void*_tmpF26[1];
struct Cyc_String_pa_struct _tmpF25;(_tmpF25.tag=0,((_tmpF25.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((void*)_tmp7A9->hd)),((
_tmpF26[0]=& _tmpF25,Cyc_Tcutil_terr(loc,((_tmpF27="bad function type attribute %s",
_tag_dyneither(_tmpF27,sizeof(char),31))),_tag_dyneither(_tmpF26,sizeof(void*),1)))))));}{
void*_tmp867=(void*)_tmp7A9->hd;void*_tmp868;int _tmp869;int _tmp86A;_LL556: if((
int)_tmp867 != 0)goto _LL558;_LL557: if(!seen_stdcall){seen_stdcall=1;++ num_convs;}
goto _LL555;_LL558: if((int)_tmp867 != 1)goto _LL55A;_LL559: if(!seen_cdecl){
seen_cdecl=1;++ num_convs;}goto _LL555;_LL55A: if((int)_tmp867 != 2)goto _LL55C;
_LL55B: if(!seen_fastcall){seen_fastcall=1;++ num_convs;}goto _LL555;_LL55C: if(
_tmp867 <= (void*)17)goto _LL55E;if(*((int*)_tmp867)!= 3)goto _LL55E;_tmp868=(void*)((
struct Cyc_Absyn_Format_att_struct*)_tmp867)->f1;_tmp869=((struct Cyc_Absyn_Format_att_struct*)
_tmp867)->f2;_tmp86A=((struct Cyc_Absyn_Format_att_struct*)_tmp867)->f3;_LL55D:
if(!seen_format){seen_format=1;ft=_tmp868;fmt_desc_arg=_tmp869;fmt_arg_start=
_tmp86A;}else{const char*_tmpF2A;void*_tmpF29;(_tmpF29=0,Cyc_Tcutil_terr(loc,((
_tmpF2A="function can't have multiple format attributes",_tag_dyneither(_tmpF2A,
sizeof(char),47))),_tag_dyneither(_tmpF29,sizeof(void*),0)));}goto _LL555;_LL55E:;
_LL55F: goto _LL555;_LL555:;}}if(num_convs > 1){const char*_tmpF2D;void*_tmpF2C;(
_tmpF2C=0,Cyc_Tcutil_terr(loc,((_tmpF2D="function can't have multiple calling conventions",
_tag_dyneither(_tmpF2D,sizeof(char),49))),_tag_dyneither(_tmpF2C,sizeof(void*),0)));}
Cyc_Tcutil_check_unique_tvars(loc,*_tmp7A1);{struct Cyc_List_List*b=*_tmp7A1;for(
0;b != 0;b=b->tl){((struct Cyc_Absyn_Tvar*)b->hd)->identity=Cyc_Tcutil_new_tvar_id();
cvtenv.kind_env=Cyc_Tcutil_add_bound_tvar(cvtenv.kind_env,(struct Cyc_Absyn_Tvar*)
b->hd);{void*_tmp86F=Cyc_Absyn_compress_kb((void*)((struct Cyc_Absyn_Tvar*)b->hd)->kind);
void*_tmp870;_LL561: if(*((int*)_tmp86F)!= 0)goto _LL563;_tmp870=(void*)((struct
Cyc_Absyn_Eq_kb_struct*)_tmp86F)->f1;if((int)_tmp870 != 1)goto _LL563;_LL562:{
const char*_tmpF31;void*_tmpF30[1];struct Cyc_String_pa_struct _tmpF2F;(_tmpF2F.tag=
0,((_tmpF2F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)
b->hd)->name),((_tmpF30[0]=& _tmpF2F,Cyc_Tcutil_terr(loc,((_tmpF31="function attempts to abstract Mem type variable %s",
_tag_dyneither(_tmpF31,sizeof(char),51))),_tag_dyneither(_tmpF30,sizeof(void*),1)))))));}
goto _LL560;_LL563:;_LL564: goto _LL560;_LL560:;}}}{struct _RegionHandle*_tmp874=Cyc_Tcenv_get_fnrgn(
te);{struct Cyc_Tcutil_CVTEnv _tmpF32;struct Cyc_Tcutil_CVTEnv _tmp875=(_tmpF32.r=
_tmp874,((_tmpF32.kind_env=cvtenv.kind_env,((_tmpF32.free_vars=0,((_tmpF32.free_evars=
0,((_tmpF32.generalize_evars=cvtenv.generalize_evars,((_tmpF32.fn_result=1,
_tmpF32)))))))))));_tmp875=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp875,(void*)1,
_tmp7A4,1);_tmp875.fn_result=0;{struct Cyc_List_List*a=_tmp7A5;for(0;a != 0;a=a->tl){
struct _tuple3*_tmp876=(struct _tuple3*)a->hd;void*_tmp877=(*_tmp876).f3;_tmp875=
Cyc_Tcutil_i_check_valid_type(loc,te,_tmp875,(void*)1,_tmp877,1);((*_tmp876).f2).real_const=
Cyc_Tcutil_extract_const_from_typedef(loc,((*_tmp876).f2).print_const,_tmp877);}}
if(_tmp7A7 != 0){if(_tmp7A6){const char*_tmpF35;void*_tmpF34;(_tmpF34=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpF35="both c_vararg and cyc_vararg",
_tag_dyneither(_tmpF35,sizeof(char),29))),_tag_dyneither(_tmpF34,sizeof(void*),0)));}{
void*_tmp87B;int _tmp87C;struct Cyc_Absyn_VarargInfo _tmp87A=*_tmp7A7;_tmp87B=(void*)
_tmp87A.type;_tmp87C=_tmp87A.inject;_tmp875=Cyc_Tcutil_i_check_valid_type(loc,te,
_tmp875,(void*)1,_tmp87B,1);(_tmp7A7->tq).real_const=Cyc_Tcutil_extract_const_from_typedef(
loc,(_tmp7A7->tq).print_const,_tmp87B);if(_tmp87C){void*_tmp87D=Cyc_Tcutil_compress(
_tmp87B);struct Cyc_Absyn_TunionInfo _tmp87E;union Cyc_Absyn_TunionInfoU_union
_tmp87F;struct Cyc_Absyn_Tuniondecl**_tmp880;_LL566: if(_tmp87D <= (void*)4)goto
_LL568;if(*((int*)_tmp87D)!= 2)goto _LL568;_tmp87E=((struct Cyc_Absyn_TunionType_struct*)
_tmp87D)->f1;_tmp87F=_tmp87E.tunion_info;if((((((struct Cyc_Absyn_TunionType_struct*)
_tmp87D)->f1).tunion_info).KnownTunion).tag != 1)goto _LL568;_tmp880=(_tmp87F.KnownTunion).f1;
_LL567: if((*_tmp880)->is_flat){const char*_tmpF38;void*_tmpF37;(_tmpF37=0,Cyc_Tcutil_terr(
loc,((_tmpF38="cant inject into a flat tunion",_tag_dyneither(_tmpF38,sizeof(
char),31))),_tag_dyneither(_tmpF37,sizeof(void*),0)));}goto _LL565;_LL568:;_LL569:{
const char*_tmpF3B;void*_tmpF3A;(_tmpF3A=0,Cyc_Tcutil_terr(loc,((_tmpF3B="can't inject a non-[x]tunion type",
_tag_dyneither(_tmpF3B,sizeof(char),34))),_tag_dyneither(_tmpF3A,sizeof(void*),0)));}
goto _LL565;_LL565:;}}}if(seen_format){int _tmp885=((int(*)(struct Cyc_List_List*x))
Cyc_List_length)(_tmp7A5);if((((fmt_desc_arg < 0  || fmt_desc_arg > _tmp885) || 
fmt_arg_start < 0) || _tmp7A7 == 0  && fmt_arg_start != 0) || _tmp7A7 != 0  && 
fmt_arg_start != _tmp885 + 1){const char*_tmpF3E;void*_tmpF3D;(_tmpF3D=0,Cyc_Tcutil_terr(
loc,((_tmpF3E="bad format descriptor",_tag_dyneither(_tmpF3E,sizeof(char),22))),
_tag_dyneither(_tmpF3D,sizeof(void*),0)));}else{void*_tmp889;struct _tuple3
_tmp888=*((struct _tuple3*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp7A5,
fmt_desc_arg - 1);_tmp889=_tmp888.f3;{void*_tmp88A=Cyc_Tcutil_compress(_tmp889);
struct Cyc_Absyn_PtrInfo _tmp88B;void*_tmp88C;struct Cyc_Absyn_PtrAtts _tmp88D;
struct Cyc_Absyn_Conref*_tmp88E;struct Cyc_Absyn_Conref*_tmp88F;_LL56B: if(_tmp88A
<= (void*)4)goto _LL56D;if(*((int*)_tmp88A)!= 4)goto _LL56D;_tmp88B=((struct Cyc_Absyn_PointerType_struct*)
_tmp88A)->f1;_tmp88C=(void*)_tmp88B.elt_typ;_tmp88D=_tmp88B.ptr_atts;_tmp88E=
_tmp88D.bounds;_tmp88F=_tmp88D.zero_term;_LL56C:{struct _tuple0 _tmpF3F;struct
_tuple0 _tmp891=(_tmpF3F.f1=Cyc_Tcutil_compress(_tmp88C),((_tmpF3F.f2=Cyc_Absyn_conref_def((
void*)((void*)0),_tmp88E),_tmpF3F)));void*_tmp892;void*_tmp893;void*_tmp894;void*
_tmp895;_LL570: _tmp892=_tmp891.f1;if(_tmp892 <= (void*)4)goto _LL572;if(*((int*)
_tmp892)!= 5)goto _LL572;_tmp893=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp892)->f1;
if((int)_tmp893 != 2)goto _LL572;_tmp894=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp892)->f2;if((int)_tmp894 != 0)goto _LL572;_tmp895=_tmp891.f2;if((int)_tmp895 != 
0)goto _LL572;_LL571: goto _LL56F;_LL572:;_LL573:{const char*_tmpF42;void*_tmpF41;(
_tmpF41=0,Cyc_Tcutil_terr(loc,((_tmpF42="format descriptor is not a char ? type",
_tag_dyneither(_tmpF42,sizeof(char),39))),_tag_dyneither(_tmpF41,sizeof(void*),0)));}
goto _LL56F;_LL56F:;}goto _LL56A;_LL56D:;_LL56E:{const char*_tmpF45;void*_tmpF44;(
_tmpF44=0,Cyc_Tcutil_terr(loc,((_tmpF45="format descriptor is not a char ? type",
_tag_dyneither(_tmpF45,sizeof(char),39))),_tag_dyneither(_tmpF44,sizeof(void*),0)));}
goto _LL56A;_LL56A:;}if(fmt_arg_start != 0){int problem;{struct _tuple0 _tmpF46;
struct _tuple0 _tmp89B=(_tmpF46.f1=ft,((_tmpF46.f2=Cyc_Tcutil_compress((void*)((
struct Cyc_Absyn_VarargInfo*)_check_null(_tmp7A7))->type),_tmpF46)));void*_tmp89C;
void*_tmp89D;struct Cyc_Absyn_TunionInfo _tmp89E;union Cyc_Absyn_TunionInfoU_union
_tmp89F;struct Cyc_Absyn_Tuniondecl**_tmp8A0;struct Cyc_Absyn_Tuniondecl*_tmp8A1;
void*_tmp8A2;void*_tmp8A3;struct Cyc_Absyn_TunionInfo _tmp8A4;union Cyc_Absyn_TunionInfoU_union
_tmp8A5;struct Cyc_Absyn_Tuniondecl**_tmp8A6;struct Cyc_Absyn_Tuniondecl*_tmp8A7;
_LL575: _tmp89C=_tmp89B.f1;if((int)_tmp89C != 0)goto _LL577;_tmp89D=_tmp89B.f2;if(
_tmp89D <= (void*)4)goto _LL577;if(*((int*)_tmp89D)!= 2)goto _LL577;_tmp89E=((
struct Cyc_Absyn_TunionType_struct*)_tmp89D)->f1;_tmp89F=_tmp89E.tunion_info;if((((((
struct Cyc_Absyn_TunionType_struct*)_tmp89D)->f1).tunion_info).KnownTunion).tag != 
1)goto _LL577;_tmp8A0=(_tmp89F.KnownTunion).f1;_tmp8A1=*_tmp8A0;_LL576: problem=
Cyc_Absyn_qvar_cmp(_tmp8A1->name,Cyc_Absyn_tunion_print_arg_qvar)!= 0;goto _LL574;
_LL577: _tmp8A2=_tmp89B.f1;if((int)_tmp8A2 != 1)goto _LL579;_tmp8A3=_tmp89B.f2;if(
_tmp8A3 <= (void*)4)goto _LL579;if(*((int*)_tmp8A3)!= 2)goto _LL579;_tmp8A4=((
struct Cyc_Absyn_TunionType_struct*)_tmp8A3)->f1;_tmp8A5=_tmp8A4.tunion_info;if((((((
struct Cyc_Absyn_TunionType_struct*)_tmp8A3)->f1).tunion_info).KnownTunion).tag != 
1)goto _LL579;_tmp8A6=(_tmp8A5.KnownTunion).f1;_tmp8A7=*_tmp8A6;_LL578: problem=
Cyc_Absyn_qvar_cmp(_tmp8A7->name,Cyc_Absyn_tunion_scanf_arg_qvar)!= 0;goto _LL574;
_LL579:;_LL57A: problem=1;goto _LL574;_LL574:;}if(problem){const char*_tmpF49;void*
_tmpF48;(_tmpF48=0,Cyc_Tcutil_terr(loc,((_tmpF49="format attribute and vararg types don't match",
_tag_dyneither(_tmpF49,sizeof(char),46))),_tag_dyneither(_tmpF48,sizeof(void*),0)));}}}}{
struct Cyc_List_List*rpo=_tmp7A8;for(0;rpo != 0;rpo=rpo->tl){struct _tuple0 _tmp8AB;
void*_tmp8AC;void*_tmp8AD;struct _tuple0*_tmp8AA=(struct _tuple0*)rpo->hd;_tmp8AB=*
_tmp8AA;_tmp8AC=_tmp8AB.f1;_tmp8AD=_tmp8AB.f2;_tmp875=Cyc_Tcutil_i_check_valid_type(
loc,te,_tmp875,(void*)6,_tmp8AC,1);_tmp875=Cyc_Tcutil_i_check_valid_type(loc,te,
_tmp875,(void*)5,_tmp8AD,1);}}if(*_tmp7A3 != 0)_tmp875=Cyc_Tcutil_i_check_valid_type(
loc,te,_tmp875,(void*)6,(void*)((struct Cyc_Core_Opt*)_check_null(*_tmp7A3))->v,1);
else{struct Cyc_List_List*effect=0;{struct Cyc_List_List*tvs=_tmp875.free_vars;
for(0;tvs != 0;tvs=tvs->tl){struct Cyc_Absyn_Tvar*_tmp8AF;int _tmp8B0;struct
_tuple18 _tmp8AE=*((struct _tuple18*)tvs->hd);_tmp8AF=_tmp8AE.f1;_tmp8B0=_tmp8AE.f2;
if(!_tmp8B0)continue;{void*_tmp8B1=Cyc_Absyn_compress_kb((void*)_tmp8AF->kind);
struct Cyc_Core_Opt*_tmp8B2;struct Cyc_Core_Opt**_tmp8B3;void*_tmp8B4;struct Cyc_Core_Opt*
_tmp8B5;struct Cyc_Core_Opt**_tmp8B6;void*_tmp8B7;void*_tmp8B8;void*_tmp8B9;void*
_tmp8BA;struct Cyc_Core_Opt*_tmp8BB;struct Cyc_Core_Opt**_tmp8BC;void*_tmp8BD;void*
_tmp8BE;struct Cyc_Core_Opt*_tmp8BF;struct Cyc_Core_Opt**_tmp8C0;_LL57C: if(*((int*)
_tmp8B1)!= 2)goto _LL57E;_tmp8B2=((struct Cyc_Absyn_Less_kb_struct*)_tmp8B1)->f1;
_tmp8B3=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)_tmp8B1)->f1;
_tmp8B4=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp8B1)->f2;if((int)_tmp8B4 != 
5)goto _LL57E;_LL57D: _tmp8B6=_tmp8B3;_tmp8B7=(void*)3;goto _LL57F;_LL57E: if(*((int*)
_tmp8B1)!= 2)goto _LL580;_tmp8B5=((struct Cyc_Absyn_Less_kb_struct*)_tmp8B1)->f1;
_tmp8B6=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)_tmp8B1)->f1;
_tmp8B7=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp8B1)->f2;if(!(_tmp8B7 == (
void*)3  || _tmp8B7 == (void*)4))goto _LL580;_LL57F:*_tmp8B6=Cyc_Tcutil_kind_to_bound_opt(
_tmp8B7);_tmp8B8=_tmp8B7;goto _LL581;_LL580: if(*((int*)_tmp8B1)!= 0)goto _LL582;
_tmp8B8=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp8B1)->f1;if(!((_tmp8B8 == (
void*)3  || _tmp8B8 == (void*)4) || _tmp8B8 == (void*)5))goto _LL582;_LL581:{struct
Cyc_Absyn_AccessEff_struct*_tmpF58;struct Cyc_Absyn_VarType_struct*_tmpF57;struct
Cyc_Absyn_VarType_struct _tmpF56;struct Cyc_Absyn_AccessEff_struct _tmpF55;struct
Cyc_List_List*_tmpF54;effect=((_tmpF54=_cycalloc(sizeof(*_tmpF54)),((_tmpF54->hd=(
void*)((void*)((_tmpF58=_cycalloc(sizeof(*_tmpF58)),((_tmpF58[0]=((_tmpF55.tag=
19,((_tmpF55.f1=(void*)((void*)((_tmpF57=_cycalloc(sizeof(*_tmpF57)),((_tmpF57[0]=((
_tmpF56.tag=1,((_tmpF56.f1=_tmp8AF,_tmpF56)))),_tmpF57))))),_tmpF55)))),_tmpF58))))),((
_tmpF54->tl=effect,_tmpF54))))));}goto _LL57B;_LL582: if(*((int*)_tmp8B1)!= 2)goto
_LL584;_tmp8B9=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp8B1)->f2;if((int)
_tmp8B9 != 7)goto _LL584;_LL583: goto _LL585;_LL584: if(*((int*)_tmp8B1)!= 0)goto
_LL586;_tmp8BA=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp8B1)->f1;if((int)
_tmp8BA != 7)goto _LL586;_LL585: goto _LL57B;_LL586: if(*((int*)_tmp8B1)!= 2)goto
_LL588;_tmp8BB=((struct Cyc_Absyn_Less_kb_struct*)_tmp8B1)->f1;_tmp8BC=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*)_tmp8B1)->f1;_tmp8BD=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp8B1)->f2;if((int)_tmp8BD != 6)goto _LL588;_LL587:*_tmp8BC=Cyc_Tcutil_kind_to_bound_opt((
void*)6);goto _LL589;_LL588: if(*((int*)_tmp8B1)!= 0)goto _LL58A;_tmp8BE=(void*)((
struct Cyc_Absyn_Eq_kb_struct*)_tmp8B1)->f1;if((int)_tmp8BE != 6)goto _LL58A;_LL589:{
struct Cyc_Absyn_VarType_struct*_tmpF5E;struct Cyc_Absyn_VarType_struct _tmpF5D;
struct Cyc_List_List*_tmpF5C;effect=((_tmpF5C=_cycalloc(sizeof(*_tmpF5C)),((
_tmpF5C->hd=(void*)((void*)((_tmpF5E=_cycalloc(sizeof(*_tmpF5E)),((_tmpF5E[0]=((
_tmpF5D.tag=1,((_tmpF5D.f1=_tmp8AF,_tmpF5D)))),_tmpF5E))))),((_tmpF5C->tl=effect,
_tmpF5C))))));}goto _LL57B;_LL58A: if(*((int*)_tmp8B1)!= 1)goto _LL58C;_tmp8BF=((
struct Cyc_Absyn_Unknown_kb_struct*)_tmp8B1)->f1;_tmp8C0=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Unknown_kb_struct*)_tmp8B1)->f1;_LL58B:{struct Cyc_Absyn_Less_kb_struct*
_tmpF64;struct Cyc_Absyn_Less_kb_struct _tmpF63;struct Cyc_Core_Opt*_tmpF62;*
_tmp8C0=((_tmpF62=_cycalloc(sizeof(*_tmpF62)),((_tmpF62->v=(void*)((void*)((
_tmpF64=_cycalloc(sizeof(*_tmpF64)),((_tmpF64[0]=((_tmpF63.tag=2,((_tmpF63.f1=0,((
_tmpF63.f2=(void*)((void*)0),_tmpF63)))))),_tmpF64))))),_tmpF62))));}goto _LL58D;
_LL58C:;_LL58D:{struct Cyc_Absyn_RgnsEff_struct*_tmpF73;struct Cyc_Absyn_VarType_struct*
_tmpF72;struct Cyc_Absyn_VarType_struct _tmpF71;struct Cyc_Absyn_RgnsEff_struct
_tmpF70;struct Cyc_List_List*_tmpF6F;effect=((_tmpF6F=_cycalloc(sizeof(*_tmpF6F)),((
_tmpF6F->hd=(void*)((void*)((_tmpF73=_cycalloc(sizeof(*_tmpF73)),((_tmpF73[0]=((
_tmpF70.tag=21,((_tmpF70.f1=(void*)((void*)((_tmpF72=_cycalloc(sizeof(*_tmpF72)),((
_tmpF72[0]=((_tmpF71.tag=1,((_tmpF71.f1=_tmp8AF,_tmpF71)))),_tmpF72))))),_tmpF70)))),
_tmpF73))))),((_tmpF6F->tl=effect,_tmpF6F))))));}goto _LL57B;_LL57B:;}}}{struct
Cyc_List_List*ts=_tmp875.free_evars;for(0;ts != 0;ts=ts->tl){void*_tmp8D2;int
_tmp8D3;struct _tuple19 _tmp8D1=*((struct _tuple19*)ts->hd);_tmp8D2=_tmp8D1.f1;
_tmp8D3=_tmp8D1.f2;if(!_tmp8D3)continue;{void*_tmp8D4=Cyc_Tcutil_typ_kind(
_tmp8D2);_LL58F: if((int)_tmp8D4 != 5)goto _LL591;_LL590: goto _LL592;_LL591: if((int)
_tmp8D4 != 4)goto _LL593;_LL592: goto _LL594;_LL593: if((int)_tmp8D4 != 3)goto _LL595;
_LL594:{struct Cyc_Absyn_AccessEff_struct*_tmpF79;struct Cyc_Absyn_AccessEff_struct
_tmpF78;struct Cyc_List_List*_tmpF77;effect=((_tmpF77=_cycalloc(sizeof(*_tmpF77)),((
_tmpF77->hd=(void*)((void*)((_tmpF79=_cycalloc(sizeof(*_tmpF79)),((_tmpF79[0]=((
_tmpF78.tag=19,((_tmpF78.f1=(void*)_tmp8D2,_tmpF78)))),_tmpF79))))),((_tmpF77->tl=
effect,_tmpF77))))));}goto _LL58E;_LL595: if((int)_tmp8D4 != 6)goto _LL597;_LL596:{
struct Cyc_List_List*_tmpF7A;effect=((_tmpF7A=_cycalloc(sizeof(*_tmpF7A)),((
_tmpF7A->hd=(void*)_tmp8D2,((_tmpF7A->tl=effect,_tmpF7A))))));}goto _LL58E;_LL597:
if((int)_tmp8D4 != 7)goto _LL599;_LL598: goto _LL58E;_LL599:;_LL59A:{struct Cyc_Absyn_RgnsEff_struct*
_tmpF80;struct Cyc_Absyn_RgnsEff_struct _tmpF7F;struct Cyc_List_List*_tmpF7E;effect=((
_tmpF7E=_cycalloc(sizeof(*_tmpF7E)),((_tmpF7E->hd=(void*)((void*)((_tmpF80=
_cycalloc(sizeof(*_tmpF80)),((_tmpF80[0]=((_tmpF7F.tag=21,((_tmpF7F.f1=(void*)
_tmp8D2,_tmpF7F)))),_tmpF80))))),((_tmpF7E->tl=effect,_tmpF7E))))));}goto _LL58E;
_LL58E:;}}}{struct Cyc_Absyn_JoinEff_struct*_tmpF86;struct Cyc_Absyn_JoinEff_struct
_tmpF85;struct Cyc_Core_Opt*_tmpF84;*_tmp7A3=((_tmpF84=_cycalloc(sizeof(*_tmpF84)),((
_tmpF84->v=(void*)((void*)((_tmpF86=_cycalloc(sizeof(*_tmpF86)),((_tmpF86[0]=((
_tmpF85.tag=20,((_tmpF85.f1=effect,_tmpF85)))),_tmpF86))))),_tmpF84))));}}if(*
_tmp7A1 != 0){struct Cyc_List_List*bs=*_tmp7A1;for(0;bs != 0;bs=bs->tl){void*
_tmp8DF=Cyc_Absyn_compress_kb((void*)((struct Cyc_Absyn_Tvar*)bs->hd)->kind);
struct Cyc_Core_Opt*_tmp8E0;struct Cyc_Core_Opt**_tmp8E1;struct Cyc_Core_Opt*
_tmp8E2;struct Cyc_Core_Opt**_tmp8E3;void*_tmp8E4;struct Cyc_Core_Opt*_tmp8E5;
struct Cyc_Core_Opt**_tmp8E6;void*_tmp8E7;struct Cyc_Core_Opt*_tmp8E8;struct Cyc_Core_Opt**
_tmp8E9;void*_tmp8EA;struct Cyc_Core_Opt*_tmp8EB;struct Cyc_Core_Opt**_tmp8EC;void*
_tmp8ED;void*_tmp8EE;_LL59C: if(*((int*)_tmp8DF)!= 1)goto _LL59E;_tmp8E0=((struct
Cyc_Absyn_Unknown_kb_struct*)_tmp8DF)->f1;_tmp8E1=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Unknown_kb_struct*)_tmp8DF)->f1;_LL59D:{const char*_tmpF8A;void*
_tmpF89[1];struct Cyc_String_pa_struct _tmpF88;(_tmpF88.tag=0,((_tmpF88.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)bs->hd)->name),((
_tmpF89[0]=& _tmpF88,Cyc_Tcutil_warn(loc,((_tmpF8A="Type variable %s unconstrained, assuming boxed",
_tag_dyneither(_tmpF8A,sizeof(char),47))),_tag_dyneither(_tmpF89,sizeof(void*),1)))))));}
_tmp8E3=_tmp8E1;goto _LL59F;_LL59E: if(*((int*)_tmp8DF)!= 2)goto _LL5A0;_tmp8E2=((
struct Cyc_Absyn_Less_kb_struct*)_tmp8DF)->f1;_tmp8E3=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*)_tmp8DF)->f1;_tmp8E4=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp8DF)->f2;if((int)_tmp8E4 != 1)goto _LL5A0;_LL59F: _tmp8E6=_tmp8E3;goto _LL5A1;
_LL5A0: if(*((int*)_tmp8DF)!= 2)goto _LL5A2;_tmp8E5=((struct Cyc_Absyn_Less_kb_struct*)
_tmp8DF)->f1;_tmp8E6=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp8DF)->f1;_tmp8E7=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp8DF)->f2;if((
int)_tmp8E7 != 0)goto _LL5A2;_LL5A1:*_tmp8E6=Cyc_Tcutil_kind_to_bound_opt((void*)2);
goto _LL59B;_LL5A2: if(*((int*)_tmp8DF)!= 2)goto _LL5A4;_tmp8E8=((struct Cyc_Absyn_Less_kb_struct*)
_tmp8DF)->f1;_tmp8E9=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp8DF)->f1;_tmp8EA=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp8DF)->f2;if((
int)_tmp8EA != 5)goto _LL5A4;_LL5A3:*_tmp8E9=Cyc_Tcutil_kind_to_bound_opt((void*)3);
goto _LL59B;_LL5A4: if(*((int*)_tmp8DF)!= 2)goto _LL5A6;_tmp8EB=((struct Cyc_Absyn_Less_kb_struct*)
_tmp8DF)->f1;_tmp8EC=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp8DF)->f1;_tmp8ED=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp8DF)->f2;
_LL5A5:*_tmp8EC=Cyc_Tcutil_kind_to_bound_opt(_tmp8ED);goto _LL59B;_LL5A6: if(*((
int*)_tmp8DF)!= 0)goto _LL5A8;_tmp8EE=(void*)((struct Cyc_Absyn_Eq_kb_struct*)
_tmp8DF)->f1;if((int)_tmp8EE != 1)goto _LL5A8;_LL5A7:{const char*_tmpF8D;void*
_tmpF8C;(_tmpF8C=0,Cyc_Tcutil_terr(loc,((_tmpF8D="functions can't abstract M types",
_tag_dyneither(_tmpF8D,sizeof(char),33))),_tag_dyneither(_tmpF8C,sizeof(void*),0)));}
goto _LL59B;_LL5A8:;_LL5A9: goto _LL59B;_LL59B:;}}cvtenv.kind_env=Cyc_Tcutil_remove_bound_tvars(
Cyc_Core_heap_region,_tmp875.kind_env,*_tmp7A1);_tmp875.free_vars=Cyc_Tcutil_remove_bound_tvars_bool(
_tmp875.r,_tmp875.free_vars,*_tmp7A1);{struct Cyc_List_List*tvs=_tmp875.free_vars;
for(0;tvs != 0;tvs=tvs->tl){struct Cyc_Absyn_Tvar*_tmp8F5;int _tmp8F6;struct
_tuple18 _tmp8F4=*((struct _tuple18*)tvs->hd);_tmp8F5=_tmp8F4.f1;_tmp8F6=_tmp8F4.f2;
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,
_tmp8F5,_tmp8F6);}}{struct Cyc_List_List*evs=_tmp875.free_evars;for(0;evs != 0;evs=
evs->tl){void*_tmp8F8;int _tmp8F9;struct _tuple19 _tmp8F7=*((struct _tuple19*)evs->hd);
_tmp8F8=_tmp8F7.f1;_tmp8F9=_tmp8F7.f2;cvtenv.free_evars=Cyc_Tcutil_add_free_evar(
cvtenv.r,cvtenv.free_evars,_tmp8F8,_tmp8F9);}}}goto _LL4D7;}}_LL4F6: if(*((int*)
_tmp777)!= 9)goto _LL4F8;_tmp7AA=((struct Cyc_Absyn_TupleType_struct*)_tmp777)->f1;
_LL4F7: for(0;_tmp7AA != 0;_tmp7AA=_tmp7AA->tl){struct _tuple5*_tmp8FB=(struct
_tuple5*)_tmp7AA->hd;cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)1,(*
_tmp8FB).f2,1);((*_tmp8FB).f1).real_const=Cyc_Tcutil_extract_const_from_typedef(
loc,((*_tmp8FB).f1).print_const,(*_tmp8FB).f2);}goto _LL4D7;_LL4F8: if(*((int*)
_tmp777)!= 11)goto _LL4FA;_tmp7AB=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp777)->f1;_tmp7AC=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp777)->f2;_LL4F9: {
struct _RegionHandle*_tmp8FC=Cyc_Tcenv_get_fnrgn(te);{struct Cyc_List_List*
prev_fields=0;for(0;_tmp7AC != 0;_tmp7AC=_tmp7AC->tl){struct Cyc_Absyn_Aggrfield
_tmp8FE;struct _dyneither_ptr*_tmp8FF;struct Cyc_Absyn_Tqual _tmp900;struct Cyc_Absyn_Tqual*
_tmp901;void*_tmp902;struct Cyc_Absyn_Exp*_tmp903;struct Cyc_List_List*_tmp904;
struct Cyc_Absyn_Aggrfield*_tmp8FD=(struct Cyc_Absyn_Aggrfield*)_tmp7AC->hd;
_tmp8FE=*_tmp8FD;_tmp8FF=_tmp8FE.name;_tmp900=_tmp8FE.tq;_tmp901=(struct Cyc_Absyn_Tqual*)&(*
_tmp8FD).tq;_tmp902=(void*)_tmp8FE.type;_tmp903=_tmp8FE.width;_tmp904=_tmp8FE.attributes;
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*
l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,_tmp8FF)){
const char*_tmpF91;void*_tmpF90[1];struct Cyc_String_pa_struct _tmpF8F;(_tmpF8F.tag=
0,((_tmpF8F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp8FF),((_tmpF90[
0]=& _tmpF8F,Cyc_Tcutil_terr(loc,((_tmpF91="duplicate field %s",_tag_dyneither(
_tmpF91,sizeof(char),19))),_tag_dyneither(_tmpF90,sizeof(void*),1)))))));}{const
char*_tmpF92;if(Cyc_strcmp((struct _dyneither_ptr)*_tmp8FF,((_tmpF92="",
_tag_dyneither(_tmpF92,sizeof(char),1))))!= 0){struct Cyc_List_List*_tmpF93;
prev_fields=((_tmpF93=_region_malloc(_tmp8FC,sizeof(*_tmpF93)),((_tmpF93->hd=
_tmp8FF,((_tmpF93->tl=prev_fields,_tmpF93))))));}}cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,(void*)1,_tmp902,1);_tmp901->real_const=Cyc_Tcutil_extract_const_from_typedef(
loc,_tmp901->print_const,_tmp902);if(_tmp7AB == (void*)1  && !Cyc_Tcutil_bits_only(
_tmp902)){const char*_tmpF97;void*_tmpF96[1];struct Cyc_String_pa_struct _tmpF95;(
_tmpF95.tag=0,((_tmpF95.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp8FF),((
_tmpF96[0]=& _tmpF95,Cyc_Tcutil_warn(loc,((_tmpF97="union member %s is not `bits-only' so it can only be written and not read",
_tag_dyneither(_tmpF97,sizeof(char),74))),_tag_dyneither(_tmpF96,sizeof(void*),1)))))));}
Cyc_Tcutil_check_bitfield(loc,te,_tmp902,_tmp903,_tmp8FF);Cyc_Tcutil_check_field_atts(
loc,_tmp8FF,_tmp904);}}goto _LL4D7;}_LL4FA: if(*((int*)_tmp777)!= 10)goto _LL4FC;
_tmp7AD=((struct Cyc_Absyn_AggrType_struct*)_tmp777)->f1;_tmp7AE=_tmp7AD.aggr_info;
_tmp7AF=(union Cyc_Absyn_AggrInfoU_union*)&(((struct Cyc_Absyn_AggrType_struct*)
_tmp777)->f1).aggr_info;_tmp7B0=_tmp7AD.targs;_tmp7B1=(struct Cyc_List_List**)&(((
struct Cyc_Absyn_AggrType_struct*)_tmp777)->f1).targs;_LL4FB:{union Cyc_Absyn_AggrInfoU_union
_tmp90D=*_tmp7AF;void*_tmp90E;struct _tuple2*_tmp90F;struct Cyc_Absyn_Aggrdecl**
_tmp910;struct Cyc_Absyn_Aggrdecl*_tmp911;_LL5AB: if((_tmp90D.UnknownAggr).tag != 0)
goto _LL5AD;_tmp90E=(_tmp90D.UnknownAggr).f1;_tmp90F=(_tmp90D.UnknownAggr).f2;
_LL5AC: {struct Cyc_Absyn_Aggrdecl**adp;{struct _handler_cons _tmp912;_push_handler(&
_tmp912);{int _tmp914=0;if(setjmp(_tmp912.handler))_tmp914=1;if(!_tmp914){adp=Cyc_Tcenv_lookup_aggrdecl(
te,loc,_tmp90F);{union Cyc_Absyn_AggrInfoU_union _tmpF98;*_tmp7AF=(union Cyc_Absyn_AggrInfoU_union)(((
_tmpF98.KnownAggr).tag=1,(((_tmpF98.KnownAggr).f1=adp,_tmpF98))));};_pop_handler();}
else{void*_tmp913=(void*)_exn_thrown;void*_tmp917=_tmp913;_LL5B0: if(_tmp917 != 
Cyc_Dict_Absent)goto _LL5B2;_LL5B1: {struct Cyc_Tcenv_Genv*_tmp918=((struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct Cyc_Absyn_Aggrdecl*_tmpF99;struct Cyc_Absyn_Aggrdecl*_tmp919=(_tmpF99=
_cycalloc(sizeof(*_tmpF99)),((_tmpF99->kind=(void*)_tmp90E,((_tmpF99->sc=(void*)((
void*)3),((_tmpF99->name=_tmp90F,((_tmpF99->tvs=0,((_tmpF99->impl=0,((_tmpF99->attributes=
0,_tmpF99)))))))))))));Cyc_Tc_tcAggrdecl(te,_tmp918,loc,_tmp919);adp=Cyc_Tcenv_lookup_aggrdecl(
te,loc,_tmp90F);{union Cyc_Absyn_AggrInfoU_union _tmpF9A;*_tmp7AF=(union Cyc_Absyn_AggrInfoU_union)(((
_tmpF9A.KnownAggr).tag=1,(((_tmpF9A.KnownAggr).f1=adp,_tmpF9A))));}if(*_tmp7B1 != 
0){{const char*_tmpF9E;void*_tmpF9D[1];struct Cyc_String_pa_struct _tmpF9C;(_tmpF9C.tag=
0,((_tmpF9C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp90F)),((_tmpF9D[0]=& _tmpF9C,Cyc_Tcutil_terr(loc,((_tmpF9E="declare parameterized type %s before using",
_tag_dyneither(_tmpF9E,sizeof(char),43))),_tag_dyneither(_tmpF9D,sizeof(void*),1)))))));}
return cvtenv;}goto _LL5AF;}_LL5B2:;_LL5B3:(void)_throw(_tmp917);_LL5AF:;}}}
_tmp911=*adp;goto _LL5AE;}_LL5AD: if((_tmp90D.KnownAggr).tag != 1)goto _LL5AA;
_tmp910=(_tmp90D.KnownAggr).f1;_tmp911=*_tmp910;_LL5AE: {struct Cyc_List_List*tvs=
_tmp911->tvs;struct Cyc_List_List*ts=*_tmp7B1;for(0;ts != 0  && tvs != 0;(ts=ts->tl,
tvs=tvs->tl)){void*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd);cvtenv=
Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,(void*)ts->hd,1);}if(ts != 0){const
char*_tmpFA2;void*_tmpFA1[1];struct Cyc_String_pa_struct _tmpFA0;(_tmpFA0.tag=0,((
_tmpFA0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp911->name)),((_tmpFA1[0]=& _tmpFA0,Cyc_Tcutil_terr(loc,((_tmpFA2="too many parameters for type %s",
_tag_dyneither(_tmpFA2,sizeof(char),32))),_tag_dyneither(_tmpFA1,sizeof(void*),1)))))));}
if(tvs != 0){struct Cyc_List_List*hidden_ts=0;for(0;tvs != 0;tvs=tvs->tl){void*k=
Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd);void*e=Cyc_Absyn_new_evar(0,
0);{struct Cyc_List_List*_tmpFA3;hidden_ts=((_tmpFA3=_cycalloc(sizeof(*_tmpFA3)),((
_tmpFA3->hd=(void*)e,((_tmpFA3->tl=hidden_ts,_tmpFA3))))));}cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,k,e,1);}*_tmp7B1=Cyc_List_imp_append(*_tmp7B1,Cyc_List_imp_rev(
hidden_ts));}}_LL5AA:;}goto _LL4D7;_LL4FC: if(*((int*)_tmp777)!= 16)goto _LL4FE;
_tmp7B2=((struct Cyc_Absyn_TypedefType_struct*)_tmp777)->f1;_tmp7B3=((struct Cyc_Absyn_TypedefType_struct*)
_tmp777)->f2;_tmp7B4=(struct Cyc_List_List**)&((struct Cyc_Absyn_TypedefType_struct*)
_tmp777)->f2;_tmp7B5=((struct Cyc_Absyn_TypedefType_struct*)_tmp777)->f3;_tmp7B6=(
struct Cyc_Absyn_Typedefdecl**)&((struct Cyc_Absyn_TypedefType_struct*)_tmp777)->f3;
_tmp7B7=((struct Cyc_Absyn_TypedefType_struct*)_tmp777)->f4;_tmp7B8=(void***)&((
struct Cyc_Absyn_TypedefType_struct*)_tmp777)->f4;_LL4FD: {struct Cyc_List_List*
targs=*_tmp7B4;struct Cyc_Absyn_Typedefdecl*td;{struct _handler_cons _tmp923;
_push_handler(& _tmp923);{int _tmp925=0;if(setjmp(_tmp923.handler))_tmp925=1;if(!
_tmp925){td=Cyc_Tcenv_lookup_typedefdecl(te,loc,_tmp7B2);;_pop_handler();}else{
void*_tmp924=(void*)_exn_thrown;void*_tmp927=_tmp924;_LL5B5: if(_tmp927 != Cyc_Dict_Absent)
goto _LL5B7;_LL5B6:{const char*_tmpFA7;void*_tmpFA6[1];struct Cyc_String_pa_struct
_tmpFA5;(_tmpFA5.tag=0,((_tmpFA5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp7B2)),((_tmpFA6[0]=& _tmpFA5,Cyc_Tcutil_terr(loc,((
_tmpFA7="unbound typedef name %s",_tag_dyneither(_tmpFA7,sizeof(char),24))),
_tag_dyneither(_tmpFA6,sizeof(void*),1)))))));}return cvtenv;_LL5B7:;_LL5B8:(void)
_throw(_tmp927);_LL5B4:;}}}*_tmp7B6=(struct Cyc_Absyn_Typedefdecl*)td;_tmp7B2[0]=(
td->name)[_check_known_subscript_notnull(1,0)];{struct Cyc_List_List*tvs=td->tvs;
struct Cyc_List_List*ts=targs;struct _RegionHandle*_tmp92B=Cyc_Tcenv_get_fnrgn(te);{
struct Cyc_List_List*inst=0;for(0;ts != 0  && tvs != 0;(ts=ts->tl,tvs=tvs->tl)){void*
k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd);cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,k,(void*)ts->hd,1);{struct _tuple8*_tmpFAA;struct Cyc_List_List*
_tmpFA9;inst=((_tmpFA9=_region_malloc(_tmp92B,sizeof(*_tmpFA9)),((_tmpFA9->hd=((
_tmpFAA=_region_malloc(_tmp92B,sizeof(*_tmpFAA)),((_tmpFAA->f1=(struct Cyc_Absyn_Tvar*)
tvs->hd,((_tmpFAA->f2=(void*)ts->hd,_tmpFAA)))))),((_tmpFA9->tl=inst,_tmpFA9))))));}}
if(ts != 0){const char*_tmpFAE;void*_tmpFAD[1];struct Cyc_String_pa_struct _tmpFAC;(
_tmpFAC.tag=0,((_tmpFAC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp7B2)),((_tmpFAD[0]=& _tmpFAC,Cyc_Tcutil_terr(loc,((_tmpFAE="too many parameters for typedef %s",
_tag_dyneither(_tmpFAE,sizeof(char),35))),_tag_dyneither(_tmpFAD,sizeof(void*),1)))))));}
if(tvs != 0){struct Cyc_List_List*hidden_ts=0;for(0;tvs != 0;tvs=tvs->tl){void*k=
Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd);void*e=Cyc_Absyn_new_evar(0,
0);{struct Cyc_List_List*_tmpFAF;hidden_ts=((_tmpFAF=_cycalloc(sizeof(*_tmpFAF)),((
_tmpFAF->hd=(void*)e,((_tmpFAF->tl=hidden_ts,_tmpFAF))))));}cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,k,e,1);{struct _tuple8*_tmpFB2;struct Cyc_List_List*_tmpFB1;inst=(
struct Cyc_List_List*)((_tmpFB1=_cycalloc(sizeof(*_tmpFB1)),((_tmpFB1->hd=(struct
_tuple8*)((_tmpFB2=_cycalloc(sizeof(*_tmpFB2)),((_tmpFB2->f1=(struct Cyc_Absyn_Tvar*)
tvs->hd,((_tmpFB2->f2=e,_tmpFB2)))))),((_tmpFB1->tl=inst,_tmpFB1))))));}}*
_tmp7B4=Cyc_List_imp_append(targs,Cyc_List_imp_rev(hidden_ts));}if(td->defn != 0){
void*new_typ=Cyc_Tcutil_rsubstitute(_tmp92B,inst,(void*)((struct Cyc_Core_Opt*)
_check_null(td->defn))->v);void**_tmpFB3;*_tmp7B8=((_tmpFB3=_cycalloc(sizeof(*
_tmpFB3)),((_tmpFB3[0]=new_typ,_tmpFB3))));}}goto _LL4D7;}}_LL4FE: if((int)_tmp777
!= 3)goto _LL500;_LL4FF: goto _LL501;_LL500: if((int)_tmp777 != 2)goto _LL502;_LL501:
goto _LL4D7;_LL502: if(_tmp777 <= (void*)4)goto _LL504;if(*((int*)_tmp777)!= 14)goto
_LL504;_tmp7B9=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)_tmp777)->f1;
_LL503: cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)5,_tmp7B9,1);
goto _LL4D7;_LL504: if(_tmp777 <= (void*)4)goto _LL506;if(*((int*)_tmp777)!= 15)goto
_LL506;_tmp7BA=(void*)((struct Cyc_Absyn_DynRgnType_struct*)_tmp777)->f1;_tmp7BB=(
void*)((struct Cyc_Absyn_DynRgnType_struct*)_tmp777)->f2;_LL505: cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,(void*)3,_tmp7BA,0);cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,
cvtenv,(void*)3,_tmp7BB,1);goto _LL4D7;_LL506: if(_tmp777 <= (void*)4)goto _LL508;
if(*((int*)_tmp777)!= 19)goto _LL508;_tmp7BC=(void*)((struct Cyc_Absyn_AccessEff_struct*)
_tmp777)->f1;_LL507: cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)5,
_tmp7BC,1);goto _LL4D7;_LL508: if(_tmp777 <= (void*)4)goto _LL50A;if(*((int*)_tmp777)
!= 21)goto _LL50A;_tmp7BD=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp777)->f1;
_LL509: cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)0,_tmp7BD,1);
goto _LL4D7;_LL50A: if(_tmp777 <= (void*)4)goto _LL4D7;if(*((int*)_tmp777)!= 20)goto
_LL4D7;_tmp7BE=((struct Cyc_Absyn_JoinEff_struct*)_tmp777)->f1;_LL50B: for(0;
_tmp7BE != 0;_tmp7BE=_tmp7BE->tl){cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,
cvtenv,(void*)6,(void*)_tmp7BE->hd,1);}goto _LL4D7;_LL4D7:;}if(!Cyc_Tcutil_kind_leq(
Cyc_Tcutil_typ_kind(t),expected_kind)){{void*_tmp935=t;struct Cyc_Core_Opt*
_tmp936;struct Cyc_Core_Opt*_tmp937;_LL5BA: if(_tmp935 <= (void*)4)goto _LL5BC;if(*((
int*)_tmp935)!= 0)goto _LL5BC;_tmp936=((struct Cyc_Absyn_Evar_struct*)_tmp935)->f1;
_tmp937=((struct Cyc_Absyn_Evar_struct*)_tmp935)->f2;_LL5BB: {struct
_dyneither_ptr s;{struct Cyc_Core_Opt*_tmp938=_tmp936;struct Cyc_Core_Opt _tmp939;
void*_tmp93A;_LL5BF: if(_tmp938 != 0)goto _LL5C1;_LL5C0:{const char*_tmpFB4;s=((
_tmpFB4="kind=NULL ",_tag_dyneither(_tmpFB4,sizeof(char),11)));}goto _LL5BE;
_LL5C1: if(_tmp938 == 0)goto _LL5BE;_tmp939=*_tmp938;_tmp93A=(void*)_tmp939.v;
_LL5C2:{const char*_tmpFB8;void*_tmpFB7[1];struct Cyc_String_pa_struct _tmpFB6;s=(
struct _dyneither_ptr)((_tmpFB6.tag=0,((_tmpFB6.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_kind2string(_tmp93A)),((_tmpFB7[0]=& _tmpFB6,Cyc_aprintf(((
_tmpFB8="kind=%s ",_tag_dyneither(_tmpFB8,sizeof(char),9))),_tag_dyneither(
_tmpFB7,sizeof(void*),1))))))));}goto _LL5BE;_LL5BE:;}{struct Cyc_Core_Opt*_tmp93F=
_tmp937;struct Cyc_Core_Opt _tmp940;void*_tmp941;_LL5C4: if(_tmp93F != 0)goto _LL5C6;
_LL5C5:{const char*_tmpFBC;void*_tmpFBB[1];struct Cyc_String_pa_struct _tmpFBA;s=(
struct _dyneither_ptr)((_tmpFBA.tag=0,((_tmpFBA.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)s),((_tmpFBB[0]=& _tmpFBA,Cyc_aprintf(((_tmpFBC="%s ref=NULL",
_tag_dyneither(_tmpFBC,sizeof(char),12))),_tag_dyneither(_tmpFBB,sizeof(void*),1))))))));}
goto _LL5C3;_LL5C6: if(_tmp93F == 0)goto _LL5C3;_tmp940=*_tmp93F;_tmp941=(void*)
_tmp940.v;_LL5C7:{const char*_tmpFC1;void*_tmpFC0[2];struct Cyc_String_pa_struct
_tmpFBF;struct Cyc_String_pa_struct _tmpFBE;s=(struct _dyneither_ptr)((_tmpFBE.tag=
0,((_tmpFBE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
_tmp941)),((_tmpFBF.tag=0,((_tmpFBF.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)s),((_tmpFC0[0]=& _tmpFBF,((_tmpFC0[1]=& _tmpFBE,Cyc_aprintf(((
_tmpFC1="%s ref=%s",_tag_dyneither(_tmpFC1,sizeof(char),10))),_tag_dyneither(
_tmpFC0,sizeof(void*),2))))))))))))));}goto _LL5C3;_LL5C3:;}{const char*_tmpFC5;
void*_tmpFC4[1];struct Cyc_String_pa_struct _tmpFC3;(_tmpFC3.tag=0,((_tmpFC3.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmpFC4[0]=& _tmpFC3,Cyc_fprintf(
Cyc_stderr,((_tmpFC5="evar info: %s\n",_tag_dyneither(_tmpFC5,sizeof(char),15))),
_tag_dyneither(_tmpFC4,sizeof(void*),1)))))));}goto _LL5B9;}_LL5BC:;_LL5BD: goto
_LL5B9;_LL5B9:;}{const char*_tmpFCB;void*_tmpFCA[3];struct Cyc_String_pa_struct
_tmpFC9;struct Cyc_String_pa_struct _tmpFC8;struct Cyc_String_pa_struct _tmpFC7;(
_tmpFC7.tag=0,((_tmpFC7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(
expected_kind)),((_tmpFC8.tag=0,((_tmpFC8.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_kind2string(Cyc_Tcutil_typ_kind(t))),((_tmpFC9.tag=0,((
_tmpFC9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((
_tmpFCA[0]=& _tmpFC9,((_tmpFCA[1]=& _tmpFC8,((_tmpFCA[2]=& _tmpFC7,Cyc_Tcutil_terr(
loc,((_tmpFCB="type %s has kind %s but as used here needs kind %s",
_tag_dyneither(_tmpFCB,sizeof(char),51))),_tag_dyneither(_tmpFCA,sizeof(void*),3)))))))))))))))))));}}
return cvtenv;}static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type_level_exp(
struct Cyc_Absyn_Exp*e,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv);
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type_level_exp(struct Cyc_Absyn_Exp*
e,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv){{void*_tmp951=(void*)e->r;
struct Cyc_List_List*_tmp952;struct Cyc_Absyn_Exp*_tmp953;struct Cyc_Absyn_Exp*
_tmp954;struct Cyc_Absyn_Exp*_tmp955;struct Cyc_Absyn_Exp*_tmp956;struct Cyc_Absyn_Exp*
_tmp957;struct Cyc_Absyn_Exp*_tmp958;struct Cyc_Absyn_Exp*_tmp959;struct Cyc_Absyn_Exp*
_tmp95A;struct Cyc_Absyn_Exp*_tmp95B;void*_tmp95C;struct Cyc_Absyn_Exp*_tmp95D;
void*_tmp95E;void*_tmp95F;void*_tmp960;struct Cyc_Absyn_Exp*_tmp961;_LL5C9: if(*((
int*)_tmp951)!= 0)goto _LL5CB;_LL5CA: goto _LL5CC;_LL5CB: if(*((int*)_tmp951)!= 33)
goto _LL5CD;_LL5CC: goto _LL5CE;_LL5CD: if(*((int*)_tmp951)!= 34)goto _LL5CF;_LL5CE:
goto _LL5D0;_LL5CF: if(*((int*)_tmp951)!= 1)goto _LL5D1;_LL5D0: goto _LL5C8;_LL5D1:
if(*((int*)_tmp951)!= 3)goto _LL5D3;_tmp952=((struct Cyc_Absyn_Primop_e_struct*)
_tmp951)->f2;_LL5D2: for(0;_tmp952 != 0;_tmp952=_tmp952->tl){cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp((
struct Cyc_Absyn_Exp*)_tmp952->hd,te,cvtenv);}goto _LL5C8;_LL5D3: if(*((int*)
_tmp951)!= 6)goto _LL5D5;_tmp953=((struct Cyc_Absyn_Conditional_e_struct*)_tmp951)->f1;
_tmp954=((struct Cyc_Absyn_Conditional_e_struct*)_tmp951)->f2;_tmp955=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp951)->f3;_LL5D4: cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp953,te,
cvtenv);cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp954,te,cvtenv);cvtenv=
Cyc_Tcutil_i_check_valid_type_level_exp(_tmp955,te,cvtenv);goto _LL5C8;_LL5D5: if(*((
int*)_tmp951)!= 7)goto _LL5D7;_tmp956=((struct Cyc_Absyn_And_e_struct*)_tmp951)->f1;
_tmp957=((struct Cyc_Absyn_And_e_struct*)_tmp951)->f2;_LL5D6: _tmp958=_tmp956;
_tmp959=_tmp957;goto _LL5D8;_LL5D7: if(*((int*)_tmp951)!= 8)goto _LL5D9;_tmp958=((
struct Cyc_Absyn_Or_e_struct*)_tmp951)->f1;_tmp959=((struct Cyc_Absyn_Or_e_struct*)
_tmp951)->f2;_LL5D8: _tmp95A=_tmp958;_tmp95B=_tmp959;goto _LL5DA;_LL5D9: if(*((int*)
_tmp951)!= 9)goto _LL5DB;_tmp95A=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp951)->f1;
_tmp95B=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp951)->f2;_LL5DA: cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(
_tmp95A,te,cvtenv);cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp95B,te,
cvtenv);goto _LL5C8;_LL5DB: if(*((int*)_tmp951)!= 15)goto _LL5DD;_tmp95C=(void*)((
struct Cyc_Absyn_Cast_e_struct*)_tmp951)->f1;_tmp95D=((struct Cyc_Absyn_Cast_e_struct*)
_tmp951)->f2;_LL5DC: cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp95D,te,
cvtenv);cvtenv=Cyc_Tcutil_i_check_valid_type(e->loc,te,cvtenv,(void*)0,_tmp95C,1);
goto _LL5C8;_LL5DD: if(*((int*)_tmp951)!= 20)goto _LL5DF;_tmp95E=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)
_tmp951)->f1;_LL5DE: _tmp95F=_tmp95E;goto _LL5E0;_LL5DF: if(*((int*)_tmp951)!= 18)
goto _LL5E1;_tmp95F=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp951)->f1;
_LL5E0: cvtenv=Cyc_Tcutil_i_check_valid_type(e->loc,te,cvtenv,(void*)0,_tmp95F,1);
goto _LL5C8;_LL5E1: if(*((int*)_tmp951)!= 39)goto _LL5E3;_tmp960=(void*)((struct Cyc_Absyn_Valueof_e_struct*)
_tmp951)->f1;_LL5E2: cvtenv=Cyc_Tcutil_i_check_valid_type(e->loc,te,cvtenv,(void*)
7,_tmp960,1);goto _LL5C8;_LL5E3: if(*((int*)_tmp951)!= 19)goto _LL5E5;_tmp961=((
struct Cyc_Absyn_Sizeofexp_e_struct*)_tmp951)->f1;_LL5E4: cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(
_tmp961,te,cvtenv);goto _LL5C8;_LL5E5:;_LL5E6: {const char*_tmpFCE;void*_tmpFCD;(
_tmpFCD=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmpFCE="non-type-level-expression in Tcutil::i_check_valid_type_level_exp",
_tag_dyneither(_tmpFCE,sizeof(char),66))),_tag_dyneither(_tmpFCD,sizeof(void*),0)));}
_LL5C8:;}return cvtenv;}static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_check_valid_type(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv
cvt,void*expected_kind,void*t);static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_check_valid_type(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv
cvt,void*expected_kind,void*t){struct Cyc_List_List*_tmp964=cvt.kind_env;cvt=Cyc_Tcutil_i_check_valid_type(
loc,te,cvt,expected_kind,t,1);{struct Cyc_List_List*vs=cvt.free_vars;for(0;vs != 0;
vs=vs->tl){struct Cyc_Absyn_Tvar*_tmp966;struct _tuple18 _tmp965=*((struct _tuple18*)
vs->hd);_tmp966=_tmp965.f1;cvt.kind_env=Cyc_Tcutil_fast_add_free_tvar(_tmp964,
_tmp966);}}{struct Cyc_List_List*evs=cvt.free_evars;for(0;evs != 0;evs=evs->tl){
void*_tmp968;struct _tuple19 _tmp967=*((struct _tuple19*)evs->hd);_tmp968=_tmp967.f1;{
void*_tmp969=Cyc_Tcutil_compress(_tmp968);struct Cyc_Core_Opt*_tmp96A;struct Cyc_Core_Opt**
_tmp96B;_LL5E8: if(_tmp969 <= (void*)4)goto _LL5EA;if(*((int*)_tmp969)!= 0)goto
_LL5EA;_tmp96A=((struct Cyc_Absyn_Evar_struct*)_tmp969)->f4;_tmp96B=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Evar_struct*)_tmp969)->f4;_LL5E9: if(*_tmp96B == 0){struct Cyc_Core_Opt*
_tmpFCF;*_tmp96B=((_tmpFCF=_cycalloc(sizeof(*_tmpFCF)),((_tmpFCF->v=_tmp964,
_tmpFCF))));}else{struct Cyc_List_List*_tmp96D=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(*_tmp96B))->v;struct Cyc_List_List*_tmp96E=0;for(0;_tmp96D != 0;
_tmp96D=_tmp96D->tl){if(((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,
_tmp964,(struct Cyc_Absyn_Tvar*)_tmp96D->hd)){struct Cyc_List_List*_tmpFD0;_tmp96E=((
_tmpFD0=_cycalloc(sizeof(*_tmpFD0)),((_tmpFD0->hd=(struct Cyc_Absyn_Tvar*)_tmp96D->hd,((
_tmpFD0->tl=_tmp96E,_tmpFD0))))));}}{struct Cyc_Core_Opt*_tmpFD1;*_tmp96B=((
_tmpFD1=_cycalloc(sizeof(*_tmpFD1)),((_tmpFD1->v=_tmp96E,_tmpFD1))));}}goto
_LL5E7;_LL5EA:;_LL5EB: goto _LL5E7;_LL5E7:;}}}return cvt;}void Cyc_Tcutil_check_valid_toplevel_type(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,void*t);void Cyc_Tcutil_check_valid_toplevel_type(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,void*t){int
generalize_evars=Cyc_Tcutil_is_function_type(t);struct Cyc_List_List*_tmp971=Cyc_Tcenv_lookup_type_vars(
te);struct _RegionHandle*_tmp972=Cyc_Tcenv_get_fnrgn(te);struct Cyc_Tcutil_CVTEnv
_tmpFD2;struct Cyc_Tcutil_CVTEnv _tmp973=Cyc_Tcutil_check_valid_type(loc,te,((
_tmpFD2.r=_tmp972,((_tmpFD2.kind_env=_tmp971,((_tmpFD2.free_vars=0,((_tmpFD2.free_evars=
0,((_tmpFD2.generalize_evars=generalize_evars,((_tmpFD2.fn_result=0,_tmpFD2)))))))))))),(
void*)1,t);struct Cyc_List_List*_tmp974=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct Cyc_Absyn_Tvar*(*f)(struct _tuple18*),struct Cyc_List_List*x))
Cyc_List_rmap)(_tmp972,(struct Cyc_Absyn_Tvar*(*)(struct _tuple18*))Cyc_Core_fst,
_tmp973.free_vars);struct Cyc_List_List*_tmp975=((struct Cyc_List_List*(*)(struct
_RegionHandle*,void*(*f)(struct _tuple19*),struct Cyc_List_List*x))Cyc_List_rmap)(
_tmp972,(void*(*)(struct _tuple19*))Cyc_Core_fst,_tmp973.free_evars);if(_tmp971 != 
0){struct Cyc_List_List*_tmp976=0;{struct Cyc_List_List*_tmp977=_tmp974;for(0;(
unsigned int)_tmp977;_tmp977=_tmp977->tl){struct Cyc_Absyn_Tvar*_tmp978=(struct
Cyc_Absyn_Tvar*)_tmp977->hd;int found=0;{struct Cyc_List_List*_tmp979=_tmp971;for(
0;(unsigned int)_tmp979;_tmp979=_tmp979->tl){if(Cyc_Absyn_tvar_cmp(_tmp978,(
struct Cyc_Absyn_Tvar*)_tmp979->hd)== 0){found=1;break;}}}if(!found){struct Cyc_List_List*
_tmpFD3;_tmp976=((_tmpFD3=_region_malloc(_tmp972,sizeof(*_tmpFD3)),((_tmpFD3->hd=(
struct Cyc_Absyn_Tvar*)_tmp977->hd,((_tmpFD3->tl=_tmp976,_tmpFD3))))));}}}_tmp974=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp976);}{
struct Cyc_List_List*x=_tmp974;for(0;x != 0;x=x->tl){void*_tmp97B=Cyc_Absyn_compress_kb((
void*)((struct Cyc_Absyn_Tvar*)x->hd)->kind);struct Cyc_Core_Opt*_tmp97C;struct Cyc_Core_Opt**
_tmp97D;struct Cyc_Core_Opt*_tmp97E;struct Cyc_Core_Opt**_tmp97F;void*_tmp980;
struct Cyc_Core_Opt*_tmp981;struct Cyc_Core_Opt**_tmp982;void*_tmp983;struct Cyc_Core_Opt*
_tmp984;struct Cyc_Core_Opt**_tmp985;void*_tmp986;void*_tmp987;_LL5ED: if(*((int*)
_tmp97B)!= 1)goto _LL5EF;_tmp97C=((struct Cyc_Absyn_Unknown_kb_struct*)_tmp97B)->f1;
_tmp97D=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)_tmp97B)->f1;
_LL5EE: _tmp97F=_tmp97D;goto _LL5F0;_LL5EF: if(*((int*)_tmp97B)!= 2)goto _LL5F1;
_tmp97E=((struct Cyc_Absyn_Less_kb_struct*)_tmp97B)->f1;_tmp97F=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*)_tmp97B)->f1;_tmp980=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp97B)->f2;if((int)_tmp980 != 1)goto _LL5F1;_LL5F0:*_tmp97F=Cyc_Tcutil_kind_to_bound_opt((
void*)2);goto _LL5EC;_LL5F1: if(*((int*)_tmp97B)!= 2)goto _LL5F3;_tmp981=((struct
Cyc_Absyn_Less_kb_struct*)_tmp97B)->f1;_tmp982=(struct Cyc_Core_Opt**)&((struct
Cyc_Absyn_Less_kb_struct*)_tmp97B)->f1;_tmp983=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp97B)->f2;if((int)_tmp983 != 5)goto _LL5F3;_LL5F2:*_tmp982=Cyc_Tcutil_kind_to_bound_opt((
void*)3);goto _LL5EC;_LL5F3: if(*((int*)_tmp97B)!= 2)goto _LL5F5;_tmp984=((struct
Cyc_Absyn_Less_kb_struct*)_tmp97B)->f1;_tmp985=(struct Cyc_Core_Opt**)&((struct
Cyc_Absyn_Less_kb_struct*)_tmp97B)->f1;_tmp986=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp97B)->f2;_LL5F4:*_tmp985=Cyc_Tcutil_kind_to_bound_opt(_tmp986);goto _LL5EC;
_LL5F5: if(*((int*)_tmp97B)!= 0)goto _LL5F7;_tmp987=(void*)((struct Cyc_Absyn_Eq_kb_struct*)
_tmp97B)->f1;if((int)_tmp987 != 1)goto _LL5F7;_LL5F6:{const char*_tmpFD7;void*
_tmpFD6[1];struct Cyc_String_pa_struct _tmpFD5;(_tmpFD5.tag=0,((_tmpFD5.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_tvar2string((struct Cyc_Absyn_Tvar*)
x->hd)),((_tmpFD6[0]=& _tmpFD5,Cyc_Tcutil_terr(loc,((_tmpFD7="type variable %s cannot have kind M",
_tag_dyneither(_tmpFD7,sizeof(char),36))),_tag_dyneither(_tmpFD6,sizeof(void*),1)))))));}
goto _LL5EC;_LL5F7:;_LL5F8: goto _LL5EC;_LL5EC:;}}if(_tmp974 != 0  || _tmp975 != 0){{
void*_tmp98B=Cyc_Tcutil_compress(t);struct Cyc_Absyn_FnInfo _tmp98C;struct Cyc_List_List*
_tmp98D;struct Cyc_List_List**_tmp98E;struct Cyc_Core_Opt*_tmp98F;void*_tmp990;
struct Cyc_List_List*_tmp991;int _tmp992;struct Cyc_Absyn_VarargInfo*_tmp993;struct
Cyc_List_List*_tmp994;struct Cyc_List_List*_tmp995;_LL5FA: if(_tmp98B <= (void*)4)
goto _LL5FC;if(*((int*)_tmp98B)!= 8)goto _LL5FC;_tmp98C=((struct Cyc_Absyn_FnType_struct*)
_tmp98B)->f1;_tmp98D=_tmp98C.tvars;_tmp98E=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_struct*)
_tmp98B)->f1).tvars;_tmp98F=_tmp98C.effect;_tmp990=(void*)_tmp98C.ret_typ;
_tmp991=_tmp98C.args;_tmp992=_tmp98C.c_varargs;_tmp993=_tmp98C.cyc_varargs;
_tmp994=_tmp98C.rgn_po;_tmp995=_tmp98C.attributes;_LL5FB: if(*_tmp98E == 0){*
_tmp98E=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_copy)(_tmp974);
_tmp974=0;}goto _LL5F9;_LL5FC:;_LL5FD: goto _LL5F9;_LL5F9:;}if(_tmp974 != 0){const
char*_tmpFDB;void*_tmpFDA[1];struct Cyc_String_pa_struct _tmpFD9;(_tmpFD9.tag=0,((
_tmpFD9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)
_tmp974->hd)->name),((_tmpFDA[0]=& _tmpFD9,Cyc_Tcutil_terr(loc,((_tmpFDB="unbound type variable %s",
_tag_dyneither(_tmpFDB,sizeof(char),25))),_tag_dyneither(_tmpFDA,sizeof(void*),1)))))));}
if(_tmp975 != 0)for(0;_tmp975 != 0;_tmp975=_tmp975->tl){void*e=(void*)_tmp975->hd;
void*_tmp999=Cyc_Tcutil_typ_kind(e);_LL5FF: if((int)_tmp999 != 4)goto _LL601;_LL600:
if(!Cyc_Tcutil_unify(e,(void*)3)){const char*_tmpFDE;void*_tmpFDD;(_tmpFDD=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmpFDE="can't unify evar with unique region!",_tag_dyneither(_tmpFDE,sizeof(
char),37))),_tag_dyneither(_tmpFDD,sizeof(void*),0)));}goto _LL5FE;_LL601: if((int)
_tmp999 != 5)goto _LL603;_LL602: goto _LL604;_LL603: if((int)_tmp999 != 3)goto _LL605;
_LL604: if(!Cyc_Tcutil_unify(e,(void*)2)){const char*_tmpFE1;void*_tmpFE0;(_tmpFE0=
0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmpFE1="can't unify evar with heap!",_tag_dyneither(_tmpFE1,sizeof(char),28))),
_tag_dyneither(_tmpFE0,sizeof(void*),0)));}goto _LL5FE;_LL605: if((int)_tmp999 != 6)
goto _LL607;_LL606: if(!Cyc_Tcutil_unify(e,Cyc_Absyn_empty_effect)){const char*
_tmpFE4;void*_tmpFE3;(_tmpFE3=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpFE4="can't unify evar with {}!",
_tag_dyneither(_tmpFE4,sizeof(char),26))),_tag_dyneither(_tmpFE3,sizeof(void*),0)));}
goto _LL5FE;_LL607:;_LL608:{const char*_tmpFE9;void*_tmpFE8[2];struct Cyc_String_pa_struct
_tmpFE7;struct Cyc_String_pa_struct _tmpFE6;(_tmpFE6.tag=0,((_tmpFE6.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpFE7.tag=0,((
_tmpFE7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(e)),((
_tmpFE8[0]=& _tmpFE7,((_tmpFE8[1]=& _tmpFE6,Cyc_Tcutil_terr(loc,((_tmpFE9="hidden type variable %s isn't abstracted in type %s",
_tag_dyneither(_tmpFE9,sizeof(char),52))),_tag_dyneither(_tmpFE8,sizeof(void*),2)))))))))))));}
goto _LL5FE;_LL5FE:;}}}void Cyc_Tcutil_check_fndecl_valid_type(struct Cyc_Position_Segment*
loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Fndecl*fd);void Cyc_Tcutil_check_fndecl_valid_type(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Fndecl*fd){
void*t=Cyc_Tcutil_fndecl2typ(fd);Cyc_Tcutil_check_valid_toplevel_type(loc,te,t);{
void*_tmp9A5=Cyc_Tcutil_compress(t);struct Cyc_Absyn_FnInfo _tmp9A6;struct Cyc_List_List*
_tmp9A7;struct Cyc_Core_Opt*_tmp9A8;void*_tmp9A9;_LL60A: if(_tmp9A5 <= (void*)4)
goto _LL60C;if(*((int*)_tmp9A5)!= 8)goto _LL60C;_tmp9A6=((struct Cyc_Absyn_FnType_struct*)
_tmp9A5)->f1;_tmp9A7=_tmp9A6.tvars;_tmp9A8=_tmp9A6.effect;_tmp9A9=(void*)_tmp9A6.ret_typ;
_LL60B: fd->tvs=_tmp9A7;fd->effect=_tmp9A8;goto _LL609;_LL60C:;_LL60D: {const char*
_tmpFEC;void*_tmpFEB;(_tmpFEB=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpFEC="check_fndecl_valid_type: not a FnType",
_tag_dyneither(_tmpFEC,sizeof(char),38))),_tag_dyneither(_tmpFEB,sizeof(void*),0)));}
_LL609:;}{struct _RegionHandle*_tmp9AC=Cyc_Tcenv_get_fnrgn(te);{const char*_tmpFED;
Cyc_Tcutil_check_unique_vars(((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct _dyneither_ptr*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_rmap)(
_tmp9AC,(struct _dyneither_ptr*(*)(struct _tuple14*t))Cyc_Tcutil_fst_fdarg,fd->args),
loc,((_tmpFED="function declaration has repeated parameter",_tag_dyneither(
_tmpFED,sizeof(char),44))));}{struct Cyc_Core_Opt*_tmpFEE;fd->cached_typ=((
_tmpFEE=_cycalloc(sizeof(*_tmpFEE)),((_tmpFEE->v=(void*)t,_tmpFEE))));}}}void Cyc_Tcutil_check_type(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*
bound_tvars,void*expected_kind,int allow_evars,void*t);void Cyc_Tcutil_check_type(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*
bound_tvars,void*expected_kind,int allow_evars,void*t){struct _RegionHandle*
_tmp9AF=Cyc_Tcenv_get_fnrgn(te);struct Cyc_Tcutil_CVTEnv _tmpFEF;struct Cyc_Tcutil_CVTEnv
_tmp9B0=Cyc_Tcutil_check_valid_type(loc,te,((_tmpFEF.r=_tmp9AF,((_tmpFEF.kind_env=
bound_tvars,((_tmpFEF.free_vars=0,((_tmpFEF.free_evars=0,((_tmpFEF.generalize_evars=
0,((_tmpFEF.fn_result=0,_tmpFEF)))))))))))),expected_kind,t);struct Cyc_List_List*
_tmp9B1=Cyc_Tcutil_remove_bound_tvars(_tmp9AF,((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct Cyc_Absyn_Tvar*(*f)(struct _tuple18*),struct Cyc_List_List*x))
Cyc_List_rmap)(_tmp9AF,(struct Cyc_Absyn_Tvar*(*)(struct _tuple18*))Cyc_Core_fst,
_tmp9B0.free_vars),bound_tvars);struct Cyc_List_List*_tmp9B2=((struct Cyc_List_List*(*)(
struct _RegionHandle*,void*(*f)(struct _tuple19*),struct Cyc_List_List*x))Cyc_List_rmap)(
_tmp9AF,(void*(*)(struct _tuple19*))Cyc_Core_fst,_tmp9B0.free_evars);{struct Cyc_List_List*
fs=_tmp9B1;for(0;fs != 0;fs=fs->tl){struct _dyneither_ptr*_tmp9B3=((struct Cyc_Absyn_Tvar*)
fs->hd)->name;const char*_tmpFF4;void*_tmpFF3[2];struct Cyc_String_pa_struct
_tmpFF2;struct Cyc_String_pa_struct _tmpFF1;(_tmpFF1.tag=0,((_tmpFF1.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpFF2.tag=0,((
_tmpFF2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp9B3),((_tmpFF3[0]=&
_tmpFF2,((_tmpFF3[1]=& _tmpFF1,Cyc_Tcutil_terr(loc,((_tmpFF4="unbound type variable %s in type %s",
_tag_dyneither(_tmpFF4,sizeof(char),36))),_tag_dyneither(_tmpFF3,sizeof(void*),2)))))))))))));}}
if(!allow_evars  && _tmp9B2 != 0)for(0;_tmp9B2 != 0;_tmp9B2=_tmp9B2->tl){void*e=(
void*)_tmp9B2->hd;void*_tmp9B8=Cyc_Tcutil_typ_kind(e);_LL60F: if((int)_tmp9B8 != 4)
goto _LL611;_LL610: if(!Cyc_Tcutil_unify(e,(void*)3)){const char*_tmpFF7;void*
_tmpFF6;(_tmpFF6=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmpFF7="can't unify evar with unique region!",_tag_dyneither(_tmpFF7,sizeof(
char),37))),_tag_dyneither(_tmpFF6,sizeof(void*),0)));}goto _LL60E;_LL611: if((int)
_tmp9B8 != 5)goto _LL613;_LL612: goto _LL614;_LL613: if((int)_tmp9B8 != 3)goto _LL615;
_LL614: if(!Cyc_Tcutil_unify(e,(void*)2)){const char*_tmpFFA;void*_tmpFF9;(_tmpFF9=
0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmpFFA="can't unify evar with heap!",_tag_dyneither(_tmpFFA,sizeof(char),28))),
_tag_dyneither(_tmpFF9,sizeof(void*),0)));}goto _LL60E;_LL615: if((int)_tmp9B8 != 6)
goto _LL617;_LL616: if(!Cyc_Tcutil_unify(e,Cyc_Absyn_empty_effect)){const char*
_tmpFFD;void*_tmpFFC;(_tmpFFC=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpFFD="can't unify evar with {}!",
_tag_dyneither(_tmpFFD,sizeof(char),26))),_tag_dyneither(_tmpFFC,sizeof(void*),0)));}
goto _LL60E;_LL617:;_LL618:{const char*_tmp1002;void*_tmp1001[2];struct Cyc_String_pa_struct
_tmp1000;struct Cyc_String_pa_struct _tmpFFF;(_tmpFFF.tag=0,((_tmpFFF.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp1000.tag=
0,((_tmp1000.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
e)),((_tmp1001[0]=& _tmp1000,((_tmp1001[1]=& _tmpFFF,Cyc_Tcutil_terr(loc,((
_tmp1002="hidden type variable %s isn't abstracted in type %s",_tag_dyneither(
_tmp1002,sizeof(char),52))),_tag_dyneither(_tmp1001,sizeof(void*),2)))))))))))));}
goto _LL60E;_LL60E:;}}void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*tv);
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
vs->hd,(void*)vs2->hd)== 0){const char*_tmp1007;void*_tmp1006[2];struct Cyc_String_pa_struct
_tmp1005;struct Cyc_String_pa_struct _tmp1004;(_tmp1004.tag=0,((_tmp1004.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)a2string((void*)vs->hd)),((_tmp1005.tag=
0,((_tmp1005.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg),((_tmp1006[0]=&
_tmp1005,((_tmp1006[1]=& _tmp1004,Cyc_Tcutil_terr(loc,((_tmp1007="%s: %s",
_tag_dyneither(_tmp1007,sizeof(char),7))),_tag_dyneither(_tmp1006,sizeof(void*),
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
struct Cyc_Position_Segment*loc,struct Cyc_List_List*tvs){const char*_tmp1008;((
void(*)(int(*cmp)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*
vs,struct Cyc_Position_Segment*loc,struct _dyneither_ptr(*a2string)(struct Cyc_Absyn_Tvar*),
struct _dyneither_ptr msg))Cyc_Tcutil_check_unique_unsorted)(Cyc_Absyn_tvar_cmp,
tvs,loc,Cyc_Tcutil_tvar2string,((_tmp1008="duplicate type variable",
_tag_dyneither(_tmp1008,sizeof(char),24))));}struct _tuple20{struct Cyc_Absyn_Aggrfield*
f1;int f2;};struct _tuple21{struct Cyc_List_List*f1;void*f2;};struct _tuple22{struct
Cyc_Absyn_Aggrfield*f1;void*f2;};struct Cyc_List_List*Cyc_Tcutil_resolve_struct_designators(
struct _RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,
struct Cyc_List_List*sdfields);struct Cyc_List_List*Cyc_Tcutil_resolve_struct_designators(
struct _RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,
struct Cyc_List_List*sdfields){struct Cyc_List_List*fields=0;{struct Cyc_List_List*
sd_fields=sdfields;for(0;sd_fields != 0;sd_fields=sd_fields->tl){const char*
_tmp1009;if(Cyc_strcmp((struct _dyneither_ptr)*((struct Cyc_Absyn_Aggrfield*)
sd_fields->hd)->name,((_tmp1009="",_tag_dyneither(_tmp1009,sizeof(char),1))))!= 
0){struct _tuple20*_tmp100C;struct Cyc_List_List*_tmp100B;fields=((_tmp100B=
_cycalloc(sizeof(*_tmp100B)),((_tmp100B->hd=((_tmp100C=_cycalloc(sizeof(*
_tmp100C)),((_tmp100C->f1=(struct Cyc_Absyn_Aggrfield*)sd_fields->hd,((_tmp100C->f2=
0,_tmp100C)))))),((_tmp100B->tl=fields,_tmp100B))))));}}}fields=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(fields);{struct Cyc_List_List*ans=0;for(
0;des != 0;des=des->tl){struct _tuple21 _tmp9CD;struct Cyc_List_List*_tmp9CE;void*
_tmp9CF;struct _tuple21*_tmp9CC=(struct _tuple21*)des->hd;_tmp9CD=*_tmp9CC;_tmp9CE=
_tmp9CD.f1;_tmp9CF=_tmp9CD.f2;if(_tmp9CE == 0){struct Cyc_List_List*_tmp9D0=fields;
for(0;_tmp9D0 != 0;_tmp9D0=_tmp9D0->tl){if(!(*((struct _tuple20*)_tmp9D0->hd)).f2){(*((
struct _tuple20*)_tmp9D0->hd)).f2=1;{struct Cyc_Absyn_FieldName_struct*_tmp1012;
struct Cyc_Absyn_FieldName_struct _tmp1011;struct Cyc_List_List*_tmp1010;(*((struct
_tuple21*)des->hd)).f1=(struct Cyc_List_List*)((_tmp1010=_cycalloc(sizeof(*
_tmp1010)),((_tmp1010->hd=(void*)((void*)((_tmp1012=_cycalloc(sizeof(*_tmp1012)),((
_tmp1012[0]=((_tmp1011.tag=1,((_tmp1011.f1=((*((struct _tuple20*)_tmp9D0->hd)).f1)->name,
_tmp1011)))),_tmp1012))))),((_tmp1010->tl=0,_tmp1010))))));}{struct _tuple22*
_tmp1015;struct Cyc_List_List*_tmp1014;ans=((_tmp1014=_region_malloc(rgn,sizeof(*
_tmp1014)),((_tmp1014->hd=((_tmp1015=_region_malloc(rgn,sizeof(*_tmp1015)),((
_tmp1015->f1=(*((struct _tuple20*)_tmp9D0->hd)).f1,((_tmp1015->f2=_tmp9CF,
_tmp1015)))))),((_tmp1014->tl=ans,_tmp1014))))));}break;}}if(_tmp9D0 == 0){const
char*_tmp1018;void*_tmp1017;(_tmp1017=0,Cyc_Tcutil_terr(loc,((_tmp1018="too many arguments to struct",
_tag_dyneither(_tmp1018,sizeof(char),29))),_tag_dyneither(_tmp1017,sizeof(void*),
0)));}}else{if(_tmp9CE->tl != 0){const char*_tmp101B;void*_tmp101A;(_tmp101A=0,Cyc_Tcutil_terr(
loc,((_tmp101B="multiple designators are not supported",_tag_dyneither(_tmp101B,
sizeof(char),39))),_tag_dyneither(_tmp101A,sizeof(void*),0)));}else{void*_tmp9DA=(
void*)_tmp9CE->hd;struct _dyneither_ptr*_tmp9DB;_LL61A: if(*((int*)_tmp9DA)!= 0)
goto _LL61C;_LL61B:{const char*_tmp101E;void*_tmp101D;(_tmp101D=0,Cyc_Tcutil_terr(
loc,((_tmp101E="array designator used in argument to struct",_tag_dyneither(
_tmp101E,sizeof(char),44))),_tag_dyneither(_tmp101D,sizeof(void*),0)));}goto
_LL619;_LL61C: if(*((int*)_tmp9DA)!= 1)goto _LL619;_tmp9DB=((struct Cyc_Absyn_FieldName_struct*)
_tmp9DA)->f1;_LL61D: {struct Cyc_List_List*_tmp9DE=fields;for(0;_tmp9DE != 0;
_tmp9DE=_tmp9DE->tl){if(Cyc_strptrcmp(_tmp9DB,((*((struct _tuple20*)_tmp9DE->hd)).f1)->name)
== 0){if((*((struct _tuple20*)_tmp9DE->hd)).f2){const char*_tmp1022;void*_tmp1021[
1];struct Cyc_String_pa_struct _tmp1020;(_tmp1020.tag=0,((_tmp1020.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*_tmp9DB),((_tmp1021[0]=& _tmp1020,Cyc_Tcutil_terr(
loc,((_tmp1022="field %s has already been used as an argument",_tag_dyneither(
_tmp1022,sizeof(char),46))),_tag_dyneither(_tmp1021,sizeof(void*),1)))))));}(*((
struct _tuple20*)_tmp9DE->hd)).f2=1;{struct _tuple22*_tmp1025;struct Cyc_List_List*
_tmp1024;ans=((_tmp1024=_region_malloc(rgn,sizeof(*_tmp1024)),((_tmp1024->hd=((
_tmp1025=_region_malloc(rgn,sizeof(*_tmp1025)),((_tmp1025->f1=(*((struct _tuple20*)
_tmp9DE->hd)).f1,((_tmp1025->f2=_tmp9CF,_tmp1025)))))),((_tmp1024->tl=ans,
_tmp1024))))));}break;}}if(_tmp9DE == 0){const char*_tmp1029;void*_tmp1028[1];
struct Cyc_String_pa_struct _tmp1027;(_tmp1027.tag=0,((_tmp1027.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*_tmp9DB),((_tmp1028[0]=& _tmp1027,Cyc_Tcutil_terr(
loc,((_tmp1029="bad field designator %s",_tag_dyneither(_tmp1029,sizeof(char),24))),
_tag_dyneither(_tmp1028,sizeof(void*),1)))))));}goto _LL619;}_LL619:;}}}for(0;
fields != 0;fields=fields->tl){if(!(*((struct _tuple20*)fields->hd)).f2){{const
char*_tmp102C;void*_tmp102B;(_tmp102B=0,Cyc_Tcutil_terr(loc,((_tmp102C="too few arguments to struct",
_tag_dyneither(_tmp102C,sizeof(char),28))),_tag_dyneither(_tmp102B,sizeof(void*),
0)));}break;}}return((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
ans);}}int Cyc_Tcutil_is_tagged_pointer_typ_elt(void*t,void**elt_typ_dest);int Cyc_Tcutil_is_tagged_pointer_typ_elt(
void*t,void**elt_typ_dest){void*_tmp9E9=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo
_tmp9EA;void*_tmp9EB;struct Cyc_Absyn_PtrAtts _tmp9EC;struct Cyc_Absyn_Conref*
_tmp9ED;_LL61F: if(_tmp9E9 <= (void*)4)goto _LL621;if(*((int*)_tmp9E9)!= 4)goto
_LL621;_tmp9EA=((struct Cyc_Absyn_PointerType_struct*)_tmp9E9)->f1;_tmp9EB=(void*)
_tmp9EA.elt_typ;_tmp9EC=_tmp9EA.ptr_atts;_tmp9ED=_tmp9EC.bounds;_LL620: {struct
Cyc_Absyn_Conref*_tmp9EE=Cyc_Absyn_compress_conref(_tmp9ED);union Cyc_Absyn_Constraint_union
_tmp9EF=(Cyc_Absyn_compress_conref(_tmp9EE))->v;void*_tmp9F0;_LL624: if((_tmp9EF.Eq_constr).tag
!= 0)goto _LL626;_tmp9F0=(_tmp9EF.Eq_constr).f1;if((int)_tmp9F0 != 0)goto _LL626;
_LL625:*elt_typ_dest=_tmp9EB;return 1;_LL626: if((_tmp9EF.No_constr).tag != 2)goto
_LL628;_LL627:{union Cyc_Absyn_Constraint_union _tmp102D;_tmp9EE->v=(union Cyc_Absyn_Constraint_union)(((
_tmp102D.Eq_constr).tag=0,(((_tmp102D.Eq_constr).f1=(void*)((void*)0),_tmp102D))));}*
elt_typ_dest=_tmp9EB;return 1;_LL628:;_LL629: return 0;_LL623:;}_LL621:;_LL622:
return 0;_LL61E:;}int Cyc_Tcutil_is_zero_pointer_typ_elt(void*t,void**elt_typ_dest);
int Cyc_Tcutil_is_zero_pointer_typ_elt(void*t,void**elt_typ_dest){void*_tmp9F2=
Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp9F3;void*_tmp9F4;struct Cyc_Absyn_PtrAtts
_tmp9F5;struct Cyc_Absyn_Conref*_tmp9F6;_LL62B: if(_tmp9F2 <= (void*)4)goto _LL62D;
if(*((int*)_tmp9F2)!= 4)goto _LL62D;_tmp9F3=((struct Cyc_Absyn_PointerType_struct*)
_tmp9F2)->f1;_tmp9F4=(void*)_tmp9F3.elt_typ;_tmp9F5=_tmp9F3.ptr_atts;_tmp9F6=
_tmp9F5.zero_term;_LL62C:*elt_typ_dest=_tmp9F4;return((int(*)(int,struct Cyc_Absyn_Conref*
x))Cyc_Absyn_conref_def)(0,_tmp9F6);_LL62D:;_LL62E: return 0;_LL62A:;}int Cyc_Tcutil_is_zero_ptr_type(
void*t,void**ptr_type,int*is_dyneither,void**elt_type);int Cyc_Tcutil_is_zero_ptr_type(
void*t,void**ptr_type,int*is_dyneither,void**elt_type){void*_tmp9F7=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_PtrInfo _tmp9F8;void*_tmp9F9;struct Cyc_Absyn_PtrAtts _tmp9FA;
struct Cyc_Absyn_Conref*_tmp9FB;struct Cyc_Absyn_Conref*_tmp9FC;struct Cyc_Absyn_ArrayInfo
_tmp9FD;void*_tmp9FE;struct Cyc_Absyn_Tqual _tmp9FF;struct Cyc_Absyn_Exp*_tmpA00;
struct Cyc_Absyn_Conref*_tmpA01;_LL630: if(_tmp9F7 <= (void*)4)goto _LL634;if(*((int*)
_tmp9F7)!= 4)goto _LL632;_tmp9F8=((struct Cyc_Absyn_PointerType_struct*)_tmp9F7)->f1;
_tmp9F9=(void*)_tmp9F8.elt_typ;_tmp9FA=_tmp9F8.ptr_atts;_tmp9FB=_tmp9FA.bounds;
_tmp9FC=_tmp9FA.zero_term;_LL631: if(((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,_tmp9FC)){*ptr_type=t;*elt_type=_tmp9F9;{void*_tmpA02=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,
_tmp9FB);_LL637: if((int)_tmpA02 != 0)goto _LL639;_LL638:*is_dyneither=1;goto _LL636;
_LL639:;_LL63A:*is_dyneither=0;goto _LL636;_LL636:;}return 1;}else{return 0;}_LL632:
if(*((int*)_tmp9F7)!= 7)goto _LL634;_tmp9FD=((struct Cyc_Absyn_ArrayType_struct*)
_tmp9F7)->f1;_tmp9FE=(void*)_tmp9FD.elt_type;_tmp9FF=_tmp9FD.tq;_tmpA00=_tmp9FD.num_elts;
_tmpA01=_tmp9FD.zero_term;_LL633: if(((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,_tmpA01)){*elt_type=_tmp9FE;*is_dyneither=0;{struct Cyc_Absyn_PointerType_struct
_tmp1042;struct Cyc_Absyn_PtrAtts _tmp1041;struct Cyc_Absyn_Upper_b_struct _tmp1040;
struct Cyc_Absyn_Upper_b_struct*_tmp103F;struct Cyc_Absyn_PtrInfo _tmp103E;struct
Cyc_Absyn_PointerType_struct*_tmp103D;*ptr_type=(void*)((_tmp103D=_cycalloc(
sizeof(*_tmp103D)),((_tmp103D[0]=((_tmp1042.tag=4,((_tmp1042.f1=((_tmp103E.elt_typ=(
void*)_tmp9FE,((_tmp103E.elt_tq=_tmp9FF,((_tmp103E.ptr_atts=((_tmp1041.rgn=(void*)((
void*)2),((_tmp1041.nullable=Cyc_Absyn_false_conref,((_tmp1041.bounds=Cyc_Absyn_new_conref((
void*)((_tmp103F=_cycalloc(sizeof(*_tmp103F)),((_tmp103F[0]=((_tmp1040.tag=0,((
_tmp1040.f1=(struct Cyc_Absyn_Exp*)_check_null(_tmpA00),_tmp1040)))),_tmp103F))))),((
_tmp1041.zero_term=_tmpA01,((_tmp1041.ptrloc=0,_tmp1041)))))))))),_tmp103E)))))),
_tmp1042)))),_tmp103D))));}return 1;}else{return 0;}_LL634:;_LL635: return 0;_LL62F:;}
int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*e1,void**ptr_type,int*
is_dyneither,void**elt_type);int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*
e1,void**ptr_type,int*is_dyneither,void**elt_type){void*_tmpA09=(void*)e1->r;
struct Cyc_Absyn_Exp*_tmpA0A;struct Cyc_Absyn_Exp*_tmpA0B;struct Cyc_Absyn_Exp*
_tmpA0C;struct Cyc_Absyn_Exp*_tmpA0D;struct Cyc_Absyn_Exp*_tmpA0E;struct Cyc_Absyn_Exp*
_tmpA0F;_LL63C: if(*((int*)_tmpA09)!= 15)goto _LL63E;_LL63D: {const char*_tmp1046;
void*_tmp1045[1];struct Cyc_String_pa_struct _tmp1044;(_tmp1044.tag=0,((_tmp1044.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e1)),((
_tmp1045[0]=& _tmp1044,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmp1046="we have a cast in a lhs:  %s",_tag_dyneither(
_tmp1046,sizeof(char),29))),_tag_dyneither(_tmp1045,sizeof(void*),1)))))));}
_LL63E: if(*((int*)_tmpA09)!= 22)goto _LL640;_tmpA0A=((struct Cyc_Absyn_Deref_e_struct*)
_tmpA09)->f1;_LL63F: _tmpA0B=_tmpA0A;goto _LL641;_LL640: if(*((int*)_tmpA09)!= 25)
goto _LL642;_tmpA0B=((struct Cyc_Absyn_Subscript_e_struct*)_tmpA09)->f1;_LL641:
return Cyc_Tcutil_is_zero_ptr_type((void*)((struct Cyc_Core_Opt*)_check_null(
_tmpA0B->topt))->v,ptr_type,is_dyneither,elt_type);_LL642: if(*((int*)_tmpA09)!= 
24)goto _LL644;_tmpA0C=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmpA09)->f1;_LL643:
_tmpA0D=_tmpA0C;goto _LL645;_LL644: if(*((int*)_tmpA09)!= 23)goto _LL646;_tmpA0D=((
struct Cyc_Absyn_AggrMember_e_struct*)_tmpA09)->f1;_LL645: if(Cyc_Tcutil_is_zero_ptr_type((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpA0D->topt))->v,ptr_type,is_dyneither,
elt_type)){const char*_tmp104A;void*_tmp1049[1];struct Cyc_String_pa_struct
_tmp1048;(_tmp1048.tag=0,((_tmp1048.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_exp2string(e1)),((_tmp1049[0]=& _tmp1048,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp104A="found zero pointer aggregate member assignment: %s",
_tag_dyneither(_tmp104A,sizeof(char),51))),_tag_dyneither(_tmp1049,sizeof(void*),
1)))))));}return 0;_LL646: if(*((int*)_tmpA09)!= 14)goto _LL648;_tmpA0E=((struct Cyc_Absyn_Instantiate_e_struct*)
_tmpA09)->f1;_LL647: _tmpA0F=_tmpA0E;goto _LL649;_LL648: if(*((int*)_tmpA09)!= 13)
goto _LL64A;_tmpA0F=((struct Cyc_Absyn_NoInstantiate_e_struct*)_tmpA09)->f1;_LL649:
if(Cyc_Tcutil_is_zero_ptr_type((void*)((struct Cyc_Core_Opt*)_check_null(_tmpA0F->topt))->v,
ptr_type,is_dyneither,elt_type)){const char*_tmp104E;void*_tmp104D[1];struct Cyc_String_pa_struct
_tmp104C;(_tmp104C.tag=0,((_tmp104C.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_exp2string(e1)),((_tmp104D[0]=& _tmp104C,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp104E="found zero pointer instantiate/noinstantiate: %s",
_tag_dyneither(_tmp104E,sizeof(char),49))),_tag_dyneither(_tmp104D,sizeof(void*),
1)))))));}return 0;_LL64A: if(*((int*)_tmpA09)!= 1)goto _LL64C;_LL64B: return 0;
_LL64C:;_LL64D: {const char*_tmp1052;void*_tmp1051[1];struct Cyc_String_pa_struct
_tmp1050;(_tmp1050.tag=0,((_tmp1050.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_exp2string(e1)),((_tmp1051[0]=& _tmp1050,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1052="found bad lhs in is_zero_ptr_deref: %s",
_tag_dyneither(_tmp1052,sizeof(char),39))),_tag_dyneither(_tmp1051,sizeof(void*),
1)))))));}_LL63B:;}int Cyc_Tcutil_is_tagged_pointer_typ(void*t);int Cyc_Tcutil_is_tagged_pointer_typ(
void*t){void*ignore=(void*)0;return Cyc_Tcutil_is_tagged_pointer_typ_elt(t,&
ignore);}static int Cyc_Tcutil_is_noalias_region(void*r);static int Cyc_Tcutil_is_noalias_region(
void*r){void*_tmpA1C=Cyc_Tcutil_compress(r);struct Cyc_Absyn_Tvar*_tmpA1D;_LL64F:
if((int)_tmpA1C != 3)goto _LL651;_LL650: return 1;_LL651: if(_tmpA1C <= (void*)4)goto
_LL653;if(*((int*)_tmpA1C)!= 1)goto _LL653;_tmpA1D=((struct Cyc_Absyn_VarType_struct*)
_tmpA1C)->f1;_LL652: return Cyc_Tcutil_tvar_kind(_tmpA1D)== (void*)4  || Cyc_Tcutil_tvar_kind(
_tmpA1D)== (void*)5;_LL653:;_LL654: return 0;_LL64E:;}int Cyc_Tcutil_is_noalias_pointer(
void*t);int Cyc_Tcutil_is_noalias_pointer(void*t){void*_tmpA1E=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_PtrInfo _tmpA1F;struct Cyc_Absyn_PtrAtts _tmpA20;void*_tmpA21;
_LL656: if(_tmpA1E <= (void*)4)goto _LL658;if(*((int*)_tmpA1E)!= 4)goto _LL658;
_tmpA1F=((struct Cyc_Absyn_PointerType_struct*)_tmpA1E)->f1;_tmpA20=_tmpA1F.ptr_atts;
_tmpA21=(void*)_tmpA20.rgn;_LL657: return Cyc_Tcutil_is_noalias_region(_tmpA21);
_LL658:;_LL659: return 0;_LL655:;}int Cyc_Tcutil_is_noalias_pointer_or_aggr(struct
_RegionHandle*rgn,void*t);int Cyc_Tcutil_is_noalias_pointer_or_aggr(struct
_RegionHandle*rgn,void*t){void*_tmpA22=Cyc_Tcutil_compress(t);if(Cyc_Tcutil_is_noalias_pointer(
_tmpA22))return 1;{void*_tmpA23=_tmpA22;struct Cyc_List_List*_tmpA24;struct Cyc_Absyn_AggrInfo
_tmpA25;union Cyc_Absyn_AggrInfoU_union _tmpA26;struct Cyc_Absyn_Aggrdecl**_tmpA27;
struct Cyc_List_List*_tmpA28;struct Cyc_List_List*_tmpA29;struct Cyc_Absyn_AggrInfo
_tmpA2A;union Cyc_Absyn_AggrInfoU_union _tmpA2B;struct Cyc_Absyn_TunionInfo _tmpA2C;
union Cyc_Absyn_TunionInfoU_union _tmpA2D;struct Cyc_List_List*_tmpA2E;struct Cyc_Core_Opt*
_tmpA2F;struct Cyc_Absyn_TunionFieldInfo _tmpA30;union Cyc_Absyn_TunionFieldInfoU_union
_tmpA31;struct Cyc_List_List*_tmpA32;_LL65B: if(_tmpA23 <= (void*)4)goto _LL667;if(*((
int*)_tmpA23)!= 9)goto _LL65D;_tmpA24=((struct Cyc_Absyn_TupleType_struct*)_tmpA23)->f1;
_LL65C: while(_tmpA24 != 0){if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,(*((
struct _tuple5*)_tmpA24->hd)).f2))return 1;_tmpA24=_tmpA24->tl;}return 0;_LL65D: if(*((
int*)_tmpA23)!= 10)goto _LL65F;_tmpA25=((struct Cyc_Absyn_AggrType_struct*)_tmpA23)->f1;
_tmpA26=_tmpA25.aggr_info;if((((((struct Cyc_Absyn_AggrType_struct*)_tmpA23)->f1).aggr_info).KnownAggr).tag
!= 1)goto _LL65F;_tmpA27=(_tmpA26.KnownAggr).f1;_tmpA28=_tmpA25.targs;_LL65E: if((*
_tmpA27)->impl == 0)return 0;else{struct Cyc_List_List*_tmpA33=((struct Cyc_List_List*(*)(
struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(rgn,rgn,(*_tmpA27)->tvs,_tmpA28);struct Cyc_List_List*_tmpA34=((
struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmpA27)->impl))->fields;void*t;
while(_tmpA34 != 0){t=Cyc_Tcutil_rsubstitute(rgn,_tmpA33,(void*)((struct Cyc_Absyn_Aggrfield*)
_tmpA34->hd)->type);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,t))return 1;
_tmpA34=_tmpA34->tl;}return 0;}_LL65F: if(*((int*)_tmpA23)!= 11)goto _LL661;_tmpA29=((
struct Cyc_Absyn_AnonAggrType_struct*)_tmpA23)->f2;_LL660: while(_tmpA29 != 0){if(
Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,(void*)((struct Cyc_Absyn_Aggrfield*)
_tmpA29->hd)->type))return 1;_tmpA29=_tmpA29->tl;}return 0;_LL661: if(*((int*)
_tmpA23)!= 10)goto _LL663;_tmpA2A=((struct Cyc_Absyn_AggrType_struct*)_tmpA23)->f1;
_tmpA2B=_tmpA2A.aggr_info;if((((((struct Cyc_Absyn_AggrType_struct*)_tmpA23)->f1).aggr_info).UnknownAggr).tag
!= 0)goto _LL663;_LL662: {const char*_tmp1055;void*_tmp1054;(_tmp1054=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1055="got unknown aggr in is_noalias_aggr",
_tag_dyneither(_tmp1055,sizeof(char),36))),_tag_dyneither(_tmp1054,sizeof(void*),
0)));}_LL663: if(*((int*)_tmpA23)!= 2)goto _LL665;_tmpA2C=((struct Cyc_Absyn_TunionType_struct*)
_tmpA23)->f1;_tmpA2D=_tmpA2C.tunion_info;_tmpA2E=_tmpA2C.targs;_tmpA2F=_tmpA2C.rgn;
_LL664: {union Cyc_Absyn_TunionInfoU_union _tmpA37=_tmpA2D;struct Cyc_Absyn_UnknownTunionInfo
_tmpA38;struct _tuple2*_tmpA39;int _tmpA3A;int _tmpA3B;struct Cyc_Absyn_Tuniondecl**
_tmpA3C;struct Cyc_Absyn_Tuniondecl*_tmpA3D;struct Cyc_Absyn_Tuniondecl _tmpA3E;
struct Cyc_List_List*_tmpA3F;struct Cyc_Core_Opt*_tmpA40;int _tmpA41;_LL66A: if((
_tmpA37.UnknownTunion).tag != 0)goto _LL66C;_tmpA38=(_tmpA37.UnknownTunion).f1;
_tmpA39=_tmpA38.name;_tmpA3A=_tmpA38.is_xtunion;_tmpA3B=_tmpA38.is_flat;_LL66B: {
const char*_tmp1058;void*_tmp1057;(_tmp1057=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1058="got unknown tunion in is_noalias_aggr",
_tag_dyneither(_tmp1058,sizeof(char),38))),_tag_dyneither(_tmp1057,sizeof(void*),
0)));}_LL66C: if((_tmpA37.KnownTunion).tag != 1)goto _LL669;_tmpA3C=(_tmpA37.KnownTunion).f1;
_tmpA3D=*_tmpA3C;_tmpA3E=*_tmpA3D;_tmpA3F=_tmpA3E.tvs;_tmpA40=_tmpA3E.fields;
_tmpA41=_tmpA3E.is_flat;_LL66D: if(!_tmpA41){if(_tmpA2F == 0)return 0;return Cyc_Tcutil_is_noalias_region((
void*)_tmpA2F->v);}else{if(_tmpA40 == 0){const char*_tmp105B;void*_tmp105A;(
_tmp105A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp105B="got tunion with no fields",_tag_dyneither(_tmp105B,sizeof(char),26))),
_tag_dyneither(_tmp105A,sizeof(void*),0)));}else{struct Cyc_List_List*_tmpA46=((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmpA3F,_tmpA2E);struct Cyc_List_List*
_tmpA47=(struct Cyc_List_List*)_tmpA40->v;while(_tmpA47 != 0){struct Cyc_List_List*
_tmpA48=((struct Cyc_Absyn_Tunionfield*)_tmpA47->hd)->typs;while(_tmpA48 != 0){
_tmpA22=Cyc_Tcutil_rsubstitute(rgn,_tmpA46,(*((struct _tuple5*)_tmpA48->hd)).f2);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,_tmpA22))return 1;_tmpA48=_tmpA48->tl;}
_tmpA47=_tmpA47->tl;}return 0;}}_LL669:;}_LL665: if(*((int*)_tmpA23)!= 3)goto
_LL667;_tmpA30=((struct Cyc_Absyn_TunionFieldType_struct*)_tmpA23)->f1;_tmpA31=
_tmpA30.field_info;_tmpA32=_tmpA30.targs;_LL666: {union Cyc_Absyn_TunionFieldInfoU_union
_tmpA49=_tmpA31;struct Cyc_Absyn_Tuniondecl*_tmpA4A;struct Cyc_Absyn_Tunionfield*
_tmpA4B;_LL66F: if((_tmpA49.UnknownTunionfield).tag != 0)goto _LL671;_LL670: {const
char*_tmp105E;void*_tmp105D;(_tmp105D=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp105E="got unknown tunion field in is_noalias_aggr",
_tag_dyneither(_tmp105E,sizeof(char),44))),_tag_dyneither(_tmp105D,sizeof(void*),
0)));}_LL671: if((_tmpA49.KnownTunionfield).tag != 1)goto _LL66E;_tmpA4A=(_tmpA49.KnownTunionfield).f1;
_tmpA4B=(_tmpA49.KnownTunionfield).f2;_LL672: {struct Cyc_List_List*_tmpA4E=((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmpA4A->tvs,_tmpA32);struct Cyc_List_List*
_tmpA4F=_tmpA4B->typs;while(_tmpA4F != 0){_tmpA22=Cyc_Tcutil_rsubstitute(rgn,
_tmpA4E,(*((struct _tuple5*)_tmpA4F->hd)).f2);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(
rgn,_tmpA22))return 1;_tmpA4F=_tmpA4F->tl;}return 0;}_LL66E:;}_LL667:;_LL668:
return 0;_LL65A:;}}static int Cyc_Tcutil_is_noalias_field(struct _RegionHandle*r,
void*t,struct _dyneither_ptr*f);static int Cyc_Tcutil_is_noalias_field(struct
_RegionHandle*r,void*t,struct _dyneither_ptr*f){void*_tmpA50=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_AggrInfo _tmpA51;union Cyc_Absyn_AggrInfoU_union _tmpA52;struct
Cyc_Absyn_Aggrdecl**_tmpA53;struct Cyc_Absyn_Aggrdecl*_tmpA54;struct Cyc_List_List*
_tmpA55;struct Cyc_List_List*_tmpA56;_LL674: if(_tmpA50 <= (void*)4)goto _LL678;if(*((
int*)_tmpA50)!= 10)goto _LL676;_tmpA51=((struct Cyc_Absyn_AggrType_struct*)_tmpA50)->f1;
_tmpA52=_tmpA51.aggr_info;if((((((struct Cyc_Absyn_AggrType_struct*)_tmpA50)->f1).aggr_info).KnownAggr).tag
!= 1)goto _LL676;_tmpA53=(_tmpA52.KnownAggr).f1;_tmpA54=*_tmpA53;_tmpA55=_tmpA51.targs;
_LL675: _tmpA56=_tmpA54->impl == 0?0:((struct Cyc_Absyn_AggrdeclImpl*)_check_null(
_tmpA54->impl))->fields;goto _LL677;_LL676: if(*((int*)_tmpA50)!= 11)goto _LL678;
_tmpA56=((struct Cyc_Absyn_AnonAggrType_struct*)_tmpA50)->f2;_LL677: {struct Cyc_Absyn_Aggrfield*
_tmpA57=Cyc_Absyn_lookup_field(_tmpA56,f);{struct Cyc_Absyn_Aggrfield*_tmpA58=
_tmpA57;struct Cyc_Absyn_Aggrfield _tmpA59;void*_tmpA5A;_LL67B: if(_tmpA58 != 0)goto
_LL67D;_LL67C: {const char*_tmp1061;void*_tmp1060;(_tmp1060=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1061="is_noalias_field: missing field",
_tag_dyneither(_tmp1061,sizeof(char),32))),_tag_dyneither(_tmp1060,sizeof(void*),
0)));}_LL67D: if(_tmpA58 == 0)goto _LL67A;_tmpA59=*_tmpA58;_tmpA5A=(void*)_tmpA59.type;
_LL67E: return Cyc_Tcutil_is_noalias_pointer_or_aggr(r,_tmpA5A);_LL67A:;}return 0;}
_LL678:;_LL679: {const char*_tmp1065;void*_tmp1064[1];struct Cyc_String_pa_struct
_tmp1063;(_tmp1063.tag=0,((_tmp1063.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp1064[0]=& _tmp1063,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1065="is_noalias_field: invalid type |%s|",
_tag_dyneither(_tmp1065,sizeof(char),36))),_tag_dyneither(_tmp1064,sizeof(void*),
1)))))));}_LL673:;}static int Cyc_Tcutil_is_noalias_path_aux(struct _RegionHandle*r,
struct Cyc_Absyn_Exp*e,int ignore_leaf);static int Cyc_Tcutil_is_noalias_path_aux(
struct _RegionHandle*r,struct Cyc_Absyn_Exp*e,int ignore_leaf){void*_tmpA60=(void*)
e->r;void*_tmpA61;struct Cyc_Absyn_Exp*_tmpA62;struct _dyneither_ptr*_tmpA63;
struct Cyc_Absyn_Exp*_tmpA64;struct Cyc_Absyn_Exp*_tmpA65;void*_tmpA66;void*
_tmpA67;void*_tmpA68;struct Cyc_Absyn_Exp*_tmpA69;struct Cyc_Absyn_Exp*_tmpA6A;
struct Cyc_Absyn_Exp*_tmpA6B;struct Cyc_Absyn_Exp*_tmpA6C;void*_tmpA6D;struct Cyc_Absyn_Exp*
_tmpA6E;struct Cyc_Absyn_Stmt*_tmpA6F;_LL680: if(*((int*)_tmpA60)!= 5)goto _LL682;
_LL681: goto _LL683;_LL682: if(*((int*)_tmpA60)!= 7)goto _LL684;_LL683: goto _LL685;
_LL684: if(*((int*)_tmpA60)!= 8)goto _LL686;_LL685: goto _LL687;_LL686: if(*((int*)
_tmpA60)!= 12)goto _LL688;_LL687: goto _LL689;_LL688: if(*((int*)_tmpA60)!= 16)goto
_LL68A;_LL689: goto _LL68B;_LL68A: if(*((int*)_tmpA60)!= 18)goto _LL68C;_LL68B: goto
_LL68D;_LL68C: if(*((int*)_tmpA60)!= 19)goto _LL68E;_LL68D: goto _LL68F;_LL68E: if(*((
int*)_tmpA60)!= 20)goto _LL690;_LL68F: goto _LL691;_LL690: if(*((int*)_tmpA60)!= 21)
goto _LL692;_LL691: goto _LL693;_LL692: if(*((int*)_tmpA60)!= 27)goto _LL694;_LL693:
goto _LL695;_LL694: if(*((int*)_tmpA60)!= 29)goto _LL696;_LL695: goto _LL697;_LL696:
if(*((int*)_tmpA60)!= 28)goto _LL698;_LL697: goto _LL699;_LL698: if(*((int*)_tmpA60)
!= 33)goto _LL69A;_LL699: goto _LL69B;_LL69A: if(*((int*)_tmpA60)!= 34)goto _LL69C;
_LL69B: goto _LL69D;_LL69C: if(*((int*)_tmpA60)!= 36)goto _LL69E;_LL69D: goto _LL69F;
_LL69E: if(*((int*)_tmpA60)!= 1)goto _LL6A0;_tmpA61=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmpA60)->f2;if(_tmpA61 <= (void*)1)goto _LL6A0;if(*((int*)_tmpA61)!= 0)goto _LL6A0;
_LL69F: goto _LL6A1;_LL6A0: if(*((int*)_tmpA60)!= 3)goto _LL6A2;_LL6A1: return 0;
_LL6A2: if(*((int*)_tmpA60)!= 22)goto _LL6A4;_LL6A3: goto _LL6A5;_LL6A4: if(*((int*)
_tmpA60)!= 24)goto _LL6A6;_LL6A5: return 0;_LL6A6: if(*((int*)_tmpA60)!= 23)goto
_LL6A8;_tmpA62=((struct Cyc_Absyn_AggrMember_e_struct*)_tmpA60)->f1;_tmpA63=((
struct Cyc_Absyn_AggrMember_e_struct*)_tmpA60)->f2;_LL6A7: return(ignore_leaf  || 
Cyc_Tcutil_is_noalias_field(r,(void*)((struct Cyc_Core_Opt*)_check_null(_tmpA62->topt))->v,
_tmpA63)) && Cyc_Tcutil_is_noalias_path_aux(r,_tmpA62,0);_LL6A8: if(*((int*)
_tmpA60)!= 25)goto _LL6AA;_tmpA64=((struct Cyc_Absyn_Subscript_e_struct*)_tmpA60)->f1;
_tmpA65=((struct Cyc_Absyn_Subscript_e_struct*)_tmpA60)->f2;_LL6A9: {void*_tmpA70=
Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmpA64->topt))->v);
struct Cyc_Absyn_PtrInfo _tmpA71;void*_tmpA72;struct Cyc_List_List*_tmpA73;_LL6CD:
if(_tmpA70 <= (void*)4)goto _LL6D1;if(*((int*)_tmpA70)!= 4)goto _LL6CF;_tmpA71=((
struct Cyc_Absyn_PointerType_struct*)_tmpA70)->f1;_tmpA72=(void*)_tmpA71.elt_typ;
_LL6CE: return 0;_LL6CF: if(*((int*)_tmpA70)!= 9)goto _LL6D1;_tmpA73=((struct Cyc_Absyn_TupleType_struct*)
_tmpA70)->f1;_LL6D0: {unsigned int _tmpA75;int _tmpA76;struct _tuple7 _tmpA74=Cyc_Evexp_eval_const_uint_exp(
_tmpA65);_tmpA75=_tmpA74.f1;_tmpA76=_tmpA74.f2;if(!_tmpA76){const char*_tmp1068;
void*_tmp1067;(_tmp1067=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap))Cyc_Tcutil_impos)(((_tmp1068="is_noalias_path_aux: non-constant subscript",
_tag_dyneither(_tmp1068,sizeof(char),44))),_tag_dyneither(_tmp1067,sizeof(void*),
0)));}{struct _handler_cons _tmpA79;_push_handler(& _tmpA79);{int _tmpA7B=0;if(
setjmp(_tmpA79.handler))_tmpA7B=1;if(!_tmpA7B){{void*_tmpA7C=(*((struct _tuple5*(*)(
struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmpA73,(int)_tmpA75)).f2;int _tmpA7D=(
ignore_leaf  || Cyc_Tcutil_is_noalias_pointer_or_aggr(r,_tmpA7C)) && Cyc_Tcutil_is_noalias_path_aux(
r,_tmpA64,0);_npop_handler(0);return _tmpA7D;};_pop_handler();}else{void*_tmpA7A=(
void*)_exn_thrown;void*_tmpA7F=_tmpA7A;_LL6D4: if(_tmpA7F != Cyc_List_Nth)goto
_LL6D6;_LL6D5: {const char*_tmp106B;void*_tmp106A;return(_tmp106A=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp106B="is_noalias_path_aux: out-of-bounds subscript",
_tag_dyneither(_tmp106B,sizeof(char),45))),_tag_dyneither(_tmp106A,sizeof(void*),
0)));}_LL6D6:;_LL6D7:(void)_throw(_tmpA7F);_LL6D3:;}}}}_LL6D1:;_LL6D2: {const
char*_tmp106E;void*_tmp106D;(_tmp106D=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp106E="is_noalias_path_aux: subscript on non-pointer/tuple",
_tag_dyneither(_tmp106E,sizeof(char),52))),_tag_dyneither(_tmp106D,sizeof(void*),
0)));}_LL6CC:;}_LL6AA: if(*((int*)_tmpA60)!= 32)goto _LL6AC;_LL6AB: goto _LL6AD;
_LL6AC: if(*((int*)_tmpA60)!= 26)goto _LL6AE;_LL6AD: goto _LL6AF;_LL6AE: if(*((int*)
_tmpA60)!= 30)goto _LL6B0;_LL6AF: goto _LL6B1;_LL6B0: if(*((int*)_tmpA60)!= 31)goto
_LL6B2;_LL6B1: goto _LL6B3;_LL6B2: if(*((int*)_tmpA60)!= 0)goto _LL6B4;_LL6B3: goto
_LL6B5;_LL6B4: if(*((int*)_tmpA60)!= 35)goto _LL6B6;_LL6B5: goto _LL6B7;_LL6B6: if(*((
int*)_tmpA60)!= 17)goto _LL6B8;_LL6B7: goto _LL6B9;_LL6B8: if(*((int*)_tmpA60)!= 1)
goto _LL6BA;_tmpA66=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmpA60)->f2;if(
_tmpA66 <= (void*)1)goto _LL6BA;if(*((int*)_tmpA66)!= 3)goto _LL6BA;_LL6B9: goto
_LL6BB;_LL6BA: if(*((int*)_tmpA60)!= 1)goto _LL6BC;_tmpA67=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmpA60)->f2;if(_tmpA67 <= (void*)1)goto _LL6BC;if(*((int*)_tmpA67)!= 4)goto _LL6BC;
_LL6BB: goto _LL6BD;_LL6BC: if(*((int*)_tmpA60)!= 1)goto _LL6BE;_tmpA68=(void*)((
struct Cyc_Absyn_Var_e_struct*)_tmpA60)->f2;if(_tmpA68 <= (void*)1)goto _LL6BE;if(*((
int*)_tmpA68)!= 2)goto _LL6BE;_LL6BD: {int _tmpA84=ignore_leaf  || Cyc_Tcutil_is_noalias_pointer_or_aggr(
r,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);return _tmpA84;}_LL6BE:
if(*((int*)_tmpA60)!= 6)goto _LL6C0;_tmpA69=((struct Cyc_Absyn_Conditional_e_struct*)
_tmpA60)->f2;_LL6BF: _tmpA6A=_tmpA69;goto _LL6C1;_LL6C0: if(*((int*)_tmpA60)!= 9)
goto _LL6C2;_tmpA6A=((struct Cyc_Absyn_SeqExp_e_struct*)_tmpA60)->f2;_LL6C1:
_tmpA6B=_tmpA6A;goto _LL6C3;_LL6C2: if(*((int*)_tmpA60)!= 4)goto _LL6C4;_tmpA6B=((
struct Cyc_Absyn_AssignOp_e_struct*)_tmpA60)->f1;_LL6C3: return Cyc_Tcutil_is_noalias_path_aux(
r,_tmpA6B,ignore_leaf);_LL6C4: if(*((int*)_tmpA60)!= 11)goto _LL6C6;_tmpA6C=((
struct Cyc_Absyn_FnCall_e_struct*)_tmpA60)->f1;_LL6C5: {void*_tmpA85=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpA6C->topt))->v);struct Cyc_Absyn_FnInfo
_tmpA86;void*_tmpA87;struct Cyc_Absyn_PtrInfo _tmpA88;void*_tmpA89;_LL6D9: if(
_tmpA85 <= (void*)4)goto _LL6DD;if(*((int*)_tmpA85)!= 8)goto _LL6DB;_tmpA86=((
struct Cyc_Absyn_FnType_struct*)_tmpA85)->f1;_tmpA87=(void*)_tmpA86.ret_typ;
_LL6DA: return ignore_leaf  || Cyc_Tcutil_is_noalias_pointer_or_aggr(r,_tmpA87);
_LL6DB: if(*((int*)_tmpA85)!= 4)goto _LL6DD;_tmpA88=((struct Cyc_Absyn_PointerType_struct*)
_tmpA85)->f1;_tmpA89=(void*)_tmpA88.elt_typ;_LL6DC:{void*_tmpA8A=Cyc_Tcutil_compress(
_tmpA89);struct Cyc_Absyn_FnInfo _tmpA8B;void*_tmpA8C;_LL6E0: if(_tmpA8A <= (void*)4)
goto _LL6E2;if(*((int*)_tmpA8A)!= 8)goto _LL6E2;_tmpA8B=((struct Cyc_Absyn_FnType_struct*)
_tmpA8A)->f1;_tmpA8C=(void*)_tmpA8B.ret_typ;_LL6E1: return ignore_leaf  || Cyc_Tcutil_is_noalias_pointer_or_aggr(
r,_tmpA8C);_LL6E2:;_LL6E3: goto _LL6DF;_LL6DF:;}goto _LL6DE;_LL6DD:;_LL6DE: {const
char*_tmp1072;void*_tmp1071[1];struct Cyc_String_pa_struct _tmp1070;(_tmp1070.tag=
0,((_tmp1070.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpA6C->topt))->v)),((_tmp1071[0]=&
_tmp1070,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp1072="Fncall has non function type %s",_tag_dyneither(_tmp1072,sizeof(char),
32))),_tag_dyneither(_tmp1071,sizeof(void*),1)))))));}_LL6D8:;}_LL6C6: if(*((int*)
_tmpA60)!= 15)goto _LL6C8;_tmpA6D=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmpA60)->f1;
_tmpA6E=((struct Cyc_Absyn_Cast_e_struct*)_tmpA60)->f2;_LL6C7: return Cyc_Tcutil_is_noalias_pointer_or_aggr(
r,_tmpA6D) && Cyc_Tcutil_is_noalias_path_aux(r,_tmpA6E,1);_LL6C8: if(*((int*)
_tmpA60)!= 38)goto _LL6CA;_tmpA6F=((struct Cyc_Absyn_StmtExp_e_struct*)_tmpA60)->f1;
_LL6C9: while(1){void*_tmpA90=(void*)_tmpA6F->r;struct Cyc_Absyn_Stmt*_tmpA91;
struct Cyc_Absyn_Stmt*_tmpA92;struct Cyc_Absyn_Decl*_tmpA93;struct Cyc_Absyn_Stmt*
_tmpA94;struct Cyc_Absyn_Exp*_tmpA95;_LL6E5: if(_tmpA90 <= (void*)1)goto _LL6EB;if(*((
int*)_tmpA90)!= 1)goto _LL6E7;_tmpA91=((struct Cyc_Absyn_Seq_s_struct*)_tmpA90)->f1;
_tmpA92=((struct Cyc_Absyn_Seq_s_struct*)_tmpA90)->f2;_LL6E6: _tmpA6F=_tmpA92;goto
_LL6E4;_LL6E7: if(*((int*)_tmpA90)!= 11)goto _LL6E9;_tmpA93=((struct Cyc_Absyn_Decl_s_struct*)
_tmpA90)->f1;_tmpA94=((struct Cyc_Absyn_Decl_s_struct*)_tmpA90)->f2;_LL6E8:
_tmpA6F=_tmpA94;goto _LL6E4;_LL6E9: if(*((int*)_tmpA90)!= 0)goto _LL6EB;_tmpA95=((
struct Cyc_Absyn_Exp_s_struct*)_tmpA90)->f1;_LL6EA: return Cyc_Tcutil_is_noalias_path_aux(
r,_tmpA95,ignore_leaf);_LL6EB:;_LL6EC: {const char*_tmp1075;void*_tmp1074;(
_tmp1074=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp1075="is_noalias_stmt_exp: ill-formed StmtExp",_tag_dyneither(_tmp1075,
sizeof(char),40))),_tag_dyneither(_tmp1074,sizeof(void*),0)));}_LL6E4:;}_LL6CA:;
_LL6CB: return 0;_LL67F:;}int Cyc_Tcutil_is_noalias_path(struct _RegionHandle*r,
struct Cyc_Absyn_Exp*e);int Cyc_Tcutil_is_noalias_path(struct _RegionHandle*r,
struct Cyc_Absyn_Exp*e){return Cyc_Tcutil_is_noalias_path_aux(r,e,0);}struct
_tuple10 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e);
struct _tuple10 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*
e){struct _tuple10 _tmp1076;struct _tuple10 bogus_ans=(_tmp1076.f1=0,((_tmp1076.f2=(
void*)2,_tmp1076)));void*_tmpA98=(void*)e->r;struct _tuple2*_tmpA99;void*_tmpA9A;
struct Cyc_Absyn_Exp*_tmpA9B;struct _dyneither_ptr*_tmpA9C;struct Cyc_Absyn_Exp*
_tmpA9D;struct _dyneither_ptr*_tmpA9E;struct Cyc_Absyn_Exp*_tmpA9F;struct Cyc_Absyn_Exp*
_tmpAA0;struct Cyc_Absyn_Exp*_tmpAA1;_LL6EE: if(*((int*)_tmpA98)!= 1)goto _LL6F0;
_tmpA99=((struct Cyc_Absyn_Var_e_struct*)_tmpA98)->f1;_tmpA9A=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmpA98)->f2;_LL6EF: {void*_tmpAA2=_tmpA9A;struct Cyc_Absyn_Vardecl*_tmpAA3;
struct Cyc_Absyn_Vardecl*_tmpAA4;struct Cyc_Absyn_Vardecl*_tmpAA5;struct Cyc_Absyn_Vardecl*
_tmpAA6;_LL6FB: if((int)_tmpAA2 != 0)goto _LL6FD;_LL6FC: goto _LL6FE;_LL6FD: if(
_tmpAA2 <= (void*)1)goto _LL6FF;if(*((int*)_tmpAA2)!= 1)goto _LL6FF;_LL6FE: return
bogus_ans;_LL6FF: if(_tmpAA2 <= (void*)1)goto _LL701;if(*((int*)_tmpAA2)!= 0)goto
_LL701;_tmpAA3=((struct Cyc_Absyn_Global_b_struct*)_tmpAA2)->f1;_LL700: {void*
_tmpAA7=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
_LL708: if(_tmpAA7 <= (void*)4)goto _LL70A;if(*((int*)_tmpAA7)!= 7)goto _LL70A;
_LL709: {struct _tuple10 _tmp1077;return(_tmp1077.f1=1,((_tmp1077.f2=(void*)2,
_tmp1077)));}_LL70A:;_LL70B: {struct _tuple10 _tmp1078;return(_tmp1078.f1=(_tmpAA3->tq).real_const,((
_tmp1078.f2=(void*)2,_tmp1078)));}_LL707:;}_LL701: if(_tmpAA2 <= (void*)1)goto
_LL703;if(*((int*)_tmpAA2)!= 3)goto _LL703;_tmpAA4=((struct Cyc_Absyn_Local_b_struct*)
_tmpAA2)->f1;_LL702: {void*_tmpAAA=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);_LL70D: if(_tmpAAA <= (void*)4)goto _LL70F;if(*((int*)
_tmpAAA)!= 7)goto _LL70F;_LL70E: {struct _tuple10 _tmp1079;return(_tmp1079.f1=1,((
_tmp1079.f2=(void*)((struct Cyc_Core_Opt*)_check_null(_tmpAA4->rgn))->v,_tmp1079)));}
_LL70F:;_LL710: _tmpAA4->escapes=1;{struct _tuple10 _tmp107A;return(_tmp107A.f1=(
_tmpAA4->tq).real_const,((_tmp107A.f2=(void*)((struct Cyc_Core_Opt*)_check_null(
_tmpAA4->rgn))->v,_tmp107A)));}_LL70C:;}_LL703: if(_tmpAA2 <= (void*)1)goto _LL705;
if(*((int*)_tmpAA2)!= 4)goto _LL705;_tmpAA5=((struct Cyc_Absyn_Pat_b_struct*)
_tmpAA2)->f1;_LL704: _tmpAA6=_tmpAA5;goto _LL706;_LL705: if(_tmpAA2 <= (void*)1)goto
_LL6FA;if(*((int*)_tmpAA2)!= 2)goto _LL6FA;_tmpAA6=((struct Cyc_Absyn_Param_b_struct*)
_tmpAA2)->f1;_LL706: _tmpAA6->escapes=1;{struct _tuple10 _tmp107B;return(_tmp107B.f1=(
_tmpAA6->tq).real_const,((_tmp107B.f2=(void*)((struct Cyc_Core_Opt*)_check_null(
_tmpAA6->rgn))->v,_tmp107B)));}_LL6FA:;}_LL6F0: if(*((int*)_tmpA98)!= 23)goto
_LL6F2;_tmpA9B=((struct Cyc_Absyn_AggrMember_e_struct*)_tmpA98)->f1;_tmpA9C=((
struct Cyc_Absyn_AggrMember_e_struct*)_tmpA98)->f2;_LL6F1: {void*_tmpAAE=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpA9B->topt))->v);struct Cyc_List_List*
_tmpAAF;struct Cyc_Absyn_AggrInfo _tmpAB0;union Cyc_Absyn_AggrInfoU_union _tmpAB1;
struct Cyc_Absyn_Aggrdecl**_tmpAB2;struct Cyc_Absyn_Aggrdecl*_tmpAB3;_LL712: if(
_tmpAAE <= (void*)4)goto _LL716;if(*((int*)_tmpAAE)!= 11)goto _LL714;_tmpAAF=((
struct Cyc_Absyn_AnonAggrType_struct*)_tmpAAE)->f2;_LL713: {struct Cyc_Absyn_Aggrfield*
_tmpAB4=Cyc_Absyn_lookup_field(_tmpAAF,_tmpA9C);if(_tmpAB4 != 0  && _tmpAB4->width
!= 0){struct _tuple10 _tmp107C;return(_tmp107C.f1=(_tmpAB4->tq).real_const,((
_tmp107C.f2=(Cyc_Tcutil_addressof_props(te,_tmpA9B)).f2,_tmp107C)));}return
bogus_ans;}_LL714: if(*((int*)_tmpAAE)!= 10)goto _LL716;_tmpAB0=((struct Cyc_Absyn_AggrType_struct*)
_tmpAAE)->f1;_tmpAB1=_tmpAB0.aggr_info;if((((((struct Cyc_Absyn_AggrType_struct*)
_tmpAAE)->f1).aggr_info).KnownAggr).tag != 1)goto _LL716;_tmpAB2=(_tmpAB1.KnownAggr).f1;
_tmpAB3=*_tmpAB2;_LL715: {struct Cyc_Absyn_Aggrfield*_tmpAB6=Cyc_Absyn_lookup_decl_field(
_tmpAB3,_tmpA9C);if(_tmpAB6 != 0  && _tmpAB6->width != 0){struct _tuple10 _tmp107D;
return(_tmp107D.f1=(_tmpAB6->tq).real_const,((_tmp107D.f2=(Cyc_Tcutil_addressof_props(
te,_tmpA9B)).f2,_tmp107D)));}return bogus_ans;}_LL716:;_LL717: return bogus_ans;
_LL711:;}_LL6F2: if(*((int*)_tmpA98)!= 24)goto _LL6F4;_tmpA9D=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmpA98)->f1;_tmpA9E=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmpA98)->f2;_LL6F3: {
void*_tmpAB8=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmpA9D->topt))->v);struct Cyc_Absyn_PtrInfo _tmpAB9;void*_tmpABA;struct Cyc_Absyn_PtrAtts
_tmpABB;void*_tmpABC;_LL719: if(_tmpAB8 <= (void*)4)goto _LL71B;if(*((int*)_tmpAB8)
!= 4)goto _LL71B;_tmpAB9=((struct Cyc_Absyn_PointerType_struct*)_tmpAB8)->f1;
_tmpABA=(void*)_tmpAB9.elt_typ;_tmpABB=_tmpAB9.ptr_atts;_tmpABC=(void*)_tmpABB.rgn;
_LL71A: {struct Cyc_Absyn_Aggrfield*finfo;{void*_tmpABD=Cyc_Tcutil_compress(
_tmpABA);struct Cyc_List_List*_tmpABE;struct Cyc_Absyn_AggrInfo _tmpABF;union Cyc_Absyn_AggrInfoU_union
_tmpAC0;struct Cyc_Absyn_Aggrdecl**_tmpAC1;struct Cyc_Absyn_Aggrdecl*_tmpAC2;
_LL71E: if(_tmpABD <= (void*)4)goto _LL722;if(*((int*)_tmpABD)!= 11)goto _LL720;
_tmpABE=((struct Cyc_Absyn_AnonAggrType_struct*)_tmpABD)->f2;_LL71F: finfo=Cyc_Absyn_lookup_field(
_tmpABE,_tmpA9E);goto _LL71D;_LL720: if(*((int*)_tmpABD)!= 10)goto _LL722;_tmpABF=((
struct Cyc_Absyn_AggrType_struct*)_tmpABD)->f1;_tmpAC0=_tmpABF.aggr_info;if((((((
struct Cyc_Absyn_AggrType_struct*)_tmpABD)->f1).aggr_info).KnownAggr).tag != 1)
goto _LL722;_tmpAC1=(_tmpAC0.KnownAggr).f1;_tmpAC2=*_tmpAC1;_LL721: finfo=Cyc_Absyn_lookup_decl_field(
_tmpAC2,_tmpA9E);goto _LL71D;_LL722:;_LL723: return bogus_ans;_LL71D:;}if(finfo != 0
 && finfo->width != 0){struct _tuple10 _tmp107E;return(_tmp107E.f1=(finfo->tq).real_const,((
_tmp107E.f2=_tmpABC,_tmp107E)));}return bogus_ans;}_LL71B:;_LL71C: return bogus_ans;
_LL718:;}_LL6F4: if(*((int*)_tmpA98)!= 22)goto _LL6F6;_tmpA9F=((struct Cyc_Absyn_Deref_e_struct*)
_tmpA98)->f1;_LL6F5: {void*_tmpAC4=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmpA9F->topt))->v);struct Cyc_Absyn_PtrInfo _tmpAC5;struct Cyc_Absyn_Tqual
_tmpAC6;struct Cyc_Absyn_PtrAtts _tmpAC7;void*_tmpAC8;_LL725: if(_tmpAC4 <= (void*)4)
goto _LL727;if(*((int*)_tmpAC4)!= 4)goto _LL727;_tmpAC5=((struct Cyc_Absyn_PointerType_struct*)
_tmpAC4)->f1;_tmpAC6=_tmpAC5.elt_tq;_tmpAC7=_tmpAC5.ptr_atts;_tmpAC8=(void*)
_tmpAC7.rgn;_LL726: {struct _tuple10 _tmp107F;return(_tmp107F.f1=_tmpAC6.real_const,((
_tmp107F.f2=_tmpAC8,_tmp107F)));}_LL727:;_LL728: return bogus_ans;_LL724:;}_LL6F6:
if(*((int*)_tmpA98)!= 25)goto _LL6F8;_tmpAA0=((struct Cyc_Absyn_Subscript_e_struct*)
_tmpA98)->f1;_tmpAA1=((struct Cyc_Absyn_Subscript_e_struct*)_tmpA98)->f2;_LL6F7: {
void*t=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmpAA0->topt))->v);
void*_tmpACA=t;struct Cyc_List_List*_tmpACB;struct Cyc_Absyn_PtrInfo _tmpACC;struct
Cyc_Absyn_Tqual _tmpACD;struct Cyc_Absyn_PtrAtts _tmpACE;void*_tmpACF;struct Cyc_Absyn_ArrayInfo
_tmpAD0;struct Cyc_Absyn_Tqual _tmpAD1;_LL72A: if(_tmpACA <= (void*)4)goto _LL730;if(*((
int*)_tmpACA)!= 9)goto _LL72C;_tmpACB=((struct Cyc_Absyn_TupleType_struct*)_tmpACA)->f1;
_LL72B: {unsigned int _tmpAD3;int _tmpAD4;struct _tuple7 _tmpAD2=Cyc_Evexp_eval_const_uint_exp(
_tmpAA1);_tmpAD3=_tmpAD2.f1;_tmpAD4=_tmpAD2.f2;if(!_tmpAD4)return bogus_ans;{
struct _tuple5*_tmpAD5=Cyc_Absyn_lookup_tuple_field(_tmpACB,(int)_tmpAD3);if(
_tmpAD5 != 0){struct _tuple10 _tmp1080;return(_tmp1080.f1=((*_tmpAD5).f1).real_const,((
_tmp1080.f2=(Cyc_Tcutil_addressof_props(te,_tmpAA0)).f2,_tmp1080)));}return
bogus_ans;}}_LL72C: if(*((int*)_tmpACA)!= 4)goto _LL72E;_tmpACC=((struct Cyc_Absyn_PointerType_struct*)
_tmpACA)->f1;_tmpACD=_tmpACC.elt_tq;_tmpACE=_tmpACC.ptr_atts;_tmpACF=(void*)
_tmpACE.rgn;_LL72D: {struct _tuple10 _tmp1081;return(_tmp1081.f1=_tmpACD.real_const,((
_tmp1081.f2=_tmpACF,_tmp1081)));}_LL72E: if(*((int*)_tmpACA)!= 7)goto _LL730;
_tmpAD0=((struct Cyc_Absyn_ArrayType_struct*)_tmpACA)->f1;_tmpAD1=_tmpAD0.tq;
_LL72F: {struct _tuple10 _tmp1082;return(_tmp1082.f1=_tmpAD1.real_const,((_tmp1082.f2=(
Cyc_Tcutil_addressof_props(te,_tmpAA0)).f2,_tmp1082)));}_LL730:;_LL731: return
bogus_ans;_LL729:;}_LL6F8:;_LL6F9:{const char*_tmp1085;void*_tmp1084;(_tmp1084=0,
Cyc_Tcutil_terr(e->loc,((_tmp1085="unary & applied to non-lvalue",_tag_dyneither(
_tmp1085,sizeof(char),30))),_tag_dyneither(_tmp1084,sizeof(void*),0)));}return
bogus_ans;_LL6ED:;}void*Cyc_Tcutil_array_to_ptr(struct Cyc_Tcenv_Tenv*te,void*
e_typ,struct Cyc_Absyn_Exp*e);void*Cyc_Tcutil_array_to_ptr(struct Cyc_Tcenv_Tenv*
te,void*e_typ,struct Cyc_Absyn_Exp*e){void*_tmpADC=Cyc_Tcutil_compress(e_typ);
struct Cyc_Absyn_ArrayInfo _tmpADD;void*_tmpADE;struct Cyc_Absyn_Tqual _tmpADF;
struct Cyc_Absyn_Conref*_tmpAE0;_LL733: if(_tmpADC <= (void*)4)goto _LL735;if(*((int*)
_tmpADC)!= 7)goto _LL735;_tmpADD=((struct Cyc_Absyn_ArrayType_struct*)_tmpADC)->f1;
_tmpADE=(void*)_tmpADD.elt_type;_tmpADF=_tmpADD.tq;_tmpAE0=_tmpADD.zero_term;
_LL734: {void*_tmpAE2;struct _tuple10 _tmpAE1=Cyc_Tcutil_addressof_props(te,e);
_tmpAE2=_tmpAE1.f2;{struct Cyc_Absyn_Upper_b_struct _tmp1088;struct Cyc_Absyn_Upper_b_struct*
_tmp1087;return Cyc_Absyn_atb_typ(_tmpADE,_tmpAE2,_tmpADF,(void*)((_tmp1087=
_cycalloc(sizeof(*_tmp1087)),((_tmp1087[0]=((_tmp1088.tag=0,((_tmp1088.f1=e,
_tmp1088)))),_tmp1087)))),_tmpAE0);}}_LL735:;_LL736: return e_typ;_LL732:;}void Cyc_Tcutil_check_bound(
struct Cyc_Position_Segment*loc,unsigned int i,struct Cyc_Absyn_Conref*b);void Cyc_Tcutil_check_bound(
struct Cyc_Position_Segment*loc,unsigned int i,struct Cyc_Absyn_Conref*b){b=Cyc_Absyn_compress_conref(
b);{union Cyc_Absyn_Constraint_union _tmpAE5=b->v;void*_tmpAE6;void*_tmpAE7;struct
Cyc_Absyn_Exp*_tmpAE8;_LL738: if((_tmpAE5.Eq_constr).tag != 0)goto _LL73A;_tmpAE6=(
_tmpAE5.Eq_constr).f1;if((int)_tmpAE6 != 0)goto _LL73A;_LL739: return;_LL73A: if((
_tmpAE5.Eq_constr).tag != 0)goto _LL73C;_tmpAE7=(_tmpAE5.Eq_constr).f1;if(_tmpAE7
<= (void*)1)goto _LL73C;if(*((int*)_tmpAE7)!= 0)goto _LL73C;_tmpAE8=((struct Cyc_Absyn_Upper_b_struct*)
_tmpAE7)->f1;_LL73B: {unsigned int _tmpAEA;int _tmpAEB;struct _tuple7 _tmpAE9=Cyc_Evexp_eval_const_uint_exp(
_tmpAE8);_tmpAEA=_tmpAE9.f1;_tmpAEB=_tmpAE9.f2;if(_tmpAEB  && _tmpAEA <= i){const
char*_tmp108D;void*_tmp108C[2];struct Cyc_Int_pa_struct _tmp108B;struct Cyc_Int_pa_struct
_tmp108A;(_tmp108A.tag=1,((_tmp108A.f1=(unsigned long)((int)i),((_tmp108B.tag=1,((
_tmp108B.f1=(unsigned long)((int)_tmpAEA),((_tmp108C[0]=& _tmp108B,((_tmp108C[1]=&
_tmp108A,Cyc_Tcutil_terr(loc,((_tmp108D="dereference is out of bounds: %d <= %d",
_tag_dyneither(_tmp108D,sizeof(char),39))),_tag_dyneither(_tmp108C,sizeof(void*),
2)))))))))))));}return;}_LL73C: if((_tmpAE5.No_constr).tag != 2)goto _LL73E;_LL73D:{
struct Cyc_Absyn_Upper_b_struct*_tmp1093;struct Cyc_Absyn_Upper_b_struct _tmp1092;
union Cyc_Absyn_Constraint_union _tmp1091;b->v=(union Cyc_Absyn_Constraint_union)(((
_tmp1091.Eq_constr).tag=0,(((_tmp1091.Eq_constr).f1=(void*)((void*)((void*)((
_tmp1093=_cycalloc(sizeof(*_tmp1093)),((_tmp1093[0]=((_tmp1092.tag=0,((_tmp1092.f1=
Cyc_Absyn_uint_exp(i + 1,0),_tmp1092)))),_tmp1093)))))),_tmp1091))));}return;
_LL73E:;_LL73F: {const char*_tmp1096;void*_tmp1095;(_tmp1095=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1096="check_bound -- bad compressed conref",
_tag_dyneither(_tmp1096,sizeof(char),37))),_tag_dyneither(_tmp1095,sizeof(void*),
0)));}_LL737:;}}void Cyc_Tcutil_check_nonzero_bound(struct Cyc_Position_Segment*
loc,struct Cyc_Absyn_Conref*b);void Cyc_Tcutil_check_nonzero_bound(struct Cyc_Position_Segment*
loc,struct Cyc_Absyn_Conref*b){Cyc_Tcutil_check_bound(loc,0,b);}int Cyc_Tcutil_is_bound_one(
struct Cyc_Absyn_Conref*b);int Cyc_Tcutil_is_bound_one(struct Cyc_Absyn_Conref*b){
union Cyc_Absyn_Constraint_union _tmpAF5=(Cyc_Absyn_compress_conref(b))->v;void*
_tmpAF6;struct Cyc_Absyn_Exp*_tmpAF7;_LL741: if((_tmpAF5.Eq_constr).tag != 0)goto
_LL743;_tmpAF6=(_tmpAF5.Eq_constr).f1;if(_tmpAF6 <= (void*)1)goto _LL743;if(*((int*)
_tmpAF6)!= 0)goto _LL743;_tmpAF7=((struct Cyc_Absyn_Upper_b_struct*)_tmpAF6)->f1;
_LL742: {unsigned int _tmpAF9;int _tmpAFA;struct _tuple7 _tmpAF8=Cyc_Evexp_eval_const_uint_exp(
_tmpAF7);_tmpAF9=_tmpAF8.f1;_tmpAFA=_tmpAF8.f2;return _tmpAFA  && _tmpAF9 == 1;}
_LL743:;_LL744: return 0;_LL740:;}int Cyc_Tcutil_bits_only(void*t);int Cyc_Tcutil_bits_only(
void*t){void*_tmpAFB=Cyc_Tcutil_compress(t);struct Cyc_Absyn_ArrayInfo _tmpAFC;
void*_tmpAFD;struct Cyc_Absyn_Conref*_tmpAFE;struct Cyc_List_List*_tmpAFF;struct
Cyc_Absyn_AggrInfo _tmpB00;union Cyc_Absyn_AggrInfoU_union _tmpB01;struct Cyc_Absyn_AggrInfo
_tmpB02;union Cyc_Absyn_AggrInfoU_union _tmpB03;struct Cyc_Absyn_Aggrdecl**_tmpB04;
struct Cyc_Absyn_Aggrdecl*_tmpB05;struct Cyc_List_List*_tmpB06;struct Cyc_List_List*
_tmpB07;_LL746: if((int)_tmpAFB != 0)goto _LL748;_LL747: goto _LL749;_LL748: if(
_tmpAFB <= (void*)4)goto _LL74A;if(*((int*)_tmpAFB)!= 5)goto _LL74A;_LL749: goto
_LL74B;_LL74A: if((int)_tmpAFB != 1)goto _LL74C;_LL74B: goto _LL74D;_LL74C: if(_tmpAFB
<= (void*)4)goto _LL75C;if(*((int*)_tmpAFB)!= 6)goto _LL74E;_LL74D: return 1;_LL74E:
if(*((int*)_tmpAFB)!= 12)goto _LL750;_LL74F: goto _LL751;_LL750: if(*((int*)_tmpAFB)
!= 13)goto _LL752;_LL751: return 0;_LL752: if(*((int*)_tmpAFB)!= 7)goto _LL754;
_tmpAFC=((struct Cyc_Absyn_ArrayType_struct*)_tmpAFB)->f1;_tmpAFD=(void*)_tmpAFC.elt_type;
_tmpAFE=_tmpAFC.zero_term;_LL753: return !((int(*)(int,struct Cyc_Absyn_Conref*x))
Cyc_Absyn_conref_def)(0,_tmpAFE) && Cyc_Tcutil_bits_only(_tmpAFD);_LL754: if(*((
int*)_tmpAFB)!= 9)goto _LL756;_tmpAFF=((struct Cyc_Absyn_TupleType_struct*)_tmpAFB)->f1;
_LL755: for(0;_tmpAFF != 0;_tmpAFF=_tmpAFF->tl){if(!Cyc_Tcutil_bits_only((*((
struct _tuple5*)_tmpAFF->hd)).f2))return 0;}return 1;_LL756: if(*((int*)_tmpAFB)!= 
10)goto _LL758;_tmpB00=((struct Cyc_Absyn_AggrType_struct*)_tmpAFB)->f1;_tmpB01=
_tmpB00.aggr_info;if((((((struct Cyc_Absyn_AggrType_struct*)_tmpAFB)->f1).aggr_info).UnknownAggr).tag
!= 0)goto _LL758;_LL757: return 0;_LL758: if(*((int*)_tmpAFB)!= 10)goto _LL75A;
_tmpB02=((struct Cyc_Absyn_AggrType_struct*)_tmpAFB)->f1;_tmpB03=_tmpB02.aggr_info;
if((((((struct Cyc_Absyn_AggrType_struct*)_tmpAFB)->f1).aggr_info).KnownAggr).tag
!= 1)goto _LL75A;_tmpB04=(_tmpB03.KnownAggr).f1;_tmpB05=*_tmpB04;_tmpB06=_tmpB02.targs;
_LL759: if(_tmpB05->impl == 0)return 0;{struct _RegionHandle _tmpB08=_new_region("rgn");
struct _RegionHandle*rgn=& _tmpB08;_push_region(rgn);{struct Cyc_List_List*_tmpB09=((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmpB05->tvs,_tmpB06);{struct Cyc_List_List*
fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpB05->impl))->fields;for(0;fs
!= 0;fs=fs->tl){if(!Cyc_Tcutil_bits_only(Cyc_Tcutil_rsubstitute(rgn,_tmpB09,(
void*)((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))){int _tmpB0A=0;_npop_handler(0);
return _tmpB0A;}}}{int _tmpB0B=1;_npop_handler(0);return _tmpB0B;}};_pop_region(rgn);}
_LL75A: if(*((int*)_tmpAFB)!= 11)goto _LL75C;_tmpB07=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmpAFB)->f2;_LL75B: for(0;_tmpB07 != 0;_tmpB07=_tmpB07->tl){if(!Cyc_Tcutil_bits_only((
void*)((struct Cyc_Absyn_Aggrfield*)_tmpB07->hd)->type))return 0;}return 1;_LL75C:;
_LL75D: return 0;_LL745:;}struct _tuple23{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*
f2;};static int Cyc_Tcutil_cnst_exp(struct Cyc_Tcenv_Tenv*te,int var_okay,struct Cyc_Absyn_Exp*
e);static int Cyc_Tcutil_cnst_exp(struct Cyc_Tcenv_Tenv*te,int var_okay,struct Cyc_Absyn_Exp*
e){void*_tmpB0C=(void*)e->r;struct _tuple2*_tmpB0D;void*_tmpB0E;struct Cyc_Absyn_Exp*
_tmpB0F;struct Cyc_Absyn_Exp*_tmpB10;struct Cyc_Absyn_Exp*_tmpB11;struct Cyc_Absyn_Exp*
_tmpB12;struct Cyc_Absyn_Exp*_tmpB13;struct Cyc_Absyn_Exp*_tmpB14;struct Cyc_Absyn_Exp*
_tmpB15;void*_tmpB16;struct Cyc_Absyn_Exp*_tmpB17;void*_tmpB18;void*_tmpB19;
struct Cyc_Absyn_Exp*_tmpB1A;struct Cyc_Absyn_Exp*_tmpB1B;struct Cyc_Absyn_Exp*
_tmpB1C;struct Cyc_Absyn_Exp*_tmpB1D;struct Cyc_List_List*_tmpB1E;struct Cyc_List_List*
_tmpB1F;struct Cyc_List_List*_tmpB20;void*_tmpB21;struct Cyc_List_List*_tmpB22;
struct Cyc_List_List*_tmpB23;struct Cyc_List_List*_tmpB24;_LL75F: if(*((int*)
_tmpB0C)!= 0)goto _LL761;_LL760: goto _LL762;_LL761: if(*((int*)_tmpB0C)!= 18)goto
_LL763;_LL762: goto _LL764;_LL763: if(*((int*)_tmpB0C)!= 19)goto _LL765;_LL764: goto
_LL766;_LL765: if(*((int*)_tmpB0C)!= 20)goto _LL767;_LL766: goto _LL768;_LL767: if(*((
int*)_tmpB0C)!= 21)goto _LL769;_LL768: goto _LL76A;_LL769: if(*((int*)_tmpB0C)!= 33)
goto _LL76B;_LL76A: goto _LL76C;_LL76B: if(*((int*)_tmpB0C)!= 34)goto _LL76D;_LL76C:
return 1;_LL76D: if(*((int*)_tmpB0C)!= 1)goto _LL76F;_tmpB0D=((struct Cyc_Absyn_Var_e_struct*)
_tmpB0C)->f1;_tmpB0E=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmpB0C)->f2;_LL76E: {
void*_tmpB25=_tmpB0E;struct Cyc_Absyn_Vardecl*_tmpB26;_LL78E: if(_tmpB25 <= (void*)
1)goto _LL792;if(*((int*)_tmpB25)!= 1)goto _LL790;_LL78F: return 1;_LL790: if(*((int*)
_tmpB25)!= 0)goto _LL792;_tmpB26=((struct Cyc_Absyn_Global_b_struct*)_tmpB25)->f1;
_LL791: {void*_tmpB27=Cyc_Tcutil_compress((void*)_tmpB26->type);_LL797: if(
_tmpB27 <= (void*)4)goto _LL79B;if(*((int*)_tmpB27)!= 7)goto _LL799;_LL798: goto
_LL79A;_LL799: if(*((int*)_tmpB27)!= 8)goto _LL79B;_LL79A: return 1;_LL79B:;_LL79C:
return var_okay;_LL796:;}_LL792: if((int)_tmpB25 != 0)goto _LL794;_LL793: return 0;
_LL794:;_LL795: return var_okay;_LL78D:;}_LL76F: if(*((int*)_tmpB0C)!= 6)goto _LL771;
_tmpB0F=((struct Cyc_Absyn_Conditional_e_struct*)_tmpB0C)->f1;_tmpB10=((struct Cyc_Absyn_Conditional_e_struct*)
_tmpB0C)->f2;_tmpB11=((struct Cyc_Absyn_Conditional_e_struct*)_tmpB0C)->f3;_LL770:
return(Cyc_Tcutil_cnst_exp(te,0,_tmpB0F) && Cyc_Tcutil_cnst_exp(te,0,_tmpB10))
 && Cyc_Tcutil_cnst_exp(te,0,_tmpB11);_LL771: if(*((int*)_tmpB0C)!= 9)goto _LL773;
_tmpB12=((struct Cyc_Absyn_SeqExp_e_struct*)_tmpB0C)->f1;_tmpB13=((struct Cyc_Absyn_SeqExp_e_struct*)
_tmpB0C)->f2;_LL772: return Cyc_Tcutil_cnst_exp(te,0,_tmpB12) && Cyc_Tcutil_cnst_exp(
te,0,_tmpB13);_LL773: if(*((int*)_tmpB0C)!= 13)goto _LL775;_tmpB14=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmpB0C)->f1;_LL774: _tmpB15=_tmpB14;goto _LL776;_LL775: if(*((int*)_tmpB0C)!= 14)
goto _LL777;_tmpB15=((struct Cyc_Absyn_Instantiate_e_struct*)_tmpB0C)->f1;_LL776:
return Cyc_Tcutil_cnst_exp(te,var_okay,_tmpB15);_LL777: if(*((int*)_tmpB0C)!= 15)
goto _LL779;_tmpB16=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmpB0C)->f1;_tmpB17=((
struct Cyc_Absyn_Cast_e_struct*)_tmpB0C)->f2;_tmpB18=(void*)((struct Cyc_Absyn_Cast_e_struct*)
_tmpB0C)->f4;if((int)_tmpB18 != 1)goto _LL779;_LL778: return Cyc_Tcutil_cnst_exp(te,
var_okay,_tmpB17);_LL779: if(*((int*)_tmpB0C)!= 15)goto _LL77B;_tmpB19=(void*)((
struct Cyc_Absyn_Cast_e_struct*)_tmpB0C)->f1;_tmpB1A=((struct Cyc_Absyn_Cast_e_struct*)
_tmpB0C)->f2;_LL77A: return Cyc_Tcutil_cnst_exp(te,var_okay,_tmpB1A);_LL77B: if(*((
int*)_tmpB0C)!= 16)goto _LL77D;_tmpB1B=((struct Cyc_Absyn_Address_e_struct*)
_tmpB0C)->f1;_LL77C: return Cyc_Tcutil_cnst_exp(te,1,_tmpB1B);_LL77D: if(*((int*)
_tmpB0C)!= 29)goto _LL77F;_tmpB1C=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmpB0C)->f2;_tmpB1D=((struct Cyc_Absyn_Comprehension_e_struct*)_tmpB0C)->f3;
_LL77E: return Cyc_Tcutil_cnst_exp(te,0,_tmpB1C) && Cyc_Tcutil_cnst_exp(te,0,
_tmpB1D);_LL77F: if(*((int*)_tmpB0C)!= 28)goto _LL781;_tmpB1E=((struct Cyc_Absyn_Array_e_struct*)
_tmpB0C)->f1;_LL780: _tmpB1F=_tmpB1E;goto _LL782;_LL781: if(*((int*)_tmpB0C)!= 31)
goto _LL783;_tmpB1F=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmpB0C)->f2;_LL782:
_tmpB20=_tmpB1F;goto _LL784;_LL783: if(*((int*)_tmpB0C)!= 30)goto _LL785;_tmpB20=((
struct Cyc_Absyn_Struct_e_struct*)_tmpB0C)->f3;_LL784: for(0;_tmpB20 != 0;_tmpB20=
_tmpB20->tl){if(!Cyc_Tcutil_cnst_exp(te,0,(*((struct _tuple23*)_tmpB20->hd)).f2))
return 0;}return 1;_LL785: if(*((int*)_tmpB0C)!= 3)goto _LL787;_tmpB21=(void*)((
struct Cyc_Absyn_Primop_e_struct*)_tmpB0C)->f1;_tmpB22=((struct Cyc_Absyn_Primop_e_struct*)
_tmpB0C)->f2;_LL786: _tmpB23=_tmpB22;goto _LL788;_LL787: if(*((int*)_tmpB0C)!= 26)
goto _LL789;_tmpB23=((struct Cyc_Absyn_Tuple_e_struct*)_tmpB0C)->f1;_LL788: _tmpB24=
_tmpB23;goto _LL78A;_LL789: if(*((int*)_tmpB0C)!= 32)goto _LL78B;_tmpB24=((struct
Cyc_Absyn_Tunion_e_struct*)_tmpB0C)->f1;_LL78A: for(0;_tmpB24 != 0;_tmpB24=_tmpB24->tl){
if(!Cyc_Tcutil_cnst_exp(te,0,(struct Cyc_Absyn_Exp*)_tmpB24->hd))return 0;}return 1;
_LL78B:;_LL78C: return 0;_LL75E:;}int Cyc_Tcutil_is_const_exp(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e);int Cyc_Tcutil_is_const_exp(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Absyn_Exp*e){return Cyc_Tcutil_cnst_exp(te,0,e);}static int Cyc_Tcutil_fields_support_default(
struct Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs);int Cyc_Tcutil_supports_default(
void*t);int Cyc_Tcutil_supports_default(void*t){void*_tmpB28=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_PtrInfo _tmpB29;struct Cyc_Absyn_PtrAtts _tmpB2A;struct Cyc_Absyn_Conref*
_tmpB2B;struct Cyc_Absyn_Conref*_tmpB2C;struct Cyc_Absyn_ArrayInfo _tmpB2D;void*
_tmpB2E;struct Cyc_List_List*_tmpB2F;struct Cyc_Absyn_AggrInfo _tmpB30;union Cyc_Absyn_AggrInfoU_union
_tmpB31;struct Cyc_List_List*_tmpB32;struct Cyc_List_List*_tmpB33;_LL79E: if((int)
_tmpB28 != 0)goto _LL7A0;_LL79F: goto _LL7A1;_LL7A0: if(_tmpB28 <= (void*)4)goto _LL7A2;
if(*((int*)_tmpB28)!= 5)goto _LL7A2;_LL7A1: goto _LL7A3;_LL7A2: if((int)_tmpB28 != 1)
goto _LL7A4;_LL7A3: goto _LL7A5;_LL7A4: if(_tmpB28 <= (void*)4)goto _LL7B4;if(*((int*)
_tmpB28)!= 6)goto _LL7A6;_LL7A5: return 1;_LL7A6: if(*((int*)_tmpB28)!= 4)goto _LL7A8;
_tmpB29=((struct Cyc_Absyn_PointerType_struct*)_tmpB28)->f1;_tmpB2A=_tmpB29.ptr_atts;
_tmpB2B=_tmpB2A.nullable;_tmpB2C=_tmpB2A.bounds;_LL7A7: {void*_tmpB34=Cyc_Absyn_conref_def((
void*)((void*)0),_tmpB2C);_LL7B7: if((int)_tmpB34 != 0)goto _LL7B9;_LL7B8: return 1;
_LL7B9:;_LL7BA: return((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
1,_tmpB2B);_LL7B6:;}_LL7A8: if(*((int*)_tmpB28)!= 7)goto _LL7AA;_tmpB2D=((struct
Cyc_Absyn_ArrayType_struct*)_tmpB28)->f1;_tmpB2E=(void*)_tmpB2D.elt_type;_LL7A9:
return Cyc_Tcutil_supports_default(_tmpB2E);_LL7AA: if(*((int*)_tmpB28)!= 9)goto
_LL7AC;_tmpB2F=((struct Cyc_Absyn_TupleType_struct*)_tmpB28)->f1;_LL7AB: for(0;
_tmpB2F != 0;_tmpB2F=_tmpB2F->tl){if(!Cyc_Tcutil_supports_default((*((struct
_tuple5*)_tmpB2F->hd)).f2))return 0;}return 1;_LL7AC: if(*((int*)_tmpB28)!= 10)goto
_LL7AE;_tmpB30=((struct Cyc_Absyn_AggrType_struct*)_tmpB28)->f1;_tmpB31=_tmpB30.aggr_info;
_tmpB32=_tmpB30.targs;_LL7AD: {struct Cyc_Absyn_Aggrdecl*_tmpB35=Cyc_Absyn_get_known_aggrdecl(
_tmpB31);if(_tmpB35->impl == 0)return 0;if(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmpB35->impl))->exist_vars != 0)return 0;return Cyc_Tcutil_fields_support_default(
_tmpB35->tvs,_tmpB32,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpB35->impl))->fields);}
_LL7AE: if(*((int*)_tmpB28)!= 11)goto _LL7B0;_tmpB33=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmpB28)->f2;_LL7AF: return Cyc_Tcutil_fields_support_default(0,0,_tmpB33);_LL7B0:
if(*((int*)_tmpB28)!= 13)goto _LL7B2;_LL7B1: goto _LL7B3;_LL7B2: if(*((int*)_tmpB28)
!= 12)goto _LL7B4;_LL7B3: return 1;_LL7B4:;_LL7B5: return 0;_LL79D:;}static int Cyc_Tcutil_fields_support_default(
struct Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs);static int
Cyc_Tcutil_fields_support_default(struct Cyc_List_List*tvs,struct Cyc_List_List*ts,
struct Cyc_List_List*fs){struct _RegionHandle _tmpB36=_new_region("rgn");struct
_RegionHandle*rgn=& _tmpB36;_push_region(rgn);{struct Cyc_List_List*_tmpB37=((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,tvs,ts);for(0;fs != 0;fs=fs->tl){
void*t=Cyc_Tcutil_rsubstitute(rgn,_tmpB37,(void*)((struct Cyc_Absyn_Aggrfield*)fs->hd)->type);
if(!Cyc_Tcutil_supports_default(t)){int _tmpB38=0;_npop_handler(0);return _tmpB38;}}}{
int _tmpB39=1;_npop_handler(0);return _tmpB39;};_pop_region(rgn);}int Cyc_Tcutil_admits_zero(
void*t);int Cyc_Tcutil_admits_zero(void*t){void*_tmpB3A=Cyc_Tcutil_compress(t);
struct Cyc_Absyn_PtrInfo _tmpB3B;struct Cyc_Absyn_PtrAtts _tmpB3C;struct Cyc_Absyn_Conref*
_tmpB3D;struct Cyc_Absyn_Conref*_tmpB3E;_LL7BC: if(_tmpB3A <= (void*)4)goto _LL7BE;
if(*((int*)_tmpB3A)!= 5)goto _LL7BE;_LL7BD: goto _LL7BF;_LL7BE: if((int)_tmpB3A != 1)
goto _LL7C0;_LL7BF: goto _LL7C1;_LL7C0: if(_tmpB3A <= (void*)4)goto _LL7C4;if(*((int*)
_tmpB3A)!= 6)goto _LL7C2;_LL7C1: return 1;_LL7C2: if(*((int*)_tmpB3A)!= 4)goto _LL7C4;
_tmpB3B=((struct Cyc_Absyn_PointerType_struct*)_tmpB3A)->f1;_tmpB3C=_tmpB3B.ptr_atts;
_tmpB3D=_tmpB3C.nullable;_tmpB3E=_tmpB3C.bounds;_LL7C3: {union Cyc_Absyn_Constraint_union
_tmpB3F=(Cyc_Absyn_compress_conref(_tmpB3E))->v;void*_tmpB40;_LL7C7: if((_tmpB3F.Eq_constr).tag
!= 0)goto _LL7C9;_tmpB40=(_tmpB3F.Eq_constr).f1;if((int)_tmpB40 != 0)goto _LL7C9;
_LL7C8: return 0;_LL7C9: if((_tmpB3F.Eq_constr).tag != 0)goto _LL7CB;_LL7CA: {union
Cyc_Absyn_Constraint_union _tmpB41=(((struct Cyc_Absyn_Conref*(*)(struct Cyc_Absyn_Conref*
x))Cyc_Absyn_compress_conref)(_tmpB3D))->v;int _tmpB42;_LL7CE: if((_tmpB41.Eq_constr).tag
!= 0)goto _LL7D0;_tmpB42=(_tmpB41.Eq_constr).f1;_LL7CF: return _tmpB42;_LL7D0:;
_LL7D1: return 0;_LL7CD:;}_LL7CB:;_LL7CC: return 0;_LL7C6:;}_LL7C4:;_LL7C5: return 0;
_LL7BB:;}int Cyc_Tcutil_is_noreturn(void*t);int Cyc_Tcutil_is_noreturn(void*t){{
void*_tmpB43=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmpB44;void*_tmpB45;
struct Cyc_Absyn_FnInfo _tmpB46;struct Cyc_List_List*_tmpB47;_LL7D3: if(_tmpB43 <= (
void*)4)goto _LL7D7;if(*((int*)_tmpB43)!= 4)goto _LL7D5;_tmpB44=((struct Cyc_Absyn_PointerType_struct*)
_tmpB43)->f1;_tmpB45=(void*)_tmpB44.elt_typ;_LL7D4: return Cyc_Tcutil_is_noreturn(
_tmpB45);_LL7D5: if(*((int*)_tmpB43)!= 8)goto _LL7D7;_tmpB46=((struct Cyc_Absyn_FnType_struct*)
_tmpB43)->f1;_tmpB47=_tmpB46.attributes;_LL7D6: for(0;_tmpB47 != 0;_tmpB47=_tmpB47->tl){
void*_tmpB48=(void*)_tmpB47->hd;_LL7DA: if((int)_tmpB48 != 3)goto _LL7DC;_LL7DB:
return 1;_LL7DC:;_LL7DD: continue;_LL7D9:;}goto _LL7D2;_LL7D7:;_LL7D8: goto _LL7D2;
_LL7D2:;}return 0;}struct Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(void*t,
struct Cyc_List_List*atts);struct Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(
void*t,struct Cyc_List_List*atts){void*_tmpB49=Cyc_Tcutil_compress(t);struct Cyc_Absyn_FnInfo
_tmpB4A;struct Cyc_List_List*_tmpB4B;struct Cyc_List_List**_tmpB4C;_LL7DF: if(
_tmpB49 <= (void*)4)goto _LL7E1;if(*((int*)_tmpB49)!= 8)goto _LL7E1;_tmpB4A=((
struct Cyc_Absyn_FnType_struct*)_tmpB49)->f1;_tmpB4B=_tmpB4A.attributes;_tmpB4C=(
struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_struct*)_tmpB49)->f1).attributes;
_LL7E0: {struct Cyc_List_List*_tmpB4D=0;for(0;atts != 0;atts=atts->tl){if(Cyc_Absyn_fntype_att((
void*)atts->hd)){struct Cyc_List_List*_tmp1097;*_tmpB4C=((_tmp1097=_cycalloc(
sizeof(*_tmp1097)),((_tmp1097->hd=(void*)((void*)atts->hd),((_tmp1097->tl=*
_tmpB4C,_tmp1097))))));}else{struct Cyc_List_List*_tmp1098;_tmpB4D=((_tmp1098=
_cycalloc(sizeof(*_tmp1098)),((_tmp1098->hd=(void*)((void*)atts->hd),((_tmp1098->tl=
_tmpB4D,_tmp1098))))));}}return _tmpB4D;}_LL7E1:;_LL7E2: {const char*_tmp109B;void*
_tmp109A;(_tmp109A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmp109B="transfer_fn_type_atts",_tag_dyneither(_tmp109B,
sizeof(char),22))),_tag_dyneither(_tmp109A,sizeof(void*),0)));}_LL7DE:;}
