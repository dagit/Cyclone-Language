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
 struct Cyc___cycFILE;extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_Cstdio___abstractFILE;
struct Cyc_String_pa_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_struct{
int tag;unsigned long f1;};struct Cyc_Double_pa_struct{int tag;double f1;};struct Cyc_LongDouble_pa_struct{
int tag;long double f1;};struct Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{
int tag;unsigned long*f1;};int Cyc_fprintf(struct Cyc___cycFILE*,struct
_dyneither_ptr,struct _dyneither_ptr);struct Cyc_ShortPtr_sa_struct{int tag;short*
f1;};struct Cyc_UShortPtr_sa_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_struct{
int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_struct{
int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_struct{int tag;double*f1;}
;struct Cyc_FloatPtr_sa_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int
tag;struct _dyneither_ptr f1;};extern char Cyc_FileCloseError[19];extern char Cyc_FileOpenError[
18];struct Cyc_FileOpenError_struct{char*tag;struct _dyneither_ptr f1;};struct Cyc_Core_NewRegion{
struct _DynRegionHandle*dynregion;};struct Cyc_Core_Opt{void*v;};extern char Cyc_Core_Invalid_argument[
21];struct Cyc_Core_Invalid_argument_struct{char*tag;struct _dyneither_ptr f1;};
extern char Cyc_Core_Failure[12];struct Cyc_Core_Failure_struct{char*tag;struct
_dyneither_ptr f1;};extern char Cyc_Core_Impossible[15];struct Cyc_Core_Impossible_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[14];extern char Cyc_Core_Unreachable[
16];struct Cyc_Core_Unreachable_struct{char*tag;struct _dyneither_ptr f1;};extern
char Cyc_Core_Open_Region[16];extern char Cyc_Core_Free_Region[16];struct Cyc_List_List{
void*hd;struct Cyc_List_List*tl;};struct Cyc_List_List*Cyc_List_list(struct
_dyneither_ptr);extern char Cyc_List_List_mismatch[18];void Cyc_List_iter(void(*f)(
void*),struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*
x);extern char Cyc_List_Nth[8];struct Cyc_Iter_Iter{void*env;int(*next)(void*env,
void*dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Dict_T;struct
Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;struct Cyc_Dict_T*t;};
extern char Cyc_Dict_Present[12];extern char Cyc_Dict_Absent[11];struct Cyc_Dict_Dict
Cyc_Dict_rempty(struct _RegionHandle*,int(*cmp)(void*,void*));int Cyc_Dict_member(
struct Cyc_Dict_Dict d,void*k);struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict
d,void*k,void*v);void*Cyc_Dict_lookup(struct Cyc_Dict_Dict d,void*k);struct _tuple0{
void*f1;void*f2;};struct _tuple0*Cyc_Dict_rchoose(struct _RegionHandle*r,struct Cyc_Dict_Dict
d);struct _tuple0*Cyc_Dict_rchoose(struct _RegionHandle*,struct Cyc_Dict_Dict d);
struct Cyc_Iter_Iter Cyc_Dict_make_iter(struct _RegionHandle*rgn,struct Cyc_Dict_Dict
d);struct Cyc_Lineno_Pos{struct _dyneither_ptr logical_file;struct _dyneither_ptr
line;int line_no;int col;};extern char Cyc_Position_Exit[9];struct Cyc_Position_Segment;
struct Cyc_Position_Error{struct _dyneither_ptr source;struct Cyc_Position_Segment*
seg;void*kind;struct _dyneither_ptr desc;};extern char Cyc_Position_Nocontext[14];
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
_dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];int Cyc_Absyn_tvar_cmp(
struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*);extern void*Cyc_Absyn_uint_typ;
extern void*Cyc_Absyn_empty_effect;struct Cyc_RgnOrder_RgnPO;struct Cyc_RgnOrder_RgnPO*
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
struct Cyc_RgnOrder_RgnPO*po);struct Cyc_Set_Set;extern char Cyc_Set_Absent[11];
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
_dyneither_ptr fmt,struct _dyneither_ptr ap);void*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*
t);void*Cyc_Tcutil_typ_kind(void*t);void*Cyc_Tcutil_compress(void*t);int Cyc_Tcutil_unify(
void*,void*);void*Cyc_Tcutil_normalize_effect(void*e);struct Cyc_PP_Ppstate;
struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs: 1;
int qvar_to_Cids: 1;int add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int
rewrite_temp_tvars: 1;int print_all_tvars: 1;int print_all_kinds: 1;int
print_all_effects: 1;int print_using_stmts: 1;int print_externC_stmts: 1;int
print_full_evars: 1;int print_zeroterm: 1;int generate_line_directives: 1;int
use_curr_namespace: 1;struct Cyc_List_List*curr_namespace;};struct _dyneither_ptr
Cyc_Absynpp_typ2string(void*);static struct _dyneither_ptr Cyc_RgnOrder_tvar2string(
struct Cyc_Absyn_Tvar*t){return*t->name;}static int Cyc_RgnOrder_valid_constraint(
void*eff,void*rgn);static void*Cyc_RgnOrder_filter_effect(void*eff,void*k);struct
Cyc_RgnOrder_RgnInfo{void*these_outlive;int resetable;};struct Cyc_RgnOrder_RgnPO{
struct Cyc_Dict_Dict d;void*these_outlive_heap;void*these_outlive_unique;struct Cyc_Absyn_Tvar*
youngest_unresettable;};static int Cyc_RgnOrder_valid_constraint(void*eff,void*rgn){
void*_tmp0=Cyc_Tcutil_typ_kind(rgn);void*_tmp1=Cyc_Tcutil_compress(eff);struct
Cyc_List_List*_tmp2;void*_tmp3;void*_tmp4;_LL1: if(_tmp1 <= (void*)4)goto _LLB;if(*((
int*)_tmp1)!= 20)goto _LL3;_tmp2=((struct Cyc_Absyn_JoinEff_struct*)_tmp1)->f1;
_LL2: for(0;_tmp2 != 0;_tmp2=_tmp2->tl){if(!Cyc_RgnOrder_valid_constraint((void*)
_tmp2->hd,rgn))return 0;}return 1;_LL3: if(*((int*)_tmp1)!= 0)goto _LL5;_LL4: goto
_LL6;_LL5: if(*((int*)_tmp1)!= 1)goto _LL7;_LL6: return 1;_LL7: if(*((int*)_tmp1)!= 
21)goto _LL9;_tmp3=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp1)->f1;_LL8: if(
_tmp0 == (void*)3)return 1;else{return 0;}_LL9: if(*((int*)_tmp1)!= 19)goto _LLB;
_tmp4=(void*)((struct Cyc_Absyn_AccessEff_struct*)_tmp1)->f1;_LLA: {void*_tmp5=
Cyc_Tcutil_typ_kind(_tmp4);return _tmp0 == (void*)3  || _tmp5 == _tmp0;}_LLB:;_LLC:({
struct Cyc_String_pa_struct _tmp8;_tmp8.tag=0;_tmp8.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string(eff));{void*_tmp6[1]={& _tmp8};((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmp7="bad effect |%s| passed to add_outlives_constraint";_tag_dyneither(_tmp7,
sizeof(char),50);}),_tag_dyneither(_tmp6,sizeof(void*),1));}});_LL0:;}static void*
Cyc_RgnOrder_filter_effect(void*eff,void*k){void*_tmp9=Cyc_Tcutil_compress(eff);
void*_tmpA=_tmp9;struct Cyc_List_List*_tmpB;void*_tmpC;_LLE: if(_tmpA <= (void*)4)
goto _LL16;if(*((int*)_tmpA)!= 20)goto _LL10;_tmpB=((struct Cyc_Absyn_JoinEff_struct*)
_tmpA)->f1;_LLF: {struct Cyc_List_List*effs=0;for(0;_tmpB != 0;_tmpB=_tmpB->tl){
void*_tmpD=Cyc_RgnOrder_filter_effect((void*)_tmpB->hd,k);if(_tmpD != (void*)0)
effs=({struct Cyc_List_List*_tmpE=_cycalloc(sizeof(*_tmpE));_tmpE->hd=(void*)
_tmpD;_tmpE->tl=effs;_tmpE;});}return(void*)({struct Cyc_Absyn_JoinEff_struct*
_tmpF=_cycalloc(sizeof(*_tmpF));_tmpF[0]=({struct Cyc_Absyn_JoinEff_struct _tmp10;
_tmp10.tag=20;_tmp10.f1=Cyc_List_imp_rev(effs);_tmp10;});_tmpF;});}_LL10: if(*((
int*)_tmpA)!= 0)goto _LL12;_LL11: goto _LL13;_LL12: if(*((int*)_tmpA)!= 21)goto _LL14;
_LL13: return _tmp9;_LL14: if(*((int*)_tmpA)!= 19)goto _LL16;_tmpC=(void*)((struct
Cyc_Absyn_AccessEff_struct*)_tmpA)->f1;_LL15: if(Cyc_Tcutil_typ_kind(_tmpC)!= k)
return _tmp9;else{return(void*)0;}_LL16:;_LL17:({void*_tmp11=0;((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp12="bad effect passed to filter_effect";
_tag_dyneither(_tmp12,sizeof(char),35);}),_tag_dyneither(_tmp11,sizeof(void*),0));});
_LLD:;}struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_outlives_constraint(struct
_RegionHandle*r,struct Cyc_RgnOrder_RgnPO*po,void*eff,void*rgn,struct Cyc_Position_Segment*
loc){eff=Cyc_Tcutil_normalize_effect(eff);{struct Cyc_RgnOrder_RgnPO*_tmp13=({
struct Cyc_RgnOrder_RgnPO*_tmp27=_region_malloc(r,sizeof(*_tmp27));_tmp27[0]=*po;
_tmp27;});if(!Cyc_RgnOrder_valid_constraint(eff,rgn)){({void*_tmp14=0;Cyc_Tcutil_terr(
loc,({const char*_tmp15="Invalid region ordering constraint; kind mismatch";
_tag_dyneither(_tmp15,sizeof(char),50);}),_tag_dyneither(_tmp14,sizeof(void*),0));});
return _tmp13;}{void*_tmp16=Cyc_Tcutil_compress(rgn);struct Cyc_Absyn_Tvar*_tmp17;
_LL19: if(_tmp16 <= (void*)4)goto _LL1D;if(*((int*)_tmp16)!= 1)goto _LL1B;_tmp17=((
struct Cyc_Absyn_VarType_struct*)_tmp16)->f1;_LL1A: {struct Cyc_Dict_Dict _tmp18=po->d;
if(((int(*)(struct Cyc_Dict_Dict d,struct Cyc_Absyn_Tvar*k))Cyc_Dict_member)(_tmp18,
_tmp17)){void*_tmp19=(((struct Cyc_RgnOrder_RgnInfo*(*)(struct Cyc_Dict_Dict d,
struct Cyc_Absyn_Tvar*k))Cyc_Dict_lookup)(_tmp18,_tmp17))->these_outlive;_tmp18=((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct Cyc_Absyn_Tvar*k,struct Cyc_RgnOrder_RgnInfo*
v))Cyc_Dict_insert)(_tmp18,_tmp17,({struct Cyc_RgnOrder_RgnInfo*_tmp1A=
_region_malloc(r,sizeof(*_tmp1A));_tmp1A->these_outlive=(void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp1B=_cycalloc(sizeof(*_tmp1B));_tmp1B[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp1C;_tmp1C.tag=20;_tmp1C.f1=({void*_tmp1D[2];_tmp1D[1]=_tmp19;_tmp1D[0]=eff;
Cyc_List_list(_tag_dyneither(_tmp1D,sizeof(void*),2));});_tmp1C;});_tmp1B;});
_tmp1A->resetable=0;_tmp1A;}));}else{_tmp18=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict
d,struct Cyc_Absyn_Tvar*k,struct Cyc_RgnOrder_RgnInfo*v))Cyc_Dict_insert)(_tmp18,
_tmp17,({struct Cyc_RgnOrder_RgnInfo*_tmp1E=_region_malloc(r,sizeof(*_tmp1E));
_tmp1E->these_outlive=eff;_tmp1E->resetable=0;_tmp1E;}));}_tmp13->d=_tmp18;
return _tmp13;}_LL1B: if(*((int*)_tmp16)!= 0)goto _LL1D;_LL1C: Cyc_Tcutil_unify(rgn,(
void*)2);goto _LL1E;_LL1D: if((int)_tmp16 != 2)goto _LL1F;_LL1E: _tmp13->these_outlive_heap=(
void*)({struct Cyc_Absyn_JoinEff_struct*_tmp1F=_cycalloc(sizeof(*_tmp1F));_tmp1F[
0]=({struct Cyc_Absyn_JoinEff_struct _tmp20;_tmp20.tag=20;_tmp20.f1=({void*_tmp21[
2];_tmp21[1]=po->these_outlive_heap;_tmp21[0]=eff;Cyc_List_list(_tag_dyneither(
_tmp21,sizeof(void*),2));});_tmp20;});_tmp1F;});return _tmp13;_LL1F: if((int)
_tmp16 != 3)goto _LL21;_LL20: _tmp13->these_outlive_unique=(void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp22=_cycalloc(sizeof(*_tmp22));_tmp22[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp23;_tmp23.tag=20;_tmp23.f1=({void*_tmp24[2];_tmp24[1]=po->these_outlive_unique;
_tmp24[0]=eff;Cyc_List_list(_tag_dyneither(_tmp24,sizeof(void*),2));});_tmp23;});
_tmp22;});return _tmp13;_LL21:;_LL22:({void*_tmp25=0;((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp26="RgnOrder::add_outlives_constraint passed a bad region";
_tag_dyneither(_tmp26,sizeof(char),54);}),_tag_dyneither(_tmp25,sizeof(void*),0));});
_LL18:;}}}struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_youngest(struct _RegionHandle*
r,struct Cyc_RgnOrder_RgnPO*po,struct Cyc_Absyn_Tvar*rgn,int resettable,int opened){
if(!opened  && ((int(*)(struct Cyc_Dict_Dict d,struct Cyc_Absyn_Tvar*k))Cyc_Dict_member)(
po->d,rgn))({void*_tmp28=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap))Cyc_Tcutil_impos)(({const char*_tmp29="RgnOrder::add_youngest: repeated region";
_tag_dyneither(_tmp29,sizeof(char),40);}),_tag_dyneither(_tmp28,sizeof(void*),0));});{
struct Cyc_RgnOrder_RgnPO*_tmp2A=({struct Cyc_RgnOrder_RgnPO*_tmp31=_region_malloc(
r,sizeof(*_tmp31));_tmp31[0]=*po;_tmp31;});struct Cyc_Absyn_AccessEff_struct*
_tmp2B=({struct Cyc_Absyn_AccessEff_struct*_tmp2D=_cycalloc(sizeof(*_tmp2D));
_tmp2D[0]=({struct Cyc_Absyn_AccessEff_struct _tmp2E;_tmp2E.tag=19;_tmp2E.f1=(void*)((
void*)({struct Cyc_Absyn_VarType_struct*_tmp2F=_cycalloc(sizeof(*_tmp2F));_tmp2F[
0]=({struct Cyc_Absyn_VarType_struct _tmp30;_tmp30.tag=1;_tmp30.f1=po->youngest_unresettable;
_tmp30;});_tmp2F;}));_tmp2E;});_tmp2D;});_tmp2A->d=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,struct Cyc_Absyn_Tvar*k,struct Cyc_RgnOrder_RgnInfo*v))Cyc_Dict_insert)(
po->d,rgn,({struct Cyc_RgnOrder_RgnInfo*_tmp2C=_region_malloc(r,sizeof(*_tmp2C));
_tmp2C->these_outlive=(void*)_tmp2B;_tmp2C->resetable=resettable;_tmp2C;}));if(!
resettable);_tmp2A->youngest_unresettable=rgn;return _tmp2A;}}struct Cyc_RgnOrder_RgnPO*
Cyc_RgnOrder_initial_fn_po(struct _RegionHandle*r,struct Cyc_List_List*tvs,struct
Cyc_List_List*po,void*effect,struct Cyc_Absyn_Tvar*fst_rgn,struct Cyc_Position_Segment*
loc){struct Cyc_Dict_Dict d=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(
struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*)))Cyc_Dict_rempty)(r,Cyc_Absyn_tvar_cmp);{
struct Cyc_List_List*_tmp32=tvs;for(0;_tmp32 != 0;_tmp32=_tmp32->tl){void*_tmp33=
Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp32->hd);_LL24: if((int)_tmp33 != 3)
goto _LL26;_LL25: goto _LL27;_LL26: if((int)_tmp33 != 4)goto _LL28;_LL27: goto _LL29;
_LL28: if((int)_tmp33 != 5)goto _LL2A;_LL29: d=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict
d,struct Cyc_Absyn_Tvar*k,struct Cyc_RgnOrder_RgnInfo*v))Cyc_Dict_insert)(d,(
struct Cyc_Absyn_Tvar*)_tmp32->hd,({struct Cyc_RgnOrder_RgnInfo*_tmp34=
_region_malloc(r,sizeof(*_tmp34));_tmp34->these_outlive=Cyc_Absyn_empty_effect;
_tmp34->resetable=0;_tmp34;}));goto _LL23;goto _LL23;_LL2A:;_LL2B: goto _LL23;_LL23:;}}
if(!Cyc_RgnOrder_valid_constraint(effect,(void*)({struct Cyc_Absyn_VarType_struct*
_tmp35=_cycalloc(sizeof(*_tmp35));_tmp35[0]=({struct Cyc_Absyn_VarType_struct
_tmp36;_tmp36.tag=1;_tmp36.f1=fst_rgn;_tmp36;});_tmp35;}))){({void*_tmp37=0;Cyc_Tcutil_terr(
loc,({const char*_tmp38="Invalid region ordering constraint; kind mismatch";
_tag_dyneither(_tmp38,sizeof(char),50);}),_tag_dyneither(_tmp37,sizeof(void*),0));});
return({struct Cyc_RgnOrder_RgnPO*_tmp39=_region_malloc(r,sizeof(*_tmp39));_tmp39->d=
d;_tmp39->these_outlive_heap=Cyc_Absyn_empty_effect;_tmp39->these_outlive_unique=
Cyc_Absyn_empty_effect;_tmp39->youngest_unresettable=fst_rgn;_tmp39;});}d=((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct Cyc_Absyn_Tvar*k,struct Cyc_RgnOrder_RgnInfo*
v))Cyc_Dict_insert)(d,fst_rgn,({struct Cyc_RgnOrder_RgnInfo*_tmp3A=_region_malloc(
r,sizeof(*_tmp3A));_tmp3A->these_outlive=effect;_tmp3A->resetable=0;_tmp3A;}));{
struct Cyc_RgnOrder_RgnPO*_tmp3B=({struct Cyc_RgnOrder_RgnPO*_tmp3C=_region_malloc(
r,sizeof(*_tmp3C));_tmp3C->d=d;_tmp3C->these_outlive_heap=Cyc_Absyn_empty_effect;
_tmp3C->these_outlive_unique=Cyc_Absyn_empty_effect;_tmp3C->youngest_unresettable=
fst_rgn;_tmp3C;});for(0;po != 0;po=po->tl){_tmp3B=Cyc_RgnOrder_add_outlives_constraint(
r,_tmp3B,(*((struct _tuple0*)po->hd)).f1,(*((struct _tuple0*)po->hd)).f2,loc);}
return _tmp3B;}}int Cyc_RgnOrder_is_region_resetable(struct Cyc_RgnOrder_RgnPO*po,
struct Cyc_Absyn_Tvar*r){return(((struct Cyc_RgnOrder_RgnInfo*(*)(struct Cyc_Dict_Dict
d,struct Cyc_Absyn_Tvar*k))Cyc_Dict_lookup)(po->d,r))->resetable;}static int Cyc_RgnOrder_contains_rgnseff(
struct Cyc_Absyn_Tvar*rgns_of_var,void*eff){void*_tmp3D=Cyc_Tcutil_normalize_effect(
Cyc_Tcutil_compress(eff));void*_tmp3E;struct Cyc_List_List*_tmp3F;_LL2D: if(_tmp3D
<= (void*)4)goto _LL31;if(*((int*)_tmp3D)!= 21)goto _LL2F;_tmp3E=(void*)((struct
Cyc_Absyn_RgnsEff_struct*)_tmp3D)->f1;_LL2E: {void*_tmp40=Cyc_Tcutil_compress(
_tmp3E);struct Cyc_Absyn_Tvar*_tmp41;_LL34: if(_tmp40 <= (void*)4)goto _LL36;if(*((
int*)_tmp40)!= 1)goto _LL36;_tmp41=((struct Cyc_Absyn_VarType_struct*)_tmp40)->f1;
_LL35: return Cyc_Absyn_tvar_cmp(_tmp41,rgns_of_var)== 0;_LL36:;_LL37: return 0;
_LL33:;}_LL2F: if(*((int*)_tmp3D)!= 20)goto _LL31;_tmp3F=((struct Cyc_Absyn_JoinEff_struct*)
_tmp3D)->f1;_LL30: for(0;_tmp3F != 0;_tmp3F=_tmp3F->tl){if(Cyc_RgnOrder_contains_rgnseff(
rgns_of_var,(void*)_tmp3F->hd))return 1;}return 0;_LL31:;_LL32: return 0;_LL2C:;}
struct Cyc_RgnOrder_OutlivesEnv{struct _RegionHandle*r;struct Cyc_List_List*seen;
struct Cyc_List_List*todo;};static void Cyc_RgnOrder_add_to_search(struct Cyc_RgnOrder_OutlivesEnv*
env,void*eff){void*_tmp42=Cyc_Tcutil_compress(eff);void*_tmp43;struct Cyc_Absyn_Tvar*
_tmp44;struct Cyc_List_List*_tmp45;_LL39: if(_tmp42 <= (void*)4)goto _LL3D;if(*((int*)
_tmp42)!= 19)goto _LL3B;_tmp43=(void*)((struct Cyc_Absyn_AccessEff_struct*)_tmp42)->f1;
if(_tmp43 <= (void*)4)goto _LL3B;if(*((int*)_tmp43)!= 1)goto _LL3B;_tmp44=((struct
Cyc_Absyn_VarType_struct*)_tmp43)->f1;_LL3A:{struct Cyc_List_List*_tmp46=env->seen;
for(0;_tmp46 != 0;_tmp46=_tmp46->tl){if(Cyc_Absyn_tvar_cmp(_tmp44,(struct Cyc_Absyn_Tvar*)
_tmp46->hd)== 0)return;}}env->seen=({struct Cyc_List_List*_tmp47=_region_malloc(
env->r,sizeof(*_tmp47));_tmp47->hd=_tmp44;_tmp47->tl=env->seen;_tmp47;});env->todo=({
struct Cyc_List_List*_tmp48=_region_malloc(env->r,sizeof(*_tmp48));_tmp48->hd=
_tmp44;_tmp48->tl=env->todo;_tmp48;});return;_LL3B: if(*((int*)_tmp42)!= 20)goto
_LL3D;_tmp45=((struct Cyc_Absyn_JoinEff_struct*)_tmp42)->f1;_LL3C: for(0;_tmp45 != 
0;_tmp45=_tmp45->tl){Cyc_RgnOrder_add_to_search(env,(void*)_tmp45->hd);}return;
_LL3D:;_LL3E: return;_LL38:;}static struct Cyc_RgnOrder_OutlivesEnv Cyc_RgnOrder_initial_env(
struct _RegionHandle*listrgn,struct Cyc_RgnOrder_RgnPO*po,void*rgn){struct Cyc_RgnOrder_OutlivesEnv
_tmp49=({struct Cyc_RgnOrder_OutlivesEnv _tmp51;_tmp51.r=listrgn;_tmp51.seen=0;
_tmp51.todo=0;_tmp51;});void*r=Cyc_Tcutil_compress(rgn);{void*_tmp4A=Cyc_Tcutil_typ_kind(
r);_LL40: if((int)_tmp4A != 3)goto _LL42;_LL41: Cyc_RgnOrder_add_to_search(& _tmp49,
po->these_outlive_heap);goto _LL3F;_LL42: if((int)_tmp4A != 4)goto _LL44;_LL43: Cyc_RgnOrder_add_to_search(&
_tmp49,po->these_outlive_unique);goto _LL3F;_LL44: if((int)_tmp4A != 5)goto _LL46;
_LL45: Cyc_RgnOrder_add_to_search(& _tmp49,po->these_outlive_heap);Cyc_RgnOrder_add_to_search(&
_tmp49,po->these_outlive_unique);goto _LL3F;_LL46:;_LL47:({void*_tmp4B=0;((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmp4C="RgnOrder: rgn not of correct kind";_tag_dyneither(_tmp4C,sizeof(char),34);}),
_tag_dyneither(_tmp4B,sizeof(void*),0));});_LL3F:;}{void*_tmp4D=r;struct Cyc_Absyn_Tvar*
_tmp4E;_LL49: if(_tmp4D <= (void*)4)goto _LL4B;if(*((int*)_tmp4D)!= 1)goto _LL4B;
_tmp4E=((struct Cyc_Absyn_VarType_struct*)_tmp4D)->f1;_LL4A: _tmp49.seen=({struct
Cyc_List_List*_tmp4F=_region_malloc(listrgn,sizeof(*_tmp4F));_tmp4F->hd=_tmp4E;
_tmp4F->tl=_tmp49.seen;_tmp4F;});_tmp49.todo=({struct Cyc_List_List*_tmp50=
_region_malloc(listrgn,sizeof(*_tmp50));_tmp50->hd=_tmp4E;_tmp50->tl=_tmp49.todo;
_tmp50;});goto _LL4C;_LL4B:;_LL4C: return _tmp49;_LL48:;}}static int Cyc_RgnOrder_atomic_effect_outlives(
struct Cyc_RgnOrder_RgnPO*po,void*eff,void*rgn){struct _RegionHandle _tmp52=
_new_region("listrgn");struct _RegionHandle*listrgn=& _tmp52;_push_region(listrgn);{
void*_tmp53=Cyc_Tcutil_compress(eff);void*_tmp54;void*_tmp55;_LL4E: if(_tmp53 <= (
void*)4)goto _LL52;if(*((int*)_tmp53)!= 19)goto _LL50;_tmp54=(void*)((struct Cyc_Absyn_AccessEff_struct*)
_tmp53)->f1;_LL4F: {void*_tmp56=Cyc_Tcutil_compress(_tmp54);struct Cyc_Absyn_Tvar*
_tmp57;_LL55: if((int)_tmp56 != 3)goto _LL57;_LL56: {int _tmp58=1;_npop_handler(0);
return _tmp58;}_LL57: if((int)_tmp56 != 2)goto _LL59;_LL58: {void*_tmp59=Cyc_Tcutil_compress(
rgn);struct Cyc_Absyn_Tvar*_tmp5A;_LL5E: if((int)_tmp59 != 3)goto _LL60;_LL5F: {int
_tmp5B=0;_npop_handler(0);return _tmp5B;}_LL60: if(_tmp59 <= (void*)4)goto _LL62;if(*((
int*)_tmp59)!= 1)goto _LL62;_tmp5A=((struct Cyc_Absyn_VarType_struct*)_tmp59)->f1;
_LL61: if(Cyc_Tcutil_tvar_kind(_tmp5A)== (void*)3){int _tmp5C=1;_npop_handler(0);
return _tmp5C;}else{int _tmp5D=0;_npop_handler(0);return _tmp5D;}_LL62:;_LL63: {int
_tmp5E=1;_npop_handler(0);return _tmp5E;}_LL5D:;}_LL59: if(_tmp56 <= (void*)4)goto
_LL5B;if(*((int*)_tmp56)!= 1)goto _LL5B;_tmp57=((struct Cyc_Absyn_VarType_struct*)
_tmp56)->f1;_LL5A: {struct Cyc_RgnOrder_OutlivesEnv _tmp5F=Cyc_RgnOrder_initial_env(
listrgn,po,rgn);while(_tmp5F.todo != 0){struct Cyc_Absyn_Tvar*_tmp60=(struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*)_check_null(_tmp5F.todo))->hd;_tmp5F.todo=((struct Cyc_List_List*)
_check_null(_tmp5F.todo))->tl;if(Cyc_Absyn_tvar_cmp(_tmp60,_tmp57)== 0){int
_tmp61=1;_npop_handler(0);return _tmp61;}if(((int(*)(struct Cyc_Dict_Dict d,struct
Cyc_Absyn_Tvar*k))Cyc_Dict_member)(po->d,_tmp60))Cyc_RgnOrder_add_to_search(&
_tmp5F,(((struct Cyc_RgnOrder_RgnInfo*(*)(struct Cyc_Dict_Dict d,struct Cyc_Absyn_Tvar*
k))Cyc_Dict_lookup)(po->d,_tmp60))->these_outlive);else{({void*_tmp62=0;((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmp63="RgnOrder: type variable not in dict";_tag_dyneither(_tmp63,sizeof(char),
36);}),_tag_dyneither(_tmp62,sizeof(void*),0));});}}{int _tmp64=0;_npop_handler(0);
return _tmp64;}}_LL5B:;_LL5C: {int _tmp65=0;_npop_handler(0);return _tmp65;}_LL54:;}
_LL50: if(*((int*)_tmp53)!= 21)goto _LL52;_tmp55=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp53)->f1;_LL51: {void*_tmp66=Cyc_Tcutil_compress(_tmp55);struct Cyc_Absyn_Tvar*
_tmp67;_LL65: if(_tmp66 <= (void*)4)goto _LL67;if(*((int*)_tmp66)!= 1)goto _LL67;
_tmp67=((struct Cyc_Absyn_VarType_struct*)_tmp66)->f1;_LL66: {struct Cyc_RgnOrder_OutlivesEnv
_tmp68=Cyc_RgnOrder_initial_env(listrgn,po,rgn);void*_tmp69=Cyc_Tcutil_typ_kind(
rgn);if(_tmp69 == (void*)3  || _tmp69 == (void*)5){if(Cyc_RgnOrder_contains_rgnseff(
_tmp67,po->these_outlive_heap)){int _tmp6A=1;_npop_handler(0);return _tmp6A;}}if(
_tmp69 == (void*)4  || _tmp69 == (void*)5){if(Cyc_RgnOrder_contains_rgnseff(_tmp67,
po->these_outlive_unique)){int _tmp6B=1;_npop_handler(0);return _tmp6B;}}while(
_tmp68.todo != 0){struct Cyc_Absyn_Tvar*_tmp6C=(struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)
_check_null(_tmp68.todo))->hd;_tmp68.todo=((struct Cyc_List_List*)_check_null(
_tmp68.todo))->tl;if(((int(*)(struct Cyc_Dict_Dict d,struct Cyc_Absyn_Tvar*k))Cyc_Dict_member)(
po->d,_tmp6C)){void*_tmp6D=(((struct Cyc_RgnOrder_RgnInfo*(*)(struct Cyc_Dict_Dict
d,struct Cyc_Absyn_Tvar*k))Cyc_Dict_lookup)(po->d,_tmp6C))->these_outlive;if(Cyc_RgnOrder_contains_rgnseff(
_tmp67,_tmp6D)){int _tmp6E=1;_npop_handler(0);return _tmp6E;}Cyc_RgnOrder_add_to_search(&
_tmp68,_tmp6D);}else{({void*_tmp6F=0;((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp70="RgnOrder: type variable not in dict";
_tag_dyneither(_tmp70,sizeof(char),36);}),_tag_dyneither(_tmp6F,sizeof(void*),0));});}}{
int _tmp71=0;_npop_handler(0);return _tmp71;}}_LL67:;_LL68: {int _tmp72=0;
_npop_handler(0);return _tmp72;}_LL64:;}_LL52:;_LL53: {int _tmp73=0;_npop_handler(
0);return _tmp73;}_LL4D:;};_pop_region(listrgn);}int Cyc_RgnOrder_effect_outlives(
struct Cyc_RgnOrder_RgnPO*po,void*eff,void*rgn){eff=Cyc_Tcutil_normalize_effect(
eff);{void*_tmp74=Cyc_Tcutil_compress(eff);struct Cyc_List_List*_tmp75;_LL6A: if(
_tmp74 <= (void*)4)goto _LL6C;if(*((int*)_tmp74)!= 20)goto _LL6C;_tmp75=((struct Cyc_Absyn_JoinEff_struct*)
_tmp74)->f1;_LL6B: for(0;_tmp75 != 0;_tmp75=_tmp75->tl){if(!Cyc_RgnOrder_effect_outlives(
po,(void*)_tmp75->hd,rgn))return 0;}return 1;_LL6C:;_LL6D: return Cyc_RgnOrder_atomic_effect_outlives(
po,eff,rgn);_LL69:;}}static void Cyc_RgnOrder_pin_effect(void*eff,void*bd){eff=Cyc_Tcutil_normalize_effect(
eff);{void*_tmp76=Cyc_Tcutil_compress(eff);struct Cyc_List_List*_tmp77;void*
_tmp78;void*_tmp79;_LL6F: if(_tmp76 <= (void*)4)goto _LL77;if(*((int*)_tmp76)!= 20)
goto _LL71;_tmp77=((struct Cyc_Absyn_JoinEff_struct*)_tmp76)->f1;_LL70: for(0;
_tmp77 != 0;_tmp77=_tmp77->tl){Cyc_RgnOrder_pin_effect((void*)_tmp77->hd,bd);}
return;_LL71: if(*((int*)_tmp76)!= 21)goto _LL73;_tmp78=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp76)->f1;_LL72: {void*_tmp7A=Cyc_Tcutil_compress(_tmp78);_LL7A: if(_tmp7A <= (
void*)4)goto _LL7C;if(*((int*)_tmp7A)!= 0)goto _LL7C;_LL7B: Cyc_Tcutil_unify(_tmp78,
Cyc_Absyn_uint_typ);return;_LL7C:;_LL7D: return;_LL79:;}_LL73: if(*((int*)_tmp76)
!= 19)goto _LL75;_tmp79=(void*)((struct Cyc_Absyn_AccessEff_struct*)_tmp76)->f1;
_LL74: {void*_tmp7B=Cyc_Tcutil_compress(_tmp79);_LL7F: if(_tmp7B <= (void*)4)goto
_LL81;if(*((int*)_tmp7B)!= 0)goto _LL81;_LL80: Cyc_Tcutil_unify(_tmp79,bd);return;
_LL81:;_LL82: return;_LL7E:;}_LL75: if(*((int*)_tmp76)!= 0)goto _LL77;_LL76: Cyc_Tcutil_unify(
eff,Cyc_Absyn_empty_effect);return;_LL77:;_LL78: return;_LL6E:;}}int Cyc_RgnOrder_satisfies_constraints(
struct Cyc_RgnOrder_RgnPO*po,struct Cyc_List_List*constraints,void*default_bound,
int do_pin){{struct Cyc_List_List*_tmp7C=constraints;for(0;_tmp7C != 0;_tmp7C=
_tmp7C->tl){struct _tuple0 _tmp7E;void*_tmp7F;struct _tuple0*_tmp7D=(struct _tuple0*)
_tmp7C->hd;_tmp7E=*_tmp7D;_tmp7F=_tmp7E.f2;{void*_tmp80=Cyc_Tcutil_compress(
_tmp7F);_LL84: if(_tmp80 <= (void*)4)goto _LL86;if(*((int*)_tmp80)!= 0)goto _LL86;
_LL85: if(do_pin)Cyc_Tcutil_unify(_tmp7F,default_bound);goto _LL83;_LL86:;_LL87:
goto _LL83;_LL83:;}}}{struct Cyc_List_List*_tmp81=constraints;for(0;_tmp81 != 0;
_tmp81=_tmp81->tl){struct _tuple0 _tmp83;void*_tmp84;void*_tmp85;struct _tuple0*
_tmp82=(struct _tuple0*)_tmp81->hd;_tmp83=*_tmp82;_tmp84=_tmp83.f1;_tmp85=_tmp83.f2;
if(do_pin)Cyc_RgnOrder_pin_effect(_tmp84,_tmp85);if(!Cyc_RgnOrder_effect_outlives(
po,_tmp84,_tmp85))return 0;}}return 1;}static int Cyc_RgnOrder_eff_outlives_atomic_eff(
struct Cyc_RgnOrder_RgnPO*po,void*eff1,void*eff2){eff2=Cyc_Tcutil_compress(Cyc_Tcutil_normalize_effect(
eff2));{void*_tmp86=eff2;struct Cyc_List_List*_tmp87;void*_tmp88;void*_tmp89;
_LL89: if(_tmp86 <= (void*)4)goto _LL8F;if(*((int*)_tmp86)!= 20)goto _LL8B;_tmp87=((
struct Cyc_Absyn_JoinEff_struct*)_tmp86)->f1;_LL8A: for(0;_tmp87 != 0;_tmp87=_tmp87->tl){
if(Cyc_RgnOrder_eff_outlives_atomic_eff(po,eff1,(void*)_tmp87->hd))return 1;}
return 0;_LL8B: if(*((int*)_tmp86)!= 19)goto _LL8D;_tmp88=(void*)((struct Cyc_Absyn_AccessEff_struct*)
_tmp86)->f1;_LL8C: return Cyc_RgnOrder_effect_outlives(po,eff1,_tmp88);_LL8D: if(*((
int*)_tmp86)!= 21)goto _LL8F;_tmp89=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp86)->f1;_LL8E: {void*_tmp8A=eff1;void*_tmp8B;_LL92: if(_tmp8A <= (void*)4)goto
_LL94;if(*((int*)_tmp8A)!= 21)goto _LL94;_tmp8B=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp8A)->f1;_LL93:{struct _tuple0 _tmp8D=({struct _tuple0 _tmp8C;_tmp8C.f1=Cyc_Tcutil_compress(
_tmp89);_tmp8C.f2=Cyc_Tcutil_compress(_tmp8B);_tmp8C;});void*_tmp8E;struct Cyc_Absyn_Tvar*
_tmp8F;void*_tmp90;struct Cyc_Absyn_Tvar*_tmp91;_LL97: _tmp8E=_tmp8D.f1;if(_tmp8E
<= (void*)4)goto _LL99;if(*((int*)_tmp8E)!= 1)goto _LL99;_tmp8F=((struct Cyc_Absyn_VarType_struct*)
_tmp8E)->f1;_tmp90=_tmp8D.f2;if(_tmp90 <= (void*)4)goto _LL99;if(*((int*)_tmp90)!= 
1)goto _LL99;_tmp91=((struct Cyc_Absyn_VarType_struct*)_tmp90)->f1;_LL98: return Cyc_Absyn_tvar_cmp(
_tmp8F,_tmp91)== 0;_LL99:;_LL9A: goto _LL96;_LL96:;}goto _LL95;_LL94:;_LL95: return
eff1 == (void*)2;_LL91:;}_LL8F:;_LL90: return eff1 == (void*)2;_LL88:;}}int Cyc_RgnOrder_eff_outlives_eff(
struct Cyc_RgnOrder_RgnPO*po,void*eff1,void*eff2){eff1=Cyc_Tcutil_compress(Cyc_Tcutil_normalize_effect(
eff1));{void*_tmp92=eff1;struct Cyc_List_List*_tmp93;_LL9C: if(_tmp92 <= (void*)4)
goto _LL9E;if(*((int*)_tmp92)!= 20)goto _LL9E;_tmp93=((struct Cyc_Absyn_JoinEff_struct*)
_tmp92)->f1;_LL9D: for(0;_tmp93 != 0;_tmp93=_tmp93->tl){if(!Cyc_RgnOrder_eff_outlives_eff(
po,(void*)_tmp93->hd,eff2))return 0;}return 1;_LL9E:;_LL9F: return Cyc_RgnOrder_eff_outlives_atomic_eff(
po,eff1,eff2);_LL9B:;}}struct _tuple10{struct Cyc_Absyn_Tvar*f1;struct Cyc_RgnOrder_RgnInfo*
f2;};void Cyc_RgnOrder_print_region_po(struct Cyc_RgnOrder_RgnPO*po){struct
_RegionHandle _tmp94=_new_region("r");struct _RegionHandle*r=& _tmp94;_push_region(
r);{struct Cyc_Iter_Iter _tmp95=((struct Cyc_Iter_Iter(*)(struct _RegionHandle*rgn,
struct Cyc_Dict_Dict d))Cyc_Dict_make_iter)(r,po->d);struct _tuple10 _tmp96=*((
struct _tuple10*(*)(struct _RegionHandle*r,struct Cyc_Dict_Dict d))Cyc_Dict_rchoose)(
r,po->d);({void*_tmp97=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp98="region po:\n";
_tag_dyneither(_tmp98,sizeof(char),12);}),_tag_dyneither(_tmp97,sizeof(void*),0));});
while(((int(*)(struct Cyc_Iter_Iter,struct _tuple10*))Cyc_Iter_next)(_tmp95,&
_tmp96)){({struct Cyc_String_pa_struct _tmp9C;_tmp9C.tag=0;_tmp9C.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((_tmp96.f2)->these_outlive));{
struct Cyc_String_pa_struct _tmp9B;_tmp9B.tag=0;_tmp9B.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)*(_tmp96.f1)->name);{void*_tmp99[2]={& _tmp9B,& _tmp9C};Cyc_fprintf(
Cyc_stderr,({const char*_tmp9A="  %s outlived by %s\n";_tag_dyneither(_tmp9A,
sizeof(char),21);}),_tag_dyneither(_tmp99,sizeof(void*),2));}}});}({struct Cyc_String_pa_struct
_tmp9F;_tmp9F.tag=0;_tmp9F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
po->these_outlive_heap));{void*_tmp9D[1]={& _tmp9F};Cyc_fprintf(Cyc_stderr,({
const char*_tmp9E="  these outlive heap: %s\n";_tag_dyneither(_tmp9E,sizeof(char),
26);}),_tag_dyneither(_tmp9D,sizeof(void*),1));}});({struct Cyc_String_pa_struct
_tmpA2;_tmpA2.tag=0;_tmpA2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
po->these_outlive_unique));{void*_tmpA0[1]={& _tmpA2};Cyc_fprintf(Cyc_stderr,({
const char*_tmpA1="  these outlive unique: %s\n";_tag_dyneither(_tmpA1,sizeof(
char),28);}),_tag_dyneither(_tmpA0,sizeof(void*),1));}});};_pop_region(r);}
