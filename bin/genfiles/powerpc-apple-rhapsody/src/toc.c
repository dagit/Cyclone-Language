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
struct _RegionHandle*Cyc_Core_heap_region;struct Cyc_Core_NewRegion Cyc_Core_new_dynregion();
extern char Cyc_Core_Open_Region[16];extern char Cyc_Core_Free_Region[16];void Cyc_Core_free_dynregion(
struct _DynRegionHandle*);struct Cyc___cycFILE;extern struct Cyc___cycFILE*Cyc_stderr;
struct Cyc_Cstdio___abstractFILE;struct Cyc_String_pa_struct{int tag;struct
_dyneither_ptr f1;};struct Cyc_Int_pa_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_struct{
int tag;double f1;};struct Cyc_LongDouble_pa_struct{int tag;long double f1;};struct
Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{int tag;
unsigned long*f1;};struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr,struct
_dyneither_ptr);int Cyc_fflush(struct Cyc___cycFILE*);int Cyc_fprintf(struct Cyc___cycFILE*,
struct _dyneither_ptr,struct _dyneither_ptr);struct Cyc_ShortPtr_sa_struct{int tag;
short*f1;};struct Cyc_UShortPtr_sa_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_struct{
int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_struct{
int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_struct{int tag;double*f1;}
;struct Cyc_FloatPtr_sa_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int
tag;struct _dyneither_ptr f1;};int Cyc_vfprintf(struct Cyc___cycFILE*,struct
_dyneither_ptr,struct _dyneither_ptr);extern char Cyc_FileCloseError[19];extern char
Cyc_FileOpenError[18];struct Cyc_FileOpenError_struct{char*tag;struct
_dyneither_ptr f1;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};struct
Cyc_List_List*Cyc_List_list(struct _dyneither_ptr);int Cyc_List_length(struct Cyc_List_List*
x);struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);
struct Cyc_List_List*Cyc_List_rmap(struct _RegionHandle*,void*(*f)(void*),struct
Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rmap_c(struct _RegionHandle*,void*(*
f)(void*,void*),void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[
18];void Cyc_List_iter(void(*f)(void*),struct Cyc_List_List*x);void Cyc_List_iter_c(
void(*f)(void*,void*),void*env,struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rev(
struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rrev(struct _RegionHandle*,
struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*
x);struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*
y);struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*x,struct Cyc_List_List*
y);extern char Cyc_List_Nth[8];void*Cyc_List_nth(struct Cyc_List_List*x,int n);int
Cyc_List_forall(int(*pred)(void*),struct Cyc_List_List*x);int Cyc_strcmp(struct
_dyneither_ptr s1,struct _dyneither_ptr s2);struct _dyneither_ptr Cyc_strconcat(
struct _dyneither_ptr,struct _dyneither_ptr);struct Cyc_Iter_Iter{void*env;int(*
next)(void*env,void*dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct
Cyc_Set_Set;struct Cyc_Set_Set*Cyc_Set_rempty(struct _RegionHandle*r,int(*cmp)(
void*,void*));struct Cyc_Set_Set*Cyc_Set_rinsert(struct _RegionHandle*r,struct Cyc_Set_Set*
s,void*elt);int Cyc_Set_member(struct Cyc_Set_Set*s,void*elt);extern char Cyc_Set_Absent[
11];struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct
_RegionHandle*r;struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[12];extern char
Cyc_Dict_Absent[11];struct Cyc_Dict_Dict Cyc_Dict_rempty(struct _RegionHandle*,int(*
cmp)(void*,void*));struct Cyc_Dict_Dict Cyc_Dict_rshare(struct _RegionHandle*,
struct Cyc_Dict_Dict);struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict d,
void*k,void*v);void*Cyc_Dict_lookup(struct Cyc_Dict_Dict d,void*k);void**Cyc_Dict_lookup_opt(
struct Cyc_Dict_Dict d,void*k);struct _tuple0{void*f1;void*f2;};struct _tuple0*Cyc_Dict_rchoose(
struct _RegionHandle*r,struct Cyc_Dict_Dict d);struct _tuple0*Cyc_Dict_rchoose(
struct _RegionHandle*,struct Cyc_Dict_Dict d);struct Cyc_Lineno_Pos{struct
_dyneither_ptr logical_file;struct _dyneither_ptr line;int line_no;int col;};extern
char Cyc_Position_Exit[9];struct Cyc_Position_Segment;struct _dyneither_ptr Cyc_Position_string_of_segment(
struct Cyc_Position_Segment*);struct Cyc_Position_Error{struct _dyneither_ptr source;
struct Cyc_Position_Segment*seg;void*kind;struct _dyneither_ptr desc;};extern char
Cyc_Position_Nocontext[14];struct Cyc_Xarray_Xarray{struct _RegionHandle*r;struct
_dyneither_ptr elmts;int num_elmts;};int Cyc_Xarray_length(struct Cyc_Xarray_Xarray*);
void*Cyc_Xarray_get(struct Cyc_Xarray_Xarray*,int);struct Cyc_Xarray_Xarray*Cyc_Xarray_rcreate_empty(
struct _RegionHandle*);int Cyc_Xarray_add_ind(struct Cyc_Xarray_Xarray*,void*);
struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct
_union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{
int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct
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
UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};struct
Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*
targs;struct Cyc_Core_Opt*rgn;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct
_tuple1*datatype_name;struct _tuple1*field_name;int is_extensible;};struct
_union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo
val;};struct _tuple2{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*
f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple2 val;
};union Cyc_Absyn_DatatypeFieldInfoU{struct
_union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct
_union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};struct Cyc_Absyn_DatatypeFieldInfo{
union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct
_tuple3{void*f1;struct _tuple1*f2;struct Cyc_Core_Opt*f3;};struct
_union_AggrInfoU_UnknownAggr{int tag;struct _tuple3 val;};struct
_union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{
struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr
KnownAggr;};struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*
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
_union_Cnst_String_c String_c;};union Cyc_Absyn_Cnst Cyc_Absyn_Char_c(void*,char);
union Cyc_Absyn_Cnst Cyc_Absyn_Short_c(void*,short);union Cyc_Absyn_Cnst Cyc_Absyn_Int_c(
void*,int);union Cyc_Absyn_Cnst Cyc_Absyn_LongLong_c(void*,long long);union Cyc_Absyn_Cnst
Cyc_Absyn_Float_c(struct _dyneither_ptr);struct Cyc_Absyn_VarargCallInfo{int
num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};
struct Cyc_Absyn_StructField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_struct{
int tag;unsigned int f1;};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*
rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;};struct Cyc_Absyn_Const_e_struct{
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
_dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];int Cyc_Absyn_qvar_cmp(
struct _tuple1*,struct _tuple1*);void*Cyc_Absyn_conref_val(union Cyc_Absyn_Constraint*
x);void*Cyc_Absyn_conref_def(void*y,union Cyc_Absyn_Constraint*x);extern union Cyc_Absyn_Constraint*
Cyc_Absyn_true_conref;extern union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref;
extern void*Cyc_Absyn_char_typ;extern void*Cyc_Absyn_uint_typ;extern void*Cyc_Absyn_sint_typ;
extern void*Cyc_Absyn_exn_typ;extern void*Cyc_Absyn_bounds_one;void*Cyc_Absyn_star_typ(
void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);
void*Cyc_Absyn_cstar_typ(void*t,struct Cyc_Absyn_Tqual tq);void*Cyc_Absyn_dyneither_typ(
void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);
void*Cyc_Absyn_void_star_typ();void*Cyc_Absyn_strct(struct _dyneither_ptr*name);
void*Cyc_Absyn_strctq(struct _tuple1*name);void*Cyc_Absyn_unionq_typ(struct
_tuple1*name);void*Cyc_Absyn_array_typ(void*elt_type,struct Cyc_Absyn_Tqual tq,
struct Cyc_Absyn_Exp*num_elts,union Cyc_Absyn_Constraint*zero_term,struct Cyc_Position_Segment*
ztloc);struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);struct Cyc_Absyn_Exp*
Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_null_exp(struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(
void*,int,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_signed_int_exp(
int,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(
unsigned int,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_char_exp(
char c,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_float_exp(
struct _dyneither_ptr f,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(
struct _dyneither_ptr s,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_var_exp(
struct _tuple1*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(
struct _tuple1*,void*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(
void*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(
void*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_add_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_times_exp(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_divide_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_eq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_neq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_lt_exp(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_lte_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_assignop_exp(struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_assign_exp(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_post_inc_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_fncall_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_throw_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*,struct Cyc_Absyn_Exp*,int user_cast,
void*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(
void*t,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_sizeofexp_exp(
struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(
struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,struct Cyc_Position_Segment*);struct
Cyc_Absyn_Exp*Cyc_Absyn_aggrarrow_exp(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct
Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_match_exn_exp(struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(
void*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_unresolvedmem_exp(
struct Cyc_Core_Opt*,struct Cyc_List_List*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Stmt*
Cyc_Absyn_new_stmt(void*s,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*
Cyc_Absyn_skip_stmt(struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(
struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(
struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmts(struct Cyc_List_List*,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_return_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(struct Cyc_Absyn_Exp*e,struct
Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,struct Cyc_Position_Segment*loc);struct
Cyc_Absyn_Stmt*Cyc_Absyn_for_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,
struct Cyc_Absyn_Exp*e3,struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_switch_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*,
struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(struct
Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*loc);struct
Cyc_Absyn_Stmt*Cyc_Absyn_declare_stmt(struct _tuple1*,void*,struct Cyc_Absyn_Exp*
init,struct Cyc_Absyn_Stmt*,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*
Cyc_Absyn_label_stmt(struct _dyneither_ptr*v,struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_goto_stmt(struct _dyneither_ptr*lab,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_assign_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*
r,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(
struct _tuple1*x,void*t,struct Cyc_Absyn_Exp*init);struct Cyc_Absyn_Vardecl*Cyc_Absyn_static_vardecl(
struct _tuple1*x,void*t,struct Cyc_Absyn_Exp*init);int Cyc_Absyn_is_lvalue(struct
Cyc_Absyn_Exp*);struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,
struct _dyneither_ptr*);struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(
struct Cyc_Absyn_Aggrdecl*,struct _dyneither_ptr*);struct _dyneither_ptr*Cyc_Absyn_fieldname(
int);struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfoU
info);extern int Cyc_Absyn_no_regions;struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct
Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs: 1;int qvar_to_Cids: 1;int
add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int rewrite_temp_tvars: 1;int
print_all_tvars: 1;int print_all_kinds: 1;int print_all_effects: 1;int
print_using_stmts: 1;int print_externC_stmts: 1;int print_full_evars: 1;int
print_zeroterm: 1;int generate_line_directives: 1;int use_curr_namespace: 1;struct Cyc_List_List*
curr_namespace;};struct _dyneither_ptr Cyc_Absynpp_typ2string(void*);struct
_dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);struct _dyneither_ptr
Cyc_Absynpp_stmt2string(struct Cyc_Absyn_Stmt*);struct _dyneither_ptr Cyc_Absynpp_qvar2string(
struct _tuple1*);struct Cyc_RgnOrder_RgnPO;struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_initial_fn_po(
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
le;int allow_valueof;};void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap);void Cyc_Tcutil_terr(struct Cyc_Position_Segment*,struct
_dyneither_ptr fmt,struct _dyneither_ptr ap);void Cyc_Tcutil_warn(struct Cyc_Position_Segment*,
struct _dyneither_ptr fmt,struct _dyneither_ptr ap);void*Cyc_Tcutil_tvar_kind(struct
Cyc_Absyn_Tvar*t);void*Cyc_Tcutil_typ_kind(void*t);void*Cyc_Tcutil_compress(void*
t);int Cyc_Tcutil_is_pointer_type(void*t);int Cyc_Tcutil_is_pointer_or_boxed(void*
t,int*is_dyneither_ptr);int Cyc_Tcutil_unify(void*,void*);struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(
struct _RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,
void*,struct Cyc_List_List*fields);int Cyc_Tcutil_is_tagged_pointer_typ(void*);int
Cyc_Tcutil_is_tagged_pointer_typ_elt(void*t,void**elt_typ_dest);int Cyc_Tcutil_is_zero_pointer_typ_elt(
void*t,void**elt_typ_dest);int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*
e1,void**ptr_type,int*is_dyneither,void**elt_type);struct _tuple10{struct Cyc_Absyn_Tqual
f1;void*f2;};void*Cyc_Tcutil_snd_tqt(struct _tuple10*);struct _tuple11{
unsigned int f1;int f2;};struct _tuple11 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*
e);int Cyc_Evexp_c_can_eval(struct Cyc_Absyn_Exp*e);int Cyc_Evexp_same_const_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);int Cyc_Evexp_lte_const_exp(struct
Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);struct Cyc_CfFlowInfo_VarRoot_struct{int
tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_CfFlowInfo_MallocPt_struct{int tag;
struct Cyc_Absyn_Exp*f1;void*f2;};struct Cyc_CfFlowInfo_InitParam_struct{int tag;
int f1;void*f2;};struct Cyc_CfFlowInfo_Place{void*root;struct Cyc_List_List*fields;
};struct Cyc_CfFlowInfo_UniquePlace{struct Cyc_CfFlowInfo_Place place;struct Cyc_List_List*
path;};struct _union_RelnOp_EqualConst{int tag;unsigned int val;};struct _tuple12{
struct Cyc_Absyn_Vardecl*f1;void*f2;};struct _union_RelnOp_LessVar{int tag;struct
_tuple12 val;};struct _union_RelnOp_LessNumelts{int tag;struct Cyc_Absyn_Vardecl*val;
};struct _union_RelnOp_LessConst{int tag;unsigned int val;};struct
_union_RelnOp_LessEqNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};union Cyc_CfFlowInfo_RelnOp{
struct _union_RelnOp_EqualConst EqualConst;struct _union_RelnOp_LessVar LessVar;
struct _union_RelnOp_LessNumelts LessNumelts;struct _union_RelnOp_LessConst
LessConst;struct _union_RelnOp_LessEqNumelts LessEqNumelts;};struct Cyc_CfFlowInfo_Reln{
struct Cyc_Absyn_Vardecl*vd;union Cyc_CfFlowInfo_RelnOp rop;};struct Cyc_CfFlowInfo_TagCmp{
void*cmp;void*bd;};extern char Cyc_CfFlowInfo_HasTagCmps[15];struct Cyc_CfFlowInfo_HasTagCmps_struct{
char*tag;struct Cyc_List_List*f1;};extern char Cyc_CfFlowInfo_IsZero[11];extern char
Cyc_CfFlowInfo_NotZero[12];struct Cyc_CfFlowInfo_NotZero_struct{char*tag;struct
Cyc_List_List*f1;};extern char Cyc_CfFlowInfo_UnknownZ[13];struct Cyc_CfFlowInfo_UnknownZ_struct{
char*tag;struct Cyc_List_List*f1;};struct _union_AbsLVal_PlaceL{int tag;struct Cyc_CfFlowInfo_Place*
val;};struct _union_AbsLVal_UnknownL{int tag;int val;};union Cyc_CfFlowInfo_AbsLVal{
struct _union_AbsLVal_PlaceL PlaceL;struct _union_AbsLVal_UnknownL UnknownL;};struct
Cyc_CfFlowInfo_UnknownR_struct{int tag;void*f1;};struct Cyc_CfFlowInfo_Esc_struct{
int tag;void*f1;};struct Cyc_CfFlowInfo_AddressOf_struct{int tag;struct Cyc_CfFlowInfo_Place*
f1;};struct Cyc_CfFlowInfo_TagCmps_struct{int tag;struct Cyc_List_List*f1;};struct
Cyc_CfFlowInfo_Aggregate_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_CfFlowInfo_ConsumeInfo{
struct Cyc_Dict_Dict consumed;struct Cyc_List_List*may_consume;};struct
_union_FlowInfo_BottomFL{int tag;int val;};struct _tuple13{struct Cyc_Dict_Dict f1;
struct Cyc_List_List*f2;struct Cyc_CfFlowInfo_ConsumeInfo f3;};struct
_union_FlowInfo_ReachableFL{int tag;struct _tuple13 val;};union Cyc_CfFlowInfo_FlowInfo{
struct _union_FlowInfo_BottomFL BottomFL;struct _union_FlowInfo_ReachableFL
ReachableFL;};struct Cyc_CfFlowInfo_FlowEnv{struct _RegionHandle*r;void*
unknown_none;void*unknown_this;void*unknown_all;void*esc_none;void*esc_this;void*
esc_all;struct Cyc_Dict_Dict mt_flowdict;struct Cyc_Dict_Dict mt_place_set;struct Cyc_CfFlowInfo_Place*
dummy_place;};struct Cyc_CfFlowInfo_Region_k_struct{int tag;struct Cyc_Absyn_Tvar*
f1;};struct Cyc_List_List*Cyc_Toc_toc(struct Cyc_List_List*ds);struct _tuple1*Cyc_Toc_temp_var();
extern struct _dyneither_ptr Cyc_Toc_globals;extern int Cyc_noexpand_r;int Cyc_Toc_warn_bounds_checks=
0;int Cyc_Toc_warn_all_null_deref=0;unsigned int Cyc_Toc_total_bounds_checks=0;
unsigned int Cyc_Toc_bounds_checks_eliminated=0;static struct Cyc_List_List*Cyc_Toc_result_decls=
0;struct Cyc_Toc_TocState{struct _DynRegionHandle*dyn;struct Cyc_List_List**
tuple_types;struct Cyc_Dict_Dict*aggrs_so_far;struct Cyc_Set_Set**datatypes_so_far;
struct Cyc_Dict_Dict*xdatatypes_so_far;struct Cyc_Dict_Dict*qvar_tags;struct Cyc_Xarray_Xarray*
temp_labels;};static struct Cyc_Toc_TocState*Cyc_Toc_toc_state=0;struct _tuple14{
struct _tuple1*f1;struct _dyneither_ptr f2;};int Cyc_Toc_qvar_tag_cmp(struct _tuple14*
x,struct _tuple14*y);int Cyc_Toc_qvar_tag_cmp(struct _tuple14*x,struct _tuple14*y){
struct _tuple1*_tmp1;struct _dyneither_ptr _tmp2;struct _tuple14 _tmp0=*x;_tmp1=_tmp0.f1;
_tmp2=_tmp0.f2;{struct _tuple1*_tmp4;struct _dyneither_ptr _tmp5;struct _tuple14
_tmp3=*y;_tmp4=_tmp3.f1;_tmp5=_tmp3.f2;{int i=Cyc_Absyn_qvar_cmp(_tmp1,_tmp4);if(
i != 0)return i;return Cyc_strcmp((struct _dyneither_ptr)_tmp2,(struct _dyneither_ptr)
_tmp5);}}}struct _tuple15{struct Cyc_Absyn_Aggrdecl*f1;void*f2;};void*Cyc_Toc_aggrdecl_type(
struct _tuple1*q,void*(*type_maker)(struct _tuple1*));void*Cyc_Toc_aggrdecl_type(
struct _tuple1*q,void*(*type_maker)(struct _tuple1*)){struct _DynRegionHandle*_tmp7;
struct Cyc_Dict_Dict*_tmp8;struct Cyc_Toc_TocState _tmp6=*((struct Cyc_Toc_TocState*)
_check_null(Cyc_Toc_toc_state));_tmp7=_tmp6.dyn;_tmp8=_tmp6.aggrs_so_far;{struct
_DynRegionFrame _tmp9;struct _RegionHandle*d=_open_dynregion(& _tmp9,_tmp7);{struct
_tuple15**v=((struct _tuple15**(*)(struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup_opt)(*
_tmp8,q);if(v == 0){void*_tmpA=type_maker(q);_npop_handler(0);return _tmpA;}else{
struct _tuple15 _tmpC;void*_tmpD;struct _tuple15*_tmpB=*v;_tmpC=*_tmpB;_tmpD=_tmpC.f2;{
void*_tmpE=_tmpD;_npop_handler(0);return _tmpE;}}};_pop_dynregion(d);}}static int
Cyc_Toc_tuple_type_counter=0;static int Cyc_Toc_temp_var_counter=0;static int Cyc_Toc_fresh_label_counter=
0;char Cyc_Toc_Toc_Unimplemented[22]="\000\000\000\000Toc_Unimplemented\000";char
Cyc_Toc_Toc_Impossible[19]="\000\000\000\000Toc_Impossible\000";static void*Cyc_Toc_unimp(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap);static void*Cyc_Toc_unimp(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap){Cyc_vfprintf(Cyc_stderr,fmt,ap);{const
char*_tmp8A1;void*_tmp8A0;(_tmp8A0=0,Cyc_fprintf(Cyc_stderr,((_tmp8A1="\n",
_tag_dyneither(_tmp8A1,sizeof(char),2))),_tag_dyneither(_tmp8A0,sizeof(void*),0)));}
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);(int)_throw((void*)Cyc_Toc_Toc_Unimplemented);}
static void*Cyc_Toc_toc_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
static void*Cyc_Toc_toc_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){Cyc_vfprintf(
Cyc_stderr,fmt,ap);{const char*_tmp8A4;void*_tmp8A3;(_tmp8A3=0,Cyc_fprintf(Cyc_stderr,((
_tmp8A4="\n",_tag_dyneither(_tmp8A4,sizeof(char),2))),_tag_dyneither(_tmp8A3,
sizeof(void*),0)));}Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);(int)_throw((
void*)Cyc_Toc_Toc_Impossible);}char Cyc_Toc_Match_error[16]="\000\000\000\000Match_error\000";
static char _tmp13[5]="curr";static struct _dyneither_ptr Cyc_Toc_curr_string={_tmp13,
_tmp13,_tmp13 + 5};static struct _dyneither_ptr*Cyc_Toc_curr_sp=& Cyc_Toc_curr_string;
static char _tmp14[4]="tag";static struct _dyneither_ptr Cyc_Toc_tag_string={_tmp14,
_tmp14,_tmp14 + 4};static struct _dyneither_ptr*Cyc_Toc_tag_sp=& Cyc_Toc_tag_string;
static char _tmp15[4]="val";static struct _dyneither_ptr Cyc_Toc_val_string={_tmp15,
_tmp15,_tmp15 + 4};static struct _dyneither_ptr*Cyc_Toc_val_sp=& Cyc_Toc_val_string;
static char _tmp16[14]="_handler_cons";static struct _dyneither_ptr Cyc_Toc__handler_cons_string={
_tmp16,_tmp16,_tmp16 + 14};static struct _dyneither_ptr*Cyc_Toc__handler_cons_sp=&
Cyc_Toc__handler_cons_string;static char _tmp17[8]="handler";static struct
_dyneither_ptr Cyc_Toc_handler_string={_tmp17,_tmp17,_tmp17 + 8};static struct
_dyneither_ptr*Cyc_Toc_handler_sp=& Cyc_Toc_handler_string;static char _tmp18[14]="_RegionHandle";
static struct _dyneither_ptr Cyc_Toc__RegionHandle_string={_tmp18,_tmp18,_tmp18 + 14};
static struct _dyneither_ptr*Cyc_Toc__RegionHandle_sp=& Cyc_Toc__RegionHandle_string;
static char _tmp19[17]="_DynRegionHandle";static struct _dyneither_ptr Cyc_Toc__DynRegionHandle_string={
_tmp19,_tmp19,_tmp19 + 17};static struct _dyneither_ptr*Cyc_Toc__DynRegionHandle_sp=&
Cyc_Toc__DynRegionHandle_string;static char _tmp1A[16]="_DynRegionFrame";static
struct _dyneither_ptr Cyc_Toc__DynRegionFrame_string={_tmp1A,_tmp1A,_tmp1A + 16};
static struct _dyneither_ptr*Cyc_Toc__DynRegionFrame_sp=& Cyc_Toc__DynRegionFrame_string;
struct _dyneither_ptr Cyc_Toc_globals={(void*)0,(void*)0,(void*)(0 + 0)};static char
_tmp1B[7]="_throw";static struct _dyneither_ptr Cyc_Toc__throw_str={_tmp1B,_tmp1B,
_tmp1B + 7};static struct _tuple1 Cyc_Toc__throw_pr={{.Loc_n={3,0}},& Cyc_Toc__throw_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__throw_re={1,& Cyc_Toc__throw_pr,(void*)((
void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__throw_ev={0,(void*)& Cyc_Toc__throw_re,
0,(void*)Cyc_Absyn_EmptyAnnot};static struct Cyc_Absyn_Exp*Cyc_Toc__throw_e=& Cyc_Toc__throw_ev;
static char _tmp1D[7]="setjmp";static struct _dyneither_ptr Cyc_Toc_setjmp_str={
_tmp1D,_tmp1D,_tmp1D + 7};static struct _tuple1 Cyc_Toc_setjmp_pr={{.Loc_n={3,0}},&
Cyc_Toc_setjmp_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc_setjmp_re={1,& Cyc_Toc_setjmp_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc_setjmp_ev={0,(void*)& Cyc_Toc_setjmp_re,
0,(void*)Cyc_Absyn_EmptyAnnot};static struct Cyc_Absyn_Exp*Cyc_Toc_setjmp_e=& Cyc_Toc_setjmp_ev;
static char _tmp1F[14]="_push_handler";static struct _dyneither_ptr Cyc_Toc__push_handler_str={
_tmp1F,_tmp1F,_tmp1F + 14};static struct _tuple1 Cyc_Toc__push_handler_pr={{.Loc_n={
3,0}},& Cyc_Toc__push_handler_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__push_handler_re={
1,& Cyc_Toc__push_handler_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__push_handler_ev={
0,(void*)& Cyc_Toc__push_handler_re,0,(void*)Cyc_Absyn_EmptyAnnot};static struct
Cyc_Absyn_Exp*Cyc_Toc__push_handler_e=& Cyc_Toc__push_handler_ev;static char _tmp21[
13]="_pop_handler";static struct _dyneither_ptr Cyc_Toc__pop_handler_str={_tmp21,
_tmp21,_tmp21 + 13};static struct _tuple1 Cyc_Toc__pop_handler_pr={{.Loc_n={3,0}},&
Cyc_Toc__pop_handler_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__pop_handler_re={
1,& Cyc_Toc__pop_handler_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__pop_handler_ev={
0,(void*)& Cyc_Toc__pop_handler_re,0,(void*)Cyc_Absyn_EmptyAnnot};static struct Cyc_Absyn_Exp*
Cyc_Toc__pop_handler_e=& Cyc_Toc__pop_handler_ev;static char _tmp23[12]="_exn_thrown";
static struct _dyneither_ptr Cyc_Toc__exn_thrown_str={_tmp23,_tmp23,_tmp23 + 12};
static struct _tuple1 Cyc_Toc__exn_thrown_pr={{.Loc_n={3,0}},& Cyc_Toc__exn_thrown_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__exn_thrown_re={1,& Cyc_Toc__exn_thrown_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__exn_thrown_ev={0,(void*)& Cyc_Toc__exn_thrown_re,
0,(void*)Cyc_Absyn_EmptyAnnot};static struct Cyc_Absyn_Exp*Cyc_Toc__exn_thrown_e=&
Cyc_Toc__exn_thrown_ev;static char _tmp25[14]="_npop_handler";static struct
_dyneither_ptr Cyc_Toc__npop_handler_str={_tmp25,_tmp25,_tmp25 + 14};static struct
_tuple1 Cyc_Toc__npop_handler_pr={{.Loc_n={3,0}},& Cyc_Toc__npop_handler_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__npop_handler_re={1,& Cyc_Toc__npop_handler_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__npop_handler_ev={0,(void*)&
Cyc_Toc__npop_handler_re,0,(void*)Cyc_Absyn_EmptyAnnot};static struct Cyc_Absyn_Exp*
Cyc_Toc__npop_handler_e=& Cyc_Toc__npop_handler_ev;static char _tmp27[12]="_check_null";
static struct _dyneither_ptr Cyc_Toc__check_null_str={_tmp27,_tmp27,_tmp27 + 12};
static struct _tuple1 Cyc_Toc__check_null_pr={{.Loc_n={3,0}},& Cyc_Toc__check_null_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__check_null_re={1,& Cyc_Toc__check_null_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__check_null_ev={0,(void*)& Cyc_Toc__check_null_re,
0,(void*)Cyc_Absyn_EmptyAnnot};static struct Cyc_Absyn_Exp*Cyc_Toc__check_null_e=&
Cyc_Toc__check_null_ev;static char _tmp29[28]="_check_known_subscript_null";static
struct _dyneither_ptr Cyc_Toc__check_known_subscript_null_str={_tmp29,_tmp29,
_tmp29 + 28};static struct _tuple1 Cyc_Toc__check_known_subscript_null_pr={{.Loc_n={
3,0}},& Cyc_Toc__check_known_subscript_null_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__check_known_subscript_null_re={1,& Cyc_Toc__check_known_subscript_null_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__check_known_subscript_null_ev={
0,(void*)& Cyc_Toc__check_known_subscript_null_re,0,(void*)Cyc_Absyn_EmptyAnnot};
static struct Cyc_Absyn_Exp*Cyc_Toc__check_known_subscript_null_e=& Cyc_Toc__check_known_subscript_null_ev;
static char _tmp2B[31]="_check_known_subscript_notnull";static struct _dyneither_ptr
Cyc_Toc__check_known_subscript_notnull_str={_tmp2B,_tmp2B,_tmp2B + 31};static
struct _tuple1 Cyc_Toc__check_known_subscript_notnull_pr={{.Loc_n={3,0}},& Cyc_Toc__check_known_subscript_notnull_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__check_known_subscript_notnull_re={1,&
Cyc_Toc__check_known_subscript_notnull_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp
Cyc_Toc__check_known_subscript_notnull_ev={0,(void*)& Cyc_Toc__check_known_subscript_notnull_re,
0,(void*)Cyc_Absyn_EmptyAnnot};static struct Cyc_Absyn_Exp*Cyc_Toc__check_known_subscript_notnull_e=&
Cyc_Toc__check_known_subscript_notnull_ev;static char _tmp2D[27]="_check_dyneither_subscript";
static struct _dyneither_ptr Cyc_Toc__check_dyneither_subscript_str={_tmp2D,_tmp2D,
_tmp2D + 27};static struct _tuple1 Cyc_Toc__check_dyneither_subscript_pr={{.Loc_n={3,
0}},& Cyc_Toc__check_dyneither_subscript_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__check_dyneither_subscript_re={1,& Cyc_Toc__check_dyneither_subscript_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__check_dyneither_subscript_ev={
0,(void*)& Cyc_Toc__check_dyneither_subscript_re,0,(void*)Cyc_Absyn_EmptyAnnot};
static struct Cyc_Absyn_Exp*Cyc_Toc__check_dyneither_subscript_e=& Cyc_Toc__check_dyneither_subscript_ev;
static char _tmp2F[15]="_dyneither_ptr";static struct _dyneither_ptr Cyc_Toc__dyneither_ptr_str={
_tmp2F,_tmp2F,_tmp2F + 15};static struct _tuple1 Cyc_Toc__dyneither_ptr_pr={{.Loc_n={
3,0}},& Cyc_Toc__dyneither_ptr_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__dyneither_ptr_re={
1,& Cyc_Toc__dyneither_ptr_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_ev={
0,(void*)& Cyc_Toc__dyneither_ptr_re,0,(void*)Cyc_Absyn_EmptyAnnot};static struct
Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_e=& Cyc_Toc__dyneither_ptr_ev;static char
_tmp31[15]="_tag_dyneither";static struct _dyneither_ptr Cyc_Toc__tag_dyneither_str={
_tmp31,_tmp31,_tmp31 + 15};static struct _tuple1 Cyc_Toc__tag_dyneither_pr={{.Loc_n={
3,0}},& Cyc_Toc__tag_dyneither_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__tag_dyneither_re={
1,& Cyc_Toc__tag_dyneither_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__tag_dyneither_ev={
0,(void*)& Cyc_Toc__tag_dyneither_re,0,(void*)Cyc_Absyn_EmptyAnnot};static struct
Cyc_Absyn_Exp*Cyc_Toc__tag_dyneither_e=& Cyc_Toc__tag_dyneither_ev;static char
_tmp33[20]="_init_dyneither_ptr";static struct _dyneither_ptr Cyc_Toc__init_dyneither_ptr_str={
_tmp33,_tmp33,_tmp33 + 20};static struct _tuple1 Cyc_Toc__init_dyneither_ptr_pr={{.Loc_n={
3,0}},& Cyc_Toc__init_dyneither_ptr_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__init_dyneither_ptr_re={
1,& Cyc_Toc__init_dyneither_ptr_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp
Cyc_Toc__init_dyneither_ptr_ev={0,(void*)& Cyc_Toc__init_dyneither_ptr_re,0,(void*)
Cyc_Absyn_EmptyAnnot};static struct Cyc_Absyn_Exp*Cyc_Toc__init_dyneither_ptr_e=&
Cyc_Toc__init_dyneither_ptr_ev;static char _tmp35[21]="_untag_dyneither_ptr";
static struct _dyneither_ptr Cyc_Toc__untag_dyneither_ptr_str={_tmp35,_tmp35,_tmp35
+ 21};static struct _tuple1 Cyc_Toc__untag_dyneither_ptr_pr={{.Loc_n={3,0}},& Cyc_Toc__untag_dyneither_ptr_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__untag_dyneither_ptr_re={1,& Cyc_Toc__untag_dyneither_ptr_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__untag_dyneither_ptr_ev={0,(
void*)& Cyc_Toc__untag_dyneither_ptr_re,0,(void*)Cyc_Absyn_EmptyAnnot};static
struct Cyc_Absyn_Exp*Cyc_Toc__untag_dyneither_ptr_e=& Cyc_Toc__untag_dyneither_ptr_ev;
static char _tmp37[20]="_get_dyneither_size";static struct _dyneither_ptr Cyc_Toc__get_dyneither_size_str={
_tmp37,_tmp37,_tmp37 + 20};static struct _tuple1 Cyc_Toc__get_dyneither_size_pr={{.Loc_n={
3,0}},& Cyc_Toc__get_dyneither_size_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__get_dyneither_size_re={
1,& Cyc_Toc__get_dyneither_size_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp
Cyc_Toc__get_dyneither_size_ev={0,(void*)& Cyc_Toc__get_dyneither_size_re,0,(void*)
Cyc_Absyn_EmptyAnnot};static struct Cyc_Absyn_Exp*Cyc_Toc__get_dyneither_size_e=&
Cyc_Toc__get_dyneither_size_ev;static char _tmp39[19]="_get_zero_arr_size";static
struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_str={_tmp39,_tmp39,_tmp39 + 19};
static struct _tuple1 Cyc_Toc__get_zero_arr_size_pr={{.Loc_n={3,0}},& Cyc_Toc__get_zero_arr_size_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__get_zero_arr_size_re={1,& Cyc_Toc__get_zero_arr_size_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_ev={0,(
void*)& Cyc_Toc__get_zero_arr_size_re,0,(void*)Cyc_Absyn_EmptyAnnot};static struct
Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_e=& Cyc_Toc__get_zero_arr_size_ev;static
char _tmp3B[24]="_get_zero_arr_size_char";static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_char_str={
_tmp3B,_tmp3B,_tmp3B + 24};static struct _tuple1 Cyc_Toc__get_zero_arr_size_char_pr={{.Loc_n={
3,0}},& Cyc_Toc__get_zero_arr_size_char_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__get_zero_arr_size_char_re={1,& Cyc_Toc__get_zero_arr_size_char_pr,(void*)((
void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_char_ev={0,(void*)&
Cyc_Toc__get_zero_arr_size_char_re,0,(void*)Cyc_Absyn_EmptyAnnot};static struct
Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_char_e=& Cyc_Toc__get_zero_arr_size_char_ev;
static char _tmp3D[25]="_get_zero_arr_size_short";static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_short_str={
_tmp3D,_tmp3D,_tmp3D + 25};static struct _tuple1 Cyc_Toc__get_zero_arr_size_short_pr={{.Loc_n={
3,0}},& Cyc_Toc__get_zero_arr_size_short_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__get_zero_arr_size_short_re={1,& Cyc_Toc__get_zero_arr_size_short_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_short_ev={
0,(void*)& Cyc_Toc__get_zero_arr_size_short_re,0,(void*)Cyc_Absyn_EmptyAnnot};
static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_short_e=& Cyc_Toc__get_zero_arr_size_short_ev;
static char _tmp3F[23]="_get_zero_arr_size_int";static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_int_str={
_tmp3F,_tmp3F,_tmp3F + 23};static struct _tuple1 Cyc_Toc__get_zero_arr_size_int_pr={{.Loc_n={
3,0}},& Cyc_Toc__get_zero_arr_size_int_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__get_zero_arr_size_int_re={
1,& Cyc_Toc__get_zero_arr_size_int_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp
Cyc_Toc__get_zero_arr_size_int_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_int_re,0,(
void*)Cyc_Absyn_EmptyAnnot};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_int_e=&
Cyc_Toc__get_zero_arr_size_int_ev;static char _tmp41[25]="_get_zero_arr_size_float";
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_float_str={_tmp41,_tmp41,
_tmp41 + 25};static struct _tuple1 Cyc_Toc__get_zero_arr_size_float_pr={{.Loc_n={3,0}},&
Cyc_Toc__get_zero_arr_size_float_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__get_zero_arr_size_float_re={
1,& Cyc_Toc__get_zero_arr_size_float_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp
Cyc_Toc__get_zero_arr_size_float_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_float_re,
0,(void*)Cyc_Absyn_EmptyAnnot};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_float_e=&
Cyc_Toc__get_zero_arr_size_float_ev;static char _tmp43[26]="_get_zero_arr_size_double";
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_double_str={_tmp43,_tmp43,
_tmp43 + 26};static struct _tuple1 Cyc_Toc__get_zero_arr_size_double_pr={{.Loc_n={3,
0}},& Cyc_Toc__get_zero_arr_size_double_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__get_zero_arr_size_double_re={1,& Cyc_Toc__get_zero_arr_size_double_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_double_ev={
0,(void*)& Cyc_Toc__get_zero_arr_size_double_re,0,(void*)Cyc_Absyn_EmptyAnnot};
static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_double_e=& Cyc_Toc__get_zero_arr_size_double_ev;
static char _tmp45[30]="_get_zero_arr_size_longdouble";static struct _dyneither_ptr
Cyc_Toc__get_zero_arr_size_longdouble_str={_tmp45,_tmp45,_tmp45 + 30};static
struct _tuple1 Cyc_Toc__get_zero_arr_size_longdouble_pr={{.Loc_n={3,0}},& Cyc_Toc__get_zero_arr_size_longdouble_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__get_zero_arr_size_longdouble_re={1,&
Cyc_Toc__get_zero_arr_size_longdouble_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp
Cyc_Toc__get_zero_arr_size_longdouble_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_longdouble_re,
0,(void*)Cyc_Absyn_EmptyAnnot};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_longdouble_e=&
Cyc_Toc__get_zero_arr_size_longdouble_ev;static char _tmp47[28]="_get_zero_arr_size_voidstar";
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_voidstar_str={_tmp47,_tmp47,
_tmp47 + 28};static struct _tuple1 Cyc_Toc__get_zero_arr_size_voidstar_pr={{.Loc_n={
3,0}},& Cyc_Toc__get_zero_arr_size_voidstar_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__get_zero_arr_size_voidstar_re={1,& Cyc_Toc__get_zero_arr_size_voidstar_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_voidstar_ev={
0,(void*)& Cyc_Toc__get_zero_arr_size_voidstar_re,0,(void*)Cyc_Absyn_EmptyAnnot};
static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_voidstar_e=& Cyc_Toc__get_zero_arr_size_voidstar_ev;
static char _tmp49[20]="_dyneither_ptr_plus";static struct _dyneither_ptr Cyc_Toc__dyneither_ptr_plus_str={
_tmp49,_tmp49,_tmp49 + 20};static struct _tuple1 Cyc_Toc__dyneither_ptr_plus_pr={{.Loc_n={
3,0}},& Cyc_Toc__dyneither_ptr_plus_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__dyneither_ptr_plus_re={
1,& Cyc_Toc__dyneither_ptr_plus_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp
Cyc_Toc__dyneither_ptr_plus_ev={0,(void*)& Cyc_Toc__dyneither_ptr_plus_re,0,(void*)
Cyc_Absyn_EmptyAnnot};static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_plus_e=&
Cyc_Toc__dyneither_ptr_plus_ev;static char _tmp4B[15]="_zero_arr_plus";static
struct _dyneither_ptr Cyc_Toc__zero_arr_plus_str={_tmp4B,_tmp4B,_tmp4B + 15};static
struct _tuple1 Cyc_Toc__zero_arr_plus_pr={{.Loc_n={3,0}},& Cyc_Toc__zero_arr_plus_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_plus_re={1,& Cyc_Toc__zero_arr_plus_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_ev={0,(void*)&
Cyc_Toc__zero_arr_plus_re,0,(void*)Cyc_Absyn_EmptyAnnot};static struct Cyc_Absyn_Exp*
Cyc_Toc__zero_arr_plus_e=& Cyc_Toc__zero_arr_plus_ev;static char _tmp4D[20]="_zero_arr_plus_char";
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_char_str={_tmp4D,_tmp4D,_tmp4D + 
20};static struct _tuple1 Cyc_Toc__zero_arr_plus_char_pr={{.Loc_n={3,0}},& Cyc_Toc__zero_arr_plus_char_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_plus_char_re={1,& Cyc_Toc__zero_arr_plus_char_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_char_ev={0,(
void*)& Cyc_Toc__zero_arr_plus_char_re,0,(void*)Cyc_Absyn_EmptyAnnot};static
struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_char_e=& Cyc_Toc__zero_arr_plus_char_ev;
static char _tmp4F[21]="_zero_arr_plus_short";static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_short_str={
_tmp4F,_tmp4F,_tmp4F + 21};static struct _tuple1 Cyc_Toc__zero_arr_plus_short_pr={{.Loc_n={
3,0}},& Cyc_Toc__zero_arr_plus_short_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_plus_short_re={
1,& Cyc_Toc__zero_arr_plus_short_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp
Cyc_Toc__zero_arr_plus_short_ev={0,(void*)& Cyc_Toc__zero_arr_plus_short_re,0,(
void*)Cyc_Absyn_EmptyAnnot};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_short_e=&
Cyc_Toc__zero_arr_plus_short_ev;static char _tmp51[19]="_zero_arr_plus_int";static
struct _dyneither_ptr Cyc_Toc__zero_arr_plus_int_str={_tmp51,_tmp51,_tmp51 + 19};
static struct _tuple1 Cyc_Toc__zero_arr_plus_int_pr={{.Loc_n={3,0}},& Cyc_Toc__zero_arr_plus_int_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_plus_int_re={1,& Cyc_Toc__zero_arr_plus_int_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_int_ev={0,(
void*)& Cyc_Toc__zero_arr_plus_int_re,0,(void*)Cyc_Absyn_EmptyAnnot};static struct
Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_int_e=& Cyc_Toc__zero_arr_plus_int_ev;static
char _tmp53[21]="_zero_arr_plus_float";static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_float_str={
_tmp53,_tmp53,_tmp53 + 21};static struct _tuple1 Cyc_Toc__zero_arr_plus_float_pr={{.Loc_n={
3,0}},& Cyc_Toc__zero_arr_plus_float_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_plus_float_re={
1,& Cyc_Toc__zero_arr_plus_float_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp
Cyc_Toc__zero_arr_plus_float_ev={0,(void*)& Cyc_Toc__zero_arr_plus_float_re,0,(
void*)Cyc_Absyn_EmptyAnnot};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_float_e=&
Cyc_Toc__zero_arr_plus_float_ev;static char _tmp55[22]="_zero_arr_plus_double";
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_double_str={_tmp55,_tmp55,
_tmp55 + 22};static struct _tuple1 Cyc_Toc__zero_arr_plus_double_pr={{.Loc_n={3,0}},&
Cyc_Toc__zero_arr_plus_double_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_plus_double_re={
1,& Cyc_Toc__zero_arr_plus_double_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp
Cyc_Toc__zero_arr_plus_double_ev={0,(void*)& Cyc_Toc__zero_arr_plus_double_re,0,(
void*)Cyc_Absyn_EmptyAnnot};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_double_e=&
Cyc_Toc__zero_arr_plus_double_ev;static char _tmp57[26]="_zero_arr_plus_longdouble";
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_longdouble_str={_tmp57,_tmp57,
_tmp57 + 26};static struct _tuple1 Cyc_Toc__zero_arr_plus_longdouble_pr={{.Loc_n={3,
0}},& Cyc_Toc__zero_arr_plus_longdouble_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__zero_arr_plus_longdouble_re={1,& Cyc_Toc__zero_arr_plus_longdouble_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_longdouble_ev={
0,(void*)& Cyc_Toc__zero_arr_plus_longdouble_re,0,(void*)Cyc_Absyn_EmptyAnnot};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_longdouble_e=& Cyc_Toc__zero_arr_plus_longdouble_ev;
static char _tmp59[24]="_zero_arr_plus_voidstar";static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_voidstar_str={
_tmp59,_tmp59,_tmp59 + 24};static struct _tuple1 Cyc_Toc__zero_arr_plus_voidstar_pr={{.Loc_n={
3,0}},& Cyc_Toc__zero_arr_plus_voidstar_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__zero_arr_plus_voidstar_re={1,& Cyc_Toc__zero_arr_plus_voidstar_pr,(void*)((
void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_voidstar_ev={0,(void*)&
Cyc_Toc__zero_arr_plus_voidstar_re,0,(void*)Cyc_Absyn_EmptyAnnot};static struct
Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_voidstar_e=& Cyc_Toc__zero_arr_plus_voidstar_ev;
static char _tmp5B[28]="_dyneither_ptr_inplace_plus";static struct _dyneither_ptr Cyc_Toc__dyneither_ptr_inplace_plus_str={
_tmp5B,_tmp5B,_tmp5B + 28};static struct _tuple1 Cyc_Toc__dyneither_ptr_inplace_plus_pr={{.Loc_n={
3,0}},& Cyc_Toc__dyneither_ptr_inplace_plus_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__dyneither_ptr_inplace_plus_re={1,& Cyc_Toc__dyneither_ptr_inplace_plus_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_inplace_plus_ev={
0,(void*)& Cyc_Toc__dyneither_ptr_inplace_plus_re,0,(void*)Cyc_Absyn_EmptyAnnot};
static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_inplace_plus_e=& Cyc_Toc__dyneither_ptr_inplace_plus_ev;
static char _tmp5D[23]="_zero_arr_inplace_plus";static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_str={
_tmp5D,_tmp5D,_tmp5D + 23};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_pr={{.Loc_n={
3,0}},& Cyc_Toc__zero_arr_inplace_plus_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_inplace_plus_re={
1,& Cyc_Toc__zero_arr_inplace_plus_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp
Cyc_Toc__zero_arr_inplace_plus_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_re,0,(
void*)Cyc_Absyn_EmptyAnnot};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_e=&
Cyc_Toc__zero_arr_inplace_plus_ev;static char _tmp5F[28]="_zero_arr_inplace_plus_char";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_char_str={_tmp5F,_tmp5F,
_tmp5F + 28};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_char_pr={{.Loc_n={
3,0}},& Cyc_Toc__zero_arr_inplace_plus_char_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__zero_arr_inplace_plus_char_re={1,& Cyc_Toc__zero_arr_inplace_plus_char_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_char_ev={
0,(void*)& Cyc_Toc__zero_arr_inplace_plus_char_re,0,(void*)Cyc_Absyn_EmptyAnnot};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_char_e=& Cyc_Toc__zero_arr_inplace_plus_char_ev;
static char _tmp61[29]="_zero_arr_inplace_plus_short";static struct _dyneither_ptr
Cyc_Toc__zero_arr_inplace_plus_short_str={_tmp61,_tmp61,_tmp61 + 29};static struct
_tuple1 Cyc_Toc__zero_arr_inplace_plus_short_pr={{.Loc_n={3,0}},& Cyc_Toc__zero_arr_inplace_plus_short_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_inplace_plus_short_re={1,&
Cyc_Toc__zero_arr_inplace_plus_short_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp
Cyc_Toc__zero_arr_inplace_plus_short_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_short_re,
0,(void*)Cyc_Absyn_EmptyAnnot};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_short_e=&
Cyc_Toc__zero_arr_inplace_plus_short_ev;static char _tmp63[27]="_zero_arr_inplace_plus_int";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_int_str={_tmp63,_tmp63,
_tmp63 + 27};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_int_pr={{.Loc_n={3,
0}},& Cyc_Toc__zero_arr_inplace_plus_int_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__zero_arr_inplace_plus_int_re={1,& Cyc_Toc__zero_arr_inplace_plus_int_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_int_ev={
0,(void*)& Cyc_Toc__zero_arr_inplace_plus_int_re,0,(void*)Cyc_Absyn_EmptyAnnot};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_int_e=& Cyc_Toc__zero_arr_inplace_plus_int_ev;
static char _tmp65[29]="_zero_arr_inplace_plus_float";static struct _dyneither_ptr
Cyc_Toc__zero_arr_inplace_plus_float_str={_tmp65,_tmp65,_tmp65 + 29};static struct
_tuple1 Cyc_Toc__zero_arr_inplace_plus_float_pr={{.Loc_n={3,0}},& Cyc_Toc__zero_arr_inplace_plus_float_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_inplace_plus_float_re={1,&
Cyc_Toc__zero_arr_inplace_plus_float_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp
Cyc_Toc__zero_arr_inplace_plus_float_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_float_re,
0,(void*)Cyc_Absyn_EmptyAnnot};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_float_e=&
Cyc_Toc__zero_arr_inplace_plus_float_ev;static char _tmp67[30]="_zero_arr_inplace_plus_double";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_double_str={_tmp67,
_tmp67,_tmp67 + 30};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_double_pr={{.Loc_n={
3,0}},& Cyc_Toc__zero_arr_inplace_plus_double_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__zero_arr_inplace_plus_double_re={1,& Cyc_Toc__zero_arr_inplace_plus_double_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_double_ev={
0,(void*)& Cyc_Toc__zero_arr_inplace_plus_double_re,0,(void*)Cyc_Absyn_EmptyAnnot};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_double_e=& Cyc_Toc__zero_arr_inplace_plus_double_ev;
static char _tmp69[34]="_zero_arr_inplace_plus_longdouble";static struct
_dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_longdouble_str={_tmp69,_tmp69,
_tmp69 + 34};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_longdouble_pr={{.Loc_n={
3,0}},& Cyc_Toc__zero_arr_inplace_plus_longdouble_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__zero_arr_inplace_plus_longdouble_re={1,& Cyc_Toc__zero_arr_inplace_plus_longdouble_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_longdouble_ev={
0,(void*)& Cyc_Toc__zero_arr_inplace_plus_longdouble_re,0,(void*)Cyc_Absyn_EmptyAnnot};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_longdouble_e=& Cyc_Toc__zero_arr_inplace_plus_longdouble_ev;
static char _tmp6B[32]="_zero_arr_inplace_plus_voidstar";static struct
_dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_voidstar_str={_tmp6B,_tmp6B,_tmp6B + 
32};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_voidstar_pr={{.Loc_n={3,0}},&
Cyc_Toc__zero_arr_inplace_plus_voidstar_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__zero_arr_inplace_plus_voidstar_re={1,& Cyc_Toc__zero_arr_inplace_plus_voidstar_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_voidstar_ev={
0,(void*)& Cyc_Toc__zero_arr_inplace_plus_voidstar_re,0,(void*)Cyc_Absyn_EmptyAnnot};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_voidstar_e=& Cyc_Toc__zero_arr_inplace_plus_voidstar_ev;
static char _tmp6D[33]="_dyneither_ptr_inplace_plus_post";static struct
_dyneither_ptr Cyc_Toc__dyneither_ptr_inplace_plus_post_str={_tmp6D,_tmp6D,_tmp6D
+ 33};static struct _tuple1 Cyc_Toc__dyneither_ptr_inplace_plus_post_pr={{.Loc_n={3,
0}},& Cyc_Toc__dyneither_ptr_inplace_plus_post_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__dyneither_ptr_inplace_plus_post_re={1,& Cyc_Toc__dyneither_ptr_inplace_plus_post_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_inplace_plus_post_ev={
0,(void*)& Cyc_Toc__dyneither_ptr_inplace_plus_post_re,0,(void*)Cyc_Absyn_EmptyAnnot};
static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_inplace_plus_post_e=& Cyc_Toc__dyneither_ptr_inplace_plus_post_ev;
static char _tmp6F[28]="_zero_arr_inplace_plus_post";static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_str={
_tmp6F,_tmp6F,_tmp6F + 28};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_pr={{.Loc_n={
3,0}},& Cyc_Toc__zero_arr_inplace_plus_post_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__zero_arr_inplace_plus_post_re={1,& Cyc_Toc__zero_arr_inplace_plus_post_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_ev={
0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_re,0,(void*)Cyc_Absyn_EmptyAnnot};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_e=& Cyc_Toc__zero_arr_inplace_plus_post_ev;
static char _tmp71[33]="_zero_arr_inplace_plus_post_char";static struct
_dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_char_str={_tmp71,_tmp71,_tmp71
+ 33};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_char_pr={{.Loc_n={3,
0}},& Cyc_Toc__zero_arr_inplace_plus_post_char_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__zero_arr_inplace_plus_post_char_re={1,& Cyc_Toc__zero_arr_inplace_plus_post_char_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_char_ev={
0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_char_re,0,(void*)Cyc_Absyn_EmptyAnnot};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_char_e=& Cyc_Toc__zero_arr_inplace_plus_post_char_ev;
static char _tmp73[34]="_zero_arr_inplace_plus_post_short";static struct
_dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_short_str={_tmp73,_tmp73,
_tmp73 + 34};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_short_pr={{.Loc_n={
3,0}},& Cyc_Toc__zero_arr_inplace_plus_post_short_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__zero_arr_inplace_plus_post_short_re={1,& Cyc_Toc__zero_arr_inplace_plus_post_short_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_short_ev={
0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_short_re,0,(void*)Cyc_Absyn_EmptyAnnot};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_short_e=& Cyc_Toc__zero_arr_inplace_plus_post_short_ev;
static char _tmp75[32]="_zero_arr_inplace_plus_post_int";static struct
_dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_int_str={_tmp75,_tmp75,_tmp75 + 
32};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_int_pr={{.Loc_n={3,0}},&
Cyc_Toc__zero_arr_inplace_plus_post_int_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__zero_arr_inplace_plus_post_int_re={1,& Cyc_Toc__zero_arr_inplace_plus_post_int_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_int_ev={
0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_int_re,0,(void*)Cyc_Absyn_EmptyAnnot};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_int_e=& Cyc_Toc__zero_arr_inplace_plus_post_int_ev;
static char _tmp77[34]="_zero_arr_inplace_plus_post_float";static struct
_dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_float_str={_tmp77,_tmp77,
_tmp77 + 34};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_float_pr={{.Loc_n={
3,0}},& Cyc_Toc__zero_arr_inplace_plus_post_float_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__zero_arr_inplace_plus_post_float_re={1,& Cyc_Toc__zero_arr_inplace_plus_post_float_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_float_ev={
0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_float_re,0,(void*)Cyc_Absyn_EmptyAnnot};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_float_e=& Cyc_Toc__zero_arr_inplace_plus_post_float_ev;
static char _tmp79[35]="_zero_arr_inplace_plus_post_double";static struct
_dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_double_str={_tmp79,_tmp79,
_tmp79 + 35};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_double_pr={{.Loc_n={
3,0}},& Cyc_Toc__zero_arr_inplace_plus_post_double_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__zero_arr_inplace_plus_post_double_re={1,& Cyc_Toc__zero_arr_inplace_plus_post_double_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_double_ev={
0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_double_re,0,(void*)Cyc_Absyn_EmptyAnnot};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_double_e=& Cyc_Toc__zero_arr_inplace_plus_post_double_ev;
static char _tmp7B[39]="_zero_arr_inplace_plus_post_longdouble";static struct
_dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_longdouble_str={_tmp7B,_tmp7B,
_tmp7B + 39};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_longdouble_pr={{.Loc_n={
3,0}},& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__zero_arr_inplace_plus_post_longdouble_re={1,& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_longdouble_ev={
0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_re,0,(void*)Cyc_Absyn_EmptyAnnot};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_longdouble_e=& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_ev;
static char _tmp7D[37]="_zero_arr_inplace_plus_post_voidstar";static struct
_dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_voidstar_str={_tmp7D,_tmp7D,
_tmp7D + 37};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_voidstar_pr={{.Loc_n={
3,0}},& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__zero_arr_inplace_plus_post_voidstar_re={1,& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_voidstar_ev={
0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_re,0,(void*)Cyc_Absyn_EmptyAnnot};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_voidstar_e=& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_ev;
static char _tmp7F[10]="_cycalloc";static struct _dyneither_ptr Cyc_Toc__cycalloc_str={
_tmp7F,_tmp7F,_tmp7F + 10};static struct _tuple1 Cyc_Toc__cycalloc_pr={{.Loc_n={3,0}},&
Cyc_Toc__cycalloc_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__cycalloc_re={1,&
Cyc_Toc__cycalloc_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__cycalloc_ev={
0,(void*)& Cyc_Toc__cycalloc_re,0,(void*)Cyc_Absyn_EmptyAnnot};static struct Cyc_Absyn_Exp*
Cyc_Toc__cycalloc_e=& Cyc_Toc__cycalloc_ev;static char _tmp81[11]="_cyccalloc";
static struct _dyneither_ptr Cyc_Toc__cyccalloc_str={_tmp81,_tmp81,_tmp81 + 11};
static struct _tuple1 Cyc_Toc__cyccalloc_pr={{.Loc_n={3,0}},& Cyc_Toc__cyccalloc_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__cyccalloc_re={1,& Cyc_Toc__cyccalloc_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__cyccalloc_ev={0,(void*)& Cyc_Toc__cyccalloc_re,
0,(void*)Cyc_Absyn_EmptyAnnot};static struct Cyc_Absyn_Exp*Cyc_Toc__cyccalloc_e=&
Cyc_Toc__cyccalloc_ev;static char _tmp83[17]="_cycalloc_atomic";static struct
_dyneither_ptr Cyc_Toc__cycalloc_atomic_str={_tmp83,_tmp83,_tmp83 + 17};static
struct _tuple1 Cyc_Toc__cycalloc_atomic_pr={{.Loc_n={3,0}},& Cyc_Toc__cycalloc_atomic_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__cycalloc_atomic_re={1,& Cyc_Toc__cycalloc_atomic_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__cycalloc_atomic_ev={0,(void*)&
Cyc_Toc__cycalloc_atomic_re,0,(void*)Cyc_Absyn_EmptyAnnot};static struct Cyc_Absyn_Exp*
Cyc_Toc__cycalloc_atomic_e=& Cyc_Toc__cycalloc_atomic_ev;static char _tmp85[18]="_cyccalloc_atomic";
static struct _dyneither_ptr Cyc_Toc__cyccalloc_atomic_str={_tmp85,_tmp85,_tmp85 + 
18};static struct _tuple1 Cyc_Toc__cyccalloc_atomic_pr={{.Loc_n={3,0}},& Cyc_Toc__cyccalloc_atomic_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__cyccalloc_atomic_re={1,& Cyc_Toc__cyccalloc_atomic_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__cyccalloc_atomic_ev={0,(void*)&
Cyc_Toc__cyccalloc_atomic_re,0,(void*)Cyc_Absyn_EmptyAnnot};static struct Cyc_Absyn_Exp*
Cyc_Toc__cyccalloc_atomic_e=& Cyc_Toc__cyccalloc_atomic_ev;static char _tmp87[15]="_region_malloc";
static struct _dyneither_ptr Cyc_Toc__region_malloc_str={_tmp87,_tmp87,_tmp87 + 15};
static struct _tuple1 Cyc_Toc__region_malloc_pr={{.Loc_n={3,0}},& Cyc_Toc__region_malloc_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__region_malloc_re={1,& Cyc_Toc__region_malloc_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__region_malloc_ev={0,(void*)&
Cyc_Toc__region_malloc_re,0,(void*)Cyc_Absyn_EmptyAnnot};static struct Cyc_Absyn_Exp*
Cyc_Toc__region_malloc_e=& Cyc_Toc__region_malloc_ev;static char _tmp89[15]="_region_calloc";
static struct _dyneither_ptr Cyc_Toc__region_calloc_str={_tmp89,_tmp89,_tmp89 + 15};
static struct _tuple1 Cyc_Toc__region_calloc_pr={{.Loc_n={3,0}},& Cyc_Toc__region_calloc_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__region_calloc_re={1,& Cyc_Toc__region_calloc_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__region_calloc_ev={0,(void*)&
Cyc_Toc__region_calloc_re,0,(void*)Cyc_Absyn_EmptyAnnot};static struct Cyc_Absyn_Exp*
Cyc_Toc__region_calloc_e=& Cyc_Toc__region_calloc_ev;static char _tmp8B[13]="_check_times";
static struct _dyneither_ptr Cyc_Toc__check_times_str={_tmp8B,_tmp8B,_tmp8B + 13};
static struct _tuple1 Cyc_Toc__check_times_pr={{.Loc_n={3,0}},& Cyc_Toc__check_times_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__check_times_re={1,& Cyc_Toc__check_times_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__check_times_ev={0,(void*)&
Cyc_Toc__check_times_re,0,(void*)Cyc_Absyn_EmptyAnnot};static struct Cyc_Absyn_Exp*
Cyc_Toc__check_times_e=& Cyc_Toc__check_times_ev;static char _tmp8D[12]="_new_region";
static struct _dyneither_ptr Cyc_Toc__new_region_str={_tmp8D,_tmp8D,_tmp8D + 12};
static struct _tuple1 Cyc_Toc__new_region_pr={{.Loc_n={3,0}},& Cyc_Toc__new_region_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__new_region_re={1,& Cyc_Toc__new_region_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__new_region_ev={0,(void*)& Cyc_Toc__new_region_re,
0,(void*)Cyc_Absyn_EmptyAnnot};static struct Cyc_Absyn_Exp*Cyc_Toc__new_region_e=&
Cyc_Toc__new_region_ev;static char _tmp8F[13]="_push_region";static struct
_dyneither_ptr Cyc_Toc__push_region_str={_tmp8F,_tmp8F,_tmp8F + 13};static struct
_tuple1 Cyc_Toc__push_region_pr={{.Loc_n={3,0}},& Cyc_Toc__push_region_str};static
struct Cyc_Absyn_Var_e_struct Cyc_Toc__push_region_re={1,& Cyc_Toc__push_region_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__push_region_ev={0,(void*)&
Cyc_Toc__push_region_re,0,(void*)Cyc_Absyn_EmptyAnnot};static struct Cyc_Absyn_Exp*
Cyc_Toc__push_region_e=& Cyc_Toc__push_region_ev;static char _tmp91[12]="_pop_region";
static struct _dyneither_ptr Cyc_Toc__pop_region_str={_tmp91,_tmp91,_tmp91 + 12};
static struct _tuple1 Cyc_Toc__pop_region_pr={{.Loc_n={3,0}},& Cyc_Toc__pop_region_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__pop_region_re={1,& Cyc_Toc__pop_region_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__pop_region_ev={0,(void*)& Cyc_Toc__pop_region_re,
0,(void*)Cyc_Absyn_EmptyAnnot};static struct Cyc_Absyn_Exp*Cyc_Toc__pop_region_e=&
Cyc_Toc__pop_region_ev;static char _tmp93[16]="_open_dynregion";static struct
_dyneither_ptr Cyc_Toc__open_dynregion_str={_tmp93,_tmp93,_tmp93 + 16};static
struct _tuple1 Cyc_Toc__open_dynregion_pr={{.Loc_n={3,0}},& Cyc_Toc__open_dynregion_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__open_dynregion_re={1,& Cyc_Toc__open_dynregion_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__open_dynregion_ev={0,(void*)&
Cyc_Toc__open_dynregion_re,0,(void*)Cyc_Absyn_EmptyAnnot};static struct Cyc_Absyn_Exp*
Cyc_Toc__open_dynregion_e=& Cyc_Toc__open_dynregion_ev;static char _tmp95[16]="_push_dynregion";
static struct _dyneither_ptr Cyc_Toc__push_dynregion_str={_tmp95,_tmp95,_tmp95 + 16};
static struct _tuple1 Cyc_Toc__push_dynregion_pr={{.Loc_n={3,0}},& Cyc_Toc__push_dynregion_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__push_dynregion_re={1,& Cyc_Toc__push_dynregion_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__push_dynregion_ev={0,(void*)&
Cyc_Toc__push_dynregion_re,0,(void*)Cyc_Absyn_EmptyAnnot};static struct Cyc_Absyn_Exp*
Cyc_Toc__push_dynregion_e=& Cyc_Toc__push_dynregion_ev;static char _tmp97[15]="_pop_dynregion";
static struct _dyneither_ptr Cyc_Toc__pop_dynregion_str={_tmp97,_tmp97,_tmp97 + 15};
static struct _tuple1 Cyc_Toc__pop_dynregion_pr={{.Loc_n={3,0}},& Cyc_Toc__pop_dynregion_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__pop_dynregion_re={1,& Cyc_Toc__pop_dynregion_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__pop_dynregion_ev={0,(void*)&
Cyc_Toc__pop_dynregion_re,0,(void*)Cyc_Absyn_EmptyAnnot};static struct Cyc_Absyn_Exp*
Cyc_Toc__pop_dynregion_e=& Cyc_Toc__pop_dynregion_ev;static char _tmp99[14]="_reset_region";
static struct _dyneither_ptr Cyc_Toc__reset_region_str={_tmp99,_tmp99,_tmp99 + 14};
static struct _tuple1 Cyc_Toc__reset_region_pr={{.Loc_n={3,0}},& Cyc_Toc__reset_region_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__reset_region_re={1,& Cyc_Toc__reset_region_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__reset_region_ev={0,(void*)&
Cyc_Toc__reset_region_re,0,(void*)Cyc_Absyn_EmptyAnnot};static struct Cyc_Absyn_Exp*
Cyc_Toc__reset_region_e=& Cyc_Toc__reset_region_ev;static char _tmp9B[19]="_throw_arraybounds";
static struct _dyneither_ptr Cyc_Toc__throw_arraybounds_str={_tmp9B,_tmp9B,_tmp9B + 
19};static struct _tuple1 Cyc_Toc__throw_arraybounds_pr={{.Loc_n={3,0}},& Cyc_Toc__throw_arraybounds_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__throw_arraybounds_re={1,& Cyc_Toc__throw_arraybounds_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__throw_arraybounds_ev={0,(
void*)& Cyc_Toc__throw_arraybounds_re,0,(void*)Cyc_Absyn_EmptyAnnot};static struct
Cyc_Absyn_Exp*Cyc_Toc__throw_arraybounds_e=& Cyc_Toc__throw_arraybounds_ev;static
char _tmp9D[29]="_dyneither_ptr_decrease_size";static struct _dyneither_ptr Cyc_Toc__dyneither_ptr_decrease_size_str={
_tmp9D,_tmp9D,_tmp9D + 29};static struct _tuple1 Cyc_Toc__dyneither_ptr_decrease_size_pr={{.Loc_n={
3,0}},& Cyc_Toc__dyneither_ptr_decrease_size_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__dyneither_ptr_decrease_size_re={1,& Cyc_Toc__dyneither_ptr_decrease_size_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_decrease_size_ev={
0,(void*)& Cyc_Toc__dyneither_ptr_decrease_size_re,0,(void*)Cyc_Absyn_EmptyAnnot};
static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_decrease_size_e=& Cyc_Toc__dyneither_ptr_decrease_size_ev;
static char _tmp9F[11]="_swap_word";static struct _dyneither_ptr Cyc_Toc__swap_word_str={
_tmp9F,_tmp9F,_tmp9F + 11};static struct _tuple1 Cyc_Toc__swap_word_pr={{.Loc_n={3,0}},&
Cyc_Toc__swap_word_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__swap_word_re={
1,& Cyc_Toc__swap_word_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__swap_word_ev={
0,(void*)& Cyc_Toc__swap_word_re,0,(void*)Cyc_Absyn_EmptyAnnot};static struct Cyc_Absyn_Exp*
Cyc_Toc__swap_word_e=& Cyc_Toc__swap_word_ev;static char _tmpA1[16]="_swap_dyneither";
static struct _dyneither_ptr Cyc_Toc__swap_dyneither_str={_tmpA1,_tmpA1,_tmpA1 + 16};
static struct _tuple1 Cyc_Toc__swap_dyneither_pr={{.Loc_n={3,0}},& Cyc_Toc__swap_dyneither_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__swap_dyneither_re={1,& Cyc_Toc__swap_dyneither_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__swap_dyneither_ev={0,(void*)&
Cyc_Toc__swap_dyneither_re,0,(void*)Cyc_Absyn_EmptyAnnot};static struct Cyc_Absyn_Exp*
Cyc_Toc__swap_dyneither_e=& Cyc_Toc__swap_dyneither_ev;static struct Cyc_Absyn_AggrType_struct
Cyc_Toc_dyneither_ptr_typ_v={10,{{.UnknownAggr={1,{(void*)0,& Cyc_Toc__dyneither_ptr_pr,
0}}},0}};static void*Cyc_Toc_dyneither_ptr_typ=(void*)& Cyc_Toc_dyneither_ptr_typ_v;
static struct Cyc_Absyn_Tqual Cyc_Toc_mt_tq={0,0,0,0,0};static struct Cyc_Absyn_Stmt*
Cyc_Toc_skip_stmt_dl();static struct Cyc_Absyn_Stmt*Cyc_Toc_skip_stmt_dl(){static
struct Cyc_Absyn_Stmt**skip_stmt_opt=0;if(skip_stmt_opt == 0){struct Cyc_Absyn_Stmt**
_tmp8A5;skip_stmt_opt=((_tmp8A5=_cycalloc(sizeof(*_tmp8A5)),((_tmp8A5[0]=Cyc_Absyn_skip_stmt(
0),_tmp8A5))));}return*skip_stmt_opt;}static struct Cyc_Absyn_Exp*Cyc_Toc_cast_it(
void*t,struct Cyc_Absyn_Exp*e);static struct Cyc_Absyn_Exp*Cyc_Toc_cast_it(void*t,
struct Cyc_Absyn_Exp*e){return Cyc_Absyn_cast_exp(t,e,0,(void*)1,0);}static void*
Cyc_Toc_cast_it_r(void*t,struct Cyc_Absyn_Exp*e);static void*Cyc_Toc_cast_it_r(
void*t,struct Cyc_Absyn_Exp*e){struct Cyc_Absyn_Cast_e_struct _tmp8A8;struct Cyc_Absyn_Cast_e_struct*
_tmp8A7;return(void*)((_tmp8A7=_cycalloc(sizeof(*_tmp8A7)),((_tmp8A7[0]=((
_tmp8A8.tag=15,((_tmp8A8.f1=(void*)t,((_tmp8A8.f2=e,((_tmp8A8.f3=0,((_tmp8A8.f4=(
void*)((void*)1),_tmp8A8)))))))))),_tmp8A7))));}static void*Cyc_Toc_deref_exp_r(
struct Cyc_Absyn_Exp*e);static void*Cyc_Toc_deref_exp_r(struct Cyc_Absyn_Exp*e){
struct Cyc_Absyn_Deref_e_struct _tmp8AB;struct Cyc_Absyn_Deref_e_struct*_tmp8AA;
return(void*)((_tmp8AA=_cycalloc(sizeof(*_tmp8AA)),((_tmp8AA[0]=((_tmp8AB.tag=22,((
_tmp8AB.f1=e,_tmp8AB)))),_tmp8AA))));}static void*Cyc_Toc_subscript_exp_r(struct
Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);static void*Cyc_Toc_subscript_exp_r(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){struct Cyc_Absyn_Subscript_e_struct
_tmp8AE;struct Cyc_Absyn_Subscript_e_struct*_tmp8AD;return(void*)((_tmp8AD=
_cycalloc(sizeof(*_tmp8AD)),((_tmp8AD[0]=((_tmp8AE.tag=25,((_tmp8AE.f1=e1,((
_tmp8AE.f2=e2,_tmp8AE)))))),_tmp8AD))));}static void*Cyc_Toc_stmt_exp_r(struct Cyc_Absyn_Stmt*
s);static void*Cyc_Toc_stmt_exp_r(struct Cyc_Absyn_Stmt*s){struct Cyc_Absyn_StmtExp_e_struct
_tmp8B1;struct Cyc_Absyn_StmtExp_e_struct*_tmp8B0;return(void*)((_tmp8B0=
_cycalloc(sizeof(*_tmp8B0)),((_tmp8B0[0]=((_tmp8B1.tag=38,((_tmp8B1.f1=s,_tmp8B1)))),
_tmp8B0))));}static void*Cyc_Toc_sizeoftyp_exp_r(void*t);static void*Cyc_Toc_sizeoftyp_exp_r(
void*t){struct Cyc_Absyn_Sizeoftyp_e_struct _tmp8B4;struct Cyc_Absyn_Sizeoftyp_e_struct*
_tmp8B3;return(void*)((_tmp8B3=_cycalloc(sizeof(*_tmp8B3)),((_tmp8B3[0]=((
_tmp8B4.tag=18,((_tmp8B4.f1=(void*)t,_tmp8B4)))),_tmp8B3))));}static void*Cyc_Toc_fncall_exp_r(
struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es);static void*Cyc_Toc_fncall_exp_r(
struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es){struct Cyc_Absyn_FnCall_e_struct
_tmp8B7;struct Cyc_Absyn_FnCall_e_struct*_tmp8B6;return(void*)((_tmp8B6=_cycalloc(
sizeof(*_tmp8B6)),((_tmp8B6[0]=((_tmp8B7.tag=11,((_tmp8B7.f1=e,((_tmp8B7.f2=es,((
_tmp8B7.f3=0,_tmp8B7)))))))),_tmp8B6))));}static void*Cyc_Toc_exp_stmt_r(struct
Cyc_Absyn_Exp*e);static void*Cyc_Toc_exp_stmt_r(struct Cyc_Absyn_Exp*e){struct Cyc_Absyn_Exp_s_struct
_tmp8BA;struct Cyc_Absyn_Exp_s_struct*_tmp8B9;return(void*)((_tmp8B9=_cycalloc(
sizeof(*_tmp8B9)),((_tmp8B9[0]=((_tmp8BA.tag=0,((_tmp8BA.f1=e,_tmp8BA)))),
_tmp8B9))));}static void*Cyc_Toc_seq_stmt_r(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*
s2);static void*Cyc_Toc_seq_stmt_r(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*
s2){struct Cyc_Absyn_Seq_s_struct _tmp8BD;struct Cyc_Absyn_Seq_s_struct*_tmp8BC;
return(void*)((_tmp8BC=_cycalloc(sizeof(*_tmp8BC)),((_tmp8BC[0]=((_tmp8BD.tag=1,((
_tmp8BD.f1=s1,((_tmp8BD.f2=s2,_tmp8BD)))))),_tmp8BC))));}static void*Cyc_Toc_conditional_exp_r(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3);static void*
Cyc_Toc_conditional_exp_r(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct
Cyc_Absyn_Exp*e3){struct Cyc_Absyn_Conditional_e_struct _tmp8C0;struct Cyc_Absyn_Conditional_e_struct*
_tmp8BF;return(void*)((_tmp8BF=_cycalloc(sizeof(*_tmp8BF)),((_tmp8BF[0]=((
_tmp8C0.tag=6,((_tmp8C0.f1=e1,((_tmp8C0.f2=e2,((_tmp8C0.f3=e3,_tmp8C0)))))))),
_tmp8BF))));}static void*Cyc_Toc_aggrmember_exp_r(struct Cyc_Absyn_Exp*e,struct
_dyneither_ptr*n);static void*Cyc_Toc_aggrmember_exp_r(struct Cyc_Absyn_Exp*e,
struct _dyneither_ptr*n){struct Cyc_Absyn_AggrMember_e_struct _tmp8C3;struct Cyc_Absyn_AggrMember_e_struct*
_tmp8C2;return(void*)((_tmp8C2=_cycalloc(sizeof(*_tmp8C2)),((_tmp8C2[0]=((
_tmp8C3.tag=23,((_tmp8C3.f1=e,((_tmp8C3.f2=n,((_tmp8C3.f3=0,((_tmp8C3.f4=0,
_tmp8C3)))))))))),_tmp8C2))));}static void*Cyc_Toc_aggrarrow_exp_r(struct Cyc_Absyn_Exp*
e,struct _dyneither_ptr*n);static void*Cyc_Toc_aggrarrow_exp_r(struct Cyc_Absyn_Exp*
e,struct _dyneither_ptr*n){struct Cyc_Absyn_AggrArrow_e_struct _tmp8C6;struct Cyc_Absyn_AggrArrow_e_struct*
_tmp8C5;return(void*)((_tmp8C5=_cycalloc(sizeof(*_tmp8C5)),((_tmp8C5[0]=((
_tmp8C6.tag=24,((_tmp8C6.f1=e,((_tmp8C6.f2=n,((_tmp8C6.f3=0,((_tmp8C6.f4=0,
_tmp8C6)))))))))),_tmp8C5))));}static void*Cyc_Toc_unresolvedmem_exp_r(struct Cyc_Core_Opt*
tdopt,struct Cyc_List_List*ds);static void*Cyc_Toc_unresolvedmem_exp_r(struct Cyc_Core_Opt*
tdopt,struct Cyc_List_List*ds){struct Cyc_Absyn_UnresolvedMem_e_struct _tmp8C9;
struct Cyc_Absyn_UnresolvedMem_e_struct*_tmp8C8;return(void*)((_tmp8C8=_cycalloc(
sizeof(*_tmp8C8)),((_tmp8C8[0]=((_tmp8C9.tag=37,((_tmp8C9.f1=tdopt,((_tmp8C9.f2=
ds,_tmp8C9)))))),_tmp8C8))));}static void*Cyc_Toc_goto_stmt_r(struct
_dyneither_ptr*v,struct Cyc_Absyn_Stmt*s);static void*Cyc_Toc_goto_stmt_r(struct
_dyneither_ptr*v,struct Cyc_Absyn_Stmt*s){struct Cyc_Absyn_Goto_s_struct _tmp8CC;
struct Cyc_Absyn_Goto_s_struct*_tmp8CB;return(void*)((_tmp8CB=_cycalloc(sizeof(*
_tmp8CB)),((_tmp8CB[0]=((_tmp8CC.tag=7,((_tmp8CC.f1=v,((_tmp8CC.f2=s,_tmp8CC)))))),
_tmp8CB))));}static struct Cyc_Absyn_Const_e_struct Cyc_Toc_zero_exp={0,{.Int_c={4,{(
void*)0,0}}}};struct Cyc_Toc_functionSet{struct Cyc_Absyn_Exp*fchar;struct Cyc_Absyn_Exp*
fshort;struct Cyc_Absyn_Exp*fint;struct Cyc_Absyn_Exp*ffloat;struct Cyc_Absyn_Exp*
fdouble;struct Cyc_Absyn_Exp*flongdouble;struct Cyc_Absyn_Exp*fvoidstar;};struct
Cyc_Toc_functionSet Cyc_Toc__zero_arr_plus_functionSet={& Cyc_Toc__zero_arr_plus_char_ev,&
Cyc_Toc__zero_arr_plus_short_ev,& Cyc_Toc__zero_arr_plus_int_ev,& Cyc_Toc__zero_arr_plus_float_ev,&
Cyc_Toc__zero_arr_plus_double_ev,& Cyc_Toc__zero_arr_plus_longdouble_ev,& Cyc_Toc__zero_arr_plus_voidstar_ev};
struct Cyc_Toc_functionSet Cyc_Toc__get_zero_arr_size_functionSet={& Cyc_Toc__get_zero_arr_size_char_ev,&
Cyc_Toc__get_zero_arr_size_short_ev,& Cyc_Toc__get_zero_arr_size_int_ev,& Cyc_Toc__get_zero_arr_size_float_ev,&
Cyc_Toc__get_zero_arr_size_double_ev,& Cyc_Toc__get_zero_arr_size_longdouble_ev,&
Cyc_Toc__get_zero_arr_size_voidstar_ev};struct Cyc_Toc_functionSet Cyc_Toc__zero_arr_inplace_plus_functionSet={&
Cyc_Toc__zero_arr_inplace_plus_char_ev,& Cyc_Toc__zero_arr_inplace_plus_short_ev,&
Cyc_Toc__zero_arr_inplace_plus_int_ev,& Cyc_Toc__zero_arr_inplace_plus_float_ev,&
Cyc_Toc__zero_arr_inplace_plus_double_ev,& Cyc_Toc__zero_arr_inplace_plus_longdouble_ev,&
Cyc_Toc__zero_arr_inplace_plus_voidstar_ev};struct Cyc_Toc_functionSet Cyc_Toc__zero_arr_inplace_plus_post_functionSet={&
Cyc_Toc__zero_arr_inplace_plus_post_char_ev,& Cyc_Toc__zero_arr_inplace_plus_post_short_ev,&
Cyc_Toc__zero_arr_inplace_plus_post_int_ev,& Cyc_Toc__zero_arr_inplace_plus_post_float_ev,&
Cyc_Toc__zero_arr_inplace_plus_post_double_ev,& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_ev,&
Cyc_Toc__zero_arr_inplace_plus_post_voidstar_ev};static struct Cyc_Absyn_Exp*Cyc_Toc_getFunctionType(
struct Cyc_Toc_functionSet*fS,void*t);static struct Cyc_Absyn_Exp*Cyc_Toc_getFunctionType(
struct Cyc_Toc_functionSet*fS,void*t){struct Cyc_Absyn_Exp*function;{void*_tmpC0=t;
void*_tmpC1;int _tmpC2;_LL1: if(_tmpC0 <= (void*)4)goto _LL3;if(*((int*)_tmpC0)!= 5)
goto _LL3;_tmpC1=(void*)((struct Cyc_Absyn_IntType_struct*)_tmpC0)->f2;_LL2:{void*
_tmpC3=_tmpC1;_LLC: if((int)_tmpC3 != 0)goto _LLE;_LLD: function=fS->fchar;goto _LLB;
_LLE: if((int)_tmpC3 != 1)goto _LL10;_LLF: function=fS->fshort;goto _LLB;_LL10: if((
int)_tmpC3 != 2)goto _LL12;_LL11: function=fS->fint;goto _LLB;_LL12:;_LL13: {struct
Cyc_Core_Impossible_struct _tmp8D2;const char*_tmp8D1;struct Cyc_Core_Impossible_struct*
_tmp8D0;(int)_throw((void*)((_tmp8D0=_cycalloc(sizeof(*_tmp8D0)),((_tmp8D0[0]=((
_tmp8D2.tag=Cyc_Core_Impossible,((_tmp8D2.f1=((_tmp8D1="impossible IntType (not char, short or int)",
_tag_dyneither(_tmp8D1,sizeof(char),44))),_tmp8D2)))),_tmp8D0)))));}_LLB:;}goto
_LL0;_LL3: if((int)_tmpC0 != 1)goto _LL5;_LL4: function=fS->ffloat;goto _LL0;_LL5: if(
_tmpC0 <= (void*)4)goto _LL9;if(*((int*)_tmpC0)!= 6)goto _LL7;_tmpC2=((struct Cyc_Absyn_DoubleType_struct*)
_tmpC0)->f1;_LL6: switch(_tmpC2){case 1: _LL14: function=fS->flongdouble;break;
default: _LL15: function=fS->fdouble;}goto _LL0;_LL7: if(*((int*)_tmpC0)!= 4)goto
_LL9;_LL8: function=fS->fvoidstar;goto _LL0;_LL9:;_LLA: {struct Cyc_Core_Impossible_struct
_tmp8D8;const char*_tmp8D7;struct Cyc_Core_Impossible_struct*_tmp8D6;(int)_throw((
void*)((_tmp8D6=_cycalloc(sizeof(*_tmp8D6)),((_tmp8D6[0]=((_tmp8D8.tag=Cyc_Core_Impossible,((
_tmp8D8.f1=((_tmp8D7="impossible expression type (not int, float, double, or pointer)",
_tag_dyneither(_tmp8D7,sizeof(char),64))),_tmp8D8)))),_tmp8D6)))));}_LL0:;}
return function;}struct Cyc_Absyn_Exp*Cyc_Toc_getFunction(struct Cyc_Toc_functionSet*
fS,struct Cyc_Absyn_Exp*arr);struct Cyc_Absyn_Exp*Cyc_Toc_getFunction(struct Cyc_Toc_functionSet*
fS,struct Cyc_Absyn_Exp*arr){return Cyc_Toc_getFunctionType(fS,Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(arr->topt))->v));}struct Cyc_Absyn_Exp*
Cyc_Toc_getFunctionRemovePointer(struct Cyc_Toc_functionSet*fS,struct Cyc_Absyn_Exp*
arr);struct Cyc_Absyn_Exp*Cyc_Toc_getFunctionRemovePointer(struct Cyc_Toc_functionSet*
fS,struct Cyc_Absyn_Exp*arr){void*_tmpCA=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(arr->topt))->v);struct Cyc_Absyn_PtrInfo _tmpCB;void*_tmpCC;_LL18: if(
_tmpCA <= (void*)4)goto _LL1A;if(*((int*)_tmpCA)!= 4)goto _LL1A;_tmpCB=((struct Cyc_Absyn_PointerType_struct*)
_tmpCA)->f1;_tmpCC=_tmpCB.elt_typ;_LL19: return Cyc_Toc_getFunctionType(fS,_tmpCC);
_LL1A:;_LL1B: {struct Cyc_Core_Impossible_struct _tmp8DE;const char*_tmp8DD;struct
Cyc_Core_Impossible_struct*_tmp8DC;(int)_throw((void*)((_tmp8DC=_cycalloc(
sizeof(*_tmp8DC)),((_tmp8DC[0]=((_tmp8DE.tag=Cyc_Core_Impossible,((_tmp8DE.f1=((
_tmp8DD="impossible type (not pointer)",_tag_dyneither(_tmp8DD,sizeof(char),30))),
_tmp8DE)))),_tmp8DC)))));}_LL17:;}struct _tuple16{struct Cyc_List_List*f1;struct
Cyc_Absyn_Exp*f2;};static int Cyc_Toc_is_zero(struct Cyc_Absyn_Exp*e);static int Cyc_Toc_is_zero(
struct Cyc_Absyn_Exp*e){void*_tmpD0=e->r;union Cyc_Absyn_Cnst _tmpD1;struct _tuple4
_tmpD2;char _tmpD3;union Cyc_Absyn_Cnst _tmpD4;struct _tuple5 _tmpD5;short _tmpD6;
union Cyc_Absyn_Cnst _tmpD7;struct _tuple6 _tmpD8;int _tmpD9;union Cyc_Absyn_Cnst
_tmpDA;struct _tuple7 _tmpDB;long long _tmpDC;union Cyc_Absyn_Cnst _tmpDD;int _tmpDE;
struct Cyc_Absyn_Exp*_tmpDF;struct Cyc_List_List*_tmpE0;struct Cyc_List_List*_tmpE1;
struct Cyc_List_List*_tmpE2;struct Cyc_List_List*_tmpE3;struct Cyc_List_List*_tmpE4;
_LL1D: if(*((int*)_tmpD0)!= 0)goto _LL1F;_tmpD1=((struct Cyc_Absyn_Const_e_struct*)
_tmpD0)->f1;if((_tmpD1.Char_c).tag != 2)goto _LL1F;_tmpD2=(struct _tuple4)(_tmpD1.Char_c).val;
_tmpD3=_tmpD2.f2;_LL1E: return _tmpD3 == '\000';_LL1F: if(*((int*)_tmpD0)!= 0)goto
_LL21;_tmpD4=((struct Cyc_Absyn_Const_e_struct*)_tmpD0)->f1;if((_tmpD4.Short_c).tag
!= 3)goto _LL21;_tmpD5=(struct _tuple5)(_tmpD4.Short_c).val;_tmpD6=_tmpD5.f2;_LL20:
return _tmpD6 == 0;_LL21: if(*((int*)_tmpD0)!= 0)goto _LL23;_tmpD7=((struct Cyc_Absyn_Const_e_struct*)
_tmpD0)->f1;if((_tmpD7.Int_c).tag != 4)goto _LL23;_tmpD8=(struct _tuple6)(_tmpD7.Int_c).val;
_tmpD9=_tmpD8.f2;_LL22: return _tmpD9 == 0;_LL23: if(*((int*)_tmpD0)!= 0)goto _LL25;
_tmpDA=((struct Cyc_Absyn_Const_e_struct*)_tmpD0)->f1;if((_tmpDA.LongLong_c).tag
!= 5)goto _LL25;_tmpDB=(struct _tuple7)(_tmpDA.LongLong_c).val;_tmpDC=_tmpDB.f2;
_LL24: return _tmpDC == 0;_LL25: if(*((int*)_tmpD0)!= 0)goto _LL27;_tmpDD=((struct Cyc_Absyn_Const_e_struct*)
_tmpD0)->f1;if((_tmpDD.Null_c).tag != 1)goto _LL27;_tmpDE=(int)(_tmpDD.Null_c).val;
_LL26: return 1;_LL27: if(*((int*)_tmpD0)!= 15)goto _LL29;_tmpDF=((struct Cyc_Absyn_Cast_e_struct*)
_tmpD0)->f2;_LL28: return Cyc_Toc_is_zero(_tmpDF);_LL29: if(*((int*)_tmpD0)!= 26)
goto _LL2B;_tmpE0=((struct Cyc_Absyn_Tuple_e_struct*)_tmpD0)->f1;_LL2A: return((int(*)(
int(*pred)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_forall)(Cyc_Toc_is_zero,
_tmpE0);_LL2B: if(*((int*)_tmpD0)!= 28)goto _LL2D;_tmpE1=((struct Cyc_Absyn_Array_e_struct*)
_tmpD0)->f1;_LL2C: _tmpE2=_tmpE1;goto _LL2E;_LL2D: if(*((int*)_tmpD0)!= 30)goto
_LL2F;_tmpE2=((struct Cyc_Absyn_Aggregate_e_struct*)_tmpD0)->f3;_LL2E: _tmpE3=
_tmpE2;goto _LL30;_LL2F: if(*((int*)_tmpD0)!= 27)goto _LL31;_tmpE3=((struct Cyc_Absyn_CompoundLit_e_struct*)
_tmpD0)->f2;_LL30: _tmpE4=_tmpE3;goto _LL32;_LL31: if(*((int*)_tmpD0)!= 37)goto
_LL33;_tmpE4=((struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmpD0)->f2;_LL32: for(0;
_tmpE4 != 0;_tmpE4=_tmpE4->tl){if(!Cyc_Toc_is_zero((*((struct _tuple16*)_tmpE4->hd)).f2))
return 0;}return 1;_LL33:;_LL34: return 0;_LL1C:;}static int Cyc_Toc_is_nullable(void*
t);static int Cyc_Toc_is_nullable(void*t){void*_tmpE5=Cyc_Tcutil_compress(t);
struct Cyc_Absyn_PtrInfo _tmpE6;struct Cyc_Absyn_PtrAtts _tmpE7;union Cyc_Absyn_Constraint*
_tmpE8;_LL36: if(_tmpE5 <= (void*)4)goto _LL38;if(*((int*)_tmpE5)!= 4)goto _LL38;
_tmpE6=((struct Cyc_Absyn_PointerType_struct*)_tmpE5)->f1;_tmpE7=_tmpE6.ptr_atts;
_tmpE8=_tmpE7.nullable;_LL37: return((int(*)(int y,union Cyc_Absyn_Constraint*x))
Cyc_Absyn_conref_def)(0,_tmpE8);_LL38:;_LL39: {const char*_tmp8E1;void*_tmp8E0;(
_tmp8E0=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmp8E1="is_nullable",_tag_dyneither(_tmp8E1,sizeof(char),12))),_tag_dyneither(
_tmp8E0,sizeof(void*),0)));}_LL35:;}static char _tmpF7[1]="";static char _tmpF8[8]="*bogus*";
static struct _tuple1*Cyc_Toc_collapse_qvar_tag(struct _tuple1*x,struct
_dyneither_ptr tag);static struct _tuple1*Cyc_Toc_collapse_qvar_tag(struct _tuple1*x,
struct _dyneither_ptr tag){struct _DynRegionHandle*_tmpEC;struct Cyc_Dict_Dict*
_tmpED;struct Cyc_Toc_TocState _tmpEB=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));
_tmpEC=_tmpEB.dyn;_tmpED=_tmpEB.qvar_tags;{static struct _dyneither_ptr
bogus_string={_tmpF8,_tmpF8,_tmpF8 + 8};static struct _tuple1 bogus_qvar={{.Loc_n={3,
0}},& bogus_string};static struct _tuple14 pair={& bogus_qvar,{_tmpF7,_tmpF7,_tmpF7 + 
1}};{struct _tuple14 _tmp8E2;pair=((_tmp8E2.f1=x,((_tmp8E2.f2=tag,_tmp8E2))));}{
struct _DynRegionFrame _tmpEF;struct _RegionHandle*d=_open_dynregion(& _tmpEF,_tmpEC);{
struct _tuple1**_tmpF0=((struct _tuple1**(*)(struct Cyc_Dict_Dict d,struct _tuple14*k))
Cyc_Dict_lookup_opt)(*_tmpED,(struct _tuple14*)& pair);if(_tmpF0 != 0){struct
_tuple1*_tmpF1=*_tmpF0;_npop_handler(0);return _tmpF1;}{struct _tuple14*_tmp8E3;
struct _tuple14*_tmpF2=(_tmp8E3=_cycalloc(sizeof(*_tmp8E3)),((_tmp8E3->f1=x,((
_tmp8E3->f2=tag,_tmp8E3)))));struct _dyneither_ptr*_tmp8E6;struct _tuple1*_tmp8E5;
struct _tuple1*res=(_tmp8E5=_cycalloc(sizeof(*_tmp8E5)),((_tmp8E5->f1=(*x).f1,((
_tmp8E5->f2=((_tmp8E6=_cycalloc(sizeof(*_tmp8E6)),((_tmp8E6[0]=(struct
_dyneither_ptr)Cyc_strconcat((struct _dyneither_ptr)*(*x).f2,(struct
_dyneither_ptr)tag),_tmp8E6)))),_tmp8E5)))));*_tmpED=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,struct _tuple14*k,struct _tuple1*v))Cyc_Dict_insert)(*_tmpED,(
struct _tuple14*)_tmpF2,res);{struct _tuple1*_tmpF3=res;_npop_handler(0);return
_tmpF3;}}};_pop_dynregion(d);}}}struct _tuple17{void*f1;struct Cyc_List_List*f2;};
static void*Cyc_Toc_add_tuple_type(struct Cyc_List_List*tqs0);static void*Cyc_Toc_add_tuple_type(
struct Cyc_List_List*tqs0){struct _DynRegionHandle*_tmpFA;struct Cyc_List_List**
_tmpFB;struct Cyc_Toc_TocState _tmpF9=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));
_tmpFA=_tmpF9.dyn;_tmpFB=_tmpF9.tuple_types;{struct _DynRegionFrame _tmpFC;struct
_RegionHandle*d=_open_dynregion(& _tmpFC,_tmpFA);{struct Cyc_List_List*_tmpFD=*
_tmpFB;for(0;_tmpFD != 0;_tmpFD=_tmpFD->tl){struct _tuple17 _tmpFF;void*_tmp100;
struct Cyc_List_List*_tmp101;struct _tuple17*_tmpFE=(struct _tuple17*)_tmpFD->hd;
_tmpFF=*_tmpFE;_tmp100=_tmpFF.f1;_tmp101=_tmpFF.f2;{struct Cyc_List_List*_tmp102=
tqs0;for(0;_tmp102 != 0  && _tmp101 != 0;(_tmp102=_tmp102->tl,_tmp101=_tmp101->tl)){
if(!Cyc_Tcutil_unify((*((struct _tuple10*)_tmp102->hd)).f2,(void*)_tmp101->hd))
break;}if(_tmp102 == 0  && _tmp101 == 0){void*_tmp103=_tmp100;_npop_handler(0);
return _tmp103;}}}}{struct Cyc_Int_pa_struct _tmp8EE;void*_tmp8ED[1];const char*
_tmp8EC;struct _dyneither_ptr*_tmp8EB;struct _dyneither_ptr*xname=(_tmp8EB=
_cycalloc(sizeof(*_tmp8EB)),((_tmp8EB[0]=(struct _dyneither_ptr)((_tmp8EE.tag=1,((
_tmp8EE.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++,((_tmp8ED[0]=& _tmp8EE,Cyc_aprintf(((
_tmp8EC="_tuple%d",_tag_dyneither(_tmp8EC,sizeof(char),9))),_tag_dyneither(
_tmp8ED,sizeof(void*),1)))))))),_tmp8EB)));void*x=Cyc_Absyn_strct(xname);struct
Cyc_List_List*ts=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct
_tuple10*),struct Cyc_List_List*x))Cyc_List_rmap)(d,Cyc_Tcutil_snd_tqt,tqs0);
struct Cyc_List_List*_tmp104=0;struct Cyc_List_List*ts2=ts;{int i=1;for(0;ts2 != 0;(
ts2=ts2->tl,i ++)){struct Cyc_Absyn_Aggrfield*_tmp8F1;struct Cyc_List_List*_tmp8F0;
_tmp104=((_tmp8F0=_cycalloc(sizeof(*_tmp8F0)),((_tmp8F0->hd=((_tmp8F1=_cycalloc(
sizeof(*_tmp8F1)),((_tmp8F1->name=Cyc_Absyn_fieldname(i),((_tmp8F1->tq=Cyc_Toc_mt_tq,((
_tmp8F1->type=(void*)ts2->hd,((_tmp8F1->width=0,((_tmp8F1->attributes=0,_tmp8F1)))))))))))),((
_tmp8F0->tl=_tmp104,_tmp8F0))))));}}_tmp104=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(_tmp104);{struct _tuple1*_tmp8F6;struct Cyc_Absyn_AggrdeclImpl*
_tmp8F5;struct Cyc_Absyn_Aggrdecl*_tmp8F4;struct Cyc_Absyn_Aggrdecl*_tmp107=(
_tmp8F4=_cycalloc(sizeof(*_tmp8F4)),((_tmp8F4->kind=(void*)0,((_tmp8F4->sc=(void*)
2,((_tmp8F4->name=((_tmp8F6=_cycalloc(sizeof(*_tmp8F6)),((_tmp8F6->f1=Cyc_Absyn_Rel_n(
0),((_tmp8F6->f2=xname,_tmp8F6)))))),((_tmp8F4->tvs=0,((_tmp8F4->impl=((_tmp8F5=
_cycalloc(sizeof(*_tmp8F5)),((_tmp8F5->exist_vars=0,((_tmp8F5->rgn_po=0,((
_tmp8F5->fields=_tmp104,((_tmp8F5->tagged=0,_tmp8F5)))))))))),((_tmp8F4->attributes=
0,_tmp8F4)))))))))))));{struct Cyc_Absyn_Aggr_d_struct*_tmp8FC;struct Cyc_Absyn_Aggr_d_struct
_tmp8FB;struct Cyc_List_List*_tmp8FA;Cyc_Toc_result_decls=((_tmp8FA=_cycalloc(
sizeof(*_tmp8FA)),((_tmp8FA->hd=Cyc_Absyn_new_decl((void*)((_tmp8FC=_cycalloc(
sizeof(*_tmp8FC)),((_tmp8FC[0]=((_tmp8FB.tag=6,((_tmp8FB.f1=_tmp107,_tmp8FB)))),
_tmp8FC)))),0),((_tmp8FA->tl=Cyc_Toc_result_decls,_tmp8FA))))));}{struct _tuple17*
_tmp8FF;struct Cyc_List_List*_tmp8FE;*_tmpFB=((_tmp8FE=_region_malloc(d,sizeof(*
_tmp8FE)),((_tmp8FE->hd=((_tmp8FF=_region_malloc(d,sizeof(*_tmp8FF)),((_tmp8FF->f1=
x,((_tmp8FF->f2=ts,_tmp8FF)))))),((_tmp8FE->tl=*_tmpFB,_tmp8FE))))));}{void*
_tmp10D=x;_npop_handler(0);return _tmp10D;}}};_pop_dynregion(d);}}struct _tuple1*
Cyc_Toc_temp_var();struct _tuple1*Cyc_Toc_temp_var(){int _tmp115=Cyc_Toc_temp_var_counter
++;struct _dyneither_ptr*_tmp90C;const char*_tmp90B;void*_tmp90A[1];struct Cyc_Int_pa_struct
_tmp909;struct _tuple1*_tmp908;struct _tuple1*res=(_tmp908=_cycalloc(sizeof(*
_tmp908)),((_tmp908->f1=Cyc_Absyn_Loc_n,((_tmp908->f2=((_tmp90C=_cycalloc(
sizeof(*_tmp90C)),((_tmp90C[0]=(struct _dyneither_ptr)((_tmp909.tag=1,((_tmp909.f1=(
unsigned int)_tmp115,((_tmp90A[0]=& _tmp909,Cyc_aprintf(((_tmp90B="_tmp%X",
_tag_dyneither(_tmp90B,sizeof(char),7))),_tag_dyneither(_tmp90A,sizeof(void*),1)))))))),
_tmp90C)))),_tmp908)))));return res;}static struct _dyneither_ptr*Cyc_Toc_fresh_label();
static struct _dyneither_ptr*Cyc_Toc_fresh_label(){struct _DynRegionHandle*_tmp11C;
struct Cyc_Xarray_Xarray*_tmp11D;struct Cyc_Toc_TocState _tmp11B=*((struct Cyc_Toc_TocState*)
_check_null(Cyc_Toc_toc_state));_tmp11C=_tmp11B.dyn;_tmp11D=_tmp11B.temp_labels;{
struct _DynRegionFrame _tmp11E;struct _RegionHandle*d=_open_dynregion(& _tmp11E,
_tmp11C);{int _tmp11F=Cyc_Toc_fresh_label_counter ++;if(_tmp11F < ((int(*)(struct
Cyc_Xarray_Xarray*))Cyc_Xarray_length)(_tmp11D)){struct _dyneither_ptr*_tmp120=((
struct _dyneither_ptr*(*)(struct Cyc_Xarray_Xarray*,int))Cyc_Xarray_get)(_tmp11D,
_tmp11F);_npop_handler(0);return _tmp120;}{struct Cyc_Int_pa_struct _tmp914;void*
_tmp913[1];const char*_tmp912;struct _dyneither_ptr*_tmp911;struct _dyneither_ptr*
res=(_tmp911=_cycalloc(sizeof(*_tmp911)),((_tmp911[0]=(struct _dyneither_ptr)((
_tmp914.tag=1,((_tmp914.f1=(unsigned int)_tmp11F,((_tmp913[0]=& _tmp914,Cyc_aprintf(((
_tmp912="_LL%X",_tag_dyneither(_tmp912,sizeof(char),6))),_tag_dyneither(_tmp913,
sizeof(void*),1)))))))),_tmp911)));if(((int(*)(struct Cyc_Xarray_Xarray*,struct
_dyneither_ptr*))Cyc_Xarray_add_ind)(_tmp11D,res)!= _tmp11F){const char*_tmp917;
void*_tmp916;(_tmp916=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Toc_toc_impos)(((_tmp917="fresh_label: add_ind returned bad index...",
_tag_dyneither(_tmp917,sizeof(char),43))),_tag_dyneither(_tmp916,sizeof(void*),0)));}{
struct _dyneither_ptr*_tmp123=res;_npop_handler(0);return _tmp123;}}};
_pop_dynregion(d);}}static struct Cyc_Absyn_Exp*Cyc_Toc_datatype_tag(struct Cyc_Absyn_Datatypedecl*
td,struct _tuple1*name,int carries_value);static struct Cyc_Absyn_Exp*Cyc_Toc_datatype_tag(
struct Cyc_Absyn_Datatypedecl*td,struct _tuple1*name,int carries_value){int ans=0;
struct Cyc_List_List*_tmp128=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(td->fields))->v;while(Cyc_Absyn_qvar_cmp(name,((struct Cyc_Absyn_Datatypefield*)((
struct Cyc_List_List*)_check_null(_tmp128))->hd)->name)!= 0){if(carries_value  && ((
struct Cyc_Absyn_Datatypefield*)_tmp128->hd)->typs != 0  || !carries_value  && ((
struct Cyc_Absyn_Datatypefield*)_tmp128->hd)->typs == 0)++ ans;_tmp128=_tmp128->tl;}
return Cyc_Absyn_uint_exp((unsigned int)ans,0);}static int Cyc_Toc_num_void_tags(
struct Cyc_Absyn_Datatypedecl*td);static int Cyc_Toc_num_void_tags(struct Cyc_Absyn_Datatypedecl*
td){int ans=0;{struct Cyc_List_List*_tmp129=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(td->fields))->v;for(0;_tmp129 != 0;_tmp129=_tmp129->tl){if(((struct
Cyc_Absyn_Datatypefield*)_tmp129->hd)->typs == 0)++ ans;}}return ans;}static void*
Cyc_Toc_typ_to_c(void*t);static struct _tuple8*Cyc_Toc_arg_to_c(struct _tuple8*a);
static struct _tuple8*Cyc_Toc_arg_to_c(struct _tuple8*a){struct Cyc_Core_Opt*_tmp12B;
struct Cyc_Absyn_Tqual _tmp12C;void*_tmp12D;struct _tuple8 _tmp12A=*a;_tmp12B=
_tmp12A.f1;_tmp12C=_tmp12A.f2;_tmp12D=_tmp12A.f3;{struct _tuple8*_tmp918;return(
_tmp918=_cycalloc(sizeof(*_tmp918)),((_tmp918->f1=_tmp12B,((_tmp918->f2=_tmp12C,((
_tmp918->f3=Cyc_Toc_typ_to_c(_tmp12D),_tmp918)))))));}}static struct _tuple10*Cyc_Toc_typ_to_c_f(
struct _tuple10*x);static struct _tuple10*Cyc_Toc_typ_to_c_f(struct _tuple10*x){
struct Cyc_Absyn_Tqual _tmp130;void*_tmp131;struct _tuple10 _tmp12F=*x;_tmp130=
_tmp12F.f1;_tmp131=_tmp12F.f2;{struct _tuple10*_tmp919;return(_tmp919=_cycalloc(
sizeof(*_tmp919)),((_tmp919->f1=_tmp130,((_tmp919->f2=Cyc_Toc_typ_to_c(_tmp131),
_tmp919)))));}}static void*Cyc_Toc_typ_to_c_array(void*t);static void*Cyc_Toc_typ_to_c_array(
void*t){void*_tmp133=Cyc_Tcutil_compress(t);struct Cyc_Absyn_ArrayInfo _tmp134;
void*_tmp135;struct Cyc_Absyn_Tqual _tmp136;struct Cyc_Absyn_Exp*_tmp137;union Cyc_Absyn_Constraint*
_tmp138;struct Cyc_Position_Segment*_tmp139;struct Cyc_Core_Opt*_tmp13A;struct Cyc_Core_Opt
_tmp13B;void*_tmp13C;_LL3B: if(_tmp133 <= (void*)4)goto _LL3F;if(*((int*)_tmp133)!= 
7)goto _LL3D;_tmp134=((struct Cyc_Absyn_ArrayType_struct*)_tmp133)->f1;_tmp135=
_tmp134.elt_type;_tmp136=_tmp134.tq;_tmp137=_tmp134.num_elts;_tmp138=_tmp134.zero_term;
_tmp139=_tmp134.zt_loc;_LL3C: return Cyc_Absyn_array_typ(Cyc_Toc_typ_to_c_array(
_tmp135),_tmp136,_tmp137,Cyc_Absyn_false_conref,_tmp139);_LL3D: if(*((int*)
_tmp133)!= 0)goto _LL3F;_tmp13A=((struct Cyc_Absyn_Evar_struct*)_tmp133)->f2;if(
_tmp13A == 0)goto _LL3F;_tmp13B=*_tmp13A;_tmp13C=(void*)_tmp13B.v;_LL3E: return Cyc_Toc_typ_to_c_array(
_tmp13C);_LL3F:;_LL40: return Cyc_Toc_typ_to_c(t);_LL3A:;}static struct Cyc_Absyn_Aggrfield*
Cyc_Toc_aggrfield_to_c(struct Cyc_Absyn_Aggrfield*f);static struct Cyc_Absyn_Aggrfield*
Cyc_Toc_aggrfield_to_c(struct Cyc_Absyn_Aggrfield*f){struct Cyc_Absyn_Aggrfield*
_tmp91A;return(_tmp91A=_cycalloc(sizeof(*_tmp91A)),((_tmp91A->name=f->name,((
_tmp91A->tq=Cyc_Toc_mt_tq,((_tmp91A->type=Cyc_Toc_typ_to_c(f->type),((_tmp91A->width=
f->width,((_tmp91A->attributes=f->attributes,_tmp91A)))))))))));}static void Cyc_Toc_enumfields_to_c(
struct Cyc_List_List*fs);static void Cyc_Toc_enumfields_to_c(struct Cyc_List_List*fs){
return;}static void*Cyc_Toc_char_star_typ();static void*Cyc_Toc_char_star_typ(){
static void**cs=0;if(cs == 0){void**_tmp91B;cs=((_tmp91B=_cycalloc(sizeof(*_tmp91B)),((
_tmp91B[0]=Cyc_Absyn_star_typ(Cyc_Absyn_char_typ,(void*)2,Cyc_Toc_mt_tq,Cyc_Absyn_false_conref),
_tmp91B))));}return*cs;}static void*Cyc_Toc_rgn_typ();static void*Cyc_Toc_rgn_typ(){
static void**r=0;if(r == 0){void**_tmp91C;r=((_tmp91C=_cycalloc(sizeof(*_tmp91C)),((
_tmp91C[0]=Cyc_Absyn_cstar_typ(Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp),Cyc_Toc_mt_tq),
_tmp91C))));}return*r;}static void*Cyc_Toc_dyn_rgn_typ();static void*Cyc_Toc_dyn_rgn_typ(){
static void**r=0;if(r == 0){void**_tmp91D;r=((_tmp91D=_cycalloc(sizeof(*_tmp91D)),((
_tmp91D[0]=Cyc_Absyn_cstar_typ(Cyc_Absyn_strct(Cyc_Toc__DynRegionHandle_sp),Cyc_Toc_mt_tq),
_tmp91D))));}return*r;}static void*Cyc_Toc_typ_to_c(void*t);static void*Cyc_Toc_typ_to_c(
void*t){void*_tmp141=t;struct Cyc_Core_Opt*_tmp142;struct Cyc_Core_Opt*_tmp143;
struct Cyc_Core_Opt _tmp144;void*_tmp145;struct Cyc_Absyn_Tvar*_tmp146;struct Cyc_Absyn_DatatypeInfo
_tmp147;union Cyc_Absyn_DatatypeInfoU _tmp148;struct Cyc_Absyn_Datatypedecl**
_tmp149;struct Cyc_Absyn_Datatypedecl*_tmp14A;struct Cyc_Absyn_DatatypeFieldInfo
_tmp14B;union Cyc_Absyn_DatatypeFieldInfoU _tmp14C;struct _tuple2 _tmp14D;struct Cyc_Absyn_Datatypedecl*
_tmp14E;struct Cyc_Absyn_Datatypefield*_tmp14F;struct Cyc_Absyn_PtrInfo _tmp150;
void*_tmp151;struct Cyc_Absyn_Tqual _tmp152;struct Cyc_Absyn_PtrAtts _tmp153;union
Cyc_Absyn_Constraint*_tmp154;struct Cyc_Absyn_ArrayInfo _tmp155;void*_tmp156;
struct Cyc_Absyn_Tqual _tmp157;struct Cyc_Absyn_Exp*_tmp158;struct Cyc_Position_Segment*
_tmp159;struct Cyc_Absyn_FnInfo _tmp15A;void*_tmp15B;struct Cyc_List_List*_tmp15C;
int _tmp15D;struct Cyc_Absyn_VarargInfo*_tmp15E;struct Cyc_List_List*_tmp15F;struct
Cyc_List_List*_tmp160;void*_tmp161;struct Cyc_List_List*_tmp162;struct Cyc_Absyn_AggrInfo
_tmp163;union Cyc_Absyn_AggrInfoU _tmp164;struct Cyc_List_List*_tmp165;struct
_tuple1*_tmp166;struct Cyc_List_List*_tmp167;struct _tuple1*_tmp168;struct Cyc_List_List*
_tmp169;struct Cyc_Absyn_Typedefdecl*_tmp16A;void**_tmp16B;void*_tmp16C;_LL42: if((
int)_tmp141 != 0)goto _LL44;_LL43: return t;_LL44: if(_tmp141 <= (void*)4)goto _LL56;
if(*((int*)_tmp141)!= 0)goto _LL46;_tmp142=((struct Cyc_Absyn_Evar_struct*)_tmp141)->f2;
if(_tmp142 != 0)goto _LL46;_LL45: return Cyc_Absyn_sint_typ;_LL46: if(*((int*)_tmp141)
!= 0)goto _LL48;_tmp143=((struct Cyc_Absyn_Evar_struct*)_tmp141)->f2;if(_tmp143 == 
0)goto _LL48;_tmp144=*_tmp143;_tmp145=(void*)_tmp144.v;_LL47: return Cyc_Toc_typ_to_c(
_tmp145);_LL48: if(*((int*)_tmp141)!= 1)goto _LL4A;_tmp146=((struct Cyc_Absyn_VarType_struct*)
_tmp141)->f1;_LL49: if(Cyc_Tcutil_tvar_kind(_tmp146)== (void*)0)return(void*)0;
else{return Cyc_Absyn_void_star_typ();}_LL4A: if(*((int*)_tmp141)!= 2)goto _LL4C;
_tmp147=((struct Cyc_Absyn_DatatypeType_struct*)_tmp141)->f1;_tmp148=_tmp147.datatype_info;
if((_tmp148.KnownDatatype).tag != 2)goto _LL4C;_tmp149=(struct Cyc_Absyn_Datatypedecl**)(
_tmp148.KnownDatatype).val;_tmp14A=*_tmp149;_LL4B: return Cyc_Absyn_void_star_typ();
_LL4C: if(*((int*)_tmp141)!= 2)goto _LL4E;_LL4D: {const char*_tmp920;void*_tmp91F;(
_tmp91F=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmp920="unresolved DatatypeType",_tag_dyneither(_tmp920,sizeof(char),24))),
_tag_dyneither(_tmp91F,sizeof(void*),0)));}_LL4E: if(*((int*)_tmp141)!= 3)goto
_LL50;_tmp14B=((struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp141)->f1;_tmp14C=
_tmp14B.field_info;if((_tmp14C.KnownDatatypefield).tag != 2)goto _LL50;_tmp14D=(
struct _tuple2)(_tmp14C.KnownDatatypefield).val;_tmp14E=_tmp14D.f1;_tmp14F=
_tmp14D.f2;_LL4F: if(_tmp14F->typs == 0){if(_tmp14E->is_extensible)return Cyc_Toc_char_star_typ();
else{return Cyc_Absyn_uint_typ;}}else{const char*_tmp921;return Cyc_Absyn_strctq(
Cyc_Toc_collapse_qvar_tag(_tmp14F->name,((_tmp921="_struct",_tag_dyneither(
_tmp921,sizeof(char),8)))));}_LL50: if(*((int*)_tmp141)!= 3)goto _LL52;_LL51: {
const char*_tmp924;void*_tmp923;(_tmp923=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmp924="unresolved DatatypeFieldType",
_tag_dyneither(_tmp924,sizeof(char),29))),_tag_dyneither(_tmp923,sizeof(void*),0)));}
_LL52: if(*((int*)_tmp141)!= 4)goto _LL54;_tmp150=((struct Cyc_Absyn_PointerType_struct*)
_tmp141)->f1;_tmp151=_tmp150.elt_typ;_tmp152=_tmp150.elt_tq;_tmp153=_tmp150.ptr_atts;
_tmp154=_tmp153.bounds;_LL53: _tmp151=Cyc_Toc_typ_to_c_array(_tmp151);{void*
_tmp172=Cyc_Absyn_conref_def((void*)((void*)0),_tmp154);_LL7D: if((int)_tmp172 != 
0)goto _LL7F;_LL7E: return Cyc_Toc_dyneither_ptr_typ;_LL7F:;_LL80: return Cyc_Absyn_star_typ(
_tmp151,(void*)2,_tmp152,Cyc_Absyn_false_conref);_LL7C:;}_LL54: if(*((int*)
_tmp141)!= 5)goto _LL56;_LL55: goto _LL57;_LL56: if((int)_tmp141 != 1)goto _LL58;_LL57:
goto _LL59;_LL58: if(_tmp141 <= (void*)4)goto _LL70;if(*((int*)_tmp141)!= 6)goto
_LL5A;_LL59: return t;_LL5A: if(*((int*)_tmp141)!= 7)goto _LL5C;_tmp155=((struct Cyc_Absyn_ArrayType_struct*)
_tmp141)->f1;_tmp156=_tmp155.elt_type;_tmp157=_tmp155.tq;_tmp158=_tmp155.num_elts;
_tmp159=_tmp155.zt_loc;_LL5B: return Cyc_Absyn_array_typ(Cyc_Toc_typ_to_c_array(
_tmp156),_tmp157,_tmp158,Cyc_Absyn_false_conref,_tmp159);_LL5C: if(*((int*)
_tmp141)!= 8)goto _LL5E;_tmp15A=((struct Cyc_Absyn_FnType_struct*)_tmp141)->f1;
_tmp15B=_tmp15A.ret_typ;_tmp15C=_tmp15A.args;_tmp15D=_tmp15A.c_varargs;_tmp15E=
_tmp15A.cyc_varargs;_tmp15F=_tmp15A.attributes;_LL5D: {struct Cyc_List_List*
_tmp173=0;for(0;_tmp15F != 0;_tmp15F=_tmp15F->tl){void*_tmp174=(void*)_tmp15F->hd;
_LL82: if((int)_tmp174 != 3)goto _LL84;_LL83: goto _LL85;_LL84: if((int)_tmp174 != 4)
goto _LL86;_LL85: goto _LL87;_LL86: if(_tmp174 <= (void*)17)goto _LL8A;if(*((int*)
_tmp174)!= 3)goto _LL88;_LL87: continue;_LL88: if(*((int*)_tmp174)!= 4)goto _LL8A;
_LL89: continue;_LL8A:;_LL8B:{struct Cyc_List_List*_tmp925;_tmp173=((_tmp925=
_cycalloc(sizeof(*_tmp925)),((_tmp925->hd=(void*)((void*)_tmp15F->hd),((_tmp925->tl=
_tmp173,_tmp925))))));}goto _LL81;_LL81:;}{struct Cyc_List_List*_tmp176=((struct
Cyc_List_List*(*)(struct _tuple8*(*f)(struct _tuple8*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Toc_arg_to_c,_tmp15C);if(_tmp15E != 0){void*_tmp177=Cyc_Toc_typ_to_c(Cyc_Absyn_dyneither_typ(
_tmp15E->type,(void*)2,Cyc_Toc_mt_tq,Cyc_Absyn_false_conref));struct _tuple8*
_tmp926;struct _tuple8*_tmp178=(_tmp926=_cycalloc(sizeof(*_tmp926)),((_tmp926->f1=
_tmp15E->name,((_tmp926->f2=_tmp15E->tq,((_tmp926->f3=_tmp177,_tmp926)))))));
struct Cyc_List_List*_tmp927;_tmp176=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp176,((_tmp927=_cycalloc(
sizeof(*_tmp927)),((_tmp927->hd=_tmp178,((_tmp927->tl=0,_tmp927)))))));}{struct
Cyc_Absyn_FnType_struct _tmp92D;struct Cyc_Absyn_FnInfo _tmp92C;struct Cyc_Absyn_FnType_struct*
_tmp92B;return(void*)((_tmp92B=_cycalloc(sizeof(*_tmp92B)),((_tmp92B[0]=((
_tmp92D.tag=8,((_tmp92D.f1=((_tmp92C.tvars=0,((_tmp92C.effect=0,((_tmp92C.ret_typ=
Cyc_Toc_typ_to_c(_tmp15B),((_tmp92C.args=_tmp176,((_tmp92C.c_varargs=_tmp15D,((
_tmp92C.cyc_varargs=0,((_tmp92C.rgn_po=0,((_tmp92C.attributes=_tmp173,_tmp92C)))))))))))))))),
_tmp92D)))),_tmp92B))));}}}_LL5E: if(*((int*)_tmp141)!= 9)goto _LL60;_tmp160=((
struct Cyc_Absyn_TupleType_struct*)_tmp141)->f1;_LL5F: _tmp160=((struct Cyc_List_List*(*)(
struct _tuple10*(*f)(struct _tuple10*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_typ_to_c_f,
_tmp160);return Cyc_Toc_add_tuple_type(_tmp160);_LL60: if(*((int*)_tmp141)!= 11)
goto _LL62;_tmp161=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)_tmp141)->f1;
_tmp162=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp141)->f2;_LL61: {struct Cyc_Absyn_AnonAggrType_struct
_tmp930;struct Cyc_Absyn_AnonAggrType_struct*_tmp92F;return(void*)((_tmp92F=
_cycalloc(sizeof(*_tmp92F)),((_tmp92F[0]=((_tmp930.tag=11,((_tmp930.f1=(void*)
_tmp161,((_tmp930.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(
struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_aggrfield_to_c,
_tmp162),_tmp930)))))),_tmp92F))));}_LL62: if(*((int*)_tmp141)!= 10)goto _LL64;
_tmp163=((struct Cyc_Absyn_AggrType_struct*)_tmp141)->f1;_tmp164=_tmp163.aggr_info;
_tmp165=_tmp163.targs;_LL63:{union Cyc_Absyn_AggrInfoU _tmp180=_tmp164;struct
_tuple3 _tmp181;_LL8D: if((_tmp180.UnknownAggr).tag != 1)goto _LL8F;_tmp181=(struct
_tuple3)(_tmp180.UnknownAggr).val;_LL8E: return t;_LL8F:;_LL90: goto _LL8C;_LL8C:;}{
struct Cyc_Absyn_Aggrdecl*_tmp182=Cyc_Absyn_get_known_aggrdecl(_tmp164);if(
_tmp182->kind == (void*)1)return Cyc_Toc_aggrdecl_type(_tmp182->name,Cyc_Absyn_unionq_typ);
else{return Cyc_Toc_aggrdecl_type(_tmp182->name,Cyc_Absyn_strctq);}}_LL64: if(*((
int*)_tmp141)!= 12)goto _LL66;_tmp166=((struct Cyc_Absyn_EnumType_struct*)_tmp141)->f1;
_LL65: return t;_LL66: if(*((int*)_tmp141)!= 13)goto _LL68;_tmp167=((struct Cyc_Absyn_AnonEnumType_struct*)
_tmp141)->f1;_LL67: Cyc_Toc_enumfields_to_c(_tmp167);return t;_LL68: if(*((int*)
_tmp141)!= 16)goto _LL6A;_tmp168=((struct Cyc_Absyn_TypedefType_struct*)_tmp141)->f1;
_tmp169=((struct Cyc_Absyn_TypedefType_struct*)_tmp141)->f2;_tmp16A=((struct Cyc_Absyn_TypedefType_struct*)
_tmp141)->f3;_tmp16B=((struct Cyc_Absyn_TypedefType_struct*)_tmp141)->f4;_LL69:
if(_tmp16B == 0  || Cyc_noexpand_r){if(_tmp169 != 0){struct Cyc_Absyn_TypedefType_struct
_tmp933;struct Cyc_Absyn_TypedefType_struct*_tmp932;return(void*)((_tmp932=
_cycalloc(sizeof(*_tmp932)),((_tmp932[0]=((_tmp933.tag=16,((_tmp933.f1=_tmp168,((
_tmp933.f2=0,((_tmp933.f3=_tmp16A,((_tmp933.f4=0,_tmp933)))))))))),_tmp932))));}
else{return t;}}else{struct Cyc_Absyn_TypedefType_struct _tmp939;void**_tmp938;
struct Cyc_Absyn_TypedefType_struct*_tmp937;return(void*)((_tmp937=_cycalloc(
sizeof(*_tmp937)),((_tmp937[0]=((_tmp939.tag=16,((_tmp939.f1=_tmp168,((_tmp939.f2=
0,((_tmp939.f3=_tmp16A,((_tmp939.f4=((_tmp938=_cycalloc(sizeof(*_tmp938)),((
_tmp938[0]=Cyc_Toc_typ_to_c_array(*_tmp16B),_tmp938)))),_tmp939)))))))))),
_tmp937))));}_LL6A: if(*((int*)_tmp141)!= 18)goto _LL6C;_LL6B: return Cyc_Absyn_uint_typ;
_LL6C: if(*((int*)_tmp141)!= 14)goto _LL6E;_tmp16C=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp141)->f1;_LL6D: return Cyc_Toc_rgn_typ();_LL6E: if(*((int*)_tmp141)!= 15)goto
_LL70;_LL6F: return Cyc_Toc_dyn_rgn_typ();_LL70: if((int)_tmp141 != 2)goto _LL72;
_LL71: {const char*_tmp93C;void*_tmp93B;(_tmp93B=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmp93C="Toc::typ_to_c: type translation passed the heap region",
_tag_dyneither(_tmp93C,sizeof(char),55))),_tag_dyneither(_tmp93B,sizeof(void*),0)));}
_LL72: if((int)_tmp141 != 3)goto _LL74;_LL73: {const char*_tmp93F;void*_tmp93E;(
_tmp93E=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmp93F="Toc::typ_to_c: type translation passed the unique region",
_tag_dyneither(_tmp93F,sizeof(char),57))),_tag_dyneither(_tmp93E,sizeof(void*),0)));}
_LL74: if(_tmp141 <= (void*)4)goto _LL76;if(*((int*)_tmp141)!= 19)goto _LL76;_LL75:
goto _LL77;_LL76: if(_tmp141 <= (void*)4)goto _LL78;if(*((int*)_tmp141)!= 20)goto
_LL78;_LL77: goto _LL79;_LL78: if(_tmp141 <= (void*)4)goto _LL7A;if(*((int*)_tmp141)
!= 21)goto _LL7A;_LL79: {const char*_tmp942;void*_tmp941;(_tmp941=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmp942="Toc::typ_to_c: type translation passed an effect",
_tag_dyneither(_tmp942,sizeof(char),49))),_tag_dyneither(_tmp941,sizeof(void*),0)));}
_LL7A: if(_tmp141 <= (void*)4)goto _LL41;if(*((int*)_tmp141)!= 17)goto _LL41;_LL7B: {
const char*_tmp945;void*_tmp944;(_tmp944=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmp945="Toc::typ_to_c: type translation passed a valueof_t",
_tag_dyneither(_tmp945,sizeof(char),51))),_tag_dyneither(_tmp944,sizeof(void*),0)));}
_LL41:;}static struct Cyc_Absyn_Exp*Cyc_Toc_array_to_ptr_cast(void*t,struct Cyc_Absyn_Exp*
e,struct Cyc_Position_Segment*l);static struct Cyc_Absyn_Exp*Cyc_Toc_array_to_ptr_cast(
void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*l){void*_tmp190=t;struct
Cyc_Absyn_ArrayInfo _tmp191;void*_tmp192;struct Cyc_Absyn_Tqual _tmp193;_LL92: if(
_tmp190 <= (void*)4)goto _LL94;if(*((int*)_tmp190)!= 7)goto _LL94;_tmp191=((struct
Cyc_Absyn_ArrayType_struct*)_tmp190)->f1;_tmp192=_tmp191.elt_type;_tmp193=
_tmp191.tq;_LL93: return Cyc_Toc_cast_it(Cyc_Absyn_star_typ(_tmp192,(void*)2,
_tmp193,Cyc_Absyn_false_conref),e);_LL94:;_LL95: return Cyc_Toc_cast_it(t,e);_LL91:;}
static int Cyc_Toc_atomic_typ(void*t);static int Cyc_Toc_atomic_typ(void*t){void*
_tmp194=Cyc_Tcutil_compress(t);struct Cyc_Absyn_ArrayInfo _tmp195;void*_tmp196;
struct Cyc_Absyn_AggrInfo _tmp197;union Cyc_Absyn_AggrInfoU _tmp198;struct Cyc_List_List*
_tmp199;struct Cyc_Absyn_DatatypeFieldInfo _tmp19A;union Cyc_Absyn_DatatypeFieldInfoU
_tmp19B;struct _tuple2 _tmp19C;struct Cyc_Absyn_Datatypedecl*_tmp19D;struct Cyc_Absyn_Datatypefield*
_tmp19E;struct Cyc_List_List*_tmp19F;_LL97: if((int)_tmp194 != 0)goto _LL99;_LL98:
return 1;_LL99: if(_tmp194 <= (void*)4)goto _LLA1;if(*((int*)_tmp194)!= 1)goto _LL9B;
_LL9A: return 0;_LL9B: if(*((int*)_tmp194)!= 5)goto _LL9D;_LL9C: goto _LL9E;_LL9D: if(*((
int*)_tmp194)!= 12)goto _LL9F;_LL9E: goto _LLA0;_LL9F: if(*((int*)_tmp194)!= 13)goto
_LLA1;_LLA0: goto _LLA2;_LLA1: if((int)_tmp194 != 1)goto _LLA3;_LLA2: goto _LLA4;_LLA3:
if(_tmp194 <= (void*)4)goto _LLB9;if(*((int*)_tmp194)!= 6)goto _LLA5;_LLA4: goto
_LLA6;_LLA5: if(*((int*)_tmp194)!= 8)goto _LLA7;_LLA6: goto _LLA8;_LLA7: if(*((int*)
_tmp194)!= 18)goto _LLA9;_LLA8: return 1;_LLA9: if(*((int*)_tmp194)!= 7)goto _LLAB;
_tmp195=((struct Cyc_Absyn_ArrayType_struct*)_tmp194)->f1;_tmp196=_tmp195.elt_type;
_LLAA: return Cyc_Toc_atomic_typ(_tmp196);_LLAB: if(*((int*)_tmp194)!= 10)goto _LLAD;
_tmp197=((struct Cyc_Absyn_AggrType_struct*)_tmp194)->f1;_tmp198=_tmp197.aggr_info;
_LLAC:{union Cyc_Absyn_AggrInfoU _tmp1A0=_tmp198;struct _tuple3 _tmp1A1;_LLBC: if((
_tmp1A0.UnknownAggr).tag != 1)goto _LLBE;_tmp1A1=(struct _tuple3)(_tmp1A0.UnknownAggr).val;
_LLBD: return 0;_LLBE:;_LLBF: goto _LLBB;_LLBB:;}{struct Cyc_Absyn_Aggrdecl*_tmp1A2=
Cyc_Absyn_get_known_aggrdecl(_tmp198);if(_tmp1A2->impl == 0)return 0;{struct Cyc_List_List*
_tmp1A3=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp1A2->impl))->fields;for(
0;_tmp1A3 != 0;_tmp1A3=_tmp1A3->tl){if(!Cyc_Toc_atomic_typ(((struct Cyc_Absyn_Aggrfield*)
_tmp1A3->hd)->type))return 0;}}return 1;}_LLAD: if(*((int*)_tmp194)!= 11)goto _LLAF;
_tmp199=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp194)->f2;_LLAE: for(0;_tmp199
!= 0;_tmp199=_tmp199->tl){if(!Cyc_Toc_atomic_typ(((struct Cyc_Absyn_Aggrfield*)
_tmp199->hd)->type))return 0;}return 1;_LLAF: if(*((int*)_tmp194)!= 3)goto _LLB1;
_tmp19A=((struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp194)->f1;_tmp19B=_tmp19A.field_info;
if((_tmp19B.KnownDatatypefield).tag != 2)goto _LLB1;_tmp19C=(struct _tuple2)(
_tmp19B.KnownDatatypefield).val;_tmp19D=_tmp19C.f1;_tmp19E=_tmp19C.f2;_LLB0:
_tmp19F=_tmp19E->typs;goto _LLB2;_LLB1: if(*((int*)_tmp194)!= 9)goto _LLB3;_tmp19F=((
struct Cyc_Absyn_TupleType_struct*)_tmp194)->f1;_LLB2: for(0;_tmp19F != 0;_tmp19F=
_tmp19F->tl){if(!Cyc_Toc_atomic_typ((*((struct _tuple10*)_tmp19F->hd)).f2))return
0;}return 1;_LLB3: if(*((int*)_tmp194)!= 2)goto _LLB5;_LLB4: goto _LLB6;_LLB5: if(*((
int*)_tmp194)!= 4)goto _LLB7;_LLB6: goto _LLB8;_LLB7: if(*((int*)_tmp194)!= 14)goto
_LLB9;_LLB8: return 0;_LLB9:;_LLBA: {const char*_tmp949;void*_tmp948[1];struct Cyc_String_pa_struct
_tmp947;(_tmp947.tag=0,((_tmp947.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmp948[0]=& _tmp947,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmp949="atomic_typ:  bad type %s",
_tag_dyneither(_tmp949,sizeof(char),25))),_tag_dyneither(_tmp948,sizeof(void*),1)))))));}
_LL96:;}static int Cyc_Toc_is_void_star(void*t);static int Cyc_Toc_is_void_star(void*
t){void*_tmp1A7=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp1A8;void*
_tmp1A9;_LLC1: if(_tmp1A7 <= (void*)4)goto _LLC3;if(*((int*)_tmp1A7)!= 4)goto _LLC3;
_tmp1A8=((struct Cyc_Absyn_PointerType_struct*)_tmp1A7)->f1;_tmp1A9=_tmp1A8.elt_typ;
_LLC2: {void*_tmp1AA=Cyc_Tcutil_compress(_tmp1A9);_LLC6: if((int)_tmp1AA != 0)goto
_LLC8;_LLC7: return 1;_LLC8:;_LLC9: return 0;_LLC5:;}_LLC3:;_LLC4: return 0;_LLC0:;}
static int Cyc_Toc_is_tvar(void*t);static int Cyc_Toc_is_tvar(void*t){void*_tmp1AB=
Cyc_Tcutil_compress(t);_LLCB: if(_tmp1AB <= (void*)4)goto _LLCD;if(*((int*)_tmp1AB)
!= 1)goto _LLCD;_LLCC: return 1;_LLCD:;_LLCE: return 0;_LLCA:;}static int Cyc_Toc_is_void_star_or_tvar(
void*t);static int Cyc_Toc_is_void_star_or_tvar(void*t){return Cyc_Toc_is_void_star(
t) || Cyc_Toc_is_tvar(t);}static int Cyc_Toc_is_poly_field(void*t,struct
_dyneither_ptr*f);static int Cyc_Toc_is_poly_field(void*t,struct _dyneither_ptr*f){
void*_tmp1AC=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo _tmp1AD;union Cyc_Absyn_AggrInfoU
_tmp1AE;struct Cyc_List_List*_tmp1AF;_LLD0: if(_tmp1AC <= (void*)4)goto _LLD4;if(*((
int*)_tmp1AC)!= 10)goto _LLD2;_tmp1AD=((struct Cyc_Absyn_AggrType_struct*)_tmp1AC)->f1;
_tmp1AE=_tmp1AD.aggr_info;_LLD1: {struct Cyc_Absyn_Aggrdecl*_tmp1B0=Cyc_Absyn_get_known_aggrdecl(
_tmp1AE);if(_tmp1B0->impl == 0){const char*_tmp94C;void*_tmp94B;(_tmp94B=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmp94C="is_poly_field: type missing fields",
_tag_dyneither(_tmp94C,sizeof(char),35))),_tag_dyneither(_tmp94B,sizeof(void*),0)));}
_tmp1AF=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp1B0->impl))->fields;goto
_LLD3;}_LLD2: if(*((int*)_tmp1AC)!= 11)goto _LLD4;_tmp1AF=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp1AC)->f2;_LLD3: {struct Cyc_Absyn_Aggrfield*_tmp1B3=Cyc_Absyn_lookup_field(
_tmp1AF,f);if(_tmp1B3 == 0){const char*_tmp950;void*_tmp94F[1];struct Cyc_String_pa_struct
_tmp94E;(_tmp94E.tag=0,((_tmp94E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*
f),((_tmp94F[0]=& _tmp94E,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Toc_toc_impos)(((_tmp950="is_poly_field: bad field %s",_tag_dyneither(
_tmp950,sizeof(char),28))),_tag_dyneither(_tmp94F,sizeof(void*),1)))))));}return
Cyc_Toc_is_void_star_or_tvar(_tmp1B3->type);}_LLD4:;_LLD5: {const char*_tmp954;
void*_tmp953[1];struct Cyc_String_pa_struct _tmp952;(_tmp952.tag=0,((_tmp952.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp953[
0]=& _tmp952,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmp954="is_poly_field: bad type %s",_tag_dyneither(_tmp954,sizeof(char),27))),
_tag_dyneither(_tmp953,sizeof(void*),1)))))));}_LLCF:;}static int Cyc_Toc_is_poly_project(
struct Cyc_Absyn_Exp*e);static int Cyc_Toc_is_poly_project(struct Cyc_Absyn_Exp*e){
void*_tmp1BA=e->r;struct Cyc_Absyn_Exp*_tmp1BB;struct _dyneither_ptr*_tmp1BC;
struct Cyc_Absyn_Exp*_tmp1BD;struct _dyneither_ptr*_tmp1BE;_LLD7: if(*((int*)
_tmp1BA)!= 23)goto _LLD9;_tmp1BB=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp1BA)->f1;
_tmp1BC=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp1BA)->f2;_LLD8: return Cyc_Toc_is_poly_field((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp1BB->topt))->v,_tmp1BC);_LLD9: if(*((
int*)_tmp1BA)!= 24)goto _LLDB;_tmp1BD=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp1BA)->f1;_tmp1BE=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp1BA)->f2;_LLDA: {
void*_tmp1BF=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp1BD->topt))->v);struct Cyc_Absyn_PtrInfo _tmp1C0;void*_tmp1C1;_LLDE: if(_tmp1BF
<= (void*)4)goto _LLE0;if(*((int*)_tmp1BF)!= 4)goto _LLE0;_tmp1C0=((struct Cyc_Absyn_PointerType_struct*)
_tmp1BF)->f1;_tmp1C1=_tmp1C0.elt_typ;_LLDF: return Cyc_Toc_is_poly_field(_tmp1C1,
_tmp1BE);_LLE0:;_LLE1: {const char*_tmp958;void*_tmp957[1];struct Cyc_String_pa_struct
_tmp956;(_tmp956.tag=0,((_tmp956.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(_tmp1BD->topt))->v)),((
_tmp957[0]=& _tmp956,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmp958="is_poly_project: bad type %s",_tag_dyneither(_tmp958,sizeof(char),29))),
_tag_dyneither(_tmp957,sizeof(void*),1)))))));}_LLDD:;}_LLDB:;_LLDC: return 0;
_LLD6:;}static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_ptr(struct Cyc_Absyn_Exp*s);
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_ptr(struct Cyc_Absyn_Exp*s){struct Cyc_List_List*
_tmp959;return Cyc_Absyn_fncall_exp(Cyc_Toc__cycalloc_e,((_tmp959=_cycalloc(
sizeof(*_tmp959)),((_tmp959->hd=s,((_tmp959->tl=0,_tmp959)))))),0);}static struct
Cyc_Absyn_Exp*Cyc_Toc_malloc_atomic(struct Cyc_Absyn_Exp*s);static struct Cyc_Absyn_Exp*
Cyc_Toc_malloc_atomic(struct Cyc_Absyn_Exp*s){struct Cyc_List_List*_tmp95A;return
Cyc_Absyn_fncall_exp(Cyc_Toc__cycalloc_atomic_e,((_tmp95A=_cycalloc(sizeof(*
_tmp95A)),((_tmp95A->hd=s,((_tmp95A->tl=0,_tmp95A)))))),0);}static struct Cyc_Absyn_Exp*
Cyc_Toc_malloc_exp(void*t,struct Cyc_Absyn_Exp*s);static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_exp(
void*t,struct Cyc_Absyn_Exp*s){if(Cyc_Toc_atomic_typ(t))return Cyc_Toc_malloc_atomic(
s);return Cyc_Toc_malloc_ptr(s);}static struct Cyc_Absyn_Exp*Cyc_Toc_rmalloc_exp(
struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s);static struct Cyc_Absyn_Exp*Cyc_Toc_rmalloc_exp(
struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s){struct Cyc_Absyn_Exp*_tmp95B[2];
return Cyc_Absyn_fncall_exp(Cyc_Toc__region_malloc_e,((_tmp95B[1]=s,((_tmp95B[0]=
rgn,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmp95B,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}static struct Cyc_Absyn_Exp*
Cyc_Toc_calloc_exp(void*elt_type,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n);
static struct Cyc_Absyn_Exp*Cyc_Toc_calloc_exp(void*elt_type,struct Cyc_Absyn_Exp*s,
struct Cyc_Absyn_Exp*n){if(Cyc_Toc_atomic_typ(elt_type)){struct Cyc_Absyn_Exp*
_tmp95C[2];return Cyc_Absyn_fncall_exp(Cyc_Toc__cyccalloc_atomic_e,((_tmp95C[1]=n,((
_tmp95C[0]=s,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmp95C,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}else{struct Cyc_Absyn_Exp*
_tmp95D[2];return Cyc_Absyn_fncall_exp(Cyc_Toc__cyccalloc_e,((_tmp95D[1]=n,((
_tmp95D[0]=s,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmp95D,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}}static struct Cyc_Absyn_Exp*
Cyc_Toc_rcalloc_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*
n);static struct Cyc_Absyn_Exp*Cyc_Toc_rcalloc_exp(struct Cyc_Absyn_Exp*rgn,struct
Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n){struct Cyc_Absyn_Exp*_tmp95E[3];return Cyc_Absyn_fncall_exp(
Cyc_Toc__region_calloc_e,((_tmp95E[2]=n,((_tmp95E[1]=s,((_tmp95E[0]=rgn,((struct
Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp95E,
sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);}static struct Cyc_Absyn_Exp*Cyc_Toc_newthrow_exp(
struct Cyc_Absyn_Exp*e);static struct Cyc_Absyn_Exp*Cyc_Toc_newthrow_exp(struct Cyc_Absyn_Exp*
e){struct Cyc_List_List*_tmp95F;return Cyc_Absyn_fncall_exp(Cyc_Toc__throw_e,((
_tmp95F=_cycalloc(sizeof(*_tmp95F)),((_tmp95F->hd=e,((_tmp95F->tl=0,_tmp95F)))))),
0);}static struct Cyc_Absyn_Stmt*Cyc_Toc_throw_match_stmt();static struct Cyc_Absyn_Stmt*
Cyc_Toc_throw_match_stmt(){static struct Cyc_Absyn_Stmt**throw_match_stmt_opt=0;
if(throw_match_stmt_opt == 0){struct Cyc_Absyn_Stmt**_tmp960;throw_match_stmt_opt=((
_tmp960=_cycalloc(sizeof(*_tmp960)),((_tmp960[0]=Cyc_Absyn_exp_stmt(Cyc_Toc_newthrow_exp(
Cyc_Absyn_match_exn_exp(0)),0),_tmp960))));}return*throw_match_stmt_opt;}static
struct Cyc_Absyn_Exp*Cyc_Toc_make_toplevel_dyn_arr(void*t,struct Cyc_Absyn_Exp*sz,
struct Cyc_Absyn_Exp*e);static struct Cyc_Absyn_Exp*Cyc_Toc_make_toplevel_dyn_arr(
void*t,struct Cyc_Absyn_Exp*sz,struct Cyc_Absyn_Exp*e){int is_string=0;{void*
_tmp1CD=e->r;union Cyc_Absyn_Cnst _tmp1CE;struct _dyneither_ptr _tmp1CF;_LLE3: if(*((
int*)_tmp1CD)!= 0)goto _LLE5;_tmp1CE=((struct Cyc_Absyn_Const_e_struct*)_tmp1CD)->f1;
if((_tmp1CE.String_c).tag != 7)goto _LLE5;_tmp1CF=(struct _dyneither_ptr)(_tmp1CE.String_c).val;
_LLE4: is_string=1;goto _LLE2;_LLE5:;_LLE6: goto _LLE2;_LLE2:;}{struct Cyc_Absyn_Exp*
xexp;struct Cyc_Absyn_Exp*xplussz;if(is_string){struct _tuple1*x=Cyc_Toc_temp_var();
void*vd_typ=Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq,(struct Cyc_Absyn_Exp*)
sz,Cyc_Absyn_false_conref,0);struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_static_vardecl(
x,vd_typ,(struct Cyc_Absyn_Exp*)e);{struct Cyc_Absyn_Var_d_struct*_tmp966;struct
Cyc_Absyn_Var_d_struct _tmp965;struct Cyc_List_List*_tmp964;Cyc_Toc_result_decls=((
_tmp964=_cycalloc(sizeof(*_tmp964)),((_tmp964->hd=Cyc_Absyn_new_decl((void*)((
_tmp966=_cycalloc(sizeof(*_tmp966)),((_tmp966[0]=((_tmp965.tag=0,((_tmp965.f1=vd,
_tmp965)))),_tmp966)))),0),((_tmp964->tl=Cyc_Toc_result_decls,_tmp964))))));}
xexp=Cyc_Absyn_var_exp(x,0);xplussz=Cyc_Absyn_add_exp(xexp,sz,0);}else{xexp=Cyc_Toc_cast_it(
Cyc_Absyn_void_star_typ(),e);xplussz=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),
Cyc_Absyn_add_exp(e,sz,0));}{struct Cyc_Absyn_Exp*urm_exp;{struct _tuple16*_tmp96D;
struct _tuple16*_tmp96C;struct _tuple16*_tmp96B;struct _tuple16*_tmp96A[3];urm_exp=
Cyc_Absyn_unresolvedmem_exp(0,((_tmp96A[2]=((_tmp96D=_cycalloc(sizeof(*_tmp96D)),((
_tmp96D->f1=0,((_tmp96D->f2=xplussz,_tmp96D)))))),((_tmp96A[1]=((_tmp96C=
_cycalloc(sizeof(*_tmp96C)),((_tmp96C->f1=0,((_tmp96C->f2=xexp,_tmp96C)))))),((
_tmp96A[0]=((_tmp96B=_cycalloc(sizeof(*_tmp96B)),((_tmp96B->f1=0,((_tmp96B->f2=
xexp,_tmp96B)))))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmp96A,sizeof(struct _tuple16*),3)))))))),0);}return urm_exp;}}}
struct Cyc_Toc_FallthruInfo{struct _dyneither_ptr*label;struct Cyc_List_List*
binders;struct Cyc_Dict_Dict next_case_env;};struct Cyc_Toc_Env{struct
_dyneither_ptr**break_lab;struct _dyneither_ptr**continue_lab;struct Cyc_Toc_FallthruInfo*
fallthru_info;struct Cyc_Dict_Dict varmap;int toplevel;struct _RegionHandle*rgn;};
static int Cyc_Toc_is_toplevel(struct Cyc_Toc_Env*nv);static int Cyc_Toc_is_toplevel(
struct Cyc_Toc_Env*nv){struct Cyc_Toc_Env _tmp1D8;int _tmp1D9;struct Cyc_Toc_Env*
_tmp1D7=nv;_tmp1D8=*_tmp1D7;_tmp1D9=_tmp1D8.toplevel;return _tmp1D9;}static struct
Cyc_Absyn_Exp*Cyc_Toc_lookup_varmap(struct Cyc_Toc_Env*nv,struct _tuple1*x);static
struct Cyc_Absyn_Exp*Cyc_Toc_lookup_varmap(struct Cyc_Toc_Env*nv,struct _tuple1*x){
struct Cyc_Toc_Env _tmp1DB;struct Cyc_Dict_Dict _tmp1DC;struct Cyc_Toc_Env*_tmp1DA=nv;
_tmp1DB=*_tmp1DA;_tmp1DC=_tmp1DB.varmap;return((struct Cyc_Absyn_Exp*(*)(struct
Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup)(_tmp1DC,x);}static struct Cyc_Toc_Env*
Cyc_Toc_empty_env(struct _RegionHandle*r);static struct Cyc_Toc_Env*Cyc_Toc_empty_env(
struct _RegionHandle*r){struct Cyc_Toc_Env*_tmp96E;return(_tmp96E=_region_malloc(r,
sizeof(*_tmp96E)),((_tmp96E->break_lab=(struct _dyneither_ptr**)0,((_tmp96E->continue_lab=(
struct _dyneither_ptr**)0,((_tmp96E->fallthru_info=(struct Cyc_Toc_FallthruInfo*)0,((
_tmp96E->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_rempty)(r,Cyc_Absyn_qvar_cmp),((
_tmp96E->toplevel=(int)1,((_tmp96E->rgn=(struct _RegionHandle*)r,_tmp96E)))))))))))));}
static struct Cyc_Toc_Env*Cyc_Toc_share_env(struct _RegionHandle*r,struct Cyc_Toc_Env*
e);static struct Cyc_Toc_Env*Cyc_Toc_share_env(struct _RegionHandle*r,struct Cyc_Toc_Env*
e){struct Cyc_Toc_Env _tmp1DF;struct _dyneither_ptr**_tmp1E0;struct _dyneither_ptr**
_tmp1E1;struct Cyc_Toc_FallthruInfo*_tmp1E2;struct Cyc_Dict_Dict _tmp1E3;int _tmp1E4;
struct Cyc_Toc_Env*_tmp1DE=e;_tmp1DF=*_tmp1DE;_tmp1E0=_tmp1DF.break_lab;_tmp1E1=
_tmp1DF.continue_lab;_tmp1E2=_tmp1DF.fallthru_info;_tmp1E3=_tmp1DF.varmap;
_tmp1E4=_tmp1DF.toplevel;{struct Cyc_Toc_Env*_tmp96F;return(_tmp96F=
_region_malloc(r,sizeof(*_tmp96F)),((_tmp96F->break_lab=(struct _dyneither_ptr**)((
struct _dyneither_ptr**)_tmp1E0),((_tmp96F->continue_lab=(struct _dyneither_ptr**)((
struct _dyneither_ptr**)_tmp1E1),((_tmp96F->fallthru_info=(struct Cyc_Toc_FallthruInfo*)
_tmp1E2,((_tmp96F->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp1E3),((_tmp96F->toplevel=(
int)_tmp1E4,((_tmp96F->rgn=(struct _RegionHandle*)r,_tmp96F)))))))))))));}}static
struct Cyc_Toc_Env*Cyc_Toc_clear_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*
e);static struct Cyc_Toc_Env*Cyc_Toc_clear_toplevel(struct _RegionHandle*r,struct
Cyc_Toc_Env*e){struct Cyc_Toc_Env _tmp1E7;struct _dyneither_ptr**_tmp1E8;struct
_dyneither_ptr**_tmp1E9;struct Cyc_Toc_FallthruInfo*_tmp1EA;struct Cyc_Dict_Dict
_tmp1EB;int _tmp1EC;struct Cyc_Toc_Env*_tmp1E6=e;_tmp1E7=*_tmp1E6;_tmp1E8=_tmp1E7.break_lab;
_tmp1E9=_tmp1E7.continue_lab;_tmp1EA=_tmp1E7.fallthru_info;_tmp1EB=_tmp1E7.varmap;
_tmp1EC=_tmp1E7.toplevel;{struct Cyc_Toc_Env*_tmp970;return(_tmp970=
_region_malloc(r,sizeof(*_tmp970)),((_tmp970->break_lab=(struct _dyneither_ptr**)((
struct _dyneither_ptr**)_tmp1E8),((_tmp970->continue_lab=(struct _dyneither_ptr**)((
struct _dyneither_ptr**)_tmp1E9),((_tmp970->fallthru_info=(struct Cyc_Toc_FallthruInfo*)
_tmp1EA,((_tmp970->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp1EB),((_tmp970->toplevel=(
int)0,((_tmp970->rgn=(struct _RegionHandle*)r,_tmp970)))))))))))));}}static struct
Cyc_Toc_Env*Cyc_Toc_set_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*e);
static struct Cyc_Toc_Env*Cyc_Toc_set_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*
e){struct Cyc_Toc_Env _tmp1EF;struct _dyneither_ptr**_tmp1F0;struct _dyneither_ptr**
_tmp1F1;struct Cyc_Toc_FallthruInfo*_tmp1F2;struct Cyc_Dict_Dict _tmp1F3;int _tmp1F4;
struct Cyc_Toc_Env*_tmp1EE=e;_tmp1EF=*_tmp1EE;_tmp1F0=_tmp1EF.break_lab;_tmp1F1=
_tmp1EF.continue_lab;_tmp1F2=_tmp1EF.fallthru_info;_tmp1F3=_tmp1EF.varmap;
_tmp1F4=_tmp1EF.toplevel;{struct Cyc_Toc_Env*_tmp971;return(_tmp971=
_region_malloc(r,sizeof(*_tmp971)),((_tmp971->break_lab=(struct _dyneither_ptr**)((
struct _dyneither_ptr**)_tmp1F0),((_tmp971->continue_lab=(struct _dyneither_ptr**)((
struct _dyneither_ptr**)_tmp1F1),((_tmp971->fallthru_info=(struct Cyc_Toc_FallthruInfo*)
_tmp1F2,((_tmp971->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp1F3),((_tmp971->toplevel=(
int)1,((_tmp971->rgn=(struct _RegionHandle*)r,_tmp971)))))))))))));}}static struct
Cyc_Toc_Env*Cyc_Toc_add_varmap(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct
_tuple1*x,struct Cyc_Absyn_Exp*y);static struct Cyc_Toc_Env*Cyc_Toc_add_varmap(
struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _tuple1*x,struct Cyc_Absyn_Exp*y){{
union Cyc_Absyn_Nmspace _tmp1F6=(*x).f1;struct Cyc_List_List*_tmp1F7;_LLE8: if((
_tmp1F6.Rel_n).tag != 1)goto _LLEA;_tmp1F7=(struct Cyc_List_List*)(_tmp1F6.Rel_n).val;
_LLE9: {const char*_tmp975;void*_tmp974[1];struct Cyc_String_pa_struct _tmp973;(
_tmp973.tag=0,((_tmp973.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
x)),((_tmp974[0]=& _tmp973,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap))Cyc_Toc_toc_impos)(((_tmp975="Toc::add_varmap on Rel_n: %s\n",_tag_dyneither(
_tmp975,sizeof(char),30))),_tag_dyneither(_tmp974,sizeof(void*),1)))))));}_LLEA:;
_LLEB: goto _LLE7;_LLE7:;}{struct Cyc_Toc_Env _tmp1FC;struct _dyneither_ptr**_tmp1FD;
struct _dyneither_ptr**_tmp1FE;struct Cyc_Toc_FallthruInfo*_tmp1FF;struct Cyc_Dict_Dict
_tmp200;int _tmp201;struct Cyc_Toc_Env*_tmp1FB=e;_tmp1FC=*_tmp1FB;_tmp1FD=_tmp1FC.break_lab;
_tmp1FE=_tmp1FC.continue_lab;_tmp1FF=_tmp1FC.fallthru_info;_tmp200=_tmp1FC.varmap;
_tmp201=_tmp1FC.toplevel;{struct Cyc_Dict_Dict _tmp202=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,struct _tuple1*k,struct Cyc_Absyn_Exp*v))Cyc_Dict_insert)(((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(
r,_tmp200),x,y);struct Cyc_Toc_Env*_tmp976;return(_tmp976=_region_malloc(r,
sizeof(*_tmp976)),((_tmp976->break_lab=(struct _dyneither_ptr**)((struct
_dyneither_ptr**)_tmp1FD),((_tmp976->continue_lab=(struct _dyneither_ptr**)((
struct _dyneither_ptr**)_tmp1FE),((_tmp976->fallthru_info=(struct Cyc_Toc_FallthruInfo*)
_tmp1FF,((_tmp976->varmap=(struct Cyc_Dict_Dict)_tmp202,((_tmp976->toplevel=(int)
_tmp201,((_tmp976->rgn=(struct _RegionHandle*)r,_tmp976)))))))))))));}}}static
struct Cyc_Toc_Env*Cyc_Toc_loop_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e);
static struct Cyc_Toc_Env*Cyc_Toc_loop_env(struct _RegionHandle*r,struct Cyc_Toc_Env*
e){struct Cyc_Toc_Env _tmp205;struct _dyneither_ptr**_tmp206;struct _dyneither_ptr**
_tmp207;struct Cyc_Toc_FallthruInfo*_tmp208;struct Cyc_Dict_Dict _tmp209;int _tmp20A;
struct Cyc_Toc_Env*_tmp204=e;_tmp205=*_tmp204;_tmp206=_tmp205.break_lab;_tmp207=
_tmp205.continue_lab;_tmp208=_tmp205.fallthru_info;_tmp209=_tmp205.varmap;
_tmp20A=_tmp205.toplevel;{struct Cyc_Toc_Env*_tmp977;return(_tmp977=
_region_malloc(r,sizeof(*_tmp977)),((_tmp977->break_lab=(struct _dyneither_ptr**)
0,((_tmp977->continue_lab=(struct _dyneither_ptr**)0,((_tmp977->fallthru_info=(
struct Cyc_Toc_FallthruInfo*)_tmp208,((_tmp977->varmap=(struct Cyc_Dict_Dict)((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(
r,_tmp209),((_tmp977->toplevel=(int)_tmp20A,((_tmp977->rgn=(struct _RegionHandle*)
r,_tmp977)))))))))))));}}static struct Cyc_Toc_Env*Cyc_Toc_non_last_switchclause_env(
struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*break_l,struct
_dyneither_ptr*fallthru_l,struct Cyc_List_List*fallthru_binders,struct Cyc_Toc_Env*
next_case_env);static struct Cyc_Toc_Env*Cyc_Toc_non_last_switchclause_env(struct
_RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*break_l,struct
_dyneither_ptr*fallthru_l,struct Cyc_List_List*fallthru_binders,struct Cyc_Toc_Env*
next_case_env){struct Cyc_List_List*fallthru_vars=0;for(0;fallthru_binders != 0;
fallthru_binders=fallthru_binders->tl){struct Cyc_List_List*_tmp978;fallthru_vars=((
_tmp978=_region_malloc(r,sizeof(*_tmp978)),((_tmp978->hd=((struct Cyc_Absyn_Vardecl*)
fallthru_binders->hd)->name,((_tmp978->tl=fallthru_vars,_tmp978))))));}
fallthru_vars=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
fallthru_vars);{struct Cyc_Toc_Env _tmp20E;struct _dyneither_ptr**_tmp20F;struct
_dyneither_ptr**_tmp210;struct Cyc_Toc_FallthruInfo*_tmp211;struct Cyc_Dict_Dict
_tmp212;int _tmp213;struct Cyc_Toc_Env*_tmp20D=e;_tmp20E=*_tmp20D;_tmp20F=_tmp20E.break_lab;
_tmp210=_tmp20E.continue_lab;_tmp211=_tmp20E.fallthru_info;_tmp212=_tmp20E.varmap;
_tmp213=_tmp20E.toplevel;{struct Cyc_Toc_Env _tmp215;struct Cyc_Dict_Dict _tmp216;
struct Cyc_Toc_Env*_tmp214=next_case_env;_tmp215=*_tmp214;_tmp216=_tmp215.varmap;{
struct Cyc_Toc_FallthruInfo*_tmp979;struct Cyc_Toc_FallthruInfo*fi=(_tmp979=
_region_malloc(r,sizeof(*_tmp979)),((_tmp979->label=fallthru_l,((_tmp979->binders=
fallthru_vars,((_tmp979->next_case_env=((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp216),_tmp979)))))));
struct _dyneither_ptr**_tmp97C;struct Cyc_Toc_Env*_tmp97B;return(_tmp97B=
_region_malloc(r,sizeof(*_tmp97B)),((_tmp97B->break_lab=(struct _dyneither_ptr**)((
_tmp97C=_region_malloc(r,sizeof(*_tmp97C)),((_tmp97C[0]=break_l,_tmp97C)))),((
_tmp97B->continue_lab=(struct _dyneither_ptr**)((struct _dyneither_ptr**)_tmp210),((
_tmp97B->fallthru_info=(struct Cyc_Toc_FallthruInfo*)fi,((_tmp97B->varmap=(struct
Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))
Cyc_Dict_rshare)(r,_tmp212),((_tmp97B->toplevel=(int)_tmp213,((_tmp97B->rgn=(
struct _RegionHandle*)r,_tmp97B)))))))))))));}}}}static struct Cyc_Toc_Env*Cyc_Toc_last_switchclause_env(
struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*break_l);static
struct Cyc_Toc_Env*Cyc_Toc_last_switchclause_env(struct _RegionHandle*r,struct Cyc_Toc_Env*
e,struct _dyneither_ptr*break_l){struct Cyc_Toc_Env _tmp21B;struct _dyneither_ptr**
_tmp21C;struct _dyneither_ptr**_tmp21D;struct Cyc_Toc_FallthruInfo*_tmp21E;struct
Cyc_Dict_Dict _tmp21F;int _tmp220;struct Cyc_Toc_Env*_tmp21A=e;_tmp21B=*_tmp21A;
_tmp21C=_tmp21B.break_lab;_tmp21D=_tmp21B.continue_lab;_tmp21E=_tmp21B.fallthru_info;
_tmp21F=_tmp21B.varmap;_tmp220=_tmp21B.toplevel;{struct _dyneither_ptr**_tmp97F;
struct Cyc_Toc_Env*_tmp97E;return(_tmp97E=_region_malloc(r,sizeof(*_tmp97E)),((
_tmp97E->break_lab=(struct _dyneither_ptr**)((_tmp97F=_region_malloc(r,sizeof(*
_tmp97F)),((_tmp97F[0]=break_l,_tmp97F)))),((_tmp97E->continue_lab=(struct
_dyneither_ptr**)((struct _dyneither_ptr**)_tmp21D),((_tmp97E->fallthru_info=(
struct Cyc_Toc_FallthruInfo*)0,((_tmp97E->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(
struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp21F),((_tmp97E->toplevel=(
int)_tmp220,((_tmp97E->rgn=(struct _RegionHandle*)r,_tmp97E)))))))))))));}}static
struct Cyc_Toc_Env*Cyc_Toc_switch_as_switch_env(struct _RegionHandle*r,struct Cyc_Toc_Env*
e,struct _dyneither_ptr*next_l);static struct Cyc_Toc_Env*Cyc_Toc_switch_as_switch_env(
struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*next_l){struct Cyc_Toc_Env
_tmp224;struct _dyneither_ptr**_tmp225;struct _dyneither_ptr**_tmp226;struct Cyc_Toc_FallthruInfo*
_tmp227;struct Cyc_Dict_Dict _tmp228;int _tmp229;struct Cyc_Toc_Env*_tmp223=e;
_tmp224=*_tmp223;_tmp225=_tmp224.break_lab;_tmp226=_tmp224.continue_lab;_tmp227=
_tmp224.fallthru_info;_tmp228=_tmp224.varmap;_tmp229=_tmp224.toplevel;{struct Cyc_Toc_FallthruInfo*
_tmp982;struct Cyc_Toc_Env*_tmp981;return(_tmp981=_region_malloc(r,sizeof(*
_tmp981)),((_tmp981->break_lab=(struct _dyneither_ptr**)0,((_tmp981->continue_lab=(
struct _dyneither_ptr**)((struct _dyneither_ptr**)_tmp226),((_tmp981->fallthru_info=(
struct Cyc_Toc_FallthruInfo*)((_tmp982=_region_malloc(r,sizeof(*_tmp982)),((
_tmp982->label=next_l,((_tmp982->binders=0,((_tmp982->next_case_env=((struct Cyc_Dict_Dict(*)(
struct _RegionHandle*,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_rempty)(
r,Cyc_Absyn_qvar_cmp),_tmp982)))))))),((_tmp981->varmap=(struct Cyc_Dict_Dict)((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(
r,_tmp228),((_tmp981->toplevel=(int)_tmp229,((_tmp981->rgn=(struct _RegionHandle*)
r,_tmp981)))))))))))));}}static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct
Cyc_Absyn_Exp*e);static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*
s);static int Cyc_Toc_need_null_check(struct Cyc_Absyn_Exp*e);static int Cyc_Toc_need_null_check(
struct Cyc_Absyn_Exp*e){void*_tmp22C=e->annot;_LLED: if(*((void**)_tmp22C)!= Cyc_CfFlowInfo_UnknownZ)
goto _LLEF;_LLEE: return Cyc_Toc_is_nullable((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);_LLEF: if(*((void**)_tmp22C)!= Cyc_CfFlowInfo_NotZero)
goto _LLF1;_LLF0: return 0;_LLF1: if(_tmp22C != Cyc_CfFlowInfo_IsZero)goto _LLF3;_LLF2:{
const char*_tmp985;void*_tmp984;(_tmp984=0,Cyc_Tcutil_terr(e->loc,((_tmp985="dereference of NULL pointer",
_tag_dyneither(_tmp985,sizeof(char),28))),_tag_dyneither(_tmp984,sizeof(void*),0)));}
return 0;_LLF3: if(_tmp22C != Cyc_Absyn_EmptyAnnot)goto _LLF5;_LLF4: return 0;_LLF5:
if(*((void**)_tmp22C)!= Cyc_CfFlowInfo_HasTagCmps)goto _LLF7;_LLF6:{const char*
_tmp988;void*_tmp987;(_tmp987=0,Cyc_Tcutil_warn(e->loc,((_tmp988="compiler oddity: pointer compared to tag type",
_tag_dyneither(_tmp988,sizeof(char),46))),_tag_dyneither(_tmp987,sizeof(void*),0)));}
return Cyc_Toc_is_nullable((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
_LLF7:;_LLF8: {const char*_tmp98B;void*_tmp98A;(_tmp98A=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmp98B="need_null_check",
_tag_dyneither(_tmp98B,sizeof(char),16))),_tag_dyneither(_tmp98A,sizeof(void*),0)));}
_LLEC:;}static struct Cyc_List_List*Cyc_Toc_get_relns(struct Cyc_Absyn_Exp*e);
static struct Cyc_List_List*Cyc_Toc_get_relns(struct Cyc_Absyn_Exp*e){void*_tmp233=
e->annot;struct Cyc_List_List*_tmp234;struct Cyc_List_List*_tmp235;_LLFA: if(*((
void**)_tmp233)!= Cyc_CfFlowInfo_UnknownZ)goto _LLFC;_tmp234=((struct Cyc_CfFlowInfo_UnknownZ_struct*)
_tmp233)->f1;_LLFB: return _tmp234;_LLFC: if(*((void**)_tmp233)!= Cyc_CfFlowInfo_NotZero)
goto _LLFE;_tmp235=((struct Cyc_CfFlowInfo_NotZero_struct*)_tmp233)->f1;_LLFD:
return _tmp235;_LLFE: if(_tmp233 != Cyc_CfFlowInfo_IsZero)goto _LL100;_LLFF:{const
char*_tmp98E;void*_tmp98D;(_tmp98D=0,Cyc_Tcutil_terr(e->loc,((_tmp98E="dereference of NULL pointer",
_tag_dyneither(_tmp98E,sizeof(char),28))),_tag_dyneither(_tmp98D,sizeof(void*),0)));}
return 0;_LL100: if(*((void**)_tmp233)!= Cyc_CfFlowInfo_HasTagCmps)goto _LL102;
_LL101: goto _LL103;_LL102: if(_tmp233 != Cyc_Absyn_EmptyAnnot)goto _LL104;_LL103:
return 0;_LL104:;_LL105: {const char*_tmp991;void*_tmp990;(_tmp990=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmp991="get_relns",
_tag_dyneither(_tmp991,sizeof(char),10))),_tag_dyneither(_tmp990,sizeof(void*),0)));}
_LLF9:;}static int Cyc_Toc_check_const_array(unsigned int i,void*t);static int Cyc_Toc_check_const_array(
unsigned int i,void*t){void*_tmp23A=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo
_tmp23B;struct Cyc_Absyn_PtrAtts _tmp23C;union Cyc_Absyn_Constraint*_tmp23D;union
Cyc_Absyn_Constraint*_tmp23E;struct Cyc_Absyn_ArrayInfo _tmp23F;struct Cyc_Absyn_Exp*
_tmp240;_LL107: if(_tmp23A <= (void*)4)goto _LL10B;if(*((int*)_tmp23A)!= 4)goto
_LL109;_tmp23B=((struct Cyc_Absyn_PointerType_struct*)_tmp23A)->f1;_tmp23C=
_tmp23B.ptr_atts;_tmp23D=_tmp23C.bounds;_tmp23E=_tmp23C.zero_term;_LL108: {void*
_tmp241=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp23D);struct Cyc_Absyn_Exp*
_tmp242;_LL10E: if((int)_tmp241 != 0)goto _LL110;_LL10F: return 0;_LL110: if(_tmp241 <= (
void*)1)goto _LL10D;if(*((int*)_tmp241)!= 0)goto _LL10D;_tmp242=((struct Cyc_Absyn_Upper_b_struct*)
_tmp241)->f1;_LL111: {unsigned int _tmp244;int _tmp245;struct _tuple11 _tmp243=Cyc_Evexp_eval_const_uint_exp(
_tmp242);_tmp244=_tmp243.f1;_tmp245=_tmp243.f2;return _tmp245  && i <= _tmp244;}
_LL10D:;}_LL109: if(*((int*)_tmp23A)!= 7)goto _LL10B;_tmp23F=((struct Cyc_Absyn_ArrayType_struct*)
_tmp23A)->f1;_tmp240=_tmp23F.num_elts;_LL10A: if(_tmp240 == 0)return 0;{
unsigned int _tmp247;int _tmp248;struct _tuple11 _tmp246=Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)_tmp240);_tmp247=_tmp246.f1;_tmp248=_tmp246.f2;return
_tmp248  && i <= _tmp247;}_LL10B:;_LL10C: return 0;_LL106:;}static int Cyc_Toc_check_leq_size_var(
struct Cyc_List_List*relns,struct Cyc_Absyn_Vardecl*v,struct Cyc_Absyn_Vardecl*y);
static int Cyc_Toc_check_leq_size_var(struct Cyc_List_List*relns,struct Cyc_Absyn_Vardecl*
v,struct Cyc_Absyn_Vardecl*y){for(0;relns != 0;relns=relns->tl){struct Cyc_CfFlowInfo_Reln*
_tmp249=(struct Cyc_CfFlowInfo_Reln*)relns->hd;if(_tmp249->vd != y)continue;{union
Cyc_CfFlowInfo_RelnOp _tmp24A=_tmp249->rop;struct Cyc_Absyn_Vardecl*_tmp24B;struct
Cyc_Absyn_Vardecl*_tmp24C;_LL113: if((_tmp24A.LessNumelts).tag != 3)goto _LL115;
_tmp24B=(struct Cyc_Absyn_Vardecl*)(_tmp24A.LessNumelts).val;_LL114: _tmp24C=
_tmp24B;goto _LL116;_LL115: if((_tmp24A.LessEqNumelts).tag != 5)goto _LL117;_tmp24C=(
struct Cyc_Absyn_Vardecl*)(_tmp24A.LessEqNumelts).val;_LL116: if(_tmp24C == v)
return 1;else{goto _LL112;}_LL117:;_LL118: continue;_LL112:;}}return 0;}static int Cyc_Toc_check_leq_size(
struct Cyc_List_List*relns,struct Cyc_Absyn_Vardecl*v,struct Cyc_Absyn_Exp*e);
static int Cyc_Toc_check_leq_size(struct Cyc_List_List*relns,struct Cyc_Absyn_Vardecl*
v,struct Cyc_Absyn_Exp*e){{void*_tmp24D=e->r;void*_tmp24E;struct Cyc_Absyn_Vardecl*
_tmp24F;void*_tmp250;struct Cyc_Absyn_Vardecl*_tmp251;void*_tmp252;struct Cyc_Absyn_Vardecl*
_tmp253;void*_tmp254;struct Cyc_Absyn_Vardecl*_tmp255;void*_tmp256;struct Cyc_List_List*
_tmp257;struct Cyc_List_List _tmp258;struct Cyc_Absyn_Exp*_tmp259;_LL11A: if(*((int*)
_tmp24D)!= 1)goto _LL11C;_tmp24E=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp24D)->f2;
if(_tmp24E <= (void*)1)goto _LL11C;if(*((int*)_tmp24E)!= 4)goto _LL11C;_tmp24F=((
struct Cyc_Absyn_Pat_b_struct*)_tmp24E)->f1;_LL11B: _tmp251=_tmp24F;goto _LL11D;
_LL11C: if(*((int*)_tmp24D)!= 1)goto _LL11E;_tmp250=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp24D)->f2;if(_tmp250 <= (void*)1)goto _LL11E;if(*((int*)_tmp250)!= 3)goto _LL11E;
_tmp251=((struct Cyc_Absyn_Local_b_struct*)_tmp250)->f1;_LL11D: _tmp253=_tmp251;
goto _LL11F;_LL11E: if(*((int*)_tmp24D)!= 1)goto _LL120;_tmp252=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp24D)->f2;if(_tmp252 <= (void*)1)goto _LL120;if(*((int*)_tmp252)!= 0)goto _LL120;
_tmp253=((struct Cyc_Absyn_Global_b_struct*)_tmp252)->f1;_LL11F: _tmp255=_tmp253;
goto _LL121;_LL120: if(*((int*)_tmp24D)!= 1)goto _LL122;_tmp254=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp24D)->f2;if(_tmp254 <= (void*)1)goto _LL122;if(*((int*)_tmp254)!= 2)goto _LL122;
_tmp255=((struct Cyc_Absyn_Param_b_struct*)_tmp254)->f1;_LL121: if(_tmp255->escapes)
return 0;if(Cyc_Toc_check_leq_size_var(relns,v,_tmp255))return 1;goto _LL119;_LL122:
if(*((int*)_tmp24D)!= 3)goto _LL124;_tmp256=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp24D)->f1;if((int)_tmp256 != 19)goto _LL124;_tmp257=((struct Cyc_Absyn_Primop_e_struct*)
_tmp24D)->f2;if(_tmp257 == 0)goto _LL124;_tmp258=*_tmp257;_tmp259=(struct Cyc_Absyn_Exp*)
_tmp258.hd;_LL123:{void*_tmp25A=_tmp259->r;void*_tmp25B;struct Cyc_Absyn_Vardecl*
_tmp25C;void*_tmp25D;struct Cyc_Absyn_Vardecl*_tmp25E;void*_tmp25F;struct Cyc_Absyn_Vardecl*
_tmp260;void*_tmp261;struct Cyc_Absyn_Vardecl*_tmp262;_LL127: if(*((int*)_tmp25A)
!= 1)goto _LL129;_tmp25B=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp25A)->f2;if(
_tmp25B <= (void*)1)goto _LL129;if(*((int*)_tmp25B)!= 4)goto _LL129;_tmp25C=((
struct Cyc_Absyn_Pat_b_struct*)_tmp25B)->f1;_LL128: _tmp25E=_tmp25C;goto _LL12A;
_LL129: if(*((int*)_tmp25A)!= 1)goto _LL12B;_tmp25D=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp25A)->f2;if(_tmp25D <= (void*)1)goto _LL12B;if(*((int*)_tmp25D)!= 3)goto _LL12B;
_tmp25E=((struct Cyc_Absyn_Local_b_struct*)_tmp25D)->f1;_LL12A: _tmp260=_tmp25E;
goto _LL12C;_LL12B: if(*((int*)_tmp25A)!= 1)goto _LL12D;_tmp25F=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp25A)->f2;if(_tmp25F <= (void*)1)goto _LL12D;if(*((int*)_tmp25F)!= 0)goto _LL12D;
_tmp260=((struct Cyc_Absyn_Global_b_struct*)_tmp25F)->f1;_LL12C: _tmp262=_tmp260;
goto _LL12E;_LL12D: if(*((int*)_tmp25A)!= 1)goto _LL12F;_tmp261=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp25A)->f2;if(_tmp261 <= (void*)1)goto _LL12F;if(*((int*)_tmp261)!= 2)goto _LL12F;
_tmp262=((struct Cyc_Absyn_Param_b_struct*)_tmp261)->f1;_LL12E: return _tmp262 == v;
_LL12F:;_LL130: goto _LL126;_LL126:;}goto _LL119;_LL124:;_LL125: goto _LL119;_LL119:;}
return 0;}static int Cyc_Toc_check_bounds(struct Cyc_List_List*relns,struct Cyc_Absyn_Exp*
a,struct Cyc_Absyn_Exp*i);static int Cyc_Toc_check_bounds(struct Cyc_List_List*relns,
struct Cyc_Absyn_Exp*a,struct Cyc_Absyn_Exp*i){{void*_tmp263=a->r;void*_tmp264;
struct Cyc_Absyn_Vardecl*_tmp265;void*_tmp266;struct Cyc_Absyn_Vardecl*_tmp267;
void*_tmp268;struct Cyc_Absyn_Vardecl*_tmp269;void*_tmp26A;struct Cyc_Absyn_Vardecl*
_tmp26B;_LL132: if(*((int*)_tmp263)!= 1)goto _LL134;_tmp264=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp263)->f2;if(_tmp264 <= (void*)1)goto _LL134;if(*((int*)_tmp264)!= 4)goto _LL134;
_tmp265=((struct Cyc_Absyn_Pat_b_struct*)_tmp264)->f1;_LL133: _tmp267=_tmp265;goto
_LL135;_LL134: if(*((int*)_tmp263)!= 1)goto _LL136;_tmp266=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp263)->f2;if(_tmp266 <= (void*)1)goto _LL136;if(*((int*)_tmp266)!= 3)goto _LL136;
_tmp267=((struct Cyc_Absyn_Local_b_struct*)_tmp266)->f1;_LL135: _tmp269=_tmp267;
goto _LL137;_LL136: if(*((int*)_tmp263)!= 1)goto _LL138;_tmp268=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp263)->f2;if(_tmp268 <= (void*)1)goto _LL138;if(*((int*)_tmp268)!= 0)goto _LL138;
_tmp269=((struct Cyc_Absyn_Global_b_struct*)_tmp268)->f1;_LL137: _tmp26B=_tmp269;
goto _LL139;_LL138: if(*((int*)_tmp263)!= 1)goto _LL13A;_tmp26A=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp263)->f2;if(_tmp26A <= (void*)1)goto _LL13A;if(*((int*)_tmp26A)!= 2)goto _LL13A;
_tmp26B=((struct Cyc_Absyn_Param_b_struct*)_tmp26A)->f1;_LL139: if(_tmp26B->escapes)
return 0;inner_loop: {void*_tmp26C=i->r;void*_tmp26D;struct Cyc_Absyn_Exp*_tmp26E;
union Cyc_Absyn_Cnst _tmp26F;struct _tuple6 _tmp270;void*_tmp271;int _tmp272;union Cyc_Absyn_Cnst
_tmp273;struct _tuple6 _tmp274;void*_tmp275;int _tmp276;union Cyc_Absyn_Cnst _tmp277;
struct _tuple6 _tmp278;void*_tmp279;int _tmp27A;void*_tmp27B;struct Cyc_List_List*
_tmp27C;struct Cyc_List_List _tmp27D;struct Cyc_Absyn_Exp*_tmp27E;struct Cyc_List_List*
_tmp27F;struct Cyc_List_List _tmp280;struct Cyc_Absyn_Exp*_tmp281;void*_tmp282;
struct Cyc_Absyn_Vardecl*_tmp283;void*_tmp284;struct Cyc_Absyn_Vardecl*_tmp285;
void*_tmp286;struct Cyc_Absyn_Vardecl*_tmp287;void*_tmp288;struct Cyc_Absyn_Vardecl*
_tmp289;_LL13D: if(*((int*)_tmp26C)!= 15)goto _LL13F;_tmp26D=(void*)((struct Cyc_Absyn_Cast_e_struct*)
_tmp26C)->f1;_tmp26E=((struct Cyc_Absyn_Cast_e_struct*)_tmp26C)->f2;_LL13E: i=
_tmp26E;goto inner_loop;_LL13F: if(*((int*)_tmp26C)!= 0)goto _LL141;_tmp26F=((
struct Cyc_Absyn_Const_e_struct*)_tmp26C)->f1;if((_tmp26F.Int_c).tag != 4)goto
_LL141;_tmp270=(struct _tuple6)(_tmp26F.Int_c).val;_tmp271=_tmp270.f1;if((int)
_tmp271 != 2)goto _LL141;_tmp272=_tmp270.f2;_LL140: _tmp276=_tmp272;goto _LL142;
_LL141: if(*((int*)_tmp26C)!= 0)goto _LL143;_tmp273=((struct Cyc_Absyn_Const_e_struct*)
_tmp26C)->f1;if((_tmp273.Int_c).tag != 4)goto _LL143;_tmp274=(struct _tuple6)(
_tmp273.Int_c).val;_tmp275=_tmp274.f1;if((int)_tmp275 != 0)goto _LL143;_tmp276=
_tmp274.f2;_LL142: return _tmp276 >= 0  && Cyc_Toc_check_const_array((unsigned int)(
_tmp276 + 1),_tmp26B->type);_LL143: if(*((int*)_tmp26C)!= 0)goto _LL145;_tmp277=((
struct Cyc_Absyn_Const_e_struct*)_tmp26C)->f1;if((_tmp277.Int_c).tag != 4)goto
_LL145;_tmp278=(struct _tuple6)(_tmp277.Int_c).val;_tmp279=_tmp278.f1;if((int)
_tmp279 != 1)goto _LL145;_tmp27A=_tmp278.f2;_LL144: return Cyc_Toc_check_const_array((
unsigned int)(_tmp27A + 1),_tmp26B->type);_LL145: if(*((int*)_tmp26C)!= 3)goto
_LL147;_tmp27B=(void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp26C)->f1;if((int)
_tmp27B != 4)goto _LL147;_tmp27C=((struct Cyc_Absyn_Primop_e_struct*)_tmp26C)->f2;
if(_tmp27C == 0)goto _LL147;_tmp27D=*_tmp27C;_tmp27E=(struct Cyc_Absyn_Exp*)_tmp27D.hd;
_tmp27F=_tmp27D.tl;if(_tmp27F == 0)goto _LL147;_tmp280=*_tmp27F;_tmp281=(struct Cyc_Absyn_Exp*)
_tmp280.hd;_LL146: return Cyc_Toc_check_leq_size(relns,_tmp26B,_tmp281);_LL147: if(*((
int*)_tmp26C)!= 1)goto _LL149;_tmp282=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp26C)->f2;if(_tmp282 <= (void*)1)goto _LL149;if(*((int*)_tmp282)!= 4)goto _LL149;
_tmp283=((struct Cyc_Absyn_Pat_b_struct*)_tmp282)->f1;_LL148: _tmp285=_tmp283;goto
_LL14A;_LL149: if(*((int*)_tmp26C)!= 1)goto _LL14B;_tmp284=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp26C)->f2;if(_tmp284 <= (void*)1)goto _LL14B;if(*((int*)_tmp284)!= 3)goto _LL14B;
_tmp285=((struct Cyc_Absyn_Local_b_struct*)_tmp284)->f1;_LL14A: _tmp287=_tmp285;
goto _LL14C;_LL14B: if(*((int*)_tmp26C)!= 1)goto _LL14D;_tmp286=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp26C)->f2;if(_tmp286 <= (void*)1)goto _LL14D;if(*((int*)_tmp286)!= 0)goto _LL14D;
_tmp287=((struct Cyc_Absyn_Global_b_struct*)_tmp286)->f1;_LL14C: _tmp289=_tmp287;
goto _LL14E;_LL14D: if(*((int*)_tmp26C)!= 1)goto _LL14F;_tmp288=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp26C)->f2;if(_tmp288 <= (void*)1)goto _LL14F;if(*((int*)_tmp288)!= 2)goto _LL14F;
_tmp289=((struct Cyc_Absyn_Param_b_struct*)_tmp288)->f1;_LL14E: if(_tmp289->escapes)
return 0;{struct Cyc_List_List*_tmp28A=relns;for(0;_tmp28A != 0;_tmp28A=_tmp28A->tl){
struct Cyc_CfFlowInfo_Reln*_tmp28B=(struct Cyc_CfFlowInfo_Reln*)_tmp28A->hd;if(
_tmp28B->vd == _tmp289){union Cyc_CfFlowInfo_RelnOp _tmp28C=_tmp28B->rop;struct Cyc_Absyn_Vardecl*
_tmp28D;struct _tuple12 _tmp28E;struct Cyc_Absyn_Vardecl*_tmp28F;void*_tmp290;
unsigned int _tmp291;_LL152: if((_tmp28C.LessNumelts).tag != 3)goto _LL154;_tmp28D=(
struct Cyc_Absyn_Vardecl*)(_tmp28C.LessNumelts).val;_LL153: if(_tmp26B == _tmp28D)
return 1;else{goto _LL151;}_LL154: if((_tmp28C.LessVar).tag != 2)goto _LL156;_tmp28E=(
struct _tuple12)(_tmp28C.LessVar).val;_tmp28F=_tmp28E.f1;_tmp290=_tmp28E.f2;
_LL155:{struct _tuple0 _tmp992;struct _tuple0 _tmp293=(_tmp992.f1=Cyc_Tcutil_compress(
_tmp290),((_tmp992.f2=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(a->topt))->v),_tmp992)));void*_tmp294;void*_tmp295;void*_tmp296;
struct Cyc_Absyn_PtrInfo _tmp297;struct Cyc_Absyn_PtrAtts _tmp298;union Cyc_Absyn_Constraint*
_tmp299;_LL15B: _tmp294=_tmp293.f1;if(_tmp294 <= (void*)4)goto _LL15D;if(*((int*)
_tmp294)!= 18)goto _LL15D;_tmp295=(void*)((struct Cyc_Absyn_TagType_struct*)
_tmp294)->f1;_tmp296=_tmp293.f2;if(_tmp296 <= (void*)4)goto _LL15D;if(*((int*)
_tmp296)!= 4)goto _LL15D;_tmp297=((struct Cyc_Absyn_PointerType_struct*)_tmp296)->f1;
_tmp298=_tmp297.ptr_atts;_tmp299=_tmp298.bounds;_LL15C:{void*_tmp29A=Cyc_Absyn_conref_val(
_tmp299);struct Cyc_Absyn_Exp*_tmp29B;_LL160: if(_tmp29A <= (void*)1)goto _LL162;if(*((
int*)_tmp29A)!= 0)goto _LL162;_tmp29B=((struct Cyc_Absyn_Upper_b_struct*)_tmp29A)->f1;
_LL161: {struct Cyc_Absyn_Exp*_tmp29C=Cyc_Absyn_cast_exp(Cyc_Absyn_uint_typ,Cyc_Absyn_valueof_exp(
_tmp295,0),0,(void*)1,0);if(Cyc_Evexp_lte_const_exp(_tmp29C,_tmp29B))return 1;
goto _LL15F;}_LL162:;_LL163: goto _LL15F;_LL15F:;}goto _LL15A;_LL15D:;_LL15E: goto
_LL15A;_LL15A:;}{struct Cyc_List_List*_tmp29D=relns;for(0;_tmp29D != 0;_tmp29D=
_tmp29D->tl){struct Cyc_CfFlowInfo_Reln*_tmp29E=(struct Cyc_CfFlowInfo_Reln*)
_tmp29D->hd;if(_tmp29E->vd == _tmp28F){union Cyc_CfFlowInfo_RelnOp _tmp29F=_tmp29E->rop;
struct Cyc_Absyn_Vardecl*_tmp2A0;struct Cyc_Absyn_Vardecl*_tmp2A1;unsigned int
_tmp2A2;struct _tuple12 _tmp2A3;struct Cyc_Absyn_Vardecl*_tmp2A4;_LL165: if((_tmp29F.LessEqNumelts).tag
!= 5)goto _LL167;_tmp2A0=(struct Cyc_Absyn_Vardecl*)(_tmp29F.LessEqNumelts).val;
_LL166: _tmp2A1=_tmp2A0;goto _LL168;_LL167: if((_tmp29F.LessNumelts).tag != 3)goto
_LL169;_tmp2A1=(struct Cyc_Absyn_Vardecl*)(_tmp29F.LessNumelts).val;_LL168: if(
_tmp26B == _tmp2A1)return 1;goto _LL164;_LL169: if((_tmp29F.EqualConst).tag != 1)goto
_LL16B;_tmp2A2=(unsigned int)(_tmp29F.EqualConst).val;_LL16A: return Cyc_Toc_check_const_array(
_tmp2A2,_tmp26B->type);_LL16B: if((_tmp29F.LessVar).tag != 2)goto _LL16D;_tmp2A3=(
struct _tuple12)(_tmp29F.LessVar).val;_tmp2A4=_tmp2A3.f1;_LL16C: if(Cyc_Toc_check_leq_size_var(
relns,_tmp26B,_tmp2A4))return 1;goto _LL164;_LL16D:;_LL16E: goto _LL164;_LL164:;}}}
goto _LL151;_LL156: if((_tmp28C.LessConst).tag != 4)goto _LL158;_tmp291=(
unsigned int)(_tmp28C.LessConst).val;_LL157: return Cyc_Toc_check_const_array(
_tmp291,_tmp26B->type);_LL158:;_LL159: goto _LL151;_LL151:;}}}goto _LL13C;_LL14F:;
_LL150: goto _LL13C;_LL13C:;}goto _LL131;_LL13A:;_LL13B: goto _LL131;_LL131:;}return 0;}
static void*Cyc_Toc_get_c_typ(struct Cyc_Absyn_Exp*e);static void*Cyc_Toc_get_c_typ(
struct Cyc_Absyn_Exp*e){if(e->topt == 0){const char*_tmp995;void*_tmp994;(_tmp994=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmp995="Missing type in primop ",_tag_dyneither(_tmp995,sizeof(char),24))),
_tag_dyneither(_tmp994,sizeof(void*),0)));}return Cyc_Toc_typ_to_c((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v);}static void*Cyc_Toc_get_cyc_typ(
struct Cyc_Absyn_Exp*e);static void*Cyc_Toc_get_cyc_typ(struct Cyc_Absyn_Exp*e){if(
e->topt == 0){const char*_tmp998;void*_tmp997;(_tmp997=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmp998="Missing type in primop ",
_tag_dyneither(_tmp998,sizeof(char),24))),_tag_dyneither(_tmp997,sizeof(void*),0)));}
return(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;}static struct
_tuple10*Cyc_Toc_tup_to_c(struct Cyc_Absyn_Exp*e);static struct _tuple10*Cyc_Toc_tup_to_c(
struct Cyc_Absyn_Exp*e){struct _tuple10*_tmp999;return(_tmp999=_cycalloc(sizeof(*
_tmp999)),((_tmp999->f1=Cyc_Toc_mt_tq,((_tmp999->f2=Cyc_Toc_typ_to_c((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v),_tmp999)))));}static struct _tuple16*
Cyc_Toc_add_designator(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e);static struct
_tuple16*Cyc_Toc_add_designator(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e){Cyc_Toc_exp_to_c(
nv,e);{struct _tuple16*_tmp99A;return(_tmp99A=_cycalloc(sizeof(*_tmp99A)),((
_tmp99A->f1=0,((_tmp99A->f2=e,_tmp99A)))));}}static struct Cyc_Absyn_Exp*Cyc_Toc_make_struct(
struct Cyc_Toc_Env*nv,struct _tuple1*x,void*struct_typ,struct Cyc_Absyn_Stmt*s,int
pointer,struct Cyc_Absyn_Exp*rgnopt,int is_atomic);static struct Cyc_Absyn_Exp*Cyc_Toc_make_struct(
struct Cyc_Toc_Env*nv,struct _tuple1*x,void*struct_typ,struct Cyc_Absyn_Stmt*s,int
pointer,struct Cyc_Absyn_Exp*rgnopt,int is_atomic){struct Cyc_Absyn_Exp*eo;void*t;
if(pointer){t=Cyc_Absyn_cstar_typ(struct_typ,Cyc_Toc_mt_tq);{struct Cyc_Absyn_Exp*
_tmp2AB=Cyc_Absyn_sizeofexp_exp(Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(x,0),0),0);
if(rgnopt == 0  || Cyc_Absyn_no_regions)eo=(struct Cyc_Absyn_Exp*)(is_atomic?Cyc_Toc_malloc_atomic(
_tmp2AB): Cyc_Toc_malloc_ptr(_tmp2AB));else{struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)
rgnopt;Cyc_Toc_exp_to_c(nv,r);eo=(struct Cyc_Absyn_Exp*)Cyc_Toc_rmalloc_exp(r,
_tmp2AB);}}}else{t=struct_typ;eo=0;}return Cyc_Absyn_stmt_exp(Cyc_Absyn_declare_stmt(
x,t,eo,s,0),0);}static struct Cyc_Absyn_Stmt*Cyc_Toc_init_comprehension(struct Cyc_Toc_Env*
nv,struct Cyc_Absyn_Exp*lhs,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*e1,
struct Cyc_Absyn_Exp*e2,int zero_term,struct Cyc_Absyn_Stmt*s,int
e1_already_translated);static struct Cyc_Absyn_Stmt*Cyc_Toc_init_anon_struct(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,void*struct_type,struct Cyc_List_List*
dles,struct Cyc_Absyn_Stmt*s);static struct Cyc_Absyn_Stmt*Cyc_Toc_init_array(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,struct Cyc_List_List*dles0,struct Cyc_Absyn_Stmt*
s);static struct Cyc_Absyn_Stmt*Cyc_Toc_init_array(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*
lhs,struct Cyc_List_List*dles0,struct Cyc_Absyn_Stmt*s){int count=((int(*)(struct
Cyc_List_List*x))Cyc_List_length)(dles0)- 1;{struct Cyc_List_List*_tmp2AC=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(dles0);for(0;
_tmp2AC != 0;_tmp2AC=_tmp2AC->tl){struct _tuple16 _tmp2AE;struct Cyc_List_List*
_tmp2AF;struct Cyc_Absyn_Exp*_tmp2B0;struct _tuple16*_tmp2AD=(struct _tuple16*)
_tmp2AC->hd;_tmp2AE=*_tmp2AD;_tmp2AF=_tmp2AE.f1;_tmp2B0=_tmp2AE.f2;{struct Cyc_Absyn_Exp*
e_index;if(_tmp2AF == 0)e_index=Cyc_Absyn_signed_int_exp(count --,0);else{if(
_tmp2AF->tl != 0){const char*_tmp99D;void*_tmp99C;(_tmp99C=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmp99D="multiple designators in array",
_tag_dyneither(_tmp99D,sizeof(char),30))),_tag_dyneither(_tmp99C,sizeof(void*),0)));}{
void*_tmp2B3=(void*)_tmp2AF->hd;void*_tmp2B4=_tmp2B3;struct Cyc_Absyn_Exp*_tmp2B5;
_LL170: if(*((int*)_tmp2B4)!= 0)goto _LL172;_tmp2B5=((struct Cyc_Absyn_ArrayElement_struct*)
_tmp2B4)->f1;_LL171: Cyc_Toc_exp_to_c(nv,_tmp2B5);e_index=_tmp2B5;goto _LL16F;
_LL172: if(*((int*)_tmp2B4)!= 1)goto _LL16F;_LL173: {const char*_tmp9A0;void*
_tmp99F;(_tmp99F=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((
_tmp9A0="field name designators in array",_tag_dyneither(_tmp9A0,sizeof(char),32))),
_tag_dyneither(_tmp99F,sizeof(void*),0)));}_LL16F:;}}{struct Cyc_Absyn_Exp*lval=
Cyc_Absyn_subscript_exp(lhs,e_index,0);void*_tmp2B8=_tmp2B0->r;struct Cyc_List_List*
_tmp2B9;struct Cyc_Absyn_Vardecl*_tmp2BA;struct Cyc_Absyn_Exp*_tmp2BB;struct Cyc_Absyn_Exp*
_tmp2BC;int _tmp2BD;void*_tmp2BE;struct Cyc_List_List*_tmp2BF;_LL175: if(*((int*)
_tmp2B8)!= 28)goto _LL177;_tmp2B9=((struct Cyc_Absyn_Array_e_struct*)_tmp2B8)->f1;
_LL176: s=Cyc_Toc_init_array(nv,lval,_tmp2B9,s);goto _LL174;_LL177: if(*((int*)
_tmp2B8)!= 29)goto _LL179;_tmp2BA=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp2B8)->f1;_tmp2BB=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp2B8)->f2;
_tmp2BC=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp2B8)->f3;_tmp2BD=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmp2B8)->f4;_LL178: s=Cyc_Toc_init_comprehension(
nv,lval,_tmp2BA,_tmp2BB,_tmp2BC,_tmp2BD,s,0);goto _LL174;_LL179: if(*((int*)
_tmp2B8)!= 31)goto _LL17B;_tmp2BE=(void*)((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp2B8)->f1;_tmp2BF=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp2B8)->f2;_LL17A:
s=Cyc_Toc_init_anon_struct(nv,lval,_tmp2BE,_tmp2BF,s);goto _LL174;_LL17B:;_LL17C:
Cyc_Toc_exp_to_c(nv,_tmp2B0);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(
lhs,e_index,0),_tmp2B0,0),s,0);goto _LL174;_LL174:;}}}}return s;}static struct Cyc_Absyn_Stmt*
Cyc_Toc_init_comprehension(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,struct
Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int zero_term,
struct Cyc_Absyn_Stmt*s,int e1_already_translated);static struct Cyc_Absyn_Stmt*Cyc_Toc_init_comprehension(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,struct Cyc_Absyn_Vardecl*vd,struct
Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int zero_term,struct Cyc_Absyn_Stmt*s,int
e1_already_translated){struct _tuple1*_tmp2C0=vd->name;void*_tmp2C1=Cyc_Toc_typ_to_c((
void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v);if(!e1_already_translated)
Cyc_Toc_exp_to_c(nv,e1);{struct Cyc_Toc_Env _tmp2C3;struct _RegionHandle*_tmp2C4;
struct Cyc_Toc_Env*_tmp2C2=nv;_tmp2C3=*_tmp2C2;_tmp2C4=_tmp2C3.rgn;{struct Cyc_Absyn_Local_b_struct
_tmp9A3;struct Cyc_Absyn_Local_b_struct*_tmp9A2;struct Cyc_Toc_Env*nv2=Cyc_Toc_add_varmap(
_tmp2C4,nv,_tmp2C0,Cyc_Absyn_varb_exp(_tmp2C0,(void*)((_tmp9A2=_cycalloc(sizeof(*
_tmp9A2)),((_tmp9A2[0]=((_tmp9A3.tag=3,((_tmp9A3.f1=vd,_tmp9A3)))),_tmp9A2)))),0));
struct _tuple1*max=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*ea=Cyc_Absyn_assign_exp(
Cyc_Absyn_var_exp(_tmp2C0,0),Cyc_Absyn_signed_int_exp(0,0),0);struct Cyc_Absyn_Exp*
eb=Cyc_Absyn_lt_exp(Cyc_Absyn_var_exp(_tmp2C0,0),Cyc_Absyn_var_exp(max,0),0);
struct Cyc_Absyn_Exp*ec=Cyc_Absyn_post_inc_exp(Cyc_Absyn_var_exp(_tmp2C0,0),0);
struct Cyc_Absyn_Exp*lval=Cyc_Absyn_subscript_exp(lhs,Cyc_Absyn_var_exp(_tmp2C0,0),
0);struct Cyc_Absyn_Stmt*body;{void*_tmp2C5=e2->r;struct Cyc_List_List*_tmp2C6;
struct Cyc_Absyn_Vardecl*_tmp2C7;struct Cyc_Absyn_Exp*_tmp2C8;struct Cyc_Absyn_Exp*
_tmp2C9;int _tmp2CA;void*_tmp2CB;struct Cyc_List_List*_tmp2CC;_LL17E: if(*((int*)
_tmp2C5)!= 28)goto _LL180;_tmp2C6=((struct Cyc_Absyn_Array_e_struct*)_tmp2C5)->f1;
_LL17F: body=Cyc_Toc_init_array(nv2,lval,_tmp2C6,Cyc_Toc_skip_stmt_dl());goto
_LL17D;_LL180: if(*((int*)_tmp2C5)!= 29)goto _LL182;_tmp2C7=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp2C5)->f1;_tmp2C8=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp2C5)->f2;
_tmp2C9=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp2C5)->f3;_tmp2CA=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmp2C5)->f4;_LL181: body=Cyc_Toc_init_comprehension(
nv2,lval,_tmp2C7,_tmp2C8,_tmp2C9,_tmp2CA,Cyc_Toc_skip_stmt_dl(),0);goto _LL17D;
_LL182: if(*((int*)_tmp2C5)!= 31)goto _LL184;_tmp2CB=(void*)((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp2C5)->f1;_tmp2CC=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp2C5)->f2;_LL183:
body=Cyc_Toc_init_anon_struct(nv,lval,_tmp2CB,_tmp2CC,Cyc_Toc_skip_stmt_dl());
goto _LL17D;_LL184:;_LL185: Cyc_Toc_exp_to_c(nv2,e2);body=Cyc_Absyn_assign_stmt(
lval,e2,0);goto _LL17D;_LL17D:;}{struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_for_stmt(ea,eb,
ec,body,0);if(zero_term){struct Cyc_Absyn_Exp*ex=Cyc_Absyn_assign_exp(Cyc_Absyn_subscript_exp(
Cyc_Absyn_new_exp(lhs->r,0),Cyc_Absyn_var_exp(max,0),0),Cyc_Toc_cast_it(_tmp2C1,
Cyc_Absyn_uint_exp(0,0)),0);s2=Cyc_Absyn_seq_stmt(s2,Cyc_Absyn_exp_stmt(ex,0),0);}
return Cyc_Absyn_seq_stmt(Cyc_Absyn_declare_stmt(max,Cyc_Absyn_uint_typ,(struct
Cyc_Absyn_Exp*)e1,Cyc_Absyn_declare_stmt(_tmp2C0,Cyc_Absyn_uint_typ,0,s2,0),0),s,
0);}}}}static struct Cyc_Absyn_Stmt*Cyc_Toc_init_anon_struct(struct Cyc_Toc_Env*nv,
struct Cyc_Absyn_Exp*lhs,void*struct_type,struct Cyc_List_List*dles,struct Cyc_Absyn_Stmt*
s);static struct Cyc_Absyn_Stmt*Cyc_Toc_init_anon_struct(struct Cyc_Toc_Env*nv,
struct Cyc_Absyn_Exp*lhs,void*struct_type,struct Cyc_List_List*dles,struct Cyc_Absyn_Stmt*
s){{struct Cyc_List_List*_tmp2CF=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_rev)(dles);for(0;_tmp2CF != 0;_tmp2CF=_tmp2CF->tl){struct _tuple16 _tmp2D1;
struct Cyc_List_List*_tmp2D2;struct Cyc_Absyn_Exp*_tmp2D3;struct _tuple16*_tmp2D0=(
struct _tuple16*)_tmp2CF->hd;_tmp2D1=*_tmp2D0;_tmp2D2=_tmp2D1.f1;_tmp2D3=_tmp2D1.f2;
if(_tmp2D2 == 0){const char*_tmp9A6;void*_tmp9A5;(_tmp9A5=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmp9A6="empty designator list",
_tag_dyneither(_tmp9A6,sizeof(char),22))),_tag_dyneither(_tmp9A5,sizeof(void*),0)));}
if(_tmp2D2->tl != 0){const char*_tmp9A9;void*_tmp9A8;(_tmp9A8=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmp9A9="too many designators in anonymous struct",
_tag_dyneither(_tmp9A9,sizeof(char),41))),_tag_dyneither(_tmp9A8,sizeof(void*),0)));}{
void*_tmp2D8=(void*)_tmp2D2->hd;struct _dyneither_ptr*_tmp2D9;_LL187: if(*((int*)
_tmp2D8)!= 1)goto _LL189;_tmp2D9=((struct Cyc_Absyn_FieldName_struct*)_tmp2D8)->f1;
_LL188: {struct Cyc_Absyn_Exp*lval=Cyc_Absyn_aggrmember_exp(lhs,_tmp2D9,0);{void*
_tmp2DA=_tmp2D3->r;struct Cyc_List_List*_tmp2DB;struct Cyc_Absyn_Vardecl*_tmp2DC;
struct Cyc_Absyn_Exp*_tmp2DD;struct Cyc_Absyn_Exp*_tmp2DE;int _tmp2DF;void*_tmp2E0;
struct Cyc_List_List*_tmp2E1;_LL18C: if(*((int*)_tmp2DA)!= 28)goto _LL18E;_tmp2DB=((
struct Cyc_Absyn_Array_e_struct*)_tmp2DA)->f1;_LL18D: s=Cyc_Toc_init_array(nv,lval,
_tmp2DB,s);goto _LL18B;_LL18E: if(*((int*)_tmp2DA)!= 29)goto _LL190;_tmp2DC=((
struct Cyc_Absyn_Comprehension_e_struct*)_tmp2DA)->f1;_tmp2DD=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp2DA)->f2;_tmp2DE=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp2DA)->f3;
_tmp2DF=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp2DA)->f4;_LL18F: s=Cyc_Toc_init_comprehension(
nv,lval,_tmp2DC,_tmp2DD,_tmp2DE,_tmp2DF,s,0);goto _LL18B;_LL190: if(*((int*)
_tmp2DA)!= 31)goto _LL192;_tmp2E0=(void*)((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp2DA)->f1;_tmp2E1=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp2DA)->f2;_LL191:
s=Cyc_Toc_init_anon_struct(nv,lval,_tmp2E0,_tmp2E1,s);goto _LL18B;_LL192:;_LL193:
Cyc_Toc_exp_to_c(nv,_tmp2D3);if(Cyc_Toc_is_poly_field(struct_type,_tmp2D9))
_tmp2D3=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp2D3);s=Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(lval,_tmp2D3,0),0),s,0);goto _LL18B;
_LL18B:;}goto _LL186;}_LL189:;_LL18A: {const char*_tmp9AC;void*_tmp9AB;(_tmp9AB=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmp9AC="array designator in struct",_tag_dyneither(_tmp9AC,sizeof(char),27))),
_tag_dyneither(_tmp9AB,sizeof(void*),0)));}_LL186:;}}}return s;}static struct Cyc_Absyn_Exp*
Cyc_Toc_init_tuple(struct Cyc_Toc_Env*nv,int pointer,struct Cyc_Absyn_Exp*rgnopt,
struct Cyc_List_List*es);static struct Cyc_Absyn_Exp*Cyc_Toc_init_tuple(struct Cyc_Toc_Env*
nv,int pointer,struct Cyc_Absyn_Exp*rgnopt,struct Cyc_List_List*es){struct Cyc_Toc_Env
_tmp2E5;struct _RegionHandle*_tmp2E6;struct Cyc_Toc_Env*_tmp2E4=nv;_tmp2E5=*
_tmp2E4;_tmp2E6=_tmp2E5.rgn;{struct Cyc_List_List*_tmp2E7=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _tuple10*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*
x))Cyc_List_rmap)(_tmp2E6,Cyc_Toc_tup_to_c,es);void*_tmp2E8=Cyc_Toc_add_tuple_type(
_tmp2E7);struct _tuple1*_tmp2E9=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp2EA=
Cyc_Absyn_var_exp(_tmp2E9,0);struct Cyc_Absyn_Stmt*_tmp2EB=Cyc_Absyn_exp_stmt(
_tmp2EA,0);struct Cyc_Absyn_Exp*(*_tmp2EC)(struct Cyc_Absyn_Exp*,struct
_dyneither_ptr*,struct Cyc_Position_Segment*)=pointer?Cyc_Absyn_aggrarrow_exp: Cyc_Absyn_aggrmember_exp;
int is_atomic=1;struct Cyc_List_List*_tmp2ED=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(_tmp2E6,es);{int i=((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmp2ED);for(0;_tmp2ED != 0;(_tmp2ED=
_tmp2ED->tl,-- i)){struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)_tmp2ED->hd;struct
Cyc_Absyn_Exp*lval=_tmp2EC(_tmp2EA,Cyc_Absyn_fieldname(i),0);is_atomic=is_atomic
 && Cyc_Toc_atomic_typ((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);{
void*_tmp2EE=e->r;struct Cyc_List_List*_tmp2EF;struct Cyc_Absyn_Vardecl*_tmp2F0;
struct Cyc_Absyn_Exp*_tmp2F1;struct Cyc_Absyn_Exp*_tmp2F2;int _tmp2F3;_LL195: if(*((
int*)_tmp2EE)!= 28)goto _LL197;_tmp2EF=((struct Cyc_Absyn_Array_e_struct*)_tmp2EE)->f1;
_LL196: _tmp2EB=Cyc_Toc_init_array(nv,lval,_tmp2EF,_tmp2EB);goto _LL194;_LL197: if(*((
int*)_tmp2EE)!= 29)goto _LL199;_tmp2F0=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp2EE)->f1;_tmp2F1=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp2EE)->f2;
_tmp2F2=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp2EE)->f3;_tmp2F3=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmp2EE)->f4;_LL198: _tmp2EB=Cyc_Toc_init_comprehension(
nv,lval,_tmp2F0,_tmp2F1,_tmp2F2,_tmp2F3,_tmp2EB,0);goto _LL194;_LL199:;_LL19A: Cyc_Toc_exp_to_c(
nv,e);_tmp2EB=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp2EC(
_tmp2EA,Cyc_Absyn_fieldname(i),0),e,0),0),_tmp2EB,0);goto _LL194;_LL194:;}}}
return Cyc_Toc_make_struct(nv,_tmp2E9,_tmp2E8,_tmp2EB,pointer,rgnopt,is_atomic);}}
static int Cyc_Toc_get_member_offset(struct Cyc_Absyn_Aggrdecl*ad,struct
_dyneither_ptr*f);static int Cyc_Toc_get_member_offset(struct Cyc_Absyn_Aggrdecl*ad,
struct _dyneither_ptr*f){int i=1;{struct Cyc_List_List*_tmp2F4=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(ad->impl))->fields;for(0;_tmp2F4 != 0;_tmp2F4=_tmp2F4->tl){struct Cyc_Absyn_Aggrfield*
_tmp2F5=(struct Cyc_Absyn_Aggrfield*)_tmp2F4->hd;if(Cyc_strcmp((struct
_dyneither_ptr)*_tmp2F5->name,(struct _dyneither_ptr)*f)== 0)return i;++ i;}}{
struct Cyc_String_pa_struct _tmp9B4;void*_tmp9B3[1];const char*_tmp9B2;void*_tmp9B1;(
_tmp9B1=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)((
struct _dyneither_ptr)((_tmp9B4.tag=0,((_tmp9B4.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*f),((_tmp9B3[0]=& _tmp9B4,Cyc_aprintf(((_tmp9B2="get_member_offset %s failed",
_tag_dyneither(_tmp9B2,sizeof(char),28))),_tag_dyneither(_tmp9B3,sizeof(void*),1)))))))),
_tag_dyneither(_tmp9B1,sizeof(void*),0)));}}static struct Cyc_Absyn_Exp*Cyc_Toc_init_struct(
struct Cyc_Toc_Env*nv,void*struct_type,int has_exists,int pointer,struct Cyc_Absyn_Exp*
rgnopt,struct Cyc_List_List*dles,struct _tuple1*tdn);static struct Cyc_Absyn_Exp*Cyc_Toc_init_struct(
struct Cyc_Toc_Env*nv,void*struct_type,int has_exists,int pointer,struct Cyc_Absyn_Exp*
rgnopt,struct Cyc_List_List*dles,struct _tuple1*tdn){struct _tuple1*_tmp2FA=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp2FB=Cyc_Absyn_var_exp(_tmp2FA,0);struct Cyc_Absyn_Stmt*
_tmp2FC=Cyc_Absyn_exp_stmt(_tmp2FB,0);struct Cyc_Absyn_Exp*(*_tmp2FD)(struct Cyc_Absyn_Exp*,
struct _dyneither_ptr*,struct Cyc_Position_Segment*)=pointer?Cyc_Absyn_aggrarrow_exp:
Cyc_Absyn_aggrmember_exp;void*_tmp2FE=Cyc_Toc_aggrdecl_type(tdn,Cyc_Absyn_strctq);
int is_atomic=1;struct Cyc_Absyn_Aggrdecl*ad;{void*_tmp2FF=Cyc_Tcutil_compress(
struct_type);struct Cyc_Absyn_AggrInfo _tmp300;union Cyc_Absyn_AggrInfoU _tmp301;
_LL19C: if(_tmp2FF <= (void*)4)goto _LL19E;if(*((int*)_tmp2FF)!= 10)goto _LL19E;
_tmp300=((struct Cyc_Absyn_AggrType_struct*)_tmp2FF)->f1;_tmp301=_tmp300.aggr_info;
_LL19D: ad=Cyc_Absyn_get_known_aggrdecl(_tmp301);goto _LL19B;_LL19E:;_LL19F: {
const char*_tmp9B7;void*_tmp9B6;(_tmp9B6=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmp9B7="init_struct: bad struct type",
_tag_dyneither(_tmp9B7,sizeof(char),29))),_tag_dyneither(_tmp9B6,sizeof(void*),0)));}
_LL19B:;}{int is_tagged_union=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged;
struct Cyc_Toc_Env _tmp305;struct _RegionHandle*_tmp306;struct Cyc_Toc_Env*_tmp304=
nv;_tmp305=*_tmp304;_tmp306=_tmp305.rgn;{struct Cyc_List_List*_tmp307=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(_tmp306,dles);for(0;
_tmp307 != 0;_tmp307=_tmp307->tl){struct _tuple16 _tmp309;struct Cyc_List_List*
_tmp30A;struct Cyc_Absyn_Exp*_tmp30B;struct _tuple16*_tmp308=(struct _tuple16*)
_tmp307->hd;_tmp309=*_tmp308;_tmp30A=_tmp309.f1;_tmp30B=_tmp309.f2;is_atomic=
is_atomic  && Cyc_Toc_atomic_typ((void*)((struct Cyc_Core_Opt*)_check_null(_tmp30B->topt))->v);
if(_tmp30A == 0){const char*_tmp9BA;void*_tmp9B9;(_tmp9B9=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmp9BA="empty designator list",
_tag_dyneither(_tmp9BA,sizeof(char),22))),_tag_dyneither(_tmp9B9,sizeof(void*),0)));}
if(_tmp30A->tl != 0){struct _tuple1*_tmp30E=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*
_tmp30F=Cyc_Absyn_var_exp(_tmp30E,0);for(0;_tmp30A != 0;_tmp30A=_tmp30A->tl){void*
_tmp310=(void*)_tmp30A->hd;struct _dyneither_ptr*_tmp311;_LL1A1: if(*((int*)
_tmp310)!= 1)goto _LL1A3;_tmp311=((struct Cyc_Absyn_FieldName_struct*)_tmp310)->f1;
_LL1A2: if(Cyc_Toc_is_poly_field(struct_type,_tmp311))_tmp30F=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),
_tmp30F);_tmp2FC=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(
_tmp2FD(_tmp2FB,_tmp311,0),_tmp30F,0),0),_tmp2FC,0);goto _LL1A0;_LL1A3:;_LL1A4: {
const char*_tmp9BD;void*_tmp9BC;(_tmp9BC=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmp9BD="array designator in struct",
_tag_dyneither(_tmp9BD,sizeof(char),27))),_tag_dyneither(_tmp9BC,sizeof(void*),0)));}
_LL1A0:;}Cyc_Toc_exp_to_c(nv,_tmp30B);_tmp2FC=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(
Cyc_Absyn_assign_exp(_tmp30F,_tmp30B,0),0),_tmp2FC,0);}else{void*_tmp314=(void*)
_tmp30A->hd;struct _dyneither_ptr*_tmp315;_LL1A6: if(*((int*)_tmp314)!= 1)goto
_LL1A8;_tmp315=((struct Cyc_Absyn_FieldName_struct*)_tmp314)->f1;_LL1A7: {struct
Cyc_Absyn_Exp*lval=_tmp2FD(_tmp2FB,_tmp315,0);if(is_tagged_union){int i=Cyc_Toc_get_member_offset(
ad,_tmp315);struct Cyc_Absyn_Exp*f_tag_exp=Cyc_Absyn_signed_int_exp(i,0);struct
Cyc_Absyn_Exp*lhs=Cyc_Absyn_aggrmember_exp(lval,Cyc_Toc_tag_sp,0);struct Cyc_Absyn_Exp*
assn_exp=Cyc_Absyn_assign_exp(lhs,f_tag_exp,0);_tmp2FC=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(
assn_exp,0),_tmp2FC,0);lval=Cyc_Absyn_aggrmember_exp(lval,Cyc_Toc_val_sp,0);}{
void*_tmp316=_tmp30B->r;struct Cyc_List_List*_tmp317;struct Cyc_Absyn_Vardecl*
_tmp318;struct Cyc_Absyn_Exp*_tmp319;struct Cyc_Absyn_Exp*_tmp31A;int _tmp31B;void*
_tmp31C;struct Cyc_List_List*_tmp31D;_LL1AB: if(*((int*)_tmp316)!= 28)goto _LL1AD;
_tmp317=((struct Cyc_Absyn_Array_e_struct*)_tmp316)->f1;_LL1AC: _tmp2FC=Cyc_Toc_init_array(
nv,lval,_tmp317,_tmp2FC);goto _LL1AA;_LL1AD: if(*((int*)_tmp316)!= 29)goto _LL1AF;
_tmp318=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp316)->f1;_tmp319=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmp316)->f2;_tmp31A=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp316)->f3;_tmp31B=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp316)->f4;
_LL1AE: _tmp2FC=Cyc_Toc_init_comprehension(nv,lval,_tmp318,_tmp319,_tmp31A,
_tmp31B,_tmp2FC,0);goto _LL1AA;_LL1AF: if(*((int*)_tmp316)!= 31)goto _LL1B1;_tmp31C=(
void*)((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp316)->f1;_tmp31D=((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp316)->f2;_LL1B0: _tmp2FC=Cyc_Toc_init_anon_struct(nv,lval,_tmp31C,_tmp31D,
_tmp2FC);goto _LL1AA;_LL1B1:;_LL1B2: {int was_ptr_type=Cyc_Tcutil_is_pointer_type((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp30B->topt))->v);Cyc_Toc_exp_to_c(nv,
_tmp30B);{struct Cyc_Absyn_Aggrfield*_tmp31E=Cyc_Absyn_lookup_decl_field(ad,
_tmp315);if(Cyc_Toc_is_poly_field(struct_type,_tmp315) && !was_ptr_type)_tmp30B=
Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp30B);if(has_exists)_tmp30B=Cyc_Toc_cast_it(
Cyc_Toc_typ_to_c(((struct Cyc_Absyn_Aggrfield*)_check_null(_tmp31E))->type),
_tmp30B);_tmp2FC=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(lval,
_tmp30B,0),0),_tmp2FC,0);goto _LL1AA;}}_LL1AA:;}goto _LL1A5;}_LL1A8:;_LL1A9: {
const char*_tmp9C0;void*_tmp9BF;(_tmp9BF=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmp9C0="array designator in struct",
_tag_dyneither(_tmp9C0,sizeof(char),27))),_tag_dyneither(_tmp9BF,sizeof(void*),0)));}
_LL1A5:;}}}return Cyc_Toc_make_struct(nv,_tmp2FA,_tmp2FE,_tmp2FC,pointer,rgnopt,
is_atomic);}}struct _tuple18{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Exp*f2;};
static struct Cyc_Absyn_Exp*Cyc_Toc_assignop_lvalue(struct Cyc_Absyn_Exp*el,struct
_tuple18*pr);static struct Cyc_Absyn_Exp*Cyc_Toc_assignop_lvalue(struct Cyc_Absyn_Exp*
el,struct _tuple18*pr){return Cyc_Absyn_assignop_exp(el,(*pr).f1,(*pr).f2,0);}
static struct Cyc_Absyn_Exp*Cyc_Toc_address_lvalue(struct Cyc_Absyn_Exp*e1,int
ignore);static struct Cyc_Absyn_Exp*Cyc_Toc_address_lvalue(struct Cyc_Absyn_Exp*e1,
int ignore){return Cyc_Absyn_address_exp(e1,0);}static struct Cyc_Absyn_Exp*Cyc_Toc_incr_lvalue(
struct Cyc_Absyn_Exp*e1,void*incr);static struct Cyc_Absyn_Exp*Cyc_Toc_incr_lvalue(
struct Cyc_Absyn_Exp*e1,void*incr){struct Cyc_Absyn_Increment_e_struct _tmp9C3;
struct Cyc_Absyn_Increment_e_struct*_tmp9C2;return Cyc_Absyn_new_exp((void*)((
_tmp9C2=_cycalloc(sizeof(*_tmp9C2)),((_tmp9C2[0]=((_tmp9C3.tag=5,((_tmp9C3.f1=e1,((
_tmp9C3.f2=(void*)incr,_tmp9C3)))))),_tmp9C2)))),0);}static void Cyc_Toc_lvalue_assign_stmt(
struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,
void*),void*f_env);static void Cyc_Toc_lvalue_assign(struct Cyc_Absyn_Exp*e1,struct
Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env);
static void Cyc_Toc_lvalue_assign(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,
struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){void*_tmp323=e1->r;
struct Cyc_Absyn_Stmt*_tmp324;void*_tmp325;struct Cyc_Absyn_Exp*_tmp326;struct Cyc_Absyn_Exp*
_tmp327;struct _dyneither_ptr*_tmp328;int _tmp329;int _tmp32A;_LL1B4: if(*((int*)
_tmp323)!= 38)goto _LL1B6;_tmp324=((struct Cyc_Absyn_StmtExp_e_struct*)_tmp323)->f1;
_LL1B5: Cyc_Toc_lvalue_assign_stmt(_tmp324,fs,f,f_env);goto _LL1B3;_LL1B6: if(*((
int*)_tmp323)!= 15)goto _LL1B8;_tmp325=(void*)((struct Cyc_Absyn_Cast_e_struct*)
_tmp323)->f1;_tmp326=((struct Cyc_Absyn_Cast_e_struct*)_tmp323)->f2;_LL1B7: Cyc_Toc_lvalue_assign(
_tmp326,fs,f,f_env);goto _LL1B3;_LL1B8: if(*((int*)_tmp323)!= 23)goto _LL1BA;
_tmp327=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp323)->f1;_tmp328=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp323)->f2;_tmp329=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp323)->f3;_tmp32A=((
struct Cyc_Absyn_AggrMember_e_struct*)_tmp323)->f4;_LL1B9: e1->r=_tmp327->r;{
struct Cyc_List_List*_tmp9C4;Cyc_Toc_lvalue_assign(e1,(struct Cyc_List_List*)((
_tmp9C4=_cycalloc(sizeof(*_tmp9C4)),((_tmp9C4->hd=_tmp328,((_tmp9C4->tl=fs,
_tmp9C4)))))),f,f_env);}goto _LL1B3;_LL1BA:;_LL1BB: {struct Cyc_Absyn_Exp*e1_copy=
Cyc_Absyn_copy_exp(e1);for(0;fs != 0;fs=fs->tl){e1_copy=Cyc_Absyn_aggrmember_exp(
e1_copy,(struct _dyneither_ptr*)fs->hd,e1_copy->loc);}e1->r=(f(e1_copy,f_env))->r;
goto _LL1B3;}_LL1B3:;}static void Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*s,
struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*
f_env);static void Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*
fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){void*_tmp32C=
s->r;struct Cyc_Absyn_Exp*_tmp32D;struct Cyc_Absyn_Decl*_tmp32E;struct Cyc_Absyn_Stmt*
_tmp32F;struct Cyc_Absyn_Stmt*_tmp330;_LL1BD: if(_tmp32C <= (void*)1)goto _LL1C3;if(*((
int*)_tmp32C)!= 0)goto _LL1BF;_tmp32D=((struct Cyc_Absyn_Exp_s_struct*)_tmp32C)->f1;
_LL1BE: Cyc_Toc_lvalue_assign(_tmp32D,fs,f,f_env);goto _LL1BC;_LL1BF: if(*((int*)
_tmp32C)!= 11)goto _LL1C1;_tmp32E=((struct Cyc_Absyn_Decl_s_struct*)_tmp32C)->f1;
_tmp32F=((struct Cyc_Absyn_Decl_s_struct*)_tmp32C)->f2;_LL1C0: Cyc_Toc_lvalue_assign_stmt(
_tmp32F,fs,f,f_env);goto _LL1BC;_LL1C1: if(*((int*)_tmp32C)!= 1)goto _LL1C3;_tmp330=((
struct Cyc_Absyn_Seq_s_struct*)_tmp32C)->f2;_LL1C2: Cyc_Toc_lvalue_assign_stmt(
_tmp330,fs,f,f_env);goto _LL1BC;_LL1C3:;_LL1C4: {const char*_tmp9C8;void*_tmp9C7[1];
struct Cyc_String_pa_struct _tmp9C6;(_tmp9C6.tag=0,((_tmp9C6.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_stmt2string(s)),((_tmp9C7[0]=&
_tmp9C6,Cyc_Toc_toc_impos(((_tmp9C8="lvalue_assign_stmt: %s",_tag_dyneither(
_tmp9C8,sizeof(char),23))),_tag_dyneither(_tmp9C7,sizeof(void*),1)))))));}_LL1BC:;}
static struct Cyc_Absyn_Exp*Cyc_Toc_push_address_exp(struct Cyc_Absyn_Exp*e);static
struct Cyc_Absyn_Exp*Cyc_Toc_push_address_exp(struct Cyc_Absyn_Exp*e){void*_tmp334=
e->r;void*_tmp335;void**_tmp336;struct Cyc_Absyn_Exp*_tmp337;struct Cyc_Absyn_Exp**
_tmp338;struct Cyc_Absyn_Exp*_tmp339;_LL1C6: if(*((int*)_tmp334)!= 15)goto _LL1C8;
_tmp335=(void**)&((struct Cyc_Absyn_Cast_e_struct*)_tmp334)->f1;_tmp336=(void**)((
void**)&((struct Cyc_Absyn_Cast_e_struct*)_tmp334)->f1);_tmp337=((struct Cyc_Absyn_Cast_e_struct*)
_tmp334)->f2;_tmp338=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Cast_e_struct*)
_tmp334)->f2;_LL1C7:*_tmp338=Cyc_Toc_push_address_exp(*_tmp338);*_tmp336=Cyc_Absyn_cstar_typ(*
_tmp336,Cyc_Toc_mt_tq);return e;_LL1C8: if(*((int*)_tmp334)!= 22)goto _LL1CA;
_tmp339=((struct Cyc_Absyn_Deref_e_struct*)_tmp334)->f1;_LL1C9: return _tmp339;
_LL1CA:;_LL1CB: if(Cyc_Absyn_is_lvalue(e))return Cyc_Absyn_address_exp(e,0);{const
char*_tmp9CC;void*_tmp9CB[1];struct Cyc_String_pa_struct _tmp9CA;(_tmp9CA.tag=0,((
_tmp9CA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((
_tmp9CB[0]=& _tmp9CA,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmp9CC="can't take & of %s",_tag_dyneither(_tmp9CC,sizeof(char),19))),
_tag_dyneither(_tmp9CB,sizeof(void*),1)))))));}_LL1C5:;}static struct Cyc_List_List*
Cyc_Toc_rmap_2c(struct _RegionHandle*r2,void*(*f)(void*,void*),void*env,struct Cyc_List_List*
x);static struct Cyc_List_List*Cyc_Toc_rmap_2c(struct _RegionHandle*r2,void*(*f)(
void*,void*),void*env,struct Cyc_List_List*x){struct Cyc_List_List*result;struct
Cyc_List_List*prev;if(x == 0)return 0;{struct Cyc_List_List*_tmp9CD;result=((
_tmp9CD=_region_malloc(r2,sizeof(*_tmp9CD)),((_tmp9CD->hd=(void*)f((void*)x->hd,
env),((_tmp9CD->tl=0,_tmp9CD))))));}prev=result;for(x=x->tl;x != 0;x=x->tl){{
struct Cyc_List_List*_tmp9CE;((struct Cyc_List_List*)_check_null(prev))->tl=((
_tmp9CE=_region_malloc(r2,sizeof(*_tmp9CE)),((_tmp9CE->hd=(void*)f((void*)x->hd,
env),((_tmp9CE->tl=0,_tmp9CE))))));}prev=((struct Cyc_List_List*)_check_null(prev))->tl;}
return result;}static struct Cyc_List_List*Cyc_Toc_map_2c(void*(*f)(void*,void*),
void*env,struct Cyc_List_List*x);static struct Cyc_List_List*Cyc_Toc_map_2c(void*(*
f)(void*,void*),void*env,struct Cyc_List_List*x){return Cyc_Toc_rmap_2c(Cyc_Core_heap_region,
f,env,x);}static struct _tuple16*Cyc_Toc_make_dle(struct Cyc_Absyn_Exp*e);static
struct _tuple16*Cyc_Toc_make_dle(struct Cyc_Absyn_Exp*e){struct _tuple16*_tmp9CF;
return(_tmp9CF=_cycalloc(sizeof(*_tmp9CF)),((_tmp9CF->f1=0,((_tmp9CF->f2=e,
_tmp9CF)))));}static struct Cyc_Absyn_PtrInfo Cyc_Toc_get_ptr_type(void*t);static
struct Cyc_Absyn_PtrInfo Cyc_Toc_get_ptr_type(void*t){void*_tmp340=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_PtrInfo _tmp341;_LL1CD: if(_tmp340 <= (void*)4)goto _LL1CF;if(*((
int*)_tmp340)!= 4)goto _LL1CF;_tmp341=((struct Cyc_Absyn_PointerType_struct*)
_tmp340)->f1;_LL1CE: return _tmp341;_LL1CF:;_LL1D0: {const char*_tmp9D2;void*
_tmp9D1;(_tmp9D1=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmp9D2="get_ptr_typ: not a pointer!",_tag_dyneither(_tmp9D2,sizeof(char),28))),
_tag_dyneither(_tmp9D1,sizeof(void*),0)));}_LL1CC:;}static struct Cyc_Absyn_Exp*
Cyc_Toc_generate_zero(void*t);static struct Cyc_Absyn_Exp*Cyc_Toc_generate_zero(
void*t){struct Cyc_Absyn_Exp*res;{void*_tmp344=Cyc_Tcutil_compress(t);void*
_tmp345;void*_tmp346;void*_tmp347;void*_tmp348;void*_tmp349;void*_tmp34A;void*
_tmp34B;void*_tmp34C;void*_tmp34D;void*_tmp34E;_LL1D2: if(_tmp344 <= (void*)4)goto
_LL1E2;if(*((int*)_tmp344)!= 4)goto _LL1D4;_LL1D3: res=Cyc_Absyn_null_exp(0);goto
_LL1D1;_LL1D4: if(*((int*)_tmp344)!= 5)goto _LL1D6;_tmp345=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp344)->f1;_tmp346=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp344)->f2;if((
int)_tmp346 != 0)goto _LL1D6;_LL1D5: res=Cyc_Absyn_const_exp(Cyc_Absyn_Char_c(
_tmp345,'\000'),0);goto _LL1D1;_LL1D6: if(*((int*)_tmp344)!= 5)goto _LL1D8;_tmp347=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp344)->f1;_tmp348=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp344)->f2;if((int)_tmp348 != 1)goto _LL1D8;_LL1D7: res=Cyc_Absyn_const_exp(Cyc_Absyn_Short_c(
_tmp347,0),0);goto _LL1D1;_LL1D8: if(*((int*)_tmp344)!= 12)goto _LL1DA;_LL1D9: goto
_LL1DB;_LL1DA: if(*((int*)_tmp344)!= 13)goto _LL1DC;_LL1DB: _tmp349=(void*)1;goto
_LL1DD;_LL1DC: if(*((int*)_tmp344)!= 5)goto _LL1DE;_tmp349=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp344)->f1;_tmp34A=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp344)->f2;if((
int)_tmp34A != 2)goto _LL1DE;_LL1DD: _tmp34B=_tmp349;goto _LL1DF;_LL1DE: if(*((int*)
_tmp344)!= 5)goto _LL1E0;_tmp34B=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp344)->f1;
_tmp34C=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp344)->f2;if((int)_tmp34C != 
3)goto _LL1E0;_LL1DF: res=Cyc_Absyn_const_exp(Cyc_Absyn_Int_c(_tmp34B,0),0);goto
_LL1D1;_LL1E0: if(*((int*)_tmp344)!= 5)goto _LL1E2;_tmp34D=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp344)->f1;_tmp34E=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp344)->f2;if((
int)_tmp34E != 4)goto _LL1E2;_LL1E1: res=Cyc_Absyn_const_exp(Cyc_Absyn_LongLong_c(
_tmp34D,(long long)0),0);goto _LL1D1;_LL1E2: if((int)_tmp344 != 1)goto _LL1E4;_LL1E3:
goto _LL1E5;_LL1E4: if(_tmp344 <= (void*)4)goto _LL1E6;if(*((int*)_tmp344)!= 6)goto
_LL1E6;_LL1E5:{const char*_tmp9D3;res=Cyc_Absyn_const_exp(Cyc_Absyn_Float_c(((
_tmp9D3="0.0",_tag_dyneither(_tmp9D3,sizeof(char),4)))),0);}goto _LL1D1;_LL1E6:;
_LL1E7: {const char*_tmp9D7;void*_tmp9D6[1];struct Cyc_String_pa_struct _tmp9D5;(
_tmp9D5.tag=0,((_tmp9D5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t)),((_tmp9D6[0]=& _tmp9D5,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap))Cyc_Toc_toc_impos)(((_tmp9D7="found non-zero type %s in generate_zero",
_tag_dyneither(_tmp9D7,sizeof(char),40))),_tag_dyneither(_tmp9D6,sizeof(void*),1)))))));}
_LL1D1:;}{struct Cyc_Core_Opt*_tmp9D8;res->topt=((_tmp9D8=_cycalloc(sizeof(*
_tmp9D8)),((_tmp9D8->v=(void*)t,_tmp9D8))));}return res;}static void Cyc_Toc_zero_ptr_assign_to_c(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*
popt,struct Cyc_Absyn_Exp*e2,void*ptr_type,int is_dyneither,void*elt_type);static
void Cyc_Toc_zero_ptr_assign_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e,
struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,void*
ptr_type,int is_dyneither,void*elt_type){void*fat_ptr_type=Cyc_Absyn_dyneither_typ(
elt_type,(void*)2,Cyc_Toc_mt_tq,Cyc_Absyn_true_conref);void*_tmp354=Cyc_Toc_typ_to_c(
elt_type);void*_tmp355=Cyc_Toc_typ_to_c(fat_ptr_type);void*_tmp356=Cyc_Absyn_cstar_typ(
_tmp354,Cyc_Toc_mt_tq);struct Cyc_Core_Opt*_tmp9D9;struct Cyc_Core_Opt*_tmp357=(
_tmp9D9=_cycalloc(sizeof(*_tmp9D9)),((_tmp9D9->v=(void*)_tmp356,_tmp9D9)));
struct Cyc_Absyn_Exp*xinit;{void*_tmp358=e1->r;struct Cyc_Absyn_Exp*_tmp359;struct
Cyc_Absyn_Exp*_tmp35A;struct Cyc_Absyn_Exp*_tmp35B;_LL1E9: if(*((int*)_tmp358)!= 
22)goto _LL1EB;_tmp359=((struct Cyc_Absyn_Deref_e_struct*)_tmp358)->f1;_LL1EA: if(!
is_dyneither){_tmp359=Cyc_Toc_cast_it(fat_ptr_type,_tmp359);{struct Cyc_Core_Opt*
_tmp9DA;_tmp359->topt=((_tmp9DA=_cycalloc(sizeof(*_tmp9DA)),((_tmp9DA->v=(void*)
fat_ptr_type,_tmp9DA))));}}Cyc_Toc_exp_to_c(nv,_tmp359);xinit=_tmp359;goto _LL1E8;
_LL1EB: if(*((int*)_tmp358)!= 25)goto _LL1ED;_tmp35A=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp358)->f1;_tmp35B=((struct Cyc_Absyn_Subscript_e_struct*)_tmp358)->f2;_LL1EC:
if(!is_dyneither){_tmp35A=Cyc_Toc_cast_it(fat_ptr_type,_tmp35A);{struct Cyc_Core_Opt*
_tmp9DB;_tmp35A->topt=((_tmp9DB=_cycalloc(sizeof(*_tmp9DB)),((_tmp9DB->v=(void*)
fat_ptr_type,_tmp9DB))));}}Cyc_Toc_exp_to_c(nv,_tmp35A);Cyc_Toc_exp_to_c(nv,
_tmp35B);{struct Cyc_Absyn_Exp*_tmp9DC[3];xinit=Cyc_Absyn_fncall_exp(Cyc_Toc__dyneither_ptr_plus_e,((
_tmp9DC[2]=_tmp35B,((_tmp9DC[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
elt_type),0),((_tmp9DC[0]=_tmp35A,((struct Cyc_List_List*(*)(struct _dyneither_ptr))
Cyc_List_list)(_tag_dyneither(_tmp9DC,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);}
goto _LL1E8;_LL1ED:;_LL1EE: {const char*_tmp9DF;void*_tmp9DE;(_tmp9DE=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmp9DF="found bad lhs for zero-terminated pointer assignment",
_tag_dyneither(_tmp9DF,sizeof(char),53))),_tag_dyneither(_tmp9DE,sizeof(void*),0)));}
_LL1E8:;}{struct _tuple1*_tmp361=Cyc_Toc_temp_var();struct Cyc_Toc_Env _tmp363;
struct _RegionHandle*_tmp364;struct Cyc_Toc_Env*_tmp362=nv;_tmp363=*_tmp362;
_tmp364=_tmp363.rgn;{struct Cyc_Toc_Env*_tmp365=Cyc_Toc_add_varmap(_tmp364,nv,
_tmp361,Cyc_Absyn_var_exp(_tmp361,0));struct Cyc_Absyn_Vardecl*_tmp9E0;struct Cyc_Absyn_Vardecl*
_tmp366=(_tmp9E0=_cycalloc(sizeof(*_tmp9E0)),((_tmp9E0->sc=(void*)2,((_tmp9E0->name=
_tmp361,((_tmp9E0->tq=Cyc_Toc_mt_tq,((_tmp9E0->type=_tmp355,((_tmp9E0->initializer=(
struct Cyc_Absyn_Exp*)xinit,((_tmp9E0->rgn=0,((_tmp9E0->attributes=0,((_tmp9E0->escapes=
0,_tmp9E0)))))))))))))))));struct Cyc_Absyn_Local_b_struct _tmp9E3;struct Cyc_Absyn_Local_b_struct*
_tmp9E2;struct Cyc_Absyn_Local_b_struct*_tmp367=(_tmp9E2=_cycalloc(sizeof(*
_tmp9E2)),((_tmp9E2[0]=((_tmp9E3.tag=3,((_tmp9E3.f1=_tmp366,_tmp9E3)))),_tmp9E2)));
struct Cyc_Absyn_Exp*_tmp368=Cyc_Absyn_varb_exp(_tmp361,(void*)_tmp367,0);{struct
Cyc_Core_Opt*_tmp9E4;_tmp368->topt=((_tmp9E4=_cycalloc(sizeof(*_tmp9E4)),((
_tmp9E4->v=(void*)fat_ptr_type,_tmp9E4))));}{struct Cyc_Absyn_Exp*_tmp36A=Cyc_Absyn_deref_exp(
_tmp368,0);{struct Cyc_Core_Opt*_tmp9E5;_tmp36A->topt=((_tmp9E5=_cycalloc(sizeof(*
_tmp9E5)),((_tmp9E5->v=(void*)elt_type,_tmp9E5))));}Cyc_Toc_exp_to_c(_tmp365,
_tmp36A);{struct _tuple1*_tmp36C=Cyc_Toc_temp_var();_tmp365=Cyc_Toc_add_varmap(
_tmp364,_tmp365,_tmp36C,Cyc_Absyn_var_exp(_tmp36C,0));{struct Cyc_Absyn_Vardecl*
_tmp9E6;struct Cyc_Absyn_Vardecl*_tmp36D=(_tmp9E6=_cycalloc(sizeof(*_tmp9E6)),((
_tmp9E6->sc=(void*)2,((_tmp9E6->name=_tmp36C,((_tmp9E6->tq=Cyc_Toc_mt_tq,((
_tmp9E6->type=_tmp354,((_tmp9E6->initializer=(struct Cyc_Absyn_Exp*)_tmp36A,((
_tmp9E6->rgn=0,((_tmp9E6->attributes=0,((_tmp9E6->escapes=0,_tmp9E6)))))))))))))))));
struct Cyc_Absyn_Local_b_struct _tmp9E9;struct Cyc_Absyn_Local_b_struct*_tmp9E8;
struct Cyc_Absyn_Local_b_struct*_tmp36E=(_tmp9E8=_cycalloc(sizeof(*_tmp9E8)),((
_tmp9E8[0]=((_tmp9E9.tag=3,((_tmp9E9.f1=_tmp36D,_tmp9E9)))),_tmp9E8)));struct Cyc_Absyn_Exp*
z_init=e2;if(popt != 0){struct Cyc_Absyn_Exp*_tmp36F=Cyc_Absyn_varb_exp(_tmp36C,(
void*)_tmp36E,0);_tmp36F->topt=_tmp36A->topt;z_init=Cyc_Absyn_prim2_exp((void*)
popt->v,_tmp36F,e2,0);z_init->topt=_tmp36F->topt;}Cyc_Toc_exp_to_c(_tmp365,
z_init);{struct _tuple1*_tmp370=Cyc_Toc_temp_var();struct Cyc_Absyn_Vardecl*
_tmp9EA;struct Cyc_Absyn_Vardecl*_tmp371=(_tmp9EA=_cycalloc(sizeof(*_tmp9EA)),((
_tmp9EA->sc=(void*)2,((_tmp9EA->name=_tmp370,((_tmp9EA->tq=Cyc_Toc_mt_tq,((
_tmp9EA->type=_tmp354,((_tmp9EA->initializer=(struct Cyc_Absyn_Exp*)z_init,((
_tmp9EA->rgn=0,((_tmp9EA->attributes=0,((_tmp9EA->escapes=0,_tmp9EA)))))))))))))))));
struct Cyc_Absyn_Local_b_struct _tmp9ED;struct Cyc_Absyn_Local_b_struct*_tmp9EC;
struct Cyc_Absyn_Local_b_struct*_tmp372=(_tmp9EC=_cycalloc(sizeof(*_tmp9EC)),((
_tmp9EC[0]=((_tmp9ED.tag=3,((_tmp9ED.f1=_tmp371,_tmp9ED)))),_tmp9EC)));_tmp365=
Cyc_Toc_add_varmap(_tmp364,_tmp365,_tmp370,Cyc_Absyn_var_exp(_tmp370,0));{struct
Cyc_Absyn_Exp*_tmp373=Cyc_Absyn_varb_exp(_tmp36C,(void*)_tmp36E,0);_tmp373->topt=
_tmp36A->topt;{struct Cyc_Absyn_Exp*_tmp374=Cyc_Toc_generate_zero(elt_type);
struct Cyc_Absyn_Exp*_tmp375=Cyc_Absyn_prim2_exp((void*)5,_tmp373,_tmp374,0);{
struct Cyc_Core_Opt*_tmp9EE;_tmp375->topt=((_tmp9EE=_cycalloc(sizeof(*_tmp9EE)),((
_tmp9EE->v=(void*)Cyc_Absyn_sint_typ,_tmp9EE))));}Cyc_Toc_exp_to_c(_tmp365,
_tmp375);{struct Cyc_Absyn_Exp*_tmp377=Cyc_Absyn_varb_exp(_tmp370,(void*)_tmp372,
0);_tmp377->topt=_tmp36A->topt;{struct Cyc_Absyn_Exp*_tmp378=Cyc_Toc_generate_zero(
elt_type);struct Cyc_Absyn_Exp*_tmp379=Cyc_Absyn_prim2_exp((void*)6,_tmp377,
_tmp378,0);{struct Cyc_Core_Opt*_tmp9EF;_tmp379->topt=((_tmp9EF=_cycalloc(sizeof(*
_tmp9EF)),((_tmp9EF->v=(void*)Cyc_Absyn_sint_typ,_tmp9EF))));}Cyc_Toc_exp_to_c(
_tmp365,_tmp379);{struct Cyc_Absyn_Exp*_tmp9F0[2];struct Cyc_List_List*_tmp37B=(
_tmp9F0[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_type),0),((_tmp9F0[0]=
Cyc_Absyn_varb_exp(_tmp361,(void*)_tmp367,0),((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp9F0,sizeof(struct Cyc_Absyn_Exp*),
2)))));struct Cyc_Absyn_Exp*_tmp37C=Cyc_Absyn_uint_exp(1,0);struct Cyc_Absyn_Exp*
xsize;xsize=Cyc_Absyn_prim2_exp((void*)5,Cyc_Absyn_fncall_exp(Cyc_Toc__get_dyneither_size_e,
_tmp37B,0),_tmp37C,0);{struct Cyc_Absyn_Exp*_tmp37D=Cyc_Absyn_and_exp(xsize,Cyc_Absyn_and_exp(
_tmp375,_tmp379,0),0);struct Cyc_Absyn_Stmt*_tmp37E=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(
Cyc_Toc__throw_arraybounds_e,0,0),0);struct Cyc_Absyn_Exp*_tmp37F=Cyc_Absyn_aggrmember_exp(
Cyc_Absyn_varb_exp(_tmp361,(void*)_tmp367,0),Cyc_Toc_curr_sp,0);_tmp37F=Cyc_Toc_cast_it(
_tmp356,_tmp37F);{struct Cyc_Absyn_Exp*_tmp380=Cyc_Absyn_deref_exp(_tmp37F,0);
struct Cyc_Absyn_Exp*_tmp381=Cyc_Absyn_assign_exp(_tmp380,Cyc_Absyn_var_exp(
_tmp370,0),0);struct Cyc_Absyn_Stmt*_tmp382=Cyc_Absyn_exp_stmt(_tmp381,0);_tmp382=
Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(_tmp37D,_tmp37E,Cyc_Absyn_skip_stmt(
0),0),_tmp382,0);{struct Cyc_Absyn_Var_d_struct*_tmp9F6;struct Cyc_Absyn_Var_d_struct
_tmp9F5;struct Cyc_Absyn_Decl*_tmp9F4;_tmp382=Cyc_Absyn_decl_stmt(((_tmp9F4=
_cycalloc(sizeof(*_tmp9F4)),((_tmp9F4->r=(void*)((_tmp9F6=_cycalloc(sizeof(*
_tmp9F6)),((_tmp9F6[0]=((_tmp9F5.tag=0,((_tmp9F5.f1=_tmp371,_tmp9F5)))),_tmp9F6)))),((
_tmp9F4->loc=0,_tmp9F4)))))),_tmp382,0);}{struct Cyc_Absyn_Var_d_struct*_tmp9FC;
struct Cyc_Absyn_Var_d_struct _tmp9FB;struct Cyc_Absyn_Decl*_tmp9FA;_tmp382=Cyc_Absyn_decl_stmt(((
_tmp9FA=_cycalloc(sizeof(*_tmp9FA)),((_tmp9FA->r=(void*)((_tmp9FC=_cycalloc(
sizeof(*_tmp9FC)),((_tmp9FC[0]=((_tmp9FB.tag=0,((_tmp9FB.f1=_tmp36D,_tmp9FB)))),
_tmp9FC)))),((_tmp9FA->loc=0,_tmp9FA)))))),_tmp382,0);}{struct Cyc_Absyn_Var_d_struct*
_tmpA02;struct Cyc_Absyn_Var_d_struct _tmpA01;struct Cyc_Absyn_Decl*_tmpA00;_tmp382=
Cyc_Absyn_decl_stmt(((_tmpA00=_cycalloc(sizeof(*_tmpA00)),((_tmpA00->r=(void*)((
_tmpA02=_cycalloc(sizeof(*_tmpA02)),((_tmpA02[0]=((_tmpA01.tag=0,((_tmpA01.f1=
_tmp366,_tmpA01)))),_tmpA02)))),((_tmpA00->loc=0,_tmpA00)))))),_tmp382,0);}e->r=
Cyc_Toc_stmt_exp_r(_tmp382);}}}}}}}}}}}}}}static void*Cyc_Toc_check_tagged_union(
struct Cyc_Absyn_Exp*e1,void*e1_c_type,void*aggrtype,struct _dyneither_ptr*f,
struct Cyc_Absyn_Exp*(*aggrproj)(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,
struct Cyc_Position_Segment*));static void*Cyc_Toc_check_tagged_union(struct Cyc_Absyn_Exp*
e1,void*e1_c_type,void*aggrtype,struct _dyneither_ptr*f,struct Cyc_Absyn_Exp*(*
aggrproj)(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,struct Cyc_Position_Segment*)){
struct Cyc_Absyn_Aggrdecl*ad;{void*_tmp397=Cyc_Tcutil_compress(aggrtype);struct
Cyc_Absyn_AggrInfo _tmp398;union Cyc_Absyn_AggrInfoU _tmp399;_LL1F0: if(_tmp397 <= (
void*)4)goto _LL1F2;if(*((int*)_tmp397)!= 10)goto _LL1F2;_tmp398=((struct Cyc_Absyn_AggrType_struct*)
_tmp397)->f1;_tmp399=_tmp398.aggr_info;_LL1F1: ad=Cyc_Absyn_get_known_aggrdecl(
_tmp399);goto _LL1EF;_LL1F2:;_LL1F3: {struct Cyc_String_pa_struct _tmpA0A;void*
_tmpA09[1];const char*_tmpA08;void*_tmpA07;(_tmpA07=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)((struct
_dyneither_ptr)((_tmpA0A.tag=0,((_tmpA0A.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(aggrtype)),((_tmpA09[0]=& _tmpA0A,Cyc_aprintf(((
_tmpA08="expecting union but found %s in check_tagged_union",_tag_dyneither(
_tmpA08,sizeof(char),51))),_tag_dyneither(_tmpA09,sizeof(void*),1)))))))),
_tag_dyneither(_tmpA07,sizeof(void*),0)));}_LL1EF:;}{struct _tuple1*_tmp39E=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp39F=Cyc_Absyn_var_exp(_tmp39E,0);struct Cyc_Absyn_Exp*
_tmp3A0=Cyc_Absyn_aggrmember_exp(aggrproj(_tmp39F,f,0),Cyc_Toc_tag_sp,0);struct
Cyc_Absyn_Exp*_tmp3A1=Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(ad,f),0);
struct Cyc_Absyn_Exp*_tmp3A2=Cyc_Absyn_neq_exp(_tmp3A0,_tmp3A1,0);struct Cyc_Absyn_Exp*
_tmp3A3=Cyc_Absyn_aggrmember_exp(aggrproj(_tmp39F,f,0),Cyc_Toc_val_sp,0);struct
Cyc_Absyn_Stmt*_tmp3A4=Cyc_Absyn_exp_stmt(_tmp3A3,0);struct Cyc_Absyn_Stmt*
_tmp3A5=Cyc_Absyn_ifthenelse_stmt(_tmp3A2,Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),
0);struct Cyc_Absyn_Stmt*_tmp3A6=Cyc_Absyn_declare_stmt(_tmp39E,e1_c_type,(struct
Cyc_Absyn_Exp*)e1,Cyc_Absyn_seq_stmt(_tmp3A5,_tmp3A4,0),0);return Cyc_Toc_stmt_exp_r(
_tmp3A6);}}static int Cyc_Toc_is_tagged_union_project(struct Cyc_Absyn_Exp*e,int*
f_tag,void**tagged_member_type,int clear_read);static int Cyc_Toc_is_tagged_union_project(
struct Cyc_Absyn_Exp*e,int*f_tag,void**tagged_member_type,int clear_read){void*
_tmp3A7=e->r;struct Cyc_Absyn_Exp*_tmp3A8;struct Cyc_Absyn_Exp*_tmp3A9;struct
_dyneither_ptr*_tmp3AA;int _tmp3AB;int*_tmp3AC;struct Cyc_Absyn_Exp*_tmp3AD;struct
_dyneither_ptr*_tmp3AE;int _tmp3AF;int*_tmp3B0;_LL1F5: if(*((int*)_tmp3A7)!= 15)
goto _LL1F7;_tmp3A8=((struct Cyc_Absyn_Cast_e_struct*)_tmp3A7)->f2;_LL1F6: {const
char*_tmpA0D;void*_tmpA0C;(_tmpA0C=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpA0D="cast on lhs!",_tag_dyneither(
_tmpA0D,sizeof(char),13))),_tag_dyneither(_tmpA0C,sizeof(void*),0)));}_LL1F7: if(*((
int*)_tmp3A7)!= 23)goto _LL1F9;_tmp3A9=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp3A7)->f1;_tmp3AA=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp3A7)->f2;_tmp3AB=((
struct Cyc_Absyn_AggrMember_e_struct*)_tmp3A7)->f4;_tmp3AC=(int*)&((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp3A7)->f4;_LL1F8: {void*_tmp3B3=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp3A9->topt))->v);struct Cyc_Absyn_AggrInfo _tmp3B4;union Cyc_Absyn_AggrInfoU
_tmp3B5;_LL1FE: if(_tmp3B3 <= (void*)4)goto _LL200;if(*((int*)_tmp3B3)!= 10)goto
_LL200;_tmp3B4=((struct Cyc_Absyn_AggrType_struct*)_tmp3B3)->f1;_tmp3B5=_tmp3B4.aggr_info;
_LL1FF: {struct Cyc_Absyn_Aggrdecl*_tmp3B6=Cyc_Absyn_get_known_aggrdecl(_tmp3B5);*
f_tag=Cyc_Toc_get_member_offset(_tmp3B6,_tmp3AA);{const char*_tmpA12;void*_tmpA11[
2];struct Cyc_String_pa_struct _tmpA10;struct Cyc_String_pa_struct _tmpA0F;struct
_dyneither_ptr str=(struct _dyneither_ptr)((_tmpA0F.tag=0,((_tmpA0F.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*_tmp3AA),((_tmpA10.tag=0,((_tmpA10.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp3B6->name).f2),((_tmpA11[0]=&
_tmpA10,((_tmpA11[1]=& _tmpA0F,Cyc_aprintf(((_tmpA12="_union_%s_%s",
_tag_dyneither(_tmpA12,sizeof(char),13))),_tag_dyneither(_tmpA11,sizeof(void*),2))))))))))))));{
struct _dyneither_ptr*_tmpA13;*tagged_member_type=Cyc_Absyn_strct(((_tmpA13=
_cycalloc(sizeof(*_tmpA13)),((_tmpA13[0]=str,_tmpA13)))));}if(clear_read)*
_tmp3AC=0;return((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp3B6->impl))->tagged;}}
_LL200:;_LL201: return 0;_LL1FD:;}_LL1F9: if(*((int*)_tmp3A7)!= 24)goto _LL1FB;
_tmp3AD=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp3A7)->f1;_tmp3AE=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp3A7)->f2;_tmp3AF=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp3A7)->f4;_tmp3B0=(
int*)&((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp3A7)->f4;_LL1FA: {void*_tmp3BC=
Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp3AD->topt))->v);
struct Cyc_Absyn_PtrInfo _tmp3BD;void*_tmp3BE;_LL203: if(_tmp3BC <= (void*)4)goto
_LL205;if(*((int*)_tmp3BC)!= 4)goto _LL205;_tmp3BD=((struct Cyc_Absyn_PointerType_struct*)
_tmp3BC)->f1;_tmp3BE=_tmp3BD.elt_typ;_LL204: {void*_tmp3BF=Cyc_Tcutil_compress(
_tmp3BE);struct Cyc_Absyn_AggrInfo _tmp3C0;union Cyc_Absyn_AggrInfoU _tmp3C1;_LL208:
if(_tmp3BF <= (void*)4)goto _LL20A;if(*((int*)_tmp3BF)!= 10)goto _LL20A;_tmp3C0=((
struct Cyc_Absyn_AggrType_struct*)_tmp3BF)->f1;_tmp3C1=_tmp3C0.aggr_info;_LL209: {
struct Cyc_Absyn_Aggrdecl*_tmp3C2=Cyc_Absyn_get_known_aggrdecl(_tmp3C1);*f_tag=
Cyc_Toc_get_member_offset(_tmp3C2,_tmp3AE);{const char*_tmpA18;void*_tmpA17[2];
struct Cyc_String_pa_struct _tmpA16;struct Cyc_String_pa_struct _tmpA15;struct
_dyneither_ptr str=(struct _dyneither_ptr)((_tmpA15.tag=0,((_tmpA15.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*_tmp3AE),((_tmpA16.tag=0,((_tmpA16.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp3C2->name).f2),((_tmpA17[0]=&
_tmpA16,((_tmpA17[1]=& _tmpA15,Cyc_aprintf(((_tmpA18="_union_%s_%s",
_tag_dyneither(_tmpA18,sizeof(char),13))),_tag_dyneither(_tmpA17,sizeof(void*),2))))))))))))));{
struct _dyneither_ptr*_tmpA19;*tagged_member_type=Cyc_Absyn_strct(((_tmpA19=
_cycalloc(sizeof(*_tmpA19)),((_tmpA19[0]=str,_tmpA19)))));}if(clear_read)*
_tmp3B0=0;return((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp3C2->impl))->tagged;}}
_LL20A:;_LL20B: return 0;_LL207:;}_LL205:;_LL206: return 0;_LL202:;}_LL1FB:;_LL1FC:
return 0;_LL1F4:;}static void*Cyc_Toc_tagged_union_assignop(struct Cyc_Absyn_Exp*e1,
void*e1_cyc_type,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,void*e2_cyc_type,
int tag_num,void*member_type);static void*Cyc_Toc_tagged_union_assignop(struct Cyc_Absyn_Exp*
e1,void*e1_cyc_type,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,void*
e2_cyc_type,int tag_num,void*member_type){struct _tuple1*_tmp3C8=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*temp_exp=Cyc_Absyn_var_exp(_tmp3C8,0);struct Cyc_Absyn_Exp*
temp_val=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_val_sp,0);struct Cyc_Absyn_Exp*
temp_tag=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_tag_sp,0);struct Cyc_Absyn_Exp*
f_tag=Cyc_Absyn_signed_int_exp(tag_num,0);struct Cyc_Absyn_Stmt*s3=Cyc_Absyn_exp_stmt(
Cyc_Absyn_assignop_exp(temp_val,popt,e2,0),0);struct Cyc_Absyn_Stmt*s2;if(popt == 
0)s2=Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(temp_tag,f_tag,0),0);else{struct Cyc_Absyn_Exp*
_tmp3C9=Cyc_Absyn_neq_exp(temp_tag,f_tag,0);s2=Cyc_Absyn_ifthenelse_stmt(_tmp3C9,
Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),0);}{struct Cyc_Absyn_Stmt*s1=
Cyc_Absyn_declare_stmt(_tmp3C8,Cyc_Absyn_cstar_typ(member_type,Cyc_Toc_mt_tq),(
struct Cyc_Absyn_Exp*)Cyc_Toc_push_address_exp(e1),Cyc_Absyn_seq_stmt(s2,s3,0),0);
return Cyc_Toc_stmt_exp_r(s1);}}struct _tuple19{struct _tuple1*f1;void*f2;struct Cyc_Absyn_Exp*
f3;};struct _tuple20{struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Exp*f2;};static
void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e);static void
_tmpA78(unsigned int*_tmpA77,unsigned int*_tmpA76,struct _tuple1***_tmpA74){for(*
_tmpA77=0;*_tmpA77 < *_tmpA76;(*_tmpA77)++){(*_tmpA74)[*_tmpA77]=Cyc_Toc_temp_var();}}
static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e){void*
_tmp3CA=e->r;if(e->topt == 0){const char*_tmpA1D;void*_tmpA1C[1];struct Cyc_String_pa_struct
_tmpA1B;(_tmpA1B.tag=0,((_tmpA1B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(e)),((_tmpA1C[0]=& _tmpA1B,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpA1D="exp_to_c: no type for %s",
_tag_dyneither(_tmpA1D,sizeof(char),25))),_tag_dyneither(_tmpA1C,sizeof(void*),1)))))));}{
void*old_typ=(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;void*_tmp3CE=
_tmp3CA;union Cyc_Absyn_Cnst _tmp3CF;int _tmp3D0;struct _tuple1*_tmp3D1;void*_tmp3D2;
struct _tuple1*_tmp3D3;void*_tmp3D4;struct Cyc_List_List*_tmp3D5;struct Cyc_Absyn_Exp*
_tmp3D6;void*_tmp3D7;struct Cyc_Absyn_Exp*_tmp3D8;struct Cyc_Core_Opt*_tmp3D9;
struct Cyc_Absyn_Exp*_tmp3DA;struct Cyc_Absyn_Exp*_tmp3DB;struct Cyc_Absyn_Exp*
_tmp3DC;struct Cyc_Absyn_Exp*_tmp3DD;struct Cyc_Absyn_Exp*_tmp3DE;struct Cyc_Absyn_Exp*
_tmp3DF;struct Cyc_Absyn_Exp*_tmp3E0;struct Cyc_Absyn_Exp*_tmp3E1;struct Cyc_Absyn_Exp*
_tmp3E2;struct Cyc_Absyn_Exp*_tmp3E3;struct Cyc_Absyn_Exp*_tmp3E4;struct Cyc_List_List*
_tmp3E5;struct Cyc_Absyn_Exp*_tmp3E6;struct Cyc_List_List*_tmp3E7;struct Cyc_Absyn_VarargCallInfo*
_tmp3E8;struct Cyc_Absyn_Exp*_tmp3E9;struct Cyc_List_List*_tmp3EA;struct Cyc_Absyn_VarargCallInfo*
_tmp3EB;struct Cyc_Absyn_VarargCallInfo _tmp3EC;int _tmp3ED;struct Cyc_List_List*
_tmp3EE;struct Cyc_Absyn_VarargInfo*_tmp3EF;struct Cyc_Absyn_Exp*_tmp3F0;struct Cyc_Absyn_Exp*
_tmp3F1;struct Cyc_Absyn_Exp*_tmp3F2;struct Cyc_List_List*_tmp3F3;void*_tmp3F4;
void**_tmp3F5;struct Cyc_Absyn_Exp*_tmp3F6;int _tmp3F7;void*_tmp3F8;struct Cyc_Absyn_Exp*
_tmp3F9;struct Cyc_Absyn_Exp*_tmp3FA;struct Cyc_Absyn_Exp*_tmp3FB;struct Cyc_Absyn_Exp*
_tmp3FC;void*_tmp3FD;void*_tmp3FE;void*_tmp3FF;struct _dyneither_ptr*_tmp400;void*
_tmp401;void*_tmp402;unsigned int _tmp403;struct Cyc_Absyn_Exp*_tmp404;struct Cyc_Absyn_Exp*
_tmp405;struct _dyneither_ptr*_tmp406;int _tmp407;int _tmp408;struct Cyc_Absyn_Exp*
_tmp409;struct _dyneither_ptr*_tmp40A;int _tmp40B;int _tmp40C;struct Cyc_Absyn_Exp*
_tmp40D;struct Cyc_Absyn_Exp*_tmp40E;struct Cyc_List_List*_tmp40F;struct Cyc_List_List*
_tmp410;struct Cyc_Absyn_Vardecl*_tmp411;struct Cyc_Absyn_Exp*_tmp412;struct Cyc_Absyn_Exp*
_tmp413;int _tmp414;struct _tuple1*_tmp415;struct Cyc_List_List*_tmp416;struct Cyc_List_List*
_tmp417;struct Cyc_Absyn_Aggrdecl*_tmp418;void*_tmp419;struct Cyc_List_List*
_tmp41A;struct Cyc_List_List*_tmp41B;struct Cyc_Absyn_Datatypedecl*_tmp41C;struct
Cyc_Absyn_Datatypefield*_tmp41D;struct Cyc_List_List*_tmp41E;struct Cyc_Absyn_Datatypedecl*
_tmp41F;struct Cyc_Absyn_Datatypefield*_tmp420;struct Cyc_Absyn_MallocInfo _tmp421;
int _tmp422;struct Cyc_Absyn_Exp*_tmp423;void**_tmp424;struct Cyc_Absyn_Exp*_tmp425;
int _tmp426;struct Cyc_Absyn_Exp*_tmp427;struct Cyc_Absyn_Exp*_tmp428;struct Cyc_Absyn_Exp*
_tmp429;struct _dyneither_ptr*_tmp42A;struct Cyc_Absyn_Stmt*_tmp42B;_LL20D: if(*((
int*)_tmp3CE)!= 0)goto _LL20F;_tmp3CF=((struct Cyc_Absyn_Const_e_struct*)_tmp3CE)->f1;
if((_tmp3CF.Null_c).tag != 1)goto _LL20F;_tmp3D0=(int)(_tmp3CF.Null_c).val;_LL20E: {
struct Cyc_Absyn_Exp*_tmp42C=Cyc_Absyn_uint_exp(0,0);if(Cyc_Tcutil_is_tagged_pointer_typ(
old_typ)){if(Cyc_Toc_is_toplevel(nv))e->r=(Cyc_Toc_make_toplevel_dyn_arr(old_typ,
_tmp42C,_tmp42C))->r;else{struct Cyc_Absyn_Exp*_tmpA1E[3];e->r=Cyc_Toc_fncall_exp_r(
Cyc_Toc__tag_dyneither_e,((_tmpA1E[2]=_tmp42C,((_tmpA1E[1]=_tmp42C,((_tmpA1E[0]=
_tmp42C,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpA1E,sizeof(struct Cyc_Absyn_Exp*),3)))))))));}}else{e->r=(void*)&
Cyc_Toc_zero_exp;}goto _LL20C;}_LL20F: if(*((int*)_tmp3CE)!= 0)goto _LL211;_LL210:
goto _LL20C;_LL211: if(*((int*)_tmp3CE)!= 1)goto _LL213;_tmp3D1=((struct Cyc_Absyn_Var_e_struct*)
_tmp3CE)->f1;_tmp3D2=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp3CE)->f2;_LL212:{
struct _handler_cons _tmp42E;_push_handler(& _tmp42E);{int _tmp430=0;if(setjmp(
_tmp42E.handler))_tmp430=1;if(!_tmp430){e->r=(Cyc_Toc_lookup_varmap(nv,_tmp3D1))->r;;
_pop_handler();}else{void*_tmp42F=(void*)_exn_thrown;void*_tmp432=_tmp42F;_LL268:
if(_tmp432 != Cyc_Dict_Absent)goto _LL26A;_LL269: {const char*_tmpA22;void*_tmpA21[
1];struct Cyc_String_pa_struct _tmpA20;(_tmpA20.tag=0,((_tmpA20.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp3D1)),((
_tmpA21[0]=& _tmpA20,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpA22="Can't find %s in exp_to_c, Var\n",_tag_dyneither(_tmpA22,sizeof(char),
32))),_tag_dyneither(_tmpA21,sizeof(void*),1)))))));}_LL26A:;_LL26B:(void)_throw(
_tmp432);_LL267:;}}}goto _LL20C;_LL213: if(*((int*)_tmp3CE)!= 2)goto _LL215;_tmp3D3=((
struct Cyc_Absyn_UnknownId_e_struct*)_tmp3CE)->f1;_LL214: {const char*_tmpA25;void*
_tmpA24;(_tmpA24=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpA25="unknownid",_tag_dyneither(_tmpA25,sizeof(char),10))),_tag_dyneither(
_tmpA24,sizeof(void*),0)));}_LL215: if(*((int*)_tmp3CE)!= 3)goto _LL217;_tmp3D4=(
void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp3CE)->f1;_tmp3D5=((struct Cyc_Absyn_Primop_e_struct*)
_tmp3CE)->f2;_LL216: {struct Cyc_List_List*_tmp438=((struct Cyc_List_List*(*)(void*(*
f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_get_cyc_typ,
_tmp3D5);((void(*)(void(*f)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*
env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Toc_exp_to_c,nv,_tmp3D5);{void*
_tmp439=_tmp3D4;_LL26D: if((int)_tmp439 != 19)goto _LL26F;_LL26E: {struct Cyc_Absyn_Exp*
arg=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp3D5))->hd;{void*
_tmp43A=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(arg->topt))->v);
struct Cyc_Absyn_ArrayInfo _tmp43B;struct Cyc_Absyn_Exp*_tmp43C;struct Cyc_Absyn_PtrInfo
_tmp43D;void*_tmp43E;struct Cyc_Absyn_PtrAtts _tmp43F;union Cyc_Absyn_Constraint*
_tmp440;union Cyc_Absyn_Constraint*_tmp441;union Cyc_Absyn_Constraint*_tmp442;
_LL282: if(_tmp43A <= (void*)4)goto _LL286;if(*((int*)_tmp43A)!= 7)goto _LL284;
_tmp43B=((struct Cyc_Absyn_ArrayType_struct*)_tmp43A)->f1;_tmp43C=_tmp43B.num_elts;
_LL283: if(!Cyc_Evexp_c_can_eval((struct Cyc_Absyn_Exp*)_check_null(_tmp43C))){
const char*_tmpA28;void*_tmpA27;(_tmpA27=0,Cyc_Tcutil_terr(e->loc,((_tmpA28="can't calculate numelts",
_tag_dyneither(_tmpA28,sizeof(char),24))),_tag_dyneither(_tmpA27,sizeof(void*),0)));}
e->r=_tmp43C->r;goto _LL281;_LL284: if(*((int*)_tmp43A)!= 4)goto _LL286;_tmp43D=((
struct Cyc_Absyn_PointerType_struct*)_tmp43A)->f1;_tmp43E=_tmp43D.elt_typ;_tmp43F=
_tmp43D.ptr_atts;_tmp440=_tmp43F.nullable;_tmp441=_tmp43F.bounds;_tmp442=_tmp43F.zero_term;
_LL285:{void*_tmp445=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp441);struct
Cyc_Absyn_Exp*_tmp446;_LL289: if((int)_tmp445 != 0)goto _LL28B;_LL28A:{struct Cyc_Absyn_Exp*
_tmpA29[2];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__get_dyneither_size_e,((_tmpA29[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp43E),0),((_tmpA29[0]=(struct Cyc_Absyn_Exp*)
_tmp3D5->hd,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpA29,sizeof(struct Cyc_Absyn_Exp*),2)))))));}goto _LL288;_LL28B:
if(_tmp445 <= (void*)1)goto _LL288;if(*((int*)_tmp445)!= 0)goto _LL288;_tmp446=((
struct Cyc_Absyn_Upper_b_struct*)_tmp445)->f1;_LL28C: if(((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,_tmp442)){struct Cyc_Absyn_Exp*function_e=Cyc_Toc_getFunction(&
Cyc_Toc__get_zero_arr_size_functionSet,(struct Cyc_Absyn_Exp*)_tmp3D5->hd);struct
Cyc_Absyn_Exp*_tmpA2A[2];e->r=Cyc_Toc_fncall_exp_r(function_e,((_tmpA2A[1]=
_tmp446,((_tmpA2A[0]=(struct Cyc_Absyn_Exp*)_tmp3D5->hd,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpA2A,sizeof(struct Cyc_Absyn_Exp*),
2)))))));}else{if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
0,_tmp440)){if(!Cyc_Evexp_c_can_eval(_tmp446)){const char*_tmpA2D;void*_tmpA2C;(
_tmpA2C=0,Cyc_Tcutil_terr(e->loc,((_tmpA2D="can't calculate numelts",
_tag_dyneither(_tmpA2D,sizeof(char),24))),_tag_dyneither(_tmpA2C,sizeof(void*),0)));}
e->r=Cyc_Toc_conditional_exp_r(arg,_tmp446,Cyc_Absyn_uint_exp(0,0));}else{e->r=
_tmp446->r;goto _LL288;}}goto _LL288;_LL288:;}goto _LL281;_LL286:;_LL287: {const
char*_tmpA32;void*_tmpA31[2];struct Cyc_String_pa_struct _tmpA30;struct Cyc_String_pa_struct
_tmpA2F;(_tmpA2F.tag=0,((_tmpA2F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(arg->topt))->v)),((
_tmpA30.tag=0,((_tmpA30.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(arg->topt))->v)),((_tmpA31[0]=& _tmpA30,((
_tmpA31[1]=& _tmpA2F,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpA32="size primop applied to non-array %s (%s)",_tag_dyneither(_tmpA32,
sizeof(char),41))),_tag_dyneither(_tmpA31,sizeof(void*),2)))))))))))));}_LL281:;}
goto _LL26C;}_LL26F: if((int)_tmp439 != 0)goto _LL271;_LL270:{void*_tmp44F=Cyc_Tcutil_compress((
void*)((struct Cyc_List_List*)_check_null(_tmp438))->hd);struct Cyc_Absyn_PtrInfo
_tmp450;void*_tmp451;struct Cyc_Absyn_PtrAtts _tmp452;union Cyc_Absyn_Constraint*
_tmp453;union Cyc_Absyn_Constraint*_tmp454;_LL28E: if(_tmp44F <= (void*)4)goto
_LL290;if(*((int*)_tmp44F)!= 4)goto _LL290;_tmp450=((struct Cyc_Absyn_PointerType_struct*)
_tmp44F)->f1;_tmp451=_tmp450.elt_typ;_tmp452=_tmp450.ptr_atts;_tmp453=_tmp452.bounds;
_tmp454=_tmp452.zero_term;_LL28F:{void*_tmp455=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,
_tmp453);struct Cyc_Absyn_Exp*_tmp456;_LL293: if((int)_tmp455 != 0)goto _LL295;
_LL294: {struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp3D5))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct
Cyc_List_List*)_check_null(_tmp3D5->tl))->hd;{struct Cyc_Absyn_Exp*_tmpA33[3];e->r=
Cyc_Toc_fncall_exp_r(Cyc_Toc__dyneither_ptr_plus_e,((_tmpA33[2]=e2,((_tmpA33[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp451),0),((_tmpA33[0]=e1,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpA33,sizeof(struct Cyc_Absyn_Exp*),
3)))))))));}goto _LL292;}_LL295: if(_tmp455 <= (void*)1)goto _LL292;if(*((int*)
_tmp455)!= 0)goto _LL292;_tmp456=((struct Cyc_Absyn_Upper_b_struct*)_tmp455)->f1;
_LL296: if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmp454)){struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp3D5))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct
Cyc_List_List*)_check_null(_tmp3D5->tl))->hd;struct Cyc_Absyn_Exp*_tmpA34[3];e->r=(
Cyc_Absyn_fncall_exp(Cyc_Toc_getFunction(& Cyc_Toc__zero_arr_plus_functionSet,e1),((
_tmpA34[2]=e2,((_tmpA34[1]=_tmp456,((_tmpA34[0]=e1,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpA34,sizeof(struct Cyc_Absyn_Exp*),
3)))))))),0))->r;}goto _LL292;_LL292:;}goto _LL28D;_LL290:;_LL291: goto _LL28D;
_LL28D:;}goto _LL26C;_LL271: if((int)_tmp439 != 2)goto _LL273;_LL272: {void*elt_typ=(
void*)0;if(Cyc_Tcutil_is_tagged_pointer_typ_elt((void*)((struct Cyc_List_List*)
_check_null(_tmp438))->hd,& elt_typ)){struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp3D5))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp3D5->tl))->hd;if(Cyc_Tcutil_is_tagged_pointer_typ((
void*)((struct Cyc_List_List*)_check_null(_tmp438->tl))->hd)){e1->r=Cyc_Toc_aggrmember_exp_r(
Cyc_Absyn_new_exp(e1->r,0),Cyc_Toc_curr_sp);e2->r=Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp(
e2->r,0),Cyc_Toc_curr_sp);e->r=(Cyc_Absyn_divide_exp(Cyc_Absyn_copy_exp(e),Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c(elt_typ),0),0))->r;}else{struct Cyc_Absyn_Exp*_tmpA35[3];e->r=
Cyc_Toc_fncall_exp_r(Cyc_Toc__dyneither_ptr_plus_e,((_tmpA35[2]=Cyc_Absyn_prim1_exp((
void*)2,e2,0),((_tmpA35[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0),((
_tmpA35[0]=e1,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpA35,sizeof(struct Cyc_Absyn_Exp*),3)))))))));}}goto _LL26C;}
_LL273: if((int)_tmp439 != 5)goto _LL275;_LL274: goto _LL276;_LL275: if((int)_tmp439 != 
6)goto _LL277;_LL276: goto _LL278;_LL277: if((int)_tmp439 != 7)goto _LL279;_LL278: goto
_LL27A;_LL279: if((int)_tmp439 != 9)goto _LL27B;_LL27A: goto _LL27C;_LL27B: if((int)
_tmp439 != 8)goto _LL27D;_LL27C: goto _LL27E;_LL27D: if((int)_tmp439 != 10)goto _LL27F;
_LL27E: {struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp3D5))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct
Cyc_List_List*)_check_null(_tmp3D5->tl))->hd;void*t1=(void*)((struct Cyc_List_List*)
_check_null(_tmp438))->hd;void*t2=(void*)((struct Cyc_List_List*)_check_null(
_tmp438->tl))->hd;if(Cyc_Tcutil_is_tagged_pointer_typ(t1))e1->r=Cyc_Toc_aggrmember_exp_r(
Cyc_Absyn_new_exp(e1->r,0),Cyc_Toc_curr_sp);if(Cyc_Tcutil_is_tagged_pointer_typ(
t2))e2->r=Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp(e2->r,0),Cyc_Toc_curr_sp);
goto _LL26C;}_LL27F:;_LL280: goto _LL26C;_LL26C:;}goto _LL20C;}_LL217: if(*((int*)
_tmp3CE)!= 5)goto _LL219;_tmp3D6=((struct Cyc_Absyn_Increment_e_struct*)_tmp3CE)->f1;
_tmp3D7=(void*)((struct Cyc_Absyn_Increment_e_struct*)_tmp3CE)->f2;_LL218: {void*
e2_cyc_typ=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp3D6->topt))->v;void*
ptr_type=(void*)0;void*elt_type=(void*)0;int is_dyneither=0;const char*_tmpA36;
struct _dyneither_ptr incr_str=(_tmpA36="increment",_tag_dyneither(_tmpA36,sizeof(
char),10));if(_tmp3D7 == (void*)2  || _tmp3D7 == (void*)3){const char*_tmpA37;
incr_str=((_tmpA37="decrement",_tag_dyneither(_tmpA37,sizeof(char),10)));}if(Cyc_Tcutil_is_zero_ptr_deref(
_tmp3D6,& ptr_type,& is_dyneither,& elt_type)){{const char*_tmpA3B;void*_tmpA3A[1];
struct Cyc_String_pa_struct _tmpA39;(_tmpA39.tag=0,((_tmpA39.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)incr_str),((_tmpA3A[0]=& _tmpA39,Cyc_Tcutil_terr(
e->loc,((_tmpA3B="in-place %s is not supported when dereferencing a zero-terminated pointer",
_tag_dyneither(_tmpA3B,sizeof(char),74))),_tag_dyneither(_tmpA3A,sizeof(void*),1)))))));}{
const char*_tmpA3E;void*_tmpA3D;(_tmpA3D=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpA3E="in-place inc/dec on zero-term",
_tag_dyneither(_tmpA3E,sizeof(char),30))),_tag_dyneither(_tmpA3D,sizeof(void*),0)));}}{
void*tunion_member_type=(void*)0;int f_tag=0;if(Cyc_Toc_is_tagged_union_project(
_tmp3D6,& f_tag,& tunion_member_type,1)){struct Cyc_Absyn_Exp*_tmp460=Cyc_Absyn_signed_int_exp(
1,0);{struct Cyc_Core_Opt*_tmpA3F;_tmp460->topt=((_tmpA3F=_cycalloc(sizeof(*
_tmpA3F)),((_tmpA3F->v=(void*)Cyc_Absyn_sint_typ,_tmpA3F))));}{void*_tmp462=
_tmp3D7;_LL298: if((int)_tmp462 != 0)goto _LL29A;_LL299:{struct Cyc_Absyn_AssignOp_e_struct
_tmpA45;struct Cyc_Core_Opt*_tmpA44;struct Cyc_Absyn_AssignOp_e_struct*_tmpA43;e->r=(
void*)((_tmpA43=_cycalloc(sizeof(*_tmpA43)),((_tmpA43[0]=((_tmpA45.tag=4,((
_tmpA45.f1=_tmp3D6,((_tmpA45.f2=((_tmpA44=_cycalloc(sizeof(*_tmpA44)),((_tmpA44->v=(
void*)((void*)0),_tmpA44)))),((_tmpA45.f3=_tmp460,_tmpA45)))))))),_tmpA43))));}
Cyc_Toc_exp_to_c(nv,e);return;_LL29A: if((int)_tmp462 != 2)goto _LL29C;_LL29B:{
struct Cyc_Absyn_AssignOp_e_struct _tmpA4B;struct Cyc_Core_Opt*_tmpA4A;struct Cyc_Absyn_AssignOp_e_struct*
_tmpA49;e->r=(void*)((_tmpA49=_cycalloc(sizeof(*_tmpA49)),((_tmpA49[0]=((_tmpA4B.tag=
4,((_tmpA4B.f1=_tmp3D6,((_tmpA4B.f2=((_tmpA4A=_cycalloc(sizeof(*_tmpA4A)),((
_tmpA4A->v=(void*)((void*)2),_tmpA4A)))),((_tmpA4B.f3=_tmp460,_tmpA4B)))))))),
_tmpA49))));}Cyc_Toc_exp_to_c(nv,e);return;_LL29C:;_LL29D:{const char*_tmpA4F;
void*_tmpA4E[1];struct Cyc_String_pa_struct _tmpA4D;(_tmpA4D.tag=0,((_tmpA4D.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)incr_str),((_tmpA4E[0]=& _tmpA4D,Cyc_Tcutil_terr(
e->loc,((_tmpA4F="in-place post-%s is not supported on @tagged union members",
_tag_dyneither(_tmpA4F,sizeof(char),59))),_tag_dyneither(_tmpA4E,sizeof(void*),1)))))));}{
const char*_tmpA52;void*_tmpA51;(_tmpA51=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpA52="in-place inc/dec on @tagged union",
_tag_dyneither(_tmpA52,sizeof(char),34))),_tag_dyneither(_tmpA51,sizeof(void*),0)));}
_LL297:;}}Cyc_Toc_exp_to_c(nv,_tmp3D6);{void*elt_typ=(void*)0;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(
old_typ,& elt_typ)){struct Cyc_Absyn_Exp*fn_e;int change=1;fn_e=(_tmp3D7 == (void*)1
 || _tmp3D7 == (void*)3)?Cyc_Toc__dyneither_ptr_inplace_plus_post_e: Cyc_Toc__dyneither_ptr_inplace_plus_e;
if(_tmp3D7 == (void*)2  || _tmp3D7 == (void*)3)change=- 1;{struct Cyc_Absyn_Exp*
_tmpA53[3];e->r=Cyc_Toc_fncall_exp_r(fn_e,((_tmpA53[2]=Cyc_Absyn_signed_int_exp(
change,0),((_tmpA53[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0),((
_tmpA53[0]=Cyc_Toc_push_address_exp(_tmp3D6),((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpA53,sizeof(struct Cyc_Absyn_Exp*),
3)))))))));}}else{if(Cyc_Tcutil_is_zero_pointer_typ_elt(old_typ,& elt_typ)){
struct Cyc_Absyn_Exp*fn_e;{void*_tmp46F=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp3D6->topt))->v);void*_tmp470;int _tmp471;_LL29F: if(_tmp46F <= (
void*)4)goto _LL2A1;if(*((int*)_tmp46F)!= 5)goto _LL2A1;_tmp470=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp46F)->f2;_LL2A0:{void*_tmp472=_tmp470;_LL2AA: if((int)_tmp472 != 0)goto _LL2AC;
_LL2AB: fn_e=_tmp3D7 == (void*)1?Cyc_Toc__zero_arr_inplace_plus_post_char_e: Cyc_Toc__zero_arr_inplace_plus_char_e;
goto _LL2A9;_LL2AC: if((int)_tmp472 != 1)goto _LL2AE;_LL2AD: fn_e=_tmp3D7 == (void*)1?
Cyc_Toc__zero_arr_inplace_plus_post_short_e: Cyc_Toc__zero_arr_inplace_plus_short_e;
goto _LL2A9;_LL2AE: if((int)_tmp472 != 2)goto _LL2B0;_LL2AF: fn_e=_tmp3D7 == (void*)1?
Cyc_Toc__zero_arr_inplace_plus_post_int_e: Cyc_Toc__zero_arr_inplace_plus_int_e;
goto _LL2A9;_LL2B0:;_LL2B1: {struct Cyc_Core_Impossible_struct _tmpA59;const char*
_tmpA58;struct Cyc_Core_Impossible_struct*_tmpA57;(int)_throw((void*)((_tmpA57=
_cycalloc(sizeof(*_tmpA57)),((_tmpA57[0]=((_tmpA59.tag=Cyc_Core_Impossible,((
_tmpA59.f1=((_tmpA58="impossible IntType (not char, short or int)",
_tag_dyneither(_tmpA58,sizeof(char),44))),_tmpA59)))),_tmpA57)))));}_LL2A9:;}
goto _LL29E;_LL2A1: if((int)_tmp46F != 1)goto _LL2A3;_LL2A2: fn_e=_tmp3D7 == (void*)1?
Cyc_Toc__zero_arr_inplace_plus_post_float_e: Cyc_Toc__zero_arr_inplace_plus_float_e;
goto _LL29E;_LL2A3: if(_tmp46F <= (void*)4)goto _LL2A7;if(*((int*)_tmp46F)!= 6)goto
_LL2A5;_tmp471=((struct Cyc_Absyn_DoubleType_struct*)_tmp46F)->f1;_LL2A4: switch(
_tmp471){case 1: _LL2B2: fn_e=_tmp3D7 == (void*)1?Cyc_Toc__zero_arr_inplace_plus_post_longdouble_e:
Cyc_Toc__zero_arr_inplace_plus_longdouble_e;break;default: _LL2B3: fn_e=_tmp3D7 == (
void*)1?Cyc_Toc__zero_arr_inplace_plus_post_double_e: Cyc_Toc__zero_arr_inplace_plus_double_e;}
goto _LL29E;_LL2A5: if(*((int*)_tmp46F)!= 4)goto _LL2A7;_LL2A6: fn_e=_tmp3D7 == (void*)
1?Cyc_Toc__zero_arr_inplace_plus_post_voidstar_e: Cyc_Toc__zero_arr_inplace_plus_voidstar_e;
goto _LL29E;_LL2A7:;_LL2A8: {struct Cyc_Core_Impossible_struct _tmpA5F;const char*
_tmpA5E;struct Cyc_Core_Impossible_struct*_tmpA5D;(int)_throw((void*)((_tmpA5D=
_cycalloc(sizeof(*_tmpA5D)),((_tmpA5D[0]=((_tmpA5F.tag=Cyc_Core_Impossible,((
_tmpA5F.f1=((_tmpA5E="impossible expression type (not int, float, double, or pointer)",
_tag_dyneither(_tmpA5E,sizeof(char),64))),_tmpA5F)))),_tmpA5D)))));}_LL29E:;}{
struct Cyc_Absyn_Exp*_tmpA60[2];e->r=Cyc_Toc_fncall_exp_r(fn_e,((_tmpA60[1]=Cyc_Absyn_signed_int_exp(
1,0),((_tmpA60[0]=_tmp3D6,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpA60,sizeof(struct Cyc_Absyn_Exp*),2)))))));}}else{if(elt_typ == (
void*)0  && !Cyc_Absyn_is_lvalue(_tmp3D6)){Cyc_Toc_lvalue_assign(_tmp3D6,0,Cyc_Toc_incr_lvalue,
_tmp3D7);e->r=_tmp3D6->r;}}}goto _LL20C;}}}_LL219: if(*((int*)_tmp3CE)!= 4)goto
_LL21B;_tmp3D8=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp3CE)->f1;_tmp3D9=((
struct Cyc_Absyn_AssignOp_e_struct*)_tmp3CE)->f2;_tmp3DA=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmp3CE)->f3;_LL21A: {void*e1_old_typ=(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp3D8->topt))->v;void*e2_old_typ=(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp3DA->topt))->v;int f_tag=0;void*tagged_member_struct_type=(void*)0;if(Cyc_Toc_is_tagged_union_project(
_tmp3D8,& f_tag,& tagged_member_struct_type,1)){Cyc_Toc_exp_to_c(nv,_tmp3D8);Cyc_Toc_exp_to_c(
nv,_tmp3DA);e->r=Cyc_Toc_tagged_union_assignop(_tmp3D8,e1_old_typ,_tmp3D9,
_tmp3DA,e2_old_typ,f_tag,tagged_member_struct_type);return;}{void*ptr_type=(void*)
0;void*elt_type=(void*)0;int is_dyneither=0;if(Cyc_Tcutil_is_zero_ptr_deref(
_tmp3D8,& ptr_type,& is_dyneither,& elt_type)){Cyc_Toc_zero_ptr_assign_to_c(nv,e,
_tmp3D8,_tmp3D9,_tmp3DA,ptr_type,is_dyneither,elt_type);return;}{int e1_poly=Cyc_Toc_is_poly_project(
_tmp3D8);Cyc_Toc_exp_to_c(nv,_tmp3D8);Cyc_Toc_exp_to_c(nv,_tmp3DA);{int done=0;
if(_tmp3D9 != 0){void*elt_typ=(void*)0;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(
old_typ,& elt_typ)){struct Cyc_Absyn_Exp*change;{void*_tmp47B=(void*)_tmp3D9->v;
_LL2B6: if((int)_tmp47B != 0)goto _LL2B8;_LL2B7: change=_tmp3DA;goto _LL2B5;_LL2B8:
if((int)_tmp47B != 2)goto _LL2BA;_LL2B9: change=Cyc_Absyn_prim1_exp((void*)2,
_tmp3DA,0);goto _LL2B5;_LL2BA:;_LL2BB: {const char*_tmpA63;void*_tmpA62;(_tmpA62=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpA63="bad t ? pointer arithmetic",_tag_dyneither(_tmpA63,sizeof(char),27))),
_tag_dyneither(_tmpA62,sizeof(void*),0)));}_LL2B5:;}done=1;{struct Cyc_Absyn_Exp*
_tmp47E=Cyc_Toc__dyneither_ptr_inplace_plus_e;struct Cyc_Absyn_Exp*_tmpA64[3];e->r=
Cyc_Toc_fncall_exp_r(_tmp47E,((_tmpA64[2]=change,((_tmpA64[1]=Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c(elt_typ),0),((_tmpA64[0]=Cyc_Toc_push_address_exp(_tmp3D8),((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmpA64,sizeof(struct Cyc_Absyn_Exp*),3)))))))));}}else{if(Cyc_Tcutil_is_zero_pointer_typ_elt(
old_typ,& elt_typ)){void*_tmp480=(void*)_tmp3D9->v;_LL2BD: if((int)_tmp480 != 0)
goto _LL2BF;_LL2BE: done=1;{struct Cyc_Absyn_Exp*_tmpA65[2];e->r=Cyc_Toc_fncall_exp_r(
Cyc_Toc_getFunction(& Cyc_Toc__zero_arr_inplace_plus_functionSet,_tmp3D8),((
_tmpA65[1]=_tmp3DA,((_tmpA65[0]=_tmp3D8,((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpA65,sizeof(struct Cyc_Absyn_Exp*),
2)))))));}goto _LL2BC;_LL2BF:;_LL2C0: {const char*_tmpA68;void*_tmpA67;(_tmpA67=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpA68="bad zero-terminated pointer arithmetic",_tag_dyneither(_tmpA68,sizeof(
char),39))),_tag_dyneither(_tmpA67,sizeof(void*),0)));}_LL2BC:;}}}if(!done){if(
e1_poly)_tmp3DA->r=Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(
_tmp3DA->r,0));if(!Cyc_Absyn_is_lvalue(_tmp3D8)){{struct _tuple18 _tmpA6B;struct
_tuple18*_tmpA6A;((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct
Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,struct _tuple18*),struct _tuple18*f_env))
Cyc_Toc_lvalue_assign)(_tmp3D8,0,Cyc_Toc_assignop_lvalue,((_tmpA6A=_cycalloc(
sizeof(struct _tuple18)* 1),((_tmpA6A[0]=((_tmpA6B.f1=_tmp3D9,((_tmpA6B.f2=
_tmp3DA,_tmpA6B)))),_tmpA6A)))));}e->r=_tmp3D8->r;}}goto _LL20C;}}}}_LL21B: if(*((
int*)_tmp3CE)!= 6)goto _LL21D;_tmp3DB=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp3CE)->f1;_tmp3DC=((struct Cyc_Absyn_Conditional_e_struct*)_tmp3CE)->f2;
_tmp3DD=((struct Cyc_Absyn_Conditional_e_struct*)_tmp3CE)->f3;_LL21C: Cyc_Toc_exp_to_c(
nv,_tmp3DB);Cyc_Toc_exp_to_c(nv,_tmp3DC);Cyc_Toc_exp_to_c(nv,_tmp3DD);goto _LL20C;
_LL21D: if(*((int*)_tmp3CE)!= 7)goto _LL21F;_tmp3DE=((struct Cyc_Absyn_And_e_struct*)
_tmp3CE)->f1;_tmp3DF=((struct Cyc_Absyn_And_e_struct*)_tmp3CE)->f2;_LL21E: Cyc_Toc_exp_to_c(
nv,_tmp3DE);Cyc_Toc_exp_to_c(nv,_tmp3DF);goto _LL20C;_LL21F: if(*((int*)_tmp3CE)!= 
8)goto _LL221;_tmp3E0=((struct Cyc_Absyn_Or_e_struct*)_tmp3CE)->f1;_tmp3E1=((
struct Cyc_Absyn_Or_e_struct*)_tmp3CE)->f2;_LL220: Cyc_Toc_exp_to_c(nv,_tmp3E0);
Cyc_Toc_exp_to_c(nv,_tmp3E1);goto _LL20C;_LL221: if(*((int*)_tmp3CE)!= 9)goto
_LL223;_tmp3E2=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp3CE)->f1;_tmp3E3=((struct
Cyc_Absyn_SeqExp_e_struct*)_tmp3CE)->f2;_LL222: Cyc_Toc_exp_to_c(nv,_tmp3E2);Cyc_Toc_exp_to_c(
nv,_tmp3E3);goto _LL20C;_LL223: if(*((int*)_tmp3CE)!= 10)goto _LL225;_tmp3E4=((
struct Cyc_Absyn_UnknownCall_e_struct*)_tmp3CE)->f1;_tmp3E5=((struct Cyc_Absyn_UnknownCall_e_struct*)
_tmp3CE)->f2;_LL224: _tmp3E6=_tmp3E4;_tmp3E7=_tmp3E5;goto _LL226;_LL225: if(*((int*)
_tmp3CE)!= 11)goto _LL227;_tmp3E6=((struct Cyc_Absyn_FnCall_e_struct*)_tmp3CE)->f1;
_tmp3E7=((struct Cyc_Absyn_FnCall_e_struct*)_tmp3CE)->f2;_tmp3E8=((struct Cyc_Absyn_FnCall_e_struct*)
_tmp3CE)->f3;if(_tmp3E8 != 0)goto _LL227;_LL226: Cyc_Toc_exp_to_c(nv,_tmp3E6);((
void(*)(void(*f)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,
struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Toc_exp_to_c,nv,_tmp3E7);goto _LL20C;
_LL227: if(*((int*)_tmp3CE)!= 11)goto _LL229;_tmp3E9=((struct Cyc_Absyn_FnCall_e_struct*)
_tmp3CE)->f1;_tmp3EA=((struct Cyc_Absyn_FnCall_e_struct*)_tmp3CE)->f2;_tmp3EB=((
struct Cyc_Absyn_FnCall_e_struct*)_tmp3CE)->f3;if(_tmp3EB == 0)goto _LL229;_tmp3EC=*
_tmp3EB;_tmp3ED=_tmp3EC.num_varargs;_tmp3EE=_tmp3EC.injectors;_tmp3EF=_tmp3EC.vai;
_LL228: {struct Cyc_Toc_Env _tmp487;struct _RegionHandle*_tmp488;struct Cyc_Toc_Env*
_tmp486=nv;_tmp487=*_tmp486;_tmp488=_tmp487.rgn;{struct _tuple1*argv=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*argvexp=Cyc_Absyn_var_exp(argv,0);struct Cyc_Absyn_Exp*
num_varargs_exp=Cyc_Absyn_uint_exp((unsigned int)_tmp3ED,0);void*cva_type=Cyc_Toc_typ_to_c(
_tmp3EF->type);void*arr_type=Cyc_Absyn_array_typ(cva_type,Cyc_Toc_mt_tq,(struct
Cyc_Absyn_Exp*)num_varargs_exp,Cyc_Absyn_false_conref,0);int num_args=((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmp3EA);int num_normargs=num_args - 
_tmp3ED;struct Cyc_List_List*new_args=0;{int i=0;for(0;i < num_normargs;(++ i,
_tmp3EA=_tmp3EA->tl)){Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp3EA))->hd);{struct Cyc_List_List*_tmpA6C;new_args=((_tmpA6C=
_cycalloc(sizeof(*_tmpA6C)),((_tmpA6C->hd=(struct Cyc_Absyn_Exp*)_tmp3EA->hd,((
_tmpA6C->tl=new_args,_tmpA6C))))));}}}{struct Cyc_Absyn_Exp*_tmpA6F[3];struct Cyc_List_List*
_tmpA6E;new_args=((_tmpA6E=_cycalloc(sizeof(*_tmpA6E)),((_tmpA6E->hd=Cyc_Absyn_fncall_exp(
Cyc_Toc__tag_dyneither_e,((_tmpA6F[2]=num_varargs_exp,((_tmpA6F[1]=Cyc_Absyn_sizeoftyp_exp(
cva_type,0),((_tmpA6F[0]=argvexp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))
Cyc_List_list)(_tag_dyneither(_tmpA6F,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0),((
_tmpA6E->tl=new_args,_tmpA6E))))));}new_args=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(new_args);Cyc_Toc_exp_to_c(nv,_tmp3E9);{struct Cyc_Absyn_Stmt*
s=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(_tmp3E9,new_args,0),0);if(_tmp3EF->inject){
struct Cyc_Absyn_Datatypedecl*tud;{void*_tmp48C=Cyc_Tcutil_compress(_tmp3EF->type);
struct Cyc_Absyn_DatatypeInfo _tmp48D;union Cyc_Absyn_DatatypeInfoU _tmp48E;struct
Cyc_Absyn_Datatypedecl**_tmp48F;struct Cyc_Absyn_Datatypedecl*_tmp490;_LL2C2: if(
_tmp48C <= (void*)4)goto _LL2C4;if(*((int*)_tmp48C)!= 2)goto _LL2C4;_tmp48D=((
struct Cyc_Absyn_DatatypeType_struct*)_tmp48C)->f1;_tmp48E=_tmp48D.datatype_info;
if((_tmp48E.KnownDatatype).tag != 2)goto _LL2C4;_tmp48F=(struct Cyc_Absyn_Datatypedecl**)(
_tmp48E.KnownDatatype).val;_tmp490=*_tmp48F;_LL2C3: tud=_tmp490;goto _LL2C1;_LL2C4:;
_LL2C5: {const char*_tmpA72;void*_tmpA71;(_tmpA71=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpA72="toc: unknown datatype in vararg with inject",
_tag_dyneither(_tmpA72,sizeof(char),44))),_tag_dyneither(_tmpA71,sizeof(void*),0)));}
_LL2C1:;}{unsigned int _tmpA77;unsigned int _tmpA76;struct _dyneither_ptr _tmpA75;
struct _tuple1**_tmpA74;unsigned int _tmpA73;struct _dyneither_ptr vs=(_tmpA73=(
unsigned int)_tmp3ED,((_tmpA74=(struct _tuple1**)_region_malloc(_tmp488,
_check_times(sizeof(struct _tuple1*),_tmpA73)),((_tmpA75=_tag_dyneither(_tmpA74,
sizeof(struct _tuple1*),_tmpA73),((((_tmpA76=_tmpA73,_tmpA78(& _tmpA77,& _tmpA76,&
_tmpA74))),_tmpA75)))))));if(_tmp3ED != 0){struct Cyc_List_List*_tmp493=0;{int i=
_tmp3ED - 1;for(0;i >= 0;-- i){struct Cyc_List_List*_tmpA79;_tmp493=((_tmpA79=
_cycalloc(sizeof(*_tmpA79)),((_tmpA79->hd=Cyc_Toc_make_dle(Cyc_Absyn_address_exp(
Cyc_Absyn_var_exp(*((struct _tuple1**)_check_dyneither_subscript(vs,sizeof(struct
_tuple1*),i)),0),0)),((_tmpA79->tl=_tmp493,_tmpA79))))));}}s=Cyc_Absyn_declare_stmt(
argv,arr_type,(struct Cyc_Absyn_Exp*)Cyc_Absyn_unresolvedmem_exp(0,_tmp493,0),s,0);{
int i=0;for(0;_tmp3EA != 0;(((_tmp3EA=_tmp3EA->tl,_tmp3EE=_tmp3EE->tl)),++ i)){
struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)_tmp3EA->hd;void*arg_type=(void*)((
struct Cyc_Core_Opt*)_check_null(arg->topt))->v;struct _tuple1*var=*((struct
_tuple1**)_check_dyneither_subscript(vs,sizeof(struct _tuple1*),i));struct Cyc_Absyn_Exp*
varexp=Cyc_Absyn_var_exp(var,0);struct Cyc_Absyn_Datatypefield _tmp496;struct
_tuple1*_tmp497;struct Cyc_List_List*_tmp498;struct Cyc_Absyn_Datatypefield*
_tmp495=(struct Cyc_Absyn_Datatypefield*)((struct Cyc_List_List*)_check_null(
_tmp3EE))->hd;_tmp496=*_tmp495;_tmp497=_tmp496.name;_tmp498=_tmp496.typs;{void*
field_typ=Cyc_Toc_typ_to_c((*((struct _tuple10*)((struct Cyc_List_List*)
_check_null(_tmp498))->hd)).f2);Cyc_Toc_exp_to_c(nv,arg);if(Cyc_Toc_is_void_star_or_tvar(
field_typ))arg=Cyc_Toc_cast_it(field_typ,arg);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(
Cyc_Absyn_aggrmember_exp(varexp,Cyc_Absyn_fieldname(1),0),arg,0),s,0);s=Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt(Cyc_Absyn_aggrmember_exp(varexp,Cyc_Toc_tag_sp,0),Cyc_Toc_datatype_tag(
tud,_tmp497,1),0),s,0);{const char*_tmpA7A;s=Cyc_Absyn_declare_stmt(var,Cyc_Absyn_strctq(
Cyc_Toc_collapse_qvar_tag(_tmp497,((_tmpA7A="_struct",_tag_dyneither(_tmpA7A,
sizeof(char),8))))),0,s,0);}}}}}else{struct _tuple16*_tmpA7B[3];struct Cyc_List_List*
_tmp49A=(_tmpA7B[2]=Cyc_Toc_make_dle(Cyc_Absyn_uint_exp(0,0)),((_tmpA7B[1]=Cyc_Toc_make_dle(
Cyc_Absyn_uint_exp(0,0)),((_tmpA7B[0]=Cyc_Toc_make_dle(Cyc_Absyn_uint_exp(0,0)),((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmpA7B,sizeof(struct _tuple16*),3)))))));s=Cyc_Absyn_declare_stmt(argv,Cyc_Absyn_void_star_typ(),(
struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(0,0),s,0);}}}else{{int i=0;for(0;_tmp3EA
!= 0;(_tmp3EA=_tmp3EA->tl,++ i)){Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)
_tmp3EA->hd);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(
argvexp,Cyc_Absyn_uint_exp((unsigned int)i,0),0),(struct Cyc_Absyn_Exp*)_tmp3EA->hd,
0),s,0);}}s=Cyc_Absyn_declare_stmt(argv,arr_type,0,s,0);}e->r=Cyc_Toc_stmt_exp_r(
s);}}goto _LL20C;}_LL229: if(*((int*)_tmp3CE)!= 12)goto _LL22B;_tmp3F0=((struct Cyc_Absyn_Throw_e_struct*)
_tmp3CE)->f1;_LL22A: Cyc_Toc_exp_to_c(nv,_tmp3F0);e->r=(Cyc_Toc_array_to_ptr_cast(
Cyc_Toc_typ_to_c(old_typ),Cyc_Toc_newthrow_exp(_tmp3F0),0))->r;goto _LL20C;_LL22B:
if(*((int*)_tmp3CE)!= 13)goto _LL22D;_tmp3F1=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmp3CE)->f1;_LL22C: Cyc_Toc_exp_to_c(nv,_tmp3F1);goto _LL20C;_LL22D: if(*((int*)
_tmp3CE)!= 14)goto _LL22F;_tmp3F2=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp3CE)->f1;
_tmp3F3=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp3CE)->f2;_LL22E: Cyc_Toc_exp_to_c(
nv,_tmp3F2);for(0;_tmp3F3 != 0;_tmp3F3=_tmp3F3->tl){void*k=Cyc_Tcutil_typ_kind((
void*)_tmp3F3->hd);if(((k != (void*)6  && k != (void*)3) && k != (void*)4) && k != (
void*)5){{void*_tmp4A0=Cyc_Tcutil_compress((void*)_tmp3F3->hd);_LL2C7: if(_tmp4A0
<= (void*)4)goto _LL2CB;if(*((int*)_tmp4A0)!= 1)goto _LL2C9;_LL2C8: goto _LL2CA;
_LL2C9: if(*((int*)_tmp4A0)!= 2)goto _LL2CB;_LL2CA: continue;_LL2CB:;_LL2CC: e->r=(
Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v),_tmp3F2,0))->r;goto _LL2C6;_LL2C6:;}break;}}goto _LL20C;
_LL22F: if(*((int*)_tmp3CE)!= 15)goto _LL231;_tmp3F4=(void**)&((struct Cyc_Absyn_Cast_e_struct*)
_tmp3CE)->f1;_tmp3F5=(void**)((void**)&((struct Cyc_Absyn_Cast_e_struct*)_tmp3CE)->f1);
_tmp3F6=((struct Cyc_Absyn_Cast_e_struct*)_tmp3CE)->f2;_tmp3F7=((struct Cyc_Absyn_Cast_e_struct*)
_tmp3CE)->f3;_tmp3F8=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp3CE)->f4;_LL230: {
void*old_t2=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp3F6->topt))->v;void*
new_typ=*_tmp3F5;*_tmp3F5=Cyc_Toc_typ_to_c(new_typ);Cyc_Toc_exp_to_c(nv,_tmp3F6);{
struct _tuple0 _tmpA7C;struct _tuple0 _tmp4A2=(_tmpA7C.f1=Cyc_Tcutil_compress(old_t2),((
_tmpA7C.f2=Cyc_Tcutil_compress(new_typ),_tmpA7C)));void*_tmp4A3;struct Cyc_Absyn_PtrInfo
_tmp4A4;void*_tmp4A5;struct Cyc_Absyn_PtrInfo _tmp4A6;void*_tmp4A7;struct Cyc_Absyn_PtrInfo
_tmp4A8;void*_tmp4A9;_LL2CE: _tmp4A3=_tmp4A2.f1;if(_tmp4A3 <= (void*)4)goto _LL2D0;
if(*((int*)_tmp4A3)!= 4)goto _LL2D0;_tmp4A4=((struct Cyc_Absyn_PointerType_struct*)
_tmp4A3)->f1;_tmp4A5=_tmp4A2.f2;if(_tmp4A5 <= (void*)4)goto _LL2D0;if(*((int*)
_tmp4A5)!= 4)goto _LL2D0;_tmp4A6=((struct Cyc_Absyn_PointerType_struct*)_tmp4A5)->f1;
_LL2CF: {int _tmp4AA=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
0,(_tmp4A4.ptr_atts).nullable);int _tmp4AB=((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,(_tmp4A6.ptr_atts).nullable);void*_tmp4AC=Cyc_Absyn_conref_def(
Cyc_Absyn_bounds_one,(_tmp4A4.ptr_atts).bounds);void*_tmp4AD=Cyc_Absyn_conref_def(
Cyc_Absyn_bounds_one,(_tmp4A6.ptr_atts).bounds);int _tmp4AE=((int(*)(int y,union
Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp4A4.ptr_atts).zero_term);
int _tmp4AF=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(
_tmp4A6.ptr_atts).zero_term);{struct _tuple0 _tmpA7D;struct _tuple0 _tmp4B1=(_tmpA7D.f1=
_tmp4AC,((_tmpA7D.f2=_tmp4AD,_tmpA7D)));void*_tmp4B2;struct Cyc_Absyn_Exp*_tmp4B3;
void*_tmp4B4;struct Cyc_Absyn_Exp*_tmp4B5;void*_tmp4B6;struct Cyc_Absyn_Exp*
_tmp4B7;void*_tmp4B8;void*_tmp4B9;void*_tmp4BA;struct Cyc_Absyn_Exp*_tmp4BB;void*
_tmp4BC;void*_tmp4BD;_LL2D5: _tmp4B2=_tmp4B1.f1;if(_tmp4B2 <= (void*)1)goto _LL2D7;
if(*((int*)_tmp4B2)!= 0)goto _LL2D7;_tmp4B3=((struct Cyc_Absyn_Upper_b_struct*)
_tmp4B2)->f1;_tmp4B4=_tmp4B1.f2;if(_tmp4B4 <= (void*)1)goto _LL2D7;if(*((int*)
_tmp4B4)!= 0)goto _LL2D7;_tmp4B5=((struct Cyc_Absyn_Upper_b_struct*)_tmp4B4)->f1;
_LL2D6: if((!Cyc_Evexp_c_can_eval(_tmp4B3) || !Cyc_Evexp_c_can_eval(_tmp4B5)) && 
!Cyc_Evexp_same_const_exp(_tmp4B3,_tmp4B5)){const char*_tmpA80;void*_tmpA7F;(
_tmpA7F=0,Cyc_Tcutil_terr(e->loc,((_tmpA80="can't validate cast due to potential size differences",
_tag_dyneither(_tmpA80,sizeof(char),54))),_tag_dyneither(_tmpA7F,sizeof(void*),0)));}
if(_tmp4AA  && !_tmp4AB){if(Cyc_Toc_is_toplevel(nv)){const char*_tmpA83;void*
_tmpA82;(_tmpA82=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((
_tmpA83="can't do NULL-check conversion at top-level",_tag_dyneither(_tmpA83,
sizeof(char),44))),_tag_dyneither(_tmpA82,sizeof(void*),0)));}if(_tmp3F8 != (void*)
2){const char*_tmpA87;void*_tmpA86[1];struct Cyc_String_pa_struct _tmpA85;(_tmpA85.tag=
0,((_tmpA85.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(
e)),((_tmpA86[0]=& _tmpA85,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap))Cyc_Tcutil_impos)(((_tmpA87="null-check conversion mis-classified: %s",
_tag_dyneither(_tmpA87,sizeof(char),41))),_tag_dyneither(_tmpA86,sizeof(void*),1)))))));}{
int do_null_check=Cyc_Toc_need_null_check(_tmp3F6);if(do_null_check){if(!_tmp3F7){
const char*_tmpA8A;void*_tmpA89;(_tmpA89=0,Cyc_Tcutil_warn(e->loc,((_tmpA8A="inserted null check due to implicit cast from * to @ type",
_tag_dyneither(_tmpA8A,sizeof(char),58))),_tag_dyneither(_tmpA89,sizeof(void*),0)));}{
struct Cyc_List_List*_tmpA8B;e->r=Cyc_Toc_cast_it_r(*_tmp3F5,Cyc_Absyn_fncall_exp(
Cyc_Toc__check_null_e,((_tmpA8B=_cycalloc(sizeof(*_tmpA8B)),((_tmpA8B->hd=
_tmp3F6,((_tmpA8B->tl=0,_tmpA8B)))))),0));}}}}goto _LL2D4;_LL2D7: _tmp4B6=_tmp4B1.f1;
if(_tmp4B6 <= (void*)1)goto _LL2D9;if(*((int*)_tmp4B6)!= 0)goto _LL2D9;_tmp4B7=((
struct Cyc_Absyn_Upper_b_struct*)_tmp4B6)->f1;_tmp4B8=_tmp4B1.f2;if((int)_tmp4B8
!= 0)goto _LL2D9;_LL2D8: if(!Cyc_Evexp_c_can_eval(_tmp4B7)){const char*_tmpA8E;void*
_tmpA8D;(_tmpA8D=0,Cyc_Tcutil_terr(e->loc,((_tmpA8E="cannot perform coercion since numelts cannot be determined statically.",
_tag_dyneither(_tmpA8E,sizeof(char),71))),_tag_dyneither(_tmpA8D,sizeof(void*),0)));}
if(_tmp3F8 == (void*)2){const char*_tmpA92;void*_tmpA91[1];struct Cyc_String_pa_struct
_tmpA90;(_tmpA90.tag=0,((_tmpA90.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(e)),((_tmpA91[0]=& _tmpA90,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpA92="conversion mis-classified as null-check: %s",
_tag_dyneither(_tmpA92,sizeof(char),44))),_tag_dyneither(_tmpA91,sizeof(void*),1)))))));}
if(Cyc_Toc_is_toplevel(nv)){if((_tmp4AE  && !(_tmp4A6.elt_tq).real_const) && !
_tmp4AF)_tmp4B7=Cyc_Absyn_prim2_exp((void*)2,_tmp4B7,Cyc_Absyn_uint_exp(1,0),0);
e->r=(Cyc_Toc_make_toplevel_dyn_arr(old_t2,_tmp4B7,_tmp3F6))->r;}else{struct Cyc_Absyn_Exp*
_tmp4CD=Cyc_Toc__tag_dyneither_e;if(_tmp4AE){struct _tuple1*_tmp4CE=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp4CF=Cyc_Absyn_var_exp(_tmp4CE,0);struct Cyc_Absyn_Exp*
arg3;{void*_tmp4D0=_tmp3F6->r;union Cyc_Absyn_Cnst _tmp4D1;struct _dyneither_ptr
_tmp4D2;_LL2DE: if(*((int*)_tmp4D0)!= 0)goto _LL2E0;_tmp4D1=((struct Cyc_Absyn_Const_e_struct*)
_tmp4D0)->f1;if((_tmp4D1.String_c).tag != 7)goto _LL2E0;_tmp4D2=(struct
_dyneither_ptr)(_tmp4D1.String_c).val;_LL2DF: arg3=_tmp4B7;goto _LL2DD;_LL2E0:;
_LL2E1:{struct Cyc_Absyn_Exp*_tmpA93[2];arg3=Cyc_Absyn_fncall_exp(Cyc_Toc_getFunctionRemovePointer(&
Cyc_Toc__get_zero_arr_size_functionSet,_tmp3F6),((_tmpA93[1]=_tmp4B7,((_tmpA93[0]=
_tmp4CF,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpA93,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}goto _LL2DD;
_LL2DD:;}if(!_tmp4AF  && !(_tmp4A6.elt_tq).real_const)arg3=Cyc_Absyn_prim2_exp((
void*)2,arg3,Cyc_Absyn_uint_exp(1,0),0);{struct Cyc_Absyn_Exp*_tmp4D4=Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c(_tmp4A6.elt_typ),0);struct Cyc_Absyn_Exp*_tmpA94[3];struct Cyc_Absyn_Exp*
_tmp4D5=Cyc_Absyn_fncall_exp(_tmp4CD,((_tmpA94[2]=arg3,((_tmpA94[1]=_tmp4D4,((
_tmpA94[0]=_tmp4CF,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpA94,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);struct Cyc_Absyn_Stmt*
_tmp4D6=Cyc_Absyn_exp_stmt(_tmp4D5,0);_tmp4D6=Cyc_Absyn_declare_stmt(_tmp4CE,Cyc_Toc_typ_to_c(
old_t2),(struct Cyc_Absyn_Exp*)_tmp3F6,_tmp4D6,0);e->r=Cyc_Toc_stmt_exp_r(_tmp4D6);}}
else{struct Cyc_Absyn_Exp*_tmpA95[3];e->r=Cyc_Toc_fncall_exp_r(_tmp4CD,((_tmpA95[
2]=_tmp4B7,((_tmpA95[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp4A6.elt_typ),
0),((_tmpA95[0]=_tmp3F6,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpA95,sizeof(struct Cyc_Absyn_Exp*),3)))))))));}}goto _LL2D4;
_LL2D9: _tmp4B9=_tmp4B1.f1;if((int)_tmp4B9 != 0)goto _LL2DB;_tmp4BA=_tmp4B1.f2;if(
_tmp4BA <= (void*)1)goto _LL2DB;if(*((int*)_tmp4BA)!= 0)goto _LL2DB;_tmp4BB=((
struct Cyc_Absyn_Upper_b_struct*)_tmp4BA)->f1;_LL2DA: if(!Cyc_Evexp_c_can_eval(
_tmp4BB)){const char*_tmpA98;void*_tmpA97;(_tmpA97=0,Cyc_Tcutil_terr(e->loc,((
_tmpA98="cannot perform coercion since numelts cannot be determined statically.",
_tag_dyneither(_tmpA98,sizeof(char),71))),_tag_dyneither(_tmpA97,sizeof(void*),0)));}
if(Cyc_Toc_is_toplevel(nv)){const char*_tmpA9B;void*_tmpA9A;(_tmpA9A=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpA9B="can't coerce t? to t* or t@ at the top-level",
_tag_dyneither(_tmpA9B,sizeof(char),45))),_tag_dyneither(_tmpA9A,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*_tmp4DD=_tmp4BB;if(_tmp4AE  && !_tmp4AF)_tmp4DD=Cyc_Absyn_add_exp(
_tmp4BB,Cyc_Absyn_uint_exp(1,0),0);{struct Cyc_Absyn_Exp*_tmp4DE=Cyc_Toc__untag_dyneither_ptr_e;
struct Cyc_Absyn_Exp*_tmpA9C[3];struct Cyc_Absyn_Exp*_tmp4DF=Cyc_Absyn_fncall_exp(
_tmp4DE,((_tmpA9C[2]=_tmp4DD,((_tmpA9C[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
_tmp4A4.elt_typ),0),((_tmpA9C[0]=_tmp3F6,((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpA9C,sizeof(struct Cyc_Absyn_Exp*),
3)))))))),0);if(_tmp4AB){struct Cyc_List_List*_tmpA9D;_tmp4DF->r=Cyc_Toc_fncall_exp_r(
Cyc_Toc__check_null_e,((_tmpA9D=_cycalloc(sizeof(*_tmpA9D)),((_tmpA9D->hd=Cyc_Absyn_copy_exp(
_tmp4DF),((_tmpA9D->tl=0,_tmpA9D)))))));}e->r=Cyc_Toc_cast_it_r(*_tmp3F5,_tmp4DF);
goto _LL2D4;}}_LL2DB: _tmp4BC=_tmp4B1.f1;if((int)_tmp4BC != 0)goto _LL2D4;_tmp4BD=
_tmp4B1.f2;if((int)_tmp4BD != 0)goto _LL2D4;_LL2DC: DynCast: if((_tmp4AE  && !_tmp4AF)
 && !(_tmp4A6.elt_tq).real_const){if(Cyc_Toc_is_toplevel(nv)){const char*_tmpAA0;
void*_tmpA9F;(_tmpA9F=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Toc_unimp)(((_tmpAA0="can't coerce a ZEROTERM to a non-const NOZEROTERM pointer at toplevel",
_tag_dyneither(_tmpAA0,sizeof(char),70))),_tag_dyneither(_tmpA9F,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*_tmp4E4=Cyc_Toc__dyneither_ptr_decrease_size_e;struct Cyc_Absyn_Exp*
_tmpAA1[3];e->r=Cyc_Toc_fncall_exp_r(_tmp4E4,((_tmpAA1[2]=Cyc_Absyn_uint_exp(1,0),((
_tmpAA1[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp4A4.elt_typ),0),((
_tmpAA1[0]=_tmp3F6,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpAA1,sizeof(struct Cyc_Absyn_Exp*),3)))))))));}}goto _LL2D4;
_LL2D4:;}goto _LL2CD;}_LL2D0: _tmp4A7=_tmp4A2.f1;if(_tmp4A7 <= (void*)4)goto _LL2D2;
if(*((int*)_tmp4A7)!= 4)goto _LL2D2;_tmp4A8=((struct Cyc_Absyn_PointerType_struct*)
_tmp4A7)->f1;_tmp4A9=_tmp4A2.f2;if(_tmp4A9 <= (void*)4)goto _LL2D2;if(*((int*)
_tmp4A9)!= 5)goto _LL2D2;_LL2D1:{void*_tmp4E6=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,(
_tmp4A8.ptr_atts).bounds);_LL2E3: if((int)_tmp4E6 != 0)goto _LL2E5;_LL2E4: _tmp3F6->r=
Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp(_tmp3F6->r,_tmp3F6->loc),Cyc_Toc_curr_sp);
goto _LL2E2;_LL2E5:;_LL2E6: goto _LL2E2;_LL2E2:;}goto _LL2CD;_LL2D2:;_LL2D3: goto
_LL2CD;_LL2CD:;}goto _LL20C;}_LL231: if(*((int*)_tmp3CE)!= 16)goto _LL233;_tmp3F9=((
struct Cyc_Absyn_Address_e_struct*)_tmp3CE)->f1;_LL232:{void*_tmp4E7=_tmp3F9->r;
struct _tuple1*_tmp4E8;struct Cyc_List_List*_tmp4E9;struct Cyc_List_List*_tmp4EA;
struct Cyc_List_List*_tmp4EB;_LL2E8: if(*((int*)_tmp4E7)!= 30)goto _LL2EA;_tmp4E8=((
struct Cyc_Absyn_Aggregate_e_struct*)_tmp4E7)->f1;_tmp4E9=((struct Cyc_Absyn_Aggregate_e_struct*)
_tmp4E7)->f2;_tmp4EA=((struct Cyc_Absyn_Aggregate_e_struct*)_tmp4E7)->f3;_LL2E9:
if(Cyc_Toc_is_toplevel(nv)){const char*_tmpAA5;void*_tmpAA4[1];struct Cyc_String_pa_struct
_tmpAA3;(_tmpAA3.tag=0,((_tmpAA3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Position_string_of_segment(_tmp3F9->loc)),((_tmpAA4[0]=& _tmpAA3,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpAA5="%s: & on non-identifiers at the top-level",
_tag_dyneither(_tmpAA5,sizeof(char),42))),_tag_dyneither(_tmpAA4,sizeof(void*),1)))))));}
e->r=(Cyc_Toc_init_struct(nv,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp3F9->topt))->v,
_tmp4E9 != 0,1,0,_tmp4EA,_tmp4E8))->r;goto _LL2E7;_LL2EA: if(*((int*)_tmp4E7)!= 26)
goto _LL2EC;_tmp4EB=((struct Cyc_Absyn_Tuple_e_struct*)_tmp4E7)->f1;_LL2EB: if(Cyc_Toc_is_toplevel(
nv)){const char*_tmpAA9;void*_tmpAA8[1];struct Cyc_String_pa_struct _tmpAA7;(
_tmpAA7.tag=0,((_tmpAA7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Position_string_of_segment(
_tmp3F9->loc)),((_tmpAA8[0]=& _tmpAA7,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpAA9="%s: & on non-identifiers at the top-level",
_tag_dyneither(_tmpAA9,sizeof(char),42))),_tag_dyneither(_tmpAA8,sizeof(void*),1)))))));}
e->r=(Cyc_Toc_init_tuple(nv,1,0,_tmp4EB))->r;goto _LL2E7;_LL2EC:;_LL2ED: Cyc_Toc_exp_to_c(
nv,_tmp3F9);if(!Cyc_Absyn_is_lvalue(_tmp3F9)){((void(*)(struct Cyc_Absyn_Exp*e1,
struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,int),int
f_env))Cyc_Toc_lvalue_assign)(_tmp3F9,0,Cyc_Toc_address_lvalue,1);e->r=Cyc_Toc_cast_it_r(
Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),_tmp3F9);}
goto _LL2E7;_LL2E7:;}goto _LL20C;_LL233: if(*((int*)_tmp3CE)!= 17)goto _LL235;
_tmp3FA=((struct Cyc_Absyn_New_e_struct*)_tmp3CE)->f1;_tmp3FB=((struct Cyc_Absyn_New_e_struct*)
_tmp3CE)->f2;_LL234: if(Cyc_Toc_is_toplevel(nv)){const char*_tmpAAD;void*_tmpAAC[1];
struct Cyc_String_pa_struct _tmpAAB;(_tmpAAB.tag=0,((_tmpAAB.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Position_string_of_segment(_tmp3FB->loc)),((
_tmpAAC[0]=& _tmpAAB,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((
_tmpAAD="%s: new at top-level",_tag_dyneither(_tmpAAD,sizeof(char),21))),
_tag_dyneither(_tmpAAC,sizeof(void*),1)))))));}{void*_tmp4F5=_tmp3FB->r;struct
Cyc_List_List*_tmp4F6;struct Cyc_Absyn_Vardecl*_tmp4F7;struct Cyc_Absyn_Exp*
_tmp4F8;struct Cyc_Absyn_Exp*_tmp4F9;int _tmp4FA;struct _tuple1*_tmp4FB;struct Cyc_List_List*
_tmp4FC;struct Cyc_List_List*_tmp4FD;struct Cyc_Absyn_Aggrdecl*_tmp4FE;struct Cyc_List_List*
_tmp4FF;_LL2EF: if(*((int*)_tmp4F5)!= 28)goto _LL2F1;_tmp4F6=((struct Cyc_Absyn_Array_e_struct*)
_tmp4F5)->f1;_LL2F0: {struct _tuple1*_tmp500=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*
_tmp501=Cyc_Absyn_var_exp(_tmp500,0);struct Cyc_Absyn_Stmt*_tmp502=Cyc_Toc_init_array(
nv,_tmp501,_tmp4F6,Cyc_Absyn_exp_stmt(_tmp501,0));void*old_elt_typ;{void*_tmp503=
Cyc_Tcutil_compress(old_typ);struct Cyc_Absyn_PtrInfo _tmp504;void*_tmp505;struct
Cyc_Absyn_Tqual _tmp506;struct Cyc_Absyn_PtrAtts _tmp507;union Cyc_Absyn_Constraint*
_tmp508;_LL2FA: if(_tmp503 <= (void*)4)goto _LL2FC;if(*((int*)_tmp503)!= 4)goto
_LL2FC;_tmp504=((struct Cyc_Absyn_PointerType_struct*)_tmp503)->f1;_tmp505=
_tmp504.elt_typ;_tmp506=_tmp504.elt_tq;_tmp507=_tmp504.ptr_atts;_tmp508=_tmp507.zero_term;
_LL2FB: old_elt_typ=_tmp505;goto _LL2F9;_LL2FC:;_LL2FD: {const char*_tmpAB0;void*
_tmpAAF;old_elt_typ=((_tmpAAF=0,Cyc_Toc_toc_impos(((_tmpAB0="exp_to_c:new array expression doesn't have ptr type",
_tag_dyneither(_tmpAB0,sizeof(char),52))),_tag_dyneither(_tmpAAF,sizeof(void*),0))));}
_LL2F9:;}{void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);void*_tmp50B=Cyc_Absyn_cstar_typ(
elt_typ,Cyc_Toc_mt_tq);struct Cyc_Absyn_Exp*_tmp50C=Cyc_Absyn_times_exp(Cyc_Absyn_sizeoftyp_exp(
elt_typ,0),Cyc_Absyn_signed_int_exp(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp4F6),0),0);struct Cyc_Absyn_Exp*e1;if(_tmp3FA == 0  || Cyc_Absyn_no_regions)e1=
Cyc_Toc_malloc_exp(old_elt_typ,_tmp50C);else{struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)
_tmp3FA;Cyc_Toc_exp_to_c(nv,r);e1=Cyc_Toc_rmalloc_exp(r,_tmp50C);}e->r=Cyc_Toc_stmt_exp_r(
Cyc_Absyn_declare_stmt(_tmp500,_tmp50B,(struct Cyc_Absyn_Exp*)e1,_tmp502,0));goto
_LL2EE;}}_LL2F1: if(*((int*)_tmp4F5)!= 29)goto _LL2F3;_tmp4F7=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp4F5)->f1;_tmp4F8=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp4F5)->f2;
_tmp4F9=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp4F5)->f3;_tmp4FA=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmp4F5)->f4;_LL2F2: {int is_dyneither_ptr=0;{
void*_tmp50D=Cyc_Tcutil_compress(old_typ);struct Cyc_Absyn_PtrInfo _tmp50E;void*
_tmp50F;struct Cyc_Absyn_Tqual _tmp510;struct Cyc_Absyn_PtrAtts _tmp511;union Cyc_Absyn_Constraint*
_tmp512;union Cyc_Absyn_Constraint*_tmp513;_LL2FF: if(_tmp50D <= (void*)4)goto
_LL301;if(*((int*)_tmp50D)!= 4)goto _LL301;_tmp50E=((struct Cyc_Absyn_PointerType_struct*)
_tmp50D)->f1;_tmp50F=_tmp50E.elt_typ;_tmp510=_tmp50E.elt_tq;_tmp511=_tmp50E.ptr_atts;
_tmp512=_tmp511.bounds;_tmp513=_tmp511.zero_term;_LL300: is_dyneither_ptr=Cyc_Absyn_conref_def(
Cyc_Absyn_bounds_one,_tmp512)== (void*)0;goto _LL2FE;_LL301:;_LL302: {const char*
_tmpAB3;void*_tmpAB2;(_tmpAB2=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAB3="exp_to_c: comprehension not an array type",
_tag_dyneither(_tmpAB3,sizeof(char),42))),_tag_dyneither(_tmpAB2,sizeof(void*),0)));}
_LL2FE:;}{struct _tuple1*max=Cyc_Toc_temp_var();struct _tuple1*a=Cyc_Toc_temp_var();
void*old_elt_typ=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp4F9->topt))->v;
void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);void*ptr_typ=Cyc_Absyn_cstar_typ(
elt_typ,Cyc_Toc_mt_tq);Cyc_Toc_exp_to_c(nv,_tmp4F8);{struct Cyc_Absyn_Exp*_tmp516=
Cyc_Absyn_var_exp(max,0);if(_tmp4FA)_tmp516=Cyc_Absyn_add_exp(_tmp516,Cyc_Absyn_uint_exp(
1,0),0);{struct Cyc_Absyn_Stmt*s=Cyc_Toc_init_comprehension(nv,Cyc_Absyn_var_exp(
a,0),_tmp4F7,Cyc_Absyn_var_exp(max,0),_tmp4F9,_tmp4FA,Cyc_Toc_skip_stmt_dl(),1);
struct Cyc_Toc_Env _tmp518;struct _RegionHandle*_tmp519;struct Cyc_Toc_Env*_tmp517=
nv;_tmp518=*_tmp517;_tmp519=_tmp518.rgn;{struct _tuple19*_tmpAB6;struct Cyc_List_List*
_tmpAB5;struct Cyc_List_List*decls=(_tmpAB5=_region_malloc(_tmp519,sizeof(*
_tmpAB5)),((_tmpAB5->hd=((_tmpAB6=_region_malloc(_tmp519,sizeof(*_tmpAB6)),((
_tmpAB6->f1=max,((_tmpAB6->f2=Cyc_Absyn_uint_typ,((_tmpAB6->f3=(struct Cyc_Absyn_Exp*)
_tmp4F8,_tmpAB6)))))))),((_tmpAB5->tl=0,_tmpAB5)))));struct Cyc_Absyn_Exp*ai;if(
_tmp3FA == 0  || Cyc_Absyn_no_regions){struct Cyc_Absyn_Exp*_tmpAB7[2];ai=Cyc_Toc_malloc_exp(
old_elt_typ,Cyc_Absyn_fncall_exp(Cyc_Toc__check_times_e,((_tmpAB7[1]=_tmp516,((
_tmpAB7[0]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0),((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpAB7,sizeof(struct Cyc_Absyn_Exp*),
2)))))),0));}else{struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)_tmp3FA;Cyc_Toc_exp_to_c(
nv,r);{struct Cyc_Absyn_Exp*_tmpAB8[2];ai=Cyc_Toc_rmalloc_exp(r,Cyc_Absyn_fncall_exp(
Cyc_Toc__check_times_e,((_tmpAB8[1]=_tmp516,((_tmpAB8[0]=Cyc_Absyn_sizeoftyp_exp(
elt_typ,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpAB8,sizeof(struct Cyc_Absyn_Exp*),2)))))),0));}}{struct Cyc_Absyn_Exp*
ainit=Cyc_Toc_cast_it(ptr_typ,ai);{struct _tuple19*_tmpABB;struct Cyc_List_List*
_tmpABA;decls=((_tmpABA=_region_malloc(_tmp519,sizeof(*_tmpABA)),((_tmpABA->hd=((
_tmpABB=_region_malloc(_tmp519,sizeof(*_tmpABB)),((_tmpABB->f1=a,((_tmpABB->f2=
ptr_typ,((_tmpABB->f3=(struct Cyc_Absyn_Exp*)ainit,_tmpABB)))))))),((_tmpABA->tl=
decls,_tmpABA))))));}if(is_dyneither_ptr){struct _tuple1*_tmp51E=Cyc_Toc_temp_var();
void*_tmp51F=Cyc_Toc_typ_to_c(old_typ);struct Cyc_Absyn_Exp*_tmp520=Cyc_Toc__tag_dyneither_e;
struct Cyc_Absyn_Exp*_tmpABC[3];struct Cyc_Absyn_Exp*_tmp521=Cyc_Absyn_fncall_exp(
_tmp520,((_tmpABC[2]=_tmp516,((_tmpABC[1]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0),((
_tmpABC[0]=Cyc_Absyn_var_exp(a,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))
Cyc_List_list)(_tag_dyneither(_tmpABC,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);{
struct _tuple19*_tmpABF;struct Cyc_List_List*_tmpABE;decls=((_tmpABE=
_region_malloc(_tmp519,sizeof(*_tmpABE)),((_tmpABE->hd=((_tmpABF=_region_malloc(
_tmp519,sizeof(*_tmpABF)),((_tmpABF->f1=_tmp51E,((_tmpABF->f2=_tmp51F,((_tmpABF->f3=(
struct Cyc_Absyn_Exp*)_tmp521,_tmpABF)))))))),((_tmpABE->tl=decls,_tmpABE))))));}
s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(Cyc_Absyn_var_exp(_tmp51E,0),0),0);}
else{s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(Cyc_Absyn_var_exp(a,0),0),0);}{
struct Cyc_List_List*_tmp525=decls;for(0;_tmp525 != 0;_tmp525=_tmp525->tl){struct
_tuple1*_tmp527;void*_tmp528;struct Cyc_Absyn_Exp*_tmp529;struct _tuple19 _tmp526=*((
struct _tuple19*)_tmp525->hd);_tmp527=_tmp526.f1;_tmp528=_tmp526.f2;_tmp529=
_tmp526.f3;s=Cyc_Absyn_declare_stmt(_tmp527,_tmp528,_tmp529,s,0);}}e->r=Cyc_Toc_stmt_exp_r(
s);}}goto _LL2EE;}}}}_LL2F3: if(*((int*)_tmp4F5)!= 30)goto _LL2F5;_tmp4FB=((struct
Cyc_Absyn_Aggregate_e_struct*)_tmp4F5)->f1;_tmp4FC=((struct Cyc_Absyn_Aggregate_e_struct*)
_tmp4F5)->f2;_tmp4FD=((struct Cyc_Absyn_Aggregate_e_struct*)_tmp4F5)->f3;_tmp4FE=((
struct Cyc_Absyn_Aggregate_e_struct*)_tmp4F5)->f4;_LL2F4: e->r=(Cyc_Toc_init_struct(
nv,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp3FB->topt))->v,_tmp4FC != 0,1,
_tmp3FA,_tmp4FD,_tmp4FB))->r;goto _LL2EE;_LL2F5: if(*((int*)_tmp4F5)!= 26)goto
_LL2F7;_tmp4FF=((struct Cyc_Absyn_Tuple_e_struct*)_tmp4F5)->f1;_LL2F6: e->r=(Cyc_Toc_init_tuple(
nv,1,_tmp3FA,_tmp4FF))->r;goto _LL2EE;_LL2F7:;_LL2F8: {void*old_elt_typ=(void*)((
struct Cyc_Core_Opt*)_check_null(_tmp3FB->topt))->v;void*elt_typ=Cyc_Toc_typ_to_c(
old_elt_typ);struct _tuple1*_tmp52C=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*
_tmp52D=Cyc_Absyn_var_exp(_tmp52C,0);struct Cyc_Absyn_Exp*mexp=Cyc_Absyn_sizeofexp_exp(
Cyc_Absyn_deref_exp(_tmp52D,0),0);struct Cyc_Absyn_Exp*inner_mexp=mexp;if(_tmp3FA
== 0  || Cyc_Absyn_no_regions)mexp=Cyc_Toc_malloc_exp(old_elt_typ,mexp);else{
struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)_tmp3FA;Cyc_Toc_exp_to_c(nv,r);mexp=
Cyc_Toc_rmalloc_exp(r,mexp);}{int done=0;{void*_tmp52E=_tmp3FB->r;void*_tmp52F;
struct Cyc_Absyn_Exp*_tmp530;_LL304: if(*((int*)_tmp52E)!= 15)goto _LL306;_tmp52F=(
void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp52E)->f1;_tmp530=((struct Cyc_Absyn_Cast_e_struct*)
_tmp52E)->f2;_LL305:{struct _tuple0 _tmpAC0;struct _tuple0 _tmp532=(_tmpAC0.f1=Cyc_Tcutil_compress(
_tmp52F),((_tmpAC0.f2=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp530->topt))->v),_tmpAC0)));void*_tmp533;struct Cyc_Absyn_PtrInfo
_tmp534;void*_tmp535;struct Cyc_Absyn_PtrAtts _tmp536;union Cyc_Absyn_Constraint*
_tmp537;void*_tmp538;struct Cyc_Absyn_PtrInfo _tmp539;struct Cyc_Absyn_PtrAtts
_tmp53A;union Cyc_Absyn_Constraint*_tmp53B;_LL309: _tmp533=_tmp532.f1;if(_tmp533 <= (
void*)4)goto _LL30B;if(*((int*)_tmp533)!= 4)goto _LL30B;_tmp534=((struct Cyc_Absyn_PointerType_struct*)
_tmp533)->f1;_tmp535=_tmp534.elt_typ;_tmp536=_tmp534.ptr_atts;_tmp537=_tmp536.bounds;
_tmp538=_tmp532.f2;if(_tmp538 <= (void*)4)goto _LL30B;if(*((int*)_tmp538)!= 4)goto
_LL30B;_tmp539=((struct Cyc_Absyn_PointerType_struct*)_tmp538)->f1;_tmp53A=
_tmp539.ptr_atts;_tmp53B=_tmp53A.bounds;_LL30A:{struct _tuple0 _tmpAC1;struct
_tuple0 _tmp53D=(_tmpAC1.f1=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp537),((
_tmpAC1.f2=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp53B),_tmpAC1)));void*
_tmp53E;void*_tmp53F;struct Cyc_Absyn_Exp*_tmp540;_LL30E: _tmp53E=_tmp53D.f1;if((
int)_tmp53E != 0)goto _LL310;_tmp53F=_tmp53D.f2;if(_tmp53F <= (void*)1)goto _LL310;
if(*((int*)_tmp53F)!= 0)goto _LL310;_tmp540=((struct Cyc_Absyn_Upper_b_struct*)
_tmp53F)->f1;_LL30F: Cyc_Toc_exp_to_c(nv,_tmp530);inner_mexp->r=Cyc_Toc_sizeoftyp_exp_r(
elt_typ);done=1;{struct Cyc_Absyn_Exp*_tmp541=Cyc_Toc__init_dyneither_ptr_e;{
struct Cyc_Absyn_Exp*_tmpAC2[4];e->r=Cyc_Toc_fncall_exp_r(_tmp541,((_tmpAC2[3]=
_tmp540,((_tmpAC2[2]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp535),0),((
_tmpAC2[1]=_tmp530,((_tmpAC2[0]=mexp,((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpAC2,sizeof(struct Cyc_Absyn_Exp*),
4)))))))))));}goto _LL30D;}_LL310:;_LL311: goto _LL30D;_LL30D:;}goto _LL308;_LL30B:;
_LL30C: goto _LL308;_LL308:;}goto _LL303;_LL306:;_LL307: goto _LL303;_LL303:;}if(!
done){struct Cyc_Absyn_Stmt*_tmp543=Cyc_Absyn_exp_stmt(_tmp52D,0);struct Cyc_Absyn_Exp*
_tmp544=Cyc_Absyn_signed_int_exp(0,0);Cyc_Toc_exp_to_c(nv,_tmp3FB);_tmp543=Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(_tmp52D,_tmp544,0),_tmp3FB,0),
_tmp543,0);{void*_tmp545=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);e->r=Cyc_Toc_stmt_exp_r(
Cyc_Absyn_declare_stmt(_tmp52C,_tmp545,(struct Cyc_Absyn_Exp*)mexp,_tmp543,0));}}
goto _LL2EE;}}_LL2EE:;}goto _LL20C;_LL235: if(*((int*)_tmp3CE)!= 19)goto _LL237;
_tmp3FC=((struct Cyc_Absyn_Sizeofexp_e_struct*)_tmp3CE)->f1;_LL236: Cyc_Toc_exp_to_c(
nv,_tmp3FC);goto _LL20C;_LL237: if(*((int*)_tmp3CE)!= 18)goto _LL239;_tmp3FD=(void*)((
struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp3CE)->f1;_LL238:{struct Cyc_Absyn_Sizeoftyp_e_struct
_tmpAC5;struct Cyc_Absyn_Sizeoftyp_e_struct*_tmpAC4;e->r=(void*)((_tmpAC4=
_cycalloc(sizeof(*_tmpAC4)),((_tmpAC4[0]=((_tmpAC5.tag=18,((_tmpAC5.f1=(void*)
Cyc_Toc_typ_to_c_array(_tmp3FD),_tmpAC5)))),_tmpAC4))));}goto _LL20C;_LL239: if(*((
int*)_tmp3CE)!= 21)goto _LL23B;_LL23A: {const char*_tmpAC8;void*_tmpAC7;(_tmpAC7=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpAC8="__gen() in code generator",_tag_dyneither(_tmpAC8,sizeof(char),26))),
_tag_dyneither(_tmpAC7,sizeof(void*),0)));}_LL23B: if(*((int*)_tmp3CE)!= 20)goto
_LL23D;_tmp3FE=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp3CE)->f1;_tmp3FF=(
void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp3CE)->f2;if(*((int*)_tmp3FF)!= 0)
goto _LL23D;_tmp400=((struct Cyc_Absyn_StructField_struct*)_tmp3FF)->f1;_LL23C:{
struct Cyc_Absyn_Offsetof_e_struct _tmpAD2;struct Cyc_Absyn_StructField_struct
_tmpAD1;struct Cyc_Absyn_StructField_struct*_tmpAD0;struct Cyc_Absyn_Offsetof_e_struct*
_tmpACF;e->r=(void*)((_tmpACF=_cycalloc(sizeof(*_tmpACF)),((_tmpACF[0]=((_tmpAD2.tag=
20,((_tmpAD2.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp3FE),((_tmpAD2.f2=(void*)((
void*)((_tmpAD0=_cycalloc(sizeof(*_tmpAD0)),((_tmpAD0[0]=((_tmpAD1.tag=0,((
_tmpAD1.f1=_tmp400,_tmpAD1)))),_tmpAD0))))),_tmpAD2)))))),_tmpACF))));}goto
_LL20C;_LL23D: if(*((int*)_tmp3CE)!= 20)goto _LL23F;_tmp401=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)
_tmp3CE)->f1;_tmp402=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp3CE)->f2;
if(*((int*)_tmp402)!= 1)goto _LL23F;_tmp403=((struct Cyc_Absyn_TupleIndex_struct*)
_tmp402)->f1;_LL23E:{void*_tmp54E=Cyc_Tcutil_compress(_tmp401);struct Cyc_Absyn_AggrInfo
_tmp54F;union Cyc_Absyn_AggrInfoU _tmp550;struct Cyc_List_List*_tmp551;_LL313: if(
_tmp54E <= (void*)4)goto _LL31B;if(*((int*)_tmp54E)!= 10)goto _LL315;_tmp54F=((
struct Cyc_Absyn_AggrType_struct*)_tmp54E)->f1;_tmp550=_tmp54F.aggr_info;_LL314: {
struct Cyc_Absyn_Aggrdecl*_tmp552=Cyc_Absyn_get_known_aggrdecl(_tmp550);if(
_tmp552->impl == 0){const char*_tmpAD5;void*_tmpAD4;(_tmpAD4=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAD5="struct fields must be known",
_tag_dyneither(_tmpAD5,sizeof(char),28))),_tag_dyneither(_tmpAD4,sizeof(void*),0)));}
_tmp551=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp552->impl))->fields;goto
_LL316;}_LL315: if(*((int*)_tmp54E)!= 11)goto _LL317;_tmp551=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp54E)->f2;_LL316: {struct Cyc_Absyn_Aggrfield*_tmp555=((struct Cyc_Absyn_Aggrfield*(*)(
struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp551,(int)_tmp403);{struct Cyc_Absyn_Offsetof_e_struct
_tmpADF;struct Cyc_Absyn_StructField_struct _tmpADE;struct Cyc_Absyn_StructField_struct*
_tmpADD;struct Cyc_Absyn_Offsetof_e_struct*_tmpADC;e->r=(void*)((_tmpADC=
_cycalloc(sizeof(*_tmpADC)),((_tmpADC[0]=((_tmpADF.tag=20,((_tmpADF.f1=(void*)
Cyc_Toc_typ_to_c_array(_tmp401),((_tmpADF.f2=(void*)((void*)((_tmpADD=_cycalloc(
sizeof(*_tmpADD)),((_tmpADD[0]=((_tmpADE.tag=0,((_tmpADE.f1=_tmp555->name,
_tmpADE)))),_tmpADD))))),_tmpADF)))))),_tmpADC))));}goto _LL312;}_LL317: if(*((int*)
_tmp54E)!= 9)goto _LL319;_LL318:{struct Cyc_Absyn_Offsetof_e_struct _tmpAE9;struct
Cyc_Absyn_StructField_struct _tmpAE8;struct Cyc_Absyn_StructField_struct*_tmpAE7;
struct Cyc_Absyn_Offsetof_e_struct*_tmpAE6;e->r=(void*)((_tmpAE6=_cycalloc(
sizeof(*_tmpAE6)),((_tmpAE6[0]=((_tmpAE9.tag=20,((_tmpAE9.f1=(void*)Cyc_Toc_typ_to_c_array(
_tmp401),((_tmpAE9.f2=(void*)((void*)((_tmpAE7=_cycalloc(sizeof(*_tmpAE7)),((
_tmpAE7[0]=((_tmpAE8.tag=0,((_tmpAE8.f1=Cyc_Absyn_fieldname((int)(_tmp403 + 1)),
_tmpAE8)))),_tmpAE7))))),_tmpAE9)))))),_tmpAE6))));}goto _LL312;_LL319: if(*((int*)
_tmp54E)!= 3)goto _LL31B;_LL31A: if(_tmp403 == 0){struct Cyc_Absyn_Offsetof_e_struct
_tmpAF3;struct Cyc_Absyn_StructField_struct _tmpAF2;struct Cyc_Absyn_StructField_struct*
_tmpAF1;struct Cyc_Absyn_Offsetof_e_struct*_tmpAF0;e->r=(void*)((_tmpAF0=
_cycalloc(sizeof(*_tmpAF0)),((_tmpAF0[0]=((_tmpAF3.tag=20,((_tmpAF3.f1=(void*)
Cyc_Toc_typ_to_c_array(_tmp401),((_tmpAF3.f2=(void*)((void*)((_tmpAF1=_cycalloc(
sizeof(*_tmpAF1)),((_tmpAF1[0]=((_tmpAF2.tag=0,((_tmpAF2.f1=Cyc_Toc_tag_sp,
_tmpAF2)))),_tmpAF1))))),_tmpAF3)))))),_tmpAF0))));}else{struct Cyc_Absyn_Offsetof_e_struct
_tmpAFD;struct Cyc_Absyn_StructField_struct _tmpAFC;struct Cyc_Absyn_StructField_struct*
_tmpAFB;struct Cyc_Absyn_Offsetof_e_struct*_tmpAFA;e->r=(void*)((_tmpAFA=
_cycalloc(sizeof(*_tmpAFA)),((_tmpAFA[0]=((_tmpAFD.tag=20,((_tmpAFD.f1=(void*)
Cyc_Toc_typ_to_c_array(_tmp401),((_tmpAFD.f2=(void*)((void*)((_tmpAFB=_cycalloc(
sizeof(*_tmpAFB)),((_tmpAFB[0]=((_tmpAFC.tag=0,((_tmpAFC.f1=Cyc_Absyn_fieldname((
int)_tmp403),_tmpAFC)))),_tmpAFB))))),_tmpAFD)))))),_tmpAFA))));}goto _LL312;
_LL31B:;_LL31C: {const char*_tmpB00;void*_tmpAFF;(_tmpAFF=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB00="impossible type for offsetof tuple index",
_tag_dyneither(_tmpB00,sizeof(char),41))),_tag_dyneither(_tmpAFF,sizeof(void*),0)));}
_LL312:;}goto _LL20C;_LL23F: if(*((int*)_tmp3CE)!= 22)goto _LL241;_tmp404=((struct
Cyc_Absyn_Deref_e_struct*)_tmp3CE)->f1;_LL240: {void*_tmp568=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp404->topt))->v);{void*_tmp569=
_tmp568;struct Cyc_Absyn_PtrInfo _tmp56A;void*_tmp56B;struct Cyc_Absyn_Tqual _tmp56C;
struct Cyc_Absyn_PtrAtts _tmp56D;void*_tmp56E;union Cyc_Absyn_Constraint*_tmp56F;
union Cyc_Absyn_Constraint*_tmp570;union Cyc_Absyn_Constraint*_tmp571;_LL31E: if(
_tmp569 <= (void*)4)goto _LL320;if(*((int*)_tmp569)!= 4)goto _LL320;_tmp56A=((
struct Cyc_Absyn_PointerType_struct*)_tmp569)->f1;_tmp56B=_tmp56A.elt_typ;_tmp56C=
_tmp56A.elt_tq;_tmp56D=_tmp56A.ptr_atts;_tmp56E=_tmp56D.rgn;_tmp56F=_tmp56D.nullable;
_tmp570=_tmp56D.bounds;_tmp571=_tmp56D.zero_term;_LL31F:{void*_tmp572=Cyc_Absyn_conref_def(
Cyc_Absyn_bounds_one,_tmp570);struct Cyc_Absyn_Exp*_tmp573;_LL323: if(_tmp572 <= (
void*)1)goto _LL325;if(*((int*)_tmp572)!= 0)goto _LL325;_tmp573=((struct Cyc_Absyn_Upper_b_struct*)
_tmp572)->f1;_LL324: {int do_null_check=Cyc_Toc_need_null_check(_tmp404);Cyc_Toc_exp_to_c(
nv,_tmp404);if(do_null_check){if(Cyc_Toc_warn_all_null_deref){const char*_tmpB03;
void*_tmpB02;(_tmpB02=0,Cyc_Tcutil_warn(e->loc,((_tmpB03="inserted null check due to dereference",
_tag_dyneither(_tmpB03,sizeof(char),39))),_tag_dyneither(_tmpB02,sizeof(void*),0)));}{
struct Cyc_List_List*_tmpB04;_tmp404->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(
_tmp568),Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,((_tmpB04=_cycalloc(sizeof(*
_tmpB04)),((_tmpB04->hd=Cyc_Absyn_copy_exp(_tmp404),((_tmpB04->tl=0,_tmpB04)))))),
0));}}if(!((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmp571)){unsigned int _tmp578;int _tmp579;struct _tuple11 _tmp577=Cyc_Evexp_eval_const_uint_exp(
_tmp573);_tmp578=_tmp577.f1;_tmp579=_tmp577.f2;if(!_tmp579  || _tmp578 <= 0){const
char*_tmpB07;void*_tmpB06;(_tmpB06=0,Cyc_Tcutil_terr(e->loc,((_tmpB07="cannot determine dereference is in bounds",
_tag_dyneither(_tmpB07,sizeof(char),42))),_tag_dyneither(_tmpB06,sizeof(void*),0)));}}
goto _LL322;}_LL325: if((int)_tmp572 != 0)goto _LL322;_LL326: {struct Cyc_Absyn_Exp*
_tmp57C=Cyc_Absyn_uint_exp(0,0);{struct Cyc_Core_Opt*_tmpB08;_tmp57C->topt=((
_tmpB08=_cycalloc(sizeof(*_tmpB08)),((_tmpB08->v=(void*)Cyc_Absyn_uint_typ,
_tmpB08))));}e->r=Cyc_Toc_subscript_exp_r(_tmp404,_tmp57C);Cyc_Toc_exp_to_c(nv,e);
goto _LL322;}_LL322:;}goto _LL31D;_LL320:;_LL321: {const char*_tmpB0B;void*_tmpB0A;(
_tmpB0A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpB0B="exp_to_c: Deref: non-pointer",_tag_dyneither(_tmpB0B,sizeof(char),29))),
_tag_dyneither(_tmpB0A,sizeof(void*),0)));}_LL31D:;}goto _LL20C;}_LL241: if(*((int*)
_tmp3CE)!= 23)goto _LL243;_tmp405=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp3CE)->f1;
_tmp406=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp3CE)->f2;_tmp407=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp3CE)->f3;_tmp408=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp3CE)->f4;_LL242: {
int is_poly=Cyc_Toc_is_poly_project(e);void*e1_cyc_type=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp405->topt))->v;Cyc_Toc_exp_to_c(nv,_tmp405);if(_tmp407  && 
_tmp408)e->r=Cyc_Toc_check_tagged_union(_tmp405,Cyc_Toc_typ_to_c(e1_cyc_type),
e1_cyc_type,_tmp406,Cyc_Absyn_aggrmember_exp);if(is_poly)e->r=(Cyc_Toc_array_to_ptr_cast(
Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),Cyc_Absyn_new_exp(
e->r,0),0))->r;goto _LL20C;}_LL243: if(*((int*)_tmp3CE)!= 24)goto _LL245;_tmp409=((
struct Cyc_Absyn_AggrArrow_e_struct*)_tmp3CE)->f1;_tmp40A=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp3CE)->f2;_tmp40B=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp3CE)->f3;_tmp40C=((
struct Cyc_Absyn_AggrArrow_e_struct*)_tmp3CE)->f4;_LL244: {void*e1typ=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp409->topt))->v);int do_null_check=Cyc_Toc_need_null_check(
_tmp409);Cyc_Toc_exp_to_c(nv,_tmp409);{int is_poly=Cyc_Toc_is_poly_project(e);
void*_tmp581;struct Cyc_Absyn_Tqual _tmp582;struct Cyc_Absyn_PtrAtts _tmp583;void*
_tmp584;union Cyc_Absyn_Constraint*_tmp585;union Cyc_Absyn_Constraint*_tmp586;
union Cyc_Absyn_Constraint*_tmp587;struct Cyc_Absyn_PtrInfo _tmp580=Cyc_Toc_get_ptr_type(
e1typ);_tmp581=_tmp580.elt_typ;_tmp582=_tmp580.elt_tq;_tmp583=_tmp580.ptr_atts;
_tmp584=_tmp583.rgn;_tmp585=_tmp583.nullable;_tmp586=_tmp583.bounds;_tmp587=
_tmp583.zero_term;{void*_tmp588=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,
_tmp586);struct Cyc_Absyn_Exp*_tmp589;_LL328: if(_tmp588 <= (void*)1)goto _LL32A;if(*((
int*)_tmp588)!= 0)goto _LL32A;_tmp589=((struct Cyc_Absyn_Upper_b_struct*)_tmp588)->f1;
_LL329: {unsigned int _tmp58B;int _tmp58C;struct _tuple11 _tmp58A=Cyc_Evexp_eval_const_uint_exp(
_tmp589);_tmp58B=_tmp58A.f1;_tmp58C=_tmp58A.f2;if(_tmp58C){if(_tmp58B < 1){const
char*_tmpB0E;void*_tmpB0D;(_tmpB0D=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpB0E="exp_to_c:  AggrArrow_e on pointer of size 0",
_tag_dyneither(_tmpB0E,sizeof(char),44))),_tag_dyneither(_tmpB0D,sizeof(void*),0)));}
if(do_null_check){if(Cyc_Toc_warn_all_null_deref){const char*_tmpB11;void*_tmpB10;(
_tmpB10=0,Cyc_Tcutil_warn(e->loc,((_tmpB11="inserted null check due to dereference",
_tag_dyneither(_tmpB11,sizeof(char),39))),_tag_dyneither(_tmpB10,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*_tmpB12[1];_tmp409->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(
e1typ),Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,((_tmpB12[0]=Cyc_Absyn_new_exp(
_tmp409->r,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpB12,sizeof(struct Cyc_Absyn_Exp*),1)))),0));}}}else{if(!Cyc_Evexp_c_can_eval(
_tmp589)){const char*_tmpB15;void*_tmpB14;(_tmpB14=0,Cyc_Tcutil_terr(e->loc,((
_tmpB15="cannot determine pointer dereference in bounds",_tag_dyneither(_tmpB15,
sizeof(char),47))),_tag_dyneither(_tmpB14,sizeof(void*),0)));}{struct Cyc_Absyn_Exp*
_tmpB16[4];_tmp409->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(e1typ),Cyc_Absyn_fncall_exp(
Cyc_Toc__check_known_subscript_null_e,((_tmpB16[3]=Cyc_Absyn_uint_exp(0,0),((
_tmpB16[2]=Cyc_Absyn_sizeoftyp_exp(_tmp581,0),((_tmpB16[1]=_tmp589,((_tmpB16[0]=
Cyc_Absyn_new_exp(_tmp409->r,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))
Cyc_List_list)(_tag_dyneither(_tmpB16,sizeof(struct Cyc_Absyn_Exp*),4)))))))))),0));}}
goto _LL327;}_LL32A: if((int)_tmp588 != 0)goto _LL327;_LL32B: {void*ta1=Cyc_Toc_typ_to_c_array(
_tmp581);{struct Cyc_Absyn_Exp*_tmpB17[3];_tmp409->r=Cyc_Toc_cast_it_r(Cyc_Absyn_cstar_typ(
ta1,_tmp582),Cyc_Absyn_fncall_exp(Cyc_Toc__check_dyneither_subscript_e,((_tmpB17[
2]=Cyc_Absyn_uint_exp(0,0),((_tmpB17[1]=Cyc_Absyn_sizeoftyp_exp(ta1,0),((_tmpB17[
0]=Cyc_Absyn_new_exp(_tmp409->r,0),((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB17,sizeof(struct Cyc_Absyn_Exp*),
3)))))))),0));}goto _LL327;}_LL327:;}if(_tmp40B  && _tmp40C)e->r=Cyc_Toc_check_tagged_union(
_tmp409,Cyc_Toc_typ_to_c(e1typ),_tmp581,_tmp40A,Cyc_Absyn_aggrarrow_exp);if(
is_poly  && _tmp40C)e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v),Cyc_Absyn_new_exp(e->r,0),0))->r;
goto _LL20C;}}_LL245: if(*((int*)_tmp3CE)!= 25)goto _LL247;_tmp40D=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp3CE)->f1;_tmp40E=((struct Cyc_Absyn_Subscript_e_struct*)_tmp3CE)->f2;_LL246: {
void*_tmp596=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp40D->topt))->v);{void*_tmp597=_tmp596;struct Cyc_List_List*_tmp598;struct Cyc_Absyn_PtrInfo
_tmp599;void*_tmp59A;struct Cyc_Absyn_Tqual _tmp59B;struct Cyc_Absyn_PtrAtts _tmp59C;
void*_tmp59D;union Cyc_Absyn_Constraint*_tmp59E;union Cyc_Absyn_Constraint*_tmp59F;
union Cyc_Absyn_Constraint*_tmp5A0;_LL32D: if(_tmp597 <= (void*)4)goto _LL331;if(*((
int*)_tmp597)!= 9)goto _LL32F;_tmp598=((struct Cyc_Absyn_TupleType_struct*)_tmp597)->f1;
_LL32E: Cyc_Toc_exp_to_c(nv,_tmp40D);Cyc_Toc_exp_to_c(nv,_tmp40E);{unsigned int
_tmp5A2;int _tmp5A3;struct _tuple11 _tmp5A1=Cyc_Evexp_eval_const_uint_exp(_tmp40E);
_tmp5A2=_tmp5A1.f1;_tmp5A3=_tmp5A1.f2;if(!_tmp5A3){const char*_tmpB1A;void*
_tmpB19;(_tmpB19=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmpB1A="unknown tuple subscript in translation to C",_tag_dyneither(_tmpB1A,
sizeof(char),44))),_tag_dyneither(_tmpB19,sizeof(void*),0)));}e->r=Cyc_Toc_aggrmember_exp_r(
_tmp40D,Cyc_Absyn_fieldname((int)(_tmp5A2 + 1)));goto _LL32C;}_LL32F: if(*((int*)
_tmp597)!= 4)goto _LL331;_tmp599=((struct Cyc_Absyn_PointerType_struct*)_tmp597)->f1;
_tmp59A=_tmp599.elt_typ;_tmp59B=_tmp599.elt_tq;_tmp59C=_tmp599.ptr_atts;_tmp59D=
_tmp59C.rgn;_tmp59E=_tmp59C.nullable;_tmp59F=_tmp59C.bounds;_tmp5A0=_tmp59C.zero_term;
_LL330: {struct Cyc_List_List*_tmp5A6=Cyc_Toc_get_relns(_tmp40D);int in_bnds=0;{
void*_tmp5A7=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp59F);_LL334:;_LL335:
in_bnds=Cyc_Toc_check_bounds(_tmp5A6,_tmp40D,_tmp40E);if(Cyc_Toc_warn_bounds_checks
 && !in_bnds){const char*_tmpB1E;void*_tmpB1D[1];struct Cyc_String_pa_struct
_tmpB1C;(_tmpB1C.tag=0,((_tmpB1C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(e)),((_tmpB1D[0]=& _tmpB1C,Cyc_Tcutil_warn(e->loc,((
_tmpB1E="bounds check necessary for %s",_tag_dyneither(_tmpB1E,sizeof(char),30))),
_tag_dyneither(_tmpB1D,sizeof(void*),1)))))));}_LL333:;}Cyc_Toc_exp_to_c(nv,
_tmp40D);Cyc_Toc_exp_to_c(nv,_tmp40E);++ Cyc_Toc_total_bounds_checks;{void*
_tmp5AB=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp59F);struct Cyc_Absyn_Exp*
_tmp5AC;_LL337: if(_tmp5AB <= (void*)1)goto _LL339;if(*((int*)_tmp5AB)!= 0)goto
_LL339;_tmp5AC=((struct Cyc_Absyn_Upper_b_struct*)_tmp5AB)->f1;_LL338: {int
possibly_null=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmp59E);void*ta1=Cyc_Toc_typ_to_c(_tmp59A);void*ta2=Cyc_Absyn_cstar_typ(ta1,
_tmp59B);if(in_bnds)++ Cyc_Toc_bounds_checks_eliminated;else{if(((int(*)(int y,
union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp5A0)){if(!Cyc_Evexp_c_can_eval(
_tmp5AC)){const char*_tmpB21;void*_tmpB20;(_tmpB20=0,Cyc_Tcutil_terr(e->loc,((
_tmpB21="cannot determine subscript is in bounds",_tag_dyneither(_tmpB21,sizeof(
char),40))),_tag_dyneither(_tmpB20,sizeof(void*),0)));}{struct Cyc_Absyn_Exp*
function_e=Cyc_Toc_getFunction(& Cyc_Toc__zero_arr_plus_functionSet,_tmp40D);
struct Cyc_Absyn_Exp*_tmpB22[3];e->r=Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(ta2,Cyc_Absyn_fncall_exp(
function_e,((_tmpB22[2]=_tmp40E,((_tmpB22[1]=_tmp5AC,((_tmpB22[0]=_tmp40D,((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmpB22,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0)));}}else{if(possibly_null){if(
!Cyc_Evexp_c_can_eval(_tmp5AC)){const char*_tmpB25;void*_tmpB24;(_tmpB24=0,Cyc_Tcutil_terr(
e->loc,((_tmpB25="cannot determine subscript is in bounds",_tag_dyneither(
_tmpB25,sizeof(char),40))),_tag_dyneither(_tmpB24,sizeof(void*),0)));}if(Cyc_Toc_warn_all_null_deref){
const char*_tmpB28;void*_tmpB27;(_tmpB27=0,Cyc_Tcutil_warn(e->loc,((_tmpB28="inserted null check due to dereference",
_tag_dyneither(_tmpB28,sizeof(char),39))),_tag_dyneither(_tmpB27,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*_tmpB29[4];e->r=Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(ta2,Cyc_Absyn_fncall_exp(
Cyc_Toc__check_known_subscript_null_e,((_tmpB29[3]=_tmp40E,((_tmpB29[2]=Cyc_Absyn_sizeoftyp_exp(
ta1,0),((_tmpB29[1]=_tmp5AC,((_tmpB29[0]=_tmp40D,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB29,sizeof(struct Cyc_Absyn_Exp*),
4)))))))))),0)));}}else{struct Cyc_Absyn_Exp*_tmpB2A[2];_tmp40E->r=Cyc_Toc_fncall_exp_r(
Cyc_Toc__check_known_subscript_notnull_e,((_tmpB2A[1]=Cyc_Absyn_copy_exp(_tmp40E),((
_tmpB2A[0]=_tmp5AC,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpB2A,sizeof(struct Cyc_Absyn_Exp*),2)))))));}}}goto _LL336;}
_LL339: if((int)_tmp5AB != 0)goto _LL336;_LL33A: {void*ta1=Cyc_Toc_typ_to_c_array(
_tmp59A);if(in_bnds){++ Cyc_Toc_bounds_checks_eliminated;e->r=Cyc_Toc_subscript_exp_r(
Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(ta1,_tmp59B),Cyc_Absyn_aggrmember_exp(
_tmp40D,Cyc_Toc_curr_sp,0)),_tmp40E);}else{struct Cyc_Absyn_Exp*_tmp5B6=Cyc_Toc__check_dyneither_subscript_e;
struct Cyc_Absyn_Exp*_tmpB2B[3];e->r=Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(
ta1,_tmp59B),Cyc_Absyn_fncall_exp(_tmp5B6,((_tmpB2B[2]=_tmp40E,((_tmpB2B[1]=Cyc_Absyn_sizeoftyp_exp(
ta1,0),((_tmpB2B[0]=_tmp40D,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpB2B,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0)));}goto _LL336;}
_LL336:;}goto _LL32C;}_LL331:;_LL332: {const char*_tmpB2E;void*_tmpB2D;(_tmpB2D=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpB2E="exp_to_c: Subscript on non-tuple/array/tuple ptr",_tag_dyneither(
_tmpB2E,sizeof(char),49))),_tag_dyneither(_tmpB2D,sizeof(void*),0)));}_LL32C:;}
goto _LL20C;}_LL247: if(*((int*)_tmp3CE)!= 26)goto _LL249;_tmp40F=((struct Cyc_Absyn_Tuple_e_struct*)
_tmp3CE)->f1;_LL248: if(!Cyc_Toc_is_toplevel(nv))e->r=(Cyc_Toc_init_tuple(nv,0,0,
_tmp40F))->r;else{struct Cyc_List_List*_tmp5BA=((struct Cyc_List_List*(*)(struct
_tuple10*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_tup_to_c,
_tmp40F);void*_tmp5BB=Cyc_Toc_add_tuple_type(_tmp5BA);struct Cyc_List_List*dles=0;{
int i=1;for(0;_tmp40F != 0;(_tmp40F=_tmp40F->tl,i ++)){Cyc_Toc_exp_to_c(nv,(struct
Cyc_Absyn_Exp*)_tmp40F->hd);{struct _tuple16*_tmpB31;struct Cyc_List_List*_tmpB30;
dles=((_tmpB30=_cycalloc(sizeof(*_tmpB30)),((_tmpB30->hd=((_tmpB31=_cycalloc(
sizeof(*_tmpB31)),((_tmpB31->f1=0,((_tmpB31->f2=(struct Cyc_Absyn_Exp*)_tmp40F->hd,
_tmpB31)))))),((_tmpB30->tl=dles,_tmpB30))))));}}}dles=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);e->r=Cyc_Toc_unresolvedmem_exp_r(
0,dles);}goto _LL20C;_LL249: if(*((int*)_tmp3CE)!= 28)goto _LL24B;_tmp410=((struct
Cyc_Absyn_Array_e_struct*)_tmp3CE)->f1;_LL24A: e->r=Cyc_Toc_unresolvedmem_exp_r(0,
_tmp410);{struct Cyc_List_List*_tmp5BE=_tmp410;for(0;_tmp5BE != 0;_tmp5BE=_tmp5BE->tl){
struct _tuple16 _tmp5C0;struct Cyc_Absyn_Exp*_tmp5C1;struct _tuple16*_tmp5BF=(struct
_tuple16*)_tmp5BE->hd;_tmp5C0=*_tmp5BF;_tmp5C1=_tmp5C0.f2;Cyc_Toc_exp_to_c(nv,
_tmp5C1);}}goto _LL20C;_LL24B: if(*((int*)_tmp3CE)!= 29)goto _LL24D;_tmp411=((
struct Cyc_Absyn_Comprehension_e_struct*)_tmp3CE)->f1;_tmp412=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp3CE)->f2;_tmp413=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp3CE)->f3;
_tmp414=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp3CE)->f4;_LL24C: {
unsigned int _tmp5C3;int _tmp5C4;struct _tuple11 _tmp5C2=Cyc_Evexp_eval_const_uint_exp(
_tmp412);_tmp5C3=_tmp5C2.f1;_tmp5C4=_tmp5C2.f2;{void*_tmp5C5=Cyc_Toc_typ_to_c((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp413->topt))->v);Cyc_Toc_exp_to_c(nv,
_tmp413);{struct Cyc_List_List*es=0;if(!Cyc_Toc_is_zero(_tmp413)){if(!_tmp5C4){
const char*_tmpB34;void*_tmpB33;(_tmpB33=0,Cyc_Tcutil_terr(_tmp412->loc,((_tmpB34="cannot determine value of constant",
_tag_dyneither(_tmpB34,sizeof(char),35))),_tag_dyneither(_tmpB33,sizeof(void*),0)));}{
unsigned int i=0;for(0;i < _tmp5C3;++ i){struct _tuple16*_tmpB37;struct Cyc_List_List*
_tmpB36;es=((_tmpB36=_cycalloc(sizeof(*_tmpB36)),((_tmpB36->hd=((_tmpB37=
_cycalloc(sizeof(*_tmpB37)),((_tmpB37->f1=0,((_tmpB37->f2=_tmp413,_tmpB37)))))),((
_tmpB36->tl=es,_tmpB36))))));}}if(_tmp414){struct Cyc_Absyn_Exp*_tmp5CA=Cyc_Toc_cast_it(
_tmp5C5,Cyc_Absyn_uint_exp(0,0));struct _tuple16*_tmpB3A;struct Cyc_List_List*
_tmpB39;es=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_imp_append)(es,((_tmpB39=_cycalloc(sizeof(*_tmpB39)),((_tmpB39->hd=((
_tmpB3A=_cycalloc(sizeof(*_tmpB3A)),((_tmpB3A->f1=0,((_tmpB3A->f2=_tmp5CA,
_tmpB3A)))))),((_tmpB39->tl=0,_tmpB39)))))));}}e->r=Cyc_Toc_unresolvedmem_exp_r(
0,es);goto _LL20C;}}}_LL24D: if(*((int*)_tmp3CE)!= 30)goto _LL24F;_tmp415=((struct
Cyc_Absyn_Aggregate_e_struct*)_tmp3CE)->f1;_tmp416=((struct Cyc_Absyn_Aggregate_e_struct*)
_tmp3CE)->f2;_tmp417=((struct Cyc_Absyn_Aggregate_e_struct*)_tmp3CE)->f3;_tmp418=((
struct Cyc_Absyn_Aggregate_e_struct*)_tmp3CE)->f4;_LL24E: if(!Cyc_Toc_is_toplevel(
nv))e->r=(Cyc_Toc_init_struct(nv,old_typ,_tmp416 != 0,0,0,_tmp417,_tmp415))->r;
else{if(_tmp418 == 0){const char*_tmpB3D;void*_tmpB3C;(_tmpB3C=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB3D="Aggregate_e: missing aggrdecl pointer",
_tag_dyneither(_tmpB3D,sizeof(char),38))),_tag_dyneither(_tmpB3C,sizeof(void*),0)));}{
struct Cyc_Absyn_Aggrdecl*sd2=(struct Cyc_Absyn_Aggrdecl*)_tmp418;struct Cyc_Toc_Env
_tmp5D0;struct _RegionHandle*_tmp5D1;struct Cyc_Toc_Env*_tmp5CF=nv;_tmp5D0=*
_tmp5CF;_tmp5D1=_tmp5D0.rgn;{struct Cyc_List_List*_tmp5D2=((struct Cyc_List_List*(*)(
struct _RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,
void*,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(
_tmp5D1,e->loc,_tmp417,sd2->kind,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(sd2->impl))->fields);
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp418->impl))->tagged){struct
_tuple20 _tmp5D4;struct Cyc_Absyn_Aggrfield*_tmp5D5;struct Cyc_Absyn_Exp*_tmp5D6;
struct _tuple20*_tmp5D3=(struct _tuple20*)((struct Cyc_List_List*)_check_null(
_tmp5D2))->hd;_tmp5D4=*_tmp5D3;_tmp5D5=_tmp5D4.f1;_tmp5D6=_tmp5D4.f2;{void*
_tmp5D7=_tmp5D5->type;Cyc_Toc_exp_to_c(nv,_tmp5D6);if(Cyc_Toc_is_void_star_or_tvar(
_tmp5D7))_tmp5D6->r=Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(
_tmp5D6->r,0));{int i=Cyc_Toc_get_member_offset((struct Cyc_Absyn_Aggrdecl*)
_tmp418,_tmp5D5->name);struct Cyc_Absyn_Exp*field_tag_exp=Cyc_Absyn_signed_int_exp(
i,0);struct _tuple16*_tmpB42;struct _tuple16*_tmpB41;struct _tuple16*_tmpB40[2];
struct Cyc_List_List*_tmp5D8=(_tmpB40[1]=((_tmpB42=_cycalloc(sizeof(*_tmpB42)),((
_tmpB42->f1=0,((_tmpB42->f2=_tmp5D6,_tmpB42)))))),((_tmpB40[0]=((_tmpB41=
_cycalloc(sizeof(*_tmpB41)),((_tmpB41->f1=0,((_tmpB41->f2=field_tag_exp,_tmpB41)))))),((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmpB40,sizeof(struct _tuple16*),2)))));struct Cyc_Absyn_Exp*umem=Cyc_Absyn_unresolvedmem_exp(
0,_tmp5D8,0);struct Cyc_Absyn_FieldName_struct*_tmpB48;struct Cyc_Absyn_FieldName_struct
_tmpB47;void*_tmpB46[1];struct Cyc_List_List*ds=(_tmpB46[0]=(void*)((_tmpB48=
_cycalloc(sizeof(*_tmpB48)),((_tmpB48[0]=((_tmpB47.tag=1,((_tmpB47.f1=_tmp5D5->name,
_tmpB47)))),_tmpB48)))),Cyc_List_list(_tag_dyneither(_tmpB46,sizeof(void*),1)));
struct _tuple16*_tmpB4B;struct _tuple16*_tmpB4A[1];struct Cyc_List_List*dles=(
_tmpB4A[0]=((_tmpB4B=_cycalloc(sizeof(*_tmpB4B)),((_tmpB4B->f1=ds,((_tmpB4B->f2=
umem,_tmpB4B)))))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpB4A,sizeof(struct _tuple16*),1)));e->r=Cyc_Toc_unresolvedmem_exp_r(
0,dles);}}}else{struct Cyc_List_List*_tmp5E1=0;struct Cyc_List_List*_tmp5E2=((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(sd2->impl))->fields;for(0;_tmp5E2 != 0;
_tmp5E2=_tmp5E2->tl){struct Cyc_List_List*_tmp5E3=_tmp5D2;for(0;_tmp5E3 != 0;
_tmp5E3=_tmp5E3->tl){if((*((struct _tuple20*)_tmp5E3->hd)).f1 == (struct Cyc_Absyn_Aggrfield*)
_tmp5E2->hd){struct _tuple20 _tmp5E5;struct Cyc_Absyn_Aggrfield*_tmp5E6;struct Cyc_Absyn_Exp*
_tmp5E7;struct _tuple20*_tmp5E4=(struct _tuple20*)_tmp5E3->hd;_tmp5E5=*_tmp5E4;
_tmp5E6=_tmp5E5.f1;_tmp5E7=_tmp5E5.f2;{void*_tmp5E8=_tmp5E6->type;Cyc_Toc_exp_to_c(
nv,_tmp5E7);if(Cyc_Toc_is_void_star_or_tvar(_tmp5E8))_tmp5E7->r=Cyc_Toc_cast_it_r(
Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(_tmp5E7->r,0));{struct _tuple16*
_tmpB4E;struct Cyc_List_List*_tmpB4D;_tmp5E1=((_tmpB4D=_cycalloc(sizeof(*_tmpB4D)),((
_tmpB4D->hd=((_tmpB4E=_cycalloc(sizeof(*_tmpB4E)),((_tmpB4E->f1=0,((_tmpB4E->f2=
_tmp5E7,_tmpB4E)))))),((_tmpB4D->tl=_tmp5E1,_tmpB4D))))));}break;}}}}e->r=Cyc_Toc_unresolvedmem_exp_r(
0,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp5E1));}}}}
goto _LL20C;_LL24F: if(*((int*)_tmp3CE)!= 31)goto _LL251;_tmp419=(void*)((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp3CE)->f1;_tmp41A=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp3CE)->f2;_LL250: {
struct Cyc_List_List*fs;{void*_tmp5EB=Cyc_Tcutil_compress(_tmp419);struct Cyc_List_List*
_tmp5EC;_LL33C: if(_tmp5EB <= (void*)4)goto _LL33E;if(*((int*)_tmp5EB)!= 11)goto
_LL33E;_tmp5EC=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp5EB)->f2;_LL33D: fs=
_tmp5EC;goto _LL33B;_LL33E:;_LL33F: {const char*_tmpB52;void*_tmpB51[1];struct Cyc_String_pa_struct
_tmpB50;(_tmpB50.tag=0,((_tmpB50.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp419)),((_tmpB51[0]=& _tmpB50,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB52="anon struct has type %s",
_tag_dyneither(_tmpB52,sizeof(char),24))),_tag_dyneither(_tmpB51,sizeof(void*),1)))))));}
_LL33B:;}{struct Cyc_Toc_Env _tmp5F1;struct _RegionHandle*_tmp5F2;struct Cyc_Toc_Env*
_tmp5F0=nv;_tmp5F1=*_tmp5F0;_tmp5F2=_tmp5F1.rgn;{struct Cyc_List_List*_tmp5F3=((
struct Cyc_List_List*(*)(struct _RegionHandle*rgn,struct Cyc_Position_Segment*loc,
struct Cyc_List_List*des,void*,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(
_tmp5F2,e->loc,_tmp41A,(void*)0,fs);for(0;_tmp5F3 != 0;_tmp5F3=_tmp5F3->tl){
struct _tuple20 _tmp5F5;struct Cyc_Absyn_Aggrfield*_tmp5F6;struct Cyc_Absyn_Exp*
_tmp5F7;struct _tuple20*_tmp5F4=(struct _tuple20*)_tmp5F3->hd;_tmp5F5=*_tmp5F4;
_tmp5F6=_tmp5F5.f1;_tmp5F7=_tmp5F5.f2;{void*_tmp5F8=_tmp5F6->type;Cyc_Toc_exp_to_c(
nv,_tmp5F7);if(Cyc_Toc_is_void_star_or_tvar(_tmp5F8))_tmp5F7->r=Cyc_Toc_cast_it_r(
Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(_tmp5F7->r,0));}}e->r=Cyc_Toc_unresolvedmem_exp_r(
0,_tmp41A);}goto _LL20C;}}_LL251: if(*((int*)_tmp3CE)!= 32)goto _LL253;_tmp41B=((
struct Cyc_Absyn_Datatype_e_struct*)_tmp3CE)->f1;if(_tmp41B != 0)goto _LL253;
_tmp41C=((struct Cyc_Absyn_Datatype_e_struct*)_tmp3CE)->f2;_tmp41D=((struct Cyc_Absyn_Datatype_e_struct*)
_tmp3CE)->f3;_LL252: {struct _tuple1*qv=_tmp41D->name;struct Cyc_Absyn_Exp*tag_exp=
_tmp41C->is_extensible?Cyc_Absyn_var_exp(qv,0): Cyc_Toc_datatype_tag(_tmp41C,qv,0);
e->r=tag_exp->r;goto _LL20C;}_LL253: if(*((int*)_tmp3CE)!= 32)goto _LL255;_tmp41E=((
struct Cyc_Absyn_Datatype_e_struct*)_tmp3CE)->f1;_tmp41F=((struct Cyc_Absyn_Datatype_e_struct*)
_tmp3CE)->f2;_tmp420=((struct Cyc_Absyn_Datatype_e_struct*)_tmp3CE)->f3;_LL254: {
void*datatype_ctype;struct Cyc_Absyn_Exp*tag_exp;struct _tuple1*_tmp5F9=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp5FA=Cyc_Absyn_var_exp(_tmp5F9,0);struct Cyc_Absyn_Exp*
member_exp;{const char*_tmpB53;datatype_ctype=Cyc_Absyn_strctq(Cyc_Toc_collapse_qvar_tag(
_tmp420->name,((_tmpB53="_struct",_tag_dyneither(_tmpB53,sizeof(char),8)))));}
tag_exp=_tmp41F->is_extensible?Cyc_Absyn_var_exp(_tmp420->name,0): Cyc_Toc_datatype_tag(
_tmp41F,_tmp420->name,1);member_exp=_tmp5FA;{struct Cyc_List_List*_tmp5FC=_tmp420->typs;
if(Cyc_Toc_is_toplevel(nv)){struct Cyc_List_List*dles=0;for(0;_tmp41E != 0;(
_tmp41E=_tmp41E->tl,_tmp5FC=_tmp5FC->tl)){struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)
_tmp41E->hd;void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple10*)((struct Cyc_List_List*)
_check_null(_tmp5FC))->hd)).f2);Cyc_Toc_exp_to_c(nv,cur_e);if(Cyc_Toc_is_void_star_or_tvar(
field_typ))cur_e=Cyc_Toc_cast_it(field_typ,cur_e);{struct _tuple16*_tmpB56;struct
Cyc_List_List*_tmpB55;dles=((_tmpB55=_cycalloc(sizeof(*_tmpB55)),((_tmpB55->hd=((
_tmpB56=_cycalloc(sizeof(*_tmpB56)),((_tmpB56->f1=0,((_tmpB56->f2=cur_e,_tmpB56)))))),((
_tmpB55->tl=dles,_tmpB55))))));}}{struct _tuple16*_tmpB59;struct Cyc_List_List*
_tmpB58;dles=((_tmpB58=_cycalloc(sizeof(*_tmpB58)),((_tmpB58->hd=((_tmpB59=
_cycalloc(sizeof(*_tmpB59)),((_tmpB59->f1=0,((_tmpB59->f2=tag_exp,_tmpB59)))))),((
_tmpB58->tl=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
dles),_tmpB58))))));}e->r=Cyc_Toc_unresolvedmem_exp_r(0,dles);}else{struct Cyc_List_List*
_tmpB5A;struct Cyc_List_List*_tmp601=(_tmpB5A=_cycalloc(sizeof(*_tmpB5A)),((
_tmpB5A->hd=Cyc_Absyn_assign_stmt(Cyc_Absyn_aggrmember_exp(member_exp,Cyc_Toc_tag_sp,
0),tag_exp,0),((_tmpB5A->tl=0,_tmpB5A)))));{int i=1;for(0;_tmp41E != 0;(((_tmp41E=
_tmp41E->tl,i ++)),_tmp5FC=_tmp5FC->tl)){struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)
_tmp41E->hd;void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple10*)((struct Cyc_List_List*)
_check_null(_tmp5FC))->hd)).f2);Cyc_Toc_exp_to_c(nv,cur_e);if(Cyc_Toc_is_void_star_or_tvar(
field_typ))cur_e=Cyc_Toc_cast_it(field_typ,cur_e);{struct Cyc_Absyn_Stmt*_tmp602=
Cyc_Absyn_assign_stmt(Cyc_Absyn_aggrmember_exp(member_exp,Cyc_Absyn_fieldname(i),
0),cur_e,0);struct Cyc_List_List*_tmpB5B;_tmp601=((_tmpB5B=_cycalloc(sizeof(*
_tmpB5B)),((_tmpB5B->hd=_tmp602,((_tmpB5B->tl=_tmp601,_tmpB5B))))));}}}{struct
Cyc_Absyn_Stmt*_tmp604=Cyc_Absyn_exp_stmt(_tmp5FA,0);struct Cyc_List_List*_tmpB5C;
struct Cyc_Absyn_Stmt*_tmp605=Cyc_Absyn_seq_stmts(((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(((_tmpB5C=_cycalloc(sizeof(*_tmpB5C)),((
_tmpB5C->hd=_tmp604,((_tmpB5C->tl=_tmp601,_tmpB5C))))))),0);e->r=Cyc_Toc_stmt_exp_r(
Cyc_Absyn_declare_stmt(_tmp5F9,datatype_ctype,0,_tmp605,0));}}goto _LL20C;}}
_LL255: if(*((int*)_tmp3CE)!= 33)goto _LL257;_LL256: goto _LL258;_LL257: if(*((int*)
_tmp3CE)!= 34)goto _LL259;_LL258: goto _LL20C;_LL259: if(*((int*)_tmp3CE)!= 35)goto
_LL25B;_tmp421=((struct Cyc_Absyn_Malloc_e_struct*)_tmp3CE)->f1;_tmp422=_tmp421.is_calloc;
_tmp423=_tmp421.rgn;_tmp424=_tmp421.elt_type;_tmp425=_tmp421.num_elts;_tmp426=
_tmp421.fat_result;_LL25A: {void*t_c=Cyc_Toc_typ_to_c(*((void**)_check_null(
_tmp424)));Cyc_Toc_exp_to_c(nv,_tmp425);if(_tmp426){struct _tuple1*_tmp608=Cyc_Toc_temp_var();
struct _tuple1*_tmp609=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*pexp;struct Cyc_Absyn_Exp*
xexp;struct Cyc_Absyn_Exp*rexp;if(_tmp422){xexp=_tmp425;if(_tmp423 != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=(struct Cyc_Absyn_Exp*)_tmp423;Cyc_Toc_exp_to_c(nv,rgn);
pexp=Cyc_Toc_rcalloc_exp(rgn,Cyc_Absyn_sizeoftyp_exp(t_c,0),Cyc_Absyn_var_exp(
_tmp608,0));}else{pexp=Cyc_Toc_calloc_exp(*_tmp424,Cyc_Absyn_sizeoftyp_exp(t_c,0),
Cyc_Absyn_var_exp(_tmp608,0));}{struct Cyc_Absyn_Exp*_tmpB5D[3];rexp=Cyc_Absyn_fncall_exp(
Cyc_Toc__tag_dyneither_e,((_tmpB5D[2]=Cyc_Absyn_var_exp(_tmp608,0),((_tmpB5D[1]=
Cyc_Absyn_sizeoftyp_exp(t_c,0),((_tmpB5D[0]=Cyc_Absyn_var_exp(_tmp609,0),((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmpB5D,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);}}else{xexp=Cyc_Absyn_times_exp(
Cyc_Absyn_sizeoftyp_exp(t_c,0),_tmp425,0);if(_tmp423 != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=(struct Cyc_Absyn_Exp*)_tmp423;Cyc_Toc_exp_to_c(nv,rgn);
pexp=Cyc_Toc_rmalloc_exp(rgn,Cyc_Absyn_var_exp(_tmp608,0));}else{pexp=Cyc_Toc_malloc_exp(*
_tmp424,Cyc_Absyn_var_exp(_tmp608,0));}{struct Cyc_Absyn_Exp*_tmpB5E[3];rexp=Cyc_Absyn_fncall_exp(
Cyc_Toc__tag_dyneither_e,((_tmpB5E[2]=Cyc_Absyn_var_exp(_tmp608,0),((_tmpB5E[1]=
Cyc_Absyn_uint_exp(1,0),((_tmpB5E[0]=Cyc_Absyn_var_exp(_tmp609,0),((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB5E,sizeof(struct Cyc_Absyn_Exp*),
3)))))))),0);}}{struct Cyc_Absyn_Stmt*_tmp60C=Cyc_Absyn_declare_stmt(_tmp608,Cyc_Absyn_uint_typ,(
struct Cyc_Absyn_Exp*)xexp,Cyc_Absyn_declare_stmt(_tmp609,Cyc_Absyn_cstar_typ(t_c,
Cyc_Toc_mt_tq),(struct Cyc_Absyn_Exp*)pexp,Cyc_Absyn_exp_stmt(rexp,0),0),0);e->r=
Cyc_Toc_stmt_exp_r(_tmp60C);}}else{struct Cyc_Absyn_Exp*_tmp60D=Cyc_Absyn_sizeoftyp_exp(
t_c,0);{void*_tmp60E=_tmp425->r;union Cyc_Absyn_Cnst _tmp60F;struct _tuple6 _tmp610;
int _tmp611;_LL341: if(*((int*)_tmp60E)!= 0)goto _LL343;_tmp60F=((struct Cyc_Absyn_Const_e_struct*)
_tmp60E)->f1;if((_tmp60F.Int_c).tag != 4)goto _LL343;_tmp610=(struct _tuple6)(
_tmp60F.Int_c).val;_tmp611=_tmp610.f2;if(_tmp611 != 1)goto _LL343;_LL342: goto
_LL340;_LL343:;_LL344: _tmp60D=Cyc_Absyn_times_exp(_tmp60D,_tmp425,0);goto _LL340;
_LL340:;}if(_tmp423 != 0  && !Cyc_Absyn_no_regions){struct Cyc_Absyn_Exp*rgn=(
struct Cyc_Absyn_Exp*)_tmp423;Cyc_Toc_exp_to_c(nv,rgn);e->r=(Cyc_Toc_rmalloc_exp(
rgn,_tmp60D))->r;}else{e->r=(Cyc_Toc_malloc_exp(*_tmp424,_tmp60D))->r;}}goto
_LL20C;}_LL25B: if(*((int*)_tmp3CE)!= 36)goto _LL25D;_tmp427=((struct Cyc_Absyn_Swap_e_struct*)
_tmp3CE)->f1;_tmp428=((struct Cyc_Absyn_Swap_e_struct*)_tmp3CE)->f2;_LL25C: {int
is_dyneither_ptr=0;void*e1_old_typ=(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp427->topt))->v;void*e2_old_typ=(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp428->topt))->v;if(!Cyc_Tcutil_is_pointer_or_boxed(e1_old_typ,&
is_dyneither_ptr)){const char*_tmpB61;void*_tmpB60;(_tmpB60=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB61="Swap_e: is_pointer_or_boxed: not a pointer or boxed type",
_tag_dyneither(_tmpB61,sizeof(char),57))),_tag_dyneither(_tmpB60,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*swap_fn;if(is_dyneither_ptr)swap_fn=Cyc_Toc__swap_dyneither_e;
else{swap_fn=Cyc_Toc__swap_word_e;}Cyc_Toc_exp_to_c(nv,_tmp427);Cyc_Toc_exp_to_c(
nv,_tmp428);{struct Cyc_Absyn_Exp*_tmpB62[2];e->r=Cyc_Toc_fncall_exp_r(swap_fn,((
_tmpB62[1]=Cyc_Toc_push_address_exp(_tmp428),((_tmpB62[0]=Cyc_Toc_push_address_exp(
_tmp427),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpB62,sizeof(struct Cyc_Absyn_Exp*),2)))))));}goto _LL20C;}}
_LL25D: if(*((int*)_tmp3CE)!= 39)goto _LL25F;_tmp429=((struct Cyc_Absyn_Tagcheck_e_struct*)
_tmp3CE)->f1;_tmp42A=((struct Cyc_Absyn_Tagcheck_e_struct*)_tmp3CE)->f2;_LL25E: {
void*_tmp615=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp429->topt))->v);Cyc_Toc_exp_to_c(nv,_tmp429);{void*_tmp616=_tmp615;struct Cyc_Absyn_AggrInfo
_tmp617;union Cyc_Absyn_AggrInfoU _tmp618;struct Cyc_Absyn_Aggrdecl**_tmp619;struct
Cyc_Absyn_Aggrdecl*_tmp61A;_LL346: if(_tmp616 <= (void*)4)goto _LL348;if(*((int*)
_tmp616)!= 10)goto _LL348;_tmp617=((struct Cyc_Absyn_AggrType_struct*)_tmp616)->f1;
_tmp618=_tmp617.aggr_info;if((_tmp618.KnownAggr).tag != 2)goto _LL348;_tmp619=(
struct Cyc_Absyn_Aggrdecl**)(_tmp618.KnownAggr).val;_tmp61A=*_tmp619;_LL347: {
struct Cyc_Absyn_Exp*_tmp61B=Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(
_tmp61A,_tmp42A),0);struct Cyc_Absyn_Exp*_tmp61C=Cyc_Absyn_aggrmember_exp(_tmp429,
_tmp42A,0);struct Cyc_Absyn_Exp*_tmp61D=Cyc_Absyn_aggrmember_exp(_tmp61C,Cyc_Toc_tag_sp,
0);e->r=(Cyc_Absyn_eq_exp(_tmp61D,_tmp61B,0))->r;goto _LL345;}_LL348:;_LL349: {
const char*_tmpB65;void*_tmpB64;(_tmpB64=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB65="non-aggregate type in tagcheck",
_tag_dyneither(_tmpB65,sizeof(char),31))),_tag_dyneither(_tmpB64,sizeof(void*),0)));}
_LL345:;}goto _LL20C;}_LL25F: if(*((int*)_tmp3CE)!= 38)goto _LL261;_tmp42B=((struct
Cyc_Absyn_StmtExp_e_struct*)_tmp3CE)->f1;_LL260: Cyc_Toc_stmt_to_c(nv,_tmp42B);
goto _LL20C;_LL261: if(*((int*)_tmp3CE)!= 37)goto _LL263;_LL262: {const char*_tmpB68;
void*_tmpB67;(_tmpB67=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Toc_toc_impos)(((_tmpB68="UnresolvedMem",_tag_dyneither(_tmpB68,sizeof(char),
14))),_tag_dyneither(_tmpB67,sizeof(void*),0)));}_LL263: if(*((int*)_tmp3CE)!= 27)
goto _LL265;_LL264: {const char*_tmpB6B;void*_tmpB6A;(_tmpB6A=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpB6B="compoundlit",
_tag_dyneither(_tmpB6B,sizeof(char),12))),_tag_dyneither(_tmpB6A,sizeof(void*),0)));}
_LL265: if(*((int*)_tmp3CE)!= 40)goto _LL20C;_LL266: {const char*_tmpB6E;void*
_tmpB6D;(_tmpB6D=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpB6E="valueof(-)",_tag_dyneither(_tmpB6E,sizeof(char),11))),_tag_dyneither(
_tmpB6D,sizeof(void*),0)));}_LL20C:;}}static struct Cyc_Absyn_Stmt*Cyc_Toc_if_neq_stmt(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Stmt*fail_stmt);
static struct Cyc_Absyn_Stmt*Cyc_Toc_if_neq_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2,struct Cyc_Absyn_Stmt*fail_stmt){return Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_neq_exp(
e1,e2,0),fail_stmt,Cyc_Toc_skip_stmt_dl(),0);}struct _tuple21{struct Cyc_Toc_Env*
f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple22{struct _tuple1*
f1;void*f2;};struct _tuple23{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};
static struct _tuple21 Cyc_Toc_xlate_pat(struct Cyc_Toc_Env*nv,struct _RegionHandle*
rgn,void*t,struct Cyc_Absyn_Exp*r,struct Cyc_Absyn_Exp*path,struct Cyc_Absyn_Pat*p,
struct Cyc_Absyn_Stmt**tag_fail_stmt,struct Cyc_Absyn_Stmt*fail_stmt,struct Cyc_List_List*
decls);static struct _tuple21 Cyc_Toc_xlate_pat(struct Cyc_Toc_Env*nv,struct
_RegionHandle*rgn,void*t,struct Cyc_Absyn_Exp*r,struct Cyc_Absyn_Exp*path,struct
Cyc_Absyn_Pat*p,struct Cyc_Absyn_Stmt**tag_fail_stmt,struct Cyc_Absyn_Stmt*
fail_stmt,struct Cyc_List_List*decls){struct Cyc_Absyn_Stmt*s;{void*_tmp626=p->r;
struct Cyc_Absyn_Vardecl*_tmp627;struct Cyc_Absyn_Vardecl _tmp628;struct _tuple1*
_tmp629;struct Cyc_Absyn_Pat*_tmp62A;struct Cyc_Absyn_Vardecl*_tmp62B;struct Cyc_Absyn_Vardecl
_tmp62C;struct _tuple1*_tmp62D;struct Cyc_Absyn_Vardecl*_tmp62E;struct Cyc_Absyn_Pat*
_tmp62F;void*_tmp630;int _tmp631;char _tmp632;struct _dyneither_ptr _tmp633;struct
Cyc_Absyn_Enumdecl*_tmp634;struct Cyc_Absyn_Enumfield*_tmp635;void*_tmp636;struct
Cyc_Absyn_Enumfield*_tmp637;struct Cyc_Absyn_Datatypedecl*_tmp638;struct Cyc_Absyn_Datatypefield*
_tmp639;struct Cyc_List_List*_tmp63A;struct Cyc_Absyn_Pat*_tmp63B;struct Cyc_Absyn_Pat
_tmp63C;void*_tmp63D;struct Cyc_Absyn_Datatypedecl*_tmp63E;struct Cyc_Absyn_Datatypefield*
_tmp63F;struct Cyc_List_List*_tmp640;struct Cyc_List_List*_tmp641;struct Cyc_List_List*
_tmp642;struct Cyc_Absyn_AggrInfo*_tmp643;struct Cyc_Absyn_AggrInfo*_tmp644;struct
Cyc_Absyn_AggrInfo _tmp645;union Cyc_Absyn_AggrInfoU _tmp646;struct Cyc_List_List*
_tmp647;struct Cyc_Absyn_Pat*_tmp648;_LL34B: if(_tmp626 <= (void*)2)goto _LL34F;if(*((
int*)_tmp626)!= 0)goto _LL34D;_tmp627=((struct Cyc_Absyn_Var_p_struct*)_tmp626)->f1;
_tmp628=*_tmp627;_tmp629=_tmp628.name;_tmp62A=((struct Cyc_Absyn_Var_p_struct*)
_tmp626)->f2;_LL34C: return Cyc_Toc_xlate_pat(Cyc_Toc_add_varmap(rgn,nv,_tmp629,r),
rgn,t,r,path,_tmp62A,tag_fail_stmt,fail_stmt,decls);_LL34D: if(*((int*)_tmp626)!= 
2)goto _LL34F;_tmp62B=((struct Cyc_Absyn_TagInt_p_struct*)_tmp626)->f2;_tmp62C=*
_tmp62B;_tmp62D=_tmp62C.name;_LL34E: nv=Cyc_Toc_add_varmap(rgn,nv,_tmp62D,r);goto
_LL350;_LL34F: if((int)_tmp626 != 0)goto _LL351;_LL350: s=Cyc_Toc_skip_stmt_dl();
goto _LL34A;_LL351: if(_tmp626 <= (void*)2)goto _LL353;if(*((int*)_tmp626)!= 1)goto
_LL353;_tmp62E=((struct Cyc_Absyn_Reference_p_struct*)_tmp626)->f1;_tmp62F=((
struct Cyc_Absyn_Reference_p_struct*)_tmp626)->f2;_LL352: {struct _tuple1*_tmp649=
Cyc_Toc_temp_var();{struct _tuple22*_tmpB71;struct Cyc_List_List*_tmpB70;decls=((
_tmpB70=_region_malloc(rgn,sizeof(*_tmpB70)),((_tmpB70->hd=((_tmpB71=
_region_malloc(rgn,sizeof(*_tmpB71)),((_tmpB71->f1=_tmp649,((_tmpB71->f2=Cyc_Absyn_cstar_typ(
Cyc_Toc_typ_to_c(t),Cyc_Toc_mt_tq),_tmpB71)))))),((_tmpB70->tl=decls,_tmpB70))))));}
nv=Cyc_Toc_add_varmap(rgn,nv,_tmp62E->name,Cyc_Absyn_var_exp(_tmp649,0));s=Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp(_tmp649,0),Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c(
t),Cyc_Toc_mt_tq),Cyc_Toc_push_address_exp(path)),0);{struct _tuple21 _tmp64C=Cyc_Toc_xlate_pat(
nv,rgn,t,r,path,_tmp62F,tag_fail_stmt,fail_stmt,decls);_tmp64C.f3=Cyc_Absyn_seq_stmt(
s,_tmp64C.f3,0);return _tmp64C;}}_LL353: if((int)_tmp626 != 1)goto _LL355;_LL354: s=
Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_signed_int_exp(0,0),fail_stmt);goto _LL34A;_LL355:
if(_tmp626 <= (void*)2)goto _LL357;if(*((int*)_tmp626)!= 7)goto _LL357;_tmp630=(
void*)((struct Cyc_Absyn_Int_p_struct*)_tmp626)->f1;_tmp631=((struct Cyc_Absyn_Int_p_struct*)
_tmp626)->f2;_LL356: s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_int_exp(_tmp630,_tmp631,0),
fail_stmt);goto _LL34A;_LL357: if(_tmp626 <= (void*)2)goto _LL359;if(*((int*)_tmp626)
!= 8)goto _LL359;_tmp632=((struct Cyc_Absyn_Char_p_struct*)_tmp626)->f1;_LL358: s=
Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_char_exp(_tmp632,0),fail_stmt);goto _LL34A;_LL359:
if(_tmp626 <= (void*)2)goto _LL35B;if(*((int*)_tmp626)!= 9)goto _LL35B;_tmp633=((
struct Cyc_Absyn_Float_p_struct*)_tmp626)->f1;_LL35A: s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_float_exp(
_tmp633,0),fail_stmt);goto _LL34A;_LL35B: if(_tmp626 <= (void*)2)goto _LL35D;if(*((
int*)_tmp626)!= 10)goto _LL35D;_tmp634=((struct Cyc_Absyn_Enum_p_struct*)_tmp626)->f1;
_tmp635=((struct Cyc_Absyn_Enum_p_struct*)_tmp626)->f2;_LL35C:{struct Cyc_Absyn_Enum_e_struct
_tmpB74;struct Cyc_Absyn_Enum_e_struct*_tmpB73;s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_new_exp((
void*)((_tmpB73=_cycalloc(sizeof(*_tmpB73)),((_tmpB73[0]=((_tmpB74.tag=33,((
_tmpB74.f1=_tmp635->name,((_tmpB74.f2=(struct Cyc_Absyn_Enumdecl*)_tmp634,((
_tmpB74.f3=(struct Cyc_Absyn_Enumfield*)_tmp635,_tmpB74)))))))),_tmpB73)))),0),
fail_stmt);}goto _LL34A;_LL35D: if(_tmp626 <= (void*)2)goto _LL35F;if(*((int*)
_tmp626)!= 11)goto _LL35F;_tmp636=(void*)((struct Cyc_Absyn_AnonEnum_p_struct*)
_tmp626)->f1;_tmp637=((struct Cyc_Absyn_AnonEnum_p_struct*)_tmp626)->f2;_LL35E:{
struct Cyc_Absyn_AnonEnum_e_struct _tmpB77;struct Cyc_Absyn_AnonEnum_e_struct*
_tmpB76;s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_new_exp((void*)((_tmpB76=_cycalloc(
sizeof(*_tmpB76)),((_tmpB76[0]=((_tmpB77.tag=34,((_tmpB77.f1=_tmp637->name,((
_tmpB77.f2=(void*)_tmp636,((_tmpB77.f3=(struct Cyc_Absyn_Enumfield*)_tmp637,
_tmpB77)))))))),_tmpB76)))),0),fail_stmt);}goto _LL34A;_LL35F: if(_tmp626 <= (void*)
2)goto _LL361;if(*((int*)_tmp626)!= 6)goto _LL361;_tmp638=((struct Cyc_Absyn_Datatype_p_struct*)
_tmp626)->f1;_tmp639=((struct Cyc_Absyn_Datatype_p_struct*)_tmp626)->f2;_tmp63A=((
struct Cyc_Absyn_Datatype_p_struct*)_tmp626)->f3;if(_tmp63A != 0)goto _LL361;_LL360: {
struct Cyc_Absyn_Exp*cmp_exp;if(_tmp638->is_extensible)cmp_exp=Cyc_Absyn_var_exp(
_tmp639->name,0);else{cmp_exp=Cyc_Toc_datatype_tag(_tmp638,_tmp639->name,0);r=
Cyc_Toc_cast_it(Cyc_Absyn_sint_typ,r);}s=Cyc_Toc_if_neq_stmt(r,cmp_exp,fail_stmt);
goto _LL34A;}_LL361: if(_tmp626 <= (void*)2)goto _LL363;if(*((int*)_tmp626)!= 4)goto
_LL363;_tmp63B=((struct Cyc_Absyn_Pointer_p_struct*)_tmp626)->f1;_tmp63C=*_tmp63B;
_tmp63D=_tmp63C.r;if(_tmp63D <= (void*)2)goto _LL363;if(*((int*)_tmp63D)!= 6)goto
_LL363;_tmp63E=((struct Cyc_Absyn_Datatype_p_struct*)_tmp63D)->f1;_tmp63F=((
struct Cyc_Absyn_Datatype_p_struct*)_tmp63D)->f2;_tmp640=((struct Cyc_Absyn_Datatype_p_struct*)
_tmp63D)->f3;if(!(_tmp640 != 0))goto _LL363;_LL362: s=Cyc_Toc_skip_stmt_dl();{int
cnt=1;const char*_tmpB78;struct _tuple1*tufstrct=Cyc_Toc_collapse_qvar_tag(_tmp63F->name,((
_tmpB78="_struct",_tag_dyneither(_tmpB78,sizeof(char),8))));struct Cyc_Absyn_Exp*
rcast=Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(Cyc_Absyn_strctq(tufstrct),Cyc_Toc_mt_tq),
r);struct Cyc_List_List*_tmp651=_tmp63F->typs;for(0;_tmp640 != 0;(((_tmp640=
_tmp640->tl,_tmp651=((struct Cyc_List_List*)_check_null(_tmp651))->tl)),++ cnt)){
struct Cyc_Absyn_Pat*_tmp652=(struct Cyc_Absyn_Pat*)_tmp640->hd;if(_tmp652->r == (
void*)0)continue;{void*_tmp653=(*((struct _tuple10*)((struct Cyc_List_List*)
_check_null(_tmp651))->hd)).f2;struct _tuple1*_tmp654=Cyc_Toc_temp_var();void*
_tmp655=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp652->topt))->v;void*_tmp656=
Cyc_Toc_typ_to_c(_tmp655);struct Cyc_Absyn_Exp*_tmp657=Cyc_Absyn_aggrarrow_exp(
rcast,Cyc_Absyn_fieldname(cnt),0);if(Cyc_Toc_is_void_star_or_tvar(Cyc_Toc_typ_to_c(
_tmp653)))_tmp657=Cyc_Toc_cast_it(_tmp656,_tmp657);{struct _tuple22*_tmpB7B;
struct Cyc_List_List*_tmpB7A;decls=((_tmpB7A=_region_malloc(rgn,sizeof(*_tmpB7A)),((
_tmpB7A->hd=((_tmpB7B=_region_malloc(rgn,sizeof(*_tmpB7B)),((_tmpB7B->f1=_tmp654,((
_tmpB7B->f2=_tmp656,_tmpB7B)))))),((_tmpB7A->tl=decls,_tmpB7A))))));}{struct
_tuple21 _tmp65A=Cyc_Toc_xlate_pat(nv,rgn,_tmp655,Cyc_Absyn_var_exp(_tmp654,0),
_tmp657,_tmp652,(struct Cyc_Absyn_Stmt**)& fail_stmt,fail_stmt,decls);nv=_tmp65A.f1;
decls=_tmp65A.f2;{struct Cyc_Absyn_Stmt*_tmp65B=_tmp65A.f3;struct Cyc_Absyn_Stmt*
_tmp65C=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp654,0),_tmp657,0);s=Cyc_Absyn_seq_stmt(
s,Cyc_Absyn_seq_stmt(_tmp65C,_tmp65B,0),0);}}}}{struct Cyc_Absyn_Exp*test_exp;if(
_tmp63E->is_extensible){struct Cyc_Absyn_Exp*e2=Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(
Cyc_Absyn_void_star_typ(),Cyc_Toc_mt_tq),r);struct Cyc_Absyn_Exp*e1=Cyc_Absyn_deref_exp(
e2,0);struct Cyc_Absyn_Exp*e=Cyc_Absyn_var_exp(_tmp63F->name,0);test_exp=Cyc_Absyn_neq_exp(
e1,e,0);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(test_exp,fail_stmt,Cyc_Toc_skip_stmt_dl(),
0),s,0);}else{struct Cyc_Absyn_Exp*e3=Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(Cyc_Absyn_sint_typ,
Cyc_Toc_mt_tq),r);struct Cyc_Absyn_Exp*e1=Cyc_Absyn_deref_exp(e3,0);struct Cyc_Absyn_Exp*
e=Cyc_Toc_datatype_tag(_tmp63E,_tmp63F->name,1);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(
Cyc_Absyn_neq_exp(e1,e,0),fail_stmt,Cyc_Toc_skip_stmt_dl(),0),s,0);if(
tag_fail_stmt != 0){int max_tag=Cyc_Toc_num_void_tags(_tmp63E);if(max_tag != 0){
struct Cyc_Absyn_Exp*max_tag_exp=Cyc_Absyn_uint_exp((unsigned int)max_tag,0);
struct Cyc_Absyn_Exp*e5=Cyc_Absyn_lte_exp(r,Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),
max_tag_exp),0);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(e5,*tag_fail_stmt,
Cyc_Toc_skip_stmt_dl(),0),s,0);}}}goto _LL34A;}}_LL363: if(_tmp626 <= (void*)2)goto
_LL365;if(*((int*)_tmp626)!= 6)goto _LL365;_tmp641=((struct Cyc_Absyn_Datatype_p_struct*)
_tmp626)->f3;_LL364: _tmp642=_tmp641;goto _LL366;_LL365: if(_tmp626 <= (void*)2)goto
_LL367;if(*((int*)_tmp626)!= 3)goto _LL367;_tmp642=((struct Cyc_Absyn_Tuple_p_struct*)
_tmp626)->f1;_LL366: s=Cyc_Toc_skip_stmt_dl();{int cnt=1;for(0;_tmp642 != 0;(
_tmp642=_tmp642->tl,++ cnt)){struct Cyc_Absyn_Pat*_tmp65E=(struct Cyc_Absyn_Pat*)
_tmp642->hd;if(_tmp65E->r == (void*)0)continue;{struct _tuple1*_tmp65F=Cyc_Toc_temp_var();
void*_tmp660=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp65E->topt))->v;{struct
_tuple22*_tmpB7E;struct Cyc_List_List*_tmpB7D;decls=((_tmpB7D=_region_malloc(rgn,
sizeof(*_tmpB7D)),((_tmpB7D->hd=((_tmpB7E=_region_malloc(rgn,sizeof(*_tmpB7E)),((
_tmpB7E->f1=_tmp65F,((_tmpB7E->f2=Cyc_Toc_typ_to_c(_tmp660),_tmpB7E)))))),((
_tmpB7D->tl=decls,_tmpB7D))))));}{struct _tuple21 _tmp663=Cyc_Toc_xlate_pat(nv,rgn,
_tmp660,Cyc_Absyn_var_exp(_tmp65F,0),Cyc_Absyn_aggrmember_exp(path,Cyc_Absyn_fieldname(
cnt),0),_tmp65E,(struct Cyc_Absyn_Stmt**)& fail_stmt,fail_stmt,decls);nv=_tmp663.f1;
decls=_tmp663.f2;{struct Cyc_Absyn_Stmt*_tmp664=_tmp663.f3;struct Cyc_Absyn_Stmt*
_tmp665=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp65F,0),Cyc_Absyn_aggrmember_exp(
r,Cyc_Absyn_fieldname(cnt),0),0);s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_seq_stmt(
_tmp665,_tmp664,0),0);}}}}goto _LL34A;}_LL367: if(_tmp626 <= (void*)2)goto _LL369;
if(*((int*)_tmp626)!= 5)goto _LL369;_tmp643=((struct Cyc_Absyn_Aggr_p_struct*)
_tmp626)->f1;if(_tmp643 != 0)goto _LL369;_LL368: {const char*_tmpB81;void*_tmpB80;(
_tmpB80=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpB81="unresolved aggregate pattern!",_tag_dyneither(_tmpB81,sizeof(char),30))),
_tag_dyneither(_tmpB80,sizeof(void*),0)));}_LL369: if(_tmp626 <= (void*)2)goto
_LL36B;if(*((int*)_tmp626)!= 5)goto _LL36B;_tmp644=((struct Cyc_Absyn_Aggr_p_struct*)
_tmp626)->f1;if(_tmp644 == 0)goto _LL36B;_tmp645=*_tmp644;_tmp646=_tmp645.aggr_info;
_tmp647=((struct Cyc_Absyn_Aggr_p_struct*)_tmp626)->f3;_LL36A: {struct Cyc_Absyn_Aggrdecl*
_tmp668=Cyc_Absyn_get_known_aggrdecl(_tmp646);if(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp668->impl))->tagged){struct _tuple23 _tmp66A;struct Cyc_List_List*
_tmp66B;struct Cyc_Absyn_Pat*_tmp66C;struct _tuple23*_tmp669=(struct _tuple23*)((
struct Cyc_List_List*)_check_null(_tmp647))->hd;_tmp66A=*_tmp669;_tmp66B=_tmp66A.f1;
_tmp66C=_tmp66A.f2;{struct _dyneither_ptr*f;{void*_tmp66D=(void*)((struct Cyc_List_List*)
_check_null(_tmp66B))->hd;struct _dyneither_ptr*_tmp66E;_LL374: if(*((int*)_tmp66D)
!= 1)goto _LL376;_tmp66E=((struct Cyc_Absyn_FieldName_struct*)_tmp66D)->f1;_LL375:
f=_tmp66E;goto _LL373;_LL376:;_LL377: {const char*_tmpB84;void*_tmpB83;(_tmpB83=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpB84="no field name in tagged union pattern",_tag_dyneither(_tmpB84,sizeof(
char),38))),_tag_dyneither(_tmpB83,sizeof(void*),0)));}_LL373:;}{struct _tuple1*
_tmp671=Cyc_Toc_temp_var();void*_tmp672=(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp66C->topt))->v;void*_tmp673=Cyc_Toc_typ_to_c(_tmp672);{struct _tuple22*
_tmpB87;struct Cyc_List_List*_tmpB86;decls=((_tmpB86=_region_malloc(rgn,sizeof(*
_tmpB86)),((_tmpB86->hd=((_tmpB87=_region_malloc(rgn,sizeof(*_tmpB87)),((_tmpB87->f1=
_tmp671,((_tmpB87->f2=_tmp673,_tmpB87)))))),((_tmpB86->tl=decls,_tmpB86))))));}{
struct Cyc_Absyn_Exp*_tmp676=Cyc_Absyn_aggrmember_exp(Cyc_Absyn_aggrmember_exp(
path,f,0),Cyc_Toc_val_sp,0);struct Cyc_Absyn_Exp*_tmp677=Cyc_Absyn_aggrmember_exp(
Cyc_Absyn_aggrmember_exp(r,f,0),Cyc_Toc_val_sp,0);_tmp677=Cyc_Toc_cast_it(
_tmp673,_tmp677);_tmp676=Cyc_Toc_cast_it(_tmp673,_tmp676);{struct _tuple21 _tmp678=
Cyc_Toc_xlate_pat(nv,rgn,_tmp672,Cyc_Absyn_var_exp(_tmp671,0),_tmp676,_tmp66C,(
struct Cyc_Absyn_Stmt**)& fail_stmt,fail_stmt,decls);nv=_tmp678.f1;decls=_tmp678.f2;{
struct Cyc_Absyn_Stmt*_tmp679=_tmp678.f3;struct Cyc_Absyn_Stmt*_tmp67A=Cyc_Toc_if_neq_stmt(
Cyc_Absyn_aggrmember_exp(Cyc_Absyn_aggrmember_exp(r,f,0),Cyc_Toc_tag_sp,0),Cyc_Absyn_signed_int_exp(
Cyc_Toc_get_member_offset(_tmp668,f),0),fail_stmt);struct Cyc_Absyn_Stmt*_tmp67B=
Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp671,0),_tmp677,0);s=Cyc_Absyn_seq_stmt(
_tmp67A,Cyc_Absyn_seq_stmt(_tmp67B,_tmp679,0),0);}}}}}}else{s=Cyc_Toc_skip_stmt_dl();
for(0;_tmp647 != 0;_tmp647=_tmp647->tl){struct _tuple23*_tmp67C=(struct _tuple23*)
_tmp647->hd;struct Cyc_Absyn_Pat*_tmp67D=(*_tmp67C).f2;if(_tmp67D->r == (void*)0)
continue;{struct _dyneither_ptr*f;{void*_tmp67E=(void*)((struct Cyc_List_List*)
_check_null((*_tmp67C).f1))->hd;struct _dyneither_ptr*_tmp67F;_LL379: if(*((int*)
_tmp67E)!= 1)goto _LL37B;_tmp67F=((struct Cyc_Absyn_FieldName_struct*)_tmp67E)->f1;
_LL37A: f=_tmp67F;goto _LL378;_LL37B:;_LL37C:(int)_throw((void*)Cyc_Toc_Match_error);
_LL378:;}{struct _tuple1*_tmp680=Cyc_Toc_temp_var();void*_tmp681=(void*)((struct
Cyc_Core_Opt*)_check_null(_tmp67D->topt))->v;void*_tmp682=Cyc_Toc_typ_to_c(
_tmp681);{struct _tuple22*_tmpB8A;struct Cyc_List_List*_tmpB89;decls=((_tmpB89=
_region_malloc(rgn,sizeof(*_tmpB89)),((_tmpB89->hd=((_tmpB8A=_region_malloc(rgn,
sizeof(*_tmpB8A)),((_tmpB8A->f1=_tmp680,((_tmpB8A->f2=_tmp682,_tmpB8A)))))),((
_tmpB89->tl=decls,_tmpB89))))));}{struct _tuple21 _tmp685=Cyc_Toc_xlate_pat(nv,rgn,
_tmp681,Cyc_Absyn_var_exp(_tmp680,0),Cyc_Absyn_aggrmember_exp(path,f,0),_tmp67D,(
struct Cyc_Absyn_Stmt**)& fail_stmt,fail_stmt,decls);nv=_tmp685.f1;decls=_tmp685.f2;{
struct Cyc_Absyn_Exp*_tmp686=Cyc_Absyn_aggrmember_exp(r,f,0);if(Cyc_Toc_is_void_star_or_tvar(((
struct Cyc_Absyn_Aggrfield*)_check_null(Cyc_Absyn_lookup_field(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp668->impl))->fields,f)))->type))_tmp686=Cyc_Toc_cast_it(_tmp682,
_tmp686);{struct Cyc_Absyn_Stmt*_tmp687=_tmp685.f3;struct Cyc_Absyn_Stmt*_tmp688=
Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp680,0),_tmp686,0);s=Cyc_Absyn_seq_stmt(
s,Cyc_Absyn_seq_stmt(_tmp688,_tmp687,0),0);}}}}}}}goto _LL34A;}_LL36B: if(_tmp626
<= (void*)2)goto _LL36D;if(*((int*)_tmp626)!= 4)goto _LL36D;_tmp648=((struct Cyc_Absyn_Pointer_p_struct*)
_tmp626)->f1;_LL36C: {struct _tuple1*_tmp689=Cyc_Toc_temp_var();void*_tmp68A=(
void*)((struct Cyc_Core_Opt*)_check_null(_tmp648->topt))->v;{struct _tuple22*
_tmpB8D;struct Cyc_List_List*_tmpB8C;decls=((_tmpB8C=_region_malloc(rgn,sizeof(*
_tmpB8C)),((_tmpB8C->hd=((_tmpB8D=_region_malloc(rgn,sizeof(*_tmpB8D)),((_tmpB8D->f1=
_tmp689,((_tmpB8D->f2=Cyc_Toc_typ_to_c(_tmp68A),_tmpB8D)))))),((_tmpB8C->tl=
decls,_tmpB8C))))));}{struct _tuple21 _tmp68D=Cyc_Toc_xlate_pat(nv,rgn,_tmp68A,Cyc_Absyn_var_exp(
_tmp689,0),Cyc_Absyn_deref_exp(path,0),_tmp648,(struct Cyc_Absyn_Stmt**)&
fail_stmt,fail_stmt,decls);nv=_tmp68D.f1;decls=_tmp68D.f2;{struct Cyc_Absyn_Stmt*
_tmp68E=_tmp68D.f3;struct Cyc_Absyn_Stmt*_tmp68F=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp(_tmp689,0),Cyc_Absyn_deref_exp(r,0),0),_tmp68E,0);if(Cyc_Toc_is_nullable(
t))s=Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_eq_exp(r,Cyc_Absyn_signed_int_exp(
0,0),0),fail_stmt,Cyc_Toc_skip_stmt_dl(),0),_tmp68F,0);else{s=_tmp68F;}goto
_LL34A;}}}_LL36D: if(_tmp626 <= (void*)2)goto _LL36F;if(*((int*)_tmp626)!= 12)goto
_LL36F;_LL36E: {const char*_tmpB90;void*_tmpB8F;(_tmpB8F=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB90="unknownid pat",
_tag_dyneither(_tmpB90,sizeof(char),14))),_tag_dyneither(_tmpB8F,sizeof(void*),0)));}
_LL36F: if(_tmp626 <= (void*)2)goto _LL371;if(*((int*)_tmp626)!= 13)goto _LL371;
_LL370: {const char*_tmpB93;void*_tmpB92;(_tmpB92=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB93="unknowncall pat",
_tag_dyneither(_tmpB93,sizeof(char),16))),_tag_dyneither(_tmpB92,sizeof(void*),0)));}
_LL371: if(_tmp626 <= (void*)2)goto _LL34A;if(*((int*)_tmp626)!= 14)goto _LL34A;
_LL372: {const char*_tmpB96;void*_tmpB95;(_tmpB95=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB96="exp pat",
_tag_dyneither(_tmpB96,sizeof(char),8))),_tag_dyneither(_tmpB95,sizeof(void*),0)));}
_LL34A:;}{struct _tuple21 _tmpB97;return(_tmpB97.f1=nv,((_tmpB97.f2=decls,((
_tmpB97.f3=s,_tmpB97)))));}}struct _tuple24{struct _dyneither_ptr*f1;struct
_dyneither_ptr*f2;struct Cyc_Absyn_Switch_clause*f3;};static struct _tuple24*Cyc_Toc_gen_label(
struct _RegionHandle*r,struct Cyc_Absyn_Switch_clause*sc);static struct _tuple24*Cyc_Toc_gen_label(
struct _RegionHandle*r,struct Cyc_Absyn_Switch_clause*sc){struct _tuple24*_tmpB98;
return(_tmpB98=_region_malloc(r,sizeof(*_tmpB98)),((_tmpB98->f1=Cyc_Toc_fresh_label(),((
_tmpB98->f2=Cyc_Toc_fresh_label(),((_tmpB98->f3=sc,_tmpB98)))))));}static int Cyc_Toc_is_mixed_datatype(
void*t);static int Cyc_Toc_is_mixed_datatype(void*t){{void*_tmp698=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_DatatypeInfo _tmp699;union Cyc_Absyn_DatatypeInfoU _tmp69A;
struct Cyc_Absyn_Datatypedecl**_tmp69B;struct Cyc_Absyn_Datatypedecl*_tmp69C;
_LL37E: if(_tmp698 <= (void*)4)goto _LL380;if(*((int*)_tmp698)!= 2)goto _LL380;
_tmp699=((struct Cyc_Absyn_DatatypeType_struct*)_tmp698)->f1;_tmp69A=_tmp699.datatype_info;
if((_tmp69A.KnownDatatype).tag != 2)goto _LL380;_tmp69B=(struct Cyc_Absyn_Datatypedecl**)(
_tmp69A.KnownDatatype).val;_tmp69C=*_tmp69B;_LL37F: {int seen_novalue=0;int
seen_value=0;{struct Cyc_List_List*_tmp69D=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp69C->fields))->v;for(0;(unsigned int)_tmp69D;_tmp69D=_tmp69D->tl){
if(((struct Cyc_Absyn_Datatypefield*)_tmp69D->hd)->typs == 0)seen_value=1;else{
seen_novalue=1;}if(seen_value  && seen_novalue)return 1;}}goto _LL37D;}_LL380:;
_LL381: goto _LL37D;_LL37D:;}return 0;}static int Cyc_Toc_no_tag_test(struct Cyc_Absyn_Pat*
p);static int Cyc_Toc_no_tag_test(struct Cyc_Absyn_Pat*p){void*_tmp69E=p->r;struct
Cyc_Absyn_Pat*_tmp69F;struct Cyc_Absyn_Pat*_tmp6A0;struct Cyc_Absyn_Pat _tmp6A1;
void*_tmp6A2;struct Cyc_Absyn_Datatypedecl*_tmp6A3;struct Cyc_Absyn_Datatypefield*
_tmp6A4;struct Cyc_List_List*_tmp6A5;_LL383: if(_tmp69E <= (void*)2)goto _LL387;if(*((
int*)_tmp69E)!= 0)goto _LL385;_tmp69F=((struct Cyc_Absyn_Var_p_struct*)_tmp69E)->f2;
_LL384: return Cyc_Toc_no_tag_test(_tmp69F);_LL385: if(*((int*)_tmp69E)!= 4)goto
_LL387;_tmp6A0=((struct Cyc_Absyn_Pointer_p_struct*)_tmp69E)->f1;_tmp6A1=*_tmp6A0;
_tmp6A2=_tmp6A1.r;if(_tmp6A2 <= (void*)2)goto _LL387;if(*((int*)_tmp6A2)!= 6)goto
_LL387;_tmp6A3=((struct Cyc_Absyn_Datatype_p_struct*)_tmp6A2)->f1;_tmp6A4=((
struct Cyc_Absyn_Datatype_p_struct*)_tmp6A2)->f2;_tmp6A5=((struct Cyc_Absyn_Datatype_p_struct*)
_tmp6A2)->f3;if(!(_tmp6A5 != 0))goto _LL387;_LL386: return 0;_LL387:;_LL388: return 1;
_LL382:;}static void Cyc_Toc_xlate_switch(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*
whole_s,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*scs);static void Cyc_Toc_xlate_switch(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*whole_s,struct Cyc_Absyn_Exp*e,struct
Cyc_List_List*scs){Cyc_Toc_exp_to_c(nv,e);{void*_tmp6A6=(void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v;int leave_as_switch;{void*_tmp6A7=Cyc_Tcutil_compress(
_tmp6A6);_LL38A: if(_tmp6A7 <= (void*)4)goto _LL38E;if(*((int*)_tmp6A7)!= 5)goto
_LL38C;_LL38B: goto _LL38D;_LL38C: if(*((int*)_tmp6A7)!= 12)goto _LL38E;_LL38D:
leave_as_switch=1;goto _LL389;_LL38E:;_LL38F: leave_as_switch=0;goto _LL389;_LL389:;}{
struct Cyc_List_List*_tmp6A8=scs;for(0;_tmp6A8 != 0;_tmp6A8=_tmp6A8->tl){if((
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Switch_clause*)
_tmp6A8->hd)->pat_vars))->v != 0  || ((struct Cyc_Absyn_Switch_clause*)_tmp6A8->hd)->where_clause
!= 0){leave_as_switch=0;break;}}}if(leave_as_switch){struct _dyneither_ptr*next_l=
Cyc_Toc_fresh_label();{struct Cyc_List_List*_tmp6A9=scs;for(0;_tmp6A9 != 0;_tmp6A9=
_tmp6A9->tl){struct Cyc_Absyn_Stmt*_tmp6AA=((struct Cyc_Absyn_Switch_clause*)
_tmp6A9->hd)->body;((struct Cyc_Absyn_Switch_clause*)_tmp6A9->hd)->body=Cyc_Absyn_label_stmt(
next_l,_tmp6AA,0);next_l=Cyc_Toc_fresh_label();{struct Cyc_Toc_Env _tmp6AC;struct
_RegionHandle*_tmp6AD;struct Cyc_Toc_Env*_tmp6AB=nv;_tmp6AC=*_tmp6AB;_tmp6AD=
_tmp6AC.rgn;Cyc_Toc_stmt_to_c(Cyc_Toc_switch_as_switch_env(_tmp6AD,nv,next_l),
_tmp6AA);}}}return;}{struct _tuple1*v=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*r=
Cyc_Absyn_var_exp(v,0);struct Cyc_Absyn_Exp*path=Cyc_Absyn_var_exp(v,0);struct
_dyneither_ptr*end_l=Cyc_Toc_fresh_label();struct Cyc_Toc_Env _tmp6AF;struct
_RegionHandle*_tmp6B0;struct Cyc_Toc_Env*_tmp6AE=nv;_tmp6AF=*_tmp6AE;_tmp6B0=
_tmp6AF.rgn;{struct Cyc_Toc_Env*_tmp6B1=Cyc_Toc_share_env(_tmp6B0,nv);struct Cyc_List_List*
lscs=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple24*(*f)(struct
_RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct _RegionHandle*env,struct Cyc_List_List*
x))Cyc_List_rmap_c)(_tmp6B0,Cyc_Toc_gen_label,_tmp6B0,scs);struct Cyc_List_List*
test_stmts=0;struct Cyc_List_List*nvs=0;struct Cyc_List_List*decls=0;int
is_datatype=Cyc_Toc_is_mixed_datatype(_tmp6A6);int needs_tag_test=is_datatype;{
struct Cyc_List_List*_tmp6B2=lscs;for(0;_tmp6B2 != 0;_tmp6B2=_tmp6B2->tl){struct
Cyc_Absyn_Switch_clause*sc=(*((struct _tuple24*)_tmp6B2->hd)).f3;struct
_dyneither_ptr*fail_lab=_tmp6B2->tl == 0?end_l:(*((struct _tuple24*)((struct Cyc_List_List*)
_check_null(_tmp6B2->tl))->hd)).f1;struct Cyc_Absyn_Stmt**tag_fail_stmt=0;if(
needs_tag_test  && !Cyc_Toc_no_tag_test(sc->pattern)){{struct Cyc_List_List*
_tmp6B3=_tmp6B2->tl;for(0;(unsigned int)_tmp6B3;_tmp6B3=_tmp6B3->tl){if(Cyc_Toc_no_tag_test(((*((
struct _tuple24*)_tmp6B3->hd)).f3)->pattern)){{struct Cyc_Absyn_Stmt**_tmpB99;
tag_fail_stmt=((_tmpB99=_region_malloc(_tmp6B0,sizeof(*_tmpB99)),((_tmpB99[0]=
Cyc_Absyn_goto_stmt((*((struct _tuple24*)_tmp6B3->hd)).f1,0),_tmpB99))));}
needs_tag_test=0;break;}}}if(tag_fail_stmt == 0){struct Cyc_Absyn_Stmt**_tmpB9A;
tag_fail_stmt=((_tmpB9A=_region_malloc(_tmp6B0,sizeof(*_tmpB9A)),((_tmpB9A[0]=
Cyc_Absyn_goto_stmt(fail_lab,0),_tmpB9A))));}}{struct Cyc_Toc_Env*_tmp6B7;struct
Cyc_List_List*_tmp6B8;struct Cyc_Absyn_Stmt*_tmp6B9;struct _tuple21 _tmp6B6=Cyc_Toc_xlate_pat(
_tmp6B1,_tmp6B0,_tmp6A6,r,path,sc->pattern,tag_fail_stmt,Cyc_Absyn_goto_stmt(
fail_lab,0),decls);_tmp6B7=_tmp6B6.f1;_tmp6B8=_tmp6B6.f2;_tmp6B9=_tmp6B6.f3;if(
is_datatype  && Cyc_Toc_no_tag_test(sc->pattern))needs_tag_test=1;if(sc->where_clause
!= 0){struct Cyc_Absyn_Exp*_tmp6BA=(struct Cyc_Absyn_Exp*)_check_null(sc->where_clause);
Cyc_Toc_exp_to_c(_tmp6B7,_tmp6BA);_tmp6B9=Cyc_Absyn_seq_stmt(_tmp6B9,Cyc_Absyn_ifthenelse_stmt(
Cyc_Absyn_prim1_exp((void*)11,_tmp6BA,0),Cyc_Absyn_goto_stmt(fail_lab,0),Cyc_Toc_skip_stmt_dl(),
0),0);}decls=_tmp6B8;{struct Cyc_List_List*_tmpB9B;nvs=((_tmpB9B=_region_malloc(
_tmp6B0,sizeof(*_tmpB9B)),((_tmpB9B->hd=_tmp6B7,((_tmpB9B->tl=nvs,_tmpB9B))))));}{
struct Cyc_List_List*_tmpB9C;test_stmts=((_tmpB9C=_region_malloc(_tmp6B0,sizeof(*
_tmpB9C)),((_tmpB9C->hd=_tmp6B9,((_tmpB9C->tl=test_stmts,_tmpB9C))))));}}}}nvs=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(nvs);test_stmts=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(test_stmts);{
struct Cyc_List_List*stmts=0;for(0;lscs != 0;(((lscs=lscs->tl,nvs=nvs->tl)),
test_stmts=test_stmts->tl)){struct _tuple24 _tmp6BE;struct _dyneither_ptr*_tmp6BF;
struct _dyneither_ptr*_tmp6C0;struct Cyc_Absyn_Switch_clause*_tmp6C1;struct
_tuple24*_tmp6BD=(struct _tuple24*)lscs->hd;_tmp6BE=*_tmp6BD;_tmp6BF=_tmp6BE.f1;
_tmp6C0=_tmp6BE.f2;_tmp6C1=_tmp6BE.f3;{struct Cyc_Toc_Env*_tmp6C2=(struct Cyc_Toc_Env*)((
struct Cyc_List_List*)_check_null(nvs))->hd;struct Cyc_Absyn_Stmt*s=_tmp6C1->body;
struct Cyc_Toc_Env _tmp6C4;struct _RegionHandle*_tmp6C5;struct Cyc_Toc_Env*_tmp6C3=
_tmp6B1;_tmp6C4=*_tmp6C3;_tmp6C5=_tmp6C4.rgn;if(lscs->tl != 0){struct _tuple24
_tmp6C7;struct _dyneither_ptr*_tmp6C8;struct Cyc_Absyn_Switch_clause*_tmp6C9;
struct _tuple24*_tmp6C6=(struct _tuple24*)((struct Cyc_List_List*)_check_null(lscs->tl))->hd;
_tmp6C7=*_tmp6C6;_tmp6C8=_tmp6C7.f2;_tmp6C9=_tmp6C7.f3;Cyc_Toc_stmt_to_c(Cyc_Toc_non_last_switchclause_env(
_tmp6C5,_tmp6C2,end_l,_tmp6C8,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp6C9->pat_vars))->v,(struct Cyc_Toc_Env*)((struct Cyc_List_List*)
_check_null(nvs->tl))->hd),s);}else{Cyc_Toc_stmt_to_c(Cyc_Toc_last_switchclause_env(
_tmp6C5,_tmp6C2,end_l),s);}s=Cyc_Absyn_seq_stmt(Cyc_Absyn_label_stmt(_tmp6BF,(
struct Cyc_Absyn_Stmt*)((struct Cyc_List_List*)_check_null(test_stmts))->hd,0),Cyc_Absyn_label_stmt(
_tmp6C0,s,0),0);{struct Cyc_List_List*_tmpB9D;stmts=((_tmpB9D=_region_malloc(
_tmp6B0,sizeof(*_tmpB9D)),((_tmpB9D->hd=s,((_tmpB9D->tl=stmts,_tmpB9D))))));}}}{
struct Cyc_Absyn_Stmt*res=Cyc_Absyn_seq_stmt(Cyc_Absyn_seq_stmts(((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(stmts),0),Cyc_Absyn_label_stmt(end_l,
Cyc_Toc_skip_stmt_dl(),0),0);for(decls;decls != 0;decls=decls->tl){struct _tuple22
_tmp6CC;struct _tuple1*_tmp6CD;void*_tmp6CE;struct _tuple22*_tmp6CB=(struct
_tuple22*)decls->hd;_tmp6CC=*_tmp6CB;_tmp6CD=_tmp6CC.f1;_tmp6CE=_tmp6CC.f2;res=
Cyc_Absyn_declare_stmt(_tmp6CD,_tmp6CE,0,res,0);}whole_s->r=(Cyc_Absyn_declare_stmt(
v,Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),(
struct Cyc_Absyn_Exp*)e,res,0))->r;}}}}}}static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*p,void*t,struct Cyc_Absyn_Exp*e,struct
Cyc_Absyn_Stmt*s);static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*body_nv,
struct Cyc_Toc_Env*init_nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*s);
static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int
cinclude);struct Cyc_Absyn_Stmt*Cyc_Toc_make_npop_handler(int n);struct Cyc_Absyn_Stmt*
Cyc_Toc_make_npop_handler(int n){struct Cyc_List_List*_tmpB9E;return Cyc_Absyn_exp_stmt(
Cyc_Absyn_fncall_exp(Cyc_Toc__npop_handler_e,((_tmpB9E=_cycalloc(sizeof(*_tmpB9E)),((
_tmpB9E->hd=Cyc_Absyn_uint_exp((unsigned int)(n - 1),0),((_tmpB9E->tl=0,_tmpB9E)))))),
0),0);}void Cyc_Toc_do_npop_before(int n,struct Cyc_Absyn_Stmt*s);void Cyc_Toc_do_npop_before(
int n,struct Cyc_Absyn_Stmt*s){if(n > 0)s->r=Cyc_Toc_seq_stmt_r(Cyc_Toc_make_npop_handler(
n),Cyc_Absyn_new_stmt(s->r,0));}static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,
struct Cyc_Absyn_Stmt*s);static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,struct
Cyc_Absyn_Stmt*s){while(1){void*_tmp6D0=s->r;struct Cyc_Absyn_Exp*_tmp6D1;struct
Cyc_Absyn_Stmt*_tmp6D2;struct Cyc_Absyn_Stmt*_tmp6D3;struct Cyc_Absyn_Exp*_tmp6D4;
struct Cyc_Absyn_Exp*_tmp6D5;struct Cyc_Absyn_Stmt*_tmp6D6;struct Cyc_Absyn_Stmt*
_tmp6D7;struct _tuple9 _tmp6D8;struct Cyc_Absyn_Exp*_tmp6D9;struct Cyc_Absyn_Stmt*
_tmp6DA;struct Cyc_Absyn_Stmt*_tmp6DB;struct Cyc_Absyn_Stmt*_tmp6DC;struct Cyc_Absyn_Stmt*
_tmp6DD;struct Cyc_Absyn_Exp*_tmp6DE;struct _tuple9 _tmp6DF;struct Cyc_Absyn_Exp*
_tmp6E0;struct _tuple9 _tmp6E1;struct Cyc_Absyn_Exp*_tmp6E2;struct Cyc_Absyn_Stmt*
_tmp6E3;struct Cyc_Absyn_Exp*_tmp6E4;struct Cyc_List_List*_tmp6E5;struct Cyc_List_List*
_tmp6E6;struct Cyc_Absyn_Switch_clause**_tmp6E7;struct Cyc_Absyn_Decl*_tmp6E8;
struct Cyc_Absyn_Stmt*_tmp6E9;struct _dyneither_ptr*_tmp6EA;struct Cyc_Absyn_Stmt*
_tmp6EB;struct Cyc_Absyn_Stmt*_tmp6EC;struct _tuple9 _tmp6ED;struct Cyc_Absyn_Exp*
_tmp6EE;struct Cyc_Absyn_Stmt*_tmp6EF;struct Cyc_List_List*_tmp6F0;struct Cyc_Absyn_Exp*
_tmp6F1;_LL391: if((int)_tmp6D0 != 0)goto _LL393;_LL392: return;_LL393: if(_tmp6D0 <= (
void*)1)goto _LL395;if(*((int*)_tmp6D0)!= 0)goto _LL395;_tmp6D1=((struct Cyc_Absyn_Exp_s_struct*)
_tmp6D0)->f1;_LL394: Cyc_Toc_exp_to_c(nv,_tmp6D1);return;_LL395: if(_tmp6D0 <= (
void*)1)goto _LL397;if(*((int*)_tmp6D0)!= 1)goto _LL397;_tmp6D2=((struct Cyc_Absyn_Seq_s_struct*)
_tmp6D0)->f1;_tmp6D3=((struct Cyc_Absyn_Seq_s_struct*)_tmp6D0)->f2;_LL396: Cyc_Toc_stmt_to_c(
nv,_tmp6D2);s=_tmp6D3;continue;_LL397: if(_tmp6D0 <= (void*)1)goto _LL399;if(*((int*)
_tmp6D0)!= 2)goto _LL399;_tmp6D4=((struct Cyc_Absyn_Return_s_struct*)_tmp6D0)->f1;
_LL398: {struct Cyc_Core_Opt*topt=0;if(_tmp6D4 != 0){{struct Cyc_Core_Opt*_tmpB9F;
topt=((_tmpB9F=_cycalloc(sizeof(*_tmpB9F)),((_tmpB9F->v=(void*)Cyc_Toc_typ_to_c((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp6D4->topt))->v),_tmpB9F))));}Cyc_Toc_exp_to_c(
nv,(struct Cyc_Absyn_Exp*)_tmp6D4);}if(s->try_depth > 0){if(topt != 0){struct
_tuple1*_tmp6F3=Cyc_Toc_temp_var();struct Cyc_Absyn_Stmt*_tmp6F4=Cyc_Absyn_return_stmt((
struct Cyc_Absyn_Exp*)Cyc_Absyn_var_exp(_tmp6F3,0),0);s->r=(Cyc_Absyn_declare_stmt(
_tmp6F3,(void*)topt->v,_tmp6D4,Cyc_Absyn_seq_stmt(Cyc_Toc_make_npop_handler(s->try_depth),
_tmp6F4,0),0))->r;}else{Cyc_Toc_do_npop_before(s->try_depth,s);}}return;}_LL399:
if(_tmp6D0 <= (void*)1)goto _LL39B;if(*((int*)_tmp6D0)!= 3)goto _LL39B;_tmp6D5=((
struct Cyc_Absyn_IfThenElse_s_struct*)_tmp6D0)->f1;_tmp6D6=((struct Cyc_Absyn_IfThenElse_s_struct*)
_tmp6D0)->f2;_tmp6D7=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmp6D0)->f3;_LL39A:
Cyc_Toc_exp_to_c(nv,_tmp6D5);Cyc_Toc_stmt_to_c(nv,_tmp6D6);s=_tmp6D7;continue;
_LL39B: if(_tmp6D0 <= (void*)1)goto _LL39D;if(*((int*)_tmp6D0)!= 4)goto _LL39D;
_tmp6D8=((struct Cyc_Absyn_While_s_struct*)_tmp6D0)->f1;_tmp6D9=_tmp6D8.f1;
_tmp6DA=((struct Cyc_Absyn_While_s_struct*)_tmp6D0)->f2;_LL39C: Cyc_Toc_exp_to_c(
nv,_tmp6D9);{struct Cyc_Toc_Env _tmp6F6;struct _RegionHandle*_tmp6F7;struct Cyc_Toc_Env*
_tmp6F5=nv;_tmp6F6=*_tmp6F5;_tmp6F7=_tmp6F6.rgn;Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(
_tmp6F7,nv),_tmp6DA);return;}_LL39D: if(_tmp6D0 <= (void*)1)goto _LL39F;if(*((int*)
_tmp6D0)!= 5)goto _LL39F;_tmp6DB=((struct Cyc_Absyn_Break_s_struct*)_tmp6D0)->f1;
_LL39E: {struct Cyc_Toc_Env _tmp6F9;struct _dyneither_ptr**_tmp6FA;struct Cyc_Toc_Env*
_tmp6F8=nv;_tmp6F9=*_tmp6F8;_tmp6FA=_tmp6F9.break_lab;if(_tmp6FA != 0)s->r=Cyc_Toc_goto_stmt_r(*
_tmp6FA,0);{int dest_depth=_tmp6DB == 0?0: _tmp6DB->try_depth;Cyc_Toc_do_npop_before(
s->try_depth - dest_depth,s);return;}}_LL39F: if(_tmp6D0 <= (void*)1)goto _LL3A1;if(*((
int*)_tmp6D0)!= 6)goto _LL3A1;_tmp6DC=((struct Cyc_Absyn_Continue_s_struct*)
_tmp6D0)->f1;_LL3A0: {struct Cyc_Toc_Env _tmp6FC;struct _dyneither_ptr**_tmp6FD;
struct Cyc_Toc_Env*_tmp6FB=nv;_tmp6FC=*_tmp6FB;_tmp6FD=_tmp6FC.continue_lab;if(
_tmp6FD != 0)s->r=Cyc_Toc_goto_stmt_r(*_tmp6FD,0);_tmp6DD=_tmp6DC;goto _LL3A2;}
_LL3A1: if(_tmp6D0 <= (void*)1)goto _LL3A3;if(*((int*)_tmp6D0)!= 7)goto _LL3A3;
_tmp6DD=((struct Cyc_Absyn_Goto_s_struct*)_tmp6D0)->f2;_LL3A2: Cyc_Toc_do_npop_before(
s->try_depth - ((struct Cyc_Absyn_Stmt*)_check_null(_tmp6DD))->try_depth,s);
return;_LL3A3: if(_tmp6D0 <= (void*)1)goto _LL3A5;if(*((int*)_tmp6D0)!= 8)goto
_LL3A5;_tmp6DE=((struct Cyc_Absyn_For_s_struct*)_tmp6D0)->f1;_tmp6DF=((struct Cyc_Absyn_For_s_struct*)
_tmp6D0)->f2;_tmp6E0=_tmp6DF.f1;_tmp6E1=((struct Cyc_Absyn_For_s_struct*)_tmp6D0)->f3;
_tmp6E2=_tmp6E1.f1;_tmp6E3=((struct Cyc_Absyn_For_s_struct*)_tmp6D0)->f4;_LL3A4:
Cyc_Toc_exp_to_c(nv,_tmp6DE);Cyc_Toc_exp_to_c(nv,_tmp6E0);Cyc_Toc_exp_to_c(nv,
_tmp6E2);{struct Cyc_Toc_Env _tmp6FF;struct _RegionHandle*_tmp700;struct Cyc_Toc_Env*
_tmp6FE=nv;_tmp6FF=*_tmp6FE;_tmp700=_tmp6FF.rgn;Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(
_tmp700,nv),_tmp6E3);return;}_LL3A5: if(_tmp6D0 <= (void*)1)goto _LL3A7;if(*((int*)
_tmp6D0)!= 9)goto _LL3A7;_tmp6E4=((struct Cyc_Absyn_Switch_s_struct*)_tmp6D0)->f1;
_tmp6E5=((struct Cyc_Absyn_Switch_s_struct*)_tmp6D0)->f2;_LL3A6: Cyc_Toc_xlate_switch(
nv,s,_tmp6E4,_tmp6E5);return;_LL3A7: if(_tmp6D0 <= (void*)1)goto _LL3A9;if(*((int*)
_tmp6D0)!= 10)goto _LL3A9;_tmp6E6=((struct Cyc_Absyn_Fallthru_s_struct*)_tmp6D0)->f1;
_tmp6E7=((struct Cyc_Absyn_Fallthru_s_struct*)_tmp6D0)->f2;_LL3A8: {struct Cyc_Toc_Env
_tmp702;struct Cyc_Toc_FallthruInfo*_tmp703;struct Cyc_Toc_Env*_tmp701=nv;_tmp702=*
_tmp701;_tmp703=_tmp702.fallthru_info;if(_tmp703 == 0){const char*_tmpBA2;void*
_tmpBA1;(_tmpBA1=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpBA2="fallthru in unexpected place",_tag_dyneither(_tmpBA2,sizeof(char),29))),
_tag_dyneither(_tmpBA1,sizeof(void*),0)));}{struct _dyneither_ptr*_tmp707;struct
Cyc_List_List*_tmp708;struct Cyc_Dict_Dict _tmp709;struct Cyc_Toc_FallthruInfo
_tmp706=*_tmp703;_tmp707=_tmp706.label;_tmp708=_tmp706.binders;_tmp709=_tmp706.next_case_env;{
struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_goto_stmt(_tmp707,0);Cyc_Toc_do_npop_before(s->try_depth
- ((*((struct Cyc_Absyn_Switch_clause**)_check_null(_tmp6E7)))->body)->try_depth,
s2);{struct Cyc_List_List*_tmp70A=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_rev)(_tmp708);struct Cyc_List_List*_tmp70B=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(_tmp6E6);for(0;_tmp70A != 0;(_tmp70A=_tmp70A->tl,
_tmp70B=_tmp70B->tl)){Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp70B))->hd);s2=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(((struct
Cyc_Absyn_Exp*(*)(struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup)(_tmp709,(
struct _tuple1*)_tmp70A->hd),(struct Cyc_Absyn_Exp*)_tmp70B->hd,0),s2,0);}s->r=s2->r;
return;}}}}_LL3A9: if(_tmp6D0 <= (void*)1)goto _LL3AB;if(*((int*)_tmp6D0)!= 11)goto
_LL3AB;_tmp6E8=((struct Cyc_Absyn_Decl_s_struct*)_tmp6D0)->f1;_tmp6E9=((struct Cyc_Absyn_Decl_s_struct*)
_tmp6D0)->f2;_LL3AA:{void*_tmp70C=_tmp6E8->r;struct Cyc_Absyn_Vardecl*_tmp70D;
struct Cyc_Absyn_Pat*_tmp70E;struct Cyc_Absyn_Exp*_tmp70F;struct Cyc_List_List*
_tmp710;struct Cyc_Absyn_Fndecl*_tmp711;struct Cyc_Absyn_Tvar*_tmp712;struct Cyc_Absyn_Vardecl*
_tmp713;int _tmp714;struct Cyc_Absyn_Exp*_tmp715;struct Cyc_Absyn_Exp*_tmp716;
struct Cyc_Absyn_Tvar*_tmp717;struct Cyc_Absyn_Vardecl*_tmp718;_LL3B4: if(_tmp70C <= (
void*)2)goto _LL3C0;if(*((int*)_tmp70C)!= 0)goto _LL3B6;_tmp70D=((struct Cyc_Absyn_Var_d_struct*)
_tmp70C)->f1;_LL3B5: {struct Cyc_Toc_Env _tmp71A;struct _RegionHandle*_tmp71B;
struct Cyc_Toc_Env*_tmp719=nv;_tmp71A=*_tmp719;_tmp71B=_tmp71A.rgn;{struct Cyc_Absyn_Local_b_struct
_tmpBA5;struct Cyc_Absyn_Local_b_struct*_tmpBA4;struct Cyc_Toc_Env*_tmp71C=Cyc_Toc_add_varmap(
_tmp71B,nv,_tmp70D->name,Cyc_Absyn_varb_exp(_tmp70D->name,(void*)((_tmpBA4=
_cycalloc(sizeof(*_tmpBA4)),((_tmpBA4[0]=((_tmpBA5.tag=3,((_tmpBA5.f1=_tmp70D,
_tmpBA5)))),_tmpBA4)))),0));Cyc_Toc_local_decl_to_c(_tmp71C,_tmp71C,_tmp70D,
_tmp6E9);}goto _LL3B3;}_LL3B6: if(*((int*)_tmp70C)!= 2)goto _LL3B8;_tmp70E=((struct
Cyc_Absyn_Let_d_struct*)_tmp70C)->f1;_tmp70F=((struct Cyc_Absyn_Let_d_struct*)
_tmp70C)->f3;_LL3B7:{void*_tmp71F=_tmp70E->r;struct Cyc_Absyn_Vardecl*_tmp720;
struct Cyc_Absyn_Pat*_tmp721;struct Cyc_Absyn_Pat _tmp722;void*_tmp723;_LL3C3: if(
_tmp71F <= (void*)2)goto _LL3C5;if(*((int*)_tmp71F)!= 0)goto _LL3C5;_tmp720=((
struct Cyc_Absyn_Var_p_struct*)_tmp71F)->f1;_tmp721=((struct Cyc_Absyn_Var_p_struct*)
_tmp71F)->f2;_tmp722=*_tmp721;_tmp723=_tmp722.r;if((int)_tmp723 != 0)goto _LL3C5;
_LL3C4: {struct _tuple1*old_name=_tmp720->name;struct _tuple1*new_name=Cyc_Toc_temp_var();
_tmp720->name=new_name;_tmp720->initializer=(struct Cyc_Absyn_Exp*)_tmp70F;{
struct Cyc_Absyn_Var_d_struct _tmpBA8;struct Cyc_Absyn_Var_d_struct*_tmpBA7;_tmp6E8->r=(
void*)((_tmpBA7=_cycalloc(sizeof(*_tmpBA7)),((_tmpBA7[0]=((_tmpBA8.tag=0,((
_tmpBA8.f1=_tmp720,_tmpBA8)))),_tmpBA7))));}{struct Cyc_Toc_Env _tmp727;struct
_RegionHandle*_tmp728;struct Cyc_Toc_Env*_tmp726=nv;_tmp727=*_tmp726;_tmp728=
_tmp727.rgn;{struct Cyc_Absyn_Local_b_struct _tmpBAB;struct Cyc_Absyn_Local_b_struct*
_tmpBAA;struct Cyc_Toc_Env*_tmp729=Cyc_Toc_add_varmap(_tmp728,nv,old_name,Cyc_Absyn_varb_exp(
new_name,(void*)((_tmpBAA=_cycalloc(sizeof(*_tmpBAA)),((_tmpBAA[0]=((_tmpBAB.tag=
3,((_tmpBAB.f1=_tmp720,_tmpBAB)))),_tmpBAA)))),0));Cyc_Toc_local_decl_to_c(
_tmp729,nv,_tmp720,_tmp6E9);}goto _LL3C2;}}_LL3C5:;_LL3C6: s->r=(Cyc_Toc_letdecl_to_c(
nv,_tmp70E,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp70F->topt))->v,_tmp70F,
_tmp6E9))->r;goto _LL3C2;_LL3C2:;}goto _LL3B3;_LL3B8: if(*((int*)_tmp70C)!= 3)goto
_LL3BA;_tmp710=((struct Cyc_Absyn_Letv_d_struct*)_tmp70C)->f1;_LL3B9: {struct Cyc_List_List*
_tmp72C=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp710);
if(_tmp72C == 0){const char*_tmpBAE;void*_tmpBAD;(_tmpBAD=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpBAE="empty Letv_d",
_tag_dyneither(_tmpBAE,sizeof(char),13))),_tag_dyneither(_tmpBAD,sizeof(void*),0)));}{
struct Cyc_Absyn_Var_d_struct _tmpBB1;struct Cyc_Absyn_Var_d_struct*_tmpBB0;_tmp6E8->r=(
void*)((_tmpBB0=_cycalloc(sizeof(*_tmpBB0)),((_tmpBB0[0]=((_tmpBB1.tag=0,((
_tmpBB1.f1=(struct Cyc_Absyn_Vardecl*)_tmp72C->hd,_tmpBB1)))),_tmpBB0))));}
_tmp72C=_tmp72C->tl;for(0;_tmp72C != 0;_tmp72C=_tmp72C->tl){struct Cyc_Absyn_Var_d_struct
_tmpBB4;struct Cyc_Absyn_Var_d_struct*_tmpBB3;struct Cyc_Absyn_Decl*_tmp731=Cyc_Absyn_new_decl((
void*)((_tmpBB3=_cycalloc(sizeof(*_tmpBB3)),((_tmpBB3[0]=((_tmpBB4.tag=0,((
_tmpBB4.f1=(struct Cyc_Absyn_Vardecl*)_tmp72C->hd,_tmpBB4)))),_tmpBB3)))),0);s->r=(
Cyc_Absyn_decl_stmt(_tmp731,Cyc_Absyn_new_stmt(s->r,0),0))->r;}Cyc_Toc_stmt_to_c(
nv,s);goto _LL3B3;}_LL3BA: if(*((int*)_tmp70C)!= 1)goto _LL3BC;_tmp711=((struct Cyc_Absyn_Fn_d_struct*)
_tmp70C)->f1;_LL3BB: {struct _tuple1*_tmp734=_tmp711->name;struct Cyc_Toc_Env
_tmp736;struct _RegionHandle*_tmp737;struct Cyc_Toc_Env*_tmp735=nv;_tmp736=*
_tmp735;_tmp737=_tmp736.rgn;{struct Cyc_Toc_Env*_tmp738=Cyc_Toc_add_varmap(
_tmp737,nv,_tmp711->name,Cyc_Absyn_var_exp(_tmp734,0));Cyc_Toc_fndecl_to_c(
_tmp738,_tmp711,0);Cyc_Toc_stmt_to_c(_tmp738,_tmp6E9);}goto _LL3B3;}_LL3BC: if(*((
int*)_tmp70C)!= 4)goto _LL3BE;_tmp712=((struct Cyc_Absyn_Region_d_struct*)_tmp70C)->f1;
_tmp713=((struct Cyc_Absyn_Region_d_struct*)_tmp70C)->f2;_tmp714=((struct Cyc_Absyn_Region_d_struct*)
_tmp70C)->f3;_tmp715=((struct Cyc_Absyn_Region_d_struct*)_tmp70C)->f4;_LL3BD: {
struct Cyc_Absyn_Stmt*_tmp739=_tmp6E9;void*rh_struct_typ=Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp);
void*rh_struct_ptr_typ=Cyc_Absyn_cstar_typ(rh_struct_typ,Cyc_Toc_mt_tq);struct
_tuple1*rh_var=Cyc_Toc_temp_var();struct _tuple1*x_var=_tmp713->name;struct Cyc_Absyn_Exp*
rh_exp=Cyc_Absyn_var_exp(rh_var,0);struct Cyc_Absyn_Exp*x_exp=Cyc_Absyn_var_exp(
x_var,0);struct Cyc_Toc_Env _tmp73B;struct _RegionHandle*_tmp73C;struct Cyc_Toc_Env*
_tmp73A=nv;_tmp73B=*_tmp73A;_tmp73C=_tmp73B.rgn;Cyc_Toc_stmt_to_c(Cyc_Toc_add_varmap(
_tmp73C,nv,x_var,x_exp),_tmp739);if(Cyc_Absyn_no_regions)s->r=(Cyc_Absyn_declare_stmt(
x_var,rh_struct_ptr_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(0,0),_tmp739,0))->r;
else{if(_tmp715 == 0){struct Cyc_Absyn_Exp*_tmpBB7[1];struct Cyc_Absyn_Exp*_tmpBB6[
1];struct Cyc_List_List*_tmpBB5;s->r=(Cyc_Absyn_declare_stmt(rh_var,rh_struct_typ,(
struct Cyc_Absyn_Exp*)Cyc_Absyn_fncall_exp(Cyc_Toc__new_region_e,((_tmpBB5=
_cycalloc(sizeof(*_tmpBB5)),((_tmpBB5->hd=Cyc_Absyn_string_exp(Cyc_Absynpp_qvar2string(
x_var),0),((_tmpBB5->tl=0,_tmpBB5)))))),0),Cyc_Absyn_declare_stmt(x_var,
rh_struct_ptr_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(rh_exp,0),Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__push_region_e,((_tmpBB6[0]=
x_exp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpBB6,sizeof(struct Cyc_Absyn_Exp*),1)))),0),0),Cyc_Absyn_seq_stmt(
_tmp739,Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_region_e,((_tmpBB7[
0]=x_exp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpBB7,sizeof(struct Cyc_Absyn_Exp*),1)))),0),0),0),0),0),0))->r;}
else{Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp715);{struct Cyc_Absyn_Exp*
_tmpBB9[1];struct Cyc_Absyn_Exp*_tmpBB8[2];s->r=(Cyc_Absyn_declare_stmt(rh_var,
Cyc_Absyn_strct(Cyc_Toc__DynRegionFrame_sp),0,Cyc_Absyn_declare_stmt(x_var,
rh_struct_ptr_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_fncall_exp(Cyc_Toc__open_dynregion_e,((
_tmpBB8[1]=(struct Cyc_Absyn_Exp*)_tmp715,((_tmpBB8[0]=Cyc_Absyn_address_exp(
rh_exp,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpBB8,sizeof(struct Cyc_Absyn_Exp*),2)))))),0),Cyc_Absyn_seq_stmt(
_tmp739,Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_dynregion_e,((
_tmpBB9[0]=x_exp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpBB9,sizeof(struct Cyc_Absyn_Exp*),1)))),0),0),0),0),0))->r;}}}
return;}_LL3BE: if(*((int*)_tmp70C)!= 5)goto _LL3C0;_tmp716=((struct Cyc_Absyn_Alias_d_struct*)
_tmp70C)->f1;_tmp717=((struct Cyc_Absyn_Alias_d_struct*)_tmp70C)->f2;_tmp718=((
struct Cyc_Absyn_Alias_d_struct*)_tmp70C)->f3;_LL3BF: {struct _tuple1*old_name=
_tmp718->name;struct _tuple1*new_name=Cyc_Toc_temp_var();_tmp718->name=new_name;
_tmp718->initializer=(struct Cyc_Absyn_Exp*)_tmp716;{struct Cyc_Absyn_Decl_s_struct
_tmpBC8;struct Cyc_Absyn_Var_d_struct*_tmpBC7;struct Cyc_Absyn_Var_d_struct _tmpBC6;
struct Cyc_Absyn_Decl*_tmpBC5;struct Cyc_Absyn_Decl_s_struct*_tmpBC4;s->r=(void*)((
_tmpBC4=_cycalloc(sizeof(*_tmpBC4)),((_tmpBC4[0]=((_tmpBC8.tag=11,((_tmpBC8.f1=((
_tmpBC5=_cycalloc(sizeof(*_tmpBC5)),((_tmpBC5->r=(void*)((_tmpBC7=_cycalloc(
sizeof(*_tmpBC7)),((_tmpBC7[0]=((_tmpBC6.tag=0,((_tmpBC6.f1=_tmp718,_tmpBC6)))),
_tmpBC7)))),((_tmpBC5->loc=0,_tmpBC5)))))),((_tmpBC8.f2=_tmp6E9,_tmpBC8)))))),
_tmpBC4))));}{struct Cyc_Toc_Env _tmp748;struct _RegionHandle*_tmp749;struct Cyc_Toc_Env*
_tmp747=nv;_tmp748=*_tmp747;_tmp749=_tmp748.rgn;{struct Cyc_Absyn_Local_b_struct
_tmpBCB;struct Cyc_Absyn_Local_b_struct*_tmpBCA;struct Cyc_Toc_Env*_tmp74A=Cyc_Toc_add_varmap(
_tmp749,nv,old_name,Cyc_Absyn_varb_exp(new_name,(void*)((_tmpBCA=_cycalloc(
sizeof(*_tmpBCA)),((_tmpBCA[0]=((_tmpBCB.tag=3,((_tmpBCB.f1=_tmp718,_tmpBCB)))),
_tmpBCA)))),0));Cyc_Toc_local_decl_to_c(_tmp74A,nv,_tmp718,_tmp6E9);}return;}}
_LL3C0:;_LL3C1: {const char*_tmpBCE;void*_tmpBCD;(_tmpBCD=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpBCE="bad nested declaration within function",
_tag_dyneither(_tmpBCE,sizeof(char),39))),_tag_dyneither(_tmpBCD,sizeof(void*),0)));}
_LL3B3:;}return;_LL3AB: if(_tmp6D0 <= (void*)1)goto _LL3AD;if(*((int*)_tmp6D0)!= 12)
goto _LL3AD;_tmp6EA=((struct Cyc_Absyn_Label_s_struct*)_tmp6D0)->f1;_tmp6EB=((
struct Cyc_Absyn_Label_s_struct*)_tmp6D0)->f2;_LL3AC: s=_tmp6EB;continue;_LL3AD:
if(_tmp6D0 <= (void*)1)goto _LL3AF;if(*((int*)_tmp6D0)!= 13)goto _LL3AF;_tmp6EC=((
struct Cyc_Absyn_Do_s_struct*)_tmp6D0)->f1;_tmp6ED=((struct Cyc_Absyn_Do_s_struct*)
_tmp6D0)->f2;_tmp6EE=_tmp6ED.f1;_LL3AE: {struct Cyc_Toc_Env _tmp750;struct
_RegionHandle*_tmp751;struct Cyc_Toc_Env*_tmp74F=nv;_tmp750=*_tmp74F;_tmp751=
_tmp750.rgn;Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(_tmp751,nv),_tmp6EC);Cyc_Toc_exp_to_c(
nv,_tmp6EE);return;}_LL3AF: if(_tmp6D0 <= (void*)1)goto _LL3B1;if(*((int*)_tmp6D0)
!= 14)goto _LL3B1;_tmp6EF=((struct Cyc_Absyn_TryCatch_s_struct*)_tmp6D0)->f1;
_tmp6F0=((struct Cyc_Absyn_TryCatch_s_struct*)_tmp6D0)->f2;_LL3B0: {struct _tuple1*
h_var=Cyc_Toc_temp_var();struct _tuple1*e_var=Cyc_Toc_temp_var();struct _tuple1*
was_thrown_var=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*h_exp=Cyc_Absyn_var_exp(
h_var,0);struct Cyc_Absyn_Exp*e_exp=Cyc_Absyn_var_exp(e_var,0);struct Cyc_Absyn_Exp*
was_thrown_exp=Cyc_Absyn_var_exp(was_thrown_var,0);void*h_typ=Cyc_Absyn_strct(
Cyc_Toc__handler_cons_sp);void*e_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_exn_typ);void*
was_thrown_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_sint_typ);{struct Cyc_Core_Opt*_tmpBCF;
e_exp->topt=((_tmpBCF=_cycalloc(sizeof(*_tmpBCF)),((_tmpBCF->v=(void*)e_typ,
_tmpBCF))));}{struct Cyc_Toc_Env _tmp754;struct _RegionHandle*_tmp755;struct Cyc_Toc_Env*
_tmp753=nv;_tmp754=*_tmp753;_tmp755=_tmp754.rgn;{struct Cyc_Toc_Env*_tmp756=Cyc_Toc_add_varmap(
_tmp755,nv,e_var,e_exp);Cyc_Toc_stmt_to_c(_tmp756,_tmp6EF);{struct Cyc_Absyn_Stmt*
_tmp757=Cyc_Absyn_seq_stmt(_tmp6EF,Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_handler_e,
0,0),0),0);struct _tuple1*_tmp758=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp759=
Cyc_Absyn_var_exp(_tmp758,0);struct Cyc_Absyn_Vardecl*_tmp75A=Cyc_Absyn_new_vardecl(
_tmp758,Cyc_Absyn_exn_typ,0);{struct Cyc_Core_Opt*_tmpBD0;_tmp759->topt=((_tmpBD0=
_cycalloc(sizeof(*_tmpBD0)),((_tmpBD0->v=(void*)Cyc_Absyn_exn_typ,_tmpBD0))));}{
struct Cyc_Absyn_Var_p_struct*_tmpBE1;struct Cyc_Absyn_Pat*_tmpBE0;struct Cyc_Core_Opt*
_tmpBDF;struct Cyc_Absyn_Var_p_struct _tmpBDE;struct Cyc_Core_Opt*_tmpBDD;struct Cyc_Absyn_Pat*
_tmpBDC;struct Cyc_Absyn_Pat*_tmp75C=(_tmpBDC=_cycalloc(sizeof(*_tmpBDC)),((
_tmpBDC->r=(void*)((_tmpBE1=_cycalloc(sizeof(*_tmpBE1)),((_tmpBE1[0]=((_tmpBDE.tag=
0,((_tmpBDE.f1=_tmp75A,((_tmpBDE.f2=((_tmpBE0=_cycalloc(sizeof(*_tmpBE0)),((
_tmpBE0->r=(void*)0,((_tmpBE0->topt=((_tmpBDF=_cycalloc(sizeof(*_tmpBDF)),((
_tmpBDF->v=(void*)Cyc_Absyn_exn_typ,_tmpBDF)))),((_tmpBE0->loc=0,_tmpBE0)))))))),
_tmpBDE)))))),_tmpBE1)))),((_tmpBDC->topt=((_tmpBDD=_cycalloc(sizeof(*_tmpBDD)),((
_tmpBDD->v=(void*)Cyc_Absyn_exn_typ,_tmpBDD)))),((_tmpBDC->loc=0,_tmpBDC)))))));
struct Cyc_Absyn_Exp*_tmp75D=Cyc_Absyn_throw_exp(_tmp759,0);{struct Cyc_Core_Opt*
_tmpBE2;_tmp75D->topt=((_tmpBE2=_cycalloc(sizeof(*_tmpBE2)),((_tmpBE2->v=(void*)((
void*)0),_tmpBE2))));}{struct Cyc_Absyn_Stmt*_tmp75F=Cyc_Absyn_exp_stmt(_tmp75D,0);
struct Cyc_Core_Opt*_tmpBE8;struct Cyc_List_List*_tmpBE7;struct Cyc_Absyn_Switch_clause*
_tmpBE6;struct Cyc_Absyn_Switch_clause*_tmp760=(_tmpBE6=_cycalloc(sizeof(*_tmpBE6)),((
_tmpBE6->pattern=_tmp75C,((_tmpBE6->pat_vars=((_tmpBE8=_cycalloc(sizeof(*_tmpBE8)),((
_tmpBE8->v=((_tmpBE7=_cycalloc(sizeof(*_tmpBE7)),((_tmpBE7->hd=_tmp75A,((_tmpBE7->tl=
0,_tmpBE7)))))),_tmpBE8)))),((_tmpBE6->where_clause=0,((_tmpBE6->body=_tmp75F,((
_tmpBE6->loc=0,_tmpBE6)))))))))));struct Cyc_List_List*_tmpBE9;struct Cyc_Absyn_Stmt*
_tmp761=Cyc_Absyn_switch_stmt(e_exp,((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_append)(_tmp6F0,((_tmpBE9=_cycalloc(sizeof(*
_tmpBE9)),((_tmpBE9->hd=_tmp760,((_tmpBE9->tl=0,_tmpBE9))))))),0);Cyc_Toc_stmt_to_c(
_tmp756,_tmp761);{struct Cyc_List_List*_tmpBEA;struct Cyc_Absyn_Exp*_tmp762=Cyc_Absyn_fncall_exp(
Cyc_Toc_setjmp_e,((_tmpBEA=_cycalloc(sizeof(*_tmpBEA)),((_tmpBEA->hd=Cyc_Absyn_aggrmember_exp(
h_exp,Cyc_Toc_handler_sp,0),((_tmpBEA->tl=0,_tmpBEA)))))),0);struct Cyc_List_List*
_tmpBEB;struct Cyc_Absyn_Stmt*_tmp763=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__push_handler_e,((
_tmpBEB=_cycalloc(sizeof(*_tmpBEB)),((_tmpBEB->hd=Cyc_Absyn_address_exp(h_exp,0),((
_tmpBEB->tl=0,_tmpBEB)))))),0),0);struct Cyc_Absyn_Exp*_tmp764=Cyc_Absyn_int_exp((
void*)0,0,0);struct Cyc_Absyn_Exp*_tmp765=Cyc_Absyn_int_exp((void*)0,1,0);s->r=(
Cyc_Absyn_declare_stmt(h_var,h_typ,0,Cyc_Absyn_seq_stmt(_tmp763,Cyc_Absyn_declare_stmt(
was_thrown_var,was_thrown_typ,(struct Cyc_Absyn_Exp*)_tmp764,Cyc_Absyn_seq_stmt(
Cyc_Absyn_ifthenelse_stmt(_tmp762,Cyc_Absyn_assign_stmt(was_thrown_exp,_tmp765,0),
Cyc_Toc_skip_stmt_dl(),0),Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_prim1_exp((void*)
11,was_thrown_exp,0),_tmp757,Cyc_Absyn_declare_stmt(e_var,e_typ,(struct Cyc_Absyn_Exp*)
Cyc_Toc_cast_it(e_typ,Cyc_Toc__exn_thrown_e),_tmp761,0),0),0),0),0),0))->r;}}}}}
return;}}_LL3B1: if(_tmp6D0 <= (void*)1)goto _LL390;if(*((int*)_tmp6D0)!= 15)goto
_LL390;_tmp6F1=((struct Cyc_Absyn_ResetRegion_s_struct*)_tmp6D0)->f1;_LL3B2: if(
Cyc_Absyn_no_regions)s->r=(void*)0;else{Cyc_Toc_exp_to_c(nv,_tmp6F1);{struct Cyc_List_List*
_tmpBEC;s->r=Cyc_Toc_exp_stmt_r(Cyc_Absyn_fncall_exp(Cyc_Toc__reset_region_e,((
_tmpBEC=_cycalloc(sizeof(*_tmpBEC)),((_tmpBEC->hd=_tmp6F1,((_tmpBEC->tl=0,
_tmpBEC)))))),0));}}return;_LL390:;}}static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*
s);struct _tuple25{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};
static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int
cinclude);static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*
f,int cinclude){f->tvs=0;f->effect=0;f->rgn_po=0;f->ret_type=Cyc_Toc_typ_to_c(f->ret_type);{
struct _RegionHandle _tmp773=_new_region("frgn");struct _RegionHandle*frgn=& _tmp773;
_push_region(frgn);{struct Cyc_Toc_Env*_tmp774=Cyc_Toc_share_env(frgn,nv);{struct
Cyc_List_List*_tmp775=f->args;for(0;_tmp775 != 0;_tmp775=_tmp775->tl){struct
_tuple1*_tmpBED;struct _tuple1*_tmp776=(_tmpBED=_cycalloc(sizeof(*_tmpBED)),((
_tmpBED->f1=(union Cyc_Absyn_Nmspace)Cyc_Absyn_Loc_n,((_tmpBED->f2=(*((struct
_tuple25*)_tmp775->hd)).f1,_tmpBED)))));(*((struct _tuple25*)_tmp775->hd)).f3=Cyc_Toc_typ_to_c((*((
struct _tuple25*)_tmp775->hd)).f3);_tmp774=Cyc_Toc_add_varmap(frgn,_tmp774,
_tmp776,Cyc_Absyn_var_exp(_tmp776,0));}}if(cinclude){Cyc_Toc_stmttypes_to_c(f->body);
_npop_handler(0);return;}if((unsigned int)f->cyc_varargs  && ((struct Cyc_Absyn_VarargInfo*)
_check_null(f->cyc_varargs))->name != 0){struct Cyc_Core_Opt*_tmp779;struct Cyc_Absyn_Tqual
_tmp77A;void*_tmp77B;int _tmp77C;struct Cyc_Absyn_VarargInfo _tmp778=*((struct Cyc_Absyn_VarargInfo*)
_check_null(f->cyc_varargs));_tmp779=_tmp778.name;_tmp77A=_tmp778.tq;_tmp77B=
_tmp778.type;_tmp77C=_tmp778.inject;{void*_tmp77D=Cyc_Toc_typ_to_c(Cyc_Absyn_dyneither_typ(
_tmp77B,(void*)2,_tmp77A,Cyc_Absyn_false_conref));struct _tuple1*_tmpBEE;struct
_tuple1*_tmp77E=(_tmpBEE=_cycalloc(sizeof(*_tmpBEE)),((_tmpBEE->f1=(union Cyc_Absyn_Nmspace)
Cyc_Absyn_Loc_n,((_tmpBEE->f2=(struct _dyneither_ptr*)((struct Cyc_Core_Opt*)
_check_null(_tmp779))->v,_tmpBEE)))));{struct _tuple25*_tmpBF1;struct Cyc_List_List*
_tmpBF0;f->args=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_append)(f->args,((_tmpBF0=_cycalloc(sizeof(*_tmpBF0)),((_tmpBF0->hd=((
_tmpBF1=_cycalloc(sizeof(*_tmpBF1)),((_tmpBF1->f1=(struct _dyneither_ptr*)_tmp779->v,((
_tmpBF1->f2=_tmp77A,((_tmpBF1->f3=_tmp77D,_tmpBF1)))))))),((_tmpBF0->tl=0,
_tmpBF0)))))));}_tmp774=Cyc_Toc_add_varmap(frgn,_tmp774,_tmp77E,Cyc_Absyn_var_exp(
_tmp77E,0));f->cyc_varargs=0;}}{struct Cyc_List_List*_tmp782=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(f->param_vardecls))->v;for(0;_tmp782 != 0;_tmp782=
_tmp782->tl){((struct Cyc_Absyn_Vardecl*)_tmp782->hd)->type=Cyc_Toc_typ_to_c(((
struct Cyc_Absyn_Vardecl*)_tmp782->hd)->type);}}Cyc_Toc_stmt_to_c(Cyc_Toc_clear_toplevel(
frgn,_tmp774),f->body);};_pop_region(frgn);}}static void*Cyc_Toc_scope_to_c(void*
s);static void*Cyc_Toc_scope_to_c(void*s){void*_tmp783=s;_LL3C8: if((int)_tmp783 != 
1)goto _LL3CA;_LL3C9: return(void*)2;_LL3CA: if((int)_tmp783 != 4)goto _LL3CC;_LL3CB:
return(void*)3;_LL3CC:;_LL3CD: return s;_LL3C7:;}static void Cyc_Toc_aggrdecl_to_c(
struct Cyc_Absyn_Aggrdecl*ad);static void Cyc_Toc_aggrdecl_to_c(struct Cyc_Absyn_Aggrdecl*
ad){struct _tuple1*_tmp784=ad->name;struct _DynRegionHandle*_tmp786;struct Cyc_Dict_Dict*
_tmp787;struct Cyc_Toc_TocState _tmp785=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));
_tmp786=_tmp785.dyn;_tmp787=_tmp785.aggrs_so_far;{struct _DynRegionFrame _tmp788;
struct _RegionHandle*d=_open_dynregion(& _tmp788,_tmp786);{int seen_defn_before;
struct _tuple15**_tmp789=((struct _tuple15**(*)(struct Cyc_Dict_Dict d,struct _tuple1*
k))Cyc_Dict_lookup_opt)(*_tmp787,_tmp784);if(_tmp789 == 0){seen_defn_before=0;{
struct _tuple15*v;if(ad->kind == (void*)0){struct _tuple15*_tmpBF2;v=((_tmpBF2=
_region_malloc(d,sizeof(*_tmpBF2)),((_tmpBF2->f1=ad,((_tmpBF2->f2=Cyc_Absyn_strctq(
_tmp784),_tmpBF2))))));}else{struct _tuple15*_tmpBF3;v=((_tmpBF3=_region_malloc(d,
sizeof(*_tmpBF3)),((_tmpBF3->f1=ad,((_tmpBF3->f2=Cyc_Absyn_unionq_typ(_tmp784),
_tmpBF3))))));}*_tmp787=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct
_tuple1*k,struct _tuple15*v))Cyc_Dict_insert)(*_tmp787,_tmp784,v);}}else{struct
_tuple15 _tmp78D;struct Cyc_Absyn_Aggrdecl*_tmp78E;void*_tmp78F;struct _tuple15*
_tmp78C=*_tmp789;_tmp78D=*_tmp78C;_tmp78E=_tmp78D.f1;_tmp78F=_tmp78D.f2;if(
_tmp78E->impl == 0){{struct _tuple15*_tmpBF4;*_tmp787=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,struct _tuple1*k,struct _tuple15*v))Cyc_Dict_insert)(*_tmp787,
_tmp784,((_tmpBF4=_region_malloc(d,sizeof(*_tmpBF4)),((_tmpBF4->f1=ad,((_tmpBF4->f2=
_tmp78F,_tmpBF4)))))));}seen_defn_before=0;}else{seen_defn_before=1;}}{struct Cyc_Absyn_Aggrdecl*
_tmpBF5;struct Cyc_Absyn_Aggrdecl*new_ad=(_tmpBF5=_cycalloc(sizeof(*_tmpBF5)),((
_tmpBF5->kind=ad->kind,((_tmpBF5->sc=(void*)2,((_tmpBF5->name=ad->name,((_tmpBF5->tvs=
0,((_tmpBF5->impl=0,((_tmpBF5->attributes=ad->attributes,_tmpBF5)))))))))))));
if(ad->impl != 0  && !seen_defn_before){{struct Cyc_Absyn_AggrdeclImpl*_tmpBF6;
new_ad->impl=((_tmpBF6=_cycalloc(sizeof(*_tmpBF6)),((_tmpBF6->exist_vars=0,((
_tmpBF6->rgn_po=0,((_tmpBF6->fields=0,((_tmpBF6->tagged=0,_tmpBF6))))))))));}{
struct Cyc_List_List*new_fields=0;{struct Cyc_List_List*_tmp792=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(ad->impl))->fields;for(0;_tmp792 != 0;_tmp792=_tmp792->tl){struct Cyc_Absyn_Aggrfield*
_tmp793=(struct Cyc_Absyn_Aggrfield*)_tmp792->hd;struct Cyc_Absyn_Aggrfield*
_tmpBF7;struct Cyc_Absyn_Aggrfield*_tmp794=(_tmpBF7=_cycalloc(sizeof(*_tmpBF7)),((
_tmpBF7->name=_tmp793->name,((_tmpBF7->tq=Cyc_Toc_mt_tq,((_tmpBF7->type=Cyc_Toc_typ_to_c_array(
_tmp793->type),((_tmpBF7->width=_tmp793->width,((_tmpBF7->attributes=_tmp793->attributes,
_tmpBF7)))))))))));if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged){
void*_tmp795=_tmp794->type;struct _dyneither_ptr*_tmp796=_tmp794->name;const char*
_tmpBFC;void*_tmpBFB[2];struct Cyc_String_pa_struct _tmpBFA;struct Cyc_String_pa_struct
_tmpBF9;struct _dyneither_ptr s=(struct _dyneither_ptr)((_tmpBF9.tag=0,((_tmpBF9.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp796),((_tmpBFA.tag=0,((_tmpBFA.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*(*ad->name).f2),((_tmpBFB[0]=&
_tmpBFA,((_tmpBFB[1]=& _tmpBF9,Cyc_aprintf(((_tmpBFC="_union_%s_%s",
_tag_dyneither(_tmpBFC,sizeof(char),13))),_tag_dyneither(_tmpBFB,sizeof(void*),2))))))))))))));
struct _dyneither_ptr*_tmpBFD;struct _dyneither_ptr*str=(_tmpBFD=_cycalloc(sizeof(*
_tmpBFD)),((_tmpBFD[0]=s,_tmpBFD)));struct Cyc_Absyn_Aggrfield*_tmpBFE;struct Cyc_Absyn_Aggrfield*
_tmp797=(_tmpBFE=_cycalloc(sizeof(*_tmpBFE)),((_tmpBFE->name=Cyc_Toc_val_sp,((
_tmpBFE->tq=Cyc_Toc_mt_tq,((_tmpBFE->type=_tmp795,((_tmpBFE->width=0,((_tmpBFE->attributes=
0,_tmpBFE)))))))))));struct Cyc_Absyn_Aggrfield*_tmpBFF;struct Cyc_Absyn_Aggrfield*
_tmp798=(_tmpBFF=_cycalloc(sizeof(*_tmpBFF)),((_tmpBFF->name=Cyc_Toc_tag_sp,((
_tmpBFF->tq=Cyc_Toc_mt_tq,((_tmpBFF->type=Cyc_Absyn_sint_typ,((_tmpBFF->width=0,((
_tmpBFF->attributes=0,_tmpBFF)))))))))));struct Cyc_Absyn_Aggrfield*_tmpC00[2];
struct Cyc_List_List*_tmp799=(_tmpC00[1]=_tmp797,((_tmpC00[0]=_tmp798,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC00,sizeof(struct Cyc_Absyn_Aggrfield*),
2)))));struct _tuple1*_tmpC05;struct Cyc_Absyn_AggrdeclImpl*_tmpC04;struct Cyc_Absyn_Aggrdecl*
_tmpC03;struct Cyc_Absyn_Aggrdecl*_tmp79A=(_tmpC03=_cycalloc(sizeof(*_tmpC03)),((
_tmpC03->kind=(void*)0,((_tmpC03->sc=(void*)2,((_tmpC03->name=((_tmpC05=
_cycalloc(sizeof(*_tmpC05)),((_tmpC05->f1=Cyc_Absyn_Loc_n,((_tmpC05->f2=str,
_tmpC05)))))),((_tmpC03->tvs=0,((_tmpC03->impl=((_tmpC04=_cycalloc(sizeof(*
_tmpC04)),((_tmpC04->exist_vars=0,((_tmpC04->rgn_po=0,((_tmpC04->fields=_tmp799,((
_tmpC04->tagged=0,_tmpC04)))))))))),((_tmpC03->attributes=0,_tmpC03)))))))))))));{
struct Cyc_Absyn_Aggr_d_struct*_tmpC0B;struct Cyc_Absyn_Aggr_d_struct _tmpC0A;
struct Cyc_List_List*_tmpC09;Cyc_Toc_result_decls=((_tmpC09=_cycalloc(sizeof(*
_tmpC09)),((_tmpC09->hd=Cyc_Absyn_new_decl((void*)((_tmpC0B=_cycalloc(sizeof(*
_tmpC0B)),((_tmpC0B[0]=((_tmpC0A.tag=6,((_tmpC0A.f1=_tmp79A,_tmpC0A)))),_tmpC0B)))),
0),((_tmpC09->tl=Cyc_Toc_result_decls,_tmpC09))))));}_tmp794->type=Cyc_Absyn_strct(
str);}{struct Cyc_List_List*_tmpC0C;new_fields=((_tmpC0C=_cycalloc(sizeof(*
_tmpC0C)),((_tmpC0C->hd=_tmp794,((_tmpC0C->tl=new_fields,_tmpC0C))))));}}}(
new_ad->impl)->fields=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
new_fields);}}{struct Cyc_Absyn_Decl*_tmpC16;struct Cyc_Absyn_Aggr_d_struct _tmpC15;
struct Cyc_Absyn_Aggr_d_struct*_tmpC14;struct Cyc_List_List*_tmpC13;Cyc_Toc_result_decls=((
_tmpC13=_cycalloc(sizeof(*_tmpC13)),((_tmpC13->hd=((_tmpC16=_cycalloc(sizeof(*
_tmpC16)),((_tmpC16->r=(void*)((_tmpC14=_cycalloc(sizeof(*_tmpC14)),((_tmpC14[0]=((
_tmpC15.tag=6,((_tmpC15.f1=new_ad,_tmpC15)))),_tmpC14)))),((_tmpC16->loc=0,
_tmpC16)))))),((_tmpC13->tl=Cyc_Toc_result_decls,_tmpC13))))));}}};
_pop_dynregion(d);}}static void Cyc_Toc_datatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*
tud);static void Cyc_Toc_datatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*tud){
struct _DynRegionHandle*_tmp7B1;struct Cyc_Set_Set**_tmp7B2;struct Cyc_Toc_TocState
_tmp7B0=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp7B1=
_tmp7B0.dyn;_tmp7B2=_tmp7B0.datatypes_so_far;{struct _DynRegionFrame _tmp7B3;
struct _RegionHandle*d=_open_dynregion(& _tmp7B3,_tmp7B1);{struct _tuple1*_tmp7B4=
tud->name;if(tud->fields == 0  || ((int(*)(struct Cyc_Set_Set*s,struct _tuple1*elt))
Cyc_Set_member)(*_tmp7B2,_tmp7B4)){_npop_handler(0);return;}*_tmp7B2=((struct Cyc_Set_Set*(*)(
struct _RegionHandle*r,struct Cyc_Set_Set*s,struct _tuple1*elt))Cyc_Set_rinsert)(d,*
_tmp7B2,_tmp7B4);}{struct Cyc_List_List*_tmp7B5=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(tud->fields))->v;for(0;_tmp7B5 != 0;_tmp7B5=_tmp7B5->tl){struct Cyc_Absyn_Datatypefield*
f=(struct Cyc_Absyn_Datatypefield*)_tmp7B5->hd;if(f->typs != 0){struct Cyc_List_List*
_tmp7B6=0;int i=1;{struct Cyc_List_List*_tmp7B7=f->typs;for(0;_tmp7B7 != 0;(_tmp7B7=
_tmp7B7->tl,i ++)){struct _dyneither_ptr*_tmp7B8=Cyc_Absyn_fieldname(i);struct Cyc_Absyn_Aggrfield*
_tmpC17;struct Cyc_Absyn_Aggrfield*_tmp7B9=(_tmpC17=_cycalloc(sizeof(*_tmpC17)),((
_tmpC17->name=_tmp7B8,((_tmpC17->tq=(*((struct _tuple10*)_tmp7B7->hd)).f1,((
_tmpC17->type=Cyc_Toc_typ_to_c_array((*((struct _tuple10*)_tmp7B7->hd)).f2),((
_tmpC17->width=0,((_tmpC17->attributes=0,_tmpC17)))))))))));struct Cyc_List_List*
_tmpC18;_tmp7B6=((_tmpC18=_cycalloc(sizeof(*_tmpC18)),((_tmpC18->hd=_tmp7B9,((
_tmpC18->tl=_tmp7B6,_tmpC18))))));}}{struct Cyc_Absyn_Aggrfield*_tmpC1B;struct Cyc_List_List*
_tmpC1A;_tmp7B6=((_tmpC1A=_cycalloc(sizeof(*_tmpC1A)),((_tmpC1A->hd=((_tmpC1B=
_cycalloc(sizeof(*_tmpC1B)),((_tmpC1B->name=Cyc_Toc_tag_sp,((_tmpC1B->tq=Cyc_Toc_mt_tq,((
_tmpC1B->type=Cyc_Absyn_sint_typ,((_tmpC1B->width=0,((_tmpC1B->attributes=0,
_tmpC1B)))))))))))),((_tmpC1A->tl=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(_tmp7B6),_tmpC1A))))));}{const char*_tmpC20;struct Cyc_Absyn_AggrdeclImpl*
_tmpC1F;struct Cyc_Absyn_Aggrdecl*_tmpC1E;struct Cyc_Absyn_Aggrdecl*_tmp7BE=(
_tmpC1E=_cycalloc(sizeof(*_tmpC1E)),((_tmpC1E->kind=(void*)0,((_tmpC1E->sc=(void*)
2,((_tmpC1E->name=Cyc_Toc_collapse_qvar_tag(f->name,((_tmpC20="_struct",
_tag_dyneither(_tmpC20,sizeof(char),8)))),((_tmpC1E->tvs=0,((_tmpC1E->impl=((
_tmpC1F=_cycalloc(sizeof(*_tmpC1F)),((_tmpC1F->exist_vars=0,((_tmpC1F->rgn_po=0,((
_tmpC1F->fields=_tmp7B6,((_tmpC1F->tagged=0,_tmpC1F)))))))))),((_tmpC1E->attributes=
0,_tmpC1E)))))))))))));struct Cyc_Absyn_Aggr_d_struct*_tmpC26;struct Cyc_Absyn_Aggr_d_struct
_tmpC25;struct Cyc_List_List*_tmpC24;Cyc_Toc_result_decls=((_tmpC24=_cycalloc(
sizeof(*_tmpC24)),((_tmpC24->hd=Cyc_Absyn_new_decl((void*)((_tmpC26=_cycalloc(
sizeof(*_tmpC26)),((_tmpC26[0]=((_tmpC25.tag=6,((_tmpC25.f1=_tmp7BE,_tmpC25)))),
_tmpC26)))),0),((_tmpC24->tl=Cyc_Toc_result_decls,_tmpC24))))));}}}};
_pop_dynregion(d);}}static void Cyc_Toc_xdatatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*
xd);static void Cyc_Toc_xdatatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*xd){if(xd->fields
== 0)return;{struct _DynRegionHandle*_tmp7C6;struct Cyc_Dict_Dict*_tmp7C7;struct
Cyc_Toc_TocState _tmp7C5=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));
_tmp7C6=_tmp7C5.dyn;_tmp7C7=_tmp7C5.xdatatypes_so_far;{struct _DynRegionFrame
_tmp7C8;struct _RegionHandle*d=_open_dynregion(& _tmp7C8,_tmp7C6);{struct _tuple1*
_tmp7C9=xd->name;struct Cyc_List_List*_tmp7CA=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(xd->fields))->v;for(0;_tmp7CA != 0;_tmp7CA=_tmp7CA->tl){struct Cyc_Absyn_Datatypefield*
f=(struct Cyc_Absyn_Datatypefield*)_tmp7CA->hd;struct _dyneither_ptr*fn=(*f->name).f2;
struct Cyc_Absyn_Exp*_tmp7CB=Cyc_Absyn_uint_exp(_get_dyneither_size(*fn,sizeof(
char))+ 4,0);void*_tmp7CC=Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq,(
struct Cyc_Absyn_Exp*)_tmp7CB,Cyc_Absyn_false_conref,0);int*_tmp7CD=((int*(*)(
struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup_opt)(*_tmp7C7,f->name);int
_tmp7CE;_LL3CF: if(_tmp7CD != 0)goto _LL3D1;_LL3D0: {struct Cyc_Absyn_Exp*initopt=0;
if(f->sc != (void*)3){char zero='\000';const char*_tmpC2E;void*_tmpC2D[5];struct Cyc_Int_pa_struct
_tmpC2C;struct Cyc_Int_pa_struct _tmpC2B;struct Cyc_Int_pa_struct _tmpC2A;struct Cyc_Int_pa_struct
_tmpC29;struct Cyc_String_pa_struct _tmpC28;initopt=(struct Cyc_Absyn_Exp*)Cyc_Absyn_string_exp((
struct _dyneither_ptr)((_tmpC28.tag=0,((_tmpC28.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*fn),((_tmpC29.tag=1,((_tmpC29.f1=(unsigned long)((int)zero),((
_tmpC2A.tag=1,((_tmpC2A.f1=(unsigned long)((int)zero),((_tmpC2B.tag=1,((_tmpC2B.f1=(
unsigned long)((int)zero),((_tmpC2C.tag=1,((_tmpC2C.f1=(unsigned long)((int)zero),((
_tmpC2D[0]=& _tmpC2C,((_tmpC2D[1]=& _tmpC2B,((_tmpC2D[2]=& _tmpC2A,((_tmpC2D[3]=&
_tmpC29,((_tmpC2D[4]=& _tmpC28,Cyc_aprintf(((_tmpC2E="%c%c%c%c%s",_tag_dyneither(
_tmpC2E,sizeof(char),11))),_tag_dyneither(_tmpC2D,sizeof(void*),5)))))))))))))))))))))))))))))))),
0);}{struct Cyc_Absyn_Vardecl*_tmp7D6=Cyc_Absyn_new_vardecl(f->name,_tmp7CC,
initopt);_tmp7D6->sc=f->sc;{struct Cyc_Absyn_Var_d_struct*_tmpC34;struct Cyc_Absyn_Var_d_struct
_tmpC33;struct Cyc_List_List*_tmpC32;Cyc_Toc_result_decls=((_tmpC32=_cycalloc(
sizeof(*_tmpC32)),((_tmpC32->hd=Cyc_Absyn_new_decl((void*)((_tmpC34=_cycalloc(
sizeof(*_tmpC34)),((_tmpC34[0]=((_tmpC33.tag=0,((_tmpC33.f1=_tmp7D6,_tmpC33)))),
_tmpC34)))),0),((_tmpC32->tl=Cyc_Toc_result_decls,_tmpC32))))));}*_tmp7C7=((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,int v))Cyc_Dict_insert)(*
_tmp7C7,f->name,f->sc != (void*)3);if(f->typs != 0){struct Cyc_List_List*fields=0;
int i=1;{struct Cyc_List_List*_tmp7DA=f->typs;for(0;_tmp7DA != 0;(_tmp7DA=_tmp7DA->tl,
i ++)){struct Cyc_Int_pa_struct _tmpC3C;void*_tmpC3B[1];const char*_tmpC3A;struct
_dyneither_ptr*_tmpC39;struct _dyneither_ptr*_tmp7DB=(_tmpC39=_cycalloc(sizeof(*
_tmpC39)),((_tmpC39[0]=(struct _dyneither_ptr)((_tmpC3C.tag=1,((_tmpC3C.f1=(
unsigned long)i,((_tmpC3B[0]=& _tmpC3C,Cyc_aprintf(((_tmpC3A="f%d",_tag_dyneither(
_tmpC3A,sizeof(char),4))),_tag_dyneither(_tmpC3B,sizeof(void*),1)))))))),_tmpC39)));
struct Cyc_Absyn_Aggrfield*_tmpC3D;struct Cyc_Absyn_Aggrfield*_tmp7DC=(_tmpC3D=
_cycalloc(sizeof(*_tmpC3D)),((_tmpC3D->name=_tmp7DB,((_tmpC3D->tq=(*((struct
_tuple10*)_tmp7DA->hd)).f1,((_tmpC3D->type=Cyc_Toc_typ_to_c_array((*((struct
_tuple10*)_tmp7DA->hd)).f2),((_tmpC3D->width=0,((_tmpC3D->attributes=0,_tmpC3D)))))))))));
struct Cyc_List_List*_tmpC3E;fields=((_tmpC3E=_cycalloc(sizeof(*_tmpC3E)),((
_tmpC3E->hd=_tmp7DC,((_tmpC3E->tl=fields,_tmpC3E))))));}}{struct Cyc_Absyn_Aggrfield*
_tmpC41;struct Cyc_List_List*_tmpC40;fields=((_tmpC40=_cycalloc(sizeof(*_tmpC40)),((
_tmpC40->hd=((_tmpC41=_cycalloc(sizeof(*_tmpC41)),((_tmpC41->name=Cyc_Toc_tag_sp,((
_tmpC41->tq=Cyc_Toc_mt_tq,((_tmpC41->type=Cyc_Absyn_cstar_typ(Cyc_Absyn_char_typ,
Cyc_Toc_mt_tq),((_tmpC41->width=0,((_tmpC41->attributes=0,_tmpC41)))))))))))),((
_tmpC40->tl=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
fields),_tmpC40))))));}{const char*_tmpC46;struct Cyc_Absyn_AggrdeclImpl*_tmpC45;
struct Cyc_Absyn_Aggrdecl*_tmpC44;struct Cyc_Absyn_Aggrdecl*_tmp7E5=(_tmpC44=
_cycalloc(sizeof(*_tmpC44)),((_tmpC44->kind=(void*)0,((_tmpC44->sc=(void*)2,((
_tmpC44->name=Cyc_Toc_collapse_qvar_tag(f->name,((_tmpC46="_struct",
_tag_dyneither(_tmpC46,sizeof(char),8)))),((_tmpC44->tvs=0,((_tmpC44->impl=((
_tmpC45=_cycalloc(sizeof(*_tmpC45)),((_tmpC45->exist_vars=0,((_tmpC45->rgn_po=0,((
_tmpC45->fields=fields,((_tmpC45->tagged=0,_tmpC45)))))))))),((_tmpC44->attributes=
0,_tmpC44)))))))))))));struct Cyc_Absyn_Aggr_d_struct*_tmpC4C;struct Cyc_Absyn_Aggr_d_struct
_tmpC4B;struct Cyc_List_List*_tmpC4A;Cyc_Toc_result_decls=((_tmpC4A=_cycalloc(
sizeof(*_tmpC4A)),((_tmpC4A->hd=Cyc_Absyn_new_decl((void*)((_tmpC4C=_cycalloc(
sizeof(*_tmpC4C)),((_tmpC4C[0]=((_tmpC4B.tag=6,((_tmpC4B.f1=_tmp7E5,_tmpC4B)))),
_tmpC4C)))),0),((_tmpC4A->tl=Cyc_Toc_result_decls,_tmpC4A))))));}}goto _LL3CE;}}
_LL3D1: if(_tmp7CD == 0)goto _LL3D3;_tmp7CE=*_tmp7CD;if(_tmp7CE != 0)goto _LL3D3;
_LL3D2: if(f->sc != (void*)3){char zero='\000';const char*_tmpC54;void*_tmpC53[5];
struct Cyc_Int_pa_struct _tmpC52;struct Cyc_Int_pa_struct _tmpC51;struct Cyc_Int_pa_struct
_tmpC50;struct Cyc_Int_pa_struct _tmpC4F;struct Cyc_String_pa_struct _tmpC4E;struct
Cyc_Absyn_Exp*_tmp7EC=Cyc_Absyn_string_exp((struct _dyneither_ptr)((_tmpC4E.tag=0,((
_tmpC4E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*fn),((_tmpC4F.tag=1,((
_tmpC4F.f1=(unsigned long)((int)zero),((_tmpC50.tag=1,((_tmpC50.f1=(
unsigned long)((int)zero),((_tmpC51.tag=1,((_tmpC51.f1=(unsigned long)((int)zero),((
_tmpC52.tag=1,((_tmpC52.f1=(unsigned long)((int)zero),((_tmpC53[0]=& _tmpC52,((
_tmpC53[1]=& _tmpC51,((_tmpC53[2]=& _tmpC50,((_tmpC53[3]=& _tmpC4F,((_tmpC53[4]=&
_tmpC4E,Cyc_aprintf(((_tmpC54="%c%c%c%c%s",_tag_dyneither(_tmpC54,sizeof(char),
11))),_tag_dyneither(_tmpC53,sizeof(void*),5)))))))))))))))))))))))))))))))),0);
struct Cyc_Absyn_Vardecl*_tmp7ED=Cyc_Absyn_new_vardecl(f->name,_tmp7CC,(struct Cyc_Absyn_Exp*)
_tmp7EC);_tmp7ED->sc=f->sc;{struct Cyc_Absyn_Var_d_struct*_tmpC5A;struct Cyc_Absyn_Var_d_struct
_tmpC59;struct Cyc_List_List*_tmpC58;Cyc_Toc_result_decls=((_tmpC58=_cycalloc(
sizeof(*_tmpC58)),((_tmpC58->hd=Cyc_Absyn_new_decl((void*)((_tmpC5A=_cycalloc(
sizeof(*_tmpC5A)),((_tmpC5A[0]=((_tmpC59.tag=0,((_tmpC59.f1=_tmp7ED,_tmpC59)))),
_tmpC5A)))),0),((_tmpC58->tl=Cyc_Toc_result_decls,_tmpC58))))));}*_tmp7C7=((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,int v))Cyc_Dict_insert)(*
_tmp7C7,f->name,1);}goto _LL3CE;_LL3D3:;_LL3D4: goto _LL3CE;_LL3CE:;}};
_pop_dynregion(d);}}}static void Cyc_Toc_enumdecl_to_c(struct Cyc_Toc_Env*nv,struct
Cyc_Absyn_Enumdecl*ed);static void Cyc_Toc_enumdecl_to_c(struct Cyc_Toc_Env*nv,
struct Cyc_Absyn_Enumdecl*ed){ed->sc=(void*)2;if(ed->fields != 0)Cyc_Toc_enumfields_to_c((
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ed->fields))->v);}static
void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*body_nv,struct Cyc_Toc_Env*init_nv,
struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*s);static void Cyc_Toc_local_decl_to_c(
struct Cyc_Toc_Env*body_nv,struct Cyc_Toc_Env*init_nv,struct Cyc_Absyn_Vardecl*vd,
struct Cyc_Absyn_Stmt*s){void*old_typ=vd->type;vd->type=Cyc_Toc_typ_to_c_array(
old_typ);if(vd->sc == (void*)5  && Cyc_Tcutil_is_tagged_pointer_typ(old_typ))vd->sc=(
void*)2;Cyc_Toc_stmt_to_c(body_nv,s);if(vd->initializer != 0){struct Cyc_Absyn_Exp*
init=(struct Cyc_Absyn_Exp*)_check_null(vd->initializer);void*_tmp7F8=init->r;
struct Cyc_Absyn_Vardecl*_tmp7F9;struct Cyc_Absyn_Exp*_tmp7FA;struct Cyc_Absyn_Exp*
_tmp7FB;int _tmp7FC;_LL3D6: if(*((int*)_tmp7F8)!= 29)goto _LL3D8;_tmp7F9=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmp7F8)->f1;_tmp7FA=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp7F8)->f2;_tmp7FB=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp7F8)->f3;
_tmp7FC=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp7F8)->f4;_LL3D7: vd->initializer=
0;s->r=(Cyc_Toc_init_comprehension(init_nv,Cyc_Absyn_var_exp(vd->name,0),_tmp7F9,
_tmp7FA,_tmp7FB,_tmp7FC,Cyc_Absyn_new_stmt(s->r,0),0))->r;goto _LL3D5;_LL3D8:;
_LL3D9: if(vd->sc == (void*)0){struct Cyc_Toc_Env _tmp7FE;struct _RegionHandle*
_tmp7FF;struct Cyc_Toc_Env*_tmp7FD=init_nv;_tmp7FE=*_tmp7FD;_tmp7FF=_tmp7FE.rgn;{
struct Cyc_Toc_Env*_tmp800=Cyc_Toc_set_toplevel(_tmp7FF,init_nv);Cyc_Toc_exp_to_c(
_tmp800,init);}}else{Cyc_Toc_exp_to_c(init_nv,init);}goto _LL3D5;_LL3D5:;}else{
void*_tmp801=Cyc_Tcutil_compress(old_typ);struct Cyc_Absyn_ArrayInfo _tmp802;void*
_tmp803;struct Cyc_Absyn_Exp*_tmp804;union Cyc_Absyn_Constraint*_tmp805;_LL3DB: if(
_tmp801 <= (void*)4)goto _LL3DD;if(*((int*)_tmp801)!= 7)goto _LL3DD;_tmp802=((
struct Cyc_Absyn_ArrayType_struct*)_tmp801)->f1;_tmp803=_tmp802.elt_type;_tmp804=
_tmp802.num_elts;_tmp805=_tmp802.zero_term;if(!((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,_tmp805))goto _LL3DD;_LL3DC: if(_tmp804 == 0){const char*
_tmpC5D;void*_tmpC5C;(_tmpC5C=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC5D="can't initialize zero-terminated array -- size unknown",
_tag_dyneither(_tmpC5D,sizeof(char),55))),_tag_dyneither(_tmpC5C,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*num_elts=(struct Cyc_Absyn_Exp*)_tmp804;struct Cyc_Absyn_Exp*
_tmp808=Cyc_Absyn_subscript_exp(Cyc_Absyn_var_exp(vd->name,0),Cyc_Absyn_add_exp(
num_elts,Cyc_Absyn_signed_int_exp(- 1,0),0),0);struct Cyc_Absyn_Exp*_tmp809=Cyc_Absyn_signed_int_exp(
0,0);s->r=Cyc_Toc_seq_stmt_r(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp808,
_tmp809,0),0),Cyc_Absyn_new_stmt(s->r,0));goto _LL3DA;}_LL3DD:;_LL3DE: goto _LL3DA;
_LL3DA:;}}static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,
struct Cyc_Absyn_Pat*p,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s);
static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*
p,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s){Cyc_Toc_exp_to_c(nv,e);{
struct _tuple1*x=Cyc_Toc_temp_var();struct Cyc_Toc_Env _tmp80B;struct _RegionHandle*
_tmp80C;struct Cyc_Toc_Env*_tmp80A=nv;_tmp80B=*_tmp80A;_tmp80C=_tmp80B.rgn;{
struct Cyc_Absyn_Stmt*_tmp80D=Cyc_Toc_throw_match_stmt();struct Cyc_Toc_Env*
_tmp80E=Cyc_Toc_share_env(_tmp80C,nv);struct Cyc_Toc_Env*_tmp810;struct Cyc_List_List*
_tmp811;struct Cyc_Absyn_Stmt*_tmp812;struct _tuple21 _tmp80F=Cyc_Toc_xlate_pat(
_tmp80E,_tmp80C,t,Cyc_Absyn_var_exp(x,0),Cyc_Absyn_var_exp(x,0),p,(struct Cyc_Absyn_Stmt**)&
_tmp80D,Cyc_Toc_throw_match_stmt(),0);_tmp810=_tmp80F.f1;_tmp811=_tmp80F.f2;
_tmp812=_tmp80F.f3;Cyc_Toc_stmt_to_c(_tmp810,s);s=Cyc_Absyn_declare_stmt(x,Cyc_Toc_typ_to_c(
t),(struct Cyc_Absyn_Exp*)e,Cyc_Absyn_seq_stmt(_tmp812,s,0),0);for(0;_tmp811 != 0;
_tmp811=_tmp811->tl){struct _tuple22 _tmp814;struct _tuple1*_tmp815;void*_tmp816;
struct _tuple22*_tmp813=(struct _tuple22*)_tmp811->hd;_tmp814=*_tmp813;_tmp815=
_tmp814.f1;_tmp816=_tmp814.f2;s=Cyc_Absyn_declare_stmt(_tmp815,_tmp816,0,s,0);}}
return s;}}static void Cyc_Toc_exptypes_to_c(struct Cyc_Absyn_Exp*e);static void Cyc_Toc_exptypes_to_c(
struct Cyc_Absyn_Exp*e){void*_tmp817=e->r;struct Cyc_Absyn_Exp*_tmp818;struct Cyc_Absyn_Exp*
_tmp819;struct Cyc_Absyn_Exp*_tmp81A;struct Cyc_Absyn_Exp*_tmp81B;struct Cyc_Absyn_Exp*
_tmp81C;struct Cyc_Absyn_Exp*_tmp81D;struct Cyc_Absyn_Exp*_tmp81E;struct Cyc_Absyn_Exp*
_tmp81F;struct Cyc_List_List*_tmp820;struct Cyc_Absyn_Exp*_tmp821;struct Cyc_Absyn_Exp*
_tmp822;struct Cyc_Absyn_Exp*_tmp823;struct Cyc_Absyn_Exp*_tmp824;struct Cyc_Absyn_Exp*
_tmp825;struct Cyc_Absyn_Exp*_tmp826;struct Cyc_Absyn_Exp*_tmp827;struct Cyc_Absyn_Exp*
_tmp828;struct Cyc_Absyn_Exp*_tmp829;struct Cyc_Absyn_Exp*_tmp82A;struct Cyc_Absyn_Exp*
_tmp82B;struct Cyc_Absyn_Exp*_tmp82C;struct Cyc_Absyn_Exp*_tmp82D;struct Cyc_Absyn_Exp*
_tmp82E;struct Cyc_Absyn_Exp*_tmp82F;struct Cyc_Absyn_Exp*_tmp830;struct Cyc_List_List*
_tmp831;struct Cyc_Absyn_Exp*_tmp832;struct Cyc_List_List*_tmp833;void*_tmp834;
void**_tmp835;struct Cyc_Absyn_Exp*_tmp836;struct _tuple8*_tmp837;struct _tuple8
_tmp838;void*_tmp839;void**_tmp83A;struct Cyc_List_List*_tmp83B;struct Cyc_List_List*
_tmp83C;struct Cyc_List_List*_tmp83D;void*_tmp83E;void**_tmp83F;void*_tmp840;void**
_tmp841;struct Cyc_Absyn_Stmt*_tmp842;struct Cyc_Absyn_MallocInfo _tmp843;struct Cyc_Absyn_MallocInfo*
_tmp844;_LL3E0: if(*((int*)_tmp817)!= 22)goto _LL3E2;_tmp818=((struct Cyc_Absyn_Deref_e_struct*)
_tmp817)->f1;_LL3E1: _tmp819=_tmp818;goto _LL3E3;_LL3E2: if(*((int*)_tmp817)!= 23)
goto _LL3E4;_tmp819=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp817)->f1;_LL3E3:
_tmp81A=_tmp819;goto _LL3E5;_LL3E4: if(*((int*)_tmp817)!= 24)goto _LL3E6;_tmp81A=((
struct Cyc_Absyn_AggrArrow_e_struct*)_tmp817)->f1;_LL3E5: _tmp81B=_tmp81A;goto
_LL3E7;_LL3E6: if(*((int*)_tmp817)!= 16)goto _LL3E8;_tmp81B=((struct Cyc_Absyn_Address_e_struct*)
_tmp817)->f1;_LL3E7: _tmp81C=_tmp81B;goto _LL3E9;_LL3E8: if(*((int*)_tmp817)!= 12)
goto _LL3EA;_tmp81C=((struct Cyc_Absyn_Throw_e_struct*)_tmp817)->f1;_LL3E9: _tmp81D=
_tmp81C;goto _LL3EB;_LL3EA: if(*((int*)_tmp817)!= 13)goto _LL3EC;_tmp81D=((struct
Cyc_Absyn_NoInstantiate_e_struct*)_tmp817)->f1;_LL3EB: _tmp81E=_tmp81D;goto _LL3ED;
_LL3EC: if(*((int*)_tmp817)!= 19)goto _LL3EE;_tmp81E=((struct Cyc_Absyn_Sizeofexp_e_struct*)
_tmp817)->f1;_LL3ED: _tmp81F=_tmp81E;goto _LL3EF;_LL3EE: if(*((int*)_tmp817)!= 5)
goto _LL3F0;_tmp81F=((struct Cyc_Absyn_Increment_e_struct*)_tmp817)->f1;_LL3EF: Cyc_Toc_exptypes_to_c(
_tmp81F);goto _LL3DF;_LL3F0: if(*((int*)_tmp817)!= 3)goto _LL3F2;_tmp820=((struct
Cyc_Absyn_Primop_e_struct*)_tmp817)->f2;_LL3F1:((void(*)(void(*f)(struct Cyc_Absyn_Exp*),
struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Toc_exptypes_to_c,_tmp820);goto _LL3DF;
_LL3F2: if(*((int*)_tmp817)!= 7)goto _LL3F4;_tmp821=((struct Cyc_Absyn_And_e_struct*)
_tmp817)->f1;_tmp822=((struct Cyc_Absyn_And_e_struct*)_tmp817)->f2;_LL3F3: _tmp823=
_tmp821;_tmp824=_tmp822;goto _LL3F5;_LL3F4: if(*((int*)_tmp817)!= 8)goto _LL3F6;
_tmp823=((struct Cyc_Absyn_Or_e_struct*)_tmp817)->f1;_tmp824=((struct Cyc_Absyn_Or_e_struct*)
_tmp817)->f2;_LL3F5: _tmp825=_tmp823;_tmp826=_tmp824;goto _LL3F7;_LL3F6: if(*((int*)
_tmp817)!= 9)goto _LL3F8;_tmp825=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp817)->f1;
_tmp826=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp817)->f2;_LL3F7: _tmp827=_tmp825;
_tmp828=_tmp826;goto _LL3F9;_LL3F8: if(*((int*)_tmp817)!= 25)goto _LL3FA;_tmp827=((
struct Cyc_Absyn_Subscript_e_struct*)_tmp817)->f1;_tmp828=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp817)->f2;_LL3F9: _tmp829=_tmp827;_tmp82A=_tmp828;goto _LL3FB;_LL3FA: if(*((int*)
_tmp817)!= 36)goto _LL3FC;_tmp829=((struct Cyc_Absyn_Swap_e_struct*)_tmp817)->f1;
_tmp82A=((struct Cyc_Absyn_Swap_e_struct*)_tmp817)->f2;_LL3FB: _tmp82B=_tmp829;
_tmp82C=_tmp82A;goto _LL3FD;_LL3FC: if(*((int*)_tmp817)!= 4)goto _LL3FE;_tmp82B=((
struct Cyc_Absyn_AssignOp_e_struct*)_tmp817)->f1;_tmp82C=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmp817)->f3;_LL3FD: Cyc_Toc_exptypes_to_c(_tmp82B);Cyc_Toc_exptypes_to_c(_tmp82C);
goto _LL3DF;_LL3FE: if(*((int*)_tmp817)!= 6)goto _LL400;_tmp82D=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp817)->f1;_tmp82E=((struct Cyc_Absyn_Conditional_e_struct*)_tmp817)->f2;
_tmp82F=((struct Cyc_Absyn_Conditional_e_struct*)_tmp817)->f3;_LL3FF: Cyc_Toc_exptypes_to_c(
_tmp82D);Cyc_Toc_exptypes_to_c(_tmp82E);Cyc_Toc_exptypes_to_c(_tmp82F);goto
_LL3DF;_LL400: if(*((int*)_tmp817)!= 11)goto _LL402;_tmp830=((struct Cyc_Absyn_FnCall_e_struct*)
_tmp817)->f1;_tmp831=((struct Cyc_Absyn_FnCall_e_struct*)_tmp817)->f2;_LL401:
_tmp832=_tmp830;_tmp833=_tmp831;goto _LL403;_LL402: if(*((int*)_tmp817)!= 10)goto
_LL404;_tmp832=((struct Cyc_Absyn_UnknownCall_e_struct*)_tmp817)->f1;_tmp833=((
struct Cyc_Absyn_UnknownCall_e_struct*)_tmp817)->f2;_LL403: Cyc_Toc_exptypes_to_c(
_tmp832);((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(
Cyc_Toc_exptypes_to_c,_tmp833);goto _LL3DF;_LL404: if(*((int*)_tmp817)!= 15)goto
_LL406;_tmp834=(void**)&((struct Cyc_Absyn_Cast_e_struct*)_tmp817)->f1;_tmp835=(
void**)((void**)&((struct Cyc_Absyn_Cast_e_struct*)_tmp817)->f1);_tmp836=((struct
Cyc_Absyn_Cast_e_struct*)_tmp817)->f2;_LL405:*_tmp835=Cyc_Toc_typ_to_c(*_tmp835);
Cyc_Toc_exptypes_to_c(_tmp836);goto _LL3DF;_LL406: if(*((int*)_tmp817)!= 27)goto
_LL408;_tmp837=((struct Cyc_Absyn_CompoundLit_e_struct*)_tmp817)->f1;_tmp838=*
_tmp837;_tmp839=_tmp838.f3;_tmp83A=(void**)&(*((struct Cyc_Absyn_CompoundLit_e_struct*)
_tmp817)->f1).f3;_tmp83B=((struct Cyc_Absyn_CompoundLit_e_struct*)_tmp817)->f2;
_LL407:*_tmp83A=Cyc_Toc_typ_to_c(*_tmp83A);_tmp83C=_tmp83B;goto _LL409;_LL408: if(*((
int*)_tmp817)!= 37)goto _LL40A;_tmp83C=((struct Cyc_Absyn_UnresolvedMem_e_struct*)
_tmp817)->f2;_LL409: _tmp83D=_tmp83C;goto _LL40B;_LL40A: if(*((int*)_tmp817)!= 28)
goto _LL40C;_tmp83D=((struct Cyc_Absyn_Array_e_struct*)_tmp817)->f1;_LL40B: for(0;
_tmp83D != 0;_tmp83D=_tmp83D->tl){struct Cyc_Absyn_Exp*_tmp846;struct _tuple16
_tmp845=*((struct _tuple16*)_tmp83D->hd);_tmp846=_tmp845.f2;Cyc_Toc_exptypes_to_c(
_tmp846);}goto _LL3DF;_LL40C: if(*((int*)_tmp817)!= 20)goto _LL40E;_tmp83E=(void**)&((
struct Cyc_Absyn_Offsetof_e_struct*)_tmp817)->f1;_tmp83F=(void**)((void**)&((
struct Cyc_Absyn_Offsetof_e_struct*)_tmp817)->f1);_LL40D: _tmp841=_tmp83F;goto
_LL40F;_LL40E: if(*((int*)_tmp817)!= 18)goto _LL410;_tmp840=(void**)&((struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp817)->f1;_tmp841=(void**)((void**)&((struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp817)->f1);_LL40F:*_tmp841=Cyc_Toc_typ_to_c(*_tmp841);goto _LL3DF;_LL410: if(*((
int*)_tmp817)!= 38)goto _LL412;_tmp842=((struct Cyc_Absyn_StmtExp_e_struct*)
_tmp817)->f1;_LL411: Cyc_Toc_stmttypes_to_c(_tmp842);goto _LL3DF;_LL412: if(*((int*)
_tmp817)!= 35)goto _LL414;_tmp843=((struct Cyc_Absyn_Malloc_e_struct*)_tmp817)->f1;
_tmp844=(struct Cyc_Absyn_MallocInfo*)&((struct Cyc_Absyn_Malloc_e_struct*)_tmp817)->f1;
_LL413: if(_tmp844->elt_type != 0){void**_tmpC5E;_tmp844->elt_type=((_tmpC5E=
_cycalloc(sizeof(*_tmpC5E)),((_tmpC5E[0]=Cyc_Toc_typ_to_c(*((void**)_check_null(
_tmp844->elt_type))),_tmpC5E))));}Cyc_Toc_exptypes_to_c(_tmp844->num_elts);goto
_LL3DF;_LL414: if(*((int*)_tmp817)!= 0)goto _LL416;_LL415: goto _LL417;_LL416: if(*((
int*)_tmp817)!= 1)goto _LL418;_LL417: goto _LL419;_LL418: if(*((int*)_tmp817)!= 2)
goto _LL41A;_LL419: goto _LL41B;_LL41A: if(*((int*)_tmp817)!= 33)goto _LL41C;_LL41B:
goto _LL41D;_LL41C: if(*((int*)_tmp817)!= 34)goto _LL41E;_LL41D: goto _LL3DF;_LL41E:
if(*((int*)_tmp817)!= 31)goto _LL420;_LL41F: goto _LL421;_LL420: if(*((int*)_tmp817)
!= 32)goto _LL422;_LL421: goto _LL423;_LL422: if(*((int*)_tmp817)!= 30)goto _LL424;
_LL423: goto _LL425;_LL424: if(*((int*)_tmp817)!= 29)goto _LL426;_LL425: goto _LL427;
_LL426: if(*((int*)_tmp817)!= 26)goto _LL428;_LL427: goto _LL429;_LL428: if(*((int*)
_tmp817)!= 14)goto _LL42A;_LL429: goto _LL42B;_LL42A: if(*((int*)_tmp817)!= 17)goto
_LL42C;_LL42B: goto _LL42D;_LL42C: if(*((int*)_tmp817)!= 40)goto _LL42E;_LL42D: goto
_LL42F;_LL42E: if(*((int*)_tmp817)!= 39)goto _LL430;_LL42F: goto _LL431;_LL430: if(*((
int*)_tmp817)!= 21)goto _LL3DF;_LL431:{const char*_tmpC61;void*_tmpC60;(_tmpC60=0,
Cyc_Tcutil_terr(e->loc,((_tmpC61="Cyclone expression within C code",
_tag_dyneither(_tmpC61,sizeof(char),33))),_tag_dyneither(_tmpC60,sizeof(void*),0)));}
goto _LL3DF;_LL3DF:;}static void Cyc_Toc_decltypes_to_c(struct Cyc_Absyn_Decl*d);
static void Cyc_Toc_decltypes_to_c(struct Cyc_Absyn_Decl*d){void*_tmp84A=d->r;
struct Cyc_Absyn_Vardecl*_tmp84B;struct Cyc_Absyn_Fndecl*_tmp84C;struct Cyc_Absyn_Aggrdecl*
_tmp84D;struct Cyc_Absyn_Enumdecl*_tmp84E;struct Cyc_Absyn_Typedefdecl*_tmp84F;
_LL433: if(_tmp84A <= (void*)2)goto _LL44F;if(*((int*)_tmp84A)!= 0)goto _LL435;
_tmp84B=((struct Cyc_Absyn_Var_d_struct*)_tmp84A)->f1;_LL434: _tmp84B->type=Cyc_Toc_typ_to_c(
_tmp84B->type);if(_tmp84B->initializer != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)
_check_null(_tmp84B->initializer));goto _LL432;_LL435: if(*((int*)_tmp84A)!= 1)
goto _LL437;_tmp84C=((struct Cyc_Absyn_Fn_d_struct*)_tmp84A)->f1;_LL436: _tmp84C->ret_type=
Cyc_Toc_typ_to_c(_tmp84C->ret_type);{struct Cyc_List_List*_tmp850=_tmp84C->args;
for(0;_tmp850 != 0;_tmp850=_tmp850->tl){(*((struct _tuple25*)_tmp850->hd)).f3=Cyc_Toc_typ_to_c((*((
struct _tuple25*)_tmp850->hd)).f3);}}goto _LL432;_LL437: if(*((int*)_tmp84A)!= 6)
goto _LL439;_tmp84D=((struct Cyc_Absyn_Aggr_d_struct*)_tmp84A)->f1;_LL438: Cyc_Toc_aggrdecl_to_c(
_tmp84D);goto _LL432;_LL439: if(*((int*)_tmp84A)!= 8)goto _LL43B;_tmp84E=((struct
Cyc_Absyn_Enum_d_struct*)_tmp84A)->f1;_LL43A: if(_tmp84E->fields != 0){struct Cyc_List_List*
_tmp851=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp84E->fields))->v;
for(0;_tmp851 != 0;_tmp851=_tmp851->tl){struct Cyc_Absyn_Enumfield*_tmp852=(struct
Cyc_Absyn_Enumfield*)_tmp851->hd;if(_tmp852->tag != 0)Cyc_Toc_exptypes_to_c((
struct Cyc_Absyn_Exp*)_check_null(_tmp852->tag));}}goto _LL432;_LL43B: if(*((int*)
_tmp84A)!= 9)goto _LL43D;_tmp84F=((struct Cyc_Absyn_Typedef_d_struct*)_tmp84A)->f1;
_LL43C:{struct Cyc_Core_Opt*_tmpC62;_tmp84F->defn=((_tmpC62=_cycalloc(sizeof(*
_tmpC62)),((_tmpC62->v=(void*)Cyc_Toc_typ_to_c_array((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp84F->defn))->v),_tmpC62))));}goto _LL432;_LL43D: if(*((int*)
_tmp84A)!= 2)goto _LL43F;_LL43E: goto _LL440;_LL43F: if(*((int*)_tmp84A)!= 3)goto
_LL441;_LL440: goto _LL442;_LL441: if(*((int*)_tmp84A)!= 7)goto _LL443;_LL442: goto
_LL444;_LL443: if(*((int*)_tmp84A)!= 10)goto _LL445;_LL444: goto _LL446;_LL445: if(*((
int*)_tmp84A)!= 11)goto _LL447;_LL446: goto _LL448;_LL447: if(*((int*)_tmp84A)!= 12)
goto _LL449;_LL448: goto _LL44A;_LL449: if(*((int*)_tmp84A)!= 13)goto _LL44B;_LL44A:
goto _LL44C;_LL44B: if(*((int*)_tmp84A)!= 4)goto _LL44D;_LL44C: goto _LL44E;_LL44D:
if(*((int*)_tmp84A)!= 5)goto _LL44F;_LL44E:{const char*_tmpC65;void*_tmpC64;(
_tmpC64=0,Cyc_Tcutil_terr(d->loc,((_tmpC65="Cyclone declaration within C code",
_tag_dyneither(_tmpC65,sizeof(char),34))),_tag_dyneither(_tmpC64,sizeof(void*),0)));}
goto _LL432;_LL44F: if((int)_tmp84A != 0)goto _LL451;_LL450: goto _LL452;_LL451: if((
int)_tmp84A != 1)goto _LL432;_LL452: goto _LL432;_LL432:;}static void Cyc_Toc_stmttypes_to_c(
struct Cyc_Absyn_Stmt*s);static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*s){
void*_tmp856=s->r;struct Cyc_Absyn_Exp*_tmp857;struct Cyc_Absyn_Stmt*_tmp858;
struct Cyc_Absyn_Stmt*_tmp859;struct Cyc_Absyn_Exp*_tmp85A;struct Cyc_Absyn_Exp*
_tmp85B;struct Cyc_Absyn_Stmt*_tmp85C;struct Cyc_Absyn_Stmt*_tmp85D;struct _tuple9
_tmp85E;struct Cyc_Absyn_Exp*_tmp85F;struct Cyc_Absyn_Stmt*_tmp860;struct Cyc_Absyn_Exp*
_tmp861;struct _tuple9 _tmp862;struct Cyc_Absyn_Exp*_tmp863;struct _tuple9 _tmp864;
struct Cyc_Absyn_Exp*_tmp865;struct Cyc_Absyn_Stmt*_tmp866;struct Cyc_Absyn_Exp*
_tmp867;struct Cyc_List_List*_tmp868;struct Cyc_Absyn_Decl*_tmp869;struct Cyc_Absyn_Stmt*
_tmp86A;struct Cyc_Absyn_Stmt*_tmp86B;struct _tuple9 _tmp86C;struct Cyc_Absyn_Exp*
_tmp86D;_LL454: if(_tmp856 <= (void*)1)goto _LL466;if(*((int*)_tmp856)!= 0)goto
_LL456;_tmp857=((struct Cyc_Absyn_Exp_s_struct*)_tmp856)->f1;_LL455: Cyc_Toc_exptypes_to_c(
_tmp857);goto _LL453;_LL456: if(*((int*)_tmp856)!= 1)goto _LL458;_tmp858=((struct
Cyc_Absyn_Seq_s_struct*)_tmp856)->f1;_tmp859=((struct Cyc_Absyn_Seq_s_struct*)
_tmp856)->f2;_LL457: Cyc_Toc_stmttypes_to_c(_tmp858);Cyc_Toc_stmttypes_to_c(
_tmp859);goto _LL453;_LL458: if(*((int*)_tmp856)!= 2)goto _LL45A;_tmp85A=((struct
Cyc_Absyn_Return_s_struct*)_tmp856)->f1;_LL459: if(_tmp85A != 0)Cyc_Toc_exptypes_to_c((
struct Cyc_Absyn_Exp*)_tmp85A);goto _LL453;_LL45A: if(*((int*)_tmp856)!= 3)goto
_LL45C;_tmp85B=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmp856)->f1;_tmp85C=((
struct Cyc_Absyn_IfThenElse_s_struct*)_tmp856)->f2;_tmp85D=((struct Cyc_Absyn_IfThenElse_s_struct*)
_tmp856)->f3;_LL45B: Cyc_Toc_exptypes_to_c(_tmp85B);Cyc_Toc_stmttypes_to_c(
_tmp85C);Cyc_Toc_stmttypes_to_c(_tmp85D);goto _LL453;_LL45C: if(*((int*)_tmp856)!= 
4)goto _LL45E;_tmp85E=((struct Cyc_Absyn_While_s_struct*)_tmp856)->f1;_tmp85F=
_tmp85E.f1;_tmp860=((struct Cyc_Absyn_While_s_struct*)_tmp856)->f2;_LL45D: Cyc_Toc_exptypes_to_c(
_tmp85F);Cyc_Toc_stmttypes_to_c(_tmp860);goto _LL453;_LL45E: if(*((int*)_tmp856)!= 
8)goto _LL460;_tmp861=((struct Cyc_Absyn_For_s_struct*)_tmp856)->f1;_tmp862=((
struct Cyc_Absyn_For_s_struct*)_tmp856)->f2;_tmp863=_tmp862.f1;_tmp864=((struct
Cyc_Absyn_For_s_struct*)_tmp856)->f3;_tmp865=_tmp864.f1;_tmp866=((struct Cyc_Absyn_For_s_struct*)
_tmp856)->f4;_LL45F: Cyc_Toc_exptypes_to_c(_tmp861);Cyc_Toc_exptypes_to_c(_tmp863);
Cyc_Toc_exptypes_to_c(_tmp865);Cyc_Toc_stmttypes_to_c(_tmp866);goto _LL453;_LL460:
if(*((int*)_tmp856)!= 9)goto _LL462;_tmp867=((struct Cyc_Absyn_Switch_s_struct*)
_tmp856)->f1;_tmp868=((struct Cyc_Absyn_Switch_s_struct*)_tmp856)->f2;_LL461: Cyc_Toc_exptypes_to_c(
_tmp867);for(0;_tmp868 != 0;_tmp868=_tmp868->tl){Cyc_Toc_stmttypes_to_c(((struct
Cyc_Absyn_Switch_clause*)_tmp868->hd)->body);}goto _LL453;_LL462: if(*((int*)
_tmp856)!= 11)goto _LL464;_tmp869=((struct Cyc_Absyn_Decl_s_struct*)_tmp856)->f1;
_tmp86A=((struct Cyc_Absyn_Decl_s_struct*)_tmp856)->f2;_LL463: Cyc_Toc_decltypes_to_c(
_tmp869);Cyc_Toc_stmttypes_to_c(_tmp86A);goto _LL453;_LL464: if(*((int*)_tmp856)!= 
13)goto _LL466;_tmp86B=((struct Cyc_Absyn_Do_s_struct*)_tmp856)->f1;_tmp86C=((
struct Cyc_Absyn_Do_s_struct*)_tmp856)->f2;_tmp86D=_tmp86C.f1;_LL465: Cyc_Toc_stmttypes_to_c(
_tmp86B);Cyc_Toc_exptypes_to_c(_tmp86D);goto _LL453;_LL466: if((int)_tmp856 != 0)
goto _LL468;_LL467: goto _LL469;_LL468: if(_tmp856 <= (void*)1)goto _LL46A;if(*((int*)
_tmp856)!= 5)goto _LL46A;_LL469: goto _LL46B;_LL46A: if(_tmp856 <= (void*)1)goto
_LL46C;if(*((int*)_tmp856)!= 6)goto _LL46C;_LL46B: goto _LL46D;_LL46C: if(_tmp856 <= (
void*)1)goto _LL46E;if(*((int*)_tmp856)!= 7)goto _LL46E;_LL46D: goto _LL453;_LL46E:
if(_tmp856 <= (void*)1)goto _LL470;if(*((int*)_tmp856)!= 10)goto _LL470;_LL46F: goto
_LL471;_LL470: if(_tmp856 <= (void*)1)goto _LL472;if(*((int*)_tmp856)!= 12)goto
_LL472;_LL471: goto _LL473;_LL472: if(_tmp856 <= (void*)1)goto _LL474;if(*((int*)
_tmp856)!= 14)goto _LL474;_LL473: goto _LL475;_LL474: if(_tmp856 <= (void*)1)goto
_LL453;if(*((int*)_tmp856)!= 15)goto _LL453;_LL475:{const char*_tmpC68;void*
_tmpC67;(_tmpC67=0,Cyc_Tcutil_terr(s->loc,((_tmpC68="Cyclone statement in C code",
_tag_dyneither(_tmpC68,sizeof(char),28))),_tag_dyneither(_tmpC67,sizeof(void*),0)));}
goto _LL453;_LL453:;}static struct Cyc_Toc_Env*Cyc_Toc_decls_to_c(struct
_RegionHandle*r,struct Cyc_Toc_Env*nv,struct Cyc_List_List*ds,int top,int cinclude);
static struct Cyc_Toc_Env*Cyc_Toc_decls_to_c(struct _RegionHandle*r,struct Cyc_Toc_Env*
nv,struct Cyc_List_List*ds,int top,int cinclude){for(0;ds != 0;ds=ds->tl){if(!Cyc_Toc_is_toplevel(
nv)){const char*_tmpC6B;void*_tmpC6A;(_tmpC6A=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpC6B="decls_to_c: not at toplevel!",
_tag_dyneither(_tmpC6B,sizeof(char),29))),_tag_dyneither(_tmpC6A,sizeof(void*),0)));}{
struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)ds->hd;void*_tmp872=d->r;struct Cyc_Absyn_Vardecl*
_tmp873;struct Cyc_Absyn_Fndecl*_tmp874;struct Cyc_Absyn_Aggrdecl*_tmp875;struct
Cyc_Absyn_Datatypedecl*_tmp876;struct Cyc_Absyn_Enumdecl*_tmp877;struct Cyc_Absyn_Typedefdecl*
_tmp878;struct Cyc_List_List*_tmp879;struct Cyc_List_List*_tmp87A;struct Cyc_List_List*
_tmp87B;struct Cyc_List_List*_tmp87C;_LL477: if(_tmp872 <= (void*)2)goto _LL48B;if(*((
int*)_tmp872)!= 0)goto _LL479;_tmp873=((struct Cyc_Absyn_Var_d_struct*)_tmp872)->f1;
_LL478: {struct _tuple1*_tmp87D=_tmp873->name;if(_tmp873->sc == (void*)4){struct
_tuple1*_tmpC6C;_tmp87D=((_tmpC6C=_cycalloc(sizeof(*_tmpC6C)),((_tmpC6C->f1=Cyc_Absyn_Rel_n(
0),((_tmpC6C->f2=(*_tmp87D).f2,_tmpC6C))))));}if(_tmp873->initializer != 0){if(
cinclude)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp873->initializer));
else{Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_check_null(_tmp873->initializer));}}{
struct Cyc_Absyn_Global_b_struct _tmpC6F;struct Cyc_Absyn_Global_b_struct*_tmpC6E;
nv=Cyc_Toc_add_varmap(r,nv,_tmp873->name,Cyc_Absyn_varb_exp(_tmp87D,(void*)((
_tmpC6E=_cycalloc(sizeof(*_tmpC6E)),((_tmpC6E[0]=((_tmpC6F.tag=0,((_tmpC6F.f1=
_tmp873,_tmpC6F)))),_tmpC6E)))),0));}_tmp873->name=_tmp87D;_tmp873->sc=Cyc_Toc_scope_to_c(
_tmp873->sc);_tmp873->type=Cyc_Toc_typ_to_c_array(_tmp873->type);{struct Cyc_List_List*
_tmpC70;Cyc_Toc_result_decls=((_tmpC70=_cycalloc(sizeof(*_tmpC70)),((_tmpC70->hd=
d,((_tmpC70->tl=Cyc_Toc_result_decls,_tmpC70))))));}goto _LL476;}_LL479: if(*((int*)
_tmp872)!= 1)goto _LL47B;_tmp874=((struct Cyc_Absyn_Fn_d_struct*)_tmp872)->f1;
_LL47A: {struct _tuple1*_tmp882=_tmp874->name;if(_tmp874->sc == (void*)4){{struct
_tuple1*_tmpC71;_tmp882=((_tmpC71=_cycalloc(sizeof(*_tmpC71)),((_tmpC71->f1=Cyc_Absyn_Rel_n(
0),((_tmpC71->f2=(*_tmp882).f2,_tmpC71))))));}_tmp874->sc=(void*)2;}nv=Cyc_Toc_add_varmap(
r,nv,_tmp874->name,Cyc_Absyn_var_exp(_tmp882,0));_tmp874->name=_tmp882;Cyc_Toc_fndecl_to_c(
nv,_tmp874,cinclude);{struct Cyc_List_List*_tmpC72;Cyc_Toc_result_decls=((_tmpC72=
_cycalloc(sizeof(*_tmpC72)),((_tmpC72->hd=d,((_tmpC72->tl=Cyc_Toc_result_decls,
_tmpC72))))));}goto _LL476;}_LL47B: if(*((int*)_tmp872)!= 2)goto _LL47D;_LL47C: goto
_LL47E;_LL47D: if(*((int*)_tmp872)!= 3)goto _LL47F;_LL47E: {const char*_tmpC75;void*
_tmpC74;(_tmpC74=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpC75="letdecl at toplevel",_tag_dyneither(_tmpC75,sizeof(char),20))),
_tag_dyneither(_tmpC74,sizeof(void*),0)));}_LL47F: if(*((int*)_tmp872)!= 4)goto
_LL481;_LL480: {const char*_tmpC78;void*_tmpC77;(_tmpC77=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC78="region decl at toplevel",
_tag_dyneither(_tmpC78,sizeof(char),24))),_tag_dyneither(_tmpC77,sizeof(void*),0)));}
_LL481: if(*((int*)_tmp872)!= 5)goto _LL483;_LL482: {const char*_tmpC7B;void*
_tmpC7A;(_tmpC7A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpC7B="alias decl at toplevel",_tag_dyneither(_tmpC7B,sizeof(char),23))),
_tag_dyneither(_tmpC7A,sizeof(void*),0)));}_LL483: if(*((int*)_tmp872)!= 6)goto
_LL485;_tmp875=((struct Cyc_Absyn_Aggr_d_struct*)_tmp872)->f1;_LL484: Cyc_Toc_aggrdecl_to_c(
_tmp875);goto _LL476;_LL485: if(*((int*)_tmp872)!= 7)goto _LL487;_tmp876=((struct
Cyc_Absyn_Datatype_d_struct*)_tmp872)->f1;_LL486: if(_tmp876->is_extensible)Cyc_Toc_xdatatypedecl_to_c(
_tmp876);else{Cyc_Toc_datatypedecl_to_c(_tmp876);}goto _LL476;_LL487: if(*((int*)
_tmp872)!= 8)goto _LL489;_tmp877=((struct Cyc_Absyn_Enum_d_struct*)_tmp872)->f1;
_LL488: Cyc_Toc_enumdecl_to_c(nv,_tmp877);{struct Cyc_List_List*_tmpC7C;Cyc_Toc_result_decls=((
_tmpC7C=_cycalloc(sizeof(*_tmpC7C)),((_tmpC7C->hd=d,((_tmpC7C->tl=Cyc_Toc_result_decls,
_tmpC7C))))));}goto _LL476;_LL489: if(*((int*)_tmp872)!= 9)goto _LL48B;_tmp878=((
struct Cyc_Absyn_Typedef_d_struct*)_tmp872)->f1;_LL48A: _tmp878->name=_tmp878->name;
_tmp878->tvs=0;if(_tmp878->defn != 0){struct Cyc_Core_Opt*_tmpC7D;_tmp878->defn=((
_tmpC7D=_cycalloc(sizeof(*_tmpC7D)),((_tmpC7D->v=(void*)Cyc_Toc_typ_to_c_array((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp878->defn))->v),_tmpC7D))));}else{
void*_tmp88D=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp878->kind))->v;_LL498:
if((int)_tmp88D != 2)goto _LL49A;_LL499:{struct Cyc_Core_Opt*_tmpC7E;_tmp878->defn=((
_tmpC7E=_cycalloc(sizeof(*_tmpC7E)),((_tmpC7E->v=(void*)Cyc_Absyn_void_star_typ(),
_tmpC7E))));}goto _LL497;_LL49A:;_LL49B:{struct Cyc_Core_Opt*_tmpC7F;_tmp878->defn=((
_tmpC7F=_cycalloc(sizeof(*_tmpC7F)),((_tmpC7F->v=(void*)((void*)0),_tmpC7F))));}
goto _LL497;_LL497:;}{struct Cyc_List_List*_tmpC80;Cyc_Toc_result_decls=((_tmpC80=
_cycalloc(sizeof(*_tmpC80)),((_tmpC80->hd=d,((_tmpC80->tl=Cyc_Toc_result_decls,
_tmpC80))))));}goto _LL476;_LL48B: if((int)_tmp872 != 0)goto _LL48D;_LL48C: goto
_LL48E;_LL48D: if((int)_tmp872 != 1)goto _LL48F;_LL48E: goto _LL476;_LL48F: if(_tmp872
<= (void*)2)goto _LL491;if(*((int*)_tmp872)!= 10)goto _LL491;_tmp879=((struct Cyc_Absyn_Namespace_d_struct*)
_tmp872)->f2;_LL490: _tmp87A=_tmp879;goto _LL492;_LL491: if(_tmp872 <= (void*)2)goto
_LL493;if(*((int*)_tmp872)!= 11)goto _LL493;_tmp87A=((struct Cyc_Absyn_Using_d_struct*)
_tmp872)->f2;_LL492: _tmp87B=_tmp87A;goto _LL494;_LL493: if(_tmp872 <= (void*)2)goto
_LL495;if(*((int*)_tmp872)!= 12)goto _LL495;_tmp87B=((struct Cyc_Absyn_ExternC_d_struct*)
_tmp872)->f1;_LL494: nv=Cyc_Toc_decls_to_c(r,nv,_tmp87B,top,cinclude);goto _LL476;
_LL495: if(_tmp872 <= (void*)2)goto _LL476;if(*((int*)_tmp872)!= 13)goto _LL476;
_tmp87C=((struct Cyc_Absyn_ExternCinclude_d_struct*)_tmp872)->f1;_LL496: nv=Cyc_Toc_decls_to_c(
r,nv,_tmp87C,top,1);goto _LL476;_LL476:;}}return nv;}static void Cyc_Toc_init();
static void Cyc_Toc_init(){struct _DynRegionHandle*_tmp892;struct Cyc_Core_NewRegion
_tmp891=Cyc_Core_new_dynregion();_tmp892=_tmp891.dynregion;{struct
_DynRegionFrame _tmp893;struct _RegionHandle*d=_open_dynregion(& _tmp893,_tmp892);{
struct Cyc_List_List**_tmpC8B;struct Cyc_Dict_Dict*_tmpC8A;struct Cyc_Set_Set**
_tmpC89;struct Cyc_Dict_Dict*_tmpC88;struct Cyc_Dict_Dict*_tmpC87;struct Cyc_Toc_TocState*
_tmpC86;Cyc_Toc_toc_state=((_tmpC86=_cycalloc(sizeof(*_tmpC86)),((_tmpC86->dyn=(
struct _DynRegionHandle*)_tmp892,((_tmpC86->tuple_types=(struct Cyc_List_List**)((
_tmpC8B=_region_malloc(d,sizeof(*_tmpC8B)),((_tmpC8B[0]=0,_tmpC8B)))),((_tmpC86->aggrs_so_far=(
struct Cyc_Dict_Dict*)((_tmpC8A=_region_malloc(d,sizeof(*_tmpC8A)),((_tmpC8A[0]=((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple1*,struct
_tuple1*)))Cyc_Dict_rempty)(d,Cyc_Absyn_qvar_cmp),_tmpC8A)))),((_tmpC86->datatypes_so_far=(
struct Cyc_Set_Set**)((_tmpC89=_region_malloc(d,sizeof(*_tmpC89)),((_tmpC89[0]=((
struct Cyc_Set_Set*(*)(struct _RegionHandle*r,int(*cmp)(struct _tuple1*,struct
_tuple1*)))Cyc_Set_rempty)(d,Cyc_Absyn_qvar_cmp),_tmpC89)))),((_tmpC86->xdatatypes_so_far=(
struct Cyc_Dict_Dict*)((_tmpC88=_region_malloc(d,sizeof(*_tmpC88)),((_tmpC88[0]=((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple1*,struct
_tuple1*)))Cyc_Dict_rempty)(d,Cyc_Absyn_qvar_cmp),_tmpC88)))),((_tmpC86->qvar_tags=(
struct Cyc_Dict_Dict*)((_tmpC87=_region_malloc(d,sizeof(*_tmpC87)),((_tmpC87[0]=((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple14*,struct
_tuple14*)))Cyc_Dict_rempty)(d,Cyc_Toc_qvar_tag_cmp),_tmpC87)))),((_tmpC86->temp_labels=(
struct Cyc_Xarray_Xarray*)((struct Cyc_Xarray_Xarray*(*)(struct _RegionHandle*))Cyc_Xarray_rcreate_empty)(
d),_tmpC86))))))))))))))));}Cyc_Toc_result_decls=0;Cyc_Toc_tuple_type_counter=0;
Cyc_Toc_temp_var_counter=0;Cyc_Toc_fresh_label_counter=0;Cyc_Toc_total_bounds_checks=
0;Cyc_Toc_bounds_checks_eliminated=0;{struct _dyneither_ptr**_tmpC8C;Cyc_Toc_globals=
_tag_dyneither(((_tmpC8C=_cycalloc(sizeof(struct _dyneither_ptr*)* 38),((_tmpC8C[
0]=& Cyc_Toc__throw_str,((_tmpC8C[1]=& Cyc_Toc_setjmp_str,((_tmpC8C[2]=& Cyc_Toc__push_handler_str,((
_tmpC8C[3]=& Cyc_Toc__pop_handler_str,((_tmpC8C[4]=& Cyc_Toc__exn_thrown_str,((
_tmpC8C[5]=& Cyc_Toc__npop_handler_str,((_tmpC8C[6]=& Cyc_Toc__check_null_str,((
_tmpC8C[7]=& Cyc_Toc__check_known_subscript_null_str,((_tmpC8C[8]=& Cyc_Toc__check_known_subscript_notnull_str,((
_tmpC8C[9]=& Cyc_Toc__check_dyneither_subscript_str,((_tmpC8C[10]=& Cyc_Toc__dyneither_ptr_str,((
_tmpC8C[11]=& Cyc_Toc__tag_dyneither_str,((_tmpC8C[12]=& Cyc_Toc__init_dyneither_ptr_str,((
_tmpC8C[13]=& Cyc_Toc__untag_dyneither_ptr_str,((_tmpC8C[14]=& Cyc_Toc__get_dyneither_size_str,((
_tmpC8C[15]=& Cyc_Toc__get_zero_arr_size_str,((_tmpC8C[16]=& Cyc_Toc__dyneither_ptr_plus_str,((
_tmpC8C[17]=& Cyc_Toc__zero_arr_plus_str,((_tmpC8C[18]=& Cyc_Toc__dyneither_ptr_inplace_plus_str,((
_tmpC8C[19]=& Cyc_Toc__zero_arr_inplace_plus_str,((_tmpC8C[20]=& Cyc_Toc__dyneither_ptr_inplace_plus_post_str,((
_tmpC8C[21]=& Cyc_Toc__zero_arr_inplace_plus_post_str,((_tmpC8C[22]=& Cyc_Toc__cycalloc_str,((
_tmpC8C[23]=& Cyc_Toc__cyccalloc_str,((_tmpC8C[24]=& Cyc_Toc__cycalloc_atomic_str,((
_tmpC8C[25]=& Cyc_Toc__cyccalloc_atomic_str,((_tmpC8C[26]=& Cyc_Toc__region_malloc_str,((
_tmpC8C[27]=& Cyc_Toc__region_calloc_str,((_tmpC8C[28]=& Cyc_Toc__check_times_str,((
_tmpC8C[29]=& Cyc_Toc__new_region_str,((_tmpC8C[30]=& Cyc_Toc__push_region_str,((
_tmpC8C[31]=& Cyc_Toc__pop_region_str,((_tmpC8C[32]=& Cyc_Toc__open_dynregion_str,((
_tmpC8C[33]=& Cyc_Toc__push_dynregion_str,((_tmpC8C[34]=& Cyc_Toc__pop_dynregion_str,((
_tmpC8C[35]=& Cyc_Toc__reset_region_str,((_tmpC8C[36]=& Cyc_Toc__throw_arraybounds_str,((
_tmpC8C[37]=& Cyc_Toc__dyneither_ptr_decrease_size_str,_tmpC8C)))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))),
sizeof(struct _dyneither_ptr*),38);};_pop_dynregion(d);}}struct Cyc_List_List*Cyc_Toc_toc(
struct Cyc_List_List*ds);struct Cyc_List_List*Cyc_Toc_toc(struct Cyc_List_List*ds){
Cyc_Toc_init();{struct _RegionHandle _tmp89B=_new_region("start");struct
_RegionHandle*start=& _tmp89B;_push_region(start);Cyc_Toc_decls_to_c(start,Cyc_Toc_empty_env(
start),ds,1,0);{struct _DynRegionHandle*_tmp89D;struct Cyc_Toc_TocState _tmp89C=*((
struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp89D=_tmp89C.dyn;Cyc_Core_free_dynregion(
_tmp89D);}{struct Cyc_List_List*_tmp89E=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(Cyc_Toc_result_decls);_npop_handler(0);return _tmp89E;};
_pop_region(start);}}
