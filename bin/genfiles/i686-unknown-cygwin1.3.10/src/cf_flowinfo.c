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
void*v;};int Cyc_Core_intcmp(int,int);extern char Cyc_Core_Invalid_argument[21];
struct Cyc_Core_Invalid_argument_struct{char*tag;struct _dyneither_ptr f1;};extern
char Cyc_Core_Failure[12];struct Cyc_Core_Failure_struct{char*tag;struct
_dyneither_ptr f1;};extern char Cyc_Core_Impossible[15];struct Cyc_Core_Impossible_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[14];extern char Cyc_Core_Unreachable[
16];struct Cyc_Core_Unreachable_struct{char*tag;struct _dyneither_ptr f1;};extern
char Cyc_Core_Open_Region[16];extern char Cyc_Core_Free_Region[16];struct Cyc_List_List{
void*hd;struct Cyc_List_List*tl;};struct Cyc_List_List*Cyc_List_rlist(struct
_RegionHandle*,struct _dyneither_ptr);int Cyc_List_length(struct Cyc_List_List*x);
void*Cyc_List_hd(struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_tl(struct
Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rcopy(struct _RegionHandle*,struct
Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rmap_c(struct _RegionHandle*,void*(*
f)(void*,void*),void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[
18];void Cyc_List_iter(void(*f)(void*),struct Cyc_List_List*x);void Cyc_List_iter_c(
void(*f)(void*,void*),void*env,struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rappend(
struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[
8];int Cyc_List_exists_c(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x);
int Cyc_List_mem(int(*compare)(void*,void*),struct Cyc_List_List*l,void*x);struct
Cyc_List_List*Cyc_List_delete_cmp(int(*cmp)(void*,void*),struct Cyc_List_List*l,
void*x);int Cyc_List_list_cmp(int(*cmp)(void*,void*),struct Cyc_List_List*l1,
struct Cyc_List_List*l2);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*
dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;extern char
Cyc_Set_Absent[11];struct Cyc___cycFILE;extern struct Cyc___cycFILE*Cyc_stderr;
struct Cyc_Cstdio___abstractFILE;struct Cyc_String_pa_struct{int tag;struct
_dyneither_ptr f1;};struct Cyc_Int_pa_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_struct{
int tag;double f1;};struct Cyc_LongDouble_pa_struct{int tag;long double f1;};struct
Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{int tag;
unsigned long*f1;};struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr,struct
_dyneither_ptr);int Cyc_fprintf(struct Cyc___cycFILE*,struct _dyneither_ptr,struct
_dyneither_ptr);struct Cyc_ShortPtr_sa_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_struct{
int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{
int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_struct{int tag;struct
_dyneither_ptr f1;};struct Cyc_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_struct{
int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int tag;struct _dyneither_ptr f1;};
extern char Cyc_FileCloseError[19];extern char Cyc_FileOpenError[18];struct Cyc_FileOpenError_struct{
char*tag;struct _dyneither_ptr f1;};struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(
void*,void*);struct _RegionHandle*r;struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[
12];extern char Cyc_Dict_Absent[11];struct Cyc_Dict_Dict Cyc_Dict_rempty(struct
_RegionHandle*,int(*cmp)(void*,void*));int Cyc_Dict_member(struct Cyc_Dict_Dict d,
void*k);struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict d,void*k,void*v);
void*Cyc_Dict_lookup(struct Cyc_Dict_Dict d,void*k);int Cyc_Dict_lookup_bool(struct
Cyc_Dict_Dict d,void*k,void**ans);void Cyc_Dict_iter(void(*f)(void*,void*),struct
Cyc_Dict_Dict d);void Cyc_Dict_iter_c(void(*f)(void*,void*,void*),void*env,struct
Cyc_Dict_Dict d);struct Cyc_Dict_Dict Cyc_Dict_union_two_c(void*(*f)(void*,void*,
void*,void*),void*env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2);struct Cyc_Dict_Dict
Cyc_Dict_intersect_c(void*(*f)(void*,void*,void*,void*),void*env,struct Cyc_Dict_Dict
d1,struct Cyc_Dict_Dict d2);int Cyc_Dict_forall_intersect(int(*f)(void*,void*,void*),
struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2);struct _tuple0{void*f1;void*f2;};
struct _tuple0*Cyc_Dict_rchoose(struct _RegionHandle*r,struct Cyc_Dict_Dict d);
struct _tuple0*Cyc_Dict_rchoose(struct _RegionHandle*,struct Cyc_Dict_Dict d);struct
Cyc_Iter_Iter Cyc_Dict_make_iter(struct _RegionHandle*rgn,struct Cyc_Dict_Dict d);
int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct _dyneither_ptr*s2);struct Cyc_Lineno_Pos{
struct _dyneither_ptr logical_file;struct _dyneither_ptr line;int line_no;int col;};
extern char Cyc_Position_Exit[9];struct Cyc_Position_Segment;int Cyc_Position_segment_equals(
struct Cyc_Position_Segment*,struct Cyc_Position_Segment*);struct _dyneither_ptr Cyc_Position_string_of_segment(
struct Cyc_Position_Segment*);struct Cyc_Position_Error{struct _dyneither_ptr source;
struct Cyc_Position_Segment*seg;void*kind;struct _dyneither_ptr desc;};extern char
Cyc_Position_Nocontext[14];struct Cyc_Absyn_Loc_n_struct{int tag;};struct Cyc_Absyn_Rel_n_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Abs_n_struct{int tag;struct Cyc_List_List*
f1;};union Cyc_Absyn_Nmspace_union{struct Cyc_Absyn_Loc_n_struct Loc_n;struct Cyc_Absyn_Rel_n_struct
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
Cyc_Absyn_UnknownTunionInfo{struct _tuple1*name;int is_xtunion;int is_flat;};struct
Cyc_Absyn_UnknownTunion_struct{int tag;struct Cyc_Absyn_UnknownTunionInfo f1;};
struct Cyc_Absyn_KnownTunion_struct{int tag;struct Cyc_Absyn_Tuniondecl**f1;};union
Cyc_Absyn_TunionInfoU_union{struct Cyc_Absyn_UnknownTunion_struct UnknownTunion;
struct Cyc_Absyn_KnownTunion_struct KnownTunion;};struct Cyc_Absyn_TunionInfo{union
Cyc_Absyn_TunionInfoU_union tunion_info;struct Cyc_List_List*targs;struct Cyc_Core_Opt*
rgn;};struct Cyc_Absyn_UnknownTunionFieldInfo{struct _tuple1*tunion_name;struct
_tuple1*field_name;int is_xtunion;};struct Cyc_Absyn_UnknownTunionfield_struct{int
tag;struct Cyc_Absyn_UnknownTunionFieldInfo f1;};struct Cyc_Absyn_KnownTunionfield_struct{
int tag;struct Cyc_Absyn_Tuniondecl*f1;struct Cyc_Absyn_Tunionfield*f2;};union Cyc_Absyn_TunionFieldInfoU_union{
struct Cyc_Absyn_UnknownTunionfield_struct UnknownTunionfield;struct Cyc_Absyn_KnownTunionfield_struct
KnownTunionfield;};struct Cyc_Absyn_TunionFieldInfo{union Cyc_Absyn_TunionFieldInfoU_union
field_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownAggr_struct{int tag;
void*f1;struct _tuple1*f2;};struct Cyc_Absyn_KnownAggr_struct{int tag;struct Cyc_Absyn_Aggrdecl**
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
_dyneither_ptr*f2;};struct Cyc_Absyn_AggrArrow_e_struct{int tag;struct Cyc_Absyn_Exp*
f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Subscript_e_struct{int tag;struct Cyc_Absyn_Exp*
f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_struct{int tag;struct Cyc_List_List*
f1;};struct _tuple2{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Tqual f2;void*f3;};
struct Cyc_Absyn_CompoundLit_e_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*
f2;};struct Cyc_Absyn_Array_e_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;
int f4;};struct Cyc_Absyn_Struct_e_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*
f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_struct{
int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Tunion_e_struct{int tag;
struct Cyc_List_List*f1;struct Cyc_Absyn_Tuniondecl*f2;struct Cyc_Absyn_Tunionfield*
f3;};struct Cyc_Absyn_Enum_e_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*
f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_struct{int tag;
struct _tuple1*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_struct{
int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_struct{int tag;
struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_struct{
int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_struct{
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Valueof_e_struct{int tag;void*f1;
};struct Cyc_Absyn_Exp{struct Cyc_Core_Opt*topt;void*r;struct Cyc_Position_Segment*
loc;void*annot;};struct Cyc_Absyn_Exp_s_struct{int tag;struct Cyc_Absyn_Exp*f1;};
struct Cyc_Absyn_Seq_s_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*
f2;};struct Cyc_Absyn_Return_s_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};
struct _tuple3{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_struct{
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
f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Tunion_p_struct{
int tag;struct Cyc_Absyn_Tuniondecl*f1;struct Cyc_Absyn_Tunionfield*f2;struct Cyc_List_List*
f3;int f4;};struct Cyc_Absyn_Int_p_struct{int tag;void*f1;int f2;};struct Cyc_Absyn_Char_p_struct{
int tag;char f1;};struct Cyc_Absyn_Float_p_struct{int tag;struct _dyneither_ptr f1;};
struct Cyc_Absyn_Enum_p_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*
f2;};struct Cyc_Absyn_AnonEnum_p_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*
f2;};struct Cyc_Absyn_UnknownId_p_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_UnknownCall_p_struct{
int tag;struct _tuple1*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;struct Cyc_Core_Opt*
topt;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*
pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*
body;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Global_b_struct{int tag;
struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_struct{int tag;struct Cyc_Absyn_Fndecl*
f1;};struct Cyc_Absyn_Param_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct
Cyc_Absyn_Local_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{void*sc;struct
_tuple1*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;
struct Cyc_Core_Opt*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{
void*sc;int is_inline;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*
effect;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*
cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;struct Cyc_Core_Opt*
cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;
struct Cyc_List_List*attributes;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*
name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*
attributes;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct
Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{
void*kind;void*sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*
impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Tunionfield{struct _tuple1*
name;struct Cyc_List_List*typs;struct Cyc_Position_Segment*loc;void*sc;};struct Cyc_Absyn_Tuniondecl{
void*sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int
is_xtunion;int is_flat;};struct Cyc_Absyn_Enumfield{struct _tuple1*name;struct Cyc_Absyn_Exp*
tag;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Enumdecl{void*sc;struct
_tuple1*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct
_tuple1*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*
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
f2;};struct Cyc_Absyn_Using_d_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*
f2;};struct Cyc_Absyn_ExternC_d_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_struct{
int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Decl{void*
r;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_ArrayElement_struct{int tag;
struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_struct{int tag;struct
_dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];int Cyc_Absyn_tvar_cmp(
struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*);void*Cyc_Absyn_conref_def(void*,
struct Cyc_Absyn_Conref*x);struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(
union Cyc_Absyn_AggrInfoU_union info);int Cyc_Absyn_is_union_type(void*);struct Cyc_RgnOrder_RgnPO;
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_initial_fn_po(struct _RegionHandle*,struct
Cyc_List_List*tvs,struct Cyc_List_List*po,void*effect,struct Cyc_Absyn_Tvar*
fst_rgn,struct Cyc_Position_Segment*);struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_outlives_constraint(
struct _RegionHandle*,struct Cyc_RgnOrder_RgnPO*po,void*eff,void*rgn,struct Cyc_Position_Segment*
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
le;int allow_valueof;};void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap);void Cyc_Tcutil_terr(struct Cyc_Position_Segment*,struct
_dyneither_ptr fmt,struct _dyneither_ptr ap);void*Cyc_Tcutil_compress(void*t);int
Cyc_Tcutil_typecmp(void*,void*);int Cyc_Tcutil_bits_only(void*t);struct Cyc_PP_Ppstate;
struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs: 1;
int qvar_to_Cids: 1;int add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int
rewrite_temp_tvars: 1;int print_all_tvars: 1;int print_all_kinds: 1;int
print_all_effects: 1;int print_using_stmts: 1;int print_externC_stmts: 1;int
print_full_evars: 1;int print_zeroterm: 1;int generate_line_directives: 1;int
use_curr_namespace: 1;struct Cyc_List_List*curr_namespace;};struct _dyneither_ptr
Cyc_Absynpp_typ2string(void*);struct _dyneither_ptr Cyc_Absynpp_exp2string(struct
Cyc_Absyn_Exp*);struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple1*);
struct _tuple4{unsigned int f1;int f2;};struct _tuple4 Cyc_Evexp_eval_const_uint_exp(
struct Cyc_Absyn_Exp*e);struct Cyc_CfFlowInfo_VarRoot_struct{int tag;struct Cyc_Absyn_Vardecl*
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
rop;};struct Cyc_CfFlowInfo_TagCmp{void*cmp;void*bd;};char Cyc_CfFlowInfo_HasTagCmps[
15]="\000\000\000\000HasTagCmps\000";struct Cyc_CfFlowInfo_HasTagCmps_struct{char*
tag;struct Cyc_List_List*f1;};char Cyc_CfFlowInfo_IsZero[11]="\000\000\000\000IsZero\000";
char Cyc_CfFlowInfo_NotZero[12]="\000\000\000\000NotZero\000";struct Cyc_CfFlowInfo_NotZero_struct{
char*tag;struct Cyc_List_List*f1;};char Cyc_CfFlowInfo_UnknownZ[13]="\000\000\000\000UnknownZ\000";
struct Cyc_CfFlowInfo_UnknownZ_struct{char*tag;struct Cyc_List_List*f1;};struct Cyc_List_List*
Cyc_CfFlowInfo_copy_tagcmps(struct _RegionHandle*,struct Cyc_List_List*);struct Cyc_CfFlowInfo_PlaceL_struct{
int tag;struct Cyc_CfFlowInfo_Place*f1;};struct Cyc_CfFlowInfo_UnknownL_struct{int
tag;};union Cyc_CfFlowInfo_AbsLVal_union{struct Cyc_CfFlowInfo_PlaceL_struct PlaceL;
struct Cyc_CfFlowInfo_UnknownL_struct UnknownL;};struct Cyc_CfFlowInfo_UnknownR_struct{
int tag;void*f1;};struct Cyc_CfFlowInfo_Esc_struct{int tag;void*f1;};struct Cyc_CfFlowInfo_AddressOf_struct{
int tag;struct Cyc_CfFlowInfo_Place*f1;};struct Cyc_CfFlowInfo_TagCmps_struct{int
tag;struct Cyc_List_List*f1;};struct Cyc_CfFlowInfo_Aggregate_struct{int tag;struct
_dyneither_ptr f1;};int Cyc_CfFlowInfo_update_place_set(struct Cyc_Dict_Dict*set,
void*place,struct Cyc_Position_Segment*loc);int Cyc_CfFlowInfo_place_set_subset(
struct Cyc_Dict_Dict s1,struct Cyc_Dict_Dict s2);int Cyc_CfFlowInfo_place_set_equals(
struct Cyc_Dict_Dict s1,struct Cyc_Dict_Dict s2);struct Cyc_Dict_Dict Cyc_CfFlowInfo_union_place_set(
struct Cyc_Dict_Dict s1,struct Cyc_Dict_Dict s2,int disjoint);struct Cyc_Dict_Dict Cyc_CfFlowInfo_union_uplace_set(
struct Cyc_Dict_Dict s1,struct Cyc_Dict_Dict s2,int disjoint);struct Cyc_CfFlowInfo_ConsumeInfo{
struct Cyc_Dict_Dict consumed;struct Cyc_List_List*may_consume;};struct Cyc_CfFlowInfo_ConsumeInfo
Cyc_CfFlowInfo_and_consume(struct _RegionHandle*,struct Cyc_CfFlowInfo_ConsumeInfo
c1,struct Cyc_CfFlowInfo_ConsumeInfo c2);int Cyc_CfFlowInfo_consume_approx(struct
Cyc_CfFlowInfo_ConsumeInfo c1,struct Cyc_CfFlowInfo_ConsumeInfo c2);struct Cyc_CfFlowInfo_BottomFL_struct{
int tag;};struct Cyc_CfFlowInfo_ReachableFL_struct{int tag;struct Cyc_Dict_Dict f1;
struct Cyc_List_List*f2;struct Cyc_CfFlowInfo_ConsumeInfo f3;};union Cyc_CfFlowInfo_FlowInfo_union{
struct Cyc_CfFlowInfo_BottomFL_struct BottomFL;struct Cyc_CfFlowInfo_ReachableFL_struct
ReachableFL;};struct Cyc_CfFlowInfo_FlowEnv{struct _RegionHandle*r;void*
unknown_none;void*unknown_this;void*unknown_all;void*esc_none;void*esc_this;void*
esc_all;struct Cyc_Dict_Dict mt_flowdict;struct Cyc_Dict_Dict mt_place_set;struct Cyc_CfFlowInfo_Place*
dummy_place;};struct Cyc_CfFlowInfo_FlowEnv*Cyc_CfFlowInfo_new_flow_env(struct
_RegionHandle*r);int Cyc_CfFlowInfo_get_field_index(void*t,struct _dyneither_ptr*f);
int Cyc_CfFlowInfo_get_field_index_fs(struct Cyc_List_List*fs,struct _dyneither_ptr*
f);int Cyc_CfFlowInfo_root_cmp(void*,void*);int Cyc_CfFlowInfo_place_cmp(struct Cyc_CfFlowInfo_Place*,
struct Cyc_CfFlowInfo_Place*);struct _dyneither_ptr Cyc_CfFlowInfo_aggrfields_to_aggrdict(
struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_List_List*,void*);void*Cyc_CfFlowInfo_typ_to_absrval(
struct Cyc_CfFlowInfo_FlowEnv*,void*t,void*leafval);void*Cyc_CfFlowInfo_initlevel(
struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict d,void*r);void*Cyc_CfFlowInfo_lookup_place(
struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*place);int Cyc_CfFlowInfo_is_unescaped(
struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*place);int Cyc_CfFlowInfo_flow_lessthan_approx(
union Cyc_CfFlowInfo_FlowInfo_union f1,union Cyc_CfFlowInfo_FlowInfo_union f2);void
Cyc_CfFlowInfo_print_absrval(void*rval);void Cyc_CfFlowInfo_print_initlevel(void*
il);void Cyc_CfFlowInfo_print_root(void*root);void Cyc_CfFlowInfo_print_place(
struct Cyc_CfFlowInfo_Place*p);void Cyc_CfFlowInfo_print_dict_set(struct Cyc_Dict_Dict
p,void(*pr)(void*));void Cyc_CfFlowInfo_print_list(struct Cyc_List_List*p,void(*pr)(
void*));void Cyc_CfFlowInfo_print_flowdict(struct Cyc_Dict_Dict d);void Cyc_CfFlowInfo_print_flow(
union Cyc_CfFlowInfo_FlowInfo_union f);struct Cyc_List_List*Cyc_CfFlowInfo_reln_assign_var(
struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Exp*);
struct Cyc_List_List*Cyc_CfFlowInfo_reln_assign_exp(struct _RegionHandle*,struct
Cyc_List_List*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*);struct Cyc_List_List*
Cyc_CfFlowInfo_reln_kill_var(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_Absyn_Vardecl*);
struct Cyc_List_List*Cyc_CfFlowInfo_reln_kill_exp(struct _RegionHandle*,struct Cyc_List_List*,
struct Cyc_Absyn_Exp*);struct Cyc_List_List*Cyc_CfFlowInfo_copy_relns(struct
_RegionHandle*,struct Cyc_List_List*);int Cyc_CfFlowInfo_same_relns(struct Cyc_List_List*,
struct Cyc_List_List*);void Cyc_CfFlowInfo_print_relns(struct Cyc_List_List*);
struct Cyc_Dict_Dict Cyc_CfFlowInfo_escape_deref(struct Cyc_CfFlowInfo_FlowEnv*fenv,
struct Cyc_Dict_Dict d,struct Cyc_Dict_Dict*all_changed,void*r);struct Cyc_Dict_Dict
Cyc_CfFlowInfo_assign_place(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Position_Segment*
loc,struct Cyc_Dict_Dict d,struct Cyc_Dict_Dict*all_changed,struct Cyc_CfFlowInfo_Place*
place,void*r);union Cyc_CfFlowInfo_FlowInfo_union Cyc_CfFlowInfo_join_flow(struct
Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict*,union Cyc_CfFlowInfo_FlowInfo_union,
union Cyc_CfFlowInfo_FlowInfo_union,int);struct _tuple5{union Cyc_CfFlowInfo_FlowInfo_union
f1;void*f2;};struct _tuple5 Cyc_CfFlowInfo_join_flow_and_rval(struct Cyc_CfFlowInfo_FlowEnv*,
struct Cyc_Dict_Dict*all_changed,struct _tuple5 pr1,struct _tuple5 pr2,int);union Cyc_CfFlowInfo_FlowInfo_union
Cyc_CfFlowInfo_after_flow(struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict*,
union Cyc_CfFlowInfo_FlowInfo_union,union Cyc_CfFlowInfo_FlowInfo_union,struct Cyc_Dict_Dict,
struct Cyc_Dict_Dict);union Cyc_CfFlowInfo_FlowInfo_union Cyc_CfFlowInfo_kill_flow_region(
struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo_union f,void*rgn);
struct Cyc_CfFlowInfo_Region_k_struct{int tag;struct Cyc_Absyn_Tvar*f1;};int Cyc_CfFlowInfo_contains_region(
void*rgn,void*t);union Cyc_CfFlowInfo_FlowInfo_union Cyc_CfFlowInfo_consume_unique_rvals(
struct Cyc_Position_Segment*loc,union Cyc_CfFlowInfo_FlowInfo_union f);void Cyc_CfFlowInfo_check_unique_rvals(
struct Cyc_Position_Segment*loc,union Cyc_CfFlowInfo_FlowInfo_union f);union Cyc_CfFlowInfo_FlowInfo_union
Cyc_CfFlowInfo_readthrough_unique_rvals(struct Cyc_Position_Segment*loc,union Cyc_CfFlowInfo_FlowInfo_union
f);union Cyc_CfFlowInfo_FlowInfo_union Cyc_CfFlowInfo_drop_unique_rvals(struct Cyc_Position_Segment*
loc,union Cyc_CfFlowInfo_FlowInfo_union f);struct _tuple6{struct Cyc_CfFlowInfo_ConsumeInfo
f1;union Cyc_CfFlowInfo_FlowInfo_union f2;};struct _tuple6 Cyc_CfFlowInfo_save_consume_info(
struct Cyc_CfFlowInfo_FlowEnv*,union Cyc_CfFlowInfo_FlowInfo_union f,int clear);
union Cyc_CfFlowInfo_FlowInfo_union Cyc_CfFlowInfo_restore_consume_info(union Cyc_CfFlowInfo_FlowInfo_union
f,struct Cyc_CfFlowInfo_ConsumeInfo c,int may_consume_only);struct _dyneither_ptr Cyc_CfFlowInfo_place_err_string(
struct Cyc_CfFlowInfo_Place*place);struct Cyc_CfFlowInfo_UniquePlace*Cyc_CfFlowInfo_unique_place_of(
struct _RegionHandle*r,struct Cyc_Absyn_Exp*e);int Cyc_CfFlowInfo_place_cmp(struct
Cyc_CfFlowInfo_Place*p1,struct Cyc_CfFlowInfo_Place*p2);int Cyc_CfFlowInfo_root_cmp(
void*r1,void*r2);struct Cyc_CfFlowInfo_FlowEnv*Cyc_CfFlowInfo_new_flow_env(struct
_RegionHandle*r){static struct Cyc_Absyn_Const_e_struct dummy_rawexp={0,(union Cyc_Absyn_Cnst_union)((
struct Cyc_Absyn_Null_c_struct){6})};static struct Cyc_Absyn_Exp dummy_exp={0,(void*)((
void*)& dummy_rawexp),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};return({struct Cyc_CfFlowInfo_FlowEnv*
_tmp0=_region_malloc(r,sizeof(*_tmp0));_tmp0->r=r;_tmp0->unknown_none=(void*)((
void*)({struct Cyc_CfFlowInfo_UnknownR_struct*_tmpE=_region_malloc(r,sizeof(*
_tmpE));_tmpE[0]=({struct Cyc_CfFlowInfo_UnknownR_struct _tmpF;_tmpF.tag=0;_tmpF.f1=(
void*)((void*)0);_tmpF;});_tmpE;}));_tmp0->unknown_this=(void*)((void*)({struct
Cyc_CfFlowInfo_UnknownR_struct*_tmpC=_region_malloc(r,sizeof(*_tmpC));_tmpC[0]=({
struct Cyc_CfFlowInfo_UnknownR_struct _tmpD;_tmpD.tag=0;_tmpD.f1=(void*)((void*)1);
_tmpD;});_tmpC;}));_tmp0->unknown_all=(void*)((void*)({struct Cyc_CfFlowInfo_UnknownR_struct*
_tmpA=_region_malloc(r,sizeof(*_tmpA));_tmpA[0]=({struct Cyc_CfFlowInfo_UnknownR_struct
_tmpB;_tmpB.tag=0;_tmpB.f1=(void*)((void*)2);_tmpB;});_tmpA;}));_tmp0->esc_none=(
void*)((void*)({struct Cyc_CfFlowInfo_Esc_struct*_tmp8=_region_malloc(r,sizeof(*
_tmp8));_tmp8[0]=({struct Cyc_CfFlowInfo_Esc_struct _tmp9;_tmp9.tag=1;_tmp9.f1=(
void*)((void*)0);_tmp9;});_tmp8;}));_tmp0->esc_this=(void*)((void*)({struct Cyc_CfFlowInfo_Esc_struct*
_tmp6=_region_malloc(r,sizeof(*_tmp6));_tmp6[0]=({struct Cyc_CfFlowInfo_Esc_struct
_tmp7;_tmp7.tag=1;_tmp7.f1=(void*)((void*)1);_tmp7;});_tmp6;}));_tmp0->esc_all=(
void*)((void*)({struct Cyc_CfFlowInfo_Esc_struct*_tmp4=_region_malloc(r,sizeof(*
_tmp4));_tmp4[0]=({struct Cyc_CfFlowInfo_Esc_struct _tmp5;_tmp5.tag=1;_tmp5.f1=(
void*)((void*)2);_tmp5;});_tmp4;}));_tmp0->mt_flowdict=Cyc_Dict_rempty(r,Cyc_CfFlowInfo_root_cmp);
_tmp0->mt_place_set=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(
struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*)))Cyc_Dict_rempty)(r,Cyc_CfFlowInfo_place_cmp);
_tmp0->dummy_place=({struct Cyc_CfFlowInfo_Place*_tmp1=_region_malloc(r,sizeof(*
_tmp1));_tmp1->root=(void*)((void*)({struct Cyc_CfFlowInfo_MallocPt_struct*_tmp2=
_region_malloc(r,sizeof(*_tmp2));_tmp2[0]=({struct Cyc_CfFlowInfo_MallocPt_struct
_tmp3;_tmp3.tag=1;_tmp3.f1=& dummy_exp;_tmp3.f2=(void*)((void*)0);_tmp3;});_tmp2;}));
_tmp1->fields=0;_tmp1;});_tmp0;});}int Cyc_CfFlowInfo_update_place_set(struct Cyc_Dict_Dict*
set,void*place,struct Cyc_Position_Segment*loc){if(set != 0){if(((int(*)(struct Cyc_Dict_Dict
d,void*k))Cyc_Dict_member)(*set,place))return 1;else{*set=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,void*k,struct Cyc_Position_Segment*v))Cyc_Dict_insert)(*set,
place,loc);}}return 0;}struct _dyneither_ptr Cyc_CfFlowInfo_place_err_string(struct
Cyc_CfFlowInfo_Place*place){void*_tmp13;struct Cyc_List_List*_tmp14;struct Cyc_CfFlowInfo_Place
_tmp12=*place;_tmp13=(void*)_tmp12.root;_tmp14=_tmp12.fields;{void*_tmp15=_tmp13;
struct Cyc_Absyn_Vardecl*_tmp16;_LL1: if(*((int*)_tmp15)!= 0)goto _LL3;_tmp16=((
struct Cyc_CfFlowInfo_VarRoot_struct*)_tmp15)->f1;_LL2: if(_tmp14 == 0)return Cyc_Absynpp_qvar2string(
_tmp16->name);else{return(struct _dyneither_ptr)({struct Cyc_String_pa_struct
_tmp19;_tmp19.tag=0;_tmp19.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp16->name));{void*_tmp17[1]={& _tmp19};Cyc_aprintf(({const char*_tmp18="reachable from %s";
_tag_dyneither(_tmp18,sizeof(char),18);}),_tag_dyneither(_tmp17,sizeof(void*),1));}});}
_LL3:;_LL4:({void*_tmp1A=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap))Cyc_Tcutil_impos)(({const char*_tmp1B="error locations not for VarRoots";
_tag_dyneither(_tmp1B,sizeof(char),33);}),_tag_dyneither(_tmp1A,sizeof(void*),0));});
_LL0:;}}struct _dyneither_ptr Cyc_CfFlowInfo_uplace_err_string(struct Cyc_CfFlowInfo_UniquePlace*
place){return Cyc_CfFlowInfo_place_err_string(& place->place);}static void Cyc_CfFlowInfo_unique_err(
void*place,struct _dyneither_ptr(*toString)(void*),struct _dyneither_ptr err_msg1,
struct _dyneither_ptr err_msg2,struct Cyc_Position_Segment*consumed_loc,struct Cyc_Position_Segment*
loc){struct _dyneither_ptr _tmp1C=toString(place);if(!Cyc_Position_segment_equals(
consumed_loc,loc)){struct _dyneither_ptr _tmp1D=Cyc_Position_string_of_segment(
consumed_loc);({struct Cyc_String_pa_struct _tmp20;_tmp20.tag=0;_tmp20.f1=(struct
_dyneither_ptr)_tmp1D;{struct Cyc_String_pa_struct _tmp1F;_tmp1F.tag=0;_tmp1F.f1=(
struct _dyneither_ptr)_tmp1C;{void*_tmp1E[2]={& _tmp1F,& _tmp20};Cyc_Tcutil_terr(
loc,err_msg1,_tag_dyneither(_tmp1E,sizeof(void*),2));}}});}else{({struct Cyc_String_pa_struct
_tmp22;_tmp22.tag=0;_tmp22.f1=(struct _dyneither_ptr)_tmp1C;{void*_tmp21[1]={&
_tmp22};Cyc_Tcutil_terr(loc,err_msg2,_tag_dyneither(_tmp21,sizeof(void*),1));}});}}
struct _tuple7{int f1;struct _dyneither_ptr(*f2)(void*);};static struct Cyc_Position_Segment*
Cyc_CfFlowInfo_combine_consume_f(struct _tuple7*env,void*place,struct Cyc_Position_Segment*
loc1,struct Cyc_Position_Segment*loc2){struct _tuple7 _tmp24;int _tmp25;struct
_dyneither_ptr(*_tmp26)(void*);struct _tuple7*_tmp23=env;_tmp24=*_tmp23;_tmp25=
_tmp24.f1;_tmp26=_tmp24.f2;if(_tmp25)Cyc_CfFlowInfo_unique_err(place,_tmp26,({
const char*_tmp27="May consume non-aliased pointer %s more than once (cf. %s)";
_tag_dyneither(_tmp27,sizeof(char),59);}),({const char*_tmp28="May consume non-aliased pointer %s more than once";
_tag_dyneither(_tmp28,sizeof(char),50);}),loc1,loc2);return loc1;}struct _tuple8{
int f1;struct _dyneither_ptr(*f2)(struct Cyc_CfFlowInfo_Place*place);};struct Cyc_Dict_Dict
Cyc_CfFlowInfo_union_place_set(struct Cyc_Dict_Dict s1,struct Cyc_Dict_Dict s2,int
disjoint){struct _tuple8 _tmp29=({struct _tuple8 _tmp2B;_tmp2B.f1=disjoint;_tmp2B.f2=
Cyc_CfFlowInfo_place_err_string;_tmp2B;});struct Cyc_Dict_Dict _tmp2A=((struct Cyc_Dict_Dict(*)(
struct Cyc_Position_Segment*(*f)(struct _tuple8*,struct Cyc_CfFlowInfo_Place*,
struct Cyc_Position_Segment*,struct Cyc_Position_Segment*),struct _tuple8*env,
struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2))Cyc_Dict_union_two_c)((struct Cyc_Position_Segment*(*)(
struct _tuple8*env,struct Cyc_CfFlowInfo_Place*place,struct Cyc_Position_Segment*
loc1,struct Cyc_Position_Segment*loc2))Cyc_CfFlowInfo_combine_consume_f,& _tmp29,
s1,s2);return _tmp2A;}struct _tuple9{int f1;struct _dyneither_ptr(*f2)(struct Cyc_CfFlowInfo_UniquePlace*
place);};struct Cyc_Dict_Dict Cyc_CfFlowInfo_union_uplace_set(struct Cyc_Dict_Dict
s1,struct Cyc_Dict_Dict s2,int disjoint){struct _tuple9 _tmp2C=({struct _tuple9 _tmp2D;
_tmp2D.f1=disjoint;_tmp2D.f2=Cyc_CfFlowInfo_uplace_err_string;_tmp2D;});return((
struct Cyc_Dict_Dict(*)(struct Cyc_Position_Segment*(*f)(struct _tuple9*,struct Cyc_CfFlowInfo_UniquePlace*,
struct Cyc_Position_Segment*,struct Cyc_Position_Segment*),struct _tuple9*env,
struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2))Cyc_Dict_union_two_c)((struct Cyc_Position_Segment*(*)(
struct _tuple9*env,struct Cyc_CfFlowInfo_UniquePlace*place,struct Cyc_Position_Segment*
loc1,struct Cyc_Position_Segment*loc2))Cyc_CfFlowInfo_combine_consume_f,& _tmp2C,
s1,s2);}struct _tuple10{void*f1;struct Cyc_Position_Segment*f2;};int Cyc_CfFlowInfo_place_set_subset(
struct Cyc_Dict_Dict s1,struct Cyc_Dict_Dict s2){if((int)s1.t == (int)s2.t)return 1;{
struct _handler_cons _tmp2E;_push_handler(& _tmp2E);{int _tmp30=0;if(setjmp(_tmp2E.handler))
_tmp30=1;if(!_tmp30){{struct _RegionHandle _tmp31=_new_region("temp");struct
_RegionHandle*temp=& _tmp31;_push_region(temp);{struct Cyc_Iter_Iter _tmp32=((
struct Cyc_Iter_Iter(*)(struct _RegionHandle*rgn,struct Cyc_Dict_Dict d))Cyc_Dict_make_iter)(
temp,s1);struct _tuple10 _tmp33=*((struct _tuple10*(*)(struct _RegionHandle*r,struct
Cyc_Dict_Dict d))Cyc_Dict_rchoose)(temp,s1);while(((int(*)(struct Cyc_Iter_Iter,
struct _tuple10*))Cyc_Iter_next)(_tmp32,& _tmp33)){void*_tmp34=_tmp33.f1;if(!((int(*)(
struct Cyc_Dict_Dict d,void*k))Cyc_Dict_member)(s2,_tmp34)){int _tmp35=0;
_npop_handler(1);return _tmp35;}}{int _tmp36=1;_npop_handler(1);return _tmp36;}};
_pop_region(temp);};_pop_handler();}else{void*_tmp2F=(void*)_exn_thrown;void*
_tmp38=_tmp2F;_LL6:;_LL7: return 1;_LL8:;_LL9:(void)_throw(_tmp38);_LL5:;}}}}int
Cyc_CfFlowInfo_place_set_equals(struct Cyc_Dict_Dict s1,struct Cyc_Dict_Dict s2){
return Cyc_CfFlowInfo_place_set_subset(s1,s2) && Cyc_CfFlowInfo_place_set_subset(
s2,s1);}int Cyc_CfFlowInfo_get_field_index_fs(struct Cyc_List_List*fs,struct
_dyneither_ptr*f){int n=0;for(0;(unsigned int)fs;fs=fs->tl){struct _dyneither_ptr*
_tmp39=((struct Cyc_Absyn_Aggrfield*)fs->hd)->name;if(Cyc_strptrcmp(_tmp39,f)== 0)
return n;n=n + 1;}(int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp3A=
_cycalloc(sizeof(*_tmp3A));_tmp3A[0]=({struct Cyc_Core_Impossible_struct _tmp3B;
_tmp3B.tag=Cyc_Core_Impossible;_tmp3B.f1=(struct _dyneither_ptr)({struct Cyc_String_pa_struct
_tmp3E;_tmp3E.tag=0;_tmp3E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);{
void*_tmp3C[1]={& _tmp3E};Cyc_aprintf(({const char*_tmp3D="get_field_index_fs failed: %s";
_tag_dyneither(_tmp3D,sizeof(char),30);}),_tag_dyneither(_tmp3C,sizeof(void*),1));}});
_tmp3B;});_tmp3A;}));}int Cyc_CfFlowInfo_get_field_index(void*t,struct
_dyneither_ptr*f){void*_tmp3F=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo
_tmp40;union Cyc_Absyn_AggrInfoU_union _tmp41;struct Cyc_List_List*_tmp42;_LLB: if(
_tmp3F <= (void*)4)goto _LLF;if(*((int*)_tmp3F)!= 10)goto _LLD;_tmp40=((struct Cyc_Absyn_AggrType_struct*)
_tmp3F)->f1;_tmp41=_tmp40.aggr_info;_LLC: {struct Cyc_List_List*_tmp43=((struct
Cyc_Absyn_AggrdeclImpl*)_check_null((Cyc_Absyn_get_known_aggrdecl(_tmp41))->impl))->fields;
_tmp42=_tmp43;goto _LLE;}_LLD: if(*((int*)_tmp3F)!= 11)goto _LLF;_tmp42=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp3F)->f2;_LLE: return Cyc_CfFlowInfo_get_field_index_fs(_tmp42,f);_LLF:;_LL10:(
int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp44=_cycalloc(sizeof(*
_tmp44));_tmp44[0]=({struct Cyc_Core_Impossible_struct _tmp45;_tmp45.tag=Cyc_Core_Impossible;
_tmp45.f1=(struct _dyneither_ptr)({struct Cyc_String_pa_struct _tmp48;_tmp48.tag=0;
_tmp48.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));{
void*_tmp46[1]={& _tmp48};Cyc_aprintf(({const char*_tmp47="get_field_index failed: %s";
_tag_dyneither(_tmp47,sizeof(char),27);}),_tag_dyneither(_tmp46,sizeof(void*),1));}});
_tmp45;});_tmp44;}));_LLA:;}int Cyc_CfFlowInfo_root_cmp(void*r1,void*r2){if((int)
r1 == (int)r2)return 0;{struct _tuple0 _tmp4A=({struct _tuple0 _tmp49;_tmp49.f1=r1;
_tmp49.f2=r2;_tmp49;});void*_tmp4B;struct Cyc_Absyn_Vardecl*_tmp4C;void*_tmp4D;
struct Cyc_Absyn_Vardecl*_tmp4E;void*_tmp4F;void*_tmp50;void*_tmp51;struct Cyc_Absyn_Exp*
_tmp52;void*_tmp53;struct Cyc_Absyn_Exp*_tmp54;void*_tmp55;void*_tmp56;void*
_tmp57;int _tmp58;void*_tmp59;int _tmp5A;_LL12: _tmp4B=_tmp4A.f1;if(*((int*)_tmp4B)
!= 0)goto _LL14;_tmp4C=((struct Cyc_CfFlowInfo_VarRoot_struct*)_tmp4B)->f1;_tmp4D=
_tmp4A.f2;if(*((int*)_tmp4D)!= 0)goto _LL14;_tmp4E=((struct Cyc_CfFlowInfo_VarRoot_struct*)
_tmp4D)->f1;_LL13: return(int)_tmp4C - (int)_tmp4E;_LL14: _tmp4F=_tmp4A.f1;if(*((
int*)_tmp4F)!= 0)goto _LL16;_LL15: return - 1;_LL16: _tmp50=_tmp4A.f2;if(*((int*)
_tmp50)!= 0)goto _LL18;_LL17: return 1;_LL18: _tmp51=_tmp4A.f1;if(*((int*)_tmp51)!= 
1)goto _LL1A;_tmp52=((struct Cyc_CfFlowInfo_MallocPt_struct*)_tmp51)->f1;_tmp53=
_tmp4A.f2;if(*((int*)_tmp53)!= 1)goto _LL1A;_tmp54=((struct Cyc_CfFlowInfo_MallocPt_struct*)
_tmp53)->f1;_LL19: return(int)_tmp52 - (int)_tmp54;_LL1A: _tmp55=_tmp4A.f1;if(*((
int*)_tmp55)!= 1)goto _LL1C;_LL1B: return - 1;_LL1C: _tmp56=_tmp4A.f2;if(*((int*)
_tmp56)!= 1)goto _LL1E;_LL1D: return 1;_LL1E: _tmp57=_tmp4A.f1;if(*((int*)_tmp57)!= 
2)goto _LL11;_tmp58=((struct Cyc_CfFlowInfo_InitParam_struct*)_tmp57)->f1;_tmp59=
_tmp4A.f2;if(*((int*)_tmp59)!= 2)goto _LL11;_tmp5A=((struct Cyc_CfFlowInfo_InitParam_struct*)
_tmp59)->f1;_LL1F: return _tmp58 - _tmp5A;_LL11:;}}static int Cyc_CfFlowInfo_intlist_cmp(
struct Cyc_List_List*list1,struct Cyc_List_List*list2){return((int(*)(int(*cmp)(
int,int),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Core_intcmp,
list1,list2);}int Cyc_CfFlowInfo_place_cmp(struct Cyc_CfFlowInfo_Place*p1,struct
Cyc_CfFlowInfo_Place*p2){if((int)p1 == (int)p2)return 0;{void*_tmp5C;struct Cyc_List_List*
_tmp5D;struct Cyc_CfFlowInfo_Place _tmp5B=*p1;_tmp5C=(void*)_tmp5B.root;_tmp5D=
_tmp5B.fields;{void*_tmp5F;struct Cyc_List_List*_tmp60;struct Cyc_CfFlowInfo_Place
_tmp5E=*p2;_tmp5F=(void*)_tmp5E.root;_tmp60=_tmp5E.fields;{int i=Cyc_CfFlowInfo_root_cmp(
_tmp5C,_tmp5F);if(i != 0)return i;return Cyc_CfFlowInfo_intlist_cmp(_tmp5D,_tmp60);}}}}
int Cyc_CfFlowInfo_uplace_cmp(struct Cyc_CfFlowInfo_UniquePlace*p1,struct Cyc_CfFlowInfo_UniquePlace*
p2){if((int)p1 == (int)p2)return 0;{struct Cyc_CfFlowInfo_Place _tmp62;struct Cyc_List_List*
_tmp63;struct Cyc_CfFlowInfo_UniquePlace _tmp61=*p1;_tmp62=_tmp61.place;_tmp63=
_tmp61.path;{struct Cyc_CfFlowInfo_Place _tmp65;struct Cyc_List_List*_tmp66;struct
Cyc_CfFlowInfo_UniquePlace _tmp64=*p2;_tmp65=_tmp64.place;_tmp66=_tmp64.path;{int
i=Cyc_CfFlowInfo_place_cmp(& _tmp62,& _tmp65);if(i != 0)return i;return((int(*)(int(*
cmp)(struct Cyc_List_List*,struct Cyc_List_List*),struct Cyc_List_List*l1,struct Cyc_List_List*
l2))Cyc_List_list_cmp)(Cyc_CfFlowInfo_intlist_cmp,_tmp63,_tmp66);}}}}static
struct _dyneither_ptr*Cyc_CfFlowInfo_place2string(struct Cyc_CfFlowInfo_Place*p){
struct Cyc_List_List*sl=0;void*_tmp68;struct Cyc_List_List*_tmp69;struct Cyc_CfFlowInfo_Place
_tmp67=*p;_tmp68=(void*)_tmp67.root;_tmp69=_tmp67.fields;{void*_tmp6A=_tmp68;
struct Cyc_Absyn_Vardecl*_tmp6B;struct Cyc_Absyn_Exp*_tmp6C;int _tmp6D;_LL21: if(*((
int*)_tmp6A)!= 0)goto _LL23;_tmp6B=((struct Cyc_CfFlowInfo_VarRoot_struct*)_tmp6A)->f1;
_LL22: sl=({struct Cyc_List_List*_tmp6E=_cycalloc(sizeof(*_tmp6E));_tmp6E->hd=({
struct _dyneither_ptr*_tmp6F=_cycalloc(sizeof(*_tmp6F));_tmp6F[0]=({struct Cyc_String_pa_struct
_tmp72;_tmp72.tag=0;_tmp72.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*
_tmp6B->name).f2);{void*_tmp70[1]={& _tmp72};Cyc_aprintf(({const char*_tmp71="%s";
_tag_dyneither(_tmp71,sizeof(char),3);}),_tag_dyneither(_tmp70,sizeof(void*),1));}});
_tmp6F;});_tmp6E->tl=sl;_tmp6E;});goto _LL20;_LL23: if(*((int*)_tmp6A)!= 1)goto
_LL25;_tmp6C=((struct Cyc_CfFlowInfo_MallocPt_struct*)_tmp6A)->f1;_LL24: sl=({
struct Cyc_List_List*_tmp73=_cycalloc(sizeof(*_tmp73));_tmp73->hd=({struct
_dyneither_ptr*_tmp74=_cycalloc(sizeof(*_tmp74));_tmp74[0]=({struct Cyc_Int_pa_struct
_tmp77;_tmp77.tag=1;_tmp77.f1=(unsigned long)((int)_tmp6C);{void*_tmp75[1]={&
_tmp77};Cyc_aprintf(({const char*_tmp76="mpt%d";_tag_dyneither(_tmp76,sizeof(char),
6);}),_tag_dyneither(_tmp75,sizeof(void*),1));}});_tmp74;});_tmp73->tl=sl;_tmp73;});
goto _LL20;_LL25: if(*((int*)_tmp6A)!= 2)goto _LL20;_tmp6D=((struct Cyc_CfFlowInfo_InitParam_struct*)
_tmp6A)->f1;_LL26: sl=({struct Cyc_List_List*_tmp78=_cycalloc(sizeof(*_tmp78));
_tmp78->hd=({struct _dyneither_ptr*_tmp79=_cycalloc(sizeof(*_tmp79));_tmp79[0]=({
struct Cyc_Int_pa_struct _tmp7C;_tmp7C.tag=1;_tmp7C.f1=(unsigned long)_tmp6D;{void*
_tmp7A[1]={& _tmp7C};Cyc_aprintf(({const char*_tmp7B="param%d";_tag_dyneither(
_tmp7B,sizeof(char),8);}),_tag_dyneither(_tmp7A,sizeof(void*),1));}});_tmp79;});
_tmp78->tl=sl;_tmp78;});goto _LL20;_LL20:;}for(0;_tmp69 != 0;_tmp69=_tmp69->tl){sl=({
struct Cyc_List_List*_tmp7D=_cycalloc(sizeof(*_tmp7D));_tmp7D->hd=({struct
_dyneither_ptr*_tmp7E=_cycalloc(sizeof(*_tmp7E));_tmp7E[0]=({struct Cyc_Int_pa_struct
_tmp81;_tmp81.tag=1;_tmp81.f1=(unsigned long)((int)_tmp69->hd);{void*_tmp7F[1]={&
_tmp81};Cyc_aprintf(({const char*_tmp80="%d";_tag_dyneither(_tmp80,sizeof(char),3);}),
_tag_dyneither(_tmp7F,sizeof(void*),1));}});_tmp7E;});_tmp7D->tl=sl;_tmp7D;});}{
struct _dyneither_ptr*_tmp82=({struct _dyneither_ptr*_tmp87=_cycalloc(sizeof(*
_tmp87));_tmp87[0]=({struct Cyc_String_pa_struct _tmp8A;_tmp8A.tag=0;_tmp8A.f1=(
struct _dyneither_ptr)({const char*_tmp8B="";_tag_dyneither(_tmp8B,sizeof(char),1);});{
void*_tmp88[1]={& _tmp8A};Cyc_aprintf(({const char*_tmp89="%s";_tag_dyneither(
_tmp89,sizeof(char),3);}),_tag_dyneither(_tmp88,sizeof(void*),1));}});_tmp87;});
for(0;sl != 0;sl=sl->tl){*_tmp82=({struct Cyc_String_pa_struct _tmp86;_tmp86.tag=0;
_tmp86.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp82);{struct Cyc_String_pa_struct
_tmp85;_tmp85.tag=0;_tmp85.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((
struct _dyneither_ptr*)sl->hd));{void*_tmp83[2]={& _tmp85,& _tmp86};Cyc_aprintf(({
const char*_tmp84="%s.%s";_tag_dyneither(_tmp84,sizeof(char),6);}),_tag_dyneither(
_tmp83,sizeof(void*),2));}}});}return _tmp82;}}static void*Cyc_CfFlowInfo_i_typ_to_absrval(
struct Cyc_CfFlowInfo_FlowEnv*fenv,int allow_zeroterm,void*t,void*leafval);struct
_dyneither_ptr Cyc_CfFlowInfo_aggrfields_to_aggrdict(struct Cyc_CfFlowInfo_FlowEnv*
fenv,struct Cyc_List_List*fs,void*leafval){unsigned int sz=(unsigned int)((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(fs);struct _dyneither_ptr d=({unsigned int
_tmp90=sz;void**_tmp91=(void**)_region_malloc(fenv->r,_check_times(sizeof(void*),
_tmp90));struct _dyneither_ptr _tmp93=_tag_dyneither(_tmp91,sizeof(void*),_tmp90);{
unsigned int _tmp92=_tmp90;unsigned int i;for(i=0;i < _tmp92;i ++){_tmp91[i]=(void*)
fenv->unknown_all;}}_tmp93;});{int i=0;for(0;i < sz;(i ++,fs=fs->tl)){struct Cyc_Absyn_Aggrfield
_tmp8D;struct _dyneither_ptr*_tmp8E;void*_tmp8F;struct Cyc_Absyn_Aggrfield*_tmp8C=(
struct Cyc_Absyn_Aggrfield*)((struct Cyc_List_List*)_check_null(fs))->hd;_tmp8D=*
_tmp8C;_tmp8E=_tmp8D.name;_tmp8F=(void*)_tmp8D.type;if(_get_dyneither_size(*
_tmp8E,sizeof(char))!= 1)*((void**)_check_dyneither_subscript(d,sizeof(void*),i))=
Cyc_CfFlowInfo_i_typ_to_absrval(fenv,0,_tmp8F,leafval);}}return d;}struct _tuple11{
struct Cyc_Absyn_Tqual f1;void*f2;};static void*Cyc_CfFlowInfo_i_typ_to_absrval(
struct Cyc_CfFlowInfo_FlowEnv*fenv,int allow_zeroterm,void*t,void*leafval){if(Cyc_Absyn_is_union_type(
t))return(void*)fenv->unknown_all;{void*_tmp94=Cyc_Tcutil_compress(t);struct Cyc_Absyn_TunionFieldInfo
_tmp95;union Cyc_Absyn_TunionFieldInfoU_union _tmp96;struct Cyc_Absyn_Tunionfield*
_tmp97;struct Cyc_List_List*_tmp98;struct Cyc_Absyn_AggrInfo _tmp99;union Cyc_Absyn_AggrInfoU_union
_tmp9A;void*_tmp9B;struct Cyc_List_List*_tmp9C;struct Cyc_Absyn_ArrayInfo _tmp9D;
void*_tmp9E;struct Cyc_Absyn_Conref*_tmp9F;void*_tmpA0;struct Cyc_Absyn_PtrInfo
_tmpA1;struct Cyc_Absyn_PtrAtts _tmpA2;struct Cyc_Absyn_Conref*_tmpA3;_LL28: if(
_tmp94 <= (void*)4)goto _LL36;if(*((int*)_tmp94)!= 3)goto _LL2A;_tmp95=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp94)->f1;_tmp96=_tmp95.field_info;if((((((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp94)->f1).field_info).KnownTunionfield).tag != 1)goto _LL2A;_tmp97=(_tmp96.KnownTunionfield).f2;
_LL29: if(_tmp97->typs == 0)return leafval;_tmp98=_tmp97->typs;goto _LL2B;_LL2A: if(*((
int*)_tmp94)!= 9)goto _LL2C;_tmp98=((struct Cyc_Absyn_TupleType_struct*)_tmp94)->f1;
_LL2B: {unsigned int sz=(unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp98);struct _dyneither_ptr d=({unsigned int _tmpA6=sz;void**_tmpA7=(void**)
_region_malloc(fenv->r,_check_times(sizeof(void*),_tmpA6));struct _dyneither_ptr
_tmpA9=_tag_dyneither(_tmpA7,sizeof(void*),_tmpA6);{unsigned int _tmpA8=_tmpA6;
unsigned int i;for(i=0;i < _tmpA8;i ++){_tmpA7[i]=(void*)fenv->unknown_all;}}_tmpA9;});{
int i=0;for(0;i < sz;++ i){*((void**)_check_dyneither_subscript(d,sizeof(void*),i))=
Cyc_CfFlowInfo_i_typ_to_absrval(fenv,0,(*((struct _tuple11*)((struct Cyc_List_List*)
_check_null(_tmp98))->hd)).f2,leafval);_tmp98=_tmp98->tl;}}return(void*)({struct
Cyc_CfFlowInfo_Aggregate_struct*_tmpA4=_region_malloc(fenv->r,sizeof(*_tmpA4));
_tmpA4[0]=({struct Cyc_CfFlowInfo_Aggregate_struct _tmpA5;_tmpA5.tag=4;_tmpA5.f1=d;
_tmpA5;});_tmpA4;});}_LL2C: if(*((int*)_tmp94)!= 10)goto _LL2E;_tmp99=((struct Cyc_Absyn_AggrType_struct*)
_tmp94)->f1;_tmp9A=_tmp99.aggr_info;_LL2D: {struct Cyc_Absyn_Aggrdecl*_tmpAA=Cyc_Absyn_get_known_aggrdecl(
_tmp9A);if(_tmpAA->impl == 0)goto _LL27;_tmp9C=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmpAA->impl))->fields;goto _LL2F;}_LL2E: if(*((int*)_tmp94)!= 11)goto
_LL30;_tmp9B=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)_tmp94)->f1;if((int)
_tmp9B != 0)goto _LL30;_tmp9C=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp94)->f2;
_LL2F: return(void*)({struct Cyc_CfFlowInfo_Aggregate_struct*_tmpAB=_region_malloc(
fenv->r,sizeof(*_tmpAB));_tmpAB[0]=({struct Cyc_CfFlowInfo_Aggregate_struct _tmpAC;
_tmpAC.tag=4;_tmpAC.f1=Cyc_CfFlowInfo_aggrfields_to_aggrdict(fenv,_tmp9C,leafval);
_tmpAC;});_tmpAB;});_LL30: if(*((int*)_tmp94)!= 7)goto _LL32;_tmp9D=((struct Cyc_Absyn_ArrayType_struct*)
_tmp94)->f1;_tmp9E=(void*)_tmp9D.elt_type;_tmp9F=_tmp9D.zero_term;if(!((int(*)(
int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp9F))goto _LL32;_LL31:
return allow_zeroterm  && Cyc_Tcutil_bits_only(_tmp9E)?(void*)fenv->unknown_all:
leafval;_LL32: if(*((int*)_tmp94)!= 18)goto _LL34;_tmpA0=(void*)((struct Cyc_Absyn_TagType_struct*)
_tmp94)->f1;_LL33: {void*_tmpAD=leafval;void*_tmpAE;void*_tmpAF;_LL39: if(_tmpAD
<= (void*)3)goto _LL3D;if(*((int*)_tmpAD)!= 0)goto _LL3B;_tmpAE=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)
_tmpAD)->f1;if((int)_tmpAE != 2)goto _LL3B;_LL3A: goto _LL3C;_LL3B: if(*((int*)_tmpAD)
!= 1)goto _LL3D;_tmpAF=(void*)((struct Cyc_CfFlowInfo_Esc_struct*)_tmpAD)->f1;if((
int)_tmpAF != 2)goto _LL3D;_LL3C: goto _LL3E;_LL3D: if((int)_tmpAD != 0)goto _LL3F;
_LL3E: goto _LL40;_LL3F: if((int)_tmpAD != 1)goto _LL41;_LL40: return(void*)({struct
Cyc_CfFlowInfo_TagCmps_struct*_tmpB0=_region_malloc(fenv->r,sizeof(*_tmpB0));
_tmpB0[0]=({struct Cyc_CfFlowInfo_TagCmps_struct _tmpB1;_tmpB1.tag=3;_tmpB1.f1=({
struct Cyc_CfFlowInfo_TagCmp*_tmpB2[1];_tmpB2[0]=({struct Cyc_CfFlowInfo_TagCmp*
_tmpB3=_region_malloc(fenv->r,sizeof(*_tmpB3));_tmpB3->cmp=(void*)((void*)5);
_tmpB3->bd=(void*)_tmpA0;_tmpB3;});((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct _dyneither_ptr))Cyc_List_rlist)(fenv->r,_tag_dyneither(_tmpB2,sizeof(
struct Cyc_CfFlowInfo_TagCmp*),1));});_tmpB1;});_tmpB0;});_LL41:;_LL42: return
leafval;_LL38:;}_LL34: if(*((int*)_tmp94)!= 4)goto _LL36;_tmpA1=((struct Cyc_Absyn_PointerType_struct*)
_tmp94)->f1;_tmpA2=_tmpA1.ptr_atts;_tmpA3=_tmpA2.nullable;if(!(!((int(*)(int,
struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmpA3)))goto _LL36;_LL35:{void*
_tmpB4=leafval;void*_tmpB5;void*_tmpB6;_LL44: if(_tmpB4 <= (void*)3)goto _LL48;if(*((
int*)_tmpB4)!= 0)goto _LL46;_tmpB5=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)
_tmpB4)->f1;if((int)_tmpB5 != 1)goto _LL46;_LL45: return(void*)2;_LL46: if(*((int*)
_tmpB4)!= 0)goto _LL48;_tmpB6=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)
_tmpB4)->f1;if((int)_tmpB6 != 2)goto _LL48;_LL47: return(void*)1;_LL48:;_LL49: goto
_LL43;_LL43:;}goto _LL27;_LL36:;_LL37: goto _LL27;_LL27:;}return Cyc_Tcutil_bits_only(
t)?(void*)fenv->unknown_all: leafval;}void*Cyc_CfFlowInfo_typ_to_absrval(struct
Cyc_CfFlowInfo_FlowEnv*fenv,void*t,void*leafval){return Cyc_CfFlowInfo_i_typ_to_absrval(
fenv,1,t,leafval);}struct _tuple12{struct Cyc_CfFlowInfo_Place*f1;struct Cyc_Position_Segment*
f2;};static int Cyc_CfFlowInfo_prefix_of_member(struct Cyc_CfFlowInfo_Place*place,
struct Cyc_Dict_Dict set){struct _RegionHandle _tmpB7=_new_region("r");struct
_RegionHandle*r=& _tmpB7;_push_region(r);{struct _tuple12 _tmpB8=({struct _tuple12
_tmpC3;_tmpC3.f1=place;_tmpC3.f2=0;_tmpC3;});struct Cyc_Iter_Iter _tmpB9=((struct
Cyc_Iter_Iter(*)(struct _RegionHandle*rgn,struct Cyc_Dict_Dict d))Cyc_Dict_make_iter)(
r,set);while(((int(*)(struct Cyc_Iter_Iter,struct _tuple12*))Cyc_Iter_next)(_tmpB9,&
_tmpB8)){struct Cyc_CfFlowInfo_Place*_tmpBA=_tmpB8.f1;void*_tmpBC;struct Cyc_List_List*
_tmpBD;struct Cyc_CfFlowInfo_Place _tmpBB=*place;_tmpBC=(void*)_tmpBB.root;_tmpBD=
_tmpBB.fields;{void*_tmpBF;struct Cyc_List_List*_tmpC0;struct Cyc_CfFlowInfo_Place
_tmpBE=*_tmpBA;_tmpBF=(void*)_tmpBE.root;_tmpC0=_tmpBE.fields;if(Cyc_CfFlowInfo_root_cmp(
_tmpBC,_tmpBF)!= 0)continue;for(0;_tmpBD != 0  && _tmpC0 != 0;(_tmpBD=_tmpBD->tl,
_tmpC0=_tmpC0->tl)){if((int)_tmpBD->hd != (int)_tmpC0->hd)break;}if(_tmpBD == 0){
int _tmpC1=1;_npop_handler(0);return _tmpC1;}}}{int _tmpC2=0;_npop_handler(0);
return _tmpC2;}};_pop_region(r);}struct Cyc_CfFlowInfo_EscPile{struct _RegionHandle*
rgn;struct Cyc_List_List*places;};static void Cyc_CfFlowInfo_add_place(struct Cyc_CfFlowInfo_EscPile*
pile,struct Cyc_CfFlowInfo_Place*place){if(!((int(*)(int(*compare)(struct Cyc_CfFlowInfo_Place*,
struct Cyc_CfFlowInfo_Place*),struct Cyc_List_List*l,struct Cyc_CfFlowInfo_Place*x))
Cyc_List_mem)(Cyc_CfFlowInfo_place_cmp,pile->places,place))pile->places=({struct
Cyc_List_List*_tmpC4=_region_malloc(pile->rgn,sizeof(*_tmpC4));_tmpC4->hd=place;
_tmpC4->tl=pile->places;_tmpC4;});}static void Cyc_CfFlowInfo_add_places(struct Cyc_CfFlowInfo_EscPile*
pile,void*r){void*_tmpC5=r;struct Cyc_CfFlowInfo_Place*_tmpC6;struct
_dyneither_ptr _tmpC7;_LL4B: if(_tmpC5 <= (void*)3)goto _LL4F;if(*((int*)_tmpC5)!= 2)
goto _LL4D;_tmpC6=((struct Cyc_CfFlowInfo_AddressOf_struct*)_tmpC5)->f1;_LL4C: Cyc_CfFlowInfo_add_place(
pile,_tmpC6);return;_LL4D: if(*((int*)_tmpC5)!= 4)goto _LL4F;_tmpC7=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmpC5)->f1;_LL4E:{int i=0;for(0;i < _get_dyneither_size(_tmpC7,sizeof(void*));++ i){
Cyc_CfFlowInfo_add_places(pile,((void**)_tmpC7.curr)[i]);}}return;_LL4F:;_LL50:
return;_LL4A:;}static void*Cyc_CfFlowInfo_insert_place_inner(struct _RegionHandle*
r,void*new_val,void*old_val){void*_tmpC8=old_val;struct _dyneither_ptr _tmpC9;
_LL52: if(_tmpC8 <= (void*)3)goto _LL54;if(*((int*)_tmpC8)!= 4)goto _LL54;_tmpC9=((
struct Cyc_CfFlowInfo_Aggregate_struct*)_tmpC8)->f1;_LL53: {struct _dyneither_ptr
d2=({unsigned int _tmpCC=_get_dyneither_size(_tmpC9,sizeof(void*));void**_tmpCD=(
void**)_region_malloc(r,_check_times(sizeof(void*),_tmpCC));struct _dyneither_ptr
_tmpCF=_tag_dyneither(_tmpCD,sizeof(void*),_tmpCC);{unsigned int _tmpCE=_tmpCC;
unsigned int i;for(i=0;i < _tmpCE;i ++){_tmpCD[i]=Cyc_CfFlowInfo_insert_place_inner(
r,new_val,((void**)_tmpC9.curr)[(int)i]);}}_tmpCF;});return(void*)({struct Cyc_CfFlowInfo_Aggregate_struct*
_tmpCA=_region_malloc(r,sizeof(*_tmpCA));_tmpCA[0]=({struct Cyc_CfFlowInfo_Aggregate_struct
_tmpCB;_tmpCB.tag=4;_tmpCB.f1=d2;_tmpCB;});_tmpCA;});}_LL54:;_LL55: return new_val;
_LL51:;}static struct _dyneither_ptr Cyc_CfFlowInfo_aggr_dict_insert(struct
_RegionHandle*r,struct _dyneither_ptr d,int n,void*rval){void*_tmpD0=*((void**)
_check_dyneither_subscript(d,sizeof(void*),n));if(_tmpD0 == rval)return d;{struct
_dyneither_ptr res=({unsigned int _tmpD1=_get_dyneither_size(d,sizeof(void*));void**
_tmpD2=(void**)_region_malloc(r,_check_times(sizeof(void*),_tmpD1));struct
_dyneither_ptr _tmpD4=_tag_dyneither(_tmpD2,sizeof(void*),_tmpD1);{unsigned int
_tmpD3=_tmpD1;unsigned int i;for(i=0;i < _tmpD3;i ++){_tmpD2[i]=((void**)d.curr)[(
int)i];}}_tmpD4;});*((void**)_check_dyneither_subscript(res,sizeof(void*),n))=
rval;return res;}}struct _tuple13{struct Cyc_List_List*f1;void*f2;};static void*Cyc_CfFlowInfo_insert_place_outer(
struct _RegionHandle*r,struct Cyc_List_List*fs,void*old_val,void*new_val){if(fs == 
0)return Cyc_CfFlowInfo_insert_place_inner(r,new_val,old_val);{struct _tuple13
_tmpD6=({struct _tuple13 _tmpD5;_tmpD5.f1=fs;_tmpD5.f2=old_val;_tmpD5;});struct Cyc_List_List*
_tmpD7;struct Cyc_List_List _tmpD8;int _tmpD9;struct Cyc_List_List*_tmpDA;void*
_tmpDB;struct _dyneither_ptr _tmpDC;_LL57: _tmpD7=_tmpD6.f1;if(_tmpD7 == 0)goto _LL59;
_tmpD8=*_tmpD7;_tmpD9=(int)_tmpD8.hd;_tmpDA=_tmpD8.tl;_tmpDB=_tmpD6.f2;if(_tmpDB
<= (void*)3)goto _LL59;if(*((int*)_tmpDB)!= 4)goto _LL59;_tmpDC=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmpDB)->f1;_LL58: {void*_tmpDD=Cyc_CfFlowInfo_insert_place_outer(r,_tmpDA,*((
void**)_check_dyneither_subscript(_tmpDC,sizeof(void*),_tmpD9)),new_val);struct
_dyneither_ptr _tmpDE=Cyc_CfFlowInfo_aggr_dict_insert(r,_tmpDC,_tmpD9,_tmpDD);if(
_tmpDE.curr == _tmpDC.curr)return old_val;else{return(void*)({struct Cyc_CfFlowInfo_Aggregate_struct*
_tmpDF=_region_malloc(r,sizeof(*_tmpDF));_tmpDF[0]=({struct Cyc_CfFlowInfo_Aggregate_struct
_tmpE0;_tmpE0.tag=4;_tmpE0.f1=_tmpDE;_tmpE0;});_tmpDF;});}}_LL59:;_LL5A:(int)
_throw((void*)({struct Cyc_Core_Impossible_struct*_tmpE1=_cycalloc(sizeof(*_tmpE1));
_tmpE1[0]=({struct Cyc_Core_Impossible_struct _tmpE2;_tmpE2.tag=Cyc_Core_Impossible;
_tmpE2.f1=({const char*_tmpE3="bad insert place";_tag_dyneither(_tmpE3,sizeof(
char),17);});_tmpE2;});_tmpE1;}));_LL56:;}}static struct Cyc_Dict_Dict Cyc_CfFlowInfo_escape_these(
struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_CfFlowInfo_EscPile*pile,struct Cyc_Dict_Dict*
all_changed,struct Cyc_Dict_Dict d){while(pile->places != 0){struct Cyc_CfFlowInfo_Place*
_tmpE4=(struct Cyc_CfFlowInfo_Place*)((struct Cyc_List_List*)_check_null(pile->places))->hd;
pile->places=((struct Cyc_List_List*)_check_null(pile->places))->tl;((int(*)(
struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*place,struct Cyc_Position_Segment*
loc))Cyc_CfFlowInfo_update_place_set)(all_changed,_tmpE4,0);{void*oldval;void*
newval;{struct _handler_cons _tmpE5;_push_handler(& _tmpE5);{int _tmpE7=0;if(setjmp(
_tmpE5.handler))_tmpE7=1;if(!_tmpE7){oldval=Cyc_CfFlowInfo_lookup_place(d,_tmpE4);;
_pop_handler();}else{void*_tmpE6=(void*)_exn_thrown;void*_tmpE9=_tmpE6;_LL5C: if(
_tmpE9 != Cyc_Dict_Absent)goto _LL5E;_LL5D: continue;_LL5E:;_LL5F:(void)_throw(
_tmpE9);_LL5B:;}}}{void*_tmpEA=Cyc_CfFlowInfo_initlevel(fenv,d,oldval);_LL61: if((
int)_tmpEA != 2)goto _LL63;_LL62: newval=(void*)fenv->esc_all;goto _LL60;_LL63: if((
int)_tmpEA != 1)goto _LL65;_LL64: newval=(void*)fenv->esc_this;goto _LL60;_LL65: if((
int)_tmpEA != 0)goto _LL60;_LL66: newval=(void*)fenv->esc_none;goto _LL60;_LL60:;}
Cyc_CfFlowInfo_add_places(pile,oldval);{void*_tmpEC;struct Cyc_List_List*_tmpED;
struct Cyc_CfFlowInfo_Place _tmpEB=*_tmpE4;_tmpEC=(void*)_tmpEB.root;_tmpED=_tmpEB.fields;
d=Cyc_Dict_insert(d,_tmpEC,Cyc_CfFlowInfo_insert_place_outer(fenv->r,_tmpED,Cyc_Dict_lookup(
d,_tmpEC),newval));}}}return d;}struct Cyc_CfFlowInfo_InitlevelEnv{struct
_RegionHandle*e;struct Cyc_Dict_Dict d;struct Cyc_List_List*seen;};static void*Cyc_CfFlowInfo_initlevel_approx(
void*r){void*_tmpEE=r;void*_tmpEF;void*_tmpF0;_LL68: if(_tmpEE <= (void*)3)goto
_LL6C;if(*((int*)_tmpEE)!= 0)goto _LL6A;_tmpEF=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)
_tmpEE)->f1;_LL69: return _tmpEF;_LL6A: if(*((int*)_tmpEE)!= 1)goto _LL6C;_tmpF0=(
void*)((struct Cyc_CfFlowInfo_Esc_struct*)_tmpEE)->f1;_LL6B: return _tmpF0;_LL6C:
if((int)_tmpEE != 0)goto _LL6E;_LL6D: goto _LL6F;_LL6E: if((int)_tmpEE != 1)goto _LL70;
_LL6F: return(void*)2;_LL70: if((int)_tmpEE != 2)goto _LL72;_LL71: return(void*)1;
_LL72: if(_tmpEE <= (void*)3)goto _LL74;if(*((int*)_tmpEE)!= 3)goto _LL74;_LL73:
return(void*)2;_LL74:;_LL75:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmpF1=_cycalloc(sizeof(*_tmpF1));_tmpF1[0]=({struct Cyc_Core_Impossible_struct
_tmpF2;_tmpF2.tag=Cyc_Core_Impossible;_tmpF2.f1=({const char*_tmpF3="initlevel_approx";
_tag_dyneither(_tmpF3,sizeof(char),17);});_tmpF2;});_tmpF1;}));_LL67:;}static
void*Cyc_CfFlowInfo_initlevel_rec(struct Cyc_CfFlowInfo_InitlevelEnv*env,void*r,
void*acc){void*this_ans;if(acc == (void*)0)return(void*)0;{void*_tmpF4=r;struct
_dyneither_ptr _tmpF5;struct Cyc_CfFlowInfo_Place*_tmpF6;_LL77: if(_tmpF4 <= (void*)
3)goto _LL7B;if(*((int*)_tmpF4)!= 4)goto _LL79;_tmpF5=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmpF4)->f1;_LL78: {unsigned int _tmpF7=_get_dyneither_size(_tmpF5,sizeof(void*));
this_ans=(void*)2;{int i=0;for(0;i < _tmpF7;++ i){this_ans=Cyc_CfFlowInfo_initlevel_rec(
env,*((void**)_check_dyneither_subscript(_tmpF5,sizeof(void*),i)),this_ans);}}
goto _LL76;}_LL79: if(*((int*)_tmpF4)!= 2)goto _LL7B;_tmpF6=((struct Cyc_CfFlowInfo_AddressOf_struct*)
_tmpF4)->f1;_LL7A: if(((int(*)(int(*compare)(struct Cyc_CfFlowInfo_Place*,struct
Cyc_CfFlowInfo_Place*),struct Cyc_List_List*l,struct Cyc_CfFlowInfo_Place*x))Cyc_List_mem)(
Cyc_CfFlowInfo_place_cmp,env->seen,_tmpF6))this_ans=(void*)2;else{env->seen=({
struct Cyc_List_List*_tmpF8=_region_malloc(env->e,sizeof(*_tmpF8));_tmpF8->hd=
_tmpF6;_tmpF8->tl=env->seen;_tmpF8;});this_ans=Cyc_CfFlowInfo_initlevel_rec(env,
Cyc_CfFlowInfo_lookup_place(env->d,_tmpF6),(void*)2);env->seen=((struct Cyc_List_List*)
_check_null(env->seen))->tl;if(this_ans == (void*)0)this_ans=(void*)1;}goto _LL76;
_LL7B:;_LL7C: this_ans=Cyc_CfFlowInfo_initlevel_approx(r);_LL76:;}if(this_ans == (
void*)0)return(void*)0;if(this_ans == (void*)1  || acc == (void*)1)return(void*)1;
return(void*)2;}void*Cyc_CfFlowInfo_initlevel(struct Cyc_CfFlowInfo_FlowEnv*env,
struct Cyc_Dict_Dict d,void*r){struct _RegionHandle*_tmpF9=env->r;struct Cyc_CfFlowInfo_InitlevelEnv
_tmpFA=({struct Cyc_CfFlowInfo_InitlevelEnv _tmpFB;_tmpFB.e=_tmpF9;_tmpFB.d=d;
_tmpFB.seen=0;_tmpFB;});return Cyc_CfFlowInfo_initlevel_rec(& _tmpFA,r,(void*)2);}
struct _tuple14{void*f1;int f2;};void*Cyc_CfFlowInfo_lookup_place(struct Cyc_Dict_Dict
d,struct Cyc_CfFlowInfo_Place*place){void*_tmpFD;struct Cyc_List_List*_tmpFE;
struct Cyc_CfFlowInfo_Place _tmpFC=*place;_tmpFD=(void*)_tmpFC.root;_tmpFE=_tmpFC.fields;{
void*_tmpFF=Cyc_Dict_lookup(d,_tmpFD);for(0;_tmpFE != 0;_tmpFE=_tmpFE->tl){struct
_tuple14 _tmp101=({struct _tuple14 _tmp100;_tmp100.f1=_tmpFF;_tmp100.f2=(int)_tmpFE->hd;
_tmp100;});void*_tmp102;struct _dyneither_ptr _tmp103;int _tmp104;_LL7E: _tmp102=
_tmp101.f1;if(_tmp102 <= (void*)3)goto _LL80;if(*((int*)_tmp102)!= 4)goto _LL80;
_tmp103=((struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp102)->f1;_tmp104=_tmp101.f2;
_LL7F: _tmpFF=*((void**)_check_dyneither_subscript(_tmp103,sizeof(void*),_tmp104));
goto _LL7D;_LL80:;_LL81:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp105=_cycalloc(sizeof(*_tmp105));_tmp105[0]=({struct Cyc_Core_Impossible_struct
_tmp106;_tmp106.tag=Cyc_Core_Impossible;_tmp106.f1=({const char*_tmp107="bad lookup_place";
_tag_dyneither(_tmp107,sizeof(char),17);});_tmp106;});_tmp105;}));_LL7D:;}return
_tmpFF;}}static int Cyc_CfFlowInfo_is_rval_unescaped(void*rval){void*_tmp108=rval;
struct _dyneither_ptr _tmp109;_LL83: if(_tmp108 <= (void*)3)goto _LL87;if(*((int*)
_tmp108)!= 1)goto _LL85;_LL84: return 0;_LL85: if(*((int*)_tmp108)!= 4)goto _LL87;
_tmp109=((struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp108)->f1;_LL86: {
unsigned int sz=_get_dyneither_size(_tmp109,sizeof(void*));{int i=0;for(0;i < sz;++
i){if(!Cyc_CfFlowInfo_is_rval_unescaped(((void**)_tmp109.curr)[i]))return 0;}}
return 1;}_LL87:;_LL88: return 1;_LL82:;}int Cyc_CfFlowInfo_is_unescaped(struct Cyc_Dict_Dict
d,struct Cyc_CfFlowInfo_Place*place){return Cyc_CfFlowInfo_is_rval_unescaped(Cyc_CfFlowInfo_lookup_place(
d,place));}struct Cyc_Dict_Dict Cyc_CfFlowInfo_escape_deref(struct Cyc_CfFlowInfo_FlowEnv*
fenv,struct Cyc_Dict_Dict d,struct Cyc_Dict_Dict*all_changed,void*r){struct
_RegionHandle*_tmp10A=fenv->r;struct Cyc_CfFlowInfo_EscPile*pile=({struct Cyc_CfFlowInfo_EscPile*
_tmp10B=_region_malloc(_tmp10A,sizeof(*_tmp10B));_tmp10B->rgn=_tmp10A;_tmp10B->places=
0;_tmp10B;});Cyc_CfFlowInfo_add_places(pile,r);return Cyc_CfFlowInfo_escape_these(
fenv,pile,all_changed,d);}struct Cyc_CfFlowInfo_AssignEnv{struct Cyc_CfFlowInfo_FlowEnv*
fenv;struct Cyc_CfFlowInfo_EscPile*pile;struct Cyc_Dict_Dict d;struct Cyc_Position_Segment*
loc;};static void*Cyc_CfFlowInfo_assign_place_inner(struct Cyc_CfFlowInfo_AssignEnv*
env,void*oldval,void*newval){struct _tuple0 _tmp10D=({struct _tuple0 _tmp10C;_tmp10C.f1=
oldval;_tmp10C.f2=newval;_tmp10C;});void*_tmp10E;void*_tmp10F;struct Cyc_CfFlowInfo_Place*
_tmp110;void*_tmp111;void*_tmp112;struct _dyneither_ptr _tmp113;void*_tmp114;
struct _dyneither_ptr _tmp115;void*_tmp116;void*_tmp117;_LL8A: _tmp10E=_tmp10D.f1;
if(_tmp10E <= (void*)3)goto _LL8C;if(*((int*)_tmp10E)!= 1)goto _LL8C;_tmp10F=
_tmp10D.f2;if(_tmp10F <= (void*)3)goto _LL8C;if(*((int*)_tmp10F)!= 2)goto _LL8C;
_tmp110=((struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp10F)->f1;_LL8B: Cyc_CfFlowInfo_add_place(
env->pile,_tmp110);goto _LL8D;_LL8C: _tmp111=_tmp10D.f1;if(_tmp111 <= (void*)3)goto
_LL8E;if(*((int*)_tmp111)!= 1)goto _LL8E;_LL8D: if(Cyc_CfFlowInfo_initlevel(env->fenv,
env->d,newval)!= (void*)2)({void*_tmp118=0;Cyc_Tcutil_terr(env->loc,({const char*
_tmp119="assignment puts possibly-uninitialized data in an escaped location";
_tag_dyneither(_tmp119,sizeof(char),67);}),_tag_dyneither(_tmp118,sizeof(void*),
0));});return(void*)(env->fenv)->esc_all;_LL8E: _tmp112=_tmp10D.f1;if(_tmp112 <= (
void*)3)goto _LL90;if(*((int*)_tmp112)!= 4)goto _LL90;_tmp113=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp112)->f1;_tmp114=_tmp10D.f2;if(_tmp114 <= (void*)3)goto _LL90;if(*((int*)
_tmp114)!= 4)goto _LL90;_tmp115=((struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp114)->f1;
_LL8F: {struct _dyneither_ptr new_d=({unsigned int _tmp11C=_get_dyneither_size(
_tmp113,sizeof(void*));void**_tmp11D=(void**)_region_malloc((env->fenv)->r,
_check_times(sizeof(void*),_tmp11C));struct _dyneither_ptr _tmp11F=_tag_dyneither(
_tmp11D,sizeof(void*),_tmp11C);{unsigned int _tmp11E=_tmp11C;unsigned int i;for(i=
0;i < _tmp11E;i ++){_tmp11D[i]=Cyc_CfFlowInfo_assign_place_inner(env,((void**)
_tmp113.curr)[(int)i],*((void**)_check_dyneither_subscript(_tmp115,sizeof(void*),(
int)i)));}}_tmp11F;});int change=0;{int i=0;for(0;i < _get_dyneither_size(_tmp113,
sizeof(void*));++ i){if(*((void**)_check_dyneither_subscript(new_d,sizeof(void*),
i))!= ((void**)_tmp113.curr)[i]){change=1;break;}}}if(!change)return oldval;
change=0;{int i=0;for(0;i < _get_dyneither_size(_tmp113,sizeof(void*));++ i){if(*((
void**)_check_dyneither_subscript(new_d,sizeof(void*),i))!= *((void**)
_check_dyneither_subscript(_tmp115,sizeof(void*),i))){change=1;break;}}}if(!
change)return newval;return(void*)({struct Cyc_CfFlowInfo_Aggregate_struct*_tmp11A=
_region_malloc((env->fenv)->r,sizeof(*_tmp11A));_tmp11A[0]=({struct Cyc_CfFlowInfo_Aggregate_struct
_tmp11B;_tmp11B.tag=4;_tmp11B.f1=new_d;_tmp11B;});_tmp11A;});}_LL90: _tmp116=
_tmp10D.f2;if(_tmp116 <= (void*)3)goto _LL92;if(*((int*)_tmp116)!= 1)goto _LL92;
_tmp117=(void*)((struct Cyc_CfFlowInfo_Esc_struct*)_tmp116)->f1;_LL91: {void*
_tmp120=_tmp117;_LL95: if((int)_tmp120 != 0)goto _LL97;_LL96: return(void*)(env->fenv)->unknown_none;
_LL97: if((int)_tmp120 != 1)goto _LL99;_LL98: return(void*)(env->fenv)->unknown_this;
_LL99: if((int)_tmp120 != 2)goto _LL94;_LL9A: return(void*)(env->fenv)->unknown_all;
_LL94:;}_LL92:;_LL93: return newval;_LL89:;}static void*Cyc_CfFlowInfo_assign_place_outer(
struct Cyc_CfFlowInfo_AssignEnv*env,struct Cyc_List_List*fs,void*oldval,void*
newval){if(fs == 0)return Cyc_CfFlowInfo_assign_place_inner(env,oldval,newval);{
struct _tuple13 _tmp122=({struct _tuple13 _tmp121;_tmp121.f1=fs;_tmp121.f2=oldval;
_tmp121;});struct Cyc_List_List*_tmp123;struct Cyc_List_List _tmp124;int _tmp125;
struct Cyc_List_List*_tmp126;void*_tmp127;struct _dyneither_ptr _tmp128;_LL9C:
_tmp123=_tmp122.f1;if(_tmp123 == 0)goto _LL9E;_tmp124=*_tmp123;_tmp125=(int)
_tmp124.hd;_tmp126=_tmp124.tl;_tmp127=_tmp122.f2;if(_tmp127 <= (void*)3)goto _LL9E;
if(*((int*)_tmp127)!= 4)goto _LL9E;_tmp128=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp127)->f1;_LL9D: {void*_tmp129=Cyc_CfFlowInfo_assign_place_outer(env,_tmp126,*((
void**)_check_dyneither_subscript(_tmp128,sizeof(void*),_tmp125)),newval);struct
_dyneither_ptr _tmp12A=Cyc_CfFlowInfo_aggr_dict_insert((env->fenv)->r,_tmp128,
_tmp125,_tmp129);if(_tmp12A.curr == _tmp128.curr)return oldval;else{return(void*)({
struct Cyc_CfFlowInfo_Aggregate_struct*_tmp12B=_region_malloc((env->fenv)->r,
sizeof(*_tmp12B));_tmp12B[0]=({struct Cyc_CfFlowInfo_Aggregate_struct _tmp12C;
_tmp12C.tag=4;_tmp12C.f1=_tmp12A;_tmp12C;});_tmp12B;});}}_LL9E:;_LL9F:(int)
_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp12D=_cycalloc(sizeof(*
_tmp12D));_tmp12D[0]=({struct Cyc_Core_Impossible_struct _tmp12E;_tmp12E.tag=Cyc_Core_Impossible;
_tmp12E.f1=({const char*_tmp12F="bad assign place";_tag_dyneither(_tmp12F,sizeof(
char),17);});_tmp12E;});_tmp12D;}));_LL9B:;}}struct Cyc_Dict_Dict Cyc_CfFlowInfo_assign_place(
struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Position_Segment*loc,struct Cyc_Dict_Dict
d,struct Cyc_Dict_Dict*all_changed,struct Cyc_CfFlowInfo_Place*place,void*r){((int(*)(
struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*place,struct Cyc_Position_Segment*
loc))Cyc_CfFlowInfo_update_place_set)(all_changed,place,0);{struct _RegionHandle*
_tmp130=fenv->r;struct Cyc_CfFlowInfo_Place _tmp132;void*_tmp133;struct Cyc_List_List*
_tmp134;struct Cyc_CfFlowInfo_Place*_tmp131=place;_tmp132=*_tmp131;_tmp133=(void*)
_tmp132.root;_tmp134=_tmp132.fields;{struct Cyc_CfFlowInfo_AssignEnv env=({struct
Cyc_CfFlowInfo_AssignEnv _tmp135;_tmp135.fenv=fenv;_tmp135.pile=({struct Cyc_CfFlowInfo_EscPile*
_tmp136=_region_malloc(_tmp130,sizeof(*_tmp136));_tmp136->rgn=_tmp130;_tmp136->places=
0;_tmp136;});_tmp135.d=d;_tmp135.loc=loc;_tmp135;});void*newval=Cyc_CfFlowInfo_assign_place_outer(&
env,_tmp134,Cyc_Dict_lookup(d,_tmp133),r);return Cyc_CfFlowInfo_escape_these(fenv,
env.pile,all_changed,Cyc_Dict_insert(d,_tmp133,newval));}}}struct Cyc_CfFlowInfo_JoinEnv{
struct Cyc_CfFlowInfo_FlowEnv*fenv;struct Cyc_CfFlowInfo_EscPile*pile;struct Cyc_Dict_Dict
d1;struct Cyc_Dict_Dict d2;};enum Cyc_CfFlowInfo_WhoIsChanged{Cyc_CfFlowInfo_Neither
 = 0,Cyc_CfFlowInfo_One  = 1,Cyc_CfFlowInfo_Two  = 2};struct Cyc_CfFlowInfo_AfterEnv{
struct Cyc_CfFlowInfo_JoinEnv joinenv;struct Cyc_Dict_Dict chg1;struct Cyc_Dict_Dict
chg2;struct Cyc_CfFlowInfo_Place*curr_place;struct Cyc_List_List**last_field_cell;
enum Cyc_CfFlowInfo_WhoIsChanged changed;};static int Cyc_CfFlowInfo_absRval_lessthan_approx(
void*ignore,void*r1,void*r2);static struct Cyc_List_List*Cyc_CfFlowInfo_join_tag_cmps(
struct _RegionHandle*r,struct Cyc_List_List*l1,struct Cyc_List_List*l2){if(l1 == l2)
return l1;{struct Cyc_List_List*_tmp137=0;for(0;l2 != 0;l2=l2->tl){struct Cyc_CfFlowInfo_TagCmp
_tmp139;void*_tmp13A;void*_tmp13B;struct Cyc_CfFlowInfo_TagCmp*_tmp138=(struct Cyc_CfFlowInfo_TagCmp*)
l2->hd;_tmp139=*_tmp138;_tmp13A=(void*)_tmp139.cmp;_tmp13B=(void*)_tmp139.bd;{
int found=0;void*joined_cmp=(void*)10;{struct Cyc_List_List*_tmp13C=l1;for(0;
_tmp13C != 0;_tmp13C=_tmp13C->tl){struct Cyc_CfFlowInfo_TagCmp _tmp13E;void*_tmp13F;
void*_tmp140;struct Cyc_CfFlowInfo_TagCmp*_tmp13D=(struct Cyc_CfFlowInfo_TagCmp*)
_tmp13C->hd;_tmp13E=*_tmp13D;_tmp13F=(void*)_tmp13E.cmp;_tmp140=(void*)_tmp13E.bd;
if(Cyc_Tcutil_typecmp(_tmp13B,_tmp140)== 0){found=1;if(_tmp13F == _tmp13A){
joined_cmp=_tmp13F;break;}}}}if(found)_tmp137=({struct Cyc_List_List*_tmp141=
_region_malloc(r,sizeof(*_tmp141));_tmp141->hd=({struct Cyc_CfFlowInfo_TagCmp*
_tmp142=_region_malloc(r,sizeof(*_tmp142));_tmp142->cmp=(void*)joined_cmp;
_tmp142->bd=(void*)_tmp13B;_tmp142;});_tmp141->tl=_tmp137;_tmp141;});}}return
_tmp137;}}static void*Cyc_CfFlowInfo_join_absRval(struct Cyc_CfFlowInfo_JoinEnv*
env,void*a,void*r1,void*r2){if(r1 == r2)return r1;{struct _RegionHandle*_tmp143=(
env->fenv)->r;{struct _tuple0 _tmp145=({struct _tuple0 _tmp144;_tmp144.f1=r1;_tmp144.f2=
r2;_tmp144;});void*_tmp146;struct Cyc_CfFlowInfo_Place*_tmp147;void*_tmp148;
struct Cyc_CfFlowInfo_Place*_tmp149;void*_tmp14A;struct Cyc_CfFlowInfo_Place*
_tmp14B;void*_tmp14C;struct Cyc_CfFlowInfo_Place*_tmp14D;void*_tmp14E;void*
_tmp14F;void*_tmp150;void*_tmp151;void*_tmp152;struct _dyneither_ptr _tmp153;void*
_tmp154;struct _dyneither_ptr _tmp155;void*_tmp156;struct Cyc_List_List*_tmp157;
void*_tmp158;struct Cyc_List_List*_tmp159;void*_tmp15A;void*_tmp15B;_LLA1: _tmp146=
_tmp145.f1;if(_tmp146 <= (void*)3)goto _LLA3;if(*((int*)_tmp146)!= 2)goto _LLA3;
_tmp147=((struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp146)->f1;_tmp148=_tmp145.f2;
if(_tmp148 <= (void*)3)goto _LLA3;if(*((int*)_tmp148)!= 2)goto _LLA3;_tmp149=((
struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp148)->f1;_LLA2: if(Cyc_CfFlowInfo_place_cmp(
_tmp147,_tmp149)== 0)return r1;Cyc_CfFlowInfo_add_place(env->pile,_tmp147);Cyc_CfFlowInfo_add_place(
env->pile,_tmp149);goto _LLA0;_LLA3: _tmp14A=_tmp145.f1;if(_tmp14A <= (void*)3)goto
_LLA5;if(*((int*)_tmp14A)!= 2)goto _LLA5;_tmp14B=((struct Cyc_CfFlowInfo_AddressOf_struct*)
_tmp14A)->f1;_LLA4: Cyc_CfFlowInfo_add_place(env->pile,_tmp14B);goto _LLA0;_LLA5:
_tmp14C=_tmp145.f2;if(_tmp14C <= (void*)3)goto _LLA7;if(*((int*)_tmp14C)!= 2)goto
_LLA7;_tmp14D=((struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp14C)->f1;_LLA6: Cyc_CfFlowInfo_add_place(
env->pile,_tmp14D);goto _LLA0;_LLA7: _tmp14E=_tmp145.f1;if((int)_tmp14E != 1)goto
_LLA9;_tmp14F=_tmp145.f2;if((int)_tmp14F != 2)goto _LLA9;_LLA8: goto _LLAA;_LLA9:
_tmp150=_tmp145.f1;if((int)_tmp150 != 2)goto _LLAB;_tmp151=_tmp145.f2;if((int)
_tmp151 != 1)goto _LLAB;_LLAA: return(void*)2;_LLAB: _tmp152=_tmp145.f1;if(_tmp152 <= (
void*)3)goto _LLAD;if(*((int*)_tmp152)!= 4)goto _LLAD;_tmp153=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp152)->f1;_tmp154=_tmp145.f2;if(_tmp154 <= (void*)3)goto _LLAD;if(*((int*)
_tmp154)!= 4)goto _LLAD;_tmp155=((struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp154)->f1;
_LLAC: {struct _dyneither_ptr new_d=({unsigned int _tmp15E=_get_dyneither_size(
_tmp153,sizeof(void*));void**_tmp15F=(void**)_region_malloc(_tmp143,_check_times(
sizeof(void*),_tmp15E));struct _dyneither_ptr _tmp161=_tag_dyneither(_tmp15F,
sizeof(void*),_tmp15E);{unsigned int _tmp160=_tmp15E;unsigned int i;for(i=0;i < 
_tmp160;i ++){_tmp15F[i]=((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*env,int a,void*r1,
void*r2))Cyc_CfFlowInfo_join_absRval)(env,0,((void**)_tmp153.curr)[(int)i],*((
void**)_check_dyneither_subscript(_tmp155,sizeof(void*),(int)i)));}}_tmp161;});
int change=0;{int i=0;for(0;i < _get_dyneither_size(_tmp153,sizeof(void*));++ i){if(*((
void**)_check_dyneither_subscript(new_d,sizeof(void*),i))!= ((void**)_tmp153.curr)[
i]){change=1;break;}}}if(!change)return r1;change=0;{int i=0;for(0;i < 
_get_dyneither_size(_tmp153,sizeof(void*));++ i){if(*((void**)
_check_dyneither_subscript(new_d,sizeof(void*),i))!= *((void**)
_check_dyneither_subscript(_tmp155,sizeof(void*),i))){change=1;break;}}}if(!
change)return r2;return(void*)({struct Cyc_CfFlowInfo_Aggregate_struct*_tmp15C=
_region_malloc(_tmp143,sizeof(*_tmp15C));_tmp15C[0]=({struct Cyc_CfFlowInfo_Aggregate_struct
_tmp15D;_tmp15D.tag=4;_tmp15D.f1=new_d;_tmp15D;});_tmp15C;});}_LLAD: _tmp156=
_tmp145.f1;if(_tmp156 <= (void*)3)goto _LLAF;if(*((int*)_tmp156)!= 3)goto _LLAF;
_tmp157=((struct Cyc_CfFlowInfo_TagCmps_struct*)_tmp156)->f1;_tmp158=_tmp145.f2;
if(_tmp158 <= (void*)3)goto _LLAF;if(*((int*)_tmp158)!= 3)goto _LLAF;_tmp159=((
struct Cyc_CfFlowInfo_TagCmps_struct*)_tmp158)->f1;_LLAE: {struct Cyc_List_List*
_tmp162=Cyc_CfFlowInfo_join_tag_cmps(_tmp143,_tmp157,_tmp159);if(_tmp162 == 
_tmp157)return r1;return(void*)({struct Cyc_CfFlowInfo_TagCmps_struct*_tmp163=
_region_malloc(_tmp143,sizeof(*_tmp163));_tmp163[0]=({struct Cyc_CfFlowInfo_TagCmps_struct
_tmp164;_tmp164.tag=3;_tmp164.f1=_tmp162;_tmp164;});_tmp163;});}_LLAF: _tmp15A=
_tmp145.f1;if(_tmp15A <= (void*)3)goto _LLB1;if(*((int*)_tmp15A)!= 3)goto _LLB1;
_LLB0: return r2;_LLB1: _tmp15B=_tmp145.f2;if(_tmp15B <= (void*)3)goto _LLB3;if(*((
int*)_tmp15B)!= 3)goto _LLB3;_LLB2: return r1;_LLB3:;_LLB4: goto _LLA0;_LLA0:;}{void*
il1=Cyc_CfFlowInfo_initlevel(env->fenv,env->d1,r1);void*il2=Cyc_CfFlowInfo_initlevel(
env->fenv,env->d2,r2);struct _tuple0 _tmp166=({struct _tuple0 _tmp165;_tmp165.f1=r1;
_tmp165.f2=r2;_tmp165;});void*_tmp167;void*_tmp168;_LLB6: _tmp167=_tmp166.f1;if(
_tmp167 <= (void*)3)goto _LLB8;if(*((int*)_tmp167)!= 1)goto _LLB8;_LLB7: goto _LLB9;
_LLB8: _tmp168=_tmp166.f2;if(_tmp168 <= (void*)3)goto _LLBA;if(*((int*)_tmp168)!= 1)
goto _LLBA;_LLB9: {struct _tuple0 _tmp16A=({struct _tuple0 _tmp169;_tmp169.f1=il1;
_tmp169.f2=il2;_tmp169;});void*_tmp16B;void*_tmp16C;void*_tmp16D;void*_tmp16E;
_LLBD: _tmp16B=_tmp16A.f2;if((int)_tmp16B != 0)goto _LLBF;_LLBE: goto _LLC0;_LLBF:
_tmp16C=_tmp16A.f1;if((int)_tmp16C != 0)goto _LLC1;_LLC0: return(void*)(env->fenv)->esc_none;
_LLC1: _tmp16D=_tmp16A.f2;if((int)_tmp16D != 1)goto _LLC3;_LLC2: goto _LLC4;_LLC3:
_tmp16E=_tmp16A.f1;if((int)_tmp16E != 1)goto _LLC5;_LLC4: return(void*)(env->fenv)->esc_this;
_LLC5:;_LLC6: return(void*)(env->fenv)->esc_all;_LLBC:;}_LLBA:;_LLBB: {struct
_tuple0 _tmp170=({struct _tuple0 _tmp16F;_tmp16F.f1=il1;_tmp16F.f2=il2;_tmp16F;});
void*_tmp171;void*_tmp172;void*_tmp173;void*_tmp174;_LLC8: _tmp171=_tmp170.f2;if((
int)_tmp171 != 0)goto _LLCA;_LLC9: goto _LLCB;_LLCA: _tmp172=_tmp170.f1;if((int)
_tmp172 != 0)goto _LLCC;_LLCB: return(void*)(env->fenv)->unknown_none;_LLCC: _tmp173=
_tmp170.f2;if((int)_tmp173 != 1)goto _LLCE;_LLCD: goto _LLCF;_LLCE: _tmp174=_tmp170.f1;
if((int)_tmp174 != 1)goto _LLD0;_LLCF: return(void*)(env->fenv)->unknown_this;_LLD0:;
_LLD1: return(void*)(env->fenv)->unknown_all;_LLC7:;}_LLB5:;}}}struct _tuple15{
union Cyc_CfFlowInfo_RelnOp_union f1;union Cyc_CfFlowInfo_RelnOp_union f2;};static
int Cyc_CfFlowInfo_same_relop(union Cyc_CfFlowInfo_RelnOp_union r1,union Cyc_CfFlowInfo_RelnOp_union
r2){struct _tuple15 _tmp176=({struct _tuple15 _tmp175;_tmp175.f1=r1;_tmp175.f2=r2;
_tmp175;});union Cyc_CfFlowInfo_RelnOp_union _tmp177;unsigned int _tmp178;union Cyc_CfFlowInfo_RelnOp_union
_tmp179;unsigned int _tmp17A;union Cyc_CfFlowInfo_RelnOp_union _tmp17B;struct Cyc_Absyn_Vardecl*
_tmp17C;union Cyc_CfFlowInfo_RelnOp_union _tmp17D;struct Cyc_Absyn_Vardecl*_tmp17E;
union Cyc_CfFlowInfo_RelnOp_union _tmp17F;struct Cyc_Absyn_Vardecl*_tmp180;union Cyc_CfFlowInfo_RelnOp_union
_tmp181;struct Cyc_Absyn_Vardecl*_tmp182;union Cyc_CfFlowInfo_RelnOp_union _tmp183;
unsigned int _tmp184;union Cyc_CfFlowInfo_RelnOp_union _tmp185;unsigned int _tmp186;
union Cyc_CfFlowInfo_RelnOp_union _tmp187;struct Cyc_Absyn_Vardecl*_tmp188;union Cyc_CfFlowInfo_RelnOp_union
_tmp189;struct Cyc_Absyn_Vardecl*_tmp18A;_LLD3: _tmp177=_tmp176.f1;if(((_tmp176.f1).EqualConst).tag
!= 0)goto _LLD5;_tmp178=(_tmp177.EqualConst).f1;_tmp179=_tmp176.f2;if(((_tmp176.f2).EqualConst).tag
!= 0)goto _LLD5;_tmp17A=(_tmp179.EqualConst).f1;_LLD4: return _tmp178 == _tmp17A;
_LLD5: _tmp17B=_tmp176.f1;if(((_tmp176.f1).LessVar).tag != 1)goto _LLD7;_tmp17C=(
_tmp17B.LessVar).f1;_tmp17D=_tmp176.f2;if(((_tmp176.f2).LessVar).tag != 1)goto
_LLD7;_tmp17E=(_tmp17D.LessVar).f1;_LLD6: return _tmp17C == _tmp17E;_LLD7: _tmp17F=
_tmp176.f1;if(((_tmp176.f1).LessNumelts).tag != 2)goto _LLD9;_tmp180=(_tmp17F.LessNumelts).f1;
_tmp181=_tmp176.f2;if(((_tmp176.f2).LessNumelts).tag != 2)goto _LLD9;_tmp182=(
_tmp181.LessNumelts).f1;_LLD8: return _tmp180 == _tmp182;_LLD9: _tmp183=_tmp176.f1;
if(((_tmp176.f1).LessConst).tag != 3)goto _LLDB;_tmp184=(_tmp183.LessConst).f1;
_tmp185=_tmp176.f2;if(((_tmp176.f2).LessConst).tag != 3)goto _LLDB;_tmp186=(
_tmp185.LessConst).f1;_LLDA: return _tmp184 == _tmp186;_LLDB: _tmp187=_tmp176.f1;if(((
_tmp176.f1).LessEqNumelts).tag != 4)goto _LLDD;_tmp188=(_tmp187.LessEqNumelts).f1;
_tmp189=_tmp176.f2;if(((_tmp176.f2).LessEqNumelts).tag != 4)goto _LLDD;_tmp18A=(
_tmp189.LessEqNumelts).f1;_LLDC: return _tmp188 == _tmp18A;_LLDD:;_LLDE: return 0;
_LLD2:;}static struct Cyc_List_List*Cyc_CfFlowInfo_join_relns(struct _RegionHandle*
r,struct Cyc_List_List*r1s,struct Cyc_List_List*r2s){if(r1s == r2s)return r1s;{
struct Cyc_List_List*res=0;int diff=0;{struct Cyc_List_List*_tmp18B=r1s;for(0;
_tmp18B != 0;_tmp18B=_tmp18B->tl){struct Cyc_CfFlowInfo_Reln*_tmp18C=(struct Cyc_CfFlowInfo_Reln*)
_tmp18B->hd;int found=0;{struct Cyc_List_List*_tmp18D=r2s;for(0;_tmp18D != 0;
_tmp18D=_tmp18D->tl){struct Cyc_CfFlowInfo_Reln*_tmp18E=(struct Cyc_CfFlowInfo_Reln*)
_tmp18D->hd;if(_tmp18C == _tmp18E  || _tmp18C->vd == _tmp18E->vd  && Cyc_CfFlowInfo_same_relop(
_tmp18C->rop,_tmp18E->rop)){res=({struct Cyc_List_List*_tmp18F=_region_malloc(r,
sizeof(*_tmp18F));_tmp18F->hd=_tmp18C;_tmp18F->tl=res;_tmp18F;});found=1;break;}}}
if(!found)diff=1;}}if(!diff)res=r1s;return res;}}static void Cyc_CfFlowInfo_remove_f(
struct Cyc_List_List**l,struct Cyc_CfFlowInfo_Place*place){struct _handler_cons
_tmp190;_push_handler(& _tmp190);{int _tmp192=0;if(setjmp(_tmp190.handler))_tmp192=
1;if(!_tmp192){*l=((struct Cyc_List_List*(*)(int(*cmp)(struct Cyc_CfFlowInfo_Place*,
struct Cyc_CfFlowInfo_Place*),struct Cyc_List_List*l,struct Cyc_CfFlowInfo_Place*x))
Cyc_List_delete_cmp)(Cyc_CfFlowInfo_place_cmp,*l,place);;_pop_handler();}else{
void*_tmp191=(void*)_exn_thrown;void*_tmp194=_tmp191;_LLE0: if(_tmp194 != Cyc_Core_Not_found)
goto _LLE2;_LLE1: goto _LLDF;_LLE2:;_LLE3:(void)_throw(_tmp194);_LLDF:;}}}static
struct Cyc_CfFlowInfo_ConsumeInfo Cyc_CfFlowInfo_or_consume(struct _RegionHandle*r,
struct Cyc_CfFlowInfo_ConsumeInfo c1,struct Cyc_CfFlowInfo_ConsumeInfo c2){struct Cyc_List_List*
mc=c2.may_consume;struct _RegionHandle*_tmp195=r;{struct Cyc_List_List*_tmp196=((
struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rcopy)(
_tmp195,c1.may_consume);((void(*)(void(*f)(struct Cyc_List_List**,struct Cyc_CfFlowInfo_Place*),
struct Cyc_List_List**env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_CfFlowInfo_remove_f,&
_tmp196,c2.may_consume);while(_tmp196 != 0){mc=({struct Cyc_List_List*_tmp197=
_region_malloc(r,sizeof(*_tmp197));_tmp197->hd=(struct Cyc_CfFlowInfo_Place*)((
struct Cyc_List_List*)_check_null(_tmp196))->hd;_tmp197->tl=mc;_tmp197;});_tmp196=((
struct Cyc_List_List*)_check_null(_tmp196))->tl;}}return({struct Cyc_CfFlowInfo_ConsumeInfo
_tmp198;_tmp198.consumed=Cyc_CfFlowInfo_union_place_set(c1.consumed,c2.consumed,
0);_tmp198.may_consume=mc;_tmp198;});}struct Cyc_CfFlowInfo_ConsumeInfo Cyc_CfFlowInfo_and_consume(
struct _RegionHandle*r,struct Cyc_CfFlowInfo_ConsumeInfo c1,struct Cyc_CfFlowInfo_ConsumeInfo
c2){struct Cyc_List_List*_tmp199=((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(r,c1.may_consume,
c2.may_consume);return({struct Cyc_CfFlowInfo_ConsumeInfo _tmp19A;_tmp19A.consumed=
Cyc_CfFlowInfo_union_place_set(c1.consumed,c2.consumed,1);_tmp19A.may_consume=
_tmp199;_tmp19A;});}struct _tuple16{union Cyc_CfFlowInfo_FlowInfo_union f1;union Cyc_CfFlowInfo_FlowInfo_union
f2;};union Cyc_CfFlowInfo_FlowInfo_union Cyc_CfFlowInfo_join_flow(struct Cyc_CfFlowInfo_FlowEnv*
fenv,struct Cyc_Dict_Dict*all_changed,union Cyc_CfFlowInfo_FlowInfo_union f1,union
Cyc_CfFlowInfo_FlowInfo_union f2,int or_consumed){struct _tuple16 _tmp19C=({struct
_tuple16 _tmp19B;_tmp19B.f1=f1;_tmp19B.f2=f2;_tmp19B;});union Cyc_CfFlowInfo_FlowInfo_union
_tmp19D;union Cyc_CfFlowInfo_FlowInfo_union _tmp19E;union Cyc_CfFlowInfo_FlowInfo_union
_tmp19F;struct Cyc_Dict_Dict _tmp1A0;struct Cyc_List_List*_tmp1A1;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp1A2;union Cyc_CfFlowInfo_FlowInfo_union _tmp1A3;struct Cyc_Dict_Dict _tmp1A4;
struct Cyc_List_List*_tmp1A5;struct Cyc_CfFlowInfo_ConsumeInfo _tmp1A6;_LLE5:
_tmp19D=_tmp19C.f1;if(((_tmp19C.f1).BottomFL).tag != 0)goto _LLE7;_LLE6: return f2;
_LLE7: _tmp19E=_tmp19C.f2;if(((_tmp19C.f2).BottomFL).tag != 0)goto _LLE9;_LLE8:
return f1;_LLE9: _tmp19F=_tmp19C.f1;if(((_tmp19C.f1).ReachableFL).tag != 1)goto
_LLE4;_tmp1A0=(_tmp19F.ReachableFL).f1;_tmp1A1=(_tmp19F.ReachableFL).f2;_tmp1A2=(
_tmp19F.ReachableFL).f3;_tmp1A3=_tmp19C.f2;if(((_tmp19C.f2).ReachableFL).tag != 1)
goto _LLE4;_tmp1A4=(_tmp1A3.ReachableFL).f1;_tmp1A5=(_tmp1A3.ReachableFL).f2;
_tmp1A6=(_tmp1A3.ReachableFL).f3;_LLEA: if(((_tmp1A0.t == _tmp1A4.t  && _tmp1A1 == 
_tmp1A5) && _tmp1A2.may_consume == _tmp1A6.may_consume) && (_tmp1A2.consumed).t == (
_tmp1A6.consumed).t)return f1;if(Cyc_CfFlowInfo_flow_lessthan_approx(f1,f2))
return f2;if(Cyc_CfFlowInfo_flow_lessthan_approx(f2,f1))return f1;{struct
_RegionHandle*_tmp1A7=fenv->r;struct Cyc_CfFlowInfo_JoinEnv _tmp1A8=({struct Cyc_CfFlowInfo_JoinEnv
_tmp1AD;_tmp1AD.fenv=fenv;_tmp1AD.pile=({struct Cyc_CfFlowInfo_EscPile*_tmp1AE=
_region_malloc(_tmp1A7,sizeof(*_tmp1AE));_tmp1AE->rgn=_tmp1A7;_tmp1AE->places=0;
_tmp1AE;});_tmp1AD.d1=_tmp1A0;_tmp1AD.d2=_tmp1A4;_tmp1AD;});struct Cyc_Dict_Dict
_tmp1A9=((struct Cyc_Dict_Dict(*)(void*(*f)(struct Cyc_CfFlowInfo_JoinEnv*,void*,
void*,void*),struct Cyc_CfFlowInfo_JoinEnv*env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict
d2))Cyc_Dict_intersect_c)(Cyc_CfFlowInfo_join_absRval,& _tmp1A8,_tmp1A0,_tmp1A4);
struct Cyc_List_List*_tmp1AA=Cyc_CfFlowInfo_join_relns(fenv->r,_tmp1A1,_tmp1A5);
struct Cyc_CfFlowInfo_ConsumeInfo _tmp1AB=or_consumed?Cyc_CfFlowInfo_or_consume(
fenv->r,_tmp1A2,_tmp1A6): Cyc_CfFlowInfo_and_consume(fenv->r,_tmp1A2,_tmp1A6);
return(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp1AC;(_tmp1AC.ReachableFL).tag=1;(_tmp1AC.ReachableFL).f1=Cyc_CfFlowInfo_escape_these(
fenv,_tmp1A8.pile,all_changed,_tmp1A9);(_tmp1AC.ReachableFL).f2=_tmp1AA;(_tmp1AC.ReachableFL).f3=
_tmp1AB;_tmp1AC;});}_LLE4:;}struct _tuple17{union Cyc_CfFlowInfo_FlowInfo_union f1;
union Cyc_CfFlowInfo_FlowInfo_union f2;union Cyc_CfFlowInfo_FlowInfo_union f3;};
struct _tuple5 Cyc_CfFlowInfo_join_flow_and_rval(struct Cyc_CfFlowInfo_FlowEnv*fenv,
struct Cyc_Dict_Dict*all_changed,struct _tuple5 pr1,struct _tuple5 pr2,int or_consumed){
union Cyc_CfFlowInfo_FlowInfo_union _tmp1B0;void*_tmp1B1;struct _tuple5 _tmp1AF=pr1;
_tmp1B0=_tmp1AF.f1;_tmp1B1=_tmp1AF.f2;{union Cyc_CfFlowInfo_FlowInfo_union _tmp1B3;
void*_tmp1B4;struct _tuple5 _tmp1B2=pr2;_tmp1B3=_tmp1B2.f1;_tmp1B4=_tmp1B2.f2;{
union Cyc_CfFlowInfo_FlowInfo_union outflow=Cyc_CfFlowInfo_join_flow(fenv,
all_changed,_tmp1B0,_tmp1B3,or_consumed);struct _tuple17 _tmp1B6=({struct _tuple17
_tmp1B5;_tmp1B5.f1=_tmp1B0;_tmp1B5.f2=_tmp1B3;_tmp1B5.f3=outflow;_tmp1B5;});
union Cyc_CfFlowInfo_FlowInfo_union _tmp1B7;union Cyc_CfFlowInfo_FlowInfo_union
_tmp1B8;union Cyc_CfFlowInfo_FlowInfo_union _tmp1B9;struct Cyc_Dict_Dict _tmp1BA;
union Cyc_CfFlowInfo_FlowInfo_union _tmp1BB;struct Cyc_Dict_Dict _tmp1BC;union Cyc_CfFlowInfo_FlowInfo_union
_tmp1BD;struct Cyc_Dict_Dict _tmp1BE;struct Cyc_List_List*_tmp1BF;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp1C0;_LLEC: _tmp1B7=_tmp1B6.f1;if(((_tmp1B6.f1).BottomFL).tag != 0)goto _LLEE;
_LLED: return({struct _tuple5 _tmp1C1;_tmp1C1.f1=outflow;_tmp1C1.f2=_tmp1B4;_tmp1C1;});
_LLEE: _tmp1B8=_tmp1B6.f2;if(((_tmp1B6.f2).BottomFL).tag != 0)goto _LLF0;_LLEF:
return({struct _tuple5 _tmp1C2;_tmp1C2.f1=outflow;_tmp1C2.f2=_tmp1B1;_tmp1C2;});
_LLF0: _tmp1B9=_tmp1B6.f1;if(((_tmp1B6.f1).ReachableFL).tag != 1)goto _LLF2;_tmp1BA=(
_tmp1B9.ReachableFL).f1;_tmp1BB=_tmp1B6.f2;if(((_tmp1B6.f2).ReachableFL).tag != 1)
goto _LLF2;_tmp1BC=(_tmp1BB.ReachableFL).f1;_tmp1BD=_tmp1B6.f3;if(((_tmp1B6.f3).ReachableFL).tag
!= 1)goto _LLF2;_tmp1BE=(_tmp1BD.ReachableFL).f1;_tmp1BF=(_tmp1BD.ReachableFL).f2;
_tmp1C0=(_tmp1BD.ReachableFL).f3;_LLF1: if(((int(*)(int ignore,void*r1,void*r2))
Cyc_CfFlowInfo_absRval_lessthan_approx)(0,_tmp1B1,_tmp1B4))return({struct _tuple5
_tmp1C3;_tmp1C3.f1=outflow;_tmp1C3.f2=_tmp1B4;_tmp1C3;});if(((int(*)(int ignore,
void*r1,void*r2))Cyc_CfFlowInfo_absRval_lessthan_approx)(0,_tmp1B4,_tmp1B1))
return({struct _tuple5 _tmp1C4;_tmp1C4.f1=outflow;_tmp1C4.f2=_tmp1B1;_tmp1C4;});{
struct _RegionHandle*_tmp1C5=fenv->r;struct Cyc_CfFlowInfo_JoinEnv _tmp1C6=({struct
Cyc_CfFlowInfo_JoinEnv _tmp1CA;_tmp1CA.fenv=fenv;_tmp1CA.pile=({struct Cyc_CfFlowInfo_EscPile*
_tmp1CB=_region_malloc(_tmp1C5,sizeof(*_tmp1CB));_tmp1CB->rgn=_tmp1C5;_tmp1CB->places=
0;_tmp1CB;});_tmp1CA.d1=_tmp1BA;_tmp1CA.d2=_tmp1BC;_tmp1CA;});void*_tmp1C7=((
void*(*)(struct Cyc_CfFlowInfo_JoinEnv*env,int a,void*r1,void*r2))Cyc_CfFlowInfo_join_absRval)(&
_tmp1C6,0,_tmp1B1,_tmp1B4);return({struct _tuple5 _tmp1C8;_tmp1C8.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp1C9;(_tmp1C9.ReachableFL).tag=1;(_tmp1C9.ReachableFL).f1=
Cyc_CfFlowInfo_escape_these(fenv,_tmp1C6.pile,all_changed,_tmp1BE);(_tmp1C9.ReachableFL).f2=
_tmp1BF;(_tmp1C9.ReachableFL).f3=_tmp1C0;_tmp1C9;});_tmp1C8.f2=_tmp1C7;_tmp1C8;});}
_LLF2:;_LLF3:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp1CC=
_cycalloc(sizeof(*_tmp1CC));_tmp1CC[0]=({struct Cyc_Core_Impossible_struct _tmp1CD;
_tmp1CD.tag=Cyc_Core_Impossible;_tmp1CD.f1=({const char*_tmp1CE="join_flow_and_rval: BottomFL outflow";
_tag_dyneither(_tmp1CE,sizeof(char),37);});_tmp1CD;});_tmp1CC;}));_LLEB:;}}}
static void*Cyc_CfFlowInfo_after_absRval_d(struct Cyc_CfFlowInfo_AfterEnv*,int,
void*,void*);static void*Cyc_CfFlowInfo_after_absRval(struct Cyc_CfFlowInfo_AfterEnv*
env,void*r1,void*r2){int changed1=env->changed == Cyc_CfFlowInfo_One  || ((int(*)(
struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*k))Cyc_Dict_member)(env->chg1,
env->curr_place);int changed2=env->changed == Cyc_CfFlowInfo_Two  || ((int(*)(
struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*k))Cyc_Dict_member)(env->chg2,
env->curr_place);if(changed1  && changed2)return((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*
env,int a,void*r1,void*r2))Cyc_CfFlowInfo_join_absRval)(& env->joinenv,0,r1,r2);
if(changed1){if(!Cyc_CfFlowInfo_prefix_of_member(env->curr_place,env->chg2))
return r1;env->changed=Cyc_CfFlowInfo_One;}if(changed2){if(!Cyc_CfFlowInfo_prefix_of_member(
env->curr_place,env->chg1))return r2;env->changed=Cyc_CfFlowInfo_Two;}{struct
_tuple0 _tmp1D0=({struct _tuple0 _tmp1CF;_tmp1CF.f1=r1;_tmp1CF.f2=r2;_tmp1CF;});
void*_tmp1D1;struct _dyneither_ptr _tmp1D2;void*_tmp1D3;struct _dyneither_ptr
_tmp1D4;_LLF5: _tmp1D1=_tmp1D0.f1;if(_tmp1D1 <= (void*)3)goto _LLF7;if(*((int*)
_tmp1D1)!= 4)goto _LLF7;_tmp1D2=((struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp1D1)->f1;
_tmp1D3=_tmp1D0.f2;if(_tmp1D3 <= (void*)3)goto _LLF7;if(*((int*)_tmp1D3)!= 4)goto
_LLF7;_tmp1D4=((struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp1D3)->f1;_LLF6: {
struct _dyneither_ptr new_d=({unsigned int _tmp1D7=_get_dyneither_size(_tmp1D2,
sizeof(void*));void**_tmp1D8=(void**)_region_malloc(((env->joinenv).fenv)->r,
_check_times(sizeof(void*),_tmp1D7));struct _dyneither_ptr _tmp1DA=_tag_dyneither(
_tmp1D8,sizeof(void*),_tmp1D7);{unsigned int _tmp1D9=_tmp1D7;unsigned int i;for(i=
0;i < _tmp1D9;i ++){_tmp1D8[i]=Cyc_CfFlowInfo_after_absRval_d(env,(int)i,((void**)
_tmp1D2.curr)[(int)i],*((void**)_check_dyneither_subscript(_tmp1D4,sizeof(void*),(
int)i)));}}_tmp1DA;});int change=0;{int i=0;for(0;i < _get_dyneither_size(_tmp1D2,
sizeof(void*));++ i){if(*((void**)_check_dyneither_subscript(new_d,sizeof(void*),
i))!= ((void**)_tmp1D2.curr)[i]){change=1;break;}}}if(!change)return r1;change=0;{
int i=0;for(0;i < _get_dyneither_size(_tmp1D2,sizeof(void*));++ i){if(*((void**)
_check_dyneither_subscript(new_d,sizeof(void*),i))!= *((void**)
_check_dyneither_subscript(_tmp1D4,sizeof(void*),i))){change=1;break;}}}if(!
change)return r2;return(void*)({struct Cyc_CfFlowInfo_Aggregate_struct*_tmp1D5=
_region_malloc(((env->joinenv).fenv)->r,sizeof(*_tmp1D5));_tmp1D5[0]=({struct Cyc_CfFlowInfo_Aggregate_struct
_tmp1D6;_tmp1D6.tag=4;_tmp1D6.f1=new_d;_tmp1D6;});_tmp1D5;});}_LLF7:;_LLF8:(int)
_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp1DB=_cycalloc(sizeof(*
_tmp1DB));_tmp1DB[0]=({struct Cyc_Core_Impossible_struct _tmp1DC;_tmp1DC.tag=Cyc_Core_Impossible;
_tmp1DC.f1=({const char*_tmp1DD="after_absRval -- non-aggregates!";_tag_dyneither(
_tmp1DD,sizeof(char),33);});_tmp1DC;});_tmp1DB;}));_LLF4:;}}static void*Cyc_CfFlowInfo_after_absRval_d(
struct Cyc_CfFlowInfo_AfterEnv*env,int key,void*r1,void*r2){if(r1 == r2)return r1;{
struct Cyc_List_List**_tmp1DE=env->last_field_cell;enum Cyc_CfFlowInfo_WhoIsChanged
_tmp1DF=env->changed;*env->last_field_cell=({struct Cyc_List_List*_tmp1E0=
_region_malloc(((env->joinenv).fenv)->r,sizeof(*_tmp1E0));_tmp1E0->hd=(void*)key;
_tmp1E0->tl=0;_tmp1E0;});env->last_field_cell=(struct Cyc_List_List**)&((struct
Cyc_List_List*)_check_null(*env->last_field_cell))->tl;{void*_tmp1E1=Cyc_CfFlowInfo_after_absRval(
env,r1,r2);env->last_field_cell=_tmp1DE;((struct Cyc_List_List*)_check_null(*env->last_field_cell))->tl=
0;env->changed=_tmp1DF;return _tmp1E1;}}}static void*Cyc_CfFlowInfo_after_root(
struct Cyc_CfFlowInfo_AfterEnv*env,void*root,void*r1,void*r2){if(r1 == r2)return r1;*
env->curr_place=({struct Cyc_CfFlowInfo_Place _tmp1E2;_tmp1E2.root=(void*)root;
_tmp1E2.fields=0;_tmp1E2;});env->last_field_cell=(struct Cyc_List_List**)&(env->curr_place)->fields;
env->changed=Cyc_CfFlowInfo_Neither;return Cyc_CfFlowInfo_after_absRval(env,r1,r2);}
union Cyc_CfFlowInfo_FlowInfo_union Cyc_CfFlowInfo_after_flow(struct Cyc_CfFlowInfo_FlowEnv*
fenv,struct Cyc_Dict_Dict*all_changed,union Cyc_CfFlowInfo_FlowInfo_union f1,union
Cyc_CfFlowInfo_FlowInfo_union f2,struct Cyc_Dict_Dict chg1,struct Cyc_Dict_Dict chg2){
struct _tuple16 _tmp1E4=({struct _tuple16 _tmp1E3;_tmp1E3.f1=f1;_tmp1E3.f2=f2;
_tmp1E3;});union Cyc_CfFlowInfo_FlowInfo_union _tmp1E5;union Cyc_CfFlowInfo_FlowInfo_union
_tmp1E6;union Cyc_CfFlowInfo_FlowInfo_union _tmp1E7;struct Cyc_Dict_Dict _tmp1E8;
struct Cyc_List_List*_tmp1E9;struct Cyc_CfFlowInfo_ConsumeInfo _tmp1EA;union Cyc_CfFlowInfo_FlowInfo_union
_tmp1EB;struct Cyc_Dict_Dict _tmp1EC;struct Cyc_List_List*_tmp1ED;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp1EE;_LLFA: _tmp1E5=_tmp1E4.f1;if(((_tmp1E4.f1).BottomFL).tag != 0)goto _LLFC;
_LLFB: goto _LLFD;_LLFC: _tmp1E6=_tmp1E4.f2;if(((_tmp1E4.f2).BottomFL).tag != 0)goto
_LLFE;_LLFD: return(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp1EF;(_tmp1EF.BottomFL).tag=0;_tmp1EF;});_LLFE: _tmp1E7=_tmp1E4.f1;if(((
_tmp1E4.f1).ReachableFL).tag != 1)goto _LLF9;_tmp1E8=(_tmp1E7.ReachableFL).f1;
_tmp1E9=(_tmp1E7.ReachableFL).f2;_tmp1EA=(_tmp1E7.ReachableFL).f3;_tmp1EB=
_tmp1E4.f2;if(((_tmp1E4.f2).ReachableFL).tag != 1)goto _LLF9;_tmp1EC=(_tmp1EB.ReachableFL).f1;
_tmp1ED=(_tmp1EB.ReachableFL).f2;_tmp1EE=(_tmp1EB.ReachableFL).f3;_LLFF: if(((
_tmp1E8.t == _tmp1EC.t  && _tmp1E9 == _tmp1ED) && _tmp1EA.may_consume == _tmp1EE.may_consume)
 && (_tmp1EA.consumed).t == (_tmp1EE.consumed).t)return f1;{struct _RegionHandle*
_tmp1F0=fenv->r;struct Cyc_CfFlowInfo_Place*_tmp1F1=fenv->dummy_place;struct Cyc_CfFlowInfo_AfterEnv
_tmp1F2=({struct Cyc_CfFlowInfo_AfterEnv _tmp1F7;_tmp1F7.joinenv=({struct Cyc_CfFlowInfo_JoinEnv
_tmp1F8;_tmp1F8.fenv=fenv;_tmp1F8.pile=({struct Cyc_CfFlowInfo_EscPile*_tmp1F9=
_region_malloc(_tmp1F0,sizeof(*_tmp1F9));_tmp1F9->rgn=_tmp1F0;_tmp1F9->places=0;
_tmp1F9;});_tmp1F8.d1=_tmp1E8;_tmp1F8.d2=_tmp1EC;_tmp1F8;});_tmp1F7.chg1=chg1;
_tmp1F7.chg2=chg2;_tmp1F7.curr_place=_tmp1F1;_tmp1F7.last_field_cell=(struct Cyc_List_List**)&
_tmp1F1->fields;_tmp1F7.changed=Cyc_CfFlowInfo_Neither;_tmp1F7;});struct Cyc_Dict_Dict
_tmp1F3=((struct Cyc_Dict_Dict(*)(void*(*f)(struct Cyc_CfFlowInfo_AfterEnv*,void*,
void*,void*),struct Cyc_CfFlowInfo_AfterEnv*env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict
d2))Cyc_Dict_union_two_c)(Cyc_CfFlowInfo_after_root,& _tmp1F2,_tmp1E8,_tmp1EC);
struct Cyc_List_List*_tmp1F4=Cyc_CfFlowInfo_join_relns(fenv->r,_tmp1E9,_tmp1ED);
struct Cyc_CfFlowInfo_ConsumeInfo _tmp1F5=Cyc_CfFlowInfo_and_consume(fenv->r,
_tmp1EA,_tmp1EE);return(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp1F6;(_tmp1F6.ReachableFL).tag=1;(_tmp1F6.ReachableFL).f1=Cyc_CfFlowInfo_escape_these(
fenv,(_tmp1F2.joinenv).pile,all_changed,_tmp1F3);(_tmp1F6.ReachableFL).f2=
_tmp1F4;(_tmp1F6.ReachableFL).f3=_tmp1F5;_tmp1F6;});}_LLF9:;}static int Cyc_CfFlowInfo_tag_cmps_lessthan_approx(
struct Cyc_List_List*l1,struct Cyc_List_List*l2){for(0;l2 != 0;l2=l2->tl){struct Cyc_CfFlowInfo_TagCmp
_tmp1FB;void*_tmp1FC;void*_tmp1FD;struct Cyc_CfFlowInfo_TagCmp*_tmp1FA=(struct Cyc_CfFlowInfo_TagCmp*)
l2->hd;_tmp1FB=*_tmp1FA;_tmp1FC=(void*)_tmp1FB.cmp;_tmp1FD=(void*)_tmp1FB.bd;{
struct Cyc_List_List*_tmp1FE=l1;for(0;_tmp1FE != 0;_tmp1FE=_tmp1FE->tl){struct Cyc_CfFlowInfo_TagCmp
_tmp200;void*_tmp201;void*_tmp202;struct Cyc_CfFlowInfo_TagCmp*_tmp1FF=(struct Cyc_CfFlowInfo_TagCmp*)
_tmp1FE->hd;_tmp200=*_tmp1FF;_tmp201=(void*)_tmp200.cmp;_tmp202=(void*)_tmp200.bd;
if(_tmp201 == _tmp1FC  && Cyc_Tcutil_typecmp(_tmp202,_tmp1FD)== 0)break;}if(
_tmp1FE == 0)return 0;}}return 1;}static int Cyc_CfFlowInfo_absRval_lessthan_approx(
void*ignore,void*r1,void*r2){if(r1 == r2)return 1;{struct _tuple0 _tmp204=({struct
_tuple0 _tmp203;_tmp203.f1=r1;_tmp203.f2=r2;_tmp203;});void*_tmp205;struct Cyc_CfFlowInfo_Place*
_tmp206;void*_tmp207;struct Cyc_CfFlowInfo_Place*_tmp208;void*_tmp209;void*
_tmp20A;void*_tmp20B;struct _dyneither_ptr _tmp20C;void*_tmp20D;struct
_dyneither_ptr _tmp20E;void*_tmp20F;void*_tmp210;void*_tmp211;void*_tmp212;void*
_tmp213;void*_tmp214;void*_tmp215;struct Cyc_List_List*_tmp216;void*_tmp217;
struct Cyc_List_List*_tmp218;void*_tmp219;_LL101: _tmp205=_tmp204.f1;if(_tmp205 <= (
void*)3)goto _LL103;if(*((int*)_tmp205)!= 2)goto _LL103;_tmp206=((struct Cyc_CfFlowInfo_AddressOf_struct*)
_tmp205)->f1;_tmp207=_tmp204.f2;if(_tmp207 <= (void*)3)goto _LL103;if(*((int*)
_tmp207)!= 2)goto _LL103;_tmp208=((struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp207)->f1;
_LL102: return Cyc_CfFlowInfo_place_cmp(_tmp206,_tmp208)== 0;_LL103: _tmp209=
_tmp204.f1;if(_tmp209 <= (void*)3)goto _LL105;if(*((int*)_tmp209)!= 2)goto _LL105;
_LL104: goto _LL106;_LL105: _tmp20A=_tmp204.f2;if(_tmp20A <= (void*)3)goto _LL107;if(*((
int*)_tmp20A)!= 2)goto _LL107;_LL106: return 0;_LL107: _tmp20B=_tmp204.f1;if(_tmp20B
<= (void*)3)goto _LL109;if(*((int*)_tmp20B)!= 4)goto _LL109;_tmp20C=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp20B)->f1;_tmp20D=_tmp204.f2;if(_tmp20D <= (void*)3)goto _LL109;if(*((int*)
_tmp20D)!= 4)goto _LL109;_tmp20E=((struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp20D)->f1;
_LL108: if(_tmp20C.curr == _tmp20E.curr)return 1;{int i=0;for(0;i < 
_get_dyneither_size(_tmp20C,sizeof(void*));++ i){if(!((int(*)(int ignore,void*r1,
void*r2))Cyc_CfFlowInfo_absRval_lessthan_approx)(0,((void**)_tmp20C.curr)[i],*((
void**)_check_dyneither_subscript(_tmp20E,sizeof(void*),i))))return 0;}}return 1;
_LL109: _tmp20F=_tmp204.f2;if((int)_tmp20F != 2)goto _LL10B;_LL10A: return r1 == (void*)
1;_LL10B: _tmp210=_tmp204.f2;if((int)_tmp210 != 0)goto _LL10D;_LL10C: goto _LL10E;
_LL10D: _tmp211=_tmp204.f2;if((int)_tmp211 != 1)goto _LL10F;_LL10E: return 0;_LL10F:
_tmp212=_tmp204.f1;if(_tmp212 <= (void*)3)goto _LL111;if(*((int*)_tmp212)!= 1)goto
_LL111;_tmp213=_tmp204.f2;if(_tmp213 <= (void*)3)goto _LL111;if(*((int*)_tmp213)!= 
1)goto _LL111;_LL110: goto _LL100;_LL111: _tmp214=_tmp204.f1;if(_tmp214 <= (void*)3)
goto _LL113;if(*((int*)_tmp214)!= 1)goto _LL113;_LL112: return 0;_LL113: _tmp215=
_tmp204.f1;if(_tmp215 <= (void*)3)goto _LL115;if(*((int*)_tmp215)!= 3)goto _LL115;
_tmp216=((struct Cyc_CfFlowInfo_TagCmps_struct*)_tmp215)->f1;_tmp217=_tmp204.f2;
if(_tmp217 <= (void*)3)goto _LL115;if(*((int*)_tmp217)!= 3)goto _LL115;_tmp218=((
struct Cyc_CfFlowInfo_TagCmps_struct*)_tmp217)->f1;_LL114: return Cyc_CfFlowInfo_tag_cmps_lessthan_approx(
_tmp216,_tmp218);_LL115: _tmp219=_tmp204.f2;if(_tmp219 <= (void*)3)goto _LL117;if(*((
int*)_tmp219)!= 3)goto _LL117;_LL116: return 0;_LL117:;_LL118: goto _LL100;_LL100:;}{
struct _tuple0 _tmp21B=({struct _tuple0 _tmp21A;_tmp21A.f1=Cyc_CfFlowInfo_initlevel_approx(
r1);_tmp21A.f2=Cyc_CfFlowInfo_initlevel_approx(r2);_tmp21A;});void*_tmp21C;void*
_tmp21D;void*_tmp21E;void*_tmp21F;void*_tmp220;void*_tmp221;_LL11A: _tmp21C=
_tmp21B.f1;if((int)_tmp21C != 2)goto _LL11C;_tmp21D=_tmp21B.f2;if((int)_tmp21D != 2)
goto _LL11C;_LL11B: return 1;_LL11C: _tmp21E=_tmp21B.f2;if((int)_tmp21E != 0)goto
_LL11E;_LL11D: return 1;_LL11E: _tmp21F=_tmp21B.f1;if((int)_tmp21F != 0)goto _LL120;
_LL11F: return 0;_LL120: _tmp220=_tmp21B.f2;if((int)_tmp220 != 1)goto _LL122;_LL121:
return 1;_LL122: _tmp221=_tmp21B.f1;if((int)_tmp221 != 1)goto _LL119;_LL123: return 0;
_LL119:;}}static int Cyc_CfFlowInfo_relns_approx(struct Cyc_List_List*r2s,struct Cyc_List_List*
r1s){if(r1s == r2s)return 1;for(0;r1s != 0;r1s=r1s->tl){struct Cyc_CfFlowInfo_Reln*
_tmp222=(struct Cyc_CfFlowInfo_Reln*)r1s->hd;int found=0;{struct Cyc_List_List*
_tmp223=r2s;for(0;_tmp223 != 0;_tmp223=_tmp223->tl){struct Cyc_CfFlowInfo_Reln*
_tmp224=(struct Cyc_CfFlowInfo_Reln*)_tmp223->hd;if(_tmp222 == _tmp224  || _tmp222->vd
== _tmp224->vd  && Cyc_CfFlowInfo_same_relop(_tmp222->rop,_tmp224->rop)){found=1;
break;}}}if(!found)return 0;}return 1;}int Cyc_CfFlowInfo_consume_approx(struct Cyc_CfFlowInfo_ConsumeInfo
c1,struct Cyc_CfFlowInfo_ConsumeInfo c2){if(c1.may_consume != c2.may_consume){
struct Cyc_List_List*_tmp225=c1.may_consume;struct Cyc_List_List*_tmp226=c2.may_consume;
for(0;_tmp226 != 0  && _tmp225 != 0;_tmp226=_tmp226->tl){if((unsigned int)_tmp225 == (
unsigned int)_tmp226)goto compare_consumed_set;{int _tmp227=Cyc_CfFlowInfo_place_cmp((
struct Cyc_CfFlowInfo_Place*)_tmp225->hd,(struct Cyc_CfFlowInfo_Place*)_tmp226->hd);
if(_tmp227 == 0)_tmp225=_tmp225->tl;}}if(_tmp225 != 0)return 0;}
compare_consumed_set: return((int(*)(struct Cyc_Dict_Dict s1,struct Cyc_Dict_Dict s2))
Cyc_CfFlowInfo_place_set_subset)(c1.consumed,c2.consumed);}int Cyc_CfFlowInfo_flow_lessthan_approx(
union Cyc_CfFlowInfo_FlowInfo_union f1,union Cyc_CfFlowInfo_FlowInfo_union f2){
struct _tuple16 _tmp229=({struct _tuple16 _tmp228;_tmp228.f1=f1;_tmp228.f2=f2;
_tmp228;});union Cyc_CfFlowInfo_FlowInfo_union _tmp22A;union Cyc_CfFlowInfo_FlowInfo_union
_tmp22B;union Cyc_CfFlowInfo_FlowInfo_union _tmp22C;struct Cyc_Dict_Dict _tmp22D;
struct Cyc_List_List*_tmp22E;struct Cyc_CfFlowInfo_ConsumeInfo _tmp22F;union Cyc_CfFlowInfo_FlowInfo_union
_tmp230;struct Cyc_Dict_Dict _tmp231;struct Cyc_List_List*_tmp232;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp233;_LL125: _tmp22A=_tmp229.f1;if(((_tmp229.f1).BottomFL).tag != 0)goto _LL127;
_LL126: return 1;_LL127: _tmp22B=_tmp229.f2;if(((_tmp229.f2).BottomFL).tag != 0)goto
_LL129;_LL128: return 0;_LL129: _tmp22C=_tmp229.f1;if(((_tmp229.f1).ReachableFL).tag
!= 1)goto _LL124;_tmp22D=(_tmp22C.ReachableFL).f1;_tmp22E=(_tmp22C.ReachableFL).f2;
_tmp22F=(_tmp22C.ReachableFL).f3;_tmp230=_tmp229.f2;if(((_tmp229.f2).ReachableFL).tag
!= 1)goto _LL124;_tmp231=(_tmp230.ReachableFL).f1;_tmp232=(_tmp230.ReachableFL).f2;
_tmp233=(_tmp230.ReachableFL).f3;_LL12A: if(((_tmp22D.t == _tmp231.t  && _tmp22E == 
_tmp232) && _tmp22F.may_consume == _tmp233.may_consume) && (_tmp22F.consumed).t == (
_tmp233.consumed).t)return 1;return(Cyc_Dict_forall_intersect(Cyc_CfFlowInfo_absRval_lessthan_approx,
_tmp22D,_tmp231) && Cyc_CfFlowInfo_relns_approx(_tmp22E,_tmp232)) && Cyc_CfFlowInfo_consume_approx(
_tmp22F,_tmp233);_LL124:;}struct Cyc_List_List*Cyc_CfFlowInfo_reln_kill_var(
struct _RegionHandle*rgn,struct Cyc_List_List*rs,struct Cyc_Absyn_Vardecl*v){struct
Cyc_List_List*p;int found=0;for(p=rs;!found  && p != 0;p=p->tl){struct Cyc_CfFlowInfo_Reln*
_tmp234=(struct Cyc_CfFlowInfo_Reln*)p->hd;if(_tmp234->vd == v){found=1;break;}{
union Cyc_CfFlowInfo_RelnOp_union _tmp235=_tmp234->rop;struct Cyc_Absyn_Vardecl*
_tmp236;struct Cyc_Absyn_Vardecl*_tmp237;struct Cyc_Absyn_Vardecl*_tmp238;_LL12C:
if((_tmp235.LessVar).tag != 1)goto _LL12E;_tmp236=(_tmp235.LessVar).f1;_LL12D:
_tmp237=_tmp236;goto _LL12F;_LL12E: if((_tmp235.LessNumelts).tag != 2)goto _LL130;
_tmp237=(_tmp235.LessNumelts).f1;_LL12F: _tmp238=_tmp237;goto _LL131;_LL130: if((
_tmp235.LessEqNumelts).tag != 4)goto _LL132;_tmp238=(_tmp235.LessEqNumelts).f1;
_LL131: if(v == _tmp238)found=1;goto _LL12B;_LL132:;_LL133: goto _LL12B;_LL12B:;}}if(
!found)return rs;{struct Cyc_List_List*_tmp239=0;for(p=rs;p != 0;p=p->tl){struct Cyc_CfFlowInfo_Reln*
_tmp23A=(struct Cyc_CfFlowInfo_Reln*)p->hd;if(_tmp23A->vd != v){{union Cyc_CfFlowInfo_RelnOp_union
_tmp23B=_tmp23A->rop;struct Cyc_Absyn_Vardecl*_tmp23C;struct Cyc_Absyn_Vardecl*
_tmp23D;struct Cyc_Absyn_Vardecl*_tmp23E;_LL135: if((_tmp23B.LessVar).tag != 1)goto
_LL137;_tmp23C=(_tmp23B.LessVar).f1;_LL136: _tmp23D=_tmp23C;goto _LL138;_LL137: if((
_tmp23B.LessNumelts).tag != 2)goto _LL139;_tmp23D=(_tmp23B.LessNumelts).f1;_LL138:
_tmp23E=_tmp23D;goto _LL13A;_LL139: if((_tmp23B.LessEqNumelts).tag != 4)goto _LL13B;
_tmp23E=(_tmp23B.LessEqNumelts).f1;_LL13A: if(v == _tmp23E)continue;goto _LL134;
_LL13B:;_LL13C: goto _LL134;_LL134:;}_tmp239=({struct Cyc_List_List*_tmp23F=
_region_malloc(rgn,sizeof(*_tmp23F));_tmp23F->hd=_tmp23A;_tmp23F->tl=_tmp239;
_tmp23F;});}}return _tmp239;}}struct Cyc_List_List*Cyc_CfFlowInfo_reln_kill_exp(
struct _RegionHandle*rgn,struct Cyc_List_List*r,struct Cyc_Absyn_Exp*e){{void*
_tmp240=(void*)e->r;void*_tmp241;struct Cyc_Absyn_Vardecl*_tmp242;void*_tmp243;
struct Cyc_Absyn_Vardecl*_tmp244;void*_tmp245;struct Cyc_Absyn_Vardecl*_tmp246;
void*_tmp247;struct Cyc_Absyn_Vardecl*_tmp248;_LL13E: if(*((int*)_tmp240)!= 1)goto
_LL140;_tmp241=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp240)->f2;if(_tmp241 <= (
void*)1)goto _LL140;if(*((int*)_tmp241)!= 0)goto _LL140;_tmp242=((struct Cyc_Absyn_Global_b_struct*)
_tmp241)->f1;_LL13F: _tmp244=_tmp242;goto _LL141;_LL140: if(*((int*)_tmp240)!= 1)
goto _LL142;_tmp243=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp240)->f2;if(
_tmp243 <= (void*)1)goto _LL142;if(*((int*)_tmp243)!= 2)goto _LL142;_tmp244=((
struct Cyc_Absyn_Param_b_struct*)_tmp243)->f1;_LL141: _tmp246=_tmp244;goto _LL143;
_LL142: if(*((int*)_tmp240)!= 1)goto _LL144;_tmp245=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp240)->f2;if(_tmp245 <= (void*)1)goto _LL144;if(*((int*)_tmp245)!= 3)goto _LL144;
_tmp246=((struct Cyc_Absyn_Local_b_struct*)_tmp245)->f1;_LL143: _tmp248=_tmp246;
goto _LL145;_LL144: if(*((int*)_tmp240)!= 1)goto _LL146;_tmp247=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp240)->f2;if(_tmp247 <= (void*)1)goto _LL146;if(*((int*)_tmp247)!= 4)goto _LL146;
_tmp248=((struct Cyc_Absyn_Pat_b_struct*)_tmp247)->f1;_LL145: if(!_tmp248->escapes)
return Cyc_CfFlowInfo_reln_kill_var(rgn,r,_tmp248);goto _LL13D;_LL146:;_LL147: goto
_LL13D;_LL13D:;}return r;}struct Cyc_CfFlowInfo_TagCmp*Cyc_CfFlowInfo_copy_tagcmp(
struct _RegionHandle*r2,struct Cyc_CfFlowInfo_TagCmp*t){return({struct Cyc_CfFlowInfo_TagCmp*
_tmp249=_region_malloc(r2,sizeof(*_tmp249));_tmp249[0]=*t;_tmp249;});}struct Cyc_List_List*
Cyc_CfFlowInfo_copy_tagcmps(struct _RegionHandle*r2,struct Cyc_List_List*ts){
return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_CfFlowInfo_TagCmp*(*
f)(struct _RegionHandle*,struct Cyc_CfFlowInfo_TagCmp*),struct _RegionHandle*env,
struct Cyc_List_List*x))Cyc_List_rmap_c)(r2,Cyc_CfFlowInfo_copy_tagcmp,r2,ts);}
struct Cyc_CfFlowInfo_Reln*Cyc_CfFlowInfo_copy_reln(struct _RegionHandle*r2,struct
Cyc_CfFlowInfo_Reln*r){return({struct Cyc_CfFlowInfo_Reln*_tmp24A=_region_malloc(
r2,sizeof(*_tmp24A));_tmp24A[0]=*r;_tmp24A;});}struct Cyc_List_List*Cyc_CfFlowInfo_copy_relns(
struct _RegionHandle*r2,struct Cyc_List_List*relns){return((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_CfFlowInfo_Reln*(*f)(struct _RegionHandle*,struct
Cyc_CfFlowInfo_Reln*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(
r2,Cyc_CfFlowInfo_copy_reln,r2,relns);}int Cyc_CfFlowInfo_same_relns(struct Cyc_List_List*
r1,struct Cyc_List_List*r2){for(0;r1 != 0  && r2 != 0;(r1=r1->tl,r2=r2->tl)){struct
Cyc_CfFlowInfo_Reln*_tmp24B=(struct Cyc_CfFlowInfo_Reln*)r1->hd;struct Cyc_CfFlowInfo_Reln*
_tmp24C=(struct Cyc_CfFlowInfo_Reln*)r2->hd;if(_tmp24B->vd != _tmp24C->vd  || !Cyc_CfFlowInfo_same_relop(
_tmp24B->rop,_tmp24C->rop))return 0;}if(r1 != 0  || r2 != 0)return 0;else{return 1;}}
struct Cyc_List_List*Cyc_CfFlowInfo_reln_assign_var(struct _RegionHandle*rgn,
struct Cyc_List_List*r,struct Cyc_Absyn_Vardecl*v,struct Cyc_Absyn_Exp*e){if(v->escapes)
return r;r=Cyc_CfFlowInfo_reln_kill_var(rgn,r,v);{void*_tmp24D=(void*)e->r;struct
Cyc_Absyn_MallocInfo _tmp24E;struct Cyc_Absyn_Exp*_tmp24F;int _tmp250;_LL149: if(*((
int*)_tmp24D)!= 35)goto _LL14B;_tmp24E=((struct Cyc_Absyn_Malloc_e_struct*)_tmp24D)->f1;
_tmp24F=_tmp24E.num_elts;_tmp250=_tmp24E.fat_result;if(_tmp250 != 1)goto _LL14B;
_LL14A: malloc_loop: {void*_tmp251=(void*)_tmp24F->r;struct Cyc_Absyn_Exp*_tmp252;
void*_tmp253;struct Cyc_Absyn_Vardecl*_tmp254;void*_tmp255;struct Cyc_Absyn_Vardecl*
_tmp256;void*_tmp257;struct Cyc_Absyn_Vardecl*_tmp258;void*_tmp259;struct Cyc_Absyn_Vardecl*
_tmp25A;_LL14E: if(*((int*)_tmp251)!= 15)goto _LL150;_tmp252=((struct Cyc_Absyn_Cast_e_struct*)
_tmp251)->f2;_LL14F: _tmp24F=_tmp252;goto malloc_loop;_LL150: if(*((int*)_tmp251)!= 
1)goto _LL152;_tmp253=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp251)->f2;if(
_tmp253 <= (void*)1)goto _LL152;if(*((int*)_tmp253)!= 4)goto _LL152;_tmp254=((
struct Cyc_Absyn_Pat_b_struct*)_tmp253)->f1;_LL151: _tmp256=_tmp254;goto _LL153;
_LL152: if(*((int*)_tmp251)!= 1)goto _LL154;_tmp255=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp251)->f2;if(_tmp255 <= (void*)1)goto _LL154;if(*((int*)_tmp255)!= 3)goto _LL154;
_tmp256=((struct Cyc_Absyn_Local_b_struct*)_tmp255)->f1;_LL153: _tmp258=_tmp256;
goto _LL155;_LL154: if(*((int*)_tmp251)!= 1)goto _LL156;_tmp257=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp251)->f2;if(_tmp257 <= (void*)1)goto _LL156;if(*((int*)_tmp257)!= 2)goto _LL156;
_tmp258=((struct Cyc_Absyn_Param_b_struct*)_tmp257)->f1;_LL155: _tmp25A=_tmp258;
goto _LL157;_LL156: if(*((int*)_tmp251)!= 1)goto _LL158;_tmp259=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp251)->f2;if(_tmp259 <= (void*)1)goto _LL158;if(*((int*)_tmp259)!= 0)goto _LL158;
_tmp25A=((struct Cyc_Absyn_Global_b_struct*)_tmp259)->f1;_LL157: if(_tmp25A->escapes)
return r;return({struct Cyc_List_List*_tmp25B=_region_malloc(rgn,sizeof(*_tmp25B));
_tmp25B->hd=({struct Cyc_CfFlowInfo_Reln*_tmp25C=_region_malloc(rgn,sizeof(*
_tmp25C));_tmp25C->vd=_tmp25A;_tmp25C->rop=(union Cyc_CfFlowInfo_RelnOp_union)({
union Cyc_CfFlowInfo_RelnOp_union _tmp25D;(_tmp25D.LessEqNumelts).tag=4;(_tmp25D.LessEqNumelts).f1=
v;_tmp25D;});_tmp25C;});_tmp25B->tl=r;_tmp25B;});_LL158:;_LL159: return r;_LL14D:;}
_LL14B:;_LL14C: goto _LL148;_LL148:;}{void*_tmp25E=Cyc_Tcutil_compress((void*)v->type);
_LL15B: if(_tmp25E <= (void*)4)goto _LL15D;if(*((int*)_tmp25E)!= 5)goto _LL15D;
_LL15C: goto _LL15A;_LL15D:;_LL15E: return r;_LL15A:;}loop: {void*_tmp25F=(void*)e->r;
struct Cyc_Absyn_Exp*_tmp260;union Cyc_Absyn_Cnst_union _tmp261;int _tmp262;void*
_tmp263;struct Cyc_List_List*_tmp264;struct Cyc_List_List _tmp265;struct Cyc_List_List*
_tmp266;struct Cyc_List_List _tmp267;struct Cyc_Absyn_Exp*_tmp268;void*_tmp269;
struct Cyc_List_List*_tmp26A;struct Cyc_List_List _tmp26B;struct Cyc_Absyn_Exp*
_tmp26C;_LL160: if(*((int*)_tmp25F)!= 15)goto _LL162;_tmp260=((struct Cyc_Absyn_Cast_e_struct*)
_tmp25F)->f2;_LL161: e=_tmp260;goto loop;_LL162: if(*((int*)_tmp25F)!= 0)goto _LL164;
_tmp261=((struct Cyc_Absyn_Const_e_struct*)_tmp25F)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)
_tmp25F)->f1).Int_c).tag != 2)goto _LL164;_tmp262=(_tmp261.Int_c).f2;_LL163: return({
struct Cyc_List_List*_tmp26D=_region_malloc(rgn,sizeof(*_tmp26D));_tmp26D->hd=({
struct Cyc_CfFlowInfo_Reln*_tmp26E=_region_malloc(rgn,sizeof(*_tmp26E));_tmp26E->vd=
v;_tmp26E->rop=(union Cyc_CfFlowInfo_RelnOp_union)({union Cyc_CfFlowInfo_RelnOp_union
_tmp26F;(_tmp26F.EqualConst).tag=0;(_tmp26F.EqualConst).f1=(unsigned int)_tmp262;
_tmp26F;});_tmp26E;});_tmp26D->tl=r;_tmp26D;});_LL164: if(*((int*)_tmp25F)!= 3)
goto _LL166;_tmp263=(void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp25F)->f1;if((
int)_tmp263 != 4)goto _LL166;_tmp264=((struct Cyc_Absyn_Primop_e_struct*)_tmp25F)->f2;
if(_tmp264 == 0)goto _LL166;_tmp265=*_tmp264;_tmp266=_tmp265.tl;if(_tmp266 == 0)
goto _LL166;_tmp267=*_tmp266;_tmp268=(struct Cyc_Absyn_Exp*)_tmp267.hd;_LL165:{
void*_tmp270=(void*)_tmp268->r;void*_tmp271;struct Cyc_List_List*_tmp272;struct
Cyc_List_List _tmp273;struct Cyc_Absyn_Exp*_tmp274;_LL16B: if(*((int*)_tmp270)!= 3)
goto _LL16D;_tmp271=(void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp270)->f1;
_tmp272=((struct Cyc_Absyn_Primop_e_struct*)_tmp270)->f2;if(_tmp272 == 0)goto
_LL16D;_tmp273=*_tmp272;_tmp274=(struct Cyc_Absyn_Exp*)_tmp273.hd;_LL16C:{void*
_tmp275=(void*)_tmp274->r;void*_tmp276;struct Cyc_Absyn_Vardecl*_tmp277;void*
_tmp278;struct Cyc_Absyn_Vardecl*_tmp279;void*_tmp27A;struct Cyc_Absyn_Vardecl*
_tmp27B;void*_tmp27C;struct Cyc_Absyn_Vardecl*_tmp27D;_LL170: if(*((int*)_tmp275)
!= 1)goto _LL172;_tmp276=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp275)->f2;if(
_tmp276 <= (void*)1)goto _LL172;if(*((int*)_tmp276)!= 0)goto _LL172;_tmp277=((
struct Cyc_Absyn_Global_b_struct*)_tmp276)->f1;_LL171: _tmp279=_tmp277;goto _LL173;
_LL172: if(*((int*)_tmp275)!= 1)goto _LL174;_tmp278=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp275)->f2;if(_tmp278 <= (void*)1)goto _LL174;if(*((int*)_tmp278)!= 3)goto _LL174;
_tmp279=((struct Cyc_Absyn_Local_b_struct*)_tmp278)->f1;_LL173: _tmp27B=_tmp279;
goto _LL175;_LL174: if(*((int*)_tmp275)!= 1)goto _LL176;_tmp27A=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp275)->f2;if(_tmp27A <= (void*)1)goto _LL176;if(*((int*)_tmp27A)!= 2)goto _LL176;
_tmp27B=((struct Cyc_Absyn_Param_b_struct*)_tmp27A)->f1;_LL175: _tmp27D=_tmp27B;
goto _LL177;_LL176: if(*((int*)_tmp275)!= 1)goto _LL178;_tmp27C=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp275)->f2;if(_tmp27C <= (void*)1)goto _LL178;if(*((int*)_tmp27C)!= 4)goto _LL178;
_tmp27D=((struct Cyc_Absyn_Pat_b_struct*)_tmp27C)->f1;_LL177: if(_tmp27D->escapes)
return r;return({struct Cyc_List_List*_tmp27E=_region_malloc(rgn,sizeof(*_tmp27E));
_tmp27E->hd=({struct Cyc_CfFlowInfo_Reln*_tmp27F=_region_malloc(rgn,sizeof(*
_tmp27F));_tmp27F->vd=v;_tmp27F->rop=(union Cyc_CfFlowInfo_RelnOp_union)({union
Cyc_CfFlowInfo_RelnOp_union _tmp280;(_tmp280.LessNumelts).tag=2;(_tmp280.LessNumelts).f1=
_tmp27D;_tmp280;});_tmp27F;});_tmp27E->tl=r;_tmp27E;});_LL178:;_LL179: goto _LL16F;
_LL16F:;}goto _LL16A;_LL16D:;_LL16E: goto _LL16A;_LL16A:;}goto _LL15F;_LL166: if(*((
int*)_tmp25F)!= 3)goto _LL168;_tmp269=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp25F)->f1;_tmp26A=((struct Cyc_Absyn_Primop_e_struct*)_tmp25F)->f2;if(_tmp26A
== 0)goto _LL168;_tmp26B=*_tmp26A;_tmp26C=(struct Cyc_Absyn_Exp*)_tmp26B.hd;_LL167:{
void*_tmp281=(void*)_tmp26C->r;void*_tmp282;struct Cyc_Absyn_Vardecl*_tmp283;void*
_tmp284;struct Cyc_Absyn_Vardecl*_tmp285;void*_tmp286;struct Cyc_Absyn_Vardecl*
_tmp287;void*_tmp288;struct Cyc_Absyn_Vardecl*_tmp289;_LL17B: if(*((int*)_tmp281)
!= 1)goto _LL17D;_tmp282=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp281)->f2;if(
_tmp282 <= (void*)1)goto _LL17D;if(*((int*)_tmp282)!= 0)goto _LL17D;_tmp283=((
struct Cyc_Absyn_Global_b_struct*)_tmp282)->f1;_LL17C: _tmp285=_tmp283;goto _LL17E;
_LL17D: if(*((int*)_tmp281)!= 1)goto _LL17F;_tmp284=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp281)->f2;if(_tmp284 <= (void*)1)goto _LL17F;if(*((int*)_tmp284)!= 3)goto _LL17F;
_tmp285=((struct Cyc_Absyn_Local_b_struct*)_tmp284)->f1;_LL17E: _tmp287=_tmp285;
goto _LL180;_LL17F: if(*((int*)_tmp281)!= 1)goto _LL181;_tmp286=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp281)->f2;if(_tmp286 <= (void*)1)goto _LL181;if(*((int*)_tmp286)!= 2)goto _LL181;
_tmp287=((struct Cyc_Absyn_Param_b_struct*)_tmp286)->f1;_LL180: _tmp289=_tmp287;
goto _LL182;_LL181: if(*((int*)_tmp281)!= 1)goto _LL183;_tmp288=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp281)->f2;if(_tmp288 <= (void*)1)goto _LL183;if(*((int*)_tmp288)!= 4)goto _LL183;
_tmp289=((struct Cyc_Absyn_Pat_b_struct*)_tmp288)->f1;_LL182: if(_tmp289->escapes)
return r;return({struct Cyc_List_List*_tmp28A=_region_malloc(rgn,sizeof(*_tmp28A));
_tmp28A->hd=({struct Cyc_CfFlowInfo_Reln*_tmp28B=_region_malloc(rgn,sizeof(*
_tmp28B));_tmp28B->vd=v;_tmp28B->rop=(union Cyc_CfFlowInfo_RelnOp_union)({union
Cyc_CfFlowInfo_RelnOp_union _tmp28C;(_tmp28C.LessEqNumelts).tag=4;(_tmp28C.LessEqNumelts).f1=
_tmp289;_tmp28C;});_tmp28B;});_tmp28A->tl=r;_tmp28A;});_LL183:;_LL184: goto _LL17A;
_LL17A:;}goto _LL15F;_LL168:;_LL169: goto _LL15F;_LL15F:;}return r;}struct Cyc_List_List*
Cyc_CfFlowInfo_reln_assign_exp(struct _RegionHandle*rgn,struct Cyc_List_List*r,
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){{void*_tmp28D=(void*)e1->r;void*
_tmp28E;struct Cyc_Absyn_Vardecl*_tmp28F;void*_tmp290;struct Cyc_Absyn_Vardecl*
_tmp291;void*_tmp292;struct Cyc_Absyn_Vardecl*_tmp293;void*_tmp294;struct Cyc_Absyn_Vardecl*
_tmp295;_LL186: if(*((int*)_tmp28D)!= 1)goto _LL188;_tmp28E=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp28D)->f2;if(_tmp28E <= (void*)1)goto _LL188;if(*((int*)_tmp28E)!= 0)goto _LL188;
_tmp28F=((struct Cyc_Absyn_Global_b_struct*)_tmp28E)->f1;_LL187: _tmp291=_tmp28F;
goto _LL189;_LL188: if(*((int*)_tmp28D)!= 1)goto _LL18A;_tmp290=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp28D)->f2;if(_tmp290 <= (void*)1)goto _LL18A;if(*((int*)_tmp290)!= 2)goto _LL18A;
_tmp291=((struct Cyc_Absyn_Param_b_struct*)_tmp290)->f1;_LL189: _tmp293=_tmp291;
goto _LL18B;_LL18A: if(*((int*)_tmp28D)!= 1)goto _LL18C;_tmp292=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp28D)->f2;if(_tmp292 <= (void*)1)goto _LL18C;if(*((int*)_tmp292)!= 3)goto _LL18C;
_tmp293=((struct Cyc_Absyn_Local_b_struct*)_tmp292)->f1;_LL18B: _tmp295=_tmp293;
goto _LL18D;_LL18C: if(*((int*)_tmp28D)!= 1)goto _LL18E;_tmp294=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp28D)->f2;if(_tmp294 <= (void*)1)goto _LL18E;if(*((int*)_tmp294)!= 4)goto _LL18E;
_tmp295=((struct Cyc_Absyn_Pat_b_struct*)_tmp294)->f1;_LL18D: if(!_tmp295->escapes)
return Cyc_CfFlowInfo_reln_assign_var(rgn,r,_tmp295,e2);goto _LL185;_LL18E:;_LL18F:
goto _LL185;_LL185:;}return r;}static void Cyc_CfFlowInfo_print_reln(struct Cyc_CfFlowInfo_Reln*
r){({struct Cyc_String_pa_struct _tmp298;_tmp298.tag=0;_tmp298.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string((r->vd)->name));{
void*_tmp296[1]={& _tmp298};Cyc_fprintf(Cyc_stderr,({const char*_tmp297="%s";
_tag_dyneither(_tmp297,sizeof(char),3);}),_tag_dyneither(_tmp296,sizeof(void*),1));}});{
union Cyc_CfFlowInfo_RelnOp_union _tmp299=r->rop;unsigned int _tmp29A;struct Cyc_Absyn_Vardecl*
_tmp29B;struct Cyc_Absyn_Vardecl*_tmp29C;unsigned int _tmp29D;struct Cyc_Absyn_Vardecl*
_tmp29E;_LL191: if((_tmp299.EqualConst).tag != 0)goto _LL193;_tmp29A=(_tmp299.EqualConst).f1;
_LL192:({struct Cyc_Int_pa_struct _tmp2A1;_tmp2A1.tag=1;_tmp2A1.f1=(unsigned long)((
int)_tmp29A);{void*_tmp29F[1]={& _tmp2A1};Cyc_fprintf(Cyc_stderr,({const char*
_tmp2A0="==%d";_tag_dyneither(_tmp2A0,sizeof(char),5);}),_tag_dyneither(_tmp29F,
sizeof(void*),1));}});goto _LL190;_LL193: if((_tmp299.LessVar).tag != 1)goto _LL195;
_tmp29B=(_tmp299.LessVar).f1;_LL194:({struct Cyc_String_pa_struct _tmp2A4;_tmp2A4.tag=
0;_tmp2A4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp29B->name));{void*_tmp2A2[1]={& _tmp2A4};Cyc_fprintf(Cyc_stderr,({const char*
_tmp2A3="<%s";_tag_dyneither(_tmp2A3,sizeof(char),4);}),_tag_dyneither(_tmp2A2,
sizeof(void*),1));}});goto _LL190;_LL195: if((_tmp299.LessNumelts).tag != 2)goto
_LL197;_tmp29C=(_tmp299.LessNumelts).f1;_LL196:({struct Cyc_String_pa_struct
_tmp2A7;_tmp2A7.tag=0;_tmp2A7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp29C->name));{void*_tmp2A5[1]={& _tmp2A7};Cyc_fprintf(
Cyc_stderr,({const char*_tmp2A6="<numelts(%s)";_tag_dyneither(_tmp2A6,sizeof(char),
13);}),_tag_dyneither(_tmp2A5,sizeof(void*),1));}});goto _LL190;_LL197: if((
_tmp299.LessConst).tag != 3)goto _LL199;_tmp29D=(_tmp299.LessConst).f1;_LL198:({
struct Cyc_Int_pa_struct _tmp2AA;_tmp2AA.tag=1;_tmp2AA.f1=(unsigned long)((int)
_tmp29D);{void*_tmp2A8[1]={& _tmp2AA};Cyc_fprintf(Cyc_stderr,({const char*_tmp2A9="<%d";
_tag_dyneither(_tmp2A9,sizeof(char),4);}),_tag_dyneither(_tmp2A8,sizeof(void*),1));}});
goto _LL190;_LL199: if((_tmp299.LessEqNumelts).tag != 4)goto _LL190;_tmp29E=(_tmp299.LessEqNumelts).f1;
_LL19A:({struct Cyc_String_pa_struct _tmp2AD;_tmp2AD.tag=0;_tmp2AD.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp29E->name));{
void*_tmp2AB[1]={& _tmp2AD};Cyc_fprintf(Cyc_stderr,({const char*_tmp2AC="<=numelts(%s)";
_tag_dyneither(_tmp2AC,sizeof(char),14);}),_tag_dyneither(_tmp2AB,sizeof(void*),
1));}});goto _LL190;_LL190:;}}void Cyc_CfFlowInfo_print_relns(struct Cyc_List_List*
r){for(0;r != 0;r=r->tl){Cyc_CfFlowInfo_print_reln((struct Cyc_CfFlowInfo_Reln*)r->hd);
if(r->tl != 0)({void*_tmp2AE=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp2AF=",";
_tag_dyneither(_tmp2AF,sizeof(char),2);}),_tag_dyneither(_tmp2AE,sizeof(void*),0));});}}
void Cyc_CfFlowInfo_print_initlevel(void*il){void*_tmp2B0=il;_LL19C: if((int)
_tmp2B0 != 0)goto _LL19E;_LL19D:({void*_tmp2B1=0;Cyc_fprintf(Cyc_stderr,({const
char*_tmp2B2="uninitialized";_tag_dyneither(_tmp2B2,sizeof(char),14);}),
_tag_dyneither(_tmp2B1,sizeof(void*),0));});goto _LL19B;_LL19E: if((int)_tmp2B0 != 
1)goto _LL1A0;_LL19F:({void*_tmp2B3=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp2B4="this-initialized";
_tag_dyneither(_tmp2B4,sizeof(char),17);}),_tag_dyneither(_tmp2B3,sizeof(void*),
0));});goto _LL19B;_LL1A0: if((int)_tmp2B0 != 2)goto _LL19B;_LL1A1:({void*_tmp2B5=0;
Cyc_fprintf(Cyc_stderr,({const char*_tmp2B6="all-initialized";_tag_dyneither(
_tmp2B6,sizeof(char),16);}),_tag_dyneither(_tmp2B5,sizeof(void*),0));});goto
_LL19B;_LL19B:;}void Cyc_CfFlowInfo_print_root(void*root){void*_tmp2B7=root;
struct Cyc_Absyn_Vardecl*_tmp2B8;struct Cyc_Absyn_Exp*_tmp2B9;void*_tmp2BA;_LL1A3:
if(*((int*)_tmp2B7)!= 0)goto _LL1A5;_tmp2B8=((struct Cyc_CfFlowInfo_VarRoot_struct*)
_tmp2B7)->f1;_LL1A4:({struct Cyc_String_pa_struct _tmp2BD;_tmp2BD.tag=0;_tmp2BD.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp2B8->name));{
void*_tmp2BB[1]={& _tmp2BD};Cyc_fprintf(Cyc_stderr,({const char*_tmp2BC="Root(%s)";
_tag_dyneither(_tmp2BC,sizeof(char),9);}),_tag_dyneither(_tmp2BB,sizeof(void*),1));}});
goto _LL1A2;_LL1A5: if(*((int*)_tmp2B7)!= 1)goto _LL1A7;_tmp2B9=((struct Cyc_CfFlowInfo_MallocPt_struct*)
_tmp2B7)->f1;_tmp2BA=(void*)((struct Cyc_CfFlowInfo_MallocPt_struct*)_tmp2B7)->f2;
_LL1A6:({struct Cyc_String_pa_struct _tmp2C1;_tmp2C1.tag=0;_tmp2C1.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp2BA));{struct
Cyc_String_pa_struct _tmp2C0;_tmp2C0.tag=0;_tmp2C0.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_exp2string(_tmp2B9));{void*_tmp2BE[2]={& _tmp2C0,&
_tmp2C1};Cyc_fprintf(Cyc_stderr,({const char*_tmp2BF="MallocPt(%s,%s)";
_tag_dyneither(_tmp2BF,sizeof(char),16);}),_tag_dyneither(_tmp2BE,sizeof(void*),
2));}}});goto _LL1A2;_LL1A7: if(*((int*)_tmp2B7)!= 2)goto _LL1A2;_LL1A8:({void*
_tmp2C2=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp2C3="InitParam(_,_)";
_tag_dyneither(_tmp2C3,sizeof(char),15);}),_tag_dyneither(_tmp2C2,sizeof(void*),
0));});goto _LL1A2;_LL1A2:;}void Cyc_CfFlowInfo_print_place(struct Cyc_CfFlowInfo_Place*
p){Cyc_CfFlowInfo_print_root((void*)p->root);{struct Cyc_List_List*_tmp2C4=p->fields;
while(_tmp2C4 != 0){({struct Cyc_Int_pa_struct _tmp2C7;_tmp2C7.tag=1;_tmp2C7.f1=(
unsigned long)((int)_tmp2C4->hd);{void*_tmp2C5[1]={& _tmp2C7};Cyc_fprintf(Cyc_stderr,({
const char*_tmp2C6=".%d";_tag_dyneither(_tmp2C6,sizeof(char),4);}),_tag_dyneither(
_tmp2C5,sizeof(void*),1));}});_tmp2C4=_tmp2C4->tl;}({void*_tmp2C8=0;Cyc_fprintf(
Cyc_stderr,({const char*_tmp2C9=" ";_tag_dyneither(_tmp2C9,sizeof(char),2);}),
_tag_dyneither(_tmp2C8,sizeof(void*),0));});}}void Cyc_CfFlowInfo_print_list(
struct Cyc_List_List*x,void(*pr)(void*)){int _tmp2CA=1;while(x != 0){if(!_tmp2CA){({
void*_tmp2CB=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp2CC=", ";_tag_dyneither(
_tmp2CC,sizeof(char),3);}),_tag_dyneither(_tmp2CB,sizeof(void*),0));});_tmp2CA=0;}
pr((void*)x->hd);x=x->tl;}({void*_tmp2CD=0;Cyc_fprintf(Cyc_stderr,({const char*
_tmp2CE="\n";_tag_dyneither(_tmp2CE,sizeof(char),2);}),_tag_dyneither(_tmp2CD,
sizeof(void*),0));});}void Cyc_CfFlowInfo_print_dict_set(struct Cyc_Dict_Dict p,
void(*pr)(void*)){struct _RegionHandle _tmp2CF=_new_region("r");struct
_RegionHandle*r=& _tmp2CF;_push_region(r);{struct _handler_cons _tmp2D0;
_push_handler(& _tmp2D0);{int _tmp2D2=0;if(setjmp(_tmp2D0.handler))_tmp2D2=1;if(!
_tmp2D2){{struct Cyc_Iter_Iter _tmp2D3=((struct Cyc_Iter_Iter(*)(struct
_RegionHandle*rgn,struct Cyc_Dict_Dict d))Cyc_Dict_make_iter)(r,p);struct _tuple10
_tmp2D4=*((struct _tuple10*(*)(struct _RegionHandle*r,struct Cyc_Dict_Dict d))Cyc_Dict_rchoose)(
r,p);int first=1;({void*_tmp2D5=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp2D6="{ ";
_tag_dyneither(_tmp2D6,sizeof(char),3);}),_tag_dyneither(_tmp2D5,sizeof(void*),0));});
while(((int(*)(struct Cyc_Iter_Iter,struct _tuple10*))Cyc_Iter_next)(_tmp2D3,&
_tmp2D4)){void*_tmp2D7=_tmp2D4.f1;if(!first){({void*_tmp2D8=0;Cyc_fprintf(Cyc_stderr,({
const char*_tmp2D9=", ";_tag_dyneither(_tmp2D9,sizeof(char),3);}),_tag_dyneither(
_tmp2D8,sizeof(void*),0));});first=0;}pr(_tmp2D7);}({void*_tmp2DA=0;Cyc_fprintf(
Cyc_stderr,({const char*_tmp2DB="}\n";_tag_dyneither(_tmp2DB,sizeof(char),3);}),
_tag_dyneither(_tmp2DA,sizeof(void*),0));});};_pop_handler();}else{void*_tmp2D1=(
void*)_exn_thrown;void*_tmp2DD=_tmp2D1;_LL1AA:;_LL1AB:({void*_tmp2DE=0;Cyc_fprintf(
Cyc_stderr,({const char*_tmp2DF="{ }\n";_tag_dyneither(_tmp2DF,sizeof(char),5);}),
_tag_dyneither(_tmp2DE,sizeof(void*),0));});_npop_handler(0);return;_LL1AC:;
_LL1AD:(void)_throw(_tmp2DD);_LL1A9:;}}};_pop_region(r);}void Cyc_CfFlowInfo_print_absrval(
void*rval){void*_tmp2E0=rval;void*_tmp2E1;void*_tmp2E2;struct Cyc_CfFlowInfo_Place*
_tmp2E3;struct _dyneither_ptr _tmp2E4;_LL1AF: if((int)_tmp2E0 != 0)goto _LL1B1;_LL1B0:({
void*_tmp2E5=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp2E6="Zero";_tag_dyneither(
_tmp2E6,sizeof(char),5);}),_tag_dyneither(_tmp2E5,sizeof(void*),0));});goto
_LL1AE;_LL1B1: if((int)_tmp2E0 != 1)goto _LL1B3;_LL1B2:({void*_tmp2E7=0;Cyc_fprintf(
Cyc_stderr,({const char*_tmp2E8="NotZeroAll";_tag_dyneither(_tmp2E8,sizeof(char),
11);}),_tag_dyneither(_tmp2E7,sizeof(void*),0));});goto _LL1AE;_LL1B3: if((int)
_tmp2E0 != 2)goto _LL1B5;_LL1B4:({void*_tmp2E9=0;Cyc_fprintf(Cyc_stderr,({const
char*_tmp2EA="NotZeroThis";_tag_dyneither(_tmp2EA,sizeof(char),12);}),
_tag_dyneither(_tmp2E9,sizeof(void*),0));});goto _LL1AE;_LL1B5: if(_tmp2E0 <= (void*)
3)goto _LL1B7;if(*((int*)_tmp2E0)!= 0)goto _LL1B7;_tmp2E1=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)
_tmp2E0)->f1;_LL1B6:({void*_tmp2EB=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp2EC="Unknown(";
_tag_dyneither(_tmp2EC,sizeof(char),9);}),_tag_dyneither(_tmp2EB,sizeof(void*),0));});
Cyc_CfFlowInfo_print_initlevel(_tmp2E1);({void*_tmp2ED=0;Cyc_fprintf(Cyc_stderr,({
const char*_tmp2EE=")";_tag_dyneither(_tmp2EE,sizeof(char),2);}),_tag_dyneither(
_tmp2ED,sizeof(void*),0));});goto _LL1AE;_LL1B7: if(_tmp2E0 <= (void*)3)goto _LL1B9;
if(*((int*)_tmp2E0)!= 1)goto _LL1B9;_tmp2E2=(void*)((struct Cyc_CfFlowInfo_Esc_struct*)
_tmp2E0)->f1;_LL1B8:({void*_tmp2EF=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp2F0="Esc(";
_tag_dyneither(_tmp2F0,sizeof(char),5);}),_tag_dyneither(_tmp2EF,sizeof(void*),0));});
Cyc_CfFlowInfo_print_initlevel(_tmp2E2);({void*_tmp2F1=0;Cyc_fprintf(Cyc_stderr,({
const char*_tmp2F2=")";_tag_dyneither(_tmp2F2,sizeof(char),2);}),_tag_dyneither(
_tmp2F1,sizeof(void*),0));});goto _LL1AE;_LL1B9: if(_tmp2E0 <= (void*)3)goto _LL1BB;
if(*((int*)_tmp2E0)!= 2)goto _LL1BB;_tmp2E3=((struct Cyc_CfFlowInfo_AddressOf_struct*)
_tmp2E0)->f1;_LL1BA:({void*_tmp2F3=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp2F4="AddrOf(";
_tag_dyneither(_tmp2F4,sizeof(char),8);}),_tag_dyneither(_tmp2F3,sizeof(void*),0));});
Cyc_CfFlowInfo_print_place(_tmp2E3);({void*_tmp2F5=0;Cyc_fprintf(Cyc_stderr,({
const char*_tmp2F6=")";_tag_dyneither(_tmp2F6,sizeof(char),2);}),_tag_dyneither(
_tmp2F5,sizeof(void*),0));});goto _LL1AE;_LL1BB: if(_tmp2E0 <= (void*)3)goto _LL1BD;
if(*((int*)_tmp2E0)!= 3)goto _LL1BD;_LL1BC:({void*_tmp2F7=0;Cyc_fprintf(Cyc_stderr,({
const char*_tmp2F8="TagCmps(?)";_tag_dyneither(_tmp2F8,sizeof(char),11);}),
_tag_dyneither(_tmp2F7,sizeof(void*),0));});goto _LL1AE;_LL1BD: if(_tmp2E0 <= (void*)
3)goto _LL1AE;if(*((int*)_tmp2E0)!= 4)goto _LL1AE;_tmp2E4=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp2E0)->f1;_LL1BE:({void*_tmp2F9=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp2FA="Aggregate{";
_tag_dyneither(_tmp2FA,sizeof(char),11);}),_tag_dyneither(_tmp2F9,sizeof(void*),
0));});{int i=0;for(0;i < _get_dyneither_size(_tmp2E4,sizeof(void*));++ i){Cyc_CfFlowInfo_print_absrval(((
void**)_tmp2E4.curr)[i]);if(i + 1 < _get_dyneither_size(_tmp2E4,sizeof(void*)))({
void*_tmp2FB=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp2FC=",";_tag_dyneither(
_tmp2FC,sizeof(char),2);}),_tag_dyneither(_tmp2FB,sizeof(void*),0));});}}({void*
_tmp2FD=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp2FE="}";_tag_dyneither(_tmp2FE,
sizeof(char),2);}),_tag_dyneither(_tmp2FD,sizeof(void*),0));});goto _LL1AE;_LL1AE:;}
static void Cyc_CfFlowInfo_print_flow_mapping(void*root,void*rval){({void*_tmp2FF=
0;Cyc_fprintf(Cyc_stderr,({const char*_tmp300="    ";_tag_dyneither(_tmp300,
sizeof(char),5);}),_tag_dyneither(_tmp2FF,sizeof(void*),0));});Cyc_CfFlowInfo_print_root(
root);({void*_tmp301=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp302=" --> ";
_tag_dyneither(_tmp302,sizeof(char),6);}),_tag_dyneither(_tmp301,sizeof(void*),0));});
Cyc_CfFlowInfo_print_absrval(rval);({void*_tmp303=0;Cyc_fprintf(Cyc_stderr,({
const char*_tmp304="\n";_tag_dyneither(_tmp304,sizeof(char),2);}),_tag_dyneither(
_tmp303,sizeof(void*),0));});}void Cyc_CfFlowInfo_print_flowdict(struct Cyc_Dict_Dict
d){Cyc_Dict_iter(Cyc_CfFlowInfo_print_flow_mapping,d);}void Cyc_CfFlowInfo_print_flow(
union Cyc_CfFlowInfo_FlowInfo_union f){union Cyc_CfFlowInfo_FlowInfo_union _tmp305=f;
struct Cyc_Dict_Dict _tmp306;struct Cyc_CfFlowInfo_ConsumeInfo _tmp307;_LL1C0: if((
_tmp305.BottomFL).tag != 0)goto _LL1C2;_LL1C1:({void*_tmp308=0;Cyc_fprintf(Cyc_stderr,({
const char*_tmp309="  BottomFL\n";_tag_dyneither(_tmp309,sizeof(char),12);}),
_tag_dyneither(_tmp308,sizeof(void*),0));});goto _LL1BF;_LL1C2: if((_tmp305.ReachableFL).tag
!= 1)goto _LL1BF;_tmp306=(_tmp305.ReachableFL).f1;_tmp307=(_tmp305.ReachableFL).f3;
_LL1C3:({void*_tmp30A=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp30B="  ReachableFL:\n";
_tag_dyneither(_tmp30B,sizeof(char),16);}),_tag_dyneither(_tmp30A,sizeof(void*),
0));});Cyc_Dict_iter(Cyc_CfFlowInfo_print_flow_mapping,_tmp306);({void*_tmp30C=0;
Cyc_fprintf(Cyc_stderr,({const char*_tmp30D="\n    consumed: ";_tag_dyneither(
_tmp30D,sizeof(char),16);}),_tag_dyneither(_tmp30C,sizeof(void*),0));});((void(*)(
struct Cyc_Dict_Dict p,void(*pr)(struct Cyc_CfFlowInfo_Place*)))Cyc_CfFlowInfo_print_dict_set)(
_tmp307.consumed,Cyc_CfFlowInfo_print_place);({void*_tmp30E=0;Cyc_fprintf(Cyc_stderr,({
const char*_tmp30F="\n    may_consume: ";_tag_dyneither(_tmp30F,sizeof(char),19);}),
_tag_dyneither(_tmp30E,sizeof(void*),0));});((void(*)(struct Cyc_List_List*x,void(*
pr)(struct Cyc_CfFlowInfo_Place*)))Cyc_CfFlowInfo_print_list)(_tmp307.may_consume,
Cyc_CfFlowInfo_print_place);({void*_tmp310=0;Cyc_fprintf(Cyc_stderr,({const char*
_tmp311="\n";_tag_dyneither(_tmp311,sizeof(char),2);}),_tag_dyneither(_tmp310,
sizeof(void*),0));});goto _LL1BF;_LL1BF:;}int Cyc_CfFlowInfo_contains_region(void*
rgn,void*t){void*_tmp312=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*_tmp313;
struct Cyc_Absyn_TunionInfo _tmp314;struct Cyc_List_List*_tmp315;struct Cyc_Core_Opt*
_tmp316;struct Cyc_List_List*_tmp317;struct Cyc_Absyn_AggrInfo _tmp318;struct Cyc_List_List*
_tmp319;struct Cyc_Absyn_TunionFieldInfo _tmp31A;struct Cyc_List_List*_tmp31B;
struct Cyc_List_List*_tmp31C;struct Cyc_Absyn_PtrInfo _tmp31D;void*_tmp31E;struct
Cyc_Absyn_PtrAtts _tmp31F;void*_tmp320;struct Cyc_List_List*_tmp321;struct Cyc_List_List*
_tmp322;struct Cyc_Absyn_ArrayInfo _tmp323;void*_tmp324;void*_tmp325;void*_tmp326;
void*_tmp327;void*_tmp328;void*_tmp329;_LL1C5: if((int)_tmp312 != 0)goto _LL1C7;
_LL1C6: goto _LL1C8;_LL1C7: if(_tmp312 <= (void*)4)goto _LL1C9;if(*((int*)_tmp312)!= 
5)goto _LL1C9;_LL1C8: goto _LL1CA;_LL1C9: if((int)_tmp312 != 1)goto _LL1CB;_LL1CA: goto
_LL1CC;_LL1CB: if(_tmp312 <= (void*)4)goto _LL1D3;if(*((int*)_tmp312)!= 6)goto
_LL1CD;_LL1CC: goto _LL1CE;_LL1CD: if(*((int*)_tmp312)!= 12)goto _LL1CF;_LL1CE: goto
_LL1D0;_LL1CF: if(*((int*)_tmp312)!= 13)goto _LL1D1;_LL1D0: goto _LL1D2;_LL1D1: if(*((
int*)_tmp312)!= 18)goto _LL1D3;_LL1D2: goto _LL1D4;_LL1D3: if((int)_tmp312 != 2)goto
_LL1D5;_LL1D4: goto _LL1D6;_LL1D5: if(_tmp312 <= (void*)4)goto _LL1D7;if(*((int*)
_tmp312)!= 0)goto _LL1D7;_LL1D6: return 0;_LL1D7: if((int)_tmp312 != 3)goto _LL1D9;
_LL1D8: {void*_tmp32A=rgn;_LL1FA: if((int)_tmp32A != 0)goto _LL1FC;_LL1FB: return 1;
_LL1FC:;_LL1FD: return 0;_LL1F9:;}_LL1D9: if(_tmp312 <= (void*)4)goto _LL1DB;if(*((
int*)_tmp312)!= 1)goto _LL1DB;_tmp313=((struct Cyc_Absyn_VarType_struct*)_tmp312)->f1;
_LL1DA: {void*_tmp32B=rgn;struct Cyc_Absyn_Tvar*_tmp32C;_LL1FF: if((int)_tmp32B != 
0)goto _LL201;_LL200: return 0;_LL201: if(_tmp32B <= (void*)1)goto _LL1FE;if(*((int*)
_tmp32B)!= 0)goto _LL1FE;_tmp32C=((struct Cyc_CfFlowInfo_Region_k_struct*)_tmp32B)->f1;
_LL202: return Cyc_Absyn_tvar_cmp(_tmp313,_tmp32C)== 0;_LL1FE:;}_LL1DB: if(_tmp312
<= (void*)4)goto _LL1DD;if(*((int*)_tmp312)!= 2)goto _LL1DD;_tmp314=((struct Cyc_Absyn_TunionType_struct*)
_tmp312)->f1;_tmp315=_tmp314.targs;_tmp316=_tmp314.rgn;_LL1DC: if((unsigned int)
_tmp316  && Cyc_CfFlowInfo_contains_region(rgn,(void*)_tmp316->v))return 1;_tmp317=
_tmp315;goto _LL1DE;_LL1DD: if(_tmp312 <= (void*)4)goto _LL1DF;if(*((int*)_tmp312)!= 
16)goto _LL1DF;_tmp317=((struct Cyc_Absyn_TypedefType_struct*)_tmp312)->f2;_LL1DE:
_tmp319=_tmp317;goto _LL1E0;_LL1DF: if(_tmp312 <= (void*)4)goto _LL1E1;if(*((int*)
_tmp312)!= 10)goto _LL1E1;_tmp318=((struct Cyc_Absyn_AggrType_struct*)_tmp312)->f1;
_tmp319=_tmp318.targs;_LL1E0: _tmp31B=_tmp319;goto _LL1E2;_LL1E1: if(_tmp312 <= (
void*)4)goto _LL1E3;if(*((int*)_tmp312)!= 3)goto _LL1E3;_tmp31A=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp312)->f1;_tmp31B=_tmp31A.targs;_LL1E2: _tmp31C=_tmp31B;goto _LL1E4;_LL1E3: if(
_tmp312 <= (void*)4)goto _LL1E5;if(*((int*)_tmp312)!= 20)goto _LL1E5;_tmp31C=((
struct Cyc_Absyn_JoinEff_struct*)_tmp312)->f1;_LL1E4: return Cyc_List_exists_c(Cyc_CfFlowInfo_contains_region,
rgn,_tmp31C);_LL1E5: if(_tmp312 <= (void*)4)goto _LL1E7;if(*((int*)_tmp312)!= 4)
goto _LL1E7;_tmp31D=((struct Cyc_Absyn_PointerType_struct*)_tmp312)->f1;_tmp31E=(
void*)_tmp31D.elt_typ;_tmp31F=_tmp31D.ptr_atts;_tmp320=(void*)_tmp31F.rgn;_LL1E6:
return Cyc_CfFlowInfo_contains_region(rgn,_tmp320) || Cyc_CfFlowInfo_contains_region(
rgn,_tmp31E);_LL1E7: if(_tmp312 <= (void*)4)goto _LL1E9;if(*((int*)_tmp312)!= 8)
goto _LL1E9;_LL1E8: return 0;_LL1E9: if(_tmp312 <= (void*)4)goto _LL1EB;if(*((int*)
_tmp312)!= 9)goto _LL1EB;_tmp321=((struct Cyc_Absyn_TupleType_struct*)_tmp312)->f1;
_LL1EA: for(0;_tmp321 != 0;_tmp321=_tmp321->tl){if(Cyc_CfFlowInfo_contains_region(
rgn,(*((struct _tuple11*)_tmp321->hd)).f2))return 1;}return 0;_LL1EB: if(_tmp312 <= (
void*)4)goto _LL1ED;if(*((int*)_tmp312)!= 11)goto _LL1ED;_tmp322=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp312)->f2;_LL1EC: for(0;_tmp322 != 0;_tmp322=_tmp322->tl){if(Cyc_CfFlowInfo_contains_region(
rgn,(void*)((struct Cyc_Absyn_Aggrfield*)_tmp322->hd)->type))return 1;}return 0;
_LL1ED: if(_tmp312 <= (void*)4)goto _LL1EF;if(*((int*)_tmp312)!= 7)goto _LL1EF;
_tmp323=((struct Cyc_Absyn_ArrayType_struct*)_tmp312)->f1;_tmp324=(void*)_tmp323.elt_type;
_LL1EE: _tmp325=_tmp324;goto _LL1F0;_LL1EF: if(_tmp312 <= (void*)4)goto _LL1F1;if(*((
int*)_tmp312)!= 19)goto _LL1F1;_tmp325=(void*)((struct Cyc_Absyn_AccessEff_struct*)
_tmp312)->f1;_LL1F0: _tmp326=_tmp325;goto _LL1F2;_LL1F1: if(_tmp312 <= (void*)4)goto
_LL1F3;if(*((int*)_tmp312)!= 21)goto _LL1F3;_tmp326=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp312)->f1;_LL1F2: return Cyc_CfFlowInfo_contains_region(rgn,_tmp326);_LL1F3: if(
_tmp312 <= (void*)4)goto _LL1F5;if(*((int*)_tmp312)!= 15)goto _LL1F5;_tmp327=(void*)((
struct Cyc_Absyn_DynRgnType_struct*)_tmp312)->f1;_tmp328=(void*)((struct Cyc_Absyn_DynRgnType_struct*)
_tmp312)->f2;_LL1F4: return Cyc_CfFlowInfo_contains_region(rgn,_tmp327) || Cyc_CfFlowInfo_contains_region(
rgn,_tmp328);_LL1F5: if(_tmp312 <= (void*)4)goto _LL1F7;if(*((int*)_tmp312)!= 14)
goto _LL1F7;_tmp329=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)_tmp312)->f1;
_LL1F6: return 0;_LL1F7: if(_tmp312 <= (void*)4)goto _LL1C4;if(*((int*)_tmp312)!= 17)
goto _LL1C4;_LL1F8: return 0;_LL1C4:;}struct _tuple18{struct Cyc_CfFlowInfo_FlowEnv*
f1;struct Cyc_Dict_Dict f2;void*f3;};static void Cyc_CfFlowInfo_kill_root(struct
_tuple18*env,void*root,void*rval){struct _tuple18 _tmp32E;struct Cyc_CfFlowInfo_FlowEnv*
_tmp32F;struct Cyc_Dict_Dict _tmp330;struct Cyc_Dict_Dict*_tmp331;void*_tmp332;
struct _tuple18*_tmp32D=env;_tmp32E=*_tmp32D;_tmp32F=_tmp32E.f1;_tmp330=_tmp32E.f2;
_tmp331=(struct Cyc_Dict_Dict*)&(*_tmp32D).f2;_tmp332=_tmp32E.f3;{void*_tmp333=
root;struct Cyc_Absyn_Vardecl*_tmp334;void*_tmp335;_LL204: if(*((int*)_tmp333)!= 0)
goto _LL206;_tmp334=((struct Cyc_CfFlowInfo_VarRoot_struct*)_tmp333)->f1;_LL205:
if(Cyc_CfFlowInfo_contains_region(_tmp332,(void*)_tmp334->type))rval=Cyc_CfFlowInfo_typ_to_absrval(
_tmp32F,(void*)_tmp334->type,(void*)_tmp32F->unknown_none);*_tmp331=Cyc_Dict_insert(*
_tmp331,root,rval);goto _LL203;_LL206: if(*((int*)_tmp333)!= 1)goto _LL208;_tmp335=(
void*)((struct Cyc_CfFlowInfo_MallocPt_struct*)_tmp333)->f2;_LL207: if(!Cyc_CfFlowInfo_contains_region(
_tmp332,_tmp335))*_tmp331=Cyc_Dict_insert(*_tmp331,root,rval);goto _LL203;_LL208:
if(*((int*)_tmp333)!= 2)goto _LL203;_LL209: goto _LL203;_LL203:;}}static struct Cyc_Dict_Dict
Cyc_CfFlowInfo_kill_flowdict_region(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Dict_Dict
fd,void*rgn){void*_tmp336=Cyc_Tcutil_compress(rgn);struct Cyc_Absyn_Tvar*_tmp337;
_LL20B: if(_tmp336 <= (void*)4)goto _LL20D;if(*((int*)_tmp336)!= 1)goto _LL20D;
_tmp337=((struct Cyc_Absyn_VarType_struct*)_tmp336)->f1;_LL20C: {struct Cyc_CfFlowInfo_Region_k_struct
v=({struct Cyc_CfFlowInfo_Region_k_struct _tmp339;_tmp339.tag=0;_tmp339.f1=_tmp337;
_tmp339;});void*v2=(void*)& v;struct _tuple18 env=({struct _tuple18 _tmp338;_tmp338.f1=
fenv;_tmp338.f2=fenv->mt_flowdict;_tmp338.f3=v2;_tmp338;});((void(*)(void(*f)(
struct _tuple18*,void*,void*),struct _tuple18*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(
Cyc_CfFlowInfo_kill_root,& env,fd);return env.f2;}_LL20D: if((int)_tmp336 != 3)goto
_LL20F;_LL20E: {struct _tuple18 env=({struct _tuple18 _tmp33A;_tmp33A.f1=fenv;
_tmp33A.f2=fenv->mt_flowdict;_tmp33A.f3=(void*)0;_tmp33A;});((void(*)(void(*f)(
struct _tuple18*,void*,void*),struct _tuple18*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(
Cyc_CfFlowInfo_kill_root,& env,fd);return env.f2;}_LL20F:;_LL210:(int)_throw((void*)({
struct Cyc_Core_Impossible_struct*_tmp33B=_cycalloc(sizeof(*_tmp33B));_tmp33B[0]=({
struct Cyc_Core_Impossible_struct _tmp33C;_tmp33C.tag=Cyc_Core_Impossible;_tmp33C.f1=({
const char*_tmp33D="kill_flowdict_region";_tag_dyneither(_tmp33D,sizeof(char),21);});
_tmp33C;});_tmp33B;}));_LL20A:;}static struct Cyc_List_List*Cyc_CfFlowInfo_kill_relns_region(
struct Cyc_List_List*relns,void*rgn){return 0;}union Cyc_CfFlowInfo_FlowInfo_union
Cyc_CfFlowInfo_kill_flow_region(struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo_union
f,void*rgn){union Cyc_CfFlowInfo_FlowInfo_union _tmp33E=f;struct Cyc_Dict_Dict
_tmp33F;struct Cyc_List_List*_tmp340;struct Cyc_CfFlowInfo_ConsumeInfo _tmp341;
_LL212: if((_tmp33E.BottomFL).tag != 0)goto _LL214;_LL213: return f;_LL214: if((
_tmp33E.ReachableFL).tag != 1)goto _LL211;_tmp33F=(_tmp33E.ReachableFL).f1;_tmp340=(
_tmp33E.ReachableFL).f2;_tmp341=(_tmp33E.ReachableFL).f3;_LL215: {struct Cyc_Dict_Dict
_tmp342=Cyc_CfFlowInfo_kill_flowdict_region(fenv,_tmp33F,rgn);struct Cyc_List_List*
_tmp343=Cyc_CfFlowInfo_kill_relns_region(_tmp340,rgn);return(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp344;(_tmp344.ReachableFL).tag=1;(_tmp344.ReachableFL).f1=
_tmp342;(_tmp344.ReachableFL).f2=_tmp343;(_tmp344.ReachableFL).f3=_tmp341;
_tmp344;});}_LL211:;}static struct Cyc_CfFlowInfo_UniquePlace*Cyc_CfFlowInfo_unique_place_of_rec(
struct _RegionHandle*r,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*flds,struct Cyc_List_List*
path){void*_tmp345=(void*)e->r;struct Cyc_Absyn_Exp*_tmp346;struct Cyc_Absyn_Exp*
_tmp347;struct Cyc_Absyn_Exp*_tmp348;void*_tmp349;struct Cyc_Absyn_Vardecl*_tmp34A;
void*_tmp34B;struct Cyc_Absyn_Vardecl*_tmp34C;void*_tmp34D;struct Cyc_Absyn_Vardecl*
_tmp34E;void*_tmp34F;struct Cyc_Absyn_Vardecl*_tmp350;struct Cyc_Absyn_Exp*_tmp351;
struct _dyneither_ptr*_tmp352;struct Cyc_Absyn_Exp*_tmp353;struct Cyc_Absyn_Exp*
_tmp354;struct Cyc_Absyn_Exp*_tmp355;struct Cyc_Absyn_Exp*_tmp356;struct
_dyneither_ptr*_tmp357;_LL217: if(*((int*)_tmp345)!= 15)goto _LL219;_tmp346=((
struct Cyc_Absyn_Cast_e_struct*)_tmp345)->f2;_LL218: _tmp347=_tmp346;goto _LL21A;
_LL219: if(*((int*)_tmp345)!= 13)goto _LL21B;_tmp347=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmp345)->f1;_LL21A: _tmp348=_tmp347;goto _LL21C;_LL21B: if(*((int*)_tmp345)!= 14)
goto _LL21D;_tmp348=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp345)->f1;_LL21C:
return Cyc_CfFlowInfo_unique_place_of_rec(r,_tmp348,flds,path);_LL21D: if(*((int*)
_tmp345)!= 1)goto _LL21F;_tmp349=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp345)->f2;
if(_tmp349 <= (void*)1)goto _LL21F;if(*((int*)_tmp349)!= 2)goto _LL21F;_tmp34A=((
struct Cyc_Absyn_Param_b_struct*)_tmp349)->f1;_LL21E: _tmp34C=_tmp34A;goto _LL220;
_LL21F: if(*((int*)_tmp345)!= 1)goto _LL221;_tmp34B=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp345)->f2;if(_tmp34B <= (void*)1)goto _LL221;if(*((int*)_tmp34B)!= 3)goto _LL221;
_tmp34C=((struct Cyc_Absyn_Local_b_struct*)_tmp34B)->f1;_LL220: _tmp34E=_tmp34C;
goto _LL222;_LL221: if(*((int*)_tmp345)!= 1)goto _LL223;_tmp34D=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp345)->f2;if(_tmp34D <= (void*)1)goto _LL223;if(*((int*)_tmp34D)!= 4)goto _LL223;
_tmp34E=((struct Cyc_Absyn_Pat_b_struct*)_tmp34D)->f1;_LL222: return({struct Cyc_CfFlowInfo_UniquePlace*
_tmp358=_region_malloc(r,sizeof(*_tmp358));_tmp358->place=({struct Cyc_CfFlowInfo_Place
_tmp359;_tmp359.root=(void*)((void*)({struct Cyc_CfFlowInfo_VarRoot_struct*
_tmp35A=_region_malloc(r,sizeof(*_tmp35A));_tmp35A[0]=({struct Cyc_CfFlowInfo_VarRoot_struct
_tmp35B;_tmp35B.tag=0;_tmp35B.f1=_tmp34E;_tmp35B;});_tmp35A;}));_tmp359.fields=
flds;_tmp359;});_tmp358->path=path;_tmp358;});_LL223: if(*((int*)_tmp345)!= 1)
goto _LL225;_tmp34F=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp345)->f2;if(
_tmp34F <= (void*)1)goto _LL225;if(*((int*)_tmp34F)!= 0)goto _LL225;_tmp350=((
struct Cyc_Absyn_Global_b_struct*)_tmp34F)->f1;_LL224:(int)_throw((void*)({struct
Cyc_Core_Impossible_struct*_tmp35C=_cycalloc(sizeof(*_tmp35C));_tmp35C[0]=({
struct Cyc_Core_Impossible_struct _tmp35D;_tmp35D.tag=Cyc_Core_Impossible;_tmp35D.f1=({
const char*_tmp35E="non-unique path";_tag_dyneither(_tmp35E,sizeof(char),16);});
_tmp35D;});_tmp35C;}));_LL225: if(*((int*)_tmp345)!= 24)goto _LL227;_tmp351=((
struct Cyc_Absyn_AggrArrow_e_struct*)_tmp345)->f1;_tmp352=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp345)->f2;_LL226:{void*_tmp35F=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp351->topt))->v);struct Cyc_Absyn_PtrInfo _tmp360;void*_tmp361;
_LL230: if(_tmp35F <= (void*)4)goto _LL232;if(*((int*)_tmp35F)!= 4)goto _LL232;
_tmp360=((struct Cyc_Absyn_PointerType_struct*)_tmp35F)->f1;_tmp361=(void*)
_tmp360.elt_typ;_LL231: if(!Cyc_Absyn_is_union_type(_tmp361))flds=({struct Cyc_List_List*
_tmp362=_region_malloc(r,sizeof(*_tmp362));_tmp362->hd=(void*)Cyc_CfFlowInfo_get_field_index(
_tmp361,_tmp352);_tmp362->tl=flds;_tmp362;});goto _LL22F;_LL232:;_LL233:(int)
_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp363=_cycalloc(sizeof(*
_tmp363));_tmp363[0]=({struct Cyc_Core_Impossible_struct _tmp364;_tmp364.tag=Cyc_Core_Impossible;
_tmp364.f1=({const char*_tmp365="anal_Rexp: AggrArrow ptr";_tag_dyneither(_tmp365,
sizeof(char),25);});_tmp364;});_tmp363;}));_LL22F:;}_tmp353=_tmp351;goto _LL228;
_LL227: if(*((int*)_tmp345)!= 22)goto _LL229;_tmp353=((struct Cyc_Absyn_Deref_e_struct*)
_tmp345)->f1;_LL228: return Cyc_CfFlowInfo_unique_place_of_rec(r,_tmp353,0,({
struct Cyc_List_List*_tmp366=_region_malloc(r,sizeof(*_tmp366));_tmp366->hd=flds;
_tmp366->tl=path;_tmp366;}));_LL229: if(*((int*)_tmp345)!= 25)goto _LL22B;_tmp354=((
struct Cyc_Absyn_Subscript_e_struct*)_tmp345)->f1;_tmp355=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp345)->f2;_LL22A: {void*_tmp367=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp354->topt))->v);struct Cyc_Absyn_PtrInfo _tmp368;void*_tmp369;
struct Cyc_Absyn_PtrAtts _tmp36A;struct Cyc_Absyn_Conref*_tmp36B;_LL235: if(_tmp367
<= (void*)4)goto _LL239;if(*((int*)_tmp367)!= 9)goto _LL237;_LL236: {unsigned int
_tmp36C=(Cyc_Evexp_eval_const_uint_exp(_tmp355)).f1;return Cyc_CfFlowInfo_unique_place_of_rec(
r,_tmp354,({struct Cyc_List_List*_tmp36D=_region_malloc(r,sizeof(*_tmp36D));
_tmp36D->hd=(void*)((int)_tmp36C);_tmp36D->tl=flds;_tmp36D;}),path);}_LL237: if(*((
int*)_tmp367)!= 4)goto _LL239;_tmp368=((struct Cyc_Absyn_PointerType_struct*)
_tmp367)->f1;_tmp369=(void*)_tmp368.elt_typ;_tmp36A=_tmp368.ptr_atts;_tmp36B=
_tmp36A.bounds;_LL238:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp36E=_cycalloc(sizeof(*_tmp36E));_tmp36E[0]=({struct Cyc_Core_Impossible_struct
_tmp36F;_tmp36F.tag=Cyc_Core_Impossible;_tmp36F.f1=({const char*_tmp370="subscripts unsupported for unique places";
_tag_dyneither(_tmp370,sizeof(char),41);});_tmp36F;});_tmp36E;}));_LL239:;_LL23A:(
int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp371=_cycalloc(sizeof(*
_tmp371));_tmp371[0]=({struct Cyc_Core_Impossible_struct _tmp372;_tmp372.tag=Cyc_Core_Impossible;
_tmp372.f1=({const char*_tmp373="unique place subscript -- bad type";
_tag_dyneither(_tmp373,sizeof(char),35);});_tmp372;});_tmp371;}));_LL234:;}
_LL22B: if(*((int*)_tmp345)!= 23)goto _LL22D;_tmp356=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp345)->f1;_tmp357=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp345)->f2;_LL22C:
if(Cyc_Absyn_is_union_type((void*)((struct Cyc_Core_Opt*)_check_null(_tmp356->topt))->v))(
int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp374=_cycalloc(sizeof(*
_tmp374));_tmp374[0]=({struct Cyc_Core_Impossible_struct _tmp375;_tmp375.tag=Cyc_Core_Impossible;
_tmp375.f1=({const char*_tmp376="unions can't contain unique places";
_tag_dyneither(_tmp376,sizeof(char),35);});_tmp375;});_tmp374;}));{int _tmp377=
Cyc_CfFlowInfo_get_field_index((void*)((struct Cyc_Core_Opt*)_check_null(_tmp356->topt))->v,
_tmp357);return Cyc_CfFlowInfo_unique_place_of_rec(r,_tmp356,({struct Cyc_List_List*
_tmp378=_region_malloc(r,sizeof(*_tmp378));_tmp378->hd=(void*)_tmp377;_tmp378->tl=
flds;_tmp378;}),path);}_LL22D:;_LL22E:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp379=_cycalloc(sizeof(*_tmp379));_tmp379[0]=({struct Cyc_Core_Impossible_struct
_tmp37A;_tmp37A.tag=Cyc_Core_Impossible;_tmp37A.f1=({const char*_tmp37B="bad unique path";
_tag_dyneither(_tmp37B,sizeof(char),16);});_tmp37A;});_tmp379;}));_LL216:;}
struct Cyc_CfFlowInfo_UniquePlace*Cyc_CfFlowInfo_unique_place_of(struct
_RegionHandle*r,struct Cyc_Absyn_Exp*e){return Cyc_CfFlowInfo_unique_place_of_rec(
r,e,0,0);}struct _tuple19{struct Cyc_Dict_Dict*f1;struct Cyc_Dict_Dict f2;struct Cyc_Position_Segment*
f3;};static void Cyc_CfFlowInfo_consume_f(struct _tuple19*env,struct Cyc_CfFlowInfo_Place*
place){struct _tuple19 _tmp37D;struct Cyc_Dict_Dict*_tmp37E;struct Cyc_Dict_Dict
_tmp37F;struct Cyc_Position_Segment*_tmp380;struct _tuple19*_tmp37C=env;_tmp37D=*
_tmp37C;_tmp37E=_tmp37D.f1;_tmp37F=_tmp37D.f2;_tmp380=_tmp37D.f3;{void*rval=(
void*)0;if(Cyc_Dict_lookup_bool(_tmp37F,(void*)place->root,& rval) && rval == (
void*)0)return;if(((int(*)(struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*
place,struct Cyc_Position_Segment*loc))Cyc_CfFlowInfo_update_place_set)((struct
Cyc_Dict_Dict*)_tmp37E,place,_tmp380)){struct Cyc_Position_Segment*_tmp381=((
struct Cyc_Position_Segment*(*)(struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*k))
Cyc_Dict_lookup)(*_tmp37E,place);((void(*)(struct Cyc_CfFlowInfo_Place*place,
struct _dyneither_ptr(*toString)(struct Cyc_CfFlowInfo_Place*),struct
_dyneither_ptr err_msg1,struct _dyneither_ptr err_msg2,struct Cyc_Position_Segment*
consumed_loc,struct Cyc_Position_Segment*loc))Cyc_CfFlowInfo_unique_err)(place,
Cyc_CfFlowInfo_place_err_string,({const char*_tmp382="May consume non-aliased pointer %s more than once (cf. %s)";
_tag_dyneither(_tmp382,sizeof(char),59);}),({const char*_tmp383="May consume non-aliased pointer %s more than once";
_tag_dyneither(_tmp383,sizeof(char),50);}),_tmp381,_tmp380);}}}union Cyc_CfFlowInfo_FlowInfo_union
Cyc_CfFlowInfo_consume_unique_rvals(struct Cyc_Position_Segment*loc,union Cyc_CfFlowInfo_FlowInfo_union
f){union Cyc_CfFlowInfo_FlowInfo_union _tmp384=f;struct Cyc_Dict_Dict _tmp385;struct
Cyc_List_List*_tmp386;struct Cyc_CfFlowInfo_ConsumeInfo _tmp387;_LL23C: if((_tmp384.BottomFL).tag
!= 0)goto _LL23E;_LL23D: return f;_LL23E: if((_tmp384.ReachableFL).tag != 1)goto
_LL23B;_tmp385=(_tmp384.ReachableFL).f1;_tmp386=(_tmp384.ReachableFL).f2;_tmp387=(
_tmp384.ReachableFL).f3;_LL23F: if(_tmp387.may_consume == 0)return f;{struct Cyc_Dict_Dict
_tmp388=_tmp387.consumed;struct _tuple19 _tmp389=({struct _tuple19 _tmp38C;_tmp38C.f1=&
_tmp388;_tmp38C.f2=_tmp385;_tmp38C.f3=loc;_tmp38C;});((void(*)(void(*f)(struct
_tuple19*,struct Cyc_CfFlowInfo_Place*),struct _tuple19*env,struct Cyc_List_List*x))
Cyc_List_iter_c)(Cyc_CfFlowInfo_consume_f,& _tmp389,_tmp387.may_consume);return(
union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union _tmp38A;(
_tmp38A.ReachableFL).tag=1;(_tmp38A.ReachableFL).f1=_tmp385;(_tmp38A.ReachableFL).f2=
_tmp386;(_tmp38A.ReachableFL).f3=({struct Cyc_CfFlowInfo_ConsumeInfo _tmp38B;
_tmp38B.consumed=_tmp388;_tmp38B.may_consume=0;_tmp38B;});_tmp38A;});}_LL23B:;}
void Cyc_CfFlowInfo_check_unique_rvals(struct Cyc_Position_Segment*loc,union Cyc_CfFlowInfo_FlowInfo_union
f){union Cyc_CfFlowInfo_FlowInfo_union _tmp38D=f;struct Cyc_Dict_Dict _tmp38E;struct
Cyc_List_List*_tmp38F;struct Cyc_CfFlowInfo_ConsumeInfo _tmp390;_LL241: if((_tmp38D.BottomFL).tag
!= 0)goto _LL243;_LL242: return;_LL243: if((_tmp38D.ReachableFL).tag != 1)goto _LL240;
_tmp38E=(_tmp38D.ReachableFL).f1;_tmp38F=(_tmp38D.ReachableFL).f2;_tmp390=(
_tmp38D.ReachableFL).f3;_LL244: {struct Cyc_List_List*_tmp391=_tmp390.may_consume;
while(_tmp391 != 0){if(((int(*)(struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*k))
Cyc_Dict_member)(_tmp390.consumed,(struct Cyc_CfFlowInfo_Place*)_tmp391->hd)){
struct Cyc_Position_Segment*_tmp392=((struct Cyc_Position_Segment*(*)(struct Cyc_Dict_Dict
d,struct Cyc_CfFlowInfo_Place*k))Cyc_Dict_lookup)(_tmp390.consumed,(struct Cyc_CfFlowInfo_Place*)
_tmp391->hd);((void(*)(struct Cyc_CfFlowInfo_Place*place,struct _dyneither_ptr(*
toString)(struct Cyc_CfFlowInfo_Place*),struct _dyneither_ptr err_msg1,struct
_dyneither_ptr err_msg2,struct Cyc_Position_Segment*consumed_loc,struct Cyc_Position_Segment*
loc))Cyc_CfFlowInfo_unique_err)((struct Cyc_CfFlowInfo_Place*)_tmp391->hd,Cyc_CfFlowInfo_place_err_string,({
const char*_tmp393="Read through possibly consumed unique pointer %s (cf. %s)";
_tag_dyneither(_tmp393,sizeof(char),58);}),({const char*_tmp394="Read through possibly consumed unique pointer %s";
_tag_dyneither(_tmp394,sizeof(char),49);}),_tmp392,loc);break;}_tmp391=_tmp391->tl;}
return;}_LL240:;}union Cyc_CfFlowInfo_FlowInfo_union Cyc_CfFlowInfo_drop_unique_rvals(
struct Cyc_Position_Segment*loc,union Cyc_CfFlowInfo_FlowInfo_union f){union Cyc_CfFlowInfo_FlowInfo_union
_tmp395=f;struct Cyc_Dict_Dict _tmp396;struct Cyc_List_List*_tmp397;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp398;_LL246: if((_tmp395.BottomFL).tag != 0)goto _LL248;_LL247: return f;_LL248:
if((_tmp395.ReachableFL).tag != 1)goto _LL245;_tmp396=(_tmp395.ReachableFL).f1;
_tmp397=(_tmp395.ReachableFL).f2;_tmp398=(_tmp395.ReachableFL).f3;_LL249: _tmp398.may_consume=
0;return(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp399;(_tmp399.ReachableFL).tag=1;(_tmp399.ReachableFL).f1=_tmp396;(_tmp399.ReachableFL).f2=
_tmp397;(_tmp399.ReachableFL).f3=_tmp398;_tmp399;});_LL245:;}union Cyc_CfFlowInfo_FlowInfo_union
Cyc_CfFlowInfo_readthrough_unique_rvals(struct Cyc_Position_Segment*loc,union Cyc_CfFlowInfo_FlowInfo_union
f){Cyc_CfFlowInfo_check_unique_rvals(loc,f);{union Cyc_CfFlowInfo_FlowInfo_union
_tmp39A=f;struct Cyc_Dict_Dict _tmp39B;struct Cyc_List_List*_tmp39C;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp39D;_LL24B: if((_tmp39A.BottomFL).tag != 0)goto _LL24D;_LL24C: return f;_LL24D:
if((_tmp39A.ReachableFL).tag != 1)goto _LL24A;_tmp39B=(_tmp39A.ReachableFL).f1;
_tmp39C=(_tmp39A.ReachableFL).f2;_tmp39D=(_tmp39A.ReachableFL).f3;_LL24E: _tmp39D.may_consume=
0;return(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp39E;(_tmp39E.ReachableFL).tag=1;(_tmp39E.ReachableFL).f1=_tmp39B;(_tmp39E.ReachableFL).f2=
_tmp39C;(_tmp39E.ReachableFL).f3=_tmp39D;_tmp39E;});_LL24A:;}}struct _tuple6 Cyc_CfFlowInfo_save_consume_info(
struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo_union f,int clear){
union Cyc_CfFlowInfo_FlowInfo_union _tmp39F=f;struct Cyc_Dict_Dict _tmp3A0;struct Cyc_List_List*
_tmp3A1;struct Cyc_CfFlowInfo_ConsumeInfo _tmp3A2;_LL250: if((_tmp39F.BottomFL).tag
!= 0)goto _LL252;_LL251: return({struct _tuple6 _tmp3A3;_tmp3A3.f1=({struct Cyc_CfFlowInfo_ConsumeInfo
_tmp3A4;_tmp3A4.consumed=fenv->mt_place_set;_tmp3A4.may_consume=0;_tmp3A4;});
_tmp3A3.f2=f;_tmp3A3;});_LL252: if((_tmp39F.ReachableFL).tag != 1)goto _LL24F;
_tmp3A0=(_tmp39F.ReachableFL).f1;_tmp3A1=(_tmp39F.ReachableFL).f2;_tmp3A2=(
_tmp39F.ReachableFL).f3;_LL253: if(clear)return({struct _tuple6 _tmp3A5;_tmp3A5.f1=
_tmp3A2;_tmp3A5.f2=(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp3A6;(_tmp3A6.ReachableFL).tag=1;(_tmp3A6.ReachableFL).f1=_tmp3A0;(_tmp3A6.ReachableFL).f2=
_tmp3A1;(_tmp3A6.ReachableFL).f3=({struct Cyc_CfFlowInfo_ConsumeInfo _tmp3A7;
_tmp3A7.consumed=fenv->mt_place_set;_tmp3A7.may_consume=0;_tmp3A7;});_tmp3A6;});
_tmp3A5;});else{return({struct _tuple6 _tmp3A8;_tmp3A8.f1=_tmp3A2;_tmp3A8.f2=f;
_tmp3A8;});}_LL24F:;}union Cyc_CfFlowInfo_FlowInfo_union Cyc_CfFlowInfo_restore_consume_info(
union Cyc_CfFlowInfo_FlowInfo_union f,struct Cyc_CfFlowInfo_ConsumeInfo c,int
may_consume_only){union Cyc_CfFlowInfo_FlowInfo_union _tmp3A9=f;struct Cyc_Dict_Dict
_tmp3AA;struct Cyc_List_List*_tmp3AB;struct Cyc_CfFlowInfo_ConsumeInfo _tmp3AC;
_LL255: if((_tmp3A9.BottomFL).tag != 0)goto _LL257;_LL256: return f;_LL257: if((
_tmp3A9.ReachableFL).tag != 1)goto _LL254;_tmp3AA=(_tmp3A9.ReachableFL).f1;_tmp3AB=(
_tmp3A9.ReachableFL).f2;_tmp3AC=(_tmp3A9.ReachableFL).f3;_LL258: if(
may_consume_only)return(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp3AD;(_tmp3AD.ReachableFL).tag=1;(_tmp3AD.ReachableFL).f1=_tmp3AA;(_tmp3AD.ReachableFL).f2=
_tmp3AB;(_tmp3AD.ReachableFL).f3=({struct Cyc_CfFlowInfo_ConsumeInfo _tmp3AE;
_tmp3AE.consumed=_tmp3AC.consumed;_tmp3AE.may_consume=c.may_consume;_tmp3AE;});
_tmp3AD;});return(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp3AF;(_tmp3AF.ReachableFL).tag=1;(_tmp3AF.ReachableFL).f1=_tmp3AA;(_tmp3AF.ReachableFL).f2=
_tmp3AB;(_tmp3AF.ReachableFL).f3=c;_tmp3AF;});_LL254:;}
