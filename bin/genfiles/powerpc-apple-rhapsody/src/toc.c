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
struct Cyc_Absyn_Loc_n_struct{int tag;};struct Cyc_Absyn_Rel_n_struct{int tag;struct
Cyc_List_List*f1;};struct Cyc_Absyn_Abs_n_struct{int tag;struct Cyc_List_List*f1;};
union Cyc_Absyn_Nmspace_union{struct Cyc_Absyn_Loc_n_struct Loc_n;struct Cyc_Absyn_Rel_n_struct
Rel_n;struct Cyc_Absyn_Abs_n_struct Abs_n;};struct _tuple1{union Cyc_Absyn_Nmspace_union
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
Cyc_Absyn_UnknownDatatypeInfo{struct _tuple1*name;int is_extensible;int is_flat;};
struct Cyc_Absyn_UnknownDatatype_struct{int tag;struct Cyc_Absyn_UnknownDatatypeInfo
f1;};struct Cyc_Absyn_KnownDatatype_struct{int tag;struct Cyc_Absyn_Datatypedecl**
f1;};union Cyc_Absyn_DatatypeInfoU_union{struct Cyc_Absyn_UnknownDatatype_struct
UnknownDatatype;struct Cyc_Absyn_KnownDatatype_struct KnownDatatype;};struct Cyc_Absyn_DatatypeInfo{
union Cyc_Absyn_DatatypeInfoU_union datatype_info;struct Cyc_List_List*targs;struct
Cyc_Core_Opt*rgn;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple1*
datatype_name;struct _tuple1*field_name;int is_extensible;};struct Cyc_Absyn_UnknownDatatypefield_struct{
int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo f1;};struct Cyc_Absyn_KnownDatatypefield_struct{
int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};union
Cyc_Absyn_DatatypeFieldInfoU_union{struct Cyc_Absyn_UnknownDatatypefield_struct
UnknownDatatypefield;struct Cyc_Absyn_KnownDatatypefield_struct KnownDatatypefield;
};struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU_union
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
_dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];int Cyc_Absyn_qvar_cmp(
struct _tuple1*,struct _tuple1*);struct Cyc_Absyn_Conref*Cyc_Absyn_compress_conref(
struct Cyc_Absyn_Conref*x);void*Cyc_Absyn_conref_val(struct Cyc_Absyn_Conref*x);
void*Cyc_Absyn_conref_def(void*,struct Cyc_Absyn_Conref*x);extern struct Cyc_Absyn_Conref*
Cyc_Absyn_true_conref;extern struct Cyc_Absyn_Conref*Cyc_Absyn_false_conref;extern
void*Cyc_Absyn_char_typ;extern void*Cyc_Absyn_uint_typ;extern void*Cyc_Absyn_sint_typ;
extern void*Cyc_Absyn_exn_typ;extern void*Cyc_Absyn_bounds_one;void*Cyc_Absyn_star_typ(
void*t,void*rgn,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Conref*zero_term);void*
Cyc_Absyn_cstar_typ(void*t,struct Cyc_Absyn_Tqual tq);void*Cyc_Absyn_dyneither_typ(
void*t,void*rgn,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Conref*zero_term);void*
Cyc_Absyn_void_star_typ();void*Cyc_Absyn_strct(struct _dyneither_ptr*name);void*
Cyc_Absyn_strctq(struct _tuple1*name);void*Cyc_Absyn_unionq_typ(struct _tuple1*
name);void*Cyc_Absyn_array_typ(void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*
num_elts,struct Cyc_Absyn_Conref*zero_term,struct Cyc_Position_Segment*ztloc);
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,struct Cyc_Position_Segment*);struct
Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(
union Cyc_Absyn_Cnst_union,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_null_exp(
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(void*,int,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_signed_int_exp(int,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_char_exp(char c,struct
Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_float_exp(struct
_dyneither_ptr f,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(
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
int);struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfoU_union
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
e1,void**ptr_type,int*is_dyneither,void**elt_type);struct _tuple4{struct Cyc_Absyn_Tqual
f1;void*f2;};void*Cyc_Tcutil_snd_tqt(struct _tuple4*);struct _tuple5{unsigned int
f1;int f2;};struct _tuple5 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);int
Cyc_Evexp_c_can_eval(struct Cyc_Absyn_Exp*e);int Cyc_Evexp_same_const_exp(struct
Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);int Cyc_Evexp_lte_const_exp(struct Cyc_Absyn_Exp*
e1,struct Cyc_Absyn_Exp*e2);struct Cyc_CfFlowInfo_VarRoot_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_CfFlowInfo_MallocPt_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;
};struct Cyc_CfFlowInfo_InitParam_struct{int tag;int f1;void*f2;};struct Cyc_CfFlowInfo_Place{
void*root;struct Cyc_List_List*fields;};struct Cyc_CfFlowInfo_UniquePlace{struct
Cyc_CfFlowInfo_Place place;struct Cyc_List_List*path;};struct Cyc_CfFlowInfo_EqualConst_struct{
int tag;unsigned int f1;};struct Cyc_CfFlowInfo_LessVar_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;void*f2;};struct Cyc_CfFlowInfo_LessNumelts_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_CfFlowInfo_LessConst_struct{int tag;unsigned int f1;};struct Cyc_CfFlowInfo_LessEqNumelts_struct{
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
Cyc_CfFlowInfo_Aggregate_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_CfFlowInfo_ConsumeInfo{
struct Cyc_Dict_Dict consumed;struct Cyc_List_List*may_consume;};struct Cyc_CfFlowInfo_BottomFL_struct{
int tag;};struct Cyc_CfFlowInfo_ReachableFL_struct{int tag;struct Cyc_Dict_Dict f1;
struct Cyc_List_List*f2;struct Cyc_CfFlowInfo_ConsumeInfo f3;};union Cyc_CfFlowInfo_FlowInfo_union{
struct Cyc_CfFlowInfo_BottomFL_struct BottomFL;struct Cyc_CfFlowInfo_ReachableFL_struct
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
temp_labels;};static struct Cyc_Toc_TocState*Cyc_Toc_toc_state=0;struct _tuple6{
struct _tuple1*f1;struct _dyneither_ptr f2;};int Cyc_Toc_qvar_tag_cmp(struct _tuple6*
x,struct _tuple6*y);int Cyc_Toc_qvar_tag_cmp(struct _tuple6*x,struct _tuple6*y){
struct _tuple1*_tmp1;struct _dyneither_ptr _tmp2;struct _tuple6 _tmp0=*x;_tmp1=_tmp0.f1;
_tmp2=_tmp0.f2;{struct _tuple1*_tmp4;struct _dyneither_ptr _tmp5;struct _tuple6 _tmp3=*
y;_tmp4=_tmp3.f1;_tmp5=_tmp3.f2;{int i=Cyc_Absyn_qvar_cmp(_tmp1,_tmp4);if(i != 0)
return i;return Cyc_strcmp((struct _dyneither_ptr)_tmp2,(struct _dyneither_ptr)_tmp5);}}}
struct _tuple7{struct Cyc_Absyn_Aggrdecl*f1;void*f2;};void*Cyc_Toc_aggrdecl_type(
struct _tuple1*q,void*(*type_maker)(struct _tuple1*));void*Cyc_Toc_aggrdecl_type(
struct _tuple1*q,void*(*type_maker)(struct _tuple1*)){struct _DynRegionHandle*_tmp7;
struct Cyc_Dict_Dict*_tmp8;struct Cyc_Toc_TocState _tmp6=*((struct Cyc_Toc_TocState*)
_check_null(Cyc_Toc_toc_state));_tmp7=_tmp6.dyn;_tmp8=_tmp6.aggrs_so_far;{struct
_DynRegionFrame _tmp9;struct _RegionHandle*d=_open_dynregion(& _tmp9,_tmp7);{struct
_tuple7**v=((struct _tuple7**(*)(struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup_opt)(*
_tmp8,q);if(v == 0){void*_tmpA=type_maker(q);_npop_handler(0);return _tmpA;}else{
struct _tuple7 _tmpC;void*_tmpD;struct _tuple7*_tmpB=*v;_tmpC=*_tmpB;_tmpD=_tmpC.f2;{
void*_tmpE=_tmpD;_npop_handler(0);return _tmpE;}}};_pop_dynregion(d);}}static int
Cyc_Toc_tuple_type_counter=0;static int Cyc_Toc_temp_var_counter=0;static int Cyc_Toc_fresh_label_counter=
0;char Cyc_Toc_Toc_Unimplemented[22]="\000\000\000\000Toc_Unimplemented\000";char
Cyc_Toc_Toc_Impossible[19]="\000\000\000\000Toc_Impossible\000";static void*Cyc_Toc_unimp(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap);static void*Cyc_Toc_unimp(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap){Cyc_vfprintf(Cyc_stderr,fmt,ap);{const
char*_tmp8D5;void*_tmp8D4;(_tmp8D4=0,Cyc_fprintf(Cyc_stderr,((_tmp8D5="\n",
_tag_dyneither(_tmp8D5,sizeof(char),2))),_tag_dyneither(_tmp8D4,sizeof(void*),0)));}
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);(int)_throw((void*)Cyc_Toc_Toc_Unimplemented);}
static void*Cyc_Toc_toc_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
static void*Cyc_Toc_toc_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){Cyc_vfprintf(
Cyc_stderr,fmt,ap);{const char*_tmp8D8;void*_tmp8D7;(_tmp8D7=0,Cyc_fprintf(Cyc_stderr,((
_tmp8D8="\n",_tag_dyneither(_tmp8D8,sizeof(char),2))),_tag_dyneither(_tmp8D7,
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
_tmp1B + 7};static struct _tuple1 Cyc_Toc__throw_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__throw_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__throw_re={1,& Cyc_Toc__throw_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp
Cyc_Toc__throw_ev={0,(void*)((void*)& Cyc_Toc__throw_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__throw_e=& Cyc_Toc__throw_ev;static char _tmp1E[7]="setjmp";
static struct _dyneither_ptr Cyc_Toc_setjmp_str={_tmp1E,_tmp1E,_tmp1E + 7};static
struct _tuple1 Cyc_Toc_setjmp_pr={(union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){
0}),& Cyc_Toc_setjmp_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc_setjmp_re={1,&
Cyc_Toc_setjmp_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc_setjmp_ev={
0,(void*)((void*)& Cyc_Toc_setjmp_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc_setjmp_e=& Cyc_Toc_setjmp_ev;static char _tmp21[
14]="_push_handler";static struct _dyneither_ptr Cyc_Toc__push_handler_str={_tmp21,
_tmp21,_tmp21 + 14};static struct _tuple1 Cyc_Toc__push_handler_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__push_handler_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__push_handler_re={1,& Cyc_Toc__push_handler_pr,(void*)((void*)0)};static
struct Cyc_Absyn_Exp Cyc_Toc__push_handler_ev={0,(void*)((void*)& Cyc_Toc__push_handler_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__push_handler_e=&
Cyc_Toc__push_handler_ev;static char _tmp24[13]="_pop_handler";static struct
_dyneither_ptr Cyc_Toc__pop_handler_str={_tmp24,_tmp24,_tmp24 + 13};static struct
_tuple1 Cyc_Toc__pop_handler_pr={(union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){
0}),& Cyc_Toc__pop_handler_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__pop_handler_re={
1,& Cyc_Toc__pop_handler_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__pop_handler_ev={
0,(void*)((void*)& Cyc_Toc__pop_handler_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__pop_handler_e=& Cyc_Toc__pop_handler_ev;static
char _tmp27[12]="_exn_thrown";static struct _dyneither_ptr Cyc_Toc__exn_thrown_str={
_tmp27,_tmp27,_tmp27 + 12};static struct _tuple1 Cyc_Toc__exn_thrown_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__exn_thrown_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__exn_thrown_re={1,& Cyc_Toc__exn_thrown_pr,(void*)((void*)0)};static
struct Cyc_Absyn_Exp Cyc_Toc__exn_thrown_ev={0,(void*)((void*)& Cyc_Toc__exn_thrown_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__exn_thrown_e=&
Cyc_Toc__exn_thrown_ev;static char _tmp2A[14]="_npop_handler";static struct
_dyneither_ptr Cyc_Toc__npop_handler_str={_tmp2A,_tmp2A,_tmp2A + 14};static struct
_tuple1 Cyc_Toc__npop_handler_pr={(union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){
0}),& Cyc_Toc__npop_handler_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__npop_handler_re={
1,& Cyc_Toc__npop_handler_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__npop_handler_ev={
0,(void*)((void*)& Cyc_Toc__npop_handler_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__npop_handler_e=& Cyc_Toc__npop_handler_ev;
static char _tmp2D[12]="_check_null";static struct _dyneither_ptr Cyc_Toc__check_null_str={
_tmp2D,_tmp2D,_tmp2D + 12};static struct _tuple1 Cyc_Toc__check_null_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__check_null_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__check_null_re={1,& Cyc_Toc__check_null_pr,(void*)((void*)0)};static
struct Cyc_Absyn_Exp Cyc_Toc__check_null_ev={0,(void*)((void*)& Cyc_Toc__check_null_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__check_null_e=&
Cyc_Toc__check_null_ev;static char _tmp30[28]="_check_known_subscript_null";static
struct _dyneither_ptr Cyc_Toc__check_known_subscript_null_str={_tmp30,_tmp30,
_tmp30 + 28};static struct _tuple1 Cyc_Toc__check_known_subscript_null_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__check_known_subscript_null_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__check_known_subscript_null_re={1,& Cyc_Toc__check_known_subscript_null_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__check_known_subscript_null_ev={
0,(void*)((void*)& Cyc_Toc__check_known_subscript_null_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__check_known_subscript_null_e=& Cyc_Toc__check_known_subscript_null_ev;
static char _tmp33[31]="_check_known_subscript_notnull";static struct _dyneither_ptr
Cyc_Toc__check_known_subscript_notnull_str={_tmp33,_tmp33,_tmp33 + 31};static
struct _tuple1 Cyc_Toc__check_known_subscript_notnull_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__check_known_subscript_notnull_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__check_known_subscript_notnull_re={1,&
Cyc_Toc__check_known_subscript_notnull_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp
Cyc_Toc__check_known_subscript_notnull_ev={0,(void*)((void*)& Cyc_Toc__check_known_subscript_notnull_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__check_known_subscript_notnull_e=&
Cyc_Toc__check_known_subscript_notnull_ev;static char _tmp36[27]="_check_dyneither_subscript";
static struct _dyneither_ptr Cyc_Toc__check_dyneither_subscript_str={_tmp36,_tmp36,
_tmp36 + 27};static struct _tuple1 Cyc_Toc__check_dyneither_subscript_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__check_dyneither_subscript_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__check_dyneither_subscript_re={1,& Cyc_Toc__check_dyneither_subscript_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__check_dyneither_subscript_ev={
0,(void*)((void*)& Cyc_Toc__check_dyneither_subscript_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__check_dyneither_subscript_e=& Cyc_Toc__check_dyneither_subscript_ev;
static char _tmp39[15]="_dyneither_ptr";static struct _dyneither_ptr Cyc_Toc__dyneither_ptr_str={
_tmp39,_tmp39,_tmp39 + 15};static struct _tuple1 Cyc_Toc__dyneither_ptr_pr={(union
Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__dyneither_ptr_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__dyneither_ptr_re={1,& Cyc_Toc__dyneither_ptr_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_ev={0,(void*)((
void*)& Cyc_Toc__dyneither_ptr_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static
struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_e=& Cyc_Toc__dyneither_ptr_ev;static
char _tmp3C[15]="_tag_dyneither";static struct _dyneither_ptr Cyc_Toc__tag_dyneither_str={
_tmp3C,_tmp3C,_tmp3C + 15};static struct _tuple1 Cyc_Toc__tag_dyneither_pr={(union
Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__tag_dyneither_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__tag_dyneither_re={1,& Cyc_Toc__tag_dyneither_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__tag_dyneither_ev={0,(void*)((
void*)& Cyc_Toc__tag_dyneither_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static
struct Cyc_Absyn_Exp*Cyc_Toc__tag_dyneither_e=& Cyc_Toc__tag_dyneither_ev;static
char _tmp3F[20]="_init_dyneither_ptr";static struct _dyneither_ptr Cyc_Toc__init_dyneither_ptr_str={
_tmp3F,_tmp3F,_tmp3F + 20};static struct _tuple1 Cyc_Toc__init_dyneither_ptr_pr={(
union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__init_dyneither_ptr_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__init_dyneither_ptr_re={1,& Cyc_Toc__init_dyneither_ptr_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__init_dyneither_ptr_ev={0,(
void*)((void*)& Cyc_Toc__init_dyneither_ptr_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__init_dyneither_ptr_e=& Cyc_Toc__init_dyneither_ptr_ev;
static char _tmp42[21]="_untag_dyneither_ptr";static struct _dyneither_ptr Cyc_Toc__untag_dyneither_ptr_str={
_tmp42,_tmp42,_tmp42 + 21};static struct _tuple1 Cyc_Toc__untag_dyneither_ptr_pr={(
union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__untag_dyneither_ptr_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__untag_dyneither_ptr_re={1,& Cyc_Toc__untag_dyneither_ptr_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__untag_dyneither_ptr_ev={0,(
void*)((void*)& Cyc_Toc__untag_dyneither_ptr_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__untag_dyneither_ptr_e=& Cyc_Toc__untag_dyneither_ptr_ev;
static char _tmp45[20]="_get_dyneither_size";static struct _dyneither_ptr Cyc_Toc__get_dyneither_size_str={
_tmp45,_tmp45,_tmp45 + 20};static struct _tuple1 Cyc_Toc__get_dyneither_size_pr={(
union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__get_dyneither_size_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__get_dyneither_size_re={1,& Cyc_Toc__get_dyneither_size_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__get_dyneither_size_ev={0,(
void*)((void*)& Cyc_Toc__get_dyneither_size_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__get_dyneither_size_e=& Cyc_Toc__get_dyneither_size_ev;
static char _tmp48[19]="_get_zero_arr_size";static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_str={
_tmp48,_tmp48,_tmp48 + 19};static struct _tuple1 Cyc_Toc__get_zero_arr_size_pr={(
union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__get_zero_arr_size_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__get_zero_arr_size_re={1,& Cyc_Toc__get_zero_arr_size_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_ev={0,(
void*)((void*)& Cyc_Toc__get_zero_arr_size_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_e=& Cyc_Toc__get_zero_arr_size_ev;
static char _tmp4B[24]="_get_zero_arr_size_char";static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_char_str={
_tmp4B,_tmp4B,_tmp4B + 24};static struct _tuple1 Cyc_Toc__get_zero_arr_size_char_pr={(
union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__get_zero_arr_size_char_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__get_zero_arr_size_char_re={1,& Cyc_Toc__get_zero_arr_size_char_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_char_ev={0,(
void*)((void*)& Cyc_Toc__get_zero_arr_size_char_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_char_e=& Cyc_Toc__get_zero_arr_size_char_ev;
static char _tmp4E[25]="_get_zero_arr_size_short";static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_short_str={
_tmp4E,_tmp4E,_tmp4E + 25};static struct _tuple1 Cyc_Toc__get_zero_arr_size_short_pr={(
union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__get_zero_arr_size_short_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__get_zero_arr_size_short_re={1,& Cyc_Toc__get_zero_arr_size_short_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_short_ev={
0,(void*)((void*)& Cyc_Toc__get_zero_arr_size_short_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_short_e=& Cyc_Toc__get_zero_arr_size_short_ev;
static char _tmp51[23]="_get_zero_arr_size_int";static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_int_str={
_tmp51,_tmp51,_tmp51 + 23};static struct _tuple1 Cyc_Toc__get_zero_arr_size_int_pr={(
union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__get_zero_arr_size_int_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__get_zero_arr_size_int_re={1,& Cyc_Toc__get_zero_arr_size_int_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_int_ev={0,(
void*)((void*)& Cyc_Toc__get_zero_arr_size_int_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_int_e=& Cyc_Toc__get_zero_arr_size_int_ev;
static char _tmp54[25]="_get_zero_arr_size_float";static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_float_str={
_tmp54,_tmp54,_tmp54 + 25};static struct _tuple1 Cyc_Toc__get_zero_arr_size_float_pr={(
union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__get_zero_arr_size_float_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__get_zero_arr_size_float_re={1,& Cyc_Toc__get_zero_arr_size_float_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_float_ev={
0,(void*)((void*)& Cyc_Toc__get_zero_arr_size_float_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_float_e=& Cyc_Toc__get_zero_arr_size_float_ev;
static char _tmp57[26]="_get_zero_arr_size_double";static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_double_str={
_tmp57,_tmp57,_tmp57 + 26};static struct _tuple1 Cyc_Toc__get_zero_arr_size_double_pr={(
union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__get_zero_arr_size_double_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__get_zero_arr_size_double_re={1,& Cyc_Toc__get_zero_arr_size_double_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_double_ev={
0,(void*)((void*)& Cyc_Toc__get_zero_arr_size_double_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_double_e=& Cyc_Toc__get_zero_arr_size_double_ev;
static char _tmp5A[30]="_get_zero_arr_size_longdouble";static struct _dyneither_ptr
Cyc_Toc__get_zero_arr_size_longdouble_str={_tmp5A,_tmp5A,_tmp5A + 30};static
struct _tuple1 Cyc_Toc__get_zero_arr_size_longdouble_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__get_zero_arr_size_longdouble_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__get_zero_arr_size_longdouble_re={1,&
Cyc_Toc__get_zero_arr_size_longdouble_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp
Cyc_Toc__get_zero_arr_size_longdouble_ev={0,(void*)((void*)& Cyc_Toc__get_zero_arr_size_longdouble_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_longdouble_e=&
Cyc_Toc__get_zero_arr_size_longdouble_ev;static char _tmp5D[28]="_get_zero_arr_size_voidstar";
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_voidstar_str={_tmp5D,_tmp5D,
_tmp5D + 28};static struct _tuple1 Cyc_Toc__get_zero_arr_size_voidstar_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__get_zero_arr_size_voidstar_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__get_zero_arr_size_voidstar_re={1,& Cyc_Toc__get_zero_arr_size_voidstar_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_voidstar_ev={
0,(void*)((void*)& Cyc_Toc__get_zero_arr_size_voidstar_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_voidstar_e=& Cyc_Toc__get_zero_arr_size_voidstar_ev;
static char _tmp60[20]="_dyneither_ptr_plus";static struct _dyneither_ptr Cyc_Toc__dyneither_ptr_plus_str={
_tmp60,_tmp60,_tmp60 + 20};static struct _tuple1 Cyc_Toc__dyneither_ptr_plus_pr={(
union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__dyneither_ptr_plus_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__dyneither_ptr_plus_re={1,& Cyc_Toc__dyneither_ptr_plus_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_plus_ev={0,(
void*)((void*)& Cyc_Toc__dyneither_ptr_plus_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_plus_e=& Cyc_Toc__dyneither_ptr_plus_ev;
static char _tmp63[15]="_zero_arr_plus";static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_str={
_tmp63,_tmp63,_tmp63 + 15};static struct _tuple1 Cyc_Toc__zero_arr_plus_pr={(union
Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__zero_arr_plus_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_plus_re={1,& Cyc_Toc__zero_arr_plus_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_ev={0,(void*)((
void*)& Cyc_Toc__zero_arr_plus_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static
struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_e=& Cyc_Toc__zero_arr_plus_ev;static
char _tmp66[20]="_zero_arr_plus_char";static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_char_str={
_tmp66,_tmp66,_tmp66 + 20};static struct _tuple1 Cyc_Toc__zero_arr_plus_char_pr={(
union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__zero_arr_plus_char_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_plus_char_re={1,& Cyc_Toc__zero_arr_plus_char_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_char_ev={0,(
void*)((void*)& Cyc_Toc__zero_arr_plus_char_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_char_e=& Cyc_Toc__zero_arr_plus_char_ev;
static char _tmp69[21]="_zero_arr_plus_short";static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_short_str={
_tmp69,_tmp69,_tmp69 + 21};static struct _tuple1 Cyc_Toc__zero_arr_plus_short_pr={(
union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__zero_arr_plus_short_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_plus_short_re={1,& Cyc_Toc__zero_arr_plus_short_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_short_ev={0,(
void*)((void*)& Cyc_Toc__zero_arr_plus_short_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_short_e=& Cyc_Toc__zero_arr_plus_short_ev;
static char _tmp6C[19]="_zero_arr_plus_int";static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_int_str={
_tmp6C,_tmp6C,_tmp6C + 19};static struct _tuple1 Cyc_Toc__zero_arr_plus_int_pr={(
union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__zero_arr_plus_int_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_plus_int_re={1,& Cyc_Toc__zero_arr_plus_int_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_int_ev={0,(
void*)((void*)& Cyc_Toc__zero_arr_plus_int_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_int_e=& Cyc_Toc__zero_arr_plus_int_ev;
static char _tmp6F[21]="_zero_arr_plus_float";static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_float_str={
_tmp6F,_tmp6F,_tmp6F + 21};static struct _tuple1 Cyc_Toc__zero_arr_plus_float_pr={(
union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__zero_arr_plus_float_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_plus_float_re={1,& Cyc_Toc__zero_arr_plus_float_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_float_ev={0,(
void*)((void*)& Cyc_Toc__zero_arr_plus_float_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_float_e=& Cyc_Toc__zero_arr_plus_float_ev;
static char _tmp72[22]="_zero_arr_plus_double";static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_double_str={
_tmp72,_tmp72,_tmp72 + 22};static struct _tuple1 Cyc_Toc__zero_arr_plus_double_pr={(
union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__zero_arr_plus_double_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_plus_double_re={1,& Cyc_Toc__zero_arr_plus_double_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_double_ev={0,(
void*)((void*)& Cyc_Toc__zero_arr_plus_double_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_double_e=& Cyc_Toc__zero_arr_plus_double_ev;
static char _tmp75[26]="_zero_arr_plus_longdouble";static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_longdouble_str={
_tmp75,_tmp75,_tmp75 + 26};static struct _tuple1 Cyc_Toc__zero_arr_plus_longdouble_pr={(
union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__zero_arr_plus_longdouble_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_plus_longdouble_re={1,& Cyc_Toc__zero_arr_plus_longdouble_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_longdouble_ev={
0,(void*)((void*)& Cyc_Toc__zero_arr_plus_longdouble_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_longdouble_e=& Cyc_Toc__zero_arr_plus_longdouble_ev;
static char _tmp78[24]="_zero_arr_plus_voidstar";static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_voidstar_str={
_tmp78,_tmp78,_tmp78 + 24};static struct _tuple1 Cyc_Toc__zero_arr_plus_voidstar_pr={(
union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__zero_arr_plus_voidstar_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_plus_voidstar_re={1,& Cyc_Toc__zero_arr_plus_voidstar_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_voidstar_ev={0,(
void*)((void*)& Cyc_Toc__zero_arr_plus_voidstar_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_voidstar_e=& Cyc_Toc__zero_arr_plus_voidstar_ev;
static char _tmp7B[28]="_dyneither_ptr_inplace_plus";static struct _dyneither_ptr Cyc_Toc__dyneither_ptr_inplace_plus_str={
_tmp7B,_tmp7B,_tmp7B + 28};static struct _tuple1 Cyc_Toc__dyneither_ptr_inplace_plus_pr={(
union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__dyneither_ptr_inplace_plus_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__dyneither_ptr_inplace_plus_re={1,& Cyc_Toc__dyneither_ptr_inplace_plus_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_inplace_plus_ev={
0,(void*)((void*)& Cyc_Toc__dyneither_ptr_inplace_plus_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_inplace_plus_e=& Cyc_Toc__dyneither_ptr_inplace_plus_ev;
static char _tmp7E[23]="_zero_arr_inplace_plus";static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_str={
_tmp7E,_tmp7E,_tmp7E + 23};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_pr={(
union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__zero_arr_inplace_plus_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_inplace_plus_re={1,& Cyc_Toc__zero_arr_inplace_plus_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_ev={0,(
void*)((void*)& Cyc_Toc__zero_arr_inplace_plus_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_e=& Cyc_Toc__zero_arr_inplace_plus_ev;
static char _tmp81[28]="_zero_arr_inplace_plus_char";static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_char_str={
_tmp81,_tmp81,_tmp81 + 28};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_char_pr={(
union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__zero_arr_inplace_plus_char_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_inplace_plus_char_re={1,& Cyc_Toc__zero_arr_inplace_plus_char_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_char_ev={
0,(void*)((void*)& Cyc_Toc__zero_arr_inplace_plus_char_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_char_e=& Cyc_Toc__zero_arr_inplace_plus_char_ev;
static char _tmp84[29]="_zero_arr_inplace_plus_short";static struct _dyneither_ptr
Cyc_Toc__zero_arr_inplace_plus_short_str={_tmp84,_tmp84,_tmp84 + 29};static struct
_tuple1 Cyc_Toc__zero_arr_inplace_plus_short_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__zero_arr_inplace_plus_short_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_inplace_plus_short_re={1,&
Cyc_Toc__zero_arr_inplace_plus_short_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp
Cyc_Toc__zero_arr_inplace_plus_short_ev={0,(void*)((void*)& Cyc_Toc__zero_arr_inplace_plus_short_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_short_e=&
Cyc_Toc__zero_arr_inplace_plus_short_ev;static char _tmp87[27]="_zero_arr_inplace_plus_int";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_int_str={_tmp87,_tmp87,
_tmp87 + 27};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_int_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__zero_arr_inplace_plus_int_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_inplace_plus_int_re={1,& Cyc_Toc__zero_arr_inplace_plus_int_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_int_ev={
0,(void*)((void*)& Cyc_Toc__zero_arr_inplace_plus_int_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_int_e=& Cyc_Toc__zero_arr_inplace_plus_int_ev;
static char _tmp8A[29]="_zero_arr_inplace_plus_float";static struct _dyneither_ptr
Cyc_Toc__zero_arr_inplace_plus_float_str={_tmp8A,_tmp8A,_tmp8A + 29};static struct
_tuple1 Cyc_Toc__zero_arr_inplace_plus_float_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__zero_arr_inplace_plus_float_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_inplace_plus_float_re={1,&
Cyc_Toc__zero_arr_inplace_plus_float_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp
Cyc_Toc__zero_arr_inplace_plus_float_ev={0,(void*)((void*)& Cyc_Toc__zero_arr_inplace_plus_float_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_float_e=&
Cyc_Toc__zero_arr_inplace_plus_float_ev;static char _tmp8D[30]="_zero_arr_inplace_plus_double";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_double_str={_tmp8D,
_tmp8D,_tmp8D + 30};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_double_pr={(
union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__zero_arr_inplace_plus_double_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_inplace_plus_double_re={1,&
Cyc_Toc__zero_arr_inplace_plus_double_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp
Cyc_Toc__zero_arr_inplace_plus_double_ev={0,(void*)((void*)& Cyc_Toc__zero_arr_inplace_plus_double_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_double_e=&
Cyc_Toc__zero_arr_inplace_plus_double_ev;static char _tmp90[34]="_zero_arr_inplace_plus_longdouble";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_longdouble_str={_tmp90,
_tmp90,_tmp90 + 34};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_longdouble_pr={(
union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__zero_arr_inplace_plus_longdouble_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_inplace_plus_longdouble_re={
1,& Cyc_Toc__zero_arr_inplace_plus_longdouble_pr,(void*)((void*)0)};static struct
Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_longdouble_ev={0,(void*)((void*)& Cyc_Toc__zero_arr_inplace_plus_longdouble_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_longdouble_e=&
Cyc_Toc__zero_arr_inplace_plus_longdouble_ev;static char _tmp93[32]="_zero_arr_inplace_plus_voidstar";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_voidstar_str={_tmp93,
_tmp93,_tmp93 + 32};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_voidstar_pr={(
union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__zero_arr_inplace_plus_voidstar_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_inplace_plus_voidstar_re={1,&
Cyc_Toc__zero_arr_inplace_plus_voidstar_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp
Cyc_Toc__zero_arr_inplace_plus_voidstar_ev={0,(void*)((void*)& Cyc_Toc__zero_arr_inplace_plus_voidstar_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_voidstar_e=&
Cyc_Toc__zero_arr_inplace_plus_voidstar_ev;static char _tmp96[33]="_dyneither_ptr_inplace_plus_post";
static struct _dyneither_ptr Cyc_Toc__dyneither_ptr_inplace_plus_post_str={_tmp96,
_tmp96,_tmp96 + 33};static struct _tuple1 Cyc_Toc__dyneither_ptr_inplace_plus_post_pr={(
union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__dyneither_ptr_inplace_plus_post_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__dyneither_ptr_inplace_plus_post_re={1,&
Cyc_Toc__dyneither_ptr_inplace_plus_post_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp
Cyc_Toc__dyneither_ptr_inplace_plus_post_ev={0,(void*)((void*)& Cyc_Toc__dyneither_ptr_inplace_plus_post_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_inplace_plus_post_e=&
Cyc_Toc__dyneither_ptr_inplace_plus_post_ev;static char _tmp99[28]="_zero_arr_inplace_plus_post";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_str={_tmp99,_tmp99,
_tmp99 + 28};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__zero_arr_inplace_plus_post_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_inplace_plus_post_re={1,& Cyc_Toc__zero_arr_inplace_plus_post_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_ev={
0,(void*)((void*)& Cyc_Toc__zero_arr_inplace_plus_post_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_e=& Cyc_Toc__zero_arr_inplace_plus_post_ev;
static char _tmp9C[33]="_zero_arr_inplace_plus_post_char";static struct
_dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_char_str={_tmp9C,_tmp9C,_tmp9C
+ 33};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_char_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__zero_arr_inplace_plus_post_char_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_inplace_plus_post_char_re={1,&
Cyc_Toc__zero_arr_inplace_plus_post_char_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp
Cyc_Toc__zero_arr_inplace_plus_post_char_ev={0,(void*)((void*)& Cyc_Toc__zero_arr_inplace_plus_post_char_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_char_e=&
Cyc_Toc__zero_arr_inplace_plus_post_char_ev;static char _tmp9F[34]="_zero_arr_inplace_plus_post_short";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_short_str={_tmp9F,
_tmp9F,_tmp9F + 34};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_short_pr={(
union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__zero_arr_inplace_plus_post_short_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_inplace_plus_post_short_re={
1,& Cyc_Toc__zero_arr_inplace_plus_post_short_pr,(void*)((void*)0)};static struct
Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_short_ev={0,(void*)((void*)& Cyc_Toc__zero_arr_inplace_plus_post_short_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_short_e=&
Cyc_Toc__zero_arr_inplace_plus_post_short_ev;static char _tmpA2[32]="_zero_arr_inplace_plus_post_int";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_int_str={_tmpA2,
_tmpA2,_tmpA2 + 32};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_int_pr={(
union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__zero_arr_inplace_plus_post_int_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_inplace_plus_post_int_re={1,&
Cyc_Toc__zero_arr_inplace_plus_post_int_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp
Cyc_Toc__zero_arr_inplace_plus_post_int_ev={0,(void*)((void*)& Cyc_Toc__zero_arr_inplace_plus_post_int_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_int_e=&
Cyc_Toc__zero_arr_inplace_plus_post_int_ev;static char _tmpA5[34]="_zero_arr_inplace_plus_post_float";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_float_str={_tmpA5,
_tmpA5,_tmpA5 + 34};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_float_pr={(
union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__zero_arr_inplace_plus_post_float_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_inplace_plus_post_float_re={
1,& Cyc_Toc__zero_arr_inplace_plus_post_float_pr,(void*)((void*)0)};static struct
Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_float_ev={0,(void*)((void*)& Cyc_Toc__zero_arr_inplace_plus_post_float_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_float_e=&
Cyc_Toc__zero_arr_inplace_plus_post_float_ev;static char _tmpA8[35]="_zero_arr_inplace_plus_post_double";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_double_str={_tmpA8,
_tmpA8,_tmpA8 + 35};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_double_pr={(
union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__zero_arr_inplace_plus_post_double_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_inplace_plus_post_double_re={
1,& Cyc_Toc__zero_arr_inplace_plus_post_double_pr,(void*)((void*)0)};static struct
Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_double_ev={0,(void*)((void*)&
Cyc_Toc__zero_arr_inplace_plus_post_double_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_double_e=& Cyc_Toc__zero_arr_inplace_plus_post_double_ev;
static char _tmpAB[39]="_zero_arr_inplace_plus_post_longdouble";static struct
_dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_longdouble_str={_tmpAB,_tmpAB,
_tmpAB + 39};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_longdouble_pr={(
union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_inplace_plus_post_longdouble_re={
1,& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_pr,(void*)((void*)0)};static
struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_longdouble_ev={0,(void*)((
void*)& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_longdouble_e=& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_ev;
static char _tmpAE[37]="_zero_arr_inplace_plus_post_voidstar";static struct
_dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_voidstar_str={_tmpAE,_tmpAE,
_tmpAE + 37};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_voidstar_pr={(
union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_inplace_plus_post_voidstar_re={
1,& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_pr,(void*)((void*)0)};static
struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_voidstar_ev={0,(void*)((
void*)& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_voidstar_e=& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_ev;
static char _tmpB1[10]="_cycalloc";static struct _dyneither_ptr Cyc_Toc__cycalloc_str={
_tmpB1,_tmpB1,_tmpB1 + 10};static struct _tuple1 Cyc_Toc__cycalloc_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__cycalloc_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__cycalloc_re={1,& Cyc_Toc__cycalloc_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp
Cyc_Toc__cycalloc_ev={0,(void*)((void*)& Cyc_Toc__cycalloc_re),0,(void*)((void*)
Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__cycalloc_e=& Cyc_Toc__cycalloc_ev;
static char _tmpB4[11]="_cyccalloc";static struct _dyneither_ptr Cyc_Toc__cyccalloc_str={
_tmpB4,_tmpB4,_tmpB4 + 11};static struct _tuple1 Cyc_Toc__cyccalloc_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__cyccalloc_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__cyccalloc_re={1,& Cyc_Toc__cyccalloc_pr,(void*)((void*)0)};static struct
Cyc_Absyn_Exp Cyc_Toc__cyccalloc_ev={0,(void*)((void*)& Cyc_Toc__cyccalloc_re),0,(
void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__cyccalloc_e=&
Cyc_Toc__cyccalloc_ev;static char _tmpB7[17]="_cycalloc_atomic";static struct
_dyneither_ptr Cyc_Toc__cycalloc_atomic_str={_tmpB7,_tmpB7,_tmpB7 + 17};static
struct _tuple1 Cyc_Toc__cycalloc_atomic_pr={(union Cyc_Absyn_Nmspace_union)((struct
Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__cycalloc_atomic_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__cycalloc_atomic_re={1,& Cyc_Toc__cycalloc_atomic_pr,(void*)((void*)0)};
static struct Cyc_Absyn_Exp Cyc_Toc__cycalloc_atomic_ev={0,(void*)((void*)& Cyc_Toc__cycalloc_atomic_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__cycalloc_atomic_e=&
Cyc_Toc__cycalloc_atomic_ev;static char _tmpBA[18]="_cyccalloc_atomic";static
struct _dyneither_ptr Cyc_Toc__cyccalloc_atomic_str={_tmpBA,_tmpBA,_tmpBA + 18};
static struct _tuple1 Cyc_Toc__cyccalloc_atomic_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__cyccalloc_atomic_str};static struct
Cyc_Absyn_Var_e_struct Cyc_Toc__cyccalloc_atomic_re={1,& Cyc_Toc__cyccalloc_atomic_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__cyccalloc_atomic_ev={0,(void*)((
void*)& Cyc_Toc__cyccalloc_atomic_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__cyccalloc_atomic_e=& Cyc_Toc__cyccalloc_atomic_ev;
static char _tmpBD[15]="_region_malloc";static struct _dyneither_ptr Cyc_Toc__region_malloc_str={
_tmpBD,_tmpBD,_tmpBD + 15};static struct _tuple1 Cyc_Toc__region_malloc_pr={(union
Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__region_malloc_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__region_malloc_re={1,& Cyc_Toc__region_malloc_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__region_malloc_ev={0,(void*)((
void*)& Cyc_Toc__region_malloc_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static
struct Cyc_Absyn_Exp*Cyc_Toc__region_malloc_e=& Cyc_Toc__region_malloc_ev;static
char _tmpC0[15]="_region_calloc";static struct _dyneither_ptr Cyc_Toc__region_calloc_str={
_tmpC0,_tmpC0,_tmpC0 + 15};static struct _tuple1 Cyc_Toc__region_calloc_pr={(union
Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__region_calloc_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__region_calloc_re={1,& Cyc_Toc__region_calloc_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__region_calloc_ev={0,(void*)((
void*)& Cyc_Toc__region_calloc_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static
struct Cyc_Absyn_Exp*Cyc_Toc__region_calloc_e=& Cyc_Toc__region_calloc_ev;static
char _tmpC3[13]="_check_times";static struct _dyneither_ptr Cyc_Toc__check_times_str={
_tmpC3,_tmpC3,_tmpC3 + 13};static struct _tuple1 Cyc_Toc__check_times_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__check_times_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__check_times_re={1,& Cyc_Toc__check_times_pr,(void*)((void*)0)};static
struct Cyc_Absyn_Exp Cyc_Toc__check_times_ev={0,(void*)((void*)& Cyc_Toc__check_times_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__check_times_e=&
Cyc_Toc__check_times_ev;static char _tmpC6[12]="_new_region";static struct
_dyneither_ptr Cyc_Toc__new_region_str={_tmpC6,_tmpC6,_tmpC6 + 12};static struct
_tuple1 Cyc_Toc__new_region_pr={(union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){
0}),& Cyc_Toc__new_region_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__new_region_re={
1,& Cyc_Toc__new_region_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__new_region_ev={
0,(void*)((void*)& Cyc_Toc__new_region_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__new_region_e=& Cyc_Toc__new_region_ev;static
char _tmpC9[13]="_push_region";static struct _dyneither_ptr Cyc_Toc__push_region_str={
_tmpC9,_tmpC9,_tmpC9 + 13};static struct _tuple1 Cyc_Toc__push_region_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__push_region_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__push_region_re={1,& Cyc_Toc__push_region_pr,(void*)((void*)0)};static
struct Cyc_Absyn_Exp Cyc_Toc__push_region_ev={0,(void*)((void*)& Cyc_Toc__push_region_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__push_region_e=&
Cyc_Toc__push_region_ev;static char _tmpCC[12]="_pop_region";static struct
_dyneither_ptr Cyc_Toc__pop_region_str={_tmpCC,_tmpCC,_tmpCC + 12};static struct
_tuple1 Cyc_Toc__pop_region_pr={(union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){
0}),& Cyc_Toc__pop_region_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__pop_region_re={
1,& Cyc_Toc__pop_region_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__pop_region_ev={
0,(void*)((void*)& Cyc_Toc__pop_region_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__pop_region_e=& Cyc_Toc__pop_region_ev;static
char _tmpCF[16]="_open_dynregion";static struct _dyneither_ptr Cyc_Toc__open_dynregion_str={
_tmpCF,_tmpCF,_tmpCF + 16};static struct _tuple1 Cyc_Toc__open_dynregion_pr={(union
Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__open_dynregion_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__open_dynregion_re={1,& Cyc_Toc__open_dynregion_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__open_dynregion_ev={0,(void*)((
void*)& Cyc_Toc__open_dynregion_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__open_dynregion_e=& Cyc_Toc__open_dynregion_ev;
static char _tmpD2[16]="_push_dynregion";static struct _dyneither_ptr Cyc_Toc__push_dynregion_str={
_tmpD2,_tmpD2,_tmpD2 + 16};static struct _tuple1 Cyc_Toc__push_dynregion_pr={(union
Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__push_dynregion_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__push_dynregion_re={1,& Cyc_Toc__push_dynregion_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__push_dynregion_ev={0,(void*)((
void*)& Cyc_Toc__push_dynregion_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__push_dynregion_e=& Cyc_Toc__push_dynregion_ev;
static char _tmpD5[15]="_pop_dynregion";static struct _dyneither_ptr Cyc_Toc__pop_dynregion_str={
_tmpD5,_tmpD5,_tmpD5 + 15};static struct _tuple1 Cyc_Toc__pop_dynregion_pr={(union
Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__pop_dynregion_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__pop_dynregion_re={1,& Cyc_Toc__pop_dynregion_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__pop_dynregion_ev={0,(void*)((
void*)& Cyc_Toc__pop_dynregion_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static
struct Cyc_Absyn_Exp*Cyc_Toc__pop_dynregion_e=& Cyc_Toc__pop_dynregion_ev;static
char _tmpD8[14]="_reset_region";static struct _dyneither_ptr Cyc_Toc__reset_region_str={
_tmpD8,_tmpD8,_tmpD8 + 14};static struct _tuple1 Cyc_Toc__reset_region_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__reset_region_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__reset_region_re={1,& Cyc_Toc__reset_region_pr,(void*)((void*)0)};static
struct Cyc_Absyn_Exp Cyc_Toc__reset_region_ev={0,(void*)((void*)& Cyc_Toc__reset_region_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__reset_region_e=&
Cyc_Toc__reset_region_ev;static char _tmpDB[19]="_throw_arraybounds";static struct
_dyneither_ptr Cyc_Toc__throw_arraybounds_str={_tmpDB,_tmpDB,_tmpDB + 19};static
struct _tuple1 Cyc_Toc__throw_arraybounds_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__throw_arraybounds_str};static struct
Cyc_Absyn_Var_e_struct Cyc_Toc__throw_arraybounds_re={1,& Cyc_Toc__throw_arraybounds_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__throw_arraybounds_ev={0,(
void*)((void*)& Cyc_Toc__throw_arraybounds_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__throw_arraybounds_e=& Cyc_Toc__throw_arraybounds_ev;
static char _tmpDE[29]="_dyneither_ptr_decrease_size";static struct _dyneither_ptr
Cyc_Toc__dyneither_ptr_decrease_size_str={_tmpDE,_tmpDE,_tmpDE + 29};static struct
_tuple1 Cyc_Toc__dyneither_ptr_decrease_size_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__dyneither_ptr_decrease_size_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__dyneither_ptr_decrease_size_re={1,&
Cyc_Toc__dyneither_ptr_decrease_size_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp
Cyc_Toc__dyneither_ptr_decrease_size_ev={0,(void*)((void*)& Cyc_Toc__dyneither_ptr_decrease_size_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_decrease_size_e=&
Cyc_Toc__dyneither_ptr_decrease_size_ev;static char _tmpE1[11]="_swap_word";static
struct _dyneither_ptr Cyc_Toc__swap_word_str={_tmpE1,_tmpE1,_tmpE1 + 11};static
struct _tuple1 Cyc_Toc__swap_word_pr={(union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){
0}),& Cyc_Toc__swap_word_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__swap_word_re={
1,& Cyc_Toc__swap_word_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__swap_word_ev={
0,(void*)((void*)& Cyc_Toc__swap_word_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__swap_word_e=& Cyc_Toc__swap_word_ev;static char
_tmpE4[16]="_swap_dyneither";static struct _dyneither_ptr Cyc_Toc__swap_dyneither_str={
_tmpE4,_tmpE4,_tmpE4 + 16};static struct _tuple1 Cyc_Toc__swap_dyneither_pr={(union
Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__swap_dyneither_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__swap_dyneither_re={1,& Cyc_Toc__swap_dyneither_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__swap_dyneither_ev={0,(void*)((
void*)& Cyc_Toc__swap_dyneither_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__swap_dyneither_e=& Cyc_Toc__swap_dyneither_ev;
static struct Cyc_Absyn_AggrType_struct Cyc_Toc_dyneither_ptr_typ_v={10,{(union Cyc_Absyn_AggrInfoU_union)((
struct Cyc_Absyn_UnknownAggr_struct){0,(void*)((void*)0),& Cyc_Toc__dyneither_ptr_pr}),
0}};static void*Cyc_Toc_dyneither_ptr_typ=(void*)& Cyc_Toc_dyneither_ptr_typ_v;
static struct Cyc_Absyn_Tqual Cyc_Toc_mt_tq={0,0,0,0,0};static struct Cyc_Absyn_Stmt*
Cyc_Toc_skip_stmt_dl();static struct Cyc_Absyn_Stmt*Cyc_Toc_skip_stmt_dl(){static
struct Cyc_Absyn_Stmt**skip_stmt_opt=0;if(skip_stmt_opt == 0){struct Cyc_Absyn_Stmt**
_tmp8D9;skip_stmt_opt=((_tmp8D9=_cycalloc(sizeof(*_tmp8D9)),((_tmp8D9[0]=Cyc_Absyn_skip_stmt(
0),_tmp8D9))));}return*skip_stmt_opt;}static struct Cyc_Absyn_Exp*Cyc_Toc_cast_it(
void*t,struct Cyc_Absyn_Exp*e);static struct Cyc_Absyn_Exp*Cyc_Toc_cast_it(void*t,
struct Cyc_Absyn_Exp*e){return Cyc_Absyn_cast_exp(t,e,0,(void*)1,0);}static void*
Cyc_Toc_cast_it_r(void*t,struct Cyc_Absyn_Exp*e);static void*Cyc_Toc_cast_it_r(
void*t,struct Cyc_Absyn_Exp*e){struct Cyc_Absyn_Cast_e_struct _tmp8DC;struct Cyc_Absyn_Cast_e_struct*
_tmp8DB;return(void*)((_tmp8DB=_cycalloc(sizeof(*_tmp8DB)),((_tmp8DB[0]=((
_tmp8DC.tag=15,((_tmp8DC.f1=(void*)t,((_tmp8DC.f2=e,((_tmp8DC.f3=0,((_tmp8DC.f4=(
void*)((void*)1),_tmp8DC)))))))))),_tmp8DB))));}static void*Cyc_Toc_deref_exp_r(
struct Cyc_Absyn_Exp*e);static void*Cyc_Toc_deref_exp_r(struct Cyc_Absyn_Exp*e){
struct Cyc_Absyn_Deref_e_struct _tmp8DF;struct Cyc_Absyn_Deref_e_struct*_tmp8DE;
return(void*)((_tmp8DE=_cycalloc(sizeof(*_tmp8DE)),((_tmp8DE[0]=((_tmp8DF.tag=22,((
_tmp8DF.f1=e,_tmp8DF)))),_tmp8DE))));}static void*Cyc_Toc_subscript_exp_r(struct
Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);static void*Cyc_Toc_subscript_exp_r(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){struct Cyc_Absyn_Subscript_e_struct
_tmp8E2;struct Cyc_Absyn_Subscript_e_struct*_tmp8E1;return(void*)((_tmp8E1=
_cycalloc(sizeof(*_tmp8E1)),((_tmp8E1[0]=((_tmp8E2.tag=25,((_tmp8E2.f1=e1,((
_tmp8E2.f2=e2,_tmp8E2)))))),_tmp8E1))));}static void*Cyc_Toc_stmt_exp_r(struct Cyc_Absyn_Stmt*
s);static void*Cyc_Toc_stmt_exp_r(struct Cyc_Absyn_Stmt*s){struct Cyc_Absyn_StmtExp_e_struct
_tmp8E5;struct Cyc_Absyn_StmtExp_e_struct*_tmp8E4;return(void*)((_tmp8E4=
_cycalloc(sizeof(*_tmp8E4)),((_tmp8E4[0]=((_tmp8E5.tag=38,((_tmp8E5.f1=s,_tmp8E5)))),
_tmp8E4))));}static void*Cyc_Toc_sizeoftyp_exp_r(void*t);static void*Cyc_Toc_sizeoftyp_exp_r(
void*t){struct Cyc_Absyn_Sizeoftyp_e_struct _tmp8E8;struct Cyc_Absyn_Sizeoftyp_e_struct*
_tmp8E7;return(void*)((_tmp8E7=_cycalloc(sizeof(*_tmp8E7)),((_tmp8E7[0]=((
_tmp8E8.tag=18,((_tmp8E8.f1=(void*)t,_tmp8E8)))),_tmp8E7))));}static void*Cyc_Toc_fncall_exp_r(
struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es);static void*Cyc_Toc_fncall_exp_r(
struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es){struct Cyc_Absyn_FnCall_e_struct
_tmp8EB;struct Cyc_Absyn_FnCall_e_struct*_tmp8EA;return(void*)((_tmp8EA=_cycalloc(
sizeof(*_tmp8EA)),((_tmp8EA[0]=((_tmp8EB.tag=11,((_tmp8EB.f1=e,((_tmp8EB.f2=es,((
_tmp8EB.f3=0,_tmp8EB)))))))),_tmp8EA))));}static void*Cyc_Toc_exp_stmt_r(struct
Cyc_Absyn_Exp*e);static void*Cyc_Toc_exp_stmt_r(struct Cyc_Absyn_Exp*e){struct Cyc_Absyn_Exp_s_struct
_tmp8EE;struct Cyc_Absyn_Exp_s_struct*_tmp8ED;return(void*)((_tmp8ED=_cycalloc(
sizeof(*_tmp8ED)),((_tmp8ED[0]=((_tmp8EE.tag=0,((_tmp8EE.f1=e,_tmp8EE)))),
_tmp8ED))));}static void*Cyc_Toc_seq_stmt_r(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*
s2);static void*Cyc_Toc_seq_stmt_r(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*
s2){struct Cyc_Absyn_Seq_s_struct _tmp8F1;struct Cyc_Absyn_Seq_s_struct*_tmp8F0;
return(void*)((_tmp8F0=_cycalloc(sizeof(*_tmp8F0)),((_tmp8F0[0]=((_tmp8F1.tag=1,((
_tmp8F1.f1=s1,((_tmp8F1.f2=s2,_tmp8F1)))))),_tmp8F0))));}static void*Cyc_Toc_conditional_exp_r(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3);static void*
Cyc_Toc_conditional_exp_r(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct
Cyc_Absyn_Exp*e3){struct Cyc_Absyn_Conditional_e_struct _tmp8F4;struct Cyc_Absyn_Conditional_e_struct*
_tmp8F3;return(void*)((_tmp8F3=_cycalloc(sizeof(*_tmp8F3)),((_tmp8F3[0]=((
_tmp8F4.tag=6,((_tmp8F4.f1=e1,((_tmp8F4.f2=e2,((_tmp8F4.f3=e3,_tmp8F4)))))))),
_tmp8F3))));}static void*Cyc_Toc_aggrmember_exp_r(struct Cyc_Absyn_Exp*e,struct
_dyneither_ptr*n);static void*Cyc_Toc_aggrmember_exp_r(struct Cyc_Absyn_Exp*e,
struct _dyneither_ptr*n){struct Cyc_Absyn_AggrMember_e_struct _tmp8F7;struct Cyc_Absyn_AggrMember_e_struct*
_tmp8F6;return(void*)((_tmp8F6=_cycalloc(sizeof(*_tmp8F6)),((_tmp8F6[0]=((
_tmp8F7.tag=23,((_tmp8F7.f1=e,((_tmp8F7.f2=n,((_tmp8F7.f3=0,((_tmp8F7.f4=0,
_tmp8F7)))))))))),_tmp8F6))));}static void*Cyc_Toc_aggrarrow_exp_r(struct Cyc_Absyn_Exp*
e,struct _dyneither_ptr*n);static void*Cyc_Toc_aggrarrow_exp_r(struct Cyc_Absyn_Exp*
e,struct _dyneither_ptr*n){struct Cyc_Absyn_AggrArrow_e_struct _tmp8FA;struct Cyc_Absyn_AggrArrow_e_struct*
_tmp8F9;return(void*)((_tmp8F9=_cycalloc(sizeof(*_tmp8F9)),((_tmp8F9[0]=((
_tmp8FA.tag=24,((_tmp8FA.f1=e,((_tmp8FA.f2=n,((_tmp8FA.f3=0,((_tmp8FA.f4=0,
_tmp8FA)))))))))),_tmp8F9))));}static void*Cyc_Toc_unresolvedmem_exp_r(struct Cyc_Core_Opt*
tdopt,struct Cyc_List_List*ds);static void*Cyc_Toc_unresolvedmem_exp_r(struct Cyc_Core_Opt*
tdopt,struct Cyc_List_List*ds){struct Cyc_Absyn_UnresolvedMem_e_struct _tmp8FD;
struct Cyc_Absyn_UnresolvedMem_e_struct*_tmp8FC;return(void*)((_tmp8FC=_cycalloc(
sizeof(*_tmp8FC)),((_tmp8FC[0]=((_tmp8FD.tag=37,((_tmp8FD.f1=tdopt,((_tmp8FD.f2=
ds,_tmp8FD)))))),_tmp8FC))));}static void*Cyc_Toc_goto_stmt_r(struct
_dyneither_ptr*v,struct Cyc_Absyn_Stmt*s);static void*Cyc_Toc_goto_stmt_r(struct
_dyneither_ptr*v,struct Cyc_Absyn_Stmt*s){struct Cyc_Absyn_Goto_s_struct _tmp900;
struct Cyc_Absyn_Goto_s_struct*_tmp8FF;return(void*)((_tmp8FF=_cycalloc(sizeof(*
_tmp8FF)),((_tmp8FF[0]=((_tmp900.tag=7,((_tmp900.f1=v,((_tmp900.f2=s,_tmp900)))))),
_tmp8FF))));}static struct Cyc_Absyn_Const_e_struct Cyc_Toc_zero_exp={0,(union Cyc_Absyn_Cnst_union)((
struct Cyc_Absyn_Int_c_struct){2,(void*)((void*)0),0})};struct Cyc_Toc_functionSet{
struct Cyc_Absyn_Exp*fchar;struct Cyc_Absyn_Exp*fshort;struct Cyc_Absyn_Exp*fint;
struct Cyc_Absyn_Exp*ffloat;struct Cyc_Absyn_Exp*fdouble;struct Cyc_Absyn_Exp*
flongdouble;struct Cyc_Absyn_Exp*fvoidstar;};struct Cyc_Toc_functionSet Cyc_Toc__zero_arr_plus_functionSet={&
Cyc_Toc__zero_arr_plus_char_ev,& Cyc_Toc__zero_arr_plus_short_ev,& Cyc_Toc__zero_arr_plus_int_ev,&
Cyc_Toc__zero_arr_plus_float_ev,& Cyc_Toc__zero_arr_plus_double_ev,& Cyc_Toc__zero_arr_plus_longdouble_ev,&
Cyc_Toc__zero_arr_plus_voidstar_ev};struct Cyc_Toc_functionSet Cyc_Toc__get_zero_arr_size_functionSet={&
Cyc_Toc__get_zero_arr_size_char_ev,& Cyc_Toc__get_zero_arr_size_short_ev,& Cyc_Toc__get_zero_arr_size_int_ev,&
Cyc_Toc__get_zero_arr_size_float_ev,& Cyc_Toc__get_zero_arr_size_double_ev,& Cyc_Toc__get_zero_arr_size_longdouble_ev,&
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
struct Cyc_Toc_functionSet*fS,void*t){struct Cyc_Absyn_Exp*function;{void*_tmp106=
t;void*_tmp107;int _tmp108;_LL1: if(_tmp106 <= (void*)4)goto _LL3;if(*((int*)_tmp106)
!= 5)goto _LL3;_tmp107=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp106)->f2;_LL2:{
void*_tmp109=_tmp107;_LLC: if((int)_tmp109 != 0)goto _LLE;_LLD: function=fS->fchar;
goto _LLB;_LLE: if((int)_tmp109 != 1)goto _LL10;_LLF: function=fS->fshort;goto _LLB;
_LL10: if((int)_tmp109 != 2)goto _LL12;_LL11: function=fS->fint;goto _LLB;_LL12:;
_LL13: {struct Cyc_Core_Impossible_struct _tmp906;const char*_tmp905;struct Cyc_Core_Impossible_struct*
_tmp904;(int)_throw((void*)((_tmp904=_cycalloc(sizeof(*_tmp904)),((_tmp904[0]=((
_tmp906.tag=Cyc_Core_Impossible,((_tmp906.f1=((_tmp905="impossible IntType (not char, short or int)",
_tag_dyneither(_tmp905,sizeof(char),44))),_tmp906)))),_tmp904)))));}_LLB:;}goto
_LL0;_LL3: if((int)_tmp106 != 1)goto _LL5;_LL4: function=fS->ffloat;goto _LL0;_LL5:
if(_tmp106 <= (void*)4)goto _LL9;if(*((int*)_tmp106)!= 6)goto _LL7;_tmp108=((struct
Cyc_Absyn_DoubleType_struct*)_tmp106)->f1;_LL6: switch(_tmp108){case 1: _LL14:
function=fS->flongdouble;break;default: _LL15: function=fS->fdouble;}goto _LL0;_LL7:
if(*((int*)_tmp106)!= 4)goto _LL9;_LL8: function=fS->fvoidstar;goto _LL0;_LL9:;_LLA: {
struct Cyc_Core_Impossible_struct _tmp90C;const char*_tmp90B;struct Cyc_Core_Impossible_struct*
_tmp90A;(int)_throw((void*)((_tmp90A=_cycalloc(sizeof(*_tmp90A)),((_tmp90A[0]=((
_tmp90C.tag=Cyc_Core_Impossible,((_tmp90C.f1=((_tmp90B="impossible expression type (not int, float, double, or pointer)",
_tag_dyneither(_tmp90B,sizeof(char),64))),_tmp90C)))),_tmp90A)))));}_LL0:;}
return function;}struct Cyc_Absyn_Exp*Cyc_Toc_getFunction(struct Cyc_Toc_functionSet*
fS,struct Cyc_Absyn_Exp*arr);struct Cyc_Absyn_Exp*Cyc_Toc_getFunction(struct Cyc_Toc_functionSet*
fS,struct Cyc_Absyn_Exp*arr){return Cyc_Toc_getFunctionType(fS,Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(arr->topt))->v));}struct Cyc_Absyn_Exp*
Cyc_Toc_getFunctionRemovePointer(struct Cyc_Toc_functionSet*fS,struct Cyc_Absyn_Exp*
arr);struct Cyc_Absyn_Exp*Cyc_Toc_getFunctionRemovePointer(struct Cyc_Toc_functionSet*
fS,struct Cyc_Absyn_Exp*arr){void*_tmp110=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(arr->topt))->v);struct Cyc_Absyn_PtrInfo _tmp111;void*_tmp112;_LL18:
if(_tmp110 <= (void*)4)goto _LL1A;if(*((int*)_tmp110)!= 4)goto _LL1A;_tmp111=((
struct Cyc_Absyn_PointerType_struct*)_tmp110)->f1;_tmp112=(void*)_tmp111.elt_typ;
_LL19: return Cyc_Toc_getFunctionType(fS,_tmp112);_LL1A:;_LL1B: {struct Cyc_Core_Impossible_struct
_tmp912;const char*_tmp911;struct Cyc_Core_Impossible_struct*_tmp910;(int)_throw((
void*)((_tmp910=_cycalloc(sizeof(*_tmp910)),((_tmp910[0]=((_tmp912.tag=Cyc_Core_Impossible,((
_tmp912.f1=((_tmp911="impossible type (not pointer)",_tag_dyneither(_tmp911,
sizeof(char),30))),_tmp912)))),_tmp910)))));}_LL17:;}struct _tuple8{struct Cyc_List_List*
f1;struct Cyc_Absyn_Exp*f2;};static int Cyc_Toc_is_zero(struct Cyc_Absyn_Exp*e);
static int Cyc_Toc_is_zero(struct Cyc_Absyn_Exp*e){void*_tmp116=(void*)e->r;union
Cyc_Absyn_Cnst_union _tmp117;char _tmp118;union Cyc_Absyn_Cnst_union _tmp119;short
_tmp11A;union Cyc_Absyn_Cnst_union _tmp11B;int _tmp11C;union Cyc_Absyn_Cnst_union
_tmp11D;long long _tmp11E;union Cyc_Absyn_Cnst_union _tmp11F;struct Cyc_Absyn_Exp*
_tmp120;struct Cyc_List_List*_tmp121;struct Cyc_List_List*_tmp122;struct Cyc_List_List*
_tmp123;struct Cyc_List_List*_tmp124;struct Cyc_List_List*_tmp125;_LL1D: if(*((int*)
_tmp116)!= 0)goto _LL1F;_tmp117=((struct Cyc_Absyn_Const_e_struct*)_tmp116)->f1;
if(((((struct Cyc_Absyn_Const_e_struct*)_tmp116)->f1).Char_c).tag != 0)goto _LL1F;
_tmp118=(_tmp117.Char_c).f2;_LL1E: return _tmp118 == '\000';_LL1F: if(*((int*)
_tmp116)!= 0)goto _LL21;_tmp119=((struct Cyc_Absyn_Const_e_struct*)_tmp116)->f1;
if(((((struct Cyc_Absyn_Const_e_struct*)_tmp116)->f1).Short_c).tag != 1)goto _LL21;
_tmp11A=(_tmp119.Short_c).f2;_LL20: return _tmp11A == 0;_LL21: if(*((int*)_tmp116)!= 
0)goto _LL23;_tmp11B=((struct Cyc_Absyn_Const_e_struct*)_tmp116)->f1;if(((((struct
Cyc_Absyn_Const_e_struct*)_tmp116)->f1).Int_c).tag != 2)goto _LL23;_tmp11C=(
_tmp11B.Int_c).f2;_LL22: return _tmp11C == 0;_LL23: if(*((int*)_tmp116)!= 0)goto
_LL25;_tmp11D=((struct Cyc_Absyn_Const_e_struct*)_tmp116)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)
_tmp116)->f1).LongLong_c).tag != 3)goto _LL25;_tmp11E=(_tmp11D.LongLong_c).f2;
_LL24: return _tmp11E == 0;_LL25: if(*((int*)_tmp116)!= 0)goto _LL27;_tmp11F=((struct
Cyc_Absyn_Const_e_struct*)_tmp116)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)
_tmp116)->f1).Null_c).tag != 6)goto _LL27;_LL26: return 1;_LL27: if(*((int*)_tmp116)
!= 15)goto _LL29;_tmp120=((struct Cyc_Absyn_Cast_e_struct*)_tmp116)->f2;_LL28:
return Cyc_Toc_is_zero(_tmp120);_LL29: if(*((int*)_tmp116)!= 26)goto _LL2B;_tmp121=((
struct Cyc_Absyn_Tuple_e_struct*)_tmp116)->f1;_LL2A: return((int(*)(int(*pred)(
struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_forall)(Cyc_Toc_is_zero,
_tmp121);_LL2B: if(*((int*)_tmp116)!= 28)goto _LL2D;_tmp122=((struct Cyc_Absyn_Array_e_struct*)
_tmp116)->f1;_LL2C: _tmp123=_tmp122;goto _LL2E;_LL2D: if(*((int*)_tmp116)!= 30)goto
_LL2F;_tmp123=((struct Cyc_Absyn_Aggregate_e_struct*)_tmp116)->f3;_LL2E: _tmp124=
_tmp123;goto _LL30;_LL2F: if(*((int*)_tmp116)!= 27)goto _LL31;_tmp124=((struct Cyc_Absyn_CompoundLit_e_struct*)
_tmp116)->f2;_LL30: _tmp125=_tmp124;goto _LL32;_LL31: if(*((int*)_tmp116)!= 37)goto
_LL33;_tmp125=((struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp116)->f2;_LL32: for(0;
_tmp125 != 0;_tmp125=_tmp125->tl){if(!Cyc_Toc_is_zero((*((struct _tuple8*)_tmp125->hd)).f2))
return 0;}return 1;_LL33:;_LL34: return 0;_LL1C:;}static int Cyc_Toc_is_nullable(void*
t);static int Cyc_Toc_is_nullable(void*t){void*_tmp126=Cyc_Tcutil_compress(t);
struct Cyc_Absyn_PtrInfo _tmp127;struct Cyc_Absyn_PtrAtts _tmp128;struct Cyc_Absyn_Conref*
_tmp129;_LL36: if(_tmp126 <= (void*)4)goto _LL38;if(*((int*)_tmp126)!= 4)goto _LL38;
_tmp127=((struct Cyc_Absyn_PointerType_struct*)_tmp126)->f1;_tmp128=_tmp127.ptr_atts;
_tmp129=_tmp128.nullable;_LL37: return((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,_tmp129);_LL38:;_LL39: {const char*_tmp915;void*_tmp914;(_tmp914=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmp915="is_nullable",
_tag_dyneither(_tmp915,sizeof(char),12))),_tag_dyneither(_tmp914,sizeof(void*),0)));}
_LL35:;}static char _tmp138[1]="";static char _tmp13A[8]="*bogus*";static struct
_tuple1*Cyc_Toc_collapse_qvar_tag(struct _tuple1*x,struct _dyneither_ptr tag);
static struct _tuple1*Cyc_Toc_collapse_qvar_tag(struct _tuple1*x,struct
_dyneither_ptr tag){struct _DynRegionHandle*_tmp12D;struct Cyc_Dict_Dict*_tmp12E;
struct Cyc_Toc_TocState _tmp12C=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));
_tmp12D=_tmp12C.dyn;_tmp12E=_tmp12C.qvar_tags;{static struct _dyneither_ptr
bogus_string={_tmp13A,_tmp13A,_tmp13A + 8};static struct _tuple1 bogus_qvar={(union
Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& bogus_string};
static struct _tuple6 pair={& bogus_qvar,{_tmp138,_tmp138,_tmp138 + 1}};{struct
_tuple6 _tmp916;pair=((_tmp916.f1=x,((_tmp916.f2=tag,_tmp916))));}{struct
_DynRegionFrame _tmp130;struct _RegionHandle*d=_open_dynregion(& _tmp130,_tmp12D);{
struct _tuple1**_tmp131=((struct _tuple1**(*)(struct Cyc_Dict_Dict d,struct _tuple6*k))
Cyc_Dict_lookup_opt)(*_tmp12E,(struct _tuple6*)& pair);if(_tmp131 != 0){struct
_tuple1*_tmp132=*_tmp131;_npop_handler(0);return _tmp132;}{struct _tuple6*_tmp917;
struct _tuple6*_tmp133=(_tmp917=_cycalloc(sizeof(*_tmp917)),((_tmp917->f1=x,((
_tmp917->f2=tag,_tmp917)))));struct _dyneither_ptr*_tmp91A;struct _tuple1*_tmp919;
struct _tuple1*res=(_tmp919=_cycalloc(sizeof(*_tmp919)),((_tmp919->f1=(*x).f1,((
_tmp919->f2=((_tmp91A=_cycalloc(sizeof(*_tmp91A)),((_tmp91A[0]=(struct
_dyneither_ptr)Cyc_strconcat((struct _dyneither_ptr)*(*x).f2,(struct
_dyneither_ptr)tag),_tmp91A)))),_tmp919)))));*_tmp12E=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,struct _tuple6*k,struct _tuple1*v))Cyc_Dict_insert)(*_tmp12E,(
struct _tuple6*)_tmp133,res);{struct _tuple1*_tmp134=res;_npop_handler(0);return
_tmp134;}}};_pop_dynregion(d);}}}struct _tuple9{void*f1;struct Cyc_List_List*f2;};
static void*Cyc_Toc_add_tuple_type(struct Cyc_List_List*tqs0);static void*Cyc_Toc_add_tuple_type(
struct Cyc_List_List*tqs0){struct _DynRegionHandle*_tmp13C;struct Cyc_List_List**
_tmp13D;struct Cyc_Toc_TocState _tmp13B=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));
_tmp13C=_tmp13B.dyn;_tmp13D=_tmp13B.tuple_types;{struct _DynRegionFrame _tmp13E;
struct _RegionHandle*d=_open_dynregion(& _tmp13E,_tmp13C);{struct Cyc_List_List*
_tmp13F=*_tmp13D;for(0;_tmp13F != 0;_tmp13F=_tmp13F->tl){struct _tuple9 _tmp141;
void*_tmp142;struct Cyc_List_List*_tmp143;struct _tuple9*_tmp140=(struct _tuple9*)
_tmp13F->hd;_tmp141=*_tmp140;_tmp142=_tmp141.f1;_tmp143=_tmp141.f2;{struct Cyc_List_List*
_tmp144=tqs0;for(0;_tmp144 != 0  && _tmp143 != 0;(_tmp144=_tmp144->tl,_tmp143=
_tmp143->tl)){if(!Cyc_Tcutil_unify((*((struct _tuple4*)_tmp144->hd)).f2,(void*)
_tmp143->hd))break;}if(_tmp144 == 0  && _tmp143 == 0){void*_tmp145=_tmp142;
_npop_handler(0);return _tmp145;}}}}{struct Cyc_Int_pa_struct _tmp922;void*_tmp921[
1];const char*_tmp920;struct _dyneither_ptr*_tmp91F;struct _dyneither_ptr*xname=(
_tmp91F=_cycalloc(sizeof(*_tmp91F)),((_tmp91F[0]=(struct _dyneither_ptr)((_tmp922.tag=
1,((_tmp922.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++,((_tmp921[0]=& _tmp922,
Cyc_aprintf(((_tmp920="_tuple%d",_tag_dyneither(_tmp920,sizeof(char),9))),
_tag_dyneither(_tmp921,sizeof(void*),1)))))))),_tmp91F)));void*x=Cyc_Absyn_strct(
xname);struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(struct _RegionHandle*,
void*(*f)(struct _tuple4*),struct Cyc_List_List*x))Cyc_List_rmap)(d,Cyc_Tcutil_snd_tqt,
tqs0);struct Cyc_List_List*_tmp146=0;struct Cyc_List_List*ts2=ts;{int i=1;for(0;ts2
!= 0;(ts2=ts2->tl,i ++)){struct Cyc_Absyn_Aggrfield*_tmp925;struct Cyc_List_List*
_tmp924;_tmp146=((_tmp924=_cycalloc(sizeof(*_tmp924)),((_tmp924->hd=((_tmp925=
_cycalloc(sizeof(*_tmp925)),((_tmp925->name=Cyc_Absyn_fieldname(i),((_tmp925->tq=
Cyc_Toc_mt_tq,((_tmp925->type=(void*)((void*)ts2->hd),((_tmp925->width=0,((
_tmp925->attributes=0,_tmp925)))))))))))),((_tmp924->tl=_tmp146,_tmp924))))));}}
_tmp146=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmp146);{struct _tuple1*_tmp92D;union Cyc_Absyn_Nmspace_union _tmp92C;struct Cyc_Absyn_AggrdeclImpl*
_tmp92B;struct Cyc_Absyn_Aggrdecl*_tmp92A;struct Cyc_Absyn_Aggrdecl*_tmp149=(
_tmp92A=_cycalloc(sizeof(*_tmp92A)),((_tmp92A->kind=(void*)((void*)0),((_tmp92A->sc=(
void*)((void*)2),((_tmp92A->name=((_tmp92D=_cycalloc(sizeof(*_tmp92D)),((_tmp92D->f1=(
union Cyc_Absyn_Nmspace_union)(((_tmp92C.Rel_n).tag=1,(((_tmp92C.Rel_n).f1=0,
_tmp92C)))),((_tmp92D->f2=xname,_tmp92D)))))),((_tmp92A->tvs=0,((_tmp92A->impl=((
_tmp92B=_cycalloc(sizeof(*_tmp92B)),((_tmp92B->exist_vars=0,((_tmp92B->rgn_po=0,((
_tmp92B->fields=_tmp146,((_tmp92B->tagged=0,_tmp92B)))))))))),((_tmp92A->attributes=
0,_tmp92A)))))))))))));{struct Cyc_Absyn_Aggr_d_struct*_tmp933;struct Cyc_Absyn_Aggr_d_struct
_tmp932;struct Cyc_List_List*_tmp931;Cyc_Toc_result_decls=((_tmp931=_cycalloc(
sizeof(*_tmp931)),((_tmp931->hd=Cyc_Absyn_new_decl((void*)((_tmp933=_cycalloc(
sizeof(*_tmp933)),((_tmp933[0]=((_tmp932.tag=6,((_tmp932.f1=_tmp149,_tmp932)))),
_tmp933)))),0),((_tmp931->tl=Cyc_Toc_result_decls,_tmp931))))));}{struct _tuple9*
_tmp936;struct Cyc_List_List*_tmp935;*_tmp13D=((_tmp935=_region_malloc(d,sizeof(*
_tmp935)),((_tmp935->hd=((_tmp936=_region_malloc(d,sizeof(*_tmp936)),((_tmp936->f1=
x,((_tmp936->f2=ts,_tmp936)))))),((_tmp935->tl=*_tmp13D,_tmp935))))));}{void*
_tmp14F=x;_npop_handler(0);return _tmp14F;}}};_pop_dynregion(d);}}struct _tuple1*
Cyc_Toc_temp_var();struct _tuple1*Cyc_Toc_temp_var(){int _tmp158=Cyc_Toc_temp_var_counter
++;union Cyc_Absyn_Nmspace_union _tmp945;struct _dyneither_ptr*_tmp944;const char*
_tmp943;void*_tmp942[1];struct Cyc_Int_pa_struct _tmp941;struct _tuple1*_tmp940;
struct _tuple1*res=(_tmp940=_cycalloc(sizeof(*_tmp940)),((_tmp940->f1=(union Cyc_Absyn_Nmspace_union)(((
_tmp945.Loc_n).tag=0,_tmp945)),((_tmp940->f2=((_tmp944=_cycalloc(sizeof(*_tmp944)),((
_tmp944[0]=(struct _dyneither_ptr)((_tmp941.tag=1,((_tmp941.f1=(unsigned int)
_tmp158,((_tmp942[0]=& _tmp941,Cyc_aprintf(((_tmp943="_tmp%X",_tag_dyneither(
_tmp943,sizeof(char),7))),_tag_dyneither(_tmp942,sizeof(void*),1)))))))),_tmp944)))),
_tmp940)))));return res;}static struct _dyneither_ptr*Cyc_Toc_fresh_label();static
struct _dyneither_ptr*Cyc_Toc_fresh_label(){struct _DynRegionHandle*_tmp160;struct
Cyc_Xarray_Xarray*_tmp161;struct Cyc_Toc_TocState _tmp15F=*((struct Cyc_Toc_TocState*)
_check_null(Cyc_Toc_toc_state));_tmp160=_tmp15F.dyn;_tmp161=_tmp15F.temp_labels;{
struct _DynRegionFrame _tmp162;struct _RegionHandle*d=_open_dynregion(& _tmp162,
_tmp160);{int _tmp163=Cyc_Toc_fresh_label_counter ++;if(_tmp163 < ((int(*)(struct
Cyc_Xarray_Xarray*))Cyc_Xarray_length)(_tmp161)){struct _dyneither_ptr*_tmp164=((
struct _dyneither_ptr*(*)(struct Cyc_Xarray_Xarray*,int))Cyc_Xarray_get)(_tmp161,
_tmp163);_npop_handler(0);return _tmp164;}{struct Cyc_Int_pa_struct _tmp94D;void*
_tmp94C[1];const char*_tmp94B;struct _dyneither_ptr*_tmp94A;struct _dyneither_ptr*
res=(_tmp94A=_cycalloc(sizeof(*_tmp94A)),((_tmp94A[0]=(struct _dyneither_ptr)((
_tmp94D.tag=1,((_tmp94D.f1=(unsigned int)_tmp163,((_tmp94C[0]=& _tmp94D,Cyc_aprintf(((
_tmp94B="_LL%X",_tag_dyneither(_tmp94B,sizeof(char),6))),_tag_dyneither(_tmp94C,
sizeof(void*),1)))))))),_tmp94A)));if(((int(*)(struct Cyc_Xarray_Xarray*,struct
_dyneither_ptr*))Cyc_Xarray_add_ind)(_tmp161,res)!= _tmp163){const char*_tmp950;
void*_tmp94F;(_tmp94F=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Toc_toc_impos)(((_tmp950="fresh_label: add_ind returned bad index...",
_tag_dyneither(_tmp950,sizeof(char),43))),_tag_dyneither(_tmp94F,sizeof(void*),0)));}{
struct _dyneither_ptr*_tmp167=res;_npop_handler(0);return _tmp167;}}};
_pop_dynregion(d);}}static struct Cyc_Absyn_Exp*Cyc_Toc_datatype_tag(struct Cyc_Absyn_Datatypedecl*
td,struct _tuple1*name,int carries_value);static struct Cyc_Absyn_Exp*Cyc_Toc_datatype_tag(
struct Cyc_Absyn_Datatypedecl*td,struct _tuple1*name,int carries_value){int ans=0;
struct Cyc_List_List*_tmp16C=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(td->fields))->v;while(Cyc_Absyn_qvar_cmp(name,((struct Cyc_Absyn_Datatypefield*)((
struct Cyc_List_List*)_check_null(_tmp16C))->hd)->name)!= 0){if((td->is_flat  || 
carries_value  && ((struct Cyc_Absyn_Datatypefield*)_tmp16C->hd)->typs != 0) || !
carries_value  && ((struct Cyc_Absyn_Datatypefield*)_tmp16C->hd)->typs == 0)++ ans;
_tmp16C=_tmp16C->tl;}return Cyc_Absyn_uint_exp((unsigned int)ans,0);}static int Cyc_Toc_num_void_tags(
struct Cyc_Absyn_Datatypedecl*td);static int Cyc_Toc_num_void_tags(struct Cyc_Absyn_Datatypedecl*
td){int ans=0;{struct Cyc_List_List*_tmp16D=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(td->fields))->v;for(0;_tmp16D != 0;_tmp16D=_tmp16D->tl){if(((struct
Cyc_Absyn_Datatypefield*)_tmp16D->hd)->typs == 0)++ ans;}}return ans;}static void*
Cyc_Toc_typ_to_c(void*t);static struct _tuple2*Cyc_Toc_arg_to_c(struct _tuple2*a);
static struct _tuple2*Cyc_Toc_arg_to_c(struct _tuple2*a){struct Cyc_Core_Opt*_tmp16F;
struct Cyc_Absyn_Tqual _tmp170;void*_tmp171;struct _tuple2 _tmp16E=*a;_tmp16F=
_tmp16E.f1;_tmp170=_tmp16E.f2;_tmp171=_tmp16E.f3;{struct _tuple2*_tmp951;return(
_tmp951=_cycalloc(sizeof(*_tmp951)),((_tmp951->f1=_tmp16F,((_tmp951->f2=_tmp170,((
_tmp951->f3=Cyc_Toc_typ_to_c(_tmp171),_tmp951)))))));}}static struct _tuple4*Cyc_Toc_typ_to_c_f(
struct _tuple4*x);static struct _tuple4*Cyc_Toc_typ_to_c_f(struct _tuple4*x){struct
Cyc_Absyn_Tqual _tmp174;void*_tmp175;struct _tuple4 _tmp173=*x;_tmp174=_tmp173.f1;
_tmp175=_tmp173.f2;{struct _tuple4*_tmp952;return(_tmp952=_cycalloc(sizeof(*
_tmp952)),((_tmp952->f1=_tmp174,((_tmp952->f2=Cyc_Toc_typ_to_c(_tmp175),_tmp952)))));}}
static void*Cyc_Toc_typ_to_c_array(void*t);static void*Cyc_Toc_typ_to_c_array(void*
t){void*_tmp177=Cyc_Tcutil_compress(t);struct Cyc_Absyn_ArrayInfo _tmp178;void*
_tmp179;struct Cyc_Absyn_Tqual _tmp17A;struct Cyc_Absyn_Exp*_tmp17B;struct Cyc_Absyn_Conref*
_tmp17C;struct Cyc_Position_Segment*_tmp17D;struct Cyc_Core_Opt*_tmp17E;struct Cyc_Core_Opt
_tmp17F;void*_tmp180;_LL3B: if(_tmp177 <= (void*)4)goto _LL3F;if(*((int*)_tmp177)!= 
7)goto _LL3D;_tmp178=((struct Cyc_Absyn_ArrayType_struct*)_tmp177)->f1;_tmp179=(
void*)_tmp178.elt_type;_tmp17A=_tmp178.tq;_tmp17B=_tmp178.num_elts;_tmp17C=
_tmp178.zero_term;_tmp17D=_tmp178.zt_loc;_LL3C: return Cyc_Absyn_array_typ(Cyc_Toc_typ_to_c_array(
_tmp179),_tmp17A,_tmp17B,Cyc_Absyn_false_conref,_tmp17D);_LL3D: if(*((int*)
_tmp177)!= 0)goto _LL3F;_tmp17E=((struct Cyc_Absyn_Evar_struct*)_tmp177)->f2;if(
_tmp17E == 0)goto _LL3F;_tmp17F=*_tmp17E;_tmp180=(void*)_tmp17F.v;_LL3E: return Cyc_Toc_typ_to_c_array(
_tmp180);_LL3F:;_LL40: return Cyc_Toc_typ_to_c(t);_LL3A:;}static struct Cyc_Absyn_Aggrfield*
Cyc_Toc_aggrfield_to_c(struct Cyc_Absyn_Aggrfield*f);static struct Cyc_Absyn_Aggrfield*
Cyc_Toc_aggrfield_to_c(struct Cyc_Absyn_Aggrfield*f){struct Cyc_Absyn_Aggrfield*
_tmp953;return(_tmp953=_cycalloc(sizeof(*_tmp953)),((_tmp953->name=f->name,((
_tmp953->tq=Cyc_Toc_mt_tq,((_tmp953->type=(void*)Cyc_Toc_typ_to_c((void*)f->type),((
_tmp953->width=f->width,((_tmp953->attributes=f->attributes,_tmp953)))))))))));}
static void Cyc_Toc_enumfields_to_c(struct Cyc_List_List*fs);static void Cyc_Toc_enumfields_to_c(
struct Cyc_List_List*fs){return;}static void*Cyc_Toc_char_star_typ();static void*
Cyc_Toc_char_star_typ(){static void**cs=0;if(cs == 0){void**_tmp954;cs=((_tmp954=
_cycalloc(sizeof(*_tmp954)),((_tmp954[0]=Cyc_Absyn_star_typ(Cyc_Absyn_char_typ,(
void*)2,Cyc_Toc_mt_tq,Cyc_Absyn_false_conref),_tmp954))));}return*cs;}static void*
Cyc_Toc_rgn_typ();static void*Cyc_Toc_rgn_typ(){static void**r=0;if(r == 0){void**
_tmp955;r=((_tmp955=_cycalloc(sizeof(*_tmp955)),((_tmp955[0]=Cyc_Absyn_cstar_typ(
Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp),Cyc_Toc_mt_tq),_tmp955))));}return*r;}
static void*Cyc_Toc_dyn_rgn_typ();static void*Cyc_Toc_dyn_rgn_typ(){static void**r=
0;if(r == 0){void**_tmp956;r=((_tmp956=_cycalloc(sizeof(*_tmp956)),((_tmp956[0]=
Cyc_Absyn_cstar_typ(Cyc_Absyn_strct(Cyc_Toc__DynRegionHandle_sp),Cyc_Toc_mt_tq),
_tmp956))));}return*r;}static void*Cyc_Toc_typ_to_c(void*t);static void*Cyc_Toc_typ_to_c(
void*t){void*_tmp185=t;struct Cyc_Core_Opt*_tmp186;struct Cyc_Core_Opt*_tmp187;
struct Cyc_Core_Opt _tmp188;void*_tmp189;struct Cyc_Absyn_Tvar*_tmp18A;struct Cyc_Absyn_DatatypeInfo
_tmp18B;union Cyc_Absyn_DatatypeInfoU_union _tmp18C;struct Cyc_Absyn_Datatypedecl**
_tmp18D;struct Cyc_Absyn_Datatypedecl*_tmp18E;struct Cyc_Absyn_DatatypeFieldInfo
_tmp18F;union Cyc_Absyn_DatatypeFieldInfoU_union _tmp190;struct Cyc_Absyn_Datatypedecl*
_tmp191;struct Cyc_Absyn_Datatypefield*_tmp192;struct Cyc_Absyn_PtrInfo _tmp193;
void*_tmp194;struct Cyc_Absyn_Tqual _tmp195;struct Cyc_Absyn_PtrAtts _tmp196;struct
Cyc_Absyn_Conref*_tmp197;struct Cyc_Absyn_ArrayInfo _tmp198;void*_tmp199;struct Cyc_Absyn_Tqual
_tmp19A;struct Cyc_Absyn_Exp*_tmp19B;struct Cyc_Position_Segment*_tmp19C;struct Cyc_Absyn_FnInfo
_tmp19D;void*_tmp19E;struct Cyc_List_List*_tmp19F;int _tmp1A0;struct Cyc_Absyn_VarargInfo*
_tmp1A1;struct Cyc_List_List*_tmp1A2;struct Cyc_List_List*_tmp1A3;void*_tmp1A4;
struct Cyc_List_List*_tmp1A5;struct Cyc_Absyn_AggrInfo _tmp1A6;union Cyc_Absyn_AggrInfoU_union
_tmp1A7;struct Cyc_List_List*_tmp1A8;struct _tuple1*_tmp1A9;struct Cyc_List_List*
_tmp1AA;struct _tuple1*_tmp1AB;struct Cyc_List_List*_tmp1AC;struct Cyc_Absyn_Typedefdecl*
_tmp1AD;void**_tmp1AE;void*_tmp1AF;_LL42: if((int)_tmp185 != 0)goto _LL44;_LL43:
return t;_LL44: if(_tmp185 <= (void*)4)goto _LL56;if(*((int*)_tmp185)!= 0)goto _LL46;
_tmp186=((struct Cyc_Absyn_Evar_struct*)_tmp185)->f2;if(_tmp186 != 0)goto _LL46;
_LL45: return Cyc_Absyn_sint_typ;_LL46: if(*((int*)_tmp185)!= 0)goto _LL48;_tmp187=((
struct Cyc_Absyn_Evar_struct*)_tmp185)->f2;if(_tmp187 == 0)goto _LL48;_tmp188=*
_tmp187;_tmp189=(void*)_tmp188.v;_LL47: return Cyc_Toc_typ_to_c(_tmp189);_LL48: if(*((
int*)_tmp185)!= 1)goto _LL4A;_tmp18A=((struct Cyc_Absyn_VarType_struct*)_tmp185)->f1;
_LL49: if(Cyc_Tcutil_tvar_kind(_tmp18A)== (void*)0)return(void*)0;else{return Cyc_Absyn_void_star_typ();}
_LL4A: if(*((int*)_tmp185)!= 2)goto _LL4C;_tmp18B=((struct Cyc_Absyn_DatatypeType_struct*)
_tmp185)->f1;_tmp18C=_tmp18B.datatype_info;if((((((struct Cyc_Absyn_DatatypeType_struct*)
_tmp185)->f1).datatype_info).KnownDatatype).tag != 1)goto _LL4C;_tmp18D=(_tmp18C.KnownDatatype).f1;
_tmp18E=*_tmp18D;_LL4B: if(_tmp18E->is_flat){const char*_tmp957;return Cyc_Absyn_unionq_typ(
Cyc_Toc_collapse_qvar_tag(_tmp18E->name,((_tmp957="_union",_tag_dyneither(
_tmp957,sizeof(char),7)))));}else{return Cyc_Absyn_void_star_typ();}_LL4C: if(*((
int*)_tmp185)!= 2)goto _LL4E;_LL4D: {const char*_tmp95A;void*_tmp959;(_tmp959=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmp95A="unresolved DatatypeType",_tag_dyneither(_tmp95A,sizeof(char),24))),
_tag_dyneither(_tmp959,sizeof(void*),0)));}_LL4E: if(*((int*)_tmp185)!= 3)goto
_LL50;_tmp18F=((struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp185)->f1;_tmp190=
_tmp18F.field_info;if((((((struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp185)->f1).field_info).KnownDatatypefield).tag
!= 1)goto _LL50;_tmp191=(_tmp190.KnownDatatypefield).f1;_tmp192=(_tmp190.KnownDatatypefield).f2;
_LL4F: if(_tmp191->is_flat){const char*_tmp95B;return Cyc_Absyn_unionq_typ(Cyc_Toc_collapse_qvar_tag(
_tmp191->name,((_tmp95B="_union",_tag_dyneither(_tmp95B,sizeof(char),7)))));}if(
_tmp192->typs == 0){if(_tmp191->is_extensible)return Cyc_Toc_char_star_typ();else{
return Cyc_Absyn_uint_typ;}}else{const char*_tmp95C;return Cyc_Absyn_strctq(Cyc_Toc_collapse_qvar_tag(
_tmp192->name,((_tmp95C="_struct",_tag_dyneither(_tmp95C,sizeof(char),8)))));}
_LL50: if(*((int*)_tmp185)!= 3)goto _LL52;_LL51: {const char*_tmp95F;void*_tmp95E;(
_tmp95E=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmp95F="unresolved DatatypeFieldType",_tag_dyneither(_tmp95F,sizeof(char),29))),
_tag_dyneither(_tmp95E,sizeof(void*),0)));}_LL52: if(*((int*)_tmp185)!= 4)goto
_LL54;_tmp193=((struct Cyc_Absyn_PointerType_struct*)_tmp185)->f1;_tmp194=(void*)
_tmp193.elt_typ;_tmp195=_tmp193.elt_tq;_tmp196=_tmp193.ptr_atts;_tmp197=_tmp196.bounds;
_LL53: _tmp194=Cyc_Toc_typ_to_c_array(_tmp194);{union Cyc_Absyn_Constraint_union
_tmp1B7=(Cyc_Absyn_compress_conref(_tmp197))->v;void*_tmp1B8;_LL7D: if((_tmp1B7.No_constr).tag
!= 2)goto _LL7F;_LL7E: goto _LL80;_LL7F: if((_tmp1B7.Eq_constr).tag != 0)goto _LL81;
_tmp1B8=(_tmp1B7.Eq_constr).f1;if((int)_tmp1B8 != 0)goto _LL81;_LL80: return Cyc_Toc_dyneither_ptr_typ;
_LL81:;_LL82: return Cyc_Absyn_star_typ(_tmp194,(void*)2,_tmp195,Cyc_Absyn_false_conref);
_LL7C:;}_LL54: if(*((int*)_tmp185)!= 5)goto _LL56;_LL55: goto _LL57;_LL56: if((int)
_tmp185 != 1)goto _LL58;_LL57: goto _LL59;_LL58: if(_tmp185 <= (void*)4)goto _LL70;if(*((
int*)_tmp185)!= 6)goto _LL5A;_LL59: return t;_LL5A: if(*((int*)_tmp185)!= 7)goto
_LL5C;_tmp198=((struct Cyc_Absyn_ArrayType_struct*)_tmp185)->f1;_tmp199=(void*)
_tmp198.elt_type;_tmp19A=_tmp198.tq;_tmp19B=_tmp198.num_elts;_tmp19C=_tmp198.zt_loc;
_LL5B: return Cyc_Absyn_array_typ(Cyc_Toc_typ_to_c_array(_tmp199),_tmp19A,_tmp19B,
Cyc_Absyn_false_conref,_tmp19C);_LL5C: if(*((int*)_tmp185)!= 8)goto _LL5E;_tmp19D=((
struct Cyc_Absyn_FnType_struct*)_tmp185)->f1;_tmp19E=(void*)_tmp19D.ret_typ;
_tmp19F=_tmp19D.args;_tmp1A0=_tmp19D.c_varargs;_tmp1A1=_tmp19D.cyc_varargs;
_tmp1A2=_tmp19D.attributes;_LL5D: {struct Cyc_List_List*_tmp1B9=0;for(0;_tmp1A2 != 
0;_tmp1A2=_tmp1A2->tl){void*_tmp1BA=(void*)_tmp1A2->hd;_LL84: if((int)_tmp1BA != 3)
goto _LL86;_LL85: goto _LL87;_LL86: if((int)_tmp1BA != 4)goto _LL88;_LL87: goto _LL89;
_LL88: if(_tmp1BA <= (void*)17)goto _LL8C;if(*((int*)_tmp1BA)!= 3)goto _LL8A;_LL89:
continue;_LL8A: if(*((int*)_tmp1BA)!= 4)goto _LL8C;_LL8B: continue;_LL8C:;_LL8D:{
struct Cyc_List_List*_tmp960;_tmp1B9=((_tmp960=_cycalloc(sizeof(*_tmp960)),((
_tmp960->hd=(void*)((void*)_tmp1A2->hd),((_tmp960->tl=_tmp1B9,_tmp960))))));}
goto _LL83;_LL83:;}{struct Cyc_List_List*_tmp1BC=((struct Cyc_List_List*(*)(struct
_tuple2*(*f)(struct _tuple2*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_arg_to_c,
_tmp19F);if(_tmp1A1 != 0){void*_tmp1BD=Cyc_Toc_typ_to_c(Cyc_Absyn_dyneither_typ((
void*)_tmp1A1->type,(void*)2,Cyc_Toc_mt_tq,Cyc_Absyn_false_conref));struct
_tuple2*_tmp961;struct _tuple2*_tmp1BE=(_tmp961=_cycalloc(sizeof(*_tmp961)),((
_tmp961->f1=_tmp1A1->name,((_tmp961->f2=_tmp1A1->tq,((_tmp961->f3=_tmp1BD,
_tmp961)))))));struct Cyc_List_List*_tmp962;_tmp1BC=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp1BC,((
_tmp962=_cycalloc(sizeof(*_tmp962)),((_tmp962->hd=_tmp1BE,((_tmp962->tl=0,
_tmp962)))))));}{struct Cyc_Absyn_FnType_struct _tmp968;struct Cyc_Absyn_FnInfo
_tmp967;struct Cyc_Absyn_FnType_struct*_tmp966;return(void*)((_tmp966=_cycalloc(
sizeof(*_tmp966)),((_tmp966[0]=((_tmp968.tag=8,((_tmp968.f1=((_tmp967.tvars=0,((
_tmp967.effect=0,((_tmp967.ret_typ=(void*)Cyc_Toc_typ_to_c(_tmp19E),((_tmp967.args=
_tmp1BC,((_tmp967.c_varargs=_tmp1A0,((_tmp967.cyc_varargs=0,((_tmp967.rgn_po=0,((
_tmp967.attributes=_tmp1B9,_tmp967)))))))))))))))),_tmp968)))),_tmp966))));}}}
_LL5E: if(*((int*)_tmp185)!= 9)goto _LL60;_tmp1A3=((struct Cyc_Absyn_TupleType_struct*)
_tmp185)->f1;_LL5F: _tmp1A3=((struct Cyc_List_List*(*)(struct _tuple4*(*f)(struct
_tuple4*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_typ_to_c_f,_tmp1A3);
return Cyc_Toc_add_tuple_type(_tmp1A3);_LL60: if(*((int*)_tmp185)!= 11)goto _LL62;
_tmp1A4=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)_tmp185)->f1;_tmp1A5=((
struct Cyc_Absyn_AnonAggrType_struct*)_tmp185)->f2;_LL61: {struct Cyc_Absyn_AnonAggrType_struct
_tmp96B;struct Cyc_Absyn_AnonAggrType_struct*_tmp96A;return(void*)((_tmp96A=
_cycalloc(sizeof(*_tmp96A)),((_tmp96A[0]=((_tmp96B.tag=11,((_tmp96B.f1=(void*)
_tmp1A4,((_tmp96B.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(
struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_aggrfield_to_c,
_tmp1A5),_tmp96B)))))),_tmp96A))));}_LL62: if(*((int*)_tmp185)!= 10)goto _LL64;
_tmp1A6=((struct Cyc_Absyn_AggrType_struct*)_tmp185)->f1;_tmp1A7=_tmp1A6.aggr_info;
_tmp1A8=_tmp1A6.targs;_LL63:{union Cyc_Absyn_AggrInfoU_union _tmp1C6=_tmp1A7;_LL8F:
if((_tmp1C6.UnknownAggr).tag != 0)goto _LL91;_LL90: return t;_LL91:;_LL92: goto _LL8E;
_LL8E:;}{struct Cyc_Absyn_Aggrdecl*_tmp1C7=Cyc_Absyn_get_known_aggrdecl(_tmp1A7);
if((void*)_tmp1C7->kind == (void*)1)return Cyc_Toc_aggrdecl_type(_tmp1C7->name,Cyc_Absyn_unionq_typ);
else{return Cyc_Toc_aggrdecl_type(_tmp1C7->name,Cyc_Absyn_strctq);}}_LL64: if(*((
int*)_tmp185)!= 12)goto _LL66;_tmp1A9=((struct Cyc_Absyn_EnumType_struct*)_tmp185)->f1;
_LL65: return t;_LL66: if(*((int*)_tmp185)!= 13)goto _LL68;_tmp1AA=((struct Cyc_Absyn_AnonEnumType_struct*)
_tmp185)->f1;_LL67: Cyc_Toc_enumfields_to_c(_tmp1AA);return t;_LL68: if(*((int*)
_tmp185)!= 16)goto _LL6A;_tmp1AB=((struct Cyc_Absyn_TypedefType_struct*)_tmp185)->f1;
_tmp1AC=((struct Cyc_Absyn_TypedefType_struct*)_tmp185)->f2;_tmp1AD=((struct Cyc_Absyn_TypedefType_struct*)
_tmp185)->f3;_tmp1AE=((struct Cyc_Absyn_TypedefType_struct*)_tmp185)->f4;_LL69:
if(_tmp1AE == 0  || Cyc_noexpand_r){if(_tmp1AC != 0){struct Cyc_Absyn_TypedefType_struct
_tmp96E;struct Cyc_Absyn_TypedefType_struct*_tmp96D;return(void*)((_tmp96D=
_cycalloc(sizeof(*_tmp96D)),((_tmp96D[0]=((_tmp96E.tag=16,((_tmp96E.f1=_tmp1AB,((
_tmp96E.f2=0,((_tmp96E.f3=_tmp1AD,((_tmp96E.f4=0,_tmp96E)))))))))),_tmp96D))));}
else{return t;}}else{struct Cyc_Absyn_TypedefType_struct _tmp974;void**_tmp973;
struct Cyc_Absyn_TypedefType_struct*_tmp972;return(void*)((_tmp972=_cycalloc(
sizeof(*_tmp972)),((_tmp972[0]=((_tmp974.tag=16,((_tmp974.f1=_tmp1AB,((_tmp974.f2=
0,((_tmp974.f3=_tmp1AD,((_tmp974.f4=((_tmp973=_cycalloc(sizeof(*_tmp973)),((
_tmp973[0]=Cyc_Toc_typ_to_c_array(*_tmp1AE),_tmp973)))),_tmp974)))))))))),
_tmp972))));}_LL6A: if(*((int*)_tmp185)!= 18)goto _LL6C;_LL6B: return Cyc_Absyn_uint_typ;
_LL6C: if(*((int*)_tmp185)!= 14)goto _LL6E;_tmp1AF=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp185)->f1;_LL6D: return Cyc_Toc_rgn_typ();_LL6E: if(*((int*)_tmp185)!= 15)goto
_LL70;_LL6F: return Cyc_Toc_dyn_rgn_typ();_LL70: if((int)_tmp185 != 2)goto _LL72;
_LL71: {const char*_tmp977;void*_tmp976;(_tmp976=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmp977="Toc::typ_to_c: type translation passed the heap region",
_tag_dyneither(_tmp977,sizeof(char),55))),_tag_dyneither(_tmp976,sizeof(void*),0)));}
_LL72: if((int)_tmp185 != 3)goto _LL74;_LL73: {const char*_tmp97A;void*_tmp979;(
_tmp979=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmp97A="Toc::typ_to_c: type translation passed the unique region",
_tag_dyneither(_tmp97A,sizeof(char),57))),_tag_dyneither(_tmp979,sizeof(void*),0)));}
_LL74: if(_tmp185 <= (void*)4)goto _LL76;if(*((int*)_tmp185)!= 19)goto _LL76;_LL75:
goto _LL77;_LL76: if(_tmp185 <= (void*)4)goto _LL78;if(*((int*)_tmp185)!= 20)goto
_LL78;_LL77: goto _LL79;_LL78: if(_tmp185 <= (void*)4)goto _LL7A;if(*((int*)_tmp185)
!= 21)goto _LL7A;_LL79: {const char*_tmp97D;void*_tmp97C;(_tmp97C=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmp97D="Toc::typ_to_c: type translation passed an effect",
_tag_dyneither(_tmp97D,sizeof(char),49))),_tag_dyneither(_tmp97C,sizeof(void*),0)));}
_LL7A: if(_tmp185 <= (void*)4)goto _LL41;if(*((int*)_tmp185)!= 17)goto _LL41;_LL7B: {
const char*_tmp980;void*_tmp97F;(_tmp97F=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmp980="Toc::typ_to_c: type translation passed a valueof_t",
_tag_dyneither(_tmp980,sizeof(char),51))),_tag_dyneither(_tmp97F,sizeof(void*),0)));}
_LL41:;}static struct Cyc_Absyn_Exp*Cyc_Toc_array_to_ptr_cast(void*t,struct Cyc_Absyn_Exp*
e,struct Cyc_Position_Segment*l);static struct Cyc_Absyn_Exp*Cyc_Toc_array_to_ptr_cast(
void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*l){void*_tmp1D5=t;struct
Cyc_Absyn_ArrayInfo _tmp1D6;void*_tmp1D7;struct Cyc_Absyn_Tqual _tmp1D8;_LL94: if(
_tmp1D5 <= (void*)4)goto _LL96;if(*((int*)_tmp1D5)!= 7)goto _LL96;_tmp1D6=((struct
Cyc_Absyn_ArrayType_struct*)_tmp1D5)->f1;_tmp1D7=(void*)_tmp1D6.elt_type;_tmp1D8=
_tmp1D6.tq;_LL95: return Cyc_Toc_cast_it(Cyc_Absyn_star_typ(_tmp1D7,(void*)2,
_tmp1D8,Cyc_Absyn_false_conref),e);_LL96:;_LL97: return Cyc_Toc_cast_it(t,e);_LL93:;}
static int Cyc_Toc_atomic_typ(void*t);static int Cyc_Toc_atomic_typ(void*t){void*
_tmp1D9=Cyc_Tcutil_compress(t);struct Cyc_Absyn_ArrayInfo _tmp1DA;void*_tmp1DB;
struct Cyc_Absyn_AggrInfo _tmp1DC;union Cyc_Absyn_AggrInfoU_union _tmp1DD;struct Cyc_List_List*
_tmp1DE;struct Cyc_Absyn_DatatypeFieldInfo _tmp1DF;union Cyc_Absyn_DatatypeFieldInfoU_union
_tmp1E0;struct Cyc_Absyn_Datatypedecl*_tmp1E1;struct Cyc_Absyn_Datatypefield*
_tmp1E2;struct Cyc_List_List*_tmp1E3;_LL99: if((int)_tmp1D9 != 0)goto _LL9B;_LL9A:
return 1;_LL9B: if(_tmp1D9 <= (void*)4)goto _LLA3;if(*((int*)_tmp1D9)!= 1)goto _LL9D;
_LL9C: return 0;_LL9D: if(*((int*)_tmp1D9)!= 5)goto _LL9F;_LL9E: goto _LLA0;_LL9F: if(*((
int*)_tmp1D9)!= 12)goto _LLA1;_LLA0: goto _LLA2;_LLA1: if(*((int*)_tmp1D9)!= 13)goto
_LLA3;_LLA2: goto _LLA4;_LLA3: if((int)_tmp1D9 != 1)goto _LLA5;_LLA4: goto _LLA6;_LLA5:
if(_tmp1D9 <= (void*)4)goto _LLBB;if(*((int*)_tmp1D9)!= 6)goto _LLA7;_LLA6: goto
_LLA8;_LLA7: if(*((int*)_tmp1D9)!= 8)goto _LLA9;_LLA8: goto _LLAA;_LLA9: if(*((int*)
_tmp1D9)!= 18)goto _LLAB;_LLAA: return 1;_LLAB: if(*((int*)_tmp1D9)!= 7)goto _LLAD;
_tmp1DA=((struct Cyc_Absyn_ArrayType_struct*)_tmp1D9)->f1;_tmp1DB=(void*)_tmp1DA.elt_type;
_LLAC: return Cyc_Toc_atomic_typ(_tmp1DB);_LLAD: if(*((int*)_tmp1D9)!= 10)goto _LLAF;
_tmp1DC=((struct Cyc_Absyn_AggrType_struct*)_tmp1D9)->f1;_tmp1DD=_tmp1DC.aggr_info;
_LLAE:{union Cyc_Absyn_AggrInfoU_union _tmp1E4=_tmp1DD;_LLBE: if((_tmp1E4.UnknownAggr).tag
!= 0)goto _LLC0;_LLBF: return 0;_LLC0:;_LLC1: goto _LLBD;_LLBD:;}{struct Cyc_Absyn_Aggrdecl*
_tmp1E5=Cyc_Absyn_get_known_aggrdecl(_tmp1DD);if(_tmp1E5->impl == 0)return 0;{
struct Cyc_List_List*_tmp1E6=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp1E5->impl))->fields;
for(0;_tmp1E6 != 0;_tmp1E6=_tmp1E6->tl){if(!Cyc_Toc_atomic_typ((void*)((struct Cyc_Absyn_Aggrfield*)
_tmp1E6->hd)->type))return 0;}}return 1;}_LLAF: if(*((int*)_tmp1D9)!= 11)goto _LLB1;
_tmp1DE=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp1D9)->f2;_LLB0: for(0;_tmp1DE
!= 0;_tmp1DE=_tmp1DE->tl){if(!Cyc_Toc_atomic_typ((void*)((struct Cyc_Absyn_Aggrfield*)
_tmp1DE->hd)->type))return 0;}return 1;_LLB1: if(*((int*)_tmp1D9)!= 3)goto _LLB3;
_tmp1DF=((struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp1D9)->f1;_tmp1E0=_tmp1DF.field_info;
if((((((struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp1D9)->f1).field_info).KnownDatatypefield).tag
!= 1)goto _LLB3;_tmp1E1=(_tmp1E0.KnownDatatypefield).f1;_tmp1E2=(_tmp1E0.KnownDatatypefield).f2;
_LLB2: _tmp1E3=_tmp1E2->typs;goto _LLB4;_LLB3: if(*((int*)_tmp1D9)!= 9)goto _LLB5;
_tmp1E3=((struct Cyc_Absyn_TupleType_struct*)_tmp1D9)->f1;_LLB4: for(0;_tmp1E3 != 0;
_tmp1E3=_tmp1E3->tl){if(!Cyc_Toc_atomic_typ((*((struct _tuple4*)_tmp1E3->hd)).f2))
return 0;}return 1;_LLB5: if(*((int*)_tmp1D9)!= 2)goto _LLB7;_LLB6: goto _LLB8;_LLB7:
if(*((int*)_tmp1D9)!= 4)goto _LLB9;_LLB8: goto _LLBA;_LLB9: if(*((int*)_tmp1D9)!= 14)
goto _LLBB;_LLBA: return 0;_LLBB:;_LLBC: {const char*_tmp984;void*_tmp983[1];struct
Cyc_String_pa_struct _tmp982;(_tmp982.tag=0,((_tmp982.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp983[0]=& _tmp982,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmp984="atomic_typ:  bad type %s",
_tag_dyneither(_tmp984,sizeof(char),25))),_tag_dyneither(_tmp983,sizeof(void*),1)))))));}
_LL98:;}static int Cyc_Toc_is_void_star(void*t);static int Cyc_Toc_is_void_star(void*
t){void*_tmp1EA=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp1EB;void*
_tmp1EC;_LLC3: if(_tmp1EA <= (void*)4)goto _LLC5;if(*((int*)_tmp1EA)!= 4)goto _LLC5;
_tmp1EB=((struct Cyc_Absyn_PointerType_struct*)_tmp1EA)->f1;_tmp1EC=(void*)
_tmp1EB.elt_typ;_LLC4: {void*_tmp1ED=Cyc_Tcutil_compress(_tmp1EC);_LLC8: if((int)
_tmp1ED != 0)goto _LLCA;_LLC9: return 1;_LLCA:;_LLCB: return 0;_LLC7:;}_LLC5:;_LLC6:
return 0;_LLC2:;}static int Cyc_Toc_is_poly_field(void*t,struct _dyneither_ptr*f);
static int Cyc_Toc_is_poly_field(void*t,struct _dyneither_ptr*f){void*_tmp1EE=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_AggrInfo _tmp1EF;union Cyc_Absyn_AggrInfoU_union _tmp1F0;struct
Cyc_List_List*_tmp1F1;_LLCD: if(_tmp1EE <= (void*)4)goto _LLD1;if(*((int*)_tmp1EE)
!= 10)goto _LLCF;_tmp1EF=((struct Cyc_Absyn_AggrType_struct*)_tmp1EE)->f1;_tmp1F0=
_tmp1EF.aggr_info;_LLCE: {struct Cyc_Absyn_Aggrdecl*_tmp1F2=Cyc_Absyn_get_known_aggrdecl(
_tmp1F0);if(_tmp1F2->impl == 0){const char*_tmp987;void*_tmp986;(_tmp986=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmp987="is_poly_field: type missing fields",
_tag_dyneither(_tmp987,sizeof(char),35))),_tag_dyneither(_tmp986,sizeof(void*),0)));}
_tmp1F1=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp1F2->impl))->fields;goto
_LLD0;}_LLCF: if(*((int*)_tmp1EE)!= 11)goto _LLD1;_tmp1F1=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp1EE)->f2;_LLD0: {struct Cyc_Absyn_Aggrfield*_tmp1F5=Cyc_Absyn_lookup_field(
_tmp1F1,f);if(_tmp1F5 == 0){const char*_tmp98B;void*_tmp98A[1];struct Cyc_String_pa_struct
_tmp989;(_tmp989.tag=0,((_tmp989.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*
f),((_tmp98A[0]=& _tmp989,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Toc_toc_impos)(((_tmp98B="is_poly_field: bad field %s",_tag_dyneither(
_tmp98B,sizeof(char),28))),_tag_dyneither(_tmp98A,sizeof(void*),1)))))));}return
Cyc_Toc_is_void_star((void*)_tmp1F5->type);}_LLD1:;_LLD2: {const char*_tmp98F;
void*_tmp98E[1];struct Cyc_String_pa_struct _tmp98D;(_tmp98D.tag=0,((_tmp98D.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp98E[
0]=& _tmp98D,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmp98F="is_poly_field: bad type %s",_tag_dyneither(_tmp98F,sizeof(char),27))),
_tag_dyneither(_tmp98E,sizeof(void*),1)))))));}_LLCC:;}static int Cyc_Toc_is_poly_project(
struct Cyc_Absyn_Exp*e);static int Cyc_Toc_is_poly_project(struct Cyc_Absyn_Exp*e){
void*_tmp1FC=(void*)e->r;struct Cyc_Absyn_Exp*_tmp1FD;struct _dyneither_ptr*
_tmp1FE;struct Cyc_Absyn_Exp*_tmp1FF;struct _dyneither_ptr*_tmp200;_LLD4: if(*((int*)
_tmp1FC)!= 23)goto _LLD6;_tmp1FD=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp1FC)->f1;
_tmp1FE=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp1FC)->f2;_LLD5: return Cyc_Toc_is_poly_field((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp1FD->topt))->v,_tmp1FE);_LLD6: if(*((
int*)_tmp1FC)!= 24)goto _LLD8;_tmp1FF=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp1FC)->f1;_tmp200=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp1FC)->f2;_LLD7: {
void*_tmp201=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp1FF->topt))->v);struct Cyc_Absyn_PtrInfo _tmp202;void*_tmp203;_LLDB: if(_tmp201
<= (void*)4)goto _LLDD;if(*((int*)_tmp201)!= 4)goto _LLDD;_tmp202=((struct Cyc_Absyn_PointerType_struct*)
_tmp201)->f1;_tmp203=(void*)_tmp202.elt_typ;_LLDC: return Cyc_Toc_is_poly_field(
_tmp203,_tmp200);_LLDD:;_LLDE: {const char*_tmp993;void*_tmp992[1];struct Cyc_String_pa_struct
_tmp991;(_tmp991.tag=0,((_tmp991.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(_tmp1FF->topt))->v)),((
_tmp992[0]=& _tmp991,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmp993="is_poly_project: bad type %s",_tag_dyneither(_tmp993,sizeof(char),29))),
_tag_dyneither(_tmp992,sizeof(void*),1)))))));}_LLDA:;}_LLD8:;_LLD9: return 0;
_LLD3:;}static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_ptr(struct Cyc_Absyn_Exp*s);
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_ptr(struct Cyc_Absyn_Exp*s){struct Cyc_List_List*
_tmp994;return Cyc_Absyn_fncall_exp(Cyc_Toc__cycalloc_e,((_tmp994=_cycalloc(
sizeof(*_tmp994)),((_tmp994->hd=s,((_tmp994->tl=0,_tmp994)))))),0);}static struct
Cyc_Absyn_Exp*Cyc_Toc_malloc_atomic(struct Cyc_Absyn_Exp*s);static struct Cyc_Absyn_Exp*
Cyc_Toc_malloc_atomic(struct Cyc_Absyn_Exp*s){struct Cyc_List_List*_tmp995;return
Cyc_Absyn_fncall_exp(Cyc_Toc__cycalloc_atomic_e,((_tmp995=_cycalloc(sizeof(*
_tmp995)),((_tmp995->hd=s,((_tmp995->tl=0,_tmp995)))))),0);}static struct Cyc_Absyn_Exp*
Cyc_Toc_malloc_exp(void*t,struct Cyc_Absyn_Exp*s);static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_exp(
void*t,struct Cyc_Absyn_Exp*s){if(Cyc_Toc_atomic_typ(t))return Cyc_Toc_malloc_atomic(
s);return Cyc_Toc_malloc_ptr(s);}static struct Cyc_Absyn_Exp*Cyc_Toc_rmalloc_exp(
struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s);static struct Cyc_Absyn_Exp*Cyc_Toc_rmalloc_exp(
struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s){struct Cyc_Absyn_Exp*_tmp996[2];
return Cyc_Absyn_fncall_exp(Cyc_Toc__region_malloc_e,((_tmp996[1]=s,((_tmp996[0]=
rgn,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmp996,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}static struct Cyc_Absyn_Exp*
Cyc_Toc_calloc_exp(void*elt_type,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n);
static struct Cyc_Absyn_Exp*Cyc_Toc_calloc_exp(void*elt_type,struct Cyc_Absyn_Exp*s,
struct Cyc_Absyn_Exp*n){if(Cyc_Toc_atomic_typ(elt_type)){struct Cyc_Absyn_Exp*
_tmp997[2];return Cyc_Absyn_fncall_exp(Cyc_Toc__cyccalloc_atomic_e,((_tmp997[1]=n,((
_tmp997[0]=s,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmp997,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}else{struct Cyc_Absyn_Exp*
_tmp998[2];return Cyc_Absyn_fncall_exp(Cyc_Toc__cyccalloc_e,((_tmp998[1]=n,((
_tmp998[0]=s,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmp998,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}}static struct Cyc_Absyn_Exp*
Cyc_Toc_rcalloc_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*
n);static struct Cyc_Absyn_Exp*Cyc_Toc_rcalloc_exp(struct Cyc_Absyn_Exp*rgn,struct
Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n){struct Cyc_Absyn_Exp*_tmp999[3];return Cyc_Absyn_fncall_exp(
Cyc_Toc__region_calloc_e,((_tmp999[2]=n,((_tmp999[1]=s,((_tmp999[0]=rgn,((struct
Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp999,
sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);}static struct Cyc_Absyn_Exp*Cyc_Toc_newthrow_exp(
struct Cyc_Absyn_Exp*e);static struct Cyc_Absyn_Exp*Cyc_Toc_newthrow_exp(struct Cyc_Absyn_Exp*
e){struct Cyc_List_List*_tmp99A;return Cyc_Absyn_fncall_exp(Cyc_Toc__throw_e,((
_tmp99A=_cycalloc(sizeof(*_tmp99A)),((_tmp99A->hd=e,((_tmp99A->tl=0,_tmp99A)))))),
0);}static struct Cyc_Absyn_Stmt*Cyc_Toc_throw_match_stmt();static struct Cyc_Absyn_Stmt*
Cyc_Toc_throw_match_stmt(){static struct Cyc_Absyn_Stmt**throw_match_stmt_opt=0;
if(throw_match_stmt_opt == 0){struct Cyc_Absyn_Stmt**_tmp99B;throw_match_stmt_opt=((
_tmp99B=_cycalloc(sizeof(*_tmp99B)),((_tmp99B[0]=Cyc_Absyn_exp_stmt(Cyc_Toc_newthrow_exp(
Cyc_Absyn_match_exn_exp(0)),0),_tmp99B))));}return*throw_match_stmt_opt;}static
struct Cyc_Absyn_Exp*Cyc_Toc_make_toplevel_dyn_arr(void*t,struct Cyc_Absyn_Exp*sz,
struct Cyc_Absyn_Exp*e);static struct Cyc_Absyn_Exp*Cyc_Toc_make_toplevel_dyn_arr(
void*t,struct Cyc_Absyn_Exp*sz,struct Cyc_Absyn_Exp*e){int is_string=0;{void*
_tmp20F=(void*)e->r;union Cyc_Absyn_Cnst_union _tmp210;_LLE0: if(*((int*)_tmp20F)!= 
0)goto _LLE2;_tmp210=((struct Cyc_Absyn_Const_e_struct*)_tmp20F)->f1;if(((((struct
Cyc_Absyn_Const_e_struct*)_tmp20F)->f1).String_c).tag != 5)goto _LLE2;_LLE1:
is_string=1;goto _LLDF;_LLE2:;_LLE3: goto _LLDF;_LLDF:;}{struct Cyc_Absyn_Exp*xexp;
struct Cyc_Absyn_Exp*xplussz;if(is_string){struct _tuple1*x=Cyc_Toc_temp_var();
void*vd_typ=Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq,(struct Cyc_Absyn_Exp*)
sz,Cyc_Absyn_false_conref,0);struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_static_vardecl(
x,vd_typ,(struct Cyc_Absyn_Exp*)e);{struct Cyc_Absyn_Var_d_struct*_tmp9A1;struct
Cyc_Absyn_Var_d_struct _tmp9A0;struct Cyc_List_List*_tmp99F;Cyc_Toc_result_decls=((
_tmp99F=_cycalloc(sizeof(*_tmp99F)),((_tmp99F->hd=Cyc_Absyn_new_decl((void*)((
_tmp9A1=_cycalloc(sizeof(*_tmp9A1)),((_tmp9A1[0]=((_tmp9A0.tag=0,((_tmp9A0.f1=vd,
_tmp9A0)))),_tmp9A1)))),0),((_tmp99F->tl=Cyc_Toc_result_decls,_tmp99F))))));}
xexp=Cyc_Absyn_var_exp(x,0);xplussz=Cyc_Absyn_add_exp(xexp,sz,0);}else{xexp=Cyc_Toc_cast_it(
Cyc_Absyn_void_star_typ(),e);xplussz=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),
Cyc_Absyn_add_exp(e,sz,0));}{struct Cyc_Absyn_Exp*urm_exp;{struct _tuple8*_tmp9A8;
struct _tuple8*_tmp9A7;struct _tuple8*_tmp9A6;struct _tuple8*_tmp9A5[3];urm_exp=Cyc_Absyn_unresolvedmem_exp(
0,((_tmp9A5[2]=((_tmp9A8=_cycalloc(sizeof(*_tmp9A8)),((_tmp9A8->f1=0,((_tmp9A8->f2=
xplussz,_tmp9A8)))))),((_tmp9A5[1]=((_tmp9A7=_cycalloc(sizeof(*_tmp9A7)),((
_tmp9A7->f1=0,((_tmp9A7->f2=xexp,_tmp9A7)))))),((_tmp9A5[0]=((_tmp9A6=_cycalloc(
sizeof(*_tmp9A6)),((_tmp9A6->f1=0,((_tmp9A6->f2=xexp,_tmp9A6)))))),((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp9A5,sizeof(struct _tuple8*),
3)))))))),0);}return urm_exp;}}}struct Cyc_Toc_FallthruInfo{struct _dyneither_ptr*
label;struct Cyc_List_List*binders;struct Cyc_Dict_Dict next_case_env;};struct Cyc_Toc_Env{
struct _dyneither_ptr**break_lab;struct _dyneither_ptr**continue_lab;struct Cyc_Toc_FallthruInfo*
fallthru_info;struct Cyc_Dict_Dict varmap;int toplevel;struct _RegionHandle*rgn;};
static int Cyc_Toc_is_toplevel(struct Cyc_Toc_Env*nv);static int Cyc_Toc_is_toplevel(
struct Cyc_Toc_Env*nv){struct Cyc_Toc_Env _tmp219;int _tmp21A;struct Cyc_Toc_Env*
_tmp218=nv;_tmp219=*_tmp218;_tmp21A=_tmp219.toplevel;return _tmp21A;}static struct
Cyc_Absyn_Exp*Cyc_Toc_lookup_varmap(struct Cyc_Toc_Env*nv,struct _tuple1*x);static
struct Cyc_Absyn_Exp*Cyc_Toc_lookup_varmap(struct Cyc_Toc_Env*nv,struct _tuple1*x){
struct Cyc_Toc_Env _tmp21C;struct Cyc_Dict_Dict _tmp21D;struct Cyc_Toc_Env*_tmp21B=nv;
_tmp21C=*_tmp21B;_tmp21D=_tmp21C.varmap;return((struct Cyc_Absyn_Exp*(*)(struct
Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup)(_tmp21D,x);}static struct Cyc_Toc_Env*
Cyc_Toc_empty_env(struct _RegionHandle*r);static struct Cyc_Toc_Env*Cyc_Toc_empty_env(
struct _RegionHandle*r){struct Cyc_Toc_Env*_tmp9A9;return(_tmp9A9=_region_malloc(r,
sizeof(*_tmp9A9)),((_tmp9A9->break_lab=(struct _dyneither_ptr**)0,((_tmp9A9->continue_lab=(
struct _dyneither_ptr**)0,((_tmp9A9->fallthru_info=(struct Cyc_Toc_FallthruInfo*)0,((
_tmp9A9->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_rempty)(r,Cyc_Absyn_qvar_cmp),((
_tmp9A9->toplevel=(int)1,((_tmp9A9->rgn=(struct _RegionHandle*)r,_tmp9A9)))))))))))));}
static struct Cyc_Toc_Env*Cyc_Toc_share_env(struct _RegionHandle*r,struct Cyc_Toc_Env*
e);static struct Cyc_Toc_Env*Cyc_Toc_share_env(struct _RegionHandle*r,struct Cyc_Toc_Env*
e){struct Cyc_Toc_Env _tmp220;struct _dyneither_ptr**_tmp221;struct _dyneither_ptr**
_tmp222;struct Cyc_Toc_FallthruInfo*_tmp223;struct Cyc_Dict_Dict _tmp224;int _tmp225;
struct Cyc_Toc_Env*_tmp21F=e;_tmp220=*_tmp21F;_tmp221=_tmp220.break_lab;_tmp222=
_tmp220.continue_lab;_tmp223=_tmp220.fallthru_info;_tmp224=_tmp220.varmap;
_tmp225=_tmp220.toplevel;{struct Cyc_Toc_Env*_tmp9AA;return(_tmp9AA=
_region_malloc(r,sizeof(*_tmp9AA)),((_tmp9AA->break_lab=(struct _dyneither_ptr**)((
struct _dyneither_ptr**)_tmp221),((_tmp9AA->continue_lab=(struct _dyneither_ptr**)((
struct _dyneither_ptr**)_tmp222),((_tmp9AA->fallthru_info=(struct Cyc_Toc_FallthruInfo*)
_tmp223,((_tmp9AA->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp224),((_tmp9AA->toplevel=(
int)_tmp225,((_tmp9AA->rgn=(struct _RegionHandle*)r,_tmp9AA)))))))))))));}}static
struct Cyc_Toc_Env*Cyc_Toc_clear_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*
e);static struct Cyc_Toc_Env*Cyc_Toc_clear_toplevel(struct _RegionHandle*r,struct
Cyc_Toc_Env*e){struct Cyc_Toc_Env _tmp228;struct _dyneither_ptr**_tmp229;struct
_dyneither_ptr**_tmp22A;struct Cyc_Toc_FallthruInfo*_tmp22B;struct Cyc_Dict_Dict
_tmp22C;int _tmp22D;struct Cyc_Toc_Env*_tmp227=e;_tmp228=*_tmp227;_tmp229=_tmp228.break_lab;
_tmp22A=_tmp228.continue_lab;_tmp22B=_tmp228.fallthru_info;_tmp22C=_tmp228.varmap;
_tmp22D=_tmp228.toplevel;{struct Cyc_Toc_Env*_tmp9AB;return(_tmp9AB=
_region_malloc(r,sizeof(*_tmp9AB)),((_tmp9AB->break_lab=(struct _dyneither_ptr**)((
struct _dyneither_ptr**)_tmp229),((_tmp9AB->continue_lab=(struct _dyneither_ptr**)((
struct _dyneither_ptr**)_tmp22A),((_tmp9AB->fallthru_info=(struct Cyc_Toc_FallthruInfo*)
_tmp22B,((_tmp9AB->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp22C),((_tmp9AB->toplevel=(
int)0,((_tmp9AB->rgn=(struct _RegionHandle*)r,_tmp9AB)))))))))))));}}static struct
Cyc_Toc_Env*Cyc_Toc_set_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*e);
static struct Cyc_Toc_Env*Cyc_Toc_set_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*
e){struct Cyc_Toc_Env _tmp230;struct _dyneither_ptr**_tmp231;struct _dyneither_ptr**
_tmp232;struct Cyc_Toc_FallthruInfo*_tmp233;struct Cyc_Dict_Dict _tmp234;int _tmp235;
struct Cyc_Toc_Env*_tmp22F=e;_tmp230=*_tmp22F;_tmp231=_tmp230.break_lab;_tmp232=
_tmp230.continue_lab;_tmp233=_tmp230.fallthru_info;_tmp234=_tmp230.varmap;
_tmp235=_tmp230.toplevel;{struct Cyc_Toc_Env*_tmp9AC;return(_tmp9AC=
_region_malloc(r,sizeof(*_tmp9AC)),((_tmp9AC->break_lab=(struct _dyneither_ptr**)((
struct _dyneither_ptr**)_tmp231),((_tmp9AC->continue_lab=(struct _dyneither_ptr**)((
struct _dyneither_ptr**)_tmp232),((_tmp9AC->fallthru_info=(struct Cyc_Toc_FallthruInfo*)
_tmp233,((_tmp9AC->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp234),((_tmp9AC->toplevel=(
int)1,((_tmp9AC->rgn=(struct _RegionHandle*)r,_tmp9AC)))))))))))));}}static struct
Cyc_Toc_Env*Cyc_Toc_add_varmap(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct
_tuple1*x,struct Cyc_Absyn_Exp*y);static struct Cyc_Toc_Env*Cyc_Toc_add_varmap(
struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _tuple1*x,struct Cyc_Absyn_Exp*y){{
union Cyc_Absyn_Nmspace_union _tmp237=(*x).f1;_LLE5: if((_tmp237.Rel_n).tag != 1)
goto _LLE7;_LLE6: {const char*_tmp9B0;void*_tmp9AF[1];struct Cyc_String_pa_struct
_tmp9AE;(_tmp9AE.tag=0,((_tmp9AE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(x)),((_tmp9AF[0]=& _tmp9AE,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmp9B0="Toc::add_varmap on Rel_n: %s\n",
_tag_dyneither(_tmp9B0,sizeof(char),30))),_tag_dyneither(_tmp9AF,sizeof(void*),1)))))));}
_LLE7:;_LLE8: goto _LLE4;_LLE4:;}{struct Cyc_Toc_Env _tmp23C;struct _dyneither_ptr**
_tmp23D;struct _dyneither_ptr**_tmp23E;struct Cyc_Toc_FallthruInfo*_tmp23F;struct
Cyc_Dict_Dict _tmp240;int _tmp241;struct Cyc_Toc_Env*_tmp23B=e;_tmp23C=*_tmp23B;
_tmp23D=_tmp23C.break_lab;_tmp23E=_tmp23C.continue_lab;_tmp23F=_tmp23C.fallthru_info;
_tmp240=_tmp23C.varmap;_tmp241=_tmp23C.toplevel;{struct Cyc_Dict_Dict _tmp242=((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,struct Cyc_Absyn_Exp*
v))Cyc_Dict_insert)(((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))
Cyc_Dict_rshare)(r,_tmp240),x,y);struct Cyc_Toc_Env*_tmp9B1;return(_tmp9B1=
_region_malloc(r,sizeof(*_tmp9B1)),((_tmp9B1->break_lab=(struct _dyneither_ptr**)((
struct _dyneither_ptr**)_tmp23D),((_tmp9B1->continue_lab=(struct _dyneither_ptr**)((
struct _dyneither_ptr**)_tmp23E),((_tmp9B1->fallthru_info=(struct Cyc_Toc_FallthruInfo*)
_tmp23F,((_tmp9B1->varmap=(struct Cyc_Dict_Dict)_tmp242,((_tmp9B1->toplevel=(int)
_tmp241,((_tmp9B1->rgn=(struct _RegionHandle*)r,_tmp9B1)))))))))))));}}}static
struct Cyc_Toc_Env*Cyc_Toc_loop_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e);
static struct Cyc_Toc_Env*Cyc_Toc_loop_env(struct _RegionHandle*r,struct Cyc_Toc_Env*
e){struct Cyc_Toc_Env _tmp245;struct _dyneither_ptr**_tmp246;struct _dyneither_ptr**
_tmp247;struct Cyc_Toc_FallthruInfo*_tmp248;struct Cyc_Dict_Dict _tmp249;int _tmp24A;
struct Cyc_Toc_Env*_tmp244=e;_tmp245=*_tmp244;_tmp246=_tmp245.break_lab;_tmp247=
_tmp245.continue_lab;_tmp248=_tmp245.fallthru_info;_tmp249=_tmp245.varmap;
_tmp24A=_tmp245.toplevel;{struct Cyc_Toc_Env*_tmp9B2;return(_tmp9B2=
_region_malloc(r,sizeof(*_tmp9B2)),((_tmp9B2->break_lab=(struct _dyneither_ptr**)
0,((_tmp9B2->continue_lab=(struct _dyneither_ptr**)0,((_tmp9B2->fallthru_info=(
struct Cyc_Toc_FallthruInfo*)_tmp248,((_tmp9B2->varmap=(struct Cyc_Dict_Dict)((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(
r,_tmp249),((_tmp9B2->toplevel=(int)_tmp24A,((_tmp9B2->rgn=(struct _RegionHandle*)
r,_tmp9B2)))))))))))));}}static struct Cyc_Toc_Env*Cyc_Toc_non_last_switchclause_env(
struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*break_l,struct
_dyneither_ptr*fallthru_l,struct Cyc_List_List*fallthru_binders,struct Cyc_Toc_Env*
next_case_env);static struct Cyc_Toc_Env*Cyc_Toc_non_last_switchclause_env(struct
_RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*break_l,struct
_dyneither_ptr*fallthru_l,struct Cyc_List_List*fallthru_binders,struct Cyc_Toc_Env*
next_case_env){struct Cyc_List_List*fallthru_vars=0;for(0;fallthru_binders != 0;
fallthru_binders=fallthru_binders->tl){struct Cyc_List_List*_tmp9B3;fallthru_vars=((
_tmp9B3=_region_malloc(r,sizeof(*_tmp9B3)),((_tmp9B3->hd=((struct Cyc_Absyn_Vardecl*)
fallthru_binders->hd)->name,((_tmp9B3->tl=fallthru_vars,_tmp9B3))))));}
fallthru_vars=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
fallthru_vars);{struct Cyc_Toc_Env _tmp24E;struct _dyneither_ptr**_tmp24F;struct
_dyneither_ptr**_tmp250;struct Cyc_Toc_FallthruInfo*_tmp251;struct Cyc_Dict_Dict
_tmp252;int _tmp253;struct Cyc_Toc_Env*_tmp24D=e;_tmp24E=*_tmp24D;_tmp24F=_tmp24E.break_lab;
_tmp250=_tmp24E.continue_lab;_tmp251=_tmp24E.fallthru_info;_tmp252=_tmp24E.varmap;
_tmp253=_tmp24E.toplevel;{struct Cyc_Toc_Env _tmp255;struct Cyc_Dict_Dict _tmp256;
struct Cyc_Toc_Env*_tmp254=next_case_env;_tmp255=*_tmp254;_tmp256=_tmp255.varmap;{
struct Cyc_Toc_FallthruInfo*_tmp9B4;struct Cyc_Toc_FallthruInfo*fi=(_tmp9B4=
_region_malloc(r,sizeof(*_tmp9B4)),((_tmp9B4->label=fallthru_l,((_tmp9B4->binders=
fallthru_vars,((_tmp9B4->next_case_env=((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp256),_tmp9B4)))))));
struct _dyneither_ptr**_tmp9B7;struct Cyc_Toc_Env*_tmp9B6;return(_tmp9B6=
_region_malloc(r,sizeof(*_tmp9B6)),((_tmp9B6->break_lab=(struct _dyneither_ptr**)((
_tmp9B7=_region_malloc(r,sizeof(*_tmp9B7)),((_tmp9B7[0]=break_l,_tmp9B7)))),((
_tmp9B6->continue_lab=(struct _dyneither_ptr**)((struct _dyneither_ptr**)_tmp250),((
_tmp9B6->fallthru_info=(struct Cyc_Toc_FallthruInfo*)fi,((_tmp9B6->varmap=(struct
Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))
Cyc_Dict_rshare)(r,_tmp252),((_tmp9B6->toplevel=(int)_tmp253,((_tmp9B6->rgn=(
struct _RegionHandle*)r,_tmp9B6)))))))))))));}}}}static struct Cyc_Toc_Env*Cyc_Toc_last_switchclause_env(
struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*break_l);static
struct Cyc_Toc_Env*Cyc_Toc_last_switchclause_env(struct _RegionHandle*r,struct Cyc_Toc_Env*
e,struct _dyneither_ptr*break_l){struct Cyc_Toc_Env _tmp25B;struct _dyneither_ptr**
_tmp25C;struct _dyneither_ptr**_tmp25D;struct Cyc_Toc_FallthruInfo*_tmp25E;struct
Cyc_Dict_Dict _tmp25F;int _tmp260;struct Cyc_Toc_Env*_tmp25A=e;_tmp25B=*_tmp25A;
_tmp25C=_tmp25B.break_lab;_tmp25D=_tmp25B.continue_lab;_tmp25E=_tmp25B.fallthru_info;
_tmp25F=_tmp25B.varmap;_tmp260=_tmp25B.toplevel;{struct _dyneither_ptr**_tmp9BA;
struct Cyc_Toc_Env*_tmp9B9;return(_tmp9B9=_region_malloc(r,sizeof(*_tmp9B9)),((
_tmp9B9->break_lab=(struct _dyneither_ptr**)((_tmp9BA=_region_malloc(r,sizeof(*
_tmp9BA)),((_tmp9BA[0]=break_l,_tmp9BA)))),((_tmp9B9->continue_lab=(struct
_dyneither_ptr**)((struct _dyneither_ptr**)_tmp25D),((_tmp9B9->fallthru_info=(
struct Cyc_Toc_FallthruInfo*)0,((_tmp9B9->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(
struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp25F),((_tmp9B9->toplevel=(
int)_tmp260,((_tmp9B9->rgn=(struct _RegionHandle*)r,_tmp9B9)))))))))))));}}static
struct Cyc_Toc_Env*Cyc_Toc_switch_as_switch_env(struct _RegionHandle*r,struct Cyc_Toc_Env*
e,struct _dyneither_ptr*next_l);static struct Cyc_Toc_Env*Cyc_Toc_switch_as_switch_env(
struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*next_l){struct Cyc_Toc_Env
_tmp264;struct _dyneither_ptr**_tmp265;struct _dyneither_ptr**_tmp266;struct Cyc_Toc_FallthruInfo*
_tmp267;struct Cyc_Dict_Dict _tmp268;int _tmp269;struct Cyc_Toc_Env*_tmp263=e;
_tmp264=*_tmp263;_tmp265=_tmp264.break_lab;_tmp266=_tmp264.continue_lab;_tmp267=
_tmp264.fallthru_info;_tmp268=_tmp264.varmap;_tmp269=_tmp264.toplevel;{struct Cyc_Toc_FallthruInfo*
_tmp9BD;struct Cyc_Toc_Env*_tmp9BC;return(_tmp9BC=_region_malloc(r,sizeof(*
_tmp9BC)),((_tmp9BC->break_lab=(struct _dyneither_ptr**)0,((_tmp9BC->continue_lab=(
struct _dyneither_ptr**)((struct _dyneither_ptr**)_tmp266),((_tmp9BC->fallthru_info=(
struct Cyc_Toc_FallthruInfo*)((_tmp9BD=_region_malloc(r,sizeof(*_tmp9BD)),((
_tmp9BD->label=next_l,((_tmp9BD->binders=0,((_tmp9BD->next_case_env=((struct Cyc_Dict_Dict(*)(
struct _RegionHandle*,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_rempty)(
r,Cyc_Absyn_qvar_cmp),_tmp9BD)))))))),((_tmp9BC->varmap=(struct Cyc_Dict_Dict)((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(
r,_tmp268),((_tmp9BC->toplevel=(int)_tmp269,((_tmp9BC->rgn=(struct _RegionHandle*)
r,_tmp9BC)))))))))))));}}static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct
Cyc_Absyn_Exp*e);static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*
s);static int Cyc_Toc_need_null_check(struct Cyc_Absyn_Exp*e);static int Cyc_Toc_need_null_check(
struct Cyc_Absyn_Exp*e){void*_tmp26C=(void*)e->annot;_LLEA: if(*((void**)_tmp26C)
!= Cyc_CfFlowInfo_UnknownZ)goto _LLEC;_LLEB: return Cyc_Toc_is_nullable((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v);_LLEC: if(*((void**)_tmp26C)!= Cyc_CfFlowInfo_NotZero)
goto _LLEE;_LLED: return 0;_LLEE: if(_tmp26C != Cyc_CfFlowInfo_IsZero)goto _LLF0;_LLEF:{
const char*_tmp9C0;void*_tmp9BF;(_tmp9BF=0,Cyc_Tcutil_terr(e->loc,((_tmp9C0="dereference of NULL pointer",
_tag_dyneither(_tmp9C0,sizeof(char),28))),_tag_dyneither(_tmp9BF,sizeof(void*),0)));}
return 0;_LLF0: if(_tmp26C != Cyc_Absyn_EmptyAnnot)goto _LLF2;_LLF1: return 0;_LLF2:
if(*((void**)_tmp26C)!= Cyc_CfFlowInfo_HasTagCmps)goto _LLF4;_LLF3:{const char*
_tmp9C3;void*_tmp9C2;(_tmp9C2=0,Cyc_Tcutil_warn(e->loc,((_tmp9C3="compiler oddity: pointer compared to tag type",
_tag_dyneither(_tmp9C3,sizeof(char),46))),_tag_dyneither(_tmp9C2,sizeof(void*),0)));}
return Cyc_Toc_is_nullable((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
_LLF4:;_LLF5: {const char*_tmp9C6;void*_tmp9C5;(_tmp9C5=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmp9C6="need_null_check",
_tag_dyneither(_tmp9C6,sizeof(char),16))),_tag_dyneither(_tmp9C5,sizeof(void*),0)));}
_LLE9:;}static struct Cyc_List_List*Cyc_Toc_get_relns(struct Cyc_Absyn_Exp*e);
static struct Cyc_List_List*Cyc_Toc_get_relns(struct Cyc_Absyn_Exp*e){void*_tmp273=(
void*)e->annot;struct Cyc_List_List*_tmp274;struct Cyc_List_List*_tmp275;_LLF7: if(*((
void**)_tmp273)!= Cyc_CfFlowInfo_UnknownZ)goto _LLF9;_tmp274=((struct Cyc_CfFlowInfo_UnknownZ_struct*)
_tmp273)->f1;_LLF8: return _tmp274;_LLF9: if(*((void**)_tmp273)!= Cyc_CfFlowInfo_NotZero)
goto _LLFB;_tmp275=((struct Cyc_CfFlowInfo_NotZero_struct*)_tmp273)->f1;_LLFA:
return _tmp275;_LLFB: if(_tmp273 != Cyc_CfFlowInfo_IsZero)goto _LLFD;_LLFC:{const
char*_tmp9C9;void*_tmp9C8;(_tmp9C8=0,Cyc_Tcutil_terr(e->loc,((_tmp9C9="dereference of NULL pointer",
_tag_dyneither(_tmp9C9,sizeof(char),28))),_tag_dyneither(_tmp9C8,sizeof(void*),0)));}
return 0;_LLFD: if(*((void**)_tmp273)!= Cyc_CfFlowInfo_HasTagCmps)goto _LLFF;_LLFE:
goto _LL100;_LLFF: if(_tmp273 != Cyc_Absyn_EmptyAnnot)goto _LL101;_LL100: return 0;
_LL101:;_LL102: {const char*_tmp9CC;void*_tmp9CB;(_tmp9CB=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmp9CC="get_relns",
_tag_dyneither(_tmp9CC,sizeof(char),10))),_tag_dyneither(_tmp9CB,sizeof(void*),0)));}
_LLF6:;}static int Cyc_Toc_check_const_array(unsigned int i,void*t);static int Cyc_Toc_check_const_array(
unsigned int i,void*t){void*_tmp27A=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo
_tmp27B;struct Cyc_Absyn_PtrAtts _tmp27C;struct Cyc_Absyn_Conref*_tmp27D;struct Cyc_Absyn_Conref*
_tmp27E;struct Cyc_Absyn_ArrayInfo _tmp27F;struct Cyc_Absyn_Exp*_tmp280;_LL104: if(
_tmp27A <= (void*)4)goto _LL108;if(*((int*)_tmp27A)!= 4)goto _LL106;_tmp27B=((
struct Cyc_Absyn_PointerType_struct*)_tmp27A)->f1;_tmp27C=_tmp27B.ptr_atts;
_tmp27D=_tmp27C.bounds;_tmp27E=_tmp27C.zero_term;_LL105: {void*_tmp281=Cyc_Absyn_conref_def(
Cyc_Absyn_bounds_one,_tmp27D);struct Cyc_Absyn_Exp*_tmp282;_LL10B: if((int)_tmp281
!= 0)goto _LL10D;_LL10C: return 0;_LL10D: if(_tmp281 <= (void*)1)goto _LL10A;if(*((int*)
_tmp281)!= 0)goto _LL10A;_tmp282=((struct Cyc_Absyn_Upper_b_struct*)_tmp281)->f1;
_LL10E: {unsigned int _tmp284;int _tmp285;struct _tuple5 _tmp283=Cyc_Evexp_eval_const_uint_exp(
_tmp282);_tmp284=_tmp283.f1;_tmp285=_tmp283.f2;return _tmp285  && i <= _tmp284;}
_LL10A:;}_LL106: if(*((int*)_tmp27A)!= 7)goto _LL108;_tmp27F=((struct Cyc_Absyn_ArrayType_struct*)
_tmp27A)->f1;_tmp280=_tmp27F.num_elts;_LL107: if(_tmp280 == 0)return 0;{
unsigned int _tmp287;int _tmp288;struct _tuple5 _tmp286=Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)_tmp280);_tmp287=_tmp286.f1;_tmp288=_tmp286.f2;return
_tmp288  && i <= _tmp287;}_LL108:;_LL109: return 0;_LL103:;}static int Cyc_Toc_check_leq_size_var(
struct Cyc_List_List*relns,struct Cyc_Absyn_Vardecl*v,struct Cyc_Absyn_Vardecl*y);
static int Cyc_Toc_check_leq_size_var(struct Cyc_List_List*relns,struct Cyc_Absyn_Vardecl*
v,struct Cyc_Absyn_Vardecl*y){for(0;relns != 0;relns=relns->tl){struct Cyc_CfFlowInfo_Reln*
_tmp289=(struct Cyc_CfFlowInfo_Reln*)relns->hd;if(_tmp289->vd != y)continue;{union
Cyc_CfFlowInfo_RelnOp_union _tmp28A=_tmp289->rop;struct Cyc_Absyn_Vardecl*_tmp28B;
struct Cyc_Absyn_Vardecl*_tmp28C;_LL110: if((_tmp28A.LessNumelts).tag != 2)goto
_LL112;_tmp28B=(_tmp28A.LessNumelts).f1;_LL111: _tmp28C=_tmp28B;goto _LL113;_LL112:
if((_tmp28A.LessEqNumelts).tag != 4)goto _LL114;_tmp28C=(_tmp28A.LessEqNumelts).f1;
_LL113: if(_tmp28C == v)return 1;else{goto _LL10F;}_LL114:;_LL115: continue;_LL10F:;}}
return 0;}static int Cyc_Toc_check_leq_size(struct Cyc_List_List*relns,struct Cyc_Absyn_Vardecl*
v,struct Cyc_Absyn_Exp*e);static int Cyc_Toc_check_leq_size(struct Cyc_List_List*
relns,struct Cyc_Absyn_Vardecl*v,struct Cyc_Absyn_Exp*e){{void*_tmp28D=(void*)e->r;
void*_tmp28E;struct Cyc_Absyn_Vardecl*_tmp28F;void*_tmp290;struct Cyc_Absyn_Vardecl*
_tmp291;void*_tmp292;struct Cyc_Absyn_Vardecl*_tmp293;void*_tmp294;struct Cyc_Absyn_Vardecl*
_tmp295;void*_tmp296;struct Cyc_List_List*_tmp297;struct Cyc_List_List _tmp298;
struct Cyc_Absyn_Exp*_tmp299;_LL117: if(*((int*)_tmp28D)!= 1)goto _LL119;_tmp28E=(
void*)((struct Cyc_Absyn_Var_e_struct*)_tmp28D)->f2;if(_tmp28E <= (void*)1)goto
_LL119;if(*((int*)_tmp28E)!= 4)goto _LL119;_tmp28F=((struct Cyc_Absyn_Pat_b_struct*)
_tmp28E)->f1;_LL118: _tmp291=_tmp28F;goto _LL11A;_LL119: if(*((int*)_tmp28D)!= 1)
goto _LL11B;_tmp290=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp28D)->f2;if(
_tmp290 <= (void*)1)goto _LL11B;if(*((int*)_tmp290)!= 3)goto _LL11B;_tmp291=((
struct Cyc_Absyn_Local_b_struct*)_tmp290)->f1;_LL11A: _tmp293=_tmp291;goto _LL11C;
_LL11B: if(*((int*)_tmp28D)!= 1)goto _LL11D;_tmp292=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp28D)->f2;if(_tmp292 <= (void*)1)goto _LL11D;if(*((int*)_tmp292)!= 0)goto _LL11D;
_tmp293=((struct Cyc_Absyn_Global_b_struct*)_tmp292)->f1;_LL11C: _tmp295=_tmp293;
goto _LL11E;_LL11D: if(*((int*)_tmp28D)!= 1)goto _LL11F;_tmp294=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp28D)->f2;if(_tmp294 <= (void*)1)goto _LL11F;if(*((int*)_tmp294)!= 2)goto _LL11F;
_tmp295=((struct Cyc_Absyn_Param_b_struct*)_tmp294)->f1;_LL11E: if(_tmp295->escapes)
return 0;if(Cyc_Toc_check_leq_size_var(relns,v,_tmp295))return 1;goto _LL116;_LL11F:
if(*((int*)_tmp28D)!= 3)goto _LL121;_tmp296=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp28D)->f1;if((int)_tmp296 != 19)goto _LL121;_tmp297=((struct Cyc_Absyn_Primop_e_struct*)
_tmp28D)->f2;if(_tmp297 == 0)goto _LL121;_tmp298=*_tmp297;_tmp299=(struct Cyc_Absyn_Exp*)
_tmp298.hd;_LL120:{void*_tmp29A=(void*)_tmp299->r;void*_tmp29B;struct Cyc_Absyn_Vardecl*
_tmp29C;void*_tmp29D;struct Cyc_Absyn_Vardecl*_tmp29E;void*_tmp29F;struct Cyc_Absyn_Vardecl*
_tmp2A0;void*_tmp2A1;struct Cyc_Absyn_Vardecl*_tmp2A2;_LL124: if(*((int*)_tmp29A)
!= 1)goto _LL126;_tmp29B=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp29A)->f2;if(
_tmp29B <= (void*)1)goto _LL126;if(*((int*)_tmp29B)!= 4)goto _LL126;_tmp29C=((
struct Cyc_Absyn_Pat_b_struct*)_tmp29B)->f1;_LL125: _tmp29E=_tmp29C;goto _LL127;
_LL126: if(*((int*)_tmp29A)!= 1)goto _LL128;_tmp29D=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp29A)->f2;if(_tmp29D <= (void*)1)goto _LL128;if(*((int*)_tmp29D)!= 3)goto _LL128;
_tmp29E=((struct Cyc_Absyn_Local_b_struct*)_tmp29D)->f1;_LL127: _tmp2A0=_tmp29E;
goto _LL129;_LL128: if(*((int*)_tmp29A)!= 1)goto _LL12A;_tmp29F=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp29A)->f2;if(_tmp29F <= (void*)1)goto _LL12A;if(*((int*)_tmp29F)!= 0)goto _LL12A;
_tmp2A0=((struct Cyc_Absyn_Global_b_struct*)_tmp29F)->f1;_LL129: _tmp2A2=_tmp2A0;
goto _LL12B;_LL12A: if(*((int*)_tmp29A)!= 1)goto _LL12C;_tmp2A1=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp29A)->f2;if(_tmp2A1 <= (void*)1)goto _LL12C;if(*((int*)_tmp2A1)!= 2)goto _LL12C;
_tmp2A2=((struct Cyc_Absyn_Param_b_struct*)_tmp2A1)->f1;_LL12B: return _tmp2A2 == v;
_LL12C:;_LL12D: goto _LL123;_LL123:;}goto _LL116;_LL121:;_LL122: goto _LL116;_LL116:;}
return 0;}static int Cyc_Toc_check_bounds(struct Cyc_List_List*relns,struct Cyc_Absyn_Exp*
a,struct Cyc_Absyn_Exp*i);static int Cyc_Toc_check_bounds(struct Cyc_List_List*relns,
struct Cyc_Absyn_Exp*a,struct Cyc_Absyn_Exp*i){{void*_tmp2A3=(void*)a->r;void*
_tmp2A4;struct Cyc_Absyn_Vardecl*_tmp2A5;void*_tmp2A6;struct Cyc_Absyn_Vardecl*
_tmp2A7;void*_tmp2A8;struct Cyc_Absyn_Vardecl*_tmp2A9;void*_tmp2AA;struct Cyc_Absyn_Vardecl*
_tmp2AB;_LL12F: if(*((int*)_tmp2A3)!= 1)goto _LL131;_tmp2A4=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp2A3)->f2;if(_tmp2A4 <= (void*)1)goto _LL131;if(*((int*)_tmp2A4)!= 4)goto _LL131;
_tmp2A5=((struct Cyc_Absyn_Pat_b_struct*)_tmp2A4)->f1;_LL130: _tmp2A7=_tmp2A5;goto
_LL132;_LL131: if(*((int*)_tmp2A3)!= 1)goto _LL133;_tmp2A6=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp2A3)->f2;if(_tmp2A6 <= (void*)1)goto _LL133;if(*((int*)_tmp2A6)!= 3)goto _LL133;
_tmp2A7=((struct Cyc_Absyn_Local_b_struct*)_tmp2A6)->f1;_LL132: _tmp2A9=_tmp2A7;
goto _LL134;_LL133: if(*((int*)_tmp2A3)!= 1)goto _LL135;_tmp2A8=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp2A3)->f2;if(_tmp2A8 <= (void*)1)goto _LL135;if(*((int*)_tmp2A8)!= 0)goto _LL135;
_tmp2A9=((struct Cyc_Absyn_Global_b_struct*)_tmp2A8)->f1;_LL134: _tmp2AB=_tmp2A9;
goto _LL136;_LL135: if(*((int*)_tmp2A3)!= 1)goto _LL137;_tmp2AA=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp2A3)->f2;if(_tmp2AA <= (void*)1)goto _LL137;if(*((int*)_tmp2AA)!= 2)goto _LL137;
_tmp2AB=((struct Cyc_Absyn_Param_b_struct*)_tmp2AA)->f1;_LL136: if(_tmp2AB->escapes)
return 0;inner_loop: {void*_tmp2AC=(void*)i->r;void*_tmp2AD;struct Cyc_Absyn_Exp*
_tmp2AE;union Cyc_Absyn_Cnst_union _tmp2AF;void*_tmp2B0;int _tmp2B1;union Cyc_Absyn_Cnst_union
_tmp2B2;void*_tmp2B3;int _tmp2B4;union Cyc_Absyn_Cnst_union _tmp2B5;void*_tmp2B6;
int _tmp2B7;void*_tmp2B8;struct Cyc_List_List*_tmp2B9;struct Cyc_List_List _tmp2BA;
struct Cyc_Absyn_Exp*_tmp2BB;struct Cyc_List_List*_tmp2BC;struct Cyc_List_List
_tmp2BD;struct Cyc_Absyn_Exp*_tmp2BE;void*_tmp2BF;struct Cyc_Absyn_Vardecl*_tmp2C0;
void*_tmp2C1;struct Cyc_Absyn_Vardecl*_tmp2C2;void*_tmp2C3;struct Cyc_Absyn_Vardecl*
_tmp2C4;void*_tmp2C5;struct Cyc_Absyn_Vardecl*_tmp2C6;_LL13A: if(*((int*)_tmp2AC)
!= 15)goto _LL13C;_tmp2AD=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp2AC)->f1;
_tmp2AE=((struct Cyc_Absyn_Cast_e_struct*)_tmp2AC)->f2;_LL13B: i=_tmp2AE;goto
inner_loop;_LL13C: if(*((int*)_tmp2AC)!= 0)goto _LL13E;_tmp2AF=((struct Cyc_Absyn_Const_e_struct*)
_tmp2AC)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)_tmp2AC)->f1).Int_c).tag != 2)
goto _LL13E;_tmp2B0=(_tmp2AF.Int_c).f1;if((int)_tmp2B0 != 2)goto _LL13E;_tmp2B1=(
_tmp2AF.Int_c).f2;_LL13D: _tmp2B4=_tmp2B1;goto _LL13F;_LL13E: if(*((int*)_tmp2AC)!= 
0)goto _LL140;_tmp2B2=((struct Cyc_Absyn_Const_e_struct*)_tmp2AC)->f1;if(((((
struct Cyc_Absyn_Const_e_struct*)_tmp2AC)->f1).Int_c).tag != 2)goto _LL140;_tmp2B3=(
_tmp2B2.Int_c).f1;if((int)_tmp2B3 != 0)goto _LL140;_tmp2B4=(_tmp2B2.Int_c).f2;
_LL13F: return _tmp2B4 >= 0  && Cyc_Toc_check_const_array((unsigned int)(_tmp2B4 + 1),(
void*)_tmp2AB->type);_LL140: if(*((int*)_tmp2AC)!= 0)goto _LL142;_tmp2B5=((struct
Cyc_Absyn_Const_e_struct*)_tmp2AC)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)
_tmp2AC)->f1).Int_c).tag != 2)goto _LL142;_tmp2B6=(_tmp2B5.Int_c).f1;if((int)
_tmp2B6 != 1)goto _LL142;_tmp2B7=(_tmp2B5.Int_c).f2;_LL141: return Cyc_Toc_check_const_array((
unsigned int)(_tmp2B7 + 1),(void*)_tmp2AB->type);_LL142: if(*((int*)_tmp2AC)!= 3)
goto _LL144;_tmp2B8=(void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp2AC)->f1;if((
int)_tmp2B8 != 4)goto _LL144;_tmp2B9=((struct Cyc_Absyn_Primop_e_struct*)_tmp2AC)->f2;
if(_tmp2B9 == 0)goto _LL144;_tmp2BA=*_tmp2B9;_tmp2BB=(struct Cyc_Absyn_Exp*)_tmp2BA.hd;
_tmp2BC=_tmp2BA.tl;if(_tmp2BC == 0)goto _LL144;_tmp2BD=*_tmp2BC;_tmp2BE=(struct Cyc_Absyn_Exp*)
_tmp2BD.hd;_LL143: return Cyc_Toc_check_leq_size(relns,_tmp2AB,_tmp2BE);_LL144: if(*((
int*)_tmp2AC)!= 1)goto _LL146;_tmp2BF=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp2AC)->f2;if(_tmp2BF <= (void*)1)goto _LL146;if(*((int*)_tmp2BF)!= 4)goto _LL146;
_tmp2C0=((struct Cyc_Absyn_Pat_b_struct*)_tmp2BF)->f1;_LL145: _tmp2C2=_tmp2C0;goto
_LL147;_LL146: if(*((int*)_tmp2AC)!= 1)goto _LL148;_tmp2C1=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp2AC)->f2;if(_tmp2C1 <= (void*)1)goto _LL148;if(*((int*)_tmp2C1)!= 3)goto _LL148;
_tmp2C2=((struct Cyc_Absyn_Local_b_struct*)_tmp2C1)->f1;_LL147: _tmp2C4=_tmp2C2;
goto _LL149;_LL148: if(*((int*)_tmp2AC)!= 1)goto _LL14A;_tmp2C3=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp2AC)->f2;if(_tmp2C3 <= (void*)1)goto _LL14A;if(*((int*)_tmp2C3)!= 0)goto _LL14A;
_tmp2C4=((struct Cyc_Absyn_Global_b_struct*)_tmp2C3)->f1;_LL149: _tmp2C6=_tmp2C4;
goto _LL14B;_LL14A: if(*((int*)_tmp2AC)!= 1)goto _LL14C;_tmp2C5=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp2AC)->f2;if(_tmp2C5 <= (void*)1)goto _LL14C;if(*((int*)_tmp2C5)!= 2)goto _LL14C;
_tmp2C6=((struct Cyc_Absyn_Param_b_struct*)_tmp2C5)->f1;_LL14B: if(_tmp2C6->escapes)
return 0;{struct Cyc_List_List*_tmp2C7=relns;for(0;_tmp2C7 != 0;_tmp2C7=_tmp2C7->tl){
struct Cyc_CfFlowInfo_Reln*_tmp2C8=(struct Cyc_CfFlowInfo_Reln*)_tmp2C7->hd;if(
_tmp2C8->vd == _tmp2C6){union Cyc_CfFlowInfo_RelnOp_union _tmp2C9=_tmp2C8->rop;
struct Cyc_Absyn_Vardecl*_tmp2CA;struct Cyc_Absyn_Vardecl*_tmp2CB;void*_tmp2CC;
unsigned int _tmp2CD;_LL14F: if((_tmp2C9.LessNumelts).tag != 2)goto _LL151;_tmp2CA=(
_tmp2C9.LessNumelts).f1;_LL150: if(_tmp2AB == _tmp2CA)return 1;else{goto _LL14E;}
_LL151: if((_tmp2C9.LessVar).tag != 1)goto _LL153;_tmp2CB=(_tmp2C9.LessVar).f1;
_tmp2CC=(_tmp2C9.LessVar).f2;_LL152:{struct _tuple0 _tmp9CD;struct _tuple0 _tmp2CF=(
_tmp9CD.f1=Cyc_Tcutil_compress(_tmp2CC),((_tmp9CD.f2=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(a->topt))->v),_tmp9CD)));void*_tmp2D0;void*
_tmp2D1;void*_tmp2D2;struct Cyc_Absyn_PtrInfo _tmp2D3;struct Cyc_Absyn_PtrAtts
_tmp2D4;struct Cyc_Absyn_Conref*_tmp2D5;_LL158: _tmp2D0=_tmp2CF.f1;if(_tmp2D0 <= (
void*)4)goto _LL15A;if(*((int*)_tmp2D0)!= 18)goto _LL15A;_tmp2D1=(void*)((struct
Cyc_Absyn_TagType_struct*)_tmp2D0)->f1;_tmp2D2=_tmp2CF.f2;if(_tmp2D2 <= (void*)4)
goto _LL15A;if(*((int*)_tmp2D2)!= 4)goto _LL15A;_tmp2D3=((struct Cyc_Absyn_PointerType_struct*)
_tmp2D2)->f1;_tmp2D4=_tmp2D3.ptr_atts;_tmp2D5=_tmp2D4.bounds;_LL159:{void*
_tmp2D6=Cyc_Absyn_conref_val(_tmp2D5);struct Cyc_Absyn_Exp*_tmp2D7;_LL15D: if(
_tmp2D6 <= (void*)1)goto _LL15F;if(*((int*)_tmp2D6)!= 0)goto _LL15F;_tmp2D7=((
struct Cyc_Absyn_Upper_b_struct*)_tmp2D6)->f1;_LL15E: {struct Cyc_Absyn_Exp*
_tmp2D8=Cyc_Absyn_cast_exp(Cyc_Absyn_uint_typ,Cyc_Absyn_valueof_exp(_tmp2D1,0),0,(
void*)1,0);if(Cyc_Evexp_lte_const_exp(_tmp2D8,_tmp2D7))return 1;goto _LL15C;}
_LL15F:;_LL160: goto _LL15C;_LL15C:;}goto _LL157;_LL15A:;_LL15B: goto _LL157;_LL157:;}{
struct Cyc_List_List*_tmp2D9=relns;for(0;_tmp2D9 != 0;_tmp2D9=_tmp2D9->tl){struct
Cyc_CfFlowInfo_Reln*_tmp2DA=(struct Cyc_CfFlowInfo_Reln*)_tmp2D9->hd;if(_tmp2DA->vd
== _tmp2CB){union Cyc_CfFlowInfo_RelnOp_union _tmp2DB=_tmp2DA->rop;struct Cyc_Absyn_Vardecl*
_tmp2DC;struct Cyc_Absyn_Vardecl*_tmp2DD;unsigned int _tmp2DE;struct Cyc_Absyn_Vardecl*
_tmp2DF;_LL162: if((_tmp2DB.LessEqNumelts).tag != 4)goto _LL164;_tmp2DC=(_tmp2DB.LessEqNumelts).f1;
_LL163: _tmp2DD=_tmp2DC;goto _LL165;_LL164: if((_tmp2DB.LessNumelts).tag != 2)goto
_LL166;_tmp2DD=(_tmp2DB.LessNumelts).f1;_LL165: if(_tmp2AB == _tmp2DD)return 1;goto
_LL161;_LL166: if((_tmp2DB.EqualConst).tag != 0)goto _LL168;_tmp2DE=(_tmp2DB.EqualConst).f1;
_LL167: return Cyc_Toc_check_const_array(_tmp2DE,(void*)_tmp2AB->type);_LL168: if((
_tmp2DB.LessVar).tag != 1)goto _LL16A;_tmp2DF=(_tmp2DB.LessVar).f1;_LL169: if(Cyc_Toc_check_leq_size_var(
relns,_tmp2AB,_tmp2DF))return 1;goto _LL161;_LL16A:;_LL16B: goto _LL161;_LL161:;}}}
goto _LL14E;_LL153: if((_tmp2C9.LessConst).tag != 3)goto _LL155;_tmp2CD=(_tmp2C9.LessConst).f1;
_LL154: return Cyc_Toc_check_const_array(_tmp2CD,(void*)_tmp2AB->type);_LL155:;
_LL156: goto _LL14E;_LL14E:;}}}goto _LL139;_LL14C:;_LL14D: goto _LL139;_LL139:;}goto
_LL12E;_LL137:;_LL138: goto _LL12E;_LL12E:;}return 0;}static void*Cyc_Toc_get_c_typ(
struct Cyc_Absyn_Exp*e);static void*Cyc_Toc_get_c_typ(struct Cyc_Absyn_Exp*e){if(e->topt
== 0){const char*_tmp9D0;void*_tmp9CF;(_tmp9CF=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmp9D0="Missing type in primop ",
_tag_dyneither(_tmp9D0,sizeof(char),24))),_tag_dyneither(_tmp9CF,sizeof(void*),0)));}
return Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);}
static void*Cyc_Toc_get_cyc_typ(struct Cyc_Absyn_Exp*e);static void*Cyc_Toc_get_cyc_typ(
struct Cyc_Absyn_Exp*e){if(e->topt == 0){const char*_tmp9D3;void*_tmp9D2;(_tmp9D2=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmp9D3="Missing type in primop ",_tag_dyneither(_tmp9D3,sizeof(char),24))),
_tag_dyneither(_tmp9D2,sizeof(void*),0)));}return(void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v;}static struct _tuple4*Cyc_Toc_tup_to_c(struct Cyc_Absyn_Exp*
e);static struct _tuple4*Cyc_Toc_tup_to_c(struct Cyc_Absyn_Exp*e){struct _tuple4*
_tmp9D4;return(_tmp9D4=_cycalloc(sizeof(*_tmp9D4)),((_tmp9D4->f1=Cyc_Toc_mt_tq,((
_tmp9D4->f2=Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),
_tmp9D4)))));}static struct _tuple8*Cyc_Toc_add_designator(struct Cyc_Toc_Env*nv,
struct Cyc_Absyn_Exp*e);static struct _tuple8*Cyc_Toc_add_designator(struct Cyc_Toc_Env*
nv,struct Cyc_Absyn_Exp*e){Cyc_Toc_exp_to_c(nv,e);{struct _tuple8*_tmp9D5;return(
_tmp9D5=_cycalloc(sizeof(*_tmp9D5)),((_tmp9D5->f1=0,((_tmp9D5->f2=e,_tmp9D5)))));}}
static struct Cyc_Absyn_Exp*Cyc_Toc_make_struct(struct Cyc_Toc_Env*nv,struct _tuple1*
x,void*struct_typ,struct Cyc_Absyn_Stmt*s,int pointer,struct Cyc_Absyn_Exp*rgnopt,
int is_atomic);static struct Cyc_Absyn_Exp*Cyc_Toc_make_struct(struct Cyc_Toc_Env*nv,
struct _tuple1*x,void*struct_typ,struct Cyc_Absyn_Stmt*s,int pointer,struct Cyc_Absyn_Exp*
rgnopt,int is_atomic){struct Cyc_Absyn_Exp*eo;void*t;if(pointer){t=Cyc_Absyn_cstar_typ(
struct_typ,Cyc_Toc_mt_tq);{struct Cyc_Absyn_Exp*_tmp2E6=Cyc_Absyn_sizeofexp_exp(
Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(x,0),0),0);if(rgnopt == 0  || Cyc_Absyn_no_regions)
eo=(struct Cyc_Absyn_Exp*)(is_atomic?Cyc_Toc_malloc_atomic(_tmp2E6): Cyc_Toc_malloc_ptr(
_tmp2E6));else{struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)rgnopt;Cyc_Toc_exp_to_c(
nv,r);eo=(struct Cyc_Absyn_Exp*)Cyc_Toc_rmalloc_exp(r,_tmp2E6);}}}else{t=
struct_typ;eo=0;}return Cyc_Absyn_stmt_exp(Cyc_Absyn_declare_stmt(x,t,eo,s,0),0);}
static struct Cyc_Absyn_Stmt*Cyc_Toc_init_comprehension(struct Cyc_Toc_Env*nv,
struct Cyc_Absyn_Exp*lhs,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*e1,struct
Cyc_Absyn_Exp*e2,int zero_term,struct Cyc_Absyn_Stmt*s,int e1_already_translated);
static struct Cyc_Absyn_Stmt*Cyc_Toc_init_anon_struct(struct Cyc_Toc_Env*nv,struct
Cyc_Absyn_Exp*lhs,void*struct_type,struct Cyc_List_List*dles,struct Cyc_Absyn_Stmt*
s);static struct Cyc_Absyn_Stmt*Cyc_Toc_init_array(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*
lhs,struct Cyc_List_List*dles0,struct Cyc_Absyn_Stmt*s);static struct Cyc_Absyn_Stmt*
Cyc_Toc_init_array(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,struct Cyc_List_List*
dles0,struct Cyc_Absyn_Stmt*s){int count=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
dles0)- 1;{struct Cyc_List_List*_tmp2E7=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_rev)(dles0);for(0;_tmp2E7 != 0;_tmp2E7=_tmp2E7->tl){struct _tuple8
_tmp2E9;struct Cyc_List_List*_tmp2EA;struct Cyc_Absyn_Exp*_tmp2EB;struct _tuple8*
_tmp2E8=(struct _tuple8*)_tmp2E7->hd;_tmp2E9=*_tmp2E8;_tmp2EA=_tmp2E9.f1;_tmp2EB=
_tmp2E9.f2;{struct Cyc_Absyn_Exp*e_index;if(_tmp2EA == 0)e_index=Cyc_Absyn_signed_int_exp(
count --,0);else{if(_tmp2EA->tl != 0){const char*_tmp9D8;void*_tmp9D7;(_tmp9D7=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmp9D8="multiple designators in array",
_tag_dyneither(_tmp9D8,sizeof(char),30))),_tag_dyneither(_tmp9D7,sizeof(void*),0)));}{
void*_tmp2EE=(void*)_tmp2EA->hd;void*_tmp2EF=_tmp2EE;struct Cyc_Absyn_Exp*_tmp2F0;
_LL16D: if(*((int*)_tmp2EF)!= 0)goto _LL16F;_tmp2F0=((struct Cyc_Absyn_ArrayElement_struct*)
_tmp2EF)->f1;_LL16E: Cyc_Toc_exp_to_c(nv,_tmp2F0);e_index=_tmp2F0;goto _LL16C;
_LL16F: if(*((int*)_tmp2EF)!= 1)goto _LL16C;_LL170: {const char*_tmp9DB;void*
_tmp9DA;(_tmp9DA=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((
_tmp9DB="field name designators in array",_tag_dyneither(_tmp9DB,sizeof(char),32))),
_tag_dyneither(_tmp9DA,sizeof(void*),0)));}_LL16C:;}}{struct Cyc_Absyn_Exp*lval=
Cyc_Absyn_subscript_exp(lhs,e_index,0);void*_tmp2F3=(void*)_tmp2EB->r;struct Cyc_List_List*
_tmp2F4;struct Cyc_Absyn_Vardecl*_tmp2F5;struct Cyc_Absyn_Exp*_tmp2F6;struct Cyc_Absyn_Exp*
_tmp2F7;int _tmp2F8;void*_tmp2F9;struct Cyc_List_List*_tmp2FA;_LL172: if(*((int*)
_tmp2F3)!= 28)goto _LL174;_tmp2F4=((struct Cyc_Absyn_Array_e_struct*)_tmp2F3)->f1;
_LL173: s=Cyc_Toc_init_array(nv,lval,_tmp2F4,s);goto _LL171;_LL174: if(*((int*)
_tmp2F3)!= 29)goto _LL176;_tmp2F5=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp2F3)->f1;_tmp2F6=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp2F3)->f2;
_tmp2F7=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp2F3)->f3;_tmp2F8=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmp2F3)->f4;_LL175: s=Cyc_Toc_init_comprehension(
nv,lval,_tmp2F5,_tmp2F6,_tmp2F7,_tmp2F8,s,0);goto _LL171;_LL176: if(*((int*)
_tmp2F3)!= 31)goto _LL178;_tmp2F9=(void*)((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp2F3)->f1;_tmp2FA=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp2F3)->f2;_LL177:
s=Cyc_Toc_init_anon_struct(nv,lval,_tmp2F9,_tmp2FA,s);goto _LL171;_LL178:;_LL179:
Cyc_Toc_exp_to_c(nv,_tmp2EB);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(
lhs,e_index,0),_tmp2EB,0),s,0);goto _LL171;_LL171:;}}}}return s;}static struct Cyc_Absyn_Stmt*
Cyc_Toc_init_comprehension(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,struct
Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int zero_term,
struct Cyc_Absyn_Stmt*s,int e1_already_translated);static struct Cyc_Absyn_Stmt*Cyc_Toc_init_comprehension(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,struct Cyc_Absyn_Vardecl*vd,struct
Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int zero_term,struct Cyc_Absyn_Stmt*s,int
e1_already_translated){struct _tuple1*_tmp2FB=vd->name;void*_tmp2FC=Cyc_Toc_typ_to_c((
void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v);if(!e1_already_translated)
Cyc_Toc_exp_to_c(nv,e1);{struct Cyc_Toc_Env _tmp2FE;struct _RegionHandle*_tmp2FF;
struct Cyc_Toc_Env*_tmp2FD=nv;_tmp2FE=*_tmp2FD;_tmp2FF=_tmp2FE.rgn;{struct Cyc_Absyn_Local_b_struct
_tmp9DE;struct Cyc_Absyn_Local_b_struct*_tmp9DD;struct Cyc_Toc_Env*nv2=Cyc_Toc_add_varmap(
_tmp2FF,nv,_tmp2FB,Cyc_Absyn_varb_exp(_tmp2FB,(void*)((_tmp9DD=_cycalloc(sizeof(*
_tmp9DD)),((_tmp9DD[0]=((_tmp9DE.tag=3,((_tmp9DE.f1=vd,_tmp9DE)))),_tmp9DD)))),0));
struct _tuple1*max=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*ea=Cyc_Absyn_assign_exp(
Cyc_Absyn_var_exp(_tmp2FB,0),Cyc_Absyn_signed_int_exp(0,0),0);struct Cyc_Absyn_Exp*
eb=Cyc_Absyn_lt_exp(Cyc_Absyn_var_exp(_tmp2FB,0),Cyc_Absyn_var_exp(max,0),0);
struct Cyc_Absyn_Exp*ec=Cyc_Absyn_post_inc_exp(Cyc_Absyn_var_exp(_tmp2FB,0),0);
struct Cyc_Absyn_Exp*lval=Cyc_Absyn_subscript_exp(lhs,Cyc_Absyn_var_exp(_tmp2FB,0),
0);struct Cyc_Absyn_Stmt*body;{void*_tmp300=(void*)e2->r;struct Cyc_List_List*
_tmp301;struct Cyc_Absyn_Vardecl*_tmp302;struct Cyc_Absyn_Exp*_tmp303;struct Cyc_Absyn_Exp*
_tmp304;int _tmp305;void*_tmp306;struct Cyc_List_List*_tmp307;_LL17B: if(*((int*)
_tmp300)!= 28)goto _LL17D;_tmp301=((struct Cyc_Absyn_Array_e_struct*)_tmp300)->f1;
_LL17C: body=Cyc_Toc_init_array(nv2,lval,_tmp301,Cyc_Toc_skip_stmt_dl());goto
_LL17A;_LL17D: if(*((int*)_tmp300)!= 29)goto _LL17F;_tmp302=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp300)->f1;_tmp303=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp300)->f2;
_tmp304=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp300)->f3;_tmp305=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmp300)->f4;_LL17E: body=Cyc_Toc_init_comprehension(
nv2,lval,_tmp302,_tmp303,_tmp304,_tmp305,Cyc_Toc_skip_stmt_dl(),0);goto _LL17A;
_LL17F: if(*((int*)_tmp300)!= 31)goto _LL181;_tmp306=(void*)((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp300)->f1;_tmp307=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp300)->f2;_LL180:
body=Cyc_Toc_init_anon_struct(nv,lval,_tmp306,_tmp307,Cyc_Toc_skip_stmt_dl());
goto _LL17A;_LL181:;_LL182: Cyc_Toc_exp_to_c(nv2,e2);body=Cyc_Absyn_assign_stmt(
lval,e2,0);goto _LL17A;_LL17A:;}{struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_for_stmt(ea,eb,
ec,body,0);if(zero_term){struct Cyc_Absyn_Exp*ex=Cyc_Absyn_assign_exp(Cyc_Absyn_subscript_exp(
Cyc_Absyn_new_exp((void*)lhs->r,0),Cyc_Absyn_var_exp(max,0),0),Cyc_Toc_cast_it(
_tmp2FC,Cyc_Absyn_uint_exp(0,0)),0);s2=Cyc_Absyn_seq_stmt(s2,Cyc_Absyn_exp_stmt(
ex,0),0);}return Cyc_Absyn_seq_stmt(Cyc_Absyn_declare_stmt(max,Cyc_Absyn_uint_typ,(
struct Cyc_Absyn_Exp*)e1,Cyc_Absyn_declare_stmt(_tmp2FB,Cyc_Absyn_uint_typ,0,s2,0),
0),s,0);}}}}static struct Cyc_Absyn_Stmt*Cyc_Toc_init_anon_struct(struct Cyc_Toc_Env*
nv,struct Cyc_Absyn_Exp*lhs,void*struct_type,struct Cyc_List_List*dles,struct Cyc_Absyn_Stmt*
s);static struct Cyc_Absyn_Stmt*Cyc_Toc_init_anon_struct(struct Cyc_Toc_Env*nv,
struct Cyc_Absyn_Exp*lhs,void*struct_type,struct Cyc_List_List*dles,struct Cyc_Absyn_Stmt*
s){{struct Cyc_List_List*_tmp30A=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_rev)(dles);for(0;_tmp30A != 0;_tmp30A=_tmp30A->tl){struct _tuple8 _tmp30C;
struct Cyc_List_List*_tmp30D;struct Cyc_Absyn_Exp*_tmp30E;struct _tuple8*_tmp30B=(
struct _tuple8*)_tmp30A->hd;_tmp30C=*_tmp30B;_tmp30D=_tmp30C.f1;_tmp30E=_tmp30C.f2;
if(_tmp30D == 0){const char*_tmp9E1;void*_tmp9E0;(_tmp9E0=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmp9E1="empty designator list",
_tag_dyneither(_tmp9E1,sizeof(char),22))),_tag_dyneither(_tmp9E0,sizeof(void*),0)));}
if(_tmp30D->tl != 0){const char*_tmp9E4;void*_tmp9E3;(_tmp9E3=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmp9E4="too many designators in anonymous struct",
_tag_dyneither(_tmp9E4,sizeof(char),41))),_tag_dyneither(_tmp9E3,sizeof(void*),0)));}{
void*_tmp313=(void*)_tmp30D->hd;struct _dyneither_ptr*_tmp314;_LL184: if(*((int*)
_tmp313)!= 1)goto _LL186;_tmp314=((struct Cyc_Absyn_FieldName_struct*)_tmp313)->f1;
_LL185: {struct Cyc_Absyn_Exp*lval=Cyc_Absyn_aggrmember_exp(lhs,_tmp314,0);{void*
_tmp315=(void*)_tmp30E->r;struct Cyc_List_List*_tmp316;struct Cyc_Absyn_Vardecl*
_tmp317;struct Cyc_Absyn_Exp*_tmp318;struct Cyc_Absyn_Exp*_tmp319;int _tmp31A;void*
_tmp31B;struct Cyc_List_List*_tmp31C;_LL189: if(*((int*)_tmp315)!= 28)goto _LL18B;
_tmp316=((struct Cyc_Absyn_Array_e_struct*)_tmp315)->f1;_LL18A: s=Cyc_Toc_init_array(
nv,lval,_tmp316,s);goto _LL188;_LL18B: if(*((int*)_tmp315)!= 29)goto _LL18D;_tmp317=((
struct Cyc_Absyn_Comprehension_e_struct*)_tmp315)->f1;_tmp318=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp315)->f2;_tmp319=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp315)->f3;
_tmp31A=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp315)->f4;_LL18C: s=Cyc_Toc_init_comprehension(
nv,lval,_tmp317,_tmp318,_tmp319,_tmp31A,s,0);goto _LL188;_LL18D: if(*((int*)
_tmp315)!= 31)goto _LL18F;_tmp31B=(void*)((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp315)->f1;_tmp31C=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp315)->f2;_LL18E:
s=Cyc_Toc_init_anon_struct(nv,lval,_tmp31B,_tmp31C,s);goto _LL188;_LL18F:;_LL190:
Cyc_Toc_exp_to_c(nv,_tmp30E);if(Cyc_Toc_is_poly_field(struct_type,_tmp314))
_tmp30E=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp30E);s=Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(lval,_tmp30E,0),0),s,0);goto _LL188;
_LL188:;}goto _LL183;}_LL186:;_LL187: {const char*_tmp9E7;void*_tmp9E6;(_tmp9E6=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmp9E7="array designator in struct",_tag_dyneither(_tmp9E7,sizeof(char),27))),
_tag_dyneither(_tmp9E6,sizeof(void*),0)));}_LL183:;}}}return s;}static struct Cyc_Absyn_Exp*
Cyc_Toc_init_tuple(struct Cyc_Toc_Env*nv,int pointer,struct Cyc_Absyn_Exp*rgnopt,
struct Cyc_List_List*es);static struct Cyc_Absyn_Exp*Cyc_Toc_init_tuple(struct Cyc_Toc_Env*
nv,int pointer,struct Cyc_Absyn_Exp*rgnopt,struct Cyc_List_List*es){struct Cyc_Toc_Env
_tmp320;struct _RegionHandle*_tmp321;struct Cyc_Toc_Env*_tmp31F=nv;_tmp320=*
_tmp31F;_tmp321=_tmp320.rgn;{struct Cyc_List_List*_tmp322=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _tuple4*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*
x))Cyc_List_rmap)(_tmp321,Cyc_Toc_tup_to_c,es);void*_tmp323=Cyc_Toc_add_tuple_type(
_tmp322);struct _tuple1*_tmp324=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp325=
Cyc_Absyn_var_exp(_tmp324,0);struct Cyc_Absyn_Stmt*_tmp326=Cyc_Absyn_exp_stmt(
_tmp325,0);struct Cyc_Absyn_Exp*(*_tmp327)(struct Cyc_Absyn_Exp*,struct
_dyneither_ptr*,struct Cyc_Position_Segment*)=pointer?Cyc_Absyn_aggrarrow_exp: Cyc_Absyn_aggrmember_exp;
int is_atomic=1;struct Cyc_List_List*_tmp328=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(_tmp321,es);{int i=((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmp328);for(0;_tmp328 != 0;(_tmp328=
_tmp328->tl,-- i)){struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)_tmp328->hd;struct
Cyc_Absyn_Exp*lval=_tmp327(_tmp325,Cyc_Absyn_fieldname(i),0);is_atomic=is_atomic
 && Cyc_Toc_atomic_typ((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);{
void*_tmp329=(void*)e->r;struct Cyc_List_List*_tmp32A;struct Cyc_Absyn_Vardecl*
_tmp32B;struct Cyc_Absyn_Exp*_tmp32C;struct Cyc_Absyn_Exp*_tmp32D;int _tmp32E;
_LL192: if(*((int*)_tmp329)!= 28)goto _LL194;_tmp32A=((struct Cyc_Absyn_Array_e_struct*)
_tmp329)->f1;_LL193: _tmp326=Cyc_Toc_init_array(nv,lval,_tmp32A,_tmp326);goto
_LL191;_LL194: if(*((int*)_tmp329)!= 29)goto _LL196;_tmp32B=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp329)->f1;_tmp32C=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp329)->f2;
_tmp32D=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp329)->f3;_tmp32E=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmp329)->f4;_LL195: _tmp326=Cyc_Toc_init_comprehension(
nv,lval,_tmp32B,_tmp32C,_tmp32D,_tmp32E,_tmp326,0);goto _LL191;_LL196:;_LL197: Cyc_Toc_exp_to_c(
nv,e);_tmp326=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp327(
_tmp325,Cyc_Absyn_fieldname(i),0),e,0),0),_tmp326,0);goto _LL191;_LL191:;}}}
return Cyc_Toc_make_struct(nv,_tmp324,_tmp323,_tmp326,pointer,rgnopt,is_atomic);}}
static int Cyc_Toc_get_member_offset(struct Cyc_Absyn_Aggrdecl*ad,struct
_dyneither_ptr*f);static int Cyc_Toc_get_member_offset(struct Cyc_Absyn_Aggrdecl*ad,
struct _dyneither_ptr*f){int i=1;{struct Cyc_List_List*_tmp32F=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(ad->impl))->fields;for(0;_tmp32F != 0;_tmp32F=_tmp32F->tl){struct Cyc_Absyn_Aggrfield*
_tmp330=(struct Cyc_Absyn_Aggrfield*)_tmp32F->hd;if(Cyc_strcmp((struct
_dyneither_ptr)*_tmp330->name,(struct _dyneither_ptr)*f)== 0)return i;++ i;}}{
struct Cyc_String_pa_struct _tmp9EF;void*_tmp9EE[1];const char*_tmp9ED;void*_tmp9EC;(
_tmp9EC=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)((
struct _dyneither_ptr)((_tmp9EF.tag=0,((_tmp9EF.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*f),((_tmp9EE[0]=& _tmp9EF,Cyc_aprintf(((_tmp9ED="get_member_offset %s failed",
_tag_dyneither(_tmp9ED,sizeof(char),28))),_tag_dyneither(_tmp9EE,sizeof(void*),1)))))))),
_tag_dyneither(_tmp9EC,sizeof(void*),0)));}}static struct Cyc_Absyn_Exp*Cyc_Toc_init_struct(
struct Cyc_Toc_Env*nv,void*struct_type,int has_exists,int pointer,struct Cyc_Absyn_Exp*
rgnopt,struct Cyc_List_List*dles,struct _tuple1*tdn);static struct Cyc_Absyn_Exp*Cyc_Toc_init_struct(
struct Cyc_Toc_Env*nv,void*struct_type,int has_exists,int pointer,struct Cyc_Absyn_Exp*
rgnopt,struct Cyc_List_List*dles,struct _tuple1*tdn){struct _tuple1*_tmp335=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp336=Cyc_Absyn_var_exp(_tmp335,0);struct Cyc_Absyn_Stmt*
_tmp337=Cyc_Absyn_exp_stmt(_tmp336,0);struct Cyc_Absyn_Exp*(*_tmp338)(struct Cyc_Absyn_Exp*,
struct _dyneither_ptr*,struct Cyc_Position_Segment*)=pointer?Cyc_Absyn_aggrarrow_exp:
Cyc_Absyn_aggrmember_exp;void*_tmp339=Cyc_Toc_aggrdecl_type(tdn,Cyc_Absyn_strctq);
int is_atomic=1;struct Cyc_Absyn_Aggrdecl*ad;{void*_tmp33A=Cyc_Tcutil_compress(
struct_type);struct Cyc_Absyn_AggrInfo _tmp33B;union Cyc_Absyn_AggrInfoU_union
_tmp33C;_LL199: if(_tmp33A <= (void*)4)goto _LL19B;if(*((int*)_tmp33A)!= 10)goto
_LL19B;_tmp33B=((struct Cyc_Absyn_AggrType_struct*)_tmp33A)->f1;_tmp33C=_tmp33B.aggr_info;
_LL19A: ad=Cyc_Absyn_get_known_aggrdecl(_tmp33C);goto _LL198;_LL19B:;_LL19C: {
const char*_tmp9F2;void*_tmp9F1;(_tmp9F1=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmp9F2="init_struct: bad struct type",
_tag_dyneither(_tmp9F2,sizeof(char),29))),_tag_dyneither(_tmp9F1,sizeof(void*),0)));}
_LL198:;}{int is_tagged_union=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged;
struct Cyc_Toc_Env _tmp340;struct _RegionHandle*_tmp341;struct Cyc_Toc_Env*_tmp33F=
nv;_tmp340=*_tmp33F;_tmp341=_tmp340.rgn;{struct Cyc_List_List*_tmp342=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(_tmp341,dles);for(0;
_tmp342 != 0;_tmp342=_tmp342->tl){struct _tuple8 _tmp344;struct Cyc_List_List*
_tmp345;struct Cyc_Absyn_Exp*_tmp346;struct _tuple8*_tmp343=(struct _tuple8*)
_tmp342->hd;_tmp344=*_tmp343;_tmp345=_tmp344.f1;_tmp346=_tmp344.f2;is_atomic=
is_atomic  && Cyc_Toc_atomic_typ((void*)((struct Cyc_Core_Opt*)_check_null(_tmp346->topt))->v);
if(_tmp345 == 0){const char*_tmp9F5;void*_tmp9F4;(_tmp9F4=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmp9F5="empty designator list",
_tag_dyneither(_tmp9F5,sizeof(char),22))),_tag_dyneither(_tmp9F4,sizeof(void*),0)));}
if(_tmp345->tl != 0){struct _tuple1*_tmp349=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*
_tmp34A=Cyc_Absyn_var_exp(_tmp349,0);for(0;_tmp345 != 0;_tmp345=_tmp345->tl){void*
_tmp34B=(void*)_tmp345->hd;struct _dyneither_ptr*_tmp34C;_LL19E: if(*((int*)
_tmp34B)!= 1)goto _LL1A0;_tmp34C=((struct Cyc_Absyn_FieldName_struct*)_tmp34B)->f1;
_LL19F: if(Cyc_Toc_is_poly_field(struct_type,_tmp34C))_tmp34A=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),
_tmp34A);_tmp337=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(
_tmp338(_tmp336,_tmp34C,0),_tmp34A,0),0),_tmp337,0);goto _LL19D;_LL1A0:;_LL1A1: {
const char*_tmp9F8;void*_tmp9F7;(_tmp9F7=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmp9F8="array designator in struct",
_tag_dyneither(_tmp9F8,sizeof(char),27))),_tag_dyneither(_tmp9F7,sizeof(void*),0)));}
_LL19D:;}Cyc_Toc_exp_to_c(nv,_tmp346);_tmp337=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(
Cyc_Absyn_assign_exp(_tmp34A,_tmp346,0),0),_tmp337,0);}else{void*_tmp34F=(void*)
_tmp345->hd;struct _dyneither_ptr*_tmp350;_LL1A3: if(*((int*)_tmp34F)!= 1)goto
_LL1A5;_tmp350=((struct Cyc_Absyn_FieldName_struct*)_tmp34F)->f1;_LL1A4: {struct
Cyc_Absyn_Exp*lval=_tmp338(_tmp336,_tmp350,0);if(is_tagged_union){int i=Cyc_Toc_get_member_offset(
ad,_tmp350);struct Cyc_Absyn_Exp*f_tag_exp=Cyc_Absyn_signed_int_exp(i,0);struct
Cyc_Absyn_Exp*lhs=Cyc_Absyn_aggrmember_exp(lval,Cyc_Toc_tag_sp,0);struct Cyc_Absyn_Exp*
assn_exp=Cyc_Absyn_assign_exp(lhs,f_tag_exp,0);_tmp337=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(
assn_exp,0),_tmp337,0);lval=Cyc_Absyn_aggrmember_exp(lval,Cyc_Toc_val_sp,0);}{
void*_tmp351=(void*)_tmp346->r;struct Cyc_List_List*_tmp352;struct Cyc_Absyn_Vardecl*
_tmp353;struct Cyc_Absyn_Exp*_tmp354;struct Cyc_Absyn_Exp*_tmp355;int _tmp356;void*
_tmp357;struct Cyc_List_List*_tmp358;_LL1A8: if(*((int*)_tmp351)!= 28)goto _LL1AA;
_tmp352=((struct Cyc_Absyn_Array_e_struct*)_tmp351)->f1;_LL1A9: _tmp337=Cyc_Toc_init_array(
nv,lval,_tmp352,_tmp337);goto _LL1A7;_LL1AA: if(*((int*)_tmp351)!= 29)goto _LL1AC;
_tmp353=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp351)->f1;_tmp354=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmp351)->f2;_tmp355=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp351)->f3;_tmp356=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp351)->f4;
_LL1AB: _tmp337=Cyc_Toc_init_comprehension(nv,lval,_tmp353,_tmp354,_tmp355,
_tmp356,_tmp337,0);goto _LL1A7;_LL1AC: if(*((int*)_tmp351)!= 31)goto _LL1AE;_tmp357=(
void*)((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp351)->f1;_tmp358=((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp351)->f2;_LL1AD: _tmp337=Cyc_Toc_init_anon_struct(nv,lval,_tmp357,_tmp358,
_tmp337);goto _LL1A7;_LL1AE:;_LL1AF: {int was_ptr_type=Cyc_Tcutil_is_pointer_type((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp346->topt))->v);Cyc_Toc_exp_to_c(nv,
_tmp346);{struct Cyc_Absyn_Aggrfield*_tmp359=Cyc_Absyn_lookup_decl_field(ad,
_tmp350);if(Cyc_Toc_is_poly_field(struct_type,_tmp350) && !was_ptr_type)_tmp346=
Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp346);if(has_exists)_tmp346=Cyc_Toc_cast_it((
void*)((struct Cyc_Absyn_Aggrfield*)_check_null(_tmp359))->type,_tmp346);_tmp337=
Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(lval,_tmp346,0),0),
_tmp337,0);goto _LL1A7;}}_LL1A7:;}goto _LL1A2;}_LL1A5:;_LL1A6: {const char*_tmp9FB;
void*_tmp9FA;(_tmp9FA=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Toc_toc_impos)(((_tmp9FB="array designator in struct",_tag_dyneither(_tmp9FB,
sizeof(char),27))),_tag_dyneither(_tmp9FA,sizeof(void*),0)));}_LL1A2:;}}}return
Cyc_Toc_make_struct(nv,_tmp335,_tmp339,_tmp337,pointer,rgnopt,is_atomic);}}
struct _tuple10{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Exp*f2;};static struct Cyc_Absyn_Exp*
Cyc_Toc_assignop_lvalue(struct Cyc_Absyn_Exp*el,struct _tuple10*pr);static struct
Cyc_Absyn_Exp*Cyc_Toc_assignop_lvalue(struct Cyc_Absyn_Exp*el,struct _tuple10*pr){
return Cyc_Absyn_assignop_exp(el,(*pr).f1,(*pr).f2,0);}static struct Cyc_Absyn_Exp*
Cyc_Toc_address_lvalue(struct Cyc_Absyn_Exp*e1,int ignore);static struct Cyc_Absyn_Exp*
Cyc_Toc_address_lvalue(struct Cyc_Absyn_Exp*e1,int ignore){return Cyc_Absyn_address_exp(
e1,0);}static struct Cyc_Absyn_Exp*Cyc_Toc_incr_lvalue(struct Cyc_Absyn_Exp*e1,void*
incr);static struct Cyc_Absyn_Exp*Cyc_Toc_incr_lvalue(struct Cyc_Absyn_Exp*e1,void*
incr){struct Cyc_Absyn_Increment_e_struct _tmp9FE;struct Cyc_Absyn_Increment_e_struct*
_tmp9FD;return Cyc_Absyn_new_exp((void*)((_tmp9FD=_cycalloc(sizeof(*_tmp9FD)),((
_tmp9FD[0]=((_tmp9FE.tag=5,((_tmp9FE.f1=e1,((_tmp9FE.f2=(void*)incr,_tmp9FE)))))),
_tmp9FD)))),0);}static void Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*s,
struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*
f_env);static void Cyc_Toc_lvalue_assign(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*
fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env);static void
Cyc_Toc_lvalue_assign(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*
f)(struct Cyc_Absyn_Exp*,void*),void*f_env){void*_tmp35E=(void*)e1->r;struct Cyc_Absyn_Stmt*
_tmp35F;void*_tmp360;struct Cyc_Absyn_Exp*_tmp361;struct Cyc_Absyn_Exp*_tmp362;
struct _dyneither_ptr*_tmp363;int _tmp364;int _tmp365;_LL1B1: if(*((int*)_tmp35E)!= 
38)goto _LL1B3;_tmp35F=((struct Cyc_Absyn_StmtExp_e_struct*)_tmp35E)->f1;_LL1B2:
Cyc_Toc_lvalue_assign_stmt(_tmp35F,fs,f,f_env);goto _LL1B0;_LL1B3: if(*((int*)
_tmp35E)!= 15)goto _LL1B5;_tmp360=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp35E)->f1;
_tmp361=((struct Cyc_Absyn_Cast_e_struct*)_tmp35E)->f2;_LL1B4: Cyc_Toc_lvalue_assign(
_tmp361,fs,f,f_env);goto _LL1B0;_LL1B5: if(*((int*)_tmp35E)!= 23)goto _LL1B7;
_tmp362=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp35E)->f1;_tmp363=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp35E)->f2;_tmp364=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp35E)->f3;_tmp365=((
struct Cyc_Absyn_AggrMember_e_struct*)_tmp35E)->f4;_LL1B6:(void*)(e1->r=(void*)((
void*)_tmp362->r));{struct Cyc_List_List*_tmp9FF;Cyc_Toc_lvalue_assign(e1,(struct
Cyc_List_List*)((_tmp9FF=_cycalloc(sizeof(*_tmp9FF)),((_tmp9FF->hd=_tmp363,((
_tmp9FF->tl=fs,_tmp9FF)))))),f,f_env);}goto _LL1B0;_LL1B7:;_LL1B8: {struct Cyc_Absyn_Exp*
e1_copy=Cyc_Absyn_copy_exp(e1);for(0;fs != 0;fs=fs->tl){e1_copy=Cyc_Absyn_aggrmember_exp(
e1_copy,(struct _dyneither_ptr*)fs->hd,e1_copy->loc);}(void*)(e1->r=(void*)((void*)(
f(e1_copy,f_env))->r));goto _LL1B0;}_LL1B0:;}static void Cyc_Toc_lvalue_assign_stmt(
struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,
void*),void*f_env);static void Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*s,
struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*
f_env){void*_tmp367=(void*)s->r;struct Cyc_Absyn_Exp*_tmp368;struct Cyc_Absyn_Decl*
_tmp369;struct Cyc_Absyn_Stmt*_tmp36A;struct Cyc_Absyn_Stmt*_tmp36B;_LL1BA: if(
_tmp367 <= (void*)1)goto _LL1C0;if(*((int*)_tmp367)!= 0)goto _LL1BC;_tmp368=((
struct Cyc_Absyn_Exp_s_struct*)_tmp367)->f1;_LL1BB: Cyc_Toc_lvalue_assign(_tmp368,
fs,f,f_env);goto _LL1B9;_LL1BC: if(*((int*)_tmp367)!= 11)goto _LL1BE;_tmp369=((
struct Cyc_Absyn_Decl_s_struct*)_tmp367)->f1;_tmp36A=((struct Cyc_Absyn_Decl_s_struct*)
_tmp367)->f2;_LL1BD: Cyc_Toc_lvalue_assign_stmt(_tmp36A,fs,f,f_env);goto _LL1B9;
_LL1BE: if(*((int*)_tmp367)!= 1)goto _LL1C0;_tmp36B=((struct Cyc_Absyn_Seq_s_struct*)
_tmp367)->f2;_LL1BF: Cyc_Toc_lvalue_assign_stmt(_tmp36B,fs,f,f_env);goto _LL1B9;
_LL1C0:;_LL1C1: {const char*_tmpA03;void*_tmpA02[1];struct Cyc_String_pa_struct
_tmpA01;(_tmpA01.tag=0,((_tmpA01.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_stmt2string(s)),((_tmpA02[0]=& _tmpA01,Cyc_Toc_toc_impos(((_tmpA03="lvalue_assign_stmt: %s",
_tag_dyneither(_tmpA03,sizeof(char),23))),_tag_dyneither(_tmpA02,sizeof(void*),1)))))));}
_LL1B9:;}static struct Cyc_List_List*Cyc_Toc_rmap_2c(struct _RegionHandle*r2,void*(*
f)(void*,void*),void*env,struct Cyc_List_List*x);static struct Cyc_List_List*Cyc_Toc_rmap_2c(
struct _RegionHandle*r2,void*(*f)(void*,void*),void*env,struct Cyc_List_List*x){
struct Cyc_List_List*result;struct Cyc_List_List*prev;if(x == 0)return 0;{struct Cyc_List_List*
_tmpA04;result=((_tmpA04=_region_malloc(r2,sizeof(*_tmpA04)),((_tmpA04->hd=(void*)
f((void*)x->hd,env),((_tmpA04->tl=0,_tmpA04))))));}prev=result;for(x=x->tl;x != 0;
x=x->tl){{struct Cyc_List_List*_tmpA05;((struct Cyc_List_List*)_check_null(prev))->tl=((
_tmpA05=_region_malloc(r2,sizeof(*_tmpA05)),((_tmpA05->hd=(void*)f((void*)x->hd,
env),((_tmpA05->tl=0,_tmpA05))))));}prev=((struct Cyc_List_List*)_check_null(prev))->tl;}
return result;}static struct Cyc_List_List*Cyc_Toc_map_2c(void*(*f)(void*,void*),
void*env,struct Cyc_List_List*x);static struct Cyc_List_List*Cyc_Toc_map_2c(void*(*
f)(void*,void*),void*env,struct Cyc_List_List*x){return Cyc_Toc_rmap_2c(Cyc_Core_heap_region,
f,env,x);}static struct _tuple8*Cyc_Toc_make_dle(struct Cyc_Absyn_Exp*e);static
struct _tuple8*Cyc_Toc_make_dle(struct Cyc_Absyn_Exp*e){struct _tuple8*_tmpA06;
return(_tmpA06=_cycalloc(sizeof(*_tmpA06)),((_tmpA06->f1=0,((_tmpA06->f2=e,
_tmpA06)))));}static struct Cyc_Absyn_PtrInfo Cyc_Toc_get_ptr_type(void*t);static
struct Cyc_Absyn_PtrInfo Cyc_Toc_get_ptr_type(void*t){void*_tmp372=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_PtrInfo _tmp373;_LL1C3: if(_tmp372 <= (void*)4)goto _LL1C5;if(*((
int*)_tmp372)!= 4)goto _LL1C5;_tmp373=((struct Cyc_Absyn_PointerType_struct*)
_tmp372)->f1;_LL1C4: return _tmp373;_LL1C5:;_LL1C6: {const char*_tmpA09;void*
_tmpA08;(_tmpA08=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpA09="get_ptr_typ: not a pointer!",_tag_dyneither(_tmpA09,sizeof(char),28))),
_tag_dyneither(_tmpA08,sizeof(void*),0)));}_LL1C2:;}static struct Cyc_Absyn_Exp*
Cyc_Toc_generate_zero(void*t);static struct Cyc_Absyn_Exp*Cyc_Toc_generate_zero(
void*t){struct Cyc_Absyn_Exp*res;{void*_tmp376=Cyc_Tcutil_compress(t);void*
_tmp377;void*_tmp378;void*_tmp379;void*_tmp37A;void*_tmp37B;void*_tmp37C;void*
_tmp37D;void*_tmp37E;void*_tmp37F;void*_tmp380;_LL1C8: if(_tmp376 <= (void*)4)goto
_LL1D8;if(*((int*)_tmp376)!= 4)goto _LL1CA;_LL1C9: res=Cyc_Absyn_null_exp(0);goto
_LL1C7;_LL1CA: if(*((int*)_tmp376)!= 5)goto _LL1CC;_tmp377=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp376)->f1;_tmp378=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp376)->f2;if((
int)_tmp378 != 0)goto _LL1CC;_LL1CB:{union Cyc_Absyn_Cnst_union _tmpA0A;res=Cyc_Absyn_const_exp((
union Cyc_Absyn_Cnst_union)(((_tmpA0A.Char_c).tag=0,(((_tmpA0A.Char_c).f1=(void*)
_tmp377,(((_tmpA0A.Char_c).f2='\000',_tmpA0A)))))),0);}goto _LL1C7;_LL1CC: if(*((
int*)_tmp376)!= 5)goto _LL1CE;_tmp379=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp376)->f1;_tmp37A=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp376)->f2;if((
int)_tmp37A != 1)goto _LL1CE;_LL1CD:{union Cyc_Absyn_Cnst_union _tmpA0B;res=Cyc_Absyn_const_exp((
union Cyc_Absyn_Cnst_union)(((_tmpA0B.Short_c).tag=1,(((_tmpA0B.Short_c).f1=(void*)
_tmp379,(((_tmpA0B.Short_c).f2=0,_tmpA0B)))))),0);}goto _LL1C7;_LL1CE: if(*((int*)
_tmp376)!= 12)goto _LL1D0;_LL1CF: goto _LL1D1;_LL1D0: if(*((int*)_tmp376)!= 13)goto
_LL1D2;_LL1D1: _tmp37B=(void*)1;goto _LL1D3;_LL1D2: if(*((int*)_tmp376)!= 5)goto
_LL1D4;_tmp37B=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp376)->f1;_tmp37C=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp376)->f2;if((int)_tmp37C != 2)goto
_LL1D4;_LL1D3: _tmp37D=_tmp37B;goto _LL1D5;_LL1D4: if(*((int*)_tmp376)!= 5)goto
_LL1D6;_tmp37D=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp376)->f1;_tmp37E=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp376)->f2;if((int)_tmp37E != 3)goto
_LL1D6;_LL1D5:{union Cyc_Absyn_Cnst_union _tmpA0C;res=Cyc_Absyn_const_exp((union
Cyc_Absyn_Cnst_union)(((_tmpA0C.Int_c).tag=2,(((_tmpA0C.Int_c).f1=(void*)_tmp37D,(((
_tmpA0C.Int_c).f2=0,_tmpA0C)))))),0);}goto _LL1C7;_LL1D6: if(*((int*)_tmp376)!= 5)
goto _LL1D8;_tmp37F=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp376)->f1;_tmp380=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp376)->f2;if((int)_tmp380 != 4)goto
_LL1D8;_LL1D7:{union Cyc_Absyn_Cnst_union _tmpA0D;res=Cyc_Absyn_const_exp((union
Cyc_Absyn_Cnst_union)(((_tmpA0D.LongLong_c).tag=3,(((_tmpA0D.LongLong_c).f1=(
void*)_tmp37F,(((_tmpA0D.LongLong_c).f2=(long long)0,_tmpA0D)))))),0);}goto
_LL1C7;_LL1D8: if((int)_tmp376 != 1)goto _LL1DA;_LL1D9: goto _LL1DB;_LL1DA: if(_tmp376
<= (void*)4)goto _LL1DC;if(*((int*)_tmp376)!= 6)goto _LL1DC;_LL1DB:{const char*
_tmpA10;union Cyc_Absyn_Cnst_union _tmpA0F;res=Cyc_Absyn_const_exp((union Cyc_Absyn_Cnst_union)(((
_tmpA0F.Float_c).tag=4,(((_tmpA0F.Float_c).f1=((_tmpA10="0.0",_tag_dyneither(
_tmpA10,sizeof(char),4))),_tmpA0F)))),0);}goto _LL1C7;_LL1DC:;_LL1DD: {const char*
_tmpA14;void*_tmpA13[1];struct Cyc_String_pa_struct _tmpA12;(_tmpA12.tag=0,((
_tmpA12.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((
_tmpA13[0]=& _tmpA12,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpA14="found non-zero type %s in generate_zero",_tag_dyneither(_tmpA14,sizeof(
char),40))),_tag_dyneither(_tmpA13,sizeof(void*),1)))))));}_LL1C7:;}{struct Cyc_Core_Opt*
_tmpA15;res->topt=((_tmpA15=_cycalloc(sizeof(*_tmpA15)),((_tmpA15->v=(void*)t,
_tmpA15))));}return res;}static void Cyc_Toc_zero_ptr_assign_to_c(struct Cyc_Toc_Env*
nv,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*popt,struct
Cyc_Absyn_Exp*e2,void*ptr_type,int is_dyneither,void*elt_type);static void Cyc_Toc_zero_ptr_assign_to_c(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*
popt,struct Cyc_Absyn_Exp*e2,void*ptr_type,int is_dyneither,void*elt_type){void*
fat_ptr_type=Cyc_Absyn_dyneither_typ(elt_type,(void*)2,Cyc_Toc_mt_tq,Cyc_Absyn_true_conref);
void*_tmp38B=Cyc_Toc_typ_to_c(elt_type);void*_tmp38C=Cyc_Toc_typ_to_c(
fat_ptr_type);void*_tmp38D=Cyc_Absyn_cstar_typ(_tmp38B,Cyc_Toc_mt_tq);struct Cyc_Core_Opt*
_tmpA16;struct Cyc_Core_Opt*_tmp38E=(_tmpA16=_cycalloc(sizeof(*_tmpA16)),((
_tmpA16->v=(void*)_tmp38D,_tmpA16)));struct Cyc_Absyn_Exp*xinit;{void*_tmp38F=(
void*)e1->r;struct Cyc_Absyn_Exp*_tmp390;struct Cyc_Absyn_Exp*_tmp391;struct Cyc_Absyn_Exp*
_tmp392;_LL1DF: if(*((int*)_tmp38F)!= 22)goto _LL1E1;_tmp390=((struct Cyc_Absyn_Deref_e_struct*)
_tmp38F)->f1;_LL1E0: if(!is_dyneither){_tmp390=Cyc_Toc_cast_it(fat_ptr_type,
_tmp390);{struct Cyc_Core_Opt*_tmpA17;_tmp390->topt=((_tmpA17=_cycalloc(sizeof(*
_tmpA17)),((_tmpA17->v=(void*)fat_ptr_type,_tmpA17))));}}Cyc_Toc_exp_to_c(nv,
_tmp390);xinit=_tmp390;goto _LL1DE;_LL1E1: if(*((int*)_tmp38F)!= 25)goto _LL1E3;
_tmp391=((struct Cyc_Absyn_Subscript_e_struct*)_tmp38F)->f1;_tmp392=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp38F)->f2;_LL1E2: if(!is_dyneither){_tmp391=Cyc_Toc_cast_it(fat_ptr_type,
_tmp391);{struct Cyc_Core_Opt*_tmpA18;_tmp391->topt=((_tmpA18=_cycalloc(sizeof(*
_tmpA18)),((_tmpA18->v=(void*)fat_ptr_type,_tmpA18))));}}Cyc_Toc_exp_to_c(nv,
_tmp391);Cyc_Toc_exp_to_c(nv,_tmp392);{struct Cyc_Absyn_Exp*_tmpA19[3];xinit=Cyc_Absyn_fncall_exp(
Cyc_Toc__dyneither_ptr_plus_e,((_tmpA19[2]=_tmp392,((_tmpA19[1]=Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c(elt_type),0),((_tmpA19[0]=_tmp391,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpA19,sizeof(struct Cyc_Absyn_Exp*),
3)))))))),0);}goto _LL1DE;_LL1E3:;_LL1E4: {const char*_tmpA1C;void*_tmpA1B;(
_tmpA1B=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpA1C="found bad lhs for zero-terminated pointer assignment",_tag_dyneither(
_tmpA1C,sizeof(char),53))),_tag_dyneither(_tmpA1B,sizeof(void*),0)));}_LL1DE:;}{
struct _tuple1*_tmp398=Cyc_Toc_temp_var();struct Cyc_Toc_Env _tmp39A;struct
_RegionHandle*_tmp39B;struct Cyc_Toc_Env*_tmp399=nv;_tmp39A=*_tmp399;_tmp39B=
_tmp39A.rgn;{struct Cyc_Toc_Env*_tmp39C=Cyc_Toc_add_varmap(_tmp39B,nv,_tmp398,Cyc_Absyn_var_exp(
_tmp398,0));struct Cyc_Absyn_Vardecl*_tmpA1D;struct Cyc_Absyn_Vardecl*_tmp39D=(
_tmpA1D=_cycalloc(sizeof(*_tmpA1D)),((_tmpA1D->sc=(void*)((void*)2),((_tmpA1D->name=
_tmp398,((_tmpA1D->tq=Cyc_Toc_mt_tq,((_tmpA1D->type=(void*)_tmp38C,((_tmpA1D->initializer=(
struct Cyc_Absyn_Exp*)xinit,((_tmpA1D->rgn=0,((_tmpA1D->attributes=0,((_tmpA1D->escapes=
0,_tmpA1D)))))))))))))))));struct Cyc_Absyn_Local_b_struct _tmpA20;struct Cyc_Absyn_Local_b_struct*
_tmpA1F;struct Cyc_Absyn_Local_b_struct*_tmp39E=(_tmpA1F=_cycalloc(sizeof(*
_tmpA1F)),((_tmpA1F[0]=((_tmpA20.tag=3,((_tmpA20.f1=_tmp39D,_tmpA20)))),_tmpA1F)));
struct Cyc_Absyn_Exp*_tmp39F=Cyc_Absyn_varb_exp(_tmp398,(void*)_tmp39E,0);{struct
Cyc_Core_Opt*_tmpA21;_tmp39F->topt=((_tmpA21=_cycalloc(sizeof(*_tmpA21)),((
_tmpA21->v=(void*)fat_ptr_type,_tmpA21))));}{struct Cyc_Absyn_Exp*_tmp3A1=Cyc_Absyn_deref_exp(
_tmp39F,0);{struct Cyc_Core_Opt*_tmpA22;_tmp3A1->topt=((_tmpA22=_cycalloc(sizeof(*
_tmpA22)),((_tmpA22->v=(void*)elt_type,_tmpA22))));}Cyc_Toc_exp_to_c(_tmp39C,
_tmp3A1);{struct _tuple1*_tmp3A3=Cyc_Toc_temp_var();_tmp39C=Cyc_Toc_add_varmap(
_tmp39B,_tmp39C,_tmp3A3,Cyc_Absyn_var_exp(_tmp3A3,0));{struct Cyc_Absyn_Vardecl*
_tmpA23;struct Cyc_Absyn_Vardecl*_tmp3A4=(_tmpA23=_cycalloc(sizeof(*_tmpA23)),((
_tmpA23->sc=(void*)((void*)2),((_tmpA23->name=_tmp3A3,((_tmpA23->tq=Cyc_Toc_mt_tq,((
_tmpA23->type=(void*)_tmp38B,((_tmpA23->initializer=(struct Cyc_Absyn_Exp*)
_tmp3A1,((_tmpA23->rgn=0,((_tmpA23->attributes=0,((_tmpA23->escapes=0,_tmpA23)))))))))))))))));
struct Cyc_Absyn_Local_b_struct _tmpA26;struct Cyc_Absyn_Local_b_struct*_tmpA25;
struct Cyc_Absyn_Local_b_struct*_tmp3A5=(_tmpA25=_cycalloc(sizeof(*_tmpA25)),((
_tmpA25[0]=((_tmpA26.tag=3,((_tmpA26.f1=_tmp3A4,_tmpA26)))),_tmpA25)));struct Cyc_Absyn_Exp*
z_init=e2;if(popt != 0){struct Cyc_Absyn_Exp*_tmp3A6=Cyc_Absyn_varb_exp(_tmp3A3,(
void*)_tmp3A5,0);_tmp3A6->topt=_tmp3A1->topt;z_init=Cyc_Absyn_prim2_exp((void*)
popt->v,_tmp3A6,e2,0);z_init->topt=_tmp3A6->topt;}Cyc_Toc_exp_to_c(_tmp39C,
z_init);{struct _tuple1*_tmp3A7=Cyc_Toc_temp_var();struct Cyc_Absyn_Vardecl*
_tmpA27;struct Cyc_Absyn_Vardecl*_tmp3A8=(_tmpA27=_cycalloc(sizeof(*_tmpA27)),((
_tmpA27->sc=(void*)((void*)2),((_tmpA27->name=_tmp3A7,((_tmpA27->tq=Cyc_Toc_mt_tq,((
_tmpA27->type=(void*)_tmp38B,((_tmpA27->initializer=(struct Cyc_Absyn_Exp*)z_init,((
_tmpA27->rgn=0,((_tmpA27->attributes=0,((_tmpA27->escapes=0,_tmpA27)))))))))))))))));
struct Cyc_Absyn_Local_b_struct _tmpA2A;struct Cyc_Absyn_Local_b_struct*_tmpA29;
struct Cyc_Absyn_Local_b_struct*_tmp3A9=(_tmpA29=_cycalloc(sizeof(*_tmpA29)),((
_tmpA29[0]=((_tmpA2A.tag=3,((_tmpA2A.f1=_tmp3A8,_tmpA2A)))),_tmpA29)));_tmp39C=
Cyc_Toc_add_varmap(_tmp39B,_tmp39C,_tmp3A7,Cyc_Absyn_var_exp(_tmp3A7,0));{struct
Cyc_Absyn_Exp*_tmp3AA=Cyc_Absyn_varb_exp(_tmp3A3,(void*)_tmp3A5,0);_tmp3AA->topt=
_tmp3A1->topt;{struct Cyc_Absyn_Exp*_tmp3AB=Cyc_Toc_generate_zero(elt_type);
struct Cyc_Absyn_Exp*_tmp3AC=Cyc_Absyn_prim2_exp((void*)5,_tmp3AA,_tmp3AB,0);{
struct Cyc_Core_Opt*_tmpA2B;_tmp3AC->topt=((_tmpA2B=_cycalloc(sizeof(*_tmpA2B)),((
_tmpA2B->v=(void*)Cyc_Absyn_sint_typ,_tmpA2B))));}Cyc_Toc_exp_to_c(_tmp39C,
_tmp3AC);{struct Cyc_Absyn_Exp*_tmp3AE=Cyc_Absyn_varb_exp(_tmp3A7,(void*)_tmp3A9,
0);_tmp3AE->topt=_tmp3A1->topt;{struct Cyc_Absyn_Exp*_tmp3AF=Cyc_Toc_generate_zero(
elt_type);struct Cyc_Absyn_Exp*_tmp3B0=Cyc_Absyn_prim2_exp((void*)6,_tmp3AE,
_tmp3AF,0);{struct Cyc_Core_Opt*_tmpA2C;_tmp3B0->topt=((_tmpA2C=_cycalloc(sizeof(*
_tmpA2C)),((_tmpA2C->v=(void*)Cyc_Absyn_sint_typ,_tmpA2C))));}Cyc_Toc_exp_to_c(
_tmp39C,_tmp3B0);{struct Cyc_Absyn_Exp*_tmpA2D[2];struct Cyc_List_List*_tmp3B2=(
_tmpA2D[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_type),0),((_tmpA2D[0]=
Cyc_Absyn_varb_exp(_tmp398,(void*)_tmp39E,0),((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpA2D,sizeof(struct Cyc_Absyn_Exp*),
2)))));struct Cyc_Absyn_Exp*_tmp3B3=Cyc_Absyn_uint_exp(1,0);struct Cyc_Absyn_Exp*
xsize;xsize=Cyc_Absyn_prim2_exp((void*)5,Cyc_Absyn_fncall_exp(Cyc_Toc__get_dyneither_size_e,
_tmp3B2,0),_tmp3B3,0);{struct Cyc_Absyn_Exp*_tmp3B4=Cyc_Absyn_and_exp(xsize,Cyc_Absyn_and_exp(
_tmp3AC,_tmp3B0,0),0);struct Cyc_Absyn_Stmt*_tmp3B5=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(
Cyc_Toc__throw_arraybounds_e,0,0),0);struct Cyc_Absyn_Exp*_tmp3B6=Cyc_Absyn_aggrmember_exp(
Cyc_Absyn_varb_exp(_tmp398,(void*)_tmp39E,0),Cyc_Toc_curr_sp,0);_tmp3B6=Cyc_Toc_cast_it(
_tmp38D,_tmp3B6);{struct Cyc_Absyn_Exp*_tmp3B7=Cyc_Absyn_deref_exp(_tmp3B6,0);
struct Cyc_Absyn_Exp*_tmp3B8=Cyc_Absyn_assign_exp(_tmp3B7,Cyc_Absyn_var_exp(
_tmp3A7,0),0);struct Cyc_Absyn_Stmt*_tmp3B9=Cyc_Absyn_exp_stmt(_tmp3B8,0);_tmp3B9=
Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(_tmp3B4,_tmp3B5,Cyc_Absyn_skip_stmt(
0),0),_tmp3B9,0);{struct Cyc_Absyn_Var_d_struct*_tmpA33;struct Cyc_Absyn_Var_d_struct
_tmpA32;struct Cyc_Absyn_Decl*_tmpA31;_tmp3B9=Cyc_Absyn_decl_stmt(((_tmpA31=
_cycalloc(sizeof(*_tmpA31)),((_tmpA31->r=(void*)((void*)((_tmpA33=_cycalloc(
sizeof(*_tmpA33)),((_tmpA33[0]=((_tmpA32.tag=0,((_tmpA32.f1=_tmp3A8,_tmpA32)))),
_tmpA33))))),((_tmpA31->loc=0,_tmpA31)))))),_tmp3B9,0);}{struct Cyc_Absyn_Var_d_struct*
_tmpA39;struct Cyc_Absyn_Var_d_struct _tmpA38;struct Cyc_Absyn_Decl*_tmpA37;_tmp3B9=
Cyc_Absyn_decl_stmt(((_tmpA37=_cycalloc(sizeof(*_tmpA37)),((_tmpA37->r=(void*)((
void*)((_tmpA39=_cycalloc(sizeof(*_tmpA39)),((_tmpA39[0]=((_tmpA38.tag=0,((
_tmpA38.f1=_tmp3A4,_tmpA38)))),_tmpA39))))),((_tmpA37->loc=0,_tmpA37)))))),
_tmp3B9,0);}{struct Cyc_Absyn_Var_d_struct*_tmpA3F;struct Cyc_Absyn_Var_d_struct
_tmpA3E;struct Cyc_Absyn_Decl*_tmpA3D;_tmp3B9=Cyc_Absyn_decl_stmt(((_tmpA3D=
_cycalloc(sizeof(*_tmpA3D)),((_tmpA3D->r=(void*)((void*)((_tmpA3F=_cycalloc(
sizeof(*_tmpA3F)),((_tmpA3F[0]=((_tmpA3E.tag=0,((_tmpA3E.f1=_tmp39D,_tmpA3E)))),
_tmpA3F))))),((_tmpA3D->loc=0,_tmpA3D)))))),_tmp3B9,0);}(void*)(e->r=(void*)Cyc_Toc_stmt_exp_r(
_tmp3B9));}}}}}}}}}}}}}}static void*Cyc_Toc_check_tagged_union(struct Cyc_Absyn_Exp*
e1,void*e1_c_type,void*aggrtype,struct _dyneither_ptr*f,struct Cyc_Absyn_Exp*(*
aggrproj)(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,struct Cyc_Position_Segment*));
static void*Cyc_Toc_check_tagged_union(struct Cyc_Absyn_Exp*e1,void*e1_c_type,void*
aggrtype,struct _dyneither_ptr*f,struct Cyc_Absyn_Exp*(*aggrproj)(struct Cyc_Absyn_Exp*,
struct _dyneither_ptr*,struct Cyc_Position_Segment*)){struct Cyc_Absyn_Aggrdecl*ad;{
void*_tmp3CE=Cyc_Tcutil_compress(aggrtype);struct Cyc_Absyn_AggrInfo _tmp3CF;union
Cyc_Absyn_AggrInfoU_union _tmp3D0;_LL1E6: if(_tmp3CE <= (void*)4)goto _LL1E8;if(*((
int*)_tmp3CE)!= 10)goto _LL1E8;_tmp3CF=((struct Cyc_Absyn_AggrType_struct*)_tmp3CE)->f1;
_tmp3D0=_tmp3CF.aggr_info;_LL1E7: ad=Cyc_Absyn_get_known_aggrdecl(_tmp3D0);goto
_LL1E5;_LL1E8:;_LL1E9: {struct Cyc_String_pa_struct _tmpA47;void*_tmpA46[1];const
char*_tmpA45;void*_tmpA44;(_tmpA44=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)((struct _dyneither_ptr)((_tmpA47.tag=0,((
_tmpA47.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
aggrtype)),((_tmpA46[0]=& _tmpA47,Cyc_aprintf(((_tmpA45="expecting union but found %s in check_tagged_union",
_tag_dyneither(_tmpA45,sizeof(char),51))),_tag_dyneither(_tmpA46,sizeof(void*),1)))))))),
_tag_dyneither(_tmpA44,sizeof(void*),0)));}_LL1E5:;}{struct _tuple1*_tmp3D5=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp3D6=Cyc_Absyn_var_exp(_tmp3D5,0);struct Cyc_Absyn_Exp*
_tmp3D7=Cyc_Absyn_aggrmember_exp(aggrproj(_tmp3D6,f,0),Cyc_Toc_tag_sp,0);struct
Cyc_Absyn_Exp*_tmp3D8=Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(ad,f),0);
struct Cyc_Absyn_Exp*_tmp3D9=Cyc_Absyn_neq_exp(_tmp3D7,_tmp3D8,0);struct Cyc_Absyn_Exp*
_tmp3DA=Cyc_Absyn_aggrmember_exp(aggrproj(_tmp3D6,f,0),Cyc_Toc_val_sp,0);struct
Cyc_Absyn_Stmt*_tmp3DB=Cyc_Absyn_exp_stmt(_tmp3DA,0);struct Cyc_Absyn_Stmt*
_tmp3DC=Cyc_Absyn_ifthenelse_stmt(_tmp3D9,Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),
0);struct Cyc_Absyn_Stmt*_tmp3DD=Cyc_Absyn_declare_stmt(_tmp3D5,e1_c_type,(struct
Cyc_Absyn_Exp*)e1,Cyc_Absyn_seq_stmt(_tmp3DC,_tmp3DB,0),0);return Cyc_Toc_stmt_exp_r(
_tmp3DD);}}static int Cyc_Toc_is_tagged_union_project(struct Cyc_Absyn_Exp*e,int*
f_tag,void**tagged_member_type,int clear_read);static int Cyc_Toc_is_tagged_union_project(
struct Cyc_Absyn_Exp*e,int*f_tag,void**tagged_member_type,int clear_read){void*
_tmp3DE=(void*)e->r;struct Cyc_Absyn_Exp*_tmp3DF;struct Cyc_Absyn_Exp*_tmp3E0;
struct _dyneither_ptr*_tmp3E1;int _tmp3E2;int*_tmp3E3;struct Cyc_Absyn_Exp*_tmp3E4;
struct _dyneither_ptr*_tmp3E5;int _tmp3E6;int*_tmp3E7;_LL1EB: if(*((int*)_tmp3DE)!= 
15)goto _LL1ED;_tmp3DF=((struct Cyc_Absyn_Cast_e_struct*)_tmp3DE)->f2;_LL1EC: {
const char*_tmpA4A;void*_tmpA49;(_tmpA49=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpA4A="cast on lhs!",
_tag_dyneither(_tmpA4A,sizeof(char),13))),_tag_dyneither(_tmpA49,sizeof(void*),0)));}
_LL1ED: if(*((int*)_tmp3DE)!= 23)goto _LL1EF;_tmp3E0=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp3DE)->f1;_tmp3E1=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp3DE)->f2;_tmp3E2=((
struct Cyc_Absyn_AggrMember_e_struct*)_tmp3DE)->f4;_tmp3E3=(int*)&((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp3DE)->f4;_LL1EE: {void*_tmp3EA=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp3E0->topt))->v);struct Cyc_Absyn_AggrInfo _tmp3EB;union Cyc_Absyn_AggrInfoU_union
_tmp3EC;_LL1F4: if(_tmp3EA <= (void*)4)goto _LL1F6;if(*((int*)_tmp3EA)!= 10)goto
_LL1F6;_tmp3EB=((struct Cyc_Absyn_AggrType_struct*)_tmp3EA)->f1;_tmp3EC=_tmp3EB.aggr_info;
_LL1F5: {struct Cyc_Absyn_Aggrdecl*_tmp3ED=Cyc_Absyn_get_known_aggrdecl(_tmp3EC);*
f_tag=Cyc_Toc_get_member_offset(_tmp3ED,_tmp3E1);{const char*_tmpA4F;void*_tmpA4E[
2];struct Cyc_String_pa_struct _tmpA4D;struct Cyc_String_pa_struct _tmpA4C;struct
_dyneither_ptr str=(struct _dyneither_ptr)((_tmpA4C.tag=0,((_tmpA4C.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*_tmp3E1),((_tmpA4D.tag=0,((_tmpA4D.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp3ED->name)),((
_tmpA4E[0]=& _tmpA4D,((_tmpA4E[1]=& _tmpA4C,Cyc_aprintf(((_tmpA4F="_union_%s_%s",
_tag_dyneither(_tmpA4F,sizeof(char),13))),_tag_dyneither(_tmpA4E,sizeof(void*),2))))))))))))));{
struct _dyneither_ptr*_tmpA50;*tagged_member_type=Cyc_Absyn_strct(((_tmpA50=
_cycalloc(sizeof(*_tmpA50)),((_tmpA50[0]=str,_tmpA50)))));}if(clear_read)*
_tmp3E3=0;return((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp3ED->impl))->tagged;}}
_LL1F6:;_LL1F7: return 0;_LL1F3:;}_LL1EF: if(*((int*)_tmp3DE)!= 24)goto _LL1F1;
_tmp3E4=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp3DE)->f1;_tmp3E5=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp3DE)->f2;_tmp3E6=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp3DE)->f4;_tmp3E7=(
int*)&((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp3DE)->f4;_LL1F0: {void*_tmp3F3=
Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp3E4->topt))->v);
struct Cyc_Absyn_PtrInfo _tmp3F4;void*_tmp3F5;_LL1F9: if(_tmp3F3 <= (void*)4)goto
_LL1FB;if(*((int*)_tmp3F3)!= 4)goto _LL1FB;_tmp3F4=((struct Cyc_Absyn_PointerType_struct*)
_tmp3F3)->f1;_tmp3F5=(void*)_tmp3F4.elt_typ;_LL1FA: {void*_tmp3F6=Cyc_Tcutil_compress(
_tmp3F5);struct Cyc_Absyn_AggrInfo _tmp3F7;union Cyc_Absyn_AggrInfoU_union _tmp3F8;
_LL1FE: if(_tmp3F6 <= (void*)4)goto _LL200;if(*((int*)_tmp3F6)!= 10)goto _LL200;
_tmp3F7=((struct Cyc_Absyn_AggrType_struct*)_tmp3F6)->f1;_tmp3F8=_tmp3F7.aggr_info;
_LL1FF: {struct Cyc_Absyn_Aggrdecl*_tmp3F9=Cyc_Absyn_get_known_aggrdecl(_tmp3F8);*
f_tag=Cyc_Toc_get_member_offset(_tmp3F9,_tmp3E5);{const char*_tmpA55;void*_tmpA54[
2];struct Cyc_String_pa_struct _tmpA53;struct Cyc_String_pa_struct _tmpA52;struct
_dyneither_ptr str=(struct _dyneither_ptr)((_tmpA52.tag=0,((_tmpA52.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*_tmp3E5),((_tmpA53.tag=0,((_tmpA53.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp3F9->name)),((
_tmpA54[0]=& _tmpA53,((_tmpA54[1]=& _tmpA52,Cyc_aprintf(((_tmpA55="_union_%s_%s",
_tag_dyneither(_tmpA55,sizeof(char),13))),_tag_dyneither(_tmpA54,sizeof(void*),2))))))))))))));{
struct _dyneither_ptr*_tmpA56;*tagged_member_type=Cyc_Absyn_strct(((_tmpA56=
_cycalloc(sizeof(*_tmpA56)),((_tmpA56[0]=str,_tmpA56)))));}if(clear_read)*
_tmp3E7=0;return((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp3F9->impl))->tagged;}}
_LL200:;_LL201: return 0;_LL1FD:;}_LL1FB:;_LL1FC: return 0;_LL1F8:;}_LL1F1:;_LL1F2:
return 0;_LL1EA:;}static void*Cyc_Toc_tagged_union_assignop(struct Cyc_Absyn_Exp*e1,
void*e1_cyc_type,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,void*e2_cyc_type,
int tag_num,void*member_type);static void*Cyc_Toc_tagged_union_assignop(struct Cyc_Absyn_Exp*
e1,void*e1_cyc_type,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,void*
e2_cyc_type,int tag_num,void*member_type){struct _tuple1*_tmp3FF=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*temp_exp=Cyc_Absyn_var_exp(_tmp3FF,0);struct Cyc_Absyn_Exp*
temp_val=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_val_sp,0);struct Cyc_Absyn_Exp*
temp_tag=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_tag_sp,0);struct Cyc_Absyn_Exp*
f_tag=Cyc_Absyn_signed_int_exp(tag_num,0);struct Cyc_Absyn_Stmt*s3=Cyc_Absyn_exp_stmt(
Cyc_Absyn_assignop_exp(temp_val,popt,e2,0),0);struct Cyc_Absyn_Stmt*s2;if(popt == 
0)s2=Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(temp_tag,f_tag,0),0);else{struct Cyc_Absyn_Exp*
_tmp400=Cyc_Absyn_neq_exp(temp_tag,f_tag,0);s2=Cyc_Absyn_ifthenelse_stmt(_tmp400,
Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),0);}{struct Cyc_Absyn_Stmt*s1=
Cyc_Absyn_declare_stmt(_tmp3FF,Cyc_Absyn_cstar_typ(member_type,Cyc_Toc_mt_tq),(
struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(e1,0),Cyc_Absyn_seq_stmt(s2,s3,0),0);
return Cyc_Toc_stmt_exp_r(s1);}}struct _tuple11{struct _tuple1*f1;void*f2;struct Cyc_Absyn_Exp*
f3;};struct _tuple12{struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Exp*f2;};static
void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e);static void
_tmpAB5(unsigned int*_tmpAB4,unsigned int*_tmpAB3,struct _tuple1***_tmpAB1){for(*
_tmpAB4=0;*_tmpAB4 < *_tmpAB3;(*_tmpAB4)++){(*_tmpAB1)[*_tmpAB4]=Cyc_Toc_temp_var();}}
static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e){void*
_tmp401=(void*)e->r;if(e->topt == 0){const char*_tmpA5A;void*_tmpA59[1];struct Cyc_String_pa_struct
_tmpA58;(_tmpA58.tag=0,((_tmpA58.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(e)),((_tmpA59[0]=& _tmpA58,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpA5A="exp_to_c: no type for %s",
_tag_dyneither(_tmpA5A,sizeof(char),25))),_tag_dyneither(_tmpA59,sizeof(void*),1)))))));}{
void*old_typ=(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;void*_tmp405=
_tmp401;union Cyc_Absyn_Cnst_union _tmp406;struct _tuple1*_tmp407;void*_tmp408;
struct _tuple1*_tmp409;void*_tmp40A;struct Cyc_List_List*_tmp40B;struct Cyc_Absyn_Exp*
_tmp40C;void*_tmp40D;struct Cyc_Absyn_Exp*_tmp40E;struct Cyc_Core_Opt*_tmp40F;
struct Cyc_Absyn_Exp*_tmp410;struct Cyc_Absyn_Exp*_tmp411;struct Cyc_Absyn_Exp*
_tmp412;struct Cyc_Absyn_Exp*_tmp413;struct Cyc_Absyn_Exp*_tmp414;struct Cyc_Absyn_Exp*
_tmp415;struct Cyc_Absyn_Exp*_tmp416;struct Cyc_Absyn_Exp*_tmp417;struct Cyc_Absyn_Exp*
_tmp418;struct Cyc_Absyn_Exp*_tmp419;struct Cyc_Absyn_Exp*_tmp41A;struct Cyc_List_List*
_tmp41B;struct Cyc_Absyn_Exp*_tmp41C;struct Cyc_List_List*_tmp41D;struct Cyc_Absyn_VarargCallInfo*
_tmp41E;struct Cyc_Absyn_Exp*_tmp41F;struct Cyc_List_List*_tmp420;struct Cyc_Absyn_VarargCallInfo*
_tmp421;struct Cyc_Absyn_VarargCallInfo _tmp422;int _tmp423;struct Cyc_List_List*
_tmp424;struct Cyc_Absyn_VarargInfo*_tmp425;struct Cyc_Absyn_Exp*_tmp426;struct Cyc_Absyn_Exp*
_tmp427;struct Cyc_Absyn_Exp*_tmp428;struct Cyc_List_List*_tmp429;void*_tmp42A;
void**_tmp42B;struct Cyc_Absyn_Exp*_tmp42C;int _tmp42D;void*_tmp42E;struct Cyc_Absyn_Exp*
_tmp42F;struct Cyc_Absyn_Exp*_tmp430;struct Cyc_Absyn_Exp*_tmp431;struct Cyc_Absyn_Exp*
_tmp432;void*_tmp433;void*_tmp434;void*_tmp435;struct _dyneither_ptr*_tmp436;void*
_tmp437;void*_tmp438;unsigned int _tmp439;struct Cyc_Absyn_Exp*_tmp43A;struct Cyc_Absyn_Exp*
_tmp43B;struct _dyneither_ptr*_tmp43C;int _tmp43D;int _tmp43E;struct Cyc_Absyn_Exp*
_tmp43F;struct _dyneither_ptr*_tmp440;int _tmp441;int _tmp442;struct Cyc_Absyn_Exp*
_tmp443;struct Cyc_Absyn_Exp*_tmp444;struct Cyc_List_List*_tmp445;struct Cyc_List_List*
_tmp446;struct Cyc_Absyn_Vardecl*_tmp447;struct Cyc_Absyn_Exp*_tmp448;struct Cyc_Absyn_Exp*
_tmp449;int _tmp44A;struct _tuple1*_tmp44B;struct Cyc_List_List*_tmp44C;struct Cyc_List_List*
_tmp44D;struct Cyc_Absyn_Aggrdecl*_tmp44E;void*_tmp44F;struct Cyc_List_List*
_tmp450;struct Cyc_List_List*_tmp451;struct Cyc_Absyn_Datatypedecl*_tmp452;struct
Cyc_Absyn_Datatypefield*_tmp453;struct Cyc_List_List*_tmp454;struct Cyc_Absyn_Datatypedecl*
_tmp455;struct Cyc_Absyn_Datatypefield*_tmp456;struct Cyc_Absyn_MallocInfo _tmp457;
int _tmp458;struct Cyc_Absyn_Exp*_tmp459;void**_tmp45A;struct Cyc_Absyn_Exp*_tmp45B;
int _tmp45C;struct Cyc_Absyn_Exp*_tmp45D;struct Cyc_Absyn_Exp*_tmp45E;struct Cyc_Absyn_Stmt*
_tmp45F;_LL203: if(*((int*)_tmp405)!= 0)goto _LL205;_tmp406=((struct Cyc_Absyn_Const_e_struct*)
_tmp405)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)_tmp405)->f1).Null_c).tag != 
6)goto _LL205;_LL204: {struct Cyc_Absyn_Exp*_tmp460=Cyc_Absyn_uint_exp(0,0);if(Cyc_Tcutil_is_tagged_pointer_typ(
old_typ)){if(Cyc_Toc_is_toplevel(nv))(void*)(e->r=(void*)((void*)(Cyc_Toc_make_toplevel_dyn_arr(
old_typ,_tmp460,_tmp460))->r));else{struct Cyc_Absyn_Exp*_tmpA5B[3];(void*)(e->r=(
void*)Cyc_Toc_fncall_exp_r(Cyc_Toc__tag_dyneither_e,((_tmpA5B[2]=_tmp460,((
_tmpA5B[1]=_tmp460,((_tmpA5B[0]=_tmp460,((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpA5B,sizeof(struct Cyc_Absyn_Exp*),
3))))))))));}}else{(void*)(e->r=(void*)((void*)& Cyc_Toc_zero_exp));}goto _LL202;}
_LL205: if(*((int*)_tmp405)!= 0)goto _LL207;_LL206: goto _LL202;_LL207: if(*((int*)
_tmp405)!= 1)goto _LL209;_tmp407=((struct Cyc_Absyn_Var_e_struct*)_tmp405)->f1;
_tmp408=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp405)->f2;_LL208:{struct
_handler_cons _tmp462;_push_handler(& _tmp462);{int _tmp464=0;if(setjmp(_tmp462.handler))
_tmp464=1;if(!_tmp464){(void*)(e->r=(void*)((void*)(Cyc_Toc_lookup_varmap(nv,
_tmp407))->r));;_pop_handler();}else{void*_tmp463=(void*)_exn_thrown;void*
_tmp466=_tmp463;_LL25C: if(_tmp466 != Cyc_Dict_Absent)goto _LL25E;_LL25D: {const
char*_tmpA5F;void*_tmpA5E[1];struct Cyc_String_pa_struct _tmpA5D;(_tmpA5D.tag=0,((
_tmpA5D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp407)),((_tmpA5E[0]=& _tmpA5D,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpA5F="Can't find %s in exp_to_c, Var\n",
_tag_dyneither(_tmpA5F,sizeof(char),32))),_tag_dyneither(_tmpA5E,sizeof(void*),1)))))));}
_LL25E:;_LL25F:(void)_throw(_tmp466);_LL25B:;}}}goto _LL202;_LL209: if(*((int*)
_tmp405)!= 2)goto _LL20B;_tmp409=((struct Cyc_Absyn_UnknownId_e_struct*)_tmp405)->f1;
_LL20A: {const char*_tmpA62;void*_tmpA61;(_tmpA61=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpA62="unknownid",
_tag_dyneither(_tmpA62,sizeof(char),10))),_tag_dyneither(_tmpA61,sizeof(void*),0)));}
_LL20B: if(*((int*)_tmp405)!= 3)goto _LL20D;_tmp40A=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp405)->f1;_tmp40B=((struct Cyc_Absyn_Primop_e_struct*)_tmp405)->f2;_LL20C: {
struct Cyc_List_List*_tmp46C=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Exp*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_get_cyc_typ,_tmp40B);((void(*)(void(*
f)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*
x))Cyc_List_iter_c)(Cyc_Toc_exp_to_c,nv,_tmp40B);{void*_tmp46D=_tmp40A;_LL261:
if((int)_tmp46D != 19)goto _LL263;_LL262: {struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp40B))->hd;{void*_tmp46E=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(arg->topt))->v);struct Cyc_Absyn_ArrayInfo
_tmp46F;struct Cyc_Absyn_Exp*_tmp470;struct Cyc_Absyn_PtrInfo _tmp471;void*_tmp472;
struct Cyc_Absyn_PtrAtts _tmp473;struct Cyc_Absyn_Conref*_tmp474;struct Cyc_Absyn_Conref*
_tmp475;struct Cyc_Absyn_Conref*_tmp476;_LL276: if(_tmp46E <= (void*)4)goto _LL27A;
if(*((int*)_tmp46E)!= 7)goto _LL278;_tmp46F=((struct Cyc_Absyn_ArrayType_struct*)
_tmp46E)->f1;_tmp470=_tmp46F.num_elts;_LL277: if(!Cyc_Evexp_c_can_eval((struct Cyc_Absyn_Exp*)
_check_null(_tmp470))){const char*_tmpA65;void*_tmpA64;(_tmpA64=0,Cyc_Tcutil_terr(
e->loc,((_tmpA65="can't calculate numelts",_tag_dyneither(_tmpA65,sizeof(char),
24))),_tag_dyneither(_tmpA64,sizeof(void*),0)));}(void*)(e->r=(void*)((void*)
_tmp470->r));goto _LL275;_LL278: if(*((int*)_tmp46E)!= 4)goto _LL27A;_tmp471=((
struct Cyc_Absyn_PointerType_struct*)_tmp46E)->f1;_tmp472=(void*)_tmp471.elt_typ;
_tmp473=_tmp471.ptr_atts;_tmp474=_tmp473.nullable;_tmp475=_tmp473.bounds;_tmp476=
_tmp473.zero_term;_LL279:{void*_tmp479=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,
_tmp475);struct Cyc_Absyn_Exp*_tmp47A;_LL27D: if((int)_tmp479 != 0)goto _LL27F;
_LL27E:{struct Cyc_Absyn_Exp*_tmpA66[2];(void*)(e->r=(void*)Cyc_Toc_fncall_exp_r(
Cyc_Toc__get_dyneither_size_e,((_tmpA66[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
_tmp472),0),((_tmpA66[0]=(struct Cyc_Absyn_Exp*)_tmp40B->hd,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpA66,sizeof(struct Cyc_Absyn_Exp*),
2))))))));}goto _LL27C;_LL27F: if(_tmp479 <= (void*)1)goto _LL27C;if(*((int*)_tmp479)
!= 0)goto _LL27C;_tmp47A=((struct Cyc_Absyn_Upper_b_struct*)_tmp479)->f1;_LL280:
if(((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp476)){
struct Cyc_Absyn_Exp*function_e=Cyc_Toc_getFunction(& Cyc_Toc__get_zero_arr_size_functionSet,(
struct Cyc_Absyn_Exp*)_tmp40B->hd);struct Cyc_Absyn_Exp*_tmpA67[2];(void*)(e->r=(
void*)Cyc_Toc_fncall_exp_r(function_e,((_tmpA67[1]=_tmp47A,((_tmpA67[0]=(struct
Cyc_Absyn_Exp*)_tmp40B->hd,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpA67,sizeof(struct Cyc_Absyn_Exp*),2))))))));}else{if(((int(*)(
int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp474)){if(!Cyc_Evexp_c_can_eval(
_tmp47A)){const char*_tmpA6A;void*_tmpA69;(_tmpA69=0,Cyc_Tcutil_terr(e->loc,((
_tmpA6A="can't calculate numelts",_tag_dyneither(_tmpA6A,sizeof(char),24))),
_tag_dyneither(_tmpA69,sizeof(void*),0)));}(void*)(e->r=(void*)Cyc_Toc_conditional_exp_r(
arg,_tmp47A,Cyc_Absyn_uint_exp(0,0)));}else{(void*)(e->r=(void*)((void*)_tmp47A->r));
goto _LL27C;}}goto _LL27C;_LL27C:;}goto _LL275;_LL27A:;_LL27B: {const char*_tmpA6F;
void*_tmpA6E[2];struct Cyc_String_pa_struct _tmpA6D;struct Cyc_String_pa_struct
_tmpA6C;(_tmpA6C.tag=0,((_tmpA6C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(arg->topt))->v)),((
_tmpA6D.tag=0,((_tmpA6D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(arg->topt))->v)),((_tmpA6E[0]=& _tmpA6D,((
_tmpA6E[1]=& _tmpA6C,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpA6F="size primop applied to non-array %s (%s)",_tag_dyneither(_tmpA6F,
sizeof(char),41))),_tag_dyneither(_tmpA6E,sizeof(void*),2)))))))))))));}_LL275:;}
goto _LL260;}_LL263: if((int)_tmp46D != 0)goto _LL265;_LL264:{void*_tmp483=Cyc_Tcutil_compress((
void*)((struct Cyc_List_List*)_check_null(_tmp46C))->hd);struct Cyc_Absyn_PtrInfo
_tmp484;void*_tmp485;struct Cyc_Absyn_PtrAtts _tmp486;struct Cyc_Absyn_Conref*
_tmp487;struct Cyc_Absyn_Conref*_tmp488;_LL282: if(_tmp483 <= (void*)4)goto _LL284;
if(*((int*)_tmp483)!= 4)goto _LL284;_tmp484=((struct Cyc_Absyn_PointerType_struct*)
_tmp483)->f1;_tmp485=(void*)_tmp484.elt_typ;_tmp486=_tmp484.ptr_atts;_tmp487=
_tmp486.bounds;_tmp488=_tmp486.zero_term;_LL283:{void*_tmp489=Cyc_Absyn_conref_def(
Cyc_Absyn_bounds_one,_tmp487);struct Cyc_Absyn_Exp*_tmp48A;_LL287: if((int)_tmp489
!= 0)goto _LL289;_LL288: {struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct
Cyc_List_List*)_check_null(_tmp40B))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp40B->tl))->hd;{struct Cyc_Absyn_Exp*_tmpA70[
3];(void*)(e->r=(void*)Cyc_Toc_fncall_exp_r(Cyc_Toc__dyneither_ptr_plus_e,((
_tmpA70[2]=e2,((_tmpA70[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp485),0),((
_tmpA70[0]=e1,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpA70,sizeof(struct Cyc_Absyn_Exp*),3))))))))));}goto _LL286;}
_LL289: if(_tmp489 <= (void*)1)goto _LL286;if(*((int*)_tmp489)!= 0)goto _LL286;
_tmp48A=((struct Cyc_Absyn_Upper_b_struct*)_tmp489)->f1;_LL28A: if(((int(*)(int,
struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp488)){struct Cyc_Absyn_Exp*
e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp40B))->hd;struct
Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(
_tmp40B->tl))->hd;struct Cyc_Absyn_Exp*_tmpA71[3];(void*)(e->r=(void*)((void*)(
Cyc_Absyn_fncall_exp(Cyc_Toc_getFunction(& Cyc_Toc__zero_arr_plus_functionSet,e1),((
_tmpA71[2]=e2,((_tmpA71[1]=_tmp48A,((_tmpA71[0]=e1,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpA71,sizeof(struct Cyc_Absyn_Exp*),
3)))))))),0))->r));}goto _LL286;_LL286:;}goto _LL281;_LL284:;_LL285: goto _LL281;
_LL281:;}goto _LL260;_LL265: if((int)_tmp46D != 2)goto _LL267;_LL266: {void*elt_typ=(
void*)0;if(Cyc_Tcutil_is_tagged_pointer_typ_elt((void*)((struct Cyc_List_List*)
_check_null(_tmp46C))->hd,& elt_typ)){struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp40B))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp40B->tl))->hd;if(Cyc_Tcutil_is_tagged_pointer_typ((
void*)((struct Cyc_List_List*)_check_null(_tmp46C->tl))->hd)){(void*)(e1->r=(void*)
Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp((void*)e1->r,0),Cyc_Toc_curr_sp));(
void*)(e2->r=(void*)Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp((void*)e2->r,0),
Cyc_Toc_curr_sp));(void*)(e->r=(void*)((void*)(Cyc_Absyn_divide_exp(Cyc_Absyn_copy_exp(
e),Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0),0))->r));}else{struct Cyc_Absyn_Exp*
_tmpA72[3];(void*)(e->r=(void*)Cyc_Toc_fncall_exp_r(Cyc_Toc__dyneither_ptr_plus_e,((
_tmpA72[2]=Cyc_Absyn_prim1_exp((void*)2,e2,0),((_tmpA72[1]=Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c(elt_typ),0),((_tmpA72[0]=e1,((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpA72,sizeof(struct Cyc_Absyn_Exp*),
3))))))))));}}goto _LL260;}_LL267: if((int)_tmp46D != 5)goto _LL269;_LL268: goto
_LL26A;_LL269: if((int)_tmp46D != 6)goto _LL26B;_LL26A: goto _LL26C;_LL26B: if((int)
_tmp46D != 7)goto _LL26D;_LL26C: goto _LL26E;_LL26D: if((int)_tmp46D != 9)goto _LL26F;
_LL26E: goto _LL270;_LL26F: if((int)_tmp46D != 8)goto _LL271;_LL270: goto _LL272;_LL271:
if((int)_tmp46D != 10)goto _LL273;_LL272: {struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp40B))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp40B->tl))->hd;void*t1=(void*)((struct Cyc_List_List*)
_check_null(_tmp46C))->hd;void*t2=(void*)((struct Cyc_List_List*)_check_null(
_tmp46C->tl))->hd;if(Cyc_Tcutil_is_tagged_pointer_typ(t1))(void*)(e1->r=(void*)
Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp((void*)e1->r,0),Cyc_Toc_curr_sp));if(
Cyc_Tcutil_is_tagged_pointer_typ(t2))(void*)(e2->r=(void*)Cyc_Toc_aggrmember_exp_r(
Cyc_Absyn_new_exp((void*)e2->r,0),Cyc_Toc_curr_sp));goto _LL260;}_LL273:;_LL274:
goto _LL260;_LL260:;}goto _LL202;}_LL20D: if(*((int*)_tmp405)!= 5)goto _LL20F;
_tmp40C=((struct Cyc_Absyn_Increment_e_struct*)_tmp405)->f1;_tmp40D=(void*)((
struct Cyc_Absyn_Increment_e_struct*)_tmp405)->f2;_LL20E: {void*e2_cyc_typ=(void*)((
struct Cyc_Core_Opt*)_check_null(_tmp40C->topt))->v;void*ptr_type=(void*)0;void*
elt_type=(void*)0;int is_dyneither=0;const char*_tmpA73;struct _dyneither_ptr
incr_str=(_tmpA73="increment",_tag_dyneither(_tmpA73,sizeof(char),10));if(
_tmp40D == (void*)2  || _tmp40D == (void*)3){const char*_tmpA74;incr_str=((_tmpA74="decrement",
_tag_dyneither(_tmpA74,sizeof(char),10)));}if(Cyc_Tcutil_is_zero_ptr_deref(
_tmp40C,& ptr_type,& is_dyneither,& elt_type)){{const char*_tmpA78;void*_tmpA77[1];
struct Cyc_String_pa_struct _tmpA76;(_tmpA76.tag=0,((_tmpA76.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)incr_str),((_tmpA77[0]=& _tmpA76,Cyc_Tcutil_terr(
e->loc,((_tmpA78="in-place %s is not supported when dereferencing a zero-terminated pointer",
_tag_dyneither(_tmpA78,sizeof(char),74))),_tag_dyneither(_tmpA77,sizeof(void*),1)))))));}{
const char*_tmpA7B;void*_tmpA7A;(_tmpA7A=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpA7B="in-place inc/dec on zero-term",
_tag_dyneither(_tmpA7B,sizeof(char),30))),_tag_dyneither(_tmpA7A,sizeof(void*),0)));}}{
void*tunion_member_type=(void*)0;int f_tag=0;if(Cyc_Toc_is_tagged_union_project(
_tmp40C,& f_tag,& tunion_member_type,1)){struct Cyc_Absyn_Exp*_tmp494=Cyc_Absyn_signed_int_exp(
1,0);{struct Cyc_Core_Opt*_tmpA7C;_tmp494->topt=((_tmpA7C=_cycalloc(sizeof(*
_tmpA7C)),((_tmpA7C->v=(void*)Cyc_Absyn_sint_typ,_tmpA7C))));}{void*_tmp496=
_tmp40D;_LL28C: if((int)_tmp496 != 0)goto _LL28E;_LL28D:{struct Cyc_Absyn_AssignOp_e_struct
_tmpA82;struct Cyc_Core_Opt*_tmpA81;struct Cyc_Absyn_AssignOp_e_struct*_tmpA80;(
void*)(e->r=(void*)((void*)((_tmpA80=_cycalloc(sizeof(*_tmpA80)),((_tmpA80[0]=((
_tmpA82.tag=4,((_tmpA82.f1=_tmp40C,((_tmpA82.f2=((_tmpA81=_cycalloc(sizeof(*
_tmpA81)),((_tmpA81->v=(void*)((void*)0),_tmpA81)))),((_tmpA82.f3=_tmp494,
_tmpA82)))))))),_tmpA80))))));}Cyc_Toc_exp_to_c(nv,e);return;_LL28E: if((int)
_tmp496 != 2)goto _LL290;_LL28F:{struct Cyc_Absyn_AssignOp_e_struct _tmpA88;struct
Cyc_Core_Opt*_tmpA87;struct Cyc_Absyn_AssignOp_e_struct*_tmpA86;(void*)(e->r=(
void*)((void*)((_tmpA86=_cycalloc(sizeof(*_tmpA86)),((_tmpA86[0]=((_tmpA88.tag=4,((
_tmpA88.f1=_tmp40C,((_tmpA88.f2=((_tmpA87=_cycalloc(sizeof(*_tmpA87)),((_tmpA87->v=(
void*)((void*)2),_tmpA87)))),((_tmpA88.f3=_tmp494,_tmpA88)))))))),_tmpA86))))));}
Cyc_Toc_exp_to_c(nv,e);return;_LL290:;_LL291:{const char*_tmpA8C;void*_tmpA8B[1];
struct Cyc_String_pa_struct _tmpA8A;(_tmpA8A.tag=0,((_tmpA8A.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)incr_str),((_tmpA8B[0]=& _tmpA8A,Cyc_Tcutil_terr(
e->loc,((_tmpA8C="in-place post-%s is not supported on @tagged union members",
_tag_dyneither(_tmpA8C,sizeof(char),59))),_tag_dyneither(_tmpA8B,sizeof(void*),1)))))));}{
const char*_tmpA8F;void*_tmpA8E;(_tmpA8E=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpA8F="in-place inc/dec on @tagged union",
_tag_dyneither(_tmpA8F,sizeof(char),34))),_tag_dyneither(_tmpA8E,sizeof(void*),0)));}
_LL28B:;}}Cyc_Toc_exp_to_c(nv,_tmp40C);{void*elt_typ=(void*)0;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(
old_typ,& elt_typ)){struct Cyc_Absyn_Exp*fn_e;int change=1;fn_e=(_tmp40D == (void*)1
 || _tmp40D == (void*)3)?Cyc_Toc__dyneither_ptr_inplace_plus_post_e: Cyc_Toc__dyneither_ptr_inplace_plus_e;
if(_tmp40D == (void*)2  || _tmp40D == (void*)3)change=- 1;{struct Cyc_Absyn_Exp*
_tmpA90[3];(void*)(e->r=(void*)Cyc_Toc_fncall_exp_r(fn_e,((_tmpA90[2]=Cyc_Absyn_signed_int_exp(
change,0),((_tmpA90[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0),((
_tmpA90[0]=Cyc_Absyn_address_exp(_tmp40C,0),((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpA90,sizeof(struct Cyc_Absyn_Exp*),
3))))))))));}}else{if(Cyc_Tcutil_is_zero_pointer_typ_elt(old_typ,& elt_typ)){
struct Cyc_Absyn_Exp*fn_e;{void*_tmp4A3=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp40C->topt))->v);void*_tmp4A4;int _tmp4A5;_LL293: if(_tmp4A3 <= (
void*)4)goto _LL295;if(*((int*)_tmp4A3)!= 5)goto _LL295;_tmp4A4=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp4A3)->f2;_LL294:{void*_tmp4A6=_tmp4A4;_LL29E: if((int)_tmp4A6 != 0)goto _LL2A0;
_LL29F: fn_e=_tmp40D == (void*)1?Cyc_Toc__zero_arr_inplace_plus_post_char_e: Cyc_Toc__zero_arr_inplace_plus_char_e;
goto _LL29D;_LL2A0: if((int)_tmp4A6 != 1)goto _LL2A2;_LL2A1: fn_e=_tmp40D == (void*)1?
Cyc_Toc__zero_arr_inplace_plus_post_short_e: Cyc_Toc__zero_arr_inplace_plus_short_e;
goto _LL29D;_LL2A2: if((int)_tmp4A6 != 2)goto _LL2A4;_LL2A3: fn_e=_tmp40D == (void*)1?
Cyc_Toc__zero_arr_inplace_plus_post_int_e: Cyc_Toc__zero_arr_inplace_plus_int_e;
goto _LL29D;_LL2A4:;_LL2A5: {struct Cyc_Core_Impossible_struct _tmpA96;const char*
_tmpA95;struct Cyc_Core_Impossible_struct*_tmpA94;(int)_throw((void*)((_tmpA94=
_cycalloc(sizeof(*_tmpA94)),((_tmpA94[0]=((_tmpA96.tag=Cyc_Core_Impossible,((
_tmpA96.f1=((_tmpA95="impossible IntType (not char, short or int)",
_tag_dyneither(_tmpA95,sizeof(char),44))),_tmpA96)))),_tmpA94)))));}_LL29D:;}
goto _LL292;_LL295: if((int)_tmp4A3 != 1)goto _LL297;_LL296: fn_e=_tmp40D == (void*)1?
Cyc_Toc__zero_arr_inplace_plus_post_float_e: Cyc_Toc__zero_arr_inplace_plus_float_e;
goto _LL292;_LL297: if(_tmp4A3 <= (void*)4)goto _LL29B;if(*((int*)_tmp4A3)!= 6)goto
_LL299;_tmp4A5=((struct Cyc_Absyn_DoubleType_struct*)_tmp4A3)->f1;_LL298: switch(
_tmp4A5){case 1: _LL2A6: fn_e=_tmp40D == (void*)1?Cyc_Toc__zero_arr_inplace_plus_post_longdouble_e:
Cyc_Toc__zero_arr_inplace_plus_longdouble_e;break;default: _LL2A7: fn_e=_tmp40D == (
void*)1?Cyc_Toc__zero_arr_inplace_plus_post_double_e: Cyc_Toc__zero_arr_inplace_plus_double_e;}
goto _LL292;_LL299: if(*((int*)_tmp4A3)!= 4)goto _LL29B;_LL29A: fn_e=_tmp40D == (void*)
1?Cyc_Toc__zero_arr_inplace_plus_post_voidstar_e: Cyc_Toc__zero_arr_inplace_plus_voidstar_e;
goto _LL292;_LL29B:;_LL29C: {struct Cyc_Core_Impossible_struct _tmpA9C;const char*
_tmpA9B;struct Cyc_Core_Impossible_struct*_tmpA9A;(int)_throw((void*)((_tmpA9A=
_cycalloc(sizeof(*_tmpA9A)),((_tmpA9A[0]=((_tmpA9C.tag=Cyc_Core_Impossible,((
_tmpA9C.f1=((_tmpA9B="impossible expression type (not int, float, double, or pointer)",
_tag_dyneither(_tmpA9B,sizeof(char),64))),_tmpA9C)))),_tmpA9A)))));}_LL292:;}{
struct Cyc_Absyn_Exp*_tmpA9D[2];(void*)(e->r=(void*)Cyc_Toc_fncall_exp_r(fn_e,((
_tmpA9D[1]=Cyc_Absyn_signed_int_exp(1,0),((_tmpA9D[0]=_tmp40C,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpA9D,sizeof(struct Cyc_Absyn_Exp*),
2))))))));}}else{if(elt_typ == (void*)0  && !Cyc_Absyn_is_lvalue(_tmp40C)){Cyc_Toc_lvalue_assign(
_tmp40C,0,Cyc_Toc_incr_lvalue,_tmp40D);(void*)(e->r=(void*)((void*)_tmp40C->r));}}}
goto _LL202;}}}_LL20F: if(*((int*)_tmp405)!= 4)goto _LL211;_tmp40E=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmp405)->f1;_tmp40F=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp405)->f2;_tmp410=((
struct Cyc_Absyn_AssignOp_e_struct*)_tmp405)->f3;_LL210: {void*ptr_type=(void*)0;
void*elt_type=(void*)0;int is_dyneither=0;if(Cyc_Tcutil_is_zero_ptr_deref(_tmp40E,&
ptr_type,& is_dyneither,& elt_type)){Cyc_Toc_zero_ptr_assign_to_c(nv,e,_tmp40E,
_tmp40F,_tmp410,ptr_type,is_dyneither,elt_type);return;}{int e1_poly=Cyc_Toc_is_poly_project(
_tmp40E);void*e1_old_typ=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp40E->topt))->v;
void*e2_old_typ=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp410->topt))->v;int
f_tag=0;void*tagged_member_struct_type=(void*)0;int e1_tagged_union_proj=Cyc_Toc_is_tagged_union_project(
_tmp40E,& f_tag,& tagged_member_struct_type,1);Cyc_Toc_exp_to_c(nv,_tmp40E);Cyc_Toc_exp_to_c(
nv,_tmp410);if(e1_tagged_union_proj){(void*)(e->r=(void*)Cyc_Toc_tagged_union_assignop(
_tmp40E,e1_old_typ,_tmp40F,_tmp410,e2_old_typ,f_tag,tagged_member_struct_type));
return;}{int done=0;if(_tmp40F != 0){void*elt_typ=(void*)0;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(
old_typ,& elt_typ)){struct Cyc_Absyn_Exp*change;{void*_tmp4AF=(void*)_tmp40F->v;
_LL2AA: if((int)_tmp4AF != 0)goto _LL2AC;_LL2AB: change=_tmp410;goto _LL2A9;_LL2AC:
if((int)_tmp4AF != 2)goto _LL2AE;_LL2AD: change=Cyc_Absyn_prim1_exp((void*)2,
_tmp410,0);goto _LL2A9;_LL2AE:;_LL2AF: {const char*_tmpAA0;void*_tmpA9F;(_tmpA9F=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpAA0="bad t ? pointer arithmetic",_tag_dyneither(_tmpAA0,sizeof(char),27))),
_tag_dyneither(_tmpA9F,sizeof(void*),0)));}_LL2A9:;}done=1;{struct Cyc_Absyn_Exp*
_tmp4B2=Cyc_Toc__dyneither_ptr_inplace_plus_e;struct Cyc_Absyn_Exp*_tmpAA1[3];(
void*)(e->r=(void*)Cyc_Toc_fncall_exp_r(_tmp4B2,((_tmpAA1[2]=change,((_tmpAA1[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0),((_tmpAA1[0]=Cyc_Absyn_address_exp(
_tmp40E,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpAA1,sizeof(struct Cyc_Absyn_Exp*),3))))))))));}}else{if(Cyc_Tcutil_is_zero_pointer_typ_elt(
old_typ,& elt_typ)){void*_tmp4B4=(void*)_tmp40F->v;_LL2B1: if((int)_tmp4B4 != 0)
goto _LL2B3;_LL2B2: done=1;{struct Cyc_Absyn_Exp*_tmpAA2[2];(void*)(e->r=(void*)Cyc_Toc_fncall_exp_r(
Cyc_Toc_getFunction(& Cyc_Toc__zero_arr_inplace_plus_functionSet,_tmp40E),((
_tmpAA2[1]=_tmp410,((_tmpAA2[0]=_tmp40E,((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpAA2,sizeof(struct Cyc_Absyn_Exp*),
2))))))));}goto _LL2B0;_LL2B3:;_LL2B4: {const char*_tmpAA5;void*_tmpAA4;(_tmpAA4=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpAA5="bad zero-terminated pointer arithmetic",_tag_dyneither(_tmpAA5,sizeof(
char),39))),_tag_dyneither(_tmpAA4,sizeof(void*),0)));}_LL2B0:;}}}if(!done){if(
e1_poly)(void*)(_tmp410->r=(void*)Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),
Cyc_Absyn_new_exp((void*)_tmp410->r,0)));if(!Cyc_Absyn_is_lvalue(_tmp40E)){{
struct _tuple10 _tmpAA8;struct _tuple10*_tmpAA7;((void(*)(struct Cyc_Absyn_Exp*e1,
struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,struct
_tuple10*),struct _tuple10*f_env))Cyc_Toc_lvalue_assign)(_tmp40E,0,Cyc_Toc_assignop_lvalue,((
_tmpAA7=_cycalloc(sizeof(struct _tuple10)* 1),((_tmpAA7[0]=((_tmpAA8.f1=_tmp40F,((
_tmpAA8.f2=_tmp410,_tmpAA8)))),_tmpAA7)))));}(void*)(e->r=(void*)((void*)_tmp40E->r));}}
goto _LL202;}}}_LL211: if(*((int*)_tmp405)!= 6)goto _LL213;_tmp411=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp405)->f1;_tmp412=((struct Cyc_Absyn_Conditional_e_struct*)_tmp405)->f2;
_tmp413=((struct Cyc_Absyn_Conditional_e_struct*)_tmp405)->f3;_LL212: Cyc_Toc_exp_to_c(
nv,_tmp411);Cyc_Toc_exp_to_c(nv,_tmp412);Cyc_Toc_exp_to_c(nv,_tmp413);goto _LL202;
_LL213: if(*((int*)_tmp405)!= 7)goto _LL215;_tmp414=((struct Cyc_Absyn_And_e_struct*)
_tmp405)->f1;_tmp415=((struct Cyc_Absyn_And_e_struct*)_tmp405)->f2;_LL214: Cyc_Toc_exp_to_c(
nv,_tmp414);Cyc_Toc_exp_to_c(nv,_tmp415);goto _LL202;_LL215: if(*((int*)_tmp405)!= 
8)goto _LL217;_tmp416=((struct Cyc_Absyn_Or_e_struct*)_tmp405)->f1;_tmp417=((
struct Cyc_Absyn_Or_e_struct*)_tmp405)->f2;_LL216: Cyc_Toc_exp_to_c(nv,_tmp416);
Cyc_Toc_exp_to_c(nv,_tmp417);goto _LL202;_LL217: if(*((int*)_tmp405)!= 9)goto
_LL219;_tmp418=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp405)->f1;_tmp419=((struct
Cyc_Absyn_SeqExp_e_struct*)_tmp405)->f2;_LL218: Cyc_Toc_exp_to_c(nv,_tmp418);Cyc_Toc_exp_to_c(
nv,_tmp419);goto _LL202;_LL219: if(*((int*)_tmp405)!= 10)goto _LL21B;_tmp41A=((
struct Cyc_Absyn_UnknownCall_e_struct*)_tmp405)->f1;_tmp41B=((struct Cyc_Absyn_UnknownCall_e_struct*)
_tmp405)->f2;_LL21A: _tmp41C=_tmp41A;_tmp41D=_tmp41B;goto _LL21C;_LL21B: if(*((int*)
_tmp405)!= 11)goto _LL21D;_tmp41C=((struct Cyc_Absyn_FnCall_e_struct*)_tmp405)->f1;
_tmp41D=((struct Cyc_Absyn_FnCall_e_struct*)_tmp405)->f2;_tmp41E=((struct Cyc_Absyn_FnCall_e_struct*)
_tmp405)->f3;if(_tmp41E != 0)goto _LL21D;_LL21C: Cyc_Toc_exp_to_c(nv,_tmp41C);((
void(*)(void(*f)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,
struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Toc_exp_to_c,nv,_tmp41D);goto _LL202;
_LL21D: if(*((int*)_tmp405)!= 11)goto _LL21F;_tmp41F=((struct Cyc_Absyn_FnCall_e_struct*)
_tmp405)->f1;_tmp420=((struct Cyc_Absyn_FnCall_e_struct*)_tmp405)->f2;_tmp421=((
struct Cyc_Absyn_FnCall_e_struct*)_tmp405)->f3;if(_tmp421 == 0)goto _LL21F;_tmp422=*
_tmp421;_tmp423=_tmp422.num_varargs;_tmp424=_tmp422.injectors;_tmp425=_tmp422.vai;
_LL21E: {struct Cyc_Toc_Env _tmp4BB;struct _RegionHandle*_tmp4BC;struct Cyc_Toc_Env*
_tmp4BA=nv;_tmp4BB=*_tmp4BA;_tmp4BC=_tmp4BB.rgn;{struct _tuple1*argv=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*argvexp=Cyc_Absyn_var_exp(argv,0);struct Cyc_Absyn_Exp*
num_varargs_exp=Cyc_Absyn_uint_exp((unsigned int)_tmp423,0);void*cva_type=Cyc_Toc_typ_to_c((
void*)_tmp425->type);void*arr_type=Cyc_Absyn_array_typ(cva_type,Cyc_Toc_mt_tq,(
struct Cyc_Absyn_Exp*)num_varargs_exp,Cyc_Absyn_false_conref,0);int num_args=((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmp420);int num_normargs=num_args - 
_tmp423;struct Cyc_List_List*new_args=0;{int i=0;for(0;i < num_normargs;(++ i,
_tmp420=_tmp420->tl)){Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp420))->hd);{struct Cyc_List_List*_tmpAA9;new_args=((_tmpAA9=
_cycalloc(sizeof(*_tmpAA9)),((_tmpAA9->hd=(struct Cyc_Absyn_Exp*)_tmp420->hd,((
_tmpAA9->tl=new_args,_tmpAA9))))));}}}{struct Cyc_Absyn_Exp*_tmpAAC[3];struct Cyc_List_List*
_tmpAAB;new_args=((_tmpAAB=_cycalloc(sizeof(*_tmpAAB)),((_tmpAAB->hd=Cyc_Absyn_fncall_exp(
Cyc_Toc__tag_dyneither_e,((_tmpAAC[2]=num_varargs_exp,((_tmpAAC[1]=Cyc_Absyn_sizeoftyp_exp(
cva_type,0),((_tmpAAC[0]=argvexp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))
Cyc_List_list)(_tag_dyneither(_tmpAAC,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0),((
_tmpAAB->tl=new_args,_tmpAAB))))));}new_args=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(new_args);Cyc_Toc_exp_to_c(nv,_tmp41F);{struct Cyc_Absyn_Stmt*
s=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(_tmp41F,new_args,0),0);if(_tmp425->inject){
struct Cyc_Absyn_Datatypedecl*tud;{void*_tmp4C0=Cyc_Tcutil_compress((void*)
_tmp425->type);struct Cyc_Absyn_DatatypeInfo _tmp4C1;union Cyc_Absyn_DatatypeInfoU_union
_tmp4C2;struct Cyc_Absyn_Datatypedecl**_tmp4C3;struct Cyc_Absyn_Datatypedecl*
_tmp4C4;_LL2B6: if(_tmp4C0 <= (void*)4)goto _LL2B8;if(*((int*)_tmp4C0)!= 2)goto
_LL2B8;_tmp4C1=((struct Cyc_Absyn_DatatypeType_struct*)_tmp4C0)->f1;_tmp4C2=
_tmp4C1.datatype_info;if((((((struct Cyc_Absyn_DatatypeType_struct*)_tmp4C0)->f1).datatype_info).KnownDatatype).tag
!= 1)goto _LL2B8;_tmp4C3=(_tmp4C2.KnownDatatype).f1;_tmp4C4=*_tmp4C3;_LL2B7: tud=
_tmp4C4;goto _LL2B5;_LL2B8:;_LL2B9: {const char*_tmpAAF;void*_tmpAAE;(_tmpAAE=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpAAF="toc: unknown datatype in vararg with inject",_tag_dyneither(_tmpAAF,
sizeof(char),44))),_tag_dyneither(_tmpAAE,sizeof(void*),0)));}_LL2B5:;}{
unsigned int _tmpAB4;unsigned int _tmpAB3;struct _dyneither_ptr _tmpAB2;struct
_tuple1**_tmpAB1;unsigned int _tmpAB0;struct _dyneither_ptr vs=(_tmpAB0=(
unsigned int)_tmp423,((_tmpAB1=(struct _tuple1**)_region_malloc(_tmp4BC,
_check_times(sizeof(struct _tuple1*),_tmpAB0)),((_tmpAB2=_tag_dyneither(_tmpAB1,
sizeof(struct _tuple1*),_tmpAB0),((((_tmpAB3=_tmpAB0,_tmpAB5(& _tmpAB4,& _tmpAB3,&
_tmpAB1))),_tmpAB2)))))));if(_tmp423 != 0){struct Cyc_List_List*_tmp4C7=0;{int i=
_tmp423 - 1;for(0;i >= 0;-- i){struct Cyc_List_List*_tmpAB6;_tmp4C7=((_tmpAB6=
_cycalloc(sizeof(*_tmpAB6)),((_tmpAB6->hd=Cyc_Toc_make_dle(Cyc_Absyn_address_exp(
Cyc_Absyn_var_exp(*((struct _tuple1**)_check_dyneither_subscript(vs,sizeof(struct
_tuple1*),i)),0),0)),((_tmpAB6->tl=_tmp4C7,_tmpAB6))))));}}s=Cyc_Absyn_declare_stmt(
argv,arr_type,(struct Cyc_Absyn_Exp*)Cyc_Absyn_unresolvedmem_exp(0,_tmp4C7,0),s,0);{
int i=0;for(0;_tmp420 != 0;(((_tmp420=_tmp420->tl,_tmp424=_tmp424->tl)),++ i)){
struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)_tmp420->hd;void*arg_type=(void*)((
struct Cyc_Core_Opt*)_check_null(arg->topt))->v;struct _tuple1*var=*((struct
_tuple1**)_check_dyneither_subscript(vs,sizeof(struct _tuple1*),i));struct Cyc_Absyn_Exp*
varexp=Cyc_Absyn_var_exp(var,0);struct Cyc_Absyn_Datatypefield _tmp4CA;struct
_tuple1*_tmp4CB;struct Cyc_List_List*_tmp4CC;struct Cyc_Absyn_Datatypefield*
_tmp4C9=(struct Cyc_Absyn_Datatypefield*)((struct Cyc_List_List*)_check_null(
_tmp424))->hd;_tmp4CA=*_tmp4C9;_tmp4CB=_tmp4CA.name;_tmp4CC=_tmp4CA.typs;{void*
field_typ=Cyc_Toc_typ_to_c((*((struct _tuple4*)((struct Cyc_List_List*)_check_null(
_tmp4CC))->hd)).f2);Cyc_Toc_exp_to_c(nv,arg);if(Cyc_Toc_is_void_star(field_typ))
arg=Cyc_Toc_cast_it(field_typ,arg);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(
Cyc_Absyn_aggrmember_exp(varexp,Cyc_Absyn_fieldname(1),0),arg,0),s,0);s=Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt(Cyc_Absyn_aggrmember_exp(varexp,Cyc_Toc_tag_sp,0),Cyc_Toc_datatype_tag(
tud,_tmp4CB,1),0),s,0);{const char*_tmpAB7;s=Cyc_Absyn_declare_stmt(var,Cyc_Absyn_strctq(
Cyc_Toc_collapse_qvar_tag(_tmp4CB,((_tmpAB7="_struct",_tag_dyneither(_tmpAB7,
sizeof(char),8))))),0,s,0);}}}}}else{struct _tuple8*_tmpAB8[3];struct Cyc_List_List*
_tmp4CE=(_tmpAB8[2]=Cyc_Toc_make_dle(Cyc_Absyn_uint_exp(0,0)),((_tmpAB8[1]=Cyc_Toc_make_dle(
Cyc_Absyn_uint_exp(0,0)),((_tmpAB8[0]=Cyc_Toc_make_dle(Cyc_Absyn_uint_exp(0,0)),((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmpAB8,sizeof(struct _tuple8*),3)))))));s=Cyc_Absyn_declare_stmt(argv,Cyc_Absyn_void_star_typ(),(
struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(0,0),s,0);}}}else{{int i=0;for(0;_tmp420
!= 0;(_tmp420=_tmp420->tl,++ i)){Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)
_tmp420->hd);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(
argvexp,Cyc_Absyn_uint_exp((unsigned int)i,0),0),(struct Cyc_Absyn_Exp*)_tmp420->hd,
0),s,0);}}s=Cyc_Absyn_declare_stmt(argv,arr_type,0,s,0);}(void*)(e->r=(void*)Cyc_Toc_stmt_exp_r(
s));}}goto _LL202;}_LL21F: if(*((int*)_tmp405)!= 12)goto _LL221;_tmp426=((struct Cyc_Absyn_Throw_e_struct*)
_tmp405)->f1;_LL220: Cyc_Toc_exp_to_c(nv,_tmp426);(void*)(e->r=(void*)((void*)(
Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c(old_typ),Cyc_Toc_newthrow_exp(_tmp426),
0))->r));goto _LL202;_LL221: if(*((int*)_tmp405)!= 13)goto _LL223;_tmp427=((struct
Cyc_Absyn_NoInstantiate_e_struct*)_tmp405)->f1;_LL222: Cyc_Toc_exp_to_c(nv,
_tmp427);goto _LL202;_LL223: if(*((int*)_tmp405)!= 14)goto _LL225;_tmp428=((struct
Cyc_Absyn_Instantiate_e_struct*)_tmp405)->f1;_tmp429=((struct Cyc_Absyn_Instantiate_e_struct*)
_tmp405)->f2;_LL224: Cyc_Toc_exp_to_c(nv,_tmp428);for(0;_tmp429 != 0;_tmp429=
_tmp429->tl){void*k=Cyc_Tcutil_typ_kind((void*)_tmp429->hd);if(((k != (void*)6
 && k != (void*)3) && k != (void*)4) && k != (void*)5){{void*_tmp4D4=Cyc_Tcutil_compress((
void*)_tmp429->hd);_LL2BB: if(_tmp4D4 <= (void*)4)goto _LL2BF;if(*((int*)_tmp4D4)!= 
1)goto _LL2BD;_LL2BC: goto _LL2BE;_LL2BD: if(*((int*)_tmp4D4)!= 2)goto _LL2BF;_LL2BE:
continue;_LL2BF:;_LL2C0:(void*)(e->r=(void*)((void*)(Cyc_Toc_array_to_ptr_cast(
Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),_tmp428,
0))->r));goto _LL2BA;_LL2BA:;}break;}}goto _LL202;_LL225: if(*((int*)_tmp405)!= 15)
goto _LL227;_tmp42A=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp405)->f1;_tmp42B=(
void**)&((void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp405)->f1);_tmp42C=((struct
Cyc_Absyn_Cast_e_struct*)_tmp405)->f2;_tmp42D=((struct Cyc_Absyn_Cast_e_struct*)
_tmp405)->f3;_tmp42E=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp405)->f4;_LL226: {
void*old_t2=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp42C->topt))->v;void*
new_typ=*_tmp42B;*_tmp42B=Cyc_Toc_typ_to_c(new_typ);Cyc_Toc_exp_to_c(nv,_tmp42C);{
struct _tuple0 _tmpAB9;struct _tuple0 _tmp4D6=(_tmpAB9.f1=Cyc_Tcutil_compress(old_t2),((
_tmpAB9.f2=Cyc_Tcutil_compress(new_typ),_tmpAB9)));void*_tmp4D7;struct Cyc_Absyn_PtrInfo
_tmp4D8;void*_tmp4D9;struct Cyc_Absyn_PtrInfo _tmp4DA;void*_tmp4DB;struct Cyc_Absyn_PtrInfo
_tmp4DC;void*_tmp4DD;_LL2C2: _tmp4D7=_tmp4D6.f1;if(_tmp4D7 <= (void*)4)goto _LL2C4;
if(*((int*)_tmp4D7)!= 4)goto _LL2C4;_tmp4D8=((struct Cyc_Absyn_PointerType_struct*)
_tmp4D7)->f1;_tmp4D9=_tmp4D6.f2;if(_tmp4D9 <= (void*)4)goto _LL2C4;if(*((int*)
_tmp4D9)!= 4)goto _LL2C4;_tmp4DA=((struct Cyc_Absyn_PointerType_struct*)_tmp4D9)->f1;
_LL2C3: {int _tmp4DE=((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,(_tmp4D8.ptr_atts).nullable);int _tmp4DF=((int(*)(int,struct Cyc_Absyn_Conref*x))
Cyc_Absyn_conref_def)(0,(_tmp4DA.ptr_atts).nullable);void*_tmp4E0=Cyc_Absyn_conref_def(
Cyc_Absyn_bounds_one,(_tmp4D8.ptr_atts).bounds);void*_tmp4E1=Cyc_Absyn_conref_def(
Cyc_Absyn_bounds_one,(_tmp4DA.ptr_atts).bounds);int _tmp4E2=((int(*)(int,struct
Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,(_tmp4D8.ptr_atts).zero_term);int
_tmp4E3=((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,(_tmp4DA.ptr_atts).zero_term);{
struct _tuple0 _tmpABA;struct _tuple0 _tmp4E5=(_tmpABA.f1=_tmp4E0,((_tmpABA.f2=
_tmp4E1,_tmpABA)));void*_tmp4E6;struct Cyc_Absyn_Exp*_tmp4E7;void*_tmp4E8;struct
Cyc_Absyn_Exp*_tmp4E9;void*_tmp4EA;struct Cyc_Absyn_Exp*_tmp4EB;void*_tmp4EC;void*
_tmp4ED;void*_tmp4EE;struct Cyc_Absyn_Exp*_tmp4EF;void*_tmp4F0;void*_tmp4F1;
_LL2C9: _tmp4E6=_tmp4E5.f1;if(_tmp4E6 <= (void*)1)goto _LL2CB;if(*((int*)_tmp4E6)!= 
0)goto _LL2CB;_tmp4E7=((struct Cyc_Absyn_Upper_b_struct*)_tmp4E6)->f1;_tmp4E8=
_tmp4E5.f2;if(_tmp4E8 <= (void*)1)goto _LL2CB;if(*((int*)_tmp4E8)!= 0)goto _LL2CB;
_tmp4E9=((struct Cyc_Absyn_Upper_b_struct*)_tmp4E8)->f1;_LL2CA: if((!Cyc_Evexp_c_can_eval(
_tmp4E7) || !Cyc_Evexp_c_can_eval(_tmp4E9)) && !Cyc_Evexp_same_const_exp(_tmp4E7,
_tmp4E9)){const char*_tmpABD;void*_tmpABC;(_tmpABC=0,Cyc_Tcutil_terr(e->loc,((
_tmpABD="can't validate cast due to potential size differences",_tag_dyneither(
_tmpABD,sizeof(char),54))),_tag_dyneither(_tmpABC,sizeof(void*),0)));}if(_tmp4DE
 && !_tmp4DF){if(Cyc_Toc_is_toplevel(nv)){const char*_tmpAC0;void*_tmpABF;(
_tmpABF=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((
_tmpAC0="can't do NULL-check conversion at top-level",_tag_dyneither(_tmpAC0,
sizeof(char),44))),_tag_dyneither(_tmpABF,sizeof(void*),0)));}if(_tmp42E != (void*)
2){const char*_tmpAC4;void*_tmpAC3[1];struct Cyc_String_pa_struct _tmpAC2;(_tmpAC2.tag=
0,((_tmpAC2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(
e)),((_tmpAC3[0]=& _tmpAC2,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap))Cyc_Tcutil_impos)(((_tmpAC4="null-check conversion mis-classified: %s",
_tag_dyneither(_tmpAC4,sizeof(char),41))),_tag_dyneither(_tmpAC3,sizeof(void*),1)))))));}{
int do_null_check=Cyc_Toc_need_null_check(_tmp42C);if(do_null_check){if(!_tmp42D){
const char*_tmpAC7;void*_tmpAC6;(_tmpAC6=0,Cyc_Tcutil_warn(e->loc,((_tmpAC7="inserted null check due to implicit cast from * to @ type",
_tag_dyneither(_tmpAC7,sizeof(char),58))),_tag_dyneither(_tmpAC6,sizeof(void*),0)));}{
struct Cyc_List_List*_tmpAC8;(void*)(e->r=(void*)Cyc_Toc_cast_it_r(*_tmp42B,Cyc_Absyn_fncall_exp(
Cyc_Toc__check_null_e,((_tmpAC8=_cycalloc(sizeof(*_tmpAC8)),((_tmpAC8->hd=
_tmp42C,((_tmpAC8->tl=0,_tmpAC8)))))),0)));}}}}goto _LL2C8;_LL2CB: _tmp4EA=_tmp4E5.f1;
if(_tmp4EA <= (void*)1)goto _LL2CD;if(*((int*)_tmp4EA)!= 0)goto _LL2CD;_tmp4EB=((
struct Cyc_Absyn_Upper_b_struct*)_tmp4EA)->f1;_tmp4EC=_tmp4E5.f2;if((int)_tmp4EC
!= 0)goto _LL2CD;_LL2CC: if(!Cyc_Evexp_c_can_eval(_tmp4EB)){const char*_tmpACB;void*
_tmpACA;(_tmpACA=0,Cyc_Tcutil_terr(e->loc,((_tmpACB="cannot perform coercion since numelts cannot be determined statically.",
_tag_dyneither(_tmpACB,sizeof(char),71))),_tag_dyneither(_tmpACA,sizeof(void*),0)));}
if(_tmp42E == (void*)2){const char*_tmpACF;void*_tmpACE[1];struct Cyc_String_pa_struct
_tmpACD;(_tmpACD.tag=0,((_tmpACD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(e)),((_tmpACE[0]=& _tmpACD,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpACF="conversion mis-classified as null-check: %s",
_tag_dyneither(_tmpACF,sizeof(char),44))),_tag_dyneither(_tmpACE,sizeof(void*),1)))))));}
if(Cyc_Toc_is_toplevel(nv)){if((_tmp4E2  && !(_tmp4DA.elt_tq).real_const) && !
_tmp4E3)_tmp4EB=Cyc_Absyn_prim2_exp((void*)2,_tmp4EB,Cyc_Absyn_uint_exp(1,0),0);(
void*)(e->r=(void*)((void*)(Cyc_Toc_make_toplevel_dyn_arr(old_t2,_tmp4EB,_tmp42C))->r));}
else{struct Cyc_Absyn_Exp*_tmp501=Cyc_Toc__tag_dyneither_e;if(_tmp4E2){struct
_tuple1*_tmp502=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp503=Cyc_Absyn_var_exp(
_tmp502,0);struct Cyc_Absyn_Exp*arg3;{void*_tmp504=(void*)_tmp42C->r;union Cyc_Absyn_Cnst_union
_tmp505;_LL2D2: if(*((int*)_tmp504)!= 0)goto _LL2D4;_tmp505=((struct Cyc_Absyn_Const_e_struct*)
_tmp504)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)_tmp504)->f1).String_c).tag
!= 5)goto _LL2D4;_LL2D3: arg3=_tmp4EB;goto _LL2D1;_LL2D4:;_LL2D5:{struct Cyc_Absyn_Exp*
_tmpAD0[2];arg3=Cyc_Absyn_fncall_exp(Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__get_zero_arr_size_functionSet,
_tmp42C),((_tmpAD0[1]=_tmp4EB,((_tmpAD0[0]=_tmp503,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpAD0,sizeof(struct Cyc_Absyn_Exp*),
2)))))),0);}goto _LL2D1;_LL2D1:;}if(!_tmp4E3  && !(_tmp4DA.elt_tq).real_const)arg3=
Cyc_Absyn_prim2_exp((void*)2,arg3,Cyc_Absyn_uint_exp(1,0),0);{struct Cyc_Absyn_Exp*
_tmp507=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c((void*)_tmp4DA.elt_typ),0);
struct Cyc_Absyn_Exp*_tmpAD1[3];struct Cyc_Absyn_Exp*_tmp508=Cyc_Absyn_fncall_exp(
_tmp501,((_tmpAD1[2]=arg3,((_tmpAD1[1]=_tmp507,((_tmpAD1[0]=_tmp503,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpAD1,sizeof(struct Cyc_Absyn_Exp*),
3)))))))),0);struct Cyc_Absyn_Stmt*_tmp509=Cyc_Absyn_exp_stmt(_tmp508,0);_tmp509=
Cyc_Absyn_declare_stmt(_tmp502,Cyc_Toc_typ_to_c(old_t2),(struct Cyc_Absyn_Exp*)
_tmp42C,_tmp509,0);(void*)(e->r=(void*)Cyc_Toc_stmt_exp_r(_tmp509));}}else{
struct Cyc_Absyn_Exp*_tmpAD2[3];(void*)(e->r=(void*)Cyc_Toc_fncall_exp_r(_tmp501,((
_tmpAD2[2]=_tmp4EB,((_tmpAD2[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c((void*)
_tmp4DA.elt_typ),0),((_tmpAD2[0]=_tmp42C,((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpAD2,sizeof(struct Cyc_Absyn_Exp*),
3))))))))));}}goto _LL2C8;_LL2CD: _tmp4ED=_tmp4E5.f1;if((int)_tmp4ED != 0)goto
_LL2CF;_tmp4EE=_tmp4E5.f2;if(_tmp4EE <= (void*)1)goto _LL2CF;if(*((int*)_tmp4EE)!= 
0)goto _LL2CF;_tmp4EF=((struct Cyc_Absyn_Upper_b_struct*)_tmp4EE)->f1;_LL2CE: if(!
Cyc_Evexp_c_can_eval(_tmp4EF)){const char*_tmpAD5;void*_tmpAD4;(_tmpAD4=0,Cyc_Tcutil_terr(
e->loc,((_tmpAD5="cannot perform coercion since numelts cannot be determined statically.",
_tag_dyneither(_tmpAD5,sizeof(char),71))),_tag_dyneither(_tmpAD4,sizeof(void*),0)));}
if(Cyc_Toc_is_toplevel(nv)){const char*_tmpAD8;void*_tmpAD7;(_tmpAD7=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpAD8="can't coerce t? to t* or t@ at the top-level",
_tag_dyneither(_tmpAD8,sizeof(char),45))),_tag_dyneither(_tmpAD7,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*_tmp510=_tmp4EF;if(_tmp4E2  && !_tmp4E3)_tmp510=Cyc_Absyn_add_exp(
_tmp4EF,Cyc_Absyn_uint_exp(1,0),0);{struct Cyc_Absyn_Exp*_tmp511=Cyc_Toc__untag_dyneither_ptr_e;
struct Cyc_Absyn_Exp*_tmpAD9[3];struct Cyc_Absyn_Exp*_tmp512=Cyc_Absyn_fncall_exp(
_tmp511,((_tmpAD9[2]=_tmp510,((_tmpAD9[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c((
void*)_tmp4D8.elt_typ),0),((_tmpAD9[0]=_tmp42C,((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpAD9,sizeof(struct Cyc_Absyn_Exp*),
3)))))))),0);if(_tmp4DF){struct Cyc_List_List*_tmpADA;(void*)(_tmp512->r=(void*)
Cyc_Toc_fncall_exp_r(Cyc_Toc__check_null_e,((_tmpADA=_cycalloc(sizeof(*_tmpADA)),((
_tmpADA->hd=Cyc_Absyn_copy_exp(_tmp512),((_tmpADA->tl=0,_tmpADA))))))));}(void*)(
e->r=(void*)Cyc_Toc_cast_it_r(*_tmp42B,_tmp512));goto _LL2C8;}}_LL2CF: _tmp4F0=
_tmp4E5.f1;if((int)_tmp4F0 != 0)goto _LL2C8;_tmp4F1=_tmp4E5.f2;if((int)_tmp4F1 != 0)
goto _LL2C8;_LL2D0: DynCast: if((_tmp4E2  && !_tmp4E3) && !(_tmp4DA.elt_tq).real_const){
if(Cyc_Toc_is_toplevel(nv)){const char*_tmpADD;void*_tmpADC;(_tmpADC=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpADD="can't coerce a ZEROTERM to a non-const NOZEROTERM pointer at toplevel",
_tag_dyneither(_tmpADD,sizeof(char),70))),_tag_dyneither(_tmpADC,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*_tmp517=Cyc_Toc__dyneither_ptr_decrease_size_e;struct Cyc_Absyn_Exp*
_tmpADE[3];(void*)(e->r=(void*)Cyc_Toc_fncall_exp_r(_tmp517,((_tmpADE[2]=Cyc_Absyn_uint_exp(
1,0),((_tmpADE[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c((void*)_tmp4D8.elt_typ),
0),((_tmpADE[0]=_tmp42C,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpADE,sizeof(struct Cyc_Absyn_Exp*),3))))))))));}}goto _LL2C8;
_LL2C8:;}goto _LL2C1;}_LL2C4: _tmp4DB=_tmp4D6.f1;if(_tmp4DB <= (void*)4)goto _LL2C6;
if(*((int*)_tmp4DB)!= 4)goto _LL2C6;_tmp4DC=((struct Cyc_Absyn_PointerType_struct*)
_tmp4DB)->f1;_tmp4DD=_tmp4D6.f2;if(_tmp4DD <= (void*)4)goto _LL2C6;if(*((int*)
_tmp4DD)!= 5)goto _LL2C6;_LL2C5:{void*_tmp519=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,(
_tmp4DC.ptr_atts).bounds);_LL2D7: if((int)_tmp519 != 0)goto _LL2D9;_LL2D8:(void*)(
_tmp42C->r=(void*)Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp((void*)_tmp42C->r,
_tmp42C->loc),Cyc_Toc_curr_sp));goto _LL2D6;_LL2D9:;_LL2DA: goto _LL2D6;_LL2D6:;}
goto _LL2C1;_LL2C6:;_LL2C7: goto _LL2C1;_LL2C1:;}goto _LL202;}_LL227: if(*((int*)
_tmp405)!= 16)goto _LL229;_tmp42F=((struct Cyc_Absyn_Address_e_struct*)_tmp405)->f1;
_LL228:{void*_tmp51A=(void*)_tmp42F->r;struct _tuple1*_tmp51B;struct Cyc_List_List*
_tmp51C;struct Cyc_List_List*_tmp51D;struct Cyc_List_List*_tmp51E;_LL2DC: if(*((int*)
_tmp51A)!= 30)goto _LL2DE;_tmp51B=((struct Cyc_Absyn_Aggregate_e_struct*)_tmp51A)->f1;
_tmp51C=((struct Cyc_Absyn_Aggregate_e_struct*)_tmp51A)->f2;_tmp51D=((struct Cyc_Absyn_Aggregate_e_struct*)
_tmp51A)->f3;_LL2DD: if(Cyc_Toc_is_toplevel(nv)){const char*_tmpAE2;void*_tmpAE1[1];
struct Cyc_String_pa_struct _tmpAE0;(_tmpAE0.tag=0,((_tmpAE0.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Position_string_of_segment(_tmp42F->loc)),((
_tmpAE1[0]=& _tmpAE0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((
_tmpAE2="%s: & on non-identifiers at the top-level",_tag_dyneither(_tmpAE2,
sizeof(char),42))),_tag_dyneither(_tmpAE1,sizeof(void*),1)))))));}(void*)(e->r=(
void*)((void*)(Cyc_Toc_init_struct(nv,(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp42F->topt))->v,_tmp51C != 0,1,0,_tmp51D,_tmp51B))->r));goto _LL2DB;_LL2DE: if(*((
int*)_tmp51A)!= 26)goto _LL2E0;_tmp51E=((struct Cyc_Absyn_Tuple_e_struct*)_tmp51A)->f1;
_LL2DF: if(Cyc_Toc_is_toplevel(nv)){const char*_tmpAE6;void*_tmpAE5[1];struct Cyc_String_pa_struct
_tmpAE4;(_tmpAE4.tag=0,((_tmpAE4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Position_string_of_segment(_tmp42F->loc)),((_tmpAE5[0]=& _tmpAE4,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpAE6="%s: & on non-identifiers at the top-level",
_tag_dyneither(_tmpAE6,sizeof(char),42))),_tag_dyneither(_tmpAE5,sizeof(void*),1)))))));}(
void*)(e->r=(void*)((void*)(Cyc_Toc_init_tuple(nv,1,0,_tmp51E))->r));goto _LL2DB;
_LL2E0:;_LL2E1: Cyc_Toc_exp_to_c(nv,_tmp42F);if(!Cyc_Absyn_is_lvalue(_tmp42F)){((
void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(
struct Cyc_Absyn_Exp*,int),int f_env))Cyc_Toc_lvalue_assign)(_tmp42F,0,Cyc_Toc_address_lvalue,
1);(void*)(e->r=(void*)Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v),_tmp42F));}goto _LL2DB;_LL2DB:;}goto _LL202;_LL229: if(*((
int*)_tmp405)!= 17)goto _LL22B;_tmp430=((struct Cyc_Absyn_New_e_struct*)_tmp405)->f1;
_tmp431=((struct Cyc_Absyn_New_e_struct*)_tmp405)->f2;_LL22A: if(Cyc_Toc_is_toplevel(
nv)){const char*_tmpAEA;void*_tmpAE9[1];struct Cyc_String_pa_struct _tmpAE8;(
_tmpAE8.tag=0,((_tmpAE8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Position_string_of_segment(
_tmp431->loc)),((_tmpAE9[0]=& _tmpAE8,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpAEA="%s: new at top-level",_tag_dyneither(
_tmpAEA,sizeof(char),21))),_tag_dyneither(_tmpAE9,sizeof(void*),1)))))));}{void*
_tmp528=(void*)_tmp431->r;struct Cyc_List_List*_tmp529;struct Cyc_Absyn_Vardecl*
_tmp52A;struct Cyc_Absyn_Exp*_tmp52B;struct Cyc_Absyn_Exp*_tmp52C;int _tmp52D;
struct _tuple1*_tmp52E;struct Cyc_List_List*_tmp52F;struct Cyc_List_List*_tmp530;
struct Cyc_Absyn_Aggrdecl*_tmp531;struct Cyc_List_List*_tmp532;_LL2E3: if(*((int*)
_tmp528)!= 28)goto _LL2E5;_tmp529=((struct Cyc_Absyn_Array_e_struct*)_tmp528)->f1;
_LL2E4: {struct _tuple1*_tmp533=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp534=
Cyc_Absyn_var_exp(_tmp533,0);struct Cyc_Absyn_Stmt*_tmp535=Cyc_Toc_init_array(nv,
_tmp534,_tmp529,Cyc_Absyn_exp_stmt(_tmp534,0));void*old_elt_typ;{void*_tmp536=
Cyc_Tcutil_compress(old_typ);struct Cyc_Absyn_PtrInfo _tmp537;void*_tmp538;struct
Cyc_Absyn_Tqual _tmp539;struct Cyc_Absyn_PtrAtts _tmp53A;struct Cyc_Absyn_Conref*
_tmp53B;_LL2EE: if(_tmp536 <= (void*)4)goto _LL2F0;if(*((int*)_tmp536)!= 4)goto
_LL2F0;_tmp537=((struct Cyc_Absyn_PointerType_struct*)_tmp536)->f1;_tmp538=(void*)
_tmp537.elt_typ;_tmp539=_tmp537.elt_tq;_tmp53A=_tmp537.ptr_atts;_tmp53B=_tmp53A.zero_term;
_LL2EF: old_elt_typ=_tmp538;goto _LL2ED;_LL2F0:;_LL2F1: {const char*_tmpAED;void*
_tmpAEC;old_elt_typ=((_tmpAEC=0,Cyc_Toc_toc_impos(((_tmpAED="exp_to_c:new array expression doesn't have ptr type",
_tag_dyneither(_tmpAED,sizeof(char),52))),_tag_dyneither(_tmpAEC,sizeof(void*),0))));}
_LL2ED:;}{void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);void*_tmp53E=Cyc_Absyn_cstar_typ(
elt_typ,Cyc_Toc_mt_tq);struct Cyc_Absyn_Exp*_tmp53F=Cyc_Absyn_times_exp(Cyc_Absyn_sizeoftyp_exp(
elt_typ,0),Cyc_Absyn_signed_int_exp(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp529),0),0);struct Cyc_Absyn_Exp*e1;if(_tmp430 == 0  || Cyc_Absyn_no_regions)e1=
Cyc_Toc_malloc_exp(old_elt_typ,_tmp53F);else{struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)
_tmp430;Cyc_Toc_exp_to_c(nv,r);e1=Cyc_Toc_rmalloc_exp(r,_tmp53F);}(void*)(e->r=(
void*)Cyc_Toc_stmt_exp_r(Cyc_Absyn_declare_stmt(_tmp533,_tmp53E,(struct Cyc_Absyn_Exp*)
e1,_tmp535,0)));goto _LL2E2;}}_LL2E5: if(*((int*)_tmp528)!= 29)goto _LL2E7;_tmp52A=((
struct Cyc_Absyn_Comprehension_e_struct*)_tmp528)->f1;_tmp52B=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp528)->f2;_tmp52C=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp528)->f3;
_tmp52D=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp528)->f4;_LL2E6: {int
is_dyneither_ptr=0;{void*_tmp540=Cyc_Tcutil_compress(old_typ);struct Cyc_Absyn_PtrInfo
_tmp541;void*_tmp542;struct Cyc_Absyn_Tqual _tmp543;struct Cyc_Absyn_PtrAtts _tmp544;
struct Cyc_Absyn_Conref*_tmp545;struct Cyc_Absyn_Conref*_tmp546;_LL2F3: if(_tmp540
<= (void*)4)goto _LL2F5;if(*((int*)_tmp540)!= 4)goto _LL2F5;_tmp541=((struct Cyc_Absyn_PointerType_struct*)
_tmp540)->f1;_tmp542=(void*)_tmp541.elt_typ;_tmp543=_tmp541.elt_tq;_tmp544=
_tmp541.ptr_atts;_tmp545=_tmp544.bounds;_tmp546=_tmp544.zero_term;_LL2F4:
is_dyneither_ptr=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp545)== (void*)0;
goto _LL2F2;_LL2F5:;_LL2F6: {const char*_tmpAF0;void*_tmpAEF;(_tmpAEF=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAF0="exp_to_c: comprehension not an array type",
_tag_dyneither(_tmpAF0,sizeof(char),42))),_tag_dyneither(_tmpAEF,sizeof(void*),0)));}
_LL2F2:;}{struct _tuple1*max=Cyc_Toc_temp_var();struct _tuple1*a=Cyc_Toc_temp_var();
void*old_elt_typ=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp52C->topt))->v;
void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);void*ptr_typ=Cyc_Absyn_cstar_typ(
elt_typ,Cyc_Toc_mt_tq);Cyc_Toc_exp_to_c(nv,_tmp52B);{struct Cyc_Absyn_Exp*_tmp549=
Cyc_Absyn_var_exp(max,0);if(_tmp52D)_tmp549=Cyc_Absyn_add_exp(_tmp549,Cyc_Absyn_uint_exp(
1,0),0);{struct Cyc_Absyn_Stmt*s=Cyc_Toc_init_comprehension(nv,Cyc_Absyn_var_exp(
a,0),_tmp52A,Cyc_Absyn_var_exp(max,0),_tmp52C,_tmp52D,Cyc_Toc_skip_stmt_dl(),1);
struct Cyc_Toc_Env _tmp54B;struct _RegionHandle*_tmp54C;struct Cyc_Toc_Env*_tmp54A=
nv;_tmp54B=*_tmp54A;_tmp54C=_tmp54B.rgn;{struct _tuple11*_tmpAF3;struct Cyc_List_List*
_tmpAF2;struct Cyc_List_List*decls=(_tmpAF2=_region_malloc(_tmp54C,sizeof(*
_tmpAF2)),((_tmpAF2->hd=((_tmpAF3=_region_malloc(_tmp54C,sizeof(*_tmpAF3)),((
_tmpAF3->f1=max,((_tmpAF3->f2=Cyc_Absyn_uint_typ,((_tmpAF3->f3=(struct Cyc_Absyn_Exp*)
_tmp52B,_tmpAF3)))))))),((_tmpAF2->tl=0,_tmpAF2)))));struct Cyc_Absyn_Exp*ai;if(
_tmp430 == 0  || Cyc_Absyn_no_regions){struct Cyc_Absyn_Exp*_tmpAF4[2];ai=Cyc_Toc_malloc_exp(
old_elt_typ,Cyc_Absyn_fncall_exp(Cyc_Toc__check_times_e,((_tmpAF4[1]=_tmp549,((
_tmpAF4[0]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0),((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpAF4,sizeof(struct Cyc_Absyn_Exp*),
2)))))),0));}else{struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)_tmp430;Cyc_Toc_exp_to_c(
nv,r);{struct Cyc_Absyn_Exp*_tmpAF5[2];ai=Cyc_Toc_rmalloc_exp(r,Cyc_Absyn_fncall_exp(
Cyc_Toc__check_times_e,((_tmpAF5[1]=_tmp549,((_tmpAF5[0]=Cyc_Absyn_sizeoftyp_exp(
elt_typ,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpAF5,sizeof(struct Cyc_Absyn_Exp*),2)))))),0));}}{struct Cyc_Absyn_Exp*
ainit=Cyc_Toc_cast_it(ptr_typ,ai);{struct _tuple11*_tmpAF8;struct Cyc_List_List*
_tmpAF7;decls=((_tmpAF7=_region_malloc(_tmp54C,sizeof(*_tmpAF7)),((_tmpAF7->hd=((
_tmpAF8=_region_malloc(_tmp54C,sizeof(*_tmpAF8)),((_tmpAF8->f1=a,((_tmpAF8->f2=
ptr_typ,((_tmpAF8->f3=(struct Cyc_Absyn_Exp*)ainit,_tmpAF8)))))))),((_tmpAF7->tl=
decls,_tmpAF7))))));}if(is_dyneither_ptr){struct _tuple1*_tmp551=Cyc_Toc_temp_var();
void*_tmp552=Cyc_Toc_typ_to_c(old_typ);struct Cyc_Absyn_Exp*_tmp553=Cyc_Toc__tag_dyneither_e;
struct Cyc_Absyn_Exp*_tmpAF9[3];struct Cyc_Absyn_Exp*_tmp554=Cyc_Absyn_fncall_exp(
_tmp553,((_tmpAF9[2]=_tmp549,((_tmpAF9[1]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0),((
_tmpAF9[0]=Cyc_Absyn_var_exp(a,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))
Cyc_List_list)(_tag_dyneither(_tmpAF9,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);{
struct _tuple11*_tmpAFC;struct Cyc_List_List*_tmpAFB;decls=((_tmpAFB=
_region_malloc(_tmp54C,sizeof(*_tmpAFB)),((_tmpAFB->hd=((_tmpAFC=_region_malloc(
_tmp54C,sizeof(*_tmpAFC)),((_tmpAFC->f1=_tmp551,((_tmpAFC->f2=_tmp552,((_tmpAFC->f3=(
struct Cyc_Absyn_Exp*)_tmp554,_tmpAFC)))))))),((_tmpAFB->tl=decls,_tmpAFB))))));}
s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(Cyc_Absyn_var_exp(_tmp551,0),0),0);}
else{s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(Cyc_Absyn_var_exp(a,0),0),0);}{
struct Cyc_List_List*_tmp558=decls;for(0;_tmp558 != 0;_tmp558=_tmp558->tl){struct
_tuple1*_tmp55A;void*_tmp55B;struct Cyc_Absyn_Exp*_tmp55C;struct _tuple11 _tmp559=*((
struct _tuple11*)_tmp558->hd);_tmp55A=_tmp559.f1;_tmp55B=_tmp559.f2;_tmp55C=
_tmp559.f3;s=Cyc_Absyn_declare_stmt(_tmp55A,_tmp55B,_tmp55C,s,0);}}(void*)(e->r=(
void*)Cyc_Toc_stmt_exp_r(s));}}goto _LL2E2;}}}}_LL2E7: if(*((int*)_tmp528)!= 30)
goto _LL2E9;_tmp52E=((struct Cyc_Absyn_Aggregate_e_struct*)_tmp528)->f1;_tmp52F=((
struct Cyc_Absyn_Aggregate_e_struct*)_tmp528)->f2;_tmp530=((struct Cyc_Absyn_Aggregate_e_struct*)
_tmp528)->f3;_tmp531=((struct Cyc_Absyn_Aggregate_e_struct*)_tmp528)->f4;_LL2E8:(
void*)(e->r=(void*)((void*)(Cyc_Toc_init_struct(nv,(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp431->topt))->v,_tmp52F != 0,1,_tmp430,_tmp530,_tmp52E))->r));goto
_LL2E2;_LL2E9: if(*((int*)_tmp528)!= 26)goto _LL2EB;_tmp532=((struct Cyc_Absyn_Tuple_e_struct*)
_tmp528)->f1;_LL2EA:(void*)(e->r=(void*)((void*)(Cyc_Toc_init_tuple(nv,1,_tmp430,
_tmp532))->r));goto _LL2E2;_LL2EB:;_LL2EC: {void*old_elt_typ=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp431->topt))->v;void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);struct
_tuple1*_tmp55F=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp560=Cyc_Absyn_var_exp(
_tmp55F,0);struct Cyc_Absyn_Exp*mexp=Cyc_Absyn_sizeofexp_exp(Cyc_Absyn_deref_exp(
_tmp560,0),0);struct Cyc_Absyn_Exp*inner_mexp=mexp;if(_tmp430 == 0  || Cyc_Absyn_no_regions)
mexp=Cyc_Toc_malloc_exp(old_elt_typ,mexp);else{struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)
_tmp430;Cyc_Toc_exp_to_c(nv,r);mexp=Cyc_Toc_rmalloc_exp(r,mexp);}{int done=0;{
void*_tmp561=(void*)_tmp431->r;void*_tmp562;struct Cyc_Absyn_Exp*_tmp563;_LL2F8:
if(*((int*)_tmp561)!= 15)goto _LL2FA;_tmp562=(void*)((struct Cyc_Absyn_Cast_e_struct*)
_tmp561)->f1;_tmp563=((struct Cyc_Absyn_Cast_e_struct*)_tmp561)->f2;_LL2F9:{
struct _tuple0 _tmpAFD;struct _tuple0 _tmp565=(_tmpAFD.f1=Cyc_Tcutil_compress(
_tmp562),((_tmpAFD.f2=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp563->topt))->v),_tmpAFD)));void*_tmp566;struct Cyc_Absyn_PtrInfo
_tmp567;void*_tmp568;struct Cyc_Absyn_PtrAtts _tmp569;struct Cyc_Absyn_Conref*
_tmp56A;void*_tmp56B;struct Cyc_Absyn_PtrInfo _tmp56C;struct Cyc_Absyn_PtrAtts
_tmp56D;struct Cyc_Absyn_Conref*_tmp56E;_LL2FD: _tmp566=_tmp565.f1;if(_tmp566 <= (
void*)4)goto _LL2FF;if(*((int*)_tmp566)!= 4)goto _LL2FF;_tmp567=((struct Cyc_Absyn_PointerType_struct*)
_tmp566)->f1;_tmp568=(void*)_tmp567.elt_typ;_tmp569=_tmp567.ptr_atts;_tmp56A=
_tmp569.bounds;_tmp56B=_tmp565.f2;if(_tmp56B <= (void*)4)goto _LL2FF;if(*((int*)
_tmp56B)!= 4)goto _LL2FF;_tmp56C=((struct Cyc_Absyn_PointerType_struct*)_tmp56B)->f1;
_tmp56D=_tmp56C.ptr_atts;_tmp56E=_tmp56D.bounds;_LL2FE:{struct _tuple0 _tmpAFE;
struct _tuple0 _tmp570=(_tmpAFE.f1=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,
_tmp56A),((_tmpAFE.f2=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp56E),_tmpAFE)));
void*_tmp571;void*_tmp572;struct Cyc_Absyn_Exp*_tmp573;_LL302: _tmp571=_tmp570.f1;
if((int)_tmp571 != 0)goto _LL304;_tmp572=_tmp570.f2;if(_tmp572 <= (void*)1)goto
_LL304;if(*((int*)_tmp572)!= 0)goto _LL304;_tmp573=((struct Cyc_Absyn_Upper_b_struct*)
_tmp572)->f1;_LL303: Cyc_Toc_exp_to_c(nv,_tmp563);(void*)(inner_mexp->r=(void*)
Cyc_Toc_sizeoftyp_exp_r(elt_typ));done=1;{struct Cyc_Absyn_Exp*_tmp574=Cyc_Toc__init_dyneither_ptr_e;{
struct Cyc_Absyn_Exp*_tmpAFF[4];(void*)(e->r=(void*)Cyc_Toc_fncall_exp_r(_tmp574,((
_tmpAFF[3]=_tmp573,((_tmpAFF[2]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp568),
0),((_tmpAFF[1]=_tmp563,((_tmpAFF[0]=mexp,((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpAFF,sizeof(struct Cyc_Absyn_Exp*),
4))))))))))));}goto _LL301;}_LL304:;_LL305: goto _LL301;_LL301:;}goto _LL2FC;_LL2FF:;
_LL300: goto _LL2FC;_LL2FC:;}goto _LL2F7;_LL2FA:;_LL2FB: goto _LL2F7;_LL2F7:;}if(!
done){struct Cyc_Absyn_Stmt*_tmp576=Cyc_Absyn_exp_stmt(_tmp560,0);struct Cyc_Absyn_Exp*
_tmp577=Cyc_Absyn_signed_int_exp(0,0);Cyc_Toc_exp_to_c(nv,_tmp431);_tmp576=Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(_tmp560,_tmp577,0),_tmp431,0),
_tmp576,0);{void*_tmp578=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);(void*)(e->r=(
void*)Cyc_Toc_stmt_exp_r(Cyc_Absyn_declare_stmt(_tmp55F,_tmp578,(struct Cyc_Absyn_Exp*)
mexp,_tmp576,0)));}}goto _LL2E2;}}_LL2E2:;}goto _LL202;_LL22B: if(*((int*)_tmp405)
!= 19)goto _LL22D;_tmp432=((struct Cyc_Absyn_Sizeofexp_e_struct*)_tmp405)->f1;
_LL22C: Cyc_Toc_exp_to_c(nv,_tmp432);goto _LL202;_LL22D: if(*((int*)_tmp405)!= 18)
goto _LL22F;_tmp433=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp405)->f1;
_LL22E:{struct Cyc_Absyn_Sizeoftyp_e_struct _tmpB02;struct Cyc_Absyn_Sizeoftyp_e_struct*
_tmpB01;(void*)(e->r=(void*)((void*)((_tmpB01=_cycalloc(sizeof(*_tmpB01)),((
_tmpB01[0]=((_tmpB02.tag=18,((_tmpB02.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp433),
_tmpB02)))),_tmpB01))))));}goto _LL202;_LL22F: if(*((int*)_tmp405)!= 21)goto _LL231;
_LL230: {const char*_tmpB05;void*_tmpB04;(_tmpB04=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB05="__gen() in code generator",
_tag_dyneither(_tmpB05,sizeof(char),26))),_tag_dyneither(_tmpB04,sizeof(void*),0)));}
_LL231: if(*((int*)_tmp405)!= 20)goto _LL233;_tmp434=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)
_tmp405)->f1;_tmp435=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp405)->f2;
if(*((int*)_tmp435)!= 0)goto _LL233;_tmp436=((struct Cyc_Absyn_StructField_struct*)
_tmp435)->f1;_LL232:{struct Cyc_Absyn_Offsetof_e_struct _tmpB0F;struct Cyc_Absyn_StructField_struct
_tmpB0E;struct Cyc_Absyn_StructField_struct*_tmpB0D;struct Cyc_Absyn_Offsetof_e_struct*
_tmpB0C;(void*)(e->r=(void*)((void*)((_tmpB0C=_cycalloc(sizeof(*_tmpB0C)),((
_tmpB0C[0]=((_tmpB0F.tag=20,((_tmpB0F.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp434),((
_tmpB0F.f2=(void*)((void*)((_tmpB0D=_cycalloc(sizeof(*_tmpB0D)),((_tmpB0D[0]=((
_tmpB0E.tag=0,((_tmpB0E.f1=_tmp436,_tmpB0E)))),_tmpB0D))))),_tmpB0F)))))),
_tmpB0C))))));}goto _LL202;_LL233: if(*((int*)_tmp405)!= 20)goto _LL235;_tmp437=(
void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp405)->f1;_tmp438=(void*)((struct
Cyc_Absyn_Offsetof_e_struct*)_tmp405)->f2;if(*((int*)_tmp438)!= 1)goto _LL235;
_tmp439=((struct Cyc_Absyn_TupleIndex_struct*)_tmp438)->f1;_LL234:{void*_tmp581=
Cyc_Tcutil_compress(_tmp437);struct Cyc_Absyn_AggrInfo _tmp582;union Cyc_Absyn_AggrInfoU_union
_tmp583;struct Cyc_List_List*_tmp584;_LL307: if(_tmp581 <= (void*)4)goto _LL30F;if(*((
int*)_tmp581)!= 10)goto _LL309;_tmp582=((struct Cyc_Absyn_AggrType_struct*)_tmp581)->f1;
_tmp583=_tmp582.aggr_info;_LL308: {struct Cyc_Absyn_Aggrdecl*_tmp585=Cyc_Absyn_get_known_aggrdecl(
_tmp583);if(_tmp585->impl == 0){const char*_tmpB12;void*_tmpB11;(_tmpB11=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB12="struct fields must be known",
_tag_dyneither(_tmpB12,sizeof(char),28))),_tag_dyneither(_tmpB11,sizeof(void*),0)));}
_tmp584=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp585->impl))->fields;goto
_LL30A;}_LL309: if(*((int*)_tmp581)!= 11)goto _LL30B;_tmp584=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp581)->f2;_LL30A: {struct Cyc_Absyn_Aggrfield*_tmp588=((struct Cyc_Absyn_Aggrfield*(*)(
struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp584,(int)_tmp439);{struct Cyc_Absyn_Offsetof_e_struct
_tmpB1C;struct Cyc_Absyn_StructField_struct _tmpB1B;struct Cyc_Absyn_StructField_struct*
_tmpB1A;struct Cyc_Absyn_Offsetof_e_struct*_tmpB19;(void*)(e->r=(void*)((void*)((
_tmpB19=_cycalloc(sizeof(*_tmpB19)),((_tmpB19[0]=((_tmpB1C.tag=20,((_tmpB1C.f1=(
void*)Cyc_Toc_typ_to_c_array(_tmp437),((_tmpB1C.f2=(void*)((void*)((_tmpB1A=
_cycalloc(sizeof(*_tmpB1A)),((_tmpB1A[0]=((_tmpB1B.tag=0,((_tmpB1B.f1=_tmp588->name,
_tmpB1B)))),_tmpB1A))))),_tmpB1C)))))),_tmpB19))))));}goto _LL306;}_LL30B: if(*((
int*)_tmp581)!= 9)goto _LL30D;_LL30C:{struct Cyc_Absyn_Offsetof_e_struct _tmpB26;
struct Cyc_Absyn_StructField_struct _tmpB25;struct Cyc_Absyn_StructField_struct*
_tmpB24;struct Cyc_Absyn_Offsetof_e_struct*_tmpB23;(void*)(e->r=(void*)((void*)((
_tmpB23=_cycalloc(sizeof(*_tmpB23)),((_tmpB23[0]=((_tmpB26.tag=20,((_tmpB26.f1=(
void*)Cyc_Toc_typ_to_c_array(_tmp437),((_tmpB26.f2=(void*)((void*)((_tmpB24=
_cycalloc(sizeof(*_tmpB24)),((_tmpB24[0]=((_tmpB25.tag=0,((_tmpB25.f1=Cyc_Absyn_fieldname((
int)(_tmp439 + 1)),_tmpB25)))),_tmpB24))))),_tmpB26)))))),_tmpB23))))));}goto
_LL306;_LL30D: if(*((int*)_tmp581)!= 3)goto _LL30F;_LL30E: if(_tmp439 == 0){struct
Cyc_Absyn_Offsetof_e_struct _tmpB30;struct Cyc_Absyn_StructField_struct _tmpB2F;
struct Cyc_Absyn_StructField_struct*_tmpB2E;struct Cyc_Absyn_Offsetof_e_struct*
_tmpB2D;(void*)(e->r=(void*)((void*)((_tmpB2D=_cycalloc(sizeof(*_tmpB2D)),((
_tmpB2D[0]=((_tmpB30.tag=20,((_tmpB30.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp437),((
_tmpB30.f2=(void*)((void*)((_tmpB2E=_cycalloc(sizeof(*_tmpB2E)),((_tmpB2E[0]=((
_tmpB2F.tag=0,((_tmpB2F.f1=Cyc_Toc_tag_sp,_tmpB2F)))),_tmpB2E))))),_tmpB30)))))),
_tmpB2D))))));}else{struct Cyc_Absyn_Offsetof_e_struct _tmpB3A;struct Cyc_Absyn_StructField_struct
_tmpB39;struct Cyc_Absyn_StructField_struct*_tmpB38;struct Cyc_Absyn_Offsetof_e_struct*
_tmpB37;(void*)(e->r=(void*)((void*)((_tmpB37=_cycalloc(sizeof(*_tmpB37)),((
_tmpB37[0]=((_tmpB3A.tag=20,((_tmpB3A.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp437),((
_tmpB3A.f2=(void*)((void*)((_tmpB38=_cycalloc(sizeof(*_tmpB38)),((_tmpB38[0]=((
_tmpB39.tag=0,((_tmpB39.f1=Cyc_Absyn_fieldname((int)_tmp439),_tmpB39)))),_tmpB38))))),
_tmpB3A)))))),_tmpB37))))));}goto _LL306;_LL30F:;_LL310: {const char*_tmpB3D;void*
_tmpB3C;(_tmpB3C=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpB3D="impossible type for offsetof tuple index",_tag_dyneither(_tmpB3D,
sizeof(char),41))),_tag_dyneither(_tmpB3C,sizeof(void*),0)));}_LL306:;}goto
_LL202;_LL235: if(*((int*)_tmp405)!= 22)goto _LL237;_tmp43A=((struct Cyc_Absyn_Deref_e_struct*)
_tmp405)->f1;_LL236: {void*_tmp59B=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp43A->topt))->v);{void*_tmp59C=_tmp59B;struct Cyc_Absyn_PtrInfo
_tmp59D;void*_tmp59E;struct Cyc_Absyn_Tqual _tmp59F;struct Cyc_Absyn_PtrAtts _tmp5A0;
void*_tmp5A1;struct Cyc_Absyn_Conref*_tmp5A2;struct Cyc_Absyn_Conref*_tmp5A3;
struct Cyc_Absyn_Conref*_tmp5A4;_LL312: if(_tmp59C <= (void*)4)goto _LL314;if(*((int*)
_tmp59C)!= 4)goto _LL314;_tmp59D=((struct Cyc_Absyn_PointerType_struct*)_tmp59C)->f1;
_tmp59E=(void*)_tmp59D.elt_typ;_tmp59F=_tmp59D.elt_tq;_tmp5A0=_tmp59D.ptr_atts;
_tmp5A1=(void*)_tmp5A0.rgn;_tmp5A2=_tmp5A0.nullable;_tmp5A3=_tmp5A0.bounds;
_tmp5A4=_tmp5A0.zero_term;_LL313:{void*_tmp5A5=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,
_tmp5A3);struct Cyc_Absyn_Exp*_tmp5A6;_LL317: if(_tmp5A5 <= (void*)1)goto _LL319;if(*((
int*)_tmp5A5)!= 0)goto _LL319;_tmp5A6=((struct Cyc_Absyn_Upper_b_struct*)_tmp5A5)->f1;
_LL318: {int do_null_check=Cyc_Toc_need_null_check(_tmp43A);Cyc_Toc_exp_to_c(nv,
_tmp43A);if(do_null_check){if(Cyc_Toc_warn_all_null_deref){const char*_tmpB40;
void*_tmpB3F;(_tmpB3F=0,Cyc_Tcutil_warn(e->loc,((_tmpB40="inserted null check due to dereference",
_tag_dyneither(_tmpB40,sizeof(char),39))),_tag_dyneither(_tmpB3F,sizeof(void*),0)));}{
struct Cyc_List_List*_tmpB41;(void*)(_tmp43A->r=(void*)Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(
_tmp59B),Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,((_tmpB41=_cycalloc(sizeof(*
_tmpB41)),((_tmpB41->hd=Cyc_Absyn_copy_exp(_tmp43A),((_tmpB41->tl=0,_tmpB41)))))),
0)));}}if(!((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,
_tmp5A4)){unsigned int _tmp5AB;int _tmp5AC;struct _tuple5 _tmp5AA=Cyc_Evexp_eval_const_uint_exp(
_tmp5A6);_tmp5AB=_tmp5AA.f1;_tmp5AC=_tmp5AA.f2;if(!_tmp5AC  || _tmp5AB <= 0){const
char*_tmpB44;void*_tmpB43;(_tmpB43=0,Cyc_Tcutil_terr(e->loc,((_tmpB44="cannot determine dereference is in bounds",
_tag_dyneither(_tmpB44,sizeof(char),42))),_tag_dyneither(_tmpB43,sizeof(void*),0)));}}
goto _LL316;}_LL319: if((int)_tmp5A5 != 0)goto _LL316;_LL31A: {struct Cyc_Absyn_Exp*
_tmp5AF=Cyc_Absyn_uint_exp(0,0);{struct Cyc_Core_Opt*_tmpB45;_tmp5AF->topt=((
_tmpB45=_cycalloc(sizeof(*_tmpB45)),((_tmpB45->v=(void*)Cyc_Absyn_uint_typ,
_tmpB45))));}(void*)(e->r=(void*)Cyc_Toc_subscript_exp_r(_tmp43A,_tmp5AF));Cyc_Toc_exp_to_c(
nv,e);goto _LL316;}_LL316:;}goto _LL311;_LL314:;_LL315: {const char*_tmpB48;void*
_tmpB47;(_tmpB47=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpB48="exp_to_c: Deref: non-pointer",_tag_dyneither(_tmpB48,sizeof(char),29))),
_tag_dyneither(_tmpB47,sizeof(void*),0)));}_LL311:;}goto _LL202;}_LL237: if(*((int*)
_tmp405)!= 23)goto _LL239;_tmp43B=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp405)->f1;
_tmp43C=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp405)->f2;_tmp43D=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp405)->f3;_tmp43E=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp405)->f4;_LL238: {
int is_poly=Cyc_Toc_is_poly_project(e);void*e1_cyc_type=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp43B->topt))->v;Cyc_Toc_exp_to_c(nv,_tmp43B);if(_tmp43D  && 
_tmp43E)(void*)(e->r=(void*)Cyc_Toc_check_tagged_union(_tmp43B,Cyc_Toc_typ_to_c(
e1_cyc_type),e1_cyc_type,_tmp43C,Cyc_Absyn_aggrmember_exp));if(is_poly)(void*)(e->r=(
void*)((void*)(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v),Cyc_Absyn_new_exp((void*)e->r,0),0))->r));goto _LL202;}
_LL239: if(*((int*)_tmp405)!= 24)goto _LL23B;_tmp43F=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp405)->f1;_tmp440=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp405)->f2;_tmp441=((
struct Cyc_Absyn_AggrArrow_e_struct*)_tmp405)->f3;_tmp442=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp405)->f4;_LL23A: {void*e1typ=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp43F->topt))->v);int do_null_check=Cyc_Toc_need_null_check(_tmp43F);
Cyc_Toc_exp_to_c(nv,_tmp43F);{int is_poly=Cyc_Toc_is_poly_project(e);void*_tmp5B4;
struct Cyc_Absyn_Tqual _tmp5B5;struct Cyc_Absyn_PtrAtts _tmp5B6;void*_tmp5B7;struct
Cyc_Absyn_Conref*_tmp5B8;struct Cyc_Absyn_Conref*_tmp5B9;struct Cyc_Absyn_Conref*
_tmp5BA;struct Cyc_Absyn_PtrInfo _tmp5B3=Cyc_Toc_get_ptr_type(e1typ);_tmp5B4=(void*)
_tmp5B3.elt_typ;_tmp5B5=_tmp5B3.elt_tq;_tmp5B6=_tmp5B3.ptr_atts;_tmp5B7=(void*)
_tmp5B6.rgn;_tmp5B8=_tmp5B6.nullable;_tmp5B9=_tmp5B6.bounds;_tmp5BA=_tmp5B6.zero_term;{
void*_tmp5BB=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp5B9);struct Cyc_Absyn_Exp*
_tmp5BC;_LL31C: if(_tmp5BB <= (void*)1)goto _LL31E;if(*((int*)_tmp5BB)!= 0)goto
_LL31E;_tmp5BC=((struct Cyc_Absyn_Upper_b_struct*)_tmp5BB)->f1;_LL31D: {
unsigned int _tmp5BE;int _tmp5BF;struct _tuple5 _tmp5BD=Cyc_Evexp_eval_const_uint_exp(
_tmp5BC);_tmp5BE=_tmp5BD.f1;_tmp5BF=_tmp5BD.f2;if(_tmp5BF){if(_tmp5BE < 1){const
char*_tmpB4B;void*_tmpB4A;(_tmpB4A=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpB4B="exp_to_c:  AggrArrow_e on pointer of size 0",
_tag_dyneither(_tmpB4B,sizeof(char),44))),_tag_dyneither(_tmpB4A,sizeof(void*),0)));}
if(do_null_check){if(Cyc_Toc_warn_all_null_deref){const char*_tmpB4E;void*_tmpB4D;(
_tmpB4D=0,Cyc_Tcutil_warn(e->loc,((_tmpB4E="inserted null check due to dereference",
_tag_dyneither(_tmpB4E,sizeof(char),39))),_tag_dyneither(_tmpB4D,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*_tmpB4F[1];(void*)(_tmp43F->r=(void*)Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(
e1typ),Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,((_tmpB4F[0]=Cyc_Absyn_new_exp((
void*)_tmp43F->r,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpB4F,sizeof(struct Cyc_Absyn_Exp*),1)))),0)));}}}else{if(!Cyc_Evexp_c_can_eval(
_tmp5BC)){const char*_tmpB52;void*_tmpB51;(_tmpB51=0,Cyc_Tcutil_terr(e->loc,((
_tmpB52="cannot determine pointer dereference in bounds",_tag_dyneither(_tmpB52,
sizeof(char),47))),_tag_dyneither(_tmpB51,sizeof(void*),0)));}{struct Cyc_Absyn_Exp*
_tmpB53[4];(void*)(_tmp43F->r=(void*)Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(e1typ),
Cyc_Absyn_fncall_exp(Cyc_Toc__check_known_subscript_null_e,((_tmpB53[3]=Cyc_Absyn_uint_exp(
0,0),((_tmpB53[2]=Cyc_Absyn_sizeoftyp_exp(_tmp5B4,0),((_tmpB53[1]=_tmp5BC,((
_tmpB53[0]=Cyc_Absyn_new_exp((void*)_tmp43F->r,0),((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB53,sizeof(struct Cyc_Absyn_Exp*),
4)))))))))),0)));}}goto _LL31B;}_LL31E: if((int)_tmp5BB != 0)goto _LL31B;_LL31F: {
void*ta1=Cyc_Toc_typ_to_c_array(_tmp5B4);{struct Cyc_Absyn_Exp*_tmpB54[3];(void*)(
_tmp43F->r=(void*)Cyc_Toc_cast_it_r(Cyc_Absyn_cstar_typ(ta1,_tmp5B5),Cyc_Absyn_fncall_exp(
Cyc_Toc__check_dyneither_subscript_e,((_tmpB54[2]=Cyc_Absyn_uint_exp(0,0),((
_tmpB54[1]=Cyc_Absyn_sizeoftyp_exp(ta1,0),((_tmpB54[0]=Cyc_Absyn_new_exp((void*)
_tmp43F->r,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpB54,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0)));}goto _LL31B;}
_LL31B:;}if(_tmp441  && _tmp442)(void*)(e->r=(void*)Cyc_Toc_check_tagged_union(
_tmp43F,Cyc_Toc_typ_to_c(e1typ),_tmp5B4,_tmp440,Cyc_Absyn_aggrarrow_exp));if(
is_poly)(void*)(e->r=(void*)((void*)(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),Cyc_Absyn_new_exp((void*)e->r,
0),0))->r));goto _LL202;}}_LL23B: if(*((int*)_tmp405)!= 25)goto _LL23D;_tmp443=((
struct Cyc_Absyn_Subscript_e_struct*)_tmp405)->f1;_tmp444=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp405)->f2;_LL23C: {void*_tmp5C9=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp443->topt))->v);{void*_tmp5CA=_tmp5C9;struct Cyc_List_List*
_tmp5CB;struct Cyc_Absyn_PtrInfo _tmp5CC;void*_tmp5CD;struct Cyc_Absyn_Tqual _tmp5CE;
struct Cyc_Absyn_PtrAtts _tmp5CF;void*_tmp5D0;struct Cyc_Absyn_Conref*_tmp5D1;
struct Cyc_Absyn_Conref*_tmp5D2;struct Cyc_Absyn_Conref*_tmp5D3;_LL321: if(_tmp5CA
<= (void*)4)goto _LL325;if(*((int*)_tmp5CA)!= 9)goto _LL323;_tmp5CB=((struct Cyc_Absyn_TupleType_struct*)
_tmp5CA)->f1;_LL322: Cyc_Toc_exp_to_c(nv,_tmp443);Cyc_Toc_exp_to_c(nv,_tmp444);{
unsigned int _tmp5D5;int _tmp5D6;struct _tuple5 _tmp5D4=Cyc_Evexp_eval_const_uint_exp(
_tmp444);_tmp5D5=_tmp5D4.f1;_tmp5D6=_tmp5D4.f2;if(!_tmp5D6){const char*_tmpB57;
void*_tmpB56;(_tmpB56=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmpB57="unknown tuple subscript in translation to C",
_tag_dyneither(_tmpB57,sizeof(char),44))),_tag_dyneither(_tmpB56,sizeof(void*),0)));}(
void*)(e->r=(void*)Cyc_Toc_aggrmember_exp_r(_tmp443,Cyc_Absyn_fieldname((int)(
_tmp5D5 + 1))));goto _LL320;}_LL323: if(*((int*)_tmp5CA)!= 4)goto _LL325;_tmp5CC=((
struct Cyc_Absyn_PointerType_struct*)_tmp5CA)->f1;_tmp5CD=(void*)_tmp5CC.elt_typ;
_tmp5CE=_tmp5CC.elt_tq;_tmp5CF=_tmp5CC.ptr_atts;_tmp5D0=(void*)_tmp5CF.rgn;
_tmp5D1=_tmp5CF.nullable;_tmp5D2=_tmp5CF.bounds;_tmp5D3=_tmp5CF.zero_term;_LL324: {
struct Cyc_List_List*_tmp5D9=Cyc_Toc_get_relns(_tmp443);int in_bnds=0;{void*
_tmp5DA=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp5D2);_LL328:;_LL329:
in_bnds=Cyc_Toc_check_bounds(_tmp5D9,_tmp443,_tmp444);if(Cyc_Toc_warn_bounds_checks
 && !in_bnds){const char*_tmpB5B;void*_tmpB5A[1];struct Cyc_String_pa_struct
_tmpB59;(_tmpB59.tag=0,((_tmpB59.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(e)),((_tmpB5A[0]=& _tmpB59,Cyc_Tcutil_warn(e->loc,((
_tmpB5B="bounds check necessary for %s",_tag_dyneither(_tmpB5B,sizeof(char),30))),
_tag_dyneither(_tmpB5A,sizeof(void*),1)))))));}_LL327:;}Cyc_Toc_exp_to_c(nv,
_tmp443);Cyc_Toc_exp_to_c(nv,_tmp444);++ Cyc_Toc_total_bounds_checks;{void*
_tmp5DE=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp5D2);struct Cyc_Absyn_Exp*
_tmp5DF;_LL32B: if(_tmp5DE <= (void*)1)goto _LL32D;if(*((int*)_tmp5DE)!= 0)goto
_LL32D;_tmp5DF=((struct Cyc_Absyn_Upper_b_struct*)_tmp5DE)->f1;_LL32C: {int
possibly_null=((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,
_tmp5D1);void*ta1=Cyc_Toc_typ_to_c(_tmp5CD);void*ta2=Cyc_Absyn_cstar_typ(ta1,
_tmp5CE);if(in_bnds)++ Cyc_Toc_bounds_checks_eliminated;else{if(((int(*)(int,
struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp5D3)){if(!Cyc_Evexp_c_can_eval(
_tmp5DF)){const char*_tmpB5E;void*_tmpB5D;(_tmpB5D=0,Cyc_Tcutil_terr(e->loc,((
_tmpB5E="cannot determine subscript is in bounds",_tag_dyneither(_tmpB5E,sizeof(
char),40))),_tag_dyneither(_tmpB5D,sizeof(void*),0)));}{struct Cyc_Absyn_Exp*
function_e=Cyc_Toc_getFunction(& Cyc_Toc__zero_arr_plus_functionSet,_tmp443);
struct Cyc_Absyn_Exp*_tmpB5F[3];(void*)(e->r=(void*)Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(
ta2,Cyc_Absyn_fncall_exp(function_e,((_tmpB5F[2]=_tmp444,((_tmpB5F[1]=_tmp5DF,((
_tmpB5F[0]=_tmp443,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpB5F,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0))));}}else{if(
possibly_null){if(!Cyc_Evexp_c_can_eval(_tmp5DF)){const char*_tmpB62;void*_tmpB61;(
_tmpB61=0,Cyc_Tcutil_terr(e->loc,((_tmpB62="cannot determine subscript is in bounds",
_tag_dyneither(_tmpB62,sizeof(char),40))),_tag_dyneither(_tmpB61,sizeof(void*),0)));}
if(Cyc_Toc_warn_all_null_deref){const char*_tmpB65;void*_tmpB64;(_tmpB64=0,Cyc_Tcutil_warn(
e->loc,((_tmpB65="inserted null check due to dereference",_tag_dyneither(_tmpB65,
sizeof(char),39))),_tag_dyneither(_tmpB64,sizeof(void*),0)));}{struct Cyc_Absyn_Exp*
_tmpB66[4];(void*)(e->r=(void*)Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(ta2,Cyc_Absyn_fncall_exp(
Cyc_Toc__check_known_subscript_null_e,((_tmpB66[3]=_tmp444,((_tmpB66[2]=Cyc_Absyn_sizeoftyp_exp(
ta1,0),((_tmpB66[1]=_tmp5DF,((_tmpB66[0]=_tmp443,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB66,sizeof(struct Cyc_Absyn_Exp*),
4)))))))))),0))));}}else{struct Cyc_Absyn_Exp*_tmpB67[2];(void*)(_tmp444->r=(void*)
Cyc_Toc_fncall_exp_r(Cyc_Toc__check_known_subscript_notnull_e,((_tmpB67[1]=Cyc_Absyn_copy_exp(
_tmp444),((_tmpB67[0]=_tmp5DF,((struct Cyc_List_List*(*)(struct _dyneither_ptr))
Cyc_List_list)(_tag_dyneither(_tmpB67,sizeof(struct Cyc_Absyn_Exp*),2))))))));}}}
goto _LL32A;}_LL32D: if((int)_tmp5DE != 0)goto _LL32A;_LL32E: {void*ta1=Cyc_Toc_typ_to_c_array(
_tmp5CD);if(in_bnds){++ Cyc_Toc_bounds_checks_eliminated;(void*)(e->r=(void*)Cyc_Toc_subscript_exp_r(
Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(ta1,_tmp5CE),Cyc_Absyn_aggrmember_exp(
_tmp443,Cyc_Toc_curr_sp,0)),_tmp444));}else{struct Cyc_Absyn_Exp*_tmp5E9=Cyc_Toc__check_dyneither_subscript_e;
struct Cyc_Absyn_Exp*_tmpB68[3];(void*)(e->r=(void*)Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(
Cyc_Absyn_cstar_typ(ta1,_tmp5CE),Cyc_Absyn_fncall_exp(_tmp5E9,((_tmpB68[2]=
_tmp444,((_tmpB68[1]=Cyc_Absyn_sizeoftyp_exp(ta1,0),((_tmpB68[0]=_tmp443,((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmpB68,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0))));}goto _LL32A;}_LL32A:;}goto
_LL320;}_LL325:;_LL326: {const char*_tmpB6B;void*_tmpB6A;(_tmpB6A=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB6B="exp_to_c: Subscript on non-tuple/array/tuple ptr",
_tag_dyneither(_tmpB6B,sizeof(char),49))),_tag_dyneither(_tmpB6A,sizeof(void*),0)));}
_LL320:;}goto _LL202;}_LL23D: if(*((int*)_tmp405)!= 26)goto _LL23F;_tmp445=((struct
Cyc_Absyn_Tuple_e_struct*)_tmp405)->f1;_LL23E: if(!Cyc_Toc_is_toplevel(nv))(void*)(
e->r=(void*)((void*)(Cyc_Toc_init_tuple(nv,0,0,_tmp445))->r));else{struct Cyc_List_List*
_tmp5ED=((struct Cyc_List_List*(*)(struct _tuple4*(*f)(struct Cyc_Absyn_Exp*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_tup_to_c,_tmp445);void*_tmp5EE=Cyc_Toc_add_tuple_type(
_tmp5ED);struct Cyc_List_List*dles=0;{int i=1;for(0;_tmp445 != 0;(_tmp445=_tmp445->tl,
i ++)){Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp445->hd);{struct _tuple8*
_tmpB6E;struct Cyc_List_List*_tmpB6D;dles=((_tmpB6D=_cycalloc(sizeof(*_tmpB6D)),((
_tmpB6D->hd=((_tmpB6E=_cycalloc(sizeof(*_tmpB6E)),((_tmpB6E->f1=0,((_tmpB6E->f2=(
struct Cyc_Absyn_Exp*)_tmp445->hd,_tmpB6E)))))),((_tmpB6D->tl=dles,_tmpB6D))))));}}}
dles=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);(
void*)(e->r=(void*)Cyc_Toc_unresolvedmem_exp_r(0,dles));}goto _LL202;_LL23F: if(*((
int*)_tmp405)!= 28)goto _LL241;_tmp446=((struct Cyc_Absyn_Array_e_struct*)_tmp405)->f1;
_LL240:(void*)(e->r=(void*)Cyc_Toc_unresolvedmem_exp_r(0,_tmp446));{struct Cyc_List_List*
_tmp5F1=_tmp446;for(0;_tmp5F1 != 0;_tmp5F1=_tmp5F1->tl){struct _tuple8 _tmp5F3;
struct Cyc_Absyn_Exp*_tmp5F4;struct _tuple8*_tmp5F2=(struct _tuple8*)_tmp5F1->hd;
_tmp5F3=*_tmp5F2;_tmp5F4=_tmp5F3.f2;Cyc_Toc_exp_to_c(nv,_tmp5F4);}}goto _LL202;
_LL241: if(*((int*)_tmp405)!= 29)goto _LL243;_tmp447=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp405)->f1;_tmp448=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp405)->f2;
_tmp449=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp405)->f3;_tmp44A=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmp405)->f4;_LL242: {unsigned int _tmp5F6;int
_tmp5F7;struct _tuple5 _tmp5F5=Cyc_Evexp_eval_const_uint_exp(_tmp448);_tmp5F6=
_tmp5F5.f1;_tmp5F7=_tmp5F5.f2;{void*_tmp5F8=Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp449->topt))->v);Cyc_Toc_exp_to_c(nv,_tmp449);{struct Cyc_List_List*
es=0;if(!Cyc_Toc_is_zero(_tmp449)){if(!_tmp5F7){const char*_tmpB71;void*_tmpB70;(
_tmpB70=0,Cyc_Tcutil_terr(_tmp448->loc,((_tmpB71="cannot determine value of constant",
_tag_dyneither(_tmpB71,sizeof(char),35))),_tag_dyneither(_tmpB70,sizeof(void*),0)));}{
unsigned int i=0;for(0;i < _tmp5F6;++ i){struct _tuple8*_tmpB74;struct Cyc_List_List*
_tmpB73;es=((_tmpB73=_cycalloc(sizeof(*_tmpB73)),((_tmpB73->hd=((_tmpB74=
_cycalloc(sizeof(*_tmpB74)),((_tmpB74->f1=0,((_tmpB74->f2=_tmp449,_tmpB74)))))),((
_tmpB73->tl=es,_tmpB73))))));}}if(_tmp44A){struct Cyc_Absyn_Exp*_tmp5FD=Cyc_Toc_cast_it(
_tmp5F8,Cyc_Absyn_uint_exp(0,0));struct _tuple8*_tmpB77;struct Cyc_List_List*
_tmpB76;es=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_imp_append)(es,((_tmpB76=_cycalloc(sizeof(*_tmpB76)),((_tmpB76->hd=((
_tmpB77=_cycalloc(sizeof(*_tmpB77)),((_tmpB77->f1=0,((_tmpB77->f2=_tmp5FD,
_tmpB77)))))),((_tmpB76->tl=0,_tmpB76)))))));}}(void*)(e->r=(void*)Cyc_Toc_unresolvedmem_exp_r(
0,es));goto _LL202;}}}_LL243: if(*((int*)_tmp405)!= 30)goto _LL245;_tmp44B=((struct
Cyc_Absyn_Aggregate_e_struct*)_tmp405)->f1;_tmp44C=((struct Cyc_Absyn_Aggregate_e_struct*)
_tmp405)->f2;_tmp44D=((struct Cyc_Absyn_Aggregate_e_struct*)_tmp405)->f3;_tmp44E=((
struct Cyc_Absyn_Aggregate_e_struct*)_tmp405)->f4;_LL244: if(!Cyc_Toc_is_toplevel(
nv))(void*)(e->r=(void*)((void*)(Cyc_Toc_init_struct(nv,old_typ,_tmp44C != 0,0,0,
_tmp44D,_tmp44B))->r));else{if(_tmp44E == 0){const char*_tmpB7A;void*_tmpB79;(
_tmpB79=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpB7A="Aggregate_e: missing aggrdecl pointer",_tag_dyneither(_tmpB7A,sizeof(
char),38))),_tag_dyneither(_tmpB79,sizeof(void*),0)));}{struct Cyc_Absyn_Aggrdecl*
sd2=(struct Cyc_Absyn_Aggrdecl*)_tmp44E;struct Cyc_Toc_Env _tmp603;struct
_RegionHandle*_tmp604;struct Cyc_Toc_Env*_tmp602=nv;_tmp603=*_tmp602;_tmp604=
_tmp603.rgn;{struct Cyc_List_List*_tmp605=((struct Cyc_List_List*(*)(struct
_RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,void*,
struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(_tmp604,e->loc,
_tmp44D,(void*)sd2->kind,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(sd2->impl))->fields);
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp44E->impl))->tagged){struct
_tuple12 _tmp607;struct Cyc_Absyn_Aggrfield*_tmp608;struct Cyc_Absyn_Exp*_tmp609;
struct _tuple12*_tmp606=(struct _tuple12*)((struct Cyc_List_List*)_check_null(
_tmp605))->hd;_tmp607=*_tmp606;_tmp608=_tmp607.f1;_tmp609=_tmp607.f2;{void*
_tmp60A=(void*)_tmp608->type;Cyc_Toc_exp_to_c(nv,_tmp609);if(Cyc_Toc_is_void_star(
_tmp60A))(void*)(_tmp609->r=(void*)Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),
Cyc_Absyn_new_exp((void*)_tmp609->r,0)));{int i=Cyc_Toc_get_member_offset((struct
Cyc_Absyn_Aggrdecl*)_tmp44E,_tmp608->name);struct Cyc_Absyn_Exp*field_tag_exp=Cyc_Absyn_signed_int_exp(
i,0);struct _tuple8*_tmpB7F;struct _tuple8*_tmpB7E;struct _tuple8*_tmpB7D[2];struct
Cyc_List_List*_tmp60B=(_tmpB7D[1]=((_tmpB7F=_cycalloc(sizeof(*_tmpB7F)),((
_tmpB7F->f1=0,((_tmpB7F->f2=_tmp609,_tmpB7F)))))),((_tmpB7D[0]=((_tmpB7E=
_cycalloc(sizeof(*_tmpB7E)),((_tmpB7E->f1=0,((_tmpB7E->f2=field_tag_exp,_tmpB7E)))))),((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmpB7D,sizeof(struct _tuple8*),2)))));struct Cyc_Absyn_Exp*umem=Cyc_Absyn_unresolvedmem_exp(
0,_tmp60B,0);struct Cyc_Absyn_FieldName_struct*_tmpB85;struct Cyc_Absyn_FieldName_struct
_tmpB84;void*_tmpB83[1];struct Cyc_List_List*ds=(_tmpB83[0]=(void*)((_tmpB85=
_cycalloc(sizeof(*_tmpB85)),((_tmpB85[0]=((_tmpB84.tag=1,((_tmpB84.f1=_tmp608->name,
_tmpB84)))),_tmpB85)))),Cyc_List_list(_tag_dyneither(_tmpB83,sizeof(void*),1)));
struct _tuple8*_tmpB88;struct _tuple8*_tmpB87[1];struct Cyc_List_List*dles=(_tmpB87[
0]=((_tmpB88=_cycalloc(sizeof(*_tmpB88)),((_tmpB88->f1=ds,((_tmpB88->f2=umem,
_tmpB88)))))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpB87,sizeof(struct _tuple8*),1)));(void*)(e->r=(void*)Cyc_Toc_unresolvedmem_exp_r(
0,dles));}}}else{struct Cyc_List_List*_tmp614=0;struct Cyc_List_List*_tmp615=((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(sd2->impl))->fields;for(0;_tmp615 != 0;
_tmp615=_tmp615->tl){struct Cyc_List_List*_tmp616=_tmp605;for(0;_tmp616 != 0;
_tmp616=_tmp616->tl){if((*((struct _tuple12*)_tmp616->hd)).f1 == (struct Cyc_Absyn_Aggrfield*)
_tmp615->hd){struct _tuple12 _tmp618;struct Cyc_Absyn_Aggrfield*_tmp619;struct Cyc_Absyn_Exp*
_tmp61A;struct _tuple12*_tmp617=(struct _tuple12*)_tmp616->hd;_tmp618=*_tmp617;
_tmp619=_tmp618.f1;_tmp61A=_tmp618.f2;{void*_tmp61B=(void*)_tmp619->type;Cyc_Toc_exp_to_c(
nv,_tmp61A);if(Cyc_Toc_is_void_star(_tmp61B))(void*)(_tmp61A->r=(void*)Cyc_Toc_cast_it_r(
Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp((void*)_tmp61A->r,0)));{struct
_tuple8*_tmpB8B;struct Cyc_List_List*_tmpB8A;_tmp614=((_tmpB8A=_cycalloc(sizeof(*
_tmpB8A)),((_tmpB8A->hd=((_tmpB8B=_cycalloc(sizeof(*_tmpB8B)),((_tmpB8B->f1=0,((
_tmpB8B->f2=_tmp61A,_tmpB8B)))))),((_tmpB8A->tl=_tmp614,_tmpB8A))))));}break;}}}}(
void*)(e->r=(void*)Cyc_Toc_unresolvedmem_exp_r(0,((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp614)));}}}}goto _LL202;_LL245: if(*((
int*)_tmp405)!= 31)goto _LL247;_tmp44F=(void*)((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp405)->f1;_tmp450=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp405)->f2;_LL246: {
struct Cyc_List_List*fs;{void*_tmp61E=Cyc_Tcutil_compress(_tmp44F);struct Cyc_List_List*
_tmp61F;_LL330: if(_tmp61E <= (void*)4)goto _LL332;if(*((int*)_tmp61E)!= 11)goto
_LL332;_tmp61F=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp61E)->f2;_LL331: fs=
_tmp61F;goto _LL32F;_LL332:;_LL333: {const char*_tmpB8F;void*_tmpB8E[1];struct Cyc_String_pa_struct
_tmpB8D;(_tmpB8D.tag=0,((_tmpB8D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp44F)),((_tmpB8E[0]=& _tmpB8D,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB8F="anon struct has type %s",
_tag_dyneither(_tmpB8F,sizeof(char),24))),_tag_dyneither(_tmpB8E,sizeof(void*),1)))))));}
_LL32F:;}{struct Cyc_Toc_Env _tmp624;struct _RegionHandle*_tmp625;struct Cyc_Toc_Env*
_tmp623=nv;_tmp624=*_tmp623;_tmp625=_tmp624.rgn;{struct Cyc_List_List*_tmp626=((
struct Cyc_List_List*(*)(struct _RegionHandle*rgn,struct Cyc_Position_Segment*loc,
struct Cyc_List_List*des,void*,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(
_tmp625,e->loc,_tmp450,(void*)0,fs);for(0;_tmp626 != 0;_tmp626=_tmp626->tl){
struct _tuple12 _tmp628;struct Cyc_Absyn_Aggrfield*_tmp629;struct Cyc_Absyn_Exp*
_tmp62A;struct _tuple12*_tmp627=(struct _tuple12*)_tmp626->hd;_tmp628=*_tmp627;
_tmp629=_tmp628.f1;_tmp62A=_tmp628.f2;{void*_tmp62B=(void*)_tmp629->type;Cyc_Toc_exp_to_c(
nv,_tmp62A);if(Cyc_Toc_is_void_star(_tmp62B))(void*)(_tmp62A->r=(void*)Cyc_Toc_cast_it_r(
Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp((void*)_tmp62A->r,0)));}}(void*)(e->r=(
void*)Cyc_Toc_unresolvedmem_exp_r(0,_tmp450));}goto _LL202;}}_LL247: if(*((int*)
_tmp405)!= 32)goto _LL249;_tmp451=((struct Cyc_Absyn_Datatype_e_struct*)_tmp405)->f1;
if(_tmp451 != 0)goto _LL249;_tmp452=((struct Cyc_Absyn_Datatype_e_struct*)_tmp405)->f2;
_tmp453=((struct Cyc_Absyn_Datatype_e_struct*)_tmp405)->f3;if(!(!_tmp452->is_flat))
goto _LL249;_LL248: {struct _tuple1*qv=_tmp453->name;struct Cyc_Absyn_Exp*tag_exp=
_tmp452->is_extensible?Cyc_Absyn_var_exp(qv,0): Cyc_Toc_datatype_tag(_tmp452,qv,0);(
void*)(e->r=(void*)((void*)tag_exp->r));goto _LL202;}_LL249: if(*((int*)_tmp405)!= 
32)goto _LL24B;_tmp454=((struct Cyc_Absyn_Datatype_e_struct*)_tmp405)->f1;_tmp455=((
struct Cyc_Absyn_Datatype_e_struct*)_tmp405)->f2;_tmp456=((struct Cyc_Absyn_Datatype_e_struct*)
_tmp405)->f3;_LL24A: {void*datatype_ctype;struct Cyc_Absyn_Exp*tag_exp;struct
_tuple1*_tmp62C=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp62D=Cyc_Absyn_var_exp(
_tmp62C,0);struct Cyc_Absyn_Exp*member_exp;if(_tmp455->is_flat){{const char*
_tmpB90;datatype_ctype=Cyc_Absyn_unionq_typ(Cyc_Toc_collapse_qvar_tag(_tmp455->name,((
_tmpB90="_union",_tag_dyneither(_tmpB90,sizeof(char),7)))));}tag_exp=Cyc_Toc_datatype_tag(
_tmp455,_tmp456->name,1);{struct _tuple1 _tmp630;struct _dyneither_ptr*_tmp631;
struct _tuple1*_tmp62F=_tmp456->name;_tmp630=*_tmp62F;_tmp631=_tmp630.f2;
member_exp=Cyc_Absyn_aggrmember_exp(_tmp62D,_tmp631,0);}}else{{const char*_tmpB91;
datatype_ctype=Cyc_Absyn_strctq(Cyc_Toc_collapse_qvar_tag(_tmp456->name,((
_tmpB91="_struct",_tag_dyneither(_tmpB91,sizeof(char),8)))));}tag_exp=_tmp455->is_extensible?
Cyc_Absyn_var_exp(_tmp456->name,0): Cyc_Toc_datatype_tag(_tmp455,_tmp456->name,1);
member_exp=_tmp62D;}{struct Cyc_List_List*_tmp633=_tmp456->typs;if(Cyc_Toc_is_toplevel(
nv)){struct Cyc_List_List*dles=0;for(0;_tmp454 != 0;(_tmp454=_tmp454->tl,_tmp633=
_tmp633->tl)){struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)_tmp454->hd;void*
field_typ=Cyc_Toc_typ_to_c((*((struct _tuple4*)((struct Cyc_List_List*)_check_null(
_tmp633))->hd)).f2);Cyc_Toc_exp_to_c(nv,cur_e);if(Cyc_Toc_is_void_star(field_typ))
cur_e=Cyc_Toc_cast_it(field_typ,cur_e);{struct _tuple8*_tmpB94;struct Cyc_List_List*
_tmpB93;dles=((_tmpB93=_cycalloc(sizeof(*_tmpB93)),((_tmpB93->hd=((_tmpB94=
_cycalloc(sizeof(*_tmpB94)),((_tmpB94->f1=0,((_tmpB94->f2=cur_e,_tmpB94)))))),((
_tmpB93->tl=dles,_tmpB93))))));}}{struct _tuple8*_tmpB97;struct Cyc_List_List*
_tmpB96;dles=((_tmpB96=_cycalloc(sizeof(*_tmpB96)),((_tmpB96->hd=((_tmpB97=
_cycalloc(sizeof(*_tmpB97)),((_tmpB97->f1=0,((_tmpB97->f2=tag_exp,_tmpB97)))))),((
_tmpB96->tl=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
dles),_tmpB96))))));}(void*)(e->r=(void*)Cyc_Toc_unresolvedmem_exp_r(0,dles));
if(_tmp455->is_flat){const char*_tmpB98;(void*)(e->r=(void*)Cyc_Toc_cast_it_r(Cyc_Absyn_strctq(
Cyc_Toc_collapse_qvar_tag(_tmp456->name,((_tmpB98="_struct",_tag_dyneither(
_tmpB98,sizeof(char),8))))),Cyc_Absyn_copy_exp(e)));}}else{struct Cyc_List_List*
_tmpB99;struct Cyc_List_List*_tmp639=(_tmpB99=_cycalloc(sizeof(*_tmpB99)),((
_tmpB99->hd=Cyc_Absyn_assign_stmt(Cyc_Absyn_aggrmember_exp(member_exp,Cyc_Toc_tag_sp,
0),tag_exp,0),((_tmpB99->tl=0,_tmpB99)))));{int i=1;for(0;_tmp454 != 0;(((_tmp454=
_tmp454->tl,i ++)),_tmp633=_tmp633->tl)){struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)
_tmp454->hd;void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple4*)((struct Cyc_List_List*)
_check_null(_tmp633))->hd)).f2);Cyc_Toc_exp_to_c(nv,cur_e);if(Cyc_Toc_is_void_star(
field_typ))cur_e=Cyc_Toc_cast_it(field_typ,cur_e);{struct Cyc_Absyn_Stmt*_tmp63A=
Cyc_Absyn_assign_stmt(Cyc_Absyn_aggrmember_exp(member_exp,Cyc_Absyn_fieldname(i),
0),cur_e,0);struct Cyc_List_List*_tmpB9A;_tmp639=((_tmpB9A=_cycalloc(sizeof(*
_tmpB9A)),((_tmpB9A->hd=_tmp63A,((_tmpB9A->tl=_tmp639,_tmpB9A))))));}}}{struct
Cyc_Absyn_Stmt*_tmp63C=Cyc_Absyn_exp_stmt(_tmp62D,0);struct Cyc_List_List*_tmpB9B;
struct Cyc_Absyn_Stmt*_tmp63D=Cyc_Absyn_seq_stmts(((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(((_tmpB9B=_cycalloc(sizeof(*_tmpB9B)),((
_tmpB9B->hd=_tmp63C,((_tmpB9B->tl=_tmp639,_tmpB9B))))))),0);(void*)(e->r=(void*)
Cyc_Toc_stmt_exp_r(Cyc_Absyn_declare_stmt(_tmp62C,datatype_ctype,0,_tmp63D,0)));}}
goto _LL202;}}_LL24B: if(*((int*)_tmp405)!= 33)goto _LL24D;_LL24C: goto _LL24E;_LL24D:
if(*((int*)_tmp405)!= 34)goto _LL24F;_LL24E: goto _LL202;_LL24F: if(*((int*)_tmp405)
!= 35)goto _LL251;_tmp457=((struct Cyc_Absyn_Malloc_e_struct*)_tmp405)->f1;_tmp458=
_tmp457.is_calloc;_tmp459=_tmp457.rgn;_tmp45A=_tmp457.elt_type;_tmp45B=_tmp457.num_elts;
_tmp45C=_tmp457.fat_result;_LL250: {void*t_c=Cyc_Toc_typ_to_c(*((void**)
_check_null(_tmp45A)));Cyc_Toc_exp_to_c(nv,_tmp45B);if(_tmp45C){struct _tuple1*
_tmp640=Cyc_Toc_temp_var();struct _tuple1*_tmp641=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*
pexp;struct Cyc_Absyn_Exp*xexp;struct Cyc_Absyn_Exp*rexp;if(_tmp458){xexp=_tmp45B;
if(_tmp459 != 0  && !Cyc_Absyn_no_regions){struct Cyc_Absyn_Exp*rgn=(struct Cyc_Absyn_Exp*)
_tmp459;Cyc_Toc_exp_to_c(nv,rgn);pexp=Cyc_Toc_rcalloc_exp(rgn,Cyc_Absyn_sizeoftyp_exp(
t_c,0),Cyc_Absyn_var_exp(_tmp640,0));}else{pexp=Cyc_Toc_calloc_exp(*_tmp45A,Cyc_Absyn_sizeoftyp_exp(
t_c,0),Cyc_Absyn_var_exp(_tmp640,0));}{struct Cyc_Absyn_Exp*_tmpB9C[3];rexp=Cyc_Absyn_fncall_exp(
Cyc_Toc__tag_dyneither_e,((_tmpB9C[2]=Cyc_Absyn_var_exp(_tmp640,0),((_tmpB9C[1]=
Cyc_Absyn_sizeoftyp_exp(t_c,0),((_tmpB9C[0]=Cyc_Absyn_var_exp(_tmp641,0),((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmpB9C,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);}}else{xexp=Cyc_Absyn_times_exp(
Cyc_Absyn_sizeoftyp_exp(t_c,0),_tmp45B,0);if(_tmp459 != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=(struct Cyc_Absyn_Exp*)_tmp459;Cyc_Toc_exp_to_c(nv,rgn);
pexp=Cyc_Toc_rmalloc_exp(rgn,Cyc_Absyn_var_exp(_tmp640,0));}else{pexp=Cyc_Toc_malloc_exp(*
_tmp45A,Cyc_Absyn_var_exp(_tmp640,0));}{struct Cyc_Absyn_Exp*_tmpB9D[3];rexp=Cyc_Absyn_fncall_exp(
Cyc_Toc__tag_dyneither_e,((_tmpB9D[2]=Cyc_Absyn_var_exp(_tmp640,0),((_tmpB9D[1]=
Cyc_Absyn_uint_exp(1,0),((_tmpB9D[0]=Cyc_Absyn_var_exp(_tmp641,0),((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB9D,sizeof(struct Cyc_Absyn_Exp*),
3)))))))),0);}}{struct Cyc_Absyn_Stmt*_tmp644=Cyc_Absyn_declare_stmt(_tmp640,Cyc_Absyn_uint_typ,(
struct Cyc_Absyn_Exp*)xexp,Cyc_Absyn_declare_stmt(_tmp641,Cyc_Absyn_cstar_typ(t_c,
Cyc_Toc_mt_tq),(struct Cyc_Absyn_Exp*)pexp,Cyc_Absyn_exp_stmt(rexp,0),0),0);(void*)(
e->r=(void*)Cyc_Toc_stmt_exp_r(_tmp644));}}else{struct Cyc_Absyn_Exp*_tmp645=Cyc_Absyn_sizeoftyp_exp(
t_c,0);{void*_tmp646=(void*)_tmp45B->r;union Cyc_Absyn_Cnst_union _tmp647;int
_tmp648;_LL335: if(*((int*)_tmp646)!= 0)goto _LL337;_tmp647=((struct Cyc_Absyn_Const_e_struct*)
_tmp646)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)_tmp646)->f1).Int_c).tag != 2)
goto _LL337;_tmp648=(_tmp647.Int_c).f2;if(_tmp648 != 1)goto _LL337;_LL336: goto
_LL334;_LL337:;_LL338: _tmp645=Cyc_Absyn_times_exp(_tmp645,_tmp45B,0);goto _LL334;
_LL334:;}if(_tmp459 != 0  && !Cyc_Absyn_no_regions){struct Cyc_Absyn_Exp*rgn=(
struct Cyc_Absyn_Exp*)_tmp459;Cyc_Toc_exp_to_c(nv,rgn);(void*)(e->r=(void*)((void*)(
Cyc_Toc_rmalloc_exp(rgn,_tmp645))->r));}else{(void*)(e->r=(void*)((void*)(Cyc_Toc_malloc_exp(*
_tmp45A,_tmp645))->r));}}goto _LL202;}_LL251: if(*((int*)_tmp405)!= 36)goto _LL253;
_tmp45D=((struct Cyc_Absyn_Swap_e_struct*)_tmp405)->f1;_tmp45E=((struct Cyc_Absyn_Swap_e_struct*)
_tmp405)->f2;_LL252: {int is_dyneither_ptr=0;void*e1_old_typ=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp45D->topt))->v;void*e2_old_typ=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp45E->topt))->v;if(!Cyc_Tcutil_is_pointer_or_boxed(e1_old_typ,&
is_dyneither_ptr)){const char*_tmpBA0;void*_tmpB9F;(_tmpB9F=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBA0="Swap_e: is_pointer_or_boxed: not a pointer or boxed type",
_tag_dyneither(_tmpBA0,sizeof(char),57))),_tag_dyneither(_tmpB9F,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*swap_fn;if(is_dyneither_ptr)swap_fn=Cyc_Toc__swap_dyneither_e;
else{swap_fn=Cyc_Toc__swap_word_e;}if(!Cyc_Absyn_is_lvalue(_tmp45D)){const char*
_tmpBA4;void*_tmpBA3[1];struct Cyc_String_pa_struct _tmpBA2;(_tmpBA2.tag=0,((
_tmpBA2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(
_tmp45D)),((_tmpBA3[0]=& _tmpBA2,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBA4="Swap_e: %s is not an l-value\n",
_tag_dyneither(_tmpBA4,sizeof(char),30))),_tag_dyneither(_tmpBA3,sizeof(void*),1)))))));}
if(!Cyc_Absyn_is_lvalue(_tmp45E)){const char*_tmpBA8;void*_tmpBA7[1];struct Cyc_String_pa_struct
_tmpBA6;(_tmpBA6.tag=0,((_tmpBA6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(_tmp45E)),((_tmpBA7[0]=& _tmpBA6,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBA8="Swap_e: %s is not an l-value\n",
_tag_dyneither(_tmpBA8,sizeof(char),30))),_tag_dyneither(_tmpBA7,sizeof(void*),1)))))));}
Cyc_Toc_exp_to_c(nv,_tmp45D);Cyc_Toc_exp_to_c(nv,_tmp45E);{struct Cyc_Absyn_Exp*
_tmpBA9[2];(void*)(e->r=(void*)Cyc_Toc_fncall_exp_r(swap_fn,((_tmpBA9[1]=Cyc_Absyn_address_exp(
_tmp45E,0),((_tmpBA9[0]=Cyc_Absyn_address_exp(_tmp45D,0),((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBA9,sizeof(struct Cyc_Absyn_Exp*),
2))))))));}goto _LL202;}}_LL253: if(*((int*)_tmp405)!= 38)goto _LL255;_tmp45F=((
struct Cyc_Absyn_StmtExp_e_struct*)_tmp405)->f1;_LL254: Cyc_Toc_stmt_to_c(nv,
_tmp45F);goto _LL202;_LL255: if(*((int*)_tmp405)!= 37)goto _LL257;_LL256: {const
char*_tmpBAC;void*_tmpBAB;(_tmpBAB=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBAC="UnresolvedMem",_tag_dyneither(
_tmpBAC,sizeof(char),14))),_tag_dyneither(_tmpBAB,sizeof(void*),0)));}_LL257: if(*((
int*)_tmp405)!= 27)goto _LL259;_LL258: {const char*_tmpBAF;void*_tmpBAE;(_tmpBAE=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpBAF="compoundlit",
_tag_dyneither(_tmpBAF,sizeof(char),12))),_tag_dyneither(_tmpBAE,sizeof(void*),0)));}
_LL259: if(*((int*)_tmp405)!= 39)goto _LL202;_LL25A: {const char*_tmpBB2;void*
_tmpBB1;(_tmpBB1=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpBB2="valueof(-)",_tag_dyneither(_tmpBB2,sizeof(char),11))),_tag_dyneither(
_tmpBB1,sizeof(void*),0)));}_LL202:;}}static struct Cyc_Absyn_Stmt*Cyc_Toc_if_neq_stmt(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Stmt*fail_stmt);
static struct Cyc_Absyn_Stmt*Cyc_Toc_if_neq_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2,struct Cyc_Absyn_Stmt*fail_stmt){return Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_neq_exp(
e1,e2,0),fail_stmt,Cyc_Toc_skip_stmt_dl(),0);}struct _tuple13{struct Cyc_Toc_Env*
f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple14{struct _tuple1*
f1;void*f2;};struct _tuple15{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};
static struct _tuple13 Cyc_Toc_xlate_pat(struct Cyc_Toc_Env*nv,struct _RegionHandle*
rgn,void*t,struct Cyc_Absyn_Exp*r,struct Cyc_Absyn_Exp*path,struct Cyc_Absyn_Pat*p,
struct Cyc_Absyn_Stmt**tag_fail_stmt,struct Cyc_Absyn_Stmt*fail_stmt,struct Cyc_List_List*
decls);static struct _tuple13 Cyc_Toc_xlate_pat(struct Cyc_Toc_Env*nv,struct
_RegionHandle*rgn,void*t,struct Cyc_Absyn_Exp*r,struct Cyc_Absyn_Exp*path,struct
Cyc_Absyn_Pat*p,struct Cyc_Absyn_Stmt**tag_fail_stmt,struct Cyc_Absyn_Stmt*
fail_stmt,struct Cyc_List_List*decls){struct Cyc_Absyn_Stmt*s;{void*_tmp658=(void*)
p->r;struct Cyc_Absyn_Vardecl*_tmp659;struct Cyc_Absyn_Vardecl _tmp65A;struct
_tuple1*_tmp65B;struct Cyc_Absyn_Pat*_tmp65C;struct Cyc_Absyn_Vardecl*_tmp65D;
struct Cyc_Absyn_Vardecl _tmp65E;struct _tuple1*_tmp65F;struct Cyc_Absyn_Vardecl*
_tmp660;struct Cyc_Absyn_Pat*_tmp661;void*_tmp662;int _tmp663;char _tmp664;struct
_dyneither_ptr _tmp665;struct Cyc_Absyn_Enumdecl*_tmp666;struct Cyc_Absyn_Enumfield*
_tmp667;void*_tmp668;struct Cyc_Absyn_Enumfield*_tmp669;struct Cyc_Absyn_Datatypedecl*
_tmp66A;struct Cyc_Absyn_Datatypefield*_tmp66B;struct Cyc_List_List*_tmp66C;struct
Cyc_Absyn_Pat*_tmp66D;struct Cyc_Absyn_Pat _tmp66E;void*_tmp66F;struct Cyc_Absyn_Datatypedecl*
_tmp670;struct Cyc_Absyn_Datatypefield*_tmp671;struct Cyc_List_List*_tmp672;struct
Cyc_Absyn_Datatypedecl*_tmp673;struct Cyc_Absyn_Datatypefield*_tmp674;struct Cyc_List_List*
_tmp675;struct Cyc_List_List*_tmp676;struct Cyc_List_List*_tmp677;struct Cyc_Absyn_AggrInfo
_tmp678;union Cyc_Absyn_AggrInfoU_union _tmp679;struct Cyc_List_List*_tmp67A;struct
Cyc_Absyn_Pat*_tmp67B;_LL33A: if(_tmp658 <= (void*)2)goto _LL33E;if(*((int*)_tmp658)
!= 0)goto _LL33C;_tmp659=((struct Cyc_Absyn_Var_p_struct*)_tmp658)->f1;_tmp65A=*
_tmp659;_tmp65B=_tmp65A.name;_tmp65C=((struct Cyc_Absyn_Var_p_struct*)_tmp658)->f2;
_LL33B: return Cyc_Toc_xlate_pat(Cyc_Toc_add_varmap(rgn,nv,_tmp65B,r),rgn,t,r,path,
_tmp65C,tag_fail_stmt,fail_stmt,decls);_LL33C: if(*((int*)_tmp658)!= 2)goto _LL33E;
_tmp65D=((struct Cyc_Absyn_TagInt_p_struct*)_tmp658)->f2;_tmp65E=*_tmp65D;_tmp65F=
_tmp65E.name;_LL33D: nv=Cyc_Toc_add_varmap(rgn,nv,_tmp65F,r);goto _LL33F;_LL33E:
if((int)_tmp658 != 0)goto _LL340;_LL33F: s=Cyc_Toc_skip_stmt_dl();goto _LL339;_LL340:
if(_tmp658 <= (void*)2)goto _LL342;if(*((int*)_tmp658)!= 1)goto _LL342;_tmp660=((
struct Cyc_Absyn_Reference_p_struct*)_tmp658)->f1;_tmp661=((struct Cyc_Absyn_Reference_p_struct*)
_tmp658)->f2;_LL341: {struct _tuple1*_tmp67C=Cyc_Toc_temp_var();{struct _tuple14*
_tmpBB5;struct Cyc_List_List*_tmpBB4;decls=((_tmpBB4=_region_malloc(rgn,sizeof(*
_tmpBB4)),((_tmpBB4->hd=((_tmpBB5=_region_malloc(rgn,sizeof(*_tmpBB5)),((_tmpBB5->f1=
_tmp67C,((_tmpBB5->f2=Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c(t),Cyc_Toc_mt_tq),
_tmpBB5)))))),((_tmpBB4->tl=decls,_tmpBB4))))));}nv=Cyc_Toc_add_varmap(rgn,nv,
_tmp660->name,Cyc_Absyn_var_exp(_tmp67C,0));s=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(
_tmp67C,0),Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c(t),Cyc_Toc_mt_tq),
Cyc_Absyn_address_exp(path,0)),0);{struct _tuple13 _tmp67F=Cyc_Toc_xlate_pat(nv,
rgn,t,r,path,_tmp661,tag_fail_stmt,fail_stmt,decls);_tmp67F.f3=Cyc_Absyn_seq_stmt(
s,_tmp67F.f3,0);return _tmp67F;}}_LL342: if((int)_tmp658 != 1)goto _LL344;_LL343: s=
Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_signed_int_exp(0,0),fail_stmt);goto _LL339;_LL344:
if(_tmp658 <= (void*)2)goto _LL346;if(*((int*)_tmp658)!= 7)goto _LL346;_tmp662=(
void*)((struct Cyc_Absyn_Int_p_struct*)_tmp658)->f1;_tmp663=((struct Cyc_Absyn_Int_p_struct*)
_tmp658)->f2;_LL345: s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_int_exp(_tmp662,_tmp663,0),
fail_stmt);goto _LL339;_LL346: if(_tmp658 <= (void*)2)goto _LL348;if(*((int*)_tmp658)
!= 8)goto _LL348;_tmp664=((struct Cyc_Absyn_Char_p_struct*)_tmp658)->f1;_LL347: s=
Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_char_exp(_tmp664,0),fail_stmt);goto _LL339;_LL348:
if(_tmp658 <= (void*)2)goto _LL34A;if(*((int*)_tmp658)!= 9)goto _LL34A;_tmp665=((
struct Cyc_Absyn_Float_p_struct*)_tmp658)->f1;_LL349: s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_float_exp(
_tmp665,0),fail_stmt);goto _LL339;_LL34A: if(_tmp658 <= (void*)2)goto _LL34C;if(*((
int*)_tmp658)!= 10)goto _LL34C;_tmp666=((struct Cyc_Absyn_Enum_p_struct*)_tmp658)->f1;
_tmp667=((struct Cyc_Absyn_Enum_p_struct*)_tmp658)->f2;_LL34B:{struct Cyc_Absyn_Enum_e_struct
_tmpBB8;struct Cyc_Absyn_Enum_e_struct*_tmpBB7;s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_new_exp((
void*)((_tmpBB7=_cycalloc(sizeof(*_tmpBB7)),((_tmpBB7[0]=((_tmpBB8.tag=33,((
_tmpBB8.f1=_tmp667->name,((_tmpBB8.f2=(struct Cyc_Absyn_Enumdecl*)_tmp666,((
_tmpBB8.f3=(struct Cyc_Absyn_Enumfield*)_tmp667,_tmpBB8)))))))),_tmpBB7)))),0),
fail_stmt);}goto _LL339;_LL34C: if(_tmp658 <= (void*)2)goto _LL34E;if(*((int*)
_tmp658)!= 11)goto _LL34E;_tmp668=(void*)((struct Cyc_Absyn_AnonEnum_p_struct*)
_tmp658)->f1;_tmp669=((struct Cyc_Absyn_AnonEnum_p_struct*)_tmp658)->f2;_LL34D:{
struct Cyc_Absyn_AnonEnum_e_struct _tmpBBB;struct Cyc_Absyn_AnonEnum_e_struct*
_tmpBBA;s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_new_exp((void*)((_tmpBBA=_cycalloc(
sizeof(*_tmpBBA)),((_tmpBBA[0]=((_tmpBBB.tag=34,((_tmpBBB.f1=_tmp669->name,((
_tmpBBB.f2=(void*)_tmp668,((_tmpBBB.f3=(struct Cyc_Absyn_Enumfield*)_tmp669,
_tmpBBB)))))))),_tmpBBA)))),0),fail_stmt);}goto _LL339;_LL34E: if(_tmp658 <= (void*)
2)goto _LL350;if(*((int*)_tmp658)!= 6)goto _LL350;_tmp66A=((struct Cyc_Absyn_Datatype_p_struct*)
_tmp658)->f1;_tmp66B=((struct Cyc_Absyn_Datatype_p_struct*)_tmp658)->f2;_tmp66C=((
struct Cyc_Absyn_Datatype_p_struct*)_tmp658)->f3;if(_tmp66C != 0)goto _LL350;if(!(!
_tmp66A->is_flat))goto _LL350;_LL34F: {struct Cyc_Absyn_Exp*cmp_exp;if(_tmp66A->is_extensible)
cmp_exp=Cyc_Absyn_var_exp(_tmp66B->name,0);else{cmp_exp=Cyc_Toc_datatype_tag(
_tmp66A,_tmp66B->name,0);r=Cyc_Toc_cast_it(Cyc_Absyn_sint_typ,r);}s=Cyc_Toc_if_neq_stmt(
r,cmp_exp,fail_stmt);goto _LL339;}_LL350: if(_tmp658 <= (void*)2)goto _LL352;if(*((
int*)_tmp658)!= 4)goto _LL352;_tmp66D=((struct Cyc_Absyn_Pointer_p_struct*)_tmp658)->f1;
_tmp66E=*_tmp66D;_tmp66F=(void*)_tmp66E.r;if(_tmp66F <= (void*)2)goto _LL352;if(*((
int*)_tmp66F)!= 6)goto _LL352;_tmp670=((struct Cyc_Absyn_Datatype_p_struct*)
_tmp66F)->f1;_tmp671=((struct Cyc_Absyn_Datatype_p_struct*)_tmp66F)->f2;_tmp672=((
struct Cyc_Absyn_Datatype_p_struct*)_tmp66F)->f3;if(!(_tmp672 != 0  && !_tmp670->is_flat))
goto _LL352;_LL351: s=Cyc_Toc_skip_stmt_dl();{int cnt=1;const char*_tmpBBC;struct
_tuple1*tufstrct=Cyc_Toc_collapse_qvar_tag(_tmp671->name,((_tmpBBC="_struct",
_tag_dyneither(_tmpBBC,sizeof(char),8))));struct Cyc_Absyn_Exp*rcast=Cyc_Toc_cast_it(
Cyc_Absyn_cstar_typ(Cyc_Absyn_strctq(tufstrct),Cyc_Toc_mt_tq),r);struct Cyc_List_List*
_tmp684=_tmp671->typs;for(0;_tmp672 != 0;(((_tmp672=_tmp672->tl,_tmp684=((struct
Cyc_List_List*)_check_null(_tmp684))->tl)),++ cnt)){struct Cyc_Absyn_Pat*_tmp685=(
struct Cyc_Absyn_Pat*)_tmp672->hd;if((void*)_tmp685->r == (void*)0)continue;{void*
_tmp686=(*((struct _tuple4*)((struct Cyc_List_List*)_check_null(_tmp684))->hd)).f2;
struct _tuple1*_tmp687=Cyc_Toc_temp_var();void*_tmp688=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp685->topt))->v;void*_tmp689=Cyc_Toc_typ_to_c(_tmp688);struct Cyc_Absyn_Exp*
_tmp68A=Cyc_Absyn_aggrarrow_exp(rcast,Cyc_Absyn_fieldname(cnt),0);if(Cyc_Toc_is_void_star(
Cyc_Toc_typ_to_c(_tmp686)))_tmp68A=Cyc_Toc_cast_it(_tmp689,_tmp68A);{struct
_tuple14*_tmpBBF;struct Cyc_List_List*_tmpBBE;decls=((_tmpBBE=_region_malloc(rgn,
sizeof(*_tmpBBE)),((_tmpBBE->hd=((_tmpBBF=_region_malloc(rgn,sizeof(*_tmpBBF)),((
_tmpBBF->f1=_tmp687,((_tmpBBF->f2=_tmp689,_tmpBBF)))))),((_tmpBBE->tl=decls,
_tmpBBE))))));}{struct _tuple13 _tmp68D=Cyc_Toc_xlate_pat(nv,rgn,_tmp688,Cyc_Absyn_var_exp(
_tmp687,0),_tmp68A,_tmp685,(struct Cyc_Absyn_Stmt**)& fail_stmt,fail_stmt,decls);
nv=_tmp68D.f1;decls=_tmp68D.f2;{struct Cyc_Absyn_Stmt*_tmp68E=_tmp68D.f3;struct
Cyc_Absyn_Stmt*_tmp68F=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp687,0),
_tmp68A,0);s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_seq_stmt(_tmp68F,_tmp68E,0),0);}}}}{
struct Cyc_Absyn_Exp*test_exp;if(_tmp670->is_extensible){struct Cyc_Absyn_Exp*e2=
Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(Cyc_Absyn_void_star_typ(),Cyc_Toc_mt_tq),r);
struct Cyc_Absyn_Exp*e1=Cyc_Absyn_deref_exp(e2,0);struct Cyc_Absyn_Exp*e=Cyc_Absyn_var_exp(
_tmp671->name,0);test_exp=Cyc_Absyn_neq_exp(e1,e,0);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(
test_exp,fail_stmt,Cyc_Toc_skip_stmt_dl(),0),s,0);}else{struct Cyc_Absyn_Exp*e3=
Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(Cyc_Absyn_sint_typ,Cyc_Toc_mt_tq),r);struct
Cyc_Absyn_Exp*e1=Cyc_Absyn_deref_exp(e3,0);struct Cyc_Absyn_Exp*e=Cyc_Toc_datatype_tag(
_tmp670,_tmp671->name,1);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_neq_exp(
e1,e,0),fail_stmt,Cyc_Toc_skip_stmt_dl(),0),s,0);if(tag_fail_stmt != 0){int
max_tag=Cyc_Toc_num_void_tags(_tmp670);if(max_tag != 0){struct Cyc_Absyn_Exp*
max_tag_exp=Cyc_Absyn_uint_exp((unsigned int)max_tag,0);struct Cyc_Absyn_Exp*e5=
Cyc_Absyn_lte_exp(r,Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),max_tag_exp),0);s=
Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(e5,*tag_fail_stmt,Cyc_Toc_skip_stmt_dl(),
0),s,0);}}}goto _LL339;}}_LL352: if(_tmp658 <= (void*)2)goto _LL354;if(*((int*)
_tmp658)!= 6)goto _LL354;_tmp673=((struct Cyc_Absyn_Datatype_p_struct*)_tmp658)->f1;
_tmp674=((struct Cyc_Absyn_Datatype_p_struct*)_tmp658)->f2;_tmp675=((struct Cyc_Absyn_Datatype_p_struct*)
_tmp658)->f3;if(!_tmp673->is_flat)goto _LL354;_LL353: {struct _tuple1 _tmp692;
struct _dyneither_ptr*_tmp693;struct _tuple1*_tmp691=_tmp674->name;_tmp692=*
_tmp691;_tmp693=_tmp692.f2;r=Cyc_Absyn_aggrmember_exp(r,_tmp693,0);path=Cyc_Absyn_aggrmember_exp(
path,_tmp693,0);s=Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_neq_exp(Cyc_Absyn_aggrmember_exp(
path,Cyc_Toc_tag_sp,0),Cyc_Toc_datatype_tag(_tmp673,_tmp674->name,1),0),
fail_stmt,Cyc_Toc_skip_stmt_dl(),0);{int cnt=1;for(0;_tmp675 != 0;(_tmp675=_tmp675->tl,
++ cnt)){struct Cyc_Absyn_Pat*_tmp694=(struct Cyc_Absyn_Pat*)_tmp675->hd;if((void*)
_tmp694->r == (void*)0)continue;{struct _tuple1*_tmp695=Cyc_Toc_temp_var();void*
_tmp696=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp694->topt))->v;{struct
_tuple14*_tmpBC2;struct Cyc_List_List*_tmpBC1;decls=((_tmpBC1=_region_malloc(rgn,
sizeof(*_tmpBC1)),((_tmpBC1->hd=((_tmpBC2=_region_malloc(rgn,sizeof(*_tmpBC2)),((
_tmpBC2->f1=_tmp695,((_tmpBC2->f2=Cyc_Toc_typ_to_c(_tmp696),_tmpBC2)))))),((
_tmpBC1->tl=decls,_tmpBC1))))));}{struct _tuple13 _tmp699=Cyc_Toc_xlate_pat(nv,rgn,
_tmp696,Cyc_Absyn_var_exp(_tmp695,0),Cyc_Absyn_aggrmember_exp(path,Cyc_Absyn_fieldname(
cnt),0),_tmp694,(struct Cyc_Absyn_Stmt**)& fail_stmt,fail_stmt,decls);nv=_tmp699.f1;
decls=_tmp699.f2;{struct Cyc_Absyn_Stmt*_tmp69A=_tmp699.f3;struct Cyc_Absyn_Stmt*
_tmp69B=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp695,0),Cyc_Absyn_aggrmember_exp(
r,Cyc_Absyn_fieldname(cnt),0),0);s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_seq_stmt(
_tmp69B,_tmp69A,0),0);}}}}goto _LL339;}}_LL354: if(_tmp658 <= (void*)2)goto _LL356;
if(*((int*)_tmp658)!= 6)goto _LL356;_tmp676=((struct Cyc_Absyn_Datatype_p_struct*)
_tmp658)->f3;_LL355: _tmp677=_tmp676;goto _LL357;_LL356: if(_tmp658 <= (void*)2)goto
_LL358;if(*((int*)_tmp658)!= 3)goto _LL358;_tmp677=((struct Cyc_Absyn_Tuple_p_struct*)
_tmp658)->f1;_LL357: s=Cyc_Toc_skip_stmt_dl();{int cnt=1;for(0;_tmp677 != 0;(
_tmp677=_tmp677->tl,++ cnt)){struct Cyc_Absyn_Pat*_tmp69C=(struct Cyc_Absyn_Pat*)
_tmp677->hd;if((void*)_tmp69C->r == (void*)0)continue;{struct _tuple1*_tmp69D=Cyc_Toc_temp_var();
void*_tmp69E=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp69C->topt))->v;{struct
_tuple14*_tmpBC5;struct Cyc_List_List*_tmpBC4;decls=((_tmpBC4=_region_malloc(rgn,
sizeof(*_tmpBC4)),((_tmpBC4->hd=((_tmpBC5=_region_malloc(rgn,sizeof(*_tmpBC5)),((
_tmpBC5->f1=_tmp69D,((_tmpBC5->f2=Cyc_Toc_typ_to_c(_tmp69E),_tmpBC5)))))),((
_tmpBC4->tl=decls,_tmpBC4))))));}{struct _tuple13 _tmp6A1=Cyc_Toc_xlate_pat(nv,rgn,
_tmp69E,Cyc_Absyn_var_exp(_tmp69D,0),Cyc_Absyn_aggrmember_exp(path,Cyc_Absyn_fieldname(
cnt),0),_tmp69C,(struct Cyc_Absyn_Stmt**)& fail_stmt,fail_stmt,decls);nv=_tmp6A1.f1;
decls=_tmp6A1.f2;{struct Cyc_Absyn_Stmt*_tmp6A2=_tmp6A1.f3;struct Cyc_Absyn_Stmt*
_tmp6A3=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp69D,0),Cyc_Absyn_aggrmember_exp(
r,Cyc_Absyn_fieldname(cnt),0),0);s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_seq_stmt(
_tmp6A3,_tmp6A2,0),0);}}}}goto _LL339;}_LL358: if(_tmp658 <= (void*)2)goto _LL35A;
if(*((int*)_tmp658)!= 5)goto _LL35A;_tmp678=((struct Cyc_Absyn_Aggr_p_struct*)
_tmp658)->f1;_tmp679=_tmp678.aggr_info;_tmp67A=((struct Cyc_Absyn_Aggr_p_struct*)
_tmp658)->f3;_LL359: {struct Cyc_Absyn_Aggrdecl*_tmp6A4=Cyc_Absyn_get_known_aggrdecl(
_tmp679);s=Cyc_Toc_skip_stmt_dl();for(0;_tmp67A != 0;_tmp67A=_tmp67A->tl){struct
_tuple15*_tmp6A5=(struct _tuple15*)_tmp67A->hd;struct Cyc_Absyn_Pat*_tmp6A6=(*
_tmp6A5).f2;if((void*)_tmp6A6->r == (void*)0)continue;{struct _dyneither_ptr*f;{
void*_tmp6A7=(void*)((struct Cyc_List_List*)_check_null((*_tmp6A5).f1))->hd;
struct _dyneither_ptr*_tmp6A8;_LL363: if(*((int*)_tmp6A7)!= 1)goto _LL365;_tmp6A8=((
struct Cyc_Absyn_FieldName_struct*)_tmp6A7)->f1;_LL364: f=_tmp6A8;goto _LL362;
_LL365:;_LL366:(int)_throw((void*)Cyc_Toc_Match_error);_LL362:;}{struct _tuple1*
_tmp6A9=Cyc_Toc_temp_var();void*_tmp6AA=(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp6A6->topt))->v;void*_tmp6AB=Cyc_Toc_typ_to_c(_tmp6AA);{struct _tuple14*
_tmpBC8;struct Cyc_List_List*_tmpBC7;decls=((_tmpBC7=_region_malloc(rgn,sizeof(*
_tmpBC7)),((_tmpBC7->hd=((_tmpBC8=_region_malloc(rgn,sizeof(*_tmpBC8)),((_tmpBC8->f1=
_tmp6A9,((_tmpBC8->f2=_tmp6AB,_tmpBC8)))))),((_tmpBC7->tl=decls,_tmpBC7))))));}{
struct _tuple13 _tmp6AE=Cyc_Toc_xlate_pat(nv,rgn,_tmp6AA,Cyc_Absyn_var_exp(_tmp6A9,
0),Cyc_Absyn_aggrmember_exp(path,f,0),_tmp6A6,(struct Cyc_Absyn_Stmt**)& fail_stmt,
fail_stmt,decls);nv=_tmp6AE.f1;decls=_tmp6AE.f2;{struct Cyc_Absyn_Exp*_tmp6AF=Cyc_Absyn_aggrmember_exp(
r,f,0);if(Cyc_Toc_is_void_star((void*)((struct Cyc_Absyn_Aggrfield*)_check_null(
Cyc_Absyn_lookup_field(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp6A4->impl))->fields,
f)))->type))_tmp6AF=Cyc_Toc_cast_it(_tmp6AB,_tmp6AF);{struct Cyc_Absyn_Stmt*
_tmp6B0=_tmp6AE.f3;struct Cyc_Absyn_Stmt*_tmp6B1=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(
_tmp6A9,0),_tmp6AF,0);s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_seq_stmt(_tmp6B1,_tmp6B0,
0),0);}}}}}}goto _LL339;}_LL35A: if(_tmp658 <= (void*)2)goto _LL35C;if(*((int*)
_tmp658)!= 4)goto _LL35C;_tmp67B=((struct Cyc_Absyn_Pointer_p_struct*)_tmp658)->f1;
_LL35B: {struct _tuple1*_tmp6B2=Cyc_Toc_temp_var();void*_tmp6B3=(void*)((struct
Cyc_Core_Opt*)_check_null(_tmp67B->topt))->v;{struct _tuple14*_tmpBCB;struct Cyc_List_List*
_tmpBCA;decls=((_tmpBCA=_region_malloc(rgn,sizeof(*_tmpBCA)),((_tmpBCA->hd=((
_tmpBCB=_region_malloc(rgn,sizeof(*_tmpBCB)),((_tmpBCB->f1=_tmp6B2,((_tmpBCB->f2=
Cyc_Toc_typ_to_c(_tmp6B3),_tmpBCB)))))),((_tmpBCA->tl=decls,_tmpBCA))))));}{
struct _tuple13 _tmp6B6=Cyc_Toc_xlate_pat(nv,rgn,_tmp6B3,Cyc_Absyn_var_exp(_tmp6B2,
0),Cyc_Absyn_deref_exp(path,0),_tmp67B,(struct Cyc_Absyn_Stmt**)& fail_stmt,
fail_stmt,decls);nv=_tmp6B6.f1;decls=_tmp6B6.f2;{struct Cyc_Absyn_Stmt*_tmp6B7=
_tmp6B6.f3;struct Cyc_Absyn_Stmt*_tmp6B8=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp(_tmp6B2,0),Cyc_Absyn_deref_exp(r,0),0),_tmp6B7,0);if(Cyc_Toc_is_nullable(
t))s=Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_eq_exp(r,Cyc_Absyn_signed_int_exp(
0,0),0),fail_stmt,Cyc_Toc_skip_stmt_dl(),0),_tmp6B8,0);else{s=_tmp6B8;}goto
_LL339;}}}_LL35C: if(_tmp658 <= (void*)2)goto _LL35E;if(*((int*)_tmp658)!= 12)goto
_LL35E;_LL35D: {const char*_tmpBCE;void*_tmpBCD;(_tmpBCD=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBCE="unknownid pat",
_tag_dyneither(_tmpBCE,sizeof(char),14))),_tag_dyneither(_tmpBCD,sizeof(void*),0)));}
_LL35E: if(_tmp658 <= (void*)2)goto _LL360;if(*((int*)_tmp658)!= 13)goto _LL360;
_LL35F: {const char*_tmpBD1;void*_tmpBD0;(_tmpBD0=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBD1="unknowncall pat",
_tag_dyneither(_tmpBD1,sizeof(char),16))),_tag_dyneither(_tmpBD0,sizeof(void*),0)));}
_LL360: if(_tmp658 <= (void*)2)goto _LL339;if(*((int*)_tmp658)!= 14)goto _LL339;
_LL361: {const char*_tmpBD4;void*_tmpBD3;(_tmpBD3=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBD4="exp pat",
_tag_dyneither(_tmpBD4,sizeof(char),8))),_tag_dyneither(_tmpBD3,sizeof(void*),0)));}
_LL339:;}{struct _tuple13 _tmpBD5;return(_tmpBD5.f1=nv,((_tmpBD5.f2=decls,((
_tmpBD5.f3=s,_tmpBD5)))));}}struct _tuple16{struct _dyneither_ptr*f1;struct
_dyneither_ptr*f2;struct Cyc_Absyn_Switch_clause*f3;};static struct _tuple16*Cyc_Toc_gen_label(
struct _RegionHandle*r,struct Cyc_Absyn_Switch_clause*sc);static struct _tuple16*Cyc_Toc_gen_label(
struct _RegionHandle*r,struct Cyc_Absyn_Switch_clause*sc){struct _tuple16*_tmpBD6;
return(_tmpBD6=_region_malloc(r,sizeof(*_tmpBD6)),((_tmpBD6->f1=Cyc_Toc_fresh_label(),((
_tmpBD6->f2=Cyc_Toc_fresh_label(),((_tmpBD6->f3=sc,_tmpBD6)))))));}static int Cyc_Toc_is_mixed_datatype(
void*t);static int Cyc_Toc_is_mixed_datatype(void*t){{void*_tmp6C1=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_DatatypeInfo _tmp6C2;union Cyc_Absyn_DatatypeInfoU_union _tmp6C3;
struct Cyc_Absyn_Datatypedecl**_tmp6C4;struct Cyc_Absyn_Datatypedecl*_tmp6C5;
_LL368: if(_tmp6C1 <= (void*)4)goto _LL36A;if(*((int*)_tmp6C1)!= 2)goto _LL36A;
_tmp6C2=((struct Cyc_Absyn_DatatypeType_struct*)_tmp6C1)->f1;_tmp6C3=_tmp6C2.datatype_info;
if((((((struct Cyc_Absyn_DatatypeType_struct*)_tmp6C1)->f1).datatype_info).KnownDatatype).tag
!= 1)goto _LL36A;_tmp6C4=(_tmp6C3.KnownDatatype).f1;_tmp6C5=*_tmp6C4;if(!(!
_tmp6C5->is_flat))goto _LL36A;_LL369: {int seen_novalue=0;int seen_value=0;{struct
Cyc_List_List*_tmp6C6=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(
_tmp6C5->fields))->v;for(0;(unsigned int)_tmp6C6;_tmp6C6=_tmp6C6->tl){if(((
struct Cyc_Absyn_Datatypefield*)_tmp6C6->hd)->typs == 0)seen_value=1;else{
seen_novalue=1;}if(seen_value  && seen_novalue)return 1;}}goto _LL367;}_LL36A:;
_LL36B: goto _LL367;_LL367:;}return 0;}static int Cyc_Toc_no_tag_test(struct Cyc_Absyn_Pat*
p);static int Cyc_Toc_no_tag_test(struct Cyc_Absyn_Pat*p){void*_tmp6C7=(void*)p->r;
struct Cyc_Absyn_Pat*_tmp6C8;struct Cyc_Absyn_Pat*_tmp6C9;struct Cyc_Absyn_Pat
_tmp6CA;void*_tmp6CB;struct Cyc_Absyn_Datatypedecl*_tmp6CC;struct Cyc_Absyn_Datatypefield*
_tmp6CD;struct Cyc_List_List*_tmp6CE;_LL36D: if(_tmp6C7 <= (void*)2)goto _LL371;if(*((
int*)_tmp6C7)!= 0)goto _LL36F;_tmp6C8=((struct Cyc_Absyn_Var_p_struct*)_tmp6C7)->f2;
_LL36E: return Cyc_Toc_no_tag_test(_tmp6C8);_LL36F: if(*((int*)_tmp6C7)!= 4)goto
_LL371;_tmp6C9=((struct Cyc_Absyn_Pointer_p_struct*)_tmp6C7)->f1;_tmp6CA=*_tmp6C9;
_tmp6CB=(void*)_tmp6CA.r;if(_tmp6CB <= (void*)2)goto _LL371;if(*((int*)_tmp6CB)!= 
6)goto _LL371;_tmp6CC=((struct Cyc_Absyn_Datatype_p_struct*)_tmp6CB)->f1;_tmp6CD=((
struct Cyc_Absyn_Datatype_p_struct*)_tmp6CB)->f2;_tmp6CE=((struct Cyc_Absyn_Datatype_p_struct*)
_tmp6CB)->f3;if(!(_tmp6CE != 0  && !_tmp6CC->is_flat))goto _LL371;_LL370: return 0;
_LL371:;_LL372: return 1;_LL36C:;}static void Cyc_Toc_xlate_switch(struct Cyc_Toc_Env*
nv,struct Cyc_Absyn_Stmt*whole_s,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*scs);
static void Cyc_Toc_xlate_switch(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*whole_s,
struct Cyc_Absyn_Exp*e,struct Cyc_List_List*scs){Cyc_Toc_exp_to_c(nv,e);{void*
_tmp6CF=(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;int leave_as_switch;{
void*_tmp6D0=Cyc_Tcutil_compress(_tmp6CF);_LL374: if(_tmp6D0 <= (void*)4)goto
_LL378;if(*((int*)_tmp6D0)!= 5)goto _LL376;_LL375: goto _LL377;_LL376: if(*((int*)
_tmp6D0)!= 12)goto _LL378;_LL377: leave_as_switch=1;goto _LL373;_LL378:;_LL379:
leave_as_switch=0;goto _LL373;_LL373:;}{struct Cyc_List_List*_tmp6D1=scs;for(0;
_tmp6D1 != 0;_tmp6D1=_tmp6D1->tl){if((struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(((struct Cyc_Absyn_Switch_clause*)_tmp6D1->hd)->pat_vars))->v != 0  || ((
struct Cyc_Absyn_Switch_clause*)_tmp6D1->hd)->where_clause != 0){leave_as_switch=0;
break;}}}if(leave_as_switch){struct _dyneither_ptr*next_l=Cyc_Toc_fresh_label();{
struct Cyc_List_List*_tmp6D2=scs;for(0;_tmp6D2 != 0;_tmp6D2=_tmp6D2->tl){struct Cyc_Absyn_Stmt*
_tmp6D3=((struct Cyc_Absyn_Switch_clause*)_tmp6D2->hd)->body;((struct Cyc_Absyn_Switch_clause*)
_tmp6D2->hd)->body=Cyc_Absyn_label_stmt(next_l,_tmp6D3,0);next_l=Cyc_Toc_fresh_label();{
struct Cyc_Toc_Env _tmp6D5;struct _RegionHandle*_tmp6D6;struct Cyc_Toc_Env*_tmp6D4=
nv;_tmp6D5=*_tmp6D4;_tmp6D6=_tmp6D5.rgn;Cyc_Toc_stmt_to_c(Cyc_Toc_switch_as_switch_env(
_tmp6D6,nv,next_l),_tmp6D3);}}}return;}{struct _tuple1*v=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*r=Cyc_Absyn_var_exp(v,0);struct Cyc_Absyn_Exp*path=Cyc_Absyn_var_exp(
v,0);struct _dyneither_ptr*end_l=Cyc_Toc_fresh_label();struct Cyc_Toc_Env _tmp6D8;
struct _RegionHandle*_tmp6D9;struct Cyc_Toc_Env*_tmp6D7=nv;_tmp6D8=*_tmp6D7;
_tmp6D9=_tmp6D8.rgn;{struct Cyc_Toc_Env*_tmp6DA=Cyc_Toc_share_env(_tmp6D9,nv);
struct Cyc_List_List*lscs=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct
_tuple16*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct
_RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(_tmp6D9,Cyc_Toc_gen_label,
_tmp6D9,scs);struct Cyc_List_List*test_stmts=0;struct Cyc_List_List*nvs=0;struct
Cyc_List_List*decls=0;int is_datatype=Cyc_Toc_is_mixed_datatype(_tmp6CF);int
needs_tag_test=is_datatype;{struct Cyc_List_List*_tmp6DB=lscs;for(0;_tmp6DB != 0;
_tmp6DB=_tmp6DB->tl){struct Cyc_Absyn_Switch_clause*sc=(*((struct _tuple16*)
_tmp6DB->hd)).f3;struct _dyneither_ptr*fail_lab=_tmp6DB->tl == 0?end_l:(*((struct
_tuple16*)((struct Cyc_List_List*)_check_null(_tmp6DB->tl))->hd)).f1;struct Cyc_Absyn_Stmt**
tag_fail_stmt=0;if(needs_tag_test  && !Cyc_Toc_no_tag_test(sc->pattern)){{struct
Cyc_List_List*_tmp6DC=_tmp6DB->tl;for(0;(unsigned int)_tmp6DC;_tmp6DC=_tmp6DC->tl){
if(Cyc_Toc_no_tag_test(((*((struct _tuple16*)_tmp6DC->hd)).f3)->pattern)){{struct
Cyc_Absyn_Stmt**_tmpBD7;tag_fail_stmt=((_tmpBD7=_region_malloc(_tmp6D9,sizeof(*
_tmpBD7)),((_tmpBD7[0]=Cyc_Absyn_goto_stmt((*((struct _tuple16*)_tmp6DC->hd)).f1,
0),_tmpBD7))));}needs_tag_test=0;break;}}}if(tag_fail_stmt == 0){struct Cyc_Absyn_Stmt**
_tmpBD8;tag_fail_stmt=((_tmpBD8=_region_malloc(_tmp6D9,sizeof(*_tmpBD8)),((
_tmpBD8[0]=Cyc_Absyn_goto_stmt(fail_lab,0),_tmpBD8))));}}{struct Cyc_Toc_Env*
_tmp6E0;struct Cyc_List_List*_tmp6E1;struct Cyc_Absyn_Stmt*_tmp6E2;struct _tuple13
_tmp6DF=Cyc_Toc_xlate_pat(_tmp6DA,_tmp6D9,_tmp6CF,r,path,sc->pattern,
tag_fail_stmt,Cyc_Absyn_goto_stmt(fail_lab,0),decls);_tmp6E0=_tmp6DF.f1;_tmp6E1=
_tmp6DF.f2;_tmp6E2=_tmp6DF.f3;if(is_datatype  && Cyc_Toc_no_tag_test(sc->pattern))
needs_tag_test=1;if(sc->where_clause != 0){struct Cyc_Absyn_Exp*_tmp6E3=(struct Cyc_Absyn_Exp*)
_check_null(sc->where_clause);Cyc_Toc_exp_to_c(_tmp6E0,_tmp6E3);_tmp6E2=Cyc_Absyn_seq_stmt(
_tmp6E2,Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_prim1_exp((void*)11,_tmp6E3,0),Cyc_Absyn_goto_stmt(
fail_lab,0),Cyc_Toc_skip_stmt_dl(),0),0);}decls=_tmp6E1;{struct Cyc_List_List*
_tmpBD9;nvs=((_tmpBD9=_region_malloc(_tmp6D9,sizeof(*_tmpBD9)),((_tmpBD9->hd=
_tmp6E0,((_tmpBD9->tl=nvs,_tmpBD9))))));}{struct Cyc_List_List*_tmpBDA;test_stmts=((
_tmpBDA=_region_malloc(_tmp6D9,sizeof(*_tmpBDA)),((_tmpBDA->hd=_tmp6E2,((_tmpBDA->tl=
test_stmts,_tmpBDA))))));}}}}nvs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(nvs);test_stmts=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(test_stmts);{struct Cyc_List_List*stmts=0;for(0;lscs != 0;(((
lscs=lscs->tl,nvs=nvs->tl)),test_stmts=test_stmts->tl)){struct _tuple16 _tmp6E7;
struct _dyneither_ptr*_tmp6E8;struct _dyneither_ptr*_tmp6E9;struct Cyc_Absyn_Switch_clause*
_tmp6EA;struct _tuple16*_tmp6E6=(struct _tuple16*)lscs->hd;_tmp6E7=*_tmp6E6;
_tmp6E8=_tmp6E7.f1;_tmp6E9=_tmp6E7.f2;_tmp6EA=_tmp6E7.f3;{struct Cyc_Toc_Env*
_tmp6EB=(struct Cyc_Toc_Env*)((struct Cyc_List_List*)_check_null(nvs))->hd;struct
Cyc_Absyn_Stmt*s=_tmp6EA->body;struct Cyc_Toc_Env _tmp6ED;struct _RegionHandle*
_tmp6EE;struct Cyc_Toc_Env*_tmp6EC=_tmp6DA;_tmp6ED=*_tmp6EC;_tmp6EE=_tmp6ED.rgn;
if(lscs->tl != 0){struct _tuple16 _tmp6F0;struct _dyneither_ptr*_tmp6F1;struct Cyc_Absyn_Switch_clause*
_tmp6F2;struct _tuple16*_tmp6EF=(struct _tuple16*)((struct Cyc_List_List*)
_check_null(lscs->tl))->hd;_tmp6F0=*_tmp6EF;_tmp6F1=_tmp6F0.f2;_tmp6F2=_tmp6F0.f3;
Cyc_Toc_stmt_to_c(Cyc_Toc_non_last_switchclause_env(_tmp6EE,_tmp6EB,end_l,
_tmp6F1,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp6F2->pat_vars))->v,(
struct Cyc_Toc_Env*)((struct Cyc_List_List*)_check_null(nvs->tl))->hd),s);}else{
Cyc_Toc_stmt_to_c(Cyc_Toc_last_switchclause_env(_tmp6EE,_tmp6EB,end_l),s);}s=Cyc_Absyn_seq_stmt(
Cyc_Absyn_label_stmt(_tmp6E8,(struct Cyc_Absyn_Stmt*)((struct Cyc_List_List*)
_check_null(test_stmts))->hd,0),Cyc_Absyn_label_stmt(_tmp6E9,s,0),0);{struct Cyc_List_List*
_tmpBDB;stmts=((_tmpBDB=_region_malloc(_tmp6D9,sizeof(*_tmpBDB)),((_tmpBDB->hd=s,((
_tmpBDB->tl=stmts,_tmpBDB))))));}}}{struct Cyc_Absyn_Stmt*res=Cyc_Absyn_seq_stmt(
Cyc_Absyn_seq_stmts(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
stmts),0),Cyc_Absyn_label_stmt(end_l,Cyc_Toc_skip_stmt_dl(),0),0);for(decls;
decls != 0;decls=decls->tl){struct _tuple14 _tmp6F5;struct _tuple1*_tmp6F6;void*
_tmp6F7;struct _tuple14*_tmp6F4=(struct _tuple14*)decls->hd;_tmp6F5=*_tmp6F4;
_tmp6F6=_tmp6F5.f1;_tmp6F7=_tmp6F5.f2;res=Cyc_Absyn_declare_stmt(_tmp6F6,_tmp6F7,
0,res,0);}(void*)(whole_s->r=(void*)((void*)(Cyc_Absyn_declare_stmt(v,Cyc_Toc_typ_to_c((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),(struct Cyc_Absyn_Exp*)e,
res,0))->r));}}}}}}static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*
nv,struct Cyc_Absyn_Pat*p,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s);
static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*body_nv,struct Cyc_Toc_Env*
init_nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*s);static void Cyc_Toc_fndecl_to_c(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int cinclude);struct Cyc_Absyn_Stmt*
Cyc_Toc_make_npop_handler(int n);struct Cyc_Absyn_Stmt*Cyc_Toc_make_npop_handler(
int n){struct Cyc_List_List*_tmpBDC;return Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(
Cyc_Toc__npop_handler_e,((_tmpBDC=_cycalloc(sizeof(*_tmpBDC)),((_tmpBDC->hd=Cyc_Absyn_uint_exp((
unsigned int)(n - 1),0),((_tmpBDC->tl=0,_tmpBDC)))))),0),0);}void Cyc_Toc_do_npop_before(
int n,struct Cyc_Absyn_Stmt*s);void Cyc_Toc_do_npop_before(int n,struct Cyc_Absyn_Stmt*
s){if(n > 0)(void*)(s->r=(void*)Cyc_Toc_seq_stmt_r(Cyc_Toc_make_npop_handler(n),
Cyc_Absyn_new_stmt((void*)s->r,0)));}static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*
nv,struct Cyc_Absyn_Stmt*s);static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,
struct Cyc_Absyn_Stmt*s){while(1){void*_tmp6F9=(void*)s->r;struct Cyc_Absyn_Exp*
_tmp6FA;struct Cyc_Absyn_Stmt*_tmp6FB;struct Cyc_Absyn_Stmt*_tmp6FC;struct Cyc_Absyn_Exp*
_tmp6FD;struct Cyc_Absyn_Exp*_tmp6FE;struct Cyc_Absyn_Stmt*_tmp6FF;struct Cyc_Absyn_Stmt*
_tmp700;struct _tuple3 _tmp701;struct Cyc_Absyn_Exp*_tmp702;struct Cyc_Absyn_Stmt*
_tmp703;struct Cyc_Absyn_Stmt*_tmp704;struct Cyc_Absyn_Stmt*_tmp705;struct Cyc_Absyn_Stmt*
_tmp706;struct Cyc_Absyn_Exp*_tmp707;struct _tuple3 _tmp708;struct Cyc_Absyn_Exp*
_tmp709;struct _tuple3 _tmp70A;struct Cyc_Absyn_Exp*_tmp70B;struct Cyc_Absyn_Stmt*
_tmp70C;struct Cyc_Absyn_Exp*_tmp70D;struct Cyc_List_List*_tmp70E;struct Cyc_List_List*
_tmp70F;struct Cyc_Absyn_Switch_clause**_tmp710;struct Cyc_Absyn_Decl*_tmp711;
struct Cyc_Absyn_Stmt*_tmp712;struct _dyneither_ptr*_tmp713;struct Cyc_Absyn_Stmt*
_tmp714;struct Cyc_Absyn_Stmt*_tmp715;struct _tuple3 _tmp716;struct Cyc_Absyn_Exp*
_tmp717;struct Cyc_Absyn_Stmt*_tmp718;struct Cyc_List_List*_tmp719;struct Cyc_Absyn_Exp*
_tmp71A;_LL37B: if((int)_tmp6F9 != 0)goto _LL37D;_LL37C: return;_LL37D: if(_tmp6F9 <= (
void*)1)goto _LL37F;if(*((int*)_tmp6F9)!= 0)goto _LL37F;_tmp6FA=((struct Cyc_Absyn_Exp_s_struct*)
_tmp6F9)->f1;_LL37E: Cyc_Toc_exp_to_c(nv,_tmp6FA);return;_LL37F: if(_tmp6F9 <= (
void*)1)goto _LL381;if(*((int*)_tmp6F9)!= 1)goto _LL381;_tmp6FB=((struct Cyc_Absyn_Seq_s_struct*)
_tmp6F9)->f1;_tmp6FC=((struct Cyc_Absyn_Seq_s_struct*)_tmp6F9)->f2;_LL380: Cyc_Toc_stmt_to_c(
nv,_tmp6FB);s=_tmp6FC;continue;_LL381: if(_tmp6F9 <= (void*)1)goto _LL383;if(*((int*)
_tmp6F9)!= 2)goto _LL383;_tmp6FD=((struct Cyc_Absyn_Return_s_struct*)_tmp6F9)->f1;
_LL382: {struct Cyc_Core_Opt*topt=0;if(_tmp6FD != 0){{struct Cyc_Core_Opt*_tmpBDD;
topt=((_tmpBDD=_cycalloc(sizeof(*_tmpBDD)),((_tmpBDD->v=(void*)Cyc_Toc_typ_to_c((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp6FD->topt))->v),_tmpBDD))));}Cyc_Toc_exp_to_c(
nv,(struct Cyc_Absyn_Exp*)_tmp6FD);}if(s->try_depth > 0){if(topt != 0){struct
_tuple1*_tmp71C=Cyc_Toc_temp_var();struct Cyc_Absyn_Stmt*_tmp71D=Cyc_Absyn_return_stmt((
struct Cyc_Absyn_Exp*)Cyc_Absyn_var_exp(_tmp71C,0),0);(void*)(s->r=(void*)((void*)(
Cyc_Absyn_declare_stmt(_tmp71C,(void*)topt->v,_tmp6FD,Cyc_Absyn_seq_stmt(Cyc_Toc_make_npop_handler(
s->try_depth),_tmp71D,0),0))->r));}else{Cyc_Toc_do_npop_before(s->try_depth,s);}}
return;}_LL383: if(_tmp6F9 <= (void*)1)goto _LL385;if(*((int*)_tmp6F9)!= 3)goto
_LL385;_tmp6FE=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmp6F9)->f1;_tmp6FF=((
struct Cyc_Absyn_IfThenElse_s_struct*)_tmp6F9)->f2;_tmp700=((struct Cyc_Absyn_IfThenElse_s_struct*)
_tmp6F9)->f3;_LL384: Cyc_Toc_exp_to_c(nv,_tmp6FE);Cyc_Toc_stmt_to_c(nv,_tmp6FF);s=
_tmp700;continue;_LL385: if(_tmp6F9 <= (void*)1)goto _LL387;if(*((int*)_tmp6F9)!= 4)
goto _LL387;_tmp701=((struct Cyc_Absyn_While_s_struct*)_tmp6F9)->f1;_tmp702=
_tmp701.f1;_tmp703=((struct Cyc_Absyn_While_s_struct*)_tmp6F9)->f2;_LL386: Cyc_Toc_exp_to_c(
nv,_tmp702);{struct Cyc_Toc_Env _tmp71F;struct _RegionHandle*_tmp720;struct Cyc_Toc_Env*
_tmp71E=nv;_tmp71F=*_tmp71E;_tmp720=_tmp71F.rgn;Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(
_tmp720,nv),_tmp703);return;}_LL387: if(_tmp6F9 <= (void*)1)goto _LL389;if(*((int*)
_tmp6F9)!= 5)goto _LL389;_tmp704=((struct Cyc_Absyn_Break_s_struct*)_tmp6F9)->f1;
_LL388: {struct Cyc_Toc_Env _tmp722;struct _dyneither_ptr**_tmp723;struct Cyc_Toc_Env*
_tmp721=nv;_tmp722=*_tmp721;_tmp723=_tmp722.break_lab;if(_tmp723 != 0)(void*)(s->r=(
void*)Cyc_Toc_goto_stmt_r(*_tmp723,0));{int dest_depth=_tmp704 == 0?0: _tmp704->try_depth;
Cyc_Toc_do_npop_before(s->try_depth - dest_depth,s);return;}}_LL389: if(_tmp6F9 <= (
void*)1)goto _LL38B;if(*((int*)_tmp6F9)!= 6)goto _LL38B;_tmp705=((struct Cyc_Absyn_Continue_s_struct*)
_tmp6F9)->f1;_LL38A: {struct Cyc_Toc_Env _tmp725;struct _dyneither_ptr**_tmp726;
struct Cyc_Toc_Env*_tmp724=nv;_tmp725=*_tmp724;_tmp726=_tmp725.continue_lab;if(
_tmp726 != 0)(void*)(s->r=(void*)Cyc_Toc_goto_stmt_r(*_tmp726,0));_tmp706=_tmp705;
goto _LL38C;}_LL38B: if(_tmp6F9 <= (void*)1)goto _LL38D;if(*((int*)_tmp6F9)!= 7)goto
_LL38D;_tmp706=((struct Cyc_Absyn_Goto_s_struct*)_tmp6F9)->f2;_LL38C: Cyc_Toc_do_npop_before(
s->try_depth - ((struct Cyc_Absyn_Stmt*)_check_null(_tmp706))->try_depth,s);
return;_LL38D: if(_tmp6F9 <= (void*)1)goto _LL38F;if(*((int*)_tmp6F9)!= 8)goto
_LL38F;_tmp707=((struct Cyc_Absyn_For_s_struct*)_tmp6F9)->f1;_tmp708=((struct Cyc_Absyn_For_s_struct*)
_tmp6F9)->f2;_tmp709=_tmp708.f1;_tmp70A=((struct Cyc_Absyn_For_s_struct*)_tmp6F9)->f3;
_tmp70B=_tmp70A.f1;_tmp70C=((struct Cyc_Absyn_For_s_struct*)_tmp6F9)->f4;_LL38E:
Cyc_Toc_exp_to_c(nv,_tmp707);Cyc_Toc_exp_to_c(nv,_tmp709);Cyc_Toc_exp_to_c(nv,
_tmp70B);{struct Cyc_Toc_Env _tmp728;struct _RegionHandle*_tmp729;struct Cyc_Toc_Env*
_tmp727=nv;_tmp728=*_tmp727;_tmp729=_tmp728.rgn;Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(
_tmp729,nv),_tmp70C);return;}_LL38F: if(_tmp6F9 <= (void*)1)goto _LL391;if(*((int*)
_tmp6F9)!= 9)goto _LL391;_tmp70D=((struct Cyc_Absyn_Switch_s_struct*)_tmp6F9)->f1;
_tmp70E=((struct Cyc_Absyn_Switch_s_struct*)_tmp6F9)->f2;_LL390: Cyc_Toc_xlate_switch(
nv,s,_tmp70D,_tmp70E);return;_LL391: if(_tmp6F9 <= (void*)1)goto _LL393;if(*((int*)
_tmp6F9)!= 10)goto _LL393;_tmp70F=((struct Cyc_Absyn_Fallthru_s_struct*)_tmp6F9)->f1;
_tmp710=((struct Cyc_Absyn_Fallthru_s_struct*)_tmp6F9)->f2;_LL392: {struct Cyc_Toc_Env
_tmp72B;struct Cyc_Toc_FallthruInfo*_tmp72C;struct Cyc_Toc_Env*_tmp72A=nv;_tmp72B=*
_tmp72A;_tmp72C=_tmp72B.fallthru_info;if(_tmp72C == 0){const char*_tmpBE0;void*
_tmpBDF;(_tmpBDF=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpBE0="fallthru in unexpected place",_tag_dyneither(_tmpBE0,sizeof(char),29))),
_tag_dyneither(_tmpBDF,sizeof(void*),0)));}{struct _dyneither_ptr*_tmp730;struct
Cyc_List_List*_tmp731;struct Cyc_Dict_Dict _tmp732;struct Cyc_Toc_FallthruInfo
_tmp72F=*_tmp72C;_tmp730=_tmp72F.label;_tmp731=_tmp72F.binders;_tmp732=_tmp72F.next_case_env;{
struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_goto_stmt(_tmp730,0);Cyc_Toc_do_npop_before(s->try_depth
- ((*((struct Cyc_Absyn_Switch_clause**)_check_null(_tmp710)))->body)->try_depth,
s2);{struct Cyc_List_List*_tmp733=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_rev)(_tmp731);struct Cyc_List_List*_tmp734=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(_tmp70F);for(0;_tmp733 != 0;(_tmp733=_tmp733->tl,
_tmp734=_tmp734->tl)){Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp734))->hd);s2=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(((struct
Cyc_Absyn_Exp*(*)(struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup)(_tmp732,(
struct _tuple1*)_tmp733->hd),(struct Cyc_Absyn_Exp*)_tmp734->hd,0),s2,0);}(void*)(
s->r=(void*)((void*)s2->r));return;}}}}_LL393: if(_tmp6F9 <= (void*)1)goto _LL395;
if(*((int*)_tmp6F9)!= 11)goto _LL395;_tmp711=((struct Cyc_Absyn_Decl_s_struct*)
_tmp6F9)->f1;_tmp712=((struct Cyc_Absyn_Decl_s_struct*)_tmp6F9)->f2;_LL394:{void*
_tmp735=(void*)_tmp711->r;struct Cyc_Absyn_Vardecl*_tmp736;struct Cyc_Absyn_Pat*
_tmp737;struct Cyc_Absyn_Exp*_tmp738;struct Cyc_List_List*_tmp739;struct Cyc_Absyn_Fndecl*
_tmp73A;struct Cyc_Absyn_Tvar*_tmp73B;struct Cyc_Absyn_Vardecl*_tmp73C;int _tmp73D;
struct Cyc_Absyn_Exp*_tmp73E;struct Cyc_Absyn_Exp*_tmp73F;struct Cyc_Absyn_Tvar*
_tmp740;struct Cyc_Absyn_Vardecl*_tmp741;_LL39E: if(_tmp735 <= (void*)2)goto _LL3AA;
if(*((int*)_tmp735)!= 0)goto _LL3A0;_tmp736=((struct Cyc_Absyn_Var_d_struct*)
_tmp735)->f1;_LL39F: {struct Cyc_Toc_Env _tmp743;struct _RegionHandle*_tmp744;
struct Cyc_Toc_Env*_tmp742=nv;_tmp743=*_tmp742;_tmp744=_tmp743.rgn;{struct Cyc_Absyn_Local_b_struct
_tmpBE3;struct Cyc_Absyn_Local_b_struct*_tmpBE2;struct Cyc_Toc_Env*_tmp745=Cyc_Toc_add_varmap(
_tmp744,nv,_tmp736->name,Cyc_Absyn_varb_exp(_tmp736->name,(void*)((_tmpBE2=
_cycalloc(sizeof(*_tmpBE2)),((_tmpBE2[0]=((_tmpBE3.tag=3,((_tmpBE3.f1=_tmp736,
_tmpBE3)))),_tmpBE2)))),0));Cyc_Toc_local_decl_to_c(_tmp745,_tmp745,_tmp736,
_tmp712);}goto _LL39D;}_LL3A0: if(*((int*)_tmp735)!= 2)goto _LL3A2;_tmp737=((struct
Cyc_Absyn_Let_d_struct*)_tmp735)->f1;_tmp738=((struct Cyc_Absyn_Let_d_struct*)
_tmp735)->f3;_LL3A1:{void*_tmp748=(void*)_tmp737->r;struct Cyc_Absyn_Vardecl*
_tmp749;struct Cyc_Absyn_Pat*_tmp74A;struct Cyc_Absyn_Pat _tmp74B;void*_tmp74C;
_LL3AD: if(_tmp748 <= (void*)2)goto _LL3AF;if(*((int*)_tmp748)!= 0)goto _LL3AF;
_tmp749=((struct Cyc_Absyn_Var_p_struct*)_tmp748)->f1;_tmp74A=((struct Cyc_Absyn_Var_p_struct*)
_tmp748)->f2;_tmp74B=*_tmp74A;_tmp74C=(void*)_tmp74B.r;if((int)_tmp74C != 0)goto
_LL3AF;_LL3AE: {struct _tuple1*old_name=_tmp749->name;struct _tuple1*new_name=Cyc_Toc_temp_var();
_tmp749->name=new_name;_tmp749->initializer=(struct Cyc_Absyn_Exp*)_tmp738;{
struct Cyc_Absyn_Var_d_struct _tmpBE6;struct Cyc_Absyn_Var_d_struct*_tmpBE5;(void*)(
_tmp711->r=(void*)((void*)((_tmpBE5=_cycalloc(sizeof(*_tmpBE5)),((_tmpBE5[0]=((
_tmpBE6.tag=0,((_tmpBE6.f1=_tmp749,_tmpBE6)))),_tmpBE5))))));}{struct Cyc_Toc_Env
_tmp750;struct _RegionHandle*_tmp751;struct Cyc_Toc_Env*_tmp74F=nv;_tmp750=*
_tmp74F;_tmp751=_tmp750.rgn;{struct Cyc_Absyn_Local_b_struct _tmpBE9;struct Cyc_Absyn_Local_b_struct*
_tmpBE8;struct Cyc_Toc_Env*_tmp752=Cyc_Toc_add_varmap(_tmp751,nv,old_name,Cyc_Absyn_varb_exp(
new_name,(void*)((_tmpBE8=_cycalloc(sizeof(*_tmpBE8)),((_tmpBE8[0]=((_tmpBE9.tag=
3,((_tmpBE9.f1=_tmp749,_tmpBE9)))),_tmpBE8)))),0));Cyc_Toc_local_decl_to_c(
_tmp752,nv,_tmp749,_tmp712);}goto _LL3AC;}}_LL3AF:;_LL3B0:(void*)(s->r=(void*)((
void*)(Cyc_Toc_letdecl_to_c(nv,_tmp737,(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp738->topt))->v,_tmp738,_tmp712))->r));goto _LL3AC;_LL3AC:;}goto _LL39D;_LL3A2:
if(*((int*)_tmp735)!= 3)goto _LL3A4;_tmp739=((struct Cyc_Absyn_Letv_d_struct*)
_tmp735)->f1;_LL3A3: {struct Cyc_List_List*_tmp755=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(_tmp739);if(_tmp755 == 0){const char*_tmpBEC;
void*_tmpBEB;(_tmpBEB=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmpBEC="empty Letv_d",_tag_dyneither(_tmpBEC,sizeof(char),
13))),_tag_dyneither(_tmpBEB,sizeof(void*),0)));}{struct Cyc_Absyn_Var_d_struct
_tmpBEF;struct Cyc_Absyn_Var_d_struct*_tmpBEE;(void*)(_tmp711->r=(void*)((void*)((
_tmpBEE=_cycalloc(sizeof(*_tmpBEE)),((_tmpBEE[0]=((_tmpBEF.tag=0,((_tmpBEF.f1=(
struct Cyc_Absyn_Vardecl*)_tmp755->hd,_tmpBEF)))),_tmpBEE))))));}_tmp755=_tmp755->tl;
for(0;_tmp755 != 0;_tmp755=_tmp755->tl){struct Cyc_Absyn_Var_d_struct _tmpBF2;
struct Cyc_Absyn_Var_d_struct*_tmpBF1;struct Cyc_Absyn_Decl*_tmp75A=Cyc_Absyn_new_decl((
void*)((_tmpBF1=_cycalloc(sizeof(*_tmpBF1)),((_tmpBF1[0]=((_tmpBF2.tag=0,((
_tmpBF2.f1=(struct Cyc_Absyn_Vardecl*)_tmp755->hd,_tmpBF2)))),_tmpBF1)))),0);(
void*)(s->r=(void*)((void*)(Cyc_Absyn_decl_stmt(_tmp75A,Cyc_Absyn_new_stmt((void*)
s->r,0),0))->r));}Cyc_Toc_stmt_to_c(nv,s);goto _LL39D;}_LL3A4: if(*((int*)_tmp735)
!= 1)goto _LL3A6;_tmp73A=((struct Cyc_Absyn_Fn_d_struct*)_tmp735)->f1;_LL3A5: {
struct _tuple1*_tmp75D=_tmp73A->name;struct Cyc_Toc_Env _tmp75F;struct _RegionHandle*
_tmp760;struct Cyc_Toc_Env*_tmp75E=nv;_tmp75F=*_tmp75E;_tmp760=_tmp75F.rgn;{
struct Cyc_Toc_Env*_tmp761=Cyc_Toc_add_varmap(_tmp760,nv,_tmp73A->name,Cyc_Absyn_var_exp(
_tmp75D,0));Cyc_Toc_fndecl_to_c(_tmp761,_tmp73A,0);Cyc_Toc_stmt_to_c(_tmp761,
_tmp712);}goto _LL39D;}_LL3A6: if(*((int*)_tmp735)!= 4)goto _LL3A8;_tmp73B=((struct
Cyc_Absyn_Region_d_struct*)_tmp735)->f1;_tmp73C=((struct Cyc_Absyn_Region_d_struct*)
_tmp735)->f2;_tmp73D=((struct Cyc_Absyn_Region_d_struct*)_tmp735)->f3;_tmp73E=((
struct Cyc_Absyn_Region_d_struct*)_tmp735)->f4;_LL3A7: {struct Cyc_Absyn_Stmt*
_tmp762=_tmp712;void*rh_struct_typ=Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp);
void*rh_struct_ptr_typ=Cyc_Absyn_cstar_typ(rh_struct_typ,Cyc_Toc_mt_tq);struct
_tuple1*rh_var=Cyc_Toc_temp_var();struct _tuple1*x_var=_tmp73C->name;struct Cyc_Absyn_Exp*
rh_exp=Cyc_Absyn_var_exp(rh_var,0);struct Cyc_Absyn_Exp*x_exp=Cyc_Absyn_var_exp(
x_var,0);struct Cyc_Toc_Env _tmp764;struct _RegionHandle*_tmp765;struct Cyc_Toc_Env*
_tmp763=nv;_tmp764=*_tmp763;_tmp765=_tmp764.rgn;Cyc_Toc_stmt_to_c(Cyc_Toc_add_varmap(
_tmp765,nv,x_var,x_exp),_tmp762);if(Cyc_Absyn_no_regions)(void*)(s->r=(void*)((
void*)(Cyc_Absyn_declare_stmt(x_var,rh_struct_ptr_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(
0,0),_tmp762,0))->r));else{if(_tmp73E == 0){struct Cyc_Absyn_Exp*_tmpBF5[1];struct
Cyc_Absyn_Exp*_tmpBF4[1];struct Cyc_List_List*_tmpBF3;(void*)(s->r=(void*)((void*)(
Cyc_Absyn_declare_stmt(rh_var,rh_struct_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_fncall_exp(
Cyc_Toc__new_region_e,((_tmpBF3=_cycalloc(sizeof(*_tmpBF3)),((_tmpBF3->hd=Cyc_Absyn_string_exp(
Cyc_Absynpp_qvar2string(x_var),0),((_tmpBF3->tl=0,_tmpBF3)))))),0),Cyc_Absyn_declare_stmt(
x_var,rh_struct_ptr_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(rh_exp,0),
Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__push_region_e,((
_tmpBF4[0]=x_exp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpBF4,sizeof(struct Cyc_Absyn_Exp*),1)))),0),0),Cyc_Absyn_seq_stmt(
_tmp762,Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_region_e,((_tmpBF5[
0]=x_exp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpBF5,sizeof(struct Cyc_Absyn_Exp*),1)))),0),0),0),0),0),0))->r));}
else{Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp73E);{struct Cyc_Absyn_Exp*
_tmpBF7[1];struct Cyc_Absyn_Exp*_tmpBF6[2];(void*)(s->r=(void*)((void*)(Cyc_Absyn_declare_stmt(
rh_var,Cyc_Absyn_strct(Cyc_Toc__DynRegionFrame_sp),0,Cyc_Absyn_declare_stmt(
x_var,rh_struct_ptr_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_fncall_exp(Cyc_Toc__open_dynregion_e,((
_tmpBF6[1]=(struct Cyc_Absyn_Exp*)_tmp73E,((_tmpBF6[0]=Cyc_Absyn_address_exp(
rh_exp,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpBF6,sizeof(struct Cyc_Absyn_Exp*),2)))))),0),Cyc_Absyn_seq_stmt(
_tmp762,Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_dynregion_e,((
_tmpBF7[0]=x_exp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpBF7,sizeof(struct Cyc_Absyn_Exp*),1)))),0),0),0),0),0))->r));}}}
return;}_LL3A8: if(*((int*)_tmp735)!= 5)goto _LL3AA;_tmp73F=((struct Cyc_Absyn_Alias_d_struct*)
_tmp735)->f1;_tmp740=((struct Cyc_Absyn_Alias_d_struct*)_tmp735)->f2;_tmp741=((
struct Cyc_Absyn_Alias_d_struct*)_tmp735)->f3;_LL3A9: {struct _tuple1*old_name=
_tmp741->name;struct _tuple1*new_name=Cyc_Toc_temp_var();_tmp741->name=new_name;
_tmp741->initializer=(struct Cyc_Absyn_Exp*)_tmp73F;{struct Cyc_Absyn_Decl_s_struct
_tmpC06;struct Cyc_Absyn_Var_d_struct*_tmpC05;struct Cyc_Absyn_Var_d_struct _tmpC04;
struct Cyc_Absyn_Decl*_tmpC03;struct Cyc_Absyn_Decl_s_struct*_tmpC02;(void*)(s->r=(
void*)((void*)((_tmpC02=_cycalloc(sizeof(*_tmpC02)),((_tmpC02[0]=((_tmpC06.tag=
11,((_tmpC06.f1=((_tmpC03=_cycalloc(sizeof(*_tmpC03)),((_tmpC03->r=(void*)((void*)((
_tmpC05=_cycalloc(sizeof(*_tmpC05)),((_tmpC05[0]=((_tmpC04.tag=0,((_tmpC04.f1=
_tmp741,_tmpC04)))),_tmpC05))))),((_tmpC03->loc=0,_tmpC03)))))),((_tmpC06.f2=
_tmp712,_tmpC06)))))),_tmpC02))))));}{struct Cyc_Toc_Env _tmp771;struct
_RegionHandle*_tmp772;struct Cyc_Toc_Env*_tmp770=nv;_tmp771=*_tmp770;_tmp772=
_tmp771.rgn;{struct Cyc_Absyn_Local_b_struct _tmpC09;struct Cyc_Absyn_Local_b_struct*
_tmpC08;struct Cyc_Toc_Env*_tmp773=Cyc_Toc_add_varmap(_tmp772,nv,old_name,Cyc_Absyn_varb_exp(
new_name,(void*)((_tmpC08=_cycalloc(sizeof(*_tmpC08)),((_tmpC08[0]=((_tmpC09.tag=
3,((_tmpC09.f1=_tmp741,_tmpC09)))),_tmpC08)))),0));Cyc_Toc_local_decl_to_c(
_tmp773,nv,_tmp741,_tmp712);}return;}}_LL3AA:;_LL3AB: {const char*_tmpC0C;void*
_tmpC0B;(_tmpC0B=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmpC0C="bad nested declaration within function",_tag_dyneither(_tmpC0C,sizeof(
char),39))),_tag_dyneither(_tmpC0B,sizeof(void*),0)));}_LL39D:;}return;_LL395:
if(_tmp6F9 <= (void*)1)goto _LL397;if(*((int*)_tmp6F9)!= 12)goto _LL397;_tmp713=((
struct Cyc_Absyn_Label_s_struct*)_tmp6F9)->f1;_tmp714=((struct Cyc_Absyn_Label_s_struct*)
_tmp6F9)->f2;_LL396: s=_tmp714;continue;_LL397: if(_tmp6F9 <= (void*)1)goto _LL399;
if(*((int*)_tmp6F9)!= 13)goto _LL399;_tmp715=((struct Cyc_Absyn_Do_s_struct*)
_tmp6F9)->f1;_tmp716=((struct Cyc_Absyn_Do_s_struct*)_tmp6F9)->f2;_tmp717=_tmp716.f1;
_LL398: {struct Cyc_Toc_Env _tmp779;struct _RegionHandle*_tmp77A;struct Cyc_Toc_Env*
_tmp778=nv;_tmp779=*_tmp778;_tmp77A=_tmp779.rgn;Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(
_tmp77A,nv),_tmp715);Cyc_Toc_exp_to_c(nv,_tmp717);return;}_LL399: if(_tmp6F9 <= (
void*)1)goto _LL39B;if(*((int*)_tmp6F9)!= 14)goto _LL39B;_tmp718=((struct Cyc_Absyn_TryCatch_s_struct*)
_tmp6F9)->f1;_tmp719=((struct Cyc_Absyn_TryCatch_s_struct*)_tmp6F9)->f2;_LL39A: {
struct _tuple1*h_var=Cyc_Toc_temp_var();struct _tuple1*e_var=Cyc_Toc_temp_var();
struct _tuple1*was_thrown_var=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*h_exp=Cyc_Absyn_var_exp(
h_var,0);struct Cyc_Absyn_Exp*e_exp=Cyc_Absyn_var_exp(e_var,0);struct Cyc_Absyn_Exp*
was_thrown_exp=Cyc_Absyn_var_exp(was_thrown_var,0);void*h_typ=Cyc_Absyn_strct(
Cyc_Toc__handler_cons_sp);void*e_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_exn_typ);void*
was_thrown_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_sint_typ);{struct Cyc_Core_Opt*_tmpC0D;
e_exp->topt=((_tmpC0D=_cycalloc(sizeof(*_tmpC0D)),((_tmpC0D->v=(void*)e_typ,
_tmpC0D))));}{struct Cyc_Toc_Env _tmp77D;struct _RegionHandle*_tmp77E;struct Cyc_Toc_Env*
_tmp77C=nv;_tmp77D=*_tmp77C;_tmp77E=_tmp77D.rgn;{struct Cyc_Toc_Env*_tmp77F=Cyc_Toc_add_varmap(
_tmp77E,nv,e_var,e_exp);Cyc_Toc_stmt_to_c(_tmp77F,_tmp718);{struct Cyc_Absyn_Stmt*
_tmp780=Cyc_Absyn_seq_stmt(_tmp718,Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_handler_e,
0,0),0),0);struct _tuple1*_tmp781=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp782=
Cyc_Absyn_var_exp(_tmp781,0);struct Cyc_Absyn_Vardecl*_tmp783=Cyc_Absyn_new_vardecl(
_tmp781,Cyc_Absyn_exn_typ,0);{struct Cyc_Core_Opt*_tmpC0E;_tmp782->topt=((_tmpC0E=
_cycalloc(sizeof(*_tmpC0E)),((_tmpC0E->v=(void*)Cyc_Absyn_exn_typ,_tmpC0E))));}{
struct Cyc_Absyn_Var_p_struct*_tmpC1F;struct Cyc_Absyn_Pat*_tmpC1E;struct Cyc_Core_Opt*
_tmpC1D;struct Cyc_Absyn_Var_p_struct _tmpC1C;struct Cyc_Core_Opt*_tmpC1B;struct Cyc_Absyn_Pat*
_tmpC1A;struct Cyc_Absyn_Pat*_tmp785=(_tmpC1A=_cycalloc(sizeof(*_tmpC1A)),((
_tmpC1A->r=(void*)((void*)((_tmpC1F=_cycalloc(sizeof(*_tmpC1F)),((_tmpC1F[0]=((
_tmpC1C.tag=0,((_tmpC1C.f1=_tmp783,((_tmpC1C.f2=((_tmpC1E=_cycalloc(sizeof(*
_tmpC1E)),((_tmpC1E->r=(void*)((void*)0),((_tmpC1E->topt=((_tmpC1D=_cycalloc(
sizeof(*_tmpC1D)),((_tmpC1D->v=(void*)Cyc_Absyn_exn_typ,_tmpC1D)))),((_tmpC1E->loc=
0,_tmpC1E)))))))),_tmpC1C)))))),_tmpC1F))))),((_tmpC1A->topt=((_tmpC1B=_cycalloc(
sizeof(*_tmpC1B)),((_tmpC1B->v=(void*)Cyc_Absyn_exn_typ,_tmpC1B)))),((_tmpC1A->loc=
0,_tmpC1A)))))));struct Cyc_Absyn_Exp*_tmp786=Cyc_Absyn_throw_exp(_tmp782,0);{
struct Cyc_Core_Opt*_tmpC20;_tmp786->topt=((_tmpC20=_cycalloc(sizeof(*_tmpC20)),((
_tmpC20->v=(void*)((void*)0),_tmpC20))));}{struct Cyc_Absyn_Stmt*_tmp788=Cyc_Absyn_exp_stmt(
_tmp786,0);struct Cyc_Core_Opt*_tmpC26;struct Cyc_List_List*_tmpC25;struct Cyc_Absyn_Switch_clause*
_tmpC24;struct Cyc_Absyn_Switch_clause*_tmp789=(_tmpC24=_cycalloc(sizeof(*_tmpC24)),((
_tmpC24->pattern=_tmp785,((_tmpC24->pat_vars=((_tmpC26=_cycalloc(sizeof(*_tmpC26)),((
_tmpC26->v=((_tmpC25=_cycalloc(sizeof(*_tmpC25)),((_tmpC25->hd=_tmp783,((_tmpC25->tl=
0,_tmpC25)))))),_tmpC26)))),((_tmpC24->where_clause=0,((_tmpC24->body=_tmp788,((
_tmpC24->loc=0,_tmpC24)))))))))));struct Cyc_List_List*_tmpC27;struct Cyc_Absyn_Stmt*
_tmp78A=Cyc_Absyn_switch_stmt(e_exp,((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_append)(_tmp719,((_tmpC27=_cycalloc(sizeof(*
_tmpC27)),((_tmpC27->hd=_tmp789,((_tmpC27->tl=0,_tmpC27))))))),0);Cyc_Toc_stmt_to_c(
_tmp77F,_tmp78A);{struct Cyc_List_List*_tmpC28;struct Cyc_Absyn_Exp*_tmp78B=Cyc_Absyn_fncall_exp(
Cyc_Toc_setjmp_e,((_tmpC28=_cycalloc(sizeof(*_tmpC28)),((_tmpC28->hd=Cyc_Absyn_aggrmember_exp(
h_exp,Cyc_Toc_handler_sp,0),((_tmpC28->tl=0,_tmpC28)))))),0);struct Cyc_List_List*
_tmpC29;struct Cyc_Absyn_Stmt*_tmp78C=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__push_handler_e,((
_tmpC29=_cycalloc(sizeof(*_tmpC29)),((_tmpC29->hd=Cyc_Absyn_address_exp(h_exp,0),((
_tmpC29->tl=0,_tmpC29)))))),0),0);struct Cyc_Absyn_Exp*_tmp78D=Cyc_Absyn_int_exp((
void*)0,0,0);struct Cyc_Absyn_Exp*_tmp78E=Cyc_Absyn_int_exp((void*)0,1,0);(void*)(
s->r=(void*)((void*)(Cyc_Absyn_declare_stmt(h_var,h_typ,0,Cyc_Absyn_seq_stmt(
_tmp78C,Cyc_Absyn_declare_stmt(was_thrown_var,was_thrown_typ,(struct Cyc_Absyn_Exp*)
_tmp78D,Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(_tmp78B,Cyc_Absyn_assign_stmt(
was_thrown_exp,_tmp78E,0),Cyc_Toc_skip_stmt_dl(),0),Cyc_Absyn_ifthenelse_stmt(
Cyc_Absyn_prim1_exp((void*)11,was_thrown_exp,0),_tmp780,Cyc_Absyn_declare_stmt(
e_var,e_typ,(struct Cyc_Absyn_Exp*)Cyc_Toc_cast_it(e_typ,Cyc_Toc__exn_thrown_e),
_tmp78A,0),0),0),0),0),0))->r));}}}}}return;}}_LL39B: if(_tmp6F9 <= (void*)1)goto
_LL37A;if(*((int*)_tmp6F9)!= 15)goto _LL37A;_tmp71A=((struct Cyc_Absyn_ResetRegion_s_struct*)
_tmp6F9)->f1;_LL39C: if(Cyc_Absyn_no_regions)(void*)(s->r=(void*)((void*)0));
else{Cyc_Toc_exp_to_c(nv,_tmp71A);{struct Cyc_List_List*_tmpC2A;(void*)(s->r=(
void*)Cyc_Toc_exp_stmt_r(Cyc_Absyn_fncall_exp(Cyc_Toc__reset_region_e,((_tmpC2A=
_cycalloc(sizeof(*_tmpC2A)),((_tmpC2A->hd=_tmp71A,((_tmpC2A->tl=0,_tmpC2A)))))),
0)));}}return;_LL37A:;}}static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*s);
struct _tuple17{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};static
void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int
cinclude);static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*
f,int cinclude){f->tvs=0;f->effect=0;f->rgn_po=0;(void*)(f->ret_type=(void*)Cyc_Toc_typ_to_c((
void*)f->ret_type));{struct _RegionHandle _tmp79C=_new_region("frgn");struct
_RegionHandle*frgn=& _tmp79C;_push_region(frgn);{struct Cyc_Toc_Env*_tmp79D=Cyc_Toc_share_env(
frgn,nv);{struct Cyc_List_List*_tmp79E=f->args;for(0;_tmp79E != 0;_tmp79E=_tmp79E->tl){
union Cyc_Absyn_Nmspace_union _tmpC2D;struct _tuple1*_tmpC2C;struct _tuple1*_tmp79F=(
_tmpC2C=_cycalloc(sizeof(*_tmpC2C)),((_tmpC2C->f1=(union Cyc_Absyn_Nmspace_union)((
union Cyc_Absyn_Nmspace_union)(((_tmpC2D.Loc_n).tag=0,_tmpC2D))),((_tmpC2C->f2=(*((
struct _tuple17*)_tmp79E->hd)).f1,_tmpC2C)))));(*((struct _tuple17*)_tmp79E->hd)).f3=
Cyc_Toc_typ_to_c((*((struct _tuple17*)_tmp79E->hd)).f3);_tmp79D=Cyc_Toc_add_varmap(
frgn,_tmp79D,_tmp79F,Cyc_Absyn_var_exp(_tmp79F,0));}}if(cinclude){Cyc_Toc_stmttypes_to_c(
f->body);_npop_handler(0);return;}if((unsigned int)f->cyc_varargs  && ((struct Cyc_Absyn_VarargInfo*)
_check_null(f->cyc_varargs))->name != 0){struct Cyc_Core_Opt*_tmp7A3;struct Cyc_Absyn_Tqual
_tmp7A4;void*_tmp7A5;int _tmp7A6;struct Cyc_Absyn_VarargInfo _tmp7A2=*((struct Cyc_Absyn_VarargInfo*)
_check_null(f->cyc_varargs));_tmp7A3=_tmp7A2.name;_tmp7A4=_tmp7A2.tq;_tmp7A5=(
void*)_tmp7A2.type;_tmp7A6=_tmp7A2.inject;{void*_tmp7A7=Cyc_Toc_typ_to_c(Cyc_Absyn_dyneither_typ(
_tmp7A5,(void*)2,_tmp7A4,Cyc_Absyn_false_conref));union Cyc_Absyn_Nmspace_union
_tmpC30;struct _tuple1*_tmpC2F;struct _tuple1*_tmp7A8=(_tmpC2F=_cycalloc(sizeof(*
_tmpC2F)),((_tmpC2F->f1=(union Cyc_Absyn_Nmspace_union)((union Cyc_Absyn_Nmspace_union)(((
_tmpC30.Loc_n).tag=0,_tmpC30))),((_tmpC2F->f2=(struct _dyneither_ptr*)((struct Cyc_Core_Opt*)
_check_null(_tmp7A3))->v,_tmpC2F)))));{struct _tuple17*_tmpC33;struct Cyc_List_List*
_tmpC32;f->args=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_append)(f->args,((_tmpC32=_cycalloc(sizeof(*_tmpC32)),((_tmpC32->hd=((
_tmpC33=_cycalloc(sizeof(*_tmpC33)),((_tmpC33->f1=(struct _dyneither_ptr*)_tmp7A3->v,((
_tmpC33->f2=_tmp7A4,((_tmpC33->f3=_tmp7A7,_tmpC33)))))))),((_tmpC32->tl=0,
_tmpC32)))))));}_tmp79D=Cyc_Toc_add_varmap(frgn,_tmp79D,_tmp7A8,Cyc_Absyn_var_exp(
_tmp7A8,0));f->cyc_varargs=0;}}{struct Cyc_List_List*_tmp7AD=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(f->param_vardecls))->v;for(0;_tmp7AD != 0;_tmp7AD=
_tmp7AD->tl){(void*)(((struct Cyc_Absyn_Vardecl*)_tmp7AD->hd)->type=(void*)Cyc_Toc_typ_to_c((
void*)((struct Cyc_Absyn_Vardecl*)_tmp7AD->hd)->type));}}Cyc_Toc_stmt_to_c(Cyc_Toc_clear_toplevel(
frgn,_tmp79D),f->body);};_pop_region(frgn);}}static void*Cyc_Toc_scope_to_c(void*
s);static void*Cyc_Toc_scope_to_c(void*s){void*_tmp7AE=s;_LL3B2: if((int)_tmp7AE != 
1)goto _LL3B4;_LL3B3: return(void*)2;_LL3B4: if((int)_tmp7AE != 4)goto _LL3B6;_LL3B5:
return(void*)3;_LL3B6:;_LL3B7: return s;_LL3B1:;}static void Cyc_Toc_aggrdecl_to_c(
struct Cyc_Absyn_Aggrdecl*ad);static void Cyc_Toc_aggrdecl_to_c(struct Cyc_Absyn_Aggrdecl*
ad){struct _tuple1*_tmp7AF=ad->name;struct _DynRegionHandle*_tmp7B1;struct Cyc_Dict_Dict*
_tmp7B2;struct Cyc_Toc_TocState _tmp7B0=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));
_tmp7B1=_tmp7B0.dyn;_tmp7B2=_tmp7B0.aggrs_so_far;{struct _DynRegionFrame _tmp7B3;
struct _RegionHandle*d=_open_dynregion(& _tmp7B3,_tmp7B1);{int seen_defn_before;
struct _tuple7**_tmp7B4=((struct _tuple7**(*)(struct Cyc_Dict_Dict d,struct _tuple1*k))
Cyc_Dict_lookup_opt)(*_tmp7B2,_tmp7AF);if(_tmp7B4 == 0){seen_defn_before=0;{
struct _tuple7*v;if((void*)ad->kind == (void*)0){struct _tuple7*_tmpC34;v=((_tmpC34=
_region_malloc(d,sizeof(*_tmpC34)),((_tmpC34->f1=ad,((_tmpC34->f2=Cyc_Absyn_strctq(
_tmp7AF),_tmpC34))))));}else{struct _tuple7*_tmpC35;v=((_tmpC35=_region_malloc(d,
sizeof(*_tmpC35)),((_tmpC35->f1=ad,((_tmpC35->f2=Cyc_Absyn_unionq_typ(_tmp7AF),
_tmpC35))))));}*_tmp7B2=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct
_tuple1*k,struct _tuple7*v))Cyc_Dict_insert)(*_tmp7B2,_tmp7AF,v);}}else{struct
_tuple7 _tmp7B8;struct Cyc_Absyn_Aggrdecl*_tmp7B9;void*_tmp7BA;struct _tuple7*
_tmp7B7=*_tmp7B4;_tmp7B8=*_tmp7B7;_tmp7B9=_tmp7B8.f1;_tmp7BA=_tmp7B8.f2;if(
_tmp7B9->impl == 0){{struct _tuple7*_tmpC36;*_tmp7B2=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,struct _tuple1*k,struct _tuple7*v))Cyc_Dict_insert)(*_tmp7B2,
_tmp7AF,((_tmpC36=_region_malloc(d,sizeof(*_tmpC36)),((_tmpC36->f1=ad,((_tmpC36->f2=
_tmp7BA,_tmpC36)))))));}seen_defn_before=0;}else{seen_defn_before=1;}}(void*)(ad->sc=(
void*)((void*)2));ad->tvs=0;if(ad->impl != 0){((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(ad->impl))->exist_vars=0;((struct Cyc_Absyn_AggrdeclImpl*)_check_null(
ad->impl))->rgn_po=0;if(seen_defn_before)ad->impl=0;else{struct Cyc_List_List*
_tmp7BC=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;for(0;
_tmp7BC != 0;_tmp7BC=_tmp7BC->tl){((struct Cyc_Absyn_Aggrfield*)_tmp7BC->hd)->tq=
Cyc_Toc_mt_tq;(void*)(((struct Cyc_Absyn_Aggrfield*)_tmp7BC->hd)->type=(void*)Cyc_Toc_typ_to_c_array((
void*)((struct Cyc_Absyn_Aggrfield*)_tmp7BC->hd)->type));if(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(ad->impl))->tagged){void*_tmp7BD=(void*)((struct Cyc_Absyn_Aggrfield*)
_tmp7BC->hd)->type;struct _dyneither_ptr*_tmp7BE=((struct Cyc_Absyn_Aggrfield*)
_tmp7BC->hd)->name;const char*_tmpC3B;void*_tmpC3A[2];struct Cyc_String_pa_struct
_tmpC39;struct Cyc_String_pa_struct _tmpC38;struct _dyneither_ptr s=(struct
_dyneither_ptr)((_tmpC38.tag=0,((_tmpC38.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*_tmp7BE),((_tmpC39.tag=0,((_tmpC39.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_qvar2string(ad->name)),((_tmpC3A[0]=& _tmpC39,((
_tmpC3A[1]=& _tmpC38,Cyc_aprintf(((_tmpC3B="_union_%s_%s",_tag_dyneither(_tmpC3B,
sizeof(char),13))),_tag_dyneither(_tmpC3A,sizeof(void*),2))))))))))))));struct
_dyneither_ptr*_tmpC3C;struct _dyneither_ptr*str=(_tmpC3C=_cycalloc(sizeof(*
_tmpC3C)),((_tmpC3C[0]=s,_tmpC3C)));struct Cyc_Absyn_Aggrfield*_tmpC3D;struct Cyc_Absyn_Aggrfield*
_tmp7BF=(_tmpC3D=_cycalloc(sizeof(*_tmpC3D)),((_tmpC3D->name=Cyc_Toc_val_sp,((
_tmpC3D->tq=Cyc_Toc_mt_tq,((_tmpC3D->type=(void*)_tmp7BD,((_tmpC3D->width=0,((
_tmpC3D->attributes=0,_tmpC3D)))))))))));struct Cyc_Absyn_Aggrfield*_tmpC3E;
struct Cyc_Absyn_Aggrfield*_tmp7C0=(_tmpC3E=_cycalloc(sizeof(*_tmpC3E)),((_tmpC3E->name=
Cyc_Toc_tag_sp,((_tmpC3E->tq=Cyc_Toc_mt_tq,((_tmpC3E->type=(void*)Cyc_Absyn_sint_typ,((
_tmpC3E->width=0,((_tmpC3E->attributes=0,_tmpC3E)))))))))));struct Cyc_Absyn_Aggrfield*
_tmpC3F[2];struct Cyc_List_List*_tmp7C1=(_tmpC3F[1]=_tmp7BF,((_tmpC3F[0]=_tmp7C0,((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmpC3F,sizeof(struct Cyc_Absyn_Aggrfield*),2)))));struct _tuple1*_tmpC47;union Cyc_Absyn_Nmspace_union
_tmpC46;struct Cyc_Absyn_AggrdeclImpl*_tmpC45;struct Cyc_Absyn_Aggrdecl*_tmpC44;
struct Cyc_Absyn_Aggrdecl*_tmp7C2=(_tmpC44=_cycalloc(sizeof(*_tmpC44)),((_tmpC44->kind=(
void*)((void*)0),((_tmpC44->sc=(void*)((void*)2),((_tmpC44->name=((_tmpC47=
_cycalloc(sizeof(*_tmpC47)),((_tmpC47->f1=(union Cyc_Absyn_Nmspace_union)(((
_tmpC46.Loc_n).tag=0,_tmpC46)),((_tmpC47->f2=str,_tmpC47)))))),((_tmpC44->tvs=0,((
_tmpC44->impl=((_tmpC45=_cycalloc(sizeof(*_tmpC45)),((_tmpC45->exist_vars=0,((
_tmpC45->rgn_po=0,((_tmpC45->fields=_tmp7C1,((_tmpC45->tagged=0,_tmpC45)))))))))),((
_tmpC44->attributes=0,_tmpC44)))))))))))));{struct Cyc_Absyn_Aggr_d_struct*
_tmpC4D;struct Cyc_Absyn_Aggr_d_struct _tmpC4C;struct Cyc_List_List*_tmpC4B;Cyc_Toc_result_decls=((
_tmpC4B=_cycalloc(sizeof(*_tmpC4B)),((_tmpC4B->hd=Cyc_Absyn_new_decl((void*)((
_tmpC4D=_cycalloc(sizeof(*_tmpC4D)),((_tmpC4D[0]=((_tmpC4C.tag=6,((_tmpC4C.f1=
_tmp7C2,_tmpC4C)))),_tmpC4D)))),0),((_tmpC4B->tl=Cyc_Toc_result_decls,_tmpC4B))))));}(
void*)(((struct Cyc_Absyn_Aggrfield*)_tmp7BC->hd)->type=(void*)Cyc_Absyn_strct(
str));}}}}};_pop_dynregion(d);}}static void Cyc_Toc_datatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*
tud);static void Cyc_Toc_datatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*tud){
struct _DynRegionHandle*_tmp7D3;struct Cyc_Set_Set**_tmp7D4;struct Cyc_Toc_TocState
_tmp7D2=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp7D3=
_tmp7D2.dyn;_tmp7D4=_tmp7D2.datatypes_so_far;{struct _DynRegionFrame _tmp7D5;
struct _RegionHandle*d=_open_dynregion(& _tmp7D5,_tmp7D3);{struct _tuple1*_tmp7D6=
tud->name;if(tud->fields == 0  || ((int(*)(struct Cyc_Set_Set*s,struct _tuple1*elt))
Cyc_Set_member)(*_tmp7D4,_tmp7D6)){_npop_handler(0);return;}*_tmp7D4=((struct Cyc_Set_Set*(*)(
struct _RegionHandle*r,struct Cyc_Set_Set*s,struct _tuple1*elt))Cyc_Set_rinsert)(d,*
_tmp7D4,_tmp7D6);}{struct Cyc_List_List*flat_structs=0;{struct Cyc_List_List*
_tmp7D7=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(tud->fields))->v;
for(0;_tmp7D7 != 0;_tmp7D7=_tmp7D7->tl){struct Cyc_Absyn_Datatypefield*f=(struct
Cyc_Absyn_Datatypefield*)_tmp7D7->hd;if(f->typs != 0  || tud->is_flat){struct Cyc_List_List*
_tmp7D8=0;int i=1;{struct Cyc_List_List*_tmp7D9=f->typs;for(0;_tmp7D9 != 0;(_tmp7D9=
_tmp7D9->tl,i ++)){struct _dyneither_ptr*_tmp7DA=Cyc_Absyn_fieldname(i);struct Cyc_Absyn_Aggrfield*
_tmpC4E;struct Cyc_Absyn_Aggrfield*_tmp7DB=(_tmpC4E=_cycalloc(sizeof(*_tmpC4E)),((
_tmpC4E->name=_tmp7DA,((_tmpC4E->tq=(*((struct _tuple4*)_tmp7D9->hd)).f1,((
_tmpC4E->type=(void*)Cyc_Toc_typ_to_c_array((*((struct _tuple4*)_tmp7D9->hd)).f2),((
_tmpC4E->width=0,((_tmpC4E->attributes=0,_tmpC4E)))))))))));struct Cyc_List_List*
_tmpC4F;_tmp7D8=((_tmpC4F=_cycalloc(sizeof(*_tmpC4F)),((_tmpC4F->hd=_tmp7DB,((
_tmpC4F->tl=_tmp7D8,_tmpC4F))))));}}{struct Cyc_Absyn_Aggrfield*_tmpC52;struct Cyc_List_List*
_tmpC51;_tmp7D8=((_tmpC51=_cycalloc(sizeof(*_tmpC51)),((_tmpC51->hd=((_tmpC52=
_cycalloc(sizeof(*_tmpC52)),((_tmpC52->name=Cyc_Toc_tag_sp,((_tmpC52->tq=Cyc_Toc_mt_tq,((
_tmpC52->type=(void*)Cyc_Absyn_sint_typ,((_tmpC52->width=0,((_tmpC52->attributes=
0,_tmpC52)))))))))))),((_tmpC51->tl=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(_tmp7D8),_tmpC51))))));}{const char*_tmpC57;struct Cyc_Absyn_AggrdeclImpl*
_tmpC56;struct Cyc_Absyn_Aggrdecl*_tmpC55;struct Cyc_Absyn_Aggrdecl*_tmp7E0=(
_tmpC55=_cycalloc(sizeof(*_tmpC55)),((_tmpC55->kind=(void*)((void*)0),((_tmpC55->sc=(
void*)((void*)2),((_tmpC55->name=Cyc_Toc_collapse_qvar_tag(f->name,((_tmpC57="_struct",
_tag_dyneither(_tmpC57,sizeof(char),8)))),((_tmpC55->tvs=0,((_tmpC55->impl=((
_tmpC56=_cycalloc(sizeof(*_tmpC56)),((_tmpC56->exist_vars=0,((_tmpC56->rgn_po=0,((
_tmpC56->fields=_tmp7D8,((_tmpC56->tagged=0,_tmpC56)))))))))),((_tmpC55->attributes=
0,_tmpC55)))))))))))));{struct Cyc_Absyn_Aggr_d_struct*_tmpC5D;struct Cyc_Absyn_Aggr_d_struct
_tmpC5C;struct Cyc_List_List*_tmpC5B;Cyc_Toc_result_decls=((_tmpC5B=_cycalloc(
sizeof(*_tmpC5B)),((_tmpC5B->hd=Cyc_Absyn_new_decl((void*)((_tmpC5D=_cycalloc(
sizeof(*_tmpC5D)),((_tmpC5D[0]=((_tmpC5C.tag=6,((_tmpC5C.f1=_tmp7E0,_tmpC5C)))),
_tmpC5D)))),0),((_tmpC5B->tl=Cyc_Toc_result_decls,_tmpC5B))))));}if(tud->is_flat){
struct Cyc_Absyn_AggrType_struct*_tmpC72;struct Cyc_Absyn_AggrInfo _tmpC71;struct
Cyc_Absyn_Aggrdecl**_tmpC70;union Cyc_Absyn_AggrInfoU_union _tmpC6F;struct Cyc_Absyn_AggrType_struct
_tmpC6E;struct Cyc_Absyn_Aggrfield*_tmpC6D;struct Cyc_Absyn_Aggrfield*_tmp7E4=(
_tmpC6D=_cycalloc(sizeof(*_tmpC6D)),((_tmpC6D->name=(*f->name).f2,((_tmpC6D->tq=
Cyc_Toc_mt_tq,((_tmpC6D->type=(void*)((void*)((_tmpC72=_cycalloc(sizeof(*_tmpC72)),((
_tmpC72[0]=((_tmpC6E.tag=10,((_tmpC6E.f1=((_tmpC71.aggr_info=(union Cyc_Absyn_AggrInfoU_union)(((
_tmpC6F.KnownAggr).tag=1,(((_tmpC6F.KnownAggr).f1=((_tmpC70=_cycalloc(sizeof(*
_tmpC70)),((_tmpC70[0]=_tmp7E0,_tmpC70)))),_tmpC6F)))),((_tmpC71.targs=0,_tmpC71)))),
_tmpC6E)))),_tmpC72))))),((_tmpC6D->width=0,((_tmpC6D->attributes=0,_tmpC6D)))))))))));
struct Cyc_List_List*_tmpC73;flat_structs=((_tmpC73=_cycalloc(sizeof(*_tmpC73)),((
_tmpC73->hd=_tmp7E4,((_tmpC73->tl=flat_structs,_tmpC73))))));}}}}}if(tud->is_flat){
flat_structs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
flat_structs);{const char*_tmpC78;struct Cyc_Absyn_AggrdeclImpl*_tmpC77;struct Cyc_Absyn_Aggrdecl*
_tmpC76;struct Cyc_Absyn_Aggrdecl*_tmp7EF=(_tmpC76=_cycalloc(sizeof(*_tmpC76)),((
_tmpC76->kind=(void*)((void*)1),((_tmpC76->sc=(void*)((void*)2),((_tmpC76->name=
Cyc_Toc_collapse_qvar_tag(tud->name,((_tmpC78="_union",_tag_dyneither(_tmpC78,
sizeof(char),7)))),((_tmpC76->tvs=0,((_tmpC76->impl=((_tmpC77=_cycalloc(sizeof(*
_tmpC77)),((_tmpC77->exist_vars=0,((_tmpC77->rgn_po=0,((_tmpC77->fields=
flat_structs,((_tmpC77->tagged=0,_tmpC77)))))))))),((_tmpC76->attributes=0,
_tmpC76)))))))))))));struct Cyc_Absyn_Aggr_d_struct*_tmpC7E;struct Cyc_Absyn_Aggr_d_struct
_tmpC7D;struct Cyc_List_List*_tmpC7C;Cyc_Toc_result_decls=((_tmpC7C=_cycalloc(
sizeof(*_tmpC7C)),((_tmpC7C->hd=Cyc_Absyn_new_decl((void*)((_tmpC7E=_cycalloc(
sizeof(*_tmpC7E)),((_tmpC7E[0]=((_tmpC7D.tag=6,((_tmpC7D.f1=_tmp7EF,_tmpC7D)))),
_tmpC7E)))),0),((_tmpC7C->tl=Cyc_Toc_result_decls,_tmpC7C))))));}}};
_pop_dynregion(d);}}static void Cyc_Toc_xdatatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*
xd);static void Cyc_Toc_xdatatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*xd){if(xd->fields
== 0)return;{struct _DynRegionHandle*_tmp7F7;struct Cyc_Dict_Dict*_tmp7F8;struct
Cyc_Toc_TocState _tmp7F6=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));
_tmp7F7=_tmp7F6.dyn;_tmp7F8=_tmp7F6.xdatatypes_so_far;{struct _DynRegionFrame
_tmp7F9;struct _RegionHandle*d=_open_dynregion(& _tmp7F9,_tmp7F7);{struct _tuple1*
_tmp7FA=xd->name;struct Cyc_List_List*_tmp7FB=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(xd->fields))->v;for(0;_tmp7FB != 0;_tmp7FB=_tmp7FB->tl){struct Cyc_Absyn_Datatypefield*
f=(struct Cyc_Absyn_Datatypefield*)_tmp7FB->hd;struct _dyneither_ptr*fn=(*f->name).f2;
struct Cyc_Absyn_Exp*_tmp7FC=Cyc_Absyn_uint_exp(_get_dyneither_size(*fn,sizeof(
char))+ 4,0);void*_tmp7FD=Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq,(
struct Cyc_Absyn_Exp*)_tmp7FC,Cyc_Absyn_false_conref,0);int*_tmp7FE=((int*(*)(
struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup_opt)(*_tmp7F8,f->name);int
_tmp7FF;_LL3B9: if(_tmp7FE != 0)goto _LL3BB;_LL3BA: {struct Cyc_Absyn_Exp*initopt=0;
if((void*)f->sc != (void*)3){char zero='\000';const char*_tmpC86;void*_tmpC85[5];
struct Cyc_Int_pa_struct _tmpC84;struct Cyc_Int_pa_struct _tmpC83;struct Cyc_Int_pa_struct
_tmpC82;struct Cyc_Int_pa_struct _tmpC81;struct Cyc_String_pa_struct _tmpC80;initopt=(
struct Cyc_Absyn_Exp*)Cyc_Absyn_string_exp((struct _dyneither_ptr)((_tmpC80.tag=0,((
_tmpC80.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*fn),((_tmpC81.tag=1,((
_tmpC81.f1=(unsigned long)((int)zero),((_tmpC82.tag=1,((_tmpC82.f1=(
unsigned long)((int)zero),((_tmpC83.tag=1,((_tmpC83.f1=(unsigned long)((int)zero),((
_tmpC84.tag=1,((_tmpC84.f1=(unsigned long)((int)zero),((_tmpC85[0]=& _tmpC84,((
_tmpC85[1]=& _tmpC83,((_tmpC85[2]=& _tmpC82,((_tmpC85[3]=& _tmpC81,((_tmpC85[4]=&
_tmpC80,Cyc_aprintf(((_tmpC86="%c%c%c%c%s",_tag_dyneither(_tmpC86,sizeof(char),
11))),_tag_dyneither(_tmpC85,sizeof(void*),5)))))))))))))))))))))))))))))))),0);}{
struct Cyc_Absyn_Vardecl*_tmp807=Cyc_Absyn_new_vardecl(f->name,_tmp7FD,initopt);(
void*)(_tmp807->sc=(void*)((void*)f->sc));{struct Cyc_Absyn_Var_d_struct*_tmpC8C;
struct Cyc_Absyn_Var_d_struct _tmpC8B;struct Cyc_List_List*_tmpC8A;Cyc_Toc_result_decls=((
_tmpC8A=_cycalloc(sizeof(*_tmpC8A)),((_tmpC8A->hd=Cyc_Absyn_new_decl((void*)((
_tmpC8C=_cycalloc(sizeof(*_tmpC8C)),((_tmpC8C[0]=((_tmpC8B.tag=0,((_tmpC8B.f1=
_tmp807,_tmpC8B)))),_tmpC8C)))),0),((_tmpC8A->tl=Cyc_Toc_result_decls,_tmpC8A))))));}*
_tmp7F8=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,int v))Cyc_Dict_insert)(*
_tmp7F8,f->name,(void*)f->sc != (void*)3);if(f->typs != 0){struct Cyc_List_List*
fields=0;int i=1;{struct Cyc_List_List*_tmp80B=f->typs;for(0;_tmp80B != 0;(_tmp80B=
_tmp80B->tl,i ++)){struct Cyc_Int_pa_struct _tmpC94;void*_tmpC93[1];const char*
_tmpC92;struct _dyneither_ptr*_tmpC91;struct _dyneither_ptr*_tmp80C=(_tmpC91=
_cycalloc(sizeof(*_tmpC91)),((_tmpC91[0]=(struct _dyneither_ptr)((_tmpC94.tag=1,((
_tmpC94.f1=(unsigned long)i,((_tmpC93[0]=& _tmpC94,Cyc_aprintf(((_tmpC92="f%d",
_tag_dyneither(_tmpC92,sizeof(char),4))),_tag_dyneither(_tmpC93,sizeof(void*),1)))))))),
_tmpC91)));struct Cyc_Absyn_Aggrfield*_tmpC95;struct Cyc_Absyn_Aggrfield*_tmp80D=(
_tmpC95=_cycalloc(sizeof(*_tmpC95)),((_tmpC95->name=_tmp80C,((_tmpC95->tq=(*((
struct _tuple4*)_tmp80B->hd)).f1,((_tmpC95->type=(void*)Cyc_Toc_typ_to_c_array((*((
struct _tuple4*)_tmp80B->hd)).f2),((_tmpC95->width=0,((_tmpC95->attributes=0,
_tmpC95)))))))))));struct Cyc_List_List*_tmpC96;fields=((_tmpC96=_cycalloc(
sizeof(*_tmpC96)),((_tmpC96->hd=_tmp80D,((_tmpC96->tl=fields,_tmpC96))))));}}{
struct Cyc_Absyn_Aggrfield*_tmpC99;struct Cyc_List_List*_tmpC98;fields=((_tmpC98=
_cycalloc(sizeof(*_tmpC98)),((_tmpC98->hd=((_tmpC99=_cycalloc(sizeof(*_tmpC99)),((
_tmpC99->name=Cyc_Toc_tag_sp,((_tmpC99->tq=Cyc_Toc_mt_tq,((_tmpC99->type=(void*)
Cyc_Absyn_cstar_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq),((_tmpC99->width=0,((
_tmpC99->attributes=0,_tmpC99)))))))))))),((_tmpC98->tl=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(fields),_tmpC98))))));}{const char*
_tmpC9E;struct Cyc_Absyn_AggrdeclImpl*_tmpC9D;struct Cyc_Absyn_Aggrdecl*_tmpC9C;
struct Cyc_Absyn_Aggrdecl*_tmp816=(_tmpC9C=_cycalloc(sizeof(*_tmpC9C)),((_tmpC9C->kind=(
void*)((void*)0),((_tmpC9C->sc=(void*)((void*)2),((_tmpC9C->name=Cyc_Toc_collapse_qvar_tag(
f->name,((_tmpC9E="_struct",_tag_dyneither(_tmpC9E,sizeof(char),8)))),((_tmpC9C->tvs=
0,((_tmpC9C->impl=((_tmpC9D=_cycalloc(sizeof(*_tmpC9D)),((_tmpC9D->exist_vars=0,((
_tmpC9D->rgn_po=0,((_tmpC9D->fields=fields,((_tmpC9D->tagged=0,_tmpC9D)))))))))),((
_tmpC9C->attributes=0,_tmpC9C)))))))))))));struct Cyc_Absyn_Aggr_d_struct*_tmpCA4;
struct Cyc_Absyn_Aggr_d_struct _tmpCA3;struct Cyc_List_List*_tmpCA2;Cyc_Toc_result_decls=((
_tmpCA2=_cycalloc(sizeof(*_tmpCA2)),((_tmpCA2->hd=Cyc_Absyn_new_decl((void*)((
_tmpCA4=_cycalloc(sizeof(*_tmpCA4)),((_tmpCA4[0]=((_tmpCA3.tag=6,((_tmpCA3.f1=
_tmp816,_tmpCA3)))),_tmpCA4)))),0),((_tmpCA2->tl=Cyc_Toc_result_decls,_tmpCA2))))));}}
goto _LL3B8;}}_LL3BB: if(_tmp7FE == 0)goto _LL3BD;_tmp7FF=*_tmp7FE;if(_tmp7FF != 0)
goto _LL3BD;_LL3BC: if((void*)f->sc != (void*)3){char zero='\000';const char*_tmpCAC;
void*_tmpCAB[5];struct Cyc_Int_pa_struct _tmpCAA;struct Cyc_Int_pa_struct _tmpCA9;
struct Cyc_Int_pa_struct _tmpCA8;struct Cyc_Int_pa_struct _tmpCA7;struct Cyc_String_pa_struct
_tmpCA6;struct Cyc_Absyn_Exp*_tmp81D=Cyc_Absyn_string_exp((struct _dyneither_ptr)((
_tmpCA6.tag=0,((_tmpCA6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*fn),((
_tmpCA7.tag=1,((_tmpCA7.f1=(unsigned long)((int)zero),((_tmpCA8.tag=1,((_tmpCA8.f1=(
unsigned long)((int)zero),((_tmpCA9.tag=1,((_tmpCA9.f1=(unsigned long)((int)zero),((
_tmpCAA.tag=1,((_tmpCAA.f1=(unsigned long)((int)zero),((_tmpCAB[0]=& _tmpCAA,((
_tmpCAB[1]=& _tmpCA9,((_tmpCAB[2]=& _tmpCA8,((_tmpCAB[3]=& _tmpCA7,((_tmpCAB[4]=&
_tmpCA6,Cyc_aprintf(((_tmpCAC="%c%c%c%c%s",_tag_dyneither(_tmpCAC,sizeof(char),
11))),_tag_dyneither(_tmpCAB,sizeof(void*),5)))))))))))))))))))))))))))))))),0);
struct Cyc_Absyn_Vardecl*_tmp81E=Cyc_Absyn_new_vardecl(f->name,_tmp7FD,(struct Cyc_Absyn_Exp*)
_tmp81D);(void*)(_tmp81E->sc=(void*)((void*)f->sc));{struct Cyc_Absyn_Var_d_struct*
_tmpCB2;struct Cyc_Absyn_Var_d_struct _tmpCB1;struct Cyc_List_List*_tmpCB0;Cyc_Toc_result_decls=((
_tmpCB0=_cycalloc(sizeof(*_tmpCB0)),((_tmpCB0->hd=Cyc_Absyn_new_decl((void*)((
_tmpCB2=_cycalloc(sizeof(*_tmpCB2)),((_tmpCB2[0]=((_tmpCB1.tag=0,((_tmpCB1.f1=
_tmp81E,_tmpCB1)))),_tmpCB2)))),0),((_tmpCB0->tl=Cyc_Toc_result_decls,_tmpCB0))))));}*
_tmp7F8=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,int v))Cyc_Dict_insert)(*
_tmp7F8,f->name,1);}goto _LL3B8;_LL3BD:;_LL3BE: goto _LL3B8;_LL3B8:;}};
_pop_dynregion(d);}}}static void Cyc_Toc_enumdecl_to_c(struct Cyc_Toc_Env*nv,struct
Cyc_Absyn_Enumdecl*ed);static void Cyc_Toc_enumdecl_to_c(struct Cyc_Toc_Env*nv,
struct Cyc_Absyn_Enumdecl*ed){(void*)(ed->sc=(void*)((void*)2));if(ed->fields != 0)
Cyc_Toc_enumfields_to_c((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(
ed->fields))->v);}static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*body_nv,
struct Cyc_Toc_Env*init_nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*s);
static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*body_nv,struct Cyc_Toc_Env*
init_nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*s){void*old_typ=(void*)
vd->type;(void*)(vd->type=(void*)Cyc_Toc_typ_to_c_array(old_typ));if((void*)vd->sc
== (void*)5  && Cyc_Tcutil_is_tagged_pointer_typ(old_typ))(void*)(vd->sc=(void*)((
void*)2));Cyc_Toc_stmt_to_c(body_nv,s);if(vd->initializer != 0){struct Cyc_Absyn_Exp*
init=(struct Cyc_Absyn_Exp*)_check_null(vd->initializer);void*_tmp829=(void*)init->r;
struct Cyc_Absyn_Vardecl*_tmp82A;struct Cyc_Absyn_Exp*_tmp82B;struct Cyc_Absyn_Exp*
_tmp82C;int _tmp82D;_LL3C0: if(*((int*)_tmp829)!= 29)goto _LL3C2;_tmp82A=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmp829)->f1;_tmp82B=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp829)->f2;_tmp82C=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp829)->f3;
_tmp82D=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp829)->f4;_LL3C1: vd->initializer=
0;(void*)(s->r=(void*)((void*)(Cyc_Toc_init_comprehension(init_nv,Cyc_Absyn_var_exp(
vd->name,0),_tmp82A,_tmp82B,_tmp82C,_tmp82D,Cyc_Absyn_new_stmt((void*)s->r,0),0))->r));
goto _LL3BF;_LL3C2:;_LL3C3: if((void*)vd->sc == (void*)0){struct Cyc_Toc_Env _tmp82F;
struct _RegionHandle*_tmp830;struct Cyc_Toc_Env*_tmp82E=init_nv;_tmp82F=*_tmp82E;
_tmp830=_tmp82F.rgn;{struct Cyc_Toc_Env*_tmp831=Cyc_Toc_set_toplevel(_tmp830,
init_nv);Cyc_Toc_exp_to_c(_tmp831,init);}}else{Cyc_Toc_exp_to_c(init_nv,init);}
goto _LL3BF;_LL3BF:;}else{void*_tmp832=Cyc_Tcutil_compress(old_typ);struct Cyc_Absyn_ArrayInfo
_tmp833;void*_tmp834;struct Cyc_Absyn_Exp*_tmp835;struct Cyc_Absyn_Conref*_tmp836;
_LL3C5: if(_tmp832 <= (void*)4)goto _LL3C7;if(*((int*)_tmp832)!= 7)goto _LL3C7;
_tmp833=((struct Cyc_Absyn_ArrayType_struct*)_tmp832)->f1;_tmp834=(void*)_tmp833.elt_type;
_tmp835=_tmp833.num_elts;_tmp836=_tmp833.zero_term;if(!((int(*)(int,struct Cyc_Absyn_Conref*
x))Cyc_Absyn_conref_def)(0,_tmp836))goto _LL3C7;_LL3C6: if(_tmp835 == 0){const char*
_tmpCB5;void*_tmpCB4;(_tmpCB4=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCB5="can't initialize zero-terminated array -- size unknown",
_tag_dyneither(_tmpCB5,sizeof(char),55))),_tag_dyneither(_tmpCB4,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*num_elts=(struct Cyc_Absyn_Exp*)_tmp835;struct Cyc_Absyn_Exp*
_tmp839=Cyc_Absyn_subscript_exp(Cyc_Absyn_var_exp(vd->name,0),Cyc_Absyn_add_exp(
num_elts,Cyc_Absyn_signed_int_exp(- 1,0),0),0);struct Cyc_Absyn_Exp*_tmp83A=Cyc_Absyn_signed_int_exp(
0,0);(void*)(s->r=(void*)Cyc_Toc_seq_stmt_r(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(
_tmp839,_tmp83A,0),0),Cyc_Absyn_new_stmt((void*)s->r,0)));goto _LL3C4;}_LL3C7:;
_LL3C8: goto _LL3C4;_LL3C4:;}}static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*p,void*t,struct Cyc_Absyn_Exp*e,struct
Cyc_Absyn_Stmt*s);static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*
nv,struct Cyc_Absyn_Pat*p,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s){
Cyc_Toc_exp_to_c(nv,e);{struct _tuple1*x=Cyc_Toc_temp_var();struct Cyc_Toc_Env
_tmp83C;struct _RegionHandle*_tmp83D;struct Cyc_Toc_Env*_tmp83B=nv;_tmp83C=*
_tmp83B;_tmp83D=_tmp83C.rgn;{struct Cyc_Absyn_Stmt*_tmp83E=Cyc_Toc_throw_match_stmt();
struct Cyc_Toc_Env*_tmp83F=Cyc_Toc_share_env(_tmp83D,nv);struct Cyc_Toc_Env*
_tmp841;struct Cyc_List_List*_tmp842;struct Cyc_Absyn_Stmt*_tmp843;struct _tuple13
_tmp840=Cyc_Toc_xlate_pat(_tmp83F,_tmp83D,t,Cyc_Absyn_var_exp(x,0),Cyc_Absyn_var_exp(
x,0),p,(struct Cyc_Absyn_Stmt**)& _tmp83E,Cyc_Toc_throw_match_stmt(),0);_tmp841=
_tmp840.f1;_tmp842=_tmp840.f2;_tmp843=_tmp840.f3;Cyc_Toc_stmt_to_c(_tmp841,s);s=
Cyc_Absyn_declare_stmt(x,Cyc_Toc_typ_to_c(t),(struct Cyc_Absyn_Exp*)e,Cyc_Absyn_seq_stmt(
_tmp843,s,0),0);for(0;_tmp842 != 0;_tmp842=_tmp842->tl){struct _tuple14 _tmp845;
struct _tuple1*_tmp846;void*_tmp847;struct _tuple14*_tmp844=(struct _tuple14*)
_tmp842->hd;_tmp845=*_tmp844;_tmp846=_tmp845.f1;_tmp847=_tmp845.f2;s=Cyc_Absyn_declare_stmt(
_tmp846,_tmp847,0,s,0);}}return s;}}static void Cyc_Toc_exptypes_to_c(struct Cyc_Absyn_Exp*
e);static void Cyc_Toc_exptypes_to_c(struct Cyc_Absyn_Exp*e){void*_tmp848=(void*)e->r;
struct Cyc_Absyn_Exp*_tmp849;struct Cyc_Absyn_Exp*_tmp84A;struct Cyc_Absyn_Exp*
_tmp84B;struct Cyc_Absyn_Exp*_tmp84C;struct Cyc_Absyn_Exp*_tmp84D;struct Cyc_Absyn_Exp*
_tmp84E;struct Cyc_Absyn_Exp*_tmp84F;struct Cyc_Absyn_Exp*_tmp850;struct Cyc_List_List*
_tmp851;struct Cyc_Absyn_Exp*_tmp852;struct Cyc_Absyn_Exp*_tmp853;struct Cyc_Absyn_Exp*
_tmp854;struct Cyc_Absyn_Exp*_tmp855;struct Cyc_Absyn_Exp*_tmp856;struct Cyc_Absyn_Exp*
_tmp857;struct Cyc_Absyn_Exp*_tmp858;struct Cyc_Absyn_Exp*_tmp859;struct Cyc_Absyn_Exp*
_tmp85A;struct Cyc_Absyn_Exp*_tmp85B;struct Cyc_Absyn_Exp*_tmp85C;struct Cyc_Absyn_Exp*
_tmp85D;struct Cyc_Absyn_Exp*_tmp85E;struct Cyc_Absyn_Exp*_tmp85F;struct Cyc_Absyn_Exp*
_tmp860;struct Cyc_Absyn_Exp*_tmp861;struct Cyc_List_List*_tmp862;struct Cyc_Absyn_Exp*
_tmp863;struct Cyc_List_List*_tmp864;void*_tmp865;void**_tmp866;struct Cyc_Absyn_Exp*
_tmp867;struct _tuple2*_tmp868;struct _tuple2 _tmp869;void*_tmp86A;void**_tmp86B;
struct Cyc_List_List*_tmp86C;struct Cyc_List_List*_tmp86D;struct Cyc_List_List*
_tmp86E;void*_tmp86F;void**_tmp870;void*_tmp871;void**_tmp872;struct Cyc_Absyn_Stmt*
_tmp873;struct Cyc_Absyn_MallocInfo _tmp874;struct Cyc_Absyn_MallocInfo*_tmp875;
_LL3CA: if(*((int*)_tmp848)!= 22)goto _LL3CC;_tmp849=((struct Cyc_Absyn_Deref_e_struct*)
_tmp848)->f1;_LL3CB: _tmp84A=_tmp849;goto _LL3CD;_LL3CC: if(*((int*)_tmp848)!= 23)
goto _LL3CE;_tmp84A=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp848)->f1;_LL3CD:
_tmp84B=_tmp84A;goto _LL3CF;_LL3CE: if(*((int*)_tmp848)!= 24)goto _LL3D0;_tmp84B=((
struct Cyc_Absyn_AggrArrow_e_struct*)_tmp848)->f1;_LL3CF: _tmp84C=_tmp84B;goto
_LL3D1;_LL3D0: if(*((int*)_tmp848)!= 16)goto _LL3D2;_tmp84C=((struct Cyc_Absyn_Address_e_struct*)
_tmp848)->f1;_LL3D1: _tmp84D=_tmp84C;goto _LL3D3;_LL3D2: if(*((int*)_tmp848)!= 12)
goto _LL3D4;_tmp84D=((struct Cyc_Absyn_Throw_e_struct*)_tmp848)->f1;_LL3D3: _tmp84E=
_tmp84D;goto _LL3D5;_LL3D4: if(*((int*)_tmp848)!= 13)goto _LL3D6;_tmp84E=((struct
Cyc_Absyn_NoInstantiate_e_struct*)_tmp848)->f1;_LL3D5: _tmp84F=_tmp84E;goto _LL3D7;
_LL3D6: if(*((int*)_tmp848)!= 19)goto _LL3D8;_tmp84F=((struct Cyc_Absyn_Sizeofexp_e_struct*)
_tmp848)->f1;_LL3D7: _tmp850=_tmp84F;goto _LL3D9;_LL3D8: if(*((int*)_tmp848)!= 5)
goto _LL3DA;_tmp850=((struct Cyc_Absyn_Increment_e_struct*)_tmp848)->f1;_LL3D9: Cyc_Toc_exptypes_to_c(
_tmp850);goto _LL3C9;_LL3DA: if(*((int*)_tmp848)!= 3)goto _LL3DC;_tmp851=((struct
Cyc_Absyn_Primop_e_struct*)_tmp848)->f2;_LL3DB:((void(*)(void(*f)(struct Cyc_Absyn_Exp*),
struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Toc_exptypes_to_c,_tmp851);goto _LL3C9;
_LL3DC: if(*((int*)_tmp848)!= 7)goto _LL3DE;_tmp852=((struct Cyc_Absyn_And_e_struct*)
_tmp848)->f1;_tmp853=((struct Cyc_Absyn_And_e_struct*)_tmp848)->f2;_LL3DD: _tmp854=
_tmp852;_tmp855=_tmp853;goto _LL3DF;_LL3DE: if(*((int*)_tmp848)!= 8)goto _LL3E0;
_tmp854=((struct Cyc_Absyn_Or_e_struct*)_tmp848)->f1;_tmp855=((struct Cyc_Absyn_Or_e_struct*)
_tmp848)->f2;_LL3DF: _tmp856=_tmp854;_tmp857=_tmp855;goto _LL3E1;_LL3E0: if(*((int*)
_tmp848)!= 9)goto _LL3E2;_tmp856=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp848)->f1;
_tmp857=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp848)->f2;_LL3E1: _tmp858=_tmp856;
_tmp859=_tmp857;goto _LL3E3;_LL3E2: if(*((int*)_tmp848)!= 25)goto _LL3E4;_tmp858=((
struct Cyc_Absyn_Subscript_e_struct*)_tmp848)->f1;_tmp859=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp848)->f2;_LL3E3: _tmp85A=_tmp858;_tmp85B=_tmp859;goto _LL3E5;_LL3E4: if(*((int*)
_tmp848)!= 36)goto _LL3E6;_tmp85A=((struct Cyc_Absyn_Swap_e_struct*)_tmp848)->f1;
_tmp85B=((struct Cyc_Absyn_Swap_e_struct*)_tmp848)->f2;_LL3E5: _tmp85C=_tmp85A;
_tmp85D=_tmp85B;goto _LL3E7;_LL3E6: if(*((int*)_tmp848)!= 4)goto _LL3E8;_tmp85C=((
struct Cyc_Absyn_AssignOp_e_struct*)_tmp848)->f1;_tmp85D=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmp848)->f3;_LL3E7: Cyc_Toc_exptypes_to_c(_tmp85C);Cyc_Toc_exptypes_to_c(_tmp85D);
goto _LL3C9;_LL3E8: if(*((int*)_tmp848)!= 6)goto _LL3EA;_tmp85E=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp848)->f1;_tmp85F=((struct Cyc_Absyn_Conditional_e_struct*)_tmp848)->f2;
_tmp860=((struct Cyc_Absyn_Conditional_e_struct*)_tmp848)->f3;_LL3E9: Cyc_Toc_exptypes_to_c(
_tmp85E);Cyc_Toc_exptypes_to_c(_tmp85F);Cyc_Toc_exptypes_to_c(_tmp860);goto
_LL3C9;_LL3EA: if(*((int*)_tmp848)!= 11)goto _LL3EC;_tmp861=((struct Cyc_Absyn_FnCall_e_struct*)
_tmp848)->f1;_tmp862=((struct Cyc_Absyn_FnCall_e_struct*)_tmp848)->f2;_LL3EB:
_tmp863=_tmp861;_tmp864=_tmp862;goto _LL3ED;_LL3EC: if(*((int*)_tmp848)!= 10)goto
_LL3EE;_tmp863=((struct Cyc_Absyn_UnknownCall_e_struct*)_tmp848)->f1;_tmp864=((
struct Cyc_Absyn_UnknownCall_e_struct*)_tmp848)->f2;_LL3ED: Cyc_Toc_exptypes_to_c(
_tmp863);((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(
Cyc_Toc_exptypes_to_c,_tmp864);goto _LL3C9;_LL3EE: if(*((int*)_tmp848)!= 15)goto
_LL3F0;_tmp865=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp848)->f1;_tmp866=(
void**)&((void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp848)->f1);_tmp867=((struct
Cyc_Absyn_Cast_e_struct*)_tmp848)->f2;_LL3EF:*_tmp866=Cyc_Toc_typ_to_c(*_tmp866);
Cyc_Toc_exptypes_to_c(_tmp867);goto _LL3C9;_LL3F0: if(*((int*)_tmp848)!= 27)goto
_LL3F2;_tmp868=((struct Cyc_Absyn_CompoundLit_e_struct*)_tmp848)->f1;_tmp869=*
_tmp868;_tmp86A=_tmp869.f3;_tmp86B=(void**)&(*((struct Cyc_Absyn_CompoundLit_e_struct*)
_tmp848)->f1).f3;_tmp86C=((struct Cyc_Absyn_CompoundLit_e_struct*)_tmp848)->f2;
_LL3F1:*_tmp86B=Cyc_Toc_typ_to_c(*_tmp86B);_tmp86D=_tmp86C;goto _LL3F3;_LL3F2: if(*((
int*)_tmp848)!= 37)goto _LL3F4;_tmp86D=((struct Cyc_Absyn_UnresolvedMem_e_struct*)
_tmp848)->f2;_LL3F3: _tmp86E=_tmp86D;goto _LL3F5;_LL3F4: if(*((int*)_tmp848)!= 28)
goto _LL3F6;_tmp86E=((struct Cyc_Absyn_Array_e_struct*)_tmp848)->f1;_LL3F5: for(0;
_tmp86E != 0;_tmp86E=_tmp86E->tl){struct Cyc_Absyn_Exp*_tmp877;struct _tuple8
_tmp876=*((struct _tuple8*)_tmp86E->hd);_tmp877=_tmp876.f2;Cyc_Toc_exptypes_to_c(
_tmp877);}goto _LL3C9;_LL3F6: if(*((int*)_tmp848)!= 20)goto _LL3F8;_tmp86F=(void*)((
struct Cyc_Absyn_Offsetof_e_struct*)_tmp848)->f1;_tmp870=(void**)&((void*)((
struct Cyc_Absyn_Offsetof_e_struct*)_tmp848)->f1);_LL3F7: _tmp872=_tmp870;goto
_LL3F9;_LL3F8: if(*((int*)_tmp848)!= 18)goto _LL3FA;_tmp871=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp848)->f1;_tmp872=(void**)&((void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp848)->f1);_LL3F9:*_tmp872=Cyc_Toc_typ_to_c(*_tmp872);goto _LL3C9;_LL3FA: if(*((
int*)_tmp848)!= 38)goto _LL3FC;_tmp873=((struct Cyc_Absyn_StmtExp_e_struct*)
_tmp848)->f1;_LL3FB: Cyc_Toc_stmttypes_to_c(_tmp873);goto _LL3C9;_LL3FC: if(*((int*)
_tmp848)!= 35)goto _LL3FE;_tmp874=((struct Cyc_Absyn_Malloc_e_struct*)_tmp848)->f1;
_tmp875=(struct Cyc_Absyn_MallocInfo*)&((struct Cyc_Absyn_Malloc_e_struct*)_tmp848)->f1;
_LL3FD: if(_tmp875->elt_type != 0){void**_tmpCB6;_tmp875->elt_type=((_tmpCB6=
_cycalloc(sizeof(*_tmpCB6)),((_tmpCB6[0]=Cyc_Toc_typ_to_c(*((void**)_check_null(
_tmp875->elt_type))),_tmpCB6))));}Cyc_Toc_exptypes_to_c(_tmp875->num_elts);goto
_LL3C9;_LL3FE: if(*((int*)_tmp848)!= 0)goto _LL400;_LL3FF: goto _LL401;_LL400: if(*((
int*)_tmp848)!= 1)goto _LL402;_LL401: goto _LL403;_LL402: if(*((int*)_tmp848)!= 2)
goto _LL404;_LL403: goto _LL405;_LL404: if(*((int*)_tmp848)!= 33)goto _LL406;_LL405:
goto _LL407;_LL406: if(*((int*)_tmp848)!= 34)goto _LL408;_LL407: goto _LL3C9;_LL408:
if(*((int*)_tmp848)!= 31)goto _LL40A;_LL409: goto _LL40B;_LL40A: if(*((int*)_tmp848)
!= 32)goto _LL40C;_LL40B: goto _LL40D;_LL40C: if(*((int*)_tmp848)!= 30)goto _LL40E;
_LL40D: goto _LL40F;_LL40E: if(*((int*)_tmp848)!= 29)goto _LL410;_LL40F: goto _LL411;
_LL410: if(*((int*)_tmp848)!= 26)goto _LL412;_LL411: goto _LL413;_LL412: if(*((int*)
_tmp848)!= 14)goto _LL414;_LL413: goto _LL415;_LL414: if(*((int*)_tmp848)!= 17)goto
_LL416;_LL415: goto _LL417;_LL416: if(*((int*)_tmp848)!= 39)goto _LL418;_LL417: goto
_LL419;_LL418: if(*((int*)_tmp848)!= 21)goto _LL3C9;_LL419:{const char*_tmpCB9;void*
_tmpCB8;(_tmpCB8=0,Cyc_Tcutil_terr(e->loc,((_tmpCB9="Cyclone expression within C code",
_tag_dyneither(_tmpCB9,sizeof(char),33))),_tag_dyneither(_tmpCB8,sizeof(void*),0)));}
goto _LL3C9;_LL3C9:;}static void Cyc_Toc_decltypes_to_c(struct Cyc_Absyn_Decl*d);
static void Cyc_Toc_decltypes_to_c(struct Cyc_Absyn_Decl*d){void*_tmp87B=(void*)d->r;
struct Cyc_Absyn_Vardecl*_tmp87C;struct Cyc_Absyn_Fndecl*_tmp87D;struct Cyc_Absyn_Aggrdecl*
_tmp87E;struct Cyc_Absyn_Enumdecl*_tmp87F;struct Cyc_Absyn_Typedefdecl*_tmp880;
_LL41B: if(_tmp87B <= (void*)2)goto _LL437;if(*((int*)_tmp87B)!= 0)goto _LL41D;
_tmp87C=((struct Cyc_Absyn_Var_d_struct*)_tmp87B)->f1;_LL41C:(void*)(_tmp87C->type=(
void*)Cyc_Toc_typ_to_c((void*)_tmp87C->type));if(_tmp87C->initializer != 0)Cyc_Toc_exptypes_to_c((
struct Cyc_Absyn_Exp*)_check_null(_tmp87C->initializer));goto _LL41A;_LL41D: if(*((
int*)_tmp87B)!= 1)goto _LL41F;_tmp87D=((struct Cyc_Absyn_Fn_d_struct*)_tmp87B)->f1;
_LL41E:(void*)(_tmp87D->ret_type=(void*)Cyc_Toc_typ_to_c((void*)_tmp87D->ret_type));{
struct Cyc_List_List*_tmp881=_tmp87D->args;for(0;_tmp881 != 0;_tmp881=_tmp881->tl){(*((
struct _tuple17*)_tmp881->hd)).f3=Cyc_Toc_typ_to_c((*((struct _tuple17*)_tmp881->hd)).f3);}}
goto _LL41A;_LL41F: if(*((int*)_tmp87B)!= 6)goto _LL421;_tmp87E=((struct Cyc_Absyn_Aggr_d_struct*)
_tmp87B)->f1;_LL420: Cyc_Toc_aggrdecl_to_c(_tmp87E);goto _LL41A;_LL421: if(*((int*)
_tmp87B)!= 8)goto _LL423;_tmp87F=((struct Cyc_Absyn_Enum_d_struct*)_tmp87B)->f1;
_LL422: if(_tmp87F->fields != 0){struct Cyc_List_List*_tmp882=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp87F->fields))->v;for(0;_tmp882 != 0;_tmp882=
_tmp882->tl){struct Cyc_Absyn_Enumfield*_tmp883=(struct Cyc_Absyn_Enumfield*)
_tmp882->hd;if(_tmp883->tag != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)
_check_null(_tmp883->tag));}}goto _LL41A;_LL423: if(*((int*)_tmp87B)!= 9)goto
_LL425;_tmp880=((struct Cyc_Absyn_Typedef_d_struct*)_tmp87B)->f1;_LL424:{struct
Cyc_Core_Opt*_tmpCBA;_tmp880->defn=((_tmpCBA=_cycalloc(sizeof(*_tmpCBA)),((
_tmpCBA->v=(void*)Cyc_Toc_typ_to_c_array((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp880->defn))->v),_tmpCBA))));}goto _LL41A;_LL425: if(*((int*)
_tmp87B)!= 2)goto _LL427;_LL426: goto _LL428;_LL427: if(*((int*)_tmp87B)!= 3)goto
_LL429;_LL428: goto _LL42A;_LL429: if(*((int*)_tmp87B)!= 7)goto _LL42B;_LL42A: goto
_LL42C;_LL42B: if(*((int*)_tmp87B)!= 10)goto _LL42D;_LL42C: goto _LL42E;_LL42D: if(*((
int*)_tmp87B)!= 11)goto _LL42F;_LL42E: goto _LL430;_LL42F: if(*((int*)_tmp87B)!= 12)
goto _LL431;_LL430: goto _LL432;_LL431: if(*((int*)_tmp87B)!= 13)goto _LL433;_LL432:
goto _LL434;_LL433: if(*((int*)_tmp87B)!= 4)goto _LL435;_LL434: goto _LL436;_LL435:
if(*((int*)_tmp87B)!= 5)goto _LL437;_LL436:{const char*_tmpCBD;void*_tmpCBC;(
_tmpCBC=0,Cyc_Tcutil_terr(d->loc,((_tmpCBD="Cyclone declaration within C code",
_tag_dyneither(_tmpCBD,sizeof(char),34))),_tag_dyneither(_tmpCBC,sizeof(void*),0)));}
goto _LL41A;_LL437: if((int)_tmp87B != 0)goto _LL439;_LL438: goto _LL43A;_LL439: if((
int)_tmp87B != 1)goto _LL41A;_LL43A: goto _LL41A;_LL41A:;}static void Cyc_Toc_stmttypes_to_c(
struct Cyc_Absyn_Stmt*s);static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*s){
void*_tmp887=(void*)s->r;struct Cyc_Absyn_Exp*_tmp888;struct Cyc_Absyn_Stmt*
_tmp889;struct Cyc_Absyn_Stmt*_tmp88A;struct Cyc_Absyn_Exp*_tmp88B;struct Cyc_Absyn_Exp*
_tmp88C;struct Cyc_Absyn_Stmt*_tmp88D;struct Cyc_Absyn_Stmt*_tmp88E;struct _tuple3
_tmp88F;struct Cyc_Absyn_Exp*_tmp890;struct Cyc_Absyn_Stmt*_tmp891;struct Cyc_Absyn_Exp*
_tmp892;struct _tuple3 _tmp893;struct Cyc_Absyn_Exp*_tmp894;struct _tuple3 _tmp895;
struct Cyc_Absyn_Exp*_tmp896;struct Cyc_Absyn_Stmt*_tmp897;struct Cyc_Absyn_Exp*
_tmp898;struct Cyc_List_List*_tmp899;struct Cyc_Absyn_Decl*_tmp89A;struct Cyc_Absyn_Stmt*
_tmp89B;struct Cyc_Absyn_Stmt*_tmp89C;struct _tuple3 _tmp89D;struct Cyc_Absyn_Exp*
_tmp89E;_LL43C: if(_tmp887 <= (void*)1)goto _LL44E;if(*((int*)_tmp887)!= 0)goto
_LL43E;_tmp888=((struct Cyc_Absyn_Exp_s_struct*)_tmp887)->f1;_LL43D: Cyc_Toc_exptypes_to_c(
_tmp888);goto _LL43B;_LL43E: if(*((int*)_tmp887)!= 1)goto _LL440;_tmp889=((struct
Cyc_Absyn_Seq_s_struct*)_tmp887)->f1;_tmp88A=((struct Cyc_Absyn_Seq_s_struct*)
_tmp887)->f2;_LL43F: Cyc_Toc_stmttypes_to_c(_tmp889);Cyc_Toc_stmttypes_to_c(
_tmp88A);goto _LL43B;_LL440: if(*((int*)_tmp887)!= 2)goto _LL442;_tmp88B=((struct
Cyc_Absyn_Return_s_struct*)_tmp887)->f1;_LL441: if(_tmp88B != 0)Cyc_Toc_exptypes_to_c((
struct Cyc_Absyn_Exp*)_tmp88B);goto _LL43B;_LL442: if(*((int*)_tmp887)!= 3)goto
_LL444;_tmp88C=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmp887)->f1;_tmp88D=((
struct Cyc_Absyn_IfThenElse_s_struct*)_tmp887)->f2;_tmp88E=((struct Cyc_Absyn_IfThenElse_s_struct*)
_tmp887)->f3;_LL443: Cyc_Toc_exptypes_to_c(_tmp88C);Cyc_Toc_stmttypes_to_c(
_tmp88D);Cyc_Toc_stmttypes_to_c(_tmp88E);goto _LL43B;_LL444: if(*((int*)_tmp887)!= 
4)goto _LL446;_tmp88F=((struct Cyc_Absyn_While_s_struct*)_tmp887)->f1;_tmp890=
_tmp88F.f1;_tmp891=((struct Cyc_Absyn_While_s_struct*)_tmp887)->f2;_LL445: Cyc_Toc_exptypes_to_c(
_tmp890);Cyc_Toc_stmttypes_to_c(_tmp891);goto _LL43B;_LL446: if(*((int*)_tmp887)!= 
8)goto _LL448;_tmp892=((struct Cyc_Absyn_For_s_struct*)_tmp887)->f1;_tmp893=((
struct Cyc_Absyn_For_s_struct*)_tmp887)->f2;_tmp894=_tmp893.f1;_tmp895=((struct
Cyc_Absyn_For_s_struct*)_tmp887)->f3;_tmp896=_tmp895.f1;_tmp897=((struct Cyc_Absyn_For_s_struct*)
_tmp887)->f4;_LL447: Cyc_Toc_exptypes_to_c(_tmp892);Cyc_Toc_exptypes_to_c(_tmp894);
Cyc_Toc_exptypes_to_c(_tmp896);Cyc_Toc_stmttypes_to_c(_tmp897);goto _LL43B;_LL448:
if(*((int*)_tmp887)!= 9)goto _LL44A;_tmp898=((struct Cyc_Absyn_Switch_s_struct*)
_tmp887)->f1;_tmp899=((struct Cyc_Absyn_Switch_s_struct*)_tmp887)->f2;_LL449: Cyc_Toc_exptypes_to_c(
_tmp898);for(0;_tmp899 != 0;_tmp899=_tmp899->tl){Cyc_Toc_stmttypes_to_c(((struct
Cyc_Absyn_Switch_clause*)_tmp899->hd)->body);}goto _LL43B;_LL44A: if(*((int*)
_tmp887)!= 11)goto _LL44C;_tmp89A=((struct Cyc_Absyn_Decl_s_struct*)_tmp887)->f1;
_tmp89B=((struct Cyc_Absyn_Decl_s_struct*)_tmp887)->f2;_LL44B: Cyc_Toc_decltypes_to_c(
_tmp89A);Cyc_Toc_stmttypes_to_c(_tmp89B);goto _LL43B;_LL44C: if(*((int*)_tmp887)!= 
13)goto _LL44E;_tmp89C=((struct Cyc_Absyn_Do_s_struct*)_tmp887)->f1;_tmp89D=((
struct Cyc_Absyn_Do_s_struct*)_tmp887)->f2;_tmp89E=_tmp89D.f1;_LL44D: Cyc_Toc_stmttypes_to_c(
_tmp89C);Cyc_Toc_exptypes_to_c(_tmp89E);goto _LL43B;_LL44E: if((int)_tmp887 != 0)
goto _LL450;_LL44F: goto _LL451;_LL450: if(_tmp887 <= (void*)1)goto _LL452;if(*((int*)
_tmp887)!= 5)goto _LL452;_LL451: goto _LL453;_LL452: if(_tmp887 <= (void*)1)goto
_LL454;if(*((int*)_tmp887)!= 6)goto _LL454;_LL453: goto _LL455;_LL454: if(_tmp887 <= (
void*)1)goto _LL456;if(*((int*)_tmp887)!= 7)goto _LL456;_LL455: goto _LL43B;_LL456:
if(_tmp887 <= (void*)1)goto _LL458;if(*((int*)_tmp887)!= 10)goto _LL458;_LL457: goto
_LL459;_LL458: if(_tmp887 <= (void*)1)goto _LL45A;if(*((int*)_tmp887)!= 12)goto
_LL45A;_LL459: goto _LL45B;_LL45A: if(_tmp887 <= (void*)1)goto _LL45C;if(*((int*)
_tmp887)!= 14)goto _LL45C;_LL45B: goto _LL45D;_LL45C: if(_tmp887 <= (void*)1)goto
_LL43B;if(*((int*)_tmp887)!= 15)goto _LL43B;_LL45D:{const char*_tmpCC0;void*
_tmpCBF;(_tmpCBF=0,Cyc_Tcutil_terr(s->loc,((_tmpCC0="Cyclone statement in C code",
_tag_dyneither(_tmpCC0,sizeof(char),28))),_tag_dyneither(_tmpCBF,sizeof(void*),0)));}
goto _LL43B;_LL43B:;}static struct Cyc_Toc_Env*Cyc_Toc_decls_to_c(struct
_RegionHandle*r,struct Cyc_Toc_Env*nv,struct Cyc_List_List*ds,int top,int cinclude);
static struct Cyc_Toc_Env*Cyc_Toc_decls_to_c(struct _RegionHandle*r,struct Cyc_Toc_Env*
nv,struct Cyc_List_List*ds,int top,int cinclude){for(0;ds != 0;ds=ds->tl){if(!Cyc_Toc_is_toplevel(
nv)){const char*_tmpCC3;void*_tmpCC2;(_tmpCC2=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpCC3="decls_to_c: not at toplevel!",
_tag_dyneither(_tmpCC3,sizeof(char),29))),_tag_dyneither(_tmpCC2,sizeof(void*),0)));}{
struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)ds->hd;void*_tmp8A3=(void*)d->r;
struct Cyc_Absyn_Vardecl*_tmp8A4;struct Cyc_Absyn_Fndecl*_tmp8A5;struct Cyc_Absyn_Aggrdecl*
_tmp8A6;struct Cyc_Absyn_Datatypedecl*_tmp8A7;struct Cyc_Absyn_Enumdecl*_tmp8A8;
struct Cyc_Absyn_Typedefdecl*_tmp8A9;struct Cyc_List_List*_tmp8AA;struct Cyc_List_List*
_tmp8AB;struct Cyc_List_List*_tmp8AC;struct Cyc_List_List*_tmp8AD;_LL45F: if(
_tmp8A3 <= (void*)2)goto _LL473;if(*((int*)_tmp8A3)!= 0)goto _LL461;_tmp8A4=((
struct Cyc_Absyn_Var_d_struct*)_tmp8A3)->f1;_LL460: {struct _tuple1*_tmp8AE=
_tmp8A4->name;if((void*)_tmp8A4->sc == (void*)4){union Cyc_Absyn_Nmspace_union
_tmpCC6;struct _tuple1*_tmpCC5;_tmp8AE=((_tmpCC5=_cycalloc(sizeof(*_tmpCC5)),((
_tmpCC5->f1=(union Cyc_Absyn_Nmspace_union)(((_tmpCC6.Rel_n).tag=1,(((_tmpCC6.Rel_n).f1=
0,_tmpCC6)))),((_tmpCC5->f2=(*_tmp8AE).f2,_tmpCC5))))));}if(_tmp8A4->initializer
!= 0){if(cinclude)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(
_tmp8A4->initializer));else{Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)
_check_null(_tmp8A4->initializer));}}{struct Cyc_Absyn_Global_b_struct _tmpCC9;
struct Cyc_Absyn_Global_b_struct*_tmpCC8;nv=Cyc_Toc_add_varmap(r,nv,_tmp8A4->name,
Cyc_Absyn_varb_exp(_tmp8AE,(void*)((_tmpCC8=_cycalloc(sizeof(*_tmpCC8)),((
_tmpCC8[0]=((_tmpCC9.tag=0,((_tmpCC9.f1=_tmp8A4,_tmpCC9)))),_tmpCC8)))),0));}
_tmp8A4->name=_tmp8AE;(void*)(_tmp8A4->sc=(void*)Cyc_Toc_scope_to_c((void*)
_tmp8A4->sc));(void*)(_tmp8A4->type=(void*)Cyc_Toc_typ_to_c_array((void*)_tmp8A4->type));{
struct Cyc_List_List*_tmpCCA;Cyc_Toc_result_decls=((_tmpCCA=_cycalloc(sizeof(*
_tmpCCA)),((_tmpCCA->hd=d,((_tmpCCA->tl=Cyc_Toc_result_decls,_tmpCCA))))));}goto
_LL45E;}_LL461: if(*((int*)_tmp8A3)!= 1)goto _LL463;_tmp8A5=((struct Cyc_Absyn_Fn_d_struct*)
_tmp8A3)->f1;_LL462: {struct _tuple1*_tmp8B4=_tmp8A5->name;if((void*)_tmp8A5->sc
== (void*)4){{union Cyc_Absyn_Nmspace_union _tmpCCD;struct _tuple1*_tmpCCC;_tmp8B4=((
_tmpCCC=_cycalloc(sizeof(*_tmpCCC)),((_tmpCCC->f1=(union Cyc_Absyn_Nmspace_union)(((
_tmpCCD.Rel_n).tag=1,(((_tmpCCD.Rel_n).f1=0,_tmpCCD)))),((_tmpCCC->f2=(*_tmp8B4).f2,
_tmpCCC))))));}(void*)(_tmp8A5->sc=(void*)((void*)2));}nv=Cyc_Toc_add_varmap(r,
nv,_tmp8A5->name,Cyc_Absyn_var_exp(_tmp8B4,0));_tmp8A5->name=_tmp8B4;Cyc_Toc_fndecl_to_c(
nv,_tmp8A5,cinclude);{struct Cyc_List_List*_tmpCCE;Cyc_Toc_result_decls=((_tmpCCE=
_cycalloc(sizeof(*_tmpCCE)),((_tmpCCE->hd=d,((_tmpCCE->tl=Cyc_Toc_result_decls,
_tmpCCE))))));}goto _LL45E;}_LL463: if(*((int*)_tmp8A3)!= 2)goto _LL465;_LL464: goto
_LL466;_LL465: if(*((int*)_tmp8A3)!= 3)goto _LL467;_LL466: {const char*_tmpCD1;void*
_tmpCD0;(_tmpCD0=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpCD1="letdecl at toplevel",_tag_dyneither(_tmpCD1,sizeof(char),20))),
_tag_dyneither(_tmpCD0,sizeof(void*),0)));}_LL467: if(*((int*)_tmp8A3)!= 4)goto
_LL469;_LL468: {const char*_tmpCD4;void*_tmpCD3;(_tmpCD3=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCD4="region decl at toplevel",
_tag_dyneither(_tmpCD4,sizeof(char),24))),_tag_dyneither(_tmpCD3,sizeof(void*),0)));}
_LL469: if(*((int*)_tmp8A3)!= 5)goto _LL46B;_LL46A: {const char*_tmpCD7;void*
_tmpCD6;(_tmpCD6=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpCD7="alias decl at toplevel",_tag_dyneither(_tmpCD7,sizeof(char),23))),
_tag_dyneither(_tmpCD6,sizeof(void*),0)));}_LL46B: if(*((int*)_tmp8A3)!= 6)goto
_LL46D;_tmp8A6=((struct Cyc_Absyn_Aggr_d_struct*)_tmp8A3)->f1;_LL46C: Cyc_Toc_aggrdecl_to_c(
_tmp8A6);{struct Cyc_List_List*_tmpCD8;Cyc_Toc_result_decls=((_tmpCD8=_cycalloc(
sizeof(*_tmpCD8)),((_tmpCD8->hd=d,((_tmpCD8->tl=Cyc_Toc_result_decls,_tmpCD8))))));}
goto _LL45E;_LL46D: if(*((int*)_tmp8A3)!= 7)goto _LL46F;_tmp8A7=((struct Cyc_Absyn_Datatype_d_struct*)
_tmp8A3)->f1;_LL46E: if(_tmp8A7->is_extensible)Cyc_Toc_xdatatypedecl_to_c(_tmp8A7);
else{Cyc_Toc_datatypedecl_to_c(_tmp8A7);}goto _LL45E;_LL46F: if(*((int*)_tmp8A3)!= 
8)goto _LL471;_tmp8A8=((struct Cyc_Absyn_Enum_d_struct*)_tmp8A3)->f1;_LL470: Cyc_Toc_enumdecl_to_c(
nv,_tmp8A8);{struct Cyc_List_List*_tmpCD9;Cyc_Toc_result_decls=((_tmpCD9=
_cycalloc(sizeof(*_tmpCD9)),((_tmpCD9->hd=d,((_tmpCD9->tl=Cyc_Toc_result_decls,
_tmpCD9))))));}goto _LL45E;_LL471: if(*((int*)_tmp8A3)!= 9)goto _LL473;_tmp8A9=((
struct Cyc_Absyn_Typedef_d_struct*)_tmp8A3)->f1;_LL472: _tmp8A9->name=_tmp8A9->name;
_tmp8A9->tvs=0;if(_tmp8A9->defn != 0){struct Cyc_Core_Opt*_tmpCDA;_tmp8A9->defn=((
_tmpCDA=_cycalloc(sizeof(*_tmpCDA)),((_tmpCDA->v=(void*)Cyc_Toc_typ_to_c_array((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp8A9->defn))->v),_tmpCDA))));}else{
void*_tmp8C1=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp8A9->kind))->v;_LL480:
if((int)_tmp8C1 != 2)goto _LL482;_LL481:{struct Cyc_Core_Opt*_tmpCDB;_tmp8A9->defn=((
_tmpCDB=_cycalloc(sizeof(*_tmpCDB)),((_tmpCDB->v=(void*)Cyc_Absyn_void_star_typ(),
_tmpCDB))));}goto _LL47F;_LL482:;_LL483:{struct Cyc_Core_Opt*_tmpCDC;_tmp8A9->defn=((
_tmpCDC=_cycalloc(sizeof(*_tmpCDC)),((_tmpCDC->v=(void*)((void*)0),_tmpCDC))));}
goto _LL47F;_LL47F:;}{struct Cyc_List_List*_tmpCDD;Cyc_Toc_result_decls=((_tmpCDD=
_cycalloc(sizeof(*_tmpCDD)),((_tmpCDD->hd=d,((_tmpCDD->tl=Cyc_Toc_result_decls,
_tmpCDD))))));}goto _LL45E;_LL473: if((int)_tmp8A3 != 0)goto _LL475;_LL474: goto
_LL476;_LL475: if((int)_tmp8A3 != 1)goto _LL477;_LL476: goto _LL45E;_LL477: if(_tmp8A3
<= (void*)2)goto _LL479;if(*((int*)_tmp8A3)!= 10)goto _LL479;_tmp8AA=((struct Cyc_Absyn_Namespace_d_struct*)
_tmp8A3)->f2;_LL478: _tmp8AB=_tmp8AA;goto _LL47A;_LL479: if(_tmp8A3 <= (void*)2)goto
_LL47B;if(*((int*)_tmp8A3)!= 11)goto _LL47B;_tmp8AB=((struct Cyc_Absyn_Using_d_struct*)
_tmp8A3)->f2;_LL47A: _tmp8AC=_tmp8AB;goto _LL47C;_LL47B: if(_tmp8A3 <= (void*)2)goto
_LL47D;if(*((int*)_tmp8A3)!= 12)goto _LL47D;_tmp8AC=((struct Cyc_Absyn_ExternC_d_struct*)
_tmp8A3)->f1;_LL47C: nv=Cyc_Toc_decls_to_c(r,nv,_tmp8AC,top,cinclude);goto _LL45E;
_LL47D: if(_tmp8A3 <= (void*)2)goto _LL45E;if(*((int*)_tmp8A3)!= 13)goto _LL45E;
_tmp8AD=((struct Cyc_Absyn_ExternCinclude_d_struct*)_tmp8A3)->f1;_LL47E: nv=Cyc_Toc_decls_to_c(
r,nv,_tmp8AD,top,1);goto _LL45E;_LL45E:;}}return nv;}static void Cyc_Toc_init();
static void Cyc_Toc_init(){struct _DynRegionHandle*_tmp8C6;struct Cyc_Core_NewRegion
_tmp8C5=Cyc_Core_new_dynregion();_tmp8C6=_tmp8C5.dynregion;{struct
_DynRegionFrame _tmp8C7;struct _RegionHandle*d=_open_dynregion(& _tmp8C7,_tmp8C6);{
struct Cyc_List_List**_tmpCE8;struct Cyc_Dict_Dict*_tmpCE7;struct Cyc_Set_Set**
_tmpCE6;struct Cyc_Dict_Dict*_tmpCE5;struct Cyc_Dict_Dict*_tmpCE4;struct Cyc_Toc_TocState*
_tmpCE3;Cyc_Toc_toc_state=((_tmpCE3=_cycalloc(sizeof(*_tmpCE3)),((_tmpCE3->dyn=(
struct _DynRegionHandle*)_tmp8C6,((_tmpCE3->tuple_types=(struct Cyc_List_List**)((
_tmpCE8=_region_malloc(d,sizeof(*_tmpCE8)),((_tmpCE8[0]=0,_tmpCE8)))),((_tmpCE3->aggrs_so_far=(
struct Cyc_Dict_Dict*)((_tmpCE7=_region_malloc(d,sizeof(*_tmpCE7)),((_tmpCE7[0]=((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple1*,struct
_tuple1*)))Cyc_Dict_rempty)(d,Cyc_Absyn_qvar_cmp),_tmpCE7)))),((_tmpCE3->datatypes_so_far=(
struct Cyc_Set_Set**)((_tmpCE6=_region_malloc(d,sizeof(*_tmpCE6)),((_tmpCE6[0]=((
struct Cyc_Set_Set*(*)(struct _RegionHandle*r,int(*cmp)(struct _tuple1*,struct
_tuple1*)))Cyc_Set_rempty)(d,Cyc_Absyn_qvar_cmp),_tmpCE6)))),((_tmpCE3->xdatatypes_so_far=(
struct Cyc_Dict_Dict*)((_tmpCE5=_region_malloc(d,sizeof(*_tmpCE5)),((_tmpCE5[0]=((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple1*,struct
_tuple1*)))Cyc_Dict_rempty)(d,Cyc_Absyn_qvar_cmp),_tmpCE5)))),((_tmpCE3->qvar_tags=(
struct Cyc_Dict_Dict*)((_tmpCE4=_region_malloc(d,sizeof(*_tmpCE4)),((_tmpCE4[0]=((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple6*,struct
_tuple6*)))Cyc_Dict_rempty)(d,Cyc_Toc_qvar_tag_cmp),_tmpCE4)))),((_tmpCE3->temp_labels=(
struct Cyc_Xarray_Xarray*)((struct Cyc_Xarray_Xarray*(*)(struct _RegionHandle*))Cyc_Xarray_rcreate_empty)(
d),_tmpCE3))))))))))))))));}Cyc_Toc_result_decls=0;Cyc_Toc_tuple_type_counter=0;
Cyc_Toc_temp_var_counter=0;Cyc_Toc_fresh_label_counter=0;Cyc_Toc_total_bounds_checks=
0;Cyc_Toc_bounds_checks_eliminated=0;{struct _dyneither_ptr**_tmpCE9;Cyc_Toc_globals=
_tag_dyneither(((_tmpCE9=_cycalloc(sizeof(struct _dyneither_ptr*)* 38),((_tmpCE9[
0]=& Cyc_Toc__throw_str,((_tmpCE9[1]=& Cyc_Toc_setjmp_str,((_tmpCE9[2]=& Cyc_Toc__push_handler_str,((
_tmpCE9[3]=& Cyc_Toc__pop_handler_str,((_tmpCE9[4]=& Cyc_Toc__exn_thrown_str,((
_tmpCE9[5]=& Cyc_Toc__npop_handler_str,((_tmpCE9[6]=& Cyc_Toc__check_null_str,((
_tmpCE9[7]=& Cyc_Toc__check_known_subscript_null_str,((_tmpCE9[8]=& Cyc_Toc__check_known_subscript_notnull_str,((
_tmpCE9[9]=& Cyc_Toc__check_dyneither_subscript_str,((_tmpCE9[10]=& Cyc_Toc__dyneither_ptr_str,((
_tmpCE9[11]=& Cyc_Toc__tag_dyneither_str,((_tmpCE9[12]=& Cyc_Toc__init_dyneither_ptr_str,((
_tmpCE9[13]=& Cyc_Toc__untag_dyneither_ptr_str,((_tmpCE9[14]=& Cyc_Toc__get_dyneither_size_str,((
_tmpCE9[15]=& Cyc_Toc__get_zero_arr_size_str,((_tmpCE9[16]=& Cyc_Toc__dyneither_ptr_plus_str,((
_tmpCE9[17]=& Cyc_Toc__zero_arr_plus_str,((_tmpCE9[18]=& Cyc_Toc__dyneither_ptr_inplace_plus_str,((
_tmpCE9[19]=& Cyc_Toc__zero_arr_inplace_plus_str,((_tmpCE9[20]=& Cyc_Toc__dyneither_ptr_inplace_plus_post_str,((
_tmpCE9[21]=& Cyc_Toc__zero_arr_inplace_plus_post_str,((_tmpCE9[22]=& Cyc_Toc__cycalloc_str,((
_tmpCE9[23]=& Cyc_Toc__cyccalloc_str,((_tmpCE9[24]=& Cyc_Toc__cycalloc_atomic_str,((
_tmpCE9[25]=& Cyc_Toc__cyccalloc_atomic_str,((_tmpCE9[26]=& Cyc_Toc__region_malloc_str,((
_tmpCE9[27]=& Cyc_Toc__region_calloc_str,((_tmpCE9[28]=& Cyc_Toc__check_times_str,((
_tmpCE9[29]=& Cyc_Toc__new_region_str,((_tmpCE9[30]=& Cyc_Toc__push_region_str,((
_tmpCE9[31]=& Cyc_Toc__pop_region_str,((_tmpCE9[32]=& Cyc_Toc__open_dynregion_str,((
_tmpCE9[33]=& Cyc_Toc__push_dynregion_str,((_tmpCE9[34]=& Cyc_Toc__pop_dynregion_str,((
_tmpCE9[35]=& Cyc_Toc__reset_region_str,((_tmpCE9[36]=& Cyc_Toc__throw_arraybounds_str,((
_tmpCE9[37]=& Cyc_Toc__dyneither_ptr_decrease_size_str,_tmpCE9)))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))),
sizeof(struct _dyneither_ptr*),38);};_pop_dynregion(d);}}struct Cyc_List_List*Cyc_Toc_toc(
struct Cyc_List_List*ds);struct Cyc_List_List*Cyc_Toc_toc(struct Cyc_List_List*ds){
Cyc_Toc_init();{struct _RegionHandle _tmp8CF=_new_region("start");struct
_RegionHandle*start=& _tmp8CF;_push_region(start);Cyc_Toc_decls_to_c(start,Cyc_Toc_empty_env(
start),ds,1,0);{struct _DynRegionHandle*_tmp8D1;struct Cyc_Toc_TocState _tmp8D0=*((
struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp8D1=_tmp8D0.dyn;Cyc_Core_free_dynregion(
_tmp8D1);}{struct Cyc_List_List*_tmp8D2=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(Cyc_Toc_result_decls);_npop_handler(0);return _tmp8D2;};
_pop_region(start);}}
