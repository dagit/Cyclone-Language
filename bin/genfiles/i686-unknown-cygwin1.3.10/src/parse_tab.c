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
exists,struct Cyc_List_List*po,struct Cyc_List_List*fs,int tagged);struct Cyc_Absyn_Decl*
Cyc_Absyn_aggr_decl(void*k,void*s,struct _tuple1*n,struct Cyc_List_List*ts,struct
Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*atts,struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Decl*Cyc_Absyn_tunion_decl(void*s,struct _tuple1*n,struct Cyc_List_List*
ts,struct Cyc_Core_Opt*fs,int is_xtunion,int is_flat,struct Cyc_Position_Segment*loc);
void*Cyc_Absyn_function_typ(struct Cyc_List_List*tvs,struct Cyc_Core_Opt*eff_typ,
void*ret_typ,struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*
cyc_varargs,struct Cyc_List_List*rgn_po,struct Cyc_List_List*atts);int Cyc_Absyn_fntype_att(
void*a);extern int Cyc_Absyn_porting_c_code;struct Cyc_Iter_Iter{void*env;int(*next)(
void*env,void*dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;
extern char Cyc_Set_Absent[11];struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,
void*);struct _RegionHandle*r;struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[12];
extern char Cyc_Dict_Absent[11];struct _tuple4{void*f1;void*f2;};struct _tuple4*Cyc_Dict_rchoose(
struct _RegionHandle*r,struct Cyc_Dict_Dict d);struct _tuple4*Cyc_Dict_rchoose(
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
struct Cyc_Yyltype Cyc_yylloc={0,0,0,0,0};static short Cyc_yytranslate[366]={0,2,2,2,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,137,2,2,121,135,132,2,
118,119,126,129,114,133,123,134,2,2,2,2,2,2,2,2,2,2,122,111,116,115,117,128,127,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,124,2,125,131,120,2,2,2,2,2,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,112,130,113,136,2,2,2,2,2,2,2,2,2,2,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1,
2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,
31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,
58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,
85,86,87,88,89,90,91,92,93,94,95,96,97,98,99,100,101,102,103,104,105,106,107,108,
109,110};static char _tmp26B[2]="$";static char _tmp26C[6]="error";static char _tmp26D[
12]="$undefined.";static char _tmp26E[5]="AUTO";static char _tmp26F[9]="REGISTER";
static char _tmp270[7]="STATIC";static char _tmp271[7]="EXTERN";static char _tmp272[8]="TYPEDEF";
static char _tmp273[5]="VOID";static char _tmp274[5]="CHAR";static char _tmp275[6]="SHORT";
static char _tmp276[4]="INT";static char _tmp277[5]="LONG";static char _tmp278[6]="FLOAT";
static char _tmp279[7]="DOUBLE";static char _tmp27A[7]="SIGNED";static char _tmp27B[9]="UNSIGNED";
static char _tmp27C[6]="CONST";static char _tmp27D[9]="VOLATILE";static char _tmp27E[9]="RESTRICT";
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
9]="DATATYPE";static char _tmp29D[8]="XTUNION";static char _tmp29E[7]="TUNION";
static char _tmp29F[7]="MALLOC";static char _tmp2A0[8]="RMALLOC";static char _tmp2A1[7]="CALLOC";
static char _tmp2A2[8]="RCALLOC";static char _tmp2A3[5]="SWAP";static char _tmp2A4[9]="REGION_T";
static char _tmp2A5[6]="TAG_T";static char _tmp2A6[7]="REGION";static char _tmp2A7[5]="RNEW";
static char _tmp2A8[8]="REGIONS";static char _tmp2A9[13]="RESET_REGION";static char
_tmp2AA[4]="GEN";static char _tmp2AB[16]="NOZEROTERM_QUAL";static char _tmp2AC[14]="ZEROTERM_QUAL";
static char _tmp2AD[7]="PORTON";static char _tmp2AE[8]="PORTOFF";static char _tmp2AF[8]="FLAT_kw";
static char _tmp2B0[12]="DYNREGION_T";static char _tmp2B1[6]="ALIAS";static char
_tmp2B2[8]="NUMELTS";static char _tmp2B3[8]="VALUEOF";static char _tmp2B4[10]="VALUEOF_T";
static char _tmp2B5[12]="TAGGED_QUAL";static char _tmp2B6[16]="EXTENSIBLE_QUAL";
static char _tmp2B7[15]="RESETABLE_QUAL";static char _tmp2B8[7]="PTR_OP";static char
_tmp2B9[7]="INC_OP";static char _tmp2BA[7]="DEC_OP";static char _tmp2BB[8]="LEFT_OP";
static char _tmp2BC[9]="RIGHT_OP";static char _tmp2BD[6]="LE_OP";static char _tmp2BE[6]="GE_OP";
static char _tmp2BF[6]="EQ_OP";static char _tmp2C0[6]="NE_OP";static char _tmp2C1[7]="AND_OP";
static char _tmp2C2[6]="OR_OP";static char _tmp2C3[11]="MUL_ASSIGN";static char
_tmp2C4[11]="DIV_ASSIGN";static char _tmp2C5[11]="MOD_ASSIGN";static char _tmp2C6[11]="ADD_ASSIGN";
static char _tmp2C7[11]="SUB_ASSIGN";static char _tmp2C8[12]="LEFT_ASSIGN";static
char _tmp2C9[13]="RIGHT_ASSIGN";static char _tmp2CA[11]="AND_ASSIGN";static char
_tmp2CB[11]="XOR_ASSIGN";static char _tmp2CC[10]="OR_ASSIGN";static char _tmp2CD[9]="ELLIPSIS";
static char _tmp2CE[11]="LEFT_RIGHT";static char _tmp2CF[12]="COLON_COLON";static
char _tmp2D0[11]="IDENTIFIER";static char _tmp2D1[17]="INTEGER_CONSTANT";static char
_tmp2D2[7]="STRING";static char _tmp2D3[19]="CHARACTER_CONSTANT";static char _tmp2D4[
18]="FLOATING_CONSTANT";static char _tmp2D5[9]="TYPE_VAR";static char _tmp2D6[13]="TYPEDEF_NAME";
static char _tmp2D7[16]="QUAL_IDENTIFIER";static char _tmp2D8[18]="QUAL_TYPEDEF_NAME";
static char _tmp2D9[10]="ATTRIBUTE";static char _tmp2DA[4]="';'";static char _tmp2DB[4]="'{'";
static char _tmp2DC[4]="'}'";static char _tmp2DD[4]="','";static char _tmp2DE[4]="'='";
static char _tmp2DF[4]="'<'";static char _tmp2E0[4]="'>'";static char _tmp2E1[4]="'('";
static char _tmp2E2[4]="')'";static char _tmp2E3[4]="'_'";static char _tmp2E4[4]="'$'";
static char _tmp2E5[4]="':'";static char _tmp2E6[4]="'.'";static char _tmp2E7[4]="'['";
static char _tmp2E8[4]="']'";static char _tmp2E9[4]="'*'";static char _tmp2EA[4]="'@'";
static char _tmp2EB[4]="'?'";static char _tmp2EC[4]="'+'";static char _tmp2ED[4]="'|'";
static char _tmp2EE[4]="'^'";static char _tmp2EF[4]="'&'";static char _tmp2F0[4]="'-'";
static char _tmp2F1[4]="'/'";static char _tmp2F2[4]="'%'";static char _tmp2F3[4]="'~'";
static char _tmp2F4[4]="'!'";static char _tmp2F5[5]="prog";static char _tmp2F6[17]="translation_unit";
static char _tmp2F7[12]="export_list";static char _tmp2F8[19]="export_list_values";
static char _tmp2F9[21]="external_declaration";static char _tmp2FA[20]="function_definition";
static char _tmp2FB[21]="function_definition2";static char _tmp2FC[13]="using_action";
static char _tmp2FD[15]="unusing_action";static char _tmp2FE[17]="namespace_action";
static char _tmp2FF[19]="unnamespace_action";static char _tmp300[12]="declaration";
static char _tmp301[19]="resetable_qual_opt";static char _tmp302[17]="declaration_list";
static char _tmp303[23]="declaration_specifiers";static char _tmp304[24]="storage_class_specifier";
static char _tmp305[15]="attributes_opt";static char _tmp306[11]="attributes";static
char _tmp307[15]="attribute_list";static char _tmp308[10]="attribute";static char
_tmp309[15]="type_specifier";static char _tmp30A[25]="type_specifier_notypedef";
static char _tmp30B[5]="kind";static char _tmp30C[15]="type_qualifier";static char
_tmp30D[15]="enum_specifier";static char _tmp30E[11]="enum_field";static char
_tmp30F[22]="enum_declaration_list";static char _tmp310[26]="struct_or_union_specifier";
static char _tmp311[16]="tagged_qual_opt";static char _tmp312[16]="type_params_opt";
static char _tmp313[16]="struct_or_union";static char _tmp314[24]="struct_declaration_list";
static char _tmp315[25]="struct_declaration_list0";static char _tmp316[21]="init_declarator_list";
static char _tmp317[22]="init_declarator_list0";static char _tmp318[16]="init_declarator";
static char _tmp319[19]="struct_declaration";static char _tmp31A[25]="specifier_qualifier_list";
static char _tmp31B[35]="notypedef_specifier_qualifier_list";static char _tmp31C[23]="struct_declarator_list";
static char _tmp31D[24]="struct_declarator_list0";static char _tmp31E[18]="struct_declarator";
static char _tmp31F[17]="tunion_specifier";static char _tmp320[9]="flat_opt";static
char _tmp321[18]="tunion_or_xtunion";static char _tmp322[15]="extensible_opt";
static char _tmp323[17]="tunionfield_list";static char _tmp324[18]="tunionfield_scope";
static char _tmp325[12]="tunionfield";static char _tmp326[11]="declarator";static
char _tmp327[23]="declarator_withtypedef";static char _tmp328[18]="direct_declarator";
static char _tmp329[30]="direct_declarator_withtypedef";static char _tmp32A[8]="pointer";
static char _tmp32B[12]="one_pointer";static char _tmp32C[23]="pointer_null_and_bound";
static char _tmp32D[14]="pointer_bound";static char _tmp32E[18]="zeroterm_qual_opt";
static char _tmp32F[12]="opt_rgn_opt";static char _tmp330[8]="rgn_opt";static char
_tmp331[11]="tqual_list";static char _tmp332[20]="parameter_type_list";static char
_tmp333[9]="type_var";static char _tmp334[16]="optional_effect";static char _tmp335[
19]="optional_rgn_order";static char _tmp336[10]="rgn_order";static char _tmp337[16]="optional_inject";
static char _tmp338[11]="effect_set";static char _tmp339[14]="atomic_effect";static
char _tmp33A[11]="region_set";static char _tmp33B[15]="parameter_list";static char
_tmp33C[22]="parameter_declaration";static char _tmp33D[16]="identifier_list";
static char _tmp33E[17]="identifier_list0";static char _tmp33F[12]="initializer";
static char _tmp340[18]="array_initializer";static char _tmp341[17]="initializer_list";
static char _tmp342[12]="designation";static char _tmp343[16]="designator_list";
static char _tmp344[11]="designator";static char _tmp345[10]="type_name";static char
_tmp346[14]="any_type_name";static char _tmp347[15]="type_name_list";static char
_tmp348[20]="abstract_declarator";static char _tmp349[27]="direct_abstract_declarator";
static char _tmp34A[10]="statement";static char _tmp34B[13]="open_exp_opt";static
char _tmp34C[18]="labeled_statement";static char _tmp34D[21]="expression_statement";
static char _tmp34E[19]="compound_statement";static char _tmp34F[16]="block_item_list";
static char _tmp350[20]="selection_statement";static char _tmp351[15]="switch_clauses";
static char _tmp352[20]="iteration_statement";static char _tmp353[15]="jump_statement";
static char _tmp354[12]="exp_pattern";static char _tmp355[20]="conditional_pattern";
static char _tmp356[19]="logical_or_pattern";static char _tmp357[20]="logical_and_pattern";
static char _tmp358[21]="inclusive_or_pattern";static char _tmp359[21]="exclusive_or_pattern";
static char _tmp35A[12]="and_pattern";static char _tmp35B[17]="equality_pattern";
static char _tmp35C[19]="relational_pattern";static char _tmp35D[14]="shift_pattern";
static char _tmp35E[17]="additive_pattern";static char _tmp35F[23]="multiplicative_pattern";
static char _tmp360[13]="cast_pattern";static char _tmp361[14]="unary_pattern";
static char _tmp362[16]="postfix_pattern";static char _tmp363[16]="primary_pattern";
static char _tmp364[8]="pattern";static char _tmp365[19]="tuple_pattern_list";static
char _tmp366[20]="tuple_pattern_list0";static char _tmp367[14]="field_pattern";
static char _tmp368[19]="field_pattern_list";static char _tmp369[20]="field_pattern_list0";
static char _tmp36A[11]="expression";static char _tmp36B[22]="assignment_expression";
static char _tmp36C[20]="assignment_operator";static char _tmp36D[23]="conditional_expression";
static char _tmp36E[20]="constant_expression";static char _tmp36F[22]="logical_or_expression";
static char _tmp370[23]="logical_and_expression";static char _tmp371[24]="inclusive_or_expression";
static char _tmp372[24]="exclusive_or_expression";static char _tmp373[15]="and_expression";
static char _tmp374[20]="equality_expression";static char _tmp375[22]="relational_expression";
static char _tmp376[17]="shift_expression";static char _tmp377[20]="additive_expression";
static char _tmp378[26]="multiplicative_expression";static char _tmp379[16]="cast_expression";
static char _tmp37A[17]="unary_expression";static char _tmp37B[15]="unary_operator";
static char _tmp37C[19]="postfix_expression";static char _tmp37D[19]="primary_expression";
static char _tmp37E[25]="argument_expression_list";static char _tmp37F[26]="argument_expression_list0";
static char _tmp380[9]="constant";static char _tmp381[20]="qual_opt_identifier";
static char _tmp382[17]="qual_opt_typedef";static char _tmp383[18]="struct_union_name";
static char _tmp384[11]="field_name";static char _tmp385[12]="right_angle";static
struct _dyneither_ptr Cyc_yytname[283]={{_tmp26B,_tmp26B,_tmp26B + 2},{_tmp26C,
_tmp26C,_tmp26C + 6},{_tmp26D,_tmp26D,_tmp26D + 12},{_tmp26E,_tmp26E,_tmp26E + 5},{
_tmp26F,_tmp26F,_tmp26F + 9},{_tmp270,_tmp270,_tmp270 + 7},{_tmp271,_tmp271,
_tmp271 + 7},{_tmp272,_tmp272,_tmp272 + 8},{_tmp273,_tmp273,_tmp273 + 5},{_tmp274,
_tmp274,_tmp274 + 5},{_tmp275,_tmp275,_tmp275 + 6},{_tmp276,_tmp276,_tmp276 + 4},{
_tmp277,_tmp277,_tmp277 + 5},{_tmp278,_tmp278,_tmp278 + 6},{_tmp279,_tmp279,
_tmp279 + 7},{_tmp27A,_tmp27A,_tmp27A + 7},{_tmp27B,_tmp27B,_tmp27B + 9},{_tmp27C,
_tmp27C,_tmp27C + 6},{_tmp27D,_tmp27D,_tmp27D + 9},{_tmp27E,_tmp27E,_tmp27E + 9},{
_tmp27F,_tmp27F,_tmp27F + 7},{_tmp280,_tmp280,_tmp280 + 6},{_tmp281,_tmp281,
_tmp281 + 5},{_tmp282,_tmp282,_tmp282 + 8},{_tmp283,_tmp283,_tmp283 + 7},{_tmp284,
_tmp284,_tmp284 + 7},{_tmp285,_tmp285,_tmp285 + 9},{_tmp286,_tmp286,_tmp286 + 3},{
_tmp287,_tmp287,_tmp287 + 5},{_tmp288,_tmp288,_tmp288 + 7},{_tmp289,_tmp289,
_tmp289 + 6},{_tmp28A,_tmp28A,_tmp28A + 3},{_tmp28B,_tmp28B,_tmp28B + 4},{_tmp28C,
_tmp28C,_tmp28C + 5},{_tmp28D,_tmp28D,_tmp28D + 9},{_tmp28E,_tmp28E,_tmp28E + 6},{
_tmp28F,_tmp28F,_tmp28F + 7},{_tmp290,_tmp290,_tmp290 + 5},{_tmp291,_tmp291,
_tmp291 + 8},{_tmp292,_tmp292,_tmp292 + 4},{_tmp293,_tmp293,_tmp293 + 6},{_tmp294,
_tmp294,_tmp294 + 4},{_tmp295,_tmp295,_tmp295 + 6},{_tmp296,_tmp296,_tmp296 + 7},{
_tmp297,_tmp297,_tmp297 + 4},{_tmp298,_tmp298,_tmp298 + 9},{_tmp299,_tmp299,
_tmp299 + 9},{_tmp29A,_tmp29A,_tmp29A + 6},{_tmp29B,_tmp29B,_tmp29B + 10},{_tmp29C,
_tmp29C,_tmp29C + 9},{_tmp29D,_tmp29D,_tmp29D + 8},{_tmp29E,_tmp29E,_tmp29E + 7},{
_tmp29F,_tmp29F,_tmp29F + 7},{_tmp2A0,_tmp2A0,_tmp2A0 + 8},{_tmp2A1,_tmp2A1,
_tmp2A1 + 7},{_tmp2A2,_tmp2A2,_tmp2A2 + 8},{_tmp2A3,_tmp2A3,_tmp2A3 + 5},{_tmp2A4,
_tmp2A4,_tmp2A4 + 9},{_tmp2A5,_tmp2A5,_tmp2A5 + 6},{_tmp2A6,_tmp2A6,_tmp2A6 + 7},{
_tmp2A7,_tmp2A7,_tmp2A7 + 5},{_tmp2A8,_tmp2A8,_tmp2A8 + 8},{_tmp2A9,_tmp2A9,
_tmp2A9 + 13},{_tmp2AA,_tmp2AA,_tmp2AA + 4},{_tmp2AB,_tmp2AB,_tmp2AB + 16},{_tmp2AC,
_tmp2AC,_tmp2AC + 14},{_tmp2AD,_tmp2AD,_tmp2AD + 7},{_tmp2AE,_tmp2AE,_tmp2AE + 8},{
_tmp2AF,_tmp2AF,_tmp2AF + 8},{_tmp2B0,_tmp2B0,_tmp2B0 + 12},{_tmp2B1,_tmp2B1,
_tmp2B1 + 6},{_tmp2B2,_tmp2B2,_tmp2B2 + 8},{_tmp2B3,_tmp2B3,_tmp2B3 + 8},{_tmp2B4,
_tmp2B4,_tmp2B4 + 10},{_tmp2B5,_tmp2B5,_tmp2B5 + 12},{_tmp2B6,_tmp2B6,_tmp2B6 + 16},{
_tmp2B7,_tmp2B7,_tmp2B7 + 15},{_tmp2B8,_tmp2B8,_tmp2B8 + 7},{_tmp2B9,_tmp2B9,
_tmp2B9 + 7},{_tmp2BA,_tmp2BA,_tmp2BA + 7},{_tmp2BB,_tmp2BB,_tmp2BB + 8},{_tmp2BC,
_tmp2BC,_tmp2BC + 9},{_tmp2BD,_tmp2BD,_tmp2BD + 6},{_tmp2BE,_tmp2BE,_tmp2BE + 6},{
_tmp2BF,_tmp2BF,_tmp2BF + 6},{_tmp2C0,_tmp2C0,_tmp2C0 + 6},{_tmp2C1,_tmp2C1,
_tmp2C1 + 7},{_tmp2C2,_tmp2C2,_tmp2C2 + 6},{_tmp2C3,_tmp2C3,_tmp2C3 + 11},{_tmp2C4,
_tmp2C4,_tmp2C4 + 11},{_tmp2C5,_tmp2C5,_tmp2C5 + 11},{_tmp2C6,_tmp2C6,_tmp2C6 + 11},{
_tmp2C7,_tmp2C7,_tmp2C7 + 11},{_tmp2C8,_tmp2C8,_tmp2C8 + 12},{_tmp2C9,_tmp2C9,
_tmp2C9 + 13},{_tmp2CA,_tmp2CA,_tmp2CA + 11},{_tmp2CB,_tmp2CB,_tmp2CB + 11},{
_tmp2CC,_tmp2CC,_tmp2CC + 10},{_tmp2CD,_tmp2CD,_tmp2CD + 9},{_tmp2CE,_tmp2CE,
_tmp2CE + 11},{_tmp2CF,_tmp2CF,_tmp2CF + 12},{_tmp2D0,_tmp2D0,_tmp2D0 + 11},{
_tmp2D1,_tmp2D1,_tmp2D1 + 17},{_tmp2D2,_tmp2D2,_tmp2D2 + 7},{_tmp2D3,_tmp2D3,
_tmp2D3 + 19},{_tmp2D4,_tmp2D4,_tmp2D4 + 18},{_tmp2D5,_tmp2D5,_tmp2D5 + 9},{_tmp2D6,
_tmp2D6,_tmp2D6 + 13},{_tmp2D7,_tmp2D7,_tmp2D7 + 16},{_tmp2D8,_tmp2D8,_tmp2D8 + 18},{
_tmp2D9,_tmp2D9,_tmp2D9 + 10},{_tmp2DA,_tmp2DA,_tmp2DA + 4},{_tmp2DB,_tmp2DB,
_tmp2DB + 4},{_tmp2DC,_tmp2DC,_tmp2DC + 4},{_tmp2DD,_tmp2DD,_tmp2DD + 4},{_tmp2DE,
_tmp2DE,_tmp2DE + 4},{_tmp2DF,_tmp2DF,_tmp2DF + 4},{_tmp2E0,_tmp2E0,_tmp2E0 + 4},{
_tmp2E1,_tmp2E1,_tmp2E1 + 4},{_tmp2E2,_tmp2E2,_tmp2E2 + 4},{_tmp2E3,_tmp2E3,
_tmp2E3 + 4},{_tmp2E4,_tmp2E4,_tmp2E4 + 4},{_tmp2E5,_tmp2E5,_tmp2E5 + 4},{_tmp2E6,
_tmp2E6,_tmp2E6 + 4},{_tmp2E7,_tmp2E7,_tmp2E7 + 4},{_tmp2E8,_tmp2E8,_tmp2E8 + 4},{
_tmp2E9,_tmp2E9,_tmp2E9 + 4},{_tmp2EA,_tmp2EA,_tmp2EA + 4},{_tmp2EB,_tmp2EB,
_tmp2EB + 4},{_tmp2EC,_tmp2EC,_tmp2EC + 4},{_tmp2ED,_tmp2ED,_tmp2ED + 4},{_tmp2EE,
_tmp2EE,_tmp2EE + 4},{_tmp2EF,_tmp2EF,_tmp2EF + 4},{_tmp2F0,_tmp2F0,_tmp2F0 + 4},{
_tmp2F1,_tmp2F1,_tmp2F1 + 4},{_tmp2F2,_tmp2F2,_tmp2F2 + 4},{_tmp2F3,_tmp2F3,
_tmp2F3 + 4},{_tmp2F4,_tmp2F4,_tmp2F4 + 4},{_tmp2F5,_tmp2F5,_tmp2F5 + 5},{_tmp2F6,
_tmp2F6,_tmp2F6 + 17},{_tmp2F7,_tmp2F7,_tmp2F7 + 12},{_tmp2F8,_tmp2F8,_tmp2F8 + 19},{
_tmp2F9,_tmp2F9,_tmp2F9 + 21},{_tmp2FA,_tmp2FA,_tmp2FA + 20},{_tmp2FB,_tmp2FB,
_tmp2FB + 21},{_tmp2FC,_tmp2FC,_tmp2FC + 13},{_tmp2FD,_tmp2FD,_tmp2FD + 15},{
_tmp2FE,_tmp2FE,_tmp2FE + 17},{_tmp2FF,_tmp2FF,_tmp2FF + 19},{_tmp300,_tmp300,
_tmp300 + 12},{_tmp301,_tmp301,_tmp301 + 19},{_tmp302,_tmp302,_tmp302 + 17},{
_tmp303,_tmp303,_tmp303 + 23},{_tmp304,_tmp304,_tmp304 + 24},{_tmp305,_tmp305,
_tmp305 + 15},{_tmp306,_tmp306,_tmp306 + 11},{_tmp307,_tmp307,_tmp307 + 15},{
_tmp308,_tmp308,_tmp308 + 10},{_tmp309,_tmp309,_tmp309 + 15},{_tmp30A,_tmp30A,
_tmp30A + 25},{_tmp30B,_tmp30B,_tmp30B + 5},{_tmp30C,_tmp30C,_tmp30C + 15},{_tmp30D,
_tmp30D,_tmp30D + 15},{_tmp30E,_tmp30E,_tmp30E + 11},{_tmp30F,_tmp30F,_tmp30F + 22},{
_tmp310,_tmp310,_tmp310 + 26},{_tmp311,_tmp311,_tmp311 + 16},{_tmp312,_tmp312,
_tmp312 + 16},{_tmp313,_tmp313,_tmp313 + 16},{_tmp314,_tmp314,_tmp314 + 24},{
_tmp315,_tmp315,_tmp315 + 25},{_tmp316,_tmp316,_tmp316 + 21},{_tmp317,_tmp317,
_tmp317 + 22},{_tmp318,_tmp318,_tmp318 + 16},{_tmp319,_tmp319,_tmp319 + 19},{
_tmp31A,_tmp31A,_tmp31A + 25},{_tmp31B,_tmp31B,_tmp31B + 35},{_tmp31C,_tmp31C,
_tmp31C + 23},{_tmp31D,_tmp31D,_tmp31D + 24},{_tmp31E,_tmp31E,_tmp31E + 18},{
_tmp31F,_tmp31F,_tmp31F + 17},{_tmp320,_tmp320,_tmp320 + 9},{_tmp321,_tmp321,
_tmp321 + 18},{_tmp322,_tmp322,_tmp322 + 15},{_tmp323,_tmp323,_tmp323 + 17},{
_tmp324,_tmp324,_tmp324 + 18},{_tmp325,_tmp325,_tmp325 + 12},{_tmp326,_tmp326,
_tmp326 + 11},{_tmp327,_tmp327,_tmp327 + 23},{_tmp328,_tmp328,_tmp328 + 18},{
_tmp329,_tmp329,_tmp329 + 30},{_tmp32A,_tmp32A,_tmp32A + 8},{_tmp32B,_tmp32B,
_tmp32B + 12},{_tmp32C,_tmp32C,_tmp32C + 23},{_tmp32D,_tmp32D,_tmp32D + 14},{
_tmp32E,_tmp32E,_tmp32E + 18},{_tmp32F,_tmp32F,_tmp32F + 12},{_tmp330,_tmp330,
_tmp330 + 8},{_tmp331,_tmp331,_tmp331 + 11},{_tmp332,_tmp332,_tmp332 + 20},{_tmp333,
_tmp333,_tmp333 + 9},{_tmp334,_tmp334,_tmp334 + 16},{_tmp335,_tmp335,_tmp335 + 19},{
_tmp336,_tmp336,_tmp336 + 10},{_tmp337,_tmp337,_tmp337 + 16},{_tmp338,_tmp338,
_tmp338 + 11},{_tmp339,_tmp339,_tmp339 + 14},{_tmp33A,_tmp33A,_tmp33A + 11},{
_tmp33B,_tmp33B,_tmp33B + 15},{_tmp33C,_tmp33C,_tmp33C + 22},{_tmp33D,_tmp33D,
_tmp33D + 16},{_tmp33E,_tmp33E,_tmp33E + 17},{_tmp33F,_tmp33F,_tmp33F + 12},{
_tmp340,_tmp340,_tmp340 + 18},{_tmp341,_tmp341,_tmp341 + 17},{_tmp342,_tmp342,
_tmp342 + 12},{_tmp343,_tmp343,_tmp343 + 16},{_tmp344,_tmp344,_tmp344 + 11},{
_tmp345,_tmp345,_tmp345 + 10},{_tmp346,_tmp346,_tmp346 + 14},{_tmp347,_tmp347,
_tmp347 + 15},{_tmp348,_tmp348,_tmp348 + 20},{_tmp349,_tmp349,_tmp349 + 27},{
_tmp34A,_tmp34A,_tmp34A + 10},{_tmp34B,_tmp34B,_tmp34B + 13},{_tmp34C,_tmp34C,
_tmp34C + 18},{_tmp34D,_tmp34D,_tmp34D + 21},{_tmp34E,_tmp34E,_tmp34E + 19},{
_tmp34F,_tmp34F,_tmp34F + 16},{_tmp350,_tmp350,_tmp350 + 20},{_tmp351,_tmp351,
_tmp351 + 15},{_tmp352,_tmp352,_tmp352 + 20},{_tmp353,_tmp353,_tmp353 + 15},{
_tmp354,_tmp354,_tmp354 + 12},{_tmp355,_tmp355,_tmp355 + 20},{_tmp356,_tmp356,
_tmp356 + 19},{_tmp357,_tmp357,_tmp357 + 20},{_tmp358,_tmp358,_tmp358 + 21},{
_tmp359,_tmp359,_tmp359 + 21},{_tmp35A,_tmp35A,_tmp35A + 12},{_tmp35B,_tmp35B,
_tmp35B + 17},{_tmp35C,_tmp35C,_tmp35C + 19},{_tmp35D,_tmp35D,_tmp35D + 14},{
_tmp35E,_tmp35E,_tmp35E + 17},{_tmp35F,_tmp35F,_tmp35F + 23},{_tmp360,_tmp360,
_tmp360 + 13},{_tmp361,_tmp361,_tmp361 + 14},{_tmp362,_tmp362,_tmp362 + 16},{
_tmp363,_tmp363,_tmp363 + 16},{_tmp364,_tmp364,_tmp364 + 8},{_tmp365,_tmp365,
_tmp365 + 19},{_tmp366,_tmp366,_tmp366 + 20},{_tmp367,_tmp367,_tmp367 + 14},{
_tmp368,_tmp368,_tmp368 + 19},{_tmp369,_tmp369,_tmp369 + 20},{_tmp36A,_tmp36A,
_tmp36A + 11},{_tmp36B,_tmp36B,_tmp36B + 22},{_tmp36C,_tmp36C,_tmp36C + 20},{
_tmp36D,_tmp36D,_tmp36D + 23},{_tmp36E,_tmp36E,_tmp36E + 20},{_tmp36F,_tmp36F,
_tmp36F + 22},{_tmp370,_tmp370,_tmp370 + 23},{_tmp371,_tmp371,_tmp371 + 24},{
_tmp372,_tmp372,_tmp372 + 24},{_tmp373,_tmp373,_tmp373 + 15},{_tmp374,_tmp374,
_tmp374 + 20},{_tmp375,_tmp375,_tmp375 + 22},{_tmp376,_tmp376,_tmp376 + 17},{
_tmp377,_tmp377,_tmp377 + 20},{_tmp378,_tmp378,_tmp378 + 26},{_tmp379,_tmp379,
_tmp379 + 16},{_tmp37A,_tmp37A,_tmp37A + 17},{_tmp37B,_tmp37B,_tmp37B + 15},{
_tmp37C,_tmp37C,_tmp37C + 19},{_tmp37D,_tmp37D,_tmp37D + 19},{_tmp37E,_tmp37E,
_tmp37E + 25},{_tmp37F,_tmp37F,_tmp37F + 26},{_tmp380,_tmp380,_tmp380 + 9},{_tmp381,
_tmp381,_tmp381 + 20},{_tmp382,_tmp382,_tmp382 + 17},{_tmp383,_tmp383,_tmp383 + 18},{
_tmp384,_tmp384,_tmp384 + 11},{_tmp385,_tmp385,_tmp385 + 12}};static short Cyc_yyr1[
506]={0,138,139,139,139,139,139,139,139,139,139,139,140,140,141,141,142,142,142,
143,143,143,143,144,144,145,146,147,148,149,149,149,149,149,149,149,149,150,150,
151,151,152,152,152,152,152,152,152,152,152,152,153,153,153,153,153,153,153,154,
154,155,156,156,157,157,157,157,157,157,158,158,159,159,159,159,159,159,159,159,
159,159,159,159,159,159,159,159,159,159,159,159,159,159,160,161,161,161,162,162,
162,163,163,164,164,164,165,165,165,166,166,167,167,168,168,169,169,170,170,171,
172,172,173,173,174,175,175,175,175,175,175,176,176,176,176,176,176,177,178,178,
179,179,179,180,180,180,181,181,182,182,182,183,183,184,184,184,184,185,185,185,
186,186,187,187,188,188,189,189,189,189,189,189,189,189,189,189,190,190,190,190,
190,190,190,190,190,190,190,191,191,192,193,193,193,194,194,195,195,195,196,196,
196,197,197,197,198,198,199,199,199,199,200,200,201,201,202,202,203,203,204,204,
205,205,206,206,206,206,207,207,208,208,209,209,209,210,211,211,212,212,213,213,
213,213,214,214,214,214,215,216,216,217,217,218,218,219,219,219,219,219,220,220,
221,221,221,222,222,222,222,222,222,222,222,222,222,222,223,223,223,223,223,223,
223,224,224,225,226,226,227,227,228,228,228,228,228,228,229,229,229,229,229,229,
230,230,230,230,230,230,231,231,231,231,231,231,231,231,231,231,231,231,231,231,
232,232,232,232,232,232,232,232,233,234,234,235,235,236,236,237,237,238,238,239,
239,240,240,240,241,241,241,241,241,242,242,242,243,243,243,244,244,244,244,245,
245,246,246,246,246,246,246,246,247,248,249,249,249,249,249,249,249,249,249,249,
249,249,249,249,250,250,250,251,251,252,252,253,253,253,254,254,255,255,256,256,
256,257,257,257,257,257,257,257,257,257,257,257,258,258,258,258,258,258,258,259,
260,260,261,261,262,262,263,263,264,264,265,265,265,266,266,266,266,266,267,267,
267,268,268,268,269,269,269,269,270,270,271,271,271,271,271,271,271,271,271,271,
271,271,271,271,271,271,271,271,272,272,272,273,273,273,273,273,273,273,273,273,
273,274,274,274,274,274,274,274,274,274,275,276,276,277,277,277,277,278,278,279,
279,280,280,281,281,282,282};static short Cyc_yyr2[506]={0,1,2,3,5,3,5,6,7,3,3,0,4,
3,2,3,1,1,1,2,3,3,4,3,4,2,1,2,1,2,3,5,3,7,4,4,8,0,1,1,2,1,2,1,2,1,2,1,2,1,2,1,1,
1,1,2,1,1,0,1,6,1,3,1,1,4,4,4,8,1,2,1,1,3,1,1,1,1,1,1,1,1,1,1,1,1,4,4,4,6,4,1,4,
1,1,1,1,5,2,4,1,3,1,2,3,4,9,3,0,1,0,3,1,1,0,1,1,2,1,1,3,1,3,3,1,2,1,2,1,2,1,2,1,
2,1,2,1,1,3,1,2,3,9,6,8,1,0,2,1,1,0,1,1,2,3,3,0,1,1,2,5,1,2,1,2,1,3,4,4,5,4,5,4,
4,2,1,1,3,4,4,5,4,5,4,4,2,1,2,5,2,2,1,0,3,0,1,1,0,1,1,0,1,1,0,2,3,5,5,7,1,3,0,2,
0,2,3,5,0,1,1,3,2,3,4,1,1,3,1,3,2,1,2,1,1,3,1,1,2,3,4,8,1,2,3,4,2,1,2,3,2,1,2,1,
2,3,4,3,1,3,1,1,2,3,3,4,4,5,4,3,5,4,4,2,1,1,1,1,1,1,5,0,5,3,1,2,2,3,1,2,1,2,1,2,
5,7,7,5,8,6,0,3,4,5,6,7,5,7,6,7,7,8,7,8,8,9,6,7,7,8,3,2,2,2,3,2,4,5,1,1,5,1,3,1,
3,1,3,1,3,1,3,1,3,3,1,3,3,3,3,1,3,3,1,3,3,1,3,3,3,1,4,1,2,2,4,2,6,6,1,1,1,3,1,1,
3,4,4,5,2,2,2,4,4,4,1,3,1,1,3,1,2,1,3,1,1,3,1,3,1,3,3,1,1,1,1,1,1,1,1,1,1,1,1,5,
2,2,2,5,5,1,1,3,1,3,1,3,1,3,1,3,1,3,3,1,3,3,3,3,1,3,3,1,3,3,1,3,3,3,1,4,1,2,2,2,
2,2,2,4,2,6,6,5,4,6,9,11,4,4,1,1,1,1,4,3,4,3,3,2,2,6,7,1,1,1,3,2,5,4,5,5,1,1,3,1,
1,1,1,1,1,1,1,1,1,1,1,1,1};static short Cyc_yydefact[1007]={0,18,51,52,53,54,56,71,
74,75,76,77,78,79,80,81,94,95,96,112,113,47,0,0,57,0,0,0,91,0,0,0,145,0,0,0,496,
209,498,497,499,0,0,72,0,192,192,191,1,0,16,0,0,17,0,41,49,43,69,45,82,83,0,84,0,
146,161,0,186,194,85,165,110,55,54,48,0,98,495,0,496,492,493,494,0,369,0,0,0,0,
232,0,371,372,25,27,146,146,280,0,0,0,146,0,0,0,0,0,0,0,146,0,189,190,2,0,0,0,0,
29,0,118,119,146,42,50,44,46,146,500,501,110,150,149,148,197,146,39,146,0,19,146,
146,0,174,162,187,196,195,200,146,70,0,55,102,0,100,0,496,378,0,0,0,0,0,0,0,0,0,
0,0,110,0,0,0,0,482,146,0,0,0,0,469,467,468,0,395,397,411,419,421,423,425,427,
429,432,437,440,443,447,0,449,470,481,480,0,379,377,32,0,0,110,0,0,0,128,124,126,
250,252,0,0,0,0,0,0,9,10,0,0,0,502,503,210,93,0,0,166,73,230,0,227,0,3,0,5,0,30,
0,0,146,20,0,146,116,0,107,151,147,199,0,198,0,0,0,0,0,0,0,0,0,0,0,0,496,283,285,
0,146,146,0,146,273,274,275,0,276,277,278,0,40,21,121,257,0,217,233,0,0,213,211,
0,194,0,202,58,201,0,0,103,99,0,0,373,0,0,146,457,146,413,447,0,414,415,0,0,0,0,
0,0,0,146,146,450,451,146,0,0,0,453,454,452,0,370,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,401,402,403,404,405,406,407,408,409,410,400,0,455,0,476,477,0,0,0,484,
0,110,385,386,0,383,0,234,0,0,0,146,253,225,0,129,134,130,132,125,127,146,0,259,
251,260,505,504,0,87,90,35,0,34,0,146,88,0,92,64,63,0,61,167,146,229,163,259,231,
175,176,146,86,193,26,0,28,0,120,122,236,235,22,105,117,0,0,0,136,137,139,0,109,
0,110,0,0,0,0,0,0,146,0,320,321,322,0,0,324,0,0,0,146,292,288,146,290,286,284,
146,173,218,146,0,0,224,212,219,170,0,0,146,213,172,168,194,203,59,111,0,104,418,
101,97,381,382,0,0,0,237,0,0,241,0,0,0,246,0,0,0,0,0,146,0,0,0,0,0,483,490,0,489,
396,420,0,422,424,426,428,430,431,435,436,433,434,438,439,441,442,444,445,446,
399,398,475,472,0,474,0,146,0,374,0,380,31,392,0,388,393,0,390,375,0,0,254,135,
131,133,0,213,0,194,0,261,146,146,0,272,256,0,37,0,0,0,0,0,128,0,110,146,146,0,
185,164,228,4,6,0,140,123,0,0,110,0,150,0,0,0,299,0,0,0,0,0,319,323,0,0,0,0,282,
146,23,258,211,146,221,0,0,214,0,171,217,205,169,203,188,0,7,0,456,0,0,249,0,238,
0,242,245,247,461,0,0,0,0,0,465,466,0,0,0,448,486,0,0,473,471,0,0,384,387,389,
376,0,255,226,268,0,262,263,194,0,0,213,194,0,0,38,0,89,0,0,0,0,60,62,0,177,0,0,
213,0,194,0,138,141,213,110,143,0,0,0,0,0,0,0,0,0,0,0,0,0,299,325,0,0,24,213,0,
222,220,0,213,146,204,0,8,0,0,0,248,239,243,0,0,0,0,416,417,460,488,0,491,412,
485,487,0,391,394,267,265,271,270,0,264,194,281,33,0,0,67,65,66,178,184,181,0,
183,179,194,146,150,156,293,299,0,0,0,146,0,0,327,328,330,332,334,336,338,340,
343,348,351,354,358,360,367,368,0,146,296,305,0,0,0,0,0,0,0,0,0,0,326,279,207,
223,215,206,211,13,0,0,459,458,0,244,462,146,0,478,0,269,266,36,0,182,180,0,144,
158,157,0,0,152,0,0,299,146,364,146,0,361,146,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,362,300,0,307,0,0,0,315,0,0,0,0,0,0,298,0,213,12,14,0,0,0,0,479,0,106,142,
159,156,156,294,295,0,0,0,0,299,301,331,0,333,335,337,339,341,342,346,347,344,
345,349,350,352,353,355,356,357,0,306,308,309,0,317,316,0,311,0,0,0,216,208,15,0,
0,146,0,146,155,154,297,363,0,359,302,0,146,310,318,312,313,0,240,463,0,68,0,0,0,
329,299,303,314,0,160,366,365,304,464,0,0,0};static short Cyc_yydefgoto[145]={1004,
48,653,843,49,50,278,51,439,52,441,53,702,138,54,55,500,215,425,426,216,58,233,
217,60,154,155,61,457,151,62,252,253,120,121,122,254,218,403,451,452,453,63,64,
135,258,864,865,866,65,454,66,430,67,68,69,112,149,260,305,650,582,70,583,494,
643,486,490,491,398,300,241,89,90,516,444,517,518,519,520,219,293,294,584,409,
281,224,282,283,284,285,286,728,287,288,804,805,806,807,808,809,810,811,812,813,
814,815,816,817,818,819,387,388,389,572,573,574,289,186,375,187,506,188,189,190,
191,192,193,194,195,196,197,198,199,200,201,202,534,535,203,204,72,844,234,413};
static short Cyc_yypact[1007]={2838,- -32768,- -32768,- -32768,- -32768,28,- -32768,-
-32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,-
-32768,- -32768,- -32768,- -32768,3311,215,220,- -32768,169,91,63,110,10,141,143,-
-32768,140,167,182,- -32768,233,- -32768,- -32768,- -32768,247,487,269,279,219,219,-
-32768,- -32768,2709,- -32768,242,273,- -32768,637,3311,3311,3311,- -32768,3311,-
-32768,- -32768,624,- -32768,631,3237,316,- 19,697,462,- -32768,- -32768,287,306,334,
- -32768,169,345,- -32768,1113,130,- -32768,- -32768,- -32768,5946,- -32768,341,360,
1113,352,358,359,- -32768,197,- -32768,- -32768,3575,3575,44,377,2709,2709,3575,379,
5946,265,378,692,394,265,3905,5946,- -32768,- -32768,- -32768,2709,2966,2709,2966,-
-32768,387,388,- -32768,3158,- -32768,- -32768,- -32768,- -32768,3641,- -32768,- -32768,
287,444,- -32768,- -32768,43,1912,- -32768,3237,637,- -32768,3575,3377,4452,- -32768,
316,- -32768,- -32768,- -32768,75,3575,- -32768,2966,- -32768,419,427,435,169,172,-
-32768,1113,123,6123,463,5946,5990,471,473,480,532,536,287,540,542,6209,6209,-
-32768,2312,546,6253,6253,6253,- -32768,- -32768,- -32768,236,- -32768,- -32768,- 53,
605,499,578,569,363,90,484,94,261,- -32768,2517,6253,589,7,- -32768,639,4443,640,-
-32768,- -32768,652,5946,287,4443,641,213,3509,3707,3509,559,- -32768,79,79,650,
687,702,699,- -32768,- -32768,116,704,272,- -32768,- -32768,- -32768,- -32768,41,716,-
-32768,- -32768,1015,301,- -32768,736,- -32768,739,- -32768,741,- -32768,692,4542,
3237,- -32768,743,3773,- -32768,921,- 21,- -32768,- -32768,- -32768,169,- -32768,744,
193,747,4002,749,767,758,759,4632,4002,221,753,750,- -32768,- -32768,755,1340,1507,
637,1642,- -32768,- -32768,- -32768,761,- -32768,- -32768,- -32768,311,- -32768,- -32768,
763,751,103,775,- -32768,13,760,762,324,764,462,756,- -32768,772,- -32768,103,777,
169,- -32768,5946,779,- -32768,771,776,2312,- -32768,3905,- -32768,- -32768,4092,-
-32768,807,5946,5946,5946,5946,5946,778,5946,3905,2312,- -32768,- -32768,2047,780,
392,5946,- -32768,- -32768,- -32768,5946,- -32768,6253,5946,6253,6253,6253,6253,6253,
6253,6253,6253,6253,6253,6253,6253,6253,6253,6253,6253,6253,5946,- -32768,- -32768,
- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,5946,-
-32768,265,- -32768,- -32768,4719,265,5946,- -32768,781,287,- -32768,- -32768,782,788,
1113,- -32768,362,1066,786,3575,- -32768,795,797,- -32768,3707,3707,3707,- -32768,-
-32768,3032,4806,264,- -32768,383,- -32768,- -32768,13,- -32768,- -32768,- -32768,793,
- -32768,811,3575,- -32768,812,- -32768,- -32768,799,800,806,- -32768,987,- -32768,476,
596,- -32768,- -32768,- -32768,3905,- -32768,- -32768,- -32768,2709,- -32768,2709,-
-32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,1161,5946,813,814,- -32768,
801,250,- -32768,815,240,5946,5946,808,818,5946,895,2182,820,- -32768,- -32768,-
-32768,430,890,- -32768,4896,5946,4002,2442,- -32768,- -32768,3158,- -32768,- -32768,
- -32768,3575,- -32768,- -32768,3905,817,231,- -32768,- -32768,810,- -32768,13,819,
3839,762,- -32768,- -32768,462,809,- -32768,- -32768,2580,- -32768,- -32768,- -32768,-
-32768,- -32768,- -32768,821,822,836,- -32768,265,5946,- -32768,459,4542,284,- -32768,
452,827,828,830,511,3905,514,848,849,833,6079,- -32768,- -32768,850,835,- -32768,
605,72,499,578,569,363,90,90,484,484,484,484,94,94,261,261,- -32768,- -32768,-
-32768,- -32768,- -32768,- -32768,- -32768,852,- -32768,8,3575,4362,- -32768,4797,-
-32768,- -32768,- -32768,1113,- -32768,- -32768,834,860,- -32768,32,869,- -32768,-
-32768,- -32768,- -32768,857,762,858,462,859,383,3575,3443,4983,- -32768,- -32768,
5946,912,79,874,740,872,41,1192,873,287,3575,3377,5073,- -32768,476,- -32768,-
-32768,- -32768,1020,- -32768,- -32768,921,5946,287,169,444,518,527,5946,579,543,
876,5163,5250,512,- -32768,- -32768,897,899,893,570,- -32768,3237,- -32768,751,902,
3575,- -32768,901,13,- -32768,898,- -32768,259,- -32768,- -32768,809,- -32768,904,-
-32768,2709,905,309,907,- -32768,896,- -32768,4182,- -32768,- -32768,- -32768,- -32768,
5946,993,5946,5990,906,- -32768,- -32768,905,908,4362,- -32768,- -32768,5946,5946,-
-32768,- -32768,103,530,- -32768,- -32768,- -32768,- -32768,6330,- -32768,- -32768,-
-32768,913,- -32768,- -32768,462,103,915,762,462,911,581,- -32768,931,- -32768,5946,
597,927,934,- -32768,- -32768,938,- -32768,103,939,762,940,462,925,- -32768,- -32768,
762,287,914,4002,949,943,3864,941,952,4002,5946,5337,588,5424,607,5511,579,-
-32768,955,957,- -32768,762,106,- -32768,- -32768,963,762,3905,- -32768,509,- -32768,
953,954,5946,- -32768,- -32768,- -32768,4542,609,956,961,- -32768,807,- -32768,-
-32768,557,- -32768,- -32768,- -32768,- -32768,4362,- -32768,- -32768,- -32768,- -32768,
- -32768,- -32768,958,- -32768,462,- -32768,- -32768,629,969,- -32768,- -32768,- -32768,
- -32768,- -32768,- -32768,960,- -32768,- -32768,462,3641,444,768,1048,579,968,6339,
964,2442,6253,965,- -32768,- 14,- -32768,1000,959,649,745,181,752,153,336,- -32768,-
-32768,- -32768,- -32768,1004,6253,2047,- -32768,- -32768,610,4002,630,5598,4002,653,
5685,5772,646,986,- -32768,- -32768,- -32768,- -32768,988,- -32768,902,- -32768,990,
691,- -32768,- -32768,137,- -32768,- -32768,3905,1075,- -32768,4272,- -32768,- -32768,-
-32768,996,- -32768,- -32768,999,- -32768,- -32768,- -32768,1005,169,698,4002,1012,
579,2312,- -32768,3905,982,- -32768,1777,6253,5946,6253,6253,6253,6253,6253,6253,
6253,6253,6253,6253,6253,6253,6253,6253,6253,6253,6253,5946,- -32768,- -32768,1006,
- -32768,4002,4002,656,- -32768,4002,4002,663,4002,673,5859,- -32768,13,762,- -32768,
- -32768,195,5946,1007,1013,- -32768,1030,- -32768,- -32768,1016,45,768,- -32768,-
-32768,1023,1018,1026,6253,579,- -32768,605,139,499,578,578,363,90,90,484,484,484,
484,94,94,261,261,- -32768,- -32768,- -32768,158,- -32768,- -32768,- -32768,4002,-
-32768,- -32768,4002,- -32768,4002,4002,676,- -32768,- -32768,- -32768,725,1025,3905,
1031,3905,- -32768,- -32768,- -32768,905,421,- -32768,- -32768,5946,1777,- -32768,-
-32768,- -32768,- -32768,4002,- -32768,- -32768,1034,- -32768,677,1036,1039,- -32768,
579,- -32768,- -32768,1041,- -32768,- -32768,- -32768,- -32768,- -32768,1145,1149,-
-32768};static short Cyc_yypgoto[145]={- -32768,- 8,- -32768,248,- -32768,- -32768,-
-32768,- -32768,- -32768,- -32768,- -32768,- 60,- -32768,- 100,111,- -32768,- -32768,0,
563,- -32768,39,- 162,1056,12,- -32768,- -32768,- 87,- -32768,- -32768,- 128,- -32768,375,
- -32768,- -32768,- -32768,924,920,59,445,- -32768,- -32768,561,- -32768,- -32768,-
-32768,- 582,- 66,- -32768,- -32768,206,- 217,1109,- 253,203,- -32768,- -32768,1131,150,
- -32768,- -32768,529,- 123,- 117,- 107,- 291,270,534,541,- 350,- 436,- 109,- 405,- 136,-
-32768,- 233,- 163,- 550,- 353,- -32768,666,- 133,113,- 119,- 65,- 207,78,- -32768,- -32768,
- -32768,- 36,- 271,- -32768,- 633,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,-
-32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,-
-32768,- -32768,285,970,- -32768,501,- -32768,- -32768,426,313,- -32768,- 140,- 378,-
148,- 340,- 326,- 332,843,- 322,- 325,- 307,- 319,- 279,- 95,604,468,- -32768,- -32768,- 352,
- -32768,124,467,- 51,1133,- 368,- 39};static short Cyc_yytable[6477]={56,240,322,256,
537,137,301,477,478,558,480,130,59,561,682,540,443,323,261,298,539,56,429,250,
319,543,544,542,560,140,608,307,306,59,344,299,722,549,550,57,570,114,107,329,
336,545,546,547,548,56,862,863,641,456,401,56,56,56,423,56,57,59,592,137,530,56,
144,59,59,59,312,59,612,876,487,345,279,59,290,551,552,638,36,393,339,340,341,
251,57,39,608,- 108,226,227,57,57,57,397,57,42,56,56,291,834,57,376,383,243,244,
245,246,98,59,59,877,56,56,56,56,37,222,632,342,56,765,488,99,59,59,59,59,73,75,
680,384,59,56,658,56,57,57,689,424,644,308,144,657,92,59,37,59,688,56,408,57,57,
57,57,- 153,223,410,412,57,259,59,868,124,125,126,239,127,505,352,353,432,57,139,
57,607,96,489,37,414,510,410,511,342,255,434,420,290,57,95,496,678,304,411,410,
528,529,587,239,607,92,434,647,354,355,412,220,221,601,92,861,446,228,400,483,
279,279,411,279,504,358,587,838,97,359,402,314,419,160,601,411,139,412,928,719,
401,401,401,- 233,933,315,- 233,412,161,280,108,139,56,342,100,342,101,484,102,564,
78,916,123,980,59,884,885,553,554,555,502,725,36,146,342,160,399,279,404,39,56,
56,981,56,890,103,92,661,891,752,161,57,59,59,691,59,36,489,36,886,887,979,104,
39,38,39,40,501,79,757,91,211,505,460,255,236,461,212,36,57,57,37,57,80,81,39,82,
83,396,76,39,92,618,111,472,105,570,56,92,37,84,473,85,86,841,464,640,292,87,59,
996,471,342,36,88,115,116,343,150,38,39,40,485,139,1002,617,159,106,231,240,449,
109,297,397,232,207,57,505,489,602,- 211,635,434,- 211,405,710,117,118,342,360,406,
280,280,422,280,669,710,361,362,110,434,662,400,400,400,150,434,626,777,514,515,
591,231,751,402,402,402,435,232,757,152,137,436,407,482,790,242,342,41,756,600,
794,606,609,141,610,142,297,675,153,495,56,143,56,431,636,681,313,280,350,351,
611,837,59,498,59,292,840,303,157,455,205,397,206,892,208,239,56,696,71,715,695,
893,894,209,568,210,505,342,59,57,56,57,713,697,225,712,229,479,239,720,77,93,59,
94,41,239,652,235,714,247,588,757,589,248,56,57,761,342,590,576,71,185,532,649,
237,92,59,71,92,57,257,762,71,231,992,848,489,147,148,232,129,230,741,595,309,71,
239,934,756,767,746,310,629,57,156,342,239,93,937,938,602,311,897,936,634,239,93,
703,940,941,939,434,644,445,434,356,357,342,71,71,946,947,664,659,660,71,290,139,
942,943,944,945,318,71,71,71,71,41,591,36,324,139,325,603,795,604,39,637,41,326,
740,605,726,727,337,932,42,71,606,407,993,36,948,949,45,46,47,38,39,40,71,756,
966,842,735,156,342,342,93,342,347,668,431,342,670,445,56,392,723,522,523,524,
342,768,769,770,750,724,59,239,648,327,533,455,56,328,536,775,342,330,399,331,
649,729,239,338,59,377,378,379,873,852,853,93,788,57,567,556,405,571,93,132,133,
134,406,342,45,46,47,557,739,92,346,57,533,92,342,470,36,130,828,780,349,342,38,
39,40,433,380,874,348,995,783,381,382,428,71,784,917,831,586,406,342,433,342,342,
36,896,458,849,898,462,38,39,40,912,693,128,929,36,930,856,390,337,342,342,39,
139,71,119,900,521,385,742,391,525,42,527,909,337,395,342,415,279,45,46,47,317,
342,320,320,342,538,904,862,863,957,156,342,333,334,880,881,960,320,320,320,533,
342,416,533,342,435,962,36,489,986,999,950,951,952,39,797,914,455,320,915,239,
824,562,924,42,92,925,417,455,279,418,455,45,46,47,421,56,45,46,47,16,17,18,882,
883,445,888,889,59,427,978,989,987,342,994,705,706,707,774,579,580,581,778,437,
92,684,438,255,440,685,447,93,973,974,93,57,459,991,130,463,792,465,466,467,468,
474,475,476,481,56,485,445,249,492,412,499,41,497,493,619,620,59,508,623,503,627,
507,509,344,433,526,563,433,531,633,565,566,699,899,575,71,903,71,577,578,593,
594,596,57,320,433,597,717,598,599,279,433,615,613,624,621,616,614,855,622,628,
630,280,533,639,655,656,645,642,654,665,666,859,667,926,673,686,320,677,320,320,
320,320,320,320,320,320,320,320,320,320,320,320,320,320,320,671,672,676,71,679,
571,445,687,37,690,692,955,956,760,56,958,959,694,961,280,445,701,704,766,708,
711,59,730,7,8,9,10,11,12,13,14,15,16,17,18,19,20,736,737,820,738,297,743,745,
749,674,759,700,57,754,36,753,22,763,796,764,38,39,40,239,773,93,776,982,779,93,
983,449,984,985,781,450,27,28,785,45,46,47,793,732,734,786,320,32,33,787,789,791,
35,798,799,822,997,823,835,433,836,839,445,857,845,846,850,851,867,854,433,858,
869,433,872,445,721,295,878,875,36,879,895,758,280,37,38,39,40,41,297,910,918,
931,911,913,78,428,- 211,43,44,- 211,920,406,921,45,46,47,36,954,922,320,71,36,38,
39,40,927,969,38,39,40,782,970,971,428,972,320,975,976,449,406,977,45,46,47,988,
1005,45,46,47,1006,990,78,79,998,93,1000,825,827,1001,830,1003,833,709,967,569,
238,445,158,81,860,82,83,442,448,39,718,145,113,748,847,747,965,394,744,84,663,
85,86,772,514,515,541,87,93,821,131,0,0,88,79,7,8,9,10,11,12,13,14,15,16,17,18,
19,20,158,81,129,82,83,320,0,39,0,0,0,0,0,0,185,22,0,84,0,85,86,0,0,0,0,87,0,-
146,- 146,- 146,0,88,0,0,0,27,28,0,0,0,902,0,0,906,908,0,32,33,36,0,0,35,0,0,38,39,
40,41,320,0,0,0,0,0,0,449,0,0,320,0,0,0,0,45,46,47,0,0,0,36,0,0,337,0,37,38,39,
40,41,935,0,0,0,0,0,0,449,0,43,44,0,0,0,0,45,46,47,953,0,0,0,0,0,0,0,0,0,0,923,0,
0,964,0,0,0,0,0,0,968,2,3,4,74,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,- 291,-
291,21,162,163,262,0,263,264,265,266,267,268,269,270,22,78,23,164,271,129,0,165,
24,272,0,0,0,0,0,166,167,168,169,0,27,28,29,170,0,273,171,871,0,0,320,32,33,34,
172,173,35,0,0,0,0,174,175,0,0,0,0,0,320,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,274,81,
176,82,83,37,38,39,40,41,275,136,- 291,0,0,0,0,177,0,43,277,0,0,0,0,179,0,0,180,0,
0,181,182,0,0,183,184,0,0,320,0,320,320,320,320,320,320,320,320,320,320,320,320,
320,320,320,320,320,0,0,0,0,0,0,0,0,0,0,0,2,3,4,74,6,7,8,9,10,11,12,13,14,15,16,
17,18,19,20,- 287,- 287,21,162,163,262,320,263,264,265,266,267,268,269,270,22,78,
23,164,271,0,0,165,24,272,0,0,0,0,0,166,167,168,169,0,27,28,29,170,0,273,171,0,0,
0,0,32,33,34,172,173,35,0,0,0,320,174,175,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,274,81,176,82,83,37,38,39,40,41,275,136,- 287,0,0,0,0,177,0,43,277,0,0,0,0,
179,0,0,180,0,0,181,182,0,0,183,184,2,3,4,74,6,7,8,9,10,11,12,13,14,15,16,17,18,
19,20,- 289,- 289,21,162,163,262,0,263,264,265,266,267,268,269,270,22,78,23,164,
271,0,0,165,24,272,0,0,0,0,0,166,167,168,169,0,27,28,29,170,0,273,171,0,0,0,0,32,
33,34,172,173,35,0,0,0,0,174,175,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,274,
81,176,82,83,37,38,39,40,41,275,136,- 289,0,0,0,0,177,0,43,277,0,0,0,0,179,0,0,
180,0,0,181,182,0,0,183,184,2,3,4,74,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,
726,727,21,162,163,262,0,263,264,265,266,267,268,269,270,22,78,23,164,271,0,0,
165,24,272,0,0,0,0,0,166,167,168,169,0,27,28,29,170,0,273,171,0,0,0,0,32,33,34,
172,173,35,0,0,0,0,174,175,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,274,81,176,
82,83,37,38,39,40,41,275,136,- 299,0,0,0,0,177,0,43,277,0,0,0,0,179,0,0,180,0,0,
181,182,0,0,183,184,2,3,4,74,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,162,
163,262,0,263,264,265,266,267,268,269,270,22,78,23,164,271,0,0,165,24,272,0,0,0,
0,0,166,167,168,169,0,27,28,29,170,0,273,171,0,0,0,0,32,33,34,172,173,35,0,0,0,0,
174,175,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,274,81,176,82,83,37,38,39,40,
41,275,136,276,0,0,0,0,177,0,43,277,0,0,0,0,179,0,0,180,0,0,181,182,0,0,183,184,
2,3,4,74,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,162,163,262,0,263,264,
265,266,267,268,269,270,22,78,23,164,271,0,0,165,24,272,0,0,0,0,0,166,167,168,
169,0,27,28,29,170,0,273,171,0,0,0,0,32,33,34,172,173,35,0,0,0,0,174,175,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,274,81,176,82,83,37,38,39,40,41,275,136,0,0,0,
0,0,177,0,43,277,0,0,0,0,179,0,0,180,0,0,181,182,0,0,183,184,2,3,4,74,6,7,8,9,10,
11,12,13,14,15,16,17,18,19,20,0,0,21,162,163,0,0,0,0,0,0,0,0,0,0,22,78,23,164,0,
0,0,165,24,0,0,0,0,0,0,166,167,168,169,0,27,28,29,170,0,0,171,0,0,0,0,32,33,34,
172,173,35,0,0,0,0,174,175,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,36,81,176,
82,83,37,38,39,40,41,625,0,0,0,0,0,0,177,0,43,277,0,0,0,0,179,0,0,180,0,0,181,
182,0,0,183,184,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,162,163,0,0,0,0,0,0,
0,0,0,0,22,78,0,164,0,0,0,165,0,0,0,0,0,0,0,166,167,168,169,0,27,28,0,170,0,0,
171,0,0,0,0,32,33,0,172,173,35,0,0,0,0,174,175,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,36,81,176,82,83,37,38,39,40,41,0,335,0,0,0,0,0,177,0,43,277,0,0,0,0,179,
0,0,180,0,0,181,182,0,0,183,184,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,162,
163,0,0,0,0,0,0,0,0,0,0,22,78,0,164,0,0,0,165,0,0,0,0,0,0,0,166,167,168,169,0,27,
28,0,170,0,0,171,0,0,0,0,32,33,0,172,173,35,0,0,0,0,174,175,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,36,81,176,82,83,37,38,39,40,41,0,0,0,0,0,0,0,177,0,43,277,
0,0,0,0,179,0,0,180,0,363,181,182,0,0,183,184,- 11,1,0,2,3,4,5,6,7,8,9,10,11,12,
13,14,15,16,17,18,19,20,0,0,21,364,365,366,367,368,369,370,371,372,373,0,0,22,0,
23,0,0,0,651,0,24,0,25,26,- 146,- 146,- 146,374,0,0,0,0,27,28,29,0,0,0,0,0,0,30,31,
32,33,34,0,0,35,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,36,0,0,0,0,
37,38,39,40,41,0,0,- 11,0,0,0,0,42,0,43,44,0,0,0,0,45,46,47,- 11,1,0,2,3,4,5,6,7,8,
9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,0,23,0,0,0,
0,0,24,0,25,26,- 146,- 146,- 146,0,0,0,0,0,27,28,29,0,0,0,0,0,0,30,31,32,33,34,0,0,
35,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,36,0,0,0,0,37,38,39,40,
41,0,0,- 11,0,0,0,0,42,0,43,44,0,0,0,0,45,46,47,- 11,1,0,2,3,4,5,6,7,8,9,10,11,12,
13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,0,23,0,0,0,0,0,24,0,25,
26,- 146,- 146,- 146,0,0,0,0,0,27,28,29,0,0,0,0,0,0,30,31,32,33,34,0,0,35,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,36,0,0,0,0,37,38,39,40,41,0,0,0,0,0,
0,0,42,0,43,44,0,0,0,0,45,46,47,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,
19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,0,23,0,0,0,0,0,24,0,25,26,- 146,- 146,- 146,
0,0,0,0,0,27,28,29,0,0,0,0,0,0,30,31,32,33,34,0,0,35,7,8,9,10,11,12,13,14,15,16,
17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,36,0,22,0,0,37,38,39,40,41,0,0,- 11,0,0,0,0,
42,0,43,44,0,27,28,0,45,46,47,0,0,0,0,0,32,33,0,0,0,35,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,295,0,0,0,0,0,0,0,37,38,0,40,41,297,0,0,0,0,0,0,405,- 211,
43,44,- 211,0,406,0,45,46,47,2,3,4,74,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,
0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,0,23,0,0,0,0,0,24,0,0,0,0,0,0,0,0,0,0,0,27,28,29,
0,0,0,0,0,0,0,0,32,33,34,0,0,35,0,0,0,0,0,0,0,0,2,3,4,74,6,7,8,9,10,11,12,13,14,
15,16,17,18,19,20,0,0,21,0,0,37,38,0,40,41,- 121,136,0,- 121,249,22,0,23,0,43,44,0,
0,24,0,0,0,0,0,0,0,0,0,0,0,27,28,29,0,0,0,0,0,0,0,0,32,33,34,0,0,35,0,0,0,2,3,4,
74,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,37,38,0,40,41,
22,136,0,0,0,0,0,0,24,43,44,0,- 146,- 146,- 146,0,0,0,0,0,27,28,0,0,0,0,0,0,0,0,0,
32,33,0,0,0,35,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,22,0,0,37,38,0,40,41,0,0,0,0,0,0,0,0,0,43,44,0,27,28,0,0,0,0,0,0,0,0,0,32,33,
0,0,0,35,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,295,0,0,296,
0,22,0,0,37,38,0,40,41,297,0,0,0,0,0,0,0,- 211,43,44,- 211,27,28,0,0,0,0,0,0,0,0,0,
32,33,0,0,0,35,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,295,0,
0,0,0,22,0,0,37,38,0,40,41,297,0,0,0,- 146,- 146,- 146,0,- 211,43,44,- 211,27,28,0,0,
0,0,0,0,0,0,0,32,33,0,0,0,35,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,22,0,0,37,38,0,40,41,0,0,0,0,0,0,0,0,0,43,44,0,27,28,0,0,213,0,
0,0,0,0,0,32,33,0,0,0,35,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,22,0,0,37,38,0,40,41,0,214,0,0,0,0,0,0,0,43,44,0,27,28,0,0,0,0,0,0,
0,0,0,32,33,0,0,0,35,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,22,0,0,37,38,0,40,41,0,0,- 114,0,- 146,- 146,- 146,0,0,43,44,0,27,28,0,0,0,
0,0,0,0,0,0,32,33,0,0,0,35,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,22,0,0,37,0,0,0,41,0,0,0,0,0,0,0,0,0,43,44,0,27,28,0,0,0,0,0,0,0,
0,0,32,33,0,0,0,35,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,22,0,0,37,38,0,40,41,0,0,- 115,0,0,800,801,0,0,43,44,0,27,28,0,0,0,0,78,0,
0,0,0,32,33,0,0,0,35,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,
646,0,0,0,0,22,0,0,37,38,0,40,41,79,0,0,0,0,0,0,0,0,43,44,0,27,28,0,158,81,0,82,
83,0,0,39,32,33,0,0,0,35,0,0,0,802,0,85,86,0,0,0,0,87,0,0,803,0,0,88,182,0,0,183,
184,0,0,0,0,0,0,0,0,0,37,38,0,40,41,0,0,0,0,0,0,0,0,0,43,44,162,163,262,0,263,
264,265,266,267,268,269,270,0,78,0,164,271,0,0,165,0,272,0,0,0,0,0,166,167,168,
169,0,0,0,0,170,0,273,171,0,0,0,0,0,0,0,172,173,0,0,0,0,0,174,175,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,274,81,176,82,83,0,0,39,0,0,275,136,0,0,162,163,0,
177,0,0,178,512,0,0,0,179,0,78,180,164,0,181,182,165,0,183,184,0,0,0,0,166,167,
168,169,0,0,0,0,170,0,0,171,0,0,0,0,0,0,0,172,173,0,0,0,0,0,174,175,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,36,81,176,82,83,0,0,39,0,0,0,321,513,0,162,163,0,
177,0,0,178,0,514,515,0,179,0,78,180,164,0,181,182,165,0,183,184,0,0,0,0,166,167,
168,169,0,0,0,0,170,0,0,171,0,0,0,0,0,0,0,172,173,0,0,0,0,0,174,175,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,36,81,176,82,83,0,0,39,0,0,0,321,755,0,162,163,0,
177,0,0,178,0,514,515,0,179,0,78,180,164,0,181,182,165,0,183,184,0,0,0,0,166,167,
168,169,0,0,0,0,170,0,0,171,0,0,0,0,0,0,0,172,173,0,0,0,0,0,174,175,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,36,81,176,82,83,0,0,39,0,0,0,321,919,0,162,163,0,
177,0,0,178,0,514,515,0,179,0,78,180,164,0,181,182,165,0,183,184,0,0,0,0,166,167,
168,169,0,0,0,0,170,0,0,171,0,0,0,0,0,0,0,172,173,0,0,0,0,0,174,175,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,36,81,176,82,83,0,0,39,0,0,0,321,0,0,162,163,0,177,
78,0,178,0,514,515,0,179,0,78,180,164,0,181,182,165,0,183,184,0,0,0,0,166,167,
168,169,0,0,0,0,170,0,0,171,0,0,0,0,0,0,0,172,173,0,0,0,0,79,174,175,0,0,0,0,0,0,
0,0,0,386,0,0,158,81,0,82,83,0,0,39,0,36,81,176,82,83,0,0,39,84,0,85,86,0,0,162,
163,87,177,0,0,178,0,88,0,302,179,0,78,180,164,0,181,182,165,0,183,184,0,0,0,0,
166,167,168,169,0,0,0,0,170,0,0,171,0,0,0,0,0,0,0,172,173,0,0,0,0,0,174,175,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,36,81,176,82,83,0,0,39,0,0,0,321,0,0,162,
163,0,177,0,0,178,0,0,0,0,179,0,78,180,164,0,181,182,165,0,183,184,0,0,0,0,166,
167,168,169,0,0,0,0,170,0,0,171,0,0,0,0,0,0,0,172,173,0,0,0,0,0,174,175,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,36,81,176,82,83,0,0,39,0,0,469,162,163,0,0,0,0,
177,0,0,178,0,0,0,78,179,164,0,180,0,165,181,182,0,0,183,184,0,166,167,168,169,0,
0,0,0,170,0,0,171,0,0,0,0,0,0,0,172,173,0,0,0,0,0,174,175,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,36,81,176,82,83,0,0,39,0,0,0,162,163,0,0,78,0,177,559,0,178,
0,0,0,78,179,164,0,180,0,165,181,182,0,0,183,184,0,166,167,168,169,0,0,0,0,170,0,
0,171,0,0,0,0,0,0,0,172,173,0,0,0,0,79,174,175,0,0,0,0,0,0,0,0,0,683,0,0,158,81,
0,82,83,0,0,39,0,36,81,176,82,83,0,0,39,84,0,85,86,0,0,162,163,87,177,0,0,178,0,
88,0,585,179,0,78,180,164,0,181,182,165,0,183,184,0,0,0,0,166,167,168,169,0,0,0,
0,170,0,0,171,0,0,0,0,0,0,0,172,173,0,0,0,0,0,174,175,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,36,81,176,82,83,0,0,39,0,0,0,162,163,0,0,0,0,177,631,0,178,0,0,0,
78,179,164,0,180,0,165,181,182,0,0,183,184,0,166,167,168,169,0,0,0,0,170,0,0,171,
0,0,0,0,0,0,0,172,173,0,0,0,0,0,174,175,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,36,81,176,82,83,0,0,39,0,0,0,0,0,0,162,163,0,177,0,0,178,0,0,0,698,179,0,78,
180,164,0,181,182,165,0,183,184,0,0,0,0,166,167,168,169,0,0,0,0,170,0,0,171,0,0,
0,0,0,0,0,172,173,0,0,0,0,0,174,175,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,36,
81,176,82,83,0,0,39,0,0,0,0,0,0,162,163,0,177,0,0,178,0,0,0,716,179,0,78,180,164,
0,181,182,165,0,183,184,0,0,0,0,166,167,168,169,0,0,0,0,170,0,0,171,0,0,0,0,0,0,
0,172,173,0,0,0,0,0,174,175,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,36,81,176,
82,83,0,0,39,0,0,731,162,163,0,0,0,0,177,0,0,178,0,0,0,78,179,164,0,180,0,165,
181,182,0,0,183,184,0,166,167,168,169,0,0,0,0,170,0,0,171,0,0,0,0,0,0,0,172,173,
0,0,0,0,0,174,175,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,36,81,176,82,83,0,0,
39,0,0,733,162,163,0,0,0,0,177,0,0,178,0,0,0,78,179,164,0,180,0,165,181,182,0,0,
183,184,0,166,167,168,169,0,0,0,0,170,0,0,171,0,0,0,0,0,0,0,172,173,0,0,0,0,0,
174,175,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,36,81,176,82,83,0,0,39,0,0,0,
162,163,0,0,0,0,177,826,0,178,0,0,0,78,179,164,0,180,0,165,181,182,0,0,183,184,0,
166,167,168,169,0,0,0,0,170,0,0,171,0,0,0,0,0,0,0,172,173,0,0,0,0,0,174,175,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,36,81,176,82,83,0,0,39,0,0,0,162,163,0,0,0,
0,177,829,0,178,0,0,0,78,179,164,0,180,0,165,181,182,0,0,183,184,0,166,167,168,
169,0,0,0,0,170,0,0,171,0,0,0,0,0,0,0,172,173,0,0,0,0,0,174,175,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,36,81,176,82,83,0,0,39,0,0,832,162,163,0,0,0,0,177,0,0,
178,0,0,0,78,179,164,0,180,0,165,181,182,0,0,183,184,0,166,167,168,169,0,0,0,0,
170,0,0,171,0,0,0,0,0,0,0,172,173,0,0,0,0,0,174,175,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,36,81,176,82,83,0,0,39,0,0,0,162,163,0,0,0,0,177,901,0,178,0,0,0,
78,179,164,0,180,0,165,181,182,0,0,183,184,0,166,167,168,169,0,0,0,0,170,0,0,171,
0,0,0,0,0,0,0,172,173,0,0,0,0,0,174,175,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,36,81,176,82,83,0,0,39,0,0,0,162,163,0,0,0,0,177,905,0,178,0,0,0,78,179,164,0,
180,0,165,181,182,0,0,183,184,0,166,167,168,169,0,0,0,0,170,0,0,171,0,0,0,0,0,0,
0,172,173,0,0,0,0,0,174,175,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,36,81,176,
82,83,0,0,39,0,0,0,162,163,0,0,0,0,177,907,0,178,0,0,0,78,179,164,0,180,0,165,
181,182,0,0,183,184,0,166,167,168,169,0,0,0,0,170,0,0,171,0,0,0,0,0,0,0,172,173,
0,0,0,0,0,174,175,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,36,81,176,82,83,0,0,
39,0,0,0,162,163,0,0,0,0,177,963,0,178,0,0,0,78,179,164,0,180,0,165,181,182,0,0,
183,184,0,166,167,168,169,0,0,0,0,170,0,0,171,0,0,0,0,0,162,163,172,173,0,0,0,0,
0,174,175,0,0,78,0,0,0,0,0,0,0,0,0,0,0,0,0,166,167,168,169,0,36,81,176,82,83,0,
171,39,0,0,0,0,0,0,172,173,0,177,0,0,178,174,175,0,0,179,0,0,180,0,0,181,182,0,0,
183,184,0,0,0,0,0,0,0,36,81,176,82,83,0,0,39,0,0,0,321,0,162,163,0,0,177,0,0,178,
0,0,0,0,179,78,0,180,0,0,181,182,0,0,183,184,0,0,0,166,167,168,169,0,0,0,0,0,0,0,
171,0,0,0,0,0,162,163,172,173,0,0,0,0,0,174,175,0,0,78,0,0,0,0,0,0,0,0,0,0,0,0,0,
166,167,168,169,0,36,81,176,82,83,0,171,39,0,0,0,674,0,0,172,173,0,177,0,0,178,
174,175,0,0,179,0,0,180,0,0,181,182,0,0,183,184,0,0,0,0,0,0,0,36,81,176,82,83,0,
0,39,0,0,162,163,0,0,0,0,0,316,0,0,178,0,0,78,0,179,0,0,180,0,0,181,182,0,0,183,
184,166,167,168,169,0,0,0,0,0,0,0,171,0,0,0,0,0,162,163,172,173,0,0,0,0,0,174,
175,0,0,78,0,0,0,0,0,0,0,0,0,0,0,0,0,166,167,168,169,0,36,81,176,82,83,0,171,39,
0,0,0,0,0,0,172,173,0,332,0,0,178,174,175,0,0,179,0,0,180,0,0,181,182,0,0,183,
184,0,0,0,0,0,0,0,36,81,176,82,83,0,0,39,0,0,162,163,0,0,78,0,0,177,0,0,178,0,0,
78,0,179,0,0,180,0,0,181,182,0,0,183,184,166,167,168,169,0,0,0,0,0,0,0,171,0,0,0,
0,0,0,0,172,173,0,0,0,0,79,174,175,0,0,0,0,0,0,0,0,0,771,0,0,158,81,0,82,83,0,0,
39,0,36,81,176,82,83,0,0,39,84,0,85,86,0,514,515,0,87,870,0,0,178,0,88,0,0,179,0,
0,180,0,0,181,182,0,0,183,184};static short Cyc_yycheck[6477]={0,110,165,131,344,
65,142,278,279,377,281,62,0,381,564,347,249,165,135,142,346,21,239,123,164,350,
351,349,380,65,435,150,149,21,87,142,618,356,357,0,393,49,42,171,177,352,353,354,
355,49,5,6,488,74,216,55,56,57,17,59,21,49,412,123,335,65,66,55,56,57,157,59,450,
87,61,128,136,65,138,358,359,486,101,211,179,180,181,123,49,108,495,112,100,101,
55,56,57,214,59,118,100,101,138,736,65,200,99,115,116,117,118,101,100,101,128,
115,116,117,118,106,76,473,114,123,674,112,116,115,116,117,118,103,21,125,127,
123,136,515,138,100,101,577,101,493,152,145,514,23,136,106,138,119,152,218,115,
116,117,118,113,115,81,129,123,120,152,798,55,56,57,110,59,311,82,83,239,136,65,
138,431,116,297,106,221,316,81,318,114,128,239,228,250,152,101,300,122,120,117,
81,331,332,407,142,455,79,255,496,116,117,129,96,97,428,88,795,250,102,216,114,
278,279,117,281,309,129,431,119,116,133,216,106,114,101,449,117,123,129,869,615,
400,401,402,111,875,120,114,129,116,136,42,138,250,114,111,114,111,294,116,385,
38,122,54,122,250,82,83,360,361,362,307,621,101,68,114,101,215,335,217,108,278,
279,122,281,129,116,160,518,133,655,116,250,278,279,583,281,101,412,101,116,117,
932,118,108,107,108,109,305,86,660,23,112,450,118,253,107,121,118,101,278,279,
106,281,101,102,108,104,105,113,112,108,205,458,112,111,100,687,335,212,106,118,
118,120,121,747,265,113,139,126,335,981,271,114,101,132,111,112,119,116,107,108,
109,101,250,995,123,79,118,101,476,118,100,111,488,107,88,335,515,493,428,119,
479,431,122,118,600,111,112,114,126,124,278,279,119,281,526,611,134,135,118,449,
115,400,401,402,116,455,465,697,123,124,409,101,102,400,401,402,114,107,770,112,
479,119,218,111,714,111,114,110,660,428,720,430,439,116,441,118,111,531,103,114,
439,124,441,239,479,563,160,335,84,85,449,741,439,302,441,248,746,143,112,255,
118,577,101,126,111,405,465,589,0,604,588,134,135,114,111,115,615,114,465,439,
479,441,604,589,106,603,106,280,428,616,22,23,479,25,110,435,503,118,604,111,116,
853,118,114,503,465,668,114,124,395,42,84,119,500,119,390,503,49,393,479,75,668,
54,101,102,757,642,64,65,107,62,104,638,419,114,67,476,876,770,678,646,113,111,
503,76,114,486,79,879,880,600,115,822,878,475,495,88,595,882,883,881,611,911,249,
614,80,81,114,100,101,888,889,119,113,114,107,635,465,884,885,886,887,118,115,
116,117,118,110,587,101,118,479,118,116,721,118,108,483,110,118,635,124,22,23,
177,875,118,139,607,405,977,101,890,891,126,127,128,107,108,109,152,853,912,113,
111,157,114,114,160,114,130,119,428,114,119,321,635,210,119,325,326,327,114,681,
113,114,653,119,635,589,499,118,338,449,653,118,342,695,114,118,600,118,649,119,
604,118,653,77,78,79,802,113,114,205,712,635,390,363,118,393,212,49,50,51,124,
114,126,127,128,375,119,566,86,653,380,570,114,270,101,749,111,119,132,114,107,
108,109,239,118,803,131,981,114,123,124,118,248,119,850,111,406,124,114,255,114,
114,101,821,260,119,119,263,107,108,109,841,585,112,870,101,872,111,101,316,114,
114,108,635,280,111,119,324,112,639,101,328,118,330,111,332,118,114,111,822,126,
127,128,162,114,164,165,114,345,119,5,6,119,309,114,174,175,131,132,119,179,180,
181,473,114,101,476,114,114,119,101,911,119,119,892,893,894,108,723,111,600,200,
114,747,729,382,111,118,687,114,111,611,875,117,614,126,127,128,117,822,126,127,
128,17,18,19,84,85,518,80,81,822,119,931,970,113,114,980,101,102,103,694,400,401,
402,698,113,726,566,113,794,113,570,113,390,924,925,393,822,118,972,915,118,716,
118,101,111,111,118,122,118,113,875,101,564,115,119,129,125,110,119,122,459,460,
875,117,463,113,465,113,117,87,428,118,116,431,119,474,119,114,590,826,119,439,
829,441,114,113,118,101,101,875,311,449,118,605,119,114,981,455,122,111,30,118,
112,114,779,112,111,42,822,621,118,114,101,119,129,119,114,114,793,114,867,113,
113,344,114,346,347,348,349,350,351,352,353,354,355,356,357,358,359,360,361,362,
119,119,119,503,119,687,660,114,106,119,119,900,901,667,981,904,905,125,907,875,
674,76,115,677,119,119,981,118,8,9,10,11,12,13,14,15,16,17,18,19,20,21,112,111,
726,119,111,113,117,112,112,25,593,981,125,101,116,37,119,112,119,107,108,109,
972,119,566,119,957,125,570,960,118,962,963,111,122,57,58,119,126,127,128,125,
625,626,119,450,68,69,119,119,119,73,112,119,122,986,113,111,600,111,106,757,102,
119,119,118,114,28,119,611,119,112,614,118,770,617,98,86,122,101,130,86,665,981,
106,107,108,109,110,111,113,25,119,114,113,38,118,119,120,121,122,114,124,113,
126,127,128,101,111,113,515,653,101,107,108,109,113,119,107,108,109,704,118,102,
118,118,531,113,119,118,124,114,126,127,128,119,0,126,127,128,0,119,38,86,119,
687,119,730,731,119,733,119,735,599,915,98,109,853,101,102,794,104,105,248,253,
108,614,67,46,649,753,646,911,212,642,118,519,120,121,687,123,124,348,126,726,
726,62,- 1,- 1,132,86,8,9,10,11,12,13,14,15,16,17,18,19,20,21,101,102,749,104,105,
615,- 1,108,- 1,- 1,- 1,- 1,- 1,- 1,802,37,- 1,118,- 1,120,121,- 1,- 1,- 1,- 1,126,- 1,49,50,
51,- 1,132,- 1,- 1,- 1,57,58,- 1,- 1,- 1,828,- 1,- 1,831,832,- 1,68,69,101,- 1,- 1,73,- 1,- 1,
107,108,109,110,668,- 1,- 1,- 1,- 1,- 1,- 1,118,- 1,- 1,678,- 1,- 1,- 1,- 1,126,127,128,- 1,-
1,- 1,101,- 1,- 1,870,- 1,106,107,108,109,110,877,- 1,- 1,- 1,- 1,- 1,- 1,118,- 1,120,121,-
1,- 1,- 1,- 1,126,127,128,895,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,865,- 1,- 1,909,- 1,- 1,- 1,
- 1,- 1,- 1,916,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,
- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,915,- 1,44,45,46,- 1,- 1,- 1,- 1,- 1,52,53,
54,55,- 1,57,58,59,60,- 1,62,63,800,- 1,- 1,803,68,69,70,71,72,73,- 1,- 1,- 1,- 1,78,79,
- 1,- 1,- 1,- 1,- 1,821,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,101,102,103,104,
105,106,107,108,109,110,111,112,113,- 1,- 1,- 1,- 1,118,- 1,120,121,- 1,- 1,- 1,- 1,126,-
1,- 1,129,- 1,- 1,132,133,- 1,- 1,136,137,- 1,- 1,876,- 1,878,879,880,881,882,883,884,
885,886,887,888,889,890,891,892,893,894,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,3,4,5,6,
7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,931,29,30,31,32,33,
34,35,36,37,38,39,40,41,- 1,- 1,44,45,46,- 1,- 1,- 1,- 1,- 1,52,53,54,55,- 1,57,58,59,60,
- 1,62,63,- 1,- 1,- 1,- 1,68,69,70,71,72,73,- 1,- 1,- 1,980,78,79,- 1,- 1,- 1,- 1,- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,101,102,103,104,105,106,107,108,109,110,
111,112,113,- 1,- 1,- 1,- 1,118,- 1,120,121,- 1,- 1,- 1,- 1,126,- 1,- 1,129,- 1,- 1,132,133,-
1,- 1,136,137,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,
- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,- 1,- 1,44,45,46,- 1,- 1,- 1,- 1,- 1,52,53,54,
55,- 1,57,58,59,60,- 1,62,63,- 1,- 1,- 1,- 1,68,69,70,71,72,73,- 1,- 1,- 1,- 1,78,79,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,101,102,103,104,105,106,
107,108,109,110,111,112,113,- 1,- 1,- 1,- 1,118,- 1,120,121,- 1,- 1,- 1,- 1,126,- 1,- 1,129,
- 1,- 1,132,133,- 1,- 1,136,137,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,
23,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,- 1,- 1,44,45,46,- 1,- 1,- 1,
- 1,- 1,52,53,54,55,- 1,57,58,59,60,- 1,62,63,- 1,- 1,- 1,- 1,68,69,70,71,72,73,- 1,- 1,- 1,
- 1,78,79,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,101,102,
103,104,105,106,107,108,109,110,111,112,113,- 1,- 1,- 1,- 1,118,- 1,120,121,- 1,- 1,- 1,
- 1,126,- 1,- 1,129,- 1,- 1,132,133,- 1,- 1,136,137,3,4,5,6,7,8,9,10,11,12,13,14,15,16,
17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,- 1,- 1,
44,45,46,- 1,- 1,- 1,- 1,- 1,52,53,54,55,- 1,57,58,59,60,- 1,62,63,- 1,- 1,- 1,- 1,68,69,70,
71,72,73,- 1,- 1,- 1,- 1,78,79,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,101,102,103,104,105,106,107,108,109,110,111,112,113,- 1,- 1,- 1,- 1,118,- 1,
120,121,- 1,- 1,- 1,- 1,126,- 1,- 1,129,- 1,- 1,132,133,- 1,- 1,136,137,3,4,5,6,7,8,9,10,
11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,
38,39,40,41,- 1,- 1,44,45,46,- 1,- 1,- 1,- 1,- 1,52,53,54,55,- 1,57,58,59,60,- 1,62,63,- 1,
- 1,- 1,- 1,68,69,70,71,72,73,- 1,- 1,- 1,- 1,78,79,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,101,102,103,104,105,106,107,108,109,110,111,112,- 1,- 1,
- 1,- 1,- 1,118,- 1,120,121,- 1,- 1,- 1,- 1,126,- 1,- 1,129,- 1,- 1,132,133,- 1,- 1,136,137,3,
4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,37,38,39,40,- 1,- 1,- 1,44,45,- 1,- 1,- 1,- 1,- 1,- 1,52,53,54,55,- 1,57,58,59,
60,- 1,- 1,63,- 1,- 1,- 1,- 1,68,69,70,71,72,73,- 1,- 1,- 1,- 1,78,79,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,101,102,103,104,105,106,107,108,109,
110,111,- 1,- 1,- 1,- 1,- 1,- 1,118,- 1,120,121,- 1,- 1,- 1,- 1,126,- 1,- 1,129,- 1,- 1,132,133,
- 1,- 1,136,137,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,37,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,- 1,- 1,- 1,52,53,54,55,- 1,57,
58,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,68,69,- 1,71,72,73,- 1,- 1,- 1,- 1,78,79,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,101,102,103,104,105,106,107,108,
109,110,- 1,112,- 1,- 1,- 1,- 1,- 1,118,- 1,120,121,- 1,- 1,- 1,- 1,126,- 1,- 1,129,- 1,- 1,132,
133,- 1,- 1,136,137,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,25,26,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,- 1,- 1,- 1,52,53,54,55,
- 1,57,58,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,68,69,- 1,71,72,73,- 1,- 1,- 1,- 1,78,79,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,101,102,103,104,105,106,
107,108,109,110,- 1,- 1,- 1,- 1,- 1,- 1,- 1,118,- 1,120,121,- 1,- 1,- 1,- 1,126,- 1,- 1,129,- 1,
56,132,133,- 1,- 1,136,137,0,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,
21,- 1,- 1,24,88,89,90,91,92,93,94,95,96,97,- 1,- 1,37,- 1,39,- 1,- 1,- 1,43,- 1,45,- 1,47,
48,49,50,51,115,- 1,- 1,- 1,- 1,57,58,59,- 1,- 1,- 1,- 1,- 1,- 1,66,67,68,69,70,- 1,- 1,73,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
101,- 1,- 1,- 1,- 1,106,107,108,109,110,- 1,- 1,113,- 1,- 1,- 1,- 1,118,- 1,120,121,- 1,- 1,-
1,- 1,126,127,128,0,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,
24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,39,- 1,- 1,- 1,- 1,- 1,45,- 1,47,48,49,50,
51,- 1,- 1,- 1,- 1,- 1,57,58,59,- 1,- 1,- 1,- 1,- 1,- 1,66,67,68,69,70,- 1,- 1,73,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,101,- 1,- 1,-
1,- 1,106,107,108,109,110,- 1,- 1,113,- 1,- 1,- 1,- 1,118,- 1,120,121,- 1,- 1,- 1,- 1,126,
127,128,0,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,39,- 1,- 1,- 1,- 1,- 1,45,- 1,47,48,49,50,51,- 1,- 1,
- 1,- 1,- 1,57,58,59,- 1,- 1,- 1,- 1,- 1,- 1,66,67,68,69,70,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,101,- 1,- 1,- 1,- 1,106,
107,108,109,110,- 1,- 1,- 1,- 1,- 1,- 1,- 1,118,- 1,120,121,- 1,- 1,- 1,- 1,126,127,128,1,- 1,
3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,37,- 1,39,- 1,- 1,- 1,- 1,- 1,45,- 1,47,48,49,50,51,- 1,- 1,- 1,- 1,- 1,57,58,
59,- 1,- 1,- 1,- 1,- 1,- 1,66,67,68,69,70,- 1,- 1,73,8,9,10,11,12,13,14,15,16,17,18,19,
20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,101,- 1,37,- 1,- 1,106,107,108,109,110,
- 1,- 1,113,- 1,- 1,- 1,- 1,118,- 1,120,121,- 1,57,58,- 1,126,127,128,- 1,- 1,- 1,- 1,- 1,68,
69,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,98,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,- 1,109,110,111,- 1,- 1,- 1,- 1,- 1,- 1,118,119,
120,121,122,- 1,124,- 1,126,127,128,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,
21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,39,- 1,- 1,- 1,- 1,- 1,45,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,57,58,59,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,69,70,- 1,- 1,73,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,
- 1,- 1,106,107,- 1,109,110,111,112,- 1,114,115,37,- 1,39,- 1,120,121,- 1,- 1,45,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,57,58,59,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,69,70,- 1,- 1,73,- 1,
- 1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,106,107,- 1,109,110,37,112,- 1,- 1,- 1,- 1,- 1,- 1,45,120,121,- 1,49,50,51,- 1,- 1,-
1,- 1,- 1,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,69,- 1,- 1,- 1,73,8,9,10,11,12,13,14,15,
16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,- 1,106,107,
- 1,109,110,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,120,121,- 1,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,-
1,68,69,- 1,- 1,- 1,73,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,98,- 1,- 1,101,- 1,37,- 1,- 1,106,107,- 1,109,110,111,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
119,120,121,122,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,69,- 1,- 1,- 1,73,8,9,10,11,12,
13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,98,- 1,- 1,- 1,- 1,37,- 1,- 1,
106,107,- 1,109,110,111,- 1,- 1,- 1,49,50,51,- 1,119,120,121,122,57,58,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,68,69,- 1,- 1,- 1,73,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,- 1,106,107,- 1,109,110,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,120,121,- 1,57,58,- 1,- 1,61,- 1,- 1,- 1,- 1,- 1,- 1,68,69,- 1,- 1,- 1,73,8,9,10,11,
12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,
- 1,106,107,- 1,109,110,- 1,112,- 1,- 1,- 1,- 1,- 1,- 1,- 1,120,121,- 1,57,58,- 1,- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,68,69,- 1,- 1,- 1,73,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,- 1,106,107,- 1,109,110,- 1,- 1,113,- 1,49,
50,51,- 1,- 1,120,121,- 1,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,69,- 1,- 1,- 1,73,8,9,10,
11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,
- 1,- 1,106,- 1,- 1,- 1,110,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,120,121,- 1,57,58,- 1,- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,68,69,- 1,- 1,- 1,73,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,- 1,106,107,- 1,109,110,- 1,- 1,113,- 1,- 1,
25,26,- 1,- 1,120,121,- 1,57,58,- 1,- 1,- 1,- 1,38,- 1,- 1,- 1,- 1,68,69,- 1,- 1,- 1,73,8,9,10,
11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,98,- 1,- 1,- 1,- 1,37,
- 1,- 1,106,107,- 1,109,110,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,120,121,- 1,57,58,- 1,101,102,
- 1,104,105,- 1,- 1,108,68,69,- 1,- 1,- 1,73,- 1,- 1,- 1,118,- 1,120,121,- 1,- 1,- 1,- 1,126,-
1,- 1,129,- 1,- 1,132,133,- 1,- 1,136,137,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,- 1,109,
110,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,120,121,25,26,27,- 1,29,30,31,32,33,34,35,36,- 1,38,
- 1,40,41,- 1,- 1,44,- 1,46,- 1,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,62,63,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,71,72,- 1,- 1,- 1,- 1,- 1,78,79,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,101,102,103,104,105,- 1,- 1,108,- 1,- 1,111,112,- 1,- 1,25,26,
- 1,118,- 1,- 1,121,32,- 1,- 1,- 1,126,- 1,38,129,40,- 1,132,133,44,- 1,136,137,- 1,- 1,- 1,
- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,71,72,- 1,- 1,- 1,- 1,- 1,
78,79,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,101,102,103,
104,105,- 1,- 1,108,- 1,- 1,- 1,112,113,- 1,25,26,- 1,118,- 1,- 1,121,- 1,123,124,- 1,126,-
1,38,129,40,- 1,132,133,44,- 1,136,137,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,-
1,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,71,72,- 1,- 1,- 1,- 1,- 1,78,79,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,101,102,103,104,105,- 1,- 1,108,- 1,- 1,- 1,112,
113,- 1,25,26,- 1,118,- 1,- 1,121,- 1,123,124,- 1,126,- 1,38,129,40,- 1,132,133,44,- 1,
136,137,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,71,
72,- 1,- 1,- 1,- 1,- 1,78,79,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,101,102,103,104,105,- 1,- 1,108,- 1,- 1,- 1,112,113,- 1,25,26,- 1,118,- 1,- 1,121,-
1,123,124,- 1,126,- 1,38,129,40,- 1,132,133,44,- 1,136,137,- 1,- 1,- 1,- 1,52,53,54,55,-
1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,71,72,- 1,- 1,- 1,- 1,- 1,78,79,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,101,102,103,104,105,- 1,- 1,
108,- 1,- 1,- 1,112,- 1,- 1,25,26,- 1,118,38,- 1,121,- 1,123,124,- 1,126,- 1,38,129,40,- 1,
132,133,44,- 1,136,137,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,- 1,- 1,-
1,- 1,- 1,- 1,71,72,- 1,- 1,- 1,- 1,86,78,79,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,98,- 1,- 1,101,
102,- 1,104,105,- 1,- 1,108,- 1,101,102,103,104,105,- 1,- 1,108,118,- 1,120,121,- 1,- 1,
25,26,126,118,- 1,- 1,121,- 1,132,- 1,125,126,- 1,38,129,40,- 1,132,133,44,- 1,136,137,
- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,71,72,- 1,- 1,
- 1,- 1,- 1,78,79,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
101,102,103,104,105,- 1,- 1,108,- 1,- 1,- 1,112,- 1,- 1,25,26,- 1,118,- 1,- 1,121,- 1,- 1,- 1,
- 1,126,- 1,38,129,40,- 1,132,133,44,- 1,136,137,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,
60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,71,72,- 1,- 1,- 1,- 1,- 1,78,79,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,101,102,103,104,105,- 1,- 1,108,- 1,- 1,
111,25,26,- 1,- 1,- 1,- 1,118,- 1,- 1,121,- 1,- 1,- 1,38,126,40,- 1,129,- 1,44,132,133,- 1,-
1,136,137,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,71,72,- 1,-
1,- 1,- 1,- 1,78,79,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
101,102,103,104,105,- 1,- 1,108,- 1,- 1,- 1,25,26,- 1,- 1,38,- 1,118,119,- 1,121,- 1,- 1,- 1,
38,126,40,- 1,129,- 1,44,132,133,- 1,- 1,136,137,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,
63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,71,72,- 1,- 1,- 1,- 1,86,78,79,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,98,
- 1,- 1,101,102,- 1,104,105,- 1,- 1,108,- 1,101,102,103,104,105,- 1,- 1,108,118,- 1,120,
121,- 1,- 1,25,26,126,118,- 1,- 1,121,- 1,132,- 1,125,126,- 1,38,129,40,- 1,132,133,44,-
1,136,137,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
71,72,- 1,- 1,- 1,- 1,- 1,78,79,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,101,102,103,104,105,- 1,- 1,108,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,118,119,- 1,121,
- 1,- 1,- 1,38,126,40,- 1,129,- 1,44,132,133,- 1,- 1,136,137,- 1,52,53,54,55,- 1,- 1,- 1,- 1,
60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,71,72,- 1,- 1,- 1,- 1,- 1,78,79,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,101,102,103,104,105,- 1,- 1,108,- 1,- 1,- 1,
- 1,- 1,- 1,25,26,- 1,118,- 1,- 1,121,- 1,- 1,- 1,125,126,- 1,38,129,40,- 1,132,133,44,- 1,
136,137,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,71,
72,- 1,- 1,- 1,- 1,- 1,78,79,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,101,102,103,104,105,- 1,- 1,108,- 1,- 1,- 1,- 1,- 1,- 1,25,26,- 1,118,- 1,- 1,121,- 1,
- 1,- 1,125,126,- 1,38,129,40,- 1,132,133,44,- 1,136,137,- 1,- 1,- 1,- 1,52,53,54,55,- 1,-
1,- 1,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,71,72,- 1,- 1,- 1,- 1,- 1,78,79,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,101,102,103,104,105,- 1,- 1,108,
- 1,- 1,111,25,26,- 1,- 1,- 1,- 1,118,- 1,- 1,121,- 1,- 1,- 1,38,126,40,- 1,129,- 1,44,132,
133,- 1,- 1,136,137,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,71,
72,- 1,- 1,- 1,- 1,- 1,78,79,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,101,102,103,104,105,- 1,- 1,108,- 1,- 1,111,25,26,- 1,- 1,- 1,- 1,118,- 1,- 1,121,- 1,
- 1,- 1,38,126,40,- 1,129,- 1,44,132,133,- 1,- 1,136,137,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,
- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,71,72,- 1,- 1,- 1,- 1,- 1,78,79,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,101,102,103,104,105,- 1,- 1,108,- 1,- 1,- 1,25,
26,- 1,- 1,- 1,- 1,118,119,- 1,121,- 1,- 1,- 1,38,126,40,- 1,129,- 1,44,132,133,- 1,- 1,136,
137,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,71,72,- 1,- 1,- 1,-
1,- 1,78,79,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,101,
102,103,104,105,- 1,- 1,108,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,118,119,- 1,121,- 1,- 1,- 1,38,
126,40,- 1,129,- 1,44,132,133,- 1,- 1,136,137,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,71,72,- 1,- 1,- 1,- 1,- 1,78,79,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,101,102,103,104,105,- 1,- 1,108,- 1,- 1,111,25,26,- 1,-
1,- 1,- 1,118,- 1,- 1,121,- 1,- 1,- 1,38,126,40,- 1,129,- 1,44,132,133,- 1,- 1,136,137,- 1,
52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,71,72,- 1,- 1,- 1,- 1,- 1,78,
79,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,101,102,103,
104,105,- 1,- 1,108,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,118,119,- 1,121,- 1,- 1,- 1,38,126,40,-
1,129,- 1,44,132,133,- 1,- 1,136,137,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,- 1,-
1,- 1,- 1,- 1,- 1,71,72,- 1,- 1,- 1,- 1,- 1,78,79,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,101,102,103,104,105,- 1,- 1,108,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,
118,119,- 1,121,- 1,- 1,- 1,38,126,40,- 1,129,- 1,44,132,133,- 1,- 1,136,137,- 1,52,53,54,
55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,71,72,- 1,- 1,- 1,- 1,- 1,78,79,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,101,102,103,104,105,- 1,
- 1,108,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,118,119,- 1,121,- 1,- 1,- 1,38,126,40,- 1,129,- 1,44,
132,133,- 1,- 1,136,137,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,-
1,71,72,- 1,- 1,- 1,- 1,- 1,78,79,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,101,102,103,104,105,- 1,- 1,108,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,118,119,- 1,
121,- 1,- 1,- 1,38,126,40,- 1,129,- 1,44,132,133,- 1,- 1,136,137,- 1,52,53,54,55,- 1,- 1,-
1,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,25,26,71,72,- 1,- 1,- 1,- 1,- 1,78,79,- 1,- 1,38,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,52,53,54,55,- 1,101,102,103,104,105,- 1,63,108,- 1,
- 1,- 1,- 1,- 1,- 1,71,72,- 1,118,- 1,- 1,121,78,79,- 1,- 1,126,- 1,- 1,129,- 1,- 1,132,133,- 1,
- 1,136,137,- 1,- 1,- 1,- 1,- 1,- 1,- 1,101,102,103,104,105,- 1,- 1,108,- 1,- 1,- 1,112,- 1,25,
26,- 1,- 1,118,- 1,- 1,121,- 1,- 1,- 1,- 1,126,38,- 1,129,- 1,- 1,132,133,- 1,- 1,136,137,- 1,
- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,- 1,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,25,26,71,72,- 1,- 1,- 1,- 1,
- 1,78,79,- 1,- 1,38,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,52,53,54,55,- 1,101,102,
103,104,105,- 1,63,108,- 1,- 1,- 1,112,- 1,- 1,71,72,- 1,118,- 1,- 1,121,78,79,- 1,- 1,126,
- 1,- 1,129,- 1,- 1,132,133,- 1,- 1,136,137,- 1,- 1,- 1,- 1,- 1,- 1,- 1,101,102,103,104,105,-
1,- 1,108,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,118,- 1,- 1,121,- 1,- 1,38,- 1,126,- 1,- 1,129,- 1,-
1,132,133,- 1,- 1,136,137,52,53,54,55,- 1,- 1,- 1,- 1,- 1,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,25,26,
71,72,- 1,- 1,- 1,- 1,- 1,78,79,- 1,- 1,38,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,52,53,
54,55,- 1,101,102,103,104,105,- 1,63,108,- 1,- 1,- 1,- 1,- 1,- 1,71,72,- 1,118,- 1,- 1,121,
78,79,- 1,- 1,126,- 1,- 1,129,- 1,- 1,132,133,- 1,- 1,136,137,- 1,- 1,- 1,- 1,- 1,- 1,- 1,101,
102,103,104,105,- 1,- 1,108,- 1,- 1,25,26,- 1,- 1,38,- 1,- 1,118,- 1,- 1,121,- 1,- 1,38,- 1,
126,- 1,- 1,129,- 1,- 1,132,133,- 1,- 1,136,137,52,53,54,55,- 1,- 1,- 1,- 1,- 1,- 1,- 1,63,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,71,72,- 1,- 1,- 1,- 1,86,78,79,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,98,- 1,- 1,
101,102,- 1,104,105,- 1,- 1,108,- 1,101,102,103,104,105,- 1,- 1,108,118,- 1,120,121,- 1,
123,124,- 1,126,118,- 1,- 1,121,- 1,132,- 1,- 1,126,- 1,- 1,129,- 1,- 1,132,133,- 1,- 1,136,
137};char Cyc_Yyimpossible[17]="\000\000\000\000Yyimpossible\000";char Cyc_Yystack_overflow[
21]="\000\000\000\000Yystack_overflow\000";void Cyc_yyerror(struct _dyneither_ptr);
int Cyc_yylex(struct Cyc_Lexing_lexbuf*);static int Cyc_yychar=(int)'\000';union Cyc_YYSTYPE_union
Cyc_yylval=(union Cyc_YYSTYPE_union)((struct Cyc_YYINITIALSVAL_struct){57});static
int Cyc_yynerrs=0;struct _tuple18{struct Cyc_Position_Segment*f1;struct _tuple1*f2;
int f3;};struct _tuple19{struct _dyneither_ptr f1;void*f2;};static char _tmp462[8]="stdcall";
static char _tmp463[6]="cdecl";static char _tmp464[9]="fastcall";static char _tmp465[9]="noreturn";
static char _tmp466[6]="const";static char _tmp467[8]="aligned";static char _tmp468[7]="packed";
static char _tmp469[7]="shared";static char _tmp46A[7]="unused";static char _tmp46B[5]="weak";
static char _tmp46C[10]="dllimport";static char _tmp46D[10]="dllexport";static char
_tmp46E[23]="no_instrument_function";static char _tmp46F[12]="constructor";static
char _tmp470[11]="destructor";static char _tmp471[22]="no_check_memory_usage";
static char _tmp472[5]="pure";struct _tuple20{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*
f2;};int Cyc_yyparse(struct Cyc_Lexing_lexbuf*yylex_buf){struct _RegionHandle
_tmp387=_new_region("yyregion");struct _RegionHandle*yyregion=& _tmp387;
_push_region(yyregion);{int yystate;int yyn=0;int yyerrstatus;int yychar1=0;int
yyssp_offset;short*yyss=(short*)({unsigned int _tmp8C1=10000;short*_tmp8C2=(short*)
_region_malloc(yyregion,_check_times(sizeof(short),_tmp8C1));{unsigned int
_tmp8C3=_tmp8C1;unsigned int i;for(i=0;i < _tmp8C3;i ++){_tmp8C2[i]=(short)0;}}
_tmp8C2;});int yyvsp_offset;union Cyc_YYSTYPE_union*yyvs=(union Cyc_YYSTYPE_union*)({
unsigned int _tmp8BE=10000;union Cyc_YYSTYPE_union*_tmp8BF=(union Cyc_YYSTYPE_union*)
_region_malloc(yyregion,_check_times(sizeof(union Cyc_YYSTYPE_union),_tmp8BE));{
unsigned int _tmp8C0=_tmp8BE;unsigned int i;for(i=0;i < _tmp8C0;i ++){_tmp8BF[i]=Cyc_yylval;}}
_tmp8BF;});int yylsp_offset;struct Cyc_Yyltype*yyls=(struct Cyc_Yyltype*)({
unsigned int _tmp8BB=10000;struct Cyc_Yyltype*_tmp8BC=(struct Cyc_Yyltype*)
_region_malloc(yyregion,_check_times(sizeof(struct Cyc_Yyltype),_tmp8BB));{
unsigned int _tmp8BD=_tmp8BB;unsigned int i;for(i=0;i < _tmp8BD;i ++){_tmp8BC[i]=Cyc_yynewloc();}}
_tmp8BC;});int yystacksize=10000;union Cyc_YYSTYPE_union yyval=Cyc_yylval;int yylen;
yystate=0;yyerrstatus=0;Cyc_yynerrs=0;Cyc_yychar=- 2;yyssp_offset=- 1;yyvsp_offset=
0;yylsp_offset=0;yynewstate: yyss[_check_known_subscript_notnull(10000,++
yyssp_offset)]=(short)yystate;if(yyssp_offset >= yystacksize - 1){if(yystacksize >= 
10000){Cyc_yyerror(({const char*_tmp388="parser stack overflow";_tag_dyneither(
_tmp388,sizeof(char),22);}));(int)_throw((void*)Cyc_Yystack_overflow);}}goto
yybackup;yybackup: yyn=(int)Cyc_yypact[_check_known_subscript_notnull(1007,
yystate)];if(yyn == - 32768)goto yydefault;if(Cyc_yychar == - 2)Cyc_yychar=Cyc_yylex(
yylex_buf);if(Cyc_yychar <= 0){yychar1=0;Cyc_yychar=0;}else{yychar1=Cyc_yychar > 0
 && Cyc_yychar <= 365?(int)Cyc_yytranslate[_check_known_subscript_notnull(366,Cyc_yychar)]:
283;}yyn +=yychar1;if((yyn < 0  || yyn > 6476) || Cyc_yycheck[
_check_known_subscript_notnull(6477,yyn)]!= yychar1)goto yydefault;yyn=(int)Cyc_yytable[
_check_known_subscript_notnull(6477,yyn)];if(yyn < 0){if(yyn == - 32768)goto
yyerrlab;yyn=- yyn;goto yyreduce;}else{if(yyn == 0)goto yyerrlab;}if(yyn == 1006){int
_tmp389=0;_npop_handler(0);return _tmp389;}if(Cyc_yychar != 0)Cyc_yychar=- 2;yyvs[
_check_known_subscript_notnull(10000,++ yyvsp_offset)]=Cyc_yylval;yyls[
_check_known_subscript_notnull(10000,++ yylsp_offset)]=Cyc_yylloc;if(yyerrstatus
!= 0)-- yyerrstatus;yystate=yyn;goto yynewstate;yydefault: yyn=(int)Cyc_yydefact[
_check_known_subscript_notnull(1007,yystate)];if(yyn == 0)goto yyerrlab;yyreduce:
yylen=(int)Cyc_yyr2[_check_known_subscript_notnull(506,yyn)];if(yylen > 0)yyval=
yyvs[_check_known_subscript_notnull(10000,(yyvsp_offset + 1)- yylen)];switch(yyn){
case 1: _LL1E8: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];Cyc_Parse_parse_result=
Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);break;
case 2: _LL1E9: {struct Cyc_List_List*x;struct Cyc_List_List*y;{struct _handler_cons
_tmp38A;_push_handler(& _tmp38A);{int _tmp38C=0;if(setjmp(_tmp38A.handler))_tmp38C=
1;if(!_tmp38C){x=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);;_pop_handler();}else{void*_tmp38B=(void*)_exn_thrown;void*
_tmp38E=_tmp38B;_LL1EC: if(*((void**)_tmp38E)!= Cyc_Core_Failure)goto _LL1EE;
_LL1ED: x=0;goto _LL1EB;_LL1EE:;_LL1EF:(void)_throw(_tmp38E);_LL1EB:;}}}{struct
_handler_cons _tmp38F;_push_handler(& _tmp38F);{int _tmp391=0;if(setjmp(_tmp38F.handler))
_tmp391=1;if(!_tmp391){y=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);;_pop_handler();}else{void*_tmp390=(void*)_exn_thrown;void*
_tmp393=_tmp390;_LL1F1: if(*((void**)_tmp393)!= Cyc_Core_Failure)goto _LL1F3;
_LL1F2: y=0;goto _LL1F0;_LL1F3:;_LL1F4:(void)_throw(_tmp393);_LL1F0:;}}}yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp394;(_tmp394.YY18).tag=22;(
_tmp394.YY18).f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_imp_append)(x,y);_tmp394;});break;}case 3: _LL1EA: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp395;(_tmp395.YY18).tag=22;(_tmp395.YY18).f1=({struct
Cyc_List_List*_tmp396=_cycalloc(sizeof(*_tmp396));_tmp396->hd=({struct Cyc_Absyn_Decl*
_tmp397=_cycalloc(sizeof(*_tmp397));_tmp397->r=(void*)((void*)({struct Cyc_Absyn_Using_d_struct*
_tmp398=_cycalloc(sizeof(*_tmp398));_tmp398[0]=({struct Cyc_Absyn_Using_d_struct
_tmp399;_tmp399.tag=11;_tmp399.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp399.f2=Cyc_yyget_YY18(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp399;});_tmp398;}));
_tmp397->loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp397;});_tmp396->tl=0;_tmp396;});_tmp395;});Cyc_Lex_leave_using();
break;case 4: _LL1F5: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp39A;(
_tmp39A.YY18).tag=22;(_tmp39A.YY18).f1=({struct Cyc_List_List*_tmp39B=_cycalloc(
sizeof(*_tmp39B));_tmp39B->hd=({struct Cyc_Absyn_Decl*_tmp39C=_cycalloc(sizeof(*
_tmp39C));_tmp39C->r=(void*)((void*)({struct Cyc_Absyn_Using_d_struct*_tmp39D=
_cycalloc(sizeof(*_tmp39D));_tmp39D[0]=({struct Cyc_Absyn_Using_d_struct _tmp39E;
_tmp39E.tag=11;_tmp39E.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp39E.f2=Cyc_yyget_YY18(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp39E;});_tmp39D;}));
_tmp39C->loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line);_tmp39C;});_tmp39B->tl=Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp39B;});_tmp39A;});
break;case 5: _LL1F6: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp39F;(
_tmp39F.YY18).tag=22;(_tmp39F.YY18).f1=({struct Cyc_List_List*_tmp3A0=_cycalloc(
sizeof(*_tmp3A0));_tmp3A0->hd=({struct Cyc_Absyn_Decl*_tmp3A1=_cycalloc(sizeof(*
_tmp3A1));_tmp3A1->r=(void*)((void*)({struct Cyc_Absyn_Namespace_d_struct*_tmp3A2=
_cycalloc(sizeof(*_tmp3A2));_tmp3A2[0]=({struct Cyc_Absyn_Namespace_d_struct
_tmp3A3;_tmp3A3.tag=10;_tmp3A3.f1=({struct _dyneither_ptr*_tmp3A4=_cycalloc(
sizeof(*_tmp3A4));_tmp3A4[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp3A4;});_tmp3A3.f2=
Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp3A3;});
_tmp3A2;}));_tmp3A1->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp3A1;});
_tmp3A0->tl=0;_tmp3A0;});_tmp39F;});Cyc_Lex_leave_namespace();break;case 6: _LL1F7: {
struct _dyneither_ptr nspace;struct Cyc_List_List*x;struct Cyc_List_List*y;{struct
_handler_cons _tmp3A5;_push_handler(& _tmp3A5);{int _tmp3A7=0;if(setjmp(_tmp3A5.handler))
_tmp3A7=1;if(!_tmp3A7){nspace=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);;_pop_handler();}else{
void*_tmp3A6=(void*)_exn_thrown;void*_tmp3A9=_tmp3A6;_LL1FA: if(*((void**)_tmp3A9)
!= Cyc_Core_Failure)goto _LL1FC;_LL1FB: nspace=({const char*_tmp3AA="";
_tag_dyneither(_tmp3AA,sizeof(char),1);});goto _LL1F9;_LL1FC:;_LL1FD:(void)_throw(
_tmp3A9);_LL1F9:;}}}{struct _handler_cons _tmp3AB;_push_handler(& _tmp3AB);{int
_tmp3AD=0;if(setjmp(_tmp3AB.handler))_tmp3AD=1;if(!_tmp3AD){x=Cyc_yyget_YY18(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);;_pop_handler();}
else{void*_tmp3AC=(void*)_exn_thrown;void*_tmp3AF=_tmp3AC;_LL1FF: if(*((void**)
_tmp3AF)!= Cyc_Core_Failure)goto _LL201;_LL200: x=0;goto _LL1FE;_LL201:;_LL202:(
void)_throw(_tmp3AF);_LL1FE:;}}}{struct _handler_cons _tmp3B0;_push_handler(&
_tmp3B0);{int _tmp3B2=0;if(setjmp(_tmp3B0.handler))_tmp3B2=1;if(!_tmp3B2){y=Cyc_yyget_YY18(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);;_pop_handler();}else{
void*_tmp3B1=(void*)_exn_thrown;void*_tmp3B4=_tmp3B1;_LL204: if(*((void**)_tmp3B4)
!= Cyc_Core_Failure)goto _LL206;_LL205: y=0;goto _LL203;_LL206:;_LL207:(void)_throw(
_tmp3B4);_LL203:;}}}yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp3B5;(_tmp3B5.YY18).tag=22;(_tmp3B5.YY18).f1=({struct Cyc_List_List*_tmp3B6=
_cycalloc(sizeof(*_tmp3B6));_tmp3B6->hd=({struct Cyc_Absyn_Decl*_tmp3B7=_cycalloc(
sizeof(*_tmp3B7));_tmp3B7->r=(void*)((void*)({struct Cyc_Absyn_Namespace_d_struct*
_tmp3B8=_cycalloc(sizeof(*_tmp3B8));_tmp3B8[0]=({struct Cyc_Absyn_Namespace_d_struct
_tmp3B9;_tmp3B9.tag=10;_tmp3B9.f1=({struct _dyneither_ptr*_tmp3BA=_cycalloc(
sizeof(*_tmp3BA));_tmp3BA[0]=nspace;_tmp3BA;});_tmp3B9.f2=x;_tmp3B9;});_tmp3B8;}));
_tmp3B7->loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line);_tmp3B7;});_tmp3B6->tl=y;_tmp3B6;});_tmp3B5;});
break;}case 7: _LL1F8: if(Cyc_strcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),({const char*_tmp3BB="C";
_tag_dyneither(_tmp3BB,sizeof(char),2);}))== 0)yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp3BC;(_tmp3BC.YY18).tag=22;(_tmp3BC.YY18).f1=({struct
Cyc_List_List*_tmp3BD=_cycalloc(sizeof(*_tmp3BD));_tmp3BD->hd=({struct Cyc_Absyn_Decl*
_tmp3BE=_cycalloc(sizeof(*_tmp3BE));_tmp3BE->r=(void*)((void*)({struct Cyc_Absyn_ExternC_d_struct*
_tmp3BF=_cycalloc(sizeof(*_tmp3BF));_tmp3BF[0]=({struct Cyc_Absyn_ExternC_d_struct
_tmp3C0;_tmp3C0.tag=12;_tmp3C0.f1=Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp3C0;});_tmp3BF;}));
_tmp3BE->loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line);_tmp3BE;});_tmp3BD->tl=Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp3BD;});_tmp3BC;});else{
if(Cyc_strcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),({const char*_tmp3C1="C include";
_tag_dyneither(_tmp3C1,sizeof(char),10);}))!= 0)Cyc_Parse_err(({const char*
_tmp3C2="expecting \"C\" or \"C include\"";_tag_dyneither(_tmp3C2,sizeof(char),
29);}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 4)]).last_line));yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp3C3;(_tmp3C3.YY18).tag=22;(_tmp3C3.YY18).f1=({struct Cyc_List_List*_tmp3C4=
_cycalloc(sizeof(*_tmp3C4));_tmp3C4->hd=({struct Cyc_Absyn_Decl*_tmp3C5=_cycalloc(
sizeof(*_tmp3C5));_tmp3C5->r=(void*)((void*)({struct Cyc_Absyn_ExternCinclude_d_struct*
_tmp3C6=_cycalloc(sizeof(*_tmp3C6));_tmp3C6[0]=({struct Cyc_Absyn_ExternCinclude_d_struct
_tmp3C7;_tmp3C7.tag=13;_tmp3C7.f1=Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp3C7.f2=0;_tmp3C7;});
_tmp3C6;}));_tmp3C5->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line);_tmp3C5;});
_tmp3C4->tl=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp3C4;});_tmp3C3;});}break;case 8: _LL208: if(Cyc_strcmp((struct
_dyneither_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 5)]),({const char*_tmp3C8="C include";_tag_dyneither(_tmp3C8,
sizeof(char),10);}))!= 0)Cyc_Parse_err(({const char*_tmp3C9="expecting \"C include\"";
_tag_dyneither(_tmp3C9,sizeof(char),22);}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).last_line));{struct Cyc_List_List*
exs=Cyc_yyget_YY52(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp3CA;(_tmp3CA.YY18).tag=
22;(_tmp3CA.YY18).f1=({struct Cyc_List_List*_tmp3CB=_cycalloc(sizeof(*_tmp3CB));
_tmp3CB->hd=({struct Cyc_Absyn_Decl*_tmp3CC=_cycalloc(sizeof(*_tmp3CC));_tmp3CC->r=(
void*)((void*)({struct Cyc_Absyn_ExternCinclude_d_struct*_tmp3CD=_cycalloc(
sizeof(*_tmp3CD));_tmp3CD[0]=({struct Cyc_Absyn_ExternCinclude_d_struct _tmp3CE;
_tmp3CE.tag=13;_tmp3CE.f1=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]);_tmp3CE.f2=exs;_tmp3CE;});_tmp3CD;}));_tmp3CC->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line);_tmp3CC;});
_tmp3CB->tl=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp3CB;});_tmp3CA;});break;}case 9: _LL209: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp3CF;(_tmp3CF.YY18).tag=22;(_tmp3CF.YY18).f1=({struct
Cyc_List_List*_tmp3D0=_cycalloc(sizeof(*_tmp3D0));_tmp3D0->hd=({struct Cyc_Absyn_Decl*
_tmp3D1=_cycalloc(sizeof(*_tmp3D1));_tmp3D1->r=(void*)((void*)0);_tmp3D1->loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line);_tmp3D1;});_tmp3D0->tl=Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp3D0;});_tmp3CF;});
break;case 10: _LL20A: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp3D2;(_tmp3D2.YY18).tag=22;(_tmp3D2.YY18).f1=({struct Cyc_List_List*_tmp3D3=
_cycalloc(sizeof(*_tmp3D3));_tmp3D3->hd=({struct Cyc_Absyn_Decl*_tmp3D4=_cycalloc(
sizeof(*_tmp3D4));_tmp3D4->r=(void*)((void*)1);_tmp3D4->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line);_tmp3D4;});
_tmp3D3->tl=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp3D3;});_tmp3D2;});break;case 11: _LL20B: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp3D5;(_tmp3D5.YY18).tag=22;(_tmp3D5.YY18).f1=0;_tmp3D5;});
break;case 12: _LL20C: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset
- 1)];break;case 13: _LL20D: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp3D6;(_tmp3D6.YY52).tag=56;(_tmp3D6.YY52).f1=0;_tmp3D6;});break;case 14: _LL20E:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp3D7;(_tmp3D7.YY52).tag=
56;(_tmp3D7.YY52).f1=({struct Cyc_List_List*_tmp3D8=_cycalloc(sizeof(*_tmp3D8));
_tmp3D8->hd=({struct _tuple18*_tmp3D9=_cycalloc(sizeof(*_tmp3D9));_tmp3D9->f1=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line);_tmp3D9->f2=
Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp3D9->f3=0;_tmp3D9;});_tmp3D8->tl=0;_tmp3D8;});_tmp3D7;});break;case 15: _LL20F:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp3DA;(_tmp3DA.YY52).tag=
56;(_tmp3DA.YY52).f1=({struct Cyc_List_List*_tmp3DB=_cycalloc(sizeof(*_tmp3DB));
_tmp3DB->hd=({struct _tuple18*_tmp3DC=_cycalloc(sizeof(*_tmp3DC));_tmp3DC->f1=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line);_tmp3DC->f2=
Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);
_tmp3DC->f3=0;_tmp3DC;});_tmp3DB->tl=Cyc_yyget_YY52(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp3DB;});_tmp3DA;});
break;case 16: _LL210: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp3DD;(_tmp3DD.YY18).tag=22;(_tmp3DD.YY18).f1=({struct Cyc_List_List*_tmp3DE=
_cycalloc(sizeof(*_tmp3DE));_tmp3DE->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Fn_d_struct*
_tmp3DF=_cycalloc(sizeof(*_tmp3DF));_tmp3DF[0]=({struct Cyc_Absyn_Fn_d_struct
_tmp3E0;_tmp3E0.tag=1;_tmp3E0.f1=Cyc_yyget_YY17(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp3E0;});_tmp3DF;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp3DE->tl=0;
_tmp3DE;});_tmp3DD;});break;case 17: _LL211: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 18: _LL212: yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp3E1;(_tmp3E1.YY18).tag=22;(
_tmp3E1.YY18).f1=0;_tmp3E1;});break;case 19: _LL213: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp3E2;(_tmp3E2.YY17).tag=21;(_tmp3E2.YY17).f1=Cyc_Parse_make_function(
0,Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0,
Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp3E2;});
break;case 20: _LL214: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp3E3;(_tmp3E3.YY17).tag=21;(_tmp3E3.YY17).f1=Cyc_Parse_make_function(({struct
Cyc_Core_Opt*_tmp3E4=_cycalloc(sizeof(*_tmp3E4));_tmp3E4->v=Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp3E4;}),Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0,Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp3E3;});
break;case 21: _LL215: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp3E5;(_tmp3E5.YY17).tag=21;(_tmp3E5.YY17).f1=Cyc_Parse_make_function(0,Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp3E5;});
break;case 22: _LL216: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp3E6;(_tmp3E6.YY17).tag=21;(_tmp3E6.YY17).f1=Cyc_Parse_make_function(({struct
Cyc_Core_Opt*_tmp3E7=_cycalloc(sizeof(*_tmp3E7));_tmp3E7->v=Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp3E7;}),Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp3E6;});
break;case 23: _LL217: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp3E8;(_tmp3E8.YY17).tag=21;(_tmp3E8.YY17).f1=Cyc_Parse_make_function(({struct
Cyc_Core_Opt*_tmp3E9=_cycalloc(sizeof(*_tmp3E9));_tmp3E9->v=Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp3E9;}),Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0,Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp3E8;});
break;case 24: _LL218: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp3EA;(_tmp3EA.YY17).tag=21;(_tmp3EA.YY17).f1=Cyc_Parse_make_function(({struct
Cyc_Core_Opt*_tmp3EB=_cycalloc(sizeof(*_tmp3EB));_tmp3EB->v=Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp3EB;}),Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp3EA;});
break;case 25: _LL219: Cyc_Lex_enter_using(Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]));yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 26: _LL21A: Cyc_Lex_leave_using();
break;case 27: _LL21B: Cyc_Lex_enter_namespace(({struct _dyneither_ptr*_tmp3EC=
_cycalloc(sizeof(*_tmp3EC));_tmp3EC[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp3EC;}));yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 28: _LL21C: Cyc_Lex_leave_namespace();
break;case 29: _LL21D: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp3ED;(_tmp3ED.YY18).tag=22;(_tmp3ED.YY18).f1=Cyc_Parse_make_declarations(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp3ED;});
break;case 30: _LL21E: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp3EE;(_tmp3EE.YY18).tag=22;(_tmp3EE.YY18).f1=Cyc_Parse_make_declarations(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY21(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp3EE;});
break;case 31: _LL21F: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp3EF;(_tmp3EF.YY18).tag=22;(_tmp3EF.YY18).f1=({struct Cyc_List_List*_tmp3F0=
_cycalloc(sizeof(*_tmp3F0));_tmp3F0->hd=Cyc_Absyn_let_decl(Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp3F0->tl=0;
_tmp3F0;});_tmp3EF;});break;case 32: _LL220: {struct Cyc_List_List*_tmp3F1=0;{
struct Cyc_List_List*_tmp3F2=Cyc_yyget_YY36(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);for(0;_tmp3F2 != 0;_tmp3F2=_tmp3F2->tl){struct
_dyneither_ptr*_tmp3F3=(struct _dyneither_ptr*)_tmp3F2->hd;struct _tuple1*qv=({
struct _tuple1*_tmp3F6=_cycalloc(sizeof(*_tmp3F6));_tmp3F6->f1=(union Cyc_Absyn_Nmspace_union)({
union Cyc_Absyn_Nmspace_union _tmp3F7;(_tmp3F7.Rel_n).tag=1;(_tmp3F7.Rel_n).f1=0;
_tmp3F7;});_tmp3F6->f2=_tmp3F3;_tmp3F6;});struct Cyc_Absyn_Vardecl*_tmp3F4=Cyc_Absyn_new_vardecl(
qv,Cyc_Absyn_wildtyp(0),0);_tmp3F1=({struct Cyc_List_List*_tmp3F5=_cycalloc(
sizeof(*_tmp3F5));_tmp3F5->hd=_tmp3F4;_tmp3F5->tl=_tmp3F1;_tmp3F5;});}}_tmp3F1=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp3F1);yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp3F8;(_tmp3F8.YY18).tag=22;(
_tmp3F8.YY18).f1=({struct Cyc_List_List*_tmp3F9=_cycalloc(sizeof(*_tmp3F9));
_tmp3F9->hd=Cyc_Absyn_letv_decl(_tmp3F1,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp3F9->tl=0;
_tmp3F9;});_tmp3F8;});break;}case 33: _LL221: if(Cyc_zstrcmp((struct _dyneither_ptr)
Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),({
const char*_tmp3FA="`H";_tag_dyneither(_tmp3FA,sizeof(char),3);}))== 0  || Cyc_zstrcmp((
struct _dyneither_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 4)]),({const char*_tmp3FB="`U";_tag_dyneither(_tmp3FB,sizeof(
char),3);}))== 0)Cyc_Parse_err((struct _dyneither_ptr)({struct Cyc_String_pa_struct
_tmp3FE;_tmp3FE.tag=0;_tmp3FE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]));{
void*_tmp3FC[1]={& _tmp3FE};Cyc_aprintf(({const char*_tmp3FD="bad occurrence of heap region %s";
_tag_dyneither(_tmp3FD,sizeof(char),33);}),_tag_dyneither(_tmp3FC,sizeof(void*),
1));}}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 4)]).last_line));{struct Cyc_Absyn_Tvar*tv=({struct Cyc_Absyn_Tvar*
_tmp408=_cycalloc(sizeof(*_tmp408));_tmp408->name=({struct _dyneither_ptr*_tmp409=
_cycalloc(sizeof(*_tmp409));_tmp409[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp409;});_tmp408->identity=
- 1;_tmp408->kind=(void*)Cyc_Tcutil_kind_to_bound((void*)3);_tmp408;});void*t=(
void*)({struct Cyc_Absyn_VarType_struct*_tmp406=_cycalloc(sizeof(*_tmp406));
_tmp406[0]=({struct Cyc_Absyn_VarType_struct _tmp407;_tmp407.tag=1;_tmp407.f1=tv;
_tmp407;});_tmp406;});struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(({struct
_tuple1*_tmp401=_cycalloc(sizeof(*_tmp401));_tmp401->f1=(union Cyc_Absyn_Nmspace_union)({
union Cyc_Absyn_Nmspace_union _tmp403;(_tmp403.Loc_n).tag=0;_tmp403;});_tmp401->f2=({
struct _dyneither_ptr*_tmp402=_cycalloc(sizeof(*_tmp402));_tmp402[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp402;});_tmp401;}),(
void*)({struct Cyc_Absyn_RgnHandleType_struct*_tmp404=_cycalloc(sizeof(*_tmp404));
_tmp404[0]=({struct Cyc_Absyn_RgnHandleType_struct _tmp405;_tmp405.tag=14;_tmp405.f1=(
void*)t;_tmp405;});_tmp404;}),0);yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp3FF;(_tmp3FF.YY18).tag=22;(_tmp3FF.YY18).f1=({struct Cyc_List_List*_tmp400=
_cycalloc(sizeof(*_tmp400));_tmp400->hd=Cyc_Absyn_region_decl(tv,vd,Cyc_yyget_YY31(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp400->tl=0;
_tmp400;});_tmp3FF;});break;}case 34: _LL222: if(Cyc_zstrcmp((struct _dyneither_ptr)
Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),({
const char*_tmp40A="H";_tag_dyneither(_tmp40A,sizeof(char),2);}))== 0)Cyc_Parse_err(({
const char*_tmp40B="bad occurrence of heap region `H";_tag_dyneither(_tmp40B,
sizeof(char),33);}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line));{struct Cyc_Absyn_Tvar*
tv=({struct Cyc_Absyn_Tvar*_tmp415=_cycalloc(sizeof(*_tmp415));_tmp415->name=({
struct _dyneither_ptr*_tmp416=_cycalloc(sizeof(*_tmp416));_tmp416[0]=(struct
_dyneither_ptr)({struct Cyc_String_pa_struct _tmp419;_tmp419.tag=0;_tmp419.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]));{void*_tmp417[1]={&
_tmp419};Cyc_aprintf(({const char*_tmp418="`%s";_tag_dyneither(_tmp418,sizeof(
char),4);}),_tag_dyneither(_tmp417,sizeof(void*),1));}});_tmp416;});_tmp415->identity=
- 1;_tmp415->kind=(void*)Cyc_Tcutil_kind_to_bound((void*)3);_tmp415;});void*t=(
void*)({struct Cyc_Absyn_VarType_struct*_tmp413=_cycalloc(sizeof(*_tmp413));
_tmp413[0]=({struct Cyc_Absyn_VarType_struct _tmp414;_tmp414.tag=1;_tmp414.f1=tv;
_tmp414;});_tmp413;});struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(({struct
_tuple1*_tmp40E=_cycalloc(sizeof(*_tmp40E));_tmp40E->f1=(union Cyc_Absyn_Nmspace_union)({
union Cyc_Absyn_Nmspace_union _tmp410;(_tmp410.Loc_n).tag=0;_tmp410;});_tmp40E->f2=({
struct _dyneither_ptr*_tmp40F=_cycalloc(sizeof(*_tmp40F));_tmp40F[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp40F;});_tmp40E;}),(
void*)({struct Cyc_Absyn_RgnHandleType_struct*_tmp411=_cycalloc(sizeof(*_tmp411));
_tmp411[0]=({struct Cyc_Absyn_RgnHandleType_struct _tmp412;_tmp412.tag=14;_tmp412.f1=(
void*)t;_tmp412;});_tmp411;}),0);yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp40C;(_tmp40C.YY18).tag=22;(_tmp40C.YY18).f1=({struct Cyc_List_List*_tmp40D=
_cycalloc(sizeof(*_tmp40D));_tmp40D->hd=Cyc_Absyn_region_decl(tv,vd,0,Cyc_yyget_YY4(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp40D->tl=0;
_tmp40D;});_tmp40C;});break;}case 35: _LL223: if(Cyc_zstrcmp((struct _dyneither_ptr)
Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),({
const char*_tmp41A="H";_tag_dyneither(_tmp41A,sizeof(char),2);}))== 0)Cyc_Parse_err(({
const char*_tmp41B="bad occurrence of heap region `H";_tag_dyneither(_tmp41B,
sizeof(char),33);}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line));{struct Cyc_Absyn_Tvar*
tv=({struct Cyc_Absyn_Tvar*_tmp425=_cycalloc(sizeof(*_tmp425));_tmp425->name=({
struct _dyneither_ptr*_tmp426=_cycalloc(sizeof(*_tmp426));_tmp426[0]=(struct
_dyneither_ptr)({struct Cyc_String_pa_struct _tmp429;_tmp429.tag=0;_tmp429.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]));{void*_tmp427[1]={&
_tmp429};Cyc_aprintf(({const char*_tmp428="`%s";_tag_dyneither(_tmp428,sizeof(
char),4);}),_tag_dyneither(_tmp427,sizeof(void*),1));}});_tmp426;});_tmp425->identity=
- 1;_tmp425->kind=(void*)Cyc_Tcutil_kind_to_bound((void*)3);_tmp425;});void*t=(
void*)({struct Cyc_Absyn_VarType_struct*_tmp423=_cycalloc(sizeof(*_tmp423));
_tmp423[0]=({struct Cyc_Absyn_VarType_struct _tmp424;_tmp424.tag=1;_tmp424.f1=tv;
_tmp424;});_tmp423;});struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(({struct
_tuple1*_tmp41E=_cycalloc(sizeof(*_tmp41E));_tmp41E->f1=(union Cyc_Absyn_Nmspace_union)({
union Cyc_Absyn_Nmspace_union _tmp420;(_tmp420.Loc_n).tag=0;_tmp420;});_tmp41E->f2=({
struct _dyneither_ptr*_tmp41F=_cycalloc(sizeof(*_tmp41F));_tmp41F[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp41F;});_tmp41E;}),(
void*)({struct Cyc_Absyn_RgnHandleType_struct*_tmp421=_cycalloc(sizeof(*_tmp421));
_tmp421[0]=({struct Cyc_Absyn_RgnHandleType_struct _tmp422;_tmp422.tag=14;_tmp422.f1=(
void*)t;_tmp422;});_tmp421;}),0);yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp41C;(_tmp41C.YY18).tag=22;(_tmp41C.YY18).f1=({struct Cyc_List_List*_tmp41D=
_cycalloc(sizeof(*_tmp41D));_tmp41D->hd=Cyc_Absyn_region_decl(tv,vd,1,0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp41D->tl=0;
_tmp41D;});_tmp41C;});break;}case 36: _LL224: {struct Cyc_Absyn_Tvar*tv=({struct Cyc_Absyn_Tvar*
_tmp42F=_cycalloc(sizeof(*_tmp42F));_tmp42F->name=({struct _dyneither_ptr*_tmp432=
_cycalloc(sizeof(*_tmp432));_tmp432[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 5)]);_tmp432;});_tmp42F->identity=
- 1;_tmp42F->kind=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp430=_cycalloc(
sizeof(*_tmp430));_tmp430[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp431;_tmp431.tag=0;
_tmp431.f1=(void*)((void*)3);_tmp431;});_tmp430;}));_tmp42F;});struct Cyc_Absyn_Vardecl*
vd=Cyc_Absyn_new_vardecl(({struct _tuple1*_tmp42C=_cycalloc(sizeof(*_tmp42C));
_tmp42C->f1=(union Cyc_Absyn_Nmspace_union)({union Cyc_Absyn_Nmspace_union _tmp42E;(
_tmp42E.Loc_n).tag=0;_tmp42E;});_tmp42C->f2=({struct _dyneither_ptr*_tmp42D=
_cycalloc(sizeof(*_tmp42D));_tmp42D[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp42D;});_tmp42C;}),(
void*)0,0);yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp42A;(
_tmp42A.YY18).tag=22;(_tmp42A.YY18).f1=({struct Cyc_List_List*_tmp42B=_cycalloc(
sizeof(*_tmp42B));_tmp42B->hd=Cyc_Absyn_alias_decl(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),tv,vd,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp42B->tl=0;
_tmp42B;});_tmp42A;});break;}case 37: _LL225: yyval=(union Cyc_YYSTYPE_union)({union
Cyc_YYSTYPE_union _tmp433;(_tmp433.YY31).tag=35;(_tmp433.YY31).f1=0;_tmp433;});
break;case 38: _LL226: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp434;(_tmp434.YY31).tag=35;(_tmp434.YY31).f1=1;_tmp434;});break;case 39: _LL227:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 40:
_LL228: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp435;(_tmp435.YY18).tag=
22;(_tmp435.YY18).f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_imp_append)(Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]));_tmp435;});break;case 41: _LL229: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp436;(_tmp436.YY19).tag=23;(_tmp436.YY19).f1=({struct
Cyc_Parse_Declaration_spec*_tmp437=_cycalloc(sizeof(*_tmp437));_tmp437->sc=({
struct Cyc_Core_Opt*_tmp438=_cycalloc(sizeof(*_tmp438));_tmp438->v=(void*)Cyc_yyget_YY22(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp438;});_tmp437->tq=
Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp437->type_specs=
0;_tmp437->is_inline=0;_tmp437->attributes=0;_tmp437;});_tmp436;});break;case 42:
_LL22A: if((Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->sc
!= 0)({void*_tmp439=0;Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),({const char*
_tmp43A="Only one storage class is allowed in a declaration";_tag_dyneither(
_tmp43A,sizeof(char),51);}),_tag_dyneither(_tmp439,sizeof(void*),0));});yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp43B;(_tmp43B.YY19).tag=23;(
_tmp43B.YY19).f1=({struct Cyc_Parse_Declaration_spec*_tmp43C=_cycalloc(sizeof(*
_tmp43C));_tmp43C->sc=({struct Cyc_Core_Opt*_tmp43D=_cycalloc(sizeof(*_tmp43D));
_tmp43D->v=(void*)Cyc_yyget_YY22(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp43D;});_tmp43C->tq=(Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->tq;_tmp43C->type_specs=(
Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->type_specs;
_tmp43C->is_inline=(Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->is_inline;_tmp43C->attributes=(Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes;_tmp43C;});
_tmp43B;});break;case 43: _LL22B: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp43E;(_tmp43E.YY19).tag=23;(_tmp43E.YY19).f1=({struct Cyc_Parse_Declaration_spec*
_tmp43F=_cycalloc(sizeof(*_tmp43F));_tmp43F->sc=0;_tmp43F->tq=Cyc_Absyn_empty_tqual(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp43F->type_specs=({struct Cyc_List_List*_tmp440=
_cycalloc(sizeof(*_tmp440));_tmp440->hd=(void*)Cyc_yyget_YY23(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp440->tl=0;_tmp440;});
_tmp43F->is_inline=0;_tmp43F->attributes=0;_tmp43F;});_tmp43E;});break;case 44:
_LL22C: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp441;(_tmp441.YY19).tag=
23;(_tmp441.YY19).f1=({struct Cyc_Parse_Declaration_spec*_tmp442=_cycalloc(
sizeof(*_tmp442));_tmp442->sc=(Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->sc;_tmp442->tq=(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tq;_tmp442->type_specs=({
struct Cyc_List_List*_tmp443=_cycalloc(sizeof(*_tmp443));_tmp443->hd=(void*)Cyc_yyget_YY23(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp443->tl=(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->type_specs;_tmp443;});
_tmp442->is_inline=(Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->is_inline;_tmp442->attributes=(Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes;_tmp442;});
_tmp441;});break;case 45: _LL22D: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp444;(_tmp444.YY19).tag=23;(_tmp444.YY19).f1=({struct Cyc_Parse_Declaration_spec*
_tmp445=_cycalloc(sizeof(*_tmp445));_tmp445->sc=0;_tmp445->tq=Cyc_yyget_YY25(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp445->type_specs=0;
_tmp445->is_inline=0;_tmp445->attributes=0;_tmp445;});_tmp444;});break;case 46:
_LL22E: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp446;(_tmp446.YY19).tag=
23;(_tmp446.YY19).f1=({struct Cyc_Parse_Declaration_spec*_tmp447=_cycalloc(
sizeof(*_tmp447));_tmp447->sc=(Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->sc;_tmp447->tq=Cyc_Absyn_combine_tqual(
Cyc_yyget_YY25(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tq);_tmp447->type_specs=(
Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->type_specs;
_tmp447->is_inline=(Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->is_inline;_tmp447->attributes=(Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes;_tmp447;});
_tmp446;});break;case 47: _LL22F: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp448;(_tmp448.YY19).tag=23;(_tmp448.YY19).f1=({struct Cyc_Parse_Declaration_spec*
_tmp449=_cycalloc(sizeof(*_tmp449));_tmp449->sc=0;_tmp449->tq=Cyc_Absyn_empty_tqual(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp449->type_specs=0;_tmp449->is_inline=1;_tmp449->attributes=
0;_tmp449;});_tmp448;});break;case 48: _LL230: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp44A;(_tmp44A.YY19).tag=23;(_tmp44A.YY19).f1=({struct
Cyc_Parse_Declaration_spec*_tmp44B=_cycalloc(sizeof(*_tmp44B));_tmp44B->sc=(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->sc;_tmp44B->tq=(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tq;_tmp44B->type_specs=(
Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->type_specs;
_tmp44B->is_inline=1;_tmp44B->attributes=(Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes;_tmp44B;});
_tmp44A;});break;case 49: _LL231: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp44C;(_tmp44C.YY19).tag=23;(_tmp44C.YY19).f1=({struct Cyc_Parse_Declaration_spec*
_tmp44D=_cycalloc(sizeof(*_tmp44D));_tmp44D->sc=0;_tmp44D->tq=Cyc_Absyn_empty_tqual(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp44D->type_specs=0;_tmp44D->is_inline=0;_tmp44D->attributes=
Cyc_yyget_YY45(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp44D;});
_tmp44C;});break;case 50: _LL232: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp44E;(_tmp44E.YY19).tag=23;(_tmp44E.YY19).f1=({struct Cyc_Parse_Declaration_spec*
_tmp44F=_cycalloc(sizeof(*_tmp44F));_tmp44F->sc=(Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->sc;_tmp44F->tq=(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tq;_tmp44F->type_specs=(
Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->type_specs;
_tmp44F->is_inline=(Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->is_inline;_tmp44F->attributes=Cyc_List_imp_append(Cyc_yyget_YY45(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes);_tmp44F;});
_tmp44E;});break;case 51: _LL233: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp450;(_tmp450.YY22).tag=26;(_tmp450.YY22).f1=(void*)((void*)4);_tmp450;});
break;case 52: _LL234: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp451;(_tmp451.YY22).tag=26;(_tmp451.YY22).f1=(void*)((void*)5);_tmp451;});
break;case 53: _LL235: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp452;(_tmp452.YY22).tag=26;(_tmp452.YY22).f1=(void*)((void*)3);_tmp452;});
break;case 54: _LL236: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp453;(_tmp453.YY22).tag=26;(_tmp453.YY22).f1=(void*)((void*)1);_tmp453;});
break;case 55: _LL237: if(Cyc_strcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),({const char*_tmp454="C";
_tag_dyneither(_tmp454,sizeof(char),2);}))!= 0)Cyc_Parse_err(({const char*_tmp455="only extern or extern \"C\" is allowed";
_tag_dyneither(_tmp455,sizeof(char),37);}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp456;(_tmp456.YY22).tag=26;(_tmp456.YY22).f1=(void*)((
void*)2);_tmp456;});break;case 56: _LL238: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp457;(_tmp457.YY22).tag=26;(_tmp457.YY22).f1=(void*)((void*)0);_tmp457;});
break;case 57: _LL239: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp458;(_tmp458.YY22).tag=26;(_tmp458.YY22).f1=(void*)((void*)6);_tmp458;});
break;case 58: _LL23A: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp459;(_tmp459.YY45).tag=49;(_tmp459.YY45).f1=0;_tmp459;});break;case 59: _LL23B:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 60:
_LL23C: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)];break;
case 61: _LL23D: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp45A;(
_tmp45A.YY45).tag=49;(_tmp45A.YY45).f1=({struct Cyc_List_List*_tmp45B=_cycalloc(
sizeof(*_tmp45B));_tmp45B->hd=(void*)Cyc_yyget_YY46(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp45B->tl=0;_tmp45B;});
_tmp45A;});break;case 62: _LL23E: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp45C;(_tmp45C.YY45).tag=49;(_tmp45C.YY45).f1=({struct Cyc_List_List*_tmp45D=
_cycalloc(sizeof(*_tmp45D));_tmp45D->hd=(void*)Cyc_yyget_YY46(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp45D->tl=Cyc_yyget_YY45(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp45D;});_tmp45C;});
break;case 63: _LL23F: {static struct Cyc_Absyn_Aligned_att_struct att_aligned={1,- 1};
static struct _tuple19 att_map[17]={{{_tmp462,_tmp462,_tmp462 + 8},(void*)0},{{
_tmp463,_tmp463,_tmp463 + 6},(void*)1},{{_tmp464,_tmp464,_tmp464 + 9},(void*)2},{{
_tmp465,_tmp465,_tmp465 + 9},(void*)3},{{_tmp466,_tmp466,_tmp466 + 6},(void*)4},{{
_tmp467,_tmp467,_tmp467 + 8},(void*)& att_aligned},{{_tmp468,_tmp468,_tmp468 + 7},(
void*)5},{{_tmp469,_tmp469,_tmp469 + 7},(void*)7},{{_tmp46A,_tmp46A,_tmp46A + 7},(
void*)8},{{_tmp46B,_tmp46B,_tmp46B + 5},(void*)9},{{_tmp46C,_tmp46C,_tmp46C + 10},(
void*)10},{{_tmp46D,_tmp46D,_tmp46D + 10},(void*)11},{{_tmp46E,_tmp46E,_tmp46E + 
23},(void*)12},{{_tmp46F,_tmp46F,_tmp46F + 12},(void*)13},{{_tmp470,_tmp470,
_tmp470 + 11},(void*)14},{{_tmp471,_tmp471,_tmp471 + 22},(void*)15},{{_tmp472,
_tmp472,_tmp472 + 5},(void*)16}};struct _dyneither_ptr _tmp45E=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);if((((
_get_dyneither_size(_tmp45E,sizeof(char))> 4  && *((const char*)
_check_dyneither_subscript(_tmp45E,sizeof(char),0))== '_') && *((const char*)
_check_dyneither_subscript(_tmp45E,sizeof(char),1))== '_') && *((const char*)
_check_dyneither_subscript(_tmp45E,sizeof(char),(int)(_get_dyneither_size(
_tmp45E,sizeof(char))- 2)))== '_') && *((const char*)_check_dyneither_subscript(
_tmp45E,sizeof(char),(int)(_get_dyneither_size(_tmp45E,sizeof(char))- 3)))== '_')
_tmp45E=(struct _dyneither_ptr)Cyc_substring((struct _dyneither_ptr)_tmp45E,2,
_get_dyneither_size(_tmp45E,sizeof(char))- 5);{int i=0;for(0;i < 17;++ i){if(Cyc_strcmp((
struct _dyneither_ptr)_tmp45E,(struct _dyneither_ptr)(att_map[i]).f1)== 0){yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp45F;(_tmp45F.YY46).tag=50;(
_tmp45F.YY46).f1=(void*)(att_map[i]).f2;_tmp45F;});break;}}if(i == 17){Cyc_Parse_err(({
const char*_tmp460="unrecognized attribute";_tag_dyneither(_tmp460,sizeof(char),
23);}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp461;(_tmp461.YY46).tag=50;(_tmp461.YY46).f1=(void*)((void*)1);_tmp461;});}
break;}}case 64: _LL240: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp474;(_tmp474.YY46).tag=50;(_tmp474.YY46).f1=(void*)((void*)4);_tmp474;});
break;case 65: _LL241: {struct _dyneither_ptr _tmp475=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);int _tmp477;struct
_tuple11 _tmp476=Cyc_yyget_Int_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp477=_tmp476.f2;{void*a;if(Cyc_zstrcmp((struct
_dyneither_ptr)_tmp475,({const char*_tmp478="regparm";_tag_dyneither(_tmp478,
sizeof(char),8);}))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp475,({const char*
_tmp479="__regparm__";_tag_dyneither(_tmp479,sizeof(char),12);}))== 0){if(
_tmp477 < 0  || _tmp477 > 3)Cyc_Parse_err(({const char*_tmp47A="regparm requires value between 0 and 3";
_tag_dyneither(_tmp47A,sizeof(char),39);}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));a=(void*)({
struct Cyc_Absyn_Regparm_att_struct*_tmp47B=_cycalloc_atomic(sizeof(*_tmp47B));
_tmp47B[0]=({struct Cyc_Absyn_Regparm_att_struct _tmp47C;_tmp47C.tag=0;_tmp47C.f1=
_tmp477;_tmp47C;});_tmp47B;});}else{if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp475,({
const char*_tmp47D="aligned";_tag_dyneither(_tmp47D,sizeof(char),8);}))== 0  || 
Cyc_zstrcmp((struct _dyneither_ptr)_tmp475,({const char*_tmp47E="__aligned__";
_tag_dyneither(_tmp47E,sizeof(char),12);}))== 0){if(_tmp477 < 0)Cyc_Parse_err(({
const char*_tmp47F="aligned requires positive power of two";_tag_dyneither(
_tmp47F,sizeof(char),39);}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));{
unsigned int j=(unsigned int)_tmp477;for(0;(j & 1)== 0;j=j >> 1){;}j=j >> 1;if(j != 0)
Cyc_Parse_err(({const char*_tmp480="aligned requires positive power of two";
_tag_dyneither(_tmp480,sizeof(char),39);}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));a=(void*)({
struct Cyc_Absyn_Aligned_att_struct*_tmp481=_cycalloc_atomic(sizeof(*_tmp481));
_tmp481[0]=({struct Cyc_Absyn_Aligned_att_struct _tmp482;_tmp482.tag=1;_tmp482.f1=
_tmp477;_tmp482;});_tmp481;});}}else{if(Cyc_zstrcmp((struct _dyneither_ptr)
_tmp475,({const char*_tmp483="initializes";_tag_dyneither(_tmp483,sizeof(char),12);}))
== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp475,({const char*_tmp484="__initializes__";
_tag_dyneither(_tmp484,sizeof(char),16);}))== 0)a=(void*)({struct Cyc_Absyn_Initializes_att_struct*
_tmp485=_cycalloc_atomic(sizeof(*_tmp485));_tmp485[0]=({struct Cyc_Absyn_Initializes_att_struct
_tmp486;_tmp486.tag=4;_tmp486.f1=_tmp477;_tmp486;});_tmp485;});else{Cyc_Parse_err(({
const char*_tmp487="unrecognized attribute";_tag_dyneither(_tmp487,sizeof(char),
23);}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line));a=(void*)1;}}}yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp488;(_tmp488.YY46).tag=50;(_tmp488.YY46).f1=(void*)a;
_tmp488;});break;}}case 66: _LL242: {struct _dyneither_ptr _tmp489=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);struct
_dyneither_ptr _tmp48A=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);void*a;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp489,({
const char*_tmp48B="section";_tag_dyneither(_tmp48B,sizeof(char),8);}))== 0  || 
Cyc_zstrcmp((struct _dyneither_ptr)_tmp489,({const char*_tmp48C="__section__";
_tag_dyneither(_tmp48C,sizeof(char),12);}))== 0)a=(void*)({struct Cyc_Absyn_Section_att_struct*
_tmp48D=_cycalloc(sizeof(*_tmp48D));_tmp48D[0]=({struct Cyc_Absyn_Section_att_struct
_tmp48E;_tmp48E.tag=2;_tmp48E.f1=_tmp48A;_tmp48E;});_tmp48D;});else{Cyc_Parse_err(({
const char*_tmp48F="unrecognized attribute";_tag_dyneither(_tmp48F,sizeof(char),
23);}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line));a=(void*)1;}yyval=(union Cyc_YYSTYPE_union)({union
Cyc_YYSTYPE_union _tmp490;(_tmp490.YY46).tag=50;(_tmp490.YY46).f1=(void*)a;
_tmp490;});break;}case 67: _LL243: {struct _dyneither_ptr _tmp491=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);struct
_dyneither_ptr _tmp492=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);void*a;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp491,({
const char*_tmp493="__mode__";_tag_dyneither(_tmp493,sizeof(char),9);}))== 0)a=(
void*)({struct Cyc_Absyn_Mode_att_struct*_tmp494=_cycalloc(sizeof(*_tmp494));
_tmp494[0]=({struct Cyc_Absyn_Mode_att_struct _tmp495;_tmp495.tag=5;_tmp495.f1=
_tmp492;_tmp495;});_tmp494;});else{Cyc_Parse_err(({const char*_tmp496="unrecognized attribute";
_tag_dyneither(_tmp496,sizeof(char),23);}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));a=(void*)1;}
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp497;(_tmp497.YY46).tag=
50;(_tmp497.YY46).f1=(void*)a;_tmp497;});break;}case 68: _LL244: {struct
_dyneither_ptr _tmp498=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 7)]);struct _dyneither_ptr _tmp499=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 5)]);int _tmp49B;struct
_tuple11 _tmp49A=Cyc_yyget_Int_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]);_tmp49B=_tmp49A.f2;{int _tmp49D;struct _tuple11 _tmp49C=Cyc_yyget_Int_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp49D=_tmp49C.f2;{
void*a=(void*)1;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp498,({const char*_tmp49E="format";
_tag_dyneither(_tmp49E,sizeof(char),7);}))== 0  || Cyc_zstrcmp((struct
_dyneither_ptr)_tmp498,({const char*_tmp49F="__format__";_tag_dyneither(_tmp49F,
sizeof(char),11);}))== 0){if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp499,({const
char*_tmp4A0="printf";_tag_dyneither(_tmp4A0,sizeof(char),7);}))== 0)a=(void*)({
struct Cyc_Absyn_Format_att_struct*_tmp4A1=_cycalloc(sizeof(*_tmp4A1));_tmp4A1[0]=({
struct Cyc_Absyn_Format_att_struct _tmp4A2;_tmp4A2.tag=3;_tmp4A2.f1=(void*)((void*)
0);_tmp4A2.f2=_tmp49B;_tmp4A2.f3=_tmp49D;_tmp4A2;});_tmp4A1;});else{if(Cyc_zstrcmp((
struct _dyneither_ptr)_tmp499,({const char*_tmp4A3="scanf";_tag_dyneither(_tmp4A3,
sizeof(char),6);}))== 0)a=(void*)({struct Cyc_Absyn_Format_att_struct*_tmp4A4=
_cycalloc(sizeof(*_tmp4A4));_tmp4A4[0]=({struct Cyc_Absyn_Format_att_struct
_tmp4A5;_tmp4A5.tag=3;_tmp4A5.f1=(void*)((void*)1);_tmp4A5.f2=_tmp49B;_tmp4A5.f3=
_tmp49D;_tmp4A5;});_tmp4A4;});else{Cyc_Parse_err(({const char*_tmp4A6="unrecognized format type";
_tag_dyneither(_tmp4A6,sizeof(char),25);}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).last_line));}}}else{Cyc_Parse_err(({
const char*_tmp4A7="unrecognized attribute";_tag_dyneither(_tmp4A7,sizeof(char),
23);}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 7)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));}yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp4A8;(_tmp4A8.YY46).tag=50;(_tmp4A8.YY46).f1=(void*)a;_tmp4A8;});break;}}}
case 69: _LL245: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];
break;case 70: _LL246: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp4A9;(_tmp4A9.YY23).tag=27;(_tmp4A9.YY23).f1=(void*)Cyc_Parse_type_spec((void*)({
struct Cyc_Absyn_TypedefType_struct*_tmp4AA=_cycalloc(sizeof(*_tmp4AA));_tmp4AA[0]=({
struct Cyc_Absyn_TypedefType_struct _tmp4AB;_tmp4AB.tag=16;_tmp4AB.f1=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp4AB.f2=Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp4AB.f3=0;_tmp4AB.f4=
0;_tmp4AB;});_tmp4AA;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp4A9;});
break;case 71: _LL247: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp4AC;(_tmp4AC.YY23).tag=27;(_tmp4AC.YY23).f1=(void*)Cyc_Parse_type_spec((void*)
0,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp4AC;});break;case 72: _LL248: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp4AD;(_tmp4AD.YY23).tag=27;(_tmp4AD.YY23).f1=(void*)Cyc_Parse_type_spec(
Cyc_Absyn_new_evar(0,0),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp4AD;});
break;case 73: _LL249: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp4AE;(_tmp4AE.YY23).tag=27;(_tmp4AE.YY23).f1=(void*)Cyc_Parse_type_spec(Cyc_Absyn_new_evar(
Cyc_Tcutil_kind_to_opt(Cyc_yyget_YY43(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)])),0),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp4AE;});
break;case 74: _LL24A: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp4AF;(_tmp4AF.YY23).tag=27;(_tmp4AF.YY23).f1=(void*)Cyc_Parse_type_spec(Cyc_Absyn_char_typ,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp4AF;});break;case 75: _LL24B: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp4B0;(_tmp4B0.YY23).tag=27;(_tmp4B0.YY23).f1=(void*)((
void*)({struct Cyc_Parse_Short_spec_struct*_tmp4B1=_cycalloc(sizeof(*_tmp4B1));
_tmp4B1[0]=({struct Cyc_Parse_Short_spec_struct _tmp4B2;_tmp4B2.tag=2;_tmp4B2.f1=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp4B2;});_tmp4B1;}));_tmp4B0;});break;case 76: _LL24C:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp4B3;(_tmp4B3.YY23).tag=
27;(_tmp4B3.YY23).f1=(void*)Cyc_Parse_type_spec(Cyc_Absyn_sint_typ,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp4B3;});
break;case 77: _LL24D: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp4B4;(_tmp4B4.YY23).tag=27;(_tmp4B4.YY23).f1=(void*)((void*)({struct Cyc_Parse_Long_spec_struct*
_tmp4B5=_cycalloc(sizeof(*_tmp4B5));_tmp4B5[0]=({struct Cyc_Parse_Long_spec_struct
_tmp4B6;_tmp4B6.tag=3;_tmp4B6.f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp4B6;});
_tmp4B5;}));_tmp4B4;});break;case 78: _LL24E: yyval=(union Cyc_YYSTYPE_union)({union
Cyc_YYSTYPE_union _tmp4B7;(_tmp4B7.YY23).tag=27;(_tmp4B7.YY23).f1=(void*)Cyc_Parse_type_spec(
Cyc_Absyn_float_typ,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp4B7;});
break;case 79: _LL24F: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp4B8;(_tmp4B8.YY23).tag=27;(_tmp4B8.YY23).f1=(void*)Cyc_Parse_type_spec(Cyc_Absyn_double_typ(
0),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp4B8;});break;case 80: _LL250: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp4B9;(_tmp4B9.YY23).tag=27;(_tmp4B9.YY23).f1=(void*)((
void*)({struct Cyc_Parse_Signed_spec_struct*_tmp4BA=_cycalloc(sizeof(*_tmp4BA));
_tmp4BA[0]=({struct Cyc_Parse_Signed_spec_struct _tmp4BB;_tmp4BB.tag=0;_tmp4BB.f1=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp4BB;});_tmp4BA;}));_tmp4B9;});break;case 81: _LL251:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp4BC;(_tmp4BC.YY23).tag=
27;(_tmp4BC.YY23).f1=(void*)((void*)({struct Cyc_Parse_Unsigned_spec_struct*
_tmp4BD=_cycalloc(sizeof(*_tmp4BD));_tmp4BD[0]=({struct Cyc_Parse_Unsigned_spec_struct
_tmp4BE;_tmp4BE.tag=1;_tmp4BE.f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp4BE;});
_tmp4BD;}));_tmp4BC;});break;case 82: _LL252: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 83: _LL253: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 84: _LL254:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 85:
_LL255: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp4BF;(_tmp4BF.YY23).tag=
27;(_tmp4BF.YY23).f1=(void*)Cyc_Parse_type_spec(Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp4BF;});
break;case 86: _LL256: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp4C0;(_tmp4C0.YY23).tag=27;(_tmp4C0.YY23).f1=(void*)Cyc_Parse_type_spec((void*)({
struct Cyc_Absyn_TupleType_struct*_tmp4C1=_cycalloc(sizeof(*_tmp4C1));_tmp4C1[0]=({
struct Cyc_Absyn_TupleType_struct _tmp4C2;_tmp4C2.tag=9;_tmp4C2.f1=((struct Cyc_List_List*(*)(
struct _tuple9*(*f)(struct Cyc_Position_Segment*,struct _tuple2*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_get_tqual_typ,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));_tmp4C2;});_tmp4C1;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp4C0;});break;case 87: _LL257: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp4C3;(_tmp4C3.YY23).tag=27;(_tmp4C3.YY23).f1=(void*)Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_RgnHandleType_struct*_tmp4C4=_cycalloc(sizeof(*_tmp4C4));
_tmp4C4[0]=({struct Cyc_Absyn_RgnHandleType_struct _tmp4C5;_tmp4C5.tag=14;_tmp4C5.f1=(
void*)Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp4C5;});_tmp4C4;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp4C3;});
break;case 88: _LL258: {void*_tmp4C6=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,
0);yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp4C7;(_tmp4C7.YY23).tag=
27;(_tmp4C7.YY23).f1=(void*)Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_DynRgnType_struct*
_tmp4C8=_cycalloc(sizeof(*_tmp4C8));_tmp4C8[0]=({struct Cyc_Absyn_DynRgnType_struct
_tmp4C9;_tmp4C9.tag=15;_tmp4C9.f1=(void*)Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp4C9.f2=(void*)
_tmp4C6;_tmp4C9;});_tmp4C8;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp4C7;});
break;}case 89: _LL259: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp4CA;(_tmp4CA.YY23).tag=27;(_tmp4CA.YY23).f1=(void*)Cyc_Parse_type_spec((void*)({
struct Cyc_Absyn_DynRgnType_struct*_tmp4CB=_cycalloc(sizeof(*_tmp4CB));_tmp4CB[0]=({
struct Cyc_Absyn_DynRgnType_struct _tmp4CC;_tmp4CC.tag=15;_tmp4CC.f1=(void*)Cyc_yyget_YY44(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp4CC.f2=(void*)
Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp4CC;});_tmp4CB;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp4CA;});
break;case 90: _LL25A: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp4CD;(_tmp4CD.YY23).tag=27;(_tmp4CD.YY23).f1=(void*)Cyc_Parse_type_spec((void*)({
struct Cyc_Absyn_TagType_struct*_tmp4CE=_cycalloc(sizeof(*_tmp4CE));_tmp4CE[0]=({
struct Cyc_Absyn_TagType_struct _tmp4CF;_tmp4CF.tag=18;_tmp4CF.f1=(void*)Cyc_yyget_YY44(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp4CF;});_tmp4CE;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp4CD;});break;case 91: _LL25B: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp4D0;(_tmp4D0.YY23).tag=27;(_tmp4D0.YY23).f1=(void*)Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_TagType_struct*_tmp4D1=_cycalloc(sizeof(*_tmp4D1));
_tmp4D1[0]=({struct Cyc_Absyn_TagType_struct _tmp4D2;_tmp4D2.tag=18;_tmp4D2.f1=(
void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ik,0);_tmp4D2;});
_tmp4D1;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp4D0;});break;case 92: _LL25C: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp4D3;(_tmp4D3.YY23).tag=27;(_tmp4D3.YY23).f1=(void*)Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_ValueofType_struct*_tmp4D4=_cycalloc(sizeof(*_tmp4D4));
_tmp4D4[0]=({struct Cyc_Absyn_ValueofType_struct _tmp4D5;_tmp4D5.tag=17;_tmp4D5.f1=
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp4D5;});_tmp4D4;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp4D3;});
break;case 93: _LL25D: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp4D6;(_tmp4D6.YY43).tag=47;(_tmp4D6.YY43).f1=(void*)Cyc_Parse_id_to_kind(Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp4D6;});
break;case 94: _LL25E: {struct Cyc_Position_Segment*loc=Cyc_Absyn_porting_c_code?
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line): 0;yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp4D7;(_tmp4D7.YY25).tag=29;(_tmp4D7.YY25).f1=({struct Cyc_Absyn_Tqual _tmp4D8;
_tmp4D8.print_const=1;_tmp4D8.q_volatile=0;_tmp4D8.q_restrict=0;_tmp4D8.real_const=
1;_tmp4D8.loc=loc;_tmp4D8;});_tmp4D7;});break;}case 95: _LL25F: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp4D9;(_tmp4D9.YY25).tag=29;(_tmp4D9.YY25).f1=({struct
Cyc_Absyn_Tqual _tmp4DA;_tmp4DA.print_const=0;_tmp4DA.q_volatile=1;_tmp4DA.q_restrict=
0;_tmp4DA.real_const=0;_tmp4DA.loc=0;_tmp4DA;});_tmp4D9;});break;case 96: _LL260:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp4DB;(_tmp4DB.YY25).tag=
29;(_tmp4DB.YY25).f1=({struct Cyc_Absyn_Tqual _tmp4DC;_tmp4DC.print_const=0;
_tmp4DC.q_volatile=0;_tmp4DC.q_restrict=1;_tmp4DC.real_const=0;_tmp4DC.loc=0;
_tmp4DC;});_tmp4DB;});break;case 97: _LL261: yyval=(union Cyc_YYSTYPE_union)({union
Cyc_YYSTYPE_union _tmp4DD;(_tmp4DD.YY23).tag=27;(_tmp4DD.YY23).f1=(void*)((void*)({
struct Cyc_Parse_Decl_spec_struct*_tmp4DE=_cycalloc(sizeof(*_tmp4DE));_tmp4DE[0]=({
struct Cyc_Parse_Decl_spec_struct _tmp4DF;_tmp4DF.tag=5;_tmp4DF.f1=({struct Cyc_Absyn_Decl*
_tmp4E0=_cycalloc(sizeof(*_tmp4E0));_tmp4E0->r=(void*)((void*)({struct Cyc_Absyn_Enum_d_struct*
_tmp4E1=_cycalloc(sizeof(*_tmp4E1));_tmp4E1[0]=({struct Cyc_Absyn_Enum_d_struct
_tmp4E2;_tmp4E2.tag=8;_tmp4E2.f1=({struct Cyc_Absyn_Enumdecl*_tmp4E3=_cycalloc(
sizeof(*_tmp4E3));_tmp4E3->sc=(void*)((void*)2);_tmp4E3->name=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp4E3->fields=({
struct Cyc_Core_Opt*_tmp4E4=_cycalloc(sizeof(*_tmp4E4));_tmp4E4->v=Cyc_yyget_YY48(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp4E4;});_tmp4E3;});
_tmp4E2;});_tmp4E1;}));_tmp4E0->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp4E0;});
_tmp4DF;});_tmp4DE;}));_tmp4DD;});break;case 98: _LL262: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp4E5;(_tmp4E5.YY23).tag=27;(_tmp4E5.YY23).f1=(void*)Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_EnumType_struct*_tmp4E6=_cycalloc(sizeof(*_tmp4E6));
_tmp4E6[0]=({struct Cyc_Absyn_EnumType_struct _tmp4E7;_tmp4E7.tag=12;_tmp4E7.f1=
Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
_tmp4E7.f2=0;_tmp4E7;});_tmp4E6;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp4E5;});
break;case 99: _LL263: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp4E8;(_tmp4E8.YY23).tag=27;(_tmp4E8.YY23).f1=(void*)((void*)({struct Cyc_Parse_Type_spec_struct*
_tmp4E9=_cycalloc(sizeof(*_tmp4E9));_tmp4E9[0]=({struct Cyc_Parse_Type_spec_struct
_tmp4EA;_tmp4EA.tag=4;_tmp4EA.f1=(void*)((void*)({struct Cyc_Absyn_AnonEnumType_struct*
_tmp4EB=_cycalloc(sizeof(*_tmp4EB));_tmp4EB[0]=({struct Cyc_Absyn_AnonEnumType_struct
_tmp4EC;_tmp4EC.tag=13;_tmp4EC.f1=Cyc_yyget_YY48(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp4EC;});_tmp4EB;}));
_tmp4EA.f2=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp4EA;});_tmp4E9;}));_tmp4E8;});break;case 100:
_LL264: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp4ED;(_tmp4ED.YY47).tag=
51;(_tmp4ED.YY47).f1=({struct Cyc_Absyn_Enumfield*_tmp4EE=_cycalloc(sizeof(*
_tmp4EE));_tmp4EE->name=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp4EE->tag=0;_tmp4EE->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp4EE;});
_tmp4ED;});break;case 101: _LL265: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp4EF;(_tmp4EF.YY47).tag=51;(_tmp4EF.YY47).f1=({struct Cyc_Absyn_Enumfield*
_tmp4F0=_cycalloc(sizeof(*_tmp4F0));_tmp4F0->name=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp4F0->tag=(struct Cyc_Absyn_Exp*)
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp4F0->loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp4F0;});_tmp4EF;});break;case 102: _LL266: yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp4F1;(_tmp4F1.YY48).tag=52;(
_tmp4F1.YY48).f1=({struct Cyc_List_List*_tmp4F2=_cycalloc(sizeof(*_tmp4F2));
_tmp4F2->hd=Cyc_yyget_YY47(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp4F2->tl=0;_tmp4F2;});_tmp4F1;});break;case 103: _LL267: yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp4F3;(_tmp4F3.YY48).tag=52;(
_tmp4F3.YY48).f1=({struct Cyc_List_List*_tmp4F4=_cycalloc(sizeof(*_tmp4F4));
_tmp4F4->hd=Cyc_yyget_YY47(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp4F4->tl=0;_tmp4F4;});_tmp4F3;});break;case 104: _LL268:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp4F5;(_tmp4F5.YY48).tag=
52;(_tmp4F5.YY48).f1=({struct Cyc_List_List*_tmp4F6=_cycalloc(sizeof(*_tmp4F6));
_tmp4F6->hd=Cyc_yyget_YY47(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]);_tmp4F6->tl=Cyc_yyget_YY48(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp4F6;});_tmp4F5;});
break;case 105: _LL269: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp4F7;(_tmp4F7.YY23).tag=27;(_tmp4F7.YY23).f1=(void*)Cyc_Parse_type_spec((void*)({
struct Cyc_Absyn_AnonAggrType_struct*_tmp4F8=_cycalloc(sizeof(*_tmp4F8));_tmp4F8[
0]=({struct Cyc_Absyn_AnonAggrType_struct _tmp4F9;_tmp4F9.tag=11;_tmp4F9.f1=(void*)
Cyc_yyget_YY24(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);
_tmp4F9.f2=Cyc_yyget_YY26(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset
- 1)]);_tmp4F9;});_tmp4F8;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp4F7;});
break;case 106: _LL26A: {struct Cyc_List_List*_tmp4FA=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).last_line),Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]));struct Cyc_List_List*
_tmp4FB=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line),Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]));yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp4FC;(_tmp4FC.YY23).tag=27;(_tmp4FC.YY23).f1=(void*)((void*)({struct Cyc_Parse_Decl_spec_struct*
_tmp4FD=_cycalloc(sizeof(*_tmp4FD));_tmp4FD[0]=({struct Cyc_Parse_Decl_spec_struct
_tmp4FE;_tmp4FE.tag=5;_tmp4FE.f1=Cyc_Absyn_aggr_decl(Cyc_yyget_YY24(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 8)]),(void*)2,Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 7)]),_tmp4FA,(struct Cyc_Absyn_AggrdeclImpl*)
Cyc_Absyn_aggrdecl_impl(_tmp4FB,Cyc_yyget_YY50(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY26(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY31(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 5)])),0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 8)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp4FE;});
_tmp4FD;}));_tmp4FC;});break;}case 107: _LL26B: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp4FF;(_tmp4FF.YY23).tag=27;(_tmp4FF.YY23).f1=(void*)Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_AggrType_struct*_tmp500=_cycalloc(sizeof(*_tmp500));
_tmp500[0]=({struct Cyc_Absyn_AggrType_struct _tmp501;_tmp501.tag=10;_tmp501.f1=({
struct Cyc_Absyn_AggrInfo _tmp502;_tmp502.aggr_info=(union Cyc_Absyn_AggrInfoU_union)({
union Cyc_Absyn_AggrInfoU_union _tmp503;(_tmp503.UnknownAggr).tag=0;(_tmp503.UnknownAggr).f1=(
void*)Cyc_yyget_YY24(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);(
_tmp503.UnknownAggr).f2=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_tmp503;});_tmp502.targs=Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp502;});_tmp501;});
_tmp500;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp4FF;});break;case 108: _LL26C: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp504;(_tmp504.YY31).tag=35;(_tmp504.YY31).f1=0;_tmp504;});
break;case 109: _LL26D: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp505;(_tmp505.YY31).tag=35;(_tmp505.YY31).f1=1;_tmp505;});break;case 110:
_LL26E: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp506;(_tmp506.YY40).tag=
44;(_tmp506.YY40).f1=0;_tmp506;});break;case 111: _LL26F: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp507;(_tmp507.YY40).tag=44;(_tmp507.YY40).f1=Cyc_List_imp_rev(
Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));
_tmp507;});break;case 112: _LL270: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp508;(_tmp508.YY24).tag=28;(_tmp508.YY24).f1=(void*)((void*)0);_tmp508;});
break;case 113: _LL271: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp509;(_tmp509.YY24).tag=28;(_tmp509.YY24).f1=(void*)((void*)1);_tmp509;});
break;case 114: _LL272: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp50A;(_tmp50A.YY26).tag=30;(_tmp50A.YY26).f1=0;_tmp50A;});break;case 115:
_LL273: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp50B;(_tmp50B.YY26).tag=
30;(_tmp50B.YY26).f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_flatten)(((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY27(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])));_tmp50B;});break;
case 116: _LL274: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp50C;(
_tmp50C.YY27).tag=31;(_tmp50C.YY27).f1=({struct Cyc_List_List*_tmp50D=_cycalloc(
sizeof(*_tmp50D));_tmp50D->hd=Cyc_yyget_YY26(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp50D->tl=0;_tmp50D;});_tmp50C;});break;case 117: _LL275:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp50E;(_tmp50E.YY27).tag=
31;(_tmp50E.YY27).f1=({struct Cyc_List_List*_tmp50F=_cycalloc(sizeof(*_tmp50F));
_tmp50F->hd=Cyc_yyget_YY26(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp50F->tl=Cyc_yyget_YY27(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_tmp50F;});_tmp50E;});break;case 118: _LL276: yyval=(union
Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp510;(_tmp510.YY21).tag=25;(_tmp510.YY21).f1=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY21(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp510;});break;case
119: _LL277: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp511;(
_tmp511.YY21).tag=25;(_tmp511.YY21).f1=({struct Cyc_List_List*_tmp512=_cycalloc(
sizeof(*_tmp512));_tmp512->hd=Cyc_yyget_YY20(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp512->tl=0;_tmp512;});_tmp511;});break;case 120: _LL278:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp513;(_tmp513.YY21).tag=
25;(_tmp513.YY21).f1=({struct Cyc_List_List*_tmp514=_cycalloc(sizeof(*_tmp514));
_tmp514->hd=Cyc_yyget_YY20(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp514->tl=Cyc_yyget_YY21(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]);_tmp514;});_tmp513;});break;case 121: _LL279: yyval=(union
Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp515;(_tmp515.YY20).tag=24;(_tmp515.YY20).f1=({
struct _tuple15*_tmp516=_cycalloc(sizeof(*_tmp516));_tmp516->f1=Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp516->f2=0;_tmp516;});
_tmp515;});break;case 122: _LL27A: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp517;(_tmp517.YY20).tag=24;(_tmp517.YY20).f1=({struct _tuple15*_tmp518=
_cycalloc(sizeof(*_tmp518));_tmp518->f1=Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp518->f2=(struct Cyc_Absyn_Exp*)
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp518;});
_tmp517;});break;case 123: _LL27B: {struct _RegionHandle _tmp519=_new_region("temp");
struct _RegionHandle*temp=& _tmp519;_push_region(temp);{struct _tuple16 _tmp51B;
struct Cyc_Absyn_Tqual _tmp51C;struct Cyc_List_List*_tmp51D;struct Cyc_List_List*
_tmp51E;struct _tuple16*_tmp51A=Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp51B=*_tmp51A;_tmp51C=
_tmp51B.f1;_tmp51D=_tmp51B.f2;_tmp51E=_tmp51B.f3;if(_tmp51C.loc == 0)_tmp51C.loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line);{struct Cyc_List_List*_tmp520;struct Cyc_List_List*
_tmp521;struct _tuple0 _tmp51F=((struct _tuple0(*)(struct _RegionHandle*r1,struct
_RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(temp,temp,Cyc_yyget_YY21(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp520=_tmp51F.f1;
_tmp521=_tmp51F.f2;{void*_tmp522=Cyc_Parse_speclist2typ(_tmp51D,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line));struct Cyc_List_List*
_tmp523=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(temp,temp,Cyc_Parse_apply_tmss(
temp,_tmp51C,_tmp522,_tmp520,_tmp51E),_tmp521);yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp524;(_tmp524.YY26).tag=30;(_tmp524.YY26).f1=((struct
Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Position_Segment*,
struct _tuple8*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(
Cyc_Parse_make_aggr_field,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),_tmp523);
_tmp524;});}}}_npop_handler(0);break;;_pop_region(temp);}case 124: _LL27C: yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp525;(_tmp525.YY35).tag=39;(
_tmp525.YY35).f1=({struct _tuple16*_tmp526=_cycalloc(sizeof(*_tmp526));_tmp526->f1=
Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp526->f2=({
struct Cyc_List_List*_tmp527=_cycalloc(sizeof(*_tmp527));_tmp527->hd=(void*)Cyc_yyget_YY23(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp527->tl=0;_tmp527;});
_tmp526->f3=0;_tmp526;});_tmp525;});break;case 125: _LL27D: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp528;(_tmp528.YY35).tag=39;(_tmp528.YY35).f1=({struct
_tuple16*_tmp529=_cycalloc(sizeof(*_tmp529));_tmp529->f1=(*Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f1;_tmp529->f2=({struct Cyc_List_List*
_tmp52A=_cycalloc(sizeof(*_tmp52A));_tmp52A->hd=(void*)Cyc_yyget_YY23(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp52A->tl=(*Cyc_yyget_YY35(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f2;_tmp52A;});_tmp529->f3=(*
Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f3;
_tmp529;});_tmp528;});break;case 126: _LL27E: yyval=(union Cyc_YYSTYPE_union)({union
Cyc_YYSTYPE_union _tmp52B;(_tmp52B.YY35).tag=39;(_tmp52B.YY35).f1=({struct
_tuple16*_tmp52C=_cycalloc(sizeof(*_tmp52C));_tmp52C->f1=Cyc_yyget_YY25(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp52C->f2=0;_tmp52C->f3=0;
_tmp52C;});_tmp52B;});break;case 127: _LL27F: yyval=(union Cyc_YYSTYPE_union)({union
Cyc_YYSTYPE_union _tmp52D;(_tmp52D.YY35).tag=39;(_tmp52D.YY35).f1=({struct
_tuple16*_tmp52E=_cycalloc(sizeof(*_tmp52E));_tmp52E->f1=Cyc_Absyn_combine_tqual(
Cyc_yyget_YY25(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(*
Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f1);
_tmp52E->f2=(*Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)])).f2;_tmp52E->f3=(*Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f3;_tmp52E;});_tmp52D;});
break;case 128: _LL280: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp52F;(_tmp52F.YY35).tag=39;(_tmp52F.YY35).f1=({struct _tuple16*_tmp530=
_cycalloc(sizeof(*_tmp530));_tmp530->f1=Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp530->f2=0;
_tmp530->f3=Cyc_yyget_YY45(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp530;});_tmp52F;});break;case 129: _LL281: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp531;(_tmp531.YY35).tag=39;(_tmp531.YY35).f1=({struct
_tuple16*_tmp532=_cycalloc(sizeof(*_tmp532));_tmp532->f1=(*Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f1;_tmp532->f2=(*Cyc_yyget_YY35(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f2;_tmp532->f3=Cyc_List_append(
Cyc_yyget_YY45(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(*
Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f3);
_tmp532;});_tmp531;});break;case 130: _LL282: yyval=(union Cyc_YYSTYPE_union)({union
Cyc_YYSTYPE_union _tmp533;(_tmp533.YY35).tag=39;(_tmp533.YY35).f1=({struct
_tuple16*_tmp534=_cycalloc(sizeof(*_tmp534));_tmp534->f1=Cyc_Absyn_empty_tqual(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp534->f2=({struct Cyc_List_List*_tmp535=_cycalloc(
sizeof(*_tmp535));_tmp535->hd=(void*)Cyc_yyget_YY23(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp535->tl=0;_tmp535;});
_tmp534->f3=0;_tmp534;});_tmp533;});break;case 131: _LL283: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp536;(_tmp536.YY35).tag=39;(_tmp536.YY35).f1=({struct
_tuple16*_tmp537=_cycalloc(sizeof(*_tmp537));_tmp537->f1=(*Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f1;_tmp537->f2=({struct Cyc_List_List*
_tmp538=_cycalloc(sizeof(*_tmp538));_tmp538->hd=(void*)Cyc_yyget_YY23(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp538->tl=(*Cyc_yyget_YY35(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f2;_tmp538;});_tmp537->f3=(*
Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f3;
_tmp537;});_tmp536;});break;case 132: _LL284: yyval=(union Cyc_YYSTYPE_union)({union
Cyc_YYSTYPE_union _tmp539;(_tmp539.YY35).tag=39;(_tmp539.YY35).f1=({struct
_tuple16*_tmp53A=_cycalloc(sizeof(*_tmp53A));_tmp53A->f1=Cyc_yyget_YY25(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp53A->f2=0;_tmp53A->f3=0;
_tmp53A;});_tmp539;});break;case 133: _LL285: yyval=(union Cyc_YYSTYPE_union)({union
Cyc_YYSTYPE_union _tmp53B;(_tmp53B.YY35).tag=39;(_tmp53B.YY35).f1=({struct
_tuple16*_tmp53C=_cycalloc(sizeof(*_tmp53C));_tmp53C->f1=Cyc_Absyn_combine_tqual(
Cyc_yyget_YY25(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(*
Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f1);
_tmp53C->f2=(*Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)])).f2;_tmp53C->f3=(*Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f3;_tmp53C;});_tmp53B;});
break;case 134: _LL286: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp53D;(_tmp53D.YY35).tag=39;(_tmp53D.YY35).f1=({struct _tuple16*_tmp53E=
_cycalloc(sizeof(*_tmp53E));_tmp53E->f1=Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp53E->f2=0;
_tmp53E->f3=Cyc_yyget_YY45(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp53E;});_tmp53D;});break;case 135: _LL287: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp53F;(_tmp53F.YY35).tag=39;(_tmp53F.YY35).f1=({struct
_tuple16*_tmp540=_cycalloc(sizeof(*_tmp540));_tmp540->f1=(*Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f1;_tmp540->f2=(*Cyc_yyget_YY35(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f2;_tmp540->f3=Cyc_List_append(
Cyc_yyget_YY45(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(*
Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f3);
_tmp540;});_tmp53F;});break;case 136: _LL288: yyval=(union Cyc_YYSTYPE_union)({union
Cyc_YYSTYPE_union _tmp541;(_tmp541.YY21).tag=25;(_tmp541.YY21).f1=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY21(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp541;});break;case 137:
_LL289: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp542;(_tmp542.YY21).tag=
25;(_tmp542.YY21).f1=({struct Cyc_List_List*_tmp543=_cycalloc(sizeof(*_tmp543));
_tmp543->hd=Cyc_yyget_YY20(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp543->tl=0;_tmp543;});_tmp542;});break;case 138: _LL28A: yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp544;(_tmp544.YY21).tag=25;(
_tmp544.YY21).f1=({struct Cyc_List_List*_tmp545=_cycalloc(sizeof(*_tmp545));
_tmp545->hd=Cyc_yyget_YY20(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp545->tl=Cyc_yyget_YY21(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]);_tmp545;});_tmp544;});break;case 139: _LL28B: yyval=(union
Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp546;(_tmp546.YY20).tag=24;(_tmp546.YY20).f1=({
struct _tuple15*_tmp547=_cycalloc(sizeof(*_tmp547));_tmp547->f1=Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp547->f2=0;_tmp547;});
_tmp546;});break;case 140: _LL28C: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp548;(_tmp548.YY20).tag=24;(_tmp548.YY20).f1=({struct _tuple15*_tmp549=
_cycalloc(sizeof(*_tmp549));_tmp549->f1=({struct Cyc_Parse_Declarator*_tmp54A=
_cycalloc(sizeof(*_tmp54A));_tmp54A->id=({struct _tuple1*_tmp54B=_cycalloc(
sizeof(*_tmp54B));_tmp54B->f1=(union Cyc_Absyn_Nmspace_union)({union Cyc_Absyn_Nmspace_union
_tmp54D;(_tmp54D.Rel_n).tag=1;(_tmp54D.Rel_n).f1=0;_tmp54D;});_tmp54B->f2=
_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"",sizeof(char),1);
_tmp54B;});_tmp54A->tms=0;_tmp54A;});_tmp549->f2=(struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp549;});_tmp548;});
break;case 141: _LL28D: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp54E;(_tmp54E.YY20).tag=24;(_tmp54E.YY20).f1=({struct _tuple15*_tmp54F=
_cycalloc(sizeof(*_tmp54F));_tmp54F->f1=Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp54F->f2=(struct Cyc_Absyn_Exp*)
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp54F;});
_tmp54E;});break;case 142: _LL28E: {int extensible=Cyc_yyget_YY31(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 7)]) || Cyc_yyget_YY31(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);struct Cyc_List_List*
_tmp550=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 4)]).last_line),Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 4)]));yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp551;(_tmp551.YY23).tag=27;(_tmp551.YY23).f1=(void*)((void*)({struct Cyc_Parse_Decl_spec_struct*
_tmp552=_cycalloc(sizeof(*_tmp552));_tmp552[0]=({struct Cyc_Parse_Decl_spec_struct
_tmp553;_tmp553.tag=5;_tmp553.f1=Cyc_Absyn_tunion_decl((void*)2,Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]),_tmp550,({struct Cyc_Core_Opt*
_tmp554=_cycalloc(sizeof(*_tmp554));_tmp554->v=Cyc_yyget_YY34(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp554;}),extensible,
Cyc_yyget_YY31(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 8)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 8)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp553;});
_tmp552;}));_tmp551;});break;}case 143: _LL28F: {int extensible=Cyc_yyget_YY31(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]) || Cyc_yyget_YY31(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp555;(_tmp555.YY23).tag=27;(_tmp555.YY23).f1=(void*)Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_TunionType_struct*_tmp556=_cycalloc(sizeof(*_tmp556));
_tmp556[0]=({struct Cyc_Absyn_TunionType_struct _tmp557;_tmp557.tag=2;_tmp557.f1=({
struct Cyc_Absyn_TunionInfo _tmp558;_tmp558.tunion_info=(union Cyc_Absyn_TunionInfoU_union)({
union Cyc_Absyn_TunionInfoU_union _tmp559;(_tmp559.UnknownTunion).tag=0;(_tmp559.UnknownTunion).f1=({
struct Cyc_Absyn_UnknownTunionInfo _tmp55A;_tmp55A.name=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp55A.is_xtunion=
extensible;_tmp55A.is_flat=Cyc_yyget_YY31(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 5)]);_tmp55A;});_tmp559;});_tmp558.targs=Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp558.rgn=Cyc_yyget_YY49(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp558;});_tmp557;});
_tmp556;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp555;});break;}case 144: _LL290: {int extensible=Cyc_yyget_YY31(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]) || Cyc_yyget_YY31(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp55B;(_tmp55B.YY23).tag=27;(_tmp55B.YY23).f1=(void*)Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_TunionFieldType_struct*_tmp55C=_cycalloc(sizeof(*_tmp55C));
_tmp55C[0]=({struct Cyc_Absyn_TunionFieldType_struct _tmp55D;_tmp55D.tag=3;_tmp55D.f1=({
struct Cyc_Absyn_TunionFieldInfo _tmp55E;_tmp55E.field_info=(union Cyc_Absyn_TunionFieldInfoU_union)({
union Cyc_Absyn_TunionFieldInfoU_union _tmp55F;(_tmp55F.UnknownTunionfield).tag=0;(
_tmp55F.UnknownTunionfield).f1=({struct Cyc_Absyn_UnknownTunionFieldInfo _tmp560;
_tmp560.tunion_name=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 4)]);_tmp560.field_name=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp560.is_xtunion=
extensible;_tmp560;});_tmp55F;});_tmp55E.targs=Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp55E;});_tmp55D;});
_tmp55C;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 7)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp55B;});break;}case 145: _LL291: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp561;(_tmp561.YY31).tag=35;(_tmp561.YY31).f1=1;_tmp561;});
break;case 146: _LL292: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp562;(_tmp562.YY31).tag=35;(_tmp562.YY31).f1=0;_tmp562;});break;case 147:
_LL293: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
148: _LL294: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp563;(
_tmp563.YY31).tag=35;(_tmp563.YY31).f1=0;_tmp563;});break;case 149: _LL295: yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp564;(_tmp564.YY31).tag=35;(
_tmp564.YY31).f1=1;_tmp564;});break;case 150: _LL296: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp565;(_tmp565.YY31).tag=35;(_tmp565.YY31).f1=0;_tmp565;});
break;case 151: _LL297: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp566;(_tmp566.YY31).tag=35;(_tmp566.YY31).f1=1;_tmp566;});break;case 152:
_LL298: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp567;(_tmp567.YY34).tag=
38;(_tmp567.YY34).f1=({struct Cyc_List_List*_tmp568=_cycalloc(sizeof(*_tmp568));
_tmp568->hd=Cyc_yyget_YY33(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp568->tl=0;_tmp568;});_tmp567;});break;case 153: _LL299: yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp569;(_tmp569.YY34).tag=38;(
_tmp569.YY34).f1=({struct Cyc_List_List*_tmp56A=_cycalloc(sizeof(*_tmp56A));
_tmp56A->hd=Cyc_yyget_YY33(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp56A->tl=0;_tmp56A;});_tmp569;});break;case 154: _LL29A:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp56B;(_tmp56B.YY34).tag=
38;(_tmp56B.YY34).f1=({struct Cyc_List_List*_tmp56C=_cycalloc(sizeof(*_tmp56C));
_tmp56C->hd=Cyc_yyget_YY33(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]);_tmp56C->tl=Cyc_yyget_YY34(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp56C;});_tmp56B;});
break;case 155: _LL29B: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp56D;(_tmp56D.YY34).tag=38;(_tmp56D.YY34).f1=({struct Cyc_List_List*_tmp56E=
_cycalloc(sizeof(*_tmp56E));_tmp56E->hd=Cyc_yyget_YY33(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp56E->tl=Cyc_yyget_YY34(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp56E;});_tmp56D;});
break;case 156: _LL29C: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp56F;(_tmp56F.YY32).tag=36;(_tmp56F.YY32).f1=(void*)((void*)2);_tmp56F;});
break;case 157: _LL29D: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp570;(_tmp570.YY32).tag=36;(_tmp570.YY32).f1=(void*)((void*)3);_tmp570;});
break;case 158: _LL29E: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp571;(_tmp571.YY32).tag=36;(_tmp571.YY32).f1=(void*)((void*)0);_tmp571;});
break;case 159: _LL29F: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp572;(_tmp572.YY33).tag=37;(_tmp572.YY33).f1=({struct Cyc_Absyn_Tunionfield*
_tmp573=_cycalloc(sizeof(*_tmp573));_tmp573->name=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp573->typs=0;_tmp573->loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp573->sc=(void*)Cyc_yyget_YY32(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp573;});_tmp572;});
break;case 160: _LL2A0: {struct Cyc_List_List*_tmp574=((struct Cyc_List_List*(*)(
struct _tuple9*(*f)(struct Cyc_Position_Segment*,struct _tuple2*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_get_tqual_typ,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp575;(_tmp575.YY33).tag=37;(_tmp575.YY33).f1=({struct
Cyc_Absyn_Tunionfield*_tmp576=_cycalloc(sizeof(*_tmp576));_tmp576->name=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp576->typs=
_tmp574;_tmp576->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp576->sc=(
void*)Cyc_yyget_YY32(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);
_tmp576;});_tmp575;});break;}case 161: _LL2A1: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 162: _LL2A2: yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp577;(_tmp577.YY29).tag=33;(
_tmp577.YY29).f1=({struct Cyc_Parse_Declarator*_tmp578=_cycalloc(sizeof(*_tmp578));
_tmp578->id=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->id;_tmp578->tms=Cyc_List_imp_append(Cyc_yyget_YY28(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->tms);_tmp578;});_tmp577;});
break;case 163: _LL2A3: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 164: _LL2A4: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp579;(_tmp579.YY29).tag=33;(_tmp579.YY29).f1=({struct Cyc_Parse_Declarator*
_tmp57A=_cycalloc(sizeof(*_tmp57A));_tmp57A->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->id;_tmp57A->tms=Cyc_List_imp_append(
Cyc_yyget_YY28(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tms);_tmp57A;});
_tmp579;});break;case 165: _LL2A5: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp57B;(_tmp57B.YY29).tag=33;(_tmp57B.YY29).f1=({struct Cyc_Parse_Declarator*
_tmp57C=_cycalloc(sizeof(*_tmp57C));_tmp57C->id=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp57C->tms=0;_tmp57C;});
_tmp57B;});break;case 166: _LL2A6: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)];break;case 167: _LL2A7: {struct Cyc_Parse_Declarator*_tmp57D=Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp57D->tms=({
struct Cyc_List_List*_tmp57E=_cycalloc(sizeof(*_tmp57E));_tmp57E->hd=(void*)((
void*)({struct Cyc_Absyn_Attributes_mod_struct*_tmp57F=_cycalloc(sizeof(*_tmp57F));
_tmp57F[0]=({struct Cyc_Absyn_Attributes_mod_struct _tmp580;_tmp580.tag=5;_tmp580.f1=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line);_tmp580.f2=Cyc_yyget_YY45(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp580;});_tmp57F;}));
_tmp57E->tl=_tmp57D->tms;_tmp57E;});yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)];break;}case 168: _LL2A8: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp581;(_tmp581.YY29).tag=33;(_tmp581.YY29).f1=({struct
Cyc_Parse_Declarator*_tmp582=_cycalloc(sizeof(*_tmp582));_tmp582->id=(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id;_tmp582->tms=({
struct Cyc_List_List*_tmp583=_cycalloc(sizeof(*_tmp583));_tmp583->hd=(void*)((
void*)({struct Cyc_Absyn_Carray_mod_struct*_tmp584=_cycalloc(sizeof(*_tmp584));
_tmp584[0]=({struct Cyc_Absyn_Carray_mod_struct _tmp585;_tmp585.tag=0;_tmp585.f1=
Cyc_yyget_YY51(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp585.f2=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp585;});_tmp584;}));_tmp583->tl=(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp583;});
_tmp582;});_tmp581;});break;case 169: _LL2A9: yyval=(union Cyc_YYSTYPE_union)({union
Cyc_YYSTYPE_union _tmp586;(_tmp586.YY29).tag=33;(_tmp586.YY29).f1=({struct Cyc_Parse_Declarator*
_tmp587=_cycalloc(sizeof(*_tmp587));_tmp587->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->id;_tmp587->tms=({
struct Cyc_List_List*_tmp588=_cycalloc(sizeof(*_tmp588));_tmp588->hd=(void*)((
void*)({struct Cyc_Absyn_ConstArray_mod_struct*_tmp589=_cycalloc(sizeof(*_tmp589));
_tmp589[0]=({struct Cyc_Absyn_ConstArray_mod_struct _tmp58A;_tmp58A.tag=1;_tmp58A.f1=
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);
_tmp58A.f2=Cyc_yyget_YY51(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
_tmp58A.f3=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp58A;});_tmp589;}));_tmp588->tl=(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->tms;_tmp588;});
_tmp587;});_tmp586;});break;case 170: _LL2AA: {struct _tuple17 _tmp58C;struct Cyc_List_List*
_tmp58D;int _tmp58E;struct Cyc_Absyn_VarargInfo*_tmp58F;struct Cyc_Core_Opt*_tmp590;
struct Cyc_List_List*_tmp591;struct _tuple17*_tmp58B=Cyc_yyget_YY39(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp58C=*_tmp58B;_tmp58D=
_tmp58C.f1;_tmp58E=_tmp58C.f2;_tmp58F=_tmp58C.f3;_tmp590=_tmp58C.f4;_tmp591=
_tmp58C.f5;yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp592;(
_tmp592.YY29).tag=33;(_tmp592.YY29).f1=({struct Cyc_Parse_Declarator*_tmp593=
_cycalloc(sizeof(*_tmp593));_tmp593->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id;_tmp593->tms=({
struct Cyc_List_List*_tmp594=_cycalloc(sizeof(*_tmp594));_tmp594->hd=(void*)((
void*)({struct Cyc_Absyn_Function_mod_struct*_tmp595=_cycalloc(sizeof(*_tmp595));
_tmp595[0]=({struct Cyc_Absyn_Function_mod_struct _tmp596;_tmp596.tag=3;_tmp596.f1=(
void*)((void*)({struct Cyc_Absyn_WithTypes_struct*_tmp597=_cycalloc(sizeof(*
_tmp597));_tmp597[0]=({struct Cyc_Absyn_WithTypes_struct _tmp598;_tmp598.tag=1;
_tmp598.f1=_tmp58D;_tmp598.f2=_tmp58E;_tmp598.f3=_tmp58F;_tmp598.f4=_tmp590;
_tmp598.f5=_tmp591;_tmp598;});_tmp597;}));_tmp596;});_tmp595;}));_tmp594->tl=(
Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;
_tmp594;});_tmp593;});_tmp592;});break;}case 171: _LL2AB: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp599;(_tmp599.YY29).tag=33;(_tmp599.YY29).f1=({struct
Cyc_Parse_Declarator*_tmp59A=_cycalloc(sizeof(*_tmp59A));_tmp59A->id=(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->id;_tmp59A->tms=({
struct Cyc_List_List*_tmp59B=_cycalloc(sizeof(*_tmp59B));_tmp59B->hd=(void*)((
void*)({struct Cyc_Absyn_Function_mod_struct*_tmp59C=_cycalloc(sizeof(*_tmp59C));
_tmp59C[0]=({struct Cyc_Absyn_Function_mod_struct _tmp59D;_tmp59D.tag=3;_tmp59D.f1=(
void*)((void*)({struct Cyc_Absyn_WithTypes_struct*_tmp59E=_cycalloc(sizeof(*
_tmp59E));_tmp59E[0]=({struct Cyc_Absyn_WithTypes_struct _tmp59F;_tmp59F.tag=1;
_tmp59F.f1=0;_tmp59F.f2=0;_tmp59F.f3=0;_tmp59F.f4=Cyc_yyget_YY49(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp59F.f5=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp59F;});_tmp59E;}));
_tmp59D;});_tmp59C;}));_tmp59B->tl=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->tms;_tmp59B;});_tmp59A;});
_tmp599;});break;case 172: _LL2AC: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp5A0;(_tmp5A0.YY29).tag=33;(_tmp5A0.YY29).f1=({struct Cyc_Parse_Declarator*
_tmp5A1=_cycalloc(sizeof(*_tmp5A1));_tmp5A1->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id;_tmp5A1->tms=({
struct Cyc_List_List*_tmp5A2=_cycalloc(sizeof(*_tmp5A2));_tmp5A2->hd=(void*)((
void*)({struct Cyc_Absyn_Function_mod_struct*_tmp5A3=_cycalloc(sizeof(*_tmp5A3));
_tmp5A3[0]=({struct Cyc_Absyn_Function_mod_struct _tmp5A4;_tmp5A4.tag=3;_tmp5A4.f1=(
void*)((void*)({struct Cyc_Absyn_NoTypes_struct*_tmp5A5=_cycalloc(sizeof(*_tmp5A5));
_tmp5A5[0]=({struct Cyc_Absyn_NoTypes_struct _tmp5A6;_tmp5A6.tag=0;_tmp5A6.f1=Cyc_yyget_YY36(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp5A6.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp5A6;});
_tmp5A5;}));_tmp5A4;});_tmp5A3;}));_tmp5A2->tl=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp5A2;});_tmp5A1;});
_tmp5A0;});break;case 173: _LL2AD: {struct Cyc_List_List*_tmp5A7=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),Cyc_List_imp_rev(
Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp5A8;(_tmp5A8.YY29).tag=
33;(_tmp5A8.YY29).f1=({struct Cyc_Parse_Declarator*_tmp5A9=_cycalloc(sizeof(*
_tmp5A9));_tmp5A9->id=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]))->id;_tmp5A9->tms=({struct Cyc_List_List*_tmp5AA=_cycalloc(
sizeof(*_tmp5AA));_tmp5AA->hd=(void*)((void*)({struct Cyc_Absyn_TypeParams_mod_struct*
_tmp5AB=_cycalloc(sizeof(*_tmp5AB));_tmp5AB[0]=({struct Cyc_Absyn_TypeParams_mod_struct
_tmp5AC;_tmp5AC.tag=4;_tmp5AC.f1=_tmp5A7;_tmp5AC.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp5AC.f3=0;
_tmp5AC;});_tmp5AB;}));_tmp5AA->tl=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp5AA;});_tmp5A9;});
_tmp5A8;});break;}case 174: _LL2AE: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp5AD;(_tmp5AD.YY29).tag=33;(_tmp5AD.YY29).f1=({struct Cyc_Parse_Declarator*
_tmp5AE=_cycalloc(sizeof(*_tmp5AE));_tmp5AE->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]))->id;_tmp5AE->tms=({
struct Cyc_List_List*_tmp5AF=_cycalloc(sizeof(*_tmp5AF));_tmp5AF->hd=(void*)((
void*)({struct Cyc_Absyn_Attributes_mod_struct*_tmp5B0=_cycalloc(sizeof(*_tmp5B0));
_tmp5B0[0]=({struct Cyc_Absyn_Attributes_mod_struct _tmp5B1;_tmp5B1.tag=5;_tmp5B1.f1=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp5B1.f2=Cyc_yyget_YY45(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5B1;});_tmp5B0;}));
_tmp5AF->tl=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]))->tms;_tmp5AF;});_tmp5AE;});_tmp5AD;});break;case 175: _LL2AF:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp5B2;(_tmp5B2.YY29).tag=
33;(_tmp5B2.YY29).f1=({struct Cyc_Parse_Declarator*_tmp5B3=_cycalloc(sizeof(*
_tmp5B3));_tmp5B3->id=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp5B3->tms=0;_tmp5B3;});_tmp5B2;});break;case 176: _LL2B0:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp5B4;(_tmp5B4.YY29).tag=
33;(_tmp5B4.YY29).f1=({struct Cyc_Parse_Declarator*_tmp5B5=_cycalloc(sizeof(*
_tmp5B5));_tmp5B5->id=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp5B5->tms=0;_tmp5B5;});_tmp5B4;});break;case 177: _LL2B1:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;case 178:
_LL2B2: {struct Cyc_Parse_Declarator*_tmp5B6=Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp5B6->tms=({struct Cyc_List_List*
_tmp5B7=_cycalloc(sizeof(*_tmp5B7));_tmp5B7->hd=(void*)((void*)({struct Cyc_Absyn_Attributes_mod_struct*
_tmp5B8=_cycalloc(sizeof(*_tmp5B8));_tmp5B8[0]=({struct Cyc_Absyn_Attributes_mod_struct
_tmp5B9;_tmp5B9.tag=5;_tmp5B9.f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line);_tmp5B9.f2=
Cyc_yyget_YY45(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);
_tmp5B9;});_tmp5B8;}));_tmp5B7->tl=_tmp5B6->tms;_tmp5B7;});yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;}case 179: _LL2B3:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp5BA;(_tmp5BA.YY29).tag=
33;(_tmp5BA.YY29).f1=({struct Cyc_Parse_Declarator*_tmp5BB=_cycalloc(sizeof(*
_tmp5BB));_tmp5BB->id=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]))->id;_tmp5BB->tms=({struct Cyc_List_List*_tmp5BC=_cycalloc(
sizeof(*_tmp5BC));_tmp5BC->hd=(void*)((void*)({struct Cyc_Absyn_Carray_mod_struct*
_tmp5BD=_cycalloc(sizeof(*_tmp5BD));_tmp5BD[0]=({struct Cyc_Absyn_Carray_mod_struct
_tmp5BE;_tmp5BE.tag=0;_tmp5BE.f1=Cyc_yyget_YY51(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5BE.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp5BE;});
_tmp5BD;}));_tmp5BC->tl=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]))->tms;_tmp5BC;});_tmp5BB;});_tmp5BA;});break;case 180:
_LL2B4: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp5BF;(_tmp5BF.YY29).tag=
33;(_tmp5BF.YY29).f1=({struct Cyc_Parse_Declarator*_tmp5C0=_cycalloc(sizeof(*
_tmp5C0));_tmp5C0->id=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)]))->id;_tmp5C0->tms=({struct Cyc_List_List*_tmp5C1=_cycalloc(
sizeof(*_tmp5C1));_tmp5C1->hd=(void*)((void*)({struct Cyc_Absyn_ConstArray_mod_struct*
_tmp5C2=_cycalloc(sizeof(*_tmp5C2));_tmp5C2[0]=({struct Cyc_Absyn_ConstArray_mod_struct
_tmp5C3;_tmp5C3.tag=1;_tmp5C3.f1=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp5C3.f2=Cyc_yyget_YY51(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5C3.f3=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp5C3;});
_tmp5C2;}));_tmp5C1->tl=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 4)]))->tms;_tmp5C1;});_tmp5C0;});_tmp5BF;});break;case 181:
_LL2B5: {struct _tuple17 _tmp5C5;struct Cyc_List_List*_tmp5C6;int _tmp5C7;struct Cyc_Absyn_VarargInfo*
_tmp5C8;struct Cyc_Core_Opt*_tmp5C9;struct Cyc_List_List*_tmp5CA;struct _tuple17*
_tmp5C4=Cyc_yyget_YY39(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp5C5=*_tmp5C4;_tmp5C6=_tmp5C5.f1;_tmp5C7=_tmp5C5.f2;_tmp5C8=_tmp5C5.f3;
_tmp5C9=_tmp5C5.f4;_tmp5CA=_tmp5C5.f5;yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp5CB;(_tmp5CB.YY29).tag=33;(_tmp5CB.YY29).f1=({struct Cyc_Parse_Declarator*
_tmp5CC=_cycalloc(sizeof(*_tmp5CC));_tmp5CC->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id;_tmp5CC->tms=({
struct Cyc_List_List*_tmp5CD=_cycalloc(sizeof(*_tmp5CD));_tmp5CD->hd=(void*)((
void*)({struct Cyc_Absyn_Function_mod_struct*_tmp5CE=_cycalloc(sizeof(*_tmp5CE));
_tmp5CE[0]=({struct Cyc_Absyn_Function_mod_struct _tmp5CF;_tmp5CF.tag=3;_tmp5CF.f1=(
void*)((void*)({struct Cyc_Absyn_WithTypes_struct*_tmp5D0=_cycalloc(sizeof(*
_tmp5D0));_tmp5D0[0]=({struct Cyc_Absyn_WithTypes_struct _tmp5D1;_tmp5D1.tag=1;
_tmp5D1.f1=_tmp5C6;_tmp5D1.f2=_tmp5C7;_tmp5D1.f3=_tmp5C8;_tmp5D1.f4=_tmp5C9;
_tmp5D1.f5=_tmp5CA;_tmp5D1;});_tmp5D0;}));_tmp5CF;});_tmp5CE;}));_tmp5CD->tl=(
Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;
_tmp5CD;});_tmp5CC;});_tmp5CB;});break;}case 182: _LL2B6: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp5D2;(_tmp5D2.YY29).tag=33;(_tmp5D2.YY29).f1=({struct
Cyc_Parse_Declarator*_tmp5D3=_cycalloc(sizeof(*_tmp5D3));_tmp5D3->id=(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->id;_tmp5D3->tms=({
struct Cyc_List_List*_tmp5D4=_cycalloc(sizeof(*_tmp5D4));_tmp5D4->hd=(void*)((
void*)({struct Cyc_Absyn_Function_mod_struct*_tmp5D5=_cycalloc(sizeof(*_tmp5D5));
_tmp5D5[0]=({struct Cyc_Absyn_Function_mod_struct _tmp5D6;_tmp5D6.tag=3;_tmp5D6.f1=(
void*)((void*)({struct Cyc_Absyn_WithTypes_struct*_tmp5D7=_cycalloc(sizeof(*
_tmp5D7));_tmp5D7[0]=({struct Cyc_Absyn_WithTypes_struct _tmp5D8;_tmp5D8.tag=1;
_tmp5D8.f1=0;_tmp5D8.f2=0;_tmp5D8.f3=0;_tmp5D8.f4=Cyc_yyget_YY49(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp5D8.f5=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp5D8;});_tmp5D7;}));
_tmp5D6;});_tmp5D5;}));_tmp5D4->tl=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->tms;_tmp5D4;});_tmp5D3;});
_tmp5D2;});break;case 183: _LL2B7: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp5D9;(_tmp5D9.YY29).tag=33;(_tmp5D9.YY29).f1=({struct Cyc_Parse_Declarator*
_tmp5DA=_cycalloc(sizeof(*_tmp5DA));_tmp5DA->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id;_tmp5DA->tms=({
struct Cyc_List_List*_tmp5DB=_cycalloc(sizeof(*_tmp5DB));_tmp5DB->hd=(void*)((
void*)({struct Cyc_Absyn_Function_mod_struct*_tmp5DC=_cycalloc(sizeof(*_tmp5DC));
_tmp5DC[0]=({struct Cyc_Absyn_Function_mod_struct _tmp5DD;_tmp5DD.tag=3;_tmp5DD.f1=(
void*)((void*)({struct Cyc_Absyn_NoTypes_struct*_tmp5DE=_cycalloc(sizeof(*_tmp5DE));
_tmp5DE[0]=({struct Cyc_Absyn_NoTypes_struct _tmp5DF;_tmp5DF.tag=0;_tmp5DF.f1=Cyc_yyget_YY36(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp5DF.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp5DF;});
_tmp5DE;}));_tmp5DD;});_tmp5DC;}));_tmp5DB->tl=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp5DB;});_tmp5DA;});
_tmp5D9;});break;case 184: _LL2B8: {struct Cyc_List_List*_tmp5E0=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),Cyc_List_imp_rev(
Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp5E1;(_tmp5E1.YY29).tag=
33;(_tmp5E1.YY29).f1=({struct Cyc_Parse_Declarator*_tmp5E2=_cycalloc(sizeof(*
_tmp5E2));_tmp5E2->id=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]))->id;_tmp5E2->tms=({struct Cyc_List_List*_tmp5E3=_cycalloc(
sizeof(*_tmp5E3));_tmp5E3->hd=(void*)((void*)({struct Cyc_Absyn_TypeParams_mod_struct*
_tmp5E4=_cycalloc(sizeof(*_tmp5E4));_tmp5E4[0]=({struct Cyc_Absyn_TypeParams_mod_struct
_tmp5E5;_tmp5E5.tag=4;_tmp5E5.f1=_tmp5E0;_tmp5E5.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp5E5.f3=0;
_tmp5E5;});_tmp5E4;}));_tmp5E3->tl=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp5E3;});_tmp5E2;});
_tmp5E1;});break;}case 185: _LL2B9: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp5E6;(_tmp5E6.YY29).tag=33;(_tmp5E6.YY29).f1=({struct Cyc_Parse_Declarator*
_tmp5E7=_cycalloc(sizeof(*_tmp5E7));_tmp5E7->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]))->id;_tmp5E7->tms=({
struct Cyc_List_List*_tmp5E8=_cycalloc(sizeof(*_tmp5E8));_tmp5E8->hd=(void*)((
void*)({struct Cyc_Absyn_Attributes_mod_struct*_tmp5E9=_cycalloc(sizeof(*_tmp5E9));
_tmp5E9[0]=({struct Cyc_Absyn_Attributes_mod_struct _tmp5EA;_tmp5EA.tag=5;_tmp5EA.f1=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp5EA.f2=Cyc_yyget_YY45(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5EA;});_tmp5E9;}));
_tmp5E8->tl=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]))->tms;_tmp5E8;});_tmp5E7;});_tmp5E6;});break;case 186: _LL2BA:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 187:
_LL2BB: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp5EB;(_tmp5EB.YY28).tag=
32;(_tmp5EB.YY28).f1=Cyc_List_imp_append(Cyc_yyget_YY28(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY28(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp5EB;});break;case 188:
_LL2BC: {struct Cyc_List_List*ans=0;if(Cyc_yyget_YY45(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])!= 0)ans=({struct Cyc_List_List*
_tmp5EC=_cycalloc(sizeof(*_tmp5EC));_tmp5EC->hd=(void*)((void*)({struct Cyc_Absyn_Attributes_mod_struct*
_tmp5ED=_cycalloc(sizeof(*_tmp5ED));_tmp5ED[0]=({struct Cyc_Absyn_Attributes_mod_struct
_tmp5EE;_tmp5EE.tag=5;_tmp5EE.f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line);_tmp5EE.f2=
Cyc_yyget_YY45(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp5EE;});_tmp5ED;}));_tmp5EC->tl=ans;_tmp5EC;});{struct Cyc_Absyn_PtrLoc*ptrloc=
0;if(Cyc_Absyn_porting_c_code)ptrloc=({struct Cyc_Absyn_PtrLoc*_tmp5EF=_cycalloc(
sizeof(*_tmp5EF));_tmp5EF->ptr_loc=(*Cyc_yyget_YY1(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)])).f1;_tmp5EF->rgn_loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line);_tmp5EF->zt_loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line);_tmp5EF;});ans=({struct Cyc_List_List*_tmp5F0=
_cycalloc(sizeof(*_tmp5F0));_tmp5F0->hd=(void*)((void*)({struct Cyc_Absyn_Pointer_mod_struct*
_tmp5F1=_cycalloc(sizeof(*_tmp5F1));_tmp5F1[0]=({struct Cyc_Absyn_Pointer_mod_struct
_tmp5F2;_tmp5F2.tag=2;_tmp5F2.f1=({struct Cyc_Absyn_PtrAtts _tmp5F3;_tmp5F3.rgn=(
void*)Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);
_tmp5F3.nullable=(*Cyc_yyget_YY1(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)])).f2;_tmp5F3.bounds=(*Cyc_yyget_YY1(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)])).f3;_tmp5F3.zero_term=
Cyc_yyget_YY51(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);
_tmp5F3.ptrloc=ptrloc;_tmp5F3;});_tmp5F2.f2=Cyc_yyget_YY25(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5F2;});_tmp5F1;}));
_tmp5F0->tl=ans;_tmp5F0;});yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp5F4;(_tmp5F4.YY28).tag=32;(_tmp5F4.YY28).f1=ans;_tmp5F4;});break;}}case 189:
_LL2BD: {struct Cyc_Position_Segment*loc=Cyc_Absyn_porting_c_code?Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line): 0;yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp5F5;(_tmp5F5.YY1).tag=5;(
_tmp5F5.YY1).f1=({struct _tuple12*_tmp5F6=_cycalloc(sizeof(*_tmp5F6));_tmp5F6->f1=
loc;_tmp5F6->f2=Cyc_Absyn_true_conref;_tmp5F6->f3=Cyc_yyget_YY2(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5F6;});_tmp5F5;});
break;}case 190: _LL2BE: {struct Cyc_Position_Segment*loc=Cyc_Absyn_porting_c_code?
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line): 0;yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp5F7;(_tmp5F7.YY1).tag=5;(_tmp5F7.YY1).f1=({struct _tuple12*_tmp5F8=_cycalloc(
sizeof(*_tmp5F8));_tmp5F8->f1=loc;_tmp5F8->f2=Cyc_Absyn_false_conref;_tmp5F8->f3=
Cyc_yyget_YY2(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5F8;});
_tmp5F7;});break;}case 191: _LL2BF: {struct Cyc_Position_Segment*loc=Cyc_Absyn_porting_c_code?
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line): 0;yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp5F9;(_tmp5F9.YY1).tag=5;(_tmp5F9.YY1).f1=({struct _tuple12*_tmp5FA=_cycalloc(
sizeof(*_tmp5FA));_tmp5FA->f1=loc;_tmp5FA->f2=Cyc_Absyn_true_conref;_tmp5FA->f3=
Cyc_Absyn_bounds_dyneither_conref;_tmp5FA;});_tmp5F9;});break;}case 192: _LL2C0:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp5FB;(_tmp5FB.YY2).tag=
6;(_tmp5FB.YY2).f1=Cyc_Absyn_bounds_one_conref;_tmp5FB;});break;case 193: _LL2C1:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp5FC;(_tmp5FC.YY2).tag=
6;(_tmp5FC.YY2).f1=Cyc_Absyn_new_conref((void*)({struct Cyc_Absyn_Upper_b_struct*
_tmp5FD=_cycalloc(sizeof(*_tmp5FD));_tmp5FD[0]=({struct Cyc_Absyn_Upper_b_struct
_tmp5FE;_tmp5FE.tag=0;_tmp5FE.f1=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp5FE;});_tmp5FD;}));
_tmp5FC;});break;case 194: _LL2C2: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp5FF;(_tmp5FF.YY51).tag=55;(_tmp5FF.YY51).f1=((struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)();_tmp5FF;});break;case 195: _LL2C3: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp600;(_tmp600.YY51).tag=55;(_tmp600.YY51).f1=Cyc_Absyn_true_conref;
_tmp600;});break;case 196: _LL2C4: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp601;(_tmp601.YY51).tag=55;(_tmp601.YY51).f1=Cyc_Absyn_false_conref;_tmp601;});
break;case 197: _LL2C5: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp602;(_tmp602.YY49).tag=53;(_tmp602.YY49).f1=0;_tmp602;});break;case 198:
_LL2C6: Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),(void*)3,1);yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp603;(_tmp603.YY49).tag=53;(_tmp603.YY49).f1=({struct
Cyc_Core_Opt*_tmp604=_cycalloc(sizeof(*_tmp604));_tmp604->v=(void*)Cyc_yyget_YY44(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp604;});_tmp603;});
break;case 199: _LL2C7: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp605;(_tmp605.YY49).tag=53;(_tmp605.YY49).f1=({struct Cyc_Core_Opt*_tmp606=
_cycalloc(sizeof(*_tmp606));_tmp606->v=(void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_rk,0);_tmp606;});_tmp605;});break;case 200: _LL2C8: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp607;(_tmp607.YY44).tag=48;(_tmp607.YY44).f1=(void*)Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_trk,0);_tmp607;});break;case 201: _LL2C9: Cyc_Parse_set_vartyp_kind(
Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),(void*)
5,1);yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
202: _LL2CA: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp608;(
_tmp608.YY44).tag=48;(_tmp608.YY44).f1=(void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_trk,0);_tmp608;});break;case 203: _LL2CB: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp609;(_tmp609.YY25).tag=29;(_tmp609.YY25).f1=Cyc_Absyn_empty_tqual(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset + 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset + 1)]).last_line));_tmp609;});break;case 204: _LL2CC: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp60A;(_tmp60A.YY25).tag=29;(_tmp60A.YY25).f1=Cyc_Absyn_combine_tqual(
Cyc_yyget_YY25(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY25(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp60A;});break;case
205: _LL2CD: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp60B;(
_tmp60B.YY39).tag=43;(_tmp60B.YY39).f1=({struct _tuple17*_tmp60C=_cycalloc(
sizeof(*_tmp60C));_tmp60C->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(Cyc_yyget_YY38(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]));_tmp60C->f2=0;_tmp60C->f3=0;_tmp60C->f4=Cyc_yyget_YY49(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp60C->f5=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp60C;});_tmp60B;});
break;case 206: _LL2CE: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp60D;(_tmp60D.YY39).tag=43;(_tmp60D.YY39).f1=({struct _tuple17*_tmp60E=
_cycalloc(sizeof(*_tmp60E));_tmp60E->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(Cyc_yyget_YY38(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)]));_tmp60E->f2=1;_tmp60E->f3=0;_tmp60E->f4=Cyc_yyget_YY49(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp60E->f5=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp60E;});_tmp60D;});
break;case 207: _LL2CF: {struct _tuple2 _tmp610;struct Cyc_Core_Opt*_tmp611;struct Cyc_Absyn_Tqual
_tmp612;void*_tmp613;struct _tuple2*_tmp60F=Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp610=*_tmp60F;_tmp611=
_tmp610.f1;_tmp612=_tmp610.f2;_tmp613=_tmp610.f3;{struct Cyc_Absyn_VarargInfo*
_tmp614=({struct Cyc_Absyn_VarargInfo*_tmp617=_cycalloc(sizeof(*_tmp617));_tmp617->name=
_tmp611;_tmp617->tq=_tmp612;_tmp617->type=(void*)_tmp613;_tmp617->inject=Cyc_yyget_YY31(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp617;});yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp615;(_tmp615.YY39).tag=43;(
_tmp615.YY39).f1=({struct _tuple17*_tmp616=_cycalloc(sizeof(*_tmp616));_tmp616->f1=
0;_tmp616->f2=0;_tmp616->f3=_tmp614;_tmp616->f4=Cyc_yyget_YY49(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp616->f5=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp616;});_tmp615;});
break;}}case 208: _LL2D0: {struct _tuple2 _tmp619;struct Cyc_Core_Opt*_tmp61A;struct
Cyc_Absyn_Tqual _tmp61B;void*_tmp61C;struct _tuple2*_tmp618=Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp619=*_tmp618;_tmp61A=
_tmp619.f1;_tmp61B=_tmp619.f2;_tmp61C=_tmp619.f3;{struct Cyc_Absyn_VarargInfo*
_tmp61D=({struct Cyc_Absyn_VarargInfo*_tmp620=_cycalloc(sizeof(*_tmp620));_tmp620->name=
_tmp61A;_tmp620->tq=_tmp61B;_tmp620->type=(void*)_tmp61C;_tmp620->inject=Cyc_yyget_YY31(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp620;});yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp61E;(_tmp61E.YY39).tag=43;(
_tmp61E.YY39).f1=({struct _tuple17*_tmp61F=_cycalloc(sizeof(*_tmp61F));_tmp61F->f1=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]));_tmp61F->f2=0;
_tmp61F->f3=_tmp61D;_tmp61F->f4=Cyc_yyget_YY49(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp61F->f5=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp61F;});_tmp61E;});
break;}}case 209: _LL2D1: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp621;(_tmp621.YY44).tag=48;(_tmp621.YY44).f1=(void*)Cyc_Parse_id2type(Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),(void*)({struct Cyc_Absyn_Unknown_kb_struct*
_tmp622=_cycalloc(sizeof(*_tmp622));_tmp622[0]=({struct Cyc_Absyn_Unknown_kb_struct
_tmp623;_tmp623.tag=1;_tmp623.f1=0;_tmp623;});_tmp622;}));_tmp621;});break;case
210: _LL2D2: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp624;(
_tmp624.YY44).tag=48;(_tmp624.YY44).f1=(void*)Cyc_Parse_id2type(Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_Tcutil_kind_to_bound(
Cyc_yyget_YY43(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])));
_tmp624;});break;case 211: _LL2D3: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp625;(_tmp625.YY49).tag=53;(_tmp625.YY49).f1=0;_tmp625;});break;case 212:
_LL2D4: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp626;(_tmp626.YY49).tag=
53;(_tmp626.YY49).f1=({struct Cyc_Core_Opt*_tmp627=_cycalloc(sizeof(*_tmp627));
_tmp627->v=(void*)((void*)({struct Cyc_Absyn_JoinEff_struct*_tmp628=_cycalloc(
sizeof(*_tmp628));_tmp628[0]=({struct Cyc_Absyn_JoinEff_struct _tmp629;_tmp629.tag=
20;_tmp629.f1=Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp629;});_tmp628;}));_tmp627;});_tmp626;});break;case 213:
_LL2D5: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp62A;(_tmp62A.YY50).tag=
54;(_tmp62A.YY50).f1=0;_tmp62A;});break;case 214: _LL2D6: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 215: _LL2D7: {
struct Cyc_Absyn_Less_kb_struct*_tmp62B=({struct Cyc_Absyn_Less_kb_struct*_tmp633=
_cycalloc(sizeof(*_tmp633));_tmp633[0]=({struct Cyc_Absyn_Less_kb_struct _tmp634;
_tmp634.tag=2;_tmp634.f1=0;_tmp634.f2=(void*)((void*)5);_tmp634;});_tmp633;});
struct _dyneither_ptr _tmp62C=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);void*_tmp62D=Cyc_Parse_id2type(
_tmp62C,(void*)_tmp62B);yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp62E;(_tmp62E.YY50).tag=54;(_tmp62E.YY50).f1=({struct Cyc_List_List*_tmp62F=
_cycalloc(sizeof(*_tmp62F));_tmp62F->hd=({struct _tuple4*_tmp630=_cycalloc(
sizeof(*_tmp630));_tmp630->f1=(void*)({struct Cyc_Absyn_JoinEff_struct*_tmp631=
_cycalloc(sizeof(*_tmp631));_tmp631[0]=({struct Cyc_Absyn_JoinEff_struct _tmp632;
_tmp632.tag=20;_tmp632.f1=Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]);_tmp632;});_tmp631;});_tmp630->f2=_tmp62D;_tmp630;});
_tmp62F->tl=0;_tmp62F;});_tmp62E;});break;}case 216: _LL2D8: {struct Cyc_Absyn_Less_kb_struct*
_tmp635=({struct Cyc_Absyn_Less_kb_struct*_tmp63D=_cycalloc(sizeof(*_tmp63D));
_tmp63D[0]=({struct Cyc_Absyn_Less_kb_struct _tmp63E;_tmp63E.tag=2;_tmp63E.f1=0;
_tmp63E.f2=(void*)((void*)5);_tmp63E;});_tmp63D;});struct _dyneither_ptr _tmp636=
Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);
void*_tmp637=Cyc_Parse_id2type(_tmp636,(void*)_tmp635);yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp638;(_tmp638.YY50).tag=54;(_tmp638.YY50).f1=({struct
Cyc_List_List*_tmp639=_cycalloc(sizeof(*_tmp639));_tmp639->hd=({struct _tuple4*
_tmp63A=_cycalloc(sizeof(*_tmp63A));_tmp63A->f1=(void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp63B=_cycalloc(sizeof(*_tmp63B));_tmp63B[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp63C;_tmp63C.tag=20;_tmp63C.f1=Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp63C;});_tmp63B;});
_tmp63A->f2=_tmp637;_tmp63A;});_tmp639->tl=Cyc_yyget_YY50(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp639;});_tmp638;});
break;}case 217: _LL2D9: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp63F;(_tmp63F.YY31).tag=35;(_tmp63F.YY31).f1=0;_tmp63F;});break;case 218:
_LL2DA: if(Cyc_zstrcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),({const char*_tmp640="inject";
_tag_dyneither(_tmp640,sizeof(char),7);}))!= 0)Cyc_Parse_err(({const char*_tmp641="missing type in function declaration";
_tag_dyneither(_tmp641,sizeof(char),37);}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp642;(_tmp642.YY31).tag=35;(_tmp642.YY31).f1=1;_tmp642;});
break;case 219: _LL2DB: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 220: _LL2DC: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp643;(_tmp643.YY40).tag=44;(_tmp643.YY40).f1=Cyc_List_imp_append(Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp643;});break;case 221:
_LL2DD: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp644;(_tmp644.YY40).tag=
44;(_tmp644.YY40).f1=0;_tmp644;});break;case 222: _LL2DE: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;case 223: _LL2DF:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp645;(_tmp645.YY40).tag=
44;(_tmp645.YY40).f1=({struct Cyc_List_List*_tmp646=_cycalloc(sizeof(*_tmp646));
_tmp646->hd=(void*)((void*)({struct Cyc_Absyn_RgnsEff_struct*_tmp647=_cycalloc(
sizeof(*_tmp647));_tmp647[0]=({struct Cyc_Absyn_RgnsEff_struct _tmp648;_tmp648.tag=
21;_tmp648.f1=(void*)Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp648;});_tmp647;}));_tmp646->tl=0;_tmp646;});_tmp645;});
break;case 224: _LL2E0: Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),(void*)6,0);yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp649;(_tmp649.YY40).tag=44;(_tmp649.YY40).f1=({struct
Cyc_List_List*_tmp64A=_cycalloc(sizeof(*_tmp64A));_tmp64A->hd=(void*)Cyc_yyget_YY44(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp64A->tl=0;_tmp64A;});
_tmp649;});break;case 225: _LL2E1: Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),(void*)5,1);yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp64B;(_tmp64B.YY40).tag=44;(_tmp64B.YY40).f1=({struct
Cyc_List_List*_tmp64C=_cycalloc(sizeof(*_tmp64C));_tmp64C->hd=(void*)((void*)({
struct Cyc_Absyn_AccessEff_struct*_tmp64D=_cycalloc(sizeof(*_tmp64D));_tmp64D[0]=({
struct Cyc_Absyn_AccessEff_struct _tmp64E;_tmp64E.tag=19;_tmp64E.f1=(void*)Cyc_yyget_YY44(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp64E;});_tmp64D;}));
_tmp64C->tl=0;_tmp64C;});_tmp64B;});break;case 226: _LL2E2: Cyc_Parse_set_vartyp_kind(
Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),(
void*)5,1);yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp64F;(
_tmp64F.YY40).tag=44;(_tmp64F.YY40).f1=({struct Cyc_List_List*_tmp650=_cycalloc(
sizeof(*_tmp650));_tmp650->hd=(void*)((void*)({struct Cyc_Absyn_AccessEff_struct*
_tmp651=_cycalloc(sizeof(*_tmp651));_tmp651[0]=({struct Cyc_Absyn_AccessEff_struct
_tmp652;_tmp652.tag=19;_tmp652.f1=(void*)Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp652;});_tmp651;}));
_tmp650->tl=Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp650;});_tmp64F;});break;case 227: _LL2E3: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp653;(_tmp653.YY38).tag=42;(_tmp653.YY38).f1=({struct
Cyc_List_List*_tmp654=_cycalloc(sizeof(*_tmp654));_tmp654->hd=Cyc_yyget_YY37(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp654->tl=0;_tmp654;});
_tmp653;});break;case 228: _LL2E4: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp655;(_tmp655.YY38).tag=42;(_tmp655.YY38).f1=({struct Cyc_List_List*_tmp656=
_cycalloc(sizeof(*_tmp656));_tmp656->hd=Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp656->tl=Cyc_yyget_YY38(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp656;});_tmp655;});
break;case 229: _LL2E5: {struct _tuple16 _tmp658;struct Cyc_Absyn_Tqual _tmp659;struct
Cyc_List_List*_tmp65A;struct Cyc_List_List*_tmp65B;struct _tuple16*_tmp657=Cyc_yyget_YY35(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp658=*_tmp657;
_tmp659=_tmp658.f1;_tmp65A=_tmp658.f2;_tmp65B=_tmp658.f3;if(_tmp659.loc == 0)
_tmp659.loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line);{struct Cyc_Parse_Declarator _tmp65D;struct _tuple1*
_tmp65E;struct Cyc_List_List*_tmp65F;struct Cyc_Parse_Declarator*_tmp65C=Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp65D=*_tmp65C;
_tmp65E=_tmp65D.id;_tmp65F=_tmp65D.tms;{void*_tmp660=Cyc_Parse_speclist2typ(
_tmp65A,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));struct Cyc_Absyn_Tqual _tmp662;void*_tmp663;struct
Cyc_List_List*_tmp664;struct Cyc_List_List*_tmp665;struct _tuple6 _tmp661=Cyc_Parse_apply_tms(
_tmp659,_tmp660,_tmp65B,_tmp65F);_tmp662=_tmp661.f1;_tmp663=_tmp661.f2;_tmp664=
_tmp661.f3;_tmp665=_tmp661.f4;if(_tmp664 != 0)Cyc_Parse_err(({const char*_tmp666="parameter with bad type params";
_tag_dyneither(_tmp666,sizeof(char),31);}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));if(Cyc_Absyn_is_qvar_qualified(
_tmp65E))Cyc_Parse_err(({const char*_tmp667="parameter cannot be qualified with a namespace";
_tag_dyneither(_tmp667,sizeof(char),47);}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));{struct Cyc_Core_Opt*
_tmp668=(struct Cyc_Core_Opt*)({struct Cyc_Core_Opt*_tmp66D=_cycalloc(sizeof(*
_tmp66D));_tmp66D->v=(*_tmp65E).f2;_tmp66D;});if(_tmp665 != 0)({void*_tmp669=0;
Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),({const char*_tmp66A="extra attributes on parameter, ignoring";
_tag_dyneither(_tmp66A,sizeof(char),40);}),_tag_dyneither(_tmp669,sizeof(void*),
0));});yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp66B;(_tmp66B.YY37).tag=
41;(_tmp66B.YY37).f1=({struct _tuple2*_tmp66C=_cycalloc(sizeof(*_tmp66C));_tmp66C->f1=
_tmp668;_tmp66C->f2=_tmp662;_tmp66C->f3=_tmp663;_tmp66C;});_tmp66B;});break;}}}}
case 230: _LL2E6: {struct _tuple16 _tmp66F;struct Cyc_Absyn_Tqual _tmp670;struct Cyc_List_List*
_tmp671;struct Cyc_List_List*_tmp672;struct _tuple16*_tmp66E=Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp66F=*_tmp66E;_tmp670=
_tmp66F.f1;_tmp671=_tmp66F.f2;_tmp672=_tmp66F.f3;if(_tmp670.loc == 0)_tmp670.loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);{void*_tmp673=Cyc_Parse_speclist2typ(_tmp671,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));if(_tmp672 != 0)({
void*_tmp674=0;Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),({const char*
_tmp675="bad attributes on parameter, ignoring";_tag_dyneither(_tmp675,sizeof(
char),38);}),_tag_dyneither(_tmp674,sizeof(void*),0));});yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp676;(_tmp676.YY37).tag=41;(_tmp676.YY37).f1=({struct
_tuple2*_tmp677=_cycalloc(sizeof(*_tmp677));_tmp677->f1=0;_tmp677->f2=_tmp670;
_tmp677->f3=_tmp673;_tmp677;});_tmp676;});break;}}case 231: _LL2E7: {struct
_tuple16 _tmp679;struct Cyc_Absyn_Tqual _tmp67A;struct Cyc_List_List*_tmp67B;struct
Cyc_List_List*_tmp67C;struct _tuple16*_tmp678=Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp679=*_tmp678;_tmp67A=
_tmp679.f1;_tmp67B=_tmp679.f2;_tmp67C=_tmp679.f3;if(_tmp67A.loc == 0)_tmp67A.loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line);{void*_tmp67D=Cyc_Parse_speclist2typ(_tmp67B,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));struct Cyc_List_List*
_tmp67E=(Cyc_yyget_YY30(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tms;
struct Cyc_Absyn_Tqual _tmp680;void*_tmp681;struct Cyc_List_List*_tmp682;struct Cyc_List_List*
_tmp683;struct _tuple6 _tmp67F=Cyc_Parse_apply_tms(_tmp67A,_tmp67D,_tmp67C,_tmp67E);
_tmp680=_tmp67F.f1;_tmp681=_tmp67F.f2;_tmp682=_tmp67F.f3;_tmp683=_tmp67F.f4;if(
_tmp682 != 0)({void*_tmp684=0;Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),({const char*
_tmp685="bad type parameters on formal argument, ignoring";_tag_dyneither(
_tmp685,sizeof(char),49);}),_tag_dyneither(_tmp684,sizeof(void*),0));});if(
_tmp683 != 0)({void*_tmp686=0;Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),({const char*
_tmp687="bad attributes on parameter, ignoring";_tag_dyneither(_tmp687,sizeof(
char),38);}),_tag_dyneither(_tmp686,sizeof(void*),0));});yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp688;(_tmp688.YY37).tag=41;(_tmp688.YY37).f1=({struct
_tuple2*_tmp689=_cycalloc(sizeof(*_tmp689));_tmp689->f1=0;_tmp689->f2=_tmp680;
_tmp689->f3=_tmp681;_tmp689;});_tmp688;});break;}}case 232: _LL2E8: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp68A;(_tmp68A.YY36).tag=40;(_tmp68A.YY36).f1=((struct
Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY36(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp68A;});break;case 233:
_LL2E9: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp68B;(_tmp68B.YY36).tag=
40;(_tmp68B.YY36).f1=({struct Cyc_List_List*_tmp68C=_cycalloc(sizeof(*_tmp68C));
_tmp68C->hd=({struct _dyneither_ptr*_tmp68D=_cycalloc(sizeof(*_tmp68D));_tmp68D[0]=
Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
_tmp68D;});_tmp68C->tl=0;_tmp68C;});_tmp68B;});break;case 234: _LL2EA: yyval=(union
Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp68E;(_tmp68E.YY36).tag=40;(_tmp68E.YY36).f1=({
struct Cyc_List_List*_tmp68F=_cycalloc(sizeof(*_tmp68F));_tmp68F->hd=({struct
_dyneither_ptr*_tmp690=_cycalloc(sizeof(*_tmp690));_tmp690[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp690;});_tmp68F->tl=
Cyc_yyget_YY36(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);
_tmp68F;});_tmp68E;});break;case 235: _LL2EB: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 236: _LL2EC: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 237: _LL2ED:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp691;(_tmp691.YY3).tag=
7;(_tmp691.YY3).f1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_UnresolvedMem_e_struct*
_tmp692=_cycalloc(sizeof(*_tmp692));_tmp692[0]=({struct Cyc_Absyn_UnresolvedMem_e_struct
_tmp693;_tmp693.tag=37;_tmp693.f1=0;_tmp693.f2=0;_tmp693;});_tmp692;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp691;});
break;case 238: _LL2EE: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp694;(_tmp694.YY3).tag=7;(_tmp694.YY3).f1=Cyc_Absyn_new_exp((void*)({struct
Cyc_Absyn_UnresolvedMem_e_struct*_tmp695=_cycalloc(sizeof(*_tmp695));_tmp695[0]=({
struct Cyc_Absyn_UnresolvedMem_e_struct _tmp696;_tmp696.tag=37;_tmp696.f1=0;
_tmp696.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_YY6(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));
_tmp696;});_tmp695;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp694;});
break;case 239: _LL2EF: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp697;(_tmp697.YY3).tag=7;(_tmp697.YY3).f1=Cyc_Absyn_new_exp((void*)({struct
Cyc_Absyn_UnresolvedMem_e_struct*_tmp698=_cycalloc(sizeof(*_tmp698));_tmp698[0]=({
struct Cyc_Absyn_UnresolvedMem_e_struct _tmp699;_tmp699.tag=37;_tmp699.f1=0;
_tmp699.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_YY6(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]));
_tmp699;});_tmp698;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp697;});
break;case 240: _LL2F0: {struct Cyc_Absyn_Vardecl*_tmp69A=Cyc_Absyn_new_vardecl(({
struct _tuple1*_tmp69E=_cycalloc(sizeof(*_tmp69E));_tmp69E->f1=(union Cyc_Absyn_Nmspace_union)({
union Cyc_Absyn_Nmspace_union _tmp6A0;(_tmp6A0.Loc_n).tag=0;_tmp6A0;});_tmp69E->f2=({
struct _dyneither_ptr*_tmp69F=_cycalloc(sizeof(*_tmp69F));_tmp69F[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]);_tmp69F;});_tmp69E;}),
Cyc_Absyn_uint_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).last_line)));(_tmp69A->tq).real_const=
1;yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp69B;(_tmp69B.YY3).tag=
7;(_tmp69B.YY3).f1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Comprehension_e_struct*
_tmp69C=_cycalloc(sizeof(*_tmp69C));_tmp69C[0]=({struct Cyc_Absyn_Comprehension_e_struct
_tmp69D;_tmp69D.tag=29;_tmp69D.f1=_tmp69A;_tmp69D.f2=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp69D.f3=Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp69D.f4=0;
_tmp69D;});_tmp69C;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp69B;});
break;}case 241: _LL2F1: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp6A1;(_tmp6A1.YY6).tag=10;(_tmp6A1.YY6).f1=({struct Cyc_List_List*_tmp6A2=
_cycalloc(sizeof(*_tmp6A2));_tmp6A2->hd=({struct _tuple20*_tmp6A3=_cycalloc(
sizeof(*_tmp6A3));_tmp6A3->f1=0;_tmp6A3->f2=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6A3;});_tmp6A2->tl=0;
_tmp6A2;});_tmp6A1;});break;case 242: _LL2F2: yyval=(union Cyc_YYSTYPE_union)({union
Cyc_YYSTYPE_union _tmp6A4;(_tmp6A4.YY6).tag=10;(_tmp6A4.YY6).f1=({struct Cyc_List_List*
_tmp6A5=_cycalloc(sizeof(*_tmp6A5));_tmp6A5->hd=({struct _tuple20*_tmp6A6=
_cycalloc(sizeof(*_tmp6A6));_tmp6A6->f1=Cyc_yyget_YY41(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp6A6->f2=Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6A6;});_tmp6A5->tl=
0;_tmp6A5;});_tmp6A4;});break;case 243: _LL2F3: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp6A7;(_tmp6A7.YY6).tag=10;(_tmp6A7.YY6).f1=({struct Cyc_List_List*
_tmp6A8=_cycalloc(sizeof(*_tmp6A8));_tmp6A8->hd=({struct _tuple20*_tmp6A9=
_cycalloc(sizeof(*_tmp6A9));_tmp6A9->f1=0;_tmp6A9->f2=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6A9;});_tmp6A8->tl=Cyc_yyget_YY6(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp6A8;});_tmp6A7;});
break;case 244: _LL2F4: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp6AA;(_tmp6AA.YY6).tag=10;(_tmp6AA.YY6).f1=({struct Cyc_List_List*_tmp6AB=
_cycalloc(sizeof(*_tmp6AB));_tmp6AB->hd=({struct _tuple20*_tmp6AC=_cycalloc(
sizeof(*_tmp6AC));_tmp6AC->f1=Cyc_yyget_YY41(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_tmp6AC->f2=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6AC;});_tmp6AB->tl=Cyc_yyget_YY6(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp6AB;});_tmp6AA;});
break;case 245: _LL2F5: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp6AD;(_tmp6AD.YY41).tag=45;(_tmp6AD.YY41).f1=Cyc_List_imp_rev(Cyc_yyget_YY41(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp6AD;});break;
case 246: _LL2F6: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp6AE;(
_tmp6AE.YY41).tag=45;(_tmp6AE.YY41).f1=({struct Cyc_List_List*_tmp6AF=_cycalloc(
sizeof(*_tmp6AF));_tmp6AF->hd=(void*)Cyc_yyget_YY42(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6AF->tl=0;_tmp6AF;});
_tmp6AE;});break;case 247: _LL2F7: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp6B0;(_tmp6B0.YY41).tag=45;(_tmp6B0.YY41).f1=({struct Cyc_List_List*_tmp6B1=
_cycalloc(sizeof(*_tmp6B1));_tmp6B1->hd=(void*)Cyc_yyget_YY42(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6B1->tl=Cyc_yyget_YY41(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp6B1;});_tmp6B0;});
break;case 248: _LL2F8: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp6B2;(_tmp6B2.YY42).tag=46;(_tmp6B2.YY42).f1=(void*)((void*)({struct Cyc_Absyn_ArrayElement_struct*
_tmp6B3=_cycalloc(sizeof(*_tmp6B3));_tmp6B3[0]=({struct Cyc_Absyn_ArrayElement_struct
_tmp6B4;_tmp6B4.tag=0;_tmp6B4.f1=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp6B4;});_tmp6B3;}));
_tmp6B2;});break;case 249: _LL2F9: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp6B5;(_tmp6B5.YY42).tag=46;(_tmp6B5.YY42).f1=(void*)((void*)({struct Cyc_Absyn_FieldName_struct*
_tmp6B6=_cycalloc(sizeof(*_tmp6B6));_tmp6B6[0]=({struct Cyc_Absyn_FieldName_struct
_tmp6B7;_tmp6B7.tag=1;_tmp6B7.f1=({struct _dyneither_ptr*_tmp6B8=_cycalloc(
sizeof(*_tmp6B8));_tmp6B8[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6B8;});_tmp6B7;});
_tmp6B6;}));_tmp6B5;});break;case 250: _LL2FA: {struct _tuple16 _tmp6BA;struct Cyc_Absyn_Tqual
_tmp6BB;struct Cyc_List_List*_tmp6BC;struct Cyc_List_List*_tmp6BD;struct _tuple16*
_tmp6B9=Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
_tmp6BA=*_tmp6B9;_tmp6BB=_tmp6BA.f1;_tmp6BC=_tmp6BA.f2;_tmp6BD=_tmp6BA.f3;{void*
_tmp6BE=Cyc_Parse_speclist2typ(_tmp6BC,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));if(_tmp6BD != 0)({
void*_tmp6BF=0;Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),({const char*
_tmp6C0="ignoring attributes in type";_tag_dyneither(_tmp6C0,sizeof(char),28);}),
_tag_dyneither(_tmp6BF,sizeof(void*),0));});yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp6C1;(_tmp6C1.YY37).tag=41;(_tmp6C1.YY37).f1=({struct
_tuple2*_tmp6C2=_cycalloc(sizeof(*_tmp6C2));_tmp6C2->f1=0;_tmp6C2->f2=_tmp6BB;
_tmp6C2->f3=_tmp6BE;_tmp6C2;});_tmp6C1;});break;}}case 251: _LL2FB: {struct
_tuple16 _tmp6C4;struct Cyc_Absyn_Tqual _tmp6C5;struct Cyc_List_List*_tmp6C6;struct
Cyc_List_List*_tmp6C7;struct _tuple16*_tmp6C3=Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp6C4=*_tmp6C3;_tmp6C5=
_tmp6C4.f1;_tmp6C6=_tmp6C4.f2;_tmp6C7=_tmp6C4.f3;{void*_tmp6C8=Cyc_Parse_speclist2typ(
_tmp6C6,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));struct Cyc_List_List*_tmp6C9=(Cyc_yyget_YY30(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->tms;struct _tuple6 _tmp6CA=
Cyc_Parse_apply_tms(_tmp6C5,_tmp6C8,_tmp6C7,_tmp6C9);if(_tmp6CA.f3 != 0)({void*
_tmp6CB=0;Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),({const char*
_tmp6CC="bad type params, ignoring";_tag_dyneither(_tmp6CC,sizeof(char),26);}),
_tag_dyneither(_tmp6CB,sizeof(void*),0));});if(_tmp6CA.f4 != 0)({void*_tmp6CD=0;
Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),({const char*_tmp6CE="bad specifiers, ignoring";
_tag_dyneither(_tmp6CE,sizeof(char),25);}),_tag_dyneither(_tmp6CD,sizeof(void*),
0));});yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp6CF;(_tmp6CF.YY37).tag=
41;(_tmp6CF.YY37).f1=({struct _tuple2*_tmp6D0=_cycalloc(sizeof(*_tmp6D0));_tmp6D0->f1=
0;_tmp6D0->f2=_tmp6CA.f1;_tmp6D0->f3=_tmp6CA.f2;_tmp6D0;});_tmp6CF;});break;}}
case 252: _LL2FC: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp6D1;(
_tmp6D1.YY44).tag=48;(_tmp6D1.YY44).f1=(void*)(*Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f3;_tmp6D1;});break;case
253: _LL2FD: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp6D2;(
_tmp6D2.YY44).tag=48;(_tmp6D2.YY44).f1=(void*)((void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp6D3=_cycalloc(sizeof(*_tmp6D3));_tmp6D3[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp6D4;_tmp6D4.tag=20;_tmp6D4.f1=0;_tmp6D4;});_tmp6D3;}));_tmp6D2;});break;case
254: _LL2FE: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp6D5;(
_tmp6D5.YY44).tag=48;(_tmp6D5.YY44).f1=(void*)((void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp6D6=_cycalloc(sizeof(*_tmp6D6));_tmp6D6[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp6D7;_tmp6D7.tag=20;_tmp6D7.f1=Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp6D7;});_tmp6D6;}));
_tmp6D5;});break;case 255: _LL2FF: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp6D8;(_tmp6D8.YY44).tag=48;(_tmp6D8.YY44).f1=(void*)((void*)({struct Cyc_Absyn_RgnsEff_struct*
_tmp6D9=_cycalloc(sizeof(*_tmp6D9));_tmp6D9[0]=({struct Cyc_Absyn_RgnsEff_struct
_tmp6DA;_tmp6DA.tag=21;_tmp6DA.f1=(void*)Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp6DA;});_tmp6D9;}));
_tmp6D8;});break;case 256: _LL300: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp6DB;(_tmp6DB.YY44).tag=48;(_tmp6DB.YY44).f1=(void*)((void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp6DC=_cycalloc(sizeof(*_tmp6DC));_tmp6DC[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp6DD;_tmp6DD.tag=20;_tmp6DD.f1=({struct Cyc_List_List*_tmp6DE=_cycalloc(
sizeof(*_tmp6DE));_tmp6DE->hd=(void*)Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp6DE->tl=Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6DE;});_tmp6DD;});
_tmp6DC;}));_tmp6DB;});break;case 257: _LL301: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp6DF;(_tmp6DF.YY40).tag=44;(_tmp6DF.YY40).f1=({struct
Cyc_List_List*_tmp6E0=_cycalloc(sizeof(*_tmp6E0));_tmp6E0->hd=(void*)Cyc_yyget_YY44(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6E0->tl=0;_tmp6E0;});
_tmp6DF;});break;case 258: _LL302: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp6E1;(_tmp6E1.YY40).tag=44;(_tmp6E1.YY40).f1=({struct Cyc_List_List*_tmp6E2=
_cycalloc(sizeof(*_tmp6E2));_tmp6E2->hd=(void*)Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6E2->tl=Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp6E2;});_tmp6E1;});
break;case 259: _LL303: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp6E3;(_tmp6E3.YY30).tag=34;(_tmp6E3.YY30).f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp6E4=_cycalloc(sizeof(*_tmp6E4));_tmp6E4->tms=Cyc_yyget_YY28(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6E4;});_tmp6E3;});
break;case 260: _LL304: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 261: _LL305: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp6E5;(_tmp6E5.YY30).tag=34;(_tmp6E5.YY30).f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp6E6=_cycalloc(sizeof(*_tmp6E6));_tmp6E6->tms=Cyc_List_imp_append(Cyc_yyget_YY28(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_YY30(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tms);_tmp6E6;});
_tmp6E5;});break;case 262: _LL306: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)];break;case 263: _LL307: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp6E7;(_tmp6E7.YY30).tag=34;(_tmp6E7.YY30).f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp6E8=_cycalloc(sizeof(*_tmp6E8));_tmp6E8->tms=({struct Cyc_List_List*_tmp6E9=
_cycalloc(sizeof(*_tmp6E9));_tmp6E9->hd=(void*)((void*)({struct Cyc_Absyn_Carray_mod_struct*
_tmp6EA=_cycalloc(sizeof(*_tmp6EA));_tmp6EA[0]=({struct Cyc_Absyn_Carray_mod_struct
_tmp6EB;_tmp6EB.tag=0;_tmp6EB.f1=Cyc_yyget_YY51(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6EB.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp6EB;});
_tmp6EA;}));_tmp6E9->tl=0;_tmp6E9;});_tmp6E8;});_tmp6E7;});break;case 264: _LL308:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp6EC;(_tmp6EC.YY30).tag=
34;(_tmp6EC.YY30).f1=({struct Cyc_Parse_Abstractdeclarator*_tmp6ED=_cycalloc(
sizeof(*_tmp6ED));_tmp6ED->tms=({struct Cyc_List_List*_tmp6EE=_cycalloc(sizeof(*
_tmp6EE));_tmp6EE->hd=(void*)((void*)({struct Cyc_Absyn_Carray_mod_struct*_tmp6EF=
_cycalloc(sizeof(*_tmp6EF));_tmp6EF[0]=({struct Cyc_Absyn_Carray_mod_struct
_tmp6F0;_tmp6F0.tag=0;_tmp6F0.f1=Cyc_yyget_YY51(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6F0.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp6F0;});
_tmp6EF;}));_tmp6EE->tl=(Cyc_yyget_YY30(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]))->tms;_tmp6EE;});_tmp6ED;});_tmp6EC;});break;case 265:
_LL309: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp6F1;(_tmp6F1.YY30).tag=
34;(_tmp6F1.YY30).f1=({struct Cyc_Parse_Abstractdeclarator*_tmp6F2=_cycalloc(
sizeof(*_tmp6F2));_tmp6F2->tms=({struct Cyc_List_List*_tmp6F3=_cycalloc(sizeof(*
_tmp6F3));_tmp6F3->hd=(void*)((void*)({struct Cyc_Absyn_ConstArray_mod_struct*
_tmp6F4=_cycalloc(sizeof(*_tmp6F4));_tmp6F4[0]=({struct Cyc_Absyn_ConstArray_mod_struct
_tmp6F5;_tmp6F5.tag=1;_tmp6F5.f1=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp6F5.f2=Cyc_yyget_YY51(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6F5.f3=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp6F5;});
_tmp6F4;}));_tmp6F3->tl=0;_tmp6F3;});_tmp6F2;});_tmp6F1;});break;case 266: _LL30A:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp6F6;(_tmp6F6.YY30).tag=
34;(_tmp6F6.YY30).f1=({struct Cyc_Parse_Abstractdeclarator*_tmp6F7=_cycalloc(
sizeof(*_tmp6F7));_tmp6F7->tms=({struct Cyc_List_List*_tmp6F8=_cycalloc(sizeof(*
_tmp6F8));_tmp6F8->hd=(void*)((void*)({struct Cyc_Absyn_ConstArray_mod_struct*
_tmp6F9=_cycalloc(sizeof(*_tmp6F9));_tmp6F9[0]=({struct Cyc_Absyn_ConstArray_mod_struct
_tmp6FA;_tmp6FA.tag=1;_tmp6FA.f1=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp6FA.f2=Cyc_yyget_YY51(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6FA.f3=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp6FA;});
_tmp6F9;}));_tmp6F8->tl=(Cyc_yyget_YY30(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 4)]))->tms;_tmp6F8;});_tmp6F7;});_tmp6F6;});break;case 267:
_LL30B: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp6FB;(_tmp6FB.YY30).tag=
34;(_tmp6FB.YY30).f1=({struct Cyc_Parse_Abstractdeclarator*_tmp6FC=_cycalloc(
sizeof(*_tmp6FC));_tmp6FC->tms=({struct Cyc_List_List*_tmp6FD=_cycalloc(sizeof(*
_tmp6FD));_tmp6FD->hd=(void*)((void*)({struct Cyc_Absyn_Function_mod_struct*
_tmp6FE=_cycalloc(sizeof(*_tmp6FE));_tmp6FE[0]=({struct Cyc_Absyn_Function_mod_struct
_tmp6FF;_tmp6FF.tag=3;_tmp6FF.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*
_tmp700=_cycalloc(sizeof(*_tmp700));_tmp700[0]=({struct Cyc_Absyn_WithTypes_struct
_tmp701;_tmp701.tag=1;_tmp701.f1=0;_tmp701.f2=0;_tmp701.f3=0;_tmp701.f4=Cyc_yyget_YY49(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp701.f5=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp701;});_tmp700;}));
_tmp6FF;});_tmp6FE;}));_tmp6FD->tl=0;_tmp6FD;});_tmp6FC;});_tmp6FB;});break;case
268: _LL30C: {struct _tuple17 _tmp703;struct Cyc_List_List*_tmp704;int _tmp705;struct
Cyc_Absyn_VarargInfo*_tmp706;struct Cyc_Core_Opt*_tmp707;struct Cyc_List_List*
_tmp708;struct _tuple17*_tmp702=Cyc_yyget_YY39(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp703=*_tmp702;_tmp704=
_tmp703.f1;_tmp705=_tmp703.f2;_tmp706=_tmp703.f3;_tmp707=_tmp703.f4;_tmp708=
_tmp703.f5;yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp709;(
_tmp709.YY30).tag=34;(_tmp709.YY30).f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp70A=_cycalloc(sizeof(*_tmp70A));_tmp70A->tms=({struct Cyc_List_List*_tmp70B=
_cycalloc(sizeof(*_tmp70B));_tmp70B->hd=(void*)((void*)({struct Cyc_Absyn_Function_mod_struct*
_tmp70C=_cycalloc(sizeof(*_tmp70C));_tmp70C[0]=({struct Cyc_Absyn_Function_mod_struct
_tmp70D;_tmp70D.tag=3;_tmp70D.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*
_tmp70E=_cycalloc(sizeof(*_tmp70E));_tmp70E[0]=({struct Cyc_Absyn_WithTypes_struct
_tmp70F;_tmp70F.tag=1;_tmp70F.f1=_tmp704;_tmp70F.f2=_tmp705;_tmp70F.f3=_tmp706;
_tmp70F.f4=_tmp707;_tmp70F.f5=_tmp708;_tmp70F;});_tmp70E;}));_tmp70D;});_tmp70C;}));
_tmp70B->tl=0;_tmp70B;});_tmp70A;});_tmp709;});break;}case 269: _LL30D: yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp710;(_tmp710.YY30).tag=34;(
_tmp710.YY30).f1=({struct Cyc_Parse_Abstractdeclarator*_tmp711=_cycalloc(sizeof(*
_tmp711));_tmp711->tms=({struct Cyc_List_List*_tmp712=_cycalloc(sizeof(*_tmp712));
_tmp712->hd=(void*)((void*)({struct Cyc_Absyn_Function_mod_struct*_tmp713=
_cycalloc(sizeof(*_tmp713));_tmp713[0]=({struct Cyc_Absyn_Function_mod_struct
_tmp714;_tmp714.tag=3;_tmp714.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*
_tmp715=_cycalloc(sizeof(*_tmp715));_tmp715[0]=({struct Cyc_Absyn_WithTypes_struct
_tmp716;_tmp716.tag=1;_tmp716.f1=0;_tmp716.f2=0;_tmp716.f3=0;_tmp716.f4=Cyc_yyget_YY49(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp716.f5=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp716;});_tmp715;}));
_tmp714;});_tmp713;}));_tmp712->tl=(Cyc_yyget_YY30(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->tms;_tmp712;});_tmp711;});
_tmp710;});break;case 270: _LL30E: {struct _tuple17 _tmp718;struct Cyc_List_List*
_tmp719;int _tmp71A;struct Cyc_Absyn_VarargInfo*_tmp71B;struct Cyc_Core_Opt*_tmp71C;
struct Cyc_List_List*_tmp71D;struct _tuple17*_tmp717=Cyc_yyget_YY39(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp718=*_tmp717;_tmp719=
_tmp718.f1;_tmp71A=_tmp718.f2;_tmp71B=_tmp718.f3;_tmp71C=_tmp718.f4;_tmp71D=
_tmp718.f5;yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp71E;(
_tmp71E.YY30).tag=34;(_tmp71E.YY30).f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp71F=_cycalloc(sizeof(*_tmp71F));_tmp71F->tms=({struct Cyc_List_List*_tmp720=
_cycalloc(sizeof(*_tmp720));_tmp720->hd=(void*)((void*)({struct Cyc_Absyn_Function_mod_struct*
_tmp721=_cycalloc(sizeof(*_tmp721));_tmp721[0]=({struct Cyc_Absyn_Function_mod_struct
_tmp722;_tmp722.tag=3;_tmp722.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*
_tmp723=_cycalloc(sizeof(*_tmp723));_tmp723[0]=({struct Cyc_Absyn_WithTypes_struct
_tmp724;_tmp724.tag=1;_tmp724.f1=_tmp719;_tmp724.f2=_tmp71A;_tmp724.f3=_tmp71B;
_tmp724.f4=_tmp71C;_tmp724.f5=_tmp71D;_tmp724;});_tmp723;}));_tmp722;});_tmp721;}));
_tmp720->tl=(Cyc_yyget_YY30(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]))->tms;_tmp720;});_tmp71F;});_tmp71E;});break;}case 271: _LL30F: {
struct Cyc_List_List*_tmp725=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(
struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*
x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),Cyc_List_imp_rev(
Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp726;(_tmp726.YY30).tag=
34;(_tmp726.YY30).f1=({struct Cyc_Parse_Abstractdeclarator*_tmp727=_cycalloc(
sizeof(*_tmp727));_tmp727->tms=({struct Cyc_List_List*_tmp728=_cycalloc(sizeof(*
_tmp728));_tmp728->hd=(void*)((void*)({struct Cyc_Absyn_TypeParams_mod_struct*
_tmp729=_cycalloc(sizeof(*_tmp729));_tmp729[0]=({struct Cyc_Absyn_TypeParams_mod_struct
_tmp72A;_tmp72A.tag=4;_tmp72A.f1=_tmp725;_tmp72A.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp72A.f3=0;
_tmp72A;});_tmp729;}));_tmp728->tl=(Cyc_yyget_YY30(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp728;});_tmp727;});
_tmp726;});break;}case 272: _LL310: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp72B;(_tmp72B.YY30).tag=34;(_tmp72B.YY30).f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp72C=_cycalloc(sizeof(*_tmp72C));_tmp72C->tms=({struct Cyc_List_List*_tmp72D=
_cycalloc(sizeof(*_tmp72D));_tmp72D->hd=(void*)((void*)({struct Cyc_Absyn_Attributes_mod_struct*
_tmp72E=_cycalloc(sizeof(*_tmp72E));_tmp72E[0]=({struct Cyc_Absyn_Attributes_mod_struct
_tmp72F;_tmp72F.tag=5;_tmp72F.f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp72F.f2=Cyc_yyget_YY45(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp72F;});_tmp72E;}));
_tmp72D->tl=(Cyc_yyget_YY30(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]))->tms;_tmp72D;});_tmp72C;});_tmp72B;});break;case 273: _LL311:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 274:
_LL312: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
275: _LL313: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;
case 276: _LL314: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];
break;case 277: _LL315: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 278: _LL316: yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)];break;case 279: _LL317: yyval=(union Cyc_YYSTYPE_union)({union
Cyc_YYSTYPE_union _tmp730;(_tmp730.YY9).tag=13;(_tmp730.YY9).f1=Cyc_Absyn_new_stmt((
void*)({struct Cyc_Absyn_ResetRegion_s_struct*_tmp731=_cycalloc(sizeof(*_tmp731));
_tmp731[0]=({struct Cyc_Absyn_ResetRegion_s_struct _tmp732;_tmp732.tag=15;_tmp732.f1=
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);
_tmp732;});_tmp731;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp730;});
break;case 280: _LL318: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp733;(_tmp733.YY4).tag=8;(_tmp733.YY4).f1=0;_tmp733;});break;case 281: _LL319:
if(Cyc_zstrcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),({const char*_tmp734="open";
_tag_dyneither(_tmp734,sizeof(char),5);}))!= 0)Cyc_Parse_err(({const char*_tmp735="expecting `open'";
_tag_dyneither(_tmp735,sizeof(char),17);}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));yyval=(union
Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp736;(_tmp736.YY4).tag=8;(_tmp736.YY4).f1=(
struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp736;});break;case 282: _LL31A: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp737;(_tmp737.YY9).tag=13;(_tmp737.YY9).f1=Cyc_Absyn_new_stmt((
void*)({struct Cyc_Absyn_Label_s_struct*_tmp738=_cycalloc(sizeof(*_tmp738));
_tmp738[0]=({struct Cyc_Absyn_Label_s_struct _tmp739;_tmp739.tag=12;_tmp739.f1=({
struct _dyneither_ptr*_tmp73A=_cycalloc(sizeof(*_tmp73A));_tmp73A[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp73A;});_tmp739.f2=
Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp739;});
_tmp738;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp737;});break;case 283: _LL31B: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp73B;(_tmp73B.YY9).tag=13;(_tmp73B.YY9).f1=Cyc_Absyn_skip_stmt(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp73B;});break;case 284: _LL31C: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp73C;(_tmp73C.YY9).tag=13;(_tmp73C.YY9).f1=Cyc_Absyn_exp_stmt(
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp73C;});
break;case 285: _LL31D: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp73D;(_tmp73D.YY9).tag=13;(_tmp73D.YY9).f1=Cyc_Absyn_skip_stmt(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp73D;});
break;case 286: _LL31E: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)];break;case 287: _LL31F: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp73E;(_tmp73E.YY9).tag=13;(_tmp73E.YY9).f1=Cyc_Parse_flatten_declarations(Cyc_yyget_YY18(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Absyn_skip_stmt(0));
_tmp73E;});break;case 288: _LL320: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp73F;(_tmp73F.YY9).tag=13;(_tmp73F.YY9).f1=Cyc_Parse_flatten_declarations(Cyc_yyget_YY18(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp73F;});break;case 289:
_LL321: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
290: _LL322: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp740;(
_tmp740.YY9).tag=13;(_tmp740.YY9).f1=Cyc_Absyn_seq_stmt(Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp740;});
break;case 291: _LL323: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp741;(_tmp741.YY9).tag=13;(_tmp741.YY9).f1=Cyc_Parse_flatten_decl(Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Fn_d_struct*_tmp742=_cycalloc(sizeof(*_tmp742));_tmp742[
0]=({struct Cyc_Absyn_Fn_d_struct _tmp743;_tmp743.tag=1;_tmp743.f1=Cyc_yyget_YY17(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp743;});_tmp742;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),Cyc_Absyn_skip_stmt(0));_tmp741;});break;case 292:
_LL324: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp744;(_tmp744.YY9).tag=
13;(_tmp744.YY9).f1=Cyc_Parse_flatten_decl(Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Fn_d_struct*
_tmp745=_cycalloc(sizeof(*_tmp745));_tmp745[0]=({struct Cyc_Absyn_Fn_d_struct
_tmp746;_tmp746.tag=1;_tmp746.f1=Cyc_yyget_YY17(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp746;});_tmp745;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line)),Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]));_tmp744;});break;case 293: _LL325: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp747;(_tmp747.YY9).tag=13;(_tmp747.YY9).f1=Cyc_Absyn_ifthenelse_stmt(
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Absyn_skip_stmt(0),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp747;});break;case 294: _LL326: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp748;(_tmp748.YY9).tag=13;(_tmp748.YY9).f1=Cyc_Absyn_ifthenelse_stmt(
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp748;});
break;case 295: _LL327: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp749;(_tmp749.YY9).tag=13;(_tmp749.YY9).f1=Cyc_Absyn_switch_stmt(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_YY10(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp749;});
break;case 296: _LL328: {struct Cyc_Absyn_Exp*_tmp74A=Cyc_Absyn_new_exp((void*)({
struct Cyc_Absyn_UnknownId_e_struct*_tmp74C=_cycalloc(sizeof(*_tmp74C));_tmp74C[0]=({
struct Cyc_Absyn_UnknownId_e_struct _tmp74D;_tmp74D.tag=2;_tmp74D.f1=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp74D;});_tmp74C;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line));yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp74B;(_tmp74B.YY9).tag=13;(_tmp74B.YY9).f1=Cyc_Absyn_switch_stmt(_tmp74A,Cyc_yyget_YY10(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp74B;});
break;}case 297: _LL329: {struct Cyc_Absyn_Exp*_tmp74E=Cyc_Absyn_tuple_exp(Cyc_yyget_YY5(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));yyval=(union
Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp74F;(_tmp74F.YY9).tag=13;(_tmp74F.YY9).f1=
Cyc_Absyn_switch_stmt(_tmp74E,Cyc_yyget_YY10(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp74F;});
break;}case 298: _LL32A: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp750;(_tmp750.YY9).tag=13;(_tmp750.YY9).f1=Cyc_Absyn_trycatch_stmt(Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_YY10(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp750;});
break;case 299: _LL32B: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp751;(_tmp751.YY10).tag=14;(_tmp751.YY10).f1=0;_tmp751;});break;case 300:
_LL32C: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp752;(_tmp752.YY10).tag=
14;(_tmp752.YY10).f1=({struct Cyc_List_List*_tmp753=_cycalloc(sizeof(*_tmp753));
_tmp753->hd=({struct Cyc_Absyn_Switch_clause*_tmp754=_cycalloc(sizeof(*_tmp754));
_tmp754->pattern=Cyc_Absyn_new_pat((void*)0,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line));_tmp754->pat_vars=
0;_tmp754->where_clause=0;_tmp754->body=Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp754->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp754;});
_tmp753->tl=0;_tmp753;});_tmp752;});break;case 301: _LL32D: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp755;(_tmp755.YY10).tag=14;(_tmp755.YY10).f1=({struct
Cyc_List_List*_tmp756=_cycalloc(sizeof(*_tmp756));_tmp756->hd=({struct Cyc_Absyn_Switch_clause*
_tmp757=_cycalloc(sizeof(*_tmp757));_tmp757->pattern=Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp757->pat_vars=0;
_tmp757->where_clause=0;_tmp757->body=Cyc_Absyn_fallthru_stmt(0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp757->loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp757;});_tmp756->tl=Cyc_yyget_YY10(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp756;});_tmp755;});
break;case 302: _LL32E: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp758;(_tmp758.YY10).tag=14;(_tmp758.YY10).f1=({struct Cyc_List_List*_tmp759=
_cycalloc(sizeof(*_tmp759));_tmp759->hd=({struct Cyc_Absyn_Switch_clause*_tmp75A=
_cycalloc(sizeof(*_tmp75A));_tmp75A->pattern=Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp75A->pat_vars=0;
_tmp75A->where_clause=0;_tmp75A->body=Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp75A->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line);_tmp75A;});
_tmp759->tl=Cyc_yyget_YY10(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp759;});_tmp758;});break;case 303: _LL32F: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp75B;(_tmp75B.YY10).tag=14;(_tmp75B.YY10).f1=({struct
Cyc_List_List*_tmp75C=_cycalloc(sizeof(*_tmp75C));_tmp75C->hd=({struct Cyc_Absyn_Switch_clause*
_tmp75D=_cycalloc(sizeof(*_tmp75D));_tmp75D->pattern=Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp75D->pat_vars=0;
_tmp75D->where_clause=(struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp75D->body=Cyc_Absyn_fallthru_stmt(
0,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));_tmp75D->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp75D;});
_tmp75C->tl=Cyc_yyget_YY10(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp75C;});_tmp75B;});break;case 304: _LL330: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp75E;(_tmp75E.YY10).tag=14;(_tmp75E.YY10).f1=({struct
Cyc_List_List*_tmp75F=_cycalloc(sizeof(*_tmp75F));_tmp75F->hd=({struct Cyc_Absyn_Switch_clause*
_tmp760=_cycalloc(sizeof(*_tmp760));_tmp760->pattern=Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 5)]);_tmp760->pat_vars=0;
_tmp760->where_clause=(struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp760->body=Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp760->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp760;});
_tmp75F->tl=Cyc_yyget_YY10(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp75F;});_tmp75E;});break;case 305: _LL331: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp761;(_tmp761.YY9).tag=13;(_tmp761.YY9).f1=Cyc_Absyn_while_stmt(
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp761;});
break;case 306: _LL332: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp762;(_tmp762.YY9).tag=13;(_tmp762.YY9).f1=Cyc_Absyn_do_stmt(Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp762;});
break;case 307: _LL333: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp763;(_tmp763.YY9).tag=13;(_tmp763.YY9).f1=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(
0),Cyc_Absyn_true_exp(0),Cyc_Absyn_false_exp(0),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp763;});
break;case 308: _LL334: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp764;(_tmp764.YY9).tag=13;(_tmp764.YY9).f1=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(
0),Cyc_Absyn_true_exp(0),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]),Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 6)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp764;});break;case 309: _LL335: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp765;(_tmp765.YY9).tag=13;(_tmp765.YY9).f1=Cyc_Absyn_for_stmt(
Cyc_Absyn_false_exp(0),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]),Cyc_Absyn_false_exp(0),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp765;});
break;case 310: _LL336: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp766;(_tmp766.YY9).tag=13;(_tmp766.YY9).f1=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(
0),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp766;});
break;case 311: _LL337: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp767;(_tmp767.YY9).tag=13;(_tmp767.YY9).f1=Cyc_Absyn_for_stmt(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_Absyn_true_exp(
0),Cyc_Absyn_false_exp(0),Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp767;});
break;case 312: _LL338: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp768;(_tmp768.YY9).tag=13;(_tmp768.YY9).f1=Cyc_Absyn_for_stmt(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]),Cyc_Absyn_true_exp(
0),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp768;});
break;case 313: _LL339: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp769;(_tmp769.YY9).tag=13;(_tmp769.YY9).f1=Cyc_Absyn_for_stmt(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_Absyn_false_exp(0),
Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp769;});
break;case 314: _LL33A: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp76A;(_tmp76A.YY9).tag=13;(_tmp76A.YY9).f1=Cyc_Absyn_for_stmt(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 8)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp76A;});
break;case 315: _LL33B: {struct Cyc_List_List*_tmp76B=Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);struct Cyc_Absyn_Stmt*
_tmp76C=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(0),Cyc_Absyn_false_exp(
0),Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp76D;(_tmp76D.YY9).tag=13;(_tmp76D.YY9).f1=Cyc_Parse_flatten_declarations(
_tmp76B,_tmp76C);_tmp76D;});break;}case 316: _LL33C: {struct Cyc_List_List*_tmp76E=
Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);
struct Cyc_Absyn_Stmt*_tmp76F=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_Absyn_false_exp(
0),Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp770;(_tmp770.YY9).tag=13;(_tmp770.YY9).f1=Cyc_Parse_flatten_declarations(
_tmp76E,_tmp76F);_tmp770;});break;}case 317: _LL33D: {struct Cyc_List_List*_tmp771=
Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);
struct Cyc_Absyn_Stmt*_tmp772=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(
0),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp773;(_tmp773.YY9).tag=13;(_tmp773.YY9).f1=Cyc_Parse_flatten_declarations(
_tmp771,_tmp772);_tmp773;});break;}case 318: _LL33E: {struct Cyc_List_List*_tmp774=
Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]);
struct Cyc_Absyn_Stmt*_tmp775=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp776;(_tmp776.YY9).tag=13;(_tmp776.YY9).f1=Cyc_Parse_flatten_declarations(
_tmp774,_tmp775);_tmp776;});break;}case 319: _LL33F: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp777;(_tmp777.YY9).tag=13;(_tmp777.YY9).f1=Cyc_Absyn_goto_stmt(({
struct _dyneither_ptr*_tmp778=_cycalloc(sizeof(*_tmp778));_tmp778[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp778;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp777;});
break;case 320: _LL340: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp779;(_tmp779.YY9).tag=13;(_tmp779.YY9).f1=Cyc_Absyn_continue_stmt(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp779;});
break;case 321: _LL341: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp77A;(_tmp77A.YY9).tag=13;(_tmp77A.YY9).f1=Cyc_Absyn_break_stmt(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp77A;});
break;case 322: _LL342: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp77B;(_tmp77B.YY9).tag=13;(_tmp77B.YY9).f1=Cyc_Absyn_return_stmt(0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp77B;});
break;case 323: _LL343: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp77C;(_tmp77C.YY9).tag=13;(_tmp77C.YY9).f1=Cyc_Absyn_return_stmt((struct Cyc_Absyn_Exp*)
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp77C;});
break;case 324: _LL344: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp77D;(_tmp77D.YY9).tag=13;(_tmp77D.YY9).f1=Cyc_Absyn_fallthru_stmt(0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp77D;});
break;case 325: _LL345: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp77E;(_tmp77E.YY9).tag=13;(_tmp77E.YY9).f1=Cyc_Absyn_fallthru_stmt(0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));_tmp77E;});
break;case 326: _LL346: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp77F;(_tmp77F.YY9).tag=13;(_tmp77F.YY9).f1=Cyc_Absyn_fallthru_stmt(Cyc_yyget_YY5(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).last_line));_tmp77F;});
break;case 327: _LL347: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 328: _LL348: yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)];break;case 329: _LL349: yyval=(union Cyc_YYSTYPE_union)({union
Cyc_YYSTYPE_union _tmp780;(_tmp780.YY11).tag=15;(_tmp780.YY11).f1=Cyc_Absyn_exp_pat(
Cyc_Absyn_conditional_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)])),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp780;});
break;case 330: _LL34A: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 331: _LL34B: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp781;(_tmp781.YY11).tag=15;(_tmp781.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_or_exp(
Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));_tmp781;});break;case 332: _LL34C: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 333: _LL34D: yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp782;(_tmp782.YY11).tag=15;(
_tmp782.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_and_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY11(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp782;});
break;case 334: _LL34E: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 335: _LL34F: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp783;(_tmp783.YY11).tag=15;(_tmp783.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((
void*)14,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp783;});
break;case 336: _LL350: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 337: _LL351: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp784;(_tmp784.YY11).tag=15;(_tmp784.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((
void*)15,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp784;});
break;case 338: _LL352: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 339: _LL353: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp785;(_tmp785.YY11).tag=15;(_tmp785.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((
void*)13,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp785;});
break;case 340: _LL354: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 341: _LL355: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp786;(_tmp786.YY11).tag=15;(_tmp786.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_eq_exp(
Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));_tmp786;});break;case 342: _LL356: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp787;(_tmp787.YY11).tag=15;(_tmp787.YY11).f1=Cyc_Absyn_exp_pat(
Cyc_Absyn_neq_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp787;});
break;case 343: _LL357: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 344: _LL358: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp788;(_tmp788.YY11).tag=15;(_tmp788.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_lt_exp(
Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));_tmp788;});break;case 345: _LL359: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp789;(_tmp789.YY11).tag=15;(_tmp789.YY11).f1=Cyc_Absyn_exp_pat(
Cyc_Absyn_gt_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp789;});
break;case 346: _LL35A: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp78A;(_tmp78A.YY11).tag=15;(_tmp78A.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_lte_exp(
Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));_tmp78A;});break;case 347: _LL35B: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp78B;(_tmp78B.YY11).tag=15;(_tmp78B.YY11).f1=Cyc_Absyn_exp_pat(
Cyc_Absyn_gte_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp78B;});
break;case 348: _LL35C: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 349: _LL35D: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp78C;(_tmp78C.YY11).tag=15;(_tmp78C.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((
void*)16,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp78C;});
break;case 350: _LL35E: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp78D;(_tmp78D.YY11).tag=15;(_tmp78D.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((
void*)17,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp78D;});
break;case 351: _LL35F: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 352: _LL360: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp78E;(_tmp78E.YY11).tag=15;(_tmp78E.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((
void*)0,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp78E;});
break;case 353: _LL361: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp78F;(_tmp78F.YY11).tag=15;(_tmp78F.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((
void*)2,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp78F;});
break;case 354: _LL362: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 355: _LL363: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp790;(_tmp790.YY11).tag=15;(_tmp790.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((
void*)1,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp790;});
break;case 356: _LL364: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp791;(_tmp791.YY11).tag=15;(_tmp791.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((
void*)3,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp791;});
break;case 357: _LL365: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp792;(_tmp792.YY11).tag=15;(_tmp792.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((
void*)4,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp792;});
break;case 358: _LL366: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 359: _LL367: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp793;(_tmp793.YY11).tag=15;(_tmp793.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_cast_exp((*
Cyc_yyget_YY37(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])).f3,
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),1,(void*)
0,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));_tmp793;});break;case 360: _LL368: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 361: _LL369: yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp794;(_tmp794.YY11).tag=15;(
_tmp794.YY11).f1=Cyc_Absyn_exp_pat(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp794;});break;case 362:
_LL36A: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp795;(_tmp795.YY11).tag=
15;(_tmp795.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_prim1_exp(Cyc_yyget_YY7(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp795;});
break;case 363: _LL36B: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp796;(_tmp796.YY11).tag=15;(_tmp796.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_sizeoftyp_exp((*
Cyc_yyget_YY37(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])).f3,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));_tmp796;});break;case 364: _LL36C: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp797;(_tmp797.YY11).tag=15;(_tmp797.YY11).f1=Cyc_Absyn_exp_pat(
Cyc_Absyn_sizeofexp_exp(Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));_tmp797;});break;case 365: _LL36D: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp798;(_tmp798.YY11).tag=15;(_tmp798.YY11).f1=Cyc_Absyn_exp_pat(
Cyc_Absyn_offsetof_exp((*Cyc_yyget_YY37(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)])).f3,(void*)({struct Cyc_Absyn_StructField_struct*_tmp799=
_cycalloc(sizeof(*_tmp799));_tmp799[0]=({struct Cyc_Absyn_StructField_struct
_tmp79A;_tmp79A.tag=0;_tmp79A.f1=({struct _dyneither_ptr*_tmp79B=_cycalloc(
sizeof(*_tmp79B));_tmp79B[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp79B;});_tmp79A;});
_tmp799;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));_tmp798;});break;case 366: _LL36E: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp79C;(_tmp79C.YY11).tag=15;(_tmp79C.YY11).f1=Cyc_Absyn_exp_pat(
Cyc_Absyn_offsetof_exp((*Cyc_yyget_YY37(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)])).f3,(void*)({struct Cyc_Absyn_TupleIndex_struct*_tmp79D=
_cycalloc_atomic(sizeof(*_tmp79D));_tmp79D[0]=({struct Cyc_Absyn_TupleIndex_struct
_tmp79E;_tmp79E.tag=1;_tmp79E.f1=(unsigned int)(Cyc_yyget_Int_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])).f2;_tmp79E;});_tmp79D;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));_tmp79C;});break;case 367: _LL36F: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 368: _LL370: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 369: _LL371:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp79F;(_tmp79F.YY11).tag=
15;(_tmp79F.YY11).f1=Cyc_Absyn_new_pat((void*)0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp79F;});
break;case 370: _LL372: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp7A0;(_tmp7A0.YY11).tag=15;(_tmp7A0.YY11).f1=Cyc_Absyn_exp_pat(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp7A0;});break;
case 371: _LL373: {struct Cyc_Absyn_Exp*e=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);{void*_tmp7A1=(void*)e->r;
union Cyc_Absyn_Cnst_union _tmp7A2;void*_tmp7A3;char _tmp7A4;union Cyc_Absyn_Cnst_union
_tmp7A5;void*_tmp7A6;short _tmp7A7;union Cyc_Absyn_Cnst_union _tmp7A8;void*_tmp7A9;
int _tmp7AA;union Cyc_Absyn_Cnst_union _tmp7AB;struct _dyneither_ptr _tmp7AC;union Cyc_Absyn_Cnst_union
_tmp7AD;union Cyc_Absyn_Cnst_union _tmp7AE;union Cyc_Absyn_Cnst_union _tmp7AF;_LL376:
if(*((int*)_tmp7A1)!= 0)goto _LL378;_tmp7A2=((struct Cyc_Absyn_Const_e_struct*)
_tmp7A1)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)_tmp7A1)->f1).Char_c).tag != 
0)goto _LL378;_tmp7A3=(_tmp7A2.Char_c).f1;_tmp7A4=(_tmp7A2.Char_c).f2;_LL377:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp7B0;(_tmp7B0.YY11).tag=
15;(_tmp7B0.YY11).f1=Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Char_p_struct*
_tmp7B1=_cycalloc_atomic(sizeof(*_tmp7B1));_tmp7B1[0]=({struct Cyc_Absyn_Char_p_struct
_tmp7B2;_tmp7B2.tag=8;_tmp7B2.f1=_tmp7A4;_tmp7B2;});_tmp7B1;}),e->loc);_tmp7B0;});
goto _LL375;_LL378: if(*((int*)_tmp7A1)!= 0)goto _LL37A;_tmp7A5=((struct Cyc_Absyn_Const_e_struct*)
_tmp7A1)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)_tmp7A1)->f1).Short_c).tag != 
1)goto _LL37A;_tmp7A6=(_tmp7A5.Short_c).f1;_tmp7A7=(_tmp7A5.Short_c).f2;_LL379:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp7B3;(_tmp7B3.YY11).tag=
15;(_tmp7B3.YY11).f1=Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Int_p_struct*
_tmp7B4=_cycalloc(sizeof(*_tmp7B4));_tmp7B4[0]=({struct Cyc_Absyn_Int_p_struct
_tmp7B5;_tmp7B5.tag=7;_tmp7B5.f1=(void*)_tmp7A6;_tmp7B5.f2=(int)_tmp7A7;_tmp7B5;});
_tmp7B4;}),e->loc);_tmp7B3;});goto _LL375;_LL37A: if(*((int*)_tmp7A1)!= 0)goto
_LL37C;_tmp7A8=((struct Cyc_Absyn_Const_e_struct*)_tmp7A1)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)
_tmp7A1)->f1).Int_c).tag != 2)goto _LL37C;_tmp7A9=(_tmp7A8.Int_c).f1;_tmp7AA=(
_tmp7A8.Int_c).f2;_LL37B: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp7B6;(_tmp7B6.YY11).tag=15;(_tmp7B6.YY11).f1=Cyc_Absyn_new_pat((void*)({
struct Cyc_Absyn_Int_p_struct*_tmp7B7=_cycalloc(sizeof(*_tmp7B7));_tmp7B7[0]=({
struct Cyc_Absyn_Int_p_struct _tmp7B8;_tmp7B8.tag=7;_tmp7B8.f1=(void*)_tmp7A9;
_tmp7B8.f2=_tmp7AA;_tmp7B8;});_tmp7B7;}),e->loc);_tmp7B6;});goto _LL375;_LL37C:
if(*((int*)_tmp7A1)!= 0)goto _LL37E;_tmp7AB=((struct Cyc_Absyn_Const_e_struct*)
_tmp7A1)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)_tmp7A1)->f1).Float_c).tag != 
4)goto _LL37E;_tmp7AC=(_tmp7AB.Float_c).f1;_LL37D: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp7B9;(_tmp7B9.YY11).tag=15;(_tmp7B9.YY11).f1=Cyc_Absyn_new_pat((
void*)({struct Cyc_Absyn_Float_p_struct*_tmp7BA=_cycalloc(sizeof(*_tmp7BA));
_tmp7BA[0]=({struct Cyc_Absyn_Float_p_struct _tmp7BB;_tmp7BB.tag=9;_tmp7BB.f1=
_tmp7AC;_tmp7BB;});_tmp7BA;}),e->loc);_tmp7B9;});goto _LL375;_LL37E: if(*((int*)
_tmp7A1)!= 0)goto _LL380;_tmp7AD=((struct Cyc_Absyn_Const_e_struct*)_tmp7A1)->f1;
if(((((struct Cyc_Absyn_Const_e_struct*)_tmp7A1)->f1).Null_c).tag != 6)goto _LL380;
_LL37F: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp7BC;(_tmp7BC.YY11).tag=
15;(_tmp7BC.YY11).f1=Cyc_Absyn_new_pat((void*)1,e->loc);_tmp7BC;});goto _LL375;
_LL380: if(*((int*)_tmp7A1)!= 0)goto _LL382;_tmp7AE=((struct Cyc_Absyn_Const_e_struct*)
_tmp7A1)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)_tmp7A1)->f1).String_c).tag
!= 5)goto _LL382;_LL381: Cyc_Parse_err(({const char*_tmp7BD="strings cannot occur within patterns";
_tag_dyneither(_tmp7BD,sizeof(char),37);}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));goto _LL375;
_LL382: if(*((int*)_tmp7A1)!= 0)goto _LL384;_tmp7AF=((struct Cyc_Absyn_Const_e_struct*)
_tmp7A1)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)_tmp7A1)->f1).LongLong_c).tag
!= 3)goto _LL384;_LL383: Cyc_Parse_unimp(({const char*_tmp7BE="long long's in patterns";
_tag_dyneither(_tmp7BE,sizeof(char),24);}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));goto _LL375;
_LL384:;_LL385: Cyc_Parse_err(({const char*_tmp7BF="bad constant in case";
_tag_dyneither(_tmp7BF,sizeof(char),21);}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_LL375:;}break;}
case 372: _LL374: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp7C0;(
_tmp7C0.YY11).tag=15;(_tmp7C0.YY11).f1=Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_UnknownId_p_struct*
_tmp7C1=_cycalloc(sizeof(*_tmp7C1));_tmp7C1[0]=({struct Cyc_Absyn_UnknownId_p_struct
_tmp7C2;_tmp7C2.tag=12;_tmp7C2.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp7C2;});_tmp7C1;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp7C0;});
break;case 373: _LL386: if(Cyc_strcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),({const char*_tmp7C3="as";
_tag_dyneither(_tmp7C3,sizeof(char),3);}))!= 0)Cyc_Parse_err(({const char*_tmp7C4="expecting `as'";
_tag_dyneither(_tmp7C4,sizeof(char),15);}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));yyval=(union
Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp7C5;(_tmp7C5.YY11).tag=15;(_tmp7C5.YY11).f1=
Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Var_p_struct*_tmp7C6=_cycalloc(
sizeof(*_tmp7C6));_tmp7C6[0]=({struct Cyc_Absyn_Var_p_struct _tmp7C7;_tmp7C7.tag=0;
_tmp7C7.f1=Cyc_Absyn_new_vardecl(({struct _tuple1*_tmp7C8=_cycalloc(sizeof(*
_tmp7C8));_tmp7C8->f1=(union Cyc_Absyn_Nmspace_union)({union Cyc_Absyn_Nmspace_union
_tmp7CA;(_tmp7CA.Loc_n).tag=0;_tmp7CA;});_tmp7C8->f2=({struct _dyneither_ptr*
_tmp7C9=_cycalloc(sizeof(*_tmp7C9));_tmp7C9[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp7C9;});_tmp7C8;}),(
void*)0,0);_tmp7C7.f2=Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp7C7;});_tmp7C6;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line));_tmp7C5;});
break;case 374: _LL387: {struct Cyc_List_List*_tmp7CC;int _tmp7CD;struct _tuple13
_tmp7CB=*Cyc_yyget_YY12(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
1)]);_tmp7CC=_tmp7CB.f1;_tmp7CD=_tmp7CB.f2;yyval=(union Cyc_YYSTYPE_union)({union
Cyc_YYSTYPE_union _tmp7CE;(_tmp7CE.YY11).tag=15;(_tmp7CE.YY11).f1=Cyc_Absyn_new_pat((
void*)({struct Cyc_Absyn_Tuple_p_struct*_tmp7CF=_cycalloc(sizeof(*_tmp7CF));
_tmp7CF[0]=({struct Cyc_Absyn_Tuple_p_struct _tmp7D0;_tmp7D0.tag=3;_tmp7D0.f1=
_tmp7CC;_tmp7D0.f2=_tmp7CD;_tmp7D0;});_tmp7CF;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp7CE;});
break;}case 375: _LL388: {struct Cyc_List_List*_tmp7D2;int _tmp7D3;struct _tuple13
_tmp7D1=*Cyc_yyget_YY12(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
1)]);_tmp7D2=_tmp7D1.f1;_tmp7D3=_tmp7D1.f2;yyval=(union Cyc_YYSTYPE_union)({union
Cyc_YYSTYPE_union _tmp7D4;(_tmp7D4.YY11).tag=15;(_tmp7D4.YY11).f1=Cyc_Absyn_new_pat((
void*)({struct Cyc_Absyn_UnknownCall_p_struct*_tmp7D5=_cycalloc(sizeof(*_tmp7D5));
_tmp7D5[0]=({struct Cyc_Absyn_UnknownCall_p_struct _tmp7D6;_tmp7D6.tag=13;_tmp7D6.f1=
Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);
_tmp7D6.f2=_tmp7D2;_tmp7D6.f3=_tmp7D3;_tmp7D6;});_tmp7D5;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp7D4;});
break;}case 376: _LL389: {struct Cyc_List_List*_tmp7D8;int _tmp7D9;struct _tuple13
_tmp7D7=*Cyc_yyget_YY16(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
1)]);_tmp7D8=_tmp7D7.f1;_tmp7D9=_tmp7D7.f2;{struct Cyc_List_List*_tmp7DA=((struct
Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),
struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line),Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]));yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp7DB;(_tmp7DB.YY11).tag=15;(_tmp7DB.YY11).f1=Cyc_Absyn_new_pat((void*)({
struct Cyc_Absyn_Aggr_p_struct*_tmp7DC=_cycalloc(sizeof(*_tmp7DC));_tmp7DC[0]=({
struct Cyc_Absyn_Aggr_p_struct _tmp7DD;_tmp7DD.tag=5;_tmp7DD.f1=({struct Cyc_Absyn_AggrInfo
_tmp7DE;_tmp7DE.aggr_info=(union Cyc_Absyn_AggrInfoU_union)({union Cyc_Absyn_AggrInfoU_union
_tmp7DF;(_tmp7DF.UnknownAggr).tag=0;(_tmp7DF.UnknownAggr).f1=(void*)((void*)0);(
_tmp7DF.UnknownAggr).f2=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 4)]);_tmp7DF;});_tmp7DE.targs=0;_tmp7DE;});_tmp7DD.f2=
_tmp7DA;_tmp7DD.f3=_tmp7D8;_tmp7DD.f4=_tmp7D9;_tmp7DD;});_tmp7DC;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp7DB;});
break;}}case 377: _LL38A: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp7E0;(_tmp7E0.YY11).tag=15;(_tmp7E0.YY11).f1=Cyc_Absyn_new_pat((void*)({
struct Cyc_Absyn_Pointer_p_struct*_tmp7E1=_cycalloc(sizeof(*_tmp7E1));_tmp7E1[0]=({
struct Cyc_Absyn_Pointer_p_struct _tmp7E2;_tmp7E2.tag=4;_tmp7E2.f1=Cyc_yyget_YY11(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp7E2;});_tmp7E1;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp7E0;});break;case 378: _LL38B: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp7E3;(_tmp7E3.YY11).tag=15;(_tmp7E3.YY11).f1=Cyc_Absyn_new_pat((
void*)({struct Cyc_Absyn_Pointer_p_struct*_tmp7E4=_cycalloc(sizeof(*_tmp7E4));
_tmp7E4[0]=({struct Cyc_Absyn_Pointer_p_struct _tmp7E5;_tmp7E5.tag=4;_tmp7E5.f1=
Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Pointer_p_struct*_tmp7E6=_cycalloc(
sizeof(*_tmp7E6));_tmp7E6[0]=({struct Cyc_Absyn_Pointer_p_struct _tmp7E7;_tmp7E7.tag=
4;_tmp7E7.f1=Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp7E7;});_tmp7E6;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp7E5;});
_tmp7E4;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp7E3;});break;case 379: _LL38C: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp7E8;(_tmp7E8.YY11).tag=15;(_tmp7E8.YY11).f1=Cyc_Absyn_new_pat((
void*)({struct Cyc_Absyn_Reference_p_struct*_tmp7E9=_cycalloc(sizeof(*_tmp7E9));
_tmp7E9[0]=({struct Cyc_Absyn_Reference_p_struct _tmp7EA;_tmp7EA.tag=1;_tmp7EA.f1=
Cyc_Absyn_new_vardecl(({struct _tuple1*_tmp7EB=_cycalloc(sizeof(*_tmp7EB));
_tmp7EB->f1=(union Cyc_Absyn_Nmspace_union)({union Cyc_Absyn_Nmspace_union _tmp7ED;(
_tmp7ED.Loc_n).tag=0;_tmp7ED;});_tmp7EB->f2=({struct _dyneither_ptr*_tmp7EC=
_cycalloc(sizeof(*_tmp7EC));_tmp7EC[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp7EC;});_tmp7EB;}),(void*)
0,0);_tmp7EA.f2=Cyc_Absyn_new_pat((void*)0,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp7EA;});
_tmp7E9;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp7E8;});break;case 380: _LL38D: if(Cyc_strcmp((
struct _dyneither_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]),({const char*_tmp7EE="as";_tag_dyneither(_tmp7EE,sizeof(
char),3);}))!= 0)Cyc_Parse_err(({const char*_tmp7EF="expecting `as'";
_tag_dyneither(_tmp7EF,sizeof(char),15);}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));yyval=(union
Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp7F0;(_tmp7F0.YY11).tag=15;(_tmp7F0.YY11).f1=
Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Reference_p_struct*_tmp7F1=_cycalloc(
sizeof(*_tmp7F1));_tmp7F1[0]=({struct Cyc_Absyn_Reference_p_struct _tmp7F2;_tmp7F2.tag=
1;_tmp7F2.f1=Cyc_Absyn_new_vardecl(({struct _tuple1*_tmp7F3=_cycalloc(sizeof(*
_tmp7F3));_tmp7F3->f1=(union Cyc_Absyn_Nmspace_union)({union Cyc_Absyn_Nmspace_union
_tmp7F5;(_tmp7F5.Loc_n).tag=0;_tmp7F5;});_tmp7F3->f2=({struct _dyneither_ptr*
_tmp7F4=_cycalloc(sizeof(*_tmp7F4));_tmp7F4[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp7F4;});_tmp7F3;}),(
void*)0,0);_tmp7F2.f2=Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp7F2;});_tmp7F1;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line));_tmp7F0;});
break;case 381: _LL38E: {void*_tmp7F6=Cyc_Parse_id2type(Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Tcutil_kind_to_bound((
void*)7));yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp7F7;(_tmp7F7.YY11).tag=
15;(_tmp7F7.YY11).f1=Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_TagInt_p_struct*
_tmp7F8=_cycalloc(sizeof(*_tmp7F8));_tmp7F8[0]=({struct Cyc_Absyn_TagInt_p_struct
_tmp7F9;_tmp7F9.tag=2;_tmp7F9.f1=Cyc_Parse_typ2tvar(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),_tmp7F6);
_tmp7F9.f2=Cyc_Absyn_new_vardecl(({struct _tuple1*_tmp7FA=_cycalloc(sizeof(*
_tmp7FA));_tmp7FA->f1=(union Cyc_Absyn_Nmspace_union)({union Cyc_Absyn_Nmspace_union
_tmp7FC;(_tmp7FC.Loc_n).tag=0;_tmp7FC;});_tmp7FA->f2=({struct _dyneither_ptr*
_tmp7FB=_cycalloc(sizeof(*_tmp7FB));_tmp7FB[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp7FB;});_tmp7FA;}),(
void*)({struct Cyc_Absyn_TagType_struct*_tmp7FD=_cycalloc(sizeof(*_tmp7FD));
_tmp7FD[0]=({struct Cyc_Absyn_TagType_struct _tmp7FE;_tmp7FE.tag=18;_tmp7FE.f1=(
void*)_tmp7F6;_tmp7FE;});_tmp7FD;}),0);_tmp7F9;});_tmp7F8;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp7F7;});
break;}case 382: _LL38F: {struct Cyc_Absyn_Tvar*_tmp7FF=Cyc_Tcutil_new_tvar(Cyc_Tcutil_kind_to_bound((
void*)7));yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp800;(_tmp800.YY11).tag=
15;(_tmp800.YY11).f1=Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_TagInt_p_struct*
_tmp801=_cycalloc(sizeof(*_tmp801));_tmp801[0]=({struct Cyc_Absyn_TagInt_p_struct
_tmp802;_tmp802.tag=2;_tmp802.f1=_tmp7FF;_tmp802.f2=Cyc_Absyn_new_vardecl(({
struct _tuple1*_tmp803=_cycalloc(sizeof(*_tmp803));_tmp803->f1=(union Cyc_Absyn_Nmspace_union)({
union Cyc_Absyn_Nmspace_union _tmp805;(_tmp805.Loc_n).tag=0;_tmp805;});_tmp803->f2=({
struct _dyneither_ptr*_tmp804=_cycalloc(sizeof(*_tmp804));_tmp804[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp804;});_tmp803;}),(
void*)({struct Cyc_Absyn_TagType_struct*_tmp806=_cycalloc(sizeof(*_tmp806));
_tmp806[0]=({struct Cyc_Absyn_TagType_struct _tmp807;_tmp807.tag=18;_tmp807.f1=(
void*)((void*)({struct Cyc_Absyn_VarType_struct*_tmp808=_cycalloc(sizeof(*_tmp808));
_tmp808[0]=({struct Cyc_Absyn_VarType_struct _tmp809;_tmp809.tag=1;_tmp809.f1=
_tmp7FF;_tmp809;});_tmp808;}));_tmp807;});_tmp806;}),0);_tmp802;});_tmp801;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp800;});break;}case 383: _LL390: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp80A;(_tmp80A.YY12).tag=16;(_tmp80A.YY12).f1=({struct
_tuple13*_tmp80B=_cycalloc(sizeof(*_tmp80B));_tmp80B->f1=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY13(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp80B->f2=0;_tmp80B;});
_tmp80A;});break;case 384: _LL391: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp80C;(_tmp80C.YY12).tag=16;(_tmp80C.YY12).f1=({struct _tuple13*_tmp80D=
_cycalloc(sizeof(*_tmp80D));_tmp80D->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_rev)(Cyc_yyget_YY13(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]));_tmp80D->f2=1;_tmp80D;});_tmp80C;});break;case 385: _LL392:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp80E;(_tmp80E.YY12).tag=
16;(_tmp80E.YY12).f1=({struct _tuple13*_tmp80F=_cycalloc(sizeof(*_tmp80F));
_tmp80F->f1=0;_tmp80F->f2=1;_tmp80F;});_tmp80E;});break;case 386: _LL393: yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp810;(_tmp810.YY13).tag=17;(
_tmp810.YY13).f1=({struct Cyc_List_List*_tmp811=_cycalloc(sizeof(*_tmp811));
_tmp811->hd=Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp811->tl=0;_tmp811;});_tmp810;});break;case 387: _LL394: yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp812;(_tmp812.YY13).tag=17;(
_tmp812.YY13).f1=({struct Cyc_List_List*_tmp813=_cycalloc(sizeof(*_tmp813));
_tmp813->hd=Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp813->tl=Cyc_yyget_YY13(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]);_tmp813;});_tmp812;});break;case 388: _LL395: yyval=(union
Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp814;(_tmp814.YY14).tag=18;(_tmp814.YY14).f1=({
struct _tuple14*_tmp815=_cycalloc(sizeof(*_tmp815));_tmp815->f1=0;_tmp815->f2=Cyc_yyget_YY11(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp815;});_tmp814;});
break;case 389: _LL396: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp816;(_tmp816.YY14).tag=18;(_tmp816.YY14).f1=({struct _tuple14*_tmp817=
_cycalloc(sizeof(*_tmp817));_tmp817->f1=Cyc_yyget_YY41(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp817->f2=Cyc_yyget_YY11(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp817;});_tmp816;});
break;case 390: _LL397: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp818;(_tmp818.YY16).tag=20;(_tmp818.YY16).f1=({struct _tuple13*_tmp819=
_cycalloc(sizeof(*_tmp819));_tmp819->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_rev)(Cyc_yyget_YY15(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]));_tmp819->f2=0;_tmp819;});_tmp818;});break;case 391: _LL398: yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp81A;(_tmp81A.YY16).tag=20;(
_tmp81A.YY16).f1=({struct _tuple13*_tmp81B=_cycalloc(sizeof(*_tmp81B));_tmp81B->f1=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY15(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]));_tmp81B->f2=1;_tmp81B;});
_tmp81A;});break;case 392: _LL399: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp81C;(_tmp81C.YY16).tag=20;(_tmp81C.YY16).f1=({struct _tuple13*_tmp81D=
_cycalloc(sizeof(*_tmp81D));_tmp81D->f1=0;_tmp81D->f2=1;_tmp81D;});_tmp81C;});
break;case 393: _LL39A: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp81E;(_tmp81E.YY15).tag=19;(_tmp81E.YY15).f1=({struct Cyc_List_List*_tmp81F=
_cycalloc(sizeof(*_tmp81F));_tmp81F->hd=Cyc_yyget_YY14(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp81F->tl=0;_tmp81F;});
_tmp81E;});break;case 394: _LL39B: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp820;(_tmp820.YY15).tag=19;(_tmp820.YY15).f1=({struct Cyc_List_List*_tmp821=
_cycalloc(sizeof(*_tmp821));_tmp821->hd=Cyc_yyget_YY14(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp821->tl=Cyc_yyget_YY15(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp821;});_tmp820;});
break;case 395: _LL39C: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 396: _LL39D: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp822;(_tmp822.YY3).tag=7;(_tmp822.YY3).f1=Cyc_Absyn_seq_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp822;});
break;case 397: _LL39E: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 398: _LL39F: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp823;(_tmp823.YY3).tag=7;(_tmp823.YY3).f1=Cyc_Absyn_assignop_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY8(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp823;});
break;case 399: _LL3A0: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp824;(_tmp824.YY3).tag=7;(_tmp824.YY3).f1=Cyc_Absyn_new_exp((void*)({struct
Cyc_Absyn_Swap_e_struct*_tmp825=_cycalloc(sizeof(*_tmp825));_tmp825[0]=({struct
Cyc_Absyn_Swap_e_struct _tmp826;_tmp826.tag=36;_tmp826.f1=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp826.f2=Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp826;});_tmp825;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp824;});break;case 400: _LL3A1: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp827;(_tmp827.YY8).tag=12;(_tmp827.YY8).f1=0;_tmp827;});
break;case 401: _LL3A2: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp828;(_tmp828.YY8).tag=12;(_tmp828.YY8).f1=({struct Cyc_Core_Opt*_tmp829=
_cycalloc(sizeof(*_tmp829));_tmp829->v=(void*)((void*)1);_tmp829;});_tmp828;});
break;case 402: _LL3A3: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp82A;(_tmp82A.YY8).tag=12;(_tmp82A.YY8).f1=({struct Cyc_Core_Opt*_tmp82B=
_cycalloc(sizeof(*_tmp82B));_tmp82B->v=(void*)((void*)3);_tmp82B;});_tmp82A;});
break;case 403: _LL3A4: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp82C;(_tmp82C.YY8).tag=12;(_tmp82C.YY8).f1=({struct Cyc_Core_Opt*_tmp82D=
_cycalloc(sizeof(*_tmp82D));_tmp82D->v=(void*)((void*)4);_tmp82D;});_tmp82C;});
break;case 404: _LL3A5: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp82E;(_tmp82E.YY8).tag=12;(_tmp82E.YY8).f1=({struct Cyc_Core_Opt*_tmp82F=
_cycalloc(sizeof(*_tmp82F));_tmp82F->v=(void*)((void*)0);_tmp82F;});_tmp82E;});
break;case 405: _LL3A6: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp830;(_tmp830.YY8).tag=12;(_tmp830.YY8).f1=({struct Cyc_Core_Opt*_tmp831=
_cycalloc(sizeof(*_tmp831));_tmp831->v=(void*)((void*)2);_tmp831;});_tmp830;});
break;case 406: _LL3A7: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp832;(_tmp832.YY8).tag=12;(_tmp832.YY8).f1=({struct Cyc_Core_Opt*_tmp833=
_cycalloc(sizeof(*_tmp833));_tmp833->v=(void*)((void*)16);_tmp833;});_tmp832;});
break;case 407: _LL3A8: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp834;(_tmp834.YY8).tag=12;(_tmp834.YY8).f1=({struct Cyc_Core_Opt*_tmp835=
_cycalloc(sizeof(*_tmp835));_tmp835->v=(void*)((void*)17);_tmp835;});_tmp834;});
break;case 408: _LL3A9: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp836;(_tmp836.YY8).tag=12;(_tmp836.YY8).f1=({struct Cyc_Core_Opt*_tmp837=
_cycalloc(sizeof(*_tmp837));_tmp837->v=(void*)((void*)13);_tmp837;});_tmp836;});
break;case 409: _LL3AA: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp838;(_tmp838.YY8).tag=12;(_tmp838.YY8).f1=({struct Cyc_Core_Opt*_tmp839=
_cycalloc(sizeof(*_tmp839));_tmp839->v=(void*)((void*)15);_tmp839;});_tmp838;});
break;case 410: _LL3AB: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp83A;(_tmp83A.YY8).tag=12;(_tmp83A.YY8).f1=({struct Cyc_Core_Opt*_tmp83B=
_cycalloc(sizeof(*_tmp83B));_tmp83B->v=(void*)((void*)14);_tmp83B;});_tmp83A;});
break;case 411: _LL3AC: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 412: _LL3AD: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp83C;(_tmp83C.YY3).tag=7;(_tmp83C.YY3).f1=Cyc_Absyn_conditional_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp83C;});
break;case 413: _LL3AE: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp83D;(_tmp83D.YY3).tag=7;(_tmp83D.YY3).f1=Cyc_Absyn_throw_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp83D;});
break;case 414: _LL3AF: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp83E;(_tmp83E.YY3).tag=7;(_tmp83E.YY3).f1=Cyc_Absyn_New_exp(0,Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp83E;});
break;case 415: _LL3B0: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp83F;(_tmp83F.YY3).tag=7;(_tmp83F.YY3).f1=Cyc_Absyn_New_exp(0,Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp83F;});
break;case 416: _LL3B1: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp840;(_tmp840.YY3).tag=7;(_tmp840.YY3).f1=Cyc_Absyn_New_exp((struct Cyc_Absyn_Exp*)
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp840;});
break;case 417: _LL3B2: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp841;(_tmp841.YY3).tag=7;(_tmp841.YY3).f1=Cyc_Absyn_New_exp((struct Cyc_Absyn_Exp*)
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp841;});
break;case 418: _LL3B3: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 419: _LL3B4: yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)];break;case 420: _LL3B5: yyval=(union Cyc_YYSTYPE_union)({union
Cyc_YYSTYPE_union _tmp842;(_tmp842.YY3).tag=7;(_tmp842.YY3).f1=Cyc_Absyn_or_exp(
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp842;});
break;case 421: _LL3B6: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 422: _LL3B7: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp843;(_tmp843.YY3).tag=7;(_tmp843.YY3).f1=Cyc_Absyn_and_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp843;});
break;case 423: _LL3B8: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 424: _LL3B9: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp844;(_tmp844.YY3).tag=7;(_tmp844.YY3).f1=Cyc_Absyn_prim2_exp((void*)14,Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp844;});
break;case 425: _LL3BA: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 426: _LL3BB: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp845;(_tmp845.YY3).tag=7;(_tmp845.YY3).f1=Cyc_Absyn_prim2_exp((void*)15,Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp845;});
break;case 427: _LL3BC: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 428: _LL3BD: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp846;(_tmp846.YY3).tag=7;(_tmp846.YY3).f1=Cyc_Absyn_prim2_exp((void*)13,Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp846;});
break;case 429: _LL3BE: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 430: _LL3BF: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp847;(_tmp847.YY3).tag=7;(_tmp847.YY3).f1=Cyc_Absyn_eq_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp847;});
break;case 431: _LL3C0: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp848;(_tmp848.YY3).tag=7;(_tmp848.YY3).f1=Cyc_Absyn_neq_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp848;});
break;case 432: _LL3C1: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 433: _LL3C2: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp849;(_tmp849.YY3).tag=7;(_tmp849.YY3).f1=Cyc_Absyn_lt_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp849;});
break;case 434: _LL3C3: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp84A;(_tmp84A.YY3).tag=7;(_tmp84A.YY3).f1=Cyc_Absyn_gt_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp84A;});
break;case 435: _LL3C4: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp84B;(_tmp84B.YY3).tag=7;(_tmp84B.YY3).f1=Cyc_Absyn_lte_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp84B;});
break;case 436: _LL3C5: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp84C;(_tmp84C.YY3).tag=7;(_tmp84C.YY3).f1=Cyc_Absyn_gte_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp84C;});
break;case 437: _LL3C6: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 438: _LL3C7: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp84D;(_tmp84D.YY3).tag=7;(_tmp84D.YY3).f1=Cyc_Absyn_prim2_exp((void*)16,Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp84D;});
break;case 439: _LL3C8: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp84E;(_tmp84E.YY3).tag=7;(_tmp84E.YY3).f1=Cyc_Absyn_prim2_exp((void*)17,Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp84E;});
break;case 440: _LL3C9: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 441: _LL3CA: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp84F;(_tmp84F.YY3).tag=7;(_tmp84F.YY3).f1=Cyc_Absyn_prim2_exp((void*)0,Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp84F;});
break;case 442: _LL3CB: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp850;(_tmp850.YY3).tag=7;(_tmp850.YY3).f1=Cyc_Absyn_prim2_exp((void*)2,Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp850;});
break;case 443: _LL3CC: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 444: _LL3CD: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp851;(_tmp851.YY3).tag=7;(_tmp851.YY3).f1=Cyc_Absyn_prim2_exp((void*)1,Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp851;});
break;case 445: _LL3CE: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp852;(_tmp852.YY3).tag=7;(_tmp852.YY3).f1=Cyc_Absyn_prim2_exp((void*)3,Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp852;});
break;case 446: _LL3CF: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp853;(_tmp853.YY3).tag=7;(_tmp853.YY3).f1=Cyc_Absyn_prim2_exp((void*)4,Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp853;});
break;case 447: _LL3D0: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 448: _LL3D1: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp854;(_tmp854.YY3).tag=7;(_tmp854.YY3).f1=Cyc_Absyn_cast_exp((*Cyc_yyget_YY37(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])).f3,Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),1,(void*)0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp854;});
break;case 449: _LL3D2: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 450: _LL3D3: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp855;(_tmp855.YY3).tag=7;(_tmp855.YY3).f1=Cyc_Absyn_pre_inc_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp855;});
break;case 451: _LL3D4: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp856;(_tmp856.YY3).tag=7;(_tmp856.YY3).f1=Cyc_Absyn_pre_dec_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp856;});
break;case 452: _LL3D5: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp857;(_tmp857.YY3).tag=7;(_tmp857.YY3).f1=Cyc_Absyn_address_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp857;});
break;case 453: _LL3D6: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp858;(_tmp858.YY3).tag=7;(_tmp858.YY3).f1=Cyc_Absyn_deref_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp858;});
break;case 454: _LL3D7: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 455: _LL3D8: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp859;(_tmp859.YY3).tag=7;(_tmp859.YY3).f1=Cyc_Absyn_prim1_exp(Cyc_yyget_YY7(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp859;});
break;case 456: _LL3D9: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp85A;(_tmp85A.YY3).tag=7;(_tmp85A.YY3).f1=Cyc_Absyn_sizeoftyp_exp((*Cyc_yyget_YY37(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])).f3,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp85A;});
break;case 457: _LL3DA: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp85B;(_tmp85B.YY3).tag=7;(_tmp85B.YY3).f1=Cyc_Absyn_sizeofexp_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp85B;});
break;case 458: _LL3DB: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp85C;(_tmp85C.YY3).tag=7;(_tmp85C.YY3).f1=Cyc_Absyn_offsetof_exp((*Cyc_yyget_YY37(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)])).f3,(void*)({struct
Cyc_Absyn_StructField_struct*_tmp85D=_cycalloc(sizeof(*_tmp85D));_tmp85D[0]=({
struct Cyc_Absyn_StructField_struct _tmp85E;_tmp85E.tag=0;_tmp85E.f1=({struct
_dyneither_ptr*_tmp85F=_cycalloc(sizeof(*_tmp85F));_tmp85F[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp85F;});_tmp85E;});
_tmp85D;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp85C;});break;case 459: _LL3DC: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp860;(_tmp860.YY3).tag=7;(_tmp860.YY3).f1=Cyc_Absyn_offsetof_exp((*
Cyc_yyget_YY37(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)])).f3,(
void*)({struct Cyc_Absyn_TupleIndex_struct*_tmp861=_cycalloc_atomic(sizeof(*
_tmp861));_tmp861[0]=({struct Cyc_Absyn_TupleIndex_struct _tmp862;_tmp862.tag=1;
_tmp862.f1=(unsigned int)(Cyc_yyget_Int_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)])).f2;_tmp862;});_tmp861;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp860;});
break;case 460: _LL3DD: {struct Cyc_Position_Segment*_tmp863=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);struct Cyc_List_List*
_tmp864=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
_tmp863,Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]));
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp865;(_tmp865.YY3).tag=
7;(_tmp865.YY3).f1=Cyc_Absyn_gentyp_exp(_tmp864,(*Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])).f3,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp865;});
break;}case 461: _LL3DE: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp866;(_tmp866.YY3).tag=7;(_tmp866.YY3).f1=Cyc_Absyn_new_exp((void*)({struct
Cyc_Absyn_Malloc_e_struct*_tmp867=_cycalloc(sizeof(*_tmp867));_tmp867[0]=({
struct Cyc_Absyn_Malloc_e_struct _tmp868;_tmp868.tag=35;_tmp868.f1=({struct Cyc_Absyn_MallocInfo
_tmp869;_tmp869.is_calloc=0;_tmp869.rgn=0;_tmp869.elt_type=0;_tmp869.num_elts=
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp869.fat_result=0;_tmp869;});_tmp868;});_tmp867;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp866;});
break;case 462: _LL3DF: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp86A;(_tmp86A.YY3).tag=7;(_tmp86A.YY3).f1=Cyc_Absyn_new_exp((void*)({struct
Cyc_Absyn_Malloc_e_struct*_tmp86B=_cycalloc(sizeof(*_tmp86B));_tmp86B[0]=({
struct Cyc_Absyn_Malloc_e_struct _tmp86C;_tmp86C.tag=35;_tmp86C.f1=({struct Cyc_Absyn_MallocInfo
_tmp86D;_tmp86D.is_calloc=0;_tmp86D.rgn=(struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp86D.elt_type=0;
_tmp86D.num_elts=Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp86D.fat_result=0;_tmp86D;});_tmp86C;});_tmp86B;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp86A;});
break;case 463: _LL3E0: {void*_tmp86F;struct _tuple2 _tmp86E=*Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp86F=_tmp86E.f3;yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp870;(_tmp870.YY3).tag=7;(
_tmp870.YY3).f1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Malloc_e_struct*
_tmp871=_cycalloc(sizeof(*_tmp871));_tmp871[0]=({struct Cyc_Absyn_Malloc_e_struct
_tmp872;_tmp872.tag=35;_tmp872.f1=({struct Cyc_Absyn_MallocInfo _tmp873;_tmp873.is_calloc=
1;_tmp873.rgn=0;_tmp873.elt_type=({void**_tmp874=_cycalloc(sizeof(*_tmp874));
_tmp874[0]=_tmp86F;_tmp874;});_tmp873.num_elts=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 6)]);_tmp873.fat_result=0;
_tmp873;});_tmp872;});_tmp871;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 8)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp870;});
break;}case 464: _LL3E1: {void*_tmp876;struct _tuple2 _tmp875=*Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp876=_tmp875.f3;yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp877;(_tmp877.YY3).tag=7;(
_tmp877.YY3).f1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Malloc_e_struct*
_tmp878=_cycalloc(sizeof(*_tmp878));_tmp878[0]=({struct Cyc_Absyn_Malloc_e_struct
_tmp879;_tmp879.tag=35;_tmp879.f1=({struct Cyc_Absyn_MallocInfo _tmp87A;_tmp87A.is_calloc=
1;_tmp87A.rgn=(struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 8)]);_tmp87A.elt_type=({void**
_tmp87B=_cycalloc(sizeof(*_tmp87B));_tmp87B[0]=_tmp876;_tmp87B;});_tmp87A.num_elts=
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]);
_tmp87A.fat_result=0;_tmp87A;});_tmp879;});_tmp878;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 10)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp877;});
break;}case 465: _LL3E2: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp87C;(_tmp87C.YY3).tag=7;(_tmp87C.YY3).f1=Cyc_Absyn_primop_exp((void*)19,({
struct Cyc_Absyn_Exp*_tmp87D[1];_tmp87D[0]=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp87D,sizeof(struct Cyc_Absyn_Exp*),
1));}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp87C;});break;case 466: _LL3E3: {void*_tmp87F;
struct _tuple2 _tmp87E=*Cyc_yyget_YY37(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp87F=_tmp87E.f3;yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp880;(_tmp880.YY3).tag=7;(_tmp880.YY3).f1=Cyc_Absyn_valueof_exp(_tmp87F,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp880;});
break;}case 467: _LL3E4: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp881;(_tmp881.YY7).tag=11;(_tmp881.YY7).f1=(void*)((void*)12);_tmp881;});
break;case 468: _LL3E5: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp882;(_tmp882.YY7).tag=11;(_tmp882.YY7).f1=(void*)((void*)11);_tmp882;});
break;case 469: _LL3E6: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp883;(_tmp883.YY7).tag=11;(_tmp883.YY7).f1=(void*)((void*)2);_tmp883;});
break;case 470: _LL3E7: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 471: _LL3E8: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp884;(_tmp884.YY3).tag=7;(_tmp884.YY3).f1=Cyc_Absyn_subscript_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp884;});
break;case 472: _LL3E9: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp885;(_tmp885.YY3).tag=7;(_tmp885.YY3).f1=Cyc_Absyn_unknowncall_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp885;});
break;case 473: _LL3EA: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp886;(_tmp886.YY3).tag=7;(_tmp886.YY3).f1=Cyc_Absyn_unknowncall_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_yyget_YY5(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp886;});
break;case 474: _LL3EB: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp887;(_tmp887.YY3).tag=7;(_tmp887.YY3).f1=Cyc_Absyn_aggrmember_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),({struct
_dyneither_ptr*_tmp888=_cycalloc(sizeof(*_tmp888));_tmp888[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp888;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp887;});
break;case 475: _LL3EC: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp889;(_tmp889.YY3).tag=7;(_tmp889.YY3).f1=Cyc_Absyn_aggrarrow_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),({struct
_dyneither_ptr*_tmp88A=_cycalloc(sizeof(*_tmp88A));_tmp88A[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp88A;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp889;});
break;case 476: _LL3ED: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp88B;(_tmp88B.YY3).tag=7;(_tmp88B.YY3).f1=Cyc_Absyn_post_inc_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp88B;});
break;case 477: _LL3EE: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp88C;(_tmp88C.YY3).tag=7;(_tmp88C.YY3).f1=Cyc_Absyn_post_dec_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp88C;});
break;case 478: _LL3EF: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp88D;(_tmp88D.YY3).tag=7;(_tmp88D.YY3).f1=Cyc_Absyn_new_exp((void*)({struct
Cyc_Absyn_CompoundLit_e_struct*_tmp88E=_cycalloc(sizeof(*_tmp88E));_tmp88E[0]=({
struct Cyc_Absyn_CompoundLit_e_struct _tmp88F;_tmp88F.tag=27;_tmp88F.f1=Cyc_yyget_YY37(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp88F.f2=((struct
Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY6(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp88F;});_tmp88E;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp88D;});break;case 479: _LL3F0: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp890;(_tmp890.YY3).tag=7;(_tmp890.YY3).f1=Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_CompoundLit_e_struct*_tmp891=_cycalloc(sizeof(*_tmp891));
_tmp891[0]=({struct Cyc_Absyn_CompoundLit_e_struct _tmp892;_tmp892.tag=27;_tmp892.f1=
Cyc_yyget_YY37(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]);
_tmp892.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_YY6(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]));
_tmp892;});_tmp891;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp890;});
break;case 480: _LL3F1: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp893;(_tmp893.YY3).tag=7;(_tmp893.YY3).f1=Cyc_Absyn_new_exp((void*)({struct
Cyc_Absyn_UnknownId_e_struct*_tmp894=_cycalloc(sizeof(*_tmp894));_tmp894[0]=({
struct Cyc_Absyn_UnknownId_e_struct _tmp895;_tmp895.tag=2;_tmp895.f1=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp895;});_tmp894;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp893;});break;case 481: _LL3F2: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 482: _LL3F3: yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp896;(_tmp896.YY3).tag=7;(
_tmp896.YY3).f1=Cyc_Absyn_string_exp(Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp896;});
break;case 483: _LL3F4: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)];break;case 484: _LL3F5: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp897;(_tmp897.YY3).tag=7;(_tmp897.YY3).f1=Cyc_Absyn_noinstantiate_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp897;});
break;case 485: _LL3F6: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp898;(_tmp898.YY3).tag=7;(_tmp898.YY3).f1=Cyc_Absyn_instantiate_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_List_imp_rev(
Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp898;});
break;case 486: _LL3F7: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp899;(_tmp899.YY3).tag=7;(_tmp899.YY3).f1=Cyc_Absyn_tuple_exp(Cyc_yyget_YY5(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp899;});
break;case 487: _LL3F8: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp89A;(_tmp89A.YY3).tag=7;(_tmp89A.YY3).f1=Cyc_Absyn_new_exp((void*)({struct
Cyc_Absyn_Struct_e_struct*_tmp89B=_cycalloc(sizeof(*_tmp89B));_tmp89B[0]=({
struct Cyc_Absyn_Struct_e_struct _tmp89C;_tmp89C.tag=30;_tmp89C.f1=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp89C.f2=Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp89C.f3=((struct
Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY6(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp89C.f4=0;_tmp89C;});
_tmp89B;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp89A;});break;case 488: _LL3F9: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp89D;(_tmp89D.YY3).tag=7;(_tmp89D.YY3).f1=Cyc_Absyn_stmt_exp(
Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp89D;});
break;case 489: _LL3FA: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp89E;(_tmp89E.YY5).tag=9;(_tmp89E.YY5).f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(Cyc_yyget_YY5(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]));_tmp89E;});break;case 490: _LL3FB: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp89F;(_tmp89F.YY5).tag=9;(_tmp89F.YY5).f1=({struct Cyc_List_List*
_tmp8A0=_cycalloc(sizeof(*_tmp8A0));_tmp8A0->hd=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp8A0->tl=0;_tmp8A0;});
_tmp89F;});break;case 491: _LL3FC: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp8A1;(_tmp8A1.YY5).tag=9;(_tmp8A1.YY5).f1=({struct Cyc_List_List*_tmp8A2=
_cycalloc(sizeof(*_tmp8A2));_tmp8A2->hd=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp8A2->tl=Cyc_yyget_YY5(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp8A2;});_tmp8A1;});
break;case 492: _LL3FD: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp8A3;(_tmp8A3.YY3).tag=7;(_tmp8A3.YY3).f1=Cyc_Absyn_int_exp((Cyc_yyget_Int_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f1,(Cyc_yyget_Int_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f2,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8A3;});
break;case 493: _LL3FE: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp8A4;(_tmp8A4.YY3).tag=7;(_tmp8A4.YY3).f1=Cyc_Absyn_char_exp(Cyc_yyget_Char_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8A4;});
break;case 494: _LL3FF: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp8A5;(_tmp8A5.YY3).tag=7;(_tmp8A5.YY3).f1=Cyc_Absyn_float_exp(Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8A5;});
break;case 495: _LL400: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp8A6;(_tmp8A6.YY3).tag=7;(_tmp8A6.YY3).f1=Cyc_Absyn_null_exp(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8A6;});
break;case 496: _LL401: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp8A7;(_tmp8A7.QualId_tok).tag=4;(_tmp8A7.QualId_tok).f1=({struct _tuple1*
_tmp8A8=_cycalloc(sizeof(*_tmp8A8));_tmp8A8->f1=(union Cyc_Absyn_Nmspace_union)({
union Cyc_Absyn_Nmspace_union _tmp8AA;(_tmp8AA.Rel_n).tag=1;(_tmp8AA.Rel_n).f1=0;
_tmp8AA;});_tmp8A8->f2=({struct _dyneither_ptr*_tmp8A9=_cycalloc(sizeof(*_tmp8A9));
_tmp8A9[0]=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp8A9;});_tmp8A8;});_tmp8A7;});break;case 497: _LL402: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 498: _LL403: yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp8AB;(_tmp8AB.QualId_tok).tag=
4;(_tmp8AB.QualId_tok).f1=({struct _tuple1*_tmp8AC=_cycalloc(sizeof(*_tmp8AC));
_tmp8AC->f1=(union Cyc_Absyn_Nmspace_union)({union Cyc_Absyn_Nmspace_union _tmp8AE;(
_tmp8AE.Rel_n).tag=1;(_tmp8AE.Rel_n).f1=0;_tmp8AE;});_tmp8AC->f2=({struct
_dyneither_ptr*_tmp8AD=_cycalloc(sizeof(*_tmp8AD));_tmp8AD[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp8AD;});_tmp8AC;});
_tmp8AB;});break;case 499: _LL404: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 500: _LL405: yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)];break;case 501: _LL406: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 502: _LL407: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 503: _LL408:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 504:
_LL409: break;case 505: _LL40A: yylex_buf->lex_curr_pos -=1;break;default: _LL40B:
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
yyn=(int)Cyc_yyr1[_check_known_subscript_notnull(506,yyn)];yystate=Cyc_yypgoto[
_check_known_subscript_notnull(145,yyn - 138)]+ yyss[
_check_known_subscript_notnull(10000,yyssp_offset)];if((yystate >= 0  && yystate <= 
6476) && Cyc_yycheck[_check_known_subscript_notnull(6477,yystate)]== yyss[
_check_known_subscript_notnull(10000,yyssp_offset)])yystate=(int)Cyc_yytable[
_check_known_subscript_notnull(6477,yystate)];else{yystate=(int)Cyc_yydefgoto[
_check_known_subscript_notnull(145,yyn - 138)];}goto yynewstate;yyerrlab: if(
yyerrstatus == 0){++ Cyc_yynerrs;yyn=(int)Cyc_yypact[
_check_known_subscript_notnull(1007,yystate)];if(yyn > - 32768  && yyn < 6476){int
sze=0;struct _dyneither_ptr msg;int x;int count;count=0;for(x=yyn < 0?- yyn: 0;x < 283 / 
sizeof(char*);++ x){if(Cyc_yycheck[_check_known_subscript_notnull(6477,x + yyn)]== 
x)(sze +=Cyc_strlen((struct _dyneither_ptr)Cyc_yytname[
_check_known_subscript_notnull(283,x)])+ 15,count ++);}msg=({unsigned int _tmp8AF=(
unsigned int)(sze + 15);char*_tmp8B0=(char*)_region_malloc(yyregion,_check_times(
sizeof(char),_tmp8AF + 1));struct _dyneither_ptr _tmp8B2=_tag_dyneither(_tmp8B0,
sizeof(char),_tmp8AF + 1);{unsigned int _tmp8B1=_tmp8AF;unsigned int i;for(i=0;i < 
_tmp8B1;i ++){_tmp8B0[i]='\000';}_tmp8B0[_tmp8B1]=(char)0;}_tmp8B2;});Cyc_strcpy(
msg,({const char*_tmp8B3="parse error";_tag_dyneither(_tmp8B3,sizeof(char),12);}));
if(count < 5){count=0;for(x=yyn < 0?- yyn: 0;x < 283 / sizeof(char*);++ x){if(Cyc_yycheck[
_check_known_subscript_notnull(6477,x + yyn)]== x){Cyc_strcat(msg,(struct
_dyneither_ptr)(count == 0?(struct _dyneither_ptr)({const char*_tmp8B4=", expecting `";
_tag_dyneither(_tmp8B4,sizeof(char),14);}):(struct _dyneither_ptr)({const char*
_tmp8B5=" or `";_tag_dyneither(_tmp8B5,sizeof(char),6);})));Cyc_strcat(msg,(
struct _dyneither_ptr)Cyc_yytname[_check_known_subscript_notnull(283,x)]);Cyc_strcat(
msg,({const char*_tmp8B6="'";_tag_dyneither(_tmp8B6,sizeof(char),2);}));++ count;}}}
Cyc_yyerror((struct _dyneither_ptr)msg);}else{Cyc_yyerror(({const char*_tmp8B7="parse error";
_tag_dyneither(_tmp8B7,sizeof(char),12);}));}}goto yyerrlab1;yyerrlab1: if(
yyerrstatus == 3){if(Cyc_yychar == 0){int _tmp8B8=1;_npop_handler(0);return _tmp8B8;}
Cyc_yychar=- 2;}yyerrstatus=3;goto yyerrhandle;yyerrdefault: yyerrpop: if(
yyssp_offset == 0){int _tmp8B9=1;_npop_handler(0);return _tmp8B9;}-- yyvsp_offset;
yystate=(int)yyss[_check_known_subscript_notnull(10000,-- yyssp_offset)];--
yylsp_offset;yyerrhandle: yyn=(int)Cyc_yypact[_check_known_subscript_notnull(1007,
yystate)];if(yyn == - 32768)goto yyerrdefault;yyn +=1;if((yyn < 0  || yyn > 6476) || 
Cyc_yycheck[_check_known_subscript_notnull(6477,yyn)]!= 1)goto yyerrdefault;yyn=(
int)Cyc_yytable[_check_known_subscript_notnull(6477,yyn)];if(yyn < 0){if(yyn == -
32768)goto yyerrpop;yyn=- yyn;goto yyreduce;}else{if(yyn == 0)goto yyerrpop;}if(yyn == 
1006){int _tmp8BA=0;_npop_handler(0);return _tmp8BA;}yyvs[
_check_known_subscript_notnull(10000,++ yyvsp_offset)]=Cyc_yylval;yyls[
_check_known_subscript_notnull(10000,++ yylsp_offset)]=Cyc_yylloc;goto yynewstate;};
_pop_region(yyregion);}void Cyc_yyprint(int i,union Cyc_YYSTYPE_union v){union Cyc_YYSTYPE_union
_tmp8C4=v;struct _tuple11 _tmp8C5;int _tmp8C6;char _tmp8C7;struct _dyneither_ptr
_tmp8C8;struct _tuple1*_tmp8C9;struct _tuple1 _tmp8CA;union Cyc_Absyn_Nmspace_union
_tmp8CB;struct _dyneither_ptr*_tmp8CC;_LL40E: if((_tmp8C4.Int_tok).tag != 0)goto
_LL410;_tmp8C5=(_tmp8C4.Int_tok).f1;_tmp8C6=_tmp8C5.f2;_LL40F:({struct Cyc_Int_pa_struct
_tmp8CF;_tmp8CF.tag=1;_tmp8CF.f1=(unsigned long)_tmp8C6;{void*_tmp8CD[1]={&
_tmp8CF};Cyc_fprintf(Cyc_stderr,({const char*_tmp8CE="%d";_tag_dyneither(_tmp8CE,
sizeof(char),3);}),_tag_dyneither(_tmp8CD,sizeof(void*),1));}});goto _LL40D;
_LL410: if((_tmp8C4.Char_tok).tag != 1)goto _LL412;_tmp8C7=(_tmp8C4.Char_tok).f1;
_LL411:({struct Cyc_Int_pa_struct _tmp8D2;_tmp8D2.tag=1;_tmp8D2.f1=(unsigned long)((
int)_tmp8C7);{void*_tmp8D0[1]={& _tmp8D2};Cyc_fprintf(Cyc_stderr,({const char*
_tmp8D1="%c";_tag_dyneither(_tmp8D1,sizeof(char),3);}),_tag_dyneither(_tmp8D0,
sizeof(void*),1));}});goto _LL40D;_LL412: if((_tmp8C4.String_tok).tag != 2)goto
_LL414;_tmp8C8=(_tmp8C4.String_tok).f1;_LL413:({struct Cyc_String_pa_struct
_tmp8D5;_tmp8D5.tag=0;_tmp8D5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
_tmp8C8);{void*_tmp8D3[1]={& _tmp8D5};Cyc_fprintf(Cyc_stderr,({const char*_tmp8D4="\"%s\"";
_tag_dyneither(_tmp8D4,sizeof(char),5);}),_tag_dyneither(_tmp8D3,sizeof(void*),1));}});
goto _LL40D;_LL414: if((_tmp8C4.QualId_tok).tag != 4)goto _LL416;_tmp8C9=(_tmp8C4.QualId_tok).f1;
_tmp8CA=*_tmp8C9;_tmp8CB=_tmp8CA.f1;_tmp8CC=_tmp8CA.f2;_LL415: {struct Cyc_List_List*
_tmp8D6=0;{union Cyc_Absyn_Nmspace_union _tmp8D7=_tmp8CB;struct Cyc_List_List*
_tmp8D8;struct Cyc_List_List*_tmp8D9;_LL419: if((_tmp8D7.Rel_n).tag != 1)goto _LL41B;
_tmp8D8=(_tmp8D7.Rel_n).f1;_LL41A: _tmp8D6=_tmp8D8;goto _LL418;_LL41B: if((_tmp8D7.Abs_n).tag
!= 2)goto _LL41D;_tmp8D9=(_tmp8D7.Abs_n).f1;_LL41C: _tmp8D6=_tmp8D9;goto _LL418;
_LL41D: if((_tmp8D7.Loc_n).tag != 0)goto _LL418;_LL41E: goto _LL418;_LL418:;}for(0;
_tmp8D6 != 0;_tmp8D6=_tmp8D6->tl){({struct Cyc_String_pa_struct _tmp8DC;_tmp8DC.tag=
0;_tmp8DC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct
_dyneither_ptr*)_tmp8D6->hd));{void*_tmp8DA[1]={& _tmp8DC};Cyc_fprintf(Cyc_stderr,({
const char*_tmp8DB="%s::";_tag_dyneither(_tmp8DB,sizeof(char),5);}),
_tag_dyneither(_tmp8DA,sizeof(void*),1));}});}({struct Cyc_String_pa_struct
_tmp8DF;_tmp8DF.tag=0;_tmp8DF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*
_tmp8CC);{void*_tmp8DD[1]={& _tmp8DF};Cyc_fprintf(Cyc_stderr,({const char*_tmp8DE="%s::";
_tag_dyneither(_tmp8DE,sizeof(char),5);}),_tag_dyneither(_tmp8DD,sizeof(void*),1));}});
goto _LL40D;}_LL416:;_LL417:({void*_tmp8E0=0;Cyc_fprintf(Cyc_stderr,({const char*
_tmp8E1="?";_tag_dyneither(_tmp8E1,sizeof(char),2);}),_tag_dyneither(_tmp8E0,
sizeof(void*),0));});goto _LL40D;_LL40D:;}struct Cyc_List_List*Cyc_Parse_parse_file(
struct Cyc___cycFILE*f){Cyc_Parse_parse_result=0;Cyc_yyparse(Cyc_Lexing_from_file(
f));return Cyc_Parse_parse_result;}
