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
char Cyc_Core_Open_Region[16];extern char Cyc_Core_Free_Region[16];struct Cyc_List_List{
void*hd;struct Cyc_List_List*tl;};struct Cyc_List_List*Cyc_List_list(struct
_dyneither_ptr);int Cyc_List_length(struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rmap(
struct _RegionHandle*,void*(*f)(void*),struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_rmap_c(struct _RegionHandle*,void*(*f)(void*,void*),void*env,struct Cyc_List_List*
x);extern char Cyc_List_List_mismatch[18];struct Cyc_List_List*Cyc_List_rrev(struct
_RegionHandle*,struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_imp_rev(
struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,
struct Cyc_List_List*y);struct Cyc_List_List*Cyc_List_rappend(struct _RegionHandle*,
struct Cyc_List_List*x,struct Cyc_List_List*y);struct Cyc_List_List*Cyc_List_imp_append(
struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[8];struct Cyc_List_List*
Cyc_List_rzip(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y);struct Cyc_List_List*Cyc_List_rtabulate_c(struct
_RegionHandle*r,int n,void*(*f)(void*,int),void*env);struct Cyc_Iter_Iter{void*env;
int(*next)(void*env,void*dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);
struct Cyc___cycFILE;struct Cyc_Cstdio___abstractFILE;struct Cyc_String_pa_struct{
int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_struct{int tag;unsigned long f1;}
;struct Cyc_Double_pa_struct{int tag;double f1;};struct Cyc_LongDouble_pa_struct{int
tag;long double f1;};struct Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{
int tag;unsigned long*f1;};struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr,
struct _dyneither_ptr);struct Cyc_ShortPtr_sa_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_struct{
int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{
int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_struct{int tag;struct
_dyneither_ptr f1;};struct Cyc_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_struct{
int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int tag;struct _dyneither_ptr f1;};
extern char Cyc_FileCloseError[19];extern char Cyc_FileOpenError[18];struct Cyc_FileOpenError_struct{
char*tag;struct _dyneither_ptr f1;};struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(
void*,void*);struct _RegionHandle*r;struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[
12];extern char Cyc_Dict_Absent[11];struct _tuple0*Cyc_Dict_rchoose(struct
_RegionHandle*r,struct Cyc_Dict_Dict d);struct _tuple0*Cyc_Dict_rchoose(struct
_RegionHandle*,struct Cyc_Dict_Dict d);struct Cyc_Set_Set;struct Cyc_Set_Set*Cyc_Set_rempty(
struct _RegionHandle*r,int(*cmp)(void*,void*));struct Cyc_Set_Set*Cyc_Set_rinsert(
struct _RegionHandle*r,struct Cyc_Set_Set*s,void*elt);int Cyc_Set_cardinality(
struct Cyc_Set_Set*s);int Cyc_Set_is_empty(struct Cyc_Set_Set*s);int Cyc_Set_member(
struct Cyc_Set_Set*s,void*elt);extern char Cyc_Set_Absent[11];void*Cyc_Set_choose(
struct Cyc_Set_Set*s);int Cyc_strcmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2);
int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct _dyneither_ptr*s2);struct
_dyneither_ptr Cyc_strconcat_l(struct Cyc_List_List*);struct Cyc_Lineno_Pos{struct
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
_dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_struct{int tag;struct
Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_struct{
int tag;struct Cyc_List_List*f1;};struct _tuple2{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Tqual
f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_struct{int tag;struct _tuple2*f1;struct
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
f1;};struct Cyc_Absyn_Valueof_e_struct{int tag;void*f1;};struct Cyc_Absyn_Exp{
struct Cyc_Core_Opt*topt;void*r;struct Cyc_Position_Segment*loc;void*annot;};
struct Cyc_Absyn_Exp_s_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_struct{
int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_struct{int tag;
struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct
_tuple3{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_struct{
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
_dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(
struct Cyc_Position_Segment*);struct Cyc_Absyn_Conref*Cyc_Absyn_empty_conref();
void*Cyc_Absyn_conref_def(void*,struct Cyc_Absyn_Conref*x);extern struct Cyc_Absyn_Conref*
Cyc_Absyn_true_conref;extern struct Cyc_Absyn_Conref*Cyc_Absyn_false_conref;void*
Cyc_Absyn_compress_kb(void*);void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct
Cyc_Core_Opt*tenv);void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);extern void*Cyc_Absyn_char_typ;
extern void*Cyc_Absyn_uint_typ;extern void*Cyc_Absyn_sint_typ;extern void*Cyc_Absyn_float_typ;
extern void*Cyc_Absyn_empty_effect;struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*p,
struct Cyc_Position_Segment*s);struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(
struct _tuple1*x,void*t,struct Cyc_Absyn_Exp*init);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;
struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs: 1;int qvar_to_Cids: 1;
int add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int rewrite_temp_tvars: 1;int
print_all_tvars: 1;int print_all_kinds: 1;int print_all_effects: 1;int
print_using_stmts: 1;int print_externC_stmts: 1;int print_full_evars: 1;int
print_zeroterm: 1;int generate_line_directives: 1;int use_curr_namespace: 1;struct Cyc_List_List*
curr_namespace;};struct _dyneither_ptr Cyc_Absynpp_typ2string(void*);struct
_dyneither_ptr Cyc_Absynpp_kind2string(void*);struct _dyneither_ptr Cyc_Absynpp_kindbound2string(
void*);struct _dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);struct
_dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple1*);struct Cyc_RgnOrder_RgnPO;
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
Cyc_Tcenv_Fenv;struct Cyc_Tcenv_Stmt_j_struct{int tag;struct Cyc_Absyn_Stmt*f1;};
struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Dict_Dict ae;struct Cyc_Tcenv_Fenv*
le;int allow_valueof;};struct _RegionHandle*Cyc_Tcenv_get_fnrgn(struct Cyc_Tcenv_Tenv*);
void*Cyc_Tcenv_lookup_ordinary(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,struct
Cyc_Position_Segment*,struct _tuple1*);struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl(
struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,struct _tuple1*);struct Cyc_List_List*
Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*);struct Cyc_Core_Opt*Cyc_Tcenv_lookup_opt_type_vars(
struct Cyc_Tcenv_Tenv*te);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_type_vars(struct
_RegionHandle*,struct Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*);
void Cyc_Tcenv_check_rgn_accessible(struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,
void*rgn);void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
void Cyc_Tcutil_terr(struct Cyc_Position_Segment*,struct _dyneither_ptr fmt,struct
_dyneither_ptr ap);void Cyc_Tcutil_warn(struct Cyc_Position_Segment*,struct
_dyneither_ptr fmt,struct _dyneither_ptr ap);int Cyc_Tcutil_kind_leq(void*k1,void*k2);
void*Cyc_Tcutil_compress(void*t);int Cyc_Tcutil_coerceable(void*);extern struct Cyc_Core_Opt
Cyc_Tcutil_rk;extern struct Cyc_Core_Opt Cyc_Tcutil_trk;extern struct Cyc_Core_Opt Cyc_Tcutil_ak;
extern struct Cyc_Core_Opt Cyc_Tcutil_mk;int Cyc_Tcutil_unify(void*,void*);void*Cyc_Tcutil_rsubstitute(
struct _RegionHandle*,struct Cyc_List_List*,void*);struct _tuple4{struct Cyc_List_List*
f1;struct _RegionHandle*f2;};struct _tuple5{struct Cyc_Absyn_Tvar*f1;void*f2;};
struct _tuple5*Cyc_Tcutil_r_make_inst_var(struct _tuple4*,struct Cyc_Absyn_Tvar*);
void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*vs,struct Cyc_Position_Segment*
loc,struct _dyneither_ptr err_msg);struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(
struct _RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,
void*,struct Cyc_List_List*fields);int Cyc_Tcutil_is_noalias_pointer(void*t);int
Cyc_Tcutil_is_noalias_pointer_or_aggr(struct _RegionHandle*,void*t);struct Cyc_Absyn_Tvar*
Cyc_Tcutil_new_tvar(void*k);int Cyc_Tcutil_is_const_exp(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Absyn_Exp*e);void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*,void**,struct
Cyc_Absyn_Exp*);struct Cyc_Tcexp_TestEnv{struct _tuple0*eq;int isTrue;};struct Cyc_Tcexp_TestEnv
Cyc_Tcexp_tcTest(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct
_dyneither_ptr msg_part);struct _tuple6{struct Cyc_List_List*f1;struct Cyc_List_List*
f2;};struct Cyc_Tcpat_TcPatResult{struct _tuple6*tvars_and_bounds_opt;struct Cyc_List_List*
patvars;};struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_tcPat(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Absyn_Pat*p,void**topt);void Cyc_Tcpat_check_pat_regions(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Pat*p);void Cyc_Tcpat_check_switch_exhaustive(struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*);int Cyc_Tcpat_check_let_pat_exhaustive(
struct Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Pat*p);void Cyc_Tcpat_check_catch_overlap(
struct Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*);struct
_tuple7{unsigned int f1;int f2;};struct _tuple7 Cyc_Evexp_eval_const_uint_exp(struct
Cyc_Absyn_Exp*e);struct _tuple8{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};
static void Cyc_Tcpat_resolve_pat(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p){
void*_tmp0=(void*)p->r;struct _tuple1*_tmp1;struct _tuple1*_tmp2;struct Cyc_List_List*
_tmp3;int _tmp4;struct Cyc_Absyn_AggrInfo _tmp5;union Cyc_Absyn_AggrInfoU_union _tmp6;
void*_tmp7;struct _tuple1*_tmp8;struct Cyc_List_List*_tmp9;struct Cyc_List_List*
_tmpA;struct Cyc_List_List**_tmpB;struct Cyc_List_List*_tmpC;int _tmpD;struct Cyc_Absyn_Exp*
_tmpE;_LL1: if(_tmp0 <= (void*)2)goto _LL9;if(*((int*)_tmp0)!= 12)goto _LL3;_tmp1=((
struct Cyc_Absyn_UnknownId_p_struct*)_tmp0)->f1;_LL2:{struct _handler_cons _tmpF;
_push_handler(& _tmpF);{int _tmp11=0;if(setjmp(_tmpF.handler))_tmp11=1;if(!_tmp11){{
struct _RegionHandle*_tmp12=Cyc_Tcenv_get_fnrgn(te);void*_tmp13=Cyc_Tcenv_lookup_ordinary(
_tmp12,te,p->loc,_tmp1);struct Cyc_Absyn_Datatypedecl*_tmp14;struct Cyc_Absyn_Datatypefield*
_tmp15;struct Cyc_Absyn_Enumdecl*_tmp16;struct Cyc_Absyn_Enumfield*_tmp17;void*
_tmp18;struct Cyc_Absyn_Enumfield*_tmp19;_LLC: if(*((int*)_tmp13)!= 1)goto _LLE;
_LLD:(void*)(p->r=(void*)((void*)0));({void*_tmp1A=0;Cyc_Tcutil_terr(p->loc,({
const char*_tmp1B="struct tag used without arguments in pattern";_tag_dyneither(
_tmp1B,sizeof(char),45);}),_tag_dyneither(_tmp1A,sizeof(void*),0));});
_npop_handler(0);return;_LLE: if(*((int*)_tmp13)!= 2)goto _LL10;_tmp14=((struct Cyc_Tcenv_DatatypeRes_struct*)
_tmp13)->f1;_tmp15=((struct Cyc_Tcenv_DatatypeRes_struct*)_tmp13)->f2;_LLF:(void*)(
p->r=(void*)((void*)({struct Cyc_Absyn_Datatype_p_struct*_tmp1C=_cycalloc(sizeof(*
_tmp1C));_tmp1C[0]=({struct Cyc_Absyn_Datatype_p_struct _tmp1D;_tmp1D.tag=6;_tmp1D.f1=
_tmp14;_tmp1D.f2=_tmp15;_tmp1D.f3=0;_tmp1D.f4=0;_tmp1D;});_tmp1C;})));
_npop_handler(0);return;_LL10: if(*((int*)_tmp13)!= 3)goto _LL12;_tmp16=((struct
Cyc_Tcenv_EnumRes_struct*)_tmp13)->f1;_tmp17=((struct Cyc_Tcenv_EnumRes_struct*)
_tmp13)->f2;_LL11:(void*)(p->r=(void*)((void*)({struct Cyc_Absyn_Enum_p_struct*
_tmp1E=_cycalloc(sizeof(*_tmp1E));_tmp1E[0]=({struct Cyc_Absyn_Enum_p_struct
_tmp1F;_tmp1F.tag=10;_tmp1F.f1=_tmp16;_tmp1F.f2=_tmp17;_tmp1F;});_tmp1E;})));
_npop_handler(0);return;_LL12: if(*((int*)_tmp13)!= 4)goto _LL14;_tmp18=(void*)((
struct Cyc_Tcenv_AnonEnumRes_struct*)_tmp13)->f1;_tmp19=((struct Cyc_Tcenv_AnonEnumRes_struct*)
_tmp13)->f2;_LL13:(void*)(p->r=(void*)((void*)({struct Cyc_Absyn_AnonEnum_p_struct*
_tmp20=_cycalloc(sizeof(*_tmp20));_tmp20[0]=({struct Cyc_Absyn_AnonEnum_p_struct
_tmp21;_tmp21.tag=11;_tmp21.f1=(void*)_tmp18;_tmp21.f2=_tmp19;_tmp21;});_tmp20;})));
_npop_handler(0);return;_LL14: if(*((int*)_tmp13)!= 0)goto _LLB;_LL15: goto _LLB;
_LLB:;};_pop_handler();}else{void*_tmp10=(void*)_exn_thrown;void*_tmp23=_tmp10;
_LL17: if(_tmp23 != Cyc_Dict_Absent)goto _LL19;_LL18: goto _LL16;_LL19:;_LL1A:(void)
_throw(_tmp23);_LL16:;}}}{union Cyc_Absyn_Nmspace_union _tmp24=(*_tmp1).f1;struct
Cyc_List_List*_tmp25;_LL1C: if((_tmp24.Loc_n).tag != 0)goto _LL1E;_LL1D: goto _LL1F;
_LL1E: if((_tmp24.Rel_n).tag != 1)goto _LL20;_tmp25=(_tmp24.Rel_n).f1;if(_tmp25 != 0)
goto _LL20;_LL1F:(*_tmp1).f1=(union Cyc_Absyn_Nmspace_union)({union Cyc_Absyn_Nmspace_union
_tmp26;(_tmp26.Loc_n).tag=0;_tmp26;});(void*)(p->r=(void*)((void*)({struct Cyc_Absyn_Var_p_struct*
_tmp27=_cycalloc(sizeof(*_tmp27));_tmp27[0]=({struct Cyc_Absyn_Var_p_struct _tmp28;
_tmp28.tag=0;_tmp28.f1=Cyc_Absyn_new_vardecl(_tmp1,(void*)0,0);_tmp28.f2=Cyc_Absyn_new_pat((
void*)0,0);_tmp28;});_tmp27;})));return;_LL20:;_LL21:({void*_tmp29=0;Cyc_Tcutil_terr(
p->loc,({const char*_tmp2A="qualified variable in pattern";_tag_dyneither(_tmp2A,
sizeof(char),30);}),_tag_dyneither(_tmp29,sizeof(void*),0));});return;_LL1B:;}
_LL3: if(*((int*)_tmp0)!= 13)goto _LL5;_tmp2=((struct Cyc_Absyn_UnknownCall_p_struct*)
_tmp0)->f1;_tmp3=((struct Cyc_Absyn_UnknownCall_p_struct*)_tmp0)->f2;_tmp4=((
struct Cyc_Absyn_UnknownCall_p_struct*)_tmp0)->f3;_LL4:{struct _handler_cons _tmp2B;
_push_handler(& _tmp2B);{int _tmp2D=0;if(setjmp(_tmp2B.handler))_tmp2D=1;if(!
_tmp2D){{struct _RegionHandle*_tmp2E=Cyc_Tcenv_get_fnrgn(te);void*_tmp2F=Cyc_Tcenv_lookup_ordinary(
_tmp2E,te,p->loc,_tmp2);struct Cyc_Absyn_Aggrdecl*_tmp30;struct Cyc_Absyn_Datatypedecl*
_tmp31;struct Cyc_Absyn_Datatypefield*_tmp32;_LL23: if(*((int*)_tmp2F)!= 1)goto
_LL25;_tmp30=((struct Cyc_Tcenv_AggrRes_struct*)_tmp2F)->f1;_LL24: {struct Cyc_List_List*
_tmp33=0;for(0;_tmp3 != 0;_tmp3=_tmp3->tl){_tmp33=({struct Cyc_List_List*_tmp34=
_cycalloc(sizeof(*_tmp34));_tmp34->hd=({struct _tuple8*_tmp35=_cycalloc(sizeof(*
_tmp35));_tmp35->f1=0;_tmp35->f2=(struct Cyc_Absyn_Pat*)_tmp3->hd;_tmp35;});
_tmp34->tl=_tmp33;_tmp34;});}(void*)(p->r=(void*)((void*)({struct Cyc_Absyn_Aggr_p_struct*
_tmp36=_cycalloc(sizeof(*_tmp36));_tmp36[0]=({struct Cyc_Absyn_Aggr_p_struct
_tmp37;_tmp37.tag=5;_tmp37.f1=({struct Cyc_Absyn_AggrInfo _tmp38;_tmp38.aggr_info=(
union Cyc_Absyn_AggrInfoU_union)({union Cyc_Absyn_AggrInfoU_union _tmp39;(_tmp39.KnownAggr).tag=
1;(_tmp39.KnownAggr).f1=({struct Cyc_Absyn_Aggrdecl**_tmp3A=_cycalloc(sizeof(*
_tmp3A));_tmp3A[0]=_tmp30;_tmp3A;});_tmp39;});_tmp38.targs=0;_tmp38;});_tmp37.f2=
0;_tmp37.f3=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmp33);_tmp37.f4=_tmp4;_tmp37;});_tmp36;})));_npop_handler(0);return;}_LL25: if(*((
int*)_tmp2F)!= 2)goto _LL27;_tmp31=((struct Cyc_Tcenv_DatatypeRes_struct*)_tmp2F)->f1;
_tmp32=((struct Cyc_Tcenv_DatatypeRes_struct*)_tmp2F)->f2;_LL26:(void*)(p->r=(
void*)((void*)({struct Cyc_Absyn_Datatype_p_struct*_tmp3B=_cycalloc(sizeof(*
_tmp3B));_tmp3B[0]=({struct Cyc_Absyn_Datatype_p_struct _tmp3C;_tmp3C.tag=6;_tmp3C.f1=
_tmp31;_tmp3C.f2=_tmp32;_tmp3C.f3=_tmp3;_tmp3C.f4=_tmp4;_tmp3C;});_tmp3B;})));
_npop_handler(0);return;_LL27: if(*((int*)_tmp2F)!= 3)goto _LL29;_LL28: goto _LL2A;
_LL29: if(*((int*)_tmp2F)!= 4)goto _LL2B;_LL2A:({void*_tmp3D=0;Cyc_Tcutil_terr(p->loc,({
const char*_tmp3E="enum tag used with arguments in pattern";_tag_dyneither(_tmp3E,
sizeof(char),40);}),_tag_dyneither(_tmp3D,sizeof(void*),0));});(void*)(p->r=(
void*)((void*)0));_npop_handler(0);return;_LL2B: if(*((int*)_tmp2F)!= 0)goto _LL22;
_LL2C: goto _LL22;_LL22:;};_pop_handler();}else{void*_tmp2C=(void*)_exn_thrown;
void*_tmp40=_tmp2C;_LL2E: if(_tmp40 != Cyc_Dict_Absent)goto _LL30;_LL2F: goto _LL2D;
_LL30:;_LL31:(void)_throw(_tmp40);_LL2D:;}}}({struct Cyc_String_pa_struct _tmp43;
_tmp43.tag=0;_tmp43.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp2));{void*_tmp41[1]={& _tmp43};Cyc_Tcutil_terr(p->loc,({const char*_tmp42="%s is not a constructor in pattern";
_tag_dyneither(_tmp42,sizeof(char),35);}),_tag_dyneither(_tmp41,sizeof(void*),1));}});(
void*)(p->r=(void*)((void*)0));return;_LL5: if(*((int*)_tmp0)!= 5)goto _LL7;_tmp5=((
struct Cyc_Absyn_Aggr_p_struct*)_tmp0)->f1;_tmp6=_tmp5.aggr_info;if((((((struct
Cyc_Absyn_Aggr_p_struct*)_tmp0)->f1).aggr_info).UnknownAggr).tag != 0)goto _LL7;
_tmp7=(_tmp6.UnknownAggr).f1;_tmp8=(_tmp6.UnknownAggr).f2;_tmp9=_tmp5.targs;
_tmpA=((struct Cyc_Absyn_Aggr_p_struct*)_tmp0)->f2;_tmpB=(struct Cyc_List_List**)&((
struct Cyc_Absyn_Aggr_p_struct*)_tmp0)->f2;_tmpC=((struct Cyc_Absyn_Aggr_p_struct*)
_tmp0)->f3;_tmpD=((struct Cyc_Absyn_Aggr_p_struct*)_tmp0)->f4;_LL6:{struct
_handler_cons _tmp44;_push_handler(& _tmp44);{int _tmp46=0;if(setjmp(_tmp44.handler))
_tmp46=1;if(!_tmp46){{struct Cyc_Absyn_Aggrdecl**_tmp47=Cyc_Tcenv_lookup_aggrdecl(
te,p->loc,_tmp8);struct Cyc_Absyn_Aggrdecl*_tmp48=*_tmp47;if(_tmp48->impl == 0){({
struct Cyc_String_pa_struct _tmp4B;_tmp4B.tag=0;_tmp4B.f1=(struct _dyneither_ptr)((
void*)_tmp48->kind == (void*)0?({const char*_tmp4C="struct";_tag_dyneither(_tmp4C,
sizeof(char),7);}):({const char*_tmp4D="union";_tag_dyneither(_tmp4D,sizeof(char),
6);}));{void*_tmp49[1]={& _tmp4B};Cyc_Tcutil_terr(p->loc,({const char*_tmp4A="can't destructure an abstract %s";
_tag_dyneither(_tmp4A,sizeof(char),33);}),_tag_dyneither(_tmp49,sizeof(void*),1));}});(
void*)(p->r=(void*)((void*)0));_npop_handler(0);return;}{int more_exists=((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp48->impl))->exist_vars)- ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(*
_tmpB);if(more_exists < 0){({void*_tmp4E=0;Cyc_Tcutil_terr(p->loc,({const char*
_tmp4F="too many existentially bound type variables in pattern";_tag_dyneither(
_tmp4F,sizeof(char),55);}),_tag_dyneither(_tmp4E,sizeof(void*),0));});{struct Cyc_List_List**
_tmp50=_tmpB;{int n=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp48->impl))->exist_vars);for(0;n != 0;-- n){_tmp50=&((struct Cyc_List_List*)
_check_null(*_tmp50))->tl;}}*_tmp50=0;}}else{if(more_exists > 0){struct Cyc_List_List*
_tmp51=0;for(0;more_exists != 0;-- more_exists){_tmp51=({struct Cyc_List_List*
_tmp52=_cycalloc(sizeof(*_tmp52));_tmp52->hd=Cyc_Tcutil_new_tvar((void*)({struct
Cyc_Absyn_Unknown_kb_struct*_tmp53=_cycalloc(sizeof(*_tmp53));_tmp53[0]=({struct
Cyc_Absyn_Unknown_kb_struct _tmp54;_tmp54.tag=1;_tmp54.f1=0;_tmp54;});_tmp53;}));
_tmp52->tl=_tmp51;_tmp52;});}*_tmpB=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_imp_append)(*_tmpB,_tmp51);}}(void*)(p->r=(
void*)((void*)({struct Cyc_Absyn_Aggr_p_struct*_tmp55=_cycalloc(sizeof(*_tmp55));
_tmp55[0]=({struct Cyc_Absyn_Aggr_p_struct _tmp56;_tmp56.tag=5;_tmp56.f1=({struct
Cyc_Absyn_AggrInfo _tmp57;_tmp57.aggr_info=(union Cyc_Absyn_AggrInfoU_union)({
union Cyc_Absyn_AggrInfoU_union _tmp58;(_tmp58.KnownAggr).tag=1;(_tmp58.KnownAggr).f1=
_tmp47;_tmp58;});_tmp57.targs=_tmp9;_tmp57;});_tmp56.f2=*_tmpB;_tmp56.f3=_tmpC;
_tmp56.f4=_tmpD;_tmp56;});_tmp55;})));}};_pop_handler();}else{void*_tmp45=(void*)
_exn_thrown;void*_tmp5A=_tmp45;_LL33: if(_tmp5A != Cyc_Dict_Absent)goto _LL35;_LL34:({
void*_tmp5B=0;Cyc_Tcutil_terr(p->loc,({const char*_tmp5C="Non-aggregate name has designator patterns";
_tag_dyneither(_tmp5C,sizeof(char),43);}),_tag_dyneither(_tmp5B,sizeof(void*),0));});(
void*)(p->r=(void*)((void*)0));goto _LL32;_LL35:;_LL36:(void)_throw(_tmp5A);_LL32:;}}}
return;_LL7: if(*((int*)_tmp0)!= 14)goto _LL9;_tmpE=((struct Cyc_Absyn_Exp_p_struct*)
_tmp0)->f1;_LL8: Cyc_Tcexp_tcExp(te,0,_tmpE);if(!Cyc_Tcutil_is_const_exp(te,_tmpE)){({
void*_tmp5D=0;Cyc_Tcutil_terr(p->loc,({const char*_tmp5E="non-constant expression in case pattern";
_tag_dyneither(_tmp5E,sizeof(char),40);}),_tag_dyneither(_tmp5D,sizeof(void*),0));});(
void*)(p->r=(void*)((void*)0));}{unsigned int _tmp60;int _tmp61;struct _tuple7
_tmp5F=Cyc_Evexp_eval_const_uint_exp(_tmpE);_tmp60=_tmp5F.f1;_tmp61=_tmp5F.f2;(
void*)(p->r=(void*)((void*)({struct Cyc_Absyn_Int_p_struct*_tmp62=_cycalloc(
sizeof(*_tmp62));_tmp62[0]=({struct Cyc_Absyn_Int_p_struct _tmp63;_tmp63.tag=7;
_tmp63.f1=(void*)((void*)2);_tmp63.f2=(int)_tmp60;_tmp63;});_tmp62;})));return;}
_LL9:;_LLA: return;_LL0:;}static struct _dyneither_ptr*Cyc_Tcpat_get_name(struct Cyc_Absyn_Vardecl*
vd){return(*vd->name).f2;}static void*Cyc_Tcpat_any_type(struct Cyc_List_List*s,
void**topt){if(topt != 0)return*topt;return Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_mk,({struct Cyc_Core_Opt*_tmp64=_cycalloc(sizeof(*_tmp64));_tmp64->v=s;
_tmp64;}));}static void*Cyc_Tcpat_num_type(void**topt,void*numt){if(topt != 0  && 
Cyc_Tcutil_coerceable(*topt))return*topt;{void*_tmp65=Cyc_Tcutil_compress(numt);
_LL38: if(_tmp65 <= (void*)4)goto _LL3A;if(*((int*)_tmp65)!= 12)goto _LL3A;_LL39: if(
topt != 0)return*topt;goto _LL37;_LL3A:;_LL3B: goto _LL37;_LL37:;}return numt;}static
void Cyc_Tcpat_set_vd(struct Cyc_Absyn_Vardecl*vd,struct Cyc_List_List**
v_result_ptr,void*t){(void*)(vd->type=(void*)t);vd->tq=Cyc_Absyn_empty_tqual(0);*
v_result_ptr=(struct Cyc_List_List*)({struct Cyc_List_List*_tmp66=_cycalloc(
sizeof(*_tmp66));_tmp66->hd=vd;_tmp66->tl=*v_result_ptr;_tmp66;});}static struct
Cyc_Tcpat_TcPatResult Cyc_Tcpat_combine_results(struct Cyc_Tcpat_TcPatResult res1,
struct Cyc_Tcpat_TcPatResult res2){struct _tuple6*_tmp68;struct Cyc_List_List*_tmp69;
struct Cyc_Tcpat_TcPatResult _tmp67=res1;_tmp68=_tmp67.tvars_and_bounds_opt;_tmp69=
_tmp67.patvars;{struct _tuple6*_tmp6B;struct Cyc_List_List*_tmp6C;struct Cyc_Tcpat_TcPatResult
_tmp6A=res2;_tmp6B=_tmp6A.tvars_and_bounds_opt;_tmp6C=_tmp6A.patvars;if(_tmp68 != 
0  || _tmp6B != 0){if(_tmp68 == 0)_tmp68=({struct _tuple6*_tmp6D=_cycalloc(sizeof(*
_tmp6D));_tmp6D->f1=0;_tmp6D->f2=0;_tmp6D;});if(_tmp6B == 0)_tmp6B=({struct
_tuple6*_tmp6E=_cycalloc(sizeof(*_tmp6E));_tmp6E->f1=0;_tmp6E->f2=0;_tmp6E;});
return({struct Cyc_Tcpat_TcPatResult _tmp6F;_tmp6F.tvars_and_bounds_opt=({struct
_tuple6*_tmp70=_cycalloc(sizeof(*_tmp70));_tmp70->f1=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*((struct _tuple6*)
_check_null(_tmp68))).f1,(*((struct _tuple6*)_check_null(_tmp6B))).f1);_tmp70->f2=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*((
struct _tuple6*)_check_null(_tmp68))).f2,(*((struct _tuple6*)_check_null(_tmp6B))).f2);
_tmp70;});_tmp6F.patvars=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct
Cyc_List_List*y))Cyc_List_append)(_tmp69,_tmp6C);_tmp6F;});}return({struct Cyc_Tcpat_TcPatResult
_tmp71;_tmp71.tvars_and_bounds_opt=0;_tmp71.patvars=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp69,_tmp6C);
_tmp71;});}}static struct Cyc_Absyn_Pat*Cyc_Tcpat_wild_pat(struct Cyc_Position_Segment*
loc){return({struct Cyc_Absyn_Pat*_tmp72=_cycalloc(sizeof(*_tmp72));_tmp72->loc=
loc;_tmp72->topt=0;_tmp72->r=(void*)((void*)0);_tmp72;});}struct _tuple9{struct
Cyc_Absyn_Tqual f1;void*f2;};struct _tuple10{struct Cyc_Absyn_Aggrfield*f1;struct
Cyc_Absyn_Pat*f2;};static struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_tcPatRec(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Pat*p,void**topt,void**rgn_opt){Cyc_Tcpat_resolve_pat(te,p);{
void*t;struct Cyc_Tcpat_TcPatResult res=({struct Cyc_Tcpat_TcPatResult _tmp147;
_tmp147.tvars_and_bounds_opt=0;_tmp147.patvars=0;_tmp147;});{void*_tmp73=(void*)
p->r;struct Cyc_Absyn_Vardecl*_tmp74;struct Cyc_Absyn_Pat*_tmp75;struct Cyc_Absyn_Vardecl*
_tmp76;struct Cyc_Absyn_Pat*_tmp77;struct Cyc_Absyn_Tvar*_tmp78;struct Cyc_Absyn_Vardecl*
_tmp79;void*_tmp7A;void*_tmp7B;void*_tmp7C;struct Cyc_Absyn_Enumdecl*_tmp7D;void*
_tmp7E;struct Cyc_Absyn_Pat*_tmp7F;struct Cyc_List_List*_tmp80;struct Cyc_List_List**
_tmp81;int _tmp82;struct Cyc_Absyn_AggrInfo _tmp83;union Cyc_Absyn_AggrInfoU_union
_tmp84;struct Cyc_Absyn_Aggrdecl**_tmp85;struct Cyc_Absyn_Aggrdecl*_tmp86;struct
Cyc_List_List*_tmp87;struct Cyc_List_List**_tmp88;struct Cyc_List_List*_tmp89;
struct Cyc_List_List*_tmp8A;struct Cyc_List_List**_tmp8B;int _tmp8C;struct Cyc_Absyn_Datatypedecl*
_tmp8D;struct Cyc_Absyn_Datatypefield*_tmp8E;struct Cyc_List_List*_tmp8F;struct Cyc_List_List**
_tmp90;int _tmp91;struct Cyc_Absyn_AggrInfo _tmp92;union Cyc_Absyn_AggrInfoU_union
_tmp93;_LL3D: if((int)_tmp73 != 0)goto _LL3F;_LL3E: t=Cyc_Tcpat_any_type(Cyc_Tcenv_lookup_type_vars(
te),topt);goto _LL3C;_LL3F: if(_tmp73 <= (void*)2)goto _LL53;if(*((int*)_tmp73)!= 0)
goto _LL41;_tmp74=((struct Cyc_Absyn_Var_p_struct*)_tmp73)->f1;_tmp75=((struct Cyc_Absyn_Var_p_struct*)
_tmp73)->f2;_LL40: res=Cyc_Tcpat_tcPatRec(te,_tmp75,topt,rgn_opt);t=(void*)((
struct Cyc_Core_Opt*)_check_null(_tmp75->topt))->v;Cyc_Tcpat_set_vd(_tmp74,& res.patvars,
t);goto _LL3C;_LL41: if(*((int*)_tmp73)!= 1)goto _LL43;_tmp76=((struct Cyc_Absyn_Reference_p_struct*)
_tmp73)->f1;_tmp77=((struct Cyc_Absyn_Reference_p_struct*)_tmp73)->f2;_LL42: res=
Cyc_Tcpat_tcPatRec(te,_tmp77,topt,rgn_opt);t=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp77->topt))->v;if(rgn_opt == 0){({void*_tmp94=0;Cyc_Tcutil_terr(p->loc,({
const char*_tmp95="* pattern would point into an unknown/unallowed region";
_tag_dyneither(_tmp95,sizeof(char),55);}),_tag_dyneither(_tmp94,sizeof(void*),0));});
goto _LL3C;}else{if(Cyc_Tcutil_is_noalias_pointer(t))({void*_tmp96=0;((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmp97="* pattern cannot take the address of an alias-free path";_tag_dyneither(
_tmp97,sizeof(char),56);}),_tag_dyneither(_tmp96,sizeof(void*),0));});}Cyc_Tcpat_set_vd(
_tmp76,& res.patvars,(void*)({struct Cyc_Absyn_PointerType_struct*_tmp98=_cycalloc(
sizeof(*_tmp98));_tmp98[0]=({struct Cyc_Absyn_PointerType_struct _tmp99;_tmp99.tag=
4;_tmp99.f1=({struct Cyc_Absyn_PtrInfo _tmp9A;_tmp9A.elt_typ=(void*)t;_tmp9A.elt_tq=
Cyc_Absyn_empty_tqual(0);_tmp9A.ptr_atts=({struct Cyc_Absyn_PtrAtts _tmp9B;_tmp9B.rgn=(
void*)*rgn_opt;_tmp9B.nullable=Cyc_Absyn_false_conref;_tmp9B.bounds=Cyc_Absyn_empty_conref();
_tmp9B.zero_term=((struct Cyc_Absyn_Conref*(*)())Cyc_Absyn_empty_conref)();_tmp9B.ptrloc=
0;_tmp9B;});_tmp9A;});_tmp99;});_tmp98;}));goto _LL3C;_LL43: if(*((int*)_tmp73)!= 
2)goto _LL45;_tmp78=((struct Cyc_Absyn_TagInt_p_struct*)_tmp73)->f1;_tmp79=((
struct Cyc_Absyn_TagInt_p_struct*)_tmp73)->f2;_LL44: Cyc_Tcpat_set_vd(_tmp79,& res.patvars,(
void*)_tmp79->type);{struct _RegionHandle*_tmp9C=Cyc_Tcenv_get_fnrgn(te);Cyc_Tcenv_add_type_vars(
_tmp9C,p->loc,te,({struct Cyc_Absyn_Tvar*_tmp9D[1];_tmp9D[0]=_tmp78;((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp9D,sizeof(struct Cyc_Absyn_Tvar*),
1));}));if(res.tvars_and_bounds_opt == 0)res.tvars_and_bounds_opt=({struct _tuple6*
_tmp9E=_cycalloc(sizeof(*_tmp9E));_tmp9E->f1=0;_tmp9E->f2=0;_tmp9E;});(*res.tvars_and_bounds_opt).f1=({
struct Cyc_List_List*_tmp9F=_cycalloc(sizeof(*_tmp9F));_tmp9F->hd=_tmp78;_tmp9F->tl=(*
res.tvars_and_bounds_opt).f1;_tmp9F;});t=Cyc_Absyn_uint_typ;goto _LL3C;}_LL45: if(*((
int*)_tmp73)!= 7)goto _LL47;_tmp7A=(void*)((struct Cyc_Absyn_Int_p_struct*)_tmp73)->f1;
if((int)_tmp7A != 1)goto _LL47;_LL46: t=Cyc_Tcpat_num_type(topt,Cyc_Absyn_uint_typ);
goto _LL3C;_LL47: if(*((int*)_tmp73)!= 7)goto _LL49;_tmp7B=(void*)((struct Cyc_Absyn_Int_p_struct*)
_tmp73)->f1;if((int)_tmp7B != 2)goto _LL49;_LL48: goto _LL4A;_LL49: if(*((int*)_tmp73)
!= 7)goto _LL4B;_tmp7C=(void*)((struct Cyc_Absyn_Int_p_struct*)_tmp73)->f1;if((int)
_tmp7C != 0)goto _LL4B;_LL4A: t=Cyc_Tcpat_num_type(topt,Cyc_Absyn_sint_typ);goto
_LL3C;_LL4B: if(*((int*)_tmp73)!= 8)goto _LL4D;_LL4C: t=Cyc_Tcpat_num_type(topt,Cyc_Absyn_char_typ);
goto _LL3C;_LL4D: if(*((int*)_tmp73)!= 9)goto _LL4F;_LL4E: t=Cyc_Tcpat_num_type(topt,
Cyc_Absyn_float_typ);goto _LL3C;_LL4F: if(*((int*)_tmp73)!= 10)goto _LL51;_tmp7D=((
struct Cyc_Absyn_Enum_p_struct*)_tmp73)->f1;_LL50: t=Cyc_Tcpat_num_type(topt,(void*)({
struct Cyc_Absyn_EnumType_struct*_tmpA0=_cycalloc(sizeof(*_tmpA0));_tmpA0[0]=({
struct Cyc_Absyn_EnumType_struct _tmpA1;_tmpA1.tag=12;_tmpA1.f1=_tmp7D->name;
_tmpA1.f2=(struct Cyc_Absyn_Enumdecl*)_tmp7D;_tmpA1;});_tmpA0;}));goto _LL3C;_LL51:
if(*((int*)_tmp73)!= 11)goto _LL53;_tmp7E=(void*)((struct Cyc_Absyn_AnonEnum_p_struct*)
_tmp73)->f1;_LL52: t=Cyc_Tcpat_num_type(topt,_tmp7E);goto _LL3C;_LL53: if((int)
_tmp73 != 1)goto _LL55;_LL54: if(topt != 0){void*_tmpA2=Cyc_Tcutil_compress(*topt);
_LL66: if(_tmpA2 <= (void*)4)goto _LL68;if(*((int*)_tmpA2)!= 4)goto _LL68;_LL67: t=*
topt;goto tcpat_end;_LL68:;_LL69: goto _LL65;_LL65:;}{struct Cyc_Core_Opt*_tmpA3=Cyc_Tcenv_lookup_opt_type_vars(
te);t=(void*)({struct Cyc_Absyn_PointerType_struct*_tmpA4=_cycalloc(sizeof(*
_tmpA4));_tmpA4[0]=({struct Cyc_Absyn_PointerType_struct _tmpA5;_tmpA5.tag=4;
_tmpA5.f1=({struct Cyc_Absyn_PtrInfo _tmpA6;_tmpA6.elt_typ=(void*)Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_ak,_tmpA3);_tmpA6.elt_tq=Cyc_Absyn_empty_tqual(0);
_tmpA6.ptr_atts=({struct Cyc_Absyn_PtrAtts _tmpA7;_tmpA7.rgn=(void*)Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,_tmpA3);_tmpA7.nullable=Cyc_Absyn_true_conref;
_tmpA7.bounds=Cyc_Absyn_empty_conref();_tmpA7.zero_term=((struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)();_tmpA7.ptrloc=0;_tmpA7;});_tmpA6;});_tmpA5;});_tmpA4;});
goto _LL3C;}_LL55: if(_tmp73 <= (void*)2)goto _LL57;if(*((int*)_tmp73)!= 4)goto _LL57;
_tmp7F=((struct Cyc_Absyn_Pointer_p_struct*)_tmp73)->f1;_LL56: {void*inner_typ=(
void*)0;void**_tmpA8=0;if(topt != 0){void*_tmpA9=Cyc_Tcutil_compress(*topt);
struct Cyc_Absyn_PtrInfo _tmpAA;void*_tmpAB;_LL6B: if(_tmpA9 <= (void*)4)goto _LL6D;
if(*((int*)_tmpA9)!= 4)goto _LL6D;_tmpAA=((struct Cyc_Absyn_PointerType_struct*)
_tmpA9)->f1;_tmpAB=(void*)_tmpAA.elt_typ;_LL6C: inner_typ=_tmpAB;_tmpA8=(void**)&
inner_typ;goto _LL6A;_LL6D:;_LL6E: goto _LL6A;_LL6A:;}{void*ptr_rgn=Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_trk,Cyc_Tcenv_lookup_opt_type_vars(te));res=Cyc_Tcpat_combine_results(
res,Cyc_Tcpat_tcPatRec(te,_tmp7F,_tmpA8,(void**)& ptr_rgn));if(_tmpA8 == 0){void*
_tmpAC=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp7F->topt))->v);
struct Cyc_Absyn_DatatypeFieldInfo _tmpAD;union Cyc_Absyn_DatatypeFieldInfoU_union
_tmpAE;struct Cyc_Absyn_Datatypedecl*_tmpAF;struct Cyc_Absyn_Datatypefield*_tmpB0;
struct Cyc_List_List*_tmpB1;_LL70: if(_tmpAC <= (void*)4)goto _LL72;if(*((int*)
_tmpAC)!= 3)goto _LL72;_tmpAD=((struct Cyc_Absyn_DatatypeFieldType_struct*)_tmpAC)->f1;
_tmpAE=_tmpAD.field_info;if((((((struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmpAC)->f1).field_info).KnownDatatypefield).tag != 1)goto _LL72;_tmpAF=(_tmpAE.KnownDatatypefield).f1;
_tmpB0=(_tmpAE.KnownDatatypefield).f2;_tmpB1=_tmpAD.targs;if(!(!_tmpAF->is_flat))
goto _LL72;_LL71: t=(void*)({struct Cyc_Absyn_DatatypeType_struct*_tmpB2=_cycalloc(
sizeof(*_tmpB2));_tmpB2[0]=({struct Cyc_Absyn_DatatypeType_struct _tmpB3;_tmpB3.tag=
2;_tmpB3.f1=({struct Cyc_Absyn_DatatypeInfo _tmpB4;_tmpB4.datatype_info=(union Cyc_Absyn_DatatypeInfoU_union)({
union Cyc_Absyn_DatatypeInfoU_union _tmpB6;(_tmpB6.KnownDatatype).tag=1;(_tmpB6.KnownDatatype).f1=({
struct Cyc_Absyn_Datatypedecl**_tmpB7=_cycalloc(sizeof(*_tmpB7));_tmpB7[0]=_tmpAF;
_tmpB7;});_tmpB6;});_tmpB4.targs=_tmpB1;_tmpB4.rgn=({struct Cyc_Core_Opt*_tmpB5=
_cycalloc(sizeof(*_tmpB5));_tmpB5->v=(void*)ptr_rgn;_tmpB5;});_tmpB4;});_tmpB3;});
_tmpB2;});goto _LL6F;_LL72:;_LL73: t=(void*)({struct Cyc_Absyn_PointerType_struct*
_tmpB8=_cycalloc(sizeof(*_tmpB8));_tmpB8[0]=({struct Cyc_Absyn_PointerType_struct
_tmpB9;_tmpB9.tag=4;_tmpB9.f1=({struct Cyc_Absyn_PtrInfo _tmpBA;_tmpBA.elt_typ=(
void*)((void*)((struct Cyc_Core_Opt*)_check_null(_tmp7F->topt))->v);_tmpBA.elt_tq=
Cyc_Absyn_empty_tqual(0);_tmpBA.ptr_atts=({struct Cyc_Absyn_PtrAtts _tmpBB;_tmpBB.rgn=(
void*)ptr_rgn;_tmpBB.nullable=((struct Cyc_Absyn_Conref*(*)())Cyc_Absyn_empty_conref)();
_tmpBB.bounds=Cyc_Absyn_empty_conref();_tmpBB.zero_term=((struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)();_tmpBB.ptrloc=0;_tmpBB;});_tmpBA;});_tmpB9;});_tmpB8;});
goto _LL6F;_LL6F:;}else{t=(void*)({struct Cyc_Absyn_PointerType_struct*_tmpBC=
_cycalloc(sizeof(*_tmpBC));_tmpBC[0]=({struct Cyc_Absyn_PointerType_struct _tmpBD;
_tmpBD.tag=4;_tmpBD.f1=({struct Cyc_Absyn_PtrInfo _tmpBE;_tmpBE.elt_typ=(void*)((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp7F->topt))->v);_tmpBE.elt_tq=Cyc_Absyn_empty_tqual(
0);_tmpBE.ptr_atts=({struct Cyc_Absyn_PtrAtts _tmpBF;_tmpBF.rgn=(void*)ptr_rgn;
_tmpBF.nullable=((struct Cyc_Absyn_Conref*(*)())Cyc_Absyn_empty_conref)();_tmpBF.bounds=
Cyc_Absyn_empty_conref();_tmpBF.zero_term=((struct Cyc_Absyn_Conref*(*)())Cyc_Absyn_empty_conref)();
_tmpBF.ptrloc=0;_tmpBF;});_tmpBE;});_tmpBD;});_tmpBC;});}goto _LL3C;}}_LL57: if(
_tmp73 <= (void*)2)goto _LL59;if(*((int*)_tmp73)!= 3)goto _LL59;_tmp80=((struct Cyc_Absyn_Tuple_p_struct*)
_tmp73)->f1;_tmp81=(struct Cyc_List_List**)&((struct Cyc_Absyn_Tuple_p_struct*)
_tmp73)->f1;_tmp82=((struct Cyc_Absyn_Tuple_p_struct*)_tmp73)->f2;_LL58: {struct
Cyc_List_List*_tmpC0=*_tmp81;struct Cyc_List_List*pat_ts=0;struct Cyc_List_List*
topt_ts=0;if(topt != 0){void*_tmpC1=Cyc_Tcutil_compress(*topt);struct Cyc_List_List*
_tmpC2;_LL75: if(_tmpC1 <= (void*)4)goto _LL77;if(*((int*)_tmpC1)!= 9)goto _LL77;
_tmpC2=((struct Cyc_Absyn_TupleType_struct*)_tmpC1)->f1;_LL76: topt_ts=_tmpC2;if(
_tmp82){int _tmpC3=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpC0);int
_tmpC4=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpC2);if(_tmpC3 < 
_tmpC4){struct Cyc_List_List*wild_ps=0;{int i=0;for(0;i < _tmpC4 - _tmpC3;++ i){
wild_ps=({struct Cyc_List_List*_tmpC5=_cycalloc(sizeof(*_tmpC5));_tmpC5->hd=Cyc_Tcpat_wild_pat(
p->loc);_tmpC5->tl=wild_ps;_tmpC5;});}}*_tmp81=((struct Cyc_List_List*(*)(struct
Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmpC0,wild_ps);
_tmpC0=*_tmp81;}else{if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpC0)
== ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpC2))({void*_tmpC6=0;Cyc_Tcutil_warn(
p->loc,({const char*_tmpC7="unnecessary ... in tuple pattern";_tag_dyneither(
_tmpC7,sizeof(char),33);}),_tag_dyneither(_tmpC6,sizeof(void*),0));});}}goto
_LL74;_LL77:;_LL78: goto _LL74;_LL74:;}else{if(_tmp82)({void*_tmpC8=0;Cyc_Tcutil_terr(
p->loc,({const char*_tmpC9="cannot determine missing fields for ... in tuple pattern";
_tag_dyneither(_tmpC9,sizeof(char),57);}),_tag_dyneither(_tmpC8,sizeof(void*),0));});}
for(0;_tmpC0 != 0;_tmpC0=_tmpC0->tl){void**_tmpCA=0;if(topt_ts != 0){_tmpCA=(void**)&(*((
struct _tuple9*)topt_ts->hd)).f2;topt_ts=topt_ts->tl;}res=Cyc_Tcpat_combine_results(
res,Cyc_Tcpat_tcPatRec(te,(struct Cyc_Absyn_Pat*)_tmpC0->hd,_tmpCA,rgn_opt));
pat_ts=({struct Cyc_List_List*_tmpCB=_cycalloc(sizeof(*_tmpCB));_tmpCB->hd=({
struct _tuple9*_tmpCC=_cycalloc(sizeof(*_tmpCC));_tmpCC->f1=Cyc_Absyn_empty_tqual(
0);_tmpCC->f2=(void*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Pat*)
_tmpC0->hd)->topt))->v;_tmpCC;});_tmpCB->tl=pat_ts;_tmpCB;});}t=(void*)({struct
Cyc_Absyn_TupleType_struct*_tmpCD=_cycalloc(sizeof(*_tmpCD));_tmpCD[0]=({struct
Cyc_Absyn_TupleType_struct _tmpCE;_tmpCE.tag=9;_tmpCE.f1=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(pat_ts);_tmpCE;});_tmpCD;});goto _LL3C;}
_LL59: if(_tmp73 <= (void*)2)goto _LL5B;if(*((int*)_tmp73)!= 5)goto _LL5B;_tmp83=((
struct Cyc_Absyn_Aggr_p_struct*)_tmp73)->f1;_tmp84=_tmp83.aggr_info;if((((((
struct Cyc_Absyn_Aggr_p_struct*)_tmp73)->f1).aggr_info).KnownAggr).tag != 1)goto
_LL5B;_tmp85=(_tmp84.KnownAggr).f1;_tmp86=*_tmp85;_tmp87=_tmp83.targs;_tmp88=(
struct Cyc_List_List**)&(((struct Cyc_Absyn_Aggr_p_struct*)_tmp73)->f1).targs;
_tmp89=((struct Cyc_Absyn_Aggr_p_struct*)_tmp73)->f2;_tmp8A=((struct Cyc_Absyn_Aggr_p_struct*)
_tmp73)->f3;_tmp8B=(struct Cyc_List_List**)&((struct Cyc_Absyn_Aggr_p_struct*)
_tmp73)->f3;_tmp8C=((struct Cyc_Absyn_Aggr_p_struct*)_tmp73)->f4;_LL5A: {struct
Cyc_List_List*_tmpCF=*_tmp8B;struct _dyneither_ptr aggr_str=(void*)_tmp86->kind == (
void*)0?({const char*_tmp117="struct";_tag_dyneither(_tmp117,sizeof(char),7);}):({
const char*_tmp118="union";_tag_dyneither(_tmp118,sizeof(char),6);});if(_tmp86->impl
== 0){({struct Cyc_String_pa_struct _tmpD2;_tmpD2.tag=0;_tmpD2.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)aggr_str);{void*_tmpD0[1]={& _tmpD2};Cyc_Tcutil_terr(
p->loc,({const char*_tmpD1="can't destructure an abstract %s";_tag_dyneither(
_tmpD1,sizeof(char),33);}),_tag_dyneither(_tmpD0,sizeof(void*),1));}});t=Cyc_Absyn_wildtyp(
Cyc_Tcenv_lookup_opt_type_vars(te));goto _LL3C;}if(_tmp89 != 0  || (void*)_tmp86->kind
== (void*)1  && ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp86->impl))->tagged)
rgn_opt=0;{struct _RegionHandle _tmpD3=_new_region("rgn");struct _RegionHandle*rgn=&
_tmpD3;_push_region(rgn);{struct Cyc_List_List*_tmpD4=0;struct Cyc_List_List*
outlives_constraints=0;struct Cyc_List_List*_tmpD5=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp86->impl))->exist_vars;{struct Cyc_List_List*t=_tmp89;for(0;t != 0;
t=t->tl){struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)t->hd;struct Cyc_Absyn_Tvar*
uv=(struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)_check_null(_tmpD5))->hd;_tmpD5=
_tmpD5->tl;{void*_tmpD6=Cyc_Absyn_compress_kb((void*)tv->kind);void*_tmpD7=Cyc_Absyn_compress_kb((
void*)uv->kind);int error=0;void*k2;{void*_tmpD8=_tmpD7;void*_tmpD9;void*_tmpDA;
_LL7A: if(*((int*)_tmpD8)!= 2)goto _LL7C;_tmpD9=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmpD8)->f2;_LL7B: _tmpDA=_tmpD9;goto _LL7D;_LL7C: if(*((int*)_tmpD8)!= 0)goto _LL7E;
_tmpDA=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmpD8)->f1;_LL7D: k2=_tmpDA;goto
_LL79;_LL7E:;_LL7F:({void*_tmpDB=0;((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmpDC="unconstrained existential type variable in aggregate";
_tag_dyneither(_tmpDC,sizeof(char),53);}),_tag_dyneither(_tmpDB,sizeof(void*),0));});
_LL79:;}{void*_tmpDD=_tmpD6;void*_tmpDE;struct Cyc_Core_Opt*_tmpDF;struct Cyc_Core_Opt**
_tmpE0;void*_tmpE1;struct Cyc_Core_Opt*_tmpE2;struct Cyc_Core_Opt**_tmpE3;_LL81:
if(*((int*)_tmpDD)!= 0)goto _LL83;_tmpDE=(void*)((struct Cyc_Absyn_Eq_kb_struct*)
_tmpDD)->f1;_LL82: if(!Cyc_Tcutil_kind_leq(k2,_tmpDE))error=1;goto _LL80;_LL83: if(*((
int*)_tmpDD)!= 2)goto _LL85;_tmpDF=((struct Cyc_Absyn_Less_kb_struct*)_tmpDD)->f1;
_tmpE0=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)_tmpDD)->f1;
_tmpE1=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmpDD)->f2;_LL84: _tmpE3=_tmpE0;
goto _LL86;_LL85: if(*((int*)_tmpDD)!= 1)goto _LL80;_tmpE2=((struct Cyc_Absyn_Unknown_kb_struct*)
_tmpDD)->f1;_tmpE3=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)
_tmpDD)->f1;_LL86:*_tmpE3=({struct Cyc_Core_Opt*_tmpE4=_cycalloc(sizeof(*_tmpE4));
_tmpE4->v=(void*)_tmpD7;_tmpE4;});goto _LL80;_LL80:;}if(error)({struct Cyc_String_pa_struct
_tmpE9;_tmpE9.tag=0;_tmpE9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(
k2));{struct Cyc_String_pa_struct _tmpE8;_tmpE8.tag=0;_tmpE8.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kindbound2string(_tmpD6));{
struct Cyc_String_pa_struct _tmpE7;_tmpE7.tag=0;_tmpE7.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)*tv->name);{void*_tmpE5[3]={& _tmpE7,& _tmpE8,& _tmpE9};Cyc_Tcutil_terr(
p->loc,({const char*_tmpE6="type variable %s has kind %s but must have at least kind %s";
_tag_dyneither(_tmpE6,sizeof(char),60);}),_tag_dyneither(_tmpE5,sizeof(void*),3));}}}});{
void*vartype=(void*)({struct Cyc_Absyn_VarType_struct*_tmpEF=_cycalloc(sizeof(*
_tmpEF));_tmpEF[0]=({struct Cyc_Absyn_VarType_struct _tmpF0;_tmpF0.tag=1;_tmpF0.f1=
tv;_tmpF0;});_tmpEF;});_tmpD4=({struct Cyc_List_List*_tmpEA=_region_malloc(rgn,
sizeof(*_tmpEA));_tmpEA->hd=(void*)((void*)vartype);_tmpEA->tl=_tmpD4;_tmpEA;});
if(k2 == (void*)3)outlives_constraints=({struct Cyc_List_List*_tmpEB=_cycalloc(
sizeof(*_tmpEB));_tmpEB->hd=({struct _tuple0*_tmpEC=_cycalloc(sizeof(*_tmpEC));
_tmpEC->f1=Cyc_Absyn_empty_effect;_tmpEC->f2=vartype;_tmpEC;});_tmpEB->tl=
outlives_constraints;_tmpEB;});else{if(k2 == (void*)4  || k2 == (void*)5)({void*
_tmpED=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmpEE="opened existential had unique or top region kind";
_tag_dyneither(_tmpEE,sizeof(char),49);}),_tag_dyneither(_tmpED,sizeof(void*),0));});}}}}}
_tmpD4=Cyc_List_imp_rev(_tmpD4);{struct _RegionHandle*_tmpF1=Cyc_Tcenv_get_fnrgn(
te);struct Cyc_Tcenv_Tenv*te2=Cyc_Tcenv_add_type_vars(_tmpF1,p->loc,te,_tmp89);
struct Cyc_List_List*_tmpF2=Cyc_Tcenv_lookup_type_vars(te2);struct _tuple4 _tmpF3=({
struct _tuple4 _tmp116;_tmp116.f1=_tmpF2;_tmp116.f2=rgn;_tmp116;});struct Cyc_List_List*
_tmpF4=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple5*(*f)(struct
_tuple4*,struct Cyc_Absyn_Tvar*),struct _tuple4*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(
rgn,Cyc_Tcutil_r_make_inst_var,& _tmpF3,_tmp86->tvs);struct Cyc_List_List*_tmpF5=((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp86->impl))->exist_vars,_tmpD4);struct Cyc_List_List*_tmpF6=((
struct Cyc_List_List*(*)(void*(*f)(struct _tuple5*),struct Cyc_List_List*x))Cyc_List_map)((
void*(*)(struct _tuple5*))Cyc_Core_snd,_tmpF4);struct Cyc_List_List*_tmpF7=((
struct Cyc_List_List*(*)(void*(*f)(struct _tuple5*),struct Cyc_List_List*x))Cyc_List_map)((
void*(*)(struct _tuple5*))Cyc_Core_snd,_tmpF5);struct Cyc_List_List*_tmpF8=((
struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rappend)(rgn,_tmpF4,_tmpF5);if(_tmp89 != 0  || ((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp86->impl))->rgn_po != 0){if(res.tvars_and_bounds_opt == 0)res.tvars_and_bounds_opt=({
struct _tuple6*_tmpF9=_cycalloc(sizeof(*_tmpF9));_tmpF9->f1=0;_tmpF9->f2=0;_tmpF9;});(*
res.tvars_and_bounds_opt).f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_append)((*res.tvars_and_bounds_opt).f1,_tmp89);(*
res.tvars_and_bounds_opt).f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_append)((*res.tvars_and_bounds_opt).f2,
outlives_constraints);{struct Cyc_List_List*_tmpFA=0;{struct Cyc_List_List*_tmpFB=((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp86->impl))->rgn_po;for(0;_tmpFB != 
0;_tmpFB=_tmpFB->tl){_tmpFA=({struct Cyc_List_List*_tmpFC=_cycalloc(sizeof(*
_tmpFC));_tmpFC->hd=({struct _tuple0*_tmpFD=_cycalloc(sizeof(*_tmpFD));_tmpFD->f1=
Cyc_Tcutil_rsubstitute(rgn,_tmpF8,(*((struct _tuple0*)_tmpFB->hd)).f1);_tmpFD->f2=
Cyc_Tcutil_rsubstitute(rgn,_tmpF8,(*((struct _tuple0*)_tmpFB->hd)).f2);_tmpFD;});
_tmpFC->tl=_tmpFA;_tmpFC;});}}_tmpFA=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(_tmpFA);(*res.tvars_and_bounds_opt).f2=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*res.tvars_and_bounds_opt).f2,
_tmpFA);}}*_tmp88=_tmpF6;t=(void*)({struct Cyc_Absyn_AggrType_struct*_tmpFE=
_cycalloc(sizeof(*_tmpFE));_tmpFE[0]=({struct Cyc_Absyn_AggrType_struct _tmpFF;
_tmpFF.tag=10;_tmpFF.f1=({struct Cyc_Absyn_AggrInfo _tmp100;_tmp100.aggr_info=(
union Cyc_Absyn_AggrInfoU_union)({union Cyc_Absyn_AggrInfoU_union _tmp101;(_tmp101.KnownAggr).tag=
1;(_tmp101.KnownAggr).f1=({struct Cyc_Absyn_Aggrdecl**_tmp102=_cycalloc(sizeof(*
_tmp102));_tmp102[0]=_tmp86;_tmp102;});_tmp101;});_tmp100.targs=*_tmp88;_tmp100;});
_tmpFF;});_tmpFE;});if(_tmp8C  && (void*)_tmp86->kind == (void*)1)({void*_tmp103=0;
Cyc_Tcutil_warn(p->loc,({const char*_tmp104="`...' pattern not allowed in union pattern";
_tag_dyneither(_tmp104,sizeof(char),43);}),_tag_dyneither(_tmp103,sizeof(void*),
0));});else{if(_tmp8C){int _tmp105=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmpCF);int _tmp106=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp86->impl))->fields);if(_tmp105 < _tmp106){struct Cyc_List_List*
wild_dps=0;{int i=0;for(0;i < _tmp106 - _tmp105;++ i){wild_dps=({struct Cyc_List_List*
_tmp107=_cycalloc(sizeof(*_tmp107));_tmp107->hd=({struct _tuple8*_tmp108=
_cycalloc(sizeof(*_tmp108));_tmp108->f1=0;_tmp108->f2=Cyc_Tcpat_wild_pat(p->loc);
_tmp108;});_tmp107->tl=wild_dps;_tmp107;});}}*_tmp8B=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmpCF,
wild_dps);_tmpCF=*_tmp8B;}else{if(_tmp105 == _tmp106)({void*_tmp109=0;Cyc_Tcutil_warn(
p->loc,({const char*_tmp10A="unnecessary ... in struct pattern";_tag_dyneither(
_tmp10A,sizeof(char),34);}),_tag_dyneither(_tmp109,sizeof(void*),0));});}}}{
struct Cyc_List_List*fields=((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,
struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,void*,struct Cyc_List_List*
fields))Cyc_Tcutil_resolve_aggregate_designators)(rgn,p->loc,_tmpCF,(void*)
_tmp86->kind,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp86->impl))->fields);
for(0;fields != 0;fields=fields->tl){struct _tuple10 _tmp10C;struct Cyc_Absyn_Aggrfield*
_tmp10D;struct Cyc_Absyn_Pat*_tmp10E;struct _tuple10*_tmp10B=(struct _tuple10*)
fields->hd;_tmp10C=*_tmp10B;_tmp10D=_tmp10C.f1;_tmp10E=_tmp10C.f2;{void*_tmp10F=
Cyc_Tcutil_rsubstitute(rgn,_tmpF8,(void*)_tmp10D->type);res=Cyc_Tcpat_combine_results(
res,Cyc_Tcpat_tcPatRec(te2,_tmp10E,(void**)& _tmp10F,rgn_opt));if(!Cyc_Tcutil_unify((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp10E->topt))->v,_tmp10F))({struct Cyc_String_pa_struct
_tmp115;_tmp115.tag=0;_tmp115.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(_tmp10E->topt))->v));{
struct Cyc_String_pa_struct _tmp114;_tmp114.tag=0;_tmp114.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp10F));{struct Cyc_String_pa_struct
_tmp113;_tmp113.tag=0;_tmp113.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
aggr_str);{struct Cyc_String_pa_struct _tmp112;_tmp112.tag=0;_tmp112.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*_tmp10D->name);{void*_tmp110[4]={& _tmp112,&
_tmp113,& _tmp114,& _tmp115};Cyc_Tcutil_terr(p->loc,({const char*_tmp111="field %s of %s pattern expects type %s != %s";
_tag_dyneither(_tmp111,sizeof(char),45);}),_tag_dyneither(_tmp110,sizeof(void*),
4));}}}}});}}}}}_npop_handler(0);goto _LL3C;;_pop_region(rgn);}}_LL5B: if(_tmp73 <= (
void*)2)goto _LL5D;if(*((int*)_tmp73)!= 6)goto _LL5D;_tmp8D=((struct Cyc_Absyn_Datatype_p_struct*)
_tmp73)->f1;_tmp8E=((struct Cyc_Absyn_Datatype_p_struct*)_tmp73)->f2;_tmp8F=((
struct Cyc_Absyn_Datatype_p_struct*)_tmp73)->f3;_tmp90=(struct Cyc_List_List**)&((
struct Cyc_Absyn_Datatype_p_struct*)_tmp73)->f3;_tmp91=((struct Cyc_Absyn_Datatype_p_struct*)
_tmp73)->f4;_LL5C: {struct Cyc_List_List*_tmp119=*_tmp90;if(_tmp8D->is_flat)
rgn_opt=0;{struct _RegionHandle*_tmp11A=Cyc_Tcenv_get_fnrgn(te);{struct Cyc_List_List*
tqts=_tmp8E->typs;struct Cyc_List_List*_tmp11B=Cyc_Tcenv_lookup_type_vars(te);
struct _tuple4 _tmp11C=({struct _tuple4 _tmp145;_tmp145.f1=_tmp11B;_tmp145.f2=
_tmp11A;_tmp145;});struct Cyc_List_List*_tmp11D=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _tuple5*(*f)(struct _tuple4*,struct Cyc_Absyn_Tvar*),struct
_tuple4*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(_tmp11A,Cyc_Tcutil_r_make_inst_var,&
_tmp11C,_tmp8D->tvs);struct Cyc_List_List*_tmp11E=((struct Cyc_List_List*(*)(void*(*
f)(struct _tuple5*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple5*))
Cyc_Core_snd,_tmp11D);if(tqts == 0  || _tmp8D->is_flat){struct Cyc_Core_Opt*ropt=0;
if(!_tmp8D->is_flat)ropt=({struct Cyc_Core_Opt*_tmp11F=_cycalloc(sizeof(*_tmp11F));
_tmp11F->v=(void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,({
struct Cyc_Core_Opt*_tmp120=_cycalloc(sizeof(*_tmp120));_tmp120->v=_tmp11B;
_tmp120;}));_tmp11F;});t=(void*)({struct Cyc_Absyn_DatatypeType_struct*_tmp121=
_cycalloc(sizeof(*_tmp121));_tmp121[0]=({struct Cyc_Absyn_DatatypeType_struct
_tmp122;_tmp122.tag=2;_tmp122.f1=({struct Cyc_Absyn_DatatypeInfo _tmp123;_tmp123.datatype_info=(
union Cyc_Absyn_DatatypeInfoU_union)({union Cyc_Absyn_DatatypeInfoU_union _tmp124;(
_tmp124.KnownDatatype).tag=1;(_tmp124.KnownDatatype).f1=({struct Cyc_Absyn_Datatypedecl**
_tmp125=_cycalloc(sizeof(*_tmp125));_tmp125[0]=_tmp8D;_tmp125;});_tmp124;});
_tmp123.targs=_tmp11E;_tmp123.rgn=ropt;_tmp123;});_tmp122;});_tmp121;});}else{t=(
void*)({struct Cyc_Absyn_DatatypeFieldType_struct*_tmp126=_cycalloc(sizeof(*
_tmp126));_tmp126[0]=({struct Cyc_Absyn_DatatypeFieldType_struct _tmp127;_tmp127.tag=
3;_tmp127.f1=({struct Cyc_Absyn_DatatypeFieldInfo _tmp128;_tmp128.field_info=(
union Cyc_Absyn_DatatypeFieldInfoU_union)({union Cyc_Absyn_DatatypeFieldInfoU_union
_tmp129;(_tmp129.KnownDatatypefield).tag=1;(_tmp129.KnownDatatypefield).f1=
_tmp8D;(_tmp129.KnownDatatypefield).f2=_tmp8E;_tmp129;});_tmp128.targs=_tmp11E;
_tmp128;});_tmp127;});_tmp126;});}if(topt != 0  && (tqts == 0  || _tmp8D->is_flat)){
void*_tmp12A=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_DatatypeInfo _tmp12B;
struct Cyc_List_List*_tmp12C;_LL88: if(_tmp12A <= (void*)4)goto _LL8C;if(*((int*)
_tmp12A)!= 3)goto _LL8A;_LL89: t=(void*)({struct Cyc_Absyn_DatatypeFieldType_struct*
_tmp12D=_cycalloc(sizeof(*_tmp12D));_tmp12D[0]=({struct Cyc_Absyn_DatatypeFieldType_struct
_tmp12E;_tmp12E.tag=3;_tmp12E.f1=({struct Cyc_Absyn_DatatypeFieldInfo _tmp12F;
_tmp12F.field_info=(union Cyc_Absyn_DatatypeFieldInfoU_union)({union Cyc_Absyn_DatatypeFieldInfoU_union
_tmp130;(_tmp130.KnownDatatypefield).tag=1;(_tmp130.KnownDatatypefield).f1=
_tmp8D;(_tmp130.KnownDatatypefield).f2=_tmp8E;_tmp130;});_tmp12F.targs=_tmp11E;
_tmp12F;});_tmp12E;});_tmp12D;});goto _LL87;_LL8A: if(*((int*)_tmp12A)!= 2)goto
_LL8C;_tmp12B=((struct Cyc_Absyn_DatatypeType_struct*)_tmp12A)->f1;_tmp12C=
_tmp12B.targs;_LL8B: {struct Cyc_List_List*_tmp131=_tmp11E;for(0;_tmp131 != 0  && 
_tmp12C != 0;(_tmp131=_tmp131->tl,_tmp12C=_tmp12C->tl)){Cyc_Tcutil_unify((void*)
_tmp131->hd,(void*)_tmp12C->hd);}goto _LL87;}_LL8C:;_LL8D: goto _LL87;_LL87:;}if(
_tmp91){int _tmp132=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp119);int
_tmp133=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(tqts);if(_tmp132 < 
_tmp133){struct Cyc_List_List*wild_ps=0;{int i=0;for(0;i < _tmp133 - _tmp132;++ i){
wild_ps=({struct Cyc_List_List*_tmp134=_cycalloc(sizeof(*_tmp134));_tmp134->hd=
Cyc_Tcpat_wild_pat(p->loc);_tmp134->tl=wild_ps;_tmp134;});}}*_tmp90=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp119,
wild_ps);_tmp119=*_tmp90;}else{if(_tmp132 == _tmp133)({struct Cyc_String_pa_struct
_tmp137;_tmp137.tag=0;_tmp137.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp8D->name));{void*_tmp135[1]={& _tmp137};Cyc_Tcutil_warn(
p->loc,({const char*_tmp136="unnecessary ... in datatype field %s";_tag_dyneither(
_tmp136,sizeof(char),37);}),_tag_dyneither(_tmp135,sizeof(void*),1));}});}}for(0;
_tmp119 != 0  && tqts != 0;(_tmp119=_tmp119->tl,tqts=tqts->tl)){struct Cyc_Absyn_Pat*
_tmp138=(struct Cyc_Absyn_Pat*)_tmp119->hd;void*_tmp139=Cyc_Tcutil_rsubstitute(
_tmp11A,_tmp11D,(*((struct _tuple9*)tqts->hd)).f2);res=Cyc_Tcpat_combine_results(
res,Cyc_Tcpat_tcPatRec(te,_tmp138,(void**)& _tmp139,rgn_opt));if(!Cyc_Tcutil_unify((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp138->topt))->v,_tmp139))({struct Cyc_String_pa_struct
_tmp13E;_tmp13E.tag=0;_tmp13E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(_tmp138->topt))->v));{
struct Cyc_String_pa_struct _tmp13D;_tmp13D.tag=0;_tmp13D.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp139));{struct Cyc_String_pa_struct
_tmp13C;_tmp13C.tag=0;_tmp13C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp8E->name));{void*_tmp13A[3]={& _tmp13C,& _tmp13D,&
_tmp13E};Cyc_Tcutil_terr(_tmp138->loc,({const char*_tmp13B="%s expects argument type %s, not %s";
_tag_dyneither(_tmp13B,sizeof(char),36);}),_tag_dyneither(_tmp13A,sizeof(void*),
3));}}}});}if(_tmp119 != 0)({struct Cyc_String_pa_struct _tmp141;_tmp141.tag=0;
_tmp141.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp8E->name));{void*_tmp13F[1]={& _tmp141};Cyc_Tcutil_terr(p->loc,({const char*
_tmp140="too many arguments for datatype constructor %s";_tag_dyneither(_tmp140,
sizeof(char),47);}),_tag_dyneither(_tmp13F,sizeof(void*),1));}});if(tqts != 0)({
struct Cyc_String_pa_struct _tmp144;_tmp144.tag=0;_tmp144.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp8E->name));{void*_tmp142[1]={&
_tmp144};Cyc_Tcutil_terr(p->loc,({const char*_tmp143="too few arguments for datatype constructor %s";
_tag_dyneither(_tmp143,sizeof(char),46);}),_tag_dyneither(_tmp142,sizeof(void*),
1));}});}goto _LL3C;}}_LL5D: if(_tmp73 <= (void*)2)goto _LL5F;if(*((int*)_tmp73)!= 5)
goto _LL5F;_tmp92=((struct Cyc_Absyn_Aggr_p_struct*)_tmp73)->f1;_tmp93=_tmp92.aggr_info;
if((((((struct Cyc_Absyn_Aggr_p_struct*)_tmp73)->f1).aggr_info).UnknownAggr).tag
!= 0)goto _LL5F;_LL5E: goto _LL60;_LL5F: if(_tmp73 <= (void*)2)goto _LL61;if(*((int*)
_tmp73)!= 12)goto _LL61;_LL60: goto _LL62;_LL61: if(_tmp73 <= (void*)2)goto _LL63;if(*((
int*)_tmp73)!= 14)goto _LL63;_LL62: goto _LL64;_LL63: if(_tmp73 <= (void*)2)goto _LL3C;
if(*((int*)_tmp73)!= 13)goto _LL3C;_LL64: t=Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(
te));goto _LL3C;_LL3C:;}tcpat_end: p->topt=({struct Cyc_Core_Opt*_tmp146=_cycalloc(
sizeof(*_tmp146));_tmp146->v=(void*)t;_tmp146;});return res;}}struct Cyc_Tcpat_TcPatResult
Cyc_Tcpat_tcPat(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,void**topt){struct
Cyc_Tcpat_TcPatResult _tmp148=Cyc_Tcpat_tcPatRec(te,p,(void**)topt,0);struct
_RegionHandle*_tmp149=Cyc_Tcenv_get_fnrgn(te);Cyc_Tcutil_check_unique_vars(((
struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr*(*f)(struct Cyc_Absyn_Vardecl*),
struct Cyc_List_List*x))Cyc_List_rmap)(_tmp149,Cyc_Tcpat_get_name,_tmp148.patvars),
p->loc,({const char*_tmp14A="pattern contains a repeated variable";_tag_dyneither(
_tmp14A,sizeof(char),37);}));return _tmp148;}void Cyc_Tcpat_check_pat_regions(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p){void*_tmp14B=(void*)p->r;struct
Cyc_Absyn_Pat*_tmp14C;struct Cyc_List_List*_tmp14D;struct Cyc_List_List*_tmp14E;
struct Cyc_List_List*_tmp14F;_LL8F: if(_tmp14B <= (void*)2)goto _LL97;if(*((int*)
_tmp14B)!= 4)goto _LL91;_tmp14C=((struct Cyc_Absyn_Pointer_p_struct*)_tmp14B)->f1;
_LL90: Cyc_Tcpat_check_pat_regions(te,_tmp14C);{struct _RegionHandle*_tmp150=Cyc_Tcenv_get_fnrgn(
te);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(_tmp150,(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp14C->topt))->v)){({void*_tmp151=0;Cyc_Tcutil_terr(p->loc,({const
char*_tmp152="Pattern dereferences to a non-aliased pointer; use swap";
_tag_dyneither(_tmp152,sizeof(char),56);}),_tag_dyneither(_tmp151,sizeof(void*),
0));});return;}if(Cyc_Tcutil_is_noalias_pointer_or_aggr(_tmp150,(void*)((struct
Cyc_Core_Opt*)_check_null(p->topt))->v)){({void*_tmp153=0;Cyc_Tcutil_terr(p->loc,({
const char*_tmp154="Pattern dereferences a non-aliased pointer; use swap";
_tag_dyneither(_tmp154,sizeof(char),53);}),_tag_dyneither(_tmp153,sizeof(void*),
0));});return;}{void*_tmp155=(void*)((struct Cyc_Core_Opt*)_check_null(p->topt))->v;
struct Cyc_Absyn_PtrInfo _tmp156;struct Cyc_Absyn_PtrAtts _tmp157;void*_tmp158;
struct Cyc_Absyn_DatatypeInfo _tmp159;struct Cyc_Core_Opt*_tmp15A;struct Cyc_Core_Opt
_tmp15B;void*_tmp15C;_LL9A: if(_tmp155 <= (void*)4)goto _LL9E;if(*((int*)_tmp155)!= 
4)goto _LL9C;_tmp156=((struct Cyc_Absyn_PointerType_struct*)_tmp155)->f1;_tmp157=
_tmp156.ptr_atts;_tmp158=(void*)_tmp157.rgn;_LL9B: _tmp15C=_tmp158;goto _LL9D;
_LL9C: if(*((int*)_tmp155)!= 2)goto _LL9E;_tmp159=((struct Cyc_Absyn_DatatypeType_struct*)
_tmp155)->f1;_tmp15A=_tmp159.rgn;if(_tmp15A == 0)goto _LL9E;_tmp15B=*_tmp15A;
_tmp15C=(void*)_tmp15B.v;_LL9D: Cyc_Tcenv_check_rgn_accessible(te,p->loc,_tmp15C);
return;_LL9E:;_LL9F:({void*_tmp15D=0;((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp15E="check_pat_regions: bad pointer type";
_tag_dyneither(_tmp15E,sizeof(char),36);}),_tag_dyneither(_tmp15D,sizeof(void*),
0));});_LL99:;}}_LL91: if(*((int*)_tmp14B)!= 6)goto _LL93;_tmp14D=((struct Cyc_Absyn_Datatype_p_struct*)
_tmp14B)->f3;_LL92: for(0;_tmp14D != 0;_tmp14D=_tmp14D->tl){Cyc_Tcpat_check_pat_regions(
te,(struct Cyc_Absyn_Pat*)_tmp14D->hd);}{void*_tmp15F=(void*)((struct Cyc_Core_Opt*)
_check_null(p->topt))->v;struct Cyc_Absyn_DatatypeInfo _tmp160;struct Cyc_Core_Opt*
_tmp161;struct Cyc_Absyn_DatatypeInfo _tmp162;struct Cyc_Core_Opt*_tmp163;struct Cyc_Core_Opt
_tmp164;void*_tmp165;_LLA1: if(_tmp15F <= (void*)4)goto _LLA7;if(*((int*)_tmp15F)!= 
2)goto _LLA3;_tmp160=((struct Cyc_Absyn_DatatypeType_struct*)_tmp15F)->f1;_tmp161=
_tmp160.rgn;if(_tmp161 != 0)goto _LLA3;_LLA2: return;_LLA3: if(*((int*)_tmp15F)!= 2)
goto _LLA5;_tmp162=((struct Cyc_Absyn_DatatypeType_struct*)_tmp15F)->f1;_tmp163=
_tmp162.rgn;if(_tmp163 == 0)goto _LLA5;_tmp164=*_tmp163;_tmp165=(void*)_tmp164.v;
_LLA4: Cyc_Tcenv_check_rgn_accessible(te,p->loc,_tmp165);return;_LLA5: if(*((int*)
_tmp15F)!= 3)goto _LLA7;_LLA6: return;_LLA7:;_LLA8:({void*_tmp166=0;((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmp167="check_pat_regions: bad datatype type";_tag_dyneither(_tmp167,sizeof(
char),37);}),_tag_dyneither(_tmp166,sizeof(void*),0));});_LLA0:;}_LL93: if(*((int*)
_tmp14B)!= 5)goto _LL95;_tmp14E=((struct Cyc_Absyn_Aggr_p_struct*)_tmp14B)->f3;
_LL94: for(0;_tmp14E != 0;_tmp14E=_tmp14E->tl){Cyc_Tcpat_check_pat_regions(te,(*((
struct _tuple8*)_tmp14E->hd)).f2);}return;_LL95: if(*((int*)_tmp14B)!= 3)goto _LL97;
_tmp14F=((struct Cyc_Absyn_Tuple_p_struct*)_tmp14B)->f1;_LL96: for(0;_tmp14F != 0;
_tmp14F=_tmp14F->tl){Cyc_Tcpat_check_pat_regions(te,(struct Cyc_Absyn_Pat*)
_tmp14F->hd);}return;_LL97:;_LL98: return;_LL8E:;}struct Cyc_Tcpat_Name_v_struct{
int tag;struct _dyneither_ptr f1;};struct Cyc_Tcpat_Int_v_struct{int tag;int f1;};
union Cyc_Tcpat_Name_value_union{struct Cyc_Tcpat_Name_v_struct Name_v;struct Cyc_Tcpat_Int_v_struct
Int_v;};struct Cyc_Tcpat_Con_s{union Cyc_Tcpat_Name_value_union name;int arity;int*
span;struct Cyc_Absyn_Pat*orig_pat;};struct Cyc_Tcpat_Con_struct{int tag;struct Cyc_Tcpat_Con_s*
f1;struct Cyc_List_List*f2;};static int Cyc_Tcpat_compare_con(struct Cyc_Tcpat_Con_s*
c1,struct Cyc_Tcpat_Con_s*c2){union Cyc_Tcpat_Name_value_union _tmp168=c1->name;
struct _dyneither_ptr _tmp169;int _tmp16A;_LLAA: if((_tmp168.Name_v).tag != 0)goto
_LLAC;_tmp169=(_tmp168.Name_v).f1;_LLAB: {union Cyc_Tcpat_Name_value_union _tmp16B=
c2->name;struct _dyneither_ptr _tmp16C;_LLAF: if((_tmp16B.Name_v).tag != 0)goto _LLB1;
_tmp16C=(_tmp16B.Name_v).f1;_LLB0: return Cyc_strcmp((struct _dyneither_ptr)_tmp169,(
struct _dyneither_ptr)_tmp16C);_LLB1: if((_tmp16B.Int_v).tag != 1)goto _LLAE;_LLB2:
return - 1;_LLAE:;}_LLAC: if((_tmp168.Int_v).tag != 1)goto _LLA9;_tmp16A=(_tmp168.Int_v).f1;
_LLAD: {union Cyc_Tcpat_Name_value_union _tmp16D=c2->name;int _tmp16E;_LLB4: if((
_tmp16D.Name_v).tag != 0)goto _LLB6;_LLB5: return 1;_LLB6: if((_tmp16D.Int_v).tag != 1)
goto _LLB3;_tmp16E=(_tmp16D.Int_v).f1;_LLB7: return _tmp16A - _tmp16E;_LLB3:;}_LLA9:;}
static struct Cyc_Set_Set*Cyc_Tcpat_empty_con_set(struct _RegionHandle*r){return((
struct Cyc_Set_Set*(*)(struct _RegionHandle*r,int(*cmp)(struct Cyc_Tcpat_Con_s*,
struct Cyc_Tcpat_Con_s*)))Cyc_Set_rempty)(r,Cyc_Tcpat_compare_con);}static int Cyc_Tcpat_one_opt=
1;static int Cyc_Tcpat_two_opt=2;static int Cyc_Tcpat_twofiftysix_opt=256;static
struct Cyc_Tcpat_Con_s*Cyc_Tcpat_null_con(struct _RegionHandle*r,struct Cyc_Absyn_Pat*
p){return({struct Cyc_Tcpat_Con_s*_tmp16F=_region_malloc(r,sizeof(*_tmp16F));
_tmp16F->name=(union Cyc_Tcpat_Name_value_union)({union Cyc_Tcpat_Name_value_union
_tmp170;(_tmp170.Name_v).tag=0;(_tmp170.Name_v).f1=({const char*_tmp171="NULL";
_tag_dyneither(_tmp171,sizeof(char),5);});_tmp170;});_tmp16F->arity=0;_tmp16F->span=(
int*)& Cyc_Tcpat_two_opt;_tmp16F->orig_pat=(struct Cyc_Absyn_Pat*)p;_tmp16F;});}
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_null_ptr_con(struct _RegionHandle*r,struct
Cyc_Absyn_Pat*p){return({struct Cyc_Tcpat_Con_s*_tmp172=_region_malloc(r,sizeof(*
_tmp172));_tmp172->name=(union Cyc_Tcpat_Name_value_union)({union Cyc_Tcpat_Name_value_union
_tmp173;(_tmp173.Name_v).tag=0;(_tmp173.Name_v).f1=({const char*_tmp174="&";
_tag_dyneither(_tmp174,sizeof(char),2);});_tmp173;});_tmp172->arity=1;_tmp172->span=(
int*)& Cyc_Tcpat_two_opt;_tmp172->orig_pat=(struct Cyc_Absyn_Pat*)p;_tmp172;});}
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_ptr_con(struct _RegionHandle*r,struct Cyc_Absyn_Pat*
p){return({struct Cyc_Tcpat_Con_s*_tmp175=_region_malloc(r,sizeof(*_tmp175));
_tmp175->name=(union Cyc_Tcpat_Name_value_union)({union Cyc_Tcpat_Name_value_union
_tmp176;(_tmp176.Name_v).tag=0;(_tmp176.Name_v).f1=({const char*_tmp177="&";
_tag_dyneither(_tmp177,sizeof(char),2);});_tmp176;});_tmp175->arity=1;_tmp175->span=(
int*)& Cyc_Tcpat_one_opt;_tmp175->orig_pat=(struct Cyc_Absyn_Pat*)p;_tmp175;});}
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_int_con(struct _RegionHandle*r,int i,struct
Cyc_Absyn_Pat*p){return({struct Cyc_Tcpat_Con_s*_tmp178=_region_malloc(r,sizeof(*
_tmp178));_tmp178->name=(union Cyc_Tcpat_Name_value_union)({union Cyc_Tcpat_Name_value_union
_tmp179;(_tmp179.Int_v).tag=1;(_tmp179.Int_v).f1=i;_tmp179;});_tmp178->arity=0;
_tmp178->span=0;_tmp178->orig_pat=p;_tmp178;});}static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_float_con(
struct _RegionHandle*r,struct _dyneither_ptr f,struct Cyc_Absyn_Pat*p){return({
struct Cyc_Tcpat_Con_s*_tmp17A=_region_malloc(r,sizeof(*_tmp17A));_tmp17A->name=(
union Cyc_Tcpat_Name_value_union)({union Cyc_Tcpat_Name_value_union _tmp17B;(
_tmp17B.Name_v).tag=0;(_tmp17B.Name_v).f1=f;_tmp17B;});_tmp17A->arity=0;_tmp17A->span=
0;_tmp17A->orig_pat=(struct Cyc_Absyn_Pat*)p;_tmp17A;});}static struct Cyc_Tcpat_Con_s*
Cyc_Tcpat_char_con(struct _RegionHandle*r,char c,struct Cyc_Absyn_Pat*p){return({
struct Cyc_Tcpat_Con_s*_tmp17C=_region_malloc(r,sizeof(*_tmp17C));_tmp17C->name=(
union Cyc_Tcpat_Name_value_union)({union Cyc_Tcpat_Name_value_union _tmp17D;(
_tmp17D.Int_v).tag=1;(_tmp17D.Int_v).f1=(int)c;_tmp17D;});_tmp17C->arity=0;
_tmp17C->span=(int*)& Cyc_Tcpat_twofiftysix_opt;_tmp17C->orig_pat=(struct Cyc_Absyn_Pat*)
p;_tmp17C;});}static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_tuple_con(struct
_RegionHandle*r,int i,struct Cyc_Absyn_Pat*p){return({struct Cyc_Tcpat_Con_s*
_tmp17E=_region_malloc(r,sizeof(*_tmp17E));_tmp17E->name=(union Cyc_Tcpat_Name_value_union)({
union Cyc_Tcpat_Name_value_union _tmp17F;(_tmp17F.Name_v).tag=0;(_tmp17F.Name_v).f1=({
const char*_tmp180="$";_tag_dyneither(_tmp180,sizeof(char),2);});_tmp17F;});
_tmp17E->arity=i;_tmp17E->span=(int*)& Cyc_Tcpat_one_opt;_tmp17E->orig_pat=p;
_tmp17E;});}static void*Cyc_Tcpat_null_pat(struct _RegionHandle*r,struct Cyc_Absyn_Pat*
p){return(void*)({struct Cyc_Tcpat_Con_struct*_tmp181=_region_malloc(r,sizeof(*
_tmp181));_tmp181[0]=({struct Cyc_Tcpat_Con_struct _tmp182;_tmp182.tag=0;_tmp182.f1=
Cyc_Tcpat_null_con(r,p);_tmp182.f2=0;_tmp182;});_tmp181;});}static void*Cyc_Tcpat_int_pat(
struct _RegionHandle*r,int i,struct Cyc_Absyn_Pat*p){return(void*)({struct Cyc_Tcpat_Con_struct*
_tmp183=_region_malloc(r,sizeof(*_tmp183));_tmp183[0]=({struct Cyc_Tcpat_Con_struct
_tmp184;_tmp184.tag=0;_tmp184.f1=Cyc_Tcpat_int_con(r,i,p);_tmp184.f2=0;_tmp184;});
_tmp183;});}static void*Cyc_Tcpat_char_pat(struct _RegionHandle*r,char c,struct Cyc_Absyn_Pat*
p){return(void*)({struct Cyc_Tcpat_Con_struct*_tmp185=_region_malloc(r,sizeof(*
_tmp185));_tmp185[0]=({struct Cyc_Tcpat_Con_struct _tmp186;_tmp186.tag=0;_tmp186.f1=
Cyc_Tcpat_char_con(r,c,p);_tmp186.f2=0;_tmp186;});_tmp185;});}static void*Cyc_Tcpat_float_pat(
struct _RegionHandle*r,struct _dyneither_ptr f,struct Cyc_Absyn_Pat*p){return(void*)({
struct Cyc_Tcpat_Con_struct*_tmp187=_region_malloc(r,sizeof(*_tmp187));_tmp187[0]=({
struct Cyc_Tcpat_Con_struct _tmp188;_tmp188.tag=0;_tmp188.f1=Cyc_Tcpat_float_con(r,
f,p);_tmp188.f2=0;_tmp188;});_tmp187;});}static void*Cyc_Tcpat_null_ptr_pat(
struct _RegionHandle*r,void*p,struct Cyc_Absyn_Pat*p0){return(void*)({struct Cyc_Tcpat_Con_struct*
_tmp189=_region_malloc(r,sizeof(*_tmp189));_tmp189[0]=({struct Cyc_Tcpat_Con_struct
_tmp18A;_tmp18A.tag=0;_tmp18A.f1=Cyc_Tcpat_null_ptr_con(r,p0);_tmp18A.f2=({
struct Cyc_List_List*_tmp18B=_region_malloc(r,sizeof(*_tmp18B));_tmp18B->hd=(void*)
p;_tmp18B->tl=0;_tmp18B;});_tmp18A;});_tmp189;});}static void*Cyc_Tcpat_ptr_pat(
struct _RegionHandle*r,void*p,struct Cyc_Absyn_Pat*p0){return(void*)({struct Cyc_Tcpat_Con_struct*
_tmp18C=_region_malloc(r,sizeof(*_tmp18C));_tmp18C[0]=({struct Cyc_Tcpat_Con_struct
_tmp18D;_tmp18D.tag=0;_tmp18D.f1=Cyc_Tcpat_ptr_con(r,p0);_tmp18D.f2=({struct Cyc_List_List*
_tmp18E=_region_malloc(r,sizeof(*_tmp18E));_tmp18E->hd=(void*)p;_tmp18E->tl=0;
_tmp18E;});_tmp18D;});_tmp18C;});}static void*Cyc_Tcpat_tuple_pat(struct
_RegionHandle*r,struct Cyc_List_List*ss,struct Cyc_Absyn_Pat*p){return(void*)({
struct Cyc_Tcpat_Con_struct*_tmp18F=_region_malloc(r,sizeof(*_tmp18F));_tmp18F[0]=({
struct Cyc_Tcpat_Con_struct _tmp190;_tmp190.tag=0;_tmp190.f1=Cyc_Tcpat_tuple_con(r,
Cyc_List_length(ss),p);_tmp190.f2=ss;_tmp190;});_tmp18F;});}static void*Cyc_Tcpat_con_pat(
struct _RegionHandle*r,struct _dyneither_ptr con_name,int*span,struct Cyc_List_List*
ps,struct Cyc_Absyn_Pat*p){struct Cyc_Tcpat_Con_s*c=({struct Cyc_Tcpat_Con_s*
_tmp193=_region_malloc(r,sizeof(*_tmp193));_tmp193->name=(union Cyc_Tcpat_Name_value_union)({
union Cyc_Tcpat_Name_value_union _tmp194;(_tmp194.Name_v).tag=0;(_tmp194.Name_v).f1=
con_name;_tmp194;});_tmp193->arity=Cyc_List_length(ps);_tmp193->span=span;
_tmp193->orig_pat=(struct Cyc_Absyn_Pat*)p;_tmp193;});return(void*)({struct Cyc_Tcpat_Con_struct*
_tmp191=_region_malloc(r,sizeof(*_tmp191));_tmp191[0]=({struct Cyc_Tcpat_Con_struct
_tmp192;_tmp192.tag=0;_tmp192.f1=c;_tmp192.f2=ps;_tmp192;});_tmp191;});}static
void*Cyc_Tcpat_compile_pat(struct _RegionHandle*r,struct Cyc_Absyn_Pat*p){void*s;{
void*_tmp195=(void*)p->r;void*_tmp196;int _tmp197;char _tmp198;struct
_dyneither_ptr _tmp199;struct Cyc_Absyn_Pat*_tmp19A;struct Cyc_Absyn_Pat*_tmp19B;
struct Cyc_Absyn_Pat*_tmp19C;struct Cyc_Absyn_Datatypedecl*_tmp19D;struct Cyc_Absyn_Datatypefield*
_tmp19E;struct Cyc_List_List*_tmp19F;struct Cyc_List_List*_tmp1A0;struct Cyc_Absyn_AggrInfo
_tmp1A1;union Cyc_Absyn_AggrInfoU_union _tmp1A2;struct Cyc_Absyn_Aggrdecl**_tmp1A3;
struct Cyc_Absyn_Aggrdecl*_tmp1A4;struct Cyc_List_List*_tmp1A5;struct Cyc_Absyn_Enumdecl*
_tmp1A6;struct Cyc_Absyn_Enumfield*_tmp1A7;void*_tmp1A8;struct Cyc_Absyn_Enumfield*
_tmp1A9;_LLB9: if((int)_tmp195 != 0)goto _LLBB;_LLBA: goto _LLBC;_LLBB: if(_tmp195 <= (
void*)2)goto _LLBD;if(*((int*)_tmp195)!= 2)goto _LLBD;_LLBC: s=(void*)0;goto _LLB8;
_LLBD: if((int)_tmp195 != 1)goto _LLBF;_LLBE: s=Cyc_Tcpat_null_pat(r,p);goto _LLB8;
_LLBF: if(_tmp195 <= (void*)2)goto _LLC1;if(*((int*)_tmp195)!= 7)goto _LLC1;_tmp196=(
void*)((struct Cyc_Absyn_Int_p_struct*)_tmp195)->f1;_tmp197=((struct Cyc_Absyn_Int_p_struct*)
_tmp195)->f2;_LLC0: s=Cyc_Tcpat_int_pat(r,_tmp197,(struct Cyc_Absyn_Pat*)p);goto
_LLB8;_LLC1: if(_tmp195 <= (void*)2)goto _LLC3;if(*((int*)_tmp195)!= 8)goto _LLC3;
_tmp198=((struct Cyc_Absyn_Char_p_struct*)_tmp195)->f1;_LLC2: s=Cyc_Tcpat_char_pat(
r,_tmp198,p);goto _LLB8;_LLC3: if(_tmp195 <= (void*)2)goto _LLC5;if(*((int*)_tmp195)
!= 9)goto _LLC5;_tmp199=((struct Cyc_Absyn_Float_p_struct*)_tmp195)->f1;_LLC4: s=
Cyc_Tcpat_float_pat(r,_tmp199,p);goto _LLB8;_LLC5: if(_tmp195 <= (void*)2)goto _LLC7;
if(*((int*)_tmp195)!= 0)goto _LLC7;_tmp19A=((struct Cyc_Absyn_Var_p_struct*)
_tmp195)->f2;_LLC6: s=Cyc_Tcpat_compile_pat(r,_tmp19A);goto _LLB8;_LLC7: if(_tmp195
<= (void*)2)goto _LLC9;if(*((int*)_tmp195)!= 1)goto _LLC9;_tmp19B=((struct Cyc_Absyn_Reference_p_struct*)
_tmp195)->f2;_LLC8: s=Cyc_Tcpat_compile_pat(r,_tmp19B);goto _LLB8;_LLC9: if(_tmp195
<= (void*)2)goto _LLCB;if(*((int*)_tmp195)!= 4)goto _LLCB;_tmp19C=((struct Cyc_Absyn_Pointer_p_struct*)
_tmp195)->f1;_LLCA:{void*_tmp1AA=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(p->topt))->v);struct Cyc_Absyn_PtrInfo _tmp1AB;struct Cyc_Absyn_PtrAtts
_tmp1AC;struct Cyc_Absyn_Conref*_tmp1AD;struct Cyc_Absyn_DatatypeInfo _tmp1AE;
struct Cyc_Core_Opt*_tmp1AF;struct Cyc_Core_Opt _tmp1B0;_LLDE: if(_tmp1AA <= (void*)4)
goto _LLE2;if(*((int*)_tmp1AA)!= 4)goto _LLE0;_tmp1AB=((struct Cyc_Absyn_PointerType_struct*)
_tmp1AA)->f1;_tmp1AC=_tmp1AB.ptr_atts;_tmp1AD=_tmp1AC.nullable;_LLDF: {int
is_nullable=0;int still_working=1;while(still_working){union Cyc_Absyn_Constraint_union
_tmp1B1=_tmp1AD->v;struct Cyc_Absyn_Conref*_tmp1B2;int _tmp1B3;_LLE5: if((_tmp1B1.Forward_constr).tag
!= 1)goto _LLE7;_tmp1B2=(_tmp1B1.Forward_constr).f1;_LLE6: _tmp1AD->v=_tmp1B2->v;
continue;_LLE7: if((_tmp1B1.No_constr).tag != 2)goto _LLE9;_LLE8: _tmp1AD->v=(union
Cyc_Absyn_Constraint_union)({union Cyc_Absyn_Constraint_union _tmp1B4;(_tmp1B4.Eq_constr).tag=
0;(_tmp1B4.Eq_constr).f1=(void*)0;_tmp1B4;});is_nullable=0;still_working=0;goto
_LLE4;_LLE9: if((_tmp1B1.Eq_constr).tag != 0)goto _LLE4;_tmp1B3=(_tmp1B1.Eq_constr).f1;
_LLEA: is_nullable=(int)_tmp1B3;still_working=0;goto _LLE4;_LLE4:;}{void*ss=Cyc_Tcpat_compile_pat(
r,_tmp19C);if(is_nullable)s=Cyc_Tcpat_null_ptr_pat(r,ss,p);else{s=Cyc_Tcpat_ptr_pat(
r,ss,p);}goto _LLDD;}}_LLE0: if(*((int*)_tmp1AA)!= 2)goto _LLE2;_tmp1AE=((struct Cyc_Absyn_DatatypeType_struct*)
_tmp1AA)->f1;_tmp1AF=_tmp1AE.rgn;if(_tmp1AF == 0)goto _LLE2;_tmp1B0=*_tmp1AF;_LLE1:{
void*_tmp1B5=(void*)_tmp19C->r;struct Cyc_Absyn_Datatypedecl*_tmp1B6;struct Cyc_Absyn_Datatypefield*
_tmp1B7;struct Cyc_List_List*_tmp1B8;_LLEC: if(_tmp1B5 <= (void*)2)goto _LLEE;if(*((
int*)_tmp1B5)!= 6)goto _LLEE;_tmp1B6=((struct Cyc_Absyn_Datatype_p_struct*)_tmp1B5)->f1;
_tmp1B7=((struct Cyc_Absyn_Datatype_p_struct*)_tmp1B5)->f2;_tmp1B8=((struct Cyc_Absyn_Datatype_p_struct*)
_tmp1B5)->f3;_LLED: {int*span;if(_tmp1B6->is_extensible)span=0;else{span=({int*
_tmp1B9=_region_malloc(r,sizeof(*_tmp1B9));_tmp1B9[0]=((int(*)(struct Cyc_List_List*
x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(
_tmp1B6->fields))->v);_tmp1B9;});}s=Cyc_Tcpat_con_pat(r,*(*_tmp1B7->name).f2,
span,((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct
_RegionHandle*,struct Cyc_Absyn_Pat*),struct _RegionHandle*env,struct Cyc_List_List*
x))Cyc_List_rmap_c)(r,Cyc_Tcpat_compile_pat,r,_tmp1B8),p);goto _LLEB;}_LLEE:;
_LLEF:({void*_tmp1BA=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(({const char*_tmp1BB="non-datatype pattern has datatype type";
_tag_dyneither(_tmp1BB,sizeof(char),39);}),_tag_dyneither(_tmp1BA,sizeof(void*),
0));});_LLEB:;}goto _LLDD;_LLE2:;_LLE3:({void*_tmp1BC=0;((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp1BD="pointer pattern does not have pointer type";
_tag_dyneither(_tmp1BD,sizeof(char),43);}),_tag_dyneither(_tmp1BC,sizeof(void*),
0));});_LLDD:;}goto _LLB8;_LLCB: if(_tmp195 <= (void*)2)goto _LLCD;if(*((int*)
_tmp195)!= 6)goto _LLCD;_tmp19D=((struct Cyc_Absyn_Datatype_p_struct*)_tmp195)->f1;
_tmp19E=((struct Cyc_Absyn_Datatype_p_struct*)_tmp195)->f2;_tmp19F=((struct Cyc_Absyn_Datatype_p_struct*)
_tmp195)->f3;_LLCC: {int*span;{void*_tmp1BE=Cyc_Tcutil_compress((void*)((struct
Cyc_Core_Opt*)_check_null(p->topt))->v);_LLF1: if(_tmp1BE <= (void*)4)goto _LLF5;
if(*((int*)_tmp1BE)!= 2)goto _LLF3;_LLF2: if(_tmp19D->is_extensible)span=0;else{
span=({int*_tmp1BF=_region_malloc(r,sizeof(*_tmp1BF));_tmp1BF[0]=((int(*)(struct
Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp19D->fields))->v);_tmp1BF;});}goto _LLF0;_LLF3: if(*((int*)_tmp1BE)
!= 3)goto _LLF5;_LLF4: span=(int*)& Cyc_Tcpat_one_opt;goto _LLF0;_LLF5:;_LLF6: span=({
void*_tmp1C0=0;((int*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp1C1="void datatype pattern has bad type";_tag_dyneither(_tmp1C1,
sizeof(char),35);}),_tag_dyneither(_tmp1C0,sizeof(void*),0));});goto _LLF0;_LLF0:;}
s=Cyc_Tcpat_con_pat(r,*(*_tmp19E->name).f2,span,((struct Cyc_List_List*(*)(struct
_RegionHandle*,void*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Pat*),struct
_RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcpat_compile_pat,
r,_tmp19F),p);goto _LLB8;}_LLCD: if(_tmp195 <= (void*)2)goto _LLCF;if(*((int*)
_tmp195)!= 3)goto _LLCF;_tmp1A0=((struct Cyc_Absyn_Tuple_p_struct*)_tmp195)->f1;
_LLCE: s=Cyc_Tcpat_tuple_pat(r,((struct Cyc_List_List*(*)(struct _RegionHandle*,
void*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Pat*),struct _RegionHandle*env,
struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcpat_compile_pat,r,_tmp1A0),(
struct Cyc_Absyn_Pat*)p);goto _LLB8;_LLCF: if(_tmp195 <= (void*)2)goto _LLD1;if(*((
int*)_tmp195)!= 5)goto _LLD1;_tmp1A1=((struct Cyc_Absyn_Aggr_p_struct*)_tmp195)->f1;
_tmp1A2=_tmp1A1.aggr_info;if((((((struct Cyc_Absyn_Aggr_p_struct*)_tmp195)->f1).aggr_info).KnownAggr).tag
!= 1)goto _LLD1;_tmp1A3=(_tmp1A2.KnownAggr).f1;_tmp1A4=*_tmp1A3;_tmp1A5=((struct
Cyc_Absyn_Aggr_p_struct*)_tmp195)->f3;_LLD0: if((void*)_tmp1A4->kind == (void*)0){
struct Cyc_List_List*ps=0;{struct Cyc_List_List*fields=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp1A4->impl))->fields;for(0;fields != 0;fields=fields->tl){int found=
Cyc_strcmp((struct _dyneither_ptr)*((struct Cyc_Absyn_Aggrfield*)fields->hd)->name,({
const char*_tmp1D0="";_tag_dyneither(_tmp1D0,sizeof(char),1);}))== 0;{struct Cyc_List_List*
dlps0=_tmp1A5;for(0;!found  && dlps0 != 0;dlps0=dlps0->tl){struct _tuple8 _tmp1C3;
struct Cyc_List_List*_tmp1C4;struct Cyc_Absyn_Pat*_tmp1C5;struct _tuple8*_tmp1C2=(
struct _tuple8*)dlps0->hd;_tmp1C3=*_tmp1C2;_tmp1C4=_tmp1C3.f1;_tmp1C5=_tmp1C3.f2;{
struct Cyc_List_List*_tmp1C6=_tmp1C4;struct Cyc_List_List _tmp1C7;void*_tmp1C8;
struct _dyneither_ptr*_tmp1C9;struct Cyc_List_List*_tmp1CA;_LLF8: if(_tmp1C6 == 0)
goto _LLFA;_tmp1C7=*_tmp1C6;_tmp1C8=(void*)_tmp1C7.hd;if(*((int*)_tmp1C8)!= 1)
goto _LLFA;_tmp1C9=((struct Cyc_Absyn_FieldName_struct*)_tmp1C8)->f1;_tmp1CA=
_tmp1C7.tl;if(_tmp1CA != 0)goto _LLFA;_LLF9: if(Cyc_strptrcmp(_tmp1C9,((struct Cyc_Absyn_Aggrfield*)
fields->hd)->name)== 0){ps=({struct Cyc_List_List*_tmp1CB=_region_malloc(r,
sizeof(*_tmp1CB));_tmp1CB->hd=(void*)Cyc_Tcpat_compile_pat(r,_tmp1C5);_tmp1CB->tl=
ps;_tmp1CB;});found=1;}goto _LLF7;_LLFA:;_LLFB:({void*_tmp1CC=0;((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp1CD="bad designator(s)";
_tag_dyneither(_tmp1CD,sizeof(char),18);}),_tag_dyneither(_tmp1CC,sizeof(void*),
0));});_LLF7:;}}}if(!found)({void*_tmp1CE=0;((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp1CF="bad designator";
_tag_dyneither(_tmp1CF,sizeof(char),15);}),_tag_dyneither(_tmp1CE,sizeof(void*),
0));});}}s=Cyc_Tcpat_tuple_pat(r,ps,(struct Cyc_Absyn_Pat*)p);}else{if(!((struct
Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp1A4->impl))->tagged)({void*_tmp1D1=0;Cyc_Tcutil_terr(
p->loc,({const char*_tmp1D2="patterns on untagged unions not yet supported.";
_tag_dyneither(_tmp1D2,sizeof(char),47);}),_tag_dyneither(_tmp1D1,sizeof(void*),
0));});{int*span=({int*_tmp1E1=_region_malloc(r,sizeof(*_tmp1E1));_tmp1E1[0]=((
int(*)(struct Cyc_List_List*x))Cyc_List_length)(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp1A4->impl))->fields);_tmp1E1;});int field_name;struct Cyc_List_List*
_tmp1D3=_tmp1A5;struct Cyc_List_List _tmp1D4;struct _tuple8*_tmp1D5;struct _tuple8
_tmp1D6;struct Cyc_List_List*_tmp1D7;struct Cyc_List_List _tmp1D8;void*_tmp1D9;
struct _dyneither_ptr*_tmp1DA;struct Cyc_List_List*_tmp1DB;struct Cyc_Absyn_Pat*
_tmp1DC;struct Cyc_List_List*_tmp1DD;_LLFD: if(_tmp1D3 == 0)goto _LLFF;_tmp1D4=*
_tmp1D3;_tmp1D5=(struct _tuple8*)_tmp1D4.hd;_tmp1D6=*_tmp1D5;_tmp1D7=_tmp1D6.f1;
if(_tmp1D7 == 0)goto _LLFF;_tmp1D8=*_tmp1D7;_tmp1D9=(void*)_tmp1D8.hd;if(*((int*)
_tmp1D9)!= 1)goto _LLFF;_tmp1DA=((struct Cyc_Absyn_FieldName_struct*)_tmp1D9)->f1;
_tmp1DB=_tmp1D8.tl;if(_tmp1DB != 0)goto _LLFF;_tmp1DC=_tmp1D6.f2;_tmp1DD=_tmp1D4.tl;
if(_tmp1DD != 0)goto _LLFF;_LLFE: s=Cyc_Tcpat_con_pat(r,*_tmp1DA,span,({struct Cyc_List_List*
_tmp1DE=_region_malloc(r,sizeof(*_tmp1DE));_tmp1DE->hd=(void*)Cyc_Tcpat_compile_pat(
r,_tmp1DC);_tmp1DE->tl=0;_tmp1DE;}),p);goto _LLFC;_LLFF:;_LL100:({void*_tmp1DF=0;((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const
char*_tmp1E0="bad union pattern";_tag_dyneither(_tmp1E0,sizeof(char),18);}),
_tag_dyneither(_tmp1DF,sizeof(void*),0));});_LLFC:;}}goto _LLB8;_LLD1: if(_tmp195
<= (void*)2)goto _LLD3;if(*((int*)_tmp195)!= 10)goto _LLD3;_tmp1A6=((struct Cyc_Absyn_Enum_p_struct*)
_tmp195)->f1;_tmp1A7=((struct Cyc_Absyn_Enum_p_struct*)_tmp195)->f2;_LLD2: {int
span=((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp1A6->fields))->v);s=Cyc_Tcpat_con_pat(r,*(*
_tmp1A7->name).f2,({int*_tmp1E2=_region_malloc(r,sizeof(*_tmp1E2));_tmp1E2[0]=
span;_tmp1E2;}),0,p);goto _LLB8;}_LLD3: if(_tmp195 <= (void*)2)goto _LLD5;if(*((int*)
_tmp195)!= 11)goto _LLD5;_tmp1A8=(void*)((struct Cyc_Absyn_AnonEnum_p_struct*)
_tmp195)->f1;_tmp1A9=((struct Cyc_Absyn_AnonEnum_p_struct*)_tmp195)->f2;_LLD4: {
struct Cyc_List_List*fields;{void*_tmp1E3=Cyc_Tcutil_compress(_tmp1A8);struct Cyc_List_List*
_tmp1E4;_LL102: if(_tmp1E3 <= (void*)4)goto _LL104;if(*((int*)_tmp1E3)!= 13)goto
_LL104;_tmp1E4=((struct Cyc_Absyn_AnonEnumType_struct*)_tmp1E3)->f1;_LL103: fields=
_tmp1E4;goto _LL101;_LL104:;_LL105:({void*_tmp1E5=0;((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp1E6="bad type in AnonEnum_p";
_tag_dyneither(_tmp1E6,sizeof(char),23);}),_tag_dyneither(_tmp1E5,sizeof(void*),
0));});_LL101:;}{int span=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(fields);
s=Cyc_Tcpat_con_pat(r,*(*_tmp1A9->name).f2,({int*_tmp1E7=_region_malloc(r,
sizeof(*_tmp1E7));_tmp1E7[0]=span;_tmp1E7;}),0,p);goto _LLB8;}}_LLD5: if(_tmp195 <= (
void*)2)goto _LLD7;if(*((int*)_tmp195)!= 12)goto _LLD7;_LLD6: goto _LLD8;_LLD7: if(
_tmp195 <= (void*)2)goto _LLD9;if(*((int*)_tmp195)!= 13)goto _LLD9;_LLD8: goto _LLDA;
_LLD9: if(_tmp195 <= (void*)2)goto _LLDB;if(*((int*)_tmp195)!= 5)goto _LLDB;_LLDA:
goto _LLDC;_LLDB: if(_tmp195 <= (void*)2)goto _LLB8;if(*((int*)_tmp195)!= 14)goto
_LLB8;_LLDC: s=(void*)0;_LLB8:;}return s;}struct Cyc_Tcpat_Pos_struct{int tag;struct
Cyc_Tcpat_Con_s*f1;struct Cyc_List_List*f2;};struct Cyc_Tcpat_Neg_struct{int tag;
struct Cyc_Set_Set*f1;};struct Cyc_Tcpat_Failure_struct{int tag;void*f1;};struct Cyc_Tcpat_Success_struct{
int tag;void*f1;};struct Cyc_Tcpat_IfEq_struct{int tag;struct Cyc_List_List*f1;
struct Cyc_Tcpat_Con_s*f2;void*f3;void*f4;};struct _tuple11{struct Cyc_List_List*f1;
struct Cyc_List_List*f2;struct Cyc_List_List*f3;};static void*Cyc_Tcpat_add_neg(
struct _RegionHandle*r,void*td,struct Cyc_Tcpat_Con_s*c){void*_tmp1E8=td;struct Cyc_Set_Set*
_tmp1E9;_LL107: if(*((int*)_tmp1E8)!= 1)goto _LL109;_tmp1E9=((struct Cyc_Tcpat_Neg_struct*)
_tmp1E8)->f1;_LL108: return(void*)({struct Cyc_Tcpat_Neg_struct*_tmp1EA=
_region_malloc(r,sizeof(*_tmp1EA));_tmp1EA[0]=({struct Cyc_Tcpat_Neg_struct
_tmp1EB;_tmp1EB.tag=1;_tmp1EB.f1=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,
struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_rinsert)(r,_tmp1E9,c);
_tmp1EB;});_tmp1EA;});_LL109: if(*((int*)_tmp1E8)!= 0)goto _LL106;_LL10A:({void*
_tmp1EC=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp1ED="add_neg called when td is Positive";_tag_dyneither(_tmp1ED,
sizeof(char),35);}),_tag_dyneither(_tmp1EC,sizeof(void*),0));});_LL106:;}static
void*Cyc_Tcpat_static_match(struct Cyc_Tcpat_Con_s*c,void*td){void*_tmp1EE=td;
struct Cyc_Tcpat_Con_s*_tmp1EF;struct Cyc_Set_Set*_tmp1F0;_LL10C: if(*((int*)
_tmp1EE)!= 0)goto _LL10E;_tmp1EF=((struct Cyc_Tcpat_Pos_struct*)_tmp1EE)->f1;
_LL10D: if(Cyc_Tcpat_compare_con(c,_tmp1EF)== 0)return(void*)0;else{return(void*)
1;}_LL10E: if(*((int*)_tmp1EE)!= 1)goto _LL10B;_tmp1F0=((struct Cyc_Tcpat_Neg_struct*)
_tmp1EE)->f1;_LL10F: if(((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))
Cyc_Set_member)(_tmp1F0,c))return(void*)1;else{if(c->span != 0  && *((int*)
_check_null(c->span))== ((int(*)(struct Cyc_Set_Set*s))Cyc_Set_cardinality)(
_tmp1F0)+ 1)return(void*)0;else{return(void*)2;}}_LL10B:;}struct _tuple12{struct
Cyc_Tcpat_Con_s*f1;struct Cyc_List_List*f2;};static struct Cyc_List_List*Cyc_Tcpat_augment(
struct _RegionHandle*r,struct Cyc_List_List*ctxt,void*dsc){struct Cyc_List_List*
_tmp1F1=ctxt;struct Cyc_List_List _tmp1F2;struct _tuple12*_tmp1F3;struct _tuple12
_tmp1F4;struct Cyc_Tcpat_Con_s*_tmp1F5;struct Cyc_List_List*_tmp1F6;struct Cyc_List_List*
_tmp1F7;_LL111: if(_tmp1F1 != 0)goto _LL113;_LL112: return 0;_LL113: if(_tmp1F1 == 0)
goto _LL110;_tmp1F2=*_tmp1F1;_tmp1F3=(struct _tuple12*)_tmp1F2.hd;_tmp1F4=*_tmp1F3;
_tmp1F5=_tmp1F4.f1;_tmp1F6=_tmp1F4.f2;_tmp1F7=_tmp1F2.tl;_LL114: return({struct
Cyc_List_List*_tmp1F8=_region_malloc(r,sizeof(*_tmp1F8));_tmp1F8->hd=({struct
_tuple12*_tmp1F9=_region_malloc(r,sizeof(*_tmp1F9));_tmp1F9->f1=_tmp1F5;_tmp1F9->f2=(
struct Cyc_List_List*)({struct Cyc_List_List*_tmp1FA=_region_malloc(r,sizeof(*
_tmp1FA));_tmp1FA->hd=(void*)dsc;_tmp1FA->tl=_tmp1F6;_tmp1FA;});_tmp1F9;});
_tmp1F8->tl=_tmp1F7;_tmp1F8;});_LL110:;}static struct Cyc_List_List*Cyc_Tcpat_norm_context(
struct _RegionHandle*r,struct Cyc_List_List*ctxt){struct Cyc_List_List*_tmp1FB=ctxt;
struct Cyc_List_List _tmp1FC;struct _tuple12*_tmp1FD;struct _tuple12 _tmp1FE;struct
Cyc_Tcpat_Con_s*_tmp1FF;struct Cyc_List_List*_tmp200;struct Cyc_List_List*_tmp201;
_LL116: if(_tmp1FB != 0)goto _LL118;_LL117:({void*_tmp202=0;((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp203="norm_context: empty context";
_tag_dyneither(_tmp203,sizeof(char),28);}),_tag_dyneither(_tmp202,sizeof(void*),
0));});_LL118: if(_tmp1FB == 0)goto _LL115;_tmp1FC=*_tmp1FB;_tmp1FD=(struct _tuple12*)
_tmp1FC.hd;_tmp1FE=*_tmp1FD;_tmp1FF=_tmp1FE.f1;_tmp200=_tmp1FE.f2;_tmp201=
_tmp1FC.tl;_LL119: return Cyc_Tcpat_augment(r,_tmp201,(void*)({struct Cyc_Tcpat_Pos_struct*
_tmp204=_region_malloc(r,sizeof(*_tmp204));_tmp204[0]=({struct Cyc_Tcpat_Pos_struct
_tmp205;_tmp205.tag=0;_tmp205.f1=_tmp1FF;_tmp205.f2=Cyc_List_rrev(r,_tmp200);
_tmp205;});_tmp204;}));_LL115:;}static void*Cyc_Tcpat_build_desc(struct
_RegionHandle*r,struct Cyc_List_List*ctxt,void*dsc,struct Cyc_List_List*work){
struct _tuple6 _tmp207=({struct _tuple6 _tmp206;_tmp206.f1=ctxt;_tmp206.f2=work;
_tmp206;});struct Cyc_List_List*_tmp208;struct Cyc_List_List*_tmp209;struct Cyc_List_List*
_tmp20A;struct Cyc_List_List*_tmp20B;struct Cyc_List_List*_tmp20C;struct Cyc_List_List
_tmp20D;struct _tuple12*_tmp20E;struct _tuple12 _tmp20F;struct Cyc_Tcpat_Con_s*
_tmp210;struct Cyc_List_List*_tmp211;struct Cyc_List_List*_tmp212;struct Cyc_List_List*
_tmp213;struct Cyc_List_List _tmp214;struct _tuple11*_tmp215;struct _tuple11 _tmp216;
struct Cyc_List_List*_tmp217;struct Cyc_List_List*_tmp218;_LL11B: _tmp208=_tmp207.f1;
if(_tmp208 != 0)goto _LL11D;_tmp209=_tmp207.f2;if(_tmp209 != 0)goto _LL11D;_LL11C:
return dsc;_LL11D: _tmp20A=_tmp207.f1;if(_tmp20A != 0)goto _LL11F;_LL11E: goto _LL120;
_LL11F: _tmp20B=_tmp207.f2;if(_tmp20B != 0)goto _LL121;_LL120:({void*_tmp219=0;((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const
char*_tmp21A="build_desc: ctxt and work don't match";_tag_dyneither(_tmp21A,
sizeof(char),38);}),_tag_dyneither(_tmp219,sizeof(void*),0));});_LL121: _tmp20C=
_tmp207.f1;if(_tmp20C == 0)goto _LL11A;_tmp20D=*_tmp20C;_tmp20E=(struct _tuple12*)
_tmp20D.hd;_tmp20F=*_tmp20E;_tmp210=_tmp20F.f1;_tmp211=_tmp20F.f2;_tmp212=
_tmp20D.tl;_tmp213=_tmp207.f2;if(_tmp213 == 0)goto _LL11A;_tmp214=*_tmp213;_tmp215=(
struct _tuple11*)_tmp214.hd;_tmp216=*_tmp215;_tmp217=_tmp216.f3;_tmp218=_tmp214.tl;
_LL122: {struct Cyc_Tcpat_Pos_struct*_tmp21B=({struct Cyc_Tcpat_Pos_struct*_tmp21C=
_region_malloc(r,sizeof(*_tmp21C));_tmp21C[0]=({struct Cyc_Tcpat_Pos_struct
_tmp21D;_tmp21D.tag=0;_tmp21D.f1=_tmp210;_tmp21D.f2=Cyc_List_rappend(r,Cyc_List_rrev(
r,_tmp211),({struct Cyc_List_List*_tmp21E=_region_malloc(r,sizeof(*_tmp21E));
_tmp21E->hd=(void*)dsc;_tmp21E->tl=_tmp217;_tmp21E;}));_tmp21D;});_tmp21C;});
return Cyc_Tcpat_build_desc(r,_tmp212,(void*)_tmp21B,_tmp218);}_LL11A:;}static
void*Cyc_Tcpat_match(struct _RegionHandle*r,void*p,struct Cyc_List_List*obj,void*
dsc,struct Cyc_List_List*ctx,struct Cyc_List_List*work,void*right_hand_side,struct
Cyc_List_List*rules);static void*Cyc_Tcpat_or_match(struct _RegionHandle*r,void*
dsc,struct Cyc_List_List*allmrules){struct Cyc_List_List*_tmp21F=allmrules;struct
Cyc_List_List _tmp220;struct _tuple0*_tmp221;struct _tuple0 _tmp222;void*_tmp223;
void*_tmp224;struct Cyc_List_List*_tmp225;_LL124: if(_tmp21F != 0)goto _LL126;_LL125:
return(void*)({struct Cyc_Tcpat_Failure_struct*_tmp226=_cycalloc(sizeof(*_tmp226));
_tmp226[0]=({struct Cyc_Tcpat_Failure_struct _tmp227;_tmp227.tag=0;_tmp227.f1=(
void*)dsc;_tmp227;});_tmp226;});_LL126: if(_tmp21F == 0)goto _LL123;_tmp220=*
_tmp21F;_tmp221=(struct _tuple0*)_tmp220.hd;_tmp222=*_tmp221;_tmp223=_tmp222.f1;
_tmp224=_tmp222.f2;_tmp225=_tmp220.tl;_LL127: return Cyc_Tcpat_match(r,_tmp223,0,
dsc,0,0,_tmp224,_tmp225);_LL123:;}static void*Cyc_Tcpat_match_compile(struct
_RegionHandle*r,struct Cyc_List_List*allmrules){return Cyc_Tcpat_or_match(r,(void*)({
struct Cyc_Tcpat_Neg_struct*_tmp228=_region_malloc(r,sizeof(*_tmp228));_tmp228[0]=({
struct Cyc_Tcpat_Neg_struct _tmp229;_tmp229.tag=1;_tmp229.f1=Cyc_Tcpat_empty_con_set(
r);_tmp229;});_tmp228;}),allmrules);}static void*Cyc_Tcpat_and_match(struct
_RegionHandle*r,struct Cyc_List_List*ctx,struct Cyc_List_List*work,void*
right_hand_side,struct Cyc_List_List*rules){struct Cyc_List_List*_tmp22A=work;
struct Cyc_List_List _tmp22B;struct _tuple11*_tmp22C;struct _tuple11 _tmp22D;struct
Cyc_List_List*_tmp22E;struct Cyc_List_List*_tmp22F;struct Cyc_List_List*_tmp230;
struct Cyc_List_List*_tmp231;struct Cyc_List_List _tmp232;struct _tuple11*_tmp233;
struct _tuple11 _tmp234;struct Cyc_List_List*_tmp235;struct Cyc_List_List*_tmp236;
struct Cyc_List_List*_tmp237;struct Cyc_List_List*_tmp238;_LL129: if(_tmp22A != 0)
goto _LL12B;_LL12A: return(void*)({struct Cyc_Tcpat_Success_struct*_tmp239=
_region_malloc(r,sizeof(*_tmp239));_tmp239[0]=({struct Cyc_Tcpat_Success_struct
_tmp23A;_tmp23A.tag=1;_tmp23A.f1=(void*)right_hand_side;_tmp23A;});_tmp239;});
_LL12B: if(_tmp22A == 0)goto _LL12D;_tmp22B=*_tmp22A;_tmp22C=(struct _tuple11*)
_tmp22B.hd;_tmp22D=*_tmp22C;_tmp22E=_tmp22D.f1;if(_tmp22E != 0)goto _LL12D;_tmp22F=
_tmp22D.f2;if(_tmp22F != 0)goto _LL12D;_tmp230=_tmp22D.f3;if(_tmp230 != 0)goto
_LL12D;_tmp231=_tmp22B.tl;_LL12C: return Cyc_Tcpat_and_match(r,Cyc_Tcpat_norm_context(
r,ctx),_tmp231,right_hand_side,rules);_LL12D: if(_tmp22A == 0)goto _LL128;_tmp232=*
_tmp22A;_tmp233=(struct _tuple11*)_tmp232.hd;_tmp234=*_tmp233;_tmp235=_tmp234.f1;
_tmp236=_tmp234.f2;_tmp237=_tmp234.f3;_tmp238=_tmp232.tl;_LL12E: if((_tmp235 == 0
 || _tmp236 == 0) || _tmp237 == 0)({void*_tmp23B=0;Cyc_Tcutil_impos(({const char*
_tmp23C="tcpat:and_match: malformed work frame";_tag_dyneither(_tmp23C,sizeof(
char),38);}),_tag_dyneither(_tmp23B,sizeof(void*),0));});{struct Cyc_List_List
_tmp23E;void*_tmp23F;struct Cyc_List_List*_tmp240;struct Cyc_List_List*_tmp23D=(
struct Cyc_List_List*)_tmp235;_tmp23E=*_tmp23D;_tmp23F=(void*)_tmp23E.hd;_tmp240=
_tmp23E.tl;{struct Cyc_List_List _tmp242;struct Cyc_List_List*_tmp243;struct Cyc_List_List*
_tmp244;struct Cyc_List_List*_tmp241=(struct Cyc_List_List*)_tmp236;_tmp242=*
_tmp241;_tmp243=(struct Cyc_List_List*)_tmp242.hd;_tmp244=_tmp242.tl;{struct Cyc_List_List
_tmp246;void*_tmp247;struct Cyc_List_List*_tmp248;struct Cyc_List_List*_tmp245=(
struct Cyc_List_List*)_tmp237;_tmp246=*_tmp245;_tmp247=(void*)_tmp246.hd;_tmp248=
_tmp246.tl;{struct _tuple11*_tmp249=({struct _tuple11*_tmp24B=_region_malloc(r,
sizeof(*_tmp24B));_tmp24B->f1=_tmp240;_tmp24B->f2=_tmp244;_tmp24B->f3=_tmp248;
_tmp24B;});return Cyc_Tcpat_match(r,_tmp23F,_tmp243,_tmp247,ctx,({struct Cyc_List_List*
_tmp24A=_region_malloc(r,sizeof(*_tmp24A));_tmp24A->hd=_tmp249;_tmp24A->tl=
_tmp238;_tmp24A;}),right_hand_side,rules);}}}}_LL128:;}static struct Cyc_List_List*
Cyc_Tcpat_getdargs(struct _RegionHandle*r,struct Cyc_Tcpat_Con_s*pcon,void*dsc){
void*_tmp24C=dsc;struct Cyc_Set_Set*_tmp24D;struct Cyc_List_List*_tmp24E;_LL130:
if(*((int*)_tmp24C)!= 1)goto _LL132;_tmp24D=((struct Cyc_Tcpat_Neg_struct*)_tmp24C)->f1;
_LL131: {void*any=(void*)({struct Cyc_Tcpat_Neg_struct*_tmp251=_region_malloc(r,
sizeof(*_tmp251));_tmp251[0]=({struct Cyc_Tcpat_Neg_struct _tmp252;_tmp252.tag=1;
_tmp252.f1=Cyc_Tcpat_empty_con_set(r);_tmp252;});_tmp251;});struct Cyc_List_List*
_tmp24F=0;{int i=0;for(0;i < pcon->arity;++ i){_tmp24F=({struct Cyc_List_List*
_tmp250=_region_malloc(r,sizeof(*_tmp250));_tmp250->hd=(void*)any;_tmp250->tl=
_tmp24F;_tmp250;});}}return _tmp24F;}_LL132: if(*((int*)_tmp24C)!= 0)goto _LL12F;
_tmp24E=((struct Cyc_Tcpat_Pos_struct*)_tmp24C)->f2;_LL133: return _tmp24E;_LL12F:;}
struct _tuple13{struct _RegionHandle*f1;struct Cyc_List_List*f2;};static struct Cyc_List_List*
Cyc_Tcpat_getoarg(struct _tuple13*env,int i){struct _tuple13 _tmp254;struct
_RegionHandle*_tmp255;struct Cyc_List_List*_tmp256;struct _tuple13*_tmp253=env;
_tmp254=*_tmp253;_tmp255=_tmp254.f1;_tmp256=_tmp254.f2;return({struct Cyc_List_List*
_tmp257=_region_malloc(_tmp255,sizeof(*_tmp257));_tmp257->hd=(void*)(i + 1);
_tmp257->tl=_tmp256;_tmp257;});}static struct Cyc_List_List*Cyc_Tcpat_getoargs(
struct _RegionHandle*r,struct Cyc_Tcpat_Con_s*pcon,struct Cyc_List_List*obj){struct
_tuple13 _tmp258=({struct _tuple13 _tmp259;_tmp259.f1=r;_tmp259.f2=obj;_tmp259;});
return((struct Cyc_List_List*(*)(struct _RegionHandle*r,int n,struct Cyc_List_List*(*
f)(struct _tuple13*,int),struct _tuple13*env))Cyc_List_rtabulate_c)(r,pcon->arity,
Cyc_Tcpat_getoarg,& _tmp258);}static void*Cyc_Tcpat_match(struct _RegionHandle*r,
void*p,struct Cyc_List_List*obj,void*dsc,struct Cyc_List_List*ctx,struct Cyc_List_List*
work,void*right_hand_side,struct Cyc_List_List*rules){void*_tmp25A=p;struct Cyc_Tcpat_Con_s*
_tmp25B;struct Cyc_List_List*_tmp25C;_LL135: if((int)_tmp25A != 0)goto _LL137;_LL136:
return Cyc_Tcpat_and_match(r,Cyc_Tcpat_augment(r,ctx,dsc),work,right_hand_side,
rules);_LL137: if(_tmp25A <= (void*)1)goto _LL134;if(*((int*)_tmp25A)!= 0)goto
_LL134;_tmp25B=((struct Cyc_Tcpat_Con_struct*)_tmp25A)->f1;_tmp25C=((struct Cyc_Tcpat_Con_struct*)
_tmp25A)->f2;_LL138: {void*_tmp25D=Cyc_Tcpat_static_match(_tmp25B,dsc);_LL13A:
if((int)_tmp25D != 0)goto _LL13C;_LL13B: {struct Cyc_List_List*ctx2=({struct Cyc_List_List*
_tmp260=_region_malloc(r,sizeof(*_tmp260));_tmp260->hd=({struct _tuple12*_tmp261=
_region_malloc(r,sizeof(*_tmp261));_tmp261->f1=_tmp25B;_tmp261->f2=0;_tmp261;});
_tmp260->tl=ctx;_tmp260;});struct _tuple11*work_frame=({struct _tuple11*_tmp25F=
_region_malloc(r,sizeof(*_tmp25F));_tmp25F->f1=_tmp25C;_tmp25F->f2=Cyc_Tcpat_getoargs(
r,_tmp25B,obj);_tmp25F->f3=Cyc_Tcpat_getdargs(r,_tmp25B,dsc);_tmp25F;});struct
Cyc_List_List*work2=({struct Cyc_List_List*_tmp25E=_region_malloc(r,sizeof(*
_tmp25E));_tmp25E->hd=work_frame;_tmp25E->tl=work;_tmp25E;});return Cyc_Tcpat_and_match(
r,ctx2,work2,right_hand_side,rules);}_LL13C: if((int)_tmp25D != 1)goto _LL13E;
_LL13D: return Cyc_Tcpat_or_match(r,Cyc_Tcpat_build_desc(r,ctx,dsc,work),rules);
_LL13E: if((int)_tmp25D != 2)goto _LL139;_LL13F: {struct Cyc_List_List*ctx2=({struct
Cyc_List_List*_tmp268=_region_malloc(r,sizeof(*_tmp268));_tmp268->hd=({struct
_tuple12*_tmp269=_region_malloc(r,sizeof(*_tmp269));_tmp269->f1=_tmp25B;_tmp269->f2=
0;_tmp269;});_tmp268->tl=ctx;_tmp268;});struct _tuple11*work_frame=({struct
_tuple11*_tmp267=_region_malloc(r,sizeof(*_tmp267));_tmp267->f1=_tmp25C;_tmp267->f2=
Cyc_Tcpat_getoargs(r,_tmp25B,obj);_tmp267->f3=Cyc_Tcpat_getdargs(r,_tmp25B,dsc);
_tmp267;});struct Cyc_List_List*work2=({struct Cyc_List_List*_tmp266=
_region_malloc(r,sizeof(*_tmp266));_tmp266->hd=work_frame;_tmp266->tl=work;
_tmp266;});void*_tmp262=Cyc_Tcpat_and_match(r,ctx2,work2,right_hand_side,rules);
void*_tmp263=Cyc_Tcpat_or_match(r,Cyc_Tcpat_build_desc(r,ctx,Cyc_Tcpat_add_neg(r,
dsc,_tmp25B),work),rules);return(void*)({struct Cyc_Tcpat_IfEq_struct*_tmp264=
_region_malloc(r,sizeof(*_tmp264));_tmp264[0]=({struct Cyc_Tcpat_IfEq_struct
_tmp265;_tmp265.tag=2;_tmp265.f1=obj;_tmp265.f2=_tmp25B;_tmp265.f3=(void*)
_tmp262;_tmp265.f4=(void*)_tmp263;_tmp265;});_tmp264;});}_LL139:;}_LL134:;}
static void Cyc_Tcpat_check_exhaust_overlap(struct _RegionHandle*r,void*d,void(*
not_exhaust)(struct _RegionHandle*,void*,void*),void*env1,void(*rhs_appears)(void*,
void*),void*env2,int*exhaust_done){void*_tmp26A=d;void*_tmp26B;void*_tmp26C;void*
_tmp26D;void*_tmp26E;_LL141: if(*((int*)_tmp26A)!= 0)goto _LL143;_tmp26B=(void*)((
struct Cyc_Tcpat_Failure_struct*)_tmp26A)->f1;_LL142: if(!(*exhaust_done)){
not_exhaust(r,env1,_tmp26B);*exhaust_done=1;}goto _LL140;_LL143: if(*((int*)
_tmp26A)!= 1)goto _LL145;_tmp26C=(void*)((struct Cyc_Tcpat_Success_struct*)_tmp26A)->f1;
_LL144: rhs_appears(env2,_tmp26C);goto _LL140;_LL145: if(*((int*)_tmp26A)!= 2)goto
_LL140;_tmp26D=(void*)((struct Cyc_Tcpat_IfEq_struct*)_tmp26A)->f3;_tmp26E=(void*)((
struct Cyc_Tcpat_IfEq_struct*)_tmp26A)->f4;_LL146: Cyc_Tcpat_check_exhaust_overlap(
r,_tmp26D,not_exhaust,env1,rhs_appears,env2,exhaust_done);Cyc_Tcpat_check_exhaust_overlap(
r,_tmp26E,not_exhaust,env1,rhs_appears,env2,exhaust_done);goto _LL140;_LL140:;}
struct _tuple14{int f1;struct Cyc_Position_Segment*f2;};struct _tuple15{void*f1;
struct _tuple14*f2;};struct _tuple16{struct _RegionHandle*f1;int*f2;};static struct
_tuple15*Cyc_Tcpat_get_match(struct _tuple16*env,struct Cyc_Absyn_Switch_clause*
swc){struct _tuple16 _tmp270;struct _RegionHandle*_tmp271;int*_tmp272;struct
_tuple16*_tmp26F=env;_tmp270=*_tmp26F;_tmp271=_tmp270.f1;_tmp272=_tmp270.f2;{
void*sp0=Cyc_Tcpat_compile_pat(_tmp271,swc->pattern);struct _tuple14*rhs=({struct
_tuple14*_tmp278=_region_malloc(_tmp271,sizeof(*_tmp278));_tmp278->f1=0;_tmp278->f2=(
swc->pattern)->loc;_tmp278;});void*sp;if(swc->where_clause != 0){sp=Cyc_Tcpat_tuple_pat(
_tmp271,({struct Cyc_List_List*_tmp273=_region_malloc(_tmp271,sizeof(*_tmp273));
_tmp273->hd=(void*)sp0;_tmp273->tl=({struct Cyc_List_List*_tmp274=_region_malloc(
_tmp271,sizeof(*_tmp274));_tmp274->hd=(void*)Cyc_Tcpat_int_pat(_tmp271,*_tmp272,
0);_tmp274->tl=0;_tmp274;});_tmp273;}),0);*_tmp272=*_tmp272 + 1;}else{sp=Cyc_Tcpat_tuple_pat(
_tmp271,({struct Cyc_List_List*_tmp275=_region_malloc(_tmp271,sizeof(*_tmp275));
_tmp275->hd=(void*)sp0;_tmp275->tl=({struct Cyc_List_List*_tmp276=_region_malloc(
_tmp271,sizeof(*_tmp276));_tmp276->hd=(void*)((void*)0);_tmp276->tl=0;_tmp276;});
_tmp275;}),0);}return({struct _tuple15*_tmp277=_region_malloc(_tmp271,sizeof(*
_tmp277));_tmp277->f1=sp;_tmp277->f2=rhs;_tmp277;});}}char Cyc_Tcpat_Desc2string[
16]="\000\000\000\000Desc2string\000";static struct _dyneither_ptr Cyc_Tcpat_descs2string(
struct _RegionHandle*r,struct Cyc_List_List*);static struct _dyneither_ptr Cyc_Tcpat_desc2string(
struct _RegionHandle*r,void*d){void*_tmp279=d;struct Cyc_Tcpat_Con_s*_tmp27A;
struct Cyc_List_List*_tmp27B;struct Cyc_Set_Set*_tmp27C;_LL148: if(*((int*)_tmp279)
!= 0)goto _LL14A;_tmp27A=((struct Cyc_Tcpat_Pos_struct*)_tmp279)->f1;_tmp27B=((
struct Cyc_Tcpat_Pos_struct*)_tmp279)->f2;_LL149: {union Cyc_Tcpat_Name_value_union
_tmp27D=_tmp27A->name;struct Cyc_Absyn_Pat*_tmp27E=_tmp27A->orig_pat;if(_tmp27E == 
0)return Cyc_Tcpat_desc2string(r,(void*)((struct Cyc_List_List*)_check_null(
_tmp27B))->hd);{void*_tmp27F=(void*)_tmp27E->r;struct Cyc_Absyn_Vardecl*_tmp280;
struct Cyc_Absyn_Vardecl*_tmp281;struct Cyc_Absyn_Tvar*_tmp282;struct Cyc_Absyn_Vardecl*
_tmp283;struct Cyc_Absyn_Pat*_tmp284;struct Cyc_Absyn_Pat _tmp285;void*_tmp286;
struct Cyc_Absyn_Datatypefield*_tmp287;struct Cyc_Absyn_AggrInfo _tmp288;union Cyc_Absyn_AggrInfoU_union
_tmp289;struct Cyc_Absyn_Aggrdecl**_tmp28A;struct Cyc_Absyn_Aggrdecl*_tmp28B;
struct Cyc_List_List*_tmp28C;struct Cyc_Absyn_Datatypefield*_tmp28D;int _tmp28E;
char _tmp28F;struct _dyneither_ptr _tmp290;struct Cyc_Absyn_Enumfield*_tmp291;struct
Cyc_Absyn_Enumfield*_tmp292;struct Cyc_Absyn_Exp*_tmp293;_LL14D: if((int)_tmp27F != 
0)goto _LL14F;_LL14E: return({const char*_tmp294="_";_tag_dyneither(_tmp294,sizeof(
char),2);});_LL14F: if(_tmp27F <= (void*)2)goto _LL15F;if(*((int*)_tmp27F)!= 0)goto
_LL151;_tmp280=((struct Cyc_Absyn_Var_p_struct*)_tmp27F)->f1;_LL150: return Cyc_Absynpp_qvar2string(
_tmp280->name);_LL151: if(*((int*)_tmp27F)!= 1)goto _LL153;_tmp281=((struct Cyc_Absyn_Reference_p_struct*)
_tmp27F)->f1;_LL152: return(struct _dyneither_ptr)({struct Cyc_String_pa_struct
_tmp297;_tmp297.tag=0;_tmp297.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp281->name));{void*_tmp295[1]={& _tmp297};Cyc_aprintf(({
const char*_tmp296="*%s";_tag_dyneither(_tmp296,sizeof(char),4);}),_tag_dyneither(
_tmp295,sizeof(void*),1));}});_LL153: if(*((int*)_tmp27F)!= 2)goto _LL155;_tmp282=((
struct Cyc_Absyn_TagInt_p_struct*)_tmp27F)->f1;_tmp283=((struct Cyc_Absyn_TagInt_p_struct*)
_tmp27F)->f2;_LL154: return(struct _dyneither_ptr)({struct Cyc_String_pa_struct
_tmp29B;_tmp29B.tag=0;_tmp29B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*
_tmp282->name);{struct Cyc_String_pa_struct _tmp29A;_tmp29A.tag=0;_tmp29A.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp283->name));{
void*_tmp298[2]={& _tmp29A,& _tmp29B};Cyc_aprintf(({const char*_tmp299="%s<`%s>";
_tag_dyneither(_tmp299,sizeof(char),8);}),_tag_dyneither(_tmp298,sizeof(void*),2));}}});
_LL155: if(*((int*)_tmp27F)!= 3)goto _LL157;_LL156: return(struct _dyneither_ptr)({
struct Cyc_String_pa_struct _tmp29E;_tmp29E.tag=0;_tmp29E.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Tcpat_descs2string(r,_tmp27B));{void*_tmp29C[1]={&
_tmp29E};Cyc_aprintf(({const char*_tmp29D="$(%s)";_tag_dyneither(_tmp29D,sizeof(
char),6);}),_tag_dyneither(_tmp29C,sizeof(void*),1));}});_LL157: if(*((int*)
_tmp27F)!= 4)goto _LL159;_tmp284=((struct Cyc_Absyn_Pointer_p_struct*)_tmp27F)->f1;
_tmp285=*_tmp284;_tmp286=(void*)_tmp285.r;if(_tmp286 <= (void*)2)goto _LL159;if(*((
int*)_tmp286)!= 6)goto _LL159;_tmp287=((struct Cyc_Absyn_Datatype_p_struct*)
_tmp286)->f2;_LL158: return(struct _dyneither_ptr)({struct Cyc_String_pa_struct
_tmp2A2;_tmp2A2.tag=0;_tmp2A2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Tcpat_descs2string(r,_tmp27B));{struct Cyc_String_pa_struct _tmp2A1;_tmp2A1.tag=
0;_tmp2A1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp287->name));{void*_tmp29F[2]={& _tmp2A1,& _tmp2A2};Cyc_aprintf(({const char*
_tmp2A0="&%s(%s)";_tag_dyneither(_tmp2A0,sizeof(char),8);}),_tag_dyneither(
_tmp29F,sizeof(void*),2));}}});_LL159: if(*((int*)_tmp27F)!= 4)goto _LL15B;_LL15A:
return(struct _dyneither_ptr)({struct Cyc_String_pa_struct _tmp2A5;_tmp2A5.tag=0;
_tmp2A5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcpat_descs2string(r,
_tmp27B));{void*_tmp2A3[1]={& _tmp2A5};Cyc_aprintf(({const char*_tmp2A4="&%s";
_tag_dyneither(_tmp2A4,sizeof(char),4);}),_tag_dyneither(_tmp2A3,sizeof(void*),1));}});
_LL15B: if(*((int*)_tmp27F)!= 5)goto _LL15D;_tmp288=((struct Cyc_Absyn_Aggr_p_struct*)
_tmp27F)->f1;_tmp289=_tmp288.aggr_info;if((((((struct Cyc_Absyn_Aggr_p_struct*)
_tmp27F)->f1).aggr_info).KnownAggr).tag != 1)goto _LL15D;_tmp28A=(_tmp289.KnownAggr).f1;
_tmp28B=*_tmp28A;_tmp28C=((struct Cyc_Absyn_Aggr_p_struct*)_tmp27F)->f3;_LL15C:
if((void*)_tmp28B->kind == (void*)1){struct Cyc_List_List*_tmp2A6=_tmp28C;struct
Cyc_List_List _tmp2A7;struct _tuple8*_tmp2A8;struct _tuple8 _tmp2A9;struct Cyc_List_List*
_tmp2AA;struct Cyc_List_List _tmp2AB;void*_tmp2AC;struct _dyneither_ptr*_tmp2AD;
_LL170: if(_tmp2A6 == 0)goto _LL172;_tmp2A7=*_tmp2A6;_tmp2A8=(struct _tuple8*)
_tmp2A7.hd;_tmp2A9=*_tmp2A8;_tmp2AA=_tmp2A9.f1;if(_tmp2AA == 0)goto _LL172;_tmp2AB=*
_tmp2AA;_tmp2AC=(void*)_tmp2AB.hd;if(*((int*)_tmp2AC)!= 1)goto _LL172;_tmp2AD=((
struct Cyc_Absyn_FieldName_struct*)_tmp2AC)->f1;_LL171: return(struct
_dyneither_ptr)({struct Cyc_String_pa_struct _tmp2B2;_tmp2B2.tag=0;_tmp2B2.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcpat_descs2string(r,_tmp27B));{
struct Cyc_String_pa_struct _tmp2B1;_tmp2B1.tag=0;_tmp2B1.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)*_tmp2AD);{struct Cyc_String_pa_struct _tmp2B0;_tmp2B0.tag=0;
_tmp2B0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp28B->name));{void*_tmp2AE[3]={& _tmp2B0,& _tmp2B1,& _tmp2B2};Cyc_aprintf(({
const char*_tmp2AF="%s{.%s = %s}";_tag_dyneither(_tmp2AF,sizeof(char),13);}),
_tag_dyneither(_tmp2AE,sizeof(void*),3));}}}});_LL172:;_LL173: goto _LL16F;_LL16F:;}
return(struct _dyneither_ptr)({struct Cyc_String_pa_struct _tmp2B6;_tmp2B6.tag=0;
_tmp2B6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcpat_descs2string(r,
_tmp27B));{struct Cyc_String_pa_struct _tmp2B5;_tmp2B5.tag=0;_tmp2B5.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp28B->name));{
void*_tmp2B3[2]={& _tmp2B5,& _tmp2B6};Cyc_aprintf(({const char*_tmp2B4="%s{%s}";
_tag_dyneither(_tmp2B4,sizeof(char),7);}),_tag_dyneither(_tmp2B3,sizeof(void*),2));}}});
_LL15D: if(*((int*)_tmp27F)!= 6)goto _LL15F;_tmp28D=((struct Cyc_Absyn_Datatype_p_struct*)
_tmp27F)->f2;_LL15E: return(struct _dyneither_ptr)({struct Cyc_String_pa_struct
_tmp2B9;_tmp2B9.tag=0;_tmp2B9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp28D->name));{void*_tmp2B7[1]={& _tmp2B9};Cyc_aprintf(({
const char*_tmp2B8="%s";_tag_dyneither(_tmp2B8,sizeof(char),3);}),_tag_dyneither(
_tmp2B7,sizeof(void*),1));}});_LL15F: if((int)_tmp27F != 1)goto _LL161;_LL160:
return({const char*_tmp2BA="NULL";_tag_dyneither(_tmp2BA,sizeof(char),5);});
_LL161: if(_tmp27F <= (void*)2)goto _LL16D;if(*((int*)_tmp27F)!= 7)goto _LL163;
_tmp28E=((struct Cyc_Absyn_Int_p_struct*)_tmp27F)->f2;_LL162: return(struct
_dyneither_ptr)({struct Cyc_Int_pa_struct _tmp2BD;_tmp2BD.tag=1;_tmp2BD.f1=(
unsigned long)_tmp28E;{void*_tmp2BB[1]={& _tmp2BD};Cyc_aprintf(({const char*
_tmp2BC="%d";_tag_dyneither(_tmp2BC,sizeof(char),3);}),_tag_dyneither(_tmp2BB,
sizeof(void*),1));}});_LL163: if(*((int*)_tmp27F)!= 8)goto _LL165;_tmp28F=((struct
Cyc_Absyn_Char_p_struct*)_tmp27F)->f1;_LL164: return(struct _dyneither_ptr)({
struct Cyc_Int_pa_struct _tmp2C0;_tmp2C0.tag=1;_tmp2C0.f1=(unsigned long)((int)
_tmp28F);{void*_tmp2BE[1]={& _tmp2C0};Cyc_aprintf(({const char*_tmp2BF="%d";
_tag_dyneither(_tmp2BF,sizeof(char),3);}),_tag_dyneither(_tmp2BE,sizeof(void*),1));}});
_LL165: if(*((int*)_tmp27F)!= 9)goto _LL167;_tmp290=((struct Cyc_Absyn_Float_p_struct*)
_tmp27F)->f1;_LL166: return _tmp290;_LL167: if(*((int*)_tmp27F)!= 10)goto _LL169;
_tmp291=((struct Cyc_Absyn_Enum_p_struct*)_tmp27F)->f2;_LL168: _tmp292=_tmp291;
goto _LL16A;_LL169: if(*((int*)_tmp27F)!= 11)goto _LL16B;_tmp292=((struct Cyc_Absyn_AnonEnum_p_struct*)
_tmp27F)->f2;_LL16A: return Cyc_Absynpp_qvar2string(_tmp292->name);_LL16B: if(*((
int*)_tmp27F)!= 14)goto _LL16D;_tmp293=((struct Cyc_Absyn_Exp_p_struct*)_tmp27F)->f1;
_LL16C: return Cyc_Absynpp_exp2string(_tmp293);_LL16D:;_LL16E:(int)_throw((void*)
Cyc_Tcpat_Desc2string);_LL14C:;}}_LL14A: if(*((int*)_tmp279)!= 1)goto _LL147;
_tmp27C=((struct Cyc_Tcpat_Neg_struct*)_tmp279)->f1;_LL14B: if(((int(*)(struct Cyc_Set_Set*
s))Cyc_Set_is_empty)(_tmp27C))return({const char*_tmp2C1="_";_tag_dyneither(
_tmp2C1,sizeof(char),2);});{struct Cyc_Tcpat_Con_s*_tmp2C2=((struct Cyc_Tcpat_Con_s*(*)(
struct Cyc_Set_Set*s))Cyc_Set_choose)(_tmp27C);struct Cyc_Absyn_Pat*_tmp2C3=
_tmp2C2->orig_pat;if(_tmp2C3 == 0)(int)_throw((void*)Cyc_Tcpat_Desc2string);{void*
_tmp2C4=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp2C3->topt))->v);
void*_tmp2C5;struct Cyc_Absyn_PtrInfo _tmp2C6;struct Cyc_Absyn_PtrAtts _tmp2C7;
struct Cyc_Absyn_DatatypeInfo _tmp2C8;union Cyc_Absyn_DatatypeInfoU_union _tmp2C9;
struct Cyc_Absyn_Datatypedecl**_tmp2CA;struct Cyc_Absyn_Datatypedecl*_tmp2CB;
struct Cyc_Absyn_AggrInfo _tmp2CC;union Cyc_Absyn_AggrInfoU_union _tmp2CD;struct Cyc_Absyn_Aggrdecl**
_tmp2CE;struct Cyc_Absyn_Aggrdecl*_tmp2CF;_LL175: if(_tmp2C4 <= (void*)4)goto _LL17F;
if(*((int*)_tmp2C4)!= 5)goto _LL177;_tmp2C5=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp2C4)->f2;if((int)_tmp2C5 != 0)goto _LL177;_LL176:{int i=0;for(0;i < 256;++ i){
struct Cyc_Tcpat_Con_s*_tmp2D0=Cyc_Tcpat_char_con(r,(char)i,(struct Cyc_Absyn_Pat*)
_tmp2C3);if(!((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(
_tmp27C,_tmp2D0))return(struct _dyneither_ptr)({struct Cyc_Int_pa_struct _tmp2D3;
_tmp2D3.tag=1;_tmp2D3.f1=(unsigned long)i;{void*_tmp2D1[1]={& _tmp2D3};Cyc_aprintf(({
const char*_tmp2D2="%d";_tag_dyneither(_tmp2D2,sizeof(char),3);}),_tag_dyneither(
_tmp2D1,sizeof(void*),1));}});}}(int)_throw((void*)Cyc_Tcpat_Desc2string);_LL177:
if(*((int*)_tmp2C4)!= 5)goto _LL179;_LL178:{unsigned int i=0;for(0;i < 0 - 1;++ i){
struct Cyc_Tcpat_Con_s*_tmp2D4=Cyc_Tcpat_int_con(r,(int)i,_tmp2C3);if(!((int(*)(
struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp27C,_tmp2D4))
return(struct _dyneither_ptr)({struct Cyc_Int_pa_struct _tmp2D7;_tmp2D7.tag=1;
_tmp2D7.f1=(unsigned long)((int)i);{void*_tmp2D5[1]={& _tmp2D7};Cyc_aprintf(({
const char*_tmp2D6="%d";_tag_dyneither(_tmp2D6,sizeof(char),3);}),_tag_dyneither(
_tmp2D5,sizeof(void*),1));}});}}(int)_throw((void*)Cyc_Tcpat_Desc2string);_LL179:
if(*((int*)_tmp2C4)!= 4)goto _LL17B;_tmp2C6=((struct Cyc_Absyn_PointerType_struct*)
_tmp2C4)->f1;_tmp2C7=_tmp2C6.ptr_atts;_LL17A: {struct Cyc_Absyn_Conref*_tmp2D8=
_tmp2C7.nullable;int is_nullable=((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,_tmp2D8);if(is_nullable){if(!((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*
elt))Cyc_Set_member)(_tmp27C,Cyc_Tcpat_null_con(r,(struct Cyc_Absyn_Pat*)_tmp2C3)))
return({const char*_tmp2D9="NULL";_tag_dyneither(_tmp2D9,sizeof(char),5);});}
return({const char*_tmp2DA="&_";_tag_dyneither(_tmp2DA,sizeof(char),3);});}_LL17B:
if(*((int*)_tmp2C4)!= 2)goto _LL17D;_tmp2C8=((struct Cyc_Absyn_DatatypeType_struct*)
_tmp2C4)->f1;_tmp2C9=_tmp2C8.datatype_info;if((((((struct Cyc_Absyn_DatatypeType_struct*)
_tmp2C4)->f1).datatype_info).KnownDatatype).tag != 1)goto _LL17D;_tmp2CA=(_tmp2C9.KnownDatatype).f1;
_tmp2CB=*_tmp2CA;_LL17C: if(_tmp2CB->is_extensible)(int)_throw((void*)Cyc_Tcpat_Desc2string);{
struct Cyc_List_List*_tmp2DB=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp2CB->fields))->v;int span=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp2DB);for(0;(unsigned int)_tmp2DB;_tmp2DB=_tmp2DB->tl){struct _dyneither_ptr n=*(*((
struct Cyc_Absyn_Datatypefield*)_tmp2DB->hd)->name).f2;struct Cyc_List_List*
_tmp2DC=((struct Cyc_Absyn_Datatypefield*)_tmp2DB->hd)->typs;if(!((int(*)(struct
Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp27C,(struct Cyc_Tcpat_Con_s*)({
struct Cyc_Tcpat_Con_s*_tmp2DD=_cycalloc(sizeof(*_tmp2DD));_tmp2DD->name=(union
Cyc_Tcpat_Name_value_union)({union Cyc_Tcpat_Name_value_union _tmp2DE;(_tmp2DE.Name_v).tag=
0;(_tmp2DE.Name_v).f1=n;_tmp2DE;});_tmp2DD->arity=0;_tmp2DD->span=0;_tmp2DD->orig_pat=
_tmp2C3;_tmp2DD;}))){if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp2DC)
== 0)return n;else{if(_tmp2CB->is_flat)return(struct _dyneither_ptr)({struct Cyc_String_pa_struct
_tmp2E1;_tmp2E1.tag=0;_tmp2E1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)n);{
void*_tmp2DF[1]={& _tmp2E1};Cyc_aprintf(({const char*_tmp2E0="%s(...)";
_tag_dyneither(_tmp2E0,sizeof(char),8);}),_tag_dyneither(_tmp2DF,sizeof(void*),1));}});
else{return(struct _dyneither_ptr)({struct Cyc_String_pa_struct _tmp2E4;_tmp2E4.tag=
0;_tmp2E4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)n);{void*_tmp2E2[1]={&
_tmp2E4};Cyc_aprintf(({const char*_tmp2E3="&%s(...)";_tag_dyneither(_tmp2E3,
sizeof(char),9);}),_tag_dyneither(_tmp2E2,sizeof(void*),1));}});}}}}(int)_throw((
void*)Cyc_Tcpat_Desc2string);}_LL17D: if(*((int*)_tmp2C4)!= 10)goto _LL17F;_tmp2CC=((
struct Cyc_Absyn_AggrType_struct*)_tmp2C4)->f1;_tmp2CD=_tmp2CC.aggr_info;if((((((
struct Cyc_Absyn_AggrType_struct*)_tmp2C4)->f1).aggr_info).KnownAggr).tag != 1)
goto _LL17F;_tmp2CE=(_tmp2CD.KnownAggr).f1;_tmp2CF=*_tmp2CE;if(!((void*)_tmp2CF->kind
== (void*)1))goto _LL17F;_LL17E: {struct Cyc_List_List*_tmp2E5=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp2CF->impl))->fields;int span=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp2E5);struct _tuple1 _tmp2E7;struct _dyneither_ptr*_tmp2E8;struct _dyneither_ptr
_tmp2E9;struct _tuple1*_tmp2E6=_tmp2CF->name;_tmp2E7=*_tmp2E6;_tmp2E8=_tmp2E7.f2;
_tmp2E9=*_tmp2E8;for(0;(unsigned int)_tmp2E5;_tmp2E5=_tmp2E5->tl){struct
_dyneither_ptr n=*((struct Cyc_Absyn_Aggrfield*)_tmp2E5->hd)->name;if(!((int(*)(
struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp27C,(struct
Cyc_Tcpat_Con_s*)({struct Cyc_Tcpat_Con_s*_tmp2EA=_cycalloc(sizeof(*_tmp2EA));
_tmp2EA->name=(union Cyc_Tcpat_Name_value_union)({union Cyc_Tcpat_Name_value_union
_tmp2EB;(_tmp2EB.Name_v).tag=0;(_tmp2EB.Name_v).f1=n;_tmp2EB;});_tmp2EA->arity=0;
_tmp2EA->span=0;_tmp2EA->orig_pat=_tmp2C3;_tmp2EA;})))return(struct
_dyneither_ptr)({struct Cyc_String_pa_struct _tmp2EF;_tmp2EF.tag=0;_tmp2EF.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)n);{struct Cyc_String_pa_struct
_tmp2EE;_tmp2EE.tag=0;_tmp2EE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
_tmp2E9);{void*_tmp2EC[2]={& _tmp2EE,& _tmp2EF};Cyc_aprintf(({const char*_tmp2ED="%s{.%s = _}";
_tag_dyneither(_tmp2ED,sizeof(char),12);}),_tag_dyneither(_tmp2EC,sizeof(void*),
2));}}});}(int)_throw((void*)Cyc_Tcpat_Desc2string);}_LL17F:;_LL180:(int)_throw((
void*)Cyc_Tcpat_Desc2string);_LL174:;}}_LL147:;}static struct _dyneither_ptr*Cyc_Tcpat_desc2stringptr(
struct _RegionHandle*r,void*d){return({struct _dyneither_ptr*_tmp2F0=_cycalloc(
sizeof(*_tmp2F0));_tmp2F0[0]=Cyc_Tcpat_desc2string(r,d);_tmp2F0;});}static struct
_dyneither_ptr Cyc_Tcpat_descs2string(struct _RegionHandle*r,struct Cyc_List_List*
ds){struct Cyc_List_List*_tmp2F1=((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct _dyneither_ptr*(*f)(struct _RegionHandle*,void*),struct _RegionHandle*env,
struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcpat_desc2stringptr,r,ds);struct
_dyneither_ptr*comma=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),",",
sizeof(char),2);{struct Cyc_List_List*_tmp2F2=_tmp2F1;for(0;_tmp2F2 != 0;_tmp2F2=((
struct Cyc_List_List*)_check_null(_tmp2F2))->tl){if(_tmp2F2->tl != 0){_tmp2F2->tl=(
struct Cyc_List_List*)({struct Cyc_List_List*_tmp2F3=_cycalloc(sizeof(*_tmp2F3));
_tmp2F3->hd=comma;_tmp2F3->tl=_tmp2F2->tl;_tmp2F3;});_tmp2F2=_tmp2F2->tl;}}}
return(struct _dyneither_ptr)Cyc_strconcat_l(_tmp2F1);}static void Cyc_Tcpat_not_exhaust_err(
struct _RegionHandle*r,struct Cyc_Position_Segment*loc,void*desc){struct
_handler_cons _tmp2F5;_push_handler(& _tmp2F5);{int _tmp2F7=0;if(setjmp(_tmp2F5.handler))
_tmp2F7=1;if(!_tmp2F7){{struct _dyneither_ptr _tmp2F8=Cyc_Tcpat_desc2string(r,desc);({
struct Cyc_String_pa_struct _tmp2FB;_tmp2FB.tag=0;_tmp2FB.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)_tmp2F8);{void*_tmp2F9[1]={& _tmp2FB};Cyc_Tcutil_terr(loc,({
const char*_tmp2FA="patterns may not be exhaustive.\n\tmissing case for %s";
_tag_dyneither(_tmp2FA,sizeof(char),53);}),_tag_dyneither(_tmp2F9,sizeof(void*),
1));}});};_pop_handler();}else{void*_tmp2F6=(void*)_exn_thrown;void*_tmp2FD=
_tmp2F6;_LL182: if(_tmp2FD != Cyc_Tcpat_Desc2string)goto _LL184;_LL183:({void*
_tmp2FE=0;Cyc_Tcutil_terr(loc,({const char*_tmp2FF="patterns may not be exhaustive.";
_tag_dyneither(_tmp2FF,sizeof(char),32);}),_tag_dyneither(_tmp2FE,sizeof(void*),
0));});goto _LL181;_LL184:;_LL185:(void)_throw(_tmp2FD);_LL181:;}}}static void Cyc_Tcpat_rule_occurs(
int dummy,struct _tuple14*rhs){(*rhs).f1=1;}void Cyc_Tcpat_check_switch_exhaustive(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*swcs){
struct _RegionHandle*_tmp300=Cyc_Tcenv_get_fnrgn(te);int _tmp301=0;struct _tuple16
_tmp302=({struct _tuple16 _tmp30E;_tmp30E.f1=_tmp300;_tmp30E.f2=& _tmp301;_tmp30E;});
struct Cyc_List_List*_tmp303=((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct _tuple15*(*f)(struct _tuple16*,struct Cyc_Absyn_Switch_clause*),struct
_tuple16*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(_tmp300,Cyc_Tcpat_get_match,&
_tmp302,swcs);void*_tmp304=((void*(*)(struct _RegionHandle*r,struct Cyc_List_List*
allmrules))Cyc_Tcpat_match_compile)(_tmp300,_tmp303);int _tmp305=0;((void(*)(
struct _RegionHandle*r,void*d,void(*not_exhaust)(struct _RegionHandle*,struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env1,void(*rhs_appears)(int,struct _tuple14*),
int env2,int*exhaust_done))Cyc_Tcpat_check_exhaust_overlap)(_tmp300,_tmp304,Cyc_Tcpat_not_exhaust_err,
loc,Cyc_Tcpat_rule_occurs,0,& _tmp305);for(0;_tmp303 != 0;_tmp303=_tmp303->tl){
struct _tuple15 _tmp307;struct _tuple14*_tmp308;struct _tuple14 _tmp309;int _tmp30A;
struct Cyc_Position_Segment*_tmp30B;struct _tuple15*_tmp306=(struct _tuple15*)
_tmp303->hd;_tmp307=*_tmp306;_tmp308=_tmp307.f2;_tmp309=*_tmp308;_tmp30A=_tmp309.f1;
_tmp30B=_tmp309.f2;if(!_tmp30A)({void*_tmp30C=0;Cyc_Tcutil_terr(_tmp30B,({const
char*_tmp30D="redundant pattern";_tag_dyneither(_tmp30D,sizeof(char),18);}),
_tag_dyneither(_tmp30C,sizeof(void*),0));});}}struct _tuple17{struct Cyc_Position_Segment*
f1;int f2;};static void Cyc_Tcpat_not_exhaust_warn(struct _RegionHandle*r,struct
_tuple17*pr,void*desc){(*pr).f2=0;{struct _handler_cons _tmp30F;_push_handler(&
_tmp30F);{int _tmp311=0;if(setjmp(_tmp30F.handler))_tmp311=1;if(!_tmp311){{struct
_dyneither_ptr _tmp312=Cyc_Tcpat_desc2string(r,desc);({struct Cyc_String_pa_struct
_tmp315;_tmp315.tag=0;_tmp315.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
_tmp312);{void*_tmp313[1]={& _tmp315};Cyc_Tcutil_warn((*pr).f1,({const char*
_tmp314="pattern not exhaustive.\n\tmissing case for %s";_tag_dyneither(_tmp314,
sizeof(char),45);}),_tag_dyneither(_tmp313,sizeof(void*),1));}});};_pop_handler();}
else{void*_tmp310=(void*)_exn_thrown;void*_tmp317=_tmp310;_LL187: if(_tmp317 != 
Cyc_Tcpat_Desc2string)goto _LL189;_LL188:({void*_tmp318=0;Cyc_Tcutil_warn((*pr).f1,({
const char*_tmp319="pattern not exhaustive.";_tag_dyneither(_tmp319,sizeof(char),
24);}),_tag_dyneither(_tmp318,sizeof(void*),0));});goto _LL186;_LL189:;_LL18A:(
void)_throw(_tmp317);_LL186:;}}}}static void Cyc_Tcpat_dummy_fn(int i,int j){return;}
struct _tuple18{void*f1;int f2;};int Cyc_Tcpat_check_let_pat_exhaustive(struct Cyc_Position_Segment*
loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p){struct _RegionHandle*_tmp31A=
Cyc_Tcenv_get_fnrgn(te);struct Cyc_List_List*_tmp31B=({struct Cyc_List_List*
_tmp320=_region_malloc(_tmp31A,sizeof(*_tmp320));_tmp320->hd=({struct _tuple18*
_tmp321=_region_malloc(_tmp31A,sizeof(*_tmp321));_tmp321->f1=Cyc_Tcpat_compile_pat(
_tmp31A,p);_tmp321->f2=0;_tmp321;});_tmp320->tl=0;_tmp320;});void*_tmp31C=((void*(*)(
struct _RegionHandle*r,struct Cyc_List_List*allmrules))Cyc_Tcpat_match_compile)(
_tmp31A,_tmp31B);struct _tuple17 _tmp31D=({struct _tuple17 _tmp31F;_tmp31F.f1=loc;
_tmp31F.f2=1;_tmp31F;});int _tmp31E=0;((void(*)(struct _RegionHandle*r,void*d,void(*
not_exhaust)(struct _RegionHandle*,struct _tuple17*,void*),struct _tuple17*env1,
void(*rhs_appears)(int,int),int env2,int*exhaust_done))Cyc_Tcpat_check_exhaust_overlap)(
_tmp31A,_tmp31C,Cyc_Tcpat_not_exhaust_warn,& _tmp31D,Cyc_Tcpat_dummy_fn,0,&
_tmp31E);return _tmp31D.f2;}static struct _tuple15*Cyc_Tcpat_get_match2(struct
_RegionHandle*r,struct Cyc_Absyn_Switch_clause*swc){void*sp0=Cyc_Tcpat_compile_pat(
r,swc->pattern);struct _tuple14*rhs=({struct _tuple14*_tmp323=_region_malloc(r,
sizeof(*_tmp323));_tmp323->f1=0;_tmp323->f2=(swc->pattern)->loc;_tmp323;});
return({struct _tuple15*_tmp322=_region_malloc(r,sizeof(*_tmp322));_tmp322->f1=
sp0;_tmp322->f2=rhs;_tmp322;});}static void Cyc_Tcpat_not_exhaust_err2(struct
_RegionHandle*r,struct Cyc_Position_Segment*loc,void*d){;}void Cyc_Tcpat_check_catch_overlap(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*swcs){
struct _RegionHandle*_tmp324=Cyc_Tcenv_get_fnrgn(te);struct Cyc_List_List*_tmp325=((
struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple15*(*f)(struct
_RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct _RegionHandle*env,struct Cyc_List_List*
x))Cyc_List_rmap_c)(_tmp324,Cyc_Tcpat_get_match2,_tmp324,swcs);void*_tmp326=((
void*(*)(struct _RegionHandle*r,struct Cyc_List_List*allmrules))Cyc_Tcpat_match_compile)(
_tmp324,_tmp325);int _tmp327=0;((void(*)(struct _RegionHandle*r,void*d,void(*
not_exhaust)(struct _RegionHandle*,struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*
env1,void(*rhs_appears)(int,struct _tuple14*),int env2,int*exhaust_done))Cyc_Tcpat_check_exhaust_overlap)(
_tmp324,_tmp326,Cyc_Tcpat_not_exhaust_err2,loc,Cyc_Tcpat_rule_occurs,0,& _tmp327);
for(0;_tmp325 != 0;_tmp325=_tmp325->tl){struct _tuple15 _tmp329;struct _tuple14*
_tmp32A;struct _tuple14 _tmp32B;int _tmp32C;struct Cyc_Position_Segment*_tmp32D;
struct _tuple15*_tmp328=(struct _tuple15*)_tmp325->hd;_tmp329=*_tmp328;_tmp32A=
_tmp329.f2;_tmp32B=*_tmp32A;_tmp32C=_tmp32B.f1;_tmp32D=_tmp32B.f2;if(!_tmp32C)({
void*_tmp32E=0;Cyc_Tcutil_terr(_tmp32D,({const char*_tmp32F="redundant pattern";
_tag_dyneither(_tmp32F,sizeof(char),18);}),_tag_dyneither(_tmp32E,sizeof(void*),
0));});}}
