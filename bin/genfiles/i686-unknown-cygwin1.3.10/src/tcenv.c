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
struct _dyneither_ptr);int Cyc_fflush(struct Cyc___cycFILE*);int Cyc_fprintf(struct
Cyc___cycFILE*,struct _dyneither_ptr,struct _dyneither_ptr);struct Cyc_ShortPtr_sa_struct{
int tag;short*f1;};struct Cyc_UShortPtr_sa_struct{int tag;unsigned short*f1;};
struct Cyc_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{int tag;
unsigned int*f1;};struct Cyc_StringPtr_sa_struct{int tag;struct _dyneither_ptr f1;};
struct Cyc_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_struct{
int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int tag;struct _dyneither_ptr f1;};
extern char Cyc_FileCloseError[19];extern char Cyc_FileOpenError[18];struct Cyc_FileOpenError_struct{
char*tag;struct _dyneither_ptr f1;};struct Cyc_List_List{void*hd;struct Cyc_List_List*
tl;};extern char Cyc_List_List_mismatch[18];struct Cyc_List_List*Cyc_List_rev(
struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*
x);struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*
y);extern char Cyc_List_Nth[8];struct Cyc_Iter_Iter{void*env;int(*next)(void*env,
void*dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Dict_T;struct
Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;struct Cyc_Dict_T*t;};
extern char Cyc_Dict_Present[12];extern char Cyc_Dict_Absent[11];struct Cyc_Dict_Dict
Cyc_Dict_rempty(struct _RegionHandle*,int(*cmp)(void*,void*));int Cyc_Dict_is_empty(
struct Cyc_Dict_Dict d);int Cyc_Dict_member(struct Cyc_Dict_Dict d,void*k);struct Cyc_Dict_Dict
Cyc_Dict_insert(struct Cyc_Dict_Dict d,void*k,void*v);void*Cyc_Dict_lookup(struct
Cyc_Dict_Dict d,void*k);void**Cyc_Dict_lookup_opt(struct Cyc_Dict_Dict d,void*k);
struct _tuple0{void*f1;void*f2;};struct _tuple0*Cyc_Dict_rchoose(struct
_RegionHandle*r,struct Cyc_Dict_Dict d);struct _tuple0*Cyc_Dict_rchoose(struct
_RegionHandle*,struct Cyc_Dict_Dict d);struct Cyc_Dict_Dict Cyc_Dict_rdelete(struct
_RegionHandle*,struct Cyc_Dict_Dict,void*);struct Cyc_Set_Set;struct Cyc_Set_Set*
Cyc_Set_empty(int(*cmp)(void*,void*));struct Cyc_Set_Set*Cyc_Set_rempty(struct
_RegionHandle*r,int(*cmp)(void*,void*));int Cyc_Set_member(struct Cyc_Set_Set*s,
void*elt);extern char Cyc_Set_Absent[11];struct Cyc_SlowDict_Dict;extern char Cyc_SlowDict_Present[
12];extern char Cyc_SlowDict_Absent[11];struct Cyc_Lineno_Pos{struct _dyneither_ptr
logical_file;struct _dyneither_ptr line;int line_no;int col;};extern char Cyc_Position_Exit[
9];struct Cyc_Position_Segment;struct Cyc_Position_Error{struct _dyneither_ptr
source;struct Cyc_Position_Segment*seg;void*kind;struct _dyneither_ptr desc;};
extern char Cyc_Position_Nocontext[14];int Cyc_strptrcmp(struct _dyneither_ptr*s1,
struct _dyneither_ptr*s2);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;
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
_dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];int Cyc_Absyn_varlist_cmp(
struct Cyc_List_List*,struct Cyc_List_List*);struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(
struct Cyc_Position_Segment*);extern union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref;
extern void*Cyc_Absyn_empty_effect;extern struct _tuple1*Cyc_Absyn_exn_name;extern
struct Cyc_Absyn_Datatypedecl*Cyc_Absyn_exn_tud;void*Cyc_Absyn_dyneither_typ(void*
t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);struct
Cyc_Absynpp_Params{int expand_typedefs: 1;int qvar_to_Cids: 1;int add_cyc_prefix: 1;
int to_VC: 1;int decls_first: 1;int rewrite_temp_tvars: 1;int print_all_tvars: 1;int
print_all_kinds: 1;int print_all_effects: 1;int print_using_stmts: 1;int
print_externC_stmts: 1;int print_full_evars: 1;int print_zeroterm: 1;int
generate_line_directives: 1;int use_curr_namespace: 1;struct Cyc_List_List*
curr_namespace;};struct _dyneither_ptr Cyc_Absynpp_typ2string(void*);struct
_dyneither_ptr Cyc_Absynpp_kind2string(void*);struct Cyc_RgnOrder_RgnPO;struct Cyc_RgnOrder_RgnPO*
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
le;int allow_valueof;};void*Cyc_Tcenv_env_err(struct _dyneither_ptr msg);struct
_RegionHandle*Cyc_Tcenv_get_fnrgn(struct Cyc_Tcenv_Tenv*);struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_tc_init(struct _RegionHandle*);struct Cyc_Tcenv_Genv*Cyc_Tcenv_empty_genv(
struct _RegionHandle*);struct Cyc_Tcenv_Fenv*Cyc_Tcenv_new_fenv(struct
_RegionHandle*,struct Cyc_Position_Segment*,struct Cyc_Absyn_Fndecl*);struct Cyc_Tcenv_Fenv*
Cyc_Tcenv_nested_fenv(struct Cyc_Position_Segment*,struct Cyc_Tcenv_Fenv*old_fenv,
struct Cyc_Absyn_Fndecl*new_fn);struct Cyc_List_List*Cyc_Tcenv_resolve_namespace(
struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,struct _dyneither_ptr*,struct
Cyc_List_List*);void*Cyc_Tcenv_lookup_ordinary(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*,struct _tuple1*);struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl(
struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,struct _tuple1*);struct Cyc_Absyn_Datatypedecl**
Cyc_Tcenv_lookup_datatypedecl(struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,
struct _tuple1*);struct Cyc_Absyn_Datatypedecl***Cyc_Tcenv_lookup_xdatatypedecl(
struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,struct
_tuple1*);struct Cyc_Absyn_Enumdecl**Cyc_Tcenv_lookup_enumdecl(struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*,struct _tuple1*);struct Cyc_Absyn_Typedefdecl*Cyc_Tcenv_lookup_typedefdecl(
struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,struct _tuple1*);struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_allow_valueof(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*);void*Cyc_Tcenv_return_typ(
struct Cyc_Tcenv_Tenv*);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_copy_tenv(struct
_RegionHandle*,struct Cyc_Tcenv_Tenv*);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_local_var(
struct _RegionHandle*,struct Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Vardecl*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_pat_var(struct _RegionHandle*,struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Vardecl*);struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(
struct Cyc_Tcenv_Tenv*);struct Cyc_Core_Opt*Cyc_Tcenv_lookup_opt_type_vars(struct
Cyc_Tcenv_Tenv*te);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_type_vars(struct
_RegionHandle*,struct Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_in_loop(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Stmt*continue_dest);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_in_switch(
struct _RegionHandle*,struct Cyc_Tcenv_Tenv*);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_fallthru(
struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*new_tvs,struct
Cyc_List_List*vds,struct Cyc_Absyn_Switch_clause*clause);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_fallthru(
struct _RegionHandle*,struct Cyc_Tcenv_Tenv*);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_next(
struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,void*);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_try(
struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te);int Cyc_Tcenv_get_try_depth(struct
Cyc_Tcenv_Tenv*te);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_notreadctxt(struct
_RegionHandle*,struct Cyc_Tcenv_Tenv*te);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_notreadctxt(
struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te);int Cyc_Tcenv_in_notreadctxt(struct
Cyc_Tcenv_Tenv*te);void Cyc_Tcenv_process_continue(struct Cyc_Tcenv_Tenv*,struct
Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt**);void Cyc_Tcenv_process_break(struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt**);void Cyc_Tcenv_process_goto(struct
Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*,struct _dyneither_ptr*,struct Cyc_Absyn_Stmt**);
struct _tuple10{struct Cyc_Absyn_Switch_clause*f1;struct Cyc_List_List*f2;struct Cyc_Tcenv_CList*
f3;};struct _tuple10*Cyc_Tcenv_process_fallthru(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*,
struct Cyc_Absyn_Switch_clause***);struct Cyc_Absyn_Stmt*Cyc_Tcenv_get_encloser(
struct Cyc_Tcenv_Tenv*);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_encloser(struct
_RegionHandle*,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*);struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_add_label(struct Cyc_Tcenv_Tenv*,struct _dyneither_ptr*,struct Cyc_Absyn_Stmt*);
int Cyc_Tcenv_all_labels_resolved(struct Cyc_Tcenv_Tenv*);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_block(
struct _RegionHandle*,struct Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*);struct
Cyc_Tcenv_Tenv*Cyc_Tcenv_new_named_block(struct _RegionHandle*,struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Tvar*name);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_outlives_constraints(
struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*cs,struct Cyc_Position_Segment*
loc);struct _tuple11{struct Cyc_Absyn_Tvar*f1;void*f2;};struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region_equality(
struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,void*r1,void*r2,struct _tuple11**
oldtv,struct Cyc_Position_Segment*loc);void*Cyc_Tcenv_curr_rgn(struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*
te,void*r,int resetable,int opened);void Cyc_Tcenv_check_rgn_accessible(struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*,void*rgn);void Cyc_Tcenv_check_rgn_resetable(struct
Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,void*rgn);void Cyc_Tcenv_check_effect_accessible(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void*eff);int Cyc_Tcenv_region_outlives(
struct Cyc_Tcenv_Tenv*,void*r1,void*r2);void Cyc_Tcenv_check_rgn_partial_order(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct Cyc_List_List*po);
void Cyc_Tcenv_check_delayed_effects(struct Cyc_Tcenv_Tenv*te);void Cyc_Tcenv_check_delayed_constraints(
struct Cyc_Tcenv_Tenv*te);void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap);void Cyc_Tcutil_terr(struct Cyc_Position_Segment*,struct
_dyneither_ptr fmt,struct _dyneither_ptr ap);extern struct Cyc_Core_Opt*Cyc_Tcutil_empty_var_set;
int Cyc_Tcutil_kind_leq(void*k1,void*k2);void*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*
t);void*Cyc_Tcutil_typ_kind(void*t);void*Cyc_Tcutil_compress(void*t);void*Cyc_Tcutil_kind_to_bound(
void*k);struct _tuple11 Cyc_Tcutil_swap_kind(void*t,void*kb);int Cyc_Tcutil_subset_effect(
int may_constrain_evars,void*e1,void*e2);int Cyc_Tcutil_region_in_effect(int
constrain,void*r,void*e);void Cyc_Tcutil_check_unique_tvars(struct Cyc_Position_Segment*,
struct Cyc_List_List*);struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k);int Cyc_Tcutil_new_tvar_id();
void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*);void Cyc_Tcutil_add_tvar_identities(
struct Cyc_List_List*);char Cyc_Tcenv_Env_error[14]="\000\000\000\000Env_error\000";
void*Cyc_Tcenv_env_err(struct _dyneither_ptr msg){({struct Cyc_String_pa_struct
_tmp2;_tmp2.tag=0;_tmp2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg);{
void*_tmp0[1]={& _tmp2};Cyc_fprintf(Cyc_stderr,({const char*_tmp1="Internal error in tcenv: %s\n";
_tag_dyneither(_tmp1,sizeof(char),29);}),_tag_dyneither(_tmp0,sizeof(void*),1));}});
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);(int)_throw((void*)Cyc_Tcenv_Env_error);}
struct Cyc_Tcenv_Tenv;struct Cyc_Tcenv_Genv;struct Cyc_Tcenv_CtrlEnv{struct
_RegionHandle*ctrl_rgn;void*continue_stmt;void*break_stmt;struct _tuple10*
fallthru_clause;void*next_stmt;int try_depth;};struct Cyc_Tcenv_SharedFenv{struct
_RegionHandle*frgn;void*return_typ;struct Cyc_Dict_Dict seen_labels;struct Cyc_Dict_Dict
needed_labels;struct Cyc_List_List*delayed_effect_checks;struct Cyc_List_List*
delayed_constraint_checks;};struct Cyc_Tcenv_Bindings{struct _dyneither_ptr*v;void*
b;struct Cyc_Tcenv_Bindings*tl;};struct Cyc_Tcenv_Fenv{struct Cyc_Tcenv_SharedFenv*
shared;struct Cyc_List_List*type_vars;struct Cyc_RgnOrder_RgnPO*region_order;
struct Cyc_Tcenv_Bindings*locals;struct Cyc_Absyn_Stmt*encloser;struct Cyc_Tcenv_CtrlEnv*
ctrl_env;void*capability;void*curr_rgn;int in_notreadctxt;struct _RegionHandle*
fnrgn;};char Cyc_Tcenv_NoBinding[14]="\000\000\000\000NoBinding\000";void*Cyc_Tcenv_lookup_binding(
struct Cyc_Tcenv_Bindings*bs,struct _dyneither_ptr*v){for(0;(unsigned int)bs;bs=bs->tl){
if(Cyc_strptrcmp(v,bs->v)== 0)return bs->b;}(int)_throw((void*)Cyc_Tcenv_NoBinding);}
struct Cyc_Tcenv_Genv*Cyc_Tcenv_empty_genv(struct _RegionHandle*r){return({struct
Cyc_Tcenv_Genv*_tmp3=_region_malloc(r,sizeof(*_tmp3));_tmp3->grgn=r;_tmp3->namespaces=((
struct Cyc_Set_Set*(*)(struct _RegionHandle*r,int(*cmp)(struct _dyneither_ptr*,
struct _dyneither_ptr*)))Cyc_Set_rempty)(r,Cyc_strptrcmp);_tmp3->aggrdecls=((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,
struct _dyneither_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp);_tmp3->datatypedecls=((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,
struct _dyneither_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp);_tmp3->enumdecls=((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,
struct _dyneither_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp);_tmp3->typedefs=((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,
struct _dyneither_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp);_tmp3->ordinaries=((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,
struct _dyneither_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp);_tmp3->availables=0;
_tmp3;});}struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tc_init(struct _RegionHandle*r){Cyc_Tcutil_empty_var_set=({
struct Cyc_Core_Opt*_tmp4=_cycalloc(sizeof(*_tmp4));_tmp4->v=((struct Cyc_Set_Set*(*)(
int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp);
_tmp4;});{struct Cyc_Tcenv_Genv*_tmp5=Cyc_Tcenv_empty_genv(r);_tmp5->datatypedecls=((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Datatypedecl**
v))Cyc_Dict_insert)(_tmp5->datatypedecls,(*Cyc_Absyn_exn_name).f2,({struct Cyc_Absyn_Datatypedecl**
_tmp6=_cycalloc(sizeof(*_tmp6));_tmp6[0]=Cyc_Absyn_exn_tud;_tmp6;}));{struct Cyc_List_List*
_tmp7=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(Cyc_Absyn_exn_tud->fields))->v;
for(0;_tmp7 != 0;_tmp7=_tmp7->tl){_tmp5->ordinaries=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct _tuple6*v))Cyc_Dict_insert)(
_tmp5->ordinaries,(*((struct Cyc_Absyn_Datatypefield*)_tmp7->hd)->name).f2,({
struct _tuple6*_tmp8=_region_malloc(r,sizeof(*_tmp8));_tmp8->f1=(void*)({struct
Cyc_Tcenv_DatatypeRes_struct*_tmp9=_cycalloc(sizeof(*_tmp9));_tmp9[0]=({struct
Cyc_Tcenv_DatatypeRes_struct _tmpA;_tmpA.tag=2;_tmpA.f1=Cyc_Absyn_exn_tud;_tmpA.f2=(
struct Cyc_Absyn_Datatypefield*)_tmp7->hd;_tmpA;});_tmp9;});_tmp8->f2=1;_tmp8;}));}}{
struct Cyc_Dict_Dict ae=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*
k,struct Cyc_Tcenv_Genv*v))Cyc_Dict_insert)(((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,int(*cmp)(struct Cyc_List_List*,struct Cyc_List_List*)))Cyc_Dict_rempty)(
r,Cyc_Absyn_varlist_cmp),0,_tmp5);return({struct Cyc_Tcenv_Tenv*_tmpB=
_region_malloc(r,sizeof(*_tmpB));_tmpB->ns=0;_tmpB->ae=ae;_tmpB->le=0;_tmpB->allow_valueof=
0;_tmpB;});}}}static struct Cyc_Tcenv_Genv*Cyc_Tcenv_lookup_namespace(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,struct _dyneither_ptr*n,struct Cyc_List_List*ns){
return((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(
te->ae,Cyc_Tcenv_resolve_namespace(te,loc,n,ns));}static struct Cyc_List_List*Cyc_Tcenv_outer_namespace(
struct Cyc_List_List*ns){if(ns == 0)return((struct Cyc_List_List*(*)(struct
_dyneither_ptr msg))Cyc_Tcenv_env_err)(({const char*_tmpC="outer_namespace";
_tag_dyneither(_tmpC,sizeof(char),16);}));return((struct Cyc_List_List*(*)(struct
Cyc_List_List*x))Cyc_List_rev)(((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(ns)))->tl);}static int Cyc_Tcenv_same_namespace(
struct Cyc_List_List*n1,struct Cyc_List_List*n2){if(n1 == n2)return 1;for(0;n1 != 0;
n1=n1->tl){if(n2 == 0)return 0;if(Cyc_strptrcmp((struct _dyneither_ptr*)n1->hd,(
struct _dyneither_ptr*)n2->hd)!= 0)return 0;n2=n2->tl;}if(n2 != 0)return 0;return 1;}
static void Cyc_Tcenv_check_repeat(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void*(*lookup)(struct Cyc_Tcenv_Genv*,struct _dyneither_ptr*),struct
_dyneither_ptr*v,struct Cyc_List_List*cns,struct Cyc_List_List*nss){for(0;nss != 0;
nss=nss->tl){if(!Cyc_Tcenv_same_namespace(cns,(struct Cyc_List_List*)nss->hd)){
struct Cyc_Tcenv_Genv*ge2=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct
Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,(struct Cyc_List_List*)nss->hd);struct
_handler_cons _tmpD;_push_handler(& _tmpD);{int _tmpF=0;if(setjmp(_tmpD.handler))
_tmpF=1;if(!_tmpF){lookup(ge2,v);({struct Cyc_String_pa_struct _tmp12;_tmp12.tag=0;
_tmp12.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*v);{void*_tmp10[1]={&
_tmp12};Cyc_Tcutil_terr(loc,({const char*_tmp11="%s is ambiguous";_tag_dyneither(
_tmp11,sizeof(char),16);}),_tag_dyneither(_tmp10,sizeof(void*),1));}});;
_pop_handler();}else{void*_tmpE=(void*)_exn_thrown;void*_tmp14=_tmpE;_LL1: if(
_tmp14 != Cyc_Dict_Absent)goto _LL3;_LL2: goto _LL0;_LL3:;_LL4:(void)_throw(_tmp14);
_LL0:;}}}}return;}static void*Cyc_Tcenv_scoped_lookup(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,void*(*lookup)(struct Cyc_Tcenv_Genv*,struct
_dyneither_ptr*),struct _dyneither_ptr*v){struct Cyc_List_List*cns=te->ns;while(1){
struct Cyc_Tcenv_Genv*ge=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct
Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,cns);{struct _handler_cons _tmp15;
_push_handler(& _tmp15);{int _tmp17=0;if(setjmp(_tmp15.handler))_tmp17=1;if(!
_tmp17){{void*result=lookup(ge,v);Cyc_Tcenv_check_repeat(te,loc,lookup,v,cns,ge->availables);{
void*_tmp18=result;_npop_handler(0);return _tmp18;}};_pop_handler();}else{void*
_tmp16=(void*)_exn_thrown;void*_tmp1A=_tmp16;_LL6: if(_tmp1A != Cyc_Dict_Absent)
goto _LL8;_LL7: goto _LL5;_LL8:;_LL9:(void)_throw(_tmp1A);_LL5:;}}}{struct Cyc_List_List*
_tmp1B=ge->availables;for(0;_tmp1B != 0;_tmp1B=_tmp1B->tl){struct Cyc_Tcenv_Genv*
ge2=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(
te->ae,(struct Cyc_List_List*)_tmp1B->hd);struct _handler_cons _tmp1C;_push_handler(&
_tmp1C);{int _tmp1E=0;if(setjmp(_tmp1C.handler))_tmp1E=1;if(!_tmp1E){{void*result=
lookup(ge2,v);Cyc_Tcenv_check_repeat(te,loc,lookup,v,(struct Cyc_List_List*)
_tmp1B->hd,_tmp1B->tl);{void*_tmp1F=result;_npop_handler(0);return _tmp1F;}};
_pop_handler();}else{void*_tmp1D=(void*)_exn_thrown;void*_tmp21=_tmp1D;_LLB: if(
_tmp21 != Cyc_Dict_Absent)goto _LLD;_LLC: goto _LLA;_LLD:;_LLE:(void)_throw(_tmp21);
_LLA:;}}}}if(cns == 0)(int)_throw((void*)Cyc_Dict_Absent);cns=Cyc_Tcenv_outer_namespace(
cns);}}static void*Cyc_Tcenv_lookup_ordinary_global_f(struct Cyc_Tcenv_Genv*ge,
struct _dyneither_ptr*v){struct Cyc_Dict_Dict _tmp22=ge->ordinaries;struct _tuple6*
ans=((struct _tuple6*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(
_tmp22,v);(*ans).f2=1;return(*ans).f1;}static void*Cyc_Tcenv_lookup_ordinary_global(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct _tuple1*q){struct
_tuple1 _tmp24;union Cyc_Absyn_Nmspace _tmp25;struct _dyneither_ptr*_tmp26;struct
_tuple1*_tmp23=q;_tmp24=*_tmp23;_tmp25=_tmp24.f1;_tmp26=_tmp24.f2;{union Cyc_Absyn_Nmspace
_tmp27=_tmp25;int _tmp28;struct Cyc_List_List*_tmp29;struct Cyc_List_List*_tmp2A;
struct Cyc_List_List _tmp2B;struct _dyneither_ptr*_tmp2C;struct Cyc_List_List*_tmp2D;
struct Cyc_List_List*_tmp2E;_LL10: if((_tmp27.Loc_n).tag != 3)goto _LL12;_tmp28=(int)(
_tmp27.Loc_n).val;_LL11: goto _LL13;_LL12: if((_tmp27.Rel_n).tag != 1)goto _LL14;
_tmp29=(struct Cyc_List_List*)(_tmp27.Rel_n).val;if(_tmp29 != 0)goto _LL14;_LL13:
return Cyc_Tcenv_scoped_lookup(te,loc,Cyc_Tcenv_lookup_ordinary_global_f,_tmp26);
_LL14: if((_tmp27.Rel_n).tag != 1)goto _LL16;_tmp2A=(struct Cyc_List_List*)(_tmp27.Rel_n).val;
if(_tmp2A == 0)goto _LL16;_tmp2B=*_tmp2A;_tmp2C=(struct _dyneither_ptr*)_tmp2B.hd;
_tmp2D=_tmp2B.tl;_LL15: {struct Cyc_Tcenv_Genv*_tmp2F=Cyc_Tcenv_lookup_namespace(
te,loc,_tmp2C,_tmp2D);return Cyc_Tcenv_lookup_ordinary_global_f(_tmp2F,_tmp26);}
_LL16: if((_tmp27.Abs_n).tag != 2)goto _LLF;_tmp2E=(struct Cyc_List_List*)(_tmp27.Abs_n).val;
_LL17: return Cyc_Tcenv_lookup_ordinary_global_f(((struct Cyc_Tcenv_Genv*(*)(struct
Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,_tmp2E),_tmp26);
_LLF:;}}struct Cyc_List_List*Cyc_Tcenv_resolve_namespace(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,struct _dyneither_ptr*n,struct Cyc_List_List*ns){
struct Cyc_List_List*_tmp30=te->ns;struct _RegionHandle*_tmp31=Cyc_Tcenv_get_fnrgn(
te);struct Cyc_List_List*_tmp32=0;while(1){struct Cyc_Tcenv_Genv*_tmp33=((struct
Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(
te->ae,_tmp30);struct Cyc_List_List*_tmp34=_tmp33->availables;struct Cyc_Set_Set*
_tmp35=_tmp33->namespaces;{struct Cyc_List_List*_tmp36=_tmp34;for(0;_tmp36 != 0;
_tmp36=_tmp36->tl){struct Cyc_Set_Set*_tmp37=(((struct Cyc_Tcenv_Genv*(*)(struct
Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,(struct Cyc_List_List*)
_tmp36->hd))->namespaces;if(((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*
elt))Cyc_Set_member)(_tmp37,n))_tmp32=({struct Cyc_List_List*_tmp38=
_region_malloc(_tmp31,sizeof(*_tmp38));_tmp38->hd=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp30,({struct Cyc_List_List*
_tmp39=_cycalloc(sizeof(*_tmp39));_tmp39->hd=n;_tmp39->tl=ns;_tmp39;}));_tmp38->tl=
_tmp32;_tmp38;});}}if(((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(
_tmp35,n))_tmp32=({struct Cyc_List_List*_tmp3A=_region_malloc(_tmp31,sizeof(*
_tmp3A));_tmp3A->hd=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_append)(_tmp30,({struct Cyc_List_List*_tmp3B=_cycalloc(sizeof(*_tmp3B));
_tmp3B->hd=n;_tmp3B->tl=ns;_tmp3B;}));_tmp3A->tl=_tmp32;_tmp3A;});if(_tmp32 != 0){
if(_tmp32->tl != 0)({struct Cyc_String_pa_struct _tmp3E;_tmp3E.tag=0;_tmp3E.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*n);{void*_tmp3C[1]={& _tmp3E};Cyc_Tcutil_terr(
loc,({const char*_tmp3D="%s is ambiguous";_tag_dyneither(_tmp3D,sizeof(char),16);}),
_tag_dyneither(_tmp3C,sizeof(void*),1));}});return(struct Cyc_List_List*)_tmp32->hd;}
if(_tmp30 == 0)(int)_throw((void*)Cyc_Dict_Absent);_tmp30=Cyc_Tcenv_outer_namespace(
_tmp30);}}static struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl_f(struct Cyc_Tcenv_Genv*
ge,struct _dyneither_ptr*v){struct Cyc_Dict_Dict _tmp3F=ge->aggrdecls;return((
struct Cyc_Absyn_Aggrdecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(
_tmp3F,v);}struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,struct _tuple1*q){struct _tuple1 _tmp41;union Cyc_Absyn_Nmspace
_tmp42;struct _dyneither_ptr*_tmp43;struct _tuple1*_tmp40=q;_tmp41=*_tmp40;_tmp42=
_tmp41.f1;_tmp43=_tmp41.f2;{union Cyc_Absyn_Nmspace _tmp44=_tmp42;int _tmp45;struct
Cyc_List_List*_tmp46;struct Cyc_List_List*_tmp47;struct Cyc_List_List*_tmp48;
struct Cyc_List_List _tmp49;struct _dyneither_ptr*_tmp4A;struct Cyc_List_List*_tmp4B;
_LL19: if((_tmp44.Loc_n).tag != 3)goto _LL1B;_tmp45=(int)(_tmp44.Loc_n).val;_LL1A:
goto _LL1C;_LL1B: if((_tmp44.Rel_n).tag != 1)goto _LL1D;_tmp46=(struct Cyc_List_List*)(
_tmp44.Rel_n).val;if(_tmp46 != 0)goto _LL1D;_LL1C: return((struct Cyc_Absyn_Aggrdecl**(*)(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Aggrdecl**(*
lookup)(struct Cyc_Tcenv_Genv*,struct _dyneither_ptr*),struct _dyneither_ptr*v))Cyc_Tcenv_scoped_lookup)(
te,loc,Cyc_Tcenv_lookup_aggrdecl_f,_tmp43);_LL1D: if((_tmp44.Abs_n).tag != 2)goto
_LL1F;_tmp47=(struct Cyc_List_List*)(_tmp44.Abs_n).val;_LL1E: {struct Cyc_Dict_Dict
_tmp4C=(((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))
Cyc_Dict_lookup)(te->ae,_tmp47))->aggrdecls;return((struct Cyc_Absyn_Aggrdecl**(*)(
struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp4C,_tmp43);}
_LL1F: if((_tmp44.Rel_n).tag != 1)goto _LL18;_tmp48=(struct Cyc_List_List*)(_tmp44.Rel_n).val;
if(_tmp48 == 0)goto _LL18;_tmp49=*_tmp48;_tmp4A=(struct _dyneither_ptr*)_tmp49.hd;
_tmp4B=_tmp49.tl;_LL20: {struct Cyc_Dict_Dict _tmp4D=(Cyc_Tcenv_lookup_namespace(
te,loc,_tmp4A,_tmp4B))->aggrdecls;return((struct Cyc_Absyn_Aggrdecl**(*)(struct
Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp4D,_tmp43);}_LL18:;}}
static struct Cyc_Absyn_Datatypedecl**Cyc_Tcenv_lookup_datatypedecl_f(struct Cyc_Tcenv_Genv*
ge,struct _dyneither_ptr*v){struct Cyc_Dict_Dict _tmp4E=ge->datatypedecls;return((
struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))
Cyc_Dict_lookup)(_tmp4E,v);}struct Cyc_Absyn_Datatypedecl**Cyc_Tcenv_lookup_datatypedecl(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct _tuple1*q){struct
_tuple1 _tmp50;union Cyc_Absyn_Nmspace _tmp51;struct _dyneither_ptr*_tmp52;struct
_tuple1*_tmp4F=q;_tmp50=*_tmp4F;_tmp51=_tmp50.f1;_tmp52=_tmp50.f2;{union Cyc_Absyn_Nmspace
_tmp53=_tmp51;int _tmp54;struct Cyc_List_List*_tmp55;struct Cyc_List_List*_tmp56;
struct Cyc_List_List _tmp57;struct _dyneither_ptr*_tmp58;struct Cyc_List_List*_tmp59;
struct Cyc_List_List*_tmp5A;_LL22: if((_tmp53.Loc_n).tag != 3)goto _LL24;_tmp54=(int)(
_tmp53.Loc_n).val;_LL23: goto _LL25;_LL24: if((_tmp53.Rel_n).tag != 1)goto _LL26;
_tmp55=(struct Cyc_List_List*)(_tmp53.Rel_n).val;if(_tmp55 != 0)goto _LL26;_LL25:
return((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,struct Cyc_Absyn_Datatypedecl**(*lookup)(struct Cyc_Tcenv_Genv*,struct
_dyneither_ptr*),struct _dyneither_ptr*v))Cyc_Tcenv_scoped_lookup)(te,loc,Cyc_Tcenv_lookup_datatypedecl_f,
_tmp52);_LL26: if((_tmp53.Rel_n).tag != 1)goto _LL28;_tmp56=(struct Cyc_List_List*)(
_tmp53.Rel_n).val;if(_tmp56 == 0)goto _LL28;_tmp57=*_tmp56;_tmp58=(struct
_dyneither_ptr*)_tmp57.hd;_tmp59=_tmp57.tl;_LL27: {struct Cyc_Dict_Dict _tmp5B=(
Cyc_Tcenv_lookup_namespace(te,loc,_tmp58,_tmp59))->datatypedecls;return((struct
Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(
_tmp5B,_tmp52);}_LL28: if((_tmp53.Abs_n).tag != 2)goto _LL21;_tmp5A=(struct Cyc_List_List*)(
_tmp53.Abs_n).val;_LL29: {struct Cyc_Dict_Dict _tmp5C=(((struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,_tmp5A))->datatypedecls;
return((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Dict_Dict d,struct
_dyneither_ptr*k))Cyc_Dict_lookup)(_tmp5C,_tmp52);}_LL21:;}}static struct Cyc_Absyn_Datatypedecl**
Cyc_Tcenv_lookup_xdatatypedecl_f(struct Cyc_Tcenv_Genv*ge,struct _dyneither_ptr*v){
return((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Dict_Dict d,struct
_dyneither_ptr*k))Cyc_Dict_lookup)(ge->datatypedecls,v);}struct Cyc_Absyn_Datatypedecl***
Cyc_Tcenv_lookup_xdatatypedecl(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,struct _tuple1*q){struct _tuple1 _tmp5E;union Cyc_Absyn_Nmspace
_tmp5F;struct _dyneither_ptr*_tmp60;struct _tuple1*_tmp5D=q;_tmp5E=*_tmp5D;_tmp5F=
_tmp5E.f1;_tmp60=_tmp5E.f2;{union Cyc_Absyn_Nmspace _tmp61=_tmp5F;struct Cyc_List_List*
_tmp62;int _tmp63;struct Cyc_List_List*_tmp64;struct Cyc_List_List _tmp65;struct
_dyneither_ptr*_tmp66;struct Cyc_List_List*_tmp67;struct Cyc_List_List*_tmp68;
_LL2B: if((_tmp61.Rel_n).tag != 1)goto _LL2D;_tmp62=(struct Cyc_List_List*)(_tmp61.Rel_n).val;
if(_tmp62 != 0)goto _LL2D;_LL2C: {struct _handler_cons _tmp69;_push_handler(& _tmp69);{
int _tmp6B=0;if(setjmp(_tmp69.handler))_tmp6B=1;if(!_tmp6B){{struct Cyc_Absyn_Datatypedecl***
_tmp6D=({struct Cyc_Absyn_Datatypedecl***_tmp6C=_region_malloc(r,sizeof(*_tmp6C));
_tmp6C[0]=((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,struct Cyc_Absyn_Datatypedecl**(*lookup)(struct Cyc_Tcenv_Genv*,struct
_dyneither_ptr*),struct _dyneither_ptr*v))Cyc_Tcenv_scoped_lookup)(te,loc,Cyc_Tcenv_lookup_xdatatypedecl_f,
_tmp60);_tmp6C;});_npop_handler(0);return _tmp6D;};_pop_handler();}else{void*
_tmp6A=(void*)_exn_thrown;void*_tmp6F=_tmp6A;_LL34: if(_tmp6F != Cyc_Dict_Absent)
goto _LL36;_LL35: return 0;_LL36:;_LL37:(void)_throw(_tmp6F);_LL33:;}}}_LL2D: if((
_tmp61.Loc_n).tag != 3)goto _LL2F;_tmp63=(int)(_tmp61.Loc_n).val;_LL2E:({void*
_tmp70=0;Cyc_Tcutil_terr(loc,({const char*_tmp71="lookup_xdatatypedecl: impossible";
_tag_dyneither(_tmp71,sizeof(char),33);}),_tag_dyneither(_tmp70,sizeof(void*),0));});
return 0;_LL2F: if((_tmp61.Rel_n).tag != 1)goto _LL31;_tmp64=(struct Cyc_List_List*)(
_tmp61.Rel_n).val;if(_tmp64 == 0)goto _LL31;_tmp65=*_tmp64;_tmp66=(struct
_dyneither_ptr*)_tmp65.hd;_tmp67=_tmp65.tl;_LL30: {struct Cyc_Tcenv_Genv*ge;{
struct _handler_cons _tmp72;_push_handler(& _tmp72);{int _tmp74=0;if(setjmp(_tmp72.handler))
_tmp74=1;if(!_tmp74){ge=Cyc_Tcenv_lookup_namespace(te,loc,_tmp66,_tmp67);;
_pop_handler();}else{void*_tmp73=(void*)_exn_thrown;void*_tmp76=_tmp73;_LL39: if(
_tmp76 != Cyc_Dict_Absent)goto _LL3B;_LL3A:({void*_tmp77=0;Cyc_Tcutil_terr(loc,({
const char*_tmp78="bad qualified name for @extensible datatype";_tag_dyneither(
_tmp78,sizeof(char),44);}),_tag_dyneither(_tmp77,sizeof(void*),0));});(int)
_throw((void*)Cyc_Dict_Absent);_LL3B:;_LL3C:(void)_throw(_tmp76);_LL38:;}}}{
struct Cyc_Dict_Dict _tmp79=ge->datatypedecls;return({struct Cyc_Absyn_Datatypedecl***
_tmp7A=_region_malloc(r,sizeof(*_tmp7A));_tmp7A[0]=((struct Cyc_Absyn_Datatypedecl**(*)(
struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp79,_tmp60);
_tmp7A;});}}_LL31: if((_tmp61.Abs_n).tag != 2)goto _LL2A;_tmp68=(struct Cyc_List_List*)(
_tmp61.Abs_n).val;_LL32: {struct Cyc_Dict_Dict _tmp7B=(((struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,_tmp68))->datatypedecls;
return({struct Cyc_Absyn_Datatypedecl***_tmp7C=_region_malloc(r,sizeof(*_tmp7C));
_tmp7C[0]=((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Dict_Dict d,struct
_dyneither_ptr*k))Cyc_Dict_lookup)(_tmp7B,_tmp60);_tmp7C;});}_LL2A:;}}static
struct Cyc_Absyn_Enumdecl**Cyc_Tcenv_lookup_enumdecl_f(struct Cyc_Tcenv_Genv*ge,
struct _dyneither_ptr*v){struct Cyc_Dict_Dict _tmp7D=ge->enumdecls;return((struct
Cyc_Absyn_Enumdecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(
_tmp7D,v);}struct Cyc_Absyn_Enumdecl**Cyc_Tcenv_lookup_enumdecl(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,struct _tuple1*q){struct _tuple1 _tmp7F;union Cyc_Absyn_Nmspace
_tmp80;struct _dyneither_ptr*_tmp81;struct _tuple1*_tmp7E=q;_tmp7F=*_tmp7E;_tmp80=
_tmp7F.f1;_tmp81=_tmp7F.f2;{union Cyc_Absyn_Nmspace _tmp82=_tmp80;int _tmp83;struct
Cyc_List_List*_tmp84;struct Cyc_List_List*_tmp85;struct Cyc_List_List _tmp86;struct
_dyneither_ptr*_tmp87;struct Cyc_List_List*_tmp88;struct Cyc_List_List*_tmp89;
_LL3E: if((_tmp82.Loc_n).tag != 3)goto _LL40;_tmp83=(int)(_tmp82.Loc_n).val;_LL3F:
goto _LL41;_LL40: if((_tmp82.Rel_n).tag != 1)goto _LL42;_tmp84=(struct Cyc_List_List*)(
_tmp82.Rel_n).val;if(_tmp84 != 0)goto _LL42;_LL41: return((struct Cyc_Absyn_Enumdecl**(*)(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Enumdecl**(*
lookup)(struct Cyc_Tcenv_Genv*,struct _dyneither_ptr*),struct _dyneither_ptr*v))Cyc_Tcenv_scoped_lookup)(
te,loc,Cyc_Tcenv_lookup_enumdecl_f,_tmp81);_LL42: if((_tmp82.Rel_n).tag != 1)goto
_LL44;_tmp85=(struct Cyc_List_List*)(_tmp82.Rel_n).val;if(_tmp85 == 0)goto _LL44;
_tmp86=*_tmp85;_tmp87=(struct _dyneither_ptr*)_tmp86.hd;_tmp88=_tmp86.tl;_LL43: {
struct Cyc_Dict_Dict _tmp8A=(Cyc_Tcenv_lookup_namespace(te,loc,_tmp87,_tmp88))->enumdecls;
return((struct Cyc_Absyn_Enumdecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*
k))Cyc_Dict_lookup)(_tmp8A,_tmp81);}_LL44: if((_tmp82.Abs_n).tag != 2)goto _LL3D;
_tmp89=(struct Cyc_List_List*)(_tmp82.Abs_n).val;_LL45: {struct Cyc_Dict_Dict
_tmp8B=(((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))
Cyc_Dict_lookup)(te->ae,_tmp89))->enumdecls;return((struct Cyc_Absyn_Enumdecl**(*)(
struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp8B,_tmp81);}
_LL3D:;}}static struct Cyc_Absyn_Typedefdecl*Cyc_Tcenv_lookup_typedefdecl_f(struct
Cyc_Tcenv_Genv*ge,struct _dyneither_ptr*v){struct Cyc_Dict_Dict _tmp8C=ge->typedefs;
return((struct Cyc_Absyn_Typedefdecl*(*)(struct Cyc_Dict_Dict d,struct
_dyneither_ptr*k))Cyc_Dict_lookup)(_tmp8C,v);}struct Cyc_Absyn_Typedefdecl*Cyc_Tcenv_lookup_typedefdecl(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct _tuple1*q){struct
_tuple1 _tmp8E;union Cyc_Absyn_Nmspace _tmp8F;struct _dyneither_ptr*_tmp90;struct
_tuple1*_tmp8D=q;_tmp8E=*_tmp8D;_tmp8F=_tmp8E.f1;_tmp90=_tmp8E.f2;{union Cyc_Absyn_Nmspace
_tmp91=_tmp8F;int _tmp92;struct Cyc_List_List*_tmp93;struct Cyc_List_List*_tmp94;
struct Cyc_List_List _tmp95;struct _dyneither_ptr*_tmp96;struct Cyc_List_List*_tmp97;
struct Cyc_List_List*_tmp98;_LL47: if((_tmp91.Loc_n).tag != 3)goto _LL49;_tmp92=(int)(
_tmp91.Loc_n).val;_LL48: goto _LL4A;_LL49: if((_tmp91.Rel_n).tag != 1)goto _LL4B;
_tmp93=(struct Cyc_List_List*)(_tmp91.Rel_n).val;if(_tmp93 != 0)goto _LL4B;_LL4A:
return((struct Cyc_Absyn_Typedefdecl*(*)(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,struct Cyc_Absyn_Typedefdecl*(*lookup)(struct Cyc_Tcenv_Genv*,struct
_dyneither_ptr*),struct _dyneither_ptr*v))Cyc_Tcenv_scoped_lookup)(te,loc,Cyc_Tcenv_lookup_typedefdecl_f,
_tmp90);_LL4B: if((_tmp91.Rel_n).tag != 1)goto _LL4D;_tmp94=(struct Cyc_List_List*)(
_tmp91.Rel_n).val;if(_tmp94 == 0)goto _LL4D;_tmp95=*_tmp94;_tmp96=(struct
_dyneither_ptr*)_tmp95.hd;_tmp97=_tmp95.tl;_LL4C: {struct Cyc_Dict_Dict _tmp99=(
Cyc_Tcenv_lookup_namespace(te,loc,_tmp96,_tmp97))->typedefs;return((struct Cyc_Absyn_Typedefdecl*(*)(
struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp99,_tmp90);}
_LL4D: if((_tmp91.Abs_n).tag != 2)goto _LL46;_tmp98=(struct Cyc_List_List*)(_tmp91.Abs_n).val;
_LL4E: {struct Cyc_Dict_Dict _tmp9A=(((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict
d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,_tmp98))->typedefs;return((
struct Cyc_Absyn_Typedefdecl*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(
_tmp9A,_tmp90);}_LL46:;}}static struct Cyc_Tcenv_Fenv*Cyc_Tcenv_get_fenv(struct Cyc_Tcenv_Tenv*
te,struct _dyneither_ptr err_msg){struct Cyc_Tcenv_Fenv*_tmp9B=te->le;if((struct Cyc_Tcenv_Tenv*)
te == 0)((int(*)(struct _dyneither_ptr msg))Cyc_Tcenv_env_err)(err_msg);return(
struct Cyc_Tcenv_Fenv*)_check_null(_tmp9B);}struct _RegionHandle*Cyc_Tcenv_coerce_heap_region(){
return(struct _RegionHandle*)Cyc_Core_heap_region;}struct _RegionHandle*Cyc_Tcenv_get_fnrgn(
struct Cyc_Tcenv_Tenv*te){struct Cyc_Tcenv_Fenv*_tmp9C=te->le;if(_tmp9C != 0){
struct Cyc_Tcenv_Fenv _tmp9E;struct _RegionHandle*_tmp9F;struct Cyc_Tcenv_Fenv*
_tmp9D=(struct Cyc_Tcenv_Fenv*)_tmp9C;_tmp9E=*_tmp9D;_tmp9F=_tmp9E.fnrgn;return
_tmp9F;}return Cyc_Tcenv_coerce_heap_region();}static struct Cyc_Tcenv_Tenv*Cyc_Tcenv_put_fenv(
struct _RegionHandle*l,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Fenv*fe){if(te->le
== 0)((int(*)(struct _dyneither_ptr msg))Cyc_Tcenv_env_err)(({const char*_tmpA0="put_fenv";
_tag_dyneither(_tmpA0,sizeof(char),9);}));return({struct Cyc_Tcenv_Tenv*_tmpA1=
_region_malloc(l,sizeof(*_tmpA1));_tmpA1->ns=te->ns;_tmpA1->ae=te->ae;_tmpA1->le=(
struct Cyc_Tcenv_Fenv*)fe;_tmpA1->allow_valueof=te->allow_valueof;_tmpA1;});}
static struct Cyc_Tcenv_Tenv*Cyc_Tcenv_put_emptyfenv(struct _RegionHandle*l,struct
Cyc_Tcenv_Tenv*te){return({struct Cyc_Tcenv_Tenv*_tmpA2=_region_malloc(l,sizeof(*
_tmpA2));_tmpA2->ns=te->ns;_tmpA2->ae=te->ae;_tmpA2->le=0;_tmpA2->allow_valueof=
te->allow_valueof;_tmpA2;});}static struct Cyc_Tcenv_Fenv*Cyc_Tcenv_copy_fenv_old_ctrl(
struct _RegionHandle*l,struct Cyc_Tcenv_Fenv*f){struct Cyc_Tcenv_SharedFenv*_tmpA4;
struct Cyc_List_List*_tmpA5;struct Cyc_RgnOrder_RgnPO*_tmpA6;struct Cyc_Tcenv_Bindings*
_tmpA7;struct Cyc_Absyn_Stmt*_tmpA8;struct Cyc_Tcenv_CtrlEnv*_tmpA9;void*_tmpAA;
void*_tmpAB;int _tmpAC;struct _RegionHandle*_tmpAD;struct Cyc_Tcenv_Fenv _tmpA3=*f;
_tmpA4=_tmpA3.shared;_tmpA5=_tmpA3.type_vars;_tmpA6=_tmpA3.region_order;_tmpA7=
_tmpA3.locals;_tmpA8=_tmpA3.encloser;_tmpA9=_tmpA3.ctrl_env;_tmpAA=_tmpA3.capability;
_tmpAB=_tmpA3.curr_rgn;_tmpAC=_tmpA3.in_notreadctxt;_tmpAD=_tmpA3.fnrgn;return({
struct Cyc_Tcenv_Fenv*_tmpAE=_region_malloc(l,sizeof(*_tmpAE));_tmpAE->shared=(
struct Cyc_Tcenv_SharedFenv*)_tmpA4;_tmpAE->type_vars=(struct Cyc_List_List*)
_tmpA5;_tmpAE->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmpA6;_tmpAE->locals=(
struct Cyc_Tcenv_Bindings*)((struct Cyc_Tcenv_Bindings*)_tmpA7);_tmpAE->encloser=(
struct Cyc_Absyn_Stmt*)_tmpA8;_tmpAE->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)((struct
Cyc_Tcenv_CtrlEnv*)_tmpA9);_tmpAE->capability=(void*)_tmpAA;_tmpAE->curr_rgn=(
void*)_tmpAB;_tmpAE->in_notreadctxt=(int)_tmpAC;_tmpAE->fnrgn=(struct
_RegionHandle*)l;_tmpAE;});}static struct Cyc_Tcenv_Fenv*Cyc_Tcenv_copy_fenv_new_ctrl(
struct _RegionHandle*l,struct Cyc_Tcenv_Fenv*f){struct Cyc_Tcenv_SharedFenv*_tmpB0;
struct Cyc_List_List*_tmpB1;struct Cyc_RgnOrder_RgnPO*_tmpB2;struct Cyc_Tcenv_Bindings*
_tmpB3;struct Cyc_Absyn_Stmt*_tmpB4;struct Cyc_Tcenv_CtrlEnv*_tmpB5;void*_tmpB6;
void*_tmpB7;int _tmpB8;struct Cyc_Tcenv_Fenv _tmpAF=*f;_tmpB0=_tmpAF.shared;_tmpB1=
_tmpAF.type_vars;_tmpB2=_tmpAF.region_order;_tmpB3=_tmpAF.locals;_tmpB4=_tmpAF.encloser;
_tmpB5=_tmpAF.ctrl_env;_tmpB6=_tmpAF.capability;_tmpB7=_tmpAF.curr_rgn;_tmpB8=
_tmpAF.in_notreadctxt;{struct _RegionHandle*_tmpBA;void*_tmpBB;void*_tmpBC;struct
_tuple10*_tmpBD;void*_tmpBE;int _tmpBF;struct Cyc_Tcenv_CtrlEnv _tmpB9=*_tmpB5;
_tmpBA=_tmpB9.ctrl_rgn;_tmpBB=_tmpB9.continue_stmt;_tmpBC=_tmpB9.break_stmt;
_tmpBD=_tmpB9.fallthru_clause;_tmpBE=_tmpB9.next_stmt;_tmpBF=_tmpB9.try_depth;{
struct Cyc_Tcenv_CtrlEnv*_tmpC0=({struct Cyc_Tcenv_CtrlEnv*_tmpC2=_region_malloc(l,
sizeof(*_tmpC2));_tmpC2->ctrl_rgn=_tmpBA;_tmpC2->continue_stmt=_tmpBB;_tmpC2->break_stmt=
_tmpBC;_tmpC2->fallthru_clause=_tmpBD;_tmpC2->next_stmt=_tmpBE;_tmpC2->try_depth=
_tmpBF;_tmpC2;});return({struct Cyc_Tcenv_Fenv*_tmpC1=_region_malloc(l,sizeof(*
_tmpC1));_tmpC1->shared=(struct Cyc_Tcenv_SharedFenv*)_tmpB0;_tmpC1->type_vars=(
struct Cyc_List_List*)_tmpB1;_tmpC1->region_order=(struct Cyc_RgnOrder_RgnPO*)
_tmpB2;_tmpC1->locals=(struct Cyc_Tcenv_Bindings*)((struct Cyc_Tcenv_Bindings*)
_tmpB3);_tmpC1->encloser=(struct Cyc_Absyn_Stmt*)_tmpB4;_tmpC1->ctrl_env=(struct
Cyc_Tcenv_CtrlEnv*)_tmpC0;_tmpC1->capability=(void*)_tmpB6;_tmpC1->curr_rgn=(
void*)_tmpB7;_tmpC1->in_notreadctxt=(int)_tmpB8;_tmpC1->fnrgn=(struct
_RegionHandle*)l;_tmpC1;});}}}void*Cyc_Tcenv_return_typ(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv _tmpC5;struct Cyc_Tcenv_SharedFenv*_tmpC6;struct Cyc_Tcenv_Fenv*
_tmpC4=Cyc_Tcenv_get_fenv(te,({const char*_tmpC3="return_typ";_tag_dyneither(
_tmpC3,sizeof(char),11);}));_tmpC5=*_tmpC4;_tmpC6=_tmpC5.shared;{void*_tmpC8;
struct Cyc_Tcenv_SharedFenv _tmpC7=*_tmpC6;_tmpC8=_tmpC7.return_typ;return _tmpC8;}}
struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*te){struct
Cyc_Tcenv_Fenv*_tmpC9=te->le;if(te->le == 0)return 0;{struct Cyc_List_List*_tmpCB;
struct Cyc_Tcenv_Fenv _tmpCA=*((struct Cyc_Tcenv_Fenv*)_check_null(_tmpC9));_tmpCB=
_tmpCA.type_vars;return _tmpCB;}}struct Cyc_Core_Opt*Cyc_Tcenv_lookup_opt_type_vars(
struct Cyc_Tcenv_Tenv*te){return({struct Cyc_Core_Opt*_tmpCC=_cycalloc(sizeof(*
_tmpCC));_tmpCC->v=Cyc_Tcenv_lookup_type_vars(te);_tmpCC;});}struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_add_type_vars(struct _RegionHandle*r,struct Cyc_Position_Segment*loc,
struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*tvs){struct Cyc_Tcenv_Fenv _tmpCF;
struct Cyc_Tcenv_SharedFenv*_tmpD0;struct Cyc_List_List*_tmpD1;struct Cyc_RgnOrder_RgnPO*
_tmpD2;struct Cyc_Tcenv_Bindings*_tmpD3;struct Cyc_Absyn_Stmt*_tmpD4;struct Cyc_Tcenv_CtrlEnv*
_tmpD5;void*_tmpD6;void*_tmpD7;int _tmpD8;struct Cyc_Tcenv_Fenv*_tmpCE=Cyc_Tcenv_get_fenv(
te,({const char*_tmpCD="add_type_vars";_tag_dyneither(_tmpCD,sizeof(char),14);}));
_tmpCF=*_tmpCE;_tmpD0=_tmpCF.shared;_tmpD1=_tmpCF.type_vars;_tmpD2=_tmpCF.region_order;
_tmpD3=_tmpCF.locals;_tmpD4=_tmpCF.encloser;_tmpD5=_tmpCF.ctrl_env;_tmpD6=_tmpCF.capability;
_tmpD7=_tmpCF.curr_rgn;_tmpD8=_tmpCF.in_notreadctxt;Cyc_Tcutil_add_tvar_identities(
tvs);{struct Cyc_List_List*_tmpD9=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_append)(tvs,_tmpD1);Cyc_Tcutil_check_unique_tvars(
loc,_tmpD9);{struct Cyc_Tcenv_Fenv*_tmpDA=({struct Cyc_Tcenv_Fenv*_tmpDB=
_region_malloc(r,sizeof(*_tmpDB));_tmpDB->shared=(struct Cyc_Tcenv_SharedFenv*)
_tmpD0;_tmpDB->type_vars=(struct Cyc_List_List*)_tmpD9;_tmpDB->region_order=(
struct Cyc_RgnOrder_RgnPO*)_tmpD2;_tmpDB->locals=(struct Cyc_Tcenv_Bindings*)((
struct Cyc_Tcenv_Bindings*)_tmpD3);_tmpDB->encloser=(struct Cyc_Absyn_Stmt*)_tmpD4;
_tmpDB->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)((struct Cyc_Tcenv_CtrlEnv*)_tmpD5);
_tmpDB->capability=(void*)_tmpD6;_tmpDB->curr_rgn=(void*)_tmpD7;_tmpDB->in_notreadctxt=(
int)_tmpD8;_tmpDB->fnrgn=(struct _RegionHandle*)r;_tmpDB;});return Cyc_Tcenv_put_fenv(
r,te,_tmpDA);}}}struct Cyc_Tcenv_Tenv*Cyc_Tcenv_copy_tenv(struct _RegionHandle*r,
struct Cyc_Tcenv_Tenv*te){struct Cyc_Tcenv_Fenv*_tmpDC=te->le;if(_tmpDC == 0)return
Cyc_Tcenv_put_emptyfenv(r,te);else{struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_copy_fenv_old_ctrl(
r,(struct Cyc_Tcenv_Fenv*)_tmpDC);return Cyc_Tcenv_put_fenv(r,te,fe);}}struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_add_local_var(struct _RegionHandle*r,struct Cyc_Position_Segment*loc,
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Vardecl*vd){struct _dyneither_ptr*_tmpDD=(*
vd->name).f2;{union Cyc_Absyn_Nmspace _tmpDE=(*vd->name).f1;int _tmpDF;_LL50: if((
_tmpDE.Loc_n).tag != 3)goto _LL52;_tmpDF=(int)(_tmpDE.Loc_n).val;_LL51: goto _LL4F;
_LL52:;_LL53:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmpE0=
_cycalloc(sizeof(*_tmpE0));_tmpE0[0]=({struct Cyc_Core_Impossible_struct _tmpE1;
_tmpE1.tag=Cyc_Core_Impossible;_tmpE1.f1=({const char*_tmpE2="add_local_var: called with Rel_n";
_tag_dyneither(_tmpE2,sizeof(char),33);});_tmpE1;});_tmpE0;}));_LL4F:;}{struct
Cyc_Tcenv_Fenv _tmpE5;struct Cyc_Tcenv_SharedFenv*_tmpE6;struct Cyc_List_List*
_tmpE7;struct Cyc_RgnOrder_RgnPO*_tmpE8;struct Cyc_Tcenv_Bindings*_tmpE9;struct Cyc_Absyn_Stmt*
_tmpEA;struct Cyc_Tcenv_CtrlEnv*_tmpEB;void*_tmpEC;void*_tmpED;int _tmpEE;struct
Cyc_Tcenv_Fenv*_tmpE4=Cyc_Tcenv_get_fenv(te,({const char*_tmpE3="add_local_var";
_tag_dyneither(_tmpE3,sizeof(char),14);}));_tmpE5=*_tmpE4;_tmpE6=_tmpE5.shared;
_tmpE7=_tmpE5.type_vars;_tmpE8=_tmpE5.region_order;_tmpE9=_tmpE5.locals;_tmpEA=
_tmpE5.encloser;_tmpEB=_tmpE5.ctrl_env;_tmpEC=_tmpE5.capability;_tmpED=_tmpE5.curr_rgn;
_tmpEE=_tmpE5.in_notreadctxt;{struct Cyc_Tcenv_Bindings*_tmpEF=({struct Cyc_Tcenv_Bindings*
_tmpF2=_region_malloc(r,sizeof(*_tmpF2));_tmpF2->v=_tmpDD;_tmpF2->b=(void*)({
struct Cyc_Absyn_Local_b_struct*_tmpF3=_cycalloc(sizeof(*_tmpF3));_tmpF3[0]=({
struct Cyc_Absyn_Local_b_struct _tmpF4;_tmpF4.tag=3;_tmpF4.f1=vd;_tmpF4;});_tmpF3;});
_tmpF2->tl=(struct Cyc_Tcenv_Bindings*)_tmpE9;_tmpF2;});struct Cyc_Tcenv_Fenv*
_tmpF0=({struct Cyc_Tcenv_Fenv*_tmpF1=_region_malloc(r,sizeof(*_tmpF1));_tmpF1->shared=(
struct Cyc_Tcenv_SharedFenv*)_tmpE6;_tmpF1->type_vars=(struct Cyc_List_List*)
_tmpE7;_tmpF1->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmpE8;_tmpF1->locals=(
struct Cyc_Tcenv_Bindings*)_tmpEF;_tmpF1->encloser=(struct Cyc_Absyn_Stmt*)_tmpEA;
_tmpF1->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)((struct Cyc_Tcenv_CtrlEnv*)_tmpEB);
_tmpF1->capability=(void*)_tmpEC;_tmpF1->curr_rgn=(void*)_tmpED;_tmpF1->in_notreadctxt=(
int)_tmpEE;_tmpF1->fnrgn=(struct _RegionHandle*)r;_tmpF1;});return Cyc_Tcenv_put_fenv(
r,te,_tmpF0);}}}struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_notreadctxt(struct
_RegionHandle*r,struct Cyc_Tcenv_Tenv*te){struct Cyc_Tcenv_Fenv*_tmpF5=te->le;if(
_tmpF5 == 0)return Cyc_Tcenv_put_emptyfenv(r,te);{struct Cyc_Tcenv_SharedFenv*
_tmpF7;struct Cyc_List_List*_tmpF8;struct Cyc_RgnOrder_RgnPO*_tmpF9;struct Cyc_Tcenv_Bindings*
_tmpFA;struct Cyc_Absyn_Stmt*_tmpFB;struct Cyc_Tcenv_CtrlEnv*_tmpFC;void*_tmpFD;
void*_tmpFE;int _tmpFF;struct Cyc_Tcenv_Fenv _tmpF6=*_tmpF5;_tmpF7=_tmpF6.shared;
_tmpF8=_tmpF6.type_vars;_tmpF9=_tmpF6.region_order;_tmpFA=_tmpF6.locals;_tmpFB=
_tmpF6.encloser;_tmpFC=_tmpF6.ctrl_env;_tmpFD=_tmpF6.capability;_tmpFE=_tmpF6.curr_rgn;
_tmpFF=_tmpF6.in_notreadctxt;{struct Cyc_Tcenv_Fenv*_tmp100=({struct Cyc_Tcenv_Fenv*
_tmp101=_region_malloc(r,sizeof(*_tmp101));_tmp101->shared=(struct Cyc_Tcenv_SharedFenv*)
_tmpF7;_tmp101->type_vars=(struct Cyc_List_List*)_tmpF8;_tmp101->region_order=(
struct Cyc_RgnOrder_RgnPO*)_tmpF9;_tmp101->locals=(struct Cyc_Tcenv_Bindings*)((
struct Cyc_Tcenv_Bindings*)_tmpFA);_tmp101->encloser=(struct Cyc_Absyn_Stmt*)
_tmpFB;_tmp101->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)((struct Cyc_Tcenv_CtrlEnv*)
_tmpFC);_tmp101->capability=(void*)_tmpFD;_tmp101->curr_rgn=(void*)_tmpFE;
_tmp101->in_notreadctxt=(int)1;_tmp101->fnrgn=(struct _RegionHandle*)r;_tmp101;});
return Cyc_Tcenv_put_fenv(r,te,_tmp100);}}}struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_notreadctxt(
struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){struct Cyc_Tcenv_Fenv*_tmp102=te->le;
if(_tmp102 == 0)return Cyc_Tcenv_put_emptyfenv(r,te);{struct Cyc_Tcenv_SharedFenv*
_tmp104;struct Cyc_List_List*_tmp105;struct Cyc_RgnOrder_RgnPO*_tmp106;struct Cyc_Tcenv_Bindings*
_tmp107;struct Cyc_Absyn_Stmt*_tmp108;struct Cyc_Tcenv_CtrlEnv*_tmp109;void*
_tmp10A;void*_tmp10B;int _tmp10C;struct Cyc_Tcenv_Fenv _tmp103=*_tmp102;_tmp104=
_tmp103.shared;_tmp105=_tmp103.type_vars;_tmp106=_tmp103.region_order;_tmp107=
_tmp103.locals;_tmp108=_tmp103.encloser;_tmp109=_tmp103.ctrl_env;_tmp10A=_tmp103.capability;
_tmp10B=_tmp103.curr_rgn;_tmp10C=_tmp103.in_notreadctxt;{struct Cyc_Tcenv_Fenv*
_tmp10D=({struct Cyc_Tcenv_Fenv*_tmp10E=_region_malloc(r,sizeof(*_tmp10E));
_tmp10E->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp104;_tmp10E->type_vars=(struct
Cyc_List_List*)_tmp105;_tmp10E->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp106;
_tmp10E->locals=(struct Cyc_Tcenv_Bindings*)((struct Cyc_Tcenv_Bindings*)_tmp107);
_tmp10E->encloser=(struct Cyc_Absyn_Stmt*)_tmp108;_tmp10E->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)((
struct Cyc_Tcenv_CtrlEnv*)_tmp109);_tmp10E->capability=(void*)_tmp10A;_tmp10E->curr_rgn=(
void*)_tmp10B;_tmp10E->in_notreadctxt=(int)0;_tmp10E->fnrgn=(struct _RegionHandle*)
r;_tmp10E;});return Cyc_Tcenv_put_fenv(r,te,_tmp10D);}}}int Cyc_Tcenv_in_notreadctxt(
struct Cyc_Tcenv_Tenv*te){struct Cyc_Tcenv_Fenv*_tmp10F=te->le;if(_tmp10F == 0)
return 0;{struct Cyc_Tcenv_Fenv _tmp111;int _tmp112;struct Cyc_Tcenv_Fenv*_tmp110=(
struct Cyc_Tcenv_Fenv*)_tmp10F;_tmp111=*_tmp110;_tmp112=_tmp111.in_notreadctxt;
return _tmp112;}}struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_pat_var(struct _RegionHandle*r,
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Vardecl*
vd){struct _dyneither_ptr*_tmp113=(*vd->name).f2;struct Cyc_Tcenv_Fenv _tmp116;
struct Cyc_Tcenv_SharedFenv*_tmp117;struct Cyc_List_List*_tmp118;struct Cyc_RgnOrder_RgnPO*
_tmp119;struct Cyc_Tcenv_Bindings*_tmp11A;struct Cyc_Absyn_Stmt*_tmp11B;struct Cyc_Tcenv_CtrlEnv*
_tmp11C;void*_tmp11D;void*_tmp11E;int _tmp11F;struct Cyc_Tcenv_Fenv*_tmp115=Cyc_Tcenv_get_fenv(
te,({const char*_tmp114="add_pat_var";_tag_dyneither(_tmp114,sizeof(char),12);}));
_tmp116=*_tmp115;_tmp117=_tmp116.shared;_tmp118=_tmp116.type_vars;_tmp119=
_tmp116.region_order;_tmp11A=_tmp116.locals;_tmp11B=_tmp116.encloser;_tmp11C=
_tmp116.ctrl_env;_tmp11D=_tmp116.capability;_tmp11E=_tmp116.curr_rgn;_tmp11F=
_tmp116.in_notreadctxt;{struct Cyc_Tcenv_Bindings*_tmp120=({struct Cyc_Tcenv_Bindings*
_tmp123=_region_malloc(r,sizeof(*_tmp123));_tmp123->v=_tmp113;_tmp123->b=(void*)({
struct Cyc_Absyn_Pat_b_struct*_tmp124=_cycalloc(sizeof(*_tmp124));_tmp124[0]=({
struct Cyc_Absyn_Pat_b_struct _tmp125;_tmp125.tag=4;_tmp125.f1=vd;_tmp125;});
_tmp124;});_tmp123->tl=(struct Cyc_Tcenv_Bindings*)_tmp11A;_tmp123;});struct Cyc_Tcenv_Fenv*
_tmp121=({struct Cyc_Tcenv_Fenv*_tmp122=_region_malloc(r,sizeof(*_tmp122));
_tmp122->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp117;_tmp122->type_vars=(struct
Cyc_List_List*)_tmp118;_tmp122->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp119;
_tmp122->locals=(struct Cyc_Tcenv_Bindings*)_tmp120;_tmp122->encloser=(struct Cyc_Absyn_Stmt*)
_tmp11B;_tmp122->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)((struct Cyc_Tcenv_CtrlEnv*)
_tmp11C);_tmp122->capability=(void*)_tmp11D;_tmp122->curr_rgn=(void*)_tmp11E;
_tmp122->in_notreadctxt=(int)_tmp11F;_tmp122->fnrgn=(struct _RegionHandle*)r;
_tmp122;});return Cyc_Tcenv_put_fenv(r,te,_tmp121);}}void*Cyc_Tcenv_lookup_ordinary(
struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,
struct _tuple1*q){struct Cyc_Tcenv_Fenv*_tmp126=te->le;struct _tuple1 _tmp128;union
Cyc_Absyn_Nmspace _tmp129;struct _dyneither_ptr*_tmp12A;struct _tuple1*_tmp127=q;
_tmp128=*_tmp127;_tmp129=_tmp128.f1;_tmp12A=_tmp128.f2;{union Cyc_Absyn_Nmspace
_tmp12B=_tmp129;int _tmp12C;struct Cyc_List_List*_tmp12D;_LL55: if((_tmp12B.Loc_n).tag
!= 3)goto _LL57;_tmp12C=(int)(_tmp12B.Loc_n).val;_LL56: if(_tmp126 == 0)(int)_throw((
void*)Cyc_Dict_Absent);goto _LL58;_LL57: if((_tmp12B.Rel_n).tag != 1)goto _LL59;
_tmp12D=(struct Cyc_List_List*)(_tmp12B.Rel_n).val;if(_tmp12D != 0)goto _LL59;if(!(
_tmp126 != 0))goto _LL59;_LL58: {struct Cyc_Tcenv_Fenv _tmp12F;struct Cyc_Tcenv_Bindings*
_tmp130;struct Cyc_Tcenv_Fenv*_tmp12E=(struct Cyc_Tcenv_Fenv*)_tmp126;_tmp12F=*
_tmp12E;_tmp130=_tmp12F.locals;{struct _handler_cons _tmp131;_push_handler(&
_tmp131);{int _tmp133=0;if(setjmp(_tmp131.handler))_tmp133=1;if(!_tmp133){{void*
_tmp136=(void*)({struct Cyc_Tcenv_VarRes_struct*_tmp134=_region_malloc(r,sizeof(*
_tmp134));_tmp134[0]=({struct Cyc_Tcenv_VarRes_struct _tmp135;_tmp135.tag=0;
_tmp135.f1=(void*)Cyc_Tcenv_lookup_binding(_tmp130,_tmp12A);_tmp135;});_tmp134;});
_npop_handler(0);return _tmp136;};_pop_handler();}else{void*_tmp132=(void*)
_exn_thrown;void*_tmp138=_tmp132;_LL5C:;_LL5D: return(void*)Cyc_Tcenv_lookup_ordinary_global(
te,loc,q);_LL5E:;_LL5F:(void)_throw(_tmp138);_LL5B:;}}}}_LL59:;_LL5A: {struct
_handler_cons _tmp139;_push_handler(& _tmp139);{int _tmp13B=0;if(setjmp(_tmp139.handler))
_tmp13B=1;if(!_tmp13B){{void*_tmp13C=(void*)Cyc_Tcenv_lookup_ordinary_global(te,
loc,q);_npop_handler(0);return _tmp13C;};_pop_handler();}else{void*_tmp13A=(void*)
_exn_thrown;void*_tmp13E=_tmp13A;_LL61: if(_tmp13E != Cyc_Dict_Absent)goto _LL63;
_LL62: return(void*)({struct Cyc_Tcenv_VarRes_struct*_tmp13F=_region_malloc(r,
sizeof(*_tmp13F));_tmp13F[0]=({struct Cyc_Tcenv_VarRes_struct _tmp140;_tmp140.tag=
0;_tmp140.f1=(void*)((void*)0);_tmp140;});_tmp13F;});_LL63:;_LL64:(void)_throw(
_tmp13E);_LL60:;}}}_LL54:;}}void Cyc_Tcenv_process_continue(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Stmt**sopt){struct Cyc_Tcenv_Fenv
_tmp143;struct Cyc_Tcenv_CtrlEnv*_tmp144;struct Cyc_Tcenv_Fenv*_tmp142=Cyc_Tcenv_get_fenv(
te,({const char*_tmp141="process_continue";_tag_dyneither(_tmp141,sizeof(char),17);}));
_tmp143=*_tmp142;_tmp144=_tmp143.ctrl_env;{void*_tmp145=_tmp144->continue_stmt;
struct Cyc_Absyn_Stmt*_tmp146;_LL66: if(_tmp145 <= (void*)3)goto _LL68;if(*((int*)
_tmp145)!= 0)goto _LL68;_tmp146=((struct Cyc_Tcenv_Stmt_j_struct*)_tmp145)->f1;
_LL67: _tmp146->non_local_preds=({struct Cyc_List_List*_tmp147=_cycalloc(sizeof(*
_tmp147));_tmp147->hd=s;_tmp147->tl=_tmp146->non_local_preds;_tmp147;});*sopt=(
struct Cyc_Absyn_Stmt*)_tmp146;return;_LL68: if((int)_tmp145 != 0)goto _LL6A;_LL69:({
void*_tmp148=0;Cyc_Tcutil_terr(s->loc,({const char*_tmp149="continue not in a loop";
_tag_dyneither(_tmp149,sizeof(char),23);}),_tag_dyneither(_tmp148,sizeof(void*),
0));});return;_LL6A: if((int)_tmp145 != 1)goto _LL6C;_LL6B: goto _LL6D;_LL6C: if((int)
_tmp145 != 2)goto _LL65;_LL6D:((int(*)(struct _dyneither_ptr msg))Cyc_Tcenv_env_err)(({
const char*_tmp14A="bad continue destination";_tag_dyneither(_tmp14A,sizeof(char),
25);}));_LL65:;}}void Cyc_Tcenv_process_break(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*
s,struct Cyc_Absyn_Stmt**sopt){struct Cyc_Tcenv_Fenv _tmp14D;struct Cyc_Tcenv_CtrlEnv*
_tmp14E;struct Cyc_Tcenv_SharedFenv*_tmp14F;struct Cyc_Tcenv_Fenv*_tmp14C=Cyc_Tcenv_get_fenv(
te,({const char*_tmp14B="process_break";_tag_dyneither(_tmp14B,sizeof(char),14);}));
_tmp14D=*_tmp14C;_tmp14E=_tmp14D.ctrl_env;_tmp14F=_tmp14D.shared;{void*_tmp150=
_tmp14E->break_stmt;struct Cyc_Absyn_Stmt*_tmp151;_LL6F: if(_tmp150 <= (void*)3)
goto _LL71;if(*((int*)_tmp150)!= 0)goto _LL71;_tmp151=((struct Cyc_Tcenv_Stmt_j_struct*)
_tmp150)->f1;_LL70: _tmp151->non_local_preds=({struct Cyc_List_List*_tmp152=
_cycalloc(sizeof(*_tmp152));_tmp152->hd=s;_tmp152->tl=_tmp151->non_local_preds;
_tmp152;});*sopt=(struct Cyc_Absyn_Stmt*)_tmp151;return;_LL71: if((int)_tmp150 != 0)
goto _LL73;_LL72:({void*_tmp153=0;Cyc_Tcutil_terr(s->loc,({const char*_tmp154="break not in a loop or switch";
_tag_dyneither(_tmp154,sizeof(char),30);}),_tag_dyneither(_tmp153,sizeof(void*),
0));});return;_LL73: if((int)_tmp150 != 2)goto _LL75;_LL74: if(_tmp14F->return_typ != (
void*)((void*)0))({void*_tmp155=0;Cyc_Tcutil_terr(s->loc,({const char*_tmp156="break causes function not to return a value";
_tag_dyneither(_tmp156,sizeof(char),44);}),_tag_dyneither(_tmp155,sizeof(void*),
0));});return;_LL75: if((int)_tmp150 != 1)goto _LL6E;_LL76:({void*_tmp157=0;Cyc_Tcutil_terr(
s->loc,({const char*_tmp158="break causes outer switch clause to implicitly fallthru";
_tag_dyneither(_tmp158,sizeof(char),56);}),_tag_dyneither(_tmp157,sizeof(void*),
0));});return;_LL6E:;}}void Cyc_Tcenv_process_goto(struct Cyc_Tcenv_Tenv*te,struct
Cyc_Absyn_Stmt*s,struct _dyneither_ptr*l,struct Cyc_Absyn_Stmt**sopt){struct Cyc_Tcenv_Fenv
_tmp15B;struct Cyc_Tcenv_SharedFenv*_tmp15C;struct Cyc_Tcenv_Fenv*_tmp15A=Cyc_Tcenv_get_fenv(
te,({const char*_tmp159="process_goto";_tag_dyneither(_tmp159,sizeof(char),13);}));
_tmp15B=*_tmp15A;_tmp15C=_tmp15B.shared;{struct Cyc_Absyn_Stmt**sopt2=((struct Cyc_Absyn_Stmt**(*)(
struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup_opt)(_tmp15C->seen_labels,
l);if(sopt2 == 0){struct Cyc_Dict_Dict _tmp15D=_tmp15C->needed_labels;struct Cyc_List_List**
slopt=((struct Cyc_List_List**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))
Cyc_Dict_lookup_opt)(_tmp15D,l);struct _RegionHandle*frgn=_tmp15C->frgn;if(slopt
== 0)slopt=({struct Cyc_List_List**_tmp15E=_region_malloc(frgn,sizeof(*_tmp15E));
_tmp15E[0]=0;_tmp15E;});{struct Cyc_List_List*new_needed=({struct Cyc_List_List*
_tmp15F=_cycalloc(sizeof(*_tmp15F));_tmp15F->hd=s;_tmp15F->tl=*((struct Cyc_List_List**)
_check_null(slopt));_tmp15F;});_tmp15C->needed_labels=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_List_List*v))Cyc_Dict_insert)(
_tmp15D,l,new_needed);}}else{struct Cyc_Absyn_Stmt*s=*sopt2;s->non_local_preds=({
struct Cyc_List_List*_tmp160=_cycalloc(sizeof(*_tmp160));_tmp160->hd=s;_tmp160->tl=
s->non_local_preds;_tmp160;});*sopt=(struct Cyc_Absyn_Stmt*)s;}}}struct _tuple10*
Cyc_Tcenv_process_fallthru(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,struct
Cyc_Absyn_Switch_clause***clauseopt){struct Cyc_Tcenv_Fenv _tmp163;struct Cyc_Tcenv_CtrlEnv*
_tmp164;struct Cyc_Tcenv_Fenv*_tmp162=Cyc_Tcenv_get_fenv(te,({const char*_tmp161="process_fallthru";
_tag_dyneither(_tmp161,sizeof(char),17);}));_tmp163=*_tmp162;_tmp164=_tmp163.ctrl_env;{
struct _tuple10*_tmp165=(struct _tuple10*)_tmp164->fallthru_clause;if(_tmp165 != 0){(((*
_tmp165).f1)->body)->non_local_preds=({struct Cyc_List_List*_tmp166=_cycalloc(
sizeof(*_tmp166));_tmp166->hd=s;_tmp166->tl=(((*_tmp165).f1)->body)->non_local_preds;
_tmp166;});*clauseopt=(struct Cyc_Absyn_Switch_clause**)({struct Cyc_Absyn_Switch_clause**
_tmp167=_cycalloc(sizeof(*_tmp167));_tmp167[0]=(*_tmp165).f1;_tmp167;});}return
_tmp165;}}struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_fallthru(struct _RegionHandle*r,
struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*new_tvs,struct Cyc_List_List*vds,
struct Cyc_Absyn_Switch_clause*clause){struct Cyc_Tcenv_Fenv _tmp16A;struct Cyc_Tcenv_SharedFenv*
_tmp16B;struct Cyc_List_List*_tmp16C;struct Cyc_RgnOrder_RgnPO*_tmp16D;struct Cyc_Tcenv_Bindings*
_tmp16E;struct Cyc_Absyn_Stmt*_tmp16F;struct Cyc_Tcenv_CtrlEnv*_tmp170;void*
_tmp171;void*_tmp172;int _tmp173;struct Cyc_Tcenv_Fenv*_tmp169=Cyc_Tcenv_get_fenv(
te,({const char*_tmp168="set_fallthru";_tag_dyneither(_tmp168,sizeof(char),13);}));
_tmp16A=*_tmp169;_tmp16B=_tmp16A.shared;_tmp16C=_tmp16A.type_vars;_tmp16D=
_tmp16A.region_order;_tmp16E=_tmp16A.locals;_tmp16F=_tmp16A.encloser;_tmp170=
_tmp16A.ctrl_env;_tmp171=_tmp16A.capability;_tmp172=_tmp16A.curr_rgn;_tmp173=
_tmp16A.in_notreadctxt;{struct Cyc_Tcenv_CtrlEnv _tmp175;struct _RegionHandle*
_tmp176;void*_tmp177;void*_tmp178;struct _tuple10*_tmp179;void*_tmp17A;int _tmp17B;
struct Cyc_Tcenv_CtrlEnv*_tmp174=_tmp170;_tmp175=*_tmp174;_tmp176=_tmp175.ctrl_rgn;
_tmp177=_tmp175.continue_stmt;_tmp178=_tmp175.break_stmt;_tmp179=_tmp175.fallthru_clause;
_tmp17A=_tmp175.next_stmt;_tmp17B=_tmp175.try_depth;{struct Cyc_List_List*ft_typ=
0;for(0;vds != 0;vds=vds->tl){ft_typ=({struct Cyc_List_List*_tmp17C=_region_malloc(
_tmp176,sizeof(*_tmp17C));_tmp17C->hd=(void*)((struct Cyc_Absyn_Vardecl*)vds->hd)->type;
_tmp17C->tl=ft_typ;_tmp17C;});}{struct Cyc_Tcenv_CList*_tmp17D=(struct Cyc_Tcenv_CList*)
Cyc_List_imp_rev(ft_typ);struct Cyc_Tcenv_CtrlEnv*_tmp17E=({struct Cyc_Tcenv_CtrlEnv*
_tmp181=_region_malloc(r,sizeof(*_tmp181));_tmp181->ctrl_rgn=_tmp176;_tmp181->continue_stmt=
_tmp177;_tmp181->break_stmt=_tmp178;_tmp181->fallthru_clause=({struct _tuple10*
_tmp182=_region_malloc(_tmp176,sizeof(*_tmp182));_tmp182->f1=clause;_tmp182->f2=
new_tvs;_tmp182->f3=_tmp17D;_tmp182;});_tmp181->next_stmt=_tmp17A;_tmp181->try_depth=
_tmp17B;_tmp181;});struct Cyc_Tcenv_Fenv*_tmp17F=({struct Cyc_Tcenv_Fenv*_tmp180=
_region_malloc(r,sizeof(*_tmp180));_tmp180->shared=(struct Cyc_Tcenv_SharedFenv*)
_tmp16B;_tmp180->type_vars=(struct Cyc_List_List*)_tmp16C;_tmp180->region_order=(
struct Cyc_RgnOrder_RgnPO*)_tmp16D;_tmp180->locals=(struct Cyc_Tcenv_Bindings*)((
struct Cyc_Tcenv_Bindings*)_tmp16E);_tmp180->encloser=(struct Cyc_Absyn_Stmt*)
_tmp16F;_tmp180->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp17E;_tmp180->capability=(
void*)_tmp171;_tmp180->curr_rgn=(void*)_tmp172;_tmp180->in_notreadctxt=(int)
_tmp173;_tmp180->fnrgn=(struct _RegionHandle*)r;_tmp180;});return Cyc_Tcenv_put_fenv(
r,te,_tmp17F);}}}}struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_fallthru(struct
_RegionHandle*r,struct Cyc_Tcenv_Tenv*te){struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_copy_fenv_new_ctrl(
r,Cyc_Tcenv_get_fenv(te,({const char*_tmp186="clear_fallthru";_tag_dyneither(
_tmp186,sizeof(char),15);})));struct Cyc_Tcenv_Fenv _tmp184;struct Cyc_Tcenv_CtrlEnv*
_tmp185;struct Cyc_Tcenv_Fenv*_tmp183=fe;_tmp184=*_tmp183;_tmp185=_tmp184.ctrl_env;
_tmp185->fallthru_clause=0;return Cyc_Tcenv_put_fenv(r,te,fe);}struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_set_in_loop(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*
continue_dest){struct Cyc_Tcenv_Fenv _tmp189;struct Cyc_Tcenv_SharedFenv*_tmp18A;
struct Cyc_List_List*_tmp18B;struct Cyc_RgnOrder_RgnPO*_tmp18C;struct Cyc_Tcenv_Bindings*
_tmp18D;struct Cyc_Absyn_Stmt*_tmp18E;struct Cyc_Tcenv_CtrlEnv*_tmp18F;void*
_tmp190;void*_tmp191;int _tmp192;struct Cyc_Tcenv_Fenv*_tmp188=Cyc_Tcenv_get_fenv(
te,({const char*_tmp187="set_in_loop";_tag_dyneither(_tmp187,sizeof(char),12);}));
_tmp189=*_tmp188;_tmp18A=_tmp189.shared;_tmp18B=_tmp189.type_vars;_tmp18C=
_tmp189.region_order;_tmp18D=_tmp189.locals;_tmp18E=_tmp189.encloser;_tmp18F=
_tmp189.ctrl_env;_tmp190=_tmp189.capability;_tmp191=_tmp189.curr_rgn;_tmp192=
_tmp189.in_notreadctxt;{struct Cyc_Tcenv_CtrlEnv*new_cenv=({struct Cyc_Tcenv_CtrlEnv*
_tmp194=_region_malloc(r,sizeof(*_tmp194));_tmp194->ctrl_rgn=r;_tmp194->continue_stmt=(
void*)({struct Cyc_Tcenv_Stmt_j_struct*_tmp197=_region_malloc(r,sizeof(*_tmp197));
_tmp197[0]=({struct Cyc_Tcenv_Stmt_j_struct _tmp198;_tmp198.tag=0;_tmp198.f1=
continue_dest;_tmp198;});_tmp197;});_tmp194->break_stmt=(void*)_tmp18F->next_stmt;
_tmp194->next_stmt=(void*)({struct Cyc_Tcenv_Stmt_j_struct*_tmp195=_region_malloc(
r,sizeof(*_tmp195));_tmp195[0]=({struct Cyc_Tcenv_Stmt_j_struct _tmp196;_tmp196.tag=
0;_tmp196.f1=continue_dest;_tmp196;});_tmp195;});_tmp194->fallthru_clause=(
struct _tuple10*)_tmp18F->fallthru_clause;_tmp194->try_depth=_tmp18F->try_depth;
_tmp194;});struct Cyc_Tcenv_Fenv*new_fenv=({struct Cyc_Tcenv_Fenv*_tmp193=
_region_malloc(r,sizeof(*_tmp193));_tmp193->shared=(struct Cyc_Tcenv_SharedFenv*)
_tmp18A;_tmp193->type_vars=(struct Cyc_List_List*)_tmp18B;_tmp193->region_order=(
struct Cyc_RgnOrder_RgnPO*)_tmp18C;_tmp193->locals=(struct Cyc_Tcenv_Bindings*)((
struct Cyc_Tcenv_Bindings*)_tmp18D);_tmp193->encloser=(struct Cyc_Absyn_Stmt*)
_tmp18E;_tmp193->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)new_cenv;_tmp193->capability=(
void*)_tmp190;_tmp193->curr_rgn=(void*)_tmp191;_tmp193->in_notreadctxt=(int)
_tmp192;_tmp193->fnrgn=(struct _RegionHandle*)r;_tmp193;});return Cyc_Tcenv_put_fenv(
r,te,new_fenv);}}struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_try(struct _RegionHandle*r,
struct Cyc_Tcenv_Tenv*te){struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_copy_fenv_new_ctrl(r,
Cyc_Tcenv_get_fenv(te,({const char*_tmp19C="enter_try";_tag_dyneither(_tmp19C,
sizeof(char),10);})));struct Cyc_Tcenv_Fenv _tmp19A;struct Cyc_Tcenv_CtrlEnv*
_tmp19B;struct Cyc_Tcenv_Fenv*_tmp199=fe;_tmp19A=*_tmp199;_tmp19B=_tmp19A.ctrl_env;
++ _tmp19B->try_depth;return Cyc_Tcenv_put_fenv(r,te,fe);}int Cyc_Tcenv_get_try_depth(
struct Cyc_Tcenv_Tenv*te){struct Cyc_Tcenv_Fenv _tmp19F;struct Cyc_Tcenv_CtrlEnv*
_tmp1A0;struct Cyc_Tcenv_Fenv*_tmp19E=Cyc_Tcenv_get_fenv(te,({const char*_tmp19D="get_try_depth";
_tag_dyneither(_tmp19D,sizeof(char),14);}));_tmp19F=*_tmp19E;_tmp1A0=_tmp19F.ctrl_env;
return _tmp1A0->try_depth;}struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_in_switch(struct
_RegionHandle*r,struct Cyc_Tcenv_Tenv*te){struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_copy_fenv_new_ctrl(
r,Cyc_Tcenv_get_fenv(te,({const char*_tmp1A4="set_in_switch";_tag_dyneither(
_tmp1A4,sizeof(char),14);})));struct Cyc_Tcenv_Fenv _tmp1A2;struct Cyc_Tcenv_CtrlEnv*
_tmp1A3;struct Cyc_Tcenv_Fenv*_tmp1A1=fe;_tmp1A2=*_tmp1A1;_tmp1A3=_tmp1A2.ctrl_env;
_tmp1A3->break_stmt=_tmp1A3->next_stmt;_tmp1A3->next_stmt=(void*)1;return Cyc_Tcenv_put_fenv(
r,te,fe);}struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_next(struct _RegionHandle*r,struct
Cyc_Tcenv_Tenv*te,void*j){struct Cyc_Tcenv_SharedFenv*_tmp1A7;struct Cyc_List_List*
_tmp1A8;struct Cyc_RgnOrder_RgnPO*_tmp1A9;struct Cyc_Tcenv_Bindings*_tmp1AA;struct
Cyc_Absyn_Stmt*_tmp1AB;struct Cyc_Tcenv_CtrlEnv*_tmp1AC;void*_tmp1AD;void*_tmp1AE;
int _tmp1AF;struct Cyc_Tcenv_Fenv _tmp1A6=*Cyc_Tcenv_get_fenv(te,({const char*
_tmp1A5="set_next";_tag_dyneither(_tmp1A5,sizeof(char),9);}));_tmp1A7=_tmp1A6.shared;
_tmp1A8=_tmp1A6.type_vars;_tmp1A9=_tmp1A6.region_order;_tmp1AA=_tmp1A6.locals;
_tmp1AB=_tmp1A6.encloser;_tmp1AC=_tmp1A6.ctrl_env;_tmp1AD=_tmp1A6.capability;
_tmp1AE=_tmp1A6.curr_rgn;_tmp1AF=_tmp1A6.in_notreadctxt;{struct Cyc_Tcenv_CtrlEnv*
new_cenv=({struct Cyc_Tcenv_CtrlEnv*_tmp1B1=_region_malloc(r,sizeof(*_tmp1B1));
_tmp1B1->ctrl_rgn=r;_tmp1B1->continue_stmt=(void*)_tmp1AC->continue_stmt;_tmp1B1->break_stmt=(
void*)_tmp1AC->break_stmt;_tmp1B1->next_stmt=j;_tmp1B1->fallthru_clause=(struct
_tuple10*)_tmp1AC->fallthru_clause;_tmp1B1->try_depth=_tmp1AC->try_depth;_tmp1B1;});
struct Cyc_Tcenv_Fenv*new_fenv=({struct Cyc_Tcenv_Fenv*_tmp1B0=_region_malloc(r,
sizeof(*_tmp1B0));_tmp1B0->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp1A7;_tmp1B0->type_vars=(
struct Cyc_List_List*)_tmp1A8;_tmp1B0->region_order=(struct Cyc_RgnOrder_RgnPO*)
_tmp1A9;_tmp1B0->locals=(struct Cyc_Tcenv_Bindings*)((struct Cyc_Tcenv_Bindings*)
_tmp1AA);_tmp1B0->encloser=(struct Cyc_Absyn_Stmt*)_tmp1AB;_tmp1B0->ctrl_env=(
struct Cyc_Tcenv_CtrlEnv*)new_cenv;_tmp1B0->capability=(void*)_tmp1AD;_tmp1B0->curr_rgn=(
void*)_tmp1AE;_tmp1B0->in_notreadctxt=(int)_tmp1AF;_tmp1B0->fnrgn=(struct
_RegionHandle*)r;_tmp1B0;});return Cyc_Tcenv_put_fenv(r,te,new_fenv);}}struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_add_label(struct Cyc_Tcenv_Tenv*te,struct _dyneither_ptr*v,struct Cyc_Absyn_Stmt*
s){struct Cyc_Tcenv_Fenv _tmp1B4;struct Cyc_Tcenv_SharedFenv*_tmp1B5;struct Cyc_Tcenv_Fenv*
_tmp1B3=Cyc_Tcenv_get_fenv(te,({const char*_tmp1B2="add_label";_tag_dyneither(
_tmp1B2,sizeof(char),10);}));_tmp1B4=*_tmp1B3;_tmp1B5=_tmp1B4.shared;{struct Cyc_Dict_Dict
needed=_tmp1B5->needed_labels;struct Cyc_List_List**sl_opt=((struct Cyc_List_List**(*)(
struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup_opt)(needed,v);
struct _RegionHandle*frgn=_tmp1B5->frgn;if(sl_opt != 0){_tmp1B5->needed_labels=((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict,struct
_dyneither_ptr*))Cyc_Dict_rdelete)(frgn,needed,v);{struct Cyc_List_List*_tmp1B6=*
sl_opt;s->non_local_preds=_tmp1B6;for(0;_tmp1B6 != 0;_tmp1B6=_tmp1B6->tl){void*
_tmp1B7=((struct Cyc_Absyn_Stmt*)_tmp1B6->hd)->r;struct Cyc_Absyn_Stmt*_tmp1B8;
struct Cyc_Absyn_Stmt**_tmp1B9;_LL78: if(_tmp1B7 <= (void*)1)goto _LL7A;if(*((int*)
_tmp1B7)!= 7)goto _LL7A;_tmp1B8=((struct Cyc_Absyn_Goto_s_struct*)_tmp1B7)->f2;
_tmp1B9=(struct Cyc_Absyn_Stmt**)&((struct Cyc_Absyn_Goto_s_struct*)_tmp1B7)->f2;
_LL79:*_tmp1B9=(struct Cyc_Absyn_Stmt*)s;goto _LL77;_LL7A:;_LL7B:((int(*)(struct
_dyneither_ptr msg))Cyc_Tcenv_env_err)(({const char*_tmp1BA="Tcenv: add_label backpatching of non-goto";
_tag_dyneither(_tmp1BA,sizeof(char),42);}));goto _LL77;_LL77:;}}}if(((int(*)(
struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_member)(_tmp1B5->seen_labels,
v))({struct Cyc_String_pa_struct _tmp1BD;_tmp1BD.tag=0;_tmp1BD.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*v);{void*_tmp1BB[1]={& _tmp1BD};Cyc_Tcutil_terr(
s->loc,({const char*_tmp1BC="Repeated label: %s";_tag_dyneither(_tmp1BC,sizeof(
char),19);}),_tag_dyneither(_tmp1BB,sizeof(void*),1));}});_tmp1B5->seen_labels=((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Stmt*
v))Cyc_Dict_insert)(_tmp1B5->seen_labels,v,s);return te;}}int Cyc_Tcenv_all_labels_resolved(
struct Cyc_Tcenv_Tenv*te){struct Cyc_Tcenv_Fenv _tmp1C0;struct Cyc_Tcenv_SharedFenv*
_tmp1C1;struct Cyc_Tcenv_Fenv*_tmp1BF=Cyc_Tcenv_get_fenv(te,({const char*_tmp1BE="all_labels_resolved";
_tag_dyneither(_tmp1BE,sizeof(char),20);}));_tmp1C0=*_tmp1BF;_tmp1C1=_tmp1C0.shared;
return((int(*)(struct Cyc_Dict_Dict d))Cyc_Dict_is_empty)(_tmp1C1->needed_labels);}
struct Cyc_Absyn_Stmt*Cyc_Tcenv_get_encloser(struct Cyc_Tcenv_Tenv*te){struct Cyc_Tcenv_Fenv
_tmp1C4;struct Cyc_Absyn_Stmt*_tmp1C5;struct Cyc_Tcenv_Fenv*_tmp1C3=Cyc_Tcenv_get_fenv(
te,({const char*_tmp1C2="get_encloser";_tag_dyneither(_tmp1C2,sizeof(char),13);}));
_tmp1C4=*_tmp1C3;_tmp1C5=_tmp1C4.encloser;return _tmp1C5;}struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_allow_valueof(struct _RegionHandle*r2,struct Cyc_Tcenv_Tenv*te){if(te->le
== 0)return({struct Cyc_Tcenv_Tenv*_tmp1C6=_region_malloc(r2,sizeof(*_tmp1C6));
_tmp1C6->ns=te->ns;_tmp1C6->ae=te->ae;_tmp1C6->le=0;_tmp1C6->allow_valueof=1;
_tmp1C6;});{struct Cyc_Tcenv_SharedFenv*_tmp1C8;struct Cyc_List_List*_tmp1C9;
struct Cyc_RgnOrder_RgnPO*_tmp1CA;struct Cyc_Tcenv_Bindings*_tmp1CB;struct Cyc_Absyn_Stmt*
_tmp1CC;struct Cyc_Tcenv_CtrlEnv*_tmp1CD;void*_tmp1CE;void*_tmp1CF;int _tmp1D0;
struct Cyc_Tcenv_Fenv _tmp1C7=*((struct Cyc_Tcenv_Fenv*)_check_null(te->le));
_tmp1C8=_tmp1C7.shared;_tmp1C9=_tmp1C7.type_vars;_tmp1CA=_tmp1C7.region_order;
_tmp1CB=_tmp1C7.locals;_tmp1CC=_tmp1C7.encloser;_tmp1CD=_tmp1C7.ctrl_env;_tmp1CE=
_tmp1C7.capability;_tmp1CF=_tmp1C7.curr_rgn;_tmp1D0=_tmp1C7.in_notreadctxt;{
struct Cyc_Tcenv_Fenv*_tmp1D1=({struct Cyc_Tcenv_Fenv*_tmp1D3=_region_malloc(r2,
sizeof(*_tmp1D3));_tmp1D3->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp1C8;_tmp1D3->type_vars=(
struct Cyc_List_List*)_tmp1C9;_tmp1D3->region_order=(struct Cyc_RgnOrder_RgnPO*)
_tmp1CA;_tmp1D3->locals=(struct Cyc_Tcenv_Bindings*)((struct Cyc_Tcenv_Bindings*)
_tmp1CB);_tmp1D3->encloser=(struct Cyc_Absyn_Stmt*)_tmp1CC;_tmp1D3->ctrl_env=(
struct Cyc_Tcenv_CtrlEnv*)((struct Cyc_Tcenv_CtrlEnv*)_tmp1CD);_tmp1D3->capability=(
void*)_tmp1CE;_tmp1D3->curr_rgn=(void*)_tmp1CF;_tmp1D3->in_notreadctxt=(int)
_tmp1D0;_tmp1D3->fnrgn=(struct _RegionHandle*)r2;_tmp1D3;});return({struct Cyc_Tcenv_Tenv*
_tmp1D2=_region_malloc(r2,sizeof(*_tmp1D2));_tmp1D2->ns=te->ns;_tmp1D2->ae=te->ae;
_tmp1D2->le=_tmp1D1;_tmp1D2->allow_valueof=1;_tmp1D2;});}}}struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_set_encloser(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*
s){struct Cyc_Tcenv_SharedFenv*_tmp1D6;struct Cyc_List_List*_tmp1D7;struct Cyc_RgnOrder_RgnPO*
_tmp1D8;struct Cyc_Tcenv_Bindings*_tmp1D9;struct Cyc_Absyn_Stmt*_tmp1DA;struct Cyc_Tcenv_CtrlEnv*
_tmp1DB;void*_tmp1DC;void*_tmp1DD;int _tmp1DE;struct Cyc_Tcenv_Fenv _tmp1D5=*Cyc_Tcenv_get_fenv(
te,({const char*_tmp1D4="set_encloser";_tag_dyneither(_tmp1D4,sizeof(char),13);}));
_tmp1D6=_tmp1D5.shared;_tmp1D7=_tmp1D5.type_vars;_tmp1D8=_tmp1D5.region_order;
_tmp1D9=_tmp1D5.locals;_tmp1DA=_tmp1D5.encloser;_tmp1DB=_tmp1D5.ctrl_env;_tmp1DC=
_tmp1D5.capability;_tmp1DD=_tmp1D5.curr_rgn;_tmp1DE=_tmp1D5.in_notreadctxt;{
struct Cyc_Tcenv_Fenv*_tmp1DF=({struct Cyc_Tcenv_Fenv*_tmp1E0=_region_malloc(r,
sizeof(*_tmp1E0));_tmp1E0->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp1D6;_tmp1E0->type_vars=(
struct Cyc_List_List*)_tmp1D7;_tmp1E0->region_order=(struct Cyc_RgnOrder_RgnPO*)
_tmp1D8;_tmp1E0->locals=(struct Cyc_Tcenv_Bindings*)((struct Cyc_Tcenv_Bindings*)
_tmp1D9);_tmp1E0->encloser=(struct Cyc_Absyn_Stmt*)s;_tmp1E0->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)((
struct Cyc_Tcenv_CtrlEnv*)_tmp1DB);_tmp1E0->capability=(void*)_tmp1DC;_tmp1E0->curr_rgn=(
void*)_tmp1DD;_tmp1E0->in_notreadctxt=(int)_tmp1DE;_tmp1E0->fnrgn=(struct
_RegionHandle*)r;_tmp1E0;});return Cyc_Tcenv_put_fenv(r,te,_tmp1DF);}}struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_add_region(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,void*rgn,int
resetable,int opened){struct Cyc_Absyn_Tvar*tv;{void*_tmp1E1=Cyc_Tcutil_compress(
rgn);struct Cyc_Absyn_Tvar*_tmp1E2;_LL7D: if(_tmp1E1 <= (void*)4)goto _LL7F;if(*((
int*)_tmp1E1)!= 1)goto _LL7F;_tmp1E2=((struct Cyc_Absyn_VarType_struct*)_tmp1E1)->f1;
_LL7E: tv=_tmp1E2;goto _LL7C;_LL7F:;_LL80: tv=((struct Cyc_Absyn_Tvar*(*)(struct
_dyneither_ptr msg))Cyc_Tcenv_env_err)(({const char*_tmp1E3="bad add region";
_tag_dyneither(_tmp1E3,sizeof(char),15);}));goto _LL7C;_LL7C:;}{struct Cyc_Tcenv_SharedFenv*
_tmp1E6;struct Cyc_List_List*_tmp1E7;struct Cyc_RgnOrder_RgnPO*_tmp1E8;struct Cyc_Tcenv_Bindings*
_tmp1E9;struct Cyc_Absyn_Stmt*_tmp1EA;struct Cyc_Tcenv_CtrlEnv*_tmp1EB;void*
_tmp1EC;void*_tmp1ED;int _tmp1EE;struct Cyc_Tcenv_Fenv _tmp1E5=*Cyc_Tcenv_get_fenv(
te,({const char*_tmp1E4="add_region";_tag_dyneither(_tmp1E4,sizeof(char),11);}));
_tmp1E6=_tmp1E5.shared;_tmp1E7=_tmp1E5.type_vars;_tmp1E8=_tmp1E5.region_order;
_tmp1E9=_tmp1E5.locals;_tmp1EA=_tmp1E5.encloser;_tmp1EB=_tmp1E5.ctrl_env;_tmp1EC=
_tmp1E5.capability;_tmp1ED=_tmp1E5.curr_rgn;_tmp1EE=_tmp1E5.in_notreadctxt;
_tmp1E8=Cyc_RgnOrder_add_youngest(_tmp1E6->frgn,_tmp1E8,tv,resetable,opened);
_tmp1EC=(void*)({struct Cyc_Absyn_JoinEff_struct*_tmp1EF=_cycalloc(sizeof(*
_tmp1EF));_tmp1EF[0]=({struct Cyc_Absyn_JoinEff_struct _tmp1F0;_tmp1F0.tag=20;
_tmp1F0.f1=({struct Cyc_List_List*_tmp1F1=_cycalloc(sizeof(*_tmp1F1));_tmp1F1->hd=(
void*)((void*)({struct Cyc_Absyn_AccessEff_struct*_tmp1F3=_cycalloc(sizeof(*
_tmp1F3));_tmp1F3[0]=({struct Cyc_Absyn_AccessEff_struct _tmp1F4;_tmp1F4.tag=19;
_tmp1F4.f1=(void*)rgn;_tmp1F4;});_tmp1F3;}));_tmp1F1->tl=({struct Cyc_List_List*
_tmp1F2=_cycalloc(sizeof(*_tmp1F2));_tmp1F2->hd=(void*)_tmp1EC;_tmp1F2->tl=0;
_tmp1F2;});_tmp1F1;});_tmp1F0;});_tmp1EF;});{struct Cyc_Tcenv_Fenv*_tmp1F5=({
struct Cyc_Tcenv_Fenv*_tmp1F6=_region_malloc(r,sizeof(*_tmp1F6));_tmp1F6->shared=(
struct Cyc_Tcenv_SharedFenv*)_tmp1E6;_tmp1F6->type_vars=(struct Cyc_List_List*)
_tmp1E7;_tmp1F6->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp1E8;_tmp1F6->locals=(
struct Cyc_Tcenv_Bindings*)((struct Cyc_Tcenv_Bindings*)_tmp1E9);_tmp1F6->encloser=(
struct Cyc_Absyn_Stmt*)_tmp1EA;_tmp1F6->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)((
struct Cyc_Tcenv_CtrlEnv*)_tmp1EB);_tmp1F6->capability=(void*)_tmp1EC;_tmp1F6->curr_rgn=(
void*)_tmp1ED;_tmp1F6->in_notreadctxt=(int)_tmp1EE;_tmp1F6->fnrgn=(struct
_RegionHandle*)r;_tmp1F6;});return Cyc_Tcenv_put_fenv(r,te,_tmp1F5);}}}struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_new_named_block(struct _RegionHandle*r,struct Cyc_Position_Segment*loc,
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Tvar*block_rgn){struct Cyc_Tcenv_SharedFenv*
_tmp1F9;struct Cyc_List_List*_tmp1FA;struct Cyc_RgnOrder_RgnPO*_tmp1FB;struct Cyc_Tcenv_Bindings*
_tmp1FC;struct Cyc_Absyn_Stmt*_tmp1FD;struct Cyc_Tcenv_CtrlEnv*_tmp1FE;void*
_tmp1FF;void*_tmp200;int _tmp201;struct Cyc_Tcenv_Fenv _tmp1F8=*Cyc_Tcenv_get_fenv(
te,({const char*_tmp1F7="new_named_block";_tag_dyneither(_tmp1F7,sizeof(char),16);}));
_tmp1F9=_tmp1F8.shared;_tmp1FA=_tmp1F8.type_vars;_tmp1FB=_tmp1F8.region_order;
_tmp1FC=_tmp1F8.locals;_tmp1FD=_tmp1F8.encloser;_tmp1FE=_tmp1F8.ctrl_env;_tmp1FF=
_tmp1F8.capability;_tmp200=_tmp1F8.curr_rgn;_tmp201=_tmp1F8.in_notreadctxt;{
struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_copy_fenv_old_ctrl(r,Cyc_Tcenv_get_fenv(te,({
const char*_tmp20D="new_block";_tag_dyneither(_tmp20D,sizeof(char),10);})));void*
block_typ=(void*)({struct Cyc_Absyn_VarType_struct*_tmp20B=_cycalloc(sizeof(*
_tmp20B));_tmp20B[0]=({struct Cyc_Absyn_VarType_struct _tmp20C;_tmp20C.tag=1;
_tmp20C.f1=block_rgn;_tmp20C;});_tmp20B;});_tmp1FA=({struct Cyc_List_List*_tmp202=
_cycalloc(sizeof(*_tmp202));_tmp202->hd=block_rgn;_tmp202->tl=_tmp1FA;_tmp202;});
Cyc_Tcutil_check_unique_tvars(loc,_tmp1FA);_tmp1FB=Cyc_RgnOrder_add_youngest(
_tmp1F9->frgn,_tmp1FB,block_rgn,0,0);_tmp1FF=(void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp203=_cycalloc(sizeof(*_tmp203));_tmp203[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp204;_tmp204.tag=20;_tmp204.f1=({struct Cyc_List_List*_tmp205=_cycalloc(
sizeof(*_tmp205));_tmp205->hd=(void*)((void*)({struct Cyc_Absyn_AccessEff_struct*
_tmp207=_cycalloc(sizeof(*_tmp207));_tmp207[0]=({struct Cyc_Absyn_AccessEff_struct
_tmp208;_tmp208.tag=19;_tmp208.f1=(void*)block_typ;_tmp208;});_tmp207;}));
_tmp205->tl=({struct Cyc_List_List*_tmp206=_cycalloc(sizeof(*_tmp206));_tmp206->hd=(
void*)_tmp1FF;_tmp206->tl=0;_tmp206;});_tmp205;});_tmp204;});_tmp203;});_tmp200=
block_typ;{struct Cyc_Tcenv_Fenv*_tmp209=({struct Cyc_Tcenv_Fenv*_tmp20A=
_region_malloc(r,sizeof(*_tmp20A));_tmp20A->shared=(struct Cyc_Tcenv_SharedFenv*)
_tmp1F9;_tmp20A->type_vars=(struct Cyc_List_List*)_tmp1FA;_tmp20A->region_order=(
struct Cyc_RgnOrder_RgnPO*)_tmp1FB;_tmp20A->locals=(struct Cyc_Tcenv_Bindings*)((
struct Cyc_Tcenv_Bindings*)_tmp1FC);_tmp20A->encloser=(struct Cyc_Absyn_Stmt*)
_tmp1FD;_tmp20A->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)((struct Cyc_Tcenv_CtrlEnv*)
_tmp1FE);_tmp20A->capability=(void*)_tmp1FF;_tmp20A->curr_rgn=(void*)_tmp200;
_tmp20A->in_notreadctxt=(int)_tmp201;_tmp20A->fnrgn=(struct _RegionHandle*)r;
_tmp20A;});return Cyc_Tcenv_put_fenv(r,te,_tmp209);}}}static struct Cyc_Absyn_Eq_kb_struct
Cyc_Tcenv_rgn_kb={0,(void*)((void*)3)};struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_block(
struct _RegionHandle*r,struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te){
struct Cyc_Absyn_Tvar*t=Cyc_Tcutil_new_tvar((void*)& Cyc_Tcenv_rgn_kb);Cyc_Tcutil_add_tvar_identity(
t);return Cyc_Tcenv_new_named_block(r,loc,te,t);}struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_outlives_constraints(
struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*cs,struct Cyc_Position_Segment*
loc){struct Cyc_Tcenv_SharedFenv*_tmp211;struct Cyc_List_List*_tmp212;struct Cyc_RgnOrder_RgnPO*
_tmp213;struct Cyc_Tcenv_Bindings*_tmp214;struct Cyc_Absyn_Stmt*_tmp215;struct Cyc_Tcenv_CtrlEnv*
_tmp216;void*_tmp217;void*_tmp218;int _tmp219;struct Cyc_Tcenv_Fenv _tmp210=*Cyc_Tcenv_get_fenv(
te,({const char*_tmp20F="new_outlives_constraints";_tag_dyneither(_tmp20F,sizeof(
char),25);}));_tmp211=_tmp210.shared;_tmp212=_tmp210.type_vars;_tmp213=_tmp210.region_order;
_tmp214=_tmp210.locals;_tmp215=_tmp210.encloser;_tmp216=_tmp210.ctrl_env;_tmp217=
_tmp210.capability;_tmp218=_tmp210.curr_rgn;_tmp219=_tmp210.in_notreadctxt;for(0;
cs != 0;cs=cs->tl){_tmp213=Cyc_RgnOrder_add_outlives_constraint(_tmp211->frgn,
_tmp213,(*((struct _tuple0*)cs->hd)).f1,(*((struct _tuple0*)cs->hd)).f2,loc);}{
struct Cyc_Tcenv_Fenv*_tmp21A=({struct Cyc_Tcenv_Fenv*_tmp21B=_region_malloc(r,
sizeof(*_tmp21B));_tmp21B->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp211;_tmp21B->type_vars=(
struct Cyc_List_List*)_tmp212;_tmp21B->region_order=(struct Cyc_RgnOrder_RgnPO*)
_tmp213;_tmp21B->locals=(struct Cyc_Tcenv_Bindings*)((struct Cyc_Tcenv_Bindings*)
_tmp214);_tmp21B->encloser=(struct Cyc_Absyn_Stmt*)_tmp215;_tmp21B->ctrl_env=(
struct Cyc_Tcenv_CtrlEnv*)((struct Cyc_Tcenv_CtrlEnv*)_tmp216);_tmp21B->capability=(
void*)_tmp217;_tmp21B->curr_rgn=(void*)_tmp218;_tmp21B->in_notreadctxt=(int)
_tmp219;_tmp21B->fnrgn=(struct _RegionHandle*)r;_tmp21B;});return Cyc_Tcenv_put_fenv(
r,te,_tmp21A);}}struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region_equality(struct
_RegionHandle*r,struct Cyc_Tcenv_Tenv*te,void*r1,void*r2,struct _tuple11**oldtv,
struct Cyc_Position_Segment*loc){void*_tmp21C=Cyc_Tcutil_compress(r1);void*
_tmp21D=Cyc_Tcutil_compress(r2);void*_tmp21E=Cyc_Tcutil_typ_kind(_tmp21C);void*
_tmp21F=Cyc_Tcutil_typ_kind(_tmp21D);int r1_le_r2=Cyc_Tcutil_kind_leq(_tmp21E,
_tmp21F);int r2_le_r1=Cyc_Tcutil_kind_leq(_tmp21F,_tmp21E);if(!r1_le_r2  && !
r2_le_r1){({struct Cyc_String_pa_struct _tmp225;_tmp225.tag=0;_tmp225.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(_tmp21F));{struct
Cyc_String_pa_struct _tmp224;_tmp224.tag=0;_tmp224.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_kind2string(_tmp21E));{struct Cyc_String_pa_struct
_tmp223;_tmp223.tag=0;_tmp223.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp21D));{struct Cyc_String_pa_struct _tmp222;_tmp222.tag=
0;_tmp222.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
_tmp21C));{void*_tmp220[4]={& _tmp222,& _tmp223,& _tmp224,& _tmp225};Cyc_Tcutil_terr(
loc,({const char*_tmp221="Cannot compare region handles for %s and %s\n  kinds %s and %s are not compatible\n";
_tag_dyneither(_tmp221,sizeof(char),82);}),_tag_dyneither(_tmp220,sizeof(void*),
4));}}}}});return Cyc_Tcenv_new_outlives_constraints(r,te,0,loc);}else{if(
r1_le_r2  && !r2_le_r1){struct Cyc_Absyn_Tvar*_tmp227;void*_tmp228;struct _tuple11
_tmp226=Cyc_Tcutil_swap_kind(_tmp21D,Cyc_Tcutil_kind_to_bound(_tmp21E));_tmp227=
_tmp226.f1;_tmp228=_tmp226.f2;*oldtv=({struct _tuple11*_tmp229=_region_malloc(r,
sizeof(*_tmp229));_tmp229->f1=_tmp227;_tmp229->f2=_tmp228;_tmp229;});}else{if(!
r1_le_r2  && r2_le_r1){struct Cyc_Absyn_Tvar*_tmp22B;void*_tmp22C;struct _tuple11
_tmp22A=Cyc_Tcutil_swap_kind(_tmp21C,Cyc_Tcutil_kind_to_bound(_tmp21F));_tmp22B=
_tmp22A.f1;_tmp22C=_tmp22A.f2;*oldtv=({struct _tuple11*_tmp22D=_region_malloc(r,
sizeof(*_tmp22D));_tmp22D->f1=_tmp22B;_tmp22D->f2=_tmp22C;_tmp22D;});}}}{struct
_RegionHandle*_tmp22E=Cyc_Tcenv_get_fnrgn(te);struct Cyc_List_List*_tmp22F=0;if(
_tmp21C != (void*)2  && _tmp21C != (void*)3){void*eff1=(void*)({struct Cyc_Absyn_AccessEff_struct*
_tmp232=_cycalloc(sizeof(*_tmp232));_tmp232[0]=({struct Cyc_Absyn_AccessEff_struct
_tmp233;_tmp233.tag=19;_tmp233.f1=(void*)_tmp21C;_tmp233;});_tmp232;});_tmp22F=({
struct Cyc_List_List*_tmp230=_region_malloc(_tmp22E,sizeof(*_tmp230));_tmp230->hd=({
struct _tuple0*_tmp231=_region_malloc(_tmp22E,sizeof(*_tmp231));_tmp231->f1=eff1;
_tmp231->f2=_tmp21D;_tmp231;});_tmp230->tl=_tmp22F;_tmp230;});}if(_tmp21D != (
void*)2  && _tmp21D != (void*)3){void*eff2=(void*)({struct Cyc_Absyn_AccessEff_struct*
_tmp236=_cycalloc(sizeof(*_tmp236));_tmp236[0]=({struct Cyc_Absyn_AccessEff_struct
_tmp237;_tmp237.tag=19;_tmp237.f1=(void*)_tmp21D;_tmp237;});_tmp236;});_tmp22F=({
struct Cyc_List_List*_tmp234=_region_malloc(_tmp22E,sizeof(*_tmp234));_tmp234->hd=({
struct _tuple0*_tmp235=_region_malloc(_tmp22E,sizeof(*_tmp235));_tmp235->f1=eff2;
_tmp235->f2=_tmp21C;_tmp235;});_tmp234->tl=_tmp22F;_tmp234;});}return Cyc_Tcenv_new_outlives_constraints((
struct _RegionHandle*)_tmp22E,te,_tmp22F,loc);}}void*Cyc_Tcenv_curr_rgn(struct Cyc_Tcenv_Tenv*
te){struct Cyc_Tcenv_Fenv*_tmp238=te->le;if(_tmp238 == 0)return(void*)2;{struct Cyc_Tcenv_Fenv*
fe=(struct Cyc_Tcenv_Fenv*)_tmp238;struct Cyc_Tcenv_Fenv _tmp23A;void*_tmp23B;
struct Cyc_Tcenv_Fenv*_tmp239=fe;_tmp23A=*_tmp239;_tmp23B=_tmp23A.curr_rgn;return
_tmp23B;}}void Cyc_Tcenv_check_rgn_accessible(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void*rgn){struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_get_fenv(te,({const char*_tmp245="check_rgn_accessible";
_tag_dyneither(_tmp245,sizeof(char),21);}));struct Cyc_Tcenv_Fenv _tmp23D;void*
_tmp23E;struct Cyc_RgnOrder_RgnPO*_tmp23F;struct Cyc_Tcenv_Fenv*_tmp23C=fe;_tmp23D=*
_tmp23C;_tmp23E=_tmp23D.capability;_tmp23F=_tmp23D.region_order;if(Cyc_Tcutil_region_in_effect(
0,rgn,_tmp23E) || Cyc_Tcutil_region_in_effect(1,rgn,_tmp23E))return;if(Cyc_RgnOrder_eff_outlives_eff(
_tmp23F,(void*)({struct Cyc_Absyn_AccessEff_struct*_tmp240=_cycalloc(sizeof(*
_tmp240));_tmp240[0]=({struct Cyc_Absyn_AccessEff_struct _tmp241;_tmp241.tag=19;
_tmp241.f1=(void*)rgn;_tmp241;});_tmp240;}),_tmp23E))return;({struct Cyc_String_pa_struct
_tmp244;_tmp244.tag=0;_tmp244.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(rgn));{void*_tmp242[1]={& _tmp244};Cyc_Tcutil_terr(loc,({
const char*_tmp243="Expression accesses unavailable region %s";_tag_dyneither(
_tmp243,sizeof(char),42);}),_tag_dyneither(_tmp242,sizeof(void*),1));}});}void
Cyc_Tcenv_check_rgn_resetable(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void*rgn){Cyc_Tcenv_check_rgn_accessible(te,loc,rgn);{struct Cyc_Tcenv_Fenv
_tmp248;struct Cyc_RgnOrder_RgnPO*_tmp249;struct Cyc_Tcenv_Fenv*_tmp247=Cyc_Tcenv_get_fenv(
te,({const char*_tmp246="check_rgn_resetable";_tag_dyneither(_tmp246,sizeof(char),
20);}));_tmp248=*_tmp247;_tmp249=_tmp248.region_order;{void*_tmp24A=Cyc_Tcutil_compress(
rgn);struct Cyc_Absyn_Tvar*_tmp24B;_LL82: if(_tmp24A <= (void*)4)goto _LL84;if(*((
int*)_tmp24A)!= 1)goto _LL84;_tmp24B=((struct Cyc_Absyn_VarType_struct*)_tmp24A)->f1;
_LL83: if(!Cyc_RgnOrder_is_region_resetable(_tmp249,_tmp24B))({struct Cyc_String_pa_struct
_tmp24E;_tmp24E.tag=0;_tmp24E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(rgn));{void*_tmp24C[1]={& _tmp24E};Cyc_Tcutil_terr(loc,({
const char*_tmp24D="Region %s is not resetable";_tag_dyneither(_tmp24D,sizeof(
char),27);}),_tag_dyneither(_tmp24C,sizeof(void*),1));}});return;_LL84:;_LL85:({
void*_tmp24F=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp250="check_rgn_resetable";_tag_dyneither(_tmp250,sizeof(char),20);}),
_tag_dyneither(_tmp24F,sizeof(void*),0));});_LL81:;}}}int Cyc_Tcenv_region_outlives(
struct Cyc_Tcenv_Tenv*te,void*rt_a,void*rt_b){struct Cyc_Tcenv_Fenv*_tmp251=te->le;
rt_a=Cyc_Tcutil_compress(rt_a);rt_b=Cyc_Tcutil_compress(rt_b);if(_tmp251 == 0)
return rt_a == (void*)3  || rt_a == (void*)2  && rt_b != (void*)3;{struct Cyc_Tcenv_Fenv*
fe=(struct Cyc_Tcenv_Fenv*)_tmp251;struct Cyc_Tcenv_Fenv _tmp253;struct Cyc_RgnOrder_RgnPO*
_tmp254;struct Cyc_Tcenv_Fenv*_tmp252=fe;_tmp253=*_tmp252;_tmp254=_tmp253.region_order;{
int _tmp255=Cyc_RgnOrder_effect_outlives(_tmp254,(void*)({struct Cyc_Absyn_AccessEff_struct*
_tmp256=_cycalloc(sizeof(*_tmp256));_tmp256[0]=({struct Cyc_Absyn_AccessEff_struct
_tmp257;_tmp257.tag=19;_tmp257.f1=(void*)rt_a;_tmp257;});_tmp256;}),rt_b);return
_tmp255;}}}struct _tuple12{void*f1;void*f2;struct Cyc_RgnOrder_RgnPO*f3;struct Cyc_Position_Segment*
f4;};void Cyc_Tcenv_check_effect_accessible(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void*eff){struct Cyc_Tcenv_Fenv _tmp25A;void*_tmp25B;struct Cyc_RgnOrder_RgnPO*
_tmp25C;struct Cyc_Tcenv_SharedFenv*_tmp25D;struct Cyc_Tcenv_Fenv*_tmp259=Cyc_Tcenv_get_fenv(
te,({const char*_tmp258="check_effect_accessible";_tag_dyneither(_tmp258,sizeof(
char),24);}));_tmp25A=*_tmp259;_tmp25B=_tmp25A.capability;_tmp25C=_tmp25A.region_order;
_tmp25D=_tmp25A.shared;if(Cyc_Tcutil_subset_effect(0,eff,_tmp25B))return;if(Cyc_RgnOrder_eff_outlives_eff(
_tmp25C,eff,_tmp25B))return;{struct _RegionHandle*frgn=_tmp25D->frgn;_tmp25D->delayed_effect_checks=({
struct Cyc_List_List*_tmp25E=_region_malloc(frgn,sizeof(*_tmp25E));_tmp25E->hd=({
struct _tuple12*_tmp25F=_region_malloc(frgn,sizeof(*_tmp25F));_tmp25F->f1=_tmp25B;
_tmp25F->f2=eff;_tmp25F->f3=_tmp25C;_tmp25F->f4=loc;_tmp25F;});_tmp25E->tl=
_tmp25D->delayed_effect_checks;_tmp25E;});}}void Cyc_Tcenv_check_delayed_effects(
struct Cyc_Tcenv_Tenv*te){struct Cyc_Tcenv_Fenv _tmp262;struct Cyc_Tcenv_SharedFenv*
_tmp263;struct Cyc_Tcenv_Fenv*_tmp261=Cyc_Tcenv_get_fenv(te,({const char*_tmp260="check_delayed_effects";
_tag_dyneither(_tmp260,sizeof(char),22);}));_tmp262=*_tmp261;_tmp263=_tmp262.shared;{
struct Cyc_List_List*_tmp264=_tmp263->delayed_effect_checks;for(0;_tmp264 != 0;
_tmp264=_tmp264->tl){struct _tuple12 _tmp266;void*_tmp267;void*_tmp268;struct Cyc_RgnOrder_RgnPO*
_tmp269;struct Cyc_Position_Segment*_tmp26A;struct _tuple12*_tmp265=(struct
_tuple12*)_tmp264->hd;_tmp266=*_tmp265;_tmp267=_tmp266.f1;_tmp268=_tmp266.f2;
_tmp269=_tmp266.f3;_tmp26A=_tmp266.f4;if(Cyc_Tcutil_subset_effect(1,_tmp268,
_tmp267))continue;if(Cyc_RgnOrder_eff_outlives_eff(_tmp269,_tmp268,_tmp267))
continue;({struct Cyc_String_pa_struct _tmp26E;_tmp26E.tag=0;_tmp26E.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp268));{struct
Cyc_String_pa_struct _tmp26D;_tmp26D.tag=0;_tmp26D.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp267));{void*_tmp26B[2]={& _tmp26D,&
_tmp26E};Cyc_Tcutil_terr(_tmp26A,({const char*_tmp26C="Capability \n%s\ndoes not cover function's effect\n%s";
_tag_dyneither(_tmp26C,sizeof(char),51);}),_tag_dyneither(_tmp26B,sizeof(void*),
2));}}});}}}struct _tuple13{struct Cyc_RgnOrder_RgnPO*f1;struct Cyc_List_List*f2;
struct Cyc_Position_Segment*f3;};void Cyc_Tcenv_check_rgn_partial_order(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,struct Cyc_List_List*po){struct Cyc_Tcenv_Fenv*
_tmp26F=te->le;if(_tmp26F == 0){for(0;po != 0;po=po->tl){if(!Cyc_Tcutil_subset_effect(
1,(*((struct _tuple0*)po->hd)).f1,Cyc_Absyn_empty_effect) || !Cyc_Tcutil_subset_effect(
1,(void*)({struct Cyc_Absyn_AccessEff_struct*_tmp270=_cycalloc(sizeof(*_tmp270));
_tmp270[0]=({struct Cyc_Absyn_AccessEff_struct _tmp271;_tmp271.tag=19;_tmp271.f1=(
void*)(*((struct _tuple0*)po->hd)).f2;_tmp271;});_tmp270;}),Cyc_Absyn_empty_effect))({
void*_tmp272=0;Cyc_Tcutil_terr(loc,({const char*_tmp273="the required region ordering is not satisfied here";
_tag_dyneither(_tmp273,sizeof(char),51);}),_tag_dyneither(_tmp272,sizeof(void*),
0));});}return;}{struct Cyc_Tcenv_Fenv _tmp275;struct Cyc_RgnOrder_RgnPO*_tmp276;
struct Cyc_Tcenv_SharedFenv*_tmp277;struct Cyc_Tcenv_Fenv*_tmp274=(struct Cyc_Tcenv_Fenv*)
_tmp26F;_tmp275=*_tmp274;_tmp276=_tmp275.region_order;_tmp277=_tmp275.shared;if(
!Cyc_RgnOrder_satisfies_constraints(_tmp276,po,(void*)2,0))_tmp277->delayed_constraint_checks=({
struct Cyc_List_List*_tmp278=_region_malloc(_tmp277->frgn,sizeof(*_tmp278));
_tmp278->hd=({struct _tuple13*_tmp279=_region_malloc(_tmp277->frgn,sizeof(*
_tmp279));_tmp279->f1=_tmp276;_tmp279->f2=po;_tmp279->f3=loc;_tmp279;});_tmp278->tl=
_tmp277->delayed_constraint_checks;_tmp278;});}}void Cyc_Tcenv_check_delayed_constraints(
struct Cyc_Tcenv_Tenv*te){struct Cyc_Tcenv_Fenv _tmp27C;struct Cyc_Tcenv_SharedFenv*
_tmp27D;struct Cyc_Tcenv_Fenv*_tmp27B=Cyc_Tcenv_get_fenv(te,({const char*_tmp27A="check_delayed_constraints";
_tag_dyneither(_tmp27A,sizeof(char),26);}));_tmp27C=*_tmp27B;_tmp27D=_tmp27C.shared;{
struct Cyc_List_List*_tmp27E=_tmp27D->delayed_constraint_checks;for(0;_tmp27E != 0;
_tmp27E=_tmp27E->tl){struct _tuple13 _tmp280;struct Cyc_RgnOrder_RgnPO*_tmp281;
struct Cyc_List_List*_tmp282;struct Cyc_Position_Segment*_tmp283;struct _tuple13*
_tmp27F=(struct _tuple13*)_tmp27E->hd;_tmp280=*_tmp27F;_tmp281=_tmp280.f1;_tmp282=
_tmp280.f2;_tmp283=_tmp280.f3;if(!Cyc_RgnOrder_satisfies_constraints(_tmp281,
_tmp282,(void*)2,1))({void*_tmp284=0;Cyc_Tcutil_terr(_tmp283,({const char*_tmp285="the required region ordering is not satisfied here";
_tag_dyneither(_tmp285,sizeof(char),51);}),_tag_dyneither(_tmp284,sizeof(void*),
0));});}}}struct _tuple14{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;
};struct Cyc_Tcenv_Fenv*Cyc_Tcenv_new_fenv(struct _RegionHandle*r,struct Cyc_Position_Segment*
loc,struct Cyc_Absyn_Fndecl*fd){struct Cyc_Tcenv_Bindings*locals=0;struct Cyc_Absyn_Tvar*
rgn0=({struct Cyc_Absyn_Tvar*_tmp2B2=_cycalloc(sizeof(*_tmp2B2));_tmp2B2->name=({
struct _dyneither_ptr*_tmp2B3=_cycalloc(sizeof(*_tmp2B3));_tmp2B3[0]=(struct
_dyneither_ptr)({struct Cyc_String_pa_struct _tmp2B6;_tmp2B6.tag=0;_tmp2B6.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*(*fd->name).f2);{void*_tmp2B4[1]={&
_tmp2B6};Cyc_aprintf(({const char*_tmp2B5="`%s";_tag_dyneither(_tmp2B5,sizeof(
char),4);}),_tag_dyneither(_tmp2B4,sizeof(void*),1));}});_tmp2B3;});_tmp2B2->identity=
Cyc_Tcutil_new_tvar_id();_tmp2B2->kind=(void*)& Cyc_Tcenv_rgn_kb;_tmp2B2;});
struct Cyc_List_List*_tmp286=({struct Cyc_List_List*_tmp2B1=_cycalloc(sizeof(*
_tmp2B1));_tmp2B1->hd=rgn0;_tmp2B1->tl=fd->tvs;_tmp2B1;});Cyc_Tcutil_check_unique_tvars(
loc,_tmp286);{struct Cyc_RgnOrder_RgnPO*_tmp287=Cyc_RgnOrder_initial_fn_po(r,fd->tvs,
fd->rgn_po,(void*)((struct Cyc_Core_Opt*)_check_null(fd->effect))->v,rgn0,loc);
void*param_rgn=(void*)({struct Cyc_Absyn_VarType_struct*_tmp2AF=_cycalloc(sizeof(*
_tmp2AF));_tmp2AF[0]=({struct Cyc_Absyn_VarType_struct _tmp2B0;_tmp2B0.tag=1;
_tmp2B0.f1=rgn0;_tmp2B0;});_tmp2AF;});struct Cyc_Core_Opt*param_rgn_opt=({struct
Cyc_Core_Opt*_tmp2AE=_cycalloc(sizeof(*_tmp2AE));_tmp2AE->v=(void*)param_rgn;
_tmp2AE;});struct Cyc_List_List*_tmp288=0;{struct Cyc_List_List*_tmp289=fd->args;
for(0;_tmp289 != 0;_tmp289=_tmp289->tl){struct Cyc_Absyn_Vardecl*_tmp28A=({struct
Cyc_Absyn_Vardecl*_tmp290=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl)* 1);_tmp290[
0]=({struct Cyc_Absyn_Vardecl _tmp291;_tmp291.sc=(void*)2;_tmp291.name=({struct
_tuple1*_tmp292=_cycalloc(sizeof(*_tmp292));_tmp292->f1=(union Cyc_Absyn_Nmspace)
Cyc_Absyn_Loc_n;_tmp292->f2=(*((struct _tuple14*)_tmp289->hd)).f1;_tmp292;});
_tmp291.tq=(*((struct _tuple14*)_tmp289->hd)).f2;_tmp291.type=(*((struct _tuple14*)
_tmp289->hd)).f3;_tmp291.initializer=0;_tmp291.rgn=param_rgn_opt;_tmp291.attributes=
0;_tmp291.escapes=0;_tmp291;});_tmp290;});_tmp288=({struct Cyc_List_List*_tmp28B=
_cycalloc(sizeof(struct Cyc_List_List)* 1);_tmp28B[0]=({struct Cyc_List_List
_tmp28C;_tmp28C.hd=_tmp28A;_tmp28C.tl=_tmp288;_tmp28C;});_tmp28B;});locals=({
struct Cyc_Tcenv_Bindings*_tmp28D=_region_malloc(r,sizeof(*_tmp28D));_tmp28D->v=(*((
struct _tuple14*)_tmp289->hd)).f1;_tmp28D->b=(void*)({struct Cyc_Absyn_Param_b_struct*
_tmp28E=_cycalloc(sizeof(*_tmp28E));_tmp28E[0]=({struct Cyc_Absyn_Param_b_struct
_tmp28F;_tmp28F.tag=2;_tmp28F.f1=_tmp28A;_tmp28F;});_tmp28E;});_tmp28D->tl=
locals;_tmp28D;});}}if(fd->cyc_varargs != 0){struct Cyc_Core_Opt*_tmp294;struct Cyc_Absyn_Tqual
_tmp295;void*_tmp296;int _tmp297;struct Cyc_Absyn_VarargInfo _tmp293=*((struct Cyc_Absyn_VarargInfo*)
_check_null(fd->cyc_varargs));_tmp294=_tmp293.name;_tmp295=_tmp293.tq;_tmp296=
_tmp293.type;_tmp297=_tmp293.inject;if(_tmp294 != 0){void*_tmp298=Cyc_Absyn_dyneither_typ(
_tmp296,param_rgn,_tmp295,Cyc_Absyn_false_conref);struct Cyc_Absyn_Vardecl*
_tmp299=({struct Cyc_Absyn_Vardecl*_tmp29E=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl)
* 1);_tmp29E[0]=({struct Cyc_Absyn_Vardecl _tmp29F;_tmp29F.sc=(void*)2;_tmp29F.name=({
struct _tuple1*_tmp2A0=_cycalloc(sizeof(*_tmp2A0));_tmp2A0->f1=(union Cyc_Absyn_Nmspace)
Cyc_Absyn_Loc_n;_tmp2A0->f2=(struct _dyneither_ptr*)_tmp294->v;_tmp2A0;});_tmp29F.tq=
Cyc_Absyn_empty_tqual(0);_tmp29F.type=_tmp298;_tmp29F.initializer=0;_tmp29F.rgn=
param_rgn_opt;_tmp29F.attributes=0;_tmp29F.escapes=0;_tmp29F;});_tmp29E;});
_tmp288=({struct Cyc_List_List*_tmp29A=_cycalloc(sizeof(*_tmp29A));_tmp29A->hd=
_tmp299;_tmp29A->tl=_tmp288;_tmp29A;});locals=({struct Cyc_Tcenv_Bindings*_tmp29B=
_region_malloc(r,sizeof(*_tmp29B));_tmp29B->v=(struct _dyneither_ptr*)_tmp294->v;
_tmp29B->b=(void*)({struct Cyc_Absyn_Param_b_struct*_tmp29C=_cycalloc(sizeof(*
_tmp29C));_tmp29C[0]=({struct Cyc_Absyn_Param_b_struct _tmp29D;_tmp29D.tag=2;
_tmp29D.f1=_tmp299;_tmp29D;});_tmp29C;});_tmp29B->tl=locals;_tmp29B;});}else{({
void*_tmp2A1=0;Cyc_Tcutil_terr(loc,({const char*_tmp2A2="missing name for varargs";
_tag_dyneither(_tmp2A2,sizeof(char),25);}),_tag_dyneither(_tmp2A1,sizeof(void*),
0));});}}fd->param_vardecls=({struct Cyc_Core_Opt*_tmp2A3=_cycalloc(sizeof(struct
Cyc_Core_Opt)* 1);_tmp2A3[0]=({struct Cyc_Core_Opt _tmp2A4;_tmp2A4.v=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp288);_tmp2A4;});_tmp2A3;});return({
struct Cyc_Tcenv_Fenv*_tmp2A5=_region_malloc(r,sizeof(*_tmp2A5));_tmp2A5->shared=(
struct Cyc_Tcenv_SharedFenv*)({struct Cyc_Tcenv_SharedFenv*_tmp2AD=_region_malloc(
r,sizeof(*_tmp2AD));_tmp2AD->frgn=r;_tmp2AD->return_typ=fd->ret_type;_tmp2AD->seen_labels=((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,
struct _dyneither_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp);_tmp2AD->needed_labels=((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,
struct _dyneither_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp);_tmp2AD->delayed_effect_checks=
0;_tmp2AD->delayed_constraint_checks=0;_tmp2AD;});_tmp2A5->type_vars=(struct Cyc_List_List*)
_tmp286;_tmp2A5->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp287;_tmp2A5->locals=(
struct Cyc_Tcenv_Bindings*)locals;_tmp2A5->encloser=(struct Cyc_Absyn_Stmt*)fd->body;
_tmp2A5->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)({struct Cyc_Tcenv_CtrlEnv*_tmp2AC=
_region_malloc(r,sizeof(*_tmp2AC));_tmp2AC->ctrl_rgn=r;_tmp2AC->continue_stmt=(
void*)0;_tmp2AC->break_stmt=(void*)0;_tmp2AC->fallthru_clause=0;_tmp2AC->next_stmt=(
void*)2;_tmp2AC->try_depth=0;_tmp2AC;});_tmp2A5->capability=(void*)((void*)({
struct Cyc_Absyn_JoinEff_struct*_tmp2A6=_cycalloc(sizeof(*_tmp2A6));_tmp2A6[0]=({
struct Cyc_Absyn_JoinEff_struct _tmp2A7;_tmp2A7.tag=20;_tmp2A7.f1=({struct Cyc_List_List*
_tmp2A8=_cycalloc(sizeof(*_tmp2A8));_tmp2A8->hd=(void*)((void*)({struct Cyc_Absyn_AccessEff_struct*
_tmp2AA=_cycalloc(sizeof(*_tmp2AA));_tmp2AA[0]=({struct Cyc_Absyn_AccessEff_struct
_tmp2AB;_tmp2AB.tag=19;_tmp2AB.f1=(void*)param_rgn;_tmp2AB;});_tmp2AA;}));
_tmp2A8->tl=({struct Cyc_List_List*_tmp2A9=_cycalloc(sizeof(*_tmp2A9));_tmp2A9->hd=(
void*)((void*)((struct Cyc_Core_Opt*)_check_null(fd->effect))->v);_tmp2A9->tl=0;
_tmp2A9;});_tmp2A8;});_tmp2A7;});_tmp2A6;}));_tmp2A5->curr_rgn=(void*)param_rgn;
_tmp2A5->in_notreadctxt=(int)0;_tmp2A5->fnrgn=(struct _RegionHandle*)r;_tmp2A5;});}}
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_nested_fenv(struct Cyc_Position_Segment*loc,struct
Cyc_Tcenv_Fenv*old_fenv,struct Cyc_Absyn_Fndecl*fd){struct Cyc_Tcenv_Fenv _tmp2B8;
struct Cyc_Tcenv_Bindings*_tmp2B9;struct Cyc_RgnOrder_RgnPO*_tmp2BA;struct Cyc_List_List*
_tmp2BB;struct Cyc_Tcenv_SharedFenv*_tmp2BC;struct _RegionHandle*_tmp2BD;struct Cyc_Tcenv_Fenv*
_tmp2B7=old_fenv;_tmp2B8=*_tmp2B7;_tmp2B9=_tmp2B8.locals;_tmp2BA=_tmp2B8.region_order;
_tmp2BB=_tmp2B8.type_vars;_tmp2BC=_tmp2B8.shared;_tmp2BD=_tmp2B8.fnrgn;{struct
_RegionHandle*_tmp2BE=_tmp2BC->frgn;struct Cyc_Tcenv_Bindings*_tmp2BF=(struct Cyc_Tcenv_Bindings*)
_tmp2B9;struct Cyc_Absyn_Tvar*rgn0=({struct Cyc_Absyn_Tvar*_tmp2EE=_cycalloc(
sizeof(*_tmp2EE));_tmp2EE->name=({struct _dyneither_ptr*_tmp2EF=_cycalloc(sizeof(*
_tmp2EF));_tmp2EF[0]=(struct _dyneither_ptr)({struct Cyc_String_pa_struct _tmp2F2;
_tmp2F2.tag=0;_tmp2F2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*fd->name).f2);{
void*_tmp2F0[1]={& _tmp2F2};Cyc_aprintf(({const char*_tmp2F1="`%s";_tag_dyneither(
_tmp2F1,sizeof(char),4);}),_tag_dyneither(_tmp2F0,sizeof(void*),1));}});_tmp2EF;});
_tmp2EE->identity=Cyc_Tcutil_new_tvar_id();_tmp2EE->kind=(void*)& Cyc_Tcenv_rgn_kb;
_tmp2EE;});{struct Cyc_List_List*_tmp2C0=fd->tvs;for(0;_tmp2C0 != 0;_tmp2C0=
_tmp2C0->tl){if(Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp2C0->hd)== (void*)
3)_tmp2BA=Cyc_RgnOrder_add_youngest(_tmp2BE,_tmp2BA,(struct Cyc_Absyn_Tvar*)
_tmp2C0->hd,0,0);else{if(Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp2C0->hd)
== (void*)4  || Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp2C0->hd)== (void*)
5)({void*_tmp2C1=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp2C2="non-intuitionistic tvar in nested_fenv";_tag_dyneither(
_tmp2C2,sizeof(char),39);}),_tag_dyneither(_tmp2C1,sizeof(void*),0));});}}}
_tmp2BA=Cyc_RgnOrder_add_youngest(_tmp2BE,_tmp2BA,rgn0,0,0);{struct Cyc_List_List*
_tmp2C3=({struct Cyc_List_List*_tmp2ED=_cycalloc(sizeof(*_tmp2ED));_tmp2ED->hd=
rgn0;_tmp2ED->tl=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_append)(fd->tvs,_tmp2BB);_tmp2ED;});Cyc_Tcutil_check_unique_tvars(
loc,_tmp2C3);{void*param_rgn=(void*)({struct Cyc_Absyn_VarType_struct*_tmp2EB=
_cycalloc(sizeof(*_tmp2EB));_tmp2EB[0]=({struct Cyc_Absyn_VarType_struct _tmp2EC;
_tmp2EC.tag=1;_tmp2EC.f1=rgn0;_tmp2EC;});_tmp2EB;});struct Cyc_Core_Opt*
param_rgn_opt=({struct Cyc_Core_Opt*_tmp2EA=_cycalloc(sizeof(*_tmp2EA));_tmp2EA->v=(
void*)param_rgn;_tmp2EA;});struct Cyc_List_List*_tmp2C4=0;{struct Cyc_List_List*
_tmp2C5=fd->args;for(0;_tmp2C5 != 0;_tmp2C5=_tmp2C5->tl){struct Cyc_Absyn_Vardecl*
_tmp2C6=({struct Cyc_Absyn_Vardecl*_tmp2CC=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl)
* 1);_tmp2CC[0]=({struct Cyc_Absyn_Vardecl _tmp2CD;_tmp2CD.sc=(void*)2;_tmp2CD.name=({
struct _tuple1*_tmp2CE=_cycalloc(sizeof(*_tmp2CE));_tmp2CE->f1=(union Cyc_Absyn_Nmspace)
Cyc_Absyn_Loc_n;_tmp2CE->f2=(*((struct _tuple14*)_tmp2C5->hd)).f1;_tmp2CE;});
_tmp2CD.tq=(*((struct _tuple14*)_tmp2C5->hd)).f2;_tmp2CD.type=(*((struct _tuple14*)
_tmp2C5->hd)).f3;_tmp2CD.initializer=0;_tmp2CD.rgn=param_rgn_opt;_tmp2CD.attributes=
0;_tmp2CD.escapes=0;_tmp2CD;});_tmp2CC;});_tmp2C4=({struct Cyc_List_List*_tmp2C7=
_cycalloc(sizeof(struct Cyc_List_List)* 1);_tmp2C7[0]=({struct Cyc_List_List
_tmp2C8;_tmp2C8.hd=_tmp2C6;_tmp2C8.tl=_tmp2C4;_tmp2C8;});_tmp2C7;});_tmp2BF=(
struct Cyc_Tcenv_Bindings*)({struct Cyc_Tcenv_Bindings*_tmp2C9=_region_malloc(
_tmp2BE,sizeof(*_tmp2C9));_tmp2C9->v=(*((struct _tuple14*)_tmp2C5->hd)).f1;
_tmp2C9->b=(void*)({struct Cyc_Absyn_Param_b_struct*_tmp2CA=_cycalloc(sizeof(*
_tmp2CA));_tmp2CA[0]=({struct Cyc_Absyn_Param_b_struct _tmp2CB;_tmp2CB.tag=2;
_tmp2CB.f1=_tmp2C6;_tmp2CB;});_tmp2CA;});_tmp2C9->tl=_tmp2BF;_tmp2C9;});}}if(fd->cyc_varargs
!= 0){struct Cyc_Core_Opt*_tmp2D0;struct Cyc_Absyn_Tqual _tmp2D1;void*_tmp2D2;int
_tmp2D3;struct Cyc_Absyn_VarargInfo _tmp2CF=*((struct Cyc_Absyn_VarargInfo*)
_check_null(fd->cyc_varargs));_tmp2D0=_tmp2CF.name;_tmp2D1=_tmp2CF.tq;_tmp2D2=
_tmp2CF.type;_tmp2D3=_tmp2CF.inject;if(_tmp2D0 != 0){void*_tmp2D4=Cyc_Absyn_dyneither_typ(
_tmp2D2,param_rgn,_tmp2D1,Cyc_Absyn_false_conref);struct Cyc_Absyn_Vardecl*
_tmp2D5=({struct Cyc_Absyn_Vardecl*_tmp2DA=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl)
* 1);_tmp2DA[0]=({struct Cyc_Absyn_Vardecl _tmp2DB;_tmp2DB.sc=(void*)2;_tmp2DB.name=({
struct _tuple1*_tmp2DC=_cycalloc(sizeof(*_tmp2DC));_tmp2DC->f1=(union Cyc_Absyn_Nmspace)
Cyc_Absyn_Loc_n;_tmp2DC->f2=(struct _dyneither_ptr*)_tmp2D0->v;_tmp2DC;});_tmp2DB.tq=
Cyc_Absyn_empty_tqual(0);_tmp2DB.type=_tmp2D4;_tmp2DB.initializer=0;_tmp2DB.rgn=
param_rgn_opt;_tmp2DB.attributes=0;_tmp2DB.escapes=0;_tmp2DB;});_tmp2DA;});
_tmp2C4=({struct Cyc_List_List*_tmp2D6=_cycalloc(sizeof(*_tmp2D6));_tmp2D6->hd=
_tmp2D5;_tmp2D6->tl=_tmp2C4;_tmp2D6;});_tmp2BF=(struct Cyc_Tcenv_Bindings*)({
struct Cyc_Tcenv_Bindings*_tmp2D7=_region_malloc(_tmp2BE,sizeof(*_tmp2D7));
_tmp2D7->v=(struct _dyneither_ptr*)_tmp2D0->v;_tmp2D7->b=(void*)({struct Cyc_Absyn_Param_b_struct*
_tmp2D8=_cycalloc(sizeof(*_tmp2D8));_tmp2D8[0]=({struct Cyc_Absyn_Param_b_struct
_tmp2D9;_tmp2D9.tag=2;_tmp2D9.f1=_tmp2D5;_tmp2D9;});_tmp2D8;});_tmp2D7->tl=
_tmp2BF;_tmp2D7;});}else{({void*_tmp2DD=0;Cyc_Tcutil_terr(loc,({const char*
_tmp2DE="missing name for varargs";_tag_dyneither(_tmp2DE,sizeof(char),25);}),
_tag_dyneither(_tmp2DD,sizeof(void*),0));});}}fd->param_vardecls=({struct Cyc_Core_Opt*
_tmp2DF=_cycalloc(sizeof(struct Cyc_Core_Opt)* 1);_tmp2DF[0]=({struct Cyc_Core_Opt
_tmp2E0;_tmp2E0.v=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmp2C4);_tmp2E0;});_tmp2DF;});return(struct Cyc_Tcenv_Fenv*)({struct Cyc_Tcenv_Fenv*
_tmp2E1=_region_malloc(_tmp2BE,sizeof(*_tmp2E1));_tmp2E1->shared=(struct Cyc_Tcenv_SharedFenv*)({
struct Cyc_Tcenv_SharedFenv*_tmp2E9=_region_malloc(_tmp2BE,sizeof(*_tmp2E9));
_tmp2E9->frgn=_tmp2BE;_tmp2E9->return_typ=fd->ret_type;_tmp2E9->seen_labels=((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,
struct _dyneither_ptr*)))Cyc_Dict_rempty)(_tmp2BE,Cyc_strptrcmp);_tmp2E9->needed_labels=((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,
struct _dyneither_ptr*)))Cyc_Dict_rempty)(_tmp2BE,Cyc_strptrcmp);_tmp2E9->delayed_effect_checks=
0;_tmp2E9->delayed_constraint_checks=0;_tmp2E9;});_tmp2E1->type_vars=(struct Cyc_List_List*)
_tmp2C3;_tmp2E1->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp2BA;_tmp2E1->locals=(
struct Cyc_Tcenv_Bindings*)_tmp2BF;_tmp2E1->encloser=(struct Cyc_Absyn_Stmt*)fd->body;
_tmp2E1->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)((struct Cyc_Tcenv_CtrlEnv*)({struct
Cyc_Tcenv_CtrlEnv*_tmp2E8=_region_malloc(_tmp2BE,sizeof(*_tmp2E8));_tmp2E8->ctrl_rgn=
_tmp2BE;_tmp2E8->continue_stmt=(void*)0;_tmp2E8->break_stmt=(void*)0;_tmp2E8->fallthru_clause=
0;_tmp2E8->next_stmt=(void*)2;_tmp2E8->try_depth=0;_tmp2E8;}));_tmp2E1->capability=(
void*)((void*)({struct Cyc_Absyn_JoinEff_struct*_tmp2E2=_cycalloc(sizeof(*_tmp2E2));
_tmp2E2[0]=({struct Cyc_Absyn_JoinEff_struct _tmp2E3;_tmp2E3.tag=20;_tmp2E3.f1=({
struct Cyc_List_List*_tmp2E4=_cycalloc(sizeof(*_tmp2E4));_tmp2E4->hd=(void*)((
void*)({struct Cyc_Absyn_AccessEff_struct*_tmp2E6=_cycalloc(sizeof(*_tmp2E6));
_tmp2E6[0]=({struct Cyc_Absyn_AccessEff_struct _tmp2E7;_tmp2E7.tag=19;_tmp2E7.f1=(
void*)param_rgn;_tmp2E7;});_tmp2E6;}));_tmp2E4->tl=({struct Cyc_List_List*_tmp2E5=
_cycalloc(sizeof(*_tmp2E5));_tmp2E5->hd=(void*)((void*)((struct Cyc_Core_Opt*)
_check_null(fd->effect))->v);_tmp2E5->tl=0;_tmp2E5;});_tmp2E4;});_tmp2E3;});
_tmp2E2;}));_tmp2E1->curr_rgn=(void*)param_rgn;_tmp2E1->in_notreadctxt=(int)0;
_tmp2E1->fnrgn=(struct _RegionHandle*)_tmp2BD;_tmp2E1;});}}}}
