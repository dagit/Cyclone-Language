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
 void exit(int);void*abort();struct Cyc_Core_NewRegion{struct _DynRegionHandle*
dynregion;};struct Cyc_Core_Opt{void*v;};extern char Cyc_Core_Invalid_argument[21];
struct Cyc_Core_Invalid_argument_struct{char*tag;struct _dyneither_ptr f1;};extern
char Cyc_Core_Failure[12];struct Cyc_Core_Failure_struct{char*tag;struct
_dyneither_ptr f1;};extern char Cyc_Core_Impossible[15];struct Cyc_Core_Impossible_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[14];extern char Cyc_Core_Unreachable[
16];struct Cyc_Core_Unreachable_struct{char*tag;struct _dyneither_ptr f1;};extern
char Cyc_Core_Open_Region[16];extern char Cyc_Core_Free_Region[16];struct
_dyneither_ptr Cstring_to_string(char*);struct Cyc_List_List{void*hd;struct Cyc_List_List*
tl;};struct Cyc_List_List*Cyc_List_list(struct _dyneither_ptr);struct Cyc_List_List*
Cyc_List_rlist(struct _RegionHandle*,struct _dyneither_ptr);struct Cyc_List_List*
Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[
18];struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_imp_rev(struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_append(
struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[8];void*Cyc_List_assoc_cmp(
int(*cmp)(void*,void*),struct Cyc_List_List*l,void*x);struct _dyneither_ptr Cyc_List_to_array(
struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_filter(int(*f)(void*),struct
Cyc_List_List*x);extern char Cyc_Arg_Bad[8];struct Cyc_Arg_Bad_struct{char*tag;
struct _dyneither_ptr f1;};extern char Cyc_Arg_Error[10];struct Cyc_Arg_Unit_spec_struct{
int tag;void(*f1)();};struct Cyc_Arg_Flag_spec_struct{int tag;void(*f1)(struct
_dyneither_ptr);};struct Cyc_Arg_FlagString_spec_struct{int tag;void(*f1)(struct
_dyneither_ptr,struct _dyneither_ptr);};struct Cyc_Arg_Set_spec_struct{int tag;int*
f1;};struct Cyc_Arg_Clear_spec_struct{int tag;int*f1;};struct Cyc_Arg_String_spec_struct{
int tag;void(*f1)(struct _dyneither_ptr);};struct Cyc_Arg_Int_spec_struct{int tag;
void(*f1)(int);};struct Cyc_Arg_Rest_spec_struct{int tag;void(*f1)(struct
_dyneither_ptr);};extern int Cyc_Arg_current;void Cyc_Arg_parse(struct Cyc_List_List*
specs,void(*anonfun)(struct _dyneither_ptr),struct _dyneither_ptr errmsg,struct
_dyneither_ptr args);struct Cyc___cycFILE;extern struct Cyc___cycFILE*Cyc_stdout;
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_Cstdio___abstractFILE;struct Cyc_String_pa_struct{
int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_struct{int tag;unsigned long f1;}
;struct Cyc_Double_pa_struct{int tag;double f1;};struct Cyc_LongDouble_pa_struct{int
tag;long double f1;};struct Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{
int tag;unsigned long*f1;};struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr,
struct _dyneither_ptr);int Cyc_fclose(struct Cyc___cycFILE*);int Cyc_feof(struct Cyc___cycFILE*);
int Cyc_fflush(struct Cyc___cycFILE*);int Cyc_fgetc(struct Cyc___cycFILE*);struct Cyc___cycFILE*
Cyc_fopen(const char*,const char*);int Cyc_fprintf(struct Cyc___cycFILE*,struct
_dyneither_ptr,struct _dyneither_ptr);unsigned int Cyc_fread(struct _dyneither_ptr,
unsigned int,unsigned int,struct Cyc___cycFILE*);struct Cyc_ShortPtr_sa_struct{int
tag;short*f1;};struct Cyc_UShortPtr_sa_struct{int tag;unsigned short*f1;};struct
Cyc_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{int tag;
unsigned int*f1;};struct Cyc_StringPtr_sa_struct{int tag;struct _dyneither_ptr f1;};
struct Cyc_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_struct{
int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int tag;struct _dyneither_ptr f1;};
unsigned int Cyc_fwrite(struct _dyneither_ptr,unsigned int,unsigned int,struct Cyc___cycFILE*);
int Cyc_printf(struct _dyneither_ptr,struct _dyneither_ptr);int remove(const char*);
extern char Cyc_FileCloseError[19];extern char Cyc_FileOpenError[18];struct Cyc_FileOpenError_struct{
char*tag;struct _dyneither_ptr f1;};struct Cyc___cycFILE*Cyc_file_open(struct
_dyneither_ptr,struct _dyneither_ptr);void Cyc_file_close(struct Cyc___cycFILE*);
typedef struct{int quot;int rem;}Cyc_div_t;typedef struct{long quot;long rem;}Cyc_ldiv_t;
void*abort();void exit(int);char*getenv(const char*);int system(const char*);
unsigned int Cyc_strlen(struct _dyneither_ptr s);int Cyc_strcmp(struct _dyneither_ptr
s1,struct _dyneither_ptr s2);int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct
_dyneither_ptr*s2);int Cyc_strncmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2,
unsigned int len);struct _dyneither_ptr Cyc_strconcat(struct _dyneither_ptr,struct
_dyneither_ptr);struct _dyneither_ptr Cyc_strconcat_l(struct Cyc_List_List*);struct
_dyneither_ptr Cyc_str_sepstr(struct Cyc_List_List*,struct _dyneither_ptr);struct
_dyneither_ptr Cyc_strdup(struct _dyneither_ptr src);struct _dyneither_ptr Cyc_substring(
struct _dyneither_ptr,int ofs,unsigned int n);struct _dyneither_ptr Cyc_strchr(struct
_dyneither_ptr s,char c);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*
dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Dict_T;struct Cyc_Dict_Dict{
int(*rel)(void*,void*);struct _RegionHandle*r;struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[
12];extern char Cyc_Dict_Absent[11];struct _tuple0{void*f1;void*f2;};struct _tuple0*
Cyc_Dict_rchoose(struct _RegionHandle*r,struct Cyc_Dict_Dict d);struct _tuple0*Cyc_Dict_rchoose(
struct _RegionHandle*,struct Cyc_Dict_Dict d);struct _dyneither_ptr Cyc_Filename_concat(
struct _dyneither_ptr,struct _dyneither_ptr);struct _dyneither_ptr Cyc_Filename_chop_extension(
struct _dyneither_ptr);struct _dyneither_ptr Cyc_Filename_dirname(struct
_dyneither_ptr);struct _dyneither_ptr Cyc_Filename_basename(struct _dyneither_ptr);
int Cyc_Filename_check_suffix(struct _dyneither_ptr,struct _dyneither_ptr);struct
Cyc_Lineno_Pos{struct _dyneither_ptr logical_file;struct _dyneither_ptr line;int
line_no;int col;};extern char Cyc_Position_Exit[9];void Cyc_Position_reset_position(
struct _dyneither_ptr);struct Cyc_Position_Segment;struct Cyc_Position_Error{struct
_dyneither_ptr source;struct Cyc_Position_Segment*seg;void*kind;struct
_dyneither_ptr desc;};extern char Cyc_Position_Nocontext[14];extern int Cyc_Position_use_gcc_style_location;
extern int Cyc_Position_max_errors;int Cyc_Position_error_p();int isspace(int);
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
Cyc_List_List*rgn_po;struct Cyc_List_List*fields;};struct Cyc_Absyn_Aggrdecl{void*
kind;void*sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*
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
_dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];extern int Cyc_Absyn_porting_c_code;
extern int Cyc_Absyn_no_regions;extern char Cyc_Lexing_Error[10];struct Cyc_Lexing_Error_struct{
char*tag;struct _dyneither_ptr f1;};struct Cyc_Lexing_lexbuf{void(*refill_buff)(
struct Cyc_Lexing_lexbuf*);void*refill_state;struct _dyneither_ptr lex_buffer;int
lex_buffer_len;int lex_abs_pos;int lex_start_pos;int lex_curr_pos;int lex_last_pos;
int lex_last_action;int lex_eof_reached;};struct Cyc_Lexing_function_lexbuf_state{
int(*read_fun)(struct _dyneither_ptr,int,void*);void*read_fun_state;};struct Cyc_Lexing_lex_tables{
struct _dyneither_ptr lex_base;struct _dyneither_ptr lex_backtrk;struct
_dyneither_ptr lex_default;struct _dyneither_ptr lex_trans;struct _dyneither_ptr
lex_check;};struct Cyc_List_List*Cyc_Parse_parse_file(struct Cyc___cycFILE*f);
extern int Cyc_Parse_no_register;struct Cyc_Declaration_spec;struct Cyc_Declarator;
struct Cyc_Abstractdeclarator;struct _tuple4{void*f1;int f2;};struct Cyc_Int_tok_struct{
int tag;struct _tuple4 f1;};struct Cyc_Char_tok_struct{int tag;char f1;};struct Cyc_String_tok_struct{
int tag;struct _dyneither_ptr f1;};struct Cyc_Stringopt_tok_struct{int tag;struct Cyc_Core_Opt*
f1;};struct Cyc_QualId_tok_struct{int tag;struct _tuple1*f1;};struct _tuple5{struct
Cyc_Position_Segment*f1;struct Cyc_Absyn_Conref*f2;struct Cyc_Absyn_Conref*f3;};
struct Cyc_YY1_struct{int tag;struct _tuple5*f1;};struct Cyc_YY2_struct{int tag;
struct Cyc_Absyn_Conref*f1;};struct Cyc_YY3_struct{int tag;struct Cyc_Absyn_Exp*f1;}
;struct Cyc_YY4_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_YY5_struct{int
tag;struct Cyc_List_List*f1;};struct Cyc_YY6_struct{int tag;struct Cyc_List_List*f1;
};struct Cyc_YY7_struct{int tag;void*f1;};struct Cyc_YY8_struct{int tag;struct Cyc_Core_Opt*
f1;};struct Cyc_YY9_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_YY10_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_YY11_struct{int tag;struct Cyc_Absyn_Pat*
f1;};struct _tuple6{struct Cyc_List_List*f1;int f2;};struct Cyc_YY12_struct{int tag;
struct _tuple6*f1;};struct Cyc_YY13_struct{int tag;struct Cyc_List_List*f1;};struct
_tuple7{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_YY14_struct{
int tag;struct _tuple7*f1;};struct Cyc_YY15_struct{int tag;struct Cyc_List_List*f1;};
struct Cyc_YY16_struct{int tag;struct _tuple6*f1;};struct Cyc_YY17_struct{int tag;
struct Cyc_Absyn_Fndecl*f1;};struct Cyc_YY18_struct{int tag;struct Cyc_List_List*f1;
};struct Cyc_YY19_struct{int tag;struct Cyc_Declaration_spec*f1;};struct _tuple8{
struct Cyc_Declarator*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_YY20_struct{int tag;
struct _tuple8*f1;};struct Cyc_YY21_struct{int tag;struct Cyc_List_List*f1;};struct
Cyc_YY22_struct{int tag;void*f1;};struct Cyc_YY23_struct{int tag;void*f1;};struct
Cyc_YY24_struct{int tag;void*f1;};struct Cyc_YY25_struct{int tag;struct Cyc_Absyn_Tqual
f1;};struct Cyc_YY26_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_YY27_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_YY28_struct{int tag;struct Cyc_List_List*
f1;};struct Cyc_YY29_struct{int tag;struct Cyc_Declarator*f1;};struct Cyc_YY30_struct{
int tag;struct Cyc_Abstractdeclarator*f1;};struct Cyc_YY31_struct{int tag;int f1;};
struct Cyc_YY32_struct{int tag;void*f1;};struct Cyc_YY33_struct{int tag;struct Cyc_Absyn_Tunionfield*
f1;};struct Cyc_YY34_struct{int tag;struct Cyc_List_List*f1;};struct _tuple9{struct
Cyc_Absyn_Tqual f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};struct Cyc_YY35_struct{
int tag;struct _tuple9*f1;};struct Cyc_YY36_struct{int tag;struct Cyc_List_List*f1;};
struct Cyc_YY37_struct{int tag;struct _tuple2*f1;};struct Cyc_YY38_struct{int tag;
struct Cyc_List_List*f1;};struct _tuple10{struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*
f3;struct Cyc_Core_Opt*f4;struct Cyc_List_List*f5;};struct Cyc_YY39_struct{int tag;
struct _tuple10*f1;};struct Cyc_YY40_struct{int tag;struct Cyc_List_List*f1;};struct
Cyc_YY41_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_YY42_struct{int tag;
void*f1;};struct Cyc_YY43_struct{int tag;void*f1;};struct Cyc_YY44_struct{int tag;
void*f1;};struct Cyc_YY45_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_YY46_struct{
int tag;void*f1;};struct Cyc_YY47_struct{int tag;struct Cyc_Absyn_Enumfield*f1;};
struct Cyc_YY48_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_YY49_struct{int
tag;struct Cyc_Core_Opt*f1;};struct Cyc_YY50_struct{int tag;struct Cyc_List_List*f1;
};struct Cyc_YY51_struct{int tag;struct Cyc_Absyn_Conref*f1;};struct Cyc_YY52_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_YYINITIALSVAL_struct{int tag;};union Cyc_YYSTYPE_union{
struct Cyc_Int_tok_struct Int_tok;struct Cyc_Char_tok_struct Char_tok;struct Cyc_String_tok_struct
String_tok;struct Cyc_Stringopt_tok_struct Stringopt_tok;struct Cyc_QualId_tok_struct
QualId_tok;struct Cyc_YY1_struct YY1;struct Cyc_YY2_struct YY2;struct Cyc_YY3_struct
YY3;struct Cyc_YY4_struct YY4;struct Cyc_YY5_struct YY5;struct Cyc_YY6_struct YY6;
struct Cyc_YY7_struct YY7;struct Cyc_YY8_struct YY8;struct Cyc_YY9_struct YY9;struct
Cyc_YY10_struct YY10;struct Cyc_YY11_struct YY11;struct Cyc_YY12_struct YY12;struct
Cyc_YY13_struct YY13;struct Cyc_YY14_struct YY14;struct Cyc_YY15_struct YY15;struct
Cyc_YY16_struct YY16;struct Cyc_YY17_struct YY17;struct Cyc_YY18_struct YY18;struct
Cyc_YY19_struct YY19;struct Cyc_YY20_struct YY20;struct Cyc_YY21_struct YY21;struct
Cyc_YY22_struct YY22;struct Cyc_YY23_struct YY23;struct Cyc_YY24_struct YY24;struct
Cyc_YY25_struct YY25;struct Cyc_YY26_struct YY26;struct Cyc_YY27_struct YY27;struct
Cyc_YY28_struct YY28;struct Cyc_YY29_struct YY29;struct Cyc_YY30_struct YY30;struct
Cyc_YY31_struct YY31;struct Cyc_YY32_struct YY32;struct Cyc_YY33_struct YY33;struct
Cyc_YY34_struct YY34;struct Cyc_YY35_struct YY35;struct Cyc_YY36_struct YY36;struct
Cyc_YY37_struct YY37;struct Cyc_YY38_struct YY38;struct Cyc_YY39_struct YY39;struct
Cyc_YY40_struct YY40;struct Cyc_YY41_struct YY41;struct Cyc_YY42_struct YY42;struct
Cyc_YY43_struct YY43;struct Cyc_YY44_struct YY44;struct Cyc_YY45_struct YY45;struct
Cyc_YY46_struct YY46;struct Cyc_YY47_struct YY47;struct Cyc_YY48_struct YY48;struct
Cyc_YY49_struct YY49;struct Cyc_YY50_struct YY50;struct Cyc_YY51_struct YY51;struct
Cyc_YY52_struct YY52;struct Cyc_YYINITIALSVAL_struct YYINITIALSVAL;};struct Cyc_Yyltype{
int timestamp;int first_line;int first_column;int last_line;int last_column;};struct
Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int
expand_typedefs: 1;int qvar_to_Cids: 1;int add_cyc_prefix: 1;int to_VC: 1;int
decls_first: 1;int rewrite_temp_tvars: 1;int print_all_tvars: 1;int print_all_kinds: 1;
int print_all_effects: 1;int print_using_stmts: 1;int print_externC_stmts: 1;int
print_full_evars: 1;int print_zeroterm: 1;int generate_line_directives: 1;int
use_curr_namespace: 1;struct Cyc_List_List*curr_namespace;};void Cyc_Absynpp_set_params(
struct Cyc_Absynpp_Params*fs);extern struct Cyc_Absynpp_Params Cyc_Absynpp_cyc_params_r;
extern struct Cyc_Absynpp_Params Cyc_Absynpp_c_params_r;void Cyc_Absynpp_decllist2file(
struct Cyc_List_List*tdl,struct Cyc___cycFILE*f);void Cyc_Absyndump_set_params(
struct Cyc_Absynpp_Params*fs);void Cyc_Absyndump_dumpdecllist2file(struct Cyc_List_List*
tdl,struct Cyc___cycFILE*f);struct Cyc_Set_Set;extern char Cyc_Set_Absent[11];struct
Cyc_RgnOrder_RgnPO;struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_initial_fn_po(struct
_RegionHandle*,struct Cyc_List_List*tvs,struct Cyc_List_List*po,void*effect,struct
Cyc_Absyn_Tvar*fst_rgn,struct Cyc_Position_Segment*);struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_outlives_constraint(
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
le;int allow_valueof;};struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tc_init(struct _RegionHandle*);
void Cyc_Tcutil_flush_warnings();void Cyc_Tc_tc(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*
te,int var_default_init,struct Cyc_List_List*ds);struct Cyc_List_List*Cyc_Tc_treeshake(
struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*);struct Cyc_List_List*Cyc_Toc_toc(
struct Cyc_List_List*ds);extern int Cyc_Tovc_elim_array_initializers;struct Cyc_List_List*
Cyc_Tovc_tovc(struct Cyc_List_List*decls);struct Cyc_CfFlowInfo_VarRoot_struct{int
tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_CfFlowInfo_MallocPt_struct{int tag;
struct Cyc_Absyn_Exp*f1;void*f2;};struct Cyc_CfFlowInfo_InitParam_struct{int tag;
int f1;void*f2;};struct Cyc_CfFlowInfo_Place{void*root;struct Cyc_List_List*fields;
};struct Cyc_CfFlowInfo_UniquePlace{struct Cyc_CfFlowInfo_Place place;struct Cyc_List_List*
path;};struct Cyc_CfFlowInfo_EqualConst_struct{int tag;unsigned int f1;};struct Cyc_CfFlowInfo_LessVar_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;void*f2;};struct Cyc_CfFlowInfo_LessNumelts_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_CfFlowInfo_LessConst_struct{int tag;
unsigned int f1;};struct Cyc_CfFlowInfo_LessEqNumelts_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};union Cyc_CfFlowInfo_RelnOp_union{struct Cyc_CfFlowInfo_EqualConst_struct
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
Cyc_CfFlowInfo_Aggregate_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_CfFlowInfo_ConsumeInfo{
struct Cyc_Dict_Dict consumed;struct Cyc_List_List*may_consume;};struct Cyc_CfFlowInfo_BottomFL_struct{
int tag;};struct Cyc_CfFlowInfo_ReachableFL_struct{int tag;struct Cyc_Dict_Dict f1;
struct Cyc_List_List*f2;struct Cyc_CfFlowInfo_ConsumeInfo f3;};union Cyc_CfFlowInfo_FlowInfo_union{
struct Cyc_CfFlowInfo_BottomFL_struct BottomFL;struct Cyc_CfFlowInfo_ReachableFL_struct
ReachableFL;};struct Cyc_CfFlowInfo_FlowEnv{struct _RegionHandle*r;void*
unknown_none;void*unknown_this;void*unknown_all;void*esc_none;void*esc_this;void*
esc_all;struct Cyc_Dict_Dict mt_flowdict;struct Cyc_Dict_Dict mt_place_set;struct Cyc_CfFlowInfo_Place*
dummy_place;};struct Cyc_CfFlowInfo_Region_k_struct{int tag;struct Cyc_Absyn_Tvar*
f1;};void Cyc_NewControlFlow_cf_check(struct Cyc_List_List*ds);struct Cyc_Interface_I;
struct Cyc_Interface_I*Cyc_Interface_empty();struct Cyc_Interface_I*Cyc_Interface_final();
struct Cyc_Interface_I*Cyc_Interface_extract(struct Cyc_Dict_Dict ae);struct
_tuple11{struct _dyneither_ptr f1;struct _dyneither_ptr f2;};int Cyc_Interface_is_subinterface(
struct Cyc_Interface_I*i1,struct Cyc_Interface_I*i2,struct _tuple11*info);struct Cyc_Interface_I*
Cyc_Interface_get_and_merge_list(struct Cyc_Interface_I*(*get)(void*),struct Cyc_List_List*
la,struct Cyc_List_List*linfo);struct Cyc_Interface_I*Cyc_Interface_parse(struct
Cyc___cycFILE*);void Cyc_Interface_save(struct Cyc_Interface_I*,struct Cyc___cycFILE*);
struct Cyc_Interface_I*Cyc_Interface_load(struct Cyc___cycFILE*);extern int Cyc_Cyclone_tovc_r;
extern void*Cyc_Cyclone_c_compiler;void Cyc_Port_port(struct Cyc_List_List*);void
Cyc_Lex_lex_init(int use_cyclone_keywords);int Cyc_Toc_warn_bounds_checks;int Cyc_Toc_warn_all_null_deref;
static int Cyc_pp_r=0;int Cyc_noexpand_r=0;static int Cyc_noshake_r=0;static int Cyc_stop_after_cpp_r=
0;static int Cyc_parseonly_r=0;static int Cyc_tc_r=0;static int Cyc_cf_r=0;static int
Cyc_noprint_r=0;static int Cyc_ic_r=0;static int Cyc_toc_r=0;static int Cyc_stop_after_objectfile_r=
0;static int Cyc_stop_after_asmfile_r=0;static int Cyc_inline_functions_r=0;static
int Cyc_elim_se_r=0;static int Cyc_v_r=0;static int Cyc_save_temps_r=0;static int Cyc_save_c_r=
0;static int Cyc_nogc_r=0;static int Cyc_pa_r=0;static int Cyc_pg_r=0;static int Cyc_nocheck_r=
0;static int Cyc_add_cyc_namespace_r=1;static int Cyc_generate_line_directives_r=0;
static int Cyc_print_full_evars_r=0;static int Cyc_print_all_tvars_r=0;static int Cyc_print_all_kinds_r=
0;static int Cyc_print_all_effects_r=0;static int Cyc_nocyc_setjmp_r=0;static struct
Cyc_List_List*Cyc_ccargs=0;static void Cyc_add_ccarg(struct _dyneither_ptr s){Cyc_ccargs=({
struct Cyc_List_List*_tmp0=_cycalloc(sizeof(*_tmp0));_tmp0->hd=({struct
_dyneither_ptr*_tmp1=_cycalloc(sizeof(*_tmp1));_tmp1[0]=s;_tmp1;});_tmp0->tl=Cyc_ccargs;
_tmp0;});}void Cyc_set_c_compiler(struct _dyneither_ptr s){if(Cyc_strcmp((struct
_dyneither_ptr)s,({const char*_tmp2="vc";_tag_dyneither(_tmp2,sizeof(char),3);}))
== 0){Cyc_Cyclone_c_compiler=(void*)1;Cyc_add_ccarg(({const char*_tmp3="-DVC_C";
_tag_dyneither(_tmp3,sizeof(char),7);}));}else{if(Cyc_strcmp((struct
_dyneither_ptr)s,({const char*_tmp4="gcc";_tag_dyneither(_tmp4,sizeof(char),4);}))
== 0){Cyc_Cyclone_c_compiler=(void*)0;Cyc_add_ccarg(({const char*_tmp5="-DGCC_C";
_tag_dyneither(_tmp5,sizeof(char),8);}));}}}void Cyc_set_port_c_code(){Cyc_Absyn_porting_c_code=
1;Cyc_Position_max_errors=65535;Cyc_save_c_r=1;Cyc_parseonly_r=1;}static struct
_dyneither_ptr*Cyc_output_file=0;static void Cyc_set_output_file(struct
_dyneither_ptr s){Cyc_output_file=({struct _dyneither_ptr*_tmp6=_cycalloc(sizeof(*
_tmp6));_tmp6[0]=s;_tmp6;});}extern char*Cdef_inc_path;extern char*Cdef_lib_path;
extern char*Carch;extern char*Ccomp;extern char*Cversion;static char _tmp7[1]="";
static struct _dyneither_ptr Cyc_cpp={_tmp7,_tmp7,_tmp7 + 1};static void Cyc_set_cpp(
struct _dyneither_ptr s){Cyc_cpp=s;}static struct Cyc_List_List*Cyc_cppargs=0;static
void Cyc_add_cpparg(struct _dyneither_ptr s){Cyc_cppargs=({struct Cyc_List_List*
_tmp8=_cycalloc(sizeof(*_tmp8));_tmp8->hd=({struct _dyneither_ptr*_tmp9=_cycalloc(
sizeof(*_tmp9));_tmp9[0]=s;_tmp9;});_tmp8->tl=Cyc_cppargs;_tmp8;});}static struct
_dyneither_ptr Cyc_target_arch=(struct _dyneither_ptr){(void*)0,(void*)0,(void*)(0
+ 0)};static void Cyc_set_target_arch(struct _dyneither_ptr s){Cyc_toc_r=1;Cyc_target_arch=
s;}static void Cyc_print_version(){({struct Cyc_String_pa_struct _tmpC;_tmpC.tag=0;
_tmpC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cstring_to_string(Cversion));{
void*_tmpA[1]={& _tmpC};Cyc_printf(({const char*_tmpB="The Cyclone compiler, version %s\n";
_tag_dyneither(_tmpB,sizeof(char),34);}),_tag_dyneither(_tmpA,sizeof(void*),1));}});({
struct Cyc_String_pa_struct _tmp10;_tmp10.tag=0;_tmp10.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cstring_to_string(Ccomp));{struct Cyc_String_pa_struct _tmpF;
_tmpF.tag=0;_tmpF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cstring_to_string(Carch));{void*_tmpD[2]={& _tmpF,& _tmp10};Cyc_printf(({const char*
_tmpE="Compiled for architecture: %s CC=\"%s\"\n";_tag_dyneither(_tmpE,sizeof(
char),39);}),_tag_dyneither(_tmpD,sizeof(void*),2));}}});({struct Cyc_String_pa_struct
_tmp13;_tmp13.tag=0;_tmp13.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cstring_to_string(Cdef_lib_path));{void*_tmp11[1]={& _tmp13};Cyc_printf(({const
char*_tmp12="Standard library directory: %s\n";_tag_dyneither(_tmp12,sizeof(char),
32);}),_tag_dyneither(_tmp11,sizeof(void*),1));}});({struct Cyc_String_pa_struct
_tmp16;_tmp16.tag=0;_tmp16.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cstring_to_string(Cdef_inc_path));{void*_tmp14[1]={& _tmp16};Cyc_printf(({const
char*_tmp15="Standard include directory: %s\n";_tag_dyneither(_tmp15,sizeof(char),
32);}),_tag_dyneither(_tmp14,sizeof(void*),1));}});exit(0);}static int Cyc_is_cyclone_sourcefile(
struct _dyneither_ptr s){unsigned int _tmp17=Cyc_strlen((struct _dyneither_ptr)s);
if(_tmp17 <= 4)return 0;else{return Cyc_strcmp((struct _dyneither_ptr)
_dyneither_ptr_plus(s,sizeof(char),(int)(_tmp17 - 4)),({const char*_tmp18=".cyc";
_tag_dyneither(_tmp18,sizeof(char),5);}))== 0;}}static struct Cyc_List_List*Cyc_cyclone_exec_path=
0;static void Cyc_add_cyclone_exec_path(struct _dyneither_ptr s){unsigned int _tmp19=
Cyc_strlen((struct _dyneither_ptr)s);if(_tmp19 <= 2)return;{struct _dyneither_ptr
_tmp1A=(struct _dyneither_ptr)Cyc_substring((struct _dyneither_ptr)s,2,_tmp19 - 2);
Cyc_cyclone_exec_path=({struct Cyc_List_List*_tmp1B=_cycalloc(sizeof(*_tmp1B));
_tmp1B->hd=({struct _dyneither_ptr*_tmp1C=_cycalloc(sizeof(*_tmp1C));_tmp1C[0]=
_tmp1A;_tmp1C;});_tmp1B->tl=Cyc_cyclone_exec_path;_tmp1B;});}}static struct Cyc_List_List*
Cyc_libargs=0;static void Cyc_add_libarg(struct _dyneither_ptr s){if(Cyc_strcmp((
struct _dyneither_ptr)s,({const char*_tmp1D="-lxml";_tag_dyneither(_tmp1D,sizeof(
char),6);}))== 0)Cyc_add_ccarg(s);else{Cyc_libargs=({struct Cyc_List_List*_tmp1E=
_cycalloc(sizeof(*_tmp1E));_tmp1E->hd=({struct _dyneither_ptr*_tmp1F=_cycalloc(
sizeof(*_tmp1F));_tmp1F[0]=s;_tmp1F;});_tmp1E->tl=Cyc_libargs;_tmp1E;});}}static
void Cyc_add_marg(struct _dyneither_ptr s){Cyc_add_ccarg(s);}static void Cyc_set_save_temps(){
Cyc_save_temps_r=1;Cyc_add_ccarg(({const char*_tmp20="-save-temps";_tag_dyneither(
_tmp20,sizeof(char),12);}));}static int Cyc_produce_dependencies=0;static void Cyc_set_produce_dependencies(){
Cyc_stop_after_cpp_r=1;Cyc_produce_dependencies=1;Cyc_add_cpparg(({const char*
_tmp21="-M";_tag_dyneither(_tmp21,sizeof(char),3);}));}static struct
_dyneither_ptr*Cyc_dependencies_target=0;static void Cyc_set_dependencies_target(
struct _dyneither_ptr s){Cyc_dependencies_target=({struct _dyneither_ptr*_tmp22=
_cycalloc(sizeof(*_tmp22));_tmp22[0]=s;_tmp22;});}static void Cyc_set_stop_after_objectfile(){
Cyc_stop_after_objectfile_r=1;Cyc_add_ccarg(({const char*_tmp23="-c";
_tag_dyneither(_tmp23,sizeof(char),3);}));}static void Cyc_set_nocppprecomp(){Cyc_add_cpparg(({
const char*_tmp24="-no-cpp-precomp";_tag_dyneither(_tmp24,sizeof(char),16);}));
Cyc_add_ccarg(({const char*_tmp25="-no-cpp-precomp";_tag_dyneither(_tmp25,sizeof(
char),16);}));}static void Cyc_set_lineno(){Cyc_generate_line_directives_r=1;Cyc_set_save_temps();}
static void Cyc_set_inline_functions(){Cyc_inline_functions_r=1;}static void Cyc_set_elim_se(){
Cyc_elim_se_r=1;Cyc_set_inline_functions();}static void Cyc_set_tovc(){Cyc_Cyclone_tovc_r=
1;Cyc_add_ccarg(({const char*_tmp26="-DCYC_ANSI_OUTPUT";_tag_dyneither(_tmp26,
sizeof(char),18);}));Cyc_set_elim_se();}static void Cyc_set_noboundschecks(){Cyc_add_ccarg(({
const char*_tmp27="-DNO_CYC_BOUNDS_CHECKS";_tag_dyneither(_tmp27,sizeof(char),23);}));}
static void Cyc_set_nonullchecks(){Cyc_add_ccarg(({const char*_tmp28="-DNO_CYC_NULL_CHECKS";
_tag_dyneither(_tmp28,sizeof(char),21);}));}static void Cyc_set_nochecks(){Cyc_set_noboundschecks();
Cyc_set_nonullchecks();Cyc_nocheck_r=1;}static void Cyc_set_nocyc(){Cyc_add_cyc_namespace_r=
0;Cyc_add_ccarg(({const char*_tmp29="-DNO_CYC_PREFIX";_tag_dyneither(_tmp29,
sizeof(char),16);}));}static void Cyc_set_pa(){Cyc_pa_r=1;Cyc_add_ccarg(({const
char*_tmp2A="-DCYC_REGION_PROFILE";_tag_dyneither(_tmp2A,sizeof(char),21);}));}
static void Cyc_set_pg(){Cyc_pg_r=1;Cyc_add_ccarg(({const char*_tmp2B="-pg";
_tag_dyneither(_tmp2B,sizeof(char),4);}));}static void Cyc_set_stop_after_asmfile(){
Cyc_stop_after_asmfile_r=1;Cyc_add_ccarg(({const char*_tmp2C="-S";_tag_dyneither(
_tmp2C,sizeof(char),3);}));}static void Cyc_set_all_warnings(){Cyc_Toc_warn_bounds_checks=
1;Cyc_Toc_warn_all_null_deref=1;}static void*Cyc_intype=(void*)0;static void Cyc_set_inputtype(
struct _dyneither_ptr s){if(Cyc_strcmp((struct _dyneither_ptr)s,({const char*_tmp2D="cyc";
_tag_dyneither(_tmp2D,sizeof(char),4);}))== 0)Cyc_intype=(void*)1;else{if(Cyc_strcmp((
struct _dyneither_ptr)s,({const char*_tmp2E="none";_tag_dyneither(_tmp2E,sizeof(
char),5);}))== 0)Cyc_intype=(void*)0;else{({struct Cyc_String_pa_struct _tmp31;
_tmp31.tag=0;_tmp31.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s);{void*
_tmp2F[1]={& _tmp31};Cyc_fprintf(Cyc_stderr,({const char*_tmp30="Input type '%s' not supported\n";
_tag_dyneither(_tmp30,sizeof(char),31);}),_tag_dyneither(_tmp2F,sizeof(void*),1));}});}}}
struct _dyneither_ptr Cyc_make_base_filename(struct _dyneither_ptr s,struct
_dyneither_ptr*output_file){struct _dyneither_ptr _tmp32=Cyc_Filename_dirname(
output_file == 0?s:*output_file);struct _dyneither_ptr _tmp33=Cyc_Filename_chop_extension((
struct _dyneither_ptr)Cyc_Filename_basename(s));struct _dyneither_ptr _tmp34=Cyc_strlen((
struct _dyneither_ptr)_tmp32)> 0?Cyc_Filename_concat((struct _dyneither_ptr)_tmp32,(
struct _dyneither_ptr)_tmp33): _tmp33;return(struct _dyneither_ptr)_tmp34;}static
struct Cyc_List_List*Cyc_cyclone_files=0;static void Cyc_add_other(struct
_dyneither_ptr s){if(Cyc_intype == (void*)1  || Cyc_is_cyclone_sourcefile(s)){Cyc_cyclone_files=({
struct Cyc_List_List*_tmp35=_cycalloc(sizeof(*_tmp35));_tmp35->hd=({struct
_dyneither_ptr*_tmp36=_cycalloc(sizeof(*_tmp36));_tmp36[0]=s;_tmp36;});_tmp35->tl=
Cyc_cyclone_files;_tmp35;});{struct _dyneither_ptr _tmp37=Cyc_make_base_filename(s,
Cyc_output_file);struct _dyneither_ptr _tmp38=Cyc_strconcat((struct _dyneither_ptr)
_tmp37,({const char*_tmp39=".c";_tag_dyneither(_tmp39,sizeof(char),3);}));Cyc_add_ccarg((
struct _dyneither_ptr)_tmp38);}}else{Cyc_add_ccarg(s);}}static void Cyc_remove_file(
struct _dyneither_ptr s){if(Cyc_save_temps_r)return;else{remove((const char*)
_untag_dyneither_ptr(s,sizeof(char),1));}}struct _tuple12{struct _dyneither_ptr*f1;
struct _dyneither_ptr*f2;};struct Cyc_List_List*Cyc_read_specs(struct
_dyneither_ptr file){struct Cyc_List_List*_tmp3A=0;int c;int i;char strname[256];char
strvalue[4096];struct Cyc___cycFILE*spec_file=Cyc_fopen((const char*)
_untag_dyneither_ptr(file,sizeof(char),1),(const char*)"r");if(spec_file == 0){({
struct Cyc_String_pa_struct _tmp3D;_tmp3D.tag=0;_tmp3D.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)file);{void*_tmp3B[1]={& _tmp3D};Cyc_fprintf(Cyc_stderr,({
const char*_tmp3C="Error opening spec file %s\n";_tag_dyneither(_tmp3C,sizeof(
char),28);}),_tag_dyneither(_tmp3B,sizeof(void*),1));}});Cyc_fflush((struct Cyc___cycFILE*)
Cyc_stderr);return 0;}while(1){while(1){c=Cyc_fgetc((struct Cyc___cycFILE*)
spec_file);if(isspace(c))continue;if(c == '*')break;if(c != - 1){({struct Cyc_Int_pa_struct
_tmp41;_tmp41.tag=1;_tmp41.f1=(unsigned long)c;{struct Cyc_String_pa_struct _tmp40;
_tmp40.tag=0;_tmp40.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file);{void*
_tmp3E[2]={& _tmp40,& _tmp41};Cyc_fprintf(Cyc_stderr,({const char*_tmp3F="Error reading spec file %s: unexpected character '%c'\n";
_tag_dyneither(_tmp3F,sizeof(char),55);}),_tag_dyneither(_tmp3E,sizeof(void*),2));}}});
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);}goto CLEANUP_AND_RETURN;}
JUST_AFTER_STAR: i=0;while(1){c=Cyc_fgetc((struct Cyc___cycFILE*)spec_file);if(c == 
- 1){({struct Cyc_String_pa_struct _tmp44;_tmp44.tag=0;_tmp44.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)file);{void*_tmp42[1]={& _tmp44};Cyc_fprintf(
Cyc_stderr,({const char*_tmp43="Error reading spec file %s: unexpected EOF\n";
_tag_dyneither(_tmp43,sizeof(char),44);}),_tag_dyneither(_tmp42,sizeof(void*),1));}});
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);goto CLEANUP_AND_RETURN;}if(c == ':'){
strname[_check_known_subscript_notnull(256,i)]='\000';break;}strname[
_check_known_subscript_notnull(256,i)]=(char)c;++ i;if(i >= 256){({struct Cyc_String_pa_struct
_tmp47;_tmp47.tag=0;_tmp47.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file);{
void*_tmp45[1]={& _tmp47};Cyc_fprintf(Cyc_stderr,({const char*_tmp46="Error reading spec file %s: string name too long\n";
_tag_dyneither(_tmp46,sizeof(char),50);}),_tag_dyneither(_tmp45,sizeof(void*),1));}});
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);goto CLEANUP_AND_RETURN;}}while(1){c=
Cyc_fgetc((struct Cyc___cycFILE*)spec_file);if(isspace(c))continue;break;}if(c == '*'){
_tmp3A=({struct Cyc_List_List*_tmp48=_cycalloc(sizeof(*_tmp48));_tmp48->hd=({
struct _tuple12*_tmp49=_cycalloc(sizeof(*_tmp49));_tmp49->f1=({struct
_dyneither_ptr*_tmp4C=_cycalloc(sizeof(*_tmp4C));_tmp4C[0]=(struct _dyneither_ptr)
Cyc_strdup(_tag_dyneither(strname,sizeof(char),256));_tmp4C;});_tmp49->f2=({
struct _dyneither_ptr*_tmp4A=_cycalloc(sizeof(*_tmp4A));_tmp4A[0]=(struct
_dyneither_ptr)Cyc_strdup(({const char*_tmp4B="";_tag_dyneither(_tmp4B,sizeof(
char),1);}));_tmp4A;});_tmp49;});_tmp48->tl=_tmp3A;_tmp48;});goto JUST_AFTER_STAR;}
strvalue[0]=(char)c;i=1;while(1){c=Cyc_fgetc((struct Cyc___cycFILE*)spec_file);
if((c == - 1  || c == '\n') || c == '\r'){strvalue[_check_known_subscript_notnull(
4096,i)]='\000';break;}strvalue[_check_known_subscript_notnull(4096,i)]=(char)c;
++ i;if(i >= 4096){({struct Cyc_String_pa_struct _tmp50;_tmp50.tag=0;_tmp50.f1=(
struct _dyneither_ptr)_tag_dyneither(strname,sizeof(char),256);{struct Cyc_String_pa_struct
_tmp4F;_tmp4F.tag=0;_tmp4F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file);{
void*_tmp4D[2]={& _tmp4F,& _tmp50};Cyc_fprintf(Cyc_stderr,({const char*_tmp4E="Error reading spec file %s: value of %s too long\n";
_tag_dyneither(_tmp4E,sizeof(char),50);}),_tag_dyneither(_tmp4D,sizeof(void*),2));}}});
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);goto CLEANUP_AND_RETURN;}}_tmp3A=({
struct Cyc_List_List*_tmp51=_cycalloc(sizeof(*_tmp51));_tmp51->hd=({struct
_tuple12*_tmp52=_cycalloc(sizeof(*_tmp52));_tmp52->f1=({struct _dyneither_ptr*
_tmp54=_cycalloc(sizeof(*_tmp54));_tmp54[0]=(struct _dyneither_ptr)Cyc_strdup(
_tag_dyneither(strname,sizeof(char),256));_tmp54;});_tmp52->f2=({struct
_dyneither_ptr*_tmp53=_cycalloc(sizeof(*_tmp53));_tmp53[0]=(struct _dyneither_ptr)
Cyc_strdup(_tag_dyneither(strvalue,sizeof(char),4096));_tmp53;});_tmp52;});
_tmp51->tl=_tmp3A;_tmp51;});if(c == - 1)goto CLEANUP_AND_RETURN;}CLEANUP_AND_RETURN:
Cyc_fclose((struct Cyc___cycFILE*)spec_file);return _tmp3A;}struct _dyneither_ptr
Cyc_split_specs(struct _dyneither_ptr cmdline){if(cmdline.curr == (_tag_dyneither(0,
0,0)).curr)return _tag_dyneither(0,0,0);{unsigned int _tmp55=Cyc_strlen((struct
_dyneither_ptr)cmdline);struct Cyc_List_List*_tmp56=0;char buf[4096];int i=0;int j=0;
if(_tmp55 > 4096)goto DONE;while(1){while(1){if(i >= _tmp55)goto DONE;if(*((const
char*)_check_dyneither_subscript(cmdline,sizeof(char),i))== 0)goto DONE;if(!
isspace((int)*((const char*)_check_dyneither_subscript(cmdline,sizeof(char),i))))
break;++ i;}j=0;while(1){if(i >= _tmp55)break;if(*((const char*)
_check_dyneither_subscript(cmdline,sizeof(char),i))== 0)break;if(isspace((int)*((
const char*)_check_dyneither_subscript(cmdline,sizeof(char),i))))break;if(*((
const char*)_check_dyneither_subscript(cmdline,sizeof(char),i))== '\\'){++ i;if(i
>= _tmp55)break;if(*((const char*)_check_dyneither_subscript(cmdline,sizeof(char),
i))== 0)break;buf[_check_known_subscript_notnull(4096,j)]=*((const char*)
_check_dyneither_subscript(cmdline,sizeof(char),i));++ j;}else{buf[
_check_known_subscript_notnull(4096,j)]=*((const char*)_check_dyneither_subscript(
cmdline,sizeof(char),i));++ j;}++ i;}if(j < 4096)buf[_check_known_subscript_notnull(
4096,j)]='\000';_tmp56=({struct Cyc_List_List*_tmp57=_cycalloc(sizeof(*_tmp57));
_tmp57->hd=({struct _dyneither_ptr*_tmp58=_cycalloc(sizeof(*_tmp58));_tmp58[0]=(
struct _dyneither_ptr)Cyc_strdup(_tag_dyneither(buf,sizeof(char),4096));_tmp58;});
_tmp57->tl=_tmp56;_tmp57;});}DONE: _tmp56=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(_tmp56);_tmp56=({struct Cyc_List_List*_tmp59=_cycalloc(
sizeof(*_tmp59));_tmp59->hd=_init_dyneither_ptr(_cycalloc(sizeof(struct
_dyneither_ptr)),"",sizeof(char),1);_tmp59->tl=_tmp56;_tmp59;});{struct
_dyneither_ptr _tmp5B=((struct _dyneither_ptr(*)(struct Cyc_List_List*x))Cyc_List_to_array)(
_tmp56);struct _dyneither_ptr _tmp5C=({unsigned int _tmp5D=_get_dyneither_size(
_tmp5B,sizeof(struct _dyneither_ptr*));struct _dyneither_ptr*_tmp5E=(struct
_dyneither_ptr*)_cycalloc(_check_times(sizeof(struct _dyneither_ptr),_tmp5D));
struct _dyneither_ptr _tmp60=_tag_dyneither(_tmp5E,sizeof(struct _dyneither_ptr),
_tmp5D);{unsigned int _tmp5F=_tmp5D;unsigned int k;for(k=0;k < _tmp5F;k ++){_tmp5E[k]=*((
struct _dyneither_ptr**)_tmp5B.curr)[(int)k];}}_tmp60;});return _tmp5C;}}}int Cyc_compile_failure=
0;struct Cyc___cycFILE*Cyc_try_file_open(struct _dyneither_ptr filename,struct
_dyneither_ptr mode,struct _dyneither_ptr msg_part){struct _handler_cons _tmp61;
_push_handler(& _tmp61);{int _tmp63=0;if(setjmp(_tmp61.handler))_tmp63=1;if(!
_tmp63){{struct Cyc___cycFILE*_tmp64=(struct Cyc___cycFILE*)Cyc_file_open(filename,
mode);_npop_handler(0);return _tmp64;};_pop_handler();}else{void*_tmp62=(void*)
_exn_thrown;void*_tmp66=_tmp62;_LL1:;_LL2: Cyc_compile_failure=1;({struct Cyc_String_pa_struct
_tmp6A;_tmp6A.tag=0;_tmp6A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
filename);{struct Cyc_String_pa_struct _tmp69;_tmp69.tag=0;_tmp69.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)msg_part);{void*_tmp67[2]={& _tmp69,& _tmp6A};
Cyc_fprintf(Cyc_stderr,({const char*_tmp68="\nError: couldn't open %s %s\n";
_tag_dyneither(_tmp68,sizeof(char),29);}),_tag_dyneither(_tmp67,sizeof(void*),2));}}});
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);return 0;_LL3:;_LL4:(void)_throw(
_tmp66);_LL0:;}}}struct Cyc_List_List*Cyc_do_stage(struct _dyneither_ptr stage_name,
struct Cyc_List_List*(*f)(void*,struct Cyc_List_List*),void*env,struct Cyc_List_List*
tds){struct _dyneither_ptr exn_string=({const char*_tmp88="";_tag_dyneither(_tmp88,
sizeof(char),1);});struct _dyneither_ptr explain_string=({const char*_tmp87="";
_tag_dyneither(_tmp87,sizeof(char),1);});int other_exn=0;void*ex=(void*)({struct
Cyc_Core_Impossible_struct*_tmp84=_cycalloc(sizeof(*_tmp84));_tmp84[0]=({struct
Cyc_Core_Impossible_struct _tmp85;_tmp85.tag=Cyc_Core_Impossible;_tmp85.f1=({
const char*_tmp86="";_tag_dyneither(_tmp86,sizeof(char),1);});_tmp85;});_tmp84;});
struct Cyc_List_List*_tmp6B=0;{struct _handler_cons _tmp6C;_push_handler(& _tmp6C);{
int _tmp6E=0;if(setjmp(_tmp6C.handler))_tmp6E=1;if(!_tmp6E){_tmp6B=f(env,tds);;
_pop_handler();}else{void*_tmp6D=(void*)_exn_thrown;void*_tmp70=_tmp6D;struct
_dyneither_ptr _tmp71;struct _dyneither_ptr _tmp72;struct _dyneither_ptr _tmp73;_LL6:
if(*((void**)_tmp70)!= Cyc_Core_Impossible)goto _LL8;_tmp71=((struct Cyc_Core_Impossible_struct*)
_tmp70)->f1;_LL7: exn_string=({const char*_tmp74="Exception Core::Impossible";
_tag_dyneither(_tmp74,sizeof(char),27);});explain_string=_tmp71;goto _LL5;_LL8:
if(_tmp70 != Cyc_Dict_Absent)goto _LLA;_LL9: exn_string=({const char*_tmp75="Exception Dict::Absent";
_tag_dyneither(_tmp75,sizeof(char),23);});goto _LL5;_LLA: if(*((void**)_tmp70)!= 
Cyc_Core_Invalid_argument)goto _LLC;_tmp72=((struct Cyc_Core_Invalid_argument_struct*)
_tmp70)->f1;_LLB: exn_string=({const char*_tmp76="Exception Core::Invalid_argument";
_tag_dyneither(_tmp76,sizeof(char),33);});explain_string=_tmp72;goto _LL5;_LLC:
if(*((void**)_tmp70)!= Cyc_Core_Failure)goto _LLE;_tmp73=((struct Cyc_Core_Failure_struct*)
_tmp70)->f1;_LLD: exn_string=({const char*_tmp77="Exception Core::Failure";
_tag_dyneither(_tmp77,sizeof(char),24);});explain_string=_tmp73;goto _LL5;_LLE:;
_LLF: ex=_tmp70;other_exn=1;exn_string=({const char*_tmp78="Uncaught exception";
_tag_dyneither(_tmp78,sizeof(char),19);});goto _LL5;_LL10:;_LL11:(void)_throw(
_tmp70);_LL5:;}}}if(Cyc_Position_error_p())Cyc_compile_failure=1;if(Cyc_strcmp((
struct _dyneither_ptr)exn_string,({const char*_tmp79="";_tag_dyneither(_tmp79,
sizeof(char),1);}))!= 0){Cyc_compile_failure=1;({struct Cyc_String_pa_struct
_tmp7E;_tmp7E.tag=0;_tmp7E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
explain_string);{struct Cyc_String_pa_struct _tmp7D;_tmp7D.tag=0;_tmp7D.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)stage_name);{struct Cyc_String_pa_struct
_tmp7C;_tmp7C.tag=0;_tmp7C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
exn_string);{void*_tmp7A[3]={& _tmp7C,& _tmp7D,& _tmp7E};Cyc_fprintf(Cyc_stderr,({
const char*_tmp7B="\n%s thrown during %s: %s";_tag_dyneither(_tmp7B,sizeof(char),
25);}),_tag_dyneither(_tmp7A,sizeof(void*),3));}}}});}if(Cyc_compile_failure){({
void*_tmp7F=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp80="\nCOMPILATION FAILED!\n";
_tag_dyneither(_tmp80,sizeof(char),22);}),_tag_dyneither(_tmp7F,sizeof(void*),0));});
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);if(other_exn)(int)_throw(ex);return
_tmp6B;}else{if(Cyc_v_r){({struct Cyc_String_pa_struct _tmp83;_tmp83.tag=0;_tmp83.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)stage_name);{void*_tmp81[1]={& _tmp83};
Cyc_fprintf(Cyc_stderr,({const char*_tmp82="%s completed.\n";_tag_dyneither(
_tmp82,sizeof(char),15);}),_tag_dyneither(_tmp81,sizeof(void*),1));}});Cyc_fflush((
struct Cyc___cycFILE*)Cyc_stderr);return _tmp6B;}}return _tmp6B;}struct Cyc_List_List*
Cyc_do_parse(struct Cyc___cycFILE*f,struct Cyc_List_List*ignore){Cyc_Lex_lex_init(
1);{struct Cyc_List_List*_tmp89=Cyc_Parse_parse_file(f);Cyc_Lex_lex_init(1);
return _tmp89;}}struct _tuple13{struct _RegionHandle*f1;struct Cyc_Tcenv_Tenv*f2;};
struct Cyc_List_List*Cyc_do_typecheck(struct _tuple13*tcenv,struct Cyc_List_List*
tds){struct _RegionHandle*_tmp8B;struct Cyc_Tcenv_Tenv*_tmp8C;struct _tuple13 _tmp8A=*
tcenv;_tmp8B=_tmp8A.f1;_tmp8C=_tmp8A.f2;Cyc_Tc_tc(_tmp8B,_tmp8C,1,tds);if(!Cyc_noshake_r)
tds=Cyc_Tc_treeshake(_tmp8C,tds);return tds;}struct Cyc_List_List*Cyc_do_cfcheck(
int ignore,struct Cyc_List_List*tds){Cyc_NewControlFlow_cf_check(tds);return tds;}
struct _tuple14{struct Cyc_Tcenv_Tenv*f1;struct Cyc___cycFILE*f2;struct Cyc___cycFILE*
f3;};struct Cyc_List_List*Cyc_do_interface(struct _tuple14*params,struct Cyc_List_List*
tds){struct _tuple14 _tmp8E;struct Cyc_Tcenv_Tenv*_tmp8F;struct Cyc___cycFILE*_tmp90;
struct Cyc___cycFILE*_tmp91;struct _tuple14*_tmp8D=params;_tmp8E=*_tmp8D;_tmp8F=
_tmp8E.f1;_tmp90=_tmp8E.f2;_tmp91=_tmp8E.f3;{struct Cyc_Interface_I*_tmp92=Cyc_Interface_extract(
_tmp8F->ae);if(_tmp90 == 0)Cyc_Interface_save(_tmp92,_tmp91);else{struct Cyc_Interface_I*
_tmp93=Cyc_Interface_parse((struct Cyc___cycFILE*)_tmp90);if(!Cyc_Interface_is_subinterface(
_tmp93,_tmp92,({struct _tuple11*_tmp94=_cycalloc(sizeof(*_tmp94));_tmp94->f1=({
const char*_tmp96="written interface";_tag_dyneither(_tmp96,sizeof(char),18);});
_tmp94->f2=({const char*_tmp95="maximal interface";_tag_dyneither(_tmp95,sizeof(
char),18);});_tmp94;})))Cyc_compile_failure=1;else{Cyc_Interface_save(_tmp93,
_tmp91);}}return tds;}}struct Cyc_List_List*Cyc_do_translate(int ignore,struct Cyc_List_List*
tds){return Cyc_Toc_toc(tds);}struct Cyc_List_List*Cyc_do_tovc(int ignore,struct Cyc_List_List*
tds){Cyc_Tovc_elim_array_initializers=Cyc_Cyclone_tovc_r;return Cyc_Tovc_tovc(tds);}
static struct _dyneither_ptr Cyc_cyc_setjmp=(struct _dyneither_ptr){(void*)0,(void*)
0,(void*)(0 + 0)};static struct _dyneither_ptr Cyc_cyc_include=(struct _dyneither_ptr){(
void*)0,(void*)0,(void*)(0 + 0)};static void Cyc_set_cyc_include(struct
_dyneither_ptr f){Cyc_cyc_include=f;}int Cyc_copy_internal_file(struct
_dyneither_ptr file,struct Cyc___cycFILE*out_file){if(file.curr == (_tag_dyneither(
0,0,0)).curr){({void*_tmp97=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp98="Internal error: copy_internal_file called with NULL\n";
_tag_dyneither(_tmp98,sizeof(char),53);}),_tag_dyneither(_tmp97,sizeof(void*),0));});
return 1;}{struct Cyc___cycFILE*_tmp99=Cyc_try_file_open(file,({const char*_tmpA1="r";
_tag_dyneither(_tmpA1,sizeof(char),2);}),({const char*_tmpA2="internal compiler file";
_tag_dyneither(_tmpA2,sizeof(char),23);}));if(_tmp99 == 0)return 1;{unsigned int
n_read=1024;unsigned int n_written;char buf[1024];{unsigned int _tmpA0=1024;
unsigned int i;for(i=0;i < _tmpA0;i ++){buf[i]='\000';}}while(n_read == 1024){n_read=
Cyc_fread(_tag_dyneither(buf,sizeof(char),1024),1,1024,(struct Cyc___cycFILE*)
_tmp99);if(n_read != 1024  && !Cyc_feof((struct Cyc___cycFILE*)_tmp99)){Cyc_compile_failure=
1;({struct Cyc_String_pa_struct _tmp9C;_tmp9C.tag=0;_tmp9C.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)file);{void*_tmp9A[1]={& _tmp9C};Cyc_fprintf(
Cyc_stderr,({const char*_tmp9B="\nError: problem copying %s\n";_tag_dyneither(
_tmp9B,sizeof(char),28);}),_tag_dyneither(_tmp9A,sizeof(void*),1));}});Cyc_fflush((
struct Cyc___cycFILE*)Cyc_stderr);return 1;}n_written=Cyc_fwrite(_tag_dyneither(
buf,sizeof(char),1024),1,n_read,out_file);if(n_read != n_written){Cyc_compile_failure=
1;({struct Cyc_String_pa_struct _tmp9F;_tmp9F.tag=0;_tmp9F.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)file);{void*_tmp9D[1]={& _tmp9F};Cyc_fprintf(
Cyc_stderr,({const char*_tmp9E="\nError: problem copying %s\n";_tag_dyneither(
_tmp9E,sizeof(char),28);}),_tag_dyneither(_tmp9D,sizeof(void*),1));}});Cyc_fflush((
struct Cyc___cycFILE*)Cyc_stderr);return 1;}}Cyc_fclose((struct Cyc___cycFILE*)
_tmp99);return 0;}}}struct Cyc_List_List*Cyc_do_print(struct Cyc___cycFILE*out_file,
struct Cyc_List_List*tds){struct Cyc_Absynpp_Params _tmpA3=Cyc_tc_r?Cyc_Absynpp_cyc_params_r:
Cyc_Absynpp_c_params_r;_tmpA3.expand_typedefs=!Cyc_noexpand_r;_tmpA3.to_VC=Cyc_Cyclone_tovc_r;
_tmpA3.add_cyc_prefix=Cyc_add_cyc_namespace_r;_tmpA3.generate_line_directives=
Cyc_generate_line_directives_r;_tmpA3.print_full_evars=Cyc_print_full_evars_r;
_tmpA3.print_all_tvars=Cyc_print_all_tvars_r;_tmpA3.print_all_kinds=Cyc_print_all_kinds_r;
_tmpA3.print_all_effects=Cyc_print_all_effects_r;if(Cyc_inline_functions_r)({
void*_tmpA4=0;Cyc_fprintf(out_file,({const char*_tmpA5="#define _INLINE_FUNCTIONS\n";
_tag_dyneither(_tmpA5,sizeof(char),27);}),_tag_dyneither(_tmpA4,sizeof(void*),0));});
if(!Cyc_nocyc_setjmp_r){if(Cyc_copy_internal_file(Cyc_cyc_setjmp,out_file))
return tds;}if(Cyc_copy_internal_file(Cyc_cyc_include,out_file))return tds;if(Cyc_pp_r){
Cyc_Absynpp_set_params(& _tmpA3);Cyc_Absynpp_decllist2file(tds,out_file);}else{
Cyc_Absyndump_set_params(& _tmpA3);Cyc_Absyndump_dumpdecllist2file(tds,out_file);}
Cyc_fflush((struct Cyc___cycFILE*)out_file);return tds;}static struct Cyc_List_List*
Cyc_cfiles=0;static void Cyc_remove_cfiles(){if(!Cyc_save_c_r)for(0;Cyc_cfiles != 0;
Cyc_cfiles=((struct Cyc_List_List*)_check_null(Cyc_cfiles))->tl){Cyc_remove_file(*((
struct _dyneither_ptr*)((struct Cyc_List_List*)_check_null(Cyc_cfiles))->hd));}}
static struct Cyc_List_List*Cyc_split_by_char(struct _dyneither_ptr s,char c){if(s.curr
== (_tag_dyneither(0,0,0)).curr)return 0;{struct Cyc_List_List*_tmpA6=0;
unsigned int _tmpA7=Cyc_strlen((struct _dyneither_ptr)s);while(_tmpA7 > 0){struct
_dyneither_ptr _tmpA8=Cyc_strchr(s,c);if(_tmpA8.curr == (_tag_dyneither(0,0,0)).curr){
_tmpA6=({struct Cyc_List_List*_tmpA9=_cycalloc(sizeof(*_tmpA9));_tmpA9->hd=({
struct _dyneither_ptr*_tmpAA=_cycalloc(sizeof(*_tmpAA));_tmpAA[0]=s;_tmpAA;});
_tmpA9->tl=_tmpA6;_tmpA9;});break;}else{_tmpA6=({struct Cyc_List_List*_tmpAB=
_cycalloc(sizeof(*_tmpAB));_tmpAB->hd=({struct _dyneither_ptr*_tmpAC=_cycalloc(
sizeof(*_tmpAC));_tmpAC[0]=(struct _dyneither_ptr)Cyc_substring((struct
_dyneither_ptr)s,0,(unsigned int)((((struct _dyneither_ptr)_tmpA8).curr - s.curr)/ 
sizeof(char)));_tmpAC;});_tmpAB->tl=_tmpA6;_tmpAB;});_tmpA7 -=(((struct
_dyneither_ptr)_tmpA8).curr - s.curr)/ sizeof(char);s=_dyneither_ptr_plus(_tmpA8,
sizeof(char),1);}}return((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmpA6);}}static int Cyc_file_exists(struct _dyneither_ptr file){struct Cyc___cycFILE*
f=0;{struct _handler_cons _tmpAD;_push_handler(& _tmpAD);{int _tmpAF=0;if(setjmp(
_tmpAD.handler))_tmpAF=1;if(!_tmpAF){f=(struct Cyc___cycFILE*)Cyc_file_open(file,({
const char*_tmpB0="r";_tag_dyneither(_tmpB0,sizeof(char),2);}));;_pop_handler();}
else{void*_tmpAE=(void*)_exn_thrown;void*_tmpB2=_tmpAE;_LL13:;_LL14: goto _LL12;
_LL15:;_LL16:(void)_throw(_tmpB2);_LL12:;}}}if(f == 0)return 0;else{Cyc_fclose((
struct Cyc___cycFILE*)f);return 1;}}static struct _dyneither_ptr Cyc_sprint_list(
struct Cyc_List_List*dirs){struct _dyneither_ptr tmp=({const char*_tmpB5="";
_tag_dyneither(_tmpB5,sizeof(char),1);});for(0;dirs != 0;dirs=dirs->tl){struct
_dyneither_ptr _tmpB3=*((struct _dyneither_ptr*)dirs->hd);if(_tmpB3.curr == (
_tag_dyneither(0,0,0)).curr  || Cyc_strlen((struct _dyneither_ptr)_tmpB3)== 0)
continue;_tmpB3=(struct _dyneither_ptr)Cyc_strconcat((struct _dyneither_ptr)_tmpB3,({
const char*_tmpB4=":";_tag_dyneither(_tmpB4,sizeof(char),2);}));tmp=(struct
_dyneither_ptr)Cyc_strconcat((struct _dyneither_ptr)_tmpB3,(struct _dyneither_ptr)
tmp);}return tmp;}static struct _dyneither_ptr*Cyc_find(struct Cyc_List_List*dirs,
struct _dyneither_ptr file){if(file.curr == (_tag_dyneither(0,0,0)).curr)return 0;
for(0;dirs != 0;dirs=dirs->tl){struct _dyneither_ptr _tmpB6=*((struct _dyneither_ptr*)
dirs->hd);if(_tmpB6.curr == (_tag_dyneither(0,0,0)).curr  || Cyc_strlen((struct
_dyneither_ptr)_tmpB6)== 0)continue;{struct _dyneither_ptr s=(struct _dyneither_ptr)
Cyc_Filename_concat(_tmpB6,file);if(Cyc_file_exists(s))return({struct
_dyneither_ptr*_tmpB7=_cycalloc(sizeof(*_tmpB7));_tmpB7[0]=s;_tmpB7;});}}return 0;}
static struct _dyneither_ptr Cyc_do_find(struct Cyc_List_List*dirs,struct
_dyneither_ptr file){struct _dyneither_ptr*_tmpB8=Cyc_find(dirs,file);if(_tmpB8 == 
0){Cyc_compile_failure=1;Cyc_remove_cfiles();({struct Cyc_String_pa_struct _tmpBC;
_tmpBC.tag=0;_tmpBC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_sprint_list(
dirs));{struct Cyc_String_pa_struct _tmpBB;_tmpBB.tag=0;_tmpBB.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)file);{void*_tmpB9[2]={& _tmpBB,& _tmpBC};
Cyc_fprintf(Cyc_stderr,({const char*_tmpBA="Error: can't find internal compiler file %s in path %s\n";
_tag_dyneither(_tmpBA,sizeof(char),56);}),_tag_dyneither(_tmpB9,sizeof(void*),2));}}});
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);(int)_throw((void*)({struct Cyc_Core_Failure_struct*
_tmpBD=_cycalloc(sizeof(*_tmpBD));_tmpBD[0]=({struct Cyc_Core_Failure_struct
_tmpBE;_tmpBE.tag=Cyc_Core_Failure;_tmpBE.f1=(struct _dyneither_ptr)({struct Cyc_String_pa_struct
_tmpC2;_tmpC2.tag=0;_tmpC2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_sprint_list(
dirs));{struct Cyc_String_pa_struct _tmpC1;_tmpC1.tag=0;_tmpC1.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)file);{void*_tmpBF[2]={& _tmpC1,& _tmpC2};
Cyc_aprintf(({const char*_tmpC0="Error: can't find internal compiler file %s in path %s\n";
_tag_dyneither(_tmpC0,sizeof(char),56);}),_tag_dyneither(_tmpBF,sizeof(void*),2));}}});
_tmpBE;});_tmpBD;}));}return*_tmpB8;}static struct Cyc_List_List*Cyc_add_subdir(
struct Cyc_List_List*dirs,struct _dyneither_ptr subdir){struct Cyc_List_List*_tmpC3=
0;for(0;dirs != 0;dirs=dirs->tl){_tmpC3=({struct Cyc_List_List*_tmpC4=_cycalloc(
sizeof(*_tmpC4));_tmpC4->hd=({struct _dyneither_ptr*_tmpC5=_cycalloc(sizeof(*
_tmpC5));_tmpC5[0]=(struct _dyneither_ptr)Cyc_Filename_concat(*((struct
_dyneither_ptr*)dirs->hd),subdir);_tmpC5;});_tmpC4->tl=_tmpC3;_tmpC4;});}_tmpC3=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpC3);return
_tmpC3;}static struct Cyc_List_List*Cyc_also_subdir(struct Cyc_List_List*dirs,
struct _dyneither_ptr subdir){struct Cyc_List_List*_tmpC6=0;for(0;dirs != 0;dirs=
dirs->tl){_tmpC6=({struct Cyc_List_List*_tmpC7=_cycalloc(sizeof(*_tmpC7));_tmpC7->hd=(
struct _dyneither_ptr*)dirs->hd;_tmpC7->tl=_tmpC6;_tmpC7;});_tmpC6=({struct Cyc_List_List*
_tmpC8=_cycalloc(sizeof(*_tmpC8));_tmpC8->hd=({struct _dyneither_ptr*_tmpC9=
_cycalloc(sizeof(*_tmpC9));_tmpC9[0]=(struct _dyneither_ptr)Cyc_Filename_concat(*((
struct _dyneither_ptr*)dirs->hd),subdir);_tmpC9;});_tmpC8->tl=_tmpC6;_tmpC8;});}
_tmpC6=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpC6);
return _tmpC6;}static int Cyc_is_other_special(char c){switch(c){case '\\': _LL17: goto
_LL18;case '"': _LL18: goto _LL19;case ';': _LL19: goto _LL1A;case '&': _LL1A: goto _LL1B;
case '(': _LL1B: goto _LL1C;case ')': _LL1C: goto _LL1D;case '|': _LL1D: goto _LL1E;case '^':
_LL1E: goto _LL1F;case '<': _LL1F: goto _LL20;case '>': _LL20: goto _LL21;case ' ': _LL21:
goto _LL22;case '\n': _LL22: goto _LL23;case '\t': _LL23: return 1;default: _LL24: return 0;}}
static struct _dyneither_ptr Cyc_sh_escape_string(struct _dyneither_ptr s){
unsigned int _tmpCA=Cyc_strlen((struct _dyneither_ptr)s);int _tmpCB=0;int _tmpCC=0;{
int i=0;for(0;i < _tmpCA;++ i){char _tmpCD=*((const char*)_check_dyneither_subscript(
s,sizeof(char),i));if(_tmpCD == '\'')++ _tmpCB;else{if(Cyc_is_other_special(_tmpCD))
++ _tmpCC;}}}if(_tmpCB == 0  && _tmpCC == 0)return s;if(_tmpCB == 0)return(struct
_dyneither_ptr)Cyc_strconcat_l(({struct _dyneither_ptr*_tmpCE[3];_tmpCE[2]=
_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"'",sizeof(char),2);
_tmpCE[1]=({struct _dyneither_ptr*_tmpD0=_cycalloc(sizeof(*_tmpD0));_tmpD0[0]=(
struct _dyneither_ptr)s;_tmpD0;});_tmpCE[0]=_init_dyneither_ptr(_cycalloc(sizeof(
struct _dyneither_ptr)),"'",sizeof(char),2);((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCE,sizeof(struct _dyneither_ptr*),
3));}));{unsigned int _tmpD2=(_tmpCA + _tmpCB)+ _tmpCC;struct _dyneither_ptr s2=({
unsigned int _tmpDC=_tmpD2 + 1;char*_tmpDD=(char*)_cycalloc_atomic(_check_times(
sizeof(char),_tmpDC + 1));struct _dyneither_ptr _tmpDF=_tag_dyneither(_tmpDD,
sizeof(char),_tmpDC + 1);{unsigned int _tmpDE=_tmpDC;unsigned int i;for(i=0;i < 
_tmpDE;i ++){_tmpDD[i]='\000';}_tmpDD[_tmpDE]=(char)0;}_tmpDF;});int _tmpD3=0;int
_tmpD4=0;for(0;_tmpD3 < _tmpCA;++ _tmpD3){char _tmpD5=*((const char*)
_check_dyneither_subscript(s,sizeof(char),_tmpD3));if(_tmpD5 == '\''  || Cyc_is_other_special(
_tmpD5))({struct _dyneither_ptr _tmpD6=_dyneither_ptr_plus(s2,sizeof(char),_tmpD4
++);char _tmpD7=*((char*)_check_dyneither_subscript(_tmpD6,sizeof(char),0));char
_tmpD8='\\';if(_get_dyneither_size(_tmpD6,sizeof(char))== 1  && (_tmpD7 == '\000'
 && _tmpD8 != '\000'))_throw_arraybounds();*((char*)_tmpD6.curr)=_tmpD8;});({
struct _dyneither_ptr _tmpD9=_dyneither_ptr_plus(s2,sizeof(char),_tmpD4 ++);char
_tmpDA=*((char*)_check_dyneither_subscript(_tmpD9,sizeof(char),0));char _tmpDB=
_tmpD5;if(_get_dyneither_size(_tmpD9,sizeof(char))== 1  && (_tmpDA == '\000'  && 
_tmpDB != '\000'))_throw_arraybounds();*((char*)_tmpD9.curr)=_tmpDB;});}return(
struct _dyneither_ptr)s2;}}static struct _dyneither_ptr*Cyc_sh_escape_stringptr(
struct _dyneither_ptr*sp){return({struct _dyneither_ptr*_tmpE0=_cycalloc(sizeof(*
_tmpE0));_tmpE0[0]=Cyc_sh_escape_string(*sp);_tmpE0;});}static void Cyc_process_file(
struct _dyneither_ptr filename){struct _dyneither_ptr _tmpE1=Cyc_make_base_filename(
filename,Cyc_output_file);struct _dyneither_ptr _tmpE2=Cyc_strconcat((struct
_dyneither_ptr)_tmpE1,({const char*_tmp13C=".cyp";_tag_dyneither(_tmp13C,sizeof(
char),5);}));struct _dyneither_ptr _tmpE3=Cyc_strconcat((struct _dyneither_ptr)
_tmpE1,({const char*_tmp13B=".cyci";_tag_dyneither(_tmp13B,sizeof(char),6);}));
struct _dyneither_ptr _tmpE4=Cyc_strconcat((struct _dyneither_ptr)_tmpE1,({const
char*_tmp13A=".cycio";_tag_dyneither(_tmp13A,sizeof(char),7);}));struct
_dyneither_ptr _tmpE5=Cyc_strconcat((struct _dyneither_ptr)_tmpE1,({const char*
_tmp139=".c";_tag_dyneither(_tmp139,sizeof(char),3);}));if(Cyc_v_r)({struct Cyc_String_pa_struct
_tmpE8;_tmpE8.tag=0;_tmpE8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
filename);{void*_tmpE6[1]={& _tmpE8};Cyc_fprintf(Cyc_stderr,({const char*_tmpE7="Compiling %s\n";
_tag_dyneither(_tmpE7,sizeof(char),14);}),_tag_dyneither(_tmpE6,sizeof(void*),1));}});{
struct Cyc___cycFILE*f0=Cyc_try_file_open(filename,({const char*_tmp137="r";
_tag_dyneither(_tmp137,sizeof(char),2);}),({const char*_tmp138="input file";
_tag_dyneither(_tmp138,sizeof(char),11);}));if(Cyc_compile_failure  || !((
unsigned int)f0))return;Cyc_fclose((struct Cyc___cycFILE*)f0);{struct
_dyneither_ptr _tmpE9=Cyc_str_sepstr(({struct Cyc_List_List*_tmp133=_cycalloc(
sizeof(*_tmp133));_tmp133->hd=({struct _dyneither_ptr*_tmp134=_cycalloc(sizeof(*
_tmp134));_tmp134[0]=(struct _dyneither_ptr)({const char*_tmp135="";_tag_dyneither(
_tmp135,sizeof(char),1);});_tmp134;});_tmp133->tl=((struct Cyc_List_List*(*)(
struct _dyneither_ptr*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_sh_escape_stringptr,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(
Cyc_cppargs));_tmp133;}),({const char*_tmp136=" ";_tag_dyneither(_tmp136,sizeof(
char),2);}));struct _dyneither_ptr def_inc_path=(struct _dyneither_ptr)
Cstring_to_string(Cdef_inc_path);struct Cyc_List_List*_tmpEA=Cyc_add_subdir(Cyc_cyclone_exec_path,
Cyc_target_arch);_tmpEA=Cyc_add_subdir(_tmpEA,({const char*_tmpEB="include";
_tag_dyneither(_tmpEB,sizeof(char),8);}));if(Cyc_strlen((struct _dyneither_ptr)
def_inc_path)> 0)_tmpEA=({struct Cyc_List_List*_tmpEC=_cycalloc(sizeof(*_tmpEC));
_tmpEC->hd=({struct _dyneither_ptr*_tmpED=_cycalloc(sizeof(*_tmpED));_tmpED[0]=
def_inc_path;_tmpED;});_tmpEC->tl=_tmpEA;_tmpEC;});{char*_tmpEE=getenv((const
char*)"CYCLONE_INCLUDE_PATH");if((char*)_tmpEE != 0)_tmpEA=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(Cyc_split_by_char(({
char*_tmpEF=_tmpEE;_tag_dyneither(_tmpEF,sizeof(char),_get_zero_arr_size_char(
_tmpEF,1));}),':'),_tmpEA);{struct _dyneither_ptr stdinc_string=(struct
_dyneither_ptr)Cyc_str_sepstr(({struct Cyc_List_List*_tmp12F=_cycalloc(sizeof(*
_tmp12F));_tmp12F->hd=({struct _dyneither_ptr*_tmp130=_cycalloc(sizeof(*_tmp130));
_tmp130[0]=(struct _dyneither_ptr)({const char*_tmp131="";_tag_dyneither(_tmp131,
sizeof(char),1);});_tmp130;});_tmp12F->tl=((struct Cyc_List_List*(*)(struct
_dyneither_ptr*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_sh_escape_stringptr,_tmpEA);_tmp12F;}),({const char*_tmp132=" -I";
_tag_dyneither(_tmp132,sizeof(char),4);}));struct _dyneither_ptr ofile_string;if(
Cyc_stop_after_cpp_r){if(Cyc_output_file != 0)ofile_string=(struct _dyneither_ptr)({
struct Cyc_String_pa_struct _tmpF2;_tmpF2.tag=0;_tmpF2.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)*((struct _dyneither_ptr*)_check_null(Cyc_output_file)));{
void*_tmpF0[1]={& _tmpF2};Cyc_aprintf(({const char*_tmpF1=" > %s";_tag_dyneither(
_tmpF1,sizeof(char),6);}),_tag_dyneither(_tmpF0,sizeof(void*),1));}});else{
ofile_string=({const char*_tmpF3="";_tag_dyneither(_tmpF3,sizeof(char),1);});}}
else{ofile_string=(struct _dyneither_ptr)({struct Cyc_String_pa_struct _tmpF6;
_tmpF6.tag=0;_tmpF6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_sh_escape_string((
struct _dyneither_ptr)_tmpE2));{void*_tmpF4[1]={& _tmpF6};Cyc_aprintf(({const char*
_tmpF5=" > %s";_tag_dyneither(_tmpF5,sizeof(char),6);}),_tag_dyneither(_tmpF4,
sizeof(void*),1));}});}{struct _dyneither_ptr fixup_string;if(Cyc_produce_dependencies){
if(Cyc_dependencies_target == 0)fixup_string=({const char*_tmpF7=" | sed 's/^\\(.*\\)\\.cyc\\.o:/\\1.o:/'";
_tag_dyneither(_tmpF7,sizeof(char),35);});else{fixup_string=(struct
_dyneither_ptr)({struct Cyc_String_pa_struct _tmpFA;_tmpFA.tag=0;_tmpFA.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)_check_null(Cyc_dependencies_target)));{
void*_tmpF8[1]={& _tmpFA};Cyc_aprintf(({const char*_tmpF9=" | sed 's/^.*\\.cyc\\.o:/%s:/'";
_tag_dyneither(_tmpF9,sizeof(char),29);}),_tag_dyneither(_tmpF8,sizeof(void*),1));}});}}
else{fixup_string=({const char*_tmpFB="";_tag_dyneither(_tmpFB,sizeof(char),1);});}{
struct _dyneither_ptr _tmpFC=({struct Cyc_String_pa_struct _tmp12E;_tmp12E.tag=0;
_tmp12E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)ofile_string);{struct Cyc_String_pa_struct
_tmp12D;_tmp12D.tag=0;_tmp12D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
fixup_string);{struct Cyc_String_pa_struct _tmp12C;_tmp12C.tag=0;_tmp12C.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_sh_escape_string(filename));{
struct Cyc_String_pa_struct _tmp12B;_tmp12B.tag=0;_tmp12B.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)stdinc_string);{struct Cyc_String_pa_struct _tmp12A;_tmp12A.tag=
0;_tmp12A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmpE9);{struct Cyc_String_pa_struct
_tmp129;_tmp129.tag=0;_tmp129.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_cpp);{void*_tmp127[6]={& _tmp129,& _tmp12A,& _tmp12B,& _tmp12C,& _tmp12D,& _tmp12E};
Cyc_aprintf(({const char*_tmp128="%s %s%s %s%s%s";_tag_dyneither(_tmp128,sizeof(
char),15);}),_tag_dyneither(_tmp127,sizeof(void*),6));}}}}}}});if(Cyc_v_r)({
struct Cyc_String_pa_struct _tmpFF;_tmpFF.tag=0;_tmpFF.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)_tmpFC);{void*_tmpFD[1]={& _tmpFF};Cyc_fprintf(Cyc_stderr,({
const char*_tmpFE="%s\n";_tag_dyneither(_tmpFE,sizeof(char),4);}),_tag_dyneither(
_tmpFD,sizeof(void*),1));}});if(system((const char*)_untag_dyneither_ptr(_tmpFC,
sizeof(char),1))!= 0){Cyc_compile_failure=1;({void*_tmp100=0;Cyc_fprintf(Cyc_stderr,({
const char*_tmp101="\nError: preprocessing\n";_tag_dyneither(_tmp101,sizeof(char),
23);}),_tag_dyneither(_tmp100,sizeof(void*),0));});return;}if(Cyc_stop_after_cpp_r)
return;Cyc_Position_reset_position((struct _dyneither_ptr)_tmpE2);{struct Cyc___cycFILE*
in_file=Cyc_try_file_open((struct _dyneither_ptr)_tmpE2,({const char*_tmp125="r";
_tag_dyneither(_tmp125,sizeof(char),2);}),({const char*_tmp126="file";
_tag_dyneither(_tmp126,sizeof(char),5);}));if(Cyc_compile_failure)return;{struct
Cyc_List_List*tds=0;{struct _handler_cons _tmp102;_push_handler(& _tmp102);{int
_tmp104=0;if(setjmp(_tmp102.handler))_tmp104=1;if(!_tmp104){tds=((struct Cyc_List_List*(*)(
struct _dyneither_ptr stage_name,struct Cyc_List_List*(*f)(struct Cyc___cycFILE*,
struct Cyc_List_List*),struct Cyc___cycFILE*env,struct Cyc_List_List*tds))Cyc_do_stage)(({
const char*_tmp105="parsing";_tag_dyneither(_tmp105,sizeof(char),8);}),Cyc_do_parse,(
struct Cyc___cycFILE*)_check_null(in_file),tds);;_pop_handler();}else{void*
_tmp103=(void*)_exn_thrown;void*_tmp107=_tmp103;_LL27:;_LL28: Cyc_file_close((
struct Cyc___cycFILE*)_check_null(in_file));Cyc_remove_file((struct _dyneither_ptr)
_tmpE2);(int)_throw(_tmp107);_LL29:;_LL2A:(void)_throw(_tmp107);_LL26:;}}}Cyc_file_close((
struct Cyc___cycFILE*)in_file);if(Cyc_compile_failure){Cyc_remove_file((struct
_dyneither_ptr)_tmpE2);return;}if(Cyc_Absyn_porting_c_code){Cyc_Port_port(tds);
return;}{struct _RegionHandle _tmp108=_new_region("tc_rgn");struct _RegionHandle*
tc_rgn=& _tmp108;_push_region(tc_rgn);{struct Cyc_Tcenv_Tenv*_tmp109=Cyc_Tcenv_tc_init(
tc_rgn);if(Cyc_parseonly_r)goto PRINTC;{struct _tuple13 _tmp10A=({struct _tuple13
_tmp112;_tmp112.f1=tc_rgn;_tmp112.f2=_tmp109;_tmp112;});tds=((struct Cyc_List_List*(*)(
struct _dyneither_ptr stage_name,struct Cyc_List_List*(*f)(struct _tuple13*,struct
Cyc_List_List*),struct _tuple13*env,struct Cyc_List_List*tds))Cyc_do_stage)(({
const char*_tmp10B="type checking";_tag_dyneither(_tmp10B,sizeof(char),14);}),Cyc_do_typecheck,&
_tmp10A,tds);if(Cyc_compile_failure){Cyc_remove_file((struct _dyneither_ptr)
_tmpE2);_npop_handler(0);return;}if(Cyc_tc_r)goto PRINTC;tds=((struct Cyc_List_List*(*)(
struct _dyneither_ptr stage_name,struct Cyc_List_List*(*f)(int,struct Cyc_List_List*),
int env,struct Cyc_List_List*tds))Cyc_do_stage)(({const char*_tmp10C="control-flow checking";
_tag_dyneither(_tmp10C,sizeof(char),22);}),Cyc_do_cfcheck,1,tds);if(Cyc_compile_failure){
Cyc_remove_file((struct _dyneither_ptr)_tmpE2);_npop_handler(0);return;}if(Cyc_ic_r){
struct Cyc___cycFILE*inter_file=Cyc_fopen((const char*)((char*)
_untag_dyneither_ptr(_tmpE3,sizeof(char),1)),(const char*)"r");struct Cyc___cycFILE*
inter_objfile=Cyc_try_file_open((struct _dyneither_ptr)_tmpE4,({const char*_tmp110="w";
_tag_dyneither(_tmp110,sizeof(char),2);}),({const char*_tmp111="interface object file";
_tag_dyneither(_tmp111,sizeof(char),22);}));if(inter_objfile == 0){Cyc_compile_failure=
1;_npop_handler(0);return;}Cyc_Position_reset_position((struct _dyneither_ptr)
_tmpE3);{struct _tuple14 _tmp10D=({struct _tuple14 _tmp10F;_tmp10F.f1=_tmp109;
_tmp10F.f2=inter_file;_tmp10F.f3=(struct Cyc___cycFILE*)inter_objfile;_tmp10F;});
tds=((struct Cyc_List_List*(*)(struct _dyneither_ptr stage_name,struct Cyc_List_List*(*
f)(struct _tuple14*,struct Cyc_List_List*),struct _tuple14*env,struct Cyc_List_List*
tds))Cyc_do_stage)(({const char*_tmp10E="interface checking";_tag_dyneither(
_tmp10E,sizeof(char),19);}),Cyc_do_interface,& _tmp10D,tds);if(inter_file != 0)Cyc_file_close((
struct Cyc___cycFILE*)inter_file);Cyc_file_close((struct Cyc___cycFILE*)
inter_objfile);}}}}if(Cyc_cf_r)goto PRINTC;tds=((struct Cyc_List_List*(*)(struct
_dyneither_ptr stage_name,struct Cyc_List_List*(*f)(int,struct Cyc_List_List*),int
env,struct Cyc_List_List*tds))Cyc_do_stage)(({const char*_tmp113="translation to C";
_tag_dyneither(_tmp113,sizeof(char),17);}),Cyc_do_translate,1,tds);if(Cyc_compile_failure){
Cyc_remove_file((struct _dyneither_ptr)_tmpE2);_npop_handler(0);return;}if(!Cyc_compile_failure)
Cyc_Tcutil_flush_warnings();Cyc_remove_file((struct _dyneither_ptr)_tmpE2);if(Cyc_compile_failure){
_npop_handler(0);return;}if(Cyc_Cyclone_tovc_r  || Cyc_elim_se_r)tds=((struct Cyc_List_List*(*)(
struct _dyneither_ptr stage_name,struct Cyc_List_List*(*f)(int,struct Cyc_List_List*),
int env,struct Cyc_List_List*tds))Cyc_do_stage)(({const char*_tmp114="post-pass to VC";
_tag_dyneither(_tmp114,sizeof(char),16);}),Cyc_do_tovc,1,tds);if(Cyc_compile_failure){
_npop_handler(0);return;}PRINTC: {struct Cyc___cycFILE*out_file;if((Cyc_parseonly_r
 || Cyc_tc_r) || Cyc_cf_r){if(Cyc_output_file != 0)out_file=Cyc_try_file_open(*((
struct _dyneither_ptr*)_check_null(Cyc_output_file)),({const char*_tmp115="w";
_tag_dyneither(_tmp115,sizeof(char),2);}),({const char*_tmp116="output file";
_tag_dyneither(_tmp116,sizeof(char),12);}));else{out_file=(struct Cyc___cycFILE*)
Cyc_stdout;}}else{if(Cyc_toc_r  && Cyc_output_file != 0)out_file=Cyc_try_file_open(*((
struct _dyneither_ptr*)_check_null(Cyc_output_file)),({const char*_tmp117="w";
_tag_dyneither(_tmp117,sizeof(char),2);}),({const char*_tmp118="output file";
_tag_dyneither(_tmp118,sizeof(char),12);}));else{out_file=Cyc_try_file_open((
struct _dyneither_ptr)_tmpE5,({const char*_tmp119="w";_tag_dyneither(_tmp119,
sizeof(char),2);}),({const char*_tmp11A="output file";_tag_dyneither(_tmp11A,
sizeof(char),12);}));}}if(Cyc_compile_failure  || !((unsigned int)out_file)){
_npop_handler(0);return;}{struct _handler_cons _tmp11B;_push_handler(& _tmp11B);{
int _tmp11D=0;if(setjmp(_tmp11B.handler))_tmp11D=1;if(!_tmp11D){if(!Cyc_noprint_r)
tds=((struct Cyc_List_List*(*)(struct _dyneither_ptr stage_name,struct Cyc_List_List*(*
f)(struct Cyc___cycFILE*,struct Cyc_List_List*),struct Cyc___cycFILE*env,struct Cyc_List_List*
tds))Cyc_do_stage)(({const char*_tmp11E="printing";_tag_dyneither(_tmp11E,sizeof(
char),9);}),Cyc_do_print,(struct Cyc___cycFILE*)out_file,tds);;_pop_handler();}
else{void*_tmp11C=(void*)_exn_thrown;void*_tmp120=_tmp11C;_LL2C:;_LL2D: Cyc_compile_failure=
1;Cyc_file_close((struct Cyc___cycFILE*)out_file);Cyc_cfiles=({struct Cyc_List_List*
_tmp121=_cycalloc(sizeof(*_tmp121));_tmp121->hd=({struct _dyneither_ptr*_tmp122=
_cycalloc(sizeof(*_tmp122));_tmp122[0]=(struct _dyneither_ptr)_tmpE5;_tmp122;});
_tmp121->tl=Cyc_cfiles;_tmp121;});(int)_throw(_tmp120);_LL2E:;_LL2F:(void)_throw(
_tmp120);_LL2B:;}}}Cyc_file_close((struct Cyc___cycFILE*)out_file);Cyc_cfiles=({
struct Cyc_List_List*_tmp123=_cycalloc(sizeof(*_tmp123));_tmp123->hd=({struct
_dyneither_ptr*_tmp124=_cycalloc(sizeof(*_tmp124));_tmp124[0]=(struct
_dyneither_ptr)_tmpE5;_tmp124;});_tmp123->tl=Cyc_cfiles;_tmp123;});};_pop_region(
tc_rgn);}}}}}}}}}}static char _tmp13D[8]="<final>";static struct _dyneither_ptr Cyc_final_str={
_tmp13D,_tmp13D,_tmp13D + 8};static struct _dyneither_ptr*Cyc_final_strptr=& Cyc_final_str;
static struct Cyc_Interface_I*Cyc_read_cycio(struct _dyneither_ptr*n){if(n == (
struct _dyneither_ptr*)Cyc_final_strptr)return Cyc_Interface_final();{struct
_dyneither_ptr basename;{struct _handler_cons _tmp13E;_push_handler(& _tmp13E);{int
_tmp140=0;if(setjmp(_tmp13E.handler))_tmp140=1;if(!_tmp140){basename=(struct
_dyneither_ptr)Cyc_Filename_chop_extension(*n);;_pop_handler();}else{void*
_tmp13F=(void*)_exn_thrown;void*_tmp142=_tmp13F;_LL31: if(*((void**)_tmp142)!= 
Cyc_Core_Invalid_argument)goto _LL33;_LL32: basename=*n;goto _LL30;_LL33:;_LL34:(
void)_throw(_tmp142);_LL30:;}}}{struct _dyneither_ptr _tmp143=Cyc_strconcat((
struct _dyneither_ptr)basename,({const char*_tmp148=".cycio";_tag_dyneither(
_tmp148,sizeof(char),7);}));struct Cyc___cycFILE*_tmp144=Cyc_try_file_open((
struct _dyneither_ptr)_tmp143,({const char*_tmp146="rb";_tag_dyneither(_tmp146,
sizeof(char),3);}),({const char*_tmp147="interface object file";_tag_dyneither(
_tmp147,sizeof(char),22);}));if(_tmp144 == 0){Cyc_compile_failure=1;Cyc_remove_cfiles();
exit(1);}Cyc_Position_reset_position((struct _dyneither_ptr)_tmp143);{struct Cyc_Interface_I*
_tmp145=Cyc_Interface_load((struct Cyc___cycFILE*)_tmp144);Cyc_file_close((struct
Cyc___cycFILE*)_tmp144);return _tmp145;}}}}static int Cyc_is_cfile(struct
_dyneither_ptr*n){return*((const char*)_check_dyneither_subscript(*n,sizeof(char),
0))!= '-';}void GC_blacklist_warn_clear();struct _tuple15{struct _dyneither_ptr f1;
int f2;struct _dyneither_ptr f3;void*f4;struct _dyneither_ptr f5;};int Cyc_main(int
argc,struct _dyneither_ptr argv){GC_blacklist_warn_clear();{struct Cyc_List_List*
cyclone_arch_path;struct _dyneither_ptr def_lib_path;struct _dyneither_ptr comp=(
struct _dyneither_ptr)Cstring_to_string(Ccomp);struct _RegionHandle _tmp149=
_new_region("r");struct _RegionHandle*r=& _tmp149;_push_region(r);{struct
_dyneither_ptr _tmp14A=({unsigned int _tmp2F3=(unsigned int)argc;int*_tmp2F4=(int*)
_cycalloc_atomic(_check_times(sizeof(int),_tmp2F3));struct _dyneither_ptr _tmp2F6=
_tag_dyneither(_tmp2F4,sizeof(int),_tmp2F3);{unsigned int _tmp2F5=_tmp2F3;
unsigned int i;for(i=0;i < _tmp2F5;i ++){_tmp2F4[i]=0;}}_tmp2F6;});int _tmp14B=0;int
i;int j;int k;for(i=1;i < argc;++ i){if(Cyc_strncmp(({const char*_tmp14C="-B";
_tag_dyneither(_tmp14C,sizeof(char),3);}),(struct _dyneither_ptr)*((struct
_dyneither_ptr*)_check_dyneither_subscript(argv,sizeof(struct _dyneither_ptr),i)),
2)== 0){*((int*)_check_dyneither_subscript(_tmp14A,sizeof(int),i))=1;++ _tmp14B;}
else{if(Cyc_strcmp(({const char*_tmp14D="-b";_tag_dyneither(_tmp14D,sizeof(char),
3);}),(struct _dyneither_ptr)*((struct _dyneither_ptr*)_check_dyneither_subscript(
argv,sizeof(struct _dyneither_ptr),i)))== 0){*((int*)_check_dyneither_subscript(
_tmp14A,sizeof(int),i))=1;++ _tmp14B;++ i;if(i >= argc)break;*((int*)
_check_dyneither_subscript(_tmp14A,sizeof(int),i))=1;++ _tmp14B;}}}{struct
_dyneither_ptr _tmp14E=({unsigned int _tmp2EF=(unsigned int)(_tmp14B + 1);struct
_dyneither_ptr*_tmp2F0=(struct _dyneither_ptr*)_cycalloc(_check_times(sizeof(
struct _dyneither_ptr),_tmp2EF));struct _dyneither_ptr _tmp2F2=_tag_dyneither(
_tmp2F0,sizeof(struct _dyneither_ptr),_tmp2EF);{unsigned int _tmp2F1=_tmp2EF;
unsigned int n;for(n=0;n < _tmp2F1;n ++){_tmp2F0[n]=(struct _dyneither_ptr)
_tag_dyneither(0,0,0);}}_tmp2F2;});struct _dyneither_ptr _tmp14F=({unsigned int
_tmp2EB=(unsigned int)(argc - _tmp14B);struct _dyneither_ptr*_tmp2EC=(struct
_dyneither_ptr*)_cycalloc(_check_times(sizeof(struct _dyneither_ptr),_tmp2EB));
struct _dyneither_ptr _tmp2EE=_tag_dyneither(_tmp2EC,sizeof(struct _dyneither_ptr),
_tmp2EB);{unsigned int _tmp2ED=_tmp2EB;unsigned int n;for(n=0;n < _tmp2ED;n ++){
_tmp2EC[n]=(struct _dyneither_ptr)_tag_dyneither(0,0,0);}}_tmp2EE;});*((struct
_dyneither_ptr*)_check_dyneither_subscript(_tmp14E,sizeof(struct _dyneither_ptr),
0))=(*((struct _dyneither_ptr*)_check_dyneither_subscript(_tmp14F,sizeof(struct
_dyneither_ptr),0))=*((struct _dyneither_ptr*)_check_dyneither_subscript(argv,
sizeof(struct _dyneither_ptr),0)));for(i=(j=(k=1));i < argc;++ i){if(*((int*)
_check_dyneither_subscript(_tmp14A,sizeof(int),i)))*((struct _dyneither_ptr*)
_check_dyneither_subscript(_tmp14E,sizeof(struct _dyneither_ptr),j ++))=*((struct
_dyneither_ptr*)_check_dyneither_subscript(argv,sizeof(struct _dyneither_ptr),i));
else{*((struct _dyneither_ptr*)_check_dyneither_subscript(_tmp14F,sizeof(struct
_dyneither_ptr),k ++))=*((struct _dyneither_ptr*)_check_dyneither_subscript(argv,
sizeof(struct _dyneither_ptr),i));}}{struct Cyc_List_List*options=({struct _tuple15*
_tmp16A[64];_tmp16A[63]=({struct _tuple15*_tmp2E5=_region_malloc(r,sizeof(*
_tmp2E5));_tmp2E5->f1=({const char*_tmp2EA="-noregions";_tag_dyneither(_tmp2EA,
sizeof(char),11);});_tmp2E5->f2=0;_tmp2E5->f3=({const char*_tmp2E9="";
_tag_dyneither(_tmp2E9,sizeof(char),1);});_tmp2E5->f4=(void*)({struct Cyc_Arg_Set_spec_struct*
_tmp2E7=_region_malloc(r,sizeof(*_tmp2E7));_tmp2E7[0]=({struct Cyc_Arg_Set_spec_struct
_tmp2E8;_tmp2E8.tag=3;_tmp2E8.f1=& Cyc_Absyn_no_regions;_tmp2E8;});_tmp2E7;});
_tmp2E5->f5=({const char*_tmp2E6="Generate code that doesn't use regions";
_tag_dyneither(_tmp2E6,sizeof(char),39);});_tmp2E5;});_tmp16A[62]=({struct
_tuple15*_tmp2DF=_region_malloc(r,sizeof(*_tmp2DF));_tmp2DF->f1=({const char*
_tmp2E4="-port";_tag_dyneither(_tmp2E4,sizeof(char),6);});_tmp2DF->f2=0;_tmp2DF->f3=({
const char*_tmp2E3="";_tag_dyneither(_tmp2E3,sizeof(char),1);});_tmp2DF->f4=(void*)({
struct Cyc_Arg_Unit_spec_struct*_tmp2E1=_region_malloc(r,sizeof(*_tmp2E1));
_tmp2E1[0]=({struct Cyc_Arg_Unit_spec_struct _tmp2E2;_tmp2E2.tag=0;_tmp2E2.f1=Cyc_set_port_c_code;
_tmp2E2;});_tmp2E1;});_tmp2DF->f5=({const char*_tmp2E0="Suggest how to port C code to Cyclone";
_tag_dyneither(_tmp2E0,sizeof(char),38);});_tmp2DF;});_tmp16A[61]=({struct
_tuple15*_tmp2D9=_region_malloc(r,sizeof(*_tmp2D9));_tmp2D9->f1=({const char*
_tmp2DE="-detailedlocation";_tag_dyneither(_tmp2DE,sizeof(char),18);});_tmp2D9->f2=
0;_tmp2D9->f3=({const char*_tmp2DD="";_tag_dyneither(_tmp2DD,sizeof(char),1);});
_tmp2D9->f4=(void*)({struct Cyc_Arg_Clear_spec_struct*_tmp2DB=_region_malloc(r,
sizeof(*_tmp2DB));_tmp2DB[0]=({struct Cyc_Arg_Clear_spec_struct _tmp2DC;_tmp2DC.tag=
4;_tmp2DC.f1=& Cyc_Position_use_gcc_style_location;_tmp2DC;});_tmp2DB;});_tmp2D9->f5=({
const char*_tmp2DA="Try to give more detailed location information for errors";
_tag_dyneither(_tmp2DA,sizeof(char),58);});_tmp2D9;});_tmp16A[60]=({struct
_tuple15*_tmp2D3=_region_malloc(r,sizeof(*_tmp2D3));_tmp2D3->f1=({const char*
_tmp2D8="-noregister";_tag_dyneither(_tmp2D8,sizeof(char),12);});_tmp2D3->f2=0;
_tmp2D3->f3=({const char*_tmp2D7="";_tag_dyneither(_tmp2D7,sizeof(char),1);});
_tmp2D3->f4=(void*)({struct Cyc_Arg_Set_spec_struct*_tmp2D5=_region_malloc(r,
sizeof(*_tmp2D5));_tmp2D5[0]=({struct Cyc_Arg_Set_spec_struct _tmp2D6;_tmp2D6.tag=
3;_tmp2D6.f1=& Cyc_Parse_no_register;_tmp2D6;});_tmp2D5;});_tmp2D3->f5=({const
char*_tmp2D4="Treat register storage class as public";_tag_dyneither(_tmp2D4,
sizeof(char),39);});_tmp2D3;});_tmp16A[59]=({struct _tuple15*_tmp2CD=
_region_malloc(r,sizeof(*_tmp2CD));_tmp2CD->f1=({const char*_tmp2D2="-warnnullchecks";
_tag_dyneither(_tmp2D2,sizeof(char),16);});_tmp2CD->f2=0;_tmp2CD->f3=({const char*
_tmp2D1="";_tag_dyneither(_tmp2D1,sizeof(char),1);});_tmp2CD->f4=(void*)({struct
Cyc_Arg_Set_spec_struct*_tmp2CF=_region_malloc(r,sizeof(*_tmp2CF));_tmp2CF[0]=({
struct Cyc_Arg_Set_spec_struct _tmp2D0;_tmp2D0.tag=3;_tmp2D0.f1=& Cyc_Toc_warn_all_null_deref;
_tmp2D0;});_tmp2CF;});_tmp2CD->f5=({const char*_tmp2CE="Warn when any null check can't be eliminated";
_tag_dyneither(_tmp2CE,sizeof(char),45);});_tmp2CD;});_tmp16A[58]=({struct
_tuple15*_tmp2C7=_region_malloc(r,sizeof(*_tmp2C7));_tmp2C7->f1=({const char*
_tmp2CC="-warnboundschecks";_tag_dyneither(_tmp2CC,sizeof(char),18);});_tmp2C7->f2=
0;_tmp2C7->f3=({const char*_tmp2CB="";_tag_dyneither(_tmp2CB,sizeof(char),1);});
_tmp2C7->f4=(void*)({struct Cyc_Arg_Set_spec_struct*_tmp2C9=_region_malloc(r,
sizeof(*_tmp2C9));_tmp2C9[0]=({struct Cyc_Arg_Set_spec_struct _tmp2CA;_tmp2CA.tag=
3;_tmp2CA.f1=& Cyc_Toc_warn_bounds_checks;_tmp2CA;});_tmp2C9;});_tmp2C7->f5=({
const char*_tmp2C8="Warn when bounds checks can't be eliminated";_tag_dyneither(
_tmp2C8,sizeof(char),44);});_tmp2C7;});_tmp16A[57]=({struct _tuple15*_tmp2C1=
_region_malloc(r,sizeof(*_tmp2C1));_tmp2C1->f1=({const char*_tmp2C6="-CI";
_tag_dyneither(_tmp2C6,sizeof(char),4);});_tmp2C1->f2=0;_tmp2C1->f3=({const char*
_tmp2C5=" <file>";_tag_dyneither(_tmp2C5,sizeof(char),8);});_tmp2C1->f4=(void*)({
struct Cyc_Arg_String_spec_struct*_tmp2C3=_region_malloc(r,sizeof(*_tmp2C3));
_tmp2C3[0]=({struct Cyc_Arg_String_spec_struct _tmp2C4;_tmp2C4.tag=5;_tmp2C4.f1=
Cyc_set_cyc_include;_tmp2C4;});_tmp2C3;});_tmp2C1->f5=({const char*_tmp2C2="Set cyc_include.h to be <file>";
_tag_dyneither(_tmp2C2,sizeof(char),31);});_tmp2C1;});_tmp16A[56]=({struct
_tuple15*_tmp2BB=_region_malloc(r,sizeof(*_tmp2BB));_tmp2BB->f1=({const char*
_tmp2C0="-nocyc_setjmp";_tag_dyneither(_tmp2C0,sizeof(char),14);});_tmp2BB->f2=0;
_tmp2BB->f3=({const char*_tmp2BF="";_tag_dyneither(_tmp2BF,sizeof(char),1);});
_tmp2BB->f4=(void*)({struct Cyc_Arg_Set_spec_struct*_tmp2BD=_region_malloc(r,
sizeof(*_tmp2BD));_tmp2BD[0]=({struct Cyc_Arg_Set_spec_struct _tmp2BE;_tmp2BE.tag=
3;_tmp2BE.f1=& Cyc_nocyc_setjmp_r;_tmp2BE;});_tmp2BD;});_tmp2BB->f5=({const char*
_tmp2BC="Do not use compiler special file cyc_setjmp.h";_tag_dyneither(_tmp2BC,
sizeof(char),46);});_tmp2BB;});_tmp16A[55]=({struct _tuple15*_tmp2B5=
_region_malloc(r,sizeof(*_tmp2B5));_tmp2B5->f1=({const char*_tmp2BA="-printalleffects";
_tag_dyneither(_tmp2BA,sizeof(char),17);});_tmp2B5->f2=0;_tmp2B5->f3=({const char*
_tmp2B9="";_tag_dyneither(_tmp2B9,sizeof(char),1);});_tmp2B5->f4=(void*)({struct
Cyc_Arg_Set_spec_struct*_tmp2B7=_region_malloc(r,sizeof(*_tmp2B7));_tmp2B7[0]=({
struct Cyc_Arg_Set_spec_struct _tmp2B8;_tmp2B8.tag=3;_tmp2B8.f1=& Cyc_print_all_effects_r;
_tmp2B8;});_tmp2B7;});_tmp2B5->f5=({const char*_tmp2B6="Print effects for functions (type debugging)";
_tag_dyneither(_tmp2B6,sizeof(char),45);});_tmp2B5;});_tmp16A[54]=({struct
_tuple15*_tmp2AF=_region_malloc(r,sizeof(*_tmp2AF));_tmp2AF->f1=({const char*
_tmp2B4="-printfullevars";_tag_dyneither(_tmp2B4,sizeof(char),16);});_tmp2AF->f2=
0;_tmp2AF->f3=({const char*_tmp2B3="";_tag_dyneither(_tmp2B3,sizeof(char),1);});
_tmp2AF->f4=(void*)({struct Cyc_Arg_Set_spec_struct*_tmp2B1=_region_malloc(r,
sizeof(*_tmp2B1));_tmp2B1[0]=({struct Cyc_Arg_Set_spec_struct _tmp2B2;_tmp2B2.tag=
3;_tmp2B2.f1=& Cyc_print_full_evars_r;_tmp2B2;});_tmp2B1;});_tmp2AF->f5=({const
char*_tmp2B0="Print full information for evars (type debugging)";_tag_dyneither(
_tmp2B0,sizeof(char),50);});_tmp2AF;});_tmp16A[53]=({struct _tuple15*_tmp2A9=
_region_malloc(r,sizeof(*_tmp2A9));_tmp2A9->f1=({const char*_tmp2AE="-printallkinds";
_tag_dyneither(_tmp2AE,sizeof(char),15);});_tmp2A9->f2=0;_tmp2A9->f3=({const char*
_tmp2AD="";_tag_dyneither(_tmp2AD,sizeof(char),1);});_tmp2A9->f4=(void*)({struct
Cyc_Arg_Set_spec_struct*_tmp2AB=_region_malloc(r,sizeof(*_tmp2AB));_tmp2AB[0]=({
struct Cyc_Arg_Set_spec_struct _tmp2AC;_tmp2AC.tag=3;_tmp2AC.f1=& Cyc_print_all_kinds_r;
_tmp2AC;});_tmp2AB;});_tmp2A9->f5=({const char*_tmp2AA="Always print kinds of type variables";
_tag_dyneither(_tmp2AA,sizeof(char),37);});_tmp2A9;});_tmp16A[52]=({struct
_tuple15*_tmp2A3=_region_malloc(r,sizeof(*_tmp2A3));_tmp2A3->f1=({const char*
_tmp2A8="-printalltvars";_tag_dyneither(_tmp2A8,sizeof(char),15);});_tmp2A3->f2=
0;_tmp2A3->f3=({const char*_tmp2A7="";_tag_dyneither(_tmp2A7,sizeof(char),1);});
_tmp2A3->f4=(void*)({struct Cyc_Arg_Set_spec_struct*_tmp2A5=_region_malloc(r,
sizeof(*_tmp2A5));_tmp2A5[0]=({struct Cyc_Arg_Set_spec_struct _tmp2A6;_tmp2A6.tag=
3;_tmp2A6.f1=& Cyc_print_all_tvars_r;_tmp2A6;});_tmp2A5;});_tmp2A3->f5=({const
char*_tmp2A4="Print all type variables (even implicit default effects)";
_tag_dyneither(_tmp2A4,sizeof(char),57);});_tmp2A3;});_tmp16A[51]=({struct
_tuple15*_tmp29D=_region_malloc(r,sizeof(*_tmp29D));_tmp29D->f1=({const char*
_tmp2A2="-noexpandtypedefs";_tag_dyneither(_tmp2A2,sizeof(char),18);});_tmp29D->f2=
0;_tmp29D->f3=({const char*_tmp2A1="";_tag_dyneither(_tmp2A1,sizeof(char),1);});
_tmp29D->f4=(void*)({struct Cyc_Arg_Set_spec_struct*_tmp29F=_region_malloc(r,
sizeof(*_tmp29F));_tmp29F[0]=({struct Cyc_Arg_Set_spec_struct _tmp2A0;_tmp2A0.tag=
3;_tmp2A0.f1=& Cyc_noexpand_r;_tmp2A0;});_tmp29F;});_tmp29D->f5=({const char*
_tmp29E="Don't expand typedefs in pretty printing";_tag_dyneither(_tmp29E,
sizeof(char),41);});_tmp29D;});_tmp16A[50]=({struct _tuple15*_tmp297=
_region_malloc(r,sizeof(*_tmp297));_tmp297->f1=({const char*_tmp29C="-noremoveunused";
_tag_dyneither(_tmp29C,sizeof(char),16);});_tmp297->f2=0;_tmp297->f3=({const char*
_tmp29B="";_tag_dyneither(_tmp29B,sizeof(char),1);});_tmp297->f4=(void*)({struct
Cyc_Arg_Set_spec_struct*_tmp299=_region_malloc(r,sizeof(*_tmp299));_tmp299[0]=({
struct Cyc_Arg_Set_spec_struct _tmp29A;_tmp29A.tag=3;_tmp29A.f1=& Cyc_noshake_r;
_tmp29A;});_tmp299;});_tmp297->f5=({const char*_tmp298="Don't remove externed variables that aren't used";
_tag_dyneither(_tmp298,sizeof(char),49);});_tmp297;});_tmp16A[49]=({struct
_tuple15*_tmp291=_region_malloc(r,sizeof(*_tmp291));_tmp291->f1=({const char*
_tmp296="-nogc";_tag_dyneither(_tmp296,sizeof(char),6);});_tmp291->f2=0;_tmp291->f3=({
const char*_tmp295="";_tag_dyneither(_tmp295,sizeof(char),1);});_tmp291->f4=(void*)({
struct Cyc_Arg_Set_spec_struct*_tmp293=_region_malloc(r,sizeof(*_tmp293));_tmp293[
0]=({struct Cyc_Arg_Set_spec_struct _tmp294;_tmp294.tag=3;_tmp294.f1=& Cyc_nogc_r;
_tmp294;});_tmp293;});_tmp291->f5=({const char*_tmp292="Don't link in the garbage collector";
_tag_dyneither(_tmp292,sizeof(char),36);});_tmp291;});_tmp16A[48]=({struct
_tuple15*_tmp28B=_region_malloc(r,sizeof(*_tmp28B));_tmp28B->f1=({const char*
_tmp290="-nocyc";_tag_dyneither(_tmp290,sizeof(char),7);});_tmp28B->f2=0;_tmp28B->f3=({
const char*_tmp28F="";_tag_dyneither(_tmp28F,sizeof(char),1);});_tmp28B->f4=(void*)({
struct Cyc_Arg_Unit_spec_struct*_tmp28D=_region_malloc(r,sizeof(*_tmp28D));
_tmp28D[0]=({struct Cyc_Arg_Unit_spec_struct _tmp28E;_tmp28E.tag=0;_tmp28E.f1=Cyc_set_nocyc;
_tmp28E;});_tmp28D;});_tmp28B->f5=({const char*_tmp28C="Don't add implicit namespace Cyc";
_tag_dyneither(_tmp28C,sizeof(char),33);});_tmp28B;});_tmp16A[47]=({struct
_tuple15*_tmp285=_region_malloc(r,sizeof(*_tmp285));_tmp285->f1=({const char*
_tmp28A="-no-cpp-precomp";_tag_dyneither(_tmp28A,sizeof(char),16);});_tmp285->f2=
0;_tmp285->f3=({const char*_tmp289="";_tag_dyneither(_tmp289,sizeof(char),1);});
_tmp285->f4=(void*)({struct Cyc_Arg_Unit_spec_struct*_tmp287=_region_malloc(r,
sizeof(*_tmp287));_tmp287[0]=({struct Cyc_Arg_Unit_spec_struct _tmp288;_tmp288.tag=
0;_tmp288.f1=Cyc_set_nocppprecomp;_tmp288;});_tmp287;});_tmp285->f5=({const char*
_tmp286="Don't do smart preprocessing (mac only)";_tag_dyneither(_tmp286,sizeof(
char),40);});_tmp285;});_tmp16A[46]=({struct _tuple15*_tmp27F=_region_malloc(r,
sizeof(*_tmp27F));_tmp27F->f1=({const char*_tmp284="-use-cpp";_tag_dyneither(
_tmp284,sizeof(char),9);});_tmp27F->f2=0;_tmp27F->f3=({const char*_tmp283="<path>";
_tag_dyneither(_tmp283,sizeof(char),7);});_tmp27F->f4=(void*)({struct Cyc_Arg_String_spec_struct*
_tmp281=_region_malloc(r,sizeof(*_tmp281));_tmp281[0]=({struct Cyc_Arg_String_spec_struct
_tmp282;_tmp282.tag=5;_tmp282.f1=Cyc_set_cpp;_tmp282;});_tmp281;});_tmp27F->f5=({
const char*_tmp280="Indicate which preprocessor to use";_tag_dyneither(_tmp280,
sizeof(char),35);});_tmp27F;});_tmp16A[45]=({struct _tuple15*_tmp279=
_region_malloc(r,sizeof(*_tmp279));_tmp279->f1=({const char*_tmp27E="--inline-checks";
_tag_dyneither(_tmp27E,sizeof(char),16);});_tmp279->f2=0;_tmp279->f3=({const char*
_tmp27D="";_tag_dyneither(_tmp27D,sizeof(char),1);});_tmp279->f4=(void*)({struct
Cyc_Arg_Unit_spec_struct*_tmp27B=_region_malloc(r,sizeof(*_tmp27B));_tmp27B[0]=({
struct Cyc_Arg_Unit_spec_struct _tmp27C;_tmp27C.tag=0;_tmp27C.f1=Cyc_set_inline_functions;
_tmp27C;});_tmp27B;});_tmp279->f5=({const char*_tmp27A="Inline bounds checks instead of #define";
_tag_dyneither(_tmp27A,sizeof(char),40);});_tmp279;});_tmp16A[44]=({struct
_tuple15*_tmp273=_region_malloc(r,sizeof(*_tmp273));_tmp273->f1=({const char*
_tmp278="--noboundschecks";_tag_dyneither(_tmp278,sizeof(char),17);});_tmp273->f2=
0;_tmp273->f3=({const char*_tmp277="";_tag_dyneither(_tmp277,sizeof(char),1);});
_tmp273->f4=(void*)({struct Cyc_Arg_Unit_spec_struct*_tmp275=_region_malloc(r,
sizeof(*_tmp275));_tmp275[0]=({struct Cyc_Arg_Unit_spec_struct _tmp276;_tmp276.tag=
0;_tmp276.f1=Cyc_set_noboundschecks;_tmp276;});_tmp275;});_tmp273->f5=({const
char*_tmp274="Disable bounds checks";_tag_dyneither(_tmp274,sizeof(char),22);});
_tmp273;});_tmp16A[43]=({struct _tuple15*_tmp26D=_region_malloc(r,sizeof(*_tmp26D));
_tmp26D->f1=({const char*_tmp272="--nonullchecks";_tag_dyneither(_tmp272,sizeof(
char),15);});_tmp26D->f2=0;_tmp26D->f3=({const char*_tmp271="";_tag_dyneither(
_tmp271,sizeof(char),1);});_tmp26D->f4=(void*)({struct Cyc_Arg_Unit_spec_struct*
_tmp26F=_region_malloc(r,sizeof(*_tmp26F));_tmp26F[0]=({struct Cyc_Arg_Unit_spec_struct
_tmp270;_tmp270.tag=0;_tmp270.f1=Cyc_set_nonullchecks;_tmp270;});_tmp26F;});
_tmp26D->f5=({const char*_tmp26E="Disable null checks";_tag_dyneither(_tmp26E,
sizeof(char),20);});_tmp26D;});_tmp16A[42]=({struct _tuple15*_tmp267=
_region_malloc(r,sizeof(*_tmp267));_tmp267->f1=({const char*_tmp26C="--nochecks";
_tag_dyneither(_tmp26C,sizeof(char),11);});_tmp267->f2=0;_tmp267->f3=({const char*
_tmp26B="";_tag_dyneither(_tmp26B,sizeof(char),1);});_tmp267->f4=(void*)({struct
Cyc_Arg_Unit_spec_struct*_tmp269=_region_malloc(r,sizeof(*_tmp269));_tmp269[0]=({
struct Cyc_Arg_Unit_spec_struct _tmp26A;_tmp26A.tag=0;_tmp26A.f1=Cyc_set_nochecks;
_tmp26A;});_tmp269;});_tmp267->f5=({const char*_tmp268="Disable bounds/null checks";
_tag_dyneither(_tmp268,sizeof(char),27);});_tmp267;});_tmp16A[41]=({struct
_tuple15*_tmp261=_region_malloc(r,sizeof(*_tmp261));_tmp261->f1=({const char*
_tmp266="--lineno";_tag_dyneither(_tmp266,sizeof(char),9);});_tmp261->f2=0;
_tmp261->f3=({const char*_tmp265="";_tag_dyneither(_tmp265,sizeof(char),1);});
_tmp261->f4=(void*)({struct Cyc_Arg_Unit_spec_struct*_tmp263=_region_malloc(r,
sizeof(*_tmp263));_tmp263[0]=({struct Cyc_Arg_Unit_spec_struct _tmp264;_tmp264.tag=
0;_tmp264.f1=Cyc_set_lineno;_tmp264;});_tmp263;});_tmp261->f5=({const char*
_tmp262="Generate line numbers for debugging";_tag_dyneither(_tmp262,sizeof(char),
36);});_tmp261;});_tmp16A[40]=({struct _tuple15*_tmp25B=_region_malloc(r,sizeof(*
_tmp25B));_tmp25B->f1=({const char*_tmp260="-save-c";_tag_dyneither(_tmp260,
sizeof(char),8);});_tmp25B->f2=0;_tmp25B->f3=({const char*_tmp25F="";
_tag_dyneither(_tmp25F,sizeof(char),1);});_tmp25B->f4=(void*)({struct Cyc_Arg_Set_spec_struct*
_tmp25D=_region_malloc(r,sizeof(*_tmp25D));_tmp25D[0]=({struct Cyc_Arg_Set_spec_struct
_tmp25E;_tmp25E.tag=3;_tmp25E.f1=& Cyc_save_c_r;_tmp25E;});_tmp25D;});_tmp25B->f5=({
const char*_tmp25C="Don't delete temporary C files";_tag_dyneither(_tmp25C,
sizeof(char),31);});_tmp25B;});_tmp16A[39]=({struct _tuple15*_tmp255=
_region_malloc(r,sizeof(*_tmp255));_tmp255->f1=({const char*_tmp25A="-save-temps";
_tag_dyneither(_tmp25A,sizeof(char),12);});_tmp255->f2=0;_tmp255->f3=({const char*
_tmp259="";_tag_dyneither(_tmp259,sizeof(char),1);});_tmp255->f4=(void*)({struct
Cyc_Arg_Unit_spec_struct*_tmp257=_region_malloc(r,sizeof(*_tmp257));_tmp257[0]=({
struct Cyc_Arg_Unit_spec_struct _tmp258;_tmp258.tag=0;_tmp258.f1=Cyc_set_save_temps;
_tmp258;});_tmp257;});_tmp255->f5=({const char*_tmp256="Don't delete temporary files";
_tag_dyneither(_tmp256,sizeof(char),29);});_tmp255;});_tmp16A[38]=({struct
_tuple15*_tmp24F=_region_malloc(r,sizeof(*_tmp24F));_tmp24F->f1=({const char*
_tmp254="-c-comp";_tag_dyneither(_tmp254,sizeof(char),8);});_tmp24F->f2=0;
_tmp24F->f3=({const char*_tmp253=" <compiler>";_tag_dyneither(_tmp253,sizeof(char),
12);});_tmp24F->f4=(void*)({struct Cyc_Arg_String_spec_struct*_tmp251=
_region_malloc(r,sizeof(*_tmp251));_tmp251[0]=({struct Cyc_Arg_String_spec_struct
_tmp252;_tmp252.tag=5;_tmp252.f1=Cyc_set_c_compiler;_tmp252;});_tmp251;});
_tmp24F->f5=({const char*_tmp250="Produce C output for the given compiler";
_tag_dyneither(_tmp250,sizeof(char),40);});_tmp24F;});_tmp16A[37]=({struct
_tuple15*_tmp249=_region_malloc(r,sizeof(*_tmp249));_tmp249->f1=({const char*
_tmp24E="-un-gcc";_tag_dyneither(_tmp24E,sizeof(char),8);});_tmp249->f2=0;
_tmp249->f3=({const char*_tmp24D="";_tag_dyneither(_tmp24D,sizeof(char),1);});
_tmp249->f4=(void*)({struct Cyc_Arg_Unit_spec_struct*_tmp24B=_region_malloc(r,
sizeof(*_tmp24B));_tmp24B[0]=({struct Cyc_Arg_Unit_spec_struct _tmp24C;_tmp24C.tag=
0;_tmp24C.f1=Cyc_set_tovc;_tmp24C;});_tmp24B;});_tmp249->f5=({const char*_tmp24A="Avoid gcc extensions in C output (except for attributes)";
_tag_dyneither(_tmp24A,sizeof(char),57);});_tmp249;});_tmp16A[36]=({struct
_tuple15*_tmp243=_region_malloc(r,sizeof(*_tmp243));_tmp243->f1=({const char*
_tmp248="-elim-statement-expressions";_tag_dyneither(_tmp248,sizeof(char),28);});
_tmp243->f2=0;_tmp243->f3=({const char*_tmp247="";_tag_dyneither(_tmp247,sizeof(
char),1);});_tmp243->f4=(void*)({struct Cyc_Arg_Set_spec_struct*_tmp245=
_region_malloc(r,sizeof(*_tmp245));_tmp245[0]=({struct Cyc_Arg_Set_spec_struct
_tmp246;_tmp246.tag=3;_tmp246.f1=& Cyc_elim_se_r;_tmp246;});_tmp245;});_tmp243->f5=({
const char*_tmp244="Avoid statement expressions in C output (implies --inline-checks)";
_tag_dyneither(_tmp244,sizeof(char),66);});_tmp243;});_tmp16A[35]=({struct
_tuple15*_tmp23D=_region_malloc(r,sizeof(*_tmp23D));_tmp23D->f1=({const char*
_tmp242="-up";_tag_dyneither(_tmp242,sizeof(char),4);});_tmp23D->f2=0;_tmp23D->f3=({
const char*_tmp241="";_tag_dyneither(_tmp241,sizeof(char),1);});_tmp23D->f4=(void*)({
struct Cyc_Arg_Clear_spec_struct*_tmp23F=_region_malloc(r,sizeof(*_tmp23F));
_tmp23F[0]=({struct Cyc_Arg_Clear_spec_struct _tmp240;_tmp240.tag=4;_tmp240.f1=&
Cyc_pp_r;_tmp240;});_tmp23F;});_tmp23D->f5=({const char*_tmp23E="Ugly print the C code that Cyclone generates (default)";
_tag_dyneither(_tmp23E,sizeof(char),55);});_tmp23D;});_tmp16A[34]=({struct
_tuple15*_tmp237=_region_malloc(r,sizeof(*_tmp237));_tmp237->f1=({const char*
_tmp23C="-pp";_tag_dyneither(_tmp23C,sizeof(char),4);});_tmp237->f2=0;_tmp237->f3=({
const char*_tmp23B="";_tag_dyneither(_tmp23B,sizeof(char),1);});_tmp237->f4=(void*)({
struct Cyc_Arg_Set_spec_struct*_tmp239=_region_malloc(r,sizeof(*_tmp239));_tmp239[
0]=({struct Cyc_Arg_Set_spec_struct _tmp23A;_tmp23A.tag=3;_tmp23A.f1=& Cyc_pp_r;
_tmp23A;});_tmp239;});_tmp237->f5=({const char*_tmp238="Pretty print the C code that Cyclone generates";
_tag_dyneither(_tmp238,sizeof(char),47);});_tmp237;});_tmp16A[33]=({struct
_tuple15*_tmp231=_region_malloc(r,sizeof(*_tmp231));_tmp231->f1=({const char*
_tmp236="-ic";_tag_dyneither(_tmp236,sizeof(char),4);});_tmp231->f2=0;_tmp231->f3=({
const char*_tmp235="";_tag_dyneither(_tmp235,sizeof(char),1);});_tmp231->f4=(void*)({
struct Cyc_Arg_Set_spec_struct*_tmp233=_region_malloc(r,sizeof(*_tmp233));_tmp233[
0]=({struct Cyc_Arg_Set_spec_struct _tmp234;_tmp234.tag=3;_tmp234.f1=& Cyc_ic_r;
_tmp234;});_tmp233;});_tmp231->f5=({const char*_tmp232="Activate the link-checker";
_tag_dyneither(_tmp232,sizeof(char),26);});_tmp231;});_tmp16A[32]=({struct
_tuple15*_tmp22B=_region_malloc(r,sizeof(*_tmp22B));_tmp22B->f1=({const char*
_tmp230="-stopafter-toc";_tag_dyneither(_tmp230,sizeof(char),15);});_tmp22B->f2=
0;_tmp22B->f3=({const char*_tmp22F="";_tag_dyneither(_tmp22F,sizeof(char),1);});
_tmp22B->f4=(void*)({struct Cyc_Arg_Set_spec_struct*_tmp22D=_region_malloc(r,
sizeof(*_tmp22D));_tmp22D[0]=({struct Cyc_Arg_Set_spec_struct _tmp22E;_tmp22E.tag=
3;_tmp22E.f1=& Cyc_toc_r;_tmp22E;});_tmp22D;});_tmp22B->f5=({const char*_tmp22C="Stop after translation to C";
_tag_dyneither(_tmp22C,sizeof(char),28);});_tmp22B;});_tmp16A[31]=({struct
_tuple15*_tmp225=_region_malloc(r,sizeof(*_tmp225));_tmp225->f1=({const char*
_tmp22A="-stopafter-cf";_tag_dyneither(_tmp22A,sizeof(char),14);});_tmp225->f2=0;
_tmp225->f3=({const char*_tmp229="";_tag_dyneither(_tmp229,sizeof(char),1);});
_tmp225->f4=(void*)({struct Cyc_Arg_Set_spec_struct*_tmp227=_region_malloc(r,
sizeof(*_tmp227));_tmp227[0]=({struct Cyc_Arg_Set_spec_struct _tmp228;_tmp228.tag=
3;_tmp228.f1=& Cyc_cf_r;_tmp228;});_tmp227;});_tmp225->f5=({const char*_tmp226="Stop after control-flow checking";
_tag_dyneither(_tmp226,sizeof(char),33);});_tmp225;});_tmp16A[30]=({struct
_tuple15*_tmp21F=_region_malloc(r,sizeof(*_tmp21F));_tmp21F->f1=({const char*
_tmp224="-noprint";_tag_dyneither(_tmp224,sizeof(char),9);});_tmp21F->f2=0;
_tmp21F->f3=({const char*_tmp223="";_tag_dyneither(_tmp223,sizeof(char),1);});
_tmp21F->f4=(void*)({struct Cyc_Arg_Set_spec_struct*_tmp221=_region_malloc(r,
sizeof(*_tmp221));_tmp221[0]=({struct Cyc_Arg_Set_spec_struct _tmp222;_tmp222.tag=
3;_tmp222.f1=& Cyc_noprint_r;_tmp222;});_tmp221;});_tmp21F->f5=({const char*
_tmp220="Do not print output (when stopping early)";_tag_dyneither(_tmp220,
sizeof(char),42);});_tmp21F;});_tmp16A[29]=({struct _tuple15*_tmp219=
_region_malloc(r,sizeof(*_tmp219));_tmp219->f1=({const char*_tmp21E="-stopafter-tc";
_tag_dyneither(_tmp21E,sizeof(char),14);});_tmp219->f2=0;_tmp219->f3=({const char*
_tmp21D="";_tag_dyneither(_tmp21D,sizeof(char),1);});_tmp219->f4=(void*)({struct
Cyc_Arg_Set_spec_struct*_tmp21B=_region_malloc(r,sizeof(*_tmp21B));_tmp21B[0]=({
struct Cyc_Arg_Set_spec_struct _tmp21C;_tmp21C.tag=3;_tmp21C.f1=& Cyc_tc_r;_tmp21C;});
_tmp21B;});_tmp219->f5=({const char*_tmp21A="Stop after type checking";
_tag_dyneither(_tmp21A,sizeof(char),25);});_tmp219;});_tmp16A[28]=({struct
_tuple15*_tmp213=_region_malloc(r,sizeof(*_tmp213));_tmp213->f1=({const char*
_tmp218="-stopafter-parse";_tag_dyneither(_tmp218,sizeof(char),17);});_tmp213->f2=
0;_tmp213->f3=({const char*_tmp217="";_tag_dyneither(_tmp217,sizeof(char),1);});
_tmp213->f4=(void*)({struct Cyc_Arg_Set_spec_struct*_tmp215=_region_malloc(r,
sizeof(*_tmp215));_tmp215[0]=({struct Cyc_Arg_Set_spec_struct _tmp216;_tmp216.tag=
3;_tmp216.f1=& Cyc_parseonly_r;_tmp216;});_tmp215;});_tmp213->f5=({const char*
_tmp214="Stop after parsing";_tag_dyneither(_tmp214,sizeof(char),19);});_tmp213;});
_tmp16A[27]=({struct _tuple15*_tmp20D=_region_malloc(r,sizeof(*_tmp20D));_tmp20D->f1=({
const char*_tmp212="-E";_tag_dyneither(_tmp212,sizeof(char),3);});_tmp20D->f2=0;
_tmp20D->f3=({const char*_tmp211="";_tag_dyneither(_tmp211,sizeof(char),1);});
_tmp20D->f4=(void*)({struct Cyc_Arg_Set_spec_struct*_tmp20F=_region_malloc(r,
sizeof(*_tmp20F));_tmp20F[0]=({struct Cyc_Arg_Set_spec_struct _tmp210;_tmp210.tag=
3;_tmp210.f1=& Cyc_stop_after_cpp_r;_tmp210;});_tmp20F;});_tmp20D->f5=({const char*
_tmp20E="Stop after preprocessing";_tag_dyneither(_tmp20E,sizeof(char),25);});
_tmp20D;});_tmp16A[26]=({struct _tuple15*_tmp207=_region_malloc(r,sizeof(*_tmp207));
_tmp207->f1=({const char*_tmp20C="-Wall";_tag_dyneither(_tmp20C,sizeof(char),6);});
_tmp207->f2=0;_tmp207->f3=({const char*_tmp20B="";_tag_dyneither(_tmp20B,sizeof(
char),1);});_tmp207->f4=(void*)({struct Cyc_Arg_Unit_spec_struct*_tmp209=
_region_malloc(r,sizeof(*_tmp209));_tmp209[0]=({struct Cyc_Arg_Unit_spec_struct
_tmp20A;_tmp20A.tag=0;_tmp20A.f1=Cyc_set_all_warnings;_tmp20A;});_tmp209;});
_tmp207->f5=({const char*_tmp208="Turn on all warnings";_tag_dyneither(_tmp208,
sizeof(char),21);});_tmp207;});_tmp16A[25]=({struct _tuple15*_tmp201=
_region_malloc(r,sizeof(*_tmp201));_tmp201->f1=({const char*_tmp206="-b";
_tag_dyneither(_tmp206,sizeof(char),3);});_tmp201->f2=0;_tmp201->f3=({const char*
_tmp205="<arch>";_tag_dyneither(_tmp205,sizeof(char),7);});_tmp201->f4=(void*)({
struct Cyc_Arg_String_spec_struct*_tmp203=_region_malloc(r,sizeof(*_tmp203));
_tmp203[0]=({struct Cyc_Arg_String_spec_struct _tmp204;_tmp204.tag=5;_tmp204.f1=
Cyc_set_target_arch;_tmp204;});_tmp203;});_tmp201->f5=({const char*_tmp202="Set target architecture; implies -stopafter-toc";
_tag_dyneither(_tmp202,sizeof(char),48);});_tmp201;});_tmp16A[24]=({struct
_tuple15*_tmp1FB=_region_malloc(r,sizeof(*_tmp1FB));_tmp1FB->f1=({const char*
_tmp200="-MT";_tag_dyneither(_tmp200,sizeof(char),4);});_tmp1FB->f2=0;_tmp1FB->f3=({
const char*_tmp1FF=" <target>";_tag_dyneither(_tmp1FF,sizeof(char),10);});_tmp1FB->f4=(
void*)({struct Cyc_Arg_String_spec_struct*_tmp1FD=_region_malloc(r,sizeof(*
_tmp1FD));_tmp1FD[0]=({struct Cyc_Arg_String_spec_struct _tmp1FE;_tmp1FE.tag=5;
_tmp1FE.f1=Cyc_set_dependencies_target;_tmp1FE;});_tmp1FD;});_tmp1FB->f5=({const
char*_tmp1FC="Give target for dependencies";_tag_dyneither(_tmp1FC,sizeof(char),
29);});_tmp1FB;});_tmp16A[23]=({struct _tuple15*_tmp1F5=_region_malloc(r,sizeof(*
_tmp1F5));_tmp1F5->f1=({const char*_tmp1FA="-MG";_tag_dyneither(_tmp1FA,sizeof(
char),4);});_tmp1F5->f2=0;_tmp1F5->f3=({const char*_tmp1F9="";_tag_dyneither(
_tmp1F9,sizeof(char),1);});_tmp1F5->f4=(void*)({struct Cyc_Arg_Flag_spec_struct*
_tmp1F7=_region_malloc(r,sizeof(*_tmp1F7));_tmp1F7[0]=({struct Cyc_Arg_Flag_spec_struct
_tmp1F8;_tmp1F8.tag=1;_tmp1F8.f1=Cyc_add_cpparg;_tmp1F8;});_tmp1F7;});_tmp1F5->f5=({
const char*_tmp1F6="When producing dependencies assume that missing files are generated";
_tag_dyneither(_tmp1F6,sizeof(char),68);});_tmp1F5;});_tmp16A[22]=({struct
_tuple15*_tmp1EF=_region_malloc(r,sizeof(*_tmp1EF));_tmp1EF->f1=({const char*
_tmp1F4="-M";_tag_dyneither(_tmp1F4,sizeof(char),3);});_tmp1EF->f2=0;_tmp1EF->f3=({
const char*_tmp1F3="";_tag_dyneither(_tmp1F3,sizeof(char),1);});_tmp1EF->f4=(void*)({
struct Cyc_Arg_Unit_spec_struct*_tmp1F1=_region_malloc(r,sizeof(*_tmp1F1));
_tmp1F1[0]=({struct Cyc_Arg_Unit_spec_struct _tmp1F2;_tmp1F2.tag=0;_tmp1F2.f1=Cyc_set_produce_dependencies;
_tmp1F2;});_tmp1F1;});_tmp1EF->f5=({const char*_tmp1F0="Produce dependencies";
_tag_dyneither(_tmp1F0,sizeof(char),21);});_tmp1EF;});_tmp16A[21]=({struct
_tuple15*_tmp1E9=_region_malloc(r,sizeof(*_tmp1E9));_tmp1E9->f1=({const char*
_tmp1EE="-S";_tag_dyneither(_tmp1EE,sizeof(char),3);});_tmp1E9->f2=0;_tmp1E9->f3=({
const char*_tmp1ED="";_tag_dyneither(_tmp1ED,sizeof(char),1);});_tmp1E9->f4=(void*)({
struct Cyc_Arg_Unit_spec_struct*_tmp1EB=_region_malloc(r,sizeof(*_tmp1EB));
_tmp1EB[0]=({struct Cyc_Arg_Unit_spec_struct _tmp1EC;_tmp1EC.tag=0;_tmp1EC.f1=Cyc_set_stop_after_asmfile;
_tmp1EC;});_tmp1EB;});_tmp1E9->f5=({const char*_tmp1EA="Stop after producing assembly code";
_tag_dyneither(_tmp1EA,sizeof(char),35);});_tmp1E9;});_tmp16A[20]=({struct
_tuple15*_tmp1E3=_region_malloc(r,sizeof(*_tmp1E3));_tmp1E3->f1=({const char*
_tmp1E8="-pa";_tag_dyneither(_tmp1E8,sizeof(char),4);});_tmp1E3->f2=0;_tmp1E3->f3=({
const char*_tmp1E7="";_tag_dyneither(_tmp1E7,sizeof(char),1);});_tmp1E3->f4=(void*)({
struct Cyc_Arg_Unit_spec_struct*_tmp1E5=_region_malloc(r,sizeof(*_tmp1E5));
_tmp1E5[0]=({struct Cyc_Arg_Unit_spec_struct _tmp1E6;_tmp1E6.tag=0;_tmp1E6.f1=Cyc_set_pa;
_tmp1E6;});_tmp1E5;});_tmp1E3->f5=({const char*_tmp1E4="Compile for profiling with aprof";
_tag_dyneither(_tmp1E4,sizeof(char),33);});_tmp1E3;});_tmp16A[19]=({struct
_tuple15*_tmp1DD=_region_malloc(r,sizeof(*_tmp1DD));_tmp1DD->f1=({const char*
_tmp1E2="-pg";_tag_dyneither(_tmp1E2,sizeof(char),4);});_tmp1DD->f2=0;_tmp1DD->f3=({
const char*_tmp1E1="";_tag_dyneither(_tmp1E1,sizeof(char),1);});_tmp1DD->f4=(void*)({
struct Cyc_Arg_Unit_spec_struct*_tmp1DF=_region_malloc(r,sizeof(*_tmp1DF));
_tmp1DF[0]=({struct Cyc_Arg_Unit_spec_struct _tmp1E0;_tmp1E0.tag=0;_tmp1E0.f1=Cyc_set_pg;
_tmp1E0;});_tmp1DF;});_tmp1DD->f5=({const char*_tmp1DE="Compile for profiling with gprof";
_tag_dyneither(_tmp1DE,sizeof(char),33);});_tmp1DD;});_tmp16A[18]=({struct
_tuple15*_tmp1D7=_region_malloc(r,sizeof(*_tmp1D7));_tmp1D7->f1=({const char*
_tmp1DC="-p";_tag_dyneither(_tmp1DC,sizeof(char),3);});_tmp1D7->f2=0;_tmp1D7->f3=({
const char*_tmp1DB="";_tag_dyneither(_tmp1DB,sizeof(char),1);});_tmp1D7->f4=(void*)({
struct Cyc_Arg_Flag_spec_struct*_tmp1D9=_region_malloc(r,sizeof(*_tmp1D9));
_tmp1D9[0]=({struct Cyc_Arg_Flag_spec_struct _tmp1DA;_tmp1DA.tag=1;_tmp1DA.f1=Cyc_add_ccarg;
_tmp1DA;});_tmp1D9;});_tmp1D7->f5=({const char*_tmp1D8="Compile for profiling with prof";
_tag_dyneither(_tmp1D8,sizeof(char),32);});_tmp1D7;});_tmp16A[17]=({struct
_tuple15*_tmp1D1=_region_malloc(r,sizeof(*_tmp1D1));_tmp1D1->f1=({const char*
_tmp1D6="-g";_tag_dyneither(_tmp1D6,sizeof(char),3);});_tmp1D1->f2=0;_tmp1D1->f3=({
const char*_tmp1D5="";_tag_dyneither(_tmp1D5,sizeof(char),1);});_tmp1D1->f4=(void*)({
struct Cyc_Arg_Flag_spec_struct*_tmp1D3=_region_malloc(r,sizeof(*_tmp1D3));
_tmp1D3[0]=({struct Cyc_Arg_Flag_spec_struct _tmp1D4;_tmp1D4.tag=1;_tmp1D4.f1=Cyc_add_ccarg;
_tmp1D4;});_tmp1D3;});_tmp1D1->f5=({const char*_tmp1D2="Compile for debugging";
_tag_dyneither(_tmp1D2,sizeof(char),22);});_tmp1D1;});_tmp16A[16]=({struct
_tuple15*_tmp1CB=_region_malloc(r,sizeof(*_tmp1CB));_tmp1CB->f1=({const char*
_tmp1D0="-fomit-frame-pointer";_tag_dyneither(_tmp1D0,sizeof(char),21);});
_tmp1CB->f2=0;_tmp1CB->f3=({const char*_tmp1CF="";_tag_dyneither(_tmp1CF,sizeof(
char),1);});_tmp1CB->f4=(void*)({struct Cyc_Arg_Flag_spec_struct*_tmp1CD=
_region_malloc(r,sizeof(*_tmp1CD));_tmp1CD[0]=({struct Cyc_Arg_Flag_spec_struct
_tmp1CE;_tmp1CE.tag=1;_tmp1CE.f1=Cyc_add_ccarg;_tmp1CE;});_tmp1CD;});_tmp1CB->f5=({
const char*_tmp1CC="Omit frame pointer";_tag_dyneither(_tmp1CC,sizeof(char),19);});
_tmp1CB;});_tmp16A[15]=({struct _tuple15*_tmp1C5=_region_malloc(r,sizeof(*_tmp1C5));
_tmp1C5->f1=({const char*_tmp1CA="-O3";_tag_dyneither(_tmp1CA,sizeof(char),4);});
_tmp1C5->f2=0;_tmp1C5->f3=({const char*_tmp1C9="";_tag_dyneither(_tmp1C9,sizeof(
char),1);});_tmp1C5->f4=(void*)({struct Cyc_Arg_Flag_spec_struct*_tmp1C7=
_region_malloc(r,sizeof(*_tmp1C7));_tmp1C7[0]=({struct Cyc_Arg_Flag_spec_struct
_tmp1C8;_tmp1C8.tag=1;_tmp1C8.f1=Cyc_add_ccarg;_tmp1C8;});_tmp1C7;});_tmp1C5->f5=({
const char*_tmp1C6="Even more optimization";_tag_dyneither(_tmp1C6,sizeof(char),
23);});_tmp1C5;});_tmp16A[14]=({struct _tuple15*_tmp1BF=_region_malloc(r,sizeof(*
_tmp1BF));_tmp1BF->f1=({const char*_tmp1C4="-O2";_tag_dyneither(_tmp1C4,sizeof(
char),4);});_tmp1BF->f2=0;_tmp1BF->f3=({const char*_tmp1C3="";_tag_dyneither(
_tmp1C3,sizeof(char),1);});_tmp1BF->f4=(void*)({struct Cyc_Arg_Flag_spec_struct*
_tmp1C1=_region_malloc(r,sizeof(*_tmp1C1));_tmp1C1[0]=({struct Cyc_Arg_Flag_spec_struct
_tmp1C2;_tmp1C2.tag=1;_tmp1C2.f1=Cyc_add_ccarg;_tmp1C2;});_tmp1C1;});_tmp1BF->f5=({
const char*_tmp1C0="A higher level of optimization";_tag_dyneither(_tmp1C0,
sizeof(char),31);});_tmp1BF;});_tmp16A[13]=({struct _tuple15*_tmp1B9=
_region_malloc(r,sizeof(*_tmp1B9));_tmp1B9->f1=({const char*_tmp1BE="-O";
_tag_dyneither(_tmp1BE,sizeof(char),3);});_tmp1B9->f2=0;_tmp1B9->f3=({const char*
_tmp1BD="";_tag_dyneither(_tmp1BD,sizeof(char),1);});_tmp1B9->f4=(void*)({struct
Cyc_Arg_Flag_spec_struct*_tmp1BB=_region_malloc(r,sizeof(*_tmp1BB));_tmp1BB[0]=({
struct Cyc_Arg_Flag_spec_struct _tmp1BC;_tmp1BC.tag=1;_tmp1BC.f1=Cyc_add_ccarg;
_tmp1BC;});_tmp1BB;});_tmp1B9->f5=({const char*_tmp1BA="Optimize";_tag_dyneither(
_tmp1BA,sizeof(char),9);});_tmp1B9;});_tmp16A[12]=({struct _tuple15*_tmp1B3=
_region_malloc(r,sizeof(*_tmp1B3));_tmp1B3->f1=({const char*_tmp1B8="-O0";
_tag_dyneither(_tmp1B8,sizeof(char),4);});_tmp1B3->f2=0;_tmp1B3->f3=({const char*
_tmp1B7="";_tag_dyneither(_tmp1B7,sizeof(char),1);});_tmp1B3->f4=(void*)({struct
Cyc_Arg_Flag_spec_struct*_tmp1B5=_region_malloc(r,sizeof(*_tmp1B5));_tmp1B5[0]=({
struct Cyc_Arg_Flag_spec_struct _tmp1B6;_tmp1B6.tag=1;_tmp1B6.f1=Cyc_add_ccarg;
_tmp1B6;});_tmp1B5;});_tmp1B3->f5=({const char*_tmp1B4="Don't optimize";
_tag_dyneither(_tmp1B4,sizeof(char),15);});_tmp1B3;});_tmp16A[11]=({struct
_tuple15*_tmp1AD=_region_malloc(r,sizeof(*_tmp1AD));_tmp1AD->f1=({const char*
_tmp1B2="-s";_tag_dyneither(_tmp1B2,sizeof(char),3);});_tmp1AD->f2=0;_tmp1AD->f3=({
const char*_tmp1B1="";_tag_dyneither(_tmp1B1,sizeof(char),1);});_tmp1AD->f4=(void*)({
struct Cyc_Arg_Flag_spec_struct*_tmp1AF=_region_malloc(r,sizeof(*_tmp1AF));
_tmp1AF[0]=({struct Cyc_Arg_Flag_spec_struct _tmp1B0;_tmp1B0.tag=1;_tmp1B0.f1=Cyc_add_ccarg;
_tmp1B0;});_tmp1AF;});_tmp1AD->f5=({const char*_tmp1AE="Remove all symbol table and relocation info from executable";
_tag_dyneither(_tmp1AE,sizeof(char),60);});_tmp1AD;});_tmp16A[10]=({struct
_tuple15*_tmp1A7=_region_malloc(r,sizeof(*_tmp1A7));_tmp1A7->f1=({const char*
_tmp1AC="-x";_tag_dyneither(_tmp1AC,sizeof(char),3);});_tmp1A7->f2=0;_tmp1A7->f3=({
const char*_tmp1AB=" <language>";_tag_dyneither(_tmp1AB,sizeof(char),12);});
_tmp1A7->f4=(void*)({struct Cyc_Arg_String_spec_struct*_tmp1A9=_region_malloc(r,
sizeof(*_tmp1A9));_tmp1A9[0]=({struct Cyc_Arg_String_spec_struct _tmp1AA;_tmp1AA.tag=
5;_tmp1AA.f1=Cyc_set_inputtype;_tmp1AA;});_tmp1A9;});_tmp1A7->f5=({const char*
_tmp1A8="Specify <language> for the following input files";_tag_dyneither(
_tmp1A8,sizeof(char),49);});_tmp1A7;});_tmp16A[9]=({struct _tuple15*_tmp1A1=
_region_malloc(r,sizeof(*_tmp1A1));_tmp1A1->f1=({const char*_tmp1A6="-c";
_tag_dyneither(_tmp1A6,sizeof(char),3);});_tmp1A1->f2=0;_tmp1A1->f3=({const char*
_tmp1A5="";_tag_dyneither(_tmp1A5,sizeof(char),1);});_tmp1A1->f4=(void*)({struct
Cyc_Arg_Unit_spec_struct*_tmp1A3=_region_malloc(r,sizeof(*_tmp1A3));_tmp1A3[0]=({
struct Cyc_Arg_Unit_spec_struct _tmp1A4;_tmp1A4.tag=0;_tmp1A4.f1=Cyc_set_stop_after_objectfile;
_tmp1A4;});_tmp1A3;});_tmp1A1->f5=({const char*_tmp1A2="Produce an object file instead of an executable; do not link";
_tag_dyneither(_tmp1A2,sizeof(char),61);});_tmp1A1;});_tmp16A[8]=({struct
_tuple15*_tmp19B=_region_malloc(r,sizeof(*_tmp19B));_tmp19B->f1=({const char*
_tmp1A0="-m";_tag_dyneither(_tmp1A0,sizeof(char),3);});_tmp19B->f2=1;_tmp19B->f3=({
const char*_tmp19F="<option>";_tag_dyneither(_tmp19F,sizeof(char),9);});_tmp19B->f4=(
void*)({struct Cyc_Arg_Flag_spec_struct*_tmp19D=_region_malloc(r,sizeof(*_tmp19D));
_tmp19D[0]=({struct Cyc_Arg_Flag_spec_struct _tmp19E;_tmp19E.tag=1;_tmp19E.f1=Cyc_add_marg;
_tmp19E;});_tmp19D;});_tmp19B->f5=({const char*_tmp19C="GCC specific: pass machine-dependent flag on to gcc";
_tag_dyneither(_tmp19C,sizeof(char),52);});_tmp19B;});_tmp16A[7]=({struct
_tuple15*_tmp195=_region_malloc(r,sizeof(*_tmp195));_tmp195->f1=({const char*
_tmp19A="-l";_tag_dyneither(_tmp19A,sizeof(char),3);});_tmp195->f2=1;_tmp195->f3=({
const char*_tmp199="<libname>";_tag_dyneither(_tmp199,sizeof(char),10);});_tmp195->f4=(
void*)({struct Cyc_Arg_Flag_spec_struct*_tmp197=_region_malloc(r,sizeof(*_tmp197));
_tmp197[0]=({struct Cyc_Arg_Flag_spec_struct _tmp198;_tmp198.tag=1;_tmp198.f1=Cyc_add_libarg;
_tmp198;});_tmp197;});_tmp195->f5=({const char*_tmp196="Library file";
_tag_dyneither(_tmp196,sizeof(char),13);});_tmp195;});_tmp16A[6]=({struct
_tuple15*_tmp18F=_region_malloc(r,sizeof(*_tmp18F));_tmp18F->f1=({const char*
_tmp194="-L";_tag_dyneither(_tmp194,sizeof(char),3);});_tmp18F->f2=1;_tmp18F->f3=({
const char*_tmp193="<dir>";_tag_dyneither(_tmp193,sizeof(char),6);});_tmp18F->f4=(
void*)({struct Cyc_Arg_Flag_spec_struct*_tmp191=_region_malloc(r,sizeof(*_tmp191));
_tmp191[0]=({struct Cyc_Arg_Flag_spec_struct _tmp192;_tmp192.tag=1;_tmp192.f1=Cyc_add_ccarg;
_tmp192;});_tmp191;});_tmp18F->f5=({const char*_tmp190="Add to the list of directories for -l";
_tag_dyneither(_tmp190,sizeof(char),38);});_tmp18F;});_tmp16A[5]=({struct
_tuple15*_tmp189=_region_malloc(r,sizeof(*_tmp189));_tmp189->f1=({const char*
_tmp18E="-I";_tag_dyneither(_tmp18E,sizeof(char),3);});_tmp189->f2=1;_tmp189->f3=({
const char*_tmp18D="<dir>";_tag_dyneither(_tmp18D,sizeof(char),6);});_tmp189->f4=(
void*)({struct Cyc_Arg_Flag_spec_struct*_tmp18B=_region_malloc(r,sizeof(*_tmp18B));
_tmp18B[0]=({struct Cyc_Arg_Flag_spec_struct _tmp18C;_tmp18C.tag=1;_tmp18C.f1=Cyc_add_cpparg;
_tmp18C;});_tmp18B;});_tmp189->f5=({const char*_tmp18A="Add to the list of directories to search for include files";
_tag_dyneither(_tmp18A,sizeof(char),59);});_tmp189;});_tmp16A[4]=({struct
_tuple15*_tmp183=_region_malloc(r,sizeof(*_tmp183));_tmp183->f1=({const char*
_tmp188="-B";_tag_dyneither(_tmp188,sizeof(char),3);});_tmp183->f2=1;_tmp183->f3=({
const char*_tmp187="<file>";_tag_dyneither(_tmp187,sizeof(char),7);});_tmp183->f4=(
void*)({struct Cyc_Arg_Flag_spec_struct*_tmp185=_region_malloc(r,sizeof(*_tmp185));
_tmp185[0]=({struct Cyc_Arg_Flag_spec_struct _tmp186;_tmp186.tag=1;_tmp186.f1=Cyc_add_cyclone_exec_path;
_tmp186;});_tmp185;});_tmp183->f5=({const char*_tmp184="Add to the list of directories to search for compiler files";
_tag_dyneither(_tmp184,sizeof(char),60);});_tmp183;});_tmp16A[3]=({struct
_tuple15*_tmp17D=_region_malloc(r,sizeof(*_tmp17D));_tmp17D->f1=({const char*
_tmp182="-D";_tag_dyneither(_tmp182,sizeof(char),3);});_tmp17D->f2=1;_tmp17D->f3=({
const char*_tmp181="<name>[=<value>]";_tag_dyneither(_tmp181,sizeof(char),17);});
_tmp17D->f4=(void*)({struct Cyc_Arg_Flag_spec_struct*_tmp17F=_region_malloc(r,
sizeof(*_tmp17F));_tmp17F[0]=({struct Cyc_Arg_Flag_spec_struct _tmp180;_tmp180.tag=
1;_tmp180.f1=Cyc_add_cpparg;_tmp180;});_tmp17F;});_tmp17D->f5=({const char*
_tmp17E="Pass definition to preprocessor";_tag_dyneither(_tmp17E,sizeof(char),32);});
_tmp17D;});_tmp16A[2]=({struct _tuple15*_tmp177=_region_malloc(r,sizeof(*_tmp177));
_tmp177->f1=({const char*_tmp17C="-o";_tag_dyneither(_tmp17C,sizeof(char),3);});
_tmp177->f2=0;_tmp177->f3=({const char*_tmp17B=" <file>";_tag_dyneither(_tmp17B,
sizeof(char),8);});_tmp177->f4=(void*)({struct Cyc_Arg_String_spec_struct*_tmp179=
_region_malloc(r,sizeof(*_tmp179));_tmp179[0]=({struct Cyc_Arg_String_spec_struct
_tmp17A;_tmp17A.tag=5;_tmp17A.f1=Cyc_set_output_file;_tmp17A;});_tmp179;});
_tmp177->f5=({const char*_tmp178="Set the output file name to <file>";
_tag_dyneither(_tmp178,sizeof(char),35);});_tmp177;});_tmp16A[1]=({struct
_tuple15*_tmp171=_region_malloc(r,sizeof(*_tmp171));_tmp171->f1=({const char*
_tmp176="--version";_tag_dyneither(_tmp176,sizeof(char),10);});_tmp171->f2=0;
_tmp171->f3=({const char*_tmp175="";_tag_dyneither(_tmp175,sizeof(char),1);});
_tmp171->f4=(void*)({struct Cyc_Arg_Unit_spec_struct*_tmp173=_region_malloc(r,
sizeof(*_tmp173));_tmp173[0]=({struct Cyc_Arg_Unit_spec_struct _tmp174;_tmp174.tag=
0;_tmp174.f1=Cyc_print_version;_tmp174;});_tmp173;});_tmp171->f5=({const char*
_tmp172="Print version information and exit";_tag_dyneither(_tmp172,sizeof(char),
35);});_tmp171;});_tmp16A[0]=({struct _tuple15*_tmp16B=_region_malloc(r,sizeof(*
_tmp16B));_tmp16B->f1=({const char*_tmp170="-v";_tag_dyneither(_tmp170,sizeof(
char),3);});_tmp16B->f2=0;_tmp16B->f3=({const char*_tmp16F="";_tag_dyneither(
_tmp16F,sizeof(char),1);});_tmp16B->f4=(void*)({struct Cyc_Arg_Set_spec_struct*
_tmp16D=_region_malloc(r,sizeof(*_tmp16D));_tmp16D[0]=({struct Cyc_Arg_Set_spec_struct
_tmp16E;_tmp16E.tag=3;_tmp16E.f1=& Cyc_v_r;_tmp16E;});_tmp16D;});_tmp16B->f5=({
const char*_tmp16C="Print compilation stages verbosely";_tag_dyneither(_tmp16C,
sizeof(char),35);});_tmp16B;});((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct _dyneither_ptr))Cyc_List_rlist)(r,_tag_dyneither(_tmp16A,sizeof(struct
_tuple15*),64));});struct _dyneither_ptr optstring=({const char*_tmp169="Options:";
_tag_dyneither(_tmp169,sizeof(char),9);});Cyc_Arg_current=0;Cyc_Arg_parse(
options,Cyc_add_other,optstring,_tmp14E);if(Cyc_target_arch.curr == (
_tag_dyneither(0,0,0)).curr)Cyc_target_arch=(struct _dyneither_ptr)
Cstring_to_string(Carch);{struct _dyneither_ptr cyclone_exec_prefix=({char*_tmp168=
getenv((const char*)"CYCLONE_EXEC_PREFIX");_tag_dyneither(_tmp168,sizeof(char),
_get_zero_arr_size_char(_tmp168,1));});if(cyclone_exec_prefix.curr != (
_tag_dyneither(0,0,0)).curr)Cyc_cyclone_exec_path=({struct Cyc_List_List*_tmp150=
_cycalloc(sizeof(*_tmp150));_tmp150->hd=({struct _dyneither_ptr*_tmp151=_cycalloc(
sizeof(*_tmp151));_tmp151[0]=cyclone_exec_prefix;_tmp151;});_tmp150->tl=Cyc_cyclone_exec_path;
_tmp150;});def_lib_path=(struct _dyneither_ptr)Cstring_to_string(Cdef_lib_path);
if(Cyc_strlen((struct _dyneither_ptr)def_lib_path)> 0)Cyc_cyclone_exec_path=({
struct Cyc_List_List*_tmp152=_cycalloc(sizeof(*_tmp152));_tmp152->hd=({struct
_dyneither_ptr*_tmp153=_cycalloc(sizeof(*_tmp153));_tmp153[0]=(struct
_dyneither_ptr)Cyc_Filename_concat(def_lib_path,({const char*_tmp154="cyc-lib";
_tag_dyneither(_tmp154,sizeof(char),8);}));_tmp153;});_tmp152->tl=Cyc_cyclone_exec_path;
_tmp152;});Cyc_cyclone_exec_path=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(Cyc_cyclone_exec_path);cyclone_arch_path=Cyc_also_subdir(Cyc_cyclone_exec_path,
Cyc_target_arch);{struct _dyneither_ptr _tmp155=Cyc_do_find(cyclone_arch_path,({
const char*_tmp167="cycspecs";_tag_dyneither(_tmp167,sizeof(char),9);}));if(Cyc_v_r)({
struct Cyc_String_pa_struct _tmp158;_tmp158.tag=0;_tmp158.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)_tmp155);{void*_tmp156[1]={& _tmp158};Cyc_fprintf(Cyc_stderr,({
const char*_tmp157="Reading from specs file %s\n";_tag_dyneither(_tmp157,sizeof(
char),28);}),_tag_dyneither(_tmp156,sizeof(void*),1));}});{struct Cyc_List_List*
_tmp159=Cyc_read_specs(_tmp155);struct _dyneither_ptr _tmp15A=_tag_dyneither(0,0,0);{
struct _handler_cons _tmp15B;_push_handler(& _tmp15B);{int _tmp15D=0;if(setjmp(
_tmp15B.handler))_tmp15D=1;if(!_tmp15D){{struct _dyneither_ptr _tmp15E=*((struct
_dyneither_ptr*(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct
Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_assoc_cmp)(Cyc_strptrcmp,
_tmp159,_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"cyclone",
sizeof(char),8));_tmp15A=Cyc_split_specs(_tmp15E);};_pop_handler();}else{void*
_tmp15C=(void*)_exn_thrown;void*_tmp161=_tmp15C;_LL36: if(_tmp161 != Cyc_Core_Not_found)
goto _LL38;_LL37: goto _LL35;_LL38:;_LL39:(void)_throw(_tmp161);_LL35:;}}}if(
_tmp15A.curr != (_tag_dyneither(0,0,0)).curr){Cyc_Arg_current=0;Cyc_Arg_parse(
options,Cyc_add_other,optstring,_tmp15A);}Cyc_Arg_current=0;Cyc_Arg_parse(
options,Cyc_add_other,optstring,_tmp14F);if(Cyc_strcmp((struct _dyneither_ptr)Cyc_cpp,({
const char*_tmp162="";_tag_dyneither(_tmp162,sizeof(char),1);}))== 0)Cyc_set_cpp((
struct _dyneither_ptr)({struct Cyc_String_pa_struct _tmp166;_tmp166.tag=0;_tmp166.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)_tmp155);{struct Cyc_String_pa_struct
_tmp165;_tmp165.tag=0;_tmp165.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
comp);{void*_tmp163[2]={& _tmp165,& _tmp166};Cyc_aprintf(({const char*_tmp164="%s -w -x c -E -specs %s";
_tag_dyneither(_tmp164,sizeof(char),24);}),_tag_dyneither(_tmp163,sizeof(void*),
2));}}}));}}}}}}if(Cyc_cyclone_files == 0  && Cyc_ccargs == 0){({void*_tmp2F7=0;Cyc_fprintf(
Cyc_stderr,({const char*_tmp2F8="missing file\n";_tag_dyneither(_tmp2F8,sizeof(
char),14);}),_tag_dyneither(_tmp2F7,sizeof(void*),0));});exit(1);}if(!Cyc_stop_after_cpp_r
 && !Cyc_nocyc_setjmp_r)Cyc_cyc_setjmp=Cyc_do_find(cyclone_arch_path,({const char*
_tmp2F9="cyc_setjmp.h";_tag_dyneither(_tmp2F9,sizeof(char),13);}));if(!Cyc_stop_after_cpp_r
 && Cyc_strlen((struct _dyneither_ptr)Cyc_cyc_include)== 0)Cyc_cyc_include=Cyc_do_find(
Cyc_cyclone_exec_path,({const char*_tmp2FA="cyc_include.h";_tag_dyneither(_tmp2FA,
sizeof(char),14);}));{struct Cyc_List_List*_tmp2FB=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(Cyc_cyclone_files);for(0;_tmp2FB != 0;
_tmp2FB=_tmp2FB->tl){Cyc_process_file(*((struct _dyneither_ptr*)_tmp2FB->hd));if(
Cyc_compile_failure){int _tmp2FC=1;_npop_handler(0);return _tmp2FC;}}}if(((Cyc_stop_after_cpp_r
 || Cyc_parseonly_r) || Cyc_tc_r) || Cyc_toc_r){int _tmp2FD=0;_npop_handler(0);
return _tmp2FD;}if(Cyc_ccargs == 0){int _tmp2FE=0;_npop_handler(0);return _tmp2FE;}
Cyc_add_ccarg((struct _dyneither_ptr)Cyc_strconcat(({const char*_tmp2FF="-L";
_tag_dyneither(_tmp2FF,sizeof(char),3);}),(struct _dyneither_ptr)def_lib_path));
Cyc_ccargs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_ccargs);{
struct _dyneither_ptr _tmp300=Cyc_str_sepstr(((struct Cyc_List_List*(*)(struct
_dyneither_ptr*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_sh_escape_stringptr,Cyc_ccargs),({const char*_tmp35B=" ";_tag_dyneither(
_tmp35B,sizeof(char),2);}));Cyc_libargs=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_rev)(Cyc_libargs);{struct _dyneither_ptr _tmp301=Cyc_str_sepstr(({
struct Cyc_List_List*_tmp358=_cycalloc(sizeof(*_tmp358));_tmp358->hd=
_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"",sizeof(char),1);
_tmp358->tl=((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct
_dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_sh_escape_stringptr,
Cyc_libargs);_tmp358;}),({const char*_tmp35A=" ";_tag_dyneither(_tmp35A,sizeof(
char),2);}));struct Cyc_List_List*stdlib;struct _dyneither_ptr stdlib_string;int
_tmp302=((Cyc_stop_after_asmfile_r  || Cyc_stop_after_objectfile_r) || Cyc_output_file
!= 0  && Cyc_Filename_check_suffix(*((struct _dyneither_ptr*)_check_null(Cyc_output_file)),({
const char*_tmp356=".a";_tag_dyneither(_tmp356,sizeof(char),3);}))) || Cyc_output_file
!= 0  && Cyc_Filename_check_suffix(*((struct _dyneither_ptr*)_check_null(Cyc_output_file)),({
const char*_tmp357=".lib";_tag_dyneither(_tmp357,sizeof(char),5);}));if(_tmp302){
stdlib=0;stdlib_string=(struct _dyneither_ptr)({const char*_tmp303="";
_tag_dyneither(_tmp303,sizeof(char),1);});}else{struct _dyneither_ptr libcyc_flag;
struct _dyneither_ptr nogc_filename;struct _dyneither_ptr runtime_filename;struct
_dyneither_ptr gc_filename;if(Cyc_pa_r){libcyc_flag=({const char*_tmp304="-lcyc_a";
_tag_dyneither(_tmp304,sizeof(char),8);});nogc_filename=({const char*_tmp305="nogc_a.a";
_tag_dyneither(_tmp305,sizeof(char),9);});runtime_filename=({const char*_tmp306="runtime_cyc_a.o";
_tag_dyneither(_tmp306,sizeof(char),16);});}else{if(Cyc_nocheck_r){libcyc_flag=({
const char*_tmp307="-lcyc_nocheck";_tag_dyneither(_tmp307,sizeof(char),14);});
nogc_filename=({const char*_tmp308="nogc_nocheck.a";_tag_dyneither(_tmp308,
sizeof(char),15);});runtime_filename=({const char*_tmp309="runtime_cyc.o";
_tag_dyneither(_tmp309,sizeof(char),14);});}else{if(Cyc_pg_r){libcyc_flag=({
const char*_tmp30A="-lcyc_pg";_tag_dyneither(_tmp30A,sizeof(char),9);});
runtime_filename=({const char*_tmp30B="runtime_cyc_pg.o";_tag_dyneither(_tmp30B,
sizeof(char),17);});nogc_filename=({const char*_tmp30C="nogc_pg.a";_tag_dyneither(
_tmp30C,sizeof(char),10);});}else{libcyc_flag=({const char*_tmp30D="-lcyc";
_tag_dyneither(_tmp30D,sizeof(char),6);});nogc_filename=({const char*_tmp30E="nogc.a";
_tag_dyneither(_tmp30E,sizeof(char),7);});runtime_filename=({const char*_tmp30F="runtime_cyc.o";
_tag_dyneither(_tmp30F,sizeof(char),14);});}}}gc_filename=({const char*_tmp310="gc.a";
_tag_dyneither(_tmp310,sizeof(char),5);});{struct _dyneither_ptr _tmp311=Cyc_nogc_r?
Cyc_do_find(cyclone_arch_path,nogc_filename): Cyc_do_find(cyclone_arch_path,
gc_filename);struct _dyneither_ptr _tmp312=Cyc_do_find(cyclone_arch_path,
runtime_filename);stdlib=0;stdlib_string=(struct _dyneither_ptr)({struct Cyc_String_pa_struct
_tmp317;_tmp317.tag=0;_tmp317.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
_tmp311);{struct Cyc_String_pa_struct _tmp316;_tmp316.tag=0;_tmp316.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)libcyc_flag);{struct Cyc_String_pa_struct
_tmp315;_tmp315.tag=0;_tmp315.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
_tmp312);{void*_tmp313[3]={& _tmp315,& _tmp316,& _tmp317};Cyc_aprintf(({const char*
_tmp314=" %s %s %s";_tag_dyneither(_tmp314,sizeof(char),10);}),_tag_dyneither(
_tmp313,sizeof(void*),3));}}}});}}if(Cyc_ic_r){struct _handler_cons _tmp318;
_push_handler(& _tmp318);{int _tmp31A=0;if(setjmp(_tmp318.handler))_tmp31A=1;if(!
_tmp31A){Cyc_ccargs=((struct Cyc_List_List*(*)(int(*f)(struct _dyneither_ptr*),
struct Cyc_List_List*x))Cyc_List_filter)(Cyc_is_cfile,Cyc_ccargs);Cyc_libargs=((
struct Cyc_List_List*(*)(int(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))
Cyc_List_filter)(Cyc_is_cfile,Cyc_libargs);{struct Cyc_List_List*_tmp31B=((struct
Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(
stdlib,((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_append)(Cyc_ccargs,Cyc_libargs));if(!_tmp302)_tmp31B=({struct Cyc_List_List*
_tmp31C=_cycalloc(sizeof(*_tmp31C));_tmp31C->hd=Cyc_final_strptr;_tmp31C->tl=
_tmp31B;_tmp31C;});{struct Cyc_Interface_I*_tmp31D=((struct Cyc_Interface_I*(*)(
struct Cyc_Interface_I*(*get)(struct _dyneither_ptr*),struct Cyc_List_List*la,
struct Cyc_List_List*linfo))Cyc_Interface_get_and_merge_list)(Cyc_read_cycio,
_tmp31B,_tmp31B);if(_tmp31D == 0){({void*_tmp31E=0;Cyc_fprintf(Cyc_stderr,({const
char*_tmp31F="Error: interfaces incompatible\n";_tag_dyneither(_tmp31F,sizeof(
char),32);}),_tag_dyneither(_tmp31E,sizeof(void*),0));});Cyc_compile_failure=1;
Cyc_remove_cfiles();{int _tmp320=1;_npop_handler(1);return _tmp320;}}if(_tmp302){
if(Cyc_output_file != 0){struct _dyneither_ptr _tmp321=({struct Cyc_String_pa_struct
_tmp328;_tmp328.tag=0;_tmp328.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Filename_chop_extension(*((struct _dyneither_ptr*)_check_null(Cyc_output_file))));{
void*_tmp326[1]={& _tmp328};Cyc_aprintf(({const char*_tmp327="%s.cycio";
_tag_dyneither(_tmp327,sizeof(char),9);}),_tag_dyneither(_tmp326,sizeof(void*),1));}});
struct Cyc___cycFILE*_tmp322=Cyc_try_file_open((struct _dyneither_ptr)_tmp321,({
const char*_tmp324="wb";_tag_dyneither(_tmp324,sizeof(char),3);}),({const char*
_tmp325="interface object file";_tag_dyneither(_tmp325,sizeof(char),22);}));if(
_tmp322 == 0){Cyc_compile_failure=1;Cyc_remove_cfiles();{int _tmp323=1;
_npop_handler(1);return _tmp323;}}Cyc_Interface_save((struct Cyc_Interface_I*)
_tmp31D,(struct Cyc___cycFILE*)_tmp322);Cyc_file_close((struct Cyc___cycFILE*)
_tmp322);}}else{if(!Cyc_Interface_is_subinterface(Cyc_Interface_empty(),(struct
Cyc_Interface_I*)_tmp31D,({struct _tuple11*_tmp329=_cycalloc(sizeof(*_tmp329));
_tmp329->f1=({const char*_tmp32B="empty interface";_tag_dyneither(_tmp32B,sizeof(
char),16);});_tmp329->f2=({const char*_tmp32A="global interface";_tag_dyneither(
_tmp32A,sizeof(char),17);});_tmp329;}))){({void*_tmp32C=0;Cyc_fprintf(Cyc_stderr,({
const char*_tmp32D="Error: some objects are still undefined\n";_tag_dyneither(
_tmp32D,sizeof(char),41);}),_tag_dyneither(_tmp32C,sizeof(void*),0));});Cyc_compile_failure=
1;Cyc_remove_cfiles();{int _tmp32E=1;_npop_handler(1);return _tmp32E;}}}}};
_pop_handler();}else{void*_tmp319=(void*)_exn_thrown;void*_tmp330=_tmp319;_LL3B:;
_LL3C:{void*_tmp331=_tmp330;struct _dyneither_ptr _tmp332;struct _dyneither_ptr
_tmp333;struct _dyneither_ptr _tmp334;_LL40: if(*((void**)_tmp331)!= Cyc_Core_Failure)
goto _LL42;_tmp332=((struct Cyc_Core_Failure_struct*)_tmp331)->f1;_LL41:({struct
Cyc_String_pa_struct _tmp337;_tmp337.tag=0;_tmp337.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)_tmp332);{void*_tmp335[1]={& _tmp337};Cyc_fprintf(Cyc_stderr,({
const char*_tmp336="Exception Core::Failure %s\n";_tag_dyneither(_tmp336,sizeof(
char),28);}),_tag_dyneither(_tmp335,sizeof(void*),1));}});goto _LL3F;_LL42: if(*((
void**)_tmp331)!= Cyc_Core_Impossible)goto _LL44;_tmp333=((struct Cyc_Core_Impossible_struct*)
_tmp331)->f1;_LL43:({struct Cyc_String_pa_struct _tmp33A;_tmp33A.tag=0;_tmp33A.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)_tmp333);{void*_tmp338[1]={& _tmp33A};
Cyc_fprintf(Cyc_stderr,({const char*_tmp339="Exception Core::Impossible %s\n";
_tag_dyneither(_tmp339,sizeof(char),31);}),_tag_dyneither(_tmp338,sizeof(void*),
1));}});goto _LL3F;_LL44: if(_tmp331 != Cyc_Dict_Absent)goto _LL46;_LL45:({void*
_tmp33B=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp33C="Exception Dict::Absent\n";
_tag_dyneither(_tmp33C,sizeof(char),24);}),_tag_dyneither(_tmp33B,sizeof(void*),
0));});goto _LL3F;_LL46: if(*((void**)_tmp331)!= Cyc_Core_Invalid_argument)goto
_LL48;_tmp334=((struct Cyc_Core_Invalid_argument_struct*)_tmp331)->f1;_LL47:({
struct Cyc_String_pa_struct _tmp33F;_tmp33F.tag=0;_tmp33F.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)_tmp334);{void*_tmp33D[1]={& _tmp33F};Cyc_fprintf(Cyc_stderr,({
const char*_tmp33E="Exception Core::Invalid_argument %s\n";_tag_dyneither(_tmp33E,
sizeof(char),37);}),_tag_dyneither(_tmp33D,sizeof(void*),1));}});goto _LL3F;_LL48:;
_LL49:({void*_tmp340=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp341="Uncaught exception\n";
_tag_dyneither(_tmp341,sizeof(char),20);}),_tag_dyneither(_tmp340,sizeof(void*),
0));});goto _LL3F;_LL3F:;}Cyc_compile_failure=1;Cyc_remove_cfiles();{int _tmp342=1;
_npop_handler(0);return _tmp342;}_LL3D:;_LL3E:(void)_throw(_tmp330);_LL3A:;}}}{
struct _dyneither_ptr outfile_str=({const char*_tmp355="";_tag_dyneither(_tmp355,
sizeof(char),1);});if(Cyc_output_file != 0)outfile_str=(struct _dyneither_ptr)({
struct Cyc_String_pa_struct _tmp345;_tmp345.tag=0;_tmp345.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_sh_escape_string(*((struct _dyneither_ptr*)_check_null(
Cyc_output_file))));{void*_tmp343[1]={& _tmp345};Cyc_aprintf(({const char*_tmp344=" -o %s";
_tag_dyneither(_tmp344,sizeof(char),7);}),_tag_dyneither(_tmp343,sizeof(void*),1));}});{
struct _dyneither_ptr _tmp346=({struct Cyc_String_pa_struct _tmp354;_tmp354.tag=0;
_tmp354.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp301);{struct Cyc_String_pa_struct
_tmp353;_tmp353.tag=0;_tmp353.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
stdlib_string);{struct Cyc_String_pa_struct _tmp352;_tmp352.tag=0;_tmp352.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)_tmp300);{struct Cyc_String_pa_struct
_tmp351;_tmp351.tag=0;_tmp351.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
outfile_str);{struct Cyc_String_pa_struct _tmp350;_tmp350.tag=0;_tmp350.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)comp);{void*_tmp34E[5]={& _tmp350,& _tmp351,&
_tmp352,& _tmp353,& _tmp354};Cyc_aprintf(({const char*_tmp34F="%s %s %s%s%s";
_tag_dyneither(_tmp34F,sizeof(char),13);}),_tag_dyneither(_tmp34E,sizeof(void*),
5));}}}}}});if(Cyc_v_r){({struct Cyc_String_pa_struct _tmp349;_tmp349.tag=0;
_tmp349.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp346);{void*_tmp347[1]={&
_tmp349};Cyc_fprintf(Cyc_stderr,({const char*_tmp348="%s\n";_tag_dyneither(
_tmp348,sizeof(char),4);}),_tag_dyneither(_tmp347,sizeof(void*),1));}});Cyc_fflush((
struct Cyc___cycFILE*)Cyc_stderr);}if(system((const char*)_untag_dyneither_ptr(
_tmp346,sizeof(char),1))!= 0){({void*_tmp34A=0;Cyc_fprintf(Cyc_stderr,({const
char*_tmp34B="Error: C compiler failed\n";_tag_dyneither(_tmp34B,sizeof(char),26);}),
_tag_dyneither(_tmp34A,sizeof(void*),0));});Cyc_compile_failure=1;Cyc_remove_cfiles();{
int _tmp34C=1;_npop_handler(0);return _tmp34C;}}Cyc_remove_cfiles();{int _tmp34D=
Cyc_compile_failure?1: 0;_npop_handler(0);return _tmp34D;}}}}};_pop_region(r);}}
