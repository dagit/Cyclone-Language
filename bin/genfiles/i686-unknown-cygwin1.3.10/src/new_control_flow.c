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
void*v;};struct _tuple0{void*f1;void*f2;};void*Cyc_Core_snd(struct _tuple0*);int
Cyc_Core_ptrcmp(void*,void*);extern char Cyc_Core_Invalid_argument[21];struct Cyc_Core_Invalid_argument_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[12];struct Cyc_Core_Failure_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[15];struct Cyc_Core_Impossible_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[14];extern char Cyc_Core_Unreachable[
16];struct Cyc_Core_Unreachable_struct{char*tag;struct _dyneither_ptr f1;};extern
struct _RegionHandle*Cyc_Core_heap_region;extern char Cyc_Core_Open_Region[16];
extern char Cyc_Core_Free_Region[16];struct Cyc_List_List{void*hd;struct Cyc_List_List*
tl;};struct Cyc_List_List*Cyc_List_rlist(struct _RegionHandle*,struct
_dyneither_ptr);int Cyc_List_length(struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_rcopy(struct _RegionHandle*,struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_rmap(struct _RegionHandle*,void*(*f)(void*),struct Cyc_List_List*x);
extern char Cyc_List_List_mismatch[18];void Cyc_List_iter(void(*f)(void*),struct Cyc_List_List*
x);struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_rappend(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*
y);struct Cyc_List_List*Cyc_List_rmerge(struct _RegionHandle*,int(*cmp)(void*,void*),
struct Cyc_List_List*a,struct Cyc_List_List*b);extern char Cyc_List_Nth[8];void*Cyc_List_nth(
struct Cyc_List_List*x,int n);struct Cyc_List_List*Cyc_List_rzip(struct
_RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y);int Cyc_List_memq(struct Cyc_List_List*l,void*x);struct Cyc_Iter_Iter{void*env;
int(*next)(void*env,void*dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);
struct Cyc_Set_Set;extern char Cyc_Set_Absent[11];struct Cyc___cycFILE;extern struct
Cyc___cycFILE*Cyc_stderr;struct Cyc_Cstdio___abstractFILE;struct Cyc_String_pa_struct{
int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_struct{int tag;unsigned long f1;}
;struct Cyc_Double_pa_struct{int tag;double f1;};struct Cyc_LongDouble_pa_struct{int
tag;long double f1;};struct Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{
int tag;unsigned long*f1;};struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr,
struct _dyneither_ptr);int Cyc_fprintf(struct Cyc___cycFILE*,struct _dyneither_ptr,
struct _dyneither_ptr);struct Cyc_ShortPtr_sa_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_struct{
int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{
int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_struct{int tag;struct
_dyneither_ptr f1;};struct Cyc_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_struct{
int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int tag;struct _dyneither_ptr f1;};
extern char Cyc_FileCloseError[19];extern char Cyc_FileOpenError[18];struct Cyc_FileOpenError_struct{
char*tag;struct _dyneither_ptr f1;};struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(
void*,void*);struct _RegionHandle*r;struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[
12];extern char Cyc_Dict_Absent[11];int Cyc_Dict_member(struct Cyc_Dict_Dict d,void*k);
struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict d,void*k,void*v);void*Cyc_Dict_lookup(
struct Cyc_Dict_Dict d,void*k);int Cyc_Dict_lookup_bool(struct Cyc_Dict_Dict d,void*k,
void**ans);struct _tuple0*Cyc_Dict_rchoose(struct _RegionHandle*r,struct Cyc_Dict_Dict
d);struct _tuple0*Cyc_Dict_rchoose(struct _RegionHandle*,struct Cyc_Dict_Dict d);
struct Cyc_Dict_Dict Cyc_Dict_rdelete(struct _RegionHandle*,struct Cyc_Dict_Dict,
void*);struct Cyc_Iter_Iter Cyc_Dict_make_iter(struct _RegionHandle*rgn,struct Cyc_Dict_Dict
d);int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct _dyneither_ptr*s2);struct Cyc_Hashtable_Table;
struct Cyc_Hashtable_Table*Cyc_Hashtable_rcreate(struct _RegionHandle*r,int sz,int(*
cmp)(void*,void*),int(*hash)(void*));void Cyc_Hashtable_insert(struct Cyc_Hashtable_Table*
t,void*key,void*val);void**Cyc_Hashtable_lookup_opt(struct Cyc_Hashtable_Table*t,
void*key);struct Cyc_Lineno_Pos{struct _dyneither_ptr logical_file;struct
_dyneither_ptr line;int line_no;int col;};extern char Cyc_Position_Exit[9];struct Cyc_Position_Segment;
struct Cyc_Position_Error{struct _dyneither_ptr source;struct Cyc_Position_Segment*
seg;void*kind;struct _dyneither_ptr desc;};extern char Cyc_Position_Nocontext[14];
int Cyc_Position_error_p();struct Cyc_Absyn_Loc_n_struct{int tag;};struct Cyc_Absyn_Rel_n_struct{
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
f1;};struct Cyc_Absyn_Aggr_d_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct
Cyc_Absyn_Tunion_d_struct{int tag;struct Cyc_Absyn_Tuniondecl*f1;};struct Cyc_Absyn_Enum_d_struct{
int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_struct{int tag;
struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_struct{int tag;
struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_struct{
int tag;struct _tuple1*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_struct{int tag;
struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Decl{void*r;
struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_ArrayElement_struct{int tag;
struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_struct{int tag;struct
_dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];void*Cyc_Absyn_conref_val(
struct Cyc_Absyn_Conref*x);void*Cyc_Absyn_conref_def(void*,struct Cyc_Absyn_Conref*
x);struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfoU_union
info);int Cyc_Absyn_is_union_type(void*);int Cyc_Absyn_is_aggr_type(void*t);struct
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
le;int allow_valueof;};void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap);void Cyc_Tcutil_terr(struct Cyc_Position_Segment*,struct
_dyneither_ptr fmt,struct _dyneither_ptr ap);void Cyc_Tcutil_warn(struct Cyc_Position_Segment*,
struct _dyneither_ptr fmt,struct _dyneither_ptr ap);void*Cyc_Tcutil_compress(void*t);
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,void*);
void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*);int Cyc_Tcutil_is_bound_one(
struct Cyc_Absyn_Conref*b);int Cyc_Tcutil_is_noalias_pointer(void*t);int Cyc_Tcutil_is_noalias_path(
struct Cyc_Absyn_Exp*e);int Cyc_Tcutil_is_noalias_pointer_or_aggr(void*t);int Cyc_Tcutil_is_noreturn(
void*);struct _tuple4{unsigned int f1;int f2;};struct _tuple4 Cyc_Evexp_eval_const_uint_exp(
struct Cyc_Absyn_Exp*e);struct Cyc_CfFlowInfo_VarRoot_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_CfFlowInfo_MallocPt_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;
};struct Cyc_CfFlowInfo_InitParam_struct{int tag;int f1;void*f2;};struct Cyc_CfFlowInfo_Place{
void*root;struct Cyc_List_List*fields;};struct Cyc_CfFlowInfo_EqualConst_struct{
int tag;unsigned int f1;};struct Cyc_CfFlowInfo_LessVar_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_CfFlowInfo_LessNumelts_struct{int tag;struct Cyc_Absyn_Vardecl*f1;}
;struct Cyc_CfFlowInfo_LessConst_struct{int tag;unsigned int f1;};struct Cyc_CfFlowInfo_LessEqNumelts_struct{
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
Cyc_CfFlowInfo_Aggregate_struct{int tag;struct _dyneither_ptr f1;};int Cyc_CfFlowInfo_update_place_set(
struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*place,struct Cyc_Position_Segment*
loc);struct Cyc_Dict_Dict Cyc_CfFlowInfo_union_place_set(struct Cyc_Dict_Dict s1,
struct Cyc_Dict_Dict s2,int disjoint);struct Cyc_CfFlowInfo_ConsumeInfo{struct Cyc_Dict_Dict
consumed;struct Cyc_List_List*may_consume;};struct Cyc_CfFlowInfo_BottomFL_struct{
int tag;};struct Cyc_CfFlowInfo_ReachableFL_struct{int tag;struct Cyc_Dict_Dict f1;
struct Cyc_List_List*f2;struct Cyc_CfFlowInfo_ConsumeInfo f3;};union Cyc_CfFlowInfo_FlowInfo_union{
struct Cyc_CfFlowInfo_BottomFL_struct BottomFL;struct Cyc_CfFlowInfo_ReachableFL_struct
ReachableFL;};struct Cyc_CfFlowInfo_FlowEnv{struct _RegionHandle*r;void*
unknown_none;void*unknown_this;void*unknown_all;void*esc_none;void*esc_this;void*
esc_all;struct Cyc_Dict_Dict mt_flowdict;struct Cyc_Dict_Dict mt_place_set;struct Cyc_CfFlowInfo_Place*
dummy_place;};struct Cyc_CfFlowInfo_FlowEnv*Cyc_CfFlowInfo_new_flow_env(struct
_RegionHandle*r);int Cyc_CfFlowInfo_place_cmp(struct Cyc_CfFlowInfo_Place*,struct
Cyc_CfFlowInfo_Place*);struct _dyneither_ptr Cyc_CfFlowInfo_aggrfields_to_aggrdict(
struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_List_List*,void*);void*Cyc_CfFlowInfo_typ_to_absrval(
struct Cyc_CfFlowInfo_FlowEnv*,void*t,void*leafval);void*Cyc_CfFlowInfo_initlevel(
struct Cyc_Dict_Dict d,void*r);void*Cyc_CfFlowInfo_lookup_place(struct Cyc_Dict_Dict
d,struct Cyc_CfFlowInfo_Place*place);int Cyc_CfFlowInfo_flow_lessthan_approx(union
Cyc_CfFlowInfo_FlowInfo_union f1,union Cyc_CfFlowInfo_FlowInfo_union f2);struct Cyc_List_List*
Cyc_CfFlowInfo_reln_assign_var(struct _RegionHandle*,struct Cyc_List_List*,struct
Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Exp*);struct Cyc_List_List*Cyc_CfFlowInfo_reln_assign_exp(
struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*);
struct Cyc_List_List*Cyc_CfFlowInfo_reln_kill_exp(struct _RegionHandle*,struct Cyc_List_List*,
struct Cyc_Absyn_Exp*);struct Cyc_List_List*Cyc_CfFlowInfo_copy_relns(struct
_RegionHandle*,struct Cyc_List_List*);int Cyc_CfFlowInfo_same_relns(struct Cyc_List_List*,
struct Cyc_List_List*);struct Cyc_Dict_Dict Cyc_CfFlowInfo_escape_deref(struct Cyc_CfFlowInfo_FlowEnv*
fenv,struct Cyc_Dict_Dict d,struct Cyc_Dict_Dict*all_changed,void*r);struct Cyc_Dict_Dict
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
struct Cyc_CfFlowInfo_Region_k_struct{int tag;struct Cyc_Absyn_Tvar*f1;};union Cyc_CfFlowInfo_FlowInfo_union
Cyc_CfFlowInfo_consume_unique_rvals(struct Cyc_Position_Segment*loc,union Cyc_CfFlowInfo_FlowInfo_union
f);void Cyc_CfFlowInfo_check_unique_rvals(struct Cyc_Position_Segment*loc,union Cyc_CfFlowInfo_FlowInfo_union
f);union Cyc_CfFlowInfo_FlowInfo_union Cyc_CfFlowInfo_readthrough_unique_rvals(
struct Cyc_Position_Segment*loc,union Cyc_CfFlowInfo_FlowInfo_union f);union Cyc_CfFlowInfo_FlowInfo_union
Cyc_CfFlowInfo_drop_unique_rvals(struct Cyc_Position_Segment*loc,union Cyc_CfFlowInfo_FlowInfo_union
f);struct _tuple6{struct Cyc_CfFlowInfo_ConsumeInfo f1;union Cyc_CfFlowInfo_FlowInfo_union
f2;};struct _tuple6 Cyc_CfFlowInfo_save_consume_info(struct Cyc_CfFlowInfo_FlowEnv*,
union Cyc_CfFlowInfo_FlowInfo_union f,int clear);union Cyc_CfFlowInfo_FlowInfo_union
Cyc_CfFlowInfo_restore_consume_info(union Cyc_CfFlowInfo_FlowInfo_union f,struct
Cyc_CfFlowInfo_ConsumeInfo c);struct _dyneither_ptr Cyc_CfFlowInfo_place_err_string(
struct Cyc_CfFlowInfo_Place*place);void Cyc_NewControlFlow_set_encloser(struct Cyc_Absyn_Stmt*
enclosee,struct Cyc_Absyn_Stmt*encloser);void Cyc_NewControlFlow_cf_check(struct
Cyc_List_List*ds);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct
Cyc_Absynpp_Params{int expand_typedefs: 1;int qvar_to_Cids: 1;int add_cyc_prefix: 1;
int to_VC: 1;int decls_first: 1;int rewrite_temp_tvars: 1;int print_all_tvars: 1;int
print_all_kinds: 1;int print_all_effects: 1;int print_using_stmts: 1;int
print_externC_stmts: 1;int print_full_evars: 1;int print_zeroterm: 1;int
generate_line_directives: 1;int use_curr_namespace: 1;struct Cyc_List_List*
curr_namespace;};struct _dyneither_ptr Cyc_Absynpp_typ2string(void*);struct
_dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);struct _dyneither_ptr
Cyc_Absynpp_qvar2string(struct _tuple1*);struct Cyc_NewControlFlow_CFStmtAnnot{
struct Cyc_Absyn_Stmt*encloser;int visited;};static char Cyc_NewControlFlow_CFAnnot[
12]="\000\000\000\000CFAnnot\000";struct Cyc_NewControlFlow_CFAnnot_struct{char*
tag;struct Cyc_NewControlFlow_CFStmtAnnot f1;};void Cyc_NewControlFlow_set_encloser(
struct Cyc_Absyn_Stmt*enclosee,struct Cyc_Absyn_Stmt*encloser){(void*)(enclosee->annot=(
void*)((void*)({struct Cyc_NewControlFlow_CFAnnot_struct*_tmp0=_cycalloc(sizeof(*
_tmp0));_tmp0[0]=({struct Cyc_NewControlFlow_CFAnnot_struct _tmp1;_tmp1.tag=Cyc_NewControlFlow_CFAnnot;
_tmp1.f1=({struct Cyc_NewControlFlow_CFStmtAnnot _tmp2;_tmp2.encloser=encloser;
_tmp2.visited=0;_tmp2;});_tmp1;});_tmp0;})));}struct Cyc_NewControlFlow_AnalEnv{
struct _RegionHandle*r;struct Cyc_CfFlowInfo_FlowEnv*fenv;int iterate_again;int
iteration_num;int in_try;union Cyc_CfFlowInfo_FlowInfo_union tryflow;struct Cyc_Dict_Dict*
all_changed;int noreturn;struct Cyc_List_List*param_roots;struct Cyc_Hashtable_Table*
flow_table;};static union Cyc_CfFlowInfo_FlowInfo_union Cyc_NewControlFlow_anal_stmt(
struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo_union,struct Cyc_Absyn_Stmt*);
static union Cyc_CfFlowInfo_FlowInfo_union Cyc_NewControlFlow_anal_decl(struct Cyc_NewControlFlow_AnalEnv*,
union Cyc_CfFlowInfo_FlowInfo_union,struct Cyc_Absyn_Decl*);struct _tuple7{union Cyc_CfFlowInfo_FlowInfo_union
f1;union Cyc_CfFlowInfo_AbsLVal_union f2;};static struct _tuple7 Cyc_NewControlFlow_anal_Lexp(
struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo_union,struct Cyc_Absyn_Exp*);
static struct _tuple5 Cyc_NewControlFlow_anal_Rexp(struct Cyc_NewControlFlow_AnalEnv*,
union Cyc_CfFlowInfo_FlowInfo_union,struct Cyc_Absyn_Exp*);struct _tuple8{union Cyc_CfFlowInfo_FlowInfo_union
f1;union Cyc_CfFlowInfo_FlowInfo_union f2;};static struct _tuple8 Cyc_NewControlFlow_anal_test(
struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo_union,struct Cyc_Absyn_Exp*);
static struct Cyc_List_List*Cyc_NewControlFlow_noalias_ptrs_rec(struct Cyc_NewControlFlow_AnalEnv*
env,struct Cyc_CfFlowInfo_Place*p,void*t);static struct Cyc_NewControlFlow_CFStmtAnnot*
Cyc_NewControlFlow_get_stmt_annot(struct Cyc_Absyn_Stmt*s){void*_tmp3=(void*)s->annot;
struct Cyc_NewControlFlow_CFStmtAnnot _tmp4;struct Cyc_NewControlFlow_CFStmtAnnot*
_tmp5;_LL1: if(*((void**)_tmp3)!= Cyc_NewControlFlow_CFAnnot)goto _LL3;_tmp4=((
struct Cyc_NewControlFlow_CFAnnot_struct*)_tmp3)->f1;_tmp5=(struct Cyc_NewControlFlow_CFStmtAnnot*)&((
struct Cyc_NewControlFlow_CFAnnot_struct*)_tmp3)->f1;_LL2: return _tmp5;_LL3:;_LL4:(
int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp6=_cycalloc(sizeof(*
_tmp6));_tmp6[0]=({struct Cyc_Core_Impossible_struct _tmp7;_tmp7.tag=Cyc_Core_Impossible;
_tmp7.f1=({const char*_tmp8="ControlFlow -- wrong stmt annotation";_tag_dyneither(
_tmp8,sizeof(char),37);});_tmp7;});_tmp6;}));_LL0:;}static union Cyc_CfFlowInfo_FlowInfo_union*
Cyc_NewControlFlow_get_stmt_flow(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Absyn_Stmt*
s){union Cyc_CfFlowInfo_FlowInfo_union**sflow=((union Cyc_CfFlowInfo_FlowInfo_union**(*)(
struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key))Cyc_Hashtable_lookup_opt)(
env->flow_table,s);if(sflow == 0){union Cyc_CfFlowInfo_FlowInfo_union*res=({union
Cyc_CfFlowInfo_FlowInfo_union*_tmp9=_region_malloc(env->r,sizeof(*_tmp9));_tmp9[
0]=(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union _tmpA;(
_tmpA.BottomFL).tag=0;_tmpA;});_tmp9;});((void(*)(struct Cyc_Hashtable_Table*t,
struct Cyc_Absyn_Stmt*key,union Cyc_CfFlowInfo_FlowInfo_union*val))Cyc_Hashtable_insert)(
env->flow_table,s,res);return res;}return*sflow;}struct _tuple9{struct Cyc_NewControlFlow_CFStmtAnnot*
f1;union Cyc_CfFlowInfo_FlowInfo_union*f2;};static struct _tuple9 Cyc_NewControlFlow_pre_stmt_check(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union inflow,
struct Cyc_Absyn_Stmt*s){struct Cyc_NewControlFlow_CFStmtAnnot*_tmpB=Cyc_NewControlFlow_get_stmt_annot(
s);union Cyc_CfFlowInfo_FlowInfo_union*_tmpC=Cyc_NewControlFlow_get_stmt_flow(env,
s);*_tmpC=Cyc_CfFlowInfo_join_flow(env->fenv,env->all_changed,inflow,*_tmpC,1);
_tmpB->visited=env->iteration_num;return({struct _tuple9 _tmpD;_tmpD.f1=_tmpB;
_tmpD.f2=_tmpC;_tmpD;});}static int Cyc_NewControlFlow_get_field_index_fs(struct
Cyc_List_List*fs,struct _dyneither_ptr*f){int n=0;for(0;(unsigned int)fs;fs=fs->tl){
struct _dyneither_ptr*_tmpE=((struct Cyc_Absyn_Aggrfield*)fs->hd)->name;if(Cyc_strptrcmp(
_tmpE,f)== 0)return n;n=n + 1;}(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmpF=_cycalloc(sizeof(*_tmpF));_tmpF[0]=({struct Cyc_Core_Impossible_struct
_tmp10;_tmp10.tag=Cyc_Core_Impossible;_tmp10.f1=(struct _dyneither_ptr)({struct
Cyc_String_pa_struct _tmp13;_tmp13.tag=0;_tmp13.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*f);{void*_tmp11[1]={& _tmp13};Cyc_aprintf(({const char*_tmp12="get_field_index_fs failed: %s";
_tag_dyneither(_tmp12,sizeof(char),30);}),_tag_dyneither(_tmp11,sizeof(void*),1));}});
_tmp10;});_tmpF;}));}static int Cyc_NewControlFlow_get_field_index(void*t,struct
_dyneither_ptr*f){void*_tmp14=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo
_tmp15;union Cyc_Absyn_AggrInfoU_union _tmp16;struct Cyc_List_List*_tmp17;_LL6: if(
_tmp14 <= (void*)4)goto _LLA;if(*((int*)_tmp14)!= 10)goto _LL8;_tmp15=((struct Cyc_Absyn_AggrType_struct*)
_tmp14)->f1;_tmp16=_tmp15.aggr_info;_LL7: {struct Cyc_List_List*_tmp18=((struct
Cyc_Absyn_AggrdeclImpl*)_check_null((Cyc_Absyn_get_known_aggrdecl(_tmp16))->impl))->fields;
_tmp17=_tmp18;goto _LL9;}_LL8: if(*((int*)_tmp14)!= 11)goto _LLA;_tmp17=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp14)->f2;_LL9: return Cyc_NewControlFlow_get_field_index_fs(_tmp17,f);_LLA:;
_LLB:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp19=_cycalloc(
sizeof(*_tmp19));_tmp19[0]=({struct Cyc_Core_Impossible_struct _tmp1A;_tmp1A.tag=
Cyc_Core_Impossible;_tmp1A.f1=(struct _dyneither_ptr)({struct Cyc_String_pa_struct
_tmp1D;_tmp1D.tag=0;_tmp1D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t));{void*_tmp1B[1]={& _tmp1D};Cyc_aprintf(({const char*_tmp1C="get_field_index failed: %s";
_tag_dyneither(_tmp1C,sizeof(char),27);}),_tag_dyneither(_tmp1B,sizeof(void*),1));}});
_tmp1A;});_tmp19;}));_LL5:;}static void Cyc_NewControlFlow_update_tryflow(struct
Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union new_flow){if(
env->in_try){env->tryflow=Cyc_CfFlowInfo_join_flow(env->fenv,env->all_changed,
new_flow,env->tryflow,1);{struct Cyc_CfFlowInfo_ConsumeInfo _tmp1F;struct Cyc_Dict_Dict
_tmp20;struct Cyc_List_List*_tmp21;struct _tuple6 _tmp1E=Cyc_CfFlowInfo_save_consume_info(
env->fenv,env->tryflow,0);_tmp1F=_tmp1E.f1;_tmp20=_tmp1F.consumed;_tmp21=_tmp1F.may_consume;
env->tryflow=Cyc_CfFlowInfo_restore_consume_info(env->tryflow,({struct Cyc_CfFlowInfo_ConsumeInfo
_tmp22;_tmp22.consumed=_tmp20;_tmp22.may_consume=0;_tmp22;}));}}}static void Cyc_NewControlFlow_update_flow(
struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Absyn_Stmt*s,union Cyc_CfFlowInfo_FlowInfo_union
flow){struct Cyc_NewControlFlow_CFStmtAnnot*_tmp23=Cyc_NewControlFlow_get_stmt_annot(
s);union Cyc_CfFlowInfo_FlowInfo_union*_tmp24=Cyc_NewControlFlow_get_stmt_flow(
env,s);union Cyc_CfFlowInfo_FlowInfo_union _tmp25=Cyc_CfFlowInfo_join_flow(env->fenv,
env->all_changed,flow,*_tmp24,1);if(!Cyc_CfFlowInfo_flow_lessthan_approx(_tmp25,*
_tmp24)){*_tmp24=_tmp25;if(_tmp23->visited == env->iteration_num)env->iterate_again=
1;}}static union Cyc_CfFlowInfo_FlowInfo_union Cyc_NewControlFlow_add_vars(struct
Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo_union inflow,struct Cyc_List_List*
vds,void*leafval,struct Cyc_Position_Segment*loc){union Cyc_CfFlowInfo_FlowInfo_union
_tmp26=inflow;struct Cyc_Dict_Dict _tmp27;struct Cyc_List_List*_tmp28;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp29;_LLD: if((_tmp26.BottomFL).tag != 0)goto _LLF;_LLE: return(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp2A;(_tmp2A.BottomFL).tag=0;_tmp2A;});_LLF:
if((_tmp26.ReachableFL).tag != 1)goto _LLC;_tmp27=(_tmp26.ReachableFL).f1;_tmp28=(
_tmp26.ReachableFL).f2;_tmp29=(_tmp26.ReachableFL).f3;_LL10: for(0;vds != 0;vds=
vds->tl){struct Cyc_CfFlowInfo_VarRoot_struct*_tmp2B=({struct Cyc_CfFlowInfo_VarRoot_struct*
_tmp2C=_region_malloc(fenv->r,sizeof(*_tmp2C));_tmp2C[0]=({struct Cyc_CfFlowInfo_VarRoot_struct
_tmp2D;_tmp2D.tag=0;_tmp2D.f1=(struct Cyc_Absyn_Vardecl*)vds->hd;_tmp2D;});_tmp2C;});
_tmp27=Cyc_Dict_insert(_tmp27,(void*)_tmp2B,Cyc_CfFlowInfo_typ_to_absrval(fenv,(
void*)((struct Cyc_Absyn_Vardecl*)vds->hd)->type,leafval));}return(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp2E;(_tmp2E.ReachableFL).tag=1;(_tmp2E.ReachableFL).f1=
_tmp27;(_tmp2E.ReachableFL).f2=_tmp28;(_tmp2E.ReachableFL).f3=_tmp29;_tmp2E;});
_LLC:;}struct _tuple10{struct Cyc_CfFlowInfo_Place*f1;struct Cyc_Position_Segment*
f2;};static void Cyc_NewControlFlow_remove_vars(struct Cyc_CfFlowInfo_FlowEnv*fenv,
union Cyc_CfFlowInfo_FlowInfo_union outflow,struct Cyc_Dict_Dict*out_unique_places,
struct Cyc_Dict_Dict old_unique_places){struct Cyc_CfFlowInfo_ConsumeInfo _tmp30;
struct Cyc_Dict_Dict _tmp31;struct _tuple6 _tmp2F=Cyc_CfFlowInfo_save_consume_info(
fenv,outflow,0);_tmp30=_tmp2F.f1;_tmp31=_tmp30.consumed;{struct _RegionHandle
_tmp32=_new_region("tmp");struct _RegionHandle*tmp=& _tmp32;_push_region(tmp);{
struct Cyc_Iter_Iter _tmp33=((struct Cyc_Iter_Iter(*)(struct _RegionHandle*rgn,
struct Cyc_Dict_Dict d))Cyc_Dict_make_iter)(tmp,*((struct Cyc_Dict_Dict*)
_check_null(out_unique_places)));struct _tuple10 _tmp34=*((struct _tuple10*(*)(
struct _RegionHandle*r,struct Cyc_Dict_Dict d))Cyc_Dict_rchoose)(tmp,*
out_unique_places);while(((int(*)(struct Cyc_Iter_Iter,struct _tuple10*))Cyc_Iter_next)(
_tmp33,& _tmp34)){struct Cyc_CfFlowInfo_Place*_tmp35=_tmp34.f1;if(!((int(*)(struct
Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*k))Cyc_Dict_member)(_tmp31,_tmp35))({
struct Cyc_String_pa_struct _tmp38;_tmp38.tag=0;_tmp38.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_CfFlowInfo_place_err_string(_tmp35));{void*_tmp36[1]={&
_tmp38};Cyc_Tcutil_terr(_tmp34.f2,({const char*_tmp37="Failed to consume unique variable %s";
_tag_dyneither(_tmp37,sizeof(char),37);}),_tag_dyneither(_tmp36,sizeof(void*),1));}});}};
_pop_region(tmp);}*out_unique_places=old_unique_places;}static union Cyc_CfFlowInfo_FlowInfo_union
Cyc_NewControlFlow_use_Rval(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Position_Segment*
loc,union Cyc_CfFlowInfo_FlowInfo_union inflow,void*r){union Cyc_CfFlowInfo_FlowInfo_union
_tmp39=inflow;struct Cyc_Dict_Dict _tmp3A;struct Cyc_List_List*_tmp3B;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp3C;_LL12: if((_tmp39.BottomFL).tag != 0)goto _LL14;_LL13: return(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp3D;(_tmp3D.BottomFL).tag=0;_tmp3D;});_LL14:
if((_tmp39.ReachableFL).tag != 1)goto _LL11;_tmp3A=(_tmp39.ReachableFL).f1;_tmp3B=(
_tmp39.ReachableFL).f2;_tmp3C=(_tmp39.ReachableFL).f3;_LL15: if(Cyc_CfFlowInfo_initlevel(
_tmp3A,r)!= (void*)2)({void*_tmp3E=0;Cyc_Tcutil_terr(loc,({const char*_tmp3F="expression may not be fully initialized";
_tag_dyneither(_tmp3F,sizeof(char),40);}),_tag_dyneither(_tmp3E,sizeof(void*),0));});{
struct Cyc_Dict_Dict _tmp40=Cyc_CfFlowInfo_escape_deref(env->fenv,_tmp3A,env->all_changed,
r);if(_tmp3A.t == _tmp40.t)return inflow;{union Cyc_CfFlowInfo_FlowInfo_union _tmp41=({
union Cyc_CfFlowInfo_FlowInfo_union _tmp42;(_tmp42.ReachableFL).tag=1;(_tmp42.ReachableFL).f1=
_tmp40;(_tmp42.ReachableFL).f2=_tmp3B;(_tmp42.ReachableFL).f3=_tmp3C;_tmp42;});
Cyc_NewControlFlow_update_tryflow(env,(union Cyc_CfFlowInfo_FlowInfo_union)_tmp41);
return(union Cyc_CfFlowInfo_FlowInfo_union)_tmp41;}}_LL11:;}struct _tuple11{union
Cyc_CfFlowInfo_FlowInfo_union f1;struct Cyc_List_List*f2;};static struct _tuple11 Cyc_NewControlFlow_anal_unordered_Rexps(
struct _RegionHandle*rgn,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union
inflow,struct Cyc_List_List*es,int arg1_unconsumed){if(es == 0)return({struct
_tuple11 _tmp43;_tmp43.f1=inflow;_tmp43.f2=0;_tmp43;});if(es->tl == 0){union Cyc_CfFlowInfo_FlowInfo_union
_tmp45;void*_tmp46;struct _tuple5 _tmp44=Cyc_NewControlFlow_anal_Rexp(env,inflow,(
struct Cyc_Absyn_Exp*)es->hd);_tmp45=_tmp44.f1;_tmp46=_tmp44.f2;return({struct
_tuple11 _tmp47;_tmp47.f1=_tmp45;_tmp47.f2=({struct Cyc_List_List*_tmp48=
_region_malloc(rgn,sizeof(*_tmp48));_tmp48->hd=(void*)_tmp46;_tmp48->tl=0;_tmp48;});
_tmp47;});}{struct Cyc_Dict_Dict*outer_all_changed=env->all_changed;struct Cyc_Dict_Dict
this_all_changed;union Cyc_CfFlowInfo_FlowInfo_union old_inflow;union Cyc_CfFlowInfo_FlowInfo_union
outflow;struct Cyc_List_List*rvals;struct Cyc_CfFlowInfo_ConsumeInfo _tmp4A;union
Cyc_CfFlowInfo_FlowInfo_union _tmp4B;struct _tuple6 _tmp49=Cyc_CfFlowInfo_save_consume_info(
env->fenv,inflow,1);_tmp4A=_tmp49.f1;_tmp4B=_tmp49.f2;{struct Cyc_CfFlowInfo_ConsumeInfo
outflow_consume=({struct Cyc_CfFlowInfo_ConsumeInfo _tmp60;_tmp60.consumed=(env->fenv)->mt_place_set;
_tmp60.may_consume=0;_tmp60;});int init_consume=0;do{this_all_changed=(env->fenv)->mt_place_set;
_tmp4B=Cyc_CfFlowInfo_restore_consume_info(_tmp4B,_tmp4A);env->all_changed=({
struct Cyc_Dict_Dict*_tmp4C=_region_malloc(env->r,sizeof(*_tmp4C));_tmp4C[0]=(env->fenv)->mt_place_set;
_tmp4C;});{union Cyc_CfFlowInfo_FlowInfo_union _tmp4E;void*_tmp4F;struct _tuple5
_tmp4D=Cyc_NewControlFlow_anal_Rexp(env,_tmp4B,(struct Cyc_Absyn_Exp*)es->hd);
_tmp4E=_tmp4D.f1;_tmp4F=_tmp4D.f2;outflow=_tmp4E;rvals=({struct Cyc_List_List*
_tmp50=_region_malloc(rgn,sizeof(*_tmp50));_tmp50->hd=(void*)_tmp4F;_tmp50->tl=0;
_tmp50;});this_all_changed=Cyc_CfFlowInfo_union_place_set(this_all_changed,*((
struct Cyc_Dict_Dict*)_check_null(env->all_changed)),0);if(arg1_unconsumed){union
Cyc_CfFlowInfo_FlowInfo_union _tmp51=outflow;struct Cyc_Dict_Dict _tmp52;struct Cyc_List_List*
_tmp53;struct Cyc_CfFlowInfo_ConsumeInfo _tmp54;_LL17: if((_tmp51.BottomFL).tag != 0)
goto _LL19;_LL18: goto _LL16;_LL19: if((_tmp51.ReachableFL).tag != 1)goto _LL16;_tmp52=(
_tmp51.ReachableFL).f1;_tmp53=(_tmp51.ReachableFL).f2;_tmp54=(_tmp51.ReachableFL).f3;
_LL1A: outflow=(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp55;(_tmp55.ReachableFL).tag=1;(_tmp55.ReachableFL).f1=_tmp52;(_tmp55.ReachableFL).f2=
_tmp53;(_tmp55.ReachableFL).f3=({struct Cyc_CfFlowInfo_ConsumeInfo _tmp56;_tmp56.consumed=
_tmp54.consumed;_tmp56.may_consume=_tmp4A.may_consume;_tmp56;});_tmp55;});_LL16:;}{
struct Cyc_List_List*es2=es->tl;for(0;es2 != 0;es2=es2->tl){env->all_changed=({
struct Cyc_Dict_Dict*_tmp57=_region_malloc(env->r,sizeof(*_tmp57));_tmp57[0]=(env->fenv)->mt_place_set;
_tmp57;});{union Cyc_CfFlowInfo_FlowInfo_union _tmp59;void*_tmp5A;struct _tuple5
_tmp58=Cyc_NewControlFlow_anal_Rexp(env,_tmp4B,(struct Cyc_Absyn_Exp*)es2->hd);
_tmp59=_tmp58.f1;_tmp5A=_tmp58.f2;rvals=({struct Cyc_List_List*_tmp5B=
_region_malloc(rgn,sizeof(*_tmp5B));_tmp5B->hd=(void*)_tmp5A;_tmp5B->tl=rvals;
_tmp5B;});outflow=Cyc_CfFlowInfo_after_flow(env->fenv,(struct Cyc_Dict_Dict*)&
this_all_changed,outflow,_tmp59,this_all_changed,*((struct Cyc_Dict_Dict*)
_check_null(env->all_changed)));this_all_changed=Cyc_CfFlowInfo_union_place_set(
this_all_changed,*((struct Cyc_Dict_Dict*)_check_null(env->all_changed)),0);}}}{
struct Cyc_CfFlowInfo_ConsumeInfo _tmp5D;struct _tuple6 _tmp5C=Cyc_CfFlowInfo_save_consume_info(
env->fenv,outflow,0);_tmp5D=_tmp5C.f1;if(init_consume)old_inflow=Cyc_CfFlowInfo_restore_consume_info(
_tmp4B,outflow_consume);else{old_inflow=_tmp4B;}init_consume=1;outflow_consume=
_tmp5D;_tmp4B=Cyc_CfFlowInfo_join_flow(env->fenv,outer_all_changed,_tmp4B,
outflow,1);}}}while(!Cyc_CfFlowInfo_flow_lessthan_approx(_tmp4B,old_inflow));if(
outer_all_changed == 0)env->all_changed=0;else{env->all_changed=({struct Cyc_Dict_Dict*
_tmp5E=_region_malloc(env->r,sizeof(*_tmp5E));_tmp5E[0]=Cyc_CfFlowInfo_union_place_set(*
outer_all_changed,this_all_changed,0);_tmp5E;});}Cyc_NewControlFlow_update_tryflow(
env,outflow);return({struct _tuple11 _tmp5F;_tmp5F.f1=outflow;_tmp5F.f2=Cyc_List_imp_rev(
rvals);_tmp5F;});}}}static struct _tuple5 Cyc_NewControlFlow_anal_use_ints(struct
Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union inflow,struct
Cyc_List_List*es,int arg1_unconsumed){struct _RegionHandle _tmp61=_new_region("rgn");
struct _RegionHandle*rgn=& _tmp61;_push_region(rgn);{union Cyc_CfFlowInfo_FlowInfo_union
_tmp63;struct Cyc_List_List*_tmp64;struct _tuple11 _tmp62=Cyc_NewControlFlow_anal_unordered_Rexps(
rgn,env,inflow,es,arg1_unconsumed);_tmp63=_tmp62.f1;_tmp64=_tmp62.f2;{union Cyc_CfFlowInfo_FlowInfo_union
_tmp65=_tmp63;struct Cyc_Dict_Dict _tmp66;_LL1C: if((_tmp65.ReachableFL).tag != 1)
goto _LL1E;_tmp66=(_tmp65.ReachableFL).f1;_LL1D: for(0;_tmp64 != 0;(_tmp64=_tmp64->tl,
es=((struct Cyc_List_List*)_check_null(es))->tl)){if(Cyc_CfFlowInfo_initlevel(
_tmp66,(void*)_tmp64->hd)== (void*)0)({void*_tmp67=0;Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(es))->hd)->loc,({const char*_tmp68="expression may not be initialized";
_tag_dyneither(_tmp68,sizeof(char),34);}),_tag_dyneither(_tmp67,sizeof(void*),0));});}
goto _LL1B;_LL1E: if((_tmp65.BottomFL).tag != 0)goto _LL1B;_LL1F: goto _LL1B;_LL1B:;}{
struct _tuple5 _tmp6A=({struct _tuple5 _tmp69;_tmp69.f1=_tmp63;_tmp69.f2=(void*)(env->fenv)->unknown_all;
_tmp69;});_npop_handler(0);return _tmp6A;}};_pop_region(rgn);}static union Cyc_CfFlowInfo_FlowInfo_union
Cyc_NewControlFlow_notzero(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union
inflow,union Cyc_CfFlowInfo_FlowInfo_union outflow,struct Cyc_Absyn_Exp*e,void*il){
union Cyc_CfFlowInfo_FlowInfo_union _tmp6B=outflow;struct Cyc_Dict_Dict _tmp6C;
struct Cyc_List_List*_tmp6D;struct Cyc_CfFlowInfo_ConsumeInfo _tmp6E;_LL21: if((
_tmp6B.BottomFL).tag != 0)goto _LL23;_LL22: return outflow;_LL23: if((_tmp6B.ReachableFL).tag
!= 1)goto _LL20;_tmp6C=(_tmp6B.ReachableFL).f1;_tmp6D=(_tmp6B.ReachableFL).f2;
_tmp6E=(_tmp6B.ReachableFL).f3;_LL24: {union Cyc_CfFlowInfo_AbsLVal_union _tmp6F=(
Cyc_NewControlFlow_anal_Lexp(env,inflow,e)).f2;struct Cyc_CfFlowInfo_Place*_tmp70;
_LL26: if((_tmp6F.UnknownL).tag != 1)goto _LL28;_LL27: return outflow;_LL28: if((
_tmp6F.PlaceL).tag != 0)goto _LL25;_tmp70=(_tmp6F.PlaceL).f1;_LL29: {void*nzval=il
== (void*)2?(void*)1:(void*)2;union Cyc_CfFlowInfo_FlowInfo_union _tmp71=({union
Cyc_CfFlowInfo_FlowInfo_union _tmp72;(_tmp72.ReachableFL).tag=1;(_tmp72.ReachableFL).f1=
Cyc_CfFlowInfo_assign_place(env->fenv,e->loc,_tmp6C,env->all_changed,_tmp70,
nzval);(_tmp72.ReachableFL).f2=_tmp6D;(_tmp72.ReachableFL).f3=_tmp6E;_tmp72;});
return(union Cyc_CfFlowInfo_FlowInfo_union)_tmp71;}_LL25:;}_LL20:;}static struct
_tuple8 Cyc_NewControlFlow_splitzero(struct Cyc_NewControlFlow_AnalEnv*env,union
Cyc_CfFlowInfo_FlowInfo_union inflow,union Cyc_CfFlowInfo_FlowInfo_union outflow,
struct Cyc_Absyn_Exp*e,void*il){union Cyc_CfFlowInfo_FlowInfo_union _tmp73=outflow;
struct Cyc_Dict_Dict _tmp74;struct Cyc_List_List*_tmp75;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp76;_LL2B: if((_tmp73.BottomFL).tag != 0)goto _LL2D;_LL2C: return({struct _tuple8
_tmp77;_tmp77.f1=outflow;_tmp77.f2=outflow;_tmp77;});_LL2D: if((_tmp73.ReachableFL).tag
!= 1)goto _LL2A;_tmp74=(_tmp73.ReachableFL).f1;_tmp75=(_tmp73.ReachableFL).f2;
_tmp76=(_tmp73.ReachableFL).f3;_LL2E: {union Cyc_CfFlowInfo_AbsLVal_union _tmp78=(
Cyc_NewControlFlow_anal_Lexp(env,inflow,e)).f2;struct Cyc_CfFlowInfo_Place*_tmp79;
_LL30: if((_tmp78.UnknownL).tag != 1)goto _LL32;_LL31: return({struct _tuple8 _tmp7A;
_tmp7A.f1=outflow;_tmp7A.f2=outflow;_tmp7A;});_LL32: if((_tmp78.PlaceL).tag != 0)
goto _LL2F;_tmp79=(_tmp78.PlaceL).f1;_LL33: {void*nzval=il == (void*)2?(void*)1:(
void*)2;return({struct _tuple8 _tmp7B;_tmp7B.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp7D;(_tmp7D.ReachableFL).tag=1;(_tmp7D.ReachableFL).f1=
Cyc_CfFlowInfo_assign_place(env->fenv,e->loc,_tmp74,env->all_changed,_tmp79,
nzval);(_tmp7D.ReachableFL).f2=_tmp75;(_tmp7D.ReachableFL).f3=_tmp76;_tmp7D;});
_tmp7B.f2=(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp7C;(_tmp7C.ReachableFL).tag=1;(_tmp7C.ReachableFL).f1=Cyc_CfFlowInfo_assign_place(
env->fenv,e->loc,_tmp74,env->all_changed,_tmp79,(void*)0);(_tmp7C.ReachableFL).f2=
_tmp75;(_tmp7C.ReachableFL).f3=_tmp76;_tmp7C;});_tmp7B;});}_LL2F:;}_LL2A:;}
static union Cyc_CfFlowInfo_FlowInfo_union Cyc_NewControlFlow_if_tagcmp(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo_union flow,struct Cyc_Absyn_Exp*e1,void*r1,void*p,
void*r2){union Cyc_CfFlowInfo_FlowInfo_union _tmp7E=flow;struct Cyc_Dict_Dict _tmp7F;
struct Cyc_List_List*_tmp80;struct Cyc_CfFlowInfo_ConsumeInfo _tmp81;_LL35: if((
_tmp7E.BottomFL).tag != 0)goto _LL37;_LL36: return flow;_LL37: if((_tmp7E.ReachableFL).tag
!= 1)goto _LL34;_tmp7F=(_tmp7E.ReachableFL).f1;_tmp80=(_tmp7E.ReachableFL).f2;
_tmp81=(_tmp7E.ReachableFL).f3;_LL38: {void*_tmp82=r2;struct Cyc_List_List*_tmp83;
_LL3A: if(_tmp82 <= (void*)3)goto _LL3C;if(*((int*)_tmp82)!= 3)goto _LL3C;_tmp83=((
struct Cyc_CfFlowInfo_TagCmps_struct*)_tmp82)->f1;_LL3B: {union Cyc_CfFlowInfo_AbsLVal_union
_tmp84=(Cyc_NewControlFlow_anal_Lexp(env,flow,e1)).f2;struct Cyc_CfFlowInfo_Place*
_tmp85;_LL3F: if((_tmp84.UnknownL).tag != 1)goto _LL41;_LL40: return flow;_LL41: if((
_tmp84.PlaceL).tag != 0)goto _LL3E;_tmp85=(_tmp84.PlaceL).f1;_LL42: {struct Cyc_List_List*
new_cl;{void*_tmp86=r1;struct Cyc_List_List*_tmp87;void*_tmp88;_LL44: if(_tmp86 <= (
void*)3)goto _LL48;if(*((int*)_tmp86)!= 3)goto _LL46;_tmp87=((struct Cyc_CfFlowInfo_TagCmps_struct*)
_tmp86)->f1;_LL45: new_cl=_tmp87;goto _LL43;_LL46: if(*((int*)_tmp86)!= 0)goto _LL48;
_tmp88=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp86)->f1;if((int)_tmp88
!= 2)goto _LL48;_LL47: goto _LL49;_LL48: if((int)_tmp86 != 0)goto _LL4A;_LL49: goto
_LL4B;_LL4A: if((int)_tmp86 != 1)goto _LL4C;_LL4B: new_cl=0;goto _LL43;_LL4C:;_LL4D:
return flow;_LL43:;}for(0;_tmp83 != 0;_tmp83=_tmp83->tl){void*new_cmp;{struct
_tuple0 _tmp8A=({struct _tuple0 _tmp89;_tmp89.f1=p;_tmp89.f2=(void*)((struct Cyc_CfFlowInfo_TagCmp*)
_tmp83->hd)->cmp;_tmp89;});void*_tmp8B;void*_tmp8C;void*_tmp8D;void*_tmp8E;_LL4F:
_tmp8B=_tmp8A.f1;if((int)_tmp8B != 8)goto _LL51;_LL50: goto _LL52;_LL51: _tmp8C=
_tmp8A.f2;if((int)_tmp8C != 8)goto _LL53;_LL52: new_cmp=(void*)8;goto _LL4E;_LL53:
_tmp8D=_tmp8A.f1;if((int)_tmp8D != 5)goto _LL55;_tmp8E=_tmp8A.f2;if((int)_tmp8E != 
5)goto _LL55;_LL54: new_cmp=(void*)5;goto _LL4E;_LL55:;_LL56: new_cmp=(void*)10;goto
_LL4E;_LL4E:;}new_cl=({struct Cyc_List_List*_tmp8F=_region_malloc(env->r,sizeof(*
_tmp8F));_tmp8F->hd=({struct Cyc_CfFlowInfo_TagCmp*_tmp90=_region_malloc(env->r,
sizeof(*_tmp90));_tmp90->cmp=(void*)new_cmp;_tmp90->bd=(void*)((void*)((struct
Cyc_CfFlowInfo_TagCmp*)_tmp83->hd)->bd);_tmp90;});_tmp8F->tl=new_cl;_tmp8F;});}
return(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp91;(_tmp91.ReachableFL).tag=1;(_tmp91.ReachableFL).f1=Cyc_CfFlowInfo_assign_place(
env->fenv,e1->loc,_tmp7F,env->all_changed,_tmp85,(void*)({struct Cyc_CfFlowInfo_TagCmps_struct*
_tmp92=_region_malloc(env->r,sizeof(*_tmp92));_tmp92[0]=({struct Cyc_CfFlowInfo_TagCmps_struct
_tmp93;_tmp93.tag=3;_tmp93.f1=new_cl;_tmp93;});_tmp92;}));(_tmp91.ReachableFL).f2=
_tmp80;(_tmp91.ReachableFL).f3=_tmp81;_tmp91;});}_LL3E:;}_LL3C:;_LL3D: return flow;
_LL39:;}_LL34:;}static struct Cyc_CfFlowInfo_NotZero_struct Cyc_NewControlFlow_mt_notzero_v={
Cyc_CfFlowInfo_NotZero,0};static struct Cyc_CfFlowInfo_UnknownZ_struct Cyc_NewControlFlow_mt_unknownz_v={
Cyc_CfFlowInfo_UnknownZ,0};static struct _tuple5 Cyc_NewControlFlow_anal_derefR(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union inflow,
union Cyc_CfFlowInfo_FlowInfo_union f,struct Cyc_Absyn_Exp*e,void*r){void*_tmp96=
Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
struct Cyc_Absyn_PtrInfo _tmp97;void*_tmp98;struct Cyc_Absyn_PtrAtts _tmp99;struct
Cyc_Absyn_Conref*_tmp9A;struct Cyc_Absyn_Conref*_tmp9B;_LL58: if(_tmp96 <= (void*)4)
goto _LL5A;if(*((int*)_tmp96)!= 4)goto _LL5A;_tmp97=((struct Cyc_Absyn_PointerType_struct*)
_tmp96)->f1;_tmp98=(void*)_tmp97.elt_typ;_tmp99=_tmp97.ptr_atts;_tmp9A=_tmp99.bounds;
_tmp9B=_tmp99.zero_term;_LL59: {union Cyc_CfFlowInfo_FlowInfo_union _tmp9C=f;
struct Cyc_Dict_Dict _tmp9D;struct Cyc_List_List*_tmp9E;_LL5D: if((_tmp9C.BottomFL).tag
!= 0)goto _LL5F;_LL5E: return({struct _tuple5 _tmp9F;_tmp9F.f1=f;_tmp9F.f2=Cyc_CfFlowInfo_typ_to_absrval(
env->fenv,_tmp98,(void*)(env->fenv)->unknown_all);_tmp9F;});_LL5F: if((_tmp9C.ReachableFL).tag
!= 1)goto _LL5C;_tmp9D=(_tmp9C.ReachableFL).f1;_tmp9E=(_tmp9C.ReachableFL).f2;
_LL60: if(Cyc_Tcutil_is_bound_one(_tmp9A)){void*_tmpA0=r;struct Cyc_CfFlowInfo_Place*
_tmpA1;void*_tmpA2;_LL62: if((int)_tmpA0 != 1)goto _LL64;_LL63: goto _LL65;_LL64: if((
int)_tmpA0 != 2)goto _LL66;_LL65:{void*_tmpA3=(void*)e->annot;struct Cyc_List_List*
_tmpA4;_LL6F: if(*((void**)_tmpA3)!= Cyc_CfFlowInfo_NotZero)goto _LL71;_tmpA4=((
struct Cyc_CfFlowInfo_NotZero_struct*)_tmpA3)->f1;_LL70: if(!Cyc_CfFlowInfo_same_relns(
_tmp9E,_tmpA4))goto _LL72;goto _LL6E;_LL71:;_LL72:{void*_tmpA5=(void*)e->r;_LL74:
if(*((int*)_tmpA5)!= 25)goto _LL76;_LL75:(void*)(e->annot=(void*)((void*)({struct
Cyc_CfFlowInfo_NotZero_struct*_tmpA6=_cycalloc(sizeof(*_tmpA6));_tmpA6[0]=({
struct Cyc_CfFlowInfo_NotZero_struct _tmpA7;_tmpA7.tag=Cyc_CfFlowInfo_NotZero;
_tmpA7.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,_tmp9E);_tmpA7;});
_tmpA6;})));goto _LL73;_LL76:;_LL77:(void*)(e->annot=(void*)((void*)& Cyc_NewControlFlow_mt_notzero_v));
goto _LL73;_LL73:;}goto _LL6E;_LL6E:;}goto _LL61;_LL66: if(_tmpA0 <= (void*)3)goto
_LL68;if(*((int*)_tmpA0)!= 2)goto _LL68;_tmpA1=((struct Cyc_CfFlowInfo_AddressOf_struct*)
_tmpA0)->f1;_LL67:{void*_tmpA8=(void*)e->annot;struct Cyc_List_List*_tmpA9;_LL79:
if(*((void**)_tmpA8)!= Cyc_CfFlowInfo_NotZero)goto _LL7B;_tmpA9=((struct Cyc_CfFlowInfo_NotZero_struct*)
_tmpA8)->f1;_LL7A: if(!Cyc_CfFlowInfo_same_relns(_tmp9E,_tmpA9))goto _LL7C;goto
_LL78;_LL7B:;_LL7C:{void*_tmpAA=(void*)e->r;_LL7E: if(*((int*)_tmpAA)!= 25)goto
_LL80;_LL7F:(void*)(e->annot=(void*)((void*)({struct Cyc_CfFlowInfo_NotZero_struct*
_tmpAB=_cycalloc(sizeof(*_tmpAB));_tmpAB[0]=({struct Cyc_CfFlowInfo_NotZero_struct
_tmpAC;_tmpAC.tag=Cyc_CfFlowInfo_NotZero;_tmpAC.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,
_tmp9E);_tmpAC;});_tmpAB;})));goto _LL7D;_LL80:;_LL81:(void*)(e->annot=(void*)((
void*)& Cyc_NewControlFlow_mt_notzero_v));goto _LL7D;_LL7D:;}goto _LL78;_LL78:;}
return({struct _tuple5 _tmpAD;_tmpAD.f1=f;_tmpAD.f2=Cyc_CfFlowInfo_lookup_place(
_tmp9D,_tmpA1);_tmpAD;});_LL68: if((int)_tmpA0 != 0)goto _LL6A;_LL69:(void*)(e->annot=(
void*)((void*)Cyc_CfFlowInfo_IsZero));return({struct _tuple5 _tmpAE;_tmpAE.f1=(
union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union _tmpAF;(
_tmpAF.BottomFL).tag=0;_tmpAF;});_tmpAE.f2=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,
_tmp98,(void*)(env->fenv)->unknown_all);_tmpAE;});_LL6A: if(_tmpA0 <= (void*)3)
goto _LL6C;if(*((int*)_tmpA0)!= 0)goto _LL6C;_tmpA2=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)
_tmpA0)->f1;_LL6B: f=Cyc_NewControlFlow_notzero(env,inflow,f,e,_tmpA2);goto _LL6D;
_LL6C:;_LL6D:{void*_tmpB0=(void*)e->r;_LL83: if(*((int*)_tmpB0)!= 25)goto _LL85;
_LL84:(void*)(e->annot=(void*)((void*)({struct Cyc_CfFlowInfo_UnknownZ_struct*
_tmpB1=_cycalloc(sizeof(*_tmpB1));_tmpB1[0]=({struct Cyc_CfFlowInfo_UnknownZ_struct
_tmpB2;_tmpB2.tag=Cyc_CfFlowInfo_UnknownZ;_tmpB2.f1=Cyc_CfFlowInfo_copy_relns(
Cyc_Core_heap_region,_tmp9E);_tmpB2;});_tmpB1;})));goto _LL82;_LL85:;_LL86:(void*)(
e->annot=(void*)((void*)& Cyc_NewControlFlow_mt_unknownz_v));goto _LL82;_LL82:;}
goto _LL61;_LL61:;}else{void*_tmpB3=(void*)e->annot;struct Cyc_List_List*_tmpB4;
_LL88: if(*((void**)_tmpB3)!= Cyc_CfFlowInfo_UnknownZ)goto _LL8A;_tmpB4=((struct
Cyc_CfFlowInfo_UnknownZ_struct*)_tmpB3)->f1;_LL89: if(!Cyc_CfFlowInfo_same_relns(
_tmp9E,_tmpB4))goto _LL8B;goto _LL87;_LL8A:;_LL8B:(void*)(e->annot=(void*)((void*)({
struct Cyc_CfFlowInfo_UnknownZ_struct*_tmpB5=_cycalloc(sizeof(*_tmpB5));_tmpB5[0]=({
struct Cyc_CfFlowInfo_UnknownZ_struct _tmpB6;_tmpB6.tag=Cyc_CfFlowInfo_UnknownZ;
_tmpB6.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,_tmp9E);_tmpB6;});
_tmpB5;})));goto _LL87;_LL87:;}{void*_tmpB7=Cyc_CfFlowInfo_initlevel(_tmp9D,r);
_LL8D: if((int)_tmpB7 != 0)goto _LL8F;_LL8E:({void*_tmpB8=0;Cyc_Tcutil_terr(e->loc,({
const char*_tmpB9="dereference of possibly uninitialized pointer";_tag_dyneither(
_tmpB9,sizeof(char),46);}),_tag_dyneither(_tmpB8,sizeof(void*),0));});goto _LL90;
_LL8F: if((int)_tmpB7 != 2)goto _LL91;_LL90: return({struct _tuple5 _tmpBA;_tmpBA.f1=f;
_tmpBA.f2=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmp98,(void*)(env->fenv)->unknown_all);
_tmpBA;});_LL91: if((int)_tmpB7 != 1)goto _LL8C;_LL92: return({struct _tuple5 _tmpBB;
_tmpBB.f1=f;_tmpBB.f2=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmp98,(void*)(env->fenv)->unknown_none);
_tmpBB;});_LL8C:;}_LL5C:;}_LL5A:;_LL5B:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmpBC=_cycalloc(sizeof(*_tmpBC));_tmpBC[0]=({struct Cyc_Core_Impossible_struct
_tmpBD;_tmpBD.tag=Cyc_Core_Impossible;_tmpBD.f1=({const char*_tmpBE="right deref of non-pointer-type";
_tag_dyneither(_tmpBE,sizeof(char),32);});_tmpBD;});_tmpBC;}));_LL57:;}static
struct Cyc_List_List*Cyc_NewControlFlow_add_subscript_reln(struct _RegionHandle*
rgn,struct Cyc_List_List*relns,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
void*_tmpBF=(void*)e1->r;void*_tmpC0;struct Cyc_Absyn_Vardecl*_tmpC1;void*_tmpC2;
struct Cyc_Absyn_Vardecl*_tmpC3;void*_tmpC4;struct Cyc_Absyn_Vardecl*_tmpC5;void*
_tmpC6;struct Cyc_Absyn_Vardecl*_tmpC7;_LL94: if(*((int*)_tmpBF)!= 1)goto _LL96;
_tmpC0=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmpBF)->f2;if(_tmpC0 <= (void*)1)
goto _LL96;if(*((int*)_tmpC0)!= 4)goto _LL96;_tmpC1=((struct Cyc_Absyn_Pat_b_struct*)
_tmpC0)->f1;_LL95: _tmpC3=_tmpC1;goto _LL97;_LL96: if(*((int*)_tmpBF)!= 1)goto _LL98;
_tmpC2=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmpBF)->f2;if(_tmpC2 <= (void*)1)
goto _LL98;if(*((int*)_tmpC2)!= 3)goto _LL98;_tmpC3=((struct Cyc_Absyn_Local_b_struct*)
_tmpC2)->f1;_LL97: _tmpC5=_tmpC3;goto _LL99;_LL98: if(*((int*)_tmpBF)!= 1)goto _LL9A;
_tmpC4=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmpBF)->f2;if(_tmpC4 <= (void*)1)
goto _LL9A;if(*((int*)_tmpC4)!= 2)goto _LL9A;_tmpC5=((struct Cyc_Absyn_Param_b_struct*)
_tmpC4)->f1;_LL99: _tmpC7=_tmpC5;goto _LL9B;_LL9A: if(*((int*)_tmpBF)!= 1)goto _LL9C;
_tmpC6=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmpBF)->f2;if(_tmpC6 <= (void*)1)
goto _LL9C;if(*((int*)_tmpC6)!= 0)goto _LL9C;_tmpC7=((struct Cyc_Absyn_Global_b_struct*)
_tmpC6)->f1;_LL9B: if(!_tmpC7->escapes){void*_tmpC8=(void*)e2->r;void*_tmpC9;
struct Cyc_Absyn_Vardecl*_tmpCA;void*_tmpCB;struct Cyc_Absyn_Vardecl*_tmpCC;void*
_tmpCD;struct Cyc_Absyn_Vardecl*_tmpCE;void*_tmpCF;struct Cyc_Absyn_Vardecl*_tmpD0;
_LL9F: if(*((int*)_tmpC8)!= 1)goto _LLA1;_tmpC9=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmpC8)->f2;if(_tmpC9 <= (void*)1)goto _LLA1;if(*((int*)_tmpC9)!= 4)goto _LLA1;
_tmpCA=((struct Cyc_Absyn_Pat_b_struct*)_tmpC9)->f1;_LLA0: _tmpCC=_tmpCA;goto _LLA2;
_LLA1: if(*((int*)_tmpC8)!= 1)goto _LLA3;_tmpCB=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmpC8)->f2;if(_tmpCB <= (void*)1)goto _LLA3;if(*((int*)_tmpCB)!= 3)goto _LLA3;
_tmpCC=((struct Cyc_Absyn_Local_b_struct*)_tmpCB)->f1;_LLA2: _tmpCE=_tmpCC;goto
_LLA4;_LLA3: if(*((int*)_tmpC8)!= 1)goto _LLA5;_tmpCD=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmpC8)->f2;if(_tmpCD <= (void*)1)goto _LLA5;if(*((int*)_tmpCD)!= 2)goto _LLA5;
_tmpCE=((struct Cyc_Absyn_Param_b_struct*)_tmpCD)->f1;_LLA4: _tmpD0=_tmpCE;goto
_LLA6;_LLA5: if(*((int*)_tmpC8)!= 1)goto _LLA7;_tmpCF=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmpC8)->f2;if(_tmpCF <= (void*)1)goto _LLA7;if(*((int*)_tmpCF)!= 0)goto _LLA7;
_tmpD0=((struct Cyc_Absyn_Global_b_struct*)_tmpCF)->f1;_LLA6: if(!_tmpD0->escapes){
int found=0;{struct Cyc_List_List*_tmpD1=relns;for(0;_tmpD1 != 0;_tmpD1=_tmpD1->tl){
struct Cyc_CfFlowInfo_Reln*_tmpD2=(struct Cyc_CfFlowInfo_Reln*)_tmpD1->hd;if(
_tmpD2->vd == _tmpD0){union Cyc_CfFlowInfo_RelnOp_union _tmpD3=_tmpD2->rop;struct
Cyc_Absyn_Vardecl*_tmpD4;_LLAA: if((_tmpD3.LessNumelts).tag != 2)goto _LLAC;_tmpD4=(
_tmpD3.LessNumelts).f1;if(!(_tmpD4 == _tmpC7))goto _LLAC;_LLAB: return relns;_LLAC:;
_LLAD: continue;_LLA9:;}}}if(!found)return({struct Cyc_List_List*_tmpD5=
_region_malloc(rgn,sizeof(*_tmpD5));_tmpD5->hd=({struct Cyc_CfFlowInfo_Reln*
_tmpD6=_region_malloc(rgn,sizeof(*_tmpD6));_tmpD6->vd=_tmpD0;_tmpD6->rop=(union
Cyc_CfFlowInfo_RelnOp_union)({union Cyc_CfFlowInfo_RelnOp_union _tmpD7;(_tmpD7.LessNumelts).tag=
2;(_tmpD7.LessNumelts).f1=_tmpC7;_tmpD7;});_tmpD6;});_tmpD5->tl=relns;_tmpD5;});}
return relns;_LLA7:;_LLA8: return relns;_LL9E:;}else{return relns;}_LL9C:;_LL9D:
return relns;_LL93:;}static struct Cyc_CfFlowInfo_ConsumeInfo Cyc_NewControlFlow_may_consume_place(
struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_CfFlowInfo_ConsumeInfo cinfo,
struct Cyc_CfFlowInfo_Place*place,void*t,struct Cyc_List_List**ps){struct Cyc_CfFlowInfo_FlowEnv*
_tmpD8=env->fenv;struct Cyc_List_List*_tmpD9=Cyc_NewControlFlow_noalias_ptrs_rec(
env,place,t);if(ps != 0)*ps=_tmpD9;if(_tmpD9 == 0)return cinfo;cinfo.may_consume=((
struct Cyc_List_List*(*)(struct _RegionHandle*,int(*cmp)(struct Cyc_CfFlowInfo_Place*,
struct Cyc_CfFlowInfo_Place*),struct Cyc_List_List*a,struct Cyc_List_List*b))Cyc_List_rmerge)(
_tmpD8->r,Cyc_CfFlowInfo_place_cmp,_tmpD9,cinfo.may_consume);return cinfo;}struct
_tuple12{union Cyc_CfFlowInfo_AbsLVal_union f1;union Cyc_CfFlowInfo_FlowInfo_union
f2;};static union Cyc_CfFlowInfo_FlowInfo_union Cyc_NewControlFlow_may_consume_exp(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union inflow,
struct Cyc_Absyn_Exp*e){if(Cyc_Tcutil_is_noalias_path(e)){struct Cyc_CfFlowInfo_FlowEnv*
_tmpDA=env->fenv;union Cyc_CfFlowInfo_AbsLVal_union _tmpDC;struct _tuple7 _tmpDB=Cyc_NewControlFlow_anal_Lexp(
env,inflow,e);_tmpDC=_tmpDB.f2;{struct _tuple12 _tmpDE=({struct _tuple12 _tmpDD;
_tmpDD.f1=_tmpDC;_tmpDD.f2=inflow;_tmpDD;});union Cyc_CfFlowInfo_AbsLVal_union
_tmpDF;struct Cyc_CfFlowInfo_Place*_tmpE0;union Cyc_CfFlowInfo_FlowInfo_union
_tmpE1;struct Cyc_Dict_Dict _tmpE2;struct Cyc_List_List*_tmpE3;struct Cyc_CfFlowInfo_ConsumeInfo
_tmpE4;_LLAF: _tmpDF=_tmpDE.f1;if(((_tmpDE.f1).PlaceL).tag != 0)goto _LLB1;_tmpE0=(
_tmpDF.PlaceL).f1;_tmpE1=_tmpDE.f2;if(((_tmpDE.f2).ReachableFL).tag != 1)goto
_LLB1;_tmpE2=(_tmpE1.ReachableFL).f1;_tmpE3=(_tmpE1.ReachableFL).f2;_tmpE4=(
_tmpE1.ReachableFL).f3;_LLB0: {struct Cyc_CfFlowInfo_ConsumeInfo _tmpE5=Cyc_NewControlFlow_may_consume_place(
env,_tmpE4,_tmpE0,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,0);
return(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmpE6;(_tmpE6.ReachableFL).tag=1;(_tmpE6.ReachableFL).f1=_tmpE2;(_tmpE6.ReachableFL).f2=
_tmpE3;(_tmpE6.ReachableFL).f3=_tmpE5;_tmpE6;});}_LLB1:;_LLB2:({struct Cyc_String_pa_struct
_tmpE9;_tmpE9.tag=0;_tmpE9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(
e));{void*_tmpE7[1]={& _tmpE9};Cyc_fprintf(Cyc_stderr,({const char*_tmpE8="Oops---no location for noalias_path |%s|\n";
_tag_dyneither(_tmpE8,sizeof(char),42);}),_tag_dyneither(_tmpE7,sizeof(void*),1));}});
return inflow;_LLAE:;}}return inflow;}static struct Cyc_CfFlowInfo_ConsumeInfo Cyc_NewControlFlow_consume_place(
struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_CfFlowInfo_Place*p,struct Cyc_CfFlowInfo_ConsumeInfo
cinfo,struct Cyc_Dict_Dict outdict,struct Cyc_Position_Segment*loc){if(!((int(*)(
struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*k))Cyc_Dict_member)(cinfo.consumed,
p)){struct Cyc_CfFlowInfo_Place _tmpEB;void*_tmpEC;struct Cyc_List_List*_tmpED;
struct Cyc_CfFlowInfo_Place*_tmpEA=p;_tmpEB=*_tmpEA;_tmpEC=(void*)_tmpEB.root;
_tmpED=_tmpEB.fields;{void*rval=(void*)0;if((Cyc_Dict_lookup_bool(outdict,_tmpEC,&
rval) && Cyc_CfFlowInfo_initlevel(outdict,rval)!= (void*)0) && rval != (void*)0){
void*_tmpEE=_tmpEC;struct Cyc_Absyn_Vardecl*_tmpEF;_LLB4: if(*((int*)_tmpEE)!= 0)
goto _LLB6;_tmpEF=((struct Cyc_CfFlowInfo_VarRoot_struct*)_tmpEE)->f1;_LLB5: {
struct _dyneither_ptr _tmpF0=Cyc_Absynpp_qvar2string(_tmpEF->name);if(_tmpED == 0)({
struct Cyc_String_pa_struct _tmpF3;_tmpF3.tag=0;_tmpF3.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)_tmpF0);{void*_tmpF1[1]={& _tmpF3};Cyc_Tcutil_warn(loc,({
const char*_tmpF2="may clobber unique pointer %s";_tag_dyneither(_tmpF2,sizeof(
char),30);}),_tag_dyneither(_tmpF1,sizeof(void*),1));}});else{({struct Cyc_String_pa_struct
_tmpF6;_tmpF6.tag=0;_tmpF6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
_tmpF0);{void*_tmpF4[1]={& _tmpF6};Cyc_Tcutil_warn(loc,({const char*_tmpF5="may clobber unique pointer contained in %s";
_tag_dyneither(_tmpF5,sizeof(char),43);}),_tag_dyneither(_tmpF4,sizeof(void*),1));}});}
goto _LLB3;}_LLB6:;_LLB7:({void*_tmpF7=0;((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmpF8="warning locations not for VarRoots";
_tag_dyneither(_tmpF8,sizeof(char),35);}),_tag_dyneither(_tmpF7,sizeof(void*),0));});
_LLB3:;}}}if(env->all_changed == 0)cinfo.consumed=((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,struct Cyc_Dict_Dict,struct Cyc_CfFlowInfo_Place*))Cyc_Dict_rdelete)((
env->fenv)->r,cinfo.consumed,p);return cinfo;}static struct Cyc_CfFlowInfo_ConsumeInfo
Cyc_NewControlFlow_consume_assignment(struct Cyc_NewControlFlow_AnalEnv*env,
struct Cyc_CfFlowInfo_Place*p,struct Cyc_CfFlowInfo_ConsumeInfo cinfo,struct Cyc_Dict_Dict
outdict,struct Cyc_Absyn_Exp*e){if(Cyc_Tcutil_is_noalias_path(e)){struct Cyc_CfFlowInfo_FlowEnv*
_tmpF9=env->fenv;struct Cyc_List_List*_tmpFA=0;cinfo=Cyc_NewControlFlow_may_consume_place(
env,cinfo,p,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,(struct Cyc_List_List**)&
_tmpFA);while(_tmpFA != 0){cinfo=Cyc_NewControlFlow_consume_place(env,(struct Cyc_CfFlowInfo_Place*)((
struct Cyc_List_List*)_check_null(_tmpFA))->hd,cinfo,outdict,e->loc);_tmpFA=((
struct Cyc_List_List*)_check_null(_tmpFA))->tl;}}return cinfo;}static struct Cyc_List_List*
Cyc_NewControlFlow_noalias_ptrs_aux(struct Cyc_NewControlFlow_AnalEnv*env,struct
Cyc_CfFlowInfo_Place*p,struct Cyc_List_List*ts){struct Cyc_List_List*l=0;struct Cyc_CfFlowInfo_Place
_tmpFC;void*_tmpFD;struct Cyc_List_List*_tmpFE;struct Cyc_CfFlowInfo_Place*_tmpFB=
p;_tmpFC=*_tmpFB;_tmpFD=(void*)_tmpFC.root;_tmpFE=_tmpFC.fields;{int fld=0;for(0;
ts != 0;(fld ++,ts=ts->tl)){void*_tmpFF=(void*)ts->hd;if(Cyc_Tcutil_is_noalias_pointer(
_tmpFF)){struct Cyc_List_List*_tmp100=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(
env->r,_tmpFE,({int _tmp103[1];_tmp103[0]=fld;((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(env->r,_tag_dyneither(
_tmp103,sizeof(int),1));}));l=((struct Cyc_List_List*(*)(struct _RegionHandle*,int(*
cmp)(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*),struct Cyc_List_List*
a,struct Cyc_List_List*b))Cyc_List_rmerge)(env->r,Cyc_CfFlowInfo_place_cmp,({
struct Cyc_List_List*_tmp101=_region_malloc(env->r,sizeof(*_tmp101));_tmp101->hd=({
struct Cyc_CfFlowInfo_Place*_tmp102=_region_malloc(env->r,sizeof(*_tmp102));
_tmp102->root=(void*)_tmpFD;_tmp102->fields=_tmp100;_tmp102;});_tmp101->tl=0;
_tmp101;}),l);}else{if(Cyc_Absyn_is_aggr_type(_tmpFF)){struct Cyc_List_List*
_tmp104=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_rappend)(env->r,_tmpFE,({int _tmp107[1];_tmp107[0]=
fld;((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(
env->r,_tag_dyneither(_tmp107,sizeof(int),1));}));struct Cyc_CfFlowInfo_Place*
_tmp105=({struct Cyc_CfFlowInfo_Place*_tmp106=_region_malloc(env->r,sizeof(*
_tmp106));_tmp106->root=(void*)_tmpFD;_tmp106->fields=_tmp104;_tmp106;});l=((
struct Cyc_List_List*(*)(struct _RegionHandle*,int(*cmp)(struct Cyc_CfFlowInfo_Place*,
struct Cyc_CfFlowInfo_Place*),struct Cyc_List_List*a,struct Cyc_List_List*b))Cyc_List_rmerge)(
env->r,Cyc_CfFlowInfo_place_cmp,l,Cyc_NewControlFlow_noalias_ptrs_rec(env,
_tmp105,_tmpFF));}}}}return l;}struct _tuple13{struct Cyc_Absyn_Tqual f1;void*f2;};
static struct Cyc_List_List*Cyc_NewControlFlow_noalias_ptrs_rec(struct Cyc_NewControlFlow_AnalEnv*
env,struct Cyc_CfFlowInfo_Place*p,void*t){if(Cyc_Tcutil_is_noalias_pointer(t))
return({struct Cyc_List_List*_tmp108=_region_malloc(env->r,sizeof(*_tmp108));
_tmp108->hd=p;_tmp108->tl=0;_tmp108;});{void*_tmp109=Cyc_Tcutil_compress(t);
struct Cyc_List_List*_tmp10A;struct Cyc_List_List*_tmp10B;struct Cyc_Absyn_AggrInfo
_tmp10C;union Cyc_Absyn_AggrInfoU_union _tmp10D;struct Cyc_Absyn_Aggrdecl**_tmp10E;
struct Cyc_List_List*_tmp10F;struct Cyc_Absyn_AggrInfo _tmp110;union Cyc_Absyn_AggrInfoU_union
_tmp111;struct Cyc_Absyn_TunionFieldInfo _tmp112;union Cyc_Absyn_TunionFieldInfoU_union
_tmp113;struct Cyc_List_List*_tmp114;_LLB9: if(_tmp109 <= (void*)4)goto _LLC5;if(*((
int*)_tmp109)!= 9)goto _LLBB;_tmp10A=((struct Cyc_Absyn_TupleType_struct*)_tmp109)->f1;
_LLBA: {struct Cyc_List_List*_tmp115=0;while(_tmp10A != 0){_tmp115=({struct Cyc_List_List*
_tmp116=_region_malloc(env->r,sizeof(*_tmp116));_tmp116->hd=(void*)(*((struct
_tuple13*)_tmp10A->hd)).f2;_tmp116->tl=_tmp115;_tmp116;});_tmp10A=_tmp10A->tl;}
_tmp115=Cyc_List_imp_rev(_tmp115);return Cyc_NewControlFlow_noalias_ptrs_aux(env,
p,_tmp115);}_LLBB: if(*((int*)_tmp109)!= 11)goto _LLBD;_tmp10B=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp109)->f2;_LLBC: {struct Cyc_List_List*_tmp117=0;while(_tmp10B != 0){_tmp117=({
struct Cyc_List_List*_tmp118=_region_malloc(env->r,sizeof(*_tmp118));_tmp118->hd=(
void*)((void*)((struct Cyc_Absyn_Aggrfield*)_tmp10B->hd)->type);_tmp118->tl=
_tmp117;_tmp118;});_tmp10B=_tmp10B->tl;}_tmp117=Cyc_List_imp_rev(_tmp117);return
Cyc_NewControlFlow_noalias_ptrs_aux(env,p,_tmp117);}_LLBD: if(*((int*)_tmp109)!= 
10)goto _LLBF;_tmp10C=((struct Cyc_Absyn_AggrType_struct*)_tmp109)->f1;_tmp10D=
_tmp10C.aggr_info;if((((((struct Cyc_Absyn_AggrType_struct*)_tmp109)->f1).aggr_info).KnownAggr).tag
!= 1)goto _LLBF;_tmp10E=(_tmp10D.KnownAggr).f1;_tmp10F=_tmp10C.targs;_LLBE: if((*
_tmp10E)->impl == 0)return 0;else{struct Cyc_List_List*_tmp119=0;{struct
_RegionHandle _tmp11A=_new_region("rgn");struct _RegionHandle*rgn=& _tmp11A;
_push_region(rgn);{struct Cyc_List_List*_tmp11B=((struct Cyc_List_List*(*)(struct
_RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(rgn,rgn,(*_tmp10E)->tvs,_tmp10F);struct Cyc_List_List*_tmp11C=((
struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmp10E)->impl))->fields;while(
_tmp11C != 0){_tmp119=({struct Cyc_List_List*_tmp11D=_region_malloc(env->r,sizeof(*
_tmp11D));_tmp11D->hd=(void*)Cyc_Tcutil_rsubstitute(rgn,_tmp11B,(void*)((struct
Cyc_Absyn_Aggrfield*)_tmp11C->hd)->type);_tmp11D->tl=_tmp119;_tmp11D;});_tmp11C=
_tmp11C->tl;}};_pop_region(rgn);}_tmp119=Cyc_List_imp_rev(_tmp119);return Cyc_NewControlFlow_noalias_ptrs_aux(
env,p,_tmp119);}_LLBF: if(*((int*)_tmp109)!= 10)goto _LLC1;_tmp110=((struct Cyc_Absyn_AggrType_struct*)
_tmp109)->f1;_tmp111=_tmp110.aggr_info;if((((((struct Cyc_Absyn_AggrType_struct*)
_tmp109)->f1).aggr_info).UnknownAggr).tag != 0)goto _LLC1;_LLC0:({void*_tmp11E=0;((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const
char*_tmp11F="got unknown aggr in noalias_ptrs_rec";_tag_dyneither(_tmp11F,
sizeof(char),37);}),_tag_dyneither(_tmp11E,sizeof(void*),0));});_LLC1: if(*((int*)
_tmp109)!= 2)goto _LLC3;_LLC2: if(Cyc_Tcutil_is_noalias_pointer_or_aggr(t))return({
struct Cyc_List_List*_tmp120=_region_malloc(env->r,sizeof(*_tmp120));_tmp120->hd=
p;_tmp120->tl=0;_tmp120;});else{return 0;}_LLC3: if(*((int*)_tmp109)!= 3)goto _LLC5;
_tmp112=((struct Cyc_Absyn_TunionFieldType_struct*)_tmp109)->f1;_tmp113=_tmp112.field_info;
_tmp114=_tmp112.targs;_LLC4: {union Cyc_Absyn_TunionFieldInfoU_union _tmp121=
_tmp113;struct Cyc_Absyn_Tuniondecl*_tmp122;struct Cyc_Absyn_Tunionfield*_tmp123;
_LLC8: if((_tmp121.UnknownTunionfield).tag != 0)goto _LLCA;_LLC9:({void*_tmp124=0;((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const
char*_tmp125="got unknown tunion field in noalias_ptrs_rec";_tag_dyneither(
_tmp125,sizeof(char),45);}),_tag_dyneither(_tmp124,sizeof(void*),0));});_LLCA:
if((_tmp121.KnownTunionfield).tag != 1)goto _LLC7;_tmp122=(_tmp121.KnownTunionfield).f1;
_tmp123=(_tmp121.KnownTunionfield).f2;_LLCB: {struct Cyc_List_List*_tmp126=0;{
struct _RegionHandle _tmp127=_new_region("rgn");struct _RegionHandle*rgn=& _tmp127;
_push_region(rgn);{struct Cyc_List_List*_tmp128=((struct Cyc_List_List*(*)(struct
_RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(rgn,rgn,_tmp122->tvs,_tmp114);struct Cyc_List_List*_tmp129=
_tmp123->typs;while(_tmp129 != 0){_tmp126=({struct Cyc_List_List*_tmp12A=
_region_malloc(env->r,sizeof(*_tmp12A));_tmp12A->hd=(void*)Cyc_Tcutil_rsubstitute(
rgn,_tmp128,(*((struct _tuple13*)_tmp129->hd)).f2);_tmp12A->tl=_tmp126;_tmp12A;});
_tmp129=_tmp129->tl;}};_pop_region(rgn);}_tmp126=Cyc_List_imp_rev(_tmp126);
return Cyc_NewControlFlow_noalias_ptrs_aux(env,p,_tmp126);}_LLC7:;}_LLC5:;_LLC6:
return 0;_LLB8:;}}static struct _tuple5 Cyc_NewControlFlow_do_assign(struct Cyc_CfFlowInfo_FlowEnv*
fenv,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union
outflow,struct Cyc_Absyn_Exp*lexp,union Cyc_CfFlowInfo_AbsLVal_union lval,struct Cyc_Absyn_Exp*
rexp,void*rval,struct Cyc_Position_Segment*loc){outflow=Cyc_CfFlowInfo_consume_unique_rvals(
loc,outflow);{union Cyc_CfFlowInfo_FlowInfo_union _tmp12B=outflow;struct Cyc_Dict_Dict
_tmp12C;struct Cyc_List_List*_tmp12D;struct Cyc_CfFlowInfo_ConsumeInfo _tmp12E;
_LLCD: if((_tmp12B.BottomFL).tag != 0)goto _LLCF;_LLCE: return({struct _tuple5 _tmp12F;
_tmp12F.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp130;(_tmp130.BottomFL).tag=0;_tmp130;});_tmp12F.f2=rval;_tmp12F;});_LLCF: if((
_tmp12B.ReachableFL).tag != 1)goto _LLCC;_tmp12C=(_tmp12B.ReachableFL).f1;_tmp12D=(
_tmp12B.ReachableFL).f2;_tmp12E=(_tmp12B.ReachableFL).f3;_LLD0: {union Cyc_CfFlowInfo_AbsLVal_union
_tmp131=lval;struct Cyc_CfFlowInfo_Place*_tmp132;_LLD2: if((_tmp131.PlaceL).tag != 
0)goto _LLD4;_tmp132=(_tmp131.PlaceL).f1;_LLD3: _tmp12E=Cyc_NewControlFlow_consume_assignment(
env,_tmp132,_tmp12E,_tmp12C,lexp);_tmp12C=Cyc_CfFlowInfo_assign_place(fenv,loc,
_tmp12C,env->all_changed,_tmp132,rval);_tmp12D=Cyc_CfFlowInfo_reln_assign_exp(
fenv->r,_tmp12D,lexp,rexp);outflow=(union Cyc_CfFlowInfo_FlowInfo_union)({union
Cyc_CfFlowInfo_FlowInfo_union _tmp133;(_tmp133.ReachableFL).tag=1;(_tmp133.ReachableFL).f1=
_tmp12C;(_tmp133.ReachableFL).f2=_tmp12D;(_tmp133.ReachableFL).f3=_tmp12E;
_tmp133;});Cyc_NewControlFlow_update_tryflow(env,outflow);return({struct _tuple5
_tmp134;_tmp134.f1=outflow;_tmp134.f2=rval;_tmp134;});_LLD4: if((_tmp131.UnknownL).tag
!= 1)goto _LLD1;_LLD5: return({struct _tuple5 _tmp135;_tmp135.f1=Cyc_NewControlFlow_use_Rval(
env,rexp->loc,outflow,rval);_tmp135.f2=rval;_tmp135;});_LLD1:;}_LLCC:;}}struct
_tuple14{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};static struct _tuple5 Cyc_NewControlFlow_anal_Rexp(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union inflow,
struct Cyc_Absyn_Exp*e){struct Cyc_CfFlowInfo_FlowEnv*_tmp136=env->fenv;struct Cyc_Dict_Dict
d;struct Cyc_List_List*relns;struct Cyc_CfFlowInfo_ConsumeInfo cinfo;{union Cyc_CfFlowInfo_FlowInfo_union
_tmp137=inflow;struct Cyc_Dict_Dict _tmp138;struct Cyc_List_List*_tmp139;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp13A;_LLD7: if((_tmp137.BottomFL).tag != 0)goto _LLD9;_LLD8: return({struct
_tuple5 _tmp13B;_tmp13B.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp13C;(_tmp13C.BottomFL).tag=0;_tmp13C;});_tmp13B.f2=(void*)_tmp136->unknown_all;
_tmp13B;});_LLD9: if((_tmp137.ReachableFL).tag != 1)goto _LLD6;_tmp138=(_tmp137.ReachableFL).f1;
_tmp139=(_tmp137.ReachableFL).f2;_tmp13A=(_tmp137.ReachableFL).f3;_LLDA: d=
_tmp138;relns=_tmp139;cinfo=_tmp13A;_LLD6:;}{void*_tmp13D=(void*)e->r;struct Cyc_Absyn_Exp*
_tmp13E;void*_tmp13F;struct Cyc_Absyn_Exp*_tmp140;struct Cyc_Absyn_Exp*_tmp141;
struct Cyc_Absyn_Exp*_tmp142;union Cyc_Absyn_Cnst_union _tmp143;union Cyc_Absyn_Cnst_union
_tmp144;int _tmp145;union Cyc_Absyn_Cnst_union _tmp146;void*_tmp147;struct Cyc_List_List*
_tmp148;void*_tmp149;void*_tmp14A;struct Cyc_Absyn_Vardecl*_tmp14B;void*_tmp14C;
struct Cyc_Absyn_Vardecl*_tmp14D;void*_tmp14E;struct Cyc_Absyn_Vardecl*_tmp14F;
void*_tmp150;struct Cyc_List_List*_tmp151;struct Cyc_Absyn_Exp*_tmp152;struct Cyc_Absyn_Exp*
_tmp153;struct Cyc_Core_Opt*_tmp154;struct Cyc_Core_Opt _tmp155;struct Cyc_Absyn_Exp*
_tmp156;struct Cyc_Absyn_Exp*_tmp157;struct Cyc_Core_Opt*_tmp158;struct Cyc_Absyn_Exp*
_tmp159;struct Cyc_Absyn_Exp*_tmp15A;struct Cyc_Absyn_Exp*_tmp15B;struct Cyc_Absyn_Exp*
_tmp15C;struct Cyc_Absyn_Exp*_tmp15D;struct Cyc_List_List*_tmp15E;struct Cyc_Absyn_MallocInfo
_tmp15F;int _tmp160;struct Cyc_Absyn_Exp*_tmp161;void**_tmp162;struct Cyc_Absyn_Exp*
_tmp163;int _tmp164;struct Cyc_Absyn_Exp*_tmp165;struct Cyc_Absyn_Exp*_tmp166;
struct Cyc_Absyn_Exp*_tmp167;struct Cyc_Absyn_Exp*_tmp168;struct Cyc_Absyn_Exp*
_tmp169;struct Cyc_Absyn_Exp*_tmp16A;struct Cyc_Absyn_Exp*_tmp16B;struct
_dyneither_ptr*_tmp16C;struct Cyc_Absyn_Exp*_tmp16D;struct _dyneither_ptr*_tmp16E;
struct Cyc_Absyn_Exp*_tmp16F;struct Cyc_Absyn_Exp*_tmp170;struct Cyc_Absyn_Exp*
_tmp171;struct Cyc_Absyn_Exp*_tmp172;struct Cyc_Absyn_Exp*_tmp173;struct Cyc_Absyn_Exp*
_tmp174;struct Cyc_Absyn_Exp*_tmp175;struct Cyc_Absyn_Exp*_tmp176;struct Cyc_Absyn_Exp*
_tmp177;struct Cyc_List_List*_tmp178;struct Cyc_Absyn_Tuniondecl*_tmp179;struct Cyc_List_List*
_tmp17A;struct Cyc_List_List*_tmp17B;struct Cyc_List_List*_tmp17C;struct Cyc_Absyn_Aggrdecl*
_tmp17D;struct Cyc_Absyn_Aggrdecl _tmp17E;struct Cyc_Absyn_AggrdeclImpl*_tmp17F;
struct Cyc_Absyn_AggrdeclImpl _tmp180;struct Cyc_List_List*_tmp181;struct Cyc_List_List*
_tmp182;struct Cyc_Absyn_Vardecl*_tmp183;struct Cyc_Absyn_Exp*_tmp184;struct Cyc_Absyn_Exp*
_tmp185;int _tmp186;struct Cyc_Absyn_Stmt*_tmp187;void*_tmp188;_LLDC: if(*((int*)
_tmp13D)!= 15)goto _LLDE;_tmp13E=((struct Cyc_Absyn_Cast_e_struct*)_tmp13D)->f2;
_tmp13F=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp13D)->f4;if((int)_tmp13F != 2)
goto _LLDE;_LLDD: {union Cyc_CfFlowInfo_FlowInfo_union _tmp18A;void*_tmp18B;struct
_tuple5 _tmp189=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp13E);_tmp18A=_tmp189.f1;
_tmp18B=_tmp189.f2;{union Cyc_CfFlowInfo_FlowInfo_union _tmp18D;void*_tmp18E;
struct _tuple5 _tmp18C=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp18A,_tmp13E,
_tmp18B);_tmp18D=_tmp18C.f1;_tmp18E=_tmp18C.f2;return({struct _tuple5 _tmp18F;
_tmp18F.f1=_tmp18D;_tmp18F.f2=_tmp18B;_tmp18F;});}}_LLDE: if(*((int*)_tmp13D)!= 
15)goto _LLE0;_tmp140=((struct Cyc_Absyn_Cast_e_struct*)_tmp13D)->f2;_LLDF: _tmp141=
_tmp140;goto _LLE1;_LLE0: if(*((int*)_tmp13D)!= 13)goto _LLE2;_tmp141=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmp13D)->f1;_LLE1: _tmp142=_tmp141;goto _LLE3;_LLE2: if(*((int*)_tmp13D)!= 14)goto
_LLE4;_tmp142=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp13D)->f1;_LLE3: return
Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp142);_LLE4: if(*((int*)_tmp13D)!= 0)
goto _LLE6;_tmp143=((struct Cyc_Absyn_Const_e_struct*)_tmp13D)->f1;if(((((struct
Cyc_Absyn_Const_e_struct*)_tmp13D)->f1).Null_c).tag != 6)goto _LLE6;_LLE5: goto
_LLE7;_LLE6: if(*((int*)_tmp13D)!= 0)goto _LLE8;_tmp144=((struct Cyc_Absyn_Const_e_struct*)
_tmp13D)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)_tmp13D)->f1).Int_c).tag != 2)
goto _LLE8;_tmp145=(_tmp144.Int_c).f2;if(_tmp145 != 0)goto _LLE8;_LLE7: return({
struct _tuple5 _tmp190;_tmp190.f1=inflow;_tmp190.f2=(void*)0;_tmp190;});_LLE8: if(*((
int*)_tmp13D)!= 0)goto _LLEA;_tmp146=((struct Cyc_Absyn_Const_e_struct*)_tmp13D)->f1;
if(((((struct Cyc_Absyn_Const_e_struct*)_tmp13D)->f1).Int_c).tag != 2)goto _LLEA;
_LLE9: goto _LLEB;_LLEA: if(*((int*)_tmp13D)!= 1)goto _LLEC;_tmp147=(void*)((struct
Cyc_Absyn_Var_e_struct*)_tmp13D)->f2;if(_tmp147 <= (void*)1)goto _LLEC;if(*((int*)
_tmp147)!= 1)goto _LLEC;_LLEB: return({struct _tuple5 _tmp191;_tmp191.f1=inflow;
_tmp191.f2=(void*)1;_tmp191;});_LLEC: if(*((int*)_tmp13D)!= 32)goto _LLEE;_tmp148=((
struct Cyc_Absyn_Tunion_e_struct*)_tmp13D)->f1;if(_tmp148 != 0)goto _LLEE;_LLED:
goto _LLEF;_LLEE: if(*((int*)_tmp13D)!= 0)goto _LLF0;_LLEF: goto _LLF1;_LLF0: if(*((
int*)_tmp13D)!= 19)goto _LLF2;_LLF1: goto _LLF3;_LLF2: if(*((int*)_tmp13D)!= 18)goto
_LLF4;_LLF3: goto _LLF5;_LLF4: if(*((int*)_tmp13D)!= 20)goto _LLF6;_LLF5: goto _LLF7;
_LLF6: if(*((int*)_tmp13D)!= 21)goto _LLF8;_LLF7: goto _LLF9;_LLF8: if(*((int*)
_tmp13D)!= 34)goto _LLFA;_LLF9: goto _LLFB;_LLFA: if(*((int*)_tmp13D)!= 33)goto _LLFC;
_LLFB: return({struct _tuple5 _tmp192;_tmp192.f1=inflow;_tmp192.f2=(void*)_tmp136->unknown_all;
_tmp192;});_LLFC: if(*((int*)_tmp13D)!= 1)goto _LLFE;_tmp149=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp13D)->f2;if(_tmp149 <= (void*)1)goto _LLFE;if(*((int*)_tmp149)!= 0)goto _LLFE;
_LLFD: return({struct _tuple5 _tmp193;_tmp193.f1=inflow;_tmp193.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmp136,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,(void*)_tmp136->unknown_all);
_tmp193;});_LLFE: if(*((int*)_tmp13D)!= 1)goto _LL100;_tmp14A=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp13D)->f2;if(_tmp14A <= (void*)1)goto _LL100;if(*((int*)_tmp14A)!= 2)goto _LL100;
_tmp14B=((struct Cyc_Absyn_Param_b_struct*)_tmp14A)->f1;_LLFF: _tmp14D=_tmp14B;
goto _LL101;_LL100: if(*((int*)_tmp13D)!= 1)goto _LL102;_tmp14C=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp13D)->f2;if(_tmp14C <= (void*)1)goto _LL102;if(*((int*)_tmp14C)!= 3)goto _LL102;
_tmp14D=((struct Cyc_Absyn_Local_b_struct*)_tmp14C)->f1;_LL101: _tmp14F=_tmp14D;
goto _LL103;_LL102: if(*((int*)_tmp13D)!= 1)goto _LL104;_tmp14E=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp13D)->f2;if(_tmp14E <= (void*)1)goto _LL104;if(*((int*)_tmp14E)!= 4)goto _LL104;
_tmp14F=((struct Cyc_Absyn_Pat_b_struct*)_tmp14E)->f1;_LL103: inflow=Cyc_NewControlFlow_may_consume_exp(
env,inflow,e);return({struct _tuple5 _tmp194;_tmp194.f1=inflow;_tmp194.f2=Cyc_Dict_lookup(
d,(void*)({struct Cyc_CfFlowInfo_VarRoot_struct*_tmp195=_region_malloc(env->r,
sizeof(*_tmp195));_tmp195[0]=({struct Cyc_CfFlowInfo_VarRoot_struct _tmp196;
_tmp196.tag=0;_tmp196.f1=_tmp14F;_tmp196;});_tmp195;}));_tmp194;});_LL104: if(*((
int*)_tmp13D)!= 3)goto _LL106;_tmp150=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp13D)->f1;_tmp151=((struct Cyc_Absyn_Primop_e_struct*)_tmp13D)->f2;_LL105: {
union Cyc_CfFlowInfo_FlowInfo_union _tmp198;void*_tmp199;struct _tuple5 _tmp197=Cyc_NewControlFlow_anal_use_ints(
env,inflow,_tmp151,0);_tmp198=_tmp197.f1;_tmp199=_tmp197.f2;{void*_tmp19A=
_tmp150;_LL145: if((int)_tmp19A != 0)goto _LL147;_LL146: goto _LL148;_LL147: if((int)
_tmp19A != 2)goto _LL149;_LL148: Cyc_CfFlowInfo_check_unique_rvals(((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp151))->hd)->loc,_tmp198);goto _LL144;_LL149:;
_LL14A: _tmp198=Cyc_CfFlowInfo_readthrough_unique_rvals(((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp151))->hd)->loc,_tmp198);goto _LL144;_LL144:;}
return({struct _tuple5 _tmp19B;_tmp19B.f1=_tmp198;_tmp19B.f2=_tmp199;_tmp19B;});}
_LL106: if(*((int*)_tmp13D)!= 5)goto _LL108;_tmp152=((struct Cyc_Absyn_Increment_e_struct*)
_tmp13D)->f1;_LL107: {struct Cyc_List_List _tmp19C=({struct Cyc_List_List _tmp1AB;
_tmp1AB.hd=_tmp152;_tmp1AB.tl=0;_tmp1AB;});union Cyc_CfFlowInfo_FlowInfo_union
_tmp19E;struct _tuple5 _tmp19D=Cyc_NewControlFlow_anal_use_ints(env,inflow,(struct
Cyc_List_List*)& _tmp19C,0);_tmp19E=_tmp19D.f1;Cyc_CfFlowInfo_check_unique_rvals(
_tmp152->loc,_tmp19E);{union Cyc_CfFlowInfo_AbsLVal_union _tmp1A0;struct _tuple7
_tmp19F=Cyc_NewControlFlow_anal_Lexp(env,_tmp19E,_tmp152);_tmp1A0=_tmp19F.f2;{
struct _tuple12 _tmp1A2=({struct _tuple12 _tmp1A1;_tmp1A1.f1=_tmp1A0;_tmp1A1.f2=
_tmp19E;_tmp1A1;});union Cyc_CfFlowInfo_AbsLVal_union _tmp1A3;struct Cyc_CfFlowInfo_Place*
_tmp1A4;union Cyc_CfFlowInfo_FlowInfo_union _tmp1A5;struct Cyc_Dict_Dict _tmp1A6;
struct Cyc_List_List*_tmp1A7;struct Cyc_CfFlowInfo_ConsumeInfo _tmp1A8;_LL14C:
_tmp1A3=_tmp1A2.f1;if(((_tmp1A2.f1).PlaceL).tag != 0)goto _LL14E;_tmp1A4=(_tmp1A3.PlaceL).f1;
_tmp1A5=_tmp1A2.f2;if(((_tmp1A2.f2).ReachableFL).tag != 1)goto _LL14E;_tmp1A6=(
_tmp1A5.ReachableFL).f1;_tmp1A7=(_tmp1A5.ReachableFL).f2;_tmp1A8=(_tmp1A5.ReachableFL).f3;
_LL14D: _tmp1A7=Cyc_CfFlowInfo_reln_kill_exp(_tmp136->r,_tmp1A7,_tmp152);_tmp19E=(
union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union _tmp1A9;(
_tmp1A9.ReachableFL).tag=1;(_tmp1A9.ReachableFL).f1=Cyc_CfFlowInfo_assign_place(
_tmp136,_tmp152->loc,_tmp1A6,env->all_changed,_tmp1A4,(void*)_tmp136->unknown_all);(
_tmp1A9.ReachableFL).f2=_tmp1A7;(_tmp1A9.ReachableFL).f3=_tmp1A8;_tmp1A9;});goto
_LL14B;_LL14E:;_LL14F: goto _LL14B;_LL14B:;}return({struct _tuple5 _tmp1AA;_tmp1AA.f1=
_tmp19E;_tmp1AA.f2=(void*)_tmp136->unknown_all;_tmp1AA;});}}_LL108: if(*((int*)
_tmp13D)!= 4)goto _LL10A;_tmp153=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp13D)->f1;
_tmp154=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp13D)->f2;if(_tmp154 == 0)goto
_LL10A;_tmp155=*_tmp154;_tmp156=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp13D)->f3;
_LL109: {struct Cyc_List_List _tmp1AC=({struct Cyc_List_List _tmp1BB;_tmp1BB.hd=
_tmp156;_tmp1BB.tl=0;_tmp1BB;});struct Cyc_List_List _tmp1AD=({struct Cyc_List_List
_tmp1BA;_tmp1BA.hd=_tmp153;_tmp1BA.tl=(struct Cyc_List_List*)& _tmp1AC;_tmp1BA;});
union Cyc_CfFlowInfo_FlowInfo_union _tmp1AF;struct _tuple5 _tmp1AE=Cyc_NewControlFlow_anal_use_ints(
env,inflow,(struct Cyc_List_List*)& _tmp1AD,1);_tmp1AF=_tmp1AE.f1;{union Cyc_CfFlowInfo_AbsLVal_union
_tmp1B1;struct _tuple7 _tmp1B0=Cyc_NewControlFlow_anal_Lexp(env,_tmp1AF,_tmp153);
_tmp1B1=_tmp1B0.f2;_tmp1AF=Cyc_CfFlowInfo_consume_unique_rvals(e->loc,_tmp1AF);{
union Cyc_CfFlowInfo_FlowInfo_union _tmp1B2=_tmp1AF;struct Cyc_Dict_Dict _tmp1B3;
struct Cyc_List_List*_tmp1B4;struct Cyc_CfFlowInfo_ConsumeInfo _tmp1B5;_LL151: if((
_tmp1B2.ReachableFL).tag != 1)goto _LL153;_tmp1B3=(_tmp1B2.ReachableFL).f1;_tmp1B4=(
_tmp1B2.ReachableFL).f2;_tmp1B5=(_tmp1B2.ReachableFL).f3;_LL152:{union Cyc_CfFlowInfo_AbsLVal_union
_tmp1B6=_tmp1B1;struct Cyc_CfFlowInfo_Place*_tmp1B7;_LL156: if((_tmp1B6.PlaceL).tag
!= 0)goto _LL158;_tmp1B7=(_tmp1B6.PlaceL).f1;_LL157: _tmp1B5=Cyc_NewControlFlow_consume_assignment(
env,_tmp1B7,_tmp1B5,_tmp1B3,_tmp153);_tmp1B4=Cyc_CfFlowInfo_reln_kill_exp(
_tmp136->r,_tmp1B4,_tmp153);_tmp1B3=Cyc_CfFlowInfo_assign_place(_tmp136,_tmp153->loc,
_tmp1B3,env->all_changed,_tmp1B7,(void*)_tmp136->unknown_all);_tmp1AF=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp1B8;(_tmp1B8.ReachableFL).tag=1;(_tmp1B8.ReachableFL).f1=
_tmp1B3;(_tmp1B8.ReachableFL).f2=_tmp1B4;(_tmp1B8.ReachableFL).f3=_tmp1B5;
_tmp1B8;});goto _LL155;_LL158: if((_tmp1B6.UnknownL).tag != 1)goto _LL155;_LL159:
goto _LL155;_LL155:;}goto _LL150;_LL153:;_LL154: goto _LL150;_LL150:;}return({struct
_tuple5 _tmp1B9;_tmp1B9.f1=_tmp1AF;_tmp1B9.f2=(void*)_tmp136->unknown_all;_tmp1B9;});}}
_LL10A: if(*((int*)_tmp13D)!= 4)goto _LL10C;_tmp157=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmp13D)->f1;_tmp158=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp13D)->f2;if(
_tmp158 != 0)goto _LL10C;_tmp159=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp13D)->f3;
_LL10B: {struct Cyc_Dict_Dict*_tmp1BC=env->all_changed;union Cyc_CfFlowInfo_FlowInfo_union
_tmp1BE;union Cyc_CfFlowInfo_AbsLVal_union _tmp1BF;struct _tuple7 _tmp1BD=Cyc_NewControlFlow_anal_Lexp(
env,inflow,_tmp157);_tmp1BE=_tmp1BD.f1;_tmp1BF=_tmp1BD.f2;{union Cyc_CfFlowInfo_FlowInfo_union
_tmp1C1;void*_tmp1C2;struct _tuple5 _tmp1C0=Cyc_NewControlFlow_anal_Rexp(env,
_tmp1BE,_tmp159);_tmp1C1=_tmp1C0.f1;_tmp1C2=_tmp1C0.f2;{struct _tuple5 _tmp1C3=Cyc_NewControlFlow_do_assign(
_tmp136,env,_tmp1C1,_tmp157,_tmp1BF,_tmp159,_tmp1C2,e->loc);struct Cyc_Dict_Dict*
_tmp1C4=env->all_changed;env->all_changed=_tmp1BC;{union Cyc_CfFlowInfo_FlowInfo_union
_tmp1C6;void*_tmp1C7;struct _tuple5 _tmp1C5=Cyc_NewControlFlow_anal_Rexp(env,
inflow,_tmp159);_tmp1C6=_tmp1C5.f1;_tmp1C7=_tmp1C5.f2;{union Cyc_CfFlowInfo_FlowInfo_union
_tmp1C9;union Cyc_CfFlowInfo_AbsLVal_union _tmp1CA;struct _tuple7 _tmp1C8=Cyc_NewControlFlow_anal_Lexp(
env,_tmp1C6,_tmp157);_tmp1C9=_tmp1C8.f1;_tmp1CA=_tmp1C8.f2;{struct _tuple5 _tmp1CB=
Cyc_NewControlFlow_do_assign(_tmp136,env,_tmp1C9,_tmp157,_tmp1CA,_tmp159,_tmp1C7,
e->loc);if(_tmp1BC == 0)env->all_changed=0;else{*((struct Cyc_Dict_Dict*)
_check_null(env->all_changed))=Cyc_CfFlowInfo_union_place_set(*((struct Cyc_Dict_Dict*)
_check_null(_tmp1C4)),*((struct Cyc_Dict_Dict*)_check_null(env->all_changed)),0);}{
struct _tuple5 _tmp1CC=Cyc_CfFlowInfo_join_flow_and_rval(_tmp136,_tmp1BC,_tmp1C3,
_tmp1CB,1);return _tmp1CC;}}}}}}}_LL10C: if(*((int*)_tmp13D)!= 9)goto _LL10E;
_tmp15A=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp13D)->f1;_tmp15B=((struct Cyc_Absyn_SeqExp_e_struct*)
_tmp13D)->f2;_LL10D: {union Cyc_CfFlowInfo_FlowInfo_union _tmp1CE;void*_tmp1CF;
struct _tuple5 _tmp1CD=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp15A);_tmp1CE=
_tmp1CD.f1;_tmp1CF=_tmp1CD.f2;_tmp1CE=Cyc_CfFlowInfo_drop_unique_rvals(_tmp15A->loc,
_tmp1CE);return Cyc_NewControlFlow_anal_Rexp(env,_tmp1CE,_tmp15B);}_LL10E: if(*((
int*)_tmp13D)!= 12)goto _LL110;_tmp15C=((struct Cyc_Absyn_Throw_e_struct*)_tmp13D)->f1;
_LL10F: {union Cyc_CfFlowInfo_FlowInfo_union _tmp1D1;void*_tmp1D2;struct _tuple5
_tmp1D0=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp15C);_tmp1D1=_tmp1D0.f1;
_tmp1D2=_tmp1D0.f2;_tmp1D1=Cyc_CfFlowInfo_consume_unique_rvals(_tmp15C->loc,
_tmp1D1);Cyc_NewControlFlow_use_Rval(env,_tmp15C->loc,_tmp1D1,_tmp1D2);return({
struct _tuple5 _tmp1D3;_tmp1D3.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp1D4;(_tmp1D4.BottomFL).tag=0;_tmp1D4;});_tmp1D3.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmp136,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,(void*)_tmp136->unknown_all);
_tmp1D3;});}_LL110: if(*((int*)_tmp13D)!= 11)goto _LL112;_tmp15D=((struct Cyc_Absyn_FnCall_e_struct*)
_tmp13D)->f1;_tmp15E=((struct Cyc_Absyn_FnCall_e_struct*)_tmp13D)->f2;_LL111: {
struct _RegionHandle _tmp1D5=_new_region("temp");struct _RegionHandle*temp=& _tmp1D5;
_push_region(temp);{union Cyc_CfFlowInfo_FlowInfo_union _tmp1D8;struct Cyc_List_List*
_tmp1D9;struct _tuple11 _tmp1D7=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,
inflow,({struct Cyc_List_List*_tmp1D6=_region_malloc(temp,sizeof(*_tmp1D6));
_tmp1D6->hd=_tmp15D;_tmp1D6->tl=((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct Cyc_List_List*x))Cyc_List_rcopy)(temp,_tmp15E);_tmp1D6;}),1);_tmp1D8=
_tmp1D7.f1;_tmp1D9=_tmp1D7.f2;_tmp1D8=Cyc_CfFlowInfo_consume_unique_rvals(e->loc,
_tmp1D8);{union Cyc_CfFlowInfo_FlowInfo_union _tmp1DA=Cyc_NewControlFlow_use_Rval(
env,_tmp15D->loc,_tmp1D8,(void*)((struct Cyc_List_List*)_check_null(_tmp1D9))->hd);
_tmp1D9=_tmp1D9->tl;{struct Cyc_List_List*init_params=0;{void*_tmp1DB=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp15D->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp1DC;void*_tmp1DD;_LL15B: if(_tmp1DB <= (void*)4)goto _LL15D;if(*((int*)_tmp1DB)
!= 4)goto _LL15D;_tmp1DC=((struct Cyc_Absyn_PointerType_struct*)_tmp1DB)->f1;
_tmp1DD=(void*)_tmp1DC.elt_typ;_LL15C:{void*_tmp1DE=Cyc_Tcutil_compress(_tmp1DD);
struct Cyc_Absyn_FnInfo _tmp1DF;struct Cyc_List_List*_tmp1E0;_LL160: if(_tmp1DE <= (
void*)4)goto _LL162;if(*((int*)_tmp1DE)!= 8)goto _LL162;_tmp1DF=((struct Cyc_Absyn_FnType_struct*)
_tmp1DE)->f1;_tmp1E0=_tmp1DF.attributes;_LL161: for(0;_tmp1E0 != 0;_tmp1E0=_tmp1E0->tl){
void*_tmp1E1=(void*)_tmp1E0->hd;int _tmp1E2;_LL165: if(_tmp1E1 <= (void*)17)goto
_LL167;if(*((int*)_tmp1E1)!= 4)goto _LL167;_tmp1E2=((struct Cyc_Absyn_Initializes_att_struct*)
_tmp1E1)->f1;_LL166: init_params=({struct Cyc_List_List*_tmp1E3=_region_malloc(
temp,sizeof(*_tmp1E3));_tmp1E3->hd=(void*)_tmp1E2;_tmp1E3->tl=init_params;
_tmp1E3;});goto _LL164;_LL167:;_LL168: goto _LL164;_LL164:;}goto _LL15F;_LL162:;
_LL163:({void*_tmp1E4=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(({const char*_tmp1E5="anal_Rexp: bad function type";
_tag_dyneither(_tmp1E5,sizeof(char),29);}),_tag_dyneither(_tmp1E4,sizeof(void*),
0));});_LL15F:;}goto _LL15A;_LL15D:;_LL15E:({void*_tmp1E6=0;((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp1E7="anal_Rexp: bad function type";
_tag_dyneither(_tmp1E7,sizeof(char),29);}),_tag_dyneither(_tmp1E6,sizeof(void*),
0));});_LL15A:;}{int i=1;for(0;_tmp1D9 != 0;(((_tmp1D9=_tmp1D9->tl,_tmp15E=((
struct Cyc_List_List*)_check_null(_tmp15E))->tl)),++ i)){if(!((int(*)(struct Cyc_List_List*
l,int x))Cyc_List_memq)(init_params,i)){_tmp1DA=Cyc_NewControlFlow_use_Rval(env,((
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp15E))->hd)->loc,
_tmp1DA,(void*)_tmp1D9->hd);continue;}{union Cyc_CfFlowInfo_FlowInfo_union _tmp1E8=
_tmp1D8;struct Cyc_Dict_Dict _tmp1E9;_LL16A: if((_tmp1E8.BottomFL).tag != 0)goto
_LL16C;_LL16B: goto _LL169;_LL16C: if((_tmp1E8.ReachableFL).tag != 1)goto _LL169;
_tmp1E9=(_tmp1E8.ReachableFL).f1;_LL16D: if(Cyc_CfFlowInfo_initlevel(_tmp1E9,(
void*)_tmp1D9->hd)== (void*)0)({void*_tmp1EA=0;Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp15E))->hd)->loc,({const char*_tmp1EB="expression may not be initialized";
_tag_dyneither(_tmp1EB,sizeof(char),34);}),_tag_dyneither(_tmp1EA,sizeof(void*),
0));});{union Cyc_CfFlowInfo_FlowInfo_union _tmp1EC=_tmp1DA;struct Cyc_Dict_Dict
_tmp1ED;struct Cyc_List_List*_tmp1EE;struct Cyc_CfFlowInfo_ConsumeInfo _tmp1EF;
_LL16F: if((_tmp1EC.BottomFL).tag != 0)goto _LL171;_LL170: goto _LL16E;_LL171: if((
_tmp1EC.ReachableFL).tag != 1)goto _LL16E;_tmp1ED=(_tmp1EC.ReachableFL).f1;_tmp1EE=(
_tmp1EC.ReachableFL).f2;_tmp1EF=(_tmp1EC.ReachableFL).f3;_LL172: {struct Cyc_Dict_Dict
_tmp1F0=Cyc_CfFlowInfo_escape_deref(_tmp136,_tmp1ED,env->all_changed,(void*)
_tmp1D9->hd);{void*_tmp1F1=(void*)_tmp1D9->hd;struct Cyc_CfFlowInfo_Place*_tmp1F2;
_LL174: if(_tmp1F1 <= (void*)3)goto _LL176;if(*((int*)_tmp1F1)!= 2)goto _LL176;
_tmp1F2=((struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp1F1)->f1;_LL175:{void*
_tmp1F3=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp15E))->hd)->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp1F4;void*_tmp1F5;_LL179: if(_tmp1F3 <= (void*)4)goto _LL17B;if(*((int*)_tmp1F3)
!= 4)goto _LL17B;_tmp1F4=((struct Cyc_Absyn_PointerType_struct*)_tmp1F3)->f1;
_tmp1F5=(void*)_tmp1F4.elt_typ;_LL17A: _tmp1F0=Cyc_CfFlowInfo_assign_place(
_tmp136,((struct Cyc_Absyn_Exp*)_tmp15E->hd)->loc,_tmp1F0,env->all_changed,
_tmp1F2,Cyc_CfFlowInfo_typ_to_absrval(_tmp136,_tmp1F5,(void*)_tmp136->esc_all));
goto _LL178;_LL17B:;_LL17C:({void*_tmp1F6=0;((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp1F7="anal_Rexp:bad type for initialized arg";
_tag_dyneither(_tmp1F7,sizeof(char),39);}),_tag_dyneither(_tmp1F6,sizeof(void*),
0));});_LL178:;}goto _LL173;_LL176:;_LL177: goto _LL173;_LL173:;}_tmp1DA=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp1F8;(_tmp1F8.ReachableFL).tag=1;(_tmp1F8.ReachableFL).f1=
_tmp1F0;(_tmp1F8.ReachableFL).f2=_tmp1EE;(_tmp1F8.ReachableFL).f3=_tmp1EF;
_tmp1F8;});goto _LL16E;}_LL16E:;}goto _LL169;_LL169:;}}}if(Cyc_Tcutil_is_noreturn((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp15D->topt))->v)){struct _tuple5
_tmp1FB=({struct _tuple5 _tmp1F9;_tmp1F9.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp1FA;(_tmp1FA.BottomFL).tag=0;_tmp1FA;});
_tmp1F9.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp136,(void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,(void*)_tmp136->unknown_all);_tmp1F9;});_npop_handler(0);
return _tmp1FB;}else{struct _tuple5 _tmp1FD=({struct _tuple5 _tmp1FC;_tmp1FC.f1=
_tmp1DA;_tmp1FC.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp136,(void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,(void*)_tmp136->unknown_all);_tmp1FC;});_npop_handler(0);
return _tmp1FD;}}}};_pop_region(temp);}_LL112: if(*((int*)_tmp13D)!= 35)goto _LL114;
_tmp15F=((struct Cyc_Absyn_Malloc_e_struct*)_tmp13D)->f1;_tmp160=_tmp15F.is_calloc;
_tmp161=_tmp15F.rgn;_tmp162=_tmp15F.elt_type;_tmp163=_tmp15F.num_elts;_tmp164=
_tmp15F.fat_result;_LL113: {void*root=(void*)({struct Cyc_CfFlowInfo_MallocPt_struct*
_tmp20F=_region_malloc(_tmp136->r,sizeof(*_tmp20F));_tmp20F[0]=({struct Cyc_CfFlowInfo_MallocPt_struct
_tmp210;_tmp210.tag=1;_tmp210.f1=_tmp163;_tmp210.f2=(void*)((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);_tmp210;});_tmp20F;});struct Cyc_CfFlowInfo_Place*place=({
struct Cyc_CfFlowInfo_Place*_tmp20E=_region_malloc(_tmp136->r,sizeof(*_tmp20E));
_tmp20E->root=(void*)root;_tmp20E->fields=0;_tmp20E;});void*rval=(void*)({struct
Cyc_CfFlowInfo_AddressOf_struct*_tmp20C=_region_malloc(_tmp136->r,sizeof(*
_tmp20C));_tmp20C[0]=({struct Cyc_CfFlowInfo_AddressOf_struct _tmp20D;_tmp20D.tag=
2;_tmp20D.f1=place;_tmp20D;});_tmp20C;});void*place_val=_tmp164?(void*)1: Cyc_CfFlowInfo_typ_to_absrval(
_tmp136,*((void**)_check_null(_tmp162)),(void*)_tmp136->unknown_none);union Cyc_CfFlowInfo_FlowInfo_union
outflow;Cyc_CfFlowInfo_update_place_set(env->all_changed,place,0);if(_tmp161 != 0){
struct _RegionHandle _tmp1FE=_new_region("temp");struct _RegionHandle*temp=& _tmp1FE;
_push_region(temp);{union Cyc_CfFlowInfo_FlowInfo_union _tmp201;struct Cyc_List_List*
_tmp202;struct _tuple11 _tmp200=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,
inflow,({struct Cyc_Absyn_Exp*_tmp1FF[2];_tmp1FF[1]=_tmp163;_tmp1FF[0]=(struct Cyc_Absyn_Exp*)
_tmp161;((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(
temp,_tag_dyneither(_tmp1FF,sizeof(struct Cyc_Absyn_Exp*),2));}),0);_tmp201=
_tmp200.f1;_tmp202=_tmp200.f2;outflow=_tmp201;};_pop_region(temp);}else{union Cyc_CfFlowInfo_FlowInfo_union
_tmp204;struct _tuple5 _tmp203=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp163);
_tmp204=_tmp203.f1;outflow=_tmp204;}outflow=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp163->loc,outflow);{union Cyc_CfFlowInfo_FlowInfo_union _tmp205=outflow;struct
Cyc_Dict_Dict _tmp206;struct Cyc_List_List*_tmp207;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp208;_LL17E: if((_tmp205.BottomFL).tag != 0)goto _LL180;_LL17F: return({struct
_tuple5 _tmp209;_tmp209.f1=outflow;_tmp209.f2=rval;_tmp209;});_LL180: if((_tmp205.ReachableFL).tag
!= 1)goto _LL17D;_tmp206=(_tmp205.ReachableFL).f1;_tmp207=(_tmp205.ReachableFL).f2;
_tmp208=(_tmp205.ReachableFL).f3;_LL181: return({struct _tuple5 _tmp20A;_tmp20A.f1=(
union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union _tmp20B;(
_tmp20B.ReachableFL).tag=1;(_tmp20B.ReachableFL).f1=Cyc_Dict_insert(_tmp206,root,
place_val);(_tmp20B.ReachableFL).f2=_tmp207;(_tmp20B.ReachableFL).f3=_tmp208;
_tmp20B;});_tmp20A.f2=rval;_tmp20A;});_LL17D:;}}_LL114: if(*((int*)_tmp13D)!= 36)
goto _LL116;_tmp165=((struct Cyc_Absyn_Swap_e_struct*)_tmp13D)->f1;_tmp166=((
struct Cyc_Absyn_Swap_e_struct*)_tmp13D)->f2;_LL115: {void*left_rval;void*
right_rval;union Cyc_CfFlowInfo_FlowInfo_union outflow;{struct _RegionHandle _tmp211=
_new_region("temp");struct _RegionHandle*temp=& _tmp211;_push_region(temp);{union
Cyc_CfFlowInfo_FlowInfo_union _tmp214;struct Cyc_List_List*_tmp215;struct _tuple11
_tmp213=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,({struct Cyc_Absyn_Exp*
_tmp212[2];_tmp212[1]=_tmp166;_tmp212[0]=_tmp165;((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(temp,_tag_dyneither(
_tmp212,sizeof(struct Cyc_Absyn_Exp*),2));}),0);_tmp214=_tmp213.f1;_tmp215=
_tmp213.f2;left_rval=(void*)((struct Cyc_List_List*)_check_null(_tmp215))->hd;
right_rval=(void*)((struct Cyc_List_List*)_check_null(_tmp215->tl))->hd;outflow=
_tmp214;};_pop_region(temp);}Cyc_CfFlowInfo_readthrough_unique_rvals(e->loc,
outflow);{union Cyc_CfFlowInfo_AbsLVal_union _tmp217;struct _tuple7 _tmp216=Cyc_NewControlFlow_anal_Lexp(
env,outflow,_tmp165);_tmp217=_tmp216.f2;{union Cyc_CfFlowInfo_AbsLVal_union
_tmp219;struct _tuple7 _tmp218=Cyc_NewControlFlow_anal_Lexp(env,outflow,_tmp166);
_tmp219=_tmp218.f2;{union Cyc_CfFlowInfo_FlowInfo_union _tmp21A=outflow;struct Cyc_Dict_Dict
_tmp21B;struct Cyc_List_List*_tmp21C;struct Cyc_CfFlowInfo_ConsumeInfo _tmp21D;
_LL183: if((_tmp21A.ReachableFL).tag != 1)goto _LL185;_tmp21B=(_tmp21A.ReachableFL).f1;
_tmp21C=(_tmp21A.ReachableFL).f2;_tmp21D=(_tmp21A.ReachableFL).f3;_LL184:{union
Cyc_CfFlowInfo_AbsLVal_union _tmp21E=_tmp217;struct Cyc_CfFlowInfo_Place*_tmp21F;
_LL188: if((_tmp21E.PlaceL).tag != 0)goto _LL18A;_tmp21F=(_tmp21E.PlaceL).f1;_LL189:
_tmp21B=Cyc_CfFlowInfo_assign_place(_tmp136,_tmp165->loc,_tmp21B,env->all_changed,
_tmp21F,right_rval);goto _LL187;_LL18A: if((_tmp21E.UnknownL).tag != 1)goto _LL187;
_LL18B: goto _LL187;_LL187:;}{union Cyc_CfFlowInfo_AbsLVal_union _tmp220=_tmp219;
struct Cyc_CfFlowInfo_Place*_tmp221;_LL18D: if((_tmp220.PlaceL).tag != 0)goto _LL18F;
_tmp221=(_tmp220.PlaceL).f1;_LL18E: _tmp21B=Cyc_CfFlowInfo_assign_place(_tmp136,
_tmp166->loc,_tmp21B,env->all_changed,_tmp221,left_rval);goto _LL18C;_LL18F: if((
_tmp220.UnknownL).tag != 1)goto _LL18C;_LL190: goto _LL18C;_LL18C:;}_tmp21C=Cyc_CfFlowInfo_reln_kill_exp(
_tmp136->r,_tmp21C,_tmp165);_tmp21C=Cyc_CfFlowInfo_reln_kill_exp(_tmp136->r,
_tmp21C,_tmp166);outflow=(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp222;(_tmp222.ReachableFL).tag=1;(_tmp222.ReachableFL).f1=_tmp21B;(_tmp222.ReachableFL).f2=
_tmp21C;(_tmp222.ReachableFL).f3=_tmp21D;_tmp222;});goto _LL182;_LL185:;_LL186:
goto _LL182;_LL182:;}return({struct _tuple5 _tmp223;_tmp223.f1=outflow;_tmp223.f2=(
void*)_tmp136->unknown_all;_tmp223;});}}}_LL116: if(*((int*)_tmp13D)!= 17)goto
_LL118;_tmp167=((struct Cyc_Absyn_New_e_struct*)_tmp13D)->f1;_tmp168=((struct Cyc_Absyn_New_e_struct*)
_tmp13D)->f2;_LL117: {void*root=(void*)({struct Cyc_CfFlowInfo_MallocPt_struct*
_tmp236=_region_malloc(_tmp136->r,sizeof(*_tmp236));_tmp236[0]=({struct Cyc_CfFlowInfo_MallocPt_struct
_tmp237;_tmp237.tag=1;_tmp237.f1=_tmp168;_tmp237.f2=(void*)((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);_tmp237;});_tmp236;});struct Cyc_CfFlowInfo_Place*place=({
struct Cyc_CfFlowInfo_Place*_tmp235=_region_malloc(_tmp136->r,sizeof(*_tmp235));
_tmp235->root=(void*)root;_tmp235->fields=0;_tmp235;});void*rval=(void*)({struct
Cyc_CfFlowInfo_AddressOf_struct*_tmp233=_region_malloc(_tmp136->r,sizeof(*
_tmp233));_tmp233[0]=({struct Cyc_CfFlowInfo_AddressOf_struct _tmp234;_tmp234.tag=
2;_tmp234.f1=place;_tmp234;});_tmp233;});Cyc_CfFlowInfo_update_place_set(env->all_changed,
place,0);{union Cyc_CfFlowInfo_FlowInfo_union outflow;void*place_val;if(_tmp167 != 
0){struct _RegionHandle _tmp224=_new_region("temp");struct _RegionHandle*temp=&
_tmp224;_push_region(temp);{union Cyc_CfFlowInfo_FlowInfo_union _tmp227;struct Cyc_List_List*
_tmp228;struct _tuple11 _tmp226=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,
inflow,({struct Cyc_Absyn_Exp*_tmp225[2];_tmp225[1]=_tmp168;_tmp225[0]=(struct Cyc_Absyn_Exp*)
_tmp167;((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(
temp,_tag_dyneither(_tmp225,sizeof(struct Cyc_Absyn_Exp*),2));}),0);_tmp227=
_tmp226.f1;_tmp228=_tmp226.f2;outflow=_tmp227;place_val=(void*)((struct Cyc_List_List*)
_check_null(((struct Cyc_List_List*)_check_null(_tmp228))->tl))->hd;};_pop_region(
temp);}else{union Cyc_CfFlowInfo_FlowInfo_union _tmp22A;void*_tmp22B;struct _tuple5
_tmp229=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp168);_tmp22A=_tmp229.f1;
_tmp22B=_tmp229.f2;outflow=_tmp22A;place_val=_tmp22B;}outflow=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp168->loc,outflow);{union Cyc_CfFlowInfo_FlowInfo_union _tmp22C=outflow;struct
Cyc_Dict_Dict _tmp22D;struct Cyc_List_List*_tmp22E;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp22F;_LL192: if((_tmp22C.BottomFL).tag != 0)goto _LL194;_LL193: return({struct
_tuple5 _tmp230;_tmp230.f1=outflow;_tmp230.f2=rval;_tmp230;});_LL194: if((_tmp22C.ReachableFL).tag
!= 1)goto _LL191;_tmp22D=(_tmp22C.ReachableFL).f1;_tmp22E=(_tmp22C.ReachableFL).f2;
_tmp22F=(_tmp22C.ReachableFL).f3;_LL195: return({struct _tuple5 _tmp231;_tmp231.f1=(
union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union _tmp232;(
_tmp232.ReachableFL).tag=1;(_tmp232.ReachableFL).f1=Cyc_Dict_insert(_tmp22D,root,
place_val);(_tmp232.ReachableFL).f2=_tmp22E;(_tmp232.ReachableFL).f3=_tmp22F;
_tmp232;});_tmp231.f2=rval;_tmp231;});_LL191:;}}}_LL118: if(*((int*)_tmp13D)!= 16)
goto _LL11A;_tmp169=((struct Cyc_Absyn_Address_e_struct*)_tmp13D)->f1;_LL119: {
union Cyc_CfFlowInfo_FlowInfo_union _tmp239;struct _tuple5 _tmp238=Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmp169);_tmp239=_tmp238.f1;{struct Cyc_CfFlowInfo_ConsumeInfo _tmp23B;
struct _tuple6 _tmp23A=Cyc_CfFlowInfo_save_consume_info(env->fenv,_tmp239,0);
_tmp23B=_tmp23A.f1;{union Cyc_CfFlowInfo_FlowInfo_union _tmp23D;union Cyc_CfFlowInfo_AbsLVal_union
_tmp23E;struct _tuple7 _tmp23C=Cyc_NewControlFlow_anal_Lexp(env,inflow,_tmp169);
_tmp23D=_tmp23C.f1;_tmp23E=_tmp23C.f2;_tmp23D=Cyc_CfFlowInfo_restore_consume_info(
_tmp23D,_tmp23B);{union Cyc_CfFlowInfo_AbsLVal_union _tmp23F=_tmp23E;struct Cyc_CfFlowInfo_Place*
_tmp240;_LL197: if((_tmp23F.UnknownL).tag != 1)goto _LL199;_LL198: return({struct
_tuple5 _tmp241;_tmp241.f1=_tmp23D;_tmp241.f2=(void*)1;_tmp241;});_LL199: if((
_tmp23F.PlaceL).tag != 0)goto _LL196;_tmp240=(_tmp23F.PlaceL).f1;_LL19A: return({
struct _tuple5 _tmp242;_tmp242.f1=_tmp23D;_tmp242.f2=(void*)({struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp243=_region_malloc(env->r,sizeof(*_tmp243));_tmp243[0]=({struct Cyc_CfFlowInfo_AddressOf_struct
_tmp244;_tmp244.tag=2;_tmp244.f1=_tmp240;_tmp244;});_tmp243;});_tmp242;});_LL196:;}}}}
_LL11A: if(*((int*)_tmp13D)!= 22)goto _LL11C;_tmp16A=((struct Cyc_Absyn_Deref_e_struct*)
_tmp13D)->f1;_LL11B: {union Cyc_CfFlowInfo_FlowInfo_union _tmp246;void*_tmp247;
struct _tuple5 _tmp245=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp16A);_tmp246=
_tmp245.f1;_tmp247=_tmp245.f2;_tmp246=Cyc_CfFlowInfo_readthrough_unique_rvals(e->loc,
_tmp246);return Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp246,_tmp16A,_tmp247);}
_LL11C: if(*((int*)_tmp13D)!= 23)goto _LL11E;_tmp16B=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp13D)->f1;_tmp16C=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp13D)->f2;_LL11D: {
union Cyc_CfFlowInfo_FlowInfo_union _tmp249;void*_tmp24A;struct _tuple5 _tmp248=Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmp16B);_tmp249=_tmp248.f1;_tmp24A=_tmp248.f2;_tmp249=Cyc_CfFlowInfo_drop_unique_rvals(
e->loc,_tmp249);_tmp249=Cyc_NewControlFlow_may_consume_exp(env,_tmp249,e);if(Cyc_Absyn_is_union_type((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp16B->topt))->v))return({struct
_tuple5 _tmp24B;_tmp24B.f1=_tmp249;_tmp24B.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmp136,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,(void*)_tmp136->unknown_all);
_tmp24B;});{void*_tmp24C=_tmp24A;struct _dyneither_ptr _tmp24D;_LL19C: if(_tmp24C <= (
void*)3)goto _LL19E;if(*((int*)_tmp24C)!= 4)goto _LL19E;_tmp24D=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp24C)->f1;_LL19D: {int _tmp24E=Cyc_NewControlFlow_get_field_index((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp16B->topt))->v,_tmp16C);return({struct
_tuple5 _tmp24F;_tmp24F.f1=_tmp249;_tmp24F.f2=*((void**)
_check_dyneither_subscript(_tmp24D,sizeof(void*),_tmp24E));_tmp24F;});}_LL19E:;
_LL19F:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp250=_cycalloc(
sizeof(*_tmp250));_tmp250[0]=({struct Cyc_Core_Impossible_struct _tmp251;_tmp251.tag=
Cyc_Core_Impossible;_tmp251.f1=(struct _dyneither_ptr)({struct Cyc_String_pa_struct
_tmp254;_tmp254.tag=0;_tmp254.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(e));{void*_tmp252[1]={& _tmp254};Cyc_aprintf(({const char*
_tmp253="anal_Rexp: AggrMember: %s";_tag_dyneither(_tmp253,sizeof(char),26);}),
_tag_dyneither(_tmp252,sizeof(void*),1));}});_tmp251;});_tmp250;}));_LL19B:;}}
_LL11E: if(*((int*)_tmp13D)!= 24)goto _LL120;_tmp16D=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp13D)->f1;_tmp16E=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp13D)->f2;_LL11F: {
union Cyc_CfFlowInfo_FlowInfo_union _tmp256;void*_tmp257;struct _tuple5 _tmp255=Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmp16D);_tmp256=_tmp255.f1;_tmp257=_tmp255.f2;_tmp256=Cyc_CfFlowInfo_readthrough_unique_rvals(
e->loc,_tmp256);{union Cyc_CfFlowInfo_FlowInfo_union _tmp259;void*_tmp25A;struct
_tuple5 _tmp258=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp256,_tmp16D,_tmp257);
_tmp259=_tmp258.f1;_tmp25A=_tmp258.f2;{void*_tmp25B=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp16D->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp25C;void*_tmp25D;_LL1A1: if(_tmp25B <= (void*)4)goto _LL1A3;if(*((int*)_tmp25B)
!= 4)goto _LL1A3;_tmp25C=((struct Cyc_Absyn_PointerType_struct*)_tmp25B)->f1;
_tmp25D=(void*)_tmp25C.elt_typ;_LL1A2: if(Cyc_Absyn_is_union_type(_tmp25D))return({
struct _tuple5 _tmp25E;_tmp25E.f1=_tmp259;_tmp25E.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmp136,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,(void*)_tmp136->unknown_all);
_tmp25E;});{void*_tmp25F=_tmp25A;struct _dyneither_ptr _tmp260;_LL1A6: if(_tmp25F <= (
void*)3)goto _LL1A8;if(*((int*)_tmp25F)!= 4)goto _LL1A8;_tmp260=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp25F)->f1;_LL1A7: {int _tmp261=Cyc_NewControlFlow_get_field_index(_tmp25D,
_tmp16E);return({struct _tuple5 _tmp262;_tmp262.f1=_tmp259;_tmp262.f2=*((void**)
_check_dyneither_subscript(_tmp260,sizeof(void*),_tmp261));_tmp262;});}_LL1A8:;
_LL1A9:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp263=_cycalloc(
sizeof(*_tmp263));_tmp263[0]=({struct Cyc_Core_Impossible_struct _tmp264;_tmp264.tag=
Cyc_Core_Impossible;_tmp264.f1=({const char*_tmp265="anal_Rexp: AggrArrow";
_tag_dyneither(_tmp265,sizeof(char),21);});_tmp264;});_tmp263;}));_LL1A5:;}
_LL1A3:;_LL1A4:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp266=
_cycalloc(sizeof(*_tmp266));_tmp266[0]=({struct Cyc_Core_Impossible_struct _tmp267;
_tmp267.tag=Cyc_Core_Impossible;_tmp267.f1=({const char*_tmp268="anal_Rexp: AggrArrow ptr";
_tag_dyneither(_tmp268,sizeof(char),25);});_tmp267;});_tmp266;}));_LL1A0:;}}}
_LL120: if(*((int*)_tmp13D)!= 6)goto _LL122;_tmp16F=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp13D)->f1;_tmp170=((struct Cyc_Absyn_Conditional_e_struct*)_tmp13D)->f2;
_tmp171=((struct Cyc_Absyn_Conditional_e_struct*)_tmp13D)->f3;_LL121: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp26A;union Cyc_CfFlowInfo_FlowInfo_union _tmp26B;struct _tuple8 _tmp269=Cyc_NewControlFlow_anal_test(
env,inflow,_tmp16F);_tmp26A=_tmp269.f1;_tmp26B=_tmp269.f2;_tmp26A=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp16F->loc,_tmp26A);_tmp26B=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp16F->loc,
_tmp26B);{struct _tuple5 _tmp26C=Cyc_NewControlFlow_anal_Rexp(env,_tmp26A,_tmp170);
struct _tuple5 _tmp26D=Cyc_NewControlFlow_anal_Rexp(env,_tmp26B,_tmp171);return Cyc_CfFlowInfo_join_flow_and_rval(
_tmp136,env->all_changed,_tmp26C,_tmp26D,1);}}_LL122: if(*((int*)_tmp13D)!= 7)
goto _LL124;_tmp172=((struct Cyc_Absyn_And_e_struct*)_tmp13D)->f1;_tmp173=((struct
Cyc_Absyn_And_e_struct*)_tmp13D)->f2;_LL123: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp26F;union Cyc_CfFlowInfo_FlowInfo_union _tmp270;struct _tuple8 _tmp26E=Cyc_NewControlFlow_anal_test(
env,inflow,_tmp172);_tmp26F=_tmp26E.f1;_tmp270=_tmp26E.f2;_tmp26F=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp172->loc,_tmp26F);_tmp270=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp172->loc,
_tmp270);{union Cyc_CfFlowInfo_FlowInfo_union _tmp272;void*_tmp273;struct _tuple5
_tmp271=Cyc_NewControlFlow_anal_Rexp(env,_tmp26F,_tmp173);_tmp272=_tmp271.f1;
_tmp273=_tmp271.f2;_tmp272=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp173->loc,
_tmp272);{struct _tuple5 _tmp274=({struct _tuple5 _tmp277;_tmp277.f1=_tmp272;_tmp277.f2=
_tmp273;_tmp277;});struct _tuple5 _tmp275=({struct _tuple5 _tmp276;_tmp276.f1=
_tmp270;_tmp276.f2=(void*)((void*)0);_tmp276;});return Cyc_CfFlowInfo_join_flow_and_rval(
_tmp136,env->all_changed,_tmp274,_tmp275,0);}}}_LL124: if(*((int*)_tmp13D)!= 8)
goto _LL126;_tmp174=((struct Cyc_Absyn_Or_e_struct*)_tmp13D)->f1;_tmp175=((struct
Cyc_Absyn_Or_e_struct*)_tmp13D)->f2;_LL125: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp279;union Cyc_CfFlowInfo_FlowInfo_union _tmp27A;struct _tuple8 _tmp278=Cyc_NewControlFlow_anal_test(
env,inflow,_tmp174);_tmp279=_tmp278.f1;_tmp27A=_tmp278.f2;_tmp279=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp174->loc,_tmp279);_tmp27A=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp174->loc,
_tmp27A);{union Cyc_CfFlowInfo_FlowInfo_union _tmp27C;void*_tmp27D;struct _tuple5
_tmp27B=Cyc_NewControlFlow_anal_Rexp(env,_tmp27A,_tmp175);_tmp27C=_tmp27B.f1;
_tmp27D=_tmp27B.f2;_tmp27C=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp175->loc,
_tmp27C);{struct _tuple5 _tmp27E=({struct _tuple5 _tmp281;_tmp281.f1=_tmp27C;_tmp281.f2=
_tmp27D;_tmp281;});struct _tuple5 _tmp27F=({struct _tuple5 _tmp280;_tmp280.f1=
_tmp279;_tmp280.f2=(void*)((void*)1);_tmp280;});return Cyc_CfFlowInfo_join_flow_and_rval(
_tmp136,env->all_changed,_tmp27E,_tmp27F,0);}}}_LL126: if(*((int*)_tmp13D)!= 25)
goto _LL128;_tmp176=((struct Cyc_Absyn_Subscript_e_struct*)_tmp13D)->f1;_tmp177=((
struct Cyc_Absyn_Subscript_e_struct*)_tmp13D)->f2;_LL127: {struct _RegionHandle
_tmp282=_new_region("temp");struct _RegionHandle*temp=& _tmp282;_push_region(temp);{
union Cyc_CfFlowInfo_FlowInfo_union _tmp285;struct Cyc_List_List*_tmp286;struct
_tuple11 _tmp284=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,({struct
Cyc_Absyn_Exp*_tmp283[2];_tmp283[1]=_tmp177;_tmp283[0]=_tmp176;((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(temp,_tag_dyneither(
_tmp283,sizeof(struct Cyc_Absyn_Exp*),2));}),1);_tmp285=_tmp284.f1;_tmp286=
_tmp284.f2;_tmp285=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp177->loc,_tmp285);{
union Cyc_CfFlowInfo_FlowInfo_union _tmp287=_tmp285;{union Cyc_CfFlowInfo_FlowInfo_union
_tmp288=_tmp285;struct Cyc_Dict_Dict _tmp289;struct Cyc_List_List*_tmp28A;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp28B;_LL1AB: if((_tmp288.ReachableFL).tag != 1)goto _LL1AD;_tmp289=(_tmp288.ReachableFL).f1;
_tmp28A=(_tmp288.ReachableFL).f2;_tmp28B=(_tmp288.ReachableFL).f3;_LL1AC: if(Cyc_CfFlowInfo_initlevel(
_tmp289,(void*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)
_check_null(_tmp286))->tl))->hd)== (void*)0)({void*_tmp28C=0;Cyc_Tcutil_terr(
_tmp177->loc,({const char*_tmp28D="expression may not be initialized";
_tag_dyneither(_tmp28D,sizeof(char),34);}),_tag_dyneither(_tmp28C,sizeof(void*),
0));});{struct Cyc_List_List*_tmp28E=Cyc_NewControlFlow_add_subscript_reln(
_tmp136->r,_tmp28A,_tmp176,_tmp177);if(_tmp28A != _tmp28E)_tmp287=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp28F;(_tmp28F.ReachableFL).tag=1;(_tmp28F.ReachableFL).f1=
_tmp289;(_tmp28F.ReachableFL).f2=_tmp28E;(_tmp28F.ReachableFL).f3=_tmp28B;
_tmp28F;});goto _LL1AA;}_LL1AD:;_LL1AE: goto _LL1AA;_LL1AA:;}{void*_tmp290=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp176->topt))->v);struct Cyc_List_List*
_tmp291;struct Cyc_Absyn_PtrInfo _tmp292;struct Cyc_Absyn_PtrAtts _tmp293;struct Cyc_Absyn_Conref*
_tmp294;_LL1B0: if(_tmp290 <= (void*)4)goto _LL1B4;if(*((int*)_tmp290)!= 9)goto
_LL1B2;_tmp291=((struct Cyc_Absyn_TupleType_struct*)_tmp290)->f1;_LL1B1: {void*
_tmp295=(void*)((struct Cyc_List_List*)_check_null(_tmp286))->hd;struct
_dyneither_ptr _tmp296;_LL1B7: if(_tmp295 <= (void*)3)goto _LL1B9;if(*((int*)_tmp295)
!= 4)goto _LL1B9;_tmp296=((struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp295)->f1;
_LL1B8: _tmp287=Cyc_NewControlFlow_may_consume_exp(env,_tmp287,e);{unsigned int i=(
Cyc_Evexp_eval_const_uint_exp(_tmp177)).f1;struct _tuple5 _tmp298=({struct _tuple5
_tmp297;_tmp297.f1=_tmp287;_tmp297.f2=*((void**)_check_dyneither_subscript(
_tmp296,sizeof(void*),(int)i));_tmp297;});_npop_handler(0);return _tmp298;}_LL1B9:;
_LL1BA:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp299=_cycalloc(
sizeof(*_tmp299));_tmp299[0]=({struct Cyc_Core_Impossible_struct _tmp29A;_tmp29A.tag=
Cyc_Core_Impossible;_tmp29A.f1=({const char*_tmp29B="anal_Rexp: Subscript";
_tag_dyneither(_tmp29B,sizeof(char),21);});_tmp29A;});_tmp299;}));_LL1B6:;}
_LL1B2: if(*((int*)_tmp290)!= 4)goto _LL1B4;_tmp292=((struct Cyc_Absyn_PointerType_struct*)
_tmp290)->f1;_tmp293=_tmp292.ptr_atts;_tmp294=_tmp293.bounds;_LL1B3: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp29D;void*_tmp29E;struct _tuple5 _tmp29C=Cyc_NewControlFlow_anal_derefR(env,
inflow,_tmp285,_tmp176,(void*)((struct Cyc_List_List*)_check_null(_tmp286))->hd);
_tmp29D=_tmp29C.f1;_tmp29E=_tmp29C.f2;{union Cyc_CfFlowInfo_FlowInfo_union _tmp29F=
_tmp29D;_LL1BC: if((_tmp29F.BottomFL).tag != 0)goto _LL1BE;_LL1BD: {struct _tuple5
_tmp2A1=({struct _tuple5 _tmp2A0;_tmp2A0.f1=_tmp29D;_tmp2A0.f2=_tmp29E;_tmp2A0;});
_npop_handler(0);return _tmp2A1;}_LL1BE:;_LL1BF: {struct _tuple5 _tmp2A3=({struct
_tuple5 _tmp2A2;_tmp2A2.f1=_tmp287;_tmp2A2.f2=_tmp29E;_tmp2A2;});_npop_handler(0);
return _tmp2A3;}_LL1BB:;}}_LL1B4:;_LL1B5:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp2A4=_cycalloc(sizeof(*_tmp2A4));_tmp2A4[0]=({struct Cyc_Core_Impossible_struct
_tmp2A5;_tmp2A5.tag=Cyc_Core_Impossible;_tmp2A5.f1=({const char*_tmp2A6="anal_Rexp: Subscript -- bad type";
_tag_dyneither(_tmp2A6,sizeof(char),33);});_tmp2A5;});_tmp2A4;}));_LL1AF:;}}};
_pop_region(temp);}_LL128: if(*((int*)_tmp13D)!= 32)goto _LL12A;_tmp178=((struct
Cyc_Absyn_Tunion_e_struct*)_tmp13D)->f1;_tmp179=((struct Cyc_Absyn_Tunion_e_struct*)
_tmp13D)->f2;_LL129: if(_tmp179->is_flat){struct _RegionHandle _tmp2A7=_new_region("temp");
struct _RegionHandle*temp=& _tmp2A7;_push_region(temp);{union Cyc_CfFlowInfo_FlowInfo_union
_tmp2A9;struct Cyc_List_List*_tmp2AA;struct _tuple11 _tmp2A8=Cyc_NewControlFlow_anal_unordered_Rexps(
temp,env,inflow,_tmp178,0);_tmp2A9=_tmp2A8.f1;_tmp2AA=_tmp2A8.f2;_tmp2A9=Cyc_CfFlowInfo_consume_unique_rvals(
e->loc,_tmp2A9);for(0;(unsigned int)_tmp178;(_tmp178=_tmp178->tl,_tmp2AA=_tmp2AA->tl)){
_tmp2A9=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)_tmp178->hd)->loc,
_tmp2A9,(void*)((struct Cyc_List_List*)_check_null(_tmp2AA))->hd);}{struct _tuple5
_tmp2AC=({struct _tuple5 _tmp2AB;_tmp2AB.f1=_tmp2A9;_tmp2AB.f2=(void*)_tmp136->unknown_all;
_tmp2AB;});_npop_handler(0);return _tmp2AC;}};_pop_region(temp);}_tmp17A=_tmp178;
goto _LL12B;_LL12A: if(*((int*)_tmp13D)!= 26)goto _LL12C;_tmp17A=((struct Cyc_Absyn_Tuple_e_struct*)
_tmp13D)->f1;_LL12B: {struct _RegionHandle _tmp2AD=_new_region("temp");struct
_RegionHandle*temp=& _tmp2AD;_push_region(temp);{union Cyc_CfFlowInfo_FlowInfo_union
_tmp2AF;struct Cyc_List_List*_tmp2B0;struct _tuple11 _tmp2AE=Cyc_NewControlFlow_anal_unordered_Rexps(
temp,env,inflow,_tmp17A,0);_tmp2AF=_tmp2AE.f1;_tmp2B0=_tmp2AE.f2;_tmp2AF=Cyc_CfFlowInfo_consume_unique_rvals(
e->loc,_tmp2AF);{struct _dyneither_ptr aggrdict=({unsigned int _tmp2B5=(
unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp17A);void**
_tmp2B6=(void**)_region_malloc(env->r,_check_times(sizeof(void*),_tmp2B5));
struct _dyneither_ptr _tmp2B9=_tag_dyneither(_tmp2B6,sizeof(void*),_tmp2B5);{
unsigned int _tmp2B7=_tmp2B5;unsigned int i;for(i=0;i < _tmp2B7;i ++){_tmp2B6[i]=({
void*_tmp2B8=(void*)((struct Cyc_List_List*)_check_null(_tmp2B0))->hd;_tmp2B0=
_tmp2B0->tl;_tmp2B8;});}}_tmp2B9;});struct _tuple5 _tmp2B4=({struct _tuple5 _tmp2B1;
_tmp2B1.f1=_tmp2AF;_tmp2B1.f2=(void*)({struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp2B2=_region_malloc(env->r,sizeof(*_tmp2B2));_tmp2B2[0]=({struct Cyc_CfFlowInfo_Aggregate_struct
_tmp2B3;_tmp2B3.tag=4;_tmp2B3.f1=aggrdict;_tmp2B3;});_tmp2B2;});_tmp2B1;});
_npop_handler(0);return _tmp2B4;}};_pop_region(temp);}_LL12C: if(*((int*)_tmp13D)
!= 31)goto _LL12E;_tmp17B=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp13D)->f2;
_LL12D: {struct Cyc_List_List*fs;{void*_tmp2BA=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v);struct Cyc_List_List*_tmp2BB;_LL1C1:
if(_tmp2BA <= (void*)4)goto _LL1C3;if(*((int*)_tmp2BA)!= 11)goto _LL1C3;_tmp2BB=((
struct Cyc_Absyn_AnonAggrType_struct*)_tmp2BA)->f2;_LL1C2: fs=_tmp2BB;goto _LL1C0;
_LL1C3:;_LL1C4:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp2BC=
_cycalloc(sizeof(*_tmp2BC));_tmp2BC[0]=({struct Cyc_Core_Impossible_struct _tmp2BD;
_tmp2BD.tag=Cyc_Core_Impossible;_tmp2BD.f1=({const char*_tmp2BE="anal_Rexp:anon struct has bad type";
_tag_dyneither(_tmp2BE,sizeof(char),35);});_tmp2BD;});_tmp2BC;}));_LL1C0:;}
_tmp17C=_tmp17B;_tmp181=fs;goto _LL12F;}_LL12E: if(*((int*)_tmp13D)!= 30)goto
_LL130;_tmp17C=((struct Cyc_Absyn_Struct_e_struct*)_tmp13D)->f3;_tmp17D=((struct
Cyc_Absyn_Struct_e_struct*)_tmp13D)->f4;if(_tmp17D == 0)goto _LL130;_tmp17E=*
_tmp17D;_tmp17F=_tmp17E.impl;if(_tmp17F == 0)goto _LL130;_tmp180=*_tmp17F;_tmp181=
_tmp180.fields;_LL12F: {struct _RegionHandle _tmp2BF=_new_region("temp");struct
_RegionHandle*temp=& _tmp2BF;_push_region(temp);{union Cyc_CfFlowInfo_FlowInfo_union
_tmp2C1;struct Cyc_List_List*_tmp2C2;struct _tuple11 _tmp2C0=Cyc_NewControlFlow_anal_unordered_Rexps(
temp,env,inflow,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Exp*(*
f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_rmap)(temp,(struct Cyc_Absyn_Exp*(*)(
struct _tuple14*))Cyc_Core_snd,_tmp17C),0);_tmp2C1=_tmp2C0.f1;_tmp2C2=_tmp2C0.f2;
_tmp2C1=Cyc_CfFlowInfo_consume_unique_rvals(e->loc,_tmp2C1);{struct
_dyneither_ptr aggrdict=Cyc_CfFlowInfo_aggrfields_to_aggrdict(_tmp136,_tmp181,(
void*)_tmp136->unknown_all);{int i=0;for(0;_tmp2C2 != 0;(((_tmp2C2=_tmp2C2->tl,
_tmp17C=_tmp17C->tl)),++ i)){struct Cyc_List_List*ds=(*((struct _tuple14*)((struct
Cyc_List_List*)_check_null(_tmp17C))->hd)).f1;for(0;ds != 0;ds=ds->tl){void*
_tmp2C3=(void*)ds->hd;struct _dyneither_ptr*_tmp2C4;_LL1C6: if(*((int*)_tmp2C3)!= 
0)goto _LL1C8;_LL1C7:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp2C5=_cycalloc(sizeof(*_tmp2C5));_tmp2C5[0]=({struct Cyc_Core_Impossible_struct
_tmp2C6;_tmp2C6.tag=Cyc_Core_Impossible;_tmp2C6.f1=({const char*_tmp2C7="anal_Rexp:Struct_e";
_tag_dyneither(_tmp2C7,sizeof(char),19);});_tmp2C6;});_tmp2C5;}));_LL1C8: if(*((
int*)_tmp2C3)!= 1)goto _LL1C5;_tmp2C4=((struct Cyc_Absyn_FieldName_struct*)_tmp2C3)->f1;
_LL1C9: {int _tmp2C8=Cyc_NewControlFlow_get_field_index_fs(_tmp181,_tmp2C4);*((
void**)_check_dyneither_subscript(aggrdict,sizeof(void*),_tmp2C8))=(void*)
_tmp2C2->hd;}_LL1C5:;}}}{struct _tuple5 _tmp2CC=({struct _tuple5 _tmp2C9;_tmp2C9.f1=
_tmp2C1;_tmp2C9.f2=(void*)({struct Cyc_CfFlowInfo_Aggregate_struct*_tmp2CA=
_region_malloc(env->r,sizeof(*_tmp2CA));_tmp2CA[0]=({struct Cyc_CfFlowInfo_Aggregate_struct
_tmp2CB;_tmp2CB.tag=4;_tmp2CB.f1=aggrdict;_tmp2CB;});_tmp2CA;});_tmp2C9;});
_npop_handler(0);return _tmp2CC;}}};_pop_region(temp);}_LL130: if(*((int*)_tmp13D)
!= 30)goto _LL132;_LL131:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp2CD=_cycalloc(sizeof(*_tmp2CD));_tmp2CD[0]=({struct Cyc_Core_Impossible_struct
_tmp2CE;_tmp2CE.tag=Cyc_Core_Impossible;_tmp2CE.f1=({const char*_tmp2CF="anal_Rexp:missing aggrdeclimpl";
_tag_dyneither(_tmp2CF,sizeof(char),31);});_tmp2CE;});_tmp2CD;}));_LL132: if(*((
int*)_tmp13D)!= 28)goto _LL134;_tmp182=((struct Cyc_Absyn_Array_e_struct*)_tmp13D)->f1;
_LL133: {struct _RegionHandle _tmp2D0=_new_region("temp");struct _RegionHandle*temp=&
_tmp2D0;_push_region(temp);{struct Cyc_List_List*_tmp2D1=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_Absyn_Exp*(*f)(struct _tuple14*),struct Cyc_List_List*
x))Cyc_List_rmap)(temp,(struct Cyc_Absyn_Exp*(*)(struct _tuple14*))Cyc_Core_snd,
_tmp182);union Cyc_CfFlowInfo_FlowInfo_union _tmp2D3;struct Cyc_List_List*_tmp2D4;
struct _tuple11 _tmp2D2=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,
_tmp2D1,0);_tmp2D3=_tmp2D2.f1;_tmp2D4=_tmp2D2.f2;_tmp2D3=Cyc_CfFlowInfo_consume_unique_rvals(
e->loc,_tmp2D3);for(0;_tmp2D4 != 0;(_tmp2D4=_tmp2D4->tl,_tmp2D1=_tmp2D1->tl)){
_tmp2D3=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp2D1))->hd)->loc,_tmp2D3,(void*)_tmp2D4->hd);}{struct _tuple5
_tmp2D6=({struct _tuple5 _tmp2D5;_tmp2D5.f1=_tmp2D3;_tmp2D5.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmp136,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,(void*)_tmp136->unknown_all);
_tmp2D5;});_npop_handler(0);return _tmp2D6;}};_pop_region(temp);}_LL134: if(*((int*)
_tmp13D)!= 29)goto _LL136;_tmp183=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp13D)->f1;_tmp184=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp13D)->f2;
_tmp185=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp13D)->f3;_tmp186=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmp13D)->f4;_LL135: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp2D8;void*_tmp2D9;struct _tuple5 _tmp2D7=Cyc_NewControlFlow_anal_Rexp(env,
inflow,_tmp184);_tmp2D8=_tmp2D7.f1;_tmp2D9=_tmp2D7.f2;_tmp2D8=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp184->loc,_tmp2D8);{union Cyc_CfFlowInfo_FlowInfo_union _tmp2DA=_tmp2D8;struct
Cyc_Dict_Dict _tmp2DB;struct Cyc_List_List*_tmp2DC;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp2DD;_LL1CB: if((_tmp2DA.BottomFL).tag != 0)goto _LL1CD;_LL1CC: return({struct
_tuple5 _tmp2DE;_tmp2DE.f1=_tmp2D8;_tmp2DE.f2=(void*)_tmp136->unknown_all;_tmp2DE;});
_LL1CD: if((_tmp2DA.ReachableFL).tag != 1)goto _LL1CA;_tmp2DB=(_tmp2DA.ReachableFL).f1;
_tmp2DC=(_tmp2DA.ReachableFL).f2;_tmp2DD=(_tmp2DA.ReachableFL).f3;_LL1CE: if(Cyc_CfFlowInfo_initlevel(
_tmp2DB,_tmp2D9)== (void*)0)({void*_tmp2DF=0;Cyc_Tcutil_terr(_tmp184->loc,({
const char*_tmp2E0="expression may not be initialized";_tag_dyneither(_tmp2E0,
sizeof(char),34);}),_tag_dyneither(_tmp2DF,sizeof(void*),0));});{struct Cyc_List_List*
new_relns=_tmp2DC;comp_loop: {void*_tmp2E1=(void*)_tmp184->r;struct Cyc_Absyn_Exp*
_tmp2E2;void*_tmp2E3;struct Cyc_Absyn_Vardecl*_tmp2E4;void*_tmp2E5;struct Cyc_Absyn_Vardecl*
_tmp2E6;void*_tmp2E7;struct Cyc_Absyn_Vardecl*_tmp2E8;void*_tmp2E9;struct Cyc_Absyn_Vardecl*
_tmp2EA;union Cyc_Absyn_Cnst_union _tmp2EB;int _tmp2EC;void*_tmp2ED;struct Cyc_List_List*
_tmp2EE;struct Cyc_List_List _tmp2EF;struct Cyc_Absyn_Exp*_tmp2F0;_LL1D0: if(*((int*)
_tmp2E1)!= 15)goto _LL1D2;_tmp2E2=((struct Cyc_Absyn_Cast_e_struct*)_tmp2E1)->f2;
_LL1D1: _tmp184=_tmp2E2;goto comp_loop;_LL1D2: if(*((int*)_tmp2E1)!= 1)goto _LL1D4;
_tmp2E3=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp2E1)->f2;if(_tmp2E3 <= (void*)
1)goto _LL1D4;if(*((int*)_tmp2E3)!= 0)goto _LL1D4;_tmp2E4=((struct Cyc_Absyn_Global_b_struct*)
_tmp2E3)->f1;if(!(!_tmp2E4->escapes))goto _LL1D4;_LL1D3: _tmp2E6=_tmp2E4;goto
_LL1D5;_LL1D4: if(*((int*)_tmp2E1)!= 1)goto _LL1D6;_tmp2E5=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp2E1)->f2;if(_tmp2E5 <= (void*)1)goto _LL1D6;if(*((int*)_tmp2E5)!= 3)goto _LL1D6;
_tmp2E6=((struct Cyc_Absyn_Local_b_struct*)_tmp2E5)->f1;if(!(!_tmp2E6->escapes))
goto _LL1D6;_LL1D5: _tmp2E8=_tmp2E6;goto _LL1D7;_LL1D6: if(*((int*)_tmp2E1)!= 1)goto
_LL1D8;_tmp2E7=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp2E1)->f2;if(_tmp2E7 <= (
void*)1)goto _LL1D8;if(*((int*)_tmp2E7)!= 4)goto _LL1D8;_tmp2E8=((struct Cyc_Absyn_Pat_b_struct*)
_tmp2E7)->f1;if(!(!_tmp2E8->escapes))goto _LL1D8;_LL1D7: _tmp2EA=_tmp2E8;goto
_LL1D9;_LL1D8: if(*((int*)_tmp2E1)!= 1)goto _LL1DA;_tmp2E9=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp2E1)->f2;if(_tmp2E9 <= (void*)1)goto _LL1DA;if(*((int*)_tmp2E9)!= 2)goto _LL1DA;
_tmp2EA=((struct Cyc_Absyn_Param_b_struct*)_tmp2E9)->f1;if(!(!_tmp2EA->escapes))
goto _LL1DA;_LL1D9: new_relns=({struct Cyc_List_List*_tmp2F1=_region_malloc(env->r,
sizeof(*_tmp2F1));_tmp2F1->hd=({struct Cyc_CfFlowInfo_Reln*_tmp2F2=_region_malloc(
env->r,sizeof(*_tmp2F2));_tmp2F2->vd=_tmp183;_tmp2F2->rop=(union Cyc_CfFlowInfo_RelnOp_union)({
union Cyc_CfFlowInfo_RelnOp_union _tmp2F3;(_tmp2F3.LessVar).tag=1;(_tmp2F3.LessVar).f1=
_tmp2EA;_tmp2F3;});_tmp2F2;});_tmp2F1->tl=_tmp2DC;_tmp2F1;});goto _LL1CF;_LL1DA:
if(*((int*)_tmp2E1)!= 0)goto _LL1DC;_tmp2EB=((struct Cyc_Absyn_Const_e_struct*)
_tmp2E1)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)_tmp2E1)->f1).Int_c).tag != 2)
goto _LL1DC;_tmp2EC=(_tmp2EB.Int_c).f2;_LL1DB: new_relns=({struct Cyc_List_List*
_tmp2F4=_region_malloc(env->r,sizeof(*_tmp2F4));_tmp2F4->hd=({struct Cyc_CfFlowInfo_Reln*
_tmp2F5=_region_malloc(env->r,sizeof(*_tmp2F5));_tmp2F5->vd=_tmp183;_tmp2F5->rop=(
union Cyc_CfFlowInfo_RelnOp_union)({union Cyc_CfFlowInfo_RelnOp_union _tmp2F6;(
_tmp2F6.LessConst).tag=3;(_tmp2F6.LessConst).f1=(unsigned int)_tmp2EC;_tmp2F6;});
_tmp2F5;});_tmp2F4->tl=_tmp2DC;_tmp2F4;});goto _LL1CF;_LL1DC: if(*((int*)_tmp2E1)
!= 3)goto _LL1DE;_tmp2ED=(void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp2E1)->f1;
_tmp2EE=((struct Cyc_Absyn_Primop_e_struct*)_tmp2E1)->f2;if(_tmp2EE == 0)goto
_LL1DE;_tmp2EF=*_tmp2EE;_tmp2F0=(struct Cyc_Absyn_Exp*)_tmp2EF.hd;_LL1DD:{void*
_tmp2F7=(void*)_tmp2F0->r;void*_tmp2F8;struct Cyc_Absyn_Vardecl*_tmp2F9;void*
_tmp2FA;struct Cyc_Absyn_Vardecl*_tmp2FB;void*_tmp2FC;struct Cyc_Absyn_Vardecl*
_tmp2FD;void*_tmp2FE;struct Cyc_Absyn_Vardecl*_tmp2FF;_LL1E1: if(*((int*)_tmp2F7)
!= 1)goto _LL1E3;_tmp2F8=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp2F7)->f2;if(
_tmp2F8 <= (void*)1)goto _LL1E3;if(*((int*)_tmp2F8)!= 0)goto _LL1E3;_tmp2F9=((
struct Cyc_Absyn_Global_b_struct*)_tmp2F8)->f1;if(!(!_tmp2F9->escapes))goto _LL1E3;
_LL1E2: _tmp2FB=_tmp2F9;goto _LL1E4;_LL1E3: if(*((int*)_tmp2F7)!= 1)goto _LL1E5;
_tmp2FA=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp2F7)->f2;if(_tmp2FA <= (void*)
1)goto _LL1E5;if(*((int*)_tmp2FA)!= 3)goto _LL1E5;_tmp2FB=((struct Cyc_Absyn_Local_b_struct*)
_tmp2FA)->f1;if(!(!_tmp2FB->escapes))goto _LL1E5;_LL1E4: _tmp2FD=_tmp2FB;goto
_LL1E6;_LL1E5: if(*((int*)_tmp2F7)!= 1)goto _LL1E7;_tmp2FC=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp2F7)->f2;if(_tmp2FC <= (void*)1)goto _LL1E7;if(*((int*)_tmp2FC)!= 4)goto _LL1E7;
_tmp2FD=((struct Cyc_Absyn_Pat_b_struct*)_tmp2FC)->f1;if(!(!_tmp2FD->escapes))
goto _LL1E7;_LL1E6: _tmp2FF=_tmp2FD;goto _LL1E8;_LL1E7: if(*((int*)_tmp2F7)!= 1)goto
_LL1E9;_tmp2FE=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp2F7)->f2;if(_tmp2FE <= (
void*)1)goto _LL1E9;if(*((int*)_tmp2FE)!= 2)goto _LL1E9;_tmp2FF=((struct Cyc_Absyn_Param_b_struct*)
_tmp2FE)->f1;if(!(!_tmp2FF->escapes))goto _LL1E9;_LL1E8: new_relns=({struct Cyc_List_List*
_tmp300=_region_malloc(env->r,sizeof(*_tmp300));_tmp300->hd=({struct Cyc_CfFlowInfo_Reln*
_tmp301=_region_malloc(env->r,sizeof(*_tmp301));_tmp301->vd=_tmp183;_tmp301->rop=(
union Cyc_CfFlowInfo_RelnOp_union)({union Cyc_CfFlowInfo_RelnOp_union _tmp302;(
_tmp302.LessNumelts).tag=2;(_tmp302.LessNumelts).f1=_tmp2FF;_tmp302;});_tmp301;});
_tmp300->tl=_tmp2DC;_tmp300;});goto _LL1E0;_LL1E9:;_LL1EA: goto _LL1E0;_LL1E0:;}
goto _LL1CF;_LL1DE:;_LL1DF: goto _LL1CF;_LL1CF:;}if(_tmp2DC != new_relns)_tmp2D8=(
union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union _tmp303;(
_tmp303.ReachableFL).tag=1;(_tmp303.ReachableFL).f1=_tmp2DB;(_tmp303.ReachableFL).f2=
new_relns;(_tmp303.ReachableFL).f3=_tmp2DD;_tmp303;});{void*_tmp304=_tmp2D9;
_LL1EC: if((int)_tmp304 != 0)goto _LL1EE;_LL1ED: return({struct _tuple5 _tmp305;
_tmp305.f1=_tmp2D8;_tmp305.f2=(void*)_tmp136->unknown_all;_tmp305;});_LL1EE: if((
int)_tmp304 != 2)goto _LL1F0;_LL1EF: goto _LL1F1;_LL1F0: if((int)_tmp304 != 1)goto
_LL1F2;_LL1F1: goto _LL1F3;_LL1F2: if(_tmp304 <= (void*)3)goto _LL1F4;if(*((int*)
_tmp304)!= 2)goto _LL1F4;_LL1F3: {struct Cyc_List_List _tmp306=({struct Cyc_List_List
_tmp312;_tmp312.hd=_tmp183;_tmp312.tl=0;_tmp312;});_tmp2D8=Cyc_NewControlFlow_add_vars(
_tmp136,_tmp2D8,(struct Cyc_List_List*)& _tmp306,(void*)_tmp136->unknown_all,e->loc);{
union Cyc_CfFlowInfo_FlowInfo_union _tmp308;void*_tmp309;struct _tuple5 _tmp307=Cyc_NewControlFlow_anal_Rexp(
env,_tmp2D8,_tmp185);_tmp308=_tmp307.f1;_tmp309=_tmp307.f2;_tmp308=Cyc_CfFlowInfo_consume_unique_rvals(
_tmp185->loc,_tmp308);{union Cyc_CfFlowInfo_FlowInfo_union _tmp30A=_tmp308;struct
Cyc_Dict_Dict _tmp30B;struct Cyc_CfFlowInfo_ConsumeInfo _tmp30C;_LL1F7: if((_tmp30A.BottomFL).tag
!= 0)goto _LL1F9;_LL1F8: return({struct _tuple5 _tmp30D;_tmp30D.f1=_tmp308;_tmp30D.f2=(
void*)_tmp136->unknown_all;_tmp30D;});_LL1F9: if((_tmp30A.ReachableFL).tag != 1)
goto _LL1F6;_tmp30B=(_tmp30A.ReachableFL).f1;_tmp30C=(_tmp30A.ReachableFL).f3;
_LL1FA: if(Cyc_CfFlowInfo_initlevel(_tmp30B,_tmp309)!= (void*)2){({void*_tmp30E=0;
Cyc_Tcutil_terr(_tmp184->loc,({const char*_tmp30F="expression may not be initialized";
_tag_dyneither(_tmp30F,sizeof(char),34);}),_tag_dyneither(_tmp30E,sizeof(void*),
0));});return({struct _tuple5 _tmp310;_tmp310.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp311;(_tmp311.BottomFL).tag=0;_tmp311;});
_tmp310.f2=(void*)_tmp136->unknown_all;_tmp310;});}_LL1F6:;}_tmp2D8=_tmp308;goto
_LL1F5;}}_LL1F4:;_LL1F5: while(1){struct Cyc_List_List _tmp313=({struct Cyc_List_List
_tmp31F;_tmp31F.hd=_tmp183;_tmp31F.tl=0;_tmp31F;});_tmp2D8=Cyc_NewControlFlow_add_vars(
_tmp136,_tmp2D8,(struct Cyc_List_List*)& _tmp313,(void*)_tmp136->unknown_all,e->loc);{
union Cyc_CfFlowInfo_FlowInfo_union _tmp315;void*_tmp316;struct _tuple5 _tmp314=Cyc_NewControlFlow_anal_Rexp(
env,_tmp2D8,_tmp185);_tmp315=_tmp314.f1;_tmp316=_tmp314.f2;_tmp315=Cyc_CfFlowInfo_consume_unique_rvals(
_tmp185->loc,_tmp315);{union Cyc_CfFlowInfo_FlowInfo_union _tmp317=_tmp315;struct
Cyc_Dict_Dict _tmp318;struct Cyc_CfFlowInfo_ConsumeInfo _tmp319;_LL1FC: if((_tmp317.BottomFL).tag
!= 0)goto _LL1FE;_LL1FD: goto _LL1FB;_LL1FE: if((_tmp317.ReachableFL).tag != 1)goto
_LL1FB;_tmp318=(_tmp317.ReachableFL).f1;_tmp319=(_tmp317.ReachableFL).f3;_LL1FF:
if(Cyc_CfFlowInfo_initlevel(_tmp318,_tmp316)!= (void*)2){({void*_tmp31A=0;Cyc_Tcutil_terr(
_tmp184->loc,({const char*_tmp31B="expression may not be initialized";
_tag_dyneither(_tmp31B,sizeof(char),34);}),_tag_dyneither(_tmp31A,sizeof(void*),
0));});return({struct _tuple5 _tmp31C;_tmp31C.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp31D;(_tmp31D.BottomFL).tag=0;_tmp31D;});
_tmp31C.f2=(void*)_tmp136->unknown_all;_tmp31C;});}_LL1FB:;}{union Cyc_CfFlowInfo_FlowInfo_union
_tmp31E=Cyc_CfFlowInfo_join_flow(_tmp136,env->all_changed,_tmp2D8,_tmp315,1);if(
Cyc_CfFlowInfo_flow_lessthan_approx(_tmp31E,_tmp2D8))break;_tmp2D8=_tmp31E;}}}
return({struct _tuple5 _tmp320;_tmp320.f1=_tmp2D8;_tmp320.f2=(void*)_tmp136->unknown_all;
_tmp320;});_LL1EB:;}}_LL1CA:;}}_LL136: if(*((int*)_tmp13D)!= 38)goto _LL138;
_tmp187=((struct Cyc_Absyn_StmtExp_e_struct*)_tmp13D)->f1;_LL137: while(1){union
Cyc_CfFlowInfo_FlowInfo_union*_tmp322;struct _tuple9 _tmp321=Cyc_NewControlFlow_pre_stmt_check(
env,inflow,_tmp187);_tmp322=_tmp321.f2;inflow=*_tmp322;{void*_tmp323=(void*)
_tmp187->r;struct Cyc_Absyn_Stmt*_tmp324;struct Cyc_Absyn_Stmt*_tmp325;struct Cyc_Absyn_Decl*
_tmp326;struct Cyc_Absyn_Stmt*_tmp327;struct Cyc_Absyn_Exp*_tmp328;_LL201: if(
_tmp323 <= (void*)1)goto _LL207;if(*((int*)_tmp323)!= 1)goto _LL203;_tmp324=((
struct Cyc_Absyn_Seq_s_struct*)_tmp323)->f1;_tmp325=((struct Cyc_Absyn_Seq_s_struct*)
_tmp323)->f2;_LL202: inflow=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp324);
_tmp187=_tmp325;goto _LL200;_LL203: if(*((int*)_tmp323)!= 11)goto _LL205;_tmp326=((
struct Cyc_Absyn_Decl_s_struct*)_tmp323)->f1;_tmp327=((struct Cyc_Absyn_Decl_s_struct*)
_tmp323)->f2;_LL204: inflow=Cyc_NewControlFlow_anal_decl(env,inflow,_tmp326);
_tmp187=_tmp327;goto _LL200;_LL205: if(*((int*)_tmp323)!= 0)goto _LL207;_tmp328=((
struct Cyc_Absyn_Exp_s_struct*)_tmp323)->f1;_LL206: return Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmp328);_LL207:;_LL208:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp329=_cycalloc(sizeof(*_tmp329));_tmp329[0]=({struct Cyc_Core_Impossible_struct
_tmp32A;_tmp32A.tag=Cyc_Core_Impossible;_tmp32A.f1=({const char*_tmp32B="analyze_Rexp: ill-formed StmtExp";
_tag_dyneither(_tmp32B,sizeof(char),33);});_tmp32A;});_tmp329;}));_LL200:;}}
_LL138: if(*((int*)_tmp13D)!= 1)goto _LL13A;_tmp188=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp13D)->f2;if((int)_tmp188 != 0)goto _LL13A;_LL139: goto _LL13B;_LL13A: if(*((int*)
_tmp13D)!= 2)goto _LL13C;_LL13B: goto _LL13D;_LL13C: if(*((int*)_tmp13D)!= 10)goto
_LL13E;_LL13D: goto _LL13F;_LL13E: if(*((int*)_tmp13D)!= 37)goto _LL140;_LL13F: goto
_LL141;_LL140: if(*((int*)_tmp13D)!= 27)goto _LL142;_LL141: goto _LL143;_LL142: if(*((
int*)_tmp13D)!= 39)goto _LLDB;_LL143:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp32C=_cycalloc(sizeof(*_tmp32C));_tmp32C[0]=({struct Cyc_Core_Impossible_struct
_tmp32D;_tmp32D.tag=Cyc_Core_Impossible;_tmp32D.f1=({const char*_tmp32E="anal_Rexp, unexpected exp form";
_tag_dyneither(_tmp32E,sizeof(char),31);});_tmp32D;});_tmp32C;}));_LLDB:;}}
static struct _tuple7 Cyc_NewControlFlow_anal_derefL(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo_union inflow,struct Cyc_Absyn_Exp*e,union Cyc_CfFlowInfo_FlowInfo_union
f,void*r,struct Cyc_List_List*flds){struct Cyc_CfFlowInfo_FlowEnv*_tmp32F=env->fenv;
void*_tmp330=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
struct Cyc_Absyn_PtrInfo _tmp331;void*_tmp332;struct Cyc_Absyn_PtrAtts _tmp333;
struct Cyc_Absyn_Conref*_tmp334;struct Cyc_Absyn_Conref*_tmp335;_LL20A: if(_tmp330
<= (void*)4)goto _LL20C;if(*((int*)_tmp330)!= 4)goto _LL20C;_tmp331=((struct Cyc_Absyn_PointerType_struct*)
_tmp330)->f1;_tmp332=(void*)_tmp331.elt_typ;_tmp333=_tmp331.ptr_atts;_tmp334=
_tmp333.bounds;_tmp335=_tmp333.zero_term;_LL20B: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp336=f;struct Cyc_Dict_Dict _tmp337;struct Cyc_List_List*_tmp338;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp339;_LL20F: if((_tmp336.BottomFL).tag != 0)goto _LL211;_LL210: return({struct
_tuple7 _tmp33A;_tmp33A.f1=f;_tmp33A.f2=(union Cyc_CfFlowInfo_AbsLVal_union)({
union Cyc_CfFlowInfo_AbsLVal_union _tmp33B;(_tmp33B.UnknownL).tag=1;_tmp33B;});
_tmp33A;});_LL211: if((_tmp336.ReachableFL).tag != 1)goto _LL20E;_tmp337=(_tmp336.ReachableFL).f1;
_tmp338=(_tmp336.ReachableFL).f2;_tmp339=(_tmp336.ReachableFL).f3;_LL212: if(Cyc_Tcutil_is_bound_one(
_tmp334)){void*_tmp33C=r;struct Cyc_CfFlowInfo_Place*_tmp33D;struct Cyc_CfFlowInfo_Place
_tmp33E;void*_tmp33F;struct Cyc_List_List*_tmp340;void*_tmp341;_LL214: if((int)
_tmp33C != 1)goto _LL216;_LL215: goto _LL217;_LL216: if((int)_tmp33C != 2)goto _LL218;
_LL217:(void*)(e->annot=(void*)((void*)({struct Cyc_CfFlowInfo_NotZero_struct*
_tmp342=_cycalloc(sizeof(*_tmp342));_tmp342[0]=({struct Cyc_CfFlowInfo_NotZero_struct
_tmp343;_tmp343.tag=Cyc_CfFlowInfo_NotZero;_tmp343.f1=Cyc_CfFlowInfo_copy_relns(
Cyc_Core_heap_region,_tmp338);_tmp343;});_tmp342;})));goto _LL213;_LL218: if(
_tmp33C <= (void*)3)goto _LL21A;if(*((int*)_tmp33C)!= 2)goto _LL21A;_tmp33D=((
struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp33C)->f1;_tmp33E=*_tmp33D;_tmp33F=(
void*)_tmp33E.root;_tmp340=_tmp33E.fields;_LL219:(void*)(e->annot=(void*)((void*)({
struct Cyc_CfFlowInfo_NotZero_struct*_tmp344=_cycalloc(sizeof(*_tmp344));_tmp344[
0]=({struct Cyc_CfFlowInfo_NotZero_struct _tmp345;_tmp345.tag=Cyc_CfFlowInfo_NotZero;
_tmp345.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,_tmp338);_tmp345;});
_tmp344;})));return({struct _tuple7 _tmp346;_tmp346.f1=f;_tmp346.f2=(union Cyc_CfFlowInfo_AbsLVal_union)({
union Cyc_CfFlowInfo_AbsLVal_union _tmp347;(_tmp347.PlaceL).tag=0;(_tmp347.PlaceL).f1=({
struct Cyc_CfFlowInfo_Place*_tmp348=_region_malloc(_tmp32F->r,sizeof(*_tmp348));
_tmp348->root=(void*)_tmp33F;_tmp348->fields=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(
_tmp32F->r,_tmp340,flds);_tmp348;});_tmp347;});_tmp346;});_LL21A: if((int)_tmp33C
!= 0)goto _LL21C;_LL21B:(void*)(e->annot=(void*)((void*)Cyc_CfFlowInfo_IsZero));
return({struct _tuple7 _tmp349;_tmp349.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp34B;(_tmp34B.BottomFL).tag=0;_tmp34B;});
_tmp349.f2=(union Cyc_CfFlowInfo_AbsLVal_union)({union Cyc_CfFlowInfo_AbsLVal_union
_tmp34A;(_tmp34A.UnknownL).tag=1;_tmp34A;});_tmp349;});_LL21C: if(_tmp33C <= (void*)
3)goto _LL21E;if(*((int*)_tmp33C)!= 0)goto _LL21E;_tmp341=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)
_tmp33C)->f1;_LL21D: f=Cyc_NewControlFlow_notzero(env,inflow,f,e,_tmp341);goto
_LL21F;_LL21E:;_LL21F:(void*)(e->annot=(void*)((void*)({struct Cyc_CfFlowInfo_UnknownZ_struct*
_tmp34C=_cycalloc(sizeof(*_tmp34C));_tmp34C[0]=({struct Cyc_CfFlowInfo_UnknownZ_struct
_tmp34D;_tmp34D.tag=Cyc_CfFlowInfo_UnknownZ;_tmp34D.f1=Cyc_CfFlowInfo_copy_relns(
Cyc_Core_heap_region,_tmp338);_tmp34D;});_tmp34C;})));_LL213:;}else{(void*)(e->annot=(
void*)((void*)({struct Cyc_CfFlowInfo_UnknownZ_struct*_tmp34E=_cycalloc(sizeof(*
_tmp34E));_tmp34E[0]=({struct Cyc_CfFlowInfo_UnknownZ_struct _tmp34F;_tmp34F.tag=
Cyc_CfFlowInfo_UnknownZ;_tmp34F.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,
_tmp338);_tmp34F;});_tmp34E;})));}if(Cyc_CfFlowInfo_initlevel(_tmp337,r)== (void*)
0)({void*_tmp350=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp351="dereference of possibly uninitialized pointer";
_tag_dyneither(_tmp351,sizeof(char),46);}),_tag_dyneither(_tmp350,sizeof(void*),
0));});return({struct _tuple7 _tmp352;_tmp352.f1=f;_tmp352.f2=(union Cyc_CfFlowInfo_AbsLVal_union)({
union Cyc_CfFlowInfo_AbsLVal_union _tmp353;(_tmp353.UnknownL).tag=1;_tmp353;});
_tmp352;});_LL20E:;}_LL20C:;_LL20D:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp354=_cycalloc(sizeof(*_tmp354));_tmp354[0]=({struct Cyc_Core_Impossible_struct
_tmp355;_tmp355.tag=Cyc_Core_Impossible;_tmp355.f1=({const char*_tmp356="left deref of non-pointer-type";
_tag_dyneither(_tmp356,sizeof(char),31);});_tmp355;});_tmp354;}));_LL209:;}
static struct _tuple7 Cyc_NewControlFlow_anal_Lexp_rec(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo_union inflow,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*
flds){struct Cyc_Dict_Dict d;struct Cyc_CfFlowInfo_FlowEnv*_tmp357=env->fenv;{union
Cyc_CfFlowInfo_FlowInfo_union _tmp358=inflow;struct Cyc_Dict_Dict _tmp359;struct Cyc_List_List*
_tmp35A;_LL221: if((_tmp358.BottomFL).tag != 0)goto _LL223;_LL222: return({struct
_tuple7 _tmp35B;_tmp35B.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp35D;(_tmp35D.BottomFL).tag=0;_tmp35D;});_tmp35B.f2=(union Cyc_CfFlowInfo_AbsLVal_union)({
union Cyc_CfFlowInfo_AbsLVal_union _tmp35C;(_tmp35C.UnknownL).tag=1;_tmp35C;});
_tmp35B;});_LL223: if((_tmp358.ReachableFL).tag != 1)goto _LL220;_tmp359=(_tmp358.ReachableFL).f1;
_tmp35A=(_tmp358.ReachableFL).f2;_LL224: d=_tmp359;_LL220:;}{void*_tmp35E=(void*)
e->r;struct Cyc_Absyn_Exp*_tmp35F;struct Cyc_Absyn_Exp*_tmp360;struct Cyc_Absyn_Exp*
_tmp361;void*_tmp362;struct Cyc_Absyn_Vardecl*_tmp363;void*_tmp364;struct Cyc_Absyn_Vardecl*
_tmp365;void*_tmp366;struct Cyc_Absyn_Vardecl*_tmp367;void*_tmp368;struct Cyc_Absyn_Vardecl*
_tmp369;struct Cyc_Absyn_Exp*_tmp36A;struct _dyneither_ptr*_tmp36B;struct Cyc_Absyn_Exp*
_tmp36C;struct Cyc_Absyn_Exp*_tmp36D;struct Cyc_Absyn_Exp*_tmp36E;struct Cyc_Absyn_Exp*
_tmp36F;struct _dyneither_ptr*_tmp370;_LL226: if(*((int*)_tmp35E)!= 15)goto _LL228;
_tmp35F=((struct Cyc_Absyn_Cast_e_struct*)_tmp35E)->f2;_LL227: _tmp360=_tmp35F;
goto _LL229;_LL228: if(*((int*)_tmp35E)!= 13)goto _LL22A;_tmp360=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmp35E)->f1;_LL229: _tmp361=_tmp360;goto _LL22B;_LL22A: if(*((int*)_tmp35E)!= 14)
goto _LL22C;_tmp361=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp35E)->f1;_LL22B:
return Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,_tmp361,flds);_LL22C: if(*((int*)
_tmp35E)!= 1)goto _LL22E;_tmp362=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp35E)->f2;
if(_tmp362 <= (void*)1)goto _LL22E;if(*((int*)_tmp362)!= 2)goto _LL22E;_tmp363=((
struct Cyc_Absyn_Param_b_struct*)_tmp362)->f1;_LL22D: _tmp365=_tmp363;goto _LL22F;
_LL22E: if(*((int*)_tmp35E)!= 1)goto _LL230;_tmp364=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp35E)->f2;if(_tmp364 <= (void*)1)goto _LL230;if(*((int*)_tmp364)!= 3)goto _LL230;
_tmp365=((struct Cyc_Absyn_Local_b_struct*)_tmp364)->f1;_LL22F: _tmp367=_tmp365;
goto _LL231;_LL230: if(*((int*)_tmp35E)!= 1)goto _LL232;_tmp366=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp35E)->f2;if(_tmp366 <= (void*)1)goto _LL232;if(*((int*)_tmp366)!= 4)goto _LL232;
_tmp367=((struct Cyc_Absyn_Pat_b_struct*)_tmp366)->f1;_LL231: return({struct
_tuple7 _tmp371;_tmp371.f1=inflow;_tmp371.f2=(union Cyc_CfFlowInfo_AbsLVal_union)({
union Cyc_CfFlowInfo_AbsLVal_union _tmp372;(_tmp372.PlaceL).tag=0;(_tmp372.PlaceL).f1=({
struct Cyc_CfFlowInfo_Place*_tmp373=_region_malloc(env->r,sizeof(*_tmp373));
_tmp373->root=(void*)((void*)({struct Cyc_CfFlowInfo_VarRoot_struct*_tmp374=
_region_malloc(env->r,sizeof(*_tmp374));_tmp374[0]=({struct Cyc_CfFlowInfo_VarRoot_struct
_tmp375;_tmp375.tag=0;_tmp375.f1=_tmp367;_tmp375;});_tmp374;}));_tmp373->fields=
flds;_tmp373;});_tmp372;});_tmp371;});_LL232: if(*((int*)_tmp35E)!= 1)goto _LL234;
_tmp368=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp35E)->f2;if(_tmp368 <= (void*)
1)goto _LL234;if(*((int*)_tmp368)!= 0)goto _LL234;_tmp369=((struct Cyc_Absyn_Global_b_struct*)
_tmp368)->f1;_LL233: return({struct _tuple7 _tmp376;_tmp376.f1=inflow;_tmp376.f2=(
union Cyc_CfFlowInfo_AbsLVal_union)({union Cyc_CfFlowInfo_AbsLVal_union _tmp377;(
_tmp377.UnknownL).tag=1;_tmp377;});_tmp376;});_LL234: if(*((int*)_tmp35E)!= 24)
goto _LL236;_tmp36A=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp35E)->f1;_tmp36B=((
struct Cyc_Absyn_AggrArrow_e_struct*)_tmp35E)->f2;_LL235:{void*_tmp378=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp36A->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp379;void*_tmp37A;_LL23F: if(_tmp378 <= (void*)4)goto _LL241;if(*((int*)_tmp378)
!= 4)goto _LL241;_tmp379=((struct Cyc_Absyn_PointerType_struct*)_tmp378)->f1;
_tmp37A=(void*)_tmp379.elt_typ;_LL240: if(!Cyc_Absyn_is_union_type(_tmp37A))flds=({
struct Cyc_List_List*_tmp37B=_region_malloc(env->r,sizeof(*_tmp37B));_tmp37B->hd=(
void*)Cyc_NewControlFlow_get_field_index(_tmp37A,_tmp36B);_tmp37B->tl=flds;
_tmp37B;});goto _LL23E;_LL241:;_LL242:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp37C=_cycalloc(sizeof(*_tmp37C));_tmp37C[0]=({struct Cyc_Core_Impossible_struct
_tmp37D;_tmp37D.tag=Cyc_Core_Impossible;_tmp37D.f1=({const char*_tmp37E="anal_Rexp: AggrArrow ptr";
_tag_dyneither(_tmp37E,sizeof(char),25);});_tmp37D;});_tmp37C;}));_LL23E:;}
_tmp36C=_tmp36A;goto _LL237;_LL236: if(*((int*)_tmp35E)!= 22)goto _LL238;_tmp36C=((
struct Cyc_Absyn_Deref_e_struct*)_tmp35E)->f1;_LL237: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp380;void*_tmp381;struct _tuple5 _tmp37F=Cyc_NewControlFlow_anal_Rexp(env,
inflow,_tmp36C);_tmp380=_tmp37F.f1;_tmp381=_tmp37F.f2;_tmp380=Cyc_CfFlowInfo_readthrough_unique_rvals(
e->loc,_tmp380);return Cyc_NewControlFlow_anal_derefL(env,inflow,_tmp36C,_tmp380,
_tmp381,flds);}_LL238: if(*((int*)_tmp35E)!= 25)goto _LL23A;_tmp36D=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp35E)->f1;_tmp36E=((struct Cyc_Absyn_Subscript_e_struct*)_tmp35E)->f2;_LL239: {
void*_tmp382=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp36D->topt))->v);struct Cyc_Absyn_PtrInfo _tmp383;struct Cyc_Absyn_PtrAtts
_tmp384;struct Cyc_Absyn_Conref*_tmp385;_LL244: if(_tmp382 <= (void*)4)goto _LL248;
if(*((int*)_tmp382)!= 9)goto _LL246;_LL245: {unsigned int _tmp386=(Cyc_Evexp_eval_const_uint_exp(
_tmp36E)).f1;return Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,_tmp36D,({struct
Cyc_List_List*_tmp387=_region_malloc(env->r,sizeof(*_tmp387));_tmp387->hd=(void*)((
int)_tmp386);_tmp387->tl=flds;_tmp387;}));}_LL246: if(*((int*)_tmp382)!= 4)goto
_LL248;_tmp383=((struct Cyc_Absyn_PointerType_struct*)_tmp382)->f1;_tmp384=
_tmp383.ptr_atts;_tmp385=_tmp384.bounds;_LL247: {struct _RegionHandle _tmp388=
_new_region("temp");struct _RegionHandle*temp=& _tmp388;_push_region(temp);{union
Cyc_CfFlowInfo_FlowInfo_union _tmp38B;struct Cyc_List_List*_tmp38C;struct _tuple11
_tmp38A=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,({struct Cyc_Absyn_Exp*
_tmp389[2];_tmp389[1]=_tmp36E;_tmp389[0]=_tmp36D;((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(temp,_tag_dyneither(
_tmp389,sizeof(struct Cyc_Absyn_Exp*),2));}),1);_tmp38B=_tmp38A.f1;_tmp38C=
_tmp38A.f2;_tmp38B=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp36E->loc,_tmp38B);{
union Cyc_CfFlowInfo_FlowInfo_union _tmp38D=_tmp38B;{union Cyc_CfFlowInfo_FlowInfo_union
_tmp38E=_tmp38B;struct Cyc_Dict_Dict _tmp38F;struct Cyc_List_List*_tmp390;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp391;_LL24B: if((_tmp38E.ReachableFL).tag != 1)goto _LL24D;_tmp38F=(_tmp38E.ReachableFL).f1;
_tmp390=(_tmp38E.ReachableFL).f2;_tmp391=(_tmp38E.ReachableFL).f3;_LL24C: if(Cyc_CfFlowInfo_initlevel(
_tmp38F,(void*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)
_check_null(_tmp38C))->tl))->hd)== (void*)0)({void*_tmp392=0;Cyc_Tcutil_terr(
_tmp36E->loc,({const char*_tmp393="expression may not be initialized";
_tag_dyneither(_tmp393,sizeof(char),34);}),_tag_dyneither(_tmp392,sizeof(void*),
0));});{struct Cyc_List_List*_tmp394=Cyc_NewControlFlow_add_subscript_reln(
_tmp357->r,_tmp390,_tmp36D,_tmp36E);if(_tmp390 != _tmp394)_tmp38D=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp395;(_tmp395.ReachableFL).tag=1;(_tmp395.ReachableFL).f1=
_tmp38F;(_tmp395.ReachableFL).f2=_tmp394;(_tmp395.ReachableFL).f3=_tmp391;
_tmp395;});goto _LL24A;}_LL24D:;_LL24E: goto _LL24A;_LL24A:;}{union Cyc_CfFlowInfo_FlowInfo_union
_tmp397;union Cyc_CfFlowInfo_AbsLVal_union _tmp398;struct _tuple7 _tmp396=Cyc_NewControlFlow_anal_derefL(
env,inflow,_tmp36D,_tmp38B,(void*)((struct Cyc_List_List*)_check_null(_tmp38C))->hd,
flds);_tmp397=_tmp396.f1;_tmp398=_tmp396.f2;{union Cyc_CfFlowInfo_FlowInfo_union
_tmp399=_tmp397;_LL250: if((_tmp399.BottomFL).tag != 0)goto _LL252;_LL251: {struct
_tuple7 _tmp39B=({struct _tuple7 _tmp39A;_tmp39A.f1=_tmp397;_tmp39A.f2=_tmp398;
_tmp39A;});_npop_handler(0);return _tmp39B;}_LL252:;_LL253: {struct _tuple7 _tmp39D=({
struct _tuple7 _tmp39C;_tmp39C.f1=_tmp38D;_tmp39C.f2=_tmp398;_tmp39C;});
_npop_handler(0);return _tmp39D;}_LL24F:;}}}};_pop_region(temp);}_LL248:;_LL249:(
int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp39E=_cycalloc(sizeof(*
_tmp39E));_tmp39E[0]=({struct Cyc_Core_Impossible_struct _tmp39F;_tmp39F.tag=Cyc_Core_Impossible;
_tmp39F.f1=({const char*_tmp3A0="anal_Lexp: Subscript -- bad type";_tag_dyneither(
_tmp3A0,sizeof(char),33);});_tmp39F;});_tmp39E;}));_LL243:;}_LL23A: if(*((int*)
_tmp35E)!= 23)goto _LL23C;_tmp36F=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp35E)->f1;
_tmp370=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp35E)->f2;_LL23B: if(Cyc_Absyn_is_union_type((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp36F->topt))->v))return({struct
_tuple7 _tmp3A1;_tmp3A1.f1=inflow;_tmp3A1.f2=(union Cyc_CfFlowInfo_AbsLVal_union)({
union Cyc_CfFlowInfo_AbsLVal_union _tmp3A2;(_tmp3A2.UnknownL).tag=1;_tmp3A2;});
_tmp3A1;});return Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,_tmp36F,({struct Cyc_List_List*
_tmp3A3=_region_malloc(env->r,sizeof(*_tmp3A3));_tmp3A3->hd=(void*)Cyc_NewControlFlow_get_field_index((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp36F->topt))->v,_tmp370);_tmp3A3->tl=
flds;_tmp3A3;}));_LL23C:;_LL23D: return({struct _tuple7 _tmp3A4;_tmp3A4.f1=(union
Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union _tmp3A6;(
_tmp3A6.BottomFL).tag=0;_tmp3A6;});_tmp3A4.f2=(union Cyc_CfFlowInfo_AbsLVal_union)({
union Cyc_CfFlowInfo_AbsLVal_union _tmp3A5;(_tmp3A5.UnknownL).tag=1;_tmp3A5;});
_tmp3A4;});_LL225:;}}static struct _tuple7 Cyc_NewControlFlow_anal_Lexp(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo_union inflow,struct Cyc_Absyn_Exp*e){union Cyc_CfFlowInfo_FlowInfo_union
_tmp3A8;union Cyc_CfFlowInfo_AbsLVal_union _tmp3A9;struct _tuple7 _tmp3A7=Cyc_NewControlFlow_anal_Lexp_rec(
env,inflow,e,0);_tmp3A8=_tmp3A7.f1;_tmp3A9=_tmp3A7.f2;return({struct _tuple7
_tmp3AA;_tmp3AA.f1=_tmp3A8;_tmp3AA.f2=_tmp3A9;_tmp3AA;});}static struct _tuple8 Cyc_NewControlFlow_anal_test(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union inflow,
struct Cyc_Absyn_Exp*e){struct Cyc_CfFlowInfo_FlowEnv*_tmp3AB=env->fenv;{void*
_tmp3AC=(void*)e->r;struct Cyc_Absyn_Exp*_tmp3AD;struct Cyc_Absyn_Exp*_tmp3AE;
struct Cyc_Absyn_Exp*_tmp3AF;struct Cyc_Absyn_Exp*_tmp3B0;struct Cyc_Absyn_Exp*
_tmp3B1;struct Cyc_Absyn_Exp*_tmp3B2;struct Cyc_Absyn_Exp*_tmp3B3;struct Cyc_Absyn_Exp*
_tmp3B4;struct Cyc_Absyn_Exp*_tmp3B5;void*_tmp3B6;struct Cyc_List_List*_tmp3B7;
struct Cyc_List_List _tmp3B8;struct Cyc_Absyn_Exp*_tmp3B9;struct Cyc_List_List*
_tmp3BA;void*_tmp3BB;struct Cyc_List_List*_tmp3BC;_LL255: if(*((int*)_tmp3AC)!= 6)
goto _LL257;_tmp3AD=((struct Cyc_Absyn_Conditional_e_struct*)_tmp3AC)->f1;_tmp3AE=((
struct Cyc_Absyn_Conditional_e_struct*)_tmp3AC)->f2;_tmp3AF=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp3AC)->f3;_LL256: {union Cyc_CfFlowInfo_FlowInfo_union _tmp3BE;union Cyc_CfFlowInfo_FlowInfo_union
_tmp3BF;struct _tuple8 _tmp3BD=Cyc_NewControlFlow_anal_test(env,inflow,_tmp3AD);
_tmp3BE=_tmp3BD.f1;_tmp3BF=_tmp3BD.f2;_tmp3BE=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp3AD->loc,_tmp3BE);_tmp3BF=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp3AD->loc,
_tmp3BF);{union Cyc_CfFlowInfo_FlowInfo_union _tmp3C1;union Cyc_CfFlowInfo_FlowInfo_union
_tmp3C2;struct _tuple8 _tmp3C0=Cyc_NewControlFlow_anal_test(env,_tmp3BE,_tmp3AE);
_tmp3C1=_tmp3C0.f1;_tmp3C2=_tmp3C0.f2;{union Cyc_CfFlowInfo_FlowInfo_union _tmp3C4;
union Cyc_CfFlowInfo_FlowInfo_union _tmp3C5;struct _tuple8 _tmp3C3=Cyc_NewControlFlow_anal_test(
env,_tmp3BF,_tmp3AF);_tmp3C4=_tmp3C3.f1;_tmp3C5=_tmp3C3.f2;return({struct _tuple8
_tmp3C6;_tmp3C6.f1=Cyc_CfFlowInfo_join_flow(_tmp3AB,env->all_changed,_tmp3C1,
_tmp3C4,1);_tmp3C6.f2=Cyc_CfFlowInfo_join_flow(_tmp3AB,env->all_changed,_tmp3C2,
_tmp3C5,1);_tmp3C6;});}}}_LL257: if(*((int*)_tmp3AC)!= 7)goto _LL259;_tmp3B0=((
struct Cyc_Absyn_And_e_struct*)_tmp3AC)->f1;_tmp3B1=((struct Cyc_Absyn_And_e_struct*)
_tmp3AC)->f2;_LL258: {union Cyc_CfFlowInfo_FlowInfo_union _tmp3C8;union Cyc_CfFlowInfo_FlowInfo_union
_tmp3C9;struct _tuple8 _tmp3C7=Cyc_NewControlFlow_anal_test(env,inflow,_tmp3B0);
_tmp3C8=_tmp3C7.f1;_tmp3C9=_tmp3C7.f2;_tmp3C8=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp3B0->loc,_tmp3C8);_tmp3C9=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp3B0->loc,
_tmp3C9);{union Cyc_CfFlowInfo_FlowInfo_union _tmp3CB;union Cyc_CfFlowInfo_FlowInfo_union
_tmp3CC;struct _tuple8 _tmp3CA=Cyc_NewControlFlow_anal_test(env,_tmp3C8,_tmp3B1);
_tmp3CB=_tmp3CA.f1;_tmp3CC=_tmp3CA.f2;_tmp3CB=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp3B1->loc,_tmp3CB);_tmp3CC=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp3B1->loc,
_tmp3CC);return({struct _tuple8 _tmp3CD;_tmp3CD.f1=_tmp3CB;_tmp3CD.f2=Cyc_CfFlowInfo_join_flow(
_tmp3AB,env->all_changed,_tmp3C9,_tmp3CC,0);_tmp3CD;});}}_LL259: if(*((int*)
_tmp3AC)!= 8)goto _LL25B;_tmp3B2=((struct Cyc_Absyn_Or_e_struct*)_tmp3AC)->f1;
_tmp3B3=((struct Cyc_Absyn_Or_e_struct*)_tmp3AC)->f2;_LL25A: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp3CF;union Cyc_CfFlowInfo_FlowInfo_union _tmp3D0;struct _tuple8 _tmp3CE=Cyc_NewControlFlow_anal_test(
env,inflow,_tmp3B2);_tmp3CF=_tmp3CE.f1;_tmp3D0=_tmp3CE.f2;_tmp3CF=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp3B2->loc,_tmp3CF);_tmp3D0=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp3B2->loc,
_tmp3D0);{union Cyc_CfFlowInfo_FlowInfo_union _tmp3D2;union Cyc_CfFlowInfo_FlowInfo_union
_tmp3D3;struct _tuple8 _tmp3D1=Cyc_NewControlFlow_anal_test(env,_tmp3D0,_tmp3B3);
_tmp3D2=_tmp3D1.f1;_tmp3D3=_tmp3D1.f2;_tmp3D2=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp3B3->loc,_tmp3D2);_tmp3D3=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp3B3->loc,
_tmp3D3);return({struct _tuple8 _tmp3D4;_tmp3D4.f1=Cyc_CfFlowInfo_join_flow(
_tmp3AB,env->all_changed,_tmp3CF,_tmp3D2,0);_tmp3D4.f2=_tmp3D3;_tmp3D4;});}}
_LL25B: if(*((int*)_tmp3AC)!= 9)goto _LL25D;_tmp3B4=((struct Cyc_Absyn_SeqExp_e_struct*)
_tmp3AC)->f1;_tmp3B5=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp3AC)->f2;_LL25C: {
union Cyc_CfFlowInfo_FlowInfo_union _tmp3D6;void*_tmp3D7;struct _tuple5 _tmp3D5=Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmp3B4);_tmp3D6=_tmp3D5.f1;_tmp3D7=_tmp3D5.f2;_tmp3D6=Cyc_CfFlowInfo_drop_unique_rvals(
_tmp3B4->loc,_tmp3D6);return Cyc_NewControlFlow_anal_test(env,_tmp3D6,_tmp3B5);}
_LL25D: if(*((int*)_tmp3AC)!= 3)goto _LL25F;_tmp3B6=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp3AC)->f1;if((int)_tmp3B6 != 11)goto _LL25F;_tmp3B7=((struct Cyc_Absyn_Primop_e_struct*)
_tmp3AC)->f2;if(_tmp3B7 == 0)goto _LL25F;_tmp3B8=*_tmp3B7;_tmp3B9=(struct Cyc_Absyn_Exp*)
_tmp3B8.hd;_tmp3BA=_tmp3B8.tl;if(_tmp3BA != 0)goto _LL25F;_LL25E: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp3D9;union Cyc_CfFlowInfo_FlowInfo_union _tmp3DA;struct _tuple8 _tmp3D8=Cyc_NewControlFlow_anal_test(
env,inflow,_tmp3B9);_tmp3D9=_tmp3D8.f1;_tmp3DA=_tmp3D8.f2;return({struct _tuple8
_tmp3DB;_tmp3DB.f1=_tmp3DA;_tmp3DB.f2=_tmp3D9;_tmp3DB;});}_LL25F: if(*((int*)
_tmp3AC)!= 3)goto _LL261;_tmp3BB=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp3AC)->f1;_tmp3BC=((struct Cyc_Absyn_Primop_e_struct*)_tmp3AC)->f2;_LL260: {
void*r1;void*r2;union Cyc_CfFlowInfo_FlowInfo_union f;{struct _RegionHandle _tmp3DC=
_new_region("temp");struct _RegionHandle*temp=& _tmp3DC;_push_region(temp);{union
Cyc_CfFlowInfo_FlowInfo_union _tmp3DE;struct Cyc_List_List*_tmp3DF;struct _tuple11
_tmp3DD=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,_tmp3BC,0);
_tmp3DE=_tmp3DD.f1;_tmp3DF=_tmp3DD.f2;r1=(void*)((struct Cyc_List_List*)
_check_null(_tmp3DF))->hd;r2=(void*)((struct Cyc_List_List*)_check_null(_tmp3DF->tl))->hd;
f=_tmp3DE;};_pop_region(temp);}{union Cyc_CfFlowInfo_FlowInfo_union _tmp3E0=f;
struct Cyc_Dict_Dict _tmp3E1;struct Cyc_List_List*_tmp3E2;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp3E3;_LL264: if((_tmp3E0.BottomFL).tag != 0)goto _LL266;_LL265: return({struct
_tuple8 _tmp3E4;_tmp3E4.f1=f;_tmp3E4.f2=f;_tmp3E4;});_LL266: if((_tmp3E0.ReachableFL).tag
!= 1)goto _LL263;_tmp3E1=(_tmp3E0.ReachableFL).f1;_tmp3E2=(_tmp3E0.ReachableFL).f2;
_tmp3E3=(_tmp3E0.ReachableFL).f3;_LL267: {struct Cyc_Absyn_Exp*_tmp3E5=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp3BC))->hd;struct Cyc_Absyn_Exp*_tmp3E6=(
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp3BC->tl))->hd;if(Cyc_CfFlowInfo_initlevel(
_tmp3E1,r1)== (void*)0)({void*_tmp3E7=0;Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)
_tmp3BC->hd)->loc,({const char*_tmp3E8="expression may not be initialized";
_tag_dyneither(_tmp3E8,sizeof(char),34);}),_tag_dyneither(_tmp3E7,sizeof(void*),
0));});if(Cyc_CfFlowInfo_initlevel(_tmp3E1,r2)== (void*)0)({void*_tmp3E9=0;Cyc_Tcutil_terr(((
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp3BC->tl))->hd)->loc,({
const char*_tmp3EA="expression may not be initialized";_tag_dyneither(_tmp3EA,
sizeof(char),34);}),_tag_dyneither(_tmp3E9,sizeof(void*),0));});if(_tmp3BB == (
void*)5  || _tmp3BB == (void*)6){struct _tuple0 _tmp3EC=({struct _tuple0 _tmp3EB;
_tmp3EB.f1=r1;_tmp3EB.f2=r2;_tmp3EB;});void*_tmp3ED;void*_tmp3EE;void*_tmp3EF;
void*_tmp3F0;void*_tmp3F1;void*_tmp3F2;void*_tmp3F3;void*_tmp3F4;void*_tmp3F5;
void*_tmp3F6;void*_tmp3F7;void*_tmp3F8;void*_tmp3F9;void*_tmp3FA;void*_tmp3FB;
void*_tmp3FC;void*_tmp3FD;void*_tmp3FE;void*_tmp3FF;void*_tmp400;_LL269: _tmp3ED=
_tmp3EC.f1;if(_tmp3ED <= (void*)3)goto _LL26B;if(*((int*)_tmp3ED)!= 0)goto _LL26B;
_tmp3EE=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp3ED)->f1;_tmp3EF=
_tmp3EC.f2;if((int)_tmp3EF != 0)goto _LL26B;_LL26A: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp402;union Cyc_CfFlowInfo_FlowInfo_union _tmp403;struct _tuple8 _tmp401=Cyc_NewControlFlow_splitzero(
env,f,f,_tmp3E5,_tmp3EE);_tmp402=_tmp401.f1;_tmp403=_tmp401.f2;{void*_tmp404=
_tmp3BB;_LL27E: if((int)_tmp404 != 5)goto _LL280;_LL27F: return({struct _tuple8
_tmp405;_tmp405.f1=_tmp403;_tmp405.f2=_tmp402;_tmp405;});_LL280: if((int)_tmp404
!= 6)goto _LL282;_LL281: return({struct _tuple8 _tmp406;_tmp406.f1=_tmp402;_tmp406.f2=
_tmp403;_tmp406;});_LL282:;_LL283:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp407=_cycalloc(sizeof(*_tmp407));_tmp407[0]=({struct Cyc_Core_Impossible_struct
_tmp408;_tmp408.tag=Cyc_Core_Impossible;_tmp408.f1=({const char*_tmp409="anal_test, zero-split";
_tag_dyneither(_tmp409,sizeof(char),22);});_tmp408;});_tmp407;}));_LL27D:;}}
_LL26B: _tmp3F0=_tmp3EC.f1;if((int)_tmp3F0 != 0)goto _LL26D;_tmp3F1=_tmp3EC.f2;if(
_tmp3F1 <= (void*)3)goto _LL26D;if(*((int*)_tmp3F1)!= 0)goto _LL26D;_tmp3F2=(void*)((
struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp3F1)->f1;_LL26C: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp40B;union Cyc_CfFlowInfo_FlowInfo_union _tmp40C;struct _tuple8 _tmp40A=Cyc_NewControlFlow_splitzero(
env,f,f,_tmp3E6,_tmp3F2);_tmp40B=_tmp40A.f1;_tmp40C=_tmp40A.f2;{void*_tmp40D=
_tmp3BB;_LL285: if((int)_tmp40D != 5)goto _LL287;_LL286: return({struct _tuple8
_tmp40E;_tmp40E.f1=_tmp40C;_tmp40E.f2=_tmp40B;_tmp40E;});_LL287: if((int)_tmp40D
!= 6)goto _LL289;_LL288: return({struct _tuple8 _tmp40F;_tmp40F.f1=_tmp40B;_tmp40F.f2=
_tmp40C;_tmp40F;});_LL289:;_LL28A:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp410=_cycalloc(sizeof(*_tmp410));_tmp410[0]=({struct Cyc_Core_Impossible_struct
_tmp411;_tmp411.tag=Cyc_Core_Impossible;_tmp411.f1=({const char*_tmp412="anal_test, zero-split";
_tag_dyneither(_tmp412,sizeof(char),22);});_tmp411;});_tmp410;}));_LL284:;}}
_LL26D: _tmp3F3=_tmp3EC.f1;if((int)_tmp3F3 != 0)goto _LL26F;_tmp3F4=_tmp3EC.f2;if((
int)_tmp3F4 != 0)goto _LL26F;_LL26E: if(_tmp3BB == (void*)5)return({struct _tuple8
_tmp413;_tmp413.f1=f;_tmp413.f2=(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp414;(_tmp414.BottomFL).tag=0;_tmp414;});_tmp413;});else{return({struct
_tuple8 _tmp415;_tmp415.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp416;(_tmp416.BottomFL).tag=0;_tmp416;});_tmp415.f2=f;_tmp415;});}_LL26F:
_tmp3F5=_tmp3EC.f1;if((int)_tmp3F5 != 0)goto _LL271;_tmp3F6=_tmp3EC.f2;if((int)
_tmp3F6 != 1)goto _LL271;_LL270: goto _LL272;_LL271: _tmp3F7=_tmp3EC.f1;if((int)
_tmp3F7 != 0)goto _LL273;_tmp3F8=_tmp3EC.f2;if((int)_tmp3F8 != 2)goto _LL273;_LL272:
goto _LL274;_LL273: _tmp3F9=_tmp3EC.f1;if((int)_tmp3F9 != 0)goto _LL275;_tmp3FA=
_tmp3EC.f2;if(_tmp3FA <= (void*)3)goto _LL275;if(*((int*)_tmp3FA)!= 2)goto _LL275;
_LL274: goto _LL276;_LL275: _tmp3FB=_tmp3EC.f1;if((int)_tmp3FB != 1)goto _LL277;
_tmp3FC=_tmp3EC.f2;if((int)_tmp3FC != 0)goto _LL277;_LL276: goto _LL278;_LL277:
_tmp3FD=_tmp3EC.f1;if((int)_tmp3FD != 2)goto _LL279;_tmp3FE=_tmp3EC.f2;if((int)
_tmp3FE != 0)goto _LL279;_LL278: goto _LL27A;_LL279: _tmp3FF=_tmp3EC.f1;if(_tmp3FF <= (
void*)3)goto _LL27B;if(*((int*)_tmp3FF)!= 2)goto _LL27B;_tmp400=_tmp3EC.f2;if((int)
_tmp400 != 0)goto _LL27B;_LL27A: if(_tmp3BB == (void*)6)return({struct _tuple8 _tmp417;
_tmp417.f1=f;_tmp417.f2=(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp418;(_tmp418.BottomFL).tag=0;_tmp418;});_tmp417;});else{return({struct
_tuple8 _tmp419;_tmp419.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp41A;(_tmp41A.BottomFL).tag=0;_tmp41A;});_tmp419.f2=f;_tmp419;});}_LL27B:;
_LL27C: goto _LL268;_LL268:;}{struct _tuple0 _tmp41C=({struct _tuple0 _tmp41B;_tmp41B.f1=
Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp3E5->topt))->v);
_tmp41B.f2=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp3E6->topt))->v);
_tmp41B;});void*_tmp41D;void*_tmp41E;void*_tmp41F;void*_tmp420;void*_tmp421;void*
_tmp422;_LL28C: _tmp41D=_tmp41C.f1;if(_tmp41D <= (void*)4)goto _LL28E;if(*((int*)
_tmp41D)!= 5)goto _LL28E;_tmp41E=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp41D)->f1;
if((int)_tmp41E != 1)goto _LL28E;_LL28D: goto _LL28F;_LL28E: _tmp41F=_tmp41C.f2;if(
_tmp41F <= (void*)4)goto _LL290;if(*((int*)_tmp41F)!= 5)goto _LL290;_tmp420=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp41F)->f1;if((int)_tmp420 != 1)goto _LL290;
_LL28F: goto _LL291;_LL290: _tmp421=_tmp41C.f1;if(_tmp421 <= (void*)4)goto _LL292;if(*((
int*)_tmp421)!= 18)goto _LL292;_LL291: goto _LL293;_LL292: _tmp422=_tmp41C.f2;if(
_tmp422 <= (void*)4)goto _LL294;if(*((int*)_tmp422)!= 18)goto _LL294;_LL293: goto
_LL28B;_LL294:;_LL295: return({struct _tuple8 _tmp423;_tmp423.f1=f;_tmp423.f2=f;
_tmp423;});_LL28B:;}{void*_tmp424=_tmp3BB;_LL297: if((int)_tmp424 != 5)goto _LL299;
_LL298: {union Cyc_CfFlowInfo_FlowInfo_union _tmp425=Cyc_NewControlFlow_if_tagcmp(
env,f,_tmp3E5,r1,(void*)5,r2);_tmp425=Cyc_NewControlFlow_if_tagcmp(env,_tmp425,
_tmp3E6,r2,(void*)5,r1);return({struct _tuple8 _tmp426;_tmp426.f1=_tmp425;_tmp426.f2=
f;_tmp426;});}_LL299: if((int)_tmp424 != 6)goto _LL29B;_LL29A: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp427=Cyc_NewControlFlow_if_tagcmp(env,f,_tmp3E5,r1,(void*)5,r2);_tmp427=Cyc_NewControlFlow_if_tagcmp(
env,_tmp427,_tmp3E6,r2,(void*)5,r1);return({struct _tuple8 _tmp428;_tmp428.f1=f;
_tmp428.f2=_tmp427;_tmp428;});}_LL29B: if((int)_tmp424 != 7)goto _LL29D;_LL29C: {
union Cyc_CfFlowInfo_FlowInfo_union _tmp429=Cyc_NewControlFlow_if_tagcmp(env,f,
_tmp3E6,r2,(void*)8,r1);union Cyc_CfFlowInfo_FlowInfo_union _tmp42A=Cyc_NewControlFlow_if_tagcmp(
env,f,_tmp3E5,r1,(void*)10,r2);return({struct _tuple8 _tmp42B;_tmp42B.f1=_tmp429;
_tmp42B.f2=_tmp42A;_tmp42B;});}_LL29D: if((int)_tmp424 != 9)goto _LL29F;_LL29E: {
union Cyc_CfFlowInfo_FlowInfo_union _tmp42C=Cyc_NewControlFlow_if_tagcmp(env,f,
_tmp3E6,r2,(void*)10,r1);union Cyc_CfFlowInfo_FlowInfo_union _tmp42D=Cyc_NewControlFlow_if_tagcmp(
env,f,_tmp3E5,r1,(void*)8,r2);return({struct _tuple8 _tmp42E;_tmp42E.f1=_tmp42C;
_tmp42E.f2=_tmp42D;_tmp42E;});}_LL29F: if((int)_tmp424 != 8)goto _LL2A1;_LL2A0: {
union Cyc_CfFlowInfo_FlowInfo_union _tmp42F=Cyc_NewControlFlow_if_tagcmp(env,f,
_tmp3E5,r1,(void*)8,r2);union Cyc_CfFlowInfo_FlowInfo_union _tmp430=Cyc_NewControlFlow_if_tagcmp(
env,f,_tmp3E6,r2,(void*)10,r1);{union Cyc_CfFlowInfo_FlowInfo_union _tmp431=
_tmp42F;struct Cyc_Dict_Dict _tmp432;struct Cyc_CfFlowInfo_ConsumeInfo _tmp433;
_LL2A6: if((_tmp431.BottomFL).tag != 0)goto _LL2A8;_LL2A7:(int)_throw((void*)({
struct Cyc_Core_Impossible_struct*_tmp434=_cycalloc(sizeof(*_tmp434));_tmp434[0]=({
struct Cyc_Core_Impossible_struct _tmp435;_tmp435.tag=Cyc_Core_Impossible;_tmp435.f1=({
const char*_tmp436="anal_test, Lt";_tag_dyneither(_tmp436,sizeof(char),14);});
_tmp435;});_tmp434;}));_LL2A8: if((_tmp431.ReachableFL).tag != 1)goto _LL2A5;
_tmp432=(_tmp431.ReachableFL).f1;_tmp433=(_tmp431.ReachableFL).f3;_LL2A9: _tmp3E1=
_tmp432;_tmp3E3=_tmp433;_LL2A5:;}{void*_tmp437=(void*)_tmp3E5->r;void*_tmp438;
struct Cyc_Absyn_Vardecl*_tmp439;void*_tmp43A;struct Cyc_Absyn_Vardecl*_tmp43B;
void*_tmp43C;struct Cyc_Absyn_Vardecl*_tmp43D;void*_tmp43E;struct Cyc_Absyn_Vardecl*
_tmp43F;_LL2AB: if(*((int*)_tmp437)!= 1)goto _LL2AD;_tmp438=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp437)->f2;if(_tmp438 <= (void*)1)goto _LL2AD;if(*((int*)_tmp438)!= 0)goto _LL2AD;
_tmp439=((struct Cyc_Absyn_Global_b_struct*)_tmp438)->f1;if(!(!_tmp439->escapes))
goto _LL2AD;_LL2AC: _tmp43B=_tmp439;goto _LL2AE;_LL2AD: if(*((int*)_tmp437)!= 1)goto
_LL2AF;_tmp43A=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp437)->f2;if(_tmp43A <= (
void*)1)goto _LL2AF;if(*((int*)_tmp43A)!= 3)goto _LL2AF;_tmp43B=((struct Cyc_Absyn_Local_b_struct*)
_tmp43A)->f1;if(!(!_tmp43B->escapes))goto _LL2AF;_LL2AE: _tmp43D=_tmp43B;goto
_LL2B0;_LL2AF: if(*((int*)_tmp437)!= 1)goto _LL2B1;_tmp43C=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp437)->f2;if(_tmp43C <= (void*)1)goto _LL2B1;if(*((int*)_tmp43C)!= 4)goto _LL2B1;
_tmp43D=((struct Cyc_Absyn_Pat_b_struct*)_tmp43C)->f1;if(!(!_tmp43D->escapes))
goto _LL2B1;_LL2B0: _tmp43F=_tmp43D;goto _LL2B2;_LL2B1: if(*((int*)_tmp437)!= 1)goto
_LL2B3;_tmp43E=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp437)->f2;if(_tmp43E <= (
void*)1)goto _LL2B3;if(*((int*)_tmp43E)!= 2)goto _LL2B3;_tmp43F=((struct Cyc_Absyn_Param_b_struct*)
_tmp43E)->f1;if(!(!_tmp43F->escapes))goto _LL2B3;_LL2B2: {void*_tmp440=(void*)
_tmp3E6->r;void*_tmp441;struct Cyc_Absyn_Vardecl*_tmp442;void*_tmp443;struct Cyc_Absyn_Vardecl*
_tmp444;void*_tmp445;struct Cyc_Absyn_Vardecl*_tmp446;void*_tmp447;struct Cyc_Absyn_Vardecl*
_tmp448;union Cyc_Absyn_Cnst_union _tmp449;int _tmp44A;void*_tmp44B;struct Cyc_List_List*
_tmp44C;struct Cyc_List_List _tmp44D;struct Cyc_Absyn_Exp*_tmp44E;_LL2B6: if(*((int*)
_tmp440)!= 1)goto _LL2B8;_tmp441=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp440)->f2;
if(_tmp441 <= (void*)1)goto _LL2B8;if(*((int*)_tmp441)!= 0)goto _LL2B8;_tmp442=((
struct Cyc_Absyn_Global_b_struct*)_tmp441)->f1;if(!(!_tmp442->escapes))goto _LL2B8;
_LL2B7: _tmp444=_tmp442;goto _LL2B9;_LL2B8: if(*((int*)_tmp440)!= 1)goto _LL2BA;
_tmp443=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp440)->f2;if(_tmp443 <= (void*)
1)goto _LL2BA;if(*((int*)_tmp443)!= 3)goto _LL2BA;_tmp444=((struct Cyc_Absyn_Local_b_struct*)
_tmp443)->f1;if(!(!_tmp444->escapes))goto _LL2BA;_LL2B9: _tmp446=_tmp444;goto
_LL2BB;_LL2BA: if(*((int*)_tmp440)!= 1)goto _LL2BC;_tmp445=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp440)->f2;if(_tmp445 <= (void*)1)goto _LL2BC;if(*((int*)_tmp445)!= 4)goto _LL2BC;
_tmp446=((struct Cyc_Absyn_Pat_b_struct*)_tmp445)->f1;if(!(!_tmp446->escapes))
goto _LL2BC;_LL2BB: _tmp448=_tmp446;goto _LL2BD;_LL2BC: if(*((int*)_tmp440)!= 1)goto
_LL2BE;_tmp447=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp440)->f2;if(_tmp447 <= (
void*)1)goto _LL2BE;if(*((int*)_tmp447)!= 2)goto _LL2BE;_tmp448=((struct Cyc_Absyn_Param_b_struct*)
_tmp447)->f1;if(!(!_tmp448->escapes))goto _LL2BE;_LL2BD: {struct _RegionHandle*
_tmp44F=(env->fenv)->r;struct Cyc_List_List*_tmp450=({struct Cyc_List_List*_tmp453=
_region_malloc(_tmp44F,sizeof(*_tmp453));_tmp453->hd=({struct Cyc_CfFlowInfo_Reln*
_tmp454=_region_malloc(_tmp44F,sizeof(*_tmp454));_tmp454->vd=_tmp43F;_tmp454->rop=(
union Cyc_CfFlowInfo_RelnOp_union)({union Cyc_CfFlowInfo_RelnOp_union _tmp455;(
_tmp455.LessVar).tag=1;(_tmp455.LessVar).f1=_tmp448;_tmp455;});_tmp454;});
_tmp453->tl=_tmp3E2;_tmp453;});return({struct _tuple8 _tmp451;_tmp451.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp452;(_tmp452.ReachableFL).tag=1;(_tmp452.ReachableFL).f1=
_tmp3E1;(_tmp452.ReachableFL).f2=_tmp450;(_tmp452.ReachableFL).f3=_tmp3E3;
_tmp452;});_tmp451.f2=_tmp430;_tmp451;});}_LL2BE: if(*((int*)_tmp440)!= 0)goto
_LL2C0;_tmp449=((struct Cyc_Absyn_Const_e_struct*)_tmp440)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)
_tmp440)->f1).Int_c).tag != 2)goto _LL2C0;_tmp44A=(_tmp449.Int_c).f2;_LL2BF: {
struct _RegionHandle*_tmp456=(env->fenv)->r;struct Cyc_List_List*_tmp457=({struct
Cyc_List_List*_tmp45A=_region_malloc(_tmp456,sizeof(*_tmp45A));_tmp45A->hd=({
struct Cyc_CfFlowInfo_Reln*_tmp45B=_region_malloc(_tmp456,sizeof(*_tmp45B));
_tmp45B->vd=_tmp43F;_tmp45B->rop=(union Cyc_CfFlowInfo_RelnOp_union)({union Cyc_CfFlowInfo_RelnOp_union
_tmp45C;(_tmp45C.LessConst).tag=3;(_tmp45C.LessConst).f1=(unsigned int)_tmp44A;
_tmp45C;});_tmp45B;});_tmp45A->tl=_tmp3E2;_tmp45A;});return({struct _tuple8
_tmp458;_tmp458.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp459;(_tmp459.ReachableFL).tag=1;(_tmp459.ReachableFL).f1=_tmp3E1;(_tmp459.ReachableFL).f2=
_tmp457;(_tmp459.ReachableFL).f3=_tmp3E3;_tmp459;});_tmp458.f2=_tmp430;_tmp458;});}
_LL2C0: if(*((int*)_tmp440)!= 3)goto _LL2C2;_tmp44B=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp440)->f1;_tmp44C=((struct Cyc_Absyn_Primop_e_struct*)_tmp440)->f2;if(_tmp44C
== 0)goto _LL2C2;_tmp44D=*_tmp44C;_tmp44E=(struct Cyc_Absyn_Exp*)_tmp44D.hd;_LL2C1: {
void*_tmp45D=(void*)_tmp44E->r;void*_tmp45E;struct Cyc_Absyn_Vardecl*_tmp45F;void*
_tmp460;struct Cyc_Absyn_Vardecl*_tmp461;void*_tmp462;struct Cyc_Absyn_Vardecl*
_tmp463;void*_tmp464;struct Cyc_Absyn_Vardecl*_tmp465;_LL2C5: if(*((int*)_tmp45D)
!= 1)goto _LL2C7;_tmp45E=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp45D)->f2;if(
_tmp45E <= (void*)1)goto _LL2C7;if(*((int*)_tmp45E)!= 0)goto _LL2C7;_tmp45F=((
struct Cyc_Absyn_Global_b_struct*)_tmp45E)->f1;if(!(!_tmp45F->escapes))goto _LL2C7;
_LL2C6: _tmp461=_tmp45F;goto _LL2C8;_LL2C7: if(*((int*)_tmp45D)!= 1)goto _LL2C9;
_tmp460=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp45D)->f2;if(_tmp460 <= (void*)
1)goto _LL2C9;if(*((int*)_tmp460)!= 3)goto _LL2C9;_tmp461=((struct Cyc_Absyn_Local_b_struct*)
_tmp460)->f1;if(!(!_tmp461->escapes))goto _LL2C9;_LL2C8: _tmp463=_tmp461;goto
_LL2CA;_LL2C9: if(*((int*)_tmp45D)!= 1)goto _LL2CB;_tmp462=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp45D)->f2;if(_tmp462 <= (void*)1)goto _LL2CB;if(*((int*)_tmp462)!= 4)goto _LL2CB;
_tmp463=((struct Cyc_Absyn_Pat_b_struct*)_tmp462)->f1;if(!(!_tmp463->escapes))
goto _LL2CB;_LL2CA: _tmp465=_tmp463;goto _LL2CC;_LL2CB: if(*((int*)_tmp45D)!= 1)goto
_LL2CD;_tmp464=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp45D)->f2;if(_tmp464 <= (
void*)1)goto _LL2CD;if(*((int*)_tmp464)!= 2)goto _LL2CD;_tmp465=((struct Cyc_Absyn_Param_b_struct*)
_tmp464)->f1;if(!(!_tmp465->escapes))goto _LL2CD;_LL2CC: {struct _RegionHandle*
_tmp466=(env->fenv)->r;struct Cyc_List_List*_tmp467=({struct Cyc_List_List*_tmp46A=
_region_malloc(_tmp466,sizeof(*_tmp46A));_tmp46A->hd=({struct Cyc_CfFlowInfo_Reln*
_tmp46B=_region_malloc(_tmp466,sizeof(*_tmp46B));_tmp46B->vd=_tmp43F;_tmp46B->rop=(
union Cyc_CfFlowInfo_RelnOp_union)({union Cyc_CfFlowInfo_RelnOp_union _tmp46C;(
_tmp46C.LessNumelts).tag=2;(_tmp46C.LessNumelts).f1=_tmp465;_tmp46C;});_tmp46B;});
_tmp46A->tl=_tmp3E2;_tmp46A;});return({struct _tuple8 _tmp468;_tmp468.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp469;(_tmp469.ReachableFL).tag=1;(_tmp469.ReachableFL).f1=
_tmp3E1;(_tmp469.ReachableFL).f2=_tmp467;(_tmp469.ReachableFL).f3=_tmp3E3;
_tmp469;});_tmp468.f2=_tmp430;_tmp468;});}_LL2CD:;_LL2CE: return({struct _tuple8
_tmp46D;_tmp46D.f1=_tmp42F;_tmp46D.f2=_tmp430;_tmp46D;});_LL2C4:;}_LL2C2:;_LL2C3:
return({struct _tuple8 _tmp46E;_tmp46E.f1=_tmp42F;_tmp46E.f2=_tmp430;_tmp46E;});
_LL2B5:;}_LL2B3:;_LL2B4: return({struct _tuple8 _tmp46F;_tmp46F.f1=_tmp42F;_tmp46F.f2=
_tmp430;_tmp46F;});_LL2AA:;}}_LL2A1: if((int)_tmp424 != 10)goto _LL2A3;_LL2A2: {
union Cyc_CfFlowInfo_FlowInfo_union _tmp470=Cyc_NewControlFlow_if_tagcmp(env,f,
_tmp3E5,r1,(void*)10,r2);union Cyc_CfFlowInfo_FlowInfo_union _tmp471=Cyc_NewControlFlow_if_tagcmp(
env,f,_tmp3E6,r1,(void*)8,r1);{union Cyc_CfFlowInfo_FlowInfo_union _tmp472=_tmp470;
struct Cyc_Dict_Dict _tmp473;struct Cyc_CfFlowInfo_ConsumeInfo _tmp474;_LL2D0: if((
_tmp472.BottomFL).tag != 0)goto _LL2D2;_LL2D1:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp475=_cycalloc(sizeof(*_tmp475));_tmp475[0]=({struct Cyc_Core_Impossible_struct
_tmp476;_tmp476.tag=Cyc_Core_Impossible;_tmp476.f1=({const char*_tmp477="anal_test, Lte";
_tag_dyneither(_tmp477,sizeof(char),15);});_tmp476;});_tmp475;}));_LL2D2: if((
_tmp472.ReachableFL).tag != 1)goto _LL2CF;_tmp473=(_tmp472.ReachableFL).f1;_tmp474=(
_tmp472.ReachableFL).f3;_LL2D3: _tmp3E1=_tmp473;_tmp3E3=_tmp474;_LL2CF:;}{void*
_tmp478=(void*)_tmp3E5->r;void*_tmp479;struct Cyc_Absyn_Vardecl*_tmp47A;void*
_tmp47B;struct Cyc_Absyn_Vardecl*_tmp47C;void*_tmp47D;struct Cyc_Absyn_Vardecl*
_tmp47E;void*_tmp47F;struct Cyc_Absyn_Vardecl*_tmp480;_LL2D5: if(*((int*)_tmp478)
!= 1)goto _LL2D7;_tmp479=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp478)->f2;if(
_tmp479 <= (void*)1)goto _LL2D7;if(*((int*)_tmp479)!= 0)goto _LL2D7;_tmp47A=((
struct Cyc_Absyn_Global_b_struct*)_tmp479)->f1;if(!(!_tmp47A->escapes))goto _LL2D7;
_LL2D6: _tmp47C=_tmp47A;goto _LL2D8;_LL2D7: if(*((int*)_tmp478)!= 1)goto _LL2D9;
_tmp47B=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp478)->f2;if(_tmp47B <= (void*)
1)goto _LL2D9;if(*((int*)_tmp47B)!= 3)goto _LL2D9;_tmp47C=((struct Cyc_Absyn_Local_b_struct*)
_tmp47B)->f1;if(!(!_tmp47C->escapes))goto _LL2D9;_LL2D8: _tmp47E=_tmp47C;goto
_LL2DA;_LL2D9: if(*((int*)_tmp478)!= 1)goto _LL2DB;_tmp47D=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp478)->f2;if(_tmp47D <= (void*)1)goto _LL2DB;if(*((int*)_tmp47D)!= 4)goto _LL2DB;
_tmp47E=((struct Cyc_Absyn_Pat_b_struct*)_tmp47D)->f1;if(!(!_tmp47E->escapes))
goto _LL2DB;_LL2DA: _tmp480=_tmp47E;goto _LL2DC;_LL2DB: if(*((int*)_tmp478)!= 1)goto
_LL2DD;_tmp47F=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp478)->f2;if(_tmp47F <= (
void*)1)goto _LL2DD;if(*((int*)_tmp47F)!= 2)goto _LL2DD;_tmp480=((struct Cyc_Absyn_Param_b_struct*)
_tmp47F)->f1;if(!(!_tmp480->escapes))goto _LL2DD;_LL2DC: {void*_tmp481=(void*)
_tmp3E6->r;void*_tmp482;struct Cyc_List_List*_tmp483;struct Cyc_List_List _tmp484;
struct Cyc_Absyn_Exp*_tmp485;_LL2E0: if(*((int*)_tmp481)!= 3)goto _LL2E2;_tmp482=(
void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp481)->f1;_tmp483=((struct Cyc_Absyn_Primop_e_struct*)
_tmp481)->f2;if(_tmp483 == 0)goto _LL2E2;_tmp484=*_tmp483;_tmp485=(struct Cyc_Absyn_Exp*)
_tmp484.hd;_LL2E1: {void*_tmp486=(void*)_tmp485->r;void*_tmp487;struct Cyc_Absyn_Vardecl*
_tmp488;void*_tmp489;struct Cyc_Absyn_Vardecl*_tmp48A;void*_tmp48B;struct Cyc_Absyn_Vardecl*
_tmp48C;void*_tmp48D;struct Cyc_Absyn_Vardecl*_tmp48E;_LL2E5: if(*((int*)_tmp486)
!= 1)goto _LL2E7;_tmp487=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp486)->f2;if(
_tmp487 <= (void*)1)goto _LL2E7;if(*((int*)_tmp487)!= 0)goto _LL2E7;_tmp488=((
struct Cyc_Absyn_Global_b_struct*)_tmp487)->f1;if(!(!_tmp488->escapes))goto _LL2E7;
_LL2E6: _tmp48A=_tmp488;goto _LL2E8;_LL2E7: if(*((int*)_tmp486)!= 1)goto _LL2E9;
_tmp489=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp486)->f2;if(_tmp489 <= (void*)
1)goto _LL2E9;if(*((int*)_tmp489)!= 3)goto _LL2E9;_tmp48A=((struct Cyc_Absyn_Local_b_struct*)
_tmp489)->f1;if(!(!_tmp48A->escapes))goto _LL2E9;_LL2E8: _tmp48C=_tmp48A;goto
_LL2EA;_LL2E9: if(*((int*)_tmp486)!= 1)goto _LL2EB;_tmp48B=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp486)->f2;if(_tmp48B <= (void*)1)goto _LL2EB;if(*((int*)_tmp48B)!= 4)goto _LL2EB;
_tmp48C=((struct Cyc_Absyn_Pat_b_struct*)_tmp48B)->f1;if(!(!_tmp48C->escapes))
goto _LL2EB;_LL2EA: _tmp48E=_tmp48C;goto _LL2EC;_LL2EB: if(*((int*)_tmp486)!= 1)goto
_LL2ED;_tmp48D=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp486)->f2;if(_tmp48D <= (
void*)1)goto _LL2ED;if(*((int*)_tmp48D)!= 2)goto _LL2ED;_tmp48E=((struct Cyc_Absyn_Param_b_struct*)
_tmp48D)->f1;if(!(!_tmp48E->escapes))goto _LL2ED;_LL2EC: {struct Cyc_CfFlowInfo_FlowEnv*
_tmp48F=env->fenv;struct Cyc_List_List*_tmp490=({struct Cyc_List_List*_tmp493=
_region_malloc(_tmp48F->r,sizeof(*_tmp493));_tmp493->hd=({struct Cyc_CfFlowInfo_Reln*
_tmp494=_region_malloc(_tmp48F->r,sizeof(*_tmp494));_tmp494->vd=_tmp480;_tmp494->rop=(
union Cyc_CfFlowInfo_RelnOp_union)({union Cyc_CfFlowInfo_RelnOp_union _tmp495;(
_tmp495.LessEqNumelts).tag=4;(_tmp495.LessEqNumelts).f1=_tmp48E;_tmp495;});
_tmp494;});_tmp493->tl=_tmp3E2;_tmp493;});return({struct _tuple8 _tmp491;_tmp491.f1=(
union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union _tmp492;(
_tmp492.ReachableFL).tag=1;(_tmp492.ReachableFL).f1=_tmp3E1;(_tmp492.ReachableFL).f2=
_tmp490;(_tmp492.ReachableFL).f3=_tmp3E3;_tmp492;});_tmp491.f2=_tmp471;_tmp491;});}
_LL2ED:;_LL2EE: return({struct _tuple8 _tmp496;_tmp496.f1=_tmp470;_tmp496.f2=
_tmp471;_tmp496;});_LL2E4:;}_LL2E2:;_LL2E3: return({struct _tuple8 _tmp497;_tmp497.f1=
_tmp470;_tmp497.f2=_tmp471;_tmp497;});_LL2DF:;}_LL2DD:;_LL2DE: return({struct
_tuple8 _tmp498;_tmp498.f1=_tmp470;_tmp498.f2=_tmp471;_tmp498;});_LL2D4:;}}_LL2A3:;
_LL2A4: return({struct _tuple8 _tmp499;_tmp499.f1=f;_tmp499.f2=f;_tmp499;});_LL296:;}}
_LL263:;}}_LL261:;_LL262: goto _LL254;_LL254:;}{union Cyc_CfFlowInfo_FlowInfo_union
_tmp49B;void*_tmp49C;struct _tuple5 _tmp49A=Cyc_NewControlFlow_anal_Rexp(env,
inflow,e);_tmp49B=_tmp49A.f1;_tmp49C=_tmp49A.f2;_tmp49B=Cyc_CfFlowInfo_readthrough_unique_rvals(
e->loc,_tmp49B);{union Cyc_CfFlowInfo_FlowInfo_union _tmp49D=_tmp49B;struct Cyc_Dict_Dict
_tmp49E;_LL2F0: if((_tmp49D.BottomFL).tag != 0)goto _LL2F2;_LL2F1: return({struct
_tuple8 _tmp49F;_tmp49F.f1=_tmp49B;_tmp49F.f2=_tmp49B;_tmp49F;});_LL2F2: if((
_tmp49D.ReachableFL).tag != 1)goto _LL2EF;_tmp49E=(_tmp49D.ReachableFL).f1;_LL2F3: {
void*_tmp4A0=_tmp49C;void*_tmp4A1;void*_tmp4A2;void*_tmp4A3;_LL2F5: if((int)
_tmp4A0 != 0)goto _LL2F7;_LL2F6: return({struct _tuple8 _tmp4A4;_tmp4A4.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp4A5;(_tmp4A5.BottomFL).tag=0;_tmp4A5;});
_tmp4A4.f2=_tmp49B;_tmp4A4;});_LL2F7: if((int)_tmp4A0 != 2)goto _LL2F9;_LL2F8: goto
_LL2FA;_LL2F9: if((int)_tmp4A0 != 1)goto _LL2FB;_LL2FA: goto _LL2FC;_LL2FB: if(_tmp4A0
<= (void*)3)goto _LL2FD;if(*((int*)_tmp4A0)!= 2)goto _LL2FD;_LL2FC: return({struct
_tuple8 _tmp4A6;_tmp4A6.f1=_tmp49B;_tmp4A6.f2=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp4A7;(_tmp4A7.BottomFL).tag=0;_tmp4A7;});
_tmp4A6;});_LL2FD: if(_tmp4A0 <= (void*)3)goto _LL2FF;if(*((int*)_tmp4A0)!= 0)goto
_LL2FF;_tmp4A1=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp4A0)->f1;if((
int)_tmp4A1 != 0)goto _LL2FF;_LL2FE: goto _LL300;_LL2FF: if(_tmp4A0 <= (void*)3)goto
_LL301;if(*((int*)_tmp4A0)!= 1)goto _LL301;_tmp4A2=(void*)((struct Cyc_CfFlowInfo_Esc_struct*)
_tmp4A0)->f1;if((int)_tmp4A2 != 0)goto _LL301;_LL300:({void*_tmp4A8=0;Cyc_Tcutil_terr(
e->loc,({const char*_tmp4A9="expression may not be initialized";_tag_dyneither(
_tmp4A9,sizeof(char),34);}),_tag_dyneither(_tmp4A8,sizeof(void*),0));});return({
struct _tuple8 _tmp4AA;_tmp4AA.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp4AC;(_tmp4AC.BottomFL).tag=0;_tmp4AC;});_tmp4AA.f2=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp4AB;(_tmp4AB.BottomFL).tag=0;_tmp4AB;});
_tmp4AA;});_LL301: if(_tmp4A0 <= (void*)3)goto _LL303;if(*((int*)_tmp4A0)!= 0)goto
_LL303;_tmp4A3=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp4A0)->f1;
_LL302: return Cyc_NewControlFlow_splitzero(env,inflow,_tmp49B,e,_tmp4A3);_LL303:
if(_tmp4A0 <= (void*)3)goto _LL305;if(*((int*)_tmp4A0)!= 1)goto _LL305;_LL304:
return({struct _tuple8 _tmp4AD;_tmp4AD.f1=_tmp49B;_tmp4AD.f2=_tmp49B;_tmp4AD;});
_LL305: if(_tmp4A0 <= (void*)3)goto _LL307;if(*((int*)_tmp4A0)!= 3)goto _LL307;
_LL306: return({struct _tuple8 _tmp4AE;_tmp4AE.f1=_tmp49B;_tmp4AE.f2=_tmp49B;
_tmp4AE;});_LL307: if(_tmp4A0 <= (void*)3)goto _LL2F4;if(*((int*)_tmp4A0)!= 4)goto
_LL2F4;_LL308:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp4AF=
_cycalloc(sizeof(*_tmp4AF));_tmp4AF[0]=({struct Cyc_Core_Impossible_struct _tmp4B0;
_tmp4B0.tag=Cyc_Core_Impossible;_tmp4B0.f1=({const char*_tmp4B1="anal_test";
_tag_dyneither(_tmp4B1,sizeof(char),10);});_tmp4B0;});_tmp4AF;}));_LL2F4:;}
_LL2EF:;}}}static void Cyc_NewControlFlow_check_init_params(struct Cyc_Position_Segment*
loc,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union flow){
union Cyc_CfFlowInfo_FlowInfo_union _tmp4B2=flow;struct Cyc_Dict_Dict _tmp4B3;_LL30A:
if((_tmp4B2.BottomFL).tag != 0)goto _LL30C;_LL30B: return;_LL30C: if((_tmp4B2.ReachableFL).tag
!= 1)goto _LL309;_tmp4B3=(_tmp4B2.ReachableFL).f1;_LL30D:{struct Cyc_List_List*
_tmp4B4=env->param_roots;for(0;_tmp4B4 != 0;_tmp4B4=_tmp4B4->tl){if(Cyc_CfFlowInfo_initlevel(
_tmp4B3,Cyc_CfFlowInfo_lookup_place(_tmp4B3,(struct Cyc_CfFlowInfo_Place*)_tmp4B4->hd))
!= (void*)2)({void*_tmp4B5=0;Cyc_Tcutil_terr(loc,({const char*_tmp4B6="function may not initialize all the parameters with attribute 'initializes'";
_tag_dyneither(_tmp4B6,sizeof(char),76);}),_tag_dyneither(_tmp4B5,sizeof(void*),
0));});}}return;_LL309:;}static union Cyc_CfFlowInfo_FlowInfo_union Cyc_NewControlFlow_anal_scs(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union inflow,
struct Cyc_List_List*scs){struct Cyc_CfFlowInfo_FlowEnv*_tmp4B7=env->fenv;for(0;
scs != 0;scs=scs->tl){struct Cyc_Absyn_Switch_clause _tmp4B9;struct Cyc_Core_Opt*
_tmp4BA;struct Cyc_Absyn_Exp*_tmp4BB;struct Cyc_Absyn_Stmt*_tmp4BC;struct Cyc_Position_Segment*
_tmp4BD;struct Cyc_Absyn_Switch_clause*_tmp4B8=(struct Cyc_Absyn_Switch_clause*)
scs->hd;_tmp4B9=*_tmp4B8;_tmp4BA=_tmp4B9.pat_vars;_tmp4BB=_tmp4B9.where_clause;
_tmp4BC=_tmp4B9.body;_tmp4BD=_tmp4B9.loc;{union Cyc_CfFlowInfo_FlowInfo_union
clause_inflow=Cyc_NewControlFlow_add_vars(_tmp4B7,inflow,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp4BA))->v,(void*)_tmp4B7->unknown_all,_tmp4BD);
union Cyc_CfFlowInfo_FlowInfo_union clause_outflow;if(_tmp4BB != 0){struct Cyc_Absyn_Exp*
wexp=(struct Cyc_Absyn_Exp*)_tmp4BB;union Cyc_CfFlowInfo_FlowInfo_union _tmp4BF;
union Cyc_CfFlowInfo_FlowInfo_union _tmp4C0;struct _tuple8 _tmp4BE=Cyc_NewControlFlow_anal_test(
env,clause_inflow,wexp);_tmp4BF=_tmp4BE.f1;_tmp4C0=_tmp4BE.f2;_tmp4BF=Cyc_CfFlowInfo_readthrough_unique_rvals(
wexp->loc,_tmp4BF);_tmp4C0=Cyc_CfFlowInfo_readthrough_unique_rvals(wexp->loc,
_tmp4C0);inflow=_tmp4C0;clause_outflow=Cyc_NewControlFlow_anal_stmt(env,_tmp4BF,
_tmp4BC);}else{clause_outflow=Cyc_NewControlFlow_anal_stmt(env,clause_inflow,
_tmp4BC);}{union Cyc_CfFlowInfo_FlowInfo_union _tmp4C1=clause_outflow;_LL30F: if((
_tmp4C1.BottomFL).tag != 0)goto _LL311;_LL310: goto _LL30E;_LL311:;_LL312: if(scs->tl
== 0)return clause_outflow;else{if((struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(((struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)_check_null(
scs->tl))->hd)->pat_vars))->v != 0)({void*_tmp4C2=0;Cyc_Tcutil_terr(_tmp4BC->loc,({
const char*_tmp4C3="switch clause may implicitly fallthru";_tag_dyneither(_tmp4C3,
sizeof(char),38);}),_tag_dyneither(_tmp4C2,sizeof(void*),0));});else{({void*
_tmp4C4=0;Cyc_Tcutil_warn(_tmp4BC->loc,({const char*_tmp4C5="switch clause may implicitly fallthru";
_tag_dyneither(_tmp4C5,sizeof(char),38);}),_tag_dyneither(_tmp4C4,sizeof(void*),
0));});}Cyc_NewControlFlow_update_flow(env,((struct Cyc_Absyn_Switch_clause*)((
struct Cyc_List_List*)_check_null(scs->tl))->hd)->body,clause_outflow);}goto
_LL30E;_LL30E:;}}}return(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp4C6;(_tmp4C6.BottomFL).tag=0;_tmp4C6;});}static union Cyc_CfFlowInfo_FlowInfo_union
Cyc_NewControlFlow_anal_stmt(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union
inflow,struct Cyc_Absyn_Stmt*s){union Cyc_CfFlowInfo_FlowInfo_union outflow;struct
Cyc_NewControlFlow_CFStmtAnnot*_tmp4C8;union Cyc_CfFlowInfo_FlowInfo_union*
_tmp4C9;struct _tuple9 _tmp4C7=Cyc_NewControlFlow_pre_stmt_check(env,inflow,s);
_tmp4C8=_tmp4C7.f1;_tmp4C9=_tmp4C7.f2;inflow=*_tmp4C9;{struct Cyc_CfFlowInfo_FlowEnv*
_tmp4CA=env->fenv;{void*_tmp4CB=(void*)s->r;struct Cyc_Absyn_Exp*_tmp4CC;struct
Cyc_Absyn_Exp*_tmp4CD;struct Cyc_Absyn_Exp*_tmp4CE;struct Cyc_Absyn_Stmt*_tmp4CF;
struct Cyc_Absyn_Stmt*_tmp4D0;struct Cyc_Absyn_Exp*_tmp4D1;struct Cyc_Absyn_Stmt*
_tmp4D2;struct Cyc_Absyn_Stmt*_tmp4D3;struct _tuple3 _tmp4D4;struct Cyc_Absyn_Exp*
_tmp4D5;struct Cyc_Absyn_Stmt*_tmp4D6;struct Cyc_Absyn_Stmt*_tmp4D7;struct Cyc_Absyn_Stmt*
_tmp4D8;struct _tuple3 _tmp4D9;struct Cyc_Absyn_Exp*_tmp4DA;struct Cyc_Absyn_Stmt*
_tmp4DB;struct Cyc_Absyn_Exp*_tmp4DC;struct _tuple3 _tmp4DD;struct Cyc_Absyn_Exp*
_tmp4DE;struct Cyc_Absyn_Stmt*_tmp4DF;struct _tuple3 _tmp4E0;struct Cyc_Absyn_Exp*
_tmp4E1;struct Cyc_Absyn_Stmt*_tmp4E2;struct Cyc_Absyn_Stmt*_tmp4E3;struct Cyc_Absyn_Stmt*
_tmp4E4;struct Cyc_List_List*_tmp4E5;struct Cyc_Absyn_Switch_clause**_tmp4E6;
struct Cyc_Absyn_Switch_clause*_tmp4E7;struct Cyc_Absyn_Stmt*_tmp4E8;struct Cyc_Absyn_Stmt*
_tmp4E9;struct Cyc_Absyn_Stmt*_tmp4EA;struct Cyc_Absyn_Exp*_tmp4EB;struct Cyc_List_List*
_tmp4EC;struct Cyc_Absyn_Stmt*_tmp4ED;struct Cyc_List_List*_tmp4EE;struct Cyc_Absyn_Decl*
_tmp4EF;struct Cyc_Absyn_Stmt*_tmp4F0;struct Cyc_Absyn_Stmt*_tmp4F1;struct Cyc_Absyn_Tvar*
_tmp4F2;struct Cyc_Absyn_Vardecl*_tmp4F3;int _tmp4F4;struct Cyc_Absyn_Exp*_tmp4F5;
struct Cyc_Absyn_Stmt*_tmp4F6;struct Cyc_Absyn_Exp*_tmp4F7;struct Cyc_Absyn_Exp*
_tmp4F8;struct Cyc_Absyn_Tvar*_tmp4F9;struct Cyc_Absyn_Vardecl*_tmp4FA;struct Cyc_Absyn_Stmt*
_tmp4FB;_LL314: if((int)_tmp4CB != 0)goto _LL316;_LL315: return inflow;_LL316: if(
_tmp4CB <= (void*)1)goto _LL33E;if(*((int*)_tmp4CB)!= 2)goto _LL318;_tmp4CC=((
struct Cyc_Absyn_Return_s_struct*)_tmp4CB)->f1;if(_tmp4CC != 0)goto _LL318;_LL317:
if(env->noreturn)({void*_tmp4FC=0;Cyc_Tcutil_terr(s->loc,({const char*_tmp4FD="`noreturn' function might return";
_tag_dyneither(_tmp4FD,sizeof(char),33);}),_tag_dyneither(_tmp4FC,sizeof(void*),
0));});Cyc_NewControlFlow_check_init_params(s->loc,env,inflow);return(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp4FE;(_tmp4FE.BottomFL).tag=0;_tmp4FE;});
_LL318: if(*((int*)_tmp4CB)!= 2)goto _LL31A;_tmp4CD=((struct Cyc_Absyn_Return_s_struct*)
_tmp4CB)->f1;_LL319: if(env->noreturn)({void*_tmp4FF=0;Cyc_Tcutil_terr(s->loc,({
const char*_tmp500="`noreturn' function might return";_tag_dyneither(_tmp500,
sizeof(char),33);}),_tag_dyneither(_tmp4FF,sizeof(void*),0));});{union Cyc_CfFlowInfo_FlowInfo_union
_tmp502;void*_tmp503;struct _tuple5 _tmp501=Cyc_NewControlFlow_anal_Rexp(env,
inflow,(struct Cyc_Absyn_Exp*)_check_null(_tmp4CD));_tmp502=_tmp501.f1;_tmp503=
_tmp501.f2;_tmp502=Cyc_CfFlowInfo_consume_unique_rvals(_tmp4CD->loc,_tmp502);
_tmp502=Cyc_NewControlFlow_use_Rval(env,_tmp4CD->loc,_tmp502,_tmp503);Cyc_NewControlFlow_check_init_params(
s->loc,env,_tmp502);return(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp504;(_tmp504.BottomFL).tag=0;_tmp504;});}_LL31A: if(*((int*)_tmp4CB)!= 0)goto
_LL31C;_tmp4CE=((struct Cyc_Absyn_Exp_s_struct*)_tmp4CB)->f1;_LL31B: outflow=(Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmp4CE)).f1;goto _LL313;_LL31C: if(*((int*)_tmp4CB)!= 1)goto _LL31E;
_tmp4CF=((struct Cyc_Absyn_Seq_s_struct*)_tmp4CB)->f1;_tmp4D0=((struct Cyc_Absyn_Seq_s_struct*)
_tmp4CB)->f2;_LL31D: outflow=Cyc_NewControlFlow_anal_stmt(env,Cyc_NewControlFlow_anal_stmt(
env,inflow,_tmp4CF),_tmp4D0);goto _LL313;_LL31E: if(*((int*)_tmp4CB)!= 3)goto
_LL320;_tmp4D1=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmp4CB)->f1;_tmp4D2=((
struct Cyc_Absyn_IfThenElse_s_struct*)_tmp4CB)->f2;_tmp4D3=((struct Cyc_Absyn_IfThenElse_s_struct*)
_tmp4CB)->f3;_LL31F: {union Cyc_CfFlowInfo_FlowInfo_union _tmp506;union Cyc_CfFlowInfo_FlowInfo_union
_tmp507;struct _tuple8 _tmp505=Cyc_NewControlFlow_anal_test(env,inflow,_tmp4D1);
_tmp506=_tmp505.f1;_tmp507=_tmp505.f2;_tmp506=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp4D1->loc,_tmp506);_tmp507=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp4D1->loc,
_tmp507);outflow=Cyc_CfFlowInfo_join_flow(_tmp4CA,env->all_changed,Cyc_NewControlFlow_anal_stmt(
env,_tmp506,_tmp4D2),Cyc_NewControlFlow_anal_stmt(env,_tmp507,_tmp4D3),1);goto
_LL313;}_LL320: if(*((int*)_tmp4CB)!= 4)goto _LL322;_tmp4D4=((struct Cyc_Absyn_While_s_struct*)
_tmp4CB)->f1;_tmp4D5=_tmp4D4.f1;_tmp4D6=_tmp4D4.f2;_tmp4D7=((struct Cyc_Absyn_While_s_struct*)
_tmp4CB)->f2;_LL321: {union Cyc_CfFlowInfo_FlowInfo_union*_tmp509;struct _tuple9
_tmp508=Cyc_NewControlFlow_pre_stmt_check(env,inflow,_tmp4D6);_tmp509=_tmp508.f2;{
union Cyc_CfFlowInfo_FlowInfo_union _tmp50A=*_tmp509;union Cyc_CfFlowInfo_FlowInfo_union
_tmp50C;union Cyc_CfFlowInfo_FlowInfo_union _tmp50D;struct _tuple8 _tmp50B=Cyc_NewControlFlow_anal_test(
env,_tmp50A,_tmp4D5);_tmp50C=_tmp50B.f1;_tmp50D=_tmp50B.f2;_tmp50C=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp4D5->loc,_tmp50C);_tmp50D=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp4D5->loc,
_tmp50D);{union Cyc_CfFlowInfo_FlowInfo_union _tmp50E=Cyc_NewControlFlow_anal_stmt(
env,_tmp50C,_tmp4D7);Cyc_NewControlFlow_update_flow(env,_tmp4D6,_tmp50E);outflow=
_tmp50D;goto _LL313;}}}_LL322: if(*((int*)_tmp4CB)!= 13)goto _LL324;_tmp4D8=((
struct Cyc_Absyn_Do_s_struct*)_tmp4CB)->f1;_tmp4D9=((struct Cyc_Absyn_Do_s_struct*)
_tmp4CB)->f2;_tmp4DA=_tmp4D9.f1;_tmp4DB=_tmp4D9.f2;_LL323: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp50F=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp4D8);union Cyc_CfFlowInfo_FlowInfo_union*
_tmp511;struct _tuple9 _tmp510=Cyc_NewControlFlow_pre_stmt_check(env,_tmp50F,
_tmp4DB);_tmp511=_tmp510.f2;{union Cyc_CfFlowInfo_FlowInfo_union _tmp512=*_tmp511;
union Cyc_CfFlowInfo_FlowInfo_union _tmp514;union Cyc_CfFlowInfo_FlowInfo_union
_tmp515;struct _tuple8 _tmp513=Cyc_NewControlFlow_anal_test(env,_tmp512,_tmp4DA);
_tmp514=_tmp513.f1;_tmp515=_tmp513.f2;Cyc_NewControlFlow_update_flow(env,_tmp4D8,
_tmp514);outflow=_tmp515;goto _LL313;}}_LL324: if(*((int*)_tmp4CB)!= 8)goto _LL326;
_tmp4DC=((struct Cyc_Absyn_For_s_struct*)_tmp4CB)->f1;_tmp4DD=((struct Cyc_Absyn_For_s_struct*)
_tmp4CB)->f2;_tmp4DE=_tmp4DD.f1;_tmp4DF=_tmp4DD.f2;_tmp4E0=((struct Cyc_Absyn_For_s_struct*)
_tmp4CB)->f3;_tmp4E1=_tmp4E0.f1;_tmp4E2=_tmp4E0.f2;_tmp4E3=((struct Cyc_Absyn_For_s_struct*)
_tmp4CB)->f4;_LL325: {union Cyc_CfFlowInfo_FlowInfo_union _tmp516=(Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmp4DC)).f1;_tmp516=Cyc_CfFlowInfo_drop_unique_rvals(_tmp4DC->loc,
_tmp516);{union Cyc_CfFlowInfo_FlowInfo_union*_tmp518;struct _tuple9 _tmp517=Cyc_NewControlFlow_pre_stmt_check(
env,_tmp516,_tmp4DF);_tmp518=_tmp517.f2;{union Cyc_CfFlowInfo_FlowInfo_union
_tmp519=*_tmp518;union Cyc_CfFlowInfo_FlowInfo_union _tmp51B;union Cyc_CfFlowInfo_FlowInfo_union
_tmp51C;struct _tuple8 _tmp51A=Cyc_NewControlFlow_anal_test(env,_tmp519,_tmp4DE);
_tmp51B=_tmp51A.f1;_tmp51C=_tmp51A.f2;{union Cyc_CfFlowInfo_FlowInfo_union _tmp51D=
Cyc_NewControlFlow_anal_stmt(env,_tmp51B,_tmp4E3);union Cyc_CfFlowInfo_FlowInfo_union*
_tmp51F;struct _tuple9 _tmp51E=Cyc_NewControlFlow_pre_stmt_check(env,_tmp51D,
_tmp4E2);_tmp51F=_tmp51E.f2;{union Cyc_CfFlowInfo_FlowInfo_union _tmp520=*_tmp51F;
union Cyc_CfFlowInfo_FlowInfo_union _tmp521=(Cyc_NewControlFlow_anal_Rexp(env,
_tmp520,_tmp4E1)).f1;_tmp521=Cyc_CfFlowInfo_drop_unique_rvals(_tmp4E1->loc,
_tmp521);Cyc_NewControlFlow_update_flow(env,_tmp4DF,_tmp521);outflow=_tmp51C;
goto _LL313;}}}}}_LL326: if(*((int*)_tmp4CB)!= 5)goto _LL328;_tmp4E4=((struct Cyc_Absyn_Break_s_struct*)
_tmp4CB)->f1;if(_tmp4E4 != 0)goto _LL328;_LL327: return(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp522;(_tmp522.BottomFL).tag=0;_tmp522;});
_LL328: if(*((int*)_tmp4CB)!= 10)goto _LL32A;_tmp4E5=((struct Cyc_Absyn_Fallthru_s_struct*)
_tmp4CB)->f1;_tmp4E6=((struct Cyc_Absyn_Fallthru_s_struct*)_tmp4CB)->f2;if(
_tmp4E6 == 0)goto _LL32A;_tmp4E7=*_tmp4E6;_LL329: {struct _RegionHandle _tmp523=
_new_region("temp");struct _RegionHandle*temp=& _tmp523;_push_region(temp);{union
Cyc_CfFlowInfo_FlowInfo_union _tmp525;struct Cyc_List_List*_tmp526;struct _tuple11
_tmp524=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,_tmp4E5,0);
_tmp525=_tmp524.f1;_tmp526=_tmp524.f2;for(0;_tmp526 != 0;(_tmp526=_tmp526->tl,
_tmp4E5=_tmp4E5->tl)){_tmp525=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp4E5))->hd)->loc,_tmp525,(void*)_tmp526->hd);}
_tmp525=Cyc_CfFlowInfo_consume_unique_rvals(s->loc,_tmp525);_tmp525=Cyc_NewControlFlow_add_vars(
_tmp4CA,_tmp525,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp4E7->pat_vars))->v,(
void*)_tmp4CA->unknown_all,s->loc);Cyc_NewControlFlow_update_flow(env,(struct Cyc_Absyn_Stmt*)
_tmp4E7->body,_tmp525);{union Cyc_CfFlowInfo_FlowInfo_union _tmp528=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp527;(_tmp527.BottomFL).tag=0;_tmp527;});
_npop_handler(0);return _tmp528;}};_pop_region(temp);}_LL32A: if(*((int*)_tmp4CB)
!= 5)goto _LL32C;_tmp4E8=((struct Cyc_Absyn_Break_s_struct*)_tmp4CB)->f1;_LL32B:
_tmp4E9=_tmp4E8;goto _LL32D;_LL32C: if(*((int*)_tmp4CB)!= 6)goto _LL32E;_tmp4E9=((
struct Cyc_Absyn_Continue_s_struct*)_tmp4CB)->f1;_LL32D: _tmp4EA=_tmp4E9;goto
_LL32F;_LL32E: if(*((int*)_tmp4CB)!= 7)goto _LL330;_tmp4EA=((struct Cyc_Absyn_Goto_s_struct*)
_tmp4CB)->f2;_LL32F: if(env->iteration_num == 1){struct Cyc_Absyn_Stmt*_tmp529=
_tmp4C8->encloser;struct Cyc_Absyn_Stmt*_tmp52A=(Cyc_NewControlFlow_get_stmt_annot((
struct Cyc_Absyn_Stmt*)_check_null(_tmp4EA)))->encloser;while(_tmp52A != _tmp529){
struct Cyc_Absyn_Stmt*_tmp52B=(Cyc_NewControlFlow_get_stmt_annot(_tmp529))->encloser;
if(_tmp52B == _tmp529){({void*_tmp52C=0;Cyc_Tcutil_terr(s->loc,({const char*
_tmp52D="goto enters local scope or exception handler";_tag_dyneither(_tmp52D,
sizeof(char),45);}),_tag_dyneither(_tmp52C,sizeof(void*),0));});break;}_tmp529=
_tmp52B;}}Cyc_NewControlFlow_update_flow(env,(struct Cyc_Absyn_Stmt*)_check_null(
_tmp4EA),inflow);return(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp52E;(_tmp52E.BottomFL).tag=0;_tmp52E;});_LL330: if(*((int*)_tmp4CB)!= 9)goto
_LL332;_tmp4EB=((struct Cyc_Absyn_Switch_s_struct*)_tmp4CB)->f1;_tmp4EC=((struct
Cyc_Absyn_Switch_s_struct*)_tmp4CB)->f2;_LL331: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp530;void*_tmp531;struct _tuple5 _tmp52F=Cyc_NewControlFlow_anal_Rexp(env,
inflow,_tmp4EB);_tmp530=_tmp52F.f1;_tmp531=_tmp52F.f2;_tmp530=Cyc_CfFlowInfo_consume_unique_rvals(
_tmp4EB->loc,_tmp530);_tmp530=Cyc_NewControlFlow_use_Rval(env,_tmp4EB->loc,
_tmp530,_tmp531);outflow=Cyc_NewControlFlow_anal_scs(env,_tmp530,_tmp4EC);goto
_LL313;}_LL332: if(*((int*)_tmp4CB)!= 14)goto _LL334;_tmp4ED=((struct Cyc_Absyn_TryCatch_s_struct*)
_tmp4CB)->f1;_tmp4EE=((struct Cyc_Absyn_TryCatch_s_struct*)_tmp4CB)->f2;_LL333: {
int old_in_try=env->in_try;union Cyc_CfFlowInfo_FlowInfo_union old_tryflow=env->tryflow;
env->in_try=1;env->tryflow=inflow;{union Cyc_CfFlowInfo_FlowInfo_union s1_outflow=
Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp4ED);union Cyc_CfFlowInfo_FlowInfo_union
scs_inflow=env->tryflow;env->in_try=old_in_try;env->tryflow=old_tryflow;Cyc_NewControlFlow_update_tryflow(
env,scs_inflow);{union Cyc_CfFlowInfo_FlowInfo_union scs_outflow=Cyc_NewControlFlow_anal_scs(
env,scs_inflow,_tmp4EE);{union Cyc_CfFlowInfo_FlowInfo_union _tmp532=scs_outflow;
_LL341: if((_tmp532.BottomFL).tag != 0)goto _LL343;_LL342: goto _LL340;_LL343:;_LL344:({
void*_tmp533=0;Cyc_Tcutil_terr(s->loc,({const char*_tmp534="last catch clause may implicitly fallthru";
_tag_dyneither(_tmp534,sizeof(char),42);}),_tag_dyneither(_tmp533,sizeof(void*),
0));});_LL340:;}outflow=s1_outflow;goto _LL313;}}}_LL334: if(*((int*)_tmp4CB)!= 11)
goto _LL336;_tmp4EF=((struct Cyc_Absyn_Decl_s_struct*)_tmp4CB)->f1;_tmp4F0=((
struct Cyc_Absyn_Decl_s_struct*)_tmp4CB)->f2;_LL335: outflow=Cyc_NewControlFlow_anal_stmt(
env,Cyc_NewControlFlow_anal_decl(env,inflow,_tmp4EF),_tmp4F0);goto _LL313;_LL336:
if(*((int*)_tmp4CB)!= 12)goto _LL338;_tmp4F1=((struct Cyc_Absyn_Label_s_struct*)
_tmp4CB)->f2;_LL337: outflow=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp4F1);
goto _LL313;_LL338: if(*((int*)_tmp4CB)!= 15)goto _LL33A;_tmp4F2=((struct Cyc_Absyn_Region_s_struct*)
_tmp4CB)->f1;_tmp4F3=((struct Cyc_Absyn_Region_s_struct*)_tmp4CB)->f2;_tmp4F4=((
struct Cyc_Absyn_Region_s_struct*)_tmp4CB)->f3;_tmp4F5=((struct Cyc_Absyn_Region_s_struct*)
_tmp4CB)->f4;_tmp4F6=((struct Cyc_Absyn_Region_s_struct*)_tmp4CB)->f5;_LL339: if(
_tmp4F5 != 0){struct Cyc_Absyn_Exp*_tmp535=(struct Cyc_Absyn_Exp*)_tmp4F5;union Cyc_CfFlowInfo_FlowInfo_union
_tmp537;void*_tmp538;struct _tuple5 _tmp536=Cyc_NewControlFlow_anal_Rexp(env,
inflow,_tmp535);_tmp537=_tmp536.f1;_tmp538=_tmp536.f2;_tmp537=Cyc_CfFlowInfo_consume_unique_rvals(
_tmp535->loc,_tmp537);inflow=Cyc_NewControlFlow_use_Rval(env,_tmp535->loc,
_tmp537,_tmp538);}{struct Cyc_List_List _tmp539=({struct Cyc_List_List _tmp53A;
_tmp53A.hd=_tmp4F3;_tmp53A.tl=0;_tmp53A;});inflow=Cyc_NewControlFlow_add_vars(
_tmp4CA,inflow,(struct Cyc_List_List*)& _tmp539,(void*)_tmp4CA->unknown_all,
_tmp4F6->loc);outflow=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp4F6);goto
_LL313;}_LL33A: if(*((int*)_tmp4CB)!= 16)goto _LL33C;_tmp4F7=((struct Cyc_Absyn_ResetRegion_s_struct*)
_tmp4CB)->f1;_LL33B: {union Cyc_CfFlowInfo_FlowInfo_union _tmp53C;void*_tmp53D;
struct _tuple5 _tmp53B=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp4F7);_tmp53C=
_tmp53B.f1;_tmp53D=_tmp53B.f2;_tmp53C=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp4F7->loc,_tmp53C);{union Cyc_CfFlowInfo_FlowInfo_union _tmp53E=Cyc_NewControlFlow_use_Rval(
env,_tmp4F7->loc,_tmp53C,_tmp53D);{void*_tmp53F=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp4F7->topt))->v);void*_tmp540;_LL346: if(
_tmp53F <= (void*)4)goto _LL348;if(*((int*)_tmp53F)!= 14)goto _LL348;_tmp540=(void*)((
struct Cyc_Absyn_RgnHandleType_struct*)_tmp53F)->f1;_LL347: outflow=Cyc_CfFlowInfo_kill_flow_region(
_tmp4CA,_tmp53C,_tmp540);goto _LL345;_LL348:;_LL349:(int)_throw((void*)({struct
Cyc_Core_Impossible_struct*_tmp541=_cycalloc(sizeof(*_tmp541));_tmp541[0]=({
struct Cyc_Core_Impossible_struct _tmp542;_tmp542.tag=Cyc_Core_Impossible;_tmp542.f1=({
const char*_tmp543="anal_stmt -- reset_region";_tag_dyneither(_tmp543,sizeof(char),
26);});_tmp542;});_tmp541;}));_LL345:;}goto _LL313;}}_LL33C: if(*((int*)_tmp4CB)!= 
17)goto _LL33E;_tmp4F8=((struct Cyc_Absyn_Alias_s_struct*)_tmp4CB)->f1;_tmp4F9=((
struct Cyc_Absyn_Alias_s_struct*)_tmp4CB)->f2;_tmp4FA=((struct Cyc_Absyn_Alias_s_struct*)
_tmp4CB)->f3;_tmp4FB=((struct Cyc_Absyn_Alias_s_struct*)_tmp4CB)->f4;_LL33D: {
union Cyc_CfFlowInfo_FlowInfo_union _tmp545;void*_tmp546;struct _tuple5 _tmp544=Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmp4F8);_tmp545=_tmp544.f1;_tmp546=_tmp544.f2;{struct Cyc_CfFlowInfo_ConsumeInfo
_tmp548;struct Cyc_List_List*_tmp549;struct _tuple6 _tmp547=Cyc_CfFlowInfo_save_consume_info(
_tmp4CA,_tmp545,0);_tmp548=_tmp547.f1;_tmp549=_tmp548.may_consume;_tmp545=Cyc_CfFlowInfo_consume_unique_rvals(
_tmp4F8->loc,_tmp545);_tmp545=Cyc_NewControlFlow_use_Rval(env,_tmp4F8->loc,
_tmp545,_tmp546);{struct Cyc_List_List _tmp54A=({struct Cyc_List_List _tmp551;
_tmp551.hd=_tmp4FA;_tmp551.tl=0;_tmp551;});_tmp545=Cyc_NewControlFlow_add_vars(
_tmp4CA,_tmp545,(struct Cyc_List_List*)& _tmp54A,(void*)_tmp4CA->unknown_all,s->loc);
outflow=Cyc_NewControlFlow_anal_stmt(env,_tmp545,_tmp4FB);{union Cyc_CfFlowInfo_FlowInfo_union
_tmp54B=outflow;struct Cyc_Dict_Dict _tmp54C;struct Cyc_List_List*_tmp54D;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp54E;_LL34B: if((_tmp54B.BottomFL).tag != 0)goto _LL34D;_LL34C: goto _LL34A;_LL34D:
if((_tmp54B.ReachableFL).tag != 1)goto _LL34A;_tmp54C=(_tmp54B.ReachableFL).f1;
_tmp54D=(_tmp54B.ReachableFL).f2;_tmp54E=(_tmp54B.ReachableFL).f3;_LL34E: while(
_tmp549 != 0){struct Cyc_Dict_Dict _tmp54F=_tmp54E.consumed;_tmp54E.consumed=((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict,struct Cyc_CfFlowInfo_Place*))
Cyc_Dict_rdelete)(_tmp4CA->r,_tmp54E.consumed,(struct Cyc_CfFlowInfo_Place*)
_tmp549->hd);if((_tmp54E.consumed).t != _tmp54F.t);_tmp549=_tmp549->tl;}outflow=(
union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union _tmp550;(
_tmp550.ReachableFL).tag=1;(_tmp550.ReachableFL).f1=_tmp54C;(_tmp550.ReachableFL).f2=
_tmp54D;(_tmp550.ReachableFL).f3=_tmp54E;_tmp550;});goto _LL34A;_LL34A:;}goto
_LL313;}}}_LL33E:;_LL33F:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp552=_cycalloc(sizeof(*_tmp552));_tmp552[0]=({struct Cyc_Core_Impossible_struct
_tmp553;_tmp553.tag=Cyc_Core_Impossible;_tmp553.f1=({const char*_tmp554="anal_stmt -- bad stmt syntax or unimplemented stmt form";
_tag_dyneither(_tmp554,sizeof(char),56);});_tmp553;});_tmp552;}));_LL313:;}
outflow=Cyc_CfFlowInfo_drop_unique_rvals(s->loc,outflow);{union Cyc_CfFlowInfo_FlowInfo_union
_tmp555=outflow;struct Cyc_CfFlowInfo_ConsumeInfo _tmp556;_LL350: if((_tmp555.ReachableFL).tag
!= 1)goto _LL352;_tmp556=(_tmp555.ReachableFL).f3;_LL351: goto _LL34F;_LL352:;
_LL353: goto _LL34F;_LL34F:;}return outflow;}}static void Cyc_NewControlFlow_check_nested_fun(
struct Cyc_CfFlowInfo_FlowEnv*,union Cyc_CfFlowInfo_FlowInfo_union inflow,struct Cyc_Absyn_Fndecl*
fd);static union Cyc_CfFlowInfo_FlowInfo_union Cyc_NewControlFlow_anal_decl(struct
Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union inflow,struct
Cyc_Absyn_Decl*decl){void*_tmp557=(void*)decl->r;struct Cyc_Absyn_Vardecl*_tmp558;
struct Cyc_Core_Opt*_tmp559;struct Cyc_Core_Opt _tmp55A;struct Cyc_List_List*_tmp55B;
struct Cyc_Absyn_Exp*_tmp55C;struct Cyc_List_List*_tmp55D;struct Cyc_Absyn_Fndecl*
_tmp55E;_LL355: if(_tmp557 <= (void*)2)goto _LL35D;if(*((int*)_tmp557)!= 0)goto
_LL357;_tmp558=((struct Cyc_Absyn_Var_d_struct*)_tmp557)->f1;_LL356: {struct Cyc_List_List
_tmp55F=({struct Cyc_List_List _tmp56E;_tmp56E.hd=_tmp558;_tmp56E.tl=0;_tmp56E;});
inflow=Cyc_NewControlFlow_add_vars(env->fenv,inflow,(struct Cyc_List_List*)&
_tmp55F,(void*)(env->fenv)->unknown_none,decl->loc);{struct Cyc_Absyn_Exp*_tmp560=
_tmp558->initializer;if(_tmp560 == 0)return inflow;{union Cyc_CfFlowInfo_FlowInfo_union
_tmp562;void*_tmp563;struct _tuple5 _tmp561=Cyc_NewControlFlow_anal_Rexp(env,
inflow,(struct Cyc_Absyn_Exp*)_tmp560);_tmp562=_tmp561.f1;_tmp563=_tmp561.f2;
_tmp562=Cyc_CfFlowInfo_consume_unique_rvals(_tmp560->loc,_tmp562);{union Cyc_CfFlowInfo_FlowInfo_union
_tmp564=_tmp562;struct Cyc_Dict_Dict _tmp565;struct Cyc_List_List*_tmp566;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp567;_LL360: if((_tmp564.BottomFL).tag != 0)goto _LL362;_LL361: return(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp568;(_tmp568.BottomFL).tag=0;_tmp568;});
_LL362: if((_tmp564.ReachableFL).tag != 1)goto _LL35F;_tmp565=(_tmp564.ReachableFL).f1;
_tmp566=(_tmp564.ReachableFL).f2;_tmp567=(_tmp564.ReachableFL).f3;_LL363: _tmp565=
Cyc_CfFlowInfo_assign_place(env->fenv,decl->loc,_tmp565,env->all_changed,({
struct Cyc_CfFlowInfo_Place*_tmp569=_region_malloc(env->r,sizeof(*_tmp569));
_tmp569->root=(void*)((void*)({struct Cyc_CfFlowInfo_VarRoot_struct*_tmp56A=
_region_malloc(env->r,sizeof(*_tmp56A));_tmp56A[0]=({struct Cyc_CfFlowInfo_VarRoot_struct
_tmp56B;_tmp56B.tag=0;_tmp56B.f1=_tmp558;_tmp56B;});_tmp56A;}));_tmp569->fields=
0;_tmp569;}),_tmp563);_tmp566=Cyc_CfFlowInfo_reln_assign_var(env->r,_tmp566,
_tmp558,(struct Cyc_Absyn_Exp*)_check_null(_tmp558->initializer));{union Cyc_CfFlowInfo_FlowInfo_union
_tmp56C=({union Cyc_CfFlowInfo_FlowInfo_union _tmp56D;(_tmp56D.ReachableFL).tag=1;(
_tmp56D.ReachableFL).f1=_tmp565;(_tmp56D.ReachableFL).f2=_tmp566;(_tmp56D.ReachableFL).f3=
_tmp567;_tmp56D;});Cyc_NewControlFlow_update_tryflow(env,(union Cyc_CfFlowInfo_FlowInfo_union)
_tmp56C);return(union Cyc_CfFlowInfo_FlowInfo_union)_tmp56C;}_LL35F:;}}}}_LL357:
if(*((int*)_tmp557)!= 2)goto _LL359;_tmp559=((struct Cyc_Absyn_Let_d_struct*)
_tmp557)->f2;if(_tmp559 == 0)goto _LL359;_tmp55A=*_tmp559;_tmp55B=(struct Cyc_List_List*)
_tmp55A.v;_tmp55C=((struct Cyc_Absyn_Let_d_struct*)_tmp557)->f3;_LL358: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp570;void*_tmp571;struct _tuple5 _tmp56F=Cyc_NewControlFlow_anal_Rexp(env,
inflow,_tmp55C);_tmp570=_tmp56F.f1;_tmp571=_tmp56F.f2;_tmp570=Cyc_CfFlowInfo_consume_unique_rvals(
_tmp55C->loc,_tmp570);_tmp570=Cyc_NewControlFlow_use_Rval(env,_tmp55C->loc,
_tmp570,_tmp571);return Cyc_NewControlFlow_add_vars(env->fenv,_tmp570,_tmp55B,(
void*)(env->fenv)->unknown_all,decl->loc);}_LL359: if(*((int*)_tmp557)!= 3)goto
_LL35B;_tmp55D=((struct Cyc_Absyn_Letv_d_struct*)_tmp557)->f1;_LL35A: return Cyc_NewControlFlow_add_vars(
env->fenv,inflow,_tmp55D,(void*)(env->fenv)->unknown_none,decl->loc);_LL35B: if(*((
int*)_tmp557)!= 1)goto _LL35D;_tmp55E=((struct Cyc_Absyn_Fn_d_struct*)_tmp557)->f1;
_LL35C: Cyc_NewControlFlow_check_nested_fun(env->fenv,inflow,_tmp55E);{void*
_tmp572=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp55E->cached_typ))->v;struct
Cyc_Absyn_Vardecl*_tmp573=(struct Cyc_Absyn_Vardecl*)_check_null(_tmp55E->fn_vardecl);
return Cyc_NewControlFlow_add_vars(env->fenv,inflow,({struct Cyc_List_List*_tmp574=
_region_malloc(env->r,sizeof(*_tmp574));_tmp574->hd=_tmp573;_tmp574->tl=0;
_tmp574;}),(void*)(env->fenv)->unknown_all,decl->loc);}_LL35D:;_LL35E:(int)
_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp575=_cycalloc(sizeof(*
_tmp575));_tmp575[0]=({struct Cyc_Core_Impossible_struct _tmp576;_tmp576.tag=Cyc_Core_Impossible;
_tmp576.f1=({const char*_tmp577="anal_decl: unexpected decl variant";
_tag_dyneither(_tmp577,sizeof(char),35);});_tmp576;});_tmp575;}));_LL354:;}
static void Cyc_NewControlFlow_check_fun(struct Cyc_Absyn_Fndecl*fd){struct
_RegionHandle _tmp578=_new_region("frgn");struct _RegionHandle*frgn=& _tmp578;
_push_region(frgn);{struct Cyc_CfFlowInfo_FlowEnv*fenv=Cyc_CfFlowInfo_new_flow_env(
frgn);Cyc_NewControlFlow_check_nested_fun(fenv,(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp579;(_tmp579.ReachableFL).tag=1;(_tmp579.ReachableFL).f1=
fenv->mt_flowdict;(_tmp579.ReachableFL).f2=0;(_tmp579.ReachableFL).f3=({struct
Cyc_CfFlowInfo_ConsumeInfo _tmp57A;_tmp57A.consumed=fenv->mt_place_set;_tmp57A.may_consume=
0;_tmp57A;});_tmp579;}),fd);};_pop_region(frgn);}static int Cyc_NewControlFlow_hash_ptr(
void*s){return(int)s;}static void Cyc_NewControlFlow_check_nested_fun(struct Cyc_CfFlowInfo_FlowEnv*
fenv,union Cyc_CfFlowInfo_FlowInfo_union inflow,struct Cyc_Absyn_Fndecl*fd){struct
_RegionHandle*_tmp57B=fenv->r;struct Cyc_Position_Segment*_tmp57C=(fd->body)->loc;
inflow=Cyc_NewControlFlow_add_vars(fenv,inflow,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(fd->param_vardecls))->v,(void*)fenv->unknown_all,_tmp57C);{struct Cyc_List_List*
param_roots=0;{union Cyc_CfFlowInfo_FlowInfo_union _tmp57D=inflow;struct Cyc_Dict_Dict
_tmp57E;struct Cyc_List_List*_tmp57F;struct Cyc_CfFlowInfo_ConsumeInfo _tmp580;
_LL365: if((_tmp57D.BottomFL).tag != 0)goto _LL367;_LL366:({void*_tmp581=0;((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmp582="check_fun";_tag_dyneither(_tmp582,sizeof(char),10);}),_tag_dyneither(
_tmp581,sizeof(void*),0));});_LL367: if((_tmp57D.ReachableFL).tag != 1)goto _LL364;
_tmp57E=(_tmp57D.ReachableFL).f1;_tmp57F=(_tmp57D.ReachableFL).f2;_tmp580=(
_tmp57D.ReachableFL).f3;_LL368: {struct Cyc_List_List*atts;{void*_tmp583=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(fd->cached_typ))->v);struct Cyc_Absyn_FnInfo
_tmp584;struct Cyc_List_List*_tmp585;_LL36A: if(_tmp583 <= (void*)4)goto _LL36C;if(*((
int*)_tmp583)!= 8)goto _LL36C;_tmp584=((struct Cyc_Absyn_FnType_struct*)_tmp583)->f1;
_tmp585=_tmp584.attributes;_LL36B: atts=_tmp585;goto _LL369;_LL36C:;_LL36D:({void*
_tmp586=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp587="check_fun: non-function type cached with fndecl_t";
_tag_dyneither(_tmp587,sizeof(char),50);}),_tag_dyneither(_tmp586,sizeof(void*),
0));});_LL369:;}for(0;atts != 0;atts=atts->tl){void*_tmp588=(void*)atts->hd;int
_tmp589;_LL36F: if(_tmp588 <= (void*)17)goto _LL371;if(*((int*)_tmp588)!= 4)goto
_LL371;_tmp589=((struct Cyc_Absyn_Initializes_att_struct*)_tmp588)->f1;_LL370: {
unsigned int j=(unsigned int)_tmp589;if(j > ((int(*)(struct Cyc_List_List*x))Cyc_List_length)((
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v)){({
void*_tmp58A=0;Cyc_Tcutil_terr(_tmp57C,({const char*_tmp58B="initializes attribute exceeds number of parameters";
_tag_dyneither(_tmp58B,sizeof(char),51);}),_tag_dyneither(_tmp58A,sizeof(void*),
0));});continue;}{struct Cyc_Absyn_Vardecl*_tmp58C=((struct Cyc_Absyn_Vardecl*(*)(
struct Cyc_List_List*x,int n))Cyc_List_nth)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(fd->param_vardecls))->v,(int)(j - 1));{void*_tmp58D=Cyc_Tcutil_compress((
void*)_tmp58C->type);struct Cyc_Absyn_PtrInfo _tmp58E;void*_tmp58F;struct Cyc_Absyn_PtrAtts
_tmp590;struct Cyc_Absyn_Conref*_tmp591;struct Cyc_Absyn_Conref*_tmp592;struct Cyc_Absyn_Conref*
_tmp593;_LL374: if(_tmp58D <= (void*)4)goto _LL376;if(*((int*)_tmp58D)!= 4)goto
_LL376;_tmp58E=((struct Cyc_Absyn_PointerType_struct*)_tmp58D)->f1;_tmp58F=(void*)
_tmp58E.elt_typ;_tmp590=_tmp58E.ptr_atts;_tmp591=_tmp590.nullable;_tmp592=
_tmp590.bounds;_tmp593=_tmp590.zero_term;_LL375: if(((int(*)(struct Cyc_Absyn_Conref*
x))Cyc_Absyn_conref_val)(_tmp591))({void*_tmp594=0;Cyc_Tcutil_terr(_tmp57C,({
const char*_tmp595="initializes attribute not allowed on nullable pointers";
_tag_dyneither(_tmp595,sizeof(char),55);}),_tag_dyneither(_tmp594,sizeof(void*),
0));});if(!Cyc_Tcutil_is_bound_one(_tmp592))({void*_tmp596=0;Cyc_Tcutil_terr(
_tmp57C,({const char*_tmp597="initializes attribute allowed only on pointers of size 1";
_tag_dyneither(_tmp597,sizeof(char),57);}),_tag_dyneither(_tmp596,sizeof(void*),
0));});if(((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp593))({
void*_tmp598=0;Cyc_Tcutil_terr(_tmp57C,({const char*_tmp599="initializes attribute allowed only on pointers to non-zero-terminated arrays";
_tag_dyneither(_tmp599,sizeof(char),77);}),_tag_dyneither(_tmp598,sizeof(void*),
0));});{struct Cyc_CfFlowInfo_InitParam_struct*_tmp59A=({struct Cyc_CfFlowInfo_InitParam_struct*
_tmp5A2=_region_malloc(_tmp57B,sizeof(*_tmp5A2));_tmp5A2[0]=({struct Cyc_CfFlowInfo_InitParam_struct
_tmp5A3;_tmp5A3.tag=2;_tmp5A3.f1=(int)j;_tmp5A3.f2=(void*)_tmp58F;_tmp5A3;});
_tmp5A2;});struct Cyc_CfFlowInfo_Place*_tmp59B=({struct Cyc_CfFlowInfo_Place*
_tmp5A1=_region_malloc(_tmp57B,sizeof(*_tmp5A1));_tmp5A1->root=(void*)((void*)
_tmp59A);_tmp5A1->fields=0;_tmp5A1;});_tmp57E=Cyc_Dict_insert(_tmp57E,(void*)
_tmp59A,Cyc_CfFlowInfo_typ_to_absrval(fenv,_tmp58F,(void*)fenv->esc_none));
_tmp57E=Cyc_Dict_insert(_tmp57E,(void*)({struct Cyc_CfFlowInfo_VarRoot_struct*
_tmp59C=_region_malloc(_tmp57B,sizeof(*_tmp59C));_tmp59C[0]=({struct Cyc_CfFlowInfo_VarRoot_struct
_tmp59D;_tmp59D.tag=0;_tmp59D.f1=_tmp58C;_tmp59D;});_tmp59C;}),(void*)({struct
Cyc_CfFlowInfo_AddressOf_struct*_tmp59E=_region_malloc(_tmp57B,sizeof(*_tmp59E));
_tmp59E[0]=({struct Cyc_CfFlowInfo_AddressOf_struct _tmp59F;_tmp59F.tag=2;_tmp59F.f1=
_tmp59B;_tmp59F;});_tmp59E;}));param_roots=({struct Cyc_List_List*_tmp5A0=
_region_malloc(_tmp57B,sizeof(*_tmp5A0));_tmp5A0->hd=_tmp59B;_tmp5A0->tl=
param_roots;_tmp5A0;});goto _LL373;}_LL376:;_LL377:({void*_tmp5A4=0;Cyc_Tcutil_terr(
_tmp57C,({const char*_tmp5A5="initializes attribute on non-pointer";
_tag_dyneither(_tmp5A5,sizeof(char),37);}),_tag_dyneither(_tmp5A4,sizeof(void*),
0));});_LL373:;}goto _LL36E;}}_LL371:;_LL372: goto _LL36E;_LL36E:;}inflow=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp5A6;(_tmp5A6.ReachableFL).tag=1;(_tmp5A6.ReachableFL).f1=
_tmp57E;(_tmp5A6.ReachableFL).f2=_tmp57F;(_tmp5A6.ReachableFL).f3=_tmp580;
_tmp5A6;});}_LL364:;}{int noreturn=Cyc_Tcutil_is_noreturn(Cyc_Tcutil_fndecl2typ(
fd));struct Cyc_Hashtable_Table*flow_table=((struct Cyc_Hashtable_Table*(*)(struct
_RegionHandle*r,int sz,int(*cmp)(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*),int(*
hash)(struct Cyc_Absyn_Stmt*)))Cyc_Hashtable_rcreate)(_tmp57B,33,(int(*)(struct
Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*))Cyc_Core_ptrcmp,(int(*)(struct Cyc_Absyn_Stmt*
s))Cyc_NewControlFlow_hash_ptr);struct Cyc_NewControlFlow_AnalEnv*env=({struct Cyc_NewControlFlow_AnalEnv*
_tmp5AF=_region_malloc(_tmp57B,sizeof(*_tmp5AF));_tmp5AF->r=_tmp57B;_tmp5AF->fenv=
fenv;_tmp5AF->iterate_again=1;_tmp5AF->iteration_num=0;_tmp5AF->in_try=0;_tmp5AF->tryflow=
inflow;_tmp5AF->all_changed=0;_tmp5AF->noreturn=noreturn;_tmp5AF->param_roots=
param_roots;_tmp5AF->flow_table=flow_table;_tmp5AF;});union Cyc_CfFlowInfo_FlowInfo_union
outflow=inflow;while(env->iterate_again  && !Cyc_Position_error_p()){++ env->iteration_num;
env->iterate_again=0;outflow=Cyc_NewControlFlow_anal_stmt(env,inflow,fd->body);
outflow=Cyc_CfFlowInfo_drop_unique_rvals((fd->body)->loc,outflow);}{union Cyc_CfFlowInfo_FlowInfo_union
_tmp5A7=outflow;_LL379: if((_tmp5A7.BottomFL).tag != 0)goto _LL37B;_LL37A: goto
_LL378;_LL37B:;_LL37C: Cyc_NewControlFlow_check_init_params(_tmp57C,env,outflow);
if(noreturn)({void*_tmp5A8=0;Cyc_Tcutil_terr(_tmp57C,({const char*_tmp5A9="`noreturn' function might (implicitly) return";
_tag_dyneither(_tmp5A9,sizeof(char),46);}),_tag_dyneither(_tmp5A8,sizeof(void*),
0));});else{void*_tmp5AA=Cyc_Tcutil_compress((void*)fd->ret_type);_LL37E: if((int)
_tmp5AA != 0)goto _LL380;_LL37F: goto _LL37D;_LL380: if(_tmp5AA <= (void*)4)goto _LL382;
if(*((int*)_tmp5AA)!= 6)goto _LL382;_LL381: goto _LL383;_LL382: if((int)_tmp5AA != 1)
goto _LL384;_LL383: goto _LL385;_LL384: if(_tmp5AA <= (void*)4)goto _LL386;if(*((int*)
_tmp5AA)!= 5)goto _LL386;_LL385:({void*_tmp5AB=0;Cyc_Tcutil_warn(_tmp57C,({const
char*_tmp5AC="function may not return a value";_tag_dyneither(_tmp5AC,sizeof(
char),32);}),_tag_dyneither(_tmp5AB,sizeof(void*),0));});goto _LL37D;_LL386:;
_LL387:({void*_tmp5AD=0;Cyc_Tcutil_terr(_tmp57C,({const char*_tmp5AE="function may not return a value";
_tag_dyneither(_tmp5AE,sizeof(char),32);}),_tag_dyneither(_tmp5AD,sizeof(void*),
0));});goto _LL37D;_LL37D:;}goto _LL378;_LL378:;}}}}void Cyc_NewControlFlow_cf_check(
struct Cyc_List_List*ds){for(0;ds != 0;ds=ds->tl){void*_tmp5B0=(void*)((struct Cyc_Absyn_Decl*)
ds->hd)->r;struct Cyc_Absyn_Fndecl*_tmp5B1;struct Cyc_List_List*_tmp5B2;struct Cyc_List_List*
_tmp5B3;struct Cyc_List_List*_tmp5B4;_LL389: if(_tmp5B0 <= (void*)2)goto _LL393;if(*((
int*)_tmp5B0)!= 1)goto _LL38B;_tmp5B1=((struct Cyc_Absyn_Fn_d_struct*)_tmp5B0)->f1;
_LL38A: Cyc_NewControlFlow_check_fun(_tmp5B1);goto _LL388;_LL38B: if(*((int*)
_tmp5B0)!= 10)goto _LL38D;_tmp5B2=((struct Cyc_Absyn_ExternC_d_struct*)_tmp5B0)->f1;
_LL38C: _tmp5B3=_tmp5B2;goto _LL38E;_LL38D: if(*((int*)_tmp5B0)!= 9)goto _LL38F;
_tmp5B3=((struct Cyc_Absyn_Using_d_struct*)_tmp5B0)->f2;_LL38E: _tmp5B4=_tmp5B3;
goto _LL390;_LL38F: if(*((int*)_tmp5B0)!= 8)goto _LL391;_tmp5B4=((struct Cyc_Absyn_Namespace_d_struct*)
_tmp5B0)->f2;_LL390: Cyc_NewControlFlow_cf_check(_tmp5B4);goto _LL388;_LL391: if(*((
int*)_tmp5B0)!= 11)goto _LL393;_LL392: goto _LL388;_LL393:;_LL394: goto _LL388;_LL388:;}}
