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
extern char Cyc_Core_Invalid_argument[21];struct Cyc_Core_Invalid_argument_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[12];struct Cyc_Core_Failure_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[15];struct Cyc_Core_Impossible_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[14];extern char Cyc_Core_Unreachable[
16];struct Cyc_Core_Unreachable_struct{char*tag;struct _dyneither_ptr f1;};extern
struct _RegionHandle*Cyc_Core_heap_region;extern char Cyc_Core_Open_Region[16];
extern char Cyc_Core_Free_Region[16];struct Cyc_List_List{void*hd;struct Cyc_List_List*
tl;};int Cyc_List_length(struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_map(
void*(*f)(void*),struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rmap(struct
_RegionHandle*,void*(*f)(void*),struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rmap_c(
struct _RegionHandle*,void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);
extern char Cyc_List_List_mismatch[18];struct Cyc_List_List*Cyc_List_imp_rev(struct
Cyc_List_List*x);struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,
struct Cyc_List_List*y);struct Cyc_List_List*Cyc_List_rappend(struct _RegionHandle*,
struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[8];void*Cyc_List_nth(
struct Cyc_List_List*x,int n);struct Cyc_List_List*Cyc_List_nth_tail(struct Cyc_List_List*
x,int i);int Cyc_List_exists_c(int(*pred)(void*,void*),void*env,struct Cyc_List_List*
x);struct Cyc_List_List*Cyc_List_rzip(struct _RegionHandle*r1,struct _RegionHandle*
r2,struct Cyc_List_List*x,struct Cyc_List_List*y);struct Cyc_Lineno_Pos{struct
_dyneither_ptr logical_file;struct _dyneither_ptr line;int line_no;int col;};extern
char Cyc_Position_Exit[9];struct Cyc_Position_Segment;struct Cyc_Position_Error{
struct _dyneither_ptr source;struct Cyc_Position_Segment*seg;void*kind;struct
_dyneither_ptr desc;};extern char Cyc_Position_Nocontext[14];struct
_union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{
int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};
union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct
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
Cyc_Absyn_KnownDatatypefield(struct Cyc_Absyn_Datatypedecl*,struct Cyc_Absyn_Datatypefield*);
struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;
struct Cyc_List_List*targs;};struct _tuple3{void*f1;struct _tuple1*f2;struct Cyc_Core_Opt*
f3;};struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple3 val;};struct
_union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{
struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr
KnownAggr;};union Cyc_Absyn_AggrInfoU Cyc_Absyn_KnownAggr(struct Cyc_Absyn_Aggrdecl**);
struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*
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
_union_Cnst_String_c String_c;};extern union Cyc_Absyn_Cnst Cyc_Absyn_Null_c;union
Cyc_Absyn_Cnst Cyc_Absyn_Char_c(void*,char);union Cyc_Absyn_Cnst Cyc_Absyn_Short_c(
void*,short);union Cyc_Absyn_Cnst Cyc_Absyn_Int_c(void*,int);struct Cyc_Absyn_VarargCallInfo{
int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};
struct Cyc_Absyn_StructField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_struct{
int tag;unsigned int f1;};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*
rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;};struct Cyc_Absyn_Const_e_struct{
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
_dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(
struct Cyc_Position_Segment*);struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(struct
Cyc_Position_Segment*);union Cyc_Absyn_Constraint*Cyc_Absyn_new_conref(void*x);
union Cyc_Absyn_Constraint*Cyc_Absyn_empty_conref();void*Cyc_Absyn_conref_val(
union Cyc_Absyn_Constraint*x);void*Cyc_Absyn_conref_def(void*y,union Cyc_Absyn_Constraint*
x);void*Cyc_Absyn_conref_constr(void*y,union Cyc_Absyn_Constraint*x);extern union
Cyc_Absyn_Constraint*Cyc_Absyn_true_conref;extern union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_bounds_one_conref;extern union Cyc_Absyn_Constraint*
Cyc_Absyn_bounds_dyneither_conref;void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,
struct Cyc_Core_Opt*tenv);void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);extern void*
Cyc_Absyn_char_typ;extern void*Cyc_Absyn_uchar_typ;extern void*Cyc_Absyn_ushort_typ;
extern void*Cyc_Absyn_uint_typ;extern void*Cyc_Absyn_ulonglong_typ;extern void*Cyc_Absyn_schar_typ;
extern void*Cyc_Absyn_sshort_typ;extern void*Cyc_Absyn_sint_typ;extern void*Cyc_Absyn_slonglong_typ;
extern void*Cyc_Absyn_float_typ;extern void*Cyc_Absyn_exn_typ;extern void*Cyc_Absyn_bounds_one;
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
void*,struct Cyc_Absyn_Exp*,int user_cast,void*,struct Cyc_Position_Segment*);
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
struct _dyneither_ptr);extern char Cyc_FileCloseError[19];extern char Cyc_FileOpenError[
18];struct Cyc_FileOpenError_struct{char*tag;struct _dyneither_ptr f1;};struct Cyc_PP_Ppstate;
struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs: 1;
int qvar_to_Cids: 1;int add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int
rewrite_temp_tvars: 1;int print_all_tvars: 1;int print_all_kinds: 1;int
print_all_effects: 1;int print_using_stmts: 1;int print_externC_stmts: 1;int
print_full_evars: 1;int print_zeroterm: 1;int generate_line_directives: 1;int
use_curr_namespace: 1;struct Cyc_List_List*curr_namespace;};struct _dyneither_ptr
Cyc_Absynpp_typ2string(void*);struct _dyneither_ptr Cyc_Absynpp_exp2string(struct
Cyc_Absyn_Exp*);struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple1*);
struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(
struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;extern char Cyc_Set_Absent[11];struct
Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;
struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[12];extern char Cyc_Dict_Absent[11];
struct _tuple0*Cyc_Dict_rchoose(struct _RegionHandle*r,struct Cyc_Dict_Dict d);
struct _tuple0*Cyc_Dict_rchoose(struct _RegionHandle*,struct Cyc_Dict_Dict d);struct
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
int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Tcenv_DatatypeRes_struct{int tag;
struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct Cyc_Tcenv_EnumRes_struct{
int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcenv_AnonEnumRes_struct{
int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcenv_Genv{struct
_RegionHandle*grgn;struct Cyc_Set_Set*namespaces;struct Cyc_Dict_Dict aggrdecls;
struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict
typedefs;struct Cyc_Dict_Dict ordinaries;struct Cyc_List_List*availables;};struct
Cyc_Tcenv_Fenv;struct Cyc_Tcenv_Stmt_j_struct{int tag;struct Cyc_Absyn_Stmt*f1;};
struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Dict_Dict ae;struct Cyc_Tcenv_Fenv*
le;int allow_valueof;};struct _RegionHandle*Cyc_Tcenv_get_fnrgn(struct Cyc_Tcenv_Tenv*);
void*Cyc_Tcenv_lookup_ordinary(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,struct
Cyc_Position_Segment*,struct _tuple1*);struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl(
struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,struct _tuple1*);struct Cyc_Absyn_Datatypedecl**
Cyc_Tcenv_lookup_datatypedecl(struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,
struct _tuple1*);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_copy_tenv(struct _RegionHandle*,
struct Cyc_Tcenv_Tenv*);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_local_var(struct
_RegionHandle*,struct Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Vardecl*);
struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*);struct Cyc_Core_Opt*
Cyc_Tcenv_lookup_opt_type_vars(struct Cyc_Tcenv_Tenv*te);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_notreadctxt(
struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_notreadctxt(
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
_dyneither_ptr ap);int Cyc_Tcutil_kind_leq(void*k1,void*k2);void*Cyc_Tcutil_tvar_kind(
struct Cyc_Absyn_Tvar*t);void*Cyc_Tcutil_typ_kind(void*t);void*Cyc_Tcutil_compress(
void*t);void Cyc_Tcutil_unchecked_cast(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,
void*,void*);int Cyc_Tcutil_coerce_arg(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,
void*);int Cyc_Tcutil_coerce_assign(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,
void*);int Cyc_Tcutil_coerce_to_bool(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*);
int Cyc_Tcutil_coerce_list(struct Cyc_Tcenv_Tenv*,void*,struct Cyc_List_List*);int
Cyc_Tcutil_coerce_uint_typ(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*);int Cyc_Tcutil_coerce_sint_typ(
struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*);int Cyc_Tcutil_coerceable(void*);int
Cyc_Tcutil_silent_castable(struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,
void*,void*);void*Cyc_Tcutil_castable(struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,
void*,void*);int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*);int Cyc_Tcutil_is_numeric(
struct Cyc_Absyn_Exp*);int Cyc_Tcutil_is_pointer_type(void*t);int Cyc_Tcutil_is_zero(
struct Cyc_Absyn_Exp*e);int Cyc_Tcutil_is_pointer_or_boxed(void*t,int*
is_dyneither_ptr);extern struct Cyc_Core_Opt Cyc_Tcutil_rk;extern struct Cyc_Core_Opt
Cyc_Tcutil_trk;extern struct Cyc_Core_Opt Cyc_Tcutil_ak;extern struct Cyc_Core_Opt Cyc_Tcutil_bk;
extern struct Cyc_Core_Opt Cyc_Tcutil_mk;int Cyc_Tcutil_zero_to_null(struct Cyc_Tcenv_Tenv*,
void*t,struct Cyc_Absyn_Exp*e);void*Cyc_Tcutil_max_arithmetic_type(void*,void*);
void Cyc_Tcutil_explain_failure();int Cyc_Tcutil_unify(void*,void*);void*Cyc_Tcutil_rsubstitute(
struct _RegionHandle*,struct Cyc_List_List*,void*);void*Cyc_Tcutil_fndecl2typ(
struct Cyc_Absyn_Fndecl*);struct _tuple10{struct Cyc_List_List*f1;struct
_RegionHandle*f2;};struct _tuple11{struct Cyc_Absyn_Tvar*f1;void*f2;};struct
_tuple11*Cyc_Tcutil_r_make_inst_var(struct _tuple10*,struct Cyc_Absyn_Tvar*);void
Cyc_Tcutil_check_contains_assign(struct Cyc_Absyn_Exp*);void Cyc_Tcutil_check_type(
struct Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*
bound_tvars,void*k,int allow_evars,void*);void Cyc_Tcutil_check_nonzero_bound(
struct Cyc_Position_Segment*,union Cyc_Absyn_Constraint*);void Cyc_Tcutil_check_bound(
struct Cyc_Position_Segment*,unsigned int i,union Cyc_Absyn_Constraint*);int Cyc_Tcutil_is_bound_one(
union Cyc_Absyn_Constraint*b);struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(
struct _RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,
void*,struct Cyc_List_List*fields);int Cyc_Tcutil_is_tagged_pointer_typ(void*);int
Cyc_Tcutil_is_tagged_pointer_typ_elt(void*t,void**elt_typ_dest);int Cyc_Tcutil_is_zero_pointer_typ_elt(
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
path;};struct _union_RelnOp_EqualConst{int tag;unsigned int val;};struct _tuple14{
struct Cyc_Absyn_Vardecl*f1;void*f2;};struct _union_RelnOp_LessVar{int tag;struct
_tuple14 val;};struct _union_RelnOp_LessNumelts{int tag;struct Cyc_Absyn_Vardecl*val;
};struct _union_RelnOp_LessConst{int tag;unsigned int val;};struct
_union_RelnOp_LessEqNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};union Cyc_CfFlowInfo_RelnOp{
struct _union_RelnOp_EqualConst EqualConst;struct _union_RelnOp_LessVar LessVar;
struct _union_RelnOp_LessNumelts LessNumelts;struct _union_RelnOp_LessConst
LessConst;struct _union_RelnOp_LessEqNumelts LessEqNumelts;};struct Cyc_CfFlowInfo_Reln{
struct Cyc_Absyn_Vardecl*vd;union Cyc_CfFlowInfo_RelnOp rop;};struct Cyc_CfFlowInfo_TagCmp{
void*cmp;void*bd;};extern char Cyc_CfFlowInfo_HasTagCmps[15];struct Cyc_CfFlowInfo_HasTagCmps_struct{
char*tag;struct Cyc_List_List*f1;};extern char Cyc_CfFlowInfo_IsZero[11];extern char
Cyc_CfFlowInfo_NotZero[12];struct Cyc_CfFlowInfo_NotZero_struct{char*tag;struct
Cyc_List_List*f1;};extern char Cyc_CfFlowInfo_UnknownZ[13];struct Cyc_CfFlowInfo_UnknownZ_struct{
char*tag;struct Cyc_List_List*f1;};struct _union_AbsLVal_PlaceL{int tag;struct Cyc_CfFlowInfo_Place*
val;};struct _union_AbsLVal_UnknownL{int tag;int val;};union Cyc_CfFlowInfo_AbsLVal{
struct _union_AbsLVal_PlaceL PlaceL;struct _union_AbsLVal_UnknownL UnknownL;};struct
Cyc_CfFlowInfo_UnknownR_struct{int tag;void*f1;};struct Cyc_CfFlowInfo_Esc_struct{
int tag;void*f1;};struct Cyc_CfFlowInfo_AddressOf_struct{int tag;struct Cyc_CfFlowInfo_Place*
f1;};struct Cyc_CfFlowInfo_TagCmps_struct{int tag;struct Cyc_List_List*f1;};struct
Cyc_CfFlowInfo_Aggregate_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_CfFlowInfo_ConsumeInfo{
struct Cyc_Dict_Dict consumed;struct Cyc_List_List*may_consume;};struct
_union_FlowInfo_BottomFL{int tag;int val;};struct _tuple15{struct Cyc_Dict_Dict f1;
struct Cyc_List_List*f2;struct Cyc_CfFlowInfo_ConsumeInfo f3;};struct
_union_FlowInfo_ReachableFL{int tag;struct _tuple15 val;};union Cyc_CfFlowInfo_FlowInfo{
struct _union_FlowInfo_BottomFL BottomFL;struct _union_FlowInfo_ReachableFL
ReachableFL;};struct Cyc_CfFlowInfo_FlowEnv{struct _RegionHandle*r;void*
unknown_none;void*unknown_this;void*unknown_all;void*esc_none;void*esc_this;void*
esc_all;struct Cyc_Dict_Dict mt_flowdict;struct Cyc_Dict_Dict mt_place_set;struct Cyc_CfFlowInfo_Place*
dummy_place;};struct Cyc_CfFlowInfo_Region_k_struct{int tag;struct Cyc_Absyn_Tvar*
f1;};void Cyc_NewControlFlow_set_encloser(struct Cyc_Absyn_Stmt*enclosee,struct Cyc_Absyn_Stmt*
encloser);static void*Cyc_Tcexp_expr_err(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct _dyneither_ptr msg,struct _dyneither_ptr ap){({void*_tmp0=0;
Cyc_Tcutil_terr(loc,(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,msg,
ap),_tag_dyneither(_tmp0,sizeof(void*),0));});if(topt == 0)return Cyc_Absyn_wildtyp(
Cyc_Tcenv_lookup_opt_type_vars(te));else{return*topt;}}static void Cyc_Tcexp_resolve_unknown_id(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct _tuple1*q){struct
_handler_cons _tmp1;_push_handler(& _tmp1);{int _tmp3=0;if(setjmp(_tmp1.handler))
_tmp3=1;if(!_tmp3){{struct _RegionHandle*_tmp4=Cyc_Tcenv_get_fnrgn(te);void*_tmp5=
Cyc_Tcenv_lookup_ordinary(_tmp4,te,e->loc,q);void*_tmp6;struct Cyc_Absyn_Enumdecl*
_tmp7;struct Cyc_Absyn_Enumfield*_tmp8;void*_tmp9;struct Cyc_Absyn_Enumfield*_tmpA;
struct Cyc_Absyn_Datatypedecl*_tmpB;struct Cyc_Absyn_Datatypefield*_tmpC;_LL1: if(*((
int*)_tmp5)!= 0)goto _LL3;_tmp6=(void*)((struct Cyc_Tcenv_VarRes_struct*)_tmp5)->f1;
_LL2: e->r=(void*)({struct Cyc_Absyn_Var_e_struct*_tmpD=_cycalloc(sizeof(*_tmpD));
_tmpD[0]=({struct Cyc_Absyn_Var_e_struct _tmpE;_tmpE.tag=1;_tmpE.f1=q;_tmpE.f2=(
void*)_tmp6;_tmpE;});_tmpD;});goto _LL0;_LL3: if(*((int*)_tmp5)!= 3)goto _LL5;_tmp7=((
struct Cyc_Tcenv_EnumRes_struct*)_tmp5)->f1;_tmp8=((struct Cyc_Tcenv_EnumRes_struct*)
_tmp5)->f2;_LL4: e->r=(void*)({struct Cyc_Absyn_Enum_e_struct*_tmpF=_cycalloc(
sizeof(*_tmpF));_tmpF[0]=({struct Cyc_Absyn_Enum_e_struct _tmp10;_tmp10.tag=33;
_tmp10.f1=q;_tmp10.f2=(struct Cyc_Absyn_Enumdecl*)_tmp7;_tmp10.f3=(struct Cyc_Absyn_Enumfield*)
_tmp8;_tmp10;});_tmpF;});goto _LL0;_LL5: if(*((int*)_tmp5)!= 4)goto _LL7;_tmp9=(
void*)((struct Cyc_Tcenv_AnonEnumRes_struct*)_tmp5)->f1;_tmpA=((struct Cyc_Tcenv_AnonEnumRes_struct*)
_tmp5)->f2;_LL6: e->r=(void*)({struct Cyc_Absyn_AnonEnum_e_struct*_tmp11=_cycalloc(
sizeof(*_tmp11));_tmp11[0]=({struct Cyc_Absyn_AnonEnum_e_struct _tmp12;_tmp12.tag=
34;_tmp12.f1=q;_tmp12.f2=(void*)_tmp9;_tmp12.f3=(struct Cyc_Absyn_Enumfield*)
_tmpA;_tmp12;});_tmp11;});goto _LL0;_LL7: if(*((int*)_tmp5)!= 2)goto _LL9;_tmpB=((
struct Cyc_Tcenv_DatatypeRes_struct*)_tmp5)->f1;_tmpC=((struct Cyc_Tcenv_DatatypeRes_struct*)
_tmp5)->f2;_LL8: e->r=(void*)({struct Cyc_Absyn_Datatype_e_struct*_tmp13=_cycalloc(
sizeof(*_tmp13));_tmp13[0]=({struct Cyc_Absyn_Datatype_e_struct _tmp14;_tmp14.tag=
32;_tmp14.f1=0;_tmp14.f2=_tmpB;_tmp14.f3=_tmpC;_tmp14;});_tmp13;});goto _LL0;_LL9:
if(*((int*)_tmp5)!= 1)goto _LL0;_LLA:({struct Cyc_String_pa_struct _tmp17;_tmp17.tag=
0;_tmp17.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
q));{void*_tmp15[1]={& _tmp17};Cyc_Tcutil_terr(e->loc,({const char*_tmp16="bad occurrence of type name %s";
_tag_dyneither(_tmp16,sizeof(char),31);}),_tag_dyneither(_tmp15,sizeof(void*),1));}});
e->r=(void*)({struct Cyc_Absyn_Var_e_struct*_tmp18=_cycalloc(sizeof(*_tmp18));
_tmp18[0]=({struct Cyc_Absyn_Var_e_struct _tmp19;_tmp19.tag=1;_tmp19.f1=q;_tmp19.f2=(
void*)((void*)0);_tmp19;});_tmp18;});goto _LL0;_LL0:;};_pop_handler();}else{void*
_tmp2=(void*)_exn_thrown;void*_tmp1B=_tmp2;_LLC: if(_tmp1B != Cyc_Dict_Absent)goto
_LLE;_LLD: e->r=(void*)({struct Cyc_Absyn_Var_e_struct*_tmp1C=_cycalloc(sizeof(*
_tmp1C));_tmp1C[0]=({struct Cyc_Absyn_Var_e_struct _tmp1D;_tmp1D.tag=1;_tmp1D.f1=q;
_tmp1D.f2=(void*)((void*)0);_tmp1D;});_tmp1C;});goto _LLB;_LLE:;_LLF:(void)_throw(
_tmp1B);_LLB:;}}}struct _tuple16{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
static struct _tuple16*Cyc_Tcexp_make_struct_arg(struct Cyc_Absyn_Exp*e){return({
struct _tuple16*_tmp1E=_cycalloc(sizeof(*_tmp1E));_tmp1E->f1=0;_tmp1E->f2=e;
_tmp1E;});}static void Cyc_Tcexp_resolve_unknown_fn(struct Cyc_Tcenv_Tenv*te,struct
Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*es){void*_tmp1F=e1->r;
struct _tuple1*_tmp20;_LL11: if(*((int*)_tmp1F)!= 2)goto _LL13;_tmp20=((struct Cyc_Absyn_UnknownId_e_struct*)
_tmp1F)->f1;_LL12: {struct _handler_cons _tmp21;_push_handler(& _tmp21);{int _tmp23=
0;if(setjmp(_tmp21.handler))_tmp23=1;if(!_tmp23){{struct _RegionHandle*_tmp24=Cyc_Tcenv_get_fnrgn(
te);void*_tmp25=Cyc_Tcenv_lookup_ordinary(_tmp24,te,e1->loc,_tmp20);void*_tmp26;
struct Cyc_Absyn_Datatypedecl*_tmp27;struct Cyc_Absyn_Datatypefield*_tmp28;struct
Cyc_Absyn_Aggrdecl*_tmp29;_LL16: if(*((int*)_tmp25)!= 0)goto _LL18;_tmp26=(void*)((
struct Cyc_Tcenv_VarRes_struct*)_tmp25)->f1;_LL17: e->r=(void*)({struct Cyc_Absyn_FnCall_e_struct*
_tmp2A=_cycalloc(sizeof(*_tmp2A));_tmp2A[0]=({struct Cyc_Absyn_FnCall_e_struct
_tmp2B;_tmp2B.tag=11;_tmp2B.f1=e1;_tmp2B.f2=es;_tmp2B.f3=0;_tmp2B;});_tmp2A;});
_npop_handler(0);return;_LL18: if(*((int*)_tmp25)!= 2)goto _LL1A;_tmp27=((struct
Cyc_Tcenv_DatatypeRes_struct*)_tmp25)->f1;_tmp28=((struct Cyc_Tcenv_DatatypeRes_struct*)
_tmp25)->f2;_LL19: if(_tmp28->typs == 0)({struct Cyc_String_pa_struct _tmp2E;_tmp2E.tag=
0;_tmp2E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp28->name));{void*_tmp2C[1]={& _tmp2E};Cyc_Tcutil_terr(e->loc,({const char*
_tmp2D="%s is a constant, not a function";_tag_dyneither(_tmp2D,sizeof(char),33);}),
_tag_dyneither(_tmp2C,sizeof(void*),1));}});e->r=(void*)({struct Cyc_Absyn_Datatype_e_struct*
_tmp2F=_cycalloc(sizeof(*_tmp2F));_tmp2F[0]=({struct Cyc_Absyn_Datatype_e_struct
_tmp30;_tmp30.tag=32;_tmp30.f1=es;_tmp30.f2=_tmp27;_tmp30.f3=_tmp28;_tmp30;});
_tmp2F;});_npop_handler(0);return;_LL1A: if(*((int*)_tmp25)!= 1)goto _LL1C;_tmp29=((
struct Cyc_Tcenv_AggrRes_struct*)_tmp25)->f1;_LL1B: {struct Cyc_List_List*_tmp31=((
struct Cyc_List_List*(*)(struct _tuple16*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Tcexp_make_struct_arg,es);e->r=(void*)({struct Cyc_Absyn_Aggregate_e_struct*
_tmp32=_cycalloc(sizeof(*_tmp32));_tmp32[0]=({struct Cyc_Absyn_Aggregate_e_struct
_tmp33;_tmp33.tag=30;_tmp33.f1=_tmp29->name;_tmp33.f2=0;_tmp33.f3=_tmp31;_tmp33.f4=(
struct Cyc_Absyn_Aggrdecl*)_tmp29;_tmp33;});_tmp32;});_npop_handler(0);return;}
_LL1C: if(*((int*)_tmp25)!= 4)goto _LL1E;_LL1D: goto _LL1F;_LL1E: if(*((int*)_tmp25)
!= 3)goto _LL15;_LL1F:({struct Cyc_String_pa_struct _tmp36;_tmp36.tag=0;_tmp36.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp20));{
void*_tmp34[1]={& _tmp36};Cyc_Tcutil_terr(e->loc,({const char*_tmp35="%s is an enum constructor, not a function";
_tag_dyneither(_tmp35,sizeof(char),42);}),_tag_dyneither(_tmp34,sizeof(void*),1));}});
_npop_handler(0);return;_LL15:;};_pop_handler();}else{void*_tmp22=(void*)
_exn_thrown;void*_tmp38=_tmp22;_LL21: if(_tmp38 != Cyc_Dict_Absent)goto _LL23;_LL22:
e->r=(void*)({struct Cyc_Absyn_FnCall_e_struct*_tmp39=_cycalloc(sizeof(*_tmp39));
_tmp39[0]=({struct Cyc_Absyn_FnCall_e_struct _tmp3A;_tmp3A.tag=11;_tmp3A.f1=e1;
_tmp3A.f2=es;_tmp3A.f3=0;_tmp3A;});_tmp39;});return;_LL23:;_LL24:(void)_throw(
_tmp38);_LL20:;}}}_LL13:;_LL14: e->r=(void*)({struct Cyc_Absyn_FnCall_e_struct*
_tmp3B=_cycalloc(sizeof(*_tmp3B));_tmp3B[0]=({struct Cyc_Absyn_FnCall_e_struct
_tmp3C;_tmp3C.tag=11;_tmp3C.f1=e1;_tmp3C.f2=es;_tmp3C.f3=0;_tmp3C;});_tmp3B;});
return;_LL10:;}static void Cyc_Tcexp_resolve_unresolved_mem(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*
des){if(topt == 0){e->r=(void*)({struct Cyc_Absyn_Array_e_struct*_tmp3D=_cycalloc(
sizeof(*_tmp3D));_tmp3D[0]=({struct Cyc_Absyn_Array_e_struct _tmp3E;_tmp3E.tag=28;
_tmp3E.f1=des;_tmp3E;});_tmp3D;});return;}{void*t=*topt;void*_tmp3F=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_AggrInfo _tmp40;union Cyc_Absyn_AggrInfoU _tmp41;struct Cyc_Absyn_ArrayInfo
_tmp42;void*_tmp43;struct Cyc_Absyn_Tqual _tmp44;_LL26: if(_tmp3F <= (void*)4)goto
_LL2C;if(*((int*)_tmp3F)!= 10)goto _LL28;_tmp40=((struct Cyc_Absyn_AggrType_struct*)
_tmp3F)->f1;_tmp41=_tmp40.aggr_info;_LL27:{union Cyc_Absyn_AggrInfoU _tmp45=_tmp41;
struct _tuple3 _tmp46;struct Cyc_Absyn_Aggrdecl**_tmp47;struct Cyc_Absyn_Aggrdecl*
_tmp48;_LL2F: if((_tmp45.UnknownAggr).tag != 1)goto _LL31;_tmp46=(struct _tuple3)(
_tmp45.UnknownAggr).val;_LL30:({void*_tmp49=0;((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp4A="struct type not properly set";
_tag_dyneither(_tmp4A,sizeof(char),29);}),_tag_dyneither(_tmp49,sizeof(void*),0));});
_LL31: if((_tmp45.KnownAggr).tag != 2)goto _LL2E;_tmp47=(struct Cyc_Absyn_Aggrdecl**)(
_tmp45.KnownAggr).val;_tmp48=*_tmp47;_LL32: e->r=(void*)({struct Cyc_Absyn_Aggregate_e_struct*
_tmp4B=_cycalloc(sizeof(*_tmp4B));_tmp4B[0]=({struct Cyc_Absyn_Aggregate_e_struct
_tmp4C;_tmp4C.tag=30;_tmp4C.f1=_tmp48->name;_tmp4C.f2=0;_tmp4C.f3=des;_tmp4C.f4=(
struct Cyc_Absyn_Aggrdecl*)_tmp48;_tmp4C;});_tmp4B;});_LL2E:;}goto _LL25;_LL28: if(*((
int*)_tmp3F)!= 7)goto _LL2A;_tmp42=((struct Cyc_Absyn_ArrayType_struct*)_tmp3F)->f1;
_tmp43=_tmp42.elt_type;_tmp44=_tmp42.tq;_LL29: e->r=(void*)({struct Cyc_Absyn_Array_e_struct*
_tmp4D=_cycalloc(sizeof(*_tmp4D));_tmp4D[0]=({struct Cyc_Absyn_Array_e_struct
_tmp4E;_tmp4E.tag=28;_tmp4E.f1=des;_tmp4E;});_tmp4D;});goto _LL25;_LL2A: if(*((int*)
_tmp3F)!= 11)goto _LL2C;_LL2B: e->r=(void*)({struct Cyc_Absyn_AnonStruct_e_struct*
_tmp4F=_cycalloc(sizeof(*_tmp4F));_tmp4F[0]=({struct Cyc_Absyn_AnonStruct_e_struct
_tmp50;_tmp50.tag=31;_tmp50.f1=(void*)t;_tmp50.f2=des;_tmp50;});_tmp4F;});goto
_LL25;_LL2C:;_LL2D: e->r=(void*)({struct Cyc_Absyn_Array_e_struct*_tmp51=_cycalloc(
sizeof(*_tmp51));_tmp51[0]=({struct Cyc_Absyn_Array_e_struct _tmp52;_tmp52.tag=28;
_tmp52.f1=des;_tmp52;});_tmp51;});goto _LL25;_LL25:;}}static void Cyc_Tcexp_tcExpNoInst(
struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e);static void*Cyc_Tcexp_tcExpNoPromote(
struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e);static void Cyc_Tcexp_tcExpList(
struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*es){for(0;es != 0;es=es->tl){Cyc_Tcexp_tcExp(
te,0,(struct Cyc_Absyn_Exp*)es->hd);}}struct Cyc_Tcexp_TestEnv Cyc_Tcexp_tcTest(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr msg_part){Cyc_Tcutil_check_contains_assign(
e);Cyc_Tcexp_tcExp(te,(void**)& Cyc_Absyn_sint_typ,e);if(!Cyc_Tcutil_coerce_to_bool(
te,e))({struct Cyc_String_pa_struct _tmp56;_tmp56.tag=0;_tmp56.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v));{struct Cyc_String_pa_struct _tmp55;_tmp55.tag=0;_tmp55.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)msg_part);{void*_tmp53[2]={& _tmp55,&
_tmp56};Cyc_Tcutil_terr(e->loc,({const char*_tmp54="test of %s has type %s instead of integral or * type";
_tag_dyneither(_tmp54,sizeof(char),53);}),_tag_dyneither(_tmp53,sizeof(void*),2));}}});{
void*_tmp57=e->r;void*_tmp58;struct Cyc_List_List*_tmp59;_LL34: if(*((int*)_tmp57)
!= 3)goto _LL36;_tmp58=(void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp57)->f1;
_tmp59=((struct Cyc_Absyn_Primop_e_struct*)_tmp57)->f2;_LL35: if(_tmp58 == (void*)5
 || _tmp58 == (void*)6){struct _tuple0 _tmp5B=({struct _tuple0 _tmp5A;_tmp5A.f1=(void*)((
struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp59))->hd)->topt))->v;_tmp5A.f2=(void*)((struct Cyc_Core_Opt*)
_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(((struct
Cyc_List_List*)_check_null(_tmp59))->tl))->hd)->topt))->v;_tmp5A;});void*_tmp5C;
void*_tmp5D;void*_tmp5E;void*_tmp5F;_LL39: _tmp5C=_tmp5B.f1;if(_tmp5C <= (void*)4)
goto _LL3B;if(*((int*)_tmp5C)!= 14)goto _LL3B;_tmp5D=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp5C)->f1;_tmp5E=_tmp5B.f2;if(_tmp5E <= (void*)4)goto _LL3B;if(*((int*)_tmp5E)!= 
14)goto _LL3B;_tmp5F=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)_tmp5E)->f1;
_LL3A: return({struct Cyc_Tcexp_TestEnv _tmp60;_tmp60.eq=({struct _tuple0*_tmp61=
_region_malloc(Cyc_Tcenv_get_fnrgn(te),sizeof(*_tmp61));_tmp61->f1=_tmp5D;_tmp61->f2=
_tmp5F;_tmp61;});_tmp60.isTrue=_tmp58 == (void*)5;_tmp60;});_LL3B:;_LL3C: goto
_LL38;_LL38:;}goto _LL33;_LL36:;_LL37: goto _LL33;_LL33:;}return({struct Cyc_Tcexp_TestEnv
_tmp62;_tmp62.eq=0;_tmp62.isTrue=0;_tmp62;});}static void*Cyc_Tcexp_tcConst(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,union Cyc_Absyn_Cnst*
c,struct Cyc_Absyn_Exp*e){void*t;{union Cyc_Absyn_Cnst _tmp63=*((union Cyc_Absyn_Cnst*)
_check_null(c));struct _tuple4 _tmp64;void*_tmp65;struct _tuple4 _tmp66;void*_tmp67;
struct _tuple4 _tmp68;void*_tmp69;struct _tuple5 _tmp6A;void*_tmp6B;struct _tuple7
_tmp6C;void*_tmp6D;struct _dyneither_ptr _tmp6E;struct _tuple6 _tmp6F;void*_tmp70;
int _tmp71;struct _dyneither_ptr _tmp72;int _tmp73;_LL3E: if((_tmp63.Char_c).tag != 2)
goto _LL40;_tmp64=(struct _tuple4)(_tmp63.Char_c).val;_tmp65=_tmp64.f1;if((int)
_tmp65 != 0)goto _LL40;_LL3F: t=Cyc_Absyn_schar_typ;goto _LL3D;_LL40: if((_tmp63.Char_c).tag
!= 2)goto _LL42;_tmp66=(struct _tuple4)(_tmp63.Char_c).val;_tmp67=_tmp66.f1;if((
int)_tmp67 != 1)goto _LL42;_LL41: t=Cyc_Absyn_uchar_typ;goto _LL3D;_LL42: if((_tmp63.Char_c).tag
!= 2)goto _LL44;_tmp68=(struct _tuple4)(_tmp63.Char_c).val;_tmp69=_tmp68.f1;if((
int)_tmp69 != 2)goto _LL44;_LL43: t=Cyc_Absyn_char_typ;goto _LL3D;_LL44: if((_tmp63.Short_c).tag
!= 3)goto _LL46;_tmp6A=(struct _tuple5)(_tmp63.Short_c).val;_tmp6B=_tmp6A.f1;_LL45:
t=_tmp6B == (void*)1?Cyc_Absyn_ushort_typ: Cyc_Absyn_sshort_typ;goto _LL3D;_LL46:
if((_tmp63.LongLong_c).tag != 5)goto _LL48;_tmp6C=(struct _tuple7)(_tmp63.LongLong_c).val;
_tmp6D=_tmp6C.f1;_LL47: t=_tmp6D == (void*)1?Cyc_Absyn_ulonglong_typ: Cyc_Absyn_slonglong_typ;
goto _LL3D;_LL48: if((_tmp63.Float_c).tag != 6)goto _LL4A;_tmp6E=(struct
_dyneither_ptr)(_tmp63.Float_c).val;_LL49: t=Cyc_Absyn_float_typ;goto _LL3D;_LL4A:
if((_tmp63.Int_c).tag != 4)goto _LL4C;_tmp6F=(struct _tuple6)(_tmp63.Int_c).val;
_tmp70=_tmp6F.f1;_tmp71=_tmp6F.f2;_LL4B: if(topt == 0)t=_tmp70 == (void*)1?Cyc_Absyn_uint_typ:
Cyc_Absyn_sint_typ;else{void*_tmp74=Cyc_Tcutil_compress(*topt);void*_tmp75;void*
_tmp76;void*_tmp77;void*_tmp78;void*_tmp79;void*_tmp7A;void*_tmp7B;void*_tmp7C;
void*_tmp7D;_LL51: if(_tmp74 <= (void*)4)goto _LL5D;if(*((int*)_tmp74)!= 5)goto
_LL53;_tmp75=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp74)->f1;_tmp76=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp74)->f2;if((int)_tmp76 != 0)goto _LL53;_LL52:{
void*_tmp7E=_tmp75;_LL60: if((int)_tmp7E != 1)goto _LL62;_LL61: t=Cyc_Absyn_uchar_typ;
goto _LL5F;_LL62: if((int)_tmp7E != 0)goto _LL64;_LL63: t=Cyc_Absyn_schar_typ;goto
_LL5F;_LL64: if((int)_tmp7E != 2)goto _LL5F;_LL65: t=Cyc_Absyn_char_typ;goto _LL5F;
_LL5F:;}*c=Cyc_Absyn_Char_c(_tmp75,(char)_tmp71);goto _LL50;_LL53: if(*((int*)
_tmp74)!= 5)goto _LL55;_tmp77=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp74)->f1;
_tmp78=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp74)->f2;if((int)_tmp78 != 1)
goto _LL55;_LL54: t=_tmp77 == (void*)1?Cyc_Absyn_ushort_typ: Cyc_Absyn_sshort_typ;*c=
Cyc_Absyn_Short_c(_tmp77,(short)_tmp71);goto _LL50;_LL55: if(*((int*)_tmp74)!= 5)
goto _LL57;_tmp79=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp74)->f1;_tmp7A=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp74)->f2;if((int)_tmp7A != 2)goto _LL57;
_LL56: t=_tmp79 == (void*)1?Cyc_Absyn_uint_typ: Cyc_Absyn_sint_typ;goto _LL50;_LL57:
if(*((int*)_tmp74)!= 5)goto _LL59;_tmp7B=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp74)->f1;_tmp7C=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp74)->f2;if((int)
_tmp7C != 3)goto _LL59;_LL58: t=_tmp7B == (void*)1?Cyc_Absyn_uint_typ: Cyc_Absyn_sint_typ;
goto _LL50;_LL59: if(*((int*)_tmp74)!= 4)goto _LL5B;if(!(_tmp71 == 0))goto _LL5B;
_LL5A: e->r=(void*)({struct Cyc_Absyn_Const_e_struct*_tmp7F=_cycalloc(sizeof(*
_tmp7F));_tmp7F[0]=({struct Cyc_Absyn_Const_e_struct _tmp80;_tmp80.tag=0;_tmp80.f1=
Cyc_Absyn_Null_c;_tmp80;});_tmp7F;});{struct Cyc_List_List*_tmp81=Cyc_Tcenv_lookup_type_vars(
te);t=(void*)({struct Cyc_Absyn_PointerType_struct*_tmp82=_cycalloc(sizeof(*
_tmp82));_tmp82[0]=({struct Cyc_Absyn_PointerType_struct _tmp83;_tmp83.tag=4;
_tmp83.f1=({struct Cyc_Absyn_PtrInfo _tmp84;_tmp84.elt_typ=Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_ak,({struct Cyc_Core_Opt*_tmp87=_cycalloc(sizeof(*
_tmp87));_tmp87->v=_tmp81;_tmp87;}));_tmp84.elt_tq=Cyc_Absyn_empty_tqual(0);
_tmp84.ptr_atts=({struct Cyc_Absyn_PtrAtts _tmp85;_tmp85.rgn=Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_trk,({struct Cyc_Core_Opt*_tmp86=_cycalloc(
sizeof(*_tmp86));_tmp86->v=_tmp81;_tmp86;}));_tmp85.nullable=Cyc_Absyn_true_conref;
_tmp85.bounds=Cyc_Absyn_empty_conref();_tmp85.zero_term=((union Cyc_Absyn_Constraint*(*)())
Cyc_Absyn_empty_conref)();_tmp85.ptrloc=0;_tmp85;});_tmp84;});_tmp83;});_tmp82;});
goto _LL50;}_LL5B: if(*((int*)_tmp74)!= 18)goto _LL5D;_tmp7D=(void*)((struct Cyc_Absyn_TagType_struct*)
_tmp74)->f1;_LL5C: {struct Cyc_Absyn_ValueofType_struct*_tmp88=({struct Cyc_Absyn_ValueofType_struct*
_tmp8F=_cycalloc(sizeof(*_tmp8F));_tmp8F[0]=({struct Cyc_Absyn_ValueofType_struct
_tmp90;_tmp90.tag=17;_tmp90.f1=Cyc_Absyn_uint_exp((unsigned int)_tmp71,0);_tmp90;});
_tmp8F;});if(!Cyc_Tcutil_unify(_tmp7D,(void*)_tmp88)){({struct Cyc_String_pa_struct
_tmp8C;_tmp8C.tag=0;_tmp8C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)_tmp88));{struct Cyc_String_pa_struct _tmp8B;_tmp8B.tag=0;_tmp8B.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp7D));{void*
_tmp89[2]={& _tmp8B,& _tmp8C};Cyc_Tcutil_terr(loc,({const char*_tmp8A="expecting %s but found %s";
_tag_dyneither(_tmp8A,sizeof(char),26);}),_tag_dyneither(_tmp89,sizeof(void*),2));}}});
Cyc_Tcutil_explain_failure();}t=(void*)({struct Cyc_Absyn_TagType_struct*_tmp8D=
_cycalloc(sizeof(*_tmp8D));_tmp8D[0]=({struct Cyc_Absyn_TagType_struct _tmp8E;
_tmp8E.tag=18;_tmp8E.f1=(void*)((void*)_tmp88);_tmp8E;});_tmp8D;});goto _LL50;}
_LL5D:;_LL5E: t=_tmp70 == (void*)1?Cyc_Absyn_uint_typ: Cyc_Absyn_sint_typ;goto _LL50;
_LL50:;}goto _LL3D;_LL4C: if((_tmp63.String_c).tag != 7)goto _LL4E;_tmp72=(struct
_dyneither_ptr)(_tmp63.String_c).val;_LL4D: {int len=(int)_get_dyneither_size(
_tmp72,sizeof(char));struct Cyc_Absyn_Exp*elen=Cyc_Absyn_const_exp(Cyc_Absyn_Int_c((
void*)1,len),loc);elen->topt=({struct Cyc_Core_Opt*_tmp91=_cycalloc(sizeof(*
_tmp91));_tmp91->v=(void*)Cyc_Absyn_uint_typ;_tmp91;});t=Cyc_Absyn_atb_typ(Cyc_Absyn_char_typ,(
void*)2,Cyc_Absyn_const_tqual(0),(void*)({struct Cyc_Absyn_Upper_b_struct*_tmp92=
_cycalloc(sizeof(*_tmp92));_tmp92[0]=({struct Cyc_Absyn_Upper_b_struct _tmp93;
_tmp93.tag=0;_tmp93.f1=elen;_tmp93;});_tmp92;}),Cyc_Absyn_true_conref);if(topt != 
0){void*_tmp94=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_ArrayInfo _tmp95;struct
Cyc_Absyn_Tqual _tmp96;_LL67: if(_tmp94 <= (void*)4)goto _LL6B;if(*((int*)_tmp94)!= 
7)goto _LL69;_tmp95=((struct Cyc_Absyn_ArrayType_struct*)_tmp94)->f1;_tmp96=_tmp95.tq;
_LL68: return Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,_tmp96,(struct Cyc_Absyn_Exp*)
elen,((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),0);_LL69: if(*((
int*)_tmp94)!= 4)goto _LL6B;_LL6A: if(!Cyc_Tcutil_unify(*topt,t) && Cyc_Tcutil_silent_castable(
te,loc,t,*topt)){e->topt=({struct Cyc_Core_Opt*_tmp97=_cycalloc(sizeof(*_tmp97));
_tmp97->v=(void*)t;_tmp97;});Cyc_Tcutil_unchecked_cast(te,e,*topt,(void*)3);t=*
topt;}else{t=Cyc_Absyn_atb_typ(Cyc_Absyn_char_typ,Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_rk,Cyc_Tcenv_lookup_opt_type_vars(te)),Cyc_Absyn_const_tqual(0),(void*)({
struct Cyc_Absyn_Upper_b_struct*_tmp98=_cycalloc(sizeof(*_tmp98));_tmp98[0]=({
struct Cyc_Absyn_Upper_b_struct _tmp99;_tmp99.tag=0;_tmp99.f1=elen;_tmp99;});
_tmp98;}),Cyc_Absyn_true_conref);if(!Cyc_Tcutil_unify(*topt,t) && Cyc_Tcutil_silent_castable(
te,loc,t,*topt)){e->topt=({struct Cyc_Core_Opt*_tmp9A=_cycalloc(sizeof(*_tmp9A));
_tmp9A->v=(void*)t;_tmp9A;});Cyc_Tcutil_unchecked_cast(te,e,*topt,(void*)3);t=*
topt;}}goto _LL66;_LL6B:;_LL6C: goto _LL66;_LL66:;}return t;}_LL4E: if((_tmp63.Null_c).tag
!= 1)goto _LL3D;_tmp73=(int)(_tmp63.Null_c).val;_LL4F: {struct Cyc_List_List*
_tmp9B=Cyc_Tcenv_lookup_type_vars(te);t=(void*)({struct Cyc_Absyn_PointerType_struct*
_tmp9C=_cycalloc(sizeof(*_tmp9C));_tmp9C[0]=({struct Cyc_Absyn_PointerType_struct
_tmp9D;_tmp9D.tag=4;_tmp9D.f1=({struct Cyc_Absyn_PtrInfo _tmp9E;_tmp9E.elt_typ=Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_ak,({struct Cyc_Core_Opt*_tmpA1=_cycalloc(sizeof(*
_tmpA1));_tmpA1->v=_tmp9B;_tmpA1;}));_tmp9E.elt_tq=Cyc_Absyn_empty_tqual(0);
_tmp9E.ptr_atts=({struct Cyc_Absyn_PtrAtts _tmp9F;_tmp9F.rgn=Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_trk,({struct Cyc_Core_Opt*_tmpA0=_cycalloc(
sizeof(*_tmpA0));_tmpA0->v=_tmp9B;_tmpA0;}));_tmp9F.nullable=Cyc_Absyn_true_conref;
_tmp9F.bounds=Cyc_Absyn_empty_conref();_tmp9F.zero_term=((union Cyc_Absyn_Constraint*(*)())
Cyc_Absyn_empty_conref)();_tmp9F.ptrloc=0;_tmp9F;});_tmp9E;});_tmp9D;});_tmp9C;});
goto _LL3D;}_LL3D:;}return t;}static void*Cyc_Tcexp_tcVar(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,struct _tuple1*q,void*b){void*_tmpA2=b;struct Cyc_Absyn_Vardecl*
_tmpA3;struct Cyc_Absyn_Fndecl*_tmpA4;struct Cyc_Absyn_Vardecl*_tmpA5;struct Cyc_Absyn_Vardecl*
_tmpA6;struct Cyc_Absyn_Vardecl*_tmpA7;_LL6E: if((int)_tmpA2 != 0)goto _LL70;_LL6F:
return({struct Cyc_String_pa_struct _tmpAA;_tmpAA.tag=0;_tmpAA.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(q));{void*_tmpA8[1]={&
_tmpAA};Cyc_Tcexp_expr_err(te,loc,0,({const char*_tmpA9="undeclared identifier: %s";
_tag_dyneither(_tmpA9,sizeof(char),26);}),_tag_dyneither(_tmpA8,sizeof(void*),1));}});
_LL70: if(_tmpA2 <= (void*)1)goto _LL72;if(*((int*)_tmpA2)!= 0)goto _LL72;_tmpA3=((
struct Cyc_Absyn_Global_b_struct*)_tmpA2)->f1;_LL71:*q=*_tmpA3->name;return _tmpA3->type;
_LL72: if(_tmpA2 <= (void*)1)goto _LL74;if(*((int*)_tmpA2)!= 1)goto _LL74;_tmpA4=((
struct Cyc_Absyn_Funname_b_struct*)_tmpA2)->f1;_LL73:*q=*_tmpA4->name;return Cyc_Tcutil_fndecl2typ(
_tmpA4);_LL74: if(_tmpA2 <= (void*)1)goto _LL76;if(*((int*)_tmpA2)!= 4)goto _LL76;
_tmpA5=((struct Cyc_Absyn_Pat_b_struct*)_tmpA2)->f1;_LL75: _tmpA6=_tmpA5;goto _LL77;
_LL76: if(_tmpA2 <= (void*)1)goto _LL78;if(*((int*)_tmpA2)!= 3)goto _LL78;_tmpA6=((
struct Cyc_Absyn_Local_b_struct*)_tmpA2)->f1;_LL77: _tmpA7=_tmpA6;goto _LL79;_LL78:
if(_tmpA2 <= (void*)1)goto _LL6D;if(*((int*)_tmpA2)!= 2)goto _LL6D;_tmpA7=((struct
Cyc_Absyn_Param_b_struct*)_tmpA2)->f1;_LL79:(*q).f1=({union Cyc_Absyn_Nmspace
_tmpAB;(_tmpAB.Loc_n).val=0;(_tmpAB.Loc_n).tag=3;_tmpAB;});return _tmpA7->type;
_LL6D:;}static void Cyc_Tcexp_check_format_args(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*
fmt,struct Cyc_Core_Opt*opt_args,struct _RegionHandle*temp,struct Cyc_List_List*(*
type_getter)(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,struct _dyneither_ptr,
struct Cyc_Position_Segment*)){struct Cyc_List_List*desc_types;{void*_tmpAC=fmt->r;
union Cyc_Absyn_Cnst _tmpAD;struct _dyneither_ptr _tmpAE;struct Cyc_Absyn_Exp*_tmpAF;
struct Cyc_Absyn_Exp _tmpB0;void*_tmpB1;union Cyc_Absyn_Cnst _tmpB2;struct
_dyneither_ptr _tmpB3;_LL7B: if(*((int*)_tmpAC)!= 0)goto _LL7D;_tmpAD=((struct Cyc_Absyn_Const_e_struct*)
_tmpAC)->f1;if((_tmpAD.String_c).tag != 7)goto _LL7D;_tmpAE=(struct _dyneither_ptr)(
_tmpAD.String_c).val;_LL7C: _tmpB3=_tmpAE;goto _LL7E;_LL7D: if(*((int*)_tmpAC)!= 15)
goto _LL7F;_tmpAF=((struct Cyc_Absyn_Cast_e_struct*)_tmpAC)->f2;_tmpB0=*_tmpAF;
_tmpB1=_tmpB0.r;if(*((int*)_tmpB1)!= 0)goto _LL7F;_tmpB2=((struct Cyc_Absyn_Const_e_struct*)
_tmpB1)->f1;if((_tmpB2.String_c).tag != 7)goto _LL7F;_tmpB3=(struct _dyneither_ptr)(
_tmpB2.String_c).val;_LL7E: desc_types=type_getter(temp,te,(struct _dyneither_ptr)
_tmpB3,fmt->loc);goto _LL7A;_LL7F:;_LL80: if(opt_args != 0){struct Cyc_List_List*
_tmpB4=(struct Cyc_List_List*)opt_args->v;for(0;_tmpB4 != 0;_tmpB4=_tmpB4->tl){Cyc_Tcexp_tcExp(
te,0,(struct Cyc_Absyn_Exp*)_tmpB4->hd);{struct _RegionHandle*_tmpB5=Cyc_Tcenv_get_fnrgn(
te);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(_tmpB5,(void*)((struct Cyc_Core_Opt*)
_check_null(((struct Cyc_Absyn_Exp*)_tmpB4->hd)->topt))->v) && !Cyc_Tcutil_is_noalias_path(
_tmpB5,(struct Cyc_Absyn_Exp*)_tmpB4->hd))({void*_tmpB6=0;Cyc_Tcutil_terr(((
struct Cyc_Absyn_Exp*)_tmpB4->hd)->loc,({const char*_tmpB7="Cannot consume non-unique paths; do swap instead";
_tag_dyneither(_tmpB7,sizeof(char),49);}),_tag_dyneither(_tmpB6,sizeof(void*),0));});}}}
return;_LL7A:;}if(opt_args != 0){struct Cyc_List_List*_tmpB8=(struct Cyc_List_List*)
opt_args->v;for(0;desc_types != 0  && _tmpB8 != 0;(desc_types=desc_types->tl,_tmpB8=
_tmpB8->tl)){void*t=(void*)desc_types->hd;struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)
_tmpB8->hd;Cyc_Tcexp_tcExp(te,(void**)& t,e);if(!Cyc_Tcutil_coerce_arg(te,e,t)){({
struct Cyc_String_pa_struct _tmpBC;_tmpBC.tag=0;_tmpBC.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v));{struct Cyc_String_pa_struct _tmpBB;_tmpBB.tag=0;_tmpBB.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));{void*
_tmpB9[2]={& _tmpBB,& _tmpBC};Cyc_Tcutil_terr(e->loc,({const char*_tmpBA="descriptor has type \n%s\n but argument has type \n%s";
_tag_dyneither(_tmpBA,sizeof(char),51);}),_tag_dyneither(_tmpB9,sizeof(void*),2));}}});
Cyc_Tcutil_explain_failure();}{struct _RegionHandle*_tmpBD=Cyc_Tcenv_get_fnrgn(te);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(_tmpBD,t) && !Cyc_Tcutil_is_noalias_path(
_tmpBD,e))({void*_tmpBE=0;Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_tmpB8->hd)->loc,({
const char*_tmpBF="Cannot consume non-unique paths; do swap instead";
_tag_dyneither(_tmpBF,sizeof(char),49);}),_tag_dyneither(_tmpBE,sizeof(void*),0));});}}
if(desc_types != 0)({void*_tmpC0=0;Cyc_Tcutil_terr(fmt->loc,({const char*_tmpC1="too few arguments";
_tag_dyneither(_tmpC1,sizeof(char),18);}),_tag_dyneither(_tmpC0,sizeof(void*),0));});
if(_tmpB8 != 0)({void*_tmpC2=0;Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_tmpB8->hd)->loc,({
const char*_tmpC3="too many arguments";_tag_dyneither(_tmpC3,sizeof(char),19);}),
_tag_dyneither(_tmpC2,sizeof(void*),0));});}}static void*Cyc_Tcexp_tcUnPrimop(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,void*p,struct
Cyc_Absyn_Exp*e){void*t=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);void*_tmpC4=p;_LL82: if((int)_tmpC4 != 0)goto _LL84;_LL83:
goto _LL85;_LL84: if((int)_tmpC4 != 2)goto _LL86;_LL85: if(!Cyc_Tcutil_is_numeric(e))({
struct Cyc_String_pa_struct _tmpC7;_tmpC7.tag=0;_tmpC7.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));{void*_tmpC5[1]={& _tmpC7};Cyc_Tcutil_terr(
loc,({const char*_tmpC6="expecting arithmetic type but found %s";_tag_dyneither(
_tmpC6,sizeof(char),39);}),_tag_dyneither(_tmpC5,sizeof(void*),1));}});return(
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;_LL86: if((int)_tmpC4 != 11)
goto _LL88;_LL87: Cyc_Tcutil_check_contains_assign(e);if(!Cyc_Tcutil_coerce_to_bool(
te,e))({struct Cyc_String_pa_struct _tmpCA;_tmpCA.tag=0;_tmpCA.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));{void*_tmpC8[1]={&
_tmpCA};Cyc_Tcutil_terr(loc,({const char*_tmpC9="expecting integral or * type but found %s";
_tag_dyneither(_tmpC9,sizeof(char),42);}),_tag_dyneither(_tmpC8,sizeof(void*),1));}});
return Cyc_Absyn_sint_typ;_LL88: if((int)_tmpC4 != 12)goto _LL8A;_LL89: if(!Cyc_Tcutil_is_integral(
e))({struct Cyc_String_pa_struct _tmpCD;_tmpCD.tag=0;_tmpCD.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));{void*_tmpCB[1]={&
_tmpCD};Cyc_Tcutil_terr(loc,({const char*_tmpCC="expecting integral type but found %s";
_tag_dyneither(_tmpCC,sizeof(char),37);}),_tag_dyneither(_tmpCB,sizeof(void*),1));}});
return(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;_LL8A: if((int)_tmpC4
!= 19)goto _LL8C;_LL8B:{void*_tmpCE=t;struct Cyc_Absyn_PtrInfo _tmpCF;struct Cyc_Absyn_PtrAtts
_tmpD0;union Cyc_Absyn_Constraint*_tmpD1;_LL8F: if(_tmpCE <= (void*)4)goto _LL93;if(*((
int*)_tmpCE)!= 7)goto _LL91;_LL90: goto _LL8E;_LL91: if(*((int*)_tmpCE)!= 4)goto
_LL93;_tmpCF=((struct Cyc_Absyn_PointerType_struct*)_tmpCE)->f1;_tmpD0=_tmpCF.ptr_atts;
_tmpD1=_tmpD0.bounds;_LL92:{void*_tmpD2=Cyc_Absyn_conref_constr((void*)((void*)0),
_tmpD1);struct Cyc_Absyn_Exp*_tmpD3;_LL96: if((int)_tmpD2 != 0)goto _LL98;_LL97: goto
_LL95;_LL98: if(_tmpD2 <= (void*)1)goto _LL95;if(*((int*)_tmpD2)!= 0)goto _LL95;
_tmpD3=((struct Cyc_Absyn_Upper_b_struct*)_tmpD2)->f1;_LL99: if(!Cyc_Evexp_c_can_eval(
_tmpD3))({void*_tmpD4=0;Cyc_Tcutil_terr(loc,({const char*_tmpD5="cannot apply numelts to a pointer with abstract bounds";
_tag_dyneither(_tmpD5,sizeof(char),55);}),_tag_dyneither(_tmpD4,sizeof(void*),0));});
goto _LL95;_LL95:;}goto _LL8E;_LL93:;_LL94:({struct Cyc_String_pa_struct _tmpD8;
_tmpD8.tag=0;_tmpD8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t));{void*_tmpD6[1]={& _tmpD8};Cyc_Tcutil_terr(loc,({const char*_tmpD7="numelts requires pointer or array type, not %s";
_tag_dyneither(_tmpD7,sizeof(char),47);}),_tag_dyneither(_tmpD6,sizeof(void*),1));}});
_LL8E:;}return Cyc_Absyn_uint_typ;_LL8C:;_LL8D:({void*_tmpD9=0;((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmpDA="Non-unary primop";
_tag_dyneither(_tmpDA,sizeof(char),17);}),_tag_dyneither(_tmpD9,sizeof(void*),0));});
_LL81:;}static void*Cyc_Tcexp_tcArithBinop(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*
e1,struct Cyc_Absyn_Exp*e2,int(*checker)(struct Cyc_Absyn_Exp*)){if(!checker(e1)){({
struct Cyc_String_pa_struct _tmpDD;_tmpDD.tag=0;_tmpDD.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(e1->topt))->v));{void*_tmpDB[1]={& _tmpDD};Cyc_Tcutil_terr(e1->loc,({
const char*_tmpDC="type %s cannot be used here";_tag_dyneither(_tmpDC,sizeof(char),
28);}),_tag_dyneither(_tmpDB,sizeof(void*),1));}});return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(
te));}if(!checker(e2)){({struct Cyc_String_pa_struct _tmpE0;_tmpE0.tag=0;_tmpE0.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)((
struct Cyc_Core_Opt*)_check_null(e2->topt))->v));{void*_tmpDE[1]={& _tmpE0};Cyc_Tcutil_terr(
e2->loc,({const char*_tmpDF="type %s cannot be used here";_tag_dyneither(_tmpDF,
sizeof(char),28);}),_tag_dyneither(_tmpDE,sizeof(void*),1));}});return Cyc_Absyn_wildtyp(
Cyc_Tcenv_lookup_opt_type_vars(te));}{void*t1=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(e1->topt))->v);void*t2=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(e2->topt))->v);return Cyc_Tcutil_max_arithmetic_type(
t1,t2);}}static void*Cyc_Tcexp_tcPlus(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*
e1,struct Cyc_Absyn_Exp*e2){void*t1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e1->topt))->v);void*t2=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e2->topt))->v);void*_tmpE1=t1;struct Cyc_Absyn_PtrInfo _tmpE2;void*
_tmpE3;struct Cyc_Absyn_Tqual _tmpE4;struct Cyc_Absyn_PtrAtts _tmpE5;void*_tmpE6;
union Cyc_Absyn_Constraint*_tmpE7;union Cyc_Absyn_Constraint*_tmpE8;union Cyc_Absyn_Constraint*
_tmpE9;_LL9B: if(_tmpE1 <= (void*)4)goto _LL9D;if(*((int*)_tmpE1)!= 4)goto _LL9D;
_tmpE2=((struct Cyc_Absyn_PointerType_struct*)_tmpE1)->f1;_tmpE3=_tmpE2.elt_typ;
_tmpE4=_tmpE2.elt_tq;_tmpE5=_tmpE2.ptr_atts;_tmpE6=_tmpE5.rgn;_tmpE7=_tmpE5.nullable;
_tmpE8=_tmpE5.bounds;_tmpE9=_tmpE5.zero_term;_LL9C: if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(
_tmpE3),(void*)1))({void*_tmpEA=0;Cyc_Tcutil_terr(e1->loc,({const char*_tmpEB="can't perform arithmetic on abstract pointer type";
_tag_dyneither(_tmpEB,sizeof(char),50);}),_tag_dyneither(_tmpEA,sizeof(void*),0));});
if(Cyc_Tcutil_is_noalias_pointer(t1))({void*_tmpEC=0;Cyc_Tcutil_terr(e1->loc,({
const char*_tmpED="can't perform arithmetic on non-aliasing pointer type";
_tag_dyneither(_tmpED,sizeof(char),54);}),_tag_dyneither(_tmpEC,sizeof(void*),0));});
if(!Cyc_Tcutil_coerce_sint_typ(te,e2))({struct Cyc_String_pa_struct _tmpF0;_tmpF0.tag=
0;_tmpF0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t2));{void*_tmpEE[1]={& _tmpF0};Cyc_Tcutil_terr(e2->loc,({const char*_tmpEF="expecting int but found %s";
_tag_dyneither(_tmpEF,sizeof(char),27);}),_tag_dyneither(_tmpEE,sizeof(void*),1));}});{
void*_tmpF1=Cyc_Absyn_conref_constr((void*)((void*)0),_tmpE8);struct Cyc_Absyn_Exp*
_tmpF2;_LLA0: if((int)_tmpF1 != 0)goto _LLA2;_LLA1: return t1;_LLA2: if(_tmpF1 <= (void*)
1)goto _LL9F;if(*((int*)_tmpF1)!= 0)goto _LL9F;_tmpF2=((struct Cyc_Absyn_Upper_b_struct*)
_tmpF1)->f1;_LLA3: if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
0,_tmpE9))({void*_tmpF3=0;Cyc_Tcutil_warn(e1->loc,({const char*_tmpF4="pointer arithmetic on thin, zero-terminated pointer may be expensive.";
_tag_dyneither(_tmpF4,sizeof(char),70);}),_tag_dyneither(_tmpF3,sizeof(void*),0));});{
struct Cyc_Absyn_PointerType_struct*_tmpF5=({struct Cyc_Absyn_PointerType_struct*
_tmpF6=_cycalloc(sizeof(*_tmpF6));_tmpF6[0]=({struct Cyc_Absyn_PointerType_struct
_tmpF7;_tmpF7.tag=4;_tmpF7.f1=({struct Cyc_Absyn_PtrInfo _tmpF8;_tmpF8.elt_typ=
_tmpE3;_tmpF8.elt_tq=_tmpE4;_tmpF8.ptr_atts=({struct Cyc_Absyn_PtrAtts _tmpF9;
_tmpF9.rgn=_tmpE6;_tmpF9.nullable=Cyc_Absyn_true_conref;_tmpF9.bounds=Cyc_Absyn_bounds_dyneither_conref;
_tmpF9.zero_term=_tmpE9;_tmpF9.ptrloc=0;_tmpF9;});_tmpF8;});_tmpF7;});_tmpF6;});
Cyc_Tcutil_unchecked_cast(te,e1,(void*)_tmpF5,(void*)3);return(void*)_tmpF5;}
_LL9F:;}_LL9D:;_LL9E: return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_numeric);
_LL9A:;}static void*Cyc_Tcexp_tcMinus(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*
e1,struct Cyc_Absyn_Exp*e2){void*t1=(void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v;
void*t2=(void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v;void*t1_elt=(void*)
0;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(t1,& t1_elt)){if(Cyc_Tcutil_is_tagged_pointer_typ(
t2)){if(!Cyc_Tcutil_unify(t1,t2)){({struct Cyc_String_pa_struct _tmpFD;_tmpFD.tag=
0;_tmpFD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v));{struct Cyc_String_pa_struct
_tmpFC;_tmpFC.tag=0;_tmpFC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v));{void*_tmpFA[2]={& _tmpFC,&
_tmpFD};Cyc_Tcutil_terr(e1->loc,({const char*_tmpFB="pointer arithmetic on values of different types (%s != %s)";
_tag_dyneither(_tmpFB,sizeof(char),59);}),_tag_dyneither(_tmpFA,sizeof(void*),2));}}});
Cyc_Tcutil_explain_failure();}return Cyc_Absyn_sint_typ;}else{if(!Cyc_Tcutil_kind_leq(
Cyc_Tcutil_typ_kind(t1_elt),(void*)1))({void*_tmpFE=0;Cyc_Tcutil_terr(e1->loc,({
const char*_tmpFF="can't perform arithmetic on abstract pointer type";
_tag_dyneither(_tmpFF,sizeof(char),50);}),_tag_dyneither(_tmpFE,sizeof(void*),0));});
if(Cyc_Tcutil_is_noalias_pointer(t1))({void*_tmp100=0;Cyc_Tcutil_terr(e1->loc,({
const char*_tmp101="can't perform arithmetic on non-aliasing pointer type";
_tag_dyneither(_tmp101,sizeof(char),54);}),_tag_dyneither(_tmp100,sizeof(void*),
0));});if(!Cyc_Tcutil_coerce_sint_typ(te,e2)){({struct Cyc_String_pa_struct
_tmp105;_tmp105.tag=0;_tmp105.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2));{struct Cyc_String_pa_struct _tmp104;_tmp104.tag=0;
_tmp104.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t1));{void*_tmp102[2]={& _tmp104,& _tmp105};Cyc_Tcutil_terr(e2->loc,({const char*
_tmp103="expecting either %s or int but found %s";_tag_dyneither(_tmp103,sizeof(
char),40);}),_tag_dyneither(_tmp102,sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();}
return t1;}}if(Cyc_Tcutil_is_pointer_type(t1))Cyc_Tcutil_unchecked_cast(te,e1,Cyc_Absyn_sint_typ,(
void*)3);if(Cyc_Tcutil_is_pointer_type(t2))Cyc_Tcutil_unchecked_cast(te,e2,Cyc_Absyn_sint_typ,(
void*)3);return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_numeric);}static
void*Cyc_Tcexp_tcAnyBinop(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){int e1_is_num=Cyc_Tcutil_is_numeric(
e1);int e2_is_num=Cyc_Tcutil_is_numeric(e2);void*t1=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(e1->topt))->v);void*t2=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(e2->topt))->v);if(e1_is_num  && e2_is_num)return
Cyc_Absyn_sint_typ;else{if(Cyc_Tcutil_typ_kind(t1)== (void*)2  || Cyc_Tcutil_unify(
t1,Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_bk,Cyc_Tcenv_lookup_opt_type_vars(
te)))){if(Cyc_Tcutil_unify(t1,t2))return Cyc_Absyn_sint_typ;else{if(Cyc_Tcutil_silent_castable(
te,loc,t2,t1)){Cyc_Tcutil_unchecked_cast(te,e2,t1,(void*)3);return Cyc_Absyn_sint_typ;}
else{if(Cyc_Tcutil_silent_castable(te,loc,t1,t2)){Cyc_Tcutil_unchecked_cast(te,
e1,t2,(void*)3);return Cyc_Absyn_sint_typ;}else{if(Cyc_Tcutil_zero_to_null(te,t2,
e1) || Cyc_Tcutil_zero_to_null(te,t1,e2))return Cyc_Absyn_sint_typ;else{goto
pointer_cmp;}}}}}else{pointer_cmp: {struct _tuple0 _tmp107=({struct _tuple0 _tmp106;
_tmp106.f1=Cyc_Tcutil_compress(t1);_tmp106.f2=Cyc_Tcutil_compress(t2);_tmp106;});
void*_tmp108;struct Cyc_Absyn_PtrInfo _tmp109;void*_tmp10A;void*_tmp10B;struct Cyc_Absyn_PtrInfo
_tmp10C;void*_tmp10D;void*_tmp10E;void*_tmp10F;_LLA5: _tmp108=_tmp107.f1;if(
_tmp108 <= (void*)4)goto _LLA7;if(*((int*)_tmp108)!= 4)goto _LLA7;_tmp109=((struct
Cyc_Absyn_PointerType_struct*)_tmp108)->f1;_tmp10A=_tmp109.elt_typ;_tmp10B=
_tmp107.f2;if(_tmp10B <= (void*)4)goto _LLA7;if(*((int*)_tmp10B)!= 4)goto _LLA7;
_tmp10C=((struct Cyc_Absyn_PointerType_struct*)_tmp10B)->f1;_tmp10D=_tmp10C.elt_typ;
_LLA6: if(Cyc_Tcutil_unify(_tmp10A,_tmp10D))return Cyc_Absyn_sint_typ;goto _LLA4;
_LLA7: _tmp10E=_tmp107.f1;if(_tmp10E <= (void*)4)goto _LLA9;if(*((int*)_tmp10E)!= 
14)goto _LLA9;_tmp10F=_tmp107.f2;if(_tmp10F <= (void*)4)goto _LLA9;if(*((int*)
_tmp10F)!= 14)goto _LLA9;_LLA8: return Cyc_Absyn_sint_typ;_LLA9:;_LLAA: goto _LLA4;
_LLA4:;}({struct Cyc_String_pa_struct _tmp113;_tmp113.tag=0;_tmp113.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2));{struct Cyc_String_pa_struct
_tmp112;_tmp112.tag=0;_tmp112.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t1));{void*_tmp110[2]={& _tmp112,& _tmp113};Cyc_Tcutil_terr(
loc,({const char*_tmp111="comparison not allowed between %s and %s";
_tag_dyneither(_tmp111,sizeof(char),41);}),_tag_dyneither(_tmp110,sizeof(void*),
2));}}});Cyc_Tcutil_explain_failure();return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(
te));}}}static void*Cyc_Tcexp_tcEqPrimop(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){if(Cyc_Tcexp_tcAnyBinop(te,
loc,e1,e2)== Cyc_Absyn_sint_typ)return Cyc_Absyn_sint_typ;{void*t1=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v);void*t2=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v);{struct _tuple0 _tmp115=({
struct _tuple0 _tmp114;_tmp114.f1=t1;_tmp114.f2=t2;_tmp114;});void*_tmp116;void*
_tmp117;void*_tmp118;void*_tmp119;_LLAC: _tmp116=_tmp115.f1;if(_tmp116 <= (void*)4)
goto _LLAE;if(*((int*)_tmp116)!= 14)goto _LLAE;_tmp117=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp116)->f1;_tmp118=_tmp115.f2;if(_tmp118 <= (void*)4)goto _LLAE;if(*((int*)
_tmp118)!= 14)goto _LLAE;_tmp119=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp118)->f1;_LLAD: return Cyc_Absyn_sint_typ;_LLAE:;_LLAF: goto _LLAB;_LLAB:;}({
struct Cyc_String_pa_struct _tmp11D;_tmp11D.tag=0;_tmp11D.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2));{struct Cyc_String_pa_struct
_tmp11C;_tmp11C.tag=0;_tmp11C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t1));{void*_tmp11A[2]={& _tmp11C,& _tmp11D};Cyc_Tcutil_terr(
loc,({const char*_tmp11B="comparison not allowed between %s and %s";
_tag_dyneither(_tmp11B,sizeof(char),41);}),_tag_dyneither(_tmp11A,sizeof(void*),
2));}}});Cyc_Tcutil_explain_failure();return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(
te));}}static void*Cyc_Tcexp_tcBinPrimop(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,void*p,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){void*
_tmp11E=p;_LLB1: if((int)_tmp11E != 0)goto _LLB3;_LLB2: return Cyc_Tcexp_tcPlus(te,e1,
e2);_LLB3: if((int)_tmp11E != 2)goto _LLB5;_LLB4: return Cyc_Tcexp_tcMinus(te,e1,e2);
_LLB5: if((int)_tmp11E != 1)goto _LLB7;_LLB6: goto _LLB8;_LLB7: if((int)_tmp11E != 3)
goto _LLB9;_LLB8: return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_numeric);
_LLB9: if((int)_tmp11E != 4)goto _LLBB;_LLBA: goto _LLBC;_LLBB: if((int)_tmp11E != 13)
goto _LLBD;_LLBC: goto _LLBE;_LLBD: if((int)_tmp11E != 14)goto _LLBF;_LLBE: goto _LLC0;
_LLBF: if((int)_tmp11E != 15)goto _LLC1;_LLC0: goto _LLC2;_LLC1: if((int)_tmp11E != 16)
goto _LLC3;_LLC2: goto _LLC4;_LLC3: if((int)_tmp11E != 17)goto _LLC5;_LLC4: goto _LLC6;
_LLC5: if((int)_tmp11E != 18)goto _LLC7;_LLC6: return Cyc_Tcexp_tcArithBinop(te,e1,e2,
Cyc_Tcutil_is_integral);_LLC7: if((int)_tmp11E != 5)goto _LLC9;_LLC8: goto _LLCA;
_LLC9: if((int)_tmp11E != 6)goto _LLCB;_LLCA: return Cyc_Tcexp_tcEqPrimop(te,loc,e1,
e2);_LLCB: if((int)_tmp11E != 7)goto _LLCD;_LLCC: goto _LLCE;_LLCD: if((int)_tmp11E != 
8)goto _LLCF;_LLCE: goto _LLD0;_LLCF: if((int)_tmp11E != 9)goto _LLD1;_LLD0: goto _LLD2;
_LLD1: if((int)_tmp11E != 10)goto _LLD3;_LLD2: return Cyc_Tcexp_tcAnyBinop(te,loc,e1,
e2);_LLD3:;_LLD4:({void*_tmp11F=0;((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp120="bad binary primop";
_tag_dyneither(_tmp120,sizeof(char),18);}),_tag_dyneither(_tmp11F,sizeof(void*),
0));});_LLB0:;}static void*Cyc_Tcexp_tcPrimop(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,void*p,struct Cyc_List_List*es){if(p == (void*)2  && ((int(*)(struct
Cyc_List_List*x))Cyc_List_length)(es)== 1)return Cyc_Tcexp_tcExp(te,topt,(struct
Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd);Cyc_Tcexp_tcExpList(
te,es);{void*t;switch(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es)){case
0: _LLD5: return({void*_tmp121=0;Cyc_Tcexp_expr_err(te,loc,topt,({const char*
_tmp122="primitive operator has 0 arguments";_tag_dyneither(_tmp122,sizeof(char),
35);}),_tag_dyneither(_tmp121,sizeof(void*),0));});case 1: _LLD6: t=Cyc_Tcexp_tcUnPrimop(
te,loc,topt,p,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd);
break;case 2: _LLD7: t=Cyc_Tcexp_tcBinPrimop(te,loc,topt,p,(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(es))->hd,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(((struct Cyc_List_List*)_check_null(es))->tl))->hd);break;default:
_LLD8: return({void*_tmp123=0;Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp124="primitive operator has > 2 arguments";
_tag_dyneither(_tmp124,sizeof(char),37);}),_tag_dyneither(_tmp123,sizeof(void*),
0));});}return t;}}struct _tuple17{struct Cyc_Absyn_Tqual f1;void*f2;};static void Cyc_Tcexp_check_writable(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){{void*_tmp125=e->r;void*_tmp126;
struct Cyc_Absyn_Vardecl*_tmp127;void*_tmp128;struct Cyc_Absyn_Vardecl*_tmp129;
void*_tmp12A;struct Cyc_Absyn_Vardecl*_tmp12B;void*_tmp12C;struct Cyc_Absyn_Vardecl*
_tmp12D;struct Cyc_Absyn_Exp*_tmp12E;struct Cyc_Absyn_Exp*_tmp12F;struct Cyc_Absyn_Exp*
_tmp130;struct _dyneither_ptr*_tmp131;struct Cyc_Absyn_Exp*_tmp132;struct
_dyneither_ptr*_tmp133;struct Cyc_Absyn_Exp*_tmp134;struct Cyc_Absyn_Exp*_tmp135;
struct Cyc_Absyn_Exp*_tmp136;_LLDB: if(*((int*)_tmp125)!= 1)goto _LLDD;_tmp126=(
void*)((struct Cyc_Absyn_Var_e_struct*)_tmp125)->f2;if(_tmp126 <= (void*)1)goto
_LLDD;if(*((int*)_tmp126)!= 2)goto _LLDD;_tmp127=((struct Cyc_Absyn_Param_b_struct*)
_tmp126)->f1;_LLDC: _tmp129=_tmp127;goto _LLDE;_LLDD: if(*((int*)_tmp125)!= 1)goto
_LLDF;_tmp128=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp125)->f2;if(_tmp128 <= (
void*)1)goto _LLDF;if(*((int*)_tmp128)!= 3)goto _LLDF;_tmp129=((struct Cyc_Absyn_Local_b_struct*)
_tmp128)->f1;_LLDE: _tmp12B=_tmp129;goto _LLE0;_LLDF: if(*((int*)_tmp125)!= 1)goto
_LLE1;_tmp12A=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp125)->f2;if(_tmp12A <= (
void*)1)goto _LLE1;if(*((int*)_tmp12A)!= 4)goto _LLE1;_tmp12B=((struct Cyc_Absyn_Pat_b_struct*)
_tmp12A)->f1;_LLE0: _tmp12D=_tmp12B;goto _LLE2;_LLE1: if(*((int*)_tmp125)!= 1)goto
_LLE3;_tmp12C=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp125)->f2;if(_tmp12C <= (
void*)1)goto _LLE3;if(*((int*)_tmp12C)!= 0)goto _LLE3;_tmp12D=((struct Cyc_Absyn_Global_b_struct*)
_tmp12C)->f1;_LLE2: if(!(_tmp12D->tq).real_const)return;goto _LLDA;_LLE3: if(*((int*)
_tmp125)!= 25)goto _LLE5;_tmp12E=((struct Cyc_Absyn_Subscript_e_struct*)_tmp125)->f1;
_tmp12F=((struct Cyc_Absyn_Subscript_e_struct*)_tmp125)->f2;_LLE4:{void*_tmp137=
Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp12E->topt))->v);
struct Cyc_Absyn_PtrInfo _tmp138;struct Cyc_Absyn_Tqual _tmp139;struct Cyc_Absyn_ArrayInfo
_tmp13A;struct Cyc_Absyn_Tqual _tmp13B;struct Cyc_List_List*_tmp13C;_LLF2: if(
_tmp137 <= (void*)4)goto _LLF8;if(*((int*)_tmp137)!= 4)goto _LLF4;_tmp138=((struct
Cyc_Absyn_PointerType_struct*)_tmp137)->f1;_tmp139=_tmp138.elt_tq;_LLF3: _tmp13B=
_tmp139;goto _LLF5;_LLF4: if(*((int*)_tmp137)!= 7)goto _LLF6;_tmp13A=((struct Cyc_Absyn_ArrayType_struct*)
_tmp137)->f1;_tmp13B=_tmp13A.tq;_LLF5: if(!_tmp13B.real_const)return;goto _LLF1;
_LLF6: if(*((int*)_tmp137)!= 9)goto _LLF8;_tmp13C=((struct Cyc_Absyn_TupleType_struct*)
_tmp137)->f1;_LLF7: {unsigned int _tmp13E;int _tmp13F;struct _tuple13 _tmp13D=Cyc_Evexp_eval_const_uint_exp(
_tmp12F);_tmp13E=_tmp13D.f1;_tmp13F=_tmp13D.f2;if(!_tmp13F){({void*_tmp140=0;Cyc_Tcutil_terr(
e->loc,({const char*_tmp141="tuple projection cannot use sizeof or offsetof";
_tag_dyneither(_tmp141,sizeof(char),47);}),_tag_dyneither(_tmp140,sizeof(void*),
0));});return;}{struct _handler_cons _tmp142;_push_handler(& _tmp142);{int _tmp144=0;
if(setjmp(_tmp142.handler))_tmp144=1;if(!_tmp144){{struct _tuple17 _tmp146;struct
Cyc_Absyn_Tqual _tmp147;struct _tuple17*_tmp145=((struct _tuple17*(*)(struct Cyc_List_List*
x,int n))Cyc_List_nth)(_tmp13C,(int)_tmp13E);_tmp146=*_tmp145;_tmp147=_tmp146.f1;
if(!_tmp147.real_const){_npop_handler(0);return;}};_pop_handler();}else{void*
_tmp143=(void*)_exn_thrown;void*_tmp149=_tmp143;_LLFB: if(_tmp149 != Cyc_List_Nth)
goto _LLFD;_LLFC: return;_LLFD:;_LLFE:(void)_throw(_tmp149);_LLFA:;}}}goto _LLF1;}
_LLF8:;_LLF9: goto _LLF1;_LLF1:;}goto _LLDA;_LLE5: if(*((int*)_tmp125)!= 23)goto
_LLE7;_tmp130=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp125)->f1;_tmp131=((
struct Cyc_Absyn_AggrMember_e_struct*)_tmp125)->f2;_LLE6:{void*_tmp14A=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp130->topt))->v);struct Cyc_Absyn_AggrInfo
_tmp14B;union Cyc_Absyn_AggrInfoU _tmp14C;struct Cyc_Absyn_Aggrdecl**_tmp14D;struct
Cyc_List_List*_tmp14E;_LL100: if(_tmp14A <= (void*)4)goto _LL104;if(*((int*)_tmp14A)
!= 10)goto _LL102;_tmp14B=((struct Cyc_Absyn_AggrType_struct*)_tmp14A)->f1;_tmp14C=
_tmp14B.aggr_info;if((_tmp14C.KnownAggr).tag != 2)goto _LL102;_tmp14D=(struct Cyc_Absyn_Aggrdecl**)(
_tmp14C.KnownAggr).val;_LL101: {struct Cyc_Absyn_Aggrfield*sf=(struct Cyc_Absyn_Aggrdecl**)
_tmp14D == 0?0: Cyc_Absyn_lookup_decl_field(*_tmp14D,_tmp131);if(sf == 0  || !(sf->tq).real_const)
return;goto _LLFF;}_LL102: if(*((int*)_tmp14A)!= 11)goto _LL104;_tmp14E=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp14A)->f2;_LL103: {struct Cyc_Absyn_Aggrfield*sf=Cyc_Absyn_lookup_field(
_tmp14E,_tmp131);if(sf == 0  || !(sf->tq).real_const)return;goto _LLFF;}_LL104:;
_LL105: goto _LLFF;_LLFF:;}goto _LLDA;_LLE7: if(*((int*)_tmp125)!= 24)goto _LLE9;
_tmp132=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp125)->f1;_tmp133=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp125)->f2;_LLE8:{void*_tmp14F=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp132->topt))->v);struct Cyc_Absyn_PtrInfo _tmp150;void*_tmp151;
_LL107: if(_tmp14F <= (void*)4)goto _LL109;if(*((int*)_tmp14F)!= 4)goto _LL109;
_tmp150=((struct Cyc_Absyn_PointerType_struct*)_tmp14F)->f1;_tmp151=_tmp150.elt_typ;
_LL108:{void*_tmp152=Cyc_Tcutil_compress(_tmp151);struct Cyc_Absyn_AggrInfo
_tmp153;union Cyc_Absyn_AggrInfoU _tmp154;struct Cyc_Absyn_Aggrdecl**_tmp155;struct
Cyc_List_List*_tmp156;_LL10C: if(_tmp152 <= (void*)4)goto _LL110;if(*((int*)_tmp152)
!= 10)goto _LL10E;_tmp153=((struct Cyc_Absyn_AggrType_struct*)_tmp152)->f1;_tmp154=
_tmp153.aggr_info;if((_tmp154.KnownAggr).tag != 2)goto _LL10E;_tmp155=(struct Cyc_Absyn_Aggrdecl**)(
_tmp154.KnownAggr).val;_LL10D: {struct Cyc_Absyn_Aggrfield*sf=(struct Cyc_Absyn_Aggrdecl**)
_tmp155 == 0?0: Cyc_Absyn_lookup_decl_field(*_tmp155,_tmp133);if(sf == 0  || !(sf->tq).real_const)
return;goto _LL10B;}_LL10E: if(*((int*)_tmp152)!= 11)goto _LL110;_tmp156=((struct
Cyc_Absyn_AnonAggrType_struct*)_tmp152)->f2;_LL10F: {struct Cyc_Absyn_Aggrfield*
sf=Cyc_Absyn_lookup_field(_tmp156,_tmp133);if(sf == 0  || !(sf->tq).real_const)
return;goto _LL10B;}_LL110:;_LL111: goto _LL10B;_LL10B:;}goto _LL106;_LL109:;_LL10A:
goto _LL106;_LL106:;}goto _LLDA;_LLE9: if(*((int*)_tmp125)!= 22)goto _LLEB;_tmp134=((
struct Cyc_Absyn_Deref_e_struct*)_tmp125)->f1;_LLEA:{void*_tmp157=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp134->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp158;struct Cyc_Absyn_Tqual _tmp159;struct Cyc_Absyn_ArrayInfo _tmp15A;struct Cyc_Absyn_Tqual
_tmp15B;_LL113: if(_tmp157 <= (void*)4)goto _LL117;if(*((int*)_tmp157)!= 4)goto
_LL115;_tmp158=((struct Cyc_Absyn_PointerType_struct*)_tmp157)->f1;_tmp159=
_tmp158.elt_tq;_LL114: _tmp15B=_tmp159;goto _LL116;_LL115: if(*((int*)_tmp157)!= 7)
goto _LL117;_tmp15A=((struct Cyc_Absyn_ArrayType_struct*)_tmp157)->f1;_tmp15B=
_tmp15A.tq;_LL116: if(!_tmp15B.real_const)return;goto _LL112;_LL117:;_LL118: goto
_LL112;_LL112:;}goto _LLDA;_LLEB: if(*((int*)_tmp125)!= 13)goto _LLED;_tmp135=((
struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp125)->f1;_LLEC: _tmp136=_tmp135;goto
_LLEE;_LLED: if(*((int*)_tmp125)!= 14)goto _LLEF;_tmp136=((struct Cyc_Absyn_Instantiate_e_struct*)
_tmp125)->f1;_LLEE: Cyc_Tcexp_check_writable(te,_tmp136);return;_LLEF:;_LLF0: goto
_LLDA;_LLDA:;}({struct Cyc_String_pa_struct _tmp15E;_tmp15E.tag=0;_tmp15E.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));{void*
_tmp15C[1]={& _tmp15E};Cyc_Tcutil_terr(e->loc,({const char*_tmp15D="attempt to write a const location: %s";
_tag_dyneither(_tmp15D,sizeof(char),38);}),_tag_dyneither(_tmp15C,sizeof(void*),
1));}});}static void*Cyc_Tcexp_tcIncrement(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*e,void*i){Cyc_Tcexp_tcExpNoPromote(te,0,e);
if(!Cyc_Absyn_is_lvalue(e))return({void*_tmp15F=0;Cyc_Tcexp_expr_err(te,loc,topt,({
const char*_tmp160="increment/decrement of non-lvalue";_tag_dyneither(_tmp160,
sizeof(char),34);}),_tag_dyneither(_tmp15F,sizeof(void*),0));});Cyc_Tcexp_check_writable(
te,e);{void*t=(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;if(!Cyc_Tcutil_is_numeric(
e)){void*telt=(void*)0;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(t,& telt) || Cyc_Tcutil_is_zero_pointer_typ_elt(
t,& telt) && (i == (void*)0  || i == (void*)1)){if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(
telt),(void*)1))({void*_tmp161=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp162="can't perform arithmetic on abstract pointer type";
_tag_dyneither(_tmp162,sizeof(char),50);}),_tag_dyneither(_tmp161,sizeof(void*),
0));});if(Cyc_Tcutil_is_noalias_pointer(t))({void*_tmp163=0;Cyc_Tcutil_terr(e->loc,({
const char*_tmp164="can't perform arithmetic on non-aliasing pointer type";
_tag_dyneither(_tmp164,sizeof(char),54);}),_tag_dyneither(_tmp163,sizeof(void*),
0));});}else{({struct Cyc_String_pa_struct _tmp167;_tmp167.tag=0;_tmp167.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));{void*
_tmp165[1]={& _tmp167};Cyc_Tcutil_terr(e->loc,({const char*_tmp166="expecting arithmetic or ? type but found %s";
_tag_dyneither(_tmp166,sizeof(char),44);}),_tag_dyneither(_tmp165,sizeof(void*),
1));}});}}return t;}}static void*Cyc_Tcexp_tcConditional(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2,struct Cyc_Absyn_Exp*e3){struct _tuple0*_tmp16A;int _tmp16B;struct Cyc_Tcexp_TestEnv
_tmp169=Cyc_Tcexp_tcTest(te,e1,({const char*_tmp168="conditional expression";
_tag_dyneither(_tmp168,sizeof(char),23);}));_tmp16A=_tmp169.eq;_tmp16B=_tmp169.isTrue;
Cyc_Tcexp_tcExp(te,topt,e2);Cyc_Tcexp_tcExp(te,topt,e3);{void*t=Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_mk,Cyc_Tcenv_lookup_opt_type_vars(te));struct Cyc_List_List
_tmp16C=({struct Cyc_List_List _tmp173;_tmp173.hd=e3;_tmp173.tl=0;_tmp173;});
struct Cyc_List_List _tmp16D=({struct Cyc_List_List _tmp172;_tmp172.hd=e2;_tmp172.tl=(
struct Cyc_List_List*)& _tmp16C;_tmp172;});if(!Cyc_Tcutil_coerce_list(te,t,(struct
Cyc_List_List*)& _tmp16D)){({struct Cyc_String_pa_struct _tmp171;_tmp171.tag=0;
_tmp171.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e3->topt))->v));{struct Cyc_String_pa_struct
_tmp170;_tmp170.tag=0;_tmp170.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v));{
void*_tmp16E[2]={& _tmp170,& _tmp171};Cyc_Tcutil_terr(loc,({const char*_tmp16F="conditional clause types do not match: %s != %s";
_tag_dyneither(_tmp16F,sizeof(char),48);}),_tag_dyneither(_tmp16E,sizeof(void*),
2));}}});Cyc_Tcutil_explain_failure();}return t;}}static void*Cyc_Tcexp_tcAnd(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Exp*e1,
struct Cyc_Absyn_Exp*e2){Cyc_Tcexp_tcTest(te,e1,({const char*_tmp174="logical-and expression";
_tag_dyneither(_tmp174,sizeof(char),23);}));Cyc_Tcexp_tcTest(te,e2,({const char*
_tmp175="logical-and expression";_tag_dyneither(_tmp175,sizeof(char),23);}));
return Cyc_Absyn_sint_typ;}static void*Cyc_Tcexp_tcOr(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
Cyc_Tcexp_tcTest(te,e1,({const char*_tmp176="logical-or expression";
_tag_dyneither(_tmp176,sizeof(char),22);}));Cyc_Tcexp_tcTest(te,e2,({const char*
_tmp177="logical-or expression";_tag_dyneither(_tmp177,sizeof(char),22);}));
return Cyc_Absyn_sint_typ;}static void*Cyc_Tcexp_tcAssignOp(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*
po,struct Cyc_Absyn_Exp*e2){struct _RegionHandle*_tmp178=Cyc_Tcenv_get_fnrgn(te);
Cyc_Tcexp_tcExpNoPromote(Cyc_Tcenv_enter_notreadctxt(_tmp178,te),0,e1);Cyc_Tcexp_tcExp(
te,(void**)((void**)((void*)&((struct Cyc_Core_Opt*)_check_null(e1->topt))->v)),
e2);{void*t1=(void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v;void*t2=(
void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v;{void*_tmp179=Cyc_Tcutil_compress(
t1);_LL11A: if(_tmp179 <= (void*)4)goto _LL11C;if(*((int*)_tmp179)!= 7)goto _LL11C;
_LL11B:({void*_tmp17A=0;Cyc_Tcutil_terr(loc,({const char*_tmp17B="cannot assign to an array";
_tag_dyneither(_tmp17B,sizeof(char),26);}),_tag_dyneither(_tmp17A,sizeof(void*),
0));});goto _LL119;_LL11C:;_LL11D: goto _LL119;_LL119:;}if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(
t1),(void*)1))({void*_tmp17C=0;Cyc_Tcutil_terr(loc,({const char*_tmp17D="type is abstract (can't determine size).";
_tag_dyneither(_tmp17D,sizeof(char),41);}),_tag_dyneither(_tmp17C,sizeof(void*),
0));});if(!Cyc_Absyn_is_lvalue(e1))return({void*_tmp17E=0;Cyc_Tcexp_expr_err(te,
loc,topt,({const char*_tmp17F="assignment to non-lvalue";_tag_dyneither(_tmp17F,
sizeof(char),25);}),_tag_dyneither(_tmp17E,sizeof(void*),0));});Cyc_Tcexp_check_writable(
te,e1);if(po == 0){struct _RegionHandle*_tmp180=Cyc_Tcenv_get_fnrgn(te);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(
_tmp180,t2) && !Cyc_Tcutil_is_noalias_path(_tmp180,e2))({void*_tmp181=0;Cyc_Tcutil_terr(
e2->loc,({const char*_tmp182="Cannot consume non-unique paths; do swap instead";
_tag_dyneither(_tmp182,sizeof(char),49);}),_tag_dyneither(_tmp181,sizeof(void*),
0));});if(!Cyc_Tcutil_coerce_assign(te,e2,t1)){void*_tmp183=({struct Cyc_String_pa_struct
_tmp187;_tmp187.tag=0;_tmp187.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2));{struct Cyc_String_pa_struct _tmp186;_tmp186.tag=0;
_tmp186.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t1));{void*_tmp184[2]={& _tmp186,& _tmp187};Cyc_Tcexp_expr_err(te,loc,topt,({const
char*_tmp185="type mismatch: %s != %s";_tag_dyneither(_tmp185,sizeof(char),24);}),
_tag_dyneither(_tmp184,sizeof(void*),2));}}});Cyc_Tcutil_unify(t1,t2);Cyc_Tcutil_explain_failure();
return _tmp183;}}else{void*_tmp188=(void*)po->v;void*_tmp189=Cyc_Tcexp_tcBinPrimop(
te,loc,0,_tmp188,e1,e2);if(!(Cyc_Tcutil_unify(_tmp189,t1) || Cyc_Tcutil_coerceable(
_tmp189) && Cyc_Tcutil_coerceable(t1))){void*_tmp18A=({struct Cyc_String_pa_struct
_tmp18E;_tmp18E.tag=0;_tmp18E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2));{struct Cyc_String_pa_struct _tmp18D;_tmp18D.tag=0;
_tmp18D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t1));{void*_tmp18B[2]={& _tmp18D,& _tmp18E};Cyc_Tcexp_expr_err(te,loc,topt,({const
char*_tmp18C="Cannot use this operator in an assignment when the arguments have types %s and %s";
_tag_dyneither(_tmp18C,sizeof(char),82);}),_tag_dyneither(_tmp18B,sizeof(void*),
2));}}});Cyc_Tcutil_unify(_tmp189,t1);Cyc_Tcutil_explain_failure();return _tmp18A;}
return _tmp189;}return t1;}}static void*Cyc_Tcexp_tcSeqExp(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2){Cyc_Tcexp_tcExp(te,0,e1);Cyc_Tcexp_tcExp(te,topt,e2);return(void*)((struct
Cyc_Core_Opt*)_check_null(e2->topt))->v;}static struct Cyc_Absyn_Datatypefield*Cyc_Tcexp_tcInjection(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*tu,struct _RegionHandle*r,
struct Cyc_List_List*inst,struct Cyc_List_List*fs){static struct Cyc_Absyn_DoubleType_struct
dbl={6,0};static void*dbl_typ=(void*)& dbl;struct Cyc_List_List*fields;void*t1=(
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;{void*_tmp18F=Cyc_Tcutil_compress(
t1);void*_tmp190;void*_tmp191;_LL11F: if((int)_tmp18F != 1)goto _LL121;_LL120: Cyc_Tcutil_unchecked_cast(
te,e,dbl_typ,(void*)1);t1=dbl_typ;goto _LL11E;_LL121: if(_tmp18F <= (void*)4)goto
_LL125;if(*((int*)_tmp18F)!= 5)goto _LL123;_tmp190=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp18F)->f2;if((int)_tmp190 != 0)goto _LL123;_LL122: goto _LL124;_LL123: if(*((int*)
_tmp18F)!= 5)goto _LL125;_tmp191=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp18F)->f2;
if((int)_tmp191 != 1)goto _LL125;_LL124: Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_sint_typ,(
void*)1);t1=Cyc_Absyn_sint_typ;goto _LL11E;_LL125:;_LL126: goto _LL11E;_LL11E:;}
for(fields=fs;fields != 0;fields=fields->tl){struct _tuple1*_tmp193;struct Cyc_List_List*
_tmp194;struct Cyc_Position_Segment*_tmp195;void*_tmp196;struct Cyc_Absyn_Datatypefield
_tmp192=*((struct Cyc_Absyn_Datatypefield*)fields->hd);_tmp193=_tmp192.name;
_tmp194=_tmp192.typs;_tmp195=_tmp192.loc;_tmp196=_tmp192.sc;if(_tmp194 == 0  || 
_tmp194->tl != 0)continue;{void*t2=Cyc_Tcutil_rsubstitute(r,inst,(*((struct
_tuple17*)_tmp194->hd)).f2);if(Cyc_Tcutil_unify(t1,t2))return(struct Cyc_Absyn_Datatypefield*)((
struct Cyc_Absyn_Datatypefield*)fields->hd);}}for(fields=fs;fields != 0;fields=
fields->tl){struct _tuple1*_tmp198;struct Cyc_List_List*_tmp199;struct Cyc_Position_Segment*
_tmp19A;void*_tmp19B;struct Cyc_Absyn_Datatypefield _tmp197=*((struct Cyc_Absyn_Datatypefield*)
fields->hd);_tmp198=_tmp197.name;_tmp199=_tmp197.typs;_tmp19A=_tmp197.loc;
_tmp19B=_tmp197.sc;if(_tmp199 == 0  || _tmp199->tl != 0)continue;{void*t2=Cyc_Tcutil_rsubstitute(
r,inst,(*((struct _tuple17*)_tmp199->hd)).f2);if(Cyc_Tcutil_coerce_arg(te,e,t2))
return(struct Cyc_Absyn_Datatypefield*)((struct Cyc_Absyn_Datatypefield*)fields->hd);}}({
struct Cyc_String_pa_struct _tmp19F;_tmp19F.tag=0;_tmp19F.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));{struct Cyc_String_pa_struct
_tmp19E;_tmp19E.tag=0;_tmp19E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(tu));{void*_tmp19C[2]={& _tmp19E,& _tmp19F};Cyc_Tcutil_terr(
e->loc,({const char*_tmp19D="can't find a field in %s to inject a value of type %s";
_tag_dyneither(_tmp19D,sizeof(char),54);}),_tag_dyneither(_tmp19C,sizeof(void*),
2));}}});return 0;}static void*Cyc_Tcexp_tcFnCall(struct Cyc_Tcenv_Tenv*te_orig,
struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*
args,struct Cyc_Absyn_VarargCallInfo**vararg_call_info){struct Cyc_List_List*
_tmp1A1=args;struct _RegionHandle*_tmp1A2=Cyc_Tcenv_get_fnrgn(te_orig);struct Cyc_Tcenv_Tenv*
_tmp1A3=Cyc_Tcenv_new_block(_tmp1A2,loc,te_orig);Cyc_Tcexp_tcExp(_tmp1A3,0,e);{
void*t=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
void*_tmp1A4=t;struct Cyc_Absyn_PtrInfo _tmp1A5;void*_tmp1A6;struct Cyc_Absyn_Tqual
_tmp1A7;struct Cyc_Absyn_PtrAtts _tmp1A8;void*_tmp1A9;union Cyc_Absyn_Constraint*
_tmp1AA;union Cyc_Absyn_Constraint*_tmp1AB;union Cyc_Absyn_Constraint*_tmp1AC;
_LL128: if(_tmp1A4 <= (void*)4)goto _LL12A;if(*((int*)_tmp1A4)!= 4)goto _LL12A;
_tmp1A5=((struct Cyc_Absyn_PointerType_struct*)_tmp1A4)->f1;_tmp1A6=_tmp1A5.elt_typ;
_tmp1A7=_tmp1A5.elt_tq;_tmp1A8=_tmp1A5.ptr_atts;_tmp1A9=_tmp1A8.rgn;_tmp1AA=
_tmp1A8.nullable;_tmp1AB=_tmp1A8.bounds;_tmp1AC=_tmp1A8.zero_term;_LL129: Cyc_Tcenv_check_rgn_accessible(
_tmp1A3,loc,_tmp1A9);Cyc_Tcutil_check_nonzero_bound(loc,_tmp1AB);{void*_tmp1AD=
Cyc_Tcutil_compress(_tmp1A6);struct Cyc_Absyn_FnInfo _tmp1AE;struct Cyc_List_List*
_tmp1AF;struct Cyc_Core_Opt*_tmp1B0;void*_tmp1B1;struct Cyc_List_List*_tmp1B2;int
_tmp1B3;struct Cyc_Absyn_VarargInfo*_tmp1B4;struct Cyc_List_List*_tmp1B5;struct Cyc_List_List*
_tmp1B6;_LL12D: if(_tmp1AD <= (void*)4)goto _LL12F;if(*((int*)_tmp1AD)!= 8)goto
_LL12F;_tmp1AE=((struct Cyc_Absyn_FnType_struct*)_tmp1AD)->f1;_tmp1AF=_tmp1AE.tvars;
_tmp1B0=_tmp1AE.effect;_tmp1B1=_tmp1AE.ret_typ;_tmp1B2=_tmp1AE.args;_tmp1B3=
_tmp1AE.c_varargs;_tmp1B4=_tmp1AE.cyc_varargs;_tmp1B5=_tmp1AE.rgn_po;_tmp1B6=
_tmp1AE.attributes;_LL12E: if(topt != 0)Cyc_Tcutil_unify(_tmp1B1,*topt);while(
_tmp1A1 != 0  && _tmp1B2 != 0){struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)_tmp1A1->hd;
void*t2=(*((struct _tuple8*)_tmp1B2->hd)).f3;Cyc_Tcexp_tcExp(_tmp1A3,(void**)& t2,
e1);if(!Cyc_Tcutil_coerce_arg(_tmp1A3,e1,t2)){({struct Cyc_String_pa_struct
_tmp1BA;_tmp1BA.tag=0;_tmp1BA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2));{struct Cyc_String_pa_struct _tmp1B9;_tmp1B9.tag=0;
_tmp1B9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v));{void*_tmp1B7[2]={&
_tmp1B9,& _tmp1BA};Cyc_Tcutil_terr(e1->loc,({const char*_tmp1B8="actual argument has type \n\t%s\n but formal has type \n\t%s.";
_tag_dyneither(_tmp1B8,sizeof(char),57);}),_tag_dyneither(_tmp1B7,sizeof(void*),
2));}}});Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v,
t2);Cyc_Tcutil_explain_failure();}if(Cyc_Tcutil_is_noalias_pointer_or_aggr(
_tmp1A2,t2) && !Cyc_Tcutil_is_noalias_path(_tmp1A2,e1))({void*_tmp1BB=0;Cyc_Tcutil_terr(
e1->loc,({const char*_tmp1BC="Cannot consume non-unique paths; do swap instead";
_tag_dyneither(_tmp1BC,sizeof(char),49);}),_tag_dyneither(_tmp1BB,sizeof(void*),
0));});_tmp1A1=_tmp1A1->tl;_tmp1B2=_tmp1B2->tl;}{int args_already_checked=0;{
struct Cyc_List_List*a=_tmp1B6;for(0;a != 0;a=a->tl){void*_tmp1BD=(void*)a->hd;
void*_tmp1BE;int _tmp1BF;int _tmp1C0;_LL132: if(_tmp1BD <= (void*)17)goto _LL134;if(*((
int*)_tmp1BD)!= 3)goto _LL134;_tmp1BE=(void*)((struct Cyc_Absyn_Format_att_struct*)
_tmp1BD)->f1;_tmp1BF=((struct Cyc_Absyn_Format_att_struct*)_tmp1BD)->f2;_tmp1C0=((
struct Cyc_Absyn_Format_att_struct*)_tmp1BD)->f3;_LL133:{struct _handler_cons
_tmp1C1;_push_handler(& _tmp1C1);{int _tmp1C3=0;if(setjmp(_tmp1C1.handler))_tmp1C3=
1;if(!_tmp1C3){{struct Cyc_Absyn_Exp*_tmp1C4=((struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*
x,int n))Cyc_List_nth)(args,_tmp1BF - 1);struct Cyc_Core_Opt*fmt_args;if(_tmp1C0 == 
0)fmt_args=0;else{fmt_args=({struct Cyc_Core_Opt*_tmp1C5=_cycalloc(sizeof(*
_tmp1C5));_tmp1C5->v=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,int i))Cyc_List_nth_tail)(
args,_tmp1C0 - 1);_tmp1C5;});}args_already_checked=1;{struct _RegionHandle*_tmp1C6=
Cyc_Tcenv_get_fnrgn(te_orig);void*_tmp1C7=_tmp1BE;_LL137: if((int)_tmp1C7 != 0)
goto _LL139;_LL138: Cyc_Tcexp_check_format_args(_tmp1A3,_tmp1C4,fmt_args,_tmp1C6,
Cyc_Formatstr_get_format_typs);goto _LL136;_LL139: if((int)_tmp1C7 != 1)goto _LL136;
_LL13A: Cyc_Tcexp_check_format_args(_tmp1A3,_tmp1C4,fmt_args,_tmp1C6,Cyc_Formatstr_get_scanf_typs);
goto _LL136;_LL136:;}};_pop_handler();}else{void*_tmp1C2=(void*)_exn_thrown;void*
_tmp1C9=_tmp1C2;_LL13C: if(_tmp1C9 != Cyc_List_Nth)goto _LL13E;_LL13D:({void*
_tmp1CA=0;Cyc_Tcutil_terr(loc,({const char*_tmp1CB="bad format arguments";
_tag_dyneither(_tmp1CB,sizeof(char),21);}),_tag_dyneither(_tmp1CA,sizeof(void*),
0));});goto _LL13B;_LL13E:;_LL13F:(void)_throw(_tmp1C9);_LL13B:;}}}goto _LL131;
_LL134:;_LL135: goto _LL131;_LL131:;}}if(_tmp1B2 != 0)({void*_tmp1CC=0;Cyc_Tcutil_terr(
loc,({const char*_tmp1CD="too few arguments for function";_tag_dyneither(_tmp1CD,
sizeof(char),31);}),_tag_dyneither(_tmp1CC,sizeof(void*),0));});else{if((_tmp1A1
!= 0  || _tmp1B3) || _tmp1B4 != 0){if(_tmp1B3)for(0;_tmp1A1 != 0;_tmp1A1=_tmp1A1->tl){
Cyc_Tcexp_tcExp(_tmp1A3,0,(struct Cyc_Absyn_Exp*)_tmp1A1->hd);}else{if(_tmp1B4 == 
0)({void*_tmp1CE=0;Cyc_Tcutil_terr(loc,({const char*_tmp1CF="too many arguments for function";
_tag_dyneither(_tmp1CF,sizeof(char),32);}),_tag_dyneither(_tmp1CE,sizeof(void*),
0));});else{void*_tmp1D1;int _tmp1D2;struct Cyc_Absyn_VarargInfo _tmp1D0=*_tmp1B4;
_tmp1D1=_tmp1D0.type;_tmp1D2=_tmp1D0.inject;{struct Cyc_Absyn_VarargCallInfo*
_tmp1D3=({struct Cyc_Absyn_VarargCallInfo*_tmp1EF=_cycalloc(sizeof(*_tmp1EF));
_tmp1EF->num_varargs=0;_tmp1EF->injectors=0;_tmp1EF->vai=(struct Cyc_Absyn_VarargInfo*)
_tmp1B4;_tmp1EF;});*vararg_call_info=(struct Cyc_Absyn_VarargCallInfo*)_tmp1D3;
if(!_tmp1D2)for(0;_tmp1A1 != 0;_tmp1A1=_tmp1A1->tl){struct Cyc_Absyn_Exp*e1=(
struct Cyc_Absyn_Exp*)_tmp1A1->hd;++ _tmp1D3->num_varargs;Cyc_Tcexp_tcExp(_tmp1A3,(
void**)& _tmp1D1,e1);if(!Cyc_Tcutil_coerce_arg(_tmp1A3,e1,_tmp1D1)){({struct Cyc_String_pa_struct
_tmp1D7;_tmp1D7.tag=0;_tmp1D7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v));{
struct Cyc_String_pa_struct _tmp1D6;_tmp1D6.tag=0;_tmp1D6.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp1D1));{void*_tmp1D4[2]={& _tmp1D6,&
_tmp1D7};Cyc_Tcutil_terr(loc,({const char*_tmp1D5="vararg requires type %s but argument has type %s";
_tag_dyneither(_tmp1D5,sizeof(char),49);}),_tag_dyneither(_tmp1D4,sizeof(void*),
2));}}});Cyc_Tcutil_explain_failure();}if(Cyc_Tcutil_is_noalias_pointer_or_aggr(
_tmp1A2,_tmp1D1) && !Cyc_Tcutil_is_noalias_path(_tmp1A2,e1))({void*_tmp1D8=0;Cyc_Tcutil_terr(
e1->loc,({const char*_tmp1D9="Cannot consume non-unique paths; do swap instead";
_tag_dyneither(_tmp1D9,sizeof(char),49);}),_tag_dyneither(_tmp1D8,sizeof(void*),
0));});}else{void*_tmp1DA=Cyc_Tcutil_compress(_tmp1D1);struct Cyc_Absyn_DatatypeInfo
_tmp1DB;union Cyc_Absyn_DatatypeInfoU _tmp1DC;struct Cyc_Absyn_Datatypedecl**
_tmp1DD;struct Cyc_Absyn_Datatypedecl*_tmp1DE;struct Cyc_List_List*_tmp1DF;struct
Cyc_Core_Opt*_tmp1E0;_LL141: if(_tmp1DA <= (void*)4)goto _LL143;if(*((int*)_tmp1DA)
!= 2)goto _LL143;_tmp1DB=((struct Cyc_Absyn_DatatypeType_struct*)_tmp1DA)->f1;
_tmp1DC=_tmp1DB.datatype_info;if((_tmp1DC.KnownDatatype).tag != 2)goto _LL143;
_tmp1DD=(struct Cyc_Absyn_Datatypedecl**)(_tmp1DC.KnownDatatype).val;_tmp1DE=*
_tmp1DD;_tmp1DF=_tmp1DB.targs;_tmp1E0=_tmp1DB.rgn;_LL142: {struct Cyc_Absyn_Datatypedecl*
_tmp1E1=*Cyc_Tcenv_lookup_datatypedecl(_tmp1A3,loc,_tmp1DE->name);struct Cyc_List_List*
fields=0;if(_tmp1E1->fields == 0)({struct Cyc_String_pa_struct _tmp1E4;_tmp1E4.tag=
0;_tmp1E4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
_tmp1D1));{void*_tmp1E2[1]={& _tmp1E4};Cyc_Tcutil_terr(loc,({const char*_tmp1E3="can't inject into %s";
_tag_dyneither(_tmp1E3,sizeof(char),21);}),_tag_dyneither(_tmp1E2,sizeof(void*),
1));}});else{fields=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(
_tmp1E1->fields))->v;}if(!Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp1E0))->v,Cyc_Tcenv_curr_rgn(_tmp1A3)))({void*_tmp1E5=0;Cyc_Tcutil_terr(
loc,({const char*_tmp1E6="bad region for injected varargs";_tag_dyneither(_tmp1E6,
sizeof(char),32);}),_tag_dyneither(_tmp1E5,sizeof(void*),0));});{struct
_RegionHandle*_tmp1E7=Cyc_Tcenv_get_fnrgn(_tmp1A3);{struct Cyc_List_List*_tmp1E8=((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(_tmp1E7,_tmp1E7,_tmp1E1->tvs,_tmp1DF);
for(0;_tmp1A1 != 0;_tmp1A1=_tmp1A1->tl){++ _tmp1D3->num_varargs;{struct Cyc_Absyn_Exp*
e1=(struct Cyc_Absyn_Exp*)_tmp1A1->hd;if(!args_already_checked){Cyc_Tcexp_tcExp(
_tmp1A3,0,e1);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(_tmp1E7,(void*)((struct
Cyc_Core_Opt*)_check_null(e1->topt))->v) && !Cyc_Tcutil_is_noalias_path(_tmp1E7,
e1))({void*_tmp1E9=0;Cyc_Tcutil_terr(e1->loc,({const char*_tmp1EA="Cannot consume non-unique paths; do swap instead";
_tag_dyneither(_tmp1EA,sizeof(char),49);}),_tag_dyneither(_tmp1E9,sizeof(void*),
0));});}{struct Cyc_Absyn_Datatypefield*_tmp1EB=Cyc_Tcexp_tcInjection(_tmp1A3,e1,
_tmp1D1,_tmp1E7,_tmp1E8,fields);if(_tmp1EB != 0)_tmp1D3->injectors=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp1D3->injectors,({
struct Cyc_List_List*_tmp1EC=_cycalloc(sizeof(*_tmp1EC));_tmp1EC->hd=(struct Cyc_Absyn_Datatypefield*)
_tmp1EB;_tmp1EC->tl=0;_tmp1EC;}));}}}}goto _LL140;}}_LL143:;_LL144:({void*_tmp1ED=
0;Cyc_Tcutil_terr(loc,({const char*_tmp1EE="bad inject vararg type";
_tag_dyneither(_tmp1EE,sizeof(char),23);}),_tag_dyneither(_tmp1ED,sizeof(void*),
0));});goto _LL140;_LL140:;}}}}}}Cyc_Tcenv_check_effect_accessible(_tmp1A3,loc,(
void*)((struct Cyc_Core_Opt*)_check_null(_tmp1B0))->v);Cyc_Tcenv_check_rgn_partial_order(
_tmp1A3,loc,_tmp1B5);return _tmp1B1;}_LL12F:;_LL130: return({void*_tmp1F0=0;Cyc_Tcexp_expr_err(
_tmp1A3,loc,topt,({const char*_tmp1F1="expected pointer to function";
_tag_dyneither(_tmp1F1,sizeof(char),29);}),_tag_dyneither(_tmp1F0,sizeof(void*),
0));});_LL12C:;}_LL12A:;_LL12B: return({void*_tmp1F2=0;Cyc_Tcexp_expr_err(_tmp1A3,
loc,topt,({const char*_tmp1F3="expected pointer to function";_tag_dyneither(
_tmp1F3,sizeof(char),29);}),_tag_dyneither(_tmp1F2,sizeof(void*),0));});_LL127:;}}
static void*Cyc_Tcexp_tcThrow(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*e){Cyc_Tcexp_tcExp(te,(void**)& Cyc_Absyn_exn_typ,
e);if(!Cyc_Tcutil_coerce_arg(te,e,Cyc_Absyn_exn_typ))({struct Cyc_String_pa_struct
_tmp1F6;_tmp1F6.tag=0;_tmp1F6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v));{
void*_tmp1F4[1]={& _tmp1F6};Cyc_Tcutil_terr(loc,({const char*_tmp1F5="expected datatype exn but found %s";
_tag_dyneither(_tmp1F5,sizeof(char),35);}),_tag_dyneither(_tmp1F4,sizeof(void*),
1));}});return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}static void*
Cyc_Tcexp_tcInstantiate(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,
void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*ts){Cyc_Tcexp_tcExpNoInst(
te,0,e);((struct Cyc_Core_Opt*)_check_null(e->topt))->v=(void*)Cyc_Absyn_pointer_expand((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,0);{void*t1=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);{void*_tmp1F7=t1;struct Cyc_Absyn_PtrInfo
_tmp1F8;void*_tmp1F9;struct Cyc_Absyn_Tqual _tmp1FA;struct Cyc_Absyn_PtrAtts _tmp1FB;
void*_tmp1FC;union Cyc_Absyn_Constraint*_tmp1FD;union Cyc_Absyn_Constraint*_tmp1FE;
union Cyc_Absyn_Constraint*_tmp1FF;_LL146: if(_tmp1F7 <= (void*)4)goto _LL148;if(*((
int*)_tmp1F7)!= 4)goto _LL148;_tmp1F8=((struct Cyc_Absyn_PointerType_struct*)
_tmp1F7)->f1;_tmp1F9=_tmp1F8.elt_typ;_tmp1FA=_tmp1F8.elt_tq;_tmp1FB=_tmp1F8.ptr_atts;
_tmp1FC=_tmp1FB.rgn;_tmp1FD=_tmp1FB.nullable;_tmp1FE=_tmp1FB.bounds;_tmp1FF=
_tmp1FB.zero_term;_LL147:{void*_tmp200=Cyc_Tcutil_compress(_tmp1F9);struct Cyc_Absyn_FnInfo
_tmp201;struct Cyc_List_List*_tmp202;struct Cyc_Core_Opt*_tmp203;void*_tmp204;
struct Cyc_List_List*_tmp205;int _tmp206;struct Cyc_Absyn_VarargInfo*_tmp207;struct
Cyc_List_List*_tmp208;struct Cyc_List_List*_tmp209;_LL14B: if(_tmp200 <= (void*)4)
goto _LL14D;if(*((int*)_tmp200)!= 8)goto _LL14D;_tmp201=((struct Cyc_Absyn_FnType_struct*)
_tmp200)->f1;_tmp202=_tmp201.tvars;_tmp203=_tmp201.effect;_tmp204=_tmp201.ret_typ;
_tmp205=_tmp201.args;_tmp206=_tmp201.c_varargs;_tmp207=_tmp201.cyc_varargs;
_tmp208=_tmp201.rgn_po;_tmp209=_tmp201.attributes;_LL14C: {struct _RegionHandle*
_tmp20A=Cyc_Tcenv_get_fnrgn(te);struct Cyc_List_List*instantiation=0;for(0;ts != 0
 && _tmp202 != 0;(ts=ts->tl,_tmp202=_tmp202->tl)){void*k=Cyc_Tcutil_tvar_kind((
struct Cyc_Absyn_Tvar*)_tmp202->hd);Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(
te),k,1,(void*)ts->hd);instantiation=({struct Cyc_List_List*_tmp20B=
_region_malloc(_tmp20A,sizeof(*_tmp20B));_tmp20B->hd=({struct _tuple11*_tmp20C=
_region_malloc(_tmp20A,sizeof(*_tmp20C));_tmp20C->f1=(struct Cyc_Absyn_Tvar*)
_tmp202->hd;_tmp20C->f2=(void*)ts->hd;_tmp20C;});_tmp20B->tl=instantiation;
_tmp20B;});}if(ts != 0)return({void*_tmp20D=0;Cyc_Tcexp_expr_err(te,loc,topt,({
const char*_tmp20E="too many type variables in instantiation";_tag_dyneither(
_tmp20E,sizeof(char),41);}),_tag_dyneither(_tmp20D,sizeof(void*),0));});{void*
new_fn_typ=Cyc_Tcutil_rsubstitute(_tmp20A,instantiation,(void*)({struct Cyc_Absyn_FnType_struct*
_tmp213=_cycalloc(sizeof(*_tmp213));_tmp213[0]=({struct Cyc_Absyn_FnType_struct
_tmp214;_tmp214.tag=8;_tmp214.f1=({struct Cyc_Absyn_FnInfo _tmp215;_tmp215.tvars=
_tmp202;_tmp215.effect=_tmp203;_tmp215.ret_typ=_tmp204;_tmp215.args=_tmp205;
_tmp215.c_varargs=_tmp206;_tmp215.cyc_varargs=_tmp207;_tmp215.rgn_po=_tmp208;
_tmp215.attributes=_tmp209;_tmp215;});_tmp214;});_tmp213;}));return(void*)({
struct Cyc_Absyn_PointerType_struct*_tmp20F=_cycalloc(sizeof(*_tmp20F));_tmp20F[0]=({
struct Cyc_Absyn_PointerType_struct _tmp210;_tmp210.tag=4;_tmp210.f1=({struct Cyc_Absyn_PtrInfo
_tmp211;_tmp211.elt_typ=new_fn_typ;_tmp211.elt_tq=_tmp1FA;_tmp211.ptr_atts=({
struct Cyc_Absyn_PtrAtts _tmp212;_tmp212.rgn=_tmp1FC;_tmp212.nullable=_tmp1FD;
_tmp212.bounds=_tmp1FE;_tmp212.zero_term=_tmp1FF;_tmp212.ptrloc=0;_tmp212;});
_tmp211;});_tmp210;});_tmp20F;});}}_LL14D:;_LL14E: goto _LL14A;_LL14A:;}goto _LL145;
_LL148:;_LL149: goto _LL145;_LL145:;}return({struct Cyc_String_pa_struct _tmp218;
_tmp218.tag=0;_tmp218.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t1));{void*_tmp216[1]={& _tmp218};Cyc_Tcexp_expr_err(te,loc,topt,({const char*
_tmp217="expecting polymorphic type but found %s";_tag_dyneither(_tmp217,sizeof(
char),40);}),_tag_dyneither(_tmp216,sizeof(void*),1));}});}}static void*Cyc_Tcexp_tcCast(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,void*t,struct
Cyc_Absyn_Exp*e,void**c){Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(
te),(void*)1,1,t);Cyc_Tcexp_tcExp(te,(void**)& t,e);{void*t2=(void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v;if(Cyc_Tcutil_silent_castable(te,loc,t2,t))*((void**)
_check_null(c))=(void*)1;else{void*crc=Cyc_Tcutil_castable(te,loc,t2,t);if(crc != (
void*)0)*((void**)_check_null(c))=crc;else{if(Cyc_Tcutil_zero_to_null(te,t,e))*((
void**)_check_null(c))=(void*)1;else{Cyc_Tcutil_unify(t2,t);{void*_tmp219=({
struct Cyc_String_pa_struct _tmp21D;_tmp21D.tag=0;_tmp21D.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));{struct Cyc_String_pa_struct
_tmp21C;_tmp21C.tag=0;_tmp21C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2));{void*_tmp21A[2]={& _tmp21C,& _tmp21D};Cyc_Tcexp_expr_err(
te,loc,(void**)& t,({const char*_tmp21B="cannot cast %s to %s";_tag_dyneither(
_tmp21B,sizeof(char),21);}),_tag_dyneither(_tmp21A,sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();
return _tmp219;}}}}{struct _tuple0 _tmp21F=({struct _tuple0 _tmp21E;_tmp21E.f1=e->r;
_tmp21E.f2=Cyc_Tcutil_compress(t);_tmp21E;});void*_tmp220;struct Cyc_Absyn_MallocInfo
_tmp221;int _tmp222;void*_tmp223;struct Cyc_Absyn_PtrInfo _tmp224;struct Cyc_Absyn_PtrAtts
_tmp225;union Cyc_Absyn_Constraint*_tmp226;union Cyc_Absyn_Constraint*_tmp227;
union Cyc_Absyn_Constraint*_tmp228;_LL150: _tmp220=_tmp21F.f1;if(*((int*)_tmp220)
!= 35)goto _LL152;_tmp221=((struct Cyc_Absyn_Malloc_e_struct*)_tmp220)->f1;_tmp222=
_tmp221.fat_result;_tmp223=_tmp21F.f2;if(_tmp223 <= (void*)4)goto _LL152;if(*((int*)
_tmp223)!= 4)goto _LL152;_tmp224=((struct Cyc_Absyn_PointerType_struct*)_tmp223)->f1;
_tmp225=_tmp224.ptr_atts;_tmp226=_tmp225.nullable;_tmp227=_tmp225.bounds;_tmp228=
_tmp225.zero_term;_LL151: if((_tmp222  && !((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,_tmp228)) && ((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,_tmp226)){void*_tmp229=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,
_tmp227);struct Cyc_Absyn_Exp*_tmp22A;_LL155: if(_tmp229 <= (void*)1)goto _LL157;if(*((
int*)_tmp229)!= 0)goto _LL157;_tmp22A=((struct Cyc_Absyn_Upper_b_struct*)_tmp229)->f1;
_LL156: if((Cyc_Evexp_eval_const_uint_exp(_tmp22A)).f1 == 1)({void*_tmp22B=0;Cyc_Tcutil_warn(
loc,({const char*_tmp22C="cast from ? pointer to * pointer will lose size information";
_tag_dyneither(_tmp22C,sizeof(char),60);}),_tag_dyneither(_tmp22B,sizeof(void*),
0));});goto _LL154;_LL157:;_LL158: goto _LL154;_LL154:;}goto _LL14F;_LL152:;_LL153:
goto _LL14F;_LL14F:;}return t;}}static void*Cyc_Tcexp_tcAddress(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Exp*e0,void**topt,struct Cyc_Absyn_Exp*
e){void**_tmp22D=0;struct Cyc_Absyn_Tqual _tmp22E=Cyc_Absyn_empty_tqual(0);if(topt
!= 0){void*_tmp22F=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo _tmp230;
void*_tmp231;struct Cyc_Absyn_Tqual _tmp232;struct Cyc_Absyn_PtrAtts _tmp233;union
Cyc_Absyn_Constraint*_tmp234;_LL15A: if(_tmp22F <= (void*)4)goto _LL15C;if(*((int*)
_tmp22F)!= 4)goto _LL15C;_tmp230=((struct Cyc_Absyn_PointerType_struct*)_tmp22F)->f1;
_tmp231=_tmp230.elt_typ;_tmp232=_tmp230.elt_tq;_tmp233=_tmp230.ptr_atts;_tmp234=
_tmp233.zero_term;_LL15B: _tmp22D=({void**_tmp235=_cycalloc(sizeof(*_tmp235));
_tmp235[0]=_tmp231;_tmp235;});_tmp22E=_tmp232;goto _LL159;_LL15C:;_LL15D: goto
_LL159;_LL159:;}{struct _RegionHandle*_tmp236=Cyc_Tcenv_get_fnrgn(te);Cyc_Tcexp_tcExpNoInst(
Cyc_Tcenv_clear_notreadctxt(_tmp236,te),_tmp22D,e);if(Cyc_Tcutil_is_noalias_path(
_tmp236,e))({void*_tmp237=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp238="Cannot take the address of an alias-free path";
_tag_dyneither(_tmp238,sizeof(char),46);}),_tag_dyneither(_tmp237,sizeof(void*),
0));});{void*_tmp239=e->r;struct Cyc_Absyn_Exp*_tmp23A;struct Cyc_Absyn_Exp*
_tmp23B;_LL15F: if(*((int*)_tmp239)!= 25)goto _LL161;_tmp23A=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp239)->f1;_tmp23B=((struct Cyc_Absyn_Subscript_e_struct*)_tmp239)->f2;_LL160:{
void*_tmp23C=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp23A->topt))->v);_LL164: if(_tmp23C <= (void*)4)goto _LL166;if(*((int*)_tmp23C)
!= 9)goto _LL166;_LL165: goto _LL163;_LL166:;_LL167: e0->r=(Cyc_Absyn_add_exp(
_tmp23A,_tmp23B,0))->r;return Cyc_Tcexp_tcPlus(te,_tmp23A,_tmp23B);_LL163:;}goto
_LL15E;_LL161:;_LL162: goto _LL15E;_LL15E:;}{void*_tmp23D=e->r;int _tmp23E;int
_tmp23F;_LL169: if(*((int*)_tmp23D)!= 23)goto _LL16B;_tmp23E=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp23D)->f3;if(_tmp23E != 1)goto _LL16B;_LL16A: goto _LL16C;_LL16B: if(*((int*)
_tmp23D)!= 24)goto _LL16D;_tmp23F=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp23D)->f3;
if(_tmp23F != 1)goto _LL16D;_LL16C:({void*_tmp240=0;Cyc_Tcutil_terr(e->loc,({const
char*_tmp241="cannot take the address of a @tagged union member";_tag_dyneither(
_tmp241,sizeof(char),50);}),_tag_dyneither(_tmp240,sizeof(void*),0));});goto
_LL168;_LL16D:;_LL16E: goto _LL168;_LL168:;}{int _tmp243;void*_tmp244;struct
_tuple12 _tmp242=Cyc_Tcutil_addressof_props(te,e);_tmp243=_tmp242.f1;_tmp244=
_tmp242.f2;{struct Cyc_Absyn_Tqual tq=Cyc_Absyn_empty_tqual(0);if(_tmp243){tq.print_const=
1;tq.real_const=1;}{void*t=Cyc_Absyn_at_typ((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,_tmp244,tq,Cyc_Absyn_false_conref);return t;}}}}}static
void*Cyc_Tcexp_tcSizeof(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,
void**topt,void*t){if(te->allow_valueof)return Cyc_Absyn_uint_typ;Cyc_Tcutil_check_type(
loc,te,Cyc_Tcenv_lookup_type_vars(te),(void*)1,1,t);if(!Cyc_Evexp_okay_szofarg(t))({
struct Cyc_String_pa_struct _tmp247;_tmp247.tag=0;_tmp247.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));{void*_tmp245[1]={& _tmp247};Cyc_Tcutil_terr(
loc,({const char*_tmp246="sizeof applied to type %s, which has unknown size here";
_tag_dyneither(_tmp246,sizeof(char),55);}),_tag_dyneither(_tmp245,sizeof(void*),
1));}});if(topt != 0){void*_tmp248=Cyc_Tcutil_compress(*topt);void*_tmp249;_LL170:
if(_tmp248 <= (void*)4)goto _LL172;if(*((int*)_tmp248)!= 18)goto _LL172;_tmp249=(
void*)((struct Cyc_Absyn_TagType_struct*)_tmp248)->f1;_LL171: {struct Cyc_Absyn_Exp*
_tmp24A=Cyc_Absyn_sizeoftyp_exp(t,0);struct Cyc_Absyn_ValueofType_struct*_tmp24B=({
struct Cyc_Absyn_ValueofType_struct*_tmp24C=_cycalloc(sizeof(*_tmp24C));_tmp24C[0]=({
struct Cyc_Absyn_ValueofType_struct _tmp24D;_tmp24D.tag=17;_tmp24D.f1=_tmp24A;
_tmp24D;});_tmp24C;});if(Cyc_Tcutil_unify(_tmp249,(void*)_tmp24B))return _tmp248;
goto _LL16F;}_LL172:;_LL173: goto _LL16F;_LL16F:;}return Cyc_Absyn_uint_typ;}int Cyc_Tcexp_structfield_has_name(
struct _dyneither_ptr*n,struct Cyc_Absyn_Aggrfield*sf){return Cyc_strcmp((struct
_dyneither_ptr)*n,(struct _dyneither_ptr)*sf->name)== 0;}static void*Cyc_Tcexp_tcOffsetof(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,void*t,void*n){
Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(te),(void*)1,1,t);{void*
_tmp24E=n;struct _dyneither_ptr*_tmp24F;unsigned int _tmp250;_LL175: if(*((int*)
_tmp24E)!= 0)goto _LL177;_tmp24F=((struct Cyc_Absyn_StructField_struct*)_tmp24E)->f1;
_LL176: {int bad_type=1;{void*_tmp251=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo
_tmp252;union Cyc_Absyn_AggrInfoU _tmp253;struct Cyc_Absyn_Aggrdecl**_tmp254;struct
Cyc_List_List*_tmp255;_LL17A: if(_tmp251 <= (void*)4)goto _LL17E;if(*((int*)_tmp251)
!= 10)goto _LL17C;_tmp252=((struct Cyc_Absyn_AggrType_struct*)_tmp251)->f1;_tmp253=
_tmp252.aggr_info;if((_tmp253.KnownAggr).tag != 2)goto _LL17C;_tmp254=(struct Cyc_Absyn_Aggrdecl**)(
_tmp253.KnownAggr).val;_LL17B: if((*_tmp254)->impl == 0)goto _LL179;if(!((int(*)(
int(*pred)(struct _dyneither_ptr*,struct Cyc_Absyn_Aggrfield*),struct
_dyneither_ptr*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcexp_structfield_has_name,
_tmp24F,((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmp254)->impl))->fields))({
struct Cyc_String_pa_struct _tmp258;_tmp258.tag=0;_tmp258.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)*_tmp24F);{void*_tmp256[1]={& _tmp258};Cyc_Tcutil_terr(loc,({
const char*_tmp257="no field of struct/union has name %s";_tag_dyneither(_tmp257,
sizeof(char),37);}),_tag_dyneither(_tmp256,sizeof(void*),1));}});bad_type=0;goto
_LL179;_LL17C: if(*((int*)_tmp251)!= 11)goto _LL17E;_tmp255=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp251)->f2;_LL17D: if(!((int(*)(int(*pred)(struct _dyneither_ptr*,struct Cyc_Absyn_Aggrfield*),
struct _dyneither_ptr*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcexp_structfield_has_name,
_tmp24F,_tmp255))({struct Cyc_String_pa_struct _tmp25B;_tmp25B.tag=0;_tmp25B.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp24F);{void*_tmp259[1]={& _tmp25B};
Cyc_Tcutil_terr(loc,({const char*_tmp25A="no field of struct/union has name %s";
_tag_dyneither(_tmp25A,sizeof(char),37);}),_tag_dyneither(_tmp259,sizeof(void*),
1));}});bad_type=0;goto _LL179;_LL17E:;_LL17F: goto _LL179;_LL179:;}if(bad_type)({
struct Cyc_String_pa_struct _tmp25E;_tmp25E.tag=0;_tmp25E.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));{void*_tmp25C[1]={& _tmp25E};Cyc_Tcutil_terr(
loc,({const char*_tmp25D="%s is not a known struct/union type";_tag_dyneither(
_tmp25D,sizeof(char),36);}),_tag_dyneither(_tmp25C,sizeof(void*),1));}});goto
_LL174;}_LL177: if(*((int*)_tmp24E)!= 1)goto _LL174;_tmp250=((struct Cyc_Absyn_TupleIndex_struct*)
_tmp24E)->f1;_LL178: {int bad_type=1;{void*_tmp25F=Cyc_Tcutil_compress(t);struct
Cyc_Absyn_AggrInfo _tmp260;union Cyc_Absyn_AggrInfoU _tmp261;struct Cyc_Absyn_Aggrdecl**
_tmp262;struct Cyc_List_List*_tmp263;struct Cyc_List_List*_tmp264;struct Cyc_Absyn_DatatypeFieldInfo
_tmp265;union Cyc_Absyn_DatatypeFieldInfoU _tmp266;struct _tuple2 _tmp267;struct Cyc_Absyn_Datatypefield*
_tmp268;_LL181: if(_tmp25F <= (void*)4)goto _LL189;if(*((int*)_tmp25F)!= 10)goto
_LL183;_tmp260=((struct Cyc_Absyn_AggrType_struct*)_tmp25F)->f1;_tmp261=_tmp260.aggr_info;
if((_tmp261.KnownAggr).tag != 2)goto _LL183;_tmp262=(struct Cyc_Absyn_Aggrdecl**)(
_tmp261.KnownAggr).val;_LL182: if((*_tmp262)->impl == 0)goto _LL180;_tmp263=((
struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmp262)->impl))->fields;goto _LL184;
_LL183: if(*((int*)_tmp25F)!= 11)goto _LL185;_tmp263=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp25F)->f2;_LL184: if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp263)
<= _tmp250)({struct Cyc_Int_pa_struct _tmp26C;_tmp26C.tag=1;_tmp26C.f1=(
unsigned long)((int)_tmp250);{struct Cyc_Int_pa_struct _tmp26B;_tmp26B.tag=1;
_tmp26B.f1=(unsigned long)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp263);{void*_tmp269[2]={& _tmp26B,& _tmp26C};Cyc_Tcutil_terr(loc,({const char*
_tmp26A="struct/union has too few components: %d <= %d";_tag_dyneither(_tmp26A,
sizeof(char),46);}),_tag_dyneither(_tmp269,sizeof(void*),2));}}});bad_type=0;
goto _LL180;_LL185: if(*((int*)_tmp25F)!= 9)goto _LL187;_tmp264=((struct Cyc_Absyn_TupleType_struct*)
_tmp25F)->f1;_LL186: if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp264)
<= _tmp250)({struct Cyc_Int_pa_struct _tmp270;_tmp270.tag=1;_tmp270.f1=(
unsigned long)((int)_tmp250);{struct Cyc_Int_pa_struct _tmp26F;_tmp26F.tag=1;
_tmp26F.f1=(unsigned long)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp264);{void*_tmp26D[2]={& _tmp26F,& _tmp270};Cyc_Tcutil_terr(loc,({const char*
_tmp26E="tuple has too few components: %d <= %d";_tag_dyneither(_tmp26E,sizeof(
char),39);}),_tag_dyneither(_tmp26D,sizeof(void*),2));}}});bad_type=0;goto _LL180;
_LL187: if(*((int*)_tmp25F)!= 3)goto _LL189;_tmp265=((struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp25F)->f1;_tmp266=_tmp265.field_info;if((_tmp266.KnownDatatypefield).tag != 2)
goto _LL189;_tmp267=(struct _tuple2)(_tmp266.KnownDatatypefield).val;_tmp268=
_tmp267.f2;_LL188: if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp268->typs)
< _tmp250)({struct Cyc_Int_pa_struct _tmp274;_tmp274.tag=1;_tmp274.f1=(
unsigned long)((int)_tmp250);{struct Cyc_Int_pa_struct _tmp273;_tmp273.tag=1;
_tmp273.f1=(unsigned long)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp268->typs);{void*_tmp271[2]={& _tmp273,& _tmp274};Cyc_Tcutil_terr(loc,({const
char*_tmp272="datatype field has too few components: %d < %d";_tag_dyneither(
_tmp272,sizeof(char),47);}),_tag_dyneither(_tmp271,sizeof(void*),2));}}});
bad_type=0;goto _LL180;_LL189:;_LL18A: goto _LL180;_LL180:;}if(bad_type)({struct Cyc_String_pa_struct
_tmp277;_tmp277.tag=0;_tmp277.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t));{void*_tmp275[1]={& _tmp277};Cyc_Tcutil_terr(loc,({
const char*_tmp276="%s is not a known type";_tag_dyneither(_tmp276,sizeof(char),
23);}),_tag_dyneither(_tmp275,sizeof(void*),1));}});goto _LL174;}_LL174:;}return
Cyc_Absyn_uint_typ;}static void*Cyc_Tcexp_tcDeref(struct Cyc_Tcenv_Tenv*te_orig,
struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e){struct
_RegionHandle*_tmp278=Cyc_Tcenv_get_fnrgn(te_orig);struct Cyc_Tcenv_Tenv*_tmp279=
Cyc_Tcenv_clear_notreadctxt(_tmp278,te_orig);Cyc_Tcexp_tcExp(_tmp279,0,e);{void*
t=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
void*_tmp27A=t;struct Cyc_Absyn_PtrInfo _tmp27B;void*_tmp27C;struct Cyc_Absyn_PtrAtts
_tmp27D;void*_tmp27E;union Cyc_Absyn_Constraint*_tmp27F;union Cyc_Absyn_Constraint*
_tmp280;_LL18C: if(_tmp27A <= (void*)4)goto _LL190;if(*((int*)_tmp27A)!= 0)goto
_LL18E;_LL18D: {struct Cyc_List_List*_tmp281=Cyc_Tcenv_lookup_type_vars(_tmp279);
void*_tmp282=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_mk,({struct Cyc_Core_Opt*
_tmp28D=_cycalloc(sizeof(*_tmp28D));_tmp28D->v=_tmp281;_tmp28D;}));void*_tmp283=
Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_trk,({struct Cyc_Core_Opt*
_tmp28C=_cycalloc(sizeof(*_tmp28C));_tmp28C->v=_tmp281;_tmp28C;}));union Cyc_Absyn_Constraint*
_tmp284=Cyc_Absyn_empty_conref();union Cyc_Absyn_Constraint*_tmp285=((union Cyc_Absyn_Constraint*(*)())
Cyc_Absyn_empty_conref)();struct Cyc_Absyn_PtrAtts _tmp286=({struct Cyc_Absyn_PtrAtts
_tmp28B;_tmp28B.rgn=_tmp283;_tmp28B.nullable=((union Cyc_Absyn_Constraint*(*)())
Cyc_Absyn_empty_conref)();_tmp28B.bounds=_tmp284;_tmp28B.zero_term=_tmp285;
_tmp28B.ptrloc=0;_tmp28B;});struct Cyc_Absyn_PointerType_struct*_tmp287=({struct
Cyc_Absyn_PointerType_struct*_tmp288=_cycalloc(sizeof(*_tmp288));_tmp288[0]=({
struct Cyc_Absyn_PointerType_struct _tmp289;_tmp289.tag=4;_tmp289.f1=({struct Cyc_Absyn_PtrInfo
_tmp28A;_tmp28A.elt_typ=_tmp282;_tmp28A.elt_tq=Cyc_Absyn_empty_tqual(0);_tmp28A.ptr_atts=
_tmp286;_tmp28A;});_tmp289;});_tmp288;});Cyc_Tcutil_unify(t,(void*)_tmp287);
_tmp27C=_tmp282;_tmp27E=_tmp283;_tmp27F=_tmp284;_tmp280=_tmp285;goto _LL18F;}
_LL18E: if(*((int*)_tmp27A)!= 4)goto _LL190;_tmp27B=((struct Cyc_Absyn_PointerType_struct*)
_tmp27A)->f1;_tmp27C=_tmp27B.elt_typ;_tmp27D=_tmp27B.ptr_atts;_tmp27E=_tmp27D.rgn;
_tmp27F=_tmp27D.bounds;_tmp280=_tmp27D.zero_term;_LL18F: Cyc_Tcenv_check_rgn_accessible(
_tmp279,loc,_tmp27E);Cyc_Tcutil_check_nonzero_bound(loc,_tmp27F);if(!Cyc_Tcutil_kind_leq(
Cyc_Tcutil_typ_kind(_tmp27C),(void*)1))({void*_tmp28E=0;Cyc_Tcutil_terr(loc,({
const char*_tmp28F="can't dereference abstract pointer type";_tag_dyneither(
_tmp28F,sizeof(char),40);}),_tag_dyneither(_tmp28E,sizeof(void*),0));});return
_tmp27C;_LL190:;_LL191: return({struct Cyc_String_pa_struct _tmp292;_tmp292.tag=0;
_tmp292.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));{
void*_tmp290[1]={& _tmp292};Cyc_Tcexp_expr_err(_tmp279,loc,topt,({const char*
_tmp291="expecting *, @, or ? type but found %s";_tag_dyneither(_tmp291,sizeof(
char),39);}),_tag_dyneither(_tmp290,sizeof(void*),1));}});_LL18B:;}}static void*
Cyc_Tcexp_tcAggrMember(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,
void**topt,struct Cyc_Absyn_Exp*outer_e,struct Cyc_Absyn_Exp*e,struct
_dyneither_ptr*f,int*is_tagged,int*is_read){Cyc_Tcexp_tcExpNoPromote(te,0,e);*
is_read=!Cyc_Tcenv_in_notreadctxt(te);{void*_tmp293=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v);struct Cyc_Absyn_AggrInfo _tmp294;
union Cyc_Absyn_AggrInfoU _tmp295;struct Cyc_Absyn_Aggrdecl**_tmp296;struct Cyc_Absyn_Aggrdecl*
_tmp297;struct Cyc_List_List*_tmp298;void*_tmp299;struct Cyc_List_List*_tmp29A;
_LL193: if(_tmp293 <= (void*)4)goto _LL19B;if(*((int*)_tmp293)!= 10)goto _LL195;
_tmp294=((struct Cyc_Absyn_AggrType_struct*)_tmp293)->f1;_tmp295=_tmp294.aggr_info;
if((_tmp295.KnownAggr).tag != 2)goto _LL195;_tmp296=(struct Cyc_Absyn_Aggrdecl**)(
_tmp295.KnownAggr).val;_tmp297=*_tmp296;_tmp298=_tmp294.targs;_LL194: {struct Cyc_Absyn_Aggrfield*
_tmp29D=Cyc_Absyn_lookup_decl_field(_tmp297,f);if(_tmp29D == 0)return({struct Cyc_String_pa_struct
_tmp2A1;_tmp2A1.tag=0;_tmp2A1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);{
struct Cyc_String_pa_struct _tmp2A0;_tmp2A0.tag=0;_tmp2A0.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp297->name));{void*_tmp29E[2]={&
_tmp2A0,& _tmp2A1};Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp29F="type %s has no %s field";
_tag_dyneither(_tmp29F,sizeof(char),24);}),_tag_dyneither(_tmp29E,sizeof(void*),
2));}}});if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp297->impl))->tagged)*
is_tagged=1;if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp297->impl))->exist_vars
!= 0)return({void*_tmp2A2=0;Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp2A3="must use pattern-matching to access fields of existential types";
_tag_dyneither(_tmp2A3,sizeof(char),64);}),_tag_dyneither(_tmp2A2,sizeof(void*),
0));});{void*t2;struct _RegionHandle*_tmp2A4=Cyc_Tcenv_get_fnrgn(te);{struct Cyc_List_List*
_tmp2A5=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(_tmp2A4,_tmp2A4,
_tmp297->tvs,_tmp298);t2=Cyc_Tcutil_rsubstitute(_tmp2A4,_tmp2A5,_tmp29D->type);
if(((_tmp297->kind == (void*)1  && !((struct Cyc_Absyn_AggrdeclImpl*)_check_null(
_tmp297->impl))->tagged) && !Cyc_Tcenv_in_notreadctxt(te)) && !Cyc_Tcutil_bits_only(
t2))return({struct Cyc_String_pa_struct _tmp2A8;_tmp2A8.tag=0;_tmp2A8.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*f);{void*_tmp2A6[1]={& _tmp2A8};Cyc_Tcexp_expr_err(
te,loc,topt,({const char*_tmp2A7="cannot read union member %s since it is not `bits-only'";
_tag_dyneither(_tmp2A7,sizeof(char),56);}),_tag_dyneither(_tmp2A6,sizeof(void*),
1));}});}return t2;}}_LL195: if(*((int*)_tmp293)!= 11)goto _LL197;_tmp299=(void*)((
struct Cyc_Absyn_AnonAggrType_struct*)_tmp293)->f1;_tmp29A=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp293)->f2;_LL196: {struct Cyc_Absyn_Aggrfield*_tmp2A9=Cyc_Absyn_lookup_field(
_tmp29A,f);if(_tmp2A9 == 0)return({struct Cyc_String_pa_struct _tmp2AC;_tmp2AC.tag=
0;_tmp2AC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);{void*_tmp2AA[1]={&
_tmp2AC};Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp2AB="type has no %s field";
_tag_dyneither(_tmp2AB,sizeof(char),21);}),_tag_dyneither(_tmp2AA,sizeof(void*),
1));}});if((_tmp299 == (void*)1  && !Cyc_Tcenv_in_notreadctxt(te)) && !Cyc_Tcutil_bits_only(
_tmp2A9->type))return({struct Cyc_String_pa_struct _tmp2AF;_tmp2AF.tag=0;_tmp2AF.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*f);{void*_tmp2AD[1]={& _tmp2AF};Cyc_Tcexp_expr_err(
te,loc,topt,({const char*_tmp2AE="cannot read union member %s since it is not `bits-only'";
_tag_dyneither(_tmp2AE,sizeof(char),56);}),_tag_dyneither(_tmp2AD,sizeof(void*),
1));}});return _tmp2A9->type;}_LL197: if(*((int*)_tmp293)!= 7)goto _LL199;if(!(Cyc_strcmp((
struct _dyneither_ptr)*f,({const char*_tmp29B="size";_tag_dyneither(_tmp29B,
sizeof(char),5);}))== 0))goto _LL199;_LL198: goto _LL19A;_LL199: if(*((int*)_tmp293)
!= 4)goto _LL19B;if(!(Cyc_strcmp((struct _dyneither_ptr)*f,({const char*_tmp29C="size";
_tag_dyneither(_tmp29C,sizeof(char),5);}))== 0))goto _LL19B;_LL19A:({struct Cyc_String_pa_struct
_tmp2B2;_tmp2B2.tag=0;_tmp2B2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(e));{void*_tmp2B0[1]={& _tmp2B2};Cyc_Tcutil_warn(e->loc,({
const char*_tmp2B1="deprecated `.size' used here -- change to numelts(%s)";
_tag_dyneither(_tmp2B1,sizeof(char),54);}),_tag_dyneither(_tmp2B0,sizeof(void*),
1));}});outer_e->r=(void*)({struct Cyc_Absyn_Primop_e_struct*_tmp2B3=_cycalloc(
sizeof(*_tmp2B3));_tmp2B3[0]=({struct Cyc_Absyn_Primop_e_struct _tmp2B4;_tmp2B4.tag=
3;_tmp2B4.f1=(void*)((void*)19);_tmp2B4.f2=({struct Cyc_List_List*_tmp2B5=
_cycalloc(sizeof(*_tmp2B5));_tmp2B5->hd=e;_tmp2B5->tl=0;_tmp2B5;});_tmp2B4;});
_tmp2B3;});return Cyc_Absyn_uint_typ;_LL19B:;_LL19C: if(Cyc_strcmp((struct
_dyneither_ptr)*f,({const char*_tmp2B6="size";_tag_dyneither(_tmp2B6,sizeof(char),
5);}))== 0)return({struct Cyc_String_pa_struct _tmp2B9;_tmp2B9.tag=0;_tmp2B9.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v));{void*_tmp2B7[1]={& _tmp2B9};Cyc_Tcexp_expr_err(
te,loc,topt,({const char*_tmp2B8="expecting struct, union, or array, found %s";
_tag_dyneither(_tmp2B8,sizeof(char),44);}),_tag_dyneither(_tmp2B7,sizeof(void*),
1));}});else{return({struct Cyc_String_pa_struct _tmp2BC;_tmp2BC.tag=0;_tmp2BC.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v));{void*_tmp2BA[1]={& _tmp2BC};Cyc_Tcexp_expr_err(
te,loc,topt,({const char*_tmp2BB="expecting struct or union, found %s";
_tag_dyneither(_tmp2BB,sizeof(char),36);}),_tag_dyneither(_tmp2BA,sizeof(void*),
1));}});}_LL192:;}}static void*Cyc_Tcexp_tcAggrArrow(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e,struct
_dyneither_ptr*f,int*is_tagged,int*is_read){struct _RegionHandle*_tmp2BD=Cyc_Tcenv_get_fnrgn(
te);Cyc_Tcexp_tcExp(Cyc_Tcenv_clear_notreadctxt(_tmp2BD,te),0,e);*is_read=!Cyc_Tcenv_in_notreadctxt(
te);{void*_tmp2BE=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
struct Cyc_Absyn_PtrInfo _tmp2BF;void*_tmp2C0;struct Cyc_Absyn_PtrAtts _tmp2C1;void*
_tmp2C2;union Cyc_Absyn_Constraint*_tmp2C3;union Cyc_Absyn_Constraint*_tmp2C4;
_LL19E: if(_tmp2BE <= (void*)4)goto _LL1A0;if(*((int*)_tmp2BE)!= 4)goto _LL1A0;
_tmp2BF=((struct Cyc_Absyn_PointerType_struct*)_tmp2BE)->f1;_tmp2C0=_tmp2BF.elt_typ;
_tmp2C1=_tmp2BF.ptr_atts;_tmp2C2=_tmp2C1.rgn;_tmp2C3=_tmp2C1.bounds;_tmp2C4=
_tmp2C1.zero_term;_LL19F: Cyc_Tcutil_check_nonzero_bound(loc,_tmp2C3);{void*
_tmp2C5=Cyc_Tcutil_compress(_tmp2C0);struct Cyc_Absyn_AggrInfo _tmp2C6;union Cyc_Absyn_AggrInfoU
_tmp2C7;struct Cyc_Absyn_Aggrdecl**_tmp2C8;struct Cyc_Absyn_Aggrdecl*_tmp2C9;
struct Cyc_List_List*_tmp2CA;void*_tmp2CB;struct Cyc_List_List*_tmp2CC;_LL1A3: if(
_tmp2C5 <= (void*)4)goto _LL1A7;if(*((int*)_tmp2C5)!= 10)goto _LL1A5;_tmp2C6=((
struct Cyc_Absyn_AggrType_struct*)_tmp2C5)->f1;_tmp2C7=_tmp2C6.aggr_info;if((
_tmp2C7.KnownAggr).tag != 2)goto _LL1A5;_tmp2C8=(struct Cyc_Absyn_Aggrdecl**)(
_tmp2C7.KnownAggr).val;_tmp2C9=*_tmp2C8;_tmp2CA=_tmp2C6.targs;_LL1A4: {struct Cyc_Absyn_Aggrfield*
_tmp2CD=Cyc_Absyn_lookup_decl_field(_tmp2C9,f);if(_tmp2CD == 0)return({struct Cyc_String_pa_struct
_tmp2D1;_tmp2D1.tag=0;_tmp2D1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);{
struct Cyc_String_pa_struct _tmp2D0;_tmp2D0.tag=0;_tmp2D0.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp2C9->name));{void*_tmp2CE[2]={&
_tmp2D0,& _tmp2D1};Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp2CF="type %s has no %s field";
_tag_dyneither(_tmp2CF,sizeof(char),24);}),_tag_dyneither(_tmp2CE,sizeof(void*),
2));}}});if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp2C9->impl))->tagged)*
is_tagged=1;if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp2C9->impl))->exist_vars
!= 0)return({void*_tmp2D2=0;Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp2D3="must use pattern-matching to access fields of existential types";
_tag_dyneither(_tmp2D3,sizeof(char),64);}),_tag_dyneither(_tmp2D2,sizeof(void*),
0));});{void*t3;struct _RegionHandle*_tmp2D4=Cyc_Tcenv_get_fnrgn(te);{struct Cyc_List_List*
_tmp2D5=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(_tmp2D4,_tmp2D4,
_tmp2C9->tvs,_tmp2CA);t3=Cyc_Tcutil_rsubstitute(_tmp2D4,_tmp2D5,_tmp2CD->type);}
if(((_tmp2C9->kind == (void*)1  && !((struct Cyc_Absyn_AggrdeclImpl*)_check_null(
_tmp2C9->impl))->tagged) && !Cyc_Tcenv_in_notreadctxt(te)) && !Cyc_Tcutil_bits_only(
t3))return({struct Cyc_String_pa_struct _tmp2D8;_tmp2D8.tag=0;_tmp2D8.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*f);{void*_tmp2D6[1]={& _tmp2D8};Cyc_Tcexp_expr_err(
te,loc,topt,({const char*_tmp2D7="cannot read union member %s since it is not `bits-only'";
_tag_dyneither(_tmp2D7,sizeof(char),56);}),_tag_dyneither(_tmp2D6,sizeof(void*),
1));}});return t3;}}_LL1A5: if(*((int*)_tmp2C5)!= 11)goto _LL1A7;_tmp2CB=(void*)((
struct Cyc_Absyn_AnonAggrType_struct*)_tmp2C5)->f1;_tmp2CC=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp2C5)->f2;_LL1A6: {struct Cyc_Absyn_Aggrfield*_tmp2D9=Cyc_Absyn_lookup_field(
_tmp2CC,f);if(_tmp2D9 == 0)return({struct Cyc_String_pa_struct _tmp2DC;_tmp2DC.tag=
0;_tmp2DC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);{void*_tmp2DA[1]={&
_tmp2DC};Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp2DB="type has no %s field";
_tag_dyneither(_tmp2DB,sizeof(char),21);}),_tag_dyneither(_tmp2DA,sizeof(void*),
1));}});if((_tmp2CB == (void*)1  && !Cyc_Tcenv_in_notreadctxt(te)) && !Cyc_Tcutil_bits_only(
_tmp2D9->type))return({struct Cyc_String_pa_struct _tmp2DF;_tmp2DF.tag=0;_tmp2DF.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*f);{void*_tmp2DD[1]={& _tmp2DF};Cyc_Tcexp_expr_err(
te,loc,topt,({const char*_tmp2DE="cannot read union member %s since it is not `bits-only'";
_tag_dyneither(_tmp2DE,sizeof(char),56);}),_tag_dyneither(_tmp2DD,sizeof(void*),
1));}});return _tmp2D9->type;}_LL1A7:;_LL1A8: goto _LL1A2;_LL1A2:;}goto _LL19D;
_LL1A0:;_LL1A1: goto _LL19D;_LL19D:;}return({struct Cyc_String_pa_struct _tmp2E2;
_tmp2E2.tag=0;_tmp2E2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v));{void*_tmp2E0[1]={&
_tmp2E2};Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp2E1="expecting struct or union pointer, found %s";
_tag_dyneither(_tmp2E1,sizeof(char),44);}),_tag_dyneither(_tmp2E0,sizeof(void*),
1));}});}static void*Cyc_Tcexp_ithTupleType(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,struct Cyc_List_List*ts,struct Cyc_Absyn_Exp*index){unsigned int _tmp2E4;int
_tmp2E5;struct _tuple13 _tmp2E3=Cyc_Evexp_eval_const_uint_exp(index);_tmp2E4=
_tmp2E3.f1;_tmp2E5=_tmp2E3.f2;if(!_tmp2E5)return({void*_tmp2E6=0;Cyc_Tcexp_expr_err(
te,loc,0,({const char*_tmp2E7="tuple projection cannot use sizeof or offsetof";
_tag_dyneither(_tmp2E7,sizeof(char),47);}),_tag_dyneither(_tmp2E6,sizeof(void*),
0));});{struct _handler_cons _tmp2E8;_push_handler(& _tmp2E8);{int _tmp2EA=0;if(
setjmp(_tmp2E8.handler))_tmp2EA=1;if(!_tmp2EA){{void*_tmp2EB=(*((struct _tuple17*(*)(
struct Cyc_List_List*x,int n))Cyc_List_nth)(ts,(int)_tmp2E4)).f2;_npop_handler(0);
return _tmp2EB;};_pop_handler();}else{void*_tmp2E9=(void*)_exn_thrown;void*
_tmp2ED=_tmp2E9;_LL1AA: if(_tmp2ED != Cyc_List_Nth)goto _LL1AC;_LL1AB: return({
struct Cyc_Int_pa_struct _tmp2F1;_tmp2F1.tag=1;_tmp2F1.f1=(unsigned long)((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(ts);{struct Cyc_Int_pa_struct _tmp2F0;
_tmp2F0.tag=1;_tmp2F0.f1=(unsigned long)((int)_tmp2E4);{void*_tmp2EE[2]={&
_tmp2F0,& _tmp2F1};Cyc_Tcexp_expr_err(te,loc,0,({const char*_tmp2EF="index is %d but tuple has only %d fields";
_tag_dyneither(_tmp2EF,sizeof(char),41);}),_tag_dyneither(_tmp2EE,sizeof(void*),
2));}}});_LL1AC:;_LL1AD:(void)_throw(_tmp2ED);_LL1A9:;}}}}static void*Cyc_Tcexp_tcSubscript(
struct Cyc_Tcenv_Tenv*te_orig,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
e1,struct Cyc_Absyn_Exp*e2){struct _RegionHandle*_tmp2F2=Cyc_Tcenv_get_fnrgn(
te_orig);struct Cyc_Tcenv_Tenv*_tmp2F3=Cyc_Tcenv_clear_notreadctxt(_tmp2F2,
te_orig);Cyc_Tcexp_tcExp(_tmp2F3,0,e1);Cyc_Tcexp_tcExp(_tmp2F3,0,e2);{void*t1=
Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v);void*
t2=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v);
if(!Cyc_Tcutil_coerce_sint_typ(_tmp2F3,e2))return({struct Cyc_String_pa_struct
_tmp2F6;_tmp2F6.tag=0;_tmp2F6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2));{void*_tmp2F4[1]={& _tmp2F6};Cyc_Tcexp_expr_err(
_tmp2F3,e2->loc,topt,({const char*_tmp2F5="expecting int subscript, found %s";
_tag_dyneither(_tmp2F5,sizeof(char),34);}),_tag_dyneither(_tmp2F4,sizeof(void*),
1));}});{void*_tmp2F7=t1;struct Cyc_Absyn_PtrInfo _tmp2F8;void*_tmp2F9;struct Cyc_Absyn_Tqual
_tmp2FA;struct Cyc_Absyn_PtrAtts _tmp2FB;void*_tmp2FC;union Cyc_Absyn_Constraint*
_tmp2FD;union Cyc_Absyn_Constraint*_tmp2FE;struct Cyc_List_List*_tmp2FF;_LL1AF: if(
_tmp2F7 <= (void*)4)goto _LL1B3;if(*((int*)_tmp2F7)!= 4)goto _LL1B1;_tmp2F8=((
struct Cyc_Absyn_PointerType_struct*)_tmp2F7)->f1;_tmp2F9=_tmp2F8.elt_typ;_tmp2FA=
_tmp2F8.elt_tq;_tmp2FB=_tmp2F8.ptr_atts;_tmp2FC=_tmp2FB.rgn;_tmp2FD=_tmp2FB.bounds;
_tmp2FE=_tmp2FB.zero_term;_LL1B0: if(((int(*)(int y,union Cyc_Absyn_Constraint*x))
Cyc_Absyn_conref_def)(0,_tmp2FE)){int emit_warning=1;{void*_tmp300=Cyc_Absyn_conref_def((
void*)((void*)0),_tmp2FD);struct Cyc_Absyn_Exp*_tmp301;_LL1B6: if(_tmp300 <= (void*)
1)goto _LL1B8;if(*((int*)_tmp300)!= 0)goto _LL1B8;_tmp301=((struct Cyc_Absyn_Upper_b_struct*)
_tmp300)->f1;_LL1B7: if(Cyc_Tcutil_is_const_exp(_tmp2F3,e2)){unsigned int _tmp303;
int _tmp304;struct _tuple13 _tmp302=Cyc_Evexp_eval_const_uint_exp(e2);_tmp303=
_tmp302.f1;_tmp304=_tmp302.f2;if(_tmp304){unsigned int _tmp306;int _tmp307;struct
_tuple13 _tmp305=Cyc_Evexp_eval_const_uint_exp(_tmp301);_tmp306=_tmp305.f1;
_tmp307=_tmp305.f2;if(_tmp307  && _tmp306 > _tmp303)emit_warning=0;}}goto _LL1B5;
_LL1B8: if((int)_tmp300 != 0)goto _LL1B5;_LL1B9: emit_warning=0;goto _LL1B5;_LL1B5:;}
if(emit_warning)({void*_tmp308=0;Cyc_Tcutil_warn(e2->loc,({const char*_tmp309="subscript on thin, zero-terminated pointer could be expensive.";
_tag_dyneither(_tmp309,sizeof(char),63);}),_tag_dyneither(_tmp308,sizeof(void*),
0));});}else{if(Cyc_Tcutil_is_const_exp(_tmp2F3,e2)){unsigned int _tmp30B;int
_tmp30C;struct _tuple13 _tmp30A=Cyc_Evexp_eval_const_uint_exp(e2);_tmp30B=_tmp30A.f1;
_tmp30C=_tmp30A.f2;if(_tmp30C)Cyc_Tcutil_check_bound(loc,_tmp30B,_tmp2FD);}else{
if(Cyc_Tcutil_is_bound_one(_tmp2FD) && !((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,_tmp2FE))({void*_tmp30D=0;Cyc_Tcutil_warn(e1->loc,({
const char*_tmp30E="subscript applied to pointer to one object";_tag_dyneither(
_tmp30E,sizeof(char),43);}),_tag_dyneither(_tmp30D,sizeof(void*),0));});Cyc_Tcutil_check_nonzero_bound(
loc,_tmp2FD);}}Cyc_Tcenv_check_rgn_accessible(_tmp2F3,loc,_tmp2FC);if(!Cyc_Tcutil_kind_leq(
Cyc_Tcutil_typ_kind(_tmp2F9),(void*)1))({void*_tmp30F=0;Cyc_Tcutil_terr(e1->loc,({
const char*_tmp310="can't subscript an abstract pointer";_tag_dyneither(_tmp310,
sizeof(char),36);}),_tag_dyneither(_tmp30F,sizeof(void*),0));});return _tmp2F9;
_LL1B1: if(*((int*)_tmp2F7)!= 9)goto _LL1B3;_tmp2FF=((struct Cyc_Absyn_TupleType_struct*)
_tmp2F7)->f1;_LL1B2: return Cyc_Tcexp_ithTupleType(_tmp2F3,loc,_tmp2FF,e2);_LL1B3:;
_LL1B4: return({struct Cyc_String_pa_struct _tmp313;_tmp313.tag=0;_tmp313.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));{void*
_tmp311[1]={& _tmp313};Cyc_Tcexp_expr_err(_tmp2F3,loc,topt,({const char*_tmp312="subscript applied to %s";
_tag_dyneither(_tmp312,sizeof(char),24);}),_tag_dyneither(_tmp311,sizeof(void*),
1));}});_LL1AE:;}}}static void*Cyc_Tcexp_tcTuple(struct Cyc_Tcenv_Tenv*te,struct
Cyc_Position_Segment*loc,void**topt,struct Cyc_List_List*es){int done=0;struct Cyc_List_List*
fields=0;if(topt != 0){void*_tmp314=Cyc_Tcutil_compress(*topt);struct Cyc_List_List*
_tmp315;_LL1BB: if(_tmp314 <= (void*)4)goto _LL1BD;if(*((int*)_tmp314)!= 9)goto
_LL1BD;_tmp315=((struct Cyc_Absyn_TupleType_struct*)_tmp314)->f1;_LL1BC: if(((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmp315)!= ((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(es))goto _LL1BA;for(0;es != 0;(es=es->tl,_tmp315=_tmp315->tl)){
void*_tmp316=(*((struct _tuple17*)((struct Cyc_List_List*)_check_null(_tmp315))->hd)).f2;
Cyc_Tcexp_tcExpInitializer(te,(void**)& _tmp316,(struct Cyc_Absyn_Exp*)es->hd);Cyc_Tcutil_coerce_arg(
te,(struct Cyc_Absyn_Exp*)es->hd,(*((struct _tuple17*)_tmp315->hd)).f2);fields=({
struct Cyc_List_List*_tmp317=_cycalloc(sizeof(*_tmp317));_tmp317->hd=({struct
_tuple17*_tmp318=_cycalloc(sizeof(*_tmp318));_tmp318->f1=(*((struct _tuple17*)
_tmp315->hd)).f1;_tmp318->f2=(void*)((struct Cyc_Core_Opt*)_check_null(((struct
Cyc_Absyn_Exp*)es->hd)->topt))->v;_tmp318;});_tmp317->tl=fields;_tmp317;});}done=
1;goto _LL1BA;_LL1BD:;_LL1BE: goto _LL1BA;_LL1BA:;}if(!done)for(0;es != 0;es=es->tl){
Cyc_Tcexp_tcExpInitializer(te,0,(struct Cyc_Absyn_Exp*)es->hd);fields=({struct Cyc_List_List*
_tmp319=_cycalloc(sizeof(*_tmp319));_tmp319->hd=({struct _tuple17*_tmp31A=
_cycalloc(sizeof(*_tmp31A));_tmp31A->f1=Cyc_Absyn_empty_tqual(0);_tmp31A->f2=(
void*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Exp*)es->hd)->topt))->v;
_tmp31A;});_tmp319->tl=fields;_tmp319;});}return(void*)({struct Cyc_Absyn_TupleType_struct*
_tmp31B=_cycalloc(sizeof(*_tmp31B));_tmp31B[0]=({struct Cyc_Absyn_TupleType_struct
_tmp31C;_tmp31C.tag=9;_tmp31C.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(fields);_tmp31C;});_tmp31B;});}static void*Cyc_Tcexp_tcCompoundLit(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct _tuple8*
t,struct Cyc_List_List*des){return({void*_tmp31D=0;Cyc_Tcexp_expr_err(te,loc,topt,({
const char*_tmp31E="tcCompoundLit";_tag_dyneither(_tmp31E,sizeof(char),14);}),
_tag_dyneither(_tmp31D,sizeof(void*),0));});}static void*Cyc_Tcexp_tcArray(struct
Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**elt_topt,int zero_term,
struct Cyc_List_List*des){void*res_t2;struct _RegionHandle*_tmp31F=Cyc_Tcenv_get_fnrgn(
te);{int _tmp320=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(des);struct Cyc_List_List*
es=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Exp*(*f)(
struct _tuple16*),struct Cyc_List_List*x))Cyc_List_rmap)(_tmp31F,(struct Cyc_Absyn_Exp*(*)(
struct _tuple16*))Cyc_Core_snd,des);void*res=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_mk,Cyc_Tcenv_lookup_opt_type_vars(te));struct Cyc_Absyn_Const_e_struct*
_tmp321=({struct Cyc_Absyn_Const_e_struct*_tmp329=_cycalloc(sizeof(*_tmp329));
_tmp329[0]=({struct Cyc_Absyn_Const_e_struct _tmp32A;_tmp32A.tag=0;_tmp32A.f1=Cyc_Absyn_Int_c((
void*)1,_tmp320);_tmp32A;});_tmp329;});struct Cyc_Absyn_Exp*sz_exp=Cyc_Absyn_new_exp((
void*)_tmp321,loc);if(zero_term){struct Cyc_Absyn_Exp*_tmp322=((struct Cyc_Absyn_Exp*(*)(
struct Cyc_List_List*x,int n))Cyc_List_nth)(es,_tmp320 - 1);if(!Cyc_Tcutil_is_zero(
_tmp322))({void*_tmp323=0;Cyc_Tcutil_terr(_tmp322->loc,({const char*_tmp324="zero-terminated array doesn't end with zero.";
_tag_dyneither(_tmp324,sizeof(char),45);}),_tag_dyneither(_tmp323,sizeof(void*),
0));});}sz_exp->topt=({struct Cyc_Core_Opt*_tmp325=_cycalloc(sizeof(*_tmp325));
_tmp325->v=(void*)Cyc_Absyn_uint_typ;_tmp325;});res_t2=Cyc_Absyn_array_typ(res,
Cyc_Absyn_empty_tqual(0),(struct Cyc_Absyn_Exp*)sz_exp,zero_term?Cyc_Absyn_true_conref:
Cyc_Absyn_false_conref,0);{struct Cyc_List_List*es2=es;for(0;es2 != 0;es2=es2->tl){
Cyc_Tcexp_tcExpInitializer(te,elt_topt,(struct Cyc_Absyn_Exp*)es2->hd);}}if(!Cyc_Tcutil_coerce_list(
te,res,es))({struct Cyc_String_pa_struct _tmp328;_tmp328.tag=0;_tmp328.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(res));{void*_tmp326[
1]={& _tmp328};Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(es))->hd)->loc,({const char*_tmp327="elements of array do not all have the same type (%s)";
_tag_dyneither(_tmp327,sizeof(char),53);}),_tag_dyneither(_tmp326,sizeof(void*),
1));}});}{int offset=0;for(0;des != 0;(offset ++,des=des->tl)){struct Cyc_List_List*
ds=(*((struct _tuple16*)des->hd)).f1;if(ds != 0){void*_tmp32B=(void*)ds->hd;struct
Cyc_Absyn_Exp*_tmp32C;_LL1C0: if(*((int*)_tmp32B)!= 1)goto _LL1C2;_LL1C1:({void*
_tmp32D=0;Cyc_Tcutil_terr(loc,({const char*_tmp32E="only array index designators are supported";
_tag_dyneither(_tmp32E,sizeof(char),43);}),_tag_dyneither(_tmp32D,sizeof(void*),
0));});goto _LL1BF;_LL1C2: if(*((int*)_tmp32B)!= 0)goto _LL1BF;_tmp32C=((struct Cyc_Absyn_ArrayElement_struct*)
_tmp32B)->f1;_LL1C3: Cyc_Tcexp_tcExpInitializer(te,0,_tmp32C);{unsigned int
_tmp330;int _tmp331;struct _tuple13 _tmp32F=Cyc_Evexp_eval_const_uint_exp(_tmp32C);
_tmp330=_tmp32F.f1;_tmp331=_tmp32F.f2;if(!_tmp331)({void*_tmp332=0;Cyc_Tcutil_terr(
_tmp32C->loc,({const char*_tmp333="index designator cannot use sizeof or offsetof";
_tag_dyneither(_tmp333,sizeof(char),47);}),_tag_dyneither(_tmp332,sizeof(void*),
0));});else{if(_tmp330 != offset)({struct Cyc_Int_pa_struct _tmp337;_tmp337.tag=1;
_tmp337.f1=(unsigned long)((int)_tmp330);{struct Cyc_Int_pa_struct _tmp336;_tmp336.tag=
1;_tmp336.f1=(unsigned long)offset;{void*_tmp334[2]={& _tmp336,& _tmp337};Cyc_Tcutil_terr(
_tmp32C->loc,({const char*_tmp335="expecting index designator %d but found %d";
_tag_dyneither(_tmp335,sizeof(char),43);}),_tag_dyneither(_tmp334,sizeof(void*),
2));}}});}goto _LL1BF;}_LL1BF:;}}}return res_t2;}static void*Cyc_Tcexp_tcComprehension(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Vardecl*
vd,struct Cyc_Absyn_Exp*bound,struct Cyc_Absyn_Exp*body,int*is_zero_term){Cyc_Tcexp_tcExp(
te,0,bound);{void*_tmp338=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(bound->topt))->v);_LL1C5: if(_tmp338 <= (void*)4)goto _LL1C7;if(*((int*)
_tmp338)!= 18)goto _LL1C7;_LL1C6: goto _LL1C4;_LL1C7:;_LL1C8: if(!Cyc_Tcutil_coerce_uint_typ(
te,bound))({struct Cyc_String_pa_struct _tmp33B;_tmp33B.tag=0;_tmp33B.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(bound->topt))->v));{void*_tmp339[1]={& _tmp33B};Cyc_Tcutil_terr(bound->loc,({
const char*_tmp33A="expecting unsigned int, found %s";_tag_dyneither(_tmp33A,
sizeof(char),33);}),_tag_dyneither(_tmp339,sizeof(void*),1));}});_LL1C4:;}if(!(
vd->tq).real_const)({void*_tmp33C=0;((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp33D="comprehension index variable is not declared const!";
_tag_dyneither(_tmp33D,sizeof(char),52);}),_tag_dyneither(_tmp33C,sizeof(void*),
0));});{struct _RegionHandle*_tmp33E=Cyc_Tcenv_get_fnrgn(te);struct Cyc_Tcenv_Tenv*
te2=Cyc_Tcenv_copy_tenv(_tmp33E,te);if(te2->le != 0){te2=Cyc_Tcenv_new_block(
_tmp33E,loc,te2);te2=Cyc_Tcenv_add_local_var(_tmp33E,loc,te2,vd);}{struct Cyc_Tcenv_Tenv*
_tmp33F=te2;struct Cyc_Absyn_PtrInfo pinfo;void**_tmp340=0;struct Cyc_Absyn_Tqual*
_tmp341=0;union Cyc_Absyn_Constraint**_tmp342=0;if(topt != 0){void*_tmp343=Cyc_Tcutil_compress(*
topt);struct Cyc_Absyn_PtrInfo _tmp344;struct Cyc_Absyn_ArrayInfo _tmp345;void*
_tmp346;void**_tmp347;struct Cyc_Absyn_Tqual _tmp348;struct Cyc_Absyn_Tqual*_tmp349;
struct Cyc_Absyn_Exp*_tmp34A;union Cyc_Absyn_Constraint*_tmp34B;union Cyc_Absyn_Constraint**
_tmp34C;_LL1CA: if(_tmp343 <= (void*)4)goto _LL1CE;if(*((int*)_tmp343)!= 4)goto
_LL1CC;_tmp344=((struct Cyc_Absyn_PointerType_struct*)_tmp343)->f1;_LL1CB: pinfo=
_tmp344;_tmp340=(void**)& pinfo.elt_typ;_tmp341=(struct Cyc_Absyn_Tqual*)& pinfo.elt_tq;
_tmp342=(union Cyc_Absyn_Constraint**)&(pinfo.ptr_atts).zero_term;goto _LL1C9;
_LL1CC: if(*((int*)_tmp343)!= 7)goto _LL1CE;_tmp345=((struct Cyc_Absyn_ArrayType_struct*)
_tmp343)->f1;_tmp346=_tmp345.elt_type;_tmp347=(void**)&(((struct Cyc_Absyn_ArrayType_struct*)
_tmp343)->f1).elt_type;_tmp348=_tmp345.tq;_tmp349=(struct Cyc_Absyn_Tqual*)&(((
struct Cyc_Absyn_ArrayType_struct*)_tmp343)->f1).tq;_tmp34A=_tmp345.num_elts;
_tmp34B=_tmp345.zero_term;_tmp34C=(union Cyc_Absyn_Constraint**)&(((struct Cyc_Absyn_ArrayType_struct*)
_tmp343)->f1).zero_term;_LL1CD: _tmp340=(void**)_tmp347;_tmp341=(struct Cyc_Absyn_Tqual*)
_tmp349;_tmp342=(union Cyc_Absyn_Constraint**)_tmp34C;goto _LL1C9;_LL1CE:;_LL1CF:
goto _LL1C9;_LL1C9:;}{void*t=Cyc_Tcexp_tcExp(_tmp33F,_tmp340,body);if(_tmp33F->le
== 0){if(!Cyc_Tcutil_is_const_exp(_tmp33F,bound))({void*_tmp34D=0;Cyc_Tcutil_terr(
bound->loc,({const char*_tmp34E="bound is not constant";_tag_dyneither(_tmp34E,
sizeof(char),22);}),_tag_dyneither(_tmp34D,sizeof(void*),0));});if(!Cyc_Tcutil_is_const_exp(
_tmp33F,body))({void*_tmp34F=0;Cyc_Tcutil_terr(bound->loc,({const char*_tmp350="body is not constant";
_tag_dyneither(_tmp350,sizeof(char),21);}),_tag_dyneither(_tmp34F,sizeof(void*),
0));});}if(_tmp342 != 0  && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
0,*_tmp342)){struct Cyc_Absyn_Exp*_tmp351=Cyc_Absyn_uint_exp(1,0);_tmp351->topt=({
struct Cyc_Core_Opt*_tmp352=_cycalloc(sizeof(*_tmp352));_tmp352->v=(void*)Cyc_Absyn_uint_typ;
_tmp352;});bound=Cyc_Absyn_add_exp(bound,_tmp351,0);bound->topt=({struct Cyc_Core_Opt*
_tmp353=_cycalloc(sizeof(*_tmp353));_tmp353->v=(void*)Cyc_Absyn_uint_typ;_tmp353;});*
is_zero_term=1;}{void*_tmp354=Cyc_Absyn_array_typ(t,_tmp341 == 0?Cyc_Absyn_empty_tqual(
0):*_tmp341,(struct Cyc_Absyn_Exp*)bound,_tmp342 == 0?Cyc_Absyn_false_conref:*
_tmp342,0);return _tmp354;}}}}}struct _tuple18{struct Cyc_Absyn_Aggrfield*f1;struct
Cyc_Absyn_Exp*f2;};static void*Cyc_Tcexp_tcAggregate(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,void**topt,struct _tuple1**tn,struct Cyc_List_List**
ts,struct Cyc_List_List*args,struct Cyc_Absyn_Aggrdecl**ad_opt){struct Cyc_Absyn_Aggrdecl*
ad;if(*ad_opt != 0)ad=(struct Cyc_Absyn_Aggrdecl*)_check_null(*ad_opt);else{{
struct _handler_cons _tmp355;_push_handler(& _tmp355);{int _tmp357=0;if(setjmp(
_tmp355.handler))_tmp357=1;if(!_tmp357){ad=*Cyc_Tcenv_lookup_aggrdecl(te,loc,*tn);;
_pop_handler();}else{void*_tmp356=(void*)_exn_thrown;void*_tmp359=_tmp356;_LL1D1:
if(_tmp359 != Cyc_Dict_Absent)goto _LL1D3;_LL1D2:({struct Cyc_String_pa_struct
_tmp35C;_tmp35C.tag=0;_tmp35C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(*tn));{void*_tmp35A[1]={& _tmp35C};Cyc_Tcutil_terr(loc,({
const char*_tmp35B="unbound struct/union name %s";_tag_dyneither(_tmp35B,sizeof(
char),29);}),_tag_dyneither(_tmp35A,sizeof(void*),1));}});return topt != 0?*topt:(
void*)0;_LL1D3:;_LL1D4:(void)_throw(_tmp359);_LL1D0:;}}}*ad_opt=(struct Cyc_Absyn_Aggrdecl*)
ad;*tn=ad->name;}if(ad->impl == 0){({struct Cyc_String_pa_struct _tmp35F;_tmp35F.tag=
0;_tmp35F.f1=(struct _dyneither_ptr)(ad->kind == (void*)0?({const char*_tmp360="struct";
_tag_dyneither(_tmp360,sizeof(char),7);}):({const char*_tmp361="union";
_tag_dyneither(_tmp361,sizeof(char),6);}));{void*_tmp35D[1]={& _tmp35F};Cyc_Tcutil_terr(
loc,({const char*_tmp35E="can't construct abstract %s";_tag_dyneither(_tmp35E,
sizeof(char),28);}),_tag_dyneither(_tmp35D,sizeof(void*),1));}});return Cyc_Absyn_wildtyp(
Cyc_Tcenv_lookup_opt_type_vars(te));}{struct _RegionHandle*_tmp362=Cyc_Tcenv_get_fnrgn(
te);struct _tuple10 _tmp363=({struct _tuple10 _tmp384;_tmp384.f1=Cyc_Tcenv_lookup_type_vars(
te);_tmp384.f2=_tmp362;_tmp384;});struct Cyc_List_List*_tmp364=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _tuple11*(*f)(struct _tuple10*,struct Cyc_Absyn_Tvar*),
struct _tuple10*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(_tmp362,Cyc_Tcutil_r_make_inst_var,&
_tmp363,ad->tvs);struct Cyc_List_List*_tmp365=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _tuple11*(*f)(struct _tuple10*,struct Cyc_Absyn_Tvar*),struct
_tuple10*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(_tmp362,Cyc_Tcutil_r_make_inst_var,&
_tmp363,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars);
struct Cyc_List_List*_tmp366=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple11*),
struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple11*))Cyc_Core_snd,
_tmp364);struct Cyc_List_List*_tmp367=((struct Cyc_List_List*(*)(void*(*f)(struct
_tuple11*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple11*))Cyc_Core_snd,
_tmp365);struct Cyc_List_List*_tmp368=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(
_tmp362,_tmp364,_tmp365);struct Cyc_Absyn_AggrType_struct*_tmp369=({struct Cyc_Absyn_AggrType_struct*
_tmp380=_cycalloc(sizeof(*_tmp380));_tmp380[0]=({struct Cyc_Absyn_AggrType_struct
_tmp381;_tmp381.tag=10;_tmp381.f1=({struct Cyc_Absyn_AggrInfo _tmp382;_tmp382.aggr_info=
Cyc_Absyn_KnownAggr(({struct Cyc_Absyn_Aggrdecl**_tmp383=_cycalloc(sizeof(*
_tmp383));_tmp383[0]=ad;_tmp383;}));_tmp382.targs=_tmp366;_tmp382;});_tmp381;});
_tmp380;});struct Cyc_List_List*_tmp36A=*ts;struct Cyc_List_List*_tmp36B=_tmp367;
while(_tmp36A != 0  && _tmp36B != 0){Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(
te),(void*)0,1,(void*)_tmp36A->hd);Cyc_Tcutil_unify((void*)_tmp36A->hd,(void*)
_tmp36B->hd);_tmp36A=_tmp36A->tl;_tmp36B=_tmp36B->tl;}if(_tmp36A != 0)({void*
_tmp36C=0;Cyc_Tcutil_terr(loc,({const char*_tmp36D="too many explicit witness types";
_tag_dyneither(_tmp36D,sizeof(char),32);}),_tag_dyneither(_tmp36C,sizeof(void*),
0));});*ts=_tmp367;if(topt != 0)Cyc_Tcutil_unify((void*)_tmp369,*topt);{struct Cyc_List_List*
fields=((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,struct Cyc_Position_Segment*
loc,struct Cyc_List_List*des,void*,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(
_tmp362,loc,args,ad->kind,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields);
for(0;fields != 0;fields=fields->tl){struct _tuple18 _tmp36F;struct Cyc_Absyn_Aggrfield*
_tmp370;struct Cyc_Absyn_Exp*_tmp371;struct _tuple18*_tmp36E=(struct _tuple18*)
fields->hd;_tmp36F=*_tmp36E;_tmp370=_tmp36F.f1;_tmp371=_tmp36F.f2;{void*_tmp372=
Cyc_Tcutil_rsubstitute(_tmp362,_tmp368,_tmp370->type);Cyc_Tcexp_tcExpInitializer(
te,(void**)& _tmp372,_tmp371);if(!Cyc_Tcutil_coerce_arg(te,_tmp371,_tmp372)){({
struct Cyc_String_pa_struct _tmp379;_tmp379.tag=0;_tmp379.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp371->topt))->v));{struct Cyc_String_pa_struct _tmp378;_tmp378.tag=
0;_tmp378.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
_tmp372));{struct Cyc_String_pa_struct _tmp377;_tmp377.tag=0;_tmp377.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(*tn));{struct Cyc_String_pa_struct
_tmp376;_tmp376.tag=0;_tmp376.f1=(struct _dyneither_ptr)(ad->kind == (void*)0?({
const char*_tmp37A="struct";_tag_dyneither(_tmp37A,sizeof(char),7);}):({const char*
_tmp37B="union";_tag_dyneither(_tmp37B,sizeof(char),6);}));{struct Cyc_String_pa_struct
_tmp375;_tmp375.tag=0;_tmp375.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*
_tmp370->name);{void*_tmp373[5]={& _tmp375,& _tmp376,& _tmp377,& _tmp378,& _tmp379};
Cyc_Tcutil_terr(_tmp371->loc,({const char*_tmp374="field %s of %s %s expects type %s != %s";
_tag_dyneither(_tmp374,sizeof(char),40);}),_tag_dyneither(_tmp373,sizeof(void*),
5));}}}}}});Cyc_Tcutil_explain_failure();}}}{struct Cyc_List_List*_tmp37C=0;{
struct Cyc_List_List*_tmp37D=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po;
for(0;_tmp37D != 0;_tmp37D=_tmp37D->tl){_tmp37C=({struct Cyc_List_List*_tmp37E=
_cycalloc(sizeof(*_tmp37E));_tmp37E->hd=({struct _tuple0*_tmp37F=_cycalloc(
sizeof(*_tmp37F));_tmp37F->f1=Cyc_Tcutil_rsubstitute(_tmp362,_tmp368,(*((struct
_tuple0*)_tmp37D->hd)).f1);_tmp37F->f2=Cyc_Tcutil_rsubstitute(_tmp362,_tmp368,(*((
struct _tuple0*)_tmp37D->hd)).f2);_tmp37F;});_tmp37E->tl=_tmp37C;_tmp37E;});}}
_tmp37C=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmp37C);Cyc_Tcenv_check_rgn_partial_order(te,loc,_tmp37C);return(void*)_tmp369;}}}}
static void*Cyc_Tcexp_tcAnonStruct(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void*ts,struct Cyc_List_List*args){struct _RegionHandle*_tmp385=Cyc_Tcenv_get_fnrgn(
te);{void*_tmp386=Cyc_Tcutil_compress(ts);void*_tmp387;struct Cyc_List_List*
_tmp388;_LL1D6: if(_tmp386 <= (void*)4)goto _LL1D8;if(*((int*)_tmp386)!= 11)goto
_LL1D8;_tmp387=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)_tmp386)->f1;
_tmp388=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp386)->f2;_LL1D7: if(_tmp387 == (
void*)1)({void*_tmp389=0;Cyc_Tcutil_terr(loc,({const char*_tmp38A="expecting struct but found union";
_tag_dyneither(_tmp38A,sizeof(char),33);}),_tag_dyneither(_tmp389,sizeof(void*),
0));});{struct Cyc_List_List*fields=((struct Cyc_List_List*(*)(struct _RegionHandle*
rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,void*,struct Cyc_List_List*
fields))Cyc_Tcutil_resolve_aggregate_designators)(_tmp385,loc,args,(void*)0,
_tmp388);for(0;fields != 0;fields=fields->tl){struct _tuple18 _tmp38C;struct Cyc_Absyn_Aggrfield*
_tmp38D;struct Cyc_Absyn_Exp*_tmp38E;struct _tuple18*_tmp38B=(struct _tuple18*)
fields->hd;_tmp38C=*_tmp38B;_tmp38D=_tmp38C.f1;_tmp38E=_tmp38C.f2;Cyc_Tcexp_tcExpInitializer(
te,(void**)& _tmp38D->type,_tmp38E);if(!Cyc_Tcutil_coerce_arg(te,_tmp38E,_tmp38D->type)){({
struct Cyc_String_pa_struct _tmp393;_tmp393.tag=0;_tmp393.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp38E->topt))->v));{struct Cyc_String_pa_struct _tmp392;_tmp392.tag=
0;_tmp392.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
_tmp38D->type));{struct Cyc_String_pa_struct _tmp391;_tmp391.tag=0;_tmp391.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp38D->name);{void*_tmp38F[3]={&
_tmp391,& _tmp392,& _tmp393};Cyc_Tcutil_terr(_tmp38E->loc,({const char*_tmp390="field %s of struct expects type %s != %s";
_tag_dyneither(_tmp390,sizeof(char),41);}),_tag_dyneither(_tmp38F,sizeof(void*),
3));}}}});Cyc_Tcutil_explain_failure();}}goto _LL1D5;}_LL1D8:;_LL1D9:({void*
_tmp394=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp395="tcAnonStruct: wrong type";_tag_dyneither(_tmp395,sizeof(char),
25);}),_tag_dyneither(_tmp394,sizeof(void*),0));});_LL1D5:;}return ts;}static void*
Cyc_Tcexp_tcDatatype(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**
topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es,struct Cyc_Absyn_Datatypedecl*
tud,struct Cyc_Absyn_Datatypefield*tuf){struct _RegionHandle*_tmp396=Cyc_Tcenv_get_fnrgn(
te);struct _tuple10 _tmp397=({struct _tuple10 _tmp3B2;_tmp3B2.f1=Cyc_Tcenv_lookup_type_vars(
te);_tmp3B2.f2=_tmp396;_tmp3B2;});struct Cyc_List_List*_tmp398=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _tuple11*(*f)(struct _tuple10*,struct Cyc_Absyn_Tvar*),
struct _tuple10*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(_tmp396,Cyc_Tcutil_r_make_inst_var,&
_tmp397,tud->tvs);struct Cyc_List_List*_tmp399=((struct Cyc_List_List*(*)(void*(*f)(
struct _tuple11*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple11*))
Cyc_Core_snd,_tmp398);void*res=(void*)({struct Cyc_Absyn_DatatypeFieldType_struct*
_tmp3AF=_cycalloc(sizeof(*_tmp3AF));_tmp3AF[0]=({struct Cyc_Absyn_DatatypeFieldType_struct
_tmp3B0;_tmp3B0.tag=3;_tmp3B0.f1=({struct Cyc_Absyn_DatatypeFieldInfo _tmp3B1;
_tmp3B1.field_info=Cyc_Absyn_KnownDatatypefield(tud,tuf);_tmp3B1.targs=_tmp399;
_tmp3B1;});_tmp3B0;});_tmp3AF;});if(topt != 0){void*_tmp39A=Cyc_Tcutil_compress(*
topt);struct Cyc_Absyn_DatatypeInfo _tmp39B;struct Cyc_List_List*_tmp39C;struct Cyc_Core_Opt*
_tmp39D;_LL1DB: if(_tmp39A <= (void*)4)goto _LL1DF;if(*((int*)_tmp39A)!= 3)goto
_LL1DD;_LL1DC: Cyc_Tcutil_unify(*topt,res);goto _LL1DA;_LL1DD: if(*((int*)_tmp39A)
!= 2)goto _LL1DF;_tmp39B=((struct Cyc_Absyn_DatatypeType_struct*)_tmp39A)->f1;
_tmp39C=_tmp39B.targs;_tmp39D=_tmp39B.rgn;_LL1DE:{struct Cyc_List_List*a=_tmp399;
for(0;a != 0  && _tmp39C != 0;(a=a->tl,_tmp39C=_tmp39C->tl)){Cyc_Tcutil_unify((void*)
a->hd,(void*)_tmp39C->hd);}}if(tuf->typs == 0  && es == 0){e->topt=({struct Cyc_Core_Opt*
_tmp39E=_cycalloc(sizeof(*_tmp39E));_tmp39E->v=(void*)res;_tmp39E;});res=(void*)({
struct Cyc_Absyn_DatatypeType_struct*_tmp39F=_cycalloc(sizeof(*_tmp39F));_tmp39F[
0]=({struct Cyc_Absyn_DatatypeType_struct _tmp3A0;_tmp3A0.tag=2;_tmp3A0.f1=({
struct Cyc_Absyn_DatatypeInfo _tmp3A1;_tmp3A1.datatype_info=Cyc_Absyn_KnownDatatype(({
struct Cyc_Absyn_Datatypedecl**_tmp3A2=_cycalloc(sizeof(*_tmp3A2));_tmp3A2[0]=tud;
_tmp3A2;}));_tmp3A1.targs=_tmp399;_tmp3A1.rgn=_tmp39D;_tmp3A1;});_tmp3A0;});
_tmp39F;});Cyc_Tcutil_unchecked_cast(te,e,res,(void*)1);}goto _LL1DA;_LL1DF:;
_LL1E0: goto _LL1DA;_LL1DA:;}{struct Cyc_List_List*ts=tuf->typs;for(0;es != 0  && ts
!= 0;(es=es->tl,ts=ts->tl)){struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)es->hd;
void*t=Cyc_Tcutil_rsubstitute(_tmp396,_tmp398,(*((struct _tuple17*)ts->hd)).f2);
Cyc_Tcexp_tcExpInitializer(te,(void**)& t,e);if(!Cyc_Tcutil_coerce_arg(te,e,t)){({
struct Cyc_String_pa_struct _tmp3A7;_tmp3A7.tag=0;_tmp3A7.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)(e->topt == 0?(struct _dyneither_ptr)({const char*_tmp3A8="?";
_tag_dyneither(_tmp3A8,sizeof(char),2);}): Cyc_Absynpp_typ2string((void*)((struct
Cyc_Core_Opt*)_check_null(e->topt))->v)));{struct Cyc_String_pa_struct _tmp3A6;
_tmp3A6.tag=0;_tmp3A6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t));{struct Cyc_String_pa_struct _tmp3A5;_tmp3A5.tag=0;_tmp3A5.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(tuf->name));{void*
_tmp3A3[3]={& _tmp3A5,& _tmp3A6,& _tmp3A7};Cyc_Tcutil_terr(e->loc,({const char*
_tmp3A4="datatype constructor %s expects argument of type %s but this argument has type %s";
_tag_dyneither(_tmp3A4,sizeof(char),82);}),_tag_dyneither(_tmp3A3,sizeof(void*),
3));}}}});Cyc_Tcutil_explain_failure();}}if(es != 0)return({struct Cyc_String_pa_struct
_tmp3AB;_tmp3AB.tag=0;_tmp3AB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(tuf->name));{void*_tmp3A9[1]={& _tmp3AB};Cyc_Tcexp_expr_err(
te,((struct Cyc_Absyn_Exp*)es->hd)->loc,topt,({const char*_tmp3AA="too many arguments for datatype constructor %s";
_tag_dyneither(_tmp3AA,sizeof(char),47);}),_tag_dyneither(_tmp3A9,sizeof(void*),
1));}});if(ts != 0)return({struct Cyc_String_pa_struct _tmp3AE;_tmp3AE.tag=0;
_tmp3AE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
tuf->name));{void*_tmp3AC[1]={& _tmp3AE};Cyc_Tcexp_expr_err(te,loc,topt,({const
char*_tmp3AD="too few arguments for datatype constructor %s";_tag_dyneither(
_tmp3AD,sizeof(char),46);}),_tag_dyneither(_tmp3AC,sizeof(void*),1));}});return
res;}}static int Cyc_Tcexp_zeroable_type(void*t){void*_tmp3B3=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_PtrInfo _tmp3B4;struct Cyc_Absyn_PtrAtts _tmp3B5;union Cyc_Absyn_Constraint*
_tmp3B6;struct Cyc_Absyn_ArrayInfo _tmp3B7;void*_tmp3B8;struct Cyc_List_List*
_tmp3B9;struct Cyc_Absyn_AggrInfo _tmp3BA;union Cyc_Absyn_AggrInfoU _tmp3BB;struct
Cyc_List_List*_tmp3BC;struct Cyc_List_List*_tmp3BD;_LL1E2: if((int)_tmp3B3 != 0)
goto _LL1E4;_LL1E3: return 1;_LL1E4: if(_tmp3B3 <= (void*)4)goto _LL1F0;if(*((int*)
_tmp3B3)!= 0)goto _LL1E6;_LL1E5: goto _LL1E7;_LL1E6: if(*((int*)_tmp3B3)!= 1)goto
_LL1E8;_LL1E7: goto _LL1E9;_LL1E8: if(*((int*)_tmp3B3)!= 2)goto _LL1EA;_LL1E9: goto
_LL1EB;_LL1EA: if(*((int*)_tmp3B3)!= 3)goto _LL1EC;_LL1EB: return 0;_LL1EC: if(*((int*)
_tmp3B3)!= 4)goto _LL1EE;_tmp3B4=((struct Cyc_Absyn_PointerType_struct*)_tmp3B3)->f1;
_tmp3B5=_tmp3B4.ptr_atts;_tmp3B6=_tmp3B5.nullable;_LL1ED: return((int(*)(int y,
union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(1,_tmp3B6);_LL1EE: if(*((int*)
_tmp3B3)!= 5)goto _LL1F0;_LL1EF: goto _LL1F1;_LL1F0: if((int)_tmp3B3 != 1)goto _LL1F2;
_LL1F1: goto _LL1F3;_LL1F2: if(_tmp3B3 <= (void*)4)goto _LL20C;if(*((int*)_tmp3B3)!= 
6)goto _LL1F4;_LL1F3: return 1;_LL1F4: if(*((int*)_tmp3B3)!= 7)goto _LL1F6;_tmp3B7=((
struct Cyc_Absyn_ArrayType_struct*)_tmp3B3)->f1;_tmp3B8=_tmp3B7.elt_type;_LL1F5:
return Cyc_Tcexp_zeroable_type(_tmp3B8);_LL1F6: if(*((int*)_tmp3B3)!= 8)goto _LL1F8;
_LL1F7: return 0;_LL1F8: if(*((int*)_tmp3B3)!= 9)goto _LL1FA;_tmp3B9=((struct Cyc_Absyn_TupleType_struct*)
_tmp3B3)->f1;_LL1F9: for(0;(unsigned int)_tmp3B9;_tmp3B9=_tmp3B9->tl){if(!Cyc_Tcexp_zeroable_type((*((
struct _tuple17*)_tmp3B9->hd)).f2))return 0;}return 1;_LL1FA: if(*((int*)_tmp3B3)!= 
10)goto _LL1FC;_tmp3BA=((struct Cyc_Absyn_AggrType_struct*)_tmp3B3)->f1;_tmp3BB=
_tmp3BA.aggr_info;_tmp3BC=_tmp3BA.targs;_LL1FB: {struct Cyc_Absyn_Aggrdecl*
_tmp3BE=Cyc_Absyn_get_known_aggrdecl(_tmp3BB);if(_tmp3BE->impl == 0  || ((struct
Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp3BE->impl))->exist_vars != 0)return 0;{
struct _RegionHandle _tmp3BF=_new_region("r");struct _RegionHandle*r=& _tmp3BF;
_push_region(r);{struct Cyc_List_List*_tmp3C0=((struct Cyc_List_List*(*)(struct
_RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(r,r,_tmp3BE->tvs,_tmp3BC);{struct Cyc_List_List*fs=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp3BE->impl))->fields;for(0;fs != 0;fs=fs->tl){if(!Cyc_Tcexp_zeroable_type(
Cyc_Tcutil_rsubstitute(r,_tmp3C0,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))){
int _tmp3C1=0;_npop_handler(0);return _tmp3C1;}}}{int _tmp3C2=1;_npop_handler(0);
return _tmp3C2;}};_pop_region(r);}}_LL1FC: if(*((int*)_tmp3B3)!= 12)goto _LL1FE;
_LL1FD: return 1;_LL1FE: if(*((int*)_tmp3B3)!= 18)goto _LL200;_LL1FF: return 1;_LL200:
if(*((int*)_tmp3B3)!= 11)goto _LL202;_tmp3BD=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp3B3)->f2;_LL201: for(0;_tmp3BD != 0;_tmp3BD=_tmp3BD->tl){if(!Cyc_Tcexp_zeroable_type(((
struct Cyc_Absyn_Aggrfield*)_tmp3BD->hd)->type))return 0;}return 1;_LL202: if(*((int*)
_tmp3B3)!= 13)goto _LL204;_LL203: return 1;_LL204: if(*((int*)_tmp3B3)!= 16)goto
_LL206;_LL205: return 0;_LL206: if(*((int*)_tmp3B3)!= 15)goto _LL208;_LL207: return 0;
_LL208: if(*((int*)_tmp3B3)!= 14)goto _LL20A;_LL209: return 0;_LL20A: if(*((int*)
_tmp3B3)!= 17)goto _LL20C;_LL20B: goto _LL20D;_LL20C: if((int)_tmp3B3 != 2)goto _LL20E;
_LL20D: goto _LL20F;_LL20E: if((int)_tmp3B3 != 3)goto _LL210;_LL20F: goto _LL211;_LL210:
if(_tmp3B3 <= (void*)4)goto _LL212;if(*((int*)_tmp3B3)!= 19)goto _LL212;_LL211: goto
_LL213;_LL212: if(_tmp3B3 <= (void*)4)goto _LL214;if(*((int*)_tmp3B3)!= 20)goto
_LL214;_LL213: goto _LL215;_LL214: if(_tmp3B3 <= (void*)4)goto _LL1E1;if(*((int*)
_tmp3B3)!= 21)goto _LL1E1;_LL215:({struct Cyc_String_pa_struct _tmp3C5;_tmp3C5.tag=
0;_tmp3C5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t));{void*_tmp3C3[1]={& _tmp3C5};((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp3C4="bad type `%s' in zeroable type";
_tag_dyneither(_tmp3C4,sizeof(char),31);}),_tag_dyneither(_tmp3C3,sizeof(void*),
1));}});_LL1E1:;}static void Cyc_Tcexp_check_malloc_type(int allow_zero,struct Cyc_Position_Segment*
loc,void**topt,void*t){if(Cyc_Tcutil_bits_only(t) || allow_zero  && Cyc_Tcexp_zeroable_type(
t))return;if(topt != 0){void*_tmp3C6=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo
_tmp3C7;void*_tmp3C8;_LL217: if(_tmp3C6 <= (void*)4)goto _LL219;if(*((int*)_tmp3C6)
!= 4)goto _LL219;_tmp3C7=((struct Cyc_Absyn_PointerType_struct*)_tmp3C6)->f1;
_tmp3C8=_tmp3C7.elt_typ;_LL218: Cyc_Tcutil_unify(_tmp3C8,t);if(Cyc_Tcutil_bits_only(
t) || allow_zero  && Cyc_Tcexp_zeroable_type(t))return;goto _LL216;_LL219:;_LL21A:
goto _LL216;_LL216:;}({struct Cyc_String_pa_struct _tmp3CC;_tmp3CC.tag=0;_tmp3CC.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));{struct
Cyc_String_pa_struct _tmp3CB;_tmp3CB.tag=0;_tmp3CB.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)(allow_zero?(struct _dyneither_ptr)({const char*_tmp3CD="calloc";
_tag_dyneither(_tmp3CD,sizeof(char),7);}):(struct _dyneither_ptr)({const char*
_tmp3CE="malloc";_tag_dyneither(_tmp3CE,sizeof(char),7);})));{void*_tmp3C9[2]={&
_tmp3CB,& _tmp3CC};Cyc_Tcutil_terr(loc,({const char*_tmp3CA="%s cannot be used with type %s\n\t(type needs initialization)";
_tag_dyneither(_tmp3CA,sizeof(char),60);}),_tag_dyneither(_tmp3C9,sizeof(void*),
2));}}});}static void*Cyc_Tcexp_mallocRgn(void*rgn){void*_tmp3CF=Cyc_Tcutil_typ_kind(
Cyc_Tcutil_compress(rgn));_LL21C: if((int)_tmp3CF != 4)goto _LL21E;_LL21D: return(
void*)3;_LL21E:;_LL21F: return(void*)2;_LL21B:;}static void*Cyc_Tcexp_tcMalloc(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
ropt,void***t,struct Cyc_Absyn_Exp**e,int*is_calloc,int*is_fat){void*rgn=(void*)2;
if(ropt != 0){void*expected_type=(void*)({struct Cyc_Absyn_RgnHandleType_struct*
_tmp3D5=_cycalloc(sizeof(*_tmp3D5));_tmp3D5[0]=({struct Cyc_Absyn_RgnHandleType_struct
_tmp3D6;_tmp3D6.tag=14;_tmp3D6.f1=(void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_trk,Cyc_Tcenv_lookup_opt_type_vars(te));_tmp3D6;});_tmp3D5;});void*
handle_type=Cyc_Tcexp_tcExp(te,(void**)& expected_type,(struct Cyc_Absyn_Exp*)ropt);
void*_tmp3D0=Cyc_Tcutil_compress(handle_type);void*_tmp3D1;_LL221: if(_tmp3D0 <= (
void*)4)goto _LL223;if(*((int*)_tmp3D0)!= 14)goto _LL223;_tmp3D1=(void*)((struct
Cyc_Absyn_RgnHandleType_struct*)_tmp3D0)->f1;_LL222: rgn=_tmp3D1;Cyc_Tcenv_check_rgn_accessible(
te,loc,rgn);goto _LL220;_LL223:;_LL224:({struct Cyc_String_pa_struct _tmp3D4;
_tmp3D4.tag=0;_tmp3D4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
handle_type));{void*_tmp3D2[1]={& _tmp3D4};Cyc_Tcutil_terr(ropt->loc,({const char*
_tmp3D3="expecting region_t type but found %s";_tag_dyneither(_tmp3D3,sizeof(
char),37);}),_tag_dyneither(_tmp3D2,sizeof(void*),1));}});goto _LL220;_LL220:;}
else{if(topt != 0){void*optrgn=(void*)0;if(Cyc_Tcutil_rgn_of_pointer(*topt,&
optrgn))rgn=Cyc_Tcexp_mallocRgn(optrgn);}}Cyc_Tcexp_tcExp(te,(void**)& Cyc_Absyn_uint_typ,*
e);{void*elt_type;struct Cyc_Absyn_Exp*num_elts;int one_elt;if(*is_calloc){if(*t == 
0)({void*_tmp3D7=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp3D8="calloc with empty type";_tag_dyneither(_tmp3D8,sizeof(char),
23);}),_tag_dyneither(_tmp3D7,sizeof(void*),0));});elt_type=*((void**)
_check_null(*t));Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(te),(
void*)1,1,elt_type);Cyc_Tcexp_check_malloc_type(1,loc,topt,elt_type);num_elts=*e;
one_elt=0;}else{void*_tmp3D9=(*e)->r;void*_tmp3DA;void*_tmp3DB;struct Cyc_List_List*
_tmp3DC;struct Cyc_List_List _tmp3DD;struct Cyc_Absyn_Exp*_tmp3DE;struct Cyc_List_List*
_tmp3DF;struct Cyc_List_List _tmp3E0;struct Cyc_Absyn_Exp*_tmp3E1;struct Cyc_List_List*
_tmp3E2;_LL226: if(*((int*)_tmp3D9)!= 18)goto _LL228;_tmp3DA=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp3D9)->f1;_LL227: elt_type=_tmp3DA;*t=(void**)({void**_tmp3E3=_cycalloc(
sizeof(*_tmp3E3));_tmp3E3[0]=elt_type;_tmp3E3;});num_elts=Cyc_Absyn_uint_exp(1,0);
Cyc_Tcexp_tcExp(te,(void**)& Cyc_Absyn_uint_typ,num_elts);one_elt=1;goto _LL225;
_LL228: if(*((int*)_tmp3D9)!= 3)goto _LL22A;_tmp3DB=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp3D9)->f1;if((int)_tmp3DB != 1)goto _LL22A;_tmp3DC=((struct Cyc_Absyn_Primop_e_struct*)
_tmp3D9)->f2;if(_tmp3DC == 0)goto _LL22A;_tmp3DD=*_tmp3DC;_tmp3DE=(struct Cyc_Absyn_Exp*)
_tmp3DD.hd;_tmp3DF=_tmp3DD.tl;if(_tmp3DF == 0)goto _LL22A;_tmp3E0=*_tmp3DF;_tmp3E1=(
struct Cyc_Absyn_Exp*)_tmp3E0.hd;_tmp3E2=_tmp3E0.tl;if(_tmp3E2 != 0)goto _LL22A;
_LL229:{struct _tuple0 _tmp3E5=({struct _tuple0 _tmp3E4;_tmp3E4.f1=_tmp3DE->r;
_tmp3E4.f2=_tmp3E1->r;_tmp3E4;});void*_tmp3E6;void*_tmp3E7;void*_tmp3E8;void*
_tmp3E9;_LL22D: _tmp3E6=_tmp3E5.f1;if(*((int*)_tmp3E6)!= 18)goto _LL22F;_tmp3E7=(
void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp3E6)->f1;_LL22E: Cyc_Tcexp_check_malloc_type(
0,loc,topt,_tmp3E7);elt_type=_tmp3E7;*t=(void**)({void**_tmp3EA=_cycalloc(
sizeof(*_tmp3EA));_tmp3EA[0]=elt_type;_tmp3EA;});num_elts=_tmp3E1;one_elt=0;goto
_LL22C;_LL22F: _tmp3E8=_tmp3E5.f2;if(*((int*)_tmp3E8)!= 18)goto _LL231;_tmp3E9=(
void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp3E8)->f1;_LL230: Cyc_Tcexp_check_malloc_type(
0,loc,topt,_tmp3E9);elt_type=_tmp3E9;*t=(void**)({void**_tmp3EB=_cycalloc(
sizeof(*_tmp3EB));_tmp3EB[0]=elt_type;_tmp3EB;});num_elts=_tmp3DE;one_elt=0;goto
_LL22C;_LL231:;_LL232: goto No_sizeof;_LL22C:;}goto _LL225;_LL22A:;_LL22B: No_sizeof:
elt_type=Cyc_Absyn_char_typ;*t=(void**)({void**_tmp3EC=_cycalloc(sizeof(*_tmp3EC));
_tmp3EC[0]=elt_type;_tmp3EC;});num_elts=*e;one_elt=0;goto _LL225;_LL225:;}*e=
num_elts;*is_fat=!one_elt;{void*_tmp3ED=elt_type;struct Cyc_Absyn_AggrInfo _tmp3EE;
union Cyc_Absyn_AggrInfoU _tmp3EF;struct Cyc_Absyn_Aggrdecl**_tmp3F0;struct Cyc_Absyn_Aggrdecl*
_tmp3F1;_LL234: if(_tmp3ED <= (void*)4)goto _LL236;if(*((int*)_tmp3ED)!= 10)goto
_LL236;_tmp3EE=((struct Cyc_Absyn_AggrType_struct*)_tmp3ED)->f1;_tmp3EF=_tmp3EE.aggr_info;
if((_tmp3EF.KnownAggr).tag != 2)goto _LL236;_tmp3F0=(struct Cyc_Absyn_Aggrdecl**)(
_tmp3EF.KnownAggr).val;_tmp3F1=*_tmp3F0;_LL235: if(_tmp3F1->impl != 0  && ((struct
Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp3F1->impl))->exist_vars != 0)({void*
_tmp3F2=0;Cyc_Tcutil_terr(loc,({const char*_tmp3F3="malloc with existential types not yet implemented";
_tag_dyneither(_tmp3F3,sizeof(char),50);}),_tag_dyneither(_tmp3F2,sizeof(void*),
0));});goto _LL233;_LL236:;_LL237: goto _LL233;_LL233:;}{void*(*_tmp3F4)(void*t,
void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term)=Cyc_Absyn_at_typ;
union Cyc_Absyn_Constraint*_tmp3F5=Cyc_Absyn_false_conref;if(topt != 0){void*
_tmp3F6=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo _tmp3F7;struct Cyc_Absyn_PtrAtts
_tmp3F8;union Cyc_Absyn_Constraint*_tmp3F9;union Cyc_Absyn_Constraint*_tmp3FA;
union Cyc_Absyn_Constraint*_tmp3FB;_LL239: if(_tmp3F6 <= (void*)4)goto _LL23B;if(*((
int*)_tmp3F6)!= 4)goto _LL23B;_tmp3F7=((struct Cyc_Absyn_PointerType_struct*)
_tmp3F6)->f1;_tmp3F8=_tmp3F7.ptr_atts;_tmp3F9=_tmp3F8.nullable;_tmp3FA=_tmp3F8.bounds;
_tmp3FB=_tmp3F8.zero_term;_LL23A: _tmp3F5=_tmp3FB;if(((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,_tmp3F9))_tmp3F4=Cyc_Absyn_star_typ;if(((int(*)(int y,
union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp3FB) && !(*is_calloc)){({
void*_tmp3FC=0;Cyc_Tcutil_warn(loc,({const char*_tmp3FD="converting malloc to calloc to ensure zero-termination";
_tag_dyneither(_tmp3FD,sizeof(char),55);}),_tag_dyneither(_tmp3FC,sizeof(void*),
0));});*is_calloc=1;}{void*_tmp3FE=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,
_tmp3FA);struct Cyc_Absyn_Exp*_tmp3FF;_LL23E: if((int)_tmp3FE != 0)goto _LL240;
_LL23F: goto _LL23D;_LL240: if(_tmp3FE <= (void*)1)goto _LL242;if(*((int*)_tmp3FE)!= 
0)goto _LL242;_tmp3FF=((struct Cyc_Absyn_Upper_b_struct*)_tmp3FE)->f1;if(!(!
one_elt))goto _LL242;_LL241: {int _tmp400=Cyc_Evexp_c_can_eval(num_elts);if(
_tmp400  && Cyc_Evexp_same_const_exp(_tmp3FF,num_elts)){*is_fat=0;return Cyc_Absyn_atb_typ(
elt_type,rgn,Cyc_Absyn_empty_tqual(0),Cyc_Absyn_conref_val(_tmp3FA),_tmp3F5);}{
void*_tmp401=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
num_elts->topt))->v);void*_tmp402;_LL245: if(_tmp401 <= (void*)4)goto _LL247;if(*((
int*)_tmp401)!= 18)goto _LL247;_tmp402=(void*)((struct Cyc_Absyn_TagType_struct*)
_tmp401)->f1;_LL246: {struct Cyc_Absyn_Exp*_tmp403=Cyc_Absyn_cast_exp(Cyc_Absyn_uint_typ,
Cyc_Absyn_valueof_exp(_tmp402,0),0,(void*)1,0);if(Cyc_Evexp_same_const_exp(
_tmp403,_tmp3FF)){*is_fat=0;return Cyc_Absyn_atb_typ(elt_type,rgn,Cyc_Absyn_empty_tqual(
0),Cyc_Absyn_conref_val(_tmp3FA),_tmp3F5);}goto _LL244;}_LL247:;_LL248: goto _LL244;
_LL244:;}goto _LL23D;}_LL242:;_LL243: goto _LL23D;_LL23D:;}goto _LL238;_LL23B:;
_LL23C: goto _LL238;_LL238:;}if(!one_elt)_tmp3F4=Cyc_Absyn_dyneither_typ;return
_tmp3F4(elt_type,rgn,Cyc_Absyn_empty_tqual(0),_tmp3F5);}}}static void*Cyc_Tcexp_tcSwap(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
e1,struct Cyc_Absyn_Exp*e2){Cyc_Tcexp_tcExp(te,0,e1);Cyc_Tcexp_tcExp(te,(void**)((
void**)((void*)&((struct Cyc_Core_Opt*)_check_null(e1->topt))->v)),e2);{void*t1=(
void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v;void*t2=(void*)((struct Cyc_Core_Opt*)
_check_null(e2->topt))->v;{void*_tmp404=Cyc_Tcutil_compress(t1);_LL24A: if(
_tmp404 <= (void*)4)goto _LL24C;if(*((int*)_tmp404)!= 7)goto _LL24C;_LL24B:({void*
_tmp405=0;Cyc_Tcutil_terr(loc,({const char*_tmp406="cannot assign to an array";
_tag_dyneither(_tmp406,sizeof(char),26);}),_tag_dyneither(_tmp405,sizeof(void*),
0));});goto _LL249;_LL24C:;_LL24D: goto _LL249;_LL249:;}{int ign_1=0;if(!Cyc_Tcutil_is_pointer_or_boxed(
t1,& ign_1))({void*_tmp407=0;Cyc_Tcutil_terr(loc,({const char*_tmp408="Swap not allowed for non-pointer or non-word-sized types.";
_tag_dyneither(_tmp408,sizeof(char),58);}),_tag_dyneither(_tmp407,sizeof(void*),
0));});if(!Cyc_Absyn_is_lvalue(e1))return({void*_tmp409=0;Cyc_Tcexp_expr_err(te,
e1->loc,topt,({const char*_tmp40A="swap non-lvalue";_tag_dyneither(_tmp40A,
sizeof(char),16);}),_tag_dyneither(_tmp409,sizeof(void*),0));});if(!Cyc_Absyn_is_lvalue(
e2))return({void*_tmp40B=0;Cyc_Tcexp_expr_err(te,e2->loc,topt,({const char*
_tmp40C="swap non-lvalue";_tag_dyneither(_tmp40C,sizeof(char),16);}),
_tag_dyneither(_tmp40B,sizeof(void*),0));});{void*t_ign1=(void*)0;void*t_ign2=(
void*)0;int b_ign1=0;if(Cyc_Tcutil_is_zero_ptr_deref(e1,& t_ign1,& b_ign1,& t_ign2))
return({void*_tmp40D=0;Cyc_Tcexp_expr_err(te,e1->loc,topt,({const char*_tmp40E="swap value in zeroterm array";
_tag_dyneither(_tmp40E,sizeof(char),29);}),_tag_dyneither(_tmp40D,sizeof(void*),
0));});if(Cyc_Tcutil_is_zero_ptr_deref(e2,& t_ign1,& b_ign1,& t_ign2))return({void*
_tmp40F=0;Cyc_Tcexp_expr_err(te,e2->loc,topt,({const char*_tmp410="swap value in zeroterm array";
_tag_dyneither(_tmp410,sizeof(char),29);}),_tag_dyneither(_tmp40F,sizeof(void*),
0));});Cyc_Tcexp_check_writable(te,e1);Cyc_Tcexp_check_writable(te,e2);if(!Cyc_Tcutil_unify(
t1,t2)){void*_tmp411=({struct Cyc_String_pa_struct _tmp415;_tmp415.tag=0;_tmp415.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2));{struct
Cyc_String_pa_struct _tmp414;_tmp414.tag=0;_tmp414.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));{void*_tmp412[2]={& _tmp414,&
_tmp415};Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp413="type mismatch: %s != %s";
_tag_dyneither(_tmp413,sizeof(char),24);}),_tag_dyneither(_tmp412,sizeof(void*),
2));}}});return _tmp411;}return(void*)0;}}}}int Cyc_Tcexp_in_stmt_exp=0;static void*
Cyc_Tcexp_tcStmtExp(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**
topt,struct Cyc_Absyn_Stmt*s){struct _RegionHandle*_tmp416=Cyc_Tcenv_get_fnrgn(te);{
int old_stmt_exp_state=Cyc_Tcexp_in_stmt_exp;Cyc_Tcstmt_tcStmt(Cyc_Tcenv_set_encloser(
_tmp416,te,s),s,1);Cyc_Tcexp_in_stmt_exp=old_stmt_exp_state;}Cyc_NewControlFlow_set_encloser(
s,Cyc_Tcenv_get_encloser(te));while(1){void*_tmp417=s->r;struct Cyc_Absyn_Exp*
_tmp418;struct Cyc_Absyn_Stmt*_tmp419;struct Cyc_Absyn_Stmt*_tmp41A;struct Cyc_Absyn_Decl*
_tmp41B;struct Cyc_Absyn_Stmt*_tmp41C;_LL24F: if(_tmp417 <= (void*)1)goto _LL255;if(*((
int*)_tmp417)!= 0)goto _LL251;_tmp418=((struct Cyc_Absyn_Exp_s_struct*)_tmp417)->f1;
_LL250: return(void*)((struct Cyc_Core_Opt*)_check_null(_tmp418->topt))->v;_LL251:
if(*((int*)_tmp417)!= 1)goto _LL253;_tmp419=((struct Cyc_Absyn_Seq_s_struct*)
_tmp417)->f1;_tmp41A=((struct Cyc_Absyn_Seq_s_struct*)_tmp417)->f2;_LL252: s=
_tmp41A;continue;_LL253: if(*((int*)_tmp417)!= 11)goto _LL255;_tmp41B=((struct Cyc_Absyn_Decl_s_struct*)
_tmp417)->f1;_tmp41C=((struct Cyc_Absyn_Decl_s_struct*)_tmp417)->f2;_LL254: s=
_tmp41C;continue;_LL255:;_LL256: return({void*_tmp41D=0;Cyc_Tcexp_expr_err(te,loc,
topt,({const char*_tmp41E="statement expression must end with expression";
_tag_dyneither(_tmp41E,sizeof(char),46);}),_tag_dyneither(_tmp41D,sizeof(void*),
0));});_LL24E:;}}static void*Cyc_Tcexp_tcTagcheck(struct Cyc_Tcenv_Tenv*te,struct
Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*f){
void*t=Cyc_Tcutil_compress(Cyc_Tcexp_tcExp(te,0,e));{void*_tmp41F=t;struct Cyc_Absyn_AggrInfo
_tmp420;union Cyc_Absyn_AggrInfoU _tmp421;struct Cyc_Absyn_Aggrdecl**_tmp422;struct
Cyc_Absyn_Aggrdecl*_tmp423;_LL258: if(_tmp41F <= (void*)4)goto _LL25A;if(*((int*)
_tmp41F)!= 10)goto _LL25A;_tmp420=((struct Cyc_Absyn_AggrType_struct*)_tmp41F)->f1;
_tmp421=_tmp420.aggr_info;if((_tmp421.KnownAggr).tag != 2)goto _LL25A;_tmp422=(
struct Cyc_Absyn_Aggrdecl**)(_tmp421.KnownAggr).val;_tmp423=*_tmp422;_LL259: if((
_tmp423->kind == (void*)1  && _tmp423->impl != 0) && ((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp423->impl))->tagged)goto _LL257;goto _LL25B;_LL25A:;_LL25B:({
struct Cyc_String_pa_struct _tmp426;_tmp426.tag=0;_tmp426.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));{void*_tmp424[1]={& _tmp426};Cyc_Tcutil_terr(
loc,({const char*_tmp425="expecting @tagged union but found %s";_tag_dyneither(
_tmp425,sizeof(char),37);}),_tag_dyneither(_tmp424,sizeof(void*),1));}});goto
_LL257;_LL257:;}return Cyc_Absyn_uint_typ;}static void*Cyc_Tcexp_tcNew(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*rgn_handle,
struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1){void*rgn=(void*)2;if(rgn_handle != 
0){void*expected_type=(void*)({struct Cyc_Absyn_RgnHandleType_struct*_tmp42C=
_cycalloc(sizeof(*_tmp42C));_tmp42C[0]=({struct Cyc_Absyn_RgnHandleType_struct
_tmp42D;_tmp42D.tag=14;_tmp42D.f1=(void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_trk,Cyc_Tcenv_lookup_opt_type_vars(te));_tmp42D;});_tmp42C;});void*
handle_type=Cyc_Tcexp_tcExp(te,(void**)& expected_type,(struct Cyc_Absyn_Exp*)
rgn_handle);void*_tmp427=Cyc_Tcutil_compress(handle_type);void*_tmp428;_LL25D:
if(_tmp427 <= (void*)4)goto _LL25F;if(*((int*)_tmp427)!= 14)goto _LL25F;_tmp428=(
void*)((struct Cyc_Absyn_RgnHandleType_struct*)_tmp427)->f1;_LL25E: rgn=_tmp428;
Cyc_Tcenv_check_rgn_accessible(te,loc,rgn);goto _LL25C;_LL25F:;_LL260:({struct Cyc_String_pa_struct
_tmp42B;_tmp42B.tag=0;_tmp42B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(handle_type));{void*_tmp429[1]={& _tmp42B};Cyc_Tcutil_terr(
rgn_handle->loc,({const char*_tmp42A="expecting region_t type but found %s";
_tag_dyneither(_tmp42A,sizeof(char),37);}),_tag_dyneither(_tmp429,sizeof(void*),
1));}});goto _LL25C;_LL25C:;}else{if(topt != 0){void*optrgn=(void*)0;if(Cyc_Tcutil_rgn_of_pointer(*
topt,& optrgn))rgn=Cyc_Tcexp_mallocRgn(optrgn);}}{void*_tmp42E=e1->r;struct Cyc_Core_Opt*
_tmp42F;struct Cyc_List_List*_tmp430;struct Cyc_List_List*_tmp431;union Cyc_Absyn_Cnst
_tmp432;struct _dyneither_ptr _tmp433;_LL262: if(*((int*)_tmp42E)!= 29)goto _LL264;
_LL263: {void*_tmp434=Cyc_Tcexp_tcExpNoPromote(te,topt,e1);void*_tmp435=Cyc_Tcutil_compress(
_tmp434);struct Cyc_Absyn_ArrayInfo _tmp436;void*_tmp437;struct Cyc_Absyn_Tqual
_tmp438;struct Cyc_Absyn_Exp*_tmp439;union Cyc_Absyn_Constraint*_tmp43A;_LL26D: if(
_tmp435 <= (void*)4)goto _LL26F;if(*((int*)_tmp435)!= 7)goto _LL26F;_tmp436=((
struct Cyc_Absyn_ArrayType_struct*)_tmp435)->f1;_tmp437=_tmp436.elt_type;_tmp438=
_tmp436.tq;_tmp439=_tmp436.num_elts;_tmp43A=_tmp436.zero_term;_LL26E: {struct Cyc_Absyn_Exp*
bnd=(struct Cyc_Absyn_Exp*)_check_null(_tmp439);void*b;{void*_tmp43B=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(bnd->topt))->v);void*_tmp43C;_LL272: if(
_tmp43B <= (void*)4)goto _LL274;if(*((int*)_tmp43B)!= 18)goto _LL274;_tmp43C=(void*)((
struct Cyc_Absyn_TagType_struct*)_tmp43B)->f1;_LL273: b=(void*)({struct Cyc_Absyn_Upper_b_struct*
_tmp43D=_cycalloc(sizeof(*_tmp43D));_tmp43D[0]=({struct Cyc_Absyn_Upper_b_struct
_tmp43E;_tmp43E.tag=0;_tmp43E.f1=Cyc_Absyn_cast_exp(Cyc_Absyn_uint_typ,Cyc_Absyn_valueof_exp(
_tmp43C,0),0,(void*)1,0);_tmp43E;});_tmp43D;});goto _LL271;_LL274:;_LL275: if(Cyc_Tcutil_is_const_exp(
te,bnd))b=(void*)({struct Cyc_Absyn_Upper_b_struct*_tmp43F=_cycalloc(sizeof(*
_tmp43F));_tmp43F[0]=({struct Cyc_Absyn_Upper_b_struct _tmp440;_tmp440.tag=0;
_tmp440.f1=bnd;_tmp440;});_tmp43F;});else{b=(void*)0;}_LL271:;}{void*res_typ=(
void*)({struct Cyc_Absyn_PointerType_struct*_tmp442=_cycalloc(sizeof(*_tmp442));
_tmp442[0]=({struct Cyc_Absyn_PointerType_struct _tmp443;_tmp443.tag=4;_tmp443.f1=({
struct Cyc_Absyn_PtrInfo _tmp444;_tmp444.elt_typ=_tmp437;_tmp444.elt_tq=_tmp438;
_tmp444.ptr_atts=({struct Cyc_Absyn_PtrAtts _tmp445;_tmp445.rgn=rgn;_tmp445.nullable=((
union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();_tmp445.bounds=Cyc_Absyn_new_conref(
b);_tmp445.zero_term=_tmp43A;_tmp445.ptrloc=0;_tmp445;});_tmp444;});_tmp443;});
_tmp442;});if(topt != 0){if(!Cyc_Tcutil_unify(*topt,res_typ) && Cyc_Tcutil_silent_castable(
te,loc,res_typ,*topt)){e->topt=({struct Cyc_Core_Opt*_tmp441=_cycalloc(sizeof(*
_tmp441));_tmp441->v=(void*)res_typ;_tmp441;});Cyc_Tcutil_unchecked_cast(te,e,*
topt,(void*)3);res_typ=*topt;}}return res_typ;}}_LL26F:;_LL270:({void*_tmp446=0;((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const
char*_tmp447="tcNew: comprehension returned non-array type";_tag_dyneither(
_tmp447,sizeof(char),45);}),_tag_dyneither(_tmp446,sizeof(void*),0));});_LL26C:;}
_LL264: if(*((int*)_tmp42E)!= 37)goto _LL266;_tmp42F=((struct Cyc_Absyn_UnresolvedMem_e_struct*)
_tmp42E)->f1;_tmp430=((struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp42E)->f2;
_LL265: e1->r=(void*)({struct Cyc_Absyn_Array_e_struct*_tmp448=_cycalloc(sizeof(*
_tmp448));_tmp448[0]=({struct Cyc_Absyn_Array_e_struct _tmp449;_tmp449.tag=28;
_tmp449.f1=_tmp430;_tmp449;});_tmp448;});_tmp431=_tmp430;goto _LL267;_LL266: if(*((
int*)_tmp42E)!= 28)goto _LL268;_tmp431=((struct Cyc_Absyn_Array_e_struct*)_tmp42E)->f1;
_LL267: {void**elt_typ_opt=0;int zero_term=0;if(topt != 0){void*_tmp44A=Cyc_Tcutil_compress(*
topt);struct Cyc_Absyn_PtrInfo _tmp44B;void*_tmp44C;void**_tmp44D;struct Cyc_Absyn_Tqual
_tmp44E;struct Cyc_Absyn_PtrAtts _tmp44F;union Cyc_Absyn_Constraint*_tmp450;_LL277:
if(_tmp44A <= (void*)4)goto _LL279;if(*((int*)_tmp44A)!= 4)goto _LL279;_tmp44B=((
struct Cyc_Absyn_PointerType_struct*)_tmp44A)->f1;_tmp44C=_tmp44B.elt_typ;_tmp44D=(
void**)&(((struct Cyc_Absyn_PointerType_struct*)_tmp44A)->f1).elt_typ;_tmp44E=
_tmp44B.elt_tq;_tmp44F=_tmp44B.ptr_atts;_tmp450=_tmp44F.zero_term;_LL278:
elt_typ_opt=(void**)_tmp44D;zero_term=((int(*)(int y,union Cyc_Absyn_Constraint*x))
Cyc_Absyn_conref_def)(0,_tmp450);goto _LL276;_LL279:;_LL27A: goto _LL276;_LL276:;}{
void*_tmp451=Cyc_Tcexp_tcArray(te,e1->loc,elt_typ_opt,zero_term,_tmp431);e1->topt=({
struct Cyc_Core_Opt*_tmp452=_cycalloc(sizeof(*_tmp452));_tmp452->v=(void*)_tmp451;
_tmp452;});{void*res_typ;{void*_tmp453=Cyc_Tcutil_compress(_tmp451);struct Cyc_Absyn_ArrayInfo
_tmp454;void*_tmp455;struct Cyc_Absyn_Tqual _tmp456;struct Cyc_Absyn_Exp*_tmp457;
union Cyc_Absyn_Constraint*_tmp458;_LL27C: if(_tmp453 <= (void*)4)goto _LL27E;if(*((
int*)_tmp453)!= 7)goto _LL27E;_tmp454=((struct Cyc_Absyn_ArrayType_struct*)_tmp453)->f1;
_tmp455=_tmp454.elt_type;_tmp456=_tmp454.tq;_tmp457=_tmp454.num_elts;_tmp458=
_tmp454.zero_term;_LL27D: res_typ=(void*)({struct Cyc_Absyn_PointerType_struct*
_tmp459=_cycalloc(sizeof(*_tmp459));_tmp459[0]=({struct Cyc_Absyn_PointerType_struct
_tmp45A;_tmp45A.tag=4;_tmp45A.f1=({struct Cyc_Absyn_PtrInfo _tmp45B;_tmp45B.elt_typ=
_tmp455;_tmp45B.elt_tq=_tmp456;_tmp45B.ptr_atts=({struct Cyc_Absyn_PtrAtts _tmp45C;
_tmp45C.rgn=rgn;_tmp45C.nullable=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();
_tmp45C.bounds=Cyc_Absyn_new_conref((void*)({struct Cyc_Absyn_Upper_b_struct*
_tmp45D=_cycalloc(sizeof(*_tmp45D));_tmp45D[0]=({struct Cyc_Absyn_Upper_b_struct
_tmp45E;_tmp45E.tag=0;_tmp45E.f1=(struct Cyc_Absyn_Exp*)_check_null(_tmp457);
_tmp45E;});_tmp45D;}));_tmp45C.zero_term=_tmp458;_tmp45C.ptrloc=0;_tmp45C;});
_tmp45B;});_tmp45A;});_tmp459;});if(topt != 0){if(!Cyc_Tcutil_unify(*topt,res_typ)
 && Cyc_Tcutil_silent_castable(te,loc,res_typ,*topt)){e->topt=({struct Cyc_Core_Opt*
_tmp45F=_cycalloc(sizeof(*_tmp45F));_tmp45F->v=(void*)res_typ;_tmp45F;});Cyc_Tcutil_unchecked_cast(
te,e,*topt,(void*)3);res_typ=*topt;}}goto _LL27B;_LL27E:;_LL27F:({void*_tmp460=0;((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const
char*_tmp461="tcExpNoPromote on Array_e returned non-array type";_tag_dyneither(
_tmp461,sizeof(char),50);}),_tag_dyneither(_tmp460,sizeof(void*),0));});_LL27B:;}
return res_typ;}}}_LL268: if(*((int*)_tmp42E)!= 0)goto _LL26A;_tmp432=((struct Cyc_Absyn_Const_e_struct*)
_tmp42E)->f1;if((_tmp432.String_c).tag != 7)goto _LL26A;_tmp433=(struct
_dyneither_ptr)(_tmp432.String_c).val;_LL269: {void*_tmp462=Cyc_Absyn_atb_typ(
Cyc_Absyn_char_typ,rgn,Cyc_Absyn_const_tqual(0),(void*)0,Cyc_Absyn_true_conref);
void*_tmp463=Cyc_Tcexp_tcExp(te,(void**)& _tmp462,e1);return Cyc_Absyn_atb_typ(
_tmp463,rgn,Cyc_Absyn_empty_tqual(0),(void*)({struct Cyc_Absyn_Upper_b_struct*
_tmp464=_cycalloc(sizeof(*_tmp464));_tmp464[0]=({struct Cyc_Absyn_Upper_b_struct
_tmp465;_tmp465.tag=0;_tmp465.f1=Cyc_Absyn_uint_exp(1,0);_tmp465;});_tmp464;}),
Cyc_Absyn_false_conref);}_LL26A:;_LL26B: {void**topt2=0;if(topt != 0){void*
_tmp466=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo _tmp467;void*_tmp468;
void**_tmp469;struct Cyc_Absyn_Tqual _tmp46A;_LL281: if(_tmp466 <= (void*)4)goto
_LL285;if(*((int*)_tmp466)!= 4)goto _LL283;_tmp467=((struct Cyc_Absyn_PointerType_struct*)
_tmp466)->f1;_tmp468=_tmp467.elt_typ;_tmp469=(void**)&(((struct Cyc_Absyn_PointerType_struct*)
_tmp466)->f1).elt_typ;_tmp46A=_tmp467.elt_tq;_LL282: topt2=(void**)_tmp469;goto
_LL280;_LL283: if(*((int*)_tmp466)!= 2)goto _LL285;_LL284: topt2=({void**_tmp46B=
_cycalloc(sizeof(*_tmp46B));_tmp46B[0]=*topt;_tmp46B;});goto _LL280;_LL285:;
_LL286: goto _LL280;_LL280:;}{void*telt=Cyc_Tcexp_tcExp(te,topt2,e1);void*res_typ=(
void*)({struct Cyc_Absyn_PointerType_struct*_tmp46D=_cycalloc(sizeof(*_tmp46D));
_tmp46D[0]=({struct Cyc_Absyn_PointerType_struct _tmp46E;_tmp46E.tag=4;_tmp46E.f1=({
struct Cyc_Absyn_PtrInfo _tmp46F;_tmp46F.elt_typ=telt;_tmp46F.elt_tq=Cyc_Absyn_empty_tqual(
0);_tmp46F.ptr_atts=({struct Cyc_Absyn_PtrAtts _tmp470;_tmp470.rgn=rgn;_tmp470.nullable=((
union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();_tmp470.bounds=Cyc_Absyn_bounds_one_conref;
_tmp470.zero_term=Cyc_Absyn_false_conref;_tmp470.ptrloc=0;_tmp470;});_tmp46F;});
_tmp46E;});_tmp46D;});if(topt != 0){if(!Cyc_Tcutil_unify(*topt,res_typ) && Cyc_Tcutil_silent_castable(
te,loc,res_typ,*topt)){e->topt=({struct Cyc_Core_Opt*_tmp46C=_cycalloc(sizeof(*
_tmp46C));_tmp46C->v=(void*)res_typ;_tmp46C;});Cyc_Tcutil_unchecked_cast(te,e,*
topt,(void*)3);res_typ=*topt;}}return res_typ;}}_LL261:;}}void*Cyc_Tcexp_tcExp(
struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){void*t=Cyc_Tcutil_compress(
Cyc_Tcexp_tcExpNoPromote(te,topt,e));void*_tmp471=t;struct Cyc_Absyn_ArrayInfo
_tmp472;void*_tmp473;struct Cyc_Absyn_Tqual _tmp474;struct Cyc_Absyn_Exp*_tmp475;
union Cyc_Absyn_Constraint*_tmp476;_LL288: if(_tmp471 <= (void*)4)goto _LL28A;if(*((
int*)_tmp471)!= 7)goto _LL28A;_tmp472=((struct Cyc_Absyn_ArrayType_struct*)_tmp471)->f1;
_tmp473=_tmp472.elt_type;_tmp474=_tmp472.tq;_tmp475=_tmp472.num_elts;_tmp476=
_tmp472.zero_term;_LL289: {void*_tmp478;struct _tuple12 _tmp477=Cyc_Tcutil_addressof_props(
te,e);_tmp478=_tmp477.f2;{void*_tmp479=_tmp475 == 0?(void*)((void*)0):(void*)({
struct Cyc_Absyn_Upper_b_struct*_tmp47A=_cycalloc(sizeof(*_tmp47A));_tmp47A[0]=({
struct Cyc_Absyn_Upper_b_struct _tmp47B;_tmp47B.tag=0;_tmp47B.f1=(struct Cyc_Absyn_Exp*)
_tmp475;_tmp47B;});_tmp47A;});t=Cyc_Absyn_atb_typ(_tmp473,_tmp478,_tmp474,
_tmp479,_tmp476);((struct Cyc_Core_Opt*)_check_null(e->topt))->v=(void*)t;return t;}}
_LL28A:;_LL28B: return t;_LL287:;}void*Cyc_Tcexp_tcExpInitializer(struct Cyc_Tcenv_Tenv*
te,void**topt,struct Cyc_Absyn_Exp*e){void*t=Cyc_Tcexp_tcExpNoPromote(te,topt,e);
struct _RegionHandle*_tmp47C=Cyc_Tcenv_get_fnrgn(te);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(
_tmp47C,t) && !Cyc_Tcutil_is_noalias_path(_tmp47C,e)){({struct Cyc_String_pa_struct
_tmp481;_tmp481.tag=0;_tmp481.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(e));{struct Cyc_String_pa_struct _tmp480;_tmp480.tag=0;
_tmp480.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v));{struct Cyc_String_pa_struct
_tmp47F;_tmp47F.tag=0;_tmp47F.f1=(struct _dyneither_ptr)(topt == 0?({const char*
_tmp482="NULL";_tag_dyneither(_tmp482,sizeof(char),5);}):(struct _dyneither_ptr)
Cyc_Absynpp_typ2string(*topt));{void*_tmp47D[3]={& _tmp47F,& _tmp480,& _tmp481};Cyc_fprintf(
Cyc_stderr,({const char*_tmp47E="topt=%s, e->topt->v=%s, e=%s\n";_tag_dyneither(
_tmp47E,sizeof(char),30);}),_tag_dyneither(_tmp47D,sizeof(void*),3));}}}});({
void*_tmp483=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp484="Cannot consume non-unique paths; do swap instead";
_tag_dyneither(_tmp484,sizeof(char),49);}),_tag_dyneither(_tmp483,sizeof(void*),
0));});}{void*_tmp485=e->r;union Cyc_Absyn_Cnst _tmp486;struct _dyneither_ptr
_tmp487;_LL28D: if(*((int*)_tmp485)!= 28)goto _LL28F;_LL28E: goto _LL290;_LL28F: if(*((
int*)_tmp485)!= 29)goto _LL291;_LL290: goto _LL292;_LL291: if(*((int*)_tmp485)!= 0)
goto _LL293;_tmp486=((struct Cyc_Absyn_Const_e_struct*)_tmp485)->f1;if((_tmp486.String_c).tag
!= 7)goto _LL293;_tmp487=(struct _dyneither_ptr)(_tmp486.String_c).val;_LL292:
return t;_LL293:;_LL294: t=Cyc_Tcutil_compress(t);{void*_tmp488=t;struct Cyc_Absyn_ArrayInfo
_tmp489;void*_tmp48A;struct Cyc_Absyn_Tqual _tmp48B;struct Cyc_Absyn_Exp*_tmp48C;
union Cyc_Absyn_Constraint*_tmp48D;_LL296: if(_tmp488 <= (void*)4)goto _LL298;if(*((
int*)_tmp488)!= 7)goto _LL298;_tmp489=((struct Cyc_Absyn_ArrayType_struct*)_tmp488)->f1;
_tmp48A=_tmp489.elt_type;_tmp48B=_tmp489.tq;_tmp48C=_tmp489.num_elts;_tmp48D=
_tmp489.zero_term;_LL297: {void*_tmp48F;struct _tuple12 _tmp48E=Cyc_Tcutil_addressof_props(
te,e);_tmp48F=_tmp48E.f2;{void*b=_tmp48C == 0?(void*)((void*)0):(void*)({struct
Cyc_Absyn_Upper_b_struct*_tmp490=_cycalloc(sizeof(*_tmp490));_tmp490[0]=({struct
Cyc_Absyn_Upper_b_struct _tmp491;_tmp491.tag=0;_tmp491.f1=(struct Cyc_Absyn_Exp*)
_tmp48C;_tmp491;});_tmp490;});t=Cyc_Absyn_atb_typ(_tmp48A,_tmp48F,_tmp48B,b,
_tmp48D);Cyc_Tcutil_unchecked_cast(te,e,t,(void*)3);return t;}}_LL298:;_LL299:
return t;_LL295:;}_LL28C:;}}static void*Cyc_Tcexp_tcExpNoPromote(struct Cyc_Tcenv_Tenv*
te,void**topt,struct Cyc_Absyn_Exp*e){{void*_tmp492=e->r;struct Cyc_Absyn_Exp*
_tmp493;_LL29B: if(*((int*)_tmp492)!= 13)goto _LL29D;_tmp493=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmp492)->f1;_LL29C: Cyc_Tcexp_tcExpNoInst(te,topt,_tmp493);((struct Cyc_Core_Opt*)
_check_null(_tmp493->topt))->v=(void*)Cyc_Absyn_pointer_expand((void*)((struct
Cyc_Core_Opt*)_check_null(_tmp493->topt))->v,0);e->topt=_tmp493->topt;goto _LL29A;
_LL29D:;_LL29E: Cyc_Tcexp_tcExpNoInst(te,topt,e);((struct Cyc_Core_Opt*)
_check_null(e->topt))->v=(void*)Cyc_Absyn_pointer_expand((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,0);{void*_tmp494=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);struct Cyc_Absyn_PtrInfo _tmp495;void*_tmp496;struct Cyc_Absyn_Tqual
_tmp497;struct Cyc_Absyn_PtrAtts _tmp498;void*_tmp499;union Cyc_Absyn_Constraint*
_tmp49A;union Cyc_Absyn_Constraint*_tmp49B;union Cyc_Absyn_Constraint*_tmp49C;
_LL2A0: if(_tmp494 <= (void*)4)goto _LL2A2;if(*((int*)_tmp494)!= 4)goto _LL2A2;
_tmp495=((struct Cyc_Absyn_PointerType_struct*)_tmp494)->f1;_tmp496=_tmp495.elt_typ;
_tmp497=_tmp495.elt_tq;_tmp498=_tmp495.ptr_atts;_tmp499=_tmp498.rgn;_tmp49A=
_tmp498.nullable;_tmp49B=_tmp498.bounds;_tmp49C=_tmp498.zero_term;_LL2A1:{void*
_tmp49D=Cyc_Tcutil_compress(_tmp496);struct Cyc_Absyn_FnInfo _tmp49E;struct Cyc_List_List*
_tmp49F;struct Cyc_Core_Opt*_tmp4A0;void*_tmp4A1;struct Cyc_List_List*_tmp4A2;int
_tmp4A3;struct Cyc_Absyn_VarargInfo*_tmp4A4;struct Cyc_List_List*_tmp4A5;struct Cyc_List_List*
_tmp4A6;_LL2A5: if(_tmp49D <= (void*)4)goto _LL2A7;if(*((int*)_tmp49D)!= 8)goto
_LL2A7;_tmp49E=((struct Cyc_Absyn_FnType_struct*)_tmp49D)->f1;_tmp49F=_tmp49E.tvars;
_tmp4A0=_tmp49E.effect;_tmp4A1=_tmp49E.ret_typ;_tmp4A2=_tmp49E.args;_tmp4A3=
_tmp49E.c_varargs;_tmp4A4=_tmp49E.cyc_varargs;_tmp4A5=_tmp49E.rgn_po;_tmp4A6=
_tmp49E.attributes;_LL2A6: if(_tmp49F != 0){struct _RegionHandle*_tmp4A7=Cyc_Tcenv_get_fnrgn(
te);struct _tuple10 _tmp4A8=({struct _tuple10 _tmp4B5;_tmp4B5.f1=Cyc_Tcenv_lookup_type_vars(
te);_tmp4B5.f2=_tmp4A7;_tmp4B5;});struct Cyc_List_List*inst=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _tuple11*(*f)(struct _tuple10*,struct Cyc_Absyn_Tvar*),
struct _tuple10*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(_tmp4A7,Cyc_Tcutil_r_make_inst_var,&
_tmp4A8,_tmp49F);struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(void*(*f)(
struct _tuple11*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple11*))
Cyc_Core_snd,inst);void*ftyp=Cyc_Tcutil_rsubstitute(_tmp4A7,inst,(void*)({struct
Cyc_Absyn_FnType_struct*_tmp4B2=_cycalloc(sizeof(*_tmp4B2));_tmp4B2[0]=({struct
Cyc_Absyn_FnType_struct _tmp4B3;_tmp4B3.tag=8;_tmp4B3.f1=({struct Cyc_Absyn_FnInfo
_tmp4B4;_tmp4B4.tvars=0;_tmp4B4.effect=_tmp4A0;_tmp4B4.ret_typ=_tmp4A1;_tmp4B4.args=
_tmp4A2;_tmp4B4.c_varargs=_tmp4A3;_tmp4B4.cyc_varargs=_tmp4A4;_tmp4B4.rgn_po=
_tmp4A5;_tmp4B4.attributes=_tmp4A6;_tmp4B4;});_tmp4B3;});_tmp4B2;}));struct Cyc_Absyn_PointerType_struct*
_tmp4A9=({struct Cyc_Absyn_PointerType_struct*_tmp4AE=_cycalloc(sizeof(*_tmp4AE));
_tmp4AE[0]=({struct Cyc_Absyn_PointerType_struct _tmp4AF;_tmp4AF.tag=4;_tmp4AF.f1=({
struct Cyc_Absyn_PtrInfo _tmp4B0;_tmp4B0.elt_typ=ftyp;_tmp4B0.elt_tq=_tmp497;
_tmp4B0.ptr_atts=({struct Cyc_Absyn_PtrAtts _tmp4B1;_tmp4B1.rgn=_tmp499;_tmp4B1.nullable=
_tmp49A;_tmp4B1.bounds=_tmp49B;_tmp4B1.zero_term=_tmp49C;_tmp4B1.ptrloc=0;
_tmp4B1;});_tmp4B0;});_tmp4AF;});_tmp4AE;});struct Cyc_Absyn_Exp*_tmp4AA=Cyc_Absyn_copy_exp(
e);e->r=(void*)({struct Cyc_Absyn_Instantiate_e_struct*_tmp4AB=_cycalloc(sizeof(*
_tmp4AB));_tmp4AB[0]=({struct Cyc_Absyn_Instantiate_e_struct _tmp4AC;_tmp4AC.tag=
14;_tmp4AC.f1=_tmp4AA;_tmp4AC.f2=ts;_tmp4AC;});_tmp4AB;});e->topt=({struct Cyc_Core_Opt*
_tmp4AD=_cycalloc(sizeof(*_tmp4AD));_tmp4AD->v=(void*)((void*)_tmp4A9);_tmp4AD;});}
goto _LL2A4;_LL2A7:;_LL2A8: goto _LL2A4;_LL2A4:;}goto _LL29F;_LL2A2:;_LL2A3: goto
_LL29F;_LL29F:;}goto _LL29A;_LL29A:;}return(void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v;}static void Cyc_Tcexp_tcExpNoInst(struct Cyc_Tcenv_Tenv*
te,void**topt,struct Cyc_Absyn_Exp*e){struct Cyc_Position_Segment*loc=e->loc;void*
t;{void*_tmp4B6=e->r;struct Cyc_Absyn_Exp*_tmp4B7;struct _tuple1*_tmp4B8;struct Cyc_Absyn_Exp*
_tmp4B9;struct Cyc_List_List*_tmp4BA;struct Cyc_Core_Opt*_tmp4BB;struct Cyc_List_List*
_tmp4BC;union Cyc_Absyn_Cnst _tmp4BD;union Cyc_Absyn_Cnst*_tmp4BE;struct _tuple1*
_tmp4BF;void*_tmp4C0;void*_tmp4C1;struct Cyc_List_List*_tmp4C2;struct Cyc_Absyn_Exp*
_tmp4C3;void*_tmp4C4;struct Cyc_Absyn_Exp*_tmp4C5;struct Cyc_Core_Opt*_tmp4C6;
struct Cyc_Absyn_Exp*_tmp4C7;struct Cyc_Absyn_Exp*_tmp4C8;struct Cyc_Absyn_Exp*
_tmp4C9;struct Cyc_Absyn_Exp*_tmp4CA;struct Cyc_Absyn_Exp*_tmp4CB;struct Cyc_Absyn_Exp*
_tmp4CC;struct Cyc_Absyn_Exp*_tmp4CD;struct Cyc_Absyn_Exp*_tmp4CE;struct Cyc_Absyn_Exp*
_tmp4CF;struct Cyc_Absyn_Exp*_tmp4D0;struct Cyc_Absyn_Exp*_tmp4D1;struct Cyc_List_List*
_tmp4D2;struct Cyc_Absyn_VarargCallInfo*_tmp4D3;struct Cyc_Absyn_VarargCallInfo**
_tmp4D4;struct Cyc_Absyn_Exp*_tmp4D5;struct Cyc_Absyn_Exp*_tmp4D6;struct Cyc_List_List*
_tmp4D7;void*_tmp4D8;struct Cyc_Absyn_Exp*_tmp4D9;void*_tmp4DA;void**_tmp4DB;
struct Cyc_Absyn_Exp*_tmp4DC;struct Cyc_Absyn_Exp*_tmp4DD;struct Cyc_Absyn_Exp*
_tmp4DE;struct Cyc_Absyn_Exp*_tmp4DF;void*_tmp4E0;void*_tmp4E1;void*_tmp4E2;
struct Cyc_Absyn_Exp*_tmp4E3;struct Cyc_Absyn_Exp*_tmp4E4;struct _dyneither_ptr*
_tmp4E5;int _tmp4E6;int*_tmp4E7;int _tmp4E8;int*_tmp4E9;struct Cyc_Absyn_Exp*
_tmp4EA;struct _dyneither_ptr*_tmp4EB;int _tmp4EC;int*_tmp4ED;int _tmp4EE;int*
_tmp4EF;struct Cyc_Absyn_Exp*_tmp4F0;struct Cyc_Absyn_Exp*_tmp4F1;struct Cyc_List_List*
_tmp4F2;struct _tuple8*_tmp4F3;struct Cyc_List_List*_tmp4F4;struct Cyc_List_List*
_tmp4F5;struct Cyc_Absyn_Stmt*_tmp4F6;struct Cyc_Absyn_Vardecl*_tmp4F7;struct Cyc_Absyn_Exp*
_tmp4F8;struct Cyc_Absyn_Exp*_tmp4F9;int _tmp4FA;int*_tmp4FB;struct _tuple1*_tmp4FC;
struct _tuple1**_tmp4FD;struct Cyc_List_List*_tmp4FE;struct Cyc_List_List**_tmp4FF;
struct Cyc_List_List*_tmp500;struct Cyc_Absyn_Aggrdecl*_tmp501;struct Cyc_Absyn_Aggrdecl**
_tmp502;void*_tmp503;struct Cyc_List_List*_tmp504;struct Cyc_List_List*_tmp505;
struct Cyc_Absyn_Datatypedecl*_tmp506;struct Cyc_Absyn_Datatypefield*_tmp507;
struct _tuple1*_tmp508;struct _tuple1**_tmp509;struct Cyc_Absyn_Enumdecl*_tmp50A;
struct Cyc_Absyn_Enumfield*_tmp50B;struct _tuple1*_tmp50C;struct _tuple1**_tmp50D;
void*_tmp50E;struct Cyc_Absyn_Enumfield*_tmp50F;struct Cyc_Absyn_MallocInfo _tmp510;
int _tmp511;int*_tmp512;struct Cyc_Absyn_Exp*_tmp513;void**_tmp514;void***_tmp515;
struct Cyc_Absyn_Exp*_tmp516;struct Cyc_Absyn_Exp**_tmp517;int _tmp518;int*_tmp519;
struct Cyc_Absyn_Exp*_tmp51A;struct Cyc_Absyn_Exp*_tmp51B;struct Cyc_Absyn_Exp*
_tmp51C;struct _dyneither_ptr*_tmp51D;void*_tmp51E;_LL2AA: if(*((int*)_tmp4B6)!= 
13)goto _LL2AC;_tmp4B7=((struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp4B6)->f1;
_LL2AB: Cyc_Tcexp_tcExpNoInst(te,0,_tmp4B7);return;_LL2AC: if(*((int*)_tmp4B6)!= 2)
goto _LL2AE;_tmp4B8=((struct Cyc_Absyn_UnknownId_e_struct*)_tmp4B6)->f1;_LL2AD: Cyc_Tcexp_resolve_unknown_id(
te,e,_tmp4B8);Cyc_Tcexp_tcExpNoInst(te,topt,e);return;_LL2AE: if(*((int*)_tmp4B6)
!= 10)goto _LL2B0;_tmp4B9=((struct Cyc_Absyn_UnknownCall_e_struct*)_tmp4B6)->f1;
_tmp4BA=((struct Cyc_Absyn_UnknownCall_e_struct*)_tmp4B6)->f2;_LL2AF: Cyc_Tcexp_resolve_unknown_fn(
te,e,_tmp4B9,_tmp4BA);Cyc_Tcexp_tcExpNoInst(te,topt,e);return;_LL2B0: if(*((int*)
_tmp4B6)!= 37)goto _LL2B2;_tmp4BB=((struct Cyc_Absyn_UnresolvedMem_e_struct*)
_tmp4B6)->f1;_tmp4BC=((struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp4B6)->f2;
_LL2B1: Cyc_Tcexp_resolve_unresolved_mem(te,loc,topt,e,_tmp4BC);Cyc_Tcexp_tcExpNoInst(
te,topt,e);return;_LL2B2: if(*((int*)_tmp4B6)!= 0)goto _LL2B4;_tmp4BD=((struct Cyc_Absyn_Const_e_struct*)
_tmp4B6)->f1;_tmp4BE=(union Cyc_Absyn_Cnst*)&((struct Cyc_Absyn_Const_e_struct*)
_tmp4B6)->f1;_LL2B3: t=Cyc_Tcexp_tcConst(te,loc,topt,(union Cyc_Absyn_Cnst*)
_tmp4BE,e);goto _LL2A9;_LL2B4: if(*((int*)_tmp4B6)!= 1)goto _LL2B6;_tmp4BF=((struct
Cyc_Absyn_Var_e_struct*)_tmp4B6)->f1;_tmp4C0=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp4B6)->f2;_LL2B5: t=Cyc_Tcexp_tcVar(te,loc,_tmp4BF,_tmp4C0);goto _LL2A9;_LL2B6:
if(*((int*)_tmp4B6)!= 3)goto _LL2B8;_tmp4C1=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp4B6)->f1;_tmp4C2=((struct Cyc_Absyn_Primop_e_struct*)_tmp4B6)->f2;_LL2B7: t=
Cyc_Tcexp_tcPrimop(te,loc,topt,_tmp4C1,_tmp4C2);goto _LL2A9;_LL2B8: if(*((int*)
_tmp4B6)!= 5)goto _LL2BA;_tmp4C3=((struct Cyc_Absyn_Increment_e_struct*)_tmp4B6)->f1;
_tmp4C4=(void*)((struct Cyc_Absyn_Increment_e_struct*)_tmp4B6)->f2;_LL2B9: t=Cyc_Tcexp_tcIncrement(
te,loc,topt,_tmp4C3,_tmp4C4);goto _LL2A9;_LL2BA: if(*((int*)_tmp4B6)!= 4)goto
_LL2BC;_tmp4C5=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp4B6)->f1;_tmp4C6=((
struct Cyc_Absyn_AssignOp_e_struct*)_tmp4B6)->f2;_tmp4C7=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmp4B6)->f3;_LL2BB: t=Cyc_Tcexp_tcAssignOp(te,loc,topt,_tmp4C5,_tmp4C6,_tmp4C7);
goto _LL2A9;_LL2BC: if(*((int*)_tmp4B6)!= 6)goto _LL2BE;_tmp4C8=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp4B6)->f1;_tmp4C9=((struct Cyc_Absyn_Conditional_e_struct*)_tmp4B6)->f2;
_tmp4CA=((struct Cyc_Absyn_Conditional_e_struct*)_tmp4B6)->f3;_LL2BD: t=Cyc_Tcexp_tcConditional(
te,loc,topt,_tmp4C8,_tmp4C9,_tmp4CA);goto _LL2A9;_LL2BE: if(*((int*)_tmp4B6)!= 7)
goto _LL2C0;_tmp4CB=((struct Cyc_Absyn_And_e_struct*)_tmp4B6)->f1;_tmp4CC=((struct
Cyc_Absyn_And_e_struct*)_tmp4B6)->f2;_LL2BF: t=Cyc_Tcexp_tcAnd(te,loc,_tmp4CB,
_tmp4CC);goto _LL2A9;_LL2C0: if(*((int*)_tmp4B6)!= 8)goto _LL2C2;_tmp4CD=((struct
Cyc_Absyn_Or_e_struct*)_tmp4B6)->f1;_tmp4CE=((struct Cyc_Absyn_Or_e_struct*)
_tmp4B6)->f2;_LL2C1: t=Cyc_Tcexp_tcOr(te,loc,_tmp4CD,_tmp4CE);goto _LL2A9;_LL2C2:
if(*((int*)_tmp4B6)!= 9)goto _LL2C4;_tmp4CF=((struct Cyc_Absyn_SeqExp_e_struct*)
_tmp4B6)->f1;_tmp4D0=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp4B6)->f2;_LL2C3: t=
Cyc_Tcexp_tcSeqExp(te,loc,topt,_tmp4CF,_tmp4D0);goto _LL2A9;_LL2C4: if(*((int*)
_tmp4B6)!= 11)goto _LL2C6;_tmp4D1=((struct Cyc_Absyn_FnCall_e_struct*)_tmp4B6)->f1;
_tmp4D2=((struct Cyc_Absyn_FnCall_e_struct*)_tmp4B6)->f2;_tmp4D3=((struct Cyc_Absyn_FnCall_e_struct*)
_tmp4B6)->f3;_tmp4D4=(struct Cyc_Absyn_VarargCallInfo**)&((struct Cyc_Absyn_FnCall_e_struct*)
_tmp4B6)->f3;_LL2C5: t=Cyc_Tcexp_tcFnCall(te,loc,topt,_tmp4D1,_tmp4D2,_tmp4D4);
goto _LL2A9;_LL2C6: if(*((int*)_tmp4B6)!= 12)goto _LL2C8;_tmp4D5=((struct Cyc_Absyn_Throw_e_struct*)
_tmp4B6)->f1;_LL2C7: t=Cyc_Tcexp_tcThrow(te,loc,topt,_tmp4D5);goto _LL2A9;_LL2C8:
if(*((int*)_tmp4B6)!= 14)goto _LL2CA;_tmp4D6=((struct Cyc_Absyn_Instantiate_e_struct*)
_tmp4B6)->f1;_tmp4D7=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp4B6)->f2;_LL2C9:
t=Cyc_Tcexp_tcInstantiate(te,loc,topt,_tmp4D6,_tmp4D7);goto _LL2A9;_LL2CA: if(*((
int*)_tmp4B6)!= 15)goto _LL2CC;_tmp4D8=(void*)((struct Cyc_Absyn_Cast_e_struct*)
_tmp4B6)->f1;_tmp4D9=((struct Cyc_Absyn_Cast_e_struct*)_tmp4B6)->f2;_tmp4DA=(void*)((
struct Cyc_Absyn_Cast_e_struct*)_tmp4B6)->f4;_tmp4DB=(void**)&((void*)((struct Cyc_Absyn_Cast_e_struct*)
_tmp4B6)->f4);_LL2CB: t=Cyc_Tcexp_tcCast(te,loc,topt,_tmp4D8,_tmp4D9,(void**)
_tmp4DB);goto _LL2A9;_LL2CC: if(*((int*)_tmp4B6)!= 16)goto _LL2CE;_tmp4DC=((struct
Cyc_Absyn_Address_e_struct*)_tmp4B6)->f1;_LL2CD: t=Cyc_Tcexp_tcAddress(te,loc,e,
topt,_tmp4DC);goto _LL2A9;_LL2CE: if(*((int*)_tmp4B6)!= 17)goto _LL2D0;_tmp4DD=((
struct Cyc_Absyn_New_e_struct*)_tmp4B6)->f1;_tmp4DE=((struct Cyc_Absyn_New_e_struct*)
_tmp4B6)->f2;_LL2CF: t=Cyc_Tcexp_tcNew(te,loc,topt,_tmp4DD,e,_tmp4DE);goto _LL2A9;
_LL2D0: if(*((int*)_tmp4B6)!= 19)goto _LL2D2;_tmp4DF=((struct Cyc_Absyn_Sizeofexp_e_struct*)
_tmp4B6)->f1;_LL2D1: {void*_tmp51F=Cyc_Tcexp_tcExpNoPromote(te,0,_tmp4DF);t=Cyc_Tcexp_tcSizeof(
te,loc,topt,_tmp51F);goto _LL2A9;}_LL2D2: if(*((int*)_tmp4B6)!= 18)goto _LL2D4;
_tmp4E0=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp4B6)->f1;_LL2D3: t=Cyc_Tcexp_tcSizeof(
te,loc,topt,_tmp4E0);goto _LL2A9;_LL2D4: if(*((int*)_tmp4B6)!= 20)goto _LL2D6;
_tmp4E1=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp4B6)->f1;_tmp4E2=(void*)((
struct Cyc_Absyn_Offsetof_e_struct*)_tmp4B6)->f2;_LL2D5: t=Cyc_Tcexp_tcOffsetof(te,
loc,topt,_tmp4E1,_tmp4E2);goto _LL2A9;_LL2D6: if(*((int*)_tmp4B6)!= 21)goto _LL2D8;
_LL2D7:({void*_tmp520=0;Cyc_Tcutil_terr(loc,({const char*_tmp521="gen() not in top-level initializer";
_tag_dyneither(_tmp521,sizeof(char),35);}),_tag_dyneither(_tmp520,sizeof(void*),
0));});return;_LL2D8: if(*((int*)_tmp4B6)!= 22)goto _LL2DA;_tmp4E3=((struct Cyc_Absyn_Deref_e_struct*)
_tmp4B6)->f1;_LL2D9: t=Cyc_Tcexp_tcDeref(te,loc,topt,_tmp4E3);goto _LL2A9;_LL2DA:
if(*((int*)_tmp4B6)!= 23)goto _LL2DC;_tmp4E4=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp4B6)->f1;_tmp4E5=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp4B6)->f2;_tmp4E6=((
struct Cyc_Absyn_AggrMember_e_struct*)_tmp4B6)->f3;_tmp4E7=(int*)&((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp4B6)->f3;_tmp4E8=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp4B6)->f4;_tmp4E9=(
int*)&((struct Cyc_Absyn_AggrMember_e_struct*)_tmp4B6)->f4;_LL2DB: t=Cyc_Tcexp_tcAggrMember(
te,loc,topt,e,_tmp4E4,_tmp4E5,_tmp4E7,_tmp4E9);goto _LL2A9;_LL2DC: if(*((int*)
_tmp4B6)!= 24)goto _LL2DE;_tmp4EA=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp4B6)->f1;
_tmp4EB=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp4B6)->f2;_tmp4EC=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp4B6)->f3;_tmp4ED=(int*)&((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp4B6)->f3;
_tmp4EE=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp4B6)->f4;_tmp4EF=(int*)&((
struct Cyc_Absyn_AggrArrow_e_struct*)_tmp4B6)->f4;_LL2DD: t=Cyc_Tcexp_tcAggrArrow(
te,loc,topt,_tmp4EA,_tmp4EB,_tmp4ED,_tmp4EF);goto _LL2A9;_LL2DE: if(*((int*)
_tmp4B6)!= 25)goto _LL2E0;_tmp4F0=((struct Cyc_Absyn_Subscript_e_struct*)_tmp4B6)->f1;
_tmp4F1=((struct Cyc_Absyn_Subscript_e_struct*)_tmp4B6)->f2;_LL2DF: t=Cyc_Tcexp_tcSubscript(
te,loc,topt,_tmp4F0,_tmp4F1);goto _LL2A9;_LL2E0: if(*((int*)_tmp4B6)!= 26)goto
_LL2E2;_tmp4F2=((struct Cyc_Absyn_Tuple_e_struct*)_tmp4B6)->f1;_LL2E1: t=Cyc_Tcexp_tcTuple(
te,loc,topt,_tmp4F2);goto _LL2A9;_LL2E2: if(*((int*)_tmp4B6)!= 27)goto _LL2E4;
_tmp4F3=((struct Cyc_Absyn_CompoundLit_e_struct*)_tmp4B6)->f1;_tmp4F4=((struct Cyc_Absyn_CompoundLit_e_struct*)
_tmp4B6)->f2;_LL2E3: t=Cyc_Tcexp_tcCompoundLit(te,loc,topt,_tmp4F3,_tmp4F4);goto
_LL2A9;_LL2E4: if(*((int*)_tmp4B6)!= 28)goto _LL2E6;_tmp4F5=((struct Cyc_Absyn_Array_e_struct*)
_tmp4B6)->f1;_LL2E5: {void**elt_topt=0;int zero_term=0;if(topt != 0){void*_tmp522=
Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_ArrayInfo _tmp523;void*_tmp524;void**
_tmp525;union Cyc_Absyn_Constraint*_tmp526;_LL2FD: if(_tmp522 <= (void*)4)goto
_LL2FF;if(*((int*)_tmp522)!= 7)goto _LL2FF;_tmp523=((struct Cyc_Absyn_ArrayType_struct*)
_tmp522)->f1;_tmp524=_tmp523.elt_type;_tmp525=(void**)&(((struct Cyc_Absyn_ArrayType_struct*)
_tmp522)->f1).elt_type;_tmp526=_tmp523.zero_term;_LL2FE: elt_topt=(void**)_tmp525;
zero_term=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmp526);goto _LL2FC;_LL2FF:;_LL300: goto _LL2FC;_LL2FC:;}t=Cyc_Tcexp_tcArray(te,
loc,elt_topt,zero_term,_tmp4F5);goto _LL2A9;}_LL2E6: if(*((int*)_tmp4B6)!= 38)goto
_LL2E8;_tmp4F6=((struct Cyc_Absyn_StmtExp_e_struct*)_tmp4B6)->f1;_LL2E7: t=Cyc_Tcexp_tcStmtExp(
te,loc,topt,_tmp4F6);goto _LL2A9;_LL2E8: if(*((int*)_tmp4B6)!= 29)goto _LL2EA;
_tmp4F7=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp4B6)->f1;_tmp4F8=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmp4B6)->f2;_tmp4F9=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp4B6)->f3;_tmp4FA=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp4B6)->f4;
_tmp4FB=(int*)&((struct Cyc_Absyn_Comprehension_e_struct*)_tmp4B6)->f4;_LL2E9: t=
Cyc_Tcexp_tcComprehension(te,loc,topt,_tmp4F7,_tmp4F8,_tmp4F9,_tmp4FB);goto
_LL2A9;_LL2EA: if(*((int*)_tmp4B6)!= 30)goto _LL2EC;_tmp4FC=((struct Cyc_Absyn_Aggregate_e_struct*)
_tmp4B6)->f1;_tmp4FD=(struct _tuple1**)&((struct Cyc_Absyn_Aggregate_e_struct*)
_tmp4B6)->f1;_tmp4FE=((struct Cyc_Absyn_Aggregate_e_struct*)_tmp4B6)->f2;_tmp4FF=(
struct Cyc_List_List**)&((struct Cyc_Absyn_Aggregate_e_struct*)_tmp4B6)->f2;
_tmp500=((struct Cyc_Absyn_Aggregate_e_struct*)_tmp4B6)->f3;_tmp501=((struct Cyc_Absyn_Aggregate_e_struct*)
_tmp4B6)->f4;_tmp502=(struct Cyc_Absyn_Aggrdecl**)&((struct Cyc_Absyn_Aggregate_e_struct*)
_tmp4B6)->f4;_LL2EB: t=Cyc_Tcexp_tcAggregate(te,loc,topt,_tmp4FD,_tmp4FF,_tmp500,
_tmp502);goto _LL2A9;_LL2EC: if(*((int*)_tmp4B6)!= 31)goto _LL2EE;_tmp503=(void*)((
struct Cyc_Absyn_AnonStruct_e_struct*)_tmp4B6)->f1;_tmp504=((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp4B6)->f2;_LL2ED: t=Cyc_Tcexp_tcAnonStruct(te,loc,_tmp503,_tmp504);goto _LL2A9;
_LL2EE: if(*((int*)_tmp4B6)!= 32)goto _LL2F0;_tmp505=((struct Cyc_Absyn_Datatype_e_struct*)
_tmp4B6)->f1;_tmp506=((struct Cyc_Absyn_Datatype_e_struct*)_tmp4B6)->f2;_tmp507=((
struct Cyc_Absyn_Datatype_e_struct*)_tmp4B6)->f3;_LL2EF: t=Cyc_Tcexp_tcDatatype(te,
loc,topt,e,_tmp505,_tmp506,_tmp507);goto _LL2A9;_LL2F0: if(*((int*)_tmp4B6)!= 33)
goto _LL2F2;_tmp508=((struct Cyc_Absyn_Enum_e_struct*)_tmp4B6)->f1;_tmp509=(struct
_tuple1**)&((struct Cyc_Absyn_Enum_e_struct*)_tmp4B6)->f1;_tmp50A=((struct Cyc_Absyn_Enum_e_struct*)
_tmp4B6)->f2;_tmp50B=((struct Cyc_Absyn_Enum_e_struct*)_tmp4B6)->f3;_LL2F1:*
_tmp509=((struct Cyc_Absyn_Enumfield*)_check_null(_tmp50B))->name;t=(void*)({
struct Cyc_Absyn_EnumType_struct*_tmp527=_cycalloc(sizeof(*_tmp527));_tmp527[0]=({
struct Cyc_Absyn_EnumType_struct _tmp528;_tmp528.tag=12;_tmp528.f1=((struct Cyc_Absyn_Enumdecl*)
_check_null(_tmp50A))->name;_tmp528.f2=_tmp50A;_tmp528;});_tmp527;});goto _LL2A9;
_LL2F2: if(*((int*)_tmp4B6)!= 34)goto _LL2F4;_tmp50C=((struct Cyc_Absyn_AnonEnum_e_struct*)
_tmp4B6)->f1;_tmp50D=(struct _tuple1**)&((struct Cyc_Absyn_AnonEnum_e_struct*)
_tmp4B6)->f1;_tmp50E=(void*)((struct Cyc_Absyn_AnonEnum_e_struct*)_tmp4B6)->f2;
_tmp50F=((struct Cyc_Absyn_AnonEnum_e_struct*)_tmp4B6)->f3;_LL2F3:*_tmp50D=((
struct Cyc_Absyn_Enumfield*)_check_null(_tmp50F))->name;t=_tmp50E;goto _LL2A9;
_LL2F4: if(*((int*)_tmp4B6)!= 35)goto _LL2F6;_tmp510=((struct Cyc_Absyn_Malloc_e_struct*)
_tmp4B6)->f1;_tmp511=_tmp510.is_calloc;_tmp512=(int*)&(((struct Cyc_Absyn_Malloc_e_struct*)
_tmp4B6)->f1).is_calloc;_tmp513=_tmp510.rgn;_tmp514=_tmp510.elt_type;_tmp515=(
void***)&(((struct Cyc_Absyn_Malloc_e_struct*)_tmp4B6)->f1).elt_type;_tmp516=
_tmp510.num_elts;_tmp517=(struct Cyc_Absyn_Exp**)&(((struct Cyc_Absyn_Malloc_e_struct*)
_tmp4B6)->f1).num_elts;_tmp518=_tmp510.fat_result;_tmp519=(int*)&(((struct Cyc_Absyn_Malloc_e_struct*)
_tmp4B6)->f1).fat_result;_LL2F5: t=Cyc_Tcexp_tcMalloc(te,loc,topt,_tmp513,_tmp515,
_tmp517,_tmp512,_tmp519);goto _LL2A9;_LL2F6: if(*((int*)_tmp4B6)!= 36)goto _LL2F8;
_tmp51A=((struct Cyc_Absyn_Swap_e_struct*)_tmp4B6)->f1;_tmp51B=((struct Cyc_Absyn_Swap_e_struct*)
_tmp4B6)->f2;_LL2F7: t=Cyc_Tcexp_tcSwap(te,loc,topt,_tmp51A,_tmp51B);goto _LL2A9;
_LL2F8: if(*((int*)_tmp4B6)!= 39)goto _LL2FA;_tmp51C=((struct Cyc_Absyn_Tagcheck_e_struct*)
_tmp4B6)->f1;_tmp51D=((struct Cyc_Absyn_Tagcheck_e_struct*)_tmp4B6)->f2;_LL2F9: t=
Cyc_Tcexp_tcTagcheck(te,loc,topt,_tmp51C,_tmp51D);goto _LL2A9;_LL2FA: if(*((int*)
_tmp4B6)!= 40)goto _LL2A9;_tmp51E=(void*)((struct Cyc_Absyn_Valueof_e_struct*)
_tmp4B6)->f1;_LL2FB: if(!te->allow_valueof)({void*_tmp529=0;Cyc_Tcutil_terr(e->loc,({
const char*_tmp52A="valueof(-) can only occur within types";_tag_dyneither(
_tmp52A,sizeof(char),39);}),_tag_dyneither(_tmp529,sizeof(void*),0));});t=Cyc_Absyn_sint_typ;
goto _LL2A9;_LL2A9:;}e->topt=({struct Cyc_Core_Opt*_tmp52B=_cycalloc(sizeof(*
_tmp52B));_tmp52B->v=(void*)t;_tmp52B;});}
