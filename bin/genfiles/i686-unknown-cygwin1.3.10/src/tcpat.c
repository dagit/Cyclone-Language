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
_dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(
struct Cyc_Position_Segment*);union Cyc_Absyn_Constraint*Cyc_Absyn_empty_conref();
void*Cyc_Absyn_conref_def(void*y,union Cyc_Absyn_Constraint*x);extern union Cyc_Absyn_Constraint*
Cyc_Absyn_true_conref;extern union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref;
void*Cyc_Absyn_compress_kb(void*);void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,
struct Cyc_Core_Opt*tenv);void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);extern void*
Cyc_Absyn_char_typ;extern void*Cyc_Absyn_uint_typ;extern void*Cyc_Absyn_sint_typ;
extern void*Cyc_Absyn_float_typ;extern void*Cyc_Absyn_empty_effect;struct Cyc_Absyn_Pat*
Cyc_Absyn_new_pat(void*p,struct Cyc_Position_Segment*s);struct Cyc_Absyn_Vardecl*
Cyc_Absyn_new_vardecl(struct _tuple1*x,void*t,struct Cyc_Absyn_Exp*init);struct Cyc_PP_Ppstate;
struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs: 1;
int qvar_to_Cids: 1;int add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int
rewrite_temp_tvars: 1;int print_all_tvars: 1;int print_all_kinds: 1;int
print_all_effects: 1;int print_using_stmts: 1;int print_externC_stmts: 1;int
print_full_evars: 1;int print_zeroterm: 1;int generate_line_directives: 1;int
use_curr_namespace: 1;struct Cyc_List_List*curr_namespace;};struct _dyneither_ptr
Cyc_Absynpp_typ2string(void*);struct _dyneither_ptr Cyc_Absynpp_kind2string(void*);
struct _dyneither_ptr Cyc_Absynpp_kindbound2string(void*);struct _dyneither_ptr Cyc_Absynpp_exp2string(
struct Cyc_Absyn_Exp*);struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple1*);
struct Cyc_RgnOrder_RgnPO;struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_initial_fn_po(
struct _RegionHandle*,struct Cyc_List_List*tvs,struct Cyc_List_List*po,void*effect,
struct Cyc_Absyn_Tvar*fst_rgn,struct Cyc_Position_Segment*);struct Cyc_RgnOrder_RgnPO*
Cyc_RgnOrder_add_outlives_constraint(struct _RegionHandle*,struct Cyc_RgnOrder_RgnPO*
po,void*eff,void*rgn,struct Cyc_Position_Segment*loc);struct Cyc_RgnOrder_RgnPO*
Cyc_RgnOrder_add_youngest(struct _RegionHandle*,struct Cyc_RgnOrder_RgnPO*po,
struct Cyc_Absyn_Tvar*rgn,int resetable,int opened);int Cyc_RgnOrder_is_region_resetable(
struct Cyc_RgnOrder_RgnPO*po,struct Cyc_Absyn_Tvar*r);int Cyc_RgnOrder_effect_outlives(
struct Cyc_RgnOrder_RgnPO*po,void*eff,void*rgn);int Cyc_RgnOrder_satisfies_constraints(
struct Cyc_RgnOrder_RgnPO*po,struct Cyc_List_List*constraints,void*default_bound,
int do_pin);int Cyc_RgnOrder_eff_outlives_eff(struct Cyc_RgnOrder_RgnPO*po,void*
eff1,void*eff2);void Cyc_RgnOrder_print_region_po(struct Cyc_RgnOrder_RgnPO*po);
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
struct _RegionHandle*,struct Cyc_List_List*,void*);struct _tuple10{struct Cyc_List_List*
f1;struct _RegionHandle*f2;};struct _tuple11{struct Cyc_Absyn_Tvar*f1;void*f2;};
struct _tuple11*Cyc_Tcutil_r_make_inst_var(struct _tuple10*,struct Cyc_Absyn_Tvar*);
void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*vs,struct Cyc_Position_Segment*
loc,struct _dyneither_ptr err_msg);struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(
struct _RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,
void*,struct Cyc_List_List*fields);int Cyc_Tcutil_is_noalias_pointer(void*t);int
Cyc_Tcutil_is_noalias_pointer_or_aggr(struct _RegionHandle*,void*t);struct Cyc_Absyn_Tvar*
Cyc_Tcutil_new_tvar(void*k);int Cyc_Tcutil_is_const_exp(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Absyn_Exp*e);void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*,void**,struct
Cyc_Absyn_Exp*);struct Cyc_Tcexp_TestEnv{struct _tuple0*eq;int isTrue;};struct Cyc_Tcexp_TestEnv
Cyc_Tcexp_tcTest(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct
_dyneither_ptr msg_part);struct _tuple12{struct Cyc_List_List*f1;struct Cyc_List_List*
f2;};struct Cyc_Tcpat_TcPatResult{struct _tuple12*tvars_and_bounds_opt;struct Cyc_List_List*
patvars;};struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_tcPat(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Absyn_Pat*p,void**topt);void Cyc_Tcpat_check_pat_regions(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Pat*p);void Cyc_Tcpat_check_switch_exhaustive(struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*);int Cyc_Tcpat_check_let_pat_exhaustive(
struct Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Pat*p);void Cyc_Tcpat_check_catch_overlap(
struct Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*);struct
_tuple13{unsigned int f1;int f2;};struct _tuple13 Cyc_Evexp_eval_const_uint_exp(
struct Cyc_Absyn_Exp*e);struct _tuple14{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*
f2;};static void Cyc_Tcpat_resolve_pat(struct Cyc_Tcenv_Tenv*te,void**topt,struct
Cyc_Absyn_Pat*p){void*_tmp0=p->r;struct _tuple1*_tmp1;struct _tuple1*_tmp2;struct
Cyc_List_List*_tmp3;int _tmp4;struct Cyc_Absyn_AggrInfo*_tmp5;struct Cyc_List_List*
_tmp6;struct Cyc_List_List*_tmp7;int _tmp8;struct Cyc_Absyn_AggrInfo*_tmp9;struct
Cyc_Absyn_AggrInfo _tmpA;union Cyc_Absyn_AggrInfoU _tmpB;struct _tuple3 _tmpC;void*
_tmpD;struct _tuple1*_tmpE;struct Cyc_List_List*_tmpF;struct Cyc_List_List*_tmp10;
struct Cyc_List_List**_tmp11;struct Cyc_List_List*_tmp12;int _tmp13;struct Cyc_Absyn_Exp*
_tmp14;_LL1: if(_tmp0 <= (void*)2)goto _LLB;if(*((int*)_tmp0)!= 12)goto _LL3;_tmp1=((
struct Cyc_Absyn_UnknownId_p_struct*)_tmp0)->f1;_LL2:{struct _handler_cons _tmp15;
_push_handler(& _tmp15);{int _tmp17=0;if(setjmp(_tmp15.handler))_tmp17=1;if(!
_tmp17){{struct _RegionHandle*_tmp18=Cyc_Tcenv_get_fnrgn(te);void*_tmp19=Cyc_Tcenv_lookup_ordinary(
_tmp18,te,p->loc,_tmp1);struct Cyc_Absyn_Datatypedecl*_tmp1A;struct Cyc_Absyn_Datatypefield*
_tmp1B;struct Cyc_Absyn_Enumdecl*_tmp1C;struct Cyc_Absyn_Enumfield*_tmp1D;void*
_tmp1E;struct Cyc_Absyn_Enumfield*_tmp1F;_LLE: if(*((int*)_tmp19)!= 1)goto _LL10;
_LLF: p->r=(void*)0;({void*_tmp20=0;Cyc_Tcutil_terr(p->loc,({const char*_tmp21="struct tag used without arguments in pattern";
_tag_dyneither(_tmp21,sizeof(char),45);}),_tag_dyneither(_tmp20,sizeof(void*),0));});
_npop_handler(0);return;_LL10: if(*((int*)_tmp19)!= 2)goto _LL12;_tmp1A=((struct
Cyc_Tcenv_DatatypeRes_struct*)_tmp19)->f1;_tmp1B=((struct Cyc_Tcenv_DatatypeRes_struct*)
_tmp19)->f2;_LL11: p->r=(void*)({struct Cyc_Absyn_Datatype_p_struct*_tmp22=
_cycalloc(sizeof(*_tmp22));_tmp22[0]=({struct Cyc_Absyn_Datatype_p_struct _tmp23;
_tmp23.tag=6;_tmp23.f1=_tmp1A;_tmp23.f2=_tmp1B;_tmp23.f3=0;_tmp23.f4=0;_tmp23;});
_tmp22;});_npop_handler(0);return;_LL12: if(*((int*)_tmp19)!= 3)goto _LL14;_tmp1C=((
struct Cyc_Tcenv_EnumRes_struct*)_tmp19)->f1;_tmp1D=((struct Cyc_Tcenv_EnumRes_struct*)
_tmp19)->f2;_LL13: p->r=(void*)({struct Cyc_Absyn_Enum_p_struct*_tmp24=_cycalloc(
sizeof(*_tmp24));_tmp24[0]=({struct Cyc_Absyn_Enum_p_struct _tmp25;_tmp25.tag=10;
_tmp25.f1=_tmp1C;_tmp25.f2=_tmp1D;_tmp25;});_tmp24;});_npop_handler(0);return;
_LL14: if(*((int*)_tmp19)!= 4)goto _LL16;_tmp1E=(void*)((struct Cyc_Tcenv_AnonEnumRes_struct*)
_tmp19)->f1;_tmp1F=((struct Cyc_Tcenv_AnonEnumRes_struct*)_tmp19)->f2;_LL15: p->r=(
void*)({struct Cyc_Absyn_AnonEnum_p_struct*_tmp26=_cycalloc(sizeof(*_tmp26));
_tmp26[0]=({struct Cyc_Absyn_AnonEnum_p_struct _tmp27;_tmp27.tag=11;_tmp27.f1=(
void*)_tmp1E;_tmp27.f2=_tmp1F;_tmp27;});_tmp26;});_npop_handler(0);return;_LL16:
if(*((int*)_tmp19)!= 0)goto _LLD;_LL17: goto _LLD;_LLD:;};_pop_handler();}else{void*
_tmp16=(void*)_exn_thrown;void*_tmp29=_tmp16;_LL19: if(_tmp29 != Cyc_Dict_Absent)
goto _LL1B;_LL1A: goto _LL18;_LL1B:;_LL1C:(void)_throw(_tmp29);_LL18:;}}}{union Cyc_Absyn_Nmspace
_tmp2A=(*_tmp1).f1;int _tmp2B;struct Cyc_List_List*_tmp2C;_LL1E: if((_tmp2A.Loc_n).tag
!= 3)goto _LL20;_tmp2B=(int)(_tmp2A.Loc_n).val;_LL1F: goto _LL21;_LL20: if((_tmp2A.Rel_n).tag
!= 1)goto _LL22;_tmp2C=(struct Cyc_List_List*)(_tmp2A.Rel_n).val;if(_tmp2C != 0)
goto _LL22;_LL21:(*_tmp1).f1=Cyc_Absyn_Loc_n;p->r=(void*)({struct Cyc_Absyn_Var_p_struct*
_tmp2D=_cycalloc(sizeof(*_tmp2D));_tmp2D[0]=({struct Cyc_Absyn_Var_p_struct _tmp2E;
_tmp2E.tag=0;_tmp2E.f1=Cyc_Absyn_new_vardecl(_tmp1,(void*)0,0);_tmp2E.f2=Cyc_Absyn_new_pat((
void*)0,0);_tmp2E;});_tmp2D;});return;_LL22:;_LL23:({void*_tmp2F=0;Cyc_Tcutil_terr(
p->loc,({const char*_tmp30="qualified variable in pattern";_tag_dyneither(_tmp30,
sizeof(char),30);}),_tag_dyneither(_tmp2F,sizeof(void*),0));});return;_LL1D:;}
_LL3: if(*((int*)_tmp0)!= 13)goto _LL5;_tmp2=((struct Cyc_Absyn_UnknownCall_p_struct*)
_tmp0)->f1;_tmp3=((struct Cyc_Absyn_UnknownCall_p_struct*)_tmp0)->f2;_tmp4=((
struct Cyc_Absyn_UnknownCall_p_struct*)_tmp0)->f3;_LL4:{struct _handler_cons _tmp31;
_push_handler(& _tmp31);{int _tmp33=0;if(setjmp(_tmp31.handler))_tmp33=1;if(!
_tmp33){{struct _RegionHandle*_tmp34=Cyc_Tcenv_get_fnrgn(te);void*_tmp35=Cyc_Tcenv_lookup_ordinary(
_tmp34,te,p->loc,_tmp2);struct Cyc_Absyn_Aggrdecl*_tmp36;struct Cyc_Absyn_Datatypedecl*
_tmp37;struct Cyc_Absyn_Datatypefield*_tmp38;_LL25: if(*((int*)_tmp35)!= 1)goto
_LL27;_tmp36=((struct Cyc_Tcenv_AggrRes_struct*)_tmp35)->f1;_LL26: {struct Cyc_List_List*
_tmp39=0;for(0;_tmp3 != 0;_tmp3=_tmp3->tl){_tmp39=({struct Cyc_List_List*_tmp3A=
_cycalloc(sizeof(*_tmp3A));_tmp3A->hd=({struct _tuple14*_tmp3B=_cycalloc(sizeof(*
_tmp3B));_tmp3B->f1=0;_tmp3B->f2=(struct Cyc_Absyn_Pat*)_tmp3->hd;_tmp3B;});
_tmp3A->tl=_tmp39;_tmp3A;});}p->r=(void*)({struct Cyc_Absyn_Aggr_p_struct*_tmp3C=
_cycalloc(sizeof(*_tmp3C));_tmp3C[0]=({struct Cyc_Absyn_Aggr_p_struct _tmp3D;
_tmp3D.tag=5;_tmp3D.f1=({struct Cyc_Absyn_AggrInfo*_tmp3E=_cycalloc(sizeof(*
_tmp3E));_tmp3E->aggr_info=Cyc_Absyn_KnownAggr(({struct Cyc_Absyn_Aggrdecl**
_tmp3F=_cycalloc(sizeof(*_tmp3F));_tmp3F[0]=_tmp36;_tmp3F;}));_tmp3E->targs=0;
_tmp3E;});_tmp3D.f2=0;_tmp3D.f3=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(_tmp39);_tmp3D.f4=_tmp4;_tmp3D;});_tmp3C;});_npop_handler(0);
return;}_LL27: if(*((int*)_tmp35)!= 2)goto _LL29;_tmp37=((struct Cyc_Tcenv_DatatypeRes_struct*)
_tmp35)->f1;_tmp38=((struct Cyc_Tcenv_DatatypeRes_struct*)_tmp35)->f2;_LL28: p->r=(
void*)({struct Cyc_Absyn_Datatype_p_struct*_tmp40=_cycalloc(sizeof(*_tmp40));
_tmp40[0]=({struct Cyc_Absyn_Datatype_p_struct _tmp41;_tmp41.tag=6;_tmp41.f1=
_tmp37;_tmp41.f2=_tmp38;_tmp41.f3=_tmp3;_tmp41.f4=_tmp4;_tmp41;});_tmp40;});
_npop_handler(0);return;_LL29: if(*((int*)_tmp35)!= 3)goto _LL2B;_LL2A: goto _LL2C;
_LL2B: if(*((int*)_tmp35)!= 4)goto _LL2D;_LL2C:({void*_tmp42=0;Cyc_Tcutil_terr(p->loc,({
const char*_tmp43="enum tag used with arguments in pattern";_tag_dyneither(_tmp43,
sizeof(char),40);}),_tag_dyneither(_tmp42,sizeof(void*),0));});p->r=(void*)0;
_npop_handler(0);return;_LL2D: if(*((int*)_tmp35)!= 0)goto _LL24;_LL2E: goto _LL24;
_LL24:;};_pop_handler();}else{void*_tmp32=(void*)_exn_thrown;void*_tmp45=_tmp32;
_LL30: if(_tmp45 != Cyc_Dict_Absent)goto _LL32;_LL31: goto _LL2F;_LL32:;_LL33:(void)
_throw(_tmp45);_LL2F:;}}}({struct Cyc_String_pa_struct _tmp48;_tmp48.tag=0;_tmp48.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp2));{
void*_tmp46[1]={& _tmp48};Cyc_Tcutil_terr(p->loc,({const char*_tmp47="%s is not a constructor in pattern";
_tag_dyneither(_tmp47,sizeof(char),35);}),_tag_dyneither(_tmp46,sizeof(void*),1));}});
p->r=(void*)0;return;_LL5: if(*((int*)_tmp0)!= 5)goto _LL7;_tmp5=((struct Cyc_Absyn_Aggr_p_struct*)
_tmp0)->f1;if(_tmp5 != 0)goto _LL7;_tmp6=((struct Cyc_Absyn_Aggr_p_struct*)_tmp0)->f2;
_tmp7=((struct Cyc_Absyn_Aggr_p_struct*)_tmp0)->f3;_tmp8=((struct Cyc_Absyn_Aggr_p_struct*)
_tmp0)->f4;_LL6: if(topt == 0)({void*_tmp49=0;Cyc_Tcutil_terr(p->loc,({const char*
_tmp4A="cannot determine pattern type";_tag_dyneither(_tmp4A,sizeof(char),30);}),
_tag_dyneither(_tmp49,sizeof(void*),0));});{void*t=Cyc_Tcutil_compress(*((void**)
_check_null(topt)));{void*_tmp4B=t;struct Cyc_Absyn_AggrInfo _tmp4C;_LL35: if(
_tmp4B <= (void*)4)goto _LL37;if(*((int*)_tmp4B)!= 10)goto _LL37;_tmp4C=((struct Cyc_Absyn_AggrType_struct*)
_tmp4B)->f1;_LL36: p->r=(void*)({struct Cyc_Absyn_Aggr_p_struct*_tmp4D=_cycalloc(
sizeof(*_tmp4D));_tmp4D[0]=({struct Cyc_Absyn_Aggr_p_struct _tmp4E;_tmp4E.tag=5;
_tmp4E.f1=({struct Cyc_Absyn_AggrInfo*_tmp4F=_cycalloc(sizeof(*_tmp4F));_tmp4F[0]=
_tmp4C;_tmp4F;});_tmp4E.f2=_tmp6;_tmp4E.f3=_tmp7;_tmp4E.f4=_tmp8;_tmp4E;});
_tmp4D;});Cyc_Tcpat_resolve_pat(te,topt,p);goto _LL34;_LL37:;_LL38:({struct Cyc_String_pa_struct
_tmp52;_tmp52.tag=0;_tmp52.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t));{void*_tmp50[1]={& _tmp52};Cyc_Tcutil_terr(p->loc,({const char*_tmp51="pattern expects aggregate type instead of %s";
_tag_dyneither(_tmp51,sizeof(char),45);}),_tag_dyneither(_tmp50,sizeof(void*),1));}});
goto _LL34;_LL34:;}return;}_LL7: if(*((int*)_tmp0)!= 5)goto _LL9;_tmp9=((struct Cyc_Absyn_Aggr_p_struct*)
_tmp0)->f1;if(_tmp9 == 0)goto _LL9;_tmpA=*_tmp9;_tmpB=_tmpA.aggr_info;if((_tmpB.UnknownAggr).tag
!= 1)goto _LL9;_tmpC=(struct _tuple3)(_tmpB.UnknownAggr).val;_tmpD=_tmpC.f1;_tmpE=
_tmpC.f2;_tmpF=_tmpA.targs;_tmp10=((struct Cyc_Absyn_Aggr_p_struct*)_tmp0)->f2;
_tmp11=(struct Cyc_List_List**)&((struct Cyc_Absyn_Aggr_p_struct*)_tmp0)->f2;
_tmp12=((struct Cyc_Absyn_Aggr_p_struct*)_tmp0)->f3;_tmp13=((struct Cyc_Absyn_Aggr_p_struct*)
_tmp0)->f4;_LL8:{struct _handler_cons _tmp53;_push_handler(& _tmp53);{int _tmp55=0;
if(setjmp(_tmp53.handler))_tmp55=1;if(!_tmp55){{struct Cyc_Absyn_Aggrdecl**_tmp56=
Cyc_Tcenv_lookup_aggrdecl(te,p->loc,_tmpE);struct Cyc_Absyn_Aggrdecl*_tmp57=*
_tmp56;if(_tmp57->impl == 0){({struct Cyc_String_pa_struct _tmp5A;_tmp5A.tag=0;
_tmp5A.f1=(struct _dyneither_ptr)(_tmp57->kind == (void*)0?({const char*_tmp5B="struct";
_tag_dyneither(_tmp5B,sizeof(char),7);}):({const char*_tmp5C="union";
_tag_dyneither(_tmp5C,sizeof(char),6);}));{void*_tmp58[1]={& _tmp5A};Cyc_Tcutil_terr(
p->loc,({const char*_tmp59="can't destructure an abstract %s";_tag_dyneither(
_tmp59,sizeof(char),33);}),_tag_dyneither(_tmp58,sizeof(void*),1));}});p->r=(
void*)0;_npop_handler(0);return;}{int more_exists=((int(*)(struct Cyc_List_List*x))
Cyc_List_length)(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp57->impl))->exist_vars)
- ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(*_tmp11);if(more_exists < 0){({
void*_tmp5D=0;Cyc_Tcutil_terr(p->loc,({const char*_tmp5E="too many existentially bound type variables in pattern";
_tag_dyneither(_tmp5E,sizeof(char),55);}),_tag_dyneither(_tmp5D,sizeof(void*),0));});{
struct Cyc_List_List**_tmp5F=_tmp11;{int n=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp57->impl))->exist_vars);for(0;n != 
0;-- n){_tmp5F=&((struct Cyc_List_List*)_check_null(*_tmp5F))->tl;}}*_tmp5F=0;}}
else{if(more_exists > 0){struct Cyc_List_List*_tmp60=0;for(0;more_exists != 0;--
more_exists){_tmp60=({struct Cyc_List_List*_tmp61=_cycalloc(sizeof(*_tmp61));
_tmp61->hd=Cyc_Tcutil_new_tvar((void*)({struct Cyc_Absyn_Unknown_kb_struct*_tmp62=
_cycalloc(sizeof(*_tmp62));_tmp62[0]=({struct Cyc_Absyn_Unknown_kb_struct _tmp63;
_tmp63.tag=1;_tmp63.f1=0;_tmp63;});_tmp62;}));_tmp61->tl=_tmp60;_tmp61;});}*
_tmp11=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_imp_append)(*_tmp11,_tmp60);}}p->r=(void*)({struct Cyc_Absyn_Aggr_p_struct*
_tmp64=_cycalloc(sizeof(*_tmp64));_tmp64[0]=({struct Cyc_Absyn_Aggr_p_struct
_tmp65;_tmp65.tag=5;_tmp65.f1=({struct Cyc_Absyn_AggrInfo*_tmp66=_cycalloc(
sizeof(*_tmp66));_tmp66->aggr_info=Cyc_Absyn_KnownAggr(_tmp56);_tmp66->targs=
_tmpF;_tmp66;});_tmp65.f2=*_tmp11;_tmp65.f3=_tmp12;_tmp65.f4=_tmp13;_tmp65;});
_tmp64;});}};_pop_handler();}else{void*_tmp54=(void*)_exn_thrown;void*_tmp68=
_tmp54;_LL3A: if(_tmp68 != Cyc_Dict_Absent)goto _LL3C;_LL3B:({void*_tmp69=0;Cyc_Tcutil_terr(
p->loc,({const char*_tmp6A="Non-aggregate name has designator patterns";
_tag_dyneither(_tmp6A,sizeof(char),43);}),_tag_dyneither(_tmp69,sizeof(void*),0));});
p->r=(void*)0;goto _LL39;_LL3C:;_LL3D:(void)_throw(_tmp68);_LL39:;}}}return;_LL9:
if(*((int*)_tmp0)!= 14)goto _LLB;_tmp14=((struct Cyc_Absyn_Exp_p_struct*)_tmp0)->f1;
_LLA: Cyc_Tcexp_tcExp(te,0,_tmp14);if(!Cyc_Tcutil_is_const_exp(te,_tmp14)){({void*
_tmp6B=0;Cyc_Tcutil_terr(p->loc,({const char*_tmp6C="non-constant expression in case pattern";
_tag_dyneither(_tmp6C,sizeof(char),40);}),_tag_dyneither(_tmp6B,sizeof(void*),0));});
p->r=(void*)0;}{unsigned int _tmp6E;int _tmp6F;struct _tuple13 _tmp6D=Cyc_Evexp_eval_const_uint_exp(
_tmp14);_tmp6E=_tmp6D.f1;_tmp6F=_tmp6D.f2;p->r=(void*)({struct Cyc_Absyn_Int_p_struct*
_tmp70=_cycalloc(sizeof(*_tmp70));_tmp70[0]=({struct Cyc_Absyn_Int_p_struct _tmp71;
_tmp71.tag=7;_tmp71.f1=(void*)((void*)2);_tmp71.f2=(int)_tmp6E;_tmp71;});_tmp70;});
return;}_LLB:;_LLC: return;_LL0:;}static struct _dyneither_ptr*Cyc_Tcpat_get_name(
struct Cyc_Absyn_Vardecl*vd){return(*vd->name).f2;}static void*Cyc_Tcpat_any_type(
struct Cyc_List_List*s,void**topt){if(topt != 0)return*topt;return Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_mk,({struct Cyc_Core_Opt*_tmp72=_cycalloc(sizeof(*
_tmp72));_tmp72->v=s;_tmp72;}));}static void*Cyc_Tcpat_num_type(void**topt,void*
numt){if(topt != 0  && Cyc_Tcutil_coerceable(*topt))return*topt;{void*_tmp73=Cyc_Tcutil_compress(
numt);_LL3F: if(_tmp73 <= (void*)4)goto _LL41;if(*((int*)_tmp73)!= 12)goto _LL41;
_LL40: if(topt != 0)return*topt;goto _LL3E;_LL41:;_LL42: goto _LL3E;_LL3E:;}return
numt;}static void Cyc_Tcpat_set_vd(struct Cyc_Absyn_Vardecl*vd,struct Cyc_List_List**
v_result_ptr,void*t){vd->type=t;vd->tq=Cyc_Absyn_empty_tqual(0);*v_result_ptr=(
struct Cyc_List_List*)({struct Cyc_List_List*_tmp74=_cycalloc(sizeof(*_tmp74));
_tmp74->hd=vd;_tmp74->tl=*v_result_ptr;_tmp74;});}static struct Cyc_Tcpat_TcPatResult
Cyc_Tcpat_combine_results(struct Cyc_Tcpat_TcPatResult res1,struct Cyc_Tcpat_TcPatResult
res2){struct _tuple12*_tmp76;struct Cyc_List_List*_tmp77;struct Cyc_Tcpat_TcPatResult
_tmp75=res1;_tmp76=_tmp75.tvars_and_bounds_opt;_tmp77=_tmp75.patvars;{struct
_tuple12*_tmp79;struct Cyc_List_List*_tmp7A;struct Cyc_Tcpat_TcPatResult _tmp78=
res2;_tmp79=_tmp78.tvars_and_bounds_opt;_tmp7A=_tmp78.patvars;if(_tmp76 != 0  || 
_tmp79 != 0){if(_tmp76 == 0)_tmp76=({struct _tuple12*_tmp7B=_cycalloc(sizeof(*
_tmp7B));_tmp7B->f1=0;_tmp7B->f2=0;_tmp7B;});if(_tmp79 == 0)_tmp79=({struct
_tuple12*_tmp7C=_cycalloc(sizeof(*_tmp7C));_tmp7C->f1=0;_tmp7C->f2=0;_tmp7C;});
return({struct Cyc_Tcpat_TcPatResult _tmp7D;_tmp7D.tvars_and_bounds_opt=({struct
_tuple12*_tmp7E=_cycalloc(sizeof(*_tmp7E));_tmp7E->f1=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*((struct _tuple12*)
_check_null(_tmp76))).f1,(*((struct _tuple12*)_check_null(_tmp79))).f1);_tmp7E->f2=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*((
struct _tuple12*)_check_null(_tmp76))).f2,(*((struct _tuple12*)_check_null(_tmp79))).f2);
_tmp7E;});_tmp7D.patvars=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct
Cyc_List_List*y))Cyc_List_append)(_tmp77,_tmp7A);_tmp7D;});}return({struct Cyc_Tcpat_TcPatResult
_tmp7F;_tmp7F.tvars_and_bounds_opt=0;_tmp7F.patvars=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp77,_tmp7A);
_tmp7F;});}}static struct Cyc_Absyn_Pat*Cyc_Tcpat_wild_pat(struct Cyc_Position_Segment*
loc){return({struct Cyc_Absyn_Pat*_tmp80=_cycalloc(sizeof(*_tmp80));_tmp80->loc=
loc;_tmp80->topt=0;_tmp80->r=(void*)0;_tmp80;});}struct _tuple15{struct Cyc_Absyn_Tqual
f1;void*f2;};struct _tuple16{struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Pat*f2;
};static struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_tcPatRec(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Absyn_Pat*p,void**topt,void**rgn_opt){Cyc_Tcpat_resolve_pat(te,topt,p);{
void*t;struct Cyc_Tcpat_TcPatResult res=({struct Cyc_Tcpat_TcPatResult _tmp155;
_tmp155.tvars_and_bounds_opt=0;_tmp155.patvars=0;_tmp155;});{void*_tmp81=p->r;
struct Cyc_Absyn_Vardecl*_tmp82;struct Cyc_Absyn_Pat*_tmp83;struct Cyc_Absyn_Vardecl*
_tmp84;struct Cyc_Absyn_Pat*_tmp85;struct Cyc_Absyn_Tvar*_tmp86;struct Cyc_Absyn_Vardecl*
_tmp87;void*_tmp88;void*_tmp89;void*_tmp8A;struct Cyc_Absyn_Enumdecl*_tmp8B;void*
_tmp8C;struct Cyc_Absyn_Pat*_tmp8D;struct Cyc_List_List*_tmp8E;struct Cyc_List_List**
_tmp8F;int _tmp90;struct Cyc_Absyn_AggrInfo*_tmp91;struct Cyc_Absyn_AggrInfo _tmp92;
union Cyc_Absyn_AggrInfoU _tmp93;struct Cyc_Absyn_Aggrdecl**_tmp94;struct Cyc_Absyn_Aggrdecl*
_tmp95;struct Cyc_List_List*_tmp96;struct Cyc_List_List**_tmp97;struct Cyc_List_List*
_tmp98;struct Cyc_List_List*_tmp99;struct Cyc_List_List**_tmp9A;int _tmp9B;struct
Cyc_Absyn_Datatypedecl*_tmp9C;struct Cyc_Absyn_Datatypefield*_tmp9D;struct Cyc_List_List*
_tmp9E;struct Cyc_List_List**_tmp9F;int _tmpA0;struct Cyc_Absyn_AggrInfo*_tmpA1;
struct Cyc_Absyn_AggrInfo*_tmpA2;struct Cyc_Absyn_AggrInfo _tmpA3;union Cyc_Absyn_AggrInfoU
_tmpA4;struct _tuple3 _tmpA5;_LL44: if((int)_tmp81 != 0)goto _LL46;_LL45: t=Cyc_Tcpat_any_type(
Cyc_Tcenv_lookup_type_vars(te),topt);goto _LL43;_LL46: if(_tmp81 <= (void*)2)goto
_LL5A;if(*((int*)_tmp81)!= 0)goto _LL48;_tmp82=((struct Cyc_Absyn_Var_p_struct*)
_tmp81)->f1;_tmp83=((struct Cyc_Absyn_Var_p_struct*)_tmp81)->f2;_LL47: res=Cyc_Tcpat_tcPatRec(
te,_tmp83,topt,rgn_opt);t=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp83->topt))->v;
Cyc_Tcpat_set_vd(_tmp82,& res.patvars,t);goto _LL43;_LL48: if(*((int*)_tmp81)!= 1)
goto _LL4A;_tmp84=((struct Cyc_Absyn_Reference_p_struct*)_tmp81)->f1;_tmp85=((
struct Cyc_Absyn_Reference_p_struct*)_tmp81)->f2;_LL49: res=Cyc_Tcpat_tcPatRec(te,
_tmp85,topt,rgn_opt);t=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp85->topt))->v;
if(rgn_opt == 0){({void*_tmpA6=0;Cyc_Tcutil_terr(p->loc,({const char*_tmpA7="* pattern would point into an unknown/unallowed region";
_tag_dyneither(_tmpA7,sizeof(char),55);}),_tag_dyneither(_tmpA6,sizeof(void*),0));});
goto _LL43;}else{if(Cyc_Tcutil_is_noalias_pointer(t))({void*_tmpA8=0;((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmpA9="* pattern cannot take the address of an alias-free path";_tag_dyneither(
_tmpA9,sizeof(char),56);}),_tag_dyneither(_tmpA8,sizeof(void*),0));});}Cyc_Tcpat_set_vd(
_tmp84,& res.patvars,(void*)({struct Cyc_Absyn_PointerType_struct*_tmpAA=_cycalloc(
sizeof(*_tmpAA));_tmpAA[0]=({struct Cyc_Absyn_PointerType_struct _tmpAB;_tmpAB.tag=
4;_tmpAB.f1=({struct Cyc_Absyn_PtrInfo _tmpAC;_tmpAC.elt_typ=t;_tmpAC.elt_tq=Cyc_Absyn_empty_tqual(
0);_tmpAC.ptr_atts=({struct Cyc_Absyn_PtrAtts _tmpAD;_tmpAD.rgn=*rgn_opt;_tmpAD.nullable=
Cyc_Absyn_false_conref;_tmpAD.bounds=Cyc_Absyn_empty_conref();_tmpAD.zero_term=((
union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();_tmpAD.ptrloc=0;_tmpAD;});
_tmpAC;});_tmpAB;});_tmpAA;}));goto _LL43;_LL4A: if(*((int*)_tmp81)!= 2)goto _LL4C;
_tmp86=((struct Cyc_Absyn_TagInt_p_struct*)_tmp81)->f1;_tmp87=((struct Cyc_Absyn_TagInt_p_struct*)
_tmp81)->f2;_LL4B: Cyc_Tcpat_set_vd(_tmp87,& res.patvars,_tmp87->type);{struct
_RegionHandle*_tmpAE=Cyc_Tcenv_get_fnrgn(te);Cyc_Tcenv_add_type_vars(_tmpAE,p->loc,
te,({struct Cyc_Absyn_Tvar*_tmpAF[1];_tmpAF[0]=_tmp86;((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpAF,sizeof(struct Cyc_Absyn_Tvar*),
1));}));if(res.tvars_and_bounds_opt == 0)res.tvars_and_bounds_opt=({struct
_tuple12*_tmpB0=_cycalloc(sizeof(*_tmpB0));_tmpB0->f1=0;_tmpB0->f2=0;_tmpB0;});(*
res.tvars_and_bounds_opt).f1=({struct Cyc_List_List*_tmpB1=_cycalloc(sizeof(*
_tmpB1));_tmpB1->hd=_tmp86;_tmpB1->tl=(*res.tvars_and_bounds_opt).f1;_tmpB1;});t=
Cyc_Absyn_uint_typ;goto _LL43;}_LL4C: if(*((int*)_tmp81)!= 7)goto _LL4E;_tmp88=(
void*)((struct Cyc_Absyn_Int_p_struct*)_tmp81)->f1;if((int)_tmp88 != 1)goto _LL4E;
_LL4D: t=Cyc_Tcpat_num_type(topt,Cyc_Absyn_uint_typ);goto _LL43;_LL4E: if(*((int*)
_tmp81)!= 7)goto _LL50;_tmp89=(void*)((struct Cyc_Absyn_Int_p_struct*)_tmp81)->f1;
if((int)_tmp89 != 2)goto _LL50;_LL4F: goto _LL51;_LL50: if(*((int*)_tmp81)!= 7)goto
_LL52;_tmp8A=(void*)((struct Cyc_Absyn_Int_p_struct*)_tmp81)->f1;if((int)_tmp8A != 
0)goto _LL52;_LL51: t=Cyc_Tcpat_num_type(topt,Cyc_Absyn_sint_typ);goto _LL43;_LL52:
if(*((int*)_tmp81)!= 8)goto _LL54;_LL53: t=Cyc_Tcpat_num_type(topt,Cyc_Absyn_char_typ);
goto _LL43;_LL54: if(*((int*)_tmp81)!= 9)goto _LL56;_LL55: t=Cyc_Tcpat_num_type(topt,
Cyc_Absyn_float_typ);goto _LL43;_LL56: if(*((int*)_tmp81)!= 10)goto _LL58;_tmp8B=((
struct Cyc_Absyn_Enum_p_struct*)_tmp81)->f1;_LL57: t=Cyc_Tcpat_num_type(topt,(void*)({
struct Cyc_Absyn_EnumType_struct*_tmpB2=_cycalloc(sizeof(*_tmpB2));_tmpB2[0]=({
struct Cyc_Absyn_EnumType_struct _tmpB3;_tmpB3.tag=12;_tmpB3.f1=_tmp8B->name;
_tmpB3.f2=(struct Cyc_Absyn_Enumdecl*)_tmp8B;_tmpB3;});_tmpB2;}));goto _LL43;_LL58:
if(*((int*)_tmp81)!= 11)goto _LL5A;_tmp8C=(void*)((struct Cyc_Absyn_AnonEnum_p_struct*)
_tmp81)->f1;_LL59: t=Cyc_Tcpat_num_type(topt,_tmp8C);goto _LL43;_LL5A: if((int)
_tmp81 != 1)goto _LL5C;_LL5B: if(topt != 0){void*_tmpB4=Cyc_Tcutil_compress(*topt);
_LL6F: if(_tmpB4 <= (void*)4)goto _LL71;if(*((int*)_tmpB4)!= 4)goto _LL71;_LL70: t=*
topt;goto tcpat_end;_LL71:;_LL72: goto _LL6E;_LL6E:;}{struct Cyc_Core_Opt*_tmpB5=Cyc_Tcenv_lookup_opt_type_vars(
te);t=(void*)({struct Cyc_Absyn_PointerType_struct*_tmpB6=_cycalloc(sizeof(*
_tmpB6));_tmpB6[0]=({struct Cyc_Absyn_PointerType_struct _tmpB7;_tmpB7.tag=4;
_tmpB7.f1=({struct Cyc_Absyn_PtrInfo _tmpB8;_tmpB8.elt_typ=Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_ak,_tmpB5);_tmpB8.elt_tq=Cyc_Absyn_empty_tqual(0);
_tmpB8.ptr_atts=({struct Cyc_Absyn_PtrAtts _tmpB9;_tmpB9.rgn=Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,_tmpB5);_tmpB9.nullable=Cyc_Absyn_true_conref;
_tmpB9.bounds=Cyc_Absyn_empty_conref();_tmpB9.zero_term=((union Cyc_Absyn_Constraint*(*)())
Cyc_Absyn_empty_conref)();_tmpB9.ptrloc=0;_tmpB9;});_tmpB8;});_tmpB7;});_tmpB6;});
goto _LL43;}_LL5C: if(_tmp81 <= (void*)2)goto _LL5E;if(*((int*)_tmp81)!= 4)goto _LL5E;
_tmp8D=((struct Cyc_Absyn_Pointer_p_struct*)_tmp81)->f1;_LL5D: {void*inner_typ=(
void*)0;void**_tmpBA=0;if(topt != 0){void*_tmpBB=Cyc_Tcutil_compress(*topt);
struct Cyc_Absyn_PtrInfo _tmpBC;void*_tmpBD;_LL74: if(_tmpBB <= (void*)4)goto _LL76;
if(*((int*)_tmpBB)!= 4)goto _LL76;_tmpBC=((struct Cyc_Absyn_PointerType_struct*)
_tmpBB)->f1;_tmpBD=_tmpBC.elt_typ;_LL75: inner_typ=_tmpBD;_tmpBA=(void**)&
inner_typ;goto _LL73;_LL76:;_LL77: goto _LL73;_LL73:;}{void*ptr_rgn=Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_trk,Cyc_Tcenv_lookup_opt_type_vars(te));res=Cyc_Tcpat_combine_results(
res,Cyc_Tcpat_tcPatRec(te,_tmp8D,_tmpBA,(void**)& ptr_rgn));if(_tmpBA == 0){void*
_tmpBE=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp8D->topt))->v);
struct Cyc_Absyn_DatatypeFieldInfo _tmpBF;union Cyc_Absyn_DatatypeFieldInfoU _tmpC0;
struct _tuple2 _tmpC1;struct Cyc_Absyn_Datatypedecl*_tmpC2;struct Cyc_Absyn_Datatypefield*
_tmpC3;struct Cyc_List_List*_tmpC4;_LL79: if(_tmpBE <= (void*)4)goto _LL7B;if(*((int*)
_tmpBE)!= 3)goto _LL7B;_tmpBF=((struct Cyc_Absyn_DatatypeFieldType_struct*)_tmpBE)->f1;
_tmpC0=_tmpBF.field_info;if((_tmpC0.KnownDatatypefield).tag != 2)goto _LL7B;_tmpC1=(
struct _tuple2)(_tmpC0.KnownDatatypefield).val;_tmpC2=_tmpC1.f1;_tmpC3=_tmpC1.f2;
_tmpC4=_tmpBF.targs;_LL7A: t=(void*)({struct Cyc_Absyn_DatatypeType_struct*_tmpC5=
_cycalloc(sizeof(*_tmpC5));_tmpC5[0]=({struct Cyc_Absyn_DatatypeType_struct _tmpC6;
_tmpC6.tag=2;_tmpC6.f1=({struct Cyc_Absyn_DatatypeInfo _tmpC7;_tmpC7.datatype_info=
Cyc_Absyn_KnownDatatype(({struct Cyc_Absyn_Datatypedecl**_tmpC9=_cycalloc(sizeof(*
_tmpC9));_tmpC9[0]=_tmpC2;_tmpC9;}));_tmpC7.targs=_tmpC4;_tmpC7.rgn=({struct Cyc_Core_Opt*
_tmpC8=_cycalloc(sizeof(*_tmpC8));_tmpC8->v=(void*)ptr_rgn;_tmpC8;});_tmpC7;});
_tmpC6;});_tmpC5;});goto _LL78;_LL7B:;_LL7C: t=(void*)({struct Cyc_Absyn_PointerType_struct*
_tmpCA=_cycalloc(sizeof(*_tmpCA));_tmpCA[0]=({struct Cyc_Absyn_PointerType_struct
_tmpCB;_tmpCB.tag=4;_tmpCB.f1=({struct Cyc_Absyn_PtrInfo _tmpCC;_tmpCC.elt_typ=(
void*)((struct Cyc_Core_Opt*)_check_null(_tmp8D->topt))->v;_tmpCC.elt_tq=Cyc_Absyn_empty_tqual(
0);_tmpCC.ptr_atts=({struct Cyc_Absyn_PtrAtts _tmpCD;_tmpCD.rgn=ptr_rgn;_tmpCD.nullable=((
union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();_tmpCD.bounds=Cyc_Absyn_empty_conref();
_tmpCD.zero_term=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();
_tmpCD.ptrloc=0;_tmpCD;});_tmpCC;});_tmpCB;});_tmpCA;});goto _LL78;_LL78:;}else{t=(
void*)({struct Cyc_Absyn_PointerType_struct*_tmpCE=_cycalloc(sizeof(*_tmpCE));
_tmpCE[0]=({struct Cyc_Absyn_PointerType_struct _tmpCF;_tmpCF.tag=4;_tmpCF.f1=({
struct Cyc_Absyn_PtrInfo _tmpD0;_tmpD0.elt_typ=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp8D->topt))->v;_tmpD0.elt_tq=Cyc_Absyn_empty_tqual(0);_tmpD0.ptr_atts=({
struct Cyc_Absyn_PtrAtts _tmpD1;_tmpD1.rgn=ptr_rgn;_tmpD1.nullable=((union Cyc_Absyn_Constraint*(*)())
Cyc_Absyn_empty_conref)();_tmpD1.bounds=Cyc_Absyn_empty_conref();_tmpD1.zero_term=((
union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();_tmpD1.ptrloc=0;_tmpD1;});
_tmpD0;});_tmpCF;});_tmpCE;});}goto _LL43;}}_LL5E: if(_tmp81 <= (void*)2)goto _LL60;
if(*((int*)_tmp81)!= 3)goto _LL60;_tmp8E=((struct Cyc_Absyn_Tuple_p_struct*)_tmp81)->f1;
_tmp8F=(struct Cyc_List_List**)&((struct Cyc_Absyn_Tuple_p_struct*)_tmp81)->f1;
_tmp90=((struct Cyc_Absyn_Tuple_p_struct*)_tmp81)->f2;_LL5F: {struct Cyc_List_List*
_tmpD2=*_tmp8F;struct Cyc_List_List*pat_ts=0;struct Cyc_List_List*topt_ts=0;if(
topt != 0){void*_tmpD3=Cyc_Tcutil_compress(*topt);struct Cyc_List_List*_tmpD4;
_LL7E: if(_tmpD3 <= (void*)4)goto _LL80;if(*((int*)_tmpD3)!= 9)goto _LL80;_tmpD4=((
struct Cyc_Absyn_TupleType_struct*)_tmpD3)->f1;_LL7F: topt_ts=_tmpD4;if(_tmp90){
int _tmpD5=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpD2);int _tmpD6=((
int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpD4);if(_tmpD5 < _tmpD6){struct
Cyc_List_List*wild_ps=0;{int i=0;for(0;i < _tmpD6 - _tmpD5;++ i){wild_ps=({struct Cyc_List_List*
_tmpD7=_cycalloc(sizeof(*_tmpD7));_tmpD7->hd=Cyc_Tcpat_wild_pat(p->loc);_tmpD7->tl=
wild_ps;_tmpD7;});}}*_tmp8F=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_imp_append)(_tmpD2,wild_ps);_tmpD2=*_tmp8F;}
else{if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpD2)== ((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmpD4))({void*_tmpD8=0;Cyc_Tcutil_warn(
p->loc,({const char*_tmpD9="unnecessary ... in tuple pattern";_tag_dyneither(
_tmpD9,sizeof(char),33);}),_tag_dyneither(_tmpD8,sizeof(void*),0));});}}goto
_LL7D;_LL80:;_LL81: goto _LL7D;_LL7D:;}else{if(_tmp90)({void*_tmpDA=0;Cyc_Tcutil_terr(
p->loc,({const char*_tmpDB="cannot determine missing fields for ... in tuple pattern";
_tag_dyneither(_tmpDB,sizeof(char),57);}),_tag_dyneither(_tmpDA,sizeof(void*),0));});}
for(0;_tmpD2 != 0;_tmpD2=_tmpD2->tl){void**_tmpDC=0;if(topt_ts != 0){_tmpDC=(void**)&(*((
struct _tuple15*)topt_ts->hd)).f2;topt_ts=topt_ts->tl;}res=Cyc_Tcpat_combine_results(
res,Cyc_Tcpat_tcPatRec(te,(struct Cyc_Absyn_Pat*)_tmpD2->hd,_tmpDC,rgn_opt));
pat_ts=({struct Cyc_List_List*_tmpDD=_cycalloc(sizeof(*_tmpDD));_tmpDD->hd=({
struct _tuple15*_tmpDE=_cycalloc(sizeof(*_tmpDE));_tmpDE->f1=Cyc_Absyn_empty_tqual(
0);_tmpDE->f2=(void*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Pat*)
_tmpD2->hd)->topt))->v;_tmpDE;});_tmpDD->tl=pat_ts;_tmpDD;});}t=(void*)({struct
Cyc_Absyn_TupleType_struct*_tmpDF=_cycalloc(sizeof(*_tmpDF));_tmpDF[0]=({struct
Cyc_Absyn_TupleType_struct _tmpE0;_tmpE0.tag=9;_tmpE0.f1=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(pat_ts);_tmpE0;});_tmpDF;});goto _LL43;}
_LL60: if(_tmp81 <= (void*)2)goto _LL62;if(*((int*)_tmp81)!= 5)goto _LL62;_tmp91=((
struct Cyc_Absyn_Aggr_p_struct*)_tmp81)->f1;if(_tmp91 == 0)goto _LL62;_tmp92=*
_tmp91;_tmp93=_tmp92.aggr_info;if((_tmp93.KnownAggr).tag != 2)goto _LL62;_tmp94=(
struct Cyc_Absyn_Aggrdecl**)(_tmp93.KnownAggr).val;_tmp95=*_tmp94;_tmp96=_tmp92.targs;
_tmp97=(struct Cyc_List_List**)&(*((struct Cyc_Absyn_Aggr_p_struct*)_tmp81)->f1).targs;
_tmp98=((struct Cyc_Absyn_Aggr_p_struct*)_tmp81)->f2;_tmp99=((struct Cyc_Absyn_Aggr_p_struct*)
_tmp81)->f3;_tmp9A=(struct Cyc_List_List**)&((struct Cyc_Absyn_Aggr_p_struct*)
_tmp81)->f3;_tmp9B=((struct Cyc_Absyn_Aggr_p_struct*)_tmp81)->f4;_LL61: {struct
Cyc_List_List*_tmpE1=*_tmp9A;struct _dyneither_ptr aggr_str=_tmp95->kind == (void*)
0?({const char*_tmp128="struct";_tag_dyneither(_tmp128,sizeof(char),7);}):({const
char*_tmp129="union";_tag_dyneither(_tmp129,sizeof(char),6);});if(_tmp95->impl == 
0){({struct Cyc_String_pa_struct _tmpE4;_tmpE4.tag=0;_tmpE4.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)aggr_str);{void*_tmpE2[1]={& _tmpE4};Cyc_Tcutil_terr(
p->loc,({const char*_tmpE3="can't destructure an abstract %s";_tag_dyneither(
_tmpE3,sizeof(char),33);}),_tag_dyneither(_tmpE2,sizeof(void*),1));}});t=Cyc_Absyn_wildtyp(
Cyc_Tcenv_lookup_opt_type_vars(te));goto _LL43;}if(_tmp98 != 0  || _tmp95->kind == (
void*)1  && ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp95->impl))->tagged)
rgn_opt=0;{struct _RegionHandle _tmpE5=_new_region("rgn");struct _RegionHandle*rgn=&
_tmpE5;_push_region(rgn);{struct Cyc_List_List*_tmpE6=0;struct Cyc_List_List*
outlives_constraints=0;struct Cyc_List_List*_tmpE7=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp95->impl))->exist_vars;{struct Cyc_List_List*t=_tmp98;for(0;t != 0;
t=t->tl){struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)t->hd;struct Cyc_Absyn_Tvar*
uv=(struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)_check_null(_tmpE7))->hd;_tmpE7=
_tmpE7->tl;{void*_tmpE8=Cyc_Absyn_compress_kb(tv->kind);void*_tmpE9=Cyc_Absyn_compress_kb(
uv->kind);int error=0;void*k2;{void*_tmpEA=_tmpE9;void*_tmpEB;void*_tmpEC;_LL83:
if(*((int*)_tmpEA)!= 2)goto _LL85;_tmpEB=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmpEA)->f2;_LL84: _tmpEC=_tmpEB;goto _LL86;_LL85: if(*((int*)_tmpEA)!= 0)goto _LL87;
_tmpEC=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmpEA)->f1;_LL86: k2=_tmpEC;goto
_LL82;_LL87:;_LL88:({void*_tmpED=0;((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmpEE="unconstrained existential type variable in aggregate";
_tag_dyneither(_tmpEE,sizeof(char),53);}),_tag_dyneither(_tmpED,sizeof(void*),0));});
_LL82:;}{void*_tmpEF=_tmpE8;void*_tmpF0;struct Cyc_Core_Opt*_tmpF1;struct Cyc_Core_Opt**
_tmpF2;void*_tmpF3;struct Cyc_Core_Opt*_tmpF4;struct Cyc_Core_Opt**_tmpF5;_LL8A:
if(*((int*)_tmpEF)!= 0)goto _LL8C;_tmpF0=(void*)((struct Cyc_Absyn_Eq_kb_struct*)
_tmpEF)->f1;_LL8B: if(!Cyc_Tcutil_kind_leq(k2,_tmpF0))error=1;goto _LL89;_LL8C: if(*((
int*)_tmpEF)!= 2)goto _LL8E;_tmpF1=((struct Cyc_Absyn_Less_kb_struct*)_tmpEF)->f1;
_tmpF2=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)_tmpEF)->f1;
_tmpF3=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmpEF)->f2;_LL8D: _tmpF5=_tmpF2;
goto _LL8F;_LL8E: if(*((int*)_tmpEF)!= 1)goto _LL89;_tmpF4=((struct Cyc_Absyn_Unknown_kb_struct*)
_tmpEF)->f1;_tmpF5=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)
_tmpEF)->f1;_LL8F:*_tmpF5=({struct Cyc_Core_Opt*_tmpF6=_cycalloc(sizeof(*_tmpF6));
_tmpF6->v=(void*)_tmpE9;_tmpF6;});goto _LL89;_LL89:;}if(error)({struct Cyc_String_pa_struct
_tmpFB;_tmpFB.tag=0;_tmpFB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(
k2));{struct Cyc_String_pa_struct _tmpFA;_tmpFA.tag=0;_tmpFA.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kindbound2string(_tmpE8));{
struct Cyc_String_pa_struct _tmpF9;_tmpF9.tag=0;_tmpF9.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)*tv->name);{void*_tmpF7[3]={& _tmpF9,& _tmpFA,& _tmpFB};Cyc_Tcutil_terr(
p->loc,({const char*_tmpF8="type variable %s has kind %s but must have at least kind %s";
_tag_dyneither(_tmpF8,sizeof(char),60);}),_tag_dyneither(_tmpF7,sizeof(void*),3));}}}});{
void*vartype=(void*)({struct Cyc_Absyn_VarType_struct*_tmp101=_cycalloc(sizeof(*
_tmp101));_tmp101[0]=({struct Cyc_Absyn_VarType_struct _tmp102;_tmp102.tag=1;
_tmp102.f1=tv;_tmp102;});_tmp101;});_tmpE6=({struct Cyc_List_List*_tmpFC=
_region_malloc(rgn,sizeof(*_tmpFC));_tmpFC->hd=(void*)((void*)vartype);_tmpFC->tl=
_tmpE6;_tmpFC;});if(k2 == (void*)3)outlives_constraints=({struct Cyc_List_List*
_tmpFD=_cycalloc(sizeof(*_tmpFD));_tmpFD->hd=({struct _tuple0*_tmpFE=_cycalloc(
sizeof(*_tmpFE));_tmpFE->f1=Cyc_Absyn_empty_effect;_tmpFE->f2=vartype;_tmpFE;});
_tmpFD->tl=outlives_constraints;_tmpFD;});else{if(k2 == (void*)4  || k2 == (void*)5)({
void*_tmpFF=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp100="opened existential had unique or top region kind";
_tag_dyneither(_tmp100,sizeof(char),49);}),_tag_dyneither(_tmpFF,sizeof(void*),0));});}}}}}
_tmpE6=Cyc_List_imp_rev(_tmpE6);{struct _RegionHandle*_tmp103=Cyc_Tcenv_get_fnrgn(
te);struct Cyc_Tcenv_Tenv*te2=Cyc_Tcenv_add_type_vars(_tmp103,p->loc,te,_tmp98);
struct Cyc_List_List*_tmp104=Cyc_Tcenv_lookup_type_vars(te2);struct _tuple10
_tmp105=({struct _tuple10 _tmp127;_tmp127.f1=_tmp104;_tmp127.f2=rgn;_tmp127;});
struct Cyc_List_List*_tmp106=((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct _tuple11*(*f)(struct _tuple10*,struct Cyc_Absyn_Tvar*),struct _tuple10*env,
struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,& _tmp105,
_tmp95->tvs);struct Cyc_List_List*_tmp107=((struct Cyc_List_List*(*)(struct
_RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(rgn,rgn,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp95->impl))->exist_vars,
_tmpE6);struct Cyc_List_List*_tmp108=((struct Cyc_List_List*(*)(void*(*f)(struct
_tuple11*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple11*))Cyc_Core_snd,
_tmp106);struct Cyc_List_List*_tmp109=((struct Cyc_List_List*(*)(void*(*f)(struct
_tuple11*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple11*))Cyc_Core_snd,
_tmp107);struct Cyc_List_List*_tmp10A=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(
rgn,_tmp106,_tmp107);if(_tmp98 != 0  || ((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp95->impl))->rgn_po != 0){if(res.tvars_and_bounds_opt == 0)res.tvars_and_bounds_opt=({
struct _tuple12*_tmp10B=_cycalloc(sizeof(*_tmp10B));_tmp10B->f1=0;_tmp10B->f2=0;
_tmp10B;});(*res.tvars_and_bounds_opt).f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_append)((*res.tvars_and_bounds_opt).f1,_tmp98);(*
res.tvars_and_bounds_opt).f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_append)((*res.tvars_and_bounds_opt).f2,
outlives_constraints);{struct Cyc_List_List*_tmp10C=0;{struct Cyc_List_List*
_tmp10D=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp95->impl))->rgn_po;for(0;
_tmp10D != 0;_tmp10D=_tmp10D->tl){_tmp10C=({struct Cyc_List_List*_tmp10E=_cycalloc(
sizeof(*_tmp10E));_tmp10E->hd=({struct _tuple0*_tmp10F=_cycalloc(sizeof(*_tmp10F));
_tmp10F->f1=Cyc_Tcutil_rsubstitute(rgn,_tmp10A,(*((struct _tuple0*)_tmp10D->hd)).f1);
_tmp10F->f2=Cyc_Tcutil_rsubstitute(rgn,_tmp10A,(*((struct _tuple0*)_tmp10D->hd)).f2);
_tmp10F;});_tmp10E->tl=_tmp10C;_tmp10E;});}}_tmp10C=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp10C);(*res.tvars_and_bounds_opt).f2=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*
res.tvars_and_bounds_opt).f2,_tmp10C);}}*_tmp97=_tmp108;t=(void*)({struct Cyc_Absyn_AggrType_struct*
_tmp110=_cycalloc(sizeof(*_tmp110));_tmp110[0]=({struct Cyc_Absyn_AggrType_struct
_tmp111;_tmp111.tag=10;_tmp111.f1=({struct Cyc_Absyn_AggrInfo _tmp112;_tmp112.aggr_info=
Cyc_Absyn_KnownAggr(({struct Cyc_Absyn_Aggrdecl**_tmp113=_cycalloc(sizeof(*
_tmp113));_tmp113[0]=_tmp95;_tmp113;}));_tmp112.targs=*_tmp97;_tmp112;});_tmp111;});
_tmp110;});if(_tmp9B  && _tmp95->kind == (void*)1)({void*_tmp114=0;Cyc_Tcutil_warn(
p->loc,({const char*_tmp115="`...' pattern not allowed in union pattern";
_tag_dyneither(_tmp115,sizeof(char),43);}),_tag_dyneither(_tmp114,sizeof(void*),
0));});else{if(_tmp9B){int _tmp116=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmpE1);int _tmp117=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp95->impl))->fields);if(_tmp116 < _tmp117){struct Cyc_List_List*
wild_dps=0;{int i=0;for(0;i < _tmp117 - _tmp116;++ i){wild_dps=({struct Cyc_List_List*
_tmp118=_cycalloc(sizeof(*_tmp118));_tmp118->hd=({struct _tuple14*_tmp119=
_cycalloc(sizeof(*_tmp119));_tmp119->f1=0;_tmp119->f2=Cyc_Tcpat_wild_pat(p->loc);
_tmp119;});_tmp118->tl=wild_dps;_tmp118;});}}*_tmp9A=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmpE1,
wild_dps);_tmpE1=*_tmp9A;}else{if(_tmp116 == _tmp117)({void*_tmp11A=0;Cyc_Tcutil_warn(
p->loc,({const char*_tmp11B="unnecessary ... in struct pattern";_tag_dyneither(
_tmp11B,sizeof(char),34);}),_tag_dyneither(_tmp11A,sizeof(void*),0));});}}}{
struct Cyc_List_List*fields=((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,
struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,void*,struct Cyc_List_List*
fields))Cyc_Tcutil_resolve_aggregate_designators)(rgn,p->loc,_tmpE1,_tmp95->kind,((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp95->impl))->fields);for(0;fields != 
0;fields=fields->tl){struct _tuple16 _tmp11D;struct Cyc_Absyn_Aggrfield*_tmp11E;
struct Cyc_Absyn_Pat*_tmp11F;struct _tuple16*_tmp11C=(struct _tuple16*)fields->hd;
_tmp11D=*_tmp11C;_tmp11E=_tmp11D.f1;_tmp11F=_tmp11D.f2;{void*_tmp120=Cyc_Tcutil_rsubstitute(
rgn,_tmp10A,_tmp11E->type);res=Cyc_Tcpat_combine_results(res,Cyc_Tcpat_tcPatRec(
te2,_tmp11F,(void**)& _tmp120,rgn_opt));if(!Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp11F->topt))->v,_tmp120))({struct Cyc_String_pa_struct _tmp126;
_tmp126.tag=0;_tmp126.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp11F->topt))->v));{struct Cyc_String_pa_struct
_tmp125;_tmp125.tag=0;_tmp125.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp120));{struct Cyc_String_pa_struct _tmp124;_tmp124.tag=
0;_tmp124.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)aggr_str);{struct Cyc_String_pa_struct
_tmp123;_tmp123.tag=0;_tmp123.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*
_tmp11E->name);{void*_tmp121[4]={& _tmp123,& _tmp124,& _tmp125,& _tmp126};Cyc_Tcutil_terr(
p->loc,({const char*_tmp122="field %s of %s pattern expects type %s != %s";
_tag_dyneither(_tmp122,sizeof(char),45);}),_tag_dyneither(_tmp121,sizeof(void*),
4));}}}}});}}}}}_npop_handler(0);goto _LL43;;_pop_region(rgn);}}_LL62: if(_tmp81 <= (
void*)2)goto _LL64;if(*((int*)_tmp81)!= 6)goto _LL64;_tmp9C=((struct Cyc_Absyn_Datatype_p_struct*)
_tmp81)->f1;_tmp9D=((struct Cyc_Absyn_Datatype_p_struct*)_tmp81)->f2;_tmp9E=((
struct Cyc_Absyn_Datatype_p_struct*)_tmp81)->f3;_tmp9F=(struct Cyc_List_List**)&((
struct Cyc_Absyn_Datatype_p_struct*)_tmp81)->f3;_tmpA0=((struct Cyc_Absyn_Datatype_p_struct*)
_tmp81)->f4;_LL63: {struct Cyc_List_List*_tmp12A=*_tmp9F;struct _RegionHandle*
_tmp12B=Cyc_Tcenv_get_fnrgn(te);{struct Cyc_List_List*tqts=_tmp9D->typs;struct Cyc_List_List*
_tmp12C=Cyc_Tcenv_lookup_type_vars(te);struct _tuple10 _tmp12D=({struct _tuple10
_tmp153;_tmp153.f1=_tmp12C;_tmp153.f2=_tmp12B;_tmp153;});struct Cyc_List_List*
_tmp12E=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple11*(*f)(
struct _tuple10*,struct Cyc_Absyn_Tvar*),struct _tuple10*env,struct Cyc_List_List*x))
Cyc_List_rmap_c)(_tmp12B,Cyc_Tcutil_r_make_inst_var,& _tmp12D,_tmp9C->tvs);struct
Cyc_List_List*_tmp12F=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple11*),
struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple11*))Cyc_Core_snd,
_tmp12E);if(tqts == 0){struct Cyc_Core_Opt*ropt=({struct Cyc_Core_Opt*_tmp134=
_cycalloc(sizeof(*_tmp134));_tmp134->v=(void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_rk,({struct Cyc_Core_Opt*_tmp135=_cycalloc(sizeof(*_tmp135));_tmp135->v=
_tmp12C;_tmp135;}));_tmp134;});t=(void*)({struct Cyc_Absyn_DatatypeType_struct*
_tmp130=_cycalloc(sizeof(*_tmp130));_tmp130[0]=({struct Cyc_Absyn_DatatypeType_struct
_tmp131;_tmp131.tag=2;_tmp131.f1=({struct Cyc_Absyn_DatatypeInfo _tmp132;_tmp132.datatype_info=
Cyc_Absyn_KnownDatatype(({struct Cyc_Absyn_Datatypedecl**_tmp133=_cycalloc(
sizeof(*_tmp133));_tmp133[0]=_tmp9C;_tmp133;}));_tmp132.targs=_tmp12F;_tmp132.rgn=
ropt;_tmp132;});_tmp131;});_tmp130;});}else{t=(void*)({struct Cyc_Absyn_DatatypeFieldType_struct*
_tmp136=_cycalloc(sizeof(*_tmp136));_tmp136[0]=({struct Cyc_Absyn_DatatypeFieldType_struct
_tmp137;_tmp137.tag=3;_tmp137.f1=({struct Cyc_Absyn_DatatypeFieldInfo _tmp138;
_tmp138.field_info=Cyc_Absyn_KnownDatatypefield(_tmp9C,_tmp9D);_tmp138.targs=
_tmp12F;_tmp138;});_tmp137;});_tmp136;});}if(topt != 0  && tqts == 0){void*_tmp139=
Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_DatatypeInfo _tmp13A;struct Cyc_List_List*
_tmp13B;_LL91: if(_tmp139 <= (void*)4)goto _LL95;if(*((int*)_tmp139)!= 3)goto _LL93;
_LL92: t=(void*)({struct Cyc_Absyn_DatatypeFieldType_struct*_tmp13C=_cycalloc(
sizeof(*_tmp13C));_tmp13C[0]=({struct Cyc_Absyn_DatatypeFieldType_struct _tmp13D;
_tmp13D.tag=3;_tmp13D.f1=({struct Cyc_Absyn_DatatypeFieldInfo _tmp13E;_tmp13E.field_info=
Cyc_Absyn_KnownDatatypefield(_tmp9C,_tmp9D);_tmp13E.targs=_tmp12F;_tmp13E;});
_tmp13D;});_tmp13C;});goto _LL90;_LL93: if(*((int*)_tmp139)!= 2)goto _LL95;_tmp13A=((
struct Cyc_Absyn_DatatypeType_struct*)_tmp139)->f1;_tmp13B=_tmp13A.targs;_LL94: {
struct Cyc_List_List*_tmp13F=_tmp12F;for(0;_tmp13F != 0  && _tmp13B != 0;(_tmp13F=
_tmp13F->tl,_tmp13B=_tmp13B->tl)){Cyc_Tcutil_unify((void*)_tmp13F->hd,(void*)
_tmp13B->hd);}goto _LL90;}_LL95:;_LL96: goto _LL90;_LL90:;}if(_tmpA0){int _tmp140=((
int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp12A);int _tmp141=((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(tqts);if(_tmp140 < _tmp141){struct Cyc_List_List*
wild_ps=0;{int i=0;for(0;i < _tmp141 - _tmp140;++ i){wild_ps=({struct Cyc_List_List*
_tmp142=_cycalloc(sizeof(*_tmp142));_tmp142->hd=Cyc_Tcpat_wild_pat(p->loc);
_tmp142->tl=wild_ps;_tmp142;});}}*_tmp9F=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp12A,wild_ps);_tmp12A=*_tmp9F;}
else{if(_tmp140 == _tmp141)({struct Cyc_String_pa_struct _tmp145;_tmp145.tag=0;
_tmp145.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp9C->name));{void*_tmp143[1]={& _tmp145};Cyc_Tcutil_warn(p->loc,({const char*
_tmp144="unnecessary ... in datatype field %s";_tag_dyneither(_tmp144,sizeof(
char),37);}),_tag_dyneither(_tmp143,sizeof(void*),1));}});}}for(0;_tmp12A != 0
 && tqts != 0;(_tmp12A=_tmp12A->tl,tqts=tqts->tl)){struct Cyc_Absyn_Pat*_tmp146=(
struct Cyc_Absyn_Pat*)_tmp12A->hd;void*_tmp147=Cyc_Tcutil_rsubstitute(_tmp12B,
_tmp12E,(*((struct _tuple15*)tqts->hd)).f2);res=Cyc_Tcpat_combine_results(res,Cyc_Tcpat_tcPatRec(
te,_tmp146,(void**)& _tmp147,rgn_opt));if(!Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp146->topt))->v,_tmp147))({struct Cyc_String_pa_struct _tmp14C;
_tmp14C.tag=0;_tmp14C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp146->topt))->v));{struct Cyc_String_pa_struct
_tmp14B;_tmp14B.tag=0;_tmp14B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp147));{struct Cyc_String_pa_struct _tmp14A;_tmp14A.tag=
0;_tmp14A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp9D->name));{void*_tmp148[3]={& _tmp14A,& _tmp14B,& _tmp14C};Cyc_Tcutil_terr(
_tmp146->loc,({const char*_tmp149="%s expects argument type %s, not %s";
_tag_dyneither(_tmp149,sizeof(char),36);}),_tag_dyneither(_tmp148,sizeof(void*),
3));}}}});}if(_tmp12A != 0)({struct Cyc_String_pa_struct _tmp14F;_tmp14F.tag=0;
_tmp14F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp9D->name));{void*_tmp14D[1]={& _tmp14F};Cyc_Tcutil_terr(p->loc,({const char*
_tmp14E="too many arguments for datatype constructor %s";_tag_dyneither(_tmp14E,
sizeof(char),47);}),_tag_dyneither(_tmp14D,sizeof(void*),1));}});if(tqts != 0)({
struct Cyc_String_pa_struct _tmp152;_tmp152.tag=0;_tmp152.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp9D->name));{void*_tmp150[1]={&
_tmp152};Cyc_Tcutil_terr(p->loc,({const char*_tmp151="too few arguments for datatype constructor %s";
_tag_dyneither(_tmp151,sizeof(char),46);}),_tag_dyneither(_tmp150,sizeof(void*),
1));}});}goto _LL43;}_LL64: if(_tmp81 <= (void*)2)goto _LL66;if(*((int*)_tmp81)!= 5)
goto _LL66;_tmpA1=((struct Cyc_Absyn_Aggr_p_struct*)_tmp81)->f1;if(_tmpA1 != 0)goto
_LL66;_LL65: goto _LL67;_LL66: if(_tmp81 <= (void*)2)goto _LL68;if(*((int*)_tmp81)!= 
5)goto _LL68;_tmpA2=((struct Cyc_Absyn_Aggr_p_struct*)_tmp81)->f1;if(_tmpA2 == 0)
goto _LL68;_tmpA3=*_tmpA2;_tmpA4=_tmpA3.aggr_info;if((_tmpA4.UnknownAggr).tag != 1)
goto _LL68;_tmpA5=(struct _tuple3)(_tmpA4.UnknownAggr).val;_LL67: goto _LL69;_LL68:
if(_tmp81 <= (void*)2)goto _LL6A;if(*((int*)_tmp81)!= 12)goto _LL6A;_LL69: goto _LL6B;
_LL6A: if(_tmp81 <= (void*)2)goto _LL6C;if(*((int*)_tmp81)!= 14)goto _LL6C;_LL6B:
goto _LL6D;_LL6C: if(_tmp81 <= (void*)2)goto _LL43;if(*((int*)_tmp81)!= 13)goto _LL43;
_LL6D: t=Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));goto _LL43;_LL43:;}
tcpat_end: p->topt=({struct Cyc_Core_Opt*_tmp154=_cycalloc(sizeof(*_tmp154));
_tmp154->v=(void*)t;_tmp154;});return res;}}struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_tcPat(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,void**topt){struct Cyc_Tcpat_TcPatResult
_tmp156=Cyc_Tcpat_tcPatRec(te,p,(void**)topt,0);struct _RegionHandle*_tmp157=Cyc_Tcenv_get_fnrgn(
te);Cyc_Tcutil_check_unique_vars(((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct _dyneither_ptr*(*f)(struct Cyc_Absyn_Vardecl*),struct Cyc_List_List*x))Cyc_List_rmap)(
_tmp157,Cyc_Tcpat_get_name,_tmp156.patvars),p->loc,({const char*_tmp158="pattern contains a repeated variable";
_tag_dyneither(_tmp158,sizeof(char),37);}));return _tmp156;}void Cyc_Tcpat_check_pat_regions(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p){void*_tmp159=p->r;struct Cyc_Absyn_Pat*
_tmp15A;struct Cyc_List_List*_tmp15B;struct Cyc_List_List*_tmp15C;struct Cyc_List_List*
_tmp15D;_LL98: if(_tmp159 <= (void*)2)goto _LLA0;if(*((int*)_tmp159)!= 4)goto _LL9A;
_tmp15A=((struct Cyc_Absyn_Pointer_p_struct*)_tmp159)->f1;_LL99: Cyc_Tcpat_check_pat_regions(
te,_tmp15A);{struct _RegionHandle*_tmp15E=Cyc_Tcenv_get_fnrgn(te);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(
_tmp15E,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp15A->topt))->v)){({void*
_tmp15F=0;Cyc_Tcutil_terr(p->loc,({const char*_tmp160="Pattern dereferences to a non-aliased pointer; use swap";
_tag_dyneither(_tmp160,sizeof(char),56);}),_tag_dyneither(_tmp15F,sizeof(void*),
0));});return;}if(Cyc_Tcutil_is_noalias_pointer_or_aggr(_tmp15E,(void*)((struct
Cyc_Core_Opt*)_check_null(p->topt))->v)){({void*_tmp161=0;Cyc_Tcutil_terr(p->loc,({
const char*_tmp162="Pattern dereferences a non-aliased pointer; use swap";
_tag_dyneither(_tmp162,sizeof(char),53);}),_tag_dyneither(_tmp161,sizeof(void*),
0));});return;}{void*_tmp163=(void*)((struct Cyc_Core_Opt*)_check_null(p->topt))->v;
struct Cyc_Absyn_PtrInfo _tmp164;struct Cyc_Absyn_PtrAtts _tmp165;void*_tmp166;
struct Cyc_Absyn_DatatypeInfo _tmp167;struct Cyc_Core_Opt*_tmp168;struct Cyc_Core_Opt
_tmp169;void*_tmp16A;_LLA3: if(_tmp163 <= (void*)4)goto _LLA7;if(*((int*)_tmp163)!= 
4)goto _LLA5;_tmp164=((struct Cyc_Absyn_PointerType_struct*)_tmp163)->f1;_tmp165=
_tmp164.ptr_atts;_tmp166=_tmp165.rgn;_LLA4: _tmp16A=_tmp166;goto _LLA6;_LLA5: if(*((
int*)_tmp163)!= 2)goto _LLA7;_tmp167=((struct Cyc_Absyn_DatatypeType_struct*)
_tmp163)->f1;_tmp168=_tmp167.rgn;if(_tmp168 == 0)goto _LLA7;_tmp169=*_tmp168;
_tmp16A=(void*)_tmp169.v;_LLA6: Cyc_Tcenv_check_rgn_accessible(te,p->loc,_tmp16A);
return;_LLA7:;_LLA8:({void*_tmp16B=0;((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp16C="check_pat_regions: bad pointer type";
_tag_dyneither(_tmp16C,sizeof(char),36);}),_tag_dyneither(_tmp16B,sizeof(void*),
0));});_LLA2:;}}_LL9A: if(*((int*)_tmp159)!= 6)goto _LL9C;_tmp15B=((struct Cyc_Absyn_Datatype_p_struct*)
_tmp159)->f3;_LL9B: for(0;_tmp15B != 0;_tmp15B=_tmp15B->tl){Cyc_Tcpat_check_pat_regions(
te,(struct Cyc_Absyn_Pat*)_tmp15B->hd);}{void*_tmp16D=(void*)((struct Cyc_Core_Opt*)
_check_null(p->topt))->v;struct Cyc_Absyn_DatatypeInfo _tmp16E;struct Cyc_Core_Opt*
_tmp16F;struct Cyc_Absyn_DatatypeInfo _tmp170;struct Cyc_Core_Opt*_tmp171;struct Cyc_Core_Opt
_tmp172;void*_tmp173;_LLAA: if(_tmp16D <= (void*)4)goto _LLB0;if(*((int*)_tmp16D)!= 
2)goto _LLAC;_tmp16E=((struct Cyc_Absyn_DatatypeType_struct*)_tmp16D)->f1;_tmp16F=
_tmp16E.rgn;if(_tmp16F != 0)goto _LLAC;_LLAB: return;_LLAC: if(*((int*)_tmp16D)!= 2)
goto _LLAE;_tmp170=((struct Cyc_Absyn_DatatypeType_struct*)_tmp16D)->f1;_tmp171=
_tmp170.rgn;if(_tmp171 == 0)goto _LLAE;_tmp172=*_tmp171;_tmp173=(void*)_tmp172.v;
_LLAD: Cyc_Tcenv_check_rgn_accessible(te,p->loc,_tmp173);return;_LLAE: if(*((int*)
_tmp16D)!= 3)goto _LLB0;_LLAF: return;_LLB0:;_LLB1:({void*_tmp174=0;((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmp175="check_pat_regions: bad datatype type";_tag_dyneither(_tmp175,sizeof(
char),37);}),_tag_dyneither(_tmp174,sizeof(void*),0));});_LLA9:;}_LL9C: if(*((int*)
_tmp159)!= 5)goto _LL9E;_tmp15C=((struct Cyc_Absyn_Aggr_p_struct*)_tmp159)->f3;
_LL9D: for(0;_tmp15C != 0;_tmp15C=_tmp15C->tl){Cyc_Tcpat_check_pat_regions(te,(*((
struct _tuple14*)_tmp15C->hd)).f2);}return;_LL9E: if(*((int*)_tmp159)!= 3)goto
_LLA0;_tmp15D=((struct Cyc_Absyn_Tuple_p_struct*)_tmp159)->f1;_LL9F: for(0;_tmp15D
!= 0;_tmp15D=_tmp15D->tl){Cyc_Tcpat_check_pat_regions(te,(struct Cyc_Absyn_Pat*)
_tmp15D->hd);}return;_LLA0:;_LLA1: return;_LL97:;}struct _union_Name_value_Name_v{
int tag;struct _dyneither_ptr val;};struct _union_Name_value_Int_v{int tag;int val;};
union Cyc_Tcpat_Name_value{struct _union_Name_value_Name_v Name_v;struct
_union_Name_value_Int_v Int_v;};union Cyc_Tcpat_Name_value Cyc_Tcpat_Name_v(struct
_dyneither_ptr s){return({union Cyc_Tcpat_Name_value _tmp176;(_tmp176.Name_v).val=s;(
_tmp176.Name_v).tag=1;_tmp176;});}union Cyc_Tcpat_Name_value Cyc_Tcpat_Int_v(int i){
return({union Cyc_Tcpat_Name_value _tmp177;(_tmp177.Int_v).val=i;(_tmp177.Int_v).tag=
2;_tmp177;});}struct Cyc_Tcpat_Con_s{union Cyc_Tcpat_Name_value name;int arity;int*
span;struct Cyc_Absyn_Pat*orig_pat;};struct Cyc_Tcpat_Con_struct{int tag;struct Cyc_Tcpat_Con_s*
f1;struct Cyc_List_List*f2;};static int Cyc_Tcpat_compare_con(struct Cyc_Tcpat_Con_s*
c1,struct Cyc_Tcpat_Con_s*c2){union Cyc_Tcpat_Name_value _tmp178=c1->name;struct
_dyneither_ptr _tmp179;int _tmp17A;_LLB3: if((_tmp178.Name_v).tag != 1)goto _LLB5;
_tmp179=(struct _dyneither_ptr)(_tmp178.Name_v).val;_LLB4: {union Cyc_Tcpat_Name_value
_tmp17B=c2->name;struct _dyneither_ptr _tmp17C;int _tmp17D;_LLB8: if((_tmp17B.Name_v).tag
!= 1)goto _LLBA;_tmp17C=(struct _dyneither_ptr)(_tmp17B.Name_v).val;_LLB9: return
Cyc_strcmp((struct _dyneither_ptr)_tmp179,(struct _dyneither_ptr)_tmp17C);_LLBA:
if((_tmp17B.Int_v).tag != 2)goto _LLB7;_tmp17D=(int)(_tmp17B.Int_v).val;_LLBB:
return - 1;_LLB7:;}_LLB5: if((_tmp178.Int_v).tag != 2)goto _LLB2;_tmp17A=(int)(
_tmp178.Int_v).val;_LLB6: {union Cyc_Tcpat_Name_value _tmp17E=c2->name;struct
_dyneither_ptr _tmp17F;int _tmp180;_LLBD: if((_tmp17E.Name_v).tag != 1)goto _LLBF;
_tmp17F=(struct _dyneither_ptr)(_tmp17E.Name_v).val;_LLBE: return 1;_LLBF: if((
_tmp17E.Int_v).tag != 2)goto _LLBC;_tmp180=(int)(_tmp17E.Int_v).val;_LLC0: return
_tmp17A - _tmp180;_LLBC:;}_LLB2:;}static struct Cyc_Set_Set*Cyc_Tcpat_empty_con_set(
struct _RegionHandle*r){return((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,int(*
cmp)(struct Cyc_Tcpat_Con_s*,struct Cyc_Tcpat_Con_s*)))Cyc_Set_rempty)(r,Cyc_Tcpat_compare_con);}
static int Cyc_Tcpat_one_opt=1;static int Cyc_Tcpat_two_opt=2;static int Cyc_Tcpat_twofiftysix_opt=
256;static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_null_con(struct _RegionHandle*r,struct
Cyc_Absyn_Pat*p){return({struct Cyc_Tcpat_Con_s*_tmp181=_region_malloc(r,sizeof(*
_tmp181));_tmp181->name=Cyc_Tcpat_Name_v(({const char*_tmp182="NULL";
_tag_dyneither(_tmp182,sizeof(char),5);}));_tmp181->arity=0;_tmp181->span=(int*)&
Cyc_Tcpat_two_opt;_tmp181->orig_pat=(struct Cyc_Absyn_Pat*)p;_tmp181;});}static
struct Cyc_Tcpat_Con_s*Cyc_Tcpat_null_ptr_con(struct _RegionHandle*r,struct Cyc_Absyn_Pat*
p){return({struct Cyc_Tcpat_Con_s*_tmp183=_region_malloc(r,sizeof(*_tmp183));
_tmp183->name=Cyc_Tcpat_Name_v(({const char*_tmp184="&";_tag_dyneither(_tmp184,
sizeof(char),2);}));_tmp183->arity=1;_tmp183->span=(int*)& Cyc_Tcpat_two_opt;
_tmp183->orig_pat=(struct Cyc_Absyn_Pat*)p;_tmp183;});}static struct Cyc_Tcpat_Con_s*
Cyc_Tcpat_ptr_con(struct _RegionHandle*r,struct Cyc_Absyn_Pat*p){return({struct Cyc_Tcpat_Con_s*
_tmp185=_region_malloc(r,sizeof(*_tmp185));_tmp185->name=Cyc_Tcpat_Name_v(({
const char*_tmp186="&";_tag_dyneither(_tmp186,sizeof(char),2);}));_tmp185->arity=
1;_tmp185->span=(int*)& Cyc_Tcpat_one_opt;_tmp185->orig_pat=(struct Cyc_Absyn_Pat*)
p;_tmp185;});}static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_int_con(struct _RegionHandle*
r,int i,struct Cyc_Absyn_Pat*p){return({struct Cyc_Tcpat_Con_s*_tmp187=
_region_malloc(r,sizeof(*_tmp187));_tmp187->name=Cyc_Tcpat_Int_v(i);_tmp187->arity=
0;_tmp187->span=0;_tmp187->orig_pat=p;_tmp187;});}static struct Cyc_Tcpat_Con_s*
Cyc_Tcpat_float_con(struct _RegionHandle*r,struct _dyneither_ptr f,struct Cyc_Absyn_Pat*
p){return({struct Cyc_Tcpat_Con_s*_tmp188=_region_malloc(r,sizeof(*_tmp188));
_tmp188->name=Cyc_Tcpat_Name_v(f);_tmp188->arity=0;_tmp188->span=0;_tmp188->orig_pat=(
struct Cyc_Absyn_Pat*)p;_tmp188;});}static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_char_con(
struct _RegionHandle*r,char c,struct Cyc_Absyn_Pat*p){return({struct Cyc_Tcpat_Con_s*
_tmp189=_region_malloc(r,sizeof(*_tmp189));_tmp189->name=Cyc_Tcpat_Int_v((int)c);
_tmp189->arity=0;_tmp189->span=(int*)& Cyc_Tcpat_twofiftysix_opt;_tmp189->orig_pat=(
struct Cyc_Absyn_Pat*)p;_tmp189;});}static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_tuple_con(
struct _RegionHandle*r,int i,struct Cyc_Absyn_Pat*p){return({struct Cyc_Tcpat_Con_s*
_tmp18A=_region_malloc(r,sizeof(*_tmp18A));_tmp18A->name=Cyc_Tcpat_Name_v(({
const char*_tmp18B="$";_tag_dyneither(_tmp18B,sizeof(char),2);}));_tmp18A->arity=
i;_tmp18A->span=(int*)& Cyc_Tcpat_one_opt;_tmp18A->orig_pat=p;_tmp18A;});}static
void*Cyc_Tcpat_null_pat(struct _RegionHandle*r,struct Cyc_Absyn_Pat*p){return(void*)({
struct Cyc_Tcpat_Con_struct*_tmp18C=_region_malloc(r,sizeof(*_tmp18C));_tmp18C[0]=({
struct Cyc_Tcpat_Con_struct _tmp18D;_tmp18D.tag=0;_tmp18D.f1=Cyc_Tcpat_null_con(r,
p);_tmp18D.f2=0;_tmp18D;});_tmp18C;});}static void*Cyc_Tcpat_int_pat(struct
_RegionHandle*r,int i,struct Cyc_Absyn_Pat*p){return(void*)({struct Cyc_Tcpat_Con_struct*
_tmp18E=_region_malloc(r,sizeof(*_tmp18E));_tmp18E[0]=({struct Cyc_Tcpat_Con_struct
_tmp18F;_tmp18F.tag=0;_tmp18F.f1=Cyc_Tcpat_int_con(r,i,p);_tmp18F.f2=0;_tmp18F;});
_tmp18E;});}static void*Cyc_Tcpat_char_pat(struct _RegionHandle*r,char c,struct Cyc_Absyn_Pat*
p){return(void*)({struct Cyc_Tcpat_Con_struct*_tmp190=_region_malloc(r,sizeof(*
_tmp190));_tmp190[0]=({struct Cyc_Tcpat_Con_struct _tmp191;_tmp191.tag=0;_tmp191.f1=
Cyc_Tcpat_char_con(r,c,p);_tmp191.f2=0;_tmp191;});_tmp190;});}static void*Cyc_Tcpat_float_pat(
struct _RegionHandle*r,struct _dyneither_ptr f,struct Cyc_Absyn_Pat*p){return(void*)({
struct Cyc_Tcpat_Con_struct*_tmp192=_region_malloc(r,sizeof(*_tmp192));_tmp192[0]=({
struct Cyc_Tcpat_Con_struct _tmp193;_tmp193.tag=0;_tmp193.f1=Cyc_Tcpat_float_con(r,
f,p);_tmp193.f2=0;_tmp193;});_tmp192;});}static void*Cyc_Tcpat_null_ptr_pat(
struct _RegionHandle*r,void*p,struct Cyc_Absyn_Pat*p0){return(void*)({struct Cyc_Tcpat_Con_struct*
_tmp194=_region_malloc(r,sizeof(*_tmp194));_tmp194[0]=({struct Cyc_Tcpat_Con_struct
_tmp195;_tmp195.tag=0;_tmp195.f1=Cyc_Tcpat_null_ptr_con(r,p0);_tmp195.f2=({
struct Cyc_List_List*_tmp196=_region_malloc(r,sizeof(*_tmp196));_tmp196->hd=(void*)
p;_tmp196->tl=0;_tmp196;});_tmp195;});_tmp194;});}static void*Cyc_Tcpat_ptr_pat(
struct _RegionHandle*r,void*p,struct Cyc_Absyn_Pat*p0){return(void*)({struct Cyc_Tcpat_Con_struct*
_tmp197=_region_malloc(r,sizeof(*_tmp197));_tmp197[0]=({struct Cyc_Tcpat_Con_struct
_tmp198;_tmp198.tag=0;_tmp198.f1=Cyc_Tcpat_ptr_con(r,p0);_tmp198.f2=({struct Cyc_List_List*
_tmp199=_region_malloc(r,sizeof(*_tmp199));_tmp199->hd=(void*)p;_tmp199->tl=0;
_tmp199;});_tmp198;});_tmp197;});}static void*Cyc_Tcpat_tuple_pat(struct
_RegionHandle*r,struct Cyc_List_List*ss,struct Cyc_Absyn_Pat*p){return(void*)({
struct Cyc_Tcpat_Con_struct*_tmp19A=_region_malloc(r,sizeof(*_tmp19A));_tmp19A[0]=({
struct Cyc_Tcpat_Con_struct _tmp19B;_tmp19B.tag=0;_tmp19B.f1=Cyc_Tcpat_tuple_con(r,
Cyc_List_length(ss),p);_tmp19B.f2=ss;_tmp19B;});_tmp19A;});}static void*Cyc_Tcpat_con_pat(
struct _RegionHandle*r,struct _dyneither_ptr con_name,int*span,struct Cyc_List_List*
ps,struct Cyc_Absyn_Pat*p){struct Cyc_Tcpat_Con_s*c=({struct Cyc_Tcpat_Con_s*
_tmp19E=_region_malloc(r,sizeof(*_tmp19E));_tmp19E->name=Cyc_Tcpat_Name_v(
con_name);_tmp19E->arity=Cyc_List_length(ps);_tmp19E->span=span;_tmp19E->orig_pat=(
struct Cyc_Absyn_Pat*)p;_tmp19E;});return(void*)({struct Cyc_Tcpat_Con_struct*
_tmp19C=_region_malloc(r,sizeof(*_tmp19C));_tmp19C[0]=({struct Cyc_Tcpat_Con_struct
_tmp19D;_tmp19D.tag=0;_tmp19D.f1=c;_tmp19D.f2=ps;_tmp19D;});_tmp19C;});}static
void*Cyc_Tcpat_compile_pat(struct _RegionHandle*r,struct Cyc_Absyn_Pat*p){void*s;{
void*_tmp19F=p->r;void*_tmp1A0;int _tmp1A1;char _tmp1A2;struct _dyneither_ptr
_tmp1A3;struct Cyc_Absyn_Pat*_tmp1A4;struct Cyc_Absyn_Pat*_tmp1A5;struct Cyc_Absyn_Pat*
_tmp1A6;struct Cyc_Absyn_Datatypedecl*_tmp1A7;struct Cyc_Absyn_Datatypefield*
_tmp1A8;struct Cyc_List_List*_tmp1A9;struct Cyc_List_List*_tmp1AA;struct Cyc_Absyn_AggrInfo*
_tmp1AB;struct Cyc_Absyn_AggrInfo _tmp1AC;union Cyc_Absyn_AggrInfoU _tmp1AD;struct
Cyc_Absyn_Aggrdecl**_tmp1AE;struct Cyc_Absyn_Aggrdecl*_tmp1AF;struct Cyc_List_List*
_tmp1B0;struct Cyc_Absyn_Enumdecl*_tmp1B1;struct Cyc_Absyn_Enumfield*_tmp1B2;void*
_tmp1B3;struct Cyc_Absyn_Enumfield*_tmp1B4;_LLC2: if((int)_tmp19F != 0)goto _LLC4;
_LLC3: goto _LLC5;_LLC4: if(_tmp19F <= (void*)2)goto _LLC6;if(*((int*)_tmp19F)!= 2)
goto _LLC6;_LLC5: s=(void*)0;goto _LLC1;_LLC6: if((int)_tmp19F != 1)goto _LLC8;_LLC7: s=
Cyc_Tcpat_null_pat(r,p);goto _LLC1;_LLC8: if(_tmp19F <= (void*)2)goto _LLCA;if(*((
int*)_tmp19F)!= 7)goto _LLCA;_tmp1A0=(void*)((struct Cyc_Absyn_Int_p_struct*)
_tmp19F)->f1;_tmp1A1=((struct Cyc_Absyn_Int_p_struct*)_tmp19F)->f2;_LLC9: s=Cyc_Tcpat_int_pat(
r,_tmp1A1,(struct Cyc_Absyn_Pat*)p);goto _LLC1;_LLCA: if(_tmp19F <= (void*)2)goto
_LLCC;if(*((int*)_tmp19F)!= 8)goto _LLCC;_tmp1A2=((struct Cyc_Absyn_Char_p_struct*)
_tmp19F)->f1;_LLCB: s=Cyc_Tcpat_char_pat(r,_tmp1A2,p);goto _LLC1;_LLCC: if(_tmp19F
<= (void*)2)goto _LLCE;if(*((int*)_tmp19F)!= 9)goto _LLCE;_tmp1A3=((struct Cyc_Absyn_Float_p_struct*)
_tmp19F)->f1;_LLCD: s=Cyc_Tcpat_float_pat(r,_tmp1A3,p);goto _LLC1;_LLCE: if(_tmp19F
<= (void*)2)goto _LLD0;if(*((int*)_tmp19F)!= 0)goto _LLD0;_tmp1A4=((struct Cyc_Absyn_Var_p_struct*)
_tmp19F)->f2;_LLCF: s=Cyc_Tcpat_compile_pat(r,_tmp1A4);goto _LLC1;_LLD0: if(_tmp19F
<= (void*)2)goto _LLD2;if(*((int*)_tmp19F)!= 1)goto _LLD2;_tmp1A5=((struct Cyc_Absyn_Reference_p_struct*)
_tmp19F)->f2;_LLD1: s=Cyc_Tcpat_compile_pat(r,_tmp1A5);goto _LLC1;_LLD2: if(_tmp19F
<= (void*)2)goto _LLD4;if(*((int*)_tmp19F)!= 4)goto _LLD4;_tmp1A6=((struct Cyc_Absyn_Pointer_p_struct*)
_tmp19F)->f1;_LLD3:{void*_tmp1B5=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(p->topt))->v);struct Cyc_Absyn_PtrInfo _tmp1B6;struct Cyc_Absyn_PtrAtts
_tmp1B7;union Cyc_Absyn_Constraint*_tmp1B8;struct Cyc_Absyn_DatatypeInfo _tmp1B9;
struct Cyc_Core_Opt*_tmp1BA;struct Cyc_Core_Opt _tmp1BB;_LLE7: if(_tmp1B5 <= (void*)4)
goto _LLEB;if(*((int*)_tmp1B5)!= 4)goto _LLE9;_tmp1B6=((struct Cyc_Absyn_PointerType_struct*)
_tmp1B5)->f1;_tmp1B7=_tmp1B6.ptr_atts;_tmp1B8=_tmp1B7.nullable;_LLE8: {int
is_nullable=0;int still_working=1;while(still_working){union Cyc_Absyn_Constraint*
_tmp1BC=_tmp1B8;union Cyc_Absyn_Constraint _tmp1BD;union Cyc_Absyn_Constraint*
_tmp1BE;union Cyc_Absyn_Constraint _tmp1BF;int _tmp1C0;union Cyc_Absyn_Constraint
_tmp1C1;int _tmp1C2;_LLEE: _tmp1BD=*_tmp1BC;if((_tmp1BD.Forward_constr).tag != 2)
goto _LLF0;_tmp1BE=(union Cyc_Absyn_Constraint*)(_tmp1BD.Forward_constr).val;_LLEF:*
_tmp1B8=*_tmp1BE;continue;_LLF0: _tmp1BF=*_tmp1BC;if((_tmp1BF.No_constr).tag != 3)
goto _LLF2;_tmp1C0=(int)(_tmp1BF.No_constr).val;_LLF1:({struct
_union_Constraint_Eq_constr*_tmp1C3=& _tmp1B8->Eq_constr;_tmp1C3->tag=1;_tmp1C3->val=
0;});is_nullable=0;still_working=0;goto _LLED;_LLF2: _tmp1C1=*_tmp1BC;if((_tmp1C1.Eq_constr).tag
!= 1)goto _LLED;_tmp1C2=(int)(_tmp1C1.Eq_constr).val;_LLF3: is_nullable=(int)
_tmp1C2;still_working=0;goto _LLED;_LLED:;}{void*ss=Cyc_Tcpat_compile_pat(r,
_tmp1A6);if(is_nullable)s=Cyc_Tcpat_null_ptr_pat(r,ss,p);else{s=Cyc_Tcpat_ptr_pat(
r,ss,p);}goto _LLE6;}}_LLE9: if(*((int*)_tmp1B5)!= 2)goto _LLEB;_tmp1B9=((struct Cyc_Absyn_DatatypeType_struct*)
_tmp1B5)->f1;_tmp1BA=_tmp1B9.rgn;if(_tmp1BA == 0)goto _LLEB;_tmp1BB=*_tmp1BA;_LLEA:{
void*_tmp1C4=_tmp1A6->r;struct Cyc_Absyn_Datatypedecl*_tmp1C5;struct Cyc_Absyn_Datatypefield*
_tmp1C6;struct Cyc_List_List*_tmp1C7;_LLF5: if(_tmp1C4 <= (void*)2)goto _LLF7;if(*((
int*)_tmp1C4)!= 6)goto _LLF7;_tmp1C5=((struct Cyc_Absyn_Datatype_p_struct*)_tmp1C4)->f1;
_tmp1C6=((struct Cyc_Absyn_Datatype_p_struct*)_tmp1C4)->f2;_tmp1C7=((struct Cyc_Absyn_Datatype_p_struct*)
_tmp1C4)->f3;_LLF6: {int*span;if(_tmp1C5->is_extensible)span=0;else{span=({int*
_tmp1C8=_region_malloc(r,sizeof(*_tmp1C8));_tmp1C8[0]=((int(*)(struct Cyc_List_List*
x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(
_tmp1C5->fields))->v);_tmp1C8;});}s=Cyc_Tcpat_con_pat(r,*(*_tmp1C6->name).f2,
span,((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct
_RegionHandle*,struct Cyc_Absyn_Pat*),struct _RegionHandle*env,struct Cyc_List_List*
x))Cyc_List_rmap_c)(r,Cyc_Tcpat_compile_pat,r,_tmp1C7),p);goto _LLF4;}_LLF7:;
_LLF8:({void*_tmp1C9=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(({const char*_tmp1CA="non-datatype pattern has datatype type";
_tag_dyneither(_tmp1CA,sizeof(char),39);}),_tag_dyneither(_tmp1C9,sizeof(void*),
0));});_LLF4:;}goto _LLE6;_LLEB:;_LLEC:({void*_tmp1CB=0;((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp1CC="pointer pattern does not have pointer type";
_tag_dyneither(_tmp1CC,sizeof(char),43);}),_tag_dyneither(_tmp1CB,sizeof(void*),
0));});_LLE6:;}goto _LLC1;_LLD4: if(_tmp19F <= (void*)2)goto _LLD6;if(*((int*)
_tmp19F)!= 6)goto _LLD6;_tmp1A7=((struct Cyc_Absyn_Datatype_p_struct*)_tmp19F)->f1;
_tmp1A8=((struct Cyc_Absyn_Datatype_p_struct*)_tmp19F)->f2;_tmp1A9=((struct Cyc_Absyn_Datatype_p_struct*)
_tmp19F)->f3;_LLD5: {int*span;{void*_tmp1CD=Cyc_Tcutil_compress((void*)((struct
Cyc_Core_Opt*)_check_null(p->topt))->v);_LLFA: if(_tmp1CD <= (void*)4)goto _LLFE;
if(*((int*)_tmp1CD)!= 2)goto _LLFC;_LLFB: if(_tmp1A7->is_extensible)span=0;else{
span=({int*_tmp1CE=_region_malloc(r,sizeof(*_tmp1CE));_tmp1CE[0]=((int(*)(struct
Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp1A7->fields))->v);_tmp1CE;});}goto _LLF9;_LLFC: if(*((int*)_tmp1CD)
!= 3)goto _LLFE;_LLFD: span=(int*)& Cyc_Tcpat_one_opt;goto _LLF9;_LLFE:;_LLFF: span=({
void*_tmp1CF=0;((int*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp1D0="void datatype pattern has bad type";_tag_dyneither(_tmp1D0,
sizeof(char),35);}),_tag_dyneither(_tmp1CF,sizeof(void*),0));});goto _LLF9;_LLF9:;}
s=Cyc_Tcpat_con_pat(r,*(*_tmp1A8->name).f2,span,((struct Cyc_List_List*(*)(struct
_RegionHandle*,void*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Pat*),struct
_RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcpat_compile_pat,
r,_tmp1A9),p);goto _LLC1;}_LLD6: if(_tmp19F <= (void*)2)goto _LLD8;if(*((int*)
_tmp19F)!= 3)goto _LLD8;_tmp1AA=((struct Cyc_Absyn_Tuple_p_struct*)_tmp19F)->f1;
_LLD7: s=Cyc_Tcpat_tuple_pat(r,((struct Cyc_List_List*(*)(struct _RegionHandle*,
void*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Pat*),struct _RegionHandle*env,
struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcpat_compile_pat,r,_tmp1AA),(
struct Cyc_Absyn_Pat*)p);goto _LLC1;_LLD8: if(_tmp19F <= (void*)2)goto _LLDA;if(*((
int*)_tmp19F)!= 5)goto _LLDA;_tmp1AB=((struct Cyc_Absyn_Aggr_p_struct*)_tmp19F)->f1;
if(_tmp1AB == 0)goto _LLDA;_tmp1AC=*_tmp1AB;_tmp1AD=_tmp1AC.aggr_info;if((_tmp1AD.KnownAggr).tag
!= 2)goto _LLDA;_tmp1AE=(struct Cyc_Absyn_Aggrdecl**)(_tmp1AD.KnownAggr).val;
_tmp1AF=*_tmp1AE;_tmp1B0=((struct Cyc_Absyn_Aggr_p_struct*)_tmp19F)->f3;_LLD9: if(
_tmp1AF->kind == (void*)0){struct Cyc_List_List*ps=0;{struct Cyc_List_List*fields=((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp1AF->impl))->fields;for(0;fields != 
0;fields=fields->tl){int found=Cyc_strcmp((struct _dyneither_ptr)*((struct Cyc_Absyn_Aggrfield*)
fields->hd)->name,({const char*_tmp1DF="";_tag_dyneither(_tmp1DF,sizeof(char),1);}))
== 0;{struct Cyc_List_List*dlps0=_tmp1B0;for(0;!found  && dlps0 != 0;dlps0=dlps0->tl){
struct _tuple14 _tmp1D2;struct Cyc_List_List*_tmp1D3;struct Cyc_Absyn_Pat*_tmp1D4;
struct _tuple14*_tmp1D1=(struct _tuple14*)dlps0->hd;_tmp1D2=*_tmp1D1;_tmp1D3=
_tmp1D2.f1;_tmp1D4=_tmp1D2.f2;{struct Cyc_List_List*_tmp1D5=_tmp1D3;struct Cyc_List_List
_tmp1D6;void*_tmp1D7;struct _dyneither_ptr*_tmp1D8;struct Cyc_List_List*_tmp1D9;
_LL101: if(_tmp1D5 == 0)goto _LL103;_tmp1D6=*_tmp1D5;_tmp1D7=(void*)_tmp1D6.hd;if(*((
int*)_tmp1D7)!= 1)goto _LL103;_tmp1D8=((struct Cyc_Absyn_FieldName_struct*)_tmp1D7)->f1;
_tmp1D9=_tmp1D6.tl;if(_tmp1D9 != 0)goto _LL103;_LL102: if(Cyc_strptrcmp(_tmp1D8,((
struct Cyc_Absyn_Aggrfield*)fields->hd)->name)== 0){ps=({struct Cyc_List_List*
_tmp1DA=_region_malloc(r,sizeof(*_tmp1DA));_tmp1DA->hd=(void*)Cyc_Tcpat_compile_pat(
r,_tmp1D4);_tmp1DA->tl=ps;_tmp1DA;});found=1;}goto _LL100;_LL103:;_LL104:({void*
_tmp1DB=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp1DC="bad designator(s)";_tag_dyneither(_tmp1DC,sizeof(char),18);}),
_tag_dyneither(_tmp1DB,sizeof(void*),0));});_LL100:;}}}if(!found)({void*_tmp1DD=
0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp1DE="bad designator";_tag_dyneither(_tmp1DE,sizeof(char),15);}),
_tag_dyneither(_tmp1DD,sizeof(void*),0));});}}s=Cyc_Tcpat_tuple_pat(r,ps,(struct
Cyc_Absyn_Pat*)p);}else{if(!((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp1AF->impl))->tagged)({
void*_tmp1E0=0;Cyc_Tcutil_terr(p->loc,({const char*_tmp1E1="patterns on untagged unions not yet supported.";
_tag_dyneither(_tmp1E1,sizeof(char),47);}),_tag_dyneither(_tmp1E0,sizeof(void*),
0));});{int*span=({int*_tmp1F0=_region_malloc(r,sizeof(*_tmp1F0));_tmp1F0[0]=((
int(*)(struct Cyc_List_List*x))Cyc_List_length)(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp1AF->impl))->fields);_tmp1F0;});int field_name;struct Cyc_List_List*
_tmp1E2=_tmp1B0;struct Cyc_List_List _tmp1E3;struct _tuple14*_tmp1E4;struct _tuple14
_tmp1E5;struct Cyc_List_List*_tmp1E6;struct Cyc_List_List _tmp1E7;void*_tmp1E8;
struct _dyneither_ptr*_tmp1E9;struct Cyc_List_List*_tmp1EA;struct Cyc_Absyn_Pat*
_tmp1EB;struct Cyc_List_List*_tmp1EC;_LL106: if(_tmp1E2 == 0)goto _LL108;_tmp1E3=*
_tmp1E2;_tmp1E4=(struct _tuple14*)_tmp1E3.hd;_tmp1E5=*_tmp1E4;_tmp1E6=_tmp1E5.f1;
if(_tmp1E6 == 0)goto _LL108;_tmp1E7=*_tmp1E6;_tmp1E8=(void*)_tmp1E7.hd;if(*((int*)
_tmp1E8)!= 1)goto _LL108;_tmp1E9=((struct Cyc_Absyn_FieldName_struct*)_tmp1E8)->f1;
_tmp1EA=_tmp1E7.tl;if(_tmp1EA != 0)goto _LL108;_tmp1EB=_tmp1E5.f2;_tmp1EC=_tmp1E3.tl;
if(_tmp1EC != 0)goto _LL108;_LL107: s=Cyc_Tcpat_con_pat(r,*_tmp1E9,span,({struct Cyc_List_List*
_tmp1ED=_region_malloc(r,sizeof(*_tmp1ED));_tmp1ED->hd=(void*)Cyc_Tcpat_compile_pat(
r,_tmp1EB);_tmp1ED->tl=0;_tmp1ED;}),p);goto _LL105;_LL108:;_LL109:({void*_tmp1EE=
0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp1EF="bad union pattern";_tag_dyneither(_tmp1EF,sizeof(char),18);}),
_tag_dyneither(_tmp1EE,sizeof(void*),0));});_LL105:;}}goto _LLC1;_LLDA: if(_tmp19F
<= (void*)2)goto _LLDC;if(*((int*)_tmp19F)!= 10)goto _LLDC;_tmp1B1=((struct Cyc_Absyn_Enum_p_struct*)
_tmp19F)->f1;_tmp1B2=((struct Cyc_Absyn_Enum_p_struct*)_tmp19F)->f2;_LLDB: {int
span=((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp1B1->fields))->v);s=Cyc_Tcpat_con_pat(r,*(*
_tmp1B2->name).f2,({int*_tmp1F1=_region_malloc(r,sizeof(*_tmp1F1));_tmp1F1[0]=
span;_tmp1F1;}),0,p);goto _LLC1;}_LLDC: if(_tmp19F <= (void*)2)goto _LLDE;if(*((int*)
_tmp19F)!= 11)goto _LLDE;_tmp1B3=(void*)((struct Cyc_Absyn_AnonEnum_p_struct*)
_tmp19F)->f1;_tmp1B4=((struct Cyc_Absyn_AnonEnum_p_struct*)_tmp19F)->f2;_LLDD: {
struct Cyc_List_List*fields;{void*_tmp1F2=Cyc_Tcutil_compress(_tmp1B3);struct Cyc_List_List*
_tmp1F3;_LL10B: if(_tmp1F2 <= (void*)4)goto _LL10D;if(*((int*)_tmp1F2)!= 13)goto
_LL10D;_tmp1F3=((struct Cyc_Absyn_AnonEnumType_struct*)_tmp1F2)->f1;_LL10C: fields=
_tmp1F3;goto _LL10A;_LL10D:;_LL10E:({void*_tmp1F4=0;((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp1F5="bad type in AnonEnum_p";
_tag_dyneither(_tmp1F5,sizeof(char),23);}),_tag_dyneither(_tmp1F4,sizeof(void*),
0));});_LL10A:;}{int span=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(fields);
s=Cyc_Tcpat_con_pat(r,*(*_tmp1B4->name).f2,({int*_tmp1F6=_region_malloc(r,
sizeof(*_tmp1F6));_tmp1F6[0]=span;_tmp1F6;}),0,p);goto _LLC1;}}_LLDE: if(_tmp19F <= (
void*)2)goto _LLE0;if(*((int*)_tmp19F)!= 12)goto _LLE0;_LLDF: goto _LLE1;_LLE0: if(
_tmp19F <= (void*)2)goto _LLE2;if(*((int*)_tmp19F)!= 13)goto _LLE2;_LLE1: goto _LLE3;
_LLE2: if(_tmp19F <= (void*)2)goto _LLE4;if(*((int*)_tmp19F)!= 5)goto _LLE4;_LLE3:
goto _LLE5;_LLE4: if(_tmp19F <= (void*)2)goto _LLC1;if(*((int*)_tmp19F)!= 14)goto
_LLC1;_LLE5: s=(void*)0;_LLC1:;}return s;}struct Cyc_Tcpat_Pos_struct{int tag;struct
Cyc_Tcpat_Con_s*f1;struct Cyc_List_List*f2;};struct Cyc_Tcpat_Neg_struct{int tag;
struct Cyc_Set_Set*f1;};struct Cyc_Tcpat_Failure_struct{int tag;void*f1;};struct Cyc_Tcpat_Success_struct{
int tag;void*f1;};struct Cyc_Tcpat_IfEq_struct{int tag;struct Cyc_List_List*f1;
struct Cyc_Tcpat_Con_s*f2;void*f3;void*f4;};struct _tuple17{struct Cyc_List_List*f1;
struct Cyc_List_List*f2;struct Cyc_List_List*f3;};static void*Cyc_Tcpat_add_neg(
struct _RegionHandle*r,void*td,struct Cyc_Tcpat_Con_s*c){void*_tmp1F7=td;struct Cyc_Set_Set*
_tmp1F8;_LL110: if(*((int*)_tmp1F7)!= 1)goto _LL112;_tmp1F8=((struct Cyc_Tcpat_Neg_struct*)
_tmp1F7)->f1;_LL111: return(void*)({struct Cyc_Tcpat_Neg_struct*_tmp1F9=
_region_malloc(r,sizeof(*_tmp1F9));_tmp1F9[0]=({struct Cyc_Tcpat_Neg_struct
_tmp1FA;_tmp1FA.tag=1;_tmp1FA.f1=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,
struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_rinsert)(r,_tmp1F8,c);
_tmp1FA;});_tmp1F9;});_LL112: if(*((int*)_tmp1F7)!= 0)goto _LL10F;_LL113:({void*
_tmp1FB=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp1FC="add_neg called when td is Positive";_tag_dyneither(_tmp1FC,
sizeof(char),35);}),_tag_dyneither(_tmp1FB,sizeof(void*),0));});_LL10F:;}static
void*Cyc_Tcpat_static_match(struct Cyc_Tcpat_Con_s*c,void*td){void*_tmp1FD=td;
struct Cyc_Tcpat_Con_s*_tmp1FE;struct Cyc_Set_Set*_tmp1FF;_LL115: if(*((int*)
_tmp1FD)!= 0)goto _LL117;_tmp1FE=((struct Cyc_Tcpat_Pos_struct*)_tmp1FD)->f1;
_LL116: if(Cyc_Tcpat_compare_con(c,_tmp1FE)== 0)return(void*)0;else{return(void*)
1;}_LL117: if(*((int*)_tmp1FD)!= 1)goto _LL114;_tmp1FF=((struct Cyc_Tcpat_Neg_struct*)
_tmp1FD)->f1;_LL118: if(((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))
Cyc_Set_member)(_tmp1FF,c))return(void*)1;else{if(c->span != 0  && *((int*)
_check_null(c->span))== ((int(*)(struct Cyc_Set_Set*s))Cyc_Set_cardinality)(
_tmp1FF)+ 1)return(void*)0;else{return(void*)2;}}_LL114:;}struct _tuple18{struct
Cyc_Tcpat_Con_s*f1;struct Cyc_List_List*f2;};static struct Cyc_List_List*Cyc_Tcpat_augment(
struct _RegionHandle*r,struct Cyc_List_List*ctxt,void*dsc){struct Cyc_List_List*
_tmp200=ctxt;struct Cyc_List_List _tmp201;struct _tuple18*_tmp202;struct _tuple18
_tmp203;struct Cyc_Tcpat_Con_s*_tmp204;struct Cyc_List_List*_tmp205;struct Cyc_List_List*
_tmp206;_LL11A: if(_tmp200 != 0)goto _LL11C;_LL11B: return 0;_LL11C: if(_tmp200 == 0)
goto _LL119;_tmp201=*_tmp200;_tmp202=(struct _tuple18*)_tmp201.hd;_tmp203=*_tmp202;
_tmp204=_tmp203.f1;_tmp205=_tmp203.f2;_tmp206=_tmp201.tl;_LL11D: return({struct
Cyc_List_List*_tmp207=_region_malloc(r,sizeof(*_tmp207));_tmp207->hd=({struct
_tuple18*_tmp208=_region_malloc(r,sizeof(*_tmp208));_tmp208->f1=_tmp204;_tmp208->f2=(
struct Cyc_List_List*)({struct Cyc_List_List*_tmp209=_region_malloc(r,sizeof(*
_tmp209));_tmp209->hd=(void*)dsc;_tmp209->tl=_tmp205;_tmp209;});_tmp208;});
_tmp207->tl=_tmp206;_tmp207;});_LL119:;}static struct Cyc_List_List*Cyc_Tcpat_norm_context(
struct _RegionHandle*r,struct Cyc_List_List*ctxt){struct Cyc_List_List*_tmp20A=ctxt;
struct Cyc_List_List _tmp20B;struct _tuple18*_tmp20C;struct _tuple18 _tmp20D;struct
Cyc_Tcpat_Con_s*_tmp20E;struct Cyc_List_List*_tmp20F;struct Cyc_List_List*_tmp210;
_LL11F: if(_tmp20A != 0)goto _LL121;_LL120:({void*_tmp211=0;((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp212="norm_context: empty context";
_tag_dyneither(_tmp212,sizeof(char),28);}),_tag_dyneither(_tmp211,sizeof(void*),
0));});_LL121: if(_tmp20A == 0)goto _LL11E;_tmp20B=*_tmp20A;_tmp20C=(struct _tuple18*)
_tmp20B.hd;_tmp20D=*_tmp20C;_tmp20E=_tmp20D.f1;_tmp20F=_tmp20D.f2;_tmp210=
_tmp20B.tl;_LL122: return Cyc_Tcpat_augment(r,_tmp210,(void*)({struct Cyc_Tcpat_Pos_struct*
_tmp213=_region_malloc(r,sizeof(*_tmp213));_tmp213[0]=({struct Cyc_Tcpat_Pos_struct
_tmp214;_tmp214.tag=0;_tmp214.f1=_tmp20E;_tmp214.f2=Cyc_List_rrev(r,_tmp20F);
_tmp214;});_tmp213;}));_LL11E:;}static void*Cyc_Tcpat_build_desc(struct
_RegionHandle*r,struct Cyc_List_List*ctxt,void*dsc,struct Cyc_List_List*work){
struct _tuple12 _tmp216=({struct _tuple12 _tmp215;_tmp215.f1=ctxt;_tmp215.f2=work;
_tmp215;});struct Cyc_List_List*_tmp217;struct Cyc_List_List*_tmp218;struct Cyc_List_List*
_tmp219;struct Cyc_List_List*_tmp21A;struct Cyc_List_List*_tmp21B;struct Cyc_List_List
_tmp21C;struct _tuple18*_tmp21D;struct _tuple18 _tmp21E;struct Cyc_Tcpat_Con_s*
_tmp21F;struct Cyc_List_List*_tmp220;struct Cyc_List_List*_tmp221;struct Cyc_List_List*
_tmp222;struct Cyc_List_List _tmp223;struct _tuple17*_tmp224;struct _tuple17 _tmp225;
struct Cyc_List_List*_tmp226;struct Cyc_List_List*_tmp227;_LL124: _tmp217=_tmp216.f1;
if(_tmp217 != 0)goto _LL126;_tmp218=_tmp216.f2;if(_tmp218 != 0)goto _LL126;_LL125:
return dsc;_LL126: _tmp219=_tmp216.f1;if(_tmp219 != 0)goto _LL128;_LL127: goto _LL129;
_LL128: _tmp21A=_tmp216.f2;if(_tmp21A != 0)goto _LL12A;_LL129:({void*_tmp228=0;((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const
char*_tmp229="build_desc: ctxt and work don't match";_tag_dyneither(_tmp229,
sizeof(char),38);}),_tag_dyneither(_tmp228,sizeof(void*),0));});_LL12A: _tmp21B=
_tmp216.f1;if(_tmp21B == 0)goto _LL123;_tmp21C=*_tmp21B;_tmp21D=(struct _tuple18*)
_tmp21C.hd;_tmp21E=*_tmp21D;_tmp21F=_tmp21E.f1;_tmp220=_tmp21E.f2;_tmp221=
_tmp21C.tl;_tmp222=_tmp216.f2;if(_tmp222 == 0)goto _LL123;_tmp223=*_tmp222;_tmp224=(
struct _tuple17*)_tmp223.hd;_tmp225=*_tmp224;_tmp226=_tmp225.f3;_tmp227=_tmp223.tl;
_LL12B: {struct Cyc_Tcpat_Pos_struct*_tmp22A=({struct Cyc_Tcpat_Pos_struct*_tmp22B=
_region_malloc(r,sizeof(*_tmp22B));_tmp22B[0]=({struct Cyc_Tcpat_Pos_struct
_tmp22C;_tmp22C.tag=0;_tmp22C.f1=_tmp21F;_tmp22C.f2=Cyc_List_rappend(r,Cyc_List_rrev(
r,_tmp220),({struct Cyc_List_List*_tmp22D=_region_malloc(r,sizeof(*_tmp22D));
_tmp22D->hd=(void*)dsc;_tmp22D->tl=_tmp226;_tmp22D;}));_tmp22C;});_tmp22B;});
return Cyc_Tcpat_build_desc(r,_tmp221,(void*)_tmp22A,_tmp227);}_LL123:;}static
void*Cyc_Tcpat_match(struct _RegionHandle*r,void*p,struct Cyc_List_List*obj,void*
dsc,struct Cyc_List_List*ctx,struct Cyc_List_List*work,void*right_hand_side,struct
Cyc_List_List*rules);static void*Cyc_Tcpat_or_match(struct _RegionHandle*r,void*
dsc,struct Cyc_List_List*allmrules){struct Cyc_List_List*_tmp22E=allmrules;struct
Cyc_List_List _tmp22F;struct _tuple0*_tmp230;struct _tuple0 _tmp231;void*_tmp232;
void*_tmp233;struct Cyc_List_List*_tmp234;_LL12D: if(_tmp22E != 0)goto _LL12F;_LL12E:
return(void*)({struct Cyc_Tcpat_Failure_struct*_tmp235=_cycalloc(sizeof(*_tmp235));
_tmp235[0]=({struct Cyc_Tcpat_Failure_struct _tmp236;_tmp236.tag=0;_tmp236.f1=(
void*)dsc;_tmp236;});_tmp235;});_LL12F: if(_tmp22E == 0)goto _LL12C;_tmp22F=*
_tmp22E;_tmp230=(struct _tuple0*)_tmp22F.hd;_tmp231=*_tmp230;_tmp232=_tmp231.f1;
_tmp233=_tmp231.f2;_tmp234=_tmp22F.tl;_LL130: return Cyc_Tcpat_match(r,_tmp232,0,
dsc,0,0,_tmp233,_tmp234);_LL12C:;}static void*Cyc_Tcpat_match_compile(struct
_RegionHandle*r,struct Cyc_List_List*allmrules){return Cyc_Tcpat_or_match(r,(void*)({
struct Cyc_Tcpat_Neg_struct*_tmp237=_region_malloc(r,sizeof(*_tmp237));_tmp237[0]=({
struct Cyc_Tcpat_Neg_struct _tmp238;_tmp238.tag=1;_tmp238.f1=Cyc_Tcpat_empty_con_set(
r);_tmp238;});_tmp237;}),allmrules);}static void*Cyc_Tcpat_and_match(struct
_RegionHandle*r,struct Cyc_List_List*ctx,struct Cyc_List_List*work,void*
right_hand_side,struct Cyc_List_List*rules){struct Cyc_List_List*_tmp239=work;
struct Cyc_List_List _tmp23A;struct _tuple17*_tmp23B;struct _tuple17 _tmp23C;struct
Cyc_List_List*_tmp23D;struct Cyc_List_List*_tmp23E;struct Cyc_List_List*_tmp23F;
struct Cyc_List_List*_tmp240;struct Cyc_List_List _tmp241;struct _tuple17*_tmp242;
struct _tuple17 _tmp243;struct Cyc_List_List*_tmp244;struct Cyc_List_List*_tmp245;
struct Cyc_List_List*_tmp246;struct Cyc_List_List*_tmp247;_LL132: if(_tmp239 != 0)
goto _LL134;_LL133: return(void*)({struct Cyc_Tcpat_Success_struct*_tmp248=
_region_malloc(r,sizeof(*_tmp248));_tmp248[0]=({struct Cyc_Tcpat_Success_struct
_tmp249;_tmp249.tag=1;_tmp249.f1=(void*)right_hand_side;_tmp249;});_tmp248;});
_LL134: if(_tmp239 == 0)goto _LL136;_tmp23A=*_tmp239;_tmp23B=(struct _tuple17*)
_tmp23A.hd;_tmp23C=*_tmp23B;_tmp23D=_tmp23C.f1;if(_tmp23D != 0)goto _LL136;_tmp23E=
_tmp23C.f2;if(_tmp23E != 0)goto _LL136;_tmp23F=_tmp23C.f3;if(_tmp23F != 0)goto
_LL136;_tmp240=_tmp23A.tl;_LL135: return Cyc_Tcpat_and_match(r,Cyc_Tcpat_norm_context(
r,ctx),_tmp240,right_hand_side,rules);_LL136: if(_tmp239 == 0)goto _LL131;_tmp241=*
_tmp239;_tmp242=(struct _tuple17*)_tmp241.hd;_tmp243=*_tmp242;_tmp244=_tmp243.f1;
_tmp245=_tmp243.f2;_tmp246=_tmp243.f3;_tmp247=_tmp241.tl;_LL137: if((_tmp244 == 0
 || _tmp245 == 0) || _tmp246 == 0)({void*_tmp24A=0;Cyc_Tcutil_impos(({const char*
_tmp24B="tcpat:and_match: malformed work frame";_tag_dyneither(_tmp24B,sizeof(
char),38);}),_tag_dyneither(_tmp24A,sizeof(void*),0));});{struct Cyc_List_List
_tmp24D;void*_tmp24E;struct Cyc_List_List*_tmp24F;struct Cyc_List_List*_tmp24C=(
struct Cyc_List_List*)_tmp244;_tmp24D=*_tmp24C;_tmp24E=(void*)_tmp24D.hd;_tmp24F=
_tmp24D.tl;{struct Cyc_List_List _tmp251;struct Cyc_List_List*_tmp252;struct Cyc_List_List*
_tmp253;struct Cyc_List_List*_tmp250=(struct Cyc_List_List*)_tmp245;_tmp251=*
_tmp250;_tmp252=(struct Cyc_List_List*)_tmp251.hd;_tmp253=_tmp251.tl;{struct Cyc_List_List
_tmp255;void*_tmp256;struct Cyc_List_List*_tmp257;struct Cyc_List_List*_tmp254=(
struct Cyc_List_List*)_tmp246;_tmp255=*_tmp254;_tmp256=(void*)_tmp255.hd;_tmp257=
_tmp255.tl;{struct _tuple17*_tmp258=({struct _tuple17*_tmp25A=_region_malloc(r,
sizeof(*_tmp25A));_tmp25A->f1=_tmp24F;_tmp25A->f2=_tmp253;_tmp25A->f3=_tmp257;
_tmp25A;});return Cyc_Tcpat_match(r,_tmp24E,_tmp252,_tmp256,ctx,({struct Cyc_List_List*
_tmp259=_region_malloc(r,sizeof(*_tmp259));_tmp259->hd=_tmp258;_tmp259->tl=
_tmp247;_tmp259;}),right_hand_side,rules);}}}}_LL131:;}static struct Cyc_List_List*
Cyc_Tcpat_getdargs(struct _RegionHandle*r,struct Cyc_Tcpat_Con_s*pcon,void*dsc){
void*_tmp25B=dsc;struct Cyc_Set_Set*_tmp25C;struct Cyc_List_List*_tmp25D;_LL139:
if(*((int*)_tmp25B)!= 1)goto _LL13B;_tmp25C=((struct Cyc_Tcpat_Neg_struct*)_tmp25B)->f1;
_LL13A: {void*any=(void*)({struct Cyc_Tcpat_Neg_struct*_tmp260=_region_malloc(r,
sizeof(*_tmp260));_tmp260[0]=({struct Cyc_Tcpat_Neg_struct _tmp261;_tmp261.tag=1;
_tmp261.f1=Cyc_Tcpat_empty_con_set(r);_tmp261;});_tmp260;});struct Cyc_List_List*
_tmp25E=0;{int i=0;for(0;i < pcon->arity;++ i){_tmp25E=({struct Cyc_List_List*
_tmp25F=_region_malloc(r,sizeof(*_tmp25F));_tmp25F->hd=(void*)any;_tmp25F->tl=
_tmp25E;_tmp25F;});}}return _tmp25E;}_LL13B: if(*((int*)_tmp25B)!= 0)goto _LL138;
_tmp25D=((struct Cyc_Tcpat_Pos_struct*)_tmp25B)->f2;_LL13C: return _tmp25D;_LL138:;}
struct _tuple19{struct _RegionHandle*f1;struct Cyc_List_List*f2;};static struct Cyc_List_List*
Cyc_Tcpat_getoarg(struct _tuple19*env,int i){struct _tuple19 _tmp263;struct
_RegionHandle*_tmp264;struct Cyc_List_List*_tmp265;struct _tuple19*_tmp262=env;
_tmp263=*_tmp262;_tmp264=_tmp263.f1;_tmp265=_tmp263.f2;return({struct Cyc_List_List*
_tmp266=_region_malloc(_tmp264,sizeof(*_tmp266));_tmp266->hd=(void*)(i + 1);
_tmp266->tl=_tmp265;_tmp266;});}static struct Cyc_List_List*Cyc_Tcpat_getoargs(
struct _RegionHandle*r,struct Cyc_Tcpat_Con_s*pcon,struct Cyc_List_List*obj){struct
_tuple19 _tmp267=({struct _tuple19 _tmp268;_tmp268.f1=r;_tmp268.f2=obj;_tmp268;});
return((struct Cyc_List_List*(*)(struct _RegionHandle*r,int n,struct Cyc_List_List*(*
f)(struct _tuple19*,int),struct _tuple19*env))Cyc_List_rtabulate_c)(r,pcon->arity,
Cyc_Tcpat_getoarg,& _tmp267);}static void*Cyc_Tcpat_match(struct _RegionHandle*r,
void*p,struct Cyc_List_List*obj,void*dsc,struct Cyc_List_List*ctx,struct Cyc_List_List*
work,void*right_hand_side,struct Cyc_List_List*rules){void*_tmp269=p;struct Cyc_Tcpat_Con_s*
_tmp26A;struct Cyc_List_List*_tmp26B;_LL13E: if((int)_tmp269 != 0)goto _LL140;_LL13F:
return Cyc_Tcpat_and_match(r,Cyc_Tcpat_augment(r,ctx,dsc),work,right_hand_side,
rules);_LL140: if(_tmp269 <= (void*)1)goto _LL13D;if(*((int*)_tmp269)!= 0)goto
_LL13D;_tmp26A=((struct Cyc_Tcpat_Con_struct*)_tmp269)->f1;_tmp26B=((struct Cyc_Tcpat_Con_struct*)
_tmp269)->f2;_LL141: {void*_tmp26C=Cyc_Tcpat_static_match(_tmp26A,dsc);_LL143:
if((int)_tmp26C != 0)goto _LL145;_LL144: {struct Cyc_List_List*ctx2=({struct Cyc_List_List*
_tmp26F=_region_malloc(r,sizeof(*_tmp26F));_tmp26F->hd=({struct _tuple18*_tmp270=
_region_malloc(r,sizeof(*_tmp270));_tmp270->f1=_tmp26A;_tmp270->f2=0;_tmp270;});
_tmp26F->tl=ctx;_tmp26F;});struct _tuple17*work_frame=({struct _tuple17*_tmp26E=
_region_malloc(r,sizeof(*_tmp26E));_tmp26E->f1=_tmp26B;_tmp26E->f2=Cyc_Tcpat_getoargs(
r,_tmp26A,obj);_tmp26E->f3=Cyc_Tcpat_getdargs(r,_tmp26A,dsc);_tmp26E;});struct
Cyc_List_List*work2=({struct Cyc_List_List*_tmp26D=_region_malloc(r,sizeof(*
_tmp26D));_tmp26D->hd=work_frame;_tmp26D->tl=work;_tmp26D;});return Cyc_Tcpat_and_match(
r,ctx2,work2,right_hand_side,rules);}_LL145: if((int)_tmp26C != 1)goto _LL147;
_LL146: return Cyc_Tcpat_or_match(r,Cyc_Tcpat_build_desc(r,ctx,dsc,work),rules);
_LL147: if((int)_tmp26C != 2)goto _LL142;_LL148: {struct Cyc_List_List*ctx2=({struct
Cyc_List_List*_tmp277=_region_malloc(r,sizeof(*_tmp277));_tmp277->hd=({struct
_tuple18*_tmp278=_region_malloc(r,sizeof(*_tmp278));_tmp278->f1=_tmp26A;_tmp278->f2=
0;_tmp278;});_tmp277->tl=ctx;_tmp277;});struct _tuple17*work_frame=({struct
_tuple17*_tmp276=_region_malloc(r,sizeof(*_tmp276));_tmp276->f1=_tmp26B;_tmp276->f2=
Cyc_Tcpat_getoargs(r,_tmp26A,obj);_tmp276->f3=Cyc_Tcpat_getdargs(r,_tmp26A,dsc);
_tmp276;});struct Cyc_List_List*work2=({struct Cyc_List_List*_tmp275=
_region_malloc(r,sizeof(*_tmp275));_tmp275->hd=work_frame;_tmp275->tl=work;
_tmp275;});void*_tmp271=Cyc_Tcpat_and_match(r,ctx2,work2,right_hand_side,rules);
void*_tmp272=Cyc_Tcpat_or_match(r,Cyc_Tcpat_build_desc(r,ctx,Cyc_Tcpat_add_neg(r,
dsc,_tmp26A),work),rules);return(void*)({struct Cyc_Tcpat_IfEq_struct*_tmp273=
_region_malloc(r,sizeof(*_tmp273));_tmp273[0]=({struct Cyc_Tcpat_IfEq_struct
_tmp274;_tmp274.tag=2;_tmp274.f1=obj;_tmp274.f2=_tmp26A;_tmp274.f3=(void*)
_tmp271;_tmp274.f4=(void*)_tmp272;_tmp274;});_tmp273;});}_LL142:;}_LL13D:;}
static void Cyc_Tcpat_check_exhaust_overlap(struct _RegionHandle*r,void*d,void(*
not_exhaust)(struct _RegionHandle*,void*,void*),void*env1,void(*rhs_appears)(void*,
void*),void*env2,int*exhaust_done){void*_tmp279=d;void*_tmp27A;void*_tmp27B;void*
_tmp27C;void*_tmp27D;_LL14A: if(*((int*)_tmp279)!= 0)goto _LL14C;_tmp27A=(void*)((
struct Cyc_Tcpat_Failure_struct*)_tmp279)->f1;_LL14B: if(!(*exhaust_done)){
not_exhaust(r,env1,_tmp27A);*exhaust_done=1;}goto _LL149;_LL14C: if(*((int*)
_tmp279)!= 1)goto _LL14E;_tmp27B=(void*)((struct Cyc_Tcpat_Success_struct*)_tmp279)->f1;
_LL14D: rhs_appears(env2,_tmp27B);goto _LL149;_LL14E: if(*((int*)_tmp279)!= 2)goto
_LL149;_tmp27C=(void*)((struct Cyc_Tcpat_IfEq_struct*)_tmp279)->f3;_tmp27D=(void*)((
struct Cyc_Tcpat_IfEq_struct*)_tmp279)->f4;_LL14F: Cyc_Tcpat_check_exhaust_overlap(
r,_tmp27C,not_exhaust,env1,rhs_appears,env2,exhaust_done);Cyc_Tcpat_check_exhaust_overlap(
r,_tmp27D,not_exhaust,env1,rhs_appears,env2,exhaust_done);goto _LL149;_LL149:;}
struct _tuple20{int f1;struct Cyc_Position_Segment*f2;};struct _tuple21{void*f1;
struct _tuple20*f2;};struct _tuple22{struct _RegionHandle*f1;int*f2;};static struct
_tuple21*Cyc_Tcpat_get_match(struct _tuple22*env,struct Cyc_Absyn_Switch_clause*
swc){struct _tuple22 _tmp27F;struct _RegionHandle*_tmp280;int*_tmp281;struct
_tuple22*_tmp27E=env;_tmp27F=*_tmp27E;_tmp280=_tmp27F.f1;_tmp281=_tmp27F.f2;{
void*sp0=Cyc_Tcpat_compile_pat(_tmp280,swc->pattern);struct _tuple20*rhs=({struct
_tuple20*_tmp287=_region_malloc(_tmp280,sizeof(*_tmp287));_tmp287->f1=0;_tmp287->f2=(
swc->pattern)->loc;_tmp287;});void*sp;if(swc->where_clause != 0){sp=Cyc_Tcpat_tuple_pat(
_tmp280,({struct Cyc_List_List*_tmp282=_region_malloc(_tmp280,sizeof(*_tmp282));
_tmp282->hd=(void*)sp0;_tmp282->tl=({struct Cyc_List_List*_tmp283=_region_malloc(
_tmp280,sizeof(*_tmp283));_tmp283->hd=(void*)Cyc_Tcpat_int_pat(_tmp280,*_tmp281,
0);_tmp283->tl=0;_tmp283;});_tmp282;}),0);*_tmp281=*_tmp281 + 1;}else{sp=Cyc_Tcpat_tuple_pat(
_tmp280,({struct Cyc_List_List*_tmp284=_region_malloc(_tmp280,sizeof(*_tmp284));
_tmp284->hd=(void*)sp0;_tmp284->tl=({struct Cyc_List_List*_tmp285=_region_malloc(
_tmp280,sizeof(*_tmp285));_tmp285->hd=(void*)((void*)0);_tmp285->tl=0;_tmp285;});
_tmp284;}),0);}return({struct _tuple21*_tmp286=_region_malloc(_tmp280,sizeof(*
_tmp286));_tmp286->f1=sp;_tmp286->f2=rhs;_tmp286;});}}char Cyc_Tcpat_Desc2string[
16]="\000\000\000\000Desc2string\000";static struct _dyneither_ptr Cyc_Tcpat_descs2string(
struct _RegionHandle*r,struct Cyc_List_List*);static struct _dyneither_ptr Cyc_Tcpat_desc2string(
struct _RegionHandle*r,void*d){void*_tmp288=d;struct Cyc_Tcpat_Con_s*_tmp289;
struct Cyc_List_List*_tmp28A;struct Cyc_Set_Set*_tmp28B;_LL151: if(*((int*)_tmp288)
!= 0)goto _LL153;_tmp289=((struct Cyc_Tcpat_Pos_struct*)_tmp288)->f1;_tmp28A=((
struct Cyc_Tcpat_Pos_struct*)_tmp288)->f2;_LL152: {union Cyc_Tcpat_Name_value
_tmp28C=_tmp289->name;struct Cyc_Absyn_Pat*_tmp28D=_tmp289->orig_pat;if(_tmp28D == 
0)return Cyc_Tcpat_desc2string(r,(void*)((struct Cyc_List_List*)_check_null(
_tmp28A))->hd);{void*_tmp28E=_tmp28D->r;struct Cyc_Absyn_Vardecl*_tmp28F;struct
Cyc_Absyn_Vardecl*_tmp290;struct Cyc_Absyn_Tvar*_tmp291;struct Cyc_Absyn_Vardecl*
_tmp292;struct Cyc_Absyn_Pat*_tmp293;struct Cyc_Absyn_Pat _tmp294;void*_tmp295;
struct Cyc_Absyn_Datatypefield*_tmp296;struct Cyc_Absyn_AggrInfo*_tmp297;struct Cyc_Absyn_AggrInfo
_tmp298;union Cyc_Absyn_AggrInfoU _tmp299;struct Cyc_Absyn_Aggrdecl**_tmp29A;struct
Cyc_Absyn_Aggrdecl*_tmp29B;struct Cyc_List_List*_tmp29C;struct Cyc_Absyn_Datatypefield*
_tmp29D;int _tmp29E;char _tmp29F;struct _dyneither_ptr _tmp2A0;struct Cyc_Absyn_Enumfield*
_tmp2A1;struct Cyc_Absyn_Enumfield*_tmp2A2;struct Cyc_Absyn_Exp*_tmp2A3;_LL156: if((
int)_tmp28E != 0)goto _LL158;_LL157: return({const char*_tmp2A4="_";_tag_dyneither(
_tmp2A4,sizeof(char),2);});_LL158: if(_tmp28E <= (void*)2)goto _LL168;if(*((int*)
_tmp28E)!= 0)goto _LL15A;_tmp28F=((struct Cyc_Absyn_Var_p_struct*)_tmp28E)->f1;
_LL159: return Cyc_Absynpp_qvar2string(_tmp28F->name);_LL15A: if(*((int*)_tmp28E)!= 
1)goto _LL15C;_tmp290=((struct Cyc_Absyn_Reference_p_struct*)_tmp28E)->f1;_LL15B:
return(struct _dyneither_ptr)({struct Cyc_String_pa_struct _tmp2A7;_tmp2A7.tag=0;
_tmp2A7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp290->name));{void*_tmp2A5[1]={& _tmp2A7};Cyc_aprintf(({const char*_tmp2A6="*%s";
_tag_dyneither(_tmp2A6,sizeof(char),4);}),_tag_dyneither(_tmp2A5,sizeof(void*),1));}});
_LL15C: if(*((int*)_tmp28E)!= 2)goto _LL15E;_tmp291=((struct Cyc_Absyn_TagInt_p_struct*)
_tmp28E)->f1;_tmp292=((struct Cyc_Absyn_TagInt_p_struct*)_tmp28E)->f2;_LL15D:
return(struct _dyneither_ptr)({struct Cyc_String_pa_struct _tmp2AB;_tmp2AB.tag=0;
_tmp2AB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp291->name);{struct
Cyc_String_pa_struct _tmp2AA;_tmp2AA.tag=0;_tmp2AA.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp292->name));{void*_tmp2A8[2]={&
_tmp2AA,& _tmp2AB};Cyc_aprintf(({const char*_tmp2A9="%s<`%s>";_tag_dyneither(
_tmp2A9,sizeof(char),8);}),_tag_dyneither(_tmp2A8,sizeof(void*),2));}}});_LL15E:
if(*((int*)_tmp28E)!= 3)goto _LL160;_LL15F: return(struct _dyneither_ptr)({struct
Cyc_String_pa_struct _tmp2AE;_tmp2AE.tag=0;_tmp2AE.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Tcpat_descs2string(r,_tmp28A));{void*_tmp2AC[1]={&
_tmp2AE};Cyc_aprintf(({const char*_tmp2AD="$(%s)";_tag_dyneither(_tmp2AD,sizeof(
char),6);}),_tag_dyneither(_tmp2AC,sizeof(void*),1));}});_LL160: if(*((int*)
_tmp28E)!= 4)goto _LL162;_tmp293=((struct Cyc_Absyn_Pointer_p_struct*)_tmp28E)->f1;
_tmp294=*_tmp293;_tmp295=_tmp294.r;if(_tmp295 <= (void*)2)goto _LL162;if(*((int*)
_tmp295)!= 6)goto _LL162;_tmp296=((struct Cyc_Absyn_Datatype_p_struct*)_tmp295)->f2;
_LL161: return(struct _dyneither_ptr)({struct Cyc_String_pa_struct _tmp2B2;_tmp2B2.tag=
0;_tmp2B2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcpat_descs2string(
r,_tmp28A));{struct Cyc_String_pa_struct _tmp2B1;_tmp2B1.tag=0;_tmp2B1.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp296->name));{
void*_tmp2AF[2]={& _tmp2B1,& _tmp2B2};Cyc_aprintf(({const char*_tmp2B0="&%s(%s)";
_tag_dyneither(_tmp2B0,sizeof(char),8);}),_tag_dyneither(_tmp2AF,sizeof(void*),2));}}});
_LL162: if(*((int*)_tmp28E)!= 4)goto _LL164;_LL163: return(struct _dyneither_ptr)({
struct Cyc_String_pa_struct _tmp2B5;_tmp2B5.tag=0;_tmp2B5.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Tcpat_descs2string(r,_tmp28A));{void*_tmp2B3[1]={&
_tmp2B5};Cyc_aprintf(({const char*_tmp2B4="&%s";_tag_dyneither(_tmp2B4,sizeof(
char),4);}),_tag_dyneither(_tmp2B3,sizeof(void*),1));}});_LL164: if(*((int*)
_tmp28E)!= 5)goto _LL166;_tmp297=((struct Cyc_Absyn_Aggr_p_struct*)_tmp28E)->f1;
if(_tmp297 == 0)goto _LL166;_tmp298=*_tmp297;_tmp299=_tmp298.aggr_info;if((_tmp299.KnownAggr).tag
!= 2)goto _LL166;_tmp29A=(struct Cyc_Absyn_Aggrdecl**)(_tmp299.KnownAggr).val;
_tmp29B=*_tmp29A;_tmp29C=((struct Cyc_Absyn_Aggr_p_struct*)_tmp28E)->f3;_LL165:
if(_tmp29B->kind == (void*)1){struct Cyc_List_List*_tmp2B6=_tmp29C;struct Cyc_List_List
_tmp2B7;struct _tuple14*_tmp2B8;struct _tuple14 _tmp2B9;struct Cyc_List_List*_tmp2BA;
struct Cyc_List_List _tmp2BB;void*_tmp2BC;struct _dyneither_ptr*_tmp2BD;_LL179: if(
_tmp2B6 == 0)goto _LL17B;_tmp2B7=*_tmp2B6;_tmp2B8=(struct _tuple14*)_tmp2B7.hd;
_tmp2B9=*_tmp2B8;_tmp2BA=_tmp2B9.f1;if(_tmp2BA == 0)goto _LL17B;_tmp2BB=*_tmp2BA;
_tmp2BC=(void*)_tmp2BB.hd;if(*((int*)_tmp2BC)!= 1)goto _LL17B;_tmp2BD=((struct Cyc_Absyn_FieldName_struct*)
_tmp2BC)->f1;_LL17A: return(struct _dyneither_ptr)({struct Cyc_String_pa_struct
_tmp2C2;_tmp2C2.tag=0;_tmp2C2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Tcpat_descs2string(r,_tmp28A));{struct Cyc_String_pa_struct _tmp2C1;_tmp2C1.tag=
0;_tmp2C1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp2BD);{struct Cyc_String_pa_struct
_tmp2C0;_tmp2C0.tag=0;_tmp2C0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp29B->name));{void*_tmp2BE[3]={& _tmp2C0,& _tmp2C1,&
_tmp2C2};Cyc_aprintf(({const char*_tmp2BF="%s{.%s = %s}";_tag_dyneither(_tmp2BF,
sizeof(char),13);}),_tag_dyneither(_tmp2BE,sizeof(void*),3));}}}});_LL17B:;
_LL17C: goto _LL178;_LL178:;}return(struct _dyneither_ptr)({struct Cyc_String_pa_struct
_tmp2C6;_tmp2C6.tag=0;_tmp2C6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Tcpat_descs2string(r,_tmp28A));{struct Cyc_String_pa_struct _tmp2C5;_tmp2C5.tag=
0;_tmp2C5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp29B->name));{void*_tmp2C3[2]={& _tmp2C5,& _tmp2C6};Cyc_aprintf(({const char*
_tmp2C4="%s{%s}";_tag_dyneither(_tmp2C4,sizeof(char),7);}),_tag_dyneither(
_tmp2C3,sizeof(void*),2));}}});_LL166: if(*((int*)_tmp28E)!= 6)goto _LL168;_tmp29D=((
struct Cyc_Absyn_Datatype_p_struct*)_tmp28E)->f2;_LL167: return(struct
_dyneither_ptr)({struct Cyc_String_pa_struct _tmp2C9;_tmp2C9.tag=0;_tmp2C9.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp29D->name));{
void*_tmp2C7[1]={& _tmp2C9};Cyc_aprintf(({const char*_tmp2C8="%s";_tag_dyneither(
_tmp2C8,sizeof(char),3);}),_tag_dyneither(_tmp2C7,sizeof(void*),1));}});_LL168:
if((int)_tmp28E != 1)goto _LL16A;_LL169: return({const char*_tmp2CA="NULL";
_tag_dyneither(_tmp2CA,sizeof(char),5);});_LL16A: if(_tmp28E <= (void*)2)goto
_LL176;if(*((int*)_tmp28E)!= 7)goto _LL16C;_tmp29E=((struct Cyc_Absyn_Int_p_struct*)
_tmp28E)->f2;_LL16B: return(struct _dyneither_ptr)({struct Cyc_Int_pa_struct _tmp2CD;
_tmp2CD.tag=1;_tmp2CD.f1=(unsigned long)_tmp29E;{void*_tmp2CB[1]={& _tmp2CD};Cyc_aprintf(({
const char*_tmp2CC="%d";_tag_dyneither(_tmp2CC,sizeof(char),3);}),_tag_dyneither(
_tmp2CB,sizeof(void*),1));}});_LL16C: if(*((int*)_tmp28E)!= 8)goto _LL16E;_tmp29F=((
struct Cyc_Absyn_Char_p_struct*)_tmp28E)->f1;_LL16D: return(struct _dyneither_ptr)({
struct Cyc_Int_pa_struct _tmp2D0;_tmp2D0.tag=1;_tmp2D0.f1=(unsigned long)((int)
_tmp29F);{void*_tmp2CE[1]={& _tmp2D0};Cyc_aprintf(({const char*_tmp2CF="%d";
_tag_dyneither(_tmp2CF,sizeof(char),3);}),_tag_dyneither(_tmp2CE,sizeof(void*),1));}});
_LL16E: if(*((int*)_tmp28E)!= 9)goto _LL170;_tmp2A0=((struct Cyc_Absyn_Float_p_struct*)
_tmp28E)->f1;_LL16F: return _tmp2A0;_LL170: if(*((int*)_tmp28E)!= 10)goto _LL172;
_tmp2A1=((struct Cyc_Absyn_Enum_p_struct*)_tmp28E)->f2;_LL171: _tmp2A2=_tmp2A1;
goto _LL173;_LL172: if(*((int*)_tmp28E)!= 11)goto _LL174;_tmp2A2=((struct Cyc_Absyn_AnonEnum_p_struct*)
_tmp28E)->f2;_LL173: return Cyc_Absynpp_qvar2string(_tmp2A2->name);_LL174: if(*((
int*)_tmp28E)!= 14)goto _LL176;_tmp2A3=((struct Cyc_Absyn_Exp_p_struct*)_tmp28E)->f1;
_LL175: return Cyc_Absynpp_exp2string(_tmp2A3);_LL176:;_LL177:(int)_throw((void*)
Cyc_Tcpat_Desc2string);_LL155:;}}_LL153: if(*((int*)_tmp288)!= 1)goto _LL150;
_tmp28B=((struct Cyc_Tcpat_Neg_struct*)_tmp288)->f1;_LL154: if(((int(*)(struct Cyc_Set_Set*
s))Cyc_Set_is_empty)(_tmp28B))return({const char*_tmp2D1="_";_tag_dyneither(
_tmp2D1,sizeof(char),2);});{struct Cyc_Tcpat_Con_s*_tmp2D2=((struct Cyc_Tcpat_Con_s*(*)(
struct Cyc_Set_Set*s))Cyc_Set_choose)(_tmp28B);struct Cyc_Absyn_Pat*_tmp2D3=
_tmp2D2->orig_pat;if(_tmp2D3 == 0)(int)_throw((void*)Cyc_Tcpat_Desc2string);{void*
_tmp2D4=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp2D3->topt))->v);
void*_tmp2D5;struct Cyc_Absyn_PtrInfo _tmp2D6;struct Cyc_Absyn_PtrAtts _tmp2D7;
struct Cyc_Absyn_DatatypeInfo _tmp2D8;union Cyc_Absyn_DatatypeInfoU _tmp2D9;struct
Cyc_Absyn_Datatypedecl**_tmp2DA;struct Cyc_Absyn_Datatypedecl*_tmp2DB;struct Cyc_Absyn_AggrInfo
_tmp2DC;union Cyc_Absyn_AggrInfoU _tmp2DD;struct Cyc_Absyn_Aggrdecl**_tmp2DE;struct
Cyc_Absyn_Aggrdecl*_tmp2DF;_LL17E: if(_tmp2D4 <= (void*)4)goto _LL188;if(*((int*)
_tmp2D4)!= 5)goto _LL180;_tmp2D5=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp2D4)->f2;
if((int)_tmp2D5 != 0)goto _LL180;_LL17F:{int i=0;for(0;i < 256;++ i){struct Cyc_Tcpat_Con_s*
_tmp2E0=Cyc_Tcpat_char_con(r,(char)i,(struct Cyc_Absyn_Pat*)_tmp2D3);if(!((int(*)(
struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp28B,_tmp2E0))
return(struct _dyneither_ptr)({struct Cyc_Int_pa_struct _tmp2E3;_tmp2E3.tag=1;
_tmp2E3.f1=(unsigned long)i;{void*_tmp2E1[1]={& _tmp2E3};Cyc_aprintf(({const char*
_tmp2E2="%d";_tag_dyneither(_tmp2E2,sizeof(char),3);}),_tag_dyneither(_tmp2E1,
sizeof(void*),1));}});}}(int)_throw((void*)Cyc_Tcpat_Desc2string);_LL180: if(*((
int*)_tmp2D4)!= 5)goto _LL182;_LL181:{unsigned int i=0;for(0;i < 0 - 1;++ i){struct
Cyc_Tcpat_Con_s*_tmp2E4=Cyc_Tcpat_int_con(r,(int)i,_tmp2D3);if(!((int(*)(struct
Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp28B,_tmp2E4))return(
struct _dyneither_ptr)({struct Cyc_Int_pa_struct _tmp2E7;_tmp2E7.tag=1;_tmp2E7.f1=(
unsigned long)((int)i);{void*_tmp2E5[1]={& _tmp2E7};Cyc_aprintf(({const char*
_tmp2E6="%d";_tag_dyneither(_tmp2E6,sizeof(char),3);}),_tag_dyneither(_tmp2E5,
sizeof(void*),1));}});}}(int)_throw((void*)Cyc_Tcpat_Desc2string);_LL182: if(*((
int*)_tmp2D4)!= 4)goto _LL184;_tmp2D6=((struct Cyc_Absyn_PointerType_struct*)
_tmp2D4)->f1;_tmp2D7=_tmp2D6.ptr_atts;_LL183: {union Cyc_Absyn_Constraint*_tmp2E8=
_tmp2D7.nullable;int is_nullable=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
0,_tmp2E8);if(is_nullable){if(!((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*
elt))Cyc_Set_member)(_tmp28B,Cyc_Tcpat_null_con(r,(struct Cyc_Absyn_Pat*)_tmp2D3)))
return({const char*_tmp2E9="NULL";_tag_dyneither(_tmp2E9,sizeof(char),5);});}
return({const char*_tmp2EA="&_";_tag_dyneither(_tmp2EA,sizeof(char),3);});}_LL184:
if(*((int*)_tmp2D4)!= 2)goto _LL186;_tmp2D8=((struct Cyc_Absyn_DatatypeType_struct*)
_tmp2D4)->f1;_tmp2D9=_tmp2D8.datatype_info;if((_tmp2D9.KnownDatatype).tag != 2)
goto _LL186;_tmp2DA=(struct Cyc_Absyn_Datatypedecl**)(_tmp2D9.KnownDatatype).val;
_tmp2DB=*_tmp2DA;_LL185: if(_tmp2DB->is_extensible)(int)_throw((void*)Cyc_Tcpat_Desc2string);{
struct Cyc_List_List*_tmp2EB=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp2DB->fields))->v;int span=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp2EB);for(0;(unsigned int)_tmp2EB;_tmp2EB=_tmp2EB->tl){struct _dyneither_ptr n=*(*((
struct Cyc_Absyn_Datatypefield*)_tmp2EB->hd)->name).f2;struct Cyc_List_List*
_tmp2EC=((struct Cyc_Absyn_Datatypefield*)_tmp2EB->hd)->typs;if(!((int(*)(struct
Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp28B,(struct Cyc_Tcpat_Con_s*)({
struct Cyc_Tcpat_Con_s*_tmp2ED=_cycalloc(sizeof(*_tmp2ED));_tmp2ED->name=Cyc_Tcpat_Name_v(
n);_tmp2ED->arity=0;_tmp2ED->span=0;_tmp2ED->orig_pat=_tmp2D3;_tmp2ED;}))){if(((
int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp2EC)== 0)return n;else{return(
struct _dyneither_ptr)({struct Cyc_String_pa_struct _tmp2F0;_tmp2F0.tag=0;_tmp2F0.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)n);{void*_tmp2EE[1]={& _tmp2F0};Cyc_aprintf(({
const char*_tmp2EF="&%s(...)";_tag_dyneither(_tmp2EF,sizeof(char),9);}),
_tag_dyneither(_tmp2EE,sizeof(void*),1));}});}}}(int)_throw((void*)Cyc_Tcpat_Desc2string);}
_LL186: if(*((int*)_tmp2D4)!= 10)goto _LL188;_tmp2DC=((struct Cyc_Absyn_AggrType_struct*)
_tmp2D4)->f1;_tmp2DD=_tmp2DC.aggr_info;if((_tmp2DD.KnownAggr).tag != 2)goto _LL188;
_tmp2DE=(struct Cyc_Absyn_Aggrdecl**)(_tmp2DD.KnownAggr).val;_tmp2DF=*_tmp2DE;if(
!(_tmp2DF->kind == (void*)1))goto _LL188;_LL187: {struct Cyc_List_List*_tmp2F1=((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp2DF->impl))->fields;int span=((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmp2F1);struct _tuple1 _tmp2F3;struct
_dyneither_ptr*_tmp2F4;struct _dyneither_ptr _tmp2F5;struct _tuple1*_tmp2F2=_tmp2DF->name;
_tmp2F3=*_tmp2F2;_tmp2F4=_tmp2F3.f2;_tmp2F5=*_tmp2F4;for(0;(unsigned int)_tmp2F1;
_tmp2F1=_tmp2F1->tl){struct _dyneither_ptr n=*((struct Cyc_Absyn_Aggrfield*)_tmp2F1->hd)->name;
if(!((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(
_tmp28B,(struct Cyc_Tcpat_Con_s*)({struct Cyc_Tcpat_Con_s*_tmp2F6=_cycalloc(
sizeof(*_tmp2F6));_tmp2F6->name=Cyc_Tcpat_Name_v(n);_tmp2F6->arity=0;_tmp2F6->span=
0;_tmp2F6->orig_pat=_tmp2D3;_tmp2F6;})))return(struct _dyneither_ptr)({struct Cyc_String_pa_struct
_tmp2FA;_tmp2FA.tag=0;_tmp2FA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)n);{
struct Cyc_String_pa_struct _tmp2F9;_tmp2F9.tag=0;_tmp2F9.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)_tmp2F5);{void*_tmp2F7[2]={& _tmp2F9,& _tmp2FA};Cyc_aprintf(({
const char*_tmp2F8="%s{.%s = _}";_tag_dyneither(_tmp2F8,sizeof(char),12);}),
_tag_dyneither(_tmp2F7,sizeof(void*),2));}}});}(int)_throw((void*)Cyc_Tcpat_Desc2string);}
_LL188:;_LL189:(int)_throw((void*)Cyc_Tcpat_Desc2string);_LL17D:;}}_LL150:;}
static struct _dyneither_ptr*Cyc_Tcpat_desc2stringptr(struct _RegionHandle*r,void*d){
return({struct _dyneither_ptr*_tmp2FB=_cycalloc(sizeof(*_tmp2FB));_tmp2FB[0]=Cyc_Tcpat_desc2string(
r,d);_tmp2FB;});}static struct _dyneither_ptr Cyc_Tcpat_descs2string(struct
_RegionHandle*r,struct Cyc_List_List*ds){struct Cyc_List_List*_tmp2FC=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _dyneither_ptr*(*f)(struct _RegionHandle*,void*),
struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcpat_desc2stringptr,
r,ds);struct _dyneither_ptr*comma=_init_dyneither_ptr(_cycalloc(sizeof(struct
_dyneither_ptr)),",",sizeof(char),2);{struct Cyc_List_List*_tmp2FD=_tmp2FC;for(0;
_tmp2FD != 0;_tmp2FD=((struct Cyc_List_List*)_check_null(_tmp2FD))->tl){if(_tmp2FD->tl
!= 0){_tmp2FD->tl=(struct Cyc_List_List*)({struct Cyc_List_List*_tmp2FE=_cycalloc(
sizeof(*_tmp2FE));_tmp2FE->hd=comma;_tmp2FE->tl=_tmp2FD->tl;_tmp2FE;});_tmp2FD=
_tmp2FD->tl;}}}return(struct _dyneither_ptr)Cyc_strconcat_l(_tmp2FC);}static void
Cyc_Tcpat_not_exhaust_err(struct _RegionHandle*r,struct Cyc_Position_Segment*loc,
void*desc){struct _handler_cons _tmp300;_push_handler(& _tmp300);{int _tmp302=0;if(
setjmp(_tmp300.handler))_tmp302=1;if(!_tmp302){{struct _dyneither_ptr _tmp303=Cyc_Tcpat_desc2string(
r,desc);({struct Cyc_String_pa_struct _tmp306;_tmp306.tag=0;_tmp306.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)_tmp303);{void*_tmp304[1]={& _tmp306};Cyc_Tcutil_terr(
loc,({const char*_tmp305="patterns may not be exhaustive.\n\tmissing case for %s";
_tag_dyneither(_tmp305,sizeof(char),53);}),_tag_dyneither(_tmp304,sizeof(void*),
1));}});};_pop_handler();}else{void*_tmp301=(void*)_exn_thrown;void*_tmp308=
_tmp301;_LL18B: if(_tmp308 != Cyc_Tcpat_Desc2string)goto _LL18D;_LL18C:({void*
_tmp309=0;Cyc_Tcutil_terr(loc,({const char*_tmp30A="patterns may not be exhaustive.";
_tag_dyneither(_tmp30A,sizeof(char),32);}),_tag_dyneither(_tmp309,sizeof(void*),
0));});goto _LL18A;_LL18D:;_LL18E:(void)_throw(_tmp308);_LL18A:;}}}static void Cyc_Tcpat_rule_occurs(
int dummy,struct _tuple20*rhs){(*rhs).f1=1;}void Cyc_Tcpat_check_switch_exhaustive(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*swcs){
struct _RegionHandle*_tmp30B=Cyc_Tcenv_get_fnrgn(te);int _tmp30C=0;struct _tuple22
_tmp30D=({struct _tuple22 _tmp319;_tmp319.f1=_tmp30B;_tmp319.f2=& _tmp30C;_tmp319;});
struct Cyc_List_List*_tmp30E=((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct _tuple21*(*f)(struct _tuple22*,struct Cyc_Absyn_Switch_clause*),struct
_tuple22*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(_tmp30B,Cyc_Tcpat_get_match,&
_tmp30D,swcs);void*_tmp30F=((void*(*)(struct _RegionHandle*r,struct Cyc_List_List*
allmrules))Cyc_Tcpat_match_compile)(_tmp30B,_tmp30E);int _tmp310=0;((void(*)(
struct _RegionHandle*r,void*d,void(*not_exhaust)(struct _RegionHandle*,struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env1,void(*rhs_appears)(int,struct _tuple20*),
int env2,int*exhaust_done))Cyc_Tcpat_check_exhaust_overlap)(_tmp30B,_tmp30F,Cyc_Tcpat_not_exhaust_err,
loc,Cyc_Tcpat_rule_occurs,0,& _tmp310);for(0;_tmp30E != 0;_tmp30E=_tmp30E->tl){
struct _tuple21 _tmp312;struct _tuple20*_tmp313;struct _tuple20 _tmp314;int _tmp315;
struct Cyc_Position_Segment*_tmp316;struct _tuple21*_tmp311=(struct _tuple21*)
_tmp30E->hd;_tmp312=*_tmp311;_tmp313=_tmp312.f2;_tmp314=*_tmp313;_tmp315=_tmp314.f1;
_tmp316=_tmp314.f2;if(!_tmp315)({void*_tmp317=0;Cyc_Tcutil_terr(_tmp316,({const
char*_tmp318="redundant pattern";_tag_dyneither(_tmp318,sizeof(char),18);}),
_tag_dyneither(_tmp317,sizeof(void*),0));});}}struct _tuple23{struct Cyc_Position_Segment*
f1;int f2;};static void Cyc_Tcpat_not_exhaust_warn(struct _RegionHandle*r,struct
_tuple23*pr,void*desc){(*pr).f2=0;{struct _handler_cons _tmp31A;_push_handler(&
_tmp31A);{int _tmp31C=0;if(setjmp(_tmp31A.handler))_tmp31C=1;if(!_tmp31C){{struct
_dyneither_ptr _tmp31D=Cyc_Tcpat_desc2string(r,desc);({struct Cyc_String_pa_struct
_tmp320;_tmp320.tag=0;_tmp320.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
_tmp31D);{void*_tmp31E[1]={& _tmp320};Cyc_Tcutil_warn((*pr).f1,({const char*
_tmp31F="pattern not exhaustive.\n\tmissing case for %s";_tag_dyneither(_tmp31F,
sizeof(char),45);}),_tag_dyneither(_tmp31E,sizeof(void*),1));}});};_pop_handler();}
else{void*_tmp31B=(void*)_exn_thrown;void*_tmp322=_tmp31B;_LL190: if(_tmp322 != 
Cyc_Tcpat_Desc2string)goto _LL192;_LL191:({void*_tmp323=0;Cyc_Tcutil_warn((*pr).f1,({
const char*_tmp324="pattern not exhaustive.";_tag_dyneither(_tmp324,sizeof(char),
24);}),_tag_dyneither(_tmp323,sizeof(void*),0));});goto _LL18F;_LL192:;_LL193:(
void)_throw(_tmp322);_LL18F:;}}}}static void Cyc_Tcpat_dummy_fn(int i,int j){return;}
int Cyc_Tcpat_check_let_pat_exhaustive(struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Pat*p){struct _RegionHandle*_tmp325=Cyc_Tcenv_get_fnrgn(te);
struct Cyc_List_List*_tmp326=({struct Cyc_List_List*_tmp32B=_region_malloc(_tmp325,
sizeof(*_tmp32B));_tmp32B->hd=({struct _tuple6*_tmp32C=_region_malloc(_tmp325,
sizeof(*_tmp32C));_tmp32C->f1=Cyc_Tcpat_compile_pat(_tmp325,p);_tmp32C->f2=0;
_tmp32C;});_tmp32B->tl=0;_tmp32B;});void*_tmp327=((void*(*)(struct _RegionHandle*
r,struct Cyc_List_List*allmrules))Cyc_Tcpat_match_compile)(_tmp325,_tmp326);
struct _tuple23 _tmp328=({struct _tuple23 _tmp32A;_tmp32A.f1=loc;_tmp32A.f2=1;
_tmp32A;});int _tmp329=0;((void(*)(struct _RegionHandle*r,void*d,void(*not_exhaust)(
struct _RegionHandle*,struct _tuple23*,void*),struct _tuple23*env1,void(*
rhs_appears)(int,int),int env2,int*exhaust_done))Cyc_Tcpat_check_exhaust_overlap)(
_tmp325,_tmp327,Cyc_Tcpat_not_exhaust_warn,& _tmp328,Cyc_Tcpat_dummy_fn,0,&
_tmp329);return _tmp328.f2;}static struct _tuple21*Cyc_Tcpat_get_match2(struct
_RegionHandle*r,struct Cyc_Absyn_Switch_clause*swc){void*sp0=Cyc_Tcpat_compile_pat(
r,swc->pattern);struct _tuple20*rhs=({struct _tuple20*_tmp32E=_region_malloc(r,
sizeof(*_tmp32E));_tmp32E->f1=0;_tmp32E->f2=(swc->pattern)->loc;_tmp32E;});
return({struct _tuple21*_tmp32D=_region_malloc(r,sizeof(*_tmp32D));_tmp32D->f1=
sp0;_tmp32D->f2=rhs;_tmp32D;});}static void Cyc_Tcpat_not_exhaust_err2(struct
_RegionHandle*r,struct Cyc_Position_Segment*loc,void*d){;}void Cyc_Tcpat_check_catch_overlap(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*swcs){
struct _RegionHandle*_tmp32F=Cyc_Tcenv_get_fnrgn(te);struct Cyc_List_List*_tmp330=((
struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple21*(*f)(struct
_RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct _RegionHandle*env,struct Cyc_List_List*
x))Cyc_List_rmap_c)(_tmp32F,Cyc_Tcpat_get_match2,_tmp32F,swcs);void*_tmp331=((
void*(*)(struct _RegionHandle*r,struct Cyc_List_List*allmrules))Cyc_Tcpat_match_compile)(
_tmp32F,_tmp330);int _tmp332=0;((void(*)(struct _RegionHandle*r,void*d,void(*
not_exhaust)(struct _RegionHandle*,struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*
env1,void(*rhs_appears)(int,struct _tuple20*),int env2,int*exhaust_done))Cyc_Tcpat_check_exhaust_overlap)(
_tmp32F,_tmp331,Cyc_Tcpat_not_exhaust_err2,loc,Cyc_Tcpat_rule_occurs,0,& _tmp332);
for(0;_tmp330 != 0;_tmp330=_tmp330->tl){struct _tuple21 _tmp334;struct _tuple20*
_tmp335;struct _tuple20 _tmp336;int _tmp337;struct Cyc_Position_Segment*_tmp338;
struct _tuple21*_tmp333=(struct _tuple21*)_tmp330->hd;_tmp334=*_tmp333;_tmp335=
_tmp334.f2;_tmp336=*_tmp335;_tmp337=_tmp336.f1;_tmp338=_tmp336.f2;if(!_tmp337)({
void*_tmp339=0;Cyc_Tcutil_terr(_tmp338,({const char*_tmp33A="redundant pattern";
_tag_dyneither(_tmp33A,sizeof(char),18);}),_tag_dyneither(_tmp339,sizeof(void*),
0));});}}
