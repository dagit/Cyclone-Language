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
_dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];int Cyc_Absyn_is_qvar_qualified(
struct _tuple1*);struct Cyc_Absyn_Tqual Cyc_Absyn_combine_tqual(struct Cyc_Absyn_Tqual
x,struct Cyc_Absyn_Tqual y);struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(struct Cyc_Position_Segment*);
struct Cyc_Absyn_Conref*Cyc_Absyn_new_conref(void*x);struct Cyc_Absyn_Conref*Cyc_Absyn_empty_conref();
extern struct Cyc_Absyn_Conref*Cyc_Absyn_true_conref;extern struct Cyc_Absyn_Conref*
Cyc_Absyn_false_conref;extern struct Cyc_Absyn_Conref*Cyc_Absyn_bounds_one_conref;
extern struct Cyc_Absyn_Conref*Cyc_Absyn_bounds_dyneither_conref;void*Cyc_Absyn_compress_kb(
void*);void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);void*Cyc_Absyn_int_typ(void*,void*);
extern void*Cyc_Absyn_char_typ;extern void*Cyc_Absyn_uint_typ;extern void*Cyc_Absyn_sint_typ;
extern void*Cyc_Absyn_float_typ;void*Cyc_Absyn_double_typ(int);void*Cyc_Absyn_starb_typ(
void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*b,struct Cyc_Absyn_Conref*zero_term);
void*Cyc_Absyn_array_typ(void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*
num_elts,struct Cyc_Absyn_Conref*zero_term,struct Cyc_Position_Segment*ztloc);
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,struct Cyc_Position_Segment*);struct
Cyc_Absyn_Exp*Cyc_Absyn_New_exp(struct Cyc_Absyn_Exp*rgn_handle,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_null_exp(struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_true_exp(struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_false_exp(struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(
void*,int,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(
unsigned int,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_char_exp(
char c,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_float_exp(
struct _dyneither_ptr f,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(
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
exists,struct Cyc_List_List*po,struct Cyc_List_List*fs);struct Cyc_Absyn_Decl*Cyc_Absyn_aggr_decl(
void*k,void*s,struct _tuple1*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*
i,struct Cyc_List_List*atts,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Decl*
Cyc_Absyn_tunion_decl(void*s,struct _tuple1*n,struct Cyc_List_List*ts,struct Cyc_Core_Opt*
fs,int is_xtunion,int is_flat,struct Cyc_Position_Segment*loc);void*Cyc_Absyn_function_typ(
struct Cyc_List_List*tvs,struct Cyc_Core_Opt*eff_typ,void*ret_typ,struct Cyc_List_List*
args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,struct Cyc_List_List*
rgn_po,struct Cyc_List_List*atts);int Cyc_Absyn_fntype_att(void*a);extern int Cyc_Absyn_porting_c_code;
struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(
struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;extern char Cyc_Set_Absent[11];struct
Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;
struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[12];extern char Cyc_Dict_Absent[11];
struct _tuple4{void*f1;void*f2;};struct _tuple4*Cyc_Dict_rchoose(struct
_RegionHandle*r,struct Cyc_Dict_Dict d);struct _tuple4*Cyc_Dict_rchoose(struct
_RegionHandle*,struct Cyc_Dict_Dict d);struct Cyc_RgnOrder_RgnPO;struct Cyc_RgnOrder_RgnPO*
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
};struct Cyc_Parse_Abstractdeclarator{struct Cyc_List_List*tms;};struct _tuple5{
void*f1;struct Cyc_Core_Opt*f2;};static struct _tuple5 Cyc_Parse_collapse_type_specifiers(
struct Cyc_List_List*ts,struct Cyc_Position_Segment*loc);struct _tuple6{struct Cyc_Absyn_Tqual
f1;void*f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;};static struct _tuple6
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
return;}static int Cyc_Parse_enum_counter=0;struct _tuple1*Cyc_Parse_gensym_enum(){
return({struct _tuple1*_tmp4=_cycalloc(sizeof(*_tmp4));_tmp4->f1=(union Cyc_Absyn_Nmspace_union)({
union Cyc_Absyn_Nmspace_union _tmp9;(_tmp9.Rel_n).tag=1;(_tmp9.Rel_n).f1=0;_tmp9;});
_tmp4->f2=({struct _dyneither_ptr*_tmp5=_cycalloc(sizeof(*_tmp5));_tmp5[0]=(
struct _dyneither_ptr)({struct Cyc_Int_pa_struct _tmp8;_tmp8.tag=1;_tmp8.f1=(
unsigned long)Cyc_Parse_enum_counter ++;{void*_tmp6[1]={& _tmp8};Cyc_aprintf(({
const char*_tmp7="__anonymous_enum_%d__";_tag_dyneither(_tmp7,sizeof(char),22);}),
_tag_dyneither(_tmp6,sizeof(void*),1));}});_tmp5;});_tmp4;});}struct _tuple7{
struct _tuple1*f1;struct Cyc_Absyn_Tqual f2;void*f3;struct Cyc_List_List*f4;struct
Cyc_List_List*f5;};struct _tuple8{struct _tuple7*f1;struct Cyc_Absyn_Exp*f2;};
static struct Cyc_Absyn_Aggrfield*Cyc_Parse_make_aggr_field(struct Cyc_Position_Segment*
loc,struct _tuple8*field_info){struct _tuple8 _tmpB;struct _tuple7*_tmpC;struct
_tuple7 _tmpD;struct _tuple1*_tmpE;struct Cyc_Absyn_Tqual _tmpF;void*_tmp10;struct
Cyc_List_List*_tmp11;struct Cyc_List_List*_tmp12;struct Cyc_Absyn_Exp*_tmp13;
struct _tuple8*_tmpA=field_info;_tmpB=*_tmpA;_tmpC=_tmpB.f1;_tmpD=*_tmpC;_tmpE=
_tmpD.f1;_tmpF=_tmpD.f2;_tmp10=_tmpD.f3;_tmp11=_tmpD.f4;_tmp12=_tmpD.f5;_tmp13=
_tmpB.f2;if(_tmp11 != 0)Cyc_Parse_err(({const char*_tmp14="bad type params in struct field";
_tag_dyneither(_tmp14,sizeof(char),32);}),loc);if(Cyc_Absyn_is_qvar_qualified(
_tmpE))Cyc_Parse_err(({const char*_tmp15="struct or union field cannot be qualified with a namespace";
_tag_dyneither(_tmp15,sizeof(char),59);}),loc);return({struct Cyc_Absyn_Aggrfield*
_tmp16=_cycalloc(sizeof(*_tmp16));_tmp16->name=(*_tmpE).f2;_tmp16->tq=_tmpF;
_tmp16->type=(void*)_tmp10;_tmp16->width=_tmp13;_tmp16->attributes=_tmp12;_tmp16;});}
static void*Cyc_Parse_type_spec(void*t,struct Cyc_Position_Segment*loc){return(
void*)({struct Cyc_Parse_Type_spec_struct*_tmp17=_cycalloc(sizeof(*_tmp17));
_tmp17[0]=({struct Cyc_Parse_Type_spec_struct _tmp18;_tmp18.tag=4;_tmp18.f1=(void*)
t;_tmp18.f2=loc;_tmp18;});_tmp17;});}static void*Cyc_Parse_array2ptr(void*t,int
argposn){void*_tmp19=t;struct Cyc_Absyn_ArrayInfo _tmp1A;void*_tmp1B;struct Cyc_Absyn_Tqual
_tmp1C;struct Cyc_Absyn_Exp*_tmp1D;struct Cyc_Absyn_Conref*_tmp1E;struct Cyc_Position_Segment*
_tmp1F;_LL1: if(_tmp19 <= (void*)4)goto _LL3;if(*((int*)_tmp19)!= 7)goto _LL3;_tmp1A=((
struct Cyc_Absyn_ArrayType_struct*)_tmp19)->f1;_tmp1B=(void*)_tmp1A.elt_type;
_tmp1C=_tmp1A.tq;_tmp1D=_tmp1A.num_elts;_tmp1E=_tmp1A.zero_term;_tmp1F=_tmp1A.zt_loc;
_LL2: return Cyc_Absyn_starb_typ(_tmp1B,argposn?Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_rk,0):(void*)2,_tmp1C,_tmp1D == 0?(void*)0:(void*)({struct Cyc_Absyn_Upper_b_struct*
_tmp20=_cycalloc(sizeof(*_tmp20));_tmp20[0]=({struct Cyc_Absyn_Upper_b_struct
_tmp21;_tmp21.tag=0;_tmp21.f1=(struct Cyc_Absyn_Exp*)_tmp1D;_tmp21;});_tmp20;}),
_tmp1E);_LL3:;_LL4: return t;_LL0:;}static void Cyc_Parse_arg_array2ptr(struct
_tuple2*x){(*x).f3=Cyc_Parse_array2ptr((*x).f3,1);}struct _tuple9{struct Cyc_Absyn_Tqual
f1;void*f2;};static struct _tuple9*Cyc_Parse_get_tqual_typ(struct Cyc_Position_Segment*
loc,struct _tuple2*t){return({struct _tuple9*_tmp22=_cycalloc(sizeof(*_tmp22));
_tmp22->f1=(*t).f2;_tmp22->f2=(*t).f3;_tmp22;});}static struct Cyc_Absyn_Vardecl*
Cyc_Parse_decl2vardecl(struct Cyc_Absyn_Decl*d){void*_tmp23=(void*)d->r;struct Cyc_Absyn_Vardecl*
_tmp24;_LL6: if(_tmp23 <= (void*)2)goto _LL8;if(*((int*)_tmp23)!= 0)goto _LL8;_tmp24=((
struct Cyc_Absyn_Var_d_struct*)_tmp23)->f1;_LL7: return _tmp24;_LL8:;_LL9:({void*
_tmp25=0;((int(*)(struct Cyc_Position_Segment*sg,struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Parse_abort)(d->loc,({const char*_tmp26="bad declaration in `forarray' statement";
_tag_dyneither(_tmp26,sizeof(char),40);}),_tag_dyneither(_tmp25,sizeof(void*),0));});
_LL5:;}static int Cyc_Parse_is_typeparam(void*tm){void*_tmp27=tm;_LLB: if(*((int*)
_tmp27)!= 4)goto _LLD;_LLC: return 1;_LLD:;_LLE: return 0;_LLA:;}static void*Cyc_Parse_id2type(
struct _dyneither_ptr s,void*k){if(Cyc_zstrcmp((struct _dyneither_ptr)s,({const char*
_tmp28="`H";_tag_dyneither(_tmp28,sizeof(char),3);}))== 0)return(void*)2;else{
if(Cyc_zstrcmp((struct _dyneither_ptr)s,({const char*_tmp29="`U";_tag_dyneither(
_tmp29,sizeof(char),3);}))== 0)return(void*)3;else{return(void*)({struct Cyc_Absyn_VarType_struct*
_tmp2A=_cycalloc(sizeof(*_tmp2A));_tmp2A[0]=({struct Cyc_Absyn_VarType_struct
_tmp2B;_tmp2B.tag=1;_tmp2B.f1=({struct Cyc_Absyn_Tvar*_tmp2C=_cycalloc(sizeof(*
_tmp2C));_tmp2C->name=({struct _dyneither_ptr*_tmp2D=_cycalloc(sizeof(*_tmp2D));
_tmp2D[0]=s;_tmp2D;});_tmp2C->identity=- 1;_tmp2C->kind=(void*)k;_tmp2C;});_tmp2B;});
_tmp2A;});}}}static struct Cyc_Absyn_Tvar*Cyc_Parse_copy_tvar(struct Cyc_Absyn_Tvar*
t){void*k;{void*_tmp2E=Cyc_Absyn_compress_kb((void*)t->kind);void*_tmp2F;_LL10:
if(*((int*)_tmp2E)!= 1)goto _LL12;_LL11: k=(void*)({struct Cyc_Absyn_Unknown_kb_struct*
_tmp30=_cycalloc(sizeof(*_tmp30));_tmp30[0]=({struct Cyc_Absyn_Unknown_kb_struct
_tmp31;_tmp31.tag=1;_tmp31.f1=0;_tmp31;});_tmp30;});goto _LLF;_LL12: if(*((int*)
_tmp2E)!= 2)goto _LL14;_tmp2F=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp2E)->f2;
_LL13: k=(void*)({struct Cyc_Absyn_Less_kb_struct*_tmp32=_cycalloc(sizeof(*_tmp32));
_tmp32[0]=({struct Cyc_Absyn_Less_kb_struct _tmp33;_tmp33.tag=2;_tmp33.f1=0;_tmp33.f2=(
void*)_tmp2F;_tmp33;});_tmp32;});goto _LLF;_LL14:;_LL15: k=_tmp2E;goto _LLF;_LLF:;}
return({struct Cyc_Absyn_Tvar*_tmp34=_cycalloc(sizeof(*_tmp34));_tmp34->name=t->name;
_tmp34->identity=- 1;_tmp34->kind=(void*)k;_tmp34;});}static struct Cyc_Absyn_Tvar*
Cyc_Parse_typ2tvar(struct Cyc_Position_Segment*loc,void*t){void*_tmp35=t;struct
Cyc_Absyn_Tvar*_tmp36;_LL17: if(_tmp35 <= (void*)4)goto _LL19;if(*((int*)_tmp35)!= 
1)goto _LL19;_tmp36=((struct Cyc_Absyn_VarType_struct*)_tmp35)->f1;_LL18: return
_tmp36;_LL19:;_LL1A:({void*_tmp37=0;((int(*)(struct Cyc_Position_Segment*sg,
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(loc,({const char*
_tmp38="expecting a list of type variables, not types";_tag_dyneither(_tmp38,
sizeof(char),46);}),_tag_dyneither(_tmp37,sizeof(void*),0));});_LL16:;}static
void*Cyc_Parse_tvar2typ(struct Cyc_Absyn_Tvar*pr){return(void*)({struct Cyc_Absyn_VarType_struct*
_tmp39=_cycalloc(sizeof(*_tmp39));_tmp39[0]=({struct Cyc_Absyn_VarType_struct
_tmp3A;_tmp3A.tag=1;_tmp3A.f1=pr;_tmp3A;});_tmp39;});}static void Cyc_Parse_set_vartyp_kind(
void*t,void*k,int leq){void*_tmp3B=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*
_tmp3C;struct Cyc_Absyn_Tvar _tmp3D;void*_tmp3E;void**_tmp3F;_LL1C: if(_tmp3B <= (
void*)4)goto _LL1E;if(*((int*)_tmp3B)!= 1)goto _LL1E;_tmp3C=((struct Cyc_Absyn_VarType_struct*)
_tmp3B)->f1;_tmp3D=*_tmp3C;_tmp3E=(void*)_tmp3D.kind;_tmp3F=(void**)&(*((struct
Cyc_Absyn_VarType_struct*)_tmp3B)->f1).kind;_LL1D: {void*_tmp40=Cyc_Absyn_compress_kb(*
_tmp3F);_LL21: if(*((int*)_tmp40)!= 1)goto _LL23;_LL22: if(!leq)*_tmp3F=Cyc_Tcutil_kind_to_bound(
k);else{*_tmp3F=(void*)({struct Cyc_Absyn_Less_kb_struct*_tmp41=_cycalloc(sizeof(*
_tmp41));_tmp41[0]=({struct Cyc_Absyn_Less_kb_struct _tmp42;_tmp42.tag=2;_tmp42.f1=
0;_tmp42.f2=(void*)k;_tmp42;});_tmp41;});}return;_LL23:;_LL24: return;_LL20:;}
_LL1E:;_LL1F: return;_LL1B:;}static struct Cyc_List_List*Cyc_Parse_oldstyle2newstyle(
struct Cyc_List_List*tms,struct Cyc_List_List*tds,struct Cyc_Position_Segment*loc){
if(tds == 0)return tms;if(tms == 0)return 0;{void*_tmp43=(void*)tms->hd;void*_tmp44;
_LL26: if(*((int*)_tmp43)!= 3)goto _LL28;_tmp44=(void*)((struct Cyc_Absyn_Function_mod_struct*)
_tmp43)->f1;_LL27: if(tms->tl == 0  || Cyc_Parse_is_typeparam((void*)((struct Cyc_List_List*)
_check_null(tms->tl))->hd) && ((struct Cyc_List_List*)_check_null(tms->tl))->tl == 
0){void*_tmp45=_tmp44;struct Cyc_List_List*_tmp46;_LL2B: if(*((int*)_tmp45)!= 1)
goto _LL2D;_LL2C:({void*_tmp47=0;Cyc_Tcutil_warn(loc,({const char*_tmp48="function declaration with both new- and old-style parameter declarations; ignoring old-style";
_tag_dyneither(_tmp48,sizeof(char),93);}),_tag_dyneither(_tmp47,sizeof(void*),0));});
return tms;_LL2D: if(*((int*)_tmp45)!= 0)goto _LL2A;_tmp46=((struct Cyc_Absyn_NoTypes_struct*)
_tmp45)->f1;_LL2E: if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp46)!= ((
int(*)(struct Cyc_List_List*x))Cyc_List_length)(tds))({void*_tmp49=0;((int(*)(
struct Cyc_Position_Segment*sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Parse_abort)(loc,({const char*_tmp4A="wrong number of parameter declarations in old-style function declaration";
_tag_dyneither(_tmp4A,sizeof(char),73);}),_tag_dyneither(_tmp49,sizeof(void*),0));});{
struct Cyc_List_List*_tmp4B=0;for(0;_tmp46 != 0;_tmp46=_tmp46->tl){struct Cyc_List_List*
_tmp4C=tds;for(0;_tmp4C != 0;_tmp4C=_tmp4C->tl){struct Cyc_Absyn_Decl*_tmp4D=(
struct Cyc_Absyn_Decl*)_tmp4C->hd;void*_tmp4E=(void*)_tmp4D->r;struct Cyc_Absyn_Vardecl*
_tmp4F;_LL30: if(_tmp4E <= (void*)2)goto _LL32;if(*((int*)_tmp4E)!= 0)goto _LL32;
_tmp4F=((struct Cyc_Absyn_Var_d_struct*)_tmp4E)->f1;_LL31: if(Cyc_zstrptrcmp((*
_tmp4F->name).f2,(struct _dyneither_ptr*)_tmp46->hd)!= 0)continue;if(_tmp4F->initializer
!= 0)({void*_tmp50=0;((int(*)(struct Cyc_Position_Segment*sg,struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(_tmp4D->loc,({const char*_tmp51="initializer found in parameter declaration";
_tag_dyneither(_tmp51,sizeof(char),43);}),_tag_dyneither(_tmp50,sizeof(void*),0));});
if(Cyc_Absyn_is_qvar_qualified(_tmp4F->name))({void*_tmp52=0;((int(*)(struct Cyc_Position_Segment*
sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(_tmp4D->loc,({
const char*_tmp53="namespaces forbidden in parameter declarations";_tag_dyneither(
_tmp53,sizeof(char),47);}),_tag_dyneither(_tmp52,sizeof(void*),0));});_tmp4B=({
struct Cyc_List_List*_tmp54=_cycalloc(sizeof(*_tmp54));_tmp54->hd=({struct _tuple2*
_tmp55=_cycalloc(sizeof(*_tmp55));_tmp55->f1=({struct Cyc_Core_Opt*_tmp56=
_cycalloc(sizeof(*_tmp56));_tmp56->v=(*_tmp4F->name).f2;_tmp56;});_tmp55->f2=
_tmp4F->tq;_tmp55->f3=(void*)_tmp4F->type;_tmp55;});_tmp54->tl=_tmp4B;_tmp54;});
goto L;_LL32:;_LL33:({void*_tmp57=0;((int(*)(struct Cyc_Position_Segment*sg,struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(_tmp4D->loc,({const
char*_tmp58="nonvariable declaration in parameter type";_tag_dyneither(_tmp58,
sizeof(char),42);}),_tag_dyneither(_tmp57,sizeof(void*),0));});_LL2F:;}L: if(
_tmp4C == 0)({struct Cyc_String_pa_struct _tmp5B;_tmp5B.tag=0;_tmp5B.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)_tmp46->hd));{
void*_tmp59[1]={& _tmp5B};((int(*)(struct Cyc_Position_Segment*sg,struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(loc,({const char*
_tmp5A="%s is not given a type";_tag_dyneither(_tmp5A,sizeof(char),23);}),
_tag_dyneither(_tmp59,sizeof(void*),1));}});}return({struct Cyc_List_List*_tmp5C=
_cycalloc(sizeof(*_tmp5C));_tmp5C->hd=(void*)((void*)({struct Cyc_Absyn_Function_mod_struct*
_tmp5D=_cycalloc(sizeof(*_tmp5D));_tmp5D[0]=({struct Cyc_Absyn_Function_mod_struct
_tmp5E;_tmp5E.tag=3;_tmp5E.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*
_tmp5F=_cycalloc(sizeof(*_tmp5F));_tmp5F[0]=({struct Cyc_Absyn_WithTypes_struct
_tmp60;_tmp60.tag=1;_tmp60.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(_tmp4B);_tmp60.f2=0;_tmp60.f3=0;_tmp60.f4=0;_tmp60.f5=0;_tmp60;});
_tmp5F;}));_tmp5E;});_tmp5D;}));_tmp5C->tl=0;_tmp5C;});}_LL2A:;}goto _LL29;_LL28:;
_LL29: return({struct Cyc_List_List*_tmp61=_cycalloc(sizeof(*_tmp61));_tmp61->hd=(
void*)((void*)tms->hd);_tmp61->tl=Cyc_Parse_oldstyle2newstyle(tms->tl,tds,loc);
_tmp61;});_LL25:;}}struct _tuple10{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual
f2;void*f3;};static struct Cyc_Absyn_Fndecl*Cyc_Parse_make_function(struct Cyc_Core_Opt*
dso,struct Cyc_Parse_Declarator*d,struct Cyc_List_List*tds,struct Cyc_Absyn_Stmt*
body,struct Cyc_Position_Segment*loc){if(tds != 0)d=({struct Cyc_Parse_Declarator*
_tmp62=_cycalloc(sizeof(*_tmp62));_tmp62->id=d->id;_tmp62->tms=Cyc_Parse_oldstyle2newstyle(
d->tms,tds,loc);_tmp62;});{void*sc=(void*)2;struct Cyc_List_List*tss=0;struct Cyc_Absyn_Tqual
tq=Cyc_Absyn_empty_tqual(0);int is_inline=0;struct Cyc_List_List*atts=0;if(dso != 0){
tss=((struct Cyc_Parse_Declaration_spec*)dso->v)->type_specs;tq=((struct Cyc_Parse_Declaration_spec*)
dso->v)->tq;is_inline=((struct Cyc_Parse_Declaration_spec*)dso->v)->is_inline;
atts=((struct Cyc_Parse_Declaration_spec*)dso->v)->attributes;if(((struct Cyc_Parse_Declaration_spec*)
dso->v)->sc != 0){void*_tmp63=(void*)((struct Cyc_Core_Opt*)_check_null(((struct
Cyc_Parse_Declaration_spec*)dso->v)->sc))->v;_LL35: if((int)_tmp63 != 1)goto _LL37;
_LL36: sc=(void*)3;goto _LL34;_LL37: if((int)_tmp63 != 3)goto _LL39;_LL38: sc=(void*)0;
goto _LL34;_LL39:;_LL3A: Cyc_Parse_err(({const char*_tmp64="bad storage class on function";
_tag_dyneither(_tmp64,sizeof(char),30);}),loc);goto _LL34;_LL34:;}}{void*_tmp66;
struct Cyc_Core_Opt*_tmp67;struct _tuple5 _tmp65=Cyc_Parse_collapse_type_specifiers(
tss,loc);_tmp66=_tmp65.f1;_tmp67=_tmp65.f2;{struct Cyc_Absyn_Tqual _tmp69;void*
_tmp6A;struct Cyc_List_List*_tmp6B;struct Cyc_List_List*_tmp6C;struct _tuple6 _tmp68=
Cyc_Parse_apply_tms(tq,_tmp66,atts,d->tms);_tmp69=_tmp68.f1;_tmp6A=_tmp68.f2;
_tmp6B=_tmp68.f3;_tmp6C=_tmp68.f4;if(_tmp67 != 0)({void*_tmp6D=0;Cyc_Tcutil_warn(
loc,({const char*_tmp6E="nested type declaration within function prototype";
_tag_dyneither(_tmp6E,sizeof(char),50);}),_tag_dyneither(_tmp6D,sizeof(void*),0));});
if(_tmp6B != 0)({void*_tmp6F=0;Cyc_Tcutil_warn(loc,({const char*_tmp70="bad type params, ignoring";
_tag_dyneither(_tmp70,sizeof(char),26);}),_tag_dyneither(_tmp6F,sizeof(void*),0));});{
void*_tmp71=_tmp6A;struct Cyc_Absyn_FnInfo _tmp72;struct Cyc_List_List*_tmp73;
struct Cyc_Core_Opt*_tmp74;void*_tmp75;struct Cyc_List_List*_tmp76;int _tmp77;
struct Cyc_Absyn_VarargInfo*_tmp78;struct Cyc_List_List*_tmp79;struct Cyc_List_List*
_tmp7A;_LL3C: if(_tmp71 <= (void*)4)goto _LL3E;if(*((int*)_tmp71)!= 8)goto _LL3E;
_tmp72=((struct Cyc_Absyn_FnType_struct*)_tmp71)->f1;_tmp73=_tmp72.tvars;_tmp74=
_tmp72.effect;_tmp75=(void*)_tmp72.ret_typ;_tmp76=_tmp72.args;_tmp77=_tmp72.c_varargs;
_tmp78=_tmp72.cyc_varargs;_tmp79=_tmp72.rgn_po;_tmp7A=_tmp72.attributes;_LL3D: {
struct Cyc_List_List*_tmp7B=0;{struct Cyc_List_List*_tmp7C=_tmp76;for(0;_tmp7C != 0;
_tmp7C=_tmp7C->tl){struct _tuple2 _tmp7E;struct Cyc_Core_Opt*_tmp7F;struct Cyc_Absyn_Tqual
_tmp80;void*_tmp81;struct _tuple2*_tmp7D=(struct _tuple2*)_tmp7C->hd;_tmp7E=*
_tmp7D;_tmp7F=_tmp7E.f1;_tmp80=_tmp7E.f2;_tmp81=_tmp7E.f3;if(_tmp7F == 0){Cyc_Parse_err(({
const char*_tmp82="missing argument variable in function prototype";
_tag_dyneither(_tmp82,sizeof(char),48);}),loc);_tmp7B=({struct Cyc_List_List*
_tmp83=_cycalloc(sizeof(*_tmp83));_tmp83->hd=({struct _tuple10*_tmp84=_cycalloc(
sizeof(*_tmp84));_tmp84->f1=_init_dyneither_ptr(_cycalloc(sizeof(struct
_dyneither_ptr)),"?",sizeof(char),2);_tmp84->f2=_tmp80;_tmp84->f3=_tmp81;_tmp84;});
_tmp83->tl=_tmp7B;_tmp83;});}else{_tmp7B=({struct Cyc_List_List*_tmp86=_cycalloc(
sizeof(*_tmp86));_tmp86->hd=({struct _tuple10*_tmp87=_cycalloc(sizeof(*_tmp87));
_tmp87->f1=(struct _dyneither_ptr*)_tmp7F->v;_tmp87->f2=_tmp80;_tmp87->f3=_tmp81;
_tmp87;});_tmp86->tl=_tmp7B;_tmp86;});}}}return({struct Cyc_Absyn_Fndecl*_tmp88=
_cycalloc(sizeof(*_tmp88));_tmp88->sc=(void*)sc;_tmp88->name=d->id;_tmp88->tvs=
_tmp73;_tmp88->is_inline=is_inline;_tmp88->effect=_tmp74;_tmp88->ret_type=(void*)
_tmp75;_tmp88->args=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmp7B);_tmp88->c_varargs=_tmp77;_tmp88->cyc_varargs=_tmp78;_tmp88->rgn_po=
_tmp79;_tmp88->body=body;_tmp88->cached_typ=0;_tmp88->param_vardecls=0;_tmp88->fn_vardecl=
0;_tmp88->attributes=Cyc_List_append(_tmp7A,_tmp6C);_tmp88;});}_LL3E:;_LL3F:({
void*_tmp89=0;((int(*)(struct Cyc_Position_Segment*sg,struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Parse_abort)(loc,({const char*_tmp8A="declarator is not a function prototype";
_tag_dyneither(_tmp8A,sizeof(char),39);}),_tag_dyneither(_tmp89,sizeof(void*),0));});
_LL3B:;}}}}}static char _tmp8B[52]="at most one type may appear within a type specifier";
static struct _dyneither_ptr Cyc_Parse_msg1={_tmp8B,_tmp8B,_tmp8B + 52};static char
_tmp8C[63]="const or volatile may appear only once within a type specifier";
static struct _dyneither_ptr Cyc_Parse_msg2={_tmp8C,_tmp8C,_tmp8C + 63};static char
_tmp8D[50]="type specifier includes more than one declaration";static struct
_dyneither_ptr Cyc_Parse_msg3={_tmp8D,_tmp8D,_tmp8D + 50};static char _tmp8E[60]="sign specifier may appear only once within a type specifier";
static struct _dyneither_ptr Cyc_Parse_msg4={_tmp8E,_tmp8E,_tmp8E + 60};static struct
_tuple5 Cyc_Parse_collapse_type_specifiers(struct Cyc_List_List*ts,struct Cyc_Position_Segment*
loc){struct Cyc_Core_Opt*declopt=0;int seen_type=0;int seen_sign=0;int seen_size=0;
void*t=(void*)0;void*sz=(void*)2;void*sgn=(void*)0;struct Cyc_Position_Segment*
last_loc=loc;for(0;ts != 0;ts=ts->tl){void*_tmp8F=(void*)ts->hd;void*_tmp90;
struct Cyc_Position_Segment*_tmp91;struct Cyc_Position_Segment*_tmp92;struct Cyc_Position_Segment*
_tmp93;struct Cyc_Position_Segment*_tmp94;struct Cyc_Position_Segment*_tmp95;
struct Cyc_Absyn_Decl*_tmp96;_LL41: if(*((int*)_tmp8F)!= 4)goto _LL43;_tmp90=(void*)((
struct Cyc_Parse_Type_spec_struct*)_tmp8F)->f1;_tmp91=((struct Cyc_Parse_Type_spec_struct*)
_tmp8F)->f2;_LL42: if(seen_type)Cyc_Parse_err(Cyc_Parse_msg1,_tmp91);last_loc=
_tmp91;seen_type=1;t=_tmp90;goto _LL40;_LL43: if(*((int*)_tmp8F)!= 0)goto _LL45;
_tmp92=((struct Cyc_Parse_Signed_spec_struct*)_tmp8F)->f1;_LL44: if(seen_sign)Cyc_Parse_err(
Cyc_Parse_msg4,_tmp92);if(seen_type)Cyc_Parse_err(({const char*_tmp97="signed qualifier must come before type";
_tag_dyneither(_tmp97,sizeof(char),39);}),_tmp92);last_loc=_tmp92;seen_sign=1;
sgn=(void*)0;goto _LL40;_LL45: if(*((int*)_tmp8F)!= 1)goto _LL47;_tmp93=((struct Cyc_Parse_Unsigned_spec_struct*)
_tmp8F)->f1;_LL46: if(seen_sign)Cyc_Parse_err(Cyc_Parse_msg4,_tmp93);if(seen_type)
Cyc_Parse_err(({const char*_tmp98="signed qualifier must come before type";
_tag_dyneither(_tmp98,sizeof(char),39);}),_tmp93);last_loc=_tmp93;seen_sign=1;
sgn=(void*)1;goto _LL40;_LL47: if(*((int*)_tmp8F)!= 2)goto _LL49;_tmp94=((struct Cyc_Parse_Short_spec_struct*)
_tmp8F)->f1;_LL48: if(seen_size)Cyc_Parse_err(({const char*_tmp99="integral size may appear only once within a type specifier";
_tag_dyneither(_tmp99,sizeof(char),59);}),_tmp94);if(seen_type)Cyc_Parse_err(({
const char*_tmp9A="short modifier must come before base type";_tag_dyneither(
_tmp9A,sizeof(char),42);}),_tmp94);last_loc=_tmp94;seen_size=1;sz=(void*)1;goto
_LL40;_LL49: if(*((int*)_tmp8F)!= 3)goto _LL4B;_tmp95=((struct Cyc_Parse_Long_spec_struct*)
_tmp8F)->f1;_LL4A: if(seen_type)Cyc_Parse_err(({const char*_tmp9B="long modifier must come before base type";
_tag_dyneither(_tmp9B,sizeof(char),41);}),_tmp95);if(seen_size){void*_tmp9C=sz;
_LL4E: if((int)_tmp9C != 3)goto _LL50;_LL4F: sz=(void*)4;goto _LL4D;_LL50:;_LL51: Cyc_Parse_err(({
const char*_tmp9D="extra long in type specifier";_tag_dyneither(_tmp9D,sizeof(
char),29);}),_tmp95);goto _LL4D;_LL4D:;}else{sz=(void*)3;}last_loc=_tmp95;
seen_size=1;goto _LL40;_LL4B: if(*((int*)_tmp8F)!= 5)goto _LL40;_tmp96=((struct Cyc_Parse_Decl_spec_struct*)
_tmp8F)->f1;_LL4C: last_loc=_tmp96->loc;if(declopt == 0  && !seen_type){seen_type=1;{
void*_tmp9E=(void*)_tmp96->r;struct Cyc_Absyn_Aggrdecl*_tmp9F;struct Cyc_Absyn_Tuniondecl*
_tmpA0;struct Cyc_Absyn_Enumdecl*_tmpA1;_LL53: if(_tmp9E <= (void*)2)goto _LL59;if(*((
int*)_tmp9E)!= 6)goto _LL55;_tmp9F=((struct Cyc_Absyn_Aggr_d_struct*)_tmp9E)->f1;
_LL54: {struct Cyc_List_List*_tmpA2=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Parse_tvar2typ,((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Parse_copy_tvar,_tmp9F->tvs));t=(void*)({struct Cyc_Absyn_AggrType_struct*
_tmpA3=_cycalloc(sizeof(*_tmpA3));_tmpA3[0]=({struct Cyc_Absyn_AggrType_struct
_tmpA4;_tmpA4.tag=10;_tmpA4.f1=({struct Cyc_Absyn_AggrInfo _tmpA5;_tmpA5.aggr_info=(
union Cyc_Absyn_AggrInfoU_union)({union Cyc_Absyn_AggrInfoU_union _tmpA6;(_tmpA6.UnknownAggr).tag=
0;(_tmpA6.UnknownAggr).f1=(void*)((void*)_tmp9F->kind);(_tmpA6.UnknownAggr).f2=
_tmp9F->name;_tmpA6;});_tmpA5.targs=_tmpA2;_tmpA5;});_tmpA4;});_tmpA3;});if(
_tmp9F->impl != 0)declopt=({struct Cyc_Core_Opt*_tmpA7=_cycalloc(sizeof(*_tmpA7));
_tmpA7->v=_tmp96;_tmpA7;});goto _LL52;}_LL55: if(*((int*)_tmp9E)!= 7)goto _LL57;
_tmpA0=((struct Cyc_Absyn_Tunion_d_struct*)_tmp9E)->f1;_LL56: {struct Cyc_List_List*
_tmpA8=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Parse_tvar2typ,((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*
f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Parse_copy_tvar,
_tmpA0->tvs));struct Cyc_Core_Opt*ropt=_tmpA0->is_flat?0:({struct Cyc_Core_Opt*
_tmpAF=_cycalloc(sizeof(*_tmpAF));_tmpAF->v=(void*)((void*)2);_tmpAF;});t=(void*)({
struct Cyc_Absyn_TunionType_struct*_tmpA9=_cycalloc(sizeof(*_tmpA9));_tmpA9[0]=({
struct Cyc_Absyn_TunionType_struct _tmpAA;_tmpAA.tag=2;_tmpAA.f1=({struct Cyc_Absyn_TunionInfo
_tmpAB;_tmpAB.tunion_info=(union Cyc_Absyn_TunionInfoU_union)({union Cyc_Absyn_TunionInfoU_union
_tmpAC;(_tmpAC.KnownTunion).tag=1;(_tmpAC.KnownTunion).f1=({struct Cyc_Absyn_Tuniondecl**
_tmpAD=_cycalloc(sizeof(*_tmpAD));_tmpAD[0]=_tmpA0;_tmpAD;});_tmpAC;});_tmpAB.targs=
_tmpA8;_tmpAB.rgn=ropt;_tmpAB;});_tmpAA;});_tmpA9;});if(_tmpA0->fields != 0)
declopt=({struct Cyc_Core_Opt*_tmpAE=_cycalloc(sizeof(*_tmpAE));_tmpAE->v=_tmp96;
_tmpAE;});goto _LL52;}_LL57: if(*((int*)_tmp9E)!= 8)goto _LL59;_tmpA1=((struct Cyc_Absyn_Enum_d_struct*)
_tmp9E)->f1;_LL58: t=(void*)({struct Cyc_Absyn_EnumType_struct*_tmpB0=_cycalloc(
sizeof(*_tmpB0));_tmpB0[0]=({struct Cyc_Absyn_EnumType_struct _tmpB1;_tmpB1.tag=12;
_tmpB1.f1=_tmpA1->name;_tmpB1.f2=0;_tmpB1;});_tmpB0;});declopt=({struct Cyc_Core_Opt*
_tmpB2=_cycalloc(sizeof(*_tmpB2));_tmpB2->v=_tmp96;_tmpB2;});goto _LL52;_LL59:;
_LL5A:({void*_tmpB3=0;((int(*)(struct Cyc_Position_Segment*sg,struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(_tmp96->loc,({const
char*_tmpB4="bad declaration within type specifier";_tag_dyneither(_tmpB4,
sizeof(char),38);}),_tag_dyneither(_tmpB3,sizeof(void*),0));});_LL52:;}}else{Cyc_Parse_err(
Cyc_Parse_msg3,_tmp96->loc);}goto _LL40;_LL40:;}if(!seen_type){if(!seen_sign  && !
seen_size)({void*_tmpB5=0;Cyc_Tcutil_warn(last_loc,({const char*_tmpB6="missing type within specifier";
_tag_dyneither(_tmpB6,sizeof(char),30);}),_tag_dyneither(_tmpB5,sizeof(void*),0));});
t=Cyc_Absyn_int_typ(sgn,sz);}else{if(seen_sign){void*_tmpB7=t;void*_tmpB8;void*
_tmpB9;_LL5C: if(_tmpB7 <= (void*)4)goto _LL5E;if(*((int*)_tmpB7)!= 5)goto _LL5E;
_tmpB8=(void*)((struct Cyc_Absyn_IntType_struct*)_tmpB7)->f1;_tmpB9=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmpB7)->f2;_LL5D: if(_tmpB8 != sgn)t=Cyc_Absyn_int_typ(
sgn,_tmpB9);goto _LL5B;_LL5E:;_LL5F: Cyc_Parse_err(({const char*_tmpBA="sign specification on non-integral type";
_tag_dyneither(_tmpBA,sizeof(char),40);}),last_loc);goto _LL5B;_LL5B:;}if(
seen_size){void*_tmpBB=t;void*_tmpBC;void*_tmpBD;_LL61: if(_tmpBB <= (void*)4)goto
_LL65;if(*((int*)_tmpBB)!= 5)goto _LL63;_tmpBC=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmpBB)->f1;_tmpBD=(void*)((struct Cyc_Absyn_IntType_struct*)_tmpBB)->f2;_LL62:
if(_tmpBD != sz)t=Cyc_Absyn_int_typ(_tmpBC,sz);goto _LL60;_LL63: if(*((int*)_tmpBB)
!= 6)goto _LL65;_LL64: t=Cyc_Absyn_double_typ(1);goto _LL60;_LL65:;_LL66: Cyc_Parse_err(({
const char*_tmpBE="size qualifier on non-integral type";_tag_dyneither(_tmpBE,
sizeof(char),36);}),last_loc);goto _LL60;_LL60:;}}return({struct _tuple5 _tmpBF;
_tmpBF.f1=t;_tmpBF.f2=declopt;_tmpBF;});}static struct Cyc_List_List*Cyc_Parse_apply_tmss(
struct _RegionHandle*r,struct Cyc_Absyn_Tqual tq,void*t,struct Cyc_List_List*ds,
struct Cyc_List_List*shared_atts){if(ds == 0)return 0;{struct Cyc_Parse_Declarator*
_tmpC0=(struct Cyc_Parse_Declarator*)ds->hd;struct _tuple1*_tmpC1=_tmpC0->id;
struct Cyc_Absyn_Tqual _tmpC3;void*_tmpC4;struct Cyc_List_List*_tmpC5;struct Cyc_List_List*
_tmpC6;struct _tuple6 _tmpC2=Cyc_Parse_apply_tms(tq,t,shared_atts,_tmpC0->tms);
_tmpC3=_tmpC2.f1;_tmpC4=_tmpC2.f2;_tmpC5=_tmpC2.f3;_tmpC6=_tmpC2.f4;if(ds->tl == 
0)return({struct Cyc_List_List*_tmpC7=_region_malloc(r,sizeof(*_tmpC7));_tmpC7->hd=({
struct _tuple7*_tmpC8=_region_malloc(r,sizeof(*_tmpC8));_tmpC8->f1=_tmpC1;_tmpC8->f2=
_tmpC3;_tmpC8->f3=_tmpC4;_tmpC8->f4=_tmpC5;_tmpC8->f5=_tmpC6;_tmpC8;});_tmpC7->tl=
0;_tmpC7;});else{return({struct Cyc_List_List*_tmpC9=_region_malloc(r,sizeof(*
_tmpC9));_tmpC9->hd=({struct _tuple7*_tmpCA=_region_malloc(r,sizeof(*_tmpCA));
_tmpCA->f1=_tmpC1;_tmpCA->f2=_tmpC3;_tmpCA->f3=_tmpC4;_tmpCA->f4=_tmpC5;_tmpCA->f5=
_tmpC6;_tmpCA;});_tmpC9->tl=Cyc_Parse_apply_tmss(r,_tmpC3,Cyc_Tcutil_copy_type(t),
ds->tl,shared_atts);_tmpC9;});}}}static struct _tuple6 Cyc_Parse_apply_tms(struct
Cyc_Absyn_Tqual tq,void*t,struct Cyc_List_List*atts,struct Cyc_List_List*tms){if(
tms == 0)return({struct _tuple6 _tmpCB;_tmpCB.f1=tq;_tmpCB.f2=t;_tmpCB.f3=0;_tmpCB.f4=
atts;_tmpCB;});{void*_tmpCC=(void*)tms->hd;struct Cyc_Absyn_Conref*_tmpCD;struct
Cyc_Position_Segment*_tmpCE;struct Cyc_Absyn_Exp*_tmpCF;struct Cyc_Absyn_Conref*
_tmpD0;struct Cyc_Position_Segment*_tmpD1;void*_tmpD2;struct Cyc_List_List*_tmpD3;
struct Cyc_Position_Segment*_tmpD4;struct Cyc_Absyn_PtrAtts _tmpD5;struct Cyc_Absyn_Tqual
_tmpD6;struct Cyc_Position_Segment*_tmpD7;struct Cyc_List_List*_tmpD8;_LL68: if(*((
int*)_tmpCC)!= 0)goto _LL6A;_tmpCD=((struct Cyc_Absyn_Carray_mod_struct*)_tmpCC)->f1;
_tmpCE=((struct Cyc_Absyn_Carray_mod_struct*)_tmpCC)->f2;_LL69: return Cyc_Parse_apply_tms(
Cyc_Absyn_empty_tqual(0),Cyc_Absyn_array_typ(t,tq,0,_tmpCD,_tmpCE),atts,tms->tl);
_LL6A: if(*((int*)_tmpCC)!= 1)goto _LL6C;_tmpCF=((struct Cyc_Absyn_ConstArray_mod_struct*)
_tmpCC)->f1;_tmpD0=((struct Cyc_Absyn_ConstArray_mod_struct*)_tmpCC)->f2;_tmpD1=((
struct Cyc_Absyn_ConstArray_mod_struct*)_tmpCC)->f3;_LL6B: return Cyc_Parse_apply_tms(
Cyc_Absyn_empty_tqual(0),Cyc_Absyn_array_typ(t,tq,(struct Cyc_Absyn_Exp*)_tmpCF,
_tmpD0,_tmpD1),atts,tms->tl);_LL6C: if(*((int*)_tmpCC)!= 3)goto _LL6E;_tmpD2=(void*)((
struct Cyc_Absyn_Function_mod_struct*)_tmpCC)->f1;_LL6D: {void*_tmpD9=_tmpD2;
struct Cyc_List_List*_tmpDA;int _tmpDB;struct Cyc_Absyn_VarargInfo*_tmpDC;struct Cyc_Core_Opt*
_tmpDD;struct Cyc_List_List*_tmpDE;struct Cyc_Position_Segment*_tmpDF;_LL75: if(*((
int*)_tmpD9)!= 1)goto _LL77;_tmpDA=((struct Cyc_Absyn_WithTypes_struct*)_tmpD9)->f1;
_tmpDB=((struct Cyc_Absyn_WithTypes_struct*)_tmpD9)->f2;_tmpDC=((struct Cyc_Absyn_WithTypes_struct*)
_tmpD9)->f3;_tmpDD=((struct Cyc_Absyn_WithTypes_struct*)_tmpD9)->f4;_tmpDE=((
struct Cyc_Absyn_WithTypes_struct*)_tmpD9)->f5;_LL76: {struct Cyc_List_List*
typvars=0;struct Cyc_List_List*fn_atts=0;struct Cyc_List_List*new_atts=0;{struct
Cyc_List_List*as=atts;for(0;as != 0;as=as->tl){if(Cyc_Absyn_fntype_att((void*)as->hd))
fn_atts=({struct Cyc_List_List*_tmpE0=_cycalloc(sizeof(*_tmpE0));_tmpE0->hd=(void*)((
void*)as->hd);_tmpE0->tl=fn_atts;_tmpE0;});else{new_atts=({struct Cyc_List_List*
_tmpE1=_cycalloc(sizeof(*_tmpE1));_tmpE1->hd=(void*)((void*)as->hd);_tmpE1->tl=
new_atts;_tmpE1;});}}}if(tms->tl != 0){void*_tmpE2=(void*)((struct Cyc_List_List*)
_check_null(tms->tl))->hd;struct Cyc_List_List*_tmpE3;_LL7A: if(*((int*)_tmpE2)!= 
4)goto _LL7C;_tmpE3=((struct Cyc_Absyn_TypeParams_mod_struct*)_tmpE2)->f1;_LL7B:
typvars=_tmpE3;tms=tms->tl;goto _LL79;_LL7C:;_LL7D: goto _LL79;_LL79:;}if(((((!
_tmpDB  && _tmpDC == 0) && _tmpDA != 0) && _tmpDA->tl == 0) && (*((struct _tuple2*)
_tmpDA->hd)).f1 == 0) && (*((struct _tuple2*)_tmpDA->hd)).f3 == (void*)0)_tmpDA=0;t=
Cyc_Parse_array2ptr(t,0);((void(*)(void(*f)(struct _tuple2*),struct Cyc_List_List*
x))Cyc_List_iter)(Cyc_Parse_arg_array2ptr,_tmpDA);return Cyc_Parse_apply_tms(Cyc_Absyn_empty_tqual(
tq.loc),Cyc_Absyn_function_typ(typvars,_tmpDD,t,_tmpDA,_tmpDB,_tmpDC,_tmpDE,
fn_atts),new_atts,((struct Cyc_List_List*)_check_null(tms))->tl);}_LL77: if(*((int*)
_tmpD9)!= 0)goto _LL74;_tmpDF=((struct Cyc_Absyn_NoTypes_struct*)_tmpD9)->f2;_LL78:({
void*_tmpE4=0;((int(*)(struct Cyc_Position_Segment*sg,struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Parse_abort)(_tmpDF,({const char*_tmpE5="function declaration without parameter types";
_tag_dyneither(_tmpE5,sizeof(char),45);}),_tag_dyneither(_tmpE4,sizeof(void*),0));});
_LL74:;}_LL6E: if(*((int*)_tmpCC)!= 4)goto _LL70;_tmpD3=((struct Cyc_Absyn_TypeParams_mod_struct*)
_tmpCC)->f1;_tmpD4=((struct Cyc_Absyn_TypeParams_mod_struct*)_tmpCC)->f2;_LL6F:
if(tms->tl == 0)return({struct _tuple6 _tmpE6;_tmpE6.f1=tq;_tmpE6.f2=t;_tmpE6.f3=
_tmpD3;_tmpE6.f4=atts;_tmpE6;});({void*_tmpE7=0;((int(*)(struct Cyc_Position_Segment*
sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(_tmpD4,({
const char*_tmpE8="type parameters must appear before function arguments in declarator";
_tag_dyneither(_tmpE8,sizeof(char),68);}),_tag_dyneither(_tmpE7,sizeof(void*),0));});
_LL70: if(*((int*)_tmpCC)!= 2)goto _LL72;_tmpD5=((struct Cyc_Absyn_Pointer_mod_struct*)
_tmpCC)->f1;_tmpD6=((struct Cyc_Absyn_Pointer_mod_struct*)_tmpCC)->f2;_LL71:
return Cyc_Parse_apply_tms(_tmpD6,(void*)({struct Cyc_Absyn_PointerType_struct*
_tmpE9=_cycalloc(sizeof(*_tmpE9));_tmpE9[0]=({struct Cyc_Absyn_PointerType_struct
_tmpEA;_tmpEA.tag=4;_tmpEA.f1=({struct Cyc_Absyn_PtrInfo _tmpEB;_tmpEB.elt_typ=(
void*)t;_tmpEB.elt_tq=tq;_tmpEB.ptr_atts=_tmpD5;_tmpEB;});_tmpEA;});_tmpE9;}),
atts,tms->tl);_LL72: if(*((int*)_tmpCC)!= 5)goto _LL67;_tmpD7=((struct Cyc_Absyn_Attributes_mod_struct*)
_tmpCC)->f1;_tmpD8=((struct Cyc_Absyn_Attributes_mod_struct*)_tmpCC)->f2;_LL73:
return Cyc_Parse_apply_tms(tq,t,Cyc_List_append(atts,_tmpD8),tms->tl);_LL67:;}}
void*Cyc_Parse_speclist2typ(struct Cyc_List_List*tss,struct Cyc_Position_Segment*
loc){void*_tmpED;struct Cyc_Core_Opt*_tmpEE;struct _tuple5 _tmpEC=Cyc_Parse_collapse_type_specifiers(
tss,loc);_tmpED=_tmpEC.f1;_tmpEE=_tmpEC.f2;if(_tmpEE != 0)({void*_tmpEF=0;Cyc_Tcutil_warn(
loc,({const char*_tmpF0="ignoring nested type declaration(s) in specifier list";
_tag_dyneither(_tmpF0,sizeof(char),54);}),_tag_dyneither(_tmpEF,sizeof(void*),0));});
return _tmpED;}static struct Cyc_Absyn_Decl*Cyc_Parse_v_typ_to_typedef(struct Cyc_Position_Segment*
loc,struct _tuple7*t){struct _tuple7 _tmpF2;struct _tuple1*_tmpF3;struct Cyc_Absyn_Tqual
_tmpF4;void*_tmpF5;struct Cyc_List_List*_tmpF6;struct Cyc_List_List*_tmpF7;struct
_tuple7*_tmpF1=t;_tmpF2=*_tmpF1;_tmpF3=_tmpF2.f1;_tmpF4=_tmpF2.f2;_tmpF5=_tmpF2.f3;
_tmpF6=_tmpF2.f4;_tmpF7=_tmpF2.f5;Cyc_Lex_register_typedef(_tmpF3);{struct Cyc_Core_Opt*
kind;struct Cyc_Core_Opt*type;{void*_tmpF8=_tmpF5;struct Cyc_Core_Opt*_tmpF9;_LL7F:
if(_tmpF8 <= (void*)4)goto _LL81;if(*((int*)_tmpF8)!= 0)goto _LL81;_tmpF9=((struct
Cyc_Absyn_Evar_struct*)_tmpF8)->f1;_LL80: type=0;if(_tmpF9 == 0)kind=(struct Cyc_Core_Opt*)&
Cyc_Tcutil_bk;else{kind=_tmpF9;}goto _LL7E;_LL81:;_LL82: kind=0;type=({struct Cyc_Core_Opt*
_tmpFA=_cycalloc(sizeof(*_tmpFA));_tmpFA->v=(void*)_tmpF5;_tmpFA;});goto _LL7E;
_LL7E:;}return Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Typedef_d_struct*
_tmpFB=_cycalloc(sizeof(*_tmpFB));_tmpFB[0]=({struct Cyc_Absyn_Typedef_d_struct
_tmpFC;_tmpFC.tag=9;_tmpFC.f1=({struct Cyc_Absyn_Typedefdecl*_tmpFD=_cycalloc(
sizeof(*_tmpFD));_tmpFD->name=_tmpF3;_tmpFD->tvs=_tmpF6;_tmpFD->kind=kind;_tmpFD->defn=
type;_tmpFD->atts=_tmpF7;_tmpFD->tq=_tmpF4;_tmpFD;});_tmpFC;});_tmpFB;}),loc);}}
static struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_decl(struct Cyc_Absyn_Decl*d,struct
Cyc_Absyn_Stmt*s){return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Decl_s_struct*
_tmpFE=_cycalloc(sizeof(*_tmpFE));_tmpFE[0]=({struct Cyc_Absyn_Decl_s_struct
_tmpFF;_tmpFF.tag=11;_tmpFF.f1=d;_tmpFF.f2=s;_tmpFF;});_tmpFE;}),d->loc);}static
struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_declarations(struct Cyc_List_List*ds,
struct Cyc_Absyn_Stmt*s){return((struct Cyc_Absyn_Stmt*(*)(struct Cyc_Absyn_Stmt*(*
f)(struct Cyc_Absyn_Decl*,struct Cyc_Absyn_Stmt*),struct Cyc_List_List*x,struct Cyc_Absyn_Stmt*
accum))Cyc_List_fold_right)(Cyc_Parse_flatten_decl,ds,s);}static struct Cyc_List_List*
Cyc_Parse_make_declarations(struct Cyc_Parse_Declaration_spec*ds,struct Cyc_List_List*
ids,struct Cyc_Position_Segment*tqual_loc,struct Cyc_Position_Segment*loc){struct
_RegionHandle _tmp100=_new_region("mkrgn");struct _RegionHandle*mkrgn=& _tmp100;
_push_region(mkrgn);{struct Cyc_Parse_Declaration_spec _tmp102;struct Cyc_Absyn_Tqual
_tmp103;struct Cyc_List_List*_tmp104;struct Cyc_List_List*_tmp105;struct Cyc_Parse_Declaration_spec*
_tmp101=ds;_tmp102=*_tmp101;_tmp103=_tmp102.tq;_tmp104=_tmp102.type_specs;
_tmp105=_tmp102.attributes;if(_tmp103.loc == 0)_tmp103.loc=tqual_loc;if(ds->is_inline)
Cyc_Parse_err(({const char*_tmp106="inline is allowed only on function definitions";
_tag_dyneither(_tmp106,sizeof(char),47);}),loc);if(_tmp104 == 0){Cyc_Parse_err(({
const char*_tmp107="missing type specifiers in declaration";_tag_dyneither(
_tmp107,sizeof(char),39);}),loc);{struct Cyc_List_List*_tmp108=0;_npop_handler(0);
return _tmp108;}}{void*s=(void*)2;int istypedef=0;if(ds->sc != 0){void*_tmp109=(
void*)((struct Cyc_Core_Opt*)_check_null(ds->sc))->v;_LL84: if((int)_tmp109 != 0)
goto _LL86;_LL85: istypedef=1;goto _LL83;_LL86: if((int)_tmp109 != 1)goto _LL88;_LL87:
s=(void*)3;goto _LL83;_LL88: if((int)_tmp109 != 2)goto _LL8A;_LL89: s=(void*)4;goto
_LL83;_LL8A: if((int)_tmp109 != 3)goto _LL8C;_LL8B: s=(void*)0;goto _LL83;_LL8C: if((
int)_tmp109 != 4)goto _LL8E;_LL8D: s=(void*)2;goto _LL83;_LL8E: if((int)_tmp109 != 5)
goto _LL90;_LL8F: if(Cyc_Parse_no_register)s=(void*)2;else{s=(void*)5;}goto _LL83;
_LL90: if((int)_tmp109 != 6)goto _LL83;_LL91: s=(void*)1;goto _LL83;_LL83:;}{struct
Cyc_List_List*_tmp10B;struct Cyc_List_List*_tmp10C;struct _tuple0 _tmp10A=((struct
_tuple0(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x))
Cyc_List_rsplit)(mkrgn,mkrgn,ids);_tmp10B=_tmp10A.f1;_tmp10C=_tmp10A.f2;{int
exps_empty=1;{struct Cyc_List_List*es=_tmp10C;for(0;es != 0;es=es->tl){if((struct
Cyc_Absyn_Exp*)es->hd != 0){exps_empty=0;break;}}}{struct _tuple5 _tmp10D=Cyc_Parse_collapse_type_specifiers(
_tmp104,loc);if(_tmp10B == 0){void*_tmp10F;struct Cyc_Core_Opt*_tmp110;struct
_tuple5 _tmp10E=_tmp10D;_tmp10F=_tmp10E.f1;_tmp110=_tmp10E.f2;if(_tmp110 != 0){
struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)_tmp110->v;{void*_tmp111=(void*)d->r;
struct Cyc_Absyn_Enumdecl*_tmp112;struct Cyc_Absyn_Aggrdecl*_tmp113;struct Cyc_Absyn_Tuniondecl*
_tmp114;_LL93: if(_tmp111 <= (void*)2)goto _LL99;if(*((int*)_tmp111)!= 8)goto _LL95;
_tmp112=((struct Cyc_Absyn_Enum_d_struct*)_tmp111)->f1;_LL94:(void*)(_tmp112->sc=(
void*)s);if(_tmp105 != 0)Cyc_Parse_err(({const char*_tmp115="bad attributes on enum";
_tag_dyneither(_tmp115,sizeof(char),23);}),loc);goto _LL92;_LL95: if(*((int*)
_tmp111)!= 6)goto _LL97;_tmp113=((struct Cyc_Absyn_Aggr_d_struct*)_tmp111)->f1;
_LL96:(void*)(_tmp113->sc=(void*)s);_tmp113->attributes=_tmp105;goto _LL92;_LL97:
if(*((int*)_tmp111)!= 7)goto _LL99;_tmp114=((struct Cyc_Absyn_Tunion_d_struct*)
_tmp111)->f1;_LL98:(void*)(_tmp114->sc=(void*)s);if(_tmp105 != 0)Cyc_Parse_err(({
const char*_tmp116="bad attributes on tunion";_tag_dyneither(_tmp116,sizeof(char),
25);}),loc);goto _LL92;_LL99:;_LL9A: Cyc_Parse_err(({const char*_tmp117="bad declaration";
_tag_dyneither(_tmp117,sizeof(char),16);}),loc);{struct Cyc_List_List*_tmp118=0;
_npop_handler(0);return _tmp118;}_LL92:;}{struct Cyc_List_List*_tmp11A=({struct Cyc_List_List*
_tmp119=_cycalloc(sizeof(*_tmp119));_tmp119->hd=d;_tmp119->tl=0;_tmp119;});
_npop_handler(0);return _tmp11A;}}else{void*_tmp11B=_tmp10F;struct Cyc_Absyn_AggrInfo
_tmp11C;union Cyc_Absyn_AggrInfoU_union _tmp11D;void*_tmp11E;struct _tuple1*_tmp11F;
struct Cyc_List_List*_tmp120;struct Cyc_Absyn_TunionInfo _tmp121;union Cyc_Absyn_TunionInfoU_union
_tmp122;struct Cyc_Absyn_Tuniondecl**_tmp123;struct Cyc_Absyn_TunionInfo _tmp124;
union Cyc_Absyn_TunionInfoU_union _tmp125;struct Cyc_Absyn_UnknownTunionInfo _tmp126;
struct _tuple1*_tmp127;int _tmp128;int _tmp129;struct Cyc_List_List*_tmp12A;struct
_tuple1*_tmp12B;struct Cyc_List_List*_tmp12C;_LL9C: if(_tmp11B <= (void*)4)goto
_LLA6;if(*((int*)_tmp11B)!= 10)goto _LL9E;_tmp11C=((struct Cyc_Absyn_AggrType_struct*)
_tmp11B)->f1;_tmp11D=_tmp11C.aggr_info;if((((((struct Cyc_Absyn_AggrType_struct*)
_tmp11B)->f1).aggr_info).UnknownAggr).tag != 0)goto _LL9E;_tmp11E=(_tmp11D.UnknownAggr).f1;
_tmp11F=(_tmp11D.UnknownAggr).f2;_tmp120=_tmp11C.targs;_LL9D: {struct Cyc_List_List*
_tmp12D=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
loc,_tmp120);struct Cyc_Absyn_Aggrdecl*_tmp12E=({struct Cyc_Absyn_Aggrdecl*_tmp134=
_cycalloc(sizeof(*_tmp134));_tmp134->kind=(void*)_tmp11E;_tmp134->sc=(void*)s;
_tmp134->name=_tmp11F;_tmp134->tvs=_tmp12D;_tmp134->impl=0;_tmp134->attributes=0;
_tmp134;});if(_tmp105 != 0)Cyc_Parse_err(({const char*_tmp12F="bad attributes on type declaration";
_tag_dyneither(_tmp12F,sizeof(char),35);}),loc);{struct Cyc_List_List*_tmp133=({
struct Cyc_List_List*_tmp130=_cycalloc(sizeof(*_tmp130));_tmp130->hd=Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Aggr_d_struct*_tmp131=_cycalloc(sizeof(*_tmp131));
_tmp131[0]=({struct Cyc_Absyn_Aggr_d_struct _tmp132;_tmp132.tag=6;_tmp132.f1=
_tmp12E;_tmp132;});_tmp131;}),loc);_tmp130->tl=0;_tmp130;});_npop_handler(0);
return _tmp133;}}_LL9E: if(*((int*)_tmp11B)!= 2)goto _LLA0;_tmp121=((struct Cyc_Absyn_TunionType_struct*)
_tmp11B)->f1;_tmp122=_tmp121.tunion_info;if((((((struct Cyc_Absyn_TunionType_struct*)
_tmp11B)->f1).tunion_info).KnownTunion).tag != 1)goto _LLA0;_tmp123=(_tmp122.KnownTunion).f1;
_LL9F: if(_tmp105 != 0)Cyc_Parse_err(({const char*_tmp135="bad attributes on tunion";
_tag_dyneither(_tmp135,sizeof(char),25);}),loc);{struct Cyc_List_List*_tmp139=({
struct Cyc_List_List*_tmp136=_cycalloc(sizeof(*_tmp136));_tmp136->hd=Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Tunion_d_struct*_tmp137=_cycalloc(sizeof(*_tmp137));
_tmp137[0]=({struct Cyc_Absyn_Tunion_d_struct _tmp138;_tmp138.tag=7;_tmp138.f1=*
_tmp123;_tmp138;});_tmp137;}),loc);_tmp136->tl=0;_tmp136;});_npop_handler(0);
return _tmp139;}_LLA0: if(*((int*)_tmp11B)!= 2)goto _LLA2;_tmp124=((struct Cyc_Absyn_TunionType_struct*)
_tmp11B)->f1;_tmp125=_tmp124.tunion_info;if((((((struct Cyc_Absyn_TunionType_struct*)
_tmp11B)->f1).tunion_info).UnknownTunion).tag != 0)goto _LLA2;_tmp126=(_tmp125.UnknownTunion).f1;
_tmp127=_tmp126.name;_tmp128=_tmp126.is_xtunion;_tmp129=_tmp126.is_flat;_tmp12A=
_tmp124.targs;_LLA1: {struct Cyc_List_List*_tmp13A=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,loc,_tmp12A);
struct Cyc_Absyn_Decl*_tmp13B=Cyc_Absyn_tunion_decl(s,_tmp127,_tmp13A,0,_tmp128,
_tmp129,loc);if(_tmp105 != 0)Cyc_Parse_err(({const char*_tmp13C="bad attributes on tunion";
_tag_dyneither(_tmp13C,sizeof(char),25);}),loc);{struct Cyc_List_List*_tmp13E=({
struct Cyc_List_List*_tmp13D=_cycalloc(sizeof(*_tmp13D));_tmp13D->hd=_tmp13B;
_tmp13D->tl=0;_tmp13D;});_npop_handler(0);return _tmp13E;}}_LLA2: if(*((int*)
_tmp11B)!= 12)goto _LLA4;_tmp12B=((struct Cyc_Absyn_EnumType_struct*)_tmp11B)->f1;
_LLA3: {struct Cyc_Absyn_Enumdecl*_tmp13F=({struct Cyc_Absyn_Enumdecl*_tmp146=
_cycalloc(sizeof(*_tmp146));_tmp146->sc=(void*)s;_tmp146->name=_tmp12B;_tmp146->fields=
0;_tmp146;});if(_tmp105 != 0)Cyc_Parse_err(({const char*_tmp140="bad attributes on enum";
_tag_dyneither(_tmp140,sizeof(char),23);}),loc);{struct Cyc_List_List*_tmp145=({
struct Cyc_List_List*_tmp141=_cycalloc(sizeof(*_tmp141));_tmp141->hd=({struct Cyc_Absyn_Decl*
_tmp142=_cycalloc(sizeof(*_tmp142));_tmp142->r=(void*)((void*)({struct Cyc_Absyn_Enum_d_struct*
_tmp143=_cycalloc(sizeof(*_tmp143));_tmp143[0]=({struct Cyc_Absyn_Enum_d_struct
_tmp144;_tmp144.tag=8;_tmp144.f1=_tmp13F;_tmp144;});_tmp143;}));_tmp142->loc=loc;
_tmp142;});_tmp141->tl=0;_tmp141;});_npop_handler(0);return _tmp145;}}_LLA4: if(*((
int*)_tmp11B)!= 13)goto _LLA6;_tmp12C=((struct Cyc_Absyn_AnonEnumType_struct*)
_tmp11B)->f1;_LLA5: {struct Cyc_Absyn_Enumdecl*_tmp147=({struct Cyc_Absyn_Enumdecl*
_tmp14E=_cycalloc(sizeof(*_tmp14E));_tmp14E->sc=(void*)s;_tmp14E->name=Cyc_Parse_gensym_enum();
_tmp14E->fields=({struct Cyc_Core_Opt*_tmp14F=_cycalloc(sizeof(*_tmp14F));_tmp14F->v=
_tmp12C;_tmp14F;});_tmp14E;});if(_tmp105 != 0)Cyc_Parse_err(({const char*_tmp148="bad attributes on enum";
_tag_dyneither(_tmp148,sizeof(char),23);}),loc);{struct Cyc_List_List*_tmp14D=({
struct Cyc_List_List*_tmp149=_cycalloc(sizeof(*_tmp149));_tmp149->hd=({struct Cyc_Absyn_Decl*
_tmp14A=_cycalloc(sizeof(*_tmp14A));_tmp14A->r=(void*)((void*)({struct Cyc_Absyn_Enum_d_struct*
_tmp14B=_cycalloc(sizeof(*_tmp14B));_tmp14B[0]=({struct Cyc_Absyn_Enum_d_struct
_tmp14C;_tmp14C.tag=8;_tmp14C.f1=_tmp147;_tmp14C;});_tmp14B;}));_tmp14A->loc=loc;
_tmp14A;});_tmp149->tl=0;_tmp149;});_npop_handler(0);return _tmp14D;}}_LLA6:;
_LLA7: Cyc_Parse_err(({const char*_tmp150="missing declarator";_tag_dyneither(
_tmp150,sizeof(char),19);}),loc);{struct Cyc_List_List*_tmp151=0;_npop_handler(0);
return _tmp151;}_LL9B:;}}else{void*t=_tmp10D.f1;struct Cyc_List_List*_tmp152=Cyc_Parse_apply_tmss(
mkrgn,_tmp103,t,_tmp10B,_tmp105);if(istypedef){if(!exps_empty)Cyc_Parse_err(({
const char*_tmp153="initializer in typedef declaration";_tag_dyneither(_tmp153,
sizeof(char),35);}),loc);{struct Cyc_List_List*decls=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Decl*(*f)(struct Cyc_Position_Segment*,struct _tuple7*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_v_typ_to_typedef,loc,
_tmp152);if(_tmp10D.f2 != 0){struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)(
_tmp10D.f2)->v;{void*_tmp154=(void*)d->r;struct Cyc_Absyn_Aggrdecl*_tmp155;struct
Cyc_Absyn_Tuniondecl*_tmp156;struct Cyc_Absyn_Enumdecl*_tmp157;_LLA9: if(_tmp154 <= (
void*)2)goto _LLAF;if(*((int*)_tmp154)!= 6)goto _LLAB;_tmp155=((struct Cyc_Absyn_Aggr_d_struct*)
_tmp154)->f1;_LLAA:(void*)(_tmp155->sc=(void*)s);_tmp155->attributes=_tmp105;
_tmp105=0;goto _LLA8;_LLAB: if(*((int*)_tmp154)!= 7)goto _LLAD;_tmp156=((struct Cyc_Absyn_Tunion_d_struct*)
_tmp154)->f1;_LLAC:(void*)(_tmp156->sc=(void*)s);goto _LLA8;_LLAD: if(*((int*)
_tmp154)!= 8)goto _LLAF;_tmp157=((struct Cyc_Absyn_Enum_d_struct*)_tmp154)->f1;
_LLAE:(void*)(_tmp157->sc=(void*)s);goto _LLA8;_LLAF:;_LLB0: Cyc_Parse_err(({const
char*_tmp158="declaration within typedef is not a struct, union, tunion, or xtunion";
_tag_dyneither(_tmp158,sizeof(char),70);}),loc);goto _LLA8;_LLA8:;}decls=({struct
Cyc_List_List*_tmp159=_cycalloc(sizeof(*_tmp159));_tmp159->hd=d;_tmp159->tl=
decls;_tmp159;});}{struct Cyc_List_List*_tmp15A=decls;_npop_handler(0);return
_tmp15A;}}}else{if(_tmp10D.f2 != 0)Cyc_Parse_unimp(({const char*_tmp15B="nested type declaration within declarator";
_tag_dyneither(_tmp15B,sizeof(char),42);}),loc);{struct Cyc_List_List*decls=0;{
struct Cyc_List_List*_tmp15C=_tmp152;for(0;_tmp15C != 0;(_tmp15C=_tmp15C->tl,
_tmp10C=_tmp10C->tl)){struct _tuple7 _tmp15E;struct _tuple1*_tmp15F;struct Cyc_Absyn_Tqual
_tmp160;void*_tmp161;struct Cyc_List_List*_tmp162;struct Cyc_List_List*_tmp163;
struct _tuple7*_tmp15D=(struct _tuple7*)_tmp15C->hd;_tmp15E=*_tmp15D;_tmp15F=
_tmp15E.f1;_tmp160=_tmp15E.f2;_tmp161=_tmp15E.f3;_tmp162=_tmp15E.f4;_tmp163=
_tmp15E.f5;if(_tmp162 != 0)({void*_tmp164=0;Cyc_Tcutil_warn(loc,({const char*
_tmp165="bad type params, ignoring";_tag_dyneither(_tmp165,sizeof(char),26);}),
_tag_dyneither(_tmp164,sizeof(void*),0));});if(_tmp10C == 0)({void*_tmp166=0;((
int(*)(struct Cyc_Position_Segment*sg,struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Parse_abort)(loc,({const char*_tmp167="unexpected NULL in parse!";
_tag_dyneither(_tmp167,sizeof(char),26);}),_tag_dyneither(_tmp166,sizeof(void*),
0));});{struct Cyc_Absyn_Exp*_tmp168=(struct Cyc_Absyn_Exp*)_tmp10C->hd;struct Cyc_Absyn_Vardecl*
_tmp169=Cyc_Absyn_new_vardecl(_tmp15F,_tmp161,_tmp168);_tmp169->tq=_tmp160;(void*)(
_tmp169->sc=(void*)s);_tmp169->attributes=_tmp163;{struct Cyc_Absyn_Decl*_tmp16A=({
struct Cyc_Absyn_Decl*_tmp16C=_cycalloc(sizeof(*_tmp16C));_tmp16C->r=(void*)((
void*)({struct Cyc_Absyn_Var_d_struct*_tmp16D=_cycalloc(sizeof(*_tmp16D));_tmp16D[
0]=({struct Cyc_Absyn_Var_d_struct _tmp16E;_tmp16E.tag=0;_tmp16E.f1=_tmp169;
_tmp16E;});_tmp16D;}));_tmp16C->loc=loc;_tmp16C;});decls=({struct Cyc_List_List*
_tmp16B=_cycalloc(sizeof(*_tmp16B));_tmp16B->hd=_tmp16A;_tmp16B->tl=decls;
_tmp16B;});}}}}{struct Cyc_List_List*_tmp16F=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(decls);_npop_handler(0);return _tmp16F;}}}}}}}}};_pop_region(
mkrgn);}static void*Cyc_Parse_id_to_kind(struct _dyneither_ptr s,struct Cyc_Position_Segment*
loc){if(Cyc_strlen((struct _dyneither_ptr)s)== 1  || Cyc_strlen((struct
_dyneither_ptr)s)== 2)switch(*((const char*)_check_dyneither_subscript(s,sizeof(
char),0))){case 'A': _LLB1: return(void*)0;case 'M': _LLB2: return(void*)1;case 'B':
_LLB3: return(void*)2;case 'R': _LLB4: return(void*)3;case 'U': _LLB5: if(*((const char*)
_check_dyneither_subscript(s,sizeof(char),1))== 'R')return(void*)4;else{break;}
case 'T': _LLB6: if(*((const char*)_check_dyneither_subscript(s,sizeof(char),1))== 'R')
return(void*)5;else{break;}case 'E': _LLB7: return(void*)6;case 'I': _LLB8: return(
void*)7;default: _LLB9: break;}Cyc_Parse_err((struct _dyneither_ptr)({struct Cyc_Int_pa_struct
_tmp173;_tmp173.tag=1;_tmp173.f1=(unsigned long)((int)Cyc_strlen((struct
_dyneither_ptr)s));{struct Cyc_String_pa_struct _tmp172;_tmp172.tag=0;_tmp172.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)s);{void*_tmp170[2]={& _tmp172,&
_tmp173};Cyc_aprintf(({const char*_tmp171="bad kind: %s; strlen=%d";
_tag_dyneither(_tmp171,sizeof(char),24);}),_tag_dyneither(_tmp170,sizeof(void*),
2));}}}),loc);return(void*)2;}static struct Cyc_Absyn_Exp*Cyc_Parse_pat2exp(struct
Cyc_Absyn_Pat*p){void*_tmp174=(void*)p->r;struct _tuple1*_tmp175;struct Cyc_Absyn_Vardecl*
_tmp176;struct Cyc_Absyn_Pat*_tmp177;struct Cyc_Absyn_Pat _tmp178;void*_tmp179;
struct Cyc_Absyn_Pat*_tmp17A;void*_tmp17B;int _tmp17C;char _tmp17D;struct
_dyneither_ptr _tmp17E;struct _tuple1*_tmp17F;struct Cyc_List_List*_tmp180;int
_tmp181;struct Cyc_Absyn_Exp*_tmp182;_LLBC: if(_tmp174 <= (void*)2)goto _LLC2;if(*((
int*)_tmp174)!= 12)goto _LLBE;_tmp175=((struct Cyc_Absyn_UnknownId_p_struct*)
_tmp174)->f1;_LLBD: return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_UnknownId_e_struct*
_tmp183=_cycalloc(sizeof(*_tmp183));_tmp183[0]=({struct Cyc_Absyn_UnknownId_e_struct
_tmp184;_tmp184.tag=2;_tmp184.f1=_tmp175;_tmp184;});_tmp183;}),p->loc);_LLBE: if(*((
int*)_tmp174)!= 1)goto _LLC0;_tmp176=((struct Cyc_Absyn_Reference_p_struct*)
_tmp174)->f1;_tmp177=((struct Cyc_Absyn_Reference_p_struct*)_tmp174)->f2;_tmp178=*
_tmp177;_tmp179=(void*)_tmp178.r;if((int)_tmp179 != 0)goto _LLC0;_LLBF: return Cyc_Absyn_deref_exp(
Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_UnknownId_e_struct*_tmp185=_cycalloc(
sizeof(*_tmp185));_tmp185[0]=({struct Cyc_Absyn_UnknownId_e_struct _tmp186;_tmp186.tag=
2;_tmp186.f1=_tmp176->name;_tmp186;});_tmp185;}),p->loc),p->loc);_LLC0: if(*((int*)
_tmp174)!= 4)goto _LLC2;_tmp17A=((struct Cyc_Absyn_Pointer_p_struct*)_tmp174)->f1;
_LLC1: return Cyc_Absyn_address_exp(Cyc_Parse_pat2exp(_tmp17A),p->loc);_LLC2: if((
int)_tmp174 != 1)goto _LLC4;_LLC3: return Cyc_Absyn_null_exp(p->loc);_LLC4: if(
_tmp174 <= (void*)2)goto _LLCE;if(*((int*)_tmp174)!= 7)goto _LLC6;_tmp17B=(void*)((
struct Cyc_Absyn_Int_p_struct*)_tmp174)->f1;_tmp17C=((struct Cyc_Absyn_Int_p_struct*)
_tmp174)->f2;_LLC5: return Cyc_Absyn_int_exp(_tmp17B,_tmp17C,p->loc);_LLC6: if(*((
int*)_tmp174)!= 8)goto _LLC8;_tmp17D=((struct Cyc_Absyn_Char_p_struct*)_tmp174)->f1;
_LLC7: return Cyc_Absyn_char_exp(_tmp17D,p->loc);_LLC8: if(*((int*)_tmp174)!= 9)
goto _LLCA;_tmp17E=((struct Cyc_Absyn_Float_p_struct*)_tmp174)->f1;_LLC9: return Cyc_Absyn_float_exp(
_tmp17E,p->loc);_LLCA: if(*((int*)_tmp174)!= 13)goto _LLCC;_tmp17F=((struct Cyc_Absyn_UnknownCall_p_struct*)
_tmp174)->f1;_tmp180=((struct Cyc_Absyn_UnknownCall_p_struct*)_tmp174)->f2;
_tmp181=((struct Cyc_Absyn_UnknownCall_p_struct*)_tmp174)->f3;if(_tmp181 != 0)goto
_LLCC;_LLCB: {struct Cyc_Absyn_Exp*e1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_UnknownId_e_struct*
_tmp187=_cycalloc(sizeof(*_tmp187));_tmp187[0]=({struct Cyc_Absyn_UnknownId_e_struct
_tmp188;_tmp188.tag=2;_tmp188.f1=_tmp17F;_tmp188;});_tmp187;}),p->loc);struct Cyc_List_List*
es=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Pat*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Parse_pat2exp,_tmp180);return Cyc_Absyn_unknowncall_exp(
e1,es,p->loc);}_LLCC: if(*((int*)_tmp174)!= 14)goto _LLCE;_tmp182=((struct Cyc_Absyn_Exp_p_struct*)
_tmp174)->f1;_LLCD: return _tmp182;_LLCE:;_LLCF: Cyc_Parse_err(({const char*_tmp189="cannot mix patterns and expressions in case";
_tag_dyneither(_tmp189,sizeof(char),44);}),p->loc);return Cyc_Absyn_null_exp(p->loc);
_LLBB:;}struct _tuple11{void*f1;int f2;};struct Cyc_Int_tok_struct{int tag;struct
_tuple11 f1;};struct Cyc_Char_tok_struct{int tag;char f1;};struct Cyc_String_tok_struct{
int tag;struct _dyneither_ptr f1;};struct Cyc_Stringopt_tok_struct{int tag;struct Cyc_Core_Opt*
f1;};struct Cyc_QualId_tok_struct{int tag;struct _tuple1*f1;};struct _tuple12{struct
Cyc_Position_Segment*f1;struct Cyc_Absyn_Conref*f2;struct Cyc_Absyn_Conref*f3;};
struct Cyc_YY1_struct{int tag;struct _tuple12*f1;};struct Cyc_YY2_struct{int tag;
struct Cyc_Absyn_Conref*f1;};struct Cyc_YY3_struct{int tag;struct Cyc_Absyn_Exp*f1;}
;struct Cyc_YY4_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_YY5_struct{int
tag;struct Cyc_List_List*f1;};struct Cyc_YY6_struct{int tag;struct Cyc_List_List*f1;
};struct Cyc_YY7_struct{int tag;void*f1;};struct Cyc_YY8_struct{int tag;struct Cyc_Core_Opt*
f1;};struct Cyc_YY9_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_YY10_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_YY11_struct{int tag;struct Cyc_Absyn_Pat*
f1;};struct _tuple13{struct Cyc_List_List*f1;int f2;};struct Cyc_YY12_struct{int tag;
struct _tuple13*f1;};struct Cyc_YY13_struct{int tag;struct Cyc_List_List*f1;};struct
_tuple14{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_YY14_struct{
int tag;struct _tuple14*f1;};struct Cyc_YY15_struct{int tag;struct Cyc_List_List*f1;}
;struct Cyc_YY16_struct{int tag;struct _tuple13*f1;};struct Cyc_YY17_struct{int tag;
struct Cyc_Absyn_Fndecl*f1;};struct Cyc_YY18_struct{int tag;struct Cyc_List_List*f1;
};struct Cyc_YY19_struct{int tag;struct Cyc_Parse_Declaration_spec*f1;};struct
_tuple15{struct Cyc_Parse_Declarator*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_YY20_struct{
int tag;struct _tuple15*f1;};struct Cyc_YY21_struct{int tag;struct Cyc_List_List*f1;}
;struct Cyc_YY22_struct{int tag;void*f1;};struct Cyc_YY23_struct{int tag;void*f1;};
struct Cyc_YY24_struct{int tag;void*f1;};struct Cyc_YY25_struct{int tag;struct Cyc_Absyn_Tqual
f1;};struct Cyc_YY26_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_YY27_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_YY28_struct{int tag;struct Cyc_List_List*
f1;};struct Cyc_YY29_struct{int tag;struct Cyc_Parse_Declarator*f1;};struct Cyc_YY30_struct{
int tag;struct Cyc_Parse_Abstractdeclarator*f1;};struct Cyc_YY31_struct{int tag;int
f1;};struct Cyc_YY32_struct{int tag;void*f1;};struct Cyc_YY33_struct{int tag;struct
Cyc_Absyn_Tunionfield*f1;};struct Cyc_YY34_struct{int tag;struct Cyc_List_List*f1;}
;struct _tuple16{struct Cyc_Absyn_Tqual f1;struct Cyc_List_List*f2;struct Cyc_List_List*
f3;};struct Cyc_YY35_struct{int tag;struct _tuple16*f1;};struct Cyc_YY36_struct{int
tag;struct Cyc_List_List*f1;};struct Cyc_YY37_struct{int tag;struct _tuple2*f1;};
struct Cyc_YY38_struct{int tag;struct Cyc_List_List*f1;};struct _tuple17{struct Cyc_List_List*
f1;int f2;struct Cyc_Absyn_VarargInfo*f3;struct Cyc_Core_Opt*f4;struct Cyc_List_List*
f5;};struct Cyc_YY39_struct{int tag;struct _tuple17*f1;};struct Cyc_YY40_struct{int
tag;struct Cyc_List_List*f1;};struct Cyc_YY41_struct{int tag;struct Cyc_List_List*f1;
};struct Cyc_YY42_struct{int tag;void*f1;};struct Cyc_YY43_struct{int tag;void*f1;};
struct Cyc_YY44_struct{int tag;void*f1;};struct Cyc_YY45_struct{int tag;struct Cyc_List_List*
f1;};struct Cyc_YY46_struct{int tag;void*f1;};struct Cyc_YY47_struct{int tag;struct
Cyc_Absyn_Enumfield*f1;};struct Cyc_YY48_struct{int tag;struct Cyc_List_List*f1;};
struct Cyc_YY49_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_YY50_struct{int
tag;struct Cyc_List_List*f1;};struct Cyc_YY51_struct{int tag;struct Cyc_Absyn_Conref*
f1;};struct Cyc_YY52_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_YYINITIALSVAL_struct{
int tag;};union Cyc_YYSTYPE_union{struct Cyc_Int_tok_struct Int_tok;struct Cyc_Char_tok_struct
Char_tok;struct Cyc_String_tok_struct String_tok;struct Cyc_Stringopt_tok_struct
Stringopt_tok;struct Cyc_QualId_tok_struct QualId_tok;struct Cyc_YY1_struct YY1;
struct Cyc_YY2_struct YY2;struct Cyc_YY3_struct YY3;struct Cyc_YY4_struct YY4;struct
Cyc_YY5_struct YY5;struct Cyc_YY6_struct YY6;struct Cyc_YY7_struct YY7;struct Cyc_YY8_struct
YY8;struct Cyc_YY9_struct YY9;struct Cyc_YY10_struct YY10;struct Cyc_YY11_struct YY11;
struct Cyc_YY12_struct YY12;struct Cyc_YY13_struct YY13;struct Cyc_YY14_struct YY14;
struct Cyc_YY15_struct YY15;struct Cyc_YY16_struct YY16;struct Cyc_YY17_struct YY17;
struct Cyc_YY18_struct YY18;struct Cyc_YY19_struct YY19;struct Cyc_YY20_struct YY20;
struct Cyc_YY21_struct YY21;struct Cyc_YY22_struct YY22;struct Cyc_YY23_struct YY23;
struct Cyc_YY24_struct YY24;struct Cyc_YY25_struct YY25;struct Cyc_YY26_struct YY26;
struct Cyc_YY27_struct YY27;struct Cyc_YY28_struct YY28;struct Cyc_YY29_struct YY29;
struct Cyc_YY30_struct YY30;struct Cyc_YY31_struct YY31;struct Cyc_YY32_struct YY32;
struct Cyc_YY33_struct YY33;struct Cyc_YY34_struct YY34;struct Cyc_YY35_struct YY35;
struct Cyc_YY36_struct YY36;struct Cyc_YY37_struct YY37;struct Cyc_YY38_struct YY38;
struct Cyc_YY39_struct YY39;struct Cyc_YY40_struct YY40;struct Cyc_YY41_struct YY41;
struct Cyc_YY42_struct YY42;struct Cyc_YY43_struct YY43;struct Cyc_YY44_struct YY44;
struct Cyc_YY45_struct YY45;struct Cyc_YY46_struct YY46;struct Cyc_YY47_struct YY47;
struct Cyc_YY48_struct YY48;struct Cyc_YY49_struct YY49;struct Cyc_YY50_struct YY50;
struct Cyc_YY51_struct YY51;struct Cyc_YY52_struct YY52;struct Cyc_YYINITIALSVAL_struct
YYINITIALSVAL;};static char _tmp18B[14]="$(sign_t,int)";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Int_tok={Cyc_Core_Failure,{_tmp18B,_tmp18B,_tmp18B + 14}};struct
_tuple11 Cyc_yyget_Int_tok(union Cyc_YYSTYPE_union yy1){struct _tuple11 yyzzz;{union
Cyc_YYSTYPE_union _tmp18C=yy1;struct _tuple11 _tmp18D;_LLD1: if((_tmp18C.Int_tok).tag
!= 0)goto _LLD3;_tmp18D=(_tmp18C.Int_tok).f1;_LLD2: yyzzz=_tmp18D;goto _LLD0;_LLD3:;
_LLD4:(int)_throw((void*)& Cyc_yyfail_Int_tok);_LLD0:;}return yyzzz;}static char
_tmp18F[5]="char";static struct Cyc_Core_Failure_struct Cyc_yyfail_Char_tok={Cyc_Core_Failure,{
_tmp18F,_tmp18F,_tmp18F + 5}};char Cyc_yyget_Char_tok(union Cyc_YYSTYPE_union yy1){
char yyzzz;{union Cyc_YYSTYPE_union _tmp190=yy1;char _tmp191;_LLD6: if((_tmp190.Char_tok).tag
!= 1)goto _LLD8;_tmp191=(_tmp190.Char_tok).f1;_LLD7: yyzzz=_tmp191;goto _LLD5;_LLD8:;
_LLD9:(int)_throw((void*)& Cyc_yyfail_Char_tok);_LLD5:;}return yyzzz;}static char
_tmp193[9]="string_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_String_tok={
Cyc_Core_Failure,{_tmp193,_tmp193,_tmp193 + 9}};struct _dyneither_ptr Cyc_yyget_String_tok(
union Cyc_YYSTYPE_union yy1){struct _dyneither_ptr yyzzz;{union Cyc_YYSTYPE_union
_tmp194=yy1;struct _dyneither_ptr _tmp195;_LLDB: if((_tmp194.String_tok).tag != 2)
goto _LLDD;_tmp195=(_tmp194.String_tok).f1;_LLDC: yyzzz=_tmp195;goto _LLDA;_LLDD:;
_LLDE:(int)_throw((void*)& Cyc_yyfail_String_tok);_LLDA:;}return yyzzz;}static char
_tmp197[54]="$(Position::seg_t,conref_t<bool>,conref_t<bounds_t>)@";static struct
Cyc_Core_Failure_struct Cyc_yyfail_YY1={Cyc_Core_Failure,{_tmp197,_tmp197,_tmp197
+ 54}};struct _tuple12*Cyc_yyget_YY1(union Cyc_YYSTYPE_union yy1){struct _tuple12*
yyzzz;{union Cyc_YYSTYPE_union _tmp198=yy1;struct _tuple12*_tmp199;_LLE0: if((
_tmp198.YY1).tag != 5)goto _LLE2;_tmp199=(_tmp198.YY1).f1;_LLE1: yyzzz=_tmp199;goto
_LLDF;_LLE2:;_LLE3:(int)_throw((void*)& Cyc_yyfail_YY1);_LLDF:;}return yyzzz;}
static char _tmp19B[19]="conref_t<bounds_t>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY2={Cyc_Core_Failure,{_tmp19B,_tmp19B,_tmp19B + 19}};struct Cyc_Absyn_Conref*
Cyc_yyget_YY2(union Cyc_YYSTYPE_union yy1){struct Cyc_Absyn_Conref*yyzzz;{union Cyc_YYSTYPE_union
_tmp19C=yy1;struct Cyc_Absyn_Conref*_tmp19D;_LLE5: if((_tmp19C.YY2).tag != 6)goto
_LLE7;_tmp19D=(_tmp19C.YY2).f1;_LLE6: yyzzz=_tmp19D;goto _LLE4;_LLE7:;_LLE8:(int)
_throw((void*)& Cyc_yyfail_YY2);_LLE4:;}return yyzzz;}static char _tmp19F[6]="exp_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY3={Cyc_Core_Failure,{_tmp19F,
_tmp19F,_tmp19F + 6}};struct Cyc_Absyn_Exp*Cyc_yyget_YY3(union Cyc_YYSTYPE_union yy1){
struct Cyc_Absyn_Exp*yyzzz;{union Cyc_YYSTYPE_union _tmp1A0=yy1;struct Cyc_Absyn_Exp*
_tmp1A1;_LLEA: if((_tmp1A0.YY3).tag != 7)goto _LLEC;_tmp1A1=(_tmp1A0.YY3).f1;_LLEB:
yyzzz=_tmp1A1;goto _LLE9;_LLEC:;_LLED:(int)_throw((void*)& Cyc_yyfail_YY3);_LLE9:;}
return yyzzz;}static char _tmp1A3[10]="exp_opt_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY4={Cyc_Core_Failure,{_tmp1A3,_tmp1A3,_tmp1A3 + 10}};struct Cyc_Absyn_Exp*
Cyc_yyget_YY4(union Cyc_YYSTYPE_union yy1){struct Cyc_Absyn_Exp*yyzzz;{union Cyc_YYSTYPE_union
_tmp1A4=yy1;struct Cyc_Absyn_Exp*_tmp1A5;_LLEF: if((_tmp1A4.YY4).tag != 8)goto _LLF1;
_tmp1A5=(_tmp1A4.YY4).f1;_LLF0: yyzzz=_tmp1A5;goto _LLEE;_LLF1:;_LLF2:(int)_throw((
void*)& Cyc_yyfail_YY4);_LLEE:;}return yyzzz;}static char _tmp1A7[14]="list_t<exp_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY5={Cyc_Core_Failure,{_tmp1A7,
_tmp1A7,_tmp1A7 + 14}};struct Cyc_List_List*Cyc_yyget_YY5(union Cyc_YYSTYPE_union
yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE_union _tmp1A8=yy1;struct Cyc_List_List*
_tmp1A9;_LLF4: if((_tmp1A8.YY5).tag != 9)goto _LLF6;_tmp1A9=(_tmp1A8.YY5).f1;_LLF5:
yyzzz=_tmp1A9;goto _LLF3;_LLF6:;_LLF7:(int)_throw((void*)& Cyc_yyfail_YY5);_LLF3:;}
return yyzzz;}static char _tmp1AB[39]="list_t<$(list_t<designator_t>,exp_t)@>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY6={Cyc_Core_Failure,{_tmp1AB,
_tmp1AB,_tmp1AB + 39}};struct Cyc_List_List*Cyc_yyget_YY6(union Cyc_YYSTYPE_union
yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE_union _tmp1AC=yy1;struct Cyc_List_List*
_tmp1AD;_LLF9: if((_tmp1AC.YY6).tag != 10)goto _LLFB;_tmp1AD=(_tmp1AC.YY6).f1;_LLFA:
yyzzz=_tmp1AD;goto _LLF8;_LLFB:;_LLFC:(int)_throw((void*)& Cyc_yyfail_YY6);_LLF8:;}
return yyzzz;}static char _tmp1AF[9]="primop_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY7={Cyc_Core_Failure,{_tmp1AF,_tmp1AF,_tmp1AF + 9}};void*Cyc_yyget_YY7(
union Cyc_YYSTYPE_union yy1){void*yyzzz;{union Cyc_YYSTYPE_union _tmp1B0=yy1;void*
_tmp1B1;_LLFE: if((_tmp1B0.YY7).tag != 11)goto _LL100;_tmp1B1=(_tmp1B0.YY7).f1;
_LLFF: yyzzz=_tmp1B1;goto _LLFD;_LL100:;_LL101:(int)_throw((void*)& Cyc_yyfail_YY7);
_LLFD:;}return yyzzz;}static char _tmp1B3[16]="opt_t<primop_t>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY8={Cyc_Core_Failure,{_tmp1B3,_tmp1B3,_tmp1B3 + 16}};struct Cyc_Core_Opt*
Cyc_yyget_YY8(union Cyc_YYSTYPE_union yy1){struct Cyc_Core_Opt*yyzzz;{union Cyc_YYSTYPE_union
_tmp1B4=yy1;struct Cyc_Core_Opt*_tmp1B5;_LL103: if((_tmp1B4.YY8).tag != 12)goto
_LL105;_tmp1B5=(_tmp1B4.YY8).f1;_LL104: yyzzz=_tmp1B5;goto _LL102;_LL105:;_LL106:(
int)_throw((void*)& Cyc_yyfail_YY8);_LL102:;}return yyzzz;}static char _tmp1B7[7]="qvar_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_QualId_tok={Cyc_Core_Failure,{
_tmp1B7,_tmp1B7,_tmp1B7 + 7}};struct _tuple1*Cyc_yyget_QualId_tok(union Cyc_YYSTYPE_union
yy1){struct _tuple1*yyzzz;{union Cyc_YYSTYPE_union _tmp1B8=yy1;struct _tuple1*
_tmp1B9;_LL108: if((_tmp1B8.QualId_tok).tag != 4)goto _LL10A;_tmp1B9=(_tmp1B8.QualId_tok).f1;
_LL109: yyzzz=_tmp1B9;goto _LL107;_LL10A:;_LL10B:(int)_throw((void*)& Cyc_yyfail_QualId_tok);
_LL107:;}return yyzzz;}static char _tmp1BB[7]="stmt_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY9={Cyc_Core_Failure,{_tmp1BB,_tmp1BB,_tmp1BB + 7}};struct Cyc_Absyn_Stmt*
Cyc_yyget_YY9(union Cyc_YYSTYPE_union yy1){struct Cyc_Absyn_Stmt*yyzzz;{union Cyc_YYSTYPE_union
_tmp1BC=yy1;struct Cyc_Absyn_Stmt*_tmp1BD;_LL10D: if((_tmp1BC.YY9).tag != 13)goto
_LL10F;_tmp1BD=(_tmp1BC.YY9).f1;_LL10E: yyzzz=_tmp1BD;goto _LL10C;_LL10F:;_LL110:(
int)_throw((void*)& Cyc_yyfail_YY9);_LL10C:;}return yyzzz;}static char _tmp1BF[24]="list_t<switch_clause_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY10={Cyc_Core_Failure,{_tmp1BF,
_tmp1BF,_tmp1BF + 24}};struct Cyc_List_List*Cyc_yyget_YY10(union Cyc_YYSTYPE_union
yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE_union _tmp1C0=yy1;struct Cyc_List_List*
_tmp1C1;_LL112: if((_tmp1C0.YY10).tag != 14)goto _LL114;_tmp1C1=(_tmp1C0.YY10).f1;
_LL113: yyzzz=_tmp1C1;goto _LL111;_LL114:;_LL115:(int)_throw((void*)& Cyc_yyfail_YY10);
_LL111:;}return yyzzz;}static char _tmp1C3[6]="pat_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY11={Cyc_Core_Failure,{_tmp1C3,_tmp1C3,_tmp1C3 + 6}};struct Cyc_Absyn_Pat*
Cyc_yyget_YY11(union Cyc_YYSTYPE_union yy1){struct Cyc_Absyn_Pat*yyzzz;{union Cyc_YYSTYPE_union
_tmp1C4=yy1;struct Cyc_Absyn_Pat*_tmp1C5;_LL117: if((_tmp1C4.YY11).tag != 15)goto
_LL119;_tmp1C5=(_tmp1C4.YY11).f1;_LL118: yyzzz=_tmp1C5;goto _LL116;_LL119:;_LL11A:(
int)_throw((void*)& Cyc_yyfail_YY11);_LL116:;}return yyzzz;}static char _tmp1C7[23]="$(list_t<pat_t>,bool)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY12={Cyc_Core_Failure,{_tmp1C7,
_tmp1C7,_tmp1C7 + 23}};struct _tuple13*Cyc_yyget_YY12(union Cyc_YYSTYPE_union yy1){
struct _tuple13*yyzzz;{union Cyc_YYSTYPE_union _tmp1C8=yy1;struct _tuple13*_tmp1C9;
_LL11C: if((_tmp1C8.YY12).tag != 16)goto _LL11E;_tmp1C9=(_tmp1C8.YY12).f1;_LL11D:
yyzzz=_tmp1C9;goto _LL11B;_LL11E:;_LL11F:(int)_throw((void*)& Cyc_yyfail_YY12);
_LL11B:;}return yyzzz;}static char _tmp1CB[14]="list_t<pat_t>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY13={Cyc_Core_Failure,{_tmp1CB,_tmp1CB,_tmp1CB + 14}};struct Cyc_List_List*
Cyc_yyget_YY13(union Cyc_YYSTYPE_union yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE_union
_tmp1CC=yy1;struct Cyc_List_List*_tmp1CD;_LL121: if((_tmp1CC.YY13).tag != 17)goto
_LL123;_tmp1CD=(_tmp1CC.YY13).f1;_LL122: yyzzz=_tmp1CD;goto _LL120;_LL123:;_LL124:(
int)_throw((void*)& Cyc_yyfail_YY13);_LL120:;}return yyzzz;}static char _tmp1CF[31]="$(list_t<designator_t>,pat_t)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY14={Cyc_Core_Failure,{_tmp1CF,
_tmp1CF,_tmp1CF + 31}};struct _tuple14*Cyc_yyget_YY14(union Cyc_YYSTYPE_union yy1){
struct _tuple14*yyzzz;{union Cyc_YYSTYPE_union _tmp1D0=yy1;struct _tuple14*_tmp1D1;
_LL126: if((_tmp1D0.YY14).tag != 18)goto _LL128;_tmp1D1=(_tmp1D0.YY14).f1;_LL127:
yyzzz=_tmp1D1;goto _LL125;_LL128:;_LL129:(int)_throw((void*)& Cyc_yyfail_YY14);
_LL125:;}return yyzzz;}static char _tmp1D3[39]="list_t<$(list_t<designator_t>,pat_t)@>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY15={Cyc_Core_Failure,{_tmp1D3,
_tmp1D3,_tmp1D3 + 39}};struct Cyc_List_List*Cyc_yyget_YY15(union Cyc_YYSTYPE_union
yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE_union _tmp1D4=yy1;struct Cyc_List_List*
_tmp1D5;_LL12B: if((_tmp1D4.YY15).tag != 19)goto _LL12D;_tmp1D5=(_tmp1D4.YY15).f1;
_LL12C: yyzzz=_tmp1D5;goto _LL12A;_LL12D:;_LL12E:(int)_throw((void*)& Cyc_yyfail_YY15);
_LL12A:;}return yyzzz;}static char _tmp1D7[48]="$(list_t<$(list_t<designator_t>,pat_t)@>,bool)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY16={Cyc_Core_Failure,{_tmp1D7,
_tmp1D7,_tmp1D7 + 48}};struct _tuple13*Cyc_yyget_YY16(union Cyc_YYSTYPE_union yy1){
struct _tuple13*yyzzz;{union Cyc_YYSTYPE_union _tmp1D8=yy1;struct _tuple13*_tmp1D9;
_LL130: if((_tmp1D8.YY16).tag != 20)goto _LL132;_tmp1D9=(_tmp1D8.YY16).f1;_LL131:
yyzzz=_tmp1D9;goto _LL12F;_LL132:;_LL133:(int)_throw((void*)& Cyc_yyfail_YY16);
_LL12F:;}return yyzzz;}static char _tmp1DB[9]="fndecl_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY17={Cyc_Core_Failure,{_tmp1DB,_tmp1DB,_tmp1DB + 9}};struct Cyc_Absyn_Fndecl*
Cyc_yyget_YY17(union Cyc_YYSTYPE_union yy1){struct Cyc_Absyn_Fndecl*yyzzz;{union Cyc_YYSTYPE_union
_tmp1DC=yy1;struct Cyc_Absyn_Fndecl*_tmp1DD;_LL135: if((_tmp1DC.YY17).tag != 21)
goto _LL137;_tmp1DD=(_tmp1DC.YY17).f1;_LL136: yyzzz=_tmp1DD;goto _LL134;_LL137:;
_LL138:(int)_throw((void*)& Cyc_yyfail_YY17);_LL134:;}return yyzzz;}static char
_tmp1DF[15]="list_t<decl_t>";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY18={
Cyc_Core_Failure,{_tmp1DF,_tmp1DF,_tmp1DF + 15}};struct Cyc_List_List*Cyc_yyget_YY18(
union Cyc_YYSTYPE_union yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE_union
_tmp1E0=yy1;struct Cyc_List_List*_tmp1E1;_LL13A: if((_tmp1E0.YY18).tag != 22)goto
_LL13C;_tmp1E1=(_tmp1E0.YY18).f1;_LL13B: yyzzz=_tmp1E1;goto _LL139;_LL13C:;_LL13D:(
int)_throw((void*)& Cyc_yyfail_YY18);_LL139:;}return yyzzz;}static char _tmp1E3[12]="decl_spec_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY19={Cyc_Core_Failure,{_tmp1E3,
_tmp1E3,_tmp1E3 + 12}};struct Cyc_Parse_Declaration_spec*Cyc_yyget_YY19(union Cyc_YYSTYPE_union
yy1){struct Cyc_Parse_Declaration_spec*yyzzz;{union Cyc_YYSTYPE_union _tmp1E4=yy1;
struct Cyc_Parse_Declaration_spec*_tmp1E5;_LL13F: if((_tmp1E4.YY19).tag != 23)goto
_LL141;_tmp1E5=(_tmp1E4.YY19).f1;_LL140: yyzzz=_tmp1E5;goto _LL13E;_LL141:;_LL142:(
int)_throw((void*)& Cyc_yyfail_YY19);_LL13E:;}return yyzzz;}static char _tmp1E7[27]="$(declarator_t,exp_opt_t)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY20={Cyc_Core_Failure,{_tmp1E7,
_tmp1E7,_tmp1E7 + 27}};struct _tuple15*Cyc_yyget_YY20(union Cyc_YYSTYPE_union yy1){
struct _tuple15*yyzzz;{union Cyc_YYSTYPE_union _tmp1E8=yy1;struct _tuple15*_tmp1E9;
_LL144: if((_tmp1E8.YY20).tag != 24)goto _LL146;_tmp1E9=(_tmp1E8.YY20).f1;_LL145:
yyzzz=_tmp1E9;goto _LL143;_LL146:;_LL147:(int)_throw((void*)& Cyc_yyfail_YY20);
_LL143:;}return yyzzz;}static char _tmp1EB[35]="list_t<$(declarator_t,exp_opt_t)@>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY21={Cyc_Core_Failure,{_tmp1EB,
_tmp1EB,_tmp1EB + 35}};struct Cyc_List_List*Cyc_yyget_YY21(union Cyc_YYSTYPE_union
yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE_union _tmp1EC=yy1;struct Cyc_List_List*
_tmp1ED;_LL149: if((_tmp1EC.YY21).tag != 25)goto _LL14B;_tmp1ED=(_tmp1EC.YY21).f1;
_LL14A: yyzzz=_tmp1ED;goto _LL148;_LL14B:;_LL14C:(int)_throw((void*)& Cyc_yyfail_YY21);
_LL148:;}return yyzzz;}static char _tmp1EF[16]="storage_class_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY22={Cyc_Core_Failure,{_tmp1EF,_tmp1EF,_tmp1EF + 16}};void*Cyc_yyget_YY22(
union Cyc_YYSTYPE_union yy1){void*yyzzz;{union Cyc_YYSTYPE_union _tmp1F0=yy1;void*
_tmp1F1;_LL14E: if((_tmp1F0.YY22).tag != 26)goto _LL150;_tmp1F1=(_tmp1F0.YY22).f1;
_LL14F: yyzzz=_tmp1F1;goto _LL14D;_LL150:;_LL151:(int)_throw((void*)& Cyc_yyfail_YY22);
_LL14D:;}return yyzzz;}static char _tmp1F3[17]="type_specifier_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY23={Cyc_Core_Failure,{_tmp1F3,_tmp1F3,_tmp1F3 + 17}};void*Cyc_yyget_YY23(
union Cyc_YYSTYPE_union yy1){void*yyzzz;{union Cyc_YYSTYPE_union _tmp1F4=yy1;void*
_tmp1F5;_LL153: if((_tmp1F4.YY23).tag != 27)goto _LL155;_tmp1F5=(_tmp1F4.YY23).f1;
_LL154: yyzzz=_tmp1F5;goto _LL152;_LL155:;_LL156:(int)_throw((void*)& Cyc_yyfail_YY23);
_LL152:;}return yyzzz;}static char _tmp1F7[12]="aggr_kind_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY24={Cyc_Core_Failure,{_tmp1F7,_tmp1F7,_tmp1F7 + 12}};void*Cyc_yyget_YY24(
union Cyc_YYSTYPE_union yy1){void*yyzzz;{union Cyc_YYSTYPE_union _tmp1F8=yy1;void*
_tmp1F9;_LL158: if((_tmp1F8.YY24).tag != 28)goto _LL15A;_tmp1F9=(_tmp1F8.YY24).f1;
_LL159: yyzzz=_tmp1F9;goto _LL157;_LL15A:;_LL15B:(int)_throw((void*)& Cyc_yyfail_YY24);
_LL157:;}return yyzzz;}static char _tmp1FB[8]="tqual_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY25={Cyc_Core_Failure,{_tmp1FB,_tmp1FB,_tmp1FB + 8}};struct Cyc_Absyn_Tqual
Cyc_yyget_YY25(union Cyc_YYSTYPE_union yy1){struct Cyc_Absyn_Tqual yyzzz;{union Cyc_YYSTYPE_union
_tmp1FC=yy1;struct Cyc_Absyn_Tqual _tmp1FD;_LL15D: if((_tmp1FC.YY25).tag != 29)goto
_LL15F;_tmp1FD=(_tmp1FC.YY25).f1;_LL15E: yyzzz=_tmp1FD;goto _LL15C;_LL15F:;_LL160:(
int)_throw((void*)& Cyc_yyfail_YY25);_LL15C:;}return yyzzz;}static char _tmp1FF[20]="list_t<aggrfield_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY26={Cyc_Core_Failure,{_tmp1FF,
_tmp1FF,_tmp1FF + 20}};struct Cyc_List_List*Cyc_yyget_YY26(union Cyc_YYSTYPE_union
yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE_union _tmp200=yy1;struct Cyc_List_List*
_tmp201;_LL162: if((_tmp200.YY26).tag != 30)goto _LL164;_tmp201=(_tmp200.YY26).f1;
_LL163: yyzzz=_tmp201;goto _LL161;_LL164:;_LL165:(int)_throw((void*)& Cyc_yyfail_YY26);
_LL161:;}return yyzzz;}static char _tmp203[28]="list_t<list_t<aggrfield_t>>";static
struct Cyc_Core_Failure_struct Cyc_yyfail_YY27={Cyc_Core_Failure,{_tmp203,_tmp203,
_tmp203 + 28}};struct Cyc_List_List*Cyc_yyget_YY27(union Cyc_YYSTYPE_union yy1){
struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE_union _tmp204=yy1;struct Cyc_List_List*
_tmp205;_LL167: if((_tmp204.YY27).tag != 31)goto _LL169;_tmp205=(_tmp204.YY27).f1;
_LL168: yyzzz=_tmp205;goto _LL166;_LL169:;_LL16A:(int)_throw((void*)& Cyc_yyfail_YY27);
_LL166:;}return yyzzz;}static char _tmp207[24]="list_t<type_modifier_t>";static
struct Cyc_Core_Failure_struct Cyc_yyfail_YY28={Cyc_Core_Failure,{_tmp207,_tmp207,
_tmp207 + 24}};struct Cyc_List_List*Cyc_yyget_YY28(union Cyc_YYSTYPE_union yy1){
struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE_union _tmp208=yy1;struct Cyc_List_List*
_tmp209;_LL16C: if((_tmp208.YY28).tag != 32)goto _LL16E;_tmp209=(_tmp208.YY28).f1;
_LL16D: yyzzz=_tmp209;goto _LL16B;_LL16E:;_LL16F:(int)_throw((void*)& Cyc_yyfail_YY28);
_LL16B:;}return yyzzz;}static char _tmp20B[13]="declarator_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY29={Cyc_Core_Failure,{_tmp20B,_tmp20B,_tmp20B + 13}};struct Cyc_Parse_Declarator*
Cyc_yyget_YY29(union Cyc_YYSTYPE_union yy1){struct Cyc_Parse_Declarator*yyzzz;{
union Cyc_YYSTYPE_union _tmp20C=yy1;struct Cyc_Parse_Declarator*_tmp20D;_LL171: if((
_tmp20C.YY29).tag != 33)goto _LL173;_tmp20D=(_tmp20C.YY29).f1;_LL172: yyzzz=_tmp20D;
goto _LL170;_LL173:;_LL174:(int)_throw((void*)& Cyc_yyfail_YY29);_LL170:;}return
yyzzz;}static char _tmp20F[21]="abstractdeclarator_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY30={Cyc_Core_Failure,{_tmp20F,_tmp20F,_tmp20F + 21}};struct Cyc_Parse_Abstractdeclarator*
Cyc_yyget_YY30(union Cyc_YYSTYPE_union yy1){struct Cyc_Parse_Abstractdeclarator*
yyzzz;{union Cyc_YYSTYPE_union _tmp210=yy1;struct Cyc_Parse_Abstractdeclarator*
_tmp211;_LL176: if((_tmp210.YY30).tag != 34)goto _LL178;_tmp211=(_tmp210.YY30).f1;
_LL177: yyzzz=_tmp211;goto _LL175;_LL178:;_LL179:(int)_throw((void*)& Cyc_yyfail_YY30);
_LL175:;}return yyzzz;}static char _tmp213[5]="bool";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY31={Cyc_Core_Failure,{_tmp213,_tmp213,_tmp213 + 5}};int Cyc_yyget_YY31(
union Cyc_YYSTYPE_union yy1){int yyzzz;{union Cyc_YYSTYPE_union _tmp214=yy1;int
_tmp215;_LL17B: if((_tmp214.YY31).tag != 35)goto _LL17D;_tmp215=(_tmp214.YY31).f1;
_LL17C: yyzzz=_tmp215;goto _LL17A;_LL17D:;_LL17E:(int)_throw((void*)& Cyc_yyfail_YY31);
_LL17A:;}return yyzzz;}static char _tmp217[8]="scope_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY32={Cyc_Core_Failure,{_tmp217,_tmp217,_tmp217 + 8}};void*Cyc_yyget_YY32(
union Cyc_YYSTYPE_union yy1){void*yyzzz;{union Cyc_YYSTYPE_union _tmp218=yy1;void*
_tmp219;_LL180: if((_tmp218.YY32).tag != 36)goto _LL182;_tmp219=(_tmp218.YY32).f1;
_LL181: yyzzz=_tmp219;goto _LL17F;_LL182:;_LL183:(int)_throw((void*)& Cyc_yyfail_YY32);
_LL17F:;}return yyzzz;}static char _tmp21B[14]="tunionfield_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY33={Cyc_Core_Failure,{_tmp21B,_tmp21B,_tmp21B + 14}};struct Cyc_Absyn_Tunionfield*
Cyc_yyget_YY33(union Cyc_YYSTYPE_union yy1){struct Cyc_Absyn_Tunionfield*yyzzz;{
union Cyc_YYSTYPE_union _tmp21C=yy1;struct Cyc_Absyn_Tunionfield*_tmp21D;_LL185: if((
_tmp21C.YY33).tag != 37)goto _LL187;_tmp21D=(_tmp21C.YY33).f1;_LL186: yyzzz=_tmp21D;
goto _LL184;_LL187:;_LL188:(int)_throw((void*)& Cyc_yyfail_YY33);_LL184:;}return
yyzzz;}static char _tmp21F[22]="list_t<tunionfield_t>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY34={Cyc_Core_Failure,{_tmp21F,_tmp21F,_tmp21F + 22}};struct Cyc_List_List*
Cyc_yyget_YY34(union Cyc_YYSTYPE_union yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE_union
_tmp220=yy1;struct Cyc_List_List*_tmp221;_LL18A: if((_tmp220.YY34).tag != 38)goto
_LL18C;_tmp221=(_tmp220.YY34).f1;_LL18B: yyzzz=_tmp221;goto _LL189;_LL18C:;_LL18D:(
int)_throw((void*)& Cyc_yyfail_YY34);_LL189:;}return yyzzz;}static char _tmp223[50]="$(tqual_t,list_t<type_specifier_t>,attributes_t)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY35={Cyc_Core_Failure,{_tmp223,
_tmp223,_tmp223 + 50}};struct _tuple16*Cyc_yyget_YY35(union Cyc_YYSTYPE_union yy1){
struct _tuple16*yyzzz;{union Cyc_YYSTYPE_union _tmp224=yy1;struct _tuple16*_tmp225;
_LL18F: if((_tmp224.YY35).tag != 39)goto _LL191;_tmp225=(_tmp224.YY35).f1;_LL190:
yyzzz=_tmp225;goto _LL18E;_LL191:;_LL192:(int)_throw((void*)& Cyc_yyfail_YY35);
_LL18E:;}return yyzzz;}static char _tmp227[14]="list_t<var_t>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY36={Cyc_Core_Failure,{_tmp227,_tmp227,_tmp227 + 14}};struct Cyc_List_List*
Cyc_yyget_YY36(union Cyc_YYSTYPE_union yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE_union
_tmp228=yy1;struct Cyc_List_List*_tmp229;_LL194: if((_tmp228.YY36).tag != 40)goto
_LL196;_tmp229=(_tmp228.YY36).f1;_LL195: yyzzz=_tmp229;goto _LL193;_LL196:;_LL197:(
int)_throw((void*)& Cyc_yyfail_YY36);_LL193:;}return yyzzz;}static char _tmp22B[32]="$(opt_t<var_t>,tqual_t,type_t)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY37={Cyc_Core_Failure,{_tmp22B,
_tmp22B,_tmp22B + 32}};struct _tuple2*Cyc_yyget_YY37(union Cyc_YYSTYPE_union yy1){
struct _tuple2*yyzzz;{union Cyc_YYSTYPE_union _tmp22C=yy1;struct _tuple2*_tmp22D;
_LL199: if((_tmp22C.YY37).tag != 41)goto _LL19B;_tmp22D=(_tmp22C.YY37).f1;_LL19A:
yyzzz=_tmp22D;goto _LL198;_LL19B:;_LL19C:(int)_throw((void*)& Cyc_yyfail_YY37);
_LL198:;}return yyzzz;}static char _tmp22F[40]="list_t<$(opt_t<var_t>,tqual_t,type_t)@>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY38={Cyc_Core_Failure,{_tmp22F,
_tmp22F,_tmp22F + 40}};struct Cyc_List_List*Cyc_yyget_YY38(union Cyc_YYSTYPE_union
yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE_union _tmp230=yy1;struct Cyc_List_List*
_tmp231;_LL19E: if((_tmp230.YY38).tag != 42)goto _LL1A0;_tmp231=(_tmp230.YY38).f1;
_LL19F: yyzzz=_tmp231;goto _LL19D;_LL1A0:;_LL1A1:(int)_throw((void*)& Cyc_yyfail_YY38);
_LL19D:;}return yyzzz;}static char _tmp233[107]="$(list_t<$(opt_t<var_t>,tqual_t,type_t)@>, bool,vararg_info_t *,opt_t<type_t>, list_t<$(type_t,type_t)@>)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY39={Cyc_Core_Failure,{_tmp233,
_tmp233,_tmp233 + 107}};struct _tuple17*Cyc_yyget_YY39(union Cyc_YYSTYPE_union yy1){
struct _tuple17*yyzzz;{union Cyc_YYSTYPE_union _tmp234=yy1;struct _tuple17*_tmp235;
_LL1A3: if((_tmp234.YY39).tag != 43)goto _LL1A5;_tmp235=(_tmp234.YY39).f1;_LL1A4:
yyzzz=_tmp235;goto _LL1A2;_LL1A5:;_LL1A6:(int)_throw((void*)& Cyc_yyfail_YY39);
_LL1A2:;}return yyzzz;}static char _tmp237[15]="list_t<type_t>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY40={Cyc_Core_Failure,{_tmp237,_tmp237,_tmp237 + 15}};struct Cyc_List_List*
Cyc_yyget_YY40(union Cyc_YYSTYPE_union yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE_union
_tmp238=yy1;struct Cyc_List_List*_tmp239;_LL1A8: if((_tmp238.YY40).tag != 44)goto
_LL1AA;_tmp239=(_tmp238.YY40).f1;_LL1A9: yyzzz=_tmp239;goto _LL1A7;_LL1AA:;_LL1AB:(
int)_throw((void*)& Cyc_yyfail_YY40);_LL1A7:;}return yyzzz;}static char _tmp23B[21]="list_t<designator_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY41={Cyc_Core_Failure,{_tmp23B,
_tmp23B,_tmp23B + 21}};struct Cyc_List_List*Cyc_yyget_YY41(union Cyc_YYSTYPE_union
yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE_union _tmp23C=yy1;struct Cyc_List_List*
_tmp23D;_LL1AD: if((_tmp23C.YY41).tag != 45)goto _LL1AF;_tmp23D=(_tmp23C.YY41).f1;
_LL1AE: yyzzz=_tmp23D;goto _LL1AC;_LL1AF:;_LL1B0:(int)_throw((void*)& Cyc_yyfail_YY41);
_LL1AC:;}return yyzzz;}static char _tmp23F[13]="designator_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY42={Cyc_Core_Failure,{_tmp23F,_tmp23F,_tmp23F + 13}};void*Cyc_yyget_YY42(
union Cyc_YYSTYPE_union yy1){void*yyzzz;{union Cyc_YYSTYPE_union _tmp240=yy1;void*
_tmp241;_LL1B2: if((_tmp240.YY42).tag != 46)goto _LL1B4;_tmp241=(_tmp240.YY42).f1;
_LL1B3: yyzzz=_tmp241;goto _LL1B1;_LL1B4:;_LL1B5:(int)_throw((void*)& Cyc_yyfail_YY42);
_LL1B1:;}return yyzzz;}static char _tmp243[7]="kind_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY43={Cyc_Core_Failure,{_tmp243,_tmp243,_tmp243 + 7}};void*Cyc_yyget_YY43(
union Cyc_YYSTYPE_union yy1){void*yyzzz;{union Cyc_YYSTYPE_union _tmp244=yy1;void*
_tmp245;_LL1B7: if((_tmp244.YY43).tag != 47)goto _LL1B9;_tmp245=(_tmp244.YY43).f1;
_LL1B8: yyzzz=_tmp245;goto _LL1B6;_LL1B9:;_LL1BA:(int)_throw((void*)& Cyc_yyfail_YY43);
_LL1B6:;}return yyzzz;}static char _tmp247[7]="type_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY44={Cyc_Core_Failure,{_tmp247,_tmp247,_tmp247 + 7}};void*Cyc_yyget_YY44(
union Cyc_YYSTYPE_union yy1){void*yyzzz;{union Cyc_YYSTYPE_union _tmp248=yy1;void*
_tmp249;_LL1BC: if((_tmp248.YY44).tag != 48)goto _LL1BE;_tmp249=(_tmp248.YY44).f1;
_LL1BD: yyzzz=_tmp249;goto _LL1BB;_LL1BE:;_LL1BF:(int)_throw((void*)& Cyc_yyfail_YY44);
_LL1BB:;}return yyzzz;}static char _tmp24B[20]="list_t<attribute_t>";static struct
Cyc_Core_Failure_struct Cyc_yyfail_YY45={Cyc_Core_Failure,{_tmp24B,_tmp24B,
_tmp24B + 20}};struct Cyc_List_List*Cyc_yyget_YY45(union Cyc_YYSTYPE_union yy1){
struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE_union _tmp24C=yy1;struct Cyc_List_List*
_tmp24D;_LL1C1: if((_tmp24C.YY45).tag != 49)goto _LL1C3;_tmp24D=(_tmp24C.YY45).f1;
_LL1C2: yyzzz=_tmp24D;goto _LL1C0;_LL1C3:;_LL1C4:(int)_throw((void*)& Cyc_yyfail_YY45);
_LL1C0:;}return yyzzz;}static char _tmp24F[12]="attribute_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY46={Cyc_Core_Failure,{_tmp24F,_tmp24F,_tmp24F + 12}};void*Cyc_yyget_YY46(
union Cyc_YYSTYPE_union yy1){void*yyzzz;{union Cyc_YYSTYPE_union _tmp250=yy1;void*
_tmp251;_LL1C6: if((_tmp250.YY46).tag != 50)goto _LL1C8;_tmp251=(_tmp250.YY46).f1;
_LL1C7: yyzzz=_tmp251;goto _LL1C5;_LL1C8:;_LL1C9:(int)_throw((void*)& Cyc_yyfail_YY46);
_LL1C5:;}return yyzzz;}static char _tmp253[12]="enumfield_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY47={Cyc_Core_Failure,{_tmp253,_tmp253,_tmp253 + 12}};struct Cyc_Absyn_Enumfield*
Cyc_yyget_YY47(union Cyc_YYSTYPE_union yy1){struct Cyc_Absyn_Enumfield*yyzzz;{union
Cyc_YYSTYPE_union _tmp254=yy1;struct Cyc_Absyn_Enumfield*_tmp255;_LL1CB: if((
_tmp254.YY47).tag != 51)goto _LL1CD;_tmp255=(_tmp254.YY47).f1;_LL1CC: yyzzz=_tmp255;
goto _LL1CA;_LL1CD:;_LL1CE:(int)_throw((void*)& Cyc_yyfail_YY47);_LL1CA:;}return
yyzzz;}static char _tmp257[20]="list_t<enumfield_t>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY48={Cyc_Core_Failure,{_tmp257,_tmp257,_tmp257 + 20}};struct Cyc_List_List*
Cyc_yyget_YY48(union Cyc_YYSTYPE_union yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE_union
_tmp258=yy1;struct Cyc_List_List*_tmp259;_LL1D0: if((_tmp258.YY48).tag != 52)goto
_LL1D2;_tmp259=(_tmp258.YY48).f1;_LL1D1: yyzzz=_tmp259;goto _LL1CF;_LL1D2:;_LL1D3:(
int)_throw((void*)& Cyc_yyfail_YY48);_LL1CF:;}return yyzzz;}static char _tmp25B[14]="opt_t<type_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY49={Cyc_Core_Failure,{_tmp25B,
_tmp25B,_tmp25B + 14}};struct Cyc_Core_Opt*Cyc_yyget_YY49(union Cyc_YYSTYPE_union
yy1){struct Cyc_Core_Opt*yyzzz;{union Cyc_YYSTYPE_union _tmp25C=yy1;struct Cyc_Core_Opt*
_tmp25D;_LL1D5: if((_tmp25C.YY49).tag != 53)goto _LL1D7;_tmp25D=(_tmp25C.YY49).f1;
_LL1D6: yyzzz=_tmp25D;goto _LL1D4;_LL1D7:;_LL1D8:(int)_throw((void*)& Cyc_yyfail_YY49);
_LL1D4:;}return yyzzz;}static char _tmp25F[26]="list_t<$(type_t,type_t)@>";static
struct Cyc_Core_Failure_struct Cyc_yyfail_YY50={Cyc_Core_Failure,{_tmp25F,_tmp25F,
_tmp25F + 26}};struct Cyc_List_List*Cyc_yyget_YY50(union Cyc_YYSTYPE_union yy1){
struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE_union _tmp260=yy1;struct Cyc_List_List*
_tmp261;_LL1DA: if((_tmp260.YY50).tag != 54)goto _LL1DC;_tmp261=(_tmp260.YY50).f1;
_LL1DB: yyzzz=_tmp261;goto _LL1D9;_LL1DC:;_LL1DD:(int)_throw((void*)& Cyc_yyfail_YY50);
_LL1D9:;}return yyzzz;}static char _tmp263[15]="conref_t<bool>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY51={Cyc_Core_Failure,{_tmp263,_tmp263,_tmp263 + 15}};struct Cyc_Absyn_Conref*
Cyc_yyget_YY51(union Cyc_YYSTYPE_union yy1){struct Cyc_Absyn_Conref*yyzzz;{union Cyc_YYSTYPE_union
_tmp264=yy1;struct Cyc_Absyn_Conref*_tmp265;_LL1DF: if((_tmp264.YY51).tag != 55)
goto _LL1E1;_tmp265=(_tmp264.YY51).f1;_LL1E0: yyzzz=_tmp265;goto _LL1DE;_LL1E1:;
_LL1E2:(int)_throw((void*)& Cyc_yyfail_YY51);_LL1DE:;}return yyzzz;}static char
_tmp267[40]="list_t<$(Position::seg_t,qvar_t,bool)@>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY52={Cyc_Core_Failure,{_tmp267,_tmp267,_tmp267 + 40}};struct Cyc_List_List*
Cyc_yyget_YY52(union Cyc_YYSTYPE_union yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE_union
_tmp268=yy1;struct Cyc_List_List*_tmp269;_LL1E4: if((_tmp268.YY52).tag != 56)goto
_LL1E6;_tmp269=(_tmp268.YY52).f1;_LL1E5: yyzzz=_tmp269;goto _LL1E3;_LL1E6:;_LL1E7:(
int)_throw((void*)& Cyc_yyfail_YY52);_LL1E3:;}return yyzzz;}struct Cyc_Yyltype{int
timestamp;int first_line;int first_column;int last_line;int last_column;};struct Cyc_Yyltype
Cyc_yynewloc(){return({struct Cyc_Yyltype _tmp26A;_tmp26A.timestamp=0;_tmp26A.first_line=
0;_tmp26A.first_column=0;_tmp26A.last_line=0;_tmp26A.last_column=0;_tmp26A;});}
struct Cyc_Yyltype Cyc_yylloc={0,0,0,0,0};static short Cyc_yytranslate[362]={0,2,2,2,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,133,2,2,119,131,128,2,
116,117,122,125,110,129,121,130,2,2,2,2,2,2,2,2,2,2,120,107,112,111,113,124,123,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,114,2,115,127,118,2,2,2,2,2,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,108,126,109,132,2,2,2,2,2,2,2,2,2,2,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1,
2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,
31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,
58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,
85,86,87,88,89,90,91,92,93,94,95,96,97,98,99,100,101,102,103,104,105,106};static
char _tmp26B[2]="$";static char _tmp26C[6]="error";static char _tmp26D[12]="$undefined.";
static char _tmp26E[5]="AUTO";static char _tmp26F[9]="REGISTER";static char _tmp270[7]="STATIC";
static char _tmp271[7]="EXTERN";static char _tmp272[8]="TYPEDEF";static char _tmp273[5]="VOID";
static char _tmp274[5]="CHAR";static char _tmp275[6]="SHORT";static char _tmp276[4]="INT";
static char _tmp277[5]="LONG";static char _tmp278[6]="FLOAT";static char _tmp279[7]="DOUBLE";
static char _tmp27A[7]="SIGNED";static char _tmp27B[9]="UNSIGNED";static char _tmp27C[
6]="CONST";static char _tmp27D[9]="VOLATILE";static char _tmp27E[9]="RESTRICT";
static char _tmp27F[7]="STRUCT";static char _tmp280[6]="UNION";static char _tmp281[5]="CASE";
static char _tmp282[8]="DEFAULT";static char _tmp283[7]="INLINE";static char _tmp284[7]="SIZEOF";
static char _tmp285[9]="OFFSETOF";static char _tmp286[3]="IF";static char _tmp287[5]="ELSE";
static char _tmp288[7]="SWITCH";static char _tmp289[6]="WHILE";static char _tmp28A[3]="DO";
static char _tmp28B[4]="FOR";static char _tmp28C[5]="GOTO";static char _tmp28D[9]="CONTINUE";
static char _tmp28E[6]="BREAK";static char _tmp28F[7]="RETURN";static char _tmp290[5]="ENUM";
static char _tmp291[8]="NULL_kw";static char _tmp292[4]="LET";static char _tmp293[6]="THROW";
static char _tmp294[4]="TRY";static char _tmp295[6]="CATCH";static char _tmp296[7]="EXPORT";
static char _tmp297[4]="NEW";static char _tmp298[9]="ABSTRACT";static char _tmp299[9]="FALLTHRU";
static char _tmp29A[6]="USING";static char _tmp29B[10]="NAMESPACE";static char _tmp29C[
7]="TUNION";static char _tmp29D[8]="XTUNION";static char _tmp29E[7]="MALLOC";static
char _tmp29F[8]="RMALLOC";static char _tmp2A0[7]="CALLOC";static char _tmp2A1[8]="RCALLOC";
static char _tmp2A2[5]="SWAP";static char _tmp2A3[9]="REGION_T";static char _tmp2A4[6]="TAG_T";
static char _tmp2A5[7]="REGION";static char _tmp2A6[5]="RNEW";static char _tmp2A7[8]="REGIONS";
static char _tmp2A8[13]="RESET_REGION";static char _tmp2A9[4]="GEN";static char
_tmp2AA[14]="NOZEROTERM_kw";static char _tmp2AB[12]="ZEROTERM_kw";static char
_tmp2AC[7]="PORTON";static char _tmp2AD[8]="PORTOFF";static char _tmp2AE[8]="FLAT_kw";
static char _tmp2AF[12]="DYNREGION_T";static char _tmp2B0[6]="ALIAS";static char
_tmp2B1[8]="NUMELTS";static char _tmp2B2[8]="VALUEOF";static char _tmp2B3[10]="VALUEOF_T";
static char _tmp2B4[7]="PTR_OP";static char _tmp2B5[7]="INC_OP";static char _tmp2B6[7]="DEC_OP";
static char _tmp2B7[8]="LEFT_OP";static char _tmp2B8[9]="RIGHT_OP";static char _tmp2B9[
6]="LE_OP";static char _tmp2BA[6]="GE_OP";static char _tmp2BB[6]="EQ_OP";static char
_tmp2BC[6]="NE_OP";static char _tmp2BD[7]="AND_OP";static char _tmp2BE[6]="OR_OP";
static char _tmp2BF[11]="MUL_ASSIGN";static char _tmp2C0[11]="DIV_ASSIGN";static char
_tmp2C1[11]="MOD_ASSIGN";static char _tmp2C2[11]="ADD_ASSIGN";static char _tmp2C3[11]="SUB_ASSIGN";
static char _tmp2C4[12]="LEFT_ASSIGN";static char _tmp2C5[13]="RIGHT_ASSIGN";static
char _tmp2C6[11]="AND_ASSIGN";static char _tmp2C7[11]="XOR_ASSIGN";static char
_tmp2C8[10]="OR_ASSIGN";static char _tmp2C9[9]="ELLIPSIS";static char _tmp2CA[11]="LEFT_RIGHT";
static char _tmp2CB[12]="COLON_COLON";static char _tmp2CC[11]="IDENTIFIER";static
char _tmp2CD[17]="INTEGER_CONSTANT";static char _tmp2CE[7]="STRING";static char
_tmp2CF[19]="CHARACTER_CONSTANT";static char _tmp2D0[18]="FLOATING_CONSTANT";
static char _tmp2D1[9]="TYPE_VAR";static char _tmp2D2[13]="TYPEDEF_NAME";static char
_tmp2D3[16]="QUAL_IDENTIFIER";static char _tmp2D4[18]="QUAL_TYPEDEF_NAME";static
char _tmp2D5[10]="ATTRIBUTE";static char _tmp2D6[4]="';'";static char _tmp2D7[4]="'{'";
static char _tmp2D8[4]="'}'";static char _tmp2D9[4]="','";static char _tmp2DA[4]="'='";
static char _tmp2DB[4]="'<'";static char _tmp2DC[4]="'>'";static char _tmp2DD[4]="'['";
static char _tmp2DE[4]="']'";static char _tmp2DF[4]="'('";static char _tmp2E0[4]="')'";
static char _tmp2E1[4]="'_'";static char _tmp2E2[4]="'$'";static char _tmp2E3[4]="':'";
static char _tmp2E4[4]="'.'";static char _tmp2E5[4]="'*'";static char _tmp2E6[4]="'@'";
static char _tmp2E7[4]="'?'";static char _tmp2E8[4]="'+'";static char _tmp2E9[4]="'|'";
static char _tmp2EA[4]="'^'";static char _tmp2EB[4]="'&'";static char _tmp2EC[4]="'-'";
static char _tmp2ED[4]="'/'";static char _tmp2EE[4]="'%'";static char _tmp2EF[4]="'~'";
static char _tmp2F0[4]="'!'";static char _tmp2F1[5]="prog";static char _tmp2F2[17]="translation_unit";
static char _tmp2F3[12]="export_list";static char _tmp2F4[19]="export_list_values";
static char _tmp2F5[21]="external_declaration";static char _tmp2F6[20]="function_definition";
static char _tmp2F7[21]="function_definition2";static char _tmp2F8[13]="using_action";
static char _tmp2F9[15]="unusing_action";static char _tmp2FA[17]="namespace_action";
static char _tmp2FB[19]="unnamespace_action";static char _tmp2FC[12]="declaration";
static char _tmp2FD[17]="declaration_list";static char _tmp2FE[23]="declaration_specifiers";
static char _tmp2FF[24]="storage_class_specifier";static char _tmp300[15]="attributes_opt";
static char _tmp301[11]="attributes";static char _tmp302[15]="attribute_list";static
char _tmp303[10]="attribute";static char _tmp304[15]="type_specifier";static char
_tmp305[25]="type_specifier_notypedef";static char _tmp306[5]="kind";static char
_tmp307[15]="type_qualifier";static char _tmp308[15]="enum_specifier";static char
_tmp309[11]="enum_field";static char _tmp30A[22]="enum_declaration_list";static
char _tmp30B[26]="struct_or_union_specifier";static char _tmp30C[16]="type_params_opt";
static char _tmp30D[16]="struct_or_union";static char _tmp30E[24]="struct_declaration_list";
static char _tmp30F[25]="struct_declaration_list0";static char _tmp310[21]="init_declarator_list";
static char _tmp311[22]="init_declarator_list0";static char _tmp312[16]="init_declarator";
static char _tmp313[19]="struct_declaration";static char _tmp314[25]="specifier_qualifier_list";
static char _tmp315[35]="notypedef_specifier_qualifier_list";static char _tmp316[23]="struct_declarator_list";
static char _tmp317[24]="struct_declarator_list0";static char _tmp318[18]="struct_declarator";
static char _tmp319[17]="tunion_specifier";static char _tmp31A[9]="flat_opt";static
char _tmp31B[18]="tunion_or_xtunion";static char _tmp31C[17]="tunionfield_list";
static char _tmp31D[18]="tunionfield_scope";static char _tmp31E[12]="tunionfield";
static char _tmp31F[11]="declarator";static char _tmp320[23]="declarator_withtypedef";
static char _tmp321[18]="direct_declarator";static char _tmp322[30]="direct_declarator_withtypedef";
static char _tmp323[8]="pointer";static char _tmp324[12]="one_pointer";static char
_tmp325[23]="pointer_null_and_bound";static char _tmp326[14]="pointer_bound";
static char _tmp327[18]="zeroterm_qual_opt";static char _tmp328[12]="opt_rgn_opt";
static char _tmp329[8]="rgn_opt";static char _tmp32A[11]="tqual_list";static char
_tmp32B[20]="parameter_type_list";static char _tmp32C[9]="type_var";static char
_tmp32D[16]="optional_effect";static char _tmp32E[19]="optional_rgn_order";static
char _tmp32F[10]="rgn_order";static char _tmp330[16]="optional_inject";static char
_tmp331[11]="effect_set";static char _tmp332[14]="atomic_effect";static char _tmp333[
11]="region_set";static char _tmp334[15]="parameter_list";static char _tmp335[22]="parameter_declaration";
static char _tmp336[16]="identifier_list";static char _tmp337[17]="identifier_list0";
static char _tmp338[12]="initializer";static char _tmp339[18]="array_initializer";
static char _tmp33A[17]="initializer_list";static char _tmp33B[12]="designation";
static char _tmp33C[16]="designator_list";static char _tmp33D[11]="designator";
static char _tmp33E[10]="type_name";static char _tmp33F[14]="any_type_name";static
char _tmp340[15]="type_name_list";static char _tmp341[20]="abstract_declarator";
static char _tmp342[27]="direct_abstract_declarator";static char _tmp343[10]="statement";
static char _tmp344[13]="open_exp_opt";static char _tmp345[18]="labeled_statement";
static char _tmp346[21]="expression_statement";static char _tmp347[19]="compound_statement";
static char _tmp348[16]="block_item_list";static char _tmp349[20]="selection_statement";
static char _tmp34A[15]="switch_clauses";static char _tmp34B[20]="iteration_statement";
static char _tmp34C[15]="jump_statement";static char _tmp34D[12]="exp_pattern";
static char _tmp34E[20]="conditional_pattern";static char _tmp34F[19]="logical_or_pattern";
static char _tmp350[20]="logical_and_pattern";static char _tmp351[21]="inclusive_or_pattern";
static char _tmp352[21]="exclusive_or_pattern";static char _tmp353[12]="and_pattern";
static char _tmp354[17]="equality_pattern";static char _tmp355[19]="relational_pattern";
static char _tmp356[14]="shift_pattern";static char _tmp357[17]="additive_pattern";
static char _tmp358[23]="multiplicative_pattern";static char _tmp359[13]="cast_pattern";
static char _tmp35A[14]="unary_pattern";static char _tmp35B[16]="postfix_pattern";
static char _tmp35C[16]="primary_pattern";static char _tmp35D[8]="pattern";static
char _tmp35E[19]="tuple_pattern_list";static char _tmp35F[20]="tuple_pattern_list0";
static char _tmp360[14]="field_pattern";static char _tmp361[19]="field_pattern_list";
static char _tmp362[20]="field_pattern_list0";static char _tmp363[11]="expression";
static char _tmp364[22]="assignment_expression";static char _tmp365[20]="assignment_operator";
static char _tmp366[23]="conditional_expression";static char _tmp367[20]="constant_expression";
static char _tmp368[22]="logical_or_expression";static char _tmp369[23]="logical_and_expression";
static char _tmp36A[24]="inclusive_or_expression";static char _tmp36B[24]="exclusive_or_expression";
static char _tmp36C[15]="and_expression";static char _tmp36D[20]="equality_expression";
static char _tmp36E[22]="relational_expression";static char _tmp36F[17]="shift_expression";
static char _tmp370[20]="additive_expression";static char _tmp371[26]="multiplicative_expression";
static char _tmp372[16]="cast_expression";static char _tmp373[17]="unary_expression";
static char _tmp374[15]="unary_operator";static char _tmp375[19]="postfix_expression";
static char _tmp376[19]="primary_expression";static char _tmp377[25]="argument_expression_list";
static char _tmp378[26]="argument_expression_list0";static char _tmp379[9]="constant";
static char _tmp37A[20]="qual_opt_identifier";static char _tmp37B[17]="qual_opt_typedef";
static char _tmp37C[18]="struct_union_name";static char _tmp37D[11]="field_name";
static char _tmp37E[12]="right_angle";static struct _dyneither_ptr Cyc_yytname[276]={{
_tmp26B,_tmp26B,_tmp26B + 2},{_tmp26C,_tmp26C,_tmp26C + 6},{_tmp26D,_tmp26D,
_tmp26D + 12},{_tmp26E,_tmp26E,_tmp26E + 5},{_tmp26F,_tmp26F,_tmp26F + 9},{_tmp270,
_tmp270,_tmp270 + 7},{_tmp271,_tmp271,_tmp271 + 7},{_tmp272,_tmp272,_tmp272 + 8},{
_tmp273,_tmp273,_tmp273 + 5},{_tmp274,_tmp274,_tmp274 + 5},{_tmp275,_tmp275,
_tmp275 + 6},{_tmp276,_tmp276,_tmp276 + 4},{_tmp277,_tmp277,_tmp277 + 5},{_tmp278,
_tmp278,_tmp278 + 6},{_tmp279,_tmp279,_tmp279 + 7},{_tmp27A,_tmp27A,_tmp27A + 7},{
_tmp27B,_tmp27B,_tmp27B + 9},{_tmp27C,_tmp27C,_tmp27C + 6},{_tmp27D,_tmp27D,
_tmp27D + 9},{_tmp27E,_tmp27E,_tmp27E + 9},{_tmp27F,_tmp27F,_tmp27F + 7},{_tmp280,
_tmp280,_tmp280 + 6},{_tmp281,_tmp281,_tmp281 + 5},{_tmp282,_tmp282,_tmp282 + 8},{
_tmp283,_tmp283,_tmp283 + 7},{_tmp284,_tmp284,_tmp284 + 7},{_tmp285,_tmp285,
_tmp285 + 9},{_tmp286,_tmp286,_tmp286 + 3},{_tmp287,_tmp287,_tmp287 + 5},{_tmp288,
_tmp288,_tmp288 + 7},{_tmp289,_tmp289,_tmp289 + 6},{_tmp28A,_tmp28A,_tmp28A + 3},{
_tmp28B,_tmp28B,_tmp28B + 4},{_tmp28C,_tmp28C,_tmp28C + 5},{_tmp28D,_tmp28D,
_tmp28D + 9},{_tmp28E,_tmp28E,_tmp28E + 6},{_tmp28F,_tmp28F,_tmp28F + 7},{_tmp290,
_tmp290,_tmp290 + 5},{_tmp291,_tmp291,_tmp291 + 8},{_tmp292,_tmp292,_tmp292 + 4},{
_tmp293,_tmp293,_tmp293 + 6},{_tmp294,_tmp294,_tmp294 + 4},{_tmp295,_tmp295,
_tmp295 + 6},{_tmp296,_tmp296,_tmp296 + 7},{_tmp297,_tmp297,_tmp297 + 4},{_tmp298,
_tmp298,_tmp298 + 9},{_tmp299,_tmp299,_tmp299 + 9},{_tmp29A,_tmp29A,_tmp29A + 6},{
_tmp29B,_tmp29B,_tmp29B + 10},{_tmp29C,_tmp29C,_tmp29C + 7},{_tmp29D,_tmp29D,
_tmp29D + 8},{_tmp29E,_tmp29E,_tmp29E + 7},{_tmp29F,_tmp29F,_tmp29F + 8},{_tmp2A0,
_tmp2A0,_tmp2A0 + 7},{_tmp2A1,_tmp2A1,_tmp2A1 + 8},{_tmp2A2,_tmp2A2,_tmp2A2 + 5},{
_tmp2A3,_tmp2A3,_tmp2A3 + 9},{_tmp2A4,_tmp2A4,_tmp2A4 + 6},{_tmp2A5,_tmp2A5,
_tmp2A5 + 7},{_tmp2A6,_tmp2A6,_tmp2A6 + 5},{_tmp2A7,_tmp2A7,_tmp2A7 + 8},{_tmp2A8,
_tmp2A8,_tmp2A8 + 13},{_tmp2A9,_tmp2A9,_tmp2A9 + 4},{_tmp2AA,_tmp2AA,_tmp2AA + 14},{
_tmp2AB,_tmp2AB,_tmp2AB + 12},{_tmp2AC,_tmp2AC,_tmp2AC + 7},{_tmp2AD,_tmp2AD,
_tmp2AD + 8},{_tmp2AE,_tmp2AE,_tmp2AE + 8},{_tmp2AF,_tmp2AF,_tmp2AF + 12},{_tmp2B0,
_tmp2B0,_tmp2B0 + 6},{_tmp2B1,_tmp2B1,_tmp2B1 + 8},{_tmp2B2,_tmp2B2,_tmp2B2 + 8},{
_tmp2B3,_tmp2B3,_tmp2B3 + 10},{_tmp2B4,_tmp2B4,_tmp2B4 + 7},{_tmp2B5,_tmp2B5,
_tmp2B5 + 7},{_tmp2B6,_tmp2B6,_tmp2B6 + 7},{_tmp2B7,_tmp2B7,_tmp2B7 + 8},{_tmp2B8,
_tmp2B8,_tmp2B8 + 9},{_tmp2B9,_tmp2B9,_tmp2B9 + 6},{_tmp2BA,_tmp2BA,_tmp2BA + 6},{
_tmp2BB,_tmp2BB,_tmp2BB + 6},{_tmp2BC,_tmp2BC,_tmp2BC + 6},{_tmp2BD,_tmp2BD,
_tmp2BD + 7},{_tmp2BE,_tmp2BE,_tmp2BE + 6},{_tmp2BF,_tmp2BF,_tmp2BF + 11},{_tmp2C0,
_tmp2C0,_tmp2C0 + 11},{_tmp2C1,_tmp2C1,_tmp2C1 + 11},{_tmp2C2,_tmp2C2,_tmp2C2 + 11},{
_tmp2C3,_tmp2C3,_tmp2C3 + 11},{_tmp2C4,_tmp2C4,_tmp2C4 + 12},{_tmp2C5,_tmp2C5,
_tmp2C5 + 13},{_tmp2C6,_tmp2C6,_tmp2C6 + 11},{_tmp2C7,_tmp2C7,_tmp2C7 + 11},{
_tmp2C8,_tmp2C8,_tmp2C8 + 10},{_tmp2C9,_tmp2C9,_tmp2C9 + 9},{_tmp2CA,_tmp2CA,
_tmp2CA + 11},{_tmp2CB,_tmp2CB,_tmp2CB + 12},{_tmp2CC,_tmp2CC,_tmp2CC + 11},{
_tmp2CD,_tmp2CD,_tmp2CD + 17},{_tmp2CE,_tmp2CE,_tmp2CE + 7},{_tmp2CF,_tmp2CF,
_tmp2CF + 19},{_tmp2D0,_tmp2D0,_tmp2D0 + 18},{_tmp2D1,_tmp2D1,_tmp2D1 + 9},{_tmp2D2,
_tmp2D2,_tmp2D2 + 13},{_tmp2D3,_tmp2D3,_tmp2D3 + 16},{_tmp2D4,_tmp2D4,_tmp2D4 + 18},{
_tmp2D5,_tmp2D5,_tmp2D5 + 10},{_tmp2D6,_tmp2D6,_tmp2D6 + 4},{_tmp2D7,_tmp2D7,
_tmp2D7 + 4},{_tmp2D8,_tmp2D8,_tmp2D8 + 4},{_tmp2D9,_tmp2D9,_tmp2D9 + 4},{_tmp2DA,
_tmp2DA,_tmp2DA + 4},{_tmp2DB,_tmp2DB,_tmp2DB + 4},{_tmp2DC,_tmp2DC,_tmp2DC + 4},{
_tmp2DD,_tmp2DD,_tmp2DD + 4},{_tmp2DE,_tmp2DE,_tmp2DE + 4},{_tmp2DF,_tmp2DF,
_tmp2DF + 4},{_tmp2E0,_tmp2E0,_tmp2E0 + 4},{_tmp2E1,_tmp2E1,_tmp2E1 + 4},{_tmp2E2,
_tmp2E2,_tmp2E2 + 4},{_tmp2E3,_tmp2E3,_tmp2E3 + 4},{_tmp2E4,_tmp2E4,_tmp2E4 + 4},{
_tmp2E5,_tmp2E5,_tmp2E5 + 4},{_tmp2E6,_tmp2E6,_tmp2E6 + 4},{_tmp2E7,_tmp2E7,
_tmp2E7 + 4},{_tmp2E8,_tmp2E8,_tmp2E8 + 4},{_tmp2E9,_tmp2E9,_tmp2E9 + 4},{_tmp2EA,
_tmp2EA,_tmp2EA + 4},{_tmp2EB,_tmp2EB,_tmp2EB + 4},{_tmp2EC,_tmp2EC,_tmp2EC + 4},{
_tmp2ED,_tmp2ED,_tmp2ED + 4},{_tmp2EE,_tmp2EE,_tmp2EE + 4},{_tmp2EF,_tmp2EF,
_tmp2EF + 4},{_tmp2F0,_tmp2F0,_tmp2F0 + 4},{_tmp2F1,_tmp2F1,_tmp2F1 + 5},{_tmp2F2,
_tmp2F2,_tmp2F2 + 17},{_tmp2F3,_tmp2F3,_tmp2F3 + 12},{_tmp2F4,_tmp2F4,_tmp2F4 + 19},{
_tmp2F5,_tmp2F5,_tmp2F5 + 21},{_tmp2F6,_tmp2F6,_tmp2F6 + 20},{_tmp2F7,_tmp2F7,
_tmp2F7 + 21},{_tmp2F8,_tmp2F8,_tmp2F8 + 13},{_tmp2F9,_tmp2F9,_tmp2F9 + 15},{
_tmp2FA,_tmp2FA,_tmp2FA + 17},{_tmp2FB,_tmp2FB,_tmp2FB + 19},{_tmp2FC,_tmp2FC,
_tmp2FC + 12},{_tmp2FD,_tmp2FD,_tmp2FD + 17},{_tmp2FE,_tmp2FE,_tmp2FE + 23},{
_tmp2FF,_tmp2FF,_tmp2FF + 24},{_tmp300,_tmp300,_tmp300 + 15},{_tmp301,_tmp301,
_tmp301 + 11},{_tmp302,_tmp302,_tmp302 + 15},{_tmp303,_tmp303,_tmp303 + 10},{
_tmp304,_tmp304,_tmp304 + 15},{_tmp305,_tmp305,_tmp305 + 25},{_tmp306,_tmp306,
_tmp306 + 5},{_tmp307,_tmp307,_tmp307 + 15},{_tmp308,_tmp308,_tmp308 + 15},{_tmp309,
_tmp309,_tmp309 + 11},{_tmp30A,_tmp30A,_tmp30A + 22},{_tmp30B,_tmp30B,_tmp30B + 26},{
_tmp30C,_tmp30C,_tmp30C + 16},{_tmp30D,_tmp30D,_tmp30D + 16},{_tmp30E,_tmp30E,
_tmp30E + 24},{_tmp30F,_tmp30F,_tmp30F + 25},{_tmp310,_tmp310,_tmp310 + 21},{
_tmp311,_tmp311,_tmp311 + 22},{_tmp312,_tmp312,_tmp312 + 16},{_tmp313,_tmp313,
_tmp313 + 19},{_tmp314,_tmp314,_tmp314 + 25},{_tmp315,_tmp315,_tmp315 + 35},{
_tmp316,_tmp316,_tmp316 + 23},{_tmp317,_tmp317,_tmp317 + 24},{_tmp318,_tmp318,
_tmp318 + 18},{_tmp319,_tmp319,_tmp319 + 17},{_tmp31A,_tmp31A,_tmp31A + 9},{_tmp31B,
_tmp31B,_tmp31B + 18},{_tmp31C,_tmp31C,_tmp31C + 17},{_tmp31D,_tmp31D,_tmp31D + 18},{
_tmp31E,_tmp31E,_tmp31E + 12},{_tmp31F,_tmp31F,_tmp31F + 11},{_tmp320,_tmp320,
_tmp320 + 23},{_tmp321,_tmp321,_tmp321 + 18},{_tmp322,_tmp322,_tmp322 + 30},{
_tmp323,_tmp323,_tmp323 + 8},{_tmp324,_tmp324,_tmp324 + 12},{_tmp325,_tmp325,
_tmp325 + 23},{_tmp326,_tmp326,_tmp326 + 14},{_tmp327,_tmp327,_tmp327 + 18},{
_tmp328,_tmp328,_tmp328 + 12},{_tmp329,_tmp329,_tmp329 + 8},{_tmp32A,_tmp32A,
_tmp32A + 11},{_tmp32B,_tmp32B,_tmp32B + 20},{_tmp32C,_tmp32C,_tmp32C + 9},{_tmp32D,
_tmp32D,_tmp32D + 16},{_tmp32E,_tmp32E,_tmp32E + 19},{_tmp32F,_tmp32F,_tmp32F + 10},{
_tmp330,_tmp330,_tmp330 + 16},{_tmp331,_tmp331,_tmp331 + 11},{_tmp332,_tmp332,
_tmp332 + 14},{_tmp333,_tmp333,_tmp333 + 11},{_tmp334,_tmp334,_tmp334 + 15},{
_tmp335,_tmp335,_tmp335 + 22},{_tmp336,_tmp336,_tmp336 + 16},{_tmp337,_tmp337,
_tmp337 + 17},{_tmp338,_tmp338,_tmp338 + 12},{_tmp339,_tmp339,_tmp339 + 18},{
_tmp33A,_tmp33A,_tmp33A + 17},{_tmp33B,_tmp33B,_tmp33B + 12},{_tmp33C,_tmp33C,
_tmp33C + 16},{_tmp33D,_tmp33D,_tmp33D + 11},{_tmp33E,_tmp33E,_tmp33E + 10},{
_tmp33F,_tmp33F,_tmp33F + 14},{_tmp340,_tmp340,_tmp340 + 15},{_tmp341,_tmp341,
_tmp341 + 20},{_tmp342,_tmp342,_tmp342 + 27},{_tmp343,_tmp343,_tmp343 + 10},{
_tmp344,_tmp344,_tmp344 + 13},{_tmp345,_tmp345,_tmp345 + 18},{_tmp346,_tmp346,
_tmp346 + 21},{_tmp347,_tmp347,_tmp347 + 19},{_tmp348,_tmp348,_tmp348 + 16},{
_tmp349,_tmp349,_tmp349 + 20},{_tmp34A,_tmp34A,_tmp34A + 15},{_tmp34B,_tmp34B,
_tmp34B + 20},{_tmp34C,_tmp34C,_tmp34C + 15},{_tmp34D,_tmp34D,_tmp34D + 12},{
_tmp34E,_tmp34E,_tmp34E + 20},{_tmp34F,_tmp34F,_tmp34F + 19},{_tmp350,_tmp350,
_tmp350 + 20},{_tmp351,_tmp351,_tmp351 + 21},{_tmp352,_tmp352,_tmp352 + 21},{
_tmp353,_tmp353,_tmp353 + 12},{_tmp354,_tmp354,_tmp354 + 17},{_tmp355,_tmp355,
_tmp355 + 19},{_tmp356,_tmp356,_tmp356 + 14},{_tmp357,_tmp357,_tmp357 + 17},{
_tmp358,_tmp358,_tmp358 + 23},{_tmp359,_tmp359,_tmp359 + 13},{_tmp35A,_tmp35A,
_tmp35A + 14},{_tmp35B,_tmp35B,_tmp35B + 16},{_tmp35C,_tmp35C,_tmp35C + 16},{
_tmp35D,_tmp35D,_tmp35D + 8},{_tmp35E,_tmp35E,_tmp35E + 19},{_tmp35F,_tmp35F,
_tmp35F + 20},{_tmp360,_tmp360,_tmp360 + 14},{_tmp361,_tmp361,_tmp361 + 19},{
_tmp362,_tmp362,_tmp362 + 20},{_tmp363,_tmp363,_tmp363 + 11},{_tmp364,_tmp364,
_tmp364 + 22},{_tmp365,_tmp365,_tmp365 + 20},{_tmp366,_tmp366,_tmp366 + 23},{
_tmp367,_tmp367,_tmp367 + 20},{_tmp368,_tmp368,_tmp368 + 22},{_tmp369,_tmp369,
_tmp369 + 23},{_tmp36A,_tmp36A,_tmp36A + 24},{_tmp36B,_tmp36B,_tmp36B + 24},{
_tmp36C,_tmp36C,_tmp36C + 15},{_tmp36D,_tmp36D,_tmp36D + 20},{_tmp36E,_tmp36E,
_tmp36E + 22},{_tmp36F,_tmp36F,_tmp36F + 17},{_tmp370,_tmp370,_tmp370 + 20},{
_tmp371,_tmp371,_tmp371 + 26},{_tmp372,_tmp372,_tmp372 + 16},{_tmp373,_tmp373,
_tmp373 + 17},{_tmp374,_tmp374,_tmp374 + 15},{_tmp375,_tmp375,_tmp375 + 19},{
_tmp376,_tmp376,_tmp376 + 19},{_tmp377,_tmp377,_tmp377 + 25},{_tmp378,_tmp378,
_tmp378 + 26},{_tmp379,_tmp379,_tmp379 + 9},{_tmp37A,_tmp37A,_tmp37A + 20},{_tmp37B,
_tmp37B,_tmp37B + 17},{_tmp37C,_tmp37C,_tmp37C + 18},{_tmp37D,_tmp37D,_tmp37D + 11},{
_tmp37E,_tmp37E,_tmp37E + 12}};static short Cyc_yyr1[500]={0,134,135,135,135,135,
135,135,135,135,135,135,136,136,137,137,138,138,138,139,139,139,139,140,140,141,
142,143,144,145,145,145,145,145,145,145,145,145,146,146,147,147,147,147,147,147,
147,147,147,147,148,148,148,148,148,148,148,149,149,150,151,151,152,152,152,152,
152,152,153,153,154,154,154,154,154,154,154,154,154,154,154,154,154,154,154,154,
154,154,154,154,154,154,155,156,156,156,157,157,157,158,158,159,159,159,160,160,
160,161,161,162,162,163,163,164,164,165,166,166,167,167,168,169,169,169,169,169,
169,170,170,170,170,170,170,171,172,172,173,173,173,174,174,174,175,175,176,176,
177,177,177,177,178,178,178,179,179,180,180,181,181,182,182,182,182,182,182,182,
182,182,182,183,183,183,183,183,183,183,183,183,183,183,184,184,185,186,186,186,
187,187,188,188,188,189,189,189,190,190,190,191,191,192,192,192,192,193,193,194,
194,195,195,196,196,197,197,198,198,199,199,199,199,200,200,201,201,202,202,202,
203,204,204,205,205,206,206,206,206,207,207,207,207,208,209,209,210,210,211,211,
212,212,212,212,212,213,213,214,214,214,215,215,215,215,215,215,215,215,215,215,
215,216,216,216,216,216,216,216,217,217,218,219,219,220,220,221,221,221,221,221,
221,222,222,222,222,222,222,223,223,223,223,223,223,224,224,224,224,224,224,224,
224,224,224,224,224,224,224,225,225,225,225,225,225,225,225,226,227,227,228,228,
229,229,230,230,231,231,232,232,233,233,233,234,234,234,234,234,235,235,235,236,
236,236,237,237,237,237,238,238,239,239,239,239,239,239,239,240,241,242,242,242,
242,242,242,242,242,242,242,242,242,242,242,243,243,243,244,244,245,245,246,246,
246,247,247,248,248,249,249,250,250,250,250,250,250,250,250,250,250,250,251,251,
251,251,251,251,251,252,253,253,254,254,255,255,256,256,257,257,258,258,258,259,
259,259,259,259,260,260,260,261,261,261,262,262,262,262,263,263,264,264,264,264,
264,264,264,264,264,264,264,264,264,264,264,264,264,264,264,265,265,265,266,266,
266,266,266,266,266,266,266,266,267,267,267,267,267,267,267,267,267,268,269,269,
270,270,270,270,271,271,272,272,273,273,274,274,275,275};static short Cyc_yyr2[500]={
0,1,2,3,5,3,5,6,7,3,3,0,4,3,2,3,1,1,1,2,3,3,4,3,4,2,1,2,1,2,3,5,3,6,4,9,6,8,1,2,
1,2,1,2,1,2,1,2,1,2,1,1,1,1,2,1,1,0,1,6,1,3,1,1,4,4,4,8,1,2,1,1,3,1,1,1,1,1,1,1,
1,1,1,1,1,4,4,4,6,4,1,4,1,1,1,1,5,2,4,1,3,1,2,3,4,8,3,0,3,1,1,0,1,1,2,1,1,3,1,3,
3,1,2,1,2,1,2,1,2,1,2,1,2,1,1,3,1,2,3,8,5,7,1,0,1,1,1,2,3,3,0,1,1,2,5,1,2,1,2,1,
3,4,4,5,4,5,4,4,2,1,1,3,4,4,5,4,5,4,4,2,1,2,5,2,2,1,0,3,0,1,1,0,1,1,0,1,1,0,2,3,
5,5,7,1,3,0,2,0,2,3,5,0,1,1,3,2,3,4,1,1,3,1,3,2,1,2,1,1,3,1,1,2,3,4,8,1,2,3,4,2,
1,2,3,2,1,2,1,2,3,4,3,1,3,1,1,2,3,3,4,4,5,4,3,5,4,4,2,1,1,1,1,1,1,5,0,5,3,1,2,2,
3,1,2,1,2,1,2,5,7,7,5,8,6,0,3,4,5,6,7,5,7,6,7,7,8,7,8,8,9,6,7,7,8,3,2,2,2,3,2,4,
5,1,1,5,1,3,1,3,1,3,1,3,1,3,1,3,3,1,3,3,3,3,1,3,3,1,3,3,1,3,3,3,1,4,1,2,2,4,2,6,
6,1,1,1,3,1,1,3,4,4,5,2,2,2,4,4,4,1,3,1,1,3,1,2,1,3,1,1,3,1,3,1,3,1,1,1,1,1,1,1,
1,1,1,1,1,5,2,2,2,5,5,1,1,3,1,3,1,3,1,3,1,3,1,3,3,1,3,3,3,3,1,3,3,1,3,3,1,3,3,3,
1,4,1,2,2,2,2,2,2,4,2,6,6,5,4,6,9,11,6,4,4,1,1,1,1,4,3,4,3,3,2,2,6,7,1,1,1,3,2,5,
4,5,5,1,1,3,1,1,1,1,1,1,1,1,1,1,1,1,1,1};static short Cyc_yydefact[1010]={0,18,50,
51,52,53,55,70,73,74,75,76,77,78,79,80,93,94,95,109,110,46,0,0,56,0,0,0,90,0,0,0,
142,0,0,0,490,203,492,491,493,0,0,71,0,186,186,185,1,0,16,0,0,17,0,40,48,42,68,
44,81,82,0,83,0,143,155,0,180,188,84,159,107,54,53,47,0,97,489,0,490,486,487,488,
0,363,0,0,0,0,226,0,365,366,25,27,143,143,274,0,0,0,0,143,0,0,0,0,0,0,0,143,0,
183,184,2,0,0,0,0,29,0,115,116,118,41,49,43,45,143,494,495,107,144,145,191,143,
38,143,0,19,143,0,143,168,156,181,190,189,194,143,69,0,54,101,0,99,0,490,372,0,0,
0,0,0,0,0,0,0,0,0,0,107,0,0,0,0,476,143,0,0,0,0,463,461,462,0,389,391,404,412,
414,416,418,420,422,425,430,433,436,440,0,442,464,475,474,0,373,371,32,0,0,107,0,
0,0,125,121,123,244,246,0,0,0,0,0,0,9,10,0,0,0,496,497,204,92,0,0,160,72,224,0,
221,0,3,0,5,0,30,0,0,143,20,0,143,113,0,106,193,0,192,0,0,0,0,0,0,0,0,0,0,0,0,
490,277,279,0,285,281,0,283,267,268,269,0,270,271,272,0,39,21,118,251,0,188,0,
211,227,0,0,207,205,0,196,57,195,0,0,102,98,0,0,367,0,0,143,450,143,406,440,0,
407,408,0,0,0,0,0,0,0,0,143,143,443,444,143,0,0,0,446,447,445,0,364,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,394,395,396,397,398,399,400,401,402,403,393,0,448,0,
470,471,0,0,0,478,0,107,379,380,0,377,0,228,0,0,0,143,247,219,0,126,131,127,129,
122,124,0,143,253,245,254,499,498,0,86,89,0,34,0,0,143,87,0,91,63,62,0,60,161,
143,223,157,253,225,169,170,143,85,187,26,0,28,0,117,119,230,229,22,104,114,0,0,
0,133,134,136,0,107,107,0,0,0,0,0,0,143,0,314,315,316,0,0,318,0,0,0,143,286,282,
118,284,280,278,143,167,162,188,212,143,0,0,218,206,213,164,0,0,143,207,166,197,
58,108,0,103,411,100,96,375,376,0,0,0,231,0,0,235,0,0,0,240,0,0,0,0,0,0,143,0,0,
0,0,0,477,484,0,483,390,413,0,415,417,419,421,423,424,428,429,426,427,431,432,
434,435,437,438,439,392,469,0,466,0,468,143,0,368,0,374,31,386,0,382,387,0,384,
369,0,0,248,132,128,130,188,0,0,207,0,255,143,0,143,266,250,0,0,0,0,0,0,0,0,0,
125,0,107,143,0,143,179,158,222,4,6,0,137,120,0,0,207,0,140,0,0,0,293,0,0,0,0,0,
313,317,0,0,0,0,276,143,23,252,163,205,143,215,0,0,208,0,165,211,199,197,182,0,7,
0,449,0,0,0,243,232,0,236,239,241,454,0,0,0,0,0,0,459,460,0,0,0,441,480,0,0,465,
467,0,0,378,381,383,370,0,249,220,257,188,262,0,256,0,188,0,0,207,0,33,36,0,88,0,
0,0,0,59,61,0,171,0,188,0,0,207,0,135,138,143,107,150,0,0,0,0,0,0,0,0,0,0,0,0,0,
293,319,0,0,24,207,0,216,214,0,207,143,198,0,8,0,0,0,242,233,237,0,0,0,0,0,409,
410,453,482,0,485,405,479,481,0,385,388,259,261,265,258,188,264,0,275,0,0,0,66,
64,65,172,178,173,188,175,0,177,0,141,152,151,0,0,146,287,293,0,0,0,143,0,0,321,
322,324,326,328,330,332,334,337,342,345,348,352,354,361,362,0,143,290,299,0,0,0,
0,0,0,0,0,0,0,320,273,201,217,209,200,205,13,0,0,452,451,0,238,455,143,0,458,472,
0,260,263,0,37,0,174,176,105,139,153,150,150,0,0,293,143,358,143,0,355,143,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,356,294,0,301,0,0,0,309,0,0,0,0,0,0,292,0,207,
12,14,0,0,0,0,473,35,0,143,149,148,288,289,0,0,0,0,293,295,325,0,327,329,331,333,
335,336,340,341,338,339,343,344,346,347,349,350,351,0,300,302,303,0,311,310,0,
305,0,0,0,210,202,15,0,0,143,0,0,291,357,0,353,296,0,143,304,312,306,307,0,234,
456,0,67,154,0,0,323,293,297,308,0,360,359,298,457,0,0,0};static short Cyc_yydefgoto[
142]={1007,48,653,850,49,50,277,51,438,52,440,53,138,54,55,498,216,424,425,217,
58,234,218,60,154,155,61,151,62,253,254,121,122,123,255,219,402,450,451,452,63,
64,135,801,802,803,65,453,66,429,67,68,69,113,149,259,304,650,582,70,583,494,644,
486,490,491,397,301,242,89,90,514,443,515,516,517,518,220,292,293,584,408,280,
224,281,282,283,284,285,730,286,287,811,812,813,814,815,816,817,818,819,820,821,
822,823,824,825,826,386,387,388,570,571,572,288,187,374,188,504,189,190,191,192,
193,194,195,196,197,198,199,200,201,202,203,533,534,204,205,72,851,235,412};
static short Cyc_yypact[1010]={2515,- -32768,- -32768,- -32768,- -32768,- 49,- -32768,-
-32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,-
-32768,- -32768,- -32768,- -32768,2903,155,941,- -32768,138,- 16,63,93,136,5,16,-
-32768,126,170,40,- -32768,221,- -32768,- -32768,- -32768,181,650,246,214,262,262,-
-32768,- -32768,2390,- -32768,149,363,- -32768,740,2903,2903,2903,- -32768,2903,-
-32768,- -32768,705,- -32768,531,2831,505,- 8,653,529,- -32768,- -32768,268,278,293,-
-32768,138,290,- -32768,2382,299,- -32768,- -32768,- -32768,5632,- -32768,303,346,
2382,342,345,350,- -32768,218,- -32768,- -32768,3291,3291,389,381,411,2390,2390,
3291,410,5632,271,423,928,409,271,3626,5632,- -32768,- -32768,- -32768,2390,2639,
2390,2639,- -32768,442,451,- -32768,2761,- -32768,- -32768,- -32768,- -32768,3425,-
-32768,- -32768,268,- -32768,- -32768,- 23,1617,- -32768,2831,740,- -32768,3291,4218,
3157,- -32768,505,- -32768,- -32768,- -32768,104,3291,- -32768,2639,- -32768,460,467,
492,138,- 20,- -32768,2382,111,5797,520,5632,5672,522,549,564,570,589,607,268,624,
632,5880,5880,- -32768,2005,634,5920,5920,5920,- -32768,- -32768,- -32768,184,-
-32768,- -32768,- 3,526,496,625,600,547,103,659,296,201,- -32768,902,5920,98,44,-
-32768,649,853,673,- -32768,- -32768,687,5632,268,853,672,212,3358,3693,3358,712,-
-32768,- 42,- 42,693,692,702,728,- -32768,- -32768,118,706,244,- -32768,- -32768,-
-32768,- -32768,36,729,- -32768,- -32768,754,285,- -32768,743,- -32768,752,- -32768,
766,- -32768,928,4301,2831,- -32768,771,3492,- -32768,555,761,- -32768,138,- -32768,
797,92,805,3788,806,827,818,820,4387,3788,76,812,813,- -32768,- -32768,823,1486,
1486,740,1486,- -32768,- -32768,- -32768,825,- -32768,- -32768,- -32768,129,- -32768,-
-32768,829,811,78,529,828,847,- -32768,66,831,826,161,841,- -32768,839,- -32768,78,
851,138,- -32768,5632,852,- -32768,849,850,2005,- -32768,3626,- -32768,- -32768,3874,
- -32768,881,5632,5632,5632,5632,5632,5632,854,5632,3626,2005,- -32768,- -32768,
1748,856,305,5632,- -32768,- -32768,- -32768,5632,- -32768,5920,5632,5920,5920,5920,
5920,5920,5920,5920,5920,5920,5920,5920,5920,5920,5920,5920,5920,5920,- -32768,-
-32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,
5632,- -32768,271,- -32768,- -32768,5632,4470,271,- -32768,864,268,- -32768,- -32768,
861,867,2382,- -32768,234,664,863,3291,- -32768,872,874,- -32768,3693,3693,3693,-
-32768,- -32768,4553,2970,498,- -32768,627,- -32768,- -32768,66,- -32768,- -32768,868,
- -32768,899,67,3291,- -32768,903,- -32768,- -32768,885,887,892,- -32768,1088,- -32768,
675,587,- -32768,- -32768,- -32768,3626,- -32768,- -32768,- -32768,2390,- -32768,2390,-
-32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,1015,5632,900,896,- -32768,
889,255,268,141,5632,5632,894,904,5632,981,1879,907,- -32768,- -32768,- -32768,300,
973,- -32768,4636,5632,3788,2131,- -32768,- -32768,2761,- -32768,- -32768,- -32768,
3291,- -32768,- -32768,529,- -32768,3626,901,337,- -32768,- -32768,891,- -32768,66,912,
3559,826,- -32768,787,- -32768,- -32768,2265,- -32768,- -32768,- -32768,- -32768,-
-32768,- -32768,916,908,922,- -32768,5632,271,- -32768,714,4301,189,- -32768,374,910,
911,914,930,382,3626,385,931,932,938,5757,- -32768,- -32768,936,944,- -32768,526,
114,496,625,600,547,103,103,659,659,659,659,296,296,201,201,- -32768,- -32768,-
-32768,- -32768,- -32768,437,- -32768,939,- -32768,3291,4132,- -32768,2257,- -32768,-
-32768,- -32768,2382,- -32768,- -32768,946,948,- -32768,222,959,- -32768,- -32768,-
-32768,- -32768,529,950,954,826,957,627,3291,4719,3224,- -32768,- -32768,5632,960,
969,976,- 42,968,741,964,36,3087,965,268,3291,4802,3157,- -32768,675,- -32768,-
-32768,- -32768,1030,- -32768,- -32768,555,5632,826,138,975,386,427,5632,737,433,
971,4885,4968,372,- -32768,- -32768,977,983,967,443,- -32768,2831,- -32768,811,-
-32768,986,3291,- -32768,979,66,- -32768,997,- -32768,199,- -32768,787,- -32768,1003,
- -32768,2390,1006,362,982,1000,- -32768,- -32768,3960,- -32768,- -32768,- -32768,-
-32768,5632,1091,5632,5632,5672,1005,- -32768,- -32768,1006,1009,4132,- -32768,-
-32768,5632,5632,- -32768,- -32768,78,721,- -32768,- -32768,- -32768,- -32768,1306,-
-32768,- -32768,- -32768,529,- -32768,1011,- -32768,78,529,1008,1012,826,455,- -32768,
- -32768,1017,- -32768,5632,456,1019,1023,- -32768,- -32768,1024,- -32768,78,529,1027,
1026,826,1031,- -32768,- -32768,3425,268,860,3788,1039,1033,3654,1037,1042,3788,
5632,5051,428,5134,506,5217,737,- -32768,1025,1051,- -32768,826,272,- -32768,-
-32768,1057,826,3626,- -32768,542,- -32768,1044,1045,5632,- -32768,- -32768,- -32768,
4301,465,1047,1054,481,- -32768,881,- -32768,- -32768,776,- -32768,- -32768,- -32768,-
-32768,4132,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,529,- -32768,1049,-
-32768,1070,599,1071,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,529,- -32768,
1053,- -32768,1059,- -32768,- -32768,- -32768,1062,138,617,1144,737,1065,6003,1058,
2131,5920,1056,- -32768,11,- -32768,1095,1055,760,809,176,816,312,254,- -32768,-
-32768,- -32768,- -32768,1098,5920,1748,- -32768,- -32768,487,3788,516,5300,3788,539,
5383,5466,704,1074,- -32768,- -32768,- -32768,- -32768,1076,- -32768,986,- -32768,1075,
710,- -32768,- -32768,163,- -32768,- -32768,3626,1162,- -32768,- -32768,4046,- -32768,-
-32768,1081,- -32768,1079,- -32768,- -32768,- -32768,- -32768,1080,69,860,3788,1089,
737,2005,- -32768,3626,1082,- -32768,1264,5920,5632,5920,5920,5920,5920,5920,5920,
5920,5920,5920,5920,5920,5920,5920,5920,5920,5920,5920,5632,- -32768,- -32768,1090,
- -32768,3788,3788,544,- -32768,3788,3788,566,3788,579,5549,- -32768,66,826,- -32768,
- -32768,590,5632,1083,1085,- -32768,- -32768,1105,3626,- -32768,- -32768,- -32768,-
-32768,1100,1096,1104,5920,737,- -32768,526,165,496,625,625,547,103,103,659,659,
659,659,296,296,201,201,- -32768,- -32768,- -32768,202,- -32768,- -32768,- -32768,3788,
- -32768,- -32768,3788,- -32768,3788,3788,601,- -32768,- -32768,- -32768,808,1099,3626,
1101,612,- -32768,1006,697,- -32768,- -32768,5632,1264,- -32768,- -32768,- -32768,-
-32768,3788,- -32768,- -32768,1103,- -32768,- -32768,1106,1108,- -32768,737,- -32768,-
-32768,1109,- -32768,- -32768,- -32768,- -32768,1215,1221,- -32768};static short Cyc_yypgoto[
142]={- -32768,415,- -32768,306,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,-
-32768,- 33,- 96,- 11,- -32768,- -32768,0,623,- -32768,85,- 174,1117,13,- -32768,- -32768,
- 110,- -32768,- 105,- -32768,508,- -32768,- -32768,- -32768,984,978,- 107,473,- -32768,-
-32768,620,- -32768,- -32768,- -32768,47,- -32768,- -32768,46,- 228,1164,- 344,30,-
-32768,- -32768,1189,52,- -32768,- -32768,591,- 119,31,- 132,- 238,318,592,595,- 409,-
455,- 108,- 396,- 120,- -32768,- 232,- 149,- 498,- 355,- -32768,724,- 169,- 45,- 57,- 143,-
236,123,- -32768,- -32768,- -32768,- 51,- 272,- -32768,- 595,- -32768,- -32768,- -32768,-
-32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,-
-32768,- -32768,- -32768,- -32768,- -32768,153,1032,- -32768,556,- -32768,- -32768,340,
263,- -32768,- 157,- 408,- 139,- 343,- 331,- 328,895,- 332,- 321,- 194,- 317,- 298,127,550,
518,- -32768,- -32768,- 349,- -32768,105,315,20,1185,- 356,- 32};static short Cyc_yytable[
6137]={56,536,590,241,240,475,476,318,478,336,75,300,428,59,140,538,321,541,442,
539,556,56,256,302,299,560,322,257,251,542,543,559,137,642,59,409,240,568,608,
548,549,612,108,400,125,126,127,311,128,56,73,221,222,422,139,56,56,56,229,56,
550,551,59,529,683,56,144,329,59,59,59,410,59,252,799,800,407,160,59,37,344,95,
131,411,645,57,607,290,109,36,639,137,161,306,883,258,39,431,146,608,124,56,56,
278,657,289,57,392,42,398,607,403,101,139,59,59,56,56,56,56,690,345,632,102,56,
279,487,139,92,59,59,59,59,423,57,884,56,59,56,382,57,57,57,841,57,144,508,256,
509,59,57,59,56,503,237,409,105,658,544,545,546,547,527,528,593,59,260,383,37,
496,585,376,377,378,488,96,91,768,- 147,594,305,352,353,470,92,291,57,57,481,36,
413,410,471,92,585,409,39,419,502,601,445,57,57,57,57,97,37,721,458,57,875,459,
379,313,380,354,355,399,289,381,601,57,303,57,342,400,400,400,418,314,401,410,
159,98,679,36,480,57,103,342,139,208,39,411,278,278,396,278,99,406,100,56,36,150,
891,892,116,117,648,39,433,482,617,76,59,92,279,279,298,279,430,495,727,342,500,
342,433,56,56,562,56,934,104,923,661,984,454,939,893,894,59,59,503,59,342,291,
485,107,240,754,662,343,278,512,499,759,298,339,340,341,513,92,342,312,37,71,-
205,106,92,- 205,240,395,985,360,279,477,212,240,375,489,111,361,362,568,213,56,
57,77,93,689,94,566,110,983,342,694,483,411,59,574,616,618,36,848,342,503,670,71,
38,39,40,421,57,57,71,57,241,240,232,71,112,448,712,595,233,243,899,130,722,240,
150,635,71,712,900,901,152,462,240,845,1000,156,153,469,93,434,160,411,157,399,
399,399,435,93,1005,295,- 227,629,589,- 227,342,161,401,401,401,342,71,71,759,206,
57,358,531,71,186,359,636,600,758,606,626,71,71,71,71,406,637,897,56,37,56,898,
489,207,137,231,641,602,611,209,433,59,139,59,71,210,700,430,503,232,753,211,782,
56,115,233,139,71,433,699,118,119,156,718,433,93,59,56,454,737,795,240,342,225,
342,719,717,552,553,554,59,664,342,398,92,342,342,92,240,669,223,56,671,725,682,
844,759,743,226,645,847,649,230,444,59,748,227,228,337,396,764,93,770,57,489,57,
238,855,93,696,765,244,245,246,247,835,638,342,342,236,940,758,565,342,726,569,
714,342,57,248,731,391,680,342,942,432,945,904,943,944,741,249,57,705,71,342,786,
307,946,947,308,432,783,787,456,342,309,460,952,953,133,134,856,444,742,589,57,
520,521,522,523,342,147,148,71,744,634,342,859,954,955,532,289,310,905,535,396,
606,346,468,938,41,404,838,405,256,342,141,798,142,602,143,347,156,139,997,342,
350,351,758,454,433,691,907,433,56,317,555,323,36,880,454,240,532,454,38,39,40,
59,342,771,849,36,56,342,337,911,676,38,39,40,963,649,519,778,324,59,581,524,92,
526,448,337,92,489,449,342,45,46,47,325,972,791,966,36,537,326,36,924,342,38,39,
40,38,39,40,968,948,949,950,951,404,78,427,93,327,865,93,935,342,936,342,316,999,
319,319,919,685,990,557,57,686,434,328,872,333,334,873,349,995,319,319,319,41,
532,356,357,532,57,586,330,587,432,588,776,432,79,36,331,779,338,319,348,71,39,
71,41,384,567,728,729,158,81,432,82,83,42,792,39,432,389,131,45,46,47,45,46,47,
512,444,84,41,85,86,390,513,87,603,394,604,414,605,88,92,232,996,278,619,620,415,
233,623,36,627,16,17,18,993,38,39,40,916,633,129,342,416,71,921,279,420,922,978,
240,659,660,444,404,998,405,56,772,773,862,92,45,46,47,36,707,708,709,569,59,417,
39,867,426,120,804,278,698,36,436,609,831,610,42,38,39,40,319,437,45,46,47,799,
800,716,404,455,427,279,577,578,579,439,45,46,47,93,446,827,56,93,532,860,861,
887,888,889,890,78,895,896,319,59,319,319,319,319,319,319,319,319,319,319,319,
319,319,319,319,319,319,457,57,432,652,991,342,930,931,461,463,444,464,465,432,
466,472,432,762,701,723,473,479,79,411,881,444,474,250,769,131,484,485,41,493,
385,492,489,158,81,278,82,83,903,906,39,497,910,501,505,506,507,344,734,736,57,
71,84,525,85,86,530,279,87,561,564,563,78,573,88,575,576,591,56,363,364,365,366,
367,368,369,370,371,372,592,932,59,319,596,597,599,93,598,760,614,613,763,615,
621,624,622,373,628,630,643,640,655,656,665,666,444,79,667,36,956,957,958,646,
961,962,39,654,964,965,444,967,80,81,668,82,83,93,42,39,785,674,672,673,45,46,47,
677,678,687,681,84,688,85,86,37,319,87,982,692,130,702,752,88,57,693,832,834,695,
837,703,840,704,706,319,710,713,724,740,738,986,732,745,987,739,988,989,298,755,
854,7,8,9,10,11,12,13,14,15,16,17,18,19,20,747,751,36,1001,675,756,761,871,38,39,
40,41,766,780,444,22,767,36,777,781,784,448,842,38,39,40,788,45,46,47,789,790,
793,794,27,28,448,805,796,186,806,830,45,46,47,32,33,829,843,846,35,852,853,857,
858,319,863,864,869,866,868,870,874,876,879,909,882,885,913,915,902,886,296,917,
920,36,918,925,927,928,37,38,39,40,41,298,929,960,933,937,975,976,404,977,427,-
205,43,44,- 205,979,45,46,47,980,981,1008,992,337,994,319,1002,1009,711,1003,941,
1004,1006,239,973,319,797,145,447,441,720,114,971,130,746,749,750,663,959,540,
775,393,828,132,0,0,0,0,0,0,0,0,970,0,0,0,0,0,0,974,0,0,0,2,3,4,74,6,7,8,9,10,11,
12,13,14,15,16,17,18,19,20,728,729,21,162,163,261,0,262,263,264,265,266,267,268,
269,22,78,23,164,270,0,0,165,24,271,0,0,0,0,166,167,168,169,170,27,28,29,171,0,
272,172,0,0,0,0,32,33,34,173,174,35,0,175,176,0,0,0,0,78,0,0,0,0,0,0,0,0,0,0,0,0,
878,0,0,319,273,81,177,82,83,37,38,39,40,41,274,136,- 293,0,0,0,0,319,0,178,0,43,
276,0,0,180,0,79,181,0,0,182,183,0,0,184,185,0,0,774,0,0,158,81,0,82,83,0,0,39,0,
0,0,0,0,0,0,0,0,512,0,84,0,85,86,0,513,87,0,0,0,0,319,88,319,319,319,319,319,319,
319,319,319,319,319,319,319,319,319,319,319,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,319,0,2,3,4,74,6,7,8,9,10,11,12,13,14,15,16,17,
18,19,20,0,0,21,162,163,261,0,262,263,264,265,266,267,268,269,22,78,23,164,270,0,
0,165,24,271,0,319,- 143,- 143,166,167,168,169,170,27,28,29,171,0,272,172,0,0,0,0,
32,33,34,173,174,35,0,175,176,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,273,81,
177,82,83,37,38,39,40,41,274,136,0,0,0,0,0,0,0,178,0,43,276,0,0,180,0,0,181,0,0,
182,183,0,0,184,185,2,3,4,74,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,162,
163,261,0,262,263,264,265,266,267,268,269,22,78,23,164,270,0,0,165,24,271,0,0,0,
0,166,167,168,169,170,27,28,29,171,0,272,172,0,0,0,0,32,33,34,173,174,35,0,175,
176,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,273,81,177,82,83,37,38,39,40,41,
274,136,275,0,0,0,0,0,0,178,0,43,276,0,0,180,0,0,181,0,0,182,183,0,0,184,185,2,3,
4,74,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,162,163,261,0,262,263,264,
265,266,267,268,269,22,78,23,164,270,0,0,165,24,271,0,0,0,0,166,167,168,169,170,
27,28,29,171,0,272,172,0,0,0,0,32,33,34,173,174,35,0,175,176,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,273,81,177,82,83,37,38,39,40,41,274,136,0,0,0,0,0,0,0,178,
0,43,276,0,0,180,0,0,181,0,0,182,183,0,0,184,185,2,3,4,74,6,7,8,9,10,11,12,13,14,
15,16,17,18,19,20,0,0,21,162,163,0,0,0,0,0,0,0,0,0,0,22,78,23,164,0,0,0,165,24,0,
0,0,0,0,166,167,168,169,170,27,28,29,171,0,0,172,0,0,0,0,32,33,34,173,174,35,0,
175,176,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,36,81,177,82,83,37,38,39,40,41,
625,0,0,0,0,0,0,0,0,178,0,43,276,0,0,180,0,0,181,0,0,182,183,0,0,184,185,7,8,9,
10,11,12,13,14,15,16,17,18,19,20,0,0,0,162,163,0,0,0,0,0,0,0,0,0,0,22,78,0,164,0,
0,0,165,0,0,0,0,0,0,166,167,168,169,170,27,28,0,171,0,0,172,0,0,0,0,32,33,0,173,
174,35,0,175,176,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,36,81,177,82,83,37,38,
39,40,41,0,335,0,0,0,0,0,0,0,178,0,43,276,0,0,180,0,0,181,0,0,182,183,0,0,184,
185,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,162,163,0,0,0,0,0,0,0,0,0,0,22,
78,0,164,0,0,0,165,0,0,0,0,0,0,166,167,168,169,170,27,28,0,171,0,0,172,0,0,0,0,
32,33,0,173,174,35,0,175,176,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,36,81,177,
82,83,37,38,39,40,41,0,0,0,0,0,0,0,0,0,178,0,43,276,0,0,180,0,0,181,0,0,182,183,
0,0,184,185,- 11,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,
0,0,0,78,0,0,0,0,0,0,22,0,23,0,0,0,651,0,24,0,25,26,- 143,- 143,0,0,0,0,0,27,28,29,
0,0,0,0,0,0,30,31,32,33,34,0,0,35,0,79,0,0,0,0,0,0,0,0,0,0,0,684,0,0,158,81,0,82,
83,0,0,39,36,0,0,0,0,37,38,39,40,41,0,84,- 11,85,86,0,0,87,0,42,0,43,44,88,0,45,
46,47,- 11,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,
78,0,0,0,0,0,0,22,0,23,0,0,0,0,0,24,0,25,26,- 143,- 143,0,0,0,0,0,27,28,29,0,0,0,0,
0,0,30,31,32,33,34,0,0,35,0,79,0,0,0,0,0,0,0,0,0,0,0,0,0,0,158,81,0,82,83,0,0,39,
36,0,0,0,0,37,38,39,40,41,0,84,- 11,85,86,0,0,87,0,42,0,43,44,88,0,45,46,47,- 11,1,
0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,
0,22,0,23,0,0,0,0,0,24,0,25,26,- 143,- 143,0,0,0,0,0,27,28,29,0,0,0,0,0,0,30,31,32,
33,34,0,0,35,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,36,0,0,0,0,37,38,39,
40,41,0,0,0,0,0,0,0,0,0,42,0,43,44,0,0,45,46,47,1,0,2,3,4,5,6,7,8,9,10,11,12,13,
14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,0,23,0,0,0,0,0,24,0,25,26,
- 143,- 143,0,0,0,0,0,27,28,29,0,0,0,0,0,0,30,31,32,33,34,0,0,35,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,36,0,0,0,0,37,38,39,40,41,0,0,- 11,0,0,0,0,0,0,42,0,
43,44,0,0,45,46,47,2,3,4,74,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,
0,0,0,0,0,0,0,0,0,0,22,0,23,0,0,0,0,0,24,0,0,0,- 143,- 143,0,0,0,0,0,27,28,29,0,0,
0,0,0,0,0,0,32,33,34,0,0,35,2,3,4,74,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,
0,21,0,0,0,0,0,0,0,37,38,0,40,41,22,136,23,0,250,0,0,0,24,0,0,43,44,0,0,0,0,0,0,
27,28,29,0,0,0,0,0,0,0,0,32,33,34,0,0,35,0,0,2,3,4,74,6,7,8,9,10,11,12,13,14,15,
16,17,18,19,20,0,0,21,0,0,0,0,0,37,38,0,40,41,0,136,22,0,0,0,0,0,0,0,24,43,44,0,
- 143,- 143,0,0,0,0,0,27,28,0,0,0,0,0,0,0,0,0,32,33,0,0,0,35,0,0,7,8,9,10,11,12,13,
14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,37,38,22,40,41,0,0,0,0,0,0,0,0,0,
0,0,43,44,0,0,0,27,28,0,0,0,0,0,0,0,0,0,32,33,0,0,0,35,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,296,0,0,0,0,0,0,0,37,38,0,40,41,298,0,0,0,0,0,0,404,0,405,- 205,
43,44,- 205,0,45,46,47,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,22,0,0,0,0,0,0,0,0,0,0,0,- 143,- 143,0,0,0,0,0,27,28,0,0,0,0,0,0,0,0,0,
32,33,0,0,0,35,0,0,0,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,36,0,0,
0,0,37,38,39,40,41,22,0,0,0,0,0,0,0,0,448,0,43,44,0,0,45,46,47,0,27,28,0,0,0,0,0,
0,0,0,0,32,33,0,0,0,35,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,296,
0,0,297,0,0,0,0,37,38,22,40,41,298,0,0,0,0,0,0,0,0,0,- 205,43,44,- 205,0,0,27,28,0,
0,0,0,0,0,0,0,0,32,33,0,0,0,35,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,
0,0,296,0,0,0,0,0,0,0,37,38,22,40,41,298,0,0,0,0,0,0,0,0,0,- 205,43,44,- 205,0,0,
27,28,0,0,214,0,0,0,0,0,0,32,33,0,0,0,35,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,
20,0,0,0,0,0,0,0,0,0,0,0,0,0,37,38,22,40,41,0,215,0,0,0,0,0,0,0,- 143,- 143,43,44,
0,0,0,27,28,0,0,0,0,0,0,0,0,0,32,33,0,0,0,35,0,0,7,8,9,10,11,12,13,14,15,16,17,
18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,37,38,22,40,41,0,0,0,0,0,0,0,0,0,0,0,43,44,0,
0,0,27,28,0,0,0,0,0,0,0,0,0,32,33,0,0,0,35,0,0,7,8,9,10,11,12,13,14,15,16,17,18,
19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,37,38,22,40,41,0,0,- 111,0,0,0,0,0,0,0,0,43,44,0,
0,0,27,28,0,0,0,0,0,0,0,0,0,32,33,0,0,0,35,0,0,7,8,9,10,11,12,13,14,15,16,17,18,
19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,37,38,22,40,41,0,0,- 112,0,0,0,0,0,0,0,0,43,44,0,
0,0,27,28,0,0,0,0,0,0,0,0,0,32,33,0,0,0,35,0,0,7,8,9,10,11,12,13,14,15,16,17,18,
19,20,0,0,0,0,0,647,0,0,0,0,0,0,0,37,38,22,40,41,0,0,0,0,0,0,0,0,0,0,0,43,44,807,
808,0,27,28,0,0,0,0,0,0,0,0,78,32,33,0,0,0,35,0,0,7,8,9,10,11,12,13,14,15,16,17,
18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,37,38,22,40,41,0,0,0,79,0,0,0,0,0,- 143,- 143,
43,44,0,0,0,27,28,158,81,0,82,83,0,0,39,0,32,33,0,0,0,35,0,0,0,0,809,0,85,86,0,0,
87,0,0,810,0,0,88,183,0,0,184,185,0,0,0,0,0,0,0,37,0,0,0,41,0,0,0,0,0,0,0,0,0,0,
0,43,44,162,163,261,0,262,263,264,265,266,267,268,269,0,78,0,164,270,0,0,165,0,
271,0,0,0,0,166,167,168,169,170,0,0,0,171,0,272,172,0,0,0,0,0,0,0,173,174,0,0,
175,176,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,273,81,177,82,83,0,0,39,0,0,
274,136,0,0,162,163,0,0,0,178,0,510,179,0,0,180,0,78,181,164,0,182,183,165,0,184,
185,0,0,0,166,167,168,169,170,0,0,0,171,0,0,172,0,0,0,0,0,0,0,173,174,0,0,175,
176,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,36,81,177,82,83,0,0,39,0,0,0,320,
511,0,162,163,0,512,0,178,0,0,179,0,513,180,0,78,181,164,0,182,183,165,0,184,185,
0,0,0,166,167,168,169,170,0,0,0,171,0,0,172,0,0,0,0,0,0,0,173,174,0,0,175,176,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,36,81,177,82,83,0,0,39,0,0,0,320,757,0,
162,163,0,512,0,178,0,0,179,0,513,180,0,78,181,164,0,182,183,165,0,184,185,0,0,0,
166,167,168,169,170,0,0,0,171,0,0,172,0,0,0,0,0,0,0,173,174,0,0,175,176,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,36,81,177,82,83,0,0,39,0,0,0,320,926,0,162,163,
0,512,0,178,0,0,179,0,513,180,0,78,181,164,0,182,183,165,0,184,185,0,0,0,166,167,
168,169,170,0,0,0,171,0,0,172,0,0,0,0,0,0,0,173,174,0,0,175,176,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,36,81,177,82,83,0,0,39,0,0,0,320,0,0,162,163,0,512,0,
178,0,0,179,0,513,180,0,78,181,164,0,182,183,165,0,184,185,0,0,0,166,167,168,169,
170,0,0,0,171,0,0,172,0,0,0,0,0,0,0,173,174,0,0,175,176,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,36,81,177,82,83,0,0,39,0,0,0,162,163,0,0,0,0,0,294,178,0,0,179,
0,78,180,164,0,181,0,165,182,183,0,0,184,185,166,167,168,169,170,0,0,0,171,0,0,
172,0,0,0,0,0,0,0,173,174,0,0,175,176,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
36,81,177,82,83,0,0,39,0,0,0,320,0,0,162,163,0,0,0,178,0,0,179,0,0,180,0,78,181,
164,0,182,183,165,0,184,185,0,0,0,166,167,168,169,170,0,0,0,171,0,0,172,0,0,0,0,
0,0,0,173,174,0,0,175,176,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,36,81,177,82,
83,0,0,39,0,0,467,162,163,0,0,0,0,0,0,178,0,0,179,0,78,180,164,0,181,0,165,182,
183,0,0,184,185,166,167,168,169,170,0,0,0,171,0,0,172,0,0,0,0,0,0,0,173,174,0,0,
175,176,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,36,81,177,82,83,0,0,39,0,0,0,
162,163,0,0,0,0,0,0,178,558,0,179,0,78,180,164,0,181,0,165,182,183,0,0,184,185,
166,167,168,169,170,0,0,0,171,0,0,172,0,0,0,0,0,0,0,173,174,0,0,175,176,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,36,81,177,82,83,0,0,39,0,0,0,162,163,0,0,0,0,0,
580,178,0,0,179,0,78,180,164,0,181,0,165,182,183,0,0,184,185,166,167,168,169,170,
0,0,0,171,0,0,172,0,0,0,0,0,0,0,173,174,0,0,175,176,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,36,81,177,82,83,0,0,39,0,0,0,162,163,0,0,0,0,0,0,178,631,0,179,0,
78,180,164,0,181,0,165,182,183,0,0,184,185,166,167,168,169,170,0,0,0,171,0,0,172,
0,0,0,0,0,0,0,173,174,0,0,175,176,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,36,
81,177,82,83,0,0,39,0,0,0,162,163,0,0,0,0,0,697,178,0,0,179,0,78,180,164,0,181,0,
165,182,183,0,0,184,185,166,167,168,169,170,0,0,0,171,0,0,172,0,0,0,0,0,0,0,173,
174,0,0,175,176,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,36,81,177,82,83,0,0,39,
0,0,0,162,163,0,0,0,0,0,715,178,0,0,179,0,78,180,164,0,181,0,165,182,183,0,0,184,
185,166,167,168,169,170,0,0,0,171,0,0,172,0,0,0,0,0,0,0,173,174,0,0,175,176,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,36,81,177,82,83,0,0,39,0,0,733,162,163,0,0,
0,0,0,0,178,0,0,179,0,78,180,164,0,181,0,165,182,183,0,0,184,185,166,167,168,169,
170,0,0,0,171,0,0,172,0,0,0,0,0,0,0,173,174,0,0,175,176,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,36,81,177,82,83,0,0,39,0,0,735,162,163,0,0,0,0,0,0,178,0,0,179,
0,78,180,164,0,181,0,165,182,183,0,0,184,185,166,167,168,169,170,0,0,0,171,0,0,
172,0,0,0,0,0,0,0,173,174,0,0,175,176,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
36,81,177,82,83,0,0,39,0,0,0,162,163,0,0,0,0,0,0,178,833,0,179,0,78,180,164,0,
181,0,165,182,183,0,0,184,185,166,167,168,169,170,0,0,0,171,0,0,172,0,0,0,0,0,0,
0,173,174,0,0,175,176,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,36,81,177,82,83,
0,0,39,0,0,0,162,163,0,0,0,0,0,0,178,836,0,179,0,78,180,164,0,181,0,165,182,183,
0,0,184,185,166,167,168,169,170,0,0,0,171,0,0,172,0,0,0,0,0,0,0,173,174,0,0,175,
176,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,36,81,177,82,83,0,0,39,0,0,839,162,
163,0,0,0,0,0,0,178,0,0,179,0,78,180,164,0,181,0,165,182,183,0,0,184,185,166,167,
168,169,170,0,0,0,171,0,0,172,0,0,0,0,0,0,0,173,174,0,0,175,176,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,36,81,177,82,83,0,0,39,0,0,0,162,163,0,0,0,0,0,0,178,
908,0,179,0,78,180,164,0,181,0,165,182,183,0,0,184,185,166,167,168,169,170,0,0,0,
171,0,0,172,0,0,0,0,0,0,0,173,174,0,0,175,176,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,36,81,177,82,83,0,0,39,0,0,0,162,163,0,0,0,0,0,0,178,912,0,179,0,78,180,
164,0,181,0,165,182,183,0,0,184,185,166,167,168,169,170,0,0,0,171,0,0,172,0,0,0,
0,0,0,0,173,174,0,0,175,176,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,36,81,177,
82,83,0,0,39,0,0,0,162,163,0,0,0,0,0,0,178,914,0,179,0,78,180,164,0,181,0,165,
182,183,0,0,184,185,166,167,168,169,170,0,0,0,171,0,0,172,0,0,0,0,0,0,0,173,174,
0,0,175,176,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,36,81,177,82,83,0,0,39,0,0,
0,162,163,0,0,0,0,0,0,178,969,0,179,0,78,180,164,0,181,0,165,182,183,0,0,184,185,
166,167,168,169,170,0,0,0,171,0,0,172,0,0,162,163,0,0,0,173,174,0,0,175,176,0,0,
78,0,0,0,0,0,0,0,0,0,0,0,0,166,167,168,169,170,0,36,81,177,82,83,172,0,39,0,0,0,
0,0,173,174,0,0,175,176,178,0,0,179,0,0,180,0,0,181,0,0,182,183,0,0,184,185,0,0,
0,36,81,177,82,83,0,0,39,0,0,0,320,0,162,163,0,0,0,0,178,0,0,179,0,0,180,78,0,
181,0,0,182,183,0,0,184,185,0,0,166,167,168,169,170,0,0,0,0,0,0,172,0,0,162,163,
0,0,0,173,174,0,0,175,176,0,0,78,0,0,0,0,0,0,0,0,0,0,0,0,166,167,168,169,170,0,
36,81,177,82,83,172,0,39,0,0,0,675,0,173,174,0,0,175,176,178,0,0,179,0,0,180,0,0,
181,0,0,182,183,0,0,184,185,0,0,0,36,81,177,82,83,0,0,39,0,0,0,162,163,0,0,0,0,0,
0,315,0,0,179,0,78,180,0,0,181,0,0,182,183,0,0,184,185,166,167,168,169,170,0,0,0,
0,0,0,172,0,0,162,163,0,0,0,173,174,0,0,175,176,0,0,78,0,0,0,0,0,0,0,0,0,0,0,0,
166,167,168,169,170,0,36,81,177,82,83,172,0,39,0,0,0,0,0,173,174,0,0,175,176,332,
0,0,179,0,0,180,0,0,181,0,0,182,183,0,0,184,185,0,0,0,36,81,177,82,83,0,0,39,0,0,
0,162,163,0,0,0,0,0,0,178,0,0,179,0,78,180,0,0,181,0,0,182,183,0,0,184,185,166,
167,168,169,170,0,0,0,0,0,0,172,0,0,0,0,0,0,0,173,174,0,0,175,176,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,36,81,177,82,83,0,0,39,0,0,0,0,0,0,0,0,0,0,0,877,0,0,
179,0,0,180,0,0,181,0,0,182,183,0,0,184,185};static short Cyc_yycheck[6137]={0,344,
411,111,111,277,278,164,280,178,21,143,240,0,65,346,165,349,250,347,376,21,129,
143,143,381,165,132,124,350,351,380,65,488,21,77,143,392,434,356,357,449,42,217,
55,56,57,157,59,49,99,96,97,17,65,55,56,57,103,59,358,359,49,335,562,65,66,172,
55,56,57,113,59,124,5,6,219,97,65,102,83,97,62,125,493,0,430,138,42,97,486,124,
112,150,83,118,104,240,68,495,54,101,102,136,512,138,21,212,116,216,454,218,107,
124,101,102,116,117,118,119,575,124,471,107,124,136,60,138,23,116,117,118,119,97,
49,124,136,124,138,95,55,56,57,738,59,145,315,254,317,136,65,138,152,310,108,77,
116,513,352,353,354,355,331,332,97,152,135,123,102,301,406,73,74,75,108,112,23,
675,109,112,149,78,79,107,79,139,101,102,110,97,222,113,116,88,430,77,104,229,
308,427,251,116,117,118,119,112,102,615,116,124,805,119,114,102,116,112,113,217,
251,121,448,136,118,138,110,399,400,401,110,118,217,113,79,97,120,97,107,152,112,
110,251,88,104,125,277,278,215,280,112,219,114,251,97,112,78,79,107,108,496,104,
240,293,121,108,251,160,277,278,107,280,240,110,621,110,306,110,256,277,278,384,
280,876,112,120,516,120,256,882,112,113,277,278,449,280,110,249,97,116,405,655,
111,117,335,114,304,660,107,180,181,182,121,206,110,160,102,0,117,96,213,120,427,
109,120,122,335,279,108,434,201,298,116,130,131,688,116,335,251,22,23,117,25,107,
96,938,110,583,294,125,335,394,455,456,97,749,110,512,525,42,103,104,105,117,277,
278,49,280,474,474,97,54,108,116,600,418,103,112,122,62,616,486,112,477,67,611,
130,131,108,264,495,117,985,76,99,270,79,110,97,125,108,399,400,401,117,88,999,
142,107,107,408,110,110,112,399,400,401,110,101,102,773,116,335,125,117,108,84,
129,477,427,660,429,463,116,117,118,119,405,481,125,438,102,440,129,411,97,477,
105,109,427,448,107,430,438,463,440,139,110,588,427,615,97,98,111,700,463,49,103,
477,152,448,588,107,108,157,605,454,160,463,477,448,107,718,588,110,102,110,605,
605,360,361,362,477,117,110,600,389,110,110,392,605,117,111,501,117,117,561,743,
861,639,97,918,748,498,102,250,501,647,101,102,178,488,669,206,679,438,493,440,
117,759,213,586,669,116,117,118,119,107,484,110,110,116,883,773,389,110,117,392,
603,110,463,107,117,211,115,110,885,240,888,829,886,887,117,110,477,595,249,110,
110,152,889,890,110,256,117,117,259,110,109,262,895,896,49,50,117,320,635,585,
501,324,325,326,327,110,63,64,279,640,473,110,117,897,898,338,635,111,117,342,
575,607,82,269,882,106,114,107,116,722,110,112,723,114,600,116,126,308,635,981,
110,80,81,861,600,611,580,117,614,635,116,374,116,97,809,611,749,380,614,103,104,
105,635,110,682,109,97,653,110,315,117,530,103,104,105,117,649,323,696,116,653,
404,328,564,330,116,332,568,643,120,110,122,123,124,116,919,714,117,97,345,116,
97,857,110,103,104,105,103,104,105,117,891,892,893,894,114,38,116,389,116,107,
392,877,110,879,110,162,985,164,165,848,564,117,379,635,568,110,116,107,175,176,
110,128,117,180,181,182,106,471,76,77,474,653,112,116,114,427,116,692,430,82,97,
116,697,116,201,127,438,104,440,106,108,94,22,23,97,98,448,100,101,116,715,104,
454,97,751,122,123,124,122,123,124,114,516,116,106,118,119,97,121,122,112,116,
114,97,116,128,688,97,98,829,457,458,107,103,461,97,463,17,18,19,976,103,104,105,
107,472,108,110,113,501,107,829,113,110,929,929,109,110,562,114,984,116,829,109,
110,780,728,122,123,124,97,97,98,99,688,829,115,104,793,117,107,725,882,587,97,
109,438,731,440,116,103,104,105,310,109,122,123,124,5,6,604,114,108,116,882,399,
400,401,109,122,123,124,564,109,728,882,568,621,109,110,127,128,80,81,38,76,77,
344,882,346,347,348,349,350,351,352,353,354,355,356,357,358,359,360,361,362,116,
829,600,501,109,110,872,873,116,116,660,97,107,611,107,116,614,667,591,617,120,
109,82,125,810,675,116,111,678,922,115,97,106,120,94,117,918,97,98,985,100,101,
828,833,104,117,836,109,109,113,113,83,625,626,882,653,116,116,118,119,117,985,
122,112,110,117,38,117,128,110,109,116,985,84,85,86,87,88,89,90,91,92,93,97,874,
985,449,97,116,110,688,117,665,110,107,668,120,116,30,108,111,107,42,125,116,110,
97,110,110,759,82,110,97,899,900,901,117,907,908,104,117,911,912,773,914,97,98,
110,100,101,728,116,104,706,109,117,117,122,123,124,117,110,109,117,116,110,118,
119,102,512,122,937,115,751,107,653,128,985,117,732,733,117,735,107,737,102,111,
530,117,117,108,117,108,963,116,109,966,107,968,969,107,112,755,8,9,10,11,12,13,
14,15,16,17,18,19,20,21,113,108,97,990,108,115,25,802,103,104,105,106,117,115,
861,37,117,97,117,117,113,116,107,103,104,105,117,122,123,124,117,117,115,117,56,
57,116,108,117,809,117,109,122,123,124,67,68,120,107,102,72,117,117,116,110,615,
117,97,109,98,117,109,28,108,116,835,120,82,838,839,82,126,94,109,109,97,110,25,
107,110,102,103,104,105,106,107,116,107,109,117,117,116,114,98,116,117,118,119,
120,109,122,123,124,117,110,0,117,877,117,669,117,0,599,117,884,117,117,110,922,
679,722,67,254,249,614,46,918,922,643,647,649,517,902,348,688,213,728,62,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,916,- 1,- 1,- 1,- 1,- 1,- 1,923,- 1,- 1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,
15,16,17,18,19,20,21,22,23,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,
- 1,- 1,44,45,46,- 1,- 1,- 1,- 1,51,52,53,54,55,56,57,58,59,- 1,61,62,- 1,- 1,- 1,- 1,67,68,
69,70,71,72,- 1,74,75,- 1,- 1,- 1,- 1,38,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,807,- 1,-
1,810,97,98,99,100,101,102,103,104,105,106,107,108,109,- 1,- 1,- 1,- 1,828,- 1,116,- 1,
118,119,- 1,- 1,122,- 1,82,125,- 1,- 1,128,129,- 1,- 1,132,133,- 1,- 1,94,- 1,- 1,97,98,- 1,
100,101,- 1,- 1,104,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,114,- 1,116,- 1,118,119,- 1,121,122,- 1,
- 1,- 1,- 1,883,128,885,886,887,888,889,890,891,892,893,894,895,896,897,898,899,900,
901,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,937,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,
20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,- 1,- 1,44,45,46,
- 1,984,49,50,51,52,53,54,55,56,57,58,59,- 1,61,62,- 1,- 1,- 1,- 1,67,68,69,70,71,72,-
1,74,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,97,98,99,
100,101,102,103,104,105,106,107,108,- 1,- 1,- 1,- 1,- 1,- 1,- 1,116,- 1,118,119,- 1,- 1,
122,- 1,- 1,125,- 1,- 1,128,129,- 1,- 1,132,133,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,
18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,- 1,- 1,44,
45,46,- 1,- 1,- 1,- 1,51,52,53,54,55,56,57,58,59,- 1,61,62,- 1,- 1,- 1,- 1,67,68,69,70,71,
72,- 1,74,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,97,98,
99,100,101,102,103,104,105,106,107,108,109,- 1,- 1,- 1,- 1,- 1,- 1,116,- 1,118,119,- 1,-
1,122,- 1,- 1,125,- 1,- 1,128,129,- 1,- 1,132,133,3,4,5,6,7,8,9,10,11,12,13,14,15,16,
17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,- 1,- 1,
44,45,46,- 1,- 1,- 1,- 1,51,52,53,54,55,56,57,58,59,- 1,61,62,- 1,- 1,- 1,- 1,67,68,69,70,
71,72,- 1,74,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,97,
98,99,100,101,102,103,104,105,106,107,108,- 1,- 1,- 1,- 1,- 1,- 1,- 1,116,- 1,118,119,- 1,
- 1,122,- 1,- 1,125,- 1,- 1,128,129,- 1,- 1,132,133,3,4,5,6,7,8,9,10,11,12,13,14,15,16,
17,18,19,20,21,- 1,- 1,24,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,- 1,- 1,- 1,
44,45,- 1,- 1,- 1,- 1,- 1,51,52,53,54,55,56,57,58,59,- 1,- 1,62,- 1,- 1,- 1,- 1,67,68,69,70,
71,72,- 1,74,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,97,
98,99,100,101,102,103,104,105,106,107,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,116,- 1,118,119,- 1,
- 1,122,- 1,- 1,125,- 1,- 1,128,129,- 1,- 1,132,133,8,9,10,11,12,13,14,15,16,17,18,19,
20,21,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,
- 1,- 1,- 1,- 1,51,52,53,54,55,56,57,- 1,59,- 1,- 1,62,- 1,- 1,- 1,- 1,67,68,- 1,70,71,72,- 1,
74,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,97,98,99,
100,101,102,103,104,105,106,- 1,108,- 1,- 1,- 1,- 1,- 1,- 1,- 1,116,- 1,118,119,- 1,- 1,122,
- 1,- 1,125,- 1,- 1,128,129,- 1,- 1,132,133,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,
- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,- 1,
- 1,51,52,53,54,55,56,57,- 1,59,- 1,- 1,62,- 1,- 1,- 1,- 1,67,68,- 1,70,71,72,- 1,74,75,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,97,98,99,100,101,102,
103,104,105,106,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,116,- 1,118,119,- 1,- 1,122,- 1,- 1,125,- 1,
- 1,128,129,- 1,- 1,132,133,0,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,
21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,38,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,39,- 1,- 1,- 1,43,- 1,45,- 1,47,
48,49,50,- 1,- 1,- 1,- 1,- 1,56,57,58,- 1,- 1,- 1,- 1,- 1,- 1,65,66,67,68,69,- 1,- 1,72,- 1,82,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,94,- 1,- 1,97,98,- 1,100,101,- 1,- 1,104,97,- 1,- 1,- 1,
- 1,102,103,104,105,106,- 1,116,109,118,119,- 1,- 1,122,- 1,116,- 1,118,119,128,- 1,122,
123,124,0,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,
- 1,- 1,- 1,38,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,39,- 1,- 1,- 1,- 1,- 1,45,- 1,47,48,49,50,- 1,- 1,- 1,
- 1,- 1,56,57,58,- 1,- 1,- 1,- 1,- 1,- 1,65,66,67,68,69,- 1,- 1,72,- 1,82,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,97,98,- 1,100,101,- 1,- 1,104,97,- 1,- 1,- 1,- 1,102,103,104,
105,106,- 1,116,109,118,119,- 1,- 1,122,- 1,116,- 1,118,119,128,- 1,122,123,124,0,1,- 1,
3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,37,- 1,39,- 1,- 1,- 1,- 1,- 1,45,- 1,47,48,49,50,- 1,- 1,- 1,- 1,- 1,56,57,58,
- 1,- 1,- 1,- 1,- 1,- 1,65,66,67,68,69,- 1,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,97,- 1,- 1,- 1,- 1,102,103,104,105,106,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,116,- 1,118,119,- 1,- 1,122,123,124,1,- 1,3,4,5,6,7,8,9,10,11,12,13,
14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,39,- 1,
- 1,- 1,- 1,- 1,45,- 1,47,48,49,50,- 1,- 1,- 1,- 1,- 1,56,57,58,- 1,- 1,- 1,- 1,- 1,- 1,65,66,67,
68,69,- 1,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,97,- 1,- 1,- 1,- 1,102,103,104,105,106,- 1,- 1,109,- 1,- 1,- 1,- 1,- 1,- 1,116,- 1,118,
119,- 1,- 1,122,123,124,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,39,- 1,- 1,- 1,- 1,- 1,45,- 1,- 1,- 1,49,50,- 1,
- 1,- 1,- 1,- 1,56,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,67,68,69,- 1,- 1,72,3,4,5,6,7,8,9,10,
11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,102,103,- 1,105,
106,37,108,39,- 1,111,- 1,- 1,- 1,45,- 1,- 1,118,119,- 1,- 1,- 1,- 1,- 1,- 1,56,57,58,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,67,68,69,- 1,- 1,72,- 1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,
18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,102,103,- 1,105,106,- 1,108,37,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,45,118,119,- 1,49,50,- 1,- 1,- 1,- 1,- 1,56,57,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,67,68,
- 1,- 1,- 1,72,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,102,103,37,105,106,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,118,119,- 1,
- 1,- 1,56,57,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,67,68,- 1,- 1,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,94,- 1,- 1,- 1,- 1,- 1,- 1,- 1,102,103,- 1,105,
106,107,- 1,- 1,- 1,- 1,- 1,- 1,114,- 1,116,117,118,119,120,- 1,122,123,124,8,9,10,11,12,
13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,49,50,- 1,- 1,- 1,- 1,- 1,56,57,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
67,68,- 1,- 1,- 1,72,- 1,- 1,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,-
1,- 1,- 1,97,- 1,- 1,- 1,- 1,102,103,104,105,106,37,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,116,- 1,118,
119,- 1,- 1,122,123,124,- 1,56,57,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,67,68,- 1,- 1,- 1,72,- 1,-
1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,94,- 1,- 1,97,- 1,- 1,- 1,- 1,
102,103,37,105,106,107,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,117,118,119,120,- 1,- 1,56,57,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,67,68,- 1,- 1,- 1,72,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,
19,20,21,- 1,- 1,- 1,- 1,- 1,94,- 1,- 1,- 1,- 1,- 1,- 1,- 1,102,103,37,105,106,107,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,117,118,119,120,- 1,- 1,56,57,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,67,68,-
1,- 1,- 1,72,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,102,103,37,105,106,- 1,108,- 1,- 1,- 1,- 1,- 1,- 1,- 1,49,50,118,119,- 1,-
1,- 1,56,57,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,67,68,- 1,- 1,- 1,72,- 1,- 1,8,9,10,11,12,13,14,
15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,102,103,37,105,106,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,118,119,- 1,- 1,- 1,56,57,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,-
1,67,68,- 1,- 1,- 1,72,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,102,103,37,105,106,- 1,- 1,109,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,118,
119,- 1,- 1,- 1,56,57,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,67,68,- 1,- 1,- 1,72,- 1,- 1,8,9,10,11,
12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,102,103,37,
105,106,- 1,- 1,109,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,118,119,- 1,- 1,- 1,56,57,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,67,68,- 1,- 1,- 1,72,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,-
1,- 1,- 1,- 1,94,- 1,- 1,- 1,- 1,- 1,- 1,- 1,102,103,37,105,106,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,118,119,25,26,- 1,56,57,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,38,67,68,- 1,- 1,- 1,72,- 1,- 1,
8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
102,103,37,105,106,- 1,- 1,- 1,82,- 1,- 1,- 1,- 1,- 1,49,50,118,119,- 1,- 1,- 1,56,57,97,98,
- 1,100,101,- 1,- 1,104,- 1,67,68,- 1,- 1,- 1,72,- 1,- 1,- 1,- 1,116,- 1,118,119,- 1,- 1,122,-
1,- 1,125,- 1,- 1,128,129,- 1,- 1,132,133,- 1,- 1,- 1,- 1,- 1,- 1,- 1,102,- 1,- 1,- 1,106,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,118,119,25,26,27,- 1,29,30,31,32,33,34,35,36,- 1,38,- 1,
40,41,- 1,- 1,44,- 1,46,- 1,- 1,- 1,- 1,51,52,53,54,55,- 1,- 1,- 1,59,- 1,61,62,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,70,71,- 1,- 1,74,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,97,98,99,100,101,- 1,- 1,104,- 1,- 1,107,108,- 1,- 1,25,26,- 1,- 1,- 1,116,- 1,32,
119,- 1,- 1,122,- 1,38,125,40,- 1,128,129,44,- 1,132,133,- 1,- 1,- 1,51,52,53,54,55,- 1,-
1,- 1,59,- 1,- 1,62,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,- 1,74,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,97,98,99,100,101,- 1,- 1,104,- 1,- 1,- 1,108,
109,- 1,25,26,- 1,114,- 1,116,- 1,- 1,119,- 1,121,122,- 1,38,125,40,- 1,128,129,44,- 1,
132,133,- 1,- 1,- 1,51,52,53,54,55,- 1,- 1,- 1,59,- 1,- 1,62,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,
- 1,- 1,74,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,97,98,
99,100,101,- 1,- 1,104,- 1,- 1,- 1,108,109,- 1,25,26,- 1,114,- 1,116,- 1,- 1,119,- 1,121,
122,- 1,38,125,40,- 1,128,129,44,- 1,132,133,- 1,- 1,- 1,51,52,53,54,55,- 1,- 1,- 1,59,- 1,
- 1,62,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,- 1,74,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,97,98,99,100,101,- 1,- 1,104,- 1,- 1,- 1,108,109,- 1,25,26,
- 1,114,- 1,116,- 1,- 1,119,- 1,121,122,- 1,38,125,40,- 1,128,129,44,- 1,132,133,- 1,- 1,-
1,51,52,53,54,55,- 1,- 1,- 1,59,- 1,- 1,62,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,- 1,74,75,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,97,98,99,100,101,- 1,
- 1,104,- 1,- 1,- 1,108,- 1,- 1,25,26,- 1,114,- 1,116,- 1,- 1,119,- 1,121,122,- 1,38,125,40,
- 1,128,129,44,- 1,132,133,- 1,- 1,- 1,51,52,53,54,55,- 1,- 1,- 1,59,- 1,- 1,62,- 1,- 1,- 1,-
1,- 1,- 1,- 1,70,71,- 1,- 1,74,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,97,98,99,100,101,- 1,- 1,104,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,115,116,- 1,
- 1,119,- 1,38,122,40,- 1,125,- 1,44,128,129,- 1,- 1,132,133,51,52,53,54,55,- 1,- 1,- 1,
59,- 1,- 1,62,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,- 1,74,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,97,98,99,100,101,- 1,- 1,104,- 1,- 1,- 1,108,- 1,- 1,
25,26,- 1,- 1,- 1,116,- 1,- 1,119,- 1,- 1,122,- 1,38,125,40,- 1,128,129,44,- 1,132,133,- 1,
- 1,- 1,51,52,53,54,55,- 1,- 1,- 1,59,- 1,- 1,62,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,- 1,74,75,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,97,98,99,100,101,
- 1,- 1,104,- 1,- 1,107,25,26,- 1,- 1,- 1,- 1,- 1,- 1,116,- 1,- 1,119,- 1,38,122,40,- 1,125,- 1,
44,128,129,- 1,- 1,132,133,51,52,53,54,55,- 1,- 1,- 1,59,- 1,- 1,62,- 1,- 1,- 1,- 1,- 1,- 1,-
1,70,71,- 1,- 1,74,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,97,98,99,100,101,- 1,- 1,104,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,116,117,- 1,119,- 1,
38,122,40,- 1,125,- 1,44,128,129,- 1,- 1,132,133,51,52,53,54,55,- 1,- 1,- 1,59,- 1,- 1,62,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,- 1,74,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,97,98,99,100,101,- 1,- 1,104,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,
115,116,- 1,- 1,119,- 1,38,122,40,- 1,125,- 1,44,128,129,- 1,- 1,132,133,51,52,53,54,55,
- 1,- 1,- 1,59,- 1,- 1,62,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,- 1,74,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,97,98,99,100,101,- 1,- 1,104,- 1,- 1,- 1,25,
26,- 1,- 1,- 1,- 1,- 1,- 1,116,117,- 1,119,- 1,38,122,40,- 1,125,- 1,44,128,129,- 1,- 1,132,
133,51,52,53,54,55,- 1,- 1,- 1,59,- 1,- 1,62,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,- 1,74,75,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,97,98,99,100,101,-
1,- 1,104,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,115,116,- 1,- 1,119,- 1,38,122,40,- 1,125,- 1,
44,128,129,- 1,- 1,132,133,51,52,53,54,55,- 1,- 1,- 1,59,- 1,- 1,62,- 1,- 1,- 1,- 1,- 1,- 1,-
1,70,71,- 1,- 1,74,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,97,98,99,100,101,- 1,- 1,104,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,115,116,- 1,- 1,119,- 1,
38,122,40,- 1,125,- 1,44,128,129,- 1,- 1,132,133,51,52,53,54,55,- 1,- 1,- 1,59,- 1,- 1,62,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,- 1,74,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,97,98,99,100,101,- 1,- 1,104,- 1,- 1,107,25,26,- 1,- 1,- 1,- 1,- 1,-
1,116,- 1,- 1,119,- 1,38,122,40,- 1,125,- 1,44,128,129,- 1,- 1,132,133,51,52,53,54,55,-
1,- 1,- 1,59,- 1,- 1,62,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,- 1,74,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,97,98,99,100,101,- 1,- 1,104,- 1,- 1,107,
25,26,- 1,- 1,- 1,- 1,- 1,- 1,116,- 1,- 1,119,- 1,38,122,40,- 1,125,- 1,44,128,129,- 1,- 1,
132,133,51,52,53,54,55,- 1,- 1,- 1,59,- 1,- 1,62,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,- 1,74,
75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,97,98,99,100,
101,- 1,- 1,104,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,116,117,- 1,119,- 1,38,122,40,- 1,
125,- 1,44,128,129,- 1,- 1,132,133,51,52,53,54,55,- 1,- 1,- 1,59,- 1,- 1,62,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,70,71,- 1,- 1,74,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,97,98,99,100,101,- 1,- 1,104,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,116,117,- 1,
119,- 1,38,122,40,- 1,125,- 1,44,128,129,- 1,- 1,132,133,51,52,53,54,55,- 1,- 1,- 1,59,-
1,- 1,62,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,- 1,74,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,97,98,99,100,101,- 1,- 1,104,- 1,- 1,107,25,26,- 1,- 1,-
1,- 1,- 1,- 1,116,- 1,- 1,119,- 1,38,122,40,- 1,125,- 1,44,128,129,- 1,- 1,132,133,51,52,
53,54,55,- 1,- 1,- 1,59,- 1,- 1,62,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,- 1,74,75,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,97,98,99,100,101,- 1,- 1,104,- 1,
- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,116,117,- 1,119,- 1,38,122,40,- 1,125,- 1,44,128,129,-
1,- 1,132,133,51,52,53,54,55,- 1,- 1,- 1,59,- 1,- 1,62,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,-
1,74,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,97,98,99,
100,101,- 1,- 1,104,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,116,117,- 1,119,- 1,38,122,40,-
1,125,- 1,44,128,129,- 1,- 1,132,133,51,52,53,54,55,- 1,- 1,- 1,59,- 1,- 1,62,- 1,- 1,- 1,-
1,- 1,- 1,- 1,70,71,- 1,- 1,74,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,97,98,99,100,101,- 1,- 1,104,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,116,117,
- 1,119,- 1,38,122,40,- 1,125,- 1,44,128,129,- 1,- 1,132,133,51,52,53,54,55,- 1,- 1,- 1,
59,- 1,- 1,62,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,- 1,74,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,97,98,99,100,101,- 1,- 1,104,- 1,- 1,- 1,25,26,- 1,- 1,
- 1,- 1,- 1,- 1,116,117,- 1,119,- 1,38,122,40,- 1,125,- 1,44,128,129,- 1,- 1,132,133,51,52,
53,54,55,- 1,- 1,- 1,59,- 1,- 1,62,- 1,- 1,25,26,- 1,- 1,- 1,70,71,- 1,- 1,74,75,- 1,- 1,38,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,51,52,53,54,55,- 1,97,98,99,100,101,62,- 1,104,- 1,
- 1,- 1,- 1,- 1,70,71,- 1,- 1,74,75,116,- 1,- 1,119,- 1,- 1,122,- 1,- 1,125,- 1,- 1,128,129,- 1,
- 1,132,133,- 1,- 1,- 1,97,98,99,100,101,- 1,- 1,104,- 1,- 1,- 1,108,- 1,25,26,- 1,- 1,- 1,- 1,
116,- 1,- 1,119,- 1,- 1,122,38,- 1,125,- 1,- 1,128,129,- 1,- 1,132,133,- 1,- 1,51,52,53,54,
55,- 1,- 1,- 1,- 1,- 1,- 1,62,- 1,- 1,25,26,- 1,- 1,- 1,70,71,- 1,- 1,74,75,- 1,- 1,38,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,51,52,53,54,55,- 1,97,98,99,100,101,62,- 1,104,- 1,- 1,- 1,
108,- 1,70,71,- 1,- 1,74,75,116,- 1,- 1,119,- 1,- 1,122,- 1,- 1,125,- 1,- 1,128,129,- 1,- 1,
132,133,- 1,- 1,- 1,97,98,99,100,101,- 1,- 1,104,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,116,
- 1,- 1,119,- 1,38,122,- 1,- 1,125,- 1,- 1,128,129,- 1,- 1,132,133,51,52,53,54,55,- 1,- 1,-
1,- 1,- 1,- 1,62,- 1,- 1,25,26,- 1,- 1,- 1,70,71,- 1,- 1,74,75,- 1,- 1,38,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,51,52,53,54,55,- 1,97,98,99,100,101,62,- 1,104,- 1,- 1,- 1,- 1,- 1,70,
71,- 1,- 1,74,75,116,- 1,- 1,119,- 1,- 1,122,- 1,- 1,125,- 1,- 1,128,129,- 1,- 1,132,133,- 1,
- 1,- 1,97,98,99,100,101,- 1,- 1,104,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,116,- 1,- 1,119,
- 1,38,122,- 1,- 1,125,- 1,- 1,128,129,- 1,- 1,132,133,51,52,53,54,55,- 1,- 1,- 1,- 1,- 1,- 1,
62,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,- 1,74,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,97,98,99,100,101,- 1,- 1,104,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,116,- 1,- 1,119,- 1,- 1,122,- 1,- 1,125,- 1,- 1,128,129,- 1,- 1,132,133};char Cyc_Yyimpossible[
17]="\000\000\000\000Yyimpossible\000";char Cyc_Yystack_overflow[21]="\000\000\000\000Yystack_overflow\000";
void Cyc_yyerror(struct _dyneither_ptr);int Cyc_yylex(struct Cyc_Lexing_lexbuf*);
static int Cyc_yychar=(int)'\000';union Cyc_YYSTYPE_union Cyc_yylval=(union Cyc_YYSTYPE_union)((
struct Cyc_YYINITIALSVAL_struct){57});static int Cyc_yynerrs=0;struct _tuple18{
struct Cyc_Position_Segment*f1;struct _tuple1*f2;int f3;};struct _tuple19{struct
_dyneither_ptr f1;void*f2;};static char _tmp46D[8]="stdcall";static char _tmp46E[6]="cdecl";
static char _tmp46F[9]="fastcall";static char _tmp470[9]="noreturn";static char
_tmp471[6]="const";static char _tmp472[8]="aligned";static char _tmp473[7]="packed";
static char _tmp474[7]="shared";static char _tmp475[7]="unused";static char _tmp476[5]="weak";
static char _tmp477[10]="dllimport";static char _tmp478[10]="dllexport";static char
_tmp479[23]="no_instrument_function";static char _tmp47A[12]="constructor";static
char _tmp47B[11]="destructor";static char _tmp47C[22]="no_check_memory_usage";
static char _tmp47D[5]="pure";struct _tuple20{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*
f2;};int Cyc_yyparse(struct Cyc_Lexing_lexbuf*yylex_buf){struct _RegionHandle
_tmp380=_new_region("yyregion");struct _RegionHandle*yyregion=& _tmp380;
_push_region(yyregion);{int yystate;int yyn=0;int yyerrstatus;int yychar1=0;int
yyssp_offset;short*yyss=(short*)({unsigned int _tmp8C8=10000;short*_tmp8C9=(short*)
_region_malloc(yyregion,_check_times(sizeof(short),_tmp8C8));{unsigned int
_tmp8CA=_tmp8C8;unsigned int i;for(i=0;i < _tmp8CA;i ++){_tmp8C9[i]=(short)0;}}
_tmp8C9;});int yyvsp_offset;union Cyc_YYSTYPE_union*yyvs=(union Cyc_YYSTYPE_union*)({
unsigned int _tmp8C5=10000;union Cyc_YYSTYPE_union*_tmp8C6=(union Cyc_YYSTYPE_union*)
_region_malloc(yyregion,_check_times(sizeof(union Cyc_YYSTYPE_union),_tmp8C5));{
unsigned int _tmp8C7=_tmp8C5;unsigned int i;for(i=0;i < _tmp8C7;i ++){_tmp8C6[i]=Cyc_yylval;}}
_tmp8C6;});int yylsp_offset;struct Cyc_Yyltype*yyls=(struct Cyc_Yyltype*)({
unsigned int _tmp8C2=10000;struct Cyc_Yyltype*_tmp8C3=(struct Cyc_Yyltype*)
_region_malloc(yyregion,_check_times(sizeof(struct Cyc_Yyltype),_tmp8C2));{
unsigned int _tmp8C4=_tmp8C2;unsigned int i;for(i=0;i < _tmp8C4;i ++){_tmp8C3[i]=Cyc_yynewloc();}}
_tmp8C3;});int yystacksize=10000;union Cyc_YYSTYPE_union yyval=Cyc_yylval;int yylen;
yystate=0;yyerrstatus=0;Cyc_yynerrs=0;Cyc_yychar=- 2;yyssp_offset=- 1;yyvsp_offset=
0;yylsp_offset=0;yynewstate: yyss[_check_known_subscript_notnull(10000,++
yyssp_offset)]=(short)yystate;if(yyssp_offset >= yystacksize - 1){if(yystacksize >= 
10000){Cyc_yyerror(({const char*_tmp381="parser stack overflow";_tag_dyneither(
_tmp381,sizeof(char),22);}));(int)_throw((void*)Cyc_Yystack_overflow);}}goto
yybackup;yybackup: yyn=(int)Cyc_yypact[_check_known_subscript_notnull(1010,
yystate)];if(yyn == - 32768)goto yydefault;if(Cyc_yychar == - 2)Cyc_yychar=Cyc_yylex(
yylex_buf);if(Cyc_yychar <= 0){yychar1=0;Cyc_yychar=0;}else{yychar1=Cyc_yychar > 0
 && Cyc_yychar <= 361?(int)Cyc_yytranslate[_check_known_subscript_notnull(362,Cyc_yychar)]:
276;}yyn +=yychar1;if((yyn < 0  || yyn > 6136) || Cyc_yycheck[
_check_known_subscript_notnull(6137,yyn)]!= yychar1)goto yydefault;yyn=(int)Cyc_yytable[
_check_known_subscript_notnull(6137,yyn)];if(yyn < 0){if(yyn == - 32768)goto
yyerrlab;yyn=- yyn;goto yyreduce;}else{if(yyn == 0)goto yyerrlab;}if(yyn == 1009){int
_tmp382=0;_npop_handler(0);return _tmp382;}if(Cyc_yychar != 0)Cyc_yychar=- 2;yyvs[
_check_known_subscript_notnull(10000,++ yyvsp_offset)]=Cyc_yylval;yyls[
_check_known_subscript_notnull(10000,++ yylsp_offset)]=Cyc_yylloc;if(yyerrstatus
!= 0)-- yyerrstatus;yystate=yyn;goto yynewstate;yydefault: yyn=(int)Cyc_yydefact[
_check_known_subscript_notnull(1010,yystate)];if(yyn == 0)goto yyerrlab;yyreduce:
yylen=(int)Cyc_yyr2[_check_known_subscript_notnull(500,yyn)];if(yylen > 0)yyval=
yyvs[_check_known_subscript_notnull(10000,(yyvsp_offset + 1)- yylen)];switch(yyn){
case 1: _LL1E8: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];Cyc_Parse_parse_result=
Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);break;
case 2: _LL1E9: {struct Cyc_List_List*x;struct Cyc_List_List*y;{struct _handler_cons
_tmp383;_push_handler(& _tmp383);{int _tmp385=0;if(setjmp(_tmp383.handler))_tmp385=
1;if(!_tmp385){x=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);;_pop_handler();}else{void*_tmp384=(void*)_exn_thrown;void*
_tmp387=_tmp384;_LL1EC: if(*((void**)_tmp387)!= Cyc_Core_Failure)goto _LL1EE;
_LL1ED: x=0;goto _LL1EB;_LL1EE:;_LL1EF:(void)_throw(_tmp387);_LL1EB:;}}}{struct
_handler_cons _tmp388;_push_handler(& _tmp388);{int _tmp38A=0;if(setjmp(_tmp388.handler))
_tmp38A=1;if(!_tmp38A){y=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);;_pop_handler();}else{void*_tmp389=(void*)_exn_thrown;void*
_tmp38C=_tmp389;_LL1F1: if(*((void**)_tmp38C)!= Cyc_Core_Failure)goto _LL1F3;
_LL1F2: y=0;goto _LL1F0;_LL1F3:;_LL1F4:(void)_throw(_tmp38C);_LL1F0:;}}}yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp38D;(_tmp38D.YY18).tag=22;(
_tmp38D.YY18).f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_imp_append)(x,y);_tmp38D;});break;}case 3: _LL1EA: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp38E;(_tmp38E.YY18).tag=22;(_tmp38E.YY18).f1=({struct
Cyc_List_List*_tmp38F=_cycalloc(sizeof(*_tmp38F));_tmp38F->hd=({struct Cyc_Absyn_Decl*
_tmp390=_cycalloc(sizeof(*_tmp390));_tmp390->r=(void*)((void*)({struct Cyc_Absyn_Using_d_struct*
_tmp391=_cycalloc(sizeof(*_tmp391));_tmp391[0]=({struct Cyc_Absyn_Using_d_struct
_tmp392;_tmp392.tag=11;_tmp392.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp392.f2=Cyc_yyget_YY18(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp392;});_tmp391;}));
_tmp390->loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp390;});_tmp38F->tl=0;_tmp38F;});_tmp38E;});Cyc_Lex_leave_using();
break;case 4: _LL1F5: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp393;(
_tmp393.YY18).tag=22;(_tmp393.YY18).f1=({struct Cyc_List_List*_tmp394=_cycalloc(
sizeof(*_tmp394));_tmp394->hd=({struct Cyc_Absyn_Decl*_tmp395=_cycalloc(sizeof(*
_tmp395));_tmp395->r=(void*)((void*)({struct Cyc_Absyn_Using_d_struct*_tmp396=
_cycalloc(sizeof(*_tmp396));_tmp396[0]=({struct Cyc_Absyn_Using_d_struct _tmp397;
_tmp397.tag=11;_tmp397.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp397.f2=Cyc_yyget_YY18(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp397;});_tmp396;}));
_tmp395->loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line);_tmp395;});_tmp394->tl=Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp394;});_tmp393;});
break;case 5: _LL1F6: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp398;(
_tmp398.YY18).tag=22;(_tmp398.YY18).f1=({struct Cyc_List_List*_tmp399=_cycalloc(
sizeof(*_tmp399));_tmp399->hd=({struct Cyc_Absyn_Decl*_tmp39A=_cycalloc(sizeof(*
_tmp39A));_tmp39A->r=(void*)((void*)({struct Cyc_Absyn_Namespace_d_struct*_tmp39B=
_cycalloc(sizeof(*_tmp39B));_tmp39B[0]=({struct Cyc_Absyn_Namespace_d_struct
_tmp39C;_tmp39C.tag=10;_tmp39C.f1=({struct _dyneither_ptr*_tmp39D=_cycalloc(
sizeof(*_tmp39D));_tmp39D[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp39D;});_tmp39C.f2=
Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp39C;});
_tmp39B;}));_tmp39A->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp39A;});
_tmp399->tl=0;_tmp399;});_tmp398;});Cyc_Lex_leave_namespace();break;case 6: _LL1F7: {
struct _dyneither_ptr nspace;struct Cyc_List_List*x;struct Cyc_List_List*y;{struct
_handler_cons _tmp39E;_push_handler(& _tmp39E);{int _tmp3A0=0;if(setjmp(_tmp39E.handler))
_tmp3A0=1;if(!_tmp3A0){nspace=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);;_pop_handler();}else{
void*_tmp39F=(void*)_exn_thrown;void*_tmp3A2=_tmp39F;_LL1FA: if(*((void**)_tmp3A2)
!= Cyc_Core_Failure)goto _LL1FC;_LL1FB: nspace=({const char*_tmp3A3="";
_tag_dyneither(_tmp3A3,sizeof(char),1);});goto _LL1F9;_LL1FC:;_LL1FD:(void)_throw(
_tmp3A2);_LL1F9:;}}}{struct _handler_cons _tmp3A4;_push_handler(& _tmp3A4);{int
_tmp3A6=0;if(setjmp(_tmp3A4.handler))_tmp3A6=1;if(!_tmp3A6){x=Cyc_yyget_YY18(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);;_pop_handler();}
else{void*_tmp3A5=(void*)_exn_thrown;void*_tmp3A8=_tmp3A5;_LL1FF: if(*((void**)
_tmp3A8)!= Cyc_Core_Failure)goto _LL201;_LL200: x=0;goto _LL1FE;_LL201:;_LL202:(
void)_throw(_tmp3A8);_LL1FE:;}}}{struct _handler_cons _tmp3A9;_push_handler(&
_tmp3A9);{int _tmp3AB=0;if(setjmp(_tmp3A9.handler))_tmp3AB=1;if(!_tmp3AB){y=Cyc_yyget_YY18(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);;_pop_handler();}else{
void*_tmp3AA=(void*)_exn_thrown;void*_tmp3AD=_tmp3AA;_LL204: if(*((void**)_tmp3AD)
!= Cyc_Core_Failure)goto _LL206;_LL205: y=0;goto _LL203;_LL206:;_LL207:(void)_throw(
_tmp3AD);_LL203:;}}}yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp3AE;(_tmp3AE.YY18).tag=22;(_tmp3AE.YY18).f1=({struct Cyc_List_List*_tmp3AF=
_cycalloc(sizeof(*_tmp3AF));_tmp3AF->hd=({struct Cyc_Absyn_Decl*_tmp3B0=_cycalloc(
sizeof(*_tmp3B0));_tmp3B0->r=(void*)((void*)({struct Cyc_Absyn_Namespace_d_struct*
_tmp3B1=_cycalloc(sizeof(*_tmp3B1));_tmp3B1[0]=({struct Cyc_Absyn_Namespace_d_struct
_tmp3B2;_tmp3B2.tag=10;_tmp3B2.f1=({struct _dyneither_ptr*_tmp3B3=_cycalloc(
sizeof(*_tmp3B3));_tmp3B3[0]=nspace;_tmp3B3;});_tmp3B2.f2=x;_tmp3B2;});_tmp3B1;}));
_tmp3B0->loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line);_tmp3B0;});_tmp3AF->tl=y;_tmp3AF;});_tmp3AE;});
break;}case 7: _LL1F8: if(Cyc_strcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),({const char*_tmp3B4="C";
_tag_dyneither(_tmp3B4,sizeof(char),2);}))== 0)yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp3B5;(_tmp3B5.YY18).tag=22;(_tmp3B5.YY18).f1=({struct
Cyc_List_List*_tmp3B6=_cycalloc(sizeof(*_tmp3B6));_tmp3B6->hd=({struct Cyc_Absyn_Decl*
_tmp3B7=_cycalloc(sizeof(*_tmp3B7));_tmp3B7->r=(void*)((void*)({struct Cyc_Absyn_ExternC_d_struct*
_tmp3B8=_cycalloc(sizeof(*_tmp3B8));_tmp3B8[0]=({struct Cyc_Absyn_ExternC_d_struct
_tmp3B9;_tmp3B9.tag=12;_tmp3B9.f1=Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp3B9;});_tmp3B8;}));
_tmp3B7->loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line);_tmp3B7;});_tmp3B6->tl=Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp3B6;});_tmp3B5;});else{
if(Cyc_strcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),({const char*_tmp3BA="C include";
_tag_dyneither(_tmp3BA,sizeof(char),10);}))!= 0)Cyc_Parse_err(({const char*
_tmp3BB="expecting \"C\" or \"C include\"";_tag_dyneither(_tmp3BB,sizeof(char),
29);}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 4)]).last_line));yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp3BC;(_tmp3BC.YY18).tag=22;(_tmp3BC.YY18).f1=({struct Cyc_List_List*_tmp3BD=
_cycalloc(sizeof(*_tmp3BD));_tmp3BD->hd=({struct Cyc_Absyn_Decl*_tmp3BE=_cycalloc(
sizeof(*_tmp3BE));_tmp3BE->r=(void*)((void*)({struct Cyc_Absyn_ExternCinclude_d_struct*
_tmp3BF=_cycalloc(sizeof(*_tmp3BF));_tmp3BF[0]=({struct Cyc_Absyn_ExternCinclude_d_struct
_tmp3C0;_tmp3C0.tag=13;_tmp3C0.f1=Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp3C0.f2=0;_tmp3C0;});
_tmp3BF;}));_tmp3BE->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line);_tmp3BE;});
_tmp3BD->tl=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp3BD;});_tmp3BC;});}break;case 8: _LL208: if(Cyc_strcmp((struct
_dyneither_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 5)]),({const char*_tmp3C1="C include";_tag_dyneither(_tmp3C1,
sizeof(char),10);}))!= 0)Cyc_Parse_err(({const char*_tmp3C2="expecting \"C include\"";
_tag_dyneither(_tmp3C2,sizeof(char),22);}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).last_line));{struct Cyc_List_List*
exs=Cyc_yyget_YY52(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp3C3;(_tmp3C3.YY18).tag=
22;(_tmp3C3.YY18).f1=({struct Cyc_List_List*_tmp3C4=_cycalloc(sizeof(*_tmp3C4));
_tmp3C4->hd=({struct Cyc_Absyn_Decl*_tmp3C5=_cycalloc(sizeof(*_tmp3C5));_tmp3C5->r=(
void*)((void*)({struct Cyc_Absyn_ExternCinclude_d_struct*_tmp3C6=_cycalloc(
sizeof(*_tmp3C6));_tmp3C6[0]=({struct Cyc_Absyn_ExternCinclude_d_struct _tmp3C7;
_tmp3C7.tag=13;_tmp3C7.f1=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]);_tmp3C7.f2=exs;_tmp3C7;});_tmp3C6;}));_tmp3C5->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line);_tmp3C5;});
_tmp3C4->tl=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp3C4;});_tmp3C3;});break;}case 9: _LL209: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp3C8;(_tmp3C8.YY18).tag=22;(_tmp3C8.YY18).f1=({struct
Cyc_List_List*_tmp3C9=_cycalloc(sizeof(*_tmp3C9));_tmp3C9->hd=({struct Cyc_Absyn_Decl*
_tmp3CA=_cycalloc(sizeof(*_tmp3CA));_tmp3CA->r=(void*)((void*)0);_tmp3CA->loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line);_tmp3CA;});_tmp3C9->tl=Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp3C9;});_tmp3C8;});
break;case 10: _LL20A: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp3CB;(_tmp3CB.YY18).tag=22;(_tmp3CB.YY18).f1=({struct Cyc_List_List*_tmp3CC=
_cycalloc(sizeof(*_tmp3CC));_tmp3CC->hd=({struct Cyc_Absyn_Decl*_tmp3CD=_cycalloc(
sizeof(*_tmp3CD));_tmp3CD->r=(void*)((void*)1);_tmp3CD->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line);_tmp3CD;});
_tmp3CC->tl=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp3CC;});_tmp3CB;});break;case 11: _LL20B: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp3CE;(_tmp3CE.YY18).tag=22;(_tmp3CE.YY18).f1=0;_tmp3CE;});
break;case 12: _LL20C: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset
- 1)];break;case 13: _LL20D: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp3CF;(_tmp3CF.YY52).tag=56;(_tmp3CF.YY52).f1=0;_tmp3CF;});break;case 14: _LL20E:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp3D0;(_tmp3D0.YY52).tag=
56;(_tmp3D0.YY52).f1=({struct Cyc_List_List*_tmp3D1=_cycalloc(sizeof(*_tmp3D1));
_tmp3D1->hd=({struct _tuple18*_tmp3D2=_cycalloc(sizeof(*_tmp3D2));_tmp3D2->f1=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line);_tmp3D2->f2=
Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp3D2->f3=0;_tmp3D2;});_tmp3D1->tl=0;_tmp3D1;});_tmp3D0;});break;case 15: _LL20F:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp3D3;(_tmp3D3.YY52).tag=
56;(_tmp3D3.YY52).f1=({struct Cyc_List_List*_tmp3D4=_cycalloc(sizeof(*_tmp3D4));
_tmp3D4->hd=({struct _tuple18*_tmp3D5=_cycalloc(sizeof(*_tmp3D5));_tmp3D5->f1=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line);_tmp3D5->f2=
Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);
_tmp3D5->f3=0;_tmp3D5;});_tmp3D4->tl=Cyc_yyget_YY52(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp3D4;});_tmp3D3;});
break;case 16: _LL210: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp3D6;(_tmp3D6.YY18).tag=22;(_tmp3D6.YY18).f1=({struct Cyc_List_List*_tmp3D7=
_cycalloc(sizeof(*_tmp3D7));_tmp3D7->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Fn_d_struct*
_tmp3D8=_cycalloc(sizeof(*_tmp3D8));_tmp3D8[0]=({struct Cyc_Absyn_Fn_d_struct
_tmp3D9;_tmp3D9.tag=1;_tmp3D9.f1=Cyc_yyget_YY17(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp3D9;});_tmp3D8;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp3D7->tl=0;
_tmp3D7;});_tmp3D6;});break;case 17: _LL211: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 18: _LL212: yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp3DA;(_tmp3DA.YY18).tag=22;(
_tmp3DA.YY18).f1=0;_tmp3DA;});break;case 19: _LL213: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp3DB;(_tmp3DB.YY17).tag=21;(_tmp3DB.YY17).f1=Cyc_Parse_make_function(
0,Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0,
Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp3DB;});
break;case 20: _LL214: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp3DC;(_tmp3DC.YY17).tag=21;(_tmp3DC.YY17).f1=Cyc_Parse_make_function(({struct
Cyc_Core_Opt*_tmp3DD=_cycalloc(sizeof(*_tmp3DD));_tmp3DD->v=Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp3DD;}),Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0,Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp3DC;});
break;case 21: _LL215: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp3DE;(_tmp3DE.YY17).tag=21;(_tmp3DE.YY17).f1=Cyc_Parse_make_function(0,Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp3DE;});
break;case 22: _LL216: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp3DF;(_tmp3DF.YY17).tag=21;(_tmp3DF.YY17).f1=Cyc_Parse_make_function(({struct
Cyc_Core_Opt*_tmp3E0=_cycalloc(sizeof(*_tmp3E0));_tmp3E0->v=Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp3E0;}),Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp3DF;});
break;case 23: _LL217: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp3E1;(_tmp3E1.YY17).tag=21;(_tmp3E1.YY17).f1=Cyc_Parse_make_function(({struct
Cyc_Core_Opt*_tmp3E2=_cycalloc(sizeof(*_tmp3E2));_tmp3E2->v=Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp3E2;}),Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0,Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp3E1;});
break;case 24: _LL218: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp3E3;(_tmp3E3.YY17).tag=21;(_tmp3E3.YY17).f1=Cyc_Parse_make_function(({struct
Cyc_Core_Opt*_tmp3E4=_cycalloc(sizeof(*_tmp3E4));_tmp3E4->v=Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp3E4;}),Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp3E3;});
break;case 25: _LL219: Cyc_Lex_enter_using(Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]));yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 26: _LL21A: Cyc_Lex_leave_using();
break;case 27: _LL21B: Cyc_Lex_enter_namespace(({struct _dyneither_ptr*_tmp3E5=
_cycalloc(sizeof(*_tmp3E5));_tmp3E5[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp3E5;}));yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 28: _LL21C: Cyc_Lex_leave_namespace();
break;case 29: _LL21D: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp3E6;(_tmp3E6.YY18).tag=22;(_tmp3E6.YY18).f1=Cyc_Parse_make_declarations(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp3E6;});
break;case 30: _LL21E: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp3E7;(_tmp3E7.YY18).tag=22;(_tmp3E7.YY18).f1=Cyc_Parse_make_declarations(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY21(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp3E7;});
break;case 31: _LL21F: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp3E8;(_tmp3E8.YY18).tag=22;(_tmp3E8.YY18).f1=({struct Cyc_List_List*_tmp3E9=
_cycalloc(sizeof(*_tmp3E9));_tmp3E9->hd=Cyc_Absyn_let_decl(Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp3E9->tl=0;
_tmp3E9;});_tmp3E8;});break;case 32: _LL220: {struct Cyc_List_List*_tmp3EA=0;{
struct Cyc_List_List*_tmp3EB=Cyc_yyget_YY36(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);for(0;_tmp3EB != 0;_tmp3EB=_tmp3EB->tl){struct
_dyneither_ptr*_tmp3EC=(struct _dyneither_ptr*)_tmp3EB->hd;struct _tuple1*qv=({
struct _tuple1*_tmp3EF=_cycalloc(sizeof(*_tmp3EF));_tmp3EF->f1=(union Cyc_Absyn_Nmspace_union)({
union Cyc_Absyn_Nmspace_union _tmp3F0;(_tmp3F0.Rel_n).tag=1;(_tmp3F0.Rel_n).f1=0;
_tmp3F0;});_tmp3EF->f2=_tmp3EC;_tmp3EF;});struct Cyc_Absyn_Vardecl*_tmp3ED=Cyc_Absyn_new_vardecl(
qv,Cyc_Absyn_wildtyp(0),0);_tmp3EA=({struct Cyc_List_List*_tmp3EE=_cycalloc(
sizeof(*_tmp3EE));_tmp3EE->hd=_tmp3ED;_tmp3EE->tl=_tmp3EA;_tmp3EE;});}}_tmp3EA=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp3EA);yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp3F1;(_tmp3F1.YY18).tag=22;(
_tmp3F1.YY18).f1=({struct Cyc_List_List*_tmp3F2=_cycalloc(sizeof(*_tmp3F2));
_tmp3F2->hd=Cyc_Absyn_letv_decl(_tmp3EA,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp3F2->tl=0;
_tmp3F2;});_tmp3F1;});break;}case 33: _LL221: if(Cyc_zstrcmp((struct _dyneither_ptr)
Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),({
const char*_tmp3F3="`H";_tag_dyneither(_tmp3F3,sizeof(char),3);}))== 0  || Cyc_zstrcmp((
struct _dyneither_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]),({const char*_tmp3F4="`U";_tag_dyneither(_tmp3F4,sizeof(
char),3);}))== 0)Cyc_Parse_err((struct _dyneither_ptr)({struct Cyc_String_pa_struct
_tmp3F7;_tmp3F7.tag=0;_tmp3F7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]));{
void*_tmp3F5[1]={& _tmp3F7};Cyc_aprintf(({const char*_tmp3F6="bad occurrence of heap region %s";
_tag_dyneither(_tmp3F6,sizeof(char),33);}),_tag_dyneither(_tmp3F5,sizeof(void*),
1));}}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line));{struct Cyc_Absyn_Tvar*tv=({struct Cyc_Absyn_Tvar*
_tmp401=_cycalloc(sizeof(*_tmp401));_tmp401->name=({struct _dyneither_ptr*_tmp402=
_cycalloc(sizeof(*_tmp402));_tmp402[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp402;});_tmp401->identity=
- 1;_tmp401->kind=(void*)Cyc_Tcutil_kind_to_bound((void*)3);_tmp401;});void*t=(
void*)({struct Cyc_Absyn_VarType_struct*_tmp3FF=_cycalloc(sizeof(*_tmp3FF));
_tmp3FF[0]=({struct Cyc_Absyn_VarType_struct _tmp400;_tmp400.tag=1;_tmp400.f1=tv;
_tmp400;});_tmp3FF;});struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(({struct
_tuple1*_tmp3FA=_cycalloc(sizeof(*_tmp3FA));_tmp3FA->f1=(union Cyc_Absyn_Nmspace_union)({
union Cyc_Absyn_Nmspace_union _tmp3FC;(_tmp3FC.Loc_n).tag=0;_tmp3FC;});_tmp3FA->f2=({
struct _dyneither_ptr*_tmp3FB=_cycalloc(sizeof(*_tmp3FB));_tmp3FB[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp3FB;});_tmp3FA;}),(
void*)({struct Cyc_Absyn_RgnHandleType_struct*_tmp3FD=_cycalloc(sizeof(*_tmp3FD));
_tmp3FD[0]=({struct Cyc_Absyn_RgnHandleType_struct _tmp3FE;_tmp3FE.tag=14;_tmp3FE.f1=(
void*)t;_tmp3FE;});_tmp3FD;}),0);yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp3F8;(_tmp3F8.YY18).tag=22;(_tmp3F8.YY18).f1=({struct Cyc_List_List*_tmp3F9=
_cycalloc(sizeof(*_tmp3F9));_tmp3F9->hd=Cyc_Absyn_region_decl(tv,vd,0,0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp3F9->tl=0;
_tmp3F9;});_tmp3F8;});break;}case 34: _LL222: if(Cyc_zstrcmp((struct _dyneither_ptr)
Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),({
const char*_tmp403="H";_tag_dyneither(_tmp403,sizeof(char),2);}))== 0)Cyc_Parse_err(({
const char*_tmp404="bad occurrence of heap region `H";_tag_dyneither(_tmp404,
sizeof(char),33);}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line));{struct Cyc_Absyn_Tvar*
tv=({struct Cyc_Absyn_Tvar*_tmp40E=_cycalloc(sizeof(*_tmp40E));_tmp40E->name=({
struct _dyneither_ptr*_tmp40F=_cycalloc(sizeof(*_tmp40F));_tmp40F[0]=(struct
_dyneither_ptr)({struct Cyc_String_pa_struct _tmp412;_tmp412.tag=0;_tmp412.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]));{void*_tmp410[1]={&
_tmp412};Cyc_aprintf(({const char*_tmp411="`%s";_tag_dyneither(_tmp411,sizeof(
char),4);}),_tag_dyneither(_tmp410,sizeof(void*),1));}});_tmp40F;});_tmp40E->identity=
- 1;_tmp40E->kind=(void*)Cyc_Tcutil_kind_to_bound((void*)3);_tmp40E;});void*t=(
void*)({struct Cyc_Absyn_VarType_struct*_tmp40C=_cycalloc(sizeof(*_tmp40C));
_tmp40C[0]=({struct Cyc_Absyn_VarType_struct _tmp40D;_tmp40D.tag=1;_tmp40D.f1=tv;
_tmp40D;});_tmp40C;});struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(({struct
_tuple1*_tmp407=_cycalloc(sizeof(*_tmp407));_tmp407->f1=(union Cyc_Absyn_Nmspace_union)({
union Cyc_Absyn_Nmspace_union _tmp409;(_tmp409.Loc_n).tag=0;_tmp409;});_tmp407->f2=({
struct _dyneither_ptr*_tmp408=_cycalloc(sizeof(*_tmp408));_tmp408[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp408;});_tmp407;}),(
void*)({struct Cyc_Absyn_RgnHandleType_struct*_tmp40A=_cycalloc(sizeof(*_tmp40A));
_tmp40A[0]=({struct Cyc_Absyn_RgnHandleType_struct _tmp40B;_tmp40B.tag=14;_tmp40B.f1=(
void*)t;_tmp40B;});_tmp40A;}),0);yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp405;(_tmp405.YY18).tag=22;(_tmp405.YY18).f1=({struct Cyc_List_List*_tmp406=
_cycalloc(sizeof(*_tmp406));_tmp406->hd=Cyc_Absyn_region_decl(tv,vd,0,Cyc_yyget_YY4(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp406->tl=0;
_tmp406;});_tmp405;});break;}case 35: _LL223: if(Cyc_zstrcmp((struct _dyneither_ptr)
Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),({
const char*_tmp413="resetable";_tag_dyneither(_tmp413,sizeof(char),10);}))!= 0)
Cyc_Parse_err(({const char*_tmp414="expecting [resetable]";_tag_dyneither(_tmp414,
sizeof(char),22);}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).last_line));if(Cyc_zstrcmp((
struct _dyneither_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]),({const char*_tmp415="`H";_tag_dyneither(_tmp415,sizeof(
char),3);}))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),({const char*_tmp416="`U";
_tag_dyneither(_tmp416,sizeof(char),3);})))Cyc_Parse_err((struct _dyneither_ptr)({
struct Cyc_String_pa_struct _tmp419;_tmp419.tag=0;_tmp419.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]));{void*_tmp417[1]={& _tmp419};Cyc_aprintf(({const char*
_tmp418="bad occurrence of heap region %s";_tag_dyneither(_tmp418,sizeof(char),
33);}),_tag_dyneither(_tmp417,sizeof(void*),1));}}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));{struct Cyc_Absyn_Tvar*
tv=({struct Cyc_Absyn_Tvar*_tmp423=_cycalloc(sizeof(*_tmp423));_tmp423->name=({
struct _dyneither_ptr*_tmp424=_cycalloc(sizeof(*_tmp424));_tmp424[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp424;});_tmp423->identity=
- 1;_tmp423->kind=(void*)Cyc_Tcutil_kind_to_bound((void*)3);_tmp423;});void*t=(
void*)({struct Cyc_Absyn_VarType_struct*_tmp421=_cycalloc(sizeof(*_tmp421));
_tmp421[0]=({struct Cyc_Absyn_VarType_struct _tmp422;_tmp422.tag=1;_tmp422.f1=tv;
_tmp422;});_tmp421;});struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(({struct
_tuple1*_tmp41C=_cycalloc(sizeof(*_tmp41C));_tmp41C->f1=(union Cyc_Absyn_Nmspace_union)({
union Cyc_Absyn_Nmspace_union _tmp41E;(_tmp41E.Loc_n).tag=0;_tmp41E;});_tmp41C->f2=({
struct _dyneither_ptr*_tmp41D=_cycalloc(sizeof(*_tmp41D));_tmp41D[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp41D;});_tmp41C;}),(
void*)({struct Cyc_Absyn_RgnHandleType_struct*_tmp41F=_cycalloc(sizeof(*_tmp41F));
_tmp41F[0]=({struct Cyc_Absyn_RgnHandleType_struct _tmp420;_tmp420.tag=14;_tmp420.f1=(
void*)t;_tmp420;});_tmp41F;}),0);yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp41A;(_tmp41A.YY18).tag=22;(_tmp41A.YY18).f1=({struct Cyc_List_List*_tmp41B=
_cycalloc(sizeof(*_tmp41B));_tmp41B->hd=Cyc_Absyn_region_decl(tv,vd,1,0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 8)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp41B->tl=0;
_tmp41B;});_tmp41A;});break;}case 36: _LL224: if(Cyc_zstrcmp((struct _dyneither_ptr)
Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),({
const char*_tmp425="resetable";_tag_dyneither(_tmp425,sizeof(char),10);}))!= 0)
Cyc_Parse_err(({const char*_tmp426="expecting `resetable'";_tag_dyneither(_tmp426,
sizeof(char),22);}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));if(Cyc_zstrcmp((
struct _dyneither_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]),({const char*_tmp427="H";_tag_dyneither(_tmp427,sizeof(
char),2);}))== 0)Cyc_Parse_err(({const char*_tmp428="bad occurrence of heap region `H";
_tag_dyneither(_tmp428,sizeof(char),33);}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));{struct Cyc_Absyn_Tvar*
tv=({struct Cyc_Absyn_Tvar*_tmp432=_cycalloc(sizeof(*_tmp432));_tmp432->name=({
struct _dyneither_ptr*_tmp433=_cycalloc(sizeof(*_tmp433));_tmp433[0]=(struct
_dyneither_ptr)({struct Cyc_String_pa_struct _tmp436;_tmp436.tag=0;_tmp436.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));{void*_tmp434[1]={&
_tmp436};Cyc_aprintf(({const char*_tmp435="`%s";_tag_dyneither(_tmp435,sizeof(
char),4);}),_tag_dyneither(_tmp434,sizeof(void*),1));}});_tmp433;});_tmp432->identity=
- 1;_tmp432->kind=(void*)Cyc_Tcutil_kind_to_bound((void*)3);_tmp432;});void*t=(
void*)({struct Cyc_Absyn_VarType_struct*_tmp430=_cycalloc(sizeof(*_tmp430));
_tmp430[0]=({struct Cyc_Absyn_VarType_struct _tmp431;_tmp431.tag=1;_tmp431.f1=tv;
_tmp431;});_tmp430;});struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(({struct
_tuple1*_tmp42B=_cycalloc(sizeof(*_tmp42B));_tmp42B->f1=(union Cyc_Absyn_Nmspace_union)({
union Cyc_Absyn_Nmspace_union _tmp42D;(_tmp42D.Loc_n).tag=0;_tmp42D;});_tmp42B->f2=({
struct _dyneither_ptr*_tmp42C=_cycalloc(sizeof(*_tmp42C));_tmp42C[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp42C;});_tmp42B;}),(
void*)({struct Cyc_Absyn_RgnHandleType_struct*_tmp42E=_cycalloc(sizeof(*_tmp42E));
_tmp42E[0]=({struct Cyc_Absyn_RgnHandleType_struct _tmp42F;_tmp42F.tag=14;_tmp42F.f1=(
void*)t;_tmp42F;});_tmp42E;}),0);yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp429;(_tmp429.YY18).tag=22;(_tmp429.YY18).f1=({struct Cyc_List_List*_tmp42A=
_cycalloc(sizeof(*_tmp42A));_tmp42A->hd=Cyc_Absyn_region_decl(tv,vd,1,0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp42A->tl=0;
_tmp42A;});_tmp429;});break;}case 37: _LL225: {struct Cyc_Absyn_Tvar*tv=({struct Cyc_Absyn_Tvar*
_tmp43C=_cycalloc(sizeof(*_tmp43C));_tmp43C->name=({struct _dyneither_ptr*_tmp43F=
_cycalloc(sizeof(*_tmp43F));_tmp43F[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 5)]);_tmp43F;});_tmp43C->identity=
- 1;_tmp43C->kind=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp43D=_cycalloc(
sizeof(*_tmp43D));_tmp43D[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp43E;_tmp43E.tag=0;
_tmp43E.f1=(void*)((void*)3);_tmp43E;});_tmp43D;}));_tmp43C;});struct Cyc_Absyn_Vardecl*
vd=Cyc_Absyn_new_vardecl(({struct _tuple1*_tmp439=_cycalloc(sizeof(*_tmp439));
_tmp439->f1=(union Cyc_Absyn_Nmspace_union)({union Cyc_Absyn_Nmspace_union _tmp43B;(
_tmp43B.Loc_n).tag=0;_tmp43B;});_tmp439->f2=({struct _dyneither_ptr*_tmp43A=
_cycalloc(sizeof(*_tmp43A));_tmp43A[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp43A;});_tmp439;}),(
void*)0,0);yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp437;(
_tmp437.YY18).tag=22;(_tmp437.YY18).f1=({struct Cyc_List_List*_tmp438=_cycalloc(
sizeof(*_tmp438));_tmp438->hd=Cyc_Absyn_alias_decl(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),tv,vd,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp438->tl=0;
_tmp438;});_tmp437;});break;}case 38: _LL226: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 39: _LL227: yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp440;(_tmp440.YY18).tag=22;(
_tmp440.YY18).f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_imp_append)(Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]));_tmp440;});break;case 40: _LL228: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp441;(_tmp441.YY19).tag=23;(_tmp441.YY19).f1=({struct
Cyc_Parse_Declaration_spec*_tmp442=_cycalloc(sizeof(*_tmp442));_tmp442->sc=({
struct Cyc_Core_Opt*_tmp443=_cycalloc(sizeof(*_tmp443));_tmp443->v=(void*)Cyc_yyget_YY22(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp443;});_tmp442->tq=
Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp442->type_specs=
0;_tmp442->is_inline=0;_tmp442->attributes=0;_tmp442;});_tmp441;});break;case 41:
_LL229: if((Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->sc
!= 0)({void*_tmp444=0;Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),({const char*
_tmp445="Only one storage class is allowed in a declaration";_tag_dyneither(
_tmp445,sizeof(char),51);}),_tag_dyneither(_tmp444,sizeof(void*),0));});yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp446;(_tmp446.YY19).tag=23;(
_tmp446.YY19).f1=({struct Cyc_Parse_Declaration_spec*_tmp447=_cycalloc(sizeof(*
_tmp447));_tmp447->sc=({struct Cyc_Core_Opt*_tmp448=_cycalloc(sizeof(*_tmp448));
_tmp448->v=(void*)Cyc_yyget_YY22(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp448;});_tmp447->tq=(Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->tq;_tmp447->type_specs=(
Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->type_specs;
_tmp447->is_inline=(Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->is_inline;_tmp447->attributes=(Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes;_tmp447;});
_tmp446;});break;case 42: _LL22A: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp449;(_tmp449.YY19).tag=23;(_tmp449.YY19).f1=({struct Cyc_Parse_Declaration_spec*
_tmp44A=_cycalloc(sizeof(*_tmp44A));_tmp44A->sc=0;_tmp44A->tq=Cyc_Absyn_empty_tqual(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp44A->type_specs=({struct Cyc_List_List*_tmp44B=
_cycalloc(sizeof(*_tmp44B));_tmp44B->hd=(void*)Cyc_yyget_YY23(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp44B->tl=0;_tmp44B;});
_tmp44A->is_inline=0;_tmp44A->attributes=0;_tmp44A;});_tmp449;});break;case 43:
_LL22B: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp44C;(_tmp44C.YY19).tag=
23;(_tmp44C.YY19).f1=({struct Cyc_Parse_Declaration_spec*_tmp44D=_cycalloc(
sizeof(*_tmp44D));_tmp44D->sc=(Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->sc;_tmp44D->tq=(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tq;_tmp44D->type_specs=({
struct Cyc_List_List*_tmp44E=_cycalloc(sizeof(*_tmp44E));_tmp44E->hd=(void*)Cyc_yyget_YY23(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp44E->tl=(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->type_specs;_tmp44E;});
_tmp44D->is_inline=(Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->is_inline;_tmp44D->attributes=(Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes;_tmp44D;});
_tmp44C;});break;case 44: _LL22C: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp44F;(_tmp44F.YY19).tag=23;(_tmp44F.YY19).f1=({struct Cyc_Parse_Declaration_spec*
_tmp450=_cycalloc(sizeof(*_tmp450));_tmp450->sc=0;_tmp450->tq=Cyc_yyget_YY25(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp450->type_specs=0;
_tmp450->is_inline=0;_tmp450->attributes=0;_tmp450;});_tmp44F;});break;case 45:
_LL22D: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp451;(_tmp451.YY19).tag=
23;(_tmp451.YY19).f1=({struct Cyc_Parse_Declaration_spec*_tmp452=_cycalloc(
sizeof(*_tmp452));_tmp452->sc=(Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->sc;_tmp452->tq=Cyc_Absyn_combine_tqual(
Cyc_yyget_YY25(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tq);_tmp452->type_specs=(
Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->type_specs;
_tmp452->is_inline=(Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->is_inline;_tmp452->attributes=(Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes;_tmp452;});
_tmp451;});break;case 46: _LL22E: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp453;(_tmp453.YY19).tag=23;(_tmp453.YY19).f1=({struct Cyc_Parse_Declaration_spec*
_tmp454=_cycalloc(sizeof(*_tmp454));_tmp454->sc=0;_tmp454->tq=Cyc_Absyn_empty_tqual(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp454->type_specs=0;_tmp454->is_inline=1;_tmp454->attributes=
0;_tmp454;});_tmp453;});break;case 47: _LL22F: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp455;(_tmp455.YY19).tag=23;(_tmp455.YY19).f1=({struct
Cyc_Parse_Declaration_spec*_tmp456=_cycalloc(sizeof(*_tmp456));_tmp456->sc=(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->sc;_tmp456->tq=(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tq;_tmp456->type_specs=(
Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->type_specs;
_tmp456->is_inline=1;_tmp456->attributes=(Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes;_tmp456;});
_tmp455;});break;case 48: _LL230: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp457;(_tmp457.YY19).tag=23;(_tmp457.YY19).f1=({struct Cyc_Parse_Declaration_spec*
_tmp458=_cycalloc(sizeof(*_tmp458));_tmp458->sc=0;_tmp458->tq=Cyc_Absyn_empty_tqual(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp458->type_specs=0;_tmp458->is_inline=0;_tmp458->attributes=
Cyc_yyget_YY45(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp458;});
_tmp457;});break;case 49: _LL231: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp459;(_tmp459.YY19).tag=23;(_tmp459.YY19).f1=({struct Cyc_Parse_Declaration_spec*
_tmp45A=_cycalloc(sizeof(*_tmp45A));_tmp45A->sc=(Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->sc;_tmp45A->tq=(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tq;_tmp45A->type_specs=(
Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->type_specs;
_tmp45A->is_inline=(Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->is_inline;_tmp45A->attributes=Cyc_List_imp_append(Cyc_yyget_YY45(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes);_tmp45A;});
_tmp459;});break;case 50: _LL232: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp45B;(_tmp45B.YY22).tag=26;(_tmp45B.YY22).f1=(void*)((void*)4);_tmp45B;});
break;case 51: _LL233: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp45C;(_tmp45C.YY22).tag=26;(_tmp45C.YY22).f1=(void*)((void*)5);_tmp45C;});
break;case 52: _LL234: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp45D;(_tmp45D.YY22).tag=26;(_tmp45D.YY22).f1=(void*)((void*)3);_tmp45D;});
break;case 53: _LL235: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp45E;(_tmp45E.YY22).tag=26;(_tmp45E.YY22).f1=(void*)((void*)1);_tmp45E;});
break;case 54: _LL236: if(Cyc_strcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),({const char*_tmp45F="C";
_tag_dyneither(_tmp45F,sizeof(char),2);}))!= 0)Cyc_Parse_err(({const char*_tmp460="only extern or extern \"C\" is allowed";
_tag_dyneither(_tmp460,sizeof(char),37);}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp461;(_tmp461.YY22).tag=26;(_tmp461.YY22).f1=(void*)((
void*)2);_tmp461;});break;case 55: _LL237: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp462;(_tmp462.YY22).tag=26;(_tmp462.YY22).f1=(void*)((void*)0);_tmp462;});
break;case 56: _LL238: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp463;(_tmp463.YY22).tag=26;(_tmp463.YY22).f1=(void*)((void*)6);_tmp463;});
break;case 57: _LL239: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp464;(_tmp464.YY45).tag=49;(_tmp464.YY45).f1=0;_tmp464;});break;case 58: _LL23A:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 59:
_LL23B: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)];break;
case 60: _LL23C: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp465;(
_tmp465.YY45).tag=49;(_tmp465.YY45).f1=({struct Cyc_List_List*_tmp466=_cycalloc(
sizeof(*_tmp466));_tmp466->hd=(void*)Cyc_yyget_YY46(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp466->tl=0;_tmp466;});
_tmp465;});break;case 61: _LL23D: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp467;(_tmp467.YY45).tag=49;(_tmp467.YY45).f1=({struct Cyc_List_List*_tmp468=
_cycalloc(sizeof(*_tmp468));_tmp468->hd=(void*)Cyc_yyget_YY46(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp468->tl=Cyc_yyget_YY45(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp468;});_tmp467;});
break;case 62: _LL23E: {static struct Cyc_Absyn_Aligned_att_struct att_aligned={1,- 1};
static struct _tuple19 att_map[17]={{{_tmp46D,_tmp46D,_tmp46D + 8},(void*)0},{{
_tmp46E,_tmp46E,_tmp46E + 6},(void*)1},{{_tmp46F,_tmp46F,_tmp46F + 9},(void*)2},{{
_tmp470,_tmp470,_tmp470 + 9},(void*)3},{{_tmp471,_tmp471,_tmp471 + 6},(void*)4},{{
_tmp472,_tmp472,_tmp472 + 8},(void*)& att_aligned},{{_tmp473,_tmp473,_tmp473 + 7},(
void*)5},{{_tmp474,_tmp474,_tmp474 + 7},(void*)7},{{_tmp475,_tmp475,_tmp475 + 7},(
void*)8},{{_tmp476,_tmp476,_tmp476 + 5},(void*)9},{{_tmp477,_tmp477,_tmp477 + 10},(
void*)10},{{_tmp478,_tmp478,_tmp478 + 10},(void*)11},{{_tmp479,_tmp479,_tmp479 + 
23},(void*)12},{{_tmp47A,_tmp47A,_tmp47A + 12},(void*)13},{{_tmp47B,_tmp47B,
_tmp47B + 11},(void*)14},{{_tmp47C,_tmp47C,_tmp47C + 22},(void*)15},{{_tmp47D,
_tmp47D,_tmp47D + 5},(void*)16}};struct _dyneither_ptr _tmp469=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);if((((
_get_dyneither_size(_tmp469,sizeof(char))> 4  && *((const char*)
_check_dyneither_subscript(_tmp469,sizeof(char),0))== '_') && *((const char*)
_check_dyneither_subscript(_tmp469,sizeof(char),1))== '_') && *((const char*)
_check_dyneither_subscript(_tmp469,sizeof(char),(int)(_get_dyneither_size(
_tmp469,sizeof(char))- 2)))== '_') && *((const char*)_check_dyneither_subscript(
_tmp469,sizeof(char),(int)(_get_dyneither_size(_tmp469,sizeof(char))- 3)))== '_')
_tmp469=(struct _dyneither_ptr)Cyc_substring((struct _dyneither_ptr)_tmp469,2,
_get_dyneither_size(_tmp469,sizeof(char))- 5);{int i=0;for(0;i < 17;++ i){if(Cyc_strcmp((
struct _dyneither_ptr)_tmp469,(struct _dyneither_ptr)(att_map[i]).f1)== 0){yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp46A;(_tmp46A.YY46).tag=50;(
_tmp46A.YY46).f1=(void*)(att_map[i]).f2;_tmp46A;});break;}}if(i == 17){Cyc_Parse_err(({
const char*_tmp46B="unrecognized attribute";_tag_dyneither(_tmp46B,sizeof(char),
23);}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp46C;(_tmp46C.YY46).tag=50;(_tmp46C.YY46).f1=(void*)((void*)1);_tmp46C;});}
break;}}case 63: _LL23F: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp47F;(_tmp47F.YY46).tag=50;(_tmp47F.YY46).f1=(void*)((void*)4);_tmp47F;});
break;case 64: _LL240: {struct _dyneither_ptr _tmp480=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);int _tmp482;struct
_tuple11 _tmp481=Cyc_yyget_Int_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp482=_tmp481.f2;{void*a;if(Cyc_zstrcmp((struct
_dyneither_ptr)_tmp480,({const char*_tmp483="regparm";_tag_dyneither(_tmp483,
sizeof(char),8);}))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp480,({const char*
_tmp484="__regparm__";_tag_dyneither(_tmp484,sizeof(char),12);}))== 0){if(
_tmp482 < 0  || _tmp482 > 3)Cyc_Parse_err(({const char*_tmp485="regparm requires value between 0 and 3";
_tag_dyneither(_tmp485,sizeof(char),39);}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));a=(void*)({
struct Cyc_Absyn_Regparm_att_struct*_tmp486=_cycalloc_atomic(sizeof(*_tmp486));
_tmp486[0]=({struct Cyc_Absyn_Regparm_att_struct _tmp487;_tmp487.tag=0;_tmp487.f1=
_tmp482;_tmp487;});_tmp486;});}else{if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp480,({
const char*_tmp488="aligned";_tag_dyneither(_tmp488,sizeof(char),8);}))== 0  || 
Cyc_zstrcmp((struct _dyneither_ptr)_tmp480,({const char*_tmp489="__aligned__";
_tag_dyneither(_tmp489,sizeof(char),12);}))== 0){if(_tmp482 < 0)Cyc_Parse_err(({
const char*_tmp48A="aligned requires positive power of two";_tag_dyneither(
_tmp48A,sizeof(char),39);}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));{
unsigned int j=(unsigned int)_tmp482;for(0;(j & 1)== 0;j=j >> 1){;}j=j >> 1;if(j != 0)
Cyc_Parse_err(({const char*_tmp48B="aligned requires positive power of two";
_tag_dyneither(_tmp48B,sizeof(char),39);}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));a=(void*)({
struct Cyc_Absyn_Aligned_att_struct*_tmp48C=_cycalloc_atomic(sizeof(*_tmp48C));
_tmp48C[0]=({struct Cyc_Absyn_Aligned_att_struct _tmp48D;_tmp48D.tag=1;_tmp48D.f1=
_tmp482;_tmp48D;});_tmp48C;});}}else{if(Cyc_zstrcmp((struct _dyneither_ptr)
_tmp480,({const char*_tmp48E="initializes";_tag_dyneither(_tmp48E,sizeof(char),12);}))
== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp480,({const char*_tmp48F="__initializes__";
_tag_dyneither(_tmp48F,sizeof(char),16);}))== 0)a=(void*)({struct Cyc_Absyn_Initializes_att_struct*
_tmp490=_cycalloc_atomic(sizeof(*_tmp490));_tmp490[0]=({struct Cyc_Absyn_Initializes_att_struct
_tmp491;_tmp491.tag=4;_tmp491.f1=_tmp482;_tmp491;});_tmp490;});else{Cyc_Parse_err(({
const char*_tmp492="unrecognized attribute";_tag_dyneither(_tmp492,sizeof(char),
23);}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line));a=(void*)1;}}}yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp493;(_tmp493.YY46).tag=50;(_tmp493.YY46).f1=(void*)a;
_tmp493;});break;}}case 65: _LL241: {struct _dyneither_ptr _tmp494=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);struct
_dyneither_ptr _tmp495=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);void*a;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp494,({
const char*_tmp496="section";_tag_dyneither(_tmp496,sizeof(char),8);}))== 0  || 
Cyc_zstrcmp((struct _dyneither_ptr)_tmp494,({const char*_tmp497="__section__";
_tag_dyneither(_tmp497,sizeof(char),12);}))== 0)a=(void*)({struct Cyc_Absyn_Section_att_struct*
_tmp498=_cycalloc(sizeof(*_tmp498));_tmp498[0]=({struct Cyc_Absyn_Section_att_struct
_tmp499;_tmp499.tag=2;_tmp499.f1=_tmp495;_tmp499;});_tmp498;});else{Cyc_Parse_err(({
const char*_tmp49A="unrecognized attribute";_tag_dyneither(_tmp49A,sizeof(char),
23);}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line));a=(void*)1;}yyval=(union Cyc_YYSTYPE_union)({union
Cyc_YYSTYPE_union _tmp49B;(_tmp49B.YY46).tag=50;(_tmp49B.YY46).f1=(void*)a;
_tmp49B;});break;}case 66: _LL242: {struct _dyneither_ptr _tmp49C=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);struct
_dyneither_ptr _tmp49D=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);void*a;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp49C,({
const char*_tmp49E="__mode__";_tag_dyneither(_tmp49E,sizeof(char),9);}))== 0)a=(
void*)({struct Cyc_Absyn_Mode_att_struct*_tmp49F=_cycalloc(sizeof(*_tmp49F));
_tmp49F[0]=({struct Cyc_Absyn_Mode_att_struct _tmp4A0;_tmp4A0.tag=5;_tmp4A0.f1=
_tmp49D;_tmp4A0;});_tmp49F;});else{Cyc_Parse_err(({const char*_tmp4A1="unrecognized attribute";
_tag_dyneither(_tmp4A1,sizeof(char),23);}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));a=(void*)1;}
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp4A2;(_tmp4A2.YY46).tag=
50;(_tmp4A2.YY46).f1=(void*)a;_tmp4A2;});break;}case 67: _LL243: {struct
_dyneither_ptr _tmp4A3=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 7)]);struct _dyneither_ptr _tmp4A4=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 5)]);int _tmp4A6;struct
_tuple11 _tmp4A5=Cyc_yyget_Int_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]);_tmp4A6=_tmp4A5.f2;{int _tmp4A8;struct _tuple11 _tmp4A7=Cyc_yyget_Int_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp4A8=_tmp4A7.f2;{
void*a=(void*)1;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp4A3,({const char*_tmp4A9="format";
_tag_dyneither(_tmp4A9,sizeof(char),7);}))== 0  || Cyc_zstrcmp((struct
_dyneither_ptr)_tmp4A3,({const char*_tmp4AA="__format__";_tag_dyneither(_tmp4AA,
sizeof(char),11);}))== 0){if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp4A4,({const
char*_tmp4AB="printf";_tag_dyneither(_tmp4AB,sizeof(char),7);}))== 0)a=(void*)({
struct Cyc_Absyn_Format_att_struct*_tmp4AC=_cycalloc(sizeof(*_tmp4AC));_tmp4AC[0]=({
struct Cyc_Absyn_Format_att_struct _tmp4AD;_tmp4AD.tag=3;_tmp4AD.f1=(void*)((void*)
0);_tmp4AD.f2=_tmp4A6;_tmp4AD.f3=_tmp4A8;_tmp4AD;});_tmp4AC;});else{if(Cyc_zstrcmp((
struct _dyneither_ptr)_tmp4A4,({const char*_tmp4AE="scanf";_tag_dyneither(_tmp4AE,
sizeof(char),6);}))== 0)a=(void*)({struct Cyc_Absyn_Format_att_struct*_tmp4AF=
_cycalloc(sizeof(*_tmp4AF));_tmp4AF[0]=({struct Cyc_Absyn_Format_att_struct
_tmp4B0;_tmp4B0.tag=3;_tmp4B0.f1=(void*)((void*)1);_tmp4B0.f2=_tmp4A6;_tmp4B0.f3=
_tmp4A8;_tmp4B0;});_tmp4AF;});else{Cyc_Parse_err(({const char*_tmp4B1="unrecognized format type";
_tag_dyneither(_tmp4B1,sizeof(char),25);}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).last_line));}}}else{Cyc_Parse_err(({
const char*_tmp4B2="unrecognized attribute";_tag_dyneither(_tmp4B2,sizeof(char),
23);}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 7)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));}yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp4B3;(_tmp4B3.YY46).tag=50;(_tmp4B3.YY46).f1=(void*)a;_tmp4B3;});break;}}}
case 68: _LL244: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];
break;case 69: _LL245: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp4B4;(_tmp4B4.YY23).tag=27;(_tmp4B4.YY23).f1=(void*)Cyc_Parse_type_spec((void*)({
struct Cyc_Absyn_TypedefType_struct*_tmp4B5=_cycalloc(sizeof(*_tmp4B5));_tmp4B5[0]=({
struct Cyc_Absyn_TypedefType_struct _tmp4B6;_tmp4B6.tag=16;_tmp4B6.f1=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp4B6.f2=Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp4B6.f3=0;_tmp4B6.f4=
0;_tmp4B6;});_tmp4B5;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp4B4;});
break;case 70: _LL246: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp4B7;(_tmp4B7.YY23).tag=27;(_tmp4B7.YY23).f1=(void*)Cyc_Parse_type_spec((void*)
0,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp4B7;});break;case 71: _LL247: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp4B8;(_tmp4B8.YY23).tag=27;(_tmp4B8.YY23).f1=(void*)Cyc_Parse_type_spec(
Cyc_Absyn_new_evar(0,0),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp4B8;});
break;case 72: _LL248: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp4B9;(_tmp4B9.YY23).tag=27;(_tmp4B9.YY23).f1=(void*)Cyc_Parse_type_spec(Cyc_Absyn_new_evar(
Cyc_Tcutil_kind_to_opt(Cyc_yyget_YY43(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)])),0),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp4B9;});
break;case 73: _LL249: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp4BA;(_tmp4BA.YY23).tag=27;(_tmp4BA.YY23).f1=(void*)Cyc_Parse_type_spec(Cyc_Absyn_char_typ,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp4BA;});break;case 74: _LL24A: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp4BB;(_tmp4BB.YY23).tag=27;(_tmp4BB.YY23).f1=(void*)((
void*)({struct Cyc_Parse_Short_spec_struct*_tmp4BC=_cycalloc(sizeof(*_tmp4BC));
_tmp4BC[0]=({struct Cyc_Parse_Short_spec_struct _tmp4BD;_tmp4BD.tag=2;_tmp4BD.f1=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp4BD;});_tmp4BC;}));_tmp4BB;});break;case 75: _LL24B:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp4BE;(_tmp4BE.YY23).tag=
27;(_tmp4BE.YY23).f1=(void*)Cyc_Parse_type_spec(Cyc_Absyn_sint_typ,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp4BE;});
break;case 76: _LL24C: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp4BF;(_tmp4BF.YY23).tag=27;(_tmp4BF.YY23).f1=(void*)((void*)({struct Cyc_Parse_Long_spec_struct*
_tmp4C0=_cycalloc(sizeof(*_tmp4C0));_tmp4C0[0]=({struct Cyc_Parse_Long_spec_struct
_tmp4C1;_tmp4C1.tag=3;_tmp4C1.f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp4C1;});
_tmp4C0;}));_tmp4BF;});break;case 77: _LL24D: yyval=(union Cyc_YYSTYPE_union)({union
Cyc_YYSTYPE_union _tmp4C2;(_tmp4C2.YY23).tag=27;(_tmp4C2.YY23).f1=(void*)Cyc_Parse_type_spec(
Cyc_Absyn_float_typ,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp4C2;});
break;case 78: _LL24E: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp4C3;(_tmp4C3.YY23).tag=27;(_tmp4C3.YY23).f1=(void*)Cyc_Parse_type_spec(Cyc_Absyn_double_typ(
0),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp4C3;});break;case 79: _LL24F: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp4C4;(_tmp4C4.YY23).tag=27;(_tmp4C4.YY23).f1=(void*)((
void*)({struct Cyc_Parse_Signed_spec_struct*_tmp4C5=_cycalloc(sizeof(*_tmp4C5));
_tmp4C5[0]=({struct Cyc_Parse_Signed_spec_struct _tmp4C6;_tmp4C6.tag=0;_tmp4C6.f1=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp4C6;});_tmp4C5;}));_tmp4C4;});break;case 80: _LL250:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp4C7;(_tmp4C7.YY23).tag=
27;(_tmp4C7.YY23).f1=(void*)((void*)({struct Cyc_Parse_Unsigned_spec_struct*
_tmp4C8=_cycalloc(sizeof(*_tmp4C8));_tmp4C8[0]=({struct Cyc_Parse_Unsigned_spec_struct
_tmp4C9;_tmp4C9.tag=1;_tmp4C9.f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp4C9;});
_tmp4C8;}));_tmp4C7;});break;case 81: _LL251: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 82: _LL252: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 83: _LL253:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 84:
_LL254: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp4CA;(_tmp4CA.YY23).tag=
27;(_tmp4CA.YY23).f1=(void*)Cyc_Parse_type_spec(Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp4CA;});
break;case 85: _LL255: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp4CB;(_tmp4CB.YY23).tag=27;(_tmp4CB.YY23).f1=(void*)Cyc_Parse_type_spec((void*)({
struct Cyc_Absyn_TupleType_struct*_tmp4CC=_cycalloc(sizeof(*_tmp4CC));_tmp4CC[0]=({
struct Cyc_Absyn_TupleType_struct _tmp4CD;_tmp4CD.tag=9;_tmp4CD.f1=((struct Cyc_List_List*(*)(
struct _tuple9*(*f)(struct Cyc_Position_Segment*,struct _tuple2*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_get_tqual_typ,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));_tmp4CD;});_tmp4CC;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp4CB;});break;case 86: _LL256: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp4CE;(_tmp4CE.YY23).tag=27;(_tmp4CE.YY23).f1=(void*)Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_RgnHandleType_struct*_tmp4CF=_cycalloc(sizeof(*_tmp4CF));
_tmp4CF[0]=({struct Cyc_Absyn_RgnHandleType_struct _tmp4D0;_tmp4D0.tag=14;_tmp4D0.f1=(
void*)Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp4D0;});_tmp4CF;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp4CE;});
break;case 87: _LL257: {void*_tmp4D1=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,
0);yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp4D2;(_tmp4D2.YY23).tag=
27;(_tmp4D2.YY23).f1=(void*)Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_DynRgnType_struct*
_tmp4D3=_cycalloc(sizeof(*_tmp4D3));_tmp4D3[0]=({struct Cyc_Absyn_DynRgnType_struct
_tmp4D4;_tmp4D4.tag=15;_tmp4D4.f1=(void*)Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp4D4.f2=(void*)
_tmp4D1;_tmp4D4;});_tmp4D3;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp4D2;});
break;}case 88: _LL258: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp4D5;(_tmp4D5.YY23).tag=27;(_tmp4D5.YY23).f1=(void*)Cyc_Parse_type_spec((void*)({
struct Cyc_Absyn_DynRgnType_struct*_tmp4D6=_cycalloc(sizeof(*_tmp4D6));_tmp4D6[0]=({
struct Cyc_Absyn_DynRgnType_struct _tmp4D7;_tmp4D7.tag=15;_tmp4D7.f1=(void*)Cyc_yyget_YY44(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp4D7.f2=(void*)
Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp4D7;});_tmp4D6;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp4D5;});
break;case 89: _LL259: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp4D8;(_tmp4D8.YY23).tag=27;(_tmp4D8.YY23).f1=(void*)Cyc_Parse_type_spec((void*)({
struct Cyc_Absyn_TagType_struct*_tmp4D9=_cycalloc(sizeof(*_tmp4D9));_tmp4D9[0]=({
struct Cyc_Absyn_TagType_struct _tmp4DA;_tmp4DA.tag=18;_tmp4DA.f1=(void*)Cyc_yyget_YY44(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp4DA;});_tmp4D9;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp4D8;});break;case 90: _LL25A: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp4DB;(_tmp4DB.YY23).tag=27;(_tmp4DB.YY23).f1=(void*)Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_TagType_struct*_tmp4DC=_cycalloc(sizeof(*_tmp4DC));
_tmp4DC[0]=({struct Cyc_Absyn_TagType_struct _tmp4DD;_tmp4DD.tag=18;_tmp4DD.f1=(
void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ik,0);_tmp4DD;});
_tmp4DC;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp4DB;});break;case 91: _LL25B: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp4DE;(_tmp4DE.YY23).tag=27;(_tmp4DE.YY23).f1=(void*)Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_ValueofType_struct*_tmp4DF=_cycalloc(sizeof(*_tmp4DF));
_tmp4DF[0]=({struct Cyc_Absyn_ValueofType_struct _tmp4E0;_tmp4E0.tag=17;_tmp4E0.f1=
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp4E0;});_tmp4DF;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp4DE;});
break;case 92: _LL25C: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp4E1;(_tmp4E1.YY43).tag=47;(_tmp4E1.YY43).f1=(void*)Cyc_Parse_id_to_kind(Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp4E1;});
break;case 93: _LL25D: {struct Cyc_Position_Segment*loc=Cyc_Absyn_porting_c_code?
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line): 0;yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp4E2;(_tmp4E2.YY25).tag=29;(_tmp4E2.YY25).f1=({struct Cyc_Absyn_Tqual _tmp4E3;
_tmp4E3.print_const=1;_tmp4E3.q_volatile=0;_tmp4E3.q_restrict=0;_tmp4E3.real_const=
1;_tmp4E3.loc=loc;_tmp4E3;});_tmp4E2;});break;}case 94: _LL25E: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp4E4;(_tmp4E4.YY25).tag=29;(_tmp4E4.YY25).f1=({struct
Cyc_Absyn_Tqual _tmp4E5;_tmp4E5.print_const=0;_tmp4E5.q_volatile=1;_tmp4E5.q_restrict=
0;_tmp4E5.real_const=0;_tmp4E5.loc=0;_tmp4E5;});_tmp4E4;});break;case 95: _LL25F:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp4E6;(_tmp4E6.YY25).tag=
29;(_tmp4E6.YY25).f1=({struct Cyc_Absyn_Tqual _tmp4E7;_tmp4E7.print_const=0;
_tmp4E7.q_volatile=0;_tmp4E7.q_restrict=1;_tmp4E7.real_const=0;_tmp4E7.loc=0;
_tmp4E7;});_tmp4E6;});break;case 96: _LL260: yyval=(union Cyc_YYSTYPE_union)({union
Cyc_YYSTYPE_union _tmp4E8;(_tmp4E8.YY23).tag=27;(_tmp4E8.YY23).f1=(void*)((void*)({
struct Cyc_Parse_Decl_spec_struct*_tmp4E9=_cycalloc(sizeof(*_tmp4E9));_tmp4E9[0]=({
struct Cyc_Parse_Decl_spec_struct _tmp4EA;_tmp4EA.tag=5;_tmp4EA.f1=({struct Cyc_Absyn_Decl*
_tmp4EB=_cycalloc(sizeof(*_tmp4EB));_tmp4EB->r=(void*)((void*)({struct Cyc_Absyn_Enum_d_struct*
_tmp4EC=_cycalloc(sizeof(*_tmp4EC));_tmp4EC[0]=({struct Cyc_Absyn_Enum_d_struct
_tmp4ED;_tmp4ED.tag=8;_tmp4ED.f1=({struct Cyc_Absyn_Enumdecl*_tmp4EE=_cycalloc(
sizeof(*_tmp4EE));_tmp4EE->sc=(void*)((void*)2);_tmp4EE->name=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp4EE->fields=({
struct Cyc_Core_Opt*_tmp4EF=_cycalloc(sizeof(*_tmp4EF));_tmp4EF->v=Cyc_yyget_YY48(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp4EF;});_tmp4EE;});
_tmp4ED;});_tmp4EC;}));_tmp4EB->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp4EB;});
_tmp4EA;});_tmp4E9;}));_tmp4E8;});break;case 97: _LL261: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp4F0;(_tmp4F0.YY23).tag=27;(_tmp4F0.YY23).f1=(void*)Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_EnumType_struct*_tmp4F1=_cycalloc(sizeof(*_tmp4F1));
_tmp4F1[0]=({struct Cyc_Absyn_EnumType_struct _tmp4F2;_tmp4F2.tag=12;_tmp4F2.f1=
Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
_tmp4F2.f2=0;_tmp4F2;});_tmp4F1;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp4F0;});
break;case 98: _LL262: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp4F3;(_tmp4F3.YY23).tag=27;(_tmp4F3.YY23).f1=(void*)((void*)({struct Cyc_Parse_Type_spec_struct*
_tmp4F4=_cycalloc(sizeof(*_tmp4F4));_tmp4F4[0]=({struct Cyc_Parse_Type_spec_struct
_tmp4F5;_tmp4F5.tag=4;_tmp4F5.f1=(void*)((void*)({struct Cyc_Absyn_AnonEnumType_struct*
_tmp4F6=_cycalloc(sizeof(*_tmp4F6));_tmp4F6[0]=({struct Cyc_Absyn_AnonEnumType_struct
_tmp4F7;_tmp4F7.tag=13;_tmp4F7.f1=Cyc_yyget_YY48(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp4F7;});_tmp4F6;}));
_tmp4F5.f2=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp4F5;});_tmp4F4;}));_tmp4F3;});break;case 99: _LL263:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp4F8;(_tmp4F8.YY47).tag=
51;(_tmp4F8.YY47).f1=({struct Cyc_Absyn_Enumfield*_tmp4F9=_cycalloc(sizeof(*
_tmp4F9));_tmp4F9->name=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp4F9->tag=0;_tmp4F9->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp4F9;});
_tmp4F8;});break;case 100: _LL264: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp4FA;(_tmp4FA.YY47).tag=51;(_tmp4FA.YY47).f1=({struct Cyc_Absyn_Enumfield*
_tmp4FB=_cycalloc(sizeof(*_tmp4FB));_tmp4FB->name=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp4FB->tag=(struct Cyc_Absyn_Exp*)
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp4FB->loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp4FB;});_tmp4FA;});break;case 101: _LL265: yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp4FC;(_tmp4FC.YY48).tag=52;(
_tmp4FC.YY48).f1=({struct Cyc_List_List*_tmp4FD=_cycalloc(sizeof(*_tmp4FD));
_tmp4FD->hd=Cyc_yyget_YY47(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp4FD->tl=0;_tmp4FD;});_tmp4FC;});break;case 102: _LL266: yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp4FE;(_tmp4FE.YY48).tag=52;(
_tmp4FE.YY48).f1=({struct Cyc_List_List*_tmp4FF=_cycalloc(sizeof(*_tmp4FF));
_tmp4FF->hd=Cyc_yyget_YY47(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp4FF->tl=0;_tmp4FF;});_tmp4FE;});break;case 103: _LL267:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp500;(_tmp500.YY48).tag=
52;(_tmp500.YY48).f1=({struct Cyc_List_List*_tmp501=_cycalloc(sizeof(*_tmp501));
_tmp501->hd=Cyc_yyget_YY47(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]);_tmp501->tl=Cyc_yyget_YY48(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp501;});_tmp500;});
break;case 104: _LL268: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp502;(_tmp502.YY23).tag=27;(_tmp502.YY23).f1=(void*)Cyc_Parse_type_spec((void*)({
struct Cyc_Absyn_AnonAggrType_struct*_tmp503=_cycalloc(sizeof(*_tmp503));_tmp503[
0]=({struct Cyc_Absyn_AnonAggrType_struct _tmp504;_tmp504.tag=11;_tmp504.f1=(void*)
Cyc_yyget_YY24(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);
_tmp504.f2=Cyc_yyget_YY26(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset
- 1)]);_tmp504;});_tmp503;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp502;});
break;case 105: _LL269: {struct Cyc_List_List*_tmp505=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).last_line),Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]));struct Cyc_List_List*
_tmp506=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line),Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]));yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp507;(_tmp507.YY23).tag=27;(_tmp507.YY23).f1=(void*)((void*)({struct Cyc_Parse_Decl_spec_struct*
_tmp508=_cycalloc(sizeof(*_tmp508));_tmp508[0]=({struct Cyc_Parse_Decl_spec_struct
_tmp509;_tmp509.tag=5;_tmp509.f1=Cyc_Absyn_aggr_decl(Cyc_yyget_YY24(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 7)]),(void*)2,Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),_tmp505,(struct Cyc_Absyn_AggrdeclImpl*)
Cyc_Absyn_aggrdecl_impl(_tmp506,Cyc_yyget_YY50(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY26(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])),0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp509;});
_tmp508;}));_tmp507;});break;}case 106: _LL26A: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp50A;(_tmp50A.YY23).tag=27;(_tmp50A.YY23).f1=(void*)Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_AggrType_struct*_tmp50B=_cycalloc(sizeof(*_tmp50B));
_tmp50B[0]=({struct Cyc_Absyn_AggrType_struct _tmp50C;_tmp50C.tag=10;_tmp50C.f1=({
struct Cyc_Absyn_AggrInfo _tmp50D;_tmp50D.aggr_info=(union Cyc_Absyn_AggrInfoU_union)({
union Cyc_Absyn_AggrInfoU_union _tmp50E;(_tmp50E.UnknownAggr).tag=0;(_tmp50E.UnknownAggr).f1=(
void*)Cyc_yyget_YY24(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);(
_tmp50E.UnknownAggr).f2=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_tmp50E;});_tmp50D.targs=Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp50D;});_tmp50C;});
_tmp50B;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp50A;});break;case 107: _LL26B: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp50F;(_tmp50F.YY40).tag=44;(_tmp50F.YY40).f1=0;_tmp50F;});
break;case 108: _LL26C: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp510;(_tmp510.YY40).tag=44;(_tmp510.YY40).f1=Cyc_List_imp_rev(Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp510;});break;
case 109: _LL26D: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp511;(
_tmp511.YY24).tag=28;(_tmp511.YY24).f1=(void*)((void*)0);_tmp511;});break;case
110: _LL26E: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp512;(
_tmp512.YY24).tag=28;(_tmp512.YY24).f1=(void*)((void*)1);_tmp512;});break;case
111: _LL26F: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp513;(
_tmp513.YY26).tag=30;(_tmp513.YY26).f1=0;_tmp513;});break;case 112: _LL270: yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp514;(_tmp514.YY26).tag=30;(
_tmp514.YY26).f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_flatten)(((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY27(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])));_tmp514;});break;
case 113: _LL271: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp515;(
_tmp515.YY27).tag=31;(_tmp515.YY27).f1=({struct Cyc_List_List*_tmp516=_cycalloc(
sizeof(*_tmp516));_tmp516->hd=Cyc_yyget_YY26(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp516->tl=0;_tmp516;});_tmp515;});break;case 114: _LL272:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp517;(_tmp517.YY27).tag=
31;(_tmp517.YY27).f1=({struct Cyc_List_List*_tmp518=_cycalloc(sizeof(*_tmp518));
_tmp518->hd=Cyc_yyget_YY26(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp518->tl=Cyc_yyget_YY27(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_tmp518;});_tmp517;});break;case 115: _LL273: yyval=(union
Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp519;(_tmp519.YY21).tag=25;(_tmp519.YY21).f1=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY21(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp519;});break;case
116: _LL274: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp51A;(
_tmp51A.YY21).tag=25;(_tmp51A.YY21).f1=({struct Cyc_List_List*_tmp51B=_cycalloc(
sizeof(*_tmp51B));_tmp51B->hd=Cyc_yyget_YY20(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp51B->tl=0;_tmp51B;});_tmp51A;});break;case 117: _LL275:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp51C;(_tmp51C.YY21).tag=
25;(_tmp51C.YY21).f1=({struct Cyc_List_List*_tmp51D=_cycalloc(sizeof(*_tmp51D));
_tmp51D->hd=Cyc_yyget_YY20(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp51D->tl=Cyc_yyget_YY21(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]);_tmp51D;});_tmp51C;});break;case 118: _LL276: yyval=(union
Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp51E;(_tmp51E.YY20).tag=24;(_tmp51E.YY20).f1=({
struct _tuple15*_tmp51F=_cycalloc(sizeof(*_tmp51F));_tmp51F->f1=Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp51F->f2=0;_tmp51F;});
_tmp51E;});break;case 119: _LL277: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp520;(_tmp520.YY20).tag=24;(_tmp520.YY20).f1=({struct _tuple15*_tmp521=
_cycalloc(sizeof(*_tmp521));_tmp521->f1=Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp521->f2=(struct Cyc_Absyn_Exp*)
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp521;});
_tmp520;});break;case 120: _LL278: {struct _RegionHandle _tmp522=_new_region("temp");
struct _RegionHandle*temp=& _tmp522;_push_region(temp);{struct _tuple16 _tmp524;
struct Cyc_Absyn_Tqual _tmp525;struct Cyc_List_List*_tmp526;struct Cyc_List_List*
_tmp527;struct _tuple16*_tmp523=Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp524=*_tmp523;_tmp525=
_tmp524.f1;_tmp526=_tmp524.f2;_tmp527=_tmp524.f3;if(_tmp525.loc == 0)_tmp525.loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line);{struct Cyc_List_List*_tmp529;struct Cyc_List_List*
_tmp52A;struct _tuple0 _tmp528=((struct _tuple0(*)(struct _RegionHandle*r1,struct
_RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(temp,temp,Cyc_yyget_YY21(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp529=_tmp528.f1;
_tmp52A=_tmp528.f2;{void*_tmp52B=Cyc_Parse_speclist2typ(_tmp526,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line));struct Cyc_List_List*
_tmp52C=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(temp,temp,Cyc_Parse_apply_tmss(
temp,_tmp525,_tmp52B,_tmp529,_tmp527),_tmp52A);yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp52D;(_tmp52D.YY26).tag=30;(_tmp52D.YY26).f1=((struct
Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Position_Segment*,
struct _tuple8*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(
Cyc_Parse_make_aggr_field,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),_tmp52C);
_tmp52D;});}}}_npop_handler(0);break;;_pop_region(temp);}case 121: _LL279: yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp52E;(_tmp52E.YY35).tag=39;(
_tmp52E.YY35).f1=({struct _tuple16*_tmp52F=_cycalloc(sizeof(*_tmp52F));_tmp52F->f1=
Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp52F->f2=({
struct Cyc_List_List*_tmp530=_cycalloc(sizeof(*_tmp530));_tmp530->hd=(void*)Cyc_yyget_YY23(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp530->tl=0;_tmp530;});
_tmp52F->f3=0;_tmp52F;});_tmp52E;});break;case 122: _LL27A: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp531;(_tmp531.YY35).tag=39;(_tmp531.YY35).f1=({struct
_tuple16*_tmp532=_cycalloc(sizeof(*_tmp532));_tmp532->f1=(*Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f1;_tmp532->f2=({struct Cyc_List_List*
_tmp533=_cycalloc(sizeof(*_tmp533));_tmp533->hd=(void*)Cyc_yyget_YY23(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp533->tl=(*Cyc_yyget_YY35(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f2;_tmp533;});_tmp532->f3=(*
Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f3;
_tmp532;});_tmp531;});break;case 123: _LL27B: yyval=(union Cyc_YYSTYPE_union)({union
Cyc_YYSTYPE_union _tmp534;(_tmp534.YY35).tag=39;(_tmp534.YY35).f1=({struct
_tuple16*_tmp535=_cycalloc(sizeof(*_tmp535));_tmp535->f1=Cyc_yyget_YY25(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp535->f2=0;_tmp535->f3=0;
_tmp535;});_tmp534;});break;case 124: _LL27C: yyval=(union Cyc_YYSTYPE_union)({union
Cyc_YYSTYPE_union _tmp536;(_tmp536.YY35).tag=39;(_tmp536.YY35).f1=({struct
_tuple16*_tmp537=_cycalloc(sizeof(*_tmp537));_tmp537->f1=Cyc_Absyn_combine_tqual(
Cyc_yyget_YY25(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(*
Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f1);
_tmp537->f2=(*Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)])).f2;_tmp537->f3=(*Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f3;_tmp537;});_tmp536;});
break;case 125: _LL27D: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp538;(_tmp538.YY35).tag=39;(_tmp538.YY35).f1=({struct _tuple16*_tmp539=
_cycalloc(sizeof(*_tmp539));_tmp539->f1=Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp539->f2=0;
_tmp539->f3=Cyc_yyget_YY45(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp539;});_tmp538;});break;case 126: _LL27E: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp53A;(_tmp53A.YY35).tag=39;(_tmp53A.YY35).f1=({struct
_tuple16*_tmp53B=_cycalloc(sizeof(*_tmp53B));_tmp53B->f1=(*Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f1;_tmp53B->f2=(*Cyc_yyget_YY35(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f2;_tmp53B->f3=Cyc_List_append(
Cyc_yyget_YY45(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(*
Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f3);
_tmp53B;});_tmp53A;});break;case 127: _LL27F: yyval=(union Cyc_YYSTYPE_union)({union
Cyc_YYSTYPE_union _tmp53C;(_tmp53C.YY35).tag=39;(_tmp53C.YY35).f1=({struct
_tuple16*_tmp53D=_cycalloc(sizeof(*_tmp53D));_tmp53D->f1=Cyc_Absyn_empty_tqual(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp53D->f2=({struct Cyc_List_List*_tmp53E=_cycalloc(
sizeof(*_tmp53E));_tmp53E->hd=(void*)Cyc_yyget_YY23(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp53E->tl=0;_tmp53E;});
_tmp53D->f3=0;_tmp53D;});_tmp53C;});break;case 128: _LL280: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp53F;(_tmp53F.YY35).tag=39;(_tmp53F.YY35).f1=({struct
_tuple16*_tmp540=_cycalloc(sizeof(*_tmp540));_tmp540->f1=(*Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f1;_tmp540->f2=({struct Cyc_List_List*
_tmp541=_cycalloc(sizeof(*_tmp541));_tmp541->hd=(void*)Cyc_yyget_YY23(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp541->tl=(*Cyc_yyget_YY35(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f2;_tmp541;});_tmp540->f3=(*
Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f3;
_tmp540;});_tmp53F;});break;case 129: _LL281: yyval=(union Cyc_YYSTYPE_union)({union
Cyc_YYSTYPE_union _tmp542;(_tmp542.YY35).tag=39;(_tmp542.YY35).f1=({struct
_tuple16*_tmp543=_cycalloc(sizeof(*_tmp543));_tmp543->f1=Cyc_yyget_YY25(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp543->f2=0;_tmp543->f3=0;
_tmp543;});_tmp542;});break;case 130: _LL282: yyval=(union Cyc_YYSTYPE_union)({union
Cyc_YYSTYPE_union _tmp544;(_tmp544.YY35).tag=39;(_tmp544.YY35).f1=({struct
_tuple16*_tmp545=_cycalloc(sizeof(*_tmp545));_tmp545->f1=Cyc_Absyn_combine_tqual(
Cyc_yyget_YY25(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(*
Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f1);
_tmp545->f2=(*Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)])).f2;_tmp545->f3=(*Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f3;_tmp545;});_tmp544;});
break;case 131: _LL283: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp546;(_tmp546.YY35).tag=39;(_tmp546.YY35).f1=({struct _tuple16*_tmp547=
_cycalloc(sizeof(*_tmp547));_tmp547->f1=Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp547->f2=0;
_tmp547->f3=Cyc_yyget_YY45(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp547;});_tmp546;});break;case 132: _LL284: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp548;(_tmp548.YY35).tag=39;(_tmp548.YY35).f1=({struct
_tuple16*_tmp549=_cycalloc(sizeof(*_tmp549));_tmp549->f1=(*Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f1;_tmp549->f2=(*Cyc_yyget_YY35(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f2;_tmp549->f3=Cyc_List_append(
Cyc_yyget_YY45(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(*
Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f3);
_tmp549;});_tmp548;});break;case 133: _LL285: yyval=(union Cyc_YYSTYPE_union)({union
Cyc_YYSTYPE_union _tmp54A;(_tmp54A.YY21).tag=25;(_tmp54A.YY21).f1=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY21(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp54A;});break;case 134:
_LL286: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp54B;(_tmp54B.YY21).tag=
25;(_tmp54B.YY21).f1=({struct Cyc_List_List*_tmp54C=_cycalloc(sizeof(*_tmp54C));
_tmp54C->hd=Cyc_yyget_YY20(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp54C->tl=0;_tmp54C;});_tmp54B;});break;case 135: _LL287: yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp54D;(_tmp54D.YY21).tag=25;(
_tmp54D.YY21).f1=({struct Cyc_List_List*_tmp54E=_cycalloc(sizeof(*_tmp54E));
_tmp54E->hd=Cyc_yyget_YY20(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp54E->tl=Cyc_yyget_YY21(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]);_tmp54E;});_tmp54D;});break;case 136: _LL288: yyval=(union
Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp54F;(_tmp54F.YY20).tag=24;(_tmp54F.YY20).f1=({
struct _tuple15*_tmp550=_cycalloc(sizeof(*_tmp550));_tmp550->f1=Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp550->f2=0;_tmp550;});
_tmp54F;});break;case 137: _LL289: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp551;(_tmp551.YY20).tag=24;(_tmp551.YY20).f1=({struct _tuple15*_tmp552=
_cycalloc(sizeof(*_tmp552));_tmp552->f1=({struct Cyc_Parse_Declarator*_tmp553=
_cycalloc(sizeof(*_tmp553));_tmp553->id=({struct _tuple1*_tmp554=_cycalloc(
sizeof(*_tmp554));_tmp554->f1=(union Cyc_Absyn_Nmspace_union)({union Cyc_Absyn_Nmspace_union
_tmp556;(_tmp556.Rel_n).tag=1;(_tmp556.Rel_n).f1=0;_tmp556;});_tmp554->f2=
_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"",sizeof(char),1);
_tmp554;});_tmp553->tms=0;_tmp553;});_tmp552->f2=(struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp552;});_tmp551;});
break;case 138: _LL28A: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp557;(_tmp557.YY20).tag=24;(_tmp557.YY20).f1=({struct _tuple15*_tmp558=
_cycalloc(sizeof(*_tmp558));_tmp558->f1=Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp558->f2=(struct Cyc_Absyn_Exp*)
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp558;});
_tmp557;});break;case 139: _LL28B: {struct Cyc_List_List*_tmp559=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line),Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]));yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp55A;(_tmp55A.YY23).tag=27;(_tmp55A.YY23).f1=(void*)((
void*)({struct Cyc_Parse_Decl_spec_struct*_tmp55B=_cycalloc(sizeof(*_tmp55B));
_tmp55B[0]=({struct Cyc_Parse_Decl_spec_struct _tmp55C;_tmp55C.tag=5;_tmp55C.f1=
Cyc_Absyn_tunion_decl((void*)2,Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),_tmp559,({struct Cyc_Core_Opt*
_tmp55D=_cycalloc(sizeof(*_tmp55D));_tmp55D->v=Cyc_yyget_YY34(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp55D;}),Cyc_yyget_YY31(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),Cyc_yyget_YY31(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 7)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp55C;});
_tmp55B;}));_tmp55A;});break;}case 140: _LL28C: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp55E;(_tmp55E.YY23).tag=27;(_tmp55E.YY23).f1=(void*)Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_TunionType_struct*_tmp55F=_cycalloc(sizeof(*_tmp55F));
_tmp55F[0]=({struct Cyc_Absyn_TunionType_struct _tmp560;_tmp560.tag=2;_tmp560.f1=({
struct Cyc_Absyn_TunionInfo _tmp561;_tmp561.tunion_info=(union Cyc_Absyn_TunionInfoU_union)({
union Cyc_Absyn_TunionInfoU_union _tmp562;(_tmp562.UnknownTunion).tag=0;(_tmp562.UnknownTunion).f1=({
struct Cyc_Absyn_UnknownTunionInfo _tmp563;_tmp563.name=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp563.is_xtunion=Cyc_yyget_YY31(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp563.is_flat=Cyc_yyget_YY31(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp563;});_tmp562;});
_tmp561.targs=Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp561.rgn=Cyc_yyget_YY49(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]);_tmp561;});_tmp560;});_tmp55F;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp55E;});
break;case 141: _LL28D: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp564;(_tmp564.YY23).tag=27;(_tmp564.YY23).f1=(void*)Cyc_Parse_type_spec((void*)({
struct Cyc_Absyn_TunionFieldType_struct*_tmp565=_cycalloc(sizeof(*_tmp565));
_tmp565[0]=({struct Cyc_Absyn_TunionFieldType_struct _tmp566;_tmp566.tag=3;_tmp566.f1=({
struct Cyc_Absyn_TunionFieldInfo _tmp567;_tmp567.field_info=(union Cyc_Absyn_TunionFieldInfoU_union)({
union Cyc_Absyn_TunionFieldInfoU_union _tmp568;(_tmp568.UnknownTunionfield).tag=0;(
_tmp568.UnknownTunionfield).f1=({struct Cyc_Absyn_UnknownTunionFieldInfo _tmp569;
_tmp569.tunion_name=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]);_tmp569.field_name=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp569.is_xtunion=Cyc_yyget_YY31(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]);_tmp569;});_tmp568;});
_tmp567.targs=Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp567;});_tmp566;});_tmp565;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp564;});
break;case 142: _LL28E: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp56A;(_tmp56A.YY31).tag=35;(_tmp56A.YY31).f1=1;_tmp56A;});break;case 143:
_LL28F: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp56B;(_tmp56B.YY31).tag=
35;(_tmp56B.YY31).f1=0;_tmp56B;});break;case 144: _LL290: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp56C;(_tmp56C.YY31).tag=35;(_tmp56C.YY31).f1=0;_tmp56C;});
break;case 145: _LL291: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp56D;(_tmp56D.YY31).tag=35;(_tmp56D.YY31).f1=1;_tmp56D;});break;case 146:
_LL292: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp56E;(_tmp56E.YY34).tag=
38;(_tmp56E.YY34).f1=({struct Cyc_List_List*_tmp56F=_cycalloc(sizeof(*_tmp56F));
_tmp56F->hd=Cyc_yyget_YY33(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp56F->tl=0;_tmp56F;});_tmp56E;});break;case 147: _LL293: yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp570;(_tmp570.YY34).tag=38;(
_tmp570.YY34).f1=({struct Cyc_List_List*_tmp571=_cycalloc(sizeof(*_tmp571));
_tmp571->hd=Cyc_yyget_YY33(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp571->tl=0;_tmp571;});_tmp570;});break;case 148: _LL294:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp572;(_tmp572.YY34).tag=
38;(_tmp572.YY34).f1=({struct Cyc_List_List*_tmp573=_cycalloc(sizeof(*_tmp573));
_tmp573->hd=Cyc_yyget_YY33(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]);_tmp573->tl=Cyc_yyget_YY34(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp573;});_tmp572;});
break;case 149: _LL295: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp574;(_tmp574.YY34).tag=38;(_tmp574.YY34).f1=({struct Cyc_List_List*_tmp575=
_cycalloc(sizeof(*_tmp575));_tmp575->hd=Cyc_yyget_YY33(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp575->tl=Cyc_yyget_YY34(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp575;});_tmp574;});
break;case 150: _LL296: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp576;(_tmp576.YY32).tag=36;(_tmp576.YY32).f1=(void*)((void*)2);_tmp576;});
break;case 151: _LL297: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp577;(_tmp577.YY32).tag=36;(_tmp577.YY32).f1=(void*)((void*)3);_tmp577;});
break;case 152: _LL298: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp578;(_tmp578.YY32).tag=36;(_tmp578.YY32).f1=(void*)((void*)0);_tmp578;});
break;case 153: _LL299: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp579;(_tmp579.YY33).tag=37;(_tmp579.YY33).f1=({struct Cyc_Absyn_Tunionfield*
_tmp57A=_cycalloc(sizeof(*_tmp57A));_tmp57A->name=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp57A->typs=0;_tmp57A->loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp57A->sc=(void*)Cyc_yyget_YY32(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp57A;});_tmp579;});
break;case 154: _LL29A: {struct Cyc_List_List*_tmp57B=((struct Cyc_List_List*(*)(
struct _tuple9*(*f)(struct Cyc_Position_Segment*,struct _tuple2*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_get_tqual_typ,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp57C;(_tmp57C.YY33).tag=37;(_tmp57C.YY33).f1=({struct
Cyc_Absyn_Tunionfield*_tmp57D=_cycalloc(sizeof(*_tmp57D));_tmp57D->name=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp57D->typs=
_tmp57B;_tmp57D->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp57D->sc=(
void*)Cyc_yyget_YY32(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);
_tmp57D;});_tmp57C;});break;}case 155: _LL29B: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 156: _LL29C: yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp57E;(_tmp57E.YY29).tag=33;(
_tmp57E.YY29).f1=({struct Cyc_Parse_Declarator*_tmp57F=_cycalloc(sizeof(*_tmp57F));
_tmp57F->id=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->id;_tmp57F->tms=Cyc_List_imp_append(Cyc_yyget_YY28(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->tms);_tmp57F;});_tmp57E;});
break;case 157: _LL29D: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 158: _LL29E: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp580;(_tmp580.YY29).tag=33;(_tmp580.YY29).f1=({struct Cyc_Parse_Declarator*
_tmp581=_cycalloc(sizeof(*_tmp581));_tmp581->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->id;_tmp581->tms=Cyc_List_imp_append(
Cyc_yyget_YY28(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tms);_tmp581;});
_tmp580;});break;case 159: _LL29F: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp582;(_tmp582.YY29).tag=33;(_tmp582.YY29).f1=({struct Cyc_Parse_Declarator*
_tmp583=_cycalloc(sizeof(*_tmp583));_tmp583->id=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp583->tms=0;_tmp583;});
_tmp582;});break;case 160: _LL2A0: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)];break;case 161: _LL2A1: {struct Cyc_Parse_Declarator*_tmp584=Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp584->tms=({
struct Cyc_List_List*_tmp585=_cycalloc(sizeof(*_tmp585));_tmp585->hd=(void*)((
void*)({struct Cyc_Absyn_Attributes_mod_struct*_tmp586=_cycalloc(sizeof(*_tmp586));
_tmp586[0]=({struct Cyc_Absyn_Attributes_mod_struct _tmp587;_tmp587.tag=5;_tmp587.f1=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line);_tmp587.f2=Cyc_yyget_YY45(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp587;});_tmp586;}));
_tmp585->tl=_tmp584->tms;_tmp585;});yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)];break;}case 162: _LL2A2: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp588;(_tmp588.YY29).tag=33;(_tmp588.YY29).f1=({struct
Cyc_Parse_Declarator*_tmp589=_cycalloc(sizeof(*_tmp589));_tmp589->id=(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id;_tmp589->tms=({
struct Cyc_List_List*_tmp58A=_cycalloc(sizeof(*_tmp58A));_tmp58A->hd=(void*)((
void*)({struct Cyc_Absyn_Carray_mod_struct*_tmp58B=_cycalloc(sizeof(*_tmp58B));
_tmp58B[0]=({struct Cyc_Absyn_Carray_mod_struct _tmp58C;_tmp58C.tag=0;_tmp58C.f1=
Cyc_yyget_YY51(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp58C.f2=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp58C;});_tmp58B;}));_tmp58A->tl=(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp58A;});
_tmp589;});_tmp588;});break;case 163: _LL2A3: yyval=(union Cyc_YYSTYPE_union)({union
Cyc_YYSTYPE_union _tmp58D;(_tmp58D.YY29).tag=33;(_tmp58D.YY29).f1=({struct Cyc_Parse_Declarator*
_tmp58E=_cycalloc(sizeof(*_tmp58E));_tmp58E->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->id;_tmp58E->tms=({
struct Cyc_List_List*_tmp58F=_cycalloc(sizeof(*_tmp58F));_tmp58F->hd=(void*)((
void*)({struct Cyc_Absyn_ConstArray_mod_struct*_tmp590=_cycalloc(sizeof(*_tmp590));
_tmp590[0]=({struct Cyc_Absyn_ConstArray_mod_struct _tmp591;_tmp591.tag=1;_tmp591.f1=
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);
_tmp591.f2=Cyc_yyget_YY51(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
_tmp591.f3=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp591;});_tmp590;}));_tmp58F->tl=(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->tms;_tmp58F;});
_tmp58E;});_tmp58D;});break;case 164: _LL2A4: {struct _tuple17 _tmp593;struct Cyc_List_List*
_tmp594;int _tmp595;struct Cyc_Absyn_VarargInfo*_tmp596;struct Cyc_Core_Opt*_tmp597;
struct Cyc_List_List*_tmp598;struct _tuple17*_tmp592=Cyc_yyget_YY39(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp593=*_tmp592;_tmp594=
_tmp593.f1;_tmp595=_tmp593.f2;_tmp596=_tmp593.f3;_tmp597=_tmp593.f4;_tmp598=
_tmp593.f5;yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp599;(
_tmp599.YY29).tag=33;(_tmp599.YY29).f1=({struct Cyc_Parse_Declarator*_tmp59A=
_cycalloc(sizeof(*_tmp59A));_tmp59A->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id;_tmp59A->tms=({
struct Cyc_List_List*_tmp59B=_cycalloc(sizeof(*_tmp59B));_tmp59B->hd=(void*)((
void*)({struct Cyc_Absyn_Function_mod_struct*_tmp59C=_cycalloc(sizeof(*_tmp59C));
_tmp59C[0]=({struct Cyc_Absyn_Function_mod_struct _tmp59D;_tmp59D.tag=3;_tmp59D.f1=(
void*)((void*)({struct Cyc_Absyn_WithTypes_struct*_tmp59E=_cycalloc(sizeof(*
_tmp59E));_tmp59E[0]=({struct Cyc_Absyn_WithTypes_struct _tmp59F;_tmp59F.tag=1;
_tmp59F.f1=_tmp594;_tmp59F.f2=_tmp595;_tmp59F.f3=_tmp596;_tmp59F.f4=_tmp597;
_tmp59F.f5=_tmp598;_tmp59F;});_tmp59E;}));_tmp59D;});_tmp59C;}));_tmp59B->tl=(
Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;
_tmp59B;});_tmp59A;});_tmp599;});break;}case 165: _LL2A5: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp5A0;(_tmp5A0.YY29).tag=33;(_tmp5A0.YY29).f1=({struct
Cyc_Parse_Declarator*_tmp5A1=_cycalloc(sizeof(*_tmp5A1));_tmp5A1->id=(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->id;_tmp5A1->tms=({
struct Cyc_List_List*_tmp5A2=_cycalloc(sizeof(*_tmp5A2));_tmp5A2->hd=(void*)((
void*)({struct Cyc_Absyn_Function_mod_struct*_tmp5A3=_cycalloc(sizeof(*_tmp5A3));
_tmp5A3[0]=({struct Cyc_Absyn_Function_mod_struct _tmp5A4;_tmp5A4.tag=3;_tmp5A4.f1=(
void*)((void*)({struct Cyc_Absyn_WithTypes_struct*_tmp5A5=_cycalloc(sizeof(*
_tmp5A5));_tmp5A5[0]=({struct Cyc_Absyn_WithTypes_struct _tmp5A6;_tmp5A6.tag=1;
_tmp5A6.f1=0;_tmp5A6.f2=0;_tmp5A6.f3=0;_tmp5A6.f4=Cyc_yyget_YY49(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp5A6.f5=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp5A6;});_tmp5A5;}));
_tmp5A4;});_tmp5A3;}));_tmp5A2->tl=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->tms;_tmp5A2;});_tmp5A1;});
_tmp5A0;});break;case 166: _LL2A6: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp5A7;(_tmp5A7.YY29).tag=33;(_tmp5A7.YY29).f1=({struct Cyc_Parse_Declarator*
_tmp5A8=_cycalloc(sizeof(*_tmp5A8));_tmp5A8->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id;_tmp5A8->tms=({
struct Cyc_List_List*_tmp5A9=_cycalloc(sizeof(*_tmp5A9));_tmp5A9->hd=(void*)((
void*)({struct Cyc_Absyn_Function_mod_struct*_tmp5AA=_cycalloc(sizeof(*_tmp5AA));
_tmp5AA[0]=({struct Cyc_Absyn_Function_mod_struct _tmp5AB;_tmp5AB.tag=3;_tmp5AB.f1=(
void*)((void*)({struct Cyc_Absyn_NoTypes_struct*_tmp5AC=_cycalloc(sizeof(*_tmp5AC));
_tmp5AC[0]=({struct Cyc_Absyn_NoTypes_struct _tmp5AD;_tmp5AD.tag=0;_tmp5AD.f1=Cyc_yyget_YY36(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp5AD.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp5AD;});
_tmp5AC;}));_tmp5AB;});_tmp5AA;}));_tmp5A9->tl=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp5A9;});_tmp5A8;});
_tmp5A7;});break;case 167: _LL2A7: {struct Cyc_List_List*_tmp5AE=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),Cyc_List_imp_rev(
Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp5AF;(_tmp5AF.YY29).tag=
33;(_tmp5AF.YY29).f1=({struct Cyc_Parse_Declarator*_tmp5B0=_cycalloc(sizeof(*
_tmp5B0));_tmp5B0->id=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]))->id;_tmp5B0->tms=({struct Cyc_List_List*_tmp5B1=_cycalloc(
sizeof(*_tmp5B1));_tmp5B1->hd=(void*)((void*)({struct Cyc_Absyn_TypeParams_mod_struct*
_tmp5B2=_cycalloc(sizeof(*_tmp5B2));_tmp5B2[0]=({struct Cyc_Absyn_TypeParams_mod_struct
_tmp5B3;_tmp5B3.tag=4;_tmp5B3.f1=_tmp5AE;_tmp5B3.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp5B3.f3=0;
_tmp5B3;});_tmp5B2;}));_tmp5B1->tl=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp5B1;});_tmp5B0;});
_tmp5AF;});break;}case 168: _LL2A8: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp5B4;(_tmp5B4.YY29).tag=33;(_tmp5B4.YY29).f1=({struct Cyc_Parse_Declarator*
_tmp5B5=_cycalloc(sizeof(*_tmp5B5));_tmp5B5->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]))->id;_tmp5B5->tms=({
struct Cyc_List_List*_tmp5B6=_cycalloc(sizeof(*_tmp5B6));_tmp5B6->hd=(void*)((
void*)({struct Cyc_Absyn_Attributes_mod_struct*_tmp5B7=_cycalloc(sizeof(*_tmp5B7));
_tmp5B7[0]=({struct Cyc_Absyn_Attributes_mod_struct _tmp5B8;_tmp5B8.tag=5;_tmp5B8.f1=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp5B8.f2=Cyc_yyget_YY45(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5B8;});_tmp5B7;}));
_tmp5B6->tl=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]))->tms;_tmp5B6;});_tmp5B5;});_tmp5B4;});break;case 169: _LL2A9:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp5B9;(_tmp5B9.YY29).tag=
33;(_tmp5B9.YY29).f1=({struct Cyc_Parse_Declarator*_tmp5BA=_cycalloc(sizeof(*
_tmp5BA));_tmp5BA->id=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp5BA->tms=0;_tmp5BA;});_tmp5B9;});break;case 170: _LL2AA:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp5BB;(_tmp5BB.YY29).tag=
33;(_tmp5BB.YY29).f1=({struct Cyc_Parse_Declarator*_tmp5BC=_cycalloc(sizeof(*
_tmp5BC));_tmp5BC->id=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp5BC->tms=0;_tmp5BC;});_tmp5BB;});break;case 171: _LL2AB:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;case 172:
_LL2AC: {struct Cyc_Parse_Declarator*_tmp5BD=Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp5BD->tms=({struct Cyc_List_List*
_tmp5BE=_cycalloc(sizeof(*_tmp5BE));_tmp5BE->hd=(void*)((void*)({struct Cyc_Absyn_Attributes_mod_struct*
_tmp5BF=_cycalloc(sizeof(*_tmp5BF));_tmp5BF[0]=({struct Cyc_Absyn_Attributes_mod_struct
_tmp5C0;_tmp5C0.tag=5;_tmp5C0.f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line);_tmp5C0.f2=
Cyc_yyget_YY45(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);
_tmp5C0;});_tmp5BF;}));_tmp5BE->tl=_tmp5BD->tms;_tmp5BE;});yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;}case 173: _LL2AD:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp5C1;(_tmp5C1.YY29).tag=
33;(_tmp5C1.YY29).f1=({struct Cyc_Parse_Declarator*_tmp5C2=_cycalloc(sizeof(*
_tmp5C2));_tmp5C2->id=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]))->id;_tmp5C2->tms=({struct Cyc_List_List*_tmp5C3=_cycalloc(
sizeof(*_tmp5C3));_tmp5C3->hd=(void*)((void*)({struct Cyc_Absyn_Carray_mod_struct*
_tmp5C4=_cycalloc(sizeof(*_tmp5C4));_tmp5C4[0]=({struct Cyc_Absyn_Carray_mod_struct
_tmp5C5;_tmp5C5.tag=0;_tmp5C5.f1=Cyc_yyget_YY51(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5C5.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp5C5;});
_tmp5C4;}));_tmp5C3->tl=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]))->tms;_tmp5C3;});_tmp5C2;});_tmp5C1;});break;case 174:
_LL2AE: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp5C6;(_tmp5C6.YY29).tag=
33;(_tmp5C6.YY29).f1=({struct Cyc_Parse_Declarator*_tmp5C7=_cycalloc(sizeof(*
_tmp5C7));_tmp5C7->id=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)]))->id;_tmp5C7->tms=({struct Cyc_List_List*_tmp5C8=_cycalloc(
sizeof(*_tmp5C8));_tmp5C8->hd=(void*)((void*)({struct Cyc_Absyn_ConstArray_mod_struct*
_tmp5C9=_cycalloc(sizeof(*_tmp5C9));_tmp5C9[0]=({struct Cyc_Absyn_ConstArray_mod_struct
_tmp5CA;_tmp5CA.tag=1;_tmp5CA.f1=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp5CA.f2=Cyc_yyget_YY51(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5CA.f3=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp5CA;});
_tmp5C9;}));_tmp5C8->tl=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 4)]))->tms;_tmp5C8;});_tmp5C7;});_tmp5C6;});break;case 175:
_LL2AF: {struct _tuple17 _tmp5CC;struct Cyc_List_List*_tmp5CD;int _tmp5CE;struct Cyc_Absyn_VarargInfo*
_tmp5CF;struct Cyc_Core_Opt*_tmp5D0;struct Cyc_List_List*_tmp5D1;struct _tuple17*
_tmp5CB=Cyc_yyget_YY39(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp5CC=*_tmp5CB;_tmp5CD=_tmp5CC.f1;_tmp5CE=_tmp5CC.f2;_tmp5CF=_tmp5CC.f3;
_tmp5D0=_tmp5CC.f4;_tmp5D1=_tmp5CC.f5;yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp5D2;(_tmp5D2.YY29).tag=33;(_tmp5D2.YY29).f1=({struct Cyc_Parse_Declarator*
_tmp5D3=_cycalloc(sizeof(*_tmp5D3));_tmp5D3->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id;_tmp5D3->tms=({
struct Cyc_List_List*_tmp5D4=_cycalloc(sizeof(*_tmp5D4));_tmp5D4->hd=(void*)((
void*)({struct Cyc_Absyn_Function_mod_struct*_tmp5D5=_cycalloc(sizeof(*_tmp5D5));
_tmp5D5[0]=({struct Cyc_Absyn_Function_mod_struct _tmp5D6;_tmp5D6.tag=3;_tmp5D6.f1=(
void*)((void*)({struct Cyc_Absyn_WithTypes_struct*_tmp5D7=_cycalloc(sizeof(*
_tmp5D7));_tmp5D7[0]=({struct Cyc_Absyn_WithTypes_struct _tmp5D8;_tmp5D8.tag=1;
_tmp5D8.f1=_tmp5CD;_tmp5D8.f2=_tmp5CE;_tmp5D8.f3=_tmp5CF;_tmp5D8.f4=_tmp5D0;
_tmp5D8.f5=_tmp5D1;_tmp5D8;});_tmp5D7;}));_tmp5D6;});_tmp5D5;}));_tmp5D4->tl=(
Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;
_tmp5D4;});_tmp5D3;});_tmp5D2;});break;}case 176: _LL2B0: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp5D9;(_tmp5D9.YY29).tag=33;(_tmp5D9.YY29).f1=({struct
Cyc_Parse_Declarator*_tmp5DA=_cycalloc(sizeof(*_tmp5DA));_tmp5DA->id=(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->id;_tmp5DA->tms=({
struct Cyc_List_List*_tmp5DB=_cycalloc(sizeof(*_tmp5DB));_tmp5DB->hd=(void*)((
void*)({struct Cyc_Absyn_Function_mod_struct*_tmp5DC=_cycalloc(sizeof(*_tmp5DC));
_tmp5DC[0]=({struct Cyc_Absyn_Function_mod_struct _tmp5DD;_tmp5DD.tag=3;_tmp5DD.f1=(
void*)((void*)({struct Cyc_Absyn_WithTypes_struct*_tmp5DE=_cycalloc(sizeof(*
_tmp5DE));_tmp5DE[0]=({struct Cyc_Absyn_WithTypes_struct _tmp5DF;_tmp5DF.tag=1;
_tmp5DF.f1=0;_tmp5DF.f2=0;_tmp5DF.f3=0;_tmp5DF.f4=Cyc_yyget_YY49(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp5DF.f5=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp5DF;});_tmp5DE;}));
_tmp5DD;});_tmp5DC;}));_tmp5DB->tl=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->tms;_tmp5DB;});_tmp5DA;});
_tmp5D9;});break;case 177: _LL2B1: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp5E0;(_tmp5E0.YY29).tag=33;(_tmp5E0.YY29).f1=({struct Cyc_Parse_Declarator*
_tmp5E1=_cycalloc(sizeof(*_tmp5E1));_tmp5E1->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id;_tmp5E1->tms=({
struct Cyc_List_List*_tmp5E2=_cycalloc(sizeof(*_tmp5E2));_tmp5E2->hd=(void*)((
void*)({struct Cyc_Absyn_Function_mod_struct*_tmp5E3=_cycalloc(sizeof(*_tmp5E3));
_tmp5E3[0]=({struct Cyc_Absyn_Function_mod_struct _tmp5E4;_tmp5E4.tag=3;_tmp5E4.f1=(
void*)((void*)({struct Cyc_Absyn_NoTypes_struct*_tmp5E5=_cycalloc(sizeof(*_tmp5E5));
_tmp5E5[0]=({struct Cyc_Absyn_NoTypes_struct _tmp5E6;_tmp5E6.tag=0;_tmp5E6.f1=Cyc_yyget_YY36(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp5E6.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp5E6;});
_tmp5E5;}));_tmp5E4;});_tmp5E3;}));_tmp5E2->tl=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp5E2;});_tmp5E1;});
_tmp5E0;});break;case 178: _LL2B2: {struct Cyc_List_List*_tmp5E7=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),Cyc_List_imp_rev(
Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp5E8;(_tmp5E8.YY29).tag=
33;(_tmp5E8.YY29).f1=({struct Cyc_Parse_Declarator*_tmp5E9=_cycalloc(sizeof(*
_tmp5E9));_tmp5E9->id=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]))->id;_tmp5E9->tms=({struct Cyc_List_List*_tmp5EA=_cycalloc(
sizeof(*_tmp5EA));_tmp5EA->hd=(void*)((void*)({struct Cyc_Absyn_TypeParams_mod_struct*
_tmp5EB=_cycalloc(sizeof(*_tmp5EB));_tmp5EB[0]=({struct Cyc_Absyn_TypeParams_mod_struct
_tmp5EC;_tmp5EC.tag=4;_tmp5EC.f1=_tmp5E7;_tmp5EC.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp5EC.f3=0;
_tmp5EC;});_tmp5EB;}));_tmp5EA->tl=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp5EA;});_tmp5E9;});
_tmp5E8;});break;}case 179: _LL2B3: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp5ED;(_tmp5ED.YY29).tag=33;(_tmp5ED.YY29).f1=({struct Cyc_Parse_Declarator*
_tmp5EE=_cycalloc(sizeof(*_tmp5EE));_tmp5EE->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]))->id;_tmp5EE->tms=({
struct Cyc_List_List*_tmp5EF=_cycalloc(sizeof(*_tmp5EF));_tmp5EF->hd=(void*)((
void*)({struct Cyc_Absyn_Attributes_mod_struct*_tmp5F0=_cycalloc(sizeof(*_tmp5F0));
_tmp5F0[0]=({struct Cyc_Absyn_Attributes_mod_struct _tmp5F1;_tmp5F1.tag=5;_tmp5F1.f1=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp5F1.f2=Cyc_yyget_YY45(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5F1;});_tmp5F0;}));
_tmp5EF->tl=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]))->tms;_tmp5EF;});_tmp5EE;});_tmp5ED;});break;case 180: _LL2B4:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 181:
_LL2B5: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp5F2;(_tmp5F2.YY28).tag=
32;(_tmp5F2.YY28).f1=Cyc_List_imp_append(Cyc_yyget_YY28(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY28(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp5F2;});break;case 182:
_LL2B6: {struct Cyc_List_List*ans=0;if(Cyc_yyget_YY45(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])!= 0)ans=({struct Cyc_List_List*
_tmp5F3=_cycalloc(sizeof(*_tmp5F3));_tmp5F3->hd=(void*)((void*)({struct Cyc_Absyn_Attributes_mod_struct*
_tmp5F4=_cycalloc(sizeof(*_tmp5F4));_tmp5F4[0]=({struct Cyc_Absyn_Attributes_mod_struct
_tmp5F5;_tmp5F5.tag=5;_tmp5F5.f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line);_tmp5F5.f2=
Cyc_yyget_YY45(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp5F5;});_tmp5F4;}));_tmp5F3->tl=ans;_tmp5F3;});{struct Cyc_Absyn_PtrLoc*ptrloc=
0;if(Cyc_Absyn_porting_c_code)ptrloc=({struct Cyc_Absyn_PtrLoc*_tmp5F6=_cycalloc(
sizeof(*_tmp5F6));_tmp5F6->ptr_loc=(*Cyc_yyget_YY1(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)])).f1;_tmp5F6->rgn_loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line);_tmp5F6->zt_loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line);_tmp5F6;});ans=({struct Cyc_List_List*_tmp5F7=
_cycalloc(sizeof(*_tmp5F7));_tmp5F7->hd=(void*)((void*)({struct Cyc_Absyn_Pointer_mod_struct*
_tmp5F8=_cycalloc(sizeof(*_tmp5F8));_tmp5F8[0]=({struct Cyc_Absyn_Pointer_mod_struct
_tmp5F9;_tmp5F9.tag=2;_tmp5F9.f1=({struct Cyc_Absyn_PtrAtts _tmp5FA;_tmp5FA.rgn=(
void*)Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);
_tmp5FA.nullable=(*Cyc_yyget_YY1(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)])).f2;_tmp5FA.bounds=(*Cyc_yyget_YY1(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)])).f3;_tmp5FA.zero_term=
Cyc_yyget_YY51(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);
_tmp5FA.ptrloc=ptrloc;_tmp5FA;});_tmp5F9.f2=Cyc_yyget_YY25(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5F9;});_tmp5F8;}));
_tmp5F7->tl=ans;_tmp5F7;});yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp5FB;(_tmp5FB.YY28).tag=32;(_tmp5FB.YY28).f1=ans;_tmp5FB;});break;}}case 183:
_LL2B7: {struct Cyc_Position_Segment*loc=Cyc_Absyn_porting_c_code?Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line): 0;yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp5FC;(_tmp5FC.YY1).tag=5;(
_tmp5FC.YY1).f1=({struct _tuple12*_tmp5FD=_cycalloc(sizeof(*_tmp5FD));_tmp5FD->f1=
loc;_tmp5FD->f2=Cyc_Absyn_true_conref;_tmp5FD->f3=Cyc_yyget_YY2(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5FD;});_tmp5FC;});
break;}case 184: _LL2B8: {struct Cyc_Position_Segment*loc=Cyc_Absyn_porting_c_code?
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line): 0;yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp5FE;(_tmp5FE.YY1).tag=5;(_tmp5FE.YY1).f1=({struct _tuple12*_tmp5FF=_cycalloc(
sizeof(*_tmp5FF));_tmp5FF->f1=loc;_tmp5FF->f2=Cyc_Absyn_false_conref;_tmp5FF->f3=
Cyc_yyget_YY2(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5FF;});
_tmp5FE;});break;}case 185: _LL2B9: {struct Cyc_Position_Segment*loc=Cyc_Absyn_porting_c_code?
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line): 0;yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp600;(_tmp600.YY1).tag=5;(_tmp600.YY1).f1=({struct _tuple12*_tmp601=_cycalloc(
sizeof(*_tmp601));_tmp601->f1=loc;_tmp601->f2=Cyc_Absyn_true_conref;_tmp601->f3=
Cyc_Absyn_bounds_dyneither_conref;_tmp601;});_tmp600;});break;}case 186: _LL2BA:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp602;(_tmp602.YY2).tag=
6;(_tmp602.YY2).f1=Cyc_Absyn_bounds_one_conref;_tmp602;});break;case 187: _LL2BB:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp603;(_tmp603.YY2).tag=
6;(_tmp603.YY2).f1=Cyc_Absyn_new_conref((void*)({struct Cyc_Absyn_Upper_b_struct*
_tmp604=_cycalloc(sizeof(*_tmp604));_tmp604[0]=({struct Cyc_Absyn_Upper_b_struct
_tmp605;_tmp605.tag=0;_tmp605.f1=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp605;});_tmp604;}));
_tmp603;});break;case 188: _LL2BC: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp606;(_tmp606.YY51).tag=55;(_tmp606.YY51).f1=((struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)();_tmp606;});break;case 189: _LL2BD: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp607;(_tmp607.YY51).tag=55;(_tmp607.YY51).f1=Cyc_Absyn_true_conref;
_tmp607;});break;case 190: _LL2BE: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp608;(_tmp608.YY51).tag=55;(_tmp608.YY51).f1=Cyc_Absyn_false_conref;_tmp608;});
break;case 191: _LL2BF: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp609;(_tmp609.YY49).tag=53;(_tmp609.YY49).f1=0;_tmp609;});break;case 192:
_LL2C0: Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),(void*)3,1);yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp60A;(_tmp60A.YY49).tag=53;(_tmp60A.YY49).f1=({struct
Cyc_Core_Opt*_tmp60B=_cycalloc(sizeof(*_tmp60B));_tmp60B->v=(void*)Cyc_yyget_YY44(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp60B;});_tmp60A;});
break;case 193: _LL2C1: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp60C;(_tmp60C.YY49).tag=53;(_tmp60C.YY49).f1=({struct Cyc_Core_Opt*_tmp60D=
_cycalloc(sizeof(*_tmp60D));_tmp60D->v=(void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_rk,0);_tmp60D;});_tmp60C;});break;case 194: _LL2C2: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp60E;(_tmp60E.YY44).tag=48;(_tmp60E.YY44).f1=(void*)Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_trk,0);_tmp60E;});break;case 195: _LL2C3: Cyc_Parse_set_vartyp_kind(
Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),(void*)
5,1);yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
196: _LL2C4: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp60F;(
_tmp60F.YY44).tag=48;(_tmp60F.YY44).f1=(void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_trk,0);_tmp60F;});break;case 197: _LL2C5: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp610;(_tmp610.YY25).tag=29;(_tmp610.YY25).f1=Cyc_Absyn_empty_tqual(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset + 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset + 1)]).last_line));_tmp610;});break;case 198: _LL2C6: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp611;(_tmp611.YY25).tag=29;(_tmp611.YY25).f1=Cyc_Absyn_combine_tqual(
Cyc_yyget_YY25(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY25(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp611;});break;case
199: _LL2C7: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp612;(
_tmp612.YY39).tag=43;(_tmp612.YY39).f1=({struct _tuple17*_tmp613=_cycalloc(
sizeof(*_tmp613));_tmp613->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(Cyc_yyget_YY38(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]));_tmp613->f2=0;_tmp613->f3=0;_tmp613->f4=Cyc_yyget_YY49(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp613->f5=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp613;});_tmp612;});
break;case 200: _LL2C8: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp614;(_tmp614.YY39).tag=43;(_tmp614.YY39).f1=({struct _tuple17*_tmp615=
_cycalloc(sizeof(*_tmp615));_tmp615->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(Cyc_yyget_YY38(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)]));_tmp615->f2=1;_tmp615->f3=0;_tmp615->f4=Cyc_yyget_YY49(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp615->f5=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp615;});_tmp614;});
break;case 201: _LL2C9: {struct _tuple2 _tmp617;struct Cyc_Core_Opt*_tmp618;struct Cyc_Absyn_Tqual
_tmp619;void*_tmp61A;struct _tuple2*_tmp616=Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp617=*_tmp616;_tmp618=
_tmp617.f1;_tmp619=_tmp617.f2;_tmp61A=_tmp617.f3;{struct Cyc_Absyn_VarargInfo*
_tmp61B=({struct Cyc_Absyn_VarargInfo*_tmp61E=_cycalloc(sizeof(*_tmp61E));_tmp61E->name=
_tmp618;_tmp61E->tq=_tmp619;_tmp61E->type=(void*)_tmp61A;_tmp61E->inject=Cyc_yyget_YY31(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp61E;});yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp61C;(_tmp61C.YY39).tag=43;(
_tmp61C.YY39).f1=({struct _tuple17*_tmp61D=_cycalloc(sizeof(*_tmp61D));_tmp61D->f1=
0;_tmp61D->f2=0;_tmp61D->f3=_tmp61B;_tmp61D->f4=Cyc_yyget_YY49(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp61D->f5=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp61D;});_tmp61C;});
break;}}case 202: _LL2CA: {struct _tuple2 _tmp620;struct Cyc_Core_Opt*_tmp621;struct
Cyc_Absyn_Tqual _tmp622;void*_tmp623;struct _tuple2*_tmp61F=Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp620=*_tmp61F;_tmp621=
_tmp620.f1;_tmp622=_tmp620.f2;_tmp623=_tmp620.f3;{struct Cyc_Absyn_VarargInfo*
_tmp624=({struct Cyc_Absyn_VarargInfo*_tmp627=_cycalloc(sizeof(*_tmp627));_tmp627->name=
_tmp621;_tmp627->tq=_tmp622;_tmp627->type=(void*)_tmp623;_tmp627->inject=Cyc_yyget_YY31(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp627;});yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp625;(_tmp625.YY39).tag=43;(
_tmp625.YY39).f1=({struct _tuple17*_tmp626=_cycalloc(sizeof(*_tmp626));_tmp626->f1=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]));_tmp626->f2=0;
_tmp626->f3=_tmp624;_tmp626->f4=Cyc_yyget_YY49(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp626->f5=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp626;});_tmp625;});
break;}}case 203: _LL2CB: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp628;(_tmp628.YY44).tag=48;(_tmp628.YY44).f1=(void*)Cyc_Parse_id2type(Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),(void*)({struct Cyc_Absyn_Unknown_kb_struct*
_tmp629=_cycalloc(sizeof(*_tmp629));_tmp629[0]=({struct Cyc_Absyn_Unknown_kb_struct
_tmp62A;_tmp62A.tag=1;_tmp62A.f1=0;_tmp62A;});_tmp629;}));_tmp628;});break;case
204: _LL2CC: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp62B;(
_tmp62B.YY44).tag=48;(_tmp62B.YY44).f1=(void*)Cyc_Parse_id2type(Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_Tcutil_kind_to_bound(
Cyc_yyget_YY43(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])));
_tmp62B;});break;case 205: _LL2CD: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp62C;(_tmp62C.YY49).tag=53;(_tmp62C.YY49).f1=0;_tmp62C;});break;case 206:
_LL2CE: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp62D;(_tmp62D.YY49).tag=
53;(_tmp62D.YY49).f1=({struct Cyc_Core_Opt*_tmp62E=_cycalloc(sizeof(*_tmp62E));
_tmp62E->v=(void*)((void*)({struct Cyc_Absyn_JoinEff_struct*_tmp62F=_cycalloc(
sizeof(*_tmp62F));_tmp62F[0]=({struct Cyc_Absyn_JoinEff_struct _tmp630;_tmp630.tag=
20;_tmp630.f1=Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp630;});_tmp62F;}));_tmp62E;});_tmp62D;});break;case 207:
_LL2CF: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp631;(_tmp631.YY50).tag=
54;(_tmp631.YY50).f1=0;_tmp631;});break;case 208: _LL2D0: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 209: _LL2D1: {
struct Cyc_Absyn_Less_kb_struct*_tmp632=({struct Cyc_Absyn_Less_kb_struct*_tmp63A=
_cycalloc(sizeof(*_tmp63A));_tmp63A[0]=({struct Cyc_Absyn_Less_kb_struct _tmp63B;
_tmp63B.tag=2;_tmp63B.f1=0;_tmp63B.f2=(void*)((void*)5);_tmp63B;});_tmp63A;});
struct _dyneither_ptr _tmp633=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);void*_tmp634=Cyc_Parse_id2type(
_tmp633,(void*)_tmp632);yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp635;(_tmp635.YY50).tag=54;(_tmp635.YY50).f1=({struct Cyc_List_List*_tmp636=
_cycalloc(sizeof(*_tmp636));_tmp636->hd=({struct _tuple4*_tmp637=_cycalloc(
sizeof(*_tmp637));_tmp637->f1=(void*)({struct Cyc_Absyn_JoinEff_struct*_tmp638=
_cycalloc(sizeof(*_tmp638));_tmp638[0]=({struct Cyc_Absyn_JoinEff_struct _tmp639;
_tmp639.tag=20;_tmp639.f1=Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]);_tmp639;});_tmp638;});_tmp637->f2=_tmp634;_tmp637;});
_tmp636->tl=0;_tmp636;});_tmp635;});break;}case 210: _LL2D2: {struct Cyc_Absyn_Less_kb_struct*
_tmp63C=({struct Cyc_Absyn_Less_kb_struct*_tmp644=_cycalloc(sizeof(*_tmp644));
_tmp644[0]=({struct Cyc_Absyn_Less_kb_struct _tmp645;_tmp645.tag=2;_tmp645.f1=0;
_tmp645.f2=(void*)((void*)5);_tmp645;});_tmp644;});struct _dyneither_ptr _tmp63D=
Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);
void*_tmp63E=Cyc_Parse_id2type(_tmp63D,(void*)_tmp63C);yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp63F;(_tmp63F.YY50).tag=54;(_tmp63F.YY50).f1=({struct
Cyc_List_List*_tmp640=_cycalloc(sizeof(*_tmp640));_tmp640->hd=({struct _tuple4*
_tmp641=_cycalloc(sizeof(*_tmp641));_tmp641->f1=(void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp642=_cycalloc(sizeof(*_tmp642));_tmp642[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp643;_tmp643.tag=20;_tmp643.f1=Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp643;});_tmp642;});
_tmp641->f2=_tmp63E;_tmp641;});_tmp640->tl=Cyc_yyget_YY50(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp640;});_tmp63F;});
break;}case 211: _LL2D3: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp646;(_tmp646.YY31).tag=35;(_tmp646.YY31).f1=0;_tmp646;});break;case 212:
_LL2D4: if(Cyc_zstrcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),({const char*_tmp647="inject";
_tag_dyneither(_tmp647,sizeof(char),7);}))!= 0)Cyc_Parse_err(({const char*_tmp648="missing type in function declaration";
_tag_dyneither(_tmp648,sizeof(char),37);}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp649;(_tmp649.YY31).tag=35;(_tmp649.YY31).f1=1;_tmp649;});
break;case 213: _LL2D5: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 214: _LL2D6: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp64A;(_tmp64A.YY40).tag=44;(_tmp64A.YY40).f1=Cyc_List_imp_append(Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp64A;});break;case 215:
_LL2D7: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp64B;(_tmp64B.YY40).tag=
44;(_tmp64B.YY40).f1=0;_tmp64B;});break;case 216: _LL2D8: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;case 217: _LL2D9:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp64C;(_tmp64C.YY40).tag=
44;(_tmp64C.YY40).f1=({struct Cyc_List_List*_tmp64D=_cycalloc(sizeof(*_tmp64D));
_tmp64D->hd=(void*)((void*)({struct Cyc_Absyn_RgnsEff_struct*_tmp64E=_cycalloc(
sizeof(*_tmp64E));_tmp64E[0]=({struct Cyc_Absyn_RgnsEff_struct _tmp64F;_tmp64F.tag=
21;_tmp64F.f1=(void*)Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp64F;});_tmp64E;}));_tmp64D->tl=0;_tmp64D;});_tmp64C;});
break;case 218: _LL2DA: Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),(void*)6,0);yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp650;(_tmp650.YY40).tag=44;(_tmp650.YY40).f1=({struct
Cyc_List_List*_tmp651=_cycalloc(sizeof(*_tmp651));_tmp651->hd=(void*)Cyc_yyget_YY44(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp651->tl=0;_tmp651;});
_tmp650;});break;case 219: _LL2DB: Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),(void*)5,1);yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp652;(_tmp652.YY40).tag=44;(_tmp652.YY40).f1=({struct
Cyc_List_List*_tmp653=_cycalloc(sizeof(*_tmp653));_tmp653->hd=(void*)((void*)({
struct Cyc_Absyn_AccessEff_struct*_tmp654=_cycalloc(sizeof(*_tmp654));_tmp654[0]=({
struct Cyc_Absyn_AccessEff_struct _tmp655;_tmp655.tag=19;_tmp655.f1=(void*)Cyc_yyget_YY44(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp655;});_tmp654;}));
_tmp653->tl=0;_tmp653;});_tmp652;});break;case 220: _LL2DC: Cyc_Parse_set_vartyp_kind(
Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),(
void*)5,1);yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp656;(
_tmp656.YY40).tag=44;(_tmp656.YY40).f1=({struct Cyc_List_List*_tmp657=_cycalloc(
sizeof(*_tmp657));_tmp657->hd=(void*)((void*)({struct Cyc_Absyn_AccessEff_struct*
_tmp658=_cycalloc(sizeof(*_tmp658));_tmp658[0]=({struct Cyc_Absyn_AccessEff_struct
_tmp659;_tmp659.tag=19;_tmp659.f1=(void*)Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp659;});_tmp658;}));
_tmp657->tl=Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp657;});_tmp656;});break;case 221: _LL2DD: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp65A;(_tmp65A.YY38).tag=42;(_tmp65A.YY38).f1=({struct
Cyc_List_List*_tmp65B=_cycalloc(sizeof(*_tmp65B));_tmp65B->hd=Cyc_yyget_YY37(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp65B->tl=0;_tmp65B;});
_tmp65A;});break;case 222: _LL2DE: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp65C;(_tmp65C.YY38).tag=42;(_tmp65C.YY38).f1=({struct Cyc_List_List*_tmp65D=
_cycalloc(sizeof(*_tmp65D));_tmp65D->hd=Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp65D->tl=Cyc_yyget_YY38(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp65D;});_tmp65C;});
break;case 223: _LL2DF: {struct _tuple16 _tmp65F;struct Cyc_Absyn_Tqual _tmp660;struct
Cyc_List_List*_tmp661;struct Cyc_List_List*_tmp662;struct _tuple16*_tmp65E=Cyc_yyget_YY35(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp65F=*_tmp65E;
_tmp660=_tmp65F.f1;_tmp661=_tmp65F.f2;_tmp662=_tmp65F.f3;if(_tmp660.loc == 0)
_tmp660.loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line);{struct Cyc_Parse_Declarator _tmp664;struct _tuple1*
_tmp665;struct Cyc_List_List*_tmp666;struct Cyc_Parse_Declarator*_tmp663=Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp664=*_tmp663;
_tmp665=_tmp664.id;_tmp666=_tmp664.tms;{void*_tmp667=Cyc_Parse_speclist2typ(
_tmp661,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));struct Cyc_Absyn_Tqual _tmp669;void*_tmp66A;struct
Cyc_List_List*_tmp66B;struct Cyc_List_List*_tmp66C;struct _tuple6 _tmp668=Cyc_Parse_apply_tms(
_tmp660,_tmp667,_tmp662,_tmp666);_tmp669=_tmp668.f1;_tmp66A=_tmp668.f2;_tmp66B=
_tmp668.f3;_tmp66C=_tmp668.f4;if(_tmp66B != 0)Cyc_Parse_err(({const char*_tmp66D="parameter with bad type params";
_tag_dyneither(_tmp66D,sizeof(char),31);}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));if(Cyc_Absyn_is_qvar_qualified(
_tmp665))Cyc_Parse_err(({const char*_tmp66E="parameter cannot be qualified with a namespace";
_tag_dyneither(_tmp66E,sizeof(char),47);}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));{struct Cyc_Core_Opt*
_tmp66F=(struct Cyc_Core_Opt*)({struct Cyc_Core_Opt*_tmp674=_cycalloc(sizeof(*
_tmp674));_tmp674->v=(*_tmp665).f2;_tmp674;});if(_tmp66C != 0)({void*_tmp670=0;
Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),({const char*_tmp671="extra attributes on parameter, ignoring";
_tag_dyneither(_tmp671,sizeof(char),40);}),_tag_dyneither(_tmp670,sizeof(void*),
0));});yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp672;(_tmp672.YY37).tag=
41;(_tmp672.YY37).f1=({struct _tuple2*_tmp673=_cycalloc(sizeof(*_tmp673));_tmp673->f1=
_tmp66F;_tmp673->f2=_tmp669;_tmp673->f3=_tmp66A;_tmp673;});_tmp672;});break;}}}}
case 224: _LL2E0: {struct _tuple16 _tmp676;struct Cyc_Absyn_Tqual _tmp677;struct Cyc_List_List*
_tmp678;struct Cyc_List_List*_tmp679;struct _tuple16*_tmp675=Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp676=*_tmp675;_tmp677=
_tmp676.f1;_tmp678=_tmp676.f2;_tmp679=_tmp676.f3;if(_tmp677.loc == 0)_tmp677.loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);{void*_tmp67A=Cyc_Parse_speclist2typ(_tmp678,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));if(_tmp679 != 0)({
void*_tmp67B=0;Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),({const char*
_tmp67C="bad attributes on parameter, ignoring";_tag_dyneither(_tmp67C,sizeof(
char),38);}),_tag_dyneither(_tmp67B,sizeof(void*),0));});yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp67D;(_tmp67D.YY37).tag=41;(_tmp67D.YY37).f1=({struct
_tuple2*_tmp67E=_cycalloc(sizeof(*_tmp67E));_tmp67E->f1=0;_tmp67E->f2=_tmp677;
_tmp67E->f3=_tmp67A;_tmp67E;});_tmp67D;});break;}}case 225: _LL2E1: {struct
_tuple16 _tmp680;struct Cyc_Absyn_Tqual _tmp681;struct Cyc_List_List*_tmp682;struct
Cyc_List_List*_tmp683;struct _tuple16*_tmp67F=Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp680=*_tmp67F;_tmp681=
_tmp680.f1;_tmp682=_tmp680.f2;_tmp683=_tmp680.f3;if(_tmp681.loc == 0)_tmp681.loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line);{void*_tmp684=Cyc_Parse_speclist2typ(_tmp682,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));struct Cyc_List_List*
_tmp685=(Cyc_yyget_YY30(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tms;
struct Cyc_Absyn_Tqual _tmp687;void*_tmp688;struct Cyc_List_List*_tmp689;struct Cyc_List_List*
_tmp68A;struct _tuple6 _tmp686=Cyc_Parse_apply_tms(_tmp681,_tmp684,_tmp683,_tmp685);
_tmp687=_tmp686.f1;_tmp688=_tmp686.f2;_tmp689=_tmp686.f3;_tmp68A=_tmp686.f4;if(
_tmp689 != 0)({void*_tmp68B=0;Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),({const char*
_tmp68C="bad type parameters on formal argument, ignoring";_tag_dyneither(
_tmp68C,sizeof(char),49);}),_tag_dyneither(_tmp68B,sizeof(void*),0));});if(
_tmp68A != 0)({void*_tmp68D=0;Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),({const char*
_tmp68E="bad attributes on parameter, ignoring";_tag_dyneither(_tmp68E,sizeof(
char),38);}),_tag_dyneither(_tmp68D,sizeof(void*),0));});yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp68F;(_tmp68F.YY37).tag=41;(_tmp68F.YY37).f1=({struct
_tuple2*_tmp690=_cycalloc(sizeof(*_tmp690));_tmp690->f1=0;_tmp690->f2=_tmp687;
_tmp690->f3=_tmp688;_tmp690;});_tmp68F;});break;}}case 226: _LL2E2: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp691;(_tmp691.YY36).tag=40;(_tmp691.YY36).f1=((struct
Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY36(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp691;});break;case 227:
_LL2E3: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp692;(_tmp692.YY36).tag=
40;(_tmp692.YY36).f1=({struct Cyc_List_List*_tmp693=_cycalloc(sizeof(*_tmp693));
_tmp693->hd=({struct _dyneither_ptr*_tmp694=_cycalloc(sizeof(*_tmp694));_tmp694[0]=
Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
_tmp694;});_tmp693->tl=0;_tmp693;});_tmp692;});break;case 228: _LL2E4: yyval=(union
Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp695;(_tmp695.YY36).tag=40;(_tmp695.YY36).f1=({
struct Cyc_List_List*_tmp696=_cycalloc(sizeof(*_tmp696));_tmp696->hd=({struct
_dyneither_ptr*_tmp697=_cycalloc(sizeof(*_tmp697));_tmp697[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp697;});_tmp696->tl=
Cyc_yyget_YY36(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);
_tmp696;});_tmp695;});break;case 229: _LL2E5: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 230: _LL2E6: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 231: _LL2E7:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp698;(_tmp698.YY3).tag=
7;(_tmp698.YY3).f1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_UnresolvedMem_e_struct*
_tmp699=_cycalloc(sizeof(*_tmp699));_tmp699[0]=({struct Cyc_Absyn_UnresolvedMem_e_struct
_tmp69A;_tmp69A.tag=37;_tmp69A.f1=0;_tmp69A.f2=0;_tmp69A;});_tmp699;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp698;});
break;case 232: _LL2E8: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp69B;(_tmp69B.YY3).tag=7;(_tmp69B.YY3).f1=Cyc_Absyn_new_exp((void*)({struct
Cyc_Absyn_UnresolvedMem_e_struct*_tmp69C=_cycalloc(sizeof(*_tmp69C));_tmp69C[0]=({
struct Cyc_Absyn_UnresolvedMem_e_struct _tmp69D;_tmp69D.tag=37;_tmp69D.f1=0;
_tmp69D.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_YY6(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));
_tmp69D;});_tmp69C;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp69B;});
break;case 233: _LL2E9: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp69E;(_tmp69E.YY3).tag=7;(_tmp69E.YY3).f1=Cyc_Absyn_new_exp((void*)({struct
Cyc_Absyn_UnresolvedMem_e_struct*_tmp69F=_cycalloc(sizeof(*_tmp69F));_tmp69F[0]=({
struct Cyc_Absyn_UnresolvedMem_e_struct _tmp6A0;_tmp6A0.tag=37;_tmp6A0.f1=0;
_tmp6A0.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_YY6(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]));
_tmp6A0;});_tmp69F;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp69E;});
break;case 234: _LL2EA: {struct Cyc_Absyn_Vardecl*_tmp6A1=Cyc_Absyn_new_vardecl(({
struct _tuple1*_tmp6A5=_cycalloc(sizeof(*_tmp6A5));_tmp6A5->f1=(union Cyc_Absyn_Nmspace_union)({
union Cyc_Absyn_Nmspace_union _tmp6A7;(_tmp6A7.Loc_n).tag=0;_tmp6A7;});_tmp6A5->f2=({
struct _dyneither_ptr*_tmp6A6=_cycalloc(sizeof(*_tmp6A6));_tmp6A6[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]);_tmp6A6;});_tmp6A5;}),
Cyc_Absyn_uint_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).last_line)));(_tmp6A1->tq).real_const=
1;yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp6A2;(_tmp6A2.YY3).tag=
7;(_tmp6A2.YY3).f1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Comprehension_e_struct*
_tmp6A3=_cycalloc(sizeof(*_tmp6A3));_tmp6A3[0]=({struct Cyc_Absyn_Comprehension_e_struct
_tmp6A4;_tmp6A4.tag=29;_tmp6A4.f1=_tmp6A1;_tmp6A4.f2=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp6A4.f3=Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp6A4.f4=0;
_tmp6A4;});_tmp6A3;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp6A2;});
break;}case 235: _LL2EB: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp6A8;(_tmp6A8.YY6).tag=10;(_tmp6A8.YY6).f1=({struct Cyc_List_List*_tmp6A9=
_cycalloc(sizeof(*_tmp6A9));_tmp6A9->hd=({struct _tuple20*_tmp6AA=_cycalloc(
sizeof(*_tmp6AA));_tmp6AA->f1=0;_tmp6AA->f2=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6AA;});_tmp6A9->tl=0;
_tmp6A9;});_tmp6A8;});break;case 236: _LL2EC: yyval=(union Cyc_YYSTYPE_union)({union
Cyc_YYSTYPE_union _tmp6AB;(_tmp6AB.YY6).tag=10;(_tmp6AB.YY6).f1=({struct Cyc_List_List*
_tmp6AC=_cycalloc(sizeof(*_tmp6AC));_tmp6AC->hd=({struct _tuple20*_tmp6AD=
_cycalloc(sizeof(*_tmp6AD));_tmp6AD->f1=Cyc_yyget_YY41(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp6AD->f2=Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6AD;});_tmp6AC->tl=
0;_tmp6AC;});_tmp6AB;});break;case 237: _LL2ED: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp6AE;(_tmp6AE.YY6).tag=10;(_tmp6AE.YY6).f1=({struct Cyc_List_List*
_tmp6AF=_cycalloc(sizeof(*_tmp6AF));_tmp6AF->hd=({struct _tuple20*_tmp6B0=
_cycalloc(sizeof(*_tmp6B0));_tmp6B0->f1=0;_tmp6B0->f2=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6B0;});_tmp6AF->tl=Cyc_yyget_YY6(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp6AF;});_tmp6AE;});
break;case 238: _LL2EE: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp6B1;(_tmp6B1.YY6).tag=10;(_tmp6B1.YY6).f1=({struct Cyc_List_List*_tmp6B2=
_cycalloc(sizeof(*_tmp6B2));_tmp6B2->hd=({struct _tuple20*_tmp6B3=_cycalloc(
sizeof(*_tmp6B3));_tmp6B3->f1=Cyc_yyget_YY41(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_tmp6B3->f2=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6B3;});_tmp6B2->tl=Cyc_yyget_YY6(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp6B2;});_tmp6B1;});
break;case 239: _LL2EF: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp6B4;(_tmp6B4.YY41).tag=45;(_tmp6B4.YY41).f1=Cyc_List_imp_rev(Cyc_yyget_YY41(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp6B4;});break;
case 240: _LL2F0: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp6B5;(
_tmp6B5.YY41).tag=45;(_tmp6B5.YY41).f1=({struct Cyc_List_List*_tmp6B6=_cycalloc(
sizeof(*_tmp6B6));_tmp6B6->hd=(void*)Cyc_yyget_YY42(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6B6->tl=0;_tmp6B6;});
_tmp6B5;});break;case 241: _LL2F1: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp6B7;(_tmp6B7.YY41).tag=45;(_tmp6B7.YY41).f1=({struct Cyc_List_List*_tmp6B8=
_cycalloc(sizeof(*_tmp6B8));_tmp6B8->hd=(void*)Cyc_yyget_YY42(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6B8->tl=Cyc_yyget_YY41(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp6B8;});_tmp6B7;});
break;case 242: _LL2F2: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp6B9;(_tmp6B9.YY42).tag=46;(_tmp6B9.YY42).f1=(void*)((void*)({struct Cyc_Absyn_ArrayElement_struct*
_tmp6BA=_cycalloc(sizeof(*_tmp6BA));_tmp6BA[0]=({struct Cyc_Absyn_ArrayElement_struct
_tmp6BB;_tmp6BB.tag=0;_tmp6BB.f1=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp6BB;});_tmp6BA;}));
_tmp6B9;});break;case 243: _LL2F3: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp6BC;(_tmp6BC.YY42).tag=46;(_tmp6BC.YY42).f1=(void*)((void*)({struct Cyc_Absyn_FieldName_struct*
_tmp6BD=_cycalloc(sizeof(*_tmp6BD));_tmp6BD[0]=({struct Cyc_Absyn_FieldName_struct
_tmp6BE;_tmp6BE.tag=1;_tmp6BE.f1=({struct _dyneither_ptr*_tmp6BF=_cycalloc(
sizeof(*_tmp6BF));_tmp6BF[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6BF;});_tmp6BE;});
_tmp6BD;}));_tmp6BC;});break;case 244: _LL2F4: {struct _tuple16 _tmp6C1;struct Cyc_Absyn_Tqual
_tmp6C2;struct Cyc_List_List*_tmp6C3;struct Cyc_List_List*_tmp6C4;struct _tuple16*
_tmp6C0=Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
_tmp6C1=*_tmp6C0;_tmp6C2=_tmp6C1.f1;_tmp6C3=_tmp6C1.f2;_tmp6C4=_tmp6C1.f3;{void*
_tmp6C5=Cyc_Parse_speclist2typ(_tmp6C3,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));if(_tmp6C4 != 0)({
void*_tmp6C6=0;Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),({const char*
_tmp6C7="ignoring attributes in type";_tag_dyneither(_tmp6C7,sizeof(char),28);}),
_tag_dyneither(_tmp6C6,sizeof(void*),0));});yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp6C8;(_tmp6C8.YY37).tag=41;(_tmp6C8.YY37).f1=({struct
_tuple2*_tmp6C9=_cycalloc(sizeof(*_tmp6C9));_tmp6C9->f1=0;_tmp6C9->f2=_tmp6C2;
_tmp6C9->f3=_tmp6C5;_tmp6C9;});_tmp6C8;});break;}}case 245: _LL2F5: {struct
_tuple16 _tmp6CB;struct Cyc_Absyn_Tqual _tmp6CC;struct Cyc_List_List*_tmp6CD;struct
Cyc_List_List*_tmp6CE;struct _tuple16*_tmp6CA=Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp6CB=*_tmp6CA;_tmp6CC=
_tmp6CB.f1;_tmp6CD=_tmp6CB.f2;_tmp6CE=_tmp6CB.f3;{void*_tmp6CF=Cyc_Parse_speclist2typ(
_tmp6CD,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));struct Cyc_List_List*_tmp6D0=(Cyc_yyget_YY30(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->tms;struct _tuple6 _tmp6D1=
Cyc_Parse_apply_tms(_tmp6CC,_tmp6CF,_tmp6CE,_tmp6D0);if(_tmp6D1.f3 != 0)({void*
_tmp6D2=0;Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),({const char*
_tmp6D3="bad type params, ignoring";_tag_dyneither(_tmp6D3,sizeof(char),26);}),
_tag_dyneither(_tmp6D2,sizeof(void*),0));});if(_tmp6D1.f4 != 0)({void*_tmp6D4=0;
Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),({const char*_tmp6D5="bad specifiers, ignoring";
_tag_dyneither(_tmp6D5,sizeof(char),25);}),_tag_dyneither(_tmp6D4,sizeof(void*),
0));});yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp6D6;(_tmp6D6.YY37).tag=
41;(_tmp6D6.YY37).f1=({struct _tuple2*_tmp6D7=_cycalloc(sizeof(*_tmp6D7));_tmp6D7->f1=
0;_tmp6D7->f2=_tmp6D1.f1;_tmp6D7->f3=_tmp6D1.f2;_tmp6D7;});_tmp6D6;});break;}}
case 246: _LL2F6: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp6D8;(
_tmp6D8.YY44).tag=48;(_tmp6D8.YY44).f1=(void*)(*Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f3;_tmp6D8;});break;case
247: _LL2F7: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp6D9;(
_tmp6D9.YY44).tag=48;(_tmp6D9.YY44).f1=(void*)((void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp6DA=_cycalloc(sizeof(*_tmp6DA));_tmp6DA[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp6DB;_tmp6DB.tag=20;_tmp6DB.f1=0;_tmp6DB;});_tmp6DA;}));_tmp6D9;});break;case
248: _LL2F8: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp6DC;(
_tmp6DC.YY44).tag=48;(_tmp6DC.YY44).f1=(void*)((void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp6DD=_cycalloc(sizeof(*_tmp6DD));_tmp6DD[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp6DE;_tmp6DE.tag=20;_tmp6DE.f1=Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp6DE;});_tmp6DD;}));
_tmp6DC;});break;case 249: _LL2F9: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp6DF;(_tmp6DF.YY44).tag=48;(_tmp6DF.YY44).f1=(void*)((void*)({struct Cyc_Absyn_RgnsEff_struct*
_tmp6E0=_cycalloc(sizeof(*_tmp6E0));_tmp6E0[0]=({struct Cyc_Absyn_RgnsEff_struct
_tmp6E1;_tmp6E1.tag=21;_tmp6E1.f1=(void*)Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp6E1;});_tmp6E0;}));
_tmp6DF;});break;case 250: _LL2FA: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp6E2;(_tmp6E2.YY44).tag=48;(_tmp6E2.YY44).f1=(void*)((void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp6E3=_cycalloc(sizeof(*_tmp6E3));_tmp6E3[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp6E4;_tmp6E4.tag=20;_tmp6E4.f1=({struct Cyc_List_List*_tmp6E5=_cycalloc(
sizeof(*_tmp6E5));_tmp6E5->hd=(void*)Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp6E5->tl=Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6E5;});_tmp6E4;});
_tmp6E3;}));_tmp6E2;});break;case 251: _LL2FB: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp6E6;(_tmp6E6.YY40).tag=44;(_tmp6E6.YY40).f1=({struct
Cyc_List_List*_tmp6E7=_cycalloc(sizeof(*_tmp6E7));_tmp6E7->hd=(void*)Cyc_yyget_YY44(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6E7->tl=0;_tmp6E7;});
_tmp6E6;});break;case 252: _LL2FC: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp6E8;(_tmp6E8.YY40).tag=44;(_tmp6E8.YY40).f1=({struct Cyc_List_List*_tmp6E9=
_cycalloc(sizeof(*_tmp6E9));_tmp6E9->hd=(void*)Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6E9->tl=Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp6E9;});_tmp6E8;});
break;case 253: _LL2FD: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp6EA;(_tmp6EA.YY30).tag=34;(_tmp6EA.YY30).f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp6EB=_cycalloc(sizeof(*_tmp6EB));_tmp6EB->tms=Cyc_yyget_YY28(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6EB;});_tmp6EA;});
break;case 254: _LL2FE: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 255: _LL2FF: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp6EC;(_tmp6EC.YY30).tag=34;(_tmp6EC.YY30).f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp6ED=_cycalloc(sizeof(*_tmp6ED));_tmp6ED->tms=Cyc_List_imp_append(Cyc_yyget_YY28(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_YY30(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tms);_tmp6ED;});
_tmp6EC;});break;case 256: _LL300: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)];break;case 257: _LL301: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp6EE;(_tmp6EE.YY30).tag=34;(_tmp6EE.YY30).f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp6EF=_cycalloc(sizeof(*_tmp6EF));_tmp6EF->tms=({struct Cyc_List_List*_tmp6F0=
_cycalloc(sizeof(*_tmp6F0));_tmp6F0->hd=(void*)((void*)({struct Cyc_Absyn_Carray_mod_struct*
_tmp6F1=_cycalloc(sizeof(*_tmp6F1));_tmp6F1[0]=({struct Cyc_Absyn_Carray_mod_struct
_tmp6F2;_tmp6F2.tag=0;_tmp6F2.f1=Cyc_yyget_YY51(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6F2.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp6F2;});
_tmp6F1;}));_tmp6F0->tl=0;_tmp6F0;});_tmp6EF;});_tmp6EE;});break;case 258: _LL302:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp6F3;(_tmp6F3.YY30).tag=
34;(_tmp6F3.YY30).f1=({struct Cyc_Parse_Abstractdeclarator*_tmp6F4=_cycalloc(
sizeof(*_tmp6F4));_tmp6F4->tms=({struct Cyc_List_List*_tmp6F5=_cycalloc(sizeof(*
_tmp6F5));_tmp6F5->hd=(void*)((void*)({struct Cyc_Absyn_Carray_mod_struct*_tmp6F6=
_cycalloc(sizeof(*_tmp6F6));_tmp6F6[0]=({struct Cyc_Absyn_Carray_mod_struct
_tmp6F7;_tmp6F7.tag=0;_tmp6F7.f1=Cyc_yyget_YY51(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6F7.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp6F7;});
_tmp6F6;}));_tmp6F5->tl=(Cyc_yyget_YY30(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]))->tms;_tmp6F5;});_tmp6F4;});_tmp6F3;});break;case 259:
_LL303: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp6F8;(_tmp6F8.YY30).tag=
34;(_tmp6F8.YY30).f1=({struct Cyc_Parse_Abstractdeclarator*_tmp6F9=_cycalloc(
sizeof(*_tmp6F9));_tmp6F9->tms=({struct Cyc_List_List*_tmp6FA=_cycalloc(sizeof(*
_tmp6FA));_tmp6FA->hd=(void*)((void*)({struct Cyc_Absyn_ConstArray_mod_struct*
_tmp6FB=_cycalloc(sizeof(*_tmp6FB));_tmp6FB[0]=({struct Cyc_Absyn_ConstArray_mod_struct
_tmp6FC;_tmp6FC.tag=1;_tmp6FC.f1=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp6FC.f2=Cyc_yyget_YY51(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6FC.f3=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp6FC;});
_tmp6FB;}));_tmp6FA->tl=0;_tmp6FA;});_tmp6F9;});_tmp6F8;});break;case 260: _LL304:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp6FD;(_tmp6FD.YY30).tag=
34;(_tmp6FD.YY30).f1=({struct Cyc_Parse_Abstractdeclarator*_tmp6FE=_cycalloc(
sizeof(*_tmp6FE));_tmp6FE->tms=({struct Cyc_List_List*_tmp6FF=_cycalloc(sizeof(*
_tmp6FF));_tmp6FF->hd=(void*)((void*)({struct Cyc_Absyn_ConstArray_mod_struct*
_tmp700=_cycalloc(sizeof(*_tmp700));_tmp700[0]=({struct Cyc_Absyn_ConstArray_mod_struct
_tmp701;_tmp701.tag=1;_tmp701.f1=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp701.f2=Cyc_yyget_YY51(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp701.f3=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp701;});
_tmp700;}));_tmp6FF->tl=(Cyc_yyget_YY30(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 4)]))->tms;_tmp6FF;});_tmp6FE;});_tmp6FD;});break;case 261:
_LL305: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp702;(_tmp702.YY30).tag=
34;(_tmp702.YY30).f1=({struct Cyc_Parse_Abstractdeclarator*_tmp703=_cycalloc(
sizeof(*_tmp703));_tmp703->tms=({struct Cyc_List_List*_tmp704=_cycalloc(sizeof(*
_tmp704));_tmp704->hd=(void*)((void*)({struct Cyc_Absyn_Function_mod_struct*
_tmp705=_cycalloc(sizeof(*_tmp705));_tmp705[0]=({struct Cyc_Absyn_Function_mod_struct
_tmp706;_tmp706.tag=3;_tmp706.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*
_tmp707=_cycalloc(sizeof(*_tmp707));_tmp707[0]=({struct Cyc_Absyn_WithTypes_struct
_tmp708;_tmp708.tag=1;_tmp708.f1=0;_tmp708.f2=0;_tmp708.f3=0;_tmp708.f4=Cyc_yyget_YY49(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp708.f5=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp708;});_tmp707;}));
_tmp706;});_tmp705;}));_tmp704->tl=0;_tmp704;});_tmp703;});_tmp702;});break;case
262: _LL306: {struct _tuple17 _tmp70A;struct Cyc_List_List*_tmp70B;int _tmp70C;struct
Cyc_Absyn_VarargInfo*_tmp70D;struct Cyc_Core_Opt*_tmp70E;struct Cyc_List_List*
_tmp70F;struct _tuple17*_tmp709=Cyc_yyget_YY39(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp70A=*_tmp709;_tmp70B=
_tmp70A.f1;_tmp70C=_tmp70A.f2;_tmp70D=_tmp70A.f3;_tmp70E=_tmp70A.f4;_tmp70F=
_tmp70A.f5;yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp710;(
_tmp710.YY30).tag=34;(_tmp710.YY30).f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp711=_cycalloc(sizeof(*_tmp711));_tmp711->tms=({struct Cyc_List_List*_tmp712=
_cycalloc(sizeof(*_tmp712));_tmp712->hd=(void*)((void*)({struct Cyc_Absyn_Function_mod_struct*
_tmp713=_cycalloc(sizeof(*_tmp713));_tmp713[0]=({struct Cyc_Absyn_Function_mod_struct
_tmp714;_tmp714.tag=3;_tmp714.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*
_tmp715=_cycalloc(sizeof(*_tmp715));_tmp715[0]=({struct Cyc_Absyn_WithTypes_struct
_tmp716;_tmp716.tag=1;_tmp716.f1=_tmp70B;_tmp716.f2=_tmp70C;_tmp716.f3=_tmp70D;
_tmp716.f4=_tmp70E;_tmp716.f5=_tmp70F;_tmp716;});_tmp715;}));_tmp714;});_tmp713;}));
_tmp712->tl=0;_tmp712;});_tmp711;});_tmp710;});break;}case 263: _LL307: yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp717;(_tmp717.YY30).tag=34;(
_tmp717.YY30).f1=({struct Cyc_Parse_Abstractdeclarator*_tmp718=_cycalloc(sizeof(*
_tmp718));_tmp718->tms=({struct Cyc_List_List*_tmp719=_cycalloc(sizeof(*_tmp719));
_tmp719->hd=(void*)((void*)({struct Cyc_Absyn_Function_mod_struct*_tmp71A=
_cycalloc(sizeof(*_tmp71A));_tmp71A[0]=({struct Cyc_Absyn_Function_mod_struct
_tmp71B;_tmp71B.tag=3;_tmp71B.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*
_tmp71C=_cycalloc(sizeof(*_tmp71C));_tmp71C[0]=({struct Cyc_Absyn_WithTypes_struct
_tmp71D;_tmp71D.tag=1;_tmp71D.f1=0;_tmp71D.f2=0;_tmp71D.f3=0;_tmp71D.f4=Cyc_yyget_YY49(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp71D.f5=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp71D;});_tmp71C;}));
_tmp71B;});_tmp71A;}));_tmp719->tl=(Cyc_yyget_YY30(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->tms;_tmp719;});_tmp718;});
_tmp717;});break;case 264: _LL308: {struct _tuple17 _tmp71F;struct Cyc_List_List*
_tmp720;int _tmp721;struct Cyc_Absyn_VarargInfo*_tmp722;struct Cyc_Core_Opt*_tmp723;
struct Cyc_List_List*_tmp724;struct _tuple17*_tmp71E=Cyc_yyget_YY39(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp71F=*_tmp71E;_tmp720=
_tmp71F.f1;_tmp721=_tmp71F.f2;_tmp722=_tmp71F.f3;_tmp723=_tmp71F.f4;_tmp724=
_tmp71F.f5;yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp725;(
_tmp725.YY30).tag=34;(_tmp725.YY30).f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp726=_cycalloc(sizeof(*_tmp726));_tmp726->tms=({struct Cyc_List_List*_tmp727=
_cycalloc(sizeof(*_tmp727));_tmp727->hd=(void*)((void*)({struct Cyc_Absyn_Function_mod_struct*
_tmp728=_cycalloc(sizeof(*_tmp728));_tmp728[0]=({struct Cyc_Absyn_Function_mod_struct
_tmp729;_tmp729.tag=3;_tmp729.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*
_tmp72A=_cycalloc(sizeof(*_tmp72A));_tmp72A[0]=({struct Cyc_Absyn_WithTypes_struct
_tmp72B;_tmp72B.tag=1;_tmp72B.f1=_tmp720;_tmp72B.f2=_tmp721;_tmp72B.f3=_tmp722;
_tmp72B.f4=_tmp723;_tmp72B.f5=_tmp724;_tmp72B;});_tmp72A;}));_tmp729;});_tmp728;}));
_tmp727->tl=(Cyc_yyget_YY30(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]))->tms;_tmp727;});_tmp726;});_tmp725;});break;}case 265: _LL309: {
struct Cyc_List_List*_tmp72C=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(
struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*
x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),Cyc_List_imp_rev(
Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp72D;(_tmp72D.YY30).tag=
34;(_tmp72D.YY30).f1=({struct Cyc_Parse_Abstractdeclarator*_tmp72E=_cycalloc(
sizeof(*_tmp72E));_tmp72E->tms=({struct Cyc_List_List*_tmp72F=_cycalloc(sizeof(*
_tmp72F));_tmp72F->hd=(void*)((void*)({struct Cyc_Absyn_TypeParams_mod_struct*
_tmp730=_cycalloc(sizeof(*_tmp730));_tmp730[0]=({struct Cyc_Absyn_TypeParams_mod_struct
_tmp731;_tmp731.tag=4;_tmp731.f1=_tmp72C;_tmp731.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp731.f3=0;
_tmp731;});_tmp730;}));_tmp72F->tl=(Cyc_yyget_YY30(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp72F;});_tmp72E;});
_tmp72D;});break;}case 266: _LL30A: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp732;(_tmp732.YY30).tag=34;(_tmp732.YY30).f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp733=_cycalloc(sizeof(*_tmp733));_tmp733->tms=({struct Cyc_List_List*_tmp734=
_cycalloc(sizeof(*_tmp734));_tmp734->hd=(void*)((void*)({struct Cyc_Absyn_Attributes_mod_struct*
_tmp735=_cycalloc(sizeof(*_tmp735));_tmp735[0]=({struct Cyc_Absyn_Attributes_mod_struct
_tmp736;_tmp736.tag=5;_tmp736.f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp736.f2=Cyc_yyget_YY45(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp736;});_tmp735;}));
_tmp734->tl=(Cyc_yyget_YY30(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]))->tms;_tmp734;});_tmp733;});_tmp732;});break;case 267: _LL30B:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 268:
_LL30C: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
269: _LL30D: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;
case 270: _LL30E: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];
break;case 271: _LL30F: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 272: _LL310: yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)];break;case 273: _LL311: yyval=(union Cyc_YYSTYPE_union)({union
Cyc_YYSTYPE_union _tmp737;(_tmp737.YY9).tag=13;(_tmp737.YY9).f1=Cyc_Absyn_new_stmt((
void*)({struct Cyc_Absyn_ResetRegion_s_struct*_tmp738=_cycalloc(sizeof(*_tmp738));
_tmp738[0]=({struct Cyc_Absyn_ResetRegion_s_struct _tmp739;_tmp739.tag=15;_tmp739.f1=
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);
_tmp739;});_tmp738;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp737;});
break;case 274: _LL312: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp73A;(_tmp73A.YY4).tag=8;(_tmp73A.YY4).f1=0;_tmp73A;});break;case 275: _LL313:
if(Cyc_zstrcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),({const char*_tmp73B="open";
_tag_dyneither(_tmp73B,sizeof(char),5);}))!= 0)Cyc_Parse_err(({const char*_tmp73C="expecting `open'";
_tag_dyneither(_tmp73C,sizeof(char),17);}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));yyval=(union
Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp73D;(_tmp73D.YY4).tag=8;(_tmp73D.YY4).f1=(
struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp73D;});break;case 276: _LL314: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp73E;(_tmp73E.YY9).tag=13;(_tmp73E.YY9).f1=Cyc_Absyn_new_stmt((
void*)({struct Cyc_Absyn_Label_s_struct*_tmp73F=_cycalloc(sizeof(*_tmp73F));
_tmp73F[0]=({struct Cyc_Absyn_Label_s_struct _tmp740;_tmp740.tag=12;_tmp740.f1=({
struct _dyneither_ptr*_tmp741=_cycalloc(sizeof(*_tmp741));_tmp741[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp741;});_tmp740.f2=
Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp740;});
_tmp73F;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp73E;});break;case 277: _LL315: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp742;(_tmp742.YY9).tag=13;(_tmp742.YY9).f1=Cyc_Absyn_skip_stmt(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp742;});break;case 278: _LL316: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp743;(_tmp743.YY9).tag=13;(_tmp743.YY9).f1=Cyc_Absyn_exp_stmt(
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp743;});
break;case 279: _LL317: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp744;(_tmp744.YY9).tag=13;(_tmp744.YY9).f1=Cyc_Absyn_skip_stmt(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp744;});
break;case 280: _LL318: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)];break;case 281: _LL319: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp745;(_tmp745.YY9).tag=13;(_tmp745.YY9).f1=Cyc_Parse_flatten_declarations(Cyc_yyget_YY18(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Absyn_skip_stmt(0));
_tmp745;});break;case 282: _LL31A: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp746;(_tmp746.YY9).tag=13;(_tmp746.YY9).f1=Cyc_Parse_flatten_declarations(Cyc_yyget_YY18(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp746;});break;case 283:
_LL31B: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
284: _LL31C: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp747;(
_tmp747.YY9).tag=13;(_tmp747.YY9).f1=Cyc_Absyn_seq_stmt(Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp747;});
break;case 285: _LL31D: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp748;(_tmp748.YY9).tag=13;(_tmp748.YY9).f1=Cyc_Parse_flatten_decl(Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Fn_d_struct*_tmp749=_cycalloc(sizeof(*_tmp749));_tmp749[
0]=({struct Cyc_Absyn_Fn_d_struct _tmp74A;_tmp74A.tag=1;_tmp74A.f1=Cyc_yyget_YY17(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp74A;});_tmp749;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),Cyc_Absyn_skip_stmt(0));_tmp748;});break;case 286:
_LL31E: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp74B;(_tmp74B.YY9).tag=
13;(_tmp74B.YY9).f1=Cyc_Parse_flatten_decl(Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Fn_d_struct*
_tmp74C=_cycalloc(sizeof(*_tmp74C));_tmp74C[0]=({struct Cyc_Absyn_Fn_d_struct
_tmp74D;_tmp74D.tag=1;_tmp74D.f1=Cyc_yyget_YY17(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp74D;});_tmp74C;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line)),Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]));_tmp74B;});break;case 287: _LL31F: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp74E;(_tmp74E.YY9).tag=13;(_tmp74E.YY9).f1=Cyc_Absyn_ifthenelse_stmt(
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Absyn_skip_stmt(0),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp74E;});break;case 288: _LL320: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp74F;(_tmp74F.YY9).tag=13;(_tmp74F.YY9).f1=Cyc_Absyn_ifthenelse_stmt(
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp74F;});
break;case 289: _LL321: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp750;(_tmp750.YY9).tag=13;(_tmp750.YY9).f1=Cyc_Absyn_switch_stmt(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_YY10(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp750;});
break;case 290: _LL322: {struct Cyc_Absyn_Exp*_tmp751=Cyc_Absyn_new_exp((void*)({
struct Cyc_Absyn_UnknownId_e_struct*_tmp753=_cycalloc(sizeof(*_tmp753));_tmp753[0]=({
struct Cyc_Absyn_UnknownId_e_struct _tmp754;_tmp754.tag=2;_tmp754.f1=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp754;});_tmp753;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line));yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp752;(_tmp752.YY9).tag=13;(_tmp752.YY9).f1=Cyc_Absyn_switch_stmt(_tmp751,Cyc_yyget_YY10(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp752;});
break;}case 291: _LL323: {struct Cyc_Absyn_Exp*_tmp755=Cyc_Absyn_tuple_exp(Cyc_yyget_YY5(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));yyval=(union
Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp756;(_tmp756.YY9).tag=13;(_tmp756.YY9).f1=
Cyc_Absyn_switch_stmt(_tmp755,Cyc_yyget_YY10(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp756;});
break;}case 292: _LL324: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp757;(_tmp757.YY9).tag=13;(_tmp757.YY9).f1=Cyc_Absyn_trycatch_stmt(Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_YY10(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp757;});
break;case 293: _LL325: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp758;(_tmp758.YY10).tag=14;(_tmp758.YY10).f1=0;_tmp758;});break;case 294:
_LL326: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp759;(_tmp759.YY10).tag=
14;(_tmp759.YY10).f1=({struct Cyc_List_List*_tmp75A=_cycalloc(sizeof(*_tmp75A));
_tmp75A->hd=({struct Cyc_Absyn_Switch_clause*_tmp75B=_cycalloc(sizeof(*_tmp75B));
_tmp75B->pattern=Cyc_Absyn_new_pat((void*)0,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line));_tmp75B->pat_vars=
0;_tmp75B->where_clause=0;_tmp75B->body=Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp75B->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp75B;});
_tmp75A->tl=0;_tmp75A;});_tmp759;});break;case 295: _LL327: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp75C;(_tmp75C.YY10).tag=14;(_tmp75C.YY10).f1=({struct
Cyc_List_List*_tmp75D=_cycalloc(sizeof(*_tmp75D));_tmp75D->hd=({struct Cyc_Absyn_Switch_clause*
_tmp75E=_cycalloc(sizeof(*_tmp75E));_tmp75E->pattern=Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp75E->pat_vars=0;
_tmp75E->where_clause=0;_tmp75E->body=Cyc_Absyn_fallthru_stmt(0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp75E->loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp75E;});_tmp75D->tl=Cyc_yyget_YY10(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp75D;});_tmp75C;});
break;case 296: _LL328: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp75F;(_tmp75F.YY10).tag=14;(_tmp75F.YY10).f1=({struct Cyc_List_List*_tmp760=
_cycalloc(sizeof(*_tmp760));_tmp760->hd=({struct Cyc_Absyn_Switch_clause*_tmp761=
_cycalloc(sizeof(*_tmp761));_tmp761->pattern=Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp761->pat_vars=0;
_tmp761->where_clause=0;_tmp761->body=Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp761->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line);_tmp761;});
_tmp760->tl=Cyc_yyget_YY10(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp760;});_tmp75F;});break;case 297: _LL329: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp762;(_tmp762.YY10).tag=14;(_tmp762.YY10).f1=({struct
Cyc_List_List*_tmp763=_cycalloc(sizeof(*_tmp763));_tmp763->hd=({struct Cyc_Absyn_Switch_clause*
_tmp764=_cycalloc(sizeof(*_tmp764));_tmp764->pattern=Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp764->pat_vars=0;
_tmp764->where_clause=(struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp764->body=Cyc_Absyn_fallthru_stmt(
0,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));_tmp764->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp764;});
_tmp763->tl=Cyc_yyget_YY10(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp763;});_tmp762;});break;case 298: _LL32A: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp765;(_tmp765.YY10).tag=14;(_tmp765.YY10).f1=({struct
Cyc_List_List*_tmp766=_cycalloc(sizeof(*_tmp766));_tmp766->hd=({struct Cyc_Absyn_Switch_clause*
_tmp767=_cycalloc(sizeof(*_tmp767));_tmp767->pattern=Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 5)]);_tmp767->pat_vars=0;
_tmp767->where_clause=(struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp767->body=Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp767->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp767;});
_tmp766->tl=Cyc_yyget_YY10(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp766;});_tmp765;});break;case 299: _LL32B: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp768;(_tmp768.YY9).tag=13;(_tmp768.YY9).f1=Cyc_Absyn_while_stmt(
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp768;});
break;case 300: _LL32C: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp769;(_tmp769.YY9).tag=13;(_tmp769.YY9).f1=Cyc_Absyn_do_stmt(Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp769;});
break;case 301: _LL32D: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp76A;(_tmp76A.YY9).tag=13;(_tmp76A.YY9).f1=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(
0),Cyc_Absyn_true_exp(0),Cyc_Absyn_false_exp(0),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp76A;});
break;case 302: _LL32E: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp76B;(_tmp76B.YY9).tag=13;(_tmp76B.YY9).f1=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(
0),Cyc_Absyn_true_exp(0),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]),Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 6)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp76B;});break;case 303: _LL32F: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp76C;(_tmp76C.YY9).tag=13;(_tmp76C.YY9).f1=Cyc_Absyn_for_stmt(
Cyc_Absyn_false_exp(0),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]),Cyc_Absyn_false_exp(0),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp76C;});
break;case 304: _LL330: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp76D;(_tmp76D.YY9).tag=13;(_tmp76D.YY9).f1=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(
0),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp76D;});
break;case 305: _LL331: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp76E;(_tmp76E.YY9).tag=13;(_tmp76E.YY9).f1=Cyc_Absyn_for_stmt(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_Absyn_true_exp(
0),Cyc_Absyn_false_exp(0),Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp76E;});
break;case 306: _LL332: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp76F;(_tmp76F.YY9).tag=13;(_tmp76F.YY9).f1=Cyc_Absyn_for_stmt(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]),Cyc_Absyn_true_exp(
0),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp76F;});
break;case 307: _LL333: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp770;(_tmp770.YY9).tag=13;(_tmp770.YY9).f1=Cyc_Absyn_for_stmt(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_Absyn_false_exp(0),
Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp770;});
break;case 308: _LL334: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp771;(_tmp771.YY9).tag=13;(_tmp771.YY9).f1=Cyc_Absyn_for_stmt(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 8)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp771;});
break;case 309: _LL335: {struct Cyc_List_List*_tmp772=Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);struct Cyc_Absyn_Stmt*
_tmp773=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(0),Cyc_Absyn_false_exp(
0),Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp774;(_tmp774.YY9).tag=13;(_tmp774.YY9).f1=Cyc_Parse_flatten_declarations(
_tmp772,_tmp773);_tmp774;});break;}case 310: _LL336: {struct Cyc_List_List*_tmp775=
Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);
struct Cyc_Absyn_Stmt*_tmp776=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_Absyn_false_exp(
0),Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp777;(_tmp777.YY9).tag=13;(_tmp777.YY9).f1=Cyc_Parse_flatten_declarations(
_tmp775,_tmp776);_tmp777;});break;}case 311: _LL337: {struct Cyc_List_List*_tmp778=
Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);
struct Cyc_Absyn_Stmt*_tmp779=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(
0),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp77A;(_tmp77A.YY9).tag=13;(_tmp77A.YY9).f1=Cyc_Parse_flatten_declarations(
_tmp778,_tmp779);_tmp77A;});break;}case 312: _LL338: {struct Cyc_List_List*_tmp77B=
Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]);
struct Cyc_Absyn_Stmt*_tmp77C=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp77D;(_tmp77D.YY9).tag=13;(_tmp77D.YY9).f1=Cyc_Parse_flatten_declarations(
_tmp77B,_tmp77C);_tmp77D;});break;}case 313: _LL339: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp77E;(_tmp77E.YY9).tag=13;(_tmp77E.YY9).f1=Cyc_Absyn_goto_stmt(({
struct _dyneither_ptr*_tmp77F=_cycalloc(sizeof(*_tmp77F));_tmp77F[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp77F;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp77E;});
break;case 314: _LL33A: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp780;(_tmp780.YY9).tag=13;(_tmp780.YY9).f1=Cyc_Absyn_continue_stmt(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp780;});
break;case 315: _LL33B: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp781;(_tmp781.YY9).tag=13;(_tmp781.YY9).f1=Cyc_Absyn_break_stmt(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp781;});
break;case 316: _LL33C: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp782;(_tmp782.YY9).tag=13;(_tmp782.YY9).f1=Cyc_Absyn_return_stmt(0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp782;});
break;case 317: _LL33D: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp783;(_tmp783.YY9).tag=13;(_tmp783.YY9).f1=Cyc_Absyn_return_stmt((struct Cyc_Absyn_Exp*)
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp783;});
break;case 318: _LL33E: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp784;(_tmp784.YY9).tag=13;(_tmp784.YY9).f1=Cyc_Absyn_fallthru_stmt(0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp784;});
break;case 319: _LL33F: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp785;(_tmp785.YY9).tag=13;(_tmp785.YY9).f1=Cyc_Absyn_fallthru_stmt(0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));_tmp785;});
break;case 320: _LL340: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp786;(_tmp786.YY9).tag=13;(_tmp786.YY9).f1=Cyc_Absyn_fallthru_stmt(Cyc_yyget_YY5(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).last_line));_tmp786;});
break;case 321: _LL341: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 322: _LL342: yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)];break;case 323: _LL343: yyval=(union Cyc_YYSTYPE_union)({union
Cyc_YYSTYPE_union _tmp787;(_tmp787.YY11).tag=15;(_tmp787.YY11).f1=Cyc_Absyn_exp_pat(
Cyc_Absyn_conditional_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)])),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp787;});
break;case 324: _LL344: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 325: _LL345: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp788;(_tmp788.YY11).tag=15;(_tmp788.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_or_exp(
Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));_tmp788;});break;case 326: _LL346: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 327: _LL347: yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp789;(_tmp789.YY11).tag=15;(
_tmp789.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_and_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY11(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp789;});
break;case 328: _LL348: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 329: _LL349: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp78A;(_tmp78A.YY11).tag=15;(_tmp78A.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((
void*)14,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp78A;});
break;case 330: _LL34A: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 331: _LL34B: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp78B;(_tmp78B.YY11).tag=15;(_tmp78B.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((
void*)15,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp78B;});
break;case 332: _LL34C: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 333: _LL34D: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp78C;(_tmp78C.YY11).tag=15;(_tmp78C.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((
void*)13,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp78C;});
break;case 334: _LL34E: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 335: _LL34F: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp78D;(_tmp78D.YY11).tag=15;(_tmp78D.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_eq_exp(
Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));_tmp78D;});break;case 336: _LL350: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp78E;(_tmp78E.YY11).tag=15;(_tmp78E.YY11).f1=Cyc_Absyn_exp_pat(
Cyc_Absyn_neq_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp78E;});
break;case 337: _LL351: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 338: _LL352: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp78F;(_tmp78F.YY11).tag=15;(_tmp78F.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_lt_exp(
Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));_tmp78F;});break;case 339: _LL353: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp790;(_tmp790.YY11).tag=15;(_tmp790.YY11).f1=Cyc_Absyn_exp_pat(
Cyc_Absyn_gt_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp790;});
break;case 340: _LL354: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp791;(_tmp791.YY11).tag=15;(_tmp791.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_lte_exp(
Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));_tmp791;});break;case 341: _LL355: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp792;(_tmp792.YY11).tag=15;(_tmp792.YY11).f1=Cyc_Absyn_exp_pat(
Cyc_Absyn_gte_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp792;});
break;case 342: _LL356: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 343: _LL357: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp793;(_tmp793.YY11).tag=15;(_tmp793.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((
void*)16,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp793;});
break;case 344: _LL358: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp794;(_tmp794.YY11).tag=15;(_tmp794.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((
void*)17,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp794;});
break;case 345: _LL359: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 346: _LL35A: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp795;(_tmp795.YY11).tag=15;(_tmp795.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((
void*)0,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp795;});
break;case 347: _LL35B: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp796;(_tmp796.YY11).tag=15;(_tmp796.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((
void*)2,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp796;});
break;case 348: _LL35C: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 349: _LL35D: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp797;(_tmp797.YY11).tag=15;(_tmp797.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((
void*)1,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp797;});
break;case 350: _LL35E: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp798;(_tmp798.YY11).tag=15;(_tmp798.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((
void*)3,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp798;});
break;case 351: _LL35F: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp799;(_tmp799.YY11).tag=15;(_tmp799.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((
void*)4,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp799;});
break;case 352: _LL360: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 353: _LL361: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp79A;(_tmp79A.YY11).tag=15;(_tmp79A.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_cast_exp((*
Cyc_yyget_YY37(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])).f3,
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),1,(void*)
0,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));_tmp79A;});break;case 354: _LL362: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 355: _LL363: yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp79B;(_tmp79B.YY11).tag=15;(
_tmp79B.YY11).f1=Cyc_Absyn_exp_pat(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp79B;});break;case 356:
_LL364: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp79C;(_tmp79C.YY11).tag=
15;(_tmp79C.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_prim1_exp(Cyc_yyget_YY7(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp79C;});
break;case 357: _LL365: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp79D;(_tmp79D.YY11).tag=15;(_tmp79D.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_sizeoftyp_exp((*
Cyc_yyget_YY37(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])).f3,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));_tmp79D;});break;case 358: _LL366: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp79E;(_tmp79E.YY11).tag=15;(_tmp79E.YY11).f1=Cyc_Absyn_exp_pat(
Cyc_Absyn_sizeofexp_exp(Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));_tmp79E;});break;case 359: _LL367: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp79F;(_tmp79F.YY11).tag=15;(_tmp79F.YY11).f1=Cyc_Absyn_exp_pat(
Cyc_Absyn_offsetof_exp((*Cyc_yyget_YY37(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)])).f3,(void*)({struct Cyc_Absyn_StructField_struct*_tmp7A0=
_cycalloc(sizeof(*_tmp7A0));_tmp7A0[0]=({struct Cyc_Absyn_StructField_struct
_tmp7A1;_tmp7A1.tag=0;_tmp7A1.f1=({struct _dyneither_ptr*_tmp7A2=_cycalloc(
sizeof(*_tmp7A2));_tmp7A2[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp7A2;});_tmp7A1;});
_tmp7A0;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));_tmp79F;});break;case 360: _LL368: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp7A3;(_tmp7A3.YY11).tag=15;(_tmp7A3.YY11).f1=Cyc_Absyn_exp_pat(
Cyc_Absyn_offsetof_exp((*Cyc_yyget_YY37(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)])).f3,(void*)({struct Cyc_Absyn_TupleIndex_struct*_tmp7A4=
_cycalloc_atomic(sizeof(*_tmp7A4));_tmp7A4[0]=({struct Cyc_Absyn_TupleIndex_struct
_tmp7A5;_tmp7A5.tag=1;_tmp7A5.f1=(unsigned int)(Cyc_yyget_Int_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])).f2;_tmp7A5;});_tmp7A4;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));_tmp7A3;});break;case 361: _LL369: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 362: _LL36A: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 363: _LL36B:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp7A6;(_tmp7A6.YY11).tag=
15;(_tmp7A6.YY11).f1=Cyc_Absyn_new_pat((void*)0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp7A6;});
break;case 364: _LL36C: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp7A7;(_tmp7A7.YY11).tag=15;(_tmp7A7.YY11).f1=Cyc_Absyn_exp_pat(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp7A7;});break;
case 365: _LL36D: {struct Cyc_Absyn_Exp*e=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);{void*_tmp7A8=(void*)e->r;
union Cyc_Absyn_Cnst_union _tmp7A9;void*_tmp7AA;char _tmp7AB;union Cyc_Absyn_Cnst_union
_tmp7AC;void*_tmp7AD;short _tmp7AE;union Cyc_Absyn_Cnst_union _tmp7AF;void*_tmp7B0;
int _tmp7B1;union Cyc_Absyn_Cnst_union _tmp7B2;struct _dyneither_ptr _tmp7B3;union Cyc_Absyn_Cnst_union
_tmp7B4;union Cyc_Absyn_Cnst_union _tmp7B5;union Cyc_Absyn_Cnst_union _tmp7B6;_LL370:
if(*((int*)_tmp7A8)!= 0)goto _LL372;_tmp7A9=((struct Cyc_Absyn_Const_e_struct*)
_tmp7A8)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)_tmp7A8)->f1).Char_c).tag != 
0)goto _LL372;_tmp7AA=(_tmp7A9.Char_c).f1;_tmp7AB=(_tmp7A9.Char_c).f2;_LL371:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp7B7;(_tmp7B7.YY11).tag=
15;(_tmp7B7.YY11).f1=Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Char_p_struct*
_tmp7B8=_cycalloc_atomic(sizeof(*_tmp7B8));_tmp7B8[0]=({struct Cyc_Absyn_Char_p_struct
_tmp7B9;_tmp7B9.tag=8;_tmp7B9.f1=_tmp7AB;_tmp7B9;});_tmp7B8;}),e->loc);_tmp7B7;});
goto _LL36F;_LL372: if(*((int*)_tmp7A8)!= 0)goto _LL374;_tmp7AC=((struct Cyc_Absyn_Const_e_struct*)
_tmp7A8)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)_tmp7A8)->f1).Short_c).tag != 
1)goto _LL374;_tmp7AD=(_tmp7AC.Short_c).f1;_tmp7AE=(_tmp7AC.Short_c).f2;_LL373:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp7BA;(_tmp7BA.YY11).tag=
15;(_tmp7BA.YY11).f1=Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Int_p_struct*
_tmp7BB=_cycalloc(sizeof(*_tmp7BB));_tmp7BB[0]=({struct Cyc_Absyn_Int_p_struct
_tmp7BC;_tmp7BC.tag=7;_tmp7BC.f1=(void*)_tmp7AD;_tmp7BC.f2=(int)_tmp7AE;_tmp7BC;});
_tmp7BB;}),e->loc);_tmp7BA;});goto _LL36F;_LL374: if(*((int*)_tmp7A8)!= 0)goto
_LL376;_tmp7AF=((struct Cyc_Absyn_Const_e_struct*)_tmp7A8)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)
_tmp7A8)->f1).Int_c).tag != 2)goto _LL376;_tmp7B0=(_tmp7AF.Int_c).f1;_tmp7B1=(
_tmp7AF.Int_c).f2;_LL375: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp7BD;(_tmp7BD.YY11).tag=15;(_tmp7BD.YY11).f1=Cyc_Absyn_new_pat((void*)({
struct Cyc_Absyn_Int_p_struct*_tmp7BE=_cycalloc(sizeof(*_tmp7BE));_tmp7BE[0]=({
struct Cyc_Absyn_Int_p_struct _tmp7BF;_tmp7BF.tag=7;_tmp7BF.f1=(void*)_tmp7B0;
_tmp7BF.f2=_tmp7B1;_tmp7BF;});_tmp7BE;}),e->loc);_tmp7BD;});goto _LL36F;_LL376:
if(*((int*)_tmp7A8)!= 0)goto _LL378;_tmp7B2=((struct Cyc_Absyn_Const_e_struct*)
_tmp7A8)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)_tmp7A8)->f1).Float_c).tag != 
4)goto _LL378;_tmp7B3=(_tmp7B2.Float_c).f1;_LL377: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp7C0;(_tmp7C0.YY11).tag=15;(_tmp7C0.YY11).f1=Cyc_Absyn_new_pat((
void*)({struct Cyc_Absyn_Float_p_struct*_tmp7C1=_cycalloc(sizeof(*_tmp7C1));
_tmp7C1[0]=({struct Cyc_Absyn_Float_p_struct _tmp7C2;_tmp7C2.tag=9;_tmp7C2.f1=
_tmp7B3;_tmp7C2;});_tmp7C1;}),e->loc);_tmp7C0;});goto _LL36F;_LL378: if(*((int*)
_tmp7A8)!= 0)goto _LL37A;_tmp7B4=((struct Cyc_Absyn_Const_e_struct*)_tmp7A8)->f1;
if(((((struct Cyc_Absyn_Const_e_struct*)_tmp7A8)->f1).Null_c).tag != 6)goto _LL37A;
_LL379: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp7C3;(_tmp7C3.YY11).tag=
15;(_tmp7C3.YY11).f1=Cyc_Absyn_new_pat((void*)1,e->loc);_tmp7C3;});goto _LL36F;
_LL37A: if(*((int*)_tmp7A8)!= 0)goto _LL37C;_tmp7B5=((struct Cyc_Absyn_Const_e_struct*)
_tmp7A8)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)_tmp7A8)->f1).String_c).tag
!= 5)goto _LL37C;_LL37B: Cyc_Parse_err(({const char*_tmp7C4="strings cannot occur within patterns";
_tag_dyneither(_tmp7C4,sizeof(char),37);}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));goto _LL36F;
_LL37C: if(*((int*)_tmp7A8)!= 0)goto _LL37E;_tmp7B6=((struct Cyc_Absyn_Const_e_struct*)
_tmp7A8)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)_tmp7A8)->f1).LongLong_c).tag
!= 3)goto _LL37E;_LL37D: Cyc_Parse_unimp(({const char*_tmp7C5="long long's in patterns";
_tag_dyneither(_tmp7C5,sizeof(char),24);}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));goto _LL36F;
_LL37E:;_LL37F: Cyc_Parse_err(({const char*_tmp7C6="bad constant in case";
_tag_dyneither(_tmp7C6,sizeof(char),21);}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_LL36F:;}break;}
case 366: _LL36E: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp7C7;(
_tmp7C7.YY11).tag=15;(_tmp7C7.YY11).f1=Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_UnknownId_p_struct*
_tmp7C8=_cycalloc(sizeof(*_tmp7C8));_tmp7C8[0]=({struct Cyc_Absyn_UnknownId_p_struct
_tmp7C9;_tmp7C9.tag=12;_tmp7C9.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp7C9;});_tmp7C8;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp7C7;});
break;case 367: _LL380: if(Cyc_strcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),({const char*_tmp7CA="as";
_tag_dyneither(_tmp7CA,sizeof(char),3);}))!= 0)Cyc_Parse_err(({const char*_tmp7CB="expecting `as'";
_tag_dyneither(_tmp7CB,sizeof(char),15);}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));yyval=(union
Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp7CC;(_tmp7CC.YY11).tag=15;(_tmp7CC.YY11).f1=
Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Var_p_struct*_tmp7CD=_cycalloc(
sizeof(*_tmp7CD));_tmp7CD[0]=({struct Cyc_Absyn_Var_p_struct _tmp7CE;_tmp7CE.tag=0;
_tmp7CE.f1=Cyc_Absyn_new_vardecl(({struct _tuple1*_tmp7CF=_cycalloc(sizeof(*
_tmp7CF));_tmp7CF->f1=(union Cyc_Absyn_Nmspace_union)({union Cyc_Absyn_Nmspace_union
_tmp7D1;(_tmp7D1.Loc_n).tag=0;_tmp7D1;});_tmp7CF->f2=({struct _dyneither_ptr*
_tmp7D0=_cycalloc(sizeof(*_tmp7D0));_tmp7D0[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp7D0;});_tmp7CF;}),(
void*)0,0);_tmp7CE.f2=Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp7CE;});_tmp7CD;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line));_tmp7CC;});
break;case 368: _LL381: {struct Cyc_List_List*_tmp7D3;int _tmp7D4;struct _tuple13
_tmp7D2=*Cyc_yyget_YY12(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
1)]);_tmp7D3=_tmp7D2.f1;_tmp7D4=_tmp7D2.f2;yyval=(union Cyc_YYSTYPE_union)({union
Cyc_YYSTYPE_union _tmp7D5;(_tmp7D5.YY11).tag=15;(_tmp7D5.YY11).f1=Cyc_Absyn_new_pat((
void*)({struct Cyc_Absyn_Tuple_p_struct*_tmp7D6=_cycalloc(sizeof(*_tmp7D6));
_tmp7D6[0]=({struct Cyc_Absyn_Tuple_p_struct _tmp7D7;_tmp7D7.tag=3;_tmp7D7.f1=
_tmp7D3;_tmp7D7.f2=_tmp7D4;_tmp7D7;});_tmp7D6;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp7D5;});
break;}case 369: _LL382: {struct Cyc_List_List*_tmp7D9;int _tmp7DA;struct _tuple13
_tmp7D8=*Cyc_yyget_YY12(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
1)]);_tmp7D9=_tmp7D8.f1;_tmp7DA=_tmp7D8.f2;yyval=(union Cyc_YYSTYPE_union)({union
Cyc_YYSTYPE_union _tmp7DB;(_tmp7DB.YY11).tag=15;(_tmp7DB.YY11).f1=Cyc_Absyn_new_pat((
void*)({struct Cyc_Absyn_UnknownCall_p_struct*_tmp7DC=_cycalloc(sizeof(*_tmp7DC));
_tmp7DC[0]=({struct Cyc_Absyn_UnknownCall_p_struct _tmp7DD;_tmp7DD.tag=13;_tmp7DD.f1=
Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);
_tmp7DD.f2=_tmp7D9;_tmp7DD.f3=_tmp7DA;_tmp7DD;});_tmp7DC;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp7DB;});
break;}case 370: _LL383: {struct Cyc_List_List*_tmp7DF;int _tmp7E0;struct _tuple13
_tmp7DE=*Cyc_yyget_YY16(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
1)]);_tmp7DF=_tmp7DE.f1;_tmp7E0=_tmp7DE.f2;{struct Cyc_List_List*_tmp7E1=((struct
Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),
struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line),Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]));yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp7E2;(_tmp7E2.YY11).tag=15;(_tmp7E2.YY11).f1=Cyc_Absyn_new_pat((void*)({
struct Cyc_Absyn_Aggr_p_struct*_tmp7E3=_cycalloc(sizeof(*_tmp7E3));_tmp7E3[0]=({
struct Cyc_Absyn_Aggr_p_struct _tmp7E4;_tmp7E4.tag=5;_tmp7E4.f1=({struct Cyc_Absyn_AggrInfo
_tmp7E5;_tmp7E5.aggr_info=(union Cyc_Absyn_AggrInfoU_union)({union Cyc_Absyn_AggrInfoU_union
_tmp7E6;(_tmp7E6.UnknownAggr).tag=0;(_tmp7E6.UnknownAggr).f1=(void*)((void*)0);(
_tmp7E6.UnknownAggr).f2=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 4)]);_tmp7E6;});_tmp7E5.targs=0;_tmp7E5;});_tmp7E4.f2=
_tmp7E1;_tmp7E4.f3=_tmp7DF;_tmp7E4.f4=_tmp7E0;_tmp7E4;});_tmp7E3;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp7E2;});
break;}}case 371: _LL384: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp7E7;(_tmp7E7.YY11).tag=15;(_tmp7E7.YY11).f1=Cyc_Absyn_new_pat((void*)({
struct Cyc_Absyn_Pointer_p_struct*_tmp7E8=_cycalloc(sizeof(*_tmp7E8));_tmp7E8[0]=({
struct Cyc_Absyn_Pointer_p_struct _tmp7E9;_tmp7E9.tag=4;_tmp7E9.f1=Cyc_yyget_YY11(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp7E9;});_tmp7E8;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp7E7;});break;case 372: _LL385: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp7EA;(_tmp7EA.YY11).tag=15;(_tmp7EA.YY11).f1=Cyc_Absyn_new_pat((
void*)({struct Cyc_Absyn_Pointer_p_struct*_tmp7EB=_cycalloc(sizeof(*_tmp7EB));
_tmp7EB[0]=({struct Cyc_Absyn_Pointer_p_struct _tmp7EC;_tmp7EC.tag=4;_tmp7EC.f1=
Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Pointer_p_struct*_tmp7ED=_cycalloc(
sizeof(*_tmp7ED));_tmp7ED[0]=({struct Cyc_Absyn_Pointer_p_struct _tmp7EE;_tmp7EE.tag=
4;_tmp7EE.f1=Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp7EE;});_tmp7ED;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp7EC;});
_tmp7EB;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp7EA;});break;case 373: _LL386: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp7EF;(_tmp7EF.YY11).tag=15;(_tmp7EF.YY11).f1=Cyc_Absyn_new_pat((
void*)({struct Cyc_Absyn_Reference_p_struct*_tmp7F0=_cycalloc(sizeof(*_tmp7F0));
_tmp7F0[0]=({struct Cyc_Absyn_Reference_p_struct _tmp7F1;_tmp7F1.tag=1;_tmp7F1.f1=
Cyc_Absyn_new_vardecl(({struct _tuple1*_tmp7F2=_cycalloc(sizeof(*_tmp7F2));
_tmp7F2->f1=(union Cyc_Absyn_Nmspace_union)({union Cyc_Absyn_Nmspace_union _tmp7F4;(
_tmp7F4.Loc_n).tag=0;_tmp7F4;});_tmp7F2->f2=({struct _dyneither_ptr*_tmp7F3=
_cycalloc(sizeof(*_tmp7F3));_tmp7F3[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp7F3;});_tmp7F2;}),(void*)
0,0);_tmp7F1.f2=Cyc_Absyn_new_pat((void*)0,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp7F1;});
_tmp7F0;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp7EF;});break;case 374: _LL387: if(Cyc_strcmp((
struct _dyneither_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]),({const char*_tmp7F5="as";_tag_dyneither(_tmp7F5,sizeof(
char),3);}))!= 0)Cyc_Parse_err(({const char*_tmp7F6="expecting `as'";
_tag_dyneither(_tmp7F6,sizeof(char),15);}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));yyval=(union
Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp7F7;(_tmp7F7.YY11).tag=15;(_tmp7F7.YY11).f1=
Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Reference_p_struct*_tmp7F8=_cycalloc(
sizeof(*_tmp7F8));_tmp7F8[0]=({struct Cyc_Absyn_Reference_p_struct _tmp7F9;_tmp7F9.tag=
1;_tmp7F9.f1=Cyc_Absyn_new_vardecl(({struct _tuple1*_tmp7FA=_cycalloc(sizeof(*
_tmp7FA));_tmp7FA->f1=(union Cyc_Absyn_Nmspace_union)({union Cyc_Absyn_Nmspace_union
_tmp7FC;(_tmp7FC.Loc_n).tag=0;_tmp7FC;});_tmp7FA->f2=({struct _dyneither_ptr*
_tmp7FB=_cycalloc(sizeof(*_tmp7FB));_tmp7FB[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp7FB;});_tmp7FA;}),(
void*)0,0);_tmp7F9.f2=Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp7F9;});_tmp7F8;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line));_tmp7F7;});
break;case 375: _LL388: {void*_tmp7FD=Cyc_Parse_id2type(Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Tcutil_kind_to_bound((
void*)7));yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp7FE;(_tmp7FE.YY11).tag=
15;(_tmp7FE.YY11).f1=Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_TagInt_p_struct*
_tmp7FF=_cycalloc(sizeof(*_tmp7FF));_tmp7FF[0]=({struct Cyc_Absyn_TagInt_p_struct
_tmp800;_tmp800.tag=2;_tmp800.f1=Cyc_Parse_typ2tvar(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),_tmp7FD);
_tmp800.f2=Cyc_Absyn_new_vardecl(({struct _tuple1*_tmp801=_cycalloc(sizeof(*
_tmp801));_tmp801->f1=(union Cyc_Absyn_Nmspace_union)({union Cyc_Absyn_Nmspace_union
_tmp803;(_tmp803.Loc_n).tag=0;_tmp803;});_tmp801->f2=({struct _dyneither_ptr*
_tmp802=_cycalloc(sizeof(*_tmp802));_tmp802[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp802;});_tmp801;}),(
void*)({struct Cyc_Absyn_TagType_struct*_tmp804=_cycalloc(sizeof(*_tmp804));
_tmp804[0]=({struct Cyc_Absyn_TagType_struct _tmp805;_tmp805.tag=18;_tmp805.f1=(
void*)_tmp7FD;_tmp805;});_tmp804;}),0);_tmp800;});_tmp7FF;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp7FE;});
break;}case 376: _LL389: {struct Cyc_Absyn_Tvar*_tmp806=Cyc_Tcutil_new_tvar(Cyc_Tcutil_kind_to_bound((
void*)7));yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp807;(_tmp807.YY11).tag=
15;(_tmp807.YY11).f1=Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_TagInt_p_struct*
_tmp808=_cycalloc(sizeof(*_tmp808));_tmp808[0]=({struct Cyc_Absyn_TagInt_p_struct
_tmp809;_tmp809.tag=2;_tmp809.f1=_tmp806;_tmp809.f2=Cyc_Absyn_new_vardecl(({
struct _tuple1*_tmp80A=_cycalloc(sizeof(*_tmp80A));_tmp80A->f1=(union Cyc_Absyn_Nmspace_union)({
union Cyc_Absyn_Nmspace_union _tmp80C;(_tmp80C.Loc_n).tag=0;_tmp80C;});_tmp80A->f2=({
struct _dyneither_ptr*_tmp80B=_cycalloc(sizeof(*_tmp80B));_tmp80B[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp80B;});_tmp80A;}),(
void*)({struct Cyc_Absyn_TagType_struct*_tmp80D=_cycalloc(sizeof(*_tmp80D));
_tmp80D[0]=({struct Cyc_Absyn_TagType_struct _tmp80E;_tmp80E.tag=18;_tmp80E.f1=(
void*)((void*)({struct Cyc_Absyn_VarType_struct*_tmp80F=_cycalloc(sizeof(*_tmp80F));
_tmp80F[0]=({struct Cyc_Absyn_VarType_struct _tmp810;_tmp810.tag=1;_tmp810.f1=
_tmp806;_tmp810;});_tmp80F;}));_tmp80E;});_tmp80D;}),0);_tmp809;});_tmp808;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp807;});break;}case 377: _LL38A: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp811;(_tmp811.YY12).tag=16;(_tmp811.YY12).f1=({struct
_tuple13*_tmp812=_cycalloc(sizeof(*_tmp812));_tmp812->f1=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY13(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp812->f2=0;_tmp812;});
_tmp811;});break;case 378: _LL38B: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp813;(_tmp813.YY12).tag=16;(_tmp813.YY12).f1=({struct _tuple13*_tmp814=
_cycalloc(sizeof(*_tmp814));_tmp814->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_rev)(Cyc_yyget_YY13(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]));_tmp814->f2=1;_tmp814;});_tmp813;});break;case 379: _LL38C:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp815;(_tmp815.YY12).tag=
16;(_tmp815.YY12).f1=({struct _tuple13*_tmp816=_cycalloc(sizeof(*_tmp816));
_tmp816->f1=0;_tmp816->f2=1;_tmp816;});_tmp815;});break;case 380: _LL38D: yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp817;(_tmp817.YY13).tag=17;(
_tmp817.YY13).f1=({struct Cyc_List_List*_tmp818=_cycalloc(sizeof(*_tmp818));
_tmp818->hd=Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp818->tl=0;_tmp818;});_tmp817;});break;case 381: _LL38E: yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp819;(_tmp819.YY13).tag=17;(
_tmp819.YY13).f1=({struct Cyc_List_List*_tmp81A=_cycalloc(sizeof(*_tmp81A));
_tmp81A->hd=Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp81A->tl=Cyc_yyget_YY13(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]);_tmp81A;});_tmp819;});break;case 382: _LL38F: yyval=(union
Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp81B;(_tmp81B.YY14).tag=18;(_tmp81B.YY14).f1=({
struct _tuple14*_tmp81C=_cycalloc(sizeof(*_tmp81C));_tmp81C->f1=0;_tmp81C->f2=Cyc_yyget_YY11(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp81C;});_tmp81B;});
break;case 383: _LL390: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp81D;(_tmp81D.YY14).tag=18;(_tmp81D.YY14).f1=({struct _tuple14*_tmp81E=
_cycalloc(sizeof(*_tmp81E));_tmp81E->f1=Cyc_yyget_YY41(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp81E->f2=Cyc_yyget_YY11(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp81E;});_tmp81D;});
break;case 384: _LL391: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp81F;(_tmp81F.YY16).tag=20;(_tmp81F.YY16).f1=({struct _tuple13*_tmp820=
_cycalloc(sizeof(*_tmp820));_tmp820->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_rev)(Cyc_yyget_YY15(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]));_tmp820->f2=0;_tmp820;});_tmp81F;});break;case 385: _LL392: yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp821;(_tmp821.YY16).tag=20;(
_tmp821.YY16).f1=({struct _tuple13*_tmp822=_cycalloc(sizeof(*_tmp822));_tmp822->f1=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY15(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]));_tmp822->f2=1;_tmp822;});
_tmp821;});break;case 386: _LL393: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp823;(_tmp823.YY16).tag=20;(_tmp823.YY16).f1=({struct _tuple13*_tmp824=
_cycalloc(sizeof(*_tmp824));_tmp824->f1=0;_tmp824->f2=1;_tmp824;});_tmp823;});
break;case 387: _LL394: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp825;(_tmp825.YY15).tag=19;(_tmp825.YY15).f1=({struct Cyc_List_List*_tmp826=
_cycalloc(sizeof(*_tmp826));_tmp826->hd=Cyc_yyget_YY14(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp826->tl=0;_tmp826;});
_tmp825;});break;case 388: _LL395: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp827;(_tmp827.YY15).tag=19;(_tmp827.YY15).f1=({struct Cyc_List_List*_tmp828=
_cycalloc(sizeof(*_tmp828));_tmp828->hd=Cyc_yyget_YY14(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp828->tl=Cyc_yyget_YY15(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp828;});_tmp827;});
break;case 389: _LL396: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 390: _LL397: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp829;(_tmp829.YY3).tag=7;(_tmp829.YY3).f1=Cyc_Absyn_seq_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp829;});
break;case 391: _LL398: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 392: _LL399: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp82A;(_tmp82A.YY3).tag=7;(_tmp82A.YY3).f1=Cyc_Absyn_assignop_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY8(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp82A;});
break;case 393: _LL39A: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp82B;(_tmp82B.YY8).tag=12;(_tmp82B.YY8).f1=0;_tmp82B;});break;case 394: _LL39B:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp82C;(_tmp82C.YY8).tag=
12;(_tmp82C.YY8).f1=({struct Cyc_Core_Opt*_tmp82D=_cycalloc(sizeof(*_tmp82D));
_tmp82D->v=(void*)((void*)1);_tmp82D;});_tmp82C;});break;case 395: _LL39C: yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp82E;(_tmp82E.YY8).tag=12;(
_tmp82E.YY8).f1=({struct Cyc_Core_Opt*_tmp82F=_cycalloc(sizeof(*_tmp82F));_tmp82F->v=(
void*)((void*)3);_tmp82F;});_tmp82E;});break;case 396: _LL39D: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp830;(_tmp830.YY8).tag=12;(_tmp830.YY8).f1=({struct Cyc_Core_Opt*
_tmp831=_cycalloc(sizeof(*_tmp831));_tmp831->v=(void*)((void*)4);_tmp831;});
_tmp830;});break;case 397: _LL39E: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp832;(_tmp832.YY8).tag=12;(_tmp832.YY8).f1=({struct Cyc_Core_Opt*_tmp833=
_cycalloc(sizeof(*_tmp833));_tmp833->v=(void*)((void*)0);_tmp833;});_tmp832;});
break;case 398: _LL39F: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp834;(_tmp834.YY8).tag=12;(_tmp834.YY8).f1=({struct Cyc_Core_Opt*_tmp835=
_cycalloc(sizeof(*_tmp835));_tmp835->v=(void*)((void*)2);_tmp835;});_tmp834;});
break;case 399: _LL3A0: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp836;(_tmp836.YY8).tag=12;(_tmp836.YY8).f1=({struct Cyc_Core_Opt*_tmp837=
_cycalloc(sizeof(*_tmp837));_tmp837->v=(void*)((void*)16);_tmp837;});_tmp836;});
break;case 400: _LL3A1: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp838;(_tmp838.YY8).tag=12;(_tmp838.YY8).f1=({struct Cyc_Core_Opt*_tmp839=
_cycalloc(sizeof(*_tmp839));_tmp839->v=(void*)((void*)17);_tmp839;});_tmp838;});
break;case 401: _LL3A2: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp83A;(_tmp83A.YY8).tag=12;(_tmp83A.YY8).f1=({struct Cyc_Core_Opt*_tmp83B=
_cycalloc(sizeof(*_tmp83B));_tmp83B->v=(void*)((void*)13);_tmp83B;});_tmp83A;});
break;case 402: _LL3A3: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp83C;(_tmp83C.YY8).tag=12;(_tmp83C.YY8).f1=({struct Cyc_Core_Opt*_tmp83D=
_cycalloc(sizeof(*_tmp83D));_tmp83D->v=(void*)((void*)15);_tmp83D;});_tmp83C;});
break;case 403: _LL3A4: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp83E;(_tmp83E.YY8).tag=12;(_tmp83E.YY8).f1=({struct Cyc_Core_Opt*_tmp83F=
_cycalloc(sizeof(*_tmp83F));_tmp83F->v=(void*)((void*)14);_tmp83F;});_tmp83E;});
break;case 404: _LL3A5: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 405: _LL3A6: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp840;(_tmp840.YY3).tag=7;(_tmp840.YY3).f1=Cyc_Absyn_conditional_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp840;});
break;case 406: _LL3A7: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp841;(_tmp841.YY3).tag=7;(_tmp841.YY3).f1=Cyc_Absyn_throw_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp841;});
break;case 407: _LL3A8: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp842;(_tmp842.YY3).tag=7;(_tmp842.YY3).f1=Cyc_Absyn_New_exp(0,Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp842;});
break;case 408: _LL3A9: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp843;(_tmp843.YY3).tag=7;(_tmp843.YY3).f1=Cyc_Absyn_New_exp(0,Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp843;});
break;case 409: _LL3AA: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp844;(_tmp844.YY3).tag=7;(_tmp844.YY3).f1=Cyc_Absyn_New_exp((struct Cyc_Absyn_Exp*)
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp844;});
break;case 410: _LL3AB: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp845;(_tmp845.YY3).tag=7;(_tmp845.YY3).f1=Cyc_Absyn_New_exp((struct Cyc_Absyn_Exp*)
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp845;});
break;case 411: _LL3AC: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 412: _LL3AD: yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)];break;case 413: _LL3AE: yyval=(union Cyc_YYSTYPE_union)({union
Cyc_YYSTYPE_union _tmp846;(_tmp846.YY3).tag=7;(_tmp846.YY3).f1=Cyc_Absyn_or_exp(
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp846;});
break;case 414: _LL3AF: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 415: _LL3B0: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp847;(_tmp847.YY3).tag=7;(_tmp847.YY3).f1=Cyc_Absyn_and_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp847;});
break;case 416: _LL3B1: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 417: _LL3B2: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp848;(_tmp848.YY3).tag=7;(_tmp848.YY3).f1=Cyc_Absyn_prim2_exp((void*)14,Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp848;});
break;case 418: _LL3B3: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 419: _LL3B4: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp849;(_tmp849.YY3).tag=7;(_tmp849.YY3).f1=Cyc_Absyn_prim2_exp((void*)15,Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp849;});
break;case 420: _LL3B5: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 421: _LL3B6: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp84A;(_tmp84A.YY3).tag=7;(_tmp84A.YY3).f1=Cyc_Absyn_prim2_exp((void*)13,Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp84A;});
break;case 422: _LL3B7: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 423: _LL3B8: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp84B;(_tmp84B.YY3).tag=7;(_tmp84B.YY3).f1=Cyc_Absyn_eq_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp84B;});
break;case 424: _LL3B9: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp84C;(_tmp84C.YY3).tag=7;(_tmp84C.YY3).f1=Cyc_Absyn_neq_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp84C;});
break;case 425: _LL3BA: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 426: _LL3BB: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp84D;(_tmp84D.YY3).tag=7;(_tmp84D.YY3).f1=Cyc_Absyn_lt_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp84D;});
break;case 427: _LL3BC: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp84E;(_tmp84E.YY3).tag=7;(_tmp84E.YY3).f1=Cyc_Absyn_gt_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp84E;});
break;case 428: _LL3BD: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp84F;(_tmp84F.YY3).tag=7;(_tmp84F.YY3).f1=Cyc_Absyn_lte_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp84F;});
break;case 429: _LL3BE: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp850;(_tmp850.YY3).tag=7;(_tmp850.YY3).f1=Cyc_Absyn_gte_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp850;});
break;case 430: _LL3BF: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 431: _LL3C0: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp851;(_tmp851.YY3).tag=7;(_tmp851.YY3).f1=Cyc_Absyn_prim2_exp((void*)16,Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp851;});
break;case 432: _LL3C1: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp852;(_tmp852.YY3).tag=7;(_tmp852.YY3).f1=Cyc_Absyn_prim2_exp((void*)17,Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp852;});
break;case 433: _LL3C2: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 434: _LL3C3: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp853;(_tmp853.YY3).tag=7;(_tmp853.YY3).f1=Cyc_Absyn_prim2_exp((void*)0,Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp853;});
break;case 435: _LL3C4: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp854;(_tmp854.YY3).tag=7;(_tmp854.YY3).f1=Cyc_Absyn_prim2_exp((void*)2,Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp854;});
break;case 436: _LL3C5: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 437: _LL3C6: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp855;(_tmp855.YY3).tag=7;(_tmp855.YY3).f1=Cyc_Absyn_prim2_exp((void*)1,Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp855;});
break;case 438: _LL3C7: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp856;(_tmp856.YY3).tag=7;(_tmp856.YY3).f1=Cyc_Absyn_prim2_exp((void*)3,Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp856;});
break;case 439: _LL3C8: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp857;(_tmp857.YY3).tag=7;(_tmp857.YY3).f1=Cyc_Absyn_prim2_exp((void*)4,Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp857;});
break;case 440: _LL3C9: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 441: _LL3CA: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp858;(_tmp858.YY3).tag=7;(_tmp858.YY3).f1=Cyc_Absyn_cast_exp((*Cyc_yyget_YY37(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])).f3,Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),1,(void*)0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp858;});
break;case 442: _LL3CB: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 443: _LL3CC: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp859;(_tmp859.YY3).tag=7;(_tmp859.YY3).f1=Cyc_Absyn_pre_inc_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp859;});
break;case 444: _LL3CD: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp85A;(_tmp85A.YY3).tag=7;(_tmp85A.YY3).f1=Cyc_Absyn_pre_dec_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp85A;});
break;case 445: _LL3CE: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp85B;(_tmp85B.YY3).tag=7;(_tmp85B.YY3).f1=Cyc_Absyn_address_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp85B;});
break;case 446: _LL3CF: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp85C;(_tmp85C.YY3).tag=7;(_tmp85C.YY3).f1=Cyc_Absyn_deref_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp85C;});
break;case 447: _LL3D0: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 448: _LL3D1: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp85D;(_tmp85D.YY3).tag=7;(_tmp85D.YY3).f1=Cyc_Absyn_prim1_exp(Cyc_yyget_YY7(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp85D;});
break;case 449: _LL3D2: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp85E;(_tmp85E.YY3).tag=7;(_tmp85E.YY3).f1=Cyc_Absyn_sizeoftyp_exp((*Cyc_yyget_YY37(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])).f3,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp85E;});
break;case 450: _LL3D3: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp85F;(_tmp85F.YY3).tag=7;(_tmp85F.YY3).f1=Cyc_Absyn_sizeofexp_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp85F;});
break;case 451: _LL3D4: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp860;(_tmp860.YY3).tag=7;(_tmp860.YY3).f1=Cyc_Absyn_offsetof_exp((*Cyc_yyget_YY37(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)])).f3,(void*)({struct
Cyc_Absyn_StructField_struct*_tmp861=_cycalloc(sizeof(*_tmp861));_tmp861[0]=({
struct Cyc_Absyn_StructField_struct _tmp862;_tmp862.tag=0;_tmp862.f1=({struct
_dyneither_ptr*_tmp863=_cycalloc(sizeof(*_tmp863));_tmp863[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp863;});_tmp862;});
_tmp861;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp860;});break;case 452: _LL3D5: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp864;(_tmp864.YY3).tag=7;(_tmp864.YY3).f1=Cyc_Absyn_offsetof_exp((*
Cyc_yyget_YY37(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)])).f3,(
void*)({struct Cyc_Absyn_TupleIndex_struct*_tmp865=_cycalloc_atomic(sizeof(*
_tmp865));_tmp865[0]=({struct Cyc_Absyn_TupleIndex_struct _tmp866;_tmp866.tag=1;
_tmp866.f1=(unsigned int)(Cyc_yyget_Int_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)])).f2;_tmp866;});_tmp865;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp864;});
break;case 453: _LL3D6: {struct Cyc_Position_Segment*_tmp867=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);struct Cyc_List_List*
_tmp868=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
_tmp867,Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]));
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp869;(_tmp869.YY3).tag=
7;(_tmp869.YY3).f1=Cyc_Absyn_gentyp_exp(_tmp868,(*Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])).f3,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp869;});
break;}case 454: _LL3D7: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp86A;(_tmp86A.YY3).tag=7;(_tmp86A.YY3).f1=Cyc_Absyn_new_exp((void*)({struct
Cyc_Absyn_Malloc_e_struct*_tmp86B=_cycalloc(sizeof(*_tmp86B));_tmp86B[0]=({
struct Cyc_Absyn_Malloc_e_struct _tmp86C;_tmp86C.tag=35;_tmp86C.f1=({struct Cyc_Absyn_MallocInfo
_tmp86D;_tmp86D.is_calloc=0;_tmp86D.rgn=0;_tmp86D.elt_type=0;_tmp86D.num_elts=
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp86D.fat_result=0;_tmp86D;});_tmp86C;});_tmp86B;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp86A;});
break;case 455: _LL3D8: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp86E;(_tmp86E.YY3).tag=7;(_tmp86E.YY3).f1=Cyc_Absyn_new_exp((void*)({struct
Cyc_Absyn_Malloc_e_struct*_tmp86F=_cycalloc(sizeof(*_tmp86F));_tmp86F[0]=({
struct Cyc_Absyn_Malloc_e_struct _tmp870;_tmp870.tag=35;_tmp870.f1=({struct Cyc_Absyn_MallocInfo
_tmp871;_tmp871.is_calloc=0;_tmp871.rgn=(struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp871.elt_type=0;
_tmp871.num_elts=Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp871.fat_result=0;_tmp871;});_tmp870;});_tmp86F;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp86E;});
break;case 456: _LL3D9: {void*_tmp873;struct _tuple2 _tmp872=*Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp873=_tmp872.f3;yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp874;(_tmp874.YY3).tag=7;(
_tmp874.YY3).f1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Malloc_e_struct*
_tmp875=_cycalloc(sizeof(*_tmp875));_tmp875[0]=({struct Cyc_Absyn_Malloc_e_struct
_tmp876;_tmp876.tag=35;_tmp876.f1=({struct Cyc_Absyn_MallocInfo _tmp877;_tmp877.is_calloc=
1;_tmp877.rgn=0;_tmp877.elt_type=({void**_tmp878=_cycalloc(sizeof(*_tmp878));
_tmp878[0]=_tmp873;_tmp878;});_tmp877.num_elts=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 6)]);_tmp877.fat_result=0;
_tmp877;});_tmp876;});_tmp875;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 8)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp874;});
break;}case 457: _LL3DA: {void*_tmp87A;struct _tuple2 _tmp879=*Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp87A=_tmp879.f3;yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp87B;(_tmp87B.YY3).tag=7;(
_tmp87B.YY3).f1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Malloc_e_struct*
_tmp87C=_cycalloc(sizeof(*_tmp87C));_tmp87C[0]=({struct Cyc_Absyn_Malloc_e_struct
_tmp87D;_tmp87D.tag=35;_tmp87D.f1=({struct Cyc_Absyn_MallocInfo _tmp87E;_tmp87E.is_calloc=
1;_tmp87E.rgn=(struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 8)]);_tmp87E.elt_type=({void**
_tmp87F=_cycalloc(sizeof(*_tmp87F));_tmp87F[0]=_tmp87A;_tmp87F;});_tmp87E.num_elts=
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]);
_tmp87E.fat_result=0;_tmp87E;});_tmp87D;});_tmp87C;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 10)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp87B;});
break;}case 458: _LL3DB: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp880;(_tmp880.YY3).tag=7;(_tmp880.YY3).f1=Cyc_Absyn_new_exp((void*)({struct
Cyc_Absyn_Swap_e_struct*_tmp881=_cycalloc(sizeof(*_tmp881));_tmp881[0]=({struct
Cyc_Absyn_Swap_e_struct _tmp882;_tmp882.tag=36;_tmp882.f1=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp882.f2=Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp882;});_tmp881;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp880;});break;case 459: _LL3DC: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp883;(_tmp883.YY3).tag=7;(_tmp883.YY3).f1=Cyc_Absyn_primop_exp((
void*)19,({struct Cyc_Absyn_Exp*_tmp884[1];_tmp884[0]=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp884,sizeof(struct Cyc_Absyn_Exp*),
1));}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp883;});break;case 460: _LL3DD: {void*_tmp886;
struct _tuple2 _tmp885=*Cyc_yyget_YY37(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp886=_tmp885.f3;yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp887;(_tmp887.YY3).tag=7;(_tmp887.YY3).f1=Cyc_Absyn_valueof_exp(_tmp886,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp887;});
break;}case 461: _LL3DE: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp888;(_tmp888.YY7).tag=11;(_tmp888.YY7).f1=(void*)((void*)12);_tmp888;});
break;case 462: _LL3DF: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp889;(_tmp889.YY7).tag=11;(_tmp889.YY7).f1=(void*)((void*)11);_tmp889;});
break;case 463: _LL3E0: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp88A;(_tmp88A.YY7).tag=11;(_tmp88A.YY7).f1=(void*)((void*)2);_tmp88A;});
break;case 464: _LL3E1: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 465: _LL3E2: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp88B;(_tmp88B.YY3).tag=7;(_tmp88B.YY3).f1=Cyc_Absyn_subscript_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp88B;});
break;case 466: _LL3E3: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp88C;(_tmp88C.YY3).tag=7;(_tmp88C.YY3).f1=Cyc_Absyn_unknowncall_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp88C;});
break;case 467: _LL3E4: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp88D;(_tmp88D.YY3).tag=7;(_tmp88D.YY3).f1=Cyc_Absyn_unknowncall_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_yyget_YY5(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp88D;});
break;case 468: _LL3E5: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp88E;(_tmp88E.YY3).tag=7;(_tmp88E.YY3).f1=Cyc_Absyn_aggrmember_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),({struct
_dyneither_ptr*_tmp88F=_cycalloc(sizeof(*_tmp88F));_tmp88F[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp88F;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp88E;});
break;case 469: _LL3E6: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp890;(_tmp890.YY3).tag=7;(_tmp890.YY3).f1=Cyc_Absyn_aggrarrow_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),({struct
_dyneither_ptr*_tmp891=_cycalloc(sizeof(*_tmp891));_tmp891[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp891;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp890;});
break;case 470: _LL3E7: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp892;(_tmp892.YY3).tag=7;(_tmp892.YY3).f1=Cyc_Absyn_post_inc_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp892;});
break;case 471: _LL3E8: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp893;(_tmp893.YY3).tag=7;(_tmp893.YY3).f1=Cyc_Absyn_post_dec_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp893;});
break;case 472: _LL3E9: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp894;(_tmp894.YY3).tag=7;(_tmp894.YY3).f1=Cyc_Absyn_new_exp((void*)({struct
Cyc_Absyn_CompoundLit_e_struct*_tmp895=_cycalloc(sizeof(*_tmp895));_tmp895[0]=({
struct Cyc_Absyn_CompoundLit_e_struct _tmp896;_tmp896.tag=27;_tmp896.f1=Cyc_yyget_YY37(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp896.f2=((struct
Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY6(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp896;});_tmp895;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp894;});break;case 473: _LL3EA: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp897;(_tmp897.YY3).tag=7;(_tmp897.YY3).f1=Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_CompoundLit_e_struct*_tmp898=_cycalloc(sizeof(*_tmp898));
_tmp898[0]=({struct Cyc_Absyn_CompoundLit_e_struct _tmp899;_tmp899.tag=27;_tmp899.f1=
Cyc_yyget_YY37(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]);
_tmp899.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_YY6(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]));
_tmp899;});_tmp898;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp897;});
break;case 474: _LL3EB: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp89A;(_tmp89A.YY3).tag=7;(_tmp89A.YY3).f1=Cyc_Absyn_new_exp((void*)({struct
Cyc_Absyn_UnknownId_e_struct*_tmp89B=_cycalloc(sizeof(*_tmp89B));_tmp89B[0]=({
struct Cyc_Absyn_UnknownId_e_struct _tmp89C;_tmp89C.tag=2;_tmp89C.f1=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp89C;});_tmp89B;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp89A;});break;case 475: _LL3EC: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 476: _LL3ED: yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp89D;(_tmp89D.YY3).tag=7;(
_tmp89D.YY3).f1=Cyc_Absyn_string_exp(Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp89D;});
break;case 477: _LL3EE: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)];break;case 478: _LL3EF: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp89E;(_tmp89E.YY3).tag=7;(_tmp89E.YY3).f1=Cyc_Absyn_noinstantiate_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp89E;});
break;case 479: _LL3F0: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp89F;(_tmp89F.YY3).tag=7;(_tmp89F.YY3).f1=Cyc_Absyn_instantiate_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_List_imp_rev(
Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp89F;});
break;case 480: _LL3F1: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp8A0;(_tmp8A0.YY3).tag=7;(_tmp8A0.YY3).f1=Cyc_Absyn_tuple_exp(Cyc_yyget_YY5(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8A0;});
break;case 481: _LL3F2: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp8A1;(_tmp8A1.YY3).tag=7;(_tmp8A1.YY3).f1=Cyc_Absyn_new_exp((void*)({struct
Cyc_Absyn_Struct_e_struct*_tmp8A2=_cycalloc(sizeof(*_tmp8A2));_tmp8A2[0]=({
struct Cyc_Absyn_Struct_e_struct _tmp8A3;_tmp8A3.tag=30;_tmp8A3.f1=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp8A3.f2=Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp8A3.f3=((struct
Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY6(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp8A3.f4=0;_tmp8A3;});
_tmp8A2;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp8A1;});break;case 482: _LL3F3: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp8A4;(_tmp8A4.YY3).tag=7;(_tmp8A4.YY3).f1=Cyc_Absyn_stmt_exp(
Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8A4;});
break;case 483: _LL3F4: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp8A5;(_tmp8A5.YY5).tag=9;(_tmp8A5.YY5).f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(Cyc_yyget_YY5(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]));_tmp8A5;});break;case 484: _LL3F5: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp8A6;(_tmp8A6.YY5).tag=9;(_tmp8A6.YY5).f1=({struct Cyc_List_List*
_tmp8A7=_cycalloc(sizeof(*_tmp8A7));_tmp8A7->hd=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp8A7->tl=0;_tmp8A7;});
_tmp8A6;});break;case 485: _LL3F6: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp8A8;(_tmp8A8.YY5).tag=9;(_tmp8A8.YY5).f1=({struct Cyc_List_List*_tmp8A9=
_cycalloc(sizeof(*_tmp8A9));_tmp8A9->hd=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp8A9->tl=Cyc_yyget_YY5(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp8A9;});_tmp8A8;});
break;case 486: _LL3F7: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp8AA;(_tmp8AA.YY3).tag=7;(_tmp8AA.YY3).f1=Cyc_Absyn_int_exp((Cyc_yyget_Int_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f1,(Cyc_yyget_Int_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f2,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8AA;});
break;case 487: _LL3F8: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp8AB;(_tmp8AB.YY3).tag=7;(_tmp8AB.YY3).f1=Cyc_Absyn_char_exp(Cyc_yyget_Char_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8AB;});
break;case 488: _LL3F9: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp8AC;(_tmp8AC.YY3).tag=7;(_tmp8AC.YY3).f1=Cyc_Absyn_float_exp(Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8AC;});
break;case 489: _LL3FA: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp8AD;(_tmp8AD.YY3).tag=7;(_tmp8AD.YY3).f1=Cyc_Absyn_null_exp(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8AD;});
break;case 490: _LL3FB: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp8AE;(_tmp8AE.QualId_tok).tag=4;(_tmp8AE.QualId_tok).f1=({struct _tuple1*
_tmp8AF=_cycalloc(sizeof(*_tmp8AF));_tmp8AF->f1=(union Cyc_Absyn_Nmspace_union)({
union Cyc_Absyn_Nmspace_union _tmp8B1;(_tmp8B1.Rel_n).tag=1;(_tmp8B1.Rel_n).f1=0;
_tmp8B1;});_tmp8AF->f2=({struct _dyneither_ptr*_tmp8B0=_cycalloc(sizeof(*_tmp8B0));
_tmp8B0[0]=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp8B0;});_tmp8AF;});_tmp8AE;});break;case 491: _LL3FC: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 492: _LL3FD: yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp8B2;(_tmp8B2.QualId_tok).tag=
4;(_tmp8B2.QualId_tok).f1=({struct _tuple1*_tmp8B3=_cycalloc(sizeof(*_tmp8B3));
_tmp8B3->f1=(union Cyc_Absyn_Nmspace_union)({union Cyc_Absyn_Nmspace_union _tmp8B5;(
_tmp8B5.Rel_n).tag=1;(_tmp8B5.Rel_n).f1=0;_tmp8B5;});_tmp8B3->f2=({struct
_dyneither_ptr*_tmp8B4=_cycalloc(sizeof(*_tmp8B4));_tmp8B4[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp8B4;});_tmp8B3;});
_tmp8B2;});break;case 493: _LL3FE: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 494: _LL3FF: yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)];break;case 495: _LL400: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 496: _LL401: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 497: _LL402:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 498:
_LL403: break;case 499: _LL404: yylex_buf->lex_curr_pos -=1;break;default: _LL405:
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
yyn=(int)Cyc_yyr1[_check_known_subscript_notnull(500,yyn)];yystate=Cyc_yypgoto[
_check_known_subscript_notnull(142,yyn - 134)]+ yyss[
_check_known_subscript_notnull(10000,yyssp_offset)];if((yystate >= 0  && yystate <= 
6136) && Cyc_yycheck[_check_known_subscript_notnull(6137,yystate)]== yyss[
_check_known_subscript_notnull(10000,yyssp_offset)])yystate=(int)Cyc_yytable[
_check_known_subscript_notnull(6137,yystate)];else{yystate=(int)Cyc_yydefgoto[
_check_known_subscript_notnull(142,yyn - 134)];}goto yynewstate;yyerrlab: if(
yyerrstatus == 0){++ Cyc_yynerrs;yyn=(int)Cyc_yypact[
_check_known_subscript_notnull(1010,yystate)];if(yyn > - 32768  && yyn < 6136){int
sze=0;struct _dyneither_ptr msg;int x;int count;count=0;for(x=yyn < 0?- yyn: 0;x < 276 / 
sizeof(char*);++ x){if(Cyc_yycheck[_check_known_subscript_notnull(6137,x + yyn)]== 
x)(sze +=Cyc_strlen((struct _dyneither_ptr)Cyc_yytname[
_check_known_subscript_notnull(276,x)])+ 15,count ++);}msg=({unsigned int _tmp8B6=(
unsigned int)(sze + 15);char*_tmp8B7=(char*)_region_malloc(yyregion,_check_times(
sizeof(char),_tmp8B6 + 1));struct _dyneither_ptr _tmp8B9=_tag_dyneither(_tmp8B7,
sizeof(char),_tmp8B6 + 1);{unsigned int _tmp8B8=_tmp8B6;unsigned int i;for(i=0;i < 
_tmp8B8;i ++){_tmp8B7[i]='\000';}_tmp8B7[_tmp8B8]=(char)0;}_tmp8B9;});Cyc_strcpy(
msg,({const char*_tmp8BA="parse error";_tag_dyneither(_tmp8BA,sizeof(char),12);}));
if(count < 5){count=0;for(x=yyn < 0?- yyn: 0;x < 276 / sizeof(char*);++ x){if(Cyc_yycheck[
_check_known_subscript_notnull(6137,x + yyn)]== x){Cyc_strcat(msg,(struct
_dyneither_ptr)(count == 0?(struct _dyneither_ptr)({const char*_tmp8BB=", expecting `";
_tag_dyneither(_tmp8BB,sizeof(char),14);}):(struct _dyneither_ptr)({const char*
_tmp8BC=" or `";_tag_dyneither(_tmp8BC,sizeof(char),6);})));Cyc_strcat(msg,(
struct _dyneither_ptr)Cyc_yytname[_check_known_subscript_notnull(276,x)]);Cyc_strcat(
msg,({const char*_tmp8BD="'";_tag_dyneither(_tmp8BD,sizeof(char),2);}));++ count;}}}
Cyc_yyerror((struct _dyneither_ptr)msg);}else{Cyc_yyerror(({const char*_tmp8BE="parse error";
_tag_dyneither(_tmp8BE,sizeof(char),12);}));}}goto yyerrlab1;yyerrlab1: if(
yyerrstatus == 3){if(Cyc_yychar == 0){int _tmp8BF=1;_npop_handler(0);return _tmp8BF;}
Cyc_yychar=- 2;}yyerrstatus=3;goto yyerrhandle;yyerrdefault: yyerrpop: if(
yyssp_offset == 0){int _tmp8C0=1;_npop_handler(0);return _tmp8C0;}-- yyvsp_offset;
yystate=(int)yyss[_check_known_subscript_notnull(10000,-- yyssp_offset)];--
yylsp_offset;yyerrhandle: yyn=(int)Cyc_yypact[_check_known_subscript_notnull(1010,
yystate)];if(yyn == - 32768)goto yyerrdefault;yyn +=1;if((yyn < 0  || yyn > 6136) || 
Cyc_yycheck[_check_known_subscript_notnull(6137,yyn)]!= 1)goto yyerrdefault;yyn=(
int)Cyc_yytable[_check_known_subscript_notnull(6137,yyn)];if(yyn < 0){if(yyn == -
32768)goto yyerrpop;yyn=- yyn;goto yyreduce;}else{if(yyn == 0)goto yyerrpop;}if(yyn == 
1009){int _tmp8C1=0;_npop_handler(0);return _tmp8C1;}yyvs[
_check_known_subscript_notnull(10000,++ yyvsp_offset)]=Cyc_yylval;yyls[
_check_known_subscript_notnull(10000,++ yylsp_offset)]=Cyc_yylloc;goto yynewstate;};
_pop_region(yyregion);}void Cyc_yyprint(int i,union Cyc_YYSTYPE_union v){union Cyc_YYSTYPE_union
_tmp8CB=v;struct _tuple11 _tmp8CC;int _tmp8CD;char _tmp8CE;struct _dyneither_ptr
_tmp8CF;struct _tuple1*_tmp8D0;struct _tuple1 _tmp8D1;union Cyc_Absyn_Nmspace_union
_tmp8D2;struct _dyneither_ptr*_tmp8D3;_LL408: if((_tmp8CB.Int_tok).tag != 0)goto
_LL40A;_tmp8CC=(_tmp8CB.Int_tok).f1;_tmp8CD=_tmp8CC.f2;_LL409:({struct Cyc_Int_pa_struct
_tmp8D6;_tmp8D6.tag=1;_tmp8D6.f1=(unsigned long)_tmp8CD;{void*_tmp8D4[1]={&
_tmp8D6};Cyc_fprintf(Cyc_stderr,({const char*_tmp8D5="%d";_tag_dyneither(_tmp8D5,
sizeof(char),3);}),_tag_dyneither(_tmp8D4,sizeof(void*),1));}});goto _LL407;
_LL40A: if((_tmp8CB.Char_tok).tag != 1)goto _LL40C;_tmp8CE=(_tmp8CB.Char_tok).f1;
_LL40B:({struct Cyc_Int_pa_struct _tmp8D9;_tmp8D9.tag=1;_tmp8D9.f1=(unsigned long)((
int)_tmp8CE);{void*_tmp8D7[1]={& _tmp8D9};Cyc_fprintf(Cyc_stderr,({const char*
_tmp8D8="%c";_tag_dyneither(_tmp8D8,sizeof(char),3);}),_tag_dyneither(_tmp8D7,
sizeof(void*),1));}});goto _LL407;_LL40C: if((_tmp8CB.String_tok).tag != 2)goto
_LL40E;_tmp8CF=(_tmp8CB.String_tok).f1;_LL40D:({struct Cyc_String_pa_struct
_tmp8DC;_tmp8DC.tag=0;_tmp8DC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
_tmp8CF);{void*_tmp8DA[1]={& _tmp8DC};Cyc_fprintf(Cyc_stderr,({const char*_tmp8DB="\"%s\"";
_tag_dyneither(_tmp8DB,sizeof(char),5);}),_tag_dyneither(_tmp8DA,sizeof(void*),1));}});
goto _LL407;_LL40E: if((_tmp8CB.QualId_tok).tag != 4)goto _LL410;_tmp8D0=(_tmp8CB.QualId_tok).f1;
_tmp8D1=*_tmp8D0;_tmp8D2=_tmp8D1.f1;_tmp8D3=_tmp8D1.f2;_LL40F: {struct Cyc_List_List*
_tmp8DD=0;{union Cyc_Absyn_Nmspace_union _tmp8DE=_tmp8D2;struct Cyc_List_List*
_tmp8DF;struct Cyc_List_List*_tmp8E0;_LL413: if((_tmp8DE.Rel_n).tag != 1)goto _LL415;
_tmp8DF=(_tmp8DE.Rel_n).f1;_LL414: _tmp8DD=_tmp8DF;goto _LL412;_LL415: if((_tmp8DE.Abs_n).tag
!= 2)goto _LL417;_tmp8E0=(_tmp8DE.Abs_n).f1;_LL416: _tmp8DD=_tmp8E0;goto _LL412;
_LL417: if((_tmp8DE.Loc_n).tag != 0)goto _LL412;_LL418: goto _LL412;_LL412:;}for(0;
_tmp8DD != 0;_tmp8DD=_tmp8DD->tl){({struct Cyc_String_pa_struct _tmp8E3;_tmp8E3.tag=
0;_tmp8E3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct
_dyneither_ptr*)_tmp8DD->hd));{void*_tmp8E1[1]={& _tmp8E3};Cyc_fprintf(Cyc_stderr,({
const char*_tmp8E2="%s::";_tag_dyneither(_tmp8E2,sizeof(char),5);}),
_tag_dyneither(_tmp8E1,sizeof(void*),1));}});}({struct Cyc_String_pa_struct
_tmp8E6;_tmp8E6.tag=0;_tmp8E6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*
_tmp8D3);{void*_tmp8E4[1]={& _tmp8E6};Cyc_fprintf(Cyc_stderr,({const char*_tmp8E5="%s::";
_tag_dyneither(_tmp8E5,sizeof(char),5);}),_tag_dyneither(_tmp8E4,sizeof(void*),1));}});
goto _LL407;}_LL410:;_LL411:({void*_tmp8E7=0;Cyc_fprintf(Cyc_stderr,({const char*
_tmp8E8="?";_tag_dyneither(_tmp8E8,sizeof(char),2);}),_tag_dyneither(_tmp8E7,
sizeof(void*),0));});goto _LL407;_LL407:;}struct Cyc_List_List*Cyc_Parse_parse_file(
struct Cyc___cycFILE*f){Cyc_Parse_parse_result=0;Cyc_yyparse(Cyc_Lexing_from_file(
f));return Cyc_Parse_parse_result;}
