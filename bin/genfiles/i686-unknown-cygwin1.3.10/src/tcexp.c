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
_dyneither_ptr desc;};extern char Cyc_Position_Nocontext[14];struct Cyc_Absyn_Loc_n_struct{
int tag;};struct Cyc_Absyn_Rel_n_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Abs_n_struct{
int tag;struct Cyc_List_List*f1;};union Cyc_Absyn_Nmspace_union{struct Cyc_Absyn_Loc_n_struct
Loc_n;struct Cyc_Absyn_Rel_n_struct Rel_n;struct Cyc_Absyn_Abs_n_struct Abs_n;};
struct _tuple1{union Cyc_Absyn_Nmspace_union f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Conref;
struct Cyc_Absyn_Tqual{int print_const;int q_volatile;int q_restrict;int real_const;
struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Eq_constr_struct{int tag;void*f1;
};struct Cyc_Absyn_Forward_constr_struct{int tag;struct Cyc_Absyn_Conref*f1;};
struct Cyc_Absyn_No_constr_struct{int tag;};union Cyc_Absyn_Constraint_union{struct
Cyc_Absyn_Eq_constr_struct Eq_constr;struct Cyc_Absyn_Forward_constr_struct
Forward_constr;struct Cyc_Absyn_No_constr_struct No_constr;};struct Cyc_Absyn_Conref{
union Cyc_Absyn_Constraint_union v;};struct Cyc_Absyn_Eq_kb_struct{int tag;void*f1;}
;struct Cyc_Absyn_Unknown_kb_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_struct{
int tag;struct Cyc_Core_Opt*f1;void*f2;};struct Cyc_Absyn_Tvar{struct _dyneither_ptr*
name;int identity;void*kind;};struct Cyc_Absyn_Upper_b_struct{int tag;struct Cyc_Absyn_Exp*
f1;};struct Cyc_Absyn_PtrLoc{struct Cyc_Position_Segment*ptr_loc;struct Cyc_Position_Segment*
rgn_loc;struct Cyc_Position_Segment*zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;
struct Cyc_Absyn_Conref*nullable;struct Cyc_Absyn_Conref*bounds;struct Cyc_Absyn_Conref*
zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_typ;
struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{
struct Cyc_Core_Opt*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{
struct Cyc_List_List*tvars;struct Cyc_Core_Opt*effect;void*ret_typ;struct Cyc_List_List*
args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*
rgn_po;struct Cyc_List_List*attributes;};struct Cyc_Absyn_UnknownDatatypeInfo{
struct _tuple1*name;int is_extensible;int is_flat;};struct Cyc_Absyn_UnknownDatatype_struct{
int tag;struct Cyc_Absyn_UnknownDatatypeInfo f1;};struct Cyc_Absyn_KnownDatatype_struct{
int tag;struct Cyc_Absyn_Datatypedecl**f1;};union Cyc_Absyn_DatatypeInfoU_union{
struct Cyc_Absyn_UnknownDatatype_struct UnknownDatatype;struct Cyc_Absyn_KnownDatatype_struct
KnownDatatype;};struct Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU_union
datatype_info;struct Cyc_List_List*targs;struct Cyc_Core_Opt*rgn;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{
struct _tuple1*datatype_name;struct _tuple1*field_name;int is_extensible;};struct
Cyc_Absyn_UnknownDatatypefield_struct{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo
f1;};struct Cyc_Absyn_KnownDatatypefield_struct{int tag;struct Cyc_Absyn_Datatypedecl*
f1;struct Cyc_Absyn_Datatypefield*f2;};union Cyc_Absyn_DatatypeFieldInfoU_union{
struct Cyc_Absyn_UnknownDatatypefield_struct UnknownDatatypefield;struct Cyc_Absyn_KnownDatatypefield_struct
KnownDatatypefield;};struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU_union
field_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownAggr_struct{int tag;
void*f1;struct _tuple1*f2;};struct Cyc_Absyn_KnownAggr_struct{int tag;struct Cyc_Absyn_Aggrdecl**
f1;};union Cyc_Absyn_AggrInfoU_union{struct Cyc_Absyn_UnknownAggr_struct
UnknownAggr;struct Cyc_Absyn_KnownAggr_struct KnownAggr;};struct Cyc_Absyn_AggrInfo{
union Cyc_Absyn_AggrInfoU_union aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{
void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;struct Cyc_Absyn_Conref*
zero_term;struct Cyc_Position_Segment*zt_loc;};struct Cyc_Absyn_Evar_struct{int tag;
struct Cyc_Core_Opt*f1;struct Cyc_Core_Opt*f2;int f3;struct Cyc_Core_Opt*f4;};struct
Cyc_Absyn_VarType_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_struct{
int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_struct{
int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_struct{
int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_struct{int tag;void*f1;
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
int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_struct{int tag;
struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*
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
struct Cyc_Core_Opt*fields;int is_extensible;int is_flat;};struct Cyc_Absyn_Enumfield{
struct _tuple1*name;struct Cyc_Absyn_Exp*tag;struct Cyc_Position_Segment*loc;};
struct Cyc_Absyn_Enumdecl{void*sc;struct _tuple1*name;struct Cyc_Core_Opt*fields;};
struct Cyc_Absyn_Typedefdecl{struct _tuple1*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*
tvs;struct Cyc_Core_Opt*kind;struct Cyc_Core_Opt*defn;struct Cyc_List_List*atts;};
struct Cyc_Absyn_Var_d_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_struct{
int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_struct{int tag;struct Cyc_Absyn_Pat*
f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Letv_d_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_struct{int tag;struct Cyc_Absyn_Tvar*
f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Alias_d_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Tvar*f2;struct Cyc_Absyn_Vardecl*f3;
};struct Cyc_Absyn_Aggr_d_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_struct{
int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_struct{int tag;
struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_struct{int tag;struct Cyc_Absyn_Typedefdecl*
f1;};struct Cyc_Absyn_Namespace_d_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*
f2;};struct Cyc_Absyn_Using_d_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*
f2;};struct Cyc_Absyn_ExternC_d_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_struct{
int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Decl{void*
r;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_ArrayElement_struct{int tag;
struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_struct{int tag;struct
_dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(
struct Cyc_Position_Segment*);struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(struct
Cyc_Position_Segment*);struct Cyc_Absyn_Conref*Cyc_Absyn_new_conref(void*x);
struct Cyc_Absyn_Conref*Cyc_Absyn_empty_conref();struct Cyc_Absyn_Conref*Cyc_Absyn_compress_conref(
struct Cyc_Absyn_Conref*x);void*Cyc_Absyn_conref_val(struct Cyc_Absyn_Conref*x);
void*Cyc_Absyn_conref_def(void*,struct Cyc_Absyn_Conref*x);extern struct Cyc_Absyn_Conref*
Cyc_Absyn_true_conref;extern struct Cyc_Absyn_Conref*Cyc_Absyn_false_conref;extern
struct Cyc_Absyn_Conref*Cyc_Absyn_bounds_one_conref;extern struct Cyc_Absyn_Conref*
Cyc_Absyn_bounds_dyneither_conref;void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,
struct Cyc_Core_Opt*tenv);void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);extern void*
Cyc_Absyn_char_typ;extern void*Cyc_Absyn_uchar_typ;extern void*Cyc_Absyn_ushort_typ;
extern void*Cyc_Absyn_uint_typ;extern void*Cyc_Absyn_ulonglong_typ;extern void*Cyc_Absyn_schar_typ;
extern void*Cyc_Absyn_sshort_typ;extern void*Cyc_Absyn_sint_typ;extern void*Cyc_Absyn_slonglong_typ;
extern void*Cyc_Absyn_float_typ;extern void*Cyc_Absyn_exn_typ;extern void*Cyc_Absyn_bounds_one;
void*Cyc_Absyn_atb_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*b,struct Cyc_Absyn_Conref*
zero_term);void*Cyc_Absyn_star_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,
struct Cyc_Absyn_Conref*zero_term);void*Cyc_Absyn_at_typ(void*t,void*rgn,struct
Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Conref*zero_term);void*Cyc_Absyn_dyneither_typ(
void*t,void*rgn,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Conref*zero_term);void*
Cyc_Absyn_array_typ(void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*
num_elts,struct Cyc_Absyn_Conref*zero_term,struct Cyc_Position_Segment*ztloc);
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,struct Cyc_Position_Segment*);struct
Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(
union Cyc_Absyn_Cnst_union,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(
unsigned int,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_add_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_cast_exp(void*,struct Cyc_Absyn_Exp*,int user_cast,void*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*,struct Cyc_Position_Segment*);
void*Cyc_Absyn_pointer_expand(void*,int fresh_evar);int Cyc_Absyn_is_lvalue(struct
Cyc_Absyn_Exp*);struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,
struct _dyneither_ptr*);struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(
struct Cyc_Absyn_Aggrdecl*,struct _dyneither_ptr*);struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(
union Cyc_Absyn_AggrInfoU_union info);int Cyc_strcmp(struct _dyneither_ptr s1,struct
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
struct Cyc_Absyn_Fndecl*);struct _tuple4{struct Cyc_List_List*f1;struct
_RegionHandle*f2;};struct _tuple5{struct Cyc_Absyn_Tvar*f1;void*f2;};struct _tuple5*
Cyc_Tcutil_r_make_inst_var(struct _tuple4*,struct Cyc_Absyn_Tvar*);void Cyc_Tcutil_check_contains_assign(
struct Cyc_Absyn_Exp*);void Cyc_Tcutil_check_type(struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*bound_tvars,void*k,int allow_evars,void*);
void Cyc_Tcutil_check_nonzero_bound(struct Cyc_Position_Segment*,struct Cyc_Absyn_Conref*);
void Cyc_Tcutil_check_bound(struct Cyc_Position_Segment*,unsigned int i,struct Cyc_Absyn_Conref*);
int Cyc_Tcutil_is_bound_one(struct Cyc_Absyn_Conref*b);struct Cyc_List_List*Cyc_Tcutil_resolve_struct_designators(
struct _RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,
struct Cyc_List_List*fields);int Cyc_Tcutil_is_tagged_pointer_typ(void*);int Cyc_Tcutil_is_tagged_pointer_typ_elt(
void*t,void**elt_typ_dest);int Cyc_Tcutil_is_zero_pointer_typ_elt(void*t,void**
elt_typ_dest);int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*e1,void**
ptr_type,int*is_dyneither,void**elt_type);int Cyc_Tcutil_is_noalias_pointer(void*
t);int Cyc_Tcutil_is_noalias_path(struct _RegionHandle*,struct Cyc_Absyn_Exp*e);int
Cyc_Tcutil_is_noalias_pointer_or_aggr(struct _RegionHandle*,void*t);struct _tuple6{
int f1;void*f2;};struct _tuple6 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Absyn_Exp*e);int Cyc_Tcutil_bits_only(void*t);int Cyc_Tcutil_is_const_exp(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e);int Cyc_Tcutil_rgn_of_pointer(void*
t,void**rgn);struct _tuple7{unsigned int f1;int f2;};struct _tuple7 Cyc_Evexp_eval_const_uint_exp(
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
_LL2:(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Var_e_struct*_tmpD=_cycalloc(
sizeof(*_tmpD));_tmpD[0]=({struct Cyc_Absyn_Var_e_struct _tmpE;_tmpE.tag=1;_tmpE.f1=
q;_tmpE.f2=(void*)_tmp6;_tmpE;});_tmpD;})));goto _LL0;_LL3: if(*((int*)_tmp5)!= 3)
goto _LL5;_tmp7=((struct Cyc_Tcenv_EnumRes_struct*)_tmp5)->f1;_tmp8=((struct Cyc_Tcenv_EnumRes_struct*)
_tmp5)->f2;_LL4:(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Enum_e_struct*
_tmpF=_cycalloc(sizeof(*_tmpF));_tmpF[0]=({struct Cyc_Absyn_Enum_e_struct _tmp10;
_tmp10.tag=33;_tmp10.f1=q;_tmp10.f2=(struct Cyc_Absyn_Enumdecl*)_tmp7;_tmp10.f3=(
struct Cyc_Absyn_Enumfield*)_tmp8;_tmp10;});_tmpF;})));goto _LL0;_LL5: if(*((int*)
_tmp5)!= 4)goto _LL7;_tmp9=(void*)((struct Cyc_Tcenv_AnonEnumRes_struct*)_tmp5)->f1;
_tmpA=((struct Cyc_Tcenv_AnonEnumRes_struct*)_tmp5)->f2;_LL6:(void*)(e->r=(void*)((
void*)({struct Cyc_Absyn_AnonEnum_e_struct*_tmp11=_cycalloc(sizeof(*_tmp11));
_tmp11[0]=({struct Cyc_Absyn_AnonEnum_e_struct _tmp12;_tmp12.tag=34;_tmp12.f1=q;
_tmp12.f2=(void*)_tmp9;_tmp12.f3=(struct Cyc_Absyn_Enumfield*)_tmpA;_tmp12;});
_tmp11;})));goto _LL0;_LL7: if(*((int*)_tmp5)!= 2)goto _LL9;_tmpB=((struct Cyc_Tcenv_DatatypeRes_struct*)
_tmp5)->f1;_tmpC=((struct Cyc_Tcenv_DatatypeRes_struct*)_tmp5)->f2;_LL8:(void*)(e->r=(
void*)((void*)({struct Cyc_Absyn_Datatype_e_struct*_tmp13=_cycalloc(sizeof(*
_tmp13));_tmp13[0]=({struct Cyc_Absyn_Datatype_e_struct _tmp14;_tmp14.tag=32;
_tmp14.f1=0;_tmp14.f2=_tmpB;_tmp14.f3=_tmpC;_tmp14;});_tmp13;})));goto _LL0;_LL9:
if(*((int*)_tmp5)!= 1)goto _LL0;_LLA:({struct Cyc_String_pa_struct _tmp17;_tmp17.tag=
0;_tmp17.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
q));{void*_tmp15[1]={& _tmp17};Cyc_Tcutil_terr(e->loc,({const char*_tmp16="bad occurrence of type name %s";
_tag_dyneither(_tmp16,sizeof(char),31);}),_tag_dyneither(_tmp15,sizeof(void*),1));}});(
void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Var_e_struct*_tmp18=_cycalloc(
sizeof(*_tmp18));_tmp18[0]=({struct Cyc_Absyn_Var_e_struct _tmp19;_tmp19.tag=1;
_tmp19.f1=q;_tmp19.f2=(void*)((void*)0);_tmp19;});_tmp18;})));goto _LL0;_LL0:;};
_pop_handler();}else{void*_tmp2=(void*)_exn_thrown;void*_tmp1B=_tmp2;_LLC: if(
_tmp1B != Cyc_Dict_Absent)goto _LLE;_LLD:(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Var_e_struct*
_tmp1C=_cycalloc(sizeof(*_tmp1C));_tmp1C[0]=({struct Cyc_Absyn_Var_e_struct _tmp1D;
_tmp1D.tag=1;_tmp1D.f1=q;_tmp1D.f2=(void*)((void*)0);_tmp1D;});_tmp1C;})));goto
_LLB;_LLE:;_LLF:(void)_throw(_tmp1B);_LLB:;}}}struct _tuple8{struct Cyc_List_List*
f1;struct Cyc_Absyn_Exp*f2;};static struct _tuple8*Cyc_Tcexp_make_struct_arg(struct
Cyc_Absyn_Exp*e){return({struct _tuple8*_tmp1E=_cycalloc(sizeof(*_tmp1E));_tmp1E->f1=
0;_tmp1E->f2=e;_tmp1E;});}static void Cyc_Tcexp_resolve_unknown_fn(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*es){void*
_tmp1F=(void*)e1->r;struct _tuple1*_tmp20;_LL11: if(*((int*)_tmp1F)!= 2)goto _LL13;
_tmp20=((struct Cyc_Absyn_UnknownId_e_struct*)_tmp1F)->f1;_LL12: {struct
_handler_cons _tmp21;_push_handler(& _tmp21);{int _tmp23=0;if(setjmp(_tmp21.handler))
_tmp23=1;if(!_tmp23){{struct _RegionHandle*_tmp24=Cyc_Tcenv_get_fnrgn(te);void*
_tmp25=Cyc_Tcenv_lookup_ordinary(_tmp24,te,e1->loc,_tmp20);void*_tmp26;struct Cyc_Absyn_Datatypedecl*
_tmp27;struct Cyc_Absyn_Datatypefield*_tmp28;struct Cyc_Absyn_Aggrdecl*_tmp29;
_LL16: if(*((int*)_tmp25)!= 0)goto _LL18;_tmp26=(void*)((struct Cyc_Tcenv_VarRes_struct*)
_tmp25)->f1;_LL17:(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_FnCall_e_struct*
_tmp2A=_cycalloc(sizeof(*_tmp2A));_tmp2A[0]=({struct Cyc_Absyn_FnCall_e_struct
_tmp2B;_tmp2B.tag=11;_tmp2B.f1=e1;_tmp2B.f2=es;_tmp2B.f3=0;_tmp2B;});_tmp2A;})));
_npop_handler(0);return;_LL18: if(*((int*)_tmp25)!= 2)goto _LL1A;_tmp27=((struct
Cyc_Tcenv_DatatypeRes_struct*)_tmp25)->f1;_tmp28=((struct Cyc_Tcenv_DatatypeRes_struct*)
_tmp25)->f2;_LL19: if(_tmp28->typs == 0)({struct Cyc_String_pa_struct _tmp2E;_tmp2E.tag=
0;_tmp2E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp28->name));{void*_tmp2C[1]={& _tmp2E};Cyc_Tcutil_terr(e->loc,({const char*
_tmp2D="%s is a constant, not a function";_tag_dyneither(_tmp2D,sizeof(char),33);}),
_tag_dyneither(_tmp2C,sizeof(void*),1));}});(void*)(e->r=(void*)((void*)({struct
Cyc_Absyn_Datatype_e_struct*_tmp2F=_cycalloc(sizeof(*_tmp2F));_tmp2F[0]=({struct
Cyc_Absyn_Datatype_e_struct _tmp30;_tmp30.tag=32;_tmp30.f1=es;_tmp30.f2=_tmp27;
_tmp30.f3=_tmp28;_tmp30;});_tmp2F;})));_npop_handler(0);return;_LL1A: if(*((int*)
_tmp25)!= 1)goto _LL1C;_tmp29=((struct Cyc_Tcenv_AggrRes_struct*)_tmp25)->f1;_LL1B: {
struct Cyc_List_List*_tmp31=((struct Cyc_List_List*(*)(struct _tuple8*(*f)(struct
Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcexp_make_struct_arg,
es);(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Struct_e_struct*_tmp32=
_cycalloc(sizeof(*_tmp32));_tmp32[0]=({struct Cyc_Absyn_Struct_e_struct _tmp33;
_tmp33.tag=30;_tmp33.f1=_tmp29->name;_tmp33.f2=0;_tmp33.f3=_tmp31;_tmp33.f4=(
struct Cyc_Absyn_Aggrdecl*)_tmp29;_tmp33;});_tmp32;})));_npop_handler(0);return;}
_LL1C: if(*((int*)_tmp25)!= 4)goto _LL1E;_LL1D: goto _LL1F;_LL1E: if(*((int*)_tmp25)
!= 3)goto _LL15;_LL1F:({struct Cyc_String_pa_struct _tmp36;_tmp36.tag=0;_tmp36.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp20));{
void*_tmp34[1]={& _tmp36};Cyc_Tcutil_terr(e->loc,({const char*_tmp35="%s is an enum constructor, not a function";
_tag_dyneither(_tmp35,sizeof(char),42);}),_tag_dyneither(_tmp34,sizeof(void*),1));}});
_npop_handler(0);return;_LL15:;};_pop_handler();}else{void*_tmp22=(void*)
_exn_thrown;void*_tmp38=_tmp22;_LL21: if(_tmp38 != Cyc_Dict_Absent)goto _LL23;_LL22:(
void*)(e->r=(void*)((void*)({struct Cyc_Absyn_FnCall_e_struct*_tmp39=_cycalloc(
sizeof(*_tmp39));_tmp39[0]=({struct Cyc_Absyn_FnCall_e_struct _tmp3A;_tmp3A.tag=11;
_tmp3A.f1=e1;_tmp3A.f2=es;_tmp3A.f3=0;_tmp3A;});_tmp39;})));return;_LL23:;_LL24:(
void)_throw(_tmp38);_LL20:;}}}_LL13:;_LL14:(void*)(e->r=(void*)((void*)({struct
Cyc_Absyn_FnCall_e_struct*_tmp3B=_cycalloc(sizeof(*_tmp3B));_tmp3B[0]=({struct
Cyc_Absyn_FnCall_e_struct _tmp3C;_tmp3C.tag=11;_tmp3C.f1=e1;_tmp3C.f2=es;_tmp3C.f3=
0;_tmp3C;});_tmp3B;})));return;_LL10:;}static void Cyc_Tcexp_resolve_unresolved_mem(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
e,struct Cyc_List_List*des){if(topt == 0){(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Array_e_struct*
_tmp3D=_cycalloc(sizeof(*_tmp3D));_tmp3D[0]=({struct Cyc_Absyn_Array_e_struct
_tmp3E;_tmp3E.tag=28;_tmp3E.f1=des;_tmp3E;});_tmp3D;})));return;}{void*t=*topt;
void*_tmp3F=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo _tmp40;union Cyc_Absyn_AggrInfoU_union
_tmp41;struct Cyc_Absyn_ArrayInfo _tmp42;void*_tmp43;struct Cyc_Absyn_Tqual _tmp44;
_LL26: if(_tmp3F <= (void*)4)goto _LL2C;if(*((int*)_tmp3F)!= 10)goto _LL28;_tmp40=((
struct Cyc_Absyn_AggrType_struct*)_tmp3F)->f1;_tmp41=_tmp40.aggr_info;_LL27:{
union Cyc_Absyn_AggrInfoU_union _tmp45=_tmp41;struct Cyc_Absyn_Aggrdecl**_tmp46;
struct Cyc_Absyn_Aggrdecl*_tmp47;_LL2F: if((_tmp45.UnknownAggr).tag != 0)goto _LL31;
_LL30:({void*_tmp48=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(({const char*_tmp49="struct type not properly set";
_tag_dyneither(_tmp49,sizeof(char),29);}),_tag_dyneither(_tmp48,sizeof(void*),0));});
_LL31: if((_tmp45.KnownAggr).tag != 1)goto _LL2E;_tmp46=(_tmp45.KnownAggr).f1;
_tmp47=*_tmp46;_LL32:(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Struct_e_struct*
_tmp4A=_cycalloc(sizeof(*_tmp4A));_tmp4A[0]=({struct Cyc_Absyn_Struct_e_struct
_tmp4B;_tmp4B.tag=30;_tmp4B.f1=_tmp47->name;_tmp4B.f2=0;_tmp4B.f3=des;_tmp4B.f4=(
struct Cyc_Absyn_Aggrdecl*)_tmp47;_tmp4B;});_tmp4A;})));_LL2E:;}goto _LL25;_LL28:
if(*((int*)_tmp3F)!= 7)goto _LL2A;_tmp42=((struct Cyc_Absyn_ArrayType_struct*)
_tmp3F)->f1;_tmp43=(void*)_tmp42.elt_type;_tmp44=_tmp42.tq;_LL29:(void*)(e->r=(
void*)((void*)({struct Cyc_Absyn_Array_e_struct*_tmp4C=_cycalloc(sizeof(*_tmp4C));
_tmp4C[0]=({struct Cyc_Absyn_Array_e_struct _tmp4D;_tmp4D.tag=28;_tmp4D.f1=des;
_tmp4D;});_tmp4C;})));goto _LL25;_LL2A: if(*((int*)_tmp3F)!= 11)goto _LL2C;_LL2B:(
void*)(e->r=(void*)((void*)({struct Cyc_Absyn_AnonStruct_e_struct*_tmp4E=
_cycalloc(sizeof(*_tmp4E));_tmp4E[0]=({struct Cyc_Absyn_AnonStruct_e_struct _tmp4F;
_tmp4F.tag=31;_tmp4F.f1=(void*)t;_tmp4F.f2=des;_tmp4F;});_tmp4E;})));goto _LL25;
_LL2C:;_LL2D:(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Array_e_struct*_tmp50=
_cycalloc(sizeof(*_tmp50));_tmp50[0]=({struct Cyc_Absyn_Array_e_struct _tmp51;
_tmp51.tag=28;_tmp51.f1=des;_tmp51;});_tmp50;})));goto _LL25;_LL25:;}}static void
Cyc_Tcexp_tcExpNoInst(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e);
static void*Cyc_Tcexp_tcExpNoPromote(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*
e);static void Cyc_Tcexp_tcExpList(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*es){
for(0;es != 0;es=es->tl){Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)es->hd);}}
struct Cyc_Tcexp_TestEnv Cyc_Tcexp_tcTest(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*
e,struct _dyneither_ptr msg_part){Cyc_Tcutil_check_contains_assign(e);Cyc_Tcexp_tcExp(
te,(void**)& Cyc_Absyn_sint_typ,e);if(!Cyc_Tcutil_coerce_to_bool(te,e))({struct
Cyc_String_pa_struct _tmp55;_tmp55.tag=0;_tmp55.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v));{
struct Cyc_String_pa_struct _tmp54;_tmp54.tag=0;_tmp54.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)msg_part);{void*_tmp52[2]={& _tmp54,& _tmp55};Cyc_Tcutil_terr(
e->loc,({const char*_tmp53="test of %s has type %s instead of integral or * type";
_tag_dyneither(_tmp53,sizeof(char),53);}),_tag_dyneither(_tmp52,sizeof(void*),2));}}});{
void*_tmp56=(void*)e->r;void*_tmp57;struct Cyc_List_List*_tmp58;_LL34: if(*((int*)
_tmp56)!= 3)goto _LL36;_tmp57=(void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp56)->f1;
_tmp58=((struct Cyc_Absyn_Primop_e_struct*)_tmp56)->f2;_LL35: if(_tmp57 == (void*)5
 || _tmp57 == (void*)6){struct _tuple0 _tmp5A=({struct _tuple0 _tmp59;_tmp59.f1=(void*)((
struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp58))->hd)->topt))->v;_tmp59.f2=(void*)((struct Cyc_Core_Opt*)
_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(((struct
Cyc_List_List*)_check_null(_tmp58))->tl))->hd)->topt))->v;_tmp59;});void*_tmp5B;
void*_tmp5C;void*_tmp5D;void*_tmp5E;_LL39: _tmp5B=_tmp5A.f1;if(_tmp5B <= (void*)4)
goto _LL3B;if(*((int*)_tmp5B)!= 14)goto _LL3B;_tmp5C=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp5B)->f1;_tmp5D=_tmp5A.f2;if(_tmp5D <= (void*)4)goto _LL3B;if(*((int*)_tmp5D)!= 
14)goto _LL3B;_tmp5E=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)_tmp5D)->f1;
_LL3A: return({struct Cyc_Tcexp_TestEnv _tmp5F;_tmp5F.eq=({struct _tuple0*_tmp60=
_region_malloc(Cyc_Tcenv_get_fnrgn(te),sizeof(*_tmp60));_tmp60->f1=_tmp5C;_tmp60->f2=
_tmp5E;_tmp60;});_tmp5F.isTrue=_tmp57 == (void*)5;_tmp5F;});_LL3B:;_LL3C: goto
_LL38;_LL38:;}goto _LL33;_LL36:;_LL37: goto _LL33;_LL33:;}return({struct Cyc_Tcexp_TestEnv
_tmp61;_tmp61.eq=0;_tmp61.isTrue=0;_tmp61;});}static void*Cyc_Tcexp_tcConst(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,union Cyc_Absyn_Cnst_union*
c,struct Cyc_Absyn_Exp*e){void*t;{union Cyc_Absyn_Cnst_union _tmp62=*((union Cyc_Absyn_Cnst_union*)
_check_null(c));void*_tmp63;void*_tmp64;void*_tmp65;void*_tmp66;void*_tmp67;void*
_tmp68;int _tmp69;struct _dyneither_ptr _tmp6A;_LL3E: if((_tmp62.Char_c).tag != 0)
goto _LL40;_tmp63=(_tmp62.Char_c).f1;if((int)_tmp63 != 0)goto _LL40;_LL3F: t=Cyc_Absyn_schar_typ;
goto _LL3D;_LL40: if((_tmp62.Char_c).tag != 0)goto _LL42;_tmp64=(_tmp62.Char_c).f1;
if((int)_tmp64 != 1)goto _LL42;_LL41: t=Cyc_Absyn_uchar_typ;goto _LL3D;_LL42: if((
_tmp62.Char_c).tag != 0)goto _LL44;_tmp65=(_tmp62.Char_c).f1;if((int)_tmp65 != 2)
goto _LL44;_LL43: t=Cyc_Absyn_char_typ;goto _LL3D;_LL44: if((_tmp62.Short_c).tag != 1)
goto _LL46;_tmp66=(_tmp62.Short_c).f1;_LL45: t=_tmp66 == (void*)1?Cyc_Absyn_ushort_typ:
Cyc_Absyn_sshort_typ;goto _LL3D;_LL46: if((_tmp62.LongLong_c).tag != 3)goto _LL48;
_tmp67=(_tmp62.LongLong_c).f1;_LL47: t=_tmp67 == (void*)1?Cyc_Absyn_ulonglong_typ:
Cyc_Absyn_slonglong_typ;goto _LL3D;_LL48: if((_tmp62.Float_c).tag != 4)goto _LL4A;
_LL49: t=Cyc_Absyn_float_typ;goto _LL3D;_LL4A: if((_tmp62.Int_c).tag != 2)goto _LL4C;
_tmp68=(_tmp62.Int_c).f1;_tmp69=(_tmp62.Int_c).f2;_LL4B: if(topt == 0)t=_tmp68 == (
void*)1?Cyc_Absyn_uint_typ: Cyc_Absyn_sint_typ;else{void*_tmp6B=Cyc_Tcutil_compress(*
topt);void*_tmp6C;void*_tmp6D;void*_tmp6E;void*_tmp6F;void*_tmp70;void*_tmp71;
void*_tmp72;void*_tmp73;void*_tmp74;_LL51: if(_tmp6B <= (void*)4)goto _LL5D;if(*((
int*)_tmp6B)!= 5)goto _LL53;_tmp6C=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp6B)->f1;_tmp6D=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp6B)->f2;if((int)
_tmp6D != 0)goto _LL53;_LL52:{void*_tmp75=_tmp6C;_LL60: if((int)_tmp75 != 1)goto
_LL62;_LL61: t=Cyc_Absyn_uchar_typ;goto _LL5F;_LL62: if((int)_tmp75 != 0)goto _LL64;
_LL63: t=Cyc_Absyn_schar_typ;goto _LL5F;_LL64: if((int)_tmp75 != 2)goto _LL5F;_LL65: t=
Cyc_Absyn_char_typ;goto _LL5F;_LL5F:;}*c=(union Cyc_Absyn_Cnst_union)({union Cyc_Absyn_Cnst_union
_tmp76;(_tmp76.Char_c).tag=0;(_tmp76.Char_c).f1=(void*)_tmp6C;(_tmp76.Char_c).f2=(
char)_tmp69;_tmp76;});goto _LL50;_LL53: if(*((int*)_tmp6B)!= 5)goto _LL55;_tmp6E=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp6B)->f1;_tmp6F=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp6B)->f2;if((int)_tmp6F != 1)goto _LL55;_LL54: t=_tmp6E == (void*)1?Cyc_Absyn_ushort_typ:
Cyc_Absyn_sshort_typ;*c=(union Cyc_Absyn_Cnst_union)({union Cyc_Absyn_Cnst_union
_tmp77;(_tmp77.Short_c).tag=1;(_tmp77.Short_c).f1=(void*)_tmp6E;(_tmp77.Short_c).f2=(
short)_tmp69;_tmp77;});goto _LL50;_LL55: if(*((int*)_tmp6B)!= 5)goto _LL57;_tmp70=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp6B)->f1;_tmp71=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp6B)->f2;if((int)_tmp71 != 2)goto _LL57;_LL56: t=_tmp70 == (void*)1?Cyc_Absyn_uint_typ:
Cyc_Absyn_sint_typ;goto _LL50;_LL57: if(*((int*)_tmp6B)!= 5)goto _LL59;_tmp72=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp6B)->f1;_tmp73=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp6B)->f2;if((int)_tmp73 != 3)goto _LL59;_LL58: t=_tmp72 == (void*)1?Cyc_Absyn_uint_typ:
Cyc_Absyn_sint_typ;goto _LL50;_LL59: if(*((int*)_tmp6B)!= 4)goto _LL5B;if(!(_tmp69
== 0))goto _LL5B;_LL5A:(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Const_e_struct*
_tmp78=_cycalloc(sizeof(*_tmp78));_tmp78[0]=({struct Cyc_Absyn_Const_e_struct
_tmp79;_tmp79.tag=0;_tmp79.f1=(union Cyc_Absyn_Cnst_union)({union Cyc_Absyn_Cnst_union
_tmp7A;(_tmp7A.Null_c).tag=6;_tmp7A;});_tmp79;});_tmp78;})));{struct Cyc_List_List*
_tmp7B=Cyc_Tcenv_lookup_type_vars(te);t=(void*)({struct Cyc_Absyn_PointerType_struct*
_tmp7C=_cycalloc(sizeof(*_tmp7C));_tmp7C[0]=({struct Cyc_Absyn_PointerType_struct
_tmp7D;_tmp7D.tag=4;_tmp7D.f1=({struct Cyc_Absyn_PtrInfo _tmp7E;_tmp7E.elt_typ=(
void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ak,({struct Cyc_Core_Opt*
_tmp81=_cycalloc(sizeof(*_tmp81));_tmp81->v=_tmp7B;_tmp81;}));_tmp7E.elt_tq=Cyc_Absyn_empty_tqual(
0);_tmp7E.ptr_atts=({struct Cyc_Absyn_PtrAtts _tmp7F;_tmp7F.rgn=(void*)Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_trk,({struct Cyc_Core_Opt*_tmp80=_cycalloc(
sizeof(*_tmp80));_tmp80->v=_tmp7B;_tmp80;}));_tmp7F.nullable=Cyc_Absyn_true_conref;
_tmp7F.bounds=Cyc_Absyn_empty_conref();_tmp7F.zero_term=((struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)();_tmp7F.ptrloc=0;_tmp7F;});_tmp7E;});_tmp7D;});_tmp7C;});
goto _LL50;}_LL5B: if(*((int*)_tmp6B)!= 18)goto _LL5D;_tmp74=(void*)((struct Cyc_Absyn_TagType_struct*)
_tmp6B)->f1;_LL5C: {struct Cyc_Absyn_ValueofType_struct*_tmp82=({struct Cyc_Absyn_ValueofType_struct*
_tmp89=_cycalloc(sizeof(*_tmp89));_tmp89[0]=({struct Cyc_Absyn_ValueofType_struct
_tmp8A;_tmp8A.tag=17;_tmp8A.f1=Cyc_Absyn_uint_exp((unsigned int)_tmp69,0);_tmp8A;});
_tmp89;});if(!Cyc_Tcutil_unify(_tmp74,(void*)_tmp82)){({struct Cyc_String_pa_struct
_tmp86;_tmp86.tag=0;_tmp86.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)_tmp82));{struct Cyc_String_pa_struct _tmp85;_tmp85.tag=0;_tmp85.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp74));{void*
_tmp83[2]={& _tmp85,& _tmp86};Cyc_Tcutil_terr(loc,({const char*_tmp84="expecting %s but found %s";
_tag_dyneither(_tmp84,sizeof(char),26);}),_tag_dyneither(_tmp83,sizeof(void*),2));}}});
Cyc_Tcutil_explain_failure();}t=(void*)({struct Cyc_Absyn_TagType_struct*_tmp87=
_cycalloc(sizeof(*_tmp87));_tmp87[0]=({struct Cyc_Absyn_TagType_struct _tmp88;
_tmp88.tag=18;_tmp88.f1=(void*)((void*)_tmp82);_tmp88;});_tmp87;});goto _LL50;}
_LL5D:;_LL5E: t=_tmp68 == (void*)1?Cyc_Absyn_uint_typ: Cyc_Absyn_sint_typ;goto _LL50;
_LL50:;}goto _LL3D;_LL4C: if((_tmp62.String_c).tag != 5)goto _LL4E;_tmp6A=(_tmp62.String_c).f1;
_LL4D: {int len=(int)_get_dyneither_size(_tmp6A,sizeof(char));struct Cyc_Absyn_Exp*
elen=Cyc_Absyn_const_exp((union Cyc_Absyn_Cnst_union)({union Cyc_Absyn_Cnst_union
_tmp95;(_tmp95.Int_c).tag=2;(_tmp95.Int_c).f1=(void*)((void*)1);(_tmp95.Int_c).f2=
len;_tmp95;}),loc);elen->topt=({struct Cyc_Core_Opt*_tmp8B=_cycalloc(sizeof(*
_tmp8B));_tmp8B->v=(void*)Cyc_Absyn_uint_typ;_tmp8B;});t=Cyc_Absyn_atb_typ(Cyc_Absyn_char_typ,(
void*)2,Cyc_Absyn_const_tqual(0),(void*)({struct Cyc_Absyn_Upper_b_struct*_tmp8C=
_cycalloc(sizeof(*_tmp8C));_tmp8C[0]=({struct Cyc_Absyn_Upper_b_struct _tmp8D;
_tmp8D.tag=0;_tmp8D.f1=elen;_tmp8D;});_tmp8C;}),Cyc_Absyn_true_conref);if(topt != 
0){void*_tmp8E=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_ArrayInfo _tmp8F;struct
Cyc_Absyn_Tqual _tmp90;_LL67: if(_tmp8E <= (void*)4)goto _LL6B;if(*((int*)_tmp8E)!= 
7)goto _LL69;_tmp8F=((struct Cyc_Absyn_ArrayType_struct*)_tmp8E)->f1;_tmp90=_tmp8F.tq;
_LL68: return Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,_tmp90,(struct Cyc_Absyn_Exp*)
elen,((struct Cyc_Absyn_Conref*(*)())Cyc_Absyn_empty_conref)(),0);_LL69: if(*((int*)
_tmp8E)!= 4)goto _LL6B;_LL6A: if(!Cyc_Tcutil_unify(*topt,t) && Cyc_Tcutil_silent_castable(
te,loc,t,*topt)){e->topt=({struct Cyc_Core_Opt*_tmp91=_cycalloc(sizeof(*_tmp91));
_tmp91->v=(void*)t;_tmp91;});Cyc_Tcutil_unchecked_cast(te,e,*topt,(void*)3);t=*
topt;}else{t=Cyc_Absyn_atb_typ(Cyc_Absyn_char_typ,Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_rk,Cyc_Tcenv_lookup_opt_type_vars(te)),Cyc_Absyn_const_tqual(0),(void*)({
struct Cyc_Absyn_Upper_b_struct*_tmp92=_cycalloc(sizeof(*_tmp92));_tmp92[0]=({
struct Cyc_Absyn_Upper_b_struct _tmp93;_tmp93.tag=0;_tmp93.f1=elen;_tmp93;});
_tmp92;}),Cyc_Absyn_true_conref);if(!Cyc_Tcutil_unify(*topt,t) && Cyc_Tcutil_silent_castable(
te,loc,t,*topt)){e->topt=({struct Cyc_Core_Opt*_tmp94=_cycalloc(sizeof(*_tmp94));
_tmp94->v=(void*)t;_tmp94;});Cyc_Tcutil_unchecked_cast(te,e,*topt,(void*)3);t=*
topt;}}goto _LL66;_LL6B:;_LL6C: goto _LL66;_LL66:;}return t;}_LL4E: if((_tmp62.Null_c).tag
!= 6)goto _LL3D;_LL4F: {struct Cyc_List_List*_tmp96=Cyc_Tcenv_lookup_type_vars(te);
t=(void*)({struct Cyc_Absyn_PointerType_struct*_tmp97=_cycalloc(sizeof(*_tmp97));
_tmp97[0]=({struct Cyc_Absyn_PointerType_struct _tmp98;_tmp98.tag=4;_tmp98.f1=({
struct Cyc_Absyn_PtrInfo _tmp99;_tmp99.elt_typ=(void*)Cyc_Absyn_new_evar((struct
Cyc_Core_Opt*)& Cyc_Tcutil_ak,({struct Cyc_Core_Opt*_tmp9C=_cycalloc(sizeof(*
_tmp9C));_tmp9C->v=_tmp96;_tmp9C;}));_tmp99.elt_tq=Cyc_Absyn_empty_tqual(0);
_tmp99.ptr_atts=({struct Cyc_Absyn_PtrAtts _tmp9A;_tmp9A.rgn=(void*)Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_trk,({struct Cyc_Core_Opt*_tmp9B=_cycalloc(
sizeof(*_tmp9B));_tmp9B->v=_tmp96;_tmp9B;}));_tmp9A.nullable=Cyc_Absyn_true_conref;
_tmp9A.bounds=Cyc_Absyn_empty_conref();_tmp9A.zero_term=((struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)();_tmp9A.ptrloc=0;_tmp9A;});_tmp99;});_tmp98;});_tmp97;});
goto _LL3D;}_LL3D:;}return t;}static void*Cyc_Tcexp_tcVar(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,struct _tuple1*q,void*b){void*_tmp9D=b;struct Cyc_Absyn_Vardecl*
_tmp9E;struct Cyc_Absyn_Fndecl*_tmp9F;struct Cyc_Absyn_Vardecl*_tmpA0;struct Cyc_Absyn_Vardecl*
_tmpA1;struct Cyc_Absyn_Vardecl*_tmpA2;_LL6E: if((int)_tmp9D != 0)goto _LL70;_LL6F:
return({struct Cyc_String_pa_struct _tmpA5;_tmpA5.tag=0;_tmpA5.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(q));{void*_tmpA3[1]={&
_tmpA5};Cyc_Tcexp_expr_err(te,loc,0,({const char*_tmpA4="undeclared identifier: %s";
_tag_dyneither(_tmpA4,sizeof(char),26);}),_tag_dyneither(_tmpA3,sizeof(void*),1));}});
_LL70: if(_tmp9D <= (void*)1)goto _LL72;if(*((int*)_tmp9D)!= 0)goto _LL72;_tmp9E=((
struct Cyc_Absyn_Global_b_struct*)_tmp9D)->f1;_LL71:*q=*_tmp9E->name;return(void*)
_tmp9E->type;_LL72: if(_tmp9D <= (void*)1)goto _LL74;if(*((int*)_tmp9D)!= 1)goto
_LL74;_tmp9F=((struct Cyc_Absyn_Funname_b_struct*)_tmp9D)->f1;_LL73:*q=*_tmp9F->name;
return Cyc_Tcutil_fndecl2typ(_tmp9F);_LL74: if(_tmp9D <= (void*)1)goto _LL76;if(*((
int*)_tmp9D)!= 4)goto _LL76;_tmpA0=((struct Cyc_Absyn_Pat_b_struct*)_tmp9D)->f1;
_LL75: _tmpA1=_tmpA0;goto _LL77;_LL76: if(_tmp9D <= (void*)1)goto _LL78;if(*((int*)
_tmp9D)!= 3)goto _LL78;_tmpA1=((struct Cyc_Absyn_Local_b_struct*)_tmp9D)->f1;_LL77:
_tmpA2=_tmpA1;goto _LL79;_LL78: if(_tmp9D <= (void*)1)goto _LL6D;if(*((int*)_tmp9D)
!= 2)goto _LL6D;_tmpA2=((struct Cyc_Absyn_Param_b_struct*)_tmp9D)->f1;_LL79:(*q).f1=(
union Cyc_Absyn_Nmspace_union)({union Cyc_Absyn_Nmspace_union _tmpA6;(_tmpA6.Loc_n).tag=
0;_tmpA6;});return(void*)_tmpA2->type;_LL6D:;}static void Cyc_Tcexp_check_format_args(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*fmt,struct Cyc_Core_Opt*opt_args,
struct _RegionHandle*temp,struct Cyc_List_List*(*type_getter)(struct _RegionHandle*,
struct Cyc_Tcenv_Tenv*,struct _dyneither_ptr,struct Cyc_Position_Segment*)){struct
Cyc_List_List*desc_types;{void*_tmpA7=(void*)fmt->r;union Cyc_Absyn_Cnst_union
_tmpA8;struct _dyneither_ptr _tmpA9;struct Cyc_Absyn_Exp*_tmpAA;struct Cyc_Absyn_Exp
_tmpAB;void*_tmpAC;union Cyc_Absyn_Cnst_union _tmpAD;struct _dyneither_ptr _tmpAE;
_LL7B: if(*((int*)_tmpA7)!= 0)goto _LL7D;_tmpA8=((struct Cyc_Absyn_Const_e_struct*)
_tmpA7)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)_tmpA7)->f1).String_c).tag != 
5)goto _LL7D;_tmpA9=(_tmpA8.String_c).f1;_LL7C: _tmpAE=_tmpA9;goto _LL7E;_LL7D: if(*((
int*)_tmpA7)!= 15)goto _LL7F;_tmpAA=((struct Cyc_Absyn_Cast_e_struct*)_tmpA7)->f2;
_tmpAB=*_tmpAA;_tmpAC=(void*)_tmpAB.r;if(*((int*)_tmpAC)!= 0)goto _LL7F;_tmpAD=((
struct Cyc_Absyn_Const_e_struct*)_tmpAC)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)
_tmpAC)->f1).String_c).tag != 5)goto _LL7F;_tmpAE=(_tmpAD.String_c).f1;_LL7E:
desc_types=type_getter(temp,te,(struct _dyneither_ptr)_tmpAE,fmt->loc);goto _LL7A;
_LL7F:;_LL80: if(opt_args != 0){struct Cyc_List_List*_tmpAF=(struct Cyc_List_List*)
opt_args->v;for(0;_tmpAF != 0;_tmpAF=_tmpAF->tl){Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)
_tmpAF->hd);{struct _RegionHandle*_tmpB0=Cyc_Tcenv_get_fnrgn(te);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(
_tmpB0,(void*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Exp*)_tmpAF->hd)->topt))->v)
 && !Cyc_Tcutil_is_noalias_path(_tmpB0,(struct Cyc_Absyn_Exp*)_tmpAF->hd))({void*
_tmpB1=0;Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_tmpAF->hd)->loc,({const char*
_tmpB2="Cannot consume non-unique paths; do swap instead";_tag_dyneither(_tmpB2,
sizeof(char),49);}),_tag_dyneither(_tmpB1,sizeof(void*),0));});}}}return;_LL7A:;}
if(opt_args != 0){struct Cyc_List_List*_tmpB3=(struct Cyc_List_List*)opt_args->v;
for(0;desc_types != 0  && _tmpB3 != 0;(desc_types=desc_types->tl,_tmpB3=_tmpB3->tl)){
void*t=(void*)desc_types->hd;struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)_tmpB3->hd;
Cyc_Tcexp_tcExp(te,(void**)& t,e);if(!Cyc_Tcutil_coerce_arg(te,e,t)){({struct Cyc_String_pa_struct
_tmpB7;_tmpB7.tag=0;_tmpB7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v));{struct Cyc_String_pa_struct
_tmpB6;_tmpB6.tag=0;_tmpB6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t));{void*_tmpB4[2]={& _tmpB6,& _tmpB7};Cyc_Tcutil_terr(e->loc,({const char*_tmpB5="descriptor has type \n%s\n but argument has type \n%s";
_tag_dyneither(_tmpB5,sizeof(char),51);}),_tag_dyneither(_tmpB4,sizeof(void*),2));}}});
Cyc_Tcutil_explain_failure();}{struct _RegionHandle*_tmpB8=Cyc_Tcenv_get_fnrgn(te);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(_tmpB8,t) && !Cyc_Tcutil_is_noalias_path(
_tmpB8,e))({void*_tmpB9=0;Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_tmpB3->hd)->loc,({
const char*_tmpBA="Cannot consume non-unique paths; do swap instead";
_tag_dyneither(_tmpBA,sizeof(char),49);}),_tag_dyneither(_tmpB9,sizeof(void*),0));});}}
if(desc_types != 0)({void*_tmpBB=0;Cyc_Tcutil_terr(fmt->loc,({const char*_tmpBC="too few arguments";
_tag_dyneither(_tmpBC,sizeof(char),18);}),_tag_dyneither(_tmpBB,sizeof(void*),0));});
if(_tmpB3 != 0)({void*_tmpBD=0;Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_tmpB3->hd)->loc,({
const char*_tmpBE="too many arguments";_tag_dyneither(_tmpBE,sizeof(char),19);}),
_tag_dyneither(_tmpBD,sizeof(void*),0));});}}static void*Cyc_Tcexp_tcUnPrimop(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,void*p,struct
Cyc_Absyn_Exp*e){void*t=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);void*_tmpBF=p;_LL82: if((int)_tmpBF != 0)goto _LL84;_LL83:
goto _LL85;_LL84: if((int)_tmpBF != 2)goto _LL86;_LL85: if(!Cyc_Tcutil_is_numeric(e))({
struct Cyc_String_pa_struct _tmpC2;_tmpC2.tag=0;_tmpC2.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));{void*_tmpC0[1]={& _tmpC2};Cyc_Tcutil_terr(
loc,({const char*_tmpC1="expecting arithmetic type but found %s";_tag_dyneither(
_tmpC1,sizeof(char),39);}),_tag_dyneither(_tmpC0,sizeof(void*),1));}});return(
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;_LL86: if((int)_tmpBF != 11)
goto _LL88;_LL87: Cyc_Tcutil_check_contains_assign(e);if(!Cyc_Tcutil_coerce_to_bool(
te,e))({struct Cyc_String_pa_struct _tmpC5;_tmpC5.tag=0;_tmpC5.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));{void*_tmpC3[1]={&
_tmpC5};Cyc_Tcutil_terr(loc,({const char*_tmpC4="expecting integral or * type but found %s";
_tag_dyneither(_tmpC4,sizeof(char),42);}),_tag_dyneither(_tmpC3,sizeof(void*),1));}});
return Cyc_Absyn_sint_typ;_LL88: if((int)_tmpBF != 12)goto _LL8A;_LL89: if(!Cyc_Tcutil_is_integral(
e))({struct Cyc_String_pa_struct _tmpC8;_tmpC8.tag=0;_tmpC8.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));{void*_tmpC6[1]={&
_tmpC8};Cyc_Tcutil_terr(loc,({const char*_tmpC7="expecting integral type but found %s";
_tag_dyneither(_tmpC7,sizeof(char),37);}),_tag_dyneither(_tmpC6,sizeof(void*),1));}});
return(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;_LL8A: if((int)_tmpBF
!= 19)goto _LL8C;_LL8B:{void*_tmpC9=t;struct Cyc_Absyn_PtrInfo _tmpCA;struct Cyc_Absyn_PtrAtts
_tmpCB;struct Cyc_Absyn_Conref*_tmpCC;_LL8F: if(_tmpC9 <= (void*)4)goto _LL93;if(*((
int*)_tmpC9)!= 7)goto _LL91;_LL90: goto _LL8E;_LL91: if(*((int*)_tmpC9)!= 4)goto
_LL93;_tmpCA=((struct Cyc_Absyn_PointerType_struct*)_tmpC9)->f1;_tmpCB=_tmpCA.ptr_atts;
_tmpCC=_tmpCB.bounds;_LL92:{union Cyc_Absyn_Constraint_union _tmpCD=(Cyc_Absyn_compress_conref(
_tmpCC))->v;void*_tmpCE;void*_tmpCF;_LL96: if((_tmpCD.Eq_constr).tag != 0)goto
_LL98;_tmpCE=(_tmpCD.Eq_constr).f1;if((int)_tmpCE != 0)goto _LL98;_LL97: goto _LL99;
_LL98: if((_tmpCD.Eq_constr).tag != 0)goto _LL9A;_tmpCF=(_tmpCD.Eq_constr).f1;if(
_tmpCF <= (void*)1)goto _LL9A;if(*((int*)_tmpCF)!= 0)goto _LL9A;_LL99: goto _LL95;
_LL9A:;_LL9B:({void*_tmpD0=0;Cyc_Tcutil_terr(loc,({const char*_tmpD1="can't apply numelts to pointer/array of abstract length";
_tag_dyneither(_tmpD1,sizeof(char),56);}),_tag_dyneither(_tmpD0,sizeof(void*),0));});
_LL95:;}goto _LL8E;_LL93:;_LL94:({struct Cyc_String_pa_struct _tmpD4;_tmpD4.tag=0;
_tmpD4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));{
void*_tmpD2[1]={& _tmpD4};Cyc_Tcutil_terr(loc,({const char*_tmpD3="numelts requires pointer or array type, not %s";
_tag_dyneither(_tmpD3,sizeof(char),47);}),_tag_dyneither(_tmpD2,sizeof(void*),1));}});
_LL8E:;}return Cyc_Absyn_uint_typ;_LL8C:;_LL8D:({void*_tmpD5=0;((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmpD6="Non-unary primop";
_tag_dyneither(_tmpD6,sizeof(char),17);}),_tag_dyneither(_tmpD5,sizeof(void*),0));});
_LL81:;}static void*Cyc_Tcexp_tcArithBinop(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*
e1,struct Cyc_Absyn_Exp*e2,int(*checker)(struct Cyc_Absyn_Exp*)){if(!checker(e1)){({
struct Cyc_String_pa_struct _tmpD9;_tmpD9.tag=0;_tmpD9.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(e1->topt))->v));{void*_tmpD7[1]={& _tmpD9};Cyc_Tcutil_terr(e1->loc,({
const char*_tmpD8="type %s cannot be used here";_tag_dyneither(_tmpD8,sizeof(char),
28);}),_tag_dyneither(_tmpD7,sizeof(void*),1));}});return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(
te));}if(!checker(e2)){({struct Cyc_String_pa_struct _tmpDC;_tmpDC.tag=0;_tmpDC.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)((
struct Cyc_Core_Opt*)_check_null(e2->topt))->v));{void*_tmpDA[1]={& _tmpDC};Cyc_Tcutil_terr(
e2->loc,({const char*_tmpDB="type %s cannot be used here";_tag_dyneither(_tmpDB,
sizeof(char),28);}),_tag_dyneither(_tmpDA,sizeof(void*),1));}});return Cyc_Absyn_wildtyp(
Cyc_Tcenv_lookup_opt_type_vars(te));}{void*t1=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(e1->topt))->v);void*t2=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(e2->topt))->v);return Cyc_Tcutil_max_arithmetic_type(
t1,t2);}}static void*Cyc_Tcexp_tcPlus(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*
e1,struct Cyc_Absyn_Exp*e2){void*t1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e1->topt))->v);void*t2=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e2->topt))->v);void*_tmpDD=t1;struct Cyc_Absyn_PtrInfo _tmpDE;void*
_tmpDF;struct Cyc_Absyn_Tqual _tmpE0;struct Cyc_Absyn_PtrAtts _tmpE1;void*_tmpE2;
struct Cyc_Absyn_Conref*_tmpE3;struct Cyc_Absyn_Conref*_tmpE4;struct Cyc_Absyn_Conref*
_tmpE5;_LL9D: if(_tmpDD <= (void*)4)goto _LL9F;if(*((int*)_tmpDD)!= 4)goto _LL9F;
_tmpDE=((struct Cyc_Absyn_PointerType_struct*)_tmpDD)->f1;_tmpDF=(void*)_tmpDE.elt_typ;
_tmpE0=_tmpDE.elt_tq;_tmpE1=_tmpDE.ptr_atts;_tmpE2=(void*)_tmpE1.rgn;_tmpE3=
_tmpE1.nullable;_tmpE4=_tmpE1.bounds;_tmpE5=_tmpE1.zero_term;_LL9E: if(!Cyc_Tcutil_kind_leq(
Cyc_Tcutil_typ_kind(_tmpDF),(void*)1))({void*_tmpE6=0;Cyc_Tcutil_terr(e1->loc,({
const char*_tmpE7="can't perform arithmetic on abstract pointer type";
_tag_dyneither(_tmpE7,sizeof(char),50);}),_tag_dyneither(_tmpE6,sizeof(void*),0));});
if(Cyc_Tcutil_is_noalias_pointer(t1))({void*_tmpE8=0;Cyc_Tcutil_terr(e1->loc,({
const char*_tmpE9="can't perform arithmetic on non-aliasing pointer type";
_tag_dyneither(_tmpE9,sizeof(char),54);}),_tag_dyneither(_tmpE8,sizeof(void*),0));});
if(!Cyc_Tcutil_coerce_sint_typ(te,e2))({struct Cyc_String_pa_struct _tmpEC;_tmpEC.tag=
0;_tmpEC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t2));{void*_tmpEA[1]={& _tmpEC};Cyc_Tcutil_terr(e2->loc,({const char*_tmpEB="expecting int but found %s";
_tag_dyneither(_tmpEB,sizeof(char),27);}),_tag_dyneither(_tmpEA,sizeof(void*),1));}});
_tmpE4=Cyc_Absyn_compress_conref(_tmpE4);{union Cyc_Absyn_Constraint_union _tmpED=
_tmpE4->v;void*_tmpEE;void*_tmpEF;struct Cyc_Absyn_Exp*_tmpF0;_LLA2: if((_tmpED.Eq_constr).tag
!= 0)goto _LLA4;_tmpEE=(_tmpED.Eq_constr).f1;if((int)_tmpEE != 0)goto _LLA4;_LLA3:
return t1;_LLA4: if((_tmpED.Eq_constr).tag != 0)goto _LLA6;_tmpEF=(_tmpED.Eq_constr).f1;
if(_tmpEF <= (void*)1)goto _LLA6;if(*((int*)_tmpEF)!= 0)goto _LLA6;_tmpF0=((struct
Cyc_Absyn_Upper_b_struct*)_tmpEF)->f1;_LLA5: if(((int(*)(int,struct Cyc_Absyn_Conref*
x))Cyc_Absyn_conref_def)(0,_tmpE5))({void*_tmpF1=0;Cyc_Tcutil_warn(e1->loc,({
const char*_tmpF2="pointer arithmetic on thin, zero-terminated pointer may be expensive.";
_tag_dyneither(_tmpF2,sizeof(char),70);}),_tag_dyneither(_tmpF1,sizeof(void*),0));});{
struct Cyc_Absyn_PointerType_struct*_tmpF3=({struct Cyc_Absyn_PointerType_struct*
_tmpF4=_cycalloc(sizeof(*_tmpF4));_tmpF4[0]=({struct Cyc_Absyn_PointerType_struct
_tmpF5;_tmpF5.tag=4;_tmpF5.f1=({struct Cyc_Absyn_PtrInfo _tmpF6;_tmpF6.elt_typ=(
void*)_tmpDF;_tmpF6.elt_tq=_tmpE0;_tmpF6.ptr_atts=({struct Cyc_Absyn_PtrAtts
_tmpF7;_tmpF7.rgn=(void*)_tmpE2;_tmpF7.nullable=Cyc_Absyn_true_conref;_tmpF7.bounds=
Cyc_Absyn_bounds_dyneither_conref;_tmpF7.zero_term=_tmpE5;_tmpF7.ptrloc=0;_tmpF7;});
_tmpF6;});_tmpF5;});_tmpF4;});Cyc_Tcutil_unchecked_cast(te,e1,(void*)_tmpF3,(
void*)3);return(void*)_tmpF3;}_LLA6:;_LLA7: _tmpE4->v=(union Cyc_Absyn_Constraint_union)({
union Cyc_Absyn_Constraint_union _tmpF8;(_tmpF8.Eq_constr).tag=0;(_tmpF8.Eq_constr).f1=(
void*)((void*)0);_tmpF8;});return t1;_LLA1:;}_LL9F:;_LLA0: return Cyc_Tcexp_tcArithBinop(
te,e1,e2,Cyc_Tcutil_is_numeric);_LL9C:;}static void*Cyc_Tcexp_tcMinus(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){void*t1=(void*)((struct Cyc_Core_Opt*)
_check_null(e1->topt))->v;void*t2=(void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v;
void*t1_elt=(void*)0;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(t1,& t1_elt)){if(Cyc_Tcutil_is_tagged_pointer_typ(
t2)){if(!Cyc_Tcutil_unify(t1,t2)){({struct Cyc_String_pa_struct _tmpFC;_tmpFC.tag=
0;_tmpFC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v));{struct Cyc_String_pa_struct
_tmpFB;_tmpFB.tag=0;_tmpFB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v));{void*_tmpF9[2]={& _tmpFB,&
_tmpFC};Cyc_Tcutil_terr(e1->loc,({const char*_tmpFA="pointer arithmetic on values of different types (%s != %s)";
_tag_dyneither(_tmpFA,sizeof(char),59);}),_tag_dyneither(_tmpF9,sizeof(void*),2));}}});
Cyc_Tcutil_explain_failure();}return Cyc_Absyn_sint_typ;}else{if(!Cyc_Tcutil_kind_leq(
Cyc_Tcutil_typ_kind(t1_elt),(void*)1))({void*_tmpFD=0;Cyc_Tcutil_terr(e1->loc,({
const char*_tmpFE="can't perform arithmetic on abstract pointer type";
_tag_dyneither(_tmpFE,sizeof(char),50);}),_tag_dyneither(_tmpFD,sizeof(void*),0));});
if(Cyc_Tcutil_is_noalias_pointer(t1))({void*_tmpFF=0;Cyc_Tcutil_terr(e1->loc,({
const char*_tmp100="can't perform arithmetic on non-aliasing pointer type";
_tag_dyneither(_tmp100,sizeof(char),54);}),_tag_dyneither(_tmpFF,sizeof(void*),0));});
if(!Cyc_Tcutil_coerce_sint_typ(te,e2)){({struct Cyc_String_pa_struct _tmp104;
_tmp104.tag=0;_tmp104.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t2));{struct Cyc_String_pa_struct _tmp103;_tmp103.tag=0;_tmp103.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));{void*_tmp101[
2]={& _tmp103,& _tmp104};Cyc_Tcutil_terr(e2->loc,({const char*_tmp102="expecting either %s or int but found %s";
_tag_dyneither(_tmp102,sizeof(char),40);}),_tag_dyneither(_tmp101,sizeof(void*),
2));}}});Cyc_Tcutil_explain_failure();}return t1;}}if(Cyc_Tcutil_is_pointer_type(
t1))Cyc_Tcutil_unchecked_cast(te,e1,Cyc_Absyn_sint_typ,(void*)3);if(Cyc_Tcutil_is_pointer_type(
t2))Cyc_Tcutil_unchecked_cast(te,e2,Cyc_Absyn_sint_typ,(void*)3);return Cyc_Tcexp_tcArithBinop(
te,e1,e2,Cyc_Tcutil_is_numeric);}static void*Cyc_Tcexp_tcAnyBinop(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
int e1_is_num=Cyc_Tcutil_is_numeric(e1);int e2_is_num=Cyc_Tcutil_is_numeric(e2);
void*t1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v);
void*t2=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v);
if(e1_is_num  && e2_is_num)return Cyc_Absyn_sint_typ;else{if(Cyc_Tcutil_typ_kind(
t1)== (void*)2  || Cyc_Tcutil_unify(t1,Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_bk,Cyc_Tcenv_lookup_opt_type_vars(te)))){if(Cyc_Tcutil_unify(t1,t2))
return Cyc_Absyn_sint_typ;else{if(Cyc_Tcutil_silent_castable(te,loc,t2,t1)){Cyc_Tcutil_unchecked_cast(
te,e2,t1,(void*)3);return Cyc_Absyn_sint_typ;}else{if(Cyc_Tcutil_silent_castable(
te,loc,t1,t2)){Cyc_Tcutil_unchecked_cast(te,e1,t2,(void*)3);return Cyc_Absyn_sint_typ;}
else{if(Cyc_Tcutil_zero_to_null(te,t2,e1) || Cyc_Tcutil_zero_to_null(te,t1,e2))
return Cyc_Absyn_sint_typ;else{goto pointer_cmp;}}}}}else{pointer_cmp: {struct
_tuple0 _tmp106=({struct _tuple0 _tmp105;_tmp105.f1=Cyc_Tcutil_compress(t1);_tmp105.f2=
Cyc_Tcutil_compress(t2);_tmp105;});void*_tmp107;struct Cyc_Absyn_PtrInfo _tmp108;
void*_tmp109;void*_tmp10A;struct Cyc_Absyn_PtrInfo _tmp10B;void*_tmp10C;void*
_tmp10D;void*_tmp10E;_LLA9: _tmp107=_tmp106.f1;if(_tmp107 <= (void*)4)goto _LLAB;
if(*((int*)_tmp107)!= 4)goto _LLAB;_tmp108=((struct Cyc_Absyn_PointerType_struct*)
_tmp107)->f1;_tmp109=(void*)_tmp108.elt_typ;_tmp10A=_tmp106.f2;if(_tmp10A <= (
void*)4)goto _LLAB;if(*((int*)_tmp10A)!= 4)goto _LLAB;_tmp10B=((struct Cyc_Absyn_PointerType_struct*)
_tmp10A)->f1;_tmp10C=(void*)_tmp10B.elt_typ;_LLAA: if(Cyc_Tcutil_unify(_tmp109,
_tmp10C))return Cyc_Absyn_sint_typ;goto _LLA8;_LLAB: _tmp10D=_tmp106.f1;if(_tmp10D
<= (void*)4)goto _LLAD;if(*((int*)_tmp10D)!= 14)goto _LLAD;_tmp10E=_tmp106.f2;if(
_tmp10E <= (void*)4)goto _LLAD;if(*((int*)_tmp10E)!= 14)goto _LLAD;_LLAC: return Cyc_Absyn_sint_typ;
_LLAD:;_LLAE: goto _LLA8;_LLA8:;}({struct Cyc_String_pa_struct _tmp112;_tmp112.tag=0;
_tmp112.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t2));{struct Cyc_String_pa_struct _tmp111;_tmp111.tag=0;_tmp111.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));{void*_tmp10F[
2]={& _tmp111,& _tmp112};Cyc_Tcutil_terr(loc,({const char*_tmp110="comparison not allowed between %s and %s";
_tag_dyneither(_tmp110,sizeof(char),41);}),_tag_dyneither(_tmp10F,sizeof(void*),
2));}}});Cyc_Tcutil_explain_failure();return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(
te));}}}static void*Cyc_Tcexp_tcEqPrimop(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){if(Cyc_Tcexp_tcAnyBinop(te,
loc,e1,e2)== Cyc_Absyn_sint_typ)return Cyc_Absyn_sint_typ;{void*t1=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v);void*t2=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v);{struct _tuple0 _tmp114=({
struct _tuple0 _tmp113;_tmp113.f1=t1;_tmp113.f2=t2;_tmp113;});void*_tmp115;void*
_tmp116;void*_tmp117;void*_tmp118;_LLB0: _tmp115=_tmp114.f1;if(_tmp115 <= (void*)4)
goto _LLB2;if(*((int*)_tmp115)!= 14)goto _LLB2;_tmp116=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp115)->f1;_tmp117=_tmp114.f2;if(_tmp117 <= (void*)4)goto _LLB2;if(*((int*)
_tmp117)!= 14)goto _LLB2;_tmp118=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp117)->f1;_LLB1: return Cyc_Absyn_sint_typ;_LLB2:;_LLB3: goto _LLAF;_LLAF:;}({
struct Cyc_String_pa_struct _tmp11C;_tmp11C.tag=0;_tmp11C.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2));{struct Cyc_String_pa_struct
_tmp11B;_tmp11B.tag=0;_tmp11B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t1));{void*_tmp119[2]={& _tmp11B,& _tmp11C};Cyc_Tcutil_terr(
loc,({const char*_tmp11A="comparison not allowed between %s and %s";
_tag_dyneither(_tmp11A,sizeof(char),41);}),_tag_dyneither(_tmp119,sizeof(void*),
2));}}});Cyc_Tcutil_explain_failure();return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(
te));}}static void*Cyc_Tcexp_tcBinPrimop(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,void*p,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){void*
_tmp11D=p;_LLB5: if((int)_tmp11D != 0)goto _LLB7;_LLB6: return Cyc_Tcexp_tcPlus(te,e1,
e2);_LLB7: if((int)_tmp11D != 2)goto _LLB9;_LLB8: return Cyc_Tcexp_tcMinus(te,e1,e2);
_LLB9: if((int)_tmp11D != 1)goto _LLBB;_LLBA: goto _LLBC;_LLBB: if((int)_tmp11D != 3)
goto _LLBD;_LLBC: return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_numeric);
_LLBD: if((int)_tmp11D != 4)goto _LLBF;_LLBE: goto _LLC0;_LLBF: if((int)_tmp11D != 13)
goto _LLC1;_LLC0: goto _LLC2;_LLC1: if((int)_tmp11D != 14)goto _LLC3;_LLC2: goto _LLC4;
_LLC3: if((int)_tmp11D != 15)goto _LLC5;_LLC4: goto _LLC6;_LLC5: if((int)_tmp11D != 16)
goto _LLC7;_LLC6: goto _LLC8;_LLC7: if((int)_tmp11D != 17)goto _LLC9;_LLC8: goto _LLCA;
_LLC9: if((int)_tmp11D != 18)goto _LLCB;_LLCA: return Cyc_Tcexp_tcArithBinop(te,e1,e2,
Cyc_Tcutil_is_integral);_LLCB: if((int)_tmp11D != 5)goto _LLCD;_LLCC: goto _LLCE;
_LLCD: if((int)_tmp11D != 6)goto _LLCF;_LLCE: return Cyc_Tcexp_tcEqPrimop(te,loc,e1,
e2);_LLCF: if((int)_tmp11D != 7)goto _LLD1;_LLD0: goto _LLD2;_LLD1: if((int)_tmp11D != 
8)goto _LLD3;_LLD2: goto _LLD4;_LLD3: if((int)_tmp11D != 9)goto _LLD5;_LLD4: goto _LLD6;
_LLD5: if((int)_tmp11D != 10)goto _LLD7;_LLD6: return Cyc_Tcexp_tcAnyBinop(te,loc,e1,
e2);_LLD7:;_LLD8:({void*_tmp11E=0;((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp11F="bad binary primop";
_tag_dyneither(_tmp11F,sizeof(char),18);}),_tag_dyneither(_tmp11E,sizeof(void*),
0));});_LLB4:;}static void*Cyc_Tcexp_tcPrimop(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,void*p,struct Cyc_List_List*es){if(p == (void*)2  && ((int(*)(struct
Cyc_List_List*x))Cyc_List_length)(es)== 1)return Cyc_Tcexp_tcExp(te,topt,(struct
Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd);Cyc_Tcexp_tcExpList(
te,es);{void*t;switch(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es)){case
0: _LLD9: return({void*_tmp120=0;Cyc_Tcexp_expr_err(te,loc,topt,({const char*
_tmp121="primitive operator has 0 arguments";_tag_dyneither(_tmp121,sizeof(char),
35);}),_tag_dyneither(_tmp120,sizeof(void*),0));});case 1: _LLDA: t=Cyc_Tcexp_tcUnPrimop(
te,loc,topt,p,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd);
break;case 2: _LLDB: t=Cyc_Tcexp_tcBinPrimop(te,loc,topt,p,(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(es))->hd,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(((struct Cyc_List_List*)_check_null(es))->tl))->hd);break;default:
_LLDC: return({void*_tmp122=0;Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp123="primitive operator has > 2 arguments";
_tag_dyneither(_tmp123,sizeof(char),37);}),_tag_dyneither(_tmp122,sizeof(void*),
0));});}return t;}}struct _tuple9{struct Cyc_Absyn_Tqual f1;void*f2;};static void Cyc_Tcexp_check_writable(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){{void*_tmp124=(void*)e->r;void*
_tmp125;struct Cyc_Absyn_Vardecl*_tmp126;void*_tmp127;struct Cyc_Absyn_Vardecl*
_tmp128;void*_tmp129;struct Cyc_Absyn_Vardecl*_tmp12A;void*_tmp12B;struct Cyc_Absyn_Vardecl*
_tmp12C;struct Cyc_Absyn_Exp*_tmp12D;struct Cyc_Absyn_Exp*_tmp12E;struct Cyc_Absyn_Exp*
_tmp12F;struct _dyneither_ptr*_tmp130;struct Cyc_Absyn_Exp*_tmp131;struct
_dyneither_ptr*_tmp132;struct Cyc_Absyn_Exp*_tmp133;struct Cyc_Absyn_Exp*_tmp134;
struct Cyc_Absyn_Exp*_tmp135;_LLDF: if(*((int*)_tmp124)!= 1)goto _LLE1;_tmp125=(
void*)((struct Cyc_Absyn_Var_e_struct*)_tmp124)->f2;if(_tmp125 <= (void*)1)goto
_LLE1;if(*((int*)_tmp125)!= 2)goto _LLE1;_tmp126=((struct Cyc_Absyn_Param_b_struct*)
_tmp125)->f1;_LLE0: _tmp128=_tmp126;goto _LLE2;_LLE1: if(*((int*)_tmp124)!= 1)goto
_LLE3;_tmp127=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp124)->f2;if(_tmp127 <= (
void*)1)goto _LLE3;if(*((int*)_tmp127)!= 3)goto _LLE3;_tmp128=((struct Cyc_Absyn_Local_b_struct*)
_tmp127)->f1;_LLE2: _tmp12A=_tmp128;goto _LLE4;_LLE3: if(*((int*)_tmp124)!= 1)goto
_LLE5;_tmp129=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp124)->f2;if(_tmp129 <= (
void*)1)goto _LLE5;if(*((int*)_tmp129)!= 4)goto _LLE5;_tmp12A=((struct Cyc_Absyn_Pat_b_struct*)
_tmp129)->f1;_LLE4: _tmp12C=_tmp12A;goto _LLE6;_LLE5: if(*((int*)_tmp124)!= 1)goto
_LLE7;_tmp12B=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp124)->f2;if(_tmp12B <= (
void*)1)goto _LLE7;if(*((int*)_tmp12B)!= 0)goto _LLE7;_tmp12C=((struct Cyc_Absyn_Global_b_struct*)
_tmp12B)->f1;_LLE6: if(!(_tmp12C->tq).real_const)return;goto _LLDE;_LLE7: if(*((int*)
_tmp124)!= 25)goto _LLE9;_tmp12D=((struct Cyc_Absyn_Subscript_e_struct*)_tmp124)->f1;
_tmp12E=((struct Cyc_Absyn_Subscript_e_struct*)_tmp124)->f2;_LLE8:{void*_tmp136=
Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp12D->topt))->v);
struct Cyc_Absyn_PtrInfo _tmp137;struct Cyc_Absyn_Tqual _tmp138;struct Cyc_Absyn_ArrayInfo
_tmp139;struct Cyc_Absyn_Tqual _tmp13A;struct Cyc_List_List*_tmp13B;_LLF6: if(
_tmp136 <= (void*)4)goto _LLFC;if(*((int*)_tmp136)!= 4)goto _LLF8;_tmp137=((struct
Cyc_Absyn_PointerType_struct*)_tmp136)->f1;_tmp138=_tmp137.elt_tq;_LLF7: _tmp13A=
_tmp138;goto _LLF9;_LLF8: if(*((int*)_tmp136)!= 7)goto _LLFA;_tmp139=((struct Cyc_Absyn_ArrayType_struct*)
_tmp136)->f1;_tmp13A=_tmp139.tq;_LLF9: if(!_tmp13A.real_const)return;goto _LLF5;
_LLFA: if(*((int*)_tmp136)!= 9)goto _LLFC;_tmp13B=((struct Cyc_Absyn_TupleType_struct*)
_tmp136)->f1;_LLFB: {unsigned int _tmp13D;int _tmp13E;struct _tuple7 _tmp13C=Cyc_Evexp_eval_const_uint_exp(
_tmp12E);_tmp13D=_tmp13C.f1;_tmp13E=_tmp13C.f2;if(!_tmp13E){({void*_tmp13F=0;Cyc_Tcutil_terr(
e->loc,({const char*_tmp140="tuple projection cannot use sizeof or offsetof";
_tag_dyneither(_tmp140,sizeof(char),47);}),_tag_dyneither(_tmp13F,sizeof(void*),
0));});return;}{struct _handler_cons _tmp141;_push_handler(& _tmp141);{int _tmp143=0;
if(setjmp(_tmp141.handler))_tmp143=1;if(!_tmp143){{struct _tuple9 _tmp145;struct
Cyc_Absyn_Tqual _tmp146;struct _tuple9*_tmp144=((struct _tuple9*(*)(struct Cyc_List_List*
x,int n))Cyc_List_nth)(_tmp13B,(int)_tmp13D);_tmp145=*_tmp144;_tmp146=_tmp145.f1;
if(!_tmp146.real_const){_npop_handler(0);return;}};_pop_handler();}else{void*
_tmp142=(void*)_exn_thrown;void*_tmp148=_tmp142;_LLFF: if(_tmp148 != Cyc_List_Nth)
goto _LL101;_LL100: return;_LL101:;_LL102:(void)_throw(_tmp148);_LLFE:;}}}goto
_LLF5;}_LLFC:;_LLFD: goto _LLF5;_LLF5:;}goto _LLDE;_LLE9: if(*((int*)_tmp124)!= 23)
goto _LLEB;_tmp12F=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp124)->f1;_tmp130=((
struct Cyc_Absyn_AggrMember_e_struct*)_tmp124)->f2;_LLEA:{void*_tmp149=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp12F->topt))->v);struct Cyc_Absyn_AggrInfo
_tmp14A;union Cyc_Absyn_AggrInfoU_union _tmp14B;struct Cyc_Absyn_Aggrdecl**_tmp14C;
struct Cyc_List_List*_tmp14D;_LL104: if(_tmp149 <= (void*)4)goto _LL108;if(*((int*)
_tmp149)!= 10)goto _LL106;_tmp14A=((struct Cyc_Absyn_AggrType_struct*)_tmp149)->f1;
_tmp14B=_tmp14A.aggr_info;if((((((struct Cyc_Absyn_AggrType_struct*)_tmp149)->f1).aggr_info).KnownAggr).tag
!= 1)goto _LL106;_tmp14C=(_tmp14B.KnownAggr).f1;_LL105: {struct Cyc_Absyn_Aggrfield*
sf=(struct Cyc_Absyn_Aggrdecl**)_tmp14C == 0?0: Cyc_Absyn_lookup_decl_field(*
_tmp14C,_tmp130);if(sf == 0  || !(sf->tq).real_const)return;goto _LL103;}_LL106: if(*((
int*)_tmp149)!= 11)goto _LL108;_tmp14D=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp149)->f2;_LL107: {struct Cyc_Absyn_Aggrfield*sf=Cyc_Absyn_lookup_field(
_tmp14D,_tmp130);if(sf == 0  || !(sf->tq).real_const)return;goto _LL103;}_LL108:;
_LL109: goto _LL103;_LL103:;}goto _LLDE;_LLEB: if(*((int*)_tmp124)!= 24)goto _LLED;
_tmp131=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp124)->f1;_tmp132=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp124)->f2;_LLEC:{void*_tmp14E=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp131->topt))->v);struct Cyc_Absyn_PtrInfo _tmp14F;void*_tmp150;
_LL10B: if(_tmp14E <= (void*)4)goto _LL10D;if(*((int*)_tmp14E)!= 4)goto _LL10D;
_tmp14F=((struct Cyc_Absyn_PointerType_struct*)_tmp14E)->f1;_tmp150=(void*)
_tmp14F.elt_typ;_LL10C:{void*_tmp151=Cyc_Tcutil_compress(_tmp150);struct Cyc_Absyn_AggrInfo
_tmp152;union Cyc_Absyn_AggrInfoU_union _tmp153;struct Cyc_Absyn_Aggrdecl**_tmp154;
struct Cyc_List_List*_tmp155;_LL110: if(_tmp151 <= (void*)4)goto _LL114;if(*((int*)
_tmp151)!= 10)goto _LL112;_tmp152=((struct Cyc_Absyn_AggrType_struct*)_tmp151)->f1;
_tmp153=_tmp152.aggr_info;if((((((struct Cyc_Absyn_AggrType_struct*)_tmp151)->f1).aggr_info).KnownAggr).tag
!= 1)goto _LL112;_tmp154=(_tmp153.KnownAggr).f1;_LL111: {struct Cyc_Absyn_Aggrfield*
sf=(struct Cyc_Absyn_Aggrdecl**)_tmp154 == 0?0: Cyc_Absyn_lookup_decl_field(*
_tmp154,_tmp132);if(sf == 0  || !(sf->tq).real_const)return;goto _LL10F;}_LL112: if(*((
int*)_tmp151)!= 11)goto _LL114;_tmp155=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp151)->f2;_LL113: {struct Cyc_Absyn_Aggrfield*sf=Cyc_Absyn_lookup_field(
_tmp155,_tmp132);if(sf == 0  || !(sf->tq).real_const)return;goto _LL10F;}_LL114:;
_LL115: goto _LL10F;_LL10F:;}goto _LL10A;_LL10D:;_LL10E: goto _LL10A;_LL10A:;}goto
_LLDE;_LLED: if(*((int*)_tmp124)!= 22)goto _LLEF;_tmp133=((struct Cyc_Absyn_Deref_e_struct*)
_tmp124)->f1;_LLEE:{void*_tmp156=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp133->topt))->v);struct Cyc_Absyn_PtrInfo _tmp157;struct Cyc_Absyn_Tqual
_tmp158;struct Cyc_Absyn_ArrayInfo _tmp159;struct Cyc_Absyn_Tqual _tmp15A;_LL117: if(
_tmp156 <= (void*)4)goto _LL11B;if(*((int*)_tmp156)!= 4)goto _LL119;_tmp157=((
struct Cyc_Absyn_PointerType_struct*)_tmp156)->f1;_tmp158=_tmp157.elt_tq;_LL118:
_tmp15A=_tmp158;goto _LL11A;_LL119: if(*((int*)_tmp156)!= 7)goto _LL11B;_tmp159=((
struct Cyc_Absyn_ArrayType_struct*)_tmp156)->f1;_tmp15A=_tmp159.tq;_LL11A: if(!
_tmp15A.real_const)return;goto _LL116;_LL11B:;_LL11C: goto _LL116;_LL116:;}goto
_LLDE;_LLEF: if(*((int*)_tmp124)!= 13)goto _LLF1;_tmp134=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmp124)->f1;_LLF0: _tmp135=_tmp134;goto _LLF2;_LLF1: if(*((int*)_tmp124)!= 14)goto
_LLF3;_tmp135=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp124)->f1;_LLF2: Cyc_Tcexp_check_writable(
te,_tmp135);return;_LLF3:;_LLF4: goto _LLDE;_LLDE:;}({struct Cyc_String_pa_struct
_tmp15D;_tmp15D.tag=0;_tmp15D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(e));{void*_tmp15B[1]={& _tmp15D};Cyc_Tcutil_terr(e->loc,({
const char*_tmp15C="attempt to write a const location: %s";_tag_dyneither(_tmp15C,
sizeof(char),38);}),_tag_dyneither(_tmp15B,sizeof(void*),1));}});}static void*Cyc_Tcexp_tcIncrement(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
e,void*i){Cyc_Tcexp_tcExpNoPromote(te,0,e);if(!Cyc_Absyn_is_lvalue(e))return({
void*_tmp15E=0;Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp15F="increment/decrement of non-lvalue";
_tag_dyneither(_tmp15F,sizeof(char),34);}),_tag_dyneither(_tmp15E,sizeof(void*),
0));});Cyc_Tcexp_check_writable(te,e);{void*t=(void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v;if(!Cyc_Tcutil_is_numeric(e)){void*telt=(void*)0;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(
t,& telt) || Cyc_Tcutil_is_zero_pointer_typ_elt(t,& telt) && (i == (void*)0  || i == (
void*)1)){if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(telt),(void*)1))({void*
_tmp160=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp161="can't perform arithmetic on abstract pointer type";
_tag_dyneither(_tmp161,sizeof(char),50);}),_tag_dyneither(_tmp160,sizeof(void*),
0));});if(Cyc_Tcutil_is_noalias_pointer(t))({void*_tmp162=0;Cyc_Tcutil_terr(e->loc,({
const char*_tmp163="can't perform arithmetic on non-aliasing pointer type";
_tag_dyneither(_tmp163,sizeof(char),54);}),_tag_dyneither(_tmp162,sizeof(void*),
0));});}else{({struct Cyc_String_pa_struct _tmp166;_tmp166.tag=0;_tmp166.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));{void*
_tmp164[1]={& _tmp166};Cyc_Tcutil_terr(e->loc,({const char*_tmp165="expecting arithmetic or ? type but found %s";
_tag_dyneither(_tmp165,sizeof(char),44);}),_tag_dyneither(_tmp164,sizeof(void*),
1));}});}}return t;}}static void*Cyc_Tcexp_tcConditional(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2,struct Cyc_Absyn_Exp*e3){struct _tuple0*_tmp169;int _tmp16A;struct Cyc_Tcexp_TestEnv
_tmp168=Cyc_Tcexp_tcTest(te,e1,({const char*_tmp167="conditional expression";
_tag_dyneither(_tmp167,sizeof(char),23);}));_tmp169=_tmp168.eq;_tmp16A=_tmp168.isTrue;
Cyc_Tcexp_tcExp(te,topt,e2);Cyc_Tcexp_tcExp(te,topt,e3);{void*t=Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_mk,Cyc_Tcenv_lookup_opt_type_vars(te));struct Cyc_List_List
_tmp16B=({struct Cyc_List_List _tmp172;_tmp172.hd=e3;_tmp172.tl=0;_tmp172;});
struct Cyc_List_List _tmp16C=({struct Cyc_List_List _tmp171;_tmp171.hd=e2;_tmp171.tl=(
struct Cyc_List_List*)& _tmp16B;_tmp171;});if(!Cyc_Tcutil_coerce_list(te,t,(struct
Cyc_List_List*)& _tmp16C)){({struct Cyc_String_pa_struct _tmp170;_tmp170.tag=0;
_tmp170.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e3->topt))->v));{struct Cyc_String_pa_struct
_tmp16F;_tmp16F.tag=0;_tmp16F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v));{
void*_tmp16D[2]={& _tmp16F,& _tmp170};Cyc_Tcutil_terr(loc,({const char*_tmp16E="conditional clause types do not match: %s != %s";
_tag_dyneither(_tmp16E,sizeof(char),48);}),_tag_dyneither(_tmp16D,sizeof(void*),
2));}}});Cyc_Tcutil_explain_failure();}return t;}}static void*Cyc_Tcexp_tcAnd(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Exp*e1,
struct Cyc_Absyn_Exp*e2){Cyc_Tcexp_tcTest(te,e1,({const char*_tmp173="logical-and expression";
_tag_dyneither(_tmp173,sizeof(char),23);}));Cyc_Tcexp_tcTest(te,e2,({const char*
_tmp174="logical-and expression";_tag_dyneither(_tmp174,sizeof(char),23);}));
return Cyc_Absyn_sint_typ;}static void*Cyc_Tcexp_tcOr(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
Cyc_Tcexp_tcTest(te,e1,({const char*_tmp175="logical-or expression";
_tag_dyneither(_tmp175,sizeof(char),22);}));Cyc_Tcexp_tcTest(te,e2,({const char*
_tmp176="logical-or expression";_tag_dyneither(_tmp176,sizeof(char),22);}));
return Cyc_Absyn_sint_typ;}static void*Cyc_Tcexp_tcAssignOp(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*
po,struct Cyc_Absyn_Exp*e2){struct _RegionHandle*_tmp177=Cyc_Tcenv_get_fnrgn(te);
Cyc_Tcexp_tcExpNoPromote(Cyc_Tcenv_enter_notreadctxt(_tmp177,te),0,e1);Cyc_Tcexp_tcExp(
te,(void**)((void**)((void*)&((struct Cyc_Core_Opt*)_check_null(e1->topt))->v)),
e2);{void*t1=(void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v;void*t2=(
void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v;{void*_tmp178=Cyc_Tcutil_compress(
t1);_LL11E: if(_tmp178 <= (void*)4)goto _LL120;if(*((int*)_tmp178)!= 7)goto _LL120;
_LL11F:({void*_tmp179=0;Cyc_Tcutil_terr(loc,({const char*_tmp17A="cannot assign to an array";
_tag_dyneither(_tmp17A,sizeof(char),26);}),_tag_dyneither(_tmp179,sizeof(void*),
0));});goto _LL11D;_LL120:;_LL121: goto _LL11D;_LL11D:;}if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(
t1),(void*)1))({void*_tmp17B=0;Cyc_Tcutil_terr(loc,({const char*_tmp17C="type is abstract (can't determine size).";
_tag_dyneither(_tmp17C,sizeof(char),41);}),_tag_dyneither(_tmp17B,sizeof(void*),
0));});if(!Cyc_Absyn_is_lvalue(e1))return({void*_tmp17D=0;Cyc_Tcexp_expr_err(te,
loc,topt,({const char*_tmp17E="assignment to non-lvalue";_tag_dyneither(_tmp17E,
sizeof(char),25);}),_tag_dyneither(_tmp17D,sizeof(void*),0));});Cyc_Tcexp_check_writable(
te,e1);if(po == 0){struct _RegionHandle*_tmp17F=Cyc_Tcenv_get_fnrgn(te);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(
_tmp17F,t2) && !Cyc_Tcutil_is_noalias_path(_tmp17F,e2))({void*_tmp180=0;Cyc_Tcutil_terr(
e2->loc,({const char*_tmp181="Cannot consume non-unique paths; do swap instead";
_tag_dyneither(_tmp181,sizeof(char),49);}),_tag_dyneither(_tmp180,sizeof(void*),
0));});if(!Cyc_Tcutil_coerce_assign(te,e2,t1)){void*_tmp182=({struct Cyc_String_pa_struct
_tmp186;_tmp186.tag=0;_tmp186.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2));{struct Cyc_String_pa_struct _tmp185;_tmp185.tag=0;
_tmp185.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t1));{void*_tmp183[2]={& _tmp185,& _tmp186};Cyc_Tcexp_expr_err(te,loc,topt,({const
char*_tmp184="type mismatch: %s != %s";_tag_dyneither(_tmp184,sizeof(char),24);}),
_tag_dyneither(_tmp183,sizeof(void*),2));}}});Cyc_Tcutil_unify(t1,t2);Cyc_Tcutil_explain_failure();
return _tmp182;}}else{void*_tmp187=(void*)po->v;void*_tmp188=Cyc_Tcexp_tcBinPrimop(
te,loc,0,_tmp187,e1,e2);if(!(Cyc_Tcutil_unify(_tmp188,t1) || Cyc_Tcutil_coerceable(
_tmp188) && Cyc_Tcutil_coerceable(t1))){void*_tmp189=({struct Cyc_String_pa_struct
_tmp18D;_tmp18D.tag=0;_tmp18D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2));{struct Cyc_String_pa_struct _tmp18C;_tmp18C.tag=0;
_tmp18C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t1));{void*_tmp18A[2]={& _tmp18C,& _tmp18D};Cyc_Tcexp_expr_err(te,loc,topt,({const
char*_tmp18B="Cannot use this operator in an assignment when the arguments have types %s and %s";
_tag_dyneither(_tmp18B,sizeof(char),82);}),_tag_dyneither(_tmp18A,sizeof(void*),
2));}}});Cyc_Tcutil_unify(_tmp188,t1);Cyc_Tcutil_explain_failure();return _tmp189;}
return _tmp188;}return t1;}}static void*Cyc_Tcexp_tcSeqExp(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2){Cyc_Tcexp_tcExp(te,0,e1);Cyc_Tcexp_tcExp(te,topt,e2);return(void*)((struct
Cyc_Core_Opt*)_check_null(e2->topt))->v;}static struct Cyc_Absyn_Datatypefield*Cyc_Tcexp_tcInjection(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*tu,struct _RegionHandle*r,
struct Cyc_List_List*inst,struct Cyc_List_List*fs){static struct Cyc_Absyn_DoubleType_struct
dbl={6,0};static void*dbl_typ=(void*)& dbl;struct Cyc_List_List*fields;void*t1=(
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;{void*_tmp18E=Cyc_Tcutil_compress(
t1);void*_tmp18F;void*_tmp190;_LL123: if((int)_tmp18E != 1)goto _LL125;_LL124: Cyc_Tcutil_unchecked_cast(
te,e,dbl_typ,(void*)1);t1=dbl_typ;goto _LL122;_LL125: if(_tmp18E <= (void*)4)goto
_LL129;if(*((int*)_tmp18E)!= 5)goto _LL127;_tmp18F=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp18E)->f2;if((int)_tmp18F != 0)goto _LL127;_LL126: goto _LL128;_LL127: if(*((int*)
_tmp18E)!= 5)goto _LL129;_tmp190=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp18E)->f2;
if((int)_tmp190 != 1)goto _LL129;_LL128: Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_sint_typ,(
void*)1);t1=Cyc_Absyn_sint_typ;goto _LL122;_LL129:;_LL12A: goto _LL122;_LL122:;}
for(fields=fs;fields != 0;fields=fields->tl){struct _tuple1*_tmp192;struct Cyc_List_List*
_tmp193;struct Cyc_Position_Segment*_tmp194;void*_tmp195;struct Cyc_Absyn_Datatypefield
_tmp191=*((struct Cyc_Absyn_Datatypefield*)fields->hd);_tmp192=_tmp191.name;
_tmp193=_tmp191.typs;_tmp194=_tmp191.loc;_tmp195=(void*)_tmp191.sc;if(_tmp193 == 
0  || _tmp193->tl != 0)continue;{void*t2=Cyc_Tcutil_rsubstitute(r,inst,(*((struct
_tuple9*)_tmp193->hd)).f2);if(Cyc_Tcutil_unify(t1,t2))return(struct Cyc_Absyn_Datatypefield*)((
struct Cyc_Absyn_Datatypefield*)fields->hd);}}for(fields=fs;fields != 0;fields=
fields->tl){struct _tuple1*_tmp197;struct Cyc_List_List*_tmp198;struct Cyc_Position_Segment*
_tmp199;void*_tmp19A;struct Cyc_Absyn_Datatypefield _tmp196=*((struct Cyc_Absyn_Datatypefield*)
fields->hd);_tmp197=_tmp196.name;_tmp198=_tmp196.typs;_tmp199=_tmp196.loc;
_tmp19A=(void*)_tmp196.sc;if(_tmp198 == 0  || _tmp198->tl != 0)continue;{void*t2=
Cyc_Tcutil_rsubstitute(r,inst,(*((struct _tuple9*)_tmp198->hd)).f2);if(Cyc_Tcutil_coerce_arg(
te,e,t2))return(struct Cyc_Absyn_Datatypefield*)((struct Cyc_Absyn_Datatypefield*)
fields->hd);}}({struct Cyc_String_pa_struct _tmp19E;_tmp19E.tag=0;_tmp19E.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));{struct
Cyc_String_pa_struct _tmp19D;_tmp19D.tag=0;_tmp19D.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string(tu));{void*_tmp19B[2]={& _tmp19D,&
_tmp19E};Cyc_Tcutil_terr(e->loc,({const char*_tmp19C="can't find a field in %s to inject a value of type %s";
_tag_dyneither(_tmp19C,sizeof(char),54);}),_tag_dyneither(_tmp19B,sizeof(void*),
2));}}});return 0;}static void*Cyc_Tcexp_tcFnCall(struct Cyc_Tcenv_Tenv*te_orig,
struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*
args,struct Cyc_Absyn_VarargCallInfo**vararg_call_info){struct Cyc_List_List*
_tmp1A0=args;struct _RegionHandle*_tmp1A1=Cyc_Tcenv_get_fnrgn(te_orig);struct Cyc_Tcenv_Tenv*
_tmp1A2=Cyc_Tcenv_new_block(_tmp1A1,loc,te_orig);Cyc_Tcexp_tcExp(_tmp1A2,0,e);{
void*t=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
void*_tmp1A3=t;struct Cyc_Absyn_PtrInfo _tmp1A4;void*_tmp1A5;struct Cyc_Absyn_Tqual
_tmp1A6;struct Cyc_Absyn_PtrAtts _tmp1A7;void*_tmp1A8;struct Cyc_Absyn_Conref*
_tmp1A9;struct Cyc_Absyn_Conref*_tmp1AA;struct Cyc_Absyn_Conref*_tmp1AB;_LL12C: if(
_tmp1A3 <= (void*)4)goto _LL12E;if(*((int*)_tmp1A3)!= 4)goto _LL12E;_tmp1A4=((
struct Cyc_Absyn_PointerType_struct*)_tmp1A3)->f1;_tmp1A5=(void*)_tmp1A4.elt_typ;
_tmp1A6=_tmp1A4.elt_tq;_tmp1A7=_tmp1A4.ptr_atts;_tmp1A8=(void*)_tmp1A7.rgn;
_tmp1A9=_tmp1A7.nullable;_tmp1AA=_tmp1A7.bounds;_tmp1AB=_tmp1A7.zero_term;_LL12D:
Cyc_Tcenv_check_rgn_accessible(_tmp1A2,loc,_tmp1A8);Cyc_Tcutil_check_nonzero_bound(
loc,_tmp1AA);{void*_tmp1AC=Cyc_Tcutil_compress(_tmp1A5);struct Cyc_Absyn_FnInfo
_tmp1AD;struct Cyc_List_List*_tmp1AE;struct Cyc_Core_Opt*_tmp1AF;void*_tmp1B0;
struct Cyc_List_List*_tmp1B1;int _tmp1B2;struct Cyc_Absyn_VarargInfo*_tmp1B3;struct
Cyc_List_List*_tmp1B4;struct Cyc_List_List*_tmp1B5;_LL131: if(_tmp1AC <= (void*)4)
goto _LL133;if(*((int*)_tmp1AC)!= 8)goto _LL133;_tmp1AD=((struct Cyc_Absyn_FnType_struct*)
_tmp1AC)->f1;_tmp1AE=_tmp1AD.tvars;_tmp1AF=_tmp1AD.effect;_tmp1B0=(void*)_tmp1AD.ret_typ;
_tmp1B1=_tmp1AD.args;_tmp1B2=_tmp1AD.c_varargs;_tmp1B3=_tmp1AD.cyc_varargs;
_tmp1B4=_tmp1AD.rgn_po;_tmp1B5=_tmp1AD.attributes;_LL132: if(topt != 0)Cyc_Tcutil_unify(
_tmp1B0,*topt);while(_tmp1A0 != 0  && _tmp1B1 != 0){struct Cyc_Absyn_Exp*e1=(struct
Cyc_Absyn_Exp*)_tmp1A0->hd;void*t2=(*((struct _tuple2*)_tmp1B1->hd)).f3;Cyc_Tcexp_tcExp(
_tmp1A2,(void**)& t2,e1);if(!Cyc_Tcutil_coerce_arg(_tmp1A2,e1,t2)){({struct Cyc_String_pa_struct
_tmp1B9;_tmp1B9.tag=0;_tmp1B9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2));{struct Cyc_String_pa_struct _tmp1B8;_tmp1B8.tag=0;
_tmp1B8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v));{void*_tmp1B6[2]={&
_tmp1B8,& _tmp1B9};Cyc_Tcutil_terr(e1->loc,({const char*_tmp1B7="actual argument has type \n\t%s\n but formal has type \n\t%s.";
_tag_dyneither(_tmp1B7,sizeof(char),57);}),_tag_dyneither(_tmp1B6,sizeof(void*),
2));}}});Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v,
t2);Cyc_Tcutil_explain_failure();}if(Cyc_Tcutil_is_noalias_pointer_or_aggr(
_tmp1A1,t2) && !Cyc_Tcutil_is_noalias_path(_tmp1A1,e1))({void*_tmp1BA=0;Cyc_Tcutil_terr(
e1->loc,({const char*_tmp1BB="Cannot consume non-unique paths; do swap instead";
_tag_dyneither(_tmp1BB,sizeof(char),49);}),_tag_dyneither(_tmp1BA,sizeof(void*),
0));});_tmp1A0=_tmp1A0->tl;_tmp1B1=_tmp1B1->tl;}{int args_already_checked=0;{
struct Cyc_List_List*a=_tmp1B5;for(0;a != 0;a=a->tl){void*_tmp1BC=(void*)a->hd;
void*_tmp1BD;int _tmp1BE;int _tmp1BF;_LL136: if(_tmp1BC <= (void*)17)goto _LL138;if(*((
int*)_tmp1BC)!= 3)goto _LL138;_tmp1BD=(void*)((struct Cyc_Absyn_Format_att_struct*)
_tmp1BC)->f1;_tmp1BE=((struct Cyc_Absyn_Format_att_struct*)_tmp1BC)->f2;_tmp1BF=((
struct Cyc_Absyn_Format_att_struct*)_tmp1BC)->f3;_LL137:{struct _handler_cons
_tmp1C0;_push_handler(& _tmp1C0);{int _tmp1C2=0;if(setjmp(_tmp1C0.handler))_tmp1C2=
1;if(!_tmp1C2){{struct Cyc_Absyn_Exp*_tmp1C3=((struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*
x,int n))Cyc_List_nth)(args,_tmp1BE - 1);struct Cyc_Core_Opt*fmt_args;if(_tmp1BF == 
0)fmt_args=0;else{fmt_args=({struct Cyc_Core_Opt*_tmp1C4=_cycalloc(sizeof(*
_tmp1C4));_tmp1C4->v=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,int i))Cyc_List_nth_tail)(
args,_tmp1BF - 1);_tmp1C4;});}args_already_checked=1;{struct _RegionHandle*_tmp1C5=
Cyc_Tcenv_get_fnrgn(te_orig);void*_tmp1C6=_tmp1BD;_LL13B: if((int)_tmp1C6 != 0)
goto _LL13D;_LL13C: Cyc_Tcexp_check_format_args(_tmp1A2,_tmp1C3,fmt_args,_tmp1C5,
Cyc_Formatstr_get_format_typs);goto _LL13A;_LL13D: if((int)_tmp1C6 != 1)goto _LL13A;
_LL13E: Cyc_Tcexp_check_format_args(_tmp1A2,_tmp1C3,fmt_args,_tmp1C5,Cyc_Formatstr_get_scanf_typs);
goto _LL13A;_LL13A:;}};_pop_handler();}else{void*_tmp1C1=(void*)_exn_thrown;void*
_tmp1C8=_tmp1C1;_LL140: if(_tmp1C8 != Cyc_List_Nth)goto _LL142;_LL141:({void*
_tmp1C9=0;Cyc_Tcutil_terr(loc,({const char*_tmp1CA="bad format arguments";
_tag_dyneither(_tmp1CA,sizeof(char),21);}),_tag_dyneither(_tmp1C9,sizeof(void*),
0));});goto _LL13F;_LL142:;_LL143:(void)_throw(_tmp1C8);_LL13F:;}}}goto _LL135;
_LL138:;_LL139: goto _LL135;_LL135:;}}if(_tmp1B1 != 0)({void*_tmp1CB=0;Cyc_Tcutil_terr(
loc,({const char*_tmp1CC="too few arguments for function";_tag_dyneither(_tmp1CC,
sizeof(char),31);}),_tag_dyneither(_tmp1CB,sizeof(void*),0));});else{if((_tmp1A0
!= 0  || _tmp1B2) || _tmp1B3 != 0){if(_tmp1B2)for(0;_tmp1A0 != 0;_tmp1A0=_tmp1A0->tl){
Cyc_Tcexp_tcExp(_tmp1A2,0,(struct Cyc_Absyn_Exp*)_tmp1A0->hd);}else{if(_tmp1B3 == 
0)({void*_tmp1CD=0;Cyc_Tcutil_terr(loc,({const char*_tmp1CE="too many arguments for function";
_tag_dyneither(_tmp1CE,sizeof(char),32);}),_tag_dyneither(_tmp1CD,sizeof(void*),
0));});else{void*_tmp1D0;int _tmp1D1;struct Cyc_Absyn_VarargInfo _tmp1CF=*_tmp1B3;
_tmp1D0=(void*)_tmp1CF.type;_tmp1D1=_tmp1CF.inject;{struct Cyc_Absyn_VarargCallInfo*
_tmp1D2=({struct Cyc_Absyn_VarargCallInfo*_tmp1EE=_cycalloc(sizeof(*_tmp1EE));
_tmp1EE->num_varargs=0;_tmp1EE->injectors=0;_tmp1EE->vai=(struct Cyc_Absyn_VarargInfo*)
_tmp1B3;_tmp1EE;});*vararg_call_info=(struct Cyc_Absyn_VarargCallInfo*)_tmp1D2;
if(!_tmp1D1)for(0;_tmp1A0 != 0;_tmp1A0=_tmp1A0->tl){struct Cyc_Absyn_Exp*e1=(
struct Cyc_Absyn_Exp*)_tmp1A0->hd;++ _tmp1D2->num_varargs;Cyc_Tcexp_tcExp(_tmp1A2,(
void**)& _tmp1D0,e1);if(!Cyc_Tcutil_coerce_arg(_tmp1A2,e1,_tmp1D0)){({struct Cyc_String_pa_struct
_tmp1D6;_tmp1D6.tag=0;_tmp1D6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v));{
struct Cyc_String_pa_struct _tmp1D5;_tmp1D5.tag=0;_tmp1D5.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp1D0));{void*_tmp1D3[2]={& _tmp1D5,&
_tmp1D6};Cyc_Tcutil_terr(loc,({const char*_tmp1D4="vararg requires type %s but argument has type %s";
_tag_dyneither(_tmp1D4,sizeof(char),49);}),_tag_dyneither(_tmp1D3,sizeof(void*),
2));}}});Cyc_Tcutil_explain_failure();}if(Cyc_Tcutil_is_noalias_pointer_or_aggr(
_tmp1A1,_tmp1D0) && !Cyc_Tcutil_is_noalias_path(_tmp1A1,e1))({void*_tmp1D7=0;Cyc_Tcutil_terr(
e1->loc,({const char*_tmp1D8="Cannot consume non-unique paths; do swap instead";
_tag_dyneither(_tmp1D8,sizeof(char),49);}),_tag_dyneither(_tmp1D7,sizeof(void*),
0));});}else{void*_tmp1D9=Cyc_Tcutil_compress(_tmp1D0);struct Cyc_Absyn_DatatypeInfo
_tmp1DA;union Cyc_Absyn_DatatypeInfoU_union _tmp1DB;struct Cyc_Absyn_Datatypedecl**
_tmp1DC;struct Cyc_Absyn_Datatypedecl*_tmp1DD;struct Cyc_List_List*_tmp1DE;struct
Cyc_Core_Opt*_tmp1DF;_LL145: if(_tmp1D9 <= (void*)4)goto _LL147;if(*((int*)_tmp1D9)
!= 2)goto _LL147;_tmp1DA=((struct Cyc_Absyn_DatatypeType_struct*)_tmp1D9)->f1;
_tmp1DB=_tmp1DA.datatype_info;if((((((struct Cyc_Absyn_DatatypeType_struct*)
_tmp1D9)->f1).datatype_info).KnownDatatype).tag != 1)goto _LL147;_tmp1DC=(_tmp1DB.KnownDatatype).f1;
_tmp1DD=*_tmp1DC;_tmp1DE=_tmp1DA.targs;_tmp1DF=_tmp1DA.rgn;_LL146: {struct Cyc_Absyn_Datatypedecl*
_tmp1E0=*Cyc_Tcenv_lookup_datatypedecl(_tmp1A2,loc,_tmp1DD->name);struct Cyc_List_List*
fields=0;if(_tmp1E0->fields == 0)({struct Cyc_String_pa_struct _tmp1E3;_tmp1E3.tag=
0;_tmp1E3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
_tmp1D0));{void*_tmp1E1[1]={& _tmp1E3};Cyc_Tcutil_terr(loc,({const char*_tmp1E2="can't inject into %s";
_tag_dyneither(_tmp1E2,sizeof(char),21);}),_tag_dyneither(_tmp1E1,sizeof(void*),
1));}});else{fields=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(
_tmp1E0->fields))->v;}if(!Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp1DF))->v,Cyc_Tcenv_curr_rgn(_tmp1A2)))({void*_tmp1E4=0;Cyc_Tcutil_terr(
loc,({const char*_tmp1E5="bad region for injected varargs";_tag_dyneither(_tmp1E5,
sizeof(char),32);}),_tag_dyneither(_tmp1E4,sizeof(void*),0));});{struct
_RegionHandle*_tmp1E6=Cyc_Tcenv_get_fnrgn(_tmp1A2);{struct Cyc_List_List*_tmp1E7=((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(_tmp1E6,_tmp1E6,_tmp1E0->tvs,_tmp1DE);
for(0;_tmp1A0 != 0;_tmp1A0=_tmp1A0->tl){++ _tmp1D2->num_varargs;{struct Cyc_Absyn_Exp*
e1=(struct Cyc_Absyn_Exp*)_tmp1A0->hd;if(!args_already_checked){Cyc_Tcexp_tcExp(
_tmp1A2,0,e1);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(_tmp1E6,(void*)((struct
Cyc_Core_Opt*)_check_null(e1->topt))->v) && !Cyc_Tcutil_is_noalias_path(_tmp1E6,
e1))({void*_tmp1E8=0;Cyc_Tcutil_terr(e1->loc,({const char*_tmp1E9="Cannot consume non-unique paths; do swap instead";
_tag_dyneither(_tmp1E9,sizeof(char),49);}),_tag_dyneither(_tmp1E8,sizeof(void*),
0));});}{struct Cyc_Absyn_Datatypefield*_tmp1EA=Cyc_Tcexp_tcInjection(_tmp1A2,e1,
_tmp1D0,_tmp1E6,_tmp1E7,fields);if(_tmp1EA != 0)_tmp1D2->injectors=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp1D2->injectors,({
struct Cyc_List_List*_tmp1EB=_cycalloc(sizeof(*_tmp1EB));_tmp1EB->hd=(struct Cyc_Absyn_Datatypefield*)
_tmp1EA;_tmp1EB->tl=0;_tmp1EB;}));}}}}goto _LL144;}}_LL147:;_LL148:({void*_tmp1EC=
0;Cyc_Tcutil_terr(loc,({const char*_tmp1ED="bad inject vararg type";
_tag_dyneither(_tmp1ED,sizeof(char),23);}),_tag_dyneither(_tmp1EC,sizeof(void*),
0));});goto _LL144;_LL144:;}}}}}}Cyc_Tcenv_check_effect_accessible(_tmp1A2,loc,(
void*)((struct Cyc_Core_Opt*)_check_null(_tmp1AF))->v);Cyc_Tcenv_check_rgn_partial_order(
_tmp1A2,loc,_tmp1B4);return _tmp1B0;}_LL133:;_LL134: return({void*_tmp1EF=0;Cyc_Tcexp_expr_err(
_tmp1A2,loc,topt,({const char*_tmp1F0="expected pointer to function";
_tag_dyneither(_tmp1F0,sizeof(char),29);}),_tag_dyneither(_tmp1EF,sizeof(void*),
0));});_LL130:;}_LL12E:;_LL12F: return({void*_tmp1F1=0;Cyc_Tcexp_expr_err(_tmp1A2,
loc,topt,({const char*_tmp1F2="expected pointer to function";_tag_dyneither(
_tmp1F2,sizeof(char),29);}),_tag_dyneither(_tmp1F1,sizeof(void*),0));});_LL12B:;}}
static void*Cyc_Tcexp_tcThrow(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*e){Cyc_Tcexp_tcExp(te,(void**)& Cyc_Absyn_exn_typ,
e);if(!Cyc_Tcutil_coerce_arg(te,e,Cyc_Absyn_exn_typ))({struct Cyc_String_pa_struct
_tmp1F5;_tmp1F5.tag=0;_tmp1F5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v));{
void*_tmp1F3[1]={& _tmp1F5};Cyc_Tcutil_terr(loc,({const char*_tmp1F4="expected datatype exn but found %s";
_tag_dyneither(_tmp1F4,sizeof(char),35);}),_tag_dyneither(_tmp1F3,sizeof(void*),
1));}});return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}static void*
Cyc_Tcexp_tcInstantiate(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,
void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*ts){Cyc_Tcexp_tcExpNoInst(
te,0,e);(void*)(((struct Cyc_Core_Opt*)_check_null(e->topt))->v=(void*)Cyc_Absyn_pointer_expand((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,0));{void*t1=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);{void*_tmp1F6=t1;struct Cyc_Absyn_PtrInfo
_tmp1F7;void*_tmp1F8;struct Cyc_Absyn_Tqual _tmp1F9;struct Cyc_Absyn_PtrAtts _tmp1FA;
void*_tmp1FB;struct Cyc_Absyn_Conref*_tmp1FC;struct Cyc_Absyn_Conref*_tmp1FD;
struct Cyc_Absyn_Conref*_tmp1FE;_LL14A: if(_tmp1F6 <= (void*)4)goto _LL14C;if(*((int*)
_tmp1F6)!= 4)goto _LL14C;_tmp1F7=((struct Cyc_Absyn_PointerType_struct*)_tmp1F6)->f1;
_tmp1F8=(void*)_tmp1F7.elt_typ;_tmp1F9=_tmp1F7.elt_tq;_tmp1FA=_tmp1F7.ptr_atts;
_tmp1FB=(void*)_tmp1FA.rgn;_tmp1FC=_tmp1FA.nullable;_tmp1FD=_tmp1FA.bounds;
_tmp1FE=_tmp1FA.zero_term;_LL14B:{void*_tmp1FF=Cyc_Tcutil_compress(_tmp1F8);
struct Cyc_Absyn_FnInfo _tmp200;struct Cyc_List_List*_tmp201;struct Cyc_Core_Opt*
_tmp202;void*_tmp203;struct Cyc_List_List*_tmp204;int _tmp205;struct Cyc_Absyn_VarargInfo*
_tmp206;struct Cyc_List_List*_tmp207;struct Cyc_List_List*_tmp208;_LL14F: if(
_tmp1FF <= (void*)4)goto _LL151;if(*((int*)_tmp1FF)!= 8)goto _LL151;_tmp200=((
struct Cyc_Absyn_FnType_struct*)_tmp1FF)->f1;_tmp201=_tmp200.tvars;_tmp202=
_tmp200.effect;_tmp203=(void*)_tmp200.ret_typ;_tmp204=_tmp200.args;_tmp205=
_tmp200.c_varargs;_tmp206=_tmp200.cyc_varargs;_tmp207=_tmp200.rgn_po;_tmp208=
_tmp200.attributes;_LL150: {struct _RegionHandle*_tmp209=Cyc_Tcenv_get_fnrgn(te);
struct Cyc_List_List*instantiation=0;for(0;ts != 0  && _tmp201 != 0;(ts=ts->tl,
_tmp201=_tmp201->tl)){void*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp201->hd);
Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(te),k,1,(void*)ts->hd);
instantiation=({struct Cyc_List_List*_tmp20A=_region_malloc(_tmp209,sizeof(*
_tmp20A));_tmp20A->hd=({struct _tuple5*_tmp20B=_region_malloc(_tmp209,sizeof(*
_tmp20B));_tmp20B->f1=(struct Cyc_Absyn_Tvar*)_tmp201->hd;_tmp20B->f2=(void*)ts->hd;
_tmp20B;});_tmp20A->tl=instantiation;_tmp20A;});}if(ts != 0)return({void*_tmp20C=
0;Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp20D="too many type variables in instantiation";
_tag_dyneither(_tmp20D,sizeof(char),41);}),_tag_dyneither(_tmp20C,sizeof(void*),
0));});{void*new_fn_typ=Cyc_Tcutil_rsubstitute(_tmp209,instantiation,(void*)({
struct Cyc_Absyn_FnType_struct*_tmp212=_cycalloc(sizeof(*_tmp212));_tmp212[0]=({
struct Cyc_Absyn_FnType_struct _tmp213;_tmp213.tag=8;_tmp213.f1=({struct Cyc_Absyn_FnInfo
_tmp214;_tmp214.tvars=_tmp201;_tmp214.effect=_tmp202;_tmp214.ret_typ=(void*)
_tmp203;_tmp214.args=_tmp204;_tmp214.c_varargs=_tmp205;_tmp214.cyc_varargs=
_tmp206;_tmp214.rgn_po=_tmp207;_tmp214.attributes=_tmp208;_tmp214;});_tmp213;});
_tmp212;}));return(void*)({struct Cyc_Absyn_PointerType_struct*_tmp20E=_cycalloc(
sizeof(*_tmp20E));_tmp20E[0]=({struct Cyc_Absyn_PointerType_struct _tmp20F;_tmp20F.tag=
4;_tmp20F.f1=({struct Cyc_Absyn_PtrInfo _tmp210;_tmp210.elt_typ=(void*)new_fn_typ;
_tmp210.elt_tq=_tmp1F9;_tmp210.ptr_atts=({struct Cyc_Absyn_PtrAtts _tmp211;_tmp211.rgn=(
void*)_tmp1FB;_tmp211.nullable=_tmp1FC;_tmp211.bounds=_tmp1FD;_tmp211.zero_term=
_tmp1FE;_tmp211.ptrloc=0;_tmp211;});_tmp210;});_tmp20F;});_tmp20E;});}}_LL151:;
_LL152: goto _LL14E;_LL14E:;}goto _LL149;_LL14C:;_LL14D: goto _LL149;_LL149:;}return({
struct Cyc_String_pa_struct _tmp217;_tmp217.tag=0;_tmp217.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));{void*_tmp215[1]={& _tmp217};Cyc_Tcexp_expr_err(
te,loc,topt,({const char*_tmp216="expecting polymorphic type but found %s";
_tag_dyneither(_tmp216,sizeof(char),40);}),_tag_dyneither(_tmp215,sizeof(void*),
1));}});}}static void*Cyc_Tcexp_tcCast(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,void*t,struct Cyc_Absyn_Exp*e,void**c){Cyc_Tcutil_check_type(loc,
te,Cyc_Tcenv_lookup_type_vars(te),(void*)1,1,t);Cyc_Tcexp_tcExp(te,(void**)& t,e);{
void*t2=(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;if(Cyc_Tcutil_silent_castable(
te,loc,t2,t))*((void**)_check_null(c))=(void*)1;else{void*crc=Cyc_Tcutil_castable(
te,loc,t2,t);if(crc != (void*)0)*((void**)_check_null(c))=crc;else{if(Cyc_Tcutil_zero_to_null(
te,t,e))*((void**)_check_null(c))=(void*)1;else{Cyc_Tcutil_unify(t2,t);{void*
_tmp218=({struct Cyc_String_pa_struct _tmp21C;_tmp21C.tag=0;_tmp21C.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));{struct Cyc_String_pa_struct
_tmp21B;_tmp21B.tag=0;_tmp21B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2));{void*_tmp219[2]={& _tmp21B,& _tmp21C};Cyc_Tcexp_expr_err(
te,loc,(void**)& t,({const char*_tmp21A="cannot cast %s to %s";_tag_dyneither(
_tmp21A,sizeof(char),21);}),_tag_dyneither(_tmp219,sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();
return _tmp218;}}}}{struct _tuple0 _tmp21E=({struct _tuple0 _tmp21D;_tmp21D.f1=(void*)
e->r;_tmp21D.f2=Cyc_Tcutil_compress(t);_tmp21D;});void*_tmp21F;struct Cyc_Absyn_MallocInfo
_tmp220;int _tmp221;void*_tmp222;struct Cyc_Absyn_PtrInfo _tmp223;struct Cyc_Absyn_PtrAtts
_tmp224;struct Cyc_Absyn_Conref*_tmp225;struct Cyc_Absyn_Conref*_tmp226;struct Cyc_Absyn_Conref*
_tmp227;_LL154: _tmp21F=_tmp21E.f1;if(*((int*)_tmp21F)!= 35)goto _LL156;_tmp220=((
struct Cyc_Absyn_Malloc_e_struct*)_tmp21F)->f1;_tmp221=_tmp220.fat_result;_tmp222=
_tmp21E.f2;if(_tmp222 <= (void*)4)goto _LL156;if(*((int*)_tmp222)!= 4)goto _LL156;
_tmp223=((struct Cyc_Absyn_PointerType_struct*)_tmp222)->f1;_tmp224=_tmp223.ptr_atts;
_tmp225=_tmp224.nullable;_tmp226=_tmp224.bounds;_tmp227=_tmp224.zero_term;_LL155:
if((_tmp221  && !((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,
_tmp227)) && ((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,
_tmp225)){void*_tmp228=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp226);struct
Cyc_Absyn_Exp*_tmp229;_LL159: if(_tmp228 <= (void*)1)goto _LL15B;if(*((int*)_tmp228)
!= 0)goto _LL15B;_tmp229=((struct Cyc_Absyn_Upper_b_struct*)_tmp228)->f1;_LL15A:
if((Cyc_Evexp_eval_const_uint_exp(_tmp229)).f1 == 1)({void*_tmp22A=0;Cyc_Tcutil_warn(
loc,({const char*_tmp22B="cast from ? pointer to * pointer will lose size information";
_tag_dyneither(_tmp22B,sizeof(char),60);}),_tag_dyneither(_tmp22A,sizeof(void*),
0));});goto _LL158;_LL15B:;_LL15C: goto _LL158;_LL158:;}goto _LL153;_LL156:;_LL157:
goto _LL153;_LL153:;}return t;}}static void*Cyc_Tcexp_tcAddress(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Exp*e0,void**topt,struct Cyc_Absyn_Exp*
e){void**_tmp22C=0;struct Cyc_Absyn_Tqual _tmp22D=Cyc_Absyn_empty_tqual(0);if(topt
!= 0){void*_tmp22E=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo _tmp22F;
void*_tmp230;struct Cyc_Absyn_Tqual _tmp231;struct Cyc_Absyn_PtrAtts _tmp232;struct
Cyc_Absyn_Conref*_tmp233;_LL15E: if(_tmp22E <= (void*)4)goto _LL160;if(*((int*)
_tmp22E)!= 4)goto _LL160;_tmp22F=((struct Cyc_Absyn_PointerType_struct*)_tmp22E)->f1;
_tmp230=(void*)_tmp22F.elt_typ;_tmp231=_tmp22F.elt_tq;_tmp232=_tmp22F.ptr_atts;
_tmp233=_tmp232.zero_term;_LL15F: _tmp22C=({void**_tmp234=_cycalloc(sizeof(*
_tmp234));_tmp234[0]=_tmp230;_tmp234;});_tmp22D=_tmp231;goto _LL15D;_LL160:;
_LL161: goto _LL15D;_LL15D:;}{struct _RegionHandle*_tmp235=Cyc_Tcenv_get_fnrgn(te);
Cyc_Tcexp_tcExpNoInst(Cyc_Tcenv_clear_notreadctxt(_tmp235,te),_tmp22C,e);if(Cyc_Tcutil_is_noalias_path(
_tmp235,e))({void*_tmp236=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp237="Cannot take the address of an alias-free path";
_tag_dyneither(_tmp237,sizeof(char),46);}),_tag_dyneither(_tmp236,sizeof(void*),
0));});{void*_tmp238=(void*)e->r;struct Cyc_Absyn_Exp*_tmp239;struct Cyc_Absyn_Exp*
_tmp23A;_LL163: if(*((int*)_tmp238)!= 25)goto _LL165;_tmp239=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp238)->f1;_tmp23A=((struct Cyc_Absyn_Subscript_e_struct*)_tmp238)->f2;_LL164:{
void*_tmp23B=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp239->topt))->v);_LL168: if(_tmp23B <= (void*)4)goto _LL16A;if(*((int*)_tmp23B)
!= 9)goto _LL16A;_LL169: goto _LL167;_LL16A:;_LL16B:(void*)(e0->r=(void*)((void*)(
Cyc_Absyn_add_exp(_tmp239,_tmp23A,0))->r));return Cyc_Tcexp_tcPlus(te,_tmp239,
_tmp23A);_LL167:;}goto _LL162;_LL165:;_LL166: goto _LL162;_LL162:;}{int _tmp23D;void*
_tmp23E;struct _tuple6 _tmp23C=Cyc_Tcutil_addressof_props(te,e);_tmp23D=_tmp23C.f1;
_tmp23E=_tmp23C.f2;{struct Cyc_Absyn_Tqual tq=Cyc_Absyn_empty_tqual(0);if(_tmp23D){
tq.print_const=1;tq.real_const=1;}{void*t=Cyc_Absyn_at_typ((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,_tmp23E,tq,Cyc_Absyn_false_conref);return t;}}}}}static
void*Cyc_Tcexp_tcSizeof(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,
void**topt,void*t){if(te->allow_valueof)return Cyc_Absyn_uint_typ;Cyc_Tcutil_check_type(
loc,te,Cyc_Tcenv_lookup_type_vars(te),(void*)1,1,t);if(!Cyc_Evexp_okay_szofarg(t))({
struct Cyc_String_pa_struct _tmp241;_tmp241.tag=0;_tmp241.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));{void*_tmp23F[1]={& _tmp241};Cyc_Tcutil_terr(
loc,({const char*_tmp240="sizeof applied to type %s, which has unknown size here";
_tag_dyneither(_tmp240,sizeof(char),55);}),_tag_dyneither(_tmp23F,sizeof(void*),
1));}});if(topt != 0){void*_tmp242=Cyc_Tcutil_compress(*topt);void*_tmp243;_LL16D:
if(_tmp242 <= (void*)4)goto _LL16F;if(*((int*)_tmp242)!= 18)goto _LL16F;_tmp243=(
void*)((struct Cyc_Absyn_TagType_struct*)_tmp242)->f1;_LL16E: {struct Cyc_Absyn_Exp*
_tmp244=Cyc_Absyn_sizeoftyp_exp(t,0);struct Cyc_Absyn_ValueofType_struct*_tmp245=({
struct Cyc_Absyn_ValueofType_struct*_tmp246=_cycalloc(sizeof(*_tmp246));_tmp246[0]=({
struct Cyc_Absyn_ValueofType_struct _tmp247;_tmp247.tag=17;_tmp247.f1=_tmp244;
_tmp247;});_tmp246;});if(Cyc_Tcutil_unify(_tmp243,(void*)_tmp245))return _tmp242;
goto _LL16C;}_LL16F:;_LL170: goto _LL16C;_LL16C:;}return Cyc_Absyn_uint_typ;}int Cyc_Tcexp_structfield_has_name(
struct _dyneither_ptr*n,struct Cyc_Absyn_Aggrfield*sf){return Cyc_strcmp((struct
_dyneither_ptr)*n,(struct _dyneither_ptr)*sf->name)== 0;}static void*Cyc_Tcexp_tcOffsetof(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,void*t,void*n){
Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(te),(void*)1,1,t);{void*
_tmp248=n;struct _dyneither_ptr*_tmp249;unsigned int _tmp24A;_LL172: if(*((int*)
_tmp248)!= 0)goto _LL174;_tmp249=((struct Cyc_Absyn_StructField_struct*)_tmp248)->f1;
_LL173: {int bad_type=1;{void*_tmp24B=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo
_tmp24C;union Cyc_Absyn_AggrInfoU_union _tmp24D;struct Cyc_Absyn_Aggrdecl**_tmp24E;
struct Cyc_List_List*_tmp24F;_LL177: if(_tmp24B <= (void*)4)goto _LL17B;if(*((int*)
_tmp24B)!= 10)goto _LL179;_tmp24C=((struct Cyc_Absyn_AggrType_struct*)_tmp24B)->f1;
_tmp24D=_tmp24C.aggr_info;if((((((struct Cyc_Absyn_AggrType_struct*)_tmp24B)->f1).aggr_info).KnownAggr).tag
!= 1)goto _LL179;_tmp24E=(_tmp24D.KnownAggr).f1;_LL178: if((*_tmp24E)->impl == 0)
goto _LL176;if(!((int(*)(int(*pred)(struct _dyneither_ptr*,struct Cyc_Absyn_Aggrfield*),
struct _dyneither_ptr*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcexp_structfield_has_name,
_tmp249,((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmp24E)->impl))->fields))({
struct Cyc_String_pa_struct _tmp252;_tmp252.tag=0;_tmp252.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)*_tmp249);{void*_tmp250[1]={& _tmp252};Cyc_Tcutil_terr(loc,({
const char*_tmp251="no field of struct/union has name %s";_tag_dyneither(_tmp251,
sizeof(char),37);}),_tag_dyneither(_tmp250,sizeof(void*),1));}});bad_type=0;goto
_LL176;_LL179: if(*((int*)_tmp24B)!= 11)goto _LL17B;_tmp24F=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp24B)->f2;_LL17A: if(!((int(*)(int(*pred)(struct _dyneither_ptr*,struct Cyc_Absyn_Aggrfield*),
struct _dyneither_ptr*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcexp_structfield_has_name,
_tmp249,_tmp24F))({struct Cyc_String_pa_struct _tmp255;_tmp255.tag=0;_tmp255.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp249);{void*_tmp253[1]={& _tmp255};
Cyc_Tcutil_terr(loc,({const char*_tmp254="no field of struct/union has name %s";
_tag_dyneither(_tmp254,sizeof(char),37);}),_tag_dyneither(_tmp253,sizeof(void*),
1));}});bad_type=0;goto _LL176;_LL17B:;_LL17C: goto _LL176;_LL176:;}if(bad_type)({
struct Cyc_String_pa_struct _tmp258;_tmp258.tag=0;_tmp258.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));{void*_tmp256[1]={& _tmp258};Cyc_Tcutil_terr(
loc,({const char*_tmp257="%s is not a known struct/union type";_tag_dyneither(
_tmp257,sizeof(char),36);}),_tag_dyneither(_tmp256,sizeof(void*),1));}});goto
_LL171;}_LL174: if(*((int*)_tmp248)!= 1)goto _LL171;_tmp24A=((struct Cyc_Absyn_TupleIndex_struct*)
_tmp248)->f1;_LL175: {int bad_type=1;{void*_tmp259=Cyc_Tcutil_compress(t);struct
Cyc_Absyn_AggrInfo _tmp25A;union Cyc_Absyn_AggrInfoU_union _tmp25B;struct Cyc_Absyn_Aggrdecl**
_tmp25C;struct Cyc_List_List*_tmp25D;struct Cyc_List_List*_tmp25E;struct Cyc_Absyn_DatatypeFieldInfo
_tmp25F;union Cyc_Absyn_DatatypeFieldInfoU_union _tmp260;struct Cyc_Absyn_Datatypefield*
_tmp261;_LL17E: if(_tmp259 <= (void*)4)goto _LL186;if(*((int*)_tmp259)!= 10)goto
_LL180;_tmp25A=((struct Cyc_Absyn_AggrType_struct*)_tmp259)->f1;_tmp25B=_tmp25A.aggr_info;
if((((((struct Cyc_Absyn_AggrType_struct*)_tmp259)->f1).aggr_info).KnownAggr).tag
!= 1)goto _LL180;_tmp25C=(_tmp25B.KnownAggr).f1;_LL17F: if((*_tmp25C)->impl == 0)
goto _LL17D;_tmp25D=((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmp25C)->impl))->fields;
goto _LL181;_LL180: if(*((int*)_tmp259)!= 11)goto _LL182;_tmp25D=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp259)->f2;_LL181: if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp25D)
<= _tmp24A)({struct Cyc_Int_pa_struct _tmp265;_tmp265.tag=1;_tmp265.f1=(
unsigned long)((int)_tmp24A);{struct Cyc_Int_pa_struct _tmp264;_tmp264.tag=1;
_tmp264.f1=(unsigned long)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp25D);{void*_tmp262[2]={& _tmp264,& _tmp265};Cyc_Tcutil_terr(loc,({const char*
_tmp263="struct/union has too few components: %d <= %d";_tag_dyneither(_tmp263,
sizeof(char),46);}),_tag_dyneither(_tmp262,sizeof(void*),2));}}});bad_type=0;
goto _LL17D;_LL182: if(*((int*)_tmp259)!= 9)goto _LL184;_tmp25E=((struct Cyc_Absyn_TupleType_struct*)
_tmp259)->f1;_LL183: if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp25E)
<= _tmp24A)({struct Cyc_Int_pa_struct _tmp269;_tmp269.tag=1;_tmp269.f1=(
unsigned long)((int)_tmp24A);{struct Cyc_Int_pa_struct _tmp268;_tmp268.tag=1;
_tmp268.f1=(unsigned long)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp25E);{void*_tmp266[2]={& _tmp268,& _tmp269};Cyc_Tcutil_terr(loc,({const char*
_tmp267="tuple has too few components: %d <= %d";_tag_dyneither(_tmp267,sizeof(
char),39);}),_tag_dyneither(_tmp266,sizeof(void*),2));}}});bad_type=0;goto _LL17D;
_LL184: if(*((int*)_tmp259)!= 3)goto _LL186;_tmp25F=((struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp259)->f1;_tmp260=_tmp25F.field_info;if((((((struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp259)->f1).field_info).KnownDatatypefield).tag != 1)goto _LL186;_tmp261=(
_tmp260.KnownDatatypefield).f2;_LL185: if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp261->typs)< _tmp24A)({struct Cyc_Int_pa_struct _tmp26D;_tmp26D.tag=1;_tmp26D.f1=(
unsigned long)((int)_tmp24A);{struct Cyc_Int_pa_struct _tmp26C;_tmp26C.tag=1;
_tmp26C.f1=(unsigned long)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp261->typs);{void*_tmp26A[2]={& _tmp26C,& _tmp26D};Cyc_Tcutil_terr(loc,({const
char*_tmp26B="datatype field has too few components: %d < %d";_tag_dyneither(
_tmp26B,sizeof(char),47);}),_tag_dyneither(_tmp26A,sizeof(void*),2));}}});
bad_type=0;goto _LL17D;_LL186:;_LL187: goto _LL17D;_LL17D:;}if(bad_type)({struct Cyc_String_pa_struct
_tmp270;_tmp270.tag=0;_tmp270.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t));{void*_tmp26E[1]={& _tmp270};Cyc_Tcutil_terr(loc,({
const char*_tmp26F="%s is not a known type";_tag_dyneither(_tmp26F,sizeof(char),
23);}),_tag_dyneither(_tmp26E,sizeof(void*),1));}});goto _LL171;}_LL171:;}return
Cyc_Absyn_uint_typ;}static void*Cyc_Tcexp_tcDeref(struct Cyc_Tcenv_Tenv*te_orig,
struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e){struct
_RegionHandle*_tmp271=Cyc_Tcenv_get_fnrgn(te_orig);struct Cyc_Tcenv_Tenv*_tmp272=
Cyc_Tcenv_clear_notreadctxt(_tmp271,te_orig);Cyc_Tcexp_tcExp(_tmp272,0,e);{void*
t=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
void*_tmp273=t;struct Cyc_Absyn_PtrInfo _tmp274;void*_tmp275;struct Cyc_Absyn_PtrAtts
_tmp276;void*_tmp277;struct Cyc_Absyn_Conref*_tmp278;struct Cyc_Absyn_Conref*
_tmp279;_LL189: if(_tmp273 <= (void*)4)goto _LL18D;if(*((int*)_tmp273)!= 0)goto
_LL18B;_LL18A: {struct Cyc_List_List*_tmp27A=Cyc_Tcenv_lookup_type_vars(_tmp272);
void*_tmp27B=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_mk,({struct Cyc_Core_Opt*
_tmp286=_cycalloc(sizeof(*_tmp286));_tmp286->v=_tmp27A;_tmp286;}));void*_tmp27C=
Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_trk,({struct Cyc_Core_Opt*
_tmp285=_cycalloc(sizeof(*_tmp285));_tmp285->v=_tmp27A;_tmp285;}));struct Cyc_Absyn_Conref*
_tmp27D=Cyc_Absyn_empty_conref();struct Cyc_Absyn_Conref*_tmp27E=((struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)();struct Cyc_Absyn_PtrAtts _tmp27F=({struct Cyc_Absyn_PtrAtts
_tmp284;_tmp284.rgn=(void*)_tmp27C;_tmp284.nullable=((struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)();_tmp284.bounds=_tmp27D;_tmp284.zero_term=_tmp27E;
_tmp284.ptrloc=0;_tmp284;});struct Cyc_Absyn_PointerType_struct*_tmp280=({struct
Cyc_Absyn_PointerType_struct*_tmp281=_cycalloc(sizeof(*_tmp281));_tmp281[0]=({
struct Cyc_Absyn_PointerType_struct _tmp282;_tmp282.tag=4;_tmp282.f1=({struct Cyc_Absyn_PtrInfo
_tmp283;_tmp283.elt_typ=(void*)_tmp27B;_tmp283.elt_tq=Cyc_Absyn_empty_tqual(0);
_tmp283.ptr_atts=_tmp27F;_tmp283;});_tmp282;});_tmp281;});Cyc_Tcutil_unify(t,(
void*)_tmp280);_tmp275=_tmp27B;_tmp277=_tmp27C;_tmp278=_tmp27D;_tmp279=_tmp27E;
goto _LL18C;}_LL18B: if(*((int*)_tmp273)!= 4)goto _LL18D;_tmp274=((struct Cyc_Absyn_PointerType_struct*)
_tmp273)->f1;_tmp275=(void*)_tmp274.elt_typ;_tmp276=_tmp274.ptr_atts;_tmp277=(
void*)_tmp276.rgn;_tmp278=_tmp276.bounds;_tmp279=_tmp276.zero_term;_LL18C: Cyc_Tcenv_check_rgn_accessible(
_tmp272,loc,_tmp277);Cyc_Tcutil_check_nonzero_bound(loc,_tmp278);if(!Cyc_Tcutil_kind_leq(
Cyc_Tcutil_typ_kind(_tmp275),(void*)1))({void*_tmp287=0;Cyc_Tcutil_terr(loc,({
const char*_tmp288="can't dereference abstract pointer type";_tag_dyneither(
_tmp288,sizeof(char),40);}),_tag_dyneither(_tmp287,sizeof(void*),0));});return
_tmp275;_LL18D:;_LL18E: return({struct Cyc_String_pa_struct _tmp28B;_tmp28B.tag=0;
_tmp28B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));{
void*_tmp289[1]={& _tmp28B};Cyc_Tcexp_expr_err(_tmp272,loc,topt,({const char*
_tmp28A="expecting *, @, or ? type but found %s";_tag_dyneither(_tmp28A,sizeof(
char),39);}),_tag_dyneither(_tmp289,sizeof(void*),1));}});_LL188:;}}static void*
Cyc_Tcexp_tcAggrMember(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,
void**topt,struct Cyc_Absyn_Exp*outer_e,struct Cyc_Absyn_Exp*e,struct
_dyneither_ptr*f){Cyc_Tcexp_tcExpNoPromote(te,0,e);{void*_tmp28C=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);struct Cyc_Absyn_AggrInfo
_tmp28D;union Cyc_Absyn_AggrInfoU_union _tmp28E;struct Cyc_Absyn_Aggrdecl**_tmp28F;
struct Cyc_Absyn_Aggrdecl*_tmp290;struct Cyc_List_List*_tmp291;void*_tmp292;struct
Cyc_List_List*_tmp293;_LL190: if(_tmp28C <= (void*)4)goto _LL198;if(*((int*)_tmp28C)
!= 10)goto _LL192;_tmp28D=((struct Cyc_Absyn_AggrType_struct*)_tmp28C)->f1;_tmp28E=
_tmp28D.aggr_info;if((((((struct Cyc_Absyn_AggrType_struct*)_tmp28C)->f1).aggr_info).KnownAggr).tag
!= 1)goto _LL192;_tmp28F=(_tmp28E.KnownAggr).f1;_tmp290=*_tmp28F;_tmp291=_tmp28D.targs;
_LL191: {struct Cyc_Absyn_Aggrfield*_tmp296=Cyc_Absyn_lookup_decl_field(_tmp290,f);
if(_tmp296 == 0)return({struct Cyc_String_pa_struct _tmp29A;_tmp29A.tag=0;_tmp29A.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*f);{struct Cyc_String_pa_struct
_tmp299;_tmp299.tag=0;_tmp299.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp290->name));{void*_tmp297[2]={& _tmp299,& _tmp29A};Cyc_Tcexp_expr_err(
te,loc,topt,({const char*_tmp298="type %s has no %s field";_tag_dyneither(_tmp298,
sizeof(char),24);}),_tag_dyneither(_tmp297,sizeof(void*),2));}}});if(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp290->impl))->exist_vars != 0)return({void*_tmp29B=0;Cyc_Tcexp_expr_err(
te,loc,topt,({const char*_tmp29C="must use pattern-matching to access fields of existential types";
_tag_dyneither(_tmp29C,sizeof(char),64);}),_tag_dyneither(_tmp29B,sizeof(void*),
0));});{void*t2;struct _RegionHandle*_tmp29D=Cyc_Tcenv_get_fnrgn(te);{struct Cyc_List_List*
_tmp29E=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(_tmp29D,_tmp29D,
_tmp290->tvs,_tmp291);t2=Cyc_Tcutil_rsubstitute(_tmp29D,_tmp29E,(void*)_tmp296->type);
if((((void*)_tmp290->kind == (void*)1  && !((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp290->impl))->tagged) && !Cyc_Tcenv_in_notreadctxt(te)) && !Cyc_Tcutil_bits_only(
t2))return({struct Cyc_String_pa_struct _tmp2A1;_tmp2A1.tag=0;_tmp2A1.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*f);{void*_tmp29F[1]={& _tmp2A1};Cyc_Tcexp_expr_err(
te,loc,topt,({const char*_tmp2A0="cannot read union member %s since it is not `bits-only'";
_tag_dyneither(_tmp2A0,sizeof(char),56);}),_tag_dyneither(_tmp29F,sizeof(void*),
1));}});}return t2;}}_LL192: if(*((int*)_tmp28C)!= 11)goto _LL194;_tmp292=(void*)((
struct Cyc_Absyn_AnonAggrType_struct*)_tmp28C)->f1;_tmp293=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp28C)->f2;_LL193: {struct Cyc_Absyn_Aggrfield*_tmp2A2=Cyc_Absyn_lookup_field(
_tmp293,f);if(_tmp2A2 == 0)return({struct Cyc_String_pa_struct _tmp2A5;_tmp2A5.tag=
0;_tmp2A5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);{void*_tmp2A3[1]={&
_tmp2A5};Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp2A4="type has no %s field";
_tag_dyneither(_tmp2A4,sizeof(char),21);}),_tag_dyneither(_tmp2A3,sizeof(void*),
1));}});if((_tmp292 == (void*)1  && !Cyc_Tcenv_in_notreadctxt(te)) && !Cyc_Tcutil_bits_only((
void*)_tmp2A2->type))return({struct Cyc_String_pa_struct _tmp2A8;_tmp2A8.tag=0;
_tmp2A8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);{void*_tmp2A6[1]={&
_tmp2A8};Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp2A7="cannot read union member %s since it is not `bits-only'";
_tag_dyneither(_tmp2A7,sizeof(char),56);}),_tag_dyneither(_tmp2A6,sizeof(void*),
1));}});return(void*)_tmp2A2->type;}_LL194: if(*((int*)_tmp28C)!= 7)goto _LL196;
if(!(Cyc_strcmp((struct _dyneither_ptr)*f,({const char*_tmp294="size";
_tag_dyneither(_tmp294,sizeof(char),5);}))== 0))goto _LL196;_LL195: goto _LL197;
_LL196: if(*((int*)_tmp28C)!= 4)goto _LL198;if(!(Cyc_strcmp((struct _dyneither_ptr)*
f,({const char*_tmp295="size";_tag_dyneither(_tmp295,sizeof(char),5);}))== 0))
goto _LL198;_LL197:({struct Cyc_String_pa_struct _tmp2AB;_tmp2AB.tag=0;_tmp2AB.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));{void*
_tmp2A9[1]={& _tmp2AB};Cyc_Tcutil_warn(e->loc,({const char*_tmp2AA="deprecated `.size' used here -- change to numelts(%s)";
_tag_dyneither(_tmp2AA,sizeof(char),54);}),_tag_dyneither(_tmp2A9,sizeof(void*),
1));}});(void*)(outer_e->r=(void*)((void*)({struct Cyc_Absyn_Primop_e_struct*
_tmp2AC=_cycalloc(sizeof(*_tmp2AC));_tmp2AC[0]=({struct Cyc_Absyn_Primop_e_struct
_tmp2AD;_tmp2AD.tag=3;_tmp2AD.f1=(void*)((void*)19);_tmp2AD.f2=({struct Cyc_List_List*
_tmp2AE=_cycalloc(sizeof(*_tmp2AE));_tmp2AE->hd=e;_tmp2AE->tl=0;_tmp2AE;});
_tmp2AD;});_tmp2AC;})));return Cyc_Absyn_uint_typ;_LL198:;_LL199: if(Cyc_strcmp((
struct _dyneither_ptr)*f,({const char*_tmp2AF="size";_tag_dyneither(_tmp2AF,
sizeof(char),5);}))== 0)return({struct Cyc_String_pa_struct _tmp2B2;_tmp2B2.tag=0;
_tmp2B2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v));{void*_tmp2B0[1]={&
_tmp2B2};Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp2B1="expecting struct, union, or array, found %s";
_tag_dyneither(_tmp2B1,sizeof(char),44);}),_tag_dyneither(_tmp2B0,sizeof(void*),
1));}});else{return({struct Cyc_String_pa_struct _tmp2B5;_tmp2B5.tag=0;_tmp2B5.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v));{void*_tmp2B3[1]={& _tmp2B5};Cyc_Tcexp_expr_err(
te,loc,topt,({const char*_tmp2B4="expecting struct or union, found %s";
_tag_dyneither(_tmp2B4,sizeof(char),36);}),_tag_dyneither(_tmp2B3,sizeof(void*),
1));}});}_LL18F:;}}static void*Cyc_Tcexp_tcAggrArrow(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e,struct
_dyneither_ptr*f){struct _RegionHandle*_tmp2B6=Cyc_Tcenv_get_fnrgn(te);Cyc_Tcexp_tcExp(
Cyc_Tcenv_clear_notreadctxt(_tmp2B6,te),0,e);{void*_tmp2B7=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp2B8;void*_tmp2B9;struct Cyc_Absyn_PtrAtts _tmp2BA;void*_tmp2BB;struct Cyc_Absyn_Conref*
_tmp2BC;struct Cyc_Absyn_Conref*_tmp2BD;_LL19B: if(_tmp2B7 <= (void*)4)goto _LL19D;
if(*((int*)_tmp2B7)!= 4)goto _LL19D;_tmp2B8=((struct Cyc_Absyn_PointerType_struct*)
_tmp2B7)->f1;_tmp2B9=(void*)_tmp2B8.elt_typ;_tmp2BA=_tmp2B8.ptr_atts;_tmp2BB=(
void*)_tmp2BA.rgn;_tmp2BC=_tmp2BA.bounds;_tmp2BD=_tmp2BA.zero_term;_LL19C: Cyc_Tcutil_check_nonzero_bound(
loc,_tmp2BC);{void*_tmp2BE=Cyc_Tcutil_compress(_tmp2B9);struct Cyc_Absyn_AggrInfo
_tmp2BF;union Cyc_Absyn_AggrInfoU_union _tmp2C0;struct Cyc_Absyn_Aggrdecl**_tmp2C1;
struct Cyc_Absyn_Aggrdecl*_tmp2C2;struct Cyc_List_List*_tmp2C3;void*_tmp2C4;struct
Cyc_List_List*_tmp2C5;_LL1A0: if(_tmp2BE <= (void*)4)goto _LL1A4;if(*((int*)_tmp2BE)
!= 10)goto _LL1A2;_tmp2BF=((struct Cyc_Absyn_AggrType_struct*)_tmp2BE)->f1;_tmp2C0=
_tmp2BF.aggr_info;if((((((struct Cyc_Absyn_AggrType_struct*)_tmp2BE)->f1).aggr_info).KnownAggr).tag
!= 1)goto _LL1A2;_tmp2C1=(_tmp2C0.KnownAggr).f1;_tmp2C2=*_tmp2C1;_tmp2C3=_tmp2BF.targs;
_LL1A1: {struct Cyc_Absyn_Aggrfield*_tmp2C6=Cyc_Absyn_lookup_decl_field(_tmp2C2,f);
if(_tmp2C6 == 0)return({struct Cyc_String_pa_struct _tmp2CA;_tmp2CA.tag=0;_tmp2CA.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*f);{struct Cyc_String_pa_struct
_tmp2C9;_tmp2C9.tag=0;_tmp2C9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp2C2->name));{void*_tmp2C7[2]={& _tmp2C9,& _tmp2CA};Cyc_Tcexp_expr_err(
te,loc,topt,({const char*_tmp2C8="type %s has no %s field";_tag_dyneither(_tmp2C8,
sizeof(char),24);}),_tag_dyneither(_tmp2C7,sizeof(void*),2));}}});if(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp2C2->impl))->exist_vars != 0)return({void*_tmp2CB=0;Cyc_Tcexp_expr_err(
te,loc,topt,({const char*_tmp2CC="must use pattern-matching to access fields of existential types";
_tag_dyneither(_tmp2CC,sizeof(char),64);}),_tag_dyneither(_tmp2CB,sizeof(void*),
0));});{void*t3;struct _RegionHandle*_tmp2CD=Cyc_Tcenv_get_fnrgn(te);{struct Cyc_List_List*
_tmp2CE=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(_tmp2CD,_tmp2CD,
_tmp2C2->tvs,_tmp2C3);t3=Cyc_Tcutil_rsubstitute(_tmp2CD,_tmp2CE,(void*)_tmp2C6->type);}
if(((void*)_tmp2C2->kind == (void*)1  && !Cyc_Tcenv_in_notreadctxt(te)) && !Cyc_Tcutil_bits_only(
t3))return({struct Cyc_String_pa_struct _tmp2D1;_tmp2D1.tag=0;_tmp2D1.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*f);{void*_tmp2CF[1]={& _tmp2D1};Cyc_Tcexp_expr_err(
te,loc,topt,({const char*_tmp2D0="cannot read union member %s since it is not `bits-only'";
_tag_dyneither(_tmp2D0,sizeof(char),56);}),_tag_dyneither(_tmp2CF,sizeof(void*),
1));}});return t3;}}_LL1A2: if(*((int*)_tmp2BE)!= 11)goto _LL1A4;_tmp2C4=(void*)((
struct Cyc_Absyn_AnonAggrType_struct*)_tmp2BE)->f1;_tmp2C5=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp2BE)->f2;_LL1A3: {struct Cyc_Absyn_Aggrfield*_tmp2D2=Cyc_Absyn_lookup_field(
_tmp2C5,f);if(_tmp2D2 == 0)return({struct Cyc_String_pa_struct _tmp2D5;_tmp2D5.tag=
0;_tmp2D5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);{void*_tmp2D3[1]={&
_tmp2D5};Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp2D4="type has no %s field";
_tag_dyneither(_tmp2D4,sizeof(char),21);}),_tag_dyneither(_tmp2D3,sizeof(void*),
1));}});if((_tmp2C4 == (void*)1  && !Cyc_Tcenv_in_notreadctxt(te)) && !Cyc_Tcutil_bits_only((
void*)_tmp2D2->type))return({struct Cyc_String_pa_struct _tmp2D8;_tmp2D8.tag=0;
_tmp2D8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);{void*_tmp2D6[1]={&
_tmp2D8};Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp2D7="cannot read union member %s since it is not `bits-only'";
_tag_dyneither(_tmp2D7,sizeof(char),56);}),_tag_dyneither(_tmp2D6,sizeof(void*),
1));}});return(void*)_tmp2D2->type;}_LL1A4:;_LL1A5: goto _LL19F;_LL19F:;}goto
_LL19A;_LL19D:;_LL19E: goto _LL19A;_LL19A:;}return({struct Cyc_String_pa_struct
_tmp2DB;_tmp2DB.tag=0;_tmp2DB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v));{
void*_tmp2D9[1]={& _tmp2DB};Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp2DA="expecting struct or union pointer, found %s";
_tag_dyneither(_tmp2DA,sizeof(char),44);}),_tag_dyneither(_tmp2D9,sizeof(void*),
1));}});}static void*Cyc_Tcexp_ithTupleType(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,struct Cyc_List_List*ts,struct Cyc_Absyn_Exp*index){unsigned int _tmp2DD;int
_tmp2DE;struct _tuple7 _tmp2DC=Cyc_Evexp_eval_const_uint_exp(index);_tmp2DD=
_tmp2DC.f1;_tmp2DE=_tmp2DC.f2;if(!_tmp2DE)return({void*_tmp2DF=0;Cyc_Tcexp_expr_err(
te,loc,0,({const char*_tmp2E0="tuple projection cannot use sizeof or offsetof";
_tag_dyneither(_tmp2E0,sizeof(char),47);}),_tag_dyneither(_tmp2DF,sizeof(void*),
0));});{struct _handler_cons _tmp2E1;_push_handler(& _tmp2E1);{int _tmp2E3=0;if(
setjmp(_tmp2E1.handler))_tmp2E3=1;if(!_tmp2E3){{void*_tmp2E4=(*((struct _tuple9*(*)(
struct Cyc_List_List*x,int n))Cyc_List_nth)(ts,(int)_tmp2DD)).f2;_npop_handler(0);
return _tmp2E4;};_pop_handler();}else{void*_tmp2E2=(void*)_exn_thrown;void*
_tmp2E6=_tmp2E2;_LL1A7: if(_tmp2E6 != Cyc_List_Nth)goto _LL1A9;_LL1A8: return({
struct Cyc_Int_pa_struct _tmp2EA;_tmp2EA.tag=1;_tmp2EA.f1=(unsigned long)((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(ts);{struct Cyc_Int_pa_struct _tmp2E9;
_tmp2E9.tag=1;_tmp2E9.f1=(unsigned long)((int)_tmp2DD);{void*_tmp2E7[2]={&
_tmp2E9,& _tmp2EA};Cyc_Tcexp_expr_err(te,loc,0,({const char*_tmp2E8="index is %d but tuple has only %d fields";
_tag_dyneither(_tmp2E8,sizeof(char),41);}),_tag_dyneither(_tmp2E7,sizeof(void*),
2));}}});_LL1A9:;_LL1AA:(void)_throw(_tmp2E6);_LL1A6:;}}}}static void*Cyc_Tcexp_tcSubscript(
struct Cyc_Tcenv_Tenv*te_orig,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
e1,struct Cyc_Absyn_Exp*e2){struct _RegionHandle*_tmp2EB=Cyc_Tcenv_get_fnrgn(
te_orig);struct Cyc_Tcenv_Tenv*_tmp2EC=Cyc_Tcenv_clear_notreadctxt(_tmp2EB,
te_orig);Cyc_Tcexp_tcExp(_tmp2EC,0,e1);Cyc_Tcexp_tcExp(_tmp2EC,0,e2);{void*t1=
Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v);void*
t2=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v);
if(!Cyc_Tcutil_coerce_sint_typ(_tmp2EC,e2))return({struct Cyc_String_pa_struct
_tmp2EF;_tmp2EF.tag=0;_tmp2EF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2));{void*_tmp2ED[1]={& _tmp2EF};Cyc_Tcexp_expr_err(
_tmp2EC,e2->loc,topt,({const char*_tmp2EE="expecting int subscript, found %s";
_tag_dyneither(_tmp2EE,sizeof(char),34);}),_tag_dyneither(_tmp2ED,sizeof(void*),
1));}});{void*_tmp2F0=t1;struct Cyc_Absyn_PtrInfo _tmp2F1;void*_tmp2F2;struct Cyc_Absyn_Tqual
_tmp2F3;struct Cyc_Absyn_PtrAtts _tmp2F4;void*_tmp2F5;struct Cyc_Absyn_Conref*
_tmp2F6;struct Cyc_Absyn_Conref*_tmp2F7;struct Cyc_List_List*_tmp2F8;_LL1AC: if(
_tmp2F0 <= (void*)4)goto _LL1B0;if(*((int*)_tmp2F0)!= 4)goto _LL1AE;_tmp2F1=((
struct Cyc_Absyn_PointerType_struct*)_tmp2F0)->f1;_tmp2F2=(void*)_tmp2F1.elt_typ;
_tmp2F3=_tmp2F1.elt_tq;_tmp2F4=_tmp2F1.ptr_atts;_tmp2F5=(void*)_tmp2F4.rgn;
_tmp2F6=_tmp2F4.bounds;_tmp2F7=_tmp2F4.zero_term;_LL1AD: if(((int(*)(int,struct
Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp2F7)){int emit_warning=1;{union
Cyc_Absyn_Constraint_union _tmp2F9=(Cyc_Absyn_compress_conref(_tmp2F6))->v;void*
_tmp2FA;struct Cyc_Absyn_Exp*_tmp2FB;void*_tmp2FC;_LL1B3: if((_tmp2F9.Eq_constr).tag
!= 0)goto _LL1B5;_tmp2FA=(_tmp2F9.Eq_constr).f1;if(_tmp2FA <= (void*)1)goto _LL1B5;
if(*((int*)_tmp2FA)!= 0)goto _LL1B5;_tmp2FB=((struct Cyc_Absyn_Upper_b_struct*)
_tmp2FA)->f1;_LL1B4: if(Cyc_Tcutil_is_const_exp(_tmp2EC,e2)){unsigned int _tmp2FE;
int _tmp2FF;struct _tuple7 _tmp2FD=Cyc_Evexp_eval_const_uint_exp(e2);_tmp2FE=
_tmp2FD.f1;_tmp2FF=_tmp2FD.f2;if(_tmp2FF){unsigned int _tmp301;int _tmp302;struct
_tuple7 _tmp300=Cyc_Evexp_eval_const_uint_exp(_tmp2FB);_tmp301=_tmp300.f1;_tmp302=
_tmp300.f2;if(_tmp302  && _tmp301 > _tmp2FE)emit_warning=0;}}goto _LL1B2;_LL1B5: if((
_tmp2F9.Eq_constr).tag != 0)goto _LL1B7;_tmp2FC=(_tmp2F9.Eq_constr).f1;if((int)
_tmp2FC != 0)goto _LL1B7;_LL1B6: emit_warning=0;goto _LL1B2;_LL1B7:;_LL1B8: goto
_LL1B2;_LL1B2:;}if(emit_warning)({void*_tmp303=0;Cyc_Tcutil_warn(e2->loc,({const
char*_tmp304="subscript on thin, zero-terminated pointer could be expensive.";
_tag_dyneither(_tmp304,sizeof(char),63);}),_tag_dyneither(_tmp303,sizeof(void*),
0));});}else{if(Cyc_Tcutil_is_const_exp(_tmp2EC,e2)){unsigned int _tmp306;int
_tmp307;struct _tuple7 _tmp305=Cyc_Evexp_eval_const_uint_exp(e2);_tmp306=_tmp305.f1;
_tmp307=_tmp305.f2;if(_tmp307)Cyc_Tcutil_check_bound(loc,_tmp306,_tmp2F6);}else{
if(Cyc_Tcutil_is_bound_one(_tmp2F6) && !((int(*)(int,struct Cyc_Absyn_Conref*x))
Cyc_Absyn_conref_def)(0,_tmp2F7))({void*_tmp308=0;Cyc_Tcutil_warn(e1->loc,({
const char*_tmp309="subscript applied to pointer to one object";_tag_dyneither(
_tmp309,sizeof(char),43);}),_tag_dyneither(_tmp308,sizeof(void*),0));});Cyc_Tcutil_check_nonzero_bound(
loc,_tmp2F6);}}Cyc_Tcenv_check_rgn_accessible(_tmp2EC,loc,_tmp2F5);if(!Cyc_Tcutil_kind_leq(
Cyc_Tcutil_typ_kind(_tmp2F2),(void*)1))({void*_tmp30A=0;Cyc_Tcutil_terr(e1->loc,({
const char*_tmp30B="can't subscript an abstract pointer";_tag_dyneither(_tmp30B,
sizeof(char),36);}),_tag_dyneither(_tmp30A,sizeof(void*),0));});return _tmp2F2;
_LL1AE: if(*((int*)_tmp2F0)!= 9)goto _LL1B0;_tmp2F8=((struct Cyc_Absyn_TupleType_struct*)
_tmp2F0)->f1;_LL1AF: return Cyc_Tcexp_ithTupleType(_tmp2EC,loc,_tmp2F8,e2);_LL1B0:;
_LL1B1: return({struct Cyc_String_pa_struct _tmp30E;_tmp30E.tag=0;_tmp30E.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));{void*
_tmp30C[1]={& _tmp30E};Cyc_Tcexp_expr_err(_tmp2EC,loc,topt,({const char*_tmp30D="subscript applied to %s";
_tag_dyneither(_tmp30D,sizeof(char),24);}),_tag_dyneither(_tmp30C,sizeof(void*),
1));}});_LL1AB:;}}}static void*Cyc_Tcexp_tcTuple(struct Cyc_Tcenv_Tenv*te,struct
Cyc_Position_Segment*loc,void**topt,struct Cyc_List_List*es){int done=0;struct Cyc_List_List*
fields=0;if(topt != 0){void*_tmp30F=Cyc_Tcutil_compress(*topt);struct Cyc_List_List*
_tmp310;_LL1BA: if(_tmp30F <= (void*)4)goto _LL1BC;if(*((int*)_tmp30F)!= 9)goto
_LL1BC;_tmp310=((struct Cyc_Absyn_TupleType_struct*)_tmp30F)->f1;_LL1BB: if(((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmp310)!= ((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(es))goto _LL1B9;for(0;es != 0;(es=es->tl,_tmp310=_tmp310->tl)){
void*_tmp311=(*((struct _tuple9*)((struct Cyc_List_List*)_check_null(_tmp310))->hd)).f2;
Cyc_Tcexp_tcExpInitializer(te,(void**)& _tmp311,(struct Cyc_Absyn_Exp*)es->hd);Cyc_Tcutil_coerce_arg(
te,(struct Cyc_Absyn_Exp*)es->hd,(*((struct _tuple9*)_tmp310->hd)).f2);fields=({
struct Cyc_List_List*_tmp312=_cycalloc(sizeof(*_tmp312));_tmp312->hd=({struct
_tuple9*_tmp313=_cycalloc(sizeof(*_tmp313));_tmp313->f1=(*((struct _tuple9*)
_tmp310->hd)).f1;_tmp313->f2=(void*)((struct Cyc_Core_Opt*)_check_null(((struct
Cyc_Absyn_Exp*)es->hd)->topt))->v;_tmp313;});_tmp312->tl=fields;_tmp312;});}done=
1;goto _LL1B9;_LL1BC:;_LL1BD: goto _LL1B9;_LL1B9:;}if(!done)for(0;es != 0;es=es->tl){
Cyc_Tcexp_tcExpInitializer(te,0,(struct Cyc_Absyn_Exp*)es->hd);fields=({struct Cyc_List_List*
_tmp314=_cycalloc(sizeof(*_tmp314));_tmp314->hd=({struct _tuple9*_tmp315=
_cycalloc(sizeof(*_tmp315));_tmp315->f1=Cyc_Absyn_empty_tqual(0);_tmp315->f2=(
void*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Exp*)es->hd)->topt))->v;
_tmp315;});_tmp314->tl=fields;_tmp314;});}return(void*)({struct Cyc_Absyn_TupleType_struct*
_tmp316=_cycalloc(sizeof(*_tmp316));_tmp316[0]=({struct Cyc_Absyn_TupleType_struct
_tmp317;_tmp317.tag=9;_tmp317.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(fields);_tmp317;});_tmp316;});}static void*Cyc_Tcexp_tcCompoundLit(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct _tuple2*
t,struct Cyc_List_List*des){return({void*_tmp318=0;Cyc_Tcexp_expr_err(te,loc,topt,({
const char*_tmp319="tcCompoundLit";_tag_dyneither(_tmp319,sizeof(char),14);}),
_tag_dyneither(_tmp318,sizeof(void*),0));});}static void*Cyc_Tcexp_tcArray(struct
Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**elt_topt,int zero_term,
struct Cyc_List_List*des){void*res_t2;struct _RegionHandle*_tmp31A=Cyc_Tcenv_get_fnrgn(
te);{int _tmp31B=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(des);struct Cyc_List_List*
es=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Exp*(*f)(
struct _tuple8*),struct Cyc_List_List*x))Cyc_List_rmap)(_tmp31A,(struct Cyc_Absyn_Exp*(*)(
struct _tuple8*))Cyc_Core_snd,des);void*res=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_mk,Cyc_Tcenv_lookup_opt_type_vars(te));struct Cyc_Absyn_Const_e_struct*
_tmp31C=({struct Cyc_Absyn_Const_e_struct*_tmp324=_cycalloc(sizeof(*_tmp324));
_tmp324[0]=({struct Cyc_Absyn_Const_e_struct _tmp325;_tmp325.tag=0;_tmp325.f1=(
union Cyc_Absyn_Cnst_union)({union Cyc_Absyn_Cnst_union _tmp326;(_tmp326.Int_c).tag=
2;(_tmp326.Int_c).f1=(void*)((void*)1);(_tmp326.Int_c).f2=_tmp31B;_tmp326;});
_tmp325;});_tmp324;});struct Cyc_Absyn_Exp*sz_exp=Cyc_Absyn_new_exp((void*)
_tmp31C,loc);if(zero_term){struct Cyc_Absyn_Exp*_tmp31D=((struct Cyc_Absyn_Exp*(*)(
struct Cyc_List_List*x,int n))Cyc_List_nth)(es,_tmp31B - 1);if(!Cyc_Tcutil_is_zero(
_tmp31D))({void*_tmp31E=0;Cyc_Tcutil_terr(_tmp31D->loc,({const char*_tmp31F="zero-terminated array doesn't end with zero.";
_tag_dyneither(_tmp31F,sizeof(char),45);}),_tag_dyneither(_tmp31E,sizeof(void*),
0));});}sz_exp->topt=({struct Cyc_Core_Opt*_tmp320=_cycalloc(sizeof(*_tmp320));
_tmp320->v=(void*)Cyc_Absyn_uint_typ;_tmp320;});res_t2=Cyc_Absyn_array_typ(res,
Cyc_Absyn_empty_tqual(0),(struct Cyc_Absyn_Exp*)sz_exp,zero_term?Cyc_Absyn_true_conref:
Cyc_Absyn_false_conref,0);{struct Cyc_List_List*es2=es;for(0;es2 != 0;es2=es2->tl){
Cyc_Tcexp_tcExpInitializer(te,elt_topt,(struct Cyc_Absyn_Exp*)es2->hd);}}if(!Cyc_Tcutil_coerce_list(
te,res,es))({struct Cyc_String_pa_struct _tmp323;_tmp323.tag=0;_tmp323.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(res));{void*_tmp321[
1]={& _tmp323};Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(es))->hd)->loc,({const char*_tmp322="elements of array do not all have the same type (%s)";
_tag_dyneither(_tmp322,sizeof(char),53);}),_tag_dyneither(_tmp321,sizeof(void*),
1));}});}{int offset=0;for(0;des != 0;(offset ++,des=des->tl)){struct Cyc_List_List*
ds=(*((struct _tuple8*)des->hd)).f1;if(ds != 0){void*_tmp327=(void*)ds->hd;struct
Cyc_Absyn_Exp*_tmp328;_LL1BF: if(*((int*)_tmp327)!= 1)goto _LL1C1;_LL1C0:({void*
_tmp329=0;Cyc_Tcutil_terr(loc,({const char*_tmp32A="only array index designators are supported";
_tag_dyneither(_tmp32A,sizeof(char),43);}),_tag_dyneither(_tmp329,sizeof(void*),
0));});goto _LL1BE;_LL1C1: if(*((int*)_tmp327)!= 0)goto _LL1BE;_tmp328=((struct Cyc_Absyn_ArrayElement_struct*)
_tmp327)->f1;_LL1C2: Cyc_Tcexp_tcExpInitializer(te,0,_tmp328);{unsigned int
_tmp32C;int _tmp32D;struct _tuple7 _tmp32B=Cyc_Evexp_eval_const_uint_exp(_tmp328);
_tmp32C=_tmp32B.f1;_tmp32D=_tmp32B.f2;if(!_tmp32D)({void*_tmp32E=0;Cyc_Tcutil_terr(
_tmp328->loc,({const char*_tmp32F="index designator cannot use sizeof or offsetof";
_tag_dyneither(_tmp32F,sizeof(char),47);}),_tag_dyneither(_tmp32E,sizeof(void*),
0));});else{if(_tmp32C != offset)({struct Cyc_Int_pa_struct _tmp333;_tmp333.tag=1;
_tmp333.f1=(unsigned long)((int)_tmp32C);{struct Cyc_Int_pa_struct _tmp332;_tmp332.tag=
1;_tmp332.f1=(unsigned long)offset;{void*_tmp330[2]={& _tmp332,& _tmp333};Cyc_Tcutil_terr(
_tmp328->loc,({const char*_tmp331="expecting index designator %d but found %d";
_tag_dyneither(_tmp331,sizeof(char),43);}),_tag_dyneither(_tmp330,sizeof(void*),
2));}}});}goto _LL1BE;}_LL1BE:;}}}return res_t2;}static void*Cyc_Tcexp_tcComprehension(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Vardecl*
vd,struct Cyc_Absyn_Exp*bound,struct Cyc_Absyn_Exp*body,int*is_zero_term){Cyc_Tcexp_tcExp(
te,0,bound);{void*_tmp334=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(bound->topt))->v);_LL1C4: if(_tmp334 <= (void*)4)goto _LL1C6;if(*((int*)
_tmp334)!= 18)goto _LL1C6;_LL1C5: goto _LL1C3;_LL1C6:;_LL1C7: if(!Cyc_Tcutil_coerce_uint_typ(
te,bound))({struct Cyc_String_pa_struct _tmp337;_tmp337.tag=0;_tmp337.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(bound->topt))->v));{void*_tmp335[1]={& _tmp337};Cyc_Tcutil_terr(bound->loc,({
const char*_tmp336="expecting unsigned int, found %s";_tag_dyneither(_tmp336,
sizeof(char),33);}),_tag_dyneither(_tmp335,sizeof(void*),1));}});_LL1C3:;}if(!(
vd->tq).real_const)({void*_tmp338=0;((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp339="comprehension index variable is not declared const!";
_tag_dyneither(_tmp339,sizeof(char),52);}),_tag_dyneither(_tmp338,sizeof(void*),
0));});{struct _RegionHandle*_tmp33A=Cyc_Tcenv_get_fnrgn(te);struct Cyc_Tcenv_Tenv*
te2=Cyc_Tcenv_copy_tenv(_tmp33A,te);if(te2->le != 0){te2=Cyc_Tcenv_new_block(
_tmp33A,loc,te2);te2=Cyc_Tcenv_add_local_var(_tmp33A,loc,te2,vd);}{struct Cyc_Tcenv_Tenv*
_tmp33B=te2;struct Cyc_Absyn_PtrInfo pinfo;void**_tmp33C=0;struct Cyc_Absyn_Tqual*
_tmp33D=0;struct Cyc_Absyn_Conref**_tmp33E=0;if(topt != 0){void*_tmp33F=Cyc_Tcutil_compress(*
topt);struct Cyc_Absyn_PtrInfo _tmp340;struct Cyc_Absyn_ArrayInfo _tmp341;void*
_tmp342;void**_tmp343;struct Cyc_Absyn_Tqual _tmp344;struct Cyc_Absyn_Tqual*_tmp345;
struct Cyc_Absyn_Exp*_tmp346;struct Cyc_Absyn_Conref*_tmp347;struct Cyc_Absyn_Conref**
_tmp348;_LL1C9: if(_tmp33F <= (void*)4)goto _LL1CD;if(*((int*)_tmp33F)!= 4)goto
_LL1CB;_tmp340=((struct Cyc_Absyn_PointerType_struct*)_tmp33F)->f1;_LL1CA: pinfo=
_tmp340;_tmp33C=(void**)((void**)((void*)& pinfo.elt_typ));_tmp33D=(struct Cyc_Absyn_Tqual*)&
pinfo.elt_tq;_tmp33E=(struct Cyc_Absyn_Conref**)&(pinfo.ptr_atts).zero_term;goto
_LL1C8;_LL1CB: if(*((int*)_tmp33F)!= 7)goto _LL1CD;_tmp341=((struct Cyc_Absyn_ArrayType_struct*)
_tmp33F)->f1;_tmp342=(void*)_tmp341.elt_type;_tmp343=(void**)&(((struct Cyc_Absyn_ArrayType_struct*)
_tmp33F)->f1).elt_type;_tmp344=_tmp341.tq;_tmp345=(struct Cyc_Absyn_Tqual*)&(((
struct Cyc_Absyn_ArrayType_struct*)_tmp33F)->f1).tq;_tmp346=_tmp341.num_elts;
_tmp347=_tmp341.zero_term;_tmp348=(struct Cyc_Absyn_Conref**)&(((struct Cyc_Absyn_ArrayType_struct*)
_tmp33F)->f1).zero_term;_LL1CC: _tmp33C=(void**)_tmp343;_tmp33D=(struct Cyc_Absyn_Tqual*)
_tmp345;_tmp33E=(struct Cyc_Absyn_Conref**)_tmp348;goto _LL1C8;_LL1CD:;_LL1CE: goto
_LL1C8;_LL1C8:;}{void*t=Cyc_Tcexp_tcExp(_tmp33B,_tmp33C,body);if(_tmp33B->le == 0){
if(!Cyc_Tcutil_is_const_exp(_tmp33B,bound))({void*_tmp349=0;Cyc_Tcutil_terr(
bound->loc,({const char*_tmp34A="bound is not constant";_tag_dyneither(_tmp34A,
sizeof(char),22);}),_tag_dyneither(_tmp349,sizeof(void*),0));});if(!Cyc_Tcutil_is_const_exp(
_tmp33B,body))({void*_tmp34B=0;Cyc_Tcutil_terr(bound->loc,({const char*_tmp34C="body is not constant";
_tag_dyneither(_tmp34C,sizeof(char),21);}),_tag_dyneither(_tmp34B,sizeof(void*),
0));});}if(_tmp33E != 0  && ((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,*_tmp33E)){struct Cyc_Absyn_Exp*_tmp34D=Cyc_Absyn_uint_exp(1,0);_tmp34D->topt=({
struct Cyc_Core_Opt*_tmp34E=_cycalloc(sizeof(*_tmp34E));_tmp34E->v=(void*)Cyc_Absyn_uint_typ;
_tmp34E;});bound=Cyc_Absyn_add_exp(bound,_tmp34D,0);bound->topt=({struct Cyc_Core_Opt*
_tmp34F=_cycalloc(sizeof(*_tmp34F));_tmp34F->v=(void*)Cyc_Absyn_uint_typ;_tmp34F;});*
is_zero_term=1;}{void*_tmp350=Cyc_Absyn_array_typ(t,_tmp33D == 0?Cyc_Absyn_empty_tqual(
0):*_tmp33D,(struct Cyc_Absyn_Exp*)bound,_tmp33E == 0?Cyc_Absyn_false_conref:*
_tmp33E,0);return _tmp350;}}}}}struct _tuple10{struct Cyc_Absyn_Aggrfield*f1;struct
Cyc_Absyn_Exp*f2;};static void*Cyc_Tcexp_tcStruct(struct Cyc_Tcenv_Tenv*te,struct
Cyc_Position_Segment*loc,void**topt,struct _tuple1**tn,struct Cyc_List_List**ts,
struct Cyc_List_List*args,struct Cyc_Absyn_Aggrdecl**ad_opt){struct Cyc_Absyn_Aggrdecl*
ad;if(*ad_opt != 0)ad=(struct Cyc_Absyn_Aggrdecl*)_check_null(*ad_opt);else{{
struct _handler_cons _tmp351;_push_handler(& _tmp351);{int _tmp353=0;if(setjmp(
_tmp351.handler))_tmp353=1;if(!_tmp353){ad=*Cyc_Tcenv_lookup_aggrdecl(te,loc,*tn);;
_pop_handler();}else{void*_tmp352=(void*)_exn_thrown;void*_tmp355=_tmp352;_LL1D0:
if(_tmp355 != Cyc_Dict_Absent)goto _LL1D2;_LL1D1:({struct Cyc_String_pa_struct
_tmp358;_tmp358.tag=0;_tmp358.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(*tn));{void*_tmp356[1]={& _tmp358};Cyc_Tcutil_terr(loc,({
const char*_tmp357="unbound struct name %s";_tag_dyneither(_tmp357,sizeof(char),
23);}),_tag_dyneither(_tmp356,sizeof(void*),1));}});return topt != 0?*topt:(void*)
0;_LL1D2:;_LL1D3:(void)_throw(_tmp355);_LL1CF:;}}}*ad_opt=(struct Cyc_Absyn_Aggrdecl*)
ad;*tn=ad->name;}if((void*)ad->kind == (void*)1)({void*_tmp359=0;Cyc_Tcutil_terr(
loc,({const char*_tmp35A="expecting struct but found union";_tag_dyneither(
_tmp35A,sizeof(char),33);}),_tag_dyneither(_tmp359,sizeof(void*),0));});if(ad->impl
== 0){({void*_tmp35B=0;Cyc_Tcutil_terr(loc,({const char*_tmp35C="can't build abstract struct";
_tag_dyneither(_tmp35C,sizeof(char),28);}),_tag_dyneither(_tmp35B,sizeof(void*),
0));});return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}{struct
_RegionHandle*_tmp35D=Cyc_Tcenv_get_fnrgn(te);struct _tuple4 _tmp35E=({struct
_tuple4 _tmp37D;_tmp37D.f1=Cyc_Tcenv_lookup_type_vars(te);_tmp37D.f2=_tmp35D;
_tmp37D;});struct Cyc_List_List*_tmp35F=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _tuple5*(*f)(struct _tuple4*,struct Cyc_Absyn_Tvar*),struct
_tuple4*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(_tmp35D,Cyc_Tcutil_r_make_inst_var,&
_tmp35E,ad->tvs);struct Cyc_List_List*_tmp360=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _tuple5*(*f)(struct _tuple4*,struct Cyc_Absyn_Tvar*),struct
_tuple4*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(_tmp35D,Cyc_Tcutil_r_make_inst_var,&
_tmp35E,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars);
struct Cyc_List_List*_tmp361=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple5*),
struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple5*))Cyc_Core_snd,
_tmp35F);struct Cyc_List_List*_tmp362=((struct Cyc_List_List*(*)(void*(*f)(struct
_tuple5*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple5*))Cyc_Core_snd,
_tmp360);struct Cyc_List_List*_tmp363=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(
_tmp35D,_tmp35F,_tmp360);struct Cyc_Absyn_AggrType_struct*_tmp364=({struct Cyc_Absyn_AggrType_struct*
_tmp378=_cycalloc(sizeof(*_tmp378));_tmp378[0]=({struct Cyc_Absyn_AggrType_struct
_tmp379;_tmp379.tag=10;_tmp379.f1=({struct Cyc_Absyn_AggrInfo _tmp37A;_tmp37A.aggr_info=(
union Cyc_Absyn_AggrInfoU_union)({union Cyc_Absyn_AggrInfoU_union _tmp37B;(_tmp37B.KnownAggr).tag=
1;(_tmp37B.KnownAggr).f1=({struct Cyc_Absyn_Aggrdecl**_tmp37C=_cycalloc(sizeof(*
_tmp37C));_tmp37C[0]=ad;_tmp37C;});_tmp37B;});_tmp37A.targs=_tmp361;_tmp37A;});
_tmp379;});_tmp378;});struct Cyc_List_List*_tmp365=*ts;struct Cyc_List_List*
_tmp366=_tmp362;while(_tmp365 != 0  && _tmp366 != 0){Cyc_Tcutil_check_type(loc,te,
Cyc_Tcenv_lookup_type_vars(te),(void*)0,1,(void*)_tmp365->hd);Cyc_Tcutil_unify((
void*)_tmp365->hd,(void*)_tmp366->hd);_tmp365=_tmp365->tl;_tmp366=_tmp366->tl;}
if(_tmp365 != 0)({void*_tmp367=0;Cyc_Tcutil_terr(loc,({const char*_tmp368="too many explicit witness types";
_tag_dyneither(_tmp368,sizeof(char),32);}),_tag_dyneither(_tmp367,sizeof(void*),
0));});*ts=_tmp362;if(topt != 0)Cyc_Tcutil_unify((void*)_tmp364,*topt);{struct Cyc_List_List*
fields=((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,struct Cyc_Position_Segment*
loc,struct Cyc_List_List*des,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_struct_designators)(
_tmp35D,loc,args,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields);
for(0;fields != 0;fields=fields->tl){struct _tuple10 _tmp36A;struct Cyc_Absyn_Aggrfield*
_tmp36B;struct Cyc_Absyn_Exp*_tmp36C;struct _tuple10*_tmp369=(struct _tuple10*)
fields->hd;_tmp36A=*_tmp369;_tmp36B=_tmp36A.f1;_tmp36C=_tmp36A.f2;{void*_tmp36D=
Cyc_Tcutil_rsubstitute(_tmp35D,_tmp363,(void*)_tmp36B->type);Cyc_Tcexp_tcExpInitializer(
te,(void**)& _tmp36D,_tmp36C);if(!Cyc_Tcutil_coerce_arg(te,_tmp36C,_tmp36D)){({
struct Cyc_String_pa_struct _tmp373;_tmp373.tag=0;_tmp373.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp36C->topt))->v));{struct Cyc_String_pa_struct _tmp372;_tmp372.tag=
0;_tmp372.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
_tmp36D));{struct Cyc_String_pa_struct _tmp371;_tmp371.tag=0;_tmp371.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(*tn));{struct Cyc_String_pa_struct
_tmp370;_tmp370.tag=0;_tmp370.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*
_tmp36B->name);{void*_tmp36E[4]={& _tmp370,& _tmp371,& _tmp372,& _tmp373};Cyc_Tcutil_terr(
_tmp36C->loc,({const char*_tmp36F="field %s of struct %s expects type %s != %s";
_tag_dyneither(_tmp36F,sizeof(char),44);}),_tag_dyneither(_tmp36E,sizeof(void*),
4));}}}}});Cyc_Tcutil_explain_failure();}}}{struct Cyc_List_List*_tmp374=0;{
struct Cyc_List_List*_tmp375=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po;
for(0;_tmp375 != 0;_tmp375=_tmp375->tl){_tmp374=({struct Cyc_List_List*_tmp376=
_cycalloc(sizeof(*_tmp376));_tmp376->hd=({struct _tuple0*_tmp377=_cycalloc(
sizeof(*_tmp377));_tmp377->f1=Cyc_Tcutil_rsubstitute(_tmp35D,_tmp363,(*((struct
_tuple0*)_tmp375->hd)).f1);_tmp377->f2=Cyc_Tcutil_rsubstitute(_tmp35D,_tmp363,(*((
struct _tuple0*)_tmp375->hd)).f2);_tmp377;});_tmp376->tl=_tmp374;_tmp376;});}}
_tmp374=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmp374);Cyc_Tcenv_check_rgn_partial_order(te,loc,_tmp374);return(void*)_tmp364;}}}}
static void*Cyc_Tcexp_tcAnonStruct(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void*ts,struct Cyc_List_List*args){struct _RegionHandle*_tmp37E=Cyc_Tcenv_get_fnrgn(
te);{void*_tmp37F=Cyc_Tcutil_compress(ts);void*_tmp380;struct Cyc_List_List*
_tmp381;_LL1D5: if(_tmp37F <= (void*)4)goto _LL1D7;if(*((int*)_tmp37F)!= 11)goto
_LL1D7;_tmp380=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)_tmp37F)->f1;
_tmp381=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp37F)->f2;_LL1D6: if(_tmp380 == (
void*)1)({void*_tmp382=0;Cyc_Tcutil_terr(loc,({const char*_tmp383="expecting struct but found union";
_tag_dyneither(_tmp383,sizeof(char),33);}),_tag_dyneither(_tmp382,sizeof(void*),
0));});{struct Cyc_List_List*fields=((struct Cyc_List_List*(*)(struct _RegionHandle*
rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,struct Cyc_List_List*
fields))Cyc_Tcutil_resolve_struct_designators)(_tmp37E,loc,args,_tmp381);for(0;
fields != 0;fields=fields->tl){struct _tuple10 _tmp385;struct Cyc_Absyn_Aggrfield*
_tmp386;struct Cyc_Absyn_Exp*_tmp387;struct _tuple10*_tmp384=(struct _tuple10*)
fields->hd;_tmp385=*_tmp384;_tmp386=_tmp385.f1;_tmp387=_tmp385.f2;Cyc_Tcexp_tcExpInitializer(
te,(void**)((void**)((void*)& _tmp386->type)),_tmp387);if(!Cyc_Tcutil_coerce_arg(
te,_tmp387,(void*)_tmp386->type)){({struct Cyc_String_pa_struct _tmp38C;_tmp38C.tag=
0;_tmp38C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp387->topt))->v));{struct Cyc_String_pa_struct
_tmp38B;_tmp38B.tag=0;_tmp38B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_tmp386->type));{struct Cyc_String_pa_struct _tmp38A;
_tmp38A.tag=0;_tmp38A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp386->name);{
void*_tmp388[3]={& _tmp38A,& _tmp38B,& _tmp38C};Cyc_Tcutil_terr(_tmp387->loc,({
const char*_tmp389="field %s of struct expects type %s != %s";_tag_dyneither(
_tmp389,sizeof(char),41);}),_tag_dyneither(_tmp388,sizeof(void*),3));}}}});Cyc_Tcutil_explain_failure();}}
goto _LL1D4;}_LL1D7:;_LL1D8:({void*_tmp38D=0;((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp38E="tcAnonStruct: wrong type";
_tag_dyneither(_tmp38E,sizeof(char),25);}),_tag_dyneither(_tmp38D,sizeof(void*),
0));});_LL1D4:;}return ts;}static void*Cyc_Tcexp_tcDatatype(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*
es,struct Cyc_Absyn_Datatypedecl*tud,struct Cyc_Absyn_Datatypefield*tuf){struct
_RegionHandle*_tmp38F=Cyc_Tcenv_get_fnrgn(te);struct _tuple4 _tmp390=({struct
_tuple4 _tmp3AD;_tmp3AD.f1=Cyc_Tcenv_lookup_type_vars(te);_tmp3AD.f2=_tmp38F;
_tmp3AD;});struct Cyc_List_List*_tmp391=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _tuple5*(*f)(struct _tuple4*,struct Cyc_Absyn_Tvar*),struct
_tuple4*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(_tmp38F,Cyc_Tcutil_r_make_inst_var,&
_tmp390,tud->tvs);struct Cyc_List_List*_tmp392=((struct Cyc_List_List*(*)(void*(*f)(
struct _tuple5*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple5*))
Cyc_Core_snd,_tmp391);void*res=(void*)({struct Cyc_Absyn_DatatypeFieldType_struct*
_tmp3A9=_cycalloc(sizeof(*_tmp3A9));_tmp3A9[0]=({struct Cyc_Absyn_DatatypeFieldType_struct
_tmp3AA;_tmp3AA.tag=3;_tmp3AA.f1=({struct Cyc_Absyn_DatatypeFieldInfo _tmp3AB;
_tmp3AB.field_info=(union Cyc_Absyn_DatatypeFieldInfoU_union)({union Cyc_Absyn_DatatypeFieldInfoU_union
_tmp3AC;(_tmp3AC.KnownDatatypefield).tag=1;(_tmp3AC.KnownDatatypefield).f1=tud;(
_tmp3AC.KnownDatatypefield).f2=tuf;_tmp3AC;});_tmp3AB.targs=_tmp392;_tmp3AB;});
_tmp3AA;});_tmp3A9;});if(topt != 0){void*_tmp393=Cyc_Tcutil_compress(*topt);
struct Cyc_Absyn_DatatypeInfo _tmp394;struct Cyc_List_List*_tmp395;struct Cyc_Core_Opt*
_tmp396;_LL1DA: if(_tmp393 <= (void*)4)goto _LL1DE;if(*((int*)_tmp393)!= 3)goto
_LL1DC;_LL1DB: Cyc_Tcutil_unify(*topt,res);goto _LL1D9;_LL1DC: if(*((int*)_tmp393)
!= 2)goto _LL1DE;_tmp394=((struct Cyc_Absyn_DatatypeType_struct*)_tmp393)->f1;
_tmp395=_tmp394.targs;_tmp396=_tmp394.rgn;_LL1DD:{struct Cyc_List_List*a=_tmp392;
for(0;a != 0  && _tmp395 != 0;(a=a->tl,_tmp395=_tmp395->tl)){Cyc_Tcutil_unify((void*)
a->hd,(void*)_tmp395->hd);}}if(tud->is_flat  || tuf->typs == 0  && es == 0){e->topt=({
struct Cyc_Core_Opt*_tmp397=_cycalloc(sizeof(*_tmp397));_tmp397->v=(void*)res;
_tmp397;});res=(void*)({struct Cyc_Absyn_DatatypeType_struct*_tmp398=_cycalloc(
sizeof(*_tmp398));_tmp398[0]=({struct Cyc_Absyn_DatatypeType_struct _tmp399;
_tmp399.tag=2;_tmp399.f1=({struct Cyc_Absyn_DatatypeInfo _tmp39A;_tmp39A.datatype_info=(
union Cyc_Absyn_DatatypeInfoU_union)({union Cyc_Absyn_DatatypeInfoU_union _tmp39B;(
_tmp39B.KnownDatatype).tag=1;(_tmp39B.KnownDatatype).f1=({struct Cyc_Absyn_Datatypedecl**
_tmp39C=_cycalloc(sizeof(*_tmp39C));_tmp39C[0]=tud;_tmp39C;});_tmp39B;});_tmp39A.targs=
_tmp392;_tmp39A.rgn=_tmp396;_tmp39A;});_tmp399;});_tmp398;});Cyc_Tcutil_unchecked_cast(
te,e,res,(void*)1);}goto _LL1D9;_LL1DE:;_LL1DF: goto _LL1D9;_LL1D9:;}{struct Cyc_List_List*
ts=tuf->typs;for(0;es != 0  && ts != 0;(es=es->tl,ts=ts->tl)){struct Cyc_Absyn_Exp*e=(
struct Cyc_Absyn_Exp*)es->hd;void*t=Cyc_Tcutil_rsubstitute(_tmp38F,_tmp391,(*((
struct _tuple9*)ts->hd)).f2);Cyc_Tcexp_tcExpInitializer(te,(void**)& t,e);if(!Cyc_Tcutil_coerce_arg(
te,e,t)){({struct Cyc_String_pa_struct _tmp3A1;_tmp3A1.tag=0;_tmp3A1.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)(e->topt == 0?(struct _dyneither_ptr)({const
char*_tmp3A2="?";_tag_dyneither(_tmp3A2,sizeof(char),2);}): Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)));{struct Cyc_String_pa_struct
_tmp3A0;_tmp3A0.tag=0;_tmp3A0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t));{struct Cyc_String_pa_struct _tmp39F;_tmp39F.tag=0;
_tmp39F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
tuf->name));{void*_tmp39D[3]={& _tmp39F,& _tmp3A0,& _tmp3A1};Cyc_Tcutil_terr(e->loc,({
const char*_tmp39E="datatype constructor %s expects argument of type %s but this argument has type %s";
_tag_dyneither(_tmp39E,sizeof(char),82);}),_tag_dyneither(_tmp39D,sizeof(void*),
3));}}}});Cyc_Tcutil_explain_failure();}}if(es != 0)return({struct Cyc_String_pa_struct
_tmp3A5;_tmp3A5.tag=0;_tmp3A5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(tuf->name));{void*_tmp3A3[1]={& _tmp3A5};Cyc_Tcexp_expr_err(
te,((struct Cyc_Absyn_Exp*)es->hd)->loc,topt,({const char*_tmp3A4="too many arguments for datatype constructor %s";
_tag_dyneither(_tmp3A4,sizeof(char),47);}),_tag_dyneither(_tmp3A3,sizeof(void*),
1));}});if(ts != 0)return({struct Cyc_String_pa_struct _tmp3A8;_tmp3A8.tag=0;
_tmp3A8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
tuf->name));{void*_tmp3A6[1]={& _tmp3A8};Cyc_Tcexp_expr_err(te,loc,topt,({const
char*_tmp3A7="too few arguments for datatype constructor %s";_tag_dyneither(
_tmp3A7,sizeof(char),46);}),_tag_dyneither(_tmp3A6,sizeof(void*),1));}});return
res;}}static int Cyc_Tcexp_zeroable_type(void*t){void*_tmp3AE=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_PtrInfo _tmp3AF;struct Cyc_Absyn_PtrAtts _tmp3B0;struct Cyc_Absyn_Conref*
_tmp3B1;struct Cyc_Absyn_ArrayInfo _tmp3B2;void*_tmp3B3;struct Cyc_List_List*
_tmp3B4;struct Cyc_Absyn_AggrInfo _tmp3B5;union Cyc_Absyn_AggrInfoU_union _tmp3B6;
struct Cyc_List_List*_tmp3B7;struct Cyc_List_List*_tmp3B8;_LL1E1: if((int)_tmp3AE != 
0)goto _LL1E3;_LL1E2: return 1;_LL1E3: if(_tmp3AE <= (void*)4)goto _LL1EF;if(*((int*)
_tmp3AE)!= 0)goto _LL1E5;_LL1E4: goto _LL1E6;_LL1E5: if(*((int*)_tmp3AE)!= 1)goto
_LL1E7;_LL1E6: goto _LL1E8;_LL1E7: if(*((int*)_tmp3AE)!= 2)goto _LL1E9;_LL1E8: goto
_LL1EA;_LL1E9: if(*((int*)_tmp3AE)!= 3)goto _LL1EB;_LL1EA: return 0;_LL1EB: if(*((int*)
_tmp3AE)!= 4)goto _LL1ED;_tmp3AF=((struct Cyc_Absyn_PointerType_struct*)_tmp3AE)->f1;
_tmp3B0=_tmp3AF.ptr_atts;_tmp3B1=_tmp3B0.nullable;_LL1EC: return((int(*)(int,
struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(1,_tmp3B1);_LL1ED: if(*((int*)
_tmp3AE)!= 5)goto _LL1EF;_LL1EE: goto _LL1F0;_LL1EF: if((int)_tmp3AE != 1)goto _LL1F1;
_LL1F0: goto _LL1F2;_LL1F1: if(_tmp3AE <= (void*)4)goto _LL20B;if(*((int*)_tmp3AE)!= 
6)goto _LL1F3;_LL1F2: return 1;_LL1F3: if(*((int*)_tmp3AE)!= 7)goto _LL1F5;_tmp3B2=((
struct Cyc_Absyn_ArrayType_struct*)_tmp3AE)->f1;_tmp3B3=(void*)_tmp3B2.elt_type;
_LL1F4: return Cyc_Tcexp_zeroable_type(_tmp3B3);_LL1F5: if(*((int*)_tmp3AE)!= 8)
goto _LL1F7;_LL1F6: return 0;_LL1F7: if(*((int*)_tmp3AE)!= 9)goto _LL1F9;_tmp3B4=((
struct Cyc_Absyn_TupleType_struct*)_tmp3AE)->f1;_LL1F8: for(0;(unsigned int)
_tmp3B4;_tmp3B4=_tmp3B4->tl){if(!Cyc_Tcexp_zeroable_type((*((struct _tuple9*)
_tmp3B4->hd)).f2))return 0;}return 1;_LL1F9: if(*((int*)_tmp3AE)!= 10)goto _LL1FB;
_tmp3B5=((struct Cyc_Absyn_AggrType_struct*)_tmp3AE)->f1;_tmp3B6=_tmp3B5.aggr_info;
_tmp3B7=_tmp3B5.targs;_LL1FA: {struct Cyc_Absyn_Aggrdecl*_tmp3B9=Cyc_Absyn_get_known_aggrdecl(
_tmp3B6);if(_tmp3B9->impl == 0  || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(
_tmp3B9->impl))->exist_vars != 0)return 0;{struct _RegionHandle _tmp3BA=_new_region("r");
struct _RegionHandle*r=& _tmp3BA;_push_region(r);{struct Cyc_List_List*_tmp3BB=((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp3B9->tvs,_tmp3B7);{struct Cyc_List_List*
fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp3B9->impl))->fields;for(0;fs
!= 0;fs=fs->tl){if(!Cyc_Tcexp_zeroable_type(Cyc_Tcutil_rsubstitute(r,_tmp3BB,(
void*)((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))){int _tmp3BC=0;_npop_handler(0);
return _tmp3BC;}}}{int _tmp3BD=1;_npop_handler(0);return _tmp3BD;}};_pop_region(r);}}
_LL1FB: if(*((int*)_tmp3AE)!= 12)goto _LL1FD;_LL1FC: return 1;_LL1FD: if(*((int*)
_tmp3AE)!= 18)goto _LL1FF;_LL1FE: return 1;_LL1FF: if(*((int*)_tmp3AE)!= 11)goto
_LL201;_tmp3B8=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp3AE)->f2;_LL200: for(0;
_tmp3B8 != 0;_tmp3B8=_tmp3B8->tl){if(!Cyc_Tcexp_zeroable_type((void*)((struct Cyc_Absyn_Aggrfield*)
_tmp3B8->hd)->type))return 0;}return 1;_LL201: if(*((int*)_tmp3AE)!= 13)goto _LL203;
_LL202: return 1;_LL203: if(*((int*)_tmp3AE)!= 16)goto _LL205;_LL204: return 0;_LL205:
if(*((int*)_tmp3AE)!= 15)goto _LL207;_LL206: return 0;_LL207: if(*((int*)_tmp3AE)!= 
14)goto _LL209;_LL208: return 0;_LL209: if(*((int*)_tmp3AE)!= 17)goto _LL20B;_LL20A:
goto _LL20C;_LL20B: if((int)_tmp3AE != 2)goto _LL20D;_LL20C: goto _LL20E;_LL20D: if((
int)_tmp3AE != 3)goto _LL20F;_LL20E: goto _LL210;_LL20F: if(_tmp3AE <= (void*)4)goto
_LL211;if(*((int*)_tmp3AE)!= 19)goto _LL211;_LL210: goto _LL212;_LL211: if(_tmp3AE <= (
void*)4)goto _LL213;if(*((int*)_tmp3AE)!= 20)goto _LL213;_LL212: goto _LL214;_LL213:
if(_tmp3AE <= (void*)4)goto _LL1E0;if(*((int*)_tmp3AE)!= 21)goto _LL1E0;_LL214:({
struct Cyc_String_pa_struct _tmp3C0;_tmp3C0.tag=0;_tmp3C0.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));{void*_tmp3BE[1]={& _tmp3C0};((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const
char*_tmp3BF="bad type `%s' in zeroable type";_tag_dyneither(_tmp3BF,sizeof(char),
31);}),_tag_dyneither(_tmp3BE,sizeof(void*),1));}});_LL1E0:;}static void Cyc_Tcexp_check_malloc_type(
int allow_zero,struct Cyc_Position_Segment*loc,void**topt,void*t){if(Cyc_Tcutil_bits_only(
t) || allow_zero  && Cyc_Tcexp_zeroable_type(t))return;if(topt != 0){void*_tmp3C1=
Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo _tmp3C2;void*_tmp3C3;_LL216:
if(_tmp3C1 <= (void*)4)goto _LL218;if(*((int*)_tmp3C1)!= 4)goto _LL218;_tmp3C2=((
struct Cyc_Absyn_PointerType_struct*)_tmp3C1)->f1;_tmp3C3=(void*)_tmp3C2.elt_typ;
_LL217: Cyc_Tcutil_unify(_tmp3C3,t);if(Cyc_Tcutil_bits_only(t) || allow_zero  && 
Cyc_Tcexp_zeroable_type(t))return;goto _LL215;_LL218:;_LL219: goto _LL215;_LL215:;}({
struct Cyc_String_pa_struct _tmp3C7;_tmp3C7.tag=0;_tmp3C7.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));{struct Cyc_String_pa_struct
_tmp3C6;_tmp3C6.tag=0;_tmp3C6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)(
allow_zero?(struct _dyneither_ptr)({const char*_tmp3C8="calloc";_tag_dyneither(
_tmp3C8,sizeof(char),7);}):(struct _dyneither_ptr)({const char*_tmp3C9="malloc";
_tag_dyneither(_tmp3C9,sizeof(char),7);})));{void*_tmp3C4[2]={& _tmp3C6,& _tmp3C7};
Cyc_Tcutil_terr(loc,({const char*_tmp3C5="%s cannot be used with type %s\n\t(type needs initialization)";
_tag_dyneither(_tmp3C5,sizeof(char),60);}),_tag_dyneither(_tmp3C4,sizeof(void*),
2));}}});}static void*Cyc_Tcexp_mallocRgn(void*rgn){void*_tmp3CA=Cyc_Tcutil_typ_kind(
Cyc_Tcutil_compress(rgn));_LL21B: if((int)_tmp3CA != 4)goto _LL21D;_LL21C: return(
void*)3;_LL21D:;_LL21E: return(void*)2;_LL21A:;}static void*Cyc_Tcexp_tcMalloc(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
ropt,void***t,struct Cyc_Absyn_Exp**e,int*is_calloc,int*is_fat){void*rgn=(void*)2;
if(ropt != 0){void*expected_type=(void*)({struct Cyc_Absyn_RgnHandleType_struct*
_tmp3D0=_cycalloc(sizeof(*_tmp3D0));_tmp3D0[0]=({struct Cyc_Absyn_RgnHandleType_struct
_tmp3D1;_tmp3D1.tag=14;_tmp3D1.f1=(void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_trk,Cyc_Tcenv_lookup_opt_type_vars(te));_tmp3D1;});_tmp3D0;});void*
handle_type=Cyc_Tcexp_tcExp(te,(void**)& expected_type,(struct Cyc_Absyn_Exp*)ropt);
void*_tmp3CB=Cyc_Tcutil_compress(handle_type);void*_tmp3CC;_LL220: if(_tmp3CB <= (
void*)4)goto _LL222;if(*((int*)_tmp3CB)!= 14)goto _LL222;_tmp3CC=(void*)((struct
Cyc_Absyn_RgnHandleType_struct*)_tmp3CB)->f1;_LL221: rgn=_tmp3CC;Cyc_Tcenv_check_rgn_accessible(
te,loc,rgn);goto _LL21F;_LL222:;_LL223:({struct Cyc_String_pa_struct _tmp3CF;
_tmp3CF.tag=0;_tmp3CF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
handle_type));{void*_tmp3CD[1]={& _tmp3CF};Cyc_Tcutil_terr(ropt->loc,({const char*
_tmp3CE="expecting region_t type but found %s";_tag_dyneither(_tmp3CE,sizeof(
char),37);}),_tag_dyneither(_tmp3CD,sizeof(void*),1));}});goto _LL21F;_LL21F:;}
else{if(topt != 0){void*optrgn=(void*)0;if(Cyc_Tcutil_rgn_of_pointer(*topt,&
optrgn))rgn=Cyc_Tcexp_mallocRgn(optrgn);}}Cyc_Tcexp_tcExp(te,(void**)& Cyc_Absyn_uint_typ,*
e);{void*elt_type;struct Cyc_Absyn_Exp*num_elts;int one_elt;if(*is_calloc){if(*t == 
0)({void*_tmp3D2=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp3D3="calloc with empty type";_tag_dyneither(_tmp3D3,sizeof(char),
23);}),_tag_dyneither(_tmp3D2,sizeof(void*),0));});elt_type=*((void**)
_check_null(*t));Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(te),(
void*)1,1,elt_type);Cyc_Tcexp_check_malloc_type(1,loc,topt,elt_type);num_elts=*e;
one_elt=0;}else{void*_tmp3D4=(void*)(*e)->r;void*_tmp3D5;void*_tmp3D6;struct Cyc_List_List*
_tmp3D7;struct Cyc_List_List _tmp3D8;struct Cyc_Absyn_Exp*_tmp3D9;struct Cyc_List_List*
_tmp3DA;struct Cyc_List_List _tmp3DB;struct Cyc_Absyn_Exp*_tmp3DC;struct Cyc_List_List*
_tmp3DD;_LL225: if(*((int*)_tmp3D4)!= 18)goto _LL227;_tmp3D5=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp3D4)->f1;_LL226: elt_type=_tmp3D5;*t=(void**)({void**_tmp3DE=_cycalloc(
sizeof(*_tmp3DE));_tmp3DE[0]=elt_type;_tmp3DE;});num_elts=Cyc_Absyn_uint_exp(1,0);
Cyc_Tcexp_tcExp(te,(void**)& Cyc_Absyn_uint_typ,num_elts);one_elt=1;goto _LL224;
_LL227: if(*((int*)_tmp3D4)!= 3)goto _LL229;_tmp3D6=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp3D4)->f1;if((int)_tmp3D6 != 1)goto _LL229;_tmp3D7=((struct Cyc_Absyn_Primop_e_struct*)
_tmp3D4)->f2;if(_tmp3D7 == 0)goto _LL229;_tmp3D8=*_tmp3D7;_tmp3D9=(struct Cyc_Absyn_Exp*)
_tmp3D8.hd;_tmp3DA=_tmp3D8.tl;if(_tmp3DA == 0)goto _LL229;_tmp3DB=*_tmp3DA;_tmp3DC=(
struct Cyc_Absyn_Exp*)_tmp3DB.hd;_tmp3DD=_tmp3DB.tl;if(_tmp3DD != 0)goto _LL229;
_LL228:{struct _tuple0 _tmp3E0=({struct _tuple0 _tmp3DF;_tmp3DF.f1=(void*)_tmp3D9->r;
_tmp3DF.f2=(void*)_tmp3DC->r;_tmp3DF;});void*_tmp3E1;void*_tmp3E2;void*_tmp3E3;
void*_tmp3E4;_LL22C: _tmp3E1=_tmp3E0.f1;if(*((int*)_tmp3E1)!= 18)goto _LL22E;
_tmp3E2=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp3E1)->f1;_LL22D: Cyc_Tcexp_check_malloc_type(
0,loc,topt,_tmp3E2);elt_type=_tmp3E2;*t=(void**)({void**_tmp3E5=_cycalloc(
sizeof(*_tmp3E5));_tmp3E5[0]=elt_type;_tmp3E5;});num_elts=_tmp3DC;one_elt=0;goto
_LL22B;_LL22E: _tmp3E3=_tmp3E0.f2;if(*((int*)_tmp3E3)!= 18)goto _LL230;_tmp3E4=(
void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp3E3)->f1;_LL22F: Cyc_Tcexp_check_malloc_type(
0,loc,topt,_tmp3E4);elt_type=_tmp3E4;*t=(void**)({void**_tmp3E6=_cycalloc(
sizeof(*_tmp3E6));_tmp3E6[0]=elt_type;_tmp3E6;});num_elts=_tmp3D9;one_elt=0;goto
_LL22B;_LL230:;_LL231: goto No_sizeof;_LL22B:;}goto _LL224;_LL229:;_LL22A: No_sizeof:
elt_type=Cyc_Absyn_char_typ;*t=(void**)({void**_tmp3E7=_cycalloc(sizeof(*_tmp3E7));
_tmp3E7[0]=elt_type;_tmp3E7;});num_elts=*e;one_elt=0;goto _LL224;_LL224:;}*e=
num_elts;*is_fat=!one_elt;{void*_tmp3E8=elt_type;struct Cyc_Absyn_AggrInfo _tmp3E9;
union Cyc_Absyn_AggrInfoU_union _tmp3EA;struct Cyc_Absyn_Aggrdecl**_tmp3EB;struct
Cyc_Absyn_Aggrdecl*_tmp3EC;_LL233: if(_tmp3E8 <= (void*)4)goto _LL235;if(*((int*)
_tmp3E8)!= 10)goto _LL235;_tmp3E9=((struct Cyc_Absyn_AggrType_struct*)_tmp3E8)->f1;
_tmp3EA=_tmp3E9.aggr_info;if((((((struct Cyc_Absyn_AggrType_struct*)_tmp3E8)->f1).aggr_info).KnownAggr).tag
!= 1)goto _LL235;_tmp3EB=(_tmp3EA.KnownAggr).f1;_tmp3EC=*_tmp3EB;_LL234: if(
_tmp3EC->impl != 0  && ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp3EC->impl))->exist_vars
!= 0)({void*_tmp3ED=0;Cyc_Tcutil_terr(loc,({const char*_tmp3EE="malloc with existential types not yet implemented";
_tag_dyneither(_tmp3EE,sizeof(char),50);}),_tag_dyneither(_tmp3ED,sizeof(void*),
0));});goto _LL232;_LL235:;_LL236: goto _LL232;_LL232:;}{void*(*_tmp3EF)(void*t,
void*rgn,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Conref*zero_term)=Cyc_Absyn_at_typ;
struct Cyc_Absyn_Conref*_tmp3F0=Cyc_Absyn_false_conref;if(topt != 0){void*_tmp3F1=
Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo _tmp3F2;struct Cyc_Absyn_PtrAtts
_tmp3F3;struct Cyc_Absyn_Conref*_tmp3F4;struct Cyc_Absyn_Conref*_tmp3F5;struct Cyc_Absyn_Conref*
_tmp3F6;_LL238: if(_tmp3F1 <= (void*)4)goto _LL23A;if(*((int*)_tmp3F1)!= 4)goto
_LL23A;_tmp3F2=((struct Cyc_Absyn_PointerType_struct*)_tmp3F1)->f1;_tmp3F3=
_tmp3F2.ptr_atts;_tmp3F4=_tmp3F3.nullable;_tmp3F5=_tmp3F3.bounds;_tmp3F6=_tmp3F3.zero_term;
_LL239: _tmp3F0=_tmp3F6;if(((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,_tmp3F4))_tmp3EF=Cyc_Absyn_star_typ;if(((int(*)(int,struct Cyc_Absyn_Conref*x))
Cyc_Absyn_conref_def)(0,_tmp3F6) && !(*is_calloc)){({void*_tmp3F7=0;Cyc_Tcutil_warn(
loc,({const char*_tmp3F8="converting malloc to calloc to ensure zero-termination";
_tag_dyneither(_tmp3F8,sizeof(char),55);}),_tag_dyneither(_tmp3F7,sizeof(void*),
0));});*is_calloc=1;}{void*_tmp3F9=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,
_tmp3F5);struct Cyc_Absyn_Exp*_tmp3FA;_LL23D: if((int)_tmp3F9 != 0)goto _LL23F;
_LL23E: goto _LL23C;_LL23F: if(_tmp3F9 <= (void*)1)goto _LL241;if(*((int*)_tmp3F9)!= 
0)goto _LL241;_tmp3FA=((struct Cyc_Absyn_Upper_b_struct*)_tmp3F9)->f1;if(!(!
one_elt))goto _LL241;_LL240: {int _tmp3FB=Cyc_Evexp_c_can_eval(num_elts);if(
_tmp3FB  && Cyc_Evexp_same_const_exp(_tmp3FA,num_elts)){*is_fat=0;return Cyc_Absyn_atb_typ(
elt_type,rgn,Cyc_Absyn_empty_tqual(0),Cyc_Absyn_conref_val(_tmp3F5),_tmp3F0);}{
void*_tmp3FC=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
num_elts->topt))->v);void*_tmp3FD;_LL244: if(_tmp3FC <= (void*)4)goto _LL246;if(*((
int*)_tmp3FC)!= 18)goto _LL246;_tmp3FD=(void*)((struct Cyc_Absyn_TagType_struct*)
_tmp3FC)->f1;_LL245: {struct Cyc_Absyn_Exp*_tmp3FE=Cyc_Absyn_cast_exp(Cyc_Absyn_uint_typ,
Cyc_Absyn_valueof_exp(_tmp3FD,0),0,(void*)1,0);if(Cyc_Evexp_same_const_exp(
_tmp3FE,_tmp3FA)){*is_fat=0;return Cyc_Absyn_atb_typ(elt_type,rgn,Cyc_Absyn_empty_tqual(
0),Cyc_Absyn_conref_val(_tmp3F5),_tmp3F0);}goto _LL243;}_LL246:;_LL247: goto _LL243;
_LL243:;}goto _LL23C;}_LL241:;_LL242: goto _LL23C;_LL23C:;}goto _LL237;_LL23A:;
_LL23B: goto _LL237;_LL237:;}if(!one_elt)_tmp3EF=Cyc_Absyn_dyneither_typ;return
_tmp3EF(elt_type,rgn,Cyc_Absyn_empty_tqual(0),_tmp3F0);}}}static void*Cyc_Tcexp_tcSwap(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
e1,struct Cyc_Absyn_Exp*e2){Cyc_Tcexp_tcExp(te,0,e1);Cyc_Tcexp_tcExp(te,(void**)((
void**)((void*)&((struct Cyc_Core_Opt*)_check_null(e1->topt))->v)),e2);{void*t1=(
void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v;void*t2=(void*)((struct Cyc_Core_Opt*)
_check_null(e2->topt))->v;{void*_tmp3FF=Cyc_Tcutil_compress(t1);_LL249: if(
_tmp3FF <= (void*)4)goto _LL24B;if(*((int*)_tmp3FF)!= 7)goto _LL24B;_LL24A:({void*
_tmp400=0;Cyc_Tcutil_terr(loc,({const char*_tmp401="cannot assign to an array";
_tag_dyneither(_tmp401,sizeof(char),26);}),_tag_dyneither(_tmp400,sizeof(void*),
0));});goto _LL248;_LL24B:;_LL24C: goto _LL248;_LL248:;}{int ign_1=0;if(!Cyc_Tcutil_is_pointer_or_boxed(
t1,& ign_1))({void*_tmp402=0;Cyc_Tcutil_terr(loc,({const char*_tmp403="Swap not allowed for non-pointer or non-word-sized types.";
_tag_dyneither(_tmp403,sizeof(char),58);}),_tag_dyneither(_tmp402,sizeof(void*),
0));});if(!Cyc_Absyn_is_lvalue(e1))return({void*_tmp404=0;Cyc_Tcexp_expr_err(te,
e1->loc,topt,({const char*_tmp405="swap non-lvalue";_tag_dyneither(_tmp405,
sizeof(char),16);}),_tag_dyneither(_tmp404,sizeof(void*),0));});if(!Cyc_Absyn_is_lvalue(
e2))return({void*_tmp406=0;Cyc_Tcexp_expr_err(te,e2->loc,topt,({const char*
_tmp407="swap non-lvalue";_tag_dyneither(_tmp407,sizeof(char),16);}),
_tag_dyneither(_tmp406,sizeof(void*),0));});{void*t_ign1=(void*)0;void*t_ign2=(
void*)0;int b_ign1=0;if(Cyc_Tcutil_is_zero_ptr_deref(e1,& t_ign1,& b_ign1,& t_ign2))
return({void*_tmp408=0;Cyc_Tcexp_expr_err(te,e1->loc,topt,({const char*_tmp409="swap value in zeroterm array";
_tag_dyneither(_tmp409,sizeof(char),29);}),_tag_dyneither(_tmp408,sizeof(void*),
0));});if(Cyc_Tcutil_is_zero_ptr_deref(e2,& t_ign1,& b_ign1,& t_ign2))return({void*
_tmp40A=0;Cyc_Tcexp_expr_err(te,e2->loc,topt,({const char*_tmp40B="swap value in zeroterm array";
_tag_dyneither(_tmp40B,sizeof(char),29);}),_tag_dyneither(_tmp40A,sizeof(void*),
0));});Cyc_Tcexp_check_writable(te,e1);Cyc_Tcexp_check_writable(te,e2);if(!Cyc_Tcutil_unify(
t1,t2)){void*_tmp40C=({struct Cyc_String_pa_struct _tmp410;_tmp410.tag=0;_tmp410.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2));{struct
Cyc_String_pa_struct _tmp40F;_tmp40F.tag=0;_tmp40F.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));{void*_tmp40D[2]={& _tmp40F,&
_tmp410};Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp40E="type mismatch: %s != %s";
_tag_dyneither(_tmp40E,sizeof(char),24);}),_tag_dyneither(_tmp40D,sizeof(void*),
2));}}});return _tmp40C;}return(void*)0;}}}}int Cyc_Tcexp_in_stmt_exp=0;static void*
Cyc_Tcexp_tcStmtExp(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**
topt,struct Cyc_Absyn_Stmt*s){struct _RegionHandle*_tmp411=Cyc_Tcenv_get_fnrgn(te);{
int old_stmt_exp_state=Cyc_Tcexp_in_stmt_exp;Cyc_Tcstmt_tcStmt(Cyc_Tcenv_set_encloser(
_tmp411,te,s),s,1);Cyc_Tcexp_in_stmt_exp=old_stmt_exp_state;}Cyc_NewControlFlow_set_encloser(
s,Cyc_Tcenv_get_encloser(te));while(1){void*_tmp412=(void*)s->r;struct Cyc_Absyn_Exp*
_tmp413;struct Cyc_Absyn_Stmt*_tmp414;struct Cyc_Absyn_Stmt*_tmp415;struct Cyc_Absyn_Decl*
_tmp416;struct Cyc_Absyn_Stmt*_tmp417;_LL24E: if(_tmp412 <= (void*)1)goto _LL254;if(*((
int*)_tmp412)!= 0)goto _LL250;_tmp413=((struct Cyc_Absyn_Exp_s_struct*)_tmp412)->f1;
_LL24F: return(void*)((struct Cyc_Core_Opt*)_check_null(_tmp413->topt))->v;_LL250:
if(*((int*)_tmp412)!= 1)goto _LL252;_tmp414=((struct Cyc_Absyn_Seq_s_struct*)
_tmp412)->f1;_tmp415=((struct Cyc_Absyn_Seq_s_struct*)_tmp412)->f2;_LL251: s=
_tmp415;continue;_LL252: if(*((int*)_tmp412)!= 11)goto _LL254;_tmp416=((struct Cyc_Absyn_Decl_s_struct*)
_tmp412)->f1;_tmp417=((struct Cyc_Absyn_Decl_s_struct*)_tmp412)->f2;_LL253: s=
_tmp417;continue;_LL254:;_LL255: return({void*_tmp418=0;Cyc_Tcexp_expr_err(te,loc,
topt,({const char*_tmp419="statement expression must end with expression";
_tag_dyneither(_tmp419,sizeof(char),46);}),_tag_dyneither(_tmp418,sizeof(void*),
0));});_LL24D:;}}static void*Cyc_Tcexp_tcNew(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*rgn_handle,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*
e1){void*rgn=(void*)2;if(rgn_handle != 0){void*expected_type=(void*)({struct Cyc_Absyn_RgnHandleType_struct*
_tmp41F=_cycalloc(sizeof(*_tmp41F));_tmp41F[0]=({struct Cyc_Absyn_RgnHandleType_struct
_tmp420;_tmp420.tag=14;_tmp420.f1=(void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_trk,Cyc_Tcenv_lookup_opt_type_vars(te));_tmp420;});_tmp41F;});void*
handle_type=Cyc_Tcexp_tcExp(te,(void**)& expected_type,(struct Cyc_Absyn_Exp*)
rgn_handle);void*_tmp41A=Cyc_Tcutil_compress(handle_type);void*_tmp41B;_LL257:
if(_tmp41A <= (void*)4)goto _LL259;if(*((int*)_tmp41A)!= 14)goto _LL259;_tmp41B=(
void*)((struct Cyc_Absyn_RgnHandleType_struct*)_tmp41A)->f1;_LL258: rgn=_tmp41B;
Cyc_Tcenv_check_rgn_accessible(te,loc,rgn);goto _LL256;_LL259:;_LL25A:({struct Cyc_String_pa_struct
_tmp41E;_tmp41E.tag=0;_tmp41E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(handle_type));{void*_tmp41C[1]={& _tmp41E};Cyc_Tcutil_terr(
rgn_handle->loc,({const char*_tmp41D="expecting region_t type but found %s";
_tag_dyneither(_tmp41D,sizeof(char),37);}),_tag_dyneither(_tmp41C,sizeof(void*),
1));}});goto _LL256;_LL256:;}else{if(topt != 0){void*optrgn=(void*)0;if(Cyc_Tcutil_rgn_of_pointer(*
topt,& optrgn))rgn=Cyc_Tcexp_mallocRgn(optrgn);}}{void*_tmp421=(void*)e1->r;
struct Cyc_Core_Opt*_tmp422;struct Cyc_List_List*_tmp423;struct Cyc_List_List*
_tmp424;union Cyc_Absyn_Cnst_union _tmp425;struct _dyneither_ptr _tmp426;_LL25C: if(*((
int*)_tmp421)!= 29)goto _LL25E;_LL25D: {void*_tmp427=Cyc_Tcexp_tcExpNoPromote(te,
topt,e1);void*_tmp428=Cyc_Tcutil_compress(_tmp427);struct Cyc_Absyn_ArrayInfo
_tmp429;void*_tmp42A;struct Cyc_Absyn_Tqual _tmp42B;struct Cyc_Absyn_Exp*_tmp42C;
struct Cyc_Absyn_Conref*_tmp42D;_LL267: if(_tmp428 <= (void*)4)goto _LL269;if(*((int*)
_tmp428)!= 7)goto _LL269;_tmp429=((struct Cyc_Absyn_ArrayType_struct*)_tmp428)->f1;
_tmp42A=(void*)_tmp429.elt_type;_tmp42B=_tmp429.tq;_tmp42C=_tmp429.num_elts;
_tmp42D=_tmp429.zero_term;_LL268: {struct Cyc_Absyn_Exp*bnd=(struct Cyc_Absyn_Exp*)
_check_null(_tmp42C);void*b;{void*_tmp42E=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(bnd->topt))->v);void*_tmp42F;_LL26C: if(_tmp42E <= (void*)4)goto _LL26E;
if(*((int*)_tmp42E)!= 18)goto _LL26E;_tmp42F=(void*)((struct Cyc_Absyn_TagType_struct*)
_tmp42E)->f1;_LL26D: b=(void*)({struct Cyc_Absyn_Upper_b_struct*_tmp430=_cycalloc(
sizeof(*_tmp430));_tmp430[0]=({struct Cyc_Absyn_Upper_b_struct _tmp431;_tmp431.tag=
0;_tmp431.f1=Cyc_Absyn_cast_exp(Cyc_Absyn_uint_typ,Cyc_Absyn_valueof_exp(_tmp42F,
0),0,(void*)1,0);_tmp431;});_tmp430;});goto _LL26B;_LL26E:;_LL26F: if(Cyc_Tcutil_is_const_exp(
te,bnd))b=(void*)({struct Cyc_Absyn_Upper_b_struct*_tmp432=_cycalloc(sizeof(*
_tmp432));_tmp432[0]=({struct Cyc_Absyn_Upper_b_struct _tmp433;_tmp433.tag=0;
_tmp433.f1=bnd;_tmp433;});_tmp432;});else{b=(void*)0;}_LL26B:;}{void*res_typ=(
void*)({struct Cyc_Absyn_PointerType_struct*_tmp435=_cycalloc(sizeof(*_tmp435));
_tmp435[0]=({struct Cyc_Absyn_PointerType_struct _tmp436;_tmp436.tag=4;_tmp436.f1=({
struct Cyc_Absyn_PtrInfo _tmp437;_tmp437.elt_typ=(void*)_tmp42A;_tmp437.elt_tq=
_tmp42B;_tmp437.ptr_atts=({struct Cyc_Absyn_PtrAtts _tmp438;_tmp438.rgn=(void*)rgn;
_tmp438.nullable=((struct Cyc_Absyn_Conref*(*)())Cyc_Absyn_empty_conref)();
_tmp438.bounds=Cyc_Absyn_new_conref(b);_tmp438.zero_term=_tmp42D;_tmp438.ptrloc=
0;_tmp438;});_tmp437;});_tmp436;});_tmp435;});if(topt != 0){if(!Cyc_Tcutil_unify(*
topt,res_typ) && Cyc_Tcutil_silent_castable(te,loc,res_typ,*topt)){e->topt=({
struct Cyc_Core_Opt*_tmp434=_cycalloc(sizeof(*_tmp434));_tmp434->v=(void*)res_typ;
_tmp434;});Cyc_Tcutil_unchecked_cast(te,e,*topt,(void*)3);res_typ=*topt;}}return
res_typ;}}_LL269:;_LL26A:({void*_tmp439=0;((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp43A="tcNew: comprehension returned non-array type";
_tag_dyneither(_tmp43A,sizeof(char),45);}),_tag_dyneither(_tmp439,sizeof(void*),
0));});_LL266:;}_LL25E: if(*((int*)_tmp421)!= 37)goto _LL260;_tmp422=((struct Cyc_Absyn_UnresolvedMem_e_struct*)
_tmp421)->f1;_tmp423=((struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp421)->f2;
_LL25F:(void*)(e1->r=(void*)((void*)({struct Cyc_Absyn_Array_e_struct*_tmp43B=
_cycalloc(sizeof(*_tmp43B));_tmp43B[0]=({struct Cyc_Absyn_Array_e_struct _tmp43C;
_tmp43C.tag=28;_tmp43C.f1=_tmp423;_tmp43C;});_tmp43B;})));_tmp424=_tmp423;goto
_LL261;_LL260: if(*((int*)_tmp421)!= 28)goto _LL262;_tmp424=((struct Cyc_Absyn_Array_e_struct*)
_tmp421)->f1;_LL261: {void**elt_typ_opt=0;int zero_term=0;if(topt != 0){void*
_tmp43D=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo _tmp43E;void*_tmp43F;
void**_tmp440;struct Cyc_Absyn_Tqual _tmp441;struct Cyc_Absyn_PtrAtts _tmp442;struct
Cyc_Absyn_Conref*_tmp443;_LL271: if(_tmp43D <= (void*)4)goto _LL273;if(*((int*)
_tmp43D)!= 4)goto _LL273;_tmp43E=((struct Cyc_Absyn_PointerType_struct*)_tmp43D)->f1;
_tmp43F=(void*)_tmp43E.elt_typ;_tmp440=(void**)&(((struct Cyc_Absyn_PointerType_struct*)
_tmp43D)->f1).elt_typ;_tmp441=_tmp43E.elt_tq;_tmp442=_tmp43E.ptr_atts;_tmp443=
_tmp442.zero_term;_LL272: elt_typ_opt=(void**)_tmp440;zero_term=((int(*)(int,
struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp443);goto _LL270;_LL273:;
_LL274: goto _LL270;_LL270:;}{void*_tmp444=Cyc_Tcexp_tcArray(te,e1->loc,
elt_typ_opt,zero_term,_tmp424);e1->topt=({struct Cyc_Core_Opt*_tmp445=_cycalloc(
sizeof(*_tmp445));_tmp445->v=(void*)_tmp444;_tmp445;});{void*res_typ;{void*
_tmp446=Cyc_Tcutil_compress(_tmp444);struct Cyc_Absyn_ArrayInfo _tmp447;void*
_tmp448;struct Cyc_Absyn_Tqual _tmp449;struct Cyc_Absyn_Exp*_tmp44A;struct Cyc_Absyn_Conref*
_tmp44B;_LL276: if(_tmp446 <= (void*)4)goto _LL278;if(*((int*)_tmp446)!= 7)goto
_LL278;_tmp447=((struct Cyc_Absyn_ArrayType_struct*)_tmp446)->f1;_tmp448=(void*)
_tmp447.elt_type;_tmp449=_tmp447.tq;_tmp44A=_tmp447.num_elts;_tmp44B=_tmp447.zero_term;
_LL277: res_typ=(void*)({struct Cyc_Absyn_PointerType_struct*_tmp44C=_cycalloc(
sizeof(*_tmp44C));_tmp44C[0]=({struct Cyc_Absyn_PointerType_struct _tmp44D;_tmp44D.tag=
4;_tmp44D.f1=({struct Cyc_Absyn_PtrInfo _tmp44E;_tmp44E.elt_typ=(void*)_tmp448;
_tmp44E.elt_tq=_tmp449;_tmp44E.ptr_atts=({struct Cyc_Absyn_PtrAtts _tmp44F;_tmp44F.rgn=(
void*)rgn;_tmp44F.nullable=((struct Cyc_Absyn_Conref*(*)())Cyc_Absyn_empty_conref)();
_tmp44F.bounds=Cyc_Absyn_new_conref((void*)({struct Cyc_Absyn_Upper_b_struct*
_tmp450=_cycalloc(sizeof(*_tmp450));_tmp450[0]=({struct Cyc_Absyn_Upper_b_struct
_tmp451;_tmp451.tag=0;_tmp451.f1=(struct Cyc_Absyn_Exp*)_check_null(_tmp44A);
_tmp451;});_tmp450;}));_tmp44F.zero_term=_tmp44B;_tmp44F.ptrloc=0;_tmp44F;});
_tmp44E;});_tmp44D;});_tmp44C;});if(topt != 0){if(!Cyc_Tcutil_unify(*topt,res_typ)
 && Cyc_Tcutil_silent_castable(te,loc,res_typ,*topt)){e->topt=({struct Cyc_Core_Opt*
_tmp452=_cycalloc(sizeof(*_tmp452));_tmp452->v=(void*)res_typ;_tmp452;});Cyc_Tcutil_unchecked_cast(
te,e,*topt,(void*)3);res_typ=*topt;}}goto _LL275;_LL278:;_LL279:({void*_tmp453=0;((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const
char*_tmp454="tcExpNoPromote on Array_e returned non-array type";_tag_dyneither(
_tmp454,sizeof(char),50);}),_tag_dyneither(_tmp453,sizeof(void*),0));});_LL275:;}
return res_typ;}}}_LL262: if(*((int*)_tmp421)!= 0)goto _LL264;_tmp425=((struct Cyc_Absyn_Const_e_struct*)
_tmp421)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)_tmp421)->f1).String_c).tag
!= 5)goto _LL264;_tmp426=(_tmp425.String_c).f1;_LL263: {void*_tmp455=Cyc_Absyn_atb_typ(
Cyc_Absyn_char_typ,rgn,Cyc_Absyn_const_tqual(0),(void*)0,Cyc_Absyn_true_conref);
void*_tmp456=Cyc_Tcexp_tcExp(te,(void**)& _tmp455,e1);return Cyc_Absyn_atb_typ(
_tmp456,rgn,Cyc_Absyn_empty_tqual(0),(void*)({struct Cyc_Absyn_Upper_b_struct*
_tmp457=_cycalloc(sizeof(*_tmp457));_tmp457[0]=({struct Cyc_Absyn_Upper_b_struct
_tmp458;_tmp458.tag=0;_tmp458.f1=Cyc_Absyn_uint_exp(1,0);_tmp458;});_tmp457;}),
Cyc_Absyn_false_conref);}_LL264:;_LL265: {void**topt2=0;if(topt != 0){void*
_tmp459=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo _tmp45A;void*_tmp45B;
void**_tmp45C;struct Cyc_Absyn_Tqual _tmp45D;_LL27B: if(_tmp459 <= (void*)4)goto
_LL27F;if(*((int*)_tmp459)!= 4)goto _LL27D;_tmp45A=((struct Cyc_Absyn_PointerType_struct*)
_tmp459)->f1;_tmp45B=(void*)_tmp45A.elt_typ;_tmp45C=(void**)&(((struct Cyc_Absyn_PointerType_struct*)
_tmp459)->f1).elt_typ;_tmp45D=_tmp45A.elt_tq;_LL27C: topt2=(void**)_tmp45C;goto
_LL27A;_LL27D: if(*((int*)_tmp459)!= 2)goto _LL27F;_LL27E: topt2=({void**_tmp45E=
_cycalloc(sizeof(*_tmp45E));_tmp45E[0]=*topt;_tmp45E;});goto _LL27A;_LL27F:;
_LL280: goto _LL27A;_LL27A:;}{void*telt=Cyc_Tcexp_tcExp(te,topt2,e1);void*res_typ=(
void*)({struct Cyc_Absyn_PointerType_struct*_tmp460=_cycalloc(sizeof(*_tmp460));
_tmp460[0]=({struct Cyc_Absyn_PointerType_struct _tmp461;_tmp461.tag=4;_tmp461.f1=({
struct Cyc_Absyn_PtrInfo _tmp462;_tmp462.elt_typ=(void*)telt;_tmp462.elt_tq=Cyc_Absyn_empty_tqual(
0);_tmp462.ptr_atts=({struct Cyc_Absyn_PtrAtts _tmp463;_tmp463.rgn=(void*)rgn;
_tmp463.nullable=((struct Cyc_Absyn_Conref*(*)())Cyc_Absyn_empty_conref)();
_tmp463.bounds=Cyc_Absyn_bounds_one_conref;_tmp463.zero_term=Cyc_Absyn_false_conref;
_tmp463.ptrloc=0;_tmp463;});_tmp462;});_tmp461;});_tmp460;});if(topt != 0){if(!
Cyc_Tcutil_unify(*topt,res_typ) && Cyc_Tcutil_silent_castable(te,loc,res_typ,*
topt)){e->topt=({struct Cyc_Core_Opt*_tmp45F=_cycalloc(sizeof(*_tmp45F));_tmp45F->v=(
void*)res_typ;_tmp45F;});Cyc_Tcutil_unchecked_cast(te,e,*topt,(void*)3);res_typ=*
topt;}}return res_typ;}}_LL25B:;}}void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*te,
void**topt,struct Cyc_Absyn_Exp*e){void*t=Cyc_Tcutil_compress(Cyc_Tcexp_tcExpNoPromote(
te,topt,e));void*_tmp464=t;struct Cyc_Absyn_ArrayInfo _tmp465;void*_tmp466;struct
Cyc_Absyn_Tqual _tmp467;struct Cyc_Absyn_Exp*_tmp468;struct Cyc_Absyn_Conref*
_tmp469;_LL282: if(_tmp464 <= (void*)4)goto _LL284;if(*((int*)_tmp464)!= 7)goto
_LL284;_tmp465=((struct Cyc_Absyn_ArrayType_struct*)_tmp464)->f1;_tmp466=(void*)
_tmp465.elt_type;_tmp467=_tmp465.tq;_tmp468=_tmp465.num_elts;_tmp469=_tmp465.zero_term;
_LL283: {void*_tmp46B;struct _tuple6 _tmp46A=Cyc_Tcutil_addressof_props(te,e);
_tmp46B=_tmp46A.f2;{void*_tmp46C=_tmp468 == 0?(void*)((void*)0):(void*)({struct
Cyc_Absyn_Upper_b_struct*_tmp46D=_cycalloc(sizeof(*_tmp46D));_tmp46D[0]=({struct
Cyc_Absyn_Upper_b_struct _tmp46E;_tmp46E.tag=0;_tmp46E.f1=(struct Cyc_Absyn_Exp*)
_tmp468;_tmp46E;});_tmp46D;});t=Cyc_Absyn_atb_typ(_tmp466,_tmp46B,_tmp467,
_tmp46C,_tmp469);(void*)(((struct Cyc_Core_Opt*)_check_null(e->topt))->v=(void*)t);
return t;}}_LL284:;_LL285: return t;_LL281:;}void*Cyc_Tcexp_tcExpInitializer(struct
Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){void*t=Cyc_Tcexp_tcExpNoPromote(
te,topt,e);struct _RegionHandle*_tmp46F=Cyc_Tcenv_get_fnrgn(te);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(
_tmp46F,t) && !Cyc_Tcutil_is_noalias_path(_tmp46F,e)){({struct Cyc_String_pa_struct
_tmp474;_tmp474.tag=0;_tmp474.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(e));{struct Cyc_String_pa_struct _tmp473;_tmp473.tag=0;
_tmp473.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v));{struct Cyc_String_pa_struct
_tmp472;_tmp472.tag=0;_tmp472.f1=(struct _dyneither_ptr)(topt == 0?({const char*
_tmp475="NULL";_tag_dyneither(_tmp475,sizeof(char),5);}):(struct _dyneither_ptr)
Cyc_Absynpp_typ2string(*topt));{void*_tmp470[3]={& _tmp472,& _tmp473,& _tmp474};Cyc_fprintf(
Cyc_stderr,({const char*_tmp471="topt=%s, e->topt->v=%s, e=%s\n";_tag_dyneither(
_tmp471,sizeof(char),30);}),_tag_dyneither(_tmp470,sizeof(void*),3));}}}});({
void*_tmp476=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp477="Cannot consume non-unique paths; do swap instead";
_tag_dyneither(_tmp477,sizeof(char),49);}),_tag_dyneither(_tmp476,sizeof(void*),
0));});}{void*_tmp478=(void*)e->r;union Cyc_Absyn_Cnst_union _tmp479;_LL287: if(*((
int*)_tmp478)!= 28)goto _LL289;_LL288: goto _LL28A;_LL289: if(*((int*)_tmp478)!= 29)
goto _LL28B;_LL28A: goto _LL28C;_LL28B: if(*((int*)_tmp478)!= 0)goto _LL28D;_tmp479=((
struct Cyc_Absyn_Const_e_struct*)_tmp478)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)
_tmp478)->f1).String_c).tag != 5)goto _LL28D;_LL28C: return t;_LL28D:;_LL28E: t=Cyc_Tcutil_compress(
t);{void*_tmp47A=t;struct Cyc_Absyn_ArrayInfo _tmp47B;void*_tmp47C;struct Cyc_Absyn_Tqual
_tmp47D;struct Cyc_Absyn_Exp*_tmp47E;struct Cyc_Absyn_Conref*_tmp47F;_LL290: if(
_tmp47A <= (void*)4)goto _LL292;if(*((int*)_tmp47A)!= 7)goto _LL292;_tmp47B=((
struct Cyc_Absyn_ArrayType_struct*)_tmp47A)->f1;_tmp47C=(void*)_tmp47B.elt_type;
_tmp47D=_tmp47B.tq;_tmp47E=_tmp47B.num_elts;_tmp47F=_tmp47B.zero_term;_LL291: {
void*_tmp481;struct _tuple6 _tmp480=Cyc_Tcutil_addressof_props(te,e);_tmp481=
_tmp480.f2;{void*b=_tmp47E == 0?(void*)((void*)0):(void*)({struct Cyc_Absyn_Upper_b_struct*
_tmp482=_cycalloc(sizeof(*_tmp482));_tmp482[0]=({struct Cyc_Absyn_Upper_b_struct
_tmp483;_tmp483.tag=0;_tmp483.f1=(struct Cyc_Absyn_Exp*)_tmp47E;_tmp483;});
_tmp482;});t=Cyc_Absyn_atb_typ(_tmp47C,_tmp481,_tmp47D,b,_tmp47F);Cyc_Tcutil_unchecked_cast(
te,e,t,(void*)3);return t;}}_LL292:;_LL293: return t;_LL28F:;}_LL286:;}}static void*
Cyc_Tcexp_tcExpNoPromote(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*
e){{void*_tmp484=(void*)e->r;struct Cyc_Absyn_Exp*_tmp485;_LL295: if(*((int*)
_tmp484)!= 13)goto _LL297;_tmp485=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmp484)->f1;_LL296: Cyc_Tcexp_tcExpNoInst(te,topt,_tmp485);(void*)(((struct Cyc_Core_Opt*)
_check_null(_tmp485->topt))->v=(void*)Cyc_Absyn_pointer_expand((void*)((struct
Cyc_Core_Opt*)_check_null(_tmp485->topt))->v,0));e->topt=_tmp485->topt;goto
_LL294;_LL297:;_LL298: Cyc_Tcexp_tcExpNoInst(te,topt,e);(void*)(((struct Cyc_Core_Opt*)
_check_null(e->topt))->v=(void*)Cyc_Absyn_pointer_expand((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,0));{void*_tmp486=Cyc_Tcutil_compress((void*)((struct
Cyc_Core_Opt*)_check_null(e->topt))->v);struct Cyc_Absyn_PtrInfo _tmp487;void*
_tmp488;struct Cyc_Absyn_Tqual _tmp489;struct Cyc_Absyn_PtrAtts _tmp48A;void*_tmp48B;
struct Cyc_Absyn_Conref*_tmp48C;struct Cyc_Absyn_Conref*_tmp48D;struct Cyc_Absyn_Conref*
_tmp48E;_LL29A: if(_tmp486 <= (void*)4)goto _LL29C;if(*((int*)_tmp486)!= 4)goto
_LL29C;_tmp487=((struct Cyc_Absyn_PointerType_struct*)_tmp486)->f1;_tmp488=(void*)
_tmp487.elt_typ;_tmp489=_tmp487.elt_tq;_tmp48A=_tmp487.ptr_atts;_tmp48B=(void*)
_tmp48A.rgn;_tmp48C=_tmp48A.nullable;_tmp48D=_tmp48A.bounds;_tmp48E=_tmp48A.zero_term;
_LL29B:{void*_tmp48F=Cyc_Tcutil_compress(_tmp488);struct Cyc_Absyn_FnInfo _tmp490;
struct Cyc_List_List*_tmp491;struct Cyc_Core_Opt*_tmp492;void*_tmp493;struct Cyc_List_List*
_tmp494;int _tmp495;struct Cyc_Absyn_VarargInfo*_tmp496;struct Cyc_List_List*
_tmp497;struct Cyc_List_List*_tmp498;_LL29F: if(_tmp48F <= (void*)4)goto _LL2A1;if(*((
int*)_tmp48F)!= 8)goto _LL2A1;_tmp490=((struct Cyc_Absyn_FnType_struct*)_tmp48F)->f1;
_tmp491=_tmp490.tvars;_tmp492=_tmp490.effect;_tmp493=(void*)_tmp490.ret_typ;
_tmp494=_tmp490.args;_tmp495=_tmp490.c_varargs;_tmp496=_tmp490.cyc_varargs;
_tmp497=_tmp490.rgn_po;_tmp498=_tmp490.attributes;_LL2A0: if(_tmp491 != 0){struct
_RegionHandle*_tmp499=Cyc_Tcenv_get_fnrgn(te);struct _tuple4 _tmp49A=({struct
_tuple4 _tmp4A7;_tmp4A7.f1=Cyc_Tcenv_lookup_type_vars(te);_tmp4A7.f2=_tmp499;
_tmp4A7;});struct Cyc_List_List*inst=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _tuple5*(*f)(struct _tuple4*,struct Cyc_Absyn_Tvar*),struct
_tuple4*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(_tmp499,Cyc_Tcutil_r_make_inst_var,&
_tmp49A,_tmp491);struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(void*(*f)(
struct _tuple5*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple5*))
Cyc_Core_snd,inst);void*ftyp=Cyc_Tcutil_rsubstitute(_tmp499,inst,(void*)({struct
Cyc_Absyn_FnType_struct*_tmp4A4=_cycalloc(sizeof(*_tmp4A4));_tmp4A4[0]=({struct
Cyc_Absyn_FnType_struct _tmp4A5;_tmp4A5.tag=8;_tmp4A5.f1=({struct Cyc_Absyn_FnInfo
_tmp4A6;_tmp4A6.tvars=0;_tmp4A6.effect=_tmp492;_tmp4A6.ret_typ=(void*)_tmp493;
_tmp4A6.args=_tmp494;_tmp4A6.c_varargs=_tmp495;_tmp4A6.cyc_varargs=_tmp496;
_tmp4A6.rgn_po=_tmp497;_tmp4A6.attributes=_tmp498;_tmp4A6;});_tmp4A5;});_tmp4A4;}));
struct Cyc_Absyn_PointerType_struct*_tmp49B=({struct Cyc_Absyn_PointerType_struct*
_tmp4A0=_cycalloc(sizeof(*_tmp4A0));_tmp4A0[0]=({struct Cyc_Absyn_PointerType_struct
_tmp4A1;_tmp4A1.tag=4;_tmp4A1.f1=({struct Cyc_Absyn_PtrInfo _tmp4A2;_tmp4A2.elt_typ=(
void*)ftyp;_tmp4A2.elt_tq=_tmp489;_tmp4A2.ptr_atts=({struct Cyc_Absyn_PtrAtts
_tmp4A3;_tmp4A3.rgn=(void*)_tmp48B;_tmp4A3.nullable=_tmp48C;_tmp4A3.bounds=
_tmp48D;_tmp4A3.zero_term=_tmp48E;_tmp4A3.ptrloc=0;_tmp4A3;});_tmp4A2;});_tmp4A1;});
_tmp4A0;});struct Cyc_Absyn_Exp*_tmp49C=Cyc_Absyn_copy_exp(e);(void*)(e->r=(void*)((
void*)({struct Cyc_Absyn_Instantiate_e_struct*_tmp49D=_cycalloc(sizeof(*_tmp49D));
_tmp49D[0]=({struct Cyc_Absyn_Instantiate_e_struct _tmp49E;_tmp49E.tag=14;_tmp49E.f1=
_tmp49C;_tmp49E.f2=ts;_tmp49E;});_tmp49D;})));e->topt=({struct Cyc_Core_Opt*
_tmp49F=_cycalloc(sizeof(*_tmp49F));_tmp49F->v=(void*)((void*)_tmp49B);_tmp49F;});}
goto _LL29E;_LL2A1:;_LL2A2: goto _LL29E;_LL29E:;}goto _LL299;_LL29C:;_LL29D: goto
_LL299;_LL299:;}goto _LL294;_LL294:;}return(void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v;}static void Cyc_Tcexp_tcExpNoInst(struct Cyc_Tcenv_Tenv*
te,void**topt,struct Cyc_Absyn_Exp*e){struct Cyc_Position_Segment*loc=e->loc;void*
t;{void*_tmp4A8=(void*)e->r;struct Cyc_Absyn_Exp*_tmp4A9;struct _tuple1*_tmp4AA;
struct Cyc_Absyn_Exp*_tmp4AB;struct Cyc_List_List*_tmp4AC;struct Cyc_Core_Opt*
_tmp4AD;struct Cyc_List_List*_tmp4AE;union Cyc_Absyn_Cnst_union _tmp4AF;union Cyc_Absyn_Cnst_union*
_tmp4B0;struct _tuple1*_tmp4B1;void*_tmp4B2;void*_tmp4B3;struct Cyc_List_List*
_tmp4B4;struct Cyc_Absyn_Exp*_tmp4B5;void*_tmp4B6;struct Cyc_Absyn_Exp*_tmp4B7;
struct Cyc_Core_Opt*_tmp4B8;struct Cyc_Absyn_Exp*_tmp4B9;struct Cyc_Absyn_Exp*
_tmp4BA;struct Cyc_Absyn_Exp*_tmp4BB;struct Cyc_Absyn_Exp*_tmp4BC;struct Cyc_Absyn_Exp*
_tmp4BD;struct Cyc_Absyn_Exp*_tmp4BE;struct Cyc_Absyn_Exp*_tmp4BF;struct Cyc_Absyn_Exp*
_tmp4C0;struct Cyc_Absyn_Exp*_tmp4C1;struct Cyc_Absyn_Exp*_tmp4C2;struct Cyc_Absyn_Exp*
_tmp4C3;struct Cyc_List_List*_tmp4C4;struct Cyc_Absyn_VarargCallInfo*_tmp4C5;
struct Cyc_Absyn_VarargCallInfo**_tmp4C6;struct Cyc_Absyn_Exp*_tmp4C7;struct Cyc_Absyn_Exp*
_tmp4C8;struct Cyc_List_List*_tmp4C9;void*_tmp4CA;struct Cyc_Absyn_Exp*_tmp4CB;
void*_tmp4CC;void**_tmp4CD;struct Cyc_Absyn_Exp*_tmp4CE;struct Cyc_Absyn_Exp*
_tmp4CF;struct Cyc_Absyn_Exp*_tmp4D0;struct Cyc_Absyn_Exp*_tmp4D1;void*_tmp4D2;
void*_tmp4D3;void*_tmp4D4;struct Cyc_Absyn_Exp*_tmp4D5;struct Cyc_Absyn_Exp*
_tmp4D6;struct _dyneither_ptr*_tmp4D7;struct Cyc_Absyn_Exp*_tmp4D8;struct
_dyneither_ptr*_tmp4D9;struct Cyc_Absyn_Exp*_tmp4DA;struct Cyc_Absyn_Exp*_tmp4DB;
struct Cyc_List_List*_tmp4DC;struct _tuple2*_tmp4DD;struct Cyc_List_List*_tmp4DE;
struct Cyc_List_List*_tmp4DF;struct Cyc_Absyn_Stmt*_tmp4E0;struct Cyc_Absyn_Vardecl*
_tmp4E1;struct Cyc_Absyn_Exp*_tmp4E2;struct Cyc_Absyn_Exp*_tmp4E3;int _tmp4E4;int*
_tmp4E5;struct _tuple1*_tmp4E6;struct _tuple1**_tmp4E7;struct Cyc_List_List*_tmp4E8;
struct Cyc_List_List**_tmp4E9;struct Cyc_List_List*_tmp4EA;struct Cyc_Absyn_Aggrdecl*
_tmp4EB;struct Cyc_Absyn_Aggrdecl**_tmp4EC;void*_tmp4ED;struct Cyc_List_List*
_tmp4EE;struct Cyc_List_List*_tmp4EF;struct Cyc_Absyn_Datatypedecl*_tmp4F0;struct
Cyc_Absyn_Datatypefield*_tmp4F1;struct _tuple1*_tmp4F2;struct _tuple1**_tmp4F3;
struct Cyc_Absyn_Enumdecl*_tmp4F4;struct Cyc_Absyn_Enumfield*_tmp4F5;struct _tuple1*
_tmp4F6;struct _tuple1**_tmp4F7;void*_tmp4F8;struct Cyc_Absyn_Enumfield*_tmp4F9;
struct Cyc_Absyn_MallocInfo _tmp4FA;int _tmp4FB;int*_tmp4FC;struct Cyc_Absyn_Exp*
_tmp4FD;void**_tmp4FE;void***_tmp4FF;struct Cyc_Absyn_Exp*_tmp500;struct Cyc_Absyn_Exp**
_tmp501;int _tmp502;int*_tmp503;struct Cyc_Absyn_Exp*_tmp504;struct Cyc_Absyn_Exp*
_tmp505;void*_tmp506;_LL2A4: if(*((int*)_tmp4A8)!= 13)goto _LL2A6;_tmp4A9=((struct
Cyc_Absyn_NoInstantiate_e_struct*)_tmp4A8)->f1;_LL2A5: Cyc_Tcexp_tcExpNoInst(te,0,
_tmp4A9);return;_LL2A6: if(*((int*)_tmp4A8)!= 2)goto _LL2A8;_tmp4AA=((struct Cyc_Absyn_UnknownId_e_struct*)
_tmp4A8)->f1;_LL2A7: Cyc_Tcexp_resolve_unknown_id(te,e,_tmp4AA);Cyc_Tcexp_tcExpNoInst(
te,topt,e);return;_LL2A8: if(*((int*)_tmp4A8)!= 10)goto _LL2AA;_tmp4AB=((struct Cyc_Absyn_UnknownCall_e_struct*)
_tmp4A8)->f1;_tmp4AC=((struct Cyc_Absyn_UnknownCall_e_struct*)_tmp4A8)->f2;_LL2A9:
Cyc_Tcexp_resolve_unknown_fn(te,e,_tmp4AB,_tmp4AC);Cyc_Tcexp_tcExpNoInst(te,topt,
e);return;_LL2AA: if(*((int*)_tmp4A8)!= 37)goto _LL2AC;_tmp4AD=((struct Cyc_Absyn_UnresolvedMem_e_struct*)
_tmp4A8)->f1;_tmp4AE=((struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp4A8)->f2;
_LL2AB: Cyc_Tcexp_resolve_unresolved_mem(te,loc,topt,e,_tmp4AE);Cyc_Tcexp_tcExpNoInst(
te,topt,e);return;_LL2AC: if(*((int*)_tmp4A8)!= 0)goto _LL2AE;_tmp4AF=((struct Cyc_Absyn_Const_e_struct*)
_tmp4A8)->f1;_tmp4B0=(union Cyc_Absyn_Cnst_union*)&((struct Cyc_Absyn_Const_e_struct*)
_tmp4A8)->f1;_LL2AD: t=Cyc_Tcexp_tcConst(te,loc,topt,(union Cyc_Absyn_Cnst_union*)
_tmp4B0,e);goto _LL2A3;_LL2AE: if(*((int*)_tmp4A8)!= 1)goto _LL2B0;_tmp4B1=((struct
Cyc_Absyn_Var_e_struct*)_tmp4A8)->f1;_tmp4B2=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp4A8)->f2;_LL2AF: t=Cyc_Tcexp_tcVar(te,loc,_tmp4B1,_tmp4B2);goto _LL2A3;_LL2B0:
if(*((int*)_tmp4A8)!= 3)goto _LL2B2;_tmp4B3=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp4A8)->f1;_tmp4B4=((struct Cyc_Absyn_Primop_e_struct*)_tmp4A8)->f2;_LL2B1: t=
Cyc_Tcexp_tcPrimop(te,loc,topt,_tmp4B3,_tmp4B4);goto _LL2A3;_LL2B2: if(*((int*)
_tmp4A8)!= 5)goto _LL2B4;_tmp4B5=((struct Cyc_Absyn_Increment_e_struct*)_tmp4A8)->f1;
_tmp4B6=(void*)((struct Cyc_Absyn_Increment_e_struct*)_tmp4A8)->f2;_LL2B3: t=Cyc_Tcexp_tcIncrement(
te,loc,topt,_tmp4B5,_tmp4B6);goto _LL2A3;_LL2B4: if(*((int*)_tmp4A8)!= 4)goto
_LL2B6;_tmp4B7=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp4A8)->f1;_tmp4B8=((
struct Cyc_Absyn_AssignOp_e_struct*)_tmp4A8)->f2;_tmp4B9=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmp4A8)->f3;_LL2B5: t=Cyc_Tcexp_tcAssignOp(te,loc,topt,_tmp4B7,_tmp4B8,_tmp4B9);
goto _LL2A3;_LL2B6: if(*((int*)_tmp4A8)!= 6)goto _LL2B8;_tmp4BA=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp4A8)->f1;_tmp4BB=((struct Cyc_Absyn_Conditional_e_struct*)_tmp4A8)->f2;
_tmp4BC=((struct Cyc_Absyn_Conditional_e_struct*)_tmp4A8)->f3;_LL2B7: t=Cyc_Tcexp_tcConditional(
te,loc,topt,_tmp4BA,_tmp4BB,_tmp4BC);goto _LL2A3;_LL2B8: if(*((int*)_tmp4A8)!= 7)
goto _LL2BA;_tmp4BD=((struct Cyc_Absyn_And_e_struct*)_tmp4A8)->f1;_tmp4BE=((struct
Cyc_Absyn_And_e_struct*)_tmp4A8)->f2;_LL2B9: t=Cyc_Tcexp_tcAnd(te,loc,_tmp4BD,
_tmp4BE);goto _LL2A3;_LL2BA: if(*((int*)_tmp4A8)!= 8)goto _LL2BC;_tmp4BF=((struct
Cyc_Absyn_Or_e_struct*)_tmp4A8)->f1;_tmp4C0=((struct Cyc_Absyn_Or_e_struct*)
_tmp4A8)->f2;_LL2BB: t=Cyc_Tcexp_tcOr(te,loc,_tmp4BF,_tmp4C0);goto _LL2A3;_LL2BC:
if(*((int*)_tmp4A8)!= 9)goto _LL2BE;_tmp4C1=((struct Cyc_Absyn_SeqExp_e_struct*)
_tmp4A8)->f1;_tmp4C2=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp4A8)->f2;_LL2BD: t=
Cyc_Tcexp_tcSeqExp(te,loc,topt,_tmp4C1,_tmp4C2);goto _LL2A3;_LL2BE: if(*((int*)
_tmp4A8)!= 11)goto _LL2C0;_tmp4C3=((struct Cyc_Absyn_FnCall_e_struct*)_tmp4A8)->f1;
_tmp4C4=((struct Cyc_Absyn_FnCall_e_struct*)_tmp4A8)->f2;_tmp4C5=((struct Cyc_Absyn_FnCall_e_struct*)
_tmp4A8)->f3;_tmp4C6=(struct Cyc_Absyn_VarargCallInfo**)&((struct Cyc_Absyn_FnCall_e_struct*)
_tmp4A8)->f3;_LL2BF: t=Cyc_Tcexp_tcFnCall(te,loc,topt,_tmp4C3,_tmp4C4,_tmp4C6);
goto _LL2A3;_LL2C0: if(*((int*)_tmp4A8)!= 12)goto _LL2C2;_tmp4C7=((struct Cyc_Absyn_Throw_e_struct*)
_tmp4A8)->f1;_LL2C1: t=Cyc_Tcexp_tcThrow(te,loc,topt,_tmp4C7);goto _LL2A3;_LL2C2:
if(*((int*)_tmp4A8)!= 14)goto _LL2C4;_tmp4C8=((struct Cyc_Absyn_Instantiate_e_struct*)
_tmp4A8)->f1;_tmp4C9=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp4A8)->f2;_LL2C3:
t=Cyc_Tcexp_tcInstantiate(te,loc,topt,_tmp4C8,_tmp4C9);goto _LL2A3;_LL2C4: if(*((
int*)_tmp4A8)!= 15)goto _LL2C6;_tmp4CA=(void*)((struct Cyc_Absyn_Cast_e_struct*)
_tmp4A8)->f1;_tmp4CB=((struct Cyc_Absyn_Cast_e_struct*)_tmp4A8)->f2;_tmp4CC=(void*)((
struct Cyc_Absyn_Cast_e_struct*)_tmp4A8)->f4;_tmp4CD=(void**)&((void*)((struct Cyc_Absyn_Cast_e_struct*)
_tmp4A8)->f4);_LL2C5: t=Cyc_Tcexp_tcCast(te,loc,topt,_tmp4CA,_tmp4CB,(void**)
_tmp4CD);goto _LL2A3;_LL2C6: if(*((int*)_tmp4A8)!= 16)goto _LL2C8;_tmp4CE=((struct
Cyc_Absyn_Address_e_struct*)_tmp4A8)->f1;_LL2C7: t=Cyc_Tcexp_tcAddress(te,loc,e,
topt,_tmp4CE);goto _LL2A3;_LL2C8: if(*((int*)_tmp4A8)!= 17)goto _LL2CA;_tmp4CF=((
struct Cyc_Absyn_New_e_struct*)_tmp4A8)->f1;_tmp4D0=((struct Cyc_Absyn_New_e_struct*)
_tmp4A8)->f2;_LL2C9: t=Cyc_Tcexp_tcNew(te,loc,topt,_tmp4CF,e,_tmp4D0);goto _LL2A3;
_LL2CA: if(*((int*)_tmp4A8)!= 19)goto _LL2CC;_tmp4D1=((struct Cyc_Absyn_Sizeofexp_e_struct*)
_tmp4A8)->f1;_LL2CB: {void*_tmp507=Cyc_Tcexp_tcExpNoPromote(te,0,_tmp4D1);t=Cyc_Tcexp_tcSizeof(
te,loc,topt,_tmp507);goto _LL2A3;}_LL2CC: if(*((int*)_tmp4A8)!= 18)goto _LL2CE;
_tmp4D2=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp4A8)->f1;_LL2CD: t=Cyc_Tcexp_tcSizeof(
te,loc,topt,_tmp4D2);goto _LL2A3;_LL2CE: if(*((int*)_tmp4A8)!= 20)goto _LL2D0;
_tmp4D3=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp4A8)->f1;_tmp4D4=(void*)((
struct Cyc_Absyn_Offsetof_e_struct*)_tmp4A8)->f2;_LL2CF: t=Cyc_Tcexp_tcOffsetof(te,
loc,topt,_tmp4D3,_tmp4D4);goto _LL2A3;_LL2D0: if(*((int*)_tmp4A8)!= 21)goto _LL2D2;
_LL2D1:({void*_tmp508=0;Cyc_Tcutil_terr(loc,({const char*_tmp509="gen() not in top-level initializer";
_tag_dyneither(_tmp509,sizeof(char),35);}),_tag_dyneither(_tmp508,sizeof(void*),
0));});return;_LL2D2: if(*((int*)_tmp4A8)!= 22)goto _LL2D4;_tmp4D5=((struct Cyc_Absyn_Deref_e_struct*)
_tmp4A8)->f1;_LL2D3: t=Cyc_Tcexp_tcDeref(te,loc,topt,_tmp4D5);goto _LL2A3;_LL2D4:
if(*((int*)_tmp4A8)!= 23)goto _LL2D6;_tmp4D6=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp4A8)->f1;_tmp4D7=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp4A8)->f2;_LL2D5:
t=Cyc_Tcexp_tcAggrMember(te,loc,topt,e,_tmp4D6,_tmp4D7);goto _LL2A3;_LL2D6: if(*((
int*)_tmp4A8)!= 24)goto _LL2D8;_tmp4D8=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp4A8)->f1;_tmp4D9=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp4A8)->f2;_LL2D7: t=
Cyc_Tcexp_tcAggrArrow(te,loc,topt,_tmp4D8,_tmp4D9);goto _LL2A3;_LL2D8: if(*((int*)
_tmp4A8)!= 25)goto _LL2DA;_tmp4DA=((struct Cyc_Absyn_Subscript_e_struct*)_tmp4A8)->f1;
_tmp4DB=((struct Cyc_Absyn_Subscript_e_struct*)_tmp4A8)->f2;_LL2D9: t=Cyc_Tcexp_tcSubscript(
te,loc,topt,_tmp4DA,_tmp4DB);goto _LL2A3;_LL2DA: if(*((int*)_tmp4A8)!= 26)goto
_LL2DC;_tmp4DC=((struct Cyc_Absyn_Tuple_e_struct*)_tmp4A8)->f1;_LL2DB: t=Cyc_Tcexp_tcTuple(
te,loc,topt,_tmp4DC);goto _LL2A3;_LL2DC: if(*((int*)_tmp4A8)!= 27)goto _LL2DE;
_tmp4DD=((struct Cyc_Absyn_CompoundLit_e_struct*)_tmp4A8)->f1;_tmp4DE=((struct Cyc_Absyn_CompoundLit_e_struct*)
_tmp4A8)->f2;_LL2DD: t=Cyc_Tcexp_tcCompoundLit(te,loc,topt,_tmp4DD,_tmp4DE);goto
_LL2A3;_LL2DE: if(*((int*)_tmp4A8)!= 28)goto _LL2E0;_tmp4DF=((struct Cyc_Absyn_Array_e_struct*)
_tmp4A8)->f1;_LL2DF: {void**elt_topt=0;int zero_term=0;if(topt != 0){void*_tmp50A=
Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_ArrayInfo _tmp50B;void*_tmp50C;void**
_tmp50D;struct Cyc_Absyn_Conref*_tmp50E;_LL2F5: if(_tmp50A <= (void*)4)goto _LL2F7;
if(*((int*)_tmp50A)!= 7)goto _LL2F7;_tmp50B=((struct Cyc_Absyn_ArrayType_struct*)
_tmp50A)->f1;_tmp50C=(void*)_tmp50B.elt_type;_tmp50D=(void**)&(((struct Cyc_Absyn_ArrayType_struct*)
_tmp50A)->f1).elt_type;_tmp50E=_tmp50B.zero_term;_LL2F6: elt_topt=(void**)_tmp50D;
zero_term=((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp50E);
goto _LL2F4;_LL2F7:;_LL2F8: goto _LL2F4;_LL2F4:;}t=Cyc_Tcexp_tcArray(te,loc,
elt_topt,zero_term,_tmp4DF);goto _LL2A3;}_LL2E0: if(*((int*)_tmp4A8)!= 38)goto
_LL2E2;_tmp4E0=((struct Cyc_Absyn_StmtExp_e_struct*)_tmp4A8)->f1;_LL2E1: t=Cyc_Tcexp_tcStmtExp(
te,loc,topt,_tmp4E0);goto _LL2A3;_LL2E2: if(*((int*)_tmp4A8)!= 29)goto _LL2E4;
_tmp4E1=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp4A8)->f1;_tmp4E2=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmp4A8)->f2;_tmp4E3=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp4A8)->f3;_tmp4E4=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp4A8)->f4;
_tmp4E5=(int*)&((struct Cyc_Absyn_Comprehension_e_struct*)_tmp4A8)->f4;_LL2E3: t=
Cyc_Tcexp_tcComprehension(te,loc,topt,_tmp4E1,_tmp4E2,_tmp4E3,_tmp4E5);goto
_LL2A3;_LL2E4: if(*((int*)_tmp4A8)!= 30)goto _LL2E6;_tmp4E6=((struct Cyc_Absyn_Struct_e_struct*)
_tmp4A8)->f1;_tmp4E7=(struct _tuple1**)&((struct Cyc_Absyn_Struct_e_struct*)
_tmp4A8)->f1;_tmp4E8=((struct Cyc_Absyn_Struct_e_struct*)_tmp4A8)->f2;_tmp4E9=(
struct Cyc_List_List**)&((struct Cyc_Absyn_Struct_e_struct*)_tmp4A8)->f2;_tmp4EA=((
struct Cyc_Absyn_Struct_e_struct*)_tmp4A8)->f3;_tmp4EB=((struct Cyc_Absyn_Struct_e_struct*)
_tmp4A8)->f4;_tmp4EC=(struct Cyc_Absyn_Aggrdecl**)&((struct Cyc_Absyn_Struct_e_struct*)
_tmp4A8)->f4;_LL2E5: t=Cyc_Tcexp_tcStruct(te,loc,topt,_tmp4E7,_tmp4E9,_tmp4EA,
_tmp4EC);goto _LL2A3;_LL2E6: if(*((int*)_tmp4A8)!= 31)goto _LL2E8;_tmp4ED=(void*)((
struct Cyc_Absyn_AnonStruct_e_struct*)_tmp4A8)->f1;_tmp4EE=((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp4A8)->f2;_LL2E7: t=Cyc_Tcexp_tcAnonStruct(te,loc,_tmp4ED,_tmp4EE);goto _LL2A3;
_LL2E8: if(*((int*)_tmp4A8)!= 32)goto _LL2EA;_tmp4EF=((struct Cyc_Absyn_Datatype_e_struct*)
_tmp4A8)->f1;_tmp4F0=((struct Cyc_Absyn_Datatype_e_struct*)_tmp4A8)->f2;_tmp4F1=((
struct Cyc_Absyn_Datatype_e_struct*)_tmp4A8)->f3;_LL2E9: t=Cyc_Tcexp_tcDatatype(te,
loc,topt,e,_tmp4EF,_tmp4F0,_tmp4F1);goto _LL2A3;_LL2EA: if(*((int*)_tmp4A8)!= 33)
goto _LL2EC;_tmp4F2=((struct Cyc_Absyn_Enum_e_struct*)_tmp4A8)->f1;_tmp4F3=(struct
_tuple1**)&((struct Cyc_Absyn_Enum_e_struct*)_tmp4A8)->f1;_tmp4F4=((struct Cyc_Absyn_Enum_e_struct*)
_tmp4A8)->f2;_tmp4F5=((struct Cyc_Absyn_Enum_e_struct*)_tmp4A8)->f3;_LL2EB:*
_tmp4F3=((struct Cyc_Absyn_Enumfield*)_check_null(_tmp4F5))->name;t=(void*)({
struct Cyc_Absyn_EnumType_struct*_tmp50F=_cycalloc(sizeof(*_tmp50F));_tmp50F[0]=({
struct Cyc_Absyn_EnumType_struct _tmp510;_tmp510.tag=12;_tmp510.f1=((struct Cyc_Absyn_Enumdecl*)
_check_null(_tmp4F4))->name;_tmp510.f2=_tmp4F4;_tmp510;});_tmp50F;});goto _LL2A3;
_LL2EC: if(*((int*)_tmp4A8)!= 34)goto _LL2EE;_tmp4F6=((struct Cyc_Absyn_AnonEnum_e_struct*)
_tmp4A8)->f1;_tmp4F7=(struct _tuple1**)&((struct Cyc_Absyn_AnonEnum_e_struct*)
_tmp4A8)->f1;_tmp4F8=(void*)((struct Cyc_Absyn_AnonEnum_e_struct*)_tmp4A8)->f2;
_tmp4F9=((struct Cyc_Absyn_AnonEnum_e_struct*)_tmp4A8)->f3;_LL2ED:*_tmp4F7=((
struct Cyc_Absyn_Enumfield*)_check_null(_tmp4F9))->name;t=_tmp4F8;goto _LL2A3;
_LL2EE: if(*((int*)_tmp4A8)!= 35)goto _LL2F0;_tmp4FA=((struct Cyc_Absyn_Malloc_e_struct*)
_tmp4A8)->f1;_tmp4FB=_tmp4FA.is_calloc;_tmp4FC=(int*)&(((struct Cyc_Absyn_Malloc_e_struct*)
_tmp4A8)->f1).is_calloc;_tmp4FD=_tmp4FA.rgn;_tmp4FE=_tmp4FA.elt_type;_tmp4FF=(
void***)&(((struct Cyc_Absyn_Malloc_e_struct*)_tmp4A8)->f1).elt_type;_tmp500=
_tmp4FA.num_elts;_tmp501=(struct Cyc_Absyn_Exp**)&(((struct Cyc_Absyn_Malloc_e_struct*)
_tmp4A8)->f1).num_elts;_tmp502=_tmp4FA.fat_result;_tmp503=(int*)&(((struct Cyc_Absyn_Malloc_e_struct*)
_tmp4A8)->f1).fat_result;_LL2EF: t=Cyc_Tcexp_tcMalloc(te,loc,topt,_tmp4FD,_tmp4FF,
_tmp501,_tmp4FC,_tmp503);goto _LL2A3;_LL2F0: if(*((int*)_tmp4A8)!= 36)goto _LL2F2;
_tmp504=((struct Cyc_Absyn_Swap_e_struct*)_tmp4A8)->f1;_tmp505=((struct Cyc_Absyn_Swap_e_struct*)
_tmp4A8)->f2;_LL2F1: t=Cyc_Tcexp_tcSwap(te,loc,topt,_tmp504,_tmp505);goto _LL2A3;
_LL2F2: if(*((int*)_tmp4A8)!= 39)goto _LL2A3;_tmp506=(void*)((struct Cyc_Absyn_Valueof_e_struct*)
_tmp4A8)->f1;_LL2F3: if(!te->allow_valueof)({void*_tmp511=0;Cyc_Tcutil_terr(e->loc,({
const char*_tmp512="valueof(-) can only occur within types";_tag_dyneither(
_tmp512,sizeof(char),39);}),_tag_dyneither(_tmp511,sizeof(void*),0));});t=Cyc_Absyn_sint_typ;
goto _LL2A3;_LL2A3:;}e->topt=({struct Cyc_Core_Opt*_tmp513=_cycalloc(sizeof(*
_tmp513));_tmp513->v=(void*)t;_tmp513;});}
