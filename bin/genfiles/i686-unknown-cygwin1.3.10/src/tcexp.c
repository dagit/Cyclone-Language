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
void*v;};struct _tuple0{void*f1;void*f2;};void*Cyc_Core_snd(struct _tuple0*);
extern char Cyc_Core_Invalid_argument[17];struct Cyc_Core_Invalid_argument_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8];struct Cyc_Core_Failure_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11];struct Cyc_Core_Impossible_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10];struct Cyc_Core_Not_found_struct{
char*tag;};extern char Cyc_Core_Unreachable[12];struct Cyc_Core_Unreachable_struct{
char*tag;struct _dyneither_ptr f1;};extern struct _RegionHandle*Cyc_Core_heap_region;
extern char Cyc_Core_Open_Region[12];struct Cyc_Core_Open_Region_struct{char*tag;};
extern char Cyc_Core_Free_Region[12];struct Cyc_Core_Free_Region_struct{char*tag;};
struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};int Cyc_List_length(struct
Cyc_List_List*x);struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*
x);struct Cyc_List_List*Cyc_List_rmap(struct _RegionHandle*,void*(*f)(void*),
struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rmap_c(struct _RegionHandle*,
void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[
14];struct Cyc_List_List_mismatch_struct{char*tag;};struct Cyc_List_List*Cyc_List_imp_rev(
struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,
struct Cyc_List_List*y);struct Cyc_List_List*Cyc_List_rappend(struct _RegionHandle*,
struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_struct{
char*tag;};void*Cyc_List_nth(struct Cyc_List_List*x,int n);struct Cyc_List_List*Cyc_List_nth_tail(
struct Cyc_List_List*x,int i);int Cyc_List_exists_c(int(*pred)(void*,void*),void*
env,struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rzip(struct _RegionHandle*
r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y);struct Cyc_Lineno_Pos{
struct _dyneither_ptr logical_file;struct _dyneither_ptr line;int line_no;int col;};
extern char Cyc_Position_Exit[5];struct Cyc_Position_Exit_struct{char*tag;};struct
Cyc_Position_Segment;struct Cyc_Position_Lex_struct{int tag;};struct Cyc_Position_Parse_struct{
int tag;};struct Cyc_Position_Elab_struct{int tag;};struct Cyc_Position_Error{struct
_dyneither_ptr source;struct Cyc_Position_Segment*seg;void*kind;struct
_dyneither_ptr desc;};extern char Cyc_Position_Nocontext[10];struct Cyc_Position_Nocontext_struct{
char*tag;};struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct
_union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{
int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct
_union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_Loc_n Loc_n;};union Cyc_Absyn_Nmspace
Cyc_Absyn_Loc_n;union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(struct Cyc_List_List*);
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*);struct _tuple1{union
Cyc_Absyn_Nmspace f1;struct _dyneither_ptr*f2;};enum Cyc_Absyn_Scope{Cyc_Absyn_Static
 = 0,Cyc_Absyn_Abstract  = 1,Cyc_Absyn_Public  = 2,Cyc_Absyn_Extern  = 3,Cyc_Absyn_ExternC
 = 4,Cyc_Absyn_Register  = 5};struct Cyc_Absyn_Tqual{int print_const;int q_volatile;
int q_restrict;int real_const;struct Cyc_Position_Segment*loc;};enum Cyc_Absyn_Size_of{
Cyc_Absyn_Char_sz  = 0,Cyc_Absyn_Short_sz  = 1,Cyc_Absyn_Int_sz  = 2,Cyc_Absyn_Long_sz
 = 3,Cyc_Absyn_LongLong_sz  = 4};enum Cyc_Absyn_Kind{Cyc_Absyn_AnyKind  = 0,Cyc_Absyn_MemKind
 = 1,Cyc_Absyn_BoxKind  = 2,Cyc_Absyn_RgnKind  = 3,Cyc_Absyn_UniqueRgnKind  = 4,Cyc_Absyn_TopRgnKind
 = 5,Cyc_Absyn_EffKind  = 6,Cyc_Absyn_IntKind  = 7};enum Cyc_Absyn_Sign{Cyc_Absyn_Signed
 = 0,Cyc_Absyn_Unsigned  = 1,Cyc_Absyn_None  = 2};enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA
 = 0,Cyc_Absyn_UnionA  = 1};struct _union_Constraint_Eq_constr{int tag;void*val;};
struct _union_Constraint_Forward_constr{int tag;union Cyc_Absyn_Constraint*val;};
struct _union_Constraint_No_constr{int tag;int val;};union Cyc_Absyn_Constraint{
struct _union_Constraint_Eq_constr Eq_constr;struct
_union_Constraint_Forward_constr Forward_constr;struct _union_Constraint_No_constr
No_constr;};struct Cyc_Absyn_Eq_kb_struct{int tag;enum Cyc_Absyn_Kind f1;};struct
Cyc_Absyn_Unknown_kb_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_struct{
int tag;struct Cyc_Core_Opt*f1;enum Cyc_Absyn_Kind f2;};struct Cyc_Absyn_Tvar{struct
_dyneither_ptr*name;int identity;void*kind;};struct Cyc_Absyn_DynEither_b_struct{
int tag;};struct Cyc_Absyn_Upper_b_struct{int tag;struct Cyc_Absyn_Exp*f1;};extern
struct Cyc_Absyn_DynEither_b_struct Cyc_Absyn_DynEither_b_val;struct Cyc_Absyn_PtrLoc{
struct Cyc_Position_Segment*ptr_loc;struct Cyc_Position_Segment*rgn_loc;struct Cyc_Position_Segment*
zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;union Cyc_Absyn_Constraint*nullable;
union Cyc_Absyn_Constraint*bounds;union Cyc_Absyn_Constraint*zero_term;struct Cyc_Absyn_PtrLoc*
ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_typ;struct Cyc_Absyn_Tqual elt_tq;struct
Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_Numelts_ptrqual_struct{int tag;struct
Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Region_ptrqual_struct{int tag;void*f1;};struct
Cyc_Absyn_Thin_ptrqual_struct{int tag;};struct Cyc_Absyn_Fat_ptrqual_struct{int tag;
};struct Cyc_Absyn_Zeroterm_ptrqual_struct{int tag;};struct Cyc_Absyn_Nozeroterm_ptrqual_struct{
int tag;};struct Cyc_Absyn_Notnull_ptrqual_struct{int tag;};struct Cyc_Absyn_Nullable_ptrqual_struct{
int tag;};struct Cyc_Absyn_VarargInfo{struct Cyc_Core_Opt*name;struct Cyc_Absyn_Tqual
tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;struct
Cyc_Core_Opt*effect;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*
cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;};struct
Cyc_Absyn_UnknownDatatypeInfo{struct _tuple1*name;int is_extensible;};struct
_union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo
val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**
val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype
UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};struct
Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*
targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple1*datatype_name;
struct _tuple1*field_name;int is_extensible;};struct
_union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo
val;};struct _tuple2{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*
f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple2 val;
};union Cyc_Absyn_DatatypeFieldInfoU{struct
_union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct
_union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};union Cyc_Absyn_DatatypeFieldInfoU
Cyc_Absyn_KnownDatatypefield(struct Cyc_Absyn_Datatypedecl*,struct Cyc_Absyn_Datatypefield*);
struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;
struct Cyc_List_List*targs;};struct _tuple3{enum Cyc_Absyn_AggrKind f1;struct
_tuple1*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int tag;
struct _tuple3 val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**
val;};union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;
struct _union_AggrInfoU_KnownAggr KnownAggr;};union Cyc_Absyn_AggrInfoU Cyc_Absyn_KnownAggr(
struct Cyc_Absyn_Aggrdecl**);struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU
aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;
struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*
zero_term;struct Cyc_Position_Segment*zt_loc;};struct Cyc_Absyn_VoidType_struct{
int tag;};struct Cyc_Absyn_Evar_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Core_Opt*
f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_struct{int tag;struct Cyc_Absyn_Tvar*
f1;};struct Cyc_Absyn_DatatypeType_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;}
;struct Cyc_Absyn_DatatypeFieldType_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo
f1;};struct Cyc_Absyn_PointerType_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};
struct Cyc_Absyn_IntType_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of
f2;};struct Cyc_Absyn_FloatType_struct{int tag;};struct Cyc_Absyn_DoubleType_struct{
int tag;int f1;};struct Cyc_Absyn_ArrayType_struct{int tag;struct Cyc_Absyn_ArrayInfo
f1;};struct Cyc_Absyn_FnType_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_struct{int tag;struct Cyc_Absyn_AggrInfo
f1;};struct Cyc_Absyn_AnonAggrType_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct
Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_struct{int tag;struct _tuple1*f1;
struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_struct{int tag;struct
Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_struct{int tag;void*f1;};struct
Cyc_Absyn_DynRgnType_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_struct{
int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;
void**f4;};struct Cyc_Absyn_ValueofType_struct{int tag;struct Cyc_Absyn_Exp*f1;};
struct Cyc_Absyn_TagType_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_struct{
int tag;};struct Cyc_Absyn_UniqueRgn_struct{int tag;};struct Cyc_Absyn_AccessEff_struct{
int tag;void*f1;};struct Cyc_Absyn_JoinEff_struct{int tag;struct Cyc_List_List*f1;};
struct Cyc_Absyn_RgnsEff_struct{int tag;void*f1;};extern struct Cyc_Absyn_HeapRgn_struct
Cyc_Absyn_HeapRgn_val;extern struct Cyc_Absyn_UniqueRgn_struct Cyc_Absyn_UniqueRgn_val;
extern struct Cyc_Absyn_VoidType_struct Cyc_Absyn_VoidType_val;struct Cyc_Absyn_NoTypes_struct{
int tag;struct Cyc_List_List*f1;struct Cyc_Position_Segment*f2;};struct Cyc_Absyn_WithTypes_struct{
int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;struct Cyc_Core_Opt*
f4;struct Cyc_List_List*f5;};enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0,
Cyc_Absyn_Scanf_ft  = 1};struct Cyc_Absyn_Regparm_att_struct{int tag;int f1;};struct
Cyc_Absyn_Stdcall_att_struct{int tag;};struct Cyc_Absyn_Cdecl_att_struct{int tag;};
struct Cyc_Absyn_Fastcall_att_struct{int tag;};struct Cyc_Absyn_Noreturn_att_struct{
int tag;};struct Cyc_Absyn_Const_att_struct{int tag;};struct Cyc_Absyn_Aligned_att_struct{
int tag;int f1;};struct Cyc_Absyn_Packed_att_struct{int tag;};struct Cyc_Absyn_Section_att_struct{
int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_struct{int tag;};
struct Cyc_Absyn_Shared_att_struct{int tag;};struct Cyc_Absyn_Unused_att_struct{int
tag;};struct Cyc_Absyn_Weak_att_struct{int tag;};struct Cyc_Absyn_Dllimport_att_struct{
int tag;};struct Cyc_Absyn_Dllexport_att_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_struct{
int tag;};struct Cyc_Absyn_Constructor_att_struct{int tag;};struct Cyc_Absyn_Destructor_att_struct{
int tag;};struct Cyc_Absyn_No_check_memory_usage_att_struct{int tag;};struct Cyc_Absyn_Format_att_struct{
int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_struct{
int tag;int f1;};struct Cyc_Absyn_Pure_att_struct{int tag;};struct Cyc_Absyn_Mode_att_struct{
int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Carray_mod_struct{int tag;union
Cyc_Absyn_Constraint*f1;struct Cyc_Position_Segment*f2;};struct Cyc_Absyn_ConstArray_mod_struct{
int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;struct Cyc_Position_Segment*
f3;};struct Cyc_Absyn_Pointer_mod_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct
Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_struct{int tag;void*f1;};struct
Cyc_Absyn_TypeParams_mod_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Position_Segment*
f2;int f3;};struct Cyc_Absyn_Attributes_mod_struct{int tag;struct Cyc_Position_Segment*
f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct
_tuple4{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct
_tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f1;short f2;};struct
_union_Cnst_Short_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign
f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple6 val;};struct _tuple7{enum 
Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple7
val;};struct _union_Cnst_Float_c{int tag;struct _dyneither_ptr val;};struct
_union_Cnst_String_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct
_union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct
_union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct
_union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct
_union_Cnst_String_c String_c;};extern union Cyc_Absyn_Cnst Cyc_Absyn_Null_c;union
Cyc_Absyn_Cnst Cyc_Absyn_Char_c(enum Cyc_Absyn_Sign,char);union Cyc_Absyn_Cnst Cyc_Absyn_Short_c(
enum Cyc_Absyn_Sign,short);union Cyc_Absyn_Cnst Cyc_Absyn_Int_c(enum Cyc_Absyn_Sign,
int);enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0,Cyc_Absyn_Times  = 1,Cyc_Absyn_Minus
 = 2,Cyc_Absyn_Div  = 3,Cyc_Absyn_Mod  = 4,Cyc_Absyn_Eq  = 5,Cyc_Absyn_Neq  = 6,Cyc_Absyn_Gt
 = 7,Cyc_Absyn_Lt  = 8,Cyc_Absyn_Gte  = 9,Cyc_Absyn_Lte  = 10,Cyc_Absyn_Not  = 11,
Cyc_Absyn_Bitnot  = 12,Cyc_Absyn_Bitand  = 13,Cyc_Absyn_Bitor  = 14,Cyc_Absyn_Bitxor
 = 15,Cyc_Absyn_Bitlshift  = 16,Cyc_Absyn_Bitlrshift  = 17,Cyc_Absyn_Bitarshift  = 
18,Cyc_Absyn_Numelts  = 19};enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0,Cyc_Absyn_PostInc
 = 1,Cyc_Absyn_PreDec  = 2,Cyc_Absyn_PostDec  = 3};struct Cyc_Absyn_VarargCallInfo{
int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};
struct Cyc_Absyn_StructField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_struct{
int tag;unsigned int f1;};enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,
Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion
 = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**
elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;};struct Cyc_Absyn_Const_e_struct{
int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_struct{int tag;struct _tuple1*
f1;void*f2;};struct Cyc_Absyn_UnknownId_e_struct{int tag;struct _tuple1*f1;};struct
Cyc_Absyn_Primop_e_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;
};struct Cyc_Absyn_AssignOp_e_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*
f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_struct{int tag;struct Cyc_Absyn_Exp*
f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_struct{int tag;
struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnknownCall_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_FnCall_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*
f3;};struct Cyc_Absyn_Throw_e_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoInstantiate_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_struct{int tag;
struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_struct{
int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct
Cyc_Absyn_Address_e_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_struct{
int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_struct{int tag;struct Cyc_Absyn_Exp*
f1;};struct Cyc_Absyn_Offsetof_e_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_Gentyp_e_struct{
int tag;struct Cyc_List_List*f1;void*f2;};struct Cyc_Absyn_Deref_e_struct{int tag;
struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_struct{int tag;struct Cyc_Absyn_Exp*
f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_struct{int
tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_struct{
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
struct Cyc_Absyn_Skip_s_struct{int tag;};struct Cyc_Absyn_Exp_s_struct{int tag;
struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_struct{int tag;struct Cyc_Absyn_Stmt*
f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_struct{int tag;struct Cyc_Absyn_Exp*
f1;};struct Cyc_Absyn_IfThenElse_s_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*
f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*
f2;};struct Cyc_Absyn_While_s_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*
f2;};struct Cyc_Absyn_Break_s_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_struct{
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_struct{int tag;struct
_dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_struct{int tag;
struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;}
;struct Cyc_Absyn_Switch_s_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*
f2;};struct Cyc_Absyn_Fallthru_s_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**
f2;};struct Cyc_Absyn_Decl_s_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*
f2;};struct Cyc_Absyn_Label_s_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*
f2;};struct Cyc_Absyn_Do_s_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;
};struct Cyc_Absyn_TryCatch_s_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*
f2;};struct Cyc_Absyn_ResetRegion_s_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct
Cyc_Absyn_Stmt{void*r;struct Cyc_Position_Segment*loc;struct Cyc_List_List*
non_local_preds;int try_depth;void*annot;};struct Cyc_Absyn_Wild_p_struct{int tag;}
;struct Cyc_Absyn_Var_p_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*
f2;};struct Cyc_Absyn_Reference_p_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct
Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_struct{int tag;struct Cyc_Absyn_Tvar*
f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_struct{int tag;struct Cyc_List_List*
f1;int f2;};struct Cyc_Absyn_Pointer_p_struct{int tag;struct Cyc_Absyn_Pat*f1;};
struct Cyc_Absyn_Aggr_p_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*
f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_struct{int tag;
struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*
f3;int f4;};struct Cyc_Absyn_Null_p_struct{int tag;};struct Cyc_Absyn_Int_p_struct{
int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_struct{int tag;char f1;
};struct Cyc_Absyn_Float_p_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Enum_p_struct{
int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_struct{
int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_struct{
int tag;struct _tuple1*f1;};struct Cyc_Absyn_UnknownCall_p_struct{int tag;struct
_tuple1*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_struct{int tag;
struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;struct Cyc_Core_Opt*topt;
struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*
pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*
body;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Unresolved_b_struct{int tag;
};struct Cyc_Absyn_Global_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_struct{
int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_struct{int tag;struct
Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_Absyn_Pat_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};extern
struct Cyc_Absyn_Unresolved_b_struct Cyc_Absyn_Unresolved_b_val;struct Cyc_Absyn_Vardecl{
enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Absyn_Tqual tq;void*type;
struct Cyc_Absyn_Exp*initializer;struct Cyc_Core_Opt*rgn;struct Cyc_List_List*
attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int
is_inline;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*effect;
void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*
cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;struct Cyc_Core_Opt*
cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;
struct Cyc_List_List*attributes;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*
name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*
attributes;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct
Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{
enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*
tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Datatypefield{
struct _tuple1*name;struct Cyc_List_List*typs;struct Cyc_Position_Segment*loc;enum 
Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct
_tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;
};struct Cyc_Absyn_Enumfield{struct _tuple1*name;struct Cyc_Absyn_Exp*tag;struct Cyc_Position_Segment*
loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct
Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple1*name;struct Cyc_Absyn_Tqual
tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;struct Cyc_Core_Opt*defn;
struct Cyc_List_List*atts;};struct Cyc_Absyn_Var_d_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_Absyn_Fn_d_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_struct{
int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};
struct Cyc_Absyn_Letv_d_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_struct{
int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*
f4;};struct Cyc_Absyn_Alias_d_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Tvar*
f2;struct Cyc_Absyn_Vardecl*f3;};struct Cyc_Absyn_Aggr_d_struct{int tag;struct Cyc_Absyn_Aggrdecl*
f1;};struct Cyc_Absyn_Datatype_d_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};
struct Cyc_Absyn_Enum_d_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_struct{
int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_struct{int
tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_struct{
int tag;struct _tuple1*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_struct{int tag;
struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_struct{
int tag;};struct Cyc_Absyn_Portoff_d_struct{int tag;};struct Cyc_Absyn_Decl{void*r;
struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_ArrayElement_struct{int tag;
struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_struct{int tag;struct
_dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_struct{
char*tag;};struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(struct Cyc_Position_Segment*);
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(struct Cyc_Position_Segment*);union Cyc_Absyn_Constraint*
Cyc_Absyn_new_conref(void*x);union Cyc_Absyn_Constraint*Cyc_Absyn_empty_conref();
void*Cyc_Absyn_conref_val(union Cyc_Absyn_Constraint*x);void*Cyc_Absyn_conref_def(
void*y,union Cyc_Absyn_Constraint*x);void*Cyc_Absyn_conref_constr(void*y,union Cyc_Absyn_Constraint*
x);extern union Cyc_Absyn_Constraint*Cyc_Absyn_true_conref;extern union Cyc_Absyn_Constraint*
Cyc_Absyn_false_conref;extern union Cyc_Absyn_Constraint*Cyc_Absyn_bounds_one_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_bounds_dyneither_conref;void*Cyc_Absyn_new_evar(
struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);
extern void*Cyc_Absyn_char_typ;extern void*Cyc_Absyn_uchar_typ;extern void*Cyc_Absyn_ushort_typ;
extern void*Cyc_Absyn_uint_typ;extern void*Cyc_Absyn_ulonglong_typ;extern void*Cyc_Absyn_schar_typ;
extern void*Cyc_Absyn_sshort_typ;extern void*Cyc_Absyn_sint_typ;extern void*Cyc_Absyn_slonglong_typ;
extern void*Cyc_Absyn_float_typ;void*Cyc_Absyn_exn_typ();extern void*Cyc_Absyn_bounds_one;
void*Cyc_Absyn_atb_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*b,union Cyc_Absyn_Constraint*
zero_term);void*Cyc_Absyn_star_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union
Cyc_Absyn_Constraint*zero_term);void*Cyc_Absyn_at_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual
tq,union Cyc_Absyn_Constraint*zero_term);void*Cyc_Absyn_dyneither_typ(void*t,void*
rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);void*Cyc_Absyn_array_typ(
void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*num_elts,union Cyc_Absyn_Constraint*
zero_term,struct Cyc_Position_Segment*ztloc);struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(
void*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct
Cyc_Absyn_Exp*);struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_add_exp(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(
void*,struct Cyc_Absyn_Exp*,int user_cast,enum Cyc_Absyn_Coercion,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*,struct Cyc_Position_Segment*);
void*Cyc_Absyn_pointer_expand(void*,int fresh_evar);int Cyc_Absyn_is_lvalue(struct
Cyc_Absyn_Exp*);struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,
struct _dyneither_ptr*);struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(
struct Cyc_Absyn_Aggrdecl*,struct _dyneither_ptr*);struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(
union Cyc_Absyn_AggrInfoU info);int Cyc_strcmp(struct _dyneither_ptr s1,struct
_dyneither_ptr s2);struct Cyc___cycFILE;extern struct Cyc___cycFILE*Cyc_stderr;
struct Cyc_Cstdio___abstractFILE;struct Cyc_String_pa_struct{int tag;struct
_dyneither_ptr f1;};struct Cyc_Int_pa_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_struct{
int tag;double f1;};struct Cyc_LongDouble_pa_struct{int tag;long double f1;};struct
Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{int tag;
unsigned long*f1;};int Cyc_fprintf(struct Cyc___cycFILE*,struct _dyneither_ptr,
struct _dyneither_ptr);struct Cyc_ShortPtr_sa_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_struct{
int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{
int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_struct{int tag;struct
_dyneither_ptr f1;};struct Cyc_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_struct{
int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int tag;struct _dyneither_ptr f1;};
struct _dyneither_ptr Cyc_vrprintf(struct _RegionHandle*,struct _dyneither_ptr,
struct _dyneither_ptr);extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_struct{
char*tag;};extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_struct{char*
tag;struct _dyneither_ptr f1;};struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;
struct Cyc_Absynpp_Params{int expand_typedefs: 1;int qvar_to_Cids: 1;int
add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int rewrite_temp_tvars: 1;int
print_all_tvars: 1;int print_all_kinds: 1;int print_all_effects: 1;int
print_using_stmts: 1;int print_externC_stmts: 1;int print_full_evars: 1;int
print_zeroterm: 1;int generate_line_directives: 1;int use_curr_namespace: 1;struct Cyc_List_List*
curr_namespace;};struct _dyneither_ptr Cyc_Absynpp_typ2string(void*);struct
_dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);struct _dyneither_ptr
Cyc_Absynpp_qvar2string(struct _tuple1*);struct Cyc_Iter_Iter{void*env;int(*next)(
void*env,void*dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;
extern char Cyc_Set_Absent[7];struct Cyc_Set_Absent_struct{char*tag;};struct Cyc_Dict_T;
struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;struct Cyc_Dict_T*
t;};extern char Cyc_Dict_Present[8];struct Cyc_Dict_Present_struct{char*tag;};
extern char Cyc_Dict_Absent[7];struct Cyc_Dict_Absent_struct{char*tag;};struct
_tuple0*Cyc_Dict_rchoose(struct _RegionHandle*r,struct Cyc_Dict_Dict d);struct
_tuple0*Cyc_Dict_rchoose(struct _RegionHandle*,struct Cyc_Dict_Dict d);struct Cyc_RgnOrder_RgnPO;
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
int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Tcenv_DatatypeRes_struct{int tag;
struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct Cyc_Tcenv_EnumRes_struct{
int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcenv_AnonEnumRes_struct{
int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcenv_Genv{struct
_RegionHandle*grgn;struct Cyc_Set_Set*namespaces;struct Cyc_Dict_Dict aggrdecls;
struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict
typedefs;struct Cyc_Dict_Dict ordinaries;struct Cyc_List_List*availables;};struct
Cyc_Tcenv_Fenv;struct Cyc_Tcenv_NotLoop_j_struct{int tag;};struct Cyc_Tcenv_CaseEnd_j_struct{
int tag;};struct Cyc_Tcenv_FnEnd_j_struct{int tag;};struct Cyc_Tcenv_Stmt_j_struct{
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;
struct Cyc_Dict_Dict ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof;};struct
_RegionHandle*Cyc_Tcenv_get_fnrgn(struct Cyc_Tcenv_Tenv*);void*Cyc_Tcenv_lookup_ordinary(
struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,struct
_tuple1*);struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl(struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*,struct _tuple1*);struct Cyc_Absyn_Datatypedecl**Cyc_Tcenv_lookup_datatypedecl(
struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,struct _tuple1*);struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_copy_tenv(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*);struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_add_local_var(struct _RegionHandle*,struct Cyc_Position_Segment*,struct
Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Vardecl*);struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(
struct Cyc_Tcenv_Tenv*);struct Cyc_Core_Opt*Cyc_Tcenv_lookup_opt_type_vars(struct
Cyc_Tcenv_Tenv*te);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_notreadctxt(struct
_RegionHandle*,struct Cyc_Tcenv_Tenv*te);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_notreadctxt(
struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te);int Cyc_Tcenv_in_notreadctxt(struct
Cyc_Tcenv_Tenv*te);struct Cyc_Absyn_Stmt*Cyc_Tcenv_get_encloser(struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_encloser(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Stmt*);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_block(struct
_RegionHandle*,struct Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*);void*Cyc_Tcenv_curr_rgn(
struct Cyc_Tcenv_Tenv*);void Cyc_Tcenv_check_rgn_accessible(struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*,void*rgn);void Cyc_Tcenv_check_effect_accessible(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void*eff);void Cyc_Tcenv_check_rgn_partial_order(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct Cyc_List_List*po);
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);void Cyc_Tcutil_terr(
struct Cyc_Position_Segment*,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);void
Cyc_Tcutil_warn(struct Cyc_Position_Segment*,struct _dyneither_ptr fmt,struct
_dyneither_ptr ap);int Cyc_Tcutil_kind_leq(enum Cyc_Absyn_Kind k1,enum Cyc_Absyn_Kind
k2);enum Cyc_Absyn_Kind Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*t,enum Cyc_Absyn_Kind
def);enum Cyc_Absyn_Kind Cyc_Tcutil_typ_kind(void*t);void*Cyc_Tcutil_compress(
void*t);void Cyc_Tcutil_unchecked_cast(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,
void*,enum Cyc_Absyn_Coercion);int Cyc_Tcutil_coerce_arg(struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Exp*,void*);int Cyc_Tcutil_coerce_assign(struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Exp*,void*);int Cyc_Tcutil_coerce_to_bool(struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Exp*);int Cyc_Tcutil_coerce_list(struct Cyc_Tcenv_Tenv*,void*,
struct Cyc_List_List*);int Cyc_Tcutil_coerce_uint_typ(struct Cyc_Tcenv_Tenv*,struct
Cyc_Absyn_Exp*);int Cyc_Tcutil_coerce_sint_typ(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*);
int Cyc_Tcutil_coerceable(void*);int Cyc_Tcutil_silent_castable(struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*,void*,void*);enum Cyc_Absyn_Coercion Cyc_Tcutil_castable(
struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,void*,void*);int Cyc_Tcutil_is_integral(
struct Cyc_Absyn_Exp*);int Cyc_Tcutil_is_numeric(struct Cyc_Absyn_Exp*);int Cyc_Tcutil_is_pointer_type(
void*t);int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*e);int Cyc_Tcutil_is_pointer_or_boxed(
void*t,int*is_dyneither_ptr);void*Cyc_Tcutil_pointer_elt_type(void*t);void*Cyc_Tcutil_pointer_region(
void*t);extern struct Cyc_Core_Opt Cyc_Tcutil_rk;extern struct Cyc_Core_Opt Cyc_Tcutil_trk;
extern struct Cyc_Core_Opt Cyc_Tcutil_ak;extern struct Cyc_Core_Opt Cyc_Tcutil_bk;
extern struct Cyc_Core_Opt Cyc_Tcutil_mk;int Cyc_Tcutil_zero_to_null(struct Cyc_Tcenv_Tenv*,
void*t,struct Cyc_Absyn_Exp*e);void*Cyc_Tcutil_max_arithmetic_type(void*,void*);
void Cyc_Tcutil_explain_failure();int Cyc_Tcutil_unify(void*,void*);void*Cyc_Tcutil_rsubstitute(
struct _RegionHandle*,struct Cyc_List_List*,void*);void*Cyc_Tcutil_fndecl2typ(
struct Cyc_Absyn_Fndecl*);struct _tuple10{struct Cyc_List_List*f1;struct
_RegionHandle*f2;};struct _tuple11{struct Cyc_Absyn_Tvar*f1;void*f2;};struct
_tuple11*Cyc_Tcutil_r_make_inst_var(struct _tuple10*,struct Cyc_Absyn_Tvar*);void
Cyc_Tcutil_check_contains_assign(struct Cyc_Absyn_Exp*);void Cyc_Tcutil_check_type(
struct Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*
bound_tvars,enum Cyc_Absyn_Kind k,int allow_evars,void*);void Cyc_Tcutil_check_nonzero_bound(
struct Cyc_Position_Segment*,union Cyc_Absyn_Constraint*);void Cyc_Tcutil_check_bound(
struct Cyc_Position_Segment*,unsigned int i,union Cyc_Absyn_Constraint*);int Cyc_Tcutil_is_bound_one(
union Cyc_Absyn_Constraint*b);struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(
struct _RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,
enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields);int Cyc_Tcutil_is_tagged_pointer_typ(
void*);int Cyc_Tcutil_is_tagged_pointer_typ_elt(void*t,void**elt_typ_dest);int Cyc_Tcutil_is_zero_pointer_typ_elt(
void*t,void**elt_typ_dest);int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*
e1,void**ptr_type,int*is_dyneither,void**elt_type);int Cyc_Tcutil_is_noalias_pointer(
void*t);int Cyc_Tcutil_is_noalias_path(struct _RegionHandle*,struct Cyc_Absyn_Exp*e);
int Cyc_Tcutil_is_noalias_pointer_or_aggr(struct _RegionHandle*,void*t);struct
_tuple12{int f1;void*f2;};struct _tuple12 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e);int Cyc_Tcutil_bits_only(void*t);int Cyc_Tcutil_is_const_exp(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e);int Cyc_Tcutil_rgn_of_pointer(void*
t,void**rgn);struct _tuple13{unsigned int f1;int f2;};struct _tuple13 Cyc_Evexp_eval_const_uint_exp(
struct Cyc_Absyn_Exp*e);int Cyc_Evexp_c_can_eval(struct Cyc_Absyn_Exp*e);int Cyc_Evexp_same_const_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);int Cyc_Evexp_okay_szofarg(void*t);
void Cyc_Tcstmt_tcStmt(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,int
new_block);struct Cyc_List_List*Cyc_Formatstr_get_format_typs(struct _RegionHandle*,
struct Cyc_Tcenv_Tenv*,struct _dyneither_ptr,struct Cyc_Position_Segment*);struct
Cyc_List_List*Cyc_Formatstr_get_scanf_typs(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,
struct _dyneither_ptr,struct Cyc_Position_Segment*);void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*,
void**,struct Cyc_Absyn_Exp*);void*Cyc_Tcexp_tcExpInitializer(struct Cyc_Tcenv_Tenv*,
void**,struct Cyc_Absyn_Exp*);struct Cyc_Tcexp_TestEnv{struct _tuple0*eq;int isTrue;
};struct Cyc_Tcexp_TestEnv Cyc_Tcexp_tcTest(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*
e,struct _dyneither_ptr msg_part);extern int Cyc_Tcexp_in_stmt_exp;struct Cyc_CfFlowInfo_VarRoot_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_CfFlowInfo_MallocPt_struct{int tag;
struct Cyc_Absyn_Exp*f1;void*f2;};struct Cyc_CfFlowInfo_InitParam_struct{int tag;
int f1;void*f2;};struct Cyc_CfFlowInfo_Place{void*root;struct Cyc_List_List*fields;
};struct Cyc_CfFlowInfo_UniquePlace{struct Cyc_CfFlowInfo_Place place;struct Cyc_List_List*
path;};enum Cyc_CfFlowInfo_InitLevel{Cyc_CfFlowInfo_NoneIL  = 0,Cyc_CfFlowInfo_ThisIL
 = 1,Cyc_CfFlowInfo_AllIL  = 2};struct _union_RelnOp_EqualConst{int tag;
unsigned int val;};struct _tuple14{struct Cyc_Absyn_Vardecl*f1;void*f2;};struct
_union_RelnOp_LessVar{int tag;struct _tuple14 val;};struct _union_RelnOp_LessNumelts{
int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_LessConst{int tag;
unsigned int val;};struct _union_RelnOp_LessEqNumelts{int tag;struct Cyc_Absyn_Vardecl*
val;};union Cyc_CfFlowInfo_RelnOp{struct _union_RelnOp_EqualConst EqualConst;struct
_union_RelnOp_LessVar LessVar;struct _union_RelnOp_LessNumelts LessNumelts;struct
_union_RelnOp_LessConst LessConst;struct _union_RelnOp_LessEqNumelts LessEqNumelts;
};struct Cyc_CfFlowInfo_Reln{struct Cyc_Absyn_Vardecl*vd;union Cyc_CfFlowInfo_RelnOp
rop;};struct Cyc_CfFlowInfo_TagCmp{enum Cyc_Absyn_Primop cmp;void*bd;};extern char
Cyc_CfFlowInfo_HasTagCmps[11];struct Cyc_CfFlowInfo_HasTagCmps_struct{char*tag;
struct Cyc_List_List*f1;};extern char Cyc_CfFlowInfo_IsZero[7];struct Cyc_CfFlowInfo_IsZero_struct{
char*tag;};extern char Cyc_CfFlowInfo_NotZero[8];struct Cyc_CfFlowInfo_NotZero_struct{
char*tag;struct Cyc_List_List*f1;};extern char Cyc_CfFlowInfo_UnknownZ[9];struct Cyc_CfFlowInfo_UnknownZ_struct{
char*tag;struct Cyc_List_List*f1;};struct _union_AbsLVal_PlaceL{int tag;struct Cyc_CfFlowInfo_Place*
val;};struct _union_AbsLVal_UnknownL{int tag;int val;};union Cyc_CfFlowInfo_AbsLVal{
struct _union_AbsLVal_PlaceL PlaceL;struct _union_AbsLVal_UnknownL UnknownL;};struct
Cyc_CfFlowInfo_Zero_struct{int tag;};struct Cyc_CfFlowInfo_NotZeroAll_struct{int
tag;};struct Cyc_CfFlowInfo_NotZeroThis_struct{int tag;};struct Cyc_CfFlowInfo_UnknownR_struct{
int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_Esc_struct{int tag;
enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_AddressOf_struct{int tag;
struct Cyc_CfFlowInfo_Place*f1;};struct Cyc_CfFlowInfo_TagCmps_struct{int tag;
struct Cyc_List_List*f1;};struct Cyc_CfFlowInfo_Aggregate_struct{int tag;int f1;
struct _dyneither_ptr f2;};struct Cyc_CfFlowInfo_ConsumeInfo{struct Cyc_Dict_Dict
consumed;struct Cyc_List_List*may_consume;};struct _union_FlowInfo_BottomFL{int tag;
int val;};struct _tuple15{struct Cyc_Dict_Dict f1;struct Cyc_List_List*f2;struct Cyc_CfFlowInfo_ConsumeInfo
f3;};struct _union_FlowInfo_ReachableFL{int tag;struct _tuple15 val;};union Cyc_CfFlowInfo_FlowInfo{
struct _union_FlowInfo_BottomFL BottomFL;struct _union_FlowInfo_ReachableFL
ReachableFL;};struct Cyc_CfFlowInfo_FlowEnv{struct _RegionHandle*r;void*zero;void*
notzeroall;void*notzerothis;void*unknown_none;void*unknown_this;void*unknown_all;
void*esc_none;void*esc_this;void*esc_all;struct Cyc_Dict_Dict mt_flowdict;struct
Cyc_Dict_Dict mt_place_set;struct Cyc_CfFlowInfo_Place*dummy_place;};struct Cyc_CfFlowInfo_UniqueRgn_k_struct{
int tag;};struct Cyc_CfFlowInfo_Region_k_struct{int tag;struct Cyc_Absyn_Tvar*f1;};
void Cyc_NewControlFlow_set_encloser(struct Cyc_Absyn_Stmt*enclosee,struct Cyc_Absyn_Stmt*
encloser);static void*Cyc_Tcexp_expr_err(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct _dyneither_ptr msg,struct _dyneither_ptr ap){({void*_tmp0=0;
Cyc_Tcutil_terr(loc,(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,msg,
ap),_tag_dyneither(_tmp0,sizeof(void*),0));});if(topt == 0)return Cyc_Absyn_wildtyp(
Cyc_Tcenv_lookup_opt_type_vars(te));else{return*topt;}}static void Cyc_Tcexp_resolve_unknown_id(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct _tuple1*q){struct
_handler_cons _tmp1;_push_handler(& _tmp1);{int _tmp3=0;if(setjmp(_tmp1.handler))
_tmp3=1;if(!_tmp3){{struct _RegionHandle*_tmp4=Cyc_Tcenv_get_fnrgn(te);void*_tmp5=
Cyc_Tcenv_lookup_ordinary(_tmp4,te,e->loc,q);void*_tmp7;struct Cyc_Absyn_Enumdecl*
_tmp9;struct Cyc_Absyn_Enumfield*_tmpA;void*_tmpC;struct Cyc_Absyn_Enumfield*_tmpD;
struct Cyc_Absyn_Datatypedecl*_tmpF;struct Cyc_Absyn_Datatypefield*_tmp10;_LL1: {
struct Cyc_Tcenv_VarRes_struct*_tmp6=(struct Cyc_Tcenv_VarRes_struct*)_tmp5;if(
_tmp6->tag != 0)goto _LL3;else{_tmp7=(void*)_tmp6->f1;}}_LL2: e->r=(void*)({struct
Cyc_Absyn_Var_e_struct*_tmp12=_cycalloc(sizeof(*_tmp12));_tmp12[0]=({struct Cyc_Absyn_Var_e_struct
_tmp13;_tmp13.tag=1;_tmp13.f1=q;_tmp13.f2=(void*)_tmp7;_tmp13;});_tmp12;});goto
_LL0;_LL3: {struct Cyc_Tcenv_EnumRes_struct*_tmp8=(struct Cyc_Tcenv_EnumRes_struct*)
_tmp5;if(_tmp8->tag != 3)goto _LL5;else{_tmp9=_tmp8->f1;_tmpA=_tmp8->f2;}}_LL4: e->r=(
void*)({struct Cyc_Absyn_Enum_e_struct*_tmp14=_cycalloc(sizeof(*_tmp14));_tmp14[0]=({
struct Cyc_Absyn_Enum_e_struct _tmp15;_tmp15.tag=33;_tmp15.f1=q;_tmp15.f2=(struct
Cyc_Absyn_Enumdecl*)_tmp9;_tmp15.f3=(struct Cyc_Absyn_Enumfield*)_tmpA;_tmp15;});
_tmp14;});goto _LL0;_LL5: {struct Cyc_Tcenv_AnonEnumRes_struct*_tmpB=(struct Cyc_Tcenv_AnonEnumRes_struct*)
_tmp5;if(_tmpB->tag != 4)goto _LL7;else{_tmpC=(void*)_tmpB->f1;_tmpD=_tmpB->f2;}}
_LL6: e->r=(void*)({struct Cyc_Absyn_AnonEnum_e_struct*_tmp16=_cycalloc(sizeof(*
_tmp16));_tmp16[0]=({struct Cyc_Absyn_AnonEnum_e_struct _tmp17;_tmp17.tag=34;
_tmp17.f1=q;_tmp17.f2=(void*)_tmpC;_tmp17.f3=(struct Cyc_Absyn_Enumfield*)_tmpD;
_tmp17;});_tmp16;});goto _LL0;_LL7: {struct Cyc_Tcenv_DatatypeRes_struct*_tmpE=(
struct Cyc_Tcenv_DatatypeRes_struct*)_tmp5;if(_tmpE->tag != 2)goto _LL9;else{_tmpF=
_tmpE->f1;_tmp10=_tmpE->f2;}}_LL8: e->r=(void*)({struct Cyc_Absyn_Datatype_e_struct*
_tmp18=_cycalloc(sizeof(*_tmp18));_tmp18[0]=({struct Cyc_Absyn_Datatype_e_struct
_tmp19;_tmp19.tag=32;_tmp19.f1=0;_tmp19.f2=_tmpF;_tmp19.f3=_tmp10;_tmp19;});
_tmp18;});goto _LL0;_LL9: {struct Cyc_Tcenv_AggrRes_struct*_tmp11=(struct Cyc_Tcenv_AggrRes_struct*)
_tmp5;if(_tmp11->tag != 1)goto _LL0;}_LLA:({struct Cyc_String_pa_struct _tmp1C;
_tmp1C.tag=0;_tmp1C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
q));{void*_tmp1A[1]={& _tmp1C};Cyc_Tcutil_terr(e->loc,({const char*_tmp1B="bad occurrence of type name %s";
_tag_dyneither(_tmp1B,sizeof(char),31);}),_tag_dyneither(_tmp1A,sizeof(void*),1));}});
e->r=(void*)({struct Cyc_Absyn_Var_e_struct*_tmp1D=_cycalloc(sizeof(*_tmp1D));
_tmp1D[0]=({struct Cyc_Absyn_Var_e_struct _tmp1E;_tmp1E.tag=1;_tmp1E.f1=q;_tmp1E.f2=(
void*)((void*)& Cyc_Absyn_Unresolved_b_val);_tmp1E;});_tmp1D;});goto _LL0;_LL0:;};
_pop_handler();}else{void*_tmp2=(void*)_exn_thrown;void*_tmp20=_tmp2;_LLC: {
struct Cyc_Dict_Absent_struct*_tmp21=(struct Cyc_Dict_Absent_struct*)_tmp20;if(
_tmp21->tag != Cyc_Dict_Absent)goto _LLE;}_LLD: e->r=(void*)({struct Cyc_Absyn_Var_e_struct*
_tmp22=_cycalloc(sizeof(*_tmp22));_tmp22[0]=({struct Cyc_Absyn_Var_e_struct _tmp23;
_tmp23.tag=1;_tmp23.f1=q;_tmp23.f2=(void*)((void*)& Cyc_Absyn_Unresolved_b_val);
_tmp23;});_tmp22;});goto _LLB;_LLE:;_LLF:(void)_throw(_tmp20);_LLB:;}}}struct
_tuple16{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};static struct _tuple16*
Cyc_Tcexp_make_struct_arg(struct Cyc_Absyn_Exp*e){return({struct _tuple16*_tmp24=
_cycalloc(sizeof(*_tmp24));_tmp24->f1=0;_tmp24->f2=e;_tmp24;});}static void Cyc_Tcexp_resolve_unknown_fn(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*
es){void*_tmp25=e1->r;struct _tuple1*_tmp27;_LL11: {struct Cyc_Absyn_UnknownId_e_struct*
_tmp26=(struct Cyc_Absyn_UnknownId_e_struct*)_tmp25;if(_tmp26->tag != 2)goto _LL13;
else{_tmp27=_tmp26->f1;}}_LL12: {struct _handler_cons _tmp28;_push_handler(& _tmp28);{
int _tmp2A=0;if(setjmp(_tmp28.handler))_tmp2A=1;if(!_tmp2A){{struct _RegionHandle*
_tmp2B=Cyc_Tcenv_get_fnrgn(te);void*_tmp2C=Cyc_Tcenv_lookup_ordinary(_tmp2B,te,
e1->loc,_tmp27);void*_tmp2E;struct Cyc_Absyn_Datatypedecl*_tmp30;struct Cyc_Absyn_Datatypefield*
_tmp31;struct Cyc_Absyn_Aggrdecl*_tmp33;_LL16: {struct Cyc_Tcenv_VarRes_struct*
_tmp2D=(struct Cyc_Tcenv_VarRes_struct*)_tmp2C;if(_tmp2D->tag != 0)goto _LL18;else{
_tmp2E=(void*)_tmp2D->f1;}}_LL17: e->r=(void*)({struct Cyc_Absyn_FnCall_e_struct*
_tmp36=_cycalloc(sizeof(*_tmp36));_tmp36[0]=({struct Cyc_Absyn_FnCall_e_struct
_tmp37;_tmp37.tag=11;_tmp37.f1=e1;_tmp37.f2=es;_tmp37.f3=0;_tmp37;});_tmp36;});
_npop_handler(0);return;_LL18: {struct Cyc_Tcenv_DatatypeRes_struct*_tmp2F=(
struct Cyc_Tcenv_DatatypeRes_struct*)_tmp2C;if(_tmp2F->tag != 2)goto _LL1A;else{
_tmp30=_tmp2F->f1;_tmp31=_tmp2F->f2;}}_LL19: if(_tmp31->typs == 0)({struct Cyc_String_pa_struct
_tmp3A;_tmp3A.tag=0;_tmp3A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp31->name));{void*_tmp38[1]={& _tmp3A};Cyc_Tcutil_terr(e->loc,({const char*
_tmp39="%s is a constant, not a function";_tag_dyneither(_tmp39,sizeof(char),33);}),
_tag_dyneither(_tmp38,sizeof(void*),1));}});e->r=(void*)({struct Cyc_Absyn_Datatype_e_struct*
_tmp3B=_cycalloc(sizeof(*_tmp3B));_tmp3B[0]=({struct Cyc_Absyn_Datatype_e_struct
_tmp3C;_tmp3C.tag=32;_tmp3C.f1=es;_tmp3C.f2=_tmp30;_tmp3C.f3=_tmp31;_tmp3C;});
_tmp3B;});_npop_handler(0);return;_LL1A: {struct Cyc_Tcenv_AggrRes_struct*_tmp32=(
struct Cyc_Tcenv_AggrRes_struct*)_tmp2C;if(_tmp32->tag != 1)goto _LL1C;else{_tmp33=
_tmp32->f1;}}_LL1B: {struct Cyc_List_List*_tmp3D=((struct Cyc_List_List*(*)(struct
_tuple16*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcexp_make_struct_arg,
es);e->r=(void*)({struct Cyc_Absyn_Aggregate_e_struct*_tmp3E=_cycalloc(sizeof(*
_tmp3E));_tmp3E[0]=({struct Cyc_Absyn_Aggregate_e_struct _tmp3F;_tmp3F.tag=30;
_tmp3F.f1=_tmp33->name;_tmp3F.f2=0;_tmp3F.f3=_tmp3D;_tmp3F.f4=(struct Cyc_Absyn_Aggrdecl*)
_tmp33;_tmp3F;});_tmp3E;});_npop_handler(0);return;}_LL1C: {struct Cyc_Tcenv_AnonEnumRes_struct*
_tmp34=(struct Cyc_Tcenv_AnonEnumRes_struct*)_tmp2C;if(_tmp34->tag != 4)goto _LL1E;}
_LL1D: goto _LL1F;_LL1E: {struct Cyc_Tcenv_EnumRes_struct*_tmp35=(struct Cyc_Tcenv_EnumRes_struct*)
_tmp2C;if(_tmp35->tag != 3)goto _LL15;}_LL1F:({struct Cyc_String_pa_struct _tmp42;
_tmp42.tag=0;_tmp42.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp27));{void*_tmp40[1]={& _tmp42};Cyc_Tcutil_terr(e->loc,({const char*_tmp41="%s is an enum constructor, not a function";
_tag_dyneither(_tmp41,sizeof(char),42);}),_tag_dyneither(_tmp40,sizeof(void*),1));}});
_npop_handler(0);return;_LL15:;};_pop_handler();}else{void*_tmp29=(void*)
_exn_thrown;void*_tmp44=_tmp29;_LL21: {struct Cyc_Dict_Absent_struct*_tmp45=(
struct Cyc_Dict_Absent_struct*)_tmp44;if(_tmp45->tag != Cyc_Dict_Absent)goto _LL23;}
_LL22: e->r=(void*)({struct Cyc_Absyn_FnCall_e_struct*_tmp46=_cycalloc(sizeof(*
_tmp46));_tmp46[0]=({struct Cyc_Absyn_FnCall_e_struct _tmp47;_tmp47.tag=11;_tmp47.f1=
e1;_tmp47.f2=es;_tmp47.f3=0;_tmp47;});_tmp46;});return;_LL23:;_LL24:(void)_throw(
_tmp44);_LL20:;}}}_LL13:;_LL14: e->r=(void*)({struct Cyc_Absyn_FnCall_e_struct*
_tmp48=_cycalloc(sizeof(*_tmp48));_tmp48[0]=({struct Cyc_Absyn_FnCall_e_struct
_tmp49;_tmp49.tag=11;_tmp49.f1=e1;_tmp49.f2=es;_tmp49.f3=0;_tmp49;});_tmp48;});
return;_LL10:;}static void Cyc_Tcexp_resolve_unresolved_mem(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*
des){if(topt == 0){e->r=(void*)({struct Cyc_Absyn_Array_e_struct*_tmp4A=_cycalloc(
sizeof(*_tmp4A));_tmp4A[0]=({struct Cyc_Absyn_Array_e_struct _tmp4B;_tmp4B.tag=28;
_tmp4B.f1=des;_tmp4B;});_tmp4A;});return;}{void*t=*topt;void*_tmp4C=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_AggrInfo _tmp4E;union Cyc_Absyn_AggrInfoU _tmp4F;struct Cyc_Absyn_ArrayInfo
_tmp51;void*_tmp52;struct Cyc_Absyn_Tqual _tmp53;_LL26: {struct Cyc_Absyn_AggrType_struct*
_tmp4D=(struct Cyc_Absyn_AggrType_struct*)_tmp4C;if(_tmp4D->tag != 12)goto _LL28;
else{_tmp4E=_tmp4D->f1;_tmp4F=_tmp4E.aggr_info;}}_LL27:{union Cyc_Absyn_AggrInfoU
_tmp55=_tmp4F;struct _tuple3 _tmp56;struct Cyc_Absyn_Aggrdecl**_tmp57;struct Cyc_Absyn_Aggrdecl*
_tmp58;_LL2F: if((_tmp55.UnknownAggr).tag != 1)goto _LL31;_tmp56=(struct _tuple3)(
_tmp55.UnknownAggr).val;_LL30:({void*_tmp59=0;((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp5A="struct type not properly set";
_tag_dyneither(_tmp5A,sizeof(char),29);}),_tag_dyneither(_tmp59,sizeof(void*),0));});
_LL31: if((_tmp55.KnownAggr).tag != 2)goto _LL2E;_tmp57=(struct Cyc_Absyn_Aggrdecl**)(
_tmp55.KnownAggr).val;_tmp58=*_tmp57;_LL32: e->r=(void*)({struct Cyc_Absyn_Aggregate_e_struct*
_tmp5B=_cycalloc(sizeof(*_tmp5B));_tmp5B[0]=({struct Cyc_Absyn_Aggregate_e_struct
_tmp5C;_tmp5C.tag=30;_tmp5C.f1=_tmp58->name;_tmp5C.f2=0;_tmp5C.f3=des;_tmp5C.f4=(
struct Cyc_Absyn_Aggrdecl*)_tmp58;_tmp5C;});_tmp5B;});_LL2E:;}goto _LL25;_LL28: {
struct Cyc_Absyn_ArrayType_struct*_tmp50=(struct Cyc_Absyn_ArrayType_struct*)
_tmp4C;if(_tmp50->tag != 9)goto _LL2A;else{_tmp51=_tmp50->f1;_tmp52=_tmp51.elt_type;
_tmp53=_tmp51.tq;}}_LL29: e->r=(void*)({struct Cyc_Absyn_Array_e_struct*_tmp5D=
_cycalloc(sizeof(*_tmp5D));_tmp5D[0]=({struct Cyc_Absyn_Array_e_struct _tmp5E;
_tmp5E.tag=28;_tmp5E.f1=des;_tmp5E;});_tmp5D;});goto _LL25;_LL2A: {struct Cyc_Absyn_AnonAggrType_struct*
_tmp54=(struct Cyc_Absyn_AnonAggrType_struct*)_tmp4C;if(_tmp54->tag != 13)goto
_LL2C;}_LL2B: e->r=(void*)({struct Cyc_Absyn_AnonStruct_e_struct*_tmp5F=_cycalloc(
sizeof(*_tmp5F));_tmp5F[0]=({struct Cyc_Absyn_AnonStruct_e_struct _tmp60;_tmp60.tag=
31;_tmp60.f1=(void*)t;_tmp60.f2=des;_tmp60;});_tmp5F;});goto _LL25;_LL2C:;_LL2D: e->r=(
void*)({struct Cyc_Absyn_Array_e_struct*_tmp61=_cycalloc(sizeof(*_tmp61));_tmp61[
0]=({struct Cyc_Absyn_Array_e_struct _tmp62;_tmp62.tag=28;_tmp62.f1=des;_tmp62;});
_tmp61;});goto _LL25;_LL25:;}}static void Cyc_Tcexp_tcExpNoInst(struct Cyc_Tcenv_Tenv*
te,void**topt,struct Cyc_Absyn_Exp*e);static void*Cyc_Tcexp_tcExpNoPromote(struct
Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e);static void Cyc_Tcexp_tcExpList(
struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*es){for(0;es != 0;es=es->tl){Cyc_Tcexp_tcExp(
te,0,(struct Cyc_Absyn_Exp*)es->hd);}}struct Cyc_Tcexp_TestEnv Cyc_Tcexp_tcTest(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr msg_part){Cyc_Tcutil_check_contains_assign(
e);Cyc_Tcexp_tcExp(te,(void**)& Cyc_Absyn_sint_typ,e);if(!Cyc_Tcutil_coerce_to_bool(
te,e))({struct Cyc_String_pa_struct _tmp66;_tmp66.tag=0;_tmp66.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v));{struct Cyc_String_pa_struct _tmp65;_tmp65.tag=0;_tmp65.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)msg_part);{void*_tmp63[2]={& _tmp65,&
_tmp66};Cyc_Tcutil_terr(e->loc,({const char*_tmp64="test of %s has type %s instead of integral or * type";
_tag_dyneither(_tmp64,sizeof(char),53);}),_tag_dyneither(_tmp63,sizeof(void*),2));}}});{
void*_tmp67=e->r;enum Cyc_Absyn_Primop _tmp69;struct Cyc_List_List*_tmp6A;_LL34: {
struct Cyc_Absyn_Primop_e_struct*_tmp68=(struct Cyc_Absyn_Primop_e_struct*)_tmp67;
if(_tmp68->tag != 3)goto _LL36;else{_tmp69=_tmp68->f1;_tmp6A=_tmp68->f2;}}_LL35:
if(_tmp69 == Cyc_Absyn_Eq  || _tmp69 == Cyc_Absyn_Neq){struct _tuple0 _tmp6C=({struct
_tuple0 _tmp6B;_tmp6B.f1=(void*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp6A))->hd)->topt))->v;_tmp6B.f2=(void*)((
struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(((struct Cyc_List_List*)_check_null(_tmp6A))->tl))->hd)->topt))->v;
_tmp6B;});void*_tmp6D;void*_tmp6F;void*_tmp70;void*_tmp72;_LL39: _tmp6D=_tmp6C.f1;{
struct Cyc_Absyn_RgnHandleType_struct*_tmp6E=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp6D;if(_tmp6E->tag != 16)goto _LL3B;else{_tmp6F=(void*)_tmp6E->f1;}}_tmp70=
_tmp6C.f2;{struct Cyc_Absyn_RgnHandleType_struct*_tmp71=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp70;if(_tmp71->tag != 16)goto _LL3B;else{_tmp72=(void*)_tmp71->f1;}}_LL3A:
return({struct Cyc_Tcexp_TestEnv _tmp73;_tmp73.eq=({struct _tuple0*_tmp74=
_region_malloc(Cyc_Tcenv_get_fnrgn(te),sizeof(*_tmp74));_tmp74->f1=_tmp6F;_tmp74->f2=
_tmp72;_tmp74;});_tmp73.isTrue=_tmp69 == Cyc_Absyn_Eq;_tmp73;});_LL3B:;_LL3C: goto
_LL38;_LL38:;}goto _LL33;_LL36:;_LL37: goto _LL33;_LL33:;}return({struct Cyc_Tcexp_TestEnv
_tmp75;_tmp75.eq=0;_tmp75.isTrue=0;_tmp75;});}static void*Cyc_Tcexp_tcConst(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,union Cyc_Absyn_Cnst*
c,struct Cyc_Absyn_Exp*e){void*t;{union Cyc_Absyn_Cnst _tmp76=*((union Cyc_Absyn_Cnst*)
_check_null(c));struct _tuple4 _tmp77;enum Cyc_Absyn_Sign _tmp78;struct _tuple4
_tmp79;enum Cyc_Absyn_Sign _tmp7A;struct _tuple4 _tmp7B;enum Cyc_Absyn_Sign _tmp7C;
struct _tuple5 _tmp7D;enum Cyc_Absyn_Sign _tmp7E;struct _tuple7 _tmp7F;enum Cyc_Absyn_Sign
_tmp80;struct _dyneither_ptr _tmp81;struct _tuple6 _tmp82;enum Cyc_Absyn_Sign _tmp83;
int _tmp84;struct _dyneither_ptr _tmp85;int _tmp86;_LL3E: if((_tmp76.Char_c).tag != 2)
goto _LL40;_tmp77=(struct _tuple4)(_tmp76.Char_c).val;_tmp78=_tmp77.f1;if(_tmp78 != 
Cyc_Absyn_Signed)goto _LL40;_LL3F: t=Cyc_Absyn_schar_typ;goto _LL3D;_LL40: if((
_tmp76.Char_c).tag != 2)goto _LL42;_tmp79=(struct _tuple4)(_tmp76.Char_c).val;
_tmp7A=_tmp79.f1;if(_tmp7A != Cyc_Absyn_Unsigned)goto _LL42;_LL41: t=Cyc_Absyn_uchar_typ;
goto _LL3D;_LL42: if((_tmp76.Char_c).tag != 2)goto _LL44;_tmp7B=(struct _tuple4)(
_tmp76.Char_c).val;_tmp7C=_tmp7B.f1;if(_tmp7C != Cyc_Absyn_None)goto _LL44;_LL43: t=
Cyc_Absyn_char_typ;goto _LL3D;_LL44: if((_tmp76.Short_c).tag != 3)goto _LL46;_tmp7D=(
struct _tuple5)(_tmp76.Short_c).val;_tmp7E=_tmp7D.f1;_LL45: t=_tmp7E == Cyc_Absyn_Unsigned?
Cyc_Absyn_ushort_typ: Cyc_Absyn_sshort_typ;goto _LL3D;_LL46: if((_tmp76.LongLong_c).tag
!= 5)goto _LL48;_tmp7F=(struct _tuple7)(_tmp76.LongLong_c).val;_tmp80=_tmp7F.f1;
_LL47: t=_tmp80 == Cyc_Absyn_Unsigned?Cyc_Absyn_ulonglong_typ: Cyc_Absyn_slonglong_typ;
goto _LL3D;_LL48: if((_tmp76.Float_c).tag != 6)goto _LL4A;_tmp81=(struct
_dyneither_ptr)(_tmp76.Float_c).val;_LL49: t=Cyc_Absyn_float_typ;goto _LL3D;_LL4A:
if((_tmp76.Int_c).tag != 4)goto _LL4C;_tmp82=(struct _tuple6)(_tmp76.Int_c).val;
_tmp83=_tmp82.f1;_tmp84=_tmp82.f2;_LL4B: if(topt == 0)t=_tmp83 == Cyc_Absyn_Unsigned?
Cyc_Absyn_uint_typ: Cyc_Absyn_sint_typ;else{void*_tmp87=Cyc_Tcutil_compress(*topt);
enum Cyc_Absyn_Sign _tmp89;enum Cyc_Absyn_Size_of _tmp8A;enum Cyc_Absyn_Sign _tmp8C;
enum Cyc_Absyn_Size_of _tmp8D;enum Cyc_Absyn_Sign _tmp8F;enum Cyc_Absyn_Size_of
_tmp90;enum Cyc_Absyn_Sign _tmp92;enum Cyc_Absyn_Size_of _tmp93;void*_tmp96;_LL51: {
struct Cyc_Absyn_IntType_struct*_tmp88=(struct Cyc_Absyn_IntType_struct*)_tmp87;
if(_tmp88->tag != 6)goto _LL53;else{_tmp89=_tmp88->f1;_tmp8A=_tmp88->f2;if(_tmp8A
!= Cyc_Absyn_Char_sz)goto _LL53;}}_LL52: switch(_tmp89){case Cyc_Absyn_Unsigned:
_LL5F: t=Cyc_Absyn_uchar_typ;break;case Cyc_Absyn_Signed: _LL60: t=Cyc_Absyn_schar_typ;
break;case Cyc_Absyn_None: _LL61: t=Cyc_Absyn_char_typ;break;}*c=Cyc_Absyn_Char_c(
_tmp89,(char)_tmp84);goto _LL50;_LL53: {struct Cyc_Absyn_IntType_struct*_tmp8B=(
struct Cyc_Absyn_IntType_struct*)_tmp87;if(_tmp8B->tag != 6)goto _LL55;else{_tmp8C=
_tmp8B->f1;_tmp8D=_tmp8B->f2;if(_tmp8D != Cyc_Absyn_Short_sz)goto _LL55;}}_LL54: t=
_tmp8C == Cyc_Absyn_Unsigned?Cyc_Absyn_ushort_typ: Cyc_Absyn_sshort_typ;*c=Cyc_Absyn_Short_c(
_tmp8C,(short)_tmp84);goto _LL50;_LL55: {struct Cyc_Absyn_IntType_struct*_tmp8E=(
struct Cyc_Absyn_IntType_struct*)_tmp87;if(_tmp8E->tag != 6)goto _LL57;else{_tmp8F=
_tmp8E->f1;_tmp90=_tmp8E->f2;if(_tmp90 != Cyc_Absyn_Int_sz)goto _LL57;}}_LL56: t=
_tmp8F == Cyc_Absyn_Unsigned?Cyc_Absyn_uint_typ: Cyc_Absyn_sint_typ;goto _LL50;
_LL57: {struct Cyc_Absyn_IntType_struct*_tmp91=(struct Cyc_Absyn_IntType_struct*)
_tmp87;if(_tmp91->tag != 6)goto _LL59;else{_tmp92=_tmp91->f1;_tmp93=_tmp91->f2;if(
_tmp93 != Cyc_Absyn_Long_sz)goto _LL59;}}_LL58: t=_tmp92 == Cyc_Absyn_Unsigned?Cyc_Absyn_uint_typ:
Cyc_Absyn_sint_typ;goto _LL50;_LL59:{struct Cyc_Absyn_PointerType_struct*_tmp94=(
struct Cyc_Absyn_PointerType_struct*)_tmp87;if(_tmp94->tag != 5)goto _LL5B;}if(!(
_tmp84 == 0))goto _LL5B;_LL5A: e->r=(void*)({struct Cyc_Absyn_Const_e_struct*_tmp97=
_cycalloc(sizeof(*_tmp97));_tmp97[0]=({struct Cyc_Absyn_Const_e_struct _tmp98;
_tmp98.tag=0;_tmp98.f1=Cyc_Absyn_Null_c;_tmp98;});_tmp97;});{struct Cyc_List_List*
_tmp99=Cyc_Tcenv_lookup_type_vars(te);t=(void*)({struct Cyc_Absyn_PointerType_struct*
_tmp9A=_cycalloc(sizeof(*_tmp9A));_tmp9A[0]=({struct Cyc_Absyn_PointerType_struct
_tmp9B;_tmp9B.tag=5;_tmp9B.f1=({struct Cyc_Absyn_PtrInfo _tmp9C;_tmp9C.elt_typ=Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_ak,({struct Cyc_Core_Opt*_tmp9F=_cycalloc(sizeof(*
_tmp9F));_tmp9F->v=_tmp99;_tmp9F;}));_tmp9C.elt_tq=Cyc_Absyn_empty_tqual(0);
_tmp9C.ptr_atts=({struct Cyc_Absyn_PtrAtts _tmp9D;_tmp9D.rgn=Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_trk,({struct Cyc_Core_Opt*_tmp9E=_cycalloc(
sizeof(*_tmp9E));_tmp9E->v=_tmp99;_tmp9E;}));_tmp9D.nullable=Cyc_Absyn_true_conref;
_tmp9D.bounds=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();_tmp9D.zero_term=((
union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();_tmp9D.ptrloc=0;_tmp9D;});
_tmp9C;});_tmp9B;});_tmp9A;});goto _LL50;}_LL5B: {struct Cyc_Absyn_TagType_struct*
_tmp95=(struct Cyc_Absyn_TagType_struct*)_tmp87;if(_tmp95->tag != 20)goto _LL5D;
else{_tmp96=(void*)_tmp95->f1;}}_LL5C: {struct Cyc_Absyn_ValueofType_struct*
_tmpA0=({struct Cyc_Absyn_ValueofType_struct*_tmpA7=_cycalloc(sizeof(*_tmpA7));
_tmpA7[0]=({struct Cyc_Absyn_ValueofType_struct _tmpA8;_tmpA8.tag=19;_tmpA8.f1=Cyc_Absyn_uint_exp((
unsigned int)_tmp84,0);_tmpA8;});_tmpA7;});if(!Cyc_Tcutil_unify(_tmp96,(void*)
_tmpA0)){({struct Cyc_String_pa_struct _tmpA4;_tmpA4.tag=0;_tmpA4.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_tmpA0));{
struct Cyc_String_pa_struct _tmpA3;_tmpA3.tag=0;_tmpA3.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp96));{void*_tmpA1[2]={& _tmpA3,&
_tmpA4};Cyc_Tcutil_terr(loc,({const char*_tmpA2="expecting %s but found %s";
_tag_dyneither(_tmpA2,sizeof(char),26);}),_tag_dyneither(_tmpA1,sizeof(void*),2));}}});
Cyc_Tcutil_explain_failure();}t=(void*)({struct Cyc_Absyn_TagType_struct*_tmpA5=
_cycalloc(sizeof(*_tmpA5));_tmpA5[0]=({struct Cyc_Absyn_TagType_struct _tmpA6;
_tmpA6.tag=20;_tmpA6.f1=(void*)((void*)_tmpA0);_tmpA6;});_tmpA5;});goto _LL50;}
_LL5D:;_LL5E: t=_tmp83 == Cyc_Absyn_Unsigned?Cyc_Absyn_uint_typ: Cyc_Absyn_sint_typ;
goto _LL50;_LL50:;}goto _LL3D;_LL4C: if((_tmp76.String_c).tag != 7)goto _LL4E;_tmp85=(
struct _dyneither_ptr)(_tmp76.String_c).val;_LL4D: {int len=(int)
_get_dyneither_size(_tmp85,sizeof(char));struct Cyc_Absyn_Exp*elen=Cyc_Absyn_const_exp(
Cyc_Absyn_Int_c(Cyc_Absyn_Unsigned,len),loc);elen->topt=({struct Cyc_Core_Opt*
_tmpA9=_cycalloc(sizeof(*_tmpA9));_tmpA9->v=Cyc_Absyn_uint_typ;_tmpA9;});t=Cyc_Absyn_atb_typ(
Cyc_Absyn_char_typ,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Absyn_const_tqual(0),(void*)({
struct Cyc_Absyn_Upper_b_struct*_tmpAA=_cycalloc(sizeof(*_tmpAA));_tmpAA[0]=({
struct Cyc_Absyn_Upper_b_struct _tmpAB;_tmpAB.tag=1;_tmpAB.f1=elen;_tmpAB;});
_tmpAA;}),Cyc_Absyn_true_conref);if(topt != 0){void*_tmpAC=Cyc_Tcutil_compress(*
topt);struct Cyc_Absyn_ArrayInfo _tmpAE;struct Cyc_Absyn_Tqual _tmpAF;_LL64: {struct
Cyc_Absyn_ArrayType_struct*_tmpAD=(struct Cyc_Absyn_ArrayType_struct*)_tmpAC;if(
_tmpAD->tag != 9)goto _LL66;else{_tmpAE=_tmpAD->f1;_tmpAF=_tmpAE.tq;}}_LL65: return
Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,_tmpAF,(struct Cyc_Absyn_Exp*)elen,((union
Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),0);_LL66: {struct Cyc_Absyn_PointerType_struct*
_tmpB0=(struct Cyc_Absyn_PointerType_struct*)_tmpAC;if(_tmpB0->tag != 5)goto _LL68;}
_LL67: if(!Cyc_Tcutil_unify(*topt,t) && Cyc_Tcutil_silent_castable(te,loc,t,*topt)){
e->topt=({struct Cyc_Core_Opt*_tmpB1=_cycalloc(sizeof(*_tmpB1));_tmpB1->v=t;
_tmpB1;});Cyc_Tcutil_unchecked_cast(te,e,*topt,Cyc_Absyn_Other_coercion);t=*topt;}
else{t=Cyc_Absyn_atb_typ(Cyc_Absyn_char_typ,Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_rk,Cyc_Tcenv_lookup_opt_type_vars(te)),Cyc_Absyn_const_tqual(0),(void*)({
struct Cyc_Absyn_Upper_b_struct*_tmpB2=_cycalloc(sizeof(*_tmpB2));_tmpB2[0]=({
struct Cyc_Absyn_Upper_b_struct _tmpB3;_tmpB3.tag=1;_tmpB3.f1=elen;_tmpB3;});
_tmpB2;}),Cyc_Absyn_true_conref);if(!Cyc_Tcutil_unify(*topt,t) && Cyc_Tcutil_silent_castable(
te,loc,t,*topt)){e->topt=({struct Cyc_Core_Opt*_tmpB4=_cycalloc(sizeof(*_tmpB4));
_tmpB4->v=t;_tmpB4;});Cyc_Tcutil_unchecked_cast(te,e,*topt,Cyc_Absyn_Other_coercion);
t=*topt;}}goto _LL63;_LL68:;_LL69: goto _LL63;_LL63:;}return t;}_LL4E: if((_tmp76.Null_c).tag
!= 1)goto _LL3D;_tmp86=(int)(_tmp76.Null_c).val;_LL4F: {struct Cyc_List_List*
_tmpB5=Cyc_Tcenv_lookup_type_vars(te);t=(void*)({struct Cyc_Absyn_PointerType_struct*
_tmpB6=_cycalloc(sizeof(*_tmpB6));_tmpB6[0]=({struct Cyc_Absyn_PointerType_struct
_tmpB7;_tmpB7.tag=5;_tmpB7.f1=({struct Cyc_Absyn_PtrInfo _tmpB8;_tmpB8.elt_typ=Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_ak,({struct Cyc_Core_Opt*_tmpBB=_cycalloc(sizeof(*
_tmpBB));_tmpBB->v=_tmpB5;_tmpBB;}));_tmpB8.elt_tq=Cyc_Absyn_empty_tqual(0);
_tmpB8.ptr_atts=({struct Cyc_Absyn_PtrAtts _tmpB9;_tmpB9.rgn=Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_trk,({struct Cyc_Core_Opt*_tmpBA=_cycalloc(
sizeof(*_tmpBA));_tmpBA->v=_tmpB5;_tmpBA;}));_tmpB9.nullable=Cyc_Absyn_true_conref;
_tmpB9.bounds=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();_tmpB9.zero_term=((
union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();_tmpB9.ptrloc=0;_tmpB9;});
_tmpB8;});_tmpB7;});_tmpB6;});goto _LL3D;}_LL3D:;}return t;}static void*Cyc_Tcexp_tcVar(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct _tuple1*q,void*b){
void*_tmpBC=b;struct Cyc_Absyn_Vardecl*_tmpBF;struct Cyc_Absyn_Fndecl*_tmpC1;
struct Cyc_Absyn_Vardecl*_tmpC3;struct Cyc_Absyn_Vardecl*_tmpC5;struct Cyc_Absyn_Vardecl*
_tmpC7;_LL6B: {struct Cyc_Absyn_Unresolved_b_struct*_tmpBD=(struct Cyc_Absyn_Unresolved_b_struct*)
_tmpBC;if(_tmpBD->tag != 0)goto _LL6D;}_LL6C: return({struct Cyc_String_pa_struct
_tmpCA;_tmpCA.tag=0;_tmpCA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
q));{void*_tmpC8[1]={& _tmpCA};Cyc_Tcexp_expr_err(te,loc,0,({const char*_tmpC9="undeclared identifier: %s";
_tag_dyneither(_tmpC9,sizeof(char),26);}),_tag_dyneither(_tmpC8,sizeof(void*),1));}});
_LL6D: {struct Cyc_Absyn_Global_b_struct*_tmpBE=(struct Cyc_Absyn_Global_b_struct*)
_tmpBC;if(_tmpBE->tag != 1)goto _LL6F;else{_tmpBF=_tmpBE->f1;}}_LL6E:*q=*_tmpBF->name;
return _tmpBF->type;_LL6F: {struct Cyc_Absyn_Funname_b_struct*_tmpC0=(struct Cyc_Absyn_Funname_b_struct*)
_tmpBC;if(_tmpC0->tag != 2)goto _LL71;else{_tmpC1=_tmpC0->f1;}}_LL70:*q=*_tmpC1->name;
return Cyc_Tcutil_fndecl2typ(_tmpC1);_LL71: {struct Cyc_Absyn_Pat_b_struct*_tmpC2=(
struct Cyc_Absyn_Pat_b_struct*)_tmpBC;if(_tmpC2->tag != 5)goto _LL73;else{_tmpC3=
_tmpC2->f1;}}_LL72: _tmpC5=_tmpC3;goto _LL74;_LL73: {struct Cyc_Absyn_Local_b_struct*
_tmpC4=(struct Cyc_Absyn_Local_b_struct*)_tmpBC;if(_tmpC4->tag != 4)goto _LL75;
else{_tmpC5=_tmpC4->f1;}}_LL74: _tmpC7=_tmpC5;goto _LL76;_LL75: {struct Cyc_Absyn_Param_b_struct*
_tmpC6=(struct Cyc_Absyn_Param_b_struct*)_tmpBC;if(_tmpC6->tag != 3)goto _LL6A;
else{_tmpC7=_tmpC6->f1;}}_LL76:(*q).f1=({union Cyc_Absyn_Nmspace _tmpCB;(_tmpCB.Loc_n).val=
0;(_tmpCB.Loc_n).tag=3;_tmpCB;});return _tmpC7->type;_LL6A:;}static void Cyc_Tcexp_check_format_args(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*fmt,struct Cyc_Core_Opt*opt_args,
struct _RegionHandle*temp,struct Cyc_List_List*(*type_getter)(struct _RegionHandle*,
struct Cyc_Tcenv_Tenv*,struct _dyneither_ptr,struct Cyc_Position_Segment*)){struct
Cyc_List_List*desc_types;{void*_tmpCC=fmt->r;union Cyc_Absyn_Cnst _tmpCE;struct
_dyneither_ptr _tmpCF;struct Cyc_Absyn_Exp*_tmpD1;struct Cyc_Absyn_Exp _tmpD2;void*
_tmpD3;union Cyc_Absyn_Cnst _tmpD5;struct _dyneither_ptr _tmpD6;_LL78: {struct Cyc_Absyn_Const_e_struct*
_tmpCD=(struct Cyc_Absyn_Const_e_struct*)_tmpCC;if(_tmpCD->tag != 0)goto _LL7A;
else{_tmpCE=_tmpCD->f1;if((_tmpCE.String_c).tag != 7)goto _LL7A;_tmpCF=(struct
_dyneither_ptr)(_tmpCE.String_c).val;}}_LL79: _tmpD6=_tmpCF;goto _LL7B;_LL7A: {
struct Cyc_Absyn_Cast_e_struct*_tmpD0=(struct Cyc_Absyn_Cast_e_struct*)_tmpCC;if(
_tmpD0->tag != 15)goto _LL7C;else{_tmpD1=_tmpD0->f2;_tmpD2=*_tmpD1;_tmpD3=_tmpD2.r;{
struct Cyc_Absyn_Const_e_struct*_tmpD4=(struct Cyc_Absyn_Const_e_struct*)_tmpD3;
if(_tmpD4->tag != 0)goto _LL7C;else{_tmpD5=_tmpD4->f1;if((_tmpD5.String_c).tag != 7)
goto _LL7C;_tmpD6=(struct _dyneither_ptr)(_tmpD5.String_c).val;}}}}_LL7B:
desc_types=type_getter(temp,te,(struct _dyneither_ptr)_tmpD6,fmt->loc);goto _LL77;
_LL7C:;_LL7D: if(opt_args != 0){struct Cyc_List_List*_tmpD7=(struct Cyc_List_List*)
opt_args->v;for(0;_tmpD7 != 0;_tmpD7=_tmpD7->tl){Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)
_tmpD7->hd);{struct _RegionHandle*_tmpD8=Cyc_Tcenv_get_fnrgn(te);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(
_tmpD8,(void*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Exp*)_tmpD7->hd)->topt))->v)
 && !Cyc_Tcutil_is_noalias_path(_tmpD8,(struct Cyc_Absyn_Exp*)_tmpD7->hd))({void*
_tmpD9=0;Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_tmpD7->hd)->loc,({const char*
_tmpDA="Cannot consume non-unique paths; do swap instead";_tag_dyneither(_tmpDA,
sizeof(char),49);}),_tag_dyneither(_tmpD9,sizeof(void*),0));});}}}return;_LL77:;}
if(opt_args != 0){struct Cyc_List_List*_tmpDB=(struct Cyc_List_List*)opt_args->v;
for(0;desc_types != 0  && _tmpDB != 0;(desc_types=desc_types->tl,_tmpDB=_tmpDB->tl)){
void*t=(void*)desc_types->hd;struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)_tmpDB->hd;
Cyc_Tcexp_tcExp(te,(void**)& t,e);if(!Cyc_Tcutil_coerce_arg(te,e,t)){({struct Cyc_String_pa_struct
_tmpDF;_tmpDF.tag=0;_tmpDF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v));{struct Cyc_String_pa_struct
_tmpDE;_tmpDE.tag=0;_tmpDE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t));{void*_tmpDC[2]={& _tmpDE,& _tmpDF};Cyc_Tcutil_terr(e->loc,({const char*_tmpDD="descriptor has type \n%s\n but argument has type \n%s";
_tag_dyneither(_tmpDD,sizeof(char),51);}),_tag_dyneither(_tmpDC,sizeof(void*),2));}}});
Cyc_Tcutil_explain_failure();}{struct _RegionHandle*_tmpE0=Cyc_Tcenv_get_fnrgn(te);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(_tmpE0,t) && !Cyc_Tcutil_is_noalias_path(
_tmpE0,e))({void*_tmpE1=0;Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_tmpDB->hd)->loc,({
const char*_tmpE2="Cannot consume non-unique paths; do swap instead";
_tag_dyneither(_tmpE2,sizeof(char),49);}),_tag_dyneither(_tmpE1,sizeof(void*),0));});}}
if(desc_types != 0)({void*_tmpE3=0;Cyc_Tcutil_terr(fmt->loc,({const char*_tmpE4="too few arguments";
_tag_dyneither(_tmpE4,sizeof(char),18);}),_tag_dyneither(_tmpE3,sizeof(void*),0));});
if(_tmpDB != 0)({void*_tmpE5=0;Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_tmpDB->hd)->loc,({
const char*_tmpE6="too many arguments";_tag_dyneither(_tmpE6,sizeof(char),19);}),
_tag_dyneither(_tmpE5,sizeof(void*),0));});}}static void*Cyc_Tcexp_tcUnPrimop(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,enum Cyc_Absyn_Primop
p,struct Cyc_Absyn_Exp*e){void*t=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);switch(p){case Cyc_Absyn_Plus: _LL7E: goto _LL7F;case Cyc_Absyn_Minus:
_LL7F: if(!Cyc_Tcutil_is_numeric(e))({struct Cyc_String_pa_struct _tmpE9;_tmpE9.tag=
0;_tmpE9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t));{void*_tmpE7[1]={& _tmpE9};Cyc_Tcutil_terr(loc,({const char*_tmpE8="expecting arithmetic type but found %s";
_tag_dyneither(_tmpE8,sizeof(char),39);}),_tag_dyneither(_tmpE7,sizeof(void*),1));}});
return(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;case Cyc_Absyn_Not:
_LL80: Cyc_Tcutil_check_contains_assign(e);if(!Cyc_Tcutil_coerce_to_bool(te,e))({
struct Cyc_String_pa_struct _tmpEC;_tmpEC.tag=0;_tmpEC.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));{void*_tmpEA[1]={& _tmpEC};Cyc_Tcutil_terr(
loc,({const char*_tmpEB="expecting integral or * type but found %s";
_tag_dyneither(_tmpEB,sizeof(char),42);}),_tag_dyneither(_tmpEA,sizeof(void*),1));}});
return Cyc_Absyn_sint_typ;case Cyc_Absyn_Bitnot: _LL81: if(!Cyc_Tcutil_is_integral(e))({
struct Cyc_String_pa_struct _tmpEF;_tmpEF.tag=0;_tmpEF.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));{void*_tmpED[1]={& _tmpEF};Cyc_Tcutil_terr(
loc,({const char*_tmpEE="expecting integral type but found %s";_tag_dyneither(
_tmpEE,sizeof(char),37);}),_tag_dyneither(_tmpED,sizeof(void*),1));}});return(
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;case Cyc_Absyn_Numelts: _LL82:{
void*_tmpF0=t;struct Cyc_Absyn_PtrInfo _tmpF3;struct Cyc_Absyn_PtrAtts _tmpF4;union
Cyc_Absyn_Constraint*_tmpF5;_LL85: {struct Cyc_Absyn_ArrayType_struct*_tmpF1=(
struct Cyc_Absyn_ArrayType_struct*)_tmpF0;if(_tmpF1->tag != 9)goto _LL87;}_LL86:
goto _LL84;_LL87: {struct Cyc_Absyn_PointerType_struct*_tmpF2=(struct Cyc_Absyn_PointerType_struct*)
_tmpF0;if(_tmpF2->tag != 5)goto _LL89;else{_tmpF3=_tmpF2->f1;_tmpF4=_tmpF3.ptr_atts;
_tmpF5=_tmpF4.bounds;}}_LL88:{void*_tmpF6=((void*(*)(void*y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_constr)((void*)& Cyc_Absyn_DynEither_b_val,_tmpF5);struct Cyc_Absyn_Exp*
_tmpF9;_LL8C: {struct Cyc_Absyn_DynEither_b_struct*_tmpF7=(struct Cyc_Absyn_DynEither_b_struct*)
_tmpF6;if(_tmpF7->tag != 0)goto _LL8E;}_LL8D: goto _LL8B;_LL8E: {struct Cyc_Absyn_Upper_b_struct*
_tmpF8=(struct Cyc_Absyn_Upper_b_struct*)_tmpF6;if(_tmpF8->tag != 1)goto _LL8B;
else{_tmpF9=_tmpF8->f1;}}_LL8F: if(!Cyc_Evexp_c_can_eval(_tmpF9))({void*_tmpFA=0;
Cyc_Tcutil_terr(loc,({const char*_tmpFB="cannot apply numelts to a pointer with abstract bounds";
_tag_dyneither(_tmpFB,sizeof(char),55);}),_tag_dyneither(_tmpFA,sizeof(void*),0));});
goto _LL8B;_LL8B:;}goto _LL84;_LL89:;_LL8A:({struct Cyc_String_pa_struct _tmpFE;
_tmpFE.tag=0;_tmpFE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t));{void*_tmpFC[1]={& _tmpFE};Cyc_Tcutil_terr(loc,({const char*_tmpFD="numelts requires pointer or array type, not %s";
_tag_dyneither(_tmpFD,sizeof(char),47);}),_tag_dyneither(_tmpFC,sizeof(void*),1));}});
_LL84:;}return Cyc_Absyn_uint_typ;default: _LL83:({void*_tmpFF=0;((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp100="Non-unary primop";
_tag_dyneither(_tmp100,sizeof(char),17);}),_tag_dyneither(_tmpFF,sizeof(void*),0));});}}
static void*Cyc_Tcexp_tcArithBinop(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,
struct Cyc_Absyn_Exp*e2,int(*checker)(struct Cyc_Absyn_Exp*)){if(!checker(e1)){({
struct Cyc_String_pa_struct _tmp103;_tmp103.tag=0;_tmp103.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(e1->topt))->v));{void*_tmp101[1]={& _tmp103};Cyc_Tcutil_terr(e1->loc,({
const char*_tmp102="type %s cannot be used here";_tag_dyneither(_tmp102,sizeof(
char),28);}),_tag_dyneither(_tmp101,sizeof(void*),1));}});return Cyc_Absyn_wildtyp(
Cyc_Tcenv_lookup_opt_type_vars(te));}if(!checker(e2)){({struct Cyc_String_pa_struct
_tmp106;_tmp106.tag=0;_tmp106.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v));{
void*_tmp104[1]={& _tmp106};Cyc_Tcutil_terr(e2->loc,({const char*_tmp105="type %s cannot be used here";
_tag_dyneither(_tmp105,sizeof(char),28);}),_tag_dyneither(_tmp104,sizeof(void*),
1));}});return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}{void*t1=
Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v);void*
t2=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v);
return Cyc_Tcutil_max_arithmetic_type(t1,t2);}}static void*Cyc_Tcexp_tcPlus(struct
Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){void*t1=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v);void*t2=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v);void*_tmp107=t1;struct Cyc_Absyn_PtrInfo
_tmp109;void*_tmp10A;struct Cyc_Absyn_Tqual _tmp10B;struct Cyc_Absyn_PtrAtts _tmp10C;
void*_tmp10D;union Cyc_Absyn_Constraint*_tmp10E;union Cyc_Absyn_Constraint*_tmp10F;
union Cyc_Absyn_Constraint*_tmp110;_LL92: {struct Cyc_Absyn_PointerType_struct*
_tmp108=(struct Cyc_Absyn_PointerType_struct*)_tmp107;if(_tmp108->tag != 5)goto
_LL94;else{_tmp109=_tmp108->f1;_tmp10A=_tmp109.elt_typ;_tmp10B=_tmp109.elt_tq;
_tmp10C=_tmp109.ptr_atts;_tmp10D=_tmp10C.rgn;_tmp10E=_tmp10C.nullable;_tmp10F=
_tmp10C.bounds;_tmp110=_tmp10C.zero_term;}}_LL93: if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(
_tmp10A),Cyc_Absyn_MemKind))({void*_tmp111=0;Cyc_Tcutil_terr(e1->loc,({const char*
_tmp112="can't perform arithmetic on abstract pointer type";_tag_dyneither(
_tmp112,sizeof(char),50);}),_tag_dyneither(_tmp111,sizeof(void*),0));});if(Cyc_Tcutil_is_noalias_pointer(
t1))({void*_tmp113=0;Cyc_Tcutil_terr(e1->loc,({const char*_tmp114="can't perform arithmetic on non-aliasing pointer type";
_tag_dyneither(_tmp114,sizeof(char),54);}),_tag_dyneither(_tmp113,sizeof(void*),
0));});if(!Cyc_Tcutil_coerce_sint_typ(te,e2))({struct Cyc_String_pa_struct _tmp117;
_tmp117.tag=0;_tmp117.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t2));{void*_tmp115[1]={& _tmp117};Cyc_Tcutil_terr(e2->loc,({const char*_tmp116="expecting int but found %s";
_tag_dyneither(_tmp116,sizeof(char),27);}),_tag_dyneither(_tmp115,sizeof(void*),
1));}});{void*_tmp118=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)((
void*)& Cyc_Absyn_DynEither_b_val,_tmp10F);struct Cyc_Absyn_Exp*_tmp11B;_LL97: {
struct Cyc_Absyn_DynEither_b_struct*_tmp119=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp118;if(_tmp119->tag != 0)goto _LL99;}_LL98: return t1;_LL99: {struct Cyc_Absyn_Upper_b_struct*
_tmp11A=(struct Cyc_Absyn_Upper_b_struct*)_tmp118;if(_tmp11A->tag != 1)goto _LL96;
else{_tmp11B=_tmp11A->f1;}}_LL9A: if(((int(*)(int y,union Cyc_Absyn_Constraint*x))
Cyc_Absyn_conref_def)(0,_tmp110))({void*_tmp11C=0;Cyc_Tcutil_warn(e1->loc,({
const char*_tmp11D="pointer arithmetic on thin, zero-terminated pointer may be expensive.";
_tag_dyneither(_tmp11D,sizeof(char),70);}),_tag_dyneither(_tmp11C,sizeof(void*),
0));});{struct Cyc_Absyn_PointerType_struct*_tmp11E=({struct Cyc_Absyn_PointerType_struct*
_tmp11F=_cycalloc(sizeof(*_tmp11F));_tmp11F[0]=({struct Cyc_Absyn_PointerType_struct
_tmp120;_tmp120.tag=5;_tmp120.f1=({struct Cyc_Absyn_PtrInfo _tmp121;_tmp121.elt_typ=
_tmp10A;_tmp121.elt_tq=_tmp10B;_tmp121.ptr_atts=({struct Cyc_Absyn_PtrAtts _tmp122;
_tmp122.rgn=_tmp10D;_tmp122.nullable=Cyc_Absyn_true_conref;_tmp122.bounds=Cyc_Absyn_bounds_dyneither_conref;
_tmp122.zero_term=_tmp110;_tmp122.ptrloc=0;_tmp122;});_tmp121;});_tmp120;});
_tmp11F;});Cyc_Tcutil_unchecked_cast(te,e1,(void*)_tmp11E,Cyc_Absyn_Other_coercion);
return(void*)_tmp11E;}_LL96:;}_LL94:;_LL95: return Cyc_Tcexp_tcArithBinop(te,e1,e2,
Cyc_Tcutil_is_numeric);_LL91:;}static void*Cyc_Tcexp_tcMinus(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){void*t1=(void*)((struct Cyc_Core_Opt*)
_check_null(e1->topt))->v;void*t2=(void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v;
void*t1_elt=(void*)& Cyc_Absyn_VoidType_val;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(
t1,& t1_elt)){if(Cyc_Tcutil_is_tagged_pointer_typ(t2)){if(!Cyc_Tcutil_unify(t1,t2)){({
struct Cyc_String_pa_struct _tmp126;_tmp126.tag=0;_tmp126.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(e2->topt))->v));{struct Cyc_String_pa_struct _tmp125;_tmp125.tag=0;
_tmp125.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v));{void*_tmp123[2]={&
_tmp125,& _tmp126};Cyc_Tcutil_terr(e1->loc,({const char*_tmp124="pointer arithmetic on values of different types (%s != %s)";
_tag_dyneither(_tmp124,sizeof(char),59);}),_tag_dyneither(_tmp123,sizeof(void*),
2));}}});Cyc_Tcutil_explain_failure();}return Cyc_Absyn_sint_typ;}else{if(!Cyc_Tcutil_kind_leq(
Cyc_Tcutil_typ_kind(t1_elt),Cyc_Absyn_MemKind))({void*_tmp127=0;Cyc_Tcutil_terr(
e1->loc,({const char*_tmp128="can't perform arithmetic on abstract pointer type";
_tag_dyneither(_tmp128,sizeof(char),50);}),_tag_dyneither(_tmp127,sizeof(void*),
0));});if(Cyc_Tcutil_is_noalias_pointer(t1))({void*_tmp129=0;Cyc_Tcutil_terr(e1->loc,({
const char*_tmp12A="can't perform arithmetic on non-aliasing pointer type";
_tag_dyneither(_tmp12A,sizeof(char),54);}),_tag_dyneither(_tmp129,sizeof(void*),
0));});if(!Cyc_Tcutil_coerce_sint_typ(te,e2)){({struct Cyc_String_pa_struct
_tmp12E;_tmp12E.tag=0;_tmp12E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2));{struct Cyc_String_pa_struct _tmp12D;_tmp12D.tag=0;
_tmp12D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t1));{void*_tmp12B[2]={& _tmp12D,& _tmp12E};Cyc_Tcutil_terr(e2->loc,({const char*
_tmp12C="expecting either %s or int but found %s";_tag_dyneither(_tmp12C,sizeof(
char),40);}),_tag_dyneither(_tmp12B,sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();}
return t1;}}if(Cyc_Tcutil_is_pointer_type(t1))Cyc_Tcutil_unchecked_cast(te,e1,Cyc_Absyn_sint_typ,
Cyc_Absyn_Other_coercion);if(Cyc_Tcutil_is_pointer_type(t2))Cyc_Tcutil_unchecked_cast(
te,e2,Cyc_Absyn_sint_typ,Cyc_Absyn_Other_coercion);return Cyc_Tcexp_tcArithBinop(
te,e1,e2,Cyc_Tcutil_is_numeric);}static void*Cyc_Tcexp_tcAnyBinop(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
int e1_is_num=Cyc_Tcutil_is_numeric(e1);int e2_is_num=Cyc_Tcutil_is_numeric(e2);
void*t1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v);
void*t2=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v);
if(e1_is_num  && e2_is_num)return Cyc_Absyn_sint_typ;else{if(Cyc_Tcutil_typ_kind(
t1)== Cyc_Absyn_BoxKind  || Cyc_Tcutil_unify(t1,Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_bk,Cyc_Tcenv_lookup_opt_type_vars(te)))){if(Cyc_Tcutil_unify(t1,t2))
return Cyc_Absyn_sint_typ;else{if(Cyc_Tcutil_silent_castable(te,loc,t2,t1)){Cyc_Tcutil_unchecked_cast(
te,e2,t1,Cyc_Absyn_Other_coercion);return Cyc_Absyn_sint_typ;}else{if(Cyc_Tcutil_silent_castable(
te,loc,t1,t2)){Cyc_Tcutil_unchecked_cast(te,e1,t2,Cyc_Absyn_Other_coercion);
return Cyc_Absyn_sint_typ;}else{if(Cyc_Tcutil_zero_to_null(te,t2,e1) || Cyc_Tcutil_zero_to_null(
te,t1,e2))return Cyc_Absyn_sint_typ;else{goto pointer_cmp;}}}}}else{pointer_cmp: {
struct _tuple0 _tmp130=({struct _tuple0 _tmp12F;_tmp12F.f1=Cyc_Tcutil_compress(t1);
_tmp12F.f2=Cyc_Tcutil_compress(t2);_tmp12F;});void*_tmp131;struct Cyc_Absyn_PtrInfo
_tmp133;void*_tmp134;void*_tmp135;struct Cyc_Absyn_PtrInfo _tmp137;void*_tmp138;
void*_tmp139;void*_tmp13B;_LL9C: _tmp131=_tmp130.f1;{struct Cyc_Absyn_PointerType_struct*
_tmp132=(struct Cyc_Absyn_PointerType_struct*)_tmp131;if(_tmp132->tag != 5)goto
_LL9E;else{_tmp133=_tmp132->f1;_tmp134=_tmp133.elt_typ;}}_tmp135=_tmp130.f2;{
struct Cyc_Absyn_PointerType_struct*_tmp136=(struct Cyc_Absyn_PointerType_struct*)
_tmp135;if(_tmp136->tag != 5)goto _LL9E;else{_tmp137=_tmp136->f1;_tmp138=_tmp137.elt_typ;}}
_LL9D: if(Cyc_Tcutil_unify(_tmp134,_tmp138))return Cyc_Absyn_sint_typ;goto _LL9B;
_LL9E: _tmp139=_tmp130.f1;{struct Cyc_Absyn_RgnHandleType_struct*_tmp13A=(struct
Cyc_Absyn_RgnHandleType_struct*)_tmp139;if(_tmp13A->tag != 16)goto _LLA0;}_tmp13B=
_tmp130.f2;{struct Cyc_Absyn_RgnHandleType_struct*_tmp13C=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp13B;if(_tmp13C->tag != 16)goto _LLA0;}_LL9F: return Cyc_Absyn_sint_typ;_LLA0:;
_LLA1: goto _LL9B;_LL9B:;}({struct Cyc_String_pa_struct _tmp140;_tmp140.tag=0;
_tmp140.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t2));{struct Cyc_String_pa_struct _tmp13F;_tmp13F.tag=0;_tmp13F.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));{void*_tmp13D[
2]={& _tmp13F,& _tmp140};Cyc_Tcutil_terr(loc,({const char*_tmp13E="comparison not allowed between %s and %s";
_tag_dyneither(_tmp13E,sizeof(char),41);}),_tag_dyneither(_tmp13D,sizeof(void*),
2));}}});Cyc_Tcutil_explain_failure();return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(
te));}}}static void*Cyc_Tcexp_tcEqPrimop(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){if(Cyc_Tcexp_tcAnyBinop(te,
loc,e1,e2)== Cyc_Absyn_sint_typ)return Cyc_Absyn_sint_typ;{void*t1=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v);void*t2=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v);{struct _tuple0 _tmp142=({
struct _tuple0 _tmp141;_tmp141.f1=t1;_tmp141.f2=t2;_tmp141;});void*_tmp143;void*
_tmp145;void*_tmp146;void*_tmp148;_LLA3: _tmp143=_tmp142.f1;{struct Cyc_Absyn_RgnHandleType_struct*
_tmp144=(struct Cyc_Absyn_RgnHandleType_struct*)_tmp143;if(_tmp144->tag != 16)goto
_LLA5;else{_tmp145=(void*)_tmp144->f1;}}_tmp146=_tmp142.f2;{struct Cyc_Absyn_RgnHandleType_struct*
_tmp147=(struct Cyc_Absyn_RgnHandleType_struct*)_tmp146;if(_tmp147->tag != 16)goto
_LLA5;else{_tmp148=(void*)_tmp147->f1;}}_LLA4: return Cyc_Absyn_sint_typ;_LLA5:;
_LLA6: goto _LLA2;_LLA2:;}({struct Cyc_String_pa_struct _tmp14C;_tmp14C.tag=0;
_tmp14C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t2));{struct Cyc_String_pa_struct _tmp14B;_tmp14B.tag=0;_tmp14B.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));{void*_tmp149[
2]={& _tmp14B,& _tmp14C};Cyc_Tcutil_terr(loc,({const char*_tmp14A="comparison not allowed between %s and %s";
_tag_dyneither(_tmp14A,sizeof(char),41);}),_tag_dyneither(_tmp149,sizeof(void*),
2));}}});Cyc_Tcutil_explain_failure();return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(
te));}}static void*Cyc_Tcexp_tcBinPrimop(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2){switch(p){case Cyc_Absyn_Plus: _LLA7: return Cyc_Tcexp_tcPlus(te,e1,e2);case Cyc_Absyn_Minus:
_LLA8: return Cyc_Tcexp_tcMinus(te,e1,e2);case Cyc_Absyn_Times: _LLA9: goto _LLAA;case
Cyc_Absyn_Div: _LLAA: return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_numeric);
case Cyc_Absyn_Mod: _LLAB: goto _LLAC;case Cyc_Absyn_Bitand: _LLAC: goto _LLAD;case Cyc_Absyn_Bitor:
_LLAD: goto _LLAE;case Cyc_Absyn_Bitxor: _LLAE: goto _LLAF;case Cyc_Absyn_Bitlshift:
_LLAF: goto _LLB0;case Cyc_Absyn_Bitlrshift: _LLB0: goto _LLB1;case Cyc_Absyn_Bitarshift:
_LLB1: return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_integral);case Cyc_Absyn_Eq:
_LLB2: goto _LLB3;case Cyc_Absyn_Neq: _LLB3: return Cyc_Tcexp_tcEqPrimop(te,loc,e1,e2);
case Cyc_Absyn_Gt: _LLB4: goto _LLB5;case Cyc_Absyn_Lt: _LLB5: goto _LLB6;case Cyc_Absyn_Gte:
_LLB6: goto _LLB7;case Cyc_Absyn_Lte: _LLB7: return Cyc_Tcexp_tcAnyBinop(te,loc,e1,e2);
default: _LLB8:({void*_tmp14D=0;((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp14E="bad binary primop";
_tag_dyneither(_tmp14E,sizeof(char),18);}),_tag_dyneither(_tmp14D,sizeof(void*),
0));});}}static void*Cyc_Tcexp_tcPrimop(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,enum Cyc_Absyn_Primop p,struct Cyc_List_List*es){if(p == Cyc_Absyn_Minus
 && ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es)== 1)return Cyc_Tcexp_tcExp(
te,topt,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd);Cyc_Tcexp_tcExpList(
te,es);{void*t;switch(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es)){case
0: _LLBA: return({void*_tmp14F=0;Cyc_Tcexp_expr_err(te,loc,topt,({const char*
_tmp150="primitive operator has 0 arguments";_tag_dyneither(_tmp150,sizeof(char),
35);}),_tag_dyneither(_tmp14F,sizeof(void*),0));});case 1: _LLBB: t=Cyc_Tcexp_tcUnPrimop(
te,loc,topt,p,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd);
break;case 2: _LLBC: t=Cyc_Tcexp_tcBinPrimop(te,loc,topt,p,(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(es))->hd,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(((struct Cyc_List_List*)_check_null(es))->tl))->hd);break;default:
_LLBD: return({void*_tmp151=0;Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp152="primitive operator has > 2 arguments";
_tag_dyneither(_tmp152,sizeof(char),37);}),_tag_dyneither(_tmp151,sizeof(void*),
0));});}return t;}}struct _tuple17{struct Cyc_Absyn_Tqual f1;void*f2;};static void Cyc_Tcexp_check_writable(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){{void*_tmp153=e->r;void*_tmp155;
struct Cyc_Absyn_Vardecl*_tmp157;void*_tmp159;struct Cyc_Absyn_Vardecl*_tmp15B;
void*_tmp15D;struct Cyc_Absyn_Vardecl*_tmp15F;void*_tmp161;struct Cyc_Absyn_Vardecl*
_tmp163;struct Cyc_Absyn_Exp*_tmp165;struct Cyc_Absyn_Exp*_tmp166;struct Cyc_Absyn_Exp*
_tmp168;struct _dyneither_ptr*_tmp169;struct Cyc_Absyn_Exp*_tmp16B;struct
_dyneither_ptr*_tmp16C;struct Cyc_Absyn_Exp*_tmp16E;struct Cyc_Absyn_Exp*_tmp170;
struct Cyc_Absyn_Exp*_tmp172;_LLC0: {struct Cyc_Absyn_Var_e_struct*_tmp154=(struct
Cyc_Absyn_Var_e_struct*)_tmp153;if(_tmp154->tag != 1)goto _LLC2;else{_tmp155=(void*)
_tmp154->f2;{struct Cyc_Absyn_Param_b_struct*_tmp156=(struct Cyc_Absyn_Param_b_struct*)
_tmp155;if(_tmp156->tag != 3)goto _LLC2;else{_tmp157=_tmp156->f1;}}}}_LLC1: _tmp15B=
_tmp157;goto _LLC3;_LLC2: {struct Cyc_Absyn_Var_e_struct*_tmp158=(struct Cyc_Absyn_Var_e_struct*)
_tmp153;if(_tmp158->tag != 1)goto _LLC4;else{_tmp159=(void*)_tmp158->f2;{struct Cyc_Absyn_Local_b_struct*
_tmp15A=(struct Cyc_Absyn_Local_b_struct*)_tmp159;if(_tmp15A->tag != 4)goto _LLC4;
else{_tmp15B=_tmp15A->f1;}}}}_LLC3: _tmp15F=_tmp15B;goto _LLC5;_LLC4: {struct Cyc_Absyn_Var_e_struct*
_tmp15C=(struct Cyc_Absyn_Var_e_struct*)_tmp153;if(_tmp15C->tag != 1)goto _LLC6;
else{_tmp15D=(void*)_tmp15C->f2;{struct Cyc_Absyn_Pat_b_struct*_tmp15E=(struct Cyc_Absyn_Pat_b_struct*)
_tmp15D;if(_tmp15E->tag != 5)goto _LLC6;else{_tmp15F=_tmp15E->f1;}}}}_LLC5: _tmp163=
_tmp15F;goto _LLC7;_LLC6: {struct Cyc_Absyn_Var_e_struct*_tmp160=(struct Cyc_Absyn_Var_e_struct*)
_tmp153;if(_tmp160->tag != 1)goto _LLC8;else{_tmp161=(void*)_tmp160->f2;{struct Cyc_Absyn_Global_b_struct*
_tmp162=(struct Cyc_Absyn_Global_b_struct*)_tmp161;if(_tmp162->tag != 1)goto _LLC8;
else{_tmp163=_tmp162->f1;}}}}_LLC7: if(!(_tmp163->tq).real_const)return;goto _LLBF;
_LLC8: {struct Cyc_Absyn_Subscript_e_struct*_tmp164=(struct Cyc_Absyn_Subscript_e_struct*)
_tmp153;if(_tmp164->tag != 25)goto _LLCA;else{_tmp165=_tmp164->f1;_tmp166=_tmp164->f2;}}
_LLC9:{void*_tmp173=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp165->topt))->v);struct Cyc_Absyn_PtrInfo _tmp175;struct Cyc_Absyn_Tqual _tmp176;
struct Cyc_Absyn_ArrayInfo _tmp178;struct Cyc_Absyn_Tqual _tmp179;struct Cyc_List_List*
_tmp17B;_LLD7: {struct Cyc_Absyn_PointerType_struct*_tmp174=(struct Cyc_Absyn_PointerType_struct*)
_tmp173;if(_tmp174->tag != 5)goto _LLD9;else{_tmp175=_tmp174->f1;_tmp176=_tmp175.elt_tq;}}
_LLD8: _tmp179=_tmp176;goto _LLDA;_LLD9: {struct Cyc_Absyn_ArrayType_struct*_tmp177=(
struct Cyc_Absyn_ArrayType_struct*)_tmp173;if(_tmp177->tag != 9)goto _LLDB;else{
_tmp178=_tmp177->f1;_tmp179=_tmp178.tq;}}_LLDA: if(!_tmp179.real_const)return;
goto _LLD6;_LLDB: {struct Cyc_Absyn_TupleType_struct*_tmp17A=(struct Cyc_Absyn_TupleType_struct*)
_tmp173;if(_tmp17A->tag != 11)goto _LLDD;else{_tmp17B=_tmp17A->f1;}}_LLDC: {
unsigned int _tmp17D;int _tmp17E;struct _tuple13 _tmp17C=Cyc_Evexp_eval_const_uint_exp(
_tmp166);_tmp17D=_tmp17C.f1;_tmp17E=_tmp17C.f2;if(!_tmp17E){({void*_tmp17F=0;Cyc_Tcutil_terr(
e->loc,({const char*_tmp180="tuple projection cannot use sizeof or offsetof";
_tag_dyneither(_tmp180,sizeof(char),47);}),_tag_dyneither(_tmp17F,sizeof(void*),
0));});return;}{struct _handler_cons _tmp181;_push_handler(& _tmp181);{int _tmp183=0;
if(setjmp(_tmp181.handler))_tmp183=1;if(!_tmp183){{struct _tuple17 _tmp185;struct
Cyc_Absyn_Tqual _tmp186;struct _tuple17*_tmp184=((struct _tuple17*(*)(struct Cyc_List_List*
x,int n))Cyc_List_nth)(_tmp17B,(int)_tmp17D);_tmp185=*_tmp184;_tmp186=_tmp185.f1;
if(!_tmp186.real_const){_npop_handler(0);return;}};_pop_handler();}else{void*
_tmp182=(void*)_exn_thrown;void*_tmp188=_tmp182;_LLE0: {struct Cyc_List_Nth_struct*
_tmp189=(struct Cyc_List_Nth_struct*)_tmp188;if(_tmp189->tag != Cyc_List_Nth)goto
_LLE2;}_LLE1: return;_LLE2:;_LLE3:(void)_throw(_tmp188);_LLDF:;}}}goto _LLD6;}
_LLDD:;_LLDE: goto _LLD6;_LLD6:;}goto _LLBF;_LLCA: {struct Cyc_Absyn_AggrMember_e_struct*
_tmp167=(struct Cyc_Absyn_AggrMember_e_struct*)_tmp153;if(_tmp167->tag != 23)goto
_LLCC;else{_tmp168=_tmp167->f1;_tmp169=_tmp167->f2;}}_LLCB:{void*_tmp18A=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp168->topt))->v);struct Cyc_Absyn_AggrInfo
_tmp18C;union Cyc_Absyn_AggrInfoU _tmp18D;struct Cyc_Absyn_Aggrdecl**_tmp18E;struct
Cyc_List_List*_tmp190;_LLE5: {struct Cyc_Absyn_AggrType_struct*_tmp18B=(struct Cyc_Absyn_AggrType_struct*)
_tmp18A;if(_tmp18B->tag != 12)goto _LLE7;else{_tmp18C=_tmp18B->f1;_tmp18D=_tmp18C.aggr_info;
if((_tmp18D.KnownAggr).tag != 2)goto _LLE7;_tmp18E=(struct Cyc_Absyn_Aggrdecl**)(
_tmp18D.KnownAggr).val;}}_LLE6: {struct Cyc_Absyn_Aggrfield*sf=(struct Cyc_Absyn_Aggrdecl**)
_tmp18E == 0?0: Cyc_Absyn_lookup_decl_field(*_tmp18E,_tmp169);if(sf == 0  || !(sf->tq).real_const)
return;goto _LLE4;}_LLE7: {struct Cyc_Absyn_AnonAggrType_struct*_tmp18F=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp18A;if(_tmp18F->tag != 13)goto _LLE9;else{_tmp190=_tmp18F->f2;}}_LLE8: {struct
Cyc_Absyn_Aggrfield*sf=Cyc_Absyn_lookup_field(_tmp190,_tmp169);if(sf == 0  || !(sf->tq).real_const)
return;goto _LLE4;}_LLE9:;_LLEA: goto _LLE4;_LLE4:;}goto _LLBF;_LLCC: {struct Cyc_Absyn_AggrArrow_e_struct*
_tmp16A=(struct Cyc_Absyn_AggrArrow_e_struct*)_tmp153;if(_tmp16A->tag != 24)goto
_LLCE;else{_tmp16B=_tmp16A->f1;_tmp16C=_tmp16A->f2;}}_LLCD:{void*_tmp191=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp16B->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp193;void*_tmp194;_LLEC: {struct Cyc_Absyn_PointerType_struct*_tmp192=(struct
Cyc_Absyn_PointerType_struct*)_tmp191;if(_tmp192->tag != 5)goto _LLEE;else{_tmp193=
_tmp192->f1;_tmp194=_tmp193.elt_typ;}}_LLED:{void*_tmp195=Cyc_Tcutil_compress(
_tmp194);struct Cyc_Absyn_AggrInfo _tmp197;union Cyc_Absyn_AggrInfoU _tmp198;struct
Cyc_Absyn_Aggrdecl**_tmp199;struct Cyc_List_List*_tmp19B;_LLF1: {struct Cyc_Absyn_AggrType_struct*
_tmp196=(struct Cyc_Absyn_AggrType_struct*)_tmp195;if(_tmp196->tag != 12)goto _LLF3;
else{_tmp197=_tmp196->f1;_tmp198=_tmp197.aggr_info;if((_tmp198.KnownAggr).tag != 
2)goto _LLF3;_tmp199=(struct Cyc_Absyn_Aggrdecl**)(_tmp198.KnownAggr).val;}}_LLF2: {
struct Cyc_Absyn_Aggrfield*sf=(struct Cyc_Absyn_Aggrdecl**)_tmp199 == 0?0: Cyc_Absyn_lookup_decl_field(*
_tmp199,_tmp16C);if(sf == 0  || !(sf->tq).real_const)return;goto _LLF0;}_LLF3: {
struct Cyc_Absyn_AnonAggrType_struct*_tmp19A=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp195;if(_tmp19A->tag != 13)goto _LLF5;else{_tmp19B=_tmp19A->f2;}}_LLF4: {struct
Cyc_Absyn_Aggrfield*sf=Cyc_Absyn_lookup_field(_tmp19B,_tmp16C);if(sf == 0  || !(sf->tq).real_const)
return;goto _LLF0;}_LLF5:;_LLF6: goto _LLF0;_LLF0:;}goto _LLEB;_LLEE:;_LLEF: goto
_LLEB;_LLEB:;}goto _LLBF;_LLCE: {struct Cyc_Absyn_Deref_e_struct*_tmp16D=(struct
Cyc_Absyn_Deref_e_struct*)_tmp153;if(_tmp16D->tag != 22)goto _LLD0;else{_tmp16E=
_tmp16D->f1;}}_LLCF:{void*_tmp19C=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp16E->topt))->v);struct Cyc_Absyn_PtrInfo _tmp19E;struct Cyc_Absyn_Tqual
_tmp19F;struct Cyc_Absyn_ArrayInfo _tmp1A1;struct Cyc_Absyn_Tqual _tmp1A2;_LLF8: {
struct Cyc_Absyn_PointerType_struct*_tmp19D=(struct Cyc_Absyn_PointerType_struct*)
_tmp19C;if(_tmp19D->tag != 5)goto _LLFA;else{_tmp19E=_tmp19D->f1;_tmp19F=_tmp19E.elt_tq;}}
_LLF9: _tmp1A2=_tmp19F;goto _LLFB;_LLFA: {struct Cyc_Absyn_ArrayType_struct*_tmp1A0=(
struct Cyc_Absyn_ArrayType_struct*)_tmp19C;if(_tmp1A0->tag != 9)goto _LLFC;else{
_tmp1A1=_tmp1A0->f1;_tmp1A2=_tmp1A1.tq;}}_LLFB: if(!_tmp1A2.real_const)return;
goto _LLF7;_LLFC:;_LLFD: goto _LLF7;_LLF7:;}goto _LLBF;_LLD0: {struct Cyc_Absyn_NoInstantiate_e_struct*
_tmp16F=(struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp153;if(_tmp16F->tag != 13)
goto _LLD2;else{_tmp170=_tmp16F->f1;}}_LLD1: _tmp172=_tmp170;goto _LLD3;_LLD2: {
struct Cyc_Absyn_Instantiate_e_struct*_tmp171=(struct Cyc_Absyn_Instantiate_e_struct*)
_tmp153;if(_tmp171->tag != 14)goto _LLD4;else{_tmp172=_tmp171->f1;}}_LLD3: Cyc_Tcexp_check_writable(
te,_tmp172);return;_LLD4:;_LLD5: goto _LLBF;_LLBF:;}({struct Cyc_String_pa_struct
_tmp1A5;_tmp1A5.tag=0;_tmp1A5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(e));{void*_tmp1A3[1]={& _tmp1A5};Cyc_Tcutil_terr(e->loc,({
const char*_tmp1A4="attempt to write a const location: %s";_tag_dyneither(_tmp1A4,
sizeof(char),38);}),_tag_dyneither(_tmp1A3,sizeof(void*),1));}});}static void*Cyc_Tcexp_tcIncrement(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
e,enum Cyc_Absyn_Incrementor i){Cyc_Tcexp_tcExpNoPromote(te,0,e);if(!Cyc_Absyn_is_lvalue(
e))return({void*_tmp1A6=0;Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp1A7="increment/decrement of non-lvalue";
_tag_dyneither(_tmp1A7,sizeof(char),34);}),_tag_dyneither(_tmp1A6,sizeof(void*),
0));});Cyc_Tcexp_check_writable(te,e);{void*t=(void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v;if(!Cyc_Tcutil_is_numeric(e)){void*telt=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(t,& telt) || Cyc_Tcutil_is_zero_pointer_typ_elt(
t,& telt) && (i == Cyc_Absyn_PreInc  || i == Cyc_Absyn_PostInc)){if(!Cyc_Tcutil_kind_leq(
Cyc_Tcutil_typ_kind(telt),Cyc_Absyn_MemKind))({void*_tmp1A8=0;Cyc_Tcutil_terr(e->loc,({
const char*_tmp1A9="can't perform arithmetic on abstract pointer type";
_tag_dyneither(_tmp1A9,sizeof(char),50);}),_tag_dyneither(_tmp1A8,sizeof(void*),
0));});if(Cyc_Tcutil_is_noalias_pointer(t))({void*_tmp1AA=0;Cyc_Tcutil_terr(e->loc,({
const char*_tmp1AB="can't perform arithmetic on non-aliasing pointer type";
_tag_dyneither(_tmp1AB,sizeof(char),54);}),_tag_dyneither(_tmp1AA,sizeof(void*),
0));});}else{({struct Cyc_String_pa_struct _tmp1AE;_tmp1AE.tag=0;_tmp1AE.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));{void*
_tmp1AC[1]={& _tmp1AE};Cyc_Tcutil_terr(e->loc,({const char*_tmp1AD="expecting arithmetic or ? type but found %s";
_tag_dyneither(_tmp1AD,sizeof(char),44);}),_tag_dyneither(_tmp1AC,sizeof(void*),
1));}});}}return t;}}static void*Cyc_Tcexp_tcConditional(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2,struct Cyc_Absyn_Exp*e3){struct _tuple0*_tmp1B1;int _tmp1B2;struct Cyc_Tcexp_TestEnv
_tmp1B0=Cyc_Tcexp_tcTest(te,e1,({const char*_tmp1AF="conditional expression";
_tag_dyneither(_tmp1AF,sizeof(char),23);}));_tmp1B1=_tmp1B0.eq;_tmp1B2=_tmp1B0.isTrue;
Cyc_Tcexp_tcExp(te,topt,e2);Cyc_Tcexp_tcExp(te,topt,e3);{void*t=Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_mk,Cyc_Tcenv_lookup_opt_type_vars(te));struct Cyc_List_List
_tmp1B3=({struct Cyc_List_List _tmp1BA;_tmp1BA.hd=e3;_tmp1BA.tl=0;_tmp1BA;});
struct Cyc_List_List _tmp1B4=({struct Cyc_List_List _tmp1B9;_tmp1B9.hd=e2;_tmp1B9.tl=(
struct Cyc_List_List*)& _tmp1B3;_tmp1B9;});if(!Cyc_Tcutil_coerce_list(te,t,(struct
Cyc_List_List*)& _tmp1B4)){({struct Cyc_String_pa_struct _tmp1B8;_tmp1B8.tag=0;
_tmp1B8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e3->topt))->v));{struct Cyc_String_pa_struct
_tmp1B7;_tmp1B7.tag=0;_tmp1B7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v));{
void*_tmp1B5[2]={& _tmp1B7,& _tmp1B8};Cyc_Tcutil_terr(loc,({const char*_tmp1B6="conditional clause types do not match: %s != %s";
_tag_dyneither(_tmp1B6,sizeof(char),48);}),_tag_dyneither(_tmp1B5,sizeof(void*),
2));}}});Cyc_Tcutil_explain_failure();}return t;}}static void*Cyc_Tcexp_tcAnd(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Exp*e1,
struct Cyc_Absyn_Exp*e2){Cyc_Tcexp_tcTest(te,e1,({const char*_tmp1BB="logical-and expression";
_tag_dyneither(_tmp1BB,sizeof(char),23);}));Cyc_Tcexp_tcTest(te,e2,({const char*
_tmp1BC="logical-and expression";_tag_dyneither(_tmp1BC,sizeof(char),23);}));
return Cyc_Absyn_sint_typ;}static void*Cyc_Tcexp_tcOr(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
Cyc_Tcexp_tcTest(te,e1,({const char*_tmp1BD="logical-or expression";
_tag_dyneither(_tmp1BD,sizeof(char),22);}));Cyc_Tcexp_tcTest(te,e2,({const char*
_tmp1BE="logical-or expression";_tag_dyneither(_tmp1BE,sizeof(char),22);}));
return Cyc_Absyn_sint_typ;}static void*Cyc_Tcexp_tcAssignOp(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*
po,struct Cyc_Absyn_Exp*e2){struct _RegionHandle*_tmp1BF=Cyc_Tcenv_get_fnrgn(te);
Cyc_Tcexp_tcExpNoPromote(Cyc_Tcenv_enter_notreadctxt(_tmp1BF,te),0,e1);Cyc_Tcexp_tcExp(
te,(void**)((void**)((void*)&((struct Cyc_Core_Opt*)_check_null(e1->topt))->v)),
e2);{void*t1=(void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v;void*t2=(
void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v;{void*_tmp1C0=Cyc_Tcutil_compress(
t1);_LLFF: {struct Cyc_Absyn_ArrayType_struct*_tmp1C1=(struct Cyc_Absyn_ArrayType_struct*)
_tmp1C0;if(_tmp1C1->tag != 9)goto _LL101;}_LL100:({void*_tmp1C2=0;Cyc_Tcutil_terr(
loc,({const char*_tmp1C3="cannot assign to an array";_tag_dyneither(_tmp1C3,
sizeof(char),26);}),_tag_dyneither(_tmp1C2,sizeof(void*),0));});goto _LLFE;_LL101:;
_LL102: goto _LLFE;_LLFE:;}if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t1),Cyc_Absyn_MemKind))({
void*_tmp1C4=0;Cyc_Tcutil_terr(loc,({const char*_tmp1C5="type is abstract (can't determine size).";
_tag_dyneither(_tmp1C5,sizeof(char),41);}),_tag_dyneither(_tmp1C4,sizeof(void*),
0));});if(!Cyc_Absyn_is_lvalue(e1))return({void*_tmp1C6=0;Cyc_Tcexp_expr_err(te,
loc,topt,({const char*_tmp1C7="assignment to non-lvalue";_tag_dyneither(_tmp1C7,
sizeof(char),25);}),_tag_dyneither(_tmp1C6,sizeof(void*),0));});Cyc_Tcexp_check_writable(
te,e1);if(po == 0){struct _RegionHandle*_tmp1C8=Cyc_Tcenv_get_fnrgn(te);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(
_tmp1C8,t2) && !Cyc_Tcutil_is_noalias_path(_tmp1C8,e2))({void*_tmp1C9=0;Cyc_Tcutil_terr(
e2->loc,({const char*_tmp1CA="Cannot consume non-unique paths; do swap instead";
_tag_dyneither(_tmp1CA,sizeof(char),49);}),_tag_dyneither(_tmp1C9,sizeof(void*),
0));});if(!Cyc_Tcutil_coerce_assign(te,e2,t1)){void*_tmp1CB=({struct Cyc_String_pa_struct
_tmp1CF;_tmp1CF.tag=0;_tmp1CF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2));{struct Cyc_String_pa_struct _tmp1CE;_tmp1CE.tag=0;
_tmp1CE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t1));{void*_tmp1CC[2]={& _tmp1CE,& _tmp1CF};Cyc_Tcexp_expr_err(te,loc,topt,({const
char*_tmp1CD="type mismatch: %s != %s";_tag_dyneither(_tmp1CD,sizeof(char),24);}),
_tag_dyneither(_tmp1CC,sizeof(void*),2));}}});Cyc_Tcutil_unify(t1,t2);Cyc_Tcutil_explain_failure();
return _tmp1CB;}}else{enum Cyc_Absyn_Primop _tmp1D0=(enum Cyc_Absyn_Primop)po->v;
void*_tmp1D1=Cyc_Tcexp_tcBinPrimop(te,loc,0,_tmp1D0,e1,e2);if(!(Cyc_Tcutil_unify(
_tmp1D1,t1) || Cyc_Tcutil_coerceable(_tmp1D1) && Cyc_Tcutil_coerceable(t1))){
void*_tmp1D2=({struct Cyc_String_pa_struct _tmp1D6;_tmp1D6.tag=0;_tmp1D6.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2));{struct
Cyc_String_pa_struct _tmp1D5;_tmp1D5.tag=0;_tmp1D5.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));{void*_tmp1D3[2]={& _tmp1D5,&
_tmp1D6};Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp1D4="Cannot use this operator in an assignment when the arguments have types %s and %s";
_tag_dyneither(_tmp1D4,sizeof(char),82);}),_tag_dyneither(_tmp1D3,sizeof(void*),
2));}}});Cyc_Tcutil_unify(_tmp1D1,t1);Cyc_Tcutil_explain_failure();return _tmp1D2;}
return _tmp1D1;}return t1;}}static void*Cyc_Tcexp_tcSeqExp(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2){Cyc_Tcexp_tcExp(te,0,e1);Cyc_Tcexp_tcExp(te,topt,e2);return(void*)((struct
Cyc_Core_Opt*)_check_null(e2->topt))->v;}static struct Cyc_Absyn_Datatypefield*Cyc_Tcexp_tcInjection(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*tu,struct _RegionHandle*r,
struct Cyc_List_List*inst,struct Cyc_List_List*fs){static struct Cyc_Absyn_DoubleType_struct
dbl={8,0};static void*dbl_typ=(void*)& dbl;struct Cyc_List_List*fields;void*t1=(
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;{void*_tmp1D7=Cyc_Tcutil_compress(
t1);enum Cyc_Absyn_Size_of _tmp1DA;enum Cyc_Absyn_Size_of _tmp1DC;_LL104: {struct
Cyc_Absyn_FloatType_struct*_tmp1D8=(struct Cyc_Absyn_FloatType_struct*)_tmp1D7;
if(_tmp1D8->tag != 7)goto _LL106;}_LL105: Cyc_Tcutil_unchecked_cast(te,e,dbl_typ,
Cyc_Absyn_No_coercion);t1=dbl_typ;goto _LL103;_LL106: {struct Cyc_Absyn_IntType_struct*
_tmp1D9=(struct Cyc_Absyn_IntType_struct*)_tmp1D7;if(_tmp1D9->tag != 6)goto _LL108;
else{_tmp1DA=_tmp1D9->f2;if(_tmp1DA != Cyc_Absyn_Char_sz)goto _LL108;}}_LL107: goto
_LL109;_LL108: {struct Cyc_Absyn_IntType_struct*_tmp1DB=(struct Cyc_Absyn_IntType_struct*)
_tmp1D7;if(_tmp1DB->tag != 6)goto _LL10A;else{_tmp1DC=_tmp1DB->f2;if(_tmp1DC != Cyc_Absyn_Short_sz)
goto _LL10A;}}_LL109: Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_sint_typ,Cyc_Absyn_No_coercion);
t1=Cyc_Absyn_sint_typ;goto _LL103;_LL10A:;_LL10B: goto _LL103;_LL103:;}for(fields=
fs;fields != 0;fields=fields->tl){struct _tuple1*_tmp1DE;struct Cyc_List_List*
_tmp1DF;struct Cyc_Position_Segment*_tmp1E0;enum Cyc_Absyn_Scope _tmp1E1;struct Cyc_Absyn_Datatypefield
_tmp1DD=*((struct Cyc_Absyn_Datatypefield*)fields->hd);_tmp1DE=_tmp1DD.name;
_tmp1DF=_tmp1DD.typs;_tmp1E0=_tmp1DD.loc;_tmp1E1=_tmp1DD.sc;if(_tmp1DF == 0  || 
_tmp1DF->tl != 0)continue;{void*t2=Cyc_Tcutil_rsubstitute(r,inst,(*((struct
_tuple17*)_tmp1DF->hd)).f2);if(Cyc_Tcutil_unify(t1,t2))return(struct Cyc_Absyn_Datatypefield*)((
struct Cyc_Absyn_Datatypefield*)fields->hd);}}for(fields=fs;fields != 0;fields=
fields->tl){struct _tuple1*_tmp1E3;struct Cyc_List_List*_tmp1E4;struct Cyc_Position_Segment*
_tmp1E5;enum Cyc_Absyn_Scope _tmp1E6;struct Cyc_Absyn_Datatypefield _tmp1E2=*((
struct Cyc_Absyn_Datatypefield*)fields->hd);_tmp1E3=_tmp1E2.name;_tmp1E4=_tmp1E2.typs;
_tmp1E5=_tmp1E2.loc;_tmp1E6=_tmp1E2.sc;if(_tmp1E4 == 0  || _tmp1E4->tl != 0)
continue;{void*t2=Cyc_Tcutil_rsubstitute(r,inst,(*((struct _tuple17*)_tmp1E4->hd)).f2);
if(Cyc_Tcutil_coerce_arg(te,e,t2))return(struct Cyc_Absyn_Datatypefield*)((struct
Cyc_Absyn_Datatypefield*)fields->hd);}}({struct Cyc_String_pa_struct _tmp1EA;
_tmp1EA.tag=0;_tmp1EA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t1));{struct Cyc_String_pa_struct _tmp1E9;_tmp1E9.tag=0;_tmp1E9.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(tu));{void*_tmp1E7[
2]={& _tmp1E9,& _tmp1EA};Cyc_Tcutil_terr(e->loc,({const char*_tmp1E8="can't find a field in %s to inject a value of type %s";
_tag_dyneither(_tmp1E8,sizeof(char),54);}),_tag_dyneither(_tmp1E7,sizeof(void*),
2));}}});return 0;}static void*Cyc_Tcexp_tcFnCall(struct Cyc_Tcenv_Tenv*te_orig,
struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*
args,struct Cyc_Absyn_VarargCallInfo**vararg_call_info){struct Cyc_List_List*
_tmp1EC=args;struct _RegionHandle*_tmp1ED=Cyc_Tcenv_get_fnrgn(te_orig);struct Cyc_Tcenv_Tenv*
_tmp1EE=Cyc_Tcenv_new_block(_tmp1ED,loc,te_orig);Cyc_Tcexp_tcExp(_tmp1EE,0,e);{
void*t=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
void*_tmp1EF=t;struct Cyc_Absyn_PtrInfo _tmp1F1;void*_tmp1F2;struct Cyc_Absyn_Tqual
_tmp1F3;struct Cyc_Absyn_PtrAtts _tmp1F4;void*_tmp1F5;union Cyc_Absyn_Constraint*
_tmp1F6;union Cyc_Absyn_Constraint*_tmp1F7;union Cyc_Absyn_Constraint*_tmp1F8;
_LL10D: {struct Cyc_Absyn_PointerType_struct*_tmp1F0=(struct Cyc_Absyn_PointerType_struct*)
_tmp1EF;if(_tmp1F0->tag != 5)goto _LL10F;else{_tmp1F1=_tmp1F0->f1;_tmp1F2=_tmp1F1.elt_typ;
_tmp1F3=_tmp1F1.elt_tq;_tmp1F4=_tmp1F1.ptr_atts;_tmp1F5=_tmp1F4.rgn;_tmp1F6=
_tmp1F4.nullable;_tmp1F7=_tmp1F4.bounds;_tmp1F8=_tmp1F4.zero_term;}}_LL10E: Cyc_Tcenv_check_rgn_accessible(
_tmp1EE,loc,_tmp1F5);Cyc_Tcutil_check_nonzero_bound(loc,_tmp1F7);{void*_tmp1F9=
Cyc_Tcutil_compress(_tmp1F2);struct Cyc_Absyn_FnInfo _tmp1FB;struct Cyc_List_List*
_tmp1FC;struct Cyc_Core_Opt*_tmp1FD;void*_tmp1FE;struct Cyc_List_List*_tmp1FF;int
_tmp200;struct Cyc_Absyn_VarargInfo*_tmp201;struct Cyc_List_List*_tmp202;struct Cyc_List_List*
_tmp203;_LL112: {struct Cyc_Absyn_FnType_struct*_tmp1FA=(struct Cyc_Absyn_FnType_struct*)
_tmp1F9;if(_tmp1FA->tag != 10)goto _LL114;else{_tmp1FB=_tmp1FA->f1;_tmp1FC=_tmp1FB.tvars;
_tmp1FD=_tmp1FB.effect;_tmp1FE=_tmp1FB.ret_typ;_tmp1FF=_tmp1FB.args;_tmp200=
_tmp1FB.c_varargs;_tmp201=_tmp1FB.cyc_varargs;_tmp202=_tmp1FB.rgn_po;_tmp203=
_tmp1FB.attributes;}}_LL113: if(topt != 0)Cyc_Tcutil_unify(_tmp1FE,*topt);while(
_tmp1EC != 0  && _tmp1FF != 0){struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)_tmp1EC->hd;
void*t2=(*((struct _tuple8*)_tmp1FF->hd)).f3;Cyc_Tcexp_tcExp(_tmp1EE,(void**)& t2,
e1);if(!Cyc_Tcutil_coerce_arg(_tmp1EE,e1,t2)){({struct Cyc_String_pa_struct
_tmp207;_tmp207.tag=0;_tmp207.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2));{struct Cyc_String_pa_struct _tmp206;_tmp206.tag=0;
_tmp206.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v));{void*_tmp204[2]={&
_tmp206,& _tmp207};Cyc_Tcutil_terr(e1->loc,({const char*_tmp205="actual argument has type \n\t%s\n but formal has type \n\t%s.";
_tag_dyneither(_tmp205,sizeof(char),57);}),_tag_dyneither(_tmp204,sizeof(void*),
2));}}});Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v,
t2);Cyc_Tcutil_explain_failure();}if(Cyc_Tcutil_is_noalias_pointer_or_aggr(
_tmp1ED,t2) && !Cyc_Tcutil_is_noalias_path(_tmp1ED,e1))({void*_tmp208=0;Cyc_Tcutil_terr(
e1->loc,({const char*_tmp209="Cannot consume non-unique paths; do swap instead";
_tag_dyneither(_tmp209,sizeof(char),49);}),_tag_dyneither(_tmp208,sizeof(void*),
0));});_tmp1EC=_tmp1EC->tl;_tmp1FF=_tmp1FF->tl;}{int args_already_checked=0;{
struct Cyc_List_List*a=_tmp203;for(0;a != 0;a=a->tl){void*_tmp20A=(void*)a->hd;
enum Cyc_Absyn_Format_Type _tmp20C;int _tmp20D;int _tmp20E;_LL117: {struct Cyc_Absyn_Format_att_struct*
_tmp20B=(struct Cyc_Absyn_Format_att_struct*)_tmp20A;if(_tmp20B->tag != 19)goto
_LL119;else{_tmp20C=_tmp20B->f1;_tmp20D=_tmp20B->f2;_tmp20E=_tmp20B->f3;}}_LL118:{
struct _handler_cons _tmp20F;_push_handler(& _tmp20F);{int _tmp211=0;if(setjmp(
_tmp20F.handler))_tmp211=1;if(!_tmp211){{struct Cyc_Absyn_Exp*_tmp212=((struct Cyc_Absyn_Exp*(*)(
struct Cyc_List_List*x,int n))Cyc_List_nth)(args,_tmp20D - 1);struct Cyc_Core_Opt*
fmt_args;if(_tmp20E == 0)fmt_args=0;else{fmt_args=({struct Cyc_Core_Opt*_tmp213=
_cycalloc(sizeof(*_tmp213));_tmp213->v=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,int i))Cyc_List_nth_tail)(args,_tmp20E - 1);_tmp213;});}args_already_checked=1;{
struct _RegionHandle*_tmp214=Cyc_Tcenv_get_fnrgn(te_orig);switch(_tmp20C){case Cyc_Absyn_Printf_ft:
_LL11B: Cyc_Tcexp_check_format_args(_tmp1EE,_tmp212,fmt_args,_tmp214,Cyc_Formatstr_get_format_typs);
break;case Cyc_Absyn_Scanf_ft: _LL11C: Cyc_Tcexp_check_format_args(_tmp1EE,_tmp212,
fmt_args,_tmp214,Cyc_Formatstr_get_scanf_typs);break;}}};_pop_handler();}else{
void*_tmp210=(void*)_exn_thrown;void*_tmp216=_tmp210;_LL11F: {struct Cyc_List_Nth_struct*
_tmp217=(struct Cyc_List_Nth_struct*)_tmp216;if(_tmp217->tag != Cyc_List_Nth)goto
_LL121;}_LL120:({void*_tmp218=0;Cyc_Tcutil_terr(loc,({const char*_tmp219="bad format arguments";
_tag_dyneither(_tmp219,sizeof(char),21);}),_tag_dyneither(_tmp218,sizeof(void*),
0));});goto _LL11E;_LL121:;_LL122:(void)_throw(_tmp216);_LL11E:;}}}goto _LL116;
_LL119:;_LL11A: goto _LL116;_LL116:;}}if(_tmp1FF != 0)({void*_tmp21A=0;Cyc_Tcutil_terr(
loc,({const char*_tmp21B="too few arguments for function";_tag_dyneither(_tmp21B,
sizeof(char),31);}),_tag_dyneither(_tmp21A,sizeof(void*),0));});else{if((_tmp1EC
!= 0  || _tmp200) || _tmp201 != 0){if(_tmp200)for(0;_tmp1EC != 0;_tmp1EC=_tmp1EC->tl){
Cyc_Tcexp_tcExp(_tmp1EE,0,(struct Cyc_Absyn_Exp*)_tmp1EC->hd);}else{if(_tmp201 == 
0)({void*_tmp21C=0;Cyc_Tcutil_terr(loc,({const char*_tmp21D="too many arguments for function";
_tag_dyneither(_tmp21D,sizeof(char),32);}),_tag_dyneither(_tmp21C,sizeof(void*),
0));});else{void*_tmp21F;int _tmp220;struct Cyc_Absyn_VarargInfo _tmp21E=*_tmp201;
_tmp21F=_tmp21E.type;_tmp220=_tmp21E.inject;{struct Cyc_Absyn_VarargCallInfo*
_tmp221=({struct Cyc_Absyn_VarargCallInfo*_tmp23F=_cycalloc(sizeof(*_tmp23F));
_tmp23F->num_varargs=0;_tmp23F->injectors=0;_tmp23F->vai=(struct Cyc_Absyn_VarargInfo*)
_tmp201;_tmp23F;});*vararg_call_info=(struct Cyc_Absyn_VarargCallInfo*)_tmp221;
if(!_tmp220)for(0;_tmp1EC != 0;_tmp1EC=_tmp1EC->tl){struct Cyc_Absyn_Exp*e1=(
struct Cyc_Absyn_Exp*)_tmp1EC->hd;++ _tmp221->num_varargs;Cyc_Tcexp_tcExp(_tmp1EE,(
void**)& _tmp21F,e1);if(!Cyc_Tcutil_coerce_arg(_tmp1EE,e1,_tmp21F)){({struct Cyc_String_pa_struct
_tmp225;_tmp225.tag=0;_tmp225.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v));{
struct Cyc_String_pa_struct _tmp224;_tmp224.tag=0;_tmp224.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp21F));{void*_tmp222[2]={& _tmp224,&
_tmp225};Cyc_Tcutil_terr(loc,({const char*_tmp223="vararg requires type %s but argument has type %s";
_tag_dyneither(_tmp223,sizeof(char),49);}),_tag_dyneither(_tmp222,sizeof(void*),
2));}}});Cyc_Tcutil_explain_failure();}if(Cyc_Tcutil_is_noalias_pointer_or_aggr(
_tmp1ED,_tmp21F) && !Cyc_Tcutil_is_noalias_path(_tmp1ED,e1))({void*_tmp226=0;Cyc_Tcutil_terr(
e1->loc,({const char*_tmp227="Cannot consume non-unique paths; do swap instead";
_tag_dyneither(_tmp227,sizeof(char),49);}),_tag_dyneither(_tmp226,sizeof(void*),
0));});}else{void*_tmp228=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(
_tmp21F));struct Cyc_Absyn_DatatypeInfo _tmp22A;union Cyc_Absyn_DatatypeInfoU
_tmp22B;struct Cyc_Absyn_Datatypedecl**_tmp22C;struct Cyc_Absyn_Datatypedecl*
_tmp22D;struct Cyc_List_List*_tmp22E;_LL124: {struct Cyc_Absyn_DatatypeType_struct*
_tmp229=(struct Cyc_Absyn_DatatypeType_struct*)_tmp228;if(_tmp229->tag != 3)goto
_LL126;else{_tmp22A=_tmp229->f1;_tmp22B=_tmp22A.datatype_info;if((_tmp22B.KnownDatatype).tag
!= 2)goto _LL126;_tmp22C=(struct Cyc_Absyn_Datatypedecl**)(_tmp22B.KnownDatatype).val;
_tmp22D=*_tmp22C;_tmp22E=_tmp22A.targs;}}_LL125: {struct Cyc_Absyn_Datatypedecl*
_tmp22F=*Cyc_Tcenv_lookup_datatypedecl(_tmp1EE,loc,_tmp22D->name);struct Cyc_List_List*
fields=0;if(_tmp22F->fields == 0)({struct Cyc_String_pa_struct _tmp232;_tmp232.tag=
0;_tmp232.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
_tmp21F));{void*_tmp230[1]={& _tmp232};Cyc_Tcutil_terr(loc,({const char*_tmp231="can't inject into abstract datatype %s";
_tag_dyneither(_tmp231,sizeof(char),39);}),_tag_dyneither(_tmp230,sizeof(void*),
1));}});else{fields=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(
_tmp22F->fields))->v;}if(!Cyc_Tcutil_unify(Cyc_Tcutil_pointer_region(_tmp21F),
Cyc_Tcenv_curr_rgn(_tmp1EE)))({struct Cyc_String_pa_struct _tmp236;_tmp236.tag=0;
_tmp236.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
Cyc_Tcenv_curr_rgn(_tmp1EE)));{struct Cyc_String_pa_struct _tmp235;_tmp235.tag=0;
_tmp235.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
_tmp21F));{void*_tmp233[2]={& _tmp235,& _tmp236};Cyc_Tcutil_terr(loc,({const char*
_tmp234="can't unify pointer region for %s to curr_rgn %s";_tag_dyneither(
_tmp234,sizeof(char),49);}),_tag_dyneither(_tmp233,sizeof(void*),2));}}});{
struct _RegionHandle*_tmp237=Cyc_Tcenv_get_fnrgn(_tmp1EE);{struct Cyc_List_List*
_tmp238=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(_tmp237,_tmp237,
_tmp22F->tvs,_tmp22E);for(0;_tmp1EC != 0;_tmp1EC=_tmp1EC->tl){++ _tmp221->num_varargs;{
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)_tmp1EC->hd;if(!
args_already_checked){Cyc_Tcexp_tcExp(_tmp1EE,0,e1);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(
_tmp237,(void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v) && !Cyc_Tcutil_is_noalias_path(
_tmp237,e1))({void*_tmp239=0;Cyc_Tcutil_terr(e1->loc,({const char*_tmp23A="Cannot consume non-unique paths; do swap instead";
_tag_dyneither(_tmp23A,sizeof(char),49);}),_tag_dyneither(_tmp239,sizeof(void*),
0));});}{struct Cyc_Absyn_Datatypefield*_tmp23B=Cyc_Tcexp_tcInjection(_tmp1EE,e1,
Cyc_Tcutil_pointer_elt_type(_tmp21F),_tmp237,_tmp238,fields);if(_tmp23B != 0)
_tmp221->injectors=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_append)(_tmp221->injectors,({struct Cyc_List_List*_tmp23C=_cycalloc(
sizeof(*_tmp23C));_tmp23C->hd=(struct Cyc_Absyn_Datatypefield*)_tmp23B;_tmp23C->tl=
0;_tmp23C;}));}}}}goto _LL123;}}_LL126:;_LL127:({void*_tmp23D=0;Cyc_Tcutil_terr(
loc,({const char*_tmp23E="bad inject vararg type";_tag_dyneither(_tmp23E,sizeof(
char),23);}),_tag_dyneither(_tmp23D,sizeof(void*),0));});goto _LL123;_LL123:;}}}}}}
Cyc_Tcenv_check_effect_accessible(_tmp1EE,loc,(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp1FD))->v);Cyc_Tcenv_check_rgn_partial_order(_tmp1EE,loc,_tmp202);
return _tmp1FE;}_LL114:;_LL115: return({void*_tmp240=0;Cyc_Tcexp_expr_err(_tmp1EE,
loc,topt,({const char*_tmp241="expected pointer to function";_tag_dyneither(
_tmp241,sizeof(char),29);}),_tag_dyneither(_tmp240,sizeof(void*),0));});_LL111:;}
_LL10F:;_LL110: return({void*_tmp242=0;Cyc_Tcexp_expr_err(_tmp1EE,loc,topt,({
const char*_tmp243="expected pointer to function";_tag_dyneither(_tmp243,sizeof(
char),29);}),_tag_dyneither(_tmp242,sizeof(void*),0));});_LL10C:;}}static void*
Cyc_Tcexp_tcThrow(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**
topt,struct Cyc_Absyn_Exp*e){void*_tmp244=Cyc_Absyn_exn_typ();Cyc_Tcexp_tcExp(te,(
void**)& _tmp244,e);if(!Cyc_Tcutil_coerce_arg(te,e,_tmp244))({struct Cyc_String_pa_struct
_tmp248;_tmp248.tag=0;_tmp248.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v));{
struct Cyc_String_pa_struct _tmp247;_tmp247.tag=0;_tmp247.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp244));{void*_tmp245[2]={& _tmp247,&
_tmp248};Cyc_Tcutil_terr(loc,({const char*_tmp246="expected %s but found %s";
_tag_dyneither(_tmp246,sizeof(char),25);}),_tag_dyneither(_tmp245,sizeof(void*),
2));}}});return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}static void*
Cyc_Tcexp_tcInstantiate(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,
void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*ts){Cyc_Tcexp_tcExpNoInst(
te,0,e);((struct Cyc_Core_Opt*)_check_null(e->topt))->v=(void*)Cyc_Absyn_pointer_expand((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,0);{void*t1=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);{void*_tmp249=t1;struct Cyc_Absyn_PtrInfo
_tmp24B;void*_tmp24C;struct Cyc_Absyn_Tqual _tmp24D;struct Cyc_Absyn_PtrAtts _tmp24E;
void*_tmp24F;union Cyc_Absyn_Constraint*_tmp250;union Cyc_Absyn_Constraint*_tmp251;
union Cyc_Absyn_Constraint*_tmp252;_LL129: {struct Cyc_Absyn_PointerType_struct*
_tmp24A=(struct Cyc_Absyn_PointerType_struct*)_tmp249;if(_tmp24A->tag != 5)goto
_LL12B;else{_tmp24B=_tmp24A->f1;_tmp24C=_tmp24B.elt_typ;_tmp24D=_tmp24B.elt_tq;
_tmp24E=_tmp24B.ptr_atts;_tmp24F=_tmp24E.rgn;_tmp250=_tmp24E.nullable;_tmp251=
_tmp24E.bounds;_tmp252=_tmp24E.zero_term;}}_LL12A:{void*_tmp253=Cyc_Tcutil_compress(
_tmp24C);struct Cyc_Absyn_FnInfo _tmp255;struct Cyc_List_List*_tmp256;struct Cyc_Core_Opt*
_tmp257;void*_tmp258;struct Cyc_List_List*_tmp259;int _tmp25A;struct Cyc_Absyn_VarargInfo*
_tmp25B;struct Cyc_List_List*_tmp25C;struct Cyc_List_List*_tmp25D;_LL12E: {struct
Cyc_Absyn_FnType_struct*_tmp254=(struct Cyc_Absyn_FnType_struct*)_tmp253;if(
_tmp254->tag != 10)goto _LL130;else{_tmp255=_tmp254->f1;_tmp256=_tmp255.tvars;
_tmp257=_tmp255.effect;_tmp258=_tmp255.ret_typ;_tmp259=_tmp255.args;_tmp25A=
_tmp255.c_varargs;_tmp25B=_tmp255.cyc_varargs;_tmp25C=_tmp255.rgn_po;_tmp25D=
_tmp255.attributes;}}_LL12F: {struct _RegionHandle*_tmp25E=Cyc_Tcenv_get_fnrgn(te);
struct Cyc_List_List*instantiation=0;for(0;ts != 0  && _tmp256 != 0;(ts=ts->tl,
_tmp256=_tmp256->tl)){enum Cyc_Absyn_Kind k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)
_tmp256->hd,Cyc_Absyn_BoxKind);Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(
te),k,1,(void*)ts->hd);instantiation=({struct Cyc_List_List*_tmp25F=
_region_malloc(_tmp25E,sizeof(*_tmp25F));_tmp25F->hd=({struct _tuple11*_tmp260=
_region_malloc(_tmp25E,sizeof(*_tmp260));_tmp260->f1=(struct Cyc_Absyn_Tvar*)
_tmp256->hd;_tmp260->f2=(void*)ts->hd;_tmp260;});_tmp25F->tl=instantiation;
_tmp25F;});}if(ts != 0)return({void*_tmp261=0;Cyc_Tcexp_expr_err(te,loc,topt,({
const char*_tmp262="too many type variables in instantiation";_tag_dyneither(
_tmp262,sizeof(char),41);}),_tag_dyneither(_tmp261,sizeof(void*),0));});{void*
new_fn_typ=Cyc_Tcutil_rsubstitute(_tmp25E,instantiation,(void*)({struct Cyc_Absyn_FnType_struct*
_tmp267=_cycalloc(sizeof(*_tmp267));_tmp267[0]=({struct Cyc_Absyn_FnType_struct
_tmp268;_tmp268.tag=10;_tmp268.f1=({struct Cyc_Absyn_FnInfo _tmp269;_tmp269.tvars=
_tmp256;_tmp269.effect=_tmp257;_tmp269.ret_typ=_tmp258;_tmp269.args=_tmp259;
_tmp269.c_varargs=_tmp25A;_tmp269.cyc_varargs=_tmp25B;_tmp269.rgn_po=_tmp25C;
_tmp269.attributes=_tmp25D;_tmp269;});_tmp268;});_tmp267;}));return(void*)({
struct Cyc_Absyn_PointerType_struct*_tmp263=_cycalloc(sizeof(*_tmp263));_tmp263[0]=({
struct Cyc_Absyn_PointerType_struct _tmp264;_tmp264.tag=5;_tmp264.f1=({struct Cyc_Absyn_PtrInfo
_tmp265;_tmp265.elt_typ=new_fn_typ;_tmp265.elt_tq=_tmp24D;_tmp265.ptr_atts=({
struct Cyc_Absyn_PtrAtts _tmp266;_tmp266.rgn=_tmp24F;_tmp266.nullable=_tmp250;
_tmp266.bounds=_tmp251;_tmp266.zero_term=_tmp252;_tmp266.ptrloc=0;_tmp266;});
_tmp265;});_tmp264;});_tmp263;});}}_LL130:;_LL131: goto _LL12D;_LL12D:;}goto _LL128;
_LL12B:;_LL12C: goto _LL128;_LL128:;}return({struct Cyc_String_pa_struct _tmp26C;
_tmp26C.tag=0;_tmp26C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t1));{void*_tmp26A[1]={& _tmp26C};Cyc_Tcexp_expr_err(te,loc,topt,({const char*
_tmp26B="expecting polymorphic type but found %s";_tag_dyneither(_tmp26B,sizeof(
char),40);}),_tag_dyneither(_tmp26A,sizeof(void*),1));}});}}static void*Cyc_Tcexp_tcCast(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,void*t,struct
Cyc_Absyn_Exp*e,enum Cyc_Absyn_Coercion*c){Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(
te),Cyc_Absyn_MemKind,1,t);Cyc_Tcexp_tcExp(te,(void**)& t,e);{void*t2=(void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v;if(Cyc_Tcutil_silent_castable(te,
loc,t2,t))*((enum Cyc_Absyn_Coercion*)_check_null(c))=Cyc_Absyn_No_coercion;
else{enum Cyc_Absyn_Coercion crc=Cyc_Tcutil_castable(te,loc,t2,t);if(crc != Cyc_Absyn_Unknown_coercion)*((
enum Cyc_Absyn_Coercion*)_check_null(c))=crc;else{if(Cyc_Tcutil_zero_to_null(te,
t,e))*((enum Cyc_Absyn_Coercion*)_check_null(c))=Cyc_Absyn_No_coercion;else{Cyc_Tcutil_unify(
t2,t);{void*_tmp26D=({struct Cyc_String_pa_struct _tmp271;_tmp271.tag=0;_tmp271.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));{struct
Cyc_String_pa_struct _tmp270;_tmp270.tag=0;_tmp270.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2));{void*_tmp26E[2]={& _tmp270,&
_tmp271};Cyc_Tcexp_expr_err(te,loc,(void**)& t,({const char*_tmp26F="cannot cast %s to %s";
_tag_dyneither(_tmp26F,sizeof(char),21);}),_tag_dyneither(_tmp26E,sizeof(void*),
2));}}});Cyc_Tcutil_explain_failure();return _tmp26D;}}}}{struct _tuple0 _tmp273=({
struct _tuple0 _tmp272;_tmp272.f1=e->r;_tmp272.f2=Cyc_Tcutil_compress(t);_tmp272;});
void*_tmp274;struct Cyc_Absyn_MallocInfo _tmp276;int _tmp277;void*_tmp278;struct Cyc_Absyn_PtrInfo
_tmp27A;struct Cyc_Absyn_PtrAtts _tmp27B;union Cyc_Absyn_Constraint*_tmp27C;union
Cyc_Absyn_Constraint*_tmp27D;union Cyc_Absyn_Constraint*_tmp27E;_LL133: _tmp274=
_tmp273.f1;{struct Cyc_Absyn_Malloc_e_struct*_tmp275=(struct Cyc_Absyn_Malloc_e_struct*)
_tmp274;if(_tmp275->tag != 35)goto _LL135;else{_tmp276=_tmp275->f1;_tmp277=_tmp276.fat_result;}}
_tmp278=_tmp273.f2;{struct Cyc_Absyn_PointerType_struct*_tmp279=(struct Cyc_Absyn_PointerType_struct*)
_tmp278;if(_tmp279->tag != 5)goto _LL135;else{_tmp27A=_tmp279->f1;_tmp27B=_tmp27A.ptr_atts;
_tmp27C=_tmp27B.nullable;_tmp27D=_tmp27B.bounds;_tmp27E=_tmp27B.zero_term;}}
_LL134: if((_tmp277  && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
0,_tmp27E)) && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
0,_tmp27C)){void*_tmp27F=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,_tmp27D);struct Cyc_Absyn_Exp*_tmp281;_LL138: {struct Cyc_Absyn_Upper_b_struct*
_tmp280=(struct Cyc_Absyn_Upper_b_struct*)_tmp27F;if(_tmp280->tag != 1)goto _LL13A;
else{_tmp281=_tmp280->f1;}}_LL139: if((Cyc_Evexp_eval_const_uint_exp(_tmp281)).f1
== 1)({void*_tmp282=0;Cyc_Tcutil_warn(loc,({const char*_tmp283="cast from ? pointer to * pointer will lose size information";
_tag_dyneither(_tmp283,sizeof(char),60);}),_tag_dyneither(_tmp282,sizeof(void*),
0));});goto _LL137;_LL13A:;_LL13B: goto _LL137;_LL137:;}goto _LL132;_LL135:;_LL136:
goto _LL132;_LL132:;}return t;}}static void*Cyc_Tcexp_tcAddress(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Exp*e0,void**topt,struct Cyc_Absyn_Exp*
e){void**_tmp284=0;struct Cyc_Absyn_Tqual _tmp285=Cyc_Absyn_empty_tqual(0);if(topt
!= 0){void*_tmp286=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo _tmp288;
void*_tmp289;struct Cyc_Absyn_Tqual _tmp28A;struct Cyc_Absyn_PtrAtts _tmp28B;union
Cyc_Absyn_Constraint*_tmp28C;_LL13D: {struct Cyc_Absyn_PointerType_struct*_tmp287=(
struct Cyc_Absyn_PointerType_struct*)_tmp286;if(_tmp287->tag != 5)goto _LL13F;else{
_tmp288=_tmp287->f1;_tmp289=_tmp288.elt_typ;_tmp28A=_tmp288.elt_tq;_tmp28B=
_tmp288.ptr_atts;_tmp28C=_tmp28B.zero_term;}}_LL13E: _tmp284=({void**_tmp28D=
_cycalloc(sizeof(*_tmp28D));_tmp28D[0]=_tmp289;_tmp28D;});_tmp285=_tmp28A;goto
_LL13C;_LL13F:;_LL140: goto _LL13C;_LL13C:;}{struct _RegionHandle*_tmp28E=Cyc_Tcenv_get_fnrgn(
te);Cyc_Tcexp_tcExpNoInst(Cyc_Tcenv_clear_notreadctxt(_tmp28E,te),_tmp284,e);if(
Cyc_Tcutil_is_noalias_path(_tmp28E,e))({void*_tmp28F=0;Cyc_Tcutil_terr(e->loc,({
const char*_tmp290="Cannot take the address of an alias-free path";_tag_dyneither(
_tmp290,sizeof(char),46);}),_tag_dyneither(_tmp28F,sizeof(void*),0));});{void*
_tmp291=e->r;struct Cyc_Absyn_Exp*_tmp293;struct Cyc_Absyn_Exp*_tmp294;_LL142: {
struct Cyc_Absyn_Subscript_e_struct*_tmp292=(struct Cyc_Absyn_Subscript_e_struct*)
_tmp291;if(_tmp292->tag != 25)goto _LL144;else{_tmp293=_tmp292->f1;_tmp294=_tmp292->f2;}}
_LL143:{void*_tmp295=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp293->topt))->v);_LL147: {struct Cyc_Absyn_TupleType_struct*
_tmp296=(struct Cyc_Absyn_TupleType_struct*)_tmp295;if(_tmp296->tag != 11)goto
_LL149;}_LL148: goto _LL146;_LL149:;_LL14A: e0->r=(Cyc_Absyn_add_exp(_tmp293,
_tmp294,0))->r;return Cyc_Tcexp_tcPlus(te,_tmp293,_tmp294);_LL146:;}goto _LL141;
_LL144:;_LL145: goto _LL141;_LL141:;}{void*_tmp297=e->r;int _tmp299;int _tmp29B;
_LL14C: {struct Cyc_Absyn_AggrMember_e_struct*_tmp298=(struct Cyc_Absyn_AggrMember_e_struct*)
_tmp297;if(_tmp298->tag != 23)goto _LL14E;else{_tmp299=_tmp298->f3;if(_tmp299 != 1)
goto _LL14E;}}_LL14D: goto _LL14F;_LL14E: {struct Cyc_Absyn_AggrArrow_e_struct*
_tmp29A=(struct Cyc_Absyn_AggrArrow_e_struct*)_tmp297;if(_tmp29A->tag != 24)goto
_LL150;else{_tmp29B=_tmp29A->f3;if(_tmp29B != 1)goto _LL150;}}_LL14F:({void*
_tmp29C=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp29D="cannot take the address of a @tagged union member";
_tag_dyneither(_tmp29D,sizeof(char),50);}),_tag_dyneither(_tmp29C,sizeof(void*),
0));});goto _LL14B;_LL150:;_LL151: goto _LL14B;_LL14B:;}{int _tmp29F;void*_tmp2A0;
struct _tuple12 _tmp29E=Cyc_Tcutil_addressof_props(te,e);_tmp29F=_tmp29E.f1;
_tmp2A0=_tmp29E.f2;{struct Cyc_Absyn_Tqual tq=Cyc_Absyn_empty_tqual(0);if(_tmp29F){
tq.print_const=1;tq.real_const=1;}{void*t=Cyc_Absyn_at_typ((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,_tmp2A0,tq,Cyc_Absyn_false_conref);return t;}}}}}static
void*Cyc_Tcexp_tcSizeof(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,
void**topt,void*t){if(te->allow_valueof)return Cyc_Absyn_uint_typ;Cyc_Tcutil_check_type(
loc,te,Cyc_Tcenv_lookup_type_vars(te),Cyc_Absyn_MemKind,1,t);if(!Cyc_Evexp_okay_szofarg(
t))({struct Cyc_String_pa_struct _tmp2A3;_tmp2A3.tag=0;_tmp2A3.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));{void*_tmp2A1[1]={&
_tmp2A3};Cyc_Tcutil_terr(loc,({const char*_tmp2A2="sizeof applied to type %s, which has unknown size here";
_tag_dyneither(_tmp2A2,sizeof(char),55);}),_tag_dyneither(_tmp2A1,sizeof(void*),
1));}});if(topt != 0){void*_tmp2A4=Cyc_Tcutil_compress(*topt);void*_tmp2A6;_LL153: {
struct Cyc_Absyn_TagType_struct*_tmp2A5=(struct Cyc_Absyn_TagType_struct*)_tmp2A4;
if(_tmp2A5->tag != 20)goto _LL155;else{_tmp2A6=(void*)_tmp2A5->f1;}}_LL154: {
struct Cyc_Absyn_Exp*_tmp2A7=Cyc_Absyn_sizeoftyp_exp(t,0);struct Cyc_Absyn_ValueofType_struct*
_tmp2A8=({struct Cyc_Absyn_ValueofType_struct*_tmp2A9=_cycalloc(sizeof(*_tmp2A9));
_tmp2A9[0]=({struct Cyc_Absyn_ValueofType_struct _tmp2AA;_tmp2AA.tag=19;_tmp2AA.f1=
_tmp2A7;_tmp2AA;});_tmp2A9;});if(Cyc_Tcutil_unify(_tmp2A6,(void*)_tmp2A8))return
_tmp2A4;goto _LL152;}_LL155:;_LL156: goto _LL152;_LL152:;}return Cyc_Absyn_uint_typ;}
int Cyc_Tcexp_structfield_has_name(struct _dyneither_ptr*n,struct Cyc_Absyn_Aggrfield*
sf){return Cyc_strcmp((struct _dyneither_ptr)*n,(struct _dyneither_ptr)*sf->name)== 
0;}static void*Cyc_Tcexp_tcOffsetof(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,void*t,void*n){Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(
te),Cyc_Absyn_MemKind,1,t);{void*_tmp2AB=n;struct _dyneither_ptr*_tmp2AD;
unsigned int _tmp2AF;_LL158: {struct Cyc_Absyn_StructField_struct*_tmp2AC=(struct
Cyc_Absyn_StructField_struct*)_tmp2AB;if(_tmp2AC->tag != 0)goto _LL15A;else{
_tmp2AD=_tmp2AC->f1;}}_LL159: {int bad_type=1;{void*_tmp2B0=Cyc_Tcutil_compress(t);
struct Cyc_Absyn_AggrInfo _tmp2B2;union Cyc_Absyn_AggrInfoU _tmp2B3;struct Cyc_Absyn_Aggrdecl**
_tmp2B4;struct Cyc_List_List*_tmp2B6;_LL15D: {struct Cyc_Absyn_AggrType_struct*
_tmp2B1=(struct Cyc_Absyn_AggrType_struct*)_tmp2B0;if(_tmp2B1->tag != 12)goto
_LL15F;else{_tmp2B2=_tmp2B1->f1;_tmp2B3=_tmp2B2.aggr_info;if((_tmp2B3.KnownAggr).tag
!= 2)goto _LL15F;_tmp2B4=(struct Cyc_Absyn_Aggrdecl**)(_tmp2B3.KnownAggr).val;}}
_LL15E: if((*_tmp2B4)->impl == 0)goto _LL15C;if(!((int(*)(int(*pred)(struct
_dyneither_ptr*,struct Cyc_Absyn_Aggrfield*),struct _dyneither_ptr*env,struct Cyc_List_List*
x))Cyc_List_exists_c)(Cyc_Tcexp_structfield_has_name,_tmp2AD,((struct Cyc_Absyn_AggrdeclImpl*)
_check_null((*_tmp2B4)->impl))->fields))({struct Cyc_String_pa_struct _tmp2B9;
_tmp2B9.tag=0;_tmp2B9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp2AD);{
void*_tmp2B7[1]={& _tmp2B9};Cyc_Tcutil_terr(loc,({const char*_tmp2B8="no field of struct/union has name %s";
_tag_dyneither(_tmp2B8,sizeof(char),37);}),_tag_dyneither(_tmp2B7,sizeof(void*),
1));}});bad_type=0;goto _LL15C;_LL15F: {struct Cyc_Absyn_AnonAggrType_struct*
_tmp2B5=(struct Cyc_Absyn_AnonAggrType_struct*)_tmp2B0;if(_tmp2B5->tag != 13)goto
_LL161;else{_tmp2B6=_tmp2B5->f2;}}_LL160: if(!((int(*)(int(*pred)(struct
_dyneither_ptr*,struct Cyc_Absyn_Aggrfield*),struct _dyneither_ptr*env,struct Cyc_List_List*
x))Cyc_List_exists_c)(Cyc_Tcexp_structfield_has_name,_tmp2AD,_tmp2B6))({struct
Cyc_String_pa_struct _tmp2BC;_tmp2BC.tag=0;_tmp2BC.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)*_tmp2AD);{void*_tmp2BA[1]={& _tmp2BC};Cyc_Tcutil_terr(loc,({
const char*_tmp2BB="no field of struct/union has name %s";_tag_dyneither(_tmp2BB,
sizeof(char),37);}),_tag_dyneither(_tmp2BA,sizeof(void*),1));}});bad_type=0;goto
_LL15C;_LL161:;_LL162: goto _LL15C;_LL15C:;}if(bad_type)({struct Cyc_String_pa_struct
_tmp2BF;_tmp2BF.tag=0;_tmp2BF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t));{void*_tmp2BD[1]={& _tmp2BF};Cyc_Tcutil_terr(loc,({
const char*_tmp2BE="%s is not a known struct/union type";_tag_dyneither(_tmp2BE,
sizeof(char),36);}),_tag_dyneither(_tmp2BD,sizeof(void*),1));}});goto _LL157;}
_LL15A: {struct Cyc_Absyn_TupleIndex_struct*_tmp2AE=(struct Cyc_Absyn_TupleIndex_struct*)
_tmp2AB;if(_tmp2AE->tag != 1)goto _LL157;else{_tmp2AF=_tmp2AE->f1;}}_LL15B: {int
bad_type=1;{void*_tmp2C0=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo _tmp2C2;
union Cyc_Absyn_AggrInfoU _tmp2C3;struct Cyc_Absyn_Aggrdecl**_tmp2C4;struct Cyc_List_List*
_tmp2C6;struct Cyc_List_List*_tmp2C8;struct Cyc_Absyn_DatatypeFieldInfo _tmp2CA;
union Cyc_Absyn_DatatypeFieldInfoU _tmp2CB;struct _tuple2 _tmp2CC;struct Cyc_Absyn_Datatypefield*
_tmp2CD;_LL164: {struct Cyc_Absyn_AggrType_struct*_tmp2C1=(struct Cyc_Absyn_AggrType_struct*)
_tmp2C0;if(_tmp2C1->tag != 12)goto _LL166;else{_tmp2C2=_tmp2C1->f1;_tmp2C3=_tmp2C2.aggr_info;
if((_tmp2C3.KnownAggr).tag != 2)goto _LL166;_tmp2C4=(struct Cyc_Absyn_Aggrdecl**)(
_tmp2C3.KnownAggr).val;}}_LL165: if((*_tmp2C4)->impl == 0)goto _LL163;_tmp2C6=((
struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmp2C4)->impl))->fields;goto _LL167;
_LL166: {struct Cyc_Absyn_AnonAggrType_struct*_tmp2C5=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp2C0;if(_tmp2C5->tag != 13)goto _LL168;else{_tmp2C6=_tmp2C5->f2;}}_LL167: if(((
int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp2C6)<= _tmp2AF)({struct Cyc_Int_pa_struct
_tmp2D1;_tmp2D1.tag=1;_tmp2D1.f1=(unsigned long)((int)_tmp2AF);{struct Cyc_Int_pa_struct
_tmp2D0;_tmp2D0.tag=1;_tmp2D0.f1=(unsigned long)((int(*)(struct Cyc_List_List*x))
Cyc_List_length)(_tmp2C6);{void*_tmp2CE[2]={& _tmp2D0,& _tmp2D1};Cyc_Tcutil_terr(
loc,({const char*_tmp2CF="struct/union has too few components: %d <= %d";
_tag_dyneither(_tmp2CF,sizeof(char),46);}),_tag_dyneither(_tmp2CE,sizeof(void*),
2));}}});bad_type=0;goto _LL163;_LL168: {struct Cyc_Absyn_TupleType_struct*_tmp2C7=(
struct Cyc_Absyn_TupleType_struct*)_tmp2C0;if(_tmp2C7->tag != 11)goto _LL16A;else{
_tmp2C8=_tmp2C7->f1;}}_LL169: if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp2C8)<= _tmp2AF)({struct Cyc_Int_pa_struct _tmp2D5;_tmp2D5.tag=1;_tmp2D5.f1=(
unsigned long)((int)_tmp2AF);{struct Cyc_Int_pa_struct _tmp2D4;_tmp2D4.tag=1;
_tmp2D4.f1=(unsigned long)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp2C8);{void*_tmp2D2[2]={& _tmp2D4,& _tmp2D5};Cyc_Tcutil_terr(loc,({const char*
_tmp2D3="tuple has too few components: %d <= %d";_tag_dyneither(_tmp2D3,sizeof(
char),39);}),_tag_dyneither(_tmp2D2,sizeof(void*),2));}}});bad_type=0;goto _LL163;
_LL16A: {struct Cyc_Absyn_DatatypeFieldType_struct*_tmp2C9=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp2C0;if(_tmp2C9->tag != 4)goto _LL16C;else{_tmp2CA=_tmp2C9->f1;_tmp2CB=_tmp2CA.field_info;
if((_tmp2CB.KnownDatatypefield).tag != 2)goto _LL16C;_tmp2CC=(struct _tuple2)(
_tmp2CB.KnownDatatypefield).val;_tmp2CD=_tmp2CC.f2;}}_LL16B: if(((int(*)(struct
Cyc_List_List*x))Cyc_List_length)(_tmp2CD->typs)< _tmp2AF)({struct Cyc_Int_pa_struct
_tmp2D9;_tmp2D9.tag=1;_tmp2D9.f1=(unsigned long)((int)_tmp2AF);{struct Cyc_Int_pa_struct
_tmp2D8;_tmp2D8.tag=1;_tmp2D8.f1=(unsigned long)((int(*)(struct Cyc_List_List*x))
Cyc_List_length)(_tmp2CD->typs);{void*_tmp2D6[2]={& _tmp2D8,& _tmp2D9};Cyc_Tcutil_terr(
loc,({const char*_tmp2D7="datatype field has too few components: %d < %d";
_tag_dyneither(_tmp2D7,sizeof(char),47);}),_tag_dyneither(_tmp2D6,sizeof(void*),
2));}}});bad_type=0;goto _LL163;_LL16C:;_LL16D: goto _LL163;_LL163:;}if(bad_type)({
struct Cyc_String_pa_struct _tmp2DC;_tmp2DC.tag=0;_tmp2DC.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));{void*_tmp2DA[1]={& _tmp2DC};Cyc_Tcutil_terr(
loc,({const char*_tmp2DB="%s is not a known type";_tag_dyneither(_tmp2DB,sizeof(
char),23);}),_tag_dyneither(_tmp2DA,sizeof(void*),1));}});goto _LL157;}_LL157:;}
return Cyc_Absyn_uint_typ;}static void*Cyc_Tcexp_tcDeref(struct Cyc_Tcenv_Tenv*
te_orig,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e){struct
_RegionHandle*_tmp2DD=Cyc_Tcenv_get_fnrgn(te_orig);struct Cyc_Tcenv_Tenv*_tmp2DE=
Cyc_Tcenv_clear_notreadctxt(_tmp2DD,te_orig);Cyc_Tcexp_tcExp(_tmp2DE,0,e);{void*
t=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
void*_tmp2DF=t;struct Cyc_Absyn_PtrInfo _tmp2E2;void*_tmp2E3;struct Cyc_Absyn_PtrAtts
_tmp2E4;void*_tmp2E5;union Cyc_Absyn_Constraint*_tmp2E6;union Cyc_Absyn_Constraint*
_tmp2E7;_LL16F: {struct Cyc_Absyn_Evar_struct*_tmp2E0=(struct Cyc_Absyn_Evar_struct*)
_tmp2DF;if(_tmp2E0->tag != 1)goto _LL171;}_LL170: {struct Cyc_List_List*_tmp2E8=Cyc_Tcenv_lookup_type_vars(
_tmp2DE);void*_tmp2E9=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_mk,({
struct Cyc_Core_Opt*_tmp2F4=_cycalloc(sizeof(*_tmp2F4));_tmp2F4->v=_tmp2E8;
_tmp2F4;}));void*_tmp2EA=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_trk,({
struct Cyc_Core_Opt*_tmp2F3=_cycalloc(sizeof(*_tmp2F3));_tmp2F3->v=_tmp2E8;
_tmp2F3;}));union Cyc_Absyn_Constraint*_tmp2EB=((union Cyc_Absyn_Constraint*(*)())
Cyc_Absyn_empty_conref)();union Cyc_Absyn_Constraint*_tmp2EC=((union Cyc_Absyn_Constraint*(*)())
Cyc_Absyn_empty_conref)();struct Cyc_Absyn_PtrAtts _tmp2ED=({struct Cyc_Absyn_PtrAtts
_tmp2F2;_tmp2F2.rgn=_tmp2EA;_tmp2F2.nullable=((union Cyc_Absyn_Constraint*(*)())
Cyc_Absyn_empty_conref)();_tmp2F2.bounds=_tmp2EB;_tmp2F2.zero_term=_tmp2EC;
_tmp2F2.ptrloc=0;_tmp2F2;});struct Cyc_Absyn_PointerType_struct*_tmp2EE=({struct
Cyc_Absyn_PointerType_struct*_tmp2EF=_cycalloc(sizeof(*_tmp2EF));_tmp2EF[0]=({
struct Cyc_Absyn_PointerType_struct _tmp2F0;_tmp2F0.tag=5;_tmp2F0.f1=({struct Cyc_Absyn_PtrInfo
_tmp2F1;_tmp2F1.elt_typ=_tmp2E9;_tmp2F1.elt_tq=Cyc_Absyn_empty_tqual(0);_tmp2F1.ptr_atts=
_tmp2ED;_tmp2F1;});_tmp2F0;});_tmp2EF;});Cyc_Tcutil_unify(t,(void*)_tmp2EE);
_tmp2E3=_tmp2E9;_tmp2E5=_tmp2EA;_tmp2E6=_tmp2EB;_tmp2E7=_tmp2EC;goto _LL172;}
_LL171: {struct Cyc_Absyn_PointerType_struct*_tmp2E1=(struct Cyc_Absyn_PointerType_struct*)
_tmp2DF;if(_tmp2E1->tag != 5)goto _LL173;else{_tmp2E2=_tmp2E1->f1;_tmp2E3=_tmp2E2.elt_typ;
_tmp2E4=_tmp2E2.ptr_atts;_tmp2E5=_tmp2E4.rgn;_tmp2E6=_tmp2E4.bounds;_tmp2E7=
_tmp2E4.zero_term;}}_LL172: Cyc_Tcenv_check_rgn_accessible(_tmp2DE,loc,_tmp2E5);
Cyc_Tcutil_check_nonzero_bound(loc,_tmp2E6);if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(
_tmp2E3),Cyc_Absyn_MemKind))({void*_tmp2F5=0;Cyc_Tcutil_terr(loc,({const char*
_tmp2F6="can't dereference abstract pointer type";_tag_dyneither(_tmp2F6,sizeof(
char),40);}),_tag_dyneither(_tmp2F5,sizeof(void*),0));});return _tmp2E3;_LL173:;
_LL174: return({struct Cyc_String_pa_struct _tmp2F9;_tmp2F9.tag=0;_tmp2F9.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));{void*
_tmp2F7[1]={& _tmp2F9};Cyc_Tcexp_expr_err(_tmp2DE,loc,topt,({const char*_tmp2F8="expecting *, @, or ? type but found %s";
_tag_dyneither(_tmp2F8,sizeof(char),39);}),_tag_dyneither(_tmp2F7,sizeof(void*),
1));}});_LL16E:;}}static void*Cyc_Tcexp_tcAggrMember(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*outer_e,struct Cyc_Absyn_Exp*
e,struct _dyneither_ptr*f,int*is_tagged,int*is_read){Cyc_Tcexp_tcExpNoPromote(te,
0,e);*is_read=!Cyc_Tcenv_in_notreadctxt(te);{void*_tmp2FA=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);struct Cyc_Absyn_AggrInfo
_tmp2FC;union Cyc_Absyn_AggrInfoU _tmp2FD;struct Cyc_Absyn_Aggrdecl**_tmp2FE;struct
Cyc_Absyn_Aggrdecl*_tmp2FF;struct Cyc_List_List*_tmp300;enum Cyc_Absyn_AggrKind
_tmp302;struct Cyc_List_List*_tmp303;_LL176: {struct Cyc_Absyn_AggrType_struct*
_tmp2FB=(struct Cyc_Absyn_AggrType_struct*)_tmp2FA;if(_tmp2FB->tag != 12)goto
_LL178;else{_tmp2FC=_tmp2FB->f1;_tmp2FD=_tmp2FC.aggr_info;if((_tmp2FD.KnownAggr).tag
!= 2)goto _LL178;_tmp2FE=(struct Cyc_Absyn_Aggrdecl**)(_tmp2FD.KnownAggr).val;
_tmp2FF=*_tmp2FE;_tmp300=_tmp2FC.targs;}}_LL177: {struct Cyc_Absyn_Aggrfield*
_tmp308=Cyc_Absyn_lookup_decl_field(_tmp2FF,f);if(_tmp308 == 0)return({struct Cyc_String_pa_struct
_tmp30C;_tmp30C.tag=0;_tmp30C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);{
struct Cyc_String_pa_struct _tmp30B;_tmp30B.tag=0;_tmp30B.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp2FF->name));{void*_tmp309[2]={&
_tmp30B,& _tmp30C};Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp30A="type %s has no %s field";
_tag_dyneither(_tmp30A,sizeof(char),24);}),_tag_dyneither(_tmp309,sizeof(void*),
2));}}});if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp2FF->impl))->tagged)*
is_tagged=1;if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp2FF->impl))->exist_vars
!= 0)return({void*_tmp30D=0;Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp30E="must use pattern-matching to access fields of existential types";
_tag_dyneither(_tmp30E,sizeof(char),64);}),_tag_dyneither(_tmp30D,sizeof(void*),
0));});{void*t2;struct _RegionHandle*_tmp30F=Cyc_Tcenv_get_fnrgn(te);{struct Cyc_List_List*
_tmp310=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(_tmp30F,_tmp30F,
_tmp2FF->tvs,_tmp300);t2=Cyc_Tcutil_rsubstitute(_tmp30F,_tmp310,_tmp308->type);
if(((_tmp2FF->kind == Cyc_Absyn_UnionA  && !((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp2FF->impl))->tagged) && !Cyc_Tcenv_in_notreadctxt(te)) && !Cyc_Tcutil_bits_only(
t2))return({struct Cyc_String_pa_struct _tmp313;_tmp313.tag=0;_tmp313.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*f);{void*_tmp311[1]={& _tmp313};Cyc_Tcexp_expr_err(
te,loc,topt,({const char*_tmp312="cannot read union member %s since it is not `bits-only'";
_tag_dyneither(_tmp312,sizeof(char),56);}),_tag_dyneither(_tmp311,sizeof(void*),
1));}});}return t2;}}_LL178: {struct Cyc_Absyn_AnonAggrType_struct*_tmp301=(struct
Cyc_Absyn_AnonAggrType_struct*)_tmp2FA;if(_tmp301->tag != 13)goto _LL17A;else{
_tmp302=_tmp301->f1;_tmp303=_tmp301->f2;}}_LL179: {struct Cyc_Absyn_Aggrfield*
_tmp314=Cyc_Absyn_lookup_field(_tmp303,f);if(_tmp314 == 0)return({struct Cyc_String_pa_struct
_tmp317;_tmp317.tag=0;_tmp317.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);{
void*_tmp315[1]={& _tmp317};Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp316="type has no %s field";
_tag_dyneither(_tmp316,sizeof(char),21);}),_tag_dyneither(_tmp315,sizeof(void*),
1));}});if((_tmp302 == Cyc_Absyn_UnionA  && !Cyc_Tcenv_in_notreadctxt(te)) && !Cyc_Tcutil_bits_only(
_tmp314->type))return({struct Cyc_String_pa_struct _tmp31A;_tmp31A.tag=0;_tmp31A.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*f);{void*_tmp318[1]={& _tmp31A};Cyc_Tcexp_expr_err(
te,loc,topt,({const char*_tmp319="cannot read union member %s since it is not `bits-only'";
_tag_dyneither(_tmp319,sizeof(char),56);}),_tag_dyneither(_tmp318,sizeof(void*),
1));}});return _tmp314->type;}_LL17A:{struct Cyc_Absyn_ArrayType_struct*_tmp304=(
struct Cyc_Absyn_ArrayType_struct*)_tmp2FA;if(_tmp304->tag != 9)goto _LL17C;}if(!(
Cyc_strcmp((struct _dyneither_ptr)*f,({const char*_tmp305="size";_tag_dyneither(
_tmp305,sizeof(char),5);}))== 0))goto _LL17C;_LL17B: goto _LL17D;_LL17C:{struct Cyc_Absyn_PointerType_struct*
_tmp306=(struct Cyc_Absyn_PointerType_struct*)_tmp2FA;if(_tmp306->tag != 5)goto
_LL17E;}if(!(Cyc_strcmp((struct _dyneither_ptr)*f,({const char*_tmp307="size";
_tag_dyneither(_tmp307,sizeof(char),5);}))== 0))goto _LL17E;_LL17D:({struct Cyc_String_pa_struct
_tmp31D;_tmp31D.tag=0;_tmp31D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(e));{void*_tmp31B[1]={& _tmp31D};Cyc_Tcutil_warn(e->loc,({
const char*_tmp31C="deprecated `.size' used here -- change to numelts(%s)";
_tag_dyneither(_tmp31C,sizeof(char),54);}),_tag_dyneither(_tmp31B,sizeof(void*),
1));}});outer_e->r=(void*)({struct Cyc_Absyn_Primop_e_struct*_tmp31E=_cycalloc(
sizeof(*_tmp31E));_tmp31E[0]=({struct Cyc_Absyn_Primop_e_struct _tmp31F;_tmp31F.tag=
3;_tmp31F.f1=Cyc_Absyn_Numelts;_tmp31F.f2=({struct Cyc_List_List*_tmp320=
_cycalloc(sizeof(*_tmp320));_tmp320->hd=e;_tmp320->tl=0;_tmp320;});_tmp31F;});
_tmp31E;});return Cyc_Absyn_uint_typ;_LL17E:;_LL17F: if(Cyc_strcmp((struct
_dyneither_ptr)*f,({const char*_tmp321="size";_tag_dyneither(_tmp321,sizeof(char),
5);}))== 0)return({struct Cyc_String_pa_struct _tmp324;_tmp324.tag=0;_tmp324.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v));{void*_tmp322[1]={& _tmp324};Cyc_Tcexp_expr_err(
te,loc,topt,({const char*_tmp323="expecting struct, union, or array, found %s";
_tag_dyneither(_tmp323,sizeof(char),44);}),_tag_dyneither(_tmp322,sizeof(void*),
1));}});else{return({struct Cyc_String_pa_struct _tmp327;_tmp327.tag=0;_tmp327.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v));{void*_tmp325[1]={& _tmp327};Cyc_Tcexp_expr_err(
te,loc,topt,({const char*_tmp326="expecting struct or union, found %s";
_tag_dyneither(_tmp326,sizeof(char),36);}),_tag_dyneither(_tmp325,sizeof(void*),
1));}});}_LL175:;}}static void*Cyc_Tcexp_tcAggrArrow(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e,struct
_dyneither_ptr*f,int*is_tagged,int*is_read){struct _RegionHandle*_tmp328=Cyc_Tcenv_get_fnrgn(
te);Cyc_Tcexp_tcExp(Cyc_Tcenv_clear_notreadctxt(_tmp328,te),0,e);*is_read=!Cyc_Tcenv_in_notreadctxt(
te);{void*_tmp329=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
struct Cyc_Absyn_PtrInfo _tmp32B;void*_tmp32C;struct Cyc_Absyn_PtrAtts _tmp32D;void*
_tmp32E;union Cyc_Absyn_Constraint*_tmp32F;union Cyc_Absyn_Constraint*_tmp330;
_LL181: {struct Cyc_Absyn_PointerType_struct*_tmp32A=(struct Cyc_Absyn_PointerType_struct*)
_tmp329;if(_tmp32A->tag != 5)goto _LL183;else{_tmp32B=_tmp32A->f1;_tmp32C=_tmp32B.elt_typ;
_tmp32D=_tmp32B.ptr_atts;_tmp32E=_tmp32D.rgn;_tmp32F=_tmp32D.bounds;_tmp330=
_tmp32D.zero_term;}}_LL182: Cyc_Tcutil_check_nonzero_bound(loc,_tmp32F);{void*
_tmp331=Cyc_Tcutil_compress(_tmp32C);struct Cyc_Absyn_AggrInfo _tmp333;union Cyc_Absyn_AggrInfoU
_tmp334;struct Cyc_Absyn_Aggrdecl**_tmp335;struct Cyc_Absyn_Aggrdecl*_tmp336;
struct Cyc_List_List*_tmp337;enum Cyc_Absyn_AggrKind _tmp339;struct Cyc_List_List*
_tmp33A;_LL186: {struct Cyc_Absyn_AggrType_struct*_tmp332=(struct Cyc_Absyn_AggrType_struct*)
_tmp331;if(_tmp332->tag != 12)goto _LL188;else{_tmp333=_tmp332->f1;_tmp334=_tmp333.aggr_info;
if((_tmp334.KnownAggr).tag != 2)goto _LL188;_tmp335=(struct Cyc_Absyn_Aggrdecl**)(
_tmp334.KnownAggr).val;_tmp336=*_tmp335;_tmp337=_tmp333.targs;}}_LL187: {struct
Cyc_Absyn_Aggrfield*_tmp33B=Cyc_Absyn_lookup_decl_field(_tmp336,f);if(_tmp33B == 
0)return({struct Cyc_String_pa_struct _tmp33F;_tmp33F.tag=0;_tmp33F.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*f);{struct Cyc_String_pa_struct _tmp33E;
_tmp33E.tag=0;_tmp33E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp336->name));{void*_tmp33C[2]={& _tmp33E,& _tmp33F};Cyc_Tcexp_expr_err(te,loc,
topt,({const char*_tmp33D="type %s has no %s field";_tag_dyneither(_tmp33D,
sizeof(char),24);}),_tag_dyneither(_tmp33C,sizeof(void*),2));}}});if(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp336->impl))->tagged)*is_tagged=1;if(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp336->impl))->exist_vars != 0)return({void*_tmp340=0;Cyc_Tcexp_expr_err(
te,loc,topt,({const char*_tmp341="must use pattern-matching to access fields of existential types";
_tag_dyneither(_tmp341,sizeof(char),64);}),_tag_dyneither(_tmp340,sizeof(void*),
0));});{void*t3;struct _RegionHandle*_tmp342=Cyc_Tcenv_get_fnrgn(te);{struct Cyc_List_List*
_tmp343=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(_tmp342,_tmp342,
_tmp336->tvs,_tmp337);t3=Cyc_Tcutil_rsubstitute(_tmp342,_tmp343,_tmp33B->type);}
if(((_tmp336->kind == Cyc_Absyn_UnionA  && !((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp336->impl))->tagged) && !Cyc_Tcenv_in_notreadctxt(te)) && !Cyc_Tcutil_bits_only(
t3))return({struct Cyc_String_pa_struct _tmp346;_tmp346.tag=0;_tmp346.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*f);{void*_tmp344[1]={& _tmp346};Cyc_Tcexp_expr_err(
te,loc,topt,({const char*_tmp345="cannot read union member %s since it is not `bits-only'";
_tag_dyneither(_tmp345,sizeof(char),56);}),_tag_dyneither(_tmp344,sizeof(void*),
1));}});return t3;}}_LL188: {struct Cyc_Absyn_AnonAggrType_struct*_tmp338=(struct
Cyc_Absyn_AnonAggrType_struct*)_tmp331;if(_tmp338->tag != 13)goto _LL18A;else{
_tmp339=_tmp338->f1;_tmp33A=_tmp338->f2;}}_LL189: {struct Cyc_Absyn_Aggrfield*
_tmp347=Cyc_Absyn_lookup_field(_tmp33A,f);if(_tmp347 == 0)return({struct Cyc_String_pa_struct
_tmp34A;_tmp34A.tag=0;_tmp34A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);{
void*_tmp348[1]={& _tmp34A};Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp349="type has no %s field";
_tag_dyneither(_tmp349,sizeof(char),21);}),_tag_dyneither(_tmp348,sizeof(void*),
1));}});if((_tmp339 == Cyc_Absyn_UnionA  && !Cyc_Tcenv_in_notreadctxt(te)) && !Cyc_Tcutil_bits_only(
_tmp347->type))return({struct Cyc_String_pa_struct _tmp34D;_tmp34D.tag=0;_tmp34D.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*f);{void*_tmp34B[1]={& _tmp34D};Cyc_Tcexp_expr_err(
te,loc,topt,({const char*_tmp34C="cannot read union member %s since it is not `bits-only'";
_tag_dyneither(_tmp34C,sizeof(char),56);}),_tag_dyneither(_tmp34B,sizeof(void*),
1));}});return _tmp347->type;}_LL18A:;_LL18B: goto _LL185;_LL185:;}goto _LL180;
_LL183:;_LL184: goto _LL180;_LL180:;}return({struct Cyc_String_pa_struct _tmp350;
_tmp350.tag=0;_tmp350.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v));{void*_tmp34E[1]={&
_tmp350};Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp34F="expecting struct or union pointer, found %s";
_tag_dyneither(_tmp34F,sizeof(char),44);}),_tag_dyneither(_tmp34E,sizeof(void*),
1));}});}static void*Cyc_Tcexp_ithTupleType(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,struct Cyc_List_List*ts,struct Cyc_Absyn_Exp*index){unsigned int _tmp352;int
_tmp353;struct _tuple13 _tmp351=Cyc_Evexp_eval_const_uint_exp(index);_tmp352=
_tmp351.f1;_tmp353=_tmp351.f2;if(!_tmp353)return({void*_tmp354=0;Cyc_Tcexp_expr_err(
te,loc,0,({const char*_tmp355="tuple projection cannot use sizeof or offsetof";
_tag_dyneither(_tmp355,sizeof(char),47);}),_tag_dyneither(_tmp354,sizeof(void*),
0));});{struct _handler_cons _tmp356;_push_handler(& _tmp356);{int _tmp358=0;if(
setjmp(_tmp356.handler))_tmp358=1;if(!_tmp358){{void*_tmp359=(*((struct _tuple17*(*)(
struct Cyc_List_List*x,int n))Cyc_List_nth)(ts,(int)_tmp352)).f2;_npop_handler(0);
return _tmp359;};_pop_handler();}else{void*_tmp357=(void*)_exn_thrown;void*
_tmp35B=_tmp357;_LL18D: {struct Cyc_List_Nth_struct*_tmp35C=(struct Cyc_List_Nth_struct*)
_tmp35B;if(_tmp35C->tag != Cyc_List_Nth)goto _LL18F;}_LL18E: return({struct Cyc_Int_pa_struct
_tmp360;_tmp360.tag=1;_tmp360.f1=(unsigned long)((int(*)(struct Cyc_List_List*x))
Cyc_List_length)(ts);{struct Cyc_Int_pa_struct _tmp35F;_tmp35F.tag=1;_tmp35F.f1=(
unsigned long)((int)_tmp352);{void*_tmp35D[2]={& _tmp35F,& _tmp360};Cyc_Tcexp_expr_err(
te,loc,0,({const char*_tmp35E="index is %d but tuple has only %d fields";
_tag_dyneither(_tmp35E,sizeof(char),41);}),_tag_dyneither(_tmp35D,sizeof(void*),
2));}}});_LL18F:;_LL190:(void)_throw(_tmp35B);_LL18C:;}}}}static void*Cyc_Tcexp_tcSubscript(
struct Cyc_Tcenv_Tenv*te_orig,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
e1,struct Cyc_Absyn_Exp*e2){struct _RegionHandle*_tmp361=Cyc_Tcenv_get_fnrgn(
te_orig);struct Cyc_Tcenv_Tenv*_tmp362=Cyc_Tcenv_clear_notreadctxt(_tmp361,
te_orig);Cyc_Tcexp_tcExp(_tmp362,0,e1);Cyc_Tcexp_tcExp(_tmp362,0,e2);{void*t1=
Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v);void*
t2=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v);
if(!Cyc_Tcutil_coerce_sint_typ(_tmp362,e2))return({struct Cyc_String_pa_struct
_tmp365;_tmp365.tag=0;_tmp365.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2));{void*_tmp363[1]={& _tmp365};Cyc_Tcexp_expr_err(
_tmp362,e2->loc,topt,({const char*_tmp364="expecting int subscript, found %s";
_tag_dyneither(_tmp364,sizeof(char),34);}),_tag_dyneither(_tmp363,sizeof(void*),
1));}});{void*_tmp366=t1;struct Cyc_Absyn_PtrInfo _tmp368;void*_tmp369;struct Cyc_Absyn_Tqual
_tmp36A;struct Cyc_Absyn_PtrAtts _tmp36B;void*_tmp36C;union Cyc_Absyn_Constraint*
_tmp36D;union Cyc_Absyn_Constraint*_tmp36E;struct Cyc_List_List*_tmp370;_LL192: {
struct Cyc_Absyn_PointerType_struct*_tmp367=(struct Cyc_Absyn_PointerType_struct*)
_tmp366;if(_tmp367->tag != 5)goto _LL194;else{_tmp368=_tmp367->f1;_tmp369=_tmp368.elt_typ;
_tmp36A=_tmp368.elt_tq;_tmp36B=_tmp368.ptr_atts;_tmp36C=_tmp36B.rgn;_tmp36D=
_tmp36B.bounds;_tmp36E=_tmp36B.zero_term;}}_LL193: if(((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,_tmp36E)){int emit_warning=1;{void*_tmp371=((void*(*)(
void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,
_tmp36D);struct Cyc_Absyn_Exp*_tmp373;_LL199: {struct Cyc_Absyn_Upper_b_struct*
_tmp372=(struct Cyc_Absyn_Upper_b_struct*)_tmp371;if(_tmp372->tag != 1)goto _LL19B;
else{_tmp373=_tmp372->f1;}}_LL19A: if(Cyc_Tcutil_is_const_exp(_tmp362,e2)){
unsigned int _tmp376;int _tmp377;struct _tuple13 _tmp375=Cyc_Evexp_eval_const_uint_exp(
e2);_tmp376=_tmp375.f1;_tmp377=_tmp375.f2;if(_tmp377){unsigned int _tmp379;int
_tmp37A;struct _tuple13 _tmp378=Cyc_Evexp_eval_const_uint_exp(_tmp373);_tmp379=
_tmp378.f1;_tmp37A=_tmp378.f2;if(_tmp37A  && _tmp379 > _tmp376)emit_warning=0;}}
goto _LL198;_LL19B: {struct Cyc_Absyn_DynEither_b_struct*_tmp374=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp371;if(_tmp374->tag != 0)goto _LL198;}_LL19C: emit_warning=0;goto _LL198;_LL198:;}
if(emit_warning)({void*_tmp37B=0;Cyc_Tcutil_warn(e2->loc,({const char*_tmp37C="subscript on thin, zero-terminated pointer could be expensive.";
_tag_dyneither(_tmp37C,sizeof(char),63);}),_tag_dyneither(_tmp37B,sizeof(void*),
0));});}else{if(Cyc_Tcutil_is_const_exp(_tmp362,e2)){unsigned int _tmp37E;int
_tmp37F;struct _tuple13 _tmp37D=Cyc_Evexp_eval_const_uint_exp(e2);_tmp37E=_tmp37D.f1;
_tmp37F=_tmp37D.f2;if(_tmp37F)Cyc_Tcutil_check_bound(loc,_tmp37E,_tmp36D);}else{
if(Cyc_Tcutil_is_bound_one(_tmp36D) && !((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,_tmp36E))({void*_tmp380=0;Cyc_Tcutil_warn(e1->loc,({
const char*_tmp381="subscript applied to pointer to one object";_tag_dyneither(
_tmp381,sizeof(char),43);}),_tag_dyneither(_tmp380,sizeof(void*),0));});Cyc_Tcutil_check_nonzero_bound(
loc,_tmp36D);}}Cyc_Tcenv_check_rgn_accessible(_tmp362,loc,_tmp36C);if(!Cyc_Tcutil_kind_leq(
Cyc_Tcutil_typ_kind(_tmp369),Cyc_Absyn_MemKind))({void*_tmp382=0;Cyc_Tcutil_terr(
e1->loc,({const char*_tmp383="can't subscript an abstract pointer";_tag_dyneither(
_tmp383,sizeof(char),36);}),_tag_dyneither(_tmp382,sizeof(void*),0));});return
_tmp369;_LL194: {struct Cyc_Absyn_TupleType_struct*_tmp36F=(struct Cyc_Absyn_TupleType_struct*)
_tmp366;if(_tmp36F->tag != 11)goto _LL196;else{_tmp370=_tmp36F->f1;}}_LL195: return
Cyc_Tcexp_ithTupleType(_tmp362,loc,_tmp370,e2);_LL196:;_LL197: return({struct Cyc_String_pa_struct
_tmp386;_tmp386.tag=0;_tmp386.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t1));{void*_tmp384[1]={& _tmp386};Cyc_Tcexp_expr_err(
_tmp362,loc,topt,({const char*_tmp385="subscript applied to %s";_tag_dyneither(
_tmp385,sizeof(char),24);}),_tag_dyneither(_tmp384,sizeof(void*),1));}});_LL191:;}}}
static void*Cyc_Tcexp_tcTuple(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_List_List*es){int done=0;struct Cyc_List_List*fields=0;
if(topt != 0){void*_tmp387=Cyc_Tcutil_compress(*topt);struct Cyc_List_List*_tmp389;
_LL19E: {struct Cyc_Absyn_TupleType_struct*_tmp388=(struct Cyc_Absyn_TupleType_struct*)
_tmp387;if(_tmp388->tag != 11)goto _LL1A0;else{_tmp389=_tmp388->f1;}}_LL19F: if(((
int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp389)!= ((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(es))goto _LL19D;for(0;es != 0;(es=es->tl,_tmp389=_tmp389->tl)){
void*_tmp38A=(*((struct _tuple17*)((struct Cyc_List_List*)_check_null(_tmp389))->hd)).f2;
Cyc_Tcexp_tcExpInitializer(te,(void**)& _tmp38A,(struct Cyc_Absyn_Exp*)es->hd);Cyc_Tcutil_coerce_arg(
te,(struct Cyc_Absyn_Exp*)es->hd,(*((struct _tuple17*)_tmp389->hd)).f2);fields=({
struct Cyc_List_List*_tmp38B=_cycalloc(sizeof(*_tmp38B));_tmp38B->hd=({struct
_tuple17*_tmp38C=_cycalloc(sizeof(*_tmp38C));_tmp38C->f1=(*((struct _tuple17*)
_tmp389->hd)).f1;_tmp38C->f2=(void*)((struct Cyc_Core_Opt*)_check_null(((struct
Cyc_Absyn_Exp*)es->hd)->topt))->v;_tmp38C;});_tmp38B->tl=fields;_tmp38B;});}done=
1;goto _LL19D;_LL1A0:;_LL1A1: goto _LL19D;_LL19D:;}if(!done)for(0;es != 0;es=es->tl){
Cyc_Tcexp_tcExpInitializer(te,0,(struct Cyc_Absyn_Exp*)es->hd);fields=({struct Cyc_List_List*
_tmp38D=_cycalloc(sizeof(*_tmp38D));_tmp38D->hd=({struct _tuple17*_tmp38E=
_cycalloc(sizeof(*_tmp38E));_tmp38E->f1=Cyc_Absyn_empty_tqual(0);_tmp38E->f2=(
void*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Exp*)es->hd)->topt))->v;
_tmp38E;});_tmp38D->tl=fields;_tmp38D;});}return(void*)({struct Cyc_Absyn_TupleType_struct*
_tmp38F=_cycalloc(sizeof(*_tmp38F));_tmp38F[0]=({struct Cyc_Absyn_TupleType_struct
_tmp390;_tmp390.tag=11;_tmp390.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(fields);_tmp390;});_tmp38F;});}static void*Cyc_Tcexp_tcCompoundLit(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct _tuple8*
t,struct Cyc_List_List*des){return({void*_tmp391=0;Cyc_Tcexp_expr_err(te,loc,topt,({
const char*_tmp392="tcCompoundLit";_tag_dyneither(_tmp392,sizeof(char),14);}),
_tag_dyneither(_tmp391,sizeof(void*),0));});}static void*Cyc_Tcexp_tcArray(struct
Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**elt_topt,int zero_term,
struct Cyc_List_List*des){void*res_t2;struct _RegionHandle*_tmp393=Cyc_Tcenv_get_fnrgn(
te);{int _tmp394=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(des);struct Cyc_List_List*
es=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Exp*(*f)(
struct _tuple16*),struct Cyc_List_List*x))Cyc_List_rmap)(_tmp393,(struct Cyc_Absyn_Exp*(*)(
struct _tuple16*))Cyc_Core_snd,des);void*res=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_mk,Cyc_Tcenv_lookup_opt_type_vars(te));struct Cyc_Absyn_Const_e_struct*
_tmp395=({struct Cyc_Absyn_Const_e_struct*_tmp39D=_cycalloc(sizeof(*_tmp39D));
_tmp39D[0]=({struct Cyc_Absyn_Const_e_struct _tmp39E;_tmp39E.tag=0;_tmp39E.f1=Cyc_Absyn_Int_c(
Cyc_Absyn_Unsigned,_tmp394);_tmp39E;});_tmp39D;});struct Cyc_Absyn_Exp*sz_exp=Cyc_Absyn_new_exp((
void*)_tmp395,loc);if(zero_term){struct Cyc_Absyn_Exp*_tmp396=((struct Cyc_Absyn_Exp*(*)(
struct Cyc_List_List*x,int n))Cyc_List_nth)(es,_tmp394 - 1);if(!Cyc_Tcutil_is_zero(
_tmp396))({void*_tmp397=0;Cyc_Tcutil_terr(_tmp396->loc,({const char*_tmp398="zero-terminated array doesn't end with zero.";
_tag_dyneither(_tmp398,sizeof(char),45);}),_tag_dyneither(_tmp397,sizeof(void*),
0));});}sz_exp->topt=({struct Cyc_Core_Opt*_tmp399=_cycalloc(sizeof(*_tmp399));
_tmp399->v=Cyc_Absyn_uint_typ;_tmp399;});res_t2=Cyc_Absyn_array_typ(res,Cyc_Absyn_empty_tqual(
0),(struct Cyc_Absyn_Exp*)sz_exp,zero_term?Cyc_Absyn_true_conref: Cyc_Absyn_false_conref,
0);{struct Cyc_List_List*es2=es;for(0;es2 != 0;es2=es2->tl){Cyc_Tcexp_tcExpInitializer(
te,elt_topt,(struct Cyc_Absyn_Exp*)es2->hd);}}if(!Cyc_Tcutil_coerce_list(te,res,
es))({struct Cyc_String_pa_struct _tmp39C;_tmp39C.tag=0;_tmp39C.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(res));{void*_tmp39A[
1]={& _tmp39C};Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(es))->hd)->loc,({const char*_tmp39B="elements of array do not all have the same type (%s)";
_tag_dyneither(_tmp39B,sizeof(char),53);}),_tag_dyneither(_tmp39A,sizeof(void*),
1));}});}{int offset=0;for(0;des != 0;(offset ++,des=des->tl)){struct Cyc_List_List*
ds=(*((struct _tuple16*)des->hd)).f1;if(ds != 0){void*_tmp39F=(void*)ds->hd;struct
Cyc_Absyn_Exp*_tmp3A2;_LL1A3: {struct Cyc_Absyn_FieldName_struct*_tmp3A0=(struct
Cyc_Absyn_FieldName_struct*)_tmp39F;if(_tmp3A0->tag != 1)goto _LL1A5;}_LL1A4:({
void*_tmp3A3=0;Cyc_Tcutil_terr(loc,({const char*_tmp3A4="only array index designators are supported";
_tag_dyneither(_tmp3A4,sizeof(char),43);}),_tag_dyneither(_tmp3A3,sizeof(void*),
0));});goto _LL1A2;_LL1A5: {struct Cyc_Absyn_ArrayElement_struct*_tmp3A1=(struct
Cyc_Absyn_ArrayElement_struct*)_tmp39F;if(_tmp3A1->tag != 0)goto _LL1A2;else{
_tmp3A2=_tmp3A1->f1;}}_LL1A6: Cyc_Tcexp_tcExpInitializer(te,0,_tmp3A2);{
unsigned int _tmp3A6;int _tmp3A7;struct _tuple13 _tmp3A5=Cyc_Evexp_eval_const_uint_exp(
_tmp3A2);_tmp3A6=_tmp3A5.f1;_tmp3A7=_tmp3A5.f2;if(!_tmp3A7)({void*_tmp3A8=0;Cyc_Tcutil_terr(
_tmp3A2->loc,({const char*_tmp3A9="index designator cannot use sizeof or offsetof";
_tag_dyneither(_tmp3A9,sizeof(char),47);}),_tag_dyneither(_tmp3A8,sizeof(void*),
0));});else{if(_tmp3A6 != offset)({struct Cyc_Int_pa_struct _tmp3AD;_tmp3AD.tag=1;
_tmp3AD.f1=(unsigned long)((int)_tmp3A6);{struct Cyc_Int_pa_struct _tmp3AC;_tmp3AC.tag=
1;_tmp3AC.f1=(unsigned long)offset;{void*_tmp3AA[2]={& _tmp3AC,& _tmp3AD};Cyc_Tcutil_terr(
_tmp3A2->loc,({const char*_tmp3AB="expecting index designator %d but found %d";
_tag_dyneither(_tmp3AB,sizeof(char),43);}),_tag_dyneither(_tmp3AA,sizeof(void*),
2));}}});}goto _LL1A2;}_LL1A2:;}}}return res_t2;}static void*Cyc_Tcexp_tcComprehension(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Vardecl*
vd,struct Cyc_Absyn_Exp*bound,struct Cyc_Absyn_Exp*body,int*is_zero_term){Cyc_Tcexp_tcExp(
te,0,bound);{void*_tmp3AE=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(bound->topt))->v);_LL1A8: {struct Cyc_Absyn_TagType_struct*_tmp3AF=(
struct Cyc_Absyn_TagType_struct*)_tmp3AE;if(_tmp3AF->tag != 20)goto _LL1AA;}_LL1A9:
goto _LL1A7;_LL1AA:;_LL1AB: if(!Cyc_Tcutil_coerce_uint_typ(te,bound))({struct Cyc_String_pa_struct
_tmp3B2;_tmp3B2.tag=0;_tmp3B2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(bound->topt))->v));{
void*_tmp3B0[1]={& _tmp3B2};Cyc_Tcutil_terr(bound->loc,({const char*_tmp3B1="expecting unsigned int, found %s";
_tag_dyneither(_tmp3B1,sizeof(char),33);}),_tag_dyneither(_tmp3B0,sizeof(void*),
1));}});_LL1A7:;}if(!(vd->tq).real_const)({void*_tmp3B3=0;((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp3B4="comprehension index variable is not declared const!";
_tag_dyneither(_tmp3B4,sizeof(char),52);}),_tag_dyneither(_tmp3B3,sizeof(void*),
0));});{struct _RegionHandle*_tmp3B5=Cyc_Tcenv_get_fnrgn(te);struct Cyc_Tcenv_Tenv*
te2=Cyc_Tcenv_copy_tenv(_tmp3B5,te);if(te2->le != 0){te2=Cyc_Tcenv_new_block(
_tmp3B5,loc,te2);te2=Cyc_Tcenv_add_local_var(_tmp3B5,loc,te2,vd);}{struct Cyc_Tcenv_Tenv*
_tmp3B6=te2;struct Cyc_Absyn_PtrInfo pinfo;void**_tmp3B7=0;struct Cyc_Absyn_Tqual*
_tmp3B8=0;union Cyc_Absyn_Constraint**_tmp3B9=0;if(topt != 0){void*_tmp3BA=Cyc_Tcutil_compress(*
topt);struct Cyc_Absyn_PtrInfo _tmp3BC;struct Cyc_Absyn_ArrayInfo _tmp3BE;void*
_tmp3BF;void**_tmp3C0;struct Cyc_Absyn_Tqual _tmp3C1;struct Cyc_Absyn_Tqual*_tmp3C2;
struct Cyc_Absyn_Exp*_tmp3C3;union Cyc_Absyn_Constraint*_tmp3C4;union Cyc_Absyn_Constraint**
_tmp3C5;_LL1AD: {struct Cyc_Absyn_PointerType_struct*_tmp3BB=(struct Cyc_Absyn_PointerType_struct*)
_tmp3BA;if(_tmp3BB->tag != 5)goto _LL1AF;else{_tmp3BC=_tmp3BB->f1;}}_LL1AE: pinfo=
_tmp3BC;_tmp3B7=(void**)& pinfo.elt_typ;_tmp3B8=(struct Cyc_Absyn_Tqual*)& pinfo.elt_tq;
_tmp3B9=(union Cyc_Absyn_Constraint**)&(pinfo.ptr_atts).zero_term;goto _LL1AC;
_LL1AF: {struct Cyc_Absyn_ArrayType_struct*_tmp3BD=(struct Cyc_Absyn_ArrayType_struct*)
_tmp3BA;if(_tmp3BD->tag != 9)goto _LL1B1;else{_tmp3BE=_tmp3BD->f1;_tmp3BF=_tmp3BE.elt_type;
_tmp3C0=(void**)&(_tmp3BD->f1).elt_type;_tmp3C1=_tmp3BE.tq;_tmp3C2=(struct Cyc_Absyn_Tqual*)&(
_tmp3BD->f1).tq;_tmp3C3=_tmp3BE.num_elts;_tmp3C4=_tmp3BE.zero_term;_tmp3C5=(
union Cyc_Absyn_Constraint**)&(_tmp3BD->f1).zero_term;}}_LL1B0: _tmp3B7=(void**)
_tmp3C0;_tmp3B8=(struct Cyc_Absyn_Tqual*)_tmp3C2;_tmp3B9=(union Cyc_Absyn_Constraint**)
_tmp3C5;goto _LL1AC;_LL1B1:;_LL1B2: goto _LL1AC;_LL1AC:;}{void*t=Cyc_Tcexp_tcExp(
_tmp3B6,_tmp3B7,body);if(_tmp3B6->le == 0){if(!Cyc_Tcutil_is_const_exp(_tmp3B6,
bound))({void*_tmp3C6=0;Cyc_Tcutil_terr(bound->loc,({const char*_tmp3C7="bound is not constant";
_tag_dyneither(_tmp3C7,sizeof(char),22);}),_tag_dyneither(_tmp3C6,sizeof(void*),
0));});if(!Cyc_Tcutil_is_const_exp(_tmp3B6,body))({void*_tmp3C8=0;Cyc_Tcutil_terr(
bound->loc,({const char*_tmp3C9="body is not constant";_tag_dyneither(_tmp3C9,
sizeof(char),21);}),_tag_dyneither(_tmp3C8,sizeof(void*),0));});}if(_tmp3B9 != 0
 && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,*_tmp3B9)){
struct Cyc_Absyn_Exp*_tmp3CA=Cyc_Absyn_uint_exp(1,0);_tmp3CA->topt=({struct Cyc_Core_Opt*
_tmp3CB=_cycalloc(sizeof(*_tmp3CB));_tmp3CB->v=Cyc_Absyn_uint_typ;_tmp3CB;});
bound=Cyc_Absyn_add_exp(bound,_tmp3CA,0);bound->topt=({struct Cyc_Core_Opt*
_tmp3CC=_cycalloc(sizeof(*_tmp3CC));_tmp3CC->v=Cyc_Absyn_uint_typ;_tmp3CC;});*
is_zero_term=1;}{void*_tmp3CD=Cyc_Absyn_array_typ(t,_tmp3B8 == 0?Cyc_Absyn_empty_tqual(
0):*_tmp3B8,(struct Cyc_Absyn_Exp*)bound,_tmp3B9 == 0?Cyc_Absyn_false_conref:*
_tmp3B9,0);return _tmp3CD;}}}}}struct _tuple18{struct Cyc_Absyn_Aggrfield*f1;struct
Cyc_Absyn_Exp*f2;};static void*Cyc_Tcexp_tcAggregate(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,void**topt,struct _tuple1**tn,struct Cyc_List_List**
ts,struct Cyc_List_List*args,struct Cyc_Absyn_Aggrdecl**ad_opt){struct Cyc_Absyn_Aggrdecl*
ad;if(*ad_opt != 0)ad=(struct Cyc_Absyn_Aggrdecl*)_check_null(*ad_opt);else{{
struct _handler_cons _tmp3CE;_push_handler(& _tmp3CE);{int _tmp3D0=0;if(setjmp(
_tmp3CE.handler))_tmp3D0=1;if(!_tmp3D0){ad=*Cyc_Tcenv_lookup_aggrdecl(te,loc,*tn);;
_pop_handler();}else{void*_tmp3CF=(void*)_exn_thrown;void*_tmp3D2=_tmp3CF;_LL1B4: {
struct Cyc_Dict_Absent_struct*_tmp3D3=(struct Cyc_Dict_Absent_struct*)_tmp3D2;if(
_tmp3D3->tag != Cyc_Dict_Absent)goto _LL1B6;}_LL1B5:({struct Cyc_String_pa_struct
_tmp3D6;_tmp3D6.tag=0;_tmp3D6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(*tn));{void*_tmp3D4[1]={& _tmp3D6};Cyc_Tcutil_terr(loc,({
const char*_tmp3D5="unbound struct/union name %s";_tag_dyneither(_tmp3D5,sizeof(
char),29);}),_tag_dyneither(_tmp3D4,sizeof(void*),1));}});return topt != 0?*topt:(
void*)& Cyc_Absyn_VoidType_val;_LL1B6:;_LL1B7:(void)_throw(_tmp3D2);_LL1B3:;}}}*
ad_opt=(struct Cyc_Absyn_Aggrdecl*)ad;*tn=ad->name;}if(ad->impl == 0){({struct Cyc_String_pa_struct
_tmp3D9;_tmp3D9.tag=0;_tmp3D9.f1=(struct _dyneither_ptr)(ad->kind == Cyc_Absyn_StructA?({
const char*_tmp3DA="struct";_tag_dyneither(_tmp3DA,sizeof(char),7);}):({const char*
_tmp3DB="union";_tag_dyneither(_tmp3DB,sizeof(char),6);}));{void*_tmp3D7[1]={&
_tmp3D9};Cyc_Tcutil_terr(loc,({const char*_tmp3D8="can't construct abstract %s";
_tag_dyneither(_tmp3D8,sizeof(char),28);}),_tag_dyneither(_tmp3D7,sizeof(void*),
1));}});return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}{struct
_RegionHandle*_tmp3DC=Cyc_Tcenv_get_fnrgn(te);struct _tuple10 _tmp3DD=({struct
_tuple10 _tmp3FE;_tmp3FE.f1=Cyc_Tcenv_lookup_type_vars(te);_tmp3FE.f2=_tmp3DC;
_tmp3FE;});struct Cyc_List_List*_tmp3DE=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _tuple11*(*f)(struct _tuple10*,struct Cyc_Absyn_Tvar*),struct
_tuple10*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(_tmp3DC,Cyc_Tcutil_r_make_inst_var,&
_tmp3DD,ad->tvs);struct Cyc_List_List*_tmp3DF=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _tuple11*(*f)(struct _tuple10*,struct Cyc_Absyn_Tvar*),struct
_tuple10*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(_tmp3DC,Cyc_Tcutil_r_make_inst_var,&
_tmp3DD,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars);
struct Cyc_List_List*_tmp3E0=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple11*),
struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple11*))Cyc_Core_snd,
_tmp3DE);struct Cyc_List_List*_tmp3E1=((struct Cyc_List_List*(*)(void*(*f)(struct
_tuple11*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple11*))Cyc_Core_snd,
_tmp3DF);struct Cyc_List_List*_tmp3E2=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(
_tmp3DC,_tmp3DE,_tmp3DF);struct Cyc_Absyn_AggrType_struct*_tmp3E3=({struct Cyc_Absyn_AggrType_struct*
_tmp3FA=_cycalloc(sizeof(*_tmp3FA));_tmp3FA[0]=({struct Cyc_Absyn_AggrType_struct
_tmp3FB;_tmp3FB.tag=12;_tmp3FB.f1=({struct Cyc_Absyn_AggrInfo _tmp3FC;_tmp3FC.aggr_info=
Cyc_Absyn_KnownAggr(({struct Cyc_Absyn_Aggrdecl**_tmp3FD=_cycalloc(sizeof(*
_tmp3FD));_tmp3FD[0]=ad;_tmp3FD;}));_tmp3FC.targs=_tmp3E0;_tmp3FC;});_tmp3FB;});
_tmp3FA;});struct Cyc_List_List*_tmp3E4=*ts;struct Cyc_List_List*_tmp3E5=_tmp3E1;
while(_tmp3E4 != 0  && _tmp3E5 != 0){Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(
te),Cyc_Absyn_AnyKind,1,(void*)_tmp3E4->hd);Cyc_Tcutil_unify((void*)_tmp3E4->hd,(
void*)_tmp3E5->hd);_tmp3E4=_tmp3E4->tl;_tmp3E5=_tmp3E5->tl;}if(_tmp3E4 != 0)({
void*_tmp3E6=0;Cyc_Tcutil_terr(loc,({const char*_tmp3E7="too many explicit witness types";
_tag_dyneither(_tmp3E7,sizeof(char),32);}),_tag_dyneither(_tmp3E6,sizeof(void*),
0));});*ts=_tmp3E1;if(topt != 0)Cyc_Tcutil_unify((void*)_tmp3E3,*topt);{struct Cyc_List_List*
fields=((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,struct Cyc_Position_Segment*
loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))
Cyc_Tcutil_resolve_aggregate_designators)(_tmp3DC,loc,args,ad->kind,((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(ad->impl))->fields);for(0;fields != 0;fields=fields->tl){struct
_tuple18 _tmp3E9;struct Cyc_Absyn_Aggrfield*_tmp3EA;struct Cyc_Absyn_Exp*_tmp3EB;
struct _tuple18*_tmp3E8=(struct _tuple18*)fields->hd;_tmp3E9=*_tmp3E8;_tmp3EA=
_tmp3E9.f1;_tmp3EB=_tmp3E9.f2;{void*_tmp3EC=Cyc_Tcutil_rsubstitute(_tmp3DC,
_tmp3E2,_tmp3EA->type);Cyc_Tcexp_tcExpInitializer(te,(void**)& _tmp3EC,_tmp3EB);
if(!Cyc_Tcutil_coerce_arg(te,_tmp3EB,_tmp3EC)){({struct Cyc_String_pa_struct
_tmp3F3;_tmp3F3.tag=0;_tmp3F3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(_tmp3EB->topt))->v));{
struct Cyc_String_pa_struct _tmp3F2;_tmp3F2.tag=0;_tmp3F2.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp3EC));{struct Cyc_String_pa_struct
_tmp3F1;_tmp3F1.tag=0;_tmp3F1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(*tn));{struct Cyc_String_pa_struct _tmp3F0;_tmp3F0.tag=0;
_tmp3F0.f1=(struct _dyneither_ptr)(ad->kind == Cyc_Absyn_StructA?({const char*
_tmp3F4="struct";_tag_dyneither(_tmp3F4,sizeof(char),7);}):({const char*_tmp3F5="union";
_tag_dyneither(_tmp3F5,sizeof(char),6);}));{struct Cyc_String_pa_struct _tmp3EF;
_tmp3EF.tag=0;_tmp3EF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp3EA->name);{
void*_tmp3ED[5]={& _tmp3EF,& _tmp3F0,& _tmp3F1,& _tmp3F2,& _tmp3F3};Cyc_Tcutil_terr(
_tmp3EB->loc,({const char*_tmp3EE="field %s of %s %s expects type %s != %s";
_tag_dyneither(_tmp3EE,sizeof(char),40);}),_tag_dyneither(_tmp3ED,sizeof(void*),
5));}}}}}});Cyc_Tcutil_explain_failure();}}}{struct Cyc_List_List*_tmp3F6=0;{
struct Cyc_List_List*_tmp3F7=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po;
for(0;_tmp3F7 != 0;_tmp3F7=_tmp3F7->tl){_tmp3F6=({struct Cyc_List_List*_tmp3F8=
_cycalloc(sizeof(*_tmp3F8));_tmp3F8->hd=({struct _tuple0*_tmp3F9=_cycalloc(
sizeof(*_tmp3F9));_tmp3F9->f1=Cyc_Tcutil_rsubstitute(_tmp3DC,_tmp3E2,(*((struct
_tuple0*)_tmp3F7->hd)).f1);_tmp3F9->f2=Cyc_Tcutil_rsubstitute(_tmp3DC,_tmp3E2,(*((
struct _tuple0*)_tmp3F7->hd)).f2);_tmp3F9;});_tmp3F8->tl=_tmp3F6;_tmp3F8;});}}
_tmp3F6=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmp3F6);Cyc_Tcenv_check_rgn_partial_order(te,loc,_tmp3F6);return(void*)_tmp3E3;}}}}
static void*Cyc_Tcexp_tcAnonStruct(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void*ts,struct Cyc_List_List*args){struct _RegionHandle*_tmp3FF=Cyc_Tcenv_get_fnrgn(
te);{void*_tmp400=Cyc_Tcutil_compress(ts);enum Cyc_Absyn_AggrKind _tmp402;struct
Cyc_List_List*_tmp403;_LL1B9: {struct Cyc_Absyn_AnonAggrType_struct*_tmp401=(
struct Cyc_Absyn_AnonAggrType_struct*)_tmp400;if(_tmp401->tag != 13)goto _LL1BB;
else{_tmp402=_tmp401->f1;_tmp403=_tmp401->f2;}}_LL1BA: if(_tmp402 == Cyc_Absyn_UnionA)({
void*_tmp404=0;Cyc_Tcutil_terr(loc,({const char*_tmp405="expecting struct but found union";
_tag_dyneither(_tmp405,sizeof(char),33);}),_tag_dyneither(_tmp404,sizeof(void*),
0));});{struct Cyc_List_List*fields=((struct Cyc_List_List*(*)(struct _RegionHandle*
rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,
struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(_tmp3FF,
loc,args,Cyc_Absyn_StructA,_tmp403);for(0;fields != 0;fields=fields->tl){struct
_tuple18 _tmp407;struct Cyc_Absyn_Aggrfield*_tmp408;struct Cyc_Absyn_Exp*_tmp409;
struct _tuple18*_tmp406=(struct _tuple18*)fields->hd;_tmp407=*_tmp406;_tmp408=
_tmp407.f1;_tmp409=_tmp407.f2;Cyc_Tcexp_tcExpInitializer(te,(void**)& _tmp408->type,
_tmp409);if(!Cyc_Tcutil_coerce_arg(te,_tmp409,_tmp408->type)){({struct Cyc_String_pa_struct
_tmp40E;_tmp40E.tag=0;_tmp40E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(_tmp409->topt))->v));{
struct Cyc_String_pa_struct _tmp40D;_tmp40D.tag=0;_tmp40D.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp408->type));{struct Cyc_String_pa_struct
_tmp40C;_tmp40C.tag=0;_tmp40C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*
_tmp408->name);{void*_tmp40A[3]={& _tmp40C,& _tmp40D,& _tmp40E};Cyc_Tcutil_terr(
_tmp409->loc,({const char*_tmp40B="field %s of struct expects type %s != %s";
_tag_dyneither(_tmp40B,sizeof(char),41);}),_tag_dyneither(_tmp40A,sizeof(void*),
3));}}}});Cyc_Tcutil_explain_failure();}}goto _LL1B8;}_LL1BB:;_LL1BC:({void*
_tmp40F=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp410="tcAnonStruct: wrong type";_tag_dyneither(_tmp410,sizeof(char),
25);}),_tag_dyneither(_tmp40F,sizeof(void*),0));});_LL1B8:;}return ts;}static void*
Cyc_Tcexp_tcDatatype(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**
topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es,struct Cyc_Absyn_Datatypedecl*
tud,struct Cyc_Absyn_Datatypefield*tuf){struct _RegionHandle*_tmp411=Cyc_Tcenv_get_fnrgn(
te);struct _tuple10 _tmp412=({struct _tuple10 _tmp426;_tmp426.f1=Cyc_Tcenv_lookup_type_vars(
te);_tmp426.f2=_tmp411;_tmp426;});struct Cyc_List_List*_tmp413=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _tuple11*(*f)(struct _tuple10*,struct Cyc_Absyn_Tvar*),
struct _tuple10*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(_tmp411,Cyc_Tcutil_r_make_inst_var,&
_tmp412,tud->tvs);struct Cyc_List_List*_tmp414=((struct Cyc_List_List*(*)(void*(*f)(
struct _tuple11*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple11*))
Cyc_Core_snd,_tmp413);void*res=(void*)({struct Cyc_Absyn_DatatypeFieldType_struct*
_tmp423=_cycalloc(sizeof(*_tmp423));_tmp423[0]=({struct Cyc_Absyn_DatatypeFieldType_struct
_tmp424;_tmp424.tag=4;_tmp424.f1=({struct Cyc_Absyn_DatatypeFieldInfo _tmp425;
_tmp425.field_info=Cyc_Absyn_KnownDatatypefield(tud,tuf);_tmp425.targs=_tmp414;
_tmp425;});_tmp424;});_tmp423;});if(topt != 0){void*_tmp415=Cyc_Tcutil_compress(*
topt);_LL1BE: {struct Cyc_Absyn_DatatypeFieldType_struct*_tmp416=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp415;if(_tmp416->tag != 4)goto _LL1C0;}_LL1BF: Cyc_Tcutil_unify(*topt,res);goto
_LL1BD;_LL1C0:;_LL1C1: goto _LL1BD;_LL1BD:;}{struct Cyc_List_List*ts=tuf->typs;for(
0;es != 0  && ts != 0;(es=es->tl,ts=ts->tl)){struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)
es->hd;void*t=Cyc_Tcutil_rsubstitute(_tmp411,_tmp413,(*((struct _tuple17*)ts->hd)).f2);
Cyc_Tcexp_tcExpInitializer(te,(void**)& t,e);if(!Cyc_Tcutil_coerce_arg(te,e,t)){({
struct Cyc_String_pa_struct _tmp41B;_tmp41B.tag=0;_tmp41B.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)(e->topt == 0?(struct _dyneither_ptr)({const char*_tmp41C="?";
_tag_dyneither(_tmp41C,sizeof(char),2);}): Cyc_Absynpp_typ2string((void*)((struct
Cyc_Core_Opt*)_check_null(e->topt))->v)));{struct Cyc_String_pa_struct _tmp41A;
_tmp41A.tag=0;_tmp41A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t));{struct Cyc_String_pa_struct _tmp419;_tmp419.tag=0;_tmp419.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(tuf->name));{void*
_tmp417[3]={& _tmp419,& _tmp41A,& _tmp41B};Cyc_Tcutil_terr(e->loc,({const char*
_tmp418="datatype constructor %s expects argument of type %s but this argument has type %s";
_tag_dyneither(_tmp418,sizeof(char),82);}),_tag_dyneither(_tmp417,sizeof(void*),
3));}}}});Cyc_Tcutil_explain_failure();}}if(es != 0)return({struct Cyc_String_pa_struct
_tmp41F;_tmp41F.tag=0;_tmp41F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(tuf->name));{void*_tmp41D[1]={& _tmp41F};Cyc_Tcexp_expr_err(
te,((struct Cyc_Absyn_Exp*)es->hd)->loc,topt,({const char*_tmp41E="too many arguments for datatype constructor %s";
_tag_dyneither(_tmp41E,sizeof(char),47);}),_tag_dyneither(_tmp41D,sizeof(void*),
1));}});if(ts != 0)return({struct Cyc_String_pa_struct _tmp422;_tmp422.tag=0;
_tmp422.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
tuf->name));{void*_tmp420[1]={& _tmp422};Cyc_Tcexp_expr_err(te,loc,topt,({const
char*_tmp421="too few arguments for datatype constructor %s";_tag_dyneither(
_tmp421,sizeof(char),46);}),_tag_dyneither(_tmp420,sizeof(void*),1));}});return
res;}}static int Cyc_Tcexp_zeroable_type(void*t){void*_tmp427=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_PtrInfo _tmp42E;struct Cyc_Absyn_PtrAtts _tmp42F;union Cyc_Absyn_Constraint*
_tmp430;struct Cyc_Absyn_ArrayInfo _tmp435;void*_tmp436;struct Cyc_List_List*
_tmp439;struct Cyc_Absyn_AggrInfo _tmp43B;union Cyc_Absyn_AggrInfoU _tmp43C;struct
Cyc_List_List*_tmp43D;struct Cyc_List_List*_tmp441;_LL1C3: {struct Cyc_Absyn_VoidType_struct*
_tmp428=(struct Cyc_Absyn_VoidType_struct*)_tmp427;if(_tmp428->tag != 0)goto _LL1C5;}
_LL1C4: return 1;_LL1C5: {struct Cyc_Absyn_Evar_struct*_tmp429=(struct Cyc_Absyn_Evar_struct*)
_tmp427;if(_tmp429->tag != 1)goto _LL1C7;}_LL1C6: goto _LL1C8;_LL1C7: {struct Cyc_Absyn_VarType_struct*
_tmp42A=(struct Cyc_Absyn_VarType_struct*)_tmp427;if(_tmp42A->tag != 2)goto _LL1C9;}
_LL1C8: goto _LL1CA;_LL1C9: {struct Cyc_Absyn_DatatypeType_struct*_tmp42B=(struct
Cyc_Absyn_DatatypeType_struct*)_tmp427;if(_tmp42B->tag != 3)goto _LL1CB;}_LL1CA:
goto _LL1CC;_LL1CB: {struct Cyc_Absyn_DatatypeFieldType_struct*_tmp42C=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp427;if(_tmp42C->tag != 4)goto _LL1CD;}_LL1CC: return 0;_LL1CD: {struct Cyc_Absyn_PointerType_struct*
_tmp42D=(struct Cyc_Absyn_PointerType_struct*)_tmp427;if(_tmp42D->tag != 5)goto
_LL1CF;else{_tmp42E=_tmp42D->f1;_tmp42F=_tmp42E.ptr_atts;_tmp430=_tmp42F.nullable;}}
_LL1CE: return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(1,
_tmp430);_LL1CF: {struct Cyc_Absyn_IntType_struct*_tmp431=(struct Cyc_Absyn_IntType_struct*)
_tmp427;if(_tmp431->tag != 6)goto _LL1D1;}_LL1D0: goto _LL1D2;_LL1D1: {struct Cyc_Absyn_FloatType_struct*
_tmp432=(struct Cyc_Absyn_FloatType_struct*)_tmp427;if(_tmp432->tag != 7)goto
_LL1D3;}_LL1D2: goto _LL1D4;_LL1D3: {struct Cyc_Absyn_DoubleType_struct*_tmp433=(
struct Cyc_Absyn_DoubleType_struct*)_tmp427;if(_tmp433->tag != 8)goto _LL1D5;}
_LL1D4: return 1;_LL1D5: {struct Cyc_Absyn_ArrayType_struct*_tmp434=(struct Cyc_Absyn_ArrayType_struct*)
_tmp427;if(_tmp434->tag != 9)goto _LL1D7;else{_tmp435=_tmp434->f1;_tmp436=_tmp435.elt_type;}}
_LL1D6: return Cyc_Tcexp_zeroable_type(_tmp436);_LL1D7: {struct Cyc_Absyn_FnType_struct*
_tmp437=(struct Cyc_Absyn_FnType_struct*)_tmp427;if(_tmp437->tag != 10)goto _LL1D9;}
_LL1D8: return 0;_LL1D9: {struct Cyc_Absyn_TupleType_struct*_tmp438=(struct Cyc_Absyn_TupleType_struct*)
_tmp427;if(_tmp438->tag != 11)goto _LL1DB;else{_tmp439=_tmp438->f1;}}_LL1DA: for(0;(
unsigned int)_tmp439;_tmp439=_tmp439->tl){if(!Cyc_Tcexp_zeroable_type((*((struct
_tuple17*)_tmp439->hd)).f2))return 0;}return 1;_LL1DB: {struct Cyc_Absyn_AggrType_struct*
_tmp43A=(struct Cyc_Absyn_AggrType_struct*)_tmp427;if(_tmp43A->tag != 12)goto
_LL1DD;else{_tmp43B=_tmp43A->f1;_tmp43C=_tmp43B.aggr_info;_tmp43D=_tmp43B.targs;}}
_LL1DC: {struct Cyc_Absyn_Aggrdecl*_tmp44C=Cyc_Absyn_get_known_aggrdecl(_tmp43C);
if(_tmp44C->impl == 0  || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp44C->impl))->exist_vars
!= 0)return 0;{struct _RegionHandle _tmp44D=_new_region("r");struct _RegionHandle*r=&
_tmp44D;_push_region(r);{struct Cyc_List_List*_tmp44E=((struct Cyc_List_List*(*)(
struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(r,r,_tmp44C->tvs,_tmp43D);{struct Cyc_List_List*fs=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp44C->impl))->fields;for(0;fs != 0;fs=fs->tl){if(!Cyc_Tcexp_zeroable_type(
Cyc_Tcutil_rsubstitute(r,_tmp44E,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))){
int _tmp44F=0;_npop_handler(0);return _tmp44F;}}}{int _tmp450=1;_npop_handler(0);
return _tmp450;}};_pop_region(r);}}_LL1DD: {struct Cyc_Absyn_EnumType_struct*
_tmp43E=(struct Cyc_Absyn_EnumType_struct*)_tmp427;if(_tmp43E->tag != 14)goto
_LL1DF;}_LL1DE: return 1;_LL1DF: {struct Cyc_Absyn_TagType_struct*_tmp43F=(struct
Cyc_Absyn_TagType_struct*)_tmp427;if(_tmp43F->tag != 20)goto _LL1E1;}_LL1E0: return
1;_LL1E1: {struct Cyc_Absyn_AnonAggrType_struct*_tmp440=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp427;if(_tmp440->tag != 13)goto _LL1E3;else{_tmp441=_tmp440->f2;}}_LL1E2: for(0;
_tmp441 != 0;_tmp441=_tmp441->tl){if(!Cyc_Tcexp_zeroable_type(((struct Cyc_Absyn_Aggrfield*)
_tmp441->hd)->type))return 0;}return 1;_LL1E3: {struct Cyc_Absyn_AnonEnumType_struct*
_tmp442=(struct Cyc_Absyn_AnonEnumType_struct*)_tmp427;if(_tmp442->tag != 15)goto
_LL1E5;}_LL1E4: return 1;_LL1E5: {struct Cyc_Absyn_TypedefType_struct*_tmp443=(
struct Cyc_Absyn_TypedefType_struct*)_tmp427;if(_tmp443->tag != 18)goto _LL1E7;}
_LL1E6: return 0;_LL1E7: {struct Cyc_Absyn_DynRgnType_struct*_tmp444=(struct Cyc_Absyn_DynRgnType_struct*)
_tmp427;if(_tmp444->tag != 17)goto _LL1E9;}_LL1E8: return 0;_LL1E9: {struct Cyc_Absyn_RgnHandleType_struct*
_tmp445=(struct Cyc_Absyn_RgnHandleType_struct*)_tmp427;if(_tmp445->tag != 16)goto
_LL1EB;}_LL1EA: return 0;_LL1EB: {struct Cyc_Absyn_ValueofType_struct*_tmp446=(
struct Cyc_Absyn_ValueofType_struct*)_tmp427;if(_tmp446->tag != 19)goto _LL1ED;}
_LL1EC: goto _LL1EE;_LL1ED: {struct Cyc_Absyn_HeapRgn_struct*_tmp447=(struct Cyc_Absyn_HeapRgn_struct*)
_tmp427;if(_tmp447->tag != 21)goto _LL1EF;}_LL1EE: goto _LL1F0;_LL1EF: {struct Cyc_Absyn_UniqueRgn_struct*
_tmp448=(struct Cyc_Absyn_UniqueRgn_struct*)_tmp427;if(_tmp448->tag != 22)goto
_LL1F1;}_LL1F0: goto _LL1F2;_LL1F1: {struct Cyc_Absyn_AccessEff_struct*_tmp449=(
struct Cyc_Absyn_AccessEff_struct*)_tmp427;if(_tmp449->tag != 23)goto _LL1F3;}
_LL1F2: goto _LL1F4;_LL1F3: {struct Cyc_Absyn_JoinEff_struct*_tmp44A=(struct Cyc_Absyn_JoinEff_struct*)
_tmp427;if(_tmp44A->tag != 24)goto _LL1F5;}_LL1F4: goto _LL1F6;_LL1F5: {struct Cyc_Absyn_RgnsEff_struct*
_tmp44B=(struct Cyc_Absyn_RgnsEff_struct*)_tmp427;if(_tmp44B->tag != 25)goto _LL1C2;}
_LL1F6:({struct Cyc_String_pa_struct _tmp453;_tmp453.tag=0;_tmp453.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));{void*_tmp451[1]={&
_tmp453};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp452="bad type `%s' in zeroable type";_tag_dyneither(_tmp452,
sizeof(char),31);}),_tag_dyneither(_tmp451,sizeof(void*),1));}});_LL1C2:;}static
void Cyc_Tcexp_check_malloc_type(int allow_zero,struct Cyc_Position_Segment*loc,
void**topt,void*t){if(Cyc_Tcutil_bits_only(t) || allow_zero  && Cyc_Tcexp_zeroable_type(
t))return;if(topt != 0){void*_tmp454=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo
_tmp456;void*_tmp457;_LL1F8: {struct Cyc_Absyn_PointerType_struct*_tmp455=(struct
Cyc_Absyn_PointerType_struct*)_tmp454;if(_tmp455->tag != 5)goto _LL1FA;else{
_tmp456=_tmp455->f1;_tmp457=_tmp456.elt_typ;}}_LL1F9: Cyc_Tcutil_unify(_tmp457,t);
if(Cyc_Tcutil_bits_only(t) || allow_zero  && Cyc_Tcexp_zeroable_type(t))return;
goto _LL1F7;_LL1FA:;_LL1FB: goto _LL1F7;_LL1F7:;}({struct Cyc_String_pa_struct
_tmp45B;_tmp45B.tag=0;_tmp45B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t));{struct Cyc_String_pa_struct _tmp45A;_tmp45A.tag=0;
_tmp45A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)(allow_zero?(struct
_dyneither_ptr)({const char*_tmp45C="calloc";_tag_dyneither(_tmp45C,sizeof(char),
7);}):(struct _dyneither_ptr)({const char*_tmp45D="malloc";_tag_dyneither(_tmp45D,
sizeof(char),7);})));{void*_tmp458[2]={& _tmp45A,& _tmp45B};Cyc_Tcutil_terr(loc,({
const char*_tmp459="%s cannot be used with type %s\n\t(type needs initialization)";
_tag_dyneither(_tmp459,sizeof(char),60);}),_tag_dyneither(_tmp458,sizeof(void*),
2));}}});}static void*Cyc_Tcexp_mallocRgn(void*rgn){switch(Cyc_Tcutil_typ_kind(
Cyc_Tcutil_compress(rgn))){case Cyc_Absyn_UniqueRgnKind: _LL1FC: return(void*)& Cyc_Absyn_UniqueRgn_val;
default: _LL1FD: return(void*)& Cyc_Absyn_HeapRgn_val;}}static void*Cyc_Tcexp_tcMalloc(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
ropt,void***t,struct Cyc_Absyn_Exp**e,int*is_calloc,int*is_fat){void*rgn=(void*)&
Cyc_Absyn_HeapRgn_val;if(ropt != 0){void*expected_type=(void*)({struct Cyc_Absyn_RgnHandleType_struct*
_tmp464=_cycalloc(sizeof(*_tmp464));_tmp464[0]=({struct Cyc_Absyn_RgnHandleType_struct
_tmp465;_tmp465.tag=16;_tmp465.f1=(void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_trk,Cyc_Tcenv_lookup_opt_type_vars(te));_tmp465;});_tmp464;});void*
handle_type=Cyc_Tcexp_tcExp(te,(void**)& expected_type,(struct Cyc_Absyn_Exp*)ropt);
void*_tmp45E=Cyc_Tcutil_compress(handle_type);void*_tmp460;_LL200: {struct Cyc_Absyn_RgnHandleType_struct*
_tmp45F=(struct Cyc_Absyn_RgnHandleType_struct*)_tmp45E;if(_tmp45F->tag != 16)goto
_LL202;else{_tmp460=(void*)_tmp45F->f1;}}_LL201: rgn=_tmp460;Cyc_Tcenv_check_rgn_accessible(
te,loc,rgn);goto _LL1FF;_LL202:;_LL203:({struct Cyc_String_pa_struct _tmp463;
_tmp463.tag=0;_tmp463.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
handle_type));{void*_tmp461[1]={& _tmp463};Cyc_Tcutil_terr(ropt->loc,({const char*
_tmp462="expecting region_t type but found %s";_tag_dyneither(_tmp462,sizeof(
char),37);}),_tag_dyneither(_tmp461,sizeof(void*),1));}});goto _LL1FF;_LL1FF:;}
else{if(topt != 0){void*optrgn=(void*)& Cyc_Absyn_VoidType_val;if(Cyc_Tcutil_rgn_of_pointer(*
topt,& optrgn))rgn=Cyc_Tcexp_mallocRgn(optrgn);}}Cyc_Tcexp_tcExp(te,(void**)& Cyc_Absyn_uint_typ,*
e);{void*elt_type;struct Cyc_Absyn_Exp*num_elts;int one_elt;if(*is_calloc){if(*t == 
0)({void*_tmp466=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp467="calloc with empty type";_tag_dyneither(_tmp467,sizeof(char),
23);}),_tag_dyneither(_tmp466,sizeof(void*),0));});elt_type=*((void**)
_check_null(*t));Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(te),Cyc_Absyn_MemKind,
1,elt_type);Cyc_Tcexp_check_malloc_type(1,loc,topt,elt_type);num_elts=*e;one_elt=
0;}else{void*_tmp468=(*e)->r;void*_tmp46A;enum Cyc_Absyn_Primop _tmp46C;struct Cyc_List_List*
_tmp46D;struct Cyc_List_List _tmp46E;struct Cyc_Absyn_Exp*_tmp46F;struct Cyc_List_List*
_tmp470;struct Cyc_List_List _tmp471;struct Cyc_Absyn_Exp*_tmp472;struct Cyc_List_List*
_tmp473;_LL205: {struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp469=(struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp468;if(_tmp469->tag != 18)goto _LL207;else{_tmp46A=(void*)_tmp469->f1;}}_LL206:
elt_type=_tmp46A;*t=(void**)({void**_tmp474=_cycalloc(sizeof(*_tmp474));_tmp474[
0]=elt_type;_tmp474;});num_elts=Cyc_Absyn_uint_exp(1,0);Cyc_Tcexp_tcExp(te,(void**)&
Cyc_Absyn_uint_typ,num_elts);one_elt=1;goto _LL204;_LL207: {struct Cyc_Absyn_Primop_e_struct*
_tmp46B=(struct Cyc_Absyn_Primop_e_struct*)_tmp468;if(_tmp46B->tag != 3)goto _LL209;
else{_tmp46C=_tmp46B->f1;if(_tmp46C != Cyc_Absyn_Times)goto _LL209;_tmp46D=_tmp46B->f2;
if(_tmp46D == 0)goto _LL209;_tmp46E=*_tmp46D;_tmp46F=(struct Cyc_Absyn_Exp*)_tmp46E.hd;
_tmp470=_tmp46E.tl;if(_tmp470 == 0)goto _LL209;_tmp471=*_tmp470;_tmp472=(struct Cyc_Absyn_Exp*)
_tmp471.hd;_tmp473=_tmp471.tl;if(_tmp473 != 0)goto _LL209;}}_LL208:{struct _tuple0
_tmp476=({struct _tuple0 _tmp475;_tmp475.f1=_tmp46F->r;_tmp475.f2=_tmp472->r;
_tmp475;});void*_tmp477;void*_tmp479;void*_tmp47A;void*_tmp47C;_LL20C: _tmp477=
_tmp476.f1;{struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp478=(struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp477;if(_tmp478->tag != 18)goto _LL20E;else{_tmp479=(void*)_tmp478->f1;}}_LL20D:
Cyc_Tcexp_check_malloc_type(0,loc,topt,_tmp479);elt_type=_tmp479;*t=(void**)({
void**_tmp47D=_cycalloc(sizeof(*_tmp47D));_tmp47D[0]=elt_type;_tmp47D;});
num_elts=_tmp472;one_elt=0;goto _LL20B;_LL20E: _tmp47A=_tmp476.f2;{struct Cyc_Absyn_Sizeoftyp_e_struct*
_tmp47B=(struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp47A;if(_tmp47B->tag != 18)goto
_LL210;else{_tmp47C=(void*)_tmp47B->f1;}}_LL20F: Cyc_Tcexp_check_malloc_type(0,
loc,topt,_tmp47C);elt_type=_tmp47C;*t=(void**)({void**_tmp47E=_cycalloc(sizeof(*
_tmp47E));_tmp47E[0]=elt_type;_tmp47E;});num_elts=_tmp46F;one_elt=0;goto _LL20B;
_LL210:;_LL211: goto No_sizeof;_LL20B:;}goto _LL204;_LL209:;_LL20A: No_sizeof:
elt_type=Cyc_Absyn_char_typ;*t=(void**)({void**_tmp47F=_cycalloc(sizeof(*_tmp47F));
_tmp47F[0]=elt_type;_tmp47F;});num_elts=*e;one_elt=0;goto _LL204;_LL204:;}*e=
num_elts;*is_fat=!one_elt;{void*_tmp480=elt_type;struct Cyc_Absyn_AggrInfo _tmp482;
union Cyc_Absyn_AggrInfoU _tmp483;struct Cyc_Absyn_Aggrdecl**_tmp484;struct Cyc_Absyn_Aggrdecl*
_tmp485;_LL213: {struct Cyc_Absyn_AggrType_struct*_tmp481=(struct Cyc_Absyn_AggrType_struct*)
_tmp480;if(_tmp481->tag != 12)goto _LL215;else{_tmp482=_tmp481->f1;_tmp483=_tmp482.aggr_info;
if((_tmp483.KnownAggr).tag != 2)goto _LL215;_tmp484=(struct Cyc_Absyn_Aggrdecl**)(
_tmp483.KnownAggr).val;_tmp485=*_tmp484;}}_LL214: if(_tmp485->impl != 0  && ((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp485->impl))->exist_vars != 0)({void*
_tmp486=0;Cyc_Tcutil_terr(loc,({const char*_tmp487="malloc with existential types not yet implemented";
_tag_dyneither(_tmp487,sizeof(char),50);}),_tag_dyneither(_tmp486,sizeof(void*),
0));});goto _LL212;_LL215:;_LL216: goto _LL212;_LL212:;}{void*(*_tmp488)(void*t,
void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term)=Cyc_Absyn_at_typ;
union Cyc_Absyn_Constraint*_tmp489=Cyc_Absyn_false_conref;if(topt != 0){void*
_tmp48A=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo _tmp48C;struct Cyc_Absyn_PtrAtts
_tmp48D;union Cyc_Absyn_Constraint*_tmp48E;union Cyc_Absyn_Constraint*_tmp48F;
union Cyc_Absyn_Constraint*_tmp490;_LL218: {struct Cyc_Absyn_PointerType_struct*
_tmp48B=(struct Cyc_Absyn_PointerType_struct*)_tmp48A;if(_tmp48B->tag != 5)goto
_LL21A;else{_tmp48C=_tmp48B->f1;_tmp48D=_tmp48C.ptr_atts;_tmp48E=_tmp48D.nullable;
_tmp48F=_tmp48D.bounds;_tmp490=_tmp48D.zero_term;}}_LL219: _tmp489=_tmp490;if(((
int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp48E))
_tmp488=Cyc_Absyn_star_typ;if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
0,_tmp490) && !(*is_calloc)){({void*_tmp491=0;Cyc_Tcutil_warn(loc,({const char*
_tmp492="converting malloc to calloc to ensure zero-termination";_tag_dyneither(
_tmp492,sizeof(char),55);}),_tag_dyneither(_tmp491,sizeof(void*),0));});*
is_calloc=1;}{void*_tmp493=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,_tmp48F);struct Cyc_Absyn_Exp*_tmp496;_LL21D: {struct Cyc_Absyn_DynEither_b_struct*
_tmp494=(struct Cyc_Absyn_DynEither_b_struct*)_tmp493;if(_tmp494->tag != 0)goto
_LL21F;}_LL21E: goto _LL21C;_LL21F:{struct Cyc_Absyn_Upper_b_struct*_tmp495=(struct
Cyc_Absyn_Upper_b_struct*)_tmp493;if(_tmp495->tag != 1)goto _LL221;else{_tmp496=
_tmp495->f1;}}if(!(!one_elt))goto _LL221;_LL220: {int _tmp497=Cyc_Evexp_c_can_eval(
num_elts);if(_tmp497  && Cyc_Evexp_same_const_exp(_tmp496,num_elts)){*is_fat=0;
return Cyc_Absyn_atb_typ(elt_type,rgn,Cyc_Absyn_empty_tqual(0),((void*(*)(union
Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp48F),_tmp489);}{void*_tmp498=
Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(num_elts->topt))->v);
void*_tmp49A;_LL224: {struct Cyc_Absyn_TagType_struct*_tmp499=(struct Cyc_Absyn_TagType_struct*)
_tmp498;if(_tmp499->tag != 20)goto _LL226;else{_tmp49A=(void*)_tmp499->f1;}}_LL225: {
struct Cyc_Absyn_Exp*_tmp49B=Cyc_Absyn_cast_exp(Cyc_Absyn_uint_typ,Cyc_Absyn_valueof_exp(
_tmp49A,0),0,Cyc_Absyn_No_coercion,0);if(Cyc_Evexp_same_const_exp(_tmp49B,
_tmp496)){*is_fat=0;return Cyc_Absyn_atb_typ(elt_type,rgn,Cyc_Absyn_empty_tqual(0),((
void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp48F),_tmp489);}
goto _LL223;}_LL226:;_LL227: goto _LL223;_LL223:;}goto _LL21C;}_LL221:;_LL222: goto
_LL21C;_LL21C:;}goto _LL217;_LL21A:;_LL21B: goto _LL217;_LL217:;}if(!one_elt)
_tmp488=Cyc_Absyn_dyneither_typ;return _tmp488(elt_type,rgn,Cyc_Absyn_empty_tqual(
0),_tmp489);}}}static void*Cyc_Tcexp_tcSwap(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){Cyc_Tcexp_tcExp(te,
0,e1);Cyc_Tcexp_tcExp(te,(void**)((void**)((void*)&((struct Cyc_Core_Opt*)
_check_null(e1->topt))->v)),e2);{void*t1=(void*)((struct Cyc_Core_Opt*)
_check_null(e1->topt))->v;void*t2=(void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v;{
void*_tmp49C=Cyc_Tcutil_compress(t1);_LL229: {struct Cyc_Absyn_ArrayType_struct*
_tmp49D=(struct Cyc_Absyn_ArrayType_struct*)_tmp49C;if(_tmp49D->tag != 9)goto
_LL22B;}_LL22A:({void*_tmp49E=0;Cyc_Tcutil_terr(loc,({const char*_tmp49F="cannot assign to an array";
_tag_dyneither(_tmp49F,sizeof(char),26);}),_tag_dyneither(_tmp49E,sizeof(void*),
0));});goto _LL228;_LL22B:;_LL22C: goto _LL228;_LL228:;}{int ign_1=0;if(!Cyc_Tcutil_is_pointer_or_boxed(
t1,& ign_1))({void*_tmp4A0=0;Cyc_Tcutil_terr(loc,({const char*_tmp4A1="Swap not allowed for non-pointer or non-word-sized types.";
_tag_dyneither(_tmp4A1,sizeof(char),58);}),_tag_dyneither(_tmp4A0,sizeof(void*),
0));});if(!Cyc_Absyn_is_lvalue(e1))return({void*_tmp4A2=0;Cyc_Tcexp_expr_err(te,
e1->loc,topt,({const char*_tmp4A3="swap non-lvalue";_tag_dyneither(_tmp4A3,
sizeof(char),16);}),_tag_dyneither(_tmp4A2,sizeof(void*),0));});if(!Cyc_Absyn_is_lvalue(
e2))return({void*_tmp4A4=0;Cyc_Tcexp_expr_err(te,e2->loc,topt,({const char*
_tmp4A5="swap non-lvalue";_tag_dyneither(_tmp4A5,sizeof(char),16);}),
_tag_dyneither(_tmp4A4,sizeof(void*),0));});{void*t_ign1=(void*)& Cyc_Absyn_VoidType_val;
void*t_ign2=(void*)& Cyc_Absyn_VoidType_val;int b_ign1=0;if(Cyc_Tcutil_is_zero_ptr_deref(
e1,& t_ign1,& b_ign1,& t_ign2))return({void*_tmp4A6=0;Cyc_Tcexp_expr_err(te,e1->loc,
topt,({const char*_tmp4A7="swap value in zeroterm array";_tag_dyneither(_tmp4A7,
sizeof(char),29);}),_tag_dyneither(_tmp4A6,sizeof(void*),0));});if(Cyc_Tcutil_is_zero_ptr_deref(
e2,& t_ign1,& b_ign1,& t_ign2))return({void*_tmp4A8=0;Cyc_Tcexp_expr_err(te,e2->loc,
topt,({const char*_tmp4A9="swap value in zeroterm array";_tag_dyneither(_tmp4A9,
sizeof(char),29);}),_tag_dyneither(_tmp4A8,sizeof(void*),0));});Cyc_Tcexp_check_writable(
te,e1);Cyc_Tcexp_check_writable(te,e2);if(!Cyc_Tcutil_unify(t1,t2)){void*_tmp4AA=({
struct Cyc_String_pa_struct _tmp4AE;_tmp4AE.tag=0;_tmp4AE.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2));{struct Cyc_String_pa_struct
_tmp4AD;_tmp4AD.tag=0;_tmp4AD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t1));{void*_tmp4AB[2]={& _tmp4AD,& _tmp4AE};Cyc_Tcexp_expr_err(
te,loc,topt,({const char*_tmp4AC="type mismatch: %s != %s";_tag_dyneither(_tmp4AC,
sizeof(char),24);}),_tag_dyneither(_tmp4AB,sizeof(void*),2));}}});return _tmp4AA;}
return(void*)& Cyc_Absyn_VoidType_val;}}}}int Cyc_Tcexp_in_stmt_exp=0;static void*
Cyc_Tcexp_tcStmtExp(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**
topt,struct Cyc_Absyn_Stmt*s){struct _RegionHandle*_tmp4AF=Cyc_Tcenv_get_fnrgn(te);{
int old_stmt_exp_state=Cyc_Tcexp_in_stmt_exp;Cyc_Tcstmt_tcStmt(Cyc_Tcenv_set_encloser(
_tmp4AF,te,s),s,1);Cyc_Tcexp_in_stmt_exp=old_stmt_exp_state;}Cyc_NewControlFlow_set_encloser(
s,Cyc_Tcenv_get_encloser(te));while(1){void*_tmp4B0=s->r;struct Cyc_Absyn_Exp*
_tmp4B2;struct Cyc_Absyn_Stmt*_tmp4B4;struct Cyc_Absyn_Stmt*_tmp4B5;struct Cyc_Absyn_Decl*
_tmp4B7;struct Cyc_Absyn_Stmt*_tmp4B8;_LL22E: {struct Cyc_Absyn_Exp_s_struct*
_tmp4B1=(struct Cyc_Absyn_Exp_s_struct*)_tmp4B0;if(_tmp4B1->tag != 1)goto _LL230;
else{_tmp4B2=_tmp4B1->f1;}}_LL22F: return(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp4B2->topt))->v;_LL230: {struct Cyc_Absyn_Seq_s_struct*_tmp4B3=(struct Cyc_Absyn_Seq_s_struct*)
_tmp4B0;if(_tmp4B3->tag != 2)goto _LL232;else{_tmp4B4=_tmp4B3->f1;_tmp4B5=_tmp4B3->f2;}}
_LL231: s=_tmp4B5;continue;_LL232: {struct Cyc_Absyn_Decl_s_struct*_tmp4B6=(struct
Cyc_Absyn_Decl_s_struct*)_tmp4B0;if(_tmp4B6->tag != 12)goto _LL234;else{_tmp4B7=
_tmp4B6->f1;_tmp4B8=_tmp4B6->f2;}}_LL233: s=_tmp4B8;continue;_LL234:;_LL235:
return({void*_tmp4B9=0;Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp4BA="statement expression must end with expression";
_tag_dyneither(_tmp4BA,sizeof(char),46);}),_tag_dyneither(_tmp4B9,sizeof(void*),
0));});_LL22D:;}}static void*Cyc_Tcexp_tcTagcheck(struct Cyc_Tcenv_Tenv*te,struct
Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*f){
void*t=Cyc_Tcutil_compress(Cyc_Tcexp_tcExp(te,0,e));{void*_tmp4BB=t;struct Cyc_Absyn_AggrInfo
_tmp4BD;union Cyc_Absyn_AggrInfoU _tmp4BE;struct Cyc_Absyn_Aggrdecl**_tmp4BF;struct
Cyc_Absyn_Aggrdecl*_tmp4C0;_LL237: {struct Cyc_Absyn_AggrType_struct*_tmp4BC=(
struct Cyc_Absyn_AggrType_struct*)_tmp4BB;if(_tmp4BC->tag != 12)goto _LL239;else{
_tmp4BD=_tmp4BC->f1;_tmp4BE=_tmp4BD.aggr_info;if((_tmp4BE.KnownAggr).tag != 2)
goto _LL239;_tmp4BF=(struct Cyc_Absyn_Aggrdecl**)(_tmp4BE.KnownAggr).val;_tmp4C0=*
_tmp4BF;}}_LL238: if((_tmp4C0->kind == Cyc_Absyn_UnionA  && _tmp4C0->impl != 0) && ((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp4C0->impl))->tagged)goto _LL236;
goto _LL23A;_LL239:;_LL23A:({struct Cyc_String_pa_struct _tmp4C3;_tmp4C3.tag=0;
_tmp4C3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));{
void*_tmp4C1[1]={& _tmp4C3};Cyc_Tcutil_terr(loc,({const char*_tmp4C2="expecting @tagged union but found %s";
_tag_dyneither(_tmp4C2,sizeof(char),37);}),_tag_dyneither(_tmp4C1,sizeof(void*),
1));}});goto _LL236;_LL236:;}return Cyc_Absyn_uint_typ;}static void*Cyc_Tcexp_tcNew(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
rgn_handle,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1){void*rgn=(void*)& Cyc_Absyn_HeapRgn_val;
if(rgn_handle != 0){void*expected_type=(void*)({struct Cyc_Absyn_RgnHandleType_struct*
_tmp4CA=_cycalloc(sizeof(*_tmp4CA));_tmp4CA[0]=({struct Cyc_Absyn_RgnHandleType_struct
_tmp4CB;_tmp4CB.tag=16;_tmp4CB.f1=(void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_trk,Cyc_Tcenv_lookup_opt_type_vars(te));_tmp4CB;});_tmp4CA;});void*
handle_type=Cyc_Tcexp_tcExp(te,(void**)& expected_type,(struct Cyc_Absyn_Exp*)
rgn_handle);void*_tmp4C4=Cyc_Tcutil_compress(handle_type);void*_tmp4C6;_LL23C: {
struct Cyc_Absyn_RgnHandleType_struct*_tmp4C5=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp4C4;if(_tmp4C5->tag != 16)goto _LL23E;else{_tmp4C6=(void*)_tmp4C5->f1;}}_LL23D:
rgn=_tmp4C6;Cyc_Tcenv_check_rgn_accessible(te,loc,rgn);goto _LL23B;_LL23E:;_LL23F:({
struct Cyc_String_pa_struct _tmp4C9;_tmp4C9.tag=0;_tmp4C9.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string(handle_type));{void*_tmp4C7[1]={&
_tmp4C9};Cyc_Tcutil_terr(rgn_handle->loc,({const char*_tmp4C8="expecting region_t type but found %s";
_tag_dyneither(_tmp4C8,sizeof(char),37);}),_tag_dyneither(_tmp4C7,sizeof(void*),
1));}});goto _LL23B;_LL23B:;}else{if(topt != 0){void*optrgn=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_rgn_of_pointer(*topt,& optrgn))rgn=Cyc_Tcexp_mallocRgn(optrgn);}}{
void*_tmp4CC=e1->r;struct Cyc_Core_Opt*_tmp4CF;struct Cyc_List_List*_tmp4D0;struct
Cyc_List_List*_tmp4D2;union Cyc_Absyn_Cnst _tmp4D4;struct _dyneither_ptr _tmp4D5;
_LL241: {struct Cyc_Absyn_Comprehension_e_struct*_tmp4CD=(struct Cyc_Absyn_Comprehension_e_struct*)
_tmp4CC;if(_tmp4CD->tag != 29)goto _LL243;}_LL242: {void*_tmp4D6=Cyc_Tcexp_tcExpNoPromote(
te,topt,e1);void*_tmp4D7=Cyc_Tcutil_compress(_tmp4D6);struct Cyc_Absyn_ArrayInfo
_tmp4D9;void*_tmp4DA;struct Cyc_Absyn_Tqual _tmp4DB;struct Cyc_Absyn_Exp*_tmp4DC;
union Cyc_Absyn_Constraint*_tmp4DD;_LL24C: {struct Cyc_Absyn_ArrayType_struct*
_tmp4D8=(struct Cyc_Absyn_ArrayType_struct*)_tmp4D7;if(_tmp4D8->tag != 9)goto
_LL24E;else{_tmp4D9=_tmp4D8->f1;_tmp4DA=_tmp4D9.elt_type;_tmp4DB=_tmp4D9.tq;
_tmp4DC=_tmp4D9.num_elts;_tmp4DD=_tmp4D9.zero_term;}}_LL24D: {struct Cyc_Absyn_Exp*
bnd=(struct Cyc_Absyn_Exp*)_check_null(_tmp4DC);void*b;{void*_tmp4DE=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(bnd->topt))->v);void*_tmp4E0;_LL251: {
struct Cyc_Absyn_TagType_struct*_tmp4DF=(struct Cyc_Absyn_TagType_struct*)_tmp4DE;
if(_tmp4DF->tag != 20)goto _LL253;else{_tmp4E0=(void*)_tmp4DF->f1;}}_LL252: b=(void*)({
struct Cyc_Absyn_Upper_b_struct*_tmp4E1=_cycalloc(sizeof(*_tmp4E1));_tmp4E1[0]=({
struct Cyc_Absyn_Upper_b_struct _tmp4E2;_tmp4E2.tag=1;_tmp4E2.f1=Cyc_Absyn_cast_exp(
Cyc_Absyn_uint_typ,Cyc_Absyn_valueof_exp(_tmp4E0,0),0,Cyc_Absyn_No_coercion,0);
_tmp4E2;});_tmp4E1;});goto _LL250;_LL253:;_LL254: if(Cyc_Tcutil_is_const_exp(te,
bnd))b=(void*)({struct Cyc_Absyn_Upper_b_struct*_tmp4E3=_cycalloc(sizeof(*_tmp4E3));
_tmp4E3[0]=({struct Cyc_Absyn_Upper_b_struct _tmp4E4;_tmp4E4.tag=1;_tmp4E4.f1=bnd;
_tmp4E4;});_tmp4E3;});else{b=(void*)& Cyc_Absyn_DynEither_b_val;}_LL250:;}{void*
res_typ=(void*)({struct Cyc_Absyn_PointerType_struct*_tmp4E6=_cycalloc(sizeof(*
_tmp4E6));_tmp4E6[0]=({struct Cyc_Absyn_PointerType_struct _tmp4E7;_tmp4E7.tag=5;
_tmp4E7.f1=({struct Cyc_Absyn_PtrInfo _tmp4E8;_tmp4E8.elt_typ=_tmp4DA;_tmp4E8.elt_tq=
_tmp4DB;_tmp4E8.ptr_atts=({struct Cyc_Absyn_PtrAtts _tmp4E9;_tmp4E9.rgn=rgn;
_tmp4E9.nullable=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();
_tmp4E9.bounds=((union Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)(b);
_tmp4E9.zero_term=_tmp4DD;_tmp4E9.ptrloc=0;_tmp4E9;});_tmp4E8;});_tmp4E7;});
_tmp4E6;});if(topt != 0){if(!Cyc_Tcutil_unify(*topt,res_typ) && Cyc_Tcutil_silent_castable(
te,loc,res_typ,*topt)){e->topt=({struct Cyc_Core_Opt*_tmp4E5=_cycalloc(sizeof(*
_tmp4E5));_tmp4E5->v=res_typ;_tmp4E5;});Cyc_Tcutil_unchecked_cast(te,e,*topt,Cyc_Absyn_Other_coercion);
res_typ=*topt;}}return res_typ;}}_LL24E:;_LL24F:({void*_tmp4EA=0;((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp4EB="tcNew: comprehension returned non-array type";
_tag_dyneither(_tmp4EB,sizeof(char),45);}),_tag_dyneither(_tmp4EA,sizeof(void*),
0));});_LL24B:;}_LL243: {struct Cyc_Absyn_UnresolvedMem_e_struct*_tmp4CE=(struct
Cyc_Absyn_UnresolvedMem_e_struct*)_tmp4CC;if(_tmp4CE->tag != 37)goto _LL245;else{
_tmp4CF=_tmp4CE->f1;_tmp4D0=_tmp4CE->f2;}}_LL244: e1->r=(void*)({struct Cyc_Absyn_Array_e_struct*
_tmp4EC=_cycalloc(sizeof(*_tmp4EC));_tmp4EC[0]=({struct Cyc_Absyn_Array_e_struct
_tmp4ED;_tmp4ED.tag=28;_tmp4ED.f1=_tmp4D0;_tmp4ED;});_tmp4EC;});_tmp4D2=_tmp4D0;
goto _LL246;_LL245: {struct Cyc_Absyn_Array_e_struct*_tmp4D1=(struct Cyc_Absyn_Array_e_struct*)
_tmp4CC;if(_tmp4D1->tag != 28)goto _LL247;else{_tmp4D2=_tmp4D1->f1;}}_LL246: {void**
elt_typ_opt=0;int zero_term=0;if(topt != 0){void*_tmp4EE=Cyc_Tcutil_compress(*topt);
struct Cyc_Absyn_PtrInfo _tmp4F0;void*_tmp4F1;void**_tmp4F2;struct Cyc_Absyn_Tqual
_tmp4F3;struct Cyc_Absyn_PtrAtts _tmp4F4;union Cyc_Absyn_Constraint*_tmp4F5;_LL256: {
struct Cyc_Absyn_PointerType_struct*_tmp4EF=(struct Cyc_Absyn_PointerType_struct*)
_tmp4EE;if(_tmp4EF->tag != 5)goto _LL258;else{_tmp4F0=_tmp4EF->f1;_tmp4F1=_tmp4F0.elt_typ;
_tmp4F2=(void**)&(_tmp4EF->f1).elt_typ;_tmp4F3=_tmp4F0.elt_tq;_tmp4F4=_tmp4F0.ptr_atts;
_tmp4F5=_tmp4F4.zero_term;}}_LL257: elt_typ_opt=(void**)_tmp4F2;zero_term=((int(*)(
int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp4F5);goto _LL255;
_LL258:;_LL259: goto _LL255;_LL255:;}{void*_tmp4F6=Cyc_Tcexp_tcArray(te,e1->loc,
elt_typ_opt,zero_term,_tmp4D2);e1->topt=({struct Cyc_Core_Opt*_tmp4F7=_cycalloc(
sizeof(*_tmp4F7));_tmp4F7->v=_tmp4F6;_tmp4F7;});{void*res_typ;{void*_tmp4F8=Cyc_Tcutil_compress(
_tmp4F6);struct Cyc_Absyn_ArrayInfo _tmp4FA;void*_tmp4FB;struct Cyc_Absyn_Tqual
_tmp4FC;struct Cyc_Absyn_Exp*_tmp4FD;union Cyc_Absyn_Constraint*_tmp4FE;_LL25B: {
struct Cyc_Absyn_ArrayType_struct*_tmp4F9=(struct Cyc_Absyn_ArrayType_struct*)
_tmp4F8;if(_tmp4F9->tag != 9)goto _LL25D;else{_tmp4FA=_tmp4F9->f1;_tmp4FB=_tmp4FA.elt_type;
_tmp4FC=_tmp4FA.tq;_tmp4FD=_tmp4FA.num_elts;_tmp4FE=_tmp4FA.zero_term;}}_LL25C:
res_typ=(void*)({struct Cyc_Absyn_PointerType_struct*_tmp4FF=_cycalloc(sizeof(*
_tmp4FF));_tmp4FF[0]=({struct Cyc_Absyn_PointerType_struct _tmp500;_tmp500.tag=5;
_tmp500.f1=({struct Cyc_Absyn_PtrInfo _tmp501;_tmp501.elt_typ=_tmp4FB;_tmp501.elt_tq=
_tmp4FC;_tmp501.ptr_atts=({struct Cyc_Absyn_PtrAtts _tmp502;_tmp502.rgn=rgn;
_tmp502.nullable=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();
_tmp502.bounds=((union Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)((
void*)({struct Cyc_Absyn_Upper_b_struct*_tmp503=_cycalloc(sizeof(*_tmp503));
_tmp503[0]=({struct Cyc_Absyn_Upper_b_struct _tmp504;_tmp504.tag=1;_tmp504.f1=(
struct Cyc_Absyn_Exp*)_check_null(_tmp4FD);_tmp504;});_tmp503;}));_tmp502.zero_term=
_tmp4FE;_tmp502.ptrloc=0;_tmp502;});_tmp501;});_tmp500;});_tmp4FF;});if(topt != 0){
if(!Cyc_Tcutil_unify(*topt,res_typ) && Cyc_Tcutil_silent_castable(te,loc,res_typ,*
topt)){e->topt=({struct Cyc_Core_Opt*_tmp505=_cycalloc(sizeof(*_tmp505));_tmp505->v=
res_typ;_tmp505;});Cyc_Tcutil_unchecked_cast(te,e,*topt,Cyc_Absyn_Other_coercion);
res_typ=*topt;}}goto _LL25A;_LL25D:;_LL25E:({void*_tmp506=0;((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp507="tcExpNoPromote on Array_e returned non-array type";
_tag_dyneither(_tmp507,sizeof(char),50);}),_tag_dyneither(_tmp506,sizeof(void*),
0));});_LL25A:;}return res_typ;}}}_LL247: {struct Cyc_Absyn_Const_e_struct*_tmp4D3=(
struct Cyc_Absyn_Const_e_struct*)_tmp4CC;if(_tmp4D3->tag != 0)goto _LL249;else{
_tmp4D4=_tmp4D3->f1;if((_tmp4D4.String_c).tag != 7)goto _LL249;_tmp4D5=(struct
_dyneither_ptr)(_tmp4D4.String_c).val;}}_LL248: {void*_tmp508=Cyc_Absyn_atb_typ(
Cyc_Absyn_char_typ,rgn,Cyc_Absyn_const_tqual(0),(void*)& Cyc_Absyn_DynEither_b_val,
Cyc_Absyn_true_conref);void*_tmp509=Cyc_Tcexp_tcExp(te,(void**)& _tmp508,e1);
return Cyc_Absyn_atb_typ(_tmp509,rgn,Cyc_Absyn_empty_tqual(0),(void*)({struct Cyc_Absyn_Upper_b_struct*
_tmp50A=_cycalloc(sizeof(*_tmp50A));_tmp50A[0]=({struct Cyc_Absyn_Upper_b_struct
_tmp50B;_tmp50B.tag=1;_tmp50B.f1=Cyc_Absyn_uint_exp(1,0);_tmp50B;});_tmp50A;}),
Cyc_Absyn_false_conref);}_LL249:;_LL24A: {void**topt2=0;if(topt != 0){void*
_tmp50C=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo _tmp50E;void*_tmp50F;
void**_tmp510;struct Cyc_Absyn_Tqual _tmp511;_LL260: {struct Cyc_Absyn_PointerType_struct*
_tmp50D=(struct Cyc_Absyn_PointerType_struct*)_tmp50C;if(_tmp50D->tag != 5)goto
_LL262;else{_tmp50E=_tmp50D->f1;_tmp50F=_tmp50E.elt_typ;_tmp510=(void**)&(
_tmp50D->f1).elt_typ;_tmp511=_tmp50E.elt_tq;}}_LL261: topt2=(void**)_tmp510;goto
_LL25F;_LL262: {struct Cyc_Absyn_DatatypeType_struct*_tmp512=(struct Cyc_Absyn_DatatypeType_struct*)
_tmp50C;if(_tmp512->tag != 3)goto _LL264;}_LL263: topt2=({void**_tmp513=_cycalloc(
sizeof(*_tmp513));_tmp513[0]=*topt;_tmp513;});goto _LL25F;_LL264:;_LL265: goto
_LL25F;_LL25F:;}{void*telt=Cyc_Tcexp_tcExp(te,topt2,e1);void*res_typ=(void*)({
struct Cyc_Absyn_PointerType_struct*_tmp515=_cycalloc(sizeof(*_tmp515));_tmp515[0]=({
struct Cyc_Absyn_PointerType_struct _tmp516;_tmp516.tag=5;_tmp516.f1=({struct Cyc_Absyn_PtrInfo
_tmp517;_tmp517.elt_typ=telt;_tmp517.elt_tq=Cyc_Absyn_empty_tqual(0);_tmp517.ptr_atts=({
struct Cyc_Absyn_PtrAtts _tmp518;_tmp518.rgn=rgn;_tmp518.nullable=((union Cyc_Absyn_Constraint*(*)())
Cyc_Absyn_empty_conref)();_tmp518.bounds=Cyc_Absyn_bounds_one_conref;_tmp518.zero_term=
Cyc_Absyn_false_conref;_tmp518.ptrloc=0;_tmp518;});_tmp517;});_tmp516;});_tmp515;});
if(topt != 0){if(!Cyc_Tcutil_unify(*topt,res_typ) && Cyc_Tcutil_silent_castable(
te,loc,res_typ,*topt)){e->topt=({struct Cyc_Core_Opt*_tmp514=_cycalloc(sizeof(*
_tmp514));_tmp514->v=res_typ;_tmp514;});Cyc_Tcutil_unchecked_cast(te,e,*topt,Cyc_Absyn_Other_coercion);
res_typ=*topt;}}return res_typ;}}_LL240:;}}void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*
te,void**topt,struct Cyc_Absyn_Exp*e){void*t=Cyc_Tcutil_compress(Cyc_Tcexp_tcExpNoPromote(
te,topt,e));void*_tmp519=t;struct Cyc_Absyn_ArrayInfo _tmp51B;void*_tmp51C;struct
Cyc_Absyn_Tqual _tmp51D;struct Cyc_Absyn_Exp*_tmp51E;union Cyc_Absyn_Constraint*
_tmp51F;_LL267: {struct Cyc_Absyn_ArrayType_struct*_tmp51A=(struct Cyc_Absyn_ArrayType_struct*)
_tmp519;if(_tmp51A->tag != 9)goto _LL269;else{_tmp51B=_tmp51A->f1;_tmp51C=_tmp51B.elt_type;
_tmp51D=_tmp51B.tq;_tmp51E=_tmp51B.num_elts;_tmp51F=_tmp51B.zero_term;}}_LL268: {
void*_tmp521;struct _tuple12 _tmp520=Cyc_Tcutil_addressof_props(te,e);_tmp521=
_tmp520.f2;{void*_tmp522=_tmp51E == 0?(void*)& Cyc_Absyn_DynEither_b_val:(void*)({
struct Cyc_Absyn_Upper_b_struct*_tmp523=_cycalloc(sizeof(*_tmp523));_tmp523[0]=({
struct Cyc_Absyn_Upper_b_struct _tmp524;_tmp524.tag=1;_tmp524.f1=(struct Cyc_Absyn_Exp*)
_tmp51E;_tmp524;});_tmp523;});t=Cyc_Absyn_atb_typ(_tmp51C,_tmp521,_tmp51D,
_tmp522,_tmp51F);((struct Cyc_Core_Opt*)_check_null(e->topt))->v=(void*)t;return t;}}
_LL269:;_LL26A: return t;_LL266:;}void*Cyc_Tcexp_tcExpInitializer(struct Cyc_Tcenv_Tenv*
te,void**topt,struct Cyc_Absyn_Exp*e){void*t=Cyc_Tcexp_tcExpNoPromote(te,topt,e);
struct _RegionHandle*_tmp525=Cyc_Tcenv_get_fnrgn(te);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(
_tmp525,t) && !Cyc_Tcutil_is_noalias_path(_tmp525,e)){({struct Cyc_String_pa_struct
_tmp52A;_tmp52A.tag=0;_tmp52A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(e));{struct Cyc_String_pa_struct _tmp529;_tmp529.tag=0;
_tmp529.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v));{struct Cyc_String_pa_struct
_tmp528;_tmp528.tag=0;_tmp528.f1=(struct _dyneither_ptr)(topt == 0?({const char*
_tmp52B="NULL";_tag_dyneither(_tmp52B,sizeof(char),5);}):(struct _dyneither_ptr)
Cyc_Absynpp_typ2string(*topt));{void*_tmp526[3]={& _tmp528,& _tmp529,& _tmp52A};Cyc_fprintf(
Cyc_stderr,({const char*_tmp527="topt=%s, e->topt->v=%s, e=%s\n";_tag_dyneither(
_tmp527,sizeof(char),30);}),_tag_dyneither(_tmp526,sizeof(void*),3));}}}});({
void*_tmp52C=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp52D="Cannot consume non-unique paths; do swap instead";
_tag_dyneither(_tmp52D,sizeof(char),49);}),_tag_dyneither(_tmp52C,sizeof(void*),
0));});}{void*_tmp52E=e->r;union Cyc_Absyn_Cnst _tmp532;struct _dyneither_ptr
_tmp533;_LL26C: {struct Cyc_Absyn_Array_e_struct*_tmp52F=(struct Cyc_Absyn_Array_e_struct*)
_tmp52E;if(_tmp52F->tag != 28)goto _LL26E;}_LL26D: goto _LL26F;_LL26E: {struct Cyc_Absyn_Comprehension_e_struct*
_tmp530=(struct Cyc_Absyn_Comprehension_e_struct*)_tmp52E;if(_tmp530->tag != 29)
goto _LL270;}_LL26F: goto _LL271;_LL270: {struct Cyc_Absyn_Const_e_struct*_tmp531=(
struct Cyc_Absyn_Const_e_struct*)_tmp52E;if(_tmp531->tag != 0)goto _LL272;else{
_tmp532=_tmp531->f1;if((_tmp532.String_c).tag != 7)goto _LL272;_tmp533=(struct
_dyneither_ptr)(_tmp532.String_c).val;}}_LL271: return t;_LL272:;_LL273: t=Cyc_Tcutil_compress(
t);{void*_tmp534=t;struct Cyc_Absyn_ArrayInfo _tmp536;void*_tmp537;struct Cyc_Absyn_Tqual
_tmp538;struct Cyc_Absyn_Exp*_tmp539;union Cyc_Absyn_Constraint*_tmp53A;_LL275: {
struct Cyc_Absyn_ArrayType_struct*_tmp535=(struct Cyc_Absyn_ArrayType_struct*)
_tmp534;if(_tmp535->tag != 9)goto _LL277;else{_tmp536=_tmp535->f1;_tmp537=_tmp536.elt_type;
_tmp538=_tmp536.tq;_tmp539=_tmp536.num_elts;_tmp53A=_tmp536.zero_term;}}_LL276: {
void*_tmp53C;struct _tuple12 _tmp53B=Cyc_Tcutil_addressof_props(te,e);_tmp53C=
_tmp53B.f2;{void*b=_tmp539 == 0?(void*)& Cyc_Absyn_DynEither_b_val:(void*)({struct
Cyc_Absyn_Upper_b_struct*_tmp53D=_cycalloc(sizeof(*_tmp53D));_tmp53D[0]=({struct
Cyc_Absyn_Upper_b_struct _tmp53E;_tmp53E.tag=1;_tmp53E.f1=(struct Cyc_Absyn_Exp*)
_tmp539;_tmp53E;});_tmp53D;});t=Cyc_Absyn_atb_typ(_tmp537,_tmp53C,_tmp538,b,
_tmp53A);Cyc_Tcutil_unchecked_cast(te,e,t,Cyc_Absyn_Other_coercion);return t;}}
_LL277:;_LL278: return t;_LL274:;}_LL26B:;}}static void*Cyc_Tcexp_tcExpNoPromote(
struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){{void*_tmp53F=e->r;
struct Cyc_Absyn_Exp*_tmp541;_LL27A: {struct Cyc_Absyn_NoInstantiate_e_struct*
_tmp540=(struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp53F;if(_tmp540->tag != 13)
goto _LL27C;else{_tmp541=_tmp540->f1;}}_LL27B: Cyc_Tcexp_tcExpNoInst(te,topt,
_tmp541);((struct Cyc_Core_Opt*)_check_null(_tmp541->topt))->v=(void*)Cyc_Absyn_pointer_expand((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp541->topt))->v,0);e->topt=_tmp541->topt;
goto _LL279;_LL27C:;_LL27D: Cyc_Tcexp_tcExpNoInst(te,topt,e);((struct Cyc_Core_Opt*)
_check_null(e->topt))->v=(void*)Cyc_Absyn_pointer_expand((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,0);{void*_tmp542=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);struct Cyc_Absyn_PtrInfo _tmp544;void*_tmp545;struct Cyc_Absyn_Tqual
_tmp546;struct Cyc_Absyn_PtrAtts _tmp547;void*_tmp548;union Cyc_Absyn_Constraint*
_tmp549;union Cyc_Absyn_Constraint*_tmp54A;union Cyc_Absyn_Constraint*_tmp54B;
_LL27F: {struct Cyc_Absyn_PointerType_struct*_tmp543=(struct Cyc_Absyn_PointerType_struct*)
_tmp542;if(_tmp543->tag != 5)goto _LL281;else{_tmp544=_tmp543->f1;_tmp545=_tmp544.elt_typ;
_tmp546=_tmp544.elt_tq;_tmp547=_tmp544.ptr_atts;_tmp548=_tmp547.rgn;_tmp549=
_tmp547.nullable;_tmp54A=_tmp547.bounds;_tmp54B=_tmp547.zero_term;}}_LL280:{void*
_tmp54C=Cyc_Tcutil_compress(_tmp545);struct Cyc_Absyn_FnInfo _tmp54E;struct Cyc_List_List*
_tmp54F;struct Cyc_Core_Opt*_tmp550;void*_tmp551;struct Cyc_List_List*_tmp552;int
_tmp553;struct Cyc_Absyn_VarargInfo*_tmp554;struct Cyc_List_List*_tmp555;struct Cyc_List_List*
_tmp556;_LL284: {struct Cyc_Absyn_FnType_struct*_tmp54D=(struct Cyc_Absyn_FnType_struct*)
_tmp54C;if(_tmp54D->tag != 10)goto _LL286;else{_tmp54E=_tmp54D->f1;_tmp54F=_tmp54E.tvars;
_tmp550=_tmp54E.effect;_tmp551=_tmp54E.ret_typ;_tmp552=_tmp54E.args;_tmp553=
_tmp54E.c_varargs;_tmp554=_tmp54E.cyc_varargs;_tmp555=_tmp54E.rgn_po;_tmp556=
_tmp54E.attributes;}}_LL285: if(_tmp54F != 0){struct _RegionHandle*_tmp557=Cyc_Tcenv_get_fnrgn(
te);struct _tuple10 _tmp558=({struct _tuple10 _tmp565;_tmp565.f1=Cyc_Tcenv_lookup_type_vars(
te);_tmp565.f2=_tmp557;_tmp565;});struct Cyc_List_List*inst=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _tuple11*(*f)(struct _tuple10*,struct Cyc_Absyn_Tvar*),
struct _tuple10*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(_tmp557,Cyc_Tcutil_r_make_inst_var,&
_tmp558,_tmp54F);struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(void*(*f)(
struct _tuple11*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple11*))
Cyc_Core_snd,inst);void*ftyp=Cyc_Tcutil_rsubstitute(_tmp557,inst,(void*)({struct
Cyc_Absyn_FnType_struct*_tmp562=_cycalloc(sizeof(*_tmp562));_tmp562[0]=({struct
Cyc_Absyn_FnType_struct _tmp563;_tmp563.tag=10;_tmp563.f1=({struct Cyc_Absyn_FnInfo
_tmp564;_tmp564.tvars=0;_tmp564.effect=_tmp550;_tmp564.ret_typ=_tmp551;_tmp564.args=
_tmp552;_tmp564.c_varargs=_tmp553;_tmp564.cyc_varargs=_tmp554;_tmp564.rgn_po=
_tmp555;_tmp564.attributes=_tmp556;_tmp564;});_tmp563;});_tmp562;}));struct Cyc_Absyn_PointerType_struct*
_tmp559=({struct Cyc_Absyn_PointerType_struct*_tmp55E=_cycalloc(sizeof(*_tmp55E));
_tmp55E[0]=({struct Cyc_Absyn_PointerType_struct _tmp55F;_tmp55F.tag=5;_tmp55F.f1=({
struct Cyc_Absyn_PtrInfo _tmp560;_tmp560.elt_typ=ftyp;_tmp560.elt_tq=_tmp546;
_tmp560.ptr_atts=({struct Cyc_Absyn_PtrAtts _tmp561;_tmp561.rgn=_tmp548;_tmp561.nullable=
_tmp549;_tmp561.bounds=_tmp54A;_tmp561.zero_term=_tmp54B;_tmp561.ptrloc=0;
_tmp561;});_tmp560;});_tmp55F;});_tmp55E;});struct Cyc_Absyn_Exp*_tmp55A=Cyc_Absyn_copy_exp(
e);e->r=(void*)({struct Cyc_Absyn_Instantiate_e_struct*_tmp55B=_cycalloc(sizeof(*
_tmp55B));_tmp55B[0]=({struct Cyc_Absyn_Instantiate_e_struct _tmp55C;_tmp55C.tag=
14;_tmp55C.f1=_tmp55A;_tmp55C.f2=ts;_tmp55C;});_tmp55B;});e->topt=({struct Cyc_Core_Opt*
_tmp55D=_cycalloc(sizeof(*_tmp55D));_tmp55D->v=(void*)_tmp559;_tmp55D;});}goto
_LL283;_LL286:;_LL287: goto _LL283;_LL283:;}goto _LL27E;_LL281:;_LL282: goto _LL27E;
_LL27E:;}goto _LL279;_LL279:;}return(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;}
static void Cyc_Tcexp_tcExpNoInst(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*
e){struct Cyc_Position_Segment*loc=e->loc;void*t;{void*_tmp566=e->r;struct Cyc_Absyn_Exp*
_tmp568;struct _tuple1*_tmp56A;struct Cyc_Absyn_Exp*_tmp56C;struct Cyc_List_List*
_tmp56D;struct Cyc_Core_Opt*_tmp56F;struct Cyc_List_List*_tmp570;union Cyc_Absyn_Cnst
_tmp572;union Cyc_Absyn_Cnst*_tmp573;struct _tuple1*_tmp575;void*_tmp576;enum Cyc_Absyn_Primop
_tmp578;struct Cyc_List_List*_tmp579;struct Cyc_Absyn_Exp*_tmp57B;enum Cyc_Absyn_Incrementor
_tmp57C;struct Cyc_Absyn_Exp*_tmp57E;struct Cyc_Core_Opt*_tmp57F;struct Cyc_Absyn_Exp*
_tmp580;struct Cyc_Absyn_Exp*_tmp582;struct Cyc_Absyn_Exp*_tmp583;struct Cyc_Absyn_Exp*
_tmp584;struct Cyc_Absyn_Exp*_tmp586;struct Cyc_Absyn_Exp*_tmp587;struct Cyc_Absyn_Exp*
_tmp589;struct Cyc_Absyn_Exp*_tmp58A;struct Cyc_Absyn_Exp*_tmp58C;struct Cyc_Absyn_Exp*
_tmp58D;struct Cyc_Absyn_Exp*_tmp58F;struct Cyc_List_List*_tmp590;struct Cyc_Absyn_VarargCallInfo*
_tmp591;struct Cyc_Absyn_VarargCallInfo**_tmp592;struct Cyc_Absyn_Exp*_tmp594;
struct Cyc_Absyn_Exp*_tmp596;struct Cyc_List_List*_tmp597;void*_tmp599;struct Cyc_Absyn_Exp*
_tmp59A;enum Cyc_Absyn_Coercion _tmp59B;enum Cyc_Absyn_Coercion*_tmp59C;struct Cyc_Absyn_Exp*
_tmp59E;struct Cyc_Absyn_Exp*_tmp5A0;struct Cyc_Absyn_Exp*_tmp5A1;struct Cyc_Absyn_Exp*
_tmp5A3;void*_tmp5A5;void*_tmp5A7;void*_tmp5A8;struct Cyc_Absyn_Exp*_tmp5AB;
struct Cyc_Absyn_Exp*_tmp5AD;struct _dyneither_ptr*_tmp5AE;int _tmp5AF;int*_tmp5B0;
int _tmp5B1;int*_tmp5B2;struct Cyc_Absyn_Exp*_tmp5B4;struct _dyneither_ptr*_tmp5B5;
int _tmp5B6;int*_tmp5B7;int _tmp5B8;int*_tmp5B9;struct Cyc_Absyn_Exp*_tmp5BB;struct
Cyc_Absyn_Exp*_tmp5BC;struct Cyc_List_List*_tmp5BE;struct _tuple8*_tmp5C0;struct
Cyc_List_List*_tmp5C1;struct Cyc_List_List*_tmp5C3;struct Cyc_Absyn_Stmt*_tmp5C5;
struct Cyc_Absyn_Vardecl*_tmp5C7;struct Cyc_Absyn_Exp*_tmp5C8;struct Cyc_Absyn_Exp*
_tmp5C9;int _tmp5CA;int*_tmp5CB;struct _tuple1*_tmp5CD;struct _tuple1**_tmp5CE;
struct Cyc_List_List*_tmp5CF;struct Cyc_List_List**_tmp5D0;struct Cyc_List_List*
_tmp5D1;struct Cyc_Absyn_Aggrdecl*_tmp5D2;struct Cyc_Absyn_Aggrdecl**_tmp5D3;void*
_tmp5D5;struct Cyc_List_List*_tmp5D6;struct Cyc_List_List*_tmp5D8;struct Cyc_Absyn_Datatypedecl*
_tmp5D9;struct Cyc_Absyn_Datatypefield*_tmp5DA;struct _tuple1*_tmp5DC;struct
_tuple1**_tmp5DD;struct Cyc_Absyn_Enumdecl*_tmp5DE;struct Cyc_Absyn_Enumfield*
_tmp5DF;struct _tuple1*_tmp5E1;struct _tuple1**_tmp5E2;void*_tmp5E3;struct Cyc_Absyn_Enumfield*
_tmp5E4;struct Cyc_Absyn_MallocInfo _tmp5E6;int _tmp5E7;int*_tmp5E8;struct Cyc_Absyn_Exp*
_tmp5E9;void**_tmp5EA;void***_tmp5EB;struct Cyc_Absyn_Exp*_tmp5EC;struct Cyc_Absyn_Exp**
_tmp5ED;int _tmp5EE;int*_tmp5EF;struct Cyc_Absyn_Exp*_tmp5F1;struct Cyc_Absyn_Exp*
_tmp5F2;struct Cyc_Absyn_Exp*_tmp5F4;struct _dyneither_ptr*_tmp5F5;void*_tmp5F7;
_LL289: {struct Cyc_Absyn_NoInstantiate_e_struct*_tmp567=(struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmp566;if(_tmp567->tag != 13)goto _LL28B;else{_tmp568=_tmp567->f1;}}_LL28A: Cyc_Tcexp_tcExpNoInst(
te,0,_tmp568);return;_LL28B: {struct Cyc_Absyn_UnknownId_e_struct*_tmp569=(struct
Cyc_Absyn_UnknownId_e_struct*)_tmp566;if(_tmp569->tag != 2)goto _LL28D;else{
_tmp56A=_tmp569->f1;}}_LL28C: Cyc_Tcexp_resolve_unknown_id(te,e,_tmp56A);Cyc_Tcexp_tcExpNoInst(
te,topt,e);return;_LL28D: {struct Cyc_Absyn_UnknownCall_e_struct*_tmp56B=(struct
Cyc_Absyn_UnknownCall_e_struct*)_tmp566;if(_tmp56B->tag != 10)goto _LL28F;else{
_tmp56C=_tmp56B->f1;_tmp56D=_tmp56B->f2;}}_LL28E: Cyc_Tcexp_resolve_unknown_fn(te,
e,_tmp56C,_tmp56D);Cyc_Tcexp_tcExpNoInst(te,topt,e);return;_LL28F: {struct Cyc_Absyn_UnresolvedMem_e_struct*
_tmp56E=(struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp566;if(_tmp56E->tag != 37)
goto _LL291;else{_tmp56F=_tmp56E->f1;_tmp570=_tmp56E->f2;}}_LL290: Cyc_Tcexp_resolve_unresolved_mem(
te,loc,topt,e,_tmp570);Cyc_Tcexp_tcExpNoInst(te,topt,e);return;_LL291: {struct
Cyc_Absyn_Const_e_struct*_tmp571=(struct Cyc_Absyn_Const_e_struct*)_tmp566;if(
_tmp571->tag != 0)goto _LL293;else{_tmp572=_tmp571->f1;_tmp573=(union Cyc_Absyn_Cnst*)&
_tmp571->f1;}}_LL292: t=Cyc_Tcexp_tcConst(te,loc,topt,(union Cyc_Absyn_Cnst*)
_tmp573,e);goto _LL288;_LL293: {struct Cyc_Absyn_Var_e_struct*_tmp574=(struct Cyc_Absyn_Var_e_struct*)
_tmp566;if(_tmp574->tag != 1)goto _LL295;else{_tmp575=_tmp574->f1;_tmp576=(void*)
_tmp574->f2;}}_LL294: t=Cyc_Tcexp_tcVar(te,loc,_tmp575,_tmp576);goto _LL288;_LL295: {
struct Cyc_Absyn_Primop_e_struct*_tmp577=(struct Cyc_Absyn_Primop_e_struct*)
_tmp566;if(_tmp577->tag != 3)goto _LL297;else{_tmp578=_tmp577->f1;_tmp579=_tmp577->f2;}}
_LL296: t=Cyc_Tcexp_tcPrimop(te,loc,topt,_tmp578,_tmp579);goto _LL288;_LL297: {
struct Cyc_Absyn_Increment_e_struct*_tmp57A=(struct Cyc_Absyn_Increment_e_struct*)
_tmp566;if(_tmp57A->tag != 5)goto _LL299;else{_tmp57B=_tmp57A->f1;_tmp57C=_tmp57A->f2;}}
_LL298: t=Cyc_Tcexp_tcIncrement(te,loc,topt,_tmp57B,_tmp57C);goto _LL288;_LL299: {
struct Cyc_Absyn_AssignOp_e_struct*_tmp57D=(struct Cyc_Absyn_AssignOp_e_struct*)
_tmp566;if(_tmp57D->tag != 4)goto _LL29B;else{_tmp57E=_tmp57D->f1;_tmp57F=_tmp57D->f2;
_tmp580=_tmp57D->f3;}}_LL29A: t=Cyc_Tcexp_tcAssignOp(te,loc,topt,_tmp57E,_tmp57F,
_tmp580);goto _LL288;_LL29B: {struct Cyc_Absyn_Conditional_e_struct*_tmp581=(
struct Cyc_Absyn_Conditional_e_struct*)_tmp566;if(_tmp581->tag != 6)goto _LL29D;
else{_tmp582=_tmp581->f1;_tmp583=_tmp581->f2;_tmp584=_tmp581->f3;}}_LL29C: t=Cyc_Tcexp_tcConditional(
te,loc,topt,_tmp582,_tmp583,_tmp584);goto _LL288;_LL29D: {struct Cyc_Absyn_And_e_struct*
_tmp585=(struct Cyc_Absyn_And_e_struct*)_tmp566;if(_tmp585->tag != 7)goto _LL29F;
else{_tmp586=_tmp585->f1;_tmp587=_tmp585->f2;}}_LL29E: t=Cyc_Tcexp_tcAnd(te,loc,
_tmp586,_tmp587);goto _LL288;_LL29F: {struct Cyc_Absyn_Or_e_struct*_tmp588=(struct
Cyc_Absyn_Or_e_struct*)_tmp566;if(_tmp588->tag != 8)goto _LL2A1;else{_tmp589=
_tmp588->f1;_tmp58A=_tmp588->f2;}}_LL2A0: t=Cyc_Tcexp_tcOr(te,loc,_tmp589,_tmp58A);
goto _LL288;_LL2A1: {struct Cyc_Absyn_SeqExp_e_struct*_tmp58B=(struct Cyc_Absyn_SeqExp_e_struct*)
_tmp566;if(_tmp58B->tag != 9)goto _LL2A3;else{_tmp58C=_tmp58B->f1;_tmp58D=_tmp58B->f2;}}
_LL2A2: t=Cyc_Tcexp_tcSeqExp(te,loc,topt,_tmp58C,_tmp58D);goto _LL288;_LL2A3: {
struct Cyc_Absyn_FnCall_e_struct*_tmp58E=(struct Cyc_Absyn_FnCall_e_struct*)
_tmp566;if(_tmp58E->tag != 11)goto _LL2A5;else{_tmp58F=_tmp58E->f1;_tmp590=_tmp58E->f2;
_tmp591=_tmp58E->f3;_tmp592=(struct Cyc_Absyn_VarargCallInfo**)& _tmp58E->f3;}}
_LL2A4: t=Cyc_Tcexp_tcFnCall(te,loc,topt,_tmp58F,_tmp590,_tmp592);goto _LL288;
_LL2A5: {struct Cyc_Absyn_Throw_e_struct*_tmp593=(struct Cyc_Absyn_Throw_e_struct*)
_tmp566;if(_tmp593->tag != 12)goto _LL2A7;else{_tmp594=_tmp593->f1;}}_LL2A6: t=Cyc_Tcexp_tcThrow(
te,loc,topt,_tmp594);goto _LL288;_LL2A7: {struct Cyc_Absyn_Instantiate_e_struct*
_tmp595=(struct Cyc_Absyn_Instantiate_e_struct*)_tmp566;if(_tmp595->tag != 14)goto
_LL2A9;else{_tmp596=_tmp595->f1;_tmp597=_tmp595->f2;}}_LL2A8: t=Cyc_Tcexp_tcInstantiate(
te,loc,topt,_tmp596,_tmp597);goto _LL288;_LL2A9: {struct Cyc_Absyn_Cast_e_struct*
_tmp598=(struct Cyc_Absyn_Cast_e_struct*)_tmp566;if(_tmp598->tag != 15)goto _LL2AB;
else{_tmp599=(void*)_tmp598->f1;_tmp59A=_tmp598->f2;_tmp59B=_tmp598->f4;_tmp59C=(
enum Cyc_Absyn_Coercion*)& _tmp598->f4;}}_LL2AA: t=Cyc_Tcexp_tcCast(te,loc,topt,
_tmp599,_tmp59A,(enum Cyc_Absyn_Coercion*)_tmp59C);goto _LL288;_LL2AB: {struct Cyc_Absyn_Address_e_struct*
_tmp59D=(struct Cyc_Absyn_Address_e_struct*)_tmp566;if(_tmp59D->tag != 16)goto
_LL2AD;else{_tmp59E=_tmp59D->f1;}}_LL2AC: t=Cyc_Tcexp_tcAddress(te,loc,e,topt,
_tmp59E);goto _LL288;_LL2AD: {struct Cyc_Absyn_New_e_struct*_tmp59F=(struct Cyc_Absyn_New_e_struct*)
_tmp566;if(_tmp59F->tag != 17)goto _LL2AF;else{_tmp5A0=_tmp59F->f1;_tmp5A1=_tmp59F->f2;}}
_LL2AE: t=Cyc_Tcexp_tcNew(te,loc,topt,_tmp5A0,e,_tmp5A1);goto _LL288;_LL2AF: {
struct Cyc_Absyn_Sizeofexp_e_struct*_tmp5A2=(struct Cyc_Absyn_Sizeofexp_e_struct*)
_tmp566;if(_tmp5A2->tag != 19)goto _LL2B1;else{_tmp5A3=_tmp5A2->f1;}}_LL2B0: {void*
_tmp5F8=Cyc_Tcexp_tcExpNoPromote(te,0,_tmp5A3);t=Cyc_Tcexp_tcSizeof(te,loc,topt,
_tmp5F8);goto _LL288;}_LL2B1: {struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp5A4=(struct
Cyc_Absyn_Sizeoftyp_e_struct*)_tmp566;if(_tmp5A4->tag != 18)goto _LL2B3;else{
_tmp5A5=(void*)_tmp5A4->f1;}}_LL2B2: t=Cyc_Tcexp_tcSizeof(te,loc,topt,_tmp5A5);
goto _LL288;_LL2B3: {struct Cyc_Absyn_Offsetof_e_struct*_tmp5A6=(struct Cyc_Absyn_Offsetof_e_struct*)
_tmp566;if(_tmp5A6->tag != 20)goto _LL2B5;else{_tmp5A7=(void*)_tmp5A6->f1;_tmp5A8=(
void*)_tmp5A6->f2;}}_LL2B4: t=Cyc_Tcexp_tcOffsetof(te,loc,topt,_tmp5A7,_tmp5A8);
goto _LL288;_LL2B5: {struct Cyc_Absyn_Gentyp_e_struct*_tmp5A9=(struct Cyc_Absyn_Gentyp_e_struct*)
_tmp566;if(_tmp5A9->tag != 21)goto _LL2B7;}_LL2B6:({void*_tmp5F9=0;Cyc_Tcutil_terr(
loc,({const char*_tmp5FA="gen() not in top-level initializer";_tag_dyneither(
_tmp5FA,sizeof(char),35);}),_tag_dyneither(_tmp5F9,sizeof(void*),0));});return;
_LL2B7: {struct Cyc_Absyn_Deref_e_struct*_tmp5AA=(struct Cyc_Absyn_Deref_e_struct*)
_tmp566;if(_tmp5AA->tag != 22)goto _LL2B9;else{_tmp5AB=_tmp5AA->f1;}}_LL2B8: t=Cyc_Tcexp_tcDeref(
te,loc,topt,_tmp5AB);goto _LL288;_LL2B9: {struct Cyc_Absyn_AggrMember_e_struct*
_tmp5AC=(struct Cyc_Absyn_AggrMember_e_struct*)_tmp566;if(_tmp5AC->tag != 23)goto
_LL2BB;else{_tmp5AD=_tmp5AC->f1;_tmp5AE=_tmp5AC->f2;_tmp5AF=_tmp5AC->f3;_tmp5B0=(
int*)& _tmp5AC->f3;_tmp5B1=_tmp5AC->f4;_tmp5B2=(int*)& _tmp5AC->f4;}}_LL2BA: t=Cyc_Tcexp_tcAggrMember(
te,loc,topt,e,_tmp5AD,_tmp5AE,_tmp5B0,_tmp5B2);goto _LL288;_LL2BB: {struct Cyc_Absyn_AggrArrow_e_struct*
_tmp5B3=(struct Cyc_Absyn_AggrArrow_e_struct*)_tmp566;if(_tmp5B3->tag != 24)goto
_LL2BD;else{_tmp5B4=_tmp5B3->f1;_tmp5B5=_tmp5B3->f2;_tmp5B6=_tmp5B3->f3;_tmp5B7=(
int*)& _tmp5B3->f3;_tmp5B8=_tmp5B3->f4;_tmp5B9=(int*)& _tmp5B3->f4;}}_LL2BC: t=Cyc_Tcexp_tcAggrArrow(
te,loc,topt,_tmp5B4,_tmp5B5,_tmp5B7,_tmp5B9);goto _LL288;_LL2BD: {struct Cyc_Absyn_Subscript_e_struct*
_tmp5BA=(struct Cyc_Absyn_Subscript_e_struct*)_tmp566;if(_tmp5BA->tag != 25)goto
_LL2BF;else{_tmp5BB=_tmp5BA->f1;_tmp5BC=_tmp5BA->f2;}}_LL2BE: t=Cyc_Tcexp_tcSubscript(
te,loc,topt,_tmp5BB,_tmp5BC);goto _LL288;_LL2BF: {struct Cyc_Absyn_Tuple_e_struct*
_tmp5BD=(struct Cyc_Absyn_Tuple_e_struct*)_tmp566;if(_tmp5BD->tag != 26)goto _LL2C1;
else{_tmp5BE=_tmp5BD->f1;}}_LL2C0: t=Cyc_Tcexp_tcTuple(te,loc,topt,_tmp5BE);goto
_LL288;_LL2C1: {struct Cyc_Absyn_CompoundLit_e_struct*_tmp5BF=(struct Cyc_Absyn_CompoundLit_e_struct*)
_tmp566;if(_tmp5BF->tag != 27)goto _LL2C3;else{_tmp5C0=_tmp5BF->f1;_tmp5C1=_tmp5BF->f2;}}
_LL2C2: t=Cyc_Tcexp_tcCompoundLit(te,loc,topt,_tmp5C0,_tmp5C1);goto _LL288;_LL2C3: {
struct Cyc_Absyn_Array_e_struct*_tmp5C2=(struct Cyc_Absyn_Array_e_struct*)_tmp566;
if(_tmp5C2->tag != 28)goto _LL2C5;else{_tmp5C3=_tmp5C2->f1;}}_LL2C4: {void**
elt_topt=0;int zero_term=0;if(topt != 0){void*_tmp5FB=Cyc_Tcutil_compress(*topt);
struct Cyc_Absyn_ArrayInfo _tmp5FD;void*_tmp5FE;void**_tmp5FF;union Cyc_Absyn_Constraint*
_tmp600;_LL2DC: {struct Cyc_Absyn_ArrayType_struct*_tmp5FC=(struct Cyc_Absyn_ArrayType_struct*)
_tmp5FB;if(_tmp5FC->tag != 9)goto _LL2DE;else{_tmp5FD=_tmp5FC->f1;_tmp5FE=_tmp5FD.elt_type;
_tmp5FF=(void**)&(_tmp5FC->f1).elt_type;_tmp600=_tmp5FD.zero_term;}}_LL2DD:
elt_topt=(void**)_tmp5FF;zero_term=((int(*)(int y,union Cyc_Absyn_Constraint*x))
Cyc_Absyn_conref_def)(0,_tmp600);goto _LL2DB;_LL2DE:;_LL2DF: goto _LL2DB;_LL2DB:;}t=
Cyc_Tcexp_tcArray(te,loc,elt_topt,zero_term,_tmp5C3);goto _LL288;}_LL2C5: {struct
Cyc_Absyn_StmtExp_e_struct*_tmp5C4=(struct Cyc_Absyn_StmtExp_e_struct*)_tmp566;
if(_tmp5C4->tag != 38)goto _LL2C7;else{_tmp5C5=_tmp5C4->f1;}}_LL2C6: t=Cyc_Tcexp_tcStmtExp(
te,loc,topt,_tmp5C5);goto _LL288;_LL2C7: {struct Cyc_Absyn_Comprehension_e_struct*
_tmp5C6=(struct Cyc_Absyn_Comprehension_e_struct*)_tmp566;if(_tmp5C6->tag != 29)
goto _LL2C9;else{_tmp5C7=_tmp5C6->f1;_tmp5C8=_tmp5C6->f2;_tmp5C9=_tmp5C6->f3;
_tmp5CA=_tmp5C6->f4;_tmp5CB=(int*)& _tmp5C6->f4;}}_LL2C8: t=Cyc_Tcexp_tcComprehension(
te,loc,topt,_tmp5C7,_tmp5C8,_tmp5C9,_tmp5CB);goto _LL288;_LL2C9: {struct Cyc_Absyn_Aggregate_e_struct*
_tmp5CC=(struct Cyc_Absyn_Aggregate_e_struct*)_tmp566;if(_tmp5CC->tag != 30)goto
_LL2CB;else{_tmp5CD=_tmp5CC->f1;_tmp5CE=(struct _tuple1**)& _tmp5CC->f1;_tmp5CF=
_tmp5CC->f2;_tmp5D0=(struct Cyc_List_List**)& _tmp5CC->f2;_tmp5D1=_tmp5CC->f3;
_tmp5D2=_tmp5CC->f4;_tmp5D3=(struct Cyc_Absyn_Aggrdecl**)& _tmp5CC->f4;}}_LL2CA: t=
Cyc_Tcexp_tcAggregate(te,loc,topt,_tmp5CE,_tmp5D0,_tmp5D1,_tmp5D3);goto _LL288;
_LL2CB: {struct Cyc_Absyn_AnonStruct_e_struct*_tmp5D4=(struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp566;if(_tmp5D4->tag != 31)goto _LL2CD;else{_tmp5D5=(void*)_tmp5D4->f1;_tmp5D6=
_tmp5D4->f2;}}_LL2CC: t=Cyc_Tcexp_tcAnonStruct(te,loc,_tmp5D5,_tmp5D6);goto _LL288;
_LL2CD: {struct Cyc_Absyn_Datatype_e_struct*_tmp5D7=(struct Cyc_Absyn_Datatype_e_struct*)
_tmp566;if(_tmp5D7->tag != 32)goto _LL2CF;else{_tmp5D8=_tmp5D7->f1;_tmp5D9=_tmp5D7->f2;
_tmp5DA=_tmp5D7->f3;}}_LL2CE: t=Cyc_Tcexp_tcDatatype(te,loc,topt,e,_tmp5D8,
_tmp5D9,_tmp5DA);goto _LL288;_LL2CF: {struct Cyc_Absyn_Enum_e_struct*_tmp5DB=(
struct Cyc_Absyn_Enum_e_struct*)_tmp566;if(_tmp5DB->tag != 33)goto _LL2D1;else{
_tmp5DC=_tmp5DB->f1;_tmp5DD=(struct _tuple1**)& _tmp5DB->f1;_tmp5DE=_tmp5DB->f2;
_tmp5DF=_tmp5DB->f3;}}_LL2D0:*_tmp5DD=((struct Cyc_Absyn_Enumfield*)_check_null(
_tmp5DF))->name;t=(void*)({struct Cyc_Absyn_EnumType_struct*_tmp601=_cycalloc(
sizeof(*_tmp601));_tmp601[0]=({struct Cyc_Absyn_EnumType_struct _tmp602;_tmp602.tag=
14;_tmp602.f1=((struct Cyc_Absyn_Enumdecl*)_check_null(_tmp5DE))->name;_tmp602.f2=
_tmp5DE;_tmp602;});_tmp601;});goto _LL288;_LL2D1: {struct Cyc_Absyn_AnonEnum_e_struct*
_tmp5E0=(struct Cyc_Absyn_AnonEnum_e_struct*)_tmp566;if(_tmp5E0->tag != 34)goto
_LL2D3;else{_tmp5E1=_tmp5E0->f1;_tmp5E2=(struct _tuple1**)& _tmp5E0->f1;_tmp5E3=(
void*)_tmp5E0->f2;_tmp5E4=_tmp5E0->f3;}}_LL2D2:*_tmp5E2=((struct Cyc_Absyn_Enumfield*)
_check_null(_tmp5E4))->name;t=_tmp5E3;goto _LL288;_LL2D3: {struct Cyc_Absyn_Malloc_e_struct*
_tmp5E5=(struct Cyc_Absyn_Malloc_e_struct*)_tmp566;if(_tmp5E5->tag != 35)goto
_LL2D5;else{_tmp5E6=_tmp5E5->f1;_tmp5E7=_tmp5E6.is_calloc;_tmp5E8=(int*)&(
_tmp5E5->f1).is_calloc;_tmp5E9=_tmp5E6.rgn;_tmp5EA=_tmp5E6.elt_type;_tmp5EB=(
void***)&(_tmp5E5->f1).elt_type;_tmp5EC=_tmp5E6.num_elts;_tmp5ED=(struct Cyc_Absyn_Exp**)&(
_tmp5E5->f1).num_elts;_tmp5EE=_tmp5E6.fat_result;_tmp5EF=(int*)&(_tmp5E5->f1).fat_result;}}
_LL2D4: t=Cyc_Tcexp_tcMalloc(te,loc,topt,_tmp5E9,_tmp5EB,_tmp5ED,_tmp5E8,_tmp5EF);
goto _LL288;_LL2D5: {struct Cyc_Absyn_Swap_e_struct*_tmp5F0=(struct Cyc_Absyn_Swap_e_struct*)
_tmp566;if(_tmp5F0->tag != 36)goto _LL2D7;else{_tmp5F1=_tmp5F0->f1;_tmp5F2=_tmp5F0->f2;}}
_LL2D6: t=Cyc_Tcexp_tcSwap(te,loc,topt,_tmp5F1,_tmp5F2);goto _LL288;_LL2D7: {
struct Cyc_Absyn_Tagcheck_e_struct*_tmp5F3=(struct Cyc_Absyn_Tagcheck_e_struct*)
_tmp566;if(_tmp5F3->tag != 39)goto _LL2D9;else{_tmp5F4=_tmp5F3->f1;_tmp5F5=_tmp5F3->f2;}}
_LL2D8: t=Cyc_Tcexp_tcTagcheck(te,loc,topt,_tmp5F4,_tmp5F5);goto _LL288;_LL2D9: {
struct Cyc_Absyn_Valueof_e_struct*_tmp5F6=(struct Cyc_Absyn_Valueof_e_struct*)
_tmp566;if(_tmp5F6->tag != 40)goto _LL288;else{_tmp5F7=(void*)_tmp5F6->f1;}}_LL2DA:
if(!te->allow_valueof)({void*_tmp603=0;Cyc_Tcutil_terr(e->loc,({const char*
_tmp604="valueof(-) can only occur within types";_tag_dyneither(_tmp604,sizeof(
char),39);}),_tag_dyneither(_tmp603,sizeof(void*),0));});t=Cyc_Absyn_sint_typ;
goto _LL288;_LL288:;}e->topt=({struct Cyc_Core_Opt*_tmp605=_cycalloc(sizeof(*
_tmp605));_tmp605->v=t;_tmp605;});}
