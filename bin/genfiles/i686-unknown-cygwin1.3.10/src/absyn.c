#ifndef _SETJMP_H_
#define _SETJMP_H_
#ifndef _jmp_buf_def_
#define _jmp_buf_def_
typedef int jmp_buf[52];
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
char Cyc_Core_Open_Region[16];extern char Cyc_Core_Free_Region[16];struct Cyc_List_List{
void*hd;struct Cyc_List_List*tl;};struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),
struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[18];struct Cyc_List_List*
Cyc_List_imp_rev(struct Cyc_List_List*x);extern char Cyc_List_Nth[8];int Cyc_List_list_cmp(
int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2);struct Cyc_Lineno_Pos{
struct _dyneither_ptr logical_file;struct _dyneither_ptr line;int line_no;int col;};
extern char Cyc_Position_Exit[9];struct Cyc_Position_Segment;struct Cyc_Position_Segment*
Cyc_Position_segment_join(struct Cyc_Position_Segment*,struct Cyc_Position_Segment*);
struct Cyc_Position_Error{struct _dyneither_ptr source;struct Cyc_Position_Segment*
seg;void*kind;struct _dyneither_ptr desc;};extern char Cyc_Position_Nocontext[14];
struct Cyc_Absyn_Loc_n_struct{int tag;};struct Cyc_Absyn_Rel_n_struct{int tag;struct
Cyc_List_List*f1;};struct Cyc_Absyn_Abs_n_struct{int tag;struct Cyc_List_List*f1;};
union Cyc_Absyn_Nmspace_union{struct Cyc_Absyn_Loc_n_struct Loc_n;struct Cyc_Absyn_Rel_n_struct
Rel_n;struct Cyc_Absyn_Abs_n_struct Abs_n;};struct _tuple0{union Cyc_Absyn_Nmspace_union
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
Cyc_Absyn_UnknownDatatypeInfo{struct _tuple0*name;int is_extensible;int is_flat;};
struct Cyc_Absyn_UnknownDatatype_struct{int tag;struct Cyc_Absyn_UnknownDatatypeInfo
f1;};struct Cyc_Absyn_KnownDatatype_struct{int tag;struct Cyc_Absyn_Datatypedecl**
f1;};union Cyc_Absyn_DatatypeInfoU_union{struct Cyc_Absyn_UnknownDatatype_struct
UnknownDatatype;struct Cyc_Absyn_KnownDatatype_struct KnownDatatype;};struct Cyc_Absyn_DatatypeInfo{
union Cyc_Absyn_DatatypeInfoU_union datatype_info;struct Cyc_List_List*targs;struct
Cyc_Core_Opt*rgn;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple0*
datatype_name;struct _tuple0*field_name;int is_extensible;};struct Cyc_Absyn_UnknownDatatypefield_struct{
int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo f1;};struct Cyc_Absyn_KnownDatatypefield_struct{
int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};union
Cyc_Absyn_DatatypeFieldInfoU_union{struct Cyc_Absyn_UnknownDatatypefield_struct
UnknownDatatypefield;struct Cyc_Absyn_KnownDatatypefield_struct KnownDatatypefield;
};struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU_union
field_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownAggr_struct{int tag;
void*f1;struct _tuple0*f2;};struct Cyc_Absyn_KnownAggr_struct{int tag;struct Cyc_Absyn_Aggrdecl**
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
_dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_struct{int tag;struct
Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_struct{
int tag;struct Cyc_List_List*f1;};struct _tuple1{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Tqual
f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_struct{int tag;struct _tuple1*f1;struct
Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_struct{int tag;struct Cyc_List_List*f1;
};struct Cyc_Absyn_Comprehension_e_struct{int tag;struct Cyc_Absyn_Vardecl*f1;
struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_Aggregate_e_struct{
int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*
f4;};struct Cyc_Absyn_AnonStruct_e_struct{int tag;void*f1;struct Cyc_List_List*f2;}
;struct Cyc_Absyn_Datatype_e_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*
f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_struct{int tag;
struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};
struct Cyc_Absyn_AnonEnum_e_struct{int tag;struct _tuple0*f1;void*f2;struct Cyc_Absyn_Enumfield*
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
_tuple2{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_struct{
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
f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_struct{
int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct
Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Int_p_struct{int tag;void*f1;int f2;};
struct Cyc_Absyn_Char_p_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_struct{int
tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Enum_p_struct{int tag;struct Cyc_Absyn_Enumdecl*
f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_struct{int tag;void*
f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_struct{int tag;
struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_struct{int tag;struct _tuple0*f1;
struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_struct{int tag;struct Cyc_Absyn_Exp*
f1;};struct Cyc_Absyn_Pat{void*r;struct Cyc_Core_Opt*topt;struct Cyc_Position_Segment*
loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*
pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;struct Cyc_Position_Segment*
loc;};struct Cyc_Absyn_Global_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct
Cyc_Absyn_Funname_b_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_struct{int tag;struct
Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_Absyn_Vardecl{void*sc;struct _tuple0*name;struct Cyc_Absyn_Tqual tq;
void*type;struct Cyc_Absyn_Exp*initializer;struct Cyc_Core_Opt*rgn;struct Cyc_List_List*
attributes;int escapes;};struct Cyc_Absyn_Fndecl{void*sc;int is_inline;struct
_tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*effect;void*ret_type;
struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;
struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;struct Cyc_Core_Opt*
cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;
struct Cyc_List_List*attributes;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*
name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*
attributes;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct
Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{
void*kind;void*sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*
impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Datatypefield{struct
_tuple0*name;struct Cyc_List_List*typs;struct Cyc_Position_Segment*loc;void*sc;};
struct Cyc_Absyn_Datatypedecl{void*sc;struct _tuple0*name;struct Cyc_List_List*tvs;
struct Cyc_Core_Opt*fields;int is_extensible;int is_flat;};struct Cyc_Absyn_Enumfield{
struct _tuple0*name;struct Cyc_Absyn_Exp*tag;struct Cyc_Position_Segment*loc;};
struct Cyc_Absyn_Enumdecl{void*sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};
struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*
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
f2;};struct Cyc_Absyn_Using_d_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*
f2;};struct Cyc_Absyn_ExternC_d_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_struct{
int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Decl{void*
r;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_ArrayElement_struct{int tag;
struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_struct{int tag;struct
_dyneither_ptr*f1;};char Cyc_Absyn_EmptyAnnot[15]="\000\000\000\000EmptyAnnot\000";
int Cyc_Absyn_qvar_cmp(struct _tuple0*,struct _tuple0*);int Cyc_Absyn_varlist_cmp(
struct Cyc_List_List*,struct Cyc_List_List*);int Cyc_Absyn_tvar_cmp(struct Cyc_Absyn_Tvar*,
struct Cyc_Absyn_Tvar*);int Cyc_Absyn_is_qvar_qualified(struct _tuple0*);struct Cyc_Absyn_Tqual
Cyc_Absyn_const_tqual(struct Cyc_Position_Segment*);struct Cyc_Absyn_Tqual Cyc_Absyn_combine_tqual(
struct Cyc_Absyn_Tqual x,struct Cyc_Absyn_Tqual y);struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(
struct Cyc_Position_Segment*);struct Cyc_Absyn_Conref*Cyc_Absyn_new_conref(void*x);
struct Cyc_Absyn_Conref*Cyc_Absyn_empty_conref();struct Cyc_Absyn_Conref*Cyc_Absyn_compress_conref(
struct Cyc_Absyn_Conref*x);void*Cyc_Absyn_conref_val(struct Cyc_Absyn_Conref*x);
void*Cyc_Absyn_conref_def(void*,struct Cyc_Absyn_Conref*x);extern struct Cyc_Absyn_Conref*
Cyc_Absyn_true_conref;extern struct Cyc_Absyn_Conref*Cyc_Absyn_false_conref;extern
struct Cyc_Absyn_Conref*Cyc_Absyn_bounds_one_conref;extern struct Cyc_Absyn_Conref*
Cyc_Absyn_bounds_dyneither_conref;void*Cyc_Absyn_compress_kb(void*);void*Cyc_Absyn_force_kb(
void*kb);void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);void*Cyc_Absyn_int_typ(void*,void*);
extern void*Cyc_Absyn_char_typ;extern void*Cyc_Absyn_uchar_typ;extern void*Cyc_Absyn_ushort_typ;
extern void*Cyc_Absyn_uint_typ;extern void*Cyc_Absyn_ulong_typ;extern void*Cyc_Absyn_ulonglong_typ;
extern void*Cyc_Absyn_schar_typ;extern void*Cyc_Absyn_sshort_typ;extern void*Cyc_Absyn_sint_typ;
extern void*Cyc_Absyn_slong_typ;extern void*Cyc_Absyn_slonglong_typ;extern void*Cyc_Absyn_float_typ;
void*Cyc_Absyn_double_typ(int);extern void*Cyc_Absyn_empty_effect;extern struct
_tuple0*Cyc_Absyn_exn_name;extern struct Cyc_Absyn_Datatypedecl*Cyc_Absyn_exn_tud;
extern void*Cyc_Absyn_exn_typ;extern struct _tuple0*Cyc_Absyn_datatype_print_arg_qvar;
extern struct _tuple0*Cyc_Absyn_datatype_scanf_arg_qvar;void*Cyc_Absyn_string_typ(
void*rgn);void*Cyc_Absyn_const_string_typ(void*rgn);void*Cyc_Absyn_file_typ();
extern struct Cyc_Absyn_Exp*Cyc_Absyn_exp_unsigned_one;extern void*Cyc_Absyn_bounds_one;
void*Cyc_Absyn_starb_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*b,struct
Cyc_Absyn_Conref*zero_term);void*Cyc_Absyn_atb_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual
tq,void*b,struct Cyc_Absyn_Conref*zero_term);void*Cyc_Absyn_star_typ(void*t,void*
rgn,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Conref*zero_term);void*Cyc_Absyn_at_typ(
void*t,void*rgn,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Conref*zero_term);void*
Cyc_Absyn_cstar_typ(void*t,struct Cyc_Absyn_Tqual tq);void*Cyc_Absyn_dyneither_typ(
void*t,void*rgn,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Conref*zero_term);void*
Cyc_Absyn_void_star_typ();void*Cyc_Absyn_strct(struct _dyneither_ptr*name);void*
Cyc_Absyn_strctq(struct _tuple0*name);void*Cyc_Absyn_unionq_typ(struct _tuple0*
name);void*Cyc_Absyn_union_typ(struct _dyneither_ptr*name);void*Cyc_Absyn_array_typ(
void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*num_elts,struct Cyc_Absyn_Conref*
zero_term,struct Cyc_Position_Segment*ztloc);struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(
void*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_New_exp(struct
Cyc_Absyn_Exp*rgn_handle,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);struct Cyc_Absyn_Exp*
Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst_union,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_null_exp(struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_bool_exp(int,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_true_exp(
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_false_exp(struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(void*,int,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_signed_int_exp(int,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_char_exp(char c,struct Cyc_Position_Segment*);struct
Cyc_Absyn_Exp*Cyc_Absyn_float_exp(struct _dyneither_ptr f,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(struct _dyneither_ptr s,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_var_exp(struct _tuple0*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(struct _tuple0*,void*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_unknownid_exp(struct _tuple0*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_primop_exp(void*,struct Cyc_List_List*es,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(void*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(void*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_add_exp(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_times_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_divide_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_eq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_neq_exp(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_gt_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_lt_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_gte_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_lte_exp(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_assign_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_increment_exp(struct
Cyc_Absyn_Exp*,void*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_post_inc_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_post_dec_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_pre_inc_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_pre_dec_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_or_exp(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_unknowncall_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_fncall_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_throw_exp(struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_noinstantiate_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_instantiate_exp(
struct Cyc_Absyn_Exp*,struct Cyc_List_List*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_cast_exp(void*,struct Cyc_Absyn_Exp*,int user_cast,void*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeofexp_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_offsetof_exp(void*,void*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_gentyp_exp(struct Cyc_List_List*,void*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(struct Cyc_Absyn_Exp*,struct
_dyneither_ptr*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_aggrarrow_exp(
struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,struct Cyc_Position_Segment*);struct
Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_tuple_exp(struct Cyc_List_List*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_match_exn_exp(struct
Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_array_exp(struct Cyc_List_List*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_unresolvedmem_exp(
struct Cyc_Core_Opt*,struct Cyc_List_List*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Stmt*
Cyc_Absyn_new_stmt(void*s,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*
Cyc_Absyn_skip_stmt(struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(
struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(
struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmts(struct Cyc_List_List*,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_return_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(struct Cyc_Absyn_Exp*e,struct
Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,struct Cyc_Position_Segment*loc);struct
Cyc_Absyn_Stmt*Cyc_Absyn_while_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s,
struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_break_stmt(struct
Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_continue_stmt(struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_for_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2,struct Cyc_Absyn_Exp*e3,struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_switch_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*,
struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_fallthru_stmt(
struct Cyc_List_List*el,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(
struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_declare_stmt(struct _tuple0*,void*,struct Cyc_Absyn_Exp*
init,struct Cyc_Absyn_Stmt*,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*
Cyc_Absyn_label_stmt(struct _dyneither_ptr*v,struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_do_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Exp*
e,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_goto_stmt(
struct _dyneither_ptr*lab,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*
Cyc_Absyn_assign_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_trycatch_stmt(struct Cyc_Absyn_Stmt*,struct
Cyc_List_List*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(
void*p,struct Cyc_Position_Segment*s);struct Cyc_Absyn_Pat*Cyc_Absyn_exp_pat(
struct Cyc_Absyn_Exp*);struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*r,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Decl*Cyc_Absyn_let_decl(struct Cyc_Absyn_Pat*p,struct Cyc_Absyn_Exp*
e,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Decl*Cyc_Absyn_letv_decl(
struct Cyc_List_List*,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Decl*Cyc_Absyn_region_decl(
struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Vardecl*,int,struct Cyc_Absyn_Exp*,struct
Cyc_Position_Segment*);struct Cyc_Absyn_Decl*Cyc_Absyn_alias_decl(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Vardecl*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*
init);struct Cyc_Absyn_Vardecl*Cyc_Absyn_static_vardecl(struct _tuple0*x,void*t,
struct Cyc_Absyn_Exp*init);struct Cyc_Absyn_AggrdeclImpl*Cyc_Absyn_aggrdecl_impl(
struct Cyc_List_List*exists,struct Cyc_List_List*po,struct Cyc_List_List*fs,int
tagged);struct Cyc_Absyn_Decl*Cyc_Absyn_aggr_decl(void*k,void*s,struct _tuple0*n,
struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*atts,
struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Decl*Cyc_Absyn_struct_decl(void*
s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,struct
Cyc_List_List*atts,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Decl*Cyc_Absyn_union_decl(
void*s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,
struct Cyc_List_List*atts,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Decl*
Cyc_Absyn_datatype_decl(void*s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Core_Opt*
fs,int is_extensible,int is_flat,struct Cyc_Position_Segment*loc);void*Cyc_Absyn_function_typ(
struct Cyc_List_List*tvs,struct Cyc_Core_Opt*eff_typ,void*ret_typ,struct Cyc_List_List*
args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,struct Cyc_List_List*
rgn_po,struct Cyc_List_List*atts);void*Cyc_Absyn_pointer_expand(void*,int
fresh_evar);int Cyc_Absyn_is_lvalue(struct Cyc_Absyn_Exp*);struct Cyc_Absyn_Aggrfield*
Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _dyneither_ptr*);struct Cyc_Absyn_Aggrfield*
Cyc_Absyn_lookup_decl_field(struct Cyc_Absyn_Aggrdecl*,struct _dyneither_ptr*);
struct _tuple3{struct Cyc_Absyn_Tqual f1;void*f2;};struct _tuple3*Cyc_Absyn_lookup_tuple_field(
struct Cyc_List_List*,int);struct _dyneither_ptr Cyc_Absyn_attribute2string(void*);
int Cyc_Absyn_fntype_att(void*a);struct _dyneither_ptr*Cyc_Absyn_fieldname(int);
struct _tuple4{void*f1;struct _tuple0*f2;};struct _tuple4 Cyc_Absyn_aggr_kinded_name(
union Cyc_Absyn_AggrInfoU_union);struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(
union Cyc_Absyn_AggrInfoU_union info);int Cyc_Absyn_is_union_type(void*);int Cyc_Absyn_is_nontagged_union_type(
void*);int Cyc_Absyn_is_aggr_type(void*t);void Cyc_Absyn_print_decls(struct Cyc_List_List*);
extern int Cyc_Absyn_porting_c_code;extern int Cyc_Absyn_no_regions;struct Cyc_Typerep_Int_struct{
int tag;int f1;unsigned int f2;};struct Cyc_Typerep_ThinPtr_struct{int tag;
unsigned int f1;void*f2;};struct Cyc_Typerep_FatPtr_struct{int tag;void*f1;};struct
_tuple5{unsigned int f1;struct _dyneither_ptr f2;void*f3;};struct Cyc_Typerep_Struct_struct{
int tag;struct _dyneither_ptr*f1;unsigned int f2;struct _dyneither_ptr f3;};struct
_tuple6{unsigned int f1;void*f2;};struct Cyc_Typerep_Tuple_struct{int tag;
unsigned int f1;struct _dyneither_ptr f2;};struct _tuple7{unsigned int f1;struct
_dyneither_ptr f2;};struct Cyc_Typerep_TUnion_struct{int tag;struct _dyneither_ptr f1;
struct _dyneither_ptr f2;struct _dyneither_ptr f3;};struct Cyc_Typerep_TUnionField_struct{
int tag;struct _dyneither_ptr f1;struct _dyneither_ptr f2;unsigned int f3;struct
_dyneither_ptr f4;};struct _tuple8{struct _dyneither_ptr f1;void*f2;};struct Cyc_Typerep_XTUnion_struct{
int tag;struct _dyneither_ptr f1;struct _dyneither_ptr f2;};struct Cyc_Typerep_Union_struct{
int tag;struct _dyneither_ptr*f1;int f2;struct _dyneither_ptr f3;};struct Cyc_Typerep_Enum_struct{
int tag;struct _dyneither_ptr*f1;int f2;struct _dyneither_ptr f3;};unsigned int Cyc_Typerep_size_type(
void*rep);extern void*Cyc_decls_rep;extern struct Cyc_Typerep_Struct_struct Cyc_struct_Position_Segment_rep;
int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct _dyneither_ptr*s2);struct Cyc___cycFILE;
struct Cyc_Cstdio___abstractFILE;struct Cyc_String_pa_struct{int tag;struct
_dyneither_ptr f1;};struct Cyc_Int_pa_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_struct{
int tag;double f1;};struct Cyc_LongDouble_pa_struct{int tag;long double f1;};struct
Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{int tag;
unsigned long*f1;};struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr,struct
_dyneither_ptr);struct Cyc_ShortPtr_sa_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_struct{
int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{
int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_struct{int tag;struct
_dyneither_ptr f1;};struct Cyc_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_struct{
int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int tag;struct _dyneither_ptr f1;};
int Cyc_printf(struct _dyneither_ptr,struct _dyneither_ptr);extern char Cyc_FileCloseError[
19];extern char Cyc_FileOpenError[18];struct Cyc_FileOpenError_struct{char*tag;
struct _dyneither_ptr f1;};struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*
dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;extern char
Cyc_Set_Absent[11];struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);
struct _RegionHandle*r;struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[12];extern
char Cyc_Dict_Absent[11];struct _tuple9{void*f1;void*f2;};struct _tuple9*Cyc_Dict_rchoose(
struct _RegionHandle*r,struct Cyc_Dict_Dict d);struct _tuple9*Cyc_Dict_rchoose(
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
le;int allow_valueof;};void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap);void*Cyc_Tcutil_compress(void*t);void*Cyc_Tcutil_kind_to_bound(
void*k);void Cyc_Marshal_print_type(void*rep,void*val);struct _tuple10{struct Cyc_Dict_Dict
f1;int f2;};struct _tuple11{struct _dyneither_ptr f1;int f2;};extern int Cyc_Cyclone_tovc_r;
extern void*Cyc_Cyclone_c_compiler;int Cyc_Cyclone_tovc_r=0;void*Cyc_Cyclone_c_compiler=(
void*)0;static int Cyc_Absyn_strlist_cmp(struct Cyc_List_List*ss1,struct Cyc_List_List*
ss2){return((int(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),
struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_strptrcmp,
ss1,ss2);}int Cyc_Absyn_varlist_cmp(struct Cyc_List_List*vs1,struct Cyc_List_List*
vs2){if((int)vs1 == (int)vs2)return 0;return Cyc_Absyn_strlist_cmp(vs1,vs2);}struct
_tuple12{union Cyc_Absyn_Nmspace_union f1;union Cyc_Absyn_Nmspace_union f2;};int Cyc_Absyn_qvar_cmp(
struct _tuple0*q1,struct _tuple0*q2){union Cyc_Absyn_Nmspace_union _tmp0=(*q1).f1;
union Cyc_Absyn_Nmspace_union _tmp1=(*q2).f1;{struct _tuple12 _tmp3=({struct _tuple12
_tmp2;_tmp2.f1=_tmp0;_tmp2.f2=_tmp1;_tmp2;});union Cyc_Absyn_Nmspace_union _tmp4;
union Cyc_Absyn_Nmspace_union _tmp5;union Cyc_Absyn_Nmspace_union _tmp6;struct Cyc_List_List*
_tmp7;union Cyc_Absyn_Nmspace_union _tmp8;struct Cyc_List_List*_tmp9;union Cyc_Absyn_Nmspace_union
_tmpA;struct Cyc_List_List*_tmpB;union Cyc_Absyn_Nmspace_union _tmpC;struct Cyc_List_List*
_tmpD;union Cyc_Absyn_Nmspace_union _tmpE;union Cyc_Absyn_Nmspace_union _tmpF;union
Cyc_Absyn_Nmspace_union _tmp10;union Cyc_Absyn_Nmspace_union _tmp11;_LL1: _tmp4=
_tmp3.f1;if(((_tmp3.f1).Loc_n).tag != 0)goto _LL3;_tmp5=_tmp3.f2;if(((_tmp3.f2).Loc_n).tag
!= 0)goto _LL3;_LL2: goto _LL0;_LL3: _tmp6=_tmp3.f1;if(((_tmp3.f1).Rel_n).tag != 1)
goto _LL5;_tmp7=(_tmp6.Rel_n).f1;_tmp8=_tmp3.f2;if(((_tmp3.f2).Rel_n).tag != 1)
goto _LL5;_tmp9=(_tmp8.Rel_n).f1;_LL4: _tmpB=_tmp7;_tmpD=_tmp9;goto _LL6;_LL5: _tmpA=
_tmp3.f1;if(((_tmp3.f1).Abs_n).tag != 2)goto _LL7;_tmpB=(_tmpA.Abs_n).f1;_tmpC=
_tmp3.f2;if(((_tmp3.f2).Abs_n).tag != 2)goto _LL7;_tmpD=(_tmpC.Abs_n).f1;_LL6: {
int i=Cyc_Absyn_strlist_cmp(_tmpB,_tmpD);if(i != 0)return i;goto _LL0;}_LL7: _tmpE=
_tmp3.f1;if(((_tmp3.f1).Loc_n).tag != 0)goto _LL9;_LL8: return - 1;_LL9: _tmpF=_tmp3.f2;
if(((_tmp3.f2).Loc_n).tag != 0)goto _LLB;_LLA: return 1;_LLB: _tmp10=_tmp3.f1;if(((
_tmp3.f1).Rel_n).tag != 1)goto _LLD;_LLC: return - 1;_LLD: _tmp11=_tmp3.f2;if(((_tmp3.f2).Rel_n).tag
!= 1)goto _LL0;_LLE: return 1;_LL0:;}return Cyc_strptrcmp((*q1).f2,(*q2).f2);}int Cyc_Absyn_tvar_cmp(
struct Cyc_Absyn_Tvar*tv1,struct Cyc_Absyn_Tvar*tv2){int i=Cyc_strptrcmp(tv1->name,
tv2->name);if(i != 0)return i;return tv1->identity - tv2->identity;}union Cyc_Absyn_Nmspace_union
Cyc_Absyn_rel_ns_null=(union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Rel_n_struct){
1,0});int Cyc_Absyn_is_qvar_qualified(struct _tuple0*qv){union Cyc_Absyn_Nmspace_union
_tmp13=(*qv).f1;struct Cyc_List_List*_tmp14;struct Cyc_List_List*_tmp15;_LL10: if((
_tmp13.Rel_n).tag != 1)goto _LL12;_tmp14=(_tmp13.Rel_n).f1;if(_tmp14 != 0)goto _LL12;
_LL11: goto _LL13;_LL12: if((_tmp13.Abs_n).tag != 2)goto _LL14;_tmp15=(_tmp13.Abs_n).f1;
if(_tmp15 != 0)goto _LL14;_LL13: goto _LL15;_LL14: if((_tmp13.Loc_n).tag != 0)goto
_LL16;_LL15: return 0;_LL16:;_LL17: return 1;_LLF:;}static int Cyc_Absyn_new_type_counter=
0;void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*env){return(
void*)({struct Cyc_Absyn_Evar_struct*_tmp16=_cycalloc(sizeof(*_tmp16));_tmp16[0]=({
struct Cyc_Absyn_Evar_struct _tmp17;_tmp17.tag=0;_tmp17.f1=k;_tmp17.f2=0;_tmp17.f3=
Cyc_Absyn_new_type_counter ++;_tmp17.f4=env;_tmp17;});_tmp16;});}static struct Cyc_Core_Opt
Cyc_Absyn_mk={(void*)((void*)1)};void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*tenv){
return Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Absyn_mk,tenv);}struct Cyc_Absyn_Tqual
Cyc_Absyn_combine_tqual(struct Cyc_Absyn_Tqual x,struct Cyc_Absyn_Tqual y){return({
struct Cyc_Absyn_Tqual _tmp18;_tmp18.print_const=x.print_const  || y.print_const;
_tmp18.q_volatile=x.q_volatile  || y.q_volatile;_tmp18.q_restrict=x.q_restrict
 || y.q_restrict;_tmp18.real_const=x.real_const  || y.real_const;_tmp18.loc=Cyc_Position_segment_join(
x.loc,y.loc);_tmp18;});}struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(struct Cyc_Position_Segment*
loc){return({struct Cyc_Absyn_Tqual _tmp19;_tmp19.print_const=0;_tmp19.q_volatile=
0;_tmp19.q_restrict=0;_tmp19.real_const=0;_tmp19.loc=loc;_tmp19;});}struct Cyc_Absyn_Tqual
Cyc_Absyn_const_tqual(struct Cyc_Position_Segment*loc){return({struct Cyc_Absyn_Tqual
_tmp1A;_tmp1A.print_const=1;_tmp1A.q_volatile=0;_tmp1A.q_restrict=0;_tmp1A.real_const=
1;_tmp1A.loc=loc;_tmp1A;});}static struct Cyc_Absyn_Const_e_struct Cyc_Absyn_one_b_raw={
0,(union Cyc_Absyn_Cnst_union)((struct Cyc_Absyn_Int_c_struct){2,(void*)((void*)1),
1})};struct Cyc_Absyn_Exp Cyc_Absyn_exp_unsigned_one_v={0,(void*)((void*)& Cyc_Absyn_one_b_raw),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};struct Cyc_Absyn_Exp*Cyc_Absyn_exp_unsigned_one=&
Cyc_Absyn_exp_unsigned_one_v;static struct Cyc_Absyn_Upper_b_struct Cyc_Absyn_one_bt={
0,& Cyc_Absyn_exp_unsigned_one_v};void*Cyc_Absyn_bounds_one=(void*)& Cyc_Absyn_one_bt;
struct Cyc_Absyn_Conref*Cyc_Absyn_new_conref(void*x){return({struct Cyc_Absyn_Conref*
_tmp1E=_cycalloc(sizeof(*_tmp1E));_tmp1E->v=(union Cyc_Absyn_Constraint_union)({
union Cyc_Absyn_Constraint_union _tmp1F;(_tmp1F.Eq_constr).tag=0;(_tmp1F.Eq_constr).f1=(
void*)x;_tmp1F;});_tmp1E;});}struct Cyc_Absyn_Conref*Cyc_Absyn_empty_conref(){
return({struct Cyc_Absyn_Conref*_tmp20=_cycalloc(sizeof(*_tmp20));_tmp20->v=(
union Cyc_Absyn_Constraint_union)({union Cyc_Absyn_Constraint_union _tmp21;(_tmp21.No_constr).tag=
2;_tmp21;});_tmp20;});}static struct Cyc_Absyn_Conref Cyc_Absyn_true_conref_v={(
union Cyc_Absyn_Constraint_union)((struct Cyc_Absyn_Eq_constr_struct){0,(void*)1})};
struct Cyc_Absyn_Conref*Cyc_Absyn_true_conref=& Cyc_Absyn_true_conref_v;static
struct Cyc_Absyn_Conref Cyc_Absyn_false_conref_v={(union Cyc_Absyn_Constraint_union)((
struct Cyc_Absyn_Eq_constr_struct){0,(void*)0})};struct Cyc_Absyn_Conref*Cyc_Absyn_false_conref=&
Cyc_Absyn_false_conref_v;static struct Cyc_Absyn_Conref Cyc_Absyn_bounds_one_conref_v={(
union Cyc_Absyn_Constraint_union)((struct Cyc_Absyn_Eq_constr_struct){0,(void*)((
void*)& Cyc_Absyn_one_bt)})};struct Cyc_Absyn_Conref*Cyc_Absyn_bounds_one_conref=&
Cyc_Absyn_bounds_one_conref_v;static struct Cyc_Absyn_Conref Cyc_Absyn_bounds_dyneither_conref_v={(
union Cyc_Absyn_Constraint_union)((struct Cyc_Absyn_Eq_constr_struct){0,(void*)((
void*)0)})};struct Cyc_Absyn_Conref*Cyc_Absyn_bounds_dyneither_conref=& Cyc_Absyn_bounds_dyneither_conref_v;
struct Cyc_Absyn_Conref*Cyc_Absyn_compress_conref(struct Cyc_Absyn_Conref*x){union
Cyc_Absyn_Constraint_union _tmp26=x->v;struct Cyc_Absyn_Conref*_tmp27;_LL19: if((
_tmp26.No_constr).tag != 2)goto _LL1B;_LL1A: goto _LL1C;_LL1B: if((_tmp26.Eq_constr).tag
!= 0)goto _LL1D;_LL1C: return x;_LL1D: if((_tmp26.Forward_constr).tag != 1)goto _LL18;
_tmp27=(_tmp26.Forward_constr).f1;_LL1E: {struct Cyc_Absyn_Conref*_tmp28=Cyc_Absyn_compress_conref(
_tmp27);x->v=(union Cyc_Absyn_Constraint_union)({union Cyc_Absyn_Constraint_union
_tmp29;(_tmp29.Forward_constr).tag=1;(_tmp29.Forward_constr).f1=_tmp28;_tmp29;});
return _tmp28;}_LL18:;}void*Cyc_Absyn_conref_val(struct Cyc_Absyn_Conref*x){union
Cyc_Absyn_Constraint_union _tmp2A=(Cyc_Absyn_compress_conref(x))->v;void*_tmp2B;
_LL20: if((_tmp2A.Eq_constr).tag != 0)goto _LL22;_tmp2B=(_tmp2A.Eq_constr).f1;_LL21:
return _tmp2B;_LL22:;_LL23:({void*_tmp2C=0;Cyc_Tcutil_impos(({const char*_tmp2D="conref_val";
_tag_dyneither(_tmp2D,sizeof(char),11);}),_tag_dyneither(_tmp2C,sizeof(void*),0));});
_LL1F:;}void*Cyc_Absyn_conref_def(void*y,struct Cyc_Absyn_Conref*x){union Cyc_Absyn_Constraint_union
_tmp2E=(Cyc_Absyn_compress_conref(x))->v;void*_tmp2F;_LL25: if((_tmp2E.Eq_constr).tag
!= 0)goto _LL27;_tmp2F=(_tmp2E.Eq_constr).f1;_LL26: return _tmp2F;_LL27:;_LL28:
return y;_LL24:;}void*Cyc_Absyn_compress_kb(void*k){void*_tmp30=k;struct Cyc_Core_Opt*
_tmp31;struct Cyc_Core_Opt*_tmp32;struct Cyc_Core_Opt*_tmp33;struct Cyc_Core_Opt
_tmp34;void*_tmp35;void**_tmp36;struct Cyc_Core_Opt*_tmp37;struct Cyc_Core_Opt
_tmp38;void*_tmp39;void**_tmp3A;_LL2A: if(*((int*)_tmp30)!= 0)goto _LL2C;_LL2B:
goto _LL2D;_LL2C: if(*((int*)_tmp30)!= 1)goto _LL2E;_tmp31=((struct Cyc_Absyn_Unknown_kb_struct*)
_tmp30)->f1;if(_tmp31 != 0)goto _LL2E;_LL2D: goto _LL2F;_LL2E: if(*((int*)_tmp30)!= 2)
goto _LL30;_tmp32=((struct Cyc_Absyn_Less_kb_struct*)_tmp30)->f1;if(_tmp32 != 0)
goto _LL30;_LL2F: return k;_LL30: if(*((int*)_tmp30)!= 1)goto _LL32;_tmp33=((struct
Cyc_Absyn_Unknown_kb_struct*)_tmp30)->f1;if(_tmp33 == 0)goto _LL32;_tmp34=*_tmp33;
_tmp35=(void*)_tmp34.v;_tmp36=(void**)&(*((struct Cyc_Absyn_Unknown_kb_struct*)
_tmp30)->f1).v;_LL31: _tmp3A=_tmp36;goto _LL33;_LL32: if(*((int*)_tmp30)!= 2)goto
_LL29;_tmp37=((struct Cyc_Absyn_Less_kb_struct*)_tmp30)->f1;if(_tmp37 == 0)goto
_LL29;_tmp38=*_tmp37;_tmp39=(void*)_tmp38.v;_tmp3A=(void**)&(*((struct Cyc_Absyn_Less_kb_struct*)
_tmp30)->f1).v;_LL33:*_tmp3A=Cyc_Absyn_compress_kb(*_tmp3A);return*_tmp3A;_LL29:;}
void*Cyc_Absyn_force_kb(void*kb){void*_tmp3B=Cyc_Absyn_compress_kb(kb);void*
_tmp3C;struct Cyc_Core_Opt*_tmp3D;struct Cyc_Core_Opt**_tmp3E;struct Cyc_Core_Opt*
_tmp3F;struct Cyc_Core_Opt**_tmp40;void*_tmp41;_LL35: if(*((int*)_tmp3B)!= 0)goto
_LL37;_tmp3C=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp3B)->f1;_LL36: return
_tmp3C;_LL37: if(*((int*)_tmp3B)!= 1)goto _LL39;_tmp3D=((struct Cyc_Absyn_Unknown_kb_struct*)
_tmp3B)->f1;_tmp3E=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)
_tmp3B)->f1;_LL38: _tmp40=_tmp3E;_tmp41=(void*)2;goto _LL3A;_LL39: if(*((int*)
_tmp3B)!= 2)goto _LL34;_tmp3F=((struct Cyc_Absyn_Less_kb_struct*)_tmp3B)->f1;
_tmp40=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)_tmp3B)->f1;
_tmp41=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp3B)->f2;_LL3A:*_tmp40=({
struct Cyc_Core_Opt*_tmp42=_cycalloc(sizeof(*_tmp42));_tmp42->v=(void*)Cyc_Tcutil_kind_to_bound(
_tmp41);_tmp42;});return _tmp41;_LL34:;}static struct Cyc_Absyn_IntType_struct Cyc_Absyn_char_tt={
5,(void*)((void*)2),(void*)((void*)0)};static struct Cyc_Absyn_IntType_struct Cyc_Absyn_uchar_tt={
5,(void*)((void*)1),(void*)((void*)0)};static struct Cyc_Absyn_IntType_struct Cyc_Absyn_ushort_tt={
5,(void*)((void*)1),(void*)((void*)1)};static struct Cyc_Absyn_IntType_struct Cyc_Absyn_uint_tt={
5,(void*)((void*)1),(void*)((void*)2)};static struct Cyc_Absyn_IntType_struct Cyc_Absyn_ulong_tt={
5,(void*)((void*)1),(void*)((void*)3)};static struct Cyc_Absyn_IntType_struct Cyc_Absyn_ulonglong_tt={
5,(void*)((void*)1),(void*)((void*)4)};void*Cyc_Absyn_char_typ=(void*)& Cyc_Absyn_char_tt;
void*Cyc_Absyn_uchar_typ=(void*)& Cyc_Absyn_uchar_tt;void*Cyc_Absyn_ushort_typ=(
void*)& Cyc_Absyn_ushort_tt;void*Cyc_Absyn_uint_typ=(void*)& Cyc_Absyn_uint_tt;
void*Cyc_Absyn_ulong_typ=(void*)& Cyc_Absyn_ulong_tt;void*Cyc_Absyn_ulonglong_typ=(
void*)& Cyc_Absyn_ulonglong_tt;static struct Cyc_Absyn_IntType_struct Cyc_Absyn_schar_tt={
5,(void*)((void*)0),(void*)((void*)0)};static struct Cyc_Absyn_IntType_struct Cyc_Absyn_sshort_tt={
5,(void*)((void*)0),(void*)((void*)1)};static struct Cyc_Absyn_IntType_struct Cyc_Absyn_sint_tt={
5,(void*)((void*)0),(void*)((void*)2)};static struct Cyc_Absyn_IntType_struct Cyc_Absyn_slong_tt={
5,(void*)((void*)0),(void*)((void*)3)};static struct Cyc_Absyn_IntType_struct Cyc_Absyn_slonglong_tt={
5,(void*)((void*)0),(void*)((void*)4)};void*Cyc_Absyn_schar_typ=(void*)& Cyc_Absyn_schar_tt;
void*Cyc_Absyn_sshort_typ=(void*)& Cyc_Absyn_sshort_tt;void*Cyc_Absyn_sint_typ=(
void*)& Cyc_Absyn_sint_tt;void*Cyc_Absyn_slong_typ=(void*)& Cyc_Absyn_slong_tt;
void*Cyc_Absyn_slonglong_typ=(void*)& Cyc_Absyn_slonglong_tt;static struct Cyc_Absyn_IntType_struct
Cyc_Absyn_nshort_tt={5,(void*)((void*)2),(void*)((void*)1)};static struct Cyc_Absyn_IntType_struct
Cyc_Absyn_nint_tt={5,(void*)((void*)2),(void*)((void*)2)};static struct Cyc_Absyn_IntType_struct
Cyc_Absyn_nlong_tt={5,(void*)((void*)2),(void*)((void*)3)};static struct Cyc_Absyn_IntType_struct
Cyc_Absyn_nlonglong_tt={5,(void*)((void*)2),(void*)((void*)4)};void*Cyc_Absyn_nshort_typ=(
void*)& Cyc_Absyn_nshort_tt;void*Cyc_Absyn_nint_typ=(void*)& Cyc_Absyn_nint_tt;
void*Cyc_Absyn_nlong_typ=(void*)& Cyc_Absyn_nlong_tt;void*Cyc_Absyn_nlonglong_typ=(
void*)& Cyc_Absyn_nlonglong_tt;void*Cyc_Absyn_int_typ(void*sn,void*sz){void*
_tmp52=sn;_LL3C: if((int)_tmp52 != 0)goto _LL3E;_LL3D: {void*_tmp53=sz;_LL43: if((
int)_tmp53 != 0)goto _LL45;_LL44: return Cyc_Absyn_schar_typ;_LL45: if((int)_tmp53 != 
1)goto _LL47;_LL46: return Cyc_Absyn_sshort_typ;_LL47: if((int)_tmp53 != 2)goto _LL49;
_LL48: return Cyc_Absyn_sint_typ;_LL49: if((int)_tmp53 != 3)goto _LL4B;_LL4A: return
Cyc_Absyn_slong_typ;_LL4B: if((int)_tmp53 != 4)goto _LL42;_LL4C: return Cyc_Absyn_slonglong_typ;
_LL42:;}_LL3E: if((int)_tmp52 != 1)goto _LL40;_LL3F: {void*_tmp54=sz;_LL4E: if((int)
_tmp54 != 0)goto _LL50;_LL4F: return Cyc_Absyn_uchar_typ;_LL50: if((int)_tmp54 != 1)
goto _LL52;_LL51: return Cyc_Absyn_ushort_typ;_LL52: if((int)_tmp54 != 2)goto _LL54;
_LL53: return Cyc_Absyn_uint_typ;_LL54: if((int)_tmp54 != 3)goto _LL56;_LL55: return
Cyc_Absyn_ulong_typ;_LL56: if((int)_tmp54 != 4)goto _LL4D;_LL57: return Cyc_Absyn_ulonglong_typ;
_LL4D:;}_LL40: if((int)_tmp52 != 2)goto _LL3B;_LL41: {void*_tmp55=sz;_LL59: if((int)
_tmp55 != 0)goto _LL5B;_LL5A: return Cyc_Absyn_char_typ;_LL5B: if((int)_tmp55 != 1)
goto _LL5D;_LL5C: return Cyc_Absyn_nshort_typ;_LL5D: if((int)_tmp55 != 2)goto _LL5F;
_LL5E: return Cyc_Absyn_nint_typ;_LL5F: if((int)_tmp55 != 3)goto _LL61;_LL60: return
Cyc_Absyn_nlong_typ;_LL61: if((int)_tmp55 != 4)goto _LL58;_LL62: return Cyc_Absyn_nlonglong_typ;
_LL58:;}_LL3B:;}void*Cyc_Absyn_float_typ=(void*)1;void*Cyc_Absyn_double_typ(int b){
static struct Cyc_Absyn_DoubleType_struct dt={6,1};static struct Cyc_Absyn_DoubleType_struct
df={6,0};return(void*)(b?& dt:& df);}static char _tmp58[4]="exn";static struct
_dyneither_ptr Cyc_Absyn_exn_str={_tmp58,_tmp58,_tmp58 + 4};static struct _tuple0 Cyc_Absyn_exn_name_v={(
union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Abs_n_struct){2,0}),& Cyc_Absyn_exn_str};
struct _tuple0*Cyc_Absyn_exn_name=& Cyc_Absyn_exn_name_v;static char _tmp5A[15]="Null_Exception";
static struct _dyneither_ptr Cyc_Absyn_Null_Exception_str={_tmp5A,_tmp5A,_tmp5A + 15};
static struct _tuple0 Cyc_Absyn_Null_Exception_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Abs_n_struct){2,0}),& Cyc_Absyn_Null_Exception_str};struct _tuple0*
Cyc_Absyn_Null_Exception_name=& Cyc_Absyn_Null_Exception_pr;static struct Cyc_Absyn_Datatypefield
Cyc_Absyn_Null_Exception_tuf_v={& Cyc_Absyn_Null_Exception_pr,0,0,(void*)((void*)
3)};struct Cyc_Absyn_Datatypefield*Cyc_Absyn_Null_Exception_tuf=& Cyc_Absyn_Null_Exception_tuf_v;
static char _tmp5C[13]="Array_bounds";static struct _dyneither_ptr Cyc_Absyn_Array_bounds_str={
_tmp5C,_tmp5C,_tmp5C + 13};static struct _tuple0 Cyc_Absyn_Array_bounds_pr={(union
Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Abs_n_struct){2,0}),& Cyc_Absyn_Array_bounds_str};
struct _tuple0*Cyc_Absyn_Array_bounds_name=& Cyc_Absyn_Array_bounds_pr;static
struct Cyc_Absyn_Datatypefield Cyc_Absyn_Array_bounds_tuf_v={& Cyc_Absyn_Array_bounds_pr,
0,0,(void*)((void*)3)};struct Cyc_Absyn_Datatypefield*Cyc_Absyn_Array_bounds_tuf=&
Cyc_Absyn_Array_bounds_tuf_v;static char _tmp5E[16]="Match_Exception";static struct
_dyneither_ptr Cyc_Absyn_Match_Exception_str={_tmp5E,_tmp5E,_tmp5E + 16};static
struct _tuple0 Cyc_Absyn_Match_Exception_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Abs_n_struct){2,0}),& Cyc_Absyn_Match_Exception_str};struct
_tuple0*Cyc_Absyn_Match_Exception_name=& Cyc_Absyn_Match_Exception_pr;static
struct Cyc_Absyn_Datatypefield Cyc_Absyn_Match_Exception_tuf_v={& Cyc_Absyn_Match_Exception_pr,
0,0,(void*)((void*)3)};struct Cyc_Absyn_Datatypefield*Cyc_Absyn_Match_Exception_tuf=&
Cyc_Absyn_Match_Exception_tuf_v;static char _tmp60[10]="Bad_alloc";static struct
_dyneither_ptr Cyc_Absyn_Bad_alloc_str={_tmp60,_tmp60,_tmp60 + 10};static struct
_tuple0 Cyc_Absyn_Bad_alloc_pr={(union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Abs_n_struct){
2,0}),& Cyc_Absyn_Bad_alloc_str};struct _tuple0*Cyc_Absyn_Bad_alloc_name=& Cyc_Absyn_Bad_alloc_pr;
static struct Cyc_Absyn_Datatypefield Cyc_Absyn_Bad_alloc_tuf_v={& Cyc_Absyn_Bad_alloc_pr,
0,0,(void*)((void*)3)};struct Cyc_Absyn_Datatypefield*Cyc_Absyn_Bad_alloc_tuf=&
Cyc_Absyn_Bad_alloc_tuf_v;static struct Cyc_List_List Cyc_Absyn_exn_l0={(void*)& Cyc_Absyn_Null_Exception_tuf_v,
0};static struct Cyc_List_List Cyc_Absyn_exn_l1={(void*)& Cyc_Absyn_Array_bounds_tuf_v,(
struct Cyc_List_List*)& Cyc_Absyn_exn_l0};static struct Cyc_List_List Cyc_Absyn_exn_l2={(
void*)& Cyc_Absyn_Match_Exception_tuf_v,(struct Cyc_List_List*)& Cyc_Absyn_exn_l1};
static struct Cyc_List_List Cyc_Absyn_exn_l3={(void*)& Cyc_Absyn_Bad_alloc_tuf_v,(
struct Cyc_List_List*)& Cyc_Absyn_exn_l2};static struct Cyc_Core_Opt Cyc_Absyn_exn_ol={(
void*)((struct Cyc_List_List*)& Cyc_Absyn_exn_l3)};static struct Cyc_Absyn_Datatypedecl
Cyc_Absyn_exn_tud_v={(void*)((void*)3),& Cyc_Absyn_exn_name_v,0,(struct Cyc_Core_Opt*)&
Cyc_Absyn_exn_ol,1,0};struct Cyc_Absyn_Datatypedecl*Cyc_Absyn_exn_tud=& Cyc_Absyn_exn_tud_v;
static struct Cyc_Core_Opt Cyc_Absyn_heap_opt={(void*)((void*)2)};static struct Cyc_Absyn_DatatypeType_struct
Cyc_Absyn_exn_typ_tt={2,{(union Cyc_Absyn_DatatypeInfoU_union)((struct Cyc_Absyn_KnownDatatype_struct){
1,& Cyc_Absyn_exn_tud}),0,(struct Cyc_Core_Opt*)& Cyc_Absyn_heap_opt}};void*Cyc_Absyn_exn_typ=(
void*)& Cyc_Absyn_exn_typ_tt;static char _tmp64[9]="PrintArg";static struct
_dyneither_ptr Cyc_Absyn_printarg_str={_tmp64,_tmp64,_tmp64 + 9};static char _tmp65[
9]="ScanfArg";static struct _dyneither_ptr Cyc_Absyn_scanfarg_str={_tmp65,_tmp65,
_tmp65 + 9};static struct _tuple0 Cyc_Absyn_datatype_print_arg_qvar_p={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Abs_n_struct){2,0}),& Cyc_Absyn_printarg_str};static struct _tuple0
Cyc_Absyn_datatype_scanf_arg_qvar_p={(union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Abs_n_struct){
2,0}),& Cyc_Absyn_scanfarg_str};struct _tuple0*Cyc_Absyn_datatype_print_arg_qvar=&
Cyc_Absyn_datatype_print_arg_qvar_p;struct _tuple0*Cyc_Absyn_datatype_scanf_arg_qvar=&
Cyc_Absyn_datatype_scanf_arg_qvar_p;static void**Cyc_Absyn_string_t_opt=0;void*
Cyc_Absyn_string_typ(void*rgn){if(rgn != (void*)2)return Cyc_Absyn_starb_typ(Cyc_Absyn_char_typ,
rgn,Cyc_Absyn_empty_tqual(0),(void*)0,Cyc_Absyn_true_conref);if(Cyc_Absyn_string_t_opt
== 0){void*t=Cyc_Absyn_starb_typ(Cyc_Absyn_char_typ,(void*)2,Cyc_Absyn_empty_tqual(
0),(void*)0,Cyc_Absyn_true_conref);Cyc_Absyn_string_t_opt=({void**_tmp68=
_cycalloc(sizeof(*_tmp68));_tmp68[0]=t;_tmp68;});}return*((void**)_check_null(
Cyc_Absyn_string_t_opt));}static void**Cyc_Absyn_const_string_t_opt=0;void*Cyc_Absyn_const_string_typ(
void*rgn){if(rgn != (void*)2)return Cyc_Absyn_starb_typ(Cyc_Absyn_char_typ,rgn,Cyc_Absyn_const_tqual(
0),(void*)0,Cyc_Absyn_true_conref);if(Cyc_Absyn_const_string_t_opt == 0){void*t=
Cyc_Absyn_starb_typ(Cyc_Absyn_char_typ,(void*)2,Cyc_Absyn_const_tqual(0),(void*)
0,Cyc_Absyn_true_conref);Cyc_Absyn_const_string_t_opt=({void**_tmp69=_cycalloc(
sizeof(*_tmp69));_tmp69[0]=t;_tmp69;});}return*((void**)_check_null(Cyc_Absyn_const_string_t_opt));}
void*Cyc_Absyn_starb_typ(void*t,void*r,struct Cyc_Absyn_Tqual tq,void*b,struct Cyc_Absyn_Conref*
zeroterm){return(void*)({struct Cyc_Absyn_PointerType_struct*_tmp6A=_cycalloc(
sizeof(*_tmp6A));_tmp6A[0]=({struct Cyc_Absyn_PointerType_struct _tmp6B;_tmp6B.tag=
4;_tmp6B.f1=({struct Cyc_Absyn_PtrInfo _tmp6C;_tmp6C.elt_typ=(void*)t;_tmp6C.elt_tq=
tq;_tmp6C.ptr_atts=({struct Cyc_Absyn_PtrAtts _tmp6D;_tmp6D.rgn=(void*)r;_tmp6D.nullable=
Cyc_Absyn_true_conref;_tmp6D.bounds=Cyc_Absyn_new_conref(b);_tmp6D.zero_term=
zeroterm;_tmp6D.ptrloc=0;_tmp6D;});_tmp6C;});_tmp6B;});_tmp6A;});}void*Cyc_Absyn_atb_typ(
void*t,void*r,struct Cyc_Absyn_Tqual tq,void*b,struct Cyc_Absyn_Conref*zeroterm){
return(void*)({struct Cyc_Absyn_PointerType_struct*_tmp6E=_cycalloc(sizeof(*
_tmp6E));_tmp6E[0]=({struct Cyc_Absyn_PointerType_struct _tmp6F;_tmp6F.tag=4;
_tmp6F.f1=({struct Cyc_Absyn_PtrInfo _tmp70;_tmp70.elt_typ=(void*)t;_tmp70.elt_tq=
tq;_tmp70.ptr_atts=({struct Cyc_Absyn_PtrAtts _tmp71;_tmp71.rgn=(void*)r;_tmp71.nullable=
Cyc_Absyn_false_conref;_tmp71.bounds=Cyc_Absyn_new_conref(b);_tmp71.zero_term=
zeroterm;_tmp71.ptrloc=0;_tmp71;});_tmp70;});_tmp6F;});_tmp6E;});}void*Cyc_Absyn_star_typ(
void*t,void*r,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Conref*zeroterm){return(
void*)({struct Cyc_Absyn_PointerType_struct*_tmp72=_cycalloc(sizeof(*_tmp72));
_tmp72[0]=({struct Cyc_Absyn_PointerType_struct _tmp73;_tmp73.tag=4;_tmp73.f1=({
struct Cyc_Absyn_PtrInfo _tmp74;_tmp74.elt_typ=(void*)t;_tmp74.elt_tq=tq;_tmp74.ptr_atts=({
struct Cyc_Absyn_PtrAtts _tmp75;_tmp75.rgn=(void*)r;_tmp75.nullable=Cyc_Absyn_true_conref;
_tmp75.bounds=Cyc_Absyn_bounds_one_conref;_tmp75.zero_term=zeroterm;_tmp75.ptrloc=
0;_tmp75;});_tmp74;});_tmp73;});_tmp72;});}void*Cyc_Absyn_cstar_typ(void*t,
struct Cyc_Absyn_Tqual tq){return(void*)({struct Cyc_Absyn_PointerType_struct*
_tmp76=_cycalloc(sizeof(*_tmp76));_tmp76[0]=({struct Cyc_Absyn_PointerType_struct
_tmp77;_tmp77.tag=4;_tmp77.f1=({struct Cyc_Absyn_PtrInfo _tmp78;_tmp78.elt_typ=(
void*)t;_tmp78.elt_tq=tq;_tmp78.ptr_atts=({struct Cyc_Absyn_PtrAtts _tmp79;_tmp79.rgn=(
void*)((void*)2);_tmp79.nullable=Cyc_Absyn_true_conref;_tmp79.bounds=Cyc_Absyn_bounds_one_conref;
_tmp79.zero_term=Cyc_Absyn_false_conref;_tmp79.ptrloc=0;_tmp79;});_tmp78;});
_tmp77;});_tmp76;});}void*Cyc_Absyn_at_typ(void*t,void*r,struct Cyc_Absyn_Tqual tq,
struct Cyc_Absyn_Conref*zeroterm){return(void*)({struct Cyc_Absyn_PointerType_struct*
_tmp7A=_cycalloc(sizeof(*_tmp7A));_tmp7A[0]=({struct Cyc_Absyn_PointerType_struct
_tmp7B;_tmp7B.tag=4;_tmp7B.f1=({struct Cyc_Absyn_PtrInfo _tmp7C;_tmp7C.elt_typ=(
void*)t;_tmp7C.elt_tq=tq;_tmp7C.ptr_atts=({struct Cyc_Absyn_PtrAtts _tmp7D;_tmp7D.rgn=(
void*)r;_tmp7D.nullable=Cyc_Absyn_false_conref;_tmp7D.bounds=Cyc_Absyn_bounds_one_conref;
_tmp7D.zero_term=zeroterm;_tmp7D.ptrloc=0;_tmp7D;});_tmp7C;});_tmp7B;});_tmp7A;});}
void*Cyc_Absyn_dyneither_typ(void*t,void*r,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Conref*
zeroterm){return(void*)({struct Cyc_Absyn_PointerType_struct*_tmp7E=_cycalloc(
sizeof(*_tmp7E));_tmp7E[0]=({struct Cyc_Absyn_PointerType_struct _tmp7F;_tmp7F.tag=
4;_tmp7F.f1=({struct Cyc_Absyn_PtrInfo _tmp80;_tmp80.elt_typ=(void*)t;_tmp80.elt_tq=
tq;_tmp80.ptr_atts=({struct Cyc_Absyn_PtrAtts _tmp81;_tmp81.rgn=(void*)r;_tmp81.nullable=
Cyc_Absyn_true_conref;_tmp81.bounds=Cyc_Absyn_bounds_dyneither_conref;_tmp81.zero_term=
zeroterm;_tmp81.ptrloc=0;_tmp81;});_tmp80;});_tmp7F;});_tmp7E;});}void*Cyc_Absyn_array_typ(
void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*num_elts,struct Cyc_Absyn_Conref*
zero_term,struct Cyc_Position_Segment*ztloc){return(void*)({struct Cyc_Absyn_ArrayType_struct*
_tmp82=_cycalloc(sizeof(*_tmp82));_tmp82[0]=({struct Cyc_Absyn_ArrayType_struct
_tmp83;_tmp83.tag=7;_tmp83.f1=({struct Cyc_Absyn_ArrayInfo _tmp84;_tmp84.elt_type=(
void*)elt_type;_tmp84.tq=tq;_tmp84.num_elts=num_elts;_tmp84.zero_term=zero_term;
_tmp84.zt_loc=ztloc;_tmp84;});_tmp83;});_tmp82;});}static char _tmp8E[8]="__sFILE";
void*Cyc_Absyn_file_typ(){static void**file_t_opt=0;static struct _dyneither_ptr
sf_str={_tmp8E,_tmp8E,_tmp8E + 8};static struct _dyneither_ptr*sf=& sf_str;if(
file_t_opt == 0){struct _tuple0*file_t_name=({struct _tuple0*_tmp8C=_cycalloc(
sizeof(*_tmp8C));_tmp8C->f1=(union Cyc_Absyn_Nmspace_union)({union Cyc_Absyn_Nmspace_union
_tmp8D;(_tmp8D.Abs_n).tag=2;(_tmp8D.Abs_n).f1=0;_tmp8D;});_tmp8C->f2=sf;_tmp8C;});
struct Cyc_Absyn_Aggrdecl*sd=({struct Cyc_Absyn_Aggrdecl*_tmp8B=_cycalloc(sizeof(*
_tmp8B));_tmp8B->kind=(void*)((void*)0);_tmp8B->sc=(void*)((void*)1);_tmp8B->name=
file_t_name;_tmp8B->tvs=0;_tmp8B->impl=0;_tmp8B->attributes=0;_tmp8B;});void*
file_struct_typ=(void*)({struct Cyc_Absyn_AggrType_struct*_tmp86=_cycalloc(
sizeof(*_tmp86));_tmp86[0]=({struct Cyc_Absyn_AggrType_struct _tmp87;_tmp87.tag=10;
_tmp87.f1=({struct Cyc_Absyn_AggrInfo _tmp88;_tmp88.aggr_info=(union Cyc_Absyn_AggrInfoU_union)({
union Cyc_Absyn_AggrInfoU_union _tmp89;(_tmp89.KnownAggr).tag=1;(_tmp89.KnownAggr).f1=({
struct Cyc_Absyn_Aggrdecl**_tmp8A=_cycalloc(sizeof(*_tmp8A));_tmp8A[0]=sd;_tmp8A;});
_tmp89;});_tmp88.targs=0;_tmp88;});_tmp87;});_tmp86;});file_t_opt=({void**_tmp85=
_cycalloc(sizeof(*_tmp85));_tmp85[0]=Cyc_Absyn_at_typ(file_struct_typ,(void*)2,
Cyc_Absyn_empty_tqual(0),Cyc_Absyn_false_conref);_tmp85;});}return*file_t_opt;}
void*Cyc_Absyn_void_star_typ(){static void**void_star_t_opt=0;if(void_star_t_opt
== 0)void_star_t_opt=({void**_tmp8F=_cycalloc(sizeof(*_tmp8F));_tmp8F[0]=Cyc_Absyn_star_typ((
void*)0,(void*)2,Cyc_Absyn_empty_tqual(0),Cyc_Absyn_false_conref);_tmp8F;});
return*void_star_t_opt;}static struct Cyc_Absyn_JoinEff_struct Cyc_Absyn_empty_eff={
20,0};void*Cyc_Absyn_empty_effect=(void*)& Cyc_Absyn_empty_eff;void*Cyc_Absyn_aggr_typ(
void*k,struct _dyneither_ptr*name){return(void*)({struct Cyc_Absyn_AggrType_struct*
_tmp91=_cycalloc(sizeof(*_tmp91));_tmp91[0]=({struct Cyc_Absyn_AggrType_struct
_tmp92;_tmp92.tag=10;_tmp92.f1=({struct Cyc_Absyn_AggrInfo _tmp93;_tmp93.aggr_info=(
union Cyc_Absyn_AggrInfoU_union)({union Cyc_Absyn_AggrInfoU_union _tmp94;(_tmp94.UnknownAggr).tag=
0;(_tmp94.UnknownAggr).f1=(void*)k;(_tmp94.UnknownAggr).f2=({struct _tuple0*
_tmp95=_cycalloc(sizeof(*_tmp95));_tmp95->f1=Cyc_Absyn_rel_ns_null;_tmp95->f2=
name;_tmp95;});_tmp94;});_tmp93.targs=0;_tmp93;});_tmp92;});_tmp91;});}void*Cyc_Absyn_strct(
struct _dyneither_ptr*name){return Cyc_Absyn_aggr_typ((void*)0,name);}void*Cyc_Absyn_union_typ(
struct _dyneither_ptr*name){return Cyc_Absyn_aggr_typ((void*)1,name);}void*Cyc_Absyn_strctq(
struct _tuple0*name){return(void*)({struct Cyc_Absyn_AggrType_struct*_tmp96=
_cycalloc(sizeof(*_tmp96));_tmp96[0]=({struct Cyc_Absyn_AggrType_struct _tmp97;
_tmp97.tag=10;_tmp97.f1=({struct Cyc_Absyn_AggrInfo _tmp98;_tmp98.aggr_info=(union
Cyc_Absyn_AggrInfoU_union)({union Cyc_Absyn_AggrInfoU_union _tmp99;(_tmp99.UnknownAggr).tag=
0;(_tmp99.UnknownAggr).f1=(void*)((void*)0);(_tmp99.UnknownAggr).f2=name;_tmp99;});
_tmp98.targs=0;_tmp98;});_tmp97;});_tmp96;});}void*Cyc_Absyn_unionq_typ(struct
_tuple0*name){return(void*)({struct Cyc_Absyn_AggrType_struct*_tmp9A=_cycalloc(
sizeof(*_tmp9A));_tmp9A[0]=({struct Cyc_Absyn_AggrType_struct _tmp9B;_tmp9B.tag=10;
_tmp9B.f1=({struct Cyc_Absyn_AggrInfo _tmp9C;_tmp9C.aggr_info=(union Cyc_Absyn_AggrInfoU_union)({
union Cyc_Absyn_AggrInfoU_union _tmp9D;(_tmp9D.UnknownAggr).tag=0;(_tmp9D.UnknownAggr).f1=(
void*)((void*)1);(_tmp9D.UnknownAggr).f2=name;_tmp9D;});_tmp9C.targs=0;_tmp9C;});
_tmp9B;});_tmp9A;});}struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*r,struct Cyc_Position_Segment*
loc){return({struct Cyc_Absyn_Exp*_tmp9E=_cycalloc(sizeof(*_tmp9E));_tmp9E->topt=
0;_tmp9E->r=(void*)r;_tmp9E->loc=loc;_tmp9E->annot=(void*)((void*)Cyc_Absyn_EmptyAnnot);
_tmp9E;});}struct Cyc_Absyn_Exp*Cyc_Absyn_New_exp(struct Cyc_Absyn_Exp*rgn_handle,
struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_New_e_struct*_tmp9F=_cycalloc(sizeof(*_tmp9F));_tmp9F[0]=({
struct Cyc_Absyn_New_e_struct _tmpA0;_tmpA0.tag=17;_tmpA0.f1=rgn_handle;_tmpA0.f2=
e;_tmpA0;});_tmp9F;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*
e){return({struct Cyc_Absyn_Exp*_tmpA1=_cycalloc(sizeof(*_tmpA1));_tmpA1[0]=*e;
_tmpA1;});}struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst_union c,
struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Const_e_struct*
_tmpA2=_cycalloc(sizeof(*_tmpA2));_tmpA2[0]=({struct Cyc_Absyn_Const_e_struct
_tmpA3;_tmpA3.tag=0;_tmpA3.f1=c;_tmpA3;});_tmpA2;}),loc);}struct Cyc_Absyn_Exp*
Cyc_Absyn_null_exp(struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((void*)({
struct Cyc_Absyn_Const_e_struct*_tmpA4=_cycalloc(sizeof(*_tmpA4));_tmpA4[0]=({
struct Cyc_Absyn_Const_e_struct _tmpA5;_tmpA5.tag=0;_tmpA5.f1=(union Cyc_Absyn_Cnst_union)({
union Cyc_Absyn_Cnst_union _tmpA6;(_tmpA6.Null_c).tag=6;_tmpA6;});_tmpA5;});_tmpA4;}),
loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(void*s,int i,struct Cyc_Position_Segment*
seg){return Cyc_Absyn_const_exp((union Cyc_Absyn_Cnst_union)({union Cyc_Absyn_Cnst_union
_tmpA7;(_tmpA7.Int_c).tag=2;(_tmpA7.Int_c).f1=(void*)s;(_tmpA7.Int_c).f2=i;
_tmpA7;}),seg);}struct Cyc_Absyn_Exp*Cyc_Absyn_signed_int_exp(int i,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_int_exp((void*)0,i,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(
unsigned int i,struct Cyc_Position_Segment*loc){return Cyc_Absyn_int_exp((void*)1,(
int)i,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_bool_exp(int b,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_signed_int_exp(b?1: 0,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_true_exp(
struct Cyc_Position_Segment*loc){return Cyc_Absyn_bool_exp(1,loc);}struct Cyc_Absyn_Exp*
Cyc_Absyn_false_exp(struct Cyc_Position_Segment*loc){return Cyc_Absyn_bool_exp(0,
loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_char_exp(char c,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_const_exp((union Cyc_Absyn_Cnst_union)({union Cyc_Absyn_Cnst_union
_tmpA8;(_tmpA8.Char_c).tag=0;(_tmpA8.Char_c).f1=(void*)((void*)2);(_tmpA8.Char_c).f2=
c;_tmpA8;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_float_exp(struct _dyneither_ptr f,
struct Cyc_Position_Segment*loc){return Cyc_Absyn_const_exp((union Cyc_Absyn_Cnst_union)({
union Cyc_Absyn_Cnst_union _tmpA9;(_tmpA9.Float_c).tag=4;(_tmpA9.Float_c).f1=f;
_tmpA9;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(struct _dyneither_ptr s,
struct Cyc_Position_Segment*loc){return Cyc_Absyn_const_exp((union Cyc_Absyn_Cnst_union)({
union Cyc_Absyn_Cnst_union _tmpAA;(_tmpAA.String_c).tag=5;(_tmpAA.String_c).f1=s;
_tmpAA;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_var_exp(struct _tuple0*q,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Var_e_struct*_tmpAB=
_cycalloc(sizeof(*_tmpAB));_tmpAB[0]=({struct Cyc_Absyn_Var_e_struct _tmpAC;_tmpAC.tag=
1;_tmpAC.f1=q;_tmpAC.f2=(void*)((void*)0);_tmpAC;});_tmpAB;}),loc);}struct Cyc_Absyn_Exp*
Cyc_Absyn_varb_exp(struct _tuple0*q,void*b,struct Cyc_Position_Segment*loc){return
Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Var_e_struct*_tmpAD=_cycalloc(sizeof(*
_tmpAD));_tmpAD[0]=({struct Cyc_Absyn_Var_e_struct _tmpAE;_tmpAE.tag=1;_tmpAE.f1=q;
_tmpAE.f2=(void*)b;_tmpAE;});_tmpAD;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_unknownid_exp(
struct _tuple0*q,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((void*)({
struct Cyc_Absyn_UnknownId_e_struct*_tmpAF=_cycalloc(sizeof(*_tmpAF));_tmpAF[0]=({
struct Cyc_Absyn_UnknownId_e_struct _tmpB0;_tmpB0.tag=2;_tmpB0.f1=q;_tmpB0;});
_tmpAF;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_primop_exp(void*p,struct Cyc_List_List*
es,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Primop_e_struct*
_tmpB1=_cycalloc(sizeof(*_tmpB1));_tmpB1[0]=({struct Cyc_Absyn_Primop_e_struct
_tmpB2;_tmpB2.tag=3;_tmpB2.f1=(void*)p;_tmpB2.f2=es;_tmpB2;});_tmpB1;}),loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(void*p,struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_primop_exp(p,({struct Cyc_List_List*_tmpB3=_cycalloc(sizeof(*
_tmpB3));_tmpB3->hd=e;_tmpB3->tl=0;_tmpB3;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(
void*p,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_primop_exp(p,({struct Cyc_List_List*_tmpB4=_cycalloc(sizeof(*
_tmpB4));_tmpB4->hd=e1;_tmpB4->tl=({struct Cyc_List_List*_tmpB5=_cycalloc(sizeof(*
_tmpB5));_tmpB5->hd=e2;_tmpB5->tl=0;_tmpB5;});_tmpB4;}),loc);}struct Cyc_Absyn_Exp*
Cyc_Absyn_add_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_prim2_exp((void*)0,e1,e2,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_times_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_prim2_exp((void*)1,e1,e2,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_divide_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_prim2_exp((void*)3,e1,e2,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_eq_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_prim2_exp((void*)5,e1,e2,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_neq_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_prim2_exp((void*)6,e1,e2,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_gt_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_prim2_exp((void*)7,e1,e2,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_lt_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_prim2_exp((void*)8,e1,e2,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_gte_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_prim2_exp((void*)9,e1,e2,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_lte_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_prim2_exp((void*)10,e1,e2,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_AssignOp_e_struct*_tmpB6=
_cycalloc(sizeof(*_tmpB6));_tmpB6[0]=({struct Cyc_Absyn_AssignOp_e_struct _tmpB7;
_tmpB7.tag=4;_tmpB7.f1=e1;_tmpB7.f2=popt;_tmpB7.f3=e2;_tmpB7;});_tmpB6;}),loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_assign_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2,struct Cyc_Position_Segment*loc){return Cyc_Absyn_assignop_exp(e1,0,e2,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_increment_exp(struct Cyc_Absyn_Exp*e,void*i,struct
Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Increment_e_struct*
_tmpB8=_cycalloc(sizeof(*_tmpB8));_tmpB8[0]=({struct Cyc_Absyn_Increment_e_struct
_tmpB9;_tmpB9.tag=5;_tmpB9.f1=e;_tmpB9.f2=(void*)i;_tmpB9;});_tmpB8;}),loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_post_inc_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_increment_exp(e,(void*)1,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_pre_inc_exp(
struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc){return Cyc_Absyn_increment_exp(
e,(void*)0,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_pre_dec_exp(struct Cyc_Absyn_Exp*e,
struct Cyc_Position_Segment*loc){return Cyc_Absyn_increment_exp(e,(void*)2,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_post_dec_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_increment_exp(e,(void*)3,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Conditional_e_struct*
_tmpBA=_cycalloc(sizeof(*_tmpBA));_tmpBA[0]=({struct Cyc_Absyn_Conditional_e_struct
_tmpBB;_tmpBB.tag=6;_tmpBB.f1=e1;_tmpBB.f2=e2;_tmpBB.f3=e3;_tmpBB;});_tmpBA;}),
loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_And_e_struct*
_tmpBC=_cycalloc(sizeof(*_tmpBC));_tmpBC[0]=({struct Cyc_Absyn_And_e_struct _tmpBD;
_tmpBD.tag=7;_tmpBD.f1=e1;_tmpBD.f2=e2;_tmpBD;});_tmpBC;}),loc);}struct Cyc_Absyn_Exp*
Cyc_Absyn_or_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Or_e_struct*_tmpBE=
_cycalloc(sizeof(*_tmpBE));_tmpBE[0]=({struct Cyc_Absyn_Or_e_struct _tmpBF;_tmpBF.tag=
8;_tmpBF.f1=e1;_tmpBF.f2=e2;_tmpBF;});_tmpBE;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_SeqExp_e_struct*_tmpC0=
_cycalloc(sizeof(*_tmpC0));_tmpC0[0]=({struct Cyc_Absyn_SeqExp_e_struct _tmpC1;
_tmpC1.tag=9;_tmpC1.f1=e1;_tmpC1.f2=e2;_tmpC1;});_tmpC0;}),loc);}struct Cyc_Absyn_Exp*
Cyc_Absyn_unknowncall_exp(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_UnknownCall_e_struct*
_tmpC2=_cycalloc(sizeof(*_tmpC2));_tmpC2[0]=({struct Cyc_Absyn_UnknownCall_e_struct
_tmpC3;_tmpC3.tag=10;_tmpC3.f1=e;_tmpC3.f2=es;_tmpC3;});_tmpC2;}),loc);}struct
Cyc_Absyn_Exp*Cyc_Absyn_fncall_exp(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es,
struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_FnCall_e_struct*
_tmpC4=_cycalloc(sizeof(*_tmpC4));_tmpC4[0]=({struct Cyc_Absyn_FnCall_e_struct
_tmpC5;_tmpC5.tag=11;_tmpC5.f1=e;_tmpC5.f2=es;_tmpC5.f3=0;_tmpC5;});_tmpC4;}),
loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_noinstantiate_exp(struct Cyc_Absyn_Exp*e,
struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_NoInstantiate_e_struct*
_tmpC6=_cycalloc(sizeof(*_tmpC6));_tmpC6[0]=({struct Cyc_Absyn_NoInstantiate_e_struct
_tmpC7;_tmpC7.tag=13;_tmpC7.f1=e;_tmpC7;});_tmpC6;}),loc);}struct Cyc_Absyn_Exp*
Cyc_Absyn_instantiate_exp(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*ts,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Instantiate_e_struct*
_tmpC8=_cycalloc(sizeof(*_tmpC8));_tmpC8[0]=({struct Cyc_Absyn_Instantiate_e_struct
_tmpC9;_tmpC9.tag=14;_tmpC9.f1=e;_tmpC9.f2=ts;_tmpC9;});_tmpC8;}),loc);}struct
Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*t,struct Cyc_Absyn_Exp*e,int user_cast,void*
c,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Cast_e_struct*
_tmpCA=_cycalloc(sizeof(*_tmpCA));_tmpCA[0]=({struct Cyc_Absyn_Cast_e_struct
_tmpCB;_tmpCB.tag=15;_tmpCB.f1=(void*)t;_tmpCB.f2=e;_tmpCB.f3=user_cast;_tmpCB.f4=(
void*)c;_tmpCB;});_tmpCA;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_throw_exp(struct
Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((void*)({
struct Cyc_Absyn_Throw_e_struct*_tmpCC=_cycalloc(sizeof(*_tmpCC));_tmpCC[0]=({
struct Cyc_Absyn_Throw_e_struct _tmpCD;_tmpCD.tag=12;_tmpCD.f1=e;_tmpCD;});_tmpCC;}),
loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Address_e_struct*_tmpCE=
_cycalloc(sizeof(*_tmpCE));_tmpCE[0]=({struct Cyc_Absyn_Address_e_struct _tmpCF;
_tmpCF.tag=16;_tmpCF.f1=e;_tmpCF;});_tmpCE;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(
void*t,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((void*)({struct
Cyc_Absyn_Sizeoftyp_e_struct*_tmpD0=_cycalloc(sizeof(*_tmpD0));_tmpD0[0]=({
struct Cyc_Absyn_Sizeoftyp_e_struct _tmpD1;_tmpD1.tag=18;_tmpD1.f1=(void*)t;_tmpD1;});
_tmpD0;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_sizeofexp_exp(struct Cyc_Absyn_Exp*
e,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Sizeofexp_e_struct*
_tmpD2=_cycalloc(sizeof(*_tmpD2));_tmpD2[0]=({struct Cyc_Absyn_Sizeofexp_e_struct
_tmpD3;_tmpD3.tag=19;_tmpD3.f1=e;_tmpD3;});_tmpD2;}),loc);}struct Cyc_Absyn_Exp*
Cyc_Absyn_offsetof_exp(void*t,void*of,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_Offsetof_e_struct*_tmpD4=_cycalloc(sizeof(*_tmpD4));
_tmpD4[0]=({struct Cyc_Absyn_Offsetof_e_struct _tmpD5;_tmpD5.tag=20;_tmpD5.f1=(
void*)t;_tmpD5.f2=(void*)of;_tmpD5;});_tmpD4;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_gentyp_exp(
struct Cyc_List_List*tvs,void*t,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_Gentyp_e_struct*_tmpD6=_cycalloc(sizeof(*_tmpD6));_tmpD6[
0]=({struct Cyc_Absyn_Gentyp_e_struct _tmpD7;_tmpD7.tag=21;_tmpD7.f1=tvs;_tmpD7.f2=(
void*)t;_tmpD7;});_tmpD6;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct
Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((void*)({
struct Cyc_Absyn_Deref_e_struct*_tmpD8=_cycalloc(sizeof(*_tmpD8));_tmpD8[0]=({
struct Cyc_Absyn_Deref_e_struct _tmpD9;_tmpD9.tag=22;_tmpD9.f1=e;_tmpD9;});_tmpD8;}),
loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(struct Cyc_Absyn_Exp*e,struct
_dyneither_ptr*n,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((void*)({
struct Cyc_Absyn_AggrMember_e_struct*_tmpDA=_cycalloc(sizeof(*_tmpDA));_tmpDA[0]=({
struct Cyc_Absyn_AggrMember_e_struct _tmpDB;_tmpDB.tag=23;_tmpDB.f1=e;_tmpDB.f2=n;
_tmpDB.f3=0;_tmpDB.f4=0;_tmpDB;});_tmpDA;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_aggrarrow_exp(
struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_AggrArrow_e_struct*_tmpDC=
_cycalloc(sizeof(*_tmpDC));_tmpDC[0]=({struct Cyc_Absyn_AggrArrow_e_struct _tmpDD;
_tmpDD.tag=24;_tmpDD.f1=e;_tmpDD.f2=n;_tmpDD.f3=0;_tmpDD.f4=0;_tmpDD;});_tmpDC;}),
loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*e1,struct
Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((void*)({
struct Cyc_Absyn_Subscript_e_struct*_tmpDE=_cycalloc(sizeof(*_tmpDE));_tmpDE[0]=({
struct Cyc_Absyn_Subscript_e_struct _tmpDF;_tmpDF.tag=25;_tmpDF.f1=e1;_tmpDF.f2=e2;
_tmpDF;});_tmpDE;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_tuple_exp(struct Cyc_List_List*
es,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Tuple_e_struct*
_tmpE0=_cycalloc(sizeof(*_tmpE0));_tmpE0[0]=({struct Cyc_Absyn_Tuple_e_struct
_tmpE1;_tmpE1.tag=26;_tmpE1.f1=es;_tmpE1;});_tmpE0;}),loc);}struct Cyc_Absyn_Exp*
Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*loc){return
Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_StmtExp_e_struct*_tmpE2=_cycalloc(
sizeof(*_tmpE2));_tmpE2[0]=({struct Cyc_Absyn_StmtExp_e_struct _tmpE3;_tmpE3.tag=
38;_tmpE3.f1=s;_tmpE3;});_tmpE2;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_match_exn_exp(
struct Cyc_Position_Segment*loc){return Cyc_Absyn_var_exp(Cyc_Absyn_Match_Exception_name,
loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*t,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Valueof_e_struct*_tmpE4=
_cycalloc(sizeof(*_tmpE4));_tmpE4[0]=({struct Cyc_Absyn_Valueof_e_struct _tmpE5;
_tmpE5.tag=39;_tmpE5.f1=(void*)t;_tmpE5;});_tmpE4;}),loc);}struct _tuple13{struct
Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Exp*Cyc_Absyn_array_exp(
struct Cyc_List_List*es,struct Cyc_Position_Segment*loc){struct Cyc_List_List*dles=
0;for(0;es != 0;es=es->tl){dles=({struct Cyc_List_List*_tmpE6=_cycalloc(sizeof(*
_tmpE6));_tmpE6->hd=({struct _tuple13*_tmpE7=_cycalloc(sizeof(*_tmpE7));_tmpE7->f1=
0;_tmpE7->f2=(struct Cyc_Absyn_Exp*)es->hd;_tmpE7;});_tmpE6->tl=dles;_tmpE6;});}
dles=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);
return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Array_e_struct*_tmpE8=_cycalloc(
sizeof(*_tmpE8));_tmpE8[0]=({struct Cyc_Absyn_Array_e_struct _tmpE9;_tmpE9.tag=28;
_tmpE9.f1=dles;_tmpE9;});_tmpE8;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_unresolvedmem_exp(
struct Cyc_Core_Opt*n,struct Cyc_List_List*dles,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_UnresolvedMem_e_struct*_tmpEA=
_cycalloc(sizeof(*_tmpEA));_tmpEA[0]=({struct Cyc_Absyn_UnresolvedMem_e_struct
_tmpEB;_tmpEB.tag=37;_tmpEB.f1=n;_tmpEB.f2=dles;_tmpEB;});_tmpEA;}),loc);}struct
Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*s,struct Cyc_Position_Segment*loc){return({
struct Cyc_Absyn_Stmt*_tmpEC=_cycalloc(sizeof(*_tmpEC));_tmpEC->r=(void*)s;_tmpEC->loc=
loc;_tmpEC->non_local_preds=0;_tmpEC->try_depth=0;_tmpEC->annot=(void*)((void*)
Cyc_Absyn_EmptyAnnot);_tmpEC;});}struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(struct
Cyc_Position_Segment*loc){return Cyc_Absyn_new_stmt((void*)0,loc);}struct Cyc_Absyn_Stmt*
Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc){return
Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Exp_s_struct*_tmpED=_cycalloc(
sizeof(*_tmpED));_tmpED[0]=({struct Cyc_Absyn_Exp_s_struct _tmpEE;_tmpEE.tag=0;
_tmpEE.f1=e;_tmpEE;});_tmpED;}),loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmts(
struct Cyc_List_List*ss,struct Cyc_Position_Segment*loc){if(ss == 0)return Cyc_Absyn_skip_stmt(
loc);else{if(ss->tl == 0)return(struct Cyc_Absyn_Stmt*)ss->hd;else{return Cyc_Absyn_seq_stmt((
struct Cyc_Absyn_Stmt*)ss->hd,Cyc_Absyn_seq_stmts(ss->tl,loc),loc);}}}struct Cyc_Absyn_Stmt*
Cyc_Absyn_return_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Return_s_struct*_tmpEF=
_cycalloc(sizeof(*_tmpEF));_tmpEF[0]=({struct Cyc_Absyn_Return_s_struct _tmpF0;
_tmpF0.tag=2;_tmpF0.f1=e;_tmpF0;});_tmpEF;}),loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(
struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_IfThenElse_s_struct*
_tmpF1=_cycalloc(sizeof(*_tmpF1));_tmpF1[0]=({struct Cyc_Absyn_IfThenElse_s_struct
_tmpF2;_tmpF2.tag=3;_tmpF2.f1=e;_tmpF2.f2=s1;_tmpF2.f3=s2;_tmpF2;});_tmpF1;}),
loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_while_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*
s,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_While_s_struct*
_tmpF3=_cycalloc(sizeof(*_tmpF3));_tmpF3[0]=({struct Cyc_Absyn_While_s_struct
_tmpF4;_tmpF4.tag=4;_tmpF4.f1=({struct _tuple2 _tmpF5;_tmpF5.f1=e;_tmpF5.f2=Cyc_Absyn_skip_stmt(
e->loc);_tmpF5;});_tmpF4.f2=s;_tmpF4;});_tmpF3;}),loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_break_stmt(
struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Break_s_struct*
_tmpF6=_cycalloc(sizeof(*_tmpF6));_tmpF6[0]=({struct Cyc_Absyn_Break_s_struct
_tmpF7;_tmpF7.tag=5;_tmpF7.f1=0;_tmpF7;});_tmpF6;}),loc);}struct Cyc_Absyn_Stmt*
Cyc_Absyn_continue_stmt(struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_stmt((
void*)({struct Cyc_Absyn_Continue_s_struct*_tmpF8=_cycalloc(sizeof(*_tmpF8));
_tmpF8[0]=({struct Cyc_Absyn_Continue_s_struct _tmpF9;_tmpF9.tag=6;_tmpF9.f1=0;
_tmpF9;});_tmpF8;}),loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_for_stmt(struct Cyc_Absyn_Exp*
e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3,struct Cyc_Absyn_Stmt*s,struct
Cyc_Position_Segment*loc){return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_For_s_struct*
_tmpFA=_cycalloc(sizeof(*_tmpFA));_tmpFA[0]=({struct Cyc_Absyn_For_s_struct _tmpFB;
_tmpFB.tag=8;_tmpFB.f1=e1;_tmpFB.f2=({struct _tuple2 _tmpFC;_tmpFC.f1=e2;_tmpFC.f2=
Cyc_Absyn_skip_stmt(e3->loc);_tmpFC;});_tmpFB.f3=({struct _tuple2 _tmpFD;_tmpFD.f1=
e3;_tmpFD.f2=Cyc_Absyn_skip_stmt(e3->loc);_tmpFD;});_tmpFB.f4=s;_tmpFB;});_tmpFA;}),
loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_switch_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*
scs,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Switch_s_struct*
_tmpFE=_cycalloc(sizeof(*_tmpFE));_tmpFE[0]=({struct Cyc_Absyn_Switch_s_struct
_tmpFF;_tmpFF.tag=9;_tmpFF.f1=e;_tmpFF.f2=scs;_tmpFF;});_tmpFE;}),loc);}struct
Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*
s2,struct Cyc_Position_Segment*loc){struct _tuple9 _tmp101=({struct _tuple9 _tmp100;
_tmp100.f1=(void*)s1->r;_tmp100.f2=(void*)s2->r;_tmp100;});void*_tmp102;void*
_tmp103;_LL64: _tmp102=_tmp101.f1;if((int)_tmp102 != 0)goto _LL66;_LL65: return s2;
_LL66: _tmp103=_tmp101.f2;if((int)_tmp103 != 0)goto _LL68;_LL67: return s1;_LL68:;
_LL69: return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Seq_s_struct*_tmp104=
_cycalloc(sizeof(*_tmp104));_tmp104[0]=({struct Cyc_Absyn_Seq_s_struct _tmp105;
_tmp105.tag=1;_tmp105.f1=s1;_tmp105.f2=s2;_tmp105;});_tmp104;}),loc);_LL63:;}
struct Cyc_Absyn_Stmt*Cyc_Absyn_fallthru_stmt(struct Cyc_List_List*el,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Fallthru_s_struct*_tmp106=
_cycalloc(sizeof(*_tmp106));_tmp106[0]=({struct Cyc_Absyn_Fallthru_s_struct
_tmp107;_tmp107.tag=10;_tmp107.f1=el;_tmp107.f2=0;_tmp107;});_tmp106;}),loc);}
struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*
s,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Decl_s_struct*
_tmp108=_cycalloc(sizeof(*_tmp108));_tmp108[0]=({struct Cyc_Absyn_Decl_s_struct
_tmp109;_tmp109.tag=11;_tmp109.f1=d;_tmp109.f2=s;_tmp109;});_tmp108;}),loc);}
struct Cyc_Absyn_Stmt*Cyc_Absyn_declare_stmt(struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*
init,struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*loc){struct Cyc_Absyn_Decl*
d=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_struct*_tmp10C=_cycalloc(
sizeof(*_tmp10C));_tmp10C[0]=({struct Cyc_Absyn_Var_d_struct _tmp10D;_tmp10D.tag=0;
_tmp10D.f1=Cyc_Absyn_new_vardecl(x,t,init);_tmp10D;});_tmp10C;}),loc);return Cyc_Absyn_new_stmt((
void*)({struct Cyc_Absyn_Decl_s_struct*_tmp10A=_cycalloc(sizeof(*_tmp10A));
_tmp10A[0]=({struct Cyc_Absyn_Decl_s_struct _tmp10B;_tmp10B.tag=11;_tmp10B.f1=d;
_tmp10B.f2=s;_tmp10B;});_tmp10A;}),loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_label_stmt(
struct _dyneither_ptr*v,struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Label_s_struct*_tmp10E=
_cycalloc(sizeof(*_tmp10E));_tmp10E[0]=({struct Cyc_Absyn_Label_s_struct _tmp10F;
_tmp10F.tag=12;_tmp10F.f1=v;_tmp10F.f2=s;_tmp10F;});_tmp10E;}),loc);}struct Cyc_Absyn_Stmt*
Cyc_Absyn_do_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Do_s_struct*_tmp110=
_cycalloc(sizeof(*_tmp110));_tmp110[0]=({struct Cyc_Absyn_Do_s_struct _tmp111;
_tmp111.tag=13;_tmp111.f1=s;_tmp111.f2=({struct _tuple2 _tmp112;_tmp112.f1=e;
_tmp112.f2=Cyc_Absyn_skip_stmt(e->loc);_tmp112;});_tmp111;});_tmp110;}),loc);}
struct Cyc_Absyn_Stmt*Cyc_Absyn_trycatch_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*
scs,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_TryCatch_s_struct*
_tmp113=_cycalloc(sizeof(*_tmp113));_tmp113[0]=({struct Cyc_Absyn_TryCatch_s_struct
_tmp114;_tmp114.tag=14;_tmp114.f1=s;_tmp114.f2=scs;_tmp114;});_tmp113;}),loc);}
struct Cyc_Absyn_Stmt*Cyc_Absyn_goto_stmt(struct _dyneither_ptr*lab,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Goto_s_struct*_tmp115=
_cycalloc(sizeof(*_tmp115));_tmp115[0]=({struct Cyc_Absyn_Goto_s_struct _tmp116;
_tmp116.tag=7;_tmp116.f1=lab;_tmp116.f2=0;_tmp116;});_tmp115;}),loc);}struct Cyc_Absyn_Stmt*
Cyc_Absyn_assign_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(e1,e2,loc),loc);}struct Cyc_Absyn_Pat*
Cyc_Absyn_new_pat(void*p,struct Cyc_Position_Segment*s){return({struct Cyc_Absyn_Pat*
_tmp117=_cycalloc(sizeof(*_tmp117));_tmp117->r=(void*)p;_tmp117->topt=0;_tmp117->loc=
s;_tmp117;});}struct Cyc_Absyn_Pat*Cyc_Absyn_exp_pat(struct Cyc_Absyn_Exp*e){
return Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Exp_p_struct*_tmp118=_cycalloc(
sizeof(*_tmp118));_tmp118[0]=({struct Cyc_Absyn_Exp_p_struct _tmp119;_tmp119.tag=
14;_tmp119.f1=e;_tmp119;});_tmp118;}),e->loc);}struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(
void*r,struct Cyc_Position_Segment*loc){return({struct Cyc_Absyn_Decl*_tmp11A=
_cycalloc(sizeof(*_tmp11A));_tmp11A->r=(void*)r;_tmp11A->loc=loc;_tmp11A;});}
struct Cyc_Absyn_Decl*Cyc_Absyn_let_decl(struct Cyc_Absyn_Pat*p,struct Cyc_Absyn_Exp*
e,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Let_d_struct*
_tmp11B=_cycalloc(sizeof(*_tmp11B));_tmp11B[0]=({struct Cyc_Absyn_Let_d_struct
_tmp11C;_tmp11C.tag=2;_tmp11C.f1=p;_tmp11C.f2=0;_tmp11C.f3=e;_tmp11C;});_tmp11B;}),
loc);}struct Cyc_Absyn_Decl*Cyc_Absyn_letv_decl(struct Cyc_List_List*vds,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Letv_d_struct*_tmp11D=
_cycalloc(sizeof(*_tmp11D));_tmp11D[0]=({struct Cyc_Absyn_Letv_d_struct _tmp11E;
_tmp11E.tag=3;_tmp11E.f1=vds;_tmp11E;});_tmp11D;}),loc);}struct Cyc_Absyn_Decl*
Cyc_Absyn_region_decl(struct Cyc_Absyn_Tvar*tv,struct Cyc_Absyn_Vardecl*vd,int
resetable,struct Cyc_Absyn_Exp*eo,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Region_d_struct*_tmp11F=_cycalloc(sizeof(*_tmp11F));
_tmp11F[0]=({struct Cyc_Absyn_Region_d_struct _tmp120;_tmp120.tag=4;_tmp120.f1=tv;
_tmp120.f2=vd;_tmp120.f3=resetable;_tmp120.f4=eo;_tmp120;});_tmp11F;}),loc);}
struct Cyc_Absyn_Decl*Cyc_Absyn_alias_decl(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Tvar*
tv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Alias_d_struct*_tmp121=_cycalloc(sizeof(*_tmp121));
_tmp121[0]=({struct Cyc_Absyn_Alias_d_struct _tmp122;_tmp122.tag=5;_tmp122.f1=e;
_tmp122.f2=tv;_tmp122.f3=vd;_tmp122;});_tmp121;}),loc);}struct Cyc_Absyn_Vardecl*
Cyc_Absyn_new_vardecl(struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*init){return({
struct Cyc_Absyn_Vardecl*_tmp123=_cycalloc(sizeof(*_tmp123));_tmp123->sc=(void*)((
void*)2);_tmp123->name=x;_tmp123->tq=Cyc_Absyn_empty_tqual(0);_tmp123->type=(
void*)t;_tmp123->initializer=init;_tmp123->rgn=0;_tmp123->attributes=0;_tmp123->escapes=
0;_tmp123;});}struct Cyc_Absyn_Vardecl*Cyc_Absyn_static_vardecl(struct _tuple0*x,
void*t,struct Cyc_Absyn_Exp*init){return({struct Cyc_Absyn_Vardecl*_tmp124=
_cycalloc(sizeof(*_tmp124));_tmp124->sc=(void*)((void*)0);_tmp124->name=x;
_tmp124->tq=Cyc_Absyn_empty_tqual(0);_tmp124->type=(void*)t;_tmp124->initializer=
init;_tmp124->rgn=0;_tmp124->attributes=0;_tmp124->escapes=0;_tmp124;});}struct
Cyc_Absyn_AggrdeclImpl*Cyc_Absyn_aggrdecl_impl(struct Cyc_List_List*exists,struct
Cyc_List_List*po,struct Cyc_List_List*fs,int tagged){return({struct Cyc_Absyn_AggrdeclImpl*
_tmp125=_cycalloc(sizeof(*_tmp125));_tmp125->exist_vars=exists;_tmp125->rgn_po=
po;_tmp125->fields=fs;_tmp125->tagged=tagged;_tmp125;});}struct Cyc_Absyn_Decl*
Cyc_Absyn_aggr_decl(void*k,void*s,struct _tuple0*n,struct Cyc_List_List*ts,struct
Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*atts,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_struct*_tmp126=
_cycalloc(sizeof(*_tmp126));_tmp126[0]=({struct Cyc_Absyn_Aggr_d_struct _tmp127;
_tmp127.tag=6;_tmp127.f1=({struct Cyc_Absyn_Aggrdecl*_tmp128=_cycalloc(sizeof(*
_tmp128));_tmp128->kind=(void*)k;_tmp128->sc=(void*)s;_tmp128->name=n;_tmp128->tvs=
ts;_tmp128->impl=i;_tmp128->attributes=atts;_tmp128;});_tmp127;});_tmp126;}),loc);}
struct Cyc_Absyn_Decl*Cyc_Absyn_struct_decl(void*s,struct _tuple0*n,struct Cyc_List_List*
ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*atts,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_aggr_decl((void*)0,s,n,ts,i,atts,loc);}struct Cyc_Absyn_Decl*
Cyc_Absyn_union_decl(void*s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*
i,struct Cyc_List_List*atts,struct Cyc_Position_Segment*loc){return Cyc_Absyn_aggr_decl((
void*)1,s,n,ts,i,atts,loc);}struct Cyc_Absyn_Decl*Cyc_Absyn_datatype_decl(void*s,
struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Core_Opt*fs,int is_extensible,
int is_flat,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_decl((void*)({
struct Cyc_Absyn_Datatype_d_struct*_tmp129=_cycalloc(sizeof(*_tmp129));_tmp129[0]=({
struct Cyc_Absyn_Datatype_d_struct _tmp12A;_tmp12A.tag=7;_tmp12A.f1=({struct Cyc_Absyn_Datatypedecl*
_tmp12B=_cycalloc(sizeof(*_tmp12B));_tmp12B->sc=(void*)s;_tmp12B->name=n;_tmp12B->tvs=
ts;_tmp12B->fields=fs;_tmp12B->is_extensible=is_extensible;_tmp12B->is_flat=
is_flat;_tmp12B;});_tmp12A;});_tmp129;}),loc);}static struct _tuple1*Cyc_Absyn_expand_arg(
struct _tuple1*a){return({struct _tuple1*_tmp12C=_cycalloc(sizeof(*_tmp12C));
_tmp12C->f1=(*a).f1;_tmp12C->f2=(*a).f2;_tmp12C->f3=Cyc_Absyn_pointer_expand((*a).f3,
1);_tmp12C;});}void*Cyc_Absyn_function_typ(struct Cyc_List_List*tvs,struct Cyc_Core_Opt*
eff_typ,void*ret_typ,struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*
cyc_varargs,struct Cyc_List_List*rgn_po,struct Cyc_List_List*atts){return(void*)({
struct Cyc_Absyn_FnType_struct*_tmp12D=_cycalloc(sizeof(*_tmp12D));_tmp12D[0]=({
struct Cyc_Absyn_FnType_struct _tmp12E;_tmp12E.tag=8;_tmp12E.f1=({struct Cyc_Absyn_FnInfo
_tmp12F;_tmp12F.tvars=tvs;_tmp12F.ret_typ=(void*)Cyc_Absyn_pointer_expand(
ret_typ,0);_tmp12F.effect=eff_typ;_tmp12F.args=((struct Cyc_List_List*(*)(struct
_tuple1*(*f)(struct _tuple1*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absyn_expand_arg,
args);_tmp12F.c_varargs=c_varargs;_tmp12F.cyc_varargs=cyc_varargs;_tmp12F.rgn_po=
rgn_po;_tmp12F.attributes=atts;_tmp12F;});_tmp12E;});_tmp12D;});}void*Cyc_Absyn_pointer_expand(
void*t,int fresh_evar){void*_tmp130=Cyc_Tcutil_compress(t);_LL6B: if(_tmp130 <= (
void*)4)goto _LL6D;if(*((int*)_tmp130)!= 8)goto _LL6D;_LL6C: return Cyc_Absyn_at_typ(
t,fresh_evar?Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmp131=_cycalloc(sizeof(*
_tmp131));_tmp131->v=(void*)((void*)3);_tmp131;}),0):(void*)2,Cyc_Absyn_empty_tqual(
0),Cyc_Absyn_false_conref);_LL6D:;_LL6E: return t;_LL6A:;}int Cyc_Absyn_is_lvalue(
struct Cyc_Absyn_Exp*e){void*_tmp132=(void*)e->r;void*_tmp133;void*_tmp134;struct
Cyc_Absyn_Vardecl*_tmp135;void*_tmp136;struct Cyc_Absyn_Vardecl*_tmp137;struct Cyc_Absyn_Exp*
_tmp138;struct Cyc_Absyn_Exp*_tmp139;struct Cyc_Absyn_Exp*_tmp13A;_LL70: if(*((int*)
_tmp132)!= 1)goto _LL72;_tmp133=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp132)->f2;
if(_tmp133 <= (void*)1)goto _LL72;if(*((int*)_tmp133)!= 1)goto _LL72;_LL71: return 0;
_LL72: if(*((int*)_tmp132)!= 1)goto _LL74;_tmp134=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp132)->f2;if(_tmp134 <= (void*)1)goto _LL74;if(*((int*)_tmp134)!= 0)goto _LL74;
_tmp135=((struct Cyc_Absyn_Global_b_struct*)_tmp134)->f1;_LL73: _tmp137=_tmp135;
goto _LL75;_LL74: if(*((int*)_tmp132)!= 1)goto _LL76;_tmp136=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp132)->f2;if(_tmp136 <= (void*)1)goto _LL76;if(*((int*)_tmp136)!= 3)goto _LL76;
_tmp137=((struct Cyc_Absyn_Local_b_struct*)_tmp136)->f1;_LL75: {void*_tmp13B=Cyc_Tcutil_compress((
void*)_tmp137->type);_LL87: if(_tmp13B <= (void*)4)goto _LL89;if(*((int*)_tmp13B)!= 
7)goto _LL89;_LL88: return 0;_LL89:;_LL8A: return 1;_LL86:;}_LL76: if(*((int*)_tmp132)
!= 1)goto _LL78;_LL77: goto _LL79;_LL78: if(*((int*)_tmp132)!= 24)goto _LL7A;_LL79:
goto _LL7B;_LL7A: if(*((int*)_tmp132)!= 22)goto _LL7C;_LL7B: goto _LL7D;_LL7C: if(*((
int*)_tmp132)!= 25)goto _LL7E;_LL7D: return 1;_LL7E: if(*((int*)_tmp132)!= 23)goto
_LL80;_tmp138=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp132)->f1;_LL7F: return
Cyc_Absyn_is_lvalue(_tmp138);_LL80: if(*((int*)_tmp132)!= 14)goto _LL82;_tmp139=((
struct Cyc_Absyn_Instantiate_e_struct*)_tmp132)->f1;_LL81: return Cyc_Absyn_is_lvalue(
_tmp139);_LL82: if(*((int*)_tmp132)!= 13)goto _LL84;_tmp13A=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmp132)->f1;_LL83: return Cyc_Absyn_is_lvalue(_tmp13A);_LL84:;_LL85: return 0;_LL6F:;}
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*fields,
struct _dyneither_ptr*v){{struct Cyc_List_List*_tmp13C=fields;for(0;_tmp13C != 0;
_tmp13C=_tmp13C->tl){if(Cyc_strptrcmp(((struct Cyc_Absyn_Aggrfield*)_tmp13C->hd)->name,
v)== 0)return(struct Cyc_Absyn_Aggrfield*)((struct Cyc_Absyn_Aggrfield*)_tmp13C->hd);}}
return 0;}struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(struct Cyc_Absyn_Aggrdecl*
ad,struct _dyneither_ptr*v){return ad->impl == 0?0: Cyc_Absyn_lookup_field(((struct
Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields,v);}struct _tuple3*Cyc_Absyn_lookup_tuple_field(
struct Cyc_List_List*ts,int i){for(0;i != 0;-- i){if(ts == 0)return 0;ts=ts->tl;}if(ts
== 0)return 0;return(struct _tuple3*)((struct _tuple3*)ts->hd);}struct
_dyneither_ptr Cyc_Absyn_attribute2string(void*a){void*_tmp13D=a;int _tmp13E;int
_tmp13F;struct _dyneither_ptr _tmp140;void*_tmp141;int _tmp142;int _tmp143;void*
_tmp144;int _tmp145;int _tmp146;int _tmp147;struct _dyneither_ptr _tmp148;_LL8C: if(
_tmp13D <= (void*)17)goto _LL8E;if(*((int*)_tmp13D)!= 0)goto _LL8E;_tmp13E=((struct
Cyc_Absyn_Regparm_att_struct*)_tmp13D)->f1;_LL8D: return(struct _dyneither_ptr)({
struct Cyc_Int_pa_struct _tmp14B;_tmp14B.tag=1;_tmp14B.f1=(unsigned long)_tmp13E;{
void*_tmp149[1]={& _tmp14B};Cyc_aprintf(({const char*_tmp14A="regparm(%d)";
_tag_dyneither(_tmp14A,sizeof(char),12);}),_tag_dyneither(_tmp149,sizeof(void*),
1));}});_LL8E: if((int)_tmp13D != 0)goto _LL90;_LL8F: return({const char*_tmp14C="stdcall";
_tag_dyneither(_tmp14C,sizeof(char),8);});_LL90: if((int)_tmp13D != 1)goto _LL92;
_LL91: return({const char*_tmp14D="cdecl";_tag_dyneither(_tmp14D,sizeof(char),6);});
_LL92: if((int)_tmp13D != 2)goto _LL94;_LL93: return({const char*_tmp14E="fastcall";
_tag_dyneither(_tmp14E,sizeof(char),9);});_LL94: if((int)_tmp13D != 3)goto _LL96;
_LL95: return({const char*_tmp14F="noreturn";_tag_dyneither(_tmp14F,sizeof(char),9);});
_LL96: if((int)_tmp13D != 4)goto _LL98;_LL97: return({const char*_tmp150="const";
_tag_dyneither(_tmp150,sizeof(char),6);});_LL98: if(_tmp13D <= (void*)17)goto _LL9A;
if(*((int*)_tmp13D)!= 1)goto _LL9A;_tmp13F=((struct Cyc_Absyn_Aligned_att_struct*)
_tmp13D)->f1;_LL99: if(_tmp13F == - 1)return({const char*_tmp151="aligned";
_tag_dyneither(_tmp151,sizeof(char),8);});else{void*_tmp152=Cyc_Cyclone_c_compiler;
_LLBD: if((int)_tmp152 != 0)goto _LLBF;_LLBE: return(struct _dyneither_ptr)({struct
Cyc_Int_pa_struct _tmp155;_tmp155.tag=1;_tmp155.f1=(unsigned long)_tmp13F;{void*
_tmp153[1]={& _tmp155};Cyc_aprintf(({const char*_tmp154="aligned(%d)";
_tag_dyneither(_tmp154,sizeof(char),12);}),_tag_dyneither(_tmp153,sizeof(void*),
1));}});_LLBF: if((int)_tmp152 != 1)goto _LLBC;_LLC0: return(struct _dyneither_ptr)({
struct Cyc_Int_pa_struct _tmp158;_tmp158.tag=1;_tmp158.f1=(unsigned long)_tmp13F;{
void*_tmp156[1]={& _tmp158};Cyc_aprintf(({const char*_tmp157="align(%d)";
_tag_dyneither(_tmp157,sizeof(char),10);}),_tag_dyneither(_tmp156,sizeof(void*),
1));}});_LLBC:;}_LL9A: if((int)_tmp13D != 5)goto _LL9C;_LL9B: return({const char*
_tmp159="packed";_tag_dyneither(_tmp159,sizeof(char),7);});_LL9C: if(_tmp13D <= (
void*)17)goto _LL9E;if(*((int*)_tmp13D)!= 2)goto _LL9E;_tmp140=((struct Cyc_Absyn_Section_att_struct*)
_tmp13D)->f1;_LL9D: return(struct _dyneither_ptr)({struct Cyc_String_pa_struct
_tmp15C;_tmp15C.tag=0;_tmp15C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
_tmp140);{void*_tmp15A[1]={& _tmp15C};Cyc_aprintf(({const char*_tmp15B="section(\"%s\")";
_tag_dyneither(_tmp15B,sizeof(char),14);}),_tag_dyneither(_tmp15A,sizeof(void*),
1));}});_LL9E: if((int)_tmp13D != 6)goto _LLA0;_LL9F: return({const char*_tmp15D="nocommon";
_tag_dyneither(_tmp15D,sizeof(char),9);});_LLA0: if((int)_tmp13D != 7)goto _LLA2;
_LLA1: return({const char*_tmp15E="shared";_tag_dyneither(_tmp15E,sizeof(char),7);});
_LLA2: if((int)_tmp13D != 8)goto _LLA4;_LLA3: return({const char*_tmp15F="unused";
_tag_dyneither(_tmp15F,sizeof(char),7);});_LLA4: if((int)_tmp13D != 9)goto _LLA6;
_LLA5: return({const char*_tmp160="weak";_tag_dyneither(_tmp160,sizeof(char),5);});
_LLA6: if((int)_tmp13D != 10)goto _LLA8;_LLA7: return({const char*_tmp161="dllimport";
_tag_dyneither(_tmp161,sizeof(char),10);});_LLA8: if((int)_tmp13D != 11)goto _LLAA;
_LLA9: return({const char*_tmp162="dllexport";_tag_dyneither(_tmp162,sizeof(char),
10);});_LLAA: if((int)_tmp13D != 12)goto _LLAC;_LLAB: return({const char*_tmp163="no_instrument_function";
_tag_dyneither(_tmp163,sizeof(char),23);});_LLAC: if((int)_tmp13D != 13)goto _LLAE;
_LLAD: return({const char*_tmp164="constructor";_tag_dyneither(_tmp164,sizeof(char),
12);});_LLAE: if((int)_tmp13D != 14)goto _LLB0;_LLAF: return({const char*_tmp165="destructor";
_tag_dyneither(_tmp165,sizeof(char),11);});_LLB0: if((int)_tmp13D != 15)goto _LLB2;
_LLB1: return({const char*_tmp166="no_check_memory_usage";_tag_dyneither(_tmp166,
sizeof(char),22);});_LLB2: if(_tmp13D <= (void*)17)goto _LLB8;if(*((int*)_tmp13D)!= 
3)goto _LLB4;_tmp141=(void*)((struct Cyc_Absyn_Format_att_struct*)_tmp13D)->f1;if((
int)_tmp141 != 0)goto _LLB4;_tmp142=((struct Cyc_Absyn_Format_att_struct*)_tmp13D)->f2;
_tmp143=((struct Cyc_Absyn_Format_att_struct*)_tmp13D)->f3;_LLB3: return(struct
_dyneither_ptr)({struct Cyc_Int_pa_struct _tmp16A;_tmp16A.tag=1;_tmp16A.f1=(
unsigned int)_tmp143;{struct Cyc_Int_pa_struct _tmp169;_tmp169.tag=1;_tmp169.f1=(
unsigned int)_tmp142;{void*_tmp167[2]={& _tmp169,& _tmp16A};Cyc_aprintf(({const
char*_tmp168="format(printf,%u,%u)";_tag_dyneither(_tmp168,sizeof(char),21);}),
_tag_dyneither(_tmp167,sizeof(void*),2));}}});_LLB4: if(*((int*)_tmp13D)!= 3)goto
_LLB6;_tmp144=(void*)((struct Cyc_Absyn_Format_att_struct*)_tmp13D)->f1;if((int)
_tmp144 != 1)goto _LLB6;_tmp145=((struct Cyc_Absyn_Format_att_struct*)_tmp13D)->f2;
_tmp146=((struct Cyc_Absyn_Format_att_struct*)_tmp13D)->f3;_LLB5: return(struct
_dyneither_ptr)({struct Cyc_Int_pa_struct _tmp16E;_tmp16E.tag=1;_tmp16E.f1=(
unsigned int)_tmp146;{struct Cyc_Int_pa_struct _tmp16D;_tmp16D.tag=1;_tmp16D.f1=(
unsigned int)_tmp145;{void*_tmp16B[2]={& _tmp16D,& _tmp16E};Cyc_aprintf(({const
char*_tmp16C="format(scanf,%u,%u)";_tag_dyneither(_tmp16C,sizeof(char),20);}),
_tag_dyneither(_tmp16B,sizeof(void*),2));}}});_LLB6: if(*((int*)_tmp13D)!= 4)goto
_LLB8;_tmp147=((struct Cyc_Absyn_Initializes_att_struct*)_tmp13D)->f1;_LLB7:
return(struct _dyneither_ptr)({struct Cyc_Int_pa_struct _tmp171;_tmp171.tag=1;
_tmp171.f1=(unsigned long)_tmp147;{void*_tmp16F[1]={& _tmp171};Cyc_aprintf(({
const char*_tmp170="initializes(%d)";_tag_dyneither(_tmp170,sizeof(char),16);}),
_tag_dyneither(_tmp16F,sizeof(void*),1));}});_LLB8: if((int)_tmp13D != 16)goto
_LLBA;_LLB9: return({const char*_tmp172="pure";_tag_dyneither(_tmp172,sizeof(char),
5);});_LLBA: if(_tmp13D <= (void*)17)goto _LL8B;if(*((int*)_tmp13D)!= 5)goto _LL8B;
_tmp148=((struct Cyc_Absyn_Mode_att_struct*)_tmp13D)->f1;_LLBB: return(struct
_dyneither_ptr)({struct Cyc_String_pa_struct _tmp175;_tmp175.tag=0;_tmp175.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)_tmp148);{void*_tmp173[1]={& _tmp175};
Cyc_aprintf(({const char*_tmp174="__mode__(\"%s\")";_tag_dyneither(_tmp174,
sizeof(char),15);}),_tag_dyneither(_tmp173,sizeof(void*),1));}});_LL8B:;}int Cyc_Absyn_fntype_att(
void*a){void*_tmp176=a;_LLC2: if(_tmp176 <= (void*)17)goto _LLC4;if(*((int*)_tmp176)
!= 0)goto _LLC4;_LLC3: goto _LLC5;_LLC4: if((int)_tmp176 != 2)goto _LLC6;_LLC5: goto
_LLC7;_LLC6: if((int)_tmp176 != 0)goto _LLC8;_LLC7: goto _LLC9;_LLC8: if((int)_tmp176
!= 1)goto _LLCA;_LLC9: goto _LLCB;_LLCA: if((int)_tmp176 != 3)goto _LLCC;_LLCB: goto
_LLCD;_LLCC: if((int)_tmp176 != 16)goto _LLCE;_LLCD: goto _LLCF;_LLCE: if(_tmp176 <= (
void*)17)goto _LLD0;if(*((int*)_tmp176)!= 3)goto _LLD0;_LLCF: goto _LLD1;_LLD0: if((
int)_tmp176 != 4)goto _LLD2;_LLD1: return 1;_LLD2: if(_tmp176 <= (void*)17)goto _LLD4;
if(*((int*)_tmp176)!= 4)goto _LLD4;_LLD3: return 1;_LLD4:;_LLD5: return 0;_LLC1:;}
static char _tmp177[3]="f0";static struct _dyneither_ptr Cyc_Absyn_f0={_tmp177,
_tmp177,_tmp177 + 3};static struct _dyneither_ptr*Cyc_Absyn_field_names_v[1]={& Cyc_Absyn_f0};
static struct _dyneither_ptr Cyc_Absyn_field_names={(void*)((struct _dyneither_ptr**)
Cyc_Absyn_field_names_v),(void*)((struct _dyneither_ptr**)Cyc_Absyn_field_names_v),(
void*)((struct _dyneither_ptr**)Cyc_Absyn_field_names_v + 1)};struct _dyneither_ptr*
Cyc_Absyn_fieldname(int i){unsigned int fsz=_get_dyneither_size(Cyc_Absyn_field_names,
sizeof(struct _dyneither_ptr*));if(i >= fsz)Cyc_Absyn_field_names=({unsigned int
_tmp178=(unsigned int)(i + 1);struct _dyneither_ptr**_tmp179=(struct _dyneither_ptr**)
_cycalloc(_check_times(sizeof(struct _dyneither_ptr*),_tmp178));struct
_dyneither_ptr _tmp17F=_tag_dyneither(_tmp179,sizeof(struct _dyneither_ptr*),
_tmp178);{unsigned int _tmp17A=_tmp178;unsigned int j;for(j=0;j < _tmp17A;j ++){
_tmp179[j]=j < fsz?*((struct _dyneither_ptr**)_check_dyneither_subscript(Cyc_Absyn_field_names,
sizeof(struct _dyneither_ptr*),(int)j)):({struct _dyneither_ptr*_tmp17B=_cycalloc(
sizeof(*_tmp17B));_tmp17B[0]=(struct _dyneither_ptr)({struct Cyc_Int_pa_struct
_tmp17E;_tmp17E.tag=1;_tmp17E.f1=(unsigned long)((int)j);{void*_tmp17C[1]={&
_tmp17E};Cyc_aprintf(({const char*_tmp17D="f%d";_tag_dyneither(_tmp17D,sizeof(
char),4);}),_tag_dyneither(_tmp17C,sizeof(void*),1));}});_tmp17B;});}}_tmp17F;});
return*((struct _dyneither_ptr**)_check_dyneither_subscript(Cyc_Absyn_field_names,
sizeof(struct _dyneither_ptr*),i));}struct _tuple4 Cyc_Absyn_aggr_kinded_name(union
Cyc_Absyn_AggrInfoU_union info){union Cyc_Absyn_AggrInfoU_union _tmp180=info;void*
_tmp181;struct _tuple0*_tmp182;struct Cyc_Absyn_Aggrdecl**_tmp183;struct Cyc_Absyn_Aggrdecl*
_tmp184;struct Cyc_Absyn_Aggrdecl _tmp185;void*_tmp186;struct _tuple0*_tmp187;_LLD7:
if((_tmp180.UnknownAggr).tag != 0)goto _LLD9;_tmp181=(_tmp180.UnknownAggr).f1;
_tmp182=(_tmp180.UnknownAggr).f2;_LLD8: return({struct _tuple4 _tmp188;_tmp188.f1=
_tmp181;_tmp188.f2=_tmp182;_tmp188;});_LLD9: if((_tmp180.KnownAggr).tag != 1)goto
_LLD6;_tmp183=(_tmp180.KnownAggr).f1;_tmp184=*_tmp183;_tmp185=*_tmp184;_tmp186=(
void*)_tmp185.kind;_tmp187=_tmp185.name;_LLDA: return({struct _tuple4 _tmp189;
_tmp189.f1=_tmp186;_tmp189.f2=_tmp187;_tmp189;});_LLD6:;}struct Cyc_Absyn_Aggrdecl*
Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfoU_union info){union Cyc_Absyn_AggrInfoU_union
_tmp18A=info;void*_tmp18B;struct _tuple0*_tmp18C;struct Cyc_Absyn_Aggrdecl**
_tmp18D;struct Cyc_Absyn_Aggrdecl*_tmp18E;_LLDC: if((_tmp18A.UnknownAggr).tag != 0)
goto _LLDE;_tmp18B=(_tmp18A.UnknownAggr).f1;_tmp18C=(_tmp18A.UnknownAggr).f2;
_LLDD:({void*_tmp18F=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(({const char*_tmp190="unchecked aggrdecl";_tag_dyneither(
_tmp190,sizeof(char),19);}),_tag_dyneither(_tmp18F,sizeof(void*),0));});_LLDE:
if((_tmp18A.KnownAggr).tag != 1)goto _LLDB;_tmp18D=(_tmp18A.KnownAggr).f1;_tmp18E=*
_tmp18D;_LLDF: return _tmp18E;_LLDB:;}int Cyc_Absyn_is_union_type(void*t){void*
_tmp191=Cyc_Tcutil_compress(t);void*_tmp192;struct Cyc_Absyn_AggrInfo _tmp193;
union Cyc_Absyn_AggrInfoU_union _tmp194;_LLE1: if(_tmp191 <= (void*)4)goto _LLE5;if(*((
int*)_tmp191)!= 11)goto _LLE3;_tmp192=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp191)->f1;if((int)_tmp192 != 1)goto _LLE3;_LLE2: return 1;_LLE3: if(*((int*)
_tmp191)!= 10)goto _LLE5;_tmp193=((struct Cyc_Absyn_AggrType_struct*)_tmp191)->f1;
_tmp194=_tmp193.aggr_info;_LLE4: return(Cyc_Absyn_aggr_kinded_name(_tmp194)).f1 == (
void*)1;_LLE5:;_LLE6: return 0;_LLE0:;}int Cyc_Absyn_is_nontagged_union_type(void*t){
void*_tmp195=Cyc_Tcutil_compress(t);void*_tmp196;struct Cyc_Absyn_AggrInfo _tmp197;
union Cyc_Absyn_AggrInfoU_union _tmp198;_LLE8: if(_tmp195 <= (void*)4)goto _LLEC;if(*((
int*)_tmp195)!= 11)goto _LLEA;_tmp196=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp195)->f1;if((int)_tmp196 != 1)goto _LLEA;_LLE9: return 1;_LLEA: if(*((int*)
_tmp195)!= 10)goto _LLEC;_tmp197=((struct Cyc_Absyn_AggrType_struct*)_tmp195)->f1;
_tmp198=_tmp197.aggr_info;_LLEB: {union Cyc_Absyn_AggrInfoU_union _tmp199=_tmp198;
struct Cyc_Absyn_Aggrdecl**_tmp19A;struct Cyc_Absyn_Aggrdecl*_tmp19B;void*_tmp19C;
_LLEF: if((_tmp199.KnownAggr).tag != 1)goto _LLF1;_tmp19A=(_tmp199.KnownAggr).f1;
_tmp19B=*_tmp19A;_LLF0: return(void*)_tmp19B->kind == (void*)1  && ((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp19B->impl))->tagged == 0;_LLF1: if((_tmp199.UnknownAggr).tag != 0)
goto _LLEE;_tmp19C=(_tmp199.UnknownAggr).f1;_LLF2: return _tmp19C == (void*)1;_LLEE:;}
_LLEC:;_LLED: return 0;_LLE7:;}int Cyc_Absyn_is_aggr_type(void*t){void*_tmp19D=Cyc_Tcutil_compress(
t);_LLF4: if(_tmp19D <= (void*)4)goto _LLFE;if(*((int*)_tmp19D)!= 2)goto _LLF6;_LLF5:
goto _LLF7;_LLF6: if(*((int*)_tmp19D)!= 3)goto _LLF8;_LLF7: goto _LLF9;_LLF8: if(*((
int*)_tmp19D)!= 9)goto _LLFA;_LLF9: goto _LLFB;_LLFA: if(*((int*)_tmp19D)!= 10)goto
_LLFC;_LLFB: goto _LLFD;_LLFC: if(*((int*)_tmp19D)!= 11)goto _LLFE;_LLFD: return 1;
_LLFE:;_LLFF: return 0;_LLF3:;}void Cyc_Absyn_print_decls(struct Cyc_List_List*decls){((
void(*)(void*rep,struct Cyc_List_List**val))Cyc_Marshal_print_type)(Cyc_decls_rep,&
decls);({void*_tmp19E=0;Cyc_printf(({const char*_tmp19F="\n";_tag_dyneither(
_tmp19F,sizeof(char),2);}),_tag_dyneither(_tmp19E,sizeof(void*),0));});}int Cyc_Absyn_porting_c_code=
0;int Cyc_Absyn_no_regions=0;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_0;
extern struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_decl_t46H2_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_1;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_Decl_rep;extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_raw_decl_t_rep;
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_803;static struct Cyc_Typerep_Int_struct
Cyc__genrep_24={0,0,32};extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_148;
extern struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Vardecl_rep;extern struct
Cyc_Typerep_TUnion_struct Cyc_Absyn_scope_t_rep;static char _tmp1A1[7]="Static";
static struct _tuple7 Cyc__gentuple_156={0,(struct _dyneither_ptr){_tmp1A1,_tmp1A1,
_tmp1A1 + 7}};static char _tmp1A2[9]="Abstract";static struct _tuple7 Cyc__gentuple_157={
1,(struct _dyneither_ptr){_tmp1A2,_tmp1A2,_tmp1A2 + 9}};static char _tmp1A3[7]="Public";
static struct _tuple7 Cyc__gentuple_158={2,(struct _dyneither_ptr){_tmp1A3,_tmp1A3,
_tmp1A3 + 7}};static char _tmp1A4[7]="Extern";static struct _tuple7 Cyc__gentuple_159={
3,(struct _dyneither_ptr){_tmp1A4,_tmp1A4,_tmp1A4 + 7}};static char _tmp1A5[8]="ExternC";
static struct _tuple7 Cyc__gentuple_160={4,(struct _dyneither_ptr){_tmp1A5,_tmp1A5,
_tmp1A5 + 8}};static char _tmp1A6[9]="Register";static struct _tuple7 Cyc__gentuple_161={
5,(struct _dyneither_ptr){_tmp1A6,_tmp1A6,_tmp1A6 + 9}};static struct _tuple7*Cyc__genarr_162[
6]={& Cyc__gentuple_156,& Cyc__gentuple_157,& Cyc__gentuple_158,& Cyc__gentuple_159,&
Cyc__gentuple_160,& Cyc__gentuple_161};static struct _tuple5*Cyc__genarr_163[0]={};
static char _tmp1A8[6]="Scope";struct Cyc_Typerep_TUnion_struct Cyc_Absyn_scope_t_rep={
5,(struct _dyneither_ptr){_tmp1A8,_tmp1A8,_tmp1A8 + 6},{(void*)((struct _tuple7**)
Cyc__genarr_162),(void*)((struct _tuple7**)Cyc__genarr_162),(void*)((struct
_tuple7**)Cyc__genarr_162 + 6)},{(void*)((struct _tuple5**)Cyc__genarr_163),(void*)((
struct _tuple5**)Cyc__genarr_163),(void*)((struct _tuple5**)Cyc__genarr_163 + 0)}};
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_11;extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_12;extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_nmspace_t_rep;
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_18;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_19;extern struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_var_t46H2_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_13;extern struct Cyc_Typerep_FatPtr_struct
Cyc__genrep_14;static struct Cyc_Typerep_Int_struct Cyc__genrep_15={0,0,8};static
struct Cyc_Typerep_FatPtr_struct Cyc__genrep_14={2,(void*)((void*)& Cyc__genrep_15)};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_13={1,1,(void*)((void*)& Cyc__genrep_14)};
static char _tmp1AC[5]="List";static struct _dyneither_ptr Cyc__genname_23=(struct
_dyneither_ptr){_tmp1AC,_tmp1AC,_tmp1AC + 5};static char _tmp1AD[3]="hd";static
struct _tuple5 Cyc__gentuple_20={offsetof(struct Cyc_List_List,hd),(struct
_dyneither_ptr){_tmp1AD,_tmp1AD,_tmp1AD + 3},(void*)& Cyc__genrep_13};static char
_tmp1AE[3]="tl";static struct _tuple5 Cyc__gentuple_21={offsetof(struct Cyc_List_List,tl),(
struct _dyneither_ptr){_tmp1AE,_tmp1AE,_tmp1AE + 3},(void*)& Cyc__genrep_19};static
struct _tuple5*Cyc__genarr_22[2]={& Cyc__gentuple_20,& Cyc__gentuple_21};struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List0Absyn_var_t46H2_rep={3,(struct _dyneither_ptr*)& Cyc__genname_23,
sizeof(struct Cyc_List_List),{(void*)((struct _tuple5**)Cyc__genarr_22),(void*)((
struct _tuple5**)Cyc__genarr_22),(void*)((struct _tuple5**)Cyc__genarr_22 + 2)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_19={1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_var_t46H2_rep)};
struct _tuple14{unsigned int f1;struct Cyc_List_List*f2;};static struct _tuple6 Cyc__gentuple_25={
offsetof(struct _tuple14,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_26={
offsetof(struct _tuple14,f2),(void*)& Cyc__genrep_19};static struct _tuple6*Cyc__genarr_27[
2]={& Cyc__gentuple_25,& Cyc__gentuple_26};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_18={
4,sizeof(struct _tuple14),{(void*)((struct _tuple6**)Cyc__genarr_27),(void*)((
struct _tuple6**)Cyc__genarr_27),(void*)((struct _tuple6**)Cyc__genarr_27 + 2)}};
static char _tmp1B2[6]="Loc_n";static struct _tuple7 Cyc__gentuple_16={0,(struct
_dyneither_ptr){_tmp1B2,_tmp1B2,_tmp1B2 + 6}};static struct _tuple7*Cyc__genarr_17[
1]={& Cyc__gentuple_16};static char _tmp1B3[6]="Rel_n";static struct _tuple5 Cyc__gentuple_28={
0,(struct _dyneither_ptr){_tmp1B3,_tmp1B3,_tmp1B3 + 6},(void*)& Cyc__genrep_18};
static char _tmp1B4[6]="Abs_n";static struct _tuple5 Cyc__gentuple_29={1,(struct
_dyneither_ptr){_tmp1B4,_tmp1B4,_tmp1B4 + 6},(void*)& Cyc__genrep_18};static struct
_tuple5*Cyc__genarr_30[2]={& Cyc__gentuple_28,& Cyc__gentuple_29};static char
_tmp1B6[8]="Nmspace";struct Cyc_Typerep_TUnion_struct Cyc_Absyn_nmspace_t_rep={5,(
struct _dyneither_ptr){_tmp1B6,_tmp1B6,_tmp1B6 + 8},{(void*)((struct _tuple7**)Cyc__genarr_17),(
void*)((struct _tuple7**)Cyc__genarr_17),(void*)((struct _tuple7**)Cyc__genarr_17 + 
1)},{(void*)((struct _tuple5**)Cyc__genarr_30),(void*)((struct _tuple5**)Cyc__genarr_30),(
void*)((struct _tuple5**)Cyc__genarr_30 + 2)}};static struct _tuple6 Cyc__gentuple_31={
offsetof(struct _tuple0,f1),(void*)& Cyc_Absyn_nmspace_t_rep};static struct _tuple6
Cyc__gentuple_32={offsetof(struct _tuple0,f2),(void*)& Cyc__genrep_13};static
struct _tuple6*Cyc__genarr_33[2]={& Cyc__gentuple_31,& Cyc__gentuple_32};static
struct Cyc_Typerep_Tuple_struct Cyc__genrep_12={4,sizeof(struct _tuple0),{(void*)((
struct _tuple6**)Cyc__genarr_33),(void*)((struct _tuple6**)Cyc__genarr_33),(void*)((
struct _tuple6**)Cyc__genarr_33 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_11={
1,1,(void*)((void*)& Cyc__genrep_12)};extern struct Cyc_Typerep_Struct_struct Cyc_Absyn_tqual_t_rep;
static struct Cyc_Typerep_Int_struct Cyc__genrep_10={0,1,32};extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_2;extern struct Cyc_Typerep_Struct_struct Cyc_struct_Position_Segment_rep;
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_2={1,1,(void*)((void*)& Cyc_struct_Position_Segment_rep)};
static char _tmp1BB[6]="Tqual";static struct _dyneither_ptr Cyc__genname_155=(struct
_dyneither_ptr){_tmp1BB,_tmp1BB,_tmp1BB + 6};static char _tmp1BC[12]="print_const";
static struct _tuple5 Cyc__gentuple_149={offsetof(struct Cyc_Absyn_Tqual,print_const),(
struct _dyneither_ptr){_tmp1BC,_tmp1BC,_tmp1BC + 12},(void*)& Cyc__genrep_10};
static char _tmp1BD[11]="q_volatile";static struct _tuple5 Cyc__gentuple_150={
offsetof(struct Cyc_Absyn_Tqual,q_volatile),(struct _dyneither_ptr){_tmp1BD,
_tmp1BD,_tmp1BD + 11},(void*)& Cyc__genrep_10};static char _tmp1BE[11]="q_restrict";
static struct _tuple5 Cyc__gentuple_151={offsetof(struct Cyc_Absyn_Tqual,q_restrict),(
struct _dyneither_ptr){_tmp1BE,_tmp1BE,_tmp1BE + 11},(void*)& Cyc__genrep_10};
static char _tmp1BF[11]="real_const";static struct _tuple5 Cyc__gentuple_152={
offsetof(struct Cyc_Absyn_Tqual,real_const),(struct _dyneither_ptr){_tmp1BF,
_tmp1BF,_tmp1BF + 11},(void*)& Cyc__genrep_10};static char _tmp1C0[4]="loc";static
struct _tuple5 Cyc__gentuple_153={offsetof(struct Cyc_Absyn_Tqual,loc),(struct
_dyneither_ptr){_tmp1C0,_tmp1C0,_tmp1C0 + 4},(void*)& Cyc__genrep_2};static struct
_tuple5*Cyc__genarr_154[5]={& Cyc__gentuple_149,& Cyc__gentuple_150,& Cyc__gentuple_151,&
Cyc__gentuple_152,& Cyc__gentuple_153};struct Cyc_Typerep_Struct_struct Cyc_Absyn_tqual_t_rep={
3,(struct _dyneither_ptr*)& Cyc__genname_155,sizeof(struct Cyc_Absyn_Tqual),{(void*)((
struct _tuple5**)Cyc__genarr_154),(void*)((struct _tuple5**)Cyc__genarr_154),(void*)((
struct _tuple5**)Cyc__genarr_154 + 5)}};extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_type_t_rep;
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_1105;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_1110;extern struct Cyc_Typerep_Struct_struct Cyc_struct_Core_Opt0Absyn_kind_t2_rep;
extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_kind_t_rep;static char _tmp1C2[8]="AnyKind";
static struct _tuple7 Cyc__gentuple_277={0,(struct _dyneither_ptr){_tmp1C2,_tmp1C2,
_tmp1C2 + 8}};static char _tmp1C3[8]="MemKind";static struct _tuple7 Cyc__gentuple_278={
1,(struct _dyneither_ptr){_tmp1C3,_tmp1C3,_tmp1C3 + 8}};static char _tmp1C4[8]="BoxKind";
static struct _tuple7 Cyc__gentuple_279={2,(struct _dyneither_ptr){_tmp1C4,_tmp1C4,
_tmp1C4 + 8}};static char _tmp1C5[8]="RgnKind";static struct _tuple7 Cyc__gentuple_280={
3,(struct _dyneither_ptr){_tmp1C5,_tmp1C5,_tmp1C5 + 8}};static char _tmp1C6[14]="UniqueRgnKind";
static struct _tuple7 Cyc__gentuple_281={4,(struct _dyneither_ptr){_tmp1C6,_tmp1C6,
_tmp1C6 + 14}};static char _tmp1C7[11]="TopRgnKind";static struct _tuple7 Cyc__gentuple_282={
5,(struct _dyneither_ptr){_tmp1C7,_tmp1C7,_tmp1C7 + 11}};static char _tmp1C8[8]="EffKind";
static struct _tuple7 Cyc__gentuple_283={6,(struct _dyneither_ptr){_tmp1C8,_tmp1C8,
_tmp1C8 + 8}};static char _tmp1C9[8]="IntKind";static struct _tuple7 Cyc__gentuple_284={
7,(struct _dyneither_ptr){_tmp1C9,_tmp1C9,_tmp1C9 + 8}};static struct _tuple7*Cyc__genarr_285[
8]={& Cyc__gentuple_277,& Cyc__gentuple_278,& Cyc__gentuple_279,& Cyc__gentuple_280,&
Cyc__gentuple_281,& Cyc__gentuple_282,& Cyc__gentuple_283,& Cyc__gentuple_284};
static struct _tuple5*Cyc__genarr_286[0]={};static char _tmp1CB[5]="Kind";struct Cyc_Typerep_TUnion_struct
Cyc_Absyn_kind_t_rep={5,(struct _dyneither_ptr){_tmp1CB,_tmp1CB,_tmp1CB + 5},{(
void*)((struct _tuple7**)Cyc__genarr_285),(void*)((struct _tuple7**)Cyc__genarr_285),(
void*)((struct _tuple7**)Cyc__genarr_285 + 8)},{(void*)((struct _tuple5**)Cyc__genarr_286),(
void*)((struct _tuple5**)Cyc__genarr_286),(void*)((struct _tuple5**)Cyc__genarr_286
+ 0)}};static char _tmp1CC[4]="Opt";static struct _dyneither_ptr Cyc__genname_1113=(
struct _dyneither_ptr){_tmp1CC,_tmp1CC,_tmp1CC + 4};static char _tmp1CD[2]="v";
static struct _tuple5 Cyc__gentuple_1111={offsetof(struct Cyc_Core_Opt,v),(struct
_dyneither_ptr){_tmp1CD,_tmp1CD,_tmp1CD + 2},(void*)& Cyc_Absyn_kind_t_rep};static
struct _tuple5*Cyc__genarr_1112[1]={& Cyc__gentuple_1111};struct Cyc_Typerep_Struct_struct
Cyc_struct_Core_Opt0Absyn_kind_t2_rep={3,(struct _dyneither_ptr*)& Cyc__genname_1113,
sizeof(struct Cyc_Core_Opt),{(void*)((struct _tuple5**)Cyc__genarr_1112),(void*)((
struct _tuple5**)Cyc__genarr_1112),(void*)((struct _tuple5**)Cyc__genarr_1112 + 1)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_1110={1,1,(void*)((void*)& Cyc_struct_Core_Opt0Absyn_kind_t2_rep)};
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_110;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Core_Opt0Absyn_type_t2_rep;static char _tmp1D0[4]="Opt";static struct
_dyneither_ptr Cyc__genname_1145=(struct _dyneither_ptr){_tmp1D0,_tmp1D0,_tmp1D0 + 
4};static char _tmp1D1[2]="v";static struct _tuple5 Cyc__gentuple_1143={offsetof(
struct Cyc_Core_Opt,v),(struct _dyneither_ptr){_tmp1D1,_tmp1D1,_tmp1D1 + 2},(void*)&
Cyc_Absyn_type_t_rep};static struct _tuple5*Cyc__genarr_1144[1]={& Cyc__gentuple_1143};
struct Cyc_Typerep_Struct_struct Cyc_struct_Core_Opt0Absyn_type_t2_rep={3,(struct
_dyneither_ptr*)& Cyc__genname_1145,sizeof(struct Cyc_Core_Opt),{(void*)((struct
_tuple5**)Cyc__genarr_1144),(void*)((struct _tuple5**)Cyc__genarr_1144),(void*)((
struct _tuple5**)Cyc__genarr_1144 + 1)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_110={
1,1,(void*)((void*)& Cyc_struct_Core_Opt0Absyn_type_t2_rep)};extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_1106;extern struct Cyc_Typerep_Struct_struct Cyc_struct_Core_Opt0List_list_t0Absyn_tvar_t46H22_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_273;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List0Absyn_tvar_t46H2_rep;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_274;extern struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Tvar_rep;
extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_kindbound_t_rep;extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_299;static struct _tuple6 Cyc__gentuple_300={offsetof(struct _tuple6,f1),(
void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_301={offsetof(struct
_tuple6,f2),(void*)& Cyc_Absyn_kind_t_rep};static struct _tuple6*Cyc__genarr_302[2]={&
Cyc__gentuple_300,& Cyc__gentuple_301};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_299={
4,sizeof(struct _tuple6),{(void*)((struct _tuple6**)Cyc__genarr_302),(void*)((
struct _tuple6**)Cyc__genarr_302),(void*)((struct _tuple6**)Cyc__genarr_302 + 2)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_295;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_287;extern struct Cyc_Typerep_Struct_struct Cyc_struct_Core_Opt0Absyn_kindbound_t2_rep;
static char _tmp1D5[4]="Opt";static struct _dyneither_ptr Cyc__genname_290=(struct
_dyneither_ptr){_tmp1D5,_tmp1D5,_tmp1D5 + 4};static char _tmp1D6[2]="v";static
struct _tuple5 Cyc__gentuple_288={offsetof(struct Cyc_Core_Opt,v),(struct
_dyneither_ptr){_tmp1D6,_tmp1D6,_tmp1D6 + 2},(void*)& Cyc_Absyn_kindbound_t_rep};
static struct _tuple5*Cyc__genarr_289[1]={& Cyc__gentuple_288};struct Cyc_Typerep_Struct_struct
Cyc_struct_Core_Opt0Absyn_kindbound_t2_rep={3,(struct _dyneither_ptr*)& Cyc__genname_290,
sizeof(struct Cyc_Core_Opt),{(void*)((struct _tuple5**)Cyc__genarr_289),(void*)((
struct _tuple5**)Cyc__genarr_289),(void*)((struct _tuple5**)Cyc__genarr_289 + 1)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_287={1,1,(void*)((void*)& Cyc_struct_Core_Opt0Absyn_kindbound_t2_rep)};
struct _tuple15{unsigned int f1;struct Cyc_Core_Opt*f2;};static struct _tuple6 Cyc__gentuple_296={
offsetof(struct _tuple15,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_297={
offsetof(struct _tuple15,f2),(void*)& Cyc__genrep_287};static struct _tuple6*Cyc__genarr_298[
2]={& Cyc__gentuple_296,& Cyc__gentuple_297};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_295={4,sizeof(struct _tuple15),{(void*)((struct _tuple6**)Cyc__genarr_298),(
void*)((struct _tuple6**)Cyc__genarr_298),(void*)((struct _tuple6**)Cyc__genarr_298
+ 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_276;struct _tuple16{
unsigned int f1;struct Cyc_Core_Opt*f2;void*f3;};static struct _tuple6 Cyc__gentuple_291={
offsetof(struct _tuple16,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_292={
offsetof(struct _tuple16,f2),(void*)& Cyc__genrep_287};static struct _tuple6 Cyc__gentuple_293={
offsetof(struct _tuple16,f3),(void*)& Cyc_Absyn_kind_t_rep};static struct _tuple6*
Cyc__genarr_294[3]={& Cyc__gentuple_291,& Cyc__gentuple_292,& Cyc__gentuple_293};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_276={4,sizeof(struct _tuple16),{(
void*)((struct _tuple6**)Cyc__genarr_294),(void*)((struct _tuple6**)Cyc__genarr_294),(
void*)((struct _tuple6**)Cyc__genarr_294 + 3)}};static struct _tuple7*Cyc__genarr_275[
0]={};static char _tmp1DB[6]="Eq_kb";static struct _tuple5 Cyc__gentuple_303={0,(
struct _dyneither_ptr){_tmp1DB,_tmp1DB,_tmp1DB + 6},(void*)& Cyc__genrep_299};
static char _tmp1DC[11]="Unknown_kb";static struct _tuple5 Cyc__gentuple_304={1,(
struct _dyneither_ptr){_tmp1DC,_tmp1DC,_tmp1DC + 11},(void*)& Cyc__genrep_295};
static char _tmp1DD[8]="Less_kb";static struct _tuple5 Cyc__gentuple_305={2,(struct
_dyneither_ptr){_tmp1DD,_tmp1DD,_tmp1DD + 8},(void*)& Cyc__genrep_276};static
struct _tuple5*Cyc__genarr_306[3]={& Cyc__gentuple_303,& Cyc__gentuple_304,& Cyc__gentuple_305};
static char _tmp1DF[10]="KindBound";struct Cyc_Typerep_TUnion_struct Cyc_Absyn_kindbound_t_rep={
5,(struct _dyneither_ptr){_tmp1DF,_tmp1DF,_tmp1DF + 10},{(void*)((struct _tuple7**)
Cyc__genarr_275),(void*)((struct _tuple7**)Cyc__genarr_275),(void*)((struct
_tuple7**)Cyc__genarr_275 + 0)},{(void*)((struct _tuple5**)Cyc__genarr_306),(void*)((
struct _tuple5**)Cyc__genarr_306),(void*)((struct _tuple5**)Cyc__genarr_306 + 3)}};
static char _tmp1E0[5]="Tvar";static struct _dyneither_ptr Cyc__genname_311=(struct
_dyneither_ptr){_tmp1E0,_tmp1E0,_tmp1E0 + 5};static char _tmp1E1[5]="name";static
struct _tuple5 Cyc__gentuple_307={offsetof(struct Cyc_Absyn_Tvar,name),(struct
_dyneither_ptr){_tmp1E1,_tmp1E1,_tmp1E1 + 5},(void*)& Cyc__genrep_13};static char
_tmp1E2[9]="identity";static struct _tuple5 Cyc__gentuple_308={offsetof(struct Cyc_Absyn_Tvar,identity),(
struct _dyneither_ptr){_tmp1E2,_tmp1E2,_tmp1E2 + 9},(void*)& Cyc__genrep_10};static
char _tmp1E3[5]="kind";static struct _tuple5 Cyc__gentuple_309={offsetof(struct Cyc_Absyn_Tvar,kind),(
struct _dyneither_ptr){_tmp1E3,_tmp1E3,_tmp1E3 + 5},(void*)& Cyc_Absyn_kindbound_t_rep};
static struct _tuple5*Cyc__genarr_310[3]={& Cyc__gentuple_307,& Cyc__gentuple_308,&
Cyc__gentuple_309};struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Tvar_rep={3,(
struct _dyneither_ptr*)& Cyc__genname_311,sizeof(struct Cyc_Absyn_Tvar),{(void*)((
struct _tuple5**)Cyc__genarr_310),(void*)((struct _tuple5**)Cyc__genarr_310),(void*)((
struct _tuple5**)Cyc__genarr_310 + 3)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_274={
1,1,(void*)((void*)& Cyc_struct_Absyn_Tvar_rep)};static char _tmp1E6[5]="List";
static struct _dyneither_ptr Cyc__genname_315=(struct _dyneither_ptr){_tmp1E6,
_tmp1E6,_tmp1E6 + 5};static char _tmp1E7[3]="hd";static struct _tuple5 Cyc__gentuple_312={
offsetof(struct Cyc_List_List,hd),(struct _dyneither_ptr){_tmp1E7,_tmp1E7,_tmp1E7 + 
3},(void*)& Cyc__genrep_274};static char _tmp1E8[3]="tl";static struct _tuple5 Cyc__gentuple_313={
offsetof(struct Cyc_List_List,tl),(struct _dyneither_ptr){_tmp1E8,_tmp1E8,_tmp1E8 + 
3},(void*)& Cyc__genrep_273};static struct _tuple5*Cyc__genarr_314[2]={& Cyc__gentuple_312,&
Cyc__gentuple_313};struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_tvar_t46H2_rep={
3,(struct _dyneither_ptr*)& Cyc__genname_315,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_314),(void*)((struct _tuple5**)Cyc__genarr_314),(void*)((
struct _tuple5**)Cyc__genarr_314 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_273={
1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_tvar_t46H2_rep)};static char
_tmp1EB[4]="Opt";static struct _dyneither_ptr Cyc__genname_1109=(struct
_dyneither_ptr){_tmp1EB,_tmp1EB,_tmp1EB + 4};static char _tmp1EC[2]="v";static
struct _tuple5 Cyc__gentuple_1107={offsetof(struct Cyc_Core_Opt,v),(struct
_dyneither_ptr){_tmp1EC,_tmp1EC,_tmp1EC + 2},(void*)& Cyc__genrep_273};static
struct _tuple5*Cyc__genarr_1108[1]={& Cyc__gentuple_1107};struct Cyc_Typerep_Struct_struct
Cyc_struct_Core_Opt0List_list_t0Absyn_tvar_t46H22_rep={3,(struct _dyneither_ptr*)&
Cyc__genname_1109,sizeof(struct Cyc_Core_Opt),{(void*)((struct _tuple5**)Cyc__genarr_1108),(
void*)((struct _tuple5**)Cyc__genarr_1108),(void*)((struct _tuple5**)Cyc__genarr_1108
+ 1)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_1106={1,1,(void*)((void*)&
Cyc_struct_Core_Opt0List_list_t0Absyn_tvar_t46H22_rep)};struct _tuple17{
unsigned int f1;struct Cyc_Core_Opt*f2;struct Cyc_Core_Opt*f3;int f4;struct Cyc_Core_Opt*
f5;};static struct _tuple6 Cyc__gentuple_1114={offsetof(struct _tuple17,f1),(void*)&
Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_1115={offsetof(struct _tuple17,f2),(
void*)& Cyc__genrep_1110};static struct _tuple6 Cyc__gentuple_1116={offsetof(struct
_tuple17,f3),(void*)& Cyc__genrep_110};static struct _tuple6 Cyc__gentuple_1117={
offsetof(struct _tuple17,f4),(void*)& Cyc__genrep_10};static struct _tuple6 Cyc__gentuple_1118={
offsetof(struct _tuple17,f5),(void*)& Cyc__genrep_1106};static struct _tuple6*Cyc__genarr_1119[
5]={& Cyc__gentuple_1114,& Cyc__gentuple_1115,& Cyc__gentuple_1116,& Cyc__gentuple_1117,&
Cyc__gentuple_1118};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_1105={4,
sizeof(struct _tuple17),{(void*)((struct _tuple6**)Cyc__genarr_1119),(void*)((
struct _tuple6**)Cyc__genarr_1119),(void*)((struct _tuple6**)Cyc__genarr_1119 + 5)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_1101;struct _tuple18{unsigned int
f1;struct Cyc_Absyn_Tvar*f2;};static struct _tuple6 Cyc__gentuple_1102={offsetof(
struct _tuple18,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_1103={
offsetof(struct _tuple18,f2),(void*)& Cyc__genrep_274};static struct _tuple6*Cyc__genarr_1104[
2]={& Cyc__gentuple_1102,& Cyc__gentuple_1103};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_1101={4,sizeof(struct _tuple18),{(void*)((struct _tuple6**)Cyc__genarr_1104),(
void*)((struct _tuple6**)Cyc__genarr_1104),(void*)((struct _tuple6**)Cyc__genarr_1104
+ 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_1074;extern struct Cyc_Typerep_Struct_struct
Cyc_Absyn_datatype_info_t_rep;extern struct Cyc_Typerep_TUnion_struct Cyc_datatype_Absyn_DatatypeInfoU_rep;
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_1081;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_UnknownDatatypeInfo_rep;static char _tmp1F1[20]="UnknownDatatypeInfo";
static struct _dyneither_ptr Cyc__genname_1086=(struct _dyneither_ptr){_tmp1F1,
_tmp1F1,_tmp1F1 + 20};static char _tmp1F2[5]="name";static struct _tuple5 Cyc__gentuple_1082={
offsetof(struct Cyc_Absyn_UnknownDatatypeInfo,name),(struct _dyneither_ptr){
_tmp1F2,_tmp1F2,_tmp1F2 + 5},(void*)& Cyc__genrep_11};static char _tmp1F3[14]="is_extensible";
static struct _tuple5 Cyc__gentuple_1083={offsetof(struct Cyc_Absyn_UnknownDatatypeInfo,is_extensible),(
struct _dyneither_ptr){_tmp1F3,_tmp1F3,_tmp1F3 + 14},(void*)& Cyc__genrep_10};
static char _tmp1F4[8]="is_flat";static struct _tuple5 Cyc__gentuple_1084={offsetof(
struct Cyc_Absyn_UnknownDatatypeInfo,is_flat),(struct _dyneither_ptr){_tmp1F4,
_tmp1F4,_tmp1F4 + 8},(void*)& Cyc__genrep_10};static struct _tuple5*Cyc__genarr_1085[
3]={& Cyc__gentuple_1082,& Cyc__gentuple_1083,& Cyc__gentuple_1084};struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_UnknownDatatypeInfo_rep={3,(struct _dyneither_ptr*)& Cyc__genname_1086,
sizeof(struct Cyc_Absyn_UnknownDatatypeInfo),{(void*)((struct _tuple5**)Cyc__genarr_1085),(
void*)((struct _tuple5**)Cyc__genarr_1085),(void*)((struct _tuple5**)Cyc__genarr_1085
+ 3)}};struct _tuple19{unsigned int f1;struct Cyc_Absyn_UnknownDatatypeInfo f2;};
static struct _tuple6 Cyc__gentuple_1087={offsetof(struct _tuple19,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_1088={offsetof(struct _tuple19,f2),(void*)& Cyc_struct_Absyn_UnknownDatatypeInfo_rep};
static struct _tuple6*Cyc__genarr_1089[2]={& Cyc__gentuple_1087,& Cyc__gentuple_1088};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_1081={4,sizeof(struct _tuple19),{(
void*)((struct _tuple6**)Cyc__genarr_1089),(void*)((struct _tuple6**)Cyc__genarr_1089),(
void*)((struct _tuple6**)Cyc__genarr_1089 + 2)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_1076;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_1077;extern
struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_263;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_Datatypedecl_rep;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_264;
extern struct Cyc_Typerep_Struct_struct Cyc_struct_Core_Opt0List_list_t0Absyn_datatypefield_t46H22_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_265;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List0Absyn_datatypefield_t46H2_rep;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_246;extern struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Datatypefield_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_247;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List060Absyn_tqual_t4Absyn_type_t1_446H2_rep;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_248;extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_249;static struct
_tuple6 Cyc__gentuple_250={offsetof(struct _tuple3,f1),(void*)& Cyc_Absyn_tqual_t_rep};
static struct _tuple6 Cyc__gentuple_251={offsetof(struct _tuple3,f2),(void*)((void*)&
Cyc_Absyn_type_t_rep)};static struct _tuple6*Cyc__genarr_252[2]={& Cyc__gentuple_250,&
Cyc__gentuple_251};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_249={4,
sizeof(struct _tuple3),{(void*)((struct _tuple6**)Cyc__genarr_252),(void*)((struct
_tuple6**)Cyc__genarr_252),(void*)((struct _tuple6**)Cyc__genarr_252 + 2)}};static
struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_248={1,1,(void*)((void*)& Cyc__genrep_249)};
static char _tmp1F9[5]="List";static struct _dyneither_ptr Cyc__genname_256=(struct
_dyneither_ptr){_tmp1F9,_tmp1F9,_tmp1F9 + 5};static char _tmp1FA[3]="hd";static
struct _tuple5 Cyc__gentuple_253={offsetof(struct Cyc_List_List,hd),(struct
_dyneither_ptr){_tmp1FA,_tmp1FA,_tmp1FA + 3},(void*)& Cyc__genrep_248};static char
_tmp1FB[3]="tl";static struct _tuple5 Cyc__gentuple_254={offsetof(struct Cyc_List_List,tl),(
struct _dyneither_ptr){_tmp1FB,_tmp1FB,_tmp1FB + 3},(void*)& Cyc__genrep_247};
static struct _tuple5*Cyc__genarr_255[2]={& Cyc__gentuple_253,& Cyc__gentuple_254};
struct Cyc_Typerep_Struct_struct Cyc_struct_List_List060Absyn_tqual_t4Absyn_type_t1_446H2_rep={
3,(struct _dyneither_ptr*)& Cyc__genname_256,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_255),(void*)((struct _tuple5**)Cyc__genarr_255),(void*)((
struct _tuple5**)Cyc__genarr_255 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_247={
1,1,(void*)((void*)& Cyc_struct_List_List060Absyn_tqual_t4Absyn_type_t1_446H2_rep)};
static char _tmp1FE[14]="Datatypefield";static struct _dyneither_ptr Cyc__genname_262=(
struct _dyneither_ptr){_tmp1FE,_tmp1FE,_tmp1FE + 14};static char _tmp1FF[5]="name";
static struct _tuple5 Cyc__gentuple_257={offsetof(struct Cyc_Absyn_Datatypefield,name),(
struct _dyneither_ptr){_tmp1FF,_tmp1FF,_tmp1FF + 5},(void*)& Cyc__genrep_11};static
char _tmp200[5]="typs";static struct _tuple5 Cyc__gentuple_258={offsetof(struct Cyc_Absyn_Datatypefield,typs),(
struct _dyneither_ptr){_tmp200,_tmp200,_tmp200 + 5},(void*)& Cyc__genrep_247};
static char _tmp201[4]="loc";static struct _tuple5 Cyc__gentuple_259={offsetof(struct
Cyc_Absyn_Datatypefield,loc),(struct _dyneither_ptr){_tmp201,_tmp201,_tmp201 + 4},(
void*)& Cyc__genrep_2};static char _tmp202[3]="sc";static struct _tuple5 Cyc__gentuple_260={
offsetof(struct Cyc_Absyn_Datatypefield,sc),(struct _dyneither_ptr){_tmp202,
_tmp202,_tmp202 + 3},(void*)& Cyc_Absyn_scope_t_rep};static struct _tuple5*Cyc__genarr_261[
4]={& Cyc__gentuple_257,& Cyc__gentuple_258,& Cyc__gentuple_259,& Cyc__gentuple_260};
struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Datatypefield_rep={3,(struct
_dyneither_ptr*)& Cyc__genname_262,sizeof(struct Cyc_Absyn_Datatypefield),{(void*)((
struct _tuple5**)Cyc__genarr_261),(void*)((struct _tuple5**)Cyc__genarr_261),(void*)((
struct _tuple5**)Cyc__genarr_261 + 4)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_246={
1,1,(void*)((void*)& Cyc_struct_Absyn_Datatypefield_rep)};static char _tmp205[5]="List";
static struct _dyneither_ptr Cyc__genname_269=(struct _dyneither_ptr){_tmp205,
_tmp205,_tmp205 + 5};static char _tmp206[3]="hd";static struct _tuple5 Cyc__gentuple_266={
offsetof(struct Cyc_List_List,hd),(struct _dyneither_ptr){_tmp206,_tmp206,_tmp206 + 
3},(void*)& Cyc__genrep_246};static char _tmp207[3]="tl";static struct _tuple5 Cyc__gentuple_267={
offsetof(struct Cyc_List_List,tl),(struct _dyneither_ptr){_tmp207,_tmp207,_tmp207 + 
3},(void*)& Cyc__genrep_265};static struct _tuple5*Cyc__genarr_268[2]={& Cyc__gentuple_266,&
Cyc__gentuple_267};struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_datatypefield_t46H2_rep={
3,(struct _dyneither_ptr*)& Cyc__genname_269,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_268),(void*)((struct _tuple5**)Cyc__genarr_268),(void*)((
struct _tuple5**)Cyc__genarr_268 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_265={
1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_datatypefield_t46H2_rep)};static
char _tmp20A[4]="Opt";static struct _dyneither_ptr Cyc__genname_272=(struct
_dyneither_ptr){_tmp20A,_tmp20A,_tmp20A + 4};static char _tmp20B[2]="v";static
struct _tuple5 Cyc__gentuple_270={offsetof(struct Cyc_Core_Opt,v),(struct
_dyneither_ptr){_tmp20B,_tmp20B,_tmp20B + 2},(void*)& Cyc__genrep_265};static
struct _tuple5*Cyc__genarr_271[1]={& Cyc__gentuple_270};struct Cyc_Typerep_Struct_struct
Cyc_struct_Core_Opt0List_list_t0Absyn_datatypefield_t46H22_rep={3,(struct
_dyneither_ptr*)& Cyc__genname_272,sizeof(struct Cyc_Core_Opt),{(void*)((struct
_tuple5**)Cyc__genarr_271),(void*)((struct _tuple5**)Cyc__genarr_271),(void*)((
struct _tuple5**)Cyc__genarr_271 + 1)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_264={
1,1,(void*)((void*)& Cyc_struct_Core_Opt0List_list_t0Absyn_datatypefield_t46H22_rep)};
static char _tmp20E[13]="Datatypedecl";static struct _dyneither_ptr Cyc__genname_323=(
struct _dyneither_ptr){_tmp20E,_tmp20E,_tmp20E + 13};static char _tmp20F[3]="sc";
static struct _tuple5 Cyc__gentuple_316={offsetof(struct Cyc_Absyn_Datatypedecl,sc),(
struct _dyneither_ptr){_tmp20F,_tmp20F,_tmp20F + 3},(void*)& Cyc_Absyn_scope_t_rep};
static char _tmp210[5]="name";static struct _tuple5 Cyc__gentuple_317={offsetof(
struct Cyc_Absyn_Datatypedecl,name),(struct _dyneither_ptr){_tmp210,_tmp210,
_tmp210 + 5},(void*)& Cyc__genrep_11};static char _tmp211[4]="tvs";static struct
_tuple5 Cyc__gentuple_318={offsetof(struct Cyc_Absyn_Datatypedecl,tvs),(struct
_dyneither_ptr){_tmp211,_tmp211,_tmp211 + 4},(void*)& Cyc__genrep_273};static char
_tmp212[7]="fields";static struct _tuple5 Cyc__gentuple_319={offsetof(struct Cyc_Absyn_Datatypedecl,fields),(
struct _dyneither_ptr){_tmp212,_tmp212,_tmp212 + 7},(void*)& Cyc__genrep_264};
static char _tmp213[14]="is_extensible";static struct _tuple5 Cyc__gentuple_320={
offsetof(struct Cyc_Absyn_Datatypedecl,is_extensible),(struct _dyneither_ptr){
_tmp213,_tmp213,_tmp213 + 14},(void*)& Cyc__genrep_10};static char _tmp214[8]="is_flat";
static struct _tuple5 Cyc__gentuple_321={offsetof(struct Cyc_Absyn_Datatypedecl,is_flat),(
struct _dyneither_ptr){_tmp214,_tmp214,_tmp214 + 8},(void*)& Cyc__genrep_10};static
struct _tuple5*Cyc__genarr_322[6]={& Cyc__gentuple_316,& Cyc__gentuple_317,& Cyc__gentuple_318,&
Cyc__gentuple_319,& Cyc__gentuple_320,& Cyc__gentuple_321};struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_Datatypedecl_rep={3,(struct _dyneither_ptr*)& Cyc__genname_323,
sizeof(struct Cyc_Absyn_Datatypedecl),{(void*)((struct _tuple5**)Cyc__genarr_322),(
void*)((struct _tuple5**)Cyc__genarr_322),(void*)((struct _tuple5**)Cyc__genarr_322
+ 6)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_263={1,1,(void*)((void*)&
Cyc_struct_Absyn_Datatypedecl_rep)};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_1077={
1,1,(void*)((void*)& Cyc__genrep_263)};struct _tuple20{unsigned int f1;struct Cyc_Absyn_Datatypedecl**
f2;};static struct _tuple6 Cyc__gentuple_1078={offsetof(struct _tuple20,f1),(void*)&
Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_1079={offsetof(struct _tuple20,f2),(
void*)& Cyc__genrep_1077};static struct _tuple6*Cyc__genarr_1080[2]={& Cyc__gentuple_1078,&
Cyc__gentuple_1079};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_1076={4,
sizeof(struct _tuple20),{(void*)((struct _tuple6**)Cyc__genarr_1080),(void*)((
struct _tuple6**)Cyc__genarr_1080),(void*)((struct _tuple6**)Cyc__genarr_1080 + 2)}};
static struct _tuple7*Cyc__genarr_1075[0]={};static char _tmp219[16]="UnknownDatatype";
static struct _tuple5 Cyc__gentuple_1090={0,(struct _dyneither_ptr){_tmp219,_tmp219,
_tmp219 + 16},(void*)& Cyc__genrep_1081};static char _tmp21A[14]="KnownDatatype";
static struct _tuple5 Cyc__gentuple_1091={1,(struct _dyneither_ptr){_tmp21A,_tmp21A,
_tmp21A + 14},(void*)& Cyc__genrep_1076};static struct _tuple5*Cyc__genarr_1092[2]={&
Cyc__gentuple_1090,& Cyc__gentuple_1091};static char _tmp21C[14]="DatatypeInfoU";
struct Cyc_Typerep_TUnion_struct Cyc_datatype_Absyn_DatatypeInfoU_rep={5,(struct
_dyneither_ptr){_tmp21C,_tmp21C,_tmp21C + 14},{(void*)((struct _tuple7**)Cyc__genarr_1075),(
void*)((struct _tuple7**)Cyc__genarr_1075),(void*)((struct _tuple7**)Cyc__genarr_1075
+ 0)},{(void*)((struct _tuple5**)Cyc__genarr_1092),(void*)((struct _tuple5**)Cyc__genarr_1092),(
void*)((struct _tuple5**)Cyc__genarr_1092 + 2)}};extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_121;extern struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_type_t46H2_rep;
static char _tmp21D[5]="List";static struct _dyneither_ptr Cyc__genname_125=(struct
_dyneither_ptr){_tmp21D,_tmp21D,_tmp21D + 5};static char _tmp21E[3]="hd";static
struct _tuple5 Cyc__gentuple_122={offsetof(struct Cyc_List_List,hd),(struct
_dyneither_ptr){_tmp21E,_tmp21E,_tmp21E + 3},(void*)((void*)& Cyc_Absyn_type_t_rep)};
static char _tmp21F[3]="tl";static struct _tuple5 Cyc__gentuple_123={offsetof(struct
Cyc_List_List,tl),(struct _dyneither_ptr){_tmp21F,_tmp21F,_tmp21F + 3},(void*)& Cyc__genrep_121};
static struct _tuple5*Cyc__genarr_124[2]={& Cyc__gentuple_122,& Cyc__gentuple_123};
struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_type_t46H2_rep={3,(
struct _dyneither_ptr*)& Cyc__genname_125,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_124),(void*)((struct _tuple5**)Cyc__genarr_124),(void*)((
struct _tuple5**)Cyc__genarr_124 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_121={
1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_type_t46H2_rep)};static char
_tmp222[13]="DatatypeInfo";static struct _dyneither_ptr Cyc__genname_1097=(struct
_dyneither_ptr){_tmp222,_tmp222,_tmp222 + 13};static char _tmp223[14]="datatype_info";
static struct _tuple5 Cyc__gentuple_1093={offsetof(struct Cyc_Absyn_DatatypeInfo,datatype_info),(
struct _dyneither_ptr){_tmp223,_tmp223,_tmp223 + 14},(void*)& Cyc_datatype_Absyn_DatatypeInfoU_rep};
static char _tmp224[6]="targs";static struct _tuple5 Cyc__gentuple_1094={offsetof(
struct Cyc_Absyn_DatatypeInfo,targs),(struct _dyneither_ptr){_tmp224,_tmp224,
_tmp224 + 6},(void*)& Cyc__genrep_121};static char _tmp225[4]="rgn";static struct
_tuple5 Cyc__gentuple_1095={offsetof(struct Cyc_Absyn_DatatypeInfo,rgn),(struct
_dyneither_ptr){_tmp225,_tmp225,_tmp225 + 4},(void*)& Cyc__genrep_110};static
struct _tuple5*Cyc__genarr_1096[3]={& Cyc__gentuple_1093,& Cyc__gentuple_1094,& Cyc__gentuple_1095};
struct Cyc_Typerep_Struct_struct Cyc_Absyn_datatype_info_t_rep={3,(struct
_dyneither_ptr*)& Cyc__genname_1097,sizeof(struct Cyc_Absyn_DatatypeInfo),{(void*)((
struct _tuple5**)Cyc__genarr_1096),(void*)((struct _tuple5**)Cyc__genarr_1096),(
void*)((struct _tuple5**)Cyc__genarr_1096 + 3)}};struct _tuple21{unsigned int f1;
struct Cyc_Absyn_DatatypeInfo f2;};static struct _tuple6 Cyc__gentuple_1098={
offsetof(struct _tuple21,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_1099={
offsetof(struct _tuple21,f2),(void*)& Cyc_Absyn_datatype_info_t_rep};static struct
_tuple6*Cyc__genarr_1100[2]={& Cyc__gentuple_1098,& Cyc__gentuple_1099};static
struct Cyc_Typerep_Tuple_struct Cyc__genrep_1074={4,sizeof(struct _tuple21),{(void*)((
struct _tuple6**)Cyc__genarr_1100),(void*)((struct _tuple6**)Cyc__genarr_1100),(
void*)((struct _tuple6**)Cyc__genarr_1100 + 2)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_1048;extern struct Cyc_Typerep_Struct_struct Cyc_Absyn_datatype_field_info_t_rep;
extern struct Cyc_Typerep_TUnion_struct Cyc_datatype_Absyn_DatatypeFieldInfoU_rep;
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_1055;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_UnknownDatatypeFieldInfo_rep;static char _tmp228[25]="UnknownDatatypeFieldInfo";
static struct _dyneither_ptr Cyc__genname_1060=(struct _dyneither_ptr){_tmp228,
_tmp228,_tmp228 + 25};static char _tmp229[14]="datatype_name";static struct _tuple5
Cyc__gentuple_1056={offsetof(struct Cyc_Absyn_UnknownDatatypeFieldInfo,datatype_name),(
struct _dyneither_ptr){_tmp229,_tmp229,_tmp229 + 14},(void*)& Cyc__genrep_11};
static char _tmp22A[11]="field_name";static struct _tuple5 Cyc__gentuple_1057={
offsetof(struct Cyc_Absyn_UnknownDatatypeFieldInfo,field_name),(struct
_dyneither_ptr){_tmp22A,_tmp22A,_tmp22A + 11},(void*)& Cyc__genrep_11};static char
_tmp22B[14]="is_extensible";static struct _tuple5 Cyc__gentuple_1058={offsetof(
struct Cyc_Absyn_UnknownDatatypeFieldInfo,is_extensible),(struct _dyneither_ptr){
_tmp22B,_tmp22B,_tmp22B + 14},(void*)& Cyc__genrep_10};static struct _tuple5*Cyc__genarr_1059[
3]={& Cyc__gentuple_1056,& Cyc__gentuple_1057,& Cyc__gentuple_1058};struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_UnknownDatatypeFieldInfo_rep={3,(struct _dyneither_ptr*)& Cyc__genname_1060,
sizeof(struct Cyc_Absyn_UnknownDatatypeFieldInfo),{(void*)((struct _tuple5**)Cyc__genarr_1059),(
void*)((struct _tuple5**)Cyc__genarr_1059),(void*)((struct _tuple5**)Cyc__genarr_1059
+ 3)}};struct _tuple22{unsigned int f1;struct Cyc_Absyn_UnknownDatatypeFieldInfo f2;
};static struct _tuple6 Cyc__gentuple_1061={offsetof(struct _tuple22,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_1062={offsetof(struct _tuple22,f2),(void*)& Cyc_struct_Absyn_UnknownDatatypeFieldInfo_rep};
static struct _tuple6*Cyc__genarr_1063[2]={& Cyc__gentuple_1061,& Cyc__gentuple_1062};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_1055={4,sizeof(struct _tuple22),{(
void*)((struct _tuple6**)Cyc__genarr_1063),(void*)((struct _tuple6**)Cyc__genarr_1063),(
void*)((struct _tuple6**)Cyc__genarr_1063 + 2)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_1050;struct _tuple23{unsigned int f1;struct Cyc_Absyn_Datatypedecl*f2;
struct Cyc_Absyn_Datatypefield*f3;};static struct _tuple6 Cyc__gentuple_1051={
offsetof(struct _tuple23,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_1052={
offsetof(struct _tuple23,f2),(void*)((void*)& Cyc__genrep_263)};static struct
_tuple6 Cyc__gentuple_1053={offsetof(struct _tuple23,f3),(void*)& Cyc__genrep_246};
static struct _tuple6*Cyc__genarr_1054[3]={& Cyc__gentuple_1051,& Cyc__gentuple_1052,&
Cyc__gentuple_1053};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_1050={4,
sizeof(struct _tuple23),{(void*)((struct _tuple6**)Cyc__genarr_1054),(void*)((
struct _tuple6**)Cyc__genarr_1054),(void*)((struct _tuple6**)Cyc__genarr_1054 + 3)}};
static struct _tuple7*Cyc__genarr_1049[0]={};static char _tmp22F[21]="UnknownDatatypefield";
static struct _tuple5 Cyc__gentuple_1064={0,(struct _dyneither_ptr){_tmp22F,_tmp22F,
_tmp22F + 21},(void*)& Cyc__genrep_1055};static char _tmp230[19]="KnownDatatypefield";
static struct _tuple5 Cyc__gentuple_1065={1,(struct _dyneither_ptr){_tmp230,_tmp230,
_tmp230 + 19},(void*)& Cyc__genrep_1050};static struct _tuple5*Cyc__genarr_1066[2]={&
Cyc__gentuple_1064,& Cyc__gentuple_1065};static char _tmp232[19]="DatatypeFieldInfoU";
struct Cyc_Typerep_TUnion_struct Cyc_datatype_Absyn_DatatypeFieldInfoU_rep={5,(
struct _dyneither_ptr){_tmp232,_tmp232,_tmp232 + 19},{(void*)((struct _tuple7**)Cyc__genarr_1049),(
void*)((struct _tuple7**)Cyc__genarr_1049),(void*)((struct _tuple7**)Cyc__genarr_1049
+ 0)},{(void*)((struct _tuple5**)Cyc__genarr_1066),(void*)((struct _tuple5**)Cyc__genarr_1066),(
void*)((struct _tuple5**)Cyc__genarr_1066 + 2)}};static char _tmp233[18]="DatatypeFieldInfo";
static struct _dyneither_ptr Cyc__genname_1070=(struct _dyneither_ptr){_tmp233,
_tmp233,_tmp233 + 18};static char _tmp234[11]="field_info";static struct _tuple5 Cyc__gentuple_1067={
offsetof(struct Cyc_Absyn_DatatypeFieldInfo,field_info),(struct _dyneither_ptr){
_tmp234,_tmp234,_tmp234 + 11},(void*)& Cyc_datatype_Absyn_DatatypeFieldInfoU_rep};
static char _tmp235[6]="targs";static struct _tuple5 Cyc__gentuple_1068={offsetof(
struct Cyc_Absyn_DatatypeFieldInfo,targs),(struct _dyneither_ptr){_tmp235,_tmp235,
_tmp235 + 6},(void*)& Cyc__genrep_121};static struct _tuple5*Cyc__genarr_1069[2]={&
Cyc__gentuple_1067,& Cyc__gentuple_1068};struct Cyc_Typerep_Struct_struct Cyc_Absyn_datatype_field_info_t_rep={
3,(struct _dyneither_ptr*)& Cyc__genname_1070,sizeof(struct Cyc_Absyn_DatatypeFieldInfo),{(
void*)((struct _tuple5**)Cyc__genarr_1069),(void*)((struct _tuple5**)Cyc__genarr_1069),(
void*)((struct _tuple5**)Cyc__genarr_1069 + 2)}};struct _tuple24{unsigned int f1;
struct Cyc_Absyn_DatatypeFieldInfo f2;};static struct _tuple6 Cyc__gentuple_1071={
offsetof(struct _tuple24,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_1072={
offsetof(struct _tuple24,f2),(void*)& Cyc_Absyn_datatype_field_info_t_rep};static
struct _tuple6*Cyc__genarr_1073[2]={& Cyc__gentuple_1071,& Cyc__gentuple_1072};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_1048={4,sizeof(struct _tuple24),{(
void*)((struct _tuple6**)Cyc__genarr_1073),(void*)((struct _tuple6**)Cyc__genarr_1073),(
void*)((struct _tuple6**)Cyc__genarr_1073 + 2)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_1022;extern struct Cyc_Typerep_Struct_struct Cyc_Absyn_ptr_info_t_rep;
extern struct Cyc_Typerep_Struct_struct Cyc_Absyn_ptr_atts_t_rep;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_987;extern struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Conref0bool2_rep;
extern struct Cyc_Typerep_TUnion_struct Cyc_datatype_Absyn_Constraint0bool2_rep;
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_85;struct _tuple25{unsigned int f1;
int f2;};static struct _tuple6 Cyc__gentuple_86={offsetof(struct _tuple25,f1),(void*)&
Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_87={offsetof(struct _tuple25,f2),(
void*)& Cyc__genrep_10};static struct _tuple6*Cyc__genarr_88[2]={& Cyc__gentuple_86,&
Cyc__gentuple_87};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_85={4,sizeof(
struct _tuple25),{(void*)((struct _tuple6**)Cyc__genarr_88),(void*)((struct _tuple6**)
Cyc__genarr_88),(void*)((struct _tuple6**)Cyc__genarr_88 + 2)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_990;struct _tuple26{unsigned int f1;struct Cyc_Absyn_Conref*f2;};static
struct _tuple6 Cyc__gentuple_991={offsetof(struct _tuple26,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_992={offsetof(struct _tuple26,f2),(void*)& Cyc__genrep_987};
static struct _tuple6*Cyc__genarr_993[2]={& Cyc__gentuple_991,& Cyc__gentuple_992};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_990={4,sizeof(struct _tuple26),{(
void*)((struct _tuple6**)Cyc__genarr_993),(void*)((struct _tuple6**)Cyc__genarr_993),(
void*)((struct _tuple6**)Cyc__genarr_993 + 2)}};static char _tmp23A[10]="No_constr";
static struct _tuple7 Cyc__gentuple_988={0,(struct _dyneither_ptr){_tmp23A,_tmp23A,
_tmp23A + 10}};static struct _tuple7*Cyc__genarr_989[1]={& Cyc__gentuple_988};static
char _tmp23B[10]="Eq_constr";static struct _tuple5 Cyc__gentuple_994={0,(struct
_dyneither_ptr){_tmp23B,_tmp23B,_tmp23B + 10},(void*)& Cyc__genrep_85};static char
_tmp23C[15]="Forward_constr";static struct _tuple5 Cyc__gentuple_995={1,(struct
_dyneither_ptr){_tmp23C,_tmp23C,_tmp23C + 15},(void*)& Cyc__genrep_990};static
struct _tuple5*Cyc__genarr_996[2]={& Cyc__gentuple_994,& Cyc__gentuple_995};static
char _tmp23E[11]="Constraint";struct Cyc_Typerep_TUnion_struct Cyc_datatype_Absyn_Constraint0bool2_rep={
5,(struct _dyneither_ptr){_tmp23E,_tmp23E,_tmp23E + 11},{(void*)((struct _tuple7**)
Cyc__genarr_989),(void*)((struct _tuple7**)Cyc__genarr_989),(void*)((struct
_tuple7**)Cyc__genarr_989 + 1)},{(void*)((struct _tuple5**)Cyc__genarr_996),(void*)((
struct _tuple5**)Cyc__genarr_996),(void*)((struct _tuple5**)Cyc__genarr_996 + 2)}};
static char _tmp23F[7]="Conref";static struct _dyneither_ptr Cyc__genname_999=(struct
_dyneither_ptr){_tmp23F,_tmp23F,_tmp23F + 7};static char _tmp240[2]="v";static
struct _tuple5 Cyc__gentuple_997={offsetof(struct Cyc_Absyn_Conref,v),(struct
_dyneither_ptr){_tmp240,_tmp240,_tmp240 + 2},(void*)& Cyc_datatype_Absyn_Constraint0bool2_rep};
static struct _tuple5*Cyc__genarr_998[1]={& Cyc__gentuple_997};struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_Conref0bool2_rep={3,(struct _dyneither_ptr*)& Cyc__genname_999,
sizeof(struct Cyc_Absyn_Conref),{(void*)((struct _tuple5**)Cyc__genarr_998),(void*)((
struct _tuple5**)Cyc__genarr_998),(void*)((struct _tuple5**)Cyc__genarr_998 + 1)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_987={1,1,(void*)((void*)& Cyc_struct_Absyn_Conref0bool2_rep)};
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_1029;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_Conref0Absyn_bounds_t2_rep;static char _tmp243[7]="Conref";static
struct _dyneither_ptr Cyc__genname_1032=(struct _dyneither_ptr){_tmp243,_tmp243,
_tmp243 + 7};static char _tmp244[2]="v";static struct _tuple5 Cyc__gentuple_1030={
offsetof(struct Cyc_Absyn_Conref,v),(struct _dyneither_ptr){_tmp244,_tmp244,
_tmp244 + 2},(void*)& Cyc_datatype_Absyn_Constraint0bool2_rep};static struct _tuple5*
Cyc__genarr_1031[1]={& Cyc__gentuple_1030};struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Conref0Absyn_bounds_t2_rep={
3,(struct _dyneither_ptr*)& Cyc__genname_1032,sizeof(struct Cyc_Absyn_Conref),{(
void*)((struct _tuple5**)Cyc__genarr_1031),(void*)((struct _tuple5**)Cyc__genarr_1031),(
void*)((struct _tuple5**)Cyc__genarr_1031 + 1)}};static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_1029={1,1,(void*)((void*)& Cyc_struct_Absyn_Conref0Absyn_bounds_t2_rep)};
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_1023;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_PtrLoc_rep;static char _tmp247[7]="PtrLoc";static struct
_dyneither_ptr Cyc__genname_1028=(struct _dyneither_ptr){_tmp247,_tmp247,_tmp247 + 
7};static char _tmp248[8]="ptr_loc";static struct _tuple5 Cyc__gentuple_1024={
offsetof(struct Cyc_Absyn_PtrLoc,ptr_loc),(struct _dyneither_ptr){_tmp248,_tmp248,
_tmp248 + 8},(void*)& Cyc__genrep_2};static char _tmp249[8]="rgn_loc";static struct
_tuple5 Cyc__gentuple_1025={offsetof(struct Cyc_Absyn_PtrLoc,rgn_loc),(struct
_dyneither_ptr){_tmp249,_tmp249,_tmp249 + 8},(void*)& Cyc__genrep_2};static char
_tmp24A[7]="zt_loc";static struct _tuple5 Cyc__gentuple_1026={offsetof(struct Cyc_Absyn_PtrLoc,zt_loc),(
struct _dyneither_ptr){_tmp24A,_tmp24A,_tmp24A + 7},(void*)& Cyc__genrep_2};static
struct _tuple5*Cyc__genarr_1027[3]={& Cyc__gentuple_1024,& Cyc__gentuple_1025,& Cyc__gentuple_1026};
struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_PtrLoc_rep={3,(struct
_dyneither_ptr*)& Cyc__genname_1028,sizeof(struct Cyc_Absyn_PtrLoc),{(void*)((
struct _tuple5**)Cyc__genarr_1027),(void*)((struct _tuple5**)Cyc__genarr_1027),(
void*)((struct _tuple5**)Cyc__genarr_1027 + 3)}};static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_1023={1,1,(void*)((void*)& Cyc_struct_Absyn_PtrLoc_rep)};static char
_tmp24D[8]="PtrAtts";static struct _dyneither_ptr Cyc__genname_1039=(struct
_dyneither_ptr){_tmp24D,_tmp24D,_tmp24D + 8};static char _tmp24E[4]="rgn";static
struct _tuple5 Cyc__gentuple_1033={offsetof(struct Cyc_Absyn_PtrAtts,rgn),(struct
_dyneither_ptr){_tmp24E,_tmp24E,_tmp24E + 4},(void*)((void*)& Cyc_Absyn_type_t_rep)};
static char _tmp24F[9]="nullable";static struct _tuple5 Cyc__gentuple_1034={offsetof(
struct Cyc_Absyn_PtrAtts,nullable),(struct _dyneither_ptr){_tmp24F,_tmp24F,_tmp24F
+ 9},(void*)& Cyc__genrep_987};static char _tmp250[7]="bounds";static struct _tuple5
Cyc__gentuple_1035={offsetof(struct Cyc_Absyn_PtrAtts,bounds),(struct
_dyneither_ptr){_tmp250,_tmp250,_tmp250 + 7},(void*)& Cyc__genrep_1029};static char
_tmp251[10]="zero_term";static struct _tuple5 Cyc__gentuple_1036={offsetof(struct
Cyc_Absyn_PtrAtts,zero_term),(struct _dyneither_ptr){_tmp251,_tmp251,_tmp251 + 10},(
void*)& Cyc__genrep_987};static char _tmp252[7]="ptrloc";static struct _tuple5 Cyc__gentuple_1037={
offsetof(struct Cyc_Absyn_PtrAtts,ptrloc),(struct _dyneither_ptr){_tmp252,_tmp252,
_tmp252 + 7},(void*)& Cyc__genrep_1023};static struct _tuple5*Cyc__genarr_1038[5]={&
Cyc__gentuple_1033,& Cyc__gentuple_1034,& Cyc__gentuple_1035,& Cyc__gentuple_1036,&
Cyc__gentuple_1037};struct Cyc_Typerep_Struct_struct Cyc_Absyn_ptr_atts_t_rep={3,(
struct _dyneither_ptr*)& Cyc__genname_1039,sizeof(struct Cyc_Absyn_PtrAtts),{(void*)((
struct _tuple5**)Cyc__genarr_1038),(void*)((struct _tuple5**)Cyc__genarr_1038),(
void*)((struct _tuple5**)Cyc__genarr_1038 + 5)}};static char _tmp254[8]="PtrInfo";
static struct _dyneither_ptr Cyc__genname_1044=(struct _dyneither_ptr){_tmp254,
_tmp254,_tmp254 + 8};static char _tmp255[8]="elt_typ";static struct _tuple5 Cyc__gentuple_1040={
offsetof(struct Cyc_Absyn_PtrInfo,elt_typ),(struct _dyneither_ptr){_tmp255,_tmp255,
_tmp255 + 8},(void*)((void*)& Cyc_Absyn_type_t_rep)};static char _tmp256[7]="elt_tq";
static struct _tuple5 Cyc__gentuple_1041={offsetof(struct Cyc_Absyn_PtrInfo,elt_tq),(
struct _dyneither_ptr){_tmp256,_tmp256,_tmp256 + 7},(void*)& Cyc_Absyn_tqual_t_rep};
static char _tmp257[9]="ptr_atts";static struct _tuple5 Cyc__gentuple_1042={offsetof(
struct Cyc_Absyn_PtrInfo,ptr_atts),(struct _dyneither_ptr){_tmp257,_tmp257,_tmp257
+ 9},(void*)& Cyc_Absyn_ptr_atts_t_rep};static struct _tuple5*Cyc__genarr_1043[3]={&
Cyc__gentuple_1040,& Cyc__gentuple_1041,& Cyc__gentuple_1042};struct Cyc_Typerep_Struct_struct
Cyc_Absyn_ptr_info_t_rep={3,(struct _dyneither_ptr*)& Cyc__genname_1044,sizeof(
struct Cyc_Absyn_PtrInfo),{(void*)((struct _tuple5**)Cyc__genarr_1043),(void*)((
struct _tuple5**)Cyc__genarr_1043),(void*)((struct _tuple5**)Cyc__genarr_1043 + 3)}};
struct _tuple27{unsigned int f1;struct Cyc_Absyn_PtrInfo f2;};static struct _tuple6 Cyc__gentuple_1045={
offsetof(struct _tuple27,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_1046={
offsetof(struct _tuple27,f2),(void*)& Cyc_Absyn_ptr_info_t_rep};static struct
_tuple6*Cyc__genarr_1047[2]={& Cyc__gentuple_1045,& Cyc__gentuple_1046};static
struct Cyc_Typerep_Tuple_struct Cyc__genrep_1022={4,sizeof(struct _tuple27),{(void*)((
struct _tuple6**)Cyc__genarr_1047),(void*)((struct _tuple6**)Cyc__genarr_1047),(
void*)((struct _tuple6**)Cyc__genarr_1047 + 2)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_1010;extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_sign_t_rep;
static char _tmp25A[7]="Signed";static struct _tuple7 Cyc__gentuple_236={0,(struct
_dyneither_ptr){_tmp25A,_tmp25A,_tmp25A + 7}};static char _tmp25B[9]="Unsigned";
static struct _tuple7 Cyc__gentuple_237={1,(struct _dyneither_ptr){_tmp25B,_tmp25B,
_tmp25B + 9}};static char _tmp25C[5]="None";static struct _tuple7 Cyc__gentuple_238={2,(
struct _dyneither_ptr){_tmp25C,_tmp25C,_tmp25C + 5}};static struct _tuple7*Cyc__genarr_239[
3]={& Cyc__gentuple_236,& Cyc__gentuple_237,& Cyc__gentuple_238};static struct
_tuple5*Cyc__genarr_240[0]={};static char _tmp25E[5]="Sign";struct Cyc_Typerep_TUnion_struct
Cyc_Absyn_sign_t_rep={5,(struct _dyneither_ptr){_tmp25E,_tmp25E,_tmp25E + 5},{(
void*)((struct _tuple7**)Cyc__genarr_239),(void*)((struct _tuple7**)Cyc__genarr_239),(
void*)((struct _tuple7**)Cyc__genarr_239 + 3)},{(void*)((struct _tuple5**)Cyc__genarr_240),(
void*)((struct _tuple5**)Cyc__genarr_240),(void*)((struct _tuple5**)Cyc__genarr_240
+ 0)}};extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_size_of_t_rep;static char
_tmp25F[8]="Char_sz";static struct _tuple7 Cyc__gentuple_1011={0,(struct
_dyneither_ptr){_tmp25F,_tmp25F,_tmp25F + 8}};static char _tmp260[9]="Short_sz";
static struct _tuple7 Cyc__gentuple_1012={1,(struct _dyneither_ptr){_tmp260,_tmp260,
_tmp260 + 9}};static char _tmp261[7]="Int_sz";static struct _tuple7 Cyc__gentuple_1013={
2,(struct _dyneither_ptr){_tmp261,_tmp261,_tmp261 + 7}};static char _tmp262[8]="Long_sz";
static struct _tuple7 Cyc__gentuple_1014={3,(struct _dyneither_ptr){_tmp262,_tmp262,
_tmp262 + 8}};static char _tmp263[12]="LongLong_sz";static struct _tuple7 Cyc__gentuple_1015={
4,(struct _dyneither_ptr){_tmp263,_tmp263,_tmp263 + 12}};static struct _tuple7*Cyc__genarr_1016[
5]={& Cyc__gentuple_1011,& Cyc__gentuple_1012,& Cyc__gentuple_1013,& Cyc__gentuple_1014,&
Cyc__gentuple_1015};static struct _tuple5*Cyc__genarr_1017[0]={};static char _tmp265[
8]="Size_of";struct Cyc_Typerep_TUnion_struct Cyc_Absyn_size_of_t_rep={5,(struct
_dyneither_ptr){_tmp265,_tmp265,_tmp265 + 8},{(void*)((struct _tuple7**)Cyc__genarr_1016),(
void*)((struct _tuple7**)Cyc__genarr_1016),(void*)((struct _tuple7**)Cyc__genarr_1016
+ 5)},{(void*)((struct _tuple5**)Cyc__genarr_1017),(void*)((struct _tuple5**)Cyc__genarr_1017),(
void*)((struct _tuple5**)Cyc__genarr_1017 + 0)}};struct _tuple28{unsigned int f1;
void*f2;void*f3;};static struct _tuple6 Cyc__gentuple_1018={offsetof(struct _tuple28,f1),(
void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_1019={offsetof(struct
_tuple28,f2),(void*)& Cyc_Absyn_sign_t_rep};static struct _tuple6 Cyc__gentuple_1020={
offsetof(struct _tuple28,f3),(void*)& Cyc_Absyn_size_of_t_rep};static struct _tuple6*
Cyc__genarr_1021[3]={& Cyc__gentuple_1018,& Cyc__gentuple_1019,& Cyc__gentuple_1020};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_1010={4,sizeof(struct _tuple28),{(
void*)((struct _tuple6**)Cyc__genarr_1021),(void*)((struct _tuple6**)Cyc__genarr_1021),(
void*)((struct _tuple6**)Cyc__genarr_1021 + 3)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_986;extern struct Cyc_Typerep_Struct_struct Cyc_Absyn_array_info_t_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_145;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_Exp_rep;extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_raw_exp_t_rep;
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_852;extern struct Cyc_Typerep_TUnion_struct
Cyc_Absyn_cnst_t_rep;extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_867;struct
_tuple29{unsigned int f1;void*f2;char f3;};static struct _tuple6 Cyc__gentuple_868={
offsetof(struct _tuple29,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_869={
offsetof(struct _tuple29,f2),(void*)& Cyc_Absyn_sign_t_rep};static struct _tuple6 Cyc__gentuple_870={
offsetof(struct _tuple29,f3),(void*)((void*)& Cyc__genrep_15)};static struct _tuple6*
Cyc__genarr_871[3]={& Cyc__gentuple_868,& Cyc__gentuple_869,& Cyc__gentuple_870};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_867={4,sizeof(struct _tuple29),{(
void*)((struct _tuple6**)Cyc__genarr_871),(void*)((struct _tuple6**)Cyc__genarr_871),(
void*)((struct _tuple6**)Cyc__genarr_871 + 3)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_861;static struct Cyc_Typerep_Int_struct Cyc__genrep_862={0,1,16};
struct _tuple30{unsigned int f1;void*f2;short f3;};static struct _tuple6 Cyc__gentuple_863={
offsetof(struct _tuple30,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_864={
offsetof(struct _tuple30,f2),(void*)& Cyc_Absyn_sign_t_rep};static struct _tuple6 Cyc__gentuple_865={
offsetof(struct _tuple30,f3),(void*)& Cyc__genrep_862};static struct _tuple6*Cyc__genarr_866[
3]={& Cyc__gentuple_863,& Cyc__gentuple_864,& Cyc__gentuple_865};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_861={4,sizeof(struct _tuple30),{(void*)((struct _tuple6**)Cyc__genarr_866),(
void*)((struct _tuple6**)Cyc__genarr_866),(void*)((struct _tuple6**)Cyc__genarr_866
+ 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_235;struct _tuple31{
unsigned int f1;void*f2;int f3;};static struct _tuple6 Cyc__gentuple_241={offsetof(
struct _tuple31,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_242={
offsetof(struct _tuple31,f2),(void*)& Cyc_Absyn_sign_t_rep};static struct _tuple6 Cyc__gentuple_243={
offsetof(struct _tuple31,f3),(void*)& Cyc__genrep_10};static struct _tuple6*Cyc__genarr_244[
3]={& Cyc__gentuple_241,& Cyc__gentuple_242,& Cyc__gentuple_243};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_235={4,sizeof(struct _tuple31),{(void*)((struct _tuple6**)Cyc__genarr_244),(
void*)((struct _tuple6**)Cyc__genarr_244),(void*)((struct _tuple6**)Cyc__genarr_244
+ 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_855;static struct Cyc_Typerep_Int_struct
Cyc__genrep_856={0,1,64};struct _tuple32{unsigned int f1;void*f2;long long f3;};
static struct _tuple6 Cyc__gentuple_857={offsetof(struct _tuple32,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_858={offsetof(struct _tuple32,f2),(void*)& Cyc_Absyn_sign_t_rep};
static struct _tuple6 Cyc__gentuple_859={offsetof(struct _tuple32,f3),(void*)& Cyc__genrep_856};
static struct _tuple6*Cyc__genarr_860[3]={& Cyc__gentuple_857,& Cyc__gentuple_858,&
Cyc__gentuple_859};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_855={4,
sizeof(struct _tuple32),{(void*)((struct _tuple6**)Cyc__genarr_860),(void*)((
struct _tuple6**)Cyc__genarr_860),(void*)((struct _tuple6**)Cyc__genarr_860 + 3)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_81;static struct _tuple6 Cyc__gentuple_82={
offsetof(struct _tuple7,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_83={
offsetof(struct _tuple7,f2),(void*)((void*)& Cyc__genrep_14)};static struct _tuple6*
Cyc__genarr_84[2]={& Cyc__gentuple_82,& Cyc__gentuple_83};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_81={4,sizeof(struct _tuple7),{(void*)((struct _tuple6**)Cyc__genarr_84),(
void*)((struct _tuple6**)Cyc__genarr_84),(void*)((struct _tuple6**)Cyc__genarr_84 + 
2)}};static char _tmp26E[7]="Null_c";static struct _tuple7 Cyc__gentuple_853={0,(
struct _dyneither_ptr){_tmp26E,_tmp26E,_tmp26E + 7}};static struct _tuple7*Cyc__genarr_854[
1]={& Cyc__gentuple_853};static char _tmp26F[7]="Char_c";static struct _tuple5 Cyc__gentuple_872={
0,(struct _dyneither_ptr){_tmp26F,_tmp26F,_tmp26F + 7},(void*)& Cyc__genrep_867};
static char _tmp270[8]="Short_c";static struct _tuple5 Cyc__gentuple_873={1,(struct
_dyneither_ptr){_tmp270,_tmp270,_tmp270 + 8},(void*)& Cyc__genrep_861};static char
_tmp271[6]="Int_c";static struct _tuple5 Cyc__gentuple_874={2,(struct _dyneither_ptr){
_tmp271,_tmp271,_tmp271 + 6},(void*)& Cyc__genrep_235};static char _tmp272[11]="LongLong_c";
static struct _tuple5 Cyc__gentuple_875={3,(struct _dyneither_ptr){_tmp272,_tmp272,
_tmp272 + 11},(void*)& Cyc__genrep_855};static char _tmp273[8]="Float_c";static
struct _tuple5 Cyc__gentuple_876={4,(struct _dyneither_ptr){_tmp273,_tmp273,_tmp273
+ 8},(void*)& Cyc__genrep_81};static char _tmp274[9]="String_c";static struct _tuple5
Cyc__gentuple_877={5,(struct _dyneither_ptr){_tmp274,_tmp274,_tmp274 + 9},(void*)&
Cyc__genrep_81};static struct _tuple5*Cyc__genarr_878[6]={& Cyc__gentuple_872,& Cyc__gentuple_873,&
Cyc__gentuple_874,& Cyc__gentuple_875,& Cyc__gentuple_876,& Cyc__gentuple_877};
static char _tmp276[5]="Cnst";struct Cyc_Typerep_TUnion_struct Cyc_Absyn_cnst_t_rep={
5,(struct _dyneither_ptr){_tmp276,_tmp276,_tmp276 + 5},{(void*)((struct _tuple7**)
Cyc__genarr_854),(void*)((struct _tuple7**)Cyc__genarr_854),(void*)((struct
_tuple7**)Cyc__genarr_854 + 1)},{(void*)((struct _tuple5**)Cyc__genarr_878),(void*)((
struct _tuple5**)Cyc__genarr_878),(void*)((struct _tuple5**)Cyc__genarr_878 + 6)}};
struct _tuple33{unsigned int f1;union Cyc_Absyn_Cnst_union f2;};static struct _tuple6
Cyc__gentuple_879={offsetof(struct _tuple33,f1),(void*)& Cyc__genrep_24};static
struct _tuple6 Cyc__gentuple_880={offsetof(struct _tuple33,f2),(void*)& Cyc_Absyn_cnst_t_rep};
static struct _tuple6*Cyc__genarr_881[2]={& Cyc__gentuple_879,& Cyc__gentuple_880};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_852={4,sizeof(struct _tuple33),{(
void*)((struct _tuple6**)Cyc__genarr_881),(void*)((struct _tuple6**)Cyc__genarr_881),(
void*)((struct _tuple6**)Cyc__genarr_881 + 2)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_800;extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_binding_t_rep;
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_807;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_808;extern struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Fndecl_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_811;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List060Absyn_var_t4Absyn_tqual_t4Absyn_type_t1_446H2_rep;extern
struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_812;extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_813;struct _tuple34{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;
void*f3;};static struct _tuple6 Cyc__gentuple_814={offsetof(struct _tuple34,f1),(
void*)& Cyc__genrep_13};static struct _tuple6 Cyc__gentuple_815={offsetof(struct
_tuple34,f2),(void*)& Cyc_Absyn_tqual_t_rep};static struct _tuple6 Cyc__gentuple_816={
offsetof(struct _tuple34,f3),(void*)((void*)& Cyc_Absyn_type_t_rep)};static struct
_tuple6*Cyc__genarr_817[3]={& Cyc__gentuple_814,& Cyc__gentuple_815,& Cyc__gentuple_816};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_813={4,sizeof(struct _tuple34),{(
void*)((struct _tuple6**)Cyc__genarr_817),(void*)((struct _tuple6**)Cyc__genarr_817),(
void*)((struct _tuple6**)Cyc__genarr_817 + 3)}};static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_812={1,1,(void*)((void*)& Cyc__genrep_813)};static char _tmp27A[5]="List";
static struct _dyneither_ptr Cyc__genname_821=(struct _dyneither_ptr){_tmp27A,
_tmp27A,_tmp27A + 5};static char _tmp27B[3]="hd";static struct _tuple5 Cyc__gentuple_818={
offsetof(struct Cyc_List_List,hd),(struct _dyneither_ptr){_tmp27B,_tmp27B,_tmp27B + 
3},(void*)& Cyc__genrep_812};static char _tmp27C[3]="tl";static struct _tuple5 Cyc__gentuple_819={
offsetof(struct Cyc_List_List,tl),(struct _dyneither_ptr){_tmp27C,_tmp27C,_tmp27C + 
3},(void*)& Cyc__genrep_811};static struct _tuple5*Cyc__genarr_820[2]={& Cyc__gentuple_818,&
Cyc__gentuple_819};struct Cyc_Typerep_Struct_struct Cyc_struct_List_List060Absyn_var_t4Absyn_tqual_t4Absyn_type_t1_446H2_rep={
3,(struct _dyneither_ptr*)& Cyc__genname_821,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_820),(void*)((struct _tuple5**)Cyc__genarr_820),(void*)((
struct _tuple5**)Cyc__genarr_820 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_811={
1,1,(void*)((void*)& Cyc_struct_List_List060Absyn_var_t4Absyn_tqual_t4Absyn_type_t1_446H2_rep)};
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_810;extern struct Cyc_Typerep_Struct_struct
Cyc_Absyn_vararg_info_t_rep;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_658;
extern struct Cyc_Typerep_Struct_struct Cyc_struct_Core_Opt0Absyn_var_t2_rep;static
char _tmp27F[4]="Opt";static struct _dyneither_ptr Cyc__genname_661=(struct
_dyneither_ptr){_tmp27F,_tmp27F,_tmp27F + 4};static char _tmp280[2]="v";static
struct _tuple5 Cyc__gentuple_659={offsetof(struct Cyc_Core_Opt,v),(struct
_dyneither_ptr){_tmp280,_tmp280,_tmp280 + 2},(void*)& Cyc__genrep_13};static struct
_tuple5*Cyc__genarr_660[1]={& Cyc__gentuple_659};struct Cyc_Typerep_Struct_struct
Cyc_struct_Core_Opt0Absyn_var_t2_rep={3,(struct _dyneither_ptr*)& Cyc__genname_661,
sizeof(struct Cyc_Core_Opt),{(void*)((struct _tuple5**)Cyc__genarr_660),(void*)((
struct _tuple5**)Cyc__genarr_660),(void*)((struct _tuple5**)Cyc__genarr_660 + 1)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_658={1,1,(void*)((void*)& Cyc_struct_Core_Opt0Absyn_var_t2_rep)};
static char _tmp283[11]="VarargInfo";static struct _dyneither_ptr Cyc__genname_730=(
struct _dyneither_ptr){_tmp283,_tmp283,_tmp283 + 11};static char _tmp284[5]="name";
static struct _tuple5 Cyc__gentuple_725={offsetof(struct Cyc_Absyn_VarargInfo,name),(
struct _dyneither_ptr){_tmp284,_tmp284,_tmp284 + 5},(void*)& Cyc__genrep_658};
static char _tmp285[3]="tq";static struct _tuple5 Cyc__gentuple_726={offsetof(struct
Cyc_Absyn_VarargInfo,tq),(struct _dyneither_ptr){_tmp285,_tmp285,_tmp285 + 3},(
void*)& Cyc_Absyn_tqual_t_rep};static char _tmp286[5]="type";static struct _tuple5 Cyc__gentuple_727={
offsetof(struct Cyc_Absyn_VarargInfo,type),(struct _dyneither_ptr){_tmp286,_tmp286,
_tmp286 + 5},(void*)((void*)& Cyc_Absyn_type_t_rep)};static char _tmp287[7]="inject";
static struct _tuple5 Cyc__gentuple_728={offsetof(struct Cyc_Absyn_VarargInfo,inject),(
struct _dyneither_ptr){_tmp287,_tmp287,_tmp287 + 7},(void*)& Cyc__genrep_10};static
struct _tuple5*Cyc__genarr_729[4]={& Cyc__gentuple_725,& Cyc__gentuple_726,& Cyc__gentuple_727,&
Cyc__gentuple_728};struct Cyc_Typerep_Struct_struct Cyc_Absyn_vararg_info_t_rep={3,(
struct _dyneither_ptr*)& Cyc__genname_730,sizeof(struct Cyc_Absyn_VarargInfo),{(
void*)((struct _tuple5**)Cyc__genarr_729),(void*)((struct _tuple5**)Cyc__genarr_729),(
void*)((struct _tuple5**)Cyc__genarr_729 + 4)}};static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_810={1,1,(void*)((void*)& Cyc_Absyn_vararg_info_t_rep)};extern struct
Cyc_Typerep_ThinPtr_struct Cyc__genrep_372;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List060Absyn_type_t4Absyn_type_t1_446H2_rep;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_373;extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_374;static struct
_tuple6 Cyc__gentuple_375={offsetof(struct _tuple9,f1),(void*)((void*)& Cyc_Absyn_type_t_rep)};
static struct _tuple6 Cyc__gentuple_376={offsetof(struct _tuple9,f2),(void*)((void*)&
Cyc_Absyn_type_t_rep)};static struct _tuple6*Cyc__genarr_377[2]={& Cyc__gentuple_375,&
Cyc__gentuple_376};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_374={4,
sizeof(struct _tuple9),{(void*)((struct _tuple6**)Cyc__genarr_377),(void*)((struct
_tuple6**)Cyc__genarr_377),(void*)((struct _tuple6**)Cyc__genarr_377 + 2)}};static
struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_373={1,1,(void*)((void*)& Cyc__genrep_374)};
static char _tmp28C[5]="List";static struct _dyneither_ptr Cyc__genname_381=(struct
_dyneither_ptr){_tmp28C,_tmp28C,_tmp28C + 5};static char _tmp28D[3]="hd";static
struct _tuple5 Cyc__gentuple_378={offsetof(struct Cyc_List_List,hd),(struct
_dyneither_ptr){_tmp28D,_tmp28D,_tmp28D + 3},(void*)& Cyc__genrep_373};static char
_tmp28E[3]="tl";static struct _tuple5 Cyc__gentuple_379={offsetof(struct Cyc_List_List,tl),(
struct _dyneither_ptr){_tmp28E,_tmp28E,_tmp28E + 3},(void*)& Cyc__genrep_372};
static struct _tuple5*Cyc__genarr_380[2]={& Cyc__gentuple_378,& Cyc__gentuple_379};
struct Cyc_Typerep_Struct_struct Cyc_struct_List_List060Absyn_type_t4Absyn_type_t1_446H2_rep={
3,(struct _dyneither_ptr*)& Cyc__genname_381,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_380),(void*)((struct _tuple5**)Cyc__genarr_380),(void*)((
struct _tuple5**)Cyc__genarr_380 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_372={
1,1,(void*)((void*)& Cyc_struct_List_List060Absyn_type_t4Absyn_type_t1_446H2_rep)};
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_134;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_Stmt_rep;extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_raw_stmt_t_rep;
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_129;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_130;static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_130={1,1,(void*)((
void*)& Cyc_struct_Absyn_Exp_rep)};struct _tuple35{unsigned int f1;struct Cyc_Absyn_Exp*
f2;};static struct _tuple6 Cyc__gentuple_929={offsetof(struct _tuple35,f1),(void*)&
Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_930={offsetof(struct _tuple35,f2),(
void*)& Cyc__genrep_130};static struct _tuple6*Cyc__genarr_931[2]={& Cyc__gentuple_929,&
Cyc__gentuple_930};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_129={4,
sizeof(struct _tuple35),{(void*)((struct _tuple6**)Cyc__genarr_931),(void*)((
struct _tuple6**)Cyc__genarr_931),(void*)((struct _tuple6**)Cyc__genarr_931 + 2)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_543;struct _tuple36{unsigned int
f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};static struct _tuple6 Cyc__gentuple_544={
offsetof(struct _tuple36,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_545={
offsetof(struct _tuple36,f2),(void*)& Cyc__genrep_134};static struct _tuple6 Cyc__gentuple_546={
offsetof(struct _tuple36,f3),(void*)& Cyc__genrep_134};static struct _tuple6*Cyc__genarr_547[
3]={& Cyc__gentuple_544,& Cyc__gentuple_545,& Cyc__gentuple_546};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_543={4,sizeof(struct _tuple36),{(void*)((struct _tuple6**)Cyc__genarr_547),(
void*)((struct _tuple6**)Cyc__genarr_547),(void*)((struct _tuple6**)Cyc__genarr_547
+ 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_539;static struct _tuple6
Cyc__gentuple_540={offsetof(struct _tuple35,f1),(void*)& Cyc__genrep_24};static
struct _tuple6 Cyc__gentuple_541={offsetof(struct _tuple35,f2),(void*)& Cyc__genrep_145};
static struct _tuple6*Cyc__genarr_542[2]={& Cyc__gentuple_540,& Cyc__gentuple_541};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_539={4,sizeof(struct _tuple35),{(
void*)((struct _tuple6**)Cyc__genarr_542),(void*)((struct _tuple6**)Cyc__genarr_542),(
void*)((struct _tuple6**)Cyc__genarr_542 + 2)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_533;struct _tuple37{unsigned int f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Stmt*
f3;struct Cyc_Absyn_Stmt*f4;};static struct _tuple6 Cyc__gentuple_534={offsetof(
struct _tuple37,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_535={
offsetof(struct _tuple37,f2),(void*)& Cyc__genrep_130};static struct _tuple6 Cyc__gentuple_536={
offsetof(struct _tuple37,f3),(void*)& Cyc__genrep_134};static struct _tuple6 Cyc__gentuple_537={
offsetof(struct _tuple37,f4),(void*)& Cyc__genrep_134};static struct _tuple6*Cyc__genarr_538[
4]={& Cyc__gentuple_534,& Cyc__gentuple_535,& Cyc__gentuple_536,& Cyc__gentuple_537};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_533={4,sizeof(struct _tuple37),{(
void*)((struct _tuple6**)Cyc__genarr_538),(void*)((struct _tuple6**)Cyc__genarr_538),(
void*)((struct _tuple6**)Cyc__genarr_538 + 4)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_528;extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_477;static struct
_tuple6 Cyc__gentuple_478={offsetof(struct _tuple2,f1),(void*)& Cyc__genrep_130};
static struct _tuple6 Cyc__gentuple_479={offsetof(struct _tuple2,f2),(void*)& Cyc__genrep_134};
static struct _tuple6*Cyc__genarr_480[2]={& Cyc__gentuple_478,& Cyc__gentuple_479};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_477={4,sizeof(struct _tuple2),{(
void*)((struct _tuple6**)Cyc__genarr_480),(void*)((struct _tuple6**)Cyc__genarr_480),(
void*)((struct _tuple6**)Cyc__genarr_480 + 2)}};struct _tuple38{unsigned int f1;
struct _tuple2 f2;struct Cyc_Absyn_Stmt*f3;};static struct _tuple6 Cyc__gentuple_529={
offsetof(struct _tuple38,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_530={
offsetof(struct _tuple38,f2),(void*)& Cyc__genrep_477};static struct _tuple6 Cyc__gentuple_531={
offsetof(struct _tuple38,f3),(void*)& Cyc__genrep_134};static struct _tuple6*Cyc__genarr_532[
3]={& Cyc__gentuple_529,& Cyc__gentuple_530,& Cyc__gentuple_531};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_528={4,sizeof(struct _tuple38),{(void*)((struct _tuple6**)Cyc__genarr_532),(
void*)((struct _tuple6**)Cyc__genarr_532),(void*)((struct _tuple6**)Cyc__genarr_532
+ 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_524;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_519;static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_519={1,1,(void*)((
void*)& Cyc_struct_Absyn_Stmt_rep)};struct _tuple39{unsigned int f1;struct Cyc_Absyn_Stmt*
f2;};static struct _tuple6 Cyc__gentuple_525={offsetof(struct _tuple39,f1),(void*)&
Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_526={offsetof(struct _tuple39,f2),(
void*)& Cyc__genrep_519};static struct _tuple6*Cyc__genarr_527[2]={& Cyc__gentuple_525,&
Cyc__gentuple_526};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_524={4,
sizeof(struct _tuple39),{(void*)((struct _tuple6**)Cyc__genarr_527),(void*)((
struct _tuple6**)Cyc__genarr_527),(void*)((struct _tuple6**)Cyc__genarr_527 + 2)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_518;struct _tuple40{unsigned int
f1;struct _dyneither_ptr*f2;struct Cyc_Absyn_Stmt*f3;};static struct _tuple6 Cyc__gentuple_520={
offsetof(struct _tuple40,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_521={
offsetof(struct _tuple40,f2),(void*)& Cyc__genrep_13};static struct _tuple6 Cyc__gentuple_522={
offsetof(struct _tuple40,f3),(void*)& Cyc__genrep_519};static struct _tuple6*Cyc__genarr_523[
3]={& Cyc__gentuple_520,& Cyc__gentuple_521,& Cyc__gentuple_522};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_518={4,sizeof(struct _tuple40),{(void*)((struct _tuple6**)Cyc__genarr_523),(
void*)((struct _tuple6**)Cyc__genarr_523),(void*)((struct _tuple6**)Cyc__genarr_523
+ 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_511;struct _tuple41{
unsigned int f1;struct Cyc_Absyn_Exp*f2;struct _tuple2 f3;struct _tuple2 f4;struct Cyc_Absyn_Stmt*
f5;};static struct _tuple6 Cyc__gentuple_512={offsetof(struct _tuple41,f1),(void*)&
Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_513={offsetof(struct _tuple41,f2),(
void*)& Cyc__genrep_130};static struct _tuple6 Cyc__gentuple_514={offsetof(struct
_tuple41,f3),(void*)& Cyc__genrep_477};static struct _tuple6 Cyc__gentuple_515={
offsetof(struct _tuple41,f4),(void*)& Cyc__genrep_477};static struct _tuple6 Cyc__gentuple_516={
offsetof(struct _tuple41,f5),(void*)& Cyc__genrep_134};static struct _tuple6*Cyc__genarr_517[
5]={& Cyc__gentuple_512,& Cyc__gentuple_513,& Cyc__gentuple_514,& Cyc__gentuple_515,&
Cyc__gentuple_516};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_511={4,
sizeof(struct _tuple41),{(void*)((struct _tuple6**)Cyc__genarr_517),(void*)((
struct _tuple6**)Cyc__genarr_517),(void*)((struct _tuple6**)Cyc__genarr_517 + 5)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_506;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_143;extern struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_switch_clause_t46H2_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_144;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_Switch_clause_rep;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_181;
extern struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Pat_rep;extern struct Cyc_Typerep_TUnion_struct
Cyc_Absyn_raw_pat_t_rep;extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_435;
struct _tuple42{unsigned int f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Pat*f3;
};static struct _tuple6 Cyc__gentuple_436={offsetof(struct _tuple42,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_437={offsetof(struct _tuple42,f2),(void*)& Cyc__genrep_148};
static struct _tuple6 Cyc__gentuple_438={offsetof(struct _tuple42,f3),(void*)& Cyc__genrep_181};
static struct _tuple6*Cyc__genarr_439[3]={& Cyc__gentuple_436,& Cyc__gentuple_437,&
Cyc__gentuple_438};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_435={4,
sizeof(struct _tuple42),{(void*)((struct _tuple6**)Cyc__genarr_439),(void*)((
struct _tuple6**)Cyc__genarr_439),(void*)((struct _tuple6**)Cyc__genarr_439 + 3)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_430;struct _tuple43{unsigned int
f1;struct Cyc_Absyn_Tvar*f2;struct Cyc_Absyn_Vardecl*f3;};static struct _tuple6 Cyc__gentuple_431={
offsetof(struct _tuple43,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_432={
offsetof(struct _tuple43,f2),(void*)& Cyc__genrep_274};static struct _tuple6 Cyc__gentuple_433={
offsetof(struct _tuple43,f3),(void*)& Cyc__genrep_148};static struct _tuple6*Cyc__genarr_434[
3]={& Cyc__gentuple_431,& Cyc__gentuple_432,& Cyc__gentuple_433};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_430={4,sizeof(struct _tuple43),{(void*)((struct _tuple6**)Cyc__genarr_434),(
void*)((struct _tuple6**)Cyc__genarr_434),(void*)((struct _tuple6**)Cyc__genarr_434
+ 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_425;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_186;extern struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_pat_t46H2_rep;
static char _tmp29E[5]="List";static struct _dyneither_ptr Cyc__genname_190=(struct
_dyneither_ptr){_tmp29E,_tmp29E,_tmp29E + 5};static char _tmp29F[3]="hd";static
struct _tuple5 Cyc__gentuple_187={offsetof(struct Cyc_List_List,hd),(struct
_dyneither_ptr){_tmp29F,_tmp29F,_tmp29F + 3},(void*)& Cyc__genrep_181};static char
_tmp2A0[3]="tl";static struct _tuple5 Cyc__gentuple_188={offsetof(struct Cyc_List_List,tl),(
struct _dyneither_ptr){_tmp2A0,_tmp2A0,_tmp2A0 + 3},(void*)& Cyc__genrep_186};
static struct _tuple5*Cyc__genarr_189[2]={& Cyc__gentuple_187,& Cyc__gentuple_188};
struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_pat_t46H2_rep={3,(
struct _dyneither_ptr*)& Cyc__genname_190,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_189),(void*)((struct _tuple5**)Cyc__genarr_189),(void*)((
struct _tuple5**)Cyc__genarr_189 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_186={
1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_pat_t46H2_rep)};struct _tuple44{
unsigned int f1;struct Cyc_List_List*f2;int f3;};static struct _tuple6 Cyc__gentuple_426={
offsetof(struct _tuple44,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_427={
offsetof(struct _tuple44,f2),(void*)& Cyc__genrep_186};static struct _tuple6 Cyc__gentuple_428={
offsetof(struct _tuple44,f3),(void*)& Cyc__genrep_10};static struct _tuple6*Cyc__genarr_429[
3]={& Cyc__gentuple_426,& Cyc__gentuple_427,& Cyc__gentuple_428};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_425={4,sizeof(struct _tuple44),{(void*)((struct _tuple6**)Cyc__genarr_429),(
void*)((struct _tuple6**)Cyc__genarr_429),(void*)((struct _tuple6**)Cyc__genarr_429
+ 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_421;struct _tuple45{
unsigned int f1;struct Cyc_Absyn_Pat*f2;};static struct _tuple6 Cyc__gentuple_422={
offsetof(struct _tuple45,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_423={
offsetof(struct _tuple45,f2),(void*)& Cyc__genrep_181};static struct _tuple6*Cyc__genarr_424[
2]={& Cyc__gentuple_422,& Cyc__gentuple_423};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_421={4,sizeof(struct _tuple45),{(void*)((struct _tuple6**)Cyc__genarr_424),(
void*)((struct _tuple6**)Cyc__genarr_424),(void*)((struct _tuple6**)Cyc__genarr_424
+ 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_330;extern struct Cyc_Typerep_Struct_struct
Cyc_Absyn_aggr_info_t_rep;extern struct Cyc_Typerep_TUnion_struct Cyc_datatype_Absyn_AggrInfoU_rep;
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_403;extern struct Cyc_Typerep_TUnion_struct
Cyc_Absyn_aggr_kind_t_rep;static char _tmp2A5[8]="StructA";static struct _tuple7 Cyc__gentuple_388={
0,(struct _dyneither_ptr){_tmp2A5,_tmp2A5,_tmp2A5 + 8}};static char _tmp2A6[7]="UnionA";
static struct _tuple7 Cyc__gentuple_389={1,(struct _dyneither_ptr){_tmp2A6,_tmp2A6,
_tmp2A6 + 7}};static struct _tuple7*Cyc__genarr_390[2]={& Cyc__gentuple_388,& Cyc__gentuple_389};
static struct _tuple5*Cyc__genarr_391[0]={};static char _tmp2A8[9]="AggrKind";struct
Cyc_Typerep_TUnion_struct Cyc_Absyn_aggr_kind_t_rep={5,(struct _dyneither_ptr){
_tmp2A8,_tmp2A8,_tmp2A8 + 9},{(void*)((struct _tuple7**)Cyc__genarr_390),(void*)((
struct _tuple7**)Cyc__genarr_390),(void*)((struct _tuple7**)Cyc__genarr_390 + 2)},{(
void*)((struct _tuple5**)Cyc__genarr_391),(void*)((struct _tuple5**)Cyc__genarr_391),(
void*)((struct _tuple5**)Cyc__genarr_391 + 0)}};struct _tuple46{unsigned int f1;void*
f2;struct _tuple0*f3;};static struct _tuple6 Cyc__gentuple_404={offsetof(struct
_tuple46,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_405={
offsetof(struct _tuple46,f2),(void*)& Cyc_Absyn_aggr_kind_t_rep};static struct
_tuple6 Cyc__gentuple_406={offsetof(struct _tuple46,f3),(void*)& Cyc__genrep_11};
static struct _tuple6*Cyc__genarr_407[3]={& Cyc__gentuple_404,& Cyc__gentuple_405,&
Cyc__gentuple_406};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_403={4,
sizeof(struct _tuple46),{(void*)((struct _tuple6**)Cyc__genarr_407),(void*)((
struct _tuple6**)Cyc__genarr_407),(void*)((struct _tuple6**)Cyc__genarr_407 + 3)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_355;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_356;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_357;extern
struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Aggrdecl_rep;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_358;extern struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_AggrdeclImpl_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_359;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List0Absyn_aggrfield_t46H2_rep;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_360;extern struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Aggrfield_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_62;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List0Absyn_attribute_t46H2_rep;extern struct Cyc_Typerep_TUnion_struct
Cyc_Absyn_attribute_t_rep;extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_89;
extern struct Cyc_Typerep_TUnion_struct Cyc_datatype_Absyn_Format_Type_rep;static
char _tmp2AA[10]="Printf_ft";static struct _tuple7 Cyc__gentuple_90={0,(struct
_dyneither_ptr){_tmp2AA,_tmp2AA,_tmp2AA + 10}};static char _tmp2AB[9]="Scanf_ft";
static struct _tuple7 Cyc__gentuple_91={1,(struct _dyneither_ptr){_tmp2AB,_tmp2AB,
_tmp2AB + 9}};static struct _tuple7*Cyc__genarr_92[2]={& Cyc__gentuple_90,& Cyc__gentuple_91};
static struct _tuple5*Cyc__genarr_93[0]={};static char _tmp2AD[12]="Format_Type";
struct Cyc_Typerep_TUnion_struct Cyc_datatype_Absyn_Format_Type_rep={5,(struct
_dyneither_ptr){_tmp2AD,_tmp2AD,_tmp2AD + 12},{(void*)((struct _tuple7**)Cyc__genarr_92),(
void*)((struct _tuple7**)Cyc__genarr_92),(void*)((struct _tuple7**)Cyc__genarr_92 + 
2)},{(void*)((struct _tuple5**)Cyc__genarr_93),(void*)((struct _tuple5**)Cyc__genarr_93),(
void*)((struct _tuple5**)Cyc__genarr_93 + 0)}};struct _tuple47{unsigned int f1;void*
f2;int f3;int f4;};static struct _tuple6 Cyc__gentuple_94={offsetof(struct _tuple47,f1),(
void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_95={offsetof(struct
_tuple47,f2),(void*)& Cyc_datatype_Absyn_Format_Type_rep};static struct _tuple6 Cyc__gentuple_96={
offsetof(struct _tuple47,f3),(void*)& Cyc__genrep_10};static struct _tuple6 Cyc__gentuple_97={
offsetof(struct _tuple47,f4),(void*)& Cyc__genrep_10};static struct _tuple6*Cyc__genarr_98[
4]={& Cyc__gentuple_94,& Cyc__gentuple_95,& Cyc__gentuple_96,& Cyc__gentuple_97};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_89={4,sizeof(struct _tuple47),{(
void*)((struct _tuple6**)Cyc__genarr_98),(void*)((struct _tuple6**)Cyc__genarr_98),(
void*)((struct _tuple6**)Cyc__genarr_98 + 4)}};static char _tmp2AF[12]="Stdcall_att";
static struct _tuple7 Cyc__gentuple_63={0,(struct _dyneither_ptr){_tmp2AF,_tmp2AF,
_tmp2AF + 12}};static char _tmp2B0[10]="Cdecl_att";static struct _tuple7 Cyc__gentuple_64={
1,(struct _dyneither_ptr){_tmp2B0,_tmp2B0,_tmp2B0 + 10}};static char _tmp2B1[13]="Fastcall_att";
static struct _tuple7 Cyc__gentuple_65={2,(struct _dyneither_ptr){_tmp2B1,_tmp2B1,
_tmp2B1 + 13}};static char _tmp2B2[13]="Noreturn_att";static struct _tuple7 Cyc__gentuple_66={
3,(struct _dyneither_ptr){_tmp2B2,_tmp2B2,_tmp2B2 + 13}};static char _tmp2B3[10]="Const_att";
static struct _tuple7 Cyc__gentuple_67={4,(struct _dyneither_ptr){_tmp2B3,_tmp2B3,
_tmp2B3 + 10}};static char _tmp2B4[11]="Packed_att";static struct _tuple7 Cyc__gentuple_68={
5,(struct _dyneither_ptr){_tmp2B4,_tmp2B4,_tmp2B4 + 11}};static char _tmp2B5[13]="Nocommon_att";
static struct _tuple7 Cyc__gentuple_69={6,(struct _dyneither_ptr){_tmp2B5,_tmp2B5,
_tmp2B5 + 13}};static char _tmp2B6[11]="Shared_att";static struct _tuple7 Cyc__gentuple_70={
7,(struct _dyneither_ptr){_tmp2B6,_tmp2B6,_tmp2B6 + 11}};static char _tmp2B7[11]="Unused_att";
static struct _tuple7 Cyc__gentuple_71={8,(struct _dyneither_ptr){_tmp2B7,_tmp2B7,
_tmp2B7 + 11}};static char _tmp2B8[9]="Weak_att";static struct _tuple7 Cyc__gentuple_72={
9,(struct _dyneither_ptr){_tmp2B8,_tmp2B8,_tmp2B8 + 9}};static char _tmp2B9[14]="Dllimport_att";
static struct _tuple7 Cyc__gentuple_73={10,(struct _dyneither_ptr){_tmp2B9,_tmp2B9,
_tmp2B9 + 14}};static char _tmp2BA[14]="Dllexport_att";static struct _tuple7 Cyc__gentuple_74={
11,(struct _dyneither_ptr){_tmp2BA,_tmp2BA,_tmp2BA + 14}};static char _tmp2BB[27]="No_instrument_function_att";
static struct _tuple7 Cyc__gentuple_75={12,(struct _dyneither_ptr){_tmp2BB,_tmp2BB,
_tmp2BB + 27}};static char _tmp2BC[16]="Constructor_att";static struct _tuple7 Cyc__gentuple_76={
13,(struct _dyneither_ptr){_tmp2BC,_tmp2BC,_tmp2BC + 16}};static char _tmp2BD[15]="Destructor_att";
static struct _tuple7 Cyc__gentuple_77={14,(struct _dyneither_ptr){_tmp2BD,_tmp2BD,
_tmp2BD + 15}};static char _tmp2BE[26]="No_check_memory_usage_att";static struct
_tuple7 Cyc__gentuple_78={15,(struct _dyneither_ptr){_tmp2BE,_tmp2BE,_tmp2BE + 26}};
static char _tmp2BF[9]="Pure_att";static struct _tuple7 Cyc__gentuple_79={16,(struct
_dyneither_ptr){_tmp2BF,_tmp2BF,_tmp2BF + 9}};static struct _tuple7*Cyc__genarr_80[
17]={& Cyc__gentuple_63,& Cyc__gentuple_64,& Cyc__gentuple_65,& Cyc__gentuple_66,&
Cyc__gentuple_67,& Cyc__gentuple_68,& Cyc__gentuple_69,& Cyc__gentuple_70,& Cyc__gentuple_71,&
Cyc__gentuple_72,& Cyc__gentuple_73,& Cyc__gentuple_74,& Cyc__gentuple_75,& Cyc__gentuple_76,&
Cyc__gentuple_77,& Cyc__gentuple_78,& Cyc__gentuple_79};static char _tmp2C0[12]="Regparm_att";
static struct _tuple5 Cyc__gentuple_99={0,(struct _dyneither_ptr){_tmp2C0,_tmp2C0,
_tmp2C0 + 12},(void*)& Cyc__genrep_85};static char _tmp2C1[12]="Aligned_att";static
struct _tuple5 Cyc__gentuple_100={1,(struct _dyneither_ptr){_tmp2C1,_tmp2C1,_tmp2C1
+ 12},(void*)& Cyc__genrep_85};static char _tmp2C2[12]="Section_att";static struct
_tuple5 Cyc__gentuple_101={2,(struct _dyneither_ptr){_tmp2C2,_tmp2C2,_tmp2C2 + 12},(
void*)& Cyc__genrep_81};static char _tmp2C3[11]="Format_att";static struct _tuple5 Cyc__gentuple_102={
3,(struct _dyneither_ptr){_tmp2C3,_tmp2C3,_tmp2C3 + 11},(void*)& Cyc__genrep_89};
static char _tmp2C4[16]="Initializes_att";static struct _tuple5 Cyc__gentuple_103={4,(
struct _dyneither_ptr){_tmp2C4,_tmp2C4,_tmp2C4 + 16},(void*)& Cyc__genrep_85};
static char _tmp2C5[9]="Mode_att";static struct _tuple5 Cyc__gentuple_104={5,(struct
_dyneither_ptr){_tmp2C5,_tmp2C5,_tmp2C5 + 9},(void*)& Cyc__genrep_81};static struct
_tuple5*Cyc__genarr_105[6]={& Cyc__gentuple_99,& Cyc__gentuple_100,& Cyc__gentuple_101,&
Cyc__gentuple_102,& Cyc__gentuple_103,& Cyc__gentuple_104};static char _tmp2C7[10]="Attribute";
struct Cyc_Typerep_TUnion_struct Cyc_Absyn_attribute_t_rep={5,(struct
_dyneither_ptr){_tmp2C7,_tmp2C7,_tmp2C7 + 10},{(void*)((struct _tuple7**)Cyc__genarr_80),(
void*)((struct _tuple7**)Cyc__genarr_80),(void*)((struct _tuple7**)Cyc__genarr_80 + 
17)},{(void*)((struct _tuple5**)Cyc__genarr_105),(void*)((struct _tuple5**)Cyc__genarr_105),(
void*)((struct _tuple5**)Cyc__genarr_105 + 6)}};static char _tmp2C8[5]="List";static
struct _dyneither_ptr Cyc__genname_109=(struct _dyneither_ptr){_tmp2C8,_tmp2C8,
_tmp2C8 + 5};static char _tmp2C9[3]="hd";static struct _tuple5 Cyc__gentuple_106={
offsetof(struct Cyc_List_List,hd),(struct _dyneither_ptr){_tmp2C9,_tmp2C9,_tmp2C9 + 
3},(void*)& Cyc_Absyn_attribute_t_rep};static char _tmp2CA[3]="tl";static struct
_tuple5 Cyc__gentuple_107={offsetof(struct Cyc_List_List,tl),(struct _dyneither_ptr){
_tmp2CA,_tmp2CA,_tmp2CA + 3},(void*)& Cyc__genrep_62};static struct _tuple5*Cyc__genarr_108[
2]={& Cyc__gentuple_106,& Cyc__gentuple_107};struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_attribute_t46H2_rep={
3,(struct _dyneither_ptr*)& Cyc__genname_109,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_108),(void*)((struct _tuple5**)Cyc__genarr_108),(void*)((
struct _tuple5**)Cyc__genarr_108 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_62={
1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_attribute_t46H2_rep)};static char
_tmp2CD[10]="Aggrfield";static struct _dyneither_ptr Cyc__genname_367=(struct
_dyneither_ptr){_tmp2CD,_tmp2CD,_tmp2CD + 10};static char _tmp2CE[5]="name";static
struct _tuple5 Cyc__gentuple_361={offsetof(struct Cyc_Absyn_Aggrfield,name),(struct
_dyneither_ptr){_tmp2CE,_tmp2CE,_tmp2CE + 5},(void*)& Cyc__genrep_13};static char
_tmp2CF[3]="tq";static struct _tuple5 Cyc__gentuple_362={offsetof(struct Cyc_Absyn_Aggrfield,tq),(
struct _dyneither_ptr){_tmp2CF,_tmp2CF,_tmp2CF + 3},(void*)& Cyc_Absyn_tqual_t_rep};
static char _tmp2D0[5]="type";static struct _tuple5 Cyc__gentuple_363={offsetof(
struct Cyc_Absyn_Aggrfield,type),(struct _dyneither_ptr){_tmp2D0,_tmp2D0,_tmp2D0 + 
5},(void*)((void*)& Cyc_Absyn_type_t_rep)};static char _tmp2D1[6]="width";static
struct _tuple5 Cyc__gentuple_364={offsetof(struct Cyc_Absyn_Aggrfield,width),(
struct _dyneither_ptr){_tmp2D1,_tmp2D1,_tmp2D1 + 6},(void*)& Cyc__genrep_145};
static char _tmp2D2[11]="attributes";static struct _tuple5 Cyc__gentuple_365={
offsetof(struct Cyc_Absyn_Aggrfield,attributes),(struct _dyneither_ptr){_tmp2D2,
_tmp2D2,_tmp2D2 + 11},(void*)& Cyc__genrep_62};static struct _tuple5*Cyc__genarr_366[
5]={& Cyc__gentuple_361,& Cyc__gentuple_362,& Cyc__gentuple_363,& Cyc__gentuple_364,&
Cyc__gentuple_365};struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Aggrfield_rep={
3,(struct _dyneither_ptr*)& Cyc__genname_367,sizeof(struct Cyc_Absyn_Aggrfield),{(
void*)((struct _tuple5**)Cyc__genarr_366),(void*)((struct _tuple5**)Cyc__genarr_366),(
void*)((struct _tuple5**)Cyc__genarr_366 + 5)}};static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_360={1,1,(void*)((void*)& Cyc_struct_Absyn_Aggrfield_rep)};static char
_tmp2D5[5]="List";static struct _dyneither_ptr Cyc__genname_371=(struct
_dyneither_ptr){_tmp2D5,_tmp2D5,_tmp2D5 + 5};static char _tmp2D6[3]="hd";static
struct _tuple5 Cyc__gentuple_368={offsetof(struct Cyc_List_List,hd),(struct
_dyneither_ptr){_tmp2D6,_tmp2D6,_tmp2D6 + 3},(void*)& Cyc__genrep_360};static char
_tmp2D7[3]="tl";static struct _tuple5 Cyc__gentuple_369={offsetof(struct Cyc_List_List,tl),(
struct _dyneither_ptr){_tmp2D7,_tmp2D7,_tmp2D7 + 3},(void*)& Cyc__genrep_359};
static struct _tuple5*Cyc__genarr_370[2]={& Cyc__gentuple_368,& Cyc__gentuple_369};
struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_aggrfield_t46H2_rep={3,(
struct _dyneither_ptr*)& Cyc__genname_371,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_370),(void*)((struct _tuple5**)Cyc__genarr_370),(void*)((
struct _tuple5**)Cyc__genarr_370 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_359={
1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_aggrfield_t46H2_rep)};static char
_tmp2DA[13]="AggrdeclImpl";static struct _dyneither_ptr Cyc__genname_387=(struct
_dyneither_ptr){_tmp2DA,_tmp2DA,_tmp2DA + 13};static char _tmp2DB[11]="exist_vars";
static struct _tuple5 Cyc__gentuple_382={offsetof(struct Cyc_Absyn_AggrdeclImpl,exist_vars),(
struct _dyneither_ptr){_tmp2DB,_tmp2DB,_tmp2DB + 11},(void*)& Cyc__genrep_273};
static char _tmp2DC[7]="rgn_po";static struct _tuple5 Cyc__gentuple_383={offsetof(
struct Cyc_Absyn_AggrdeclImpl,rgn_po),(struct _dyneither_ptr){_tmp2DC,_tmp2DC,
_tmp2DC + 7},(void*)& Cyc__genrep_372};static char _tmp2DD[7]="fields";static struct
_tuple5 Cyc__gentuple_384={offsetof(struct Cyc_Absyn_AggrdeclImpl,fields),(struct
_dyneither_ptr){_tmp2DD,_tmp2DD,_tmp2DD + 7},(void*)& Cyc__genrep_359};static char
_tmp2DE[7]="tagged";static struct _tuple5 Cyc__gentuple_385={offsetof(struct Cyc_Absyn_AggrdeclImpl,tagged),(
struct _dyneither_ptr){_tmp2DE,_tmp2DE,_tmp2DE + 7},(void*)& Cyc__genrep_10};static
struct _tuple5*Cyc__genarr_386[4]={& Cyc__gentuple_382,& Cyc__gentuple_383,& Cyc__gentuple_384,&
Cyc__gentuple_385};struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_AggrdeclImpl_rep={
3,(struct _dyneither_ptr*)& Cyc__genname_387,sizeof(struct Cyc_Absyn_AggrdeclImpl),{(
void*)((struct _tuple5**)Cyc__genarr_386),(void*)((struct _tuple5**)Cyc__genarr_386),(
void*)((struct _tuple5**)Cyc__genarr_386 + 4)}};static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_358={1,1,(void*)((void*)& Cyc_struct_Absyn_AggrdeclImpl_rep)};static
char _tmp2E1[9]="Aggrdecl";static struct _dyneither_ptr Cyc__genname_399=(struct
_dyneither_ptr){_tmp2E1,_tmp2E1,_tmp2E1 + 9};static char _tmp2E2[5]="kind";static
struct _tuple5 Cyc__gentuple_392={offsetof(struct Cyc_Absyn_Aggrdecl,kind),(struct
_dyneither_ptr){_tmp2E2,_tmp2E2,_tmp2E2 + 5},(void*)& Cyc_Absyn_aggr_kind_t_rep};
static char _tmp2E3[3]="sc";static struct _tuple5 Cyc__gentuple_393={offsetof(struct
Cyc_Absyn_Aggrdecl,sc),(struct _dyneither_ptr){_tmp2E3,_tmp2E3,_tmp2E3 + 3},(void*)&
Cyc_Absyn_scope_t_rep};static char _tmp2E4[5]="name";static struct _tuple5 Cyc__gentuple_394={
offsetof(struct Cyc_Absyn_Aggrdecl,name),(struct _dyneither_ptr){_tmp2E4,_tmp2E4,
_tmp2E4 + 5},(void*)& Cyc__genrep_11};static char _tmp2E5[4]="tvs";static struct
_tuple5 Cyc__gentuple_395={offsetof(struct Cyc_Absyn_Aggrdecl,tvs),(struct
_dyneither_ptr){_tmp2E5,_tmp2E5,_tmp2E5 + 4},(void*)& Cyc__genrep_273};static char
_tmp2E6[5]="impl";static struct _tuple5 Cyc__gentuple_396={offsetof(struct Cyc_Absyn_Aggrdecl,impl),(
struct _dyneither_ptr){_tmp2E6,_tmp2E6,_tmp2E6 + 5},(void*)& Cyc__genrep_358};
static char _tmp2E7[11]="attributes";static struct _tuple5 Cyc__gentuple_397={
offsetof(struct Cyc_Absyn_Aggrdecl,attributes),(struct _dyneither_ptr){_tmp2E7,
_tmp2E7,_tmp2E7 + 11},(void*)& Cyc__genrep_62};static struct _tuple5*Cyc__genarr_398[
6]={& Cyc__gentuple_392,& Cyc__gentuple_393,& Cyc__gentuple_394,& Cyc__gentuple_395,&
Cyc__gentuple_396,& Cyc__gentuple_397};struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Aggrdecl_rep={
3,(struct _dyneither_ptr*)& Cyc__genname_399,sizeof(struct Cyc_Absyn_Aggrdecl),{(
void*)((struct _tuple5**)Cyc__genarr_398),(void*)((struct _tuple5**)Cyc__genarr_398),(
void*)((struct _tuple5**)Cyc__genarr_398 + 6)}};static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_357={1,1,(void*)((void*)& Cyc_struct_Absyn_Aggrdecl_rep)};static
struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_356={1,1,(void*)((void*)& Cyc__genrep_357)};
struct _tuple48{unsigned int f1;struct Cyc_Absyn_Aggrdecl**f2;};static struct _tuple6
Cyc__gentuple_400={offsetof(struct _tuple48,f1),(void*)& Cyc__genrep_24};static
struct _tuple6 Cyc__gentuple_401={offsetof(struct _tuple48,f2),(void*)& Cyc__genrep_356};
static struct _tuple6*Cyc__genarr_402[2]={& Cyc__gentuple_400,& Cyc__gentuple_401};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_355={4,sizeof(struct _tuple48),{(
void*)((struct _tuple6**)Cyc__genarr_402),(void*)((struct _tuple6**)Cyc__genarr_402),(
void*)((struct _tuple6**)Cyc__genarr_402 + 2)}};static struct _tuple7*Cyc__genarr_354[
0]={};static char _tmp2EC[12]="UnknownAggr";static struct _tuple5 Cyc__gentuple_408={
0,(struct _dyneither_ptr){_tmp2EC,_tmp2EC,_tmp2EC + 12},(void*)& Cyc__genrep_403};
static char _tmp2ED[10]="KnownAggr";static struct _tuple5 Cyc__gentuple_409={1,(
struct _dyneither_ptr){_tmp2ED,_tmp2ED,_tmp2ED + 10},(void*)& Cyc__genrep_355};
static struct _tuple5*Cyc__genarr_410[2]={& Cyc__gentuple_408,& Cyc__gentuple_409};
static char _tmp2EF[10]="AggrInfoU";struct Cyc_Typerep_TUnion_struct Cyc_datatype_Absyn_AggrInfoU_rep={
5,(struct _dyneither_ptr){_tmp2EF,_tmp2EF,_tmp2EF + 10},{(void*)((struct _tuple7**)
Cyc__genarr_354),(void*)((struct _tuple7**)Cyc__genarr_354),(void*)((struct
_tuple7**)Cyc__genarr_354 + 0)},{(void*)((struct _tuple5**)Cyc__genarr_410),(void*)((
struct _tuple5**)Cyc__genarr_410),(void*)((struct _tuple5**)Cyc__genarr_410 + 2)}};
static char _tmp2F0[9]="AggrInfo";static struct _dyneither_ptr Cyc__genname_414=(
struct _dyneither_ptr){_tmp2F0,_tmp2F0,_tmp2F0 + 9};static char _tmp2F1[10]="aggr_info";
static struct _tuple5 Cyc__gentuple_411={offsetof(struct Cyc_Absyn_AggrInfo,aggr_info),(
struct _dyneither_ptr){_tmp2F1,_tmp2F1,_tmp2F1 + 10},(void*)& Cyc_datatype_Absyn_AggrInfoU_rep};
static char _tmp2F2[6]="targs";static struct _tuple5 Cyc__gentuple_412={offsetof(
struct Cyc_Absyn_AggrInfo,targs),(struct _dyneither_ptr){_tmp2F2,_tmp2F2,_tmp2F2 + 
6},(void*)& Cyc__genrep_121};static struct _tuple5*Cyc__genarr_413[2]={& Cyc__gentuple_411,&
Cyc__gentuple_412};struct Cyc_Typerep_Struct_struct Cyc_Absyn_aggr_info_t_rep={3,(
struct _dyneither_ptr*)& Cyc__genname_414,sizeof(struct Cyc_Absyn_AggrInfo),{(void*)((
struct _tuple5**)Cyc__genarr_413),(void*)((struct _tuple5**)Cyc__genarr_413),(void*)((
struct _tuple5**)Cyc__genarr_413 + 2)}};extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_331;
extern struct Cyc_Typerep_Struct_struct Cyc_struct_List_List060List_list_t0Absyn_designator_t46H24Absyn_pat_t1_446H2_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_332;extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_333;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_334;extern
struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_designator_t46H2_rep;
extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_designator_t_rep;extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_336;struct _tuple49{unsigned int f1;struct _dyneither_ptr*f2;};static
struct _tuple6 Cyc__gentuple_337={offsetof(struct _tuple49,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_338={offsetof(struct _tuple49,f2),(void*)& Cyc__genrep_13};
static struct _tuple6*Cyc__genarr_339[2]={& Cyc__gentuple_337,& Cyc__gentuple_338};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_336={4,sizeof(struct _tuple49),{(
void*)((struct _tuple6**)Cyc__genarr_339),(void*)((struct _tuple6**)Cyc__genarr_339),(
void*)((struct _tuple6**)Cyc__genarr_339 + 2)}};static struct _tuple7*Cyc__genarr_335[
0]={};static char _tmp2F5[13]="ArrayElement";static struct _tuple5 Cyc__gentuple_340={
0,(struct _dyneither_ptr){_tmp2F5,_tmp2F5,_tmp2F5 + 13},(void*)& Cyc__genrep_129};
static char _tmp2F6[10]="FieldName";static struct _tuple5 Cyc__gentuple_341={1,(
struct _dyneither_ptr){_tmp2F6,_tmp2F6,_tmp2F6 + 10},(void*)& Cyc__genrep_336};
static struct _tuple5*Cyc__genarr_342[2]={& Cyc__gentuple_340,& Cyc__gentuple_341};
static char _tmp2F8[11]="Designator";struct Cyc_Typerep_TUnion_struct Cyc_Absyn_designator_t_rep={
5,(struct _dyneither_ptr){_tmp2F8,_tmp2F8,_tmp2F8 + 11},{(void*)((struct _tuple7**)
Cyc__genarr_335),(void*)((struct _tuple7**)Cyc__genarr_335),(void*)((struct
_tuple7**)Cyc__genarr_335 + 0)},{(void*)((struct _tuple5**)Cyc__genarr_342),(void*)((
struct _tuple5**)Cyc__genarr_342),(void*)((struct _tuple5**)Cyc__genarr_342 + 2)}};
static char _tmp2F9[5]="List";static struct _dyneither_ptr Cyc__genname_346=(struct
_dyneither_ptr){_tmp2F9,_tmp2F9,_tmp2F9 + 5};static char _tmp2FA[3]="hd";static
struct _tuple5 Cyc__gentuple_343={offsetof(struct Cyc_List_List,hd),(struct
_dyneither_ptr){_tmp2FA,_tmp2FA,_tmp2FA + 3},(void*)& Cyc_Absyn_designator_t_rep};
static char _tmp2FB[3]="tl";static struct _tuple5 Cyc__gentuple_344={offsetof(struct
Cyc_List_List,tl),(struct _dyneither_ptr){_tmp2FB,_tmp2FB,_tmp2FB + 3},(void*)& Cyc__genrep_334};
static struct _tuple5*Cyc__genarr_345[2]={& Cyc__gentuple_343,& Cyc__gentuple_344};
struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_designator_t46H2_rep={
3,(struct _dyneither_ptr*)& Cyc__genname_346,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_345),(void*)((struct _tuple5**)Cyc__genarr_345),(void*)((
struct _tuple5**)Cyc__genarr_345 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_334={
1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_designator_t46H2_rep)};struct
_tuple50{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};static struct _tuple6 Cyc__gentuple_347={
offsetof(struct _tuple50,f1),(void*)& Cyc__genrep_334};static struct _tuple6 Cyc__gentuple_348={
offsetof(struct _tuple50,f2),(void*)& Cyc__genrep_181};static struct _tuple6*Cyc__genarr_349[
2]={& Cyc__gentuple_347,& Cyc__gentuple_348};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_333={4,sizeof(struct _tuple50),{(void*)((struct _tuple6**)Cyc__genarr_349),(
void*)((struct _tuple6**)Cyc__genarr_349),(void*)((struct _tuple6**)Cyc__genarr_349
+ 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_332={1,1,(void*)((void*)&
Cyc__genrep_333)};static char _tmp300[5]="List";static struct _dyneither_ptr Cyc__genname_353=(
struct _dyneither_ptr){_tmp300,_tmp300,_tmp300 + 5};static char _tmp301[3]="hd";
static struct _tuple5 Cyc__gentuple_350={offsetof(struct Cyc_List_List,hd),(struct
_dyneither_ptr){_tmp301,_tmp301,_tmp301 + 3},(void*)& Cyc__genrep_332};static char
_tmp302[3]="tl";static struct _tuple5 Cyc__gentuple_351={offsetof(struct Cyc_List_List,tl),(
struct _dyneither_ptr){_tmp302,_tmp302,_tmp302 + 3},(void*)& Cyc__genrep_331};
static struct _tuple5*Cyc__genarr_352[2]={& Cyc__gentuple_350,& Cyc__gentuple_351};
struct Cyc_Typerep_Struct_struct Cyc_struct_List_List060List_list_t0Absyn_designator_t46H24Absyn_pat_t1_446H2_rep={
3,(struct _dyneither_ptr*)& Cyc__genname_353,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_352),(void*)((struct _tuple5**)Cyc__genarr_352),(void*)((
struct _tuple5**)Cyc__genarr_352 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_331={
1,1,(void*)((void*)& Cyc_struct_List_List060List_list_t0Absyn_designator_t46H24Absyn_pat_t1_446H2_rep)};
struct _tuple51{unsigned int f1;struct Cyc_Absyn_AggrInfo f2;struct Cyc_List_List*f3;
struct Cyc_List_List*f4;int f5;};static struct _tuple6 Cyc__gentuple_415={offsetof(
struct _tuple51,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_416={
offsetof(struct _tuple51,f2),(void*)& Cyc_Absyn_aggr_info_t_rep};static struct
_tuple6 Cyc__gentuple_417={offsetof(struct _tuple51,f3),(void*)& Cyc__genrep_273};
static struct _tuple6 Cyc__gentuple_418={offsetof(struct _tuple51,f4),(void*)& Cyc__genrep_331};
static struct _tuple6 Cyc__gentuple_419={offsetof(struct _tuple51,f5),(void*)& Cyc__genrep_10};
static struct _tuple6*Cyc__genarr_420[5]={& Cyc__gentuple_415,& Cyc__gentuple_416,&
Cyc__gentuple_417,& Cyc__gentuple_418,& Cyc__gentuple_419};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_330={4,sizeof(struct _tuple51),{(void*)((struct _tuple6**)Cyc__genarr_420),(
void*)((struct _tuple6**)Cyc__genarr_420),(void*)((struct _tuple6**)Cyc__genarr_420
+ 5)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_245;struct _tuple52{
unsigned int f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;
struct Cyc_List_List*f4;int f5;};static struct _tuple6 Cyc__gentuple_324={offsetof(
struct _tuple52,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_325={
offsetof(struct _tuple52,f2),(void*)((void*)& Cyc__genrep_263)};static struct
_tuple6 Cyc__gentuple_326={offsetof(struct _tuple52,f3),(void*)& Cyc__genrep_246};
static struct _tuple6 Cyc__gentuple_327={offsetof(struct _tuple52,f4),(void*)& Cyc__genrep_186};
static struct _tuple6 Cyc__gentuple_328={offsetof(struct _tuple52,f5),(void*)& Cyc__genrep_10};
static struct _tuple6*Cyc__genarr_329[5]={& Cyc__gentuple_324,& Cyc__gentuple_325,&
Cyc__gentuple_326,& Cyc__gentuple_327,& Cyc__gentuple_328};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_245={4,sizeof(struct _tuple52),{(void*)((struct _tuple6**)Cyc__genarr_329),(
void*)((struct _tuple6**)Cyc__genarr_329),(void*)((struct _tuple6**)Cyc__genarr_329
+ 5)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_231;struct _tuple53{
unsigned int f1;char f2;};static struct _tuple6 Cyc__gentuple_232={offsetof(struct
_tuple53,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_233={
offsetof(struct _tuple53,f2),(void*)((void*)& Cyc__genrep_15)};static struct _tuple6*
Cyc__genarr_234[2]={& Cyc__gentuple_232,& Cyc__gentuple_233};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_231={4,sizeof(struct _tuple53),{(void*)((struct _tuple6**)Cyc__genarr_234),(
void*)((struct _tuple6**)Cyc__genarr_234),(void*)((struct _tuple6**)Cyc__genarr_234
+ 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_211;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_212;extern struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Enumdecl_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_213;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Core_Opt0List_list_t0Absyn_enumfield_t46H22_rep;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_214;extern struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_enumfield_t46H2_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_201;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_Enumfield_rep;static char _tmp308[10]="Enumfield";static struct
_dyneither_ptr Cyc__genname_206=(struct _dyneither_ptr){_tmp308,_tmp308,_tmp308 + 
10};static char _tmp309[5]="name";static struct _tuple5 Cyc__gentuple_202={offsetof(
struct Cyc_Absyn_Enumfield,name),(struct _dyneither_ptr){_tmp309,_tmp309,_tmp309 + 
5},(void*)& Cyc__genrep_11};static char _tmp30A[4]="tag";static struct _tuple5 Cyc__gentuple_203={
offsetof(struct Cyc_Absyn_Enumfield,tag),(struct _dyneither_ptr){_tmp30A,_tmp30A,
_tmp30A + 4},(void*)& Cyc__genrep_145};static char _tmp30B[4]="loc";static struct
_tuple5 Cyc__gentuple_204={offsetof(struct Cyc_Absyn_Enumfield,loc),(struct
_dyneither_ptr){_tmp30B,_tmp30B,_tmp30B + 4},(void*)& Cyc__genrep_2};static struct
_tuple5*Cyc__genarr_205[3]={& Cyc__gentuple_202,& Cyc__gentuple_203,& Cyc__gentuple_204};
struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Enumfield_rep={3,(struct
_dyneither_ptr*)& Cyc__genname_206,sizeof(struct Cyc_Absyn_Enumfield),{(void*)((
struct _tuple5**)Cyc__genarr_205),(void*)((struct _tuple5**)Cyc__genarr_205),(void*)((
struct _tuple5**)Cyc__genarr_205 + 3)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_201={
1,1,(void*)((void*)& Cyc_struct_Absyn_Enumfield_rep)};static char _tmp30E[5]="List";
static struct _dyneither_ptr Cyc__genname_218=(struct _dyneither_ptr){_tmp30E,
_tmp30E,_tmp30E + 5};static char _tmp30F[3]="hd";static struct _tuple5 Cyc__gentuple_215={
offsetof(struct Cyc_List_List,hd),(struct _dyneither_ptr){_tmp30F,_tmp30F,_tmp30F + 
3},(void*)& Cyc__genrep_201};static char _tmp310[3]="tl";static struct _tuple5 Cyc__gentuple_216={
offsetof(struct Cyc_List_List,tl),(struct _dyneither_ptr){_tmp310,_tmp310,_tmp310 + 
3},(void*)& Cyc__genrep_214};static struct _tuple5*Cyc__genarr_217[2]={& Cyc__gentuple_215,&
Cyc__gentuple_216};struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_enumfield_t46H2_rep={
3,(struct _dyneither_ptr*)& Cyc__genname_218,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_217),(void*)((struct _tuple5**)Cyc__genarr_217),(void*)((
struct _tuple5**)Cyc__genarr_217 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_214={
1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_enumfield_t46H2_rep)};static char
_tmp313[4]="Opt";static struct _dyneither_ptr Cyc__genname_221=(struct
_dyneither_ptr){_tmp313,_tmp313,_tmp313 + 4};static char _tmp314[2]="v";static
struct _tuple5 Cyc__gentuple_219={offsetof(struct Cyc_Core_Opt,v),(struct
_dyneither_ptr){_tmp314,_tmp314,_tmp314 + 2},(void*)& Cyc__genrep_214};static
struct _tuple5*Cyc__genarr_220[1]={& Cyc__gentuple_219};struct Cyc_Typerep_Struct_struct
Cyc_struct_Core_Opt0List_list_t0Absyn_enumfield_t46H22_rep={3,(struct
_dyneither_ptr*)& Cyc__genname_221,sizeof(struct Cyc_Core_Opt),{(void*)((struct
_tuple5**)Cyc__genarr_220),(void*)((struct _tuple5**)Cyc__genarr_220),(void*)((
struct _tuple5**)Cyc__genarr_220 + 1)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_213={
1,1,(void*)((void*)& Cyc_struct_Core_Opt0List_list_t0Absyn_enumfield_t46H22_rep)};
static char _tmp317[9]="Enumdecl";static struct _dyneither_ptr Cyc__genname_226=(
struct _dyneither_ptr){_tmp317,_tmp317,_tmp317 + 9};static char _tmp318[3]="sc";
static struct _tuple5 Cyc__gentuple_222={offsetof(struct Cyc_Absyn_Enumdecl,sc),(
struct _dyneither_ptr){_tmp318,_tmp318,_tmp318 + 3},(void*)& Cyc_Absyn_scope_t_rep};
static char _tmp319[5]="name";static struct _tuple5 Cyc__gentuple_223={offsetof(
struct Cyc_Absyn_Enumdecl,name),(struct _dyneither_ptr){_tmp319,_tmp319,_tmp319 + 5},(
void*)& Cyc__genrep_11};static char _tmp31A[7]="fields";static struct _tuple5 Cyc__gentuple_224={
offsetof(struct Cyc_Absyn_Enumdecl,fields),(struct _dyneither_ptr){_tmp31A,_tmp31A,
_tmp31A + 7},(void*)& Cyc__genrep_213};static struct _tuple5*Cyc__genarr_225[3]={&
Cyc__gentuple_222,& Cyc__gentuple_223,& Cyc__gentuple_224};struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_Enumdecl_rep={3,(struct _dyneither_ptr*)& Cyc__genname_226,
sizeof(struct Cyc_Absyn_Enumdecl),{(void*)((struct _tuple5**)Cyc__genarr_225),(
void*)((struct _tuple5**)Cyc__genarr_225),(void*)((struct _tuple5**)Cyc__genarr_225
+ 3)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_212={1,1,(void*)((void*)&
Cyc_struct_Absyn_Enumdecl_rep)};struct _tuple54{unsigned int f1;struct Cyc_Absyn_Enumdecl*
f2;struct Cyc_Absyn_Enumfield*f3;};static struct _tuple6 Cyc__gentuple_227={
offsetof(struct _tuple54,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_228={
offsetof(struct _tuple54,f2),(void*)& Cyc__genrep_212};static struct _tuple6 Cyc__gentuple_229={
offsetof(struct _tuple54,f3),(void*)& Cyc__genrep_201};static struct _tuple6*Cyc__genarr_230[
3]={& Cyc__gentuple_227,& Cyc__gentuple_228,& Cyc__gentuple_229};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_211={4,sizeof(struct _tuple54),{(void*)((struct _tuple6**)Cyc__genarr_230),(
void*)((struct _tuple6**)Cyc__genarr_230),(void*)((struct _tuple6**)Cyc__genarr_230
+ 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_200;struct _tuple55{
unsigned int f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};static struct _tuple6 Cyc__gentuple_207={
offsetof(struct _tuple55,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_208={
offsetof(struct _tuple55,f2),(void*)((void*)& Cyc_Absyn_type_t_rep)};static struct
_tuple6 Cyc__gentuple_209={offsetof(struct _tuple55,f3),(void*)& Cyc__genrep_201};
static struct _tuple6*Cyc__genarr_210[3]={& Cyc__gentuple_207,& Cyc__gentuple_208,&
Cyc__gentuple_209};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_200={4,
sizeof(struct _tuple55),{(void*)((struct _tuple6**)Cyc__genarr_210),(void*)((
struct _tuple6**)Cyc__genarr_210),(void*)((struct _tuple6**)Cyc__genarr_210 + 3)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_196;struct _tuple56{unsigned int
f1;struct _tuple0*f2;};static struct _tuple6 Cyc__gentuple_197={offsetof(struct
_tuple56,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_198={
offsetof(struct _tuple56,f2),(void*)& Cyc__genrep_11};static struct _tuple6*Cyc__genarr_199[
2]={& Cyc__gentuple_197,& Cyc__gentuple_198};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_196={4,sizeof(struct _tuple56),{(void*)((struct _tuple6**)Cyc__genarr_199),(
void*)((struct _tuple6**)Cyc__genarr_199),(void*)((struct _tuple6**)Cyc__genarr_199
+ 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_185;struct _tuple57{
unsigned int f1;struct _tuple0*f2;struct Cyc_List_List*f3;int f4;};static struct
_tuple6 Cyc__gentuple_191={offsetof(struct _tuple57,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_192={offsetof(struct _tuple57,f2),(void*)& Cyc__genrep_11};
static struct _tuple6 Cyc__gentuple_193={offsetof(struct _tuple57,f3),(void*)& Cyc__genrep_186};
static struct _tuple6 Cyc__gentuple_194={offsetof(struct _tuple57,f4),(void*)& Cyc__genrep_10};
static struct _tuple6*Cyc__genarr_195[4]={& Cyc__gentuple_191,& Cyc__gentuple_192,&
Cyc__gentuple_193,& Cyc__gentuple_194};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_185={
4,sizeof(struct _tuple57),{(void*)((struct _tuple6**)Cyc__genarr_195),(void*)((
struct _tuple6**)Cyc__genarr_195),(void*)((struct _tuple6**)Cyc__genarr_195 + 4)}};
static char _tmp321[7]="Wild_p";static struct _tuple7 Cyc__gentuple_182={0,(struct
_dyneither_ptr){_tmp321,_tmp321,_tmp321 + 7}};static char _tmp322[7]="Null_p";
static struct _tuple7 Cyc__gentuple_183={1,(struct _dyneither_ptr){_tmp322,_tmp322,
_tmp322 + 7}};static struct _tuple7*Cyc__genarr_184[2]={& Cyc__gentuple_182,& Cyc__gentuple_183};
static char _tmp323[6]="Var_p";static struct _tuple5 Cyc__gentuple_440={0,(struct
_dyneither_ptr){_tmp323,_tmp323,_tmp323 + 6},(void*)& Cyc__genrep_435};static char
_tmp324[12]="Reference_p";static struct _tuple5 Cyc__gentuple_441={1,(struct
_dyneither_ptr){_tmp324,_tmp324,_tmp324 + 12},(void*)& Cyc__genrep_435};static char
_tmp325[9]="TagInt_p";static struct _tuple5 Cyc__gentuple_442={2,(struct
_dyneither_ptr){_tmp325,_tmp325,_tmp325 + 9},(void*)& Cyc__genrep_430};static char
_tmp326[8]="Tuple_p";static struct _tuple5 Cyc__gentuple_443={3,(struct
_dyneither_ptr){_tmp326,_tmp326,_tmp326 + 8},(void*)& Cyc__genrep_425};static char
_tmp327[10]="Pointer_p";static struct _tuple5 Cyc__gentuple_444={4,(struct
_dyneither_ptr){_tmp327,_tmp327,_tmp327 + 10},(void*)& Cyc__genrep_421};static char
_tmp328[7]="Aggr_p";static struct _tuple5 Cyc__gentuple_445={5,(struct
_dyneither_ptr){_tmp328,_tmp328,_tmp328 + 7},(void*)& Cyc__genrep_330};static char
_tmp329[11]="Datatype_p";static struct _tuple5 Cyc__gentuple_446={6,(struct
_dyneither_ptr){_tmp329,_tmp329,_tmp329 + 11},(void*)& Cyc__genrep_245};static char
_tmp32A[6]="Int_p";static struct _tuple5 Cyc__gentuple_447={7,(struct _dyneither_ptr){
_tmp32A,_tmp32A,_tmp32A + 6},(void*)& Cyc__genrep_235};static char _tmp32B[7]="Char_p";
static struct _tuple5 Cyc__gentuple_448={8,(struct _dyneither_ptr){_tmp32B,_tmp32B,
_tmp32B + 7},(void*)& Cyc__genrep_231};static char _tmp32C[8]="Float_p";static struct
_tuple5 Cyc__gentuple_449={9,(struct _dyneither_ptr){_tmp32C,_tmp32C,_tmp32C + 8},(
void*)& Cyc__genrep_81};static char _tmp32D[7]="Enum_p";static struct _tuple5 Cyc__gentuple_450={
10,(struct _dyneither_ptr){_tmp32D,_tmp32D,_tmp32D + 7},(void*)& Cyc__genrep_211};
static char _tmp32E[11]="AnonEnum_p";static struct _tuple5 Cyc__gentuple_451={11,(
struct _dyneither_ptr){_tmp32E,_tmp32E,_tmp32E + 11},(void*)& Cyc__genrep_200};
static char _tmp32F[12]="UnknownId_p";static struct _tuple5 Cyc__gentuple_452={12,(
struct _dyneither_ptr){_tmp32F,_tmp32F,_tmp32F + 12},(void*)& Cyc__genrep_196};
static char _tmp330[14]="UnknownCall_p";static struct _tuple5 Cyc__gentuple_453={13,(
struct _dyneither_ptr){_tmp330,_tmp330,_tmp330 + 14},(void*)& Cyc__genrep_185};
static char _tmp331[6]="Exp_p";static struct _tuple5 Cyc__gentuple_454={14,(struct
_dyneither_ptr){_tmp331,_tmp331,_tmp331 + 6},(void*)& Cyc__genrep_129};static
struct _tuple5*Cyc__genarr_455[15]={& Cyc__gentuple_440,& Cyc__gentuple_441,& Cyc__gentuple_442,&
Cyc__gentuple_443,& Cyc__gentuple_444,& Cyc__gentuple_445,& Cyc__gentuple_446,& Cyc__gentuple_447,&
Cyc__gentuple_448,& Cyc__gentuple_449,& Cyc__gentuple_450,& Cyc__gentuple_451,& Cyc__gentuple_452,&
Cyc__gentuple_453,& Cyc__gentuple_454};static char _tmp333[8]="Raw_pat";struct Cyc_Typerep_TUnion_struct
Cyc_Absyn_raw_pat_t_rep={5,(struct _dyneither_ptr){_tmp333,_tmp333,_tmp333 + 8},{(
void*)((struct _tuple7**)Cyc__genarr_184),(void*)((struct _tuple7**)Cyc__genarr_184),(
void*)((struct _tuple7**)Cyc__genarr_184 + 2)},{(void*)((struct _tuple5**)Cyc__genarr_455),(
void*)((struct _tuple5**)Cyc__genarr_455),(void*)((struct _tuple5**)Cyc__genarr_455
+ 15)}};static char _tmp334[4]="Pat";static struct _dyneither_ptr Cyc__genname_460=(
struct _dyneither_ptr){_tmp334,_tmp334,_tmp334 + 4};static char _tmp335[2]="r";
static struct _tuple5 Cyc__gentuple_456={offsetof(struct Cyc_Absyn_Pat,r),(struct
_dyneither_ptr){_tmp335,_tmp335,_tmp335 + 2},(void*)& Cyc_Absyn_raw_pat_t_rep};
static char _tmp336[5]="topt";static struct _tuple5 Cyc__gentuple_457={offsetof(
struct Cyc_Absyn_Pat,topt),(struct _dyneither_ptr){_tmp336,_tmp336,_tmp336 + 5},(
void*)& Cyc__genrep_110};static char _tmp337[4]="loc";static struct _tuple5 Cyc__gentuple_458={
offsetof(struct Cyc_Absyn_Pat,loc),(struct _dyneither_ptr){_tmp337,_tmp337,_tmp337
+ 4},(void*)& Cyc__genrep_2};static struct _tuple5*Cyc__genarr_459[3]={& Cyc__gentuple_456,&
Cyc__gentuple_457,& Cyc__gentuple_458};struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Pat_rep={
3,(struct _dyneither_ptr*)& Cyc__genname_460,sizeof(struct Cyc_Absyn_Pat),{(void*)((
struct _tuple5**)Cyc__genarr_459),(void*)((struct _tuple5**)Cyc__genarr_459),(void*)((
struct _tuple5**)Cyc__genarr_459 + 3)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_181={
1,1,(void*)((void*)& Cyc_struct_Absyn_Pat_rep)};extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_146;extern struct Cyc_Typerep_Struct_struct Cyc_struct_Core_Opt0List_list_t0Absyn_vardecl_t46H22_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_147;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List0Absyn_vardecl_t46H2_rep;static char _tmp33A[5]="List";static
struct _dyneither_ptr Cyc__genname_177=(struct _dyneither_ptr){_tmp33A,_tmp33A,
_tmp33A + 5};static char _tmp33B[3]="hd";static struct _tuple5 Cyc__gentuple_174={
offsetof(struct Cyc_List_List,hd),(struct _dyneither_ptr){_tmp33B,_tmp33B,_tmp33B + 
3},(void*)& Cyc__genrep_148};static char _tmp33C[3]="tl";static struct _tuple5 Cyc__gentuple_175={
offsetof(struct Cyc_List_List,tl),(struct _dyneither_ptr){_tmp33C,_tmp33C,_tmp33C + 
3},(void*)& Cyc__genrep_147};static struct _tuple5*Cyc__genarr_176[2]={& Cyc__gentuple_174,&
Cyc__gentuple_175};struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_vardecl_t46H2_rep={
3,(struct _dyneither_ptr*)& Cyc__genname_177,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_176),(void*)((struct _tuple5**)Cyc__genarr_176),(void*)((
struct _tuple5**)Cyc__genarr_176 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_147={
1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_vardecl_t46H2_rep)};static char
_tmp33F[4]="Opt";static struct _dyneither_ptr Cyc__genname_180=(struct
_dyneither_ptr){_tmp33F,_tmp33F,_tmp33F + 4};static char _tmp340[2]="v";static
struct _tuple5 Cyc__gentuple_178={offsetof(struct Cyc_Core_Opt,v),(struct
_dyneither_ptr){_tmp340,_tmp340,_tmp340 + 2},(void*)& Cyc__genrep_147};static
struct _tuple5*Cyc__genarr_179[1]={& Cyc__gentuple_178};struct Cyc_Typerep_Struct_struct
Cyc_struct_Core_Opt0List_list_t0Absyn_vardecl_t46H22_rep={3,(struct
_dyneither_ptr*)& Cyc__genname_180,sizeof(struct Cyc_Core_Opt),{(void*)((struct
_tuple5**)Cyc__genarr_179),(void*)((struct _tuple5**)Cyc__genarr_179),(void*)((
struct _tuple5**)Cyc__genarr_179 + 1)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_146={
1,1,(void*)((void*)& Cyc_struct_Core_Opt0List_list_t0Absyn_vardecl_t46H22_rep)};
static char _tmp343[14]="Switch_clause";static struct _dyneither_ptr Cyc__genname_467=(
struct _dyneither_ptr){_tmp343,_tmp343,_tmp343 + 14};static char _tmp344[8]="pattern";
static struct _tuple5 Cyc__gentuple_461={offsetof(struct Cyc_Absyn_Switch_clause,pattern),(
struct _dyneither_ptr){_tmp344,_tmp344,_tmp344 + 8},(void*)& Cyc__genrep_181};
static char _tmp345[9]="pat_vars";static struct _tuple5 Cyc__gentuple_462={offsetof(
struct Cyc_Absyn_Switch_clause,pat_vars),(struct _dyneither_ptr){_tmp345,_tmp345,
_tmp345 + 9},(void*)& Cyc__genrep_146};static char _tmp346[13]="where_clause";static
struct _tuple5 Cyc__gentuple_463={offsetof(struct Cyc_Absyn_Switch_clause,where_clause),(
struct _dyneither_ptr){_tmp346,_tmp346,_tmp346 + 13},(void*)& Cyc__genrep_145};
static char _tmp347[5]="body";static struct _tuple5 Cyc__gentuple_464={offsetof(
struct Cyc_Absyn_Switch_clause,body),(struct _dyneither_ptr){_tmp347,_tmp347,
_tmp347 + 5},(void*)& Cyc__genrep_134};static char _tmp348[4]="loc";static struct
_tuple5 Cyc__gentuple_465={offsetof(struct Cyc_Absyn_Switch_clause,loc),(struct
_dyneither_ptr){_tmp348,_tmp348,_tmp348 + 4},(void*)& Cyc__genrep_2};static struct
_tuple5*Cyc__genarr_466[5]={& Cyc__gentuple_461,& Cyc__gentuple_462,& Cyc__gentuple_463,&
Cyc__gentuple_464,& Cyc__gentuple_465};struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Switch_clause_rep={
3,(struct _dyneither_ptr*)& Cyc__genname_467,sizeof(struct Cyc_Absyn_Switch_clause),{(
void*)((struct _tuple5**)Cyc__genarr_466),(void*)((struct _tuple5**)Cyc__genarr_466),(
void*)((struct _tuple5**)Cyc__genarr_466 + 5)}};static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_144={1,1,(void*)((void*)& Cyc_struct_Absyn_Switch_clause_rep)};static
char _tmp34B[5]="List";static struct _dyneither_ptr Cyc__genname_471=(struct
_dyneither_ptr){_tmp34B,_tmp34B,_tmp34B + 5};static char _tmp34C[3]="hd";static
struct _tuple5 Cyc__gentuple_468={offsetof(struct Cyc_List_List,hd),(struct
_dyneither_ptr){_tmp34C,_tmp34C,_tmp34C + 3},(void*)((void*)& Cyc__genrep_144)};
static char _tmp34D[3]="tl";static struct _tuple5 Cyc__gentuple_469={offsetof(struct
Cyc_List_List,tl),(struct _dyneither_ptr){_tmp34D,_tmp34D,_tmp34D + 3},(void*)& Cyc__genrep_143};
static struct _tuple5*Cyc__genarr_470[2]={& Cyc__gentuple_468,& Cyc__gentuple_469};
struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_switch_clause_t46H2_rep={
3,(struct _dyneither_ptr*)& Cyc__genname_471,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_470),(void*)((struct _tuple5**)Cyc__genarr_470),(void*)((
struct _tuple5**)Cyc__genarr_470 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_143={
1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_switch_clause_t46H2_rep)};struct
_tuple58{unsigned int f1;struct Cyc_Absyn_Exp*f2;struct Cyc_List_List*f3;};static
struct _tuple6 Cyc__gentuple_507={offsetof(struct _tuple58,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_508={offsetof(struct _tuple58,f2),(void*)& Cyc__genrep_130};
static struct _tuple6 Cyc__gentuple_509={offsetof(struct _tuple58,f3),(void*)& Cyc__genrep_143};
static struct _tuple6*Cyc__genarr_510[3]={& Cyc__gentuple_507,& Cyc__gentuple_508,&
Cyc__gentuple_509};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_506={4,
sizeof(struct _tuple58),{(void*)((struct _tuple6**)Cyc__genarr_510),(void*)((
struct _tuple6**)Cyc__genarr_510),(void*)((struct _tuple6**)Cyc__genarr_510 + 3)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_495;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_497;extern struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_exp_t46H2_rep;
static char _tmp351[5]="List";static struct _dyneither_ptr Cyc__genname_501=(struct
_dyneither_ptr){_tmp351,_tmp351,_tmp351 + 5};static char _tmp352[3]="hd";static
struct _tuple5 Cyc__gentuple_498={offsetof(struct Cyc_List_List,hd),(struct
_dyneither_ptr){_tmp352,_tmp352,_tmp352 + 3},(void*)& Cyc__genrep_130};static char
_tmp353[3]="tl";static struct _tuple5 Cyc__gentuple_499={offsetof(struct Cyc_List_List,tl),(
struct _dyneither_ptr){_tmp353,_tmp353,_tmp353 + 3},(void*)& Cyc__genrep_497};
static struct _tuple5*Cyc__genarr_500[2]={& Cyc__gentuple_498,& Cyc__gentuple_499};
struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_exp_t46H2_rep={3,(
struct _dyneither_ptr*)& Cyc__genname_501,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_500),(void*)((struct _tuple5**)Cyc__genarr_500),(void*)((
struct _tuple5**)Cyc__genarr_500 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_497={
1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_exp_t46H2_rep)};extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_496;static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_496={1,1,(void*)((
void*)& Cyc__genrep_144)};struct _tuple59{unsigned int f1;struct Cyc_List_List*f2;
struct Cyc_Absyn_Switch_clause**f3;};static struct _tuple6 Cyc__gentuple_502={
offsetof(struct _tuple59,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_503={
offsetof(struct _tuple59,f2),(void*)& Cyc__genrep_497};static struct _tuple6 Cyc__gentuple_504={
offsetof(struct _tuple59,f3),(void*)& Cyc__genrep_496};static struct _tuple6*Cyc__genarr_505[
3]={& Cyc__gentuple_502,& Cyc__gentuple_503,& Cyc__gentuple_504};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_495={4,sizeof(struct _tuple59),{(void*)((struct _tuple6**)Cyc__genarr_505),(
void*)((struct _tuple6**)Cyc__genarr_505),(void*)((struct _tuple6**)Cyc__genarr_505
+ 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_490;struct _tuple60{
unsigned int f1;struct Cyc_Absyn_Decl*f2;struct Cyc_Absyn_Stmt*f3;};static struct
_tuple6 Cyc__gentuple_491={offsetof(struct _tuple60,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_492={offsetof(struct _tuple60,f2),(void*)& Cyc__genrep_1};
static struct _tuple6 Cyc__gentuple_493={offsetof(struct _tuple60,f3),(void*)& Cyc__genrep_134};
static struct _tuple6*Cyc__genarr_494[3]={& Cyc__gentuple_491,& Cyc__gentuple_492,&
Cyc__gentuple_493};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_490={4,
sizeof(struct _tuple60),{(void*)((struct _tuple6**)Cyc__genarr_494),(void*)((
struct _tuple6**)Cyc__genarr_494),(void*)((struct _tuple6**)Cyc__genarr_494 + 3)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_485;static struct _tuple6 Cyc__gentuple_486={
offsetof(struct _tuple40,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_487={
offsetof(struct _tuple40,f2),(void*)& Cyc__genrep_13};static struct _tuple6 Cyc__gentuple_488={
offsetof(struct _tuple40,f3),(void*)& Cyc__genrep_134};static struct _tuple6*Cyc__genarr_489[
3]={& Cyc__gentuple_486,& Cyc__gentuple_487,& Cyc__gentuple_488};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_485={4,sizeof(struct _tuple40),{(void*)((struct _tuple6**)Cyc__genarr_489),(
void*)((struct _tuple6**)Cyc__genarr_489),(void*)((struct _tuple6**)Cyc__genarr_489
+ 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_476;struct _tuple61{
unsigned int f1;struct Cyc_Absyn_Stmt*f2;struct _tuple2 f3;};static struct _tuple6 Cyc__gentuple_481={
offsetof(struct _tuple61,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_482={
offsetof(struct _tuple61,f2),(void*)& Cyc__genrep_134};static struct _tuple6 Cyc__gentuple_483={
offsetof(struct _tuple61,f3),(void*)& Cyc__genrep_477};static struct _tuple6*Cyc__genarr_484[
3]={& Cyc__gentuple_481,& Cyc__gentuple_482,& Cyc__gentuple_483};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_476={4,sizeof(struct _tuple61),{(void*)((struct _tuple6**)Cyc__genarr_484),(
void*)((struct _tuple6**)Cyc__genarr_484),(void*)((struct _tuple6**)Cyc__genarr_484
+ 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_142;struct _tuple62{
unsigned int f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_List_List*f3;};static struct
_tuple6 Cyc__gentuple_472={offsetof(struct _tuple62,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_473={offsetof(struct _tuple62,f2),(void*)& Cyc__genrep_134};
static struct _tuple6 Cyc__gentuple_474={offsetof(struct _tuple62,f3),(void*)& Cyc__genrep_143};
static struct _tuple6*Cyc__genarr_475[3]={& Cyc__gentuple_472,& Cyc__gentuple_473,&
Cyc__gentuple_474};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_142={4,
sizeof(struct _tuple62),{(void*)((struct _tuple6**)Cyc__genarr_475),(void*)((
struct _tuple6**)Cyc__genarr_475),(void*)((struct _tuple6**)Cyc__genarr_475 + 3)}};
static char _tmp35C[7]="Skip_s";static struct _tuple7 Cyc__gentuple_140={0,(struct
_dyneither_ptr){_tmp35C,_tmp35C,_tmp35C + 7}};static struct _tuple7*Cyc__genarr_141[
1]={& Cyc__gentuple_140};static char _tmp35D[6]="Exp_s";static struct _tuple5 Cyc__gentuple_548={
0,(struct _dyneither_ptr){_tmp35D,_tmp35D,_tmp35D + 6},(void*)& Cyc__genrep_129};
static char _tmp35E[6]="Seq_s";static struct _tuple5 Cyc__gentuple_549={1,(struct
_dyneither_ptr){_tmp35E,_tmp35E,_tmp35E + 6},(void*)& Cyc__genrep_543};static char
_tmp35F[9]="Return_s";static struct _tuple5 Cyc__gentuple_550={2,(struct
_dyneither_ptr){_tmp35F,_tmp35F,_tmp35F + 9},(void*)& Cyc__genrep_539};static char
_tmp360[13]="IfThenElse_s";static struct _tuple5 Cyc__gentuple_551={3,(struct
_dyneither_ptr){_tmp360,_tmp360,_tmp360 + 13},(void*)& Cyc__genrep_533};static char
_tmp361[8]="While_s";static struct _tuple5 Cyc__gentuple_552={4,(struct
_dyneither_ptr){_tmp361,_tmp361,_tmp361 + 8},(void*)& Cyc__genrep_528};static char
_tmp362[8]="Break_s";static struct _tuple5 Cyc__gentuple_553={5,(struct
_dyneither_ptr){_tmp362,_tmp362,_tmp362 + 8},(void*)& Cyc__genrep_524};static char
_tmp363[11]="Continue_s";static struct _tuple5 Cyc__gentuple_554={6,(struct
_dyneither_ptr){_tmp363,_tmp363,_tmp363 + 11},(void*)& Cyc__genrep_524};static char
_tmp364[7]="Goto_s";static struct _tuple5 Cyc__gentuple_555={7,(struct
_dyneither_ptr){_tmp364,_tmp364,_tmp364 + 7},(void*)& Cyc__genrep_518};static char
_tmp365[6]="For_s";static struct _tuple5 Cyc__gentuple_556={8,(struct _dyneither_ptr){
_tmp365,_tmp365,_tmp365 + 6},(void*)& Cyc__genrep_511};static char _tmp366[9]="Switch_s";
static struct _tuple5 Cyc__gentuple_557={9,(struct _dyneither_ptr){_tmp366,_tmp366,
_tmp366 + 9},(void*)& Cyc__genrep_506};static char _tmp367[11]="Fallthru_s";static
struct _tuple5 Cyc__gentuple_558={10,(struct _dyneither_ptr){_tmp367,_tmp367,
_tmp367 + 11},(void*)& Cyc__genrep_495};static char _tmp368[7]="Decl_s";static struct
_tuple5 Cyc__gentuple_559={11,(struct _dyneither_ptr){_tmp368,_tmp368,_tmp368 + 7},(
void*)& Cyc__genrep_490};static char _tmp369[8]="Label_s";static struct _tuple5 Cyc__gentuple_560={
12,(struct _dyneither_ptr){_tmp369,_tmp369,_tmp369 + 8},(void*)& Cyc__genrep_485};
static char _tmp36A[5]="Do_s";static struct _tuple5 Cyc__gentuple_561={13,(struct
_dyneither_ptr){_tmp36A,_tmp36A,_tmp36A + 5},(void*)& Cyc__genrep_476};static char
_tmp36B[11]="TryCatch_s";static struct _tuple5 Cyc__gentuple_562={14,(struct
_dyneither_ptr){_tmp36B,_tmp36B,_tmp36B + 11},(void*)& Cyc__genrep_142};static char
_tmp36C[14]="ResetRegion_s";static struct _tuple5 Cyc__gentuple_563={15,(struct
_dyneither_ptr){_tmp36C,_tmp36C,_tmp36C + 14},(void*)& Cyc__genrep_129};static
struct _tuple5*Cyc__genarr_564[16]={& Cyc__gentuple_548,& Cyc__gentuple_549,& Cyc__gentuple_550,&
Cyc__gentuple_551,& Cyc__gentuple_552,& Cyc__gentuple_553,& Cyc__gentuple_554,& Cyc__gentuple_555,&
Cyc__gentuple_556,& Cyc__gentuple_557,& Cyc__gentuple_558,& Cyc__gentuple_559,& Cyc__gentuple_560,&
Cyc__gentuple_561,& Cyc__gentuple_562,& Cyc__gentuple_563};static char _tmp36E[9]="Raw_stmt";
struct Cyc_Typerep_TUnion_struct Cyc_Absyn_raw_stmt_t_rep={5,(struct _dyneither_ptr){
_tmp36E,_tmp36E,_tmp36E + 9},{(void*)((struct _tuple7**)Cyc__genarr_141),(void*)((
struct _tuple7**)Cyc__genarr_141),(void*)((struct _tuple7**)Cyc__genarr_141 + 1)},{(
void*)((struct _tuple5**)Cyc__genarr_564),(void*)((struct _tuple5**)Cyc__genarr_564),(
void*)((struct _tuple5**)Cyc__genarr_564 + 16)}};extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_135;extern struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_stmt_t46H2_rep;
static char _tmp36F[5]="List";static struct _dyneither_ptr Cyc__genname_139=(struct
_dyneither_ptr){_tmp36F,_tmp36F,_tmp36F + 5};static char _tmp370[3]="hd";static
struct _tuple5 Cyc__gentuple_136={offsetof(struct Cyc_List_List,hd),(struct
_dyneither_ptr){_tmp370,_tmp370,_tmp370 + 3},(void*)& Cyc__genrep_134};static char
_tmp371[3]="tl";static struct _tuple5 Cyc__gentuple_137={offsetof(struct Cyc_List_List,tl),(
struct _dyneither_ptr){_tmp371,_tmp371,_tmp371 + 3},(void*)& Cyc__genrep_135};
static struct _tuple5*Cyc__genarr_138[2]={& Cyc__gentuple_136,& Cyc__gentuple_137};
struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_stmt_t46H2_rep={3,(
struct _dyneither_ptr*)& Cyc__genname_139,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_138),(void*)((struct _tuple5**)Cyc__genarr_138),(void*)((
struct _tuple5**)Cyc__genarr_138 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_135={
1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_stmt_t46H2_rep)};extern struct Cyc_Typerep_XTUnion_struct
Cyc_Absyn_absyn_annot_t_rep;static struct _tuple8*Cyc__genarr_131[0]={};static char
_tmp375[11]="AbsynAnnot";struct Cyc_Typerep_XTUnion_struct Cyc_Absyn_absyn_annot_t_rep={
7,(struct _dyneither_ptr){_tmp375,_tmp375,_tmp375 + 11},{(void*)((struct _tuple8**)
Cyc__genarr_131),(void*)((struct _tuple8**)Cyc__genarr_131),(void*)((struct
_tuple8**)Cyc__genarr_131 + 0)}};static char _tmp376[5]="Stmt";static struct
_dyneither_ptr Cyc__genname_571=(struct _dyneither_ptr){_tmp376,_tmp376,_tmp376 + 5};
static char _tmp377[2]="r";static struct _tuple5 Cyc__gentuple_565={offsetof(struct
Cyc_Absyn_Stmt,r),(struct _dyneither_ptr){_tmp377,_tmp377,_tmp377 + 2},(void*)& Cyc_Absyn_raw_stmt_t_rep};
static char _tmp378[4]="loc";static struct _tuple5 Cyc__gentuple_566={offsetof(struct
Cyc_Absyn_Stmt,loc),(struct _dyneither_ptr){_tmp378,_tmp378,_tmp378 + 4},(void*)&
Cyc__genrep_2};static char _tmp379[16]="non_local_preds";static struct _tuple5 Cyc__gentuple_567={
offsetof(struct Cyc_Absyn_Stmt,non_local_preds),(struct _dyneither_ptr){_tmp379,
_tmp379,_tmp379 + 16},(void*)& Cyc__genrep_135};static char _tmp37A[10]="try_depth";
static struct _tuple5 Cyc__gentuple_568={offsetof(struct Cyc_Absyn_Stmt,try_depth),(
struct _dyneither_ptr){_tmp37A,_tmp37A,_tmp37A + 10},(void*)& Cyc__genrep_10};
static char _tmp37B[6]="annot";static struct _tuple5 Cyc__gentuple_569={offsetof(
struct Cyc_Absyn_Stmt,annot),(struct _dyneither_ptr){_tmp37B,_tmp37B,_tmp37B + 6},(
void*)& Cyc_Absyn_absyn_annot_t_rep};static struct _tuple5*Cyc__genarr_570[5]={& Cyc__gentuple_565,&
Cyc__gentuple_566,& Cyc__gentuple_567,& Cyc__gentuple_568,& Cyc__gentuple_569};
struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Stmt_rep={3,(struct
_dyneither_ptr*)& Cyc__genname_571,sizeof(struct Cyc_Absyn_Stmt),{(void*)((struct
_tuple5**)Cyc__genarr_570),(void*)((struct _tuple5**)Cyc__genarr_570),(void*)((
struct _tuple5**)Cyc__genarr_570 + 5)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_134={
1,1,(void*)((void*)& Cyc_struct_Absyn_Stmt_rep)};extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_809;static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_809={1,1,(void*)((
void*)& Cyc_struct_Absyn_Vardecl_rep)};static char _tmp37F[7]="Fndecl";static struct
_dyneither_ptr Cyc__genname_838=(struct _dyneither_ptr){_tmp37F,_tmp37F,_tmp37F + 7};
static char _tmp380[3]="sc";static struct _tuple5 Cyc__gentuple_822={offsetof(struct
Cyc_Absyn_Fndecl,sc),(struct _dyneither_ptr){_tmp380,_tmp380,_tmp380 + 3},(void*)&
Cyc_Absyn_scope_t_rep};static char _tmp381[10]="is_inline";static struct _tuple5 Cyc__gentuple_823={
offsetof(struct Cyc_Absyn_Fndecl,is_inline),(struct _dyneither_ptr){_tmp381,
_tmp381,_tmp381 + 10},(void*)& Cyc__genrep_10};static char _tmp382[5]="name";static
struct _tuple5 Cyc__gentuple_824={offsetof(struct Cyc_Absyn_Fndecl,name),(struct
_dyneither_ptr){_tmp382,_tmp382,_tmp382 + 5},(void*)& Cyc__genrep_11};static char
_tmp383[4]="tvs";static struct _tuple5 Cyc__gentuple_825={offsetof(struct Cyc_Absyn_Fndecl,tvs),(
struct _dyneither_ptr){_tmp383,_tmp383,_tmp383 + 4},(void*)& Cyc__genrep_273};
static char _tmp384[7]="effect";static struct _tuple5 Cyc__gentuple_826={offsetof(
struct Cyc_Absyn_Fndecl,effect),(struct _dyneither_ptr){_tmp384,_tmp384,_tmp384 + 7},(
void*)& Cyc__genrep_110};static char _tmp385[9]="ret_type";static struct _tuple5 Cyc__gentuple_827={
offsetof(struct Cyc_Absyn_Fndecl,ret_type),(struct _dyneither_ptr){_tmp385,_tmp385,
_tmp385 + 9},(void*)((void*)& Cyc_Absyn_type_t_rep)};static char _tmp386[5]="args";
static struct _tuple5 Cyc__gentuple_828={offsetof(struct Cyc_Absyn_Fndecl,args),(
struct _dyneither_ptr){_tmp386,_tmp386,_tmp386 + 5},(void*)& Cyc__genrep_811};
static char _tmp387[10]="c_varargs";static struct _tuple5 Cyc__gentuple_829={
offsetof(struct Cyc_Absyn_Fndecl,c_varargs),(struct _dyneither_ptr){_tmp387,
_tmp387,_tmp387 + 10},(void*)& Cyc__genrep_10};static char _tmp388[12]="cyc_varargs";
static struct _tuple5 Cyc__gentuple_830={offsetof(struct Cyc_Absyn_Fndecl,cyc_varargs),(
struct _dyneither_ptr){_tmp388,_tmp388,_tmp388 + 12},(void*)& Cyc__genrep_810};
static char _tmp389[7]="rgn_po";static struct _tuple5 Cyc__gentuple_831={offsetof(
struct Cyc_Absyn_Fndecl,rgn_po),(struct _dyneither_ptr){_tmp389,_tmp389,_tmp389 + 7},(
void*)& Cyc__genrep_372};static char _tmp38A[5]="body";static struct _tuple5 Cyc__gentuple_832={
offsetof(struct Cyc_Absyn_Fndecl,body),(struct _dyneither_ptr){_tmp38A,_tmp38A,
_tmp38A + 5},(void*)& Cyc__genrep_134};static char _tmp38B[11]="cached_typ";static
struct _tuple5 Cyc__gentuple_833={offsetof(struct Cyc_Absyn_Fndecl,cached_typ),(
struct _dyneither_ptr){_tmp38B,_tmp38B,_tmp38B + 11},(void*)& Cyc__genrep_110};
static char _tmp38C[15]="param_vardecls";static struct _tuple5 Cyc__gentuple_834={
offsetof(struct Cyc_Absyn_Fndecl,param_vardecls),(struct _dyneither_ptr){_tmp38C,
_tmp38C,_tmp38C + 15},(void*)& Cyc__genrep_146};static char _tmp38D[11]="fn_vardecl";
static struct _tuple5 Cyc__gentuple_835={offsetof(struct Cyc_Absyn_Fndecl,fn_vardecl),(
struct _dyneither_ptr){_tmp38D,_tmp38D,_tmp38D + 11},(void*)& Cyc__genrep_809};
static char _tmp38E[11]="attributes";static struct _tuple5 Cyc__gentuple_836={
offsetof(struct Cyc_Absyn_Fndecl,attributes),(struct _dyneither_ptr){_tmp38E,
_tmp38E,_tmp38E + 11},(void*)& Cyc__genrep_62};static struct _tuple5*Cyc__genarr_837[
15]={& Cyc__gentuple_822,& Cyc__gentuple_823,& Cyc__gentuple_824,& Cyc__gentuple_825,&
Cyc__gentuple_826,& Cyc__gentuple_827,& Cyc__gentuple_828,& Cyc__gentuple_829,& Cyc__gentuple_830,&
Cyc__gentuple_831,& Cyc__gentuple_832,& Cyc__gentuple_833,& Cyc__gentuple_834,& Cyc__gentuple_835,&
Cyc__gentuple_836};struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Fndecl_rep={3,(
struct _dyneither_ptr*)& Cyc__genname_838,sizeof(struct Cyc_Absyn_Fndecl),{(void*)((
struct _tuple5**)Cyc__genarr_837),(void*)((struct _tuple5**)Cyc__genarr_837),(void*)((
struct _tuple5**)Cyc__genarr_837 + 15)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_808={
1,1,(void*)((void*)& Cyc_struct_Absyn_Fndecl_rep)};struct _tuple63{unsigned int f1;
struct Cyc_Absyn_Fndecl*f2;};static struct _tuple6 Cyc__gentuple_839={offsetof(
struct _tuple63,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_840={
offsetof(struct _tuple63,f2),(void*)& Cyc__genrep_808};static struct _tuple6*Cyc__genarr_841[
2]={& Cyc__gentuple_839,& Cyc__gentuple_840};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_807={4,sizeof(struct _tuple63),{(void*)((struct _tuple6**)Cyc__genarr_841),(
void*)((struct _tuple6**)Cyc__genarr_841),(void*)((struct _tuple6**)Cyc__genarr_841
+ 2)}};static char _tmp392[13]="Unresolved_b";static struct _tuple7 Cyc__gentuple_801={
0,(struct _dyneither_ptr){_tmp392,_tmp392,_tmp392 + 13}};static struct _tuple7*Cyc__genarr_802[
1]={& Cyc__gentuple_801};static char _tmp393[9]="Global_b";static struct _tuple5 Cyc__gentuple_842={
0,(struct _dyneither_ptr){_tmp393,_tmp393,_tmp393 + 9},(void*)& Cyc__genrep_803};
static char _tmp394[10]="Funname_b";static struct _tuple5 Cyc__gentuple_843={1,(
struct _dyneither_ptr){_tmp394,_tmp394,_tmp394 + 10},(void*)& Cyc__genrep_807};
static char _tmp395[8]="Param_b";static struct _tuple5 Cyc__gentuple_844={2,(struct
_dyneither_ptr){_tmp395,_tmp395,_tmp395 + 8},(void*)& Cyc__genrep_803};static char
_tmp396[8]="Local_b";static struct _tuple5 Cyc__gentuple_845={3,(struct
_dyneither_ptr){_tmp396,_tmp396,_tmp396 + 8},(void*)& Cyc__genrep_803};static char
_tmp397[6]="Pat_b";static struct _tuple5 Cyc__gentuple_846={4,(struct _dyneither_ptr){
_tmp397,_tmp397,_tmp397 + 6},(void*)& Cyc__genrep_803};static struct _tuple5*Cyc__genarr_847[
5]={& Cyc__gentuple_842,& Cyc__gentuple_843,& Cyc__gentuple_844,& Cyc__gentuple_845,&
Cyc__gentuple_846};static char _tmp399[8]="Binding";struct Cyc_Typerep_TUnion_struct
Cyc_Absyn_binding_t_rep={5,(struct _dyneither_ptr){_tmp399,_tmp399,_tmp399 + 8},{(
void*)((struct _tuple7**)Cyc__genarr_802),(void*)((struct _tuple7**)Cyc__genarr_802),(
void*)((struct _tuple7**)Cyc__genarr_802 + 1)},{(void*)((struct _tuple5**)Cyc__genarr_847),(
void*)((struct _tuple5**)Cyc__genarr_847),(void*)((struct _tuple5**)Cyc__genarr_847
+ 5)}};struct _tuple64{unsigned int f1;struct _tuple0*f2;void*f3;};static struct
_tuple6 Cyc__gentuple_848={offsetof(struct _tuple64,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_849={offsetof(struct _tuple64,f2),(void*)& Cyc__genrep_11};
static struct _tuple6 Cyc__gentuple_850={offsetof(struct _tuple64,f3),(void*)& Cyc_Absyn_binding_t_rep};
static struct _tuple6*Cyc__genarr_851[3]={& Cyc__gentuple_848,& Cyc__gentuple_849,&
Cyc__gentuple_850};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_800={4,
sizeof(struct _tuple64),{(void*)((struct _tuple6**)Cyc__genarr_851),(void*)((
struct _tuple6**)Cyc__genarr_851),(void*)((struct _tuple6**)Cyc__genarr_851 + 3)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_795;extern struct Cyc_Typerep_TUnion_struct
Cyc_Absyn_primop_t_rep;static char _tmp39B[5]="Plus";static struct _tuple7 Cyc__gentuple_765={
0,(struct _dyneither_ptr){_tmp39B,_tmp39B,_tmp39B + 5}};static char _tmp39C[6]="Times";
static struct _tuple7 Cyc__gentuple_766={1,(struct _dyneither_ptr){_tmp39C,_tmp39C,
_tmp39C + 6}};static char _tmp39D[6]="Minus";static struct _tuple7 Cyc__gentuple_767={
2,(struct _dyneither_ptr){_tmp39D,_tmp39D,_tmp39D + 6}};static char _tmp39E[4]="Div";
static struct _tuple7 Cyc__gentuple_768={3,(struct _dyneither_ptr){_tmp39E,_tmp39E,
_tmp39E + 4}};static char _tmp39F[4]="Mod";static struct _tuple7 Cyc__gentuple_769={4,(
struct _dyneither_ptr){_tmp39F,_tmp39F,_tmp39F + 4}};static char _tmp3A0[3]="Eq";
static struct _tuple7 Cyc__gentuple_770={5,(struct _dyneither_ptr){_tmp3A0,_tmp3A0,
_tmp3A0 + 3}};static char _tmp3A1[4]="Neq";static struct _tuple7 Cyc__gentuple_771={6,(
struct _dyneither_ptr){_tmp3A1,_tmp3A1,_tmp3A1 + 4}};static char _tmp3A2[3]="Gt";
static struct _tuple7 Cyc__gentuple_772={7,(struct _dyneither_ptr){_tmp3A2,_tmp3A2,
_tmp3A2 + 3}};static char _tmp3A3[3]="Lt";static struct _tuple7 Cyc__gentuple_773={8,(
struct _dyneither_ptr){_tmp3A3,_tmp3A3,_tmp3A3 + 3}};static char _tmp3A4[4]="Gte";
static struct _tuple7 Cyc__gentuple_774={9,(struct _dyneither_ptr){_tmp3A4,_tmp3A4,
_tmp3A4 + 4}};static char _tmp3A5[4]="Lte";static struct _tuple7 Cyc__gentuple_775={10,(
struct _dyneither_ptr){_tmp3A5,_tmp3A5,_tmp3A5 + 4}};static char _tmp3A6[4]="Not";
static struct _tuple7 Cyc__gentuple_776={11,(struct _dyneither_ptr){_tmp3A6,_tmp3A6,
_tmp3A6 + 4}};static char _tmp3A7[7]="Bitnot";static struct _tuple7 Cyc__gentuple_777={
12,(struct _dyneither_ptr){_tmp3A7,_tmp3A7,_tmp3A7 + 7}};static char _tmp3A8[7]="Bitand";
static struct _tuple7 Cyc__gentuple_778={13,(struct _dyneither_ptr){_tmp3A8,_tmp3A8,
_tmp3A8 + 7}};static char _tmp3A9[6]="Bitor";static struct _tuple7 Cyc__gentuple_779={
14,(struct _dyneither_ptr){_tmp3A9,_tmp3A9,_tmp3A9 + 6}};static char _tmp3AA[7]="Bitxor";
static struct _tuple7 Cyc__gentuple_780={15,(struct _dyneither_ptr){_tmp3AA,_tmp3AA,
_tmp3AA + 7}};static char _tmp3AB[10]="Bitlshift";static struct _tuple7 Cyc__gentuple_781={
16,(struct _dyneither_ptr){_tmp3AB,_tmp3AB,_tmp3AB + 10}};static char _tmp3AC[11]="Bitlrshift";
static struct _tuple7 Cyc__gentuple_782={17,(struct _dyneither_ptr){_tmp3AC,_tmp3AC,
_tmp3AC + 11}};static char _tmp3AD[11]="Bitarshift";static struct _tuple7 Cyc__gentuple_783={
18,(struct _dyneither_ptr){_tmp3AD,_tmp3AD,_tmp3AD + 11}};static char _tmp3AE[8]="Numelts";
static struct _tuple7 Cyc__gentuple_784={19,(struct _dyneither_ptr){_tmp3AE,_tmp3AE,
_tmp3AE + 8}};static struct _tuple7*Cyc__genarr_785[20]={& Cyc__gentuple_765,& Cyc__gentuple_766,&
Cyc__gentuple_767,& Cyc__gentuple_768,& Cyc__gentuple_769,& Cyc__gentuple_770,& Cyc__gentuple_771,&
Cyc__gentuple_772,& Cyc__gentuple_773,& Cyc__gentuple_774,& Cyc__gentuple_775,& Cyc__gentuple_776,&
Cyc__gentuple_777,& Cyc__gentuple_778,& Cyc__gentuple_779,& Cyc__gentuple_780,& Cyc__gentuple_781,&
Cyc__gentuple_782,& Cyc__gentuple_783,& Cyc__gentuple_784};static struct _tuple5*Cyc__genarr_786[
0]={};static char _tmp3B0[7]="Primop";struct Cyc_Typerep_TUnion_struct Cyc_Absyn_primop_t_rep={
5,(struct _dyneither_ptr){_tmp3B0,_tmp3B0,_tmp3B0 + 7},{(void*)((struct _tuple7**)
Cyc__genarr_785),(void*)((struct _tuple7**)Cyc__genarr_785),(void*)((struct
_tuple7**)Cyc__genarr_785 + 20)},{(void*)((struct _tuple5**)Cyc__genarr_786),(void*)((
struct _tuple5**)Cyc__genarr_786),(void*)((struct _tuple5**)Cyc__genarr_786 + 0)}};
struct _tuple65{unsigned int f1;void*f2;struct Cyc_List_List*f3;};static struct
_tuple6 Cyc__gentuple_796={offsetof(struct _tuple65,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_797={offsetof(struct _tuple65,f2),(void*)& Cyc_Absyn_primop_t_rep};
static struct _tuple6 Cyc__gentuple_798={offsetof(struct _tuple65,f3),(void*)& Cyc__genrep_497};
static struct _tuple6*Cyc__genarr_799[3]={& Cyc__gentuple_796,& Cyc__gentuple_797,&
Cyc__gentuple_798};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_795={4,
sizeof(struct _tuple65),{(void*)((struct _tuple6**)Cyc__genarr_799),(void*)((
struct _tuple6**)Cyc__genarr_799),(void*)((struct _tuple6**)Cyc__genarr_799 + 3)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_763;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_764;extern struct Cyc_Typerep_Struct_struct Cyc_struct_Core_Opt0Absyn_primop_t2_rep;
static char _tmp3B2[4]="Opt";static struct _dyneither_ptr Cyc__genname_789=(struct
_dyneither_ptr){_tmp3B2,_tmp3B2,_tmp3B2 + 4};static char _tmp3B3[2]="v";static
struct _tuple5 Cyc__gentuple_787={offsetof(struct Cyc_Core_Opt,v),(struct
_dyneither_ptr){_tmp3B3,_tmp3B3,_tmp3B3 + 2},(void*)& Cyc_Absyn_primop_t_rep};
static struct _tuple5*Cyc__genarr_788[1]={& Cyc__gentuple_787};struct Cyc_Typerep_Struct_struct
Cyc_struct_Core_Opt0Absyn_primop_t2_rep={3,(struct _dyneither_ptr*)& Cyc__genname_789,
sizeof(struct Cyc_Core_Opt),{(void*)((struct _tuple5**)Cyc__genarr_788),(void*)((
struct _tuple5**)Cyc__genarr_788),(void*)((struct _tuple5**)Cyc__genarr_788 + 1)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_764={1,1,(void*)((void*)& Cyc_struct_Core_Opt0Absyn_primop_t2_rep)};
struct _tuple66{unsigned int f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Core_Opt*f3;
struct Cyc_Absyn_Exp*f4;};static struct _tuple6 Cyc__gentuple_790={offsetof(struct
_tuple66,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_791={
offsetof(struct _tuple66,f2),(void*)& Cyc__genrep_130};static struct _tuple6 Cyc__gentuple_792={
offsetof(struct _tuple66,f3),(void*)& Cyc__genrep_764};static struct _tuple6 Cyc__gentuple_793={
offsetof(struct _tuple66,f4),(void*)& Cyc__genrep_130};static struct _tuple6*Cyc__genarr_794[
4]={& Cyc__gentuple_790,& Cyc__gentuple_791,& Cyc__gentuple_792,& Cyc__gentuple_793};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_763={4,sizeof(struct _tuple66),{(
void*)((struct _tuple6**)Cyc__genarr_794),(void*)((struct _tuple6**)Cyc__genarr_794),(
void*)((struct _tuple6**)Cyc__genarr_794 + 4)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_752;extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_incrementor_t_rep;
static char _tmp3B7[7]="PreInc";static struct _tuple7 Cyc__gentuple_753={0,(struct
_dyneither_ptr){_tmp3B7,_tmp3B7,_tmp3B7 + 7}};static char _tmp3B8[8]="PostInc";
static struct _tuple7 Cyc__gentuple_754={1,(struct _dyneither_ptr){_tmp3B8,_tmp3B8,
_tmp3B8 + 8}};static char _tmp3B9[7]="PreDec";static struct _tuple7 Cyc__gentuple_755={
2,(struct _dyneither_ptr){_tmp3B9,_tmp3B9,_tmp3B9 + 7}};static char _tmp3BA[8]="PostDec";
static struct _tuple7 Cyc__gentuple_756={3,(struct _dyneither_ptr){_tmp3BA,_tmp3BA,
_tmp3BA + 8}};static struct _tuple7*Cyc__genarr_757[4]={& Cyc__gentuple_753,& Cyc__gentuple_754,&
Cyc__gentuple_755,& Cyc__gentuple_756};static struct _tuple5*Cyc__genarr_758[0]={};
static char _tmp3BC[12]="Incrementor";struct Cyc_Typerep_TUnion_struct Cyc_Absyn_incrementor_t_rep={
5,(struct _dyneither_ptr){_tmp3BC,_tmp3BC,_tmp3BC + 12},{(void*)((struct _tuple7**)
Cyc__genarr_757),(void*)((struct _tuple7**)Cyc__genarr_757),(void*)((struct
_tuple7**)Cyc__genarr_757 + 4)},{(void*)((struct _tuple5**)Cyc__genarr_758),(void*)((
struct _tuple5**)Cyc__genarr_758),(void*)((struct _tuple5**)Cyc__genarr_758 + 0)}};
struct _tuple67{unsigned int f1;struct Cyc_Absyn_Exp*f2;void*f3;};static struct
_tuple6 Cyc__gentuple_759={offsetof(struct _tuple67,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_760={offsetof(struct _tuple67,f2),(void*)& Cyc__genrep_130};
static struct _tuple6 Cyc__gentuple_761={offsetof(struct _tuple67,f3),(void*)& Cyc_Absyn_incrementor_t_rep};
static struct _tuple6*Cyc__genarr_762[3]={& Cyc__gentuple_759,& Cyc__gentuple_760,&
Cyc__gentuple_761};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_752={4,
sizeof(struct _tuple67),{(void*)((struct _tuple6**)Cyc__genarr_762),(void*)((
struct _tuple6**)Cyc__genarr_762),(void*)((struct _tuple6**)Cyc__genarr_762 + 3)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_746;struct _tuple68{unsigned int
f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;struct Cyc_Absyn_Exp*f4;};static
struct _tuple6 Cyc__gentuple_747={offsetof(struct _tuple68,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_748={offsetof(struct _tuple68,f2),(void*)& Cyc__genrep_130};
static struct _tuple6 Cyc__gentuple_749={offsetof(struct _tuple68,f3),(void*)& Cyc__genrep_130};
static struct _tuple6 Cyc__gentuple_750={offsetof(struct _tuple68,f4),(void*)& Cyc__genrep_130};
static struct _tuple6*Cyc__genarr_751[4]={& Cyc__gentuple_747,& Cyc__gentuple_748,&
Cyc__gentuple_749,& Cyc__gentuple_750};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_746={
4,sizeof(struct _tuple68),{(void*)((struct _tuple6**)Cyc__genarr_751),(void*)((
struct _tuple6**)Cyc__genarr_751),(void*)((struct _tuple6**)Cyc__genarr_751 + 4)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_594;struct _tuple69{unsigned int
f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};static struct _tuple6 Cyc__gentuple_595={
offsetof(struct _tuple69,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_596={
offsetof(struct _tuple69,f2),(void*)& Cyc__genrep_130};static struct _tuple6 Cyc__gentuple_597={
offsetof(struct _tuple69,f3),(void*)& Cyc__genrep_130};static struct _tuple6*Cyc__genarr_598[
3]={& Cyc__gentuple_595,& Cyc__gentuple_596,& Cyc__gentuple_597};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_594={4,sizeof(struct _tuple69),{(void*)((struct _tuple6**)Cyc__genarr_598),(
void*)((struct _tuple6**)Cyc__genarr_598),(void*)((struct _tuple6**)Cyc__genarr_598
+ 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_741;static struct _tuple6
Cyc__gentuple_742={offsetof(struct _tuple58,f1),(void*)& Cyc__genrep_24};static
struct _tuple6 Cyc__gentuple_743={offsetof(struct _tuple58,f2),(void*)& Cyc__genrep_130};
static struct _tuple6 Cyc__gentuple_744={offsetof(struct _tuple58,f3),(void*)& Cyc__genrep_497};
static struct _tuple6*Cyc__genarr_745[3]={& Cyc__gentuple_742,& Cyc__gentuple_743,&
Cyc__gentuple_744};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_741={4,
sizeof(struct _tuple58),{(void*)((struct _tuple6**)Cyc__genarr_745),(void*)((
struct _tuple6**)Cyc__genarr_745),(void*)((struct _tuple6**)Cyc__genarr_745 + 3)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_722;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_723;extern struct Cyc_Typerep_Struct_struct Cyc_Absyn_vararg_call_info_t_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_724;static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_724={1,1,(void*)((void*)& Cyc_Absyn_vararg_info_t_rep)};static char
_tmp3C2[15]="VarargCallInfo";static struct _dyneither_ptr Cyc__genname_735=(struct
_dyneither_ptr){_tmp3C2,_tmp3C2,_tmp3C2 + 15};static char _tmp3C3[12]="num_varargs";
static struct _tuple5 Cyc__gentuple_731={offsetof(struct Cyc_Absyn_VarargCallInfo,num_varargs),(
struct _dyneither_ptr){_tmp3C3,_tmp3C3,_tmp3C3 + 12},(void*)& Cyc__genrep_10};
static char _tmp3C4[10]="injectors";static struct _tuple5 Cyc__gentuple_732={
offsetof(struct Cyc_Absyn_VarargCallInfo,injectors),(struct _dyneither_ptr){
_tmp3C4,_tmp3C4,_tmp3C4 + 10},(void*)& Cyc__genrep_265};static char _tmp3C5[4]="vai";
static struct _tuple5 Cyc__gentuple_733={offsetof(struct Cyc_Absyn_VarargCallInfo,vai),(
struct _dyneither_ptr){_tmp3C5,_tmp3C5,_tmp3C5 + 4},(void*)& Cyc__genrep_724};
static struct _tuple5*Cyc__genarr_734[3]={& Cyc__gentuple_731,& Cyc__gentuple_732,&
Cyc__gentuple_733};struct Cyc_Typerep_Struct_struct Cyc_Absyn_vararg_call_info_t_rep={
3,(struct _dyneither_ptr*)& Cyc__genname_735,sizeof(struct Cyc_Absyn_VarargCallInfo),{(
void*)((struct _tuple5**)Cyc__genarr_734),(void*)((struct _tuple5**)Cyc__genarr_734),(
void*)((struct _tuple5**)Cyc__genarr_734 + 3)}};static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_723={1,1,(void*)((void*)& Cyc_Absyn_vararg_call_info_t_rep)};struct
_tuple70{unsigned int f1;struct Cyc_Absyn_Exp*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_VarargCallInfo*
f4;};static struct _tuple6 Cyc__gentuple_736={offsetof(struct _tuple70,f1),(void*)&
Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_737={offsetof(struct _tuple70,f2),(
void*)& Cyc__genrep_130};static struct _tuple6 Cyc__gentuple_738={offsetof(struct
_tuple70,f3),(void*)& Cyc__genrep_497};static struct _tuple6 Cyc__gentuple_739={
offsetof(struct _tuple70,f4),(void*)& Cyc__genrep_723};static struct _tuple6*Cyc__genarr_740[
4]={& Cyc__gentuple_736,& Cyc__gentuple_737,& Cyc__gentuple_738,& Cyc__gentuple_739};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_722={4,sizeof(struct _tuple70),{(
void*)((struct _tuple6**)Cyc__genarr_740),(void*)((struct _tuple6**)Cyc__genarr_740),(
void*)((struct _tuple6**)Cyc__genarr_740 + 4)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_717;static struct _tuple6 Cyc__gentuple_718={offsetof(struct _tuple58,f1),(
void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_719={offsetof(struct
_tuple58,f2),(void*)& Cyc__genrep_130};static struct _tuple6 Cyc__gentuple_720={
offsetof(struct _tuple58,f3),(void*)& Cyc__genrep_121};static struct _tuple6*Cyc__genarr_721[
3]={& Cyc__gentuple_718,& Cyc__gentuple_719,& Cyc__gentuple_720};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_717={4,sizeof(struct _tuple58),{(void*)((struct _tuple6**)Cyc__genarr_721),(
void*)((struct _tuple6**)Cyc__genarr_721),(void*)((struct _tuple6**)Cyc__genarr_721
+ 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_704;extern struct Cyc_Typerep_TUnion_struct
Cyc_Absyn_coercion_t_rep;static char _tmp3CA[17]="Unknown_coercion";static struct
_tuple7 Cyc__gentuple_705={0,(struct _dyneither_ptr){_tmp3CA,_tmp3CA,_tmp3CA + 17}};
static char _tmp3CB[12]="No_coercion";static struct _tuple7 Cyc__gentuple_706={1,(
struct _dyneither_ptr){_tmp3CB,_tmp3CB,_tmp3CB + 12}};static char _tmp3CC[16]="NonNull_to_Null";
static struct _tuple7 Cyc__gentuple_707={2,(struct _dyneither_ptr){_tmp3CC,_tmp3CC,
_tmp3CC + 16}};static char _tmp3CD[15]="Other_coercion";static struct _tuple7 Cyc__gentuple_708={
3,(struct _dyneither_ptr){_tmp3CD,_tmp3CD,_tmp3CD + 15}};static struct _tuple7*Cyc__genarr_709[
4]={& Cyc__gentuple_705,& Cyc__gentuple_706,& Cyc__gentuple_707,& Cyc__gentuple_708};
static struct _tuple5*Cyc__genarr_710[0]={};static char _tmp3CF[9]="Coercion";struct
Cyc_Typerep_TUnion_struct Cyc_Absyn_coercion_t_rep={5,(struct _dyneither_ptr){
_tmp3CF,_tmp3CF,_tmp3CF + 9},{(void*)((struct _tuple7**)Cyc__genarr_709),(void*)((
struct _tuple7**)Cyc__genarr_709),(void*)((struct _tuple7**)Cyc__genarr_709 + 4)},{(
void*)((struct _tuple5**)Cyc__genarr_710),(void*)((struct _tuple5**)Cyc__genarr_710),(
void*)((struct _tuple5**)Cyc__genarr_710 + 0)}};struct _tuple71{unsigned int f1;void*
f2;struct Cyc_Absyn_Exp*f3;int f4;void*f5;};static struct _tuple6 Cyc__gentuple_711={
offsetof(struct _tuple71,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_712={
offsetof(struct _tuple71,f2),(void*)((void*)& Cyc_Absyn_type_t_rep)};static struct
_tuple6 Cyc__gentuple_713={offsetof(struct _tuple71,f3),(void*)& Cyc__genrep_130};
static struct _tuple6 Cyc__gentuple_714={offsetof(struct _tuple71,f4),(void*)& Cyc__genrep_10};
static struct _tuple6 Cyc__gentuple_715={offsetof(struct _tuple71,f5),(void*)& Cyc_Absyn_coercion_t_rep};
static struct _tuple6*Cyc__genarr_716[5]={& Cyc__gentuple_711,& Cyc__gentuple_712,&
Cyc__gentuple_713,& Cyc__gentuple_714,& Cyc__gentuple_715};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_704={4,sizeof(struct _tuple71),{(void*)((struct _tuple6**)Cyc__genarr_716),(
void*)((struct _tuple6**)Cyc__genarr_716),(void*)((struct _tuple6**)Cyc__genarr_716
+ 5)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_699;static struct _tuple6
Cyc__gentuple_700={offsetof(struct _tuple69,f1),(void*)& Cyc__genrep_24};static
struct _tuple6 Cyc__gentuple_701={offsetof(struct _tuple69,f2),(void*)& Cyc__genrep_145};
static struct _tuple6 Cyc__gentuple_702={offsetof(struct _tuple69,f3),(void*)& Cyc__genrep_130};
static struct _tuple6*Cyc__genarr_703[3]={& Cyc__gentuple_700,& Cyc__gentuple_701,&
Cyc__gentuple_702};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_699={4,
sizeof(struct _tuple69),{(void*)((struct _tuple6**)Cyc__genarr_703),(void*)((
struct _tuple6**)Cyc__genarr_703),(void*)((struct _tuple6**)Cyc__genarr_703 + 3)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_116;static struct _tuple6 Cyc__gentuple_117={
offsetof(struct _tuple6,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_118={
offsetof(struct _tuple6,f2),(void*)((void*)& Cyc_Absyn_type_t_rep)};static struct
_tuple6*Cyc__genarr_119[2]={& Cyc__gentuple_117,& Cyc__gentuple_118};static struct
Cyc_Typerep_Tuple_struct Cyc__genrep_116={4,sizeof(struct _tuple6),{(void*)((
struct _tuple6**)Cyc__genarr_119),(void*)((struct _tuple6**)Cyc__genarr_119),(void*)((
struct _tuple6**)Cyc__genarr_119 + 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_686;
extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_offsetof_field_t_rep;extern struct
Cyc_Typerep_Tuple_struct Cyc__genrep_688;struct _tuple72{unsigned int f1;
unsigned int f2;};static struct _tuple6 Cyc__gentuple_689={offsetof(struct _tuple72,f1),(
void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_690={offsetof(struct
_tuple72,f2),(void*)& Cyc__genrep_24};static struct _tuple6*Cyc__genarr_691[2]={&
Cyc__gentuple_689,& Cyc__gentuple_690};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_688={
4,sizeof(struct _tuple72),{(void*)((struct _tuple6**)Cyc__genarr_691),(void*)((
struct _tuple6**)Cyc__genarr_691),(void*)((struct _tuple6**)Cyc__genarr_691 + 2)}};
static struct _tuple7*Cyc__genarr_687[0]={};static char _tmp3D4[12]="StructField";
static struct _tuple5 Cyc__gentuple_692={0,(struct _dyneither_ptr){_tmp3D4,_tmp3D4,
_tmp3D4 + 12},(void*)& Cyc__genrep_336};static char _tmp3D5[11]="TupleIndex";static
struct _tuple5 Cyc__gentuple_693={1,(struct _dyneither_ptr){_tmp3D5,_tmp3D5,_tmp3D5
+ 11},(void*)& Cyc__genrep_688};static struct _tuple5*Cyc__genarr_694[2]={& Cyc__gentuple_692,&
Cyc__gentuple_693};static char _tmp3D7[14]="OffsetofField";struct Cyc_Typerep_TUnion_struct
Cyc_Absyn_offsetof_field_t_rep={5,(struct _dyneither_ptr){_tmp3D7,_tmp3D7,_tmp3D7
+ 14},{(void*)((struct _tuple7**)Cyc__genarr_687),(void*)((struct _tuple7**)Cyc__genarr_687),(
void*)((struct _tuple7**)Cyc__genarr_687 + 0)},{(void*)((struct _tuple5**)Cyc__genarr_694),(
void*)((struct _tuple5**)Cyc__genarr_694),(void*)((struct _tuple5**)Cyc__genarr_694
+ 2)}};static struct _tuple6 Cyc__gentuple_695={offsetof(struct _tuple28,f1),(void*)&
Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_696={offsetof(struct _tuple28,f2),(
void*)((void*)& Cyc_Absyn_type_t_rep)};static struct _tuple6 Cyc__gentuple_697={
offsetof(struct _tuple28,f3),(void*)& Cyc_Absyn_offsetof_field_t_rep};static struct
_tuple6*Cyc__genarr_698[3]={& Cyc__gentuple_695,& Cyc__gentuple_696,& Cyc__gentuple_697};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_686={4,sizeof(struct _tuple28),{(
void*)((struct _tuple6**)Cyc__genarr_698),(void*)((struct _tuple6**)Cyc__genarr_698),(
void*)((struct _tuple6**)Cyc__genarr_698 + 3)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_681;struct _tuple73{unsigned int f1;struct Cyc_List_List*f2;void*f3;};
static struct _tuple6 Cyc__gentuple_682={offsetof(struct _tuple73,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_683={offsetof(struct _tuple73,f2),(void*)& Cyc__genrep_273};
static struct _tuple6 Cyc__gentuple_684={offsetof(struct _tuple73,f3),(void*)((void*)&
Cyc_Absyn_type_t_rep)};static struct _tuple6*Cyc__genarr_685[3]={& Cyc__gentuple_682,&
Cyc__gentuple_683,& Cyc__gentuple_684};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_681={
4,sizeof(struct _tuple73),{(void*)((struct _tuple6**)Cyc__genarr_685),(void*)((
struct _tuple6**)Cyc__genarr_685),(void*)((struct _tuple6**)Cyc__genarr_685 + 3)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_674;struct _tuple74{unsigned int
f1;struct Cyc_Absyn_Exp*f2;struct _dyneither_ptr*f3;int f4;int f5;};static struct
_tuple6 Cyc__gentuple_675={offsetof(struct _tuple74,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_676={offsetof(struct _tuple74,f2),(void*)& Cyc__genrep_130};
static struct _tuple6 Cyc__gentuple_677={offsetof(struct _tuple74,f3),(void*)& Cyc__genrep_13};
static struct _tuple6 Cyc__gentuple_678={offsetof(struct _tuple74,f4),(void*)& Cyc__genrep_10};
static struct _tuple6 Cyc__gentuple_679={offsetof(struct _tuple74,f5),(void*)& Cyc__genrep_10};
static struct _tuple6*Cyc__genarr_680[5]={& Cyc__gentuple_675,& Cyc__gentuple_676,&
Cyc__gentuple_677,& Cyc__gentuple_678,& Cyc__gentuple_679};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_674={4,sizeof(struct _tuple74),{(void*)((struct _tuple6**)Cyc__genarr_680),(
void*)((struct _tuple6**)Cyc__genarr_680),(void*)((struct _tuple6**)Cyc__genarr_680
+ 5)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_670;static struct _tuple6
Cyc__gentuple_671={offsetof(struct _tuple14,f1),(void*)& Cyc__genrep_24};static
struct _tuple6 Cyc__gentuple_672={offsetof(struct _tuple14,f2),(void*)& Cyc__genrep_497};
static struct _tuple6*Cyc__genarr_673[2]={& Cyc__gentuple_671,& Cyc__gentuple_672};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_670={4,sizeof(struct _tuple14),{(
void*)((struct _tuple6**)Cyc__genarr_673),(void*)((struct _tuple6**)Cyc__genarr_673),(
void*)((struct _tuple6**)Cyc__genarr_673 + 2)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_655;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_656;extern
struct Cyc_Typerep_Tuple_struct Cyc__genrep_657;static struct _tuple6 Cyc__gentuple_662={
offsetof(struct _tuple1,f1),(void*)& Cyc__genrep_658};static struct _tuple6 Cyc__gentuple_663={
offsetof(struct _tuple1,f2),(void*)& Cyc_Absyn_tqual_t_rep};static struct _tuple6 Cyc__gentuple_664={
offsetof(struct _tuple1,f3),(void*)((void*)& Cyc_Absyn_type_t_rep)};static struct
_tuple6*Cyc__genarr_665[3]={& Cyc__gentuple_662,& Cyc__gentuple_663,& Cyc__gentuple_664};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_657={4,sizeof(struct _tuple1),{(
void*)((struct _tuple6**)Cyc__genarr_665),(void*)((struct _tuple6**)Cyc__genarr_665),(
void*)((struct _tuple6**)Cyc__genarr_665 + 3)}};static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_656={1,1,(void*)((void*)& Cyc__genrep_657)};extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_576;extern struct Cyc_Typerep_Struct_struct Cyc_struct_List_List060List_list_t0Absyn_designator_t46H24Absyn_exp_t1_446H2_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_577;extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_578;static struct _tuple6 Cyc__gentuple_579={offsetof(struct _tuple13,f1),(
void*)& Cyc__genrep_334};static struct _tuple6 Cyc__gentuple_580={offsetof(struct
_tuple13,f2),(void*)& Cyc__genrep_130};static struct _tuple6*Cyc__genarr_581[2]={&
Cyc__gentuple_579,& Cyc__gentuple_580};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_578={
4,sizeof(struct _tuple13),{(void*)((struct _tuple6**)Cyc__genarr_581),(void*)((
struct _tuple6**)Cyc__genarr_581),(void*)((struct _tuple6**)Cyc__genarr_581 + 2)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_577={1,1,(void*)((void*)& Cyc__genrep_578)};
static char _tmp3E0[5]="List";static struct _dyneither_ptr Cyc__genname_585=(struct
_dyneither_ptr){_tmp3E0,_tmp3E0,_tmp3E0 + 5};static char _tmp3E1[3]="hd";static
struct _tuple5 Cyc__gentuple_582={offsetof(struct Cyc_List_List,hd),(struct
_dyneither_ptr){_tmp3E1,_tmp3E1,_tmp3E1 + 3},(void*)& Cyc__genrep_577};static char
_tmp3E2[3]="tl";static struct _tuple5 Cyc__gentuple_583={offsetof(struct Cyc_List_List,tl),(
struct _dyneither_ptr){_tmp3E2,_tmp3E2,_tmp3E2 + 3},(void*)& Cyc__genrep_576};
static struct _tuple5*Cyc__genarr_584[2]={& Cyc__gentuple_582,& Cyc__gentuple_583};
struct Cyc_Typerep_Struct_struct Cyc_struct_List_List060List_list_t0Absyn_designator_t46H24Absyn_exp_t1_446H2_rep={
3,(struct _dyneither_ptr*)& Cyc__genname_585,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_584),(void*)((struct _tuple5**)Cyc__genarr_584),(void*)((
struct _tuple5**)Cyc__genarr_584 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_576={
1,1,(void*)((void*)& Cyc_struct_List_List060List_list_t0Absyn_designator_t46H24Absyn_exp_t1_446H2_rep)};
struct _tuple75{unsigned int f1;struct _tuple1*f2;struct Cyc_List_List*f3;};static
struct _tuple6 Cyc__gentuple_666={offsetof(struct _tuple75,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_667={offsetof(struct _tuple75,f2),(void*)& Cyc__genrep_656};
static struct _tuple6 Cyc__gentuple_668={offsetof(struct _tuple75,f3),(void*)& Cyc__genrep_576};
static struct _tuple6*Cyc__genarr_669[3]={& Cyc__gentuple_666,& Cyc__gentuple_667,&
Cyc__gentuple_668};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_655={4,
sizeof(struct _tuple75),{(void*)((struct _tuple6**)Cyc__genarr_669),(void*)((
struct _tuple6**)Cyc__genarr_669),(void*)((struct _tuple6**)Cyc__genarr_669 + 3)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_651;static struct _tuple6 Cyc__gentuple_652={
offsetof(struct _tuple14,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_653={
offsetof(struct _tuple14,f2),(void*)& Cyc__genrep_576};static struct _tuple6*Cyc__genarr_654[
2]={& Cyc__gentuple_652,& Cyc__gentuple_653};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_651={4,sizeof(struct _tuple14),{(void*)((struct _tuple6**)Cyc__genarr_654),(
void*)((struct _tuple6**)Cyc__genarr_654),(void*)((struct _tuple6**)Cyc__genarr_654
+ 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_644;struct _tuple76{
unsigned int f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;struct Cyc_Absyn_Exp*
f4;int f5;};static struct _tuple6 Cyc__gentuple_645={offsetof(struct _tuple76,f1),(
void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_646={offsetof(struct
_tuple76,f2),(void*)& Cyc__genrep_148};static struct _tuple6 Cyc__gentuple_647={
offsetof(struct _tuple76,f3),(void*)& Cyc__genrep_130};static struct _tuple6 Cyc__gentuple_648={
offsetof(struct _tuple76,f4),(void*)& Cyc__genrep_130};static struct _tuple6 Cyc__gentuple_649={
offsetof(struct _tuple76,f5),(void*)& Cyc__genrep_10};static struct _tuple6*Cyc__genarr_650[
5]={& Cyc__gentuple_645,& Cyc__gentuple_646,& Cyc__gentuple_647,& Cyc__gentuple_648,&
Cyc__gentuple_649};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_644={4,
sizeof(struct _tuple76),{(void*)((struct _tuple6**)Cyc__genarr_650),(void*)((
struct _tuple6**)Cyc__genarr_650),(void*)((struct _tuple6**)Cyc__genarr_650 + 5)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_636;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_637;static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_637={1,1,(void*)((
void*)& Cyc_struct_Absyn_Aggrdecl_rep)};struct _tuple77{unsigned int f1;struct
_tuple0*f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;struct Cyc_Absyn_Aggrdecl*
f5;};static struct _tuple6 Cyc__gentuple_638={offsetof(struct _tuple77,f1),(void*)&
Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_639={offsetof(struct _tuple77,f2),(
void*)& Cyc__genrep_11};static struct _tuple6 Cyc__gentuple_640={offsetof(struct
_tuple77,f3),(void*)& Cyc__genrep_121};static struct _tuple6 Cyc__gentuple_641={
offsetof(struct _tuple77,f4),(void*)& Cyc__genrep_576};static struct _tuple6 Cyc__gentuple_642={
offsetof(struct _tuple77,f5),(void*)& Cyc__genrep_637};static struct _tuple6*Cyc__genarr_643[
5]={& Cyc__gentuple_638,& Cyc__gentuple_639,& Cyc__gentuple_640,& Cyc__gentuple_641,&
Cyc__gentuple_642};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_636={4,
sizeof(struct _tuple77),{(void*)((struct _tuple6**)Cyc__genarr_643),(void*)((
struct _tuple6**)Cyc__genarr_643),(void*)((struct _tuple6**)Cyc__genarr_643 + 5)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_631;static struct _tuple6 Cyc__gentuple_632={
offsetof(struct _tuple65,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_633={
offsetof(struct _tuple65,f2),(void*)((void*)& Cyc_Absyn_type_t_rep)};static struct
_tuple6 Cyc__gentuple_634={offsetof(struct _tuple65,f3),(void*)& Cyc__genrep_576};
static struct _tuple6*Cyc__genarr_635[3]={& Cyc__gentuple_632,& Cyc__gentuple_633,&
Cyc__gentuple_634};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_631={4,
sizeof(struct _tuple65),{(void*)((struct _tuple6**)Cyc__genarr_635),(void*)((
struct _tuple6**)Cyc__genarr_635),(void*)((struct _tuple6**)Cyc__genarr_635 + 3)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_625;struct _tuple78{unsigned int
f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Datatypedecl*f3;struct Cyc_Absyn_Datatypefield*
f4;};static struct _tuple6 Cyc__gentuple_626={offsetof(struct _tuple78,f1),(void*)&
Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_627={offsetof(struct _tuple78,f2),(
void*)& Cyc__genrep_497};static struct _tuple6 Cyc__gentuple_628={offsetof(struct
_tuple78,f3),(void*)((void*)& Cyc__genrep_263)};static struct _tuple6 Cyc__gentuple_629={
offsetof(struct _tuple78,f4),(void*)& Cyc__genrep_246};static struct _tuple6*Cyc__genarr_630[
4]={& Cyc__gentuple_626,& Cyc__gentuple_627,& Cyc__gentuple_628,& Cyc__gentuple_629};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_625={4,sizeof(struct _tuple78),{(
void*)((struct _tuple6**)Cyc__genarr_630),(void*)((struct _tuple6**)Cyc__genarr_630),(
void*)((struct _tuple6**)Cyc__genarr_630 + 4)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_618;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_619;static
struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_619={1,1,(void*)((void*)& Cyc_struct_Absyn_Enumdecl_rep)};
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_612;static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_612={1,1,(void*)((void*)& Cyc_struct_Absyn_Enumfield_rep)};struct
_tuple79{unsigned int f1;struct _tuple0*f2;struct Cyc_Absyn_Enumdecl*f3;struct Cyc_Absyn_Enumfield*
f4;};static struct _tuple6 Cyc__gentuple_620={offsetof(struct _tuple79,f1),(void*)&
Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_621={offsetof(struct _tuple79,f2),(
void*)& Cyc__genrep_11};static struct _tuple6 Cyc__gentuple_622={offsetof(struct
_tuple79,f3),(void*)& Cyc__genrep_619};static struct _tuple6 Cyc__gentuple_623={
offsetof(struct _tuple79,f4),(void*)& Cyc__genrep_612};static struct _tuple6*Cyc__genarr_624[
4]={& Cyc__gentuple_620,& Cyc__gentuple_621,& Cyc__gentuple_622,& Cyc__gentuple_623};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_618={4,sizeof(struct _tuple79),{(
void*)((struct _tuple6**)Cyc__genarr_624),(void*)((struct _tuple6**)Cyc__genarr_624),(
void*)((struct _tuple6**)Cyc__genarr_624 + 4)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_611;struct _tuple80{unsigned int f1;struct _tuple0*f2;void*f3;struct Cyc_Absyn_Enumfield*
f4;};static struct _tuple6 Cyc__gentuple_613={offsetof(struct _tuple80,f1),(void*)&
Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_614={offsetof(struct _tuple80,f2),(
void*)& Cyc__genrep_11};static struct _tuple6 Cyc__gentuple_615={offsetof(struct
_tuple80,f3),(void*)((void*)& Cyc_Absyn_type_t_rep)};static struct _tuple6 Cyc__gentuple_616={
offsetof(struct _tuple80,f4),(void*)& Cyc__genrep_612};static struct _tuple6*Cyc__genarr_617[
4]={& Cyc__gentuple_613,& Cyc__gentuple_614,& Cyc__gentuple_615,& Cyc__gentuple_616};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_611={4,sizeof(struct _tuple80),{(
void*)((struct _tuple6**)Cyc__genarr_617),(void*)((struct _tuple6**)Cyc__genarr_617),(
void*)((struct _tuple6**)Cyc__genarr_617 + 4)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_599;extern struct Cyc_Typerep_Struct_struct Cyc_Absyn_malloc_info_t_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_600;static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_600={1,1,(void*)((void*)& Cyc_Absyn_type_t_rep)};static char _tmp3F1[11]="MallocInfo";
static struct _dyneither_ptr Cyc__genname_607=(struct _dyneither_ptr){_tmp3F1,
_tmp3F1,_tmp3F1 + 11};static char _tmp3F2[10]="is_calloc";static struct _tuple5 Cyc__gentuple_601={
offsetof(struct Cyc_Absyn_MallocInfo,is_calloc),(struct _dyneither_ptr){_tmp3F2,
_tmp3F2,_tmp3F2 + 10},(void*)& Cyc__genrep_10};static char _tmp3F3[4]="rgn";static
struct _tuple5 Cyc__gentuple_602={offsetof(struct Cyc_Absyn_MallocInfo,rgn),(struct
_dyneither_ptr){_tmp3F3,_tmp3F3,_tmp3F3 + 4},(void*)& Cyc__genrep_145};static char
_tmp3F4[9]="elt_type";static struct _tuple5 Cyc__gentuple_603={offsetof(struct Cyc_Absyn_MallocInfo,elt_type),(
struct _dyneither_ptr){_tmp3F4,_tmp3F4,_tmp3F4 + 9},(void*)& Cyc__genrep_600};
static char _tmp3F5[9]="num_elts";static struct _tuple5 Cyc__gentuple_604={offsetof(
struct Cyc_Absyn_MallocInfo,num_elts),(struct _dyneither_ptr){_tmp3F5,_tmp3F5,
_tmp3F5 + 9},(void*)& Cyc__genrep_130};static char _tmp3F6[11]="fat_result";static
struct _tuple5 Cyc__gentuple_605={offsetof(struct Cyc_Absyn_MallocInfo,fat_result),(
struct _dyneither_ptr){_tmp3F6,_tmp3F6,_tmp3F6 + 11},(void*)& Cyc__genrep_10};
static struct _tuple5*Cyc__genarr_606[5]={& Cyc__gentuple_601,& Cyc__gentuple_602,&
Cyc__gentuple_603,& Cyc__gentuple_604,& Cyc__gentuple_605};struct Cyc_Typerep_Struct_struct
Cyc_Absyn_malloc_info_t_rep={3,(struct _dyneither_ptr*)& Cyc__genname_607,sizeof(
struct Cyc_Absyn_MallocInfo),{(void*)((struct _tuple5**)Cyc__genarr_606),(void*)((
struct _tuple5**)Cyc__genarr_606),(void*)((struct _tuple5**)Cyc__genarr_606 + 5)}};
struct _tuple81{unsigned int f1;struct Cyc_Absyn_MallocInfo f2;};static struct _tuple6
Cyc__gentuple_608={offsetof(struct _tuple81,f1),(void*)& Cyc__genrep_24};static
struct _tuple6 Cyc__gentuple_609={offsetof(struct _tuple81,f2),(void*)& Cyc_Absyn_malloc_info_t_rep};
static struct _tuple6*Cyc__genarr_610[2]={& Cyc__gentuple_608,& Cyc__gentuple_609};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_599={4,sizeof(struct _tuple81),{(
void*)((struct _tuple6**)Cyc__genarr_610),(void*)((struct _tuple6**)Cyc__genarr_610),(
void*)((struct _tuple6**)Cyc__genarr_610 + 2)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_575;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_586;extern
struct Cyc_Typerep_Struct_struct Cyc_struct_Core_Opt0Absyn_typedef_name_t2_rep;
static char _tmp3F9[4]="Opt";static struct _dyneither_ptr Cyc__genname_589=(struct
_dyneither_ptr){_tmp3F9,_tmp3F9,_tmp3F9 + 4};static char _tmp3FA[2]="v";static
struct _tuple5 Cyc__gentuple_587={offsetof(struct Cyc_Core_Opt,v),(struct
_dyneither_ptr){_tmp3FA,_tmp3FA,_tmp3FA + 2},(void*)& Cyc__genrep_11};static struct
_tuple5*Cyc__genarr_588[1]={& Cyc__gentuple_587};struct Cyc_Typerep_Struct_struct
Cyc_struct_Core_Opt0Absyn_typedef_name_t2_rep={3,(struct _dyneither_ptr*)& Cyc__genname_589,
sizeof(struct Cyc_Core_Opt),{(void*)((struct _tuple5**)Cyc__genarr_588),(void*)((
struct _tuple5**)Cyc__genarr_588),(void*)((struct _tuple5**)Cyc__genarr_588 + 1)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_586={1,1,(void*)((void*)& Cyc_struct_Core_Opt0Absyn_typedef_name_t2_rep)};
struct _tuple82{unsigned int f1;struct Cyc_Core_Opt*f2;struct Cyc_List_List*f3;};
static struct _tuple6 Cyc__gentuple_590={offsetof(struct _tuple82,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_591={offsetof(struct _tuple82,f2),(void*)& Cyc__genrep_586};
static struct _tuple6 Cyc__gentuple_592={offsetof(struct _tuple82,f3),(void*)& Cyc__genrep_576};
static struct _tuple6*Cyc__genarr_593[3]={& Cyc__gentuple_590,& Cyc__gentuple_591,&
Cyc__gentuple_592};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_575={4,
sizeof(struct _tuple82),{(void*)((struct _tuple6**)Cyc__genarr_593),(void*)((
struct _tuple6**)Cyc__genarr_593),(void*)((struct _tuple6**)Cyc__genarr_593 + 3)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_133;static struct _tuple6 Cyc__gentuple_572={
offsetof(struct _tuple39,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_573={
offsetof(struct _tuple39,f2),(void*)& Cyc__genrep_134};static struct _tuple6*Cyc__genarr_574[
2]={& Cyc__gentuple_572,& Cyc__gentuple_573};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_133={4,sizeof(struct _tuple39),{(void*)((struct _tuple6**)Cyc__genarr_574),(
void*)((struct _tuple6**)Cyc__genarr_574),(void*)((struct _tuple6**)Cyc__genarr_574
+ 2)}};static struct _tuple7*Cyc__genarr_132[0]={};static char _tmp3FF[8]="Const_e";
static struct _tuple5 Cyc__gentuple_882={0,(struct _dyneither_ptr){_tmp3FF,_tmp3FF,
_tmp3FF + 8},(void*)& Cyc__genrep_852};static char _tmp400[6]="Var_e";static struct
_tuple5 Cyc__gentuple_883={1,(struct _dyneither_ptr){_tmp400,_tmp400,_tmp400 + 6},(
void*)& Cyc__genrep_800};static char _tmp401[12]="UnknownId_e";static struct _tuple5
Cyc__gentuple_884={2,(struct _dyneither_ptr){_tmp401,_tmp401,_tmp401 + 12},(void*)&
Cyc__genrep_196};static char _tmp402[9]="Primop_e";static struct _tuple5 Cyc__gentuple_885={
3,(struct _dyneither_ptr){_tmp402,_tmp402,_tmp402 + 9},(void*)& Cyc__genrep_795};
static char _tmp403[11]="AssignOp_e";static struct _tuple5 Cyc__gentuple_886={4,(
struct _dyneither_ptr){_tmp403,_tmp403,_tmp403 + 11},(void*)& Cyc__genrep_763};
static char _tmp404[12]="Increment_e";static struct _tuple5 Cyc__gentuple_887={5,(
struct _dyneither_ptr){_tmp404,_tmp404,_tmp404 + 12},(void*)& Cyc__genrep_752};
static char _tmp405[14]="Conditional_e";static struct _tuple5 Cyc__gentuple_888={6,(
struct _dyneither_ptr){_tmp405,_tmp405,_tmp405 + 14},(void*)& Cyc__genrep_746};
static char _tmp406[6]="And_e";static struct _tuple5 Cyc__gentuple_889={7,(struct
_dyneither_ptr){_tmp406,_tmp406,_tmp406 + 6},(void*)& Cyc__genrep_594};static char
_tmp407[5]="Or_e";static struct _tuple5 Cyc__gentuple_890={8,(struct _dyneither_ptr){
_tmp407,_tmp407,_tmp407 + 5},(void*)& Cyc__genrep_594};static char _tmp408[9]="SeqExp_e";
static struct _tuple5 Cyc__gentuple_891={9,(struct _dyneither_ptr){_tmp408,_tmp408,
_tmp408 + 9},(void*)& Cyc__genrep_594};static char _tmp409[14]="UnknownCall_e";
static struct _tuple5 Cyc__gentuple_892={10,(struct _dyneither_ptr){_tmp409,_tmp409,
_tmp409 + 14},(void*)& Cyc__genrep_741};static char _tmp40A[9]="FnCall_e";static
struct _tuple5 Cyc__gentuple_893={11,(struct _dyneither_ptr){_tmp40A,_tmp40A,
_tmp40A + 9},(void*)& Cyc__genrep_722};static char _tmp40B[8]="Throw_e";static struct
_tuple5 Cyc__gentuple_894={12,(struct _dyneither_ptr){_tmp40B,_tmp40B,_tmp40B + 8},(
void*)& Cyc__genrep_129};static char _tmp40C[16]="NoInstantiate_e";static struct
_tuple5 Cyc__gentuple_895={13,(struct _dyneither_ptr){_tmp40C,_tmp40C,_tmp40C + 16},(
void*)& Cyc__genrep_129};static char _tmp40D[14]="Instantiate_e";static struct
_tuple5 Cyc__gentuple_896={14,(struct _dyneither_ptr){_tmp40D,_tmp40D,_tmp40D + 14},(
void*)& Cyc__genrep_717};static char _tmp40E[7]="Cast_e";static struct _tuple5 Cyc__gentuple_897={
15,(struct _dyneither_ptr){_tmp40E,_tmp40E,_tmp40E + 7},(void*)& Cyc__genrep_704};
static char _tmp40F[10]="Address_e";static struct _tuple5 Cyc__gentuple_898={16,(
struct _dyneither_ptr){_tmp40F,_tmp40F,_tmp40F + 10},(void*)& Cyc__genrep_129};
static char _tmp410[6]="New_e";static struct _tuple5 Cyc__gentuple_899={17,(struct
_dyneither_ptr){_tmp410,_tmp410,_tmp410 + 6},(void*)& Cyc__genrep_699};static char
_tmp411[12]="Sizeoftyp_e";static struct _tuple5 Cyc__gentuple_900={18,(struct
_dyneither_ptr){_tmp411,_tmp411,_tmp411 + 12},(void*)& Cyc__genrep_116};static char
_tmp412[12]="Sizeofexp_e";static struct _tuple5 Cyc__gentuple_901={19,(struct
_dyneither_ptr){_tmp412,_tmp412,_tmp412 + 12},(void*)& Cyc__genrep_129};static char
_tmp413[11]="Offsetof_e";static struct _tuple5 Cyc__gentuple_902={20,(struct
_dyneither_ptr){_tmp413,_tmp413,_tmp413 + 11},(void*)& Cyc__genrep_686};static char
_tmp414[9]="Gentyp_e";static struct _tuple5 Cyc__gentuple_903={21,(struct
_dyneither_ptr){_tmp414,_tmp414,_tmp414 + 9},(void*)& Cyc__genrep_681};static char
_tmp415[8]="Deref_e";static struct _tuple5 Cyc__gentuple_904={22,(struct
_dyneither_ptr){_tmp415,_tmp415,_tmp415 + 8},(void*)& Cyc__genrep_129};static char
_tmp416[13]="AggrMember_e";static struct _tuple5 Cyc__gentuple_905={23,(struct
_dyneither_ptr){_tmp416,_tmp416,_tmp416 + 13},(void*)& Cyc__genrep_674};static char
_tmp417[12]="AggrArrow_e";static struct _tuple5 Cyc__gentuple_906={24,(struct
_dyneither_ptr){_tmp417,_tmp417,_tmp417 + 12},(void*)& Cyc__genrep_674};static char
_tmp418[12]="Subscript_e";static struct _tuple5 Cyc__gentuple_907={25,(struct
_dyneither_ptr){_tmp418,_tmp418,_tmp418 + 12},(void*)& Cyc__genrep_594};static char
_tmp419[8]="Tuple_e";static struct _tuple5 Cyc__gentuple_908={26,(struct
_dyneither_ptr){_tmp419,_tmp419,_tmp419 + 8},(void*)& Cyc__genrep_670};static char
_tmp41A[14]="CompoundLit_e";static struct _tuple5 Cyc__gentuple_909={27,(struct
_dyneither_ptr){_tmp41A,_tmp41A,_tmp41A + 14},(void*)& Cyc__genrep_655};static char
_tmp41B[8]="Array_e";static struct _tuple5 Cyc__gentuple_910={28,(struct
_dyneither_ptr){_tmp41B,_tmp41B,_tmp41B + 8},(void*)& Cyc__genrep_651};static char
_tmp41C[16]="Comprehension_e";static struct _tuple5 Cyc__gentuple_911={29,(struct
_dyneither_ptr){_tmp41C,_tmp41C,_tmp41C + 16},(void*)& Cyc__genrep_644};static char
_tmp41D[12]="Aggregate_e";static struct _tuple5 Cyc__gentuple_912={30,(struct
_dyneither_ptr){_tmp41D,_tmp41D,_tmp41D + 12},(void*)& Cyc__genrep_636};static char
_tmp41E[13]="AnonStruct_e";static struct _tuple5 Cyc__gentuple_913={31,(struct
_dyneither_ptr){_tmp41E,_tmp41E,_tmp41E + 13},(void*)& Cyc__genrep_631};static char
_tmp41F[11]="Datatype_e";static struct _tuple5 Cyc__gentuple_914={32,(struct
_dyneither_ptr){_tmp41F,_tmp41F,_tmp41F + 11},(void*)& Cyc__genrep_625};static char
_tmp420[7]="Enum_e";static struct _tuple5 Cyc__gentuple_915={33,(struct
_dyneither_ptr){_tmp420,_tmp420,_tmp420 + 7},(void*)& Cyc__genrep_618};static char
_tmp421[11]="AnonEnum_e";static struct _tuple5 Cyc__gentuple_916={34,(struct
_dyneither_ptr){_tmp421,_tmp421,_tmp421 + 11},(void*)& Cyc__genrep_611};static char
_tmp422[9]="Malloc_e";static struct _tuple5 Cyc__gentuple_917={35,(struct
_dyneither_ptr){_tmp422,_tmp422,_tmp422 + 9},(void*)& Cyc__genrep_599};static char
_tmp423[7]="Swap_e";static struct _tuple5 Cyc__gentuple_918={36,(struct
_dyneither_ptr){_tmp423,_tmp423,_tmp423 + 7},(void*)& Cyc__genrep_594};static char
_tmp424[16]="UnresolvedMem_e";static struct _tuple5 Cyc__gentuple_919={37,(struct
_dyneither_ptr){_tmp424,_tmp424,_tmp424 + 16},(void*)& Cyc__genrep_575};static char
_tmp425[10]="StmtExp_e";static struct _tuple5 Cyc__gentuple_920={38,(struct
_dyneither_ptr){_tmp425,_tmp425,_tmp425 + 10},(void*)& Cyc__genrep_133};static char
_tmp426[10]="Valueof_e";static struct _tuple5 Cyc__gentuple_921={39,(struct
_dyneither_ptr){_tmp426,_tmp426,_tmp426 + 10},(void*)& Cyc__genrep_116};static
struct _tuple5*Cyc__genarr_922[40]={& Cyc__gentuple_882,& Cyc__gentuple_883,& Cyc__gentuple_884,&
Cyc__gentuple_885,& Cyc__gentuple_886,& Cyc__gentuple_887,& Cyc__gentuple_888,& Cyc__gentuple_889,&
Cyc__gentuple_890,& Cyc__gentuple_891,& Cyc__gentuple_892,& Cyc__gentuple_893,& Cyc__gentuple_894,&
Cyc__gentuple_895,& Cyc__gentuple_896,& Cyc__gentuple_897,& Cyc__gentuple_898,& Cyc__gentuple_899,&
Cyc__gentuple_900,& Cyc__gentuple_901,& Cyc__gentuple_902,& Cyc__gentuple_903,& Cyc__gentuple_904,&
Cyc__gentuple_905,& Cyc__gentuple_906,& Cyc__gentuple_907,& Cyc__gentuple_908,& Cyc__gentuple_909,&
Cyc__gentuple_910,& Cyc__gentuple_911,& Cyc__gentuple_912,& Cyc__gentuple_913,& Cyc__gentuple_914,&
Cyc__gentuple_915,& Cyc__gentuple_916,& Cyc__gentuple_917,& Cyc__gentuple_918,& Cyc__gentuple_919,&
Cyc__gentuple_920,& Cyc__gentuple_921};static char _tmp428[8]="Raw_exp";struct Cyc_Typerep_TUnion_struct
Cyc_Absyn_raw_exp_t_rep={5,(struct _dyneither_ptr){_tmp428,_tmp428,_tmp428 + 8},{(
void*)((struct _tuple7**)Cyc__genarr_132),(void*)((struct _tuple7**)Cyc__genarr_132),(
void*)((struct _tuple7**)Cyc__genarr_132 + 0)},{(void*)((struct _tuple5**)Cyc__genarr_922),(
void*)((struct _tuple5**)Cyc__genarr_922),(void*)((struct _tuple5**)Cyc__genarr_922
+ 40)}};static char _tmp429[4]="Exp";static struct _dyneither_ptr Cyc__genname_928=(
struct _dyneither_ptr){_tmp429,_tmp429,_tmp429 + 4};static char _tmp42A[5]="topt";
static struct _tuple5 Cyc__gentuple_923={offsetof(struct Cyc_Absyn_Exp,topt),(struct
_dyneither_ptr){_tmp42A,_tmp42A,_tmp42A + 5},(void*)& Cyc__genrep_110};static char
_tmp42B[2]="r";static struct _tuple5 Cyc__gentuple_924={offsetof(struct Cyc_Absyn_Exp,r),(
struct _dyneither_ptr){_tmp42B,_tmp42B,_tmp42B + 2},(void*)& Cyc_Absyn_raw_exp_t_rep};
static char _tmp42C[4]="loc";static struct _tuple5 Cyc__gentuple_925={offsetof(struct
Cyc_Absyn_Exp,loc),(struct _dyneither_ptr){_tmp42C,_tmp42C,_tmp42C + 4},(void*)&
Cyc__genrep_2};static char _tmp42D[6]="annot";static struct _tuple5 Cyc__gentuple_926={
offsetof(struct Cyc_Absyn_Exp,annot),(struct _dyneither_ptr){_tmp42D,_tmp42D,
_tmp42D + 6},(void*)& Cyc_Absyn_absyn_annot_t_rep};static struct _tuple5*Cyc__genarr_927[
4]={& Cyc__gentuple_923,& Cyc__gentuple_924,& Cyc__gentuple_925,& Cyc__gentuple_926};
struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Exp_rep={3,(struct _dyneither_ptr*)&
Cyc__genname_928,sizeof(struct Cyc_Absyn_Exp),{(void*)((struct _tuple5**)Cyc__genarr_927),(
void*)((struct _tuple5**)Cyc__genarr_927),(void*)((struct _tuple5**)Cyc__genarr_927
+ 4)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_145={1,1,(void*)((void*)&
Cyc_struct_Absyn_Exp_rep)};static char _tmp430[10]="ArrayInfo";static struct
_dyneither_ptr Cyc__genname_1006=(struct _dyneither_ptr){_tmp430,_tmp430,_tmp430 + 
10};static char _tmp431[9]="elt_type";static struct _tuple5 Cyc__gentuple_1000={
offsetof(struct Cyc_Absyn_ArrayInfo,elt_type),(struct _dyneither_ptr){_tmp431,
_tmp431,_tmp431 + 9},(void*)((void*)& Cyc_Absyn_type_t_rep)};static char _tmp432[3]="tq";
static struct _tuple5 Cyc__gentuple_1001={offsetof(struct Cyc_Absyn_ArrayInfo,tq),(
struct _dyneither_ptr){_tmp432,_tmp432,_tmp432 + 3},(void*)& Cyc_Absyn_tqual_t_rep};
static char _tmp433[9]="num_elts";static struct _tuple5 Cyc__gentuple_1002={offsetof(
struct Cyc_Absyn_ArrayInfo,num_elts),(struct _dyneither_ptr){_tmp433,_tmp433,
_tmp433 + 9},(void*)& Cyc__genrep_145};static char _tmp434[10]="zero_term";static
struct _tuple5 Cyc__gentuple_1003={offsetof(struct Cyc_Absyn_ArrayInfo,zero_term),(
struct _dyneither_ptr){_tmp434,_tmp434,_tmp434 + 10},(void*)& Cyc__genrep_987};
static char _tmp435[7]="zt_loc";static struct _tuple5 Cyc__gentuple_1004={offsetof(
struct Cyc_Absyn_ArrayInfo,zt_loc),(struct _dyneither_ptr){_tmp435,_tmp435,_tmp435
+ 7},(void*)& Cyc__genrep_2};static struct _tuple5*Cyc__genarr_1005[5]={& Cyc__gentuple_1000,&
Cyc__gentuple_1001,& Cyc__gentuple_1002,& Cyc__gentuple_1003,& Cyc__gentuple_1004};
struct Cyc_Typerep_Struct_struct Cyc_Absyn_array_info_t_rep={3,(struct
_dyneither_ptr*)& Cyc__genname_1006,sizeof(struct Cyc_Absyn_ArrayInfo),{(void*)((
struct _tuple5**)Cyc__genarr_1005),(void*)((struct _tuple5**)Cyc__genarr_1005),(
void*)((struct _tuple5**)Cyc__genarr_1005 + 5)}};struct _tuple83{unsigned int f1;
struct Cyc_Absyn_ArrayInfo f2;};static struct _tuple6 Cyc__gentuple_1007={offsetof(
struct _tuple83,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_1008={
offsetof(struct _tuple83,f2),(void*)& Cyc_Absyn_array_info_t_rep};static struct
_tuple6*Cyc__genarr_1009[2]={& Cyc__gentuple_1007,& Cyc__gentuple_1008};static
struct Cyc_Typerep_Tuple_struct Cyc__genrep_986={4,sizeof(struct _tuple83),{(void*)((
struct _tuple6**)Cyc__genarr_1009),(void*)((struct _tuple6**)Cyc__genarr_1009),(
void*)((struct _tuple6**)Cyc__genarr_1009 + 2)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_967;extern struct Cyc_Typerep_Struct_struct Cyc_Absyn_fn_info_t_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_968;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List060Core_opt_t0Absyn_var_t46H24Absyn_tqual_t4Absyn_type_t1_44099_6H2_rep;
static char _tmp438[5]="List";static struct _dyneither_ptr Cyc__genname_972=(struct
_dyneither_ptr){_tmp438,_tmp438,_tmp438 + 5};static char _tmp439[3]="hd";static
struct _tuple5 Cyc__gentuple_969={offsetof(struct Cyc_List_List,hd),(struct
_dyneither_ptr){_tmp439,_tmp439,_tmp439 + 3},(void*)& Cyc__genrep_656};static char
_tmp43A[3]="tl";static struct _tuple5 Cyc__gentuple_970={offsetof(struct Cyc_List_List,tl),(
struct _dyneither_ptr){_tmp43A,_tmp43A,_tmp43A + 3},(void*)& Cyc__genrep_968};
static struct _tuple5*Cyc__genarr_971[2]={& Cyc__gentuple_969,& Cyc__gentuple_970};
struct Cyc_Typerep_Struct_struct Cyc_struct_List_List060Core_opt_t0Absyn_var_t46H24Absyn_tqual_t4Absyn_type_t1_44099_6H2_rep={
3,(struct _dyneither_ptr*)& Cyc__genname_972,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_971),(void*)((struct _tuple5**)Cyc__genarr_971),(void*)((
struct _tuple5**)Cyc__genarr_971 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_968={
1,1,(void*)((void*)& Cyc_struct_List_List060Core_opt_t0Absyn_var_t46H24Absyn_tqual_t4Absyn_type_t1_44099_6H2_rep)};
static char _tmp43D[7]="FnInfo";static struct _dyneither_ptr Cyc__genname_982=(struct
_dyneither_ptr){_tmp43D,_tmp43D,_tmp43D + 7};static char _tmp43E[6]="tvars";static
struct _tuple5 Cyc__gentuple_973={offsetof(struct Cyc_Absyn_FnInfo,tvars),(struct
_dyneither_ptr){_tmp43E,_tmp43E,_tmp43E + 6},(void*)& Cyc__genrep_273};static char
_tmp43F[7]="effect";static struct _tuple5 Cyc__gentuple_974={offsetof(struct Cyc_Absyn_FnInfo,effect),(
struct _dyneither_ptr){_tmp43F,_tmp43F,_tmp43F + 7},(void*)& Cyc__genrep_110};
static char _tmp440[8]="ret_typ";static struct _tuple5 Cyc__gentuple_975={offsetof(
struct Cyc_Absyn_FnInfo,ret_typ),(struct _dyneither_ptr){_tmp440,_tmp440,_tmp440 + 
8},(void*)((void*)& Cyc_Absyn_type_t_rep)};static char _tmp441[5]="args";static
struct _tuple5 Cyc__gentuple_976={offsetof(struct Cyc_Absyn_FnInfo,args),(struct
_dyneither_ptr){_tmp441,_tmp441,_tmp441 + 5},(void*)& Cyc__genrep_968};static char
_tmp442[10]="c_varargs";static struct _tuple5 Cyc__gentuple_977={offsetof(struct Cyc_Absyn_FnInfo,c_varargs),(
struct _dyneither_ptr){_tmp442,_tmp442,_tmp442 + 10},(void*)& Cyc__genrep_10};
static char _tmp443[12]="cyc_varargs";static struct _tuple5 Cyc__gentuple_978={
offsetof(struct Cyc_Absyn_FnInfo,cyc_varargs),(struct _dyneither_ptr){_tmp443,
_tmp443,_tmp443 + 12},(void*)& Cyc__genrep_810};static char _tmp444[7]="rgn_po";
static struct _tuple5 Cyc__gentuple_979={offsetof(struct Cyc_Absyn_FnInfo,rgn_po),(
struct _dyneither_ptr){_tmp444,_tmp444,_tmp444 + 7},(void*)& Cyc__genrep_372};
static char _tmp445[11]="attributes";static struct _tuple5 Cyc__gentuple_980={
offsetof(struct Cyc_Absyn_FnInfo,attributes),(struct _dyneither_ptr){_tmp445,
_tmp445,_tmp445 + 11},(void*)& Cyc__genrep_62};static struct _tuple5*Cyc__genarr_981[
8]={& Cyc__gentuple_973,& Cyc__gentuple_974,& Cyc__gentuple_975,& Cyc__gentuple_976,&
Cyc__gentuple_977,& Cyc__gentuple_978,& Cyc__gentuple_979,& Cyc__gentuple_980};
struct Cyc_Typerep_Struct_struct Cyc_Absyn_fn_info_t_rep={3,(struct _dyneither_ptr*)&
Cyc__genname_982,sizeof(struct Cyc_Absyn_FnInfo),{(void*)((struct _tuple5**)Cyc__genarr_981),(
void*)((struct _tuple5**)Cyc__genarr_981),(void*)((struct _tuple5**)Cyc__genarr_981
+ 8)}};struct _tuple84{unsigned int f1;struct Cyc_Absyn_FnInfo f2;};static struct
_tuple6 Cyc__gentuple_983={offsetof(struct _tuple84,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_984={offsetof(struct _tuple84,f2),(void*)& Cyc_Absyn_fn_info_t_rep};
static struct _tuple6*Cyc__genarr_985[2]={& Cyc__gentuple_983,& Cyc__gentuple_984};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_967={4,sizeof(struct _tuple84),{(
void*)((struct _tuple6**)Cyc__genarr_985),(void*)((struct _tuple6**)Cyc__genarr_985),(
void*)((struct _tuple6**)Cyc__genarr_985 + 2)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_963;static struct _tuple6 Cyc__gentuple_964={offsetof(struct _tuple14,f1),(
void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_965={offsetof(struct
_tuple14,f2),(void*)& Cyc__genrep_247};static struct _tuple6*Cyc__genarr_966[2]={&
Cyc__gentuple_964,& Cyc__gentuple_965};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_963={
4,sizeof(struct _tuple14),{(void*)((struct _tuple6**)Cyc__genarr_966),(void*)((
struct _tuple6**)Cyc__genarr_966),(void*)((struct _tuple6**)Cyc__genarr_966 + 2)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_959;struct _tuple85{unsigned int
f1;struct Cyc_Absyn_AggrInfo f2;};static struct _tuple6 Cyc__gentuple_960={offsetof(
struct _tuple85,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_961={
offsetof(struct _tuple85,f2),(void*)& Cyc_Absyn_aggr_info_t_rep};static struct
_tuple6*Cyc__genarr_962[2]={& Cyc__gentuple_960,& Cyc__gentuple_961};static struct
Cyc_Typerep_Tuple_struct Cyc__genrep_959={4,sizeof(struct _tuple85),{(void*)((
struct _tuple6**)Cyc__genarr_962),(void*)((struct _tuple6**)Cyc__genarr_962),(void*)((
struct _tuple6**)Cyc__genarr_962 + 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_954;
static struct _tuple6 Cyc__gentuple_955={offsetof(struct _tuple65,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_956={offsetof(struct _tuple65,f2),(void*)& Cyc_Absyn_aggr_kind_t_rep};
static struct _tuple6 Cyc__gentuple_957={offsetof(struct _tuple65,f3),(void*)& Cyc__genrep_359};
static struct _tuple6*Cyc__genarr_958[3]={& Cyc__gentuple_955,& Cyc__gentuple_956,&
Cyc__gentuple_957};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_954={4,
sizeof(struct _tuple65),{(void*)((struct _tuple6**)Cyc__genarr_958),(void*)((
struct _tuple6**)Cyc__genarr_958),(void*)((struct _tuple6**)Cyc__genarr_958 + 3)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_949;struct _tuple86{unsigned int
f1;struct _tuple0*f2;struct Cyc_Absyn_Enumdecl*f3;};static struct _tuple6 Cyc__gentuple_950={
offsetof(struct _tuple86,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_951={
offsetof(struct _tuple86,f2),(void*)& Cyc__genrep_11};static struct _tuple6 Cyc__gentuple_952={
offsetof(struct _tuple86,f3),(void*)& Cyc__genrep_619};static struct _tuple6*Cyc__genarr_953[
3]={& Cyc__gentuple_950,& Cyc__gentuple_951,& Cyc__gentuple_952};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_949={4,sizeof(struct _tuple86),{(void*)((struct _tuple6**)Cyc__genarr_953),(
void*)((struct _tuple6**)Cyc__genarr_953),(void*)((struct _tuple6**)Cyc__genarr_953
+ 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_945;static struct _tuple6
Cyc__gentuple_946={offsetof(struct _tuple14,f1),(void*)& Cyc__genrep_24};static
struct _tuple6 Cyc__gentuple_947={offsetof(struct _tuple14,f2),(void*)& Cyc__genrep_214};
static struct _tuple6*Cyc__genarr_948[2]={& Cyc__gentuple_946,& Cyc__gentuple_947};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_945={4,sizeof(struct _tuple14),{(
void*)((struct _tuple6**)Cyc__genarr_948),(void*)((struct _tuple6**)Cyc__genarr_948),(
void*)((struct _tuple6**)Cyc__genarr_948 + 2)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_940;static struct _tuple6 Cyc__gentuple_941={offsetof(struct _tuple28,f1),(
void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_942={offsetof(struct
_tuple28,f2),(void*)((void*)& Cyc_Absyn_type_t_rep)};static struct _tuple6 Cyc__gentuple_943={
offsetof(struct _tuple28,f3),(void*)((void*)& Cyc_Absyn_type_t_rep)};static struct
_tuple6*Cyc__genarr_944[3]={& Cyc__gentuple_941,& Cyc__gentuple_942,& Cyc__gentuple_943};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_940={4,sizeof(struct _tuple28),{(
void*)((struct _tuple6**)Cyc__genarr_944),(void*)((struct _tuple6**)Cyc__genarr_944),(
void*)((struct _tuple6**)Cyc__genarr_944 + 3)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_932;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_933;extern
struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Typedefdecl_rep;static char
_tmp44E[12]="Typedefdecl";static struct _dyneither_ptr Cyc__genname_1153=(struct
_dyneither_ptr){_tmp44E,_tmp44E,_tmp44E + 12};static char _tmp44F[5]="name";static
struct _tuple5 Cyc__gentuple_1146={offsetof(struct Cyc_Absyn_Typedefdecl,name),(
struct _dyneither_ptr){_tmp44F,_tmp44F,_tmp44F + 5},(void*)& Cyc__genrep_11};static
char _tmp450[3]="tq";static struct _tuple5 Cyc__gentuple_1147={offsetof(struct Cyc_Absyn_Typedefdecl,tq),(
struct _dyneither_ptr){_tmp450,_tmp450,_tmp450 + 3},(void*)& Cyc_Absyn_tqual_t_rep};
static char _tmp451[4]="tvs";static struct _tuple5 Cyc__gentuple_1148={offsetof(
struct Cyc_Absyn_Typedefdecl,tvs),(struct _dyneither_ptr){_tmp451,_tmp451,_tmp451 + 
4},(void*)& Cyc__genrep_273};static char _tmp452[5]="kind";static struct _tuple5 Cyc__gentuple_1149={
offsetof(struct Cyc_Absyn_Typedefdecl,kind),(struct _dyneither_ptr){_tmp452,
_tmp452,_tmp452 + 5},(void*)& Cyc__genrep_1110};static char _tmp453[5]="defn";static
struct _tuple5 Cyc__gentuple_1150={offsetof(struct Cyc_Absyn_Typedefdecl,defn),(
struct _dyneither_ptr){_tmp453,_tmp453,_tmp453 + 5},(void*)& Cyc__genrep_110};
static char _tmp454[5]="atts";static struct _tuple5 Cyc__gentuple_1151={offsetof(
struct Cyc_Absyn_Typedefdecl,atts),(struct _dyneither_ptr){_tmp454,_tmp454,_tmp454
+ 5},(void*)& Cyc__genrep_62};static struct _tuple5*Cyc__genarr_1152[6]={& Cyc__gentuple_1146,&
Cyc__gentuple_1147,& Cyc__gentuple_1148,& Cyc__gentuple_1149,& Cyc__gentuple_1150,&
Cyc__gentuple_1151};struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Typedefdecl_rep={
3,(struct _dyneither_ptr*)& Cyc__genname_1153,sizeof(struct Cyc_Absyn_Typedefdecl),{(
void*)((struct _tuple5**)Cyc__genarr_1152),(void*)((struct _tuple5**)Cyc__genarr_1152),(
void*)((struct _tuple5**)Cyc__genarr_1152 + 6)}};static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_933={1,1,(void*)((void*)& Cyc_struct_Absyn_Typedefdecl_rep)};struct
_tuple87{unsigned int f1;struct _tuple0*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Typedefdecl*
f4;void**f5;};static struct _tuple6 Cyc__gentuple_934={offsetof(struct _tuple87,f1),(
void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_935={offsetof(struct
_tuple87,f2),(void*)& Cyc__genrep_11};static struct _tuple6 Cyc__gentuple_936={
offsetof(struct _tuple87,f3),(void*)& Cyc__genrep_121};static struct _tuple6 Cyc__gentuple_937={
offsetof(struct _tuple87,f4),(void*)& Cyc__genrep_933};static struct _tuple6 Cyc__gentuple_938={
offsetof(struct _tuple87,f5),(void*)& Cyc__genrep_600};static struct _tuple6*Cyc__genarr_939[
5]={& Cyc__gentuple_934,& Cyc__gentuple_935,& Cyc__gentuple_936,& Cyc__gentuple_937,&
Cyc__gentuple_938};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_932={4,
sizeof(struct _tuple87),{(void*)((struct _tuple6**)Cyc__genarr_939),(void*)((
struct _tuple6**)Cyc__genarr_939),(void*)((struct _tuple6**)Cyc__genarr_939 + 5)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_120;static struct _tuple6 Cyc__gentuple_126={
offsetof(struct _tuple14,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_127={
offsetof(struct _tuple14,f2),(void*)& Cyc__genrep_121};static struct _tuple6*Cyc__genarr_128[
2]={& Cyc__gentuple_126,& Cyc__gentuple_127};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_120={4,sizeof(struct _tuple14),{(void*)((struct _tuple6**)Cyc__genarr_128),(
void*)((struct _tuple6**)Cyc__genarr_128),(void*)((struct _tuple6**)Cyc__genarr_128
+ 2)}};static char _tmp459[9]="VoidType";static struct _tuple7 Cyc__gentuple_111={0,(
struct _dyneither_ptr){_tmp459,_tmp459,_tmp459 + 9}};static char _tmp45A[10]="FloatType";
static struct _tuple7 Cyc__gentuple_112={1,(struct _dyneither_ptr){_tmp45A,_tmp45A,
_tmp45A + 10}};static char _tmp45B[8]="HeapRgn";static struct _tuple7 Cyc__gentuple_113={
2,(struct _dyneither_ptr){_tmp45B,_tmp45B,_tmp45B + 8}};static char _tmp45C[10]="UniqueRgn";
static struct _tuple7 Cyc__gentuple_114={3,(struct _dyneither_ptr){_tmp45C,_tmp45C,
_tmp45C + 10}};static struct _tuple7*Cyc__genarr_115[4]={& Cyc__gentuple_111,& Cyc__gentuple_112,&
Cyc__gentuple_113,& Cyc__gentuple_114};static char _tmp45D[5]="Evar";static struct
_tuple5 Cyc__gentuple_1120={0,(struct _dyneither_ptr){_tmp45D,_tmp45D,_tmp45D + 5},(
void*)& Cyc__genrep_1105};static char _tmp45E[8]="VarType";static struct _tuple5 Cyc__gentuple_1121={
1,(struct _dyneither_ptr){_tmp45E,_tmp45E,_tmp45E + 8},(void*)& Cyc__genrep_1101};
static char _tmp45F[13]="DatatypeType";static struct _tuple5 Cyc__gentuple_1122={2,(
struct _dyneither_ptr){_tmp45F,_tmp45F,_tmp45F + 13},(void*)& Cyc__genrep_1074};
static char _tmp460[18]="DatatypeFieldType";static struct _tuple5 Cyc__gentuple_1123={
3,(struct _dyneither_ptr){_tmp460,_tmp460,_tmp460 + 18},(void*)& Cyc__genrep_1048};
static char _tmp461[12]="PointerType";static struct _tuple5 Cyc__gentuple_1124={4,(
struct _dyneither_ptr){_tmp461,_tmp461,_tmp461 + 12},(void*)& Cyc__genrep_1022};
static char _tmp462[8]="IntType";static struct _tuple5 Cyc__gentuple_1125={5,(struct
_dyneither_ptr){_tmp462,_tmp462,_tmp462 + 8},(void*)& Cyc__genrep_1010};static char
_tmp463[11]="DoubleType";static struct _tuple5 Cyc__gentuple_1126={6,(struct
_dyneither_ptr){_tmp463,_tmp463,_tmp463 + 11},(void*)& Cyc__genrep_85};static char
_tmp464[10]="ArrayType";static struct _tuple5 Cyc__gentuple_1127={7,(struct
_dyneither_ptr){_tmp464,_tmp464,_tmp464 + 10},(void*)& Cyc__genrep_986};static char
_tmp465[7]="FnType";static struct _tuple5 Cyc__gentuple_1128={8,(struct
_dyneither_ptr){_tmp465,_tmp465,_tmp465 + 7},(void*)& Cyc__genrep_967};static char
_tmp466[10]="TupleType";static struct _tuple5 Cyc__gentuple_1129={9,(struct
_dyneither_ptr){_tmp466,_tmp466,_tmp466 + 10},(void*)& Cyc__genrep_963};static char
_tmp467[9]="AggrType";static struct _tuple5 Cyc__gentuple_1130={10,(struct
_dyneither_ptr){_tmp467,_tmp467,_tmp467 + 9},(void*)& Cyc__genrep_959};static char
_tmp468[13]="AnonAggrType";static struct _tuple5 Cyc__gentuple_1131={11,(struct
_dyneither_ptr){_tmp468,_tmp468,_tmp468 + 13},(void*)& Cyc__genrep_954};static char
_tmp469[9]="EnumType";static struct _tuple5 Cyc__gentuple_1132={12,(struct
_dyneither_ptr){_tmp469,_tmp469,_tmp469 + 9},(void*)& Cyc__genrep_949};static char
_tmp46A[13]="AnonEnumType";static struct _tuple5 Cyc__gentuple_1133={13,(struct
_dyneither_ptr){_tmp46A,_tmp46A,_tmp46A + 13},(void*)& Cyc__genrep_945};static char
_tmp46B[14]="RgnHandleType";static struct _tuple5 Cyc__gentuple_1134={14,(struct
_dyneither_ptr){_tmp46B,_tmp46B,_tmp46B + 14},(void*)& Cyc__genrep_116};static char
_tmp46C[11]="DynRgnType";static struct _tuple5 Cyc__gentuple_1135={15,(struct
_dyneither_ptr){_tmp46C,_tmp46C,_tmp46C + 11},(void*)& Cyc__genrep_940};static char
_tmp46D[12]="TypedefType";static struct _tuple5 Cyc__gentuple_1136={16,(struct
_dyneither_ptr){_tmp46D,_tmp46D,_tmp46D + 12},(void*)& Cyc__genrep_932};static char
_tmp46E[12]="ValueofType";static struct _tuple5 Cyc__gentuple_1137={17,(struct
_dyneither_ptr){_tmp46E,_tmp46E,_tmp46E + 12},(void*)& Cyc__genrep_129};static char
_tmp46F[8]="TagType";static struct _tuple5 Cyc__gentuple_1138={18,(struct
_dyneither_ptr){_tmp46F,_tmp46F,_tmp46F + 8},(void*)& Cyc__genrep_116};static char
_tmp470[10]="AccessEff";static struct _tuple5 Cyc__gentuple_1139={19,(struct
_dyneither_ptr){_tmp470,_tmp470,_tmp470 + 10},(void*)& Cyc__genrep_116};static char
_tmp471[8]="JoinEff";static struct _tuple5 Cyc__gentuple_1140={20,(struct
_dyneither_ptr){_tmp471,_tmp471,_tmp471 + 8},(void*)& Cyc__genrep_120};static char
_tmp472[8]="RgnsEff";static struct _tuple5 Cyc__gentuple_1141={21,(struct
_dyneither_ptr){_tmp472,_tmp472,_tmp472 + 8},(void*)& Cyc__genrep_116};static
struct _tuple5*Cyc__genarr_1142[22]={& Cyc__gentuple_1120,& Cyc__gentuple_1121,& Cyc__gentuple_1122,&
Cyc__gentuple_1123,& Cyc__gentuple_1124,& Cyc__gentuple_1125,& Cyc__gentuple_1126,&
Cyc__gentuple_1127,& Cyc__gentuple_1128,& Cyc__gentuple_1129,& Cyc__gentuple_1130,&
Cyc__gentuple_1131,& Cyc__gentuple_1132,& Cyc__gentuple_1133,& Cyc__gentuple_1134,&
Cyc__gentuple_1135,& Cyc__gentuple_1136,& Cyc__gentuple_1137,& Cyc__gentuple_1138,&
Cyc__gentuple_1139,& Cyc__gentuple_1140,& Cyc__gentuple_1141};static char _tmp474[5]="Type";
struct Cyc_Typerep_TUnion_struct Cyc_Absyn_type_t_rep={5,(struct _dyneither_ptr){
_tmp474,_tmp474,_tmp474 + 5},{(void*)((struct _tuple7**)Cyc__genarr_115),(void*)((
struct _tuple7**)Cyc__genarr_115),(void*)((struct _tuple7**)Cyc__genarr_115 + 4)},{(
void*)((struct _tuple5**)Cyc__genarr_1142),(void*)((struct _tuple5**)Cyc__genarr_1142),(
void*)((struct _tuple5**)Cyc__genarr_1142 + 22)}};static char _tmp475[8]="Vardecl";
static struct _dyneither_ptr Cyc__genname_173=(struct _dyneither_ptr){_tmp475,
_tmp475,_tmp475 + 8};static char _tmp476[3]="sc";static struct _tuple5 Cyc__gentuple_164={
offsetof(struct Cyc_Absyn_Vardecl,sc),(struct _dyneither_ptr){_tmp476,_tmp476,
_tmp476 + 3},(void*)& Cyc_Absyn_scope_t_rep};static char _tmp477[5]="name";static
struct _tuple5 Cyc__gentuple_165={offsetof(struct Cyc_Absyn_Vardecl,name),(struct
_dyneither_ptr){_tmp477,_tmp477,_tmp477 + 5},(void*)& Cyc__genrep_11};static char
_tmp478[3]="tq";static struct _tuple5 Cyc__gentuple_166={offsetof(struct Cyc_Absyn_Vardecl,tq),(
struct _dyneither_ptr){_tmp478,_tmp478,_tmp478 + 3},(void*)& Cyc_Absyn_tqual_t_rep};
static char _tmp479[5]="type";static struct _tuple5 Cyc__gentuple_167={offsetof(
struct Cyc_Absyn_Vardecl,type),(struct _dyneither_ptr){_tmp479,_tmp479,_tmp479 + 5},(
void*)((void*)& Cyc_Absyn_type_t_rep)};static char _tmp47A[12]="initializer";static
struct _tuple5 Cyc__gentuple_168={offsetof(struct Cyc_Absyn_Vardecl,initializer),(
struct _dyneither_ptr){_tmp47A,_tmp47A,_tmp47A + 12},(void*)& Cyc__genrep_145};
static char _tmp47B[4]="rgn";static struct _tuple5 Cyc__gentuple_169={offsetof(struct
Cyc_Absyn_Vardecl,rgn),(struct _dyneither_ptr){_tmp47B,_tmp47B,_tmp47B + 4},(void*)&
Cyc__genrep_110};static char _tmp47C[11]="attributes";static struct _tuple5 Cyc__gentuple_170={
offsetof(struct Cyc_Absyn_Vardecl,attributes),(struct _dyneither_ptr){_tmp47C,
_tmp47C,_tmp47C + 11},(void*)& Cyc__genrep_62};static char _tmp47D[8]="escapes";
static struct _tuple5 Cyc__gentuple_171={offsetof(struct Cyc_Absyn_Vardecl,escapes),(
struct _dyneither_ptr){_tmp47D,_tmp47D,_tmp47D + 8},(void*)& Cyc__genrep_10};static
struct _tuple5*Cyc__genarr_172[8]={& Cyc__gentuple_164,& Cyc__gentuple_165,& Cyc__gentuple_166,&
Cyc__gentuple_167,& Cyc__gentuple_168,& Cyc__gentuple_169,& Cyc__gentuple_170,& Cyc__gentuple_171};
struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Vardecl_rep={3,(struct
_dyneither_ptr*)& Cyc__genname_173,sizeof(struct Cyc_Absyn_Vardecl),{(void*)((
struct _tuple5**)Cyc__genarr_172),(void*)((struct _tuple5**)Cyc__genarr_172),(void*)((
struct _tuple5**)Cyc__genarr_172 + 8)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_148={
1,1,(void*)((void*)& Cyc_struct_Absyn_Vardecl_rep)};struct _tuple88{unsigned int f1;
struct Cyc_Absyn_Vardecl*f2;};static struct _tuple6 Cyc__gentuple_804={offsetof(
struct _tuple88,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_805={
offsetof(struct _tuple88,f2),(void*)& Cyc__genrep_148};static struct _tuple6*Cyc__genarr_806[
2]={& Cyc__gentuple_804,& Cyc__gentuple_805};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_803={4,sizeof(struct _tuple88),{(void*)((struct _tuple6**)Cyc__genarr_806),(
void*)((struct _tuple6**)Cyc__genarr_806),(void*)((struct _tuple6**)Cyc__genarr_806
+ 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_1186;struct _tuple89{
unsigned int f1;struct Cyc_Absyn_Pat*f2;struct Cyc_Core_Opt*f3;struct Cyc_Absyn_Exp*
f4;};static struct _tuple6 Cyc__gentuple_1187={offsetof(struct _tuple89,f1),(void*)&
Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_1188={offsetof(struct _tuple89,f2),(
void*)& Cyc__genrep_181};static struct _tuple6 Cyc__gentuple_1189={offsetof(struct
_tuple89,f3),(void*)& Cyc__genrep_146};static struct _tuple6 Cyc__gentuple_1190={
offsetof(struct _tuple89,f4),(void*)& Cyc__genrep_130};static struct _tuple6*Cyc__genarr_1191[
4]={& Cyc__gentuple_1187,& Cyc__gentuple_1188,& Cyc__gentuple_1189,& Cyc__gentuple_1190};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_1186={4,sizeof(struct _tuple89),{(
void*)((struct _tuple6**)Cyc__genarr_1191),(void*)((struct _tuple6**)Cyc__genarr_1191),(
void*)((struct _tuple6**)Cyc__genarr_1191 + 4)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_1182;static struct _tuple6 Cyc__gentuple_1183={offsetof(struct _tuple14,f1),(
void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_1184={offsetof(struct
_tuple14,f2),(void*)& Cyc__genrep_147};static struct _tuple6*Cyc__genarr_1185[2]={&
Cyc__gentuple_1183,& Cyc__gentuple_1184};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_1182={
4,sizeof(struct _tuple14),{(void*)((struct _tuple6**)Cyc__genarr_1185),(void*)((
struct _tuple6**)Cyc__genarr_1185),(void*)((struct _tuple6**)Cyc__genarr_1185 + 2)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_1175;struct _tuple90{unsigned int
f1;struct Cyc_Absyn_Tvar*f2;struct Cyc_Absyn_Vardecl*f3;int f4;struct Cyc_Absyn_Exp*
f5;};static struct _tuple6 Cyc__gentuple_1176={offsetof(struct _tuple90,f1),(void*)&
Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_1177={offsetof(struct _tuple90,f2),(
void*)& Cyc__genrep_274};static struct _tuple6 Cyc__gentuple_1178={offsetof(struct
_tuple90,f3),(void*)& Cyc__genrep_148};static struct _tuple6 Cyc__gentuple_1179={
offsetof(struct _tuple90,f4),(void*)& Cyc__genrep_10};static struct _tuple6 Cyc__gentuple_1180={
offsetof(struct _tuple90,f5),(void*)& Cyc__genrep_145};static struct _tuple6*Cyc__genarr_1181[
5]={& Cyc__gentuple_1176,& Cyc__gentuple_1177,& Cyc__gentuple_1178,& Cyc__gentuple_1179,&
Cyc__gentuple_1180};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_1175={4,
sizeof(struct _tuple90),{(void*)((struct _tuple6**)Cyc__genarr_1181),(void*)((
struct _tuple6**)Cyc__genarr_1181),(void*)((struct _tuple6**)Cyc__genarr_1181 + 5)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_1169;struct _tuple91{unsigned int
f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Tvar*f3;struct Cyc_Absyn_Vardecl*f4;};
static struct _tuple6 Cyc__gentuple_1170={offsetof(struct _tuple91,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_1171={offsetof(struct _tuple91,f2),(void*)& Cyc__genrep_130};
static struct _tuple6 Cyc__gentuple_1172={offsetof(struct _tuple91,f3),(void*)& Cyc__genrep_274};
static struct _tuple6 Cyc__gentuple_1173={offsetof(struct _tuple91,f4),(void*)& Cyc__genrep_148};
static struct _tuple6*Cyc__genarr_1174[4]={& Cyc__gentuple_1170,& Cyc__gentuple_1171,&
Cyc__gentuple_1172,& Cyc__gentuple_1173};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_1169={
4,sizeof(struct _tuple91),{(void*)((struct _tuple6**)Cyc__genarr_1174),(void*)((
struct _tuple6**)Cyc__genarr_1174),(void*)((struct _tuple6**)Cyc__genarr_1174 + 4)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_1165;struct _tuple92{unsigned int
f1;struct Cyc_Absyn_Aggrdecl*f2;};static struct _tuple6 Cyc__gentuple_1166={
offsetof(struct _tuple92,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_1167={
offsetof(struct _tuple92,f2),(void*)((void*)& Cyc__genrep_357)};static struct
_tuple6*Cyc__genarr_1168[2]={& Cyc__gentuple_1166,& Cyc__gentuple_1167};static
struct Cyc_Typerep_Tuple_struct Cyc__genrep_1165={4,sizeof(struct _tuple92),{(void*)((
struct _tuple6**)Cyc__genarr_1168),(void*)((struct _tuple6**)Cyc__genarr_1168),(
void*)((struct _tuple6**)Cyc__genarr_1168 + 2)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_1161;struct _tuple93{unsigned int f1;struct Cyc_Absyn_Datatypedecl*f2;}
;static struct _tuple6 Cyc__gentuple_1162={offsetof(struct _tuple93,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_1163={offsetof(struct _tuple93,f2),(void*)((void*)&
Cyc__genrep_263)};static struct _tuple6*Cyc__genarr_1164[2]={& Cyc__gentuple_1162,&
Cyc__gentuple_1163};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_1161={4,
sizeof(struct _tuple93),{(void*)((struct _tuple6**)Cyc__genarr_1164),(void*)((
struct _tuple6**)Cyc__genarr_1164),(void*)((struct _tuple6**)Cyc__genarr_1164 + 2)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_1157;struct _tuple94{unsigned int
f1;struct Cyc_Absyn_Enumdecl*f2;};static struct _tuple6 Cyc__gentuple_1158={
offsetof(struct _tuple94,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_1159={
offsetof(struct _tuple94,f2),(void*)& Cyc__genrep_212};static struct _tuple6*Cyc__genarr_1160[
2]={& Cyc__gentuple_1158,& Cyc__gentuple_1159};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_1157={4,sizeof(struct _tuple94),{(void*)((struct _tuple6**)Cyc__genarr_1160),(
void*)((struct _tuple6**)Cyc__genarr_1160),(void*)((struct _tuple6**)Cyc__genarr_1160
+ 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_60;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_61;static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_61={1,1,(void*)((
void*)& Cyc_struct_Absyn_Typedefdecl_rep)};struct _tuple95{unsigned int f1;struct
Cyc_Absyn_Typedefdecl*f2;};static struct _tuple6 Cyc__gentuple_1154={offsetof(
struct _tuple95,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_1155={
offsetof(struct _tuple95,f2),(void*)& Cyc__genrep_61};static struct _tuple6*Cyc__genarr_1156[
2]={& Cyc__gentuple_1154,& Cyc__gentuple_1155};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_60={4,sizeof(struct _tuple95),{(void*)((struct _tuple6**)Cyc__genarr_1156),(
void*)((struct _tuple6**)Cyc__genarr_1156),(void*)((struct _tuple6**)Cyc__genarr_1156
+ 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_55;struct _tuple96{
unsigned int f1;struct _dyneither_ptr*f2;struct Cyc_List_List*f3;};static struct
_tuple6 Cyc__gentuple_56={offsetof(struct _tuple96,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_57={offsetof(struct _tuple96,f2),(void*)& Cyc__genrep_13};
static struct _tuple6 Cyc__gentuple_58={offsetof(struct _tuple96,f3),(void*)& Cyc__genrep_0};
static struct _tuple6*Cyc__genarr_59[3]={& Cyc__gentuple_56,& Cyc__gentuple_57,& Cyc__gentuple_58};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_55={4,sizeof(struct _tuple96),{(
void*)((struct _tuple6**)Cyc__genarr_59),(void*)((struct _tuple6**)Cyc__genarr_59),(
void*)((struct _tuple6**)Cyc__genarr_59 + 3)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_50;struct _tuple97{unsigned int f1;struct _tuple0*f2;struct Cyc_List_List*
f3;};static struct _tuple6 Cyc__gentuple_51={offsetof(struct _tuple97,f1),(void*)&
Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_52={offsetof(struct _tuple97,f2),(
void*)& Cyc__genrep_11};static struct _tuple6 Cyc__gentuple_53={offsetof(struct
_tuple97,f3),(void*)& Cyc__genrep_0};static struct _tuple6*Cyc__genarr_54[3]={& Cyc__gentuple_51,&
Cyc__gentuple_52,& Cyc__gentuple_53};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_50={
4,sizeof(struct _tuple97),{(void*)((struct _tuple6**)Cyc__genarr_54),(void*)((
struct _tuple6**)Cyc__genarr_54),(void*)((struct _tuple6**)Cyc__genarr_54 + 3)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_46;static struct _tuple6 Cyc__gentuple_47={
offsetof(struct _tuple14,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_48={
offsetof(struct _tuple14,f2),(void*)& Cyc__genrep_0};static struct _tuple6*Cyc__genarr_49[
2]={& Cyc__gentuple_47,& Cyc__gentuple_48};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_46={
4,sizeof(struct _tuple14),{(void*)((struct _tuple6**)Cyc__genarr_49),(void*)((
struct _tuple6**)Cyc__genarr_49),(void*)((struct _tuple6**)Cyc__genarr_49 + 2)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_6;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_7;extern struct Cyc_Typerep_Struct_struct Cyc_struct_List_List060Position_seg_t4Absyn_qvar_t4bool1_446H2_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_8;extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_9;struct _tuple98{struct Cyc_Position_Segment*f1;struct _tuple0*f2;int
f3;};static struct _tuple6 Cyc__gentuple_34={offsetof(struct _tuple98,f1),(void*)&
Cyc__genrep_2};static struct _tuple6 Cyc__gentuple_35={offsetof(struct _tuple98,f2),(
void*)& Cyc__genrep_11};static struct _tuple6 Cyc__gentuple_36={offsetof(struct
_tuple98,f3),(void*)& Cyc__genrep_10};static struct _tuple6*Cyc__genarr_37[3]={& Cyc__gentuple_34,&
Cyc__gentuple_35,& Cyc__gentuple_36};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_9={
4,sizeof(struct _tuple98),{(void*)((struct _tuple6**)Cyc__genarr_37),(void*)((
struct _tuple6**)Cyc__genarr_37),(void*)((struct _tuple6**)Cyc__genarr_37 + 3)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_8={1,1,(void*)((void*)& Cyc__genrep_9)};
static char _tmp48F[5]="List";static struct _dyneither_ptr Cyc__genname_41=(struct
_dyneither_ptr){_tmp48F,_tmp48F,_tmp48F + 5};static char _tmp490[3]="hd";static
struct _tuple5 Cyc__gentuple_38={offsetof(struct Cyc_List_List,hd),(struct
_dyneither_ptr){_tmp490,_tmp490,_tmp490 + 3},(void*)& Cyc__genrep_8};static char
_tmp491[3]="tl";static struct _tuple5 Cyc__gentuple_39={offsetof(struct Cyc_List_List,tl),(
struct _dyneither_ptr){_tmp491,_tmp491,_tmp491 + 3},(void*)& Cyc__genrep_7};static
struct _tuple5*Cyc__genarr_40[2]={& Cyc__gentuple_38,& Cyc__gentuple_39};struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List060Position_seg_t4Absyn_qvar_t4bool1_446H2_rep={3,(struct
_dyneither_ptr*)& Cyc__genname_41,sizeof(struct Cyc_List_List),{(void*)((struct
_tuple5**)Cyc__genarr_40),(void*)((struct _tuple5**)Cyc__genarr_40),(void*)((
struct _tuple5**)Cyc__genarr_40 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_7={
1,1,(void*)((void*)& Cyc_struct_List_List060Position_seg_t4Absyn_qvar_t4bool1_446H2_rep)};
struct _tuple99{unsigned int f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};
static struct _tuple6 Cyc__gentuple_42={offsetof(struct _tuple99,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_43={offsetof(struct _tuple99,f2),(void*)& Cyc__genrep_0};
static struct _tuple6 Cyc__gentuple_44={offsetof(struct _tuple99,f3),(void*)& Cyc__genrep_7};
static struct _tuple6*Cyc__genarr_45[3]={& Cyc__gentuple_42,& Cyc__gentuple_43,& Cyc__gentuple_44};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_6={4,sizeof(struct _tuple99),{(
void*)((struct _tuple6**)Cyc__genarr_45),(void*)((struct _tuple6**)Cyc__genarr_45),(
void*)((struct _tuple6**)Cyc__genarr_45 + 3)}};static char _tmp495[9]="Porton_d";
static struct _tuple7 Cyc__gentuple_3={0,(struct _dyneither_ptr){_tmp495,_tmp495,
_tmp495 + 9}};static char _tmp496[10]="Portoff_d";static struct _tuple7 Cyc__gentuple_4={
1,(struct _dyneither_ptr){_tmp496,_tmp496,_tmp496 + 10}};static struct _tuple7*Cyc__genarr_5[
2]={& Cyc__gentuple_3,& Cyc__gentuple_4};static char _tmp497[6]="Var_d";static struct
_tuple5 Cyc__gentuple_1192={0,(struct _dyneither_ptr){_tmp497,_tmp497,_tmp497 + 6},(
void*)& Cyc__genrep_803};static char _tmp498[5]="Fn_d";static struct _tuple5 Cyc__gentuple_1193={
1,(struct _dyneither_ptr){_tmp498,_tmp498,_tmp498 + 5},(void*)& Cyc__genrep_807};
static char _tmp499[6]="Let_d";static struct _tuple5 Cyc__gentuple_1194={2,(struct
_dyneither_ptr){_tmp499,_tmp499,_tmp499 + 6},(void*)& Cyc__genrep_1186};static char
_tmp49A[7]="Letv_d";static struct _tuple5 Cyc__gentuple_1195={3,(struct
_dyneither_ptr){_tmp49A,_tmp49A,_tmp49A + 7},(void*)& Cyc__genrep_1182};static char
_tmp49B[9]="Region_d";static struct _tuple5 Cyc__gentuple_1196={4,(struct
_dyneither_ptr){_tmp49B,_tmp49B,_tmp49B + 9},(void*)& Cyc__genrep_1175};static char
_tmp49C[8]="Alias_d";static struct _tuple5 Cyc__gentuple_1197={5,(struct
_dyneither_ptr){_tmp49C,_tmp49C,_tmp49C + 8},(void*)& Cyc__genrep_1169};static char
_tmp49D[7]="Aggr_d";static struct _tuple5 Cyc__gentuple_1198={6,(struct
_dyneither_ptr){_tmp49D,_tmp49D,_tmp49D + 7},(void*)& Cyc__genrep_1165};static char
_tmp49E[11]="Datatype_d";static struct _tuple5 Cyc__gentuple_1199={7,(struct
_dyneither_ptr){_tmp49E,_tmp49E,_tmp49E + 11},(void*)& Cyc__genrep_1161};static
char _tmp49F[7]="Enum_d";static struct _tuple5 Cyc__gentuple_1200={8,(struct
_dyneither_ptr){_tmp49F,_tmp49F,_tmp49F + 7},(void*)& Cyc__genrep_1157};static char
_tmp4A0[10]="Typedef_d";static struct _tuple5 Cyc__gentuple_1201={9,(struct
_dyneither_ptr){_tmp4A0,_tmp4A0,_tmp4A0 + 10},(void*)& Cyc__genrep_60};static char
_tmp4A1[12]="Namespace_d";static struct _tuple5 Cyc__gentuple_1202={10,(struct
_dyneither_ptr){_tmp4A1,_tmp4A1,_tmp4A1 + 12},(void*)& Cyc__genrep_55};static char
_tmp4A2[8]="Using_d";static struct _tuple5 Cyc__gentuple_1203={11,(struct
_dyneither_ptr){_tmp4A2,_tmp4A2,_tmp4A2 + 8},(void*)& Cyc__genrep_50};static char
_tmp4A3[10]="ExternC_d";static struct _tuple5 Cyc__gentuple_1204={12,(struct
_dyneither_ptr){_tmp4A3,_tmp4A3,_tmp4A3 + 10},(void*)& Cyc__genrep_46};static char
_tmp4A4[17]="ExternCinclude_d";static struct _tuple5 Cyc__gentuple_1205={13,(struct
_dyneither_ptr){_tmp4A4,_tmp4A4,_tmp4A4 + 17},(void*)& Cyc__genrep_6};static struct
_tuple5*Cyc__genarr_1206[14]={& Cyc__gentuple_1192,& Cyc__gentuple_1193,& Cyc__gentuple_1194,&
Cyc__gentuple_1195,& Cyc__gentuple_1196,& Cyc__gentuple_1197,& Cyc__gentuple_1198,&
Cyc__gentuple_1199,& Cyc__gentuple_1200,& Cyc__gentuple_1201,& Cyc__gentuple_1202,&
Cyc__gentuple_1203,& Cyc__gentuple_1204,& Cyc__gentuple_1205};static char _tmp4A6[9]="Raw_decl";
struct Cyc_Typerep_TUnion_struct Cyc_Absyn_raw_decl_t_rep={5,(struct _dyneither_ptr){
_tmp4A6,_tmp4A6,_tmp4A6 + 9},{(void*)((struct _tuple7**)Cyc__genarr_5),(void*)((
struct _tuple7**)Cyc__genarr_5),(void*)((struct _tuple7**)Cyc__genarr_5 + 2)},{(
void*)((struct _tuple5**)Cyc__genarr_1206),(void*)((struct _tuple5**)Cyc__genarr_1206),(
void*)((struct _tuple5**)Cyc__genarr_1206 + 14)}};static char _tmp4A7[5]="Decl";
static struct _dyneither_ptr Cyc__genname_1210=(struct _dyneither_ptr){_tmp4A7,
_tmp4A7,_tmp4A7 + 5};static char _tmp4A8[2]="r";static struct _tuple5 Cyc__gentuple_1207={
offsetof(struct Cyc_Absyn_Decl,r),(struct _dyneither_ptr){_tmp4A8,_tmp4A8,_tmp4A8 + 
2},(void*)& Cyc_Absyn_raw_decl_t_rep};static char _tmp4A9[4]="loc";static struct
_tuple5 Cyc__gentuple_1208={offsetof(struct Cyc_Absyn_Decl,loc),(struct
_dyneither_ptr){_tmp4A9,_tmp4A9,_tmp4A9 + 4},(void*)& Cyc__genrep_2};static struct
_tuple5*Cyc__genarr_1209[2]={& Cyc__gentuple_1207,& Cyc__gentuple_1208};struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_Decl_rep={3,(struct _dyneither_ptr*)& Cyc__genname_1210,sizeof(
struct Cyc_Absyn_Decl),{(void*)((struct _tuple5**)Cyc__genarr_1209),(void*)((
struct _tuple5**)Cyc__genarr_1209),(void*)((struct _tuple5**)Cyc__genarr_1209 + 2)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_1={1,1,(void*)((void*)& Cyc_struct_Absyn_Decl_rep)};
static char _tmp4AC[5]="List";static struct _dyneither_ptr Cyc__genname_1214=(struct
_dyneither_ptr){_tmp4AC,_tmp4AC,_tmp4AC + 5};static char _tmp4AD[3]="hd";static
struct _tuple5 Cyc__gentuple_1211={offsetof(struct Cyc_List_List,hd),(struct
_dyneither_ptr){_tmp4AD,_tmp4AD,_tmp4AD + 3},(void*)& Cyc__genrep_1};static char
_tmp4AE[3]="tl";static struct _tuple5 Cyc__gentuple_1212={offsetof(struct Cyc_List_List,tl),(
struct _dyneither_ptr){_tmp4AE,_tmp4AE,_tmp4AE + 3},(void*)& Cyc__genrep_0};static
struct _tuple5*Cyc__genarr_1213[2]={& Cyc__gentuple_1211,& Cyc__gentuple_1212};
struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_decl_t46H2_rep={3,(
struct _dyneither_ptr*)& Cyc__genname_1214,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_1213),(void*)((struct _tuple5**)Cyc__genarr_1213),(
void*)((struct _tuple5**)Cyc__genarr_1213 + 2)}};static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_0={1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_decl_t46H2_rep)};
void*Cyc_decls_rep=(void*)& Cyc__genrep_0;
