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
struct _RegionHandle*Cyc_Core_heap_region;extern char Cyc_Core_Open_Region[16];
extern char Cyc_Core_Free_Region[16];struct Cyc___cycFILE;extern struct Cyc___cycFILE*
Cyc_stderr;struct Cyc_Cstdio___abstractFILE;struct Cyc_String_pa_struct{int tag;
struct _dyneither_ptr f1;};struct Cyc_Int_pa_struct{int tag;unsigned long f1;};struct
Cyc_Double_pa_struct{int tag;double f1;};struct Cyc_LongDouble_pa_struct{int tag;
long double f1;};struct Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{
int tag;unsigned long*f1;};struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr,
struct _dyneither_ptr);int Cyc_fprintf(struct Cyc___cycFILE*,struct _dyneither_ptr,
struct _dyneither_ptr);struct Cyc_ShortPtr_sa_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_struct{
int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{
int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_struct{int tag;struct
_dyneither_ptr f1;};struct Cyc_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_struct{
int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int tag;struct _dyneither_ptr f1;};
struct _dyneither_ptr Cyc_vrprintf(struct _RegionHandle*,struct _dyneither_ptr,
struct _dyneither_ptr);extern char Cyc_FileCloseError[19];extern char Cyc_FileOpenError[
18];struct Cyc_FileOpenError_struct{char*tag;struct _dyneither_ptr f1;};extern char
Cyc_Lexing_Error[10];struct Cyc_Lexing_Error_struct{char*tag;struct _dyneither_ptr
f1;};struct Cyc_Lexing_lexbuf{void(*refill_buff)(struct Cyc_Lexing_lexbuf*);void*
refill_state;struct _dyneither_ptr lex_buffer;int lex_buffer_len;int lex_abs_pos;int
lex_start_pos;int lex_curr_pos;int lex_last_pos;int lex_last_action;int
lex_eof_reached;};struct Cyc_Lexing_function_lexbuf_state{int(*read_fun)(struct
_dyneither_ptr,int,void*);void*read_fun_state;};struct Cyc_Lexing_lex_tables{
struct _dyneither_ptr lex_base;struct _dyneither_ptr lex_backtrk;struct
_dyneither_ptr lex_default;struct _dyneither_ptr lex_trans;struct _dyneither_ptr
lex_check;};struct Cyc_Lexing_lexbuf*Cyc_Lexing_from_file(struct Cyc___cycFILE*);
struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};struct Cyc_List_List*Cyc_List_list(
struct _dyneither_ptr);int Cyc_List_length(struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_map_c(
void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[
18];void Cyc_List_iter(void(*f)(void*),struct Cyc_List_List*x);void*Cyc_List_fold_right(
void*(*f)(void*,void*),struct Cyc_List_List*x,void*accum);struct Cyc_List_List*Cyc_List_rev(
struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*
x);struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*
y);struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*x,struct Cyc_List_List*
y);struct Cyc_List_List*Cyc_List_flatten(struct Cyc_List_List*x);extern char Cyc_List_Nth[
8];struct Cyc_List_List*Cyc_List_rzip(struct _RegionHandle*r1,struct _RegionHandle*
r2,struct Cyc_List_List*x,struct Cyc_List_List*y);struct _tuple0{struct Cyc_List_List*
f1;struct Cyc_List_List*f2;};struct _tuple0 Cyc_List_rsplit(struct _RegionHandle*r1,
struct _RegionHandle*r2,struct Cyc_List_List*x);unsigned int Cyc_strlen(struct
_dyneither_ptr s);int Cyc_strcmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2);
int Cyc_zstrcmp(struct _dyneither_ptr,struct _dyneither_ptr);int Cyc_zstrptrcmp(
struct _dyneither_ptr*,struct _dyneither_ptr*);struct _dyneither_ptr Cyc_strcat(
struct _dyneither_ptr dest,struct _dyneither_ptr src);struct _dyneither_ptr Cyc_strcpy(
struct _dyneither_ptr dest,struct _dyneither_ptr src);struct _dyneither_ptr Cyc_substring(
struct _dyneither_ptr,int ofs,unsigned int n);struct Cyc_Lineno_Pos{struct
_dyneither_ptr logical_file;struct _dyneither_ptr line;int line_no;int col;};extern
char Cyc_Position_Exit[9];struct Cyc_Position_Segment;struct Cyc_Position_Segment*
Cyc_Position_segment_of_abs(int,int);struct _dyneither_ptr Cyc_Position_string_of_segment(
struct Cyc_Position_Segment*);struct Cyc_Position_Error{struct _dyneither_ptr source;
struct Cyc_Position_Segment*seg;void*kind;struct _dyneither_ptr desc;};struct Cyc_Position_Error*
Cyc_Position_mk_err_parse(struct Cyc_Position_Segment*,struct _dyneither_ptr);
extern char Cyc_Position_Nocontext[14];void Cyc_Position_post_error(struct Cyc_Position_Error*);
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
UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};union Cyc_Absyn_DatatypeInfoU
Cyc_Absyn_UnknownDatatype(struct Cyc_Absyn_UnknownDatatypeInfo);union Cyc_Absyn_DatatypeInfoU
Cyc_Absyn_KnownDatatype(struct Cyc_Absyn_Datatypedecl**);struct Cyc_Absyn_DatatypeInfo{
union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;struct Cyc_Core_Opt*
rgn;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple1*datatype_name;
struct _tuple1*field_name;int is_extensible;};struct
_union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo
val;};struct _tuple2{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*
f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple2 val;
};union Cyc_Absyn_DatatypeFieldInfoU{struct
_union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct
_union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};union Cyc_Absyn_DatatypeFieldInfoU
Cyc_Absyn_UnknownDatatypefield(struct Cyc_Absyn_UnknownDatatypeFieldInfo);struct
Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;struct
Cyc_List_List*targs;};struct _tuple3{void*f1;struct _tuple1*f2;struct Cyc_Core_Opt*
f3;};struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple3 val;};struct
_union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{
struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr
KnownAggr;};union Cyc_Absyn_AggrInfoU Cyc_Absyn_UnknownAggr(void*,struct _tuple1*,
struct Cyc_Core_Opt*);struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;
struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual
tq;struct Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;struct Cyc_Position_Segment*
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
_union_Cnst_String_c String_c;};struct Cyc_Absyn_VarargCallInfo{int num_varargs;
struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_struct{
int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_struct{int tag;
unsigned int f1;};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;
void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;};struct Cyc_Absyn_Const_e_struct{
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
_dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];int Cyc_Absyn_is_qvar_qualified(
struct _tuple1*);struct Cyc_Absyn_Tqual Cyc_Absyn_combine_tqual(struct Cyc_Absyn_Tqual
x,struct Cyc_Absyn_Tqual y);struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(struct Cyc_Position_Segment*);
union Cyc_Absyn_Constraint*Cyc_Absyn_new_conref(void*x);union Cyc_Absyn_Constraint*
Cyc_Absyn_empty_conref();extern union Cyc_Absyn_Constraint*Cyc_Absyn_true_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref;extern union Cyc_Absyn_Constraint*
Cyc_Absyn_bounds_one_conref;extern union Cyc_Absyn_Constraint*Cyc_Absyn_bounds_dyneither_conref;
void*Cyc_Absyn_compress_kb(void*);void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,
struct Cyc_Core_Opt*tenv);void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);void*Cyc_Absyn_int_typ(
void*,void*);extern void*Cyc_Absyn_char_typ;extern void*Cyc_Absyn_uint_typ;extern
void*Cyc_Absyn_sint_typ;extern void*Cyc_Absyn_float_typ;void*Cyc_Absyn_double_typ(
int);void*Cyc_Absyn_starb_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*b,
union Cyc_Absyn_Constraint*zero_term);void*Cyc_Absyn_array_typ(void*elt_type,
struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*num_elts,union Cyc_Absyn_Constraint*
zero_term,struct Cyc_Position_Segment*ztloc);struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(
void*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_New_exp(struct
Cyc_Absyn_Exp*rgn_handle,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_null_exp(struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_true_exp(struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_false_exp(
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(void*,int,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_char_exp(char c,struct
Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_float_exp(struct
_dyneither_ptr f,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(
struct _dyneither_ptr s,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_primop_exp(
void*,struct Cyc_List_List*es,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_prim1_exp(void*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(void*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_eq_exp(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_neq_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_gt_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_lt_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_gte_exp(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_lte_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_assignop_exp(struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_post_inc_exp(struct
Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_post_dec_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_pre_inc_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_pre_dec_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_or_exp(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_unknowncall_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_throw_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_noinstantiate_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_instantiate_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*,struct
Cyc_Absyn_Exp*,int user_cast,void*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct
Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,struct Cyc_Position_Segment*);struct
Cyc_Absyn_Exp*Cyc_Absyn_sizeofexp_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_offsetof_exp(void*,void*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_gentyp_exp(struct Cyc_List_List*,void*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(struct Cyc_Absyn_Exp*,struct
_dyneither_ptr*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_aggrarrow_exp(
struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,struct Cyc_Position_Segment*);struct
Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_tuple_exp(struct Cyc_List_List*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*s,
struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(struct
Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*
e,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct
Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,struct Cyc_Position_Segment*loc);struct
Cyc_Absyn_Stmt*Cyc_Absyn_return_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(struct Cyc_Absyn_Exp*e,struct
Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,struct Cyc_Position_Segment*loc);struct
Cyc_Absyn_Stmt*Cyc_Absyn_while_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s,
struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_break_stmt(struct
Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_continue_stmt(struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_for_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2,struct Cyc_Absyn_Exp*e3,struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_switch_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*,
struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_fallthru_stmt(
struct Cyc_List_List*el,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_do_stmt(
struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_goto_stmt(struct _dyneither_ptr*lab,struct Cyc_Position_Segment*
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
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(struct _tuple1*x,void*t,struct Cyc_Absyn_Exp*
init);struct Cyc_Absyn_AggrdeclImpl*Cyc_Absyn_aggrdecl_impl(struct Cyc_List_List*
exists,struct Cyc_List_List*po,struct Cyc_List_List*fs,int tagged);struct Cyc_Absyn_Decl*
Cyc_Absyn_aggr_decl(void*k,void*s,struct _tuple1*n,struct Cyc_List_List*ts,struct
Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*atts,struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Decl*Cyc_Absyn_datatype_decl(void*s,struct _tuple1*n,struct Cyc_List_List*
ts,struct Cyc_Core_Opt*fs,int is_extensible,struct Cyc_Position_Segment*loc);void*
Cyc_Absyn_function_typ(struct Cyc_List_List*tvs,struct Cyc_Core_Opt*eff_typ,void*
ret_typ,struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*
cyc_varargs,struct Cyc_List_List*rgn_po,struct Cyc_List_List*atts);int Cyc_Absyn_fntype_att(
void*a);extern int Cyc_Absyn_porting_c_code;struct Cyc_Iter_Iter{void*env;int(*next)(
void*env,void*dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;
extern char Cyc_Set_Absent[11];struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,
void*);struct _RegionHandle*r;struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[12];
extern char Cyc_Dict_Absent[11];struct _tuple10{void*f1;void*f2;};struct _tuple10*
Cyc_Dict_rchoose(struct _RegionHandle*r,struct Cyc_Dict_Dict d);struct _tuple10*Cyc_Dict_rchoose(
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
le;int allow_valueof;};void Cyc_Tcutil_warn(struct Cyc_Position_Segment*,struct
_dyneither_ptr fmt,struct _dyneither_ptr ap);void*Cyc_Tcutil_copy_type(void*t);void*
Cyc_Tcutil_compress(void*t);extern struct Cyc_Core_Opt Cyc_Tcutil_rk;extern struct
Cyc_Core_Opt Cyc_Tcutil_trk;extern struct Cyc_Core_Opt Cyc_Tcutil_bk;extern struct Cyc_Core_Opt
Cyc_Tcutil_ik;struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_opt(void*k);void*Cyc_Tcutil_kind_to_bound(
void*k);struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k);struct Cyc_PP_Ppstate;
struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs: 1;
int qvar_to_Cids: 1;int add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int
rewrite_temp_tvars: 1;int print_all_tvars: 1;int print_all_kinds: 1;int
print_all_effects: 1;int print_using_stmts: 1;int print_externC_stmts: 1;int
print_full_evars: 1;int print_zeroterm: 1;int generate_line_directives: 1;int
use_curr_namespace: 1;struct Cyc_List_List*curr_namespace;};void Cyc_Lex_register_typedef(
struct _tuple1*s);void Cyc_Lex_enter_namespace(struct _dyneither_ptr*);void Cyc_Lex_leave_namespace();
void Cyc_Lex_enter_using(struct _tuple1*);void Cyc_Lex_leave_using();int Cyc_Parse_no_register=
0;struct Cyc_Parse_Signed_spec_struct{int tag;struct Cyc_Position_Segment*f1;};
struct Cyc_Parse_Unsigned_spec_struct{int tag;struct Cyc_Position_Segment*f1;};
struct Cyc_Parse_Short_spec_struct{int tag;struct Cyc_Position_Segment*f1;};struct
Cyc_Parse_Long_spec_struct{int tag;struct Cyc_Position_Segment*f1;};struct Cyc_Parse_Type_spec_struct{
int tag;void*f1;struct Cyc_Position_Segment*f2;};struct Cyc_Parse_Decl_spec_struct{
int tag;struct Cyc_Absyn_Decl*f1;};struct Cyc_Parse_Declaration_spec{struct Cyc_Core_Opt*
sc;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*type_specs;int is_inline;struct Cyc_List_List*
attributes;};struct Cyc_Parse_Declarator{struct _tuple1*id;struct Cyc_List_List*tms;
};struct Cyc_Parse_Abstractdeclarator{struct Cyc_List_List*tms;};struct _tuple11{
void*f1;struct Cyc_Core_Opt*f2;};static struct _tuple11 Cyc_Parse_collapse_type_specifiers(
struct Cyc_List_List*ts,struct Cyc_Position_Segment*loc);struct _tuple12{struct Cyc_Absyn_Tqual
f1;void*f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;};static struct _tuple12
Cyc_Parse_apply_tms(struct Cyc_Absyn_Tqual,void*,struct Cyc_List_List*,struct Cyc_List_List*);
static struct Cyc_List_List*Cyc_Parse_parse_result=0;static void Cyc_Parse_err(
struct _dyneither_ptr msg,struct Cyc_Position_Segment*sg){Cyc_Position_post_error(
Cyc_Position_mk_err_parse(sg,msg));}static void*Cyc_Parse_abort(struct Cyc_Position_Segment*
sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){Cyc_Parse_err((struct
_dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap),sg);(int)_throw((void*)
Cyc_Position_Exit);}static void Cyc_Parse_unimp(struct _dyneither_ptr msg,struct Cyc_Position_Segment*
sg){({struct Cyc_String_pa_struct _tmp3;_tmp3.tag=0;_tmp3.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)msg);{struct Cyc_String_pa_struct _tmp2;_tmp2.tag=0;_tmp2.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Position_string_of_segment(sg));{
void*_tmp0[2]={& _tmp2,& _tmp3};Cyc_fprintf(Cyc_stderr,({const char*_tmp1="%s: Warning: Cyclone does not yet support %s\n";
_tag_dyneither(_tmp1,sizeof(char),46);}),_tag_dyneither(_tmp0,sizeof(void*),2));}}});
return;}struct _tuple13{union Cyc_Absyn_Constraint*f1;union Cyc_Absyn_Constraint*f2;
union Cyc_Absyn_Constraint*f3;void*f4;};static struct _tuple13 Cyc_Parse_collapse_pointer_quals(
struct Cyc_Position_Segment*loc,union Cyc_Absyn_Constraint*nullable,union Cyc_Absyn_Constraint*
bound,void*rgn,struct Cyc_List_List*pqs){union Cyc_Absyn_Constraint*zeroterm=((
union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();for(0;pqs != 0;pqs=pqs->tl){
void*_tmp4=(void*)pqs->hd;struct Cyc_Absyn_Exp*_tmp5;void*_tmp6;_LL1: if((int)
_tmp4 != 2)goto _LL3;_LL2: zeroterm=Cyc_Absyn_true_conref;goto _LL0;_LL3: if((int)
_tmp4 != 3)goto _LL5;_LL4: zeroterm=Cyc_Absyn_false_conref;goto _LL0;_LL5: if((int)
_tmp4 != 5)goto _LL7;_LL6: nullable=Cyc_Absyn_true_conref;goto _LL0;_LL7: if((int)
_tmp4 != 4)goto _LL9;_LL8: nullable=Cyc_Absyn_false_conref;goto _LL0;_LL9: if((int)
_tmp4 != 1)goto _LLB;_LLA: bound=Cyc_Absyn_bounds_dyneither_conref;goto _LL0;_LLB:
if((int)_tmp4 != 0)goto _LLD;_LLC: bound=Cyc_Absyn_bounds_one_conref;goto _LL0;_LLD:
if(_tmp4 <= (void*)6)goto _LLF;if(*((int*)_tmp4)!= 0)goto _LLF;_tmp5=((struct Cyc_Absyn_Numelts_ptrqual_struct*)
_tmp4)->f1;_LLE: bound=Cyc_Absyn_new_conref((void*)({struct Cyc_Absyn_Upper_b_struct*
_tmp7=_cycalloc(sizeof(*_tmp7));_tmp7[0]=({struct Cyc_Absyn_Upper_b_struct _tmp8;
_tmp8.tag=0;_tmp8.f1=_tmp5;_tmp8;});_tmp7;}));goto _LL0;_LLF: if(_tmp4 <= (void*)6)
goto _LL0;if(*((int*)_tmp4)!= 1)goto _LL0;_tmp6=(void*)((struct Cyc_Absyn_Region_ptrqual_struct*)
_tmp4)->f1;_LL10: rgn=_tmp6;goto _LL0;_LL0:;}return({struct _tuple13 _tmp9;_tmp9.f1=
nullable;_tmp9.f2=bound;_tmp9.f3=zeroterm;_tmp9.f4=rgn;_tmp9;});}static int Cyc_Parse_enum_counter=
0;struct _tuple1*Cyc_Parse_gensym_enum(){return({struct _tuple1*_tmpA=_cycalloc(
sizeof(*_tmpA));_tmpA->f1=Cyc_Absyn_Rel_n(0);_tmpA->f2=({struct _dyneither_ptr*
_tmpB=_cycalloc(sizeof(*_tmpB));_tmpB[0]=(struct _dyneither_ptr)({struct Cyc_Int_pa_struct
_tmpE;_tmpE.tag=1;_tmpE.f1=(unsigned long)Cyc_Parse_enum_counter ++;{void*_tmpC[1]={&
_tmpE};Cyc_aprintf(({const char*_tmpD="__anonymous_enum_%d__";_tag_dyneither(
_tmpD,sizeof(char),22);}),_tag_dyneither(_tmpC,sizeof(void*),1));}});_tmpB;});
_tmpA;});}struct _tuple14{struct _tuple1*f1;struct Cyc_Absyn_Tqual f2;void*f3;struct
Cyc_List_List*f4;struct Cyc_List_List*f5;};struct _tuple15{struct _tuple14*f1;
struct Cyc_Absyn_Exp*f2;};static struct Cyc_Absyn_Aggrfield*Cyc_Parse_make_aggr_field(
struct Cyc_Position_Segment*loc,struct _tuple15*field_info){struct _tuple15 _tmp10;
struct _tuple14*_tmp11;struct _tuple14 _tmp12;struct _tuple1*_tmp13;struct Cyc_Absyn_Tqual
_tmp14;void*_tmp15;struct Cyc_List_List*_tmp16;struct Cyc_List_List*_tmp17;struct
Cyc_Absyn_Exp*_tmp18;struct _tuple15*_tmpF=field_info;_tmp10=*_tmpF;_tmp11=_tmp10.f1;
_tmp12=*_tmp11;_tmp13=_tmp12.f1;_tmp14=_tmp12.f2;_tmp15=_tmp12.f3;_tmp16=_tmp12.f4;
_tmp17=_tmp12.f5;_tmp18=_tmp10.f2;if(_tmp16 != 0)Cyc_Parse_err(({const char*_tmp19="bad type params in struct field";
_tag_dyneither(_tmp19,sizeof(char),32);}),loc);if(Cyc_Absyn_is_qvar_qualified(
_tmp13))Cyc_Parse_err(({const char*_tmp1A="struct or union field cannot be qualified with a namespace";
_tag_dyneither(_tmp1A,sizeof(char),59);}),loc);return({struct Cyc_Absyn_Aggrfield*
_tmp1B=_cycalloc(sizeof(*_tmp1B));_tmp1B->name=(*_tmp13).f2;_tmp1B->tq=_tmp14;
_tmp1B->type=_tmp15;_tmp1B->width=_tmp18;_tmp1B->attributes=_tmp17;_tmp1B;});}
static void*Cyc_Parse_type_spec(void*t,struct Cyc_Position_Segment*loc){return(
void*)({struct Cyc_Parse_Type_spec_struct*_tmp1C=_cycalloc(sizeof(*_tmp1C));
_tmp1C[0]=({struct Cyc_Parse_Type_spec_struct _tmp1D;_tmp1D.tag=4;_tmp1D.f1=(void*)
t;_tmp1D.f2=loc;_tmp1D;});_tmp1C;});}static void*Cyc_Parse_array2ptr(void*t,int
argposn){void*_tmp1E=t;struct Cyc_Absyn_ArrayInfo _tmp1F;void*_tmp20;struct Cyc_Absyn_Tqual
_tmp21;struct Cyc_Absyn_Exp*_tmp22;union Cyc_Absyn_Constraint*_tmp23;struct Cyc_Position_Segment*
_tmp24;_LL12: if(_tmp1E <= (void*)4)goto _LL14;if(*((int*)_tmp1E)!= 7)goto _LL14;
_tmp1F=((struct Cyc_Absyn_ArrayType_struct*)_tmp1E)->f1;_tmp20=_tmp1F.elt_type;
_tmp21=_tmp1F.tq;_tmp22=_tmp1F.num_elts;_tmp23=_tmp1F.zero_term;_tmp24=_tmp1F.zt_loc;
_LL13: return Cyc_Absyn_starb_typ(_tmp20,argposn?Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_rk,0):(void*)2,_tmp21,_tmp22 == 0?(void*)0:(void*)({struct Cyc_Absyn_Upper_b_struct*
_tmp25=_cycalloc(sizeof(*_tmp25));_tmp25[0]=({struct Cyc_Absyn_Upper_b_struct
_tmp26;_tmp26.tag=0;_tmp26.f1=(struct Cyc_Absyn_Exp*)_tmp22;_tmp26;});_tmp25;}),
_tmp23);_LL14:;_LL15: return t;_LL11:;}static void Cyc_Parse_arg_array2ptr(struct
_tuple8*x){(*x).f3=Cyc_Parse_array2ptr((*x).f3,1);}struct _tuple16{struct Cyc_Absyn_Tqual
f1;void*f2;};static struct _tuple16*Cyc_Parse_get_tqual_typ(struct Cyc_Position_Segment*
loc,struct _tuple8*t){return({struct _tuple16*_tmp27=_cycalloc(sizeof(*_tmp27));
_tmp27->f1=(*t).f2;_tmp27->f2=(*t).f3;_tmp27;});}static struct Cyc_Absyn_Vardecl*
Cyc_Parse_decl2vardecl(struct Cyc_Absyn_Decl*d){void*_tmp28=d->r;struct Cyc_Absyn_Vardecl*
_tmp29;_LL17: if(_tmp28 <= (void*)2)goto _LL19;if(*((int*)_tmp28)!= 0)goto _LL19;
_tmp29=((struct Cyc_Absyn_Var_d_struct*)_tmp28)->f1;_LL18: return _tmp29;_LL19:;
_LL1A:({void*_tmp2A=0;((int(*)(struct Cyc_Position_Segment*sg,struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(d->loc,({const char*
_tmp2B="bad declaration in `forarray' statement";_tag_dyneither(_tmp2B,sizeof(
char),40);}),_tag_dyneither(_tmp2A,sizeof(void*),0));});_LL16:;}static int Cyc_Parse_is_typeparam(
void*tm){void*_tmp2C=tm;_LL1C: if(*((int*)_tmp2C)!= 4)goto _LL1E;_LL1D: return 1;
_LL1E:;_LL1F: return 0;_LL1B:;}static void*Cyc_Parse_id2type(struct _dyneither_ptr s,
void*k){if(Cyc_zstrcmp((struct _dyneither_ptr)s,({const char*_tmp2D="`H";
_tag_dyneither(_tmp2D,sizeof(char),3);}))== 0)return(void*)2;else{if(Cyc_zstrcmp((
struct _dyneither_ptr)s,({const char*_tmp2E="`U";_tag_dyneither(_tmp2E,sizeof(char),
3);}))== 0)return(void*)3;else{return(void*)({struct Cyc_Absyn_VarType_struct*
_tmp2F=_cycalloc(sizeof(*_tmp2F));_tmp2F[0]=({struct Cyc_Absyn_VarType_struct
_tmp30;_tmp30.tag=1;_tmp30.f1=({struct Cyc_Absyn_Tvar*_tmp31=_cycalloc(sizeof(*
_tmp31));_tmp31->name=({struct _dyneither_ptr*_tmp32=_cycalloc(sizeof(*_tmp32));
_tmp32[0]=s;_tmp32;});_tmp31->identity=- 1;_tmp31->kind=k;_tmp31;});_tmp30;});
_tmp2F;});}}}static struct Cyc_Absyn_Tvar*Cyc_Parse_copy_tvar(struct Cyc_Absyn_Tvar*
t){void*k;{void*_tmp33=Cyc_Absyn_compress_kb(t->kind);void*_tmp34;_LL21: if(*((
int*)_tmp33)!= 1)goto _LL23;_LL22: k=(void*)({struct Cyc_Absyn_Unknown_kb_struct*
_tmp35=_cycalloc(sizeof(*_tmp35));_tmp35[0]=({struct Cyc_Absyn_Unknown_kb_struct
_tmp36;_tmp36.tag=1;_tmp36.f1=0;_tmp36;});_tmp35;});goto _LL20;_LL23: if(*((int*)
_tmp33)!= 2)goto _LL25;_tmp34=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp33)->f2;
_LL24: k=(void*)({struct Cyc_Absyn_Less_kb_struct*_tmp37=_cycalloc(sizeof(*_tmp37));
_tmp37[0]=({struct Cyc_Absyn_Less_kb_struct _tmp38;_tmp38.tag=2;_tmp38.f1=0;_tmp38.f2=(
void*)_tmp34;_tmp38;});_tmp37;});goto _LL20;_LL25:;_LL26: k=_tmp33;goto _LL20;_LL20:;}
return({struct Cyc_Absyn_Tvar*_tmp39=_cycalloc(sizeof(*_tmp39));_tmp39->name=t->name;
_tmp39->identity=- 1;_tmp39->kind=k;_tmp39;});}static struct Cyc_Absyn_Tvar*Cyc_Parse_typ2tvar(
struct Cyc_Position_Segment*loc,void*t){void*_tmp3A=t;struct Cyc_Absyn_Tvar*_tmp3B;
_LL28: if(_tmp3A <= (void*)4)goto _LL2A;if(*((int*)_tmp3A)!= 1)goto _LL2A;_tmp3B=((
struct Cyc_Absyn_VarType_struct*)_tmp3A)->f1;_LL29: return _tmp3B;_LL2A:;_LL2B:({
void*_tmp3C=0;((int(*)(struct Cyc_Position_Segment*sg,struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Parse_abort)(loc,({const char*_tmp3D="expecting a list of type variables, not types";
_tag_dyneither(_tmp3D,sizeof(char),46);}),_tag_dyneither(_tmp3C,sizeof(void*),0));});
_LL27:;}static void*Cyc_Parse_tvar2typ(struct Cyc_Absyn_Tvar*pr){return(void*)({
struct Cyc_Absyn_VarType_struct*_tmp3E=_cycalloc(sizeof(*_tmp3E));_tmp3E[0]=({
struct Cyc_Absyn_VarType_struct _tmp3F;_tmp3F.tag=1;_tmp3F.f1=pr;_tmp3F;});_tmp3E;});}
static void Cyc_Parse_set_vartyp_kind(void*t,void*k,int leq){void*_tmp40=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_Tvar*_tmp41;struct Cyc_Absyn_Tvar _tmp42;void*_tmp43;void**
_tmp44;_LL2D: if(_tmp40 <= (void*)4)goto _LL2F;if(*((int*)_tmp40)!= 1)goto _LL2F;
_tmp41=((struct Cyc_Absyn_VarType_struct*)_tmp40)->f1;_tmp42=*_tmp41;_tmp43=
_tmp42.kind;_tmp44=(void**)&(*((struct Cyc_Absyn_VarType_struct*)_tmp40)->f1).kind;
_LL2E: {void*_tmp45=Cyc_Absyn_compress_kb(*_tmp44);_LL32: if(*((int*)_tmp45)!= 1)
goto _LL34;_LL33: if(!leq)*_tmp44=Cyc_Tcutil_kind_to_bound(k);else{*_tmp44=(void*)({
struct Cyc_Absyn_Less_kb_struct*_tmp46=_cycalloc(sizeof(*_tmp46));_tmp46[0]=({
struct Cyc_Absyn_Less_kb_struct _tmp47;_tmp47.tag=2;_tmp47.f1=0;_tmp47.f2=(void*)k;
_tmp47;});_tmp46;});}return;_LL34:;_LL35: return;_LL31:;}_LL2F:;_LL30: return;
_LL2C:;}static struct Cyc_List_List*Cyc_Parse_oldstyle2newstyle(struct Cyc_List_List*
tms,struct Cyc_List_List*tds,struct Cyc_Position_Segment*loc){if(tds == 0)return tms;
if(tms == 0)return 0;{void*_tmp48=(void*)tms->hd;void*_tmp49;_LL37: if(*((int*)
_tmp48)!= 3)goto _LL39;_tmp49=(void*)((struct Cyc_Absyn_Function_mod_struct*)
_tmp48)->f1;_LL38: if(tms->tl == 0  || Cyc_Parse_is_typeparam((void*)((struct Cyc_List_List*)
_check_null(tms->tl))->hd) && ((struct Cyc_List_List*)_check_null(tms->tl))->tl == 
0){void*_tmp4A=_tmp49;struct Cyc_List_List*_tmp4B;_LL3C: if(*((int*)_tmp4A)!= 1)
goto _LL3E;_LL3D:({void*_tmp4C=0;Cyc_Tcutil_warn(loc,({const char*_tmp4D="function declaration with both new- and old-style parameter declarations; ignoring old-style";
_tag_dyneither(_tmp4D,sizeof(char),93);}),_tag_dyneither(_tmp4C,sizeof(void*),0));});
return tms;_LL3E: if(*((int*)_tmp4A)!= 0)goto _LL3B;_tmp4B=((struct Cyc_Absyn_NoTypes_struct*)
_tmp4A)->f1;_LL3F: if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp4B)!= ((
int(*)(struct Cyc_List_List*x))Cyc_List_length)(tds))({void*_tmp4E=0;((int(*)(
struct Cyc_Position_Segment*sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Parse_abort)(loc,({const char*_tmp4F="wrong number of parameter declarations in old-style function declaration";
_tag_dyneither(_tmp4F,sizeof(char),73);}),_tag_dyneither(_tmp4E,sizeof(void*),0));});{
struct Cyc_List_List*_tmp50=0;for(0;_tmp4B != 0;_tmp4B=_tmp4B->tl){struct Cyc_List_List*
_tmp51=tds;for(0;_tmp51 != 0;_tmp51=_tmp51->tl){struct Cyc_Absyn_Decl*_tmp52=(
struct Cyc_Absyn_Decl*)_tmp51->hd;void*_tmp53=_tmp52->r;struct Cyc_Absyn_Vardecl*
_tmp54;_LL41: if(_tmp53 <= (void*)2)goto _LL43;if(*((int*)_tmp53)!= 0)goto _LL43;
_tmp54=((struct Cyc_Absyn_Var_d_struct*)_tmp53)->f1;_LL42: if(Cyc_zstrptrcmp((*
_tmp54->name).f2,(struct _dyneither_ptr*)_tmp4B->hd)!= 0)continue;if(_tmp54->initializer
!= 0)({void*_tmp55=0;((int(*)(struct Cyc_Position_Segment*sg,struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(_tmp52->loc,({const char*_tmp56="initializer found in parameter declaration";
_tag_dyneither(_tmp56,sizeof(char),43);}),_tag_dyneither(_tmp55,sizeof(void*),0));});
if(Cyc_Absyn_is_qvar_qualified(_tmp54->name))({void*_tmp57=0;((int(*)(struct Cyc_Position_Segment*
sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(_tmp52->loc,({
const char*_tmp58="namespaces forbidden in parameter declarations";_tag_dyneither(
_tmp58,sizeof(char),47);}),_tag_dyneither(_tmp57,sizeof(void*),0));});_tmp50=({
struct Cyc_List_List*_tmp59=_cycalloc(sizeof(*_tmp59));_tmp59->hd=({struct _tuple8*
_tmp5A=_cycalloc(sizeof(*_tmp5A));_tmp5A->f1=({struct Cyc_Core_Opt*_tmp5B=
_cycalloc(sizeof(*_tmp5B));_tmp5B->v=(*_tmp54->name).f2;_tmp5B;});_tmp5A->f2=
_tmp54->tq;_tmp5A->f3=_tmp54->type;_tmp5A;});_tmp59->tl=_tmp50;_tmp59;});goto L;
_LL43:;_LL44:({void*_tmp5C=0;((int(*)(struct Cyc_Position_Segment*sg,struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(_tmp52->loc,({const
char*_tmp5D="nonvariable declaration in parameter type";_tag_dyneither(_tmp5D,
sizeof(char),42);}),_tag_dyneither(_tmp5C,sizeof(void*),0));});_LL40:;}L: if(
_tmp51 == 0)({struct Cyc_String_pa_struct _tmp60;_tmp60.tag=0;_tmp60.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)_tmp4B->hd));{
void*_tmp5E[1]={& _tmp60};((int(*)(struct Cyc_Position_Segment*sg,struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(loc,({const char*
_tmp5F="%s is not given a type";_tag_dyneither(_tmp5F,sizeof(char),23);}),
_tag_dyneither(_tmp5E,sizeof(void*),1));}});}return({struct Cyc_List_List*_tmp61=
_cycalloc(sizeof(*_tmp61));_tmp61->hd=(void*)((void*)({struct Cyc_Absyn_Function_mod_struct*
_tmp62=_cycalloc(sizeof(*_tmp62));_tmp62[0]=({struct Cyc_Absyn_Function_mod_struct
_tmp63;_tmp63.tag=3;_tmp63.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*
_tmp64=_cycalloc(sizeof(*_tmp64));_tmp64[0]=({struct Cyc_Absyn_WithTypes_struct
_tmp65;_tmp65.tag=1;_tmp65.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(_tmp50);_tmp65.f2=0;_tmp65.f3=0;_tmp65.f4=0;_tmp65.f5=0;_tmp65;});
_tmp64;}));_tmp63;});_tmp62;}));_tmp61->tl=0;_tmp61;});}_LL3B:;}goto _LL3A;_LL39:;
_LL3A: return({struct Cyc_List_List*_tmp66=_cycalloc(sizeof(*_tmp66));_tmp66->hd=(
void*)((void*)tms->hd);_tmp66->tl=Cyc_Parse_oldstyle2newstyle(tms->tl,tds,loc);
_tmp66;});_LL36:;}}struct _tuple17{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual
f2;void*f3;};static struct Cyc_Absyn_Fndecl*Cyc_Parse_make_function(struct Cyc_Core_Opt*
dso,struct Cyc_Parse_Declarator*d,struct Cyc_List_List*tds,struct Cyc_Absyn_Stmt*
body,struct Cyc_Position_Segment*loc){if(tds != 0)d=({struct Cyc_Parse_Declarator*
_tmp67=_cycalloc(sizeof(*_tmp67));_tmp67->id=d->id;_tmp67->tms=Cyc_Parse_oldstyle2newstyle(
d->tms,tds,loc);_tmp67;});{void*sc=(void*)2;struct Cyc_List_List*tss=0;struct Cyc_Absyn_Tqual
tq=Cyc_Absyn_empty_tqual(0);int is_inline=0;struct Cyc_List_List*atts=0;if(dso != 0){
tss=((struct Cyc_Parse_Declaration_spec*)dso->v)->type_specs;tq=((struct Cyc_Parse_Declaration_spec*)
dso->v)->tq;is_inline=((struct Cyc_Parse_Declaration_spec*)dso->v)->is_inline;
atts=((struct Cyc_Parse_Declaration_spec*)dso->v)->attributes;if(((struct Cyc_Parse_Declaration_spec*)
dso->v)->sc != 0){void*_tmp68=(void*)((struct Cyc_Core_Opt*)_check_null(((struct
Cyc_Parse_Declaration_spec*)dso->v)->sc))->v;_LL46: if((int)_tmp68 != 1)goto _LL48;
_LL47: sc=(void*)3;goto _LL45;_LL48: if((int)_tmp68 != 3)goto _LL4A;_LL49: sc=(void*)0;
goto _LL45;_LL4A:;_LL4B: Cyc_Parse_err(({const char*_tmp69="bad storage class on function";
_tag_dyneither(_tmp69,sizeof(char),30);}),loc);goto _LL45;_LL45:;}}{void*_tmp6B;
struct Cyc_Core_Opt*_tmp6C;struct _tuple11 _tmp6A=Cyc_Parse_collapse_type_specifiers(
tss,loc);_tmp6B=_tmp6A.f1;_tmp6C=_tmp6A.f2;{struct Cyc_Absyn_Tqual _tmp6E;void*
_tmp6F;struct Cyc_List_List*_tmp70;struct Cyc_List_List*_tmp71;struct _tuple12
_tmp6D=Cyc_Parse_apply_tms(tq,_tmp6B,atts,d->tms);_tmp6E=_tmp6D.f1;_tmp6F=_tmp6D.f2;
_tmp70=_tmp6D.f3;_tmp71=_tmp6D.f4;if(_tmp6C != 0)({void*_tmp72=0;Cyc_Tcutil_warn(
loc,({const char*_tmp73="nested type declaration within function prototype";
_tag_dyneither(_tmp73,sizeof(char),50);}),_tag_dyneither(_tmp72,sizeof(void*),0));});
if(_tmp70 != 0)({void*_tmp74=0;Cyc_Tcutil_warn(loc,({const char*_tmp75="bad type params, ignoring";
_tag_dyneither(_tmp75,sizeof(char),26);}),_tag_dyneither(_tmp74,sizeof(void*),0));});{
void*_tmp76=_tmp6F;struct Cyc_Absyn_FnInfo _tmp77;struct Cyc_List_List*_tmp78;
struct Cyc_Core_Opt*_tmp79;void*_tmp7A;struct Cyc_List_List*_tmp7B;int _tmp7C;
struct Cyc_Absyn_VarargInfo*_tmp7D;struct Cyc_List_List*_tmp7E;struct Cyc_List_List*
_tmp7F;_LL4D: if(_tmp76 <= (void*)4)goto _LL4F;if(*((int*)_tmp76)!= 8)goto _LL4F;
_tmp77=((struct Cyc_Absyn_FnType_struct*)_tmp76)->f1;_tmp78=_tmp77.tvars;_tmp79=
_tmp77.effect;_tmp7A=_tmp77.ret_typ;_tmp7B=_tmp77.args;_tmp7C=_tmp77.c_varargs;
_tmp7D=_tmp77.cyc_varargs;_tmp7E=_tmp77.rgn_po;_tmp7F=_tmp77.attributes;_LL4E: {
struct Cyc_List_List*_tmp80=0;{struct Cyc_List_List*_tmp81=_tmp7B;for(0;_tmp81 != 0;
_tmp81=_tmp81->tl){struct _tuple8 _tmp83;struct Cyc_Core_Opt*_tmp84;struct Cyc_Absyn_Tqual
_tmp85;void*_tmp86;struct _tuple8*_tmp82=(struct _tuple8*)_tmp81->hd;_tmp83=*
_tmp82;_tmp84=_tmp83.f1;_tmp85=_tmp83.f2;_tmp86=_tmp83.f3;if(_tmp84 == 0){Cyc_Parse_err(({
const char*_tmp87="missing argument variable in function prototype";
_tag_dyneither(_tmp87,sizeof(char),48);}),loc);_tmp80=({struct Cyc_List_List*
_tmp88=_cycalloc(sizeof(*_tmp88));_tmp88->hd=({struct _tuple17*_tmp89=_cycalloc(
sizeof(*_tmp89));_tmp89->f1=_init_dyneither_ptr(_cycalloc(sizeof(struct
_dyneither_ptr)),"?",sizeof(char),2);_tmp89->f2=_tmp85;_tmp89->f3=_tmp86;_tmp89;});
_tmp88->tl=_tmp80;_tmp88;});}else{_tmp80=({struct Cyc_List_List*_tmp8B=_cycalloc(
sizeof(*_tmp8B));_tmp8B->hd=({struct _tuple17*_tmp8C=_cycalloc(sizeof(*_tmp8C));
_tmp8C->f1=(struct _dyneither_ptr*)_tmp84->v;_tmp8C->f2=_tmp85;_tmp8C->f3=_tmp86;
_tmp8C;});_tmp8B->tl=_tmp80;_tmp8B;});}}}return({struct Cyc_Absyn_Fndecl*_tmp8D=
_cycalloc(sizeof(*_tmp8D));_tmp8D->sc=sc;_tmp8D->name=d->id;_tmp8D->tvs=_tmp78;
_tmp8D->is_inline=is_inline;_tmp8D->effect=_tmp79;_tmp8D->ret_type=_tmp7A;_tmp8D->args=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp80);_tmp8D->c_varargs=
_tmp7C;_tmp8D->cyc_varargs=_tmp7D;_tmp8D->rgn_po=_tmp7E;_tmp8D->body=body;_tmp8D->cached_typ=
0;_tmp8D->param_vardecls=0;_tmp8D->fn_vardecl=0;_tmp8D->attributes=Cyc_List_append(
_tmp7F,_tmp71);_tmp8D;});}_LL4F:;_LL50:({void*_tmp8E=0;((int(*)(struct Cyc_Position_Segment*
sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(loc,({const
char*_tmp8F="declarator is not a function prototype";_tag_dyneither(_tmp8F,
sizeof(char),39);}),_tag_dyneither(_tmp8E,sizeof(void*),0));});_LL4C:;}}}}}
static char _tmp90[52]="at most one type may appear within a type specifier";
static struct _dyneither_ptr Cyc_Parse_msg1={_tmp90,_tmp90,_tmp90 + 52};static char
_tmp91[63]="const or volatile may appear only once within a type specifier";
static struct _dyneither_ptr Cyc_Parse_msg2={_tmp91,_tmp91,_tmp91 + 63};static char
_tmp92[50]="type specifier includes more than one declaration";static struct
_dyneither_ptr Cyc_Parse_msg3={_tmp92,_tmp92,_tmp92 + 50};static char _tmp93[60]="sign specifier may appear only once within a type specifier";
static struct _dyneither_ptr Cyc_Parse_msg4={_tmp93,_tmp93,_tmp93 + 60};static struct
_tuple11 Cyc_Parse_collapse_type_specifiers(struct Cyc_List_List*ts,struct Cyc_Position_Segment*
loc){struct Cyc_Core_Opt*declopt=0;int seen_type=0;int seen_sign=0;int seen_size=0;
void*t=(void*)0;void*sz=(void*)2;void*sgn=(void*)0;struct Cyc_Position_Segment*
last_loc=loc;for(0;ts != 0;ts=ts->tl){void*_tmp94=(void*)ts->hd;void*_tmp95;
struct Cyc_Position_Segment*_tmp96;struct Cyc_Position_Segment*_tmp97;struct Cyc_Position_Segment*
_tmp98;struct Cyc_Position_Segment*_tmp99;struct Cyc_Position_Segment*_tmp9A;
struct Cyc_Absyn_Decl*_tmp9B;_LL52: if(*((int*)_tmp94)!= 4)goto _LL54;_tmp95=(void*)((
struct Cyc_Parse_Type_spec_struct*)_tmp94)->f1;_tmp96=((struct Cyc_Parse_Type_spec_struct*)
_tmp94)->f2;_LL53: if(seen_type)Cyc_Parse_err(Cyc_Parse_msg1,_tmp96);last_loc=
_tmp96;seen_type=1;t=_tmp95;goto _LL51;_LL54: if(*((int*)_tmp94)!= 0)goto _LL56;
_tmp97=((struct Cyc_Parse_Signed_spec_struct*)_tmp94)->f1;_LL55: if(seen_sign)Cyc_Parse_err(
Cyc_Parse_msg4,_tmp97);if(seen_type)Cyc_Parse_err(({const char*_tmp9C="signed qualifier must come before type";
_tag_dyneither(_tmp9C,sizeof(char),39);}),_tmp97);last_loc=_tmp97;seen_sign=1;
sgn=(void*)0;goto _LL51;_LL56: if(*((int*)_tmp94)!= 1)goto _LL58;_tmp98=((struct Cyc_Parse_Unsigned_spec_struct*)
_tmp94)->f1;_LL57: if(seen_sign)Cyc_Parse_err(Cyc_Parse_msg4,_tmp98);if(seen_type)
Cyc_Parse_err(({const char*_tmp9D="signed qualifier must come before type";
_tag_dyneither(_tmp9D,sizeof(char),39);}),_tmp98);last_loc=_tmp98;seen_sign=1;
sgn=(void*)1;goto _LL51;_LL58: if(*((int*)_tmp94)!= 2)goto _LL5A;_tmp99=((struct Cyc_Parse_Short_spec_struct*)
_tmp94)->f1;_LL59: if(seen_size)Cyc_Parse_err(({const char*_tmp9E="integral size may appear only once within a type specifier";
_tag_dyneither(_tmp9E,sizeof(char),59);}),_tmp99);if(seen_type)Cyc_Parse_err(({
const char*_tmp9F="short modifier must come before base type";_tag_dyneither(
_tmp9F,sizeof(char),42);}),_tmp99);last_loc=_tmp99;seen_size=1;sz=(void*)1;goto
_LL51;_LL5A: if(*((int*)_tmp94)!= 3)goto _LL5C;_tmp9A=((struct Cyc_Parse_Long_spec_struct*)
_tmp94)->f1;_LL5B: if(seen_type)Cyc_Parse_err(({const char*_tmpA0="long modifier must come before base type";
_tag_dyneither(_tmpA0,sizeof(char),41);}),_tmp9A);if(seen_size){void*_tmpA1=sz;
_LL5F: if((int)_tmpA1 != 3)goto _LL61;_LL60: sz=(void*)4;goto _LL5E;_LL61:;_LL62: Cyc_Parse_err(({
const char*_tmpA2="extra long in type specifier";_tag_dyneither(_tmpA2,sizeof(
char),29);}),_tmp9A);goto _LL5E;_LL5E:;}else{sz=(void*)3;}last_loc=_tmp9A;
seen_size=1;goto _LL51;_LL5C: if(*((int*)_tmp94)!= 5)goto _LL51;_tmp9B=((struct Cyc_Parse_Decl_spec_struct*)
_tmp94)->f1;_LL5D: last_loc=_tmp9B->loc;if(declopt == 0  && !seen_type){seen_type=1;{
void*_tmpA3=_tmp9B->r;struct Cyc_Absyn_Aggrdecl*_tmpA4;struct Cyc_Absyn_Datatypedecl*
_tmpA5;struct Cyc_Absyn_Enumdecl*_tmpA6;_LL64: if(_tmpA3 <= (void*)2)goto _LL6A;if(*((
int*)_tmpA3)!= 6)goto _LL66;_tmpA4=((struct Cyc_Absyn_Aggr_d_struct*)_tmpA3)->f1;
_LL65: {struct Cyc_List_List*_tmpA7=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Parse_tvar2typ,((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Parse_copy_tvar,_tmpA4->tvs));t=(void*)({struct Cyc_Absyn_AggrType_struct*
_tmpA8=_cycalloc(sizeof(*_tmpA8));_tmpA8[0]=({struct Cyc_Absyn_AggrType_struct
_tmpA9;_tmpA9.tag=10;_tmpA9.f1=({struct Cyc_Absyn_AggrInfo _tmpAA;_tmpAA.aggr_info=
Cyc_Absyn_UnknownAggr(_tmpA4->kind,_tmpA4->name,0);_tmpAA.targs=_tmpA7;_tmpAA;});
_tmpA9;});_tmpA8;});if(_tmpA4->impl != 0)declopt=({struct Cyc_Core_Opt*_tmpAB=
_cycalloc(sizeof(*_tmpAB));_tmpAB->v=_tmp9B;_tmpAB;});goto _LL63;}_LL66: if(*((int*)
_tmpA3)!= 7)goto _LL68;_tmpA5=((struct Cyc_Absyn_Datatype_d_struct*)_tmpA3)->f1;
_LL67: {struct Cyc_List_List*_tmpAC=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Parse_tvar2typ,((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Parse_copy_tvar,_tmpA5->tvs));struct Cyc_Core_Opt*ropt=({struct Cyc_Core_Opt*
_tmpB2=_cycalloc(sizeof(*_tmpB2));_tmpB2->v=(void*)((void*)2);_tmpB2;});t=(void*)({
struct Cyc_Absyn_DatatypeType_struct*_tmpAD=_cycalloc(sizeof(*_tmpAD));_tmpAD[0]=({
struct Cyc_Absyn_DatatypeType_struct _tmpAE;_tmpAE.tag=2;_tmpAE.f1=({struct Cyc_Absyn_DatatypeInfo
_tmpAF;_tmpAF.datatype_info=Cyc_Absyn_KnownDatatype(({struct Cyc_Absyn_Datatypedecl**
_tmpB0=_cycalloc(sizeof(*_tmpB0));_tmpB0[0]=_tmpA5;_tmpB0;}));_tmpAF.targs=
_tmpAC;_tmpAF.rgn=ropt;_tmpAF;});_tmpAE;});_tmpAD;});if(_tmpA5->fields != 0)
declopt=({struct Cyc_Core_Opt*_tmpB1=_cycalloc(sizeof(*_tmpB1));_tmpB1->v=_tmp9B;
_tmpB1;});goto _LL63;}_LL68: if(*((int*)_tmpA3)!= 8)goto _LL6A;_tmpA6=((struct Cyc_Absyn_Enum_d_struct*)
_tmpA3)->f1;_LL69: t=(void*)({struct Cyc_Absyn_EnumType_struct*_tmpB3=_cycalloc(
sizeof(*_tmpB3));_tmpB3[0]=({struct Cyc_Absyn_EnumType_struct _tmpB4;_tmpB4.tag=12;
_tmpB4.f1=_tmpA6->name;_tmpB4.f2=0;_tmpB4;});_tmpB3;});declopt=({struct Cyc_Core_Opt*
_tmpB5=_cycalloc(sizeof(*_tmpB5));_tmpB5->v=_tmp9B;_tmpB5;});goto _LL63;_LL6A:;
_LL6B:({void*_tmpB6=0;((int(*)(struct Cyc_Position_Segment*sg,struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(_tmp9B->loc,({const
char*_tmpB7="bad declaration within type specifier";_tag_dyneither(_tmpB7,
sizeof(char),38);}),_tag_dyneither(_tmpB6,sizeof(void*),0));});_LL63:;}}else{Cyc_Parse_err(
Cyc_Parse_msg3,_tmp9B->loc);}goto _LL51;_LL51:;}if(!seen_type){if(!seen_sign  && !
seen_size)({void*_tmpB8=0;Cyc_Tcutil_warn(last_loc,({const char*_tmpB9="missing type within specifier";
_tag_dyneither(_tmpB9,sizeof(char),30);}),_tag_dyneither(_tmpB8,sizeof(void*),0));});
t=Cyc_Absyn_int_typ(sgn,sz);}else{if(seen_sign){void*_tmpBA=t;void*_tmpBB;void*
_tmpBC;_LL6D: if(_tmpBA <= (void*)4)goto _LL6F;if(*((int*)_tmpBA)!= 5)goto _LL6F;
_tmpBB=(void*)((struct Cyc_Absyn_IntType_struct*)_tmpBA)->f1;_tmpBC=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmpBA)->f2;_LL6E: if(_tmpBB != sgn)t=Cyc_Absyn_int_typ(
sgn,_tmpBC);goto _LL6C;_LL6F:;_LL70: Cyc_Parse_err(({const char*_tmpBD="sign specification on non-integral type";
_tag_dyneither(_tmpBD,sizeof(char),40);}),last_loc);goto _LL6C;_LL6C:;}if(
seen_size){void*_tmpBE=t;void*_tmpBF;void*_tmpC0;_LL72: if(_tmpBE <= (void*)4)goto
_LL76;if(*((int*)_tmpBE)!= 5)goto _LL74;_tmpBF=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmpBE)->f1;_tmpC0=(void*)((struct Cyc_Absyn_IntType_struct*)_tmpBE)->f2;_LL73:
if(_tmpC0 != sz)t=Cyc_Absyn_int_typ(_tmpBF,sz);goto _LL71;_LL74: if(*((int*)_tmpBE)
!= 6)goto _LL76;_LL75: t=Cyc_Absyn_double_typ(1);goto _LL71;_LL76:;_LL77: Cyc_Parse_err(({
const char*_tmpC1="size qualifier on non-integral type";_tag_dyneither(_tmpC1,
sizeof(char),36);}),last_loc);goto _LL71;_LL71:;}}return({struct _tuple11 _tmpC2;
_tmpC2.f1=t;_tmpC2.f2=declopt;_tmpC2;});}static struct Cyc_List_List*Cyc_Parse_apply_tmss(
struct _RegionHandle*r,struct Cyc_Absyn_Tqual tq,void*t,struct Cyc_List_List*ds,
struct Cyc_List_List*shared_atts){if(ds == 0)return 0;{struct Cyc_Parse_Declarator*
_tmpC3=(struct Cyc_Parse_Declarator*)ds->hd;struct _tuple1*_tmpC4=_tmpC3->id;
struct Cyc_Absyn_Tqual _tmpC6;void*_tmpC7;struct Cyc_List_List*_tmpC8;struct Cyc_List_List*
_tmpC9;struct _tuple12 _tmpC5=Cyc_Parse_apply_tms(tq,t,shared_atts,_tmpC3->tms);
_tmpC6=_tmpC5.f1;_tmpC7=_tmpC5.f2;_tmpC8=_tmpC5.f3;_tmpC9=_tmpC5.f4;if(ds->tl == 
0)return({struct Cyc_List_List*_tmpCA=_region_malloc(r,sizeof(*_tmpCA));_tmpCA->hd=({
struct _tuple14*_tmpCB=_region_malloc(r,sizeof(*_tmpCB));_tmpCB->f1=_tmpC4;_tmpCB->f2=
_tmpC6;_tmpCB->f3=_tmpC7;_tmpCB->f4=_tmpC8;_tmpCB->f5=_tmpC9;_tmpCB;});_tmpCA->tl=
0;_tmpCA;});else{return({struct Cyc_List_List*_tmpCC=_region_malloc(r,sizeof(*
_tmpCC));_tmpCC->hd=({struct _tuple14*_tmpCD=_region_malloc(r,sizeof(*_tmpCD));
_tmpCD->f1=_tmpC4;_tmpCD->f2=_tmpC6;_tmpCD->f3=_tmpC7;_tmpCD->f4=_tmpC8;_tmpCD->f5=
_tmpC9;_tmpCD;});_tmpCC->tl=Cyc_Parse_apply_tmss(r,_tmpC6,Cyc_Tcutil_copy_type(t),
ds->tl,shared_atts);_tmpCC;});}}}static struct _tuple12 Cyc_Parse_apply_tms(struct
Cyc_Absyn_Tqual tq,void*t,struct Cyc_List_List*atts,struct Cyc_List_List*tms){if(
tms == 0)return({struct _tuple12 _tmpCE;_tmpCE.f1=tq;_tmpCE.f2=t;_tmpCE.f3=0;_tmpCE.f4=
atts;_tmpCE;});{void*_tmpCF=(void*)tms->hd;union Cyc_Absyn_Constraint*_tmpD0;
struct Cyc_Position_Segment*_tmpD1;struct Cyc_Absyn_Exp*_tmpD2;union Cyc_Absyn_Constraint*
_tmpD3;struct Cyc_Position_Segment*_tmpD4;void*_tmpD5;struct Cyc_List_List*_tmpD6;
struct Cyc_Position_Segment*_tmpD7;struct Cyc_Absyn_PtrAtts _tmpD8;struct Cyc_Absyn_Tqual
_tmpD9;struct Cyc_Position_Segment*_tmpDA;struct Cyc_List_List*_tmpDB;_LL79: if(*((
int*)_tmpCF)!= 0)goto _LL7B;_tmpD0=((struct Cyc_Absyn_Carray_mod_struct*)_tmpCF)->f1;
_tmpD1=((struct Cyc_Absyn_Carray_mod_struct*)_tmpCF)->f2;_LL7A: return Cyc_Parse_apply_tms(
Cyc_Absyn_empty_tqual(0),Cyc_Absyn_array_typ(t,tq,0,_tmpD0,_tmpD1),atts,tms->tl);
_LL7B: if(*((int*)_tmpCF)!= 1)goto _LL7D;_tmpD2=((struct Cyc_Absyn_ConstArray_mod_struct*)
_tmpCF)->f1;_tmpD3=((struct Cyc_Absyn_ConstArray_mod_struct*)_tmpCF)->f2;_tmpD4=((
struct Cyc_Absyn_ConstArray_mod_struct*)_tmpCF)->f3;_LL7C: return Cyc_Parse_apply_tms(
Cyc_Absyn_empty_tqual(0),Cyc_Absyn_array_typ(t,tq,(struct Cyc_Absyn_Exp*)_tmpD2,
_tmpD3,_tmpD4),atts,tms->tl);_LL7D: if(*((int*)_tmpCF)!= 3)goto _LL7F;_tmpD5=(void*)((
struct Cyc_Absyn_Function_mod_struct*)_tmpCF)->f1;_LL7E: {void*_tmpDC=_tmpD5;
struct Cyc_List_List*_tmpDD;int _tmpDE;struct Cyc_Absyn_VarargInfo*_tmpDF;struct Cyc_Core_Opt*
_tmpE0;struct Cyc_List_List*_tmpE1;struct Cyc_Position_Segment*_tmpE2;_LL86: if(*((
int*)_tmpDC)!= 1)goto _LL88;_tmpDD=((struct Cyc_Absyn_WithTypes_struct*)_tmpDC)->f1;
_tmpDE=((struct Cyc_Absyn_WithTypes_struct*)_tmpDC)->f2;_tmpDF=((struct Cyc_Absyn_WithTypes_struct*)
_tmpDC)->f3;_tmpE0=((struct Cyc_Absyn_WithTypes_struct*)_tmpDC)->f4;_tmpE1=((
struct Cyc_Absyn_WithTypes_struct*)_tmpDC)->f5;_LL87: {struct Cyc_List_List*
typvars=0;struct Cyc_List_List*fn_atts=0;struct Cyc_List_List*new_atts=0;{struct
Cyc_List_List*as=atts;for(0;as != 0;as=as->tl){if(Cyc_Absyn_fntype_att((void*)as->hd))
fn_atts=({struct Cyc_List_List*_tmpE3=_cycalloc(sizeof(*_tmpE3));_tmpE3->hd=(void*)((
void*)as->hd);_tmpE3->tl=fn_atts;_tmpE3;});else{new_atts=({struct Cyc_List_List*
_tmpE4=_cycalloc(sizeof(*_tmpE4));_tmpE4->hd=(void*)((void*)as->hd);_tmpE4->tl=
new_atts;_tmpE4;});}}}if(tms->tl != 0){void*_tmpE5=(void*)((struct Cyc_List_List*)
_check_null(tms->tl))->hd;struct Cyc_List_List*_tmpE6;_LL8B: if(*((int*)_tmpE5)!= 
4)goto _LL8D;_tmpE6=((struct Cyc_Absyn_TypeParams_mod_struct*)_tmpE5)->f1;_LL8C:
typvars=_tmpE6;tms=tms->tl;goto _LL8A;_LL8D:;_LL8E: goto _LL8A;_LL8A:;}if(((((!
_tmpDE  && _tmpDF == 0) && _tmpDD != 0) && _tmpDD->tl == 0) && (*((struct _tuple8*)
_tmpDD->hd)).f1 == 0) && (*((struct _tuple8*)_tmpDD->hd)).f3 == (void*)0)_tmpDD=0;t=
Cyc_Parse_array2ptr(t,0);((void(*)(void(*f)(struct _tuple8*),struct Cyc_List_List*
x))Cyc_List_iter)(Cyc_Parse_arg_array2ptr,_tmpDD);return Cyc_Parse_apply_tms(Cyc_Absyn_empty_tqual(
tq.loc),Cyc_Absyn_function_typ(typvars,_tmpE0,t,_tmpDD,_tmpDE,_tmpDF,_tmpE1,
fn_atts),new_atts,((struct Cyc_List_List*)_check_null(tms))->tl);}_LL88: if(*((int*)
_tmpDC)!= 0)goto _LL85;_tmpE2=((struct Cyc_Absyn_NoTypes_struct*)_tmpDC)->f2;_LL89:({
void*_tmpE7=0;((int(*)(struct Cyc_Position_Segment*sg,struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Parse_abort)(_tmpE2,({const char*_tmpE8="function declaration without parameter types";
_tag_dyneither(_tmpE8,sizeof(char),45);}),_tag_dyneither(_tmpE7,sizeof(void*),0));});
_LL85:;}_LL7F: if(*((int*)_tmpCF)!= 4)goto _LL81;_tmpD6=((struct Cyc_Absyn_TypeParams_mod_struct*)
_tmpCF)->f1;_tmpD7=((struct Cyc_Absyn_TypeParams_mod_struct*)_tmpCF)->f2;_LL80:
if(tms->tl == 0)return({struct _tuple12 _tmpE9;_tmpE9.f1=tq;_tmpE9.f2=t;_tmpE9.f3=
_tmpD6;_tmpE9.f4=atts;_tmpE9;});({void*_tmpEA=0;((int(*)(struct Cyc_Position_Segment*
sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(_tmpD7,({
const char*_tmpEB="type parameters must appear before function arguments in declarator";
_tag_dyneither(_tmpEB,sizeof(char),68);}),_tag_dyneither(_tmpEA,sizeof(void*),0));});
_LL81: if(*((int*)_tmpCF)!= 2)goto _LL83;_tmpD8=((struct Cyc_Absyn_Pointer_mod_struct*)
_tmpCF)->f1;_tmpD9=((struct Cyc_Absyn_Pointer_mod_struct*)_tmpCF)->f2;_LL82:
return Cyc_Parse_apply_tms(_tmpD9,(void*)({struct Cyc_Absyn_PointerType_struct*
_tmpEC=_cycalloc(sizeof(*_tmpEC));_tmpEC[0]=({struct Cyc_Absyn_PointerType_struct
_tmpED;_tmpED.tag=4;_tmpED.f1=({struct Cyc_Absyn_PtrInfo _tmpEE;_tmpEE.elt_typ=t;
_tmpEE.elt_tq=tq;_tmpEE.ptr_atts=_tmpD8;_tmpEE;});_tmpED;});_tmpEC;}),atts,tms->tl);
_LL83: if(*((int*)_tmpCF)!= 5)goto _LL78;_tmpDA=((struct Cyc_Absyn_Attributes_mod_struct*)
_tmpCF)->f1;_tmpDB=((struct Cyc_Absyn_Attributes_mod_struct*)_tmpCF)->f2;_LL84:
return Cyc_Parse_apply_tms(tq,t,Cyc_List_append(atts,_tmpDB),tms->tl);_LL78:;}}
void*Cyc_Parse_speclist2typ(struct Cyc_List_List*tss,struct Cyc_Position_Segment*
loc){void*_tmpF0;struct Cyc_Core_Opt*_tmpF1;struct _tuple11 _tmpEF=Cyc_Parse_collapse_type_specifiers(
tss,loc);_tmpF0=_tmpEF.f1;_tmpF1=_tmpEF.f2;if(_tmpF1 != 0)({void*_tmpF2=0;Cyc_Tcutil_warn(
loc,({const char*_tmpF3="ignoring nested type declaration(s) in specifier list";
_tag_dyneither(_tmpF3,sizeof(char),54);}),_tag_dyneither(_tmpF2,sizeof(void*),0));});
return _tmpF0;}static struct Cyc_Absyn_Decl*Cyc_Parse_v_typ_to_typedef(struct Cyc_Position_Segment*
loc,struct _tuple14*t){struct _tuple14 _tmpF5;struct _tuple1*_tmpF6;struct Cyc_Absyn_Tqual
_tmpF7;void*_tmpF8;struct Cyc_List_List*_tmpF9;struct Cyc_List_List*_tmpFA;struct
_tuple14*_tmpF4=t;_tmpF5=*_tmpF4;_tmpF6=_tmpF5.f1;_tmpF7=_tmpF5.f2;_tmpF8=_tmpF5.f3;
_tmpF9=_tmpF5.f4;_tmpFA=_tmpF5.f5;Cyc_Lex_register_typedef(_tmpF6);{struct Cyc_Core_Opt*
kind;struct Cyc_Core_Opt*type;{void*_tmpFB=_tmpF8;struct Cyc_Core_Opt*_tmpFC;_LL90:
if(_tmpFB <= (void*)4)goto _LL92;if(*((int*)_tmpFB)!= 0)goto _LL92;_tmpFC=((struct
Cyc_Absyn_Evar_struct*)_tmpFB)->f1;_LL91: type=0;if(_tmpFC == 0)kind=(struct Cyc_Core_Opt*)&
Cyc_Tcutil_bk;else{kind=_tmpFC;}goto _LL8F;_LL92:;_LL93: kind=0;type=({struct Cyc_Core_Opt*
_tmpFD=_cycalloc(sizeof(*_tmpFD));_tmpFD->v=(void*)_tmpF8;_tmpFD;});goto _LL8F;
_LL8F:;}return Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Typedef_d_struct*
_tmpFE=_cycalloc(sizeof(*_tmpFE));_tmpFE[0]=({struct Cyc_Absyn_Typedef_d_struct
_tmpFF;_tmpFF.tag=9;_tmpFF.f1=({struct Cyc_Absyn_Typedefdecl*_tmp100=_cycalloc(
sizeof(*_tmp100));_tmp100->name=_tmpF6;_tmp100->tvs=_tmpF9;_tmp100->kind=kind;
_tmp100->defn=type;_tmp100->atts=_tmpFA;_tmp100->tq=_tmpF7;_tmp100;});_tmpFF;});
_tmpFE;}),loc);}}static struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_decl(struct Cyc_Absyn_Decl*
d,struct Cyc_Absyn_Stmt*s){return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Decl_s_struct*
_tmp101=_cycalloc(sizeof(*_tmp101));_tmp101[0]=({struct Cyc_Absyn_Decl_s_struct
_tmp102;_tmp102.tag=11;_tmp102.f1=d;_tmp102.f2=s;_tmp102;});_tmp101;}),d->loc);}
static struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_declarations(struct Cyc_List_List*ds,
struct Cyc_Absyn_Stmt*s){return((struct Cyc_Absyn_Stmt*(*)(struct Cyc_Absyn_Stmt*(*
f)(struct Cyc_Absyn_Decl*,struct Cyc_Absyn_Stmt*),struct Cyc_List_List*x,struct Cyc_Absyn_Stmt*
accum))Cyc_List_fold_right)(Cyc_Parse_flatten_decl,ds,s);}static struct Cyc_List_List*
Cyc_Parse_make_declarations(struct Cyc_Parse_Declaration_spec*ds,struct Cyc_List_List*
ids,struct Cyc_Position_Segment*tqual_loc,struct Cyc_Position_Segment*loc){struct
_RegionHandle _tmp103=_new_region("mkrgn");struct _RegionHandle*mkrgn=& _tmp103;
_push_region(mkrgn);{struct Cyc_Parse_Declaration_spec _tmp105;struct Cyc_Absyn_Tqual
_tmp106;struct Cyc_List_List*_tmp107;struct Cyc_List_List*_tmp108;struct Cyc_Parse_Declaration_spec*
_tmp104=ds;_tmp105=*_tmp104;_tmp106=_tmp105.tq;_tmp107=_tmp105.type_specs;
_tmp108=_tmp105.attributes;if(_tmp106.loc == 0)_tmp106.loc=tqual_loc;if(ds->is_inline)
Cyc_Parse_err(({const char*_tmp109="inline is allowed only on function definitions";
_tag_dyneither(_tmp109,sizeof(char),47);}),loc);if(_tmp107 == 0){Cyc_Parse_err(({
const char*_tmp10A="missing type specifiers in declaration";_tag_dyneither(
_tmp10A,sizeof(char),39);}),loc);{struct Cyc_List_List*_tmp10B=0;_npop_handler(0);
return _tmp10B;}}{void*s=(void*)2;int istypedef=0;if(ds->sc != 0){void*_tmp10C=(
void*)((struct Cyc_Core_Opt*)_check_null(ds->sc))->v;_LL95: if((int)_tmp10C != 0)
goto _LL97;_LL96: istypedef=1;goto _LL94;_LL97: if((int)_tmp10C != 1)goto _LL99;_LL98:
s=(void*)3;goto _LL94;_LL99: if((int)_tmp10C != 2)goto _LL9B;_LL9A: s=(void*)4;goto
_LL94;_LL9B: if((int)_tmp10C != 3)goto _LL9D;_LL9C: s=(void*)0;goto _LL94;_LL9D: if((
int)_tmp10C != 4)goto _LL9F;_LL9E: s=(void*)2;goto _LL94;_LL9F: if((int)_tmp10C != 5)
goto _LLA1;_LLA0: if(Cyc_Parse_no_register)s=(void*)2;else{s=(void*)5;}goto _LL94;
_LLA1: if((int)_tmp10C != 6)goto _LL94;_LLA2: s=(void*)1;goto _LL94;_LL94:;}{struct
Cyc_List_List*_tmp10E;struct Cyc_List_List*_tmp10F;struct _tuple0 _tmp10D=((struct
_tuple0(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x))
Cyc_List_rsplit)(mkrgn,mkrgn,ids);_tmp10E=_tmp10D.f1;_tmp10F=_tmp10D.f2;{int
exps_empty=1;{struct Cyc_List_List*es=_tmp10F;for(0;es != 0;es=es->tl){if((struct
Cyc_Absyn_Exp*)es->hd != 0){exps_empty=0;break;}}}{struct _tuple11 _tmp110=Cyc_Parse_collapse_type_specifiers(
_tmp107,loc);if(_tmp10E == 0){void*_tmp112;struct Cyc_Core_Opt*_tmp113;struct
_tuple11 _tmp111=_tmp110;_tmp112=_tmp111.f1;_tmp113=_tmp111.f2;if(_tmp113 != 0){
struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)_tmp113->v;{void*_tmp114=d->r;
struct Cyc_Absyn_Enumdecl*_tmp115;struct Cyc_Absyn_Aggrdecl*_tmp116;struct Cyc_Absyn_Datatypedecl*
_tmp117;_LLA4: if(_tmp114 <= (void*)2)goto _LLAA;if(*((int*)_tmp114)!= 8)goto _LLA6;
_tmp115=((struct Cyc_Absyn_Enum_d_struct*)_tmp114)->f1;_LLA5: _tmp115->sc=s;if(
_tmp108 != 0)Cyc_Parse_err(({const char*_tmp118="bad attributes on enum";
_tag_dyneither(_tmp118,sizeof(char),23);}),loc);goto _LLA3;_LLA6: if(*((int*)
_tmp114)!= 6)goto _LLA8;_tmp116=((struct Cyc_Absyn_Aggr_d_struct*)_tmp114)->f1;
_LLA7: _tmp116->sc=s;_tmp116->attributes=_tmp108;goto _LLA3;_LLA8: if(*((int*)
_tmp114)!= 7)goto _LLAA;_tmp117=((struct Cyc_Absyn_Datatype_d_struct*)_tmp114)->f1;
_LLA9: _tmp117->sc=s;if(_tmp108 != 0)Cyc_Parse_err(({const char*_tmp119="bad attributes on datatype";
_tag_dyneither(_tmp119,sizeof(char),27);}),loc);goto _LLA3;_LLAA:;_LLAB: Cyc_Parse_err(({
const char*_tmp11A="bad declaration";_tag_dyneither(_tmp11A,sizeof(char),16);}),
loc);{struct Cyc_List_List*_tmp11B=0;_npop_handler(0);return _tmp11B;}_LLA3:;}{
struct Cyc_List_List*_tmp11D=({struct Cyc_List_List*_tmp11C=_cycalloc(sizeof(*
_tmp11C));_tmp11C->hd=d;_tmp11C->tl=0;_tmp11C;});_npop_handler(0);return _tmp11D;}}
else{void*_tmp11E=_tmp112;struct Cyc_Absyn_AggrInfo _tmp11F;union Cyc_Absyn_AggrInfoU
_tmp120;struct _tuple3 _tmp121;void*_tmp122;struct _tuple1*_tmp123;struct Cyc_List_List*
_tmp124;struct Cyc_Absyn_DatatypeInfo _tmp125;union Cyc_Absyn_DatatypeInfoU _tmp126;
struct Cyc_Absyn_Datatypedecl**_tmp127;struct Cyc_Absyn_DatatypeInfo _tmp128;union
Cyc_Absyn_DatatypeInfoU _tmp129;struct Cyc_Absyn_UnknownDatatypeInfo _tmp12A;struct
_tuple1*_tmp12B;int _tmp12C;struct Cyc_List_List*_tmp12D;struct _tuple1*_tmp12E;
struct Cyc_List_List*_tmp12F;_LLAD: if(_tmp11E <= (void*)4)goto _LLB7;if(*((int*)
_tmp11E)!= 10)goto _LLAF;_tmp11F=((struct Cyc_Absyn_AggrType_struct*)_tmp11E)->f1;
_tmp120=_tmp11F.aggr_info;if((_tmp120.UnknownAggr).tag != 1)goto _LLAF;_tmp121=(
struct _tuple3)(_tmp120.UnknownAggr).val;_tmp122=_tmp121.f1;_tmp123=_tmp121.f2;
_tmp124=_tmp11F.targs;_LLAE: {struct Cyc_List_List*_tmp130=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,loc,_tmp124);
struct Cyc_Absyn_Aggrdecl*_tmp131=({struct Cyc_Absyn_Aggrdecl*_tmp137=_cycalloc(
sizeof(*_tmp137));_tmp137->kind=_tmp122;_tmp137->sc=s;_tmp137->name=_tmp123;
_tmp137->tvs=_tmp130;_tmp137->impl=0;_tmp137->attributes=0;_tmp137;});if(_tmp108
!= 0)Cyc_Parse_err(({const char*_tmp132="bad attributes on type declaration";
_tag_dyneither(_tmp132,sizeof(char),35);}),loc);{struct Cyc_List_List*_tmp136=({
struct Cyc_List_List*_tmp133=_cycalloc(sizeof(*_tmp133));_tmp133->hd=Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Aggr_d_struct*_tmp134=_cycalloc(sizeof(*_tmp134));
_tmp134[0]=({struct Cyc_Absyn_Aggr_d_struct _tmp135;_tmp135.tag=6;_tmp135.f1=
_tmp131;_tmp135;});_tmp134;}),loc);_tmp133->tl=0;_tmp133;});_npop_handler(0);
return _tmp136;}}_LLAF: if(*((int*)_tmp11E)!= 2)goto _LLB1;_tmp125=((struct Cyc_Absyn_DatatypeType_struct*)
_tmp11E)->f1;_tmp126=_tmp125.datatype_info;if((_tmp126.KnownDatatype).tag != 2)
goto _LLB1;_tmp127=(struct Cyc_Absyn_Datatypedecl**)(_tmp126.KnownDatatype).val;
_LLB0: if(_tmp108 != 0)Cyc_Parse_err(({const char*_tmp138="bad attributes on datatype";
_tag_dyneither(_tmp138,sizeof(char),27);}),loc);{struct Cyc_List_List*_tmp13C=({
struct Cyc_List_List*_tmp139=_cycalloc(sizeof(*_tmp139));_tmp139->hd=Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Datatype_d_struct*_tmp13A=_cycalloc(sizeof(*_tmp13A));
_tmp13A[0]=({struct Cyc_Absyn_Datatype_d_struct _tmp13B;_tmp13B.tag=7;_tmp13B.f1=*
_tmp127;_tmp13B;});_tmp13A;}),loc);_tmp139->tl=0;_tmp139;});_npop_handler(0);
return _tmp13C;}_LLB1: if(*((int*)_tmp11E)!= 2)goto _LLB3;_tmp128=((struct Cyc_Absyn_DatatypeType_struct*)
_tmp11E)->f1;_tmp129=_tmp128.datatype_info;if((_tmp129.UnknownDatatype).tag != 1)
goto _LLB3;_tmp12A=(struct Cyc_Absyn_UnknownDatatypeInfo)(_tmp129.UnknownDatatype).val;
_tmp12B=_tmp12A.name;_tmp12C=_tmp12A.is_extensible;_tmp12D=_tmp128.targs;_LLB2: {
struct Cyc_List_List*_tmp13D=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(
struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*
x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,loc,_tmp12D);struct Cyc_Absyn_Decl*_tmp13E=
Cyc_Absyn_datatype_decl(s,_tmp12B,_tmp13D,0,_tmp12C,loc);if(_tmp108 != 0)Cyc_Parse_err(({
const char*_tmp13F="bad attributes on datatype";_tag_dyneither(_tmp13F,sizeof(
char),27);}),loc);{struct Cyc_List_List*_tmp141=({struct Cyc_List_List*_tmp140=
_cycalloc(sizeof(*_tmp140));_tmp140->hd=_tmp13E;_tmp140->tl=0;_tmp140;});
_npop_handler(0);return _tmp141;}}_LLB3: if(*((int*)_tmp11E)!= 12)goto _LLB5;
_tmp12E=((struct Cyc_Absyn_EnumType_struct*)_tmp11E)->f1;_LLB4: {struct Cyc_Absyn_Enumdecl*
_tmp142=({struct Cyc_Absyn_Enumdecl*_tmp149=_cycalloc(sizeof(*_tmp149));_tmp149->sc=
s;_tmp149->name=_tmp12E;_tmp149->fields=0;_tmp149;});if(_tmp108 != 0)Cyc_Parse_err(({
const char*_tmp143="bad attributes on enum";_tag_dyneither(_tmp143,sizeof(char),
23);}),loc);{struct Cyc_List_List*_tmp148=({struct Cyc_List_List*_tmp144=_cycalloc(
sizeof(*_tmp144));_tmp144->hd=({struct Cyc_Absyn_Decl*_tmp145=_cycalloc(sizeof(*
_tmp145));_tmp145->r=(void*)({struct Cyc_Absyn_Enum_d_struct*_tmp146=_cycalloc(
sizeof(*_tmp146));_tmp146[0]=({struct Cyc_Absyn_Enum_d_struct _tmp147;_tmp147.tag=
8;_tmp147.f1=_tmp142;_tmp147;});_tmp146;});_tmp145->loc=loc;_tmp145;});_tmp144->tl=
0;_tmp144;});_npop_handler(0);return _tmp148;}}_LLB5: if(*((int*)_tmp11E)!= 13)
goto _LLB7;_tmp12F=((struct Cyc_Absyn_AnonEnumType_struct*)_tmp11E)->f1;_LLB6: {
struct Cyc_Absyn_Enumdecl*_tmp14A=({struct Cyc_Absyn_Enumdecl*_tmp151=_cycalloc(
sizeof(*_tmp151));_tmp151->sc=s;_tmp151->name=Cyc_Parse_gensym_enum();_tmp151->fields=({
struct Cyc_Core_Opt*_tmp152=_cycalloc(sizeof(*_tmp152));_tmp152->v=_tmp12F;
_tmp152;});_tmp151;});if(_tmp108 != 0)Cyc_Parse_err(({const char*_tmp14B="bad attributes on enum";
_tag_dyneither(_tmp14B,sizeof(char),23);}),loc);{struct Cyc_List_List*_tmp150=({
struct Cyc_List_List*_tmp14C=_cycalloc(sizeof(*_tmp14C));_tmp14C->hd=({struct Cyc_Absyn_Decl*
_tmp14D=_cycalloc(sizeof(*_tmp14D));_tmp14D->r=(void*)({struct Cyc_Absyn_Enum_d_struct*
_tmp14E=_cycalloc(sizeof(*_tmp14E));_tmp14E[0]=({struct Cyc_Absyn_Enum_d_struct
_tmp14F;_tmp14F.tag=8;_tmp14F.f1=_tmp14A;_tmp14F;});_tmp14E;});_tmp14D->loc=loc;
_tmp14D;});_tmp14C->tl=0;_tmp14C;});_npop_handler(0);return _tmp150;}}_LLB7:;
_LLB8: Cyc_Parse_err(({const char*_tmp153="missing declarator";_tag_dyneither(
_tmp153,sizeof(char),19);}),loc);{struct Cyc_List_List*_tmp154=0;_npop_handler(0);
return _tmp154;}_LLAC:;}}else{void*t=_tmp110.f1;struct Cyc_List_List*_tmp155=Cyc_Parse_apply_tmss(
mkrgn,_tmp106,t,_tmp10E,_tmp108);if(istypedef){if(!exps_empty)Cyc_Parse_err(({
const char*_tmp156="initializer in typedef declaration";_tag_dyneither(_tmp156,
sizeof(char),35);}),loc);{struct Cyc_List_List*decls=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Decl*(*f)(struct Cyc_Position_Segment*,struct _tuple14*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_v_typ_to_typedef,loc,
_tmp155);if(_tmp110.f2 != 0){struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)(
_tmp110.f2)->v;{void*_tmp157=d->r;struct Cyc_Absyn_Aggrdecl*_tmp158;struct Cyc_Absyn_Datatypedecl*
_tmp159;struct Cyc_Absyn_Enumdecl*_tmp15A;_LLBA: if(_tmp157 <= (void*)2)goto _LLC0;
if(*((int*)_tmp157)!= 6)goto _LLBC;_tmp158=((struct Cyc_Absyn_Aggr_d_struct*)
_tmp157)->f1;_LLBB: _tmp158->sc=s;_tmp158->attributes=_tmp108;_tmp108=0;goto _LLB9;
_LLBC: if(*((int*)_tmp157)!= 7)goto _LLBE;_tmp159=((struct Cyc_Absyn_Datatype_d_struct*)
_tmp157)->f1;_LLBD: _tmp159->sc=s;goto _LLB9;_LLBE: if(*((int*)_tmp157)!= 8)goto
_LLC0;_tmp15A=((struct Cyc_Absyn_Enum_d_struct*)_tmp157)->f1;_LLBF: _tmp15A->sc=s;
goto _LLB9;_LLC0:;_LLC1: Cyc_Parse_err(({const char*_tmp15B="declaration within typedef is not a struct, union, or datatype";
_tag_dyneither(_tmp15B,sizeof(char),63);}),loc);goto _LLB9;_LLB9:;}decls=({struct
Cyc_List_List*_tmp15C=_cycalloc(sizeof(*_tmp15C));_tmp15C->hd=d;_tmp15C->tl=
decls;_tmp15C;});}{struct Cyc_List_List*_tmp15D=decls;_npop_handler(0);return
_tmp15D;}}}else{if(_tmp110.f2 != 0)Cyc_Parse_unimp(({const char*_tmp15E="nested type declaration within declarator";
_tag_dyneither(_tmp15E,sizeof(char),42);}),loc);{struct Cyc_List_List*decls=0;{
struct Cyc_List_List*_tmp15F=_tmp155;for(0;_tmp15F != 0;(_tmp15F=_tmp15F->tl,
_tmp10F=_tmp10F->tl)){struct _tuple14 _tmp161;struct _tuple1*_tmp162;struct Cyc_Absyn_Tqual
_tmp163;void*_tmp164;struct Cyc_List_List*_tmp165;struct Cyc_List_List*_tmp166;
struct _tuple14*_tmp160=(struct _tuple14*)_tmp15F->hd;_tmp161=*_tmp160;_tmp162=
_tmp161.f1;_tmp163=_tmp161.f2;_tmp164=_tmp161.f3;_tmp165=_tmp161.f4;_tmp166=
_tmp161.f5;if(_tmp165 != 0)({void*_tmp167=0;Cyc_Tcutil_warn(loc,({const char*
_tmp168="bad type params, ignoring";_tag_dyneither(_tmp168,sizeof(char),26);}),
_tag_dyneither(_tmp167,sizeof(void*),0));});if(_tmp10F == 0)({void*_tmp169=0;((
int(*)(struct Cyc_Position_Segment*sg,struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Parse_abort)(loc,({const char*_tmp16A="unexpected NULL in parse!";
_tag_dyneither(_tmp16A,sizeof(char),26);}),_tag_dyneither(_tmp169,sizeof(void*),
0));});{struct Cyc_Absyn_Exp*_tmp16B=(struct Cyc_Absyn_Exp*)_tmp10F->hd;struct Cyc_Absyn_Vardecl*
_tmp16C=Cyc_Absyn_new_vardecl(_tmp162,_tmp164,_tmp16B);_tmp16C->tq=_tmp163;
_tmp16C->sc=s;_tmp16C->attributes=_tmp166;{struct Cyc_Absyn_Decl*_tmp16D=({struct
Cyc_Absyn_Decl*_tmp16F=_cycalloc(sizeof(*_tmp16F));_tmp16F->r=(void*)({struct Cyc_Absyn_Var_d_struct*
_tmp170=_cycalloc(sizeof(*_tmp170));_tmp170[0]=({struct Cyc_Absyn_Var_d_struct
_tmp171;_tmp171.tag=0;_tmp171.f1=_tmp16C;_tmp171;});_tmp170;});_tmp16F->loc=loc;
_tmp16F;});decls=({struct Cyc_List_List*_tmp16E=_cycalloc(sizeof(*_tmp16E));
_tmp16E->hd=_tmp16D;_tmp16E->tl=decls;_tmp16E;});}}}}{struct Cyc_List_List*
_tmp172=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(decls);
_npop_handler(0);return _tmp172;}}}}}}}}};_pop_region(mkrgn);}static void*Cyc_Parse_id_to_kind(
struct _dyneither_ptr s,struct Cyc_Position_Segment*loc){if(Cyc_strlen((struct
_dyneither_ptr)s)== 1  || Cyc_strlen((struct _dyneither_ptr)s)== 2)switch(*((const
char*)_check_dyneither_subscript(s,sizeof(char),0))){case 'A': _LLC2: return(void*)
0;case 'M': _LLC3: return(void*)1;case 'B': _LLC4: return(void*)2;case 'R': _LLC5: return(
void*)3;case 'U': _LLC6: if(*((const char*)_check_dyneither_subscript(s,sizeof(char),
1))== 'R')return(void*)4;else{break;}case 'T': _LLC7: if(*((const char*)
_check_dyneither_subscript(s,sizeof(char),1))== 'R')return(void*)5;else{break;}
case 'E': _LLC8: return(void*)6;case 'I': _LLC9: return(void*)7;default: _LLCA: break;}
Cyc_Parse_err((struct _dyneither_ptr)({struct Cyc_Int_pa_struct _tmp176;_tmp176.tag=
1;_tmp176.f1=(unsigned long)((int)Cyc_strlen((struct _dyneither_ptr)s));{struct
Cyc_String_pa_struct _tmp175;_tmp175.tag=0;_tmp175.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)s);{void*_tmp173[2]={& _tmp175,& _tmp176};Cyc_aprintf(({const
char*_tmp174="bad kind: %s; strlen=%d";_tag_dyneither(_tmp174,sizeof(char),24);}),
_tag_dyneither(_tmp173,sizeof(void*),2));}}}),loc);return(void*)2;}static struct
Cyc_Absyn_Exp*Cyc_Parse_pat2exp(struct Cyc_Absyn_Pat*p){void*_tmp177=p->r;struct
_tuple1*_tmp178;struct Cyc_Absyn_Vardecl*_tmp179;struct Cyc_Absyn_Pat*_tmp17A;
struct Cyc_Absyn_Pat _tmp17B;void*_tmp17C;struct Cyc_Absyn_Pat*_tmp17D;void*_tmp17E;
int _tmp17F;char _tmp180;struct _dyneither_ptr _tmp181;struct _tuple1*_tmp182;struct
Cyc_List_List*_tmp183;int _tmp184;struct Cyc_Absyn_Exp*_tmp185;_LLCD: if(_tmp177 <= (
void*)2)goto _LLD3;if(*((int*)_tmp177)!= 12)goto _LLCF;_tmp178=((struct Cyc_Absyn_UnknownId_p_struct*)
_tmp177)->f1;_LLCE: return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_UnknownId_e_struct*
_tmp186=_cycalloc(sizeof(*_tmp186));_tmp186[0]=({struct Cyc_Absyn_UnknownId_e_struct
_tmp187;_tmp187.tag=2;_tmp187.f1=_tmp178;_tmp187;});_tmp186;}),p->loc);_LLCF: if(*((
int*)_tmp177)!= 1)goto _LLD1;_tmp179=((struct Cyc_Absyn_Reference_p_struct*)
_tmp177)->f1;_tmp17A=((struct Cyc_Absyn_Reference_p_struct*)_tmp177)->f2;_tmp17B=*
_tmp17A;_tmp17C=_tmp17B.r;if((int)_tmp17C != 0)goto _LLD1;_LLD0: return Cyc_Absyn_deref_exp(
Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_UnknownId_e_struct*_tmp188=_cycalloc(
sizeof(*_tmp188));_tmp188[0]=({struct Cyc_Absyn_UnknownId_e_struct _tmp189;_tmp189.tag=
2;_tmp189.f1=_tmp179->name;_tmp189;});_tmp188;}),p->loc),p->loc);_LLD1: if(*((int*)
_tmp177)!= 4)goto _LLD3;_tmp17D=((struct Cyc_Absyn_Pointer_p_struct*)_tmp177)->f1;
_LLD2: return Cyc_Absyn_address_exp(Cyc_Parse_pat2exp(_tmp17D),p->loc);_LLD3: if((
int)_tmp177 != 1)goto _LLD5;_LLD4: return Cyc_Absyn_null_exp(p->loc);_LLD5: if(
_tmp177 <= (void*)2)goto _LLDF;if(*((int*)_tmp177)!= 7)goto _LLD7;_tmp17E=(void*)((
struct Cyc_Absyn_Int_p_struct*)_tmp177)->f1;_tmp17F=((struct Cyc_Absyn_Int_p_struct*)
_tmp177)->f2;_LLD6: return Cyc_Absyn_int_exp(_tmp17E,_tmp17F,p->loc);_LLD7: if(*((
int*)_tmp177)!= 8)goto _LLD9;_tmp180=((struct Cyc_Absyn_Char_p_struct*)_tmp177)->f1;
_LLD8: return Cyc_Absyn_char_exp(_tmp180,p->loc);_LLD9: if(*((int*)_tmp177)!= 9)
goto _LLDB;_tmp181=((struct Cyc_Absyn_Float_p_struct*)_tmp177)->f1;_LLDA: return Cyc_Absyn_float_exp(
_tmp181,p->loc);_LLDB: if(*((int*)_tmp177)!= 13)goto _LLDD;_tmp182=((struct Cyc_Absyn_UnknownCall_p_struct*)
_tmp177)->f1;_tmp183=((struct Cyc_Absyn_UnknownCall_p_struct*)_tmp177)->f2;
_tmp184=((struct Cyc_Absyn_UnknownCall_p_struct*)_tmp177)->f3;if(_tmp184 != 0)goto
_LLDD;_LLDC: {struct Cyc_Absyn_Exp*e1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_UnknownId_e_struct*
_tmp18A=_cycalloc(sizeof(*_tmp18A));_tmp18A[0]=({struct Cyc_Absyn_UnknownId_e_struct
_tmp18B;_tmp18B.tag=2;_tmp18B.f1=_tmp182;_tmp18B;});_tmp18A;}),p->loc);struct Cyc_List_List*
es=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Pat*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Parse_pat2exp,_tmp183);return Cyc_Absyn_unknowncall_exp(
e1,es,p->loc);}_LLDD: if(*((int*)_tmp177)!= 14)goto _LLDF;_tmp185=((struct Cyc_Absyn_Exp_p_struct*)
_tmp177)->f1;_LLDE: return _tmp185;_LLDF:;_LLE0: Cyc_Parse_err(({const char*_tmp18C="cannot mix patterns and expressions in case";
_tag_dyneither(_tmp18C,sizeof(char),44);}),p->loc);return Cyc_Absyn_null_exp(p->loc);
_LLCC:;}union Cyc_YYSTYPE;struct _union_YYSTYPE_Int_tok{int tag;struct _tuple6 val;};
struct _union_YYSTYPE_Char_tok{int tag;char val;};struct _union_YYSTYPE_String_tok{
int tag;struct _dyneither_ptr val;};struct _union_YYSTYPE_Stringopt_tok{int tag;
struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_QualId_tok{int tag;struct _tuple1*
val;};struct _tuple18{struct Cyc_Position_Segment*f1;union Cyc_Absyn_Constraint*f2;
union Cyc_Absyn_Constraint*f3;};struct _union_YYSTYPE_YY1{int tag;struct _tuple18*
val;};struct _union_YYSTYPE_YY2{int tag;union Cyc_Absyn_Constraint*val;};struct
_union_YYSTYPE_YY3{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_YY4{int
tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_YY5{int tag;struct Cyc_List_List*
val;};struct _union_YYSTYPE_YY6{int tag;struct Cyc_List_List*val;};struct
_union_YYSTYPE_YY7{int tag;void*val;};struct _union_YYSTYPE_YY8{int tag;struct Cyc_Core_Opt*
val;};struct _union_YYSTYPE_YY9{int tag;struct Cyc_Absyn_Stmt*val;};struct
_union_YYSTYPE_YY10{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY11{
int tag;struct Cyc_Absyn_Pat*val;};struct _tuple19{struct Cyc_List_List*f1;int f2;};
struct _union_YYSTYPE_YY12{int tag;struct _tuple19*val;};struct _union_YYSTYPE_YY13{
int tag;struct Cyc_List_List*val;};struct _tuple20{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*
f2;};struct _union_YYSTYPE_YY14{int tag;struct _tuple20*val;};struct
_union_YYSTYPE_YY15{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY16{
int tag;struct _tuple19*val;};struct _union_YYSTYPE_YY17{int tag;struct Cyc_Absyn_Fndecl*
val;};struct _union_YYSTYPE_YY18{int tag;struct Cyc_List_List*val;};struct
_union_YYSTYPE_YY19{int tag;struct Cyc_Parse_Declaration_spec*val;};struct _tuple21{
struct Cyc_Parse_Declarator*f1;struct Cyc_Absyn_Exp*f2;};struct _union_YYSTYPE_YY20{
int tag;struct _tuple21*val;};struct _union_YYSTYPE_YY21{int tag;struct Cyc_List_List*
val;};struct _union_YYSTYPE_YY22{int tag;void*val;};struct _union_YYSTYPE_YY23{int
tag;void*val;};struct _union_YYSTYPE_YY24{int tag;void*val;};struct
_union_YYSTYPE_YY25{int tag;struct Cyc_Absyn_Tqual val;};struct _union_YYSTYPE_YY26{
int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY27{int tag;struct Cyc_List_List*
val;};struct _union_YYSTYPE_YY28{int tag;struct Cyc_List_List*val;};struct
_union_YYSTYPE_YY29{int tag;struct Cyc_Parse_Declarator*val;};struct
_union_YYSTYPE_YY30{int tag;struct Cyc_Parse_Abstractdeclarator*val;};struct
_union_YYSTYPE_YY31{int tag;int val;};struct _union_YYSTYPE_YY32{int tag;void*val;};
struct _union_YYSTYPE_YY33{int tag;struct Cyc_Absyn_Datatypefield*val;};struct
_union_YYSTYPE_YY34{int tag;struct Cyc_List_List*val;};struct _tuple22{struct Cyc_Absyn_Tqual
f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};struct _union_YYSTYPE_YY35{int
tag;struct _tuple22*val;};struct _union_YYSTYPE_YY36{int tag;struct Cyc_List_List*
val;};struct _union_YYSTYPE_YY37{int tag;struct _tuple8*val;};struct
_union_YYSTYPE_YY38{int tag;struct Cyc_List_List*val;};struct _tuple23{struct Cyc_List_List*
f1;int f2;struct Cyc_Absyn_VarargInfo*f3;struct Cyc_Core_Opt*f4;struct Cyc_List_List*
f5;};struct _union_YYSTYPE_YY39{int tag;struct _tuple23*val;};struct
_union_YYSTYPE_YY40{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY41{
int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY42{int tag;void*val;};
struct _union_YYSTYPE_YY43{int tag;void*val;};struct _union_YYSTYPE_YY44{int tag;
void*val;};struct _union_YYSTYPE_YY45{int tag;struct Cyc_List_List*val;};struct
_union_YYSTYPE_YY46{int tag;void*val;};struct _union_YYSTYPE_YY47{int tag;struct Cyc_Absyn_Enumfield*
val;};struct _union_YYSTYPE_YY48{int tag;struct Cyc_List_List*val;};struct
_union_YYSTYPE_YY49{int tag;struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_YY50{
int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY51{int tag;union Cyc_Absyn_Constraint*
val;};struct _union_YYSTYPE_YY52{int tag;struct Cyc_List_List*val;};struct
_union_YYSTYPE_YY53{int tag;void*val;};struct _union_YYSTYPE_YY54{int tag;struct Cyc_List_List*
val;};struct _union_YYSTYPE_YYINITIALSVAL{int tag;int val;};union Cyc_YYSTYPE{struct
_union_YYSTYPE_Int_tok Int_tok;struct _union_YYSTYPE_Char_tok Char_tok;struct
_union_YYSTYPE_String_tok String_tok;struct _union_YYSTYPE_Stringopt_tok
Stringopt_tok;struct _union_YYSTYPE_QualId_tok QualId_tok;struct _union_YYSTYPE_YY1
YY1;struct _union_YYSTYPE_YY2 YY2;struct _union_YYSTYPE_YY3 YY3;struct
_union_YYSTYPE_YY4 YY4;struct _union_YYSTYPE_YY5 YY5;struct _union_YYSTYPE_YY6 YY6;
struct _union_YYSTYPE_YY7 YY7;struct _union_YYSTYPE_YY8 YY8;struct _union_YYSTYPE_YY9
YY9;struct _union_YYSTYPE_YY10 YY10;struct _union_YYSTYPE_YY11 YY11;struct
_union_YYSTYPE_YY12 YY12;struct _union_YYSTYPE_YY13 YY13;struct _union_YYSTYPE_YY14
YY14;struct _union_YYSTYPE_YY15 YY15;struct _union_YYSTYPE_YY16 YY16;struct
_union_YYSTYPE_YY17 YY17;struct _union_YYSTYPE_YY18 YY18;struct _union_YYSTYPE_YY19
YY19;struct _union_YYSTYPE_YY20 YY20;struct _union_YYSTYPE_YY21 YY21;struct
_union_YYSTYPE_YY22 YY22;struct _union_YYSTYPE_YY23 YY23;struct _union_YYSTYPE_YY24
YY24;struct _union_YYSTYPE_YY25 YY25;struct _union_YYSTYPE_YY26 YY26;struct
_union_YYSTYPE_YY27 YY27;struct _union_YYSTYPE_YY28 YY28;struct _union_YYSTYPE_YY29
YY29;struct _union_YYSTYPE_YY30 YY30;struct _union_YYSTYPE_YY31 YY31;struct
_union_YYSTYPE_YY32 YY32;struct _union_YYSTYPE_YY33 YY33;struct _union_YYSTYPE_YY34
YY34;struct _union_YYSTYPE_YY35 YY35;struct _union_YYSTYPE_YY36 YY36;struct
_union_YYSTYPE_YY37 YY37;struct _union_YYSTYPE_YY38 YY38;struct _union_YYSTYPE_YY39
YY39;struct _union_YYSTYPE_YY40 YY40;struct _union_YYSTYPE_YY41 YY41;struct
_union_YYSTYPE_YY42 YY42;struct _union_YYSTYPE_YY43 YY43;struct _union_YYSTYPE_YY44
YY44;struct _union_YYSTYPE_YY45 YY45;struct _union_YYSTYPE_YY46 YY46;struct
_union_YYSTYPE_YY47 YY47;struct _union_YYSTYPE_YY48 YY48;struct _union_YYSTYPE_YY49
YY49;struct _union_YYSTYPE_YY50 YY50;struct _union_YYSTYPE_YY51 YY51;struct
_union_YYSTYPE_YY52 YY52;struct _union_YYSTYPE_YY53 YY53;struct _union_YYSTYPE_YY54
YY54;struct _union_YYSTYPE_YYINITIALSVAL YYINITIALSVAL;};static char _tmp18E[14]="$(sign_t,int)";
static struct Cyc_Core_Failure_struct Cyc_yyfail_Int_tok={Cyc_Core_Failure,{_tmp18E,
_tmp18E,_tmp18E + 14}};struct _tuple6 Cyc_yyget_Int_tok(union Cyc_YYSTYPE yy1){struct
_tuple6 yyzzz;{union Cyc_YYSTYPE _tmp18F=yy1;struct _tuple6 _tmp190;_LLE2: if((_tmp18F.Int_tok).tag
!= 1)goto _LLE4;_tmp190=(struct _tuple6)(_tmp18F.Int_tok).val;_LLE3: yyzzz=_tmp190;
goto _LLE1;_LLE4:;_LLE5:(int)_throw((void*)& Cyc_yyfail_Int_tok);_LLE1:;}return
yyzzz;}union Cyc_YYSTYPE Cyc_Int_tok(struct _tuple6 yy1){return({union Cyc_YYSTYPE
_tmp191;(_tmp191.Int_tok).val=yy1;(_tmp191.Int_tok).tag=1;_tmp191;});}static char
_tmp193[5]="char";static struct Cyc_Core_Failure_struct Cyc_yyfail_Char_tok={Cyc_Core_Failure,{
_tmp193,_tmp193,_tmp193 + 5}};char Cyc_yyget_Char_tok(union Cyc_YYSTYPE yy1){char
yyzzz;{union Cyc_YYSTYPE _tmp194=yy1;char _tmp195;_LLE7: if((_tmp194.Char_tok).tag != 
2)goto _LLE9;_tmp195=(char)(_tmp194.Char_tok).val;_LLE8: yyzzz=_tmp195;goto _LLE6;
_LLE9:;_LLEA:(int)_throw((void*)& Cyc_yyfail_Char_tok);_LLE6:;}return yyzzz;}union
Cyc_YYSTYPE Cyc_Char_tok(char yy1){return({union Cyc_YYSTYPE _tmp196;(_tmp196.Char_tok).val=
yy1;(_tmp196.Char_tok).tag=2;_tmp196;});}static char _tmp198[13]="string_t<`H>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_String_tok={Cyc_Core_Failure,{
_tmp198,_tmp198,_tmp198 + 13}};struct _dyneither_ptr Cyc_yyget_String_tok(union Cyc_YYSTYPE
yy1){struct _dyneither_ptr yyzzz;{union Cyc_YYSTYPE _tmp199=yy1;struct _dyneither_ptr
_tmp19A;_LLEC: if((_tmp199.String_tok).tag != 3)goto _LLEE;_tmp19A=(struct
_dyneither_ptr)(_tmp199.String_tok).val;_LLED: yyzzz=_tmp19A;goto _LLEB;_LLEE:;
_LLEF:(int)_throw((void*)& Cyc_yyfail_String_tok);_LLEB:;}return yyzzz;}union Cyc_YYSTYPE
Cyc_String_tok(struct _dyneither_ptr yy1){return({union Cyc_YYSTYPE _tmp19B;(_tmp19B.String_tok).val=
yy1;(_tmp19B.String_tok).tag=3;_tmp19B;});}static char _tmp19D[56]="$(Position::seg_t,conref_t<bool>,conref_t<bounds_t>)@`H";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY1={Cyc_Core_Failure,{_tmp19D,
_tmp19D,_tmp19D + 56}};struct _tuple18*Cyc_yyget_YY1(union Cyc_YYSTYPE yy1){struct
_tuple18*yyzzz;{union Cyc_YYSTYPE _tmp19E=yy1;struct _tuple18*_tmp19F;_LLF1: if((
_tmp19E.YY1).tag != 6)goto _LLF3;_tmp19F=(struct _tuple18*)(_tmp19E.YY1).val;_LLF2:
yyzzz=_tmp19F;goto _LLF0;_LLF3:;_LLF4:(int)_throw((void*)& Cyc_yyfail_YY1);_LLF0:;}
return yyzzz;}union Cyc_YYSTYPE Cyc_YY1(struct _tuple18*yy1){return({union Cyc_YYSTYPE
_tmp1A0;(_tmp1A0.YY1).val=yy1;(_tmp1A0.YY1).tag=6;_tmp1A0;});}static char _tmp1A2[
19]="conref_t<bounds_t>";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY2={Cyc_Core_Failure,{
_tmp1A2,_tmp1A2,_tmp1A2 + 19}};union Cyc_Absyn_Constraint*Cyc_yyget_YY2(union Cyc_YYSTYPE
yy1){union Cyc_Absyn_Constraint*yyzzz;{union Cyc_YYSTYPE _tmp1A3=yy1;union Cyc_Absyn_Constraint*
_tmp1A4;_LLF6: if((_tmp1A3.YY2).tag != 7)goto _LLF8;_tmp1A4=(union Cyc_Absyn_Constraint*)(
_tmp1A3.YY2).val;_LLF7: yyzzz=_tmp1A4;goto _LLF5;_LLF8:;_LLF9:(int)_throw((void*)&
Cyc_yyfail_YY2);_LLF5:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY2(union Cyc_Absyn_Constraint*
yy1){return({union Cyc_YYSTYPE _tmp1A5;(_tmp1A5.YY2).val=yy1;(_tmp1A5.YY2).tag=7;
_tmp1A5;});}static char _tmp1A7[6]="exp_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY3={
Cyc_Core_Failure,{_tmp1A7,_tmp1A7,_tmp1A7 + 6}};struct Cyc_Absyn_Exp*Cyc_yyget_YY3(
union Cyc_YYSTYPE yy1){struct Cyc_Absyn_Exp*yyzzz;{union Cyc_YYSTYPE _tmp1A8=yy1;
struct Cyc_Absyn_Exp*_tmp1A9;_LLFB: if((_tmp1A8.YY3).tag != 8)goto _LLFD;_tmp1A9=(
struct Cyc_Absyn_Exp*)(_tmp1A8.YY3).val;_LLFC: yyzzz=_tmp1A9;goto _LLFA;_LLFD:;
_LLFE:(int)_throw((void*)& Cyc_yyfail_YY3);_LLFA:;}return yyzzz;}union Cyc_YYSTYPE
Cyc_YY3(struct Cyc_Absyn_Exp*yy1){return({union Cyc_YYSTYPE _tmp1AA;(_tmp1AA.YY3).val=
yy1;(_tmp1AA.YY3).tag=8;_tmp1AA;});}static char _tmp1AC[10]="exp_opt_t";static
struct Cyc_Core_Failure_struct Cyc_yyfail_YY4={Cyc_Core_Failure,{_tmp1AC,_tmp1AC,
_tmp1AC + 10}};struct Cyc_Absyn_Exp*Cyc_yyget_YY4(union Cyc_YYSTYPE yy1){struct Cyc_Absyn_Exp*
yyzzz;{union Cyc_YYSTYPE _tmp1AD=yy1;struct Cyc_Absyn_Exp*_tmp1AE;_LL100: if((
_tmp1AD.YY4).tag != 9)goto _LL102;_tmp1AE=(struct Cyc_Absyn_Exp*)(_tmp1AD.YY4).val;
_LL101: yyzzz=_tmp1AE;goto _LLFF;_LL102:;_LL103:(int)_throw((void*)& Cyc_yyfail_YY4);
_LLFF:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY4(struct Cyc_Absyn_Exp*yy1){return({
union Cyc_YYSTYPE _tmp1AF;(_tmp1AF.YY4).val=yy1;(_tmp1AF.YY4).tag=9;_tmp1AF;});}
static char _tmp1B1[17]="list_t<exp_t,`H>";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY5={
Cyc_Core_Failure,{_tmp1B1,_tmp1B1,_tmp1B1 + 17}};struct Cyc_List_List*Cyc_yyget_YY5(
union Cyc_YYSTYPE yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp1B2=yy1;
struct Cyc_List_List*_tmp1B3;_LL105: if((_tmp1B2.YY5).tag != 10)goto _LL107;_tmp1B3=(
struct Cyc_List_List*)(_tmp1B2.YY5).val;_LL106: yyzzz=_tmp1B3;goto _LL104;_LL107:;
_LL108:(int)_throw((void*)& Cyc_yyfail_YY5);_LL104:;}return yyzzz;}union Cyc_YYSTYPE
Cyc_YY5(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp1B4;(_tmp1B4.YY5).val=
yy1;(_tmp1B4.YY5).tag=10;_tmp1B4;});}static char _tmp1B6[47]="list_t<$(list_t<designator_t,`H>,exp_t)@`H,`H>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY6={Cyc_Core_Failure,{_tmp1B6,
_tmp1B6,_tmp1B6 + 47}};struct Cyc_List_List*Cyc_yyget_YY6(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp1B7=yy1;struct Cyc_List_List*
_tmp1B8;_LL10A: if((_tmp1B7.YY6).tag != 11)goto _LL10C;_tmp1B8=(struct Cyc_List_List*)(
_tmp1B7.YY6).val;_LL10B: yyzzz=_tmp1B8;goto _LL109;_LL10C:;_LL10D:(int)_throw((
void*)& Cyc_yyfail_YY6);_LL109:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY6(struct Cyc_List_List*
yy1){return({union Cyc_YYSTYPE _tmp1B9;(_tmp1B9.YY6).val=yy1;(_tmp1B9.YY6).tag=11;
_tmp1B9;});}static char _tmp1BB[9]="primop_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY7={Cyc_Core_Failure,{_tmp1BB,_tmp1BB,_tmp1BB + 9}};void*Cyc_yyget_YY7(
union Cyc_YYSTYPE yy1){void*yyzzz;{union Cyc_YYSTYPE _tmp1BC=yy1;void*_tmp1BD;_LL10F:
if((_tmp1BC.YY7).tag != 12)goto _LL111;_tmp1BD=(void*)(_tmp1BC.YY7).val;_LL110:
yyzzz=_tmp1BD;goto _LL10E;_LL111:;_LL112:(int)_throw((void*)& Cyc_yyfail_YY7);
_LL10E:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY7(void*yy1){return({union Cyc_YYSTYPE
_tmp1BE;(_tmp1BE.YY7).val=yy1;(_tmp1BE.YY7).tag=12;_tmp1BE;});}static char _tmp1C0[
19]="opt_t<primop_t,`H>";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY8={Cyc_Core_Failure,{
_tmp1C0,_tmp1C0,_tmp1C0 + 19}};struct Cyc_Core_Opt*Cyc_yyget_YY8(union Cyc_YYSTYPE
yy1){struct Cyc_Core_Opt*yyzzz;{union Cyc_YYSTYPE _tmp1C1=yy1;struct Cyc_Core_Opt*
_tmp1C2;_LL114: if((_tmp1C1.YY8).tag != 13)goto _LL116;_tmp1C2=(struct Cyc_Core_Opt*)(
_tmp1C1.YY8).val;_LL115: yyzzz=_tmp1C2;goto _LL113;_LL116:;_LL117:(int)_throw((
void*)& Cyc_yyfail_YY8);_LL113:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY8(struct Cyc_Core_Opt*
yy1){return({union Cyc_YYSTYPE _tmp1C3;(_tmp1C3.YY8).val=yy1;(_tmp1C3.YY8).tag=13;
_tmp1C3;});}static char _tmp1C5[7]="qvar_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_QualId_tok={
Cyc_Core_Failure,{_tmp1C5,_tmp1C5,_tmp1C5 + 7}};struct _tuple1*Cyc_yyget_QualId_tok(
union Cyc_YYSTYPE yy1){struct _tuple1*yyzzz;{union Cyc_YYSTYPE _tmp1C6=yy1;struct
_tuple1*_tmp1C7;_LL119: if((_tmp1C6.QualId_tok).tag != 5)goto _LL11B;_tmp1C7=(
struct _tuple1*)(_tmp1C6.QualId_tok).val;_LL11A: yyzzz=_tmp1C7;goto _LL118;_LL11B:;
_LL11C:(int)_throw((void*)& Cyc_yyfail_QualId_tok);_LL118:;}return yyzzz;}union Cyc_YYSTYPE
Cyc_QualId_tok(struct _tuple1*yy1){return({union Cyc_YYSTYPE _tmp1C8;(_tmp1C8.QualId_tok).val=
yy1;(_tmp1C8.QualId_tok).tag=5;_tmp1C8;});}static char _tmp1CA[7]="stmt_t";static
struct Cyc_Core_Failure_struct Cyc_yyfail_YY9={Cyc_Core_Failure,{_tmp1CA,_tmp1CA,
_tmp1CA + 7}};struct Cyc_Absyn_Stmt*Cyc_yyget_YY9(union Cyc_YYSTYPE yy1){struct Cyc_Absyn_Stmt*
yyzzz;{union Cyc_YYSTYPE _tmp1CB=yy1;struct Cyc_Absyn_Stmt*_tmp1CC;_LL11E: if((
_tmp1CB.YY9).tag != 14)goto _LL120;_tmp1CC=(struct Cyc_Absyn_Stmt*)(_tmp1CB.YY9).val;
_LL11F: yyzzz=_tmp1CC;goto _LL11D;_LL120:;_LL121:(int)_throw((void*)& Cyc_yyfail_YY9);
_LL11D:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY9(struct Cyc_Absyn_Stmt*yy1){return({
union Cyc_YYSTYPE _tmp1CD;(_tmp1CD.YY9).val=yy1;(_tmp1CD.YY9).tag=14;_tmp1CD;});}
static char _tmp1CF[27]="list_t<switch_clause_t,`H>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY10={Cyc_Core_Failure,{_tmp1CF,_tmp1CF,_tmp1CF + 27}};struct Cyc_List_List*
Cyc_yyget_YY10(union Cyc_YYSTYPE yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE
_tmp1D0=yy1;struct Cyc_List_List*_tmp1D1;_LL123: if((_tmp1D0.YY10).tag != 15)goto
_LL125;_tmp1D1=(struct Cyc_List_List*)(_tmp1D0.YY10).val;_LL124: yyzzz=_tmp1D1;
goto _LL122;_LL125:;_LL126:(int)_throw((void*)& Cyc_yyfail_YY10);_LL122:;}return
yyzzz;}union Cyc_YYSTYPE Cyc_YY10(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE
_tmp1D2;(_tmp1D2.YY10).val=yy1;(_tmp1D2.YY10).tag=15;_tmp1D2;});}static char
_tmp1D4[6]="pat_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY11={Cyc_Core_Failure,{
_tmp1D4,_tmp1D4,_tmp1D4 + 6}};struct Cyc_Absyn_Pat*Cyc_yyget_YY11(union Cyc_YYSTYPE
yy1){struct Cyc_Absyn_Pat*yyzzz;{union Cyc_YYSTYPE _tmp1D5=yy1;struct Cyc_Absyn_Pat*
_tmp1D6;_LL128: if((_tmp1D5.YY11).tag != 16)goto _LL12A;_tmp1D6=(struct Cyc_Absyn_Pat*)(
_tmp1D5.YY11).val;_LL129: yyzzz=_tmp1D6;goto _LL127;_LL12A:;_LL12B:(int)_throw((
void*)& Cyc_yyfail_YY11);_LL127:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY11(struct Cyc_Absyn_Pat*
yy1){return({union Cyc_YYSTYPE _tmp1D7;(_tmp1D7.YY11).val=yy1;(_tmp1D7.YY11).tag=
16;_tmp1D7;});}static char _tmp1D9[28]="$(list_t<pat_t,`H>,bool)@`H";static struct
Cyc_Core_Failure_struct Cyc_yyfail_YY12={Cyc_Core_Failure,{_tmp1D9,_tmp1D9,
_tmp1D9 + 28}};struct _tuple19*Cyc_yyget_YY12(union Cyc_YYSTYPE yy1){struct _tuple19*
yyzzz;{union Cyc_YYSTYPE _tmp1DA=yy1;struct _tuple19*_tmp1DB;_LL12D: if((_tmp1DA.YY12).tag
!= 17)goto _LL12F;_tmp1DB=(struct _tuple19*)(_tmp1DA.YY12).val;_LL12E: yyzzz=
_tmp1DB;goto _LL12C;_LL12F:;_LL130:(int)_throw((void*)& Cyc_yyfail_YY12);_LL12C:;}
return yyzzz;}union Cyc_YYSTYPE Cyc_YY12(struct _tuple19*yy1){return({union Cyc_YYSTYPE
_tmp1DC;(_tmp1DC.YY12).val=yy1;(_tmp1DC.YY12).tag=17;_tmp1DC;});}static char
_tmp1DE[17]="list_t<pat_t,`H>";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY13={
Cyc_Core_Failure,{_tmp1DE,_tmp1DE,_tmp1DE + 17}};struct Cyc_List_List*Cyc_yyget_YY13(
union Cyc_YYSTYPE yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp1DF=yy1;
struct Cyc_List_List*_tmp1E0;_LL132: if((_tmp1DF.YY13).tag != 18)goto _LL134;_tmp1E0=(
struct Cyc_List_List*)(_tmp1DF.YY13).val;_LL133: yyzzz=_tmp1E0;goto _LL131;_LL134:;
_LL135:(int)_throw((void*)& Cyc_yyfail_YY13);_LL131:;}return yyzzz;}union Cyc_YYSTYPE
Cyc_YY13(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp1E1;(_tmp1E1.YY13).val=
yy1;(_tmp1E1.YY13).tag=18;_tmp1E1;});}static char _tmp1E3[36]="$(list_t<designator_t,`H>,pat_t)@`H";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY14={Cyc_Core_Failure,{_tmp1E3,
_tmp1E3,_tmp1E3 + 36}};struct _tuple20*Cyc_yyget_YY14(union Cyc_YYSTYPE yy1){struct
_tuple20*yyzzz;{union Cyc_YYSTYPE _tmp1E4=yy1;struct _tuple20*_tmp1E5;_LL137: if((
_tmp1E4.YY14).tag != 19)goto _LL139;_tmp1E5=(struct _tuple20*)(_tmp1E4.YY14).val;
_LL138: yyzzz=_tmp1E5;goto _LL136;_LL139:;_LL13A:(int)_throw((void*)& Cyc_yyfail_YY14);
_LL136:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY14(struct _tuple20*yy1){return({union
Cyc_YYSTYPE _tmp1E6;(_tmp1E6.YY14).val=yy1;(_tmp1E6.YY14).tag=19;_tmp1E6;});}
static char _tmp1E8[47]="list_t<$(list_t<designator_t,`H>,pat_t)@`H,`H>";static
struct Cyc_Core_Failure_struct Cyc_yyfail_YY15={Cyc_Core_Failure,{_tmp1E8,_tmp1E8,
_tmp1E8 + 47}};struct Cyc_List_List*Cyc_yyget_YY15(union Cyc_YYSTYPE yy1){struct Cyc_List_List*
yyzzz;{union Cyc_YYSTYPE _tmp1E9=yy1;struct Cyc_List_List*_tmp1EA;_LL13C: if((
_tmp1E9.YY15).tag != 20)goto _LL13E;_tmp1EA=(struct Cyc_List_List*)(_tmp1E9.YY15).val;
_LL13D: yyzzz=_tmp1EA;goto _LL13B;_LL13E:;_LL13F:(int)_throw((void*)& Cyc_yyfail_YY15);
_LL13B:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY15(struct Cyc_List_List*yy1){return({
union Cyc_YYSTYPE _tmp1EB;(_tmp1EB.YY15).val=yy1;(_tmp1EB.YY15).tag=20;_tmp1EB;});}
static char _tmp1ED[58]="$(list_t<$(list_t<designator_t,`H>,pat_t)@`H,`H>,bool)@`H";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY16={Cyc_Core_Failure,{_tmp1ED,
_tmp1ED,_tmp1ED + 58}};struct _tuple19*Cyc_yyget_YY16(union Cyc_YYSTYPE yy1){struct
_tuple19*yyzzz;{union Cyc_YYSTYPE _tmp1EE=yy1;struct _tuple19*_tmp1EF;_LL141: if((
_tmp1EE.YY16).tag != 21)goto _LL143;_tmp1EF=(struct _tuple19*)(_tmp1EE.YY16).val;
_LL142: yyzzz=_tmp1EF;goto _LL140;_LL143:;_LL144:(int)_throw((void*)& Cyc_yyfail_YY16);
_LL140:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY16(struct _tuple19*yy1){return({union
Cyc_YYSTYPE _tmp1F0;(_tmp1F0.YY16).val=yy1;(_tmp1F0.YY16).tag=21;_tmp1F0;});}
static char _tmp1F2[9]="fndecl_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY17={
Cyc_Core_Failure,{_tmp1F2,_tmp1F2,_tmp1F2 + 9}};struct Cyc_Absyn_Fndecl*Cyc_yyget_YY17(
union Cyc_YYSTYPE yy1){struct Cyc_Absyn_Fndecl*yyzzz;{union Cyc_YYSTYPE _tmp1F3=yy1;
struct Cyc_Absyn_Fndecl*_tmp1F4;_LL146: if((_tmp1F3.YY17).tag != 22)goto _LL148;
_tmp1F4=(struct Cyc_Absyn_Fndecl*)(_tmp1F3.YY17).val;_LL147: yyzzz=_tmp1F4;goto
_LL145;_LL148:;_LL149:(int)_throw((void*)& Cyc_yyfail_YY17);_LL145:;}return yyzzz;}
union Cyc_YYSTYPE Cyc_YY17(struct Cyc_Absyn_Fndecl*yy1){return({union Cyc_YYSTYPE
_tmp1F5;(_tmp1F5.YY17).val=yy1;(_tmp1F5.YY17).tag=22;_tmp1F5;});}static char
_tmp1F7[18]="list_t<decl_t,`H>";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY18={
Cyc_Core_Failure,{_tmp1F7,_tmp1F7,_tmp1F7 + 18}};struct Cyc_List_List*Cyc_yyget_YY18(
union Cyc_YYSTYPE yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp1F8=yy1;
struct Cyc_List_List*_tmp1F9;_LL14B: if((_tmp1F8.YY18).tag != 23)goto _LL14D;_tmp1F9=(
struct Cyc_List_List*)(_tmp1F8.YY18).val;_LL14C: yyzzz=_tmp1F9;goto _LL14A;_LL14D:;
_LL14E:(int)_throw((void*)& Cyc_yyfail_YY18);_LL14A:;}return yyzzz;}union Cyc_YYSTYPE
Cyc_YY18(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp1FA;(_tmp1FA.YY18).val=
yy1;(_tmp1FA.YY18).tag=23;_tmp1FA;});}static char _tmp1FC[12]="decl_spec_t";static
struct Cyc_Core_Failure_struct Cyc_yyfail_YY19={Cyc_Core_Failure,{_tmp1FC,_tmp1FC,
_tmp1FC + 12}};struct Cyc_Parse_Declaration_spec*Cyc_yyget_YY19(union Cyc_YYSTYPE
yy1){struct Cyc_Parse_Declaration_spec*yyzzz;{union Cyc_YYSTYPE _tmp1FD=yy1;struct
Cyc_Parse_Declaration_spec*_tmp1FE;_LL150: if((_tmp1FD.YY19).tag != 24)goto _LL152;
_tmp1FE=(struct Cyc_Parse_Declaration_spec*)(_tmp1FD.YY19).val;_LL151: yyzzz=
_tmp1FE;goto _LL14F;_LL152:;_LL153:(int)_throw((void*)& Cyc_yyfail_YY19);_LL14F:;}
return yyzzz;}union Cyc_YYSTYPE Cyc_YY19(struct Cyc_Parse_Declaration_spec*yy1){
return({union Cyc_YYSTYPE _tmp1FF;(_tmp1FF.YY19).val=yy1;(_tmp1FF.YY19).tag=24;
_tmp1FF;});}static char _tmp201[29]="$(declarator_t,exp_opt_t)@`H";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY20={Cyc_Core_Failure,{_tmp201,_tmp201,_tmp201 + 29}};struct _tuple21*
Cyc_yyget_YY20(union Cyc_YYSTYPE yy1){struct _tuple21*yyzzz;{union Cyc_YYSTYPE
_tmp202=yy1;struct _tuple21*_tmp203;_LL155: if((_tmp202.YY20).tag != 25)goto _LL157;
_tmp203=(struct _tuple21*)(_tmp202.YY20).val;_LL156: yyzzz=_tmp203;goto _LL154;
_LL157:;_LL158:(int)_throw((void*)& Cyc_yyfail_YY20);_LL154:;}return yyzzz;}union
Cyc_YYSTYPE Cyc_YY20(struct _tuple21*yy1){return({union Cyc_YYSTYPE _tmp204;(_tmp204.YY20).val=
yy1;(_tmp204.YY20).tag=25;_tmp204;});}static char _tmp206[40]="list_t<$(declarator_t,exp_opt_t)@`H,`H>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY21={Cyc_Core_Failure,{_tmp206,
_tmp206,_tmp206 + 40}};struct Cyc_List_List*Cyc_yyget_YY21(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp207=yy1;struct Cyc_List_List*
_tmp208;_LL15A: if((_tmp207.YY21).tag != 26)goto _LL15C;_tmp208=(struct Cyc_List_List*)(
_tmp207.YY21).val;_LL15B: yyzzz=_tmp208;goto _LL159;_LL15C:;_LL15D:(int)_throw((
void*)& Cyc_yyfail_YY21);_LL159:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY21(struct Cyc_List_List*
yy1){return({union Cyc_YYSTYPE _tmp209;(_tmp209.YY21).val=yy1;(_tmp209.YY21).tag=
26;_tmp209;});}static char _tmp20B[16]="storage_class_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY22={Cyc_Core_Failure,{_tmp20B,_tmp20B,_tmp20B + 16}};void*Cyc_yyget_YY22(
union Cyc_YYSTYPE yy1){void*yyzzz;{union Cyc_YYSTYPE _tmp20C=yy1;void*_tmp20D;_LL15F:
if((_tmp20C.YY22).tag != 27)goto _LL161;_tmp20D=(void*)(_tmp20C.YY22).val;_LL160:
yyzzz=_tmp20D;goto _LL15E;_LL161:;_LL162:(int)_throw((void*)& Cyc_yyfail_YY22);
_LL15E:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY22(void*yy1){return({union Cyc_YYSTYPE
_tmp20E;(_tmp20E.YY22).val=yy1;(_tmp20E.YY22).tag=27;_tmp20E;});}static char
_tmp210[17]="type_specifier_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY23={
Cyc_Core_Failure,{_tmp210,_tmp210,_tmp210 + 17}};void*Cyc_yyget_YY23(union Cyc_YYSTYPE
yy1){void*yyzzz;{union Cyc_YYSTYPE _tmp211=yy1;void*_tmp212;_LL164: if((_tmp211.YY23).tag
!= 28)goto _LL166;_tmp212=(void*)(_tmp211.YY23).val;_LL165: yyzzz=_tmp212;goto
_LL163;_LL166:;_LL167:(int)_throw((void*)& Cyc_yyfail_YY23);_LL163:;}return yyzzz;}
union Cyc_YYSTYPE Cyc_YY23(void*yy1){return({union Cyc_YYSTYPE _tmp213;(_tmp213.YY23).val=
yy1;(_tmp213.YY23).tag=28;_tmp213;});}static char _tmp215[12]="aggr_kind_t";static
struct Cyc_Core_Failure_struct Cyc_yyfail_YY24={Cyc_Core_Failure,{_tmp215,_tmp215,
_tmp215 + 12}};void*Cyc_yyget_YY24(union Cyc_YYSTYPE yy1){void*yyzzz;{union Cyc_YYSTYPE
_tmp216=yy1;void*_tmp217;_LL169: if((_tmp216.YY24).tag != 29)goto _LL16B;_tmp217=(
void*)(_tmp216.YY24).val;_LL16A: yyzzz=_tmp217;goto _LL168;_LL16B:;_LL16C:(int)
_throw((void*)& Cyc_yyfail_YY24);_LL168:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY24(
void*yy1){return({union Cyc_YYSTYPE _tmp218;(_tmp218.YY24).val=yy1;(_tmp218.YY24).tag=
29;_tmp218;});}static char _tmp21A[8]="tqual_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY25={Cyc_Core_Failure,{_tmp21A,_tmp21A,_tmp21A + 8}};struct Cyc_Absyn_Tqual
Cyc_yyget_YY25(union Cyc_YYSTYPE yy1){struct Cyc_Absyn_Tqual yyzzz;{union Cyc_YYSTYPE
_tmp21B=yy1;struct Cyc_Absyn_Tqual _tmp21C;_LL16E: if((_tmp21B.YY25).tag != 30)goto
_LL170;_tmp21C=(struct Cyc_Absyn_Tqual)(_tmp21B.YY25).val;_LL16F: yyzzz=_tmp21C;
goto _LL16D;_LL170:;_LL171:(int)_throw((void*)& Cyc_yyfail_YY25);_LL16D:;}return
yyzzz;}union Cyc_YYSTYPE Cyc_YY25(struct Cyc_Absyn_Tqual yy1){return({union Cyc_YYSTYPE
_tmp21D;(_tmp21D.YY25).val=yy1;(_tmp21D.YY25).tag=30;_tmp21D;});}static char
_tmp21F[23]="list_t<aggrfield_t,`H>";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY26={
Cyc_Core_Failure,{_tmp21F,_tmp21F,_tmp21F + 23}};struct Cyc_List_List*Cyc_yyget_YY26(
union Cyc_YYSTYPE yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp220=yy1;
struct Cyc_List_List*_tmp221;_LL173: if((_tmp220.YY26).tag != 31)goto _LL175;_tmp221=(
struct Cyc_List_List*)(_tmp220.YY26).val;_LL174: yyzzz=_tmp221;goto _LL172;_LL175:;
_LL176:(int)_throw((void*)& Cyc_yyfail_YY26);_LL172:;}return yyzzz;}union Cyc_YYSTYPE
Cyc_YY26(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp222;(_tmp222.YY26).val=
yy1;(_tmp222.YY26).tag=31;_tmp222;});}static char _tmp224[34]="list_t<list_t<aggrfield_t,`H>,`H>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY27={Cyc_Core_Failure,{_tmp224,
_tmp224,_tmp224 + 34}};struct Cyc_List_List*Cyc_yyget_YY27(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp225=yy1;struct Cyc_List_List*
_tmp226;_LL178: if((_tmp225.YY27).tag != 32)goto _LL17A;_tmp226=(struct Cyc_List_List*)(
_tmp225.YY27).val;_LL179: yyzzz=_tmp226;goto _LL177;_LL17A:;_LL17B:(int)_throw((
void*)& Cyc_yyfail_YY27);_LL177:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY27(struct Cyc_List_List*
yy1){return({union Cyc_YYSTYPE _tmp227;(_tmp227.YY27).val=yy1;(_tmp227.YY27).tag=
32;_tmp227;});}static char _tmp229[31]="list_t<type_modifier_t<`H>,`H>";static
struct Cyc_Core_Failure_struct Cyc_yyfail_YY28={Cyc_Core_Failure,{_tmp229,_tmp229,
_tmp229 + 31}};struct Cyc_List_List*Cyc_yyget_YY28(union Cyc_YYSTYPE yy1){struct Cyc_List_List*
yyzzz;{union Cyc_YYSTYPE _tmp22A=yy1;struct Cyc_List_List*_tmp22B;_LL17D: if((
_tmp22A.YY28).tag != 33)goto _LL17F;_tmp22B=(struct Cyc_List_List*)(_tmp22A.YY28).val;
_LL17E: yyzzz=_tmp22B;goto _LL17C;_LL17F:;_LL180:(int)_throw((void*)& Cyc_yyfail_YY28);
_LL17C:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY28(struct Cyc_List_List*yy1){return({
union Cyc_YYSTYPE _tmp22C;(_tmp22C.YY28).val=yy1;(_tmp22C.YY28).tag=33;_tmp22C;});}
static char _tmp22E[13]="declarator_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY29={
Cyc_Core_Failure,{_tmp22E,_tmp22E,_tmp22E + 13}};struct Cyc_Parse_Declarator*Cyc_yyget_YY29(
union Cyc_YYSTYPE yy1){struct Cyc_Parse_Declarator*yyzzz;{union Cyc_YYSTYPE _tmp22F=
yy1;struct Cyc_Parse_Declarator*_tmp230;_LL182: if((_tmp22F.YY29).tag != 34)goto
_LL184;_tmp230=(struct Cyc_Parse_Declarator*)(_tmp22F.YY29).val;_LL183: yyzzz=
_tmp230;goto _LL181;_LL184:;_LL185:(int)_throw((void*)& Cyc_yyfail_YY29);_LL181:;}
return yyzzz;}union Cyc_YYSTYPE Cyc_YY29(struct Cyc_Parse_Declarator*yy1){return({
union Cyc_YYSTYPE _tmp231;(_tmp231.YY29).val=yy1;(_tmp231.YY29).tag=34;_tmp231;});}
static char _tmp233[21]="abstractdeclarator_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY30={Cyc_Core_Failure,{_tmp233,_tmp233,_tmp233 + 21}};struct Cyc_Parse_Abstractdeclarator*
Cyc_yyget_YY30(union Cyc_YYSTYPE yy1){struct Cyc_Parse_Abstractdeclarator*yyzzz;{
union Cyc_YYSTYPE _tmp234=yy1;struct Cyc_Parse_Abstractdeclarator*_tmp235;_LL187:
if((_tmp234.YY30).tag != 35)goto _LL189;_tmp235=(struct Cyc_Parse_Abstractdeclarator*)(
_tmp234.YY30).val;_LL188: yyzzz=_tmp235;goto _LL186;_LL189:;_LL18A:(int)_throw((
void*)& Cyc_yyfail_YY30);_LL186:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY30(struct Cyc_Parse_Abstractdeclarator*
yy1){return({union Cyc_YYSTYPE _tmp236;(_tmp236.YY30).val=yy1;(_tmp236.YY30).tag=
35;_tmp236;});}static char _tmp238[5]="bool";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY31={Cyc_Core_Failure,{_tmp238,_tmp238,_tmp238 + 5}};int Cyc_yyget_YY31(
union Cyc_YYSTYPE yy1){int yyzzz;{union Cyc_YYSTYPE _tmp239=yy1;int _tmp23A;_LL18C: if((
_tmp239.YY31).tag != 36)goto _LL18E;_tmp23A=(int)(_tmp239.YY31).val;_LL18D: yyzzz=
_tmp23A;goto _LL18B;_LL18E:;_LL18F:(int)_throw((void*)& Cyc_yyfail_YY31);_LL18B:;}
return yyzzz;}union Cyc_YYSTYPE Cyc_YY31(int yy1){return({union Cyc_YYSTYPE _tmp23B;(
_tmp23B.YY31).val=yy1;(_tmp23B.YY31).tag=36;_tmp23B;});}static char _tmp23D[8]="scope_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY32={Cyc_Core_Failure,{_tmp23D,
_tmp23D,_tmp23D + 8}};void*Cyc_yyget_YY32(union Cyc_YYSTYPE yy1){void*yyzzz;{union
Cyc_YYSTYPE _tmp23E=yy1;void*_tmp23F;_LL191: if((_tmp23E.YY32).tag != 37)goto _LL193;
_tmp23F=(void*)(_tmp23E.YY32).val;_LL192: yyzzz=_tmp23F;goto _LL190;_LL193:;_LL194:(
int)_throw((void*)& Cyc_yyfail_YY32);_LL190:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY32(
void*yy1){return({union Cyc_YYSTYPE _tmp240;(_tmp240.YY32).val=yy1;(_tmp240.YY32).tag=
37;_tmp240;});}static char _tmp242[16]="datatypefield_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY33={Cyc_Core_Failure,{_tmp242,_tmp242,_tmp242 + 16}};struct Cyc_Absyn_Datatypefield*
Cyc_yyget_YY33(union Cyc_YYSTYPE yy1){struct Cyc_Absyn_Datatypefield*yyzzz;{union
Cyc_YYSTYPE _tmp243=yy1;struct Cyc_Absyn_Datatypefield*_tmp244;_LL196: if((_tmp243.YY33).tag
!= 38)goto _LL198;_tmp244=(struct Cyc_Absyn_Datatypefield*)(_tmp243.YY33).val;
_LL197: yyzzz=_tmp244;goto _LL195;_LL198:;_LL199:(int)_throw((void*)& Cyc_yyfail_YY33);
_LL195:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY33(struct Cyc_Absyn_Datatypefield*yy1){
return({union Cyc_YYSTYPE _tmp245;(_tmp245.YY33).val=yy1;(_tmp245.YY33).tag=38;
_tmp245;});}static char _tmp247[27]="list_t<datatypefield_t,`H>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY34={Cyc_Core_Failure,{_tmp247,_tmp247,_tmp247 + 27}};struct Cyc_List_List*
Cyc_yyget_YY34(union Cyc_YYSTYPE yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE
_tmp248=yy1;struct Cyc_List_List*_tmp249;_LL19B: if((_tmp248.YY34).tag != 39)goto
_LL19D;_tmp249=(struct Cyc_List_List*)(_tmp248.YY34).val;_LL19C: yyzzz=_tmp249;
goto _LL19A;_LL19D:;_LL19E:(int)_throw((void*)& Cyc_yyfail_YY34);_LL19A:;}return
yyzzz;}union Cyc_YYSTYPE Cyc_YY34(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE
_tmp24A;(_tmp24A.YY34).val=yy1;(_tmp24A.YY34).tag=39;_tmp24A;});}static char
_tmp24C[55]="$(tqual_t,list_t<type_specifier_t,`H>,attributes_t)@`H";static
struct Cyc_Core_Failure_struct Cyc_yyfail_YY35={Cyc_Core_Failure,{_tmp24C,_tmp24C,
_tmp24C + 55}};struct _tuple22*Cyc_yyget_YY35(union Cyc_YYSTYPE yy1){struct _tuple22*
yyzzz;{union Cyc_YYSTYPE _tmp24D=yy1;struct _tuple22*_tmp24E;_LL1A0: if((_tmp24D.YY35).tag
!= 40)goto _LL1A2;_tmp24E=(struct _tuple22*)(_tmp24D.YY35).val;_LL1A1: yyzzz=
_tmp24E;goto _LL19F;_LL1A2:;_LL1A3:(int)_throw((void*)& Cyc_yyfail_YY35);_LL19F:;}
return yyzzz;}union Cyc_YYSTYPE Cyc_YY35(struct _tuple22*yy1){return({union Cyc_YYSTYPE
_tmp24F;(_tmp24F.YY35).val=yy1;(_tmp24F.YY35).tag=40;_tmp24F;});}static char
_tmp251[17]="list_t<var_t,`H>";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY36={
Cyc_Core_Failure,{_tmp251,_tmp251,_tmp251 + 17}};struct Cyc_List_List*Cyc_yyget_YY36(
union Cyc_YYSTYPE yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp252=yy1;
struct Cyc_List_List*_tmp253;_LL1A5: if((_tmp252.YY36).tag != 41)goto _LL1A7;_tmp253=(
struct Cyc_List_List*)(_tmp252.YY36).val;_LL1A6: yyzzz=_tmp253;goto _LL1A4;_LL1A7:;
_LL1A8:(int)_throw((void*)& Cyc_yyfail_YY36);_LL1A4:;}return yyzzz;}union Cyc_YYSTYPE
Cyc_YY36(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp254;(_tmp254.YY36).val=
yy1;(_tmp254.YY36).tag=41;_tmp254;});}static char _tmp256[37]="$(opt_t<var_t,`H>,tqual_t,type_t)@`H";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY37={Cyc_Core_Failure,{_tmp256,
_tmp256,_tmp256 + 37}};struct _tuple8*Cyc_yyget_YY37(union Cyc_YYSTYPE yy1){struct
_tuple8*yyzzz;{union Cyc_YYSTYPE _tmp257=yy1;struct _tuple8*_tmp258;_LL1AA: if((
_tmp257.YY37).tag != 42)goto _LL1AC;_tmp258=(struct _tuple8*)(_tmp257.YY37).val;
_LL1AB: yyzzz=_tmp258;goto _LL1A9;_LL1AC:;_LL1AD:(int)_throw((void*)& Cyc_yyfail_YY37);
_LL1A9:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY37(struct _tuple8*yy1){return({union
Cyc_YYSTYPE _tmp259;(_tmp259.YY37).val=yy1;(_tmp259.YY37).tag=42;_tmp259;});}
static char _tmp25B[48]="list_t<$(opt_t<var_t,`H>,tqual_t,type_t)@`H,`H>";static
struct Cyc_Core_Failure_struct Cyc_yyfail_YY38={Cyc_Core_Failure,{_tmp25B,_tmp25B,
_tmp25B + 48}};struct Cyc_List_List*Cyc_yyget_YY38(union Cyc_YYSTYPE yy1){struct Cyc_List_List*
yyzzz;{union Cyc_YYSTYPE _tmp25C=yy1;struct Cyc_List_List*_tmp25D;_LL1AF: if((
_tmp25C.YY38).tag != 43)goto _LL1B1;_tmp25D=(struct Cyc_List_List*)(_tmp25C.YY38).val;
_LL1B0: yyzzz=_tmp25D;goto _LL1AE;_LL1B1:;_LL1B2:(int)_throw((void*)& Cyc_yyfail_YY38);
_LL1AE:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY38(struct Cyc_List_List*yy1){return({
union Cyc_YYSTYPE _tmp25E;(_tmp25E.YY38).val=yy1;(_tmp25E.YY38).tag=43;_tmp25E;});}
static char _tmp260[127]="$(list_t<$(opt_t<var_t,`H>,tqual_t,type_t)@`H,`H>, bool,vararg_info_t *`H,opt_t<type_t,`H>, list_t<$(type_t,type_t)@`H,`H>)@`H";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY39={Cyc_Core_Failure,{_tmp260,
_tmp260,_tmp260 + 127}};struct _tuple23*Cyc_yyget_YY39(union Cyc_YYSTYPE yy1){struct
_tuple23*yyzzz;{union Cyc_YYSTYPE _tmp261=yy1;struct _tuple23*_tmp262;_LL1B4: if((
_tmp261.YY39).tag != 44)goto _LL1B6;_tmp262=(struct _tuple23*)(_tmp261.YY39).val;
_LL1B5: yyzzz=_tmp262;goto _LL1B3;_LL1B6:;_LL1B7:(int)_throw((void*)& Cyc_yyfail_YY39);
_LL1B3:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY39(struct _tuple23*yy1){return({union
Cyc_YYSTYPE _tmp263;(_tmp263.YY39).val=yy1;(_tmp263.YY39).tag=44;_tmp263;});}
static char _tmp265[18]="list_t<type_t,`H>";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY40={
Cyc_Core_Failure,{_tmp265,_tmp265,_tmp265 + 18}};struct Cyc_List_List*Cyc_yyget_YY40(
union Cyc_YYSTYPE yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp266=yy1;
struct Cyc_List_List*_tmp267;_LL1B9: if((_tmp266.YY40).tag != 45)goto _LL1BB;_tmp267=(
struct Cyc_List_List*)(_tmp266.YY40).val;_LL1BA: yyzzz=_tmp267;goto _LL1B8;_LL1BB:;
_LL1BC:(int)_throw((void*)& Cyc_yyfail_YY40);_LL1B8:;}return yyzzz;}union Cyc_YYSTYPE
Cyc_YY40(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp268;(_tmp268.YY40).val=
yy1;(_tmp268.YY40).tag=45;_tmp268;});}static char _tmp26A[24]="list_t<designator_t,`H>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY41={Cyc_Core_Failure,{_tmp26A,
_tmp26A,_tmp26A + 24}};struct Cyc_List_List*Cyc_yyget_YY41(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp26B=yy1;struct Cyc_List_List*
_tmp26C;_LL1BE: if((_tmp26B.YY41).tag != 46)goto _LL1C0;_tmp26C=(struct Cyc_List_List*)(
_tmp26B.YY41).val;_LL1BF: yyzzz=_tmp26C;goto _LL1BD;_LL1C0:;_LL1C1:(int)_throw((
void*)& Cyc_yyfail_YY41);_LL1BD:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY41(struct Cyc_List_List*
yy1){return({union Cyc_YYSTYPE _tmp26D;(_tmp26D.YY41).val=yy1;(_tmp26D.YY41).tag=
46;_tmp26D;});}static char _tmp26F[13]="designator_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY42={Cyc_Core_Failure,{_tmp26F,_tmp26F,_tmp26F + 13}};void*Cyc_yyget_YY42(
union Cyc_YYSTYPE yy1){void*yyzzz;{union Cyc_YYSTYPE _tmp270=yy1;void*_tmp271;_LL1C3:
if((_tmp270.YY42).tag != 47)goto _LL1C5;_tmp271=(void*)(_tmp270.YY42).val;_LL1C4:
yyzzz=_tmp271;goto _LL1C2;_LL1C5:;_LL1C6:(int)_throw((void*)& Cyc_yyfail_YY42);
_LL1C2:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY42(void*yy1){return({union Cyc_YYSTYPE
_tmp272;(_tmp272.YY42).val=yy1;(_tmp272.YY42).tag=47;_tmp272;});}static char
_tmp274[7]="kind_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY43={Cyc_Core_Failure,{
_tmp274,_tmp274,_tmp274 + 7}};void*Cyc_yyget_YY43(union Cyc_YYSTYPE yy1){void*yyzzz;{
union Cyc_YYSTYPE _tmp275=yy1;void*_tmp276;_LL1C8: if((_tmp275.YY43).tag != 48)goto
_LL1CA;_tmp276=(void*)(_tmp275.YY43).val;_LL1C9: yyzzz=_tmp276;goto _LL1C7;_LL1CA:;
_LL1CB:(int)_throw((void*)& Cyc_yyfail_YY43);_LL1C7:;}return yyzzz;}union Cyc_YYSTYPE
Cyc_YY43(void*yy1){return({union Cyc_YYSTYPE _tmp277;(_tmp277.YY43).val=yy1;(
_tmp277.YY43).tag=48;_tmp277;});}static char _tmp279[7]="type_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY44={Cyc_Core_Failure,{_tmp279,_tmp279,_tmp279 + 7}};void*Cyc_yyget_YY44(
union Cyc_YYSTYPE yy1){void*yyzzz;{union Cyc_YYSTYPE _tmp27A=yy1;void*_tmp27B;_LL1CD:
if((_tmp27A.YY44).tag != 49)goto _LL1CF;_tmp27B=(void*)(_tmp27A.YY44).val;_LL1CE:
yyzzz=_tmp27B;goto _LL1CC;_LL1CF:;_LL1D0:(int)_throw((void*)& Cyc_yyfail_YY44);
_LL1CC:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY44(void*yy1){return({union Cyc_YYSTYPE
_tmp27C;(_tmp27C.YY44).val=yy1;(_tmp27C.YY44).tag=49;_tmp27C;});}static char
_tmp27E[23]="list_t<attribute_t,`H>";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY45={
Cyc_Core_Failure,{_tmp27E,_tmp27E,_tmp27E + 23}};struct Cyc_List_List*Cyc_yyget_YY45(
union Cyc_YYSTYPE yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp27F=yy1;
struct Cyc_List_List*_tmp280;_LL1D2: if((_tmp27F.YY45).tag != 50)goto _LL1D4;_tmp280=(
struct Cyc_List_List*)(_tmp27F.YY45).val;_LL1D3: yyzzz=_tmp280;goto _LL1D1;_LL1D4:;
_LL1D5:(int)_throw((void*)& Cyc_yyfail_YY45);_LL1D1:;}return yyzzz;}union Cyc_YYSTYPE
Cyc_YY45(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp281;(_tmp281.YY45).val=
yy1;(_tmp281.YY45).tag=50;_tmp281;});}static char _tmp283[12]="attribute_t";static
struct Cyc_Core_Failure_struct Cyc_yyfail_YY46={Cyc_Core_Failure,{_tmp283,_tmp283,
_tmp283 + 12}};void*Cyc_yyget_YY46(union Cyc_YYSTYPE yy1){void*yyzzz;{union Cyc_YYSTYPE
_tmp284=yy1;void*_tmp285;_LL1D7: if((_tmp284.YY46).tag != 51)goto _LL1D9;_tmp285=(
void*)(_tmp284.YY46).val;_LL1D8: yyzzz=_tmp285;goto _LL1D6;_LL1D9:;_LL1DA:(int)
_throw((void*)& Cyc_yyfail_YY46);_LL1D6:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY46(
void*yy1){return({union Cyc_YYSTYPE _tmp286;(_tmp286.YY46).val=yy1;(_tmp286.YY46).tag=
51;_tmp286;});}static char _tmp288[12]="enumfield_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY47={Cyc_Core_Failure,{_tmp288,_tmp288,_tmp288 + 12}};struct Cyc_Absyn_Enumfield*
Cyc_yyget_YY47(union Cyc_YYSTYPE yy1){struct Cyc_Absyn_Enumfield*yyzzz;{union Cyc_YYSTYPE
_tmp289=yy1;struct Cyc_Absyn_Enumfield*_tmp28A;_LL1DC: if((_tmp289.YY47).tag != 52)
goto _LL1DE;_tmp28A=(struct Cyc_Absyn_Enumfield*)(_tmp289.YY47).val;_LL1DD: yyzzz=
_tmp28A;goto _LL1DB;_LL1DE:;_LL1DF:(int)_throw((void*)& Cyc_yyfail_YY47);_LL1DB:;}
return yyzzz;}union Cyc_YYSTYPE Cyc_YY47(struct Cyc_Absyn_Enumfield*yy1){return({
union Cyc_YYSTYPE _tmp28B;(_tmp28B.YY47).val=yy1;(_tmp28B.YY47).tag=52;_tmp28B;});}
static char _tmp28D[23]="list_t<enumfield_t,`H>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY48={Cyc_Core_Failure,{_tmp28D,_tmp28D,_tmp28D + 23}};struct Cyc_List_List*
Cyc_yyget_YY48(union Cyc_YYSTYPE yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE
_tmp28E=yy1;struct Cyc_List_List*_tmp28F;_LL1E1: if((_tmp28E.YY48).tag != 53)goto
_LL1E3;_tmp28F=(struct Cyc_List_List*)(_tmp28E.YY48).val;_LL1E2: yyzzz=_tmp28F;
goto _LL1E0;_LL1E3:;_LL1E4:(int)_throw((void*)& Cyc_yyfail_YY48);_LL1E0:;}return
yyzzz;}union Cyc_YYSTYPE Cyc_YY48(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE
_tmp290;(_tmp290.YY48).val=yy1;(_tmp290.YY48).tag=53;_tmp290;});}static char
_tmp292[17]="opt_t<type_t,`H>";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY49={
Cyc_Core_Failure,{_tmp292,_tmp292,_tmp292 + 17}};struct Cyc_Core_Opt*Cyc_yyget_YY49(
union Cyc_YYSTYPE yy1){struct Cyc_Core_Opt*yyzzz;{union Cyc_YYSTYPE _tmp293=yy1;
struct Cyc_Core_Opt*_tmp294;_LL1E6: if((_tmp293.YY49).tag != 54)goto _LL1E8;_tmp294=(
struct Cyc_Core_Opt*)(_tmp293.YY49).val;_LL1E7: yyzzz=_tmp294;goto _LL1E5;_LL1E8:;
_LL1E9:(int)_throw((void*)& Cyc_yyfail_YY49);_LL1E5:;}return yyzzz;}union Cyc_YYSTYPE
Cyc_YY49(struct Cyc_Core_Opt*yy1){return({union Cyc_YYSTYPE _tmp295;(_tmp295.YY49).val=
yy1;(_tmp295.YY49).tag=54;_tmp295;});}static char _tmp297[31]="list_t<$(type_t,type_t)@`H,`H>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY50={Cyc_Core_Failure,{_tmp297,
_tmp297,_tmp297 + 31}};struct Cyc_List_List*Cyc_yyget_YY50(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp298=yy1;struct Cyc_List_List*
_tmp299;_LL1EB: if((_tmp298.YY50).tag != 55)goto _LL1ED;_tmp299=(struct Cyc_List_List*)(
_tmp298.YY50).val;_LL1EC: yyzzz=_tmp299;goto _LL1EA;_LL1ED:;_LL1EE:(int)_throw((
void*)& Cyc_yyfail_YY50);_LL1EA:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY50(struct Cyc_List_List*
yy1){return({union Cyc_YYSTYPE _tmp29A;(_tmp29A.YY50).val=yy1;(_tmp29A.YY50).tag=
55;_tmp29A;});}static char _tmp29C[15]="conref_t<bool>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY51={Cyc_Core_Failure,{_tmp29C,_tmp29C,_tmp29C + 15}};union Cyc_Absyn_Constraint*
Cyc_yyget_YY51(union Cyc_YYSTYPE yy1){union Cyc_Absyn_Constraint*yyzzz;{union Cyc_YYSTYPE
_tmp29D=yy1;union Cyc_Absyn_Constraint*_tmp29E;_LL1F0: if((_tmp29D.YY51).tag != 56)
goto _LL1F2;_tmp29E=(union Cyc_Absyn_Constraint*)(_tmp29D.YY51).val;_LL1F1: yyzzz=
_tmp29E;goto _LL1EF;_LL1F2:;_LL1F3:(int)_throw((void*)& Cyc_yyfail_YY51);_LL1EF:;}
return yyzzz;}union Cyc_YYSTYPE Cyc_YY51(union Cyc_Absyn_Constraint*yy1){return({
union Cyc_YYSTYPE _tmp29F;(_tmp29F.YY51).val=yy1;(_tmp29F.YY51).tag=56;_tmp29F;});}
static char _tmp2A1[45]="list_t<$(Position::seg_t,qvar_t,bool)@`H,`H>";static
struct Cyc_Core_Failure_struct Cyc_yyfail_YY52={Cyc_Core_Failure,{_tmp2A1,_tmp2A1,
_tmp2A1 + 45}};struct Cyc_List_List*Cyc_yyget_YY52(union Cyc_YYSTYPE yy1){struct Cyc_List_List*
yyzzz;{union Cyc_YYSTYPE _tmp2A2=yy1;struct Cyc_List_List*_tmp2A3;_LL1F5: if((
_tmp2A2.YY52).tag != 57)goto _LL1F7;_tmp2A3=(struct Cyc_List_List*)(_tmp2A2.YY52).val;
_LL1F6: yyzzz=_tmp2A3;goto _LL1F4;_LL1F7:;_LL1F8:(int)_throw((void*)& Cyc_yyfail_YY52);
_LL1F4:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY52(struct Cyc_List_List*yy1){return({
union Cyc_YYSTYPE _tmp2A4;(_tmp2A4.YY52).val=yy1;(_tmp2A4.YY52).tag=57;_tmp2A4;});}
static char _tmp2A6[15]="pointer_qual_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY53={
Cyc_Core_Failure,{_tmp2A6,_tmp2A6,_tmp2A6 + 15}};void*Cyc_yyget_YY53(union Cyc_YYSTYPE
yy1){void*yyzzz;{union Cyc_YYSTYPE _tmp2A7=yy1;void*_tmp2A8;_LL1FA: if((_tmp2A7.YY53).tag
!= 58)goto _LL1FC;_tmp2A8=(void*)(_tmp2A7.YY53).val;_LL1FB: yyzzz=_tmp2A8;goto
_LL1F9;_LL1FC:;_LL1FD:(int)_throw((void*)& Cyc_yyfail_YY53);_LL1F9:;}return yyzzz;}
union Cyc_YYSTYPE Cyc_YY53(void*yy1){return({union Cyc_YYSTYPE _tmp2A9;(_tmp2A9.YY53).val=
yy1;(_tmp2A9.YY53).tag=58;_tmp2A9;});}static char _tmp2AB[16]="pointer_quals_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY54={Cyc_Core_Failure,{_tmp2AB,
_tmp2AB,_tmp2AB + 16}};struct Cyc_List_List*Cyc_yyget_YY54(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp2AC=yy1;struct Cyc_List_List*
_tmp2AD;_LL1FF: if((_tmp2AC.YY54).tag != 59)goto _LL201;_tmp2AD=(struct Cyc_List_List*)(
_tmp2AC.YY54).val;_LL200: yyzzz=_tmp2AD;goto _LL1FE;_LL201:;_LL202:(int)_throw((
void*)& Cyc_yyfail_YY54);_LL1FE:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY54(struct Cyc_List_List*
yy1){return({union Cyc_YYSTYPE _tmp2AE;(_tmp2AE.YY54).val=yy1;(_tmp2AE.YY54).tag=
59;_tmp2AE;});}struct Cyc_Yyltype{int timestamp;int first_line;int first_column;int
last_line;int last_column;};struct Cyc_Yyltype Cyc_yynewloc(){return({struct Cyc_Yyltype
_tmp2AF;_tmp2AF.timestamp=0;_tmp2AF.first_line=0;_tmp2AF.first_column=0;_tmp2AF.last_line=
0;_tmp2AF.last_column=0;_tmp2AF;});}struct Cyc_Yyltype Cyc_yylloc={0,0,0,0,0};
static short Cyc_yytranslate[372]={0,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
2,2,2,2,2,2,2,2,2,143,2,2,127,141,138,2,124,125,132,135,120,139,129,140,2,2,2,2,
2,2,2,2,2,2,128,117,122,121,123,134,133,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
2,2,2,2,2,2,130,2,131,137,126,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
2,2,118,136,119,142,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,
18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,
45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,
72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,96,97,98,
99,100,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116};static
char _tmp2B0[2]="$";static char _tmp2B1[6]="error";static char _tmp2B2[12]="$undefined.";
static char _tmp2B3[5]="AUTO";static char _tmp2B4[9]="REGISTER";static char _tmp2B5[7]="STATIC";
static char _tmp2B6[7]="EXTERN";static char _tmp2B7[8]="TYPEDEF";static char _tmp2B8[5]="VOID";
static char _tmp2B9[5]="CHAR";static char _tmp2BA[6]="SHORT";static char _tmp2BB[4]="INT";
static char _tmp2BC[5]="LONG";static char _tmp2BD[6]="FLOAT";static char _tmp2BE[7]="DOUBLE";
static char _tmp2BF[7]="SIGNED";static char _tmp2C0[9]="UNSIGNED";static char _tmp2C1[
6]="CONST";static char _tmp2C2[9]="VOLATILE";static char _tmp2C3[9]="RESTRICT";
static char _tmp2C4[7]="STRUCT";static char _tmp2C5[6]="UNION";static char _tmp2C6[5]="CASE";
static char _tmp2C7[8]="DEFAULT";static char _tmp2C8[7]="INLINE";static char _tmp2C9[7]="SIZEOF";
static char _tmp2CA[9]="OFFSETOF";static char _tmp2CB[3]="IF";static char _tmp2CC[5]="ELSE";
static char _tmp2CD[7]="SWITCH";static char _tmp2CE[6]="WHILE";static char _tmp2CF[3]="DO";
static char _tmp2D0[4]="FOR";static char _tmp2D1[5]="GOTO";static char _tmp2D2[9]="CONTINUE";
static char _tmp2D3[6]="BREAK";static char _tmp2D4[7]="RETURN";static char _tmp2D5[5]="ENUM";
static char _tmp2D6[8]="NULL_kw";static char _tmp2D7[4]="LET";static char _tmp2D8[6]="THROW";
static char _tmp2D9[4]="TRY";static char _tmp2DA[6]="CATCH";static char _tmp2DB[7]="EXPORT";
static char _tmp2DC[4]="NEW";static char _tmp2DD[9]="ABSTRACT";static char _tmp2DE[9]="FALLTHRU";
static char _tmp2DF[6]="USING";static char _tmp2E0[10]="NAMESPACE";static char _tmp2E1[
9]="DATATYPE";static char _tmp2E2[8]="XTUNION";static char _tmp2E3[7]="TUNION";
static char _tmp2E4[7]="MALLOC";static char _tmp2E5[8]="RMALLOC";static char _tmp2E6[7]="CALLOC";
static char _tmp2E7[8]="RCALLOC";static char _tmp2E8[5]="SWAP";static char _tmp2E9[9]="REGION_T";
static char _tmp2EA[6]="TAG_T";static char _tmp2EB[7]="REGION";static char _tmp2EC[5]="RNEW";
static char _tmp2ED[8]="REGIONS";static char _tmp2EE[13]="RESET_REGION";static char
_tmp2EF[4]="GEN";static char _tmp2F0[16]="NOZEROTERM_QUAL";static char _tmp2F1[14]="ZEROTERM_QUAL";
static char _tmp2F2[12]="REGION_QUAL";static char _tmp2F3[7]="PORTON";static char
_tmp2F4[8]="PORTOFF";static char _tmp2F5[12]="DYNREGION_T";static char _tmp2F6[6]="ALIAS";
static char _tmp2F7[8]="NUMELTS";static char _tmp2F8[8]="VALUEOF";static char _tmp2F9[
10]="VALUEOF_T";static char _tmp2FA[9]="TAGCHECK";static char _tmp2FB[13]="NUMELTS_QUAL";
static char _tmp2FC[10]="THIN_QUAL";static char _tmp2FD[9]="FAT_QUAL";static char
_tmp2FE[13]="NOTNULL_QUAL";static char _tmp2FF[14]="NULLABLE_QUAL";static char
_tmp300[12]="TAGGED_QUAL";static char _tmp301[16]="EXTENSIBLE_QUAL";static char
_tmp302[15]="RESETABLE_QUAL";static char _tmp303[7]="PTR_OP";static char _tmp304[7]="INC_OP";
static char _tmp305[7]="DEC_OP";static char _tmp306[8]="LEFT_OP";static char _tmp307[9]="RIGHT_OP";
static char _tmp308[6]="LE_OP";static char _tmp309[6]="GE_OP";static char _tmp30A[6]="EQ_OP";
static char _tmp30B[6]="NE_OP";static char _tmp30C[7]="AND_OP";static char _tmp30D[6]="OR_OP";
static char _tmp30E[11]="MUL_ASSIGN";static char _tmp30F[11]="DIV_ASSIGN";static char
_tmp310[11]="MOD_ASSIGN";static char _tmp311[11]="ADD_ASSIGN";static char _tmp312[11]="SUB_ASSIGN";
static char _tmp313[12]="LEFT_ASSIGN";static char _tmp314[13]="RIGHT_ASSIGN";static
char _tmp315[11]="AND_ASSIGN";static char _tmp316[11]="XOR_ASSIGN";static char
_tmp317[10]="OR_ASSIGN";static char _tmp318[9]="ELLIPSIS";static char _tmp319[11]="LEFT_RIGHT";
static char _tmp31A[12]="COLON_COLON";static char _tmp31B[11]="IDENTIFIER";static
char _tmp31C[17]="INTEGER_CONSTANT";static char _tmp31D[7]="STRING";static char
_tmp31E[19]="CHARACTER_CONSTANT";static char _tmp31F[18]="FLOATING_CONSTANT";
static char _tmp320[9]="TYPE_VAR";static char _tmp321[13]="TYPEDEF_NAME";static char
_tmp322[16]="QUAL_IDENTIFIER";static char _tmp323[18]="QUAL_TYPEDEF_NAME";static
char _tmp324[10]="ATTRIBUTE";static char _tmp325[4]="';'";static char _tmp326[4]="'{'";
static char _tmp327[4]="'}'";static char _tmp328[4]="','";static char _tmp329[4]="'='";
static char _tmp32A[4]="'<'";static char _tmp32B[4]="'>'";static char _tmp32C[4]="'('";
static char _tmp32D[4]="')'";static char _tmp32E[4]="'_'";static char _tmp32F[4]="'$'";
static char _tmp330[4]="':'";static char _tmp331[4]="'.'";static char _tmp332[4]="'['";
static char _tmp333[4]="']'";static char _tmp334[4]="'*'";static char _tmp335[4]="'@'";
static char _tmp336[4]="'?'";static char _tmp337[4]="'+'";static char _tmp338[4]="'|'";
static char _tmp339[4]="'^'";static char _tmp33A[4]="'&'";static char _tmp33B[4]="'-'";
static char _tmp33C[4]="'/'";static char _tmp33D[4]="'%'";static char _tmp33E[4]="'~'";
static char _tmp33F[4]="'!'";static char _tmp340[5]="prog";static char _tmp341[17]="translation_unit";
static char _tmp342[12]="export_list";static char _tmp343[19]="export_list_values";
static char _tmp344[21]="external_declaration";static char _tmp345[20]="function_definition";
static char _tmp346[21]="function_definition2";static char _tmp347[13]="using_action";
static char _tmp348[15]="unusing_action";static char _tmp349[17]="namespace_action";
static char _tmp34A[19]="unnamespace_action";static char _tmp34B[12]="declaration";
static char _tmp34C[19]="resetable_qual_opt";static char _tmp34D[17]="declaration_list";
static char _tmp34E[23]="declaration_specifiers";static char _tmp34F[24]="storage_class_specifier";
static char _tmp350[15]="attributes_opt";static char _tmp351[11]="attributes";static
char _tmp352[15]="attribute_list";static char _tmp353[10]="attribute";static char
_tmp354[15]="type_specifier";static char _tmp355[25]="type_specifier_notypedef";
static char _tmp356[5]="kind";static char _tmp357[15]="type_qualifier";static char
_tmp358[15]="enum_specifier";static char _tmp359[11]="enum_field";static char
_tmp35A[22]="enum_declaration_list";static char _tmp35B[26]="struct_or_union_specifier";
static char _tmp35C[16]="type_params_opt";static char _tmp35D[16]="struct_or_union";
static char _tmp35E[24]="struct_declaration_list";static char _tmp35F[25]="struct_declaration_list0";
static char _tmp360[21]="init_declarator_list";static char _tmp361[22]="init_declarator_list0";
static char _tmp362[16]="init_declarator";static char _tmp363[19]="struct_declaration";
static char _tmp364[25]="specifier_qualifier_list";static char _tmp365[35]="notypedef_specifier_qualifier_list";
static char _tmp366[23]="struct_declarator_list";static char _tmp367[24]="struct_declarator_list0";
static char _tmp368[18]="struct_declarator";static char _tmp369[19]="datatype_specifier";
static char _tmp36A[14]="qual_datatype";static char _tmp36B[19]="datatypefield_list";
static char _tmp36C[20]="datatypefield_scope";static char _tmp36D[14]="datatypefield";
static char _tmp36E[11]="declarator";static char _tmp36F[23]="declarator_withtypedef";
static char _tmp370[18]="direct_declarator";static char _tmp371[30]="direct_declarator_withtypedef";
static char _tmp372[8]="pointer";static char _tmp373[12]="one_pointer";static char
_tmp374[14]="pointer_quals";static char _tmp375[13]="pointer_qual";static char
_tmp376[23]="pointer_null_and_bound";static char _tmp377[14]="pointer_bound";
static char _tmp378[18]="zeroterm_qual_opt";static char _tmp379[12]="opt_rgn_opt";
static char _tmp37A[8]="rgn_opt";static char _tmp37B[11]="tqual_list";static char
_tmp37C[20]="parameter_type_list";static char _tmp37D[9]="type_var";static char
_tmp37E[16]="optional_effect";static char _tmp37F[19]="optional_rgn_order";static
char _tmp380[10]="rgn_order";static char _tmp381[16]="optional_inject";static char
_tmp382[11]="effect_set";static char _tmp383[14]="atomic_effect";static char _tmp384[
11]="region_set";static char _tmp385[15]="parameter_list";static char _tmp386[22]="parameter_declaration";
static char _tmp387[16]="identifier_list";static char _tmp388[17]="identifier_list0";
static char _tmp389[12]="initializer";static char _tmp38A[18]="array_initializer";
static char _tmp38B[17]="initializer_list";static char _tmp38C[12]="designation";
static char _tmp38D[16]="designator_list";static char _tmp38E[11]="designator";
static char _tmp38F[10]="type_name";static char _tmp390[14]="any_type_name";static
char _tmp391[15]="type_name_list";static char _tmp392[20]="abstract_declarator";
static char _tmp393[27]="direct_abstract_declarator";static char _tmp394[10]="statement";
static char _tmp395[13]="open_exp_opt";static char _tmp396[18]="labeled_statement";
static char _tmp397[21]="expression_statement";static char _tmp398[19]="compound_statement";
static char _tmp399[16]="block_item_list";static char _tmp39A[20]="selection_statement";
static char _tmp39B[15]="switch_clauses";static char _tmp39C[20]="iteration_statement";
static char _tmp39D[15]="jump_statement";static char _tmp39E[12]="exp_pattern";
static char _tmp39F[20]="conditional_pattern";static char _tmp3A0[19]="logical_or_pattern";
static char _tmp3A1[20]="logical_and_pattern";static char _tmp3A2[21]="inclusive_or_pattern";
static char _tmp3A3[21]="exclusive_or_pattern";static char _tmp3A4[12]="and_pattern";
static char _tmp3A5[17]="equality_pattern";static char _tmp3A6[19]="relational_pattern";
static char _tmp3A7[14]="shift_pattern";static char _tmp3A8[17]="additive_pattern";
static char _tmp3A9[23]="multiplicative_pattern";static char _tmp3AA[13]="cast_pattern";
static char _tmp3AB[14]="unary_pattern";static char _tmp3AC[16]="postfix_pattern";
static char _tmp3AD[16]="primary_pattern";static char _tmp3AE[8]="pattern";static
char _tmp3AF[19]="tuple_pattern_list";static char _tmp3B0[20]="tuple_pattern_list0";
static char _tmp3B1[14]="field_pattern";static char _tmp3B2[19]="field_pattern_list";
static char _tmp3B3[20]="field_pattern_list0";static char _tmp3B4[11]="expression";
static char _tmp3B5[22]="assignment_expression";static char _tmp3B6[20]="assignment_operator";
static char _tmp3B7[23]="conditional_expression";static char _tmp3B8[20]="constant_expression";
static char _tmp3B9[22]="logical_or_expression";static char _tmp3BA[23]="logical_and_expression";
static char _tmp3BB[24]="inclusive_or_expression";static char _tmp3BC[24]="exclusive_or_expression";
static char _tmp3BD[15]="and_expression";static char _tmp3BE[20]="equality_expression";
static char _tmp3BF[22]="relational_expression";static char _tmp3C0[17]="shift_expression";
static char _tmp3C1[20]="additive_expression";static char _tmp3C2[26]="multiplicative_expression";
static char _tmp3C3[16]="cast_expression";static char _tmp3C4[17]="unary_expression";
static char _tmp3C5[15]="unary_operator";static char _tmp3C6[19]="postfix_expression";
static char _tmp3C7[19]="primary_expression";static char _tmp3C8[25]="argument_expression_list";
static char _tmp3C9[26]="argument_expression_list0";static char _tmp3CA[9]="constant";
static char _tmp3CB[20]="qual_opt_identifier";static char _tmp3CC[17]="qual_opt_typedef";
static char _tmp3CD[18]="struct_union_name";static char _tmp3CE[11]="field_name";
static char _tmp3CF[12]="right_angle";static struct _dyneither_ptr Cyc_yytname[288]={{
_tmp2B0,_tmp2B0,_tmp2B0 + 2},{_tmp2B1,_tmp2B1,_tmp2B1 + 6},{_tmp2B2,_tmp2B2,
_tmp2B2 + 12},{_tmp2B3,_tmp2B3,_tmp2B3 + 5},{_tmp2B4,_tmp2B4,_tmp2B4 + 9},{_tmp2B5,
_tmp2B5,_tmp2B5 + 7},{_tmp2B6,_tmp2B6,_tmp2B6 + 7},{_tmp2B7,_tmp2B7,_tmp2B7 + 8},{
_tmp2B8,_tmp2B8,_tmp2B8 + 5},{_tmp2B9,_tmp2B9,_tmp2B9 + 5},{_tmp2BA,_tmp2BA,
_tmp2BA + 6},{_tmp2BB,_tmp2BB,_tmp2BB + 4},{_tmp2BC,_tmp2BC,_tmp2BC + 5},{_tmp2BD,
_tmp2BD,_tmp2BD + 6},{_tmp2BE,_tmp2BE,_tmp2BE + 7},{_tmp2BF,_tmp2BF,_tmp2BF + 7},{
_tmp2C0,_tmp2C0,_tmp2C0 + 9},{_tmp2C1,_tmp2C1,_tmp2C1 + 6},{_tmp2C2,_tmp2C2,
_tmp2C2 + 9},{_tmp2C3,_tmp2C3,_tmp2C3 + 9},{_tmp2C4,_tmp2C4,_tmp2C4 + 7},{_tmp2C5,
_tmp2C5,_tmp2C5 + 6},{_tmp2C6,_tmp2C6,_tmp2C6 + 5},{_tmp2C7,_tmp2C7,_tmp2C7 + 8},{
_tmp2C8,_tmp2C8,_tmp2C8 + 7},{_tmp2C9,_tmp2C9,_tmp2C9 + 7},{_tmp2CA,_tmp2CA,
_tmp2CA + 9},{_tmp2CB,_tmp2CB,_tmp2CB + 3},{_tmp2CC,_tmp2CC,_tmp2CC + 5},{_tmp2CD,
_tmp2CD,_tmp2CD + 7},{_tmp2CE,_tmp2CE,_tmp2CE + 6},{_tmp2CF,_tmp2CF,_tmp2CF + 3},{
_tmp2D0,_tmp2D0,_tmp2D0 + 4},{_tmp2D1,_tmp2D1,_tmp2D1 + 5},{_tmp2D2,_tmp2D2,
_tmp2D2 + 9},{_tmp2D3,_tmp2D3,_tmp2D3 + 6},{_tmp2D4,_tmp2D4,_tmp2D4 + 7},{_tmp2D5,
_tmp2D5,_tmp2D5 + 5},{_tmp2D6,_tmp2D6,_tmp2D6 + 8},{_tmp2D7,_tmp2D7,_tmp2D7 + 4},{
_tmp2D8,_tmp2D8,_tmp2D8 + 6},{_tmp2D9,_tmp2D9,_tmp2D9 + 4},{_tmp2DA,_tmp2DA,
_tmp2DA + 6},{_tmp2DB,_tmp2DB,_tmp2DB + 7},{_tmp2DC,_tmp2DC,_tmp2DC + 4},{_tmp2DD,
_tmp2DD,_tmp2DD + 9},{_tmp2DE,_tmp2DE,_tmp2DE + 9},{_tmp2DF,_tmp2DF,_tmp2DF + 6},{
_tmp2E0,_tmp2E0,_tmp2E0 + 10},{_tmp2E1,_tmp2E1,_tmp2E1 + 9},{_tmp2E2,_tmp2E2,
_tmp2E2 + 8},{_tmp2E3,_tmp2E3,_tmp2E3 + 7},{_tmp2E4,_tmp2E4,_tmp2E4 + 7},{_tmp2E5,
_tmp2E5,_tmp2E5 + 8},{_tmp2E6,_tmp2E6,_tmp2E6 + 7},{_tmp2E7,_tmp2E7,_tmp2E7 + 8},{
_tmp2E8,_tmp2E8,_tmp2E8 + 5},{_tmp2E9,_tmp2E9,_tmp2E9 + 9},{_tmp2EA,_tmp2EA,
_tmp2EA + 6},{_tmp2EB,_tmp2EB,_tmp2EB + 7},{_tmp2EC,_tmp2EC,_tmp2EC + 5},{_tmp2ED,
_tmp2ED,_tmp2ED + 8},{_tmp2EE,_tmp2EE,_tmp2EE + 13},{_tmp2EF,_tmp2EF,_tmp2EF + 4},{
_tmp2F0,_tmp2F0,_tmp2F0 + 16},{_tmp2F1,_tmp2F1,_tmp2F1 + 14},{_tmp2F2,_tmp2F2,
_tmp2F2 + 12},{_tmp2F3,_tmp2F3,_tmp2F3 + 7},{_tmp2F4,_tmp2F4,_tmp2F4 + 8},{_tmp2F5,
_tmp2F5,_tmp2F5 + 12},{_tmp2F6,_tmp2F6,_tmp2F6 + 6},{_tmp2F7,_tmp2F7,_tmp2F7 + 8},{
_tmp2F8,_tmp2F8,_tmp2F8 + 8},{_tmp2F9,_tmp2F9,_tmp2F9 + 10},{_tmp2FA,_tmp2FA,
_tmp2FA + 9},{_tmp2FB,_tmp2FB,_tmp2FB + 13},{_tmp2FC,_tmp2FC,_tmp2FC + 10},{_tmp2FD,
_tmp2FD,_tmp2FD + 9},{_tmp2FE,_tmp2FE,_tmp2FE + 13},{_tmp2FF,_tmp2FF,_tmp2FF + 14},{
_tmp300,_tmp300,_tmp300 + 12},{_tmp301,_tmp301,_tmp301 + 16},{_tmp302,_tmp302,
_tmp302 + 15},{_tmp303,_tmp303,_tmp303 + 7},{_tmp304,_tmp304,_tmp304 + 7},{_tmp305,
_tmp305,_tmp305 + 7},{_tmp306,_tmp306,_tmp306 + 8},{_tmp307,_tmp307,_tmp307 + 9},{
_tmp308,_tmp308,_tmp308 + 6},{_tmp309,_tmp309,_tmp309 + 6},{_tmp30A,_tmp30A,
_tmp30A + 6},{_tmp30B,_tmp30B,_tmp30B + 6},{_tmp30C,_tmp30C,_tmp30C + 7},{_tmp30D,
_tmp30D,_tmp30D + 6},{_tmp30E,_tmp30E,_tmp30E + 11},{_tmp30F,_tmp30F,_tmp30F + 11},{
_tmp310,_tmp310,_tmp310 + 11},{_tmp311,_tmp311,_tmp311 + 11},{_tmp312,_tmp312,
_tmp312 + 11},{_tmp313,_tmp313,_tmp313 + 12},{_tmp314,_tmp314,_tmp314 + 13},{
_tmp315,_tmp315,_tmp315 + 11},{_tmp316,_tmp316,_tmp316 + 11},{_tmp317,_tmp317,
_tmp317 + 10},{_tmp318,_tmp318,_tmp318 + 9},{_tmp319,_tmp319,_tmp319 + 11},{_tmp31A,
_tmp31A,_tmp31A + 12},{_tmp31B,_tmp31B,_tmp31B + 11},{_tmp31C,_tmp31C,_tmp31C + 17},{
_tmp31D,_tmp31D,_tmp31D + 7},{_tmp31E,_tmp31E,_tmp31E + 19},{_tmp31F,_tmp31F,
_tmp31F + 18},{_tmp320,_tmp320,_tmp320 + 9},{_tmp321,_tmp321,_tmp321 + 13},{_tmp322,
_tmp322,_tmp322 + 16},{_tmp323,_tmp323,_tmp323 + 18},{_tmp324,_tmp324,_tmp324 + 10},{
_tmp325,_tmp325,_tmp325 + 4},{_tmp326,_tmp326,_tmp326 + 4},{_tmp327,_tmp327,
_tmp327 + 4},{_tmp328,_tmp328,_tmp328 + 4},{_tmp329,_tmp329,_tmp329 + 4},{_tmp32A,
_tmp32A,_tmp32A + 4},{_tmp32B,_tmp32B,_tmp32B + 4},{_tmp32C,_tmp32C,_tmp32C + 4},{
_tmp32D,_tmp32D,_tmp32D + 4},{_tmp32E,_tmp32E,_tmp32E + 4},{_tmp32F,_tmp32F,
_tmp32F + 4},{_tmp330,_tmp330,_tmp330 + 4},{_tmp331,_tmp331,_tmp331 + 4},{_tmp332,
_tmp332,_tmp332 + 4},{_tmp333,_tmp333,_tmp333 + 4},{_tmp334,_tmp334,_tmp334 + 4},{
_tmp335,_tmp335,_tmp335 + 4},{_tmp336,_tmp336,_tmp336 + 4},{_tmp337,_tmp337,
_tmp337 + 4},{_tmp338,_tmp338,_tmp338 + 4},{_tmp339,_tmp339,_tmp339 + 4},{_tmp33A,
_tmp33A,_tmp33A + 4},{_tmp33B,_tmp33B,_tmp33B + 4},{_tmp33C,_tmp33C,_tmp33C + 4},{
_tmp33D,_tmp33D,_tmp33D + 4},{_tmp33E,_tmp33E,_tmp33E + 4},{_tmp33F,_tmp33F,
_tmp33F + 4},{_tmp340,_tmp340,_tmp340 + 5},{_tmp341,_tmp341,_tmp341 + 17},{_tmp342,
_tmp342,_tmp342 + 12},{_tmp343,_tmp343,_tmp343 + 19},{_tmp344,_tmp344,_tmp344 + 21},{
_tmp345,_tmp345,_tmp345 + 20},{_tmp346,_tmp346,_tmp346 + 21},{_tmp347,_tmp347,
_tmp347 + 13},{_tmp348,_tmp348,_tmp348 + 15},{_tmp349,_tmp349,_tmp349 + 17},{
_tmp34A,_tmp34A,_tmp34A + 19},{_tmp34B,_tmp34B,_tmp34B + 12},{_tmp34C,_tmp34C,
_tmp34C + 19},{_tmp34D,_tmp34D,_tmp34D + 17},{_tmp34E,_tmp34E,_tmp34E + 23},{
_tmp34F,_tmp34F,_tmp34F + 24},{_tmp350,_tmp350,_tmp350 + 15},{_tmp351,_tmp351,
_tmp351 + 11},{_tmp352,_tmp352,_tmp352 + 15},{_tmp353,_tmp353,_tmp353 + 10},{
_tmp354,_tmp354,_tmp354 + 15},{_tmp355,_tmp355,_tmp355 + 25},{_tmp356,_tmp356,
_tmp356 + 5},{_tmp357,_tmp357,_tmp357 + 15},{_tmp358,_tmp358,_tmp358 + 15},{_tmp359,
_tmp359,_tmp359 + 11},{_tmp35A,_tmp35A,_tmp35A + 22},{_tmp35B,_tmp35B,_tmp35B + 26},{
_tmp35C,_tmp35C,_tmp35C + 16},{_tmp35D,_tmp35D,_tmp35D + 16},{_tmp35E,_tmp35E,
_tmp35E + 24},{_tmp35F,_tmp35F,_tmp35F + 25},{_tmp360,_tmp360,_tmp360 + 21},{
_tmp361,_tmp361,_tmp361 + 22},{_tmp362,_tmp362,_tmp362 + 16},{_tmp363,_tmp363,
_tmp363 + 19},{_tmp364,_tmp364,_tmp364 + 25},{_tmp365,_tmp365,_tmp365 + 35},{
_tmp366,_tmp366,_tmp366 + 23},{_tmp367,_tmp367,_tmp367 + 24},{_tmp368,_tmp368,
_tmp368 + 18},{_tmp369,_tmp369,_tmp369 + 19},{_tmp36A,_tmp36A,_tmp36A + 14},{
_tmp36B,_tmp36B,_tmp36B + 19},{_tmp36C,_tmp36C,_tmp36C + 20},{_tmp36D,_tmp36D,
_tmp36D + 14},{_tmp36E,_tmp36E,_tmp36E + 11},{_tmp36F,_tmp36F,_tmp36F + 23},{
_tmp370,_tmp370,_tmp370 + 18},{_tmp371,_tmp371,_tmp371 + 30},{_tmp372,_tmp372,
_tmp372 + 8},{_tmp373,_tmp373,_tmp373 + 12},{_tmp374,_tmp374,_tmp374 + 14},{_tmp375,
_tmp375,_tmp375 + 13},{_tmp376,_tmp376,_tmp376 + 23},{_tmp377,_tmp377,_tmp377 + 14},{
_tmp378,_tmp378,_tmp378 + 18},{_tmp379,_tmp379,_tmp379 + 12},{_tmp37A,_tmp37A,
_tmp37A + 8},{_tmp37B,_tmp37B,_tmp37B + 11},{_tmp37C,_tmp37C,_tmp37C + 20},{_tmp37D,
_tmp37D,_tmp37D + 9},{_tmp37E,_tmp37E,_tmp37E + 16},{_tmp37F,_tmp37F,_tmp37F + 19},{
_tmp380,_tmp380,_tmp380 + 10},{_tmp381,_tmp381,_tmp381 + 16},{_tmp382,_tmp382,
_tmp382 + 11},{_tmp383,_tmp383,_tmp383 + 14},{_tmp384,_tmp384,_tmp384 + 11},{
_tmp385,_tmp385,_tmp385 + 15},{_tmp386,_tmp386,_tmp386 + 22},{_tmp387,_tmp387,
_tmp387 + 16},{_tmp388,_tmp388,_tmp388 + 17},{_tmp389,_tmp389,_tmp389 + 12},{
_tmp38A,_tmp38A,_tmp38A + 18},{_tmp38B,_tmp38B,_tmp38B + 17},{_tmp38C,_tmp38C,
_tmp38C + 12},{_tmp38D,_tmp38D,_tmp38D + 16},{_tmp38E,_tmp38E,_tmp38E + 11},{
_tmp38F,_tmp38F,_tmp38F + 10},{_tmp390,_tmp390,_tmp390 + 14},{_tmp391,_tmp391,
_tmp391 + 15},{_tmp392,_tmp392,_tmp392 + 20},{_tmp393,_tmp393,_tmp393 + 27},{
_tmp394,_tmp394,_tmp394 + 10},{_tmp395,_tmp395,_tmp395 + 13},{_tmp396,_tmp396,
_tmp396 + 18},{_tmp397,_tmp397,_tmp397 + 21},{_tmp398,_tmp398,_tmp398 + 19},{
_tmp399,_tmp399,_tmp399 + 16},{_tmp39A,_tmp39A,_tmp39A + 20},{_tmp39B,_tmp39B,
_tmp39B + 15},{_tmp39C,_tmp39C,_tmp39C + 20},{_tmp39D,_tmp39D,_tmp39D + 15},{
_tmp39E,_tmp39E,_tmp39E + 12},{_tmp39F,_tmp39F,_tmp39F + 20},{_tmp3A0,_tmp3A0,
_tmp3A0 + 19},{_tmp3A1,_tmp3A1,_tmp3A1 + 20},{_tmp3A2,_tmp3A2,_tmp3A2 + 21},{
_tmp3A3,_tmp3A3,_tmp3A3 + 21},{_tmp3A4,_tmp3A4,_tmp3A4 + 12},{_tmp3A5,_tmp3A5,
_tmp3A5 + 17},{_tmp3A6,_tmp3A6,_tmp3A6 + 19},{_tmp3A7,_tmp3A7,_tmp3A7 + 14},{
_tmp3A8,_tmp3A8,_tmp3A8 + 17},{_tmp3A9,_tmp3A9,_tmp3A9 + 23},{_tmp3AA,_tmp3AA,
_tmp3AA + 13},{_tmp3AB,_tmp3AB,_tmp3AB + 14},{_tmp3AC,_tmp3AC,_tmp3AC + 16},{
_tmp3AD,_tmp3AD,_tmp3AD + 16},{_tmp3AE,_tmp3AE,_tmp3AE + 8},{_tmp3AF,_tmp3AF,
_tmp3AF + 19},{_tmp3B0,_tmp3B0,_tmp3B0 + 20},{_tmp3B1,_tmp3B1,_tmp3B1 + 14},{
_tmp3B2,_tmp3B2,_tmp3B2 + 19},{_tmp3B3,_tmp3B3,_tmp3B3 + 20},{_tmp3B4,_tmp3B4,
_tmp3B4 + 11},{_tmp3B5,_tmp3B5,_tmp3B5 + 22},{_tmp3B6,_tmp3B6,_tmp3B6 + 20},{
_tmp3B7,_tmp3B7,_tmp3B7 + 23},{_tmp3B8,_tmp3B8,_tmp3B8 + 20},{_tmp3B9,_tmp3B9,
_tmp3B9 + 22},{_tmp3BA,_tmp3BA,_tmp3BA + 23},{_tmp3BB,_tmp3BB,_tmp3BB + 24},{
_tmp3BC,_tmp3BC,_tmp3BC + 24},{_tmp3BD,_tmp3BD,_tmp3BD + 15},{_tmp3BE,_tmp3BE,
_tmp3BE + 20},{_tmp3BF,_tmp3BF,_tmp3BF + 22},{_tmp3C0,_tmp3C0,_tmp3C0 + 17},{
_tmp3C1,_tmp3C1,_tmp3C1 + 20},{_tmp3C2,_tmp3C2,_tmp3C2 + 26},{_tmp3C3,_tmp3C3,
_tmp3C3 + 16},{_tmp3C4,_tmp3C4,_tmp3C4 + 17},{_tmp3C5,_tmp3C5,_tmp3C5 + 15},{
_tmp3C6,_tmp3C6,_tmp3C6 + 19},{_tmp3C7,_tmp3C7,_tmp3C7 + 19},{_tmp3C8,_tmp3C8,
_tmp3C8 + 25},{_tmp3C9,_tmp3C9,_tmp3C9 + 26},{_tmp3CA,_tmp3CA,_tmp3CA + 9},{_tmp3CB,
_tmp3CB,_tmp3CB + 20},{_tmp3CC,_tmp3CC,_tmp3CC + 17},{_tmp3CD,_tmp3CD,_tmp3CD + 18},{
_tmp3CE,_tmp3CE,_tmp3CE + 11},{_tmp3CF,_tmp3CF,_tmp3CF + 12}};static short Cyc_yyr1[
513]={0,144,145,145,145,145,145,145,145,145,145,145,146,146,147,147,148,148,148,
149,149,149,149,150,150,151,152,153,154,155,155,155,155,155,155,155,156,156,157,
157,158,158,158,158,158,158,158,158,158,158,159,159,159,159,159,159,159,160,160,
161,162,162,163,163,163,163,163,163,164,164,165,165,165,165,165,165,165,165,165,
165,165,165,165,165,165,165,165,165,165,165,165,165,166,167,167,167,168,168,168,
169,169,170,170,170,171,171,171,171,171,172,172,173,173,174,174,175,175,176,177,
177,178,178,179,180,180,180,180,180,180,181,181,181,181,181,181,182,183,183,184,
184,184,185,185,185,186,186,187,187,187,187,188,188,188,189,189,190,190,191,191,
192,192,192,192,192,192,192,192,192,192,193,193,193,193,193,193,193,193,193,193,
193,194,194,195,196,196,197,197,197,197,197,197,197,197,198,198,198,199,199,200,
200,200,201,201,201,202,202,202,203,203,204,204,204,204,205,205,206,206,207,207,
208,208,209,209,210,210,211,211,211,211,212,212,213,213,214,214,214,215,216,216,
217,217,218,218,218,218,219,219,219,219,220,221,221,222,222,223,223,224,224,224,
224,224,225,225,226,226,226,227,227,227,227,227,227,227,227,227,227,227,228,228,
228,228,228,228,228,229,229,230,231,231,232,232,233,233,233,233,233,233,234,234,
234,234,234,234,235,235,235,235,235,235,236,236,236,236,236,236,236,236,236,236,
236,236,236,236,237,237,237,237,237,237,237,237,238,239,239,240,240,241,241,242,
242,243,243,244,244,245,245,245,246,246,246,246,246,247,247,247,248,248,248,249,
249,249,249,250,250,251,251,251,251,251,251,251,252,253,254,254,254,254,254,254,
254,254,254,254,254,254,254,254,254,255,255,255,256,256,257,257,258,258,258,259,
259,260,260,261,261,261,262,262,262,262,262,262,262,262,262,262,262,263,263,263,
263,263,263,263,264,265,265,266,266,267,267,268,268,269,269,270,270,270,271,271,
271,271,271,272,272,272,273,273,273,274,274,274,274,275,275,276,276,276,276,276,
276,276,276,276,276,276,276,276,276,276,276,276,276,276,276,277,277,277,278,278,
278,278,278,278,278,278,278,278,279,279,279,279,279,279,279,279,279,280,281,281,
282,282,282,282,283,283,284,284,285,285,286,286,287,287};static short Cyc_yyr2[513]={
0,1,2,3,5,3,5,6,7,3,3,0,4,3,2,3,1,1,1,2,3,3,4,3,4,2,1,2,1,2,3,5,3,7,5,8,0,1,1,2,
1,2,1,2,1,2,1,2,1,2,1,1,1,1,2,1,1,0,1,6,1,3,1,1,4,4,4,8,1,2,1,1,3,1,1,1,1,1,1,1,
1,1,1,1,1,4,4,4,6,4,1,4,1,1,1,1,5,2,4,1,3,1,2,3,4,9,8,4,3,0,3,1,1,0,1,1,2,1,1,3,
1,3,3,1,2,1,2,1,2,1,2,1,2,1,2,1,1,3,1,2,3,7,4,6,1,2,1,2,3,3,0,1,1,2,5,1,2,1,2,1,
3,4,4,5,4,5,4,4,2,1,1,3,4,4,5,4,5,4,4,2,1,2,5,0,2,4,4,1,1,1,1,1,1,2,2,1,0,3,0,1,
1,0,1,1,0,1,1,0,2,3,5,5,7,1,3,0,2,0,2,3,5,0,1,1,3,2,3,4,1,1,3,1,3,2,1,2,1,1,3,1,
1,2,3,4,8,1,2,3,4,2,1,2,3,2,1,2,1,2,3,4,3,1,3,1,1,2,3,3,4,4,5,4,3,5,4,4,2,1,1,1,
1,1,1,5,0,5,3,1,2,2,3,1,2,1,2,1,2,5,7,7,5,8,6,0,3,4,5,6,7,5,7,6,7,7,8,7,8,8,9,6,
7,7,8,3,2,2,2,3,2,4,5,1,1,5,1,3,1,3,1,3,1,3,1,3,1,3,3,1,3,3,3,3,1,3,3,1,3,3,1,3,
3,3,1,4,1,2,2,4,2,6,6,1,1,1,3,1,1,3,4,4,5,4,2,2,2,4,4,4,1,3,1,1,3,1,2,1,3,1,1,3,
1,3,1,3,3,1,1,1,1,1,1,1,1,1,1,1,1,5,2,2,2,5,5,1,1,3,1,3,1,3,1,3,1,3,1,3,3,1,3,3,
3,3,1,3,3,1,3,3,1,3,3,3,1,4,1,2,2,2,2,2,2,4,2,6,6,5,4,6,9,11,4,6,6,4,1,1,1,1,4,3,
4,3,3,2,2,6,7,1,1,1,3,2,5,4,5,5,1,1,3,1,1,1,1,1,1,1,1,1,1,1,1,1,1};static short
Cyc_yydefact[1035]={0,18,50,51,52,53,55,70,73,74,75,76,77,78,79,80,93,94,95,111,
112,46,0,0,56,0,0,144,0,90,0,0,0,0,0,0,0,37,503,213,505,504,506,0,0,71,0,196,196,
195,1,0,16,0,0,17,0,0,40,48,42,68,44,81,82,0,83,201,36,155,0,180,183,84,159,109,
54,53,47,0,97,502,0,503,499,500,501,109,0,373,0,0,0,0,236,0,375,376,25,27,0,0,0,
0,0,0,0,0,145,0,0,0,0,0,0,0,193,194,2,0,0,0,0,0,29,0,117,118,120,41,49,43,45,113,
507,508,109,203,0,202,36,38,36,0,19,0,215,0,168,156,181,190,189,0,0,187,188,191,
192,204,183,0,69,0,54,101,0,99,0,503,383,0,0,0,0,0,0,0,0,0,0,0,0,109,0,0,0,0,0,
489,0,0,0,0,0,476,474,475,0,400,402,416,424,426,428,430,432,434,437,442,445,448,
452,0,454,477,488,487,0,384,382,32,0,0,109,0,0,0,127,123,125,254,256,0,0,9,10,0,
0,0,109,509,510,214,92,0,0,160,72,234,0,231,0,3,0,5,0,284,0,30,0,0,36,20,0,114,
115,0,108,109,0,0,0,0,0,0,0,0,0,0,0,0,503,287,289,0,295,291,0,293,277,278,279,0,
280,281,282,0,39,21,120,261,0,221,237,0,0,217,215,0,198,0,0,0,206,57,205,184,0,0,
102,98,0,0,377,0,0,397,0,0,0,0,250,393,398,0,395,0,462,0,418,452,0,419,420,0,0,0,
0,0,0,0,0,0,0,455,456,36,0,0,0,458,459,457,0,374,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,406,407,408,409,410,411,412,413,414,415,405,0,460,0,483,484,0,0,0,491,0,
109,390,391,0,388,0,238,0,0,0,0,257,229,0,128,133,129,131,124,126,215,0,263,255,
264,512,511,0,86,89,0,87,0,91,107,63,62,0,60,161,215,233,157,263,235,169,170,0,
85,197,26,0,28,0,0,0,0,119,121,240,239,22,104,116,0,0,0,135,136,138,0,109,0,142,
0,0,0,0,0,0,36,0,324,325,326,0,0,328,0,0,0,0,296,292,120,294,290,288,0,167,222,0,
0,0,228,216,223,164,0,0,0,217,166,200,199,162,198,0,0,207,58,110,0,103,423,100,
96,386,387,253,0,394,249,251,381,0,0,0,0,241,245,0,0,0,0,0,0,0,0,0,0,0,0,0,0,490,
497,0,496,401,425,0,427,429,431,433,435,436,440,441,438,439,443,444,446,447,449,
450,451,404,403,482,479,0,481,0,0,0,378,0,385,31,0,379,0,0,258,134,130,132,0,217,
0,198,0,265,0,215,0,276,260,0,0,109,0,0,0,127,0,109,0,215,0,179,158,232,4,6,0,34,
0,0,139,122,0,0,217,109,150,0,0,0,303,0,0,0,0,0,323,327,0,0,0,0,286,36,23,262,
215,0,225,0,0,218,0,165,221,209,163,186,185,207,182,0,7,0,252,396,399,461,0,0,
242,0,246,466,0,0,0,0,0,470,473,0,0,0,0,0,453,493,0,0,480,478,0,0,389,392,380,
259,230,272,0,266,267,198,0,0,217,198,0,88,0,217,0,0,0,59,61,0,171,0,0,217,0,198,
0,0,0,137,140,113,143,152,151,0,0,146,0,0,0,0,0,0,0,0,0,0,0,0,0,303,329,0,0,24,
217,0,226,224,0,217,0,208,0,8,0,0,0,243,247,0,0,0,0,421,422,465,482,481,495,0,
498,417,492,494,0,271,269,275,274,0,268,198,0,113,0,66,64,65,172,178,175,0,177,
173,198,0,33,0,141,153,150,150,297,303,0,0,0,0,0,0,331,332,334,336,338,340,342,
344,347,352,355,358,362,364,371,372,0,36,300,309,0,0,0,0,0,0,0,0,0,0,330,283,211,
227,219,210,215,13,0,0,464,463,0,248,467,0,0,472,471,485,0,273,270,35,0,0,176,
174,285,106,0,149,148,0,0,303,0,368,0,0,365,36,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,366,304,0,311,0,0,0,319,0,0,0,0,0,0,302,0,217,12,14,0,0,0,0,486,105,0,0,
298,299,0,0,0,0,303,305,335,0,337,339,341,343,345,346,350,351,348,349,353,354,
356,357,359,360,361,0,310,312,313,0,321,320,0,315,0,0,0,220,212,15,0,0,0,0,154,
301,367,0,363,306,0,36,314,322,316,317,0,244,468,0,67,0,0,333,303,307,318,0,370,
369,308,469,0,0,0};static short Cyc_yydefgoto[144]={1032,50,682,877,51,52,286,53,
458,54,460,55,56,142,57,58,526,228,444,445,229,61,243,230,63,165,166,64,162,65,
264,265,125,126,127,266,231,425,473,474,475,66,67,752,753,754,68,476,69,449,70,
71,159,160,72,116,522,138,315,679,607,73,608,516,670,508,512,513,420,308,251,93,
94,547,466,548,330,331,332,232,301,302,609,431,289,462,290,291,292,293,294,760,
295,296,838,839,840,841,842,843,844,845,846,847,848,849,850,851,852,853,333,410,
411,334,335,336,297,199,397,200,532,201,202,203,204,205,206,207,208,209,210,211,
212,213,214,215,564,565,216,217,75,878,244,435};static short Cyc_yypact[1035]={
2688,- -32768,- -32768,- -32768,- -32768,- 4,- -32768,- -32768,- -32768,- -32768,- -32768,
- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,
3306,146,728,- -32768,87,47,- -32768,21,44,80,90,101,147,166,682,252,- -32768,-
-32768,266,- -32768,- -32768,- -32768,297,562,294,335,375,375,- -32768,- -32768,2553,
- -32768,637,660,- -32768,409,934,3306,3306,3306,- -32768,3306,- -32768,- -32768,593,
- -32768,20,3226,189,95,194,999,- -32768,- -32768,353,386,434,- -32768,87,418,-
-32768,912,347,- -32768,- -32768,- -32768,353,5799,- -32768,433,444,912,447,446,449,
- -32768,160,- -32768,- -32768,3454,3454,2553,2553,3454,468,5799,558,- -32768,195,
465,972,443,195,3676,5799,- -32768,- -32768,- -32768,2553,2822,2553,2822,- 36,-
-32768,491,512,- -32768,3146,- -32768,- -32768,- -32768,- -32768,3676,- -32768,- -32768,
353,- -32768,87,- -32768,1579,- -32768,3226,934,- -32768,3454,3380,4299,- -32768,189,
- -32768,- -32768,- -32768,515,536,- -32768,- -32768,- -32768,- -32768,22,999,3454,-
-32768,2822,- -32768,548,570,592,87,30,- -32768,912,65,3316,5935,606,5799,1167,613,
624,633,638,643,353,656,658,664,5976,5976,- -32768,2138,668,6068,6068,6068,-
-32768,- -32768,- -32768,50,- -32768,- -32768,- 60,634,588,609,688,684,245,134,- 56,
103,- -32768,865,6068,127,- 33,- -32768,666,3686,689,- -32768,- -32768,726,5799,353,
3686,716,161,3676,3750,3676,695,- -32768,63,63,- -32768,- -32768,119,730,284,353,-
-32768,- -32768,- -32768,- -32768,98,724,- -32768,- -32768,750,330,- -32768,748,-
-32768,751,- -32768,760,764,769,- -32768,972,1047,3226,- -32768,772,3676,- -32768,
657,784,177,789,234,796,3819,802,825,792,817,4395,3819,226,815,814,- -32768,-
-32768,829,1720,1720,934,1720,- -32768,- -32768,- -32768,835,- -32768,- -32768,-
-32768,264,- -32768,- -32768,848,813,117,863,- -32768,23,847,845,400,850,794,846,
3454,5799,- -32768,866,- -32768,- -32768,117,862,87,- -32768,5799,864,- -32768,861,
870,- -32768,195,5799,912,225,- -32768,- -32768,- -32768,868,869,2138,- -32768,3676,-
-32768,- -32768,3915,- -32768,895,5799,5799,5799,5799,5799,871,5799,3676,707,2138,
- -32768,- -32768,1861,872,357,5799,- -32768,- -32768,- -32768,5799,- -32768,6068,5799,
6068,6068,6068,6068,6068,6068,6068,6068,6068,6068,6068,6068,6068,6068,6068,6068,
6068,5799,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,-
-32768,- -32768,- -32768,5799,- -32768,195,- -32768,- -32768,4488,195,5799,- -32768,
877,353,- -32768,- -32768,876,874,912,- -32768,407,3316,878,3454,- -32768,880,886,-
-32768,3750,3750,3750,- -32768,- -32768,2984,4581,200,- -32768,350,- -32768,- -32768,
23,- -32768,- -32768,3454,- -32768,899,- -32768,889,- -32768,891,883,893,- -32768,2902,
- -32768,504,282,- -32768,- -32768,- -32768,3676,- -32768,- -32768,- -32768,2553,-
-32768,2553,911,904,901,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,
762,5799,908,907,- -32768,900,530,353,87,915,5799,5799,916,925,5799,1015,2002,929,
- -32768,- -32768,- -32768,413,1007,- -32768,4677,5799,3819,2274,- -32768,- -32768,
3146,- -32768,- -32768,- -32768,3454,- -32768,- -32768,3676,930,240,- -32768,- -32768,
918,- -32768,23,931,3602,845,- -32768,- -32768,- -32768,- -32768,794,68,932,426,-
-32768,- -32768,2418,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,924,
- -32768,- -32768,- -32768,- -32768,3612,936,939,955,- -32768,- -32768,753,1047,364,
949,950,961,390,3676,391,957,281,959,969,5840,- -32768,- -32768,964,970,- -32768,
634,130,588,609,688,684,245,245,134,134,134,134,- 56,- 56,103,103,- -32768,- -32768,
- -32768,- -32768,- -32768,- -32768,- -32768,967,- -32768,97,3454,4203,- -32768,6135,-
-32768,- -32768,974,- -32768,91,983,- -32768,- -32768,- -32768,- -32768,973,845,978,
794,977,350,3454,3528,4770,- -32768,- -32768,63,990,353,715,989,98,3066,991,353,
3454,3380,4866,- -32768,504,- -32768,- -32768,- -32768,993,- -32768,1008,823,- -32768,
- -32768,657,5799,845,353,884,394,420,5799,875,427,996,4962,5055,498,- -32768,-
-32768,1005,1009,1003,477,- -32768,3226,- -32768,813,1012,3454,- -32768,1014,23,-
-32768,1001,- -32768,214,- -32768,- -32768,- -32768,- -32768,426,- -32768,1016,- -32768,
2553,- -32768,- -32768,- -32768,1017,307,1018,- -32768,4011,- -32768,- -32768,5799,
1116,5799,1167,1019,- -32768,- -32768,195,195,1017,1020,4203,- -32768,- -32768,5799,
5799,- -32768,- -32768,117,785,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,
1021,- -32768,- -32768,794,117,1022,845,794,1011,- -32768,5799,845,484,1023,1024,-
-32768,- -32768,1025,- -32768,117,1026,845,1028,794,1029,5799,1045,- -32768,- -32768,
3676,- -32768,- -32768,- -32768,1044,87,529,3819,1046,1041,1270,1039,1049,3819,5799,
5148,571,5241,600,5334,875,- -32768,1052,1053,- -32768,845,150,- -32768,- -32768,
1031,845,3676,- -32768,646,- -32768,1048,1050,5799,- -32768,- -32768,1047,505,1056,
1063,- -32768,895,- -32768,1051,1059,- -32768,788,- -32768,- -32768,- -32768,- -32768,
4203,- -32768,- -32768,- -32768,- -32768,1062,- -32768,794,602,3676,1064,- -32768,-
-32768,- -32768,- -32768,- -32768,- -32768,1069,- -32768,- -32768,794,516,- -32768,1076,
- -32768,1072,42,884,1160,875,1079,6109,1075,2274,6068,1073,- -32768,- 53,- -32768,
1108,1066,774,827,313,842,236,358,- -32768,- -32768,- -32768,- -32768,1111,6068,1861,
- -32768,- -32768,528,3819,541,5427,3819,555,5520,5613,621,1085,- -32768,- -32768,-
-32768,- -32768,1086,- -32768,1012,- -32768,1088,622,- -32768,- -32768,184,- -32768,-
-32768,3676,1183,- -32768,- -32768,- -32768,4107,- -32768,- -32768,- -32768,1091,1093,
- -32768,- -32768,- -32768,- -32768,3676,- -32768,- -32768,3819,1098,875,2138,- -32768,
3676,1099,- -32768,1438,6068,5799,6068,6068,6068,6068,6068,6068,6068,6068,6068,
6068,6068,6068,6068,6068,6068,6068,6068,5799,- -32768,- -32768,1106,- -32768,3819,
3819,559,- -32768,3819,3819,565,3819,572,5706,- -32768,23,845,- -32768,- -32768,558,
5799,1100,1103,- -32768,- -32768,1120,579,- -32768,- -32768,1110,1107,1113,6068,875,
- -32768,634,262,588,609,609,684,245,245,134,134,134,134,- 56,- 56,103,103,- -32768,
- -32768,- -32768,298,- -32768,- -32768,- -32768,3819,- -32768,- -32768,3819,- -32768,
3819,3819,590,- -32768,- -32768,- -32768,824,1109,3676,1115,- -32768,- -32768,1017,
325,- -32768,- -32768,5799,1438,- -32768,- -32768,- -32768,- -32768,3819,- -32768,-
-32768,1117,- -32768,1118,1119,- -32768,875,- -32768,- -32768,1121,- -32768,- -32768,-
-32768,- -32768,1231,1235,- -32768};static short Cyc_yypgoto[144]={- -32768,111,-
-32768,286,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- 62,- -32768,-
98,31,- -32768,- -32768,0,625,- -32768,36,- 152,1132,5,- -32768,- -32768,- 136,- -32768,
105,1214,- 695,- -32768,- -32768,- -32768,994,985,223,421,- -32768,- -32768,612,-
-32768,- -32768,122,- -32768,- -32768,72,- 225,1185,- 446,14,- -32768,1096,- -32768,-
-32768,1209,- 474,- -32768,- -32768,580,- 119,- 58,- 123,- 464,314,586,595,- 418,- 471,-
112,- 399,- 126,- -32768,- 242,- 174,- 581,- 317,- -32768,938,- 172,196,- 151,- 161,- 278,
276,- -32768,- -32768,- -32768,- 67,- 275,- -32768,- 594,- -32768,- -32768,- -32768,-
-32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,-
-32768,- -32768,- -32768,- -32768,- -32768,173,1037,- -32768,725,857,- -32768,247,632,
- -32768,- 169,- 291,- 155,- 351,- 325,- 361,903,- 354,- 344,- 263,- 337,- 335,- 5,429,521,
933,- -32768,- 365,- -32768,167,456,- 31,- 15,- 293,45};static short Cyc_yytable[6274]={
59,144,250,343,631,62,141,340,570,139,318,499,500,712,502,567,617,572,358,465,
309,59,344,307,448,549,62,306,573,574,262,631,323,366,135,536,60,590,537,668,911,
579,580,569,111,581,582,750,751,675,136,59,78,826,674,632,62,60,59,59,59,263,59,
62,62,62,141,62,59,148,430,257,405,62,367,299,135,423,287,380,298,912,560,381,
509,150,258,60,451,129,130,131,240,132,60,60,60,671,60,143,406,316,59,59,60,76,
588,62,62,665,591,575,576,577,578,442,112,893,632,59,59,59,59,798,62,62,62,62,59,
128,659,717,39,62,39,39,721,171,60,60,59,510,59,100,719,62,137,62,314,148,432,
612,172,531,99,60,60,60,60,143,531,- 147,118,59,60,543,101,544,62,419,364,288,612,
143,868,365,60,325,60,748,557,639,559,246,530,518,433,361,362,363,96,326,173,676,
38,468,95,102,434,60,298,41,38,434,432,443,432,103,398,41,399,400,401,235,236,
300,716,364,453,44,378,379,625,104,287,287,434,287,710,422,253,254,255,256,424,
382,453,505,903,437,433,268,433,383,384,429,625,511,805,96,364,402,809,38,434,
170,403,404,708,96,41,808,59,450,79,220,812,62,822,105,423,423,423,39,319,872,
820,549,224,436,418,477,438,757,225,434,59,59,350,59,106,62,62,143,62,287,233,
234,60,161,237,108,241,531,364,43,479,691,242,871,960,145,951,146,874,527,965,
288,288,147,288,507,60,60,427,60,47,48,49,415,428,305,300,374,375,198,891,249,96,
- 215,96,38,- 215,494,324,441,539,506,41,896,495,747,39,239,328,329,267,59,482,667,
501,483,62,528,700,400,401,376,377,249,1009,925,109,788,480,926,511,583,584,585,
875,504,364,697,364,96,250,549,288,38,1010,409,96,60,784,40,41,42,409,736,113,
919,920,662,364,402,447,795,796,440,701,404,428,736,241,783,626,1025,364,453,242,
110,422,422,422,653,1011,424,424,424,1030,616,241,1021,663,921,922,359,242,141,
453,429,711,16,17,18,453,624,787,630,454,421,419,426,171,455,74,511,59,114,59,
450,723,62,- 237,62,43,- 237,123,172,414,638,613,531,614,161,738,364,80,97,615,98,
562,998,364,477,788,59,267,692,927,725,62,115,60,724,60,96,928,929,74,59,741,538,
163,740,62,74,524,739,364,364,594,74,364,696,698,305,143,755,517,134,792,60,598,
492,74,364,671,59,656,678,143,364,62,167,168,60,97,800,364,793,773,164,419,756,
882,364,97,486,778,219,761,969,970,493,705,218,74,74,966,787,971,134,221,60,222,
74,247,633,223,634,788,972,973,74,74,74,74,96,238,932,96,643,359,597,978,979,968,
245,980,981,550,626,269,772,554,364,556,74,298,359,771,338,813,341,341,453,259,
814,453,511,616,601,568,767,355,356,364,74,43,341,341,341,167,364,627,97,628,97,
883,630,260,618,629,964,364,38,477,312,681,897,341,40,41,42,829,787,364,830,249,
592,477,933,471,477,974,975,976,977,313,364,59,728,908,38,935,62,320,38,249,40,
41,42,97,364,41,249,43,364,939,97,59,678,989,364,44,62,862,321,992,364,364,143,
47,48,49,994,60,454,38,664,19,20,1004,452,40,41,42,96,364,133,952,322,1022,1016,
74,865,60,892,364,249,364,452,369,730,368,484,646,647,339,249,650,961,654,962,
1024,345,944,949,249,364,950,660,74,81,370,252,346,749,135,341,947,38,119,120,
801,347,341,40,41,42,348,96,38,876,81,349,806,714,40,41,42,661,372,373,167,121,
122,311,351,471,352,818,407,472,97,957,353,47,48,49,360,782,287,341,412,341,341,
341,341,341,341,341,341,341,341,341,341,341,341,341,341,341,38,84,189,85,86,427,
82,41,731,732,733,428,371,47,48,49,1019,354,909,413,191,83,84,249,85,86,417,1023,
41,604,605,606,87,421,287,446,931,249,88,439,89,90,59,38,520,521,91,62,774,40,41,
42,92,456,97,38,457,97,689,690,447,40,41,42,43,459,428,463,47,48,49,461,471,288,
511,750,751,469,60,467,47,48,49,758,759,764,766,341,478,452,802,803,452,888,889,
489,59,915,916,481,74,62,74,917,918,135,485,385,982,983,984,96,487,452,923,924,
38,854,488,452,490,644,40,41,42,496,789,288,497,1017,364,525,60,471,434,287,81,
900,901,498,503,47,48,49,1008,386,387,388,389,390,391,392,393,394,395,261,507,
267,514,515,467,519,811,523,551,552,553,529,43,533,534,74,396,541,366,542,341,
824,563,535,596,555,566,561,97,593,602,595,249,600,82,603,619,620,622,859,861,59,
864,623,867,621,62,586,635,169,84,636,85,86,637,640,41,641,642,587,87,831,881,
645,563,267,88,858,89,90,648,38,288,649,91,651,655,60,41,657,92,124,97,669,666,
683,672,677,44,687,611,686,688,151,152,153,47,48,49,693,694,341,174,175,154,155,
156,157,158,38,452,695,699,198,702,81,41,176,703,706,707,177,709,715,452,39,44,
452,718,178,179,180,181,720,47,48,49,182,722,937,183,729,941,943,734,745,737,744,
184,185,762,186,249,768,777,341,769,563,770,305,563,187,188,775,781,704,934,341,
74,938,785,790,810,873,794,797,804,807,815,816,817,819,359,821,38,84,189,85,86,
967,823,41,825,827,832,342,833,856,857,869,870,190,894,879,191,880,886,985,958,
192,884,467,193,885,887,194,195,890,902,196,197,996,174,175,895,898,899,904,1000,
907,913,910,914,930,945,81,946,948,953,828,955,987,988,956,97,990,991,959,993,
178,179,180,181,986,963,1001,467,1002,1003,1005,183,1033,1006,1007,1018,1034,999,
134,184,185,1020,186,1027,1028,1029,248,1031,727,735,107,470,187,188,746,464,149,
317,117,780,779,997,743,416,906,776,1012,341,685,1013,540,1014,1015,599,571,38,
84,189,85,86,855,563,41,0,0,341,342,558,0,0,0,0,190,1026,0,191,834,835,0,0,192,0,
0,193,0,0,194,195,0,81,196,197,0,0,0,0,0,0,0,0,0,0,0,467,0,0,0,0,791,0,0,0,0,0,0,
0,0,467,0,0,799,341,0,341,341,341,341,341,341,341,341,341,341,341,341,341,341,
341,341,341,0,0,0,82,0,0,0,0,0,0,0,0,0,0,0,0,0,0,169,84,0,85,86,0,0,41,0,0,0,87,
0,0,0,341,0,836,0,89,90,0,0,0,0,91,0,0,837,134,0,92,195,0,0,196,197,0,0,0,0,0,0,
467,0,0,0,0,0,0,0,0,0,0,0,0,0,0,467,0,0,0,341,0,2,3,4,77,6,7,8,9,10,11,12,13,14,
15,16,17,18,19,20,758,759,21,174,175,270,0,271,272,273,274,275,276,277,278,22,81,
23,176,279,0,0,177,24,280,0,0,27,0,0,178,179,180,181,0,28,29,0,182,0,281,183,0,0,
0,0,0,32,33,184,185,34,186,0,0,0,0,0,35,36,37,467,187,188,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,282,84,189,85,86,39,40,41,42,43,283,140,- 303,0,0,0,0,190,0,
45,285,0,0,0,0,192,0,0,193,0,0,194,195,0,0,196,197,2,3,4,77,6,7,8,9,10,11,12,13,
14,15,16,17,18,19,20,0,0,21,174,175,270,0,271,272,273,274,275,276,277,278,22,81,
23,176,279,0,0,177,24,280,0,0,27,0,0,178,179,180,181,0,28,29,0,182,0,281,183,0,0,
0,0,0,32,33,184,185,34,186,0,0,0,0,0,35,36,37,0,187,188,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,282,84,189,85,86,39,40,41,42,43,283,140,284,0,0,0,0,190,0,45,
285,0,0,0,0,192,0,0,193,0,0,194,195,0,0,196,197,2,3,4,77,6,7,8,9,10,11,12,13,14,
15,16,17,18,19,20,0,0,21,174,175,270,0,271,272,273,274,275,276,277,278,22,81,23,
176,279,0,0,177,24,280,0,0,27,0,0,178,179,180,181,0,28,29,- 36,182,0,281,183,0,0,
0,0,0,32,33,184,185,34,186,0,0,0,0,0,35,36,37,0,187,188,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,282,84,189,85,86,39,40,41,42,43,283,140,0,0,0,0,0,190,0,45,285,
0,0,0,0,192,0,0,193,0,0,194,195,0,0,196,197,2,3,4,77,6,7,8,9,10,11,12,13,14,15,
16,17,18,19,20,0,0,21,174,175,270,0,271,272,273,274,275,276,277,278,22,81,23,176,
279,0,0,177,24,280,0,0,27,0,0,178,179,180,181,0,28,29,0,182,0,281,183,0,0,0,0,0,
32,33,184,185,34,186,0,0,0,0,0,35,36,37,0,187,188,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,282,84,189,85,86,39,40,41,42,43,283,140,0,0,0,0,0,190,0,45,285,0,0,0,
0,192,0,0,193,0,0,194,195,0,0,196,197,2,3,4,77,6,7,8,9,10,11,12,13,14,15,16,17,
18,19,20,0,0,21,174,175,0,0,0,0,0,0,0,0,0,0,22,81,23,176,0,0,0,177,24,0,0,0,27,0,
0,178,179,180,181,0,28,29,0,182,0,0,183,0,0,0,0,0,32,33,184,185,34,186,0,0,0,0,0,
35,36,37,0,187,188,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,189,85,86,39,
40,41,42,43,652,0,0,0,0,0,0,190,0,45,285,0,0,0,0,192,0,0,193,0,0,194,195,0,0,196,
197,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,174,175,0,0,0,0,0,0,0,0,0,0,22,
81,0,176,0,0,0,177,0,0,0,0,27,0,0,178,179,180,181,0,28,29,0,182,0,0,183,0,0,0,0,
0,32,0,184,185,34,186,0,0,0,0,0,35,36,0,0,187,188,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,38,84,189,85,86,39,40,41,42,43,0,357,0,0,0,0,0,190,0,45,285,0,0,0,0,
192,0,0,193,0,0,194,195,0,0,196,197,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,
174,175,0,0,0,0,0,0,0,0,0,0,22,81,0,176,0,0,0,177,0,0,0,0,27,0,0,178,179,180,181,
0,28,29,0,182,0,0,183,0,0,0,0,0,32,0,184,185,34,186,0,0,0,0,0,35,36,0,0,187,188,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,189,85,86,39,40,41,42,43,0,0,0,0,
0,0,0,190,0,45,285,0,0,0,0,192,0,0,193,0,0,194,195,0,0,196,197,- 11,1,0,2,3,4,5,6,
7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,0,23,0,
0,0,680,0,24,0,25,26,27,0,0,0,0,0,0,0,28,29,- 36,0,0,0,0,0,0,0,30,31,32,33,0,0,34,
0,0,0,0,0,0,35,36,37,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,0,0,0,0,
39,40,41,42,43,0,0,- 11,0,0,0,0,44,0,45,46,0,0,0,0,47,48,49,- 11,1,0,2,3,4,5,6,7,8,
9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,0,23,0,0,0,
0,0,24,0,25,26,27,0,0,0,0,0,0,0,28,29,- 36,0,0,0,0,0,0,0,30,31,32,33,0,0,34,0,0,0,
0,0,0,35,36,37,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,0,0,0,0,39,40,
41,42,43,0,0,- 11,0,0,0,0,44,0,45,46,0,0,0,0,47,48,49,- 11,1,0,2,3,4,5,6,7,8,9,10,
11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,0,23,0,0,0,0,0,
24,0,25,26,27,0,0,0,0,0,0,0,28,29,- 36,0,0,0,0,0,0,0,30,31,32,33,0,0,34,0,0,0,0,0,
0,35,36,37,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,0,0,0,0,39,40,41,
42,43,0,0,0,0,0,0,0,44,0,45,46,0,0,0,0,47,48,49,1,0,2,3,4,5,6,7,8,9,10,11,12,13,
14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,0,23,0,0,0,0,0,24,0,25,26,
27,0,0,0,0,0,0,0,28,29,- 36,0,0,0,0,0,0,0,30,31,32,33,0,0,34,0,0,0,0,0,0,35,36,37,
0,0,0,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,38,0,0,0,0,39,40,41,
42,43,22,0,- 11,0,0,0,0,44,0,45,46,0,27,0,0,47,48,49,0,0,28,29,0,0,0,0,0,0,0,0,0,
0,32,0,0,0,34,0,0,0,0,0,0,35,36,0,0,0,0,0,0,0,0,7,8,9,10,11,12,13,14,15,16,17,18,
19,20,303,0,0,38,0,0,0,0,39,40,41,42,43,305,0,22,0,0,0,0,447,0,45,46,0,0,428,27,
47,48,49,0,0,0,0,28,29,0,0,0,0,0,0,0,0,0,0,32,0,0,0,34,0,0,0,0,0,0,35,36,0,0,0,0,
0,0,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,303,0,0,0,0,0,0,0,39,40,0,42,43,
305,0,22,0,0,0,0,427,0,45,46,0,0,428,27,47,48,49,0,0,0,0,28,29,0,0,0,0,0,0,0,0,0,
0,32,0,0,0,34,0,0,0,0,0,0,35,36,0,2,3,4,77,6,7,8,9,10,11,12,13,14,15,16,17,18,19,
20,0,0,21,0,0,38,0,0,0,0,39,40,41,42,43,22,0,23,0,0,0,0,471,24,45,46,0,27,0,0,47,
48,49,0,0,28,29,- 36,0,0,0,0,0,0,0,0,0,32,33,0,0,34,0,0,0,0,0,0,35,36,37,2,3,4,77,
6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,39,40,0,42,43,22,
140,23,0,261,0,0,0,24,45,46,0,27,0,0,0,0,0,0,0,28,29,0,0,0,0,0,0,0,0,0,0,32,33,0,
0,34,0,0,0,0,0,0,35,36,37,2,3,4,77,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,
21,0,0,0,0,0,0,0,39,40,0,42,43,22,140,0,0,0,0,0,0,24,45,46,81,27,0,0,0,0,0,0,0,
28,29,0,0,0,0,0,0,0,0,0,0,32,0,0,0,34,0,0,0,0,0,0,35,36,7,8,9,10,11,12,13,14,15,
16,17,18,19,20,0,0,0,0,0,0,82,0,0,0,0,0,0,0,0,22,39,40,327,42,43,169,84,0,85,86,
0,27,41,0,45,46,87,0,0,28,29,0,88,0,89,90,0,328,329,0,91,32,0,0,0,34,92,0,0,0,0,
0,35,36,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,303,0,0,304,0,0,0,
22,39,40,0,42,43,305,0,0,0,0,0,27,0,0,45,46,0,0,0,28,29,0,0,226,0,0,0,0,0,0,0,32,
0,0,0,34,0,0,0,0,0,0,35,36,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,22,39,40,0,42,43,0,227,0,0,0,0,27,0,0,45,46,0,0,0,28,29,0,0,0,0,
0,0,0,0,0,0,32,0,0,0,34,0,0,0,0,0,0,35,36,7,8,9,10,11,12,13,14,15,16,17,18,19,20,
0,0,0,0,0,0,0,0,303,0,0,0,0,0,0,22,39,40,0,42,43,305,0,0,0,0,81,27,0,0,45,46,0,0,
0,28,29,0,0,0,0,0,0,0,0,0,0,32,0,0,0,34,0,0,0,0,0,0,35,36,7,8,9,10,11,12,13,14,
15,16,17,18,19,20,0,0,0,0,0,0,82,0,673,0,0,0,0,0,0,22,39,40,684,42,43,169,84,0,
85,86,81,27,41,0,45,46,87,0,0,28,29,0,88,0,89,90,0,328,329,0,91,32,0,0,0,34,92,0,
0,0,0,0,35,36,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,82,0,0,0,0,0,0,
0,0,22,39,40,408,42,43,169,84,0,85,86,0,27,41,0,45,46,87,0,0,28,29,0,88,0,89,90,
0,0,0,0,91,32,0,0,0,34,92,0,0,0,0,0,35,36,0,0,0,0,0,0,0,0,0,0,0,0,174,175,270,0,
271,272,273,274,275,276,277,278,0,81,0,176,279,0,39,177,0,280,43,0,0,0,0,178,179,
180,181,0,45,46,0,182,0,281,183,0,0,0,0,0,0,0,184,185,0,186,0,0,0,0,0,0,0,0,0,
187,188,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,282,84,189,85,86,0,0,41,0,0,
283,140,0,0,174,175,0,190,0,0,191,545,0,0,0,192,0,81,193,176,0,194,195,177,0,196,
197,0,0,0,0,178,179,180,181,0,0,0,0,182,0,0,183,0,0,0,0,0,0,0,184,185,0,186,0,0,
0,0,0,0,0,0,0,187,188,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,189,85,86,
0,0,41,0,0,0,342,546,0,174,175,0,190,0,0,191,0,328,329,0,192,0,81,193,176,0,194,
195,177,0,196,197,0,0,0,0,178,179,180,181,0,0,0,0,182,0,0,183,0,0,0,0,0,0,0,184,
185,0,186,0,0,0,0,0,0,0,0,0,187,188,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,
84,189,85,86,0,0,41,0,0,0,342,786,0,174,175,0,190,0,0,191,0,328,329,0,192,0,81,
193,176,0,194,195,177,0,196,197,0,0,0,0,178,179,180,181,0,0,0,0,182,0,0,183,0,0,
0,0,0,0,0,184,185,0,186,0,0,0,0,0,0,0,0,0,187,188,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,38,84,189,85,86,0,0,41,0,0,0,342,954,0,174,175,0,190,0,0,191,0,328,
329,0,192,0,81,193,176,0,194,195,177,0,196,197,0,0,0,0,178,179,180,181,0,0,0,0,
182,0,0,183,0,0,0,0,0,0,0,184,185,0,186,0,0,0,0,0,0,0,0,0,187,188,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,189,85,86,0,0,41,0,0,0,342,0,0,174,175,0,190,0,
0,191,0,328,329,0,192,0,81,193,176,0,194,195,177,0,196,197,0,0,0,0,178,179,180,
181,0,0,0,0,182,0,0,183,0,0,0,0,0,0,0,184,185,0,186,0,0,0,0,0,0,0,0,0,187,188,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,189,85,86,0,0,41,0,0,0,0,0,0,174,
175,0,190,0,0,191,0,0,0,310,192,0,81,193,176,0,194,195,177,0,196,197,0,0,0,0,178,
179,180,181,0,0,0,0,182,0,0,183,0,0,0,0,0,0,0,184,185,0,186,0,0,0,0,0,0,0,0,0,
187,188,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,189,85,86,0,0,41,0,0,491,
174,175,0,0,0,0,190,0,0,191,0,0,0,81,192,176,0,193,0,177,194,195,0,0,196,197,0,
178,179,180,181,0,0,0,0,182,0,0,183,0,0,0,0,0,0,0,184,185,0,186,0,0,0,0,0,0,0,0,
0,187,188,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,189,85,86,0,0,41,0,0,0,
174,175,0,0,0,0,190,589,0,191,0,0,0,81,192,176,0,193,0,177,194,195,0,0,196,197,0,
178,179,180,181,0,0,0,0,182,0,0,183,0,0,0,0,0,0,0,184,185,0,186,0,0,0,0,0,0,0,0,
0,187,188,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,189,85,86,0,0,41,0,0,0,
0,0,0,174,175,0,190,0,0,191,0,0,0,610,192,0,81,193,176,0,194,195,177,0,196,197,0,
0,0,0,178,179,180,181,0,0,0,0,182,0,0,183,0,0,0,0,0,0,0,184,185,0,186,0,0,0,0,0,
0,0,0,0,187,188,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,189,85,86,0,0,41,
0,0,0,174,175,0,0,0,0,190,658,0,191,0,0,0,81,192,176,0,193,0,177,194,195,0,0,196,
197,0,178,179,180,181,0,0,0,0,182,0,0,183,0,0,0,0,0,0,0,184,185,0,186,0,0,0,0,0,
0,0,0,0,187,188,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,189,85,86,0,0,41,
0,0,0,0,0,0,174,175,0,190,0,0,191,0,0,0,726,192,0,81,193,176,0,194,195,177,0,196,
197,0,0,0,0,178,179,180,181,0,0,0,0,182,0,0,183,0,0,0,0,0,0,0,184,185,0,186,0,0,
0,0,0,0,0,0,0,187,188,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,189,85,86,
0,0,41,0,0,0,0,0,0,174,175,0,190,0,0,191,0,0,0,742,192,0,81,193,176,0,194,195,
177,0,196,197,0,0,0,0,178,179,180,181,0,0,0,0,182,0,0,183,0,0,0,0,0,0,0,184,185,
0,186,0,0,0,0,0,0,0,0,0,187,188,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,
189,85,86,0,0,41,0,0,763,174,175,0,0,0,0,190,0,0,191,0,0,0,81,192,176,0,193,0,
177,194,195,0,0,196,197,0,178,179,180,181,0,0,0,0,182,0,0,183,0,0,0,0,0,0,0,184,
185,0,186,0,0,0,0,0,0,0,0,0,187,188,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,
84,189,85,86,0,0,41,0,0,765,174,175,0,0,0,0,190,0,0,191,0,0,0,81,192,176,0,193,0,
177,194,195,0,0,196,197,0,178,179,180,181,0,0,0,0,182,0,0,183,0,0,0,0,0,0,0,184,
185,0,186,0,0,0,0,0,0,0,0,0,187,188,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,
84,189,85,86,0,0,41,0,0,0,174,175,0,0,0,0,190,860,0,191,0,0,0,81,192,176,0,193,0,
177,194,195,0,0,196,197,0,178,179,180,181,0,0,0,0,182,0,0,183,0,0,0,0,0,0,0,184,
185,0,186,0,0,0,0,0,0,0,0,0,187,188,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,
84,189,85,86,0,0,41,0,0,0,174,175,0,0,0,0,190,863,0,191,0,0,0,81,192,176,0,193,0,
177,194,195,0,0,196,197,0,178,179,180,181,0,0,0,0,182,0,0,183,0,0,0,0,0,0,0,184,
185,0,186,0,0,0,0,0,0,0,0,0,187,188,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,
84,189,85,86,0,0,41,0,0,866,174,175,0,0,0,0,190,0,0,191,0,0,0,81,192,176,0,193,0,
177,194,195,0,0,196,197,0,178,179,180,181,0,0,0,0,182,0,0,183,0,0,0,0,0,0,0,184,
185,0,186,0,0,0,0,0,0,0,0,0,187,188,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,
84,189,85,86,0,0,41,0,0,0,174,175,0,0,0,0,190,936,0,191,0,0,0,81,192,176,0,193,0,
177,194,195,0,0,196,197,0,178,179,180,181,0,0,0,0,182,0,0,183,0,0,0,0,0,0,0,184,
185,0,186,0,0,0,0,0,0,0,0,0,187,188,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,
84,189,85,86,0,0,41,0,0,0,174,175,0,0,0,0,190,940,0,191,0,0,0,81,192,176,0,193,0,
177,194,195,0,0,196,197,0,178,179,180,181,0,0,0,0,182,0,0,183,0,0,0,0,0,0,0,184,
185,0,186,0,0,0,0,0,0,0,0,0,187,188,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,
84,189,85,86,0,0,41,0,0,0,174,175,0,0,0,0,190,942,0,191,0,0,0,81,192,176,0,193,0,
177,194,195,0,0,196,197,0,178,179,180,181,0,0,0,0,182,0,0,183,0,0,0,0,0,0,0,184,
185,0,186,0,0,0,0,0,0,0,0,0,187,188,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,
84,189,85,86,0,0,41,0,0,0,174,175,0,0,0,0,190,995,0,191,0,0,0,81,192,176,0,193,0,
177,194,195,0,0,196,197,0,178,179,180,181,0,0,0,0,182,0,0,183,0,0,174,175,0,0,0,
184,185,0,186,0,0,0,0,81,0,0,0,0,187,188,0,0,0,0,0,0,0,178,179,180,181,0,0,0,0,0,
0,0,183,0,0,38,84,189,85,86,184,185,41,186,0,0,0,0,0,0,0,0,190,187,188,191,0,0,0,
0,192,0,0,193,0,0,194,195,0,0,196,197,0,0,0,0,38,84,189,85,86,0,0,41,0,0,0,704,0,
174,175,0,0,190,0,0,191,0,0,0,0,192,81,0,193,0,0,194,195,0,0,196,197,0,0,0,178,
179,180,181,0,0,0,0,0,0,0,183,0,0,174,175,0,0,0,184,185,0,186,0,0,0,0,81,0,0,0,0,
187,188,0,0,0,0,0,0,0,178,179,180,181,0,0,0,0,0,0,0,183,0,0,38,84,189,85,86,184,
185,41,186,0,0,0,0,0,0,0,0,337,187,188,191,0,0,0,0,192,0,0,193,0,0,194,195,0,0,
196,197,0,0,0,0,38,84,189,85,86,0,0,41,0,0,174,175,0,0,0,0,0,354,0,0,191,0,0,81,
0,192,0,0,193,0,0,194,195,0,0,196,197,178,179,180,181,0,0,0,0,0,0,0,183,0,0,174,
175,0,0,0,184,185,0,186,0,0,0,0,81,0,0,0,0,187,188,0,0,0,0,0,0,0,178,179,180,181,
0,0,0,0,0,0,0,183,81,0,38,84,189,85,86,184,185,41,186,0,0,0,0,0,0,0,0,190,187,
188,191,0,0,0,0,192,0,0,193,0,0,194,195,0,0,196,197,0,0,0,0,38,84,189,85,86,0,0,
41,0,0,0,82,0,0,0,0,0,905,0,0,191,0,0,713,0,192,169,84,193,85,86,194,195,41,0,
196,197,87,0,0,0,0,0,88,0,89,90,0,0,0,0,91,0,0,0,0,0,92};static short Cyc_yycheck[
6274]={0,68,114,177,450,0,68,176,369,67,161,286,287,594,289,366,434,371,190,261,
146,21,177,146,249,342,21,146,372,373,128,477,168,93,65,328,0,402,329,510,93,378,
379,368,44,380,381,5,6,523,65,51,21,748,518,454,51,21,58,59,60,128,62,58,59,60,
128,62,68,69,231,107,105,68,134,142,107,229,140,135,142,134,357,139,61,71,122,51,
249,58,59,60,107,62,58,59,60,515,62,68,133,159,102,103,68,109,399,102,103,508,
403,374,375,376,377,17,44,812,517,119,120,121,122,704,119,120,121,122,128,57,495,
602,112,128,112,112,610,107,102,103,140,118,142,122,608,140,126,142,126,149,87,
429,122,322,107,119,120,121,122,128,329,119,51,163,128,337,122,339,163,227,120,
140,450,142,768,125,140,112,142,643,352,472,354,111,320,308,123,192,193,194,23,
126,87,125,107,262,23,117,135,163,262,114,107,135,87,107,87,117,213,114,83,84,85,
102,103,143,125,120,249,124,86,87,447,122,286,287,135,289,131,229,119,120,121,
122,229,132,267,120,832,120,123,136,123,140,141,231,471,305,722,82,120,124,726,
107,135,82,129,130,128,92,114,725,262,249,118,92,730,262,742,122,422,423,424,112,
163,125,740,594,118,234,119,267,237,648,124,135,286,287,183,289,124,286,287,262,
289,357,100,101,262,122,104,49,107,472,120,116,129,549,113,773,904,122,128,124,
778,315,910,286,287,130,289,107,286,287,124,289,132,133,134,224,130,117,260,88,
89,88,810,114,171,125,173,107,128,117,171,240,121,302,114,823,124,642,112,106,
129,130,133,357,124,119,288,127,357,318,83,84,85,122,123,146,964,135,106,690,269,
139,434,382,383,384,779,117,120,555,120,218,498,704,357,107,128,218,225,357,687,
113,114,115,225,624,106,88,89,501,120,124,124,700,701,125,129,130,130,638,107,
108,447,1011,120,450,113,124,422,423,424,487,128,422,423,424,1024,431,107,108,
501,122,123,190,113,501,471,427,593,17,18,19,477,447,690,449,120,228,510,230,107,
125,0,515,458,124,460,447,613,458,117,460,116,120,59,122,223,471,122,642,124,122,
627,120,22,23,130,25,125,947,120,471,803,487,265,125,132,614,487,118,458,614,460,
330,140,141,44,501,628,330,118,628,501,51,312,628,120,120,407,57,120,125,125,117,
487,125,120,65,696,487,117,278,70,120,946,529,117,526,501,120,529,79,118,501,82,
708,120,696,665,109,602,125,788,120,92,273,673,107,125,914,915,279,561,124,102,
103,911,803,916,107,117,529,120,111,125,458,121,460,889,917,918,119,120,121,122,
412,112,856,415,478,337,412,923,924,913,124,925,926,345,624,138,662,349,120,351,
143,662,354,125,174,120,176,177,638,117,125,641,669,612,417,367,117,187,188,120,
163,116,192,193,194,168,120,122,171,124,173,125,631,120,437,130,910,120,107,624,
124,529,125,213,113,114,115,117,889,120,120,427,404,638,125,124,641,919,920,921,
922,124,120,662,618,836,107,125,662,120,107,447,113,114,115,218,120,114,454,116,
120,125,225,682,678,125,120,124,682,117,119,125,120,120,662,132,133,134,125,662,
120,107,505,20,21,125,249,113,114,115,542,120,118,884,121,1007,125,260,117,682,
117,120,498,120,267,136,620,92,271,481,482,124,508,485,905,487,907,1011,124,117,
117,517,120,120,496,288,38,137,115,124,644,781,322,875,107,117,118,711,124,329,
113,114,115,124,596,107,119,38,124,723,596,113,114,115,497,90,91,320,117,118,147,
124,124,124,738,118,128,330,899,124,132,133,134,124,682,856,366,107,368,369,370,
371,372,373,374,375,376,377,378,379,380,381,382,383,384,107,108,109,110,111,124,
92,114,107,108,109,130,138,132,133,134,1002,124,837,107,127,107,108,614,110,111,
124,1010,114,422,423,424,118,624,910,125,855,628,124,123,126,127,856,107,64,65,
132,856,666,113,114,115,138,119,412,107,119,415,119,120,124,113,114,115,116,119,
130,112,132,133,134,121,124,856,946,5,6,119,856,261,132,133,134,22,23,652,653,
472,118,447,119,120,450,119,120,117,910,137,138,124,458,910,460,90,91,950,124,56,
927,928,929,758,124,471,86,87,107,758,107,477,117,479,113,114,115,124,693,910,
128,119,120,313,910,124,135,1011,38,829,830,124,119,132,133,134,963,94,95,96,97,
98,99,100,101,102,103,121,107,748,125,128,342,125,729,131,346,347,348,119,116,
119,123,529,121,119,93,120,561,744,360,123,120,124,364,125,542,122,120,125,779,
125,92,119,107,118,125,762,763,1011,765,120,767,124,1011,385,107,107,108,117,110,
111,123,117,114,120,128,397,118,755,785,118,402,812,124,761,126,127,124,107,1011,
118,132,30,117,1011,114,42,138,117,596,135,124,131,125,125,124,120,428,125,107,
64,65,66,132,133,134,120,120,642,25,26,75,76,77,78,79,107,624,120,125,836,125,38,
114,40,119,125,120,44,125,119,638,112,124,641,125,52,53,54,55,125,132,133,134,60,
131,862,63,121,865,866,125,107,125,124,71,72,124,74,899,118,123,696,117,495,125,
117,498,84,85,119,118,118,860,708,682,863,122,25,131,112,125,125,125,125,125,125,
125,125,905,125,107,108,109,110,111,912,131,114,117,119,118,118,125,128,119,117,
117,124,108,125,127,125,125,930,902,132,124,549,135,120,125,138,139,125,28,142,
143,944,25,26,125,119,124,118,951,124,92,128,136,92,119,38,120,119,25,753,119,
935,936,120,758,939,940,119,942,52,53,54,55,117,125,125,594,124,108,119,63,0,125,
120,125,0,950,781,71,72,125,74,125,125,125,113,125,615,623,35,265,84,85,641,260,
70,160,48,678,673,946,629,225,834,669,989,837,542,992,331,994,995,415,370,107,
108,109,110,111,758,648,114,- 1,- 1,855,118,353,- 1,- 1,- 1,- 1,124,1016,- 1,127,25,26,
- 1,- 1,132,- 1,- 1,135,- 1,- 1,138,139,- 1,38,142,143,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
690,- 1,- 1,- 1,- 1,695,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,704,- 1,- 1,707,911,- 1,913,914,915,916,
917,918,919,920,921,922,923,924,925,926,927,928,929,- 1,- 1,- 1,92,- 1,- 1,- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,- 1,110,111,- 1,- 1,114,- 1,- 1,- 1,118,- 1,- 1,- 1,963,
- 1,124,- 1,126,127,- 1,- 1,- 1,- 1,132,- 1,- 1,135,950,- 1,138,139,- 1,- 1,142,143,- 1,- 1,-
1,- 1,- 1,- 1,788,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,803,- 1,- 1,- 1,1010,- 1,3,
4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,- 1,29,30,31,32,
33,34,35,36,37,38,39,40,41,- 1,- 1,44,45,46,- 1,- 1,49,- 1,- 1,52,53,54,55,- 1,57,58,- 1,
60,- 1,62,63,- 1,- 1,- 1,- 1,- 1,69,70,71,72,73,74,- 1,- 1,- 1,- 1,- 1,80,81,82,889,84,85,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,
111,112,113,114,115,116,117,118,119,- 1,- 1,- 1,- 1,124,- 1,126,127,- 1,- 1,- 1,- 1,132,-
1,- 1,135,- 1,- 1,138,139,- 1,- 1,142,143,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,
20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,- 1,- 1,44,45,46,
- 1,- 1,49,- 1,- 1,52,53,54,55,- 1,57,58,- 1,60,- 1,62,63,- 1,- 1,- 1,- 1,- 1,69,70,71,72,73,
74,- 1,- 1,- 1,- 1,- 1,80,81,82,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,114,115,116,117,118,119,- 1,- 1,- 1,-
1,124,- 1,126,127,- 1,- 1,- 1,- 1,132,- 1,- 1,135,- 1,- 1,138,139,- 1,- 1,142,143,3,4,5,6,7,
8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,
35,36,37,38,39,40,41,- 1,- 1,44,45,46,- 1,- 1,49,- 1,- 1,52,53,54,55,- 1,57,58,59,60,- 1,
62,63,- 1,- 1,- 1,- 1,- 1,69,70,71,72,73,74,- 1,- 1,- 1,- 1,- 1,80,81,82,- 1,84,85,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,
113,114,115,116,117,118,- 1,- 1,- 1,- 1,- 1,124,- 1,126,127,- 1,- 1,- 1,- 1,132,- 1,- 1,135,
- 1,- 1,138,139,- 1,- 1,142,143,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,
- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,- 1,- 1,44,45,46,- 1,- 1,49,
- 1,- 1,52,53,54,55,- 1,57,58,- 1,60,- 1,62,63,- 1,- 1,- 1,- 1,- 1,69,70,71,72,73,74,- 1,- 1,
- 1,- 1,- 1,80,81,82,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,107,108,109,110,111,112,113,114,115,116,117,118,- 1,- 1,- 1,- 1,- 1,124,- 1,
126,127,- 1,- 1,- 1,- 1,132,- 1,- 1,135,- 1,- 1,138,139,- 1,- 1,142,143,3,4,5,6,7,8,9,10,
11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,
38,39,40,- 1,- 1,- 1,44,45,- 1,- 1,- 1,49,- 1,- 1,52,53,54,55,- 1,57,58,- 1,60,- 1,- 1,63,- 1,
- 1,- 1,- 1,- 1,69,70,71,72,73,74,- 1,- 1,- 1,- 1,- 1,80,81,82,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,114,115,
116,117,- 1,- 1,- 1,- 1,- 1,- 1,124,- 1,126,127,- 1,- 1,- 1,- 1,132,- 1,- 1,135,- 1,- 1,138,139,
- 1,- 1,142,143,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,37,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,49,- 1,- 1,52,53,54,55,- 1,57,
58,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,69,- 1,71,72,73,74,- 1,- 1,- 1,- 1,- 1,80,81,- 1,- 1,84,
85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,
110,111,112,113,114,115,116,- 1,118,- 1,- 1,- 1,- 1,- 1,124,- 1,126,127,- 1,- 1,- 1,- 1,132,
- 1,- 1,135,- 1,- 1,138,139,- 1,- 1,142,143,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,
- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,49,
- 1,- 1,52,53,54,55,- 1,57,58,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,69,- 1,71,72,73,74,- 1,- 1,
- 1,- 1,- 1,80,81,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,107,108,109,110,111,112,113,114,115,116,- 1,- 1,- 1,- 1,- 1,- 1,- 1,124,- 1,126,
127,- 1,- 1,- 1,- 1,132,- 1,- 1,135,- 1,- 1,138,139,- 1,- 1,142,143,0,1,- 1,3,4,5,6,7,8,9,
10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
37,- 1,39,- 1,- 1,- 1,43,- 1,45,- 1,47,48,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,57,58,59,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,67,68,69,70,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,80,81,82,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,- 1,- 1,- 1,- 1,112,113,114,115,
116,- 1,- 1,119,- 1,- 1,- 1,- 1,124,- 1,126,127,- 1,- 1,- 1,- 1,132,133,134,0,1,- 1,3,4,5,6,
7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,37,- 1,39,- 1,- 1,- 1,- 1,- 1,45,- 1,47,48,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,57,58,59,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,67,68,69,70,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,80,81,82,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,- 1,- 1,- 1,- 1,112,113,
114,115,116,- 1,- 1,119,- 1,- 1,- 1,- 1,124,- 1,126,127,- 1,- 1,- 1,- 1,132,133,134,0,1,- 1,
3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,37,- 1,39,- 1,- 1,- 1,- 1,- 1,45,- 1,47,48,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,57,58,
59,- 1,- 1,- 1,- 1,- 1,- 1,- 1,67,68,69,70,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,80,81,82,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,- 1,- 1,- 1,- 1,
112,113,114,115,116,- 1,- 1,- 1,- 1,- 1,- 1,- 1,124,- 1,126,127,- 1,- 1,- 1,- 1,132,133,134,
1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,39,- 1,- 1,- 1,- 1,- 1,45,- 1,47,48,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
57,58,59,- 1,- 1,- 1,- 1,- 1,- 1,- 1,67,68,69,70,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,80,81,82,- 1,
- 1,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,107,- 1,- 1,- 1,
- 1,112,113,114,115,116,37,- 1,119,- 1,- 1,- 1,- 1,124,- 1,126,127,- 1,49,- 1,- 1,132,133,
134,- 1,- 1,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,69,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,
80,81,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,104,- 1,- 1,
107,- 1,- 1,- 1,- 1,112,113,114,115,116,117,- 1,37,- 1,- 1,- 1,- 1,124,- 1,126,127,- 1,- 1,
130,49,132,133,134,- 1,- 1,- 1,- 1,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,69,- 1,- 1,- 1,
73,- 1,- 1,- 1,- 1,- 1,- 1,80,81,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,
18,19,20,21,104,- 1,- 1,- 1,- 1,- 1,- 1,- 1,112,113,- 1,115,116,117,- 1,37,- 1,- 1,- 1,- 1,
124,- 1,126,127,- 1,- 1,130,49,132,133,134,- 1,- 1,- 1,- 1,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,-
1,- 1,- 1,69,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,80,81,- 1,3,4,5,6,7,8,9,10,11,12,13,14,
15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,107,- 1,- 1,- 1,- 1,112,113,114,115,116,37,- 1,39,
- 1,- 1,- 1,- 1,124,45,126,127,- 1,49,- 1,- 1,132,133,134,- 1,- 1,57,58,59,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,69,70,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,80,81,82,3,4,5,6,7,8,9,10,11,12,13,
14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,112,113,- 1,115,116,37,118,
39,- 1,121,- 1,- 1,- 1,45,126,127,- 1,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,57,58,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,69,70,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,80,81,82,3,4,5,6,7,8,9,10,11,12,13,
14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,112,113,- 1,115,116,37,118,
- 1,- 1,- 1,- 1,- 1,- 1,45,126,127,38,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,57,58,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,69,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,80,81,8,9,10,11,12,13,14,15,16,17,
18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,92,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,112,113,104,115,116,
107,108,- 1,110,111,- 1,49,114,- 1,126,127,118,- 1,- 1,57,58,- 1,124,- 1,126,127,- 1,129,
130,- 1,132,69,- 1,- 1,- 1,73,138,- 1,- 1,- 1,- 1,- 1,80,81,8,9,10,11,12,13,14,15,16,17,
18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,104,- 1,- 1,107,- 1,- 1,- 1,37,112,113,- 1,115,116,
117,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,126,127,- 1,- 1,- 1,57,58,- 1,- 1,61,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
69,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,80,81,8,9,10,11,12,13,14,15,16,17,18,19,20,21,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,112,113,- 1,115,116,- 1,118,- 1,- 1,-
1,- 1,49,- 1,- 1,126,127,- 1,- 1,- 1,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,69,- 1,- 1,- 1,
73,- 1,- 1,- 1,- 1,- 1,- 1,80,81,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,-
1,- 1,- 1,- 1,104,- 1,- 1,- 1,- 1,- 1,- 1,37,112,113,- 1,115,116,117,- 1,- 1,- 1,- 1,38,49,- 1,
- 1,126,127,- 1,- 1,- 1,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,69,- 1,- 1,- 1,73,- 1,- 1,- 1,
- 1,- 1,- 1,80,81,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,92,- 1,
104,- 1,- 1,- 1,- 1,- 1,- 1,37,112,113,104,115,116,107,108,- 1,110,111,38,49,114,- 1,126,
127,118,- 1,- 1,57,58,- 1,124,- 1,126,127,- 1,129,130,- 1,132,69,- 1,- 1,- 1,73,138,- 1,- 1,
- 1,- 1,- 1,80,81,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,92,- 1,-
1,- 1,- 1,- 1,- 1,- 1,- 1,37,112,113,104,115,116,107,108,- 1,110,111,- 1,49,114,- 1,126,
127,118,- 1,- 1,57,58,- 1,124,- 1,126,127,- 1,- 1,- 1,- 1,132,69,- 1,- 1,- 1,73,138,- 1,- 1,-
1,- 1,- 1,80,81,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,25,26,27,- 1,29,30,31,32,33,34,
35,36,- 1,38,- 1,40,41,- 1,112,44,- 1,46,116,- 1,- 1,- 1,- 1,52,53,54,55,- 1,126,127,- 1,
60,- 1,62,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,
- 1,- 1,114,- 1,- 1,117,118,- 1,- 1,25,26,- 1,124,- 1,- 1,127,32,- 1,- 1,- 1,132,- 1,38,135,
40,- 1,138,139,44,- 1,142,143,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,-
1,- 1,- 1,- 1,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,- 1,- 1,114,- 1,- 1,
- 1,118,119,- 1,25,26,- 1,124,- 1,- 1,127,- 1,129,130,- 1,132,- 1,38,135,40,- 1,138,139,
44,- 1,142,143,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,- 1,- 1,114,- 1,- 1,- 1,118,119,- 1,
25,26,- 1,124,- 1,- 1,127,- 1,129,130,- 1,132,- 1,38,135,40,- 1,138,139,44,- 1,142,143,-
1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,71,72,- 1,74,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,- 1,- 1,114,- 1,- 1,- 1,118,119,- 1,25,26,- 1,124,- 1,
- 1,127,- 1,129,130,- 1,132,- 1,38,135,40,- 1,138,139,44,- 1,142,143,- 1,- 1,- 1,- 1,52,53,
54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
107,108,109,110,111,- 1,- 1,114,- 1,- 1,- 1,118,- 1,- 1,25,26,- 1,124,- 1,- 1,127,- 1,129,
130,- 1,132,- 1,38,135,40,- 1,138,139,44,- 1,142,143,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,-
1,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,
85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,
110,111,- 1,- 1,114,- 1,- 1,- 1,- 1,- 1,- 1,25,26,- 1,124,- 1,- 1,127,- 1,- 1,- 1,131,132,- 1,
38,135,40,- 1,138,139,44,- 1,142,143,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,
63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,- 1,- 1,114,
- 1,- 1,117,25,26,- 1,- 1,- 1,- 1,124,- 1,- 1,127,- 1,- 1,- 1,38,132,40,- 1,135,- 1,44,138,
139,- 1,- 1,142,143,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,71,
72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,- 1,- 1,114,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,
124,125,- 1,127,- 1,- 1,- 1,38,132,40,- 1,135,- 1,44,138,139,- 1,- 1,142,143,- 1,52,53,54,
55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,
108,109,110,111,- 1,- 1,114,- 1,- 1,- 1,- 1,- 1,- 1,25,26,- 1,124,- 1,- 1,127,- 1,- 1,- 1,131,
132,- 1,38,135,40,- 1,138,139,44,- 1,142,143,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,
- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,- 1,
- 1,114,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,124,125,- 1,127,- 1,- 1,- 1,38,132,40,- 1,135,- 1,44,
138,139,- 1,- 1,142,143,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,-
1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,- 1,- 1,114,- 1,- 1,- 1,- 1,- 1,- 1,25,
26,- 1,124,- 1,- 1,127,- 1,- 1,- 1,131,132,- 1,38,135,40,- 1,138,139,44,- 1,142,143,- 1,- 1,
- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,71,72,- 1,74,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,107,108,109,110,111,- 1,- 1,114,- 1,- 1,- 1,- 1,- 1,- 1,25,26,- 1,124,- 1,- 1,127,
- 1,- 1,- 1,131,132,- 1,38,135,40,- 1,138,139,44,- 1,142,143,- 1,- 1,- 1,- 1,52,53,54,55,-
1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,
109,110,111,- 1,- 1,114,- 1,- 1,117,25,26,- 1,- 1,- 1,- 1,124,- 1,- 1,127,- 1,- 1,- 1,38,132,
40,- 1,135,- 1,44,138,139,- 1,- 1,142,143,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,- 1,- 1,114,- 1,- 1,
117,25,26,- 1,- 1,- 1,- 1,124,- 1,- 1,127,- 1,- 1,- 1,38,132,40,- 1,135,- 1,44,138,139,- 1,-
1,142,143,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,71,72,- 1,
74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,- 1,- 1,114,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,124,
125,- 1,127,- 1,- 1,- 1,38,132,40,- 1,135,- 1,44,138,139,- 1,- 1,142,143,- 1,52,53,54,55,
- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,
109,110,111,- 1,- 1,114,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,124,125,- 1,127,- 1,- 1,- 1,38,132,
40,- 1,135,- 1,44,138,139,- 1,- 1,142,143,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,- 1,- 1,114,- 1,- 1,
117,25,26,- 1,- 1,- 1,- 1,124,- 1,- 1,127,- 1,- 1,- 1,38,132,40,- 1,135,- 1,44,138,139,- 1,-
1,142,143,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,71,72,- 1,
74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,- 1,- 1,114,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,124,
125,- 1,127,- 1,- 1,- 1,38,132,40,- 1,135,- 1,44,138,139,- 1,- 1,142,143,- 1,52,53,54,55,
- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,
109,110,111,- 1,- 1,114,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,124,125,- 1,127,- 1,- 1,- 1,38,132,
40,- 1,135,- 1,44,138,139,- 1,- 1,142,143,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,- 1,- 1,114,- 1,- 1,
- 1,25,26,- 1,- 1,- 1,- 1,124,125,- 1,127,- 1,- 1,- 1,38,132,40,- 1,135,- 1,44,138,139,- 1,-
1,142,143,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,71,72,- 1,
74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,- 1,- 1,114,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,124,
125,- 1,127,- 1,- 1,- 1,38,132,40,- 1,135,- 1,44,138,139,- 1,- 1,142,143,- 1,52,53,54,55,
- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,- 1,25,26,- 1,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,38,- 1,- 1,- 1,
- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,- 1,- 1,- 1,63,- 1,- 1,107,108,
109,110,111,71,72,114,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,124,84,85,127,- 1,- 1,- 1,- 1,132,-
1,- 1,135,- 1,- 1,138,139,- 1,- 1,142,143,- 1,- 1,- 1,- 1,107,108,109,110,111,- 1,- 1,114,-
1,- 1,- 1,118,- 1,25,26,- 1,- 1,124,- 1,- 1,127,- 1,- 1,- 1,- 1,132,38,- 1,135,- 1,- 1,138,139,
- 1,- 1,142,143,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,- 1,- 1,- 1,63,- 1,- 1,25,26,- 1,- 1,- 1,
71,72,- 1,74,- 1,- 1,- 1,- 1,38,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,52,53,54,55,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,63,- 1,- 1,107,108,109,110,111,71,72,114,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,124,84,85,127,- 1,- 1,- 1,- 1,132,- 1,- 1,135,- 1,- 1,138,139,- 1,- 1,142,143,- 1,- 1,- 1,
- 1,107,108,109,110,111,- 1,- 1,114,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,124,- 1,- 1,127,- 1,- 1,
38,- 1,132,- 1,- 1,135,- 1,- 1,138,139,- 1,- 1,142,143,52,53,54,55,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
63,- 1,- 1,25,26,- 1,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,38,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,- 1,- 1,- 1,63,38,- 1,107,108,109,110,111,71,72,114,
74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,124,84,85,127,- 1,- 1,- 1,- 1,132,- 1,- 1,135,- 1,- 1,138,139,
- 1,- 1,142,143,- 1,- 1,- 1,- 1,107,108,109,110,111,- 1,- 1,114,- 1,- 1,- 1,92,- 1,- 1,- 1,- 1,
- 1,124,- 1,- 1,127,- 1,- 1,104,- 1,132,107,108,135,110,111,138,139,114,- 1,142,143,118,
- 1,- 1,- 1,- 1,- 1,124,- 1,126,127,- 1,- 1,- 1,- 1,132,- 1,- 1,- 1,- 1,- 1,138};char Cyc_Yyimpossible[
17]="\000\000\000\000Yyimpossible\000";char Cyc_Yystack_overflow[21]="\000\000\000\000Yystack_overflow\000";
void Cyc_yyerror(struct _dyneither_ptr);int Cyc_yylex(struct Cyc_Lexing_lexbuf*);
static int Cyc_yychar=(int)'\000';union Cyc_YYSTYPE Cyc_yylval={.YYINITIALSVAL={60,0}};
static int Cyc_yynerrs=0;struct _tuple24{struct Cyc_Position_Segment*f1;struct
_tuple1*f2;int f3;};struct _tuple25{struct _dyneither_ptr f1;void*f2;};static char
_tmp465[8]="stdcall";static char _tmp466[6]="cdecl";static char _tmp467[9]="fastcall";
static char _tmp468[9]="noreturn";static char _tmp469[6]="const";static char _tmp46A[8]="aligned";
static char _tmp46B[7]="packed";static char _tmp46C[7]="shared";static char _tmp46D[7]="unused";
static char _tmp46E[5]="weak";static char _tmp46F[10]="dllimport";static char _tmp470[
10]="dllexport";static char _tmp471[23]="no_instrument_function";static char _tmp472[
12]="constructor";static char _tmp473[11]="destructor";static char _tmp474[22]="no_check_memory_usage";
static char _tmp475[5]="pure";struct _tuple26{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*
f2;};int Cyc_yyparse(struct Cyc_Lexing_lexbuf*yylex_buf){struct _RegionHandle
_tmp3D0=_new_region("yyregion");struct _RegionHandle*yyregion=& _tmp3D0;
_push_region(yyregion);{int yystate;int yyn=0;int yyerrstatus;int yychar1=0;int
yyssp_offset;short*yyss=(short*)({unsigned int _tmp76C=10000;short*_tmp76D=(short*)
_region_malloc(yyregion,_check_times(sizeof(short),_tmp76C));{unsigned int
_tmp76E=_tmp76C;unsigned int i;for(i=0;i < _tmp76E;i ++){_tmp76D[i]=(short)0;}}
_tmp76D;});int yyvsp_offset;union Cyc_YYSTYPE*yyvs=(union Cyc_YYSTYPE*)({
unsigned int _tmp769=10000;union Cyc_YYSTYPE*_tmp76A=(union Cyc_YYSTYPE*)
_region_malloc(yyregion,_check_times(sizeof(union Cyc_YYSTYPE),_tmp769));{
unsigned int _tmp76B=_tmp769;unsigned int i;for(i=0;i < _tmp76B;i ++){_tmp76A[i]=Cyc_yylval;}}
_tmp76A;});int yylsp_offset;struct Cyc_Yyltype*yyls=(struct Cyc_Yyltype*)({
unsigned int _tmp766=10000;struct Cyc_Yyltype*_tmp767=(struct Cyc_Yyltype*)
_region_malloc(yyregion,_check_times(sizeof(struct Cyc_Yyltype),_tmp766));{
unsigned int _tmp768=_tmp766;unsigned int i;for(i=0;i < _tmp768;i ++){_tmp767[i]=Cyc_yynewloc();}}
_tmp767;});int yystacksize=10000;union Cyc_YYSTYPE yyval=Cyc_yylval;int yylen;
yystate=0;yyerrstatus=0;Cyc_yynerrs=0;Cyc_yychar=- 2;yyssp_offset=- 1;yyvsp_offset=
0;yylsp_offset=0;yynewstate: yyss[_check_known_subscript_notnull(10000,++
yyssp_offset)]=(short)yystate;if(yyssp_offset >= yystacksize - 1){if(yystacksize >= 
10000){Cyc_yyerror(({const char*_tmp3D1="parser stack overflow";_tag_dyneither(
_tmp3D1,sizeof(char),22);}));(int)_throw((void*)Cyc_Yystack_overflow);}}goto
yybackup;yybackup: yyn=(int)Cyc_yypact[_check_known_subscript_notnull(1035,
yystate)];if(yyn == - 32768)goto yydefault;if(Cyc_yychar == - 2)Cyc_yychar=Cyc_yylex(
yylex_buf);if(Cyc_yychar <= 0){yychar1=0;Cyc_yychar=0;}else{yychar1=Cyc_yychar > 0
 && Cyc_yychar <= 371?(int)Cyc_yytranslate[_check_known_subscript_notnull(372,Cyc_yychar)]:
288;}yyn +=yychar1;if((yyn < 0  || yyn > 6273) || Cyc_yycheck[
_check_known_subscript_notnull(6274,yyn)]!= yychar1)goto yydefault;yyn=(int)Cyc_yytable[
_check_known_subscript_notnull(6274,yyn)];if(yyn < 0){if(yyn == - 32768)goto
yyerrlab;yyn=- yyn;goto yyreduce;}else{if(yyn == 0)goto yyerrlab;}if(yyn == 1034){int
_tmp3D2=0;_npop_handler(0);return _tmp3D2;}if(Cyc_yychar != 0)Cyc_yychar=- 2;yyvs[
_check_known_subscript_notnull(10000,++ yyvsp_offset)]=Cyc_yylval;yyls[
_check_known_subscript_notnull(10000,++ yylsp_offset)]=Cyc_yylloc;if(yyerrstatus
!= 0)-- yyerrstatus;yystate=yyn;goto yynewstate;yydefault: yyn=(int)Cyc_yydefact[
_check_known_subscript_notnull(1035,yystate)];if(yyn == 0)goto yyerrlab;yyreduce:
yylen=(int)Cyc_yyr2[_check_known_subscript_notnull(513,yyn)];if(yylen > 0)yyval=
yyvs[_check_known_subscript_notnull(10000,(yyvsp_offset + 1)- yylen)];switch(yyn){
case 1: _LL203: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];Cyc_Parse_parse_result=
Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);break;
case 2: _LL204: {struct Cyc_List_List*x;struct Cyc_List_List*y;{struct _handler_cons
_tmp3D3;_push_handler(& _tmp3D3);{int _tmp3D5=0;if(setjmp(_tmp3D3.handler))_tmp3D5=
1;if(!_tmp3D5){x=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);;_pop_handler();}else{void*_tmp3D4=(void*)_exn_thrown;void*
_tmp3D7=_tmp3D4;_LL207: if(*((void**)_tmp3D7)!= Cyc_Core_Failure)goto _LL209;
_LL208: x=0;goto _LL206;_LL209:;_LL20A:(void)_throw(_tmp3D7);_LL206:;}}}{struct
_handler_cons _tmp3D8;_push_handler(& _tmp3D8);{int _tmp3DA=0;if(setjmp(_tmp3D8.handler))
_tmp3DA=1;if(!_tmp3DA){y=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);;_pop_handler();}else{void*_tmp3D9=(void*)_exn_thrown;void*
_tmp3DC=_tmp3D9;_LL20C: if(*((void**)_tmp3DC)!= Cyc_Core_Failure)goto _LL20E;
_LL20D: y=0;goto _LL20B;_LL20E:;_LL20F:(void)_throw(_tmp3DC);_LL20B:;}}}yyval=Cyc_YY18(((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(
x,y));break;}case 3: _LL205: yyval=Cyc_YY18(({struct Cyc_List_List*_tmp3DD=_cycalloc(
sizeof(*_tmp3DD));_tmp3DD->hd=({struct Cyc_Absyn_Decl*_tmp3DE=_cycalloc(sizeof(*
_tmp3DE));_tmp3DE->r=(void*)({struct Cyc_Absyn_Using_d_struct*_tmp3DF=_cycalloc(
sizeof(*_tmp3DF));_tmp3DF[0]=({struct Cyc_Absyn_Using_d_struct _tmp3E0;_tmp3E0.tag=
11;_tmp3E0.f1=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]);_tmp3E0.f2=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp3E0;});_tmp3DF;});_tmp3DE->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp3DE;});
_tmp3DD->tl=0;_tmp3DD;}));Cyc_Lex_leave_using();break;case 4: _LL210: yyval=Cyc_YY18(({
struct Cyc_List_List*_tmp3E1=_cycalloc(sizeof(*_tmp3E1));_tmp3E1->hd=({struct Cyc_Absyn_Decl*
_tmp3E2=_cycalloc(sizeof(*_tmp3E2));_tmp3E2->r=(void*)({struct Cyc_Absyn_Using_d_struct*
_tmp3E3=_cycalloc(sizeof(*_tmp3E3));_tmp3E3[0]=({struct Cyc_Absyn_Using_d_struct
_tmp3E4;_tmp3E4.tag=11;_tmp3E4.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp3E4.f2=Cyc_yyget_YY18(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp3E4;});_tmp3E3;});
_tmp3E2->loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line);_tmp3E2;});_tmp3E1->tl=Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp3E1;}));break;case 5:
_LL211: yyval=Cyc_YY18(({struct Cyc_List_List*_tmp3E5=_cycalloc(sizeof(*_tmp3E5));
_tmp3E5->hd=({struct Cyc_Absyn_Decl*_tmp3E6=_cycalloc(sizeof(*_tmp3E6));_tmp3E6->r=(
void*)({struct Cyc_Absyn_Namespace_d_struct*_tmp3E7=_cycalloc(sizeof(*_tmp3E7));
_tmp3E7[0]=({struct Cyc_Absyn_Namespace_d_struct _tmp3E8;_tmp3E8.tag=10;_tmp3E8.f1=({
struct _dyneither_ptr*_tmp3E9=_cycalloc(sizeof(*_tmp3E9));_tmp3E9[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp3E9;});_tmp3E8.f2=
Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp3E8;});
_tmp3E7;});_tmp3E6->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp3E6;});
_tmp3E5->tl=0;_tmp3E5;}));Cyc_Lex_leave_namespace();break;case 6: _LL212: {struct
_dyneither_ptr nspace;struct Cyc_List_List*x;struct Cyc_List_List*y;{struct
_handler_cons _tmp3EA;_push_handler(& _tmp3EA);{int _tmp3EC=0;if(setjmp(_tmp3EA.handler))
_tmp3EC=1;if(!_tmp3EC){nspace=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);;_pop_handler();}else{
void*_tmp3EB=(void*)_exn_thrown;void*_tmp3EE=_tmp3EB;_LL215: if(*((void**)_tmp3EE)
!= Cyc_Core_Failure)goto _LL217;_LL216: nspace=({const char*_tmp3EF="";
_tag_dyneither(_tmp3EF,sizeof(char),1);});goto _LL214;_LL217:;_LL218:(void)_throw(
_tmp3EE);_LL214:;}}}{struct _handler_cons _tmp3F0;_push_handler(& _tmp3F0);{int
_tmp3F2=0;if(setjmp(_tmp3F0.handler))_tmp3F2=1;if(!_tmp3F2){x=Cyc_yyget_YY18(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);;_pop_handler();}
else{void*_tmp3F1=(void*)_exn_thrown;void*_tmp3F4=_tmp3F1;_LL21A: if(*((void**)
_tmp3F4)!= Cyc_Core_Failure)goto _LL21C;_LL21B: x=0;goto _LL219;_LL21C:;_LL21D:(
void)_throw(_tmp3F4);_LL219:;}}}{struct _handler_cons _tmp3F5;_push_handler(&
_tmp3F5);{int _tmp3F7=0;if(setjmp(_tmp3F5.handler))_tmp3F7=1;if(!_tmp3F7){y=Cyc_yyget_YY18(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);;_pop_handler();}else{
void*_tmp3F6=(void*)_exn_thrown;void*_tmp3F9=_tmp3F6;_LL21F: if(*((void**)_tmp3F9)
!= Cyc_Core_Failure)goto _LL221;_LL220: y=0;goto _LL21E;_LL221:;_LL222:(void)_throw(
_tmp3F9);_LL21E:;}}}yyval=Cyc_YY18(({struct Cyc_List_List*_tmp3FA=_cycalloc(
sizeof(*_tmp3FA));_tmp3FA->hd=({struct Cyc_Absyn_Decl*_tmp3FB=_cycalloc(sizeof(*
_tmp3FB));_tmp3FB->r=(void*)({struct Cyc_Absyn_Namespace_d_struct*_tmp3FC=
_cycalloc(sizeof(*_tmp3FC));_tmp3FC[0]=({struct Cyc_Absyn_Namespace_d_struct
_tmp3FD;_tmp3FD.tag=10;_tmp3FD.f1=({struct _dyneither_ptr*_tmp3FE=_cycalloc(
sizeof(*_tmp3FE));_tmp3FE[0]=nspace;_tmp3FE;});_tmp3FD.f2=x;_tmp3FD;});_tmp3FC;});
_tmp3FB->loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line);_tmp3FB;});_tmp3FA->tl=y;_tmp3FA;}));break;}case 7:
_LL213: if(Cyc_strcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),({const char*_tmp3FF="C";
_tag_dyneither(_tmp3FF,sizeof(char),2);}))== 0)yyval=Cyc_YY18(({struct Cyc_List_List*
_tmp400=_cycalloc(sizeof(*_tmp400));_tmp400->hd=({struct Cyc_Absyn_Decl*_tmp401=
_cycalloc(sizeof(*_tmp401));_tmp401->r=(void*)({struct Cyc_Absyn_ExternC_d_struct*
_tmp402=_cycalloc(sizeof(*_tmp402));_tmp402[0]=({struct Cyc_Absyn_ExternC_d_struct
_tmp403;_tmp403.tag=12;_tmp403.f1=Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp403;});_tmp402;});
_tmp401->loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line);_tmp401;});_tmp400->tl=Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp400;}));else{if(Cyc_strcmp((
struct _dyneither_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 4)]),({const char*_tmp404="C include";_tag_dyneither(_tmp404,
sizeof(char),10);}))!= 0)Cyc_Parse_err(({const char*_tmp405="expecting \"C\" or \"C include\"";
_tag_dyneither(_tmp405,sizeof(char),29);}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).last_line));yyval=Cyc_YY18(({
struct Cyc_List_List*_tmp406=_cycalloc(sizeof(*_tmp406));_tmp406->hd=({struct Cyc_Absyn_Decl*
_tmp407=_cycalloc(sizeof(*_tmp407));_tmp407->r=(void*)({struct Cyc_Absyn_ExternCinclude_d_struct*
_tmp408=_cycalloc(sizeof(*_tmp408));_tmp408[0]=({struct Cyc_Absyn_ExternCinclude_d_struct
_tmp409;_tmp409.tag=13;_tmp409.f1=Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp409.f2=0;_tmp409;});
_tmp408;});_tmp407->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line);_tmp407;});
_tmp406->tl=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp406;}));}break;case 8: _LL223: if(Cyc_strcmp((struct
_dyneither_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 5)]),({const char*_tmp40A="C include";_tag_dyneither(_tmp40A,
sizeof(char),10);}))!= 0)Cyc_Parse_err(({const char*_tmp40B="expecting \"C include\"";
_tag_dyneither(_tmp40B,sizeof(char),22);}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).last_line));{struct Cyc_List_List*
exs=Cyc_yyget_YY52(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
yyval=Cyc_YY18(({struct Cyc_List_List*_tmp40C=_cycalloc(sizeof(*_tmp40C));_tmp40C->hd=({
struct Cyc_Absyn_Decl*_tmp40D=_cycalloc(sizeof(*_tmp40D));_tmp40D->r=(void*)({
struct Cyc_Absyn_ExternCinclude_d_struct*_tmp40E=_cycalloc(sizeof(*_tmp40E));
_tmp40E[0]=({struct Cyc_Absyn_ExternCinclude_d_struct _tmp40F;_tmp40F.tag=13;
_tmp40F.f1=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset
- 3)]);_tmp40F.f2=exs;_tmp40F;});_tmp40E;});_tmp40D->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line);_tmp40D;});
_tmp40C->tl=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp40C;}));break;}case 9: _LL224: yyval=Cyc_YY18(({struct Cyc_List_List*
_tmp410=_cycalloc(sizeof(*_tmp410));_tmp410->hd=({struct Cyc_Absyn_Decl*_tmp411=
_cycalloc(sizeof(*_tmp411));_tmp411->r=(void*)0;_tmp411->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line);_tmp411;});
_tmp410->tl=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp410;}));break;case 10: _LL225: yyval=Cyc_YY18(({struct Cyc_List_List*
_tmp412=_cycalloc(sizeof(*_tmp412));_tmp412->hd=({struct Cyc_Absyn_Decl*_tmp413=
_cycalloc(sizeof(*_tmp413));_tmp413->r=(void*)1;_tmp413->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line);_tmp413;});
_tmp412->tl=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp412;}));break;case 11: _LL226: yyval=Cyc_YY18(0);break;case 12:
_LL227: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;
case 13: _LL228: yyval=Cyc_YY52(0);break;case 14: _LL229: yyval=Cyc_YY52(({struct Cyc_List_List*
_tmp414=_cycalloc(sizeof(*_tmp414));_tmp414->hd=({struct _tuple24*_tmp415=
_cycalloc(sizeof(*_tmp415));_tmp415->f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line);_tmp415->f2=
Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp415->f3=0;_tmp415;});_tmp414->tl=0;_tmp414;}));break;case 15: _LL22A: yyval=Cyc_YY52(({
struct Cyc_List_List*_tmp416=_cycalloc(sizeof(*_tmp416));_tmp416->hd=({struct
_tuple24*_tmp417=_cycalloc(sizeof(*_tmp417));_tmp417->f1=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line);_tmp417->f2=
Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);
_tmp417->f3=0;_tmp417;});_tmp416->tl=Cyc_yyget_YY52(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp416;}));break;case 16:
_LL22B: yyval=Cyc_YY18(({struct Cyc_List_List*_tmp418=_cycalloc(sizeof(*_tmp418));
_tmp418->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Fn_d_struct*_tmp419=
_cycalloc(sizeof(*_tmp419));_tmp419[0]=({struct Cyc_Absyn_Fn_d_struct _tmp41A;
_tmp41A.tag=1;_tmp41A.f1=Cyc_yyget_YY17(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp41A;});_tmp419;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp418->tl=0;
_tmp418;}));break;case 17: _LL22C: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 18: _LL22D: yyval=Cyc_YY18(0);break;case 19: _LL22E: yyval=
Cyc_YY17(Cyc_Parse_make_function(0,Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0,Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 20:
_LL22F: yyval=Cyc_YY17(Cyc_Parse_make_function(({struct Cyc_Core_Opt*_tmp41B=
_cycalloc(sizeof(*_tmp41B));_tmp41B->v=Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp41B;}),Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0,Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 21:
_LL230: yyval=Cyc_YY17(Cyc_Parse_make_function(0,Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 22:
_LL231: yyval=Cyc_YY17(Cyc_Parse_make_function(({struct Cyc_Core_Opt*_tmp41C=
_cycalloc(sizeof(*_tmp41C));_tmp41C->v=Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp41C;}),Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 23:
_LL232: yyval=Cyc_YY17(Cyc_Parse_make_function(({struct Cyc_Core_Opt*_tmp41D=
_cycalloc(sizeof(*_tmp41D));_tmp41D->v=Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp41D;}),Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0,Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 24:
_LL233: yyval=Cyc_YY17(Cyc_Parse_make_function(({struct Cyc_Core_Opt*_tmp41E=
_cycalloc(sizeof(*_tmp41E));_tmp41E->v=Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp41E;}),Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 25:
_LL234: Cyc_Lex_enter_using(Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]));yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 26: _LL235: Cyc_Lex_leave_using();
break;case 27: _LL236: Cyc_Lex_enter_namespace(({struct _dyneither_ptr*_tmp41F=
_cycalloc(sizeof(*_tmp41F));_tmp41F[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp41F;}));yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 28: _LL237: Cyc_Lex_leave_namespace();
break;case 29: _LL238: yyval=Cyc_YY18(Cyc_Parse_make_declarations(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line)));break;case
30: _LL239: yyval=Cyc_YY18(Cyc_Parse_make_declarations(Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY21(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 31:
_LL23A: yyval=Cyc_YY18(({struct Cyc_List_List*_tmp420=_cycalloc(sizeof(*_tmp420));
_tmp420->hd=Cyc_Absyn_let_decl(Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp420->tl=0;
_tmp420;}));break;case 32: _LL23B: {struct Cyc_List_List*_tmp421=0;{struct Cyc_List_List*
_tmp422=Cyc_yyget_YY36(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
for(0;_tmp422 != 0;_tmp422=_tmp422->tl){struct _dyneither_ptr*_tmp423=(struct
_dyneither_ptr*)_tmp422->hd;struct _tuple1*qv=({struct _tuple1*_tmp426=_cycalloc(
sizeof(*_tmp426));_tmp426->f1=Cyc_Absyn_Rel_n(0);_tmp426->f2=_tmp423;_tmp426;});
struct Cyc_Absyn_Vardecl*_tmp424=Cyc_Absyn_new_vardecl(qv,Cyc_Absyn_wildtyp(0),0);
_tmp421=({struct Cyc_List_List*_tmp425=_cycalloc(sizeof(*_tmp425));_tmp425->hd=
_tmp424;_tmp425->tl=_tmp421;_tmp425;});}}_tmp421=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp421);yyval=Cyc_YY18(({struct Cyc_List_List*
_tmp427=_cycalloc(sizeof(*_tmp427));_tmp427->hd=Cyc_Absyn_letv_decl(_tmp421,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp427->tl=0;
_tmp427;}));break;}case 33: _LL23C: if(Cyc_zstrcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),({const char*_tmp428="`H";
_tag_dyneither(_tmp428,sizeof(char),3);}))== 0)Cyc_Parse_err((struct
_dyneither_ptr)({void*_tmp429=0;Cyc_aprintf(({const char*_tmp42A="bad occurrence of heap region";
_tag_dyneither(_tmp42A,sizeof(char),30);}),_tag_dyneither(_tmp429,sizeof(void*),
0));}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line));if(Cyc_zstrcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),({const char*_tmp42B="`U";
_tag_dyneither(_tmp42B,sizeof(char),3);}))== 0)Cyc_Parse_err((struct
_dyneither_ptr)({void*_tmp42C=0;Cyc_aprintf(({const char*_tmp42D="bad occurrence of unique region";
_tag_dyneither(_tmp42D,sizeof(char),32);}),_tag_dyneither(_tmp42C,sizeof(void*),
0));}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line));{struct Cyc_Absyn_Tvar*tv=({struct Cyc_Absyn_Tvar*
_tmp435=_cycalloc(sizeof(*_tmp435));_tmp435->name=({struct _dyneither_ptr*_tmp436=
_cycalloc(sizeof(*_tmp436));_tmp436[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp436;});_tmp435->identity=
- 1;_tmp435->kind=Cyc_Tcutil_kind_to_bound((void*)3);_tmp435;});void*t=(void*)({
struct Cyc_Absyn_VarType_struct*_tmp433=_cycalloc(sizeof(*_tmp433));_tmp433[0]=({
struct Cyc_Absyn_VarType_struct _tmp434;_tmp434.tag=1;_tmp434.f1=tv;_tmp434;});
_tmp433;});struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(({struct _tuple1*
_tmp42F=_cycalloc(sizeof(*_tmp42F));_tmp42F->f1=Cyc_Absyn_Loc_n;_tmp42F->f2=({
struct _dyneither_ptr*_tmp430=_cycalloc(sizeof(*_tmp430));_tmp430[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp430;});_tmp42F;}),(
void*)({struct Cyc_Absyn_RgnHandleType_struct*_tmp431=_cycalloc(sizeof(*_tmp431));
_tmp431[0]=({struct Cyc_Absyn_RgnHandleType_struct _tmp432;_tmp432.tag=14;_tmp432.f1=(
void*)t;_tmp432;});_tmp431;}),0);yyval=Cyc_YY18(({struct Cyc_List_List*_tmp42E=
_cycalloc(sizeof(*_tmp42E));_tmp42E->hd=Cyc_Absyn_region_decl(tv,vd,Cyc_yyget_YY31(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp42E->tl=0;
_tmp42E;}));break;}case 34: _LL23D: if(Cyc_zstrcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),({const char*_tmp437="H";
_tag_dyneither(_tmp437,sizeof(char),2);}))== 0)Cyc_Parse_err(({const char*_tmp438="bad occurrence of heap region `H";
_tag_dyneither(_tmp438,sizeof(char),33);}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line));if(Cyc_zstrcmp((
struct _dyneither_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]),({const char*_tmp439="U";_tag_dyneither(_tmp439,sizeof(
char),2);}))== 0)Cyc_Parse_err(({const char*_tmp43A="bad occurrence of unique region `U";
_tag_dyneither(_tmp43A,sizeof(char),35);}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line));if(Cyc_yyget_YY31(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]) && Cyc_yyget_YY4(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])!= 0)Cyc_Parse_err(({
const char*_tmp43B="open regions cannot be @resetable";_tag_dyneither(_tmp43B,
sizeof(char),34);}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));{struct Cyc_Absyn_Tvar*
tv=({struct Cyc_Absyn_Tvar*_tmp443=_cycalloc(sizeof(*_tmp443));_tmp443->name=({
struct _dyneither_ptr*_tmp444=_cycalloc(sizeof(*_tmp444));_tmp444[0]=(struct
_dyneither_ptr)({struct Cyc_String_pa_struct _tmp447;_tmp447.tag=0;_tmp447.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]));{void*_tmp445[1]={&
_tmp447};Cyc_aprintf(({const char*_tmp446="`%s";_tag_dyneither(_tmp446,sizeof(
char),4);}),_tag_dyneither(_tmp445,sizeof(void*),1));}});_tmp444;});_tmp443->identity=
- 1;_tmp443->kind=Cyc_Tcutil_kind_to_bound((void*)3);_tmp443;});void*t=(void*)({
struct Cyc_Absyn_VarType_struct*_tmp441=_cycalloc(sizeof(*_tmp441));_tmp441[0]=({
struct Cyc_Absyn_VarType_struct _tmp442;_tmp442.tag=1;_tmp442.f1=tv;_tmp442;});
_tmp441;});struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(({struct _tuple1*
_tmp43D=_cycalloc(sizeof(*_tmp43D));_tmp43D->f1=Cyc_Absyn_Loc_n;_tmp43D->f2=({
struct _dyneither_ptr*_tmp43E=_cycalloc(sizeof(*_tmp43E));_tmp43E[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp43E;});_tmp43D;}),(
void*)({struct Cyc_Absyn_RgnHandleType_struct*_tmp43F=_cycalloc(sizeof(*_tmp43F));
_tmp43F[0]=({struct Cyc_Absyn_RgnHandleType_struct _tmp440;_tmp440.tag=14;_tmp440.f1=(
void*)t;_tmp440;});_tmp43F;}),0);yyval=Cyc_YY18(({struct Cyc_List_List*_tmp43C=
_cycalloc(sizeof(*_tmp43C));_tmp43C->hd=Cyc_Absyn_region_decl(tv,vd,Cyc_yyget_YY31(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_YY4(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp43C->tl=0;
_tmp43C;}));break;}case 35: _LL23E: {struct Cyc_Absyn_Tvar*tv=({struct Cyc_Absyn_Tvar*
_tmp44B=_cycalloc(sizeof(*_tmp44B));_tmp44B->name=({struct _dyneither_ptr*_tmp44E=
_cycalloc(sizeof(*_tmp44E));_tmp44E[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 5)]);_tmp44E;});_tmp44B->identity=
- 1;_tmp44B->kind=(void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp44C=_cycalloc(sizeof(*
_tmp44C));_tmp44C[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp44D;_tmp44D.tag=0;_tmp44D.f1=(
void*)((void*)3);_tmp44D;});_tmp44C;});_tmp44B;});struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(({
struct _tuple1*_tmp449=_cycalloc(sizeof(*_tmp449));_tmp449->f1=Cyc_Absyn_Loc_n;
_tmp449->f2=({struct _dyneither_ptr*_tmp44A=_cycalloc(sizeof(*_tmp44A));_tmp44A[0]=
Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);
_tmp44A;});_tmp449;}),(void*)0,0);yyval=Cyc_YY18(({struct Cyc_List_List*_tmp448=
_cycalloc(sizeof(*_tmp448));_tmp448->hd=Cyc_Absyn_alias_decl(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),tv,vd,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp448->tl=0;
_tmp448;}));break;}case 36: _LL23F: yyval=Cyc_YY31(0);break;case 37: _LL240: yyval=Cyc_YY31(
1);break;case 38: _LL241: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 39: _LL242: yyval=Cyc_YY18(((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY18(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])));break;case 40: _LL243: yyval=
Cyc_YY19(({struct Cyc_Parse_Declaration_spec*_tmp44F=_cycalloc(sizeof(*_tmp44F));
_tmp44F->sc=({struct Cyc_Core_Opt*_tmp450=_cycalloc(sizeof(*_tmp450));_tmp450->v=(
void*)Cyc_yyget_YY22(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
_tmp450;});_tmp44F->tq=Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp44F->type_specs=
0;_tmp44F->is_inline=0;_tmp44F->attributes=0;_tmp44F;}));break;case 41: _LL244: if((
Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->sc != 
0)({void*_tmp451=0;Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),({const char*
_tmp452="Only one storage class is allowed in a declaration";_tag_dyneither(
_tmp452,sizeof(char),51);}),_tag_dyneither(_tmp451,sizeof(void*),0));});yyval=
Cyc_YY19(({struct Cyc_Parse_Declaration_spec*_tmp453=_cycalloc(sizeof(*_tmp453));
_tmp453->sc=({struct Cyc_Core_Opt*_tmp454=_cycalloc(sizeof(*_tmp454));_tmp454->v=(
void*)Cyc_yyget_YY22(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp454;});_tmp453->tq=(Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->tq;_tmp453->type_specs=(Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->type_specs;_tmp453->is_inline=(
Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->is_inline;
_tmp453->attributes=(Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->attributes;_tmp453;}));break;case 42: _LL245: yyval=Cyc_YY19(({
struct Cyc_Parse_Declaration_spec*_tmp455=_cycalloc(sizeof(*_tmp455));_tmp455->sc=
0;_tmp455->tq=Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp455->type_specs=({
struct Cyc_List_List*_tmp456=_cycalloc(sizeof(*_tmp456));_tmp456->hd=(void*)Cyc_yyget_YY23(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp456->tl=0;_tmp456;});
_tmp455->is_inline=0;_tmp455->attributes=0;_tmp455;}));break;case 43: _LL246: yyval=
Cyc_YY19(({struct Cyc_Parse_Declaration_spec*_tmp457=_cycalloc(sizeof(*_tmp457));
_tmp457->sc=(Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->sc;_tmp457->tq=(Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->tq;_tmp457->type_specs=({
struct Cyc_List_List*_tmp458=_cycalloc(sizeof(*_tmp458));_tmp458->hd=(void*)Cyc_yyget_YY23(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp458->tl=(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->type_specs;_tmp458;});
_tmp457->is_inline=(Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->is_inline;_tmp457->attributes=(Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes;_tmp457;}));
break;case 44: _LL247: yyval=Cyc_YY19(({struct Cyc_Parse_Declaration_spec*_tmp459=
_cycalloc(sizeof(*_tmp459));_tmp459->sc=0;_tmp459->tq=Cyc_yyget_YY25(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp459->type_specs=0;
_tmp459->is_inline=0;_tmp459->attributes=0;_tmp459;}));break;case 45: _LL248: yyval=
Cyc_YY19(({struct Cyc_Parse_Declaration_spec*_tmp45A=_cycalloc(sizeof(*_tmp45A));
_tmp45A->sc=(Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->sc;_tmp45A->tq=Cyc_Absyn_combine_tqual(Cyc_yyget_YY25(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->tq);_tmp45A->type_specs=(
Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->type_specs;
_tmp45A->is_inline=(Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->is_inline;_tmp45A->attributes=(Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes;_tmp45A;}));
break;case 46: _LL249: yyval=Cyc_YY19(({struct Cyc_Parse_Declaration_spec*_tmp45B=
_cycalloc(sizeof(*_tmp45B));_tmp45B->sc=0;_tmp45B->tq=Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp45B->type_specs=
0;_tmp45B->is_inline=1;_tmp45B->attributes=0;_tmp45B;}));break;case 47: _LL24A:
yyval=Cyc_YY19(({struct Cyc_Parse_Declaration_spec*_tmp45C=_cycalloc(sizeof(*
_tmp45C));_tmp45C->sc=(Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->sc;_tmp45C->tq=(Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->tq;_tmp45C->type_specs=(
Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->type_specs;
_tmp45C->is_inline=1;_tmp45C->attributes=(Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes;_tmp45C;}));
break;case 48: _LL24B: yyval=Cyc_YY19(({struct Cyc_Parse_Declaration_spec*_tmp45D=
_cycalloc(sizeof(*_tmp45D));_tmp45D->sc=0;_tmp45D->tq=Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp45D->type_specs=
0;_tmp45D->is_inline=0;_tmp45D->attributes=Cyc_yyget_YY45(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp45D;}));break;case 49:
_LL24C: yyval=Cyc_YY19(({struct Cyc_Parse_Declaration_spec*_tmp45E=_cycalloc(
sizeof(*_tmp45E));_tmp45E->sc=(Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->sc;_tmp45E->tq=(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tq;_tmp45E->type_specs=(
Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->type_specs;
_tmp45E->is_inline=(Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->is_inline;_tmp45E->attributes=Cyc_List_imp_append(Cyc_yyget_YY45(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes);_tmp45E;}));
break;case 50: _LL24D: yyval=Cyc_YY22((void*)4);break;case 51: _LL24E: yyval=Cyc_YY22((
void*)5);break;case 52: _LL24F: yyval=Cyc_YY22((void*)3);break;case 53: _LL250: yyval=
Cyc_YY22((void*)1);break;case 54: _LL251: if(Cyc_strcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),({const char*_tmp45F="C";
_tag_dyneither(_tmp45F,sizeof(char),2);}))!= 0)Cyc_Parse_err(({const char*_tmp460="only extern or extern \"C\" is allowed";
_tag_dyneither(_tmp460,sizeof(char),37);}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=Cyc_YY22((
void*)2);break;case 55: _LL252: yyval=Cyc_YY22((void*)0);break;case 56: _LL253: yyval=
Cyc_YY22((void*)6);break;case 57: _LL254: yyval=Cyc_YY45(0);break;case 58: _LL255:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 59:
_LL256: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)];break;
case 60: _LL257: yyval=Cyc_YY45(({struct Cyc_List_List*_tmp461=_cycalloc(sizeof(*
_tmp461));_tmp461->hd=(void*)Cyc_yyget_YY46(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp461->tl=0;_tmp461;}));break;case 61: _LL258: yyval=Cyc_YY45(({
struct Cyc_List_List*_tmp462=_cycalloc(sizeof(*_tmp462));_tmp462->hd=(void*)Cyc_yyget_YY46(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp462->tl=Cyc_yyget_YY45(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp462;}));break;case
62: _LL259: {static struct Cyc_Absyn_Aligned_att_struct att_aligned={1,- 1};static
struct _tuple25 att_map[17]={{{_tmp465,_tmp465,_tmp465 + 8},(void*)0},{{_tmp466,
_tmp466,_tmp466 + 6},(void*)1},{{_tmp467,_tmp467,_tmp467 + 9},(void*)2},{{_tmp468,
_tmp468,_tmp468 + 9},(void*)3},{{_tmp469,_tmp469,_tmp469 + 6},(void*)4},{{_tmp46A,
_tmp46A,_tmp46A + 8},(void*)& att_aligned},{{_tmp46B,_tmp46B,_tmp46B + 7},(void*)5},{{
_tmp46C,_tmp46C,_tmp46C + 7},(void*)7},{{_tmp46D,_tmp46D,_tmp46D + 7},(void*)8},{{
_tmp46E,_tmp46E,_tmp46E + 5},(void*)9},{{_tmp46F,_tmp46F,_tmp46F + 10},(void*)10},{{
_tmp470,_tmp470,_tmp470 + 10},(void*)11},{{_tmp471,_tmp471,_tmp471 + 23},(void*)12},{{
_tmp472,_tmp472,_tmp472 + 12},(void*)13},{{_tmp473,_tmp473,_tmp473 + 11},(void*)14},{{
_tmp474,_tmp474,_tmp474 + 22},(void*)15},{{_tmp475,_tmp475,_tmp475 + 5},(void*)16}};
struct _dyneither_ptr _tmp463=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);if((((_get_dyneither_size(
_tmp463,sizeof(char))> 4  && *((const char*)_check_dyneither_subscript(_tmp463,
sizeof(char),0))== '_') && *((const char*)_check_dyneither_subscript(_tmp463,
sizeof(char),1))== '_') && *((const char*)_check_dyneither_subscript(_tmp463,
sizeof(char),(int)(_get_dyneither_size(_tmp463,sizeof(char))- 2)))== '_') && *((
const char*)_check_dyneither_subscript(_tmp463,sizeof(char),(int)(
_get_dyneither_size(_tmp463,sizeof(char))- 3)))== '_')_tmp463=(struct
_dyneither_ptr)Cyc_substring((struct _dyneither_ptr)_tmp463,2,_get_dyneither_size(
_tmp463,sizeof(char))- 5);{int i=0;for(0;i < 17;++ i){if(Cyc_strcmp((struct
_dyneither_ptr)_tmp463,(struct _dyneither_ptr)(att_map[i]).f1)== 0){yyval=Cyc_YY46((
att_map[i]).f2);break;}}if(i == 17){Cyc_Parse_err(({const char*_tmp464="unrecognized attribute";
_tag_dyneither(_tmp464,sizeof(char),23);}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=Cyc_YY46((
void*)1);}break;}}case 63: _LL25A: yyval=Cyc_YY46((void*)4);break;case 64: _LL25B: {
struct _dyneither_ptr _tmp477=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);int _tmp479;struct _tuple6
_tmp478=Cyc_yyget_Int_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset
- 1)]);_tmp479=_tmp478.f2;{void*a;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp477,({
const char*_tmp47A="regparm";_tag_dyneither(_tmp47A,sizeof(char),8);}))== 0  || 
Cyc_zstrcmp((struct _dyneither_ptr)_tmp477,({const char*_tmp47B="__regparm__";
_tag_dyneither(_tmp47B,sizeof(char),12);}))== 0){if(_tmp479 < 0  || _tmp479 > 3)Cyc_Parse_err(({
const char*_tmp47C="regparm requires value between 0 and 3";_tag_dyneither(
_tmp47C,sizeof(char),39);}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));a=(void*)({
struct Cyc_Absyn_Regparm_att_struct*_tmp47D=_cycalloc_atomic(sizeof(*_tmp47D));
_tmp47D[0]=({struct Cyc_Absyn_Regparm_att_struct _tmp47E;_tmp47E.tag=0;_tmp47E.f1=
_tmp479;_tmp47E;});_tmp47D;});}else{if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp477,({
const char*_tmp47F="aligned";_tag_dyneither(_tmp47F,sizeof(char),8);}))== 0  || 
Cyc_zstrcmp((struct _dyneither_ptr)_tmp477,({const char*_tmp480="__aligned__";
_tag_dyneither(_tmp480,sizeof(char),12);}))== 0){if(_tmp479 < 0)Cyc_Parse_err(({
const char*_tmp481="aligned requires positive power of two";_tag_dyneither(
_tmp481,sizeof(char),39);}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));{
unsigned int j=(unsigned int)_tmp479;for(0;(j & 1)== 0;j=j >> 1){;}j=j >> 1;if(j != 0)
Cyc_Parse_err(({const char*_tmp482="aligned requires positive power of two";
_tag_dyneither(_tmp482,sizeof(char),39);}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));a=(void*)({
struct Cyc_Absyn_Aligned_att_struct*_tmp483=_cycalloc_atomic(sizeof(*_tmp483));
_tmp483[0]=({struct Cyc_Absyn_Aligned_att_struct _tmp484;_tmp484.tag=1;_tmp484.f1=
_tmp479;_tmp484;});_tmp483;});}}else{if(Cyc_zstrcmp((struct _dyneither_ptr)
_tmp477,({const char*_tmp485="initializes";_tag_dyneither(_tmp485,sizeof(char),12);}))
== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp477,({const char*_tmp486="__initializes__";
_tag_dyneither(_tmp486,sizeof(char),16);}))== 0)a=(void*)({struct Cyc_Absyn_Initializes_att_struct*
_tmp487=_cycalloc_atomic(sizeof(*_tmp487));_tmp487[0]=({struct Cyc_Absyn_Initializes_att_struct
_tmp488;_tmp488.tag=4;_tmp488.f1=_tmp479;_tmp488;});_tmp487;});else{Cyc_Parse_err(({
const char*_tmp489="unrecognized attribute";_tag_dyneither(_tmp489,sizeof(char),
23);}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line));a=(void*)1;}}}yyval=Cyc_YY46(a);break;}}case 65:
_LL25C: {struct _dyneither_ptr _tmp48A=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);struct _dyneither_ptr
_tmp48B=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);void*a;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp48A,({const
char*_tmp48C="section";_tag_dyneither(_tmp48C,sizeof(char),8);}))== 0  || Cyc_zstrcmp((
struct _dyneither_ptr)_tmp48A,({const char*_tmp48D="__section__";_tag_dyneither(
_tmp48D,sizeof(char),12);}))== 0)a=(void*)({struct Cyc_Absyn_Section_att_struct*
_tmp48E=_cycalloc(sizeof(*_tmp48E));_tmp48E[0]=({struct Cyc_Absyn_Section_att_struct
_tmp48F;_tmp48F.tag=2;_tmp48F.f1=_tmp48B;_tmp48F;});_tmp48E;});else{Cyc_Parse_err(({
const char*_tmp490="unrecognized attribute";_tag_dyneither(_tmp490,sizeof(char),
23);}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line));a=(void*)1;}yyval=Cyc_YY46(a);break;}case 66:
_LL25D: {struct _dyneither_ptr _tmp491=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);struct _dyneither_ptr
_tmp492=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);void*a;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp491,({const
char*_tmp493="__mode__";_tag_dyneither(_tmp493,sizeof(char),9);}))== 0)a=(void*)({
struct Cyc_Absyn_Mode_att_struct*_tmp494=_cycalloc(sizeof(*_tmp494));_tmp494[0]=({
struct Cyc_Absyn_Mode_att_struct _tmp495;_tmp495.tag=5;_tmp495.f1=_tmp492;_tmp495;});
_tmp494;});else{Cyc_Parse_err(({const char*_tmp496="unrecognized attribute";
_tag_dyneither(_tmp496,sizeof(char),23);}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));a=(void*)1;}
yyval=Cyc_YY46(a);break;}case 67: _LL25E: {struct _dyneither_ptr _tmp497=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 7)]);struct
_dyneither_ptr _tmp498=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 5)]);int _tmp49A;struct _tuple6 _tmp499=Cyc_yyget_Int_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp49A=_tmp499.f2;{int
_tmp49C;struct _tuple6 _tmp49B=Cyc_yyget_Int_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp49C=_tmp49B.f2;{void*
a=(void*)1;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp497,({const char*_tmp49D="format";
_tag_dyneither(_tmp49D,sizeof(char),7);}))== 0  || Cyc_zstrcmp((struct
_dyneither_ptr)_tmp497,({const char*_tmp49E="__format__";_tag_dyneither(_tmp49E,
sizeof(char),11);}))== 0){if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp498,({const
char*_tmp49F="printf";_tag_dyneither(_tmp49F,sizeof(char),7);}))== 0)a=(void*)({
struct Cyc_Absyn_Format_att_struct*_tmp4A0=_cycalloc(sizeof(*_tmp4A0));_tmp4A0[0]=({
struct Cyc_Absyn_Format_att_struct _tmp4A1;_tmp4A1.tag=3;_tmp4A1.f1=(void*)((void*)
0);_tmp4A1.f2=_tmp49A;_tmp4A1.f3=_tmp49C;_tmp4A1;});_tmp4A0;});else{if(Cyc_zstrcmp((
struct _dyneither_ptr)_tmp498,({const char*_tmp4A2="scanf";_tag_dyneither(_tmp4A2,
sizeof(char),6);}))== 0)a=(void*)({struct Cyc_Absyn_Format_att_struct*_tmp4A3=
_cycalloc(sizeof(*_tmp4A3));_tmp4A3[0]=({struct Cyc_Absyn_Format_att_struct
_tmp4A4;_tmp4A4.tag=3;_tmp4A4.f1=(void*)((void*)1);_tmp4A4.f2=_tmp49A;_tmp4A4.f3=
_tmp49C;_tmp4A4;});_tmp4A3;});else{Cyc_Parse_err(({const char*_tmp4A5="unrecognized format type";
_tag_dyneither(_tmp4A5,sizeof(char),25);}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).last_line));}}}else{Cyc_Parse_err(({
const char*_tmp4A6="unrecognized attribute";_tag_dyneither(_tmp4A6,sizeof(char),
23);}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 7)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));}yyval=Cyc_YY46(a);break;}}}case 68: _LL25F: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 69: _LL260: yyval=
Cyc_YY23(Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_TypedefType_struct*_tmp4A7=
_cycalloc(sizeof(*_tmp4A7));_tmp4A7[0]=({struct Cyc_Absyn_TypedefType_struct
_tmp4A8;_tmp4A8.tag=16;_tmp4A8.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp4A8.f2=Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp4A8.f3=0;_tmp4A8.f4=
0;_tmp4A8;});_tmp4A7;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 70:
_LL261: yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 71:
_LL262: yyval=Cyc_YY23(Cyc_Parse_type_spec(Cyc_Absyn_new_evar(0,0),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 72:
_LL263: yyval=Cyc_YY23(Cyc_Parse_type_spec(Cyc_Absyn_new_evar(Cyc_Tcutil_kind_to_opt(
Cyc_yyget_YY43(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])),0),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 73:
_LL264: yyval=Cyc_YY23(Cyc_Parse_type_spec(Cyc_Absyn_char_typ,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 74:
_LL265: yyval=Cyc_YY23((void*)({struct Cyc_Parse_Short_spec_struct*_tmp4A9=
_cycalloc(sizeof(*_tmp4A9));_tmp4A9[0]=({struct Cyc_Parse_Short_spec_struct
_tmp4AA;_tmp4AA.tag=2;_tmp4AA.f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp4AA;});
_tmp4A9;}));break;case 75: _LL266: yyval=Cyc_YY23(Cyc_Parse_type_spec(Cyc_Absyn_sint_typ,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));break;case 76: _LL267: yyval=Cyc_YY23((void*)({struct
Cyc_Parse_Long_spec_struct*_tmp4AB=_cycalloc(sizeof(*_tmp4AB));_tmp4AB[0]=({
struct Cyc_Parse_Long_spec_struct _tmp4AC;_tmp4AC.tag=3;_tmp4AC.f1=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp4AC;});
_tmp4AB;}));break;case 77: _LL268: yyval=Cyc_YY23(Cyc_Parse_type_spec(Cyc_Absyn_float_typ,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));break;case 78: _LL269: yyval=Cyc_YY23(Cyc_Parse_type_spec(
Cyc_Absyn_double_typ(0),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 79:
_LL26A: yyval=Cyc_YY23((void*)({struct Cyc_Parse_Signed_spec_struct*_tmp4AD=
_cycalloc(sizeof(*_tmp4AD));_tmp4AD[0]=({struct Cyc_Parse_Signed_spec_struct
_tmp4AE;_tmp4AE.tag=0;_tmp4AE.f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp4AE;});
_tmp4AD;}));break;case 80: _LL26B: yyval=Cyc_YY23((void*)({struct Cyc_Parse_Unsigned_spec_struct*
_tmp4AF=_cycalloc(sizeof(*_tmp4AF));_tmp4AF[0]=({struct Cyc_Parse_Unsigned_spec_struct
_tmp4B0;_tmp4B0.tag=1;_tmp4B0.f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp4B0;});
_tmp4AF;}));break;case 81: _LL26C: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 82: _LL26D: yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)];break;case 83: _LL26E: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 84: _LL26F: yyval=
Cyc_YY23(Cyc_Parse_type_spec(Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 85:
_LL270: yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_TupleType_struct*
_tmp4B1=_cycalloc(sizeof(*_tmp4B1));_tmp4B1[0]=({struct Cyc_Absyn_TupleType_struct
_tmp4B2;_tmp4B2.tag=9;_tmp4B2.f1=((struct Cyc_List_List*(*)(struct _tuple16*(*f)(
struct Cyc_Position_Segment*,struct _tuple8*),struct Cyc_Position_Segment*env,
struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_get_tqual_typ,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));_tmp4B2;});_tmp4B1;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));break;case 86: _LL271: yyval=Cyc_YY23(Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_RgnHandleType_struct*_tmp4B3=_cycalloc(sizeof(*_tmp4B3));
_tmp4B3[0]=({struct Cyc_Absyn_RgnHandleType_struct _tmp4B4;_tmp4B4.tag=14;_tmp4B4.f1=(
void*)Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp4B4;});_tmp4B3;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 87:
_LL272: {void*_tmp4B5=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,0);
yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_DynRgnType_struct*
_tmp4B6=_cycalloc(sizeof(*_tmp4B6));_tmp4B6[0]=({struct Cyc_Absyn_DynRgnType_struct
_tmp4B7;_tmp4B7.tag=15;_tmp4B7.f1=(void*)Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp4B7.f2=(void*)
_tmp4B5;_tmp4B7;});_tmp4B6;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;}case 88:
_LL273: yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_DynRgnType_struct*
_tmp4B8=_cycalloc(sizeof(*_tmp4B8));_tmp4B8[0]=({struct Cyc_Absyn_DynRgnType_struct
_tmp4B9;_tmp4B9.tag=15;_tmp4B9.f1=(void*)Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp4B9.f2=(void*)Cyc_yyget_YY44(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp4B9;});_tmp4B8;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));break;case 89: _LL274: yyval=Cyc_YY23(Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_TagType_struct*_tmp4BA=_cycalloc(sizeof(*_tmp4BA));
_tmp4BA[0]=({struct Cyc_Absyn_TagType_struct _tmp4BB;_tmp4BB.tag=18;_tmp4BB.f1=(
void*)Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp4BB;});_tmp4BA;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 90:
_LL275: yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_TagType_struct*
_tmp4BC=_cycalloc(sizeof(*_tmp4BC));_tmp4BC[0]=({struct Cyc_Absyn_TagType_struct
_tmp4BD;_tmp4BD.tag=18;_tmp4BD.f1=(void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_ik,0);_tmp4BD;});_tmp4BC;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 91:
_LL276: yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_ValueofType_struct*
_tmp4BE=_cycalloc(sizeof(*_tmp4BE));_tmp4BE[0]=({struct Cyc_Absyn_ValueofType_struct
_tmp4BF;_tmp4BF.tag=17;_tmp4BF.f1=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp4BF;});_tmp4BE;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));break;case 92: _LL277: yyval=Cyc_YY43(Cyc_Parse_id_to_kind(
Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));break;case 93: _LL278: {struct Cyc_Position_Segment*
loc=Cyc_Absyn_porting_c_code?Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line): 0;yyval=Cyc_YY25(({
struct Cyc_Absyn_Tqual _tmp4C0;_tmp4C0.print_const=1;_tmp4C0.q_volatile=0;_tmp4C0.q_restrict=
0;_tmp4C0.real_const=1;_tmp4C0.loc=loc;_tmp4C0;}));break;}case 94: _LL279: yyval=
Cyc_YY25(({struct Cyc_Absyn_Tqual _tmp4C1;_tmp4C1.print_const=0;_tmp4C1.q_volatile=
1;_tmp4C1.q_restrict=0;_tmp4C1.real_const=0;_tmp4C1.loc=0;_tmp4C1;}));break;case
95: _LL27A: yyval=Cyc_YY25(({struct Cyc_Absyn_Tqual _tmp4C2;_tmp4C2.print_const=0;
_tmp4C2.q_volatile=0;_tmp4C2.q_restrict=1;_tmp4C2.real_const=0;_tmp4C2.loc=0;
_tmp4C2;}));break;case 96: _LL27B: yyval=Cyc_YY23((void*)({struct Cyc_Parse_Decl_spec_struct*
_tmp4C3=_cycalloc(sizeof(*_tmp4C3));_tmp4C3[0]=({struct Cyc_Parse_Decl_spec_struct
_tmp4C4;_tmp4C4.tag=5;_tmp4C4.f1=({struct Cyc_Absyn_Decl*_tmp4C5=_cycalloc(
sizeof(*_tmp4C5));_tmp4C5->r=(void*)({struct Cyc_Absyn_Enum_d_struct*_tmp4C6=
_cycalloc(sizeof(*_tmp4C6));_tmp4C6[0]=({struct Cyc_Absyn_Enum_d_struct _tmp4C7;
_tmp4C7.tag=8;_tmp4C7.f1=({struct Cyc_Absyn_Enumdecl*_tmp4C8=_cycalloc(sizeof(*
_tmp4C8));_tmp4C8->sc=(void*)2;_tmp4C8->name=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp4C8->fields=({struct
Cyc_Core_Opt*_tmp4C9=_cycalloc(sizeof(*_tmp4C9));_tmp4C9->v=Cyc_yyget_YY48(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp4C9;});_tmp4C8;});
_tmp4C7;});_tmp4C6;});_tmp4C5->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp4C5;});
_tmp4C4;});_tmp4C3;}));break;case 97: _LL27C: yyval=Cyc_YY23(Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_EnumType_struct*_tmp4CA=_cycalloc(sizeof(*_tmp4CA));
_tmp4CA[0]=({struct Cyc_Absyn_EnumType_struct _tmp4CB;_tmp4CB.tag=12;_tmp4CB.f1=
Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
_tmp4CB.f2=0;_tmp4CB;});_tmp4CA;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 98:
_LL27D: yyval=Cyc_YY23((void*)({struct Cyc_Parse_Type_spec_struct*_tmp4CC=
_cycalloc(sizeof(*_tmp4CC));_tmp4CC[0]=({struct Cyc_Parse_Type_spec_struct _tmp4CD;
_tmp4CD.tag=4;_tmp4CD.f1=(void*)((void*)({struct Cyc_Absyn_AnonEnumType_struct*
_tmp4CE=_cycalloc(sizeof(*_tmp4CE));_tmp4CE[0]=({struct Cyc_Absyn_AnonEnumType_struct
_tmp4CF;_tmp4CF.tag=13;_tmp4CF.f1=Cyc_yyget_YY48(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp4CF;});_tmp4CE;}));
_tmp4CD.f2=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp4CD;});_tmp4CC;}));break;case 99: _LL27E: yyval=Cyc_YY47(({
struct Cyc_Absyn_Enumfield*_tmp4D0=_cycalloc(sizeof(*_tmp4D0));_tmp4D0->name=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp4D0->tag=0;_tmp4D0->loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp4D0;}));break;case 100: _LL27F: yyval=Cyc_YY47(({
struct Cyc_Absyn_Enumfield*_tmp4D1=_cycalloc(sizeof(*_tmp4D1));_tmp4D1->name=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp4D1->tag=(
struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp4D1->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp4D1;}));
break;case 101: _LL280: yyval=Cyc_YY48(({struct Cyc_List_List*_tmp4D2=_cycalloc(
sizeof(*_tmp4D2));_tmp4D2->hd=Cyc_yyget_YY47(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp4D2->tl=0;_tmp4D2;}));break;case 102: _LL281: yyval=Cyc_YY48(({
struct Cyc_List_List*_tmp4D3=_cycalloc(sizeof(*_tmp4D3));_tmp4D3->hd=Cyc_yyget_YY47(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp4D3->tl=0;
_tmp4D3;}));break;case 103: _LL282: yyval=Cyc_YY48(({struct Cyc_List_List*_tmp4D4=
_cycalloc(sizeof(*_tmp4D4));_tmp4D4->hd=Cyc_yyget_YY47(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp4D4->tl=Cyc_yyget_YY48(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp4D4;}));break;case
104: _LL283: yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_AnonAggrType_struct*
_tmp4D5=_cycalloc(sizeof(*_tmp4D5));_tmp4D5[0]=({struct Cyc_Absyn_AnonAggrType_struct
_tmp4D6;_tmp4D6.tag=11;_tmp4D6.f1=(void*)Cyc_yyget_YY24(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp4D6.f2=Cyc_yyget_YY26(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp4D6;});_tmp4D5;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));break;case 105: _LL284: {struct Cyc_List_List*_tmp4D7=((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 5)]).last_line),Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 5)]));struct Cyc_List_List*_tmp4D8=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line),Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]));yyval=Cyc_YY23((
void*)({struct Cyc_Parse_Decl_spec_struct*_tmp4D9=_cycalloc(sizeof(*_tmp4D9));
_tmp4D9[0]=({struct Cyc_Parse_Decl_spec_struct _tmp4DA;_tmp4DA.tag=5;_tmp4DA.f1=
Cyc_Absyn_aggr_decl(Cyc_yyget_YY24(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 7)]),(void*)2,Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),_tmp4D7,(struct Cyc_Absyn_AggrdeclImpl*)
Cyc_Absyn_aggrdecl_impl(_tmp4D8,Cyc_yyget_YY50(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY26(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),1),0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 8)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp4DA;});
_tmp4D9;}));break;}case 106: _LL285: {struct Cyc_List_List*_tmp4DB=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).last_line),Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]));struct Cyc_List_List*
_tmp4DC=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line),Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]));yyval=Cyc_YY23((void*)({struct Cyc_Parse_Decl_spec_struct*
_tmp4DD=_cycalloc(sizeof(*_tmp4DD));_tmp4DD[0]=({struct Cyc_Parse_Decl_spec_struct
_tmp4DE;_tmp4DE.tag=5;_tmp4DE.f1=Cyc_Absyn_aggr_decl(Cyc_yyget_YY24(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 7)]),(void*)2,Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),_tmp4DB,(struct Cyc_Absyn_AggrdeclImpl*)
Cyc_Absyn_aggrdecl_impl(_tmp4DC,Cyc_yyget_YY50(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY26(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0),0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp4DE;});
_tmp4DD;}));break;}case 107: _LL286: yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)({
struct Cyc_Absyn_AggrType_struct*_tmp4DF=_cycalloc(sizeof(*_tmp4DF));_tmp4DF[0]=({
struct Cyc_Absyn_AggrType_struct _tmp4E0;_tmp4E0.tag=10;_tmp4E0.f1=({struct Cyc_Absyn_AggrInfo
_tmp4E1;_tmp4E1.aggr_info=Cyc_Absyn_UnknownAggr(Cyc_yyget_YY24(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),({struct Cyc_Core_Opt*
_tmp4E2=_cycalloc_atomic(sizeof(*_tmp4E2));_tmp4E2->v=(void*)1;_tmp4E2;}));
_tmp4E1.targs=Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp4E1;});_tmp4E0;});_tmp4DF;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 108:
_LL287: yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_AggrType_struct*
_tmp4E3=_cycalloc(sizeof(*_tmp4E3));_tmp4E3[0]=({struct Cyc_Absyn_AggrType_struct
_tmp4E4;_tmp4E4.tag=10;_tmp4E4.f1=({struct Cyc_Absyn_AggrInfo _tmp4E5;_tmp4E5.aggr_info=
Cyc_Absyn_UnknownAggr(Cyc_yyget_YY24(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]),Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]),0);_tmp4E5.targs=Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp4E5;});_tmp4E4;});
_tmp4E3;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));break;case 109: _LL288: yyval=Cyc_YY40(0);break;case
110: _LL289: yyval=Cyc_YY40(Cyc_List_imp_rev(Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));break;case 111: _LL28A:
yyval=Cyc_YY24((void*)0);break;case 112: _LL28B: yyval=Cyc_YY24((void*)1);break;
case 113: _LL28C: yyval=Cyc_YY26(0);break;case 114: _LL28D: yyval=Cyc_YY26(((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_flatten)(((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(Cyc_yyget_YY27(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))));break;case 115: _LL28E: yyval=Cyc_YY27(({struct Cyc_List_List*
_tmp4E6=_cycalloc(sizeof(*_tmp4E6));_tmp4E6->hd=Cyc_yyget_YY26(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp4E6->tl=0;_tmp4E6;}));
break;case 116: _LL28F: yyval=Cyc_YY27(({struct Cyc_List_List*_tmp4E7=_cycalloc(
sizeof(*_tmp4E7));_tmp4E7->hd=Cyc_yyget_YY26(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp4E7->tl=Cyc_yyget_YY27(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp4E7;}));break;case
117: _LL290: yyval=Cyc_YY21(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_YY21(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])));
break;case 118: _LL291: yyval=Cyc_YY21(({struct Cyc_List_List*_tmp4E8=_cycalloc(
sizeof(*_tmp4E8));_tmp4E8->hd=Cyc_yyget_YY20(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp4E8->tl=0;_tmp4E8;}));break;case 119: _LL292: yyval=Cyc_YY21(({
struct Cyc_List_List*_tmp4E9=_cycalloc(sizeof(*_tmp4E9));_tmp4E9->hd=Cyc_yyget_YY20(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp4E9->tl=Cyc_yyget_YY21(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp4E9;}));break;
case 120: _LL293: yyval=Cyc_YY20(({struct _tuple21*_tmp4EA=_cycalloc(sizeof(*_tmp4EA));
_tmp4EA->f1=Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp4EA->f2=0;_tmp4EA;}));break;case 121: _LL294: yyval=Cyc_YY20(({
struct _tuple21*_tmp4EB=_cycalloc(sizeof(*_tmp4EB));_tmp4EB->f1=Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp4EB->f2=(struct
Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp4EB;}));break;case 122: _LL295: {struct _RegionHandle _tmp4EC=
_new_region("temp");struct _RegionHandle*temp=& _tmp4EC;_push_region(temp);{struct
_tuple22 _tmp4EE;struct Cyc_Absyn_Tqual _tmp4EF;struct Cyc_List_List*_tmp4F0;struct
Cyc_List_List*_tmp4F1;struct _tuple22*_tmp4ED=Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp4EE=*_tmp4ED;_tmp4EF=
_tmp4EE.f1;_tmp4F0=_tmp4EE.f2;_tmp4F1=_tmp4EE.f3;if(_tmp4EF.loc == 0)_tmp4EF.loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line);{struct Cyc_List_List*_tmp4F3;struct Cyc_List_List*
_tmp4F4;struct _tuple0 _tmp4F2=((struct _tuple0(*)(struct _RegionHandle*r1,struct
_RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(temp,temp,Cyc_yyget_YY21(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp4F3=_tmp4F2.f1;
_tmp4F4=_tmp4F2.f2;{void*_tmp4F5=Cyc_Parse_speclist2typ(_tmp4F0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line));struct Cyc_List_List*
_tmp4F6=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(temp,temp,Cyc_Parse_apply_tmss(
temp,_tmp4EF,_tmp4F5,_tmp4F3,_tmp4F1),_tmp4F4);yyval=Cyc_YY26(((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Position_Segment*,struct _tuple15*),
struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_make_aggr_field,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line),_tmp4F6));}}}_npop_handler(0);break;;_pop_region(
temp);}case 123: _LL296: yyval=Cyc_YY35(({struct _tuple22*_tmp4F7=_cycalloc(sizeof(*
_tmp4F7));_tmp4F7->f1=Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp4F7->f2=({
struct Cyc_List_List*_tmp4F8=_cycalloc(sizeof(*_tmp4F8));_tmp4F8->hd=(void*)Cyc_yyget_YY23(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp4F8->tl=0;_tmp4F8;});
_tmp4F7->f3=0;_tmp4F7;}));break;case 124: _LL297: yyval=Cyc_YY35(({struct _tuple22*
_tmp4F9=_cycalloc(sizeof(*_tmp4F9));_tmp4F9->f1=(*Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f1;_tmp4F9->f2=({struct Cyc_List_List*
_tmp4FA=_cycalloc(sizeof(*_tmp4FA));_tmp4FA->hd=(void*)Cyc_yyget_YY23(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp4FA->tl=(*Cyc_yyget_YY35(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f2;_tmp4FA;});_tmp4F9->f3=(*
Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f3;
_tmp4F9;}));break;case 125: _LL298: yyval=Cyc_YY35(({struct _tuple22*_tmp4FB=
_cycalloc(sizeof(*_tmp4FB));_tmp4FB->f1=Cyc_yyget_YY25(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp4FB->f2=0;_tmp4FB->f3=0;
_tmp4FB;}));break;case 126: _LL299: yyval=Cyc_YY35(({struct _tuple22*_tmp4FC=
_cycalloc(sizeof(*_tmp4FC));_tmp4FC->f1=Cyc_Absyn_combine_tqual(Cyc_yyget_YY25(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(*Cyc_yyget_YY35(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f1);_tmp4FC->f2=(*Cyc_yyget_YY35(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f2;_tmp4FC->f3=(*Cyc_yyget_YY35(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f3;_tmp4FC;}));break;
case 127: _LL29A: yyval=Cyc_YY35(({struct _tuple22*_tmp4FD=_cycalloc(sizeof(*_tmp4FD));
_tmp4FD->f1=Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp4FD->f2=0;
_tmp4FD->f3=Cyc_yyget_YY45(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp4FD;}));break;case 128: _LL29B: yyval=Cyc_YY35(({struct _tuple22*
_tmp4FE=_cycalloc(sizeof(*_tmp4FE));_tmp4FE->f1=(*Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f1;_tmp4FE->f2=(*Cyc_yyget_YY35(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f2;_tmp4FE->f3=Cyc_List_append(
Cyc_yyget_YY45(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(*
Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f3);
_tmp4FE;}));break;case 129: _LL29C: yyval=Cyc_YY35(({struct _tuple22*_tmp4FF=
_cycalloc(sizeof(*_tmp4FF));_tmp4FF->f1=Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp4FF->f2=({
struct Cyc_List_List*_tmp500=_cycalloc(sizeof(*_tmp500));_tmp500->hd=(void*)Cyc_yyget_YY23(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp500->tl=0;_tmp500;});
_tmp4FF->f3=0;_tmp4FF;}));break;case 130: _LL29D: yyval=Cyc_YY35(({struct _tuple22*
_tmp501=_cycalloc(sizeof(*_tmp501));_tmp501->f1=(*Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f1;_tmp501->f2=({struct Cyc_List_List*
_tmp502=_cycalloc(sizeof(*_tmp502));_tmp502->hd=(void*)Cyc_yyget_YY23(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp502->tl=(*Cyc_yyget_YY35(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f2;_tmp502;});_tmp501->f3=(*
Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f3;
_tmp501;}));break;case 131: _LL29E: yyval=Cyc_YY35(({struct _tuple22*_tmp503=
_cycalloc(sizeof(*_tmp503));_tmp503->f1=Cyc_yyget_YY25(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp503->f2=0;_tmp503->f3=0;
_tmp503;}));break;case 132: _LL29F: yyval=Cyc_YY35(({struct _tuple22*_tmp504=
_cycalloc(sizeof(*_tmp504));_tmp504->f1=Cyc_Absyn_combine_tqual(Cyc_yyget_YY25(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(*Cyc_yyget_YY35(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f1);_tmp504->f2=(*Cyc_yyget_YY35(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f2;_tmp504->f3=(*Cyc_yyget_YY35(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f3;_tmp504;}));break;
case 133: _LL2A0: yyval=Cyc_YY35(({struct _tuple22*_tmp505=_cycalloc(sizeof(*_tmp505));
_tmp505->f1=Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp505->f2=0;
_tmp505->f3=Cyc_yyget_YY45(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp505;}));break;case 134: _LL2A1: yyval=Cyc_YY35(({struct _tuple22*
_tmp506=_cycalloc(sizeof(*_tmp506));_tmp506->f1=(*Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f1;_tmp506->f2=(*Cyc_yyget_YY35(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f2;_tmp506->f3=Cyc_List_append(
Cyc_yyget_YY45(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(*
Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f3);
_tmp506;}));break;case 135: _LL2A2: yyval=Cyc_YY21(((struct Cyc_List_List*(*)(struct
Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY21(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])));break;case 136: _LL2A3:
yyval=Cyc_YY21(({struct Cyc_List_List*_tmp507=_cycalloc(sizeof(*_tmp507));_tmp507->hd=
Cyc_yyget_YY20(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp507->tl=
0;_tmp507;}));break;case 137: _LL2A4: yyval=Cyc_YY21(({struct Cyc_List_List*_tmp508=
_cycalloc(sizeof(*_tmp508));_tmp508->hd=Cyc_yyget_YY20(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp508->tl=Cyc_yyget_YY21(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp508;}));break;
case 138: _LL2A5: yyval=Cyc_YY20(({struct _tuple21*_tmp509=_cycalloc(sizeof(*_tmp509));
_tmp509->f1=Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp509->f2=0;_tmp509;}));break;case 139: _LL2A6: yyval=Cyc_YY20(({
struct _tuple21*_tmp50A=_cycalloc(sizeof(*_tmp50A));_tmp50A->f1=({struct Cyc_Parse_Declarator*
_tmp50B=_cycalloc(sizeof(*_tmp50B));_tmp50B->id=({struct _tuple1*_tmp50C=
_cycalloc(sizeof(*_tmp50C));_tmp50C->f1=Cyc_Absyn_Rel_n(0);_tmp50C->f2=
_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"",sizeof(char),1);
_tmp50C;});_tmp50B->tms=0;_tmp50B;});_tmp50A->f2=(struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp50A;}));break;case
140: _LL2A7: yyval=Cyc_YY20(({struct _tuple21*_tmp50E=_cycalloc(sizeof(*_tmp50E));
_tmp50E->f1=Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]);_tmp50E->f2=(struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp50E;}));break;case 141:
_LL2A8: {int _tmp50F=Cyc_yyget_YY31(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 6)]);struct Cyc_List_List*_tmp510=((struct Cyc_List_List*(*)(struct
Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line),Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]));yyval=Cyc_YY23((
void*)({struct Cyc_Parse_Decl_spec_struct*_tmp511=_cycalloc(sizeof(*_tmp511));
_tmp511[0]=({struct Cyc_Parse_Decl_spec_struct _tmp512;_tmp512.tag=5;_tmp512.f1=
Cyc_Absyn_datatype_decl((void*)2,Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),_tmp510,({struct Cyc_Core_Opt*
_tmp513=_cycalloc(sizeof(*_tmp513));_tmp513->v=Cyc_yyget_YY34(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp513;}),_tmp50F,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp512;});
_tmp511;}));break;}case 142: _LL2A9: {int _tmp514=Cyc_yyget_YY31(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);yyval=Cyc_YY23(Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_DatatypeType_struct*_tmp515=_cycalloc(sizeof(*_tmp515));
_tmp515[0]=({struct Cyc_Absyn_DatatypeType_struct _tmp516;_tmp516.tag=2;_tmp516.f1=({
struct Cyc_Absyn_DatatypeInfo _tmp517;_tmp517.datatype_info=Cyc_Absyn_UnknownDatatype(({
struct Cyc_Absyn_UnknownDatatypeInfo _tmp518;_tmp518.name=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp518.is_extensible=
_tmp514;_tmp518;}));_tmp517.targs=Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp517.rgn=Cyc_yyget_YY49(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp517;});_tmp516;});
_tmp515;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));break;}case 143: _LL2AA: {int _tmp519=Cyc_yyget_YY31(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]);yyval=Cyc_YY23(Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_DatatypeFieldType_struct*_tmp51A=_cycalloc(sizeof(*
_tmp51A));_tmp51A[0]=({struct Cyc_Absyn_DatatypeFieldType_struct _tmp51B;_tmp51B.tag=
3;_tmp51B.f1=({struct Cyc_Absyn_DatatypeFieldInfo _tmp51C;_tmp51C.field_info=Cyc_Absyn_UnknownDatatypefield(({
struct Cyc_Absyn_UnknownDatatypeFieldInfo _tmp51D;_tmp51D.datatype_name=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp51D.field_name=
Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp51D.is_extensible=_tmp519;_tmp51D;}));_tmp51C.targs=Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp51C;});_tmp51B;});
_tmp51A;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));break;}case 144: _LL2AB: yyval=Cyc_YY31(0);break;case
145: _LL2AC: yyval=Cyc_YY31(1);break;case 146: _LL2AD: yyval=Cyc_YY34(({struct Cyc_List_List*
_tmp51E=_cycalloc(sizeof(*_tmp51E));_tmp51E->hd=Cyc_yyget_YY33(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp51E->tl=0;_tmp51E;}));
break;case 147: _LL2AE: yyval=Cyc_YY34(({struct Cyc_List_List*_tmp51F=_cycalloc(
sizeof(*_tmp51F));_tmp51F->hd=Cyc_yyget_YY33(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_tmp51F->tl=0;_tmp51F;}));break;case 148: _LL2AF: yyval=
Cyc_YY34(({struct Cyc_List_List*_tmp520=_cycalloc(sizeof(*_tmp520));_tmp520->hd=
Cyc_yyget_YY33(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);
_tmp520->tl=Cyc_yyget_YY34(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp520;}));break;case 149: _LL2B0: yyval=Cyc_YY34(({struct Cyc_List_List*
_tmp521=_cycalloc(sizeof(*_tmp521));_tmp521->hd=Cyc_yyget_YY33(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp521->tl=Cyc_yyget_YY34(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp521;}));break;case
150: _LL2B1: yyval=Cyc_YY32((void*)2);break;case 151: _LL2B2: yyval=Cyc_YY32((void*)3);
break;case 152: _LL2B3: yyval=Cyc_YY32((void*)0);break;case 153: _LL2B4: yyval=Cyc_YY33(({
struct Cyc_Absyn_Datatypefield*_tmp522=_cycalloc(sizeof(*_tmp522));_tmp522->name=
Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
_tmp522->typs=0;_tmp522->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp522->sc=Cyc_yyget_YY32(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp522;}));break;
case 154: _LL2B5: {struct Cyc_List_List*_tmp523=((struct Cyc_List_List*(*)(struct
_tuple16*(*f)(struct Cyc_Position_Segment*,struct _tuple8*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_get_tqual_typ,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));yyval=Cyc_YY33(({
struct Cyc_Absyn_Datatypefield*_tmp524=_cycalloc(sizeof(*_tmp524));_tmp524->name=
Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);
_tmp524->typs=_tmp523;_tmp524->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp524->sc=Cyc_yyget_YY32(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp524;}));break;}
case 155: _LL2B6: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];
break;case 156: _LL2B7: yyval=Cyc_YY29(({struct Cyc_Parse_Declarator*_tmp525=
_cycalloc(sizeof(*_tmp525));_tmp525->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->id;_tmp525->tms=Cyc_List_imp_append(
Cyc_yyget_YY28(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tms);_tmp525;}));
break;case 157: _LL2B8: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 158: _LL2B9: yyval=Cyc_YY29(({struct Cyc_Parse_Declarator*
_tmp526=_cycalloc(sizeof(*_tmp526));_tmp526->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->id;_tmp526->tms=Cyc_List_imp_append(
Cyc_yyget_YY28(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tms);_tmp526;}));
break;case 159: _LL2BA: yyval=Cyc_YY29(({struct Cyc_Parse_Declarator*_tmp527=
_cycalloc(sizeof(*_tmp527));_tmp527->id=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp527->tms=0;_tmp527;}));
break;case 160: _LL2BB: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)];break;case 161: _LL2BC: {struct Cyc_Parse_Declarator*_tmp528=Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp528->tms=({
struct Cyc_List_List*_tmp529=_cycalloc(sizeof(*_tmp529));_tmp529->hd=(void*)((
void*)({struct Cyc_Absyn_Attributes_mod_struct*_tmp52A=_cycalloc(sizeof(*_tmp52A));
_tmp52A[0]=({struct Cyc_Absyn_Attributes_mod_struct _tmp52B;_tmp52B.tag=5;_tmp52B.f1=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line);_tmp52B.f2=Cyc_yyget_YY45(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp52B;});_tmp52A;}));
_tmp529->tl=_tmp528->tms;_tmp529;});yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)];break;}case 162: _LL2BD: yyval=Cyc_YY29(({struct Cyc_Parse_Declarator*
_tmp52C=_cycalloc(sizeof(*_tmp52C));_tmp52C->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id;_tmp52C->tms=({
struct Cyc_List_List*_tmp52D=_cycalloc(sizeof(*_tmp52D));_tmp52D->hd=(void*)((
void*)({struct Cyc_Absyn_Carray_mod_struct*_tmp52E=_cycalloc(sizeof(*_tmp52E));
_tmp52E[0]=({struct Cyc_Absyn_Carray_mod_struct _tmp52F;_tmp52F.tag=0;_tmp52F.f1=
Cyc_yyget_YY51(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp52F.f2=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp52F;});_tmp52E;}));_tmp52D->tl=(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp52D;});
_tmp52C;}));break;case 163: _LL2BE: yyval=Cyc_YY29(({struct Cyc_Parse_Declarator*
_tmp530=_cycalloc(sizeof(*_tmp530));_tmp530->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->id;_tmp530->tms=({
struct Cyc_List_List*_tmp531=_cycalloc(sizeof(*_tmp531));_tmp531->hd=(void*)((
void*)({struct Cyc_Absyn_ConstArray_mod_struct*_tmp532=_cycalloc(sizeof(*_tmp532));
_tmp532[0]=({struct Cyc_Absyn_ConstArray_mod_struct _tmp533;_tmp533.tag=1;_tmp533.f1=
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);
_tmp533.f2=Cyc_yyget_YY51(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
_tmp533.f3=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp533;});_tmp532;}));_tmp531->tl=(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->tms;_tmp531;});
_tmp530;}));break;case 164: _LL2BF: {struct _tuple23 _tmp535;struct Cyc_List_List*
_tmp536;int _tmp537;struct Cyc_Absyn_VarargInfo*_tmp538;struct Cyc_Core_Opt*_tmp539;
struct Cyc_List_List*_tmp53A;struct _tuple23*_tmp534=Cyc_yyget_YY39(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp535=*_tmp534;_tmp536=
_tmp535.f1;_tmp537=_tmp535.f2;_tmp538=_tmp535.f3;_tmp539=_tmp535.f4;_tmp53A=
_tmp535.f5;yyval=Cyc_YY29(({struct Cyc_Parse_Declarator*_tmp53B=_cycalloc(sizeof(*
_tmp53B));_tmp53B->id=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]))->id;_tmp53B->tms=({struct Cyc_List_List*_tmp53C=_cycalloc(
sizeof(*_tmp53C));_tmp53C->hd=(void*)((void*)({struct Cyc_Absyn_Function_mod_struct*
_tmp53D=_cycalloc(sizeof(*_tmp53D));_tmp53D[0]=({struct Cyc_Absyn_Function_mod_struct
_tmp53E;_tmp53E.tag=3;_tmp53E.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*
_tmp53F=_cycalloc(sizeof(*_tmp53F));_tmp53F[0]=({struct Cyc_Absyn_WithTypes_struct
_tmp540;_tmp540.tag=1;_tmp540.f1=_tmp536;_tmp540.f2=_tmp537;_tmp540.f3=_tmp538;
_tmp540.f4=_tmp539;_tmp540.f5=_tmp53A;_tmp540;});_tmp53F;}));_tmp53E;});_tmp53D;}));
_tmp53C->tl=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]))->tms;_tmp53C;});_tmp53B;}));break;}case 165: _LL2C0: yyval=Cyc_YY29(({
struct Cyc_Parse_Declarator*_tmp541=_cycalloc(sizeof(*_tmp541));_tmp541->id=(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->id;_tmp541->tms=({
struct Cyc_List_List*_tmp542=_cycalloc(sizeof(*_tmp542));_tmp542->hd=(void*)((
void*)({struct Cyc_Absyn_Function_mod_struct*_tmp543=_cycalloc(sizeof(*_tmp543));
_tmp543[0]=({struct Cyc_Absyn_Function_mod_struct _tmp544;_tmp544.tag=3;_tmp544.f1=(
void*)((void*)({struct Cyc_Absyn_WithTypes_struct*_tmp545=_cycalloc(sizeof(*
_tmp545));_tmp545[0]=({struct Cyc_Absyn_WithTypes_struct _tmp546;_tmp546.tag=1;
_tmp546.f1=0;_tmp546.f2=0;_tmp546.f3=0;_tmp546.f4=Cyc_yyget_YY49(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp546.f5=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp546;});_tmp545;}));
_tmp544;});_tmp543;}));_tmp542->tl=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->tms;_tmp542;});_tmp541;}));
break;case 166: _LL2C1: yyval=Cyc_YY29(({struct Cyc_Parse_Declarator*_tmp547=
_cycalloc(sizeof(*_tmp547));_tmp547->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id;_tmp547->tms=({
struct Cyc_List_List*_tmp548=_cycalloc(sizeof(*_tmp548));_tmp548->hd=(void*)((
void*)({struct Cyc_Absyn_Function_mod_struct*_tmp549=_cycalloc(sizeof(*_tmp549));
_tmp549[0]=({struct Cyc_Absyn_Function_mod_struct _tmp54A;_tmp54A.tag=3;_tmp54A.f1=(
void*)((void*)({struct Cyc_Absyn_NoTypes_struct*_tmp54B=_cycalloc(sizeof(*_tmp54B));
_tmp54B[0]=({struct Cyc_Absyn_NoTypes_struct _tmp54C;_tmp54C.tag=0;_tmp54C.f1=Cyc_yyget_YY36(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp54C.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp54C;});
_tmp54B;}));_tmp54A;});_tmp549;}));_tmp548->tl=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp548;});_tmp547;}));
break;case 167: _LL2C2: {struct Cyc_List_List*_tmp54D=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),Cyc_List_imp_rev(
Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));
yyval=Cyc_YY29(({struct Cyc_Parse_Declarator*_tmp54E=_cycalloc(sizeof(*_tmp54E));
_tmp54E->id=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]))->id;_tmp54E->tms=({struct Cyc_List_List*_tmp54F=_cycalloc(
sizeof(*_tmp54F));_tmp54F->hd=(void*)((void*)({struct Cyc_Absyn_TypeParams_mod_struct*
_tmp550=_cycalloc(sizeof(*_tmp550));_tmp550[0]=({struct Cyc_Absyn_TypeParams_mod_struct
_tmp551;_tmp551.tag=4;_tmp551.f1=_tmp54D;_tmp551.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp551.f3=0;
_tmp551;});_tmp550;}));_tmp54F->tl=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp54F;});_tmp54E;}));
break;}case 168: _LL2C3: yyval=Cyc_YY29(({struct Cyc_Parse_Declarator*_tmp552=
_cycalloc(sizeof(*_tmp552));_tmp552->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]))->id;_tmp552->tms=({
struct Cyc_List_List*_tmp553=_cycalloc(sizeof(*_tmp553));_tmp553->hd=(void*)((
void*)({struct Cyc_Absyn_Attributes_mod_struct*_tmp554=_cycalloc(sizeof(*_tmp554));
_tmp554[0]=({struct Cyc_Absyn_Attributes_mod_struct _tmp555;_tmp555.tag=5;_tmp555.f1=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp555.f2=Cyc_yyget_YY45(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp555;});_tmp554;}));
_tmp553->tl=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]))->tms;_tmp553;});_tmp552;}));break;case 169: _LL2C4: yyval=Cyc_YY29(({
struct Cyc_Parse_Declarator*_tmp556=_cycalloc(sizeof(*_tmp556));_tmp556->id=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp556->tms=0;_tmp556;}));
break;case 170: _LL2C5: yyval=Cyc_YY29(({struct Cyc_Parse_Declarator*_tmp557=
_cycalloc(sizeof(*_tmp557));_tmp557->id=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp557->tms=0;_tmp557;}));
break;case 171: _LL2C6: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)];break;case 172: _LL2C7: {struct Cyc_Parse_Declarator*_tmp558=Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp558->tms=({
struct Cyc_List_List*_tmp559=_cycalloc(sizeof(*_tmp559));_tmp559->hd=(void*)((
void*)({struct Cyc_Absyn_Attributes_mod_struct*_tmp55A=_cycalloc(sizeof(*_tmp55A));
_tmp55A[0]=({struct Cyc_Absyn_Attributes_mod_struct _tmp55B;_tmp55B.tag=5;_tmp55B.f1=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line);_tmp55B.f2=Cyc_yyget_YY45(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp55B;});_tmp55A;}));
_tmp559->tl=_tmp558->tms;_tmp559;});yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)];break;}case 173: _LL2C8: yyval=Cyc_YY29(({struct Cyc_Parse_Declarator*
_tmp55C=_cycalloc(sizeof(*_tmp55C));_tmp55C->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id;_tmp55C->tms=({
struct Cyc_List_List*_tmp55D=_cycalloc(sizeof(*_tmp55D));_tmp55D->hd=(void*)((
void*)({struct Cyc_Absyn_Carray_mod_struct*_tmp55E=_cycalloc(sizeof(*_tmp55E));
_tmp55E[0]=({struct Cyc_Absyn_Carray_mod_struct _tmp55F;_tmp55F.tag=0;_tmp55F.f1=
Cyc_yyget_YY51(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp55F.f2=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp55F;});_tmp55E;}));_tmp55D->tl=(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp55D;});
_tmp55C;}));break;case 174: _LL2C9: yyval=Cyc_YY29(({struct Cyc_Parse_Declarator*
_tmp560=_cycalloc(sizeof(*_tmp560));_tmp560->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->id;_tmp560->tms=({
struct Cyc_List_List*_tmp561=_cycalloc(sizeof(*_tmp561));_tmp561->hd=(void*)((
void*)({struct Cyc_Absyn_ConstArray_mod_struct*_tmp562=_cycalloc(sizeof(*_tmp562));
_tmp562[0]=({struct Cyc_Absyn_ConstArray_mod_struct _tmp563;_tmp563.tag=1;_tmp563.f1=
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);
_tmp563.f2=Cyc_yyget_YY51(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
_tmp563.f3=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp563;});_tmp562;}));_tmp561->tl=(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->tms;_tmp561;});
_tmp560;}));break;case 175: _LL2CA: {struct _tuple23 _tmp565;struct Cyc_List_List*
_tmp566;int _tmp567;struct Cyc_Absyn_VarargInfo*_tmp568;struct Cyc_Core_Opt*_tmp569;
struct Cyc_List_List*_tmp56A;struct _tuple23*_tmp564=Cyc_yyget_YY39(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp565=*_tmp564;_tmp566=
_tmp565.f1;_tmp567=_tmp565.f2;_tmp568=_tmp565.f3;_tmp569=_tmp565.f4;_tmp56A=
_tmp565.f5;yyval=Cyc_YY29(({struct Cyc_Parse_Declarator*_tmp56B=_cycalloc(sizeof(*
_tmp56B));_tmp56B->id=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]))->id;_tmp56B->tms=({struct Cyc_List_List*_tmp56C=_cycalloc(
sizeof(*_tmp56C));_tmp56C->hd=(void*)((void*)({struct Cyc_Absyn_Function_mod_struct*
_tmp56D=_cycalloc(sizeof(*_tmp56D));_tmp56D[0]=({struct Cyc_Absyn_Function_mod_struct
_tmp56E;_tmp56E.tag=3;_tmp56E.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*
_tmp56F=_cycalloc(sizeof(*_tmp56F));_tmp56F[0]=({struct Cyc_Absyn_WithTypes_struct
_tmp570;_tmp570.tag=1;_tmp570.f1=_tmp566;_tmp570.f2=_tmp567;_tmp570.f3=_tmp568;
_tmp570.f4=_tmp569;_tmp570.f5=_tmp56A;_tmp570;});_tmp56F;}));_tmp56E;});_tmp56D;}));
_tmp56C->tl=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]))->tms;_tmp56C;});_tmp56B;}));break;}case 176: _LL2CB: yyval=Cyc_YY29(({
struct Cyc_Parse_Declarator*_tmp571=_cycalloc(sizeof(*_tmp571));_tmp571->id=(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->id;_tmp571->tms=({
struct Cyc_List_List*_tmp572=_cycalloc(sizeof(*_tmp572));_tmp572->hd=(void*)((
void*)({struct Cyc_Absyn_Function_mod_struct*_tmp573=_cycalloc(sizeof(*_tmp573));
_tmp573[0]=({struct Cyc_Absyn_Function_mod_struct _tmp574;_tmp574.tag=3;_tmp574.f1=(
void*)((void*)({struct Cyc_Absyn_WithTypes_struct*_tmp575=_cycalloc(sizeof(*
_tmp575));_tmp575[0]=({struct Cyc_Absyn_WithTypes_struct _tmp576;_tmp576.tag=1;
_tmp576.f1=0;_tmp576.f2=0;_tmp576.f3=0;_tmp576.f4=Cyc_yyget_YY49(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp576.f5=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp576;});_tmp575;}));
_tmp574;});_tmp573;}));_tmp572->tl=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->tms;_tmp572;});_tmp571;}));
break;case 177: _LL2CC: yyval=Cyc_YY29(({struct Cyc_Parse_Declarator*_tmp577=
_cycalloc(sizeof(*_tmp577));_tmp577->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id;_tmp577->tms=({
struct Cyc_List_List*_tmp578=_cycalloc(sizeof(*_tmp578));_tmp578->hd=(void*)((
void*)({struct Cyc_Absyn_Function_mod_struct*_tmp579=_cycalloc(sizeof(*_tmp579));
_tmp579[0]=({struct Cyc_Absyn_Function_mod_struct _tmp57A;_tmp57A.tag=3;_tmp57A.f1=(
void*)((void*)({struct Cyc_Absyn_NoTypes_struct*_tmp57B=_cycalloc(sizeof(*_tmp57B));
_tmp57B[0]=({struct Cyc_Absyn_NoTypes_struct _tmp57C;_tmp57C.tag=0;_tmp57C.f1=Cyc_yyget_YY36(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp57C.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp57C;});
_tmp57B;}));_tmp57A;});_tmp579;}));_tmp578->tl=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp578;});_tmp577;}));
break;case 178: _LL2CD: {struct Cyc_List_List*_tmp57D=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),Cyc_List_imp_rev(
Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));
yyval=Cyc_YY29(({struct Cyc_Parse_Declarator*_tmp57E=_cycalloc(sizeof(*_tmp57E));
_tmp57E->id=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]))->id;_tmp57E->tms=({struct Cyc_List_List*_tmp57F=_cycalloc(
sizeof(*_tmp57F));_tmp57F->hd=(void*)((void*)({struct Cyc_Absyn_TypeParams_mod_struct*
_tmp580=_cycalloc(sizeof(*_tmp580));_tmp580[0]=({struct Cyc_Absyn_TypeParams_mod_struct
_tmp581;_tmp581.tag=4;_tmp581.f1=_tmp57D;_tmp581.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp581.f3=0;
_tmp581;});_tmp580;}));_tmp57F->tl=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp57F;});_tmp57E;}));
break;}case 179: _LL2CE: yyval=Cyc_YY29(({struct Cyc_Parse_Declarator*_tmp582=
_cycalloc(sizeof(*_tmp582));_tmp582->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]))->id;_tmp582->tms=({
struct Cyc_List_List*_tmp583=_cycalloc(sizeof(*_tmp583));_tmp583->hd=(void*)((
void*)({struct Cyc_Absyn_Attributes_mod_struct*_tmp584=_cycalloc(sizeof(*_tmp584));
_tmp584[0]=({struct Cyc_Absyn_Attributes_mod_struct _tmp585;_tmp585.tag=5;_tmp585.f1=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp585.f2=Cyc_yyget_YY45(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp585;});_tmp584;}));
_tmp583->tl=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]))->tms;_tmp583;});_tmp582;}));break;case 180: _LL2CF: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 181: _LL2D0: yyval=
Cyc_YY28(Cyc_List_imp_append(Cyc_yyget_YY28(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]),Cyc_yyget_YY28(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)])));break;case 182: _LL2D1: {struct Cyc_List_List*ans=0;if(Cyc_yyget_YY45(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])!= 0)ans=({struct Cyc_List_List*
_tmp586=_cycalloc(sizeof(*_tmp586));_tmp586->hd=(void*)((void*)({struct Cyc_Absyn_Attributes_mod_struct*
_tmp587=_cycalloc(sizeof(*_tmp587));_tmp587[0]=({struct Cyc_Absyn_Attributes_mod_struct
_tmp588;_tmp588.tag=5;_tmp588.f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line);_tmp588.f2=
Cyc_yyget_YY45(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp588;});_tmp587;}));_tmp586->tl=ans;_tmp586;});{struct Cyc_Absyn_PtrLoc*ptrloc=
0;struct Cyc_Position_Segment*_tmp58A;union Cyc_Absyn_Constraint*_tmp58B;union Cyc_Absyn_Constraint*
_tmp58C;struct _tuple18 _tmp589=*Cyc_yyget_YY1(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 4)]);_tmp58A=_tmp589.f1;_tmp58B=_tmp589.f2;_tmp58C=_tmp589.f3;
if(Cyc_Absyn_porting_c_code)ptrloc=({struct Cyc_Absyn_PtrLoc*_tmp58D=_cycalloc(
sizeof(*_tmp58D));_tmp58D->ptr_loc=_tmp58A;_tmp58D->rgn_loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line);_tmp58D->zt_loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line);_tmp58D;});{union Cyc_Absyn_Constraint*_tmp58F;
union Cyc_Absyn_Constraint*_tmp590;union Cyc_Absyn_Constraint*_tmp591;void*_tmp592;
struct _tuple13 _tmp58E=Cyc_Parse_collapse_pointer_quals(_tmp58A,_tmp58B,_tmp58C,
Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY54(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]));_tmp58F=_tmp58E.f1;
_tmp590=_tmp58E.f2;_tmp591=_tmp58E.f3;_tmp592=_tmp58E.f4;ans=({struct Cyc_List_List*
_tmp593=_cycalloc(sizeof(*_tmp593));_tmp593->hd=(void*)((void*)({struct Cyc_Absyn_Pointer_mod_struct*
_tmp594=_cycalloc(sizeof(*_tmp594));_tmp594[0]=({struct Cyc_Absyn_Pointer_mod_struct
_tmp595;_tmp595.tag=2;_tmp595.f1=({struct Cyc_Absyn_PtrAtts _tmp596;_tmp596.rgn=
_tmp592;_tmp596.nullable=_tmp58F;_tmp596.bounds=_tmp590;_tmp596.zero_term=
_tmp591;_tmp596.ptrloc=ptrloc;_tmp596;});_tmp595.f2=Cyc_yyget_YY25(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp595;});_tmp594;}));
_tmp593->tl=ans;_tmp593;});yyval=Cyc_YY28(ans);break;}}}case 183: _LL2D2: yyval=Cyc_YY54(
0);break;case 184: _LL2D3: yyval=Cyc_YY54(({struct Cyc_List_List*_tmp597=_cycalloc(
sizeof(*_tmp597));_tmp597->hd=(void*)Cyc_yyget_YY53(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp597->tl=Cyc_yyget_YY54(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp597;}));break;case
185: _LL2D4: yyval=Cyc_YY53((void*)({struct Cyc_Absyn_Numelts_ptrqual_struct*
_tmp598=_cycalloc(sizeof(*_tmp598));_tmp598[0]=({struct Cyc_Absyn_Numelts_ptrqual_struct
_tmp599;_tmp599.tag=0;_tmp599.f1=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp599;});_tmp598;}));
break;case 186: _LL2D5: yyval=Cyc_YY53((void*)({struct Cyc_Absyn_Region_ptrqual_struct*
_tmp59A=_cycalloc(sizeof(*_tmp59A));_tmp59A[0]=({struct Cyc_Absyn_Region_ptrqual_struct
_tmp59B;_tmp59B.tag=1;_tmp59B.f1=(void*)Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp59B;});_tmp59A;}));
break;case 187: _LL2D6: yyval=Cyc_YY53((void*)0);break;case 188: _LL2D7: yyval=Cyc_YY53((
void*)1);break;case 189: _LL2D8: yyval=Cyc_YY53((void*)2);break;case 190: _LL2D9:
yyval=Cyc_YY53((void*)3);break;case 191: _LL2DA: yyval=Cyc_YY53((void*)4);break;
case 192: _LL2DB: yyval=Cyc_YY53((void*)5);break;case 193: _LL2DC: {struct Cyc_Position_Segment*
loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line);yyval=Cyc_YY1(({struct _tuple18*_tmp59C=_cycalloc(
sizeof(*_tmp59C));_tmp59C->f1=loc;_tmp59C->f2=Cyc_Absyn_true_conref;_tmp59C->f3=
Cyc_yyget_YY2(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp59C;}));
break;}case 194: _LL2DD: {struct Cyc_Position_Segment*loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line);yyval=Cyc_YY1(({
struct _tuple18*_tmp59D=_cycalloc(sizeof(*_tmp59D));_tmp59D->f1=loc;_tmp59D->f2=
Cyc_Absyn_false_conref;_tmp59D->f3=Cyc_yyget_YY2(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp59D;}));break;}case 195:
_LL2DE: {struct Cyc_Position_Segment*loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);yyval=Cyc_YY1(({
struct _tuple18*_tmp59E=_cycalloc(sizeof(*_tmp59E));_tmp59E->f1=loc;_tmp59E->f2=
Cyc_Absyn_true_conref;_tmp59E->f3=Cyc_Absyn_bounds_dyneither_conref;_tmp59E;}));
break;}case 196: _LL2DF: yyval=Cyc_YY2(Cyc_Absyn_bounds_one_conref);break;case 197:
_LL2E0: yyval=Cyc_YY2(Cyc_Absyn_new_conref((void*)({struct Cyc_Absyn_Upper_b_struct*
_tmp59F=_cycalloc(sizeof(*_tmp59F));_tmp59F[0]=({struct Cyc_Absyn_Upper_b_struct
_tmp5A0;_tmp5A0.tag=0;_tmp5A0.f1=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp5A0;});_tmp59F;})));
break;case 198: _LL2E1: yyval=Cyc_YY51(((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)());
break;case 199: _LL2E2: yyval=Cyc_YY51(Cyc_Absyn_true_conref);break;case 200: _LL2E3:
yyval=Cyc_YY51(Cyc_Absyn_false_conref);break;case 201: _LL2E4: yyval=Cyc_YY49(0);
break;case 202: _LL2E5: Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),(void*)3,1);yyval=Cyc_YY49(({
struct Cyc_Core_Opt*_tmp5A1=_cycalloc(sizeof(*_tmp5A1));_tmp5A1->v=(void*)Cyc_yyget_YY44(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5A1;}));break;case
203: _LL2E6: yyval=Cyc_YY49(({struct Cyc_Core_Opt*_tmp5A2=_cycalloc(sizeof(*_tmp5A2));
_tmp5A2->v=(void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,0);
_tmp5A2;}));break;case 204: _LL2E7: yyval=Cyc_YY44(Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_trk,0));break;case 205: _LL2E8: Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY44(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),(void*)5,1);yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 206: _LL2E9: yyval=
Cyc_YY44(Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_trk,0));break;case
207: _LL2EA: yyval=Cyc_YY25(Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset + 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset + 1)]).last_line)));break;case
208: _LL2EB: yyval=Cyc_YY25(Cyc_Absyn_combine_tqual(Cyc_yyget_YY25(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY25(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])));break;case 209: _LL2EC:
yyval=Cyc_YY39(({struct _tuple23*_tmp5A3=_cycalloc(sizeof(*_tmp5A3));_tmp5A3->f1=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]));_tmp5A3->f2=0;
_tmp5A3->f3=0;_tmp5A3->f4=Cyc_yyget_YY49(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_tmp5A3->f5=Cyc_yyget_YY50(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5A3;}));break;case 210:
_LL2ED: yyval=Cyc_YY39(({struct _tuple23*_tmp5A4=_cycalloc(sizeof(*_tmp5A4));
_tmp5A4->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_YY38(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]));
_tmp5A4->f2=1;_tmp5A4->f3=0;_tmp5A4->f4=Cyc_yyget_YY49(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp5A4->f5=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5A4;}));break;case
211: _LL2EE: {struct _tuple8 _tmp5A6;struct Cyc_Core_Opt*_tmp5A7;struct Cyc_Absyn_Tqual
_tmp5A8;void*_tmp5A9;struct _tuple8*_tmp5A5=Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp5A6=*_tmp5A5;_tmp5A7=
_tmp5A6.f1;_tmp5A8=_tmp5A6.f2;_tmp5A9=_tmp5A6.f3;{struct Cyc_Absyn_VarargInfo*
_tmp5AA=({struct Cyc_Absyn_VarargInfo*_tmp5AC=_cycalloc(sizeof(*_tmp5AC));_tmp5AC->name=
_tmp5A7;_tmp5AC->tq=_tmp5A8;_tmp5AC->type=_tmp5A9;_tmp5AC->inject=Cyc_yyget_YY31(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp5AC;});yyval=
Cyc_YY39(({struct _tuple23*_tmp5AB=_cycalloc(sizeof(*_tmp5AB));_tmp5AB->f1=0;
_tmp5AB->f2=0;_tmp5AB->f3=_tmp5AA;_tmp5AB->f4=Cyc_yyget_YY49(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp5AB->f5=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5AB;}));break;}}
case 212: _LL2EF: {struct _tuple8 _tmp5AE;struct Cyc_Core_Opt*_tmp5AF;struct Cyc_Absyn_Tqual
_tmp5B0;void*_tmp5B1;struct _tuple8*_tmp5AD=Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp5AE=*_tmp5AD;_tmp5AF=
_tmp5AE.f1;_tmp5B0=_tmp5AE.f2;_tmp5B1=_tmp5AE.f3;{struct Cyc_Absyn_VarargInfo*
_tmp5B2=({struct Cyc_Absyn_VarargInfo*_tmp5B4=_cycalloc(sizeof(*_tmp5B4));_tmp5B4->name=
_tmp5AF;_tmp5B4->tq=_tmp5B0;_tmp5B4->type=_tmp5B1;_tmp5B4->inject=Cyc_yyget_YY31(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp5B4;});yyval=
Cyc_YY39(({struct _tuple23*_tmp5B3=_cycalloc(sizeof(*_tmp5B3));_tmp5B3->f1=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]));_tmp5B3->f2=0;
_tmp5B3->f3=_tmp5B2;_tmp5B3->f4=Cyc_yyget_YY49(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp5B3->f5=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5B3;}));break;}}
case 213: _LL2F0: yyval=Cyc_YY44(Cyc_Parse_id2type(Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),(void*)({struct Cyc_Absyn_Unknown_kb_struct*
_tmp5B5=_cycalloc(sizeof(*_tmp5B5));_tmp5B5[0]=({struct Cyc_Absyn_Unknown_kb_struct
_tmp5B6;_tmp5B6.tag=1;_tmp5B6.f1=0;_tmp5B6;});_tmp5B5;})));break;case 214: _LL2F1:
yyval=Cyc_YY44(Cyc_Parse_id2type(Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_Tcutil_kind_to_bound(
Cyc_yyget_YY43(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))));
break;case 215: _LL2F2: yyval=Cyc_YY49(0);break;case 216: _LL2F3: yyval=Cyc_YY49(({
struct Cyc_Core_Opt*_tmp5B7=_cycalloc(sizeof(*_tmp5B7));_tmp5B7->v=(void*)((void*)({
struct Cyc_Absyn_JoinEff_struct*_tmp5B8=_cycalloc(sizeof(*_tmp5B8));_tmp5B8[0]=({
struct Cyc_Absyn_JoinEff_struct _tmp5B9;_tmp5B9.tag=20;_tmp5B9.f1=Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5B9;});_tmp5B8;}));
_tmp5B7;}));break;case 217: _LL2F4: yyval=Cyc_YY50(0);break;case 218: _LL2F5: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 219: _LL2F6: {
struct Cyc_Absyn_Less_kb_struct*_tmp5BA=({struct Cyc_Absyn_Less_kb_struct*_tmp5C1=
_cycalloc(sizeof(*_tmp5C1));_tmp5C1[0]=({struct Cyc_Absyn_Less_kb_struct _tmp5C2;
_tmp5C2.tag=2;_tmp5C2.f1=0;_tmp5C2.f2=(void*)((void*)5);_tmp5C2;});_tmp5C1;});
struct _dyneither_ptr _tmp5BB=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);void*_tmp5BC=Cyc_Parse_id2type(
_tmp5BB,(void*)_tmp5BA);yyval=Cyc_YY50(({struct Cyc_List_List*_tmp5BD=_cycalloc(
sizeof(*_tmp5BD));_tmp5BD->hd=({struct _tuple10*_tmp5BE=_cycalloc(sizeof(*_tmp5BE));
_tmp5BE->f1=(void*)({struct Cyc_Absyn_JoinEff_struct*_tmp5BF=_cycalloc(sizeof(*
_tmp5BF));_tmp5BF[0]=({struct Cyc_Absyn_JoinEff_struct _tmp5C0;_tmp5C0.tag=20;
_tmp5C0.f1=Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset
- 2)]);_tmp5C0;});_tmp5BF;});_tmp5BE->f2=_tmp5BC;_tmp5BE;});_tmp5BD->tl=0;
_tmp5BD;}));break;}case 220: _LL2F7: {struct Cyc_Absyn_Less_kb_struct*_tmp5C3=({
struct Cyc_Absyn_Less_kb_struct*_tmp5CA=_cycalloc(sizeof(*_tmp5CA));_tmp5CA[0]=({
struct Cyc_Absyn_Less_kb_struct _tmp5CB;_tmp5CB.tag=2;_tmp5CB.f1=0;_tmp5CB.f2=(
void*)((void*)5);_tmp5CB;});_tmp5CA;});struct _dyneither_ptr _tmp5C4=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);void*_tmp5C5=Cyc_Parse_id2type(
_tmp5C4,(void*)_tmp5C3);yyval=Cyc_YY50(({struct Cyc_List_List*_tmp5C6=_cycalloc(
sizeof(*_tmp5C6));_tmp5C6->hd=({struct _tuple10*_tmp5C7=_cycalloc(sizeof(*_tmp5C7));
_tmp5C7->f1=(void*)({struct Cyc_Absyn_JoinEff_struct*_tmp5C8=_cycalloc(sizeof(*
_tmp5C8));_tmp5C8[0]=({struct Cyc_Absyn_JoinEff_struct _tmp5C9;_tmp5C9.tag=20;
_tmp5C9.f1=Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset
- 4)]);_tmp5C9;});_tmp5C8;});_tmp5C7->f2=_tmp5C5;_tmp5C7;});_tmp5C6->tl=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5C6;}));break;}
case 221: _LL2F8: yyval=Cyc_YY31(0);break;case 222: _LL2F9: if(Cyc_zstrcmp((struct
_dyneither_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),({const char*_tmp5CC="inject";_tag_dyneither(_tmp5CC,sizeof(char),
7);}))!= 0)Cyc_Parse_err(({const char*_tmp5CD="missing type in function declaration";
_tag_dyneither(_tmp5CD,sizeof(char),37);}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=Cyc_YY31(
1);break;case 223: _LL2FA: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 224: _LL2FB: yyval=Cyc_YY40(Cyc_List_imp_append(Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])));break;case 225: _LL2FC:
yyval=Cyc_YY40(0);break;case 226: _LL2FD: yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)];break;case 227: _LL2FE: yyval=Cyc_YY40(({struct Cyc_List_List*
_tmp5CE=_cycalloc(sizeof(*_tmp5CE));_tmp5CE->hd=(void*)((void*)({struct Cyc_Absyn_RgnsEff_struct*
_tmp5CF=_cycalloc(sizeof(*_tmp5CF));_tmp5CF[0]=({struct Cyc_Absyn_RgnsEff_struct
_tmp5D0;_tmp5D0.tag=21;_tmp5D0.f1=(void*)Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp5D0;});_tmp5CF;}));
_tmp5CE->tl=0;_tmp5CE;}));break;case 228: _LL2FF: Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY44(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),(void*)6,0);yyval=Cyc_YY40(({
struct Cyc_List_List*_tmp5D1=_cycalloc(sizeof(*_tmp5D1));_tmp5D1->hd=(void*)Cyc_yyget_YY44(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5D1->tl=0;_tmp5D1;}));
break;case 229: _LL300: Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),(void*)5,1);yyval=Cyc_YY40(({
struct Cyc_List_List*_tmp5D2=_cycalloc(sizeof(*_tmp5D2));_tmp5D2->hd=(void*)((
void*)({struct Cyc_Absyn_AccessEff_struct*_tmp5D3=_cycalloc(sizeof(*_tmp5D3));
_tmp5D3[0]=({struct Cyc_Absyn_AccessEff_struct _tmp5D4;_tmp5D4.tag=19;_tmp5D4.f1=(
void*)Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
_tmp5D4;});_tmp5D3;}));_tmp5D2->tl=0;_tmp5D2;}));break;case 230: _LL301: Cyc_Parse_set_vartyp_kind(
Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),(
void*)5,1);yyval=Cyc_YY40(({struct Cyc_List_List*_tmp5D5=_cycalloc(sizeof(*
_tmp5D5));_tmp5D5->hd=(void*)((void*)({struct Cyc_Absyn_AccessEff_struct*_tmp5D6=
_cycalloc(sizeof(*_tmp5D6));_tmp5D6[0]=({struct Cyc_Absyn_AccessEff_struct _tmp5D7;
_tmp5D7.tag=19;_tmp5D7.f1=(void*)Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp5D7;});_tmp5D6;}));
_tmp5D5->tl=Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp5D5;}));break;case 231: _LL302: yyval=Cyc_YY38(({struct Cyc_List_List*
_tmp5D8=_cycalloc(sizeof(*_tmp5D8));_tmp5D8->hd=Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5D8->tl=0;_tmp5D8;}));
break;case 232: _LL303: yyval=Cyc_YY38(({struct Cyc_List_List*_tmp5D9=_cycalloc(
sizeof(*_tmp5D9));_tmp5D9->hd=Cyc_yyget_YY37(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp5D9->tl=Cyc_yyget_YY38(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp5D9;}));break;case
233: _LL304: {struct _tuple22 _tmp5DB;struct Cyc_Absyn_Tqual _tmp5DC;struct Cyc_List_List*
_tmp5DD;struct Cyc_List_List*_tmp5DE;struct _tuple22*_tmp5DA=Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp5DB=*_tmp5DA;_tmp5DC=
_tmp5DB.f1;_tmp5DD=_tmp5DB.f2;_tmp5DE=_tmp5DB.f3;if(_tmp5DC.loc == 0)_tmp5DC.loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line);{struct Cyc_Parse_Declarator _tmp5E0;struct _tuple1*
_tmp5E1;struct Cyc_List_List*_tmp5E2;struct Cyc_Parse_Declarator*_tmp5DF=Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5E0=*_tmp5DF;
_tmp5E1=_tmp5E0.id;_tmp5E2=_tmp5E0.tms;{void*_tmp5E3=Cyc_Parse_speclist2typ(
_tmp5DD,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));struct Cyc_Absyn_Tqual _tmp5E5;void*_tmp5E6;struct
Cyc_List_List*_tmp5E7;struct Cyc_List_List*_tmp5E8;struct _tuple12 _tmp5E4=Cyc_Parse_apply_tms(
_tmp5DC,_tmp5E3,_tmp5DE,_tmp5E2);_tmp5E5=_tmp5E4.f1;_tmp5E6=_tmp5E4.f2;_tmp5E7=
_tmp5E4.f3;_tmp5E8=_tmp5E4.f4;if(_tmp5E7 != 0)Cyc_Parse_err(({const char*_tmp5E9="parameter with bad type params";
_tag_dyneither(_tmp5E9,sizeof(char),31);}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));if(Cyc_Absyn_is_qvar_qualified(
_tmp5E1))Cyc_Parse_err(({const char*_tmp5EA="parameter cannot be qualified with a namespace";
_tag_dyneither(_tmp5EA,sizeof(char),47);}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));{struct Cyc_Core_Opt*
_tmp5EB=(struct Cyc_Core_Opt*)({struct Cyc_Core_Opt*_tmp5EF=_cycalloc(sizeof(*
_tmp5EF));_tmp5EF->v=(*_tmp5E1).f2;_tmp5EF;});if(_tmp5E8 != 0)({void*_tmp5EC=0;
Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),({const char*_tmp5ED="extra attributes on parameter, ignoring";
_tag_dyneither(_tmp5ED,sizeof(char),40);}),_tag_dyneither(_tmp5EC,sizeof(void*),
0));});yyval=Cyc_YY37(({struct _tuple8*_tmp5EE=_cycalloc(sizeof(*_tmp5EE));
_tmp5EE->f1=_tmp5EB;_tmp5EE->f2=_tmp5E5;_tmp5EE->f3=_tmp5E6;_tmp5EE;}));break;}}}}
case 234: _LL305: {struct _tuple22 _tmp5F1;struct Cyc_Absyn_Tqual _tmp5F2;struct Cyc_List_List*
_tmp5F3;struct Cyc_List_List*_tmp5F4;struct _tuple22*_tmp5F0=Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5F1=*_tmp5F0;_tmp5F2=
_tmp5F1.f1;_tmp5F3=_tmp5F1.f2;_tmp5F4=_tmp5F1.f3;if(_tmp5F2.loc == 0)_tmp5F2.loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);{void*_tmp5F5=Cyc_Parse_speclist2typ(_tmp5F3,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));if(_tmp5F4 != 0)({
void*_tmp5F6=0;Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),({const char*
_tmp5F7="bad attributes on parameter, ignoring";_tag_dyneither(_tmp5F7,sizeof(
char),38);}),_tag_dyneither(_tmp5F6,sizeof(void*),0));});yyval=Cyc_YY37(({struct
_tuple8*_tmp5F8=_cycalloc(sizeof(*_tmp5F8));_tmp5F8->f1=0;_tmp5F8->f2=_tmp5F2;
_tmp5F8->f3=_tmp5F5;_tmp5F8;}));break;}}case 235: _LL306: {struct _tuple22 _tmp5FA;
struct Cyc_Absyn_Tqual _tmp5FB;struct Cyc_List_List*_tmp5FC;struct Cyc_List_List*
_tmp5FD;struct _tuple22*_tmp5F9=Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp5FA=*_tmp5F9;_tmp5FB=
_tmp5FA.f1;_tmp5FC=_tmp5FA.f2;_tmp5FD=_tmp5FA.f3;if(_tmp5FB.loc == 0)_tmp5FB.loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line);{void*_tmp5FE=Cyc_Parse_speclist2typ(_tmp5FC,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));struct Cyc_List_List*
_tmp5FF=(Cyc_yyget_YY30(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tms;
struct Cyc_Absyn_Tqual _tmp601;void*_tmp602;struct Cyc_List_List*_tmp603;struct Cyc_List_List*
_tmp604;struct _tuple12 _tmp600=Cyc_Parse_apply_tms(_tmp5FB,_tmp5FE,_tmp5FD,
_tmp5FF);_tmp601=_tmp600.f1;_tmp602=_tmp600.f2;_tmp603=_tmp600.f3;_tmp604=
_tmp600.f4;if(_tmp603 != 0)({void*_tmp605=0;Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),({const char*
_tmp606="bad type parameters on formal argument, ignoring";_tag_dyneither(
_tmp606,sizeof(char),49);}),_tag_dyneither(_tmp605,sizeof(void*),0));});if(
_tmp604 != 0)({void*_tmp607=0;Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),({const char*
_tmp608="bad attributes on parameter, ignoring";_tag_dyneither(_tmp608,sizeof(
char),38);}),_tag_dyneither(_tmp607,sizeof(void*),0));});yyval=Cyc_YY37(({struct
_tuple8*_tmp609=_cycalloc(sizeof(*_tmp609));_tmp609->f1=0;_tmp609->f2=_tmp601;
_tmp609->f3=_tmp602;_tmp609;}));break;}}case 236: _LL307: yyval=Cyc_YY36(((struct
Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY36(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])));break;case 237: _LL308:
yyval=Cyc_YY36(({struct Cyc_List_List*_tmp60A=_cycalloc(sizeof(*_tmp60A));_tmp60A->hd=({
struct _dyneither_ptr*_tmp60B=_cycalloc(sizeof(*_tmp60B));_tmp60B[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp60B;});_tmp60A->tl=
0;_tmp60A;}));break;case 238: _LL309: yyval=Cyc_YY36(({struct Cyc_List_List*_tmp60C=
_cycalloc(sizeof(*_tmp60C));_tmp60C->hd=({struct _dyneither_ptr*_tmp60D=_cycalloc(
sizeof(*_tmp60D));_tmp60D[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp60D;});_tmp60C->tl=Cyc_yyget_YY36(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp60C;}));break;
case 239: _LL30A: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];
break;case 240: _LL30B: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 241: _LL30C: yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)({
struct Cyc_Absyn_UnresolvedMem_e_struct*_tmp60E=_cycalloc(sizeof(*_tmp60E));
_tmp60E[0]=({struct Cyc_Absyn_UnresolvedMem_e_struct _tmp60F;_tmp60F.tag=37;
_tmp60F.f1=0;_tmp60F.f2=0;_tmp60F;});_tmp60E;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 242:
_LL30D: yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_UnresolvedMem_e_struct*
_tmp610=_cycalloc(sizeof(*_tmp610));_tmp610[0]=({struct Cyc_Absyn_UnresolvedMem_e_struct
_tmp611;_tmp611.tag=37;_tmp611.f1=0;_tmp611.f2=((struct Cyc_List_List*(*)(struct
Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY6(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp611;});_tmp610;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));break;case 243: _LL30E: yyval=Cyc_YY3(Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_UnresolvedMem_e_struct*_tmp612=_cycalloc(sizeof(*_tmp612));
_tmp612[0]=({struct Cyc_Absyn_UnresolvedMem_e_struct _tmp613;_tmp613.tag=37;
_tmp613.f1=0;_tmp613.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_YY6(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]));
_tmp613;});_tmp612;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 244:
_LL30F: {struct Cyc_Absyn_Vardecl*_tmp614=Cyc_Absyn_new_vardecl(({struct _tuple1*
_tmp617=_cycalloc(sizeof(*_tmp617));_tmp617->f1=Cyc_Absyn_Loc_n;_tmp617->f2=({
struct _dyneither_ptr*_tmp618=_cycalloc(sizeof(*_tmp618));_tmp618[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]);_tmp618;});_tmp617;}),
Cyc_Absyn_uint_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).last_line)));(_tmp614->tq).real_const=
1;yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Comprehension_e_struct*
_tmp615=_cycalloc(sizeof(*_tmp615));_tmp615[0]=({struct Cyc_Absyn_Comprehension_e_struct
_tmp616;_tmp616.tag=29;_tmp616.f1=_tmp614;_tmp616.f2=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp616.f3=Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp616.f4=0;
_tmp616;});_tmp615;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;}case 245:
_LL310: yyval=Cyc_YY6(({struct Cyc_List_List*_tmp619=_cycalloc(sizeof(*_tmp619));
_tmp619->hd=({struct _tuple26*_tmp61A=_cycalloc(sizeof(*_tmp61A));_tmp61A->f1=0;
_tmp61A->f2=Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
_tmp61A;});_tmp619->tl=0;_tmp619;}));break;case 246: _LL311: yyval=Cyc_YY6(({struct
Cyc_List_List*_tmp61B=_cycalloc(sizeof(*_tmp61B));_tmp61B->hd=({struct _tuple26*
_tmp61C=_cycalloc(sizeof(*_tmp61C));_tmp61C->f1=Cyc_yyget_YY41(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp61C->f2=Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp61C;});_tmp61B->tl=
0;_tmp61B;}));break;case 247: _LL312: yyval=Cyc_YY6(({struct Cyc_List_List*_tmp61D=
_cycalloc(sizeof(*_tmp61D));_tmp61D->hd=({struct _tuple26*_tmp61E=_cycalloc(
sizeof(*_tmp61E));_tmp61E->f1=0;_tmp61E->f2=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp61E;});_tmp61D->tl=Cyc_yyget_YY6(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp61D;}));break;
case 248: _LL313: yyval=Cyc_YY6(({struct Cyc_List_List*_tmp61F=_cycalloc(sizeof(*
_tmp61F));_tmp61F->hd=({struct _tuple26*_tmp620=_cycalloc(sizeof(*_tmp620));
_tmp620->f1=Cyc_yyget_YY41(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp620->f2=Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp620;});_tmp61F->tl=Cyc_yyget_YY6(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp61F;}));break;case
249: _LL314: yyval=Cyc_YY41(Cyc_List_imp_rev(Cyc_yyget_YY41(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));break;case 250: _LL315:
yyval=Cyc_YY41(({struct Cyc_List_List*_tmp621=_cycalloc(sizeof(*_tmp621));_tmp621->hd=(
void*)Cyc_yyget_YY42(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
_tmp621->tl=0;_tmp621;}));break;case 251: _LL316: yyval=Cyc_YY41(({struct Cyc_List_List*
_tmp622=_cycalloc(sizeof(*_tmp622));_tmp622->hd=(void*)Cyc_yyget_YY42(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp622->tl=Cyc_yyget_YY41(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp622;}));break;
case 252: _LL317: yyval=Cyc_YY42((void*)({struct Cyc_Absyn_ArrayElement_struct*
_tmp623=_cycalloc(sizeof(*_tmp623));_tmp623[0]=({struct Cyc_Absyn_ArrayElement_struct
_tmp624;_tmp624.tag=0;_tmp624.f1=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp624;});_tmp623;}));
break;case 253: _LL318: yyval=Cyc_YY42((void*)({struct Cyc_Absyn_FieldName_struct*
_tmp625=_cycalloc(sizeof(*_tmp625));_tmp625[0]=({struct Cyc_Absyn_FieldName_struct
_tmp626;_tmp626.tag=1;_tmp626.f1=({struct _dyneither_ptr*_tmp627=_cycalloc(
sizeof(*_tmp627));_tmp627[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp627;});_tmp626;});
_tmp625;}));break;case 254: _LL319: {struct _tuple22 _tmp629;struct Cyc_Absyn_Tqual
_tmp62A;struct Cyc_List_List*_tmp62B;struct Cyc_List_List*_tmp62C;struct _tuple22*
_tmp628=Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
_tmp629=*_tmp628;_tmp62A=_tmp629.f1;_tmp62B=_tmp629.f2;_tmp62C=_tmp629.f3;{void*
_tmp62D=Cyc_Parse_speclist2typ(_tmp62B,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));if(_tmp62C != 0)({
void*_tmp62E=0;Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),({const char*
_tmp62F="ignoring attributes in type";_tag_dyneither(_tmp62F,sizeof(char),28);}),
_tag_dyneither(_tmp62E,sizeof(void*),0));});yyval=Cyc_YY37(({struct _tuple8*
_tmp630=_cycalloc(sizeof(*_tmp630));_tmp630->f1=0;_tmp630->f2=_tmp62A;_tmp630->f3=
_tmp62D;_tmp630;}));break;}}case 255: _LL31A: {struct _tuple22 _tmp632;struct Cyc_Absyn_Tqual
_tmp633;struct Cyc_List_List*_tmp634;struct Cyc_List_List*_tmp635;struct _tuple22*
_tmp631=Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp632=*_tmp631;_tmp633=_tmp632.f1;_tmp634=_tmp632.f2;_tmp635=_tmp632.f3;{void*
_tmp636=Cyc_Parse_speclist2typ(_tmp634,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));struct Cyc_List_List*
_tmp637=(Cyc_yyget_YY30(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tms;
struct _tuple12 _tmp638=Cyc_Parse_apply_tms(_tmp633,_tmp636,_tmp635,_tmp637);if(
_tmp638.f3 != 0)({void*_tmp639=0;Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),({const char*
_tmp63A="bad type params, ignoring";_tag_dyneither(_tmp63A,sizeof(char),26);}),
_tag_dyneither(_tmp639,sizeof(void*),0));});if(_tmp638.f4 != 0)({void*_tmp63B=0;
Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),({const char*_tmp63C="bad specifiers, ignoring";
_tag_dyneither(_tmp63C,sizeof(char),25);}),_tag_dyneither(_tmp63B,sizeof(void*),
0));});yyval=Cyc_YY37(({struct _tuple8*_tmp63D=_cycalloc(sizeof(*_tmp63D));
_tmp63D->f1=0;_tmp63D->f2=_tmp638.f1;_tmp63D->f3=_tmp638.f2;_tmp63D;}));break;}}
case 256: _LL31B: yyval=Cyc_YY44((*Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f3);break;case 257: _LL31C:
yyval=Cyc_YY44((void*)({struct Cyc_Absyn_JoinEff_struct*_tmp63E=_cycalloc(sizeof(*
_tmp63E));_tmp63E[0]=({struct Cyc_Absyn_JoinEff_struct _tmp63F;_tmp63F.tag=20;
_tmp63F.f1=0;_tmp63F;});_tmp63E;}));break;case 258: _LL31D: yyval=Cyc_YY44((void*)({
struct Cyc_Absyn_JoinEff_struct*_tmp640=_cycalloc(sizeof(*_tmp640));_tmp640[0]=({
struct Cyc_Absyn_JoinEff_struct _tmp641;_tmp641.tag=20;_tmp641.f1=Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp641;});_tmp640;}));
break;case 259: _LL31E: yyval=Cyc_YY44((void*)({struct Cyc_Absyn_RgnsEff_struct*
_tmp642=_cycalloc(sizeof(*_tmp642));_tmp642[0]=({struct Cyc_Absyn_RgnsEff_struct
_tmp643;_tmp643.tag=21;_tmp643.f1=(void*)Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp643;});_tmp642;}));
break;case 260: _LL31F: yyval=Cyc_YY44((void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp644=_cycalloc(sizeof(*_tmp644));_tmp644[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp645;_tmp645.tag=20;_tmp645.f1=({struct Cyc_List_List*_tmp646=_cycalloc(
sizeof(*_tmp646));_tmp646->hd=(void*)Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp646->tl=Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp646;});_tmp645;});
_tmp644;}));break;case 261: _LL320: yyval=Cyc_YY40(({struct Cyc_List_List*_tmp647=
_cycalloc(sizeof(*_tmp647));_tmp647->hd=(void*)Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp647->tl=0;_tmp647;}));
break;case 262: _LL321: yyval=Cyc_YY40(({struct Cyc_List_List*_tmp648=_cycalloc(
sizeof(*_tmp648));_tmp648->hd=(void*)Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp648->tl=Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp648;}));break;
case 263: _LL322: yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator*_tmp649=
_cycalloc(sizeof(*_tmp649));_tmp649->tms=Cyc_yyget_YY28(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp649;}));break;case 264:
_LL323: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
265: _LL324: yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator*_tmp64A=_cycalloc(
sizeof(*_tmp64A));_tmp64A->tms=Cyc_List_imp_append(Cyc_yyget_YY28(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_YY30(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->tms);_tmp64A;}));break;
case 266: _LL325: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)];
break;case 267: _LL326: yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator*_tmp64B=
_cycalloc(sizeof(*_tmp64B));_tmp64B->tms=({struct Cyc_List_List*_tmp64C=_cycalloc(
sizeof(*_tmp64C));_tmp64C->hd=(void*)((void*)({struct Cyc_Absyn_Carray_mod_struct*
_tmp64D=_cycalloc(sizeof(*_tmp64D));_tmp64D[0]=({struct Cyc_Absyn_Carray_mod_struct
_tmp64E;_tmp64E.tag=0;_tmp64E.f1=Cyc_yyget_YY51(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp64E.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp64E;});
_tmp64D;}));_tmp64C->tl=0;_tmp64C;});_tmp64B;}));break;case 268: _LL327: yyval=Cyc_YY30(({
struct Cyc_Parse_Abstractdeclarator*_tmp64F=_cycalloc(sizeof(*_tmp64F));_tmp64F->tms=({
struct Cyc_List_List*_tmp650=_cycalloc(sizeof(*_tmp650));_tmp650->hd=(void*)((
void*)({struct Cyc_Absyn_Carray_mod_struct*_tmp651=_cycalloc(sizeof(*_tmp651));
_tmp651[0]=({struct Cyc_Absyn_Carray_mod_struct _tmp652;_tmp652.tag=0;_tmp652.f1=
Cyc_yyget_YY51(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp652.f2=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp652;});_tmp651;}));_tmp650->tl=(Cyc_yyget_YY30(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp650;});
_tmp64F;}));break;case 269: _LL328: yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator*
_tmp653=_cycalloc(sizeof(*_tmp653));_tmp653->tms=({struct Cyc_List_List*_tmp654=
_cycalloc(sizeof(*_tmp654));_tmp654->hd=(void*)((void*)({struct Cyc_Absyn_ConstArray_mod_struct*
_tmp655=_cycalloc(sizeof(*_tmp655));_tmp655[0]=({struct Cyc_Absyn_ConstArray_mod_struct
_tmp656;_tmp656.tag=1;_tmp656.f1=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp656.f2=Cyc_yyget_YY51(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp656.f3=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp656;});
_tmp655;}));_tmp654->tl=0;_tmp654;});_tmp653;}));break;case 270: _LL329: yyval=Cyc_YY30(({
struct Cyc_Parse_Abstractdeclarator*_tmp657=_cycalloc(sizeof(*_tmp657));_tmp657->tms=({
struct Cyc_List_List*_tmp658=_cycalloc(sizeof(*_tmp658));_tmp658->hd=(void*)((
void*)({struct Cyc_Absyn_ConstArray_mod_struct*_tmp659=_cycalloc(sizeof(*_tmp659));
_tmp659[0]=({struct Cyc_Absyn_ConstArray_mod_struct _tmp65A;_tmp65A.tag=1;_tmp65A.f1=
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);
_tmp65A.f2=Cyc_yyget_YY51(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
_tmp65A.f3=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp65A;});_tmp659;}));_tmp658->tl=(Cyc_yyget_YY30(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->tms;_tmp658;});
_tmp657;}));break;case 271: _LL32A: yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator*
_tmp65B=_cycalloc(sizeof(*_tmp65B));_tmp65B->tms=({struct Cyc_List_List*_tmp65C=
_cycalloc(sizeof(*_tmp65C));_tmp65C->hd=(void*)((void*)({struct Cyc_Absyn_Function_mod_struct*
_tmp65D=_cycalloc(sizeof(*_tmp65D));_tmp65D[0]=({struct Cyc_Absyn_Function_mod_struct
_tmp65E;_tmp65E.tag=3;_tmp65E.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*
_tmp65F=_cycalloc(sizeof(*_tmp65F));_tmp65F[0]=({struct Cyc_Absyn_WithTypes_struct
_tmp660;_tmp660.tag=1;_tmp660.f1=0;_tmp660.f2=0;_tmp660.f3=0;_tmp660.f4=Cyc_yyget_YY49(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp660.f5=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp660;});_tmp65F;}));
_tmp65E;});_tmp65D;}));_tmp65C->tl=0;_tmp65C;});_tmp65B;}));break;case 272: _LL32B: {
struct _tuple23 _tmp662;struct Cyc_List_List*_tmp663;int _tmp664;struct Cyc_Absyn_VarargInfo*
_tmp665;struct Cyc_Core_Opt*_tmp666;struct Cyc_List_List*_tmp667;struct _tuple23*
_tmp661=Cyc_yyget_YY39(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp662=*_tmp661;_tmp663=_tmp662.f1;_tmp664=_tmp662.f2;_tmp665=_tmp662.f3;
_tmp666=_tmp662.f4;_tmp667=_tmp662.f5;yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator*
_tmp668=_cycalloc(sizeof(*_tmp668));_tmp668->tms=({struct Cyc_List_List*_tmp669=
_cycalloc(sizeof(*_tmp669));_tmp669->hd=(void*)((void*)({struct Cyc_Absyn_Function_mod_struct*
_tmp66A=_cycalloc(sizeof(*_tmp66A));_tmp66A[0]=({struct Cyc_Absyn_Function_mod_struct
_tmp66B;_tmp66B.tag=3;_tmp66B.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*
_tmp66C=_cycalloc(sizeof(*_tmp66C));_tmp66C[0]=({struct Cyc_Absyn_WithTypes_struct
_tmp66D;_tmp66D.tag=1;_tmp66D.f1=_tmp663;_tmp66D.f2=_tmp664;_tmp66D.f3=_tmp665;
_tmp66D.f4=_tmp666;_tmp66D.f5=_tmp667;_tmp66D;});_tmp66C;}));_tmp66B;});_tmp66A;}));
_tmp669->tl=0;_tmp669;});_tmp668;}));break;}case 273: _LL32C: yyval=Cyc_YY30(({
struct Cyc_Parse_Abstractdeclarator*_tmp66E=_cycalloc(sizeof(*_tmp66E));_tmp66E->tms=({
struct Cyc_List_List*_tmp66F=_cycalloc(sizeof(*_tmp66F));_tmp66F->hd=(void*)((
void*)({struct Cyc_Absyn_Function_mod_struct*_tmp670=_cycalloc(sizeof(*_tmp670));
_tmp670[0]=({struct Cyc_Absyn_Function_mod_struct _tmp671;_tmp671.tag=3;_tmp671.f1=(
void*)((void*)({struct Cyc_Absyn_WithTypes_struct*_tmp672=_cycalloc(sizeof(*
_tmp672));_tmp672[0]=({struct Cyc_Absyn_WithTypes_struct _tmp673;_tmp673.tag=1;
_tmp673.f1=0;_tmp673.f2=0;_tmp673.f3=0;_tmp673.f4=Cyc_yyget_YY49(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp673.f5=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp673;});_tmp672;}));
_tmp671;});_tmp670;}));_tmp66F->tl=(Cyc_yyget_YY30(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->tms;_tmp66F;});_tmp66E;}));
break;case 274: _LL32D: {struct _tuple23 _tmp675;struct Cyc_List_List*_tmp676;int
_tmp677;struct Cyc_Absyn_VarargInfo*_tmp678;struct Cyc_Core_Opt*_tmp679;struct Cyc_List_List*
_tmp67A;struct _tuple23*_tmp674=Cyc_yyget_YY39(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp675=*_tmp674;_tmp676=
_tmp675.f1;_tmp677=_tmp675.f2;_tmp678=_tmp675.f3;_tmp679=_tmp675.f4;_tmp67A=
_tmp675.f5;yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator*_tmp67B=_cycalloc(
sizeof(*_tmp67B));_tmp67B->tms=({struct Cyc_List_List*_tmp67C=_cycalloc(sizeof(*
_tmp67C));_tmp67C->hd=(void*)((void*)({struct Cyc_Absyn_Function_mod_struct*
_tmp67D=_cycalloc(sizeof(*_tmp67D));_tmp67D[0]=({struct Cyc_Absyn_Function_mod_struct
_tmp67E;_tmp67E.tag=3;_tmp67E.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*
_tmp67F=_cycalloc(sizeof(*_tmp67F));_tmp67F[0]=({struct Cyc_Absyn_WithTypes_struct
_tmp680;_tmp680.tag=1;_tmp680.f1=_tmp676;_tmp680.f2=_tmp677;_tmp680.f3=_tmp678;
_tmp680.f4=_tmp679;_tmp680.f5=_tmp67A;_tmp680;});_tmp67F;}));_tmp67E;});_tmp67D;}));
_tmp67C->tl=(Cyc_yyget_YY30(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]))->tms;_tmp67C;});_tmp67B;}));break;}case 275: _LL32E: {struct
Cyc_List_List*_tmp681=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct
Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*
x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),Cyc_List_imp_rev(
Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));
yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator*_tmp682=_cycalloc(sizeof(*
_tmp682));_tmp682->tms=({struct Cyc_List_List*_tmp683=_cycalloc(sizeof(*_tmp683));
_tmp683->hd=(void*)((void*)({struct Cyc_Absyn_TypeParams_mod_struct*_tmp684=
_cycalloc(sizeof(*_tmp684));_tmp684[0]=({struct Cyc_Absyn_TypeParams_mod_struct
_tmp685;_tmp685.tag=4;_tmp685.f1=_tmp681;_tmp685.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp685.f3=0;
_tmp685;});_tmp684;}));_tmp683->tl=(Cyc_yyget_YY30(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp683;});_tmp682;}));
break;}case 276: _LL32F: yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator*
_tmp686=_cycalloc(sizeof(*_tmp686));_tmp686->tms=({struct Cyc_List_List*_tmp687=
_cycalloc(sizeof(*_tmp687));_tmp687->hd=(void*)((void*)({struct Cyc_Absyn_Attributes_mod_struct*
_tmp688=_cycalloc(sizeof(*_tmp688));_tmp688[0]=({struct Cyc_Absyn_Attributes_mod_struct
_tmp689;_tmp689.tag=5;_tmp689.f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp689.f2=Cyc_yyget_YY45(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp689;});_tmp688;}));
_tmp687->tl=(Cyc_yyget_YY30(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]))->tms;_tmp687;});_tmp686;}));break;case 277: _LL330: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 278: _LL331: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 279: _LL332:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 280:
_LL333: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
281: _LL334: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;
case 282: _LL335: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];
break;case 283: _LL336: yyval=Cyc_YY9(Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_ResetRegion_s_struct*
_tmp68A=_cycalloc(sizeof(*_tmp68A));_tmp68A[0]=({struct Cyc_Absyn_ResetRegion_s_struct
_tmp68B;_tmp68B.tag=15;_tmp68B.f1=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp68B;});_tmp68A;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));break;case 284: _LL337: yyval=Cyc_YY4(0);break;case 285:
_LL338: if(Cyc_zstrcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),({const char*_tmp68C="open";
_tag_dyneither(_tmp68C,sizeof(char),5);}))!= 0)Cyc_Parse_err(({const char*_tmp68D="expecting `open'";
_tag_dyneither(_tmp68D,sizeof(char),17);}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));yyval=Cyc_YY4((
struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]));break;case 286: _LL339: yyval=Cyc_YY9(Cyc_Absyn_new_stmt((void*)({
struct Cyc_Absyn_Label_s_struct*_tmp68E=_cycalloc(sizeof(*_tmp68E));_tmp68E[0]=({
struct Cyc_Absyn_Label_s_struct _tmp68F;_tmp68F.tag=12;_tmp68F.f1=({struct
_dyneither_ptr*_tmp690=_cycalloc(sizeof(*_tmp690));_tmp690[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp690;});_tmp68F.f2=
Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp68F;});
_tmp68E;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));break;case 287: _LL33A: yyval=Cyc_YY9(Cyc_Absyn_skip_stmt(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));break;case 288: _LL33B: yyval=Cyc_YY9(Cyc_Absyn_exp_stmt(
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 289:
_LL33C: yyval=Cyc_YY9(Cyc_Absyn_skip_stmt(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 290:
_LL33D: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;
case 291: _LL33E: yyval=Cyc_YY9(Cyc_Parse_flatten_declarations(Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Absyn_skip_stmt(0)));
break;case 292: _LL33F: yyval=Cyc_YY9(Cyc_Parse_flatten_declarations(Cyc_yyget_YY18(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])));break;case 293: _LL340:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 294:
_LL341: yyval=Cyc_YY9(Cyc_Absyn_seq_stmt(Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 295:
_LL342: yyval=Cyc_YY9(Cyc_Parse_flatten_decl(Cyc_Absyn_new_decl((void*)({struct
Cyc_Absyn_Fn_d_struct*_tmp691=_cycalloc(sizeof(*_tmp691));_tmp691[0]=({struct Cyc_Absyn_Fn_d_struct
_tmp692;_tmp692.tag=1;_tmp692.f1=Cyc_yyget_YY17(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp692;});_tmp691;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),Cyc_Absyn_skip_stmt(
0)));break;case 296: _LL343: yyval=Cyc_YY9(Cyc_Parse_flatten_decl(Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Fn_d_struct*_tmp693=_cycalloc(sizeof(*_tmp693));_tmp693[
0]=({struct Cyc_Absyn_Fn_d_struct _tmp694;_tmp694.tag=1;_tmp694.f1=Cyc_yyget_YY17(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp694;});_tmp693;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line)),Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)])));break;case 297: _LL344: yyval=Cyc_YY9(Cyc_Absyn_ifthenelse_stmt(
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Absyn_skip_stmt(0),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));break;case 298: _LL345: yyval=Cyc_YY9(Cyc_Absyn_ifthenelse_stmt(
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 299:
_LL346: yyval=Cyc_YY9(Cyc_Absyn_switch_stmt(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_YY10(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 300:
_LL347: {struct Cyc_Absyn_Exp*_tmp695=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_UnknownId_e_struct*
_tmp696=_cycalloc(sizeof(*_tmp696));_tmp696[0]=({struct Cyc_Absyn_UnknownId_e_struct
_tmp697;_tmp697.tag=2;_tmp697.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp697;});_tmp696;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line));yyval=Cyc_YY9(Cyc_Absyn_switch_stmt(_tmp695,Cyc_yyget_YY10(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;}case 301:
_LL348: {struct Cyc_Absyn_Exp*_tmp698=Cyc_Absyn_tuple_exp(Cyc_yyget_YY5(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));yyval=Cyc_YY9(
Cyc_Absyn_switch_stmt(_tmp698,Cyc_yyget_YY10(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;}case 302:
_LL349: yyval=Cyc_YY9(Cyc_Absyn_trycatch_stmt(Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_YY10(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 303:
_LL34A: yyval=Cyc_YY10(0);break;case 304: _LL34B: yyval=Cyc_YY10(({struct Cyc_List_List*
_tmp699=_cycalloc(sizeof(*_tmp699));_tmp699->hd=({struct Cyc_Absyn_Switch_clause*
_tmp69A=_cycalloc(sizeof(*_tmp69A));_tmp69A->pattern=Cyc_Absyn_new_pat((void*)0,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line));_tmp69A->pat_vars=0;_tmp69A->where_clause=0;
_tmp69A->body=Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp69A->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp69A;});
_tmp699->tl=0;_tmp699;}));break;case 305: _LL34C: yyval=Cyc_YY10(({struct Cyc_List_List*
_tmp69B=_cycalloc(sizeof(*_tmp69B));_tmp69B->hd=({struct Cyc_Absyn_Switch_clause*
_tmp69C=_cycalloc(sizeof(*_tmp69C));_tmp69C->pattern=Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp69C->pat_vars=0;
_tmp69C->where_clause=0;_tmp69C->body=Cyc_Absyn_fallthru_stmt(0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp69C->loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp69C;});_tmp69B->tl=Cyc_yyget_YY10(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp69B;}));break;case 306:
_LL34D: yyval=Cyc_YY10(({struct Cyc_List_List*_tmp69D=_cycalloc(sizeof(*_tmp69D));
_tmp69D->hd=({struct Cyc_Absyn_Switch_clause*_tmp69E=_cycalloc(sizeof(*_tmp69E));
_tmp69E->pattern=Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]);_tmp69E->pat_vars=0;_tmp69E->where_clause=0;_tmp69E->body=Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp69E->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line);_tmp69E;});
_tmp69D->tl=Cyc_yyget_YY10(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp69D;}));break;case 307: _LL34E: yyval=Cyc_YY10(({struct Cyc_List_List*
_tmp69F=_cycalloc(sizeof(*_tmp69F));_tmp69F->hd=({struct Cyc_Absyn_Switch_clause*
_tmp6A0=_cycalloc(sizeof(*_tmp6A0));_tmp6A0->pattern=Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp6A0->pat_vars=0;
_tmp6A0->where_clause=(struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp6A0->body=Cyc_Absyn_fallthru_stmt(
0,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));_tmp6A0->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp6A0;});
_tmp69F->tl=Cyc_yyget_YY10(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp69F;}));break;case 308: _LL34F: yyval=Cyc_YY10(({struct Cyc_List_List*
_tmp6A1=_cycalloc(sizeof(*_tmp6A1));_tmp6A1->hd=({struct Cyc_Absyn_Switch_clause*
_tmp6A2=_cycalloc(sizeof(*_tmp6A2));_tmp6A2->pattern=Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 5)]);_tmp6A2->pat_vars=0;
_tmp6A2->where_clause=(struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp6A2->body=Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp6A2->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp6A2;});
_tmp6A1->tl=Cyc_yyget_YY10(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp6A1;}));break;case 309: _LL350: yyval=Cyc_YY9(Cyc_Absyn_while_stmt(
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 310:
_LL351: yyval=Cyc_YY9(Cyc_Absyn_do_stmt(Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 5)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 311:
_LL352: yyval=Cyc_YY9(Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(
0),Cyc_Absyn_false_exp(0),Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 312:
_LL353: yyval=Cyc_YY9(Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(
0),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 313:
_LL354: yyval=Cyc_YY9(Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_Absyn_false_exp(
0),Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 314:
_LL355: yyval=Cyc_YY9(Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line)));break;case
315: _LL356: yyval=Cyc_YY9(Cyc_Absyn_for_stmt(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_Absyn_true_exp(0),
Cyc_Absyn_false_exp(0),Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 6)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));break;case 316: _LL357: yyval=Cyc_YY9(Cyc_Absyn_for_stmt(
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]),Cyc_Absyn_true_exp(
0),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 317:
_LL358: yyval=Cyc_YY9(Cyc_Absyn_for_stmt(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 5)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_Absyn_false_exp(0),
Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 318:
_LL359: yyval=Cyc_YY9(Cyc_Absyn_for_stmt(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 8)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 319:
_LL35A: {struct Cyc_List_List*_tmp6A3=Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);struct Cyc_Absyn_Stmt*
_tmp6A4=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(0),Cyc_Absyn_false_exp(
0),Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=Cyc_YY9(
Cyc_Parse_flatten_declarations(_tmp6A3,_tmp6A4));break;}case 320: _LL35B: {struct
Cyc_List_List*_tmp6A5=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)]);struct Cyc_Absyn_Stmt*_tmp6A6=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(
0),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),
Cyc_Absyn_false_exp(0),Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 6)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));yyval=Cyc_YY9(Cyc_Parse_flatten_declarations(_tmp6A5,
_tmp6A6));break;}case 321: _LL35C: {struct Cyc_List_List*_tmp6A7=Cyc_yyget_YY18(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);struct Cyc_Absyn_Stmt*
_tmp6A8=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(0),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=Cyc_YY9(
Cyc_Parse_flatten_declarations(_tmp6A7,_tmp6A8));break;}case 322: _LL35D: {struct
Cyc_List_List*_tmp6A9=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 5)]);struct Cyc_Absyn_Stmt*_tmp6AA=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(
0),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=Cyc_YY9(
Cyc_Parse_flatten_declarations(_tmp6A9,_tmp6AA));break;}case 323: _LL35E: yyval=Cyc_YY9(
Cyc_Absyn_goto_stmt(({struct _dyneither_ptr*_tmp6AB=_cycalloc(sizeof(*_tmp6AB));
_tmp6AB[0]=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp6AB;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line)));break;case
324: _LL35F: yyval=Cyc_YY9(Cyc_Absyn_continue_stmt(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line)));break;case
325: _LL360: yyval=Cyc_YY9(Cyc_Absyn_break_stmt(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line)));break;case
326: _LL361: yyval=Cyc_YY9(Cyc_Absyn_return_stmt(0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line)));break;case
327: _LL362: yyval=Cyc_YY9(Cyc_Absyn_return_stmt((struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line)));break;case
328: _LL363: yyval=Cyc_YY9(Cyc_Absyn_fallthru_stmt(0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line)));break;case
329: _LL364: yyval=Cyc_YY9(Cyc_Absyn_fallthru_stmt(0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line)));break;case
330: _LL365: yyval=Cyc_YY9(Cyc_Absyn_fallthru_stmt(Cyc_yyget_YY5(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).last_line)));break;case
331: _LL366: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;
case 332: _LL367: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];
break;case 333: _LL368: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_conditional_exp(
Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line))));break;case 334: _LL369: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 335: _LL36A: yyval=
Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_or_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY11(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))));break;case 336:
_LL36B: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
337: _LL36C: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_and_exp(Cyc_Parse_pat2exp(
Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))));break;case 338:
_LL36D: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
339: _LL36E: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((void*)14,Cyc_Parse_pat2exp(
Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))));break;case 340:
_LL36F: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
341: _LL370: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((void*)15,Cyc_Parse_pat2exp(
Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))));break;case 342:
_LL371: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
343: _LL372: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((void*)13,Cyc_Parse_pat2exp(
Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))));break;case 344:
_LL373: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
345: _LL374: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_eq_exp(Cyc_Parse_pat2exp(
Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))));break;case 346:
_LL375: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_neq_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY11(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))));break;case 347:
_LL376: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
348: _LL377: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_lt_exp(Cyc_Parse_pat2exp(
Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))));break;case 349:
_LL378: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_gt_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY11(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))));break;case 350:
_LL379: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_lte_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY11(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))));break;case 351:
_LL37A: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_gte_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY11(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))));break;case 352:
_LL37B: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
353: _LL37C: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((void*)16,Cyc_Parse_pat2exp(
Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))));break;case 354:
_LL37D: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((void*)17,Cyc_Parse_pat2exp(
Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))));break;case 355:
_LL37E: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
356: _LL37F: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((void*)0,Cyc_Parse_pat2exp(
Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))));break;case 357:
_LL380: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((void*)2,Cyc_Parse_pat2exp(
Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))));break;case 358:
_LL381: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
359: _LL382: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((void*)1,Cyc_Parse_pat2exp(
Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))));break;case 360:
_LL383: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((void*)3,Cyc_Parse_pat2exp(
Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))));break;case 361:
_LL384: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((void*)4,Cyc_Parse_pat2exp(
Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))));break;case 362:
_LL385: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
363: _LL386: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_cast_exp((*Cyc_yyget_YY37(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])).f3,Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),1,(void*)0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))));break;case 364:
_LL387: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
365: _LL388: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])));break;case 366: _LL389:
yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_prim1_exp(Cyc_yyget_YY7(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))));break;case 367:
_LL38A: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_sizeoftyp_exp((*Cyc_yyget_YY37(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])).f3,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))));break;case 368:
_LL38B: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_sizeofexp_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))));break;case 369:
_LL38C: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_offsetof_exp((*Cyc_yyget_YY37(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)])).f3,(void*)({struct
Cyc_Absyn_StructField_struct*_tmp6AC=_cycalloc(sizeof(*_tmp6AC));_tmp6AC[0]=({
struct Cyc_Absyn_StructField_struct _tmp6AD;_tmp6AD.tag=0;_tmp6AD.f1=({struct
_dyneither_ptr*_tmp6AE=_cycalloc(sizeof(*_tmp6AE));_tmp6AE[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp6AE;});_tmp6AD;});
_tmp6AC;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line))));break;case 370: _LL38D: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(
Cyc_Absyn_offsetof_exp((*Cyc_yyget_YY37(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)])).f3,(void*)({struct Cyc_Absyn_TupleIndex_struct*_tmp6AF=
_cycalloc_atomic(sizeof(*_tmp6AF));_tmp6AF[0]=({struct Cyc_Absyn_TupleIndex_struct
_tmp6B0;_tmp6B0.tag=1;_tmp6B0.f1=(unsigned int)(Cyc_yyget_Int_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])).f2;_tmp6B0;});_tmp6AF;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line))));break;case 371: _LL38E: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 372: _LL38F: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 373: _LL390:
yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)0,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 374:
_LL391: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));break;case 375: _LL392: {
struct Cyc_Absyn_Exp*e=Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);{void*_tmp6B1=e->r;union Cyc_Absyn_Cnst _tmp6B2;struct _tuple4
_tmp6B3;void*_tmp6B4;char _tmp6B5;union Cyc_Absyn_Cnst _tmp6B6;struct _tuple5 _tmp6B7;
void*_tmp6B8;short _tmp6B9;union Cyc_Absyn_Cnst _tmp6BA;struct _tuple6 _tmp6BB;void*
_tmp6BC;int _tmp6BD;union Cyc_Absyn_Cnst _tmp6BE;struct _dyneither_ptr _tmp6BF;union
Cyc_Absyn_Cnst _tmp6C0;int _tmp6C1;union Cyc_Absyn_Cnst _tmp6C2;struct _dyneither_ptr
_tmp6C3;union Cyc_Absyn_Cnst _tmp6C4;struct _tuple7 _tmp6C5;_LL395: if(*((int*)
_tmp6B1)!= 0)goto _LL397;_tmp6B2=((struct Cyc_Absyn_Const_e_struct*)_tmp6B1)->f1;
if((_tmp6B2.Char_c).tag != 2)goto _LL397;_tmp6B3=(struct _tuple4)(_tmp6B2.Char_c).val;
_tmp6B4=_tmp6B3.f1;_tmp6B5=_tmp6B3.f2;_LL396: yyval=Cyc_YY11(Cyc_Absyn_new_pat((
void*)({struct Cyc_Absyn_Char_p_struct*_tmp6C6=_cycalloc_atomic(sizeof(*_tmp6C6));
_tmp6C6[0]=({struct Cyc_Absyn_Char_p_struct _tmp6C7;_tmp6C7.tag=8;_tmp6C7.f1=
_tmp6B5;_tmp6C7;});_tmp6C6;}),e->loc));goto _LL394;_LL397: if(*((int*)_tmp6B1)!= 0)
goto _LL399;_tmp6B6=((struct Cyc_Absyn_Const_e_struct*)_tmp6B1)->f1;if((_tmp6B6.Short_c).tag
!= 3)goto _LL399;_tmp6B7=(struct _tuple5)(_tmp6B6.Short_c).val;_tmp6B8=_tmp6B7.f1;
_tmp6B9=_tmp6B7.f2;_LL398: yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Int_p_struct*
_tmp6C8=_cycalloc(sizeof(*_tmp6C8));_tmp6C8[0]=({struct Cyc_Absyn_Int_p_struct
_tmp6C9;_tmp6C9.tag=7;_tmp6C9.f1=(void*)_tmp6B8;_tmp6C9.f2=(int)_tmp6B9;_tmp6C9;});
_tmp6C8;}),e->loc));goto _LL394;_LL399: if(*((int*)_tmp6B1)!= 0)goto _LL39B;_tmp6BA=((
struct Cyc_Absyn_Const_e_struct*)_tmp6B1)->f1;if((_tmp6BA.Int_c).tag != 4)goto
_LL39B;_tmp6BB=(struct _tuple6)(_tmp6BA.Int_c).val;_tmp6BC=_tmp6BB.f1;_tmp6BD=
_tmp6BB.f2;_LL39A: yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Int_p_struct*
_tmp6CA=_cycalloc(sizeof(*_tmp6CA));_tmp6CA[0]=({struct Cyc_Absyn_Int_p_struct
_tmp6CB;_tmp6CB.tag=7;_tmp6CB.f1=(void*)_tmp6BC;_tmp6CB.f2=_tmp6BD;_tmp6CB;});
_tmp6CA;}),e->loc));goto _LL394;_LL39B: if(*((int*)_tmp6B1)!= 0)goto _LL39D;_tmp6BE=((
struct Cyc_Absyn_Const_e_struct*)_tmp6B1)->f1;if((_tmp6BE.Float_c).tag != 6)goto
_LL39D;_tmp6BF=(struct _dyneither_ptr)(_tmp6BE.Float_c).val;_LL39C: yyval=Cyc_YY11(
Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Float_p_struct*_tmp6CC=_cycalloc(
sizeof(*_tmp6CC));_tmp6CC[0]=({struct Cyc_Absyn_Float_p_struct _tmp6CD;_tmp6CD.tag=
9;_tmp6CD.f1=_tmp6BF;_tmp6CD;});_tmp6CC;}),e->loc));goto _LL394;_LL39D: if(*((int*)
_tmp6B1)!= 0)goto _LL39F;_tmp6C0=((struct Cyc_Absyn_Const_e_struct*)_tmp6B1)->f1;
if((_tmp6C0.Null_c).tag != 1)goto _LL39F;_tmp6C1=(int)(_tmp6C0.Null_c).val;_LL39E:
yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)1,e->loc));goto _LL394;_LL39F: if(*((int*)
_tmp6B1)!= 0)goto _LL3A1;_tmp6C2=((struct Cyc_Absyn_Const_e_struct*)_tmp6B1)->f1;
if((_tmp6C2.String_c).tag != 7)goto _LL3A1;_tmp6C3=(struct _dyneither_ptr)(_tmp6C2.String_c).val;
_LL3A0: Cyc_Parse_err(({const char*_tmp6CE="strings cannot occur within patterns";
_tag_dyneither(_tmp6CE,sizeof(char),37);}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));goto _LL394;
_LL3A1: if(*((int*)_tmp6B1)!= 0)goto _LL3A3;_tmp6C4=((struct Cyc_Absyn_Const_e_struct*)
_tmp6B1)->f1;if((_tmp6C4.LongLong_c).tag != 5)goto _LL3A3;_tmp6C5=(struct _tuple7)(
_tmp6C4.LongLong_c).val;_LL3A2: Cyc_Parse_unimp(({const char*_tmp6CF="long long's in patterns";
_tag_dyneither(_tmp6CF,sizeof(char),24);}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));goto _LL394;
_LL3A3:;_LL3A4: Cyc_Parse_err(({const char*_tmp6D0="bad constant in case";
_tag_dyneither(_tmp6D0,sizeof(char),21);}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_LL394:;}break;}
case 376: _LL393: yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_UnknownId_p_struct*
_tmp6D1=_cycalloc(sizeof(*_tmp6D1));_tmp6D1[0]=({struct Cyc_Absyn_UnknownId_p_struct
_tmp6D2;_tmp6D2.tag=12;_tmp6D2.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6D2;});_tmp6D1;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 377:
_LL3A5: if(Cyc_strcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),({const char*_tmp6D3="as";
_tag_dyneither(_tmp6D3,sizeof(char),3);}))!= 0)Cyc_Parse_err(({const char*_tmp6D4="expecting `as'";
_tag_dyneither(_tmp6D4,sizeof(char),15);}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));yyval=Cyc_YY11(
Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Var_p_struct*_tmp6D5=_cycalloc(
sizeof(*_tmp6D5));_tmp6D5[0]=({struct Cyc_Absyn_Var_p_struct _tmp6D6;_tmp6D6.tag=0;
_tmp6D6.f1=Cyc_Absyn_new_vardecl(({struct _tuple1*_tmp6D7=_cycalloc(sizeof(*
_tmp6D7));_tmp6D7->f1=Cyc_Absyn_Loc_n;_tmp6D7->f2=({struct _dyneither_ptr*_tmp6D8=
_cycalloc(sizeof(*_tmp6D8));_tmp6D8[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp6D8;});_tmp6D7;}),(
void*)0,0);_tmp6D6.f2=Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp6D6;});_tmp6D5;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line)));break;case
378: _LL3A6: {struct Cyc_List_List*_tmp6DA;int _tmp6DB;struct _tuple19 _tmp6D9=*Cyc_yyget_YY12(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp6DA=_tmp6D9.f1;
_tmp6DB=_tmp6D9.f2;yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Tuple_p_struct*
_tmp6DC=_cycalloc(sizeof(*_tmp6DC));_tmp6DC[0]=({struct Cyc_Absyn_Tuple_p_struct
_tmp6DD;_tmp6DD.tag=3;_tmp6DD.f1=_tmp6DA;_tmp6DD.f2=_tmp6DB;_tmp6DD;});_tmp6DC;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));break;}case 379: _LL3A7: {struct Cyc_List_List*_tmp6DF;
int _tmp6E0;struct _tuple19 _tmp6DE=*Cyc_yyget_YY12(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp6DF=_tmp6DE.f1;
_tmp6E0=_tmp6DE.f2;yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_UnknownCall_p_struct*
_tmp6E1=_cycalloc(sizeof(*_tmp6E1));_tmp6E1[0]=({struct Cyc_Absyn_UnknownCall_p_struct
_tmp6E2;_tmp6E2.tag=13;_tmp6E2.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp6E2.f2=_tmp6DF;
_tmp6E2.f3=_tmp6E0;_tmp6E2;});_tmp6E1;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;}case 380:
_LL3A8: {struct Cyc_List_List*_tmp6E4;int _tmp6E5;struct _tuple19 _tmp6E3=*Cyc_yyget_YY16(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp6E4=_tmp6E3.f1;
_tmp6E5=_tmp6E3.f2;{struct Cyc_List_List*_tmp6E6=((struct Cyc_List_List*(*)(struct
Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line),Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]));yyval=Cyc_YY11(Cyc_Absyn_new_pat((
void*)({struct Cyc_Absyn_Aggr_p_struct*_tmp6E7=_cycalloc(sizeof(*_tmp6E7));
_tmp6E7[0]=({struct Cyc_Absyn_Aggr_p_struct _tmp6E8;_tmp6E8.tag=5;_tmp6E8.f1=({
struct Cyc_Absyn_AggrInfo*_tmp6E9=_cycalloc(sizeof(*_tmp6E9));_tmp6E9->aggr_info=
Cyc_Absyn_UnknownAggr((void*)0,Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),0);_tmp6E9->targs=0;
_tmp6E9;});_tmp6E8.f2=_tmp6E6;_tmp6E8.f3=_tmp6E4;_tmp6E8.f4=_tmp6E5;_tmp6E8;});
_tmp6E7;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));break;}}case 381: _LL3A9: {struct Cyc_List_List*
_tmp6EB;int _tmp6EC;struct _tuple19 _tmp6EA=*Cyc_yyget_YY16(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp6EB=_tmp6EA.f1;
_tmp6EC=_tmp6EA.f2;{struct Cyc_List_List*_tmp6ED=((struct Cyc_List_List*(*)(struct
Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line),Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]));yyval=Cyc_YY11(Cyc_Absyn_new_pat((
void*)({struct Cyc_Absyn_Aggr_p_struct*_tmp6EE=_cycalloc(sizeof(*_tmp6EE));
_tmp6EE[0]=({struct Cyc_Absyn_Aggr_p_struct _tmp6EF;_tmp6EF.tag=5;_tmp6EF.f1=0;
_tmp6EF.f2=_tmp6ED;_tmp6EF.f3=_tmp6EB;_tmp6EF.f4=_tmp6EC;_tmp6EF;});_tmp6EE;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));break;}}case 382: _LL3AA: yyval=Cyc_YY11(Cyc_Absyn_new_pat((
void*)({struct Cyc_Absyn_Pointer_p_struct*_tmp6F0=_cycalloc(sizeof(*_tmp6F0));
_tmp6F0[0]=({struct Cyc_Absyn_Pointer_p_struct _tmp6F1;_tmp6F1.tag=4;_tmp6F1.f1=
Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6F1;});
_tmp6F0;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));break;case 383: _LL3AB: yyval=Cyc_YY11(Cyc_Absyn_new_pat((
void*)({struct Cyc_Absyn_Pointer_p_struct*_tmp6F2=_cycalloc(sizeof(*_tmp6F2));
_tmp6F2[0]=({struct Cyc_Absyn_Pointer_p_struct _tmp6F3;_tmp6F3.tag=4;_tmp6F3.f1=
Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Pointer_p_struct*_tmp6F4=_cycalloc(
sizeof(*_tmp6F4));_tmp6F4[0]=({struct Cyc_Absyn_Pointer_p_struct _tmp6F5;_tmp6F5.tag=
4;_tmp6F5.f1=Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp6F5;});_tmp6F4;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp6F3;});
_tmp6F2;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));break;case 384: _LL3AC: yyval=Cyc_YY11(Cyc_Absyn_new_pat((
void*)({struct Cyc_Absyn_Reference_p_struct*_tmp6F6=_cycalloc(sizeof(*_tmp6F6));
_tmp6F6[0]=({struct Cyc_Absyn_Reference_p_struct _tmp6F7;_tmp6F7.tag=1;_tmp6F7.f1=
Cyc_Absyn_new_vardecl(({struct _tuple1*_tmp6F8=_cycalloc(sizeof(*_tmp6F8));
_tmp6F8->f1=Cyc_Absyn_Loc_n;_tmp6F8->f2=({struct _dyneither_ptr*_tmp6F9=_cycalloc(
sizeof(*_tmp6F9));_tmp6F9[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6F9;});_tmp6F8;}),(void*)
0,0);_tmp6F7.f2=Cyc_Absyn_new_pat((void*)0,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp6F7;});
_tmp6F6;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));break;case 385: _LL3AD: if(Cyc_strcmp((struct
_dyneither_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),({const char*_tmp6FA="as";_tag_dyneither(_tmp6FA,sizeof(char),
3);}))!= 0)Cyc_Parse_err(({const char*_tmp6FB="expecting `as'";_tag_dyneither(
_tmp6FB,sizeof(char),15);}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));yyval=Cyc_YY11(
Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Reference_p_struct*_tmp6FC=_cycalloc(
sizeof(*_tmp6FC));_tmp6FC[0]=({struct Cyc_Absyn_Reference_p_struct _tmp6FD;_tmp6FD.tag=
1;_tmp6FD.f1=Cyc_Absyn_new_vardecl(({struct _tuple1*_tmp6FE=_cycalloc(sizeof(*
_tmp6FE));_tmp6FE->f1=Cyc_Absyn_Loc_n;_tmp6FE->f2=({struct _dyneither_ptr*_tmp6FF=
_cycalloc(sizeof(*_tmp6FF));_tmp6FF[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp6FF;});_tmp6FE;}),(
void*)0,0);_tmp6FD.f2=Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp6FD;});_tmp6FC;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line)));break;case
386: _LL3AE: {void*_tmp700=Cyc_Parse_id2type(Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Tcutil_kind_to_bound((
void*)7));yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_TagInt_p_struct*
_tmp701=_cycalloc(sizeof(*_tmp701));_tmp701[0]=({struct Cyc_Absyn_TagInt_p_struct
_tmp702;_tmp702.tag=2;_tmp702.f1=Cyc_Parse_typ2tvar(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),_tmp700);
_tmp702.f2=Cyc_Absyn_new_vardecl(({struct _tuple1*_tmp703=_cycalloc(sizeof(*
_tmp703));_tmp703->f1=Cyc_Absyn_Loc_n;_tmp703->f2=({struct _dyneither_ptr*_tmp704=
_cycalloc(sizeof(*_tmp704));_tmp704[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp704;});_tmp703;}),(
void*)({struct Cyc_Absyn_TagType_struct*_tmp705=_cycalloc(sizeof(*_tmp705));
_tmp705[0]=({struct Cyc_Absyn_TagType_struct _tmp706;_tmp706.tag=18;_tmp706.f1=(
void*)_tmp700;_tmp706;});_tmp705;}),0);_tmp702;});_tmp701;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;}case 387:
_LL3AF: {struct Cyc_Absyn_Tvar*_tmp707=Cyc_Tcutil_new_tvar(Cyc_Tcutil_kind_to_bound((
void*)7));yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_TagInt_p_struct*
_tmp708=_cycalloc(sizeof(*_tmp708));_tmp708[0]=({struct Cyc_Absyn_TagInt_p_struct
_tmp709;_tmp709.tag=2;_tmp709.f1=_tmp707;_tmp709.f2=Cyc_Absyn_new_vardecl(({
struct _tuple1*_tmp70A=_cycalloc(sizeof(*_tmp70A));_tmp70A->f1=Cyc_Absyn_Loc_n;
_tmp70A->f2=({struct _dyneither_ptr*_tmp70B=_cycalloc(sizeof(*_tmp70B));_tmp70B[0]=
Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);
_tmp70B;});_tmp70A;}),(void*)({struct Cyc_Absyn_TagType_struct*_tmp70C=_cycalloc(
sizeof(*_tmp70C));_tmp70C[0]=({struct Cyc_Absyn_TagType_struct _tmp70D;_tmp70D.tag=
18;_tmp70D.f1=(void*)((void*)({struct Cyc_Absyn_VarType_struct*_tmp70E=_cycalloc(
sizeof(*_tmp70E));_tmp70E[0]=({struct Cyc_Absyn_VarType_struct _tmp70F;_tmp70F.tag=
1;_tmp70F.f1=_tmp707;_tmp70F;});_tmp70E;}));_tmp70D;});_tmp70C;}),0);_tmp709;});
_tmp708;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));break;}case 388: _LL3B0: yyval=Cyc_YY12(({struct
_tuple19*_tmp710=_cycalloc(sizeof(*_tmp710));_tmp710->f1=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY13(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp710->f2=0;_tmp710;}));
break;case 389: _LL3B1: yyval=Cyc_YY12(({struct _tuple19*_tmp711=_cycalloc(sizeof(*
_tmp711));_tmp711->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(
Cyc_yyget_YY13(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]));
_tmp711->f2=1;_tmp711;}));break;case 390: _LL3B2: yyval=Cyc_YY12(({struct _tuple19*
_tmp712=_cycalloc(sizeof(*_tmp712));_tmp712->f1=0;_tmp712->f2=1;_tmp712;}));
break;case 391: _LL3B3: yyval=Cyc_YY13(({struct Cyc_List_List*_tmp713=_cycalloc(
sizeof(*_tmp713));_tmp713->hd=Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp713->tl=0;_tmp713;}));break;case 392: _LL3B4: yyval=Cyc_YY13(({
struct Cyc_List_List*_tmp714=_cycalloc(sizeof(*_tmp714));_tmp714->hd=Cyc_yyget_YY11(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp714->tl=Cyc_yyget_YY13(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp714;}));break;
case 393: _LL3B5: yyval=Cyc_YY14(({struct _tuple20*_tmp715=_cycalloc(sizeof(*_tmp715));
_tmp715->f1=0;_tmp715->f2=Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp715;}));break;case 394: _LL3B6: yyval=Cyc_YY14(({struct
_tuple20*_tmp716=_cycalloc(sizeof(*_tmp716));_tmp716->f1=Cyc_yyget_YY41(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp716->f2=Cyc_yyget_YY11(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp716;}));break;case
395: _LL3B7: yyval=Cyc_YY16(({struct _tuple19*_tmp717=_cycalloc(sizeof(*_tmp717));
_tmp717->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY15(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp717->f2=0;_tmp717;}));
break;case 396: _LL3B8: yyval=Cyc_YY16(({struct _tuple19*_tmp718=_cycalloc(sizeof(*
_tmp718));_tmp718->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(
Cyc_yyget_YY15(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]));
_tmp718->f2=1;_tmp718;}));break;case 397: _LL3B9: yyval=Cyc_YY16(({struct _tuple19*
_tmp719=_cycalloc(sizeof(*_tmp719));_tmp719->f1=0;_tmp719->f2=1;_tmp719;}));
break;case 398: _LL3BA: yyval=Cyc_YY15(({struct Cyc_List_List*_tmp71A=_cycalloc(
sizeof(*_tmp71A));_tmp71A->hd=Cyc_yyget_YY14(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp71A->tl=0;_tmp71A;}));break;case 399: _LL3BB: yyval=Cyc_YY15(({
struct Cyc_List_List*_tmp71B=_cycalloc(sizeof(*_tmp71B));_tmp71B->hd=Cyc_yyget_YY14(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp71B->tl=Cyc_yyget_YY15(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp71B;}));break;
case 400: _LL3BC: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];
break;case 401: _LL3BD: yyval=Cyc_YY3(Cyc_Absyn_seq_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 402:
_LL3BE: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
403: _LL3BF: yyval=Cyc_YY3(Cyc_Absyn_assignop_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY8(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 404:
_LL3C0: yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Swap_e_struct*
_tmp71C=_cycalloc(sizeof(*_tmp71C));_tmp71C[0]=({struct Cyc_Absyn_Swap_e_struct
_tmp71D;_tmp71D.tag=36;_tmp71D.f1=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp71D.f2=Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp71D;});_tmp71C;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));break;case 405: _LL3C1: yyval=Cyc_YY8(0);break;case 406:
_LL3C2: yyval=Cyc_YY8(({struct Cyc_Core_Opt*_tmp71E=_cycalloc(sizeof(*_tmp71E));
_tmp71E->v=(void*)((void*)1);_tmp71E;}));break;case 407: _LL3C3: yyval=Cyc_YY8(({
struct Cyc_Core_Opt*_tmp71F=_cycalloc(sizeof(*_tmp71F));_tmp71F->v=(void*)((void*)
3);_tmp71F;}));break;case 408: _LL3C4: yyval=Cyc_YY8(({struct Cyc_Core_Opt*_tmp720=
_cycalloc(sizeof(*_tmp720));_tmp720->v=(void*)((void*)4);_tmp720;}));break;case
409: _LL3C5: yyval=Cyc_YY8(({struct Cyc_Core_Opt*_tmp721=_cycalloc(sizeof(*_tmp721));
_tmp721->v=(void*)((void*)0);_tmp721;}));break;case 410: _LL3C6: yyval=Cyc_YY8(({
struct Cyc_Core_Opt*_tmp722=_cycalloc(sizeof(*_tmp722));_tmp722->v=(void*)((void*)
2);_tmp722;}));break;case 411: _LL3C7: yyval=Cyc_YY8(({struct Cyc_Core_Opt*_tmp723=
_cycalloc(sizeof(*_tmp723));_tmp723->v=(void*)((void*)16);_tmp723;}));break;case
412: _LL3C8: yyval=Cyc_YY8(({struct Cyc_Core_Opt*_tmp724=_cycalloc(sizeof(*_tmp724));
_tmp724->v=(void*)((void*)17);_tmp724;}));break;case 413: _LL3C9: yyval=Cyc_YY8(({
struct Cyc_Core_Opt*_tmp725=_cycalloc(sizeof(*_tmp725));_tmp725->v=(void*)((void*)
13);_tmp725;}));break;case 414: _LL3CA: yyval=Cyc_YY8(({struct Cyc_Core_Opt*_tmp726=
_cycalloc(sizeof(*_tmp726));_tmp726->v=(void*)((void*)15);_tmp726;}));break;case
415: _LL3CB: yyval=Cyc_YY8(({struct Cyc_Core_Opt*_tmp727=_cycalloc(sizeof(*_tmp727));
_tmp727->v=(void*)((void*)14);_tmp727;}));break;case 416: _LL3CC: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 417: _LL3CD: yyval=
Cyc_YY3(Cyc_Absyn_conditional_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 418:
_LL3CE: yyval=Cyc_YY3(Cyc_Absyn_throw_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 419:
_LL3CF: yyval=Cyc_YY3(Cyc_Absyn_New_exp(0,Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 420:
_LL3D0: yyval=Cyc_YY3(Cyc_Absyn_New_exp(0,Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 421:
_LL3D1: yyval=Cyc_YY3(Cyc_Absyn_New_exp((struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 422:
_LL3D2: yyval=Cyc_YY3(Cyc_Absyn_New_exp((struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 423:
_LL3D3: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
424: _LL3D4: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;
case 425: _LL3D5: yyval=Cyc_YY3(Cyc_Absyn_or_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 426:
_LL3D6: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
427: _LL3D7: yyval=Cyc_YY3(Cyc_Absyn_and_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 428:
_LL3D8: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
429: _LL3D9: yyval=Cyc_YY3(Cyc_Absyn_prim2_exp((void*)14,Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 430:
_LL3DA: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
431: _LL3DB: yyval=Cyc_YY3(Cyc_Absyn_prim2_exp((void*)15,Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 432:
_LL3DC: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
433: _LL3DD: yyval=Cyc_YY3(Cyc_Absyn_prim2_exp((void*)13,Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 434:
_LL3DE: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
435: _LL3DF: yyval=Cyc_YY3(Cyc_Absyn_eq_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 436:
_LL3E0: yyval=Cyc_YY3(Cyc_Absyn_neq_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 437:
_LL3E1: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
438: _LL3E2: yyval=Cyc_YY3(Cyc_Absyn_lt_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 439:
_LL3E3: yyval=Cyc_YY3(Cyc_Absyn_gt_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 440:
_LL3E4: yyval=Cyc_YY3(Cyc_Absyn_lte_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 441:
_LL3E5: yyval=Cyc_YY3(Cyc_Absyn_gte_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 442:
_LL3E6: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
443: _LL3E7: yyval=Cyc_YY3(Cyc_Absyn_prim2_exp((void*)16,Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 444:
_LL3E8: yyval=Cyc_YY3(Cyc_Absyn_prim2_exp((void*)17,Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 445:
_LL3E9: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
446: _LL3EA: yyval=Cyc_YY3(Cyc_Absyn_prim2_exp((void*)0,Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 447:
_LL3EB: yyval=Cyc_YY3(Cyc_Absyn_prim2_exp((void*)2,Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 448:
_LL3EC: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
449: _LL3ED: yyval=Cyc_YY3(Cyc_Absyn_prim2_exp((void*)1,Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 450:
_LL3EE: yyval=Cyc_YY3(Cyc_Absyn_prim2_exp((void*)3,Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 451:
_LL3EF: yyval=Cyc_YY3(Cyc_Absyn_prim2_exp((void*)4,Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 452:
_LL3F0: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
453: _LL3F1: yyval=Cyc_YY3(Cyc_Absyn_cast_exp((*Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])).f3,Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),1,(void*)0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 454:
_LL3F2: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
455: _LL3F3: yyval=Cyc_YY3(Cyc_Absyn_pre_inc_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 456:
_LL3F4: yyval=Cyc_YY3(Cyc_Absyn_pre_dec_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 457:
_LL3F5: yyval=Cyc_YY3(Cyc_Absyn_address_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 458:
_LL3F6: yyval=Cyc_YY3(Cyc_Absyn_deref_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 459:
_LL3F7: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
460: _LL3F8: yyval=Cyc_YY3(Cyc_Absyn_prim1_exp(Cyc_yyget_YY7(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 461:
_LL3F9: yyval=Cyc_YY3(Cyc_Absyn_sizeoftyp_exp((*Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])).f3,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 462:
_LL3FA: yyval=Cyc_YY3(Cyc_Absyn_sizeofexp_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 463:
_LL3FB: yyval=Cyc_YY3(Cyc_Absyn_offsetof_exp((*Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)])).f3,(void*)({struct Cyc_Absyn_StructField_struct*
_tmp728=_cycalloc(sizeof(*_tmp728));_tmp728[0]=({struct Cyc_Absyn_StructField_struct
_tmp729;_tmp729.tag=0;_tmp729.f1=({struct _dyneither_ptr*_tmp72A=_cycalloc(
sizeof(*_tmp72A));_tmp72A[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp72A;});_tmp729;});
_tmp728;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));break;case 464: _LL3FC: yyval=Cyc_YY3(Cyc_Absyn_offsetof_exp((*
Cyc_yyget_YY37(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)])).f3,(
void*)({struct Cyc_Absyn_TupleIndex_struct*_tmp72B=_cycalloc_atomic(sizeof(*
_tmp72B));_tmp72B[0]=({struct Cyc_Absyn_TupleIndex_struct _tmp72C;_tmp72C.tag=1;
_tmp72C.f1=(unsigned int)(Cyc_yyget_Int_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)])).f2;_tmp72C;});_tmp72B;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 465:
_LL3FD: {struct Cyc_Position_Segment*_tmp72D=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);struct Cyc_List_List*
_tmp72E=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
_tmp72D,Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]));
yyval=Cyc_YY3(Cyc_Absyn_gentyp_exp(_tmp72E,(*Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])).f3,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;}case 466:
_LL3FE: yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Malloc_e_struct*
_tmp72F=_cycalloc(sizeof(*_tmp72F));_tmp72F[0]=({struct Cyc_Absyn_Malloc_e_struct
_tmp730;_tmp730.tag=35;_tmp730.f1=({struct Cyc_Absyn_MallocInfo _tmp731;_tmp731.is_calloc=
0;_tmp731.rgn=0;_tmp731.elt_type=0;_tmp731.num_elts=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp731.fat_result=0;
_tmp731;});_tmp730;});_tmp72F;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 467:
_LL3FF: yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Malloc_e_struct*
_tmp732=_cycalloc(sizeof(*_tmp732));_tmp732[0]=({struct Cyc_Absyn_Malloc_e_struct
_tmp733;_tmp733.tag=35;_tmp733.f1=({struct Cyc_Absyn_MallocInfo _tmp734;_tmp734.is_calloc=
0;_tmp734.rgn=(struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp734.elt_type=0;
_tmp734.num_elts=Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp734.fat_result=0;_tmp734;});_tmp733;});_tmp732;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 468:
_LL400: {void*_tmp736;struct _tuple8 _tmp735=*Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp736=_tmp735.f3;yyval=
Cyc_YY3(Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Malloc_e_struct*_tmp737=
_cycalloc(sizeof(*_tmp737));_tmp737[0]=({struct Cyc_Absyn_Malloc_e_struct _tmp738;
_tmp738.tag=35;_tmp738.f1=({struct Cyc_Absyn_MallocInfo _tmp739;_tmp739.is_calloc=
1;_tmp739.rgn=0;_tmp739.elt_type=({void**_tmp73A=_cycalloc(sizeof(*_tmp73A));
_tmp73A[0]=_tmp736;_tmp73A;});_tmp739.num_elts=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 6)]);_tmp739.fat_result=0;
_tmp739;});_tmp738;});_tmp737;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 8)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;}case 469:
_LL401: {void*_tmp73C;struct _tuple8 _tmp73B=*Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp73C=_tmp73B.f3;yyval=
Cyc_YY3(Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Malloc_e_struct*_tmp73D=
_cycalloc(sizeof(*_tmp73D));_tmp73D[0]=({struct Cyc_Absyn_Malloc_e_struct _tmp73E;
_tmp73E.tag=35;_tmp73E.f1=({struct Cyc_Absyn_MallocInfo _tmp73F;_tmp73F.is_calloc=
1;_tmp73F.rgn=(struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 8)]);_tmp73F.elt_type=({void**
_tmp740=_cycalloc(sizeof(*_tmp740));_tmp740[0]=_tmp73C;_tmp740;});_tmp73F.num_elts=
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]);
_tmp73F.fat_result=0;_tmp73F;});_tmp73E;});_tmp73D;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 10)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;}case 470:
_LL402: yyval=Cyc_YY3(Cyc_Absyn_primop_exp((void*)19,({struct Cyc_Absyn_Exp*
_tmp741[1];_tmp741[0]=Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmp741,sizeof(struct Cyc_Absyn_Exp*),1));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 471:
_LL403: yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Tagcheck_e_struct*
_tmp742=_cycalloc(sizeof(*_tmp742));_tmp742[0]=({struct Cyc_Absyn_Tagcheck_e_struct
_tmp743;_tmp743.tag=39;_tmp743.f1=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp743.f2=({struct
_dyneither_ptr*_tmp744=_cycalloc(sizeof(*_tmp744));_tmp744[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp744;});_tmp743;});
_tmp742;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));break;case 472: _LL404: yyval=Cyc_YY3(Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_Tagcheck_e_struct*_tmp745=_cycalloc(sizeof(*_tmp745));
_tmp745[0]=({struct Cyc_Absyn_Tagcheck_e_struct _tmp746;_tmp746.tag=39;_tmp746.f1=
Cyc_Absyn_deref_exp(Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));_tmp746.f2=({
struct _dyneither_ptr*_tmp747=_cycalloc(sizeof(*_tmp747));_tmp747[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp747;});_tmp746;});
_tmp745;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));break;case 473: _LL405: {void*_tmp749;struct _tuple8
_tmp748=*Cyc_yyget_YY37(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
1)]);_tmp749=_tmp748.f3;yyval=Cyc_YY3(Cyc_Absyn_valueof_exp(_tmp749,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;}case 474:
_LL406: yyval=Cyc_YY7((void*)12);break;case 475: _LL407: yyval=Cyc_YY7((void*)11);
break;case 476: _LL408: yyval=Cyc_YY7((void*)2);break;case 477: _LL409: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 478: _LL40A: yyval=
Cyc_YY3(Cyc_Absyn_subscript_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 479:
_LL40B: yyval=Cyc_YY3(Cyc_Absyn_unknowncall_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 480:
_LL40C: yyval=Cyc_YY3(Cyc_Absyn_unknowncall_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_yyget_YY5(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 481:
_LL40D: yyval=Cyc_YY3(Cyc_Absyn_aggrmember_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),({struct _dyneither_ptr*
_tmp74A=_cycalloc(sizeof(*_tmp74A));_tmp74A[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp74A;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 482:
_LL40E: yyval=Cyc_YY3(Cyc_Absyn_aggrarrow_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),({struct _dyneither_ptr*
_tmp74B=_cycalloc(sizeof(*_tmp74B));_tmp74B[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp74B;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 483:
_LL40F: yyval=Cyc_YY3(Cyc_Absyn_post_inc_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 484:
_LL410: yyval=Cyc_YY3(Cyc_Absyn_post_dec_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 485:
_LL411: yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_CompoundLit_e_struct*
_tmp74C=_cycalloc(sizeof(*_tmp74C));_tmp74C[0]=({struct Cyc_Absyn_CompoundLit_e_struct
_tmp74D;_tmp74D.tag=27;_tmp74D.f1=Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp74D.f2=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY6(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp74D;});_tmp74C;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));break;case 486: _LL412: yyval=Cyc_YY3(Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_CompoundLit_e_struct*_tmp74E=_cycalloc(sizeof(*_tmp74E));
_tmp74E[0]=({struct Cyc_Absyn_CompoundLit_e_struct _tmp74F;_tmp74F.tag=27;_tmp74F.f1=
Cyc_yyget_YY37(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]);
_tmp74F.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_YY6(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]));
_tmp74F;});_tmp74E;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 487:
_LL413: yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_UnknownId_e_struct*
_tmp750=_cycalloc(sizeof(*_tmp750));_tmp750[0]=({struct Cyc_Absyn_UnknownId_e_struct
_tmp751;_tmp751.tag=2;_tmp751.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp751;});_tmp750;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 488:
_LL414: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
489: _LL415: yyval=Cyc_YY3(Cyc_Absyn_string_exp(Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 490:
_LL416: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;
case 491: _LL417: yyval=Cyc_YY3(Cyc_Absyn_noinstantiate_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 492:
_LL418: yyval=Cyc_YY3(Cyc_Absyn_instantiate_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_List_imp_rev(Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 493:
_LL419: yyval=Cyc_YY3(Cyc_Absyn_tuple_exp(Cyc_yyget_YY5(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 494:
_LL41A: yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Aggregate_e_struct*
_tmp752=_cycalloc(sizeof(*_tmp752));_tmp752[0]=({struct Cyc_Absyn_Aggregate_e_struct
_tmp753;_tmp753.tag=30;_tmp753.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp753.f2=Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp753.f3=((struct
Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY6(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp753.f4=0;_tmp753;});
_tmp752;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));break;case 495: _LL41B: yyval=Cyc_YY3(Cyc_Absyn_stmt_exp(
Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 496:
_LL41C: yyval=Cyc_YY5(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_YY5(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])));break;
case 497: _LL41D: yyval=Cyc_YY5(({struct Cyc_List_List*_tmp754=_cycalloc(sizeof(*
_tmp754));_tmp754->hd=Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp754->tl=0;_tmp754;}));break;case 498: _LL41E: yyval=Cyc_YY5(({
struct Cyc_List_List*_tmp755=_cycalloc(sizeof(*_tmp755));_tmp755->hd=Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp755->tl=Cyc_yyget_YY5(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp755;}));break;
case 499: _LL41F: yyval=Cyc_YY3(Cyc_Absyn_int_exp((Cyc_yyget_Int_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f1,(Cyc_yyget_Int_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f2,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 500:
_LL420: yyval=Cyc_YY3(Cyc_Absyn_char_exp(Cyc_yyget_Char_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 501:
_LL421: yyval=Cyc_YY3(Cyc_Absyn_float_exp(Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 502:
_LL422: yyval=Cyc_YY3(Cyc_Absyn_null_exp(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 503:
_LL423: yyval=Cyc_QualId_tok(({struct _tuple1*_tmp756=_cycalloc(sizeof(*_tmp756));
_tmp756->f1=Cyc_Absyn_Rel_n(0);_tmp756->f2=({struct _dyneither_ptr*_tmp757=
_cycalloc(sizeof(*_tmp757));_tmp757[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp757;});_tmp756;}));
break;case 504: _LL424: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 505: _LL425: yyval=Cyc_QualId_tok(({struct _tuple1*_tmp758=
_cycalloc(sizeof(*_tmp758));_tmp758->f1=Cyc_Absyn_Rel_n(0);_tmp758->f2=({struct
_dyneither_ptr*_tmp759=_cycalloc(sizeof(*_tmp759));_tmp759[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp759;});_tmp758;}));
break;case 506: _LL426: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 507: _LL427: yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)];break;case 508: _LL428: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 509: _LL429: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 510: _LL42A:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 511:
_LL42B: break;case 512: _LL42C: yylex_buf->lex_curr_pos -=1;break;default: _LL42D:
break;}yyvsp_offset -=yylen;yyssp_offset -=yylen;yylsp_offset -=yylen;yyvs[
_check_known_subscript_notnull(10000,++ yyvsp_offset)]=yyval;++ yylsp_offset;if(
yylen == 0){(yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line=
Cyc_yylloc.first_line;(yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_column=
Cyc_yylloc.first_column;(yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).last_line=(
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line;(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_column=(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_column;}else{(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line=(yyls[
_check_known_subscript_notnull(10000,(yylsp_offset + yylen)- 1)]).last_line;(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_column=(yyls[
_check_known_subscript_notnull(10000,(yylsp_offset + yylen)- 1)]).last_column;}
yyn=(int)Cyc_yyr1[_check_known_subscript_notnull(513,yyn)];yystate=Cyc_yypgoto[
_check_known_subscript_notnull(144,yyn - 144)]+ yyss[
_check_known_subscript_notnull(10000,yyssp_offset)];if((yystate >= 0  && yystate <= 
6273) && Cyc_yycheck[_check_known_subscript_notnull(6274,yystate)]== yyss[
_check_known_subscript_notnull(10000,yyssp_offset)])yystate=(int)Cyc_yytable[
_check_known_subscript_notnull(6274,yystate)];else{yystate=(int)Cyc_yydefgoto[
_check_known_subscript_notnull(144,yyn - 144)];}goto yynewstate;yyerrlab: if(
yyerrstatus == 0){++ Cyc_yynerrs;yyn=(int)Cyc_yypact[
_check_known_subscript_notnull(1035,yystate)];if(yyn > - 32768  && yyn < 6273){int
sze=0;struct _dyneither_ptr msg;int x;int count;count=0;for(x=yyn < 0?- yyn: 0;x < 288 / 
sizeof(char*);++ x){if(Cyc_yycheck[_check_known_subscript_notnull(6274,x + yyn)]== 
x)(sze +=Cyc_strlen((struct _dyneither_ptr)Cyc_yytname[
_check_known_subscript_notnull(288,x)])+ 15,count ++);}msg=({unsigned int _tmp75A=(
unsigned int)(sze + 15);char*_tmp75B=(char*)_region_malloc(yyregion,_check_times(
sizeof(char),_tmp75A + 1));struct _dyneither_ptr _tmp75D=_tag_dyneither(_tmp75B,
sizeof(char),_tmp75A + 1);{unsigned int _tmp75C=_tmp75A;unsigned int i;for(i=0;i < 
_tmp75C;i ++){_tmp75B[i]='\000';}_tmp75B[_tmp75C]=(char)0;}_tmp75D;});Cyc_strcpy(
msg,({const char*_tmp75E="parse error";_tag_dyneither(_tmp75E,sizeof(char),12);}));
if(count < 5){count=0;for(x=yyn < 0?- yyn: 0;x < 288 / sizeof(char*);++ x){if(Cyc_yycheck[
_check_known_subscript_notnull(6274,x + yyn)]== x){Cyc_strcat(msg,(struct
_dyneither_ptr)(count == 0?(struct _dyneither_ptr)({const char*_tmp75F=", expecting `";
_tag_dyneither(_tmp75F,sizeof(char),14);}):(struct _dyneither_ptr)({const char*
_tmp760=" or `";_tag_dyneither(_tmp760,sizeof(char),6);})));Cyc_strcat(msg,(
struct _dyneither_ptr)Cyc_yytname[_check_known_subscript_notnull(288,x)]);Cyc_strcat(
msg,({const char*_tmp761="'";_tag_dyneither(_tmp761,sizeof(char),2);}));++ count;}}}
Cyc_yyerror((struct _dyneither_ptr)msg);}else{Cyc_yyerror(({const char*_tmp762="parse error";
_tag_dyneither(_tmp762,sizeof(char),12);}));}}goto yyerrlab1;yyerrlab1: if(
yyerrstatus == 3){if(Cyc_yychar == 0){int _tmp763=1;_npop_handler(0);return _tmp763;}
Cyc_yychar=- 2;}yyerrstatus=3;goto yyerrhandle;yyerrdefault: yyerrpop: if(
yyssp_offset == 0){int _tmp764=1;_npop_handler(0);return _tmp764;}-- yyvsp_offset;
yystate=(int)yyss[_check_known_subscript_notnull(10000,-- yyssp_offset)];--
yylsp_offset;yyerrhandle: yyn=(int)Cyc_yypact[_check_known_subscript_notnull(1035,
yystate)];if(yyn == - 32768)goto yyerrdefault;yyn +=1;if((yyn < 0  || yyn > 6273) || 
Cyc_yycheck[_check_known_subscript_notnull(6274,yyn)]!= 1)goto yyerrdefault;yyn=(
int)Cyc_yytable[_check_known_subscript_notnull(6274,yyn)];if(yyn < 0){if(yyn == -
32768)goto yyerrpop;yyn=- yyn;goto yyreduce;}else{if(yyn == 0)goto yyerrpop;}if(yyn == 
1034){int _tmp765=0;_npop_handler(0);return _tmp765;}yyvs[
_check_known_subscript_notnull(10000,++ yyvsp_offset)]=Cyc_yylval;yyls[
_check_known_subscript_notnull(10000,++ yylsp_offset)]=Cyc_yylloc;goto yynewstate;};
_pop_region(yyregion);}void Cyc_yyprint(int i,union Cyc_YYSTYPE v){union Cyc_YYSTYPE
_tmp76F=v;struct _tuple6 _tmp770;int _tmp771;char _tmp772;struct _dyneither_ptr
_tmp773;struct _tuple1*_tmp774;struct _tuple1 _tmp775;union Cyc_Absyn_Nmspace _tmp776;
struct _dyneither_ptr*_tmp777;_LL430: if((_tmp76F.Int_tok).tag != 1)goto _LL432;
_tmp770=(struct _tuple6)(_tmp76F.Int_tok).val;_tmp771=_tmp770.f2;_LL431:({struct
Cyc_Int_pa_struct _tmp77A;_tmp77A.tag=1;_tmp77A.f1=(unsigned long)_tmp771;{void*
_tmp778[1]={& _tmp77A};Cyc_fprintf(Cyc_stderr,({const char*_tmp779="%d";
_tag_dyneither(_tmp779,sizeof(char),3);}),_tag_dyneither(_tmp778,sizeof(void*),1));}});
goto _LL42F;_LL432: if((_tmp76F.Char_tok).tag != 2)goto _LL434;_tmp772=(char)(
_tmp76F.Char_tok).val;_LL433:({struct Cyc_Int_pa_struct _tmp77D;_tmp77D.tag=1;
_tmp77D.f1=(unsigned long)((int)_tmp772);{void*_tmp77B[1]={& _tmp77D};Cyc_fprintf(
Cyc_stderr,({const char*_tmp77C="%c";_tag_dyneither(_tmp77C,sizeof(char),3);}),
_tag_dyneither(_tmp77B,sizeof(void*),1));}});goto _LL42F;_LL434: if((_tmp76F.String_tok).tag
!= 3)goto _LL436;_tmp773=(struct _dyneither_ptr)(_tmp76F.String_tok).val;_LL435:({
struct Cyc_String_pa_struct _tmp780;_tmp780.tag=0;_tmp780.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)_tmp773);{void*_tmp77E[1]={& _tmp780};Cyc_fprintf(Cyc_stderr,({
const char*_tmp77F="\"%s\"";_tag_dyneither(_tmp77F,sizeof(char),5);}),
_tag_dyneither(_tmp77E,sizeof(void*),1));}});goto _LL42F;_LL436: if((_tmp76F.QualId_tok).tag
!= 5)goto _LL438;_tmp774=(struct _tuple1*)(_tmp76F.QualId_tok).val;_tmp775=*
_tmp774;_tmp776=_tmp775.f1;_tmp777=_tmp775.f2;_LL437: {struct Cyc_List_List*
_tmp781=0;{union Cyc_Absyn_Nmspace _tmp782=_tmp776;struct Cyc_List_List*_tmp783;
struct Cyc_List_List*_tmp784;int _tmp785;_LL43B: if((_tmp782.Rel_n).tag != 1)goto
_LL43D;_tmp783=(struct Cyc_List_List*)(_tmp782.Rel_n).val;_LL43C: _tmp781=_tmp783;
goto _LL43A;_LL43D: if((_tmp782.Abs_n).tag != 2)goto _LL43F;_tmp784=(struct Cyc_List_List*)(
_tmp782.Abs_n).val;_LL43E: _tmp781=_tmp784;goto _LL43A;_LL43F: if((_tmp782.Loc_n).tag
!= 3)goto _LL43A;_tmp785=(int)(_tmp782.Loc_n).val;_LL440: goto _LL43A;_LL43A:;}for(
0;_tmp781 != 0;_tmp781=_tmp781->tl){({struct Cyc_String_pa_struct _tmp788;_tmp788.tag=
0;_tmp788.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct
_dyneither_ptr*)_tmp781->hd));{void*_tmp786[1]={& _tmp788};Cyc_fprintf(Cyc_stderr,({
const char*_tmp787="%s::";_tag_dyneither(_tmp787,sizeof(char),5);}),
_tag_dyneither(_tmp786,sizeof(void*),1));}});}({struct Cyc_String_pa_struct
_tmp78B;_tmp78B.tag=0;_tmp78B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*
_tmp777);{void*_tmp789[1]={& _tmp78B};Cyc_fprintf(Cyc_stderr,({const char*_tmp78A="%s::";
_tag_dyneither(_tmp78A,sizeof(char),5);}),_tag_dyneither(_tmp789,sizeof(void*),1));}});
goto _LL42F;}_LL438:;_LL439:({void*_tmp78C=0;Cyc_fprintf(Cyc_stderr,({const char*
_tmp78D="?";_tag_dyneither(_tmp78D,sizeof(char),2);}),_tag_dyneither(_tmp78C,
sizeof(void*),0));});goto _LL42F;_LL42F:;}struct Cyc_List_List*Cyc_Parse_parse_file(
struct Cyc___cycFILE*f){Cyc_Parse_parse_result=0;Cyc_yyparse(Cyc_Lexing_from_file(
f));return Cyc_Parse_parse_result;}
