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
void*v;};struct _tuple0{void*f1;void*f2;};void*Cyc_Core_fst(struct _tuple0*);int
Cyc_Core_intcmp(int,int);extern char Cyc_Core_Invalid_argument[21];struct Cyc_Core_Invalid_argument_struct{
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
int Cyc_printf(struct _dyneither_ptr,struct _dyneither_ptr);struct _dyneither_ptr Cyc_vrprintf(
struct _RegionHandle*,struct _dyneither_ptr,struct _dyneither_ptr);extern char Cyc_FileCloseError[
19];extern char Cyc_FileOpenError[18];struct Cyc_FileOpenError_struct{char*tag;
struct _dyneither_ptr f1;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
struct Cyc_List_List*Cyc_List_list(struct _dyneither_ptr);int Cyc_List_length(
struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_copy(struct Cyc_List_List*x);
struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);struct
Cyc_List_List*Cyc_List_rmap(struct _RegionHandle*,void*(*f)(void*),struct Cyc_List_List*
x);struct Cyc_List_List*Cyc_List_rmap_c(struct _RegionHandle*,void*(*f)(void*,void*),
void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[18];struct Cyc_List_List*
Cyc_List_map2(void*(*f)(void*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y);
void Cyc_List_iter(void(*f)(void*),struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_revappend(
struct Cyc_List_List*x,struct Cyc_List_List*y);struct Cyc_List_List*Cyc_List_imp_rev(
struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rappend(struct _RegionHandle*,
struct Cyc_List_List*x,struct Cyc_List_List*y);struct Cyc_List_List*Cyc_List_imp_append(
struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[8];void*Cyc_List_nth(
struct Cyc_List_List*x,int n);int Cyc_List_exists_c(int(*pred)(void*,void*),void*
env,struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_zip(struct Cyc_List_List*
x,struct Cyc_List_List*y);struct Cyc_List_List*Cyc_List_rzip(struct _RegionHandle*
r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y);struct
_tuple1{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct _tuple1 Cyc_List_rsplit(
struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x);int Cyc_List_mem(
int(*compare)(void*,void*),struct Cyc_List_List*l,void*x);void*Cyc_List_assoc_cmp(
int(*cmp)(void*,void*),struct Cyc_List_List*l,void*x);int Cyc_List_list_cmp(int(*
cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2);struct Cyc_Lineno_Pos{
struct _dyneither_ptr logical_file;struct _dyneither_ptr line;int line_no;int col;};
extern char Cyc_Position_Exit[9];struct Cyc_Position_Segment;struct Cyc_List_List*
Cyc_Position_strings_of_segments(struct Cyc_List_List*);struct Cyc_Position_Error{
struct _dyneither_ptr source;struct Cyc_Position_Segment*seg;void*kind;struct
_dyneither_ptr desc;};struct Cyc_Position_Error*Cyc_Position_mk_err_elab(struct Cyc_Position_Segment*,
struct _dyneither_ptr);extern char Cyc_Position_Nocontext[14];extern int Cyc_Position_num_errors;
extern int Cyc_Position_max_errors;void Cyc_Position_post_error(struct Cyc_Position_Error*);
struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct
_union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{
int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct
_union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_Loc_n Loc_n;};union Cyc_Absyn_Nmspace
Cyc_Absyn_Loc_n;union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(struct Cyc_List_List*);
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*);struct _tuple2{union
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
struct _tuple2*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{
int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct
_union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};
union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype
UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};union Cyc_Absyn_DatatypeInfoU
Cyc_Absyn_KnownDatatype(struct Cyc_Absyn_Datatypedecl**);struct Cyc_Absyn_DatatypeInfo{
union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;struct Cyc_Core_Opt*
rgn;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple2*datatype_name;
struct _tuple2*field_name;int is_extensible;};struct
_union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo
val;};struct _tuple3{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*
f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple3 val;
};union Cyc_Absyn_DatatypeFieldInfoU{struct
_union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct
_union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};union Cyc_Absyn_DatatypeFieldInfoU
Cyc_Absyn_KnownDatatypefield(struct Cyc_Absyn_Datatypedecl*,struct Cyc_Absyn_Datatypefield*);
struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;
struct Cyc_List_List*targs;};struct _tuple4{void*f1;struct _tuple2*f2;struct Cyc_Core_Opt*
f3;};struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple4 val;};struct
_union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{
struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr
KnownAggr;};union Cyc_Absyn_AggrInfoU Cyc_Absyn_UnknownAggr(void*,struct _tuple2*,
struct Cyc_Core_Opt*);union Cyc_Absyn_AggrInfoU Cyc_Absyn_KnownAggr(struct Cyc_Absyn_Aggrdecl**);
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
;struct Cyc_Absyn_EnumType_struct{int tag;struct _tuple2*f1;struct Cyc_Absyn_Enumdecl*
f2;};struct Cyc_Absyn_AnonEnumType_struct{int tag;struct Cyc_List_List*f1;};struct
Cyc_Absyn_RgnHandleType_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_struct{
int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_struct{int tag;struct _tuple2*
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
_tuple5{void*f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple5 val;};
struct _tuple6{void*f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple6
val;};struct _tuple7{void*f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple7
val;};struct _tuple8{void*f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;
struct _tuple8 val;};struct _union_Cnst_Float_c{int tag;struct _dyneither_ptr val;};
struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{
struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct
_union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct
_union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct
_union_Cnst_String_c String_c;};extern union Cyc_Absyn_Cnst Cyc_Absyn_Null_c;struct
Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*
vai;};struct Cyc_Absyn_StructField_struct{int tag;struct _dyneither_ptr*f1;};struct
Cyc_Absyn_TupleIndex_struct{int tag;unsigned int f1;};struct Cyc_Absyn_MallocInfo{
int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;
int fat_result;};struct Cyc_Absyn_Const_e_struct{int tag;union Cyc_Absyn_Cnst f1;};
struct Cyc_Absyn_Var_e_struct{int tag;struct _tuple2*f1;void*f2;};struct Cyc_Absyn_UnknownId_e_struct{
int tag;struct _tuple2*f1;};struct Cyc_Absyn_Primop_e_struct{int tag;void*f1;struct
Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_struct{int tag;struct Cyc_Absyn_Exp*
f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;void*f2;};struct Cyc_Absyn_Conditional_e_struct{int
tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};
struct Cyc_Absyn_And_e_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*
f2;};struct Cyc_Absyn_Or_e_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*
f2;};struct Cyc_Absyn_SeqExp_e_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*
f2;};struct Cyc_Absyn_UnknownCall_e_struct{int tag;struct Cyc_Absyn_Exp*f1;struct
Cyc_List_List*f2;};struct Cyc_Absyn_FnCall_e_struct{int tag;struct Cyc_Absyn_Exp*f1;
struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;};struct Cyc_Absyn_Throw_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoInstantiate_e_struct{int tag;
struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_struct{int tag;struct Cyc_Absyn_Exp*
f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_struct{int tag;void*f1;struct
Cyc_Absyn_Exp*f2;int f3;void*f4;};struct Cyc_Absyn_Address_e_struct{int tag;struct
Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_struct{int tag;struct Cyc_Absyn_Exp*f1;
struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_struct{int tag;void*f1;};
struct Cyc_Absyn_Sizeofexp_e_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_struct{
int tag;void*f1;void*f2;};struct Cyc_Absyn_Gentyp_e_struct{int tag;struct Cyc_List_List*
f1;void*f2;};struct Cyc_Absyn_Deref_e_struct{int tag;struct Cyc_Absyn_Exp*f1;};
struct Cyc_Absyn_AggrMember_e_struct{int tag;struct Cyc_Absyn_Exp*f1;struct
_dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_struct{int tag;struct
Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_struct{
int tag;struct Cyc_List_List*f1;};struct _tuple9{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Tqual
f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_struct{int tag;struct _tuple9*f1;struct
Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_struct{int tag;struct Cyc_List_List*f1;
};struct Cyc_Absyn_Comprehension_e_struct{int tag;struct Cyc_Absyn_Vardecl*f1;
struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_Aggregate_e_struct{
int tag;struct _tuple2*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*
f4;};struct Cyc_Absyn_AnonStruct_e_struct{int tag;void*f1;struct Cyc_List_List*f2;}
;struct Cyc_Absyn_Datatype_e_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*
f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_struct{int tag;
struct _tuple2*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};
struct Cyc_Absyn_AnonEnum_e_struct{int tag;struct _tuple2*f1;void*f2;struct Cyc_Absyn_Enumfield*
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
_tuple10{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_struct{
int tag;struct _tuple10 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_struct{
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_struct{int tag;struct
Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_struct{int tag;struct _dyneither_ptr*f1;
struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_struct{int tag;struct Cyc_Absyn_Exp*
f1;struct _tuple10 f2;struct _tuple10 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Fallthru_s_struct{
int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_struct{
int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_struct{
int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_struct{
int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple10 f2;};struct Cyc_Absyn_TryCatch_s_struct{
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
struct _tuple2*f1;};struct Cyc_Absyn_UnknownCall_p_struct{int tag;struct _tuple2*f1;
struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_struct{int tag;struct Cyc_Absyn_Exp*
f1;};struct Cyc_Absyn_Pat{void*r;struct Cyc_Core_Opt*topt;struct Cyc_Position_Segment*
loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*
pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;struct Cyc_Position_Segment*
loc;};struct Cyc_Absyn_Global_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct
Cyc_Absyn_Funname_b_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_struct{int tag;struct
Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_Absyn_Vardecl{void*sc;struct _tuple2*name;struct Cyc_Absyn_Tqual tq;
void*type;struct Cyc_Absyn_Exp*initializer;struct Cyc_Core_Opt*rgn;struct Cyc_List_List*
attributes;int escapes;};struct Cyc_Absyn_Fndecl{void*sc;int is_inline;struct
_tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*effect;void*ret_type;
struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;
struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;struct Cyc_Core_Opt*
cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;
struct Cyc_List_List*attributes;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*
name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*
attributes;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct
Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{
void*kind;void*sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*
impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Datatypefield{struct
_tuple2*name;struct Cyc_List_List*typs;struct Cyc_Position_Segment*loc;void*sc;};
struct Cyc_Absyn_Datatypedecl{void*sc;struct _tuple2*name;struct Cyc_List_List*tvs;
struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct
_tuple2*name;struct Cyc_Absyn_Exp*tag;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Enumdecl{
void*sc;struct _tuple2*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{
struct _tuple2*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*
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
f2;};struct Cyc_Absyn_Using_d_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*
f2;};struct Cyc_Absyn_ExternC_d_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_struct{
int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Decl{void*
r;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_ArrayElement_struct{int tag;
struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_struct{int tag;struct
_dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];int Cyc_Absyn_qvar_cmp(
struct _tuple2*,struct _tuple2*);int Cyc_Absyn_tvar_cmp(struct Cyc_Absyn_Tvar*,
struct Cyc_Absyn_Tvar*);struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(struct Cyc_Position_Segment*);
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(struct Cyc_Position_Segment*);union Cyc_Absyn_Constraint*
Cyc_Absyn_new_conref(void*x);union Cyc_Absyn_Constraint*Cyc_Absyn_empty_conref();
union Cyc_Absyn_Constraint*Cyc_Absyn_compress_conref(union Cyc_Absyn_Constraint*x);
void*Cyc_Absyn_conref_val(union Cyc_Absyn_Constraint*x);void*Cyc_Absyn_conref_def(
void*y,union Cyc_Absyn_Constraint*x);void*Cyc_Absyn_conref_constr(void*y,union Cyc_Absyn_Constraint*
x);extern union Cyc_Absyn_Constraint*Cyc_Absyn_true_conref;extern union Cyc_Absyn_Constraint*
Cyc_Absyn_false_conref;extern union Cyc_Absyn_Constraint*Cyc_Absyn_bounds_one_conref;
void*Cyc_Absyn_compress_kb(void*);void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,
struct Cyc_Core_Opt*tenv);extern void*Cyc_Absyn_uint_typ;extern void*Cyc_Absyn_ulong_typ;
extern void*Cyc_Absyn_ulonglong_typ;extern void*Cyc_Absyn_sint_typ;extern void*Cyc_Absyn_slong_typ;
extern void*Cyc_Absyn_slonglong_typ;extern void*Cyc_Absyn_empty_effect;extern
struct _tuple2*Cyc_Absyn_datatype_print_arg_qvar;extern struct _tuple2*Cyc_Absyn_datatype_scanf_arg_qvar;
extern void*Cyc_Absyn_bounds_one;void*Cyc_Absyn_atb_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual
tq,void*b,union Cyc_Absyn_Constraint*zero_term);struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(
struct Cyc_Absyn_Exp*);struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,struct
Cyc_Position_Segment*);struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct
Cyc_List_List*,struct _dyneither_ptr*);struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(
struct Cyc_Absyn_Aggrdecl*,struct _dyneither_ptr*);struct _tuple11{struct Cyc_Absyn_Tqual
f1;void*f2;};struct _tuple11*Cyc_Absyn_lookup_tuple_field(struct Cyc_List_List*,
int);struct _dyneither_ptr Cyc_Absyn_attribute2string(void*);int Cyc_Absyn_fntype_att(
void*a);struct _tuple12{void*f1;struct _tuple2*f2;};struct _tuple12 Cyc_Absyn_aggr_kinded_name(
union Cyc_Absyn_AggrInfoU);struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(
union Cyc_Absyn_AggrInfoU info);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;
struct Cyc_Absynpp_Params{int expand_typedefs: 1;int qvar_to_Cids: 1;int
add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int rewrite_temp_tvars: 1;int
print_all_tvars: 1;int print_all_kinds: 1;int print_all_effects: 1;int
print_using_stmts: 1;int print_externC_stmts: 1;int print_full_evars: 1;int
print_zeroterm: 1;int generate_line_directives: 1;int use_curr_namespace: 1;struct Cyc_List_List*
curr_namespace;};struct _dyneither_ptr Cyc_Absynpp_typ2string(void*);struct
_dyneither_ptr Cyc_Absynpp_kind2string(void*);struct _dyneither_ptr Cyc_Absynpp_kindbound2string(
void*);struct _dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);struct
_dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple2*);struct Cyc_Iter_Iter{void*
env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);
struct Cyc_Set_Set;extern char Cyc_Set_Absent[11];struct Cyc_Dict_T;struct Cyc_Dict_Dict{
int(*rel)(void*,void*);struct _RegionHandle*r;struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[
12];extern char Cyc_Dict_Absent[11];struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict
d,void*k,void*v);void*Cyc_Dict_lookup(struct Cyc_Dict_Dict d,void*k);struct _tuple0*
Cyc_Dict_rchoose(struct _RegionHandle*r,struct Cyc_Dict_Dict d);struct _tuple0*Cyc_Dict_rchoose(
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
le;int allow_valueof;};struct _RegionHandle*Cyc_Tcenv_get_fnrgn(struct Cyc_Tcenv_Tenv*);
void*Cyc_Tcenv_lookup_ordinary(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,struct
Cyc_Position_Segment*,struct _tuple2*);struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl(
struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,struct _tuple2*);struct Cyc_Absyn_Datatypedecl**
Cyc_Tcenv_lookup_datatypedecl(struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,
struct _tuple2*);struct Cyc_Absyn_Enumdecl**Cyc_Tcenv_lookup_enumdecl(struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*,struct _tuple2*);struct Cyc_Absyn_Typedefdecl*Cyc_Tcenv_lookup_typedefdecl(
struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,struct _tuple2*);struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_allow_valueof(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*);struct Cyc_List_List*
Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*);struct Cyc_Core_Opt*Cyc_Tcenv_lookup_opt_type_vars(
struct Cyc_Tcenv_Tenv*te);int Cyc_Tcenv_region_outlives(struct Cyc_Tcenv_Tenv*,void*
r1,void*r2);unsigned int Cyc_strlen(struct _dyneither_ptr s);int Cyc_strcmp(struct
_dyneither_ptr s1,struct _dyneither_ptr s2);int Cyc_strptrcmp(struct _dyneither_ptr*
s1,struct _dyneither_ptr*s2);struct _dyneither_ptr Cyc_strconcat(struct
_dyneither_ptr,struct _dyneither_ptr);struct _tuple13{unsigned int f1;int f2;};
struct _tuple13 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);int Cyc_Evexp_same_const_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);int Cyc_Evexp_lte_const_exp(struct
Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);int Cyc_Evexp_const_exp_cmp(struct Cyc_Absyn_Exp*
e1,struct Cyc_Absyn_Exp*e2);void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap);void Cyc_Tcutil_terr(struct Cyc_Position_Segment*,struct
_dyneither_ptr fmt,struct _dyneither_ptr ap);void Cyc_Tcutil_warn(struct Cyc_Position_Segment*,
struct _dyneither_ptr fmt,struct _dyneither_ptr ap);void Cyc_Tcutil_flush_warnings();
extern struct Cyc_Core_Opt*Cyc_Tcutil_empty_var_set;void*Cyc_Tcutil_copy_type(void*
t);int Cyc_Tcutil_kind_leq(void*k1,void*k2);void*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*
t);void*Cyc_Tcutil_typ_kind(void*t);void*Cyc_Tcutil_compress(void*t);void Cyc_Tcutil_unchecked_cast(
struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*,void*);int Cyc_Tcutil_coerce_arg(
struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*);int Cyc_Tcutil_coerce_assign(
struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*);int Cyc_Tcutil_coerce_to_bool(
struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*);int Cyc_Tcutil_coerce_list(struct Cyc_Tcenv_Tenv*,
void*,struct Cyc_List_List*);int Cyc_Tcutil_coerce_uint_typ(struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Exp*);int Cyc_Tcutil_coerce_sint_typ(struct Cyc_Tcenv_Tenv*,struct
Cyc_Absyn_Exp*);int Cyc_Tcutil_coerceable(void*);int Cyc_Tcutil_silent_castable(
struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,void*,void*);void*Cyc_Tcutil_castable(
struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,void*,void*);int Cyc_Tcutil_is_integral(
struct Cyc_Absyn_Exp*);int Cyc_Tcutil_is_numeric(struct Cyc_Absyn_Exp*);int Cyc_Tcutil_is_function_type(
void*t);int Cyc_Tcutil_is_pointer_type(void*t);int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*
e);int Cyc_Tcutil_is_pointer_or_boxed(void*t,int*is_dyneither_ptr);extern struct
Cyc_Core_Opt Cyc_Tcutil_rk;extern struct Cyc_Core_Opt Cyc_Tcutil_trk;extern struct Cyc_Core_Opt
Cyc_Tcutil_urk;extern struct Cyc_Core_Opt Cyc_Tcutil_ak;extern struct Cyc_Core_Opt Cyc_Tcutil_bk;
extern struct Cyc_Core_Opt Cyc_Tcutil_mk;extern struct Cyc_Core_Opt Cyc_Tcutil_ek;
extern struct Cyc_Core_Opt Cyc_Tcutil_ik;struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_opt(
void*k);void*Cyc_Tcutil_kind_to_bound(void*k);struct _tuple14{struct Cyc_Absyn_Tvar*
f1;void*f2;};struct _tuple14 Cyc_Tcutil_swap_kind(void*t,void*kb);int Cyc_Tcutil_zero_to_null(
struct Cyc_Tcenv_Tenv*,void*t,struct Cyc_Absyn_Exp*e);void*Cyc_Tcutil_max_arithmetic_type(
void*,void*);void Cyc_Tcutil_explain_failure();int Cyc_Tcutil_unify(void*,void*);
int Cyc_Tcutil_typecmp(void*,void*);void*Cyc_Tcutil_substitute(struct Cyc_List_List*,
void*);void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,
void*);int Cyc_Tcutil_subset_effect(int may_constrain_evars,void*e1,void*e2);int
Cyc_Tcutil_region_in_effect(int constrain,void*r,void*e);void*Cyc_Tcutil_fndecl2typ(
struct Cyc_Absyn_Fndecl*);struct _tuple14*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*,
struct Cyc_Absyn_Tvar*);struct _tuple15{struct Cyc_List_List*f1;struct _RegionHandle*
f2;};struct _tuple14*Cyc_Tcutil_r_make_inst_var(struct _tuple15*,struct Cyc_Absyn_Tvar*);
void Cyc_Tcutil_check_bitfield(struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*
te,void*field_typ,struct Cyc_Absyn_Exp*width,struct _dyneither_ptr*fn);void Cyc_Tcutil_check_contains_assign(
struct Cyc_Absyn_Exp*);void Cyc_Tcutil_check_valid_toplevel_type(struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Tenv*,void*);void Cyc_Tcutil_check_fndecl_valid_type(struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Fndecl*);void Cyc_Tcutil_check_type(struct
Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*bound_tvars,void*
k,int allow_evars,void*);void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*vs,
struct Cyc_Position_Segment*loc,struct _dyneither_ptr err_msg);void Cyc_Tcutil_check_unique_tvars(
struct Cyc_Position_Segment*,struct Cyc_List_List*);void Cyc_Tcutil_check_nonzero_bound(
struct Cyc_Position_Segment*,union Cyc_Absyn_Constraint*);void Cyc_Tcutil_check_bound(
struct Cyc_Position_Segment*,unsigned int i,union Cyc_Absyn_Constraint*);int Cyc_Tcutil_is_bound_one(
union Cyc_Absyn_Constraint*b);int Cyc_Tcutil_equal_tqual(struct Cyc_Absyn_Tqual tq1,
struct Cyc_Absyn_Tqual tq2);struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(
struct _RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,
void*,struct Cyc_List_List*fields);int Cyc_Tcutil_is_tagged_pointer_typ(void*);int
Cyc_Tcutil_is_tagged_pointer_typ_elt(void*t,void**elt_typ_dest);int Cyc_Tcutil_is_zero_pointer_typ_elt(
void*t,void**elt_typ_dest);int Cyc_Tcutil_is_zero_ptr_type(void*t,void**ptr_type,
int*is_dyneither,void**elt_type);int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*
e1,void**ptr_type,int*is_dyneither,void**elt_type);int Cyc_Tcutil_is_noalias_pointer(
void*t);int Cyc_Tcutil_is_noalias_path(struct _RegionHandle*,struct Cyc_Absyn_Exp*e);
int Cyc_Tcutil_is_noalias_pointer_or_aggr(struct _RegionHandle*,void*t);void*Cyc_Tcutil_array_to_ptr(
struct Cyc_Tcenv_Tenv*,void*t,struct Cyc_Absyn_Exp*e);struct _tuple16{int f1;void*f2;
};struct _tuple16 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*
e);void*Cyc_Tcutil_normalize_effect(void*e);struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(
void*k);int Cyc_Tcutil_new_tvar_id();void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*);
void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*);int Cyc_Tcutil_is_temp_tvar(
struct Cyc_Absyn_Tvar*);void Cyc_Tcutil_rewrite_temp_tvar(struct Cyc_Absyn_Tvar*);
int Cyc_Tcutil_same_atts(struct Cyc_List_List*,struct Cyc_List_List*);int Cyc_Tcutil_bits_only(
void*t);int Cyc_Tcutil_is_const_exp(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e);
void*Cyc_Tcutil_snd_tqt(struct _tuple11*);int Cyc_Tcutil_supports_default(void*);
int Cyc_Tcutil_admits_zero(void*t);int Cyc_Tcutil_is_noreturn(void*);int Cyc_Tcutil_extract_const_from_typedef(
struct Cyc_Position_Segment*,int declared_const,void*);struct Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(
void*t,struct Cyc_List_List*atts);int Cyc_Tcutil_rgn_of_pointer(void*t,void**rgn);
void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);struct
Cyc_Tcexp_TestEnv{struct _tuple0*eq;int isTrue;};struct Cyc_Tcexp_TestEnv Cyc_Tcexp_tcTest(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr msg_part);void
Cyc_Tc_tcAggrdecl(struct Cyc_Tcenv_Tenv*,struct Cyc_Tcenv_Genv*,struct Cyc_Position_Segment*,
struct Cyc_Absyn_Aggrdecl*);void Cyc_Tc_tcDatatypedecl(struct Cyc_Tcenv_Tenv*,
struct Cyc_Tcenv_Genv*,struct Cyc_Position_Segment*,struct Cyc_Absyn_Datatypedecl*);
void Cyc_Tc_tcEnumdecl(struct Cyc_Tcenv_Tenv*,struct Cyc_Tcenv_Genv*,struct Cyc_Position_Segment*,
struct Cyc_Absyn_Enumdecl*);extern int Cyc_Cyclone_tovc_r;char Cyc_Tcutil_Unify[10]="\000\000\000\000Unify\000";
void Cyc_Tcutil_unify_it(void*t1,void*t2);void*Cyc_Tcutil_t1_failure=(void*)0;int
Cyc_Tcutil_tq1_const=0;void*Cyc_Tcutil_t2_failure=(void*)0;int Cyc_Tcutil_tq2_const=
0;struct _dyneither_ptr Cyc_Tcutil_failure_reason=(struct _dyneither_ptr){(void*)0,(
void*)0,(void*)(0 + 0)};void Cyc_Tcutil_explain_failure(){if(Cyc_Position_num_errors
>= Cyc_Position_max_errors)return;Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);{
struct _dyneither_ptr s1=(struct _dyneither_ptr)({struct Cyc_String_pa_struct _tmp22;
_tmp22.tag=0;_tmp22.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
Cyc_Tcutil_t1_failure));{struct Cyc_String_pa_struct _tmp21;_tmp21.tag=0;_tmp21.f1=(
struct _dyneither_ptr)(Cyc_Tcutil_tq1_const?({const char*_tmp23="const ";
_tag_dyneither(_tmp23,sizeof(char),7);}):({const char*_tmp24="";_tag_dyneither(
_tmp24,sizeof(char),1);}));{void*_tmp1F[2]={& _tmp21,& _tmp22};Cyc_aprintf(({const
char*_tmp20="%s%s";_tag_dyneither(_tmp20,sizeof(char),5);}),_tag_dyneither(
_tmp1F,sizeof(void*),2));}}});struct _dyneither_ptr s2=(struct _dyneither_ptr)({
struct Cyc_String_pa_struct _tmp1C;_tmp1C.tag=0;_tmp1C.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string(Cyc_Tcutil_t2_failure));{struct Cyc_String_pa_struct
_tmp1B;_tmp1B.tag=0;_tmp1B.f1=(struct _dyneither_ptr)(Cyc_Tcutil_tq2_const?({
const char*_tmp1D="const ";_tag_dyneither(_tmp1D,sizeof(char),7);}):({const char*
_tmp1E="";_tag_dyneither(_tmp1E,sizeof(char),1);}));{void*_tmp19[2]={& _tmp1B,&
_tmp1C};Cyc_aprintf(({const char*_tmp1A="%s%s";_tag_dyneither(_tmp1A,sizeof(char),
5);}),_tag_dyneither(_tmp19,sizeof(void*),2));}}});int pos=2;({struct Cyc_String_pa_struct
_tmp2;_tmp2.tag=0;_tmp2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s1);{
void*_tmp0[1]={& _tmp2};Cyc_fprintf(Cyc_stderr,({const char*_tmp1="  %s";
_tag_dyneither(_tmp1,sizeof(char),5);}),_tag_dyneither(_tmp0,sizeof(void*),1));}});
pos +=_get_dyneither_size(s1,sizeof(char));if(pos + 5 >= 80){({void*_tmp3=0;Cyc_fprintf(
Cyc_stderr,({const char*_tmp4="\n\t";_tag_dyneither(_tmp4,sizeof(char),3);}),
_tag_dyneither(_tmp3,sizeof(void*),0));});pos=8;}else{({void*_tmp5=0;Cyc_fprintf(
Cyc_stderr,({const char*_tmp6=" ";_tag_dyneither(_tmp6,sizeof(char),2);}),
_tag_dyneither(_tmp5,sizeof(void*),0));});++ pos;}({void*_tmp7=0;Cyc_fprintf(Cyc_stderr,({
const char*_tmp8="and ";_tag_dyneither(_tmp8,sizeof(char),5);}),_tag_dyneither(
_tmp7,sizeof(void*),0));});pos +=4;if(pos + _get_dyneither_size(s2,sizeof(char))>= 
80){({void*_tmp9=0;Cyc_fprintf(Cyc_stderr,({const char*_tmpA="\n\t";
_tag_dyneither(_tmpA,sizeof(char),3);}),_tag_dyneither(_tmp9,sizeof(void*),0));});
pos=8;}({struct Cyc_String_pa_struct _tmpD;_tmpD.tag=0;_tmpD.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)s2);{void*_tmpB[1]={& _tmpD};Cyc_fprintf(
Cyc_stderr,({const char*_tmpC="%s ";_tag_dyneither(_tmpC,sizeof(char),4);}),
_tag_dyneither(_tmpB,sizeof(void*),1));}});pos +=_get_dyneither_size(s2,sizeof(
char))+ 1;if(pos + 17 >= 80){({void*_tmpE=0;Cyc_fprintf(Cyc_stderr,({const char*
_tmpF="\n\t";_tag_dyneither(_tmpF,sizeof(char),3);}),_tag_dyneither(_tmpE,
sizeof(void*),0));});pos=8;}({void*_tmp10=0;Cyc_fprintf(Cyc_stderr,({const char*
_tmp11="are not compatible. ";_tag_dyneither(_tmp11,sizeof(char),21);}),
_tag_dyneither(_tmp10,sizeof(void*),0));});pos +=17;if(Cyc_Tcutil_failure_reason.curr
!= (_tag_dyneither(0,0,0)).curr){if(pos + Cyc_strlen((struct _dyneither_ptr)Cyc_Tcutil_failure_reason)
>= 80)({void*_tmp12=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp13="\n\t";
_tag_dyneither(_tmp13,sizeof(char),3);}),_tag_dyneither(_tmp12,sizeof(void*),0));});({
struct Cyc_String_pa_struct _tmp16;_tmp16.tag=0;_tmp16.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Tcutil_failure_reason);{void*_tmp14[1]={& _tmp16};Cyc_fprintf(
Cyc_stderr,({const char*_tmp15="%s";_tag_dyneither(_tmp15,sizeof(char),3);}),
_tag_dyneither(_tmp14,sizeof(void*),1));}});}({void*_tmp17=0;Cyc_fprintf(Cyc_stderr,({
const char*_tmp18="\n";_tag_dyneither(_tmp18,sizeof(char),2);}),_tag_dyneither(
_tmp17,sizeof(void*),0));});Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);}}void
Cyc_Tcutil_terr(struct Cyc_Position_Segment*loc,struct _dyneither_ptr fmt,struct
_dyneither_ptr ap){Cyc_Position_post_error(Cyc_Position_mk_err_elab(loc,(struct
_dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap)));}void*Cyc_Tcutil_impos(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap){struct _dyneither_ptr msg=(struct
_dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap);({struct Cyc_String_pa_struct
_tmp27;_tmp27.tag=0;_tmp27.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg);{
void*_tmp25[1]={& _tmp27};Cyc_fprintf(Cyc_stderr,({const char*_tmp26="Compiler Error (Tcutil::impos): %s\n";
_tag_dyneither(_tmp26,sizeof(char),36);}),_tag_dyneither(_tmp25,sizeof(void*),1));}});
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp28=_cycalloc(sizeof(*_tmp28));_tmp28[0]=({struct Cyc_Core_Impossible_struct
_tmp29;_tmp29.tag=Cyc_Core_Impossible;_tmp29.f1=msg;_tmp29;});_tmp28;}));}static
struct _dyneither_ptr Cyc_Tcutil_tvar2string(struct Cyc_Absyn_Tvar*tv){return*tv->name;}
void Cyc_Tcutil_print_tvars(struct Cyc_List_List*tvs){for(0;tvs != 0;tvs=tvs->tl){({
struct Cyc_String_pa_struct _tmp2D;_tmp2D.tag=0;_tmp2D.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_kindbound2string(((struct Cyc_Absyn_Tvar*)tvs->hd)->kind));{
struct Cyc_String_pa_struct _tmp2C;_tmp2C.tag=0;_tmp2C.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Tcutil_tvar2string((struct Cyc_Absyn_Tvar*)tvs->hd));{
void*_tmp2A[2]={& _tmp2C,& _tmp2D};Cyc_fprintf(Cyc_stderr,({const char*_tmp2B="%s::%s ";
_tag_dyneither(_tmp2B,sizeof(char),8);}),_tag_dyneither(_tmp2A,sizeof(void*),2));}}});}({
void*_tmp2E=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp2F="\n";_tag_dyneither(
_tmp2F,sizeof(char),2);}),_tag_dyneither(_tmp2E,sizeof(void*),0));});Cyc_fflush((
struct Cyc___cycFILE*)Cyc_stderr);}static struct Cyc_List_List*Cyc_Tcutil_warning_segs=
0;static struct Cyc_List_List*Cyc_Tcutil_warning_msgs=0;void Cyc_Tcutil_warn(struct
Cyc_Position_Segment*sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){struct
_dyneither_ptr msg=(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap);
Cyc_Tcutil_warning_segs=({struct Cyc_List_List*_tmp30=_cycalloc(sizeof(*_tmp30));
_tmp30->hd=sg;_tmp30->tl=Cyc_Tcutil_warning_segs;_tmp30;});Cyc_Tcutil_warning_msgs=({
struct Cyc_List_List*_tmp31=_cycalloc(sizeof(*_tmp31));_tmp31->hd=({struct
_dyneither_ptr*_tmp32=_cycalloc(sizeof(*_tmp32));_tmp32[0]=msg;_tmp32;});_tmp31->tl=
Cyc_Tcutil_warning_msgs;_tmp31;});}void Cyc_Tcutil_flush_warnings(){if(Cyc_Tcutil_warning_segs
== 0)return;({void*_tmp33=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp34="***Warnings***\n";
_tag_dyneither(_tmp34,sizeof(char),16);}),_tag_dyneither(_tmp33,sizeof(void*),0));});{
struct Cyc_List_List*_tmp35=Cyc_Position_strings_of_segments(Cyc_Tcutil_warning_segs);
Cyc_Tcutil_warning_segs=0;Cyc_Tcutil_warning_msgs=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_Tcutil_warning_msgs);while(Cyc_Tcutil_warning_msgs
!= 0){({struct Cyc_String_pa_struct _tmp39;_tmp39.tag=0;_tmp39.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)((struct Cyc_List_List*)
_check_null(Cyc_Tcutil_warning_msgs))->hd));{struct Cyc_String_pa_struct _tmp38;
_tmp38.tag=0;_tmp38.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct
_dyneither_ptr*)((struct Cyc_List_List*)_check_null(_tmp35))->hd));{void*_tmp36[2]={&
_tmp38,& _tmp39};Cyc_fprintf(Cyc_stderr,({const char*_tmp37="%s: %s\n";
_tag_dyneither(_tmp37,sizeof(char),8);}),_tag_dyneither(_tmp36,sizeof(void*),2));}}});
_tmp35=_tmp35->tl;Cyc_Tcutil_warning_msgs=((struct Cyc_List_List*)_check_null(Cyc_Tcutil_warning_msgs))->tl;}({
void*_tmp3A=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp3B="**************\n";
_tag_dyneither(_tmp3B,sizeof(char),16);}),_tag_dyneither(_tmp3A,sizeof(void*),0));});
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);}}struct Cyc_Core_Opt*Cyc_Tcutil_empty_var_set=
0;static int Cyc_Tcutil_fast_tvar_cmp(struct Cyc_Absyn_Tvar*tv1,struct Cyc_Absyn_Tvar*
tv2){return tv1->identity - tv2->identity;}void*Cyc_Tcutil_compress(void*t){void*
_tmp3C=t;struct Cyc_Core_Opt*_tmp3D;void**_tmp3E;void**_tmp3F;void***_tmp40;
struct Cyc_Core_Opt*_tmp41;struct Cyc_Core_Opt**_tmp42;_LL1: if(_tmp3C <= (void*)4)
goto _LL9;if(*((int*)_tmp3C)!= 0)goto _LL3;_tmp3D=((struct Cyc_Absyn_Evar_struct*)
_tmp3C)->f2;if(_tmp3D != 0)goto _LL3;_LL2: goto _LL4;_LL3: if(*((int*)_tmp3C)!= 16)
goto _LL5;_tmp3E=((struct Cyc_Absyn_TypedefType_struct*)_tmp3C)->f4;if(_tmp3E != 0)
goto _LL5;_LL4: return t;_LL5: if(*((int*)_tmp3C)!= 16)goto _LL7;_tmp3F=((struct Cyc_Absyn_TypedefType_struct*)
_tmp3C)->f4;_tmp40=(void***)&((struct Cyc_Absyn_TypedefType_struct*)_tmp3C)->f4;
_LL6: {void*t2=Cyc_Tcutil_compress(*((void**)_check_null(*_tmp40)));if(t2 != *((
void**)_check_null(*_tmp40)))*_tmp40=({void**_tmp43=_cycalloc(sizeof(*_tmp43));
_tmp43[0]=t2;_tmp43;});return t2;}_LL7: if(*((int*)_tmp3C)!= 0)goto _LL9;_tmp41=((
struct Cyc_Absyn_Evar_struct*)_tmp3C)->f2;_tmp42=(struct Cyc_Core_Opt**)&((struct
Cyc_Absyn_Evar_struct*)_tmp3C)->f2;_LL8: {void*t2=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(*_tmp42))->v);if(t2 != (void*)((struct Cyc_Core_Opt*)
_check_null(*_tmp42))->v)*_tmp42=({struct Cyc_Core_Opt*_tmp44=_cycalloc(sizeof(*
_tmp44));_tmp44->v=(void*)t2;_tmp44;});return t2;}_LL9:;_LLA: return t;_LL0:;}void*
Cyc_Tcutil_copy_type(void*t);static struct Cyc_List_List*Cyc_Tcutil_copy_types(
struct Cyc_List_List*ts){return Cyc_List_map(Cyc_Tcutil_copy_type,ts);}static union
Cyc_Absyn_Constraint*Cyc_Tcutil_copy_conref(union Cyc_Absyn_Constraint*cptr){
union Cyc_Absyn_Constraint*_tmp45=cptr;union Cyc_Absyn_Constraint _tmp46;int _tmp47;
union Cyc_Absyn_Constraint _tmp48;void*_tmp49;union Cyc_Absyn_Constraint _tmp4A;
union Cyc_Absyn_Constraint*_tmp4B;_LLC: _tmp46=*_tmp45;if((_tmp46.No_constr).tag != 
3)goto _LLE;_tmp47=(int)(_tmp46.No_constr).val;_LLD: return Cyc_Absyn_empty_conref();
_LLE: _tmp48=*_tmp45;if((_tmp48.Eq_constr).tag != 1)goto _LL10;_tmp49=(void*)(
_tmp48.Eq_constr).val;_LLF: return Cyc_Absyn_new_conref(_tmp49);_LL10: _tmp4A=*
_tmp45;if((_tmp4A.Forward_constr).tag != 2)goto _LLB;_tmp4B=(union Cyc_Absyn_Constraint*)(
_tmp4A.Forward_constr).val;_LL11: return Cyc_Tcutil_copy_conref(_tmp4B);_LLB:;}
static void*Cyc_Tcutil_copy_kindbound(void*kb){void*_tmp4C=Cyc_Absyn_compress_kb(
kb);void*_tmp4D;_LL13: if(*((int*)_tmp4C)!= 1)goto _LL15;_LL14: return(void*)({
struct Cyc_Absyn_Unknown_kb_struct*_tmp4E=_cycalloc(sizeof(*_tmp4E));_tmp4E[0]=({
struct Cyc_Absyn_Unknown_kb_struct _tmp4F;_tmp4F.tag=1;_tmp4F.f1=0;_tmp4F;});
_tmp4E;});_LL15: if(*((int*)_tmp4C)!= 2)goto _LL17;_tmp4D=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp4C)->f2;_LL16: return(void*)({struct Cyc_Absyn_Less_kb_struct*_tmp50=_cycalloc(
sizeof(*_tmp50));_tmp50[0]=({struct Cyc_Absyn_Less_kb_struct _tmp51;_tmp51.tag=2;
_tmp51.f1=0;_tmp51.f2=(void*)_tmp4D;_tmp51;});_tmp50;});_LL17:;_LL18: return kb;
_LL12:;}static struct Cyc_Absyn_Tvar*Cyc_Tcutil_copy_tvar(struct Cyc_Absyn_Tvar*tv){
return({struct Cyc_Absyn_Tvar*_tmp52=_cycalloc(sizeof(*_tmp52));_tmp52->name=tv->name;
_tmp52->identity=- 1;_tmp52->kind=Cyc_Tcutil_copy_kindbound(tv->kind);_tmp52;});}
static struct _tuple9*Cyc_Tcutil_copy_arg(struct _tuple9*arg){struct _tuple9 _tmp54;
struct Cyc_Core_Opt*_tmp55;struct Cyc_Absyn_Tqual _tmp56;void*_tmp57;struct _tuple9*
_tmp53=arg;_tmp54=*_tmp53;_tmp55=_tmp54.f1;_tmp56=_tmp54.f2;_tmp57=_tmp54.f3;
return({struct _tuple9*_tmp58=_cycalloc(sizeof(*_tmp58));_tmp58->f1=_tmp55;_tmp58->f2=
_tmp56;_tmp58->f3=Cyc_Tcutil_copy_type(_tmp57);_tmp58;});}static struct _tuple11*
Cyc_Tcutil_copy_tqt(struct _tuple11*arg){struct _tuple11 _tmp5A;struct Cyc_Absyn_Tqual
_tmp5B;void*_tmp5C;struct _tuple11*_tmp59=arg;_tmp5A=*_tmp59;_tmp5B=_tmp5A.f1;
_tmp5C=_tmp5A.f2;return({struct _tuple11*_tmp5D=_cycalloc(sizeof(*_tmp5D));_tmp5D->f1=
_tmp5B;_tmp5D->f2=Cyc_Tcutil_copy_type(_tmp5C);_tmp5D;});}static struct Cyc_Absyn_Aggrfield*
Cyc_Tcutil_copy_field(struct Cyc_Absyn_Aggrfield*f){return({struct Cyc_Absyn_Aggrfield*
_tmp5E=_cycalloc(sizeof(*_tmp5E));_tmp5E->name=f->name;_tmp5E->tq=f->tq;_tmp5E->type=
Cyc_Tcutil_copy_type(f->type);_tmp5E->width=f->width;_tmp5E->attributes=f->attributes;
_tmp5E;});}static struct _tuple0*Cyc_Tcutil_copy_rgncmp(struct _tuple0*x){struct
_tuple0 _tmp60;void*_tmp61;void*_tmp62;struct _tuple0*_tmp5F=x;_tmp60=*_tmp5F;
_tmp61=_tmp60.f1;_tmp62=_tmp60.f2;return({struct _tuple0*_tmp63=_cycalloc(sizeof(*
_tmp63));_tmp63->f1=Cyc_Tcutil_copy_type(_tmp61);_tmp63->f2=Cyc_Tcutil_copy_type(
_tmp62);_tmp63;});}static struct Cyc_Absyn_Enumfield*Cyc_Tcutil_copy_enumfield(
struct Cyc_Absyn_Enumfield*f){return({struct Cyc_Absyn_Enumfield*_tmp64=_cycalloc(
sizeof(*_tmp64));_tmp64->name=f->name;_tmp64->tag=f->tag;_tmp64->loc=f->loc;
_tmp64;});}void*Cyc_Tcutil_copy_type(void*t){void*_tmp65=Cyc_Tcutil_compress(t);
struct Cyc_Absyn_Tvar*_tmp66;struct Cyc_Absyn_DatatypeInfo _tmp67;union Cyc_Absyn_DatatypeInfoU
_tmp68;struct Cyc_List_List*_tmp69;struct Cyc_Core_Opt*_tmp6A;struct Cyc_Absyn_DatatypeFieldInfo
_tmp6B;union Cyc_Absyn_DatatypeFieldInfoU _tmp6C;struct Cyc_List_List*_tmp6D;struct
Cyc_Absyn_PtrInfo _tmp6E;void*_tmp6F;struct Cyc_Absyn_Tqual _tmp70;struct Cyc_Absyn_PtrAtts
_tmp71;void*_tmp72;union Cyc_Absyn_Constraint*_tmp73;union Cyc_Absyn_Constraint*
_tmp74;union Cyc_Absyn_Constraint*_tmp75;struct Cyc_Absyn_PtrLoc*_tmp76;struct Cyc_Absyn_ArrayInfo
_tmp77;void*_tmp78;struct Cyc_Absyn_Tqual _tmp79;struct Cyc_Absyn_Exp*_tmp7A;union
Cyc_Absyn_Constraint*_tmp7B;struct Cyc_Position_Segment*_tmp7C;struct Cyc_Absyn_FnInfo
_tmp7D;struct Cyc_List_List*_tmp7E;struct Cyc_Core_Opt*_tmp7F;void*_tmp80;struct
Cyc_List_List*_tmp81;int _tmp82;struct Cyc_Absyn_VarargInfo*_tmp83;struct Cyc_List_List*
_tmp84;struct Cyc_List_List*_tmp85;struct Cyc_List_List*_tmp86;struct Cyc_Absyn_AggrInfo
_tmp87;union Cyc_Absyn_AggrInfoU _tmp88;struct _tuple4 _tmp89;void*_tmp8A;struct
_tuple2*_tmp8B;struct Cyc_Core_Opt*_tmp8C;struct Cyc_List_List*_tmp8D;struct Cyc_Absyn_AggrInfo
_tmp8E;union Cyc_Absyn_AggrInfoU _tmp8F;struct Cyc_Absyn_Aggrdecl**_tmp90;struct Cyc_List_List*
_tmp91;void*_tmp92;struct Cyc_List_List*_tmp93;struct _tuple2*_tmp94;struct Cyc_Absyn_Enumdecl*
_tmp95;struct Cyc_List_List*_tmp96;void*_tmp97;struct Cyc_Absyn_Exp*_tmp98;void*
_tmp99;void*_tmp9A;void*_tmp9B;struct _tuple2*_tmp9C;struct Cyc_List_List*_tmp9D;
struct Cyc_Absyn_Typedefdecl*_tmp9E;void*_tmp9F;struct Cyc_List_List*_tmpA0;void*
_tmpA1;_LL1A: if((int)_tmp65 != 0)goto _LL1C;_LL1B: goto _LL1D;_LL1C: if(_tmp65 <= (
void*)4)goto _LL28;if(*((int*)_tmp65)!= 0)goto _LL1E;_LL1D: return t;_LL1E: if(*((int*)
_tmp65)!= 1)goto _LL20;_tmp66=((struct Cyc_Absyn_VarType_struct*)_tmp65)->f1;_LL1F:
return(void*)({struct Cyc_Absyn_VarType_struct*_tmpA2=_cycalloc(sizeof(*_tmpA2));
_tmpA2[0]=({struct Cyc_Absyn_VarType_struct _tmpA3;_tmpA3.tag=1;_tmpA3.f1=Cyc_Tcutil_copy_tvar(
_tmp66);_tmpA3;});_tmpA2;});_LL20: if(*((int*)_tmp65)!= 2)goto _LL22;_tmp67=((
struct Cyc_Absyn_DatatypeType_struct*)_tmp65)->f1;_tmp68=_tmp67.datatype_info;
_tmp69=_tmp67.targs;_tmp6A=_tmp67.rgn;_LL21: {struct Cyc_Core_Opt*_tmpA4=(
unsigned int)_tmp6A?({struct Cyc_Core_Opt*_tmpA8=_cycalloc(sizeof(*_tmpA8));
_tmpA8->v=(void*)Cyc_Tcutil_copy_type((void*)_tmp6A->v);_tmpA8;}): 0;return(void*)({
struct Cyc_Absyn_DatatypeType_struct*_tmpA5=_cycalloc(sizeof(*_tmpA5));_tmpA5[0]=({
struct Cyc_Absyn_DatatypeType_struct _tmpA6;_tmpA6.tag=2;_tmpA6.f1=({struct Cyc_Absyn_DatatypeInfo
_tmpA7;_tmpA7.datatype_info=_tmp68;_tmpA7.targs=Cyc_Tcutil_copy_types(_tmp69);
_tmpA7.rgn=_tmpA4;_tmpA7;});_tmpA6;});_tmpA5;});}_LL22: if(*((int*)_tmp65)!= 3)
goto _LL24;_tmp6B=((struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp65)->f1;_tmp6C=
_tmp6B.field_info;_tmp6D=_tmp6B.targs;_LL23: return(void*)({struct Cyc_Absyn_DatatypeFieldType_struct*
_tmpA9=_cycalloc(sizeof(*_tmpA9));_tmpA9[0]=({struct Cyc_Absyn_DatatypeFieldType_struct
_tmpAA;_tmpAA.tag=3;_tmpAA.f1=({struct Cyc_Absyn_DatatypeFieldInfo _tmpAB;_tmpAB.field_info=
_tmp6C;_tmpAB.targs=Cyc_Tcutil_copy_types(_tmp6D);_tmpAB;});_tmpAA;});_tmpA9;});
_LL24: if(*((int*)_tmp65)!= 4)goto _LL26;_tmp6E=((struct Cyc_Absyn_PointerType_struct*)
_tmp65)->f1;_tmp6F=_tmp6E.elt_typ;_tmp70=_tmp6E.elt_tq;_tmp71=_tmp6E.ptr_atts;
_tmp72=_tmp71.rgn;_tmp73=_tmp71.nullable;_tmp74=_tmp71.bounds;_tmp75=_tmp71.zero_term;
_tmp76=_tmp71.ptrloc;_LL25: {void*_tmpAC=Cyc_Tcutil_copy_type(_tmp6F);void*
_tmpAD=Cyc_Tcutil_copy_type(_tmp72);union Cyc_Absyn_Constraint*_tmpAE=((union Cyc_Absyn_Constraint*(*)(
union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(_tmp73);struct Cyc_Absyn_Tqual
_tmpAF=_tmp70;union Cyc_Absyn_Constraint*_tmpB0=Cyc_Tcutil_copy_conref(_tmp74);
union Cyc_Absyn_Constraint*_tmpB1=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*
cptr))Cyc_Tcutil_copy_conref)(_tmp75);return(void*)({struct Cyc_Absyn_PointerType_struct*
_tmpB2=_cycalloc(sizeof(*_tmpB2));_tmpB2[0]=({struct Cyc_Absyn_PointerType_struct
_tmpB3;_tmpB3.tag=4;_tmpB3.f1=({struct Cyc_Absyn_PtrInfo _tmpB4;_tmpB4.elt_typ=
_tmpAC;_tmpB4.elt_tq=_tmpAF;_tmpB4.ptr_atts=({struct Cyc_Absyn_PtrAtts _tmpB5;
_tmpB5.rgn=_tmpAD;_tmpB5.nullable=_tmpAE;_tmpB5.bounds=_tmpB0;_tmpB5.zero_term=
_tmpB1;_tmpB5.ptrloc=_tmp76;_tmpB5;});_tmpB4;});_tmpB3;});_tmpB2;});}_LL26: if(*((
int*)_tmp65)!= 5)goto _LL28;_LL27: goto _LL29;_LL28: if((int)_tmp65 != 1)goto _LL2A;
_LL29: goto _LL2B;_LL2A: if(_tmp65 <= (void*)4)goto _LL46;if(*((int*)_tmp65)!= 6)goto
_LL2C;_LL2B: return t;_LL2C: if(*((int*)_tmp65)!= 7)goto _LL2E;_tmp77=((struct Cyc_Absyn_ArrayType_struct*)
_tmp65)->f1;_tmp78=_tmp77.elt_type;_tmp79=_tmp77.tq;_tmp7A=_tmp77.num_elts;
_tmp7B=_tmp77.zero_term;_tmp7C=_tmp77.zt_loc;_LL2D: return(void*)({struct Cyc_Absyn_ArrayType_struct*
_tmpB6=_cycalloc(sizeof(*_tmpB6));_tmpB6[0]=({struct Cyc_Absyn_ArrayType_struct
_tmpB7;_tmpB7.tag=7;_tmpB7.f1=({struct Cyc_Absyn_ArrayInfo _tmpB8;_tmpB8.elt_type=
Cyc_Tcutil_copy_type(_tmp78);_tmpB8.tq=_tmp79;_tmpB8.num_elts=_tmp7A;_tmpB8.zero_term=((
union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(
_tmp7B);_tmpB8.zt_loc=_tmp7C;_tmpB8;});_tmpB7;});_tmpB6;});_LL2E: if(*((int*)
_tmp65)!= 8)goto _LL30;_tmp7D=((struct Cyc_Absyn_FnType_struct*)_tmp65)->f1;_tmp7E=
_tmp7D.tvars;_tmp7F=_tmp7D.effect;_tmp80=_tmp7D.ret_typ;_tmp81=_tmp7D.args;
_tmp82=_tmp7D.c_varargs;_tmp83=_tmp7D.cyc_varargs;_tmp84=_tmp7D.rgn_po;_tmp85=
_tmp7D.attributes;_LL2F: {struct Cyc_List_List*_tmpB9=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Tcutil_copy_tvar,_tmp7E);struct Cyc_Core_Opt*_tmpBA=_tmp7F == 0?0:({struct Cyc_Core_Opt*
_tmpC4=_cycalloc(sizeof(*_tmpC4));_tmpC4->v=(void*)Cyc_Tcutil_copy_type((void*)
_tmp7F->v);_tmpC4;});void*_tmpBB=Cyc_Tcutil_copy_type(_tmp80);struct Cyc_List_List*
_tmpBC=((struct Cyc_List_List*(*)(struct _tuple9*(*f)(struct _tuple9*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Tcutil_copy_arg,_tmp81);int _tmpBD=_tmp82;struct Cyc_Absyn_VarargInfo*
cyc_varargs2=0;if(_tmp83 != 0){struct Cyc_Absyn_VarargInfo*cv=(struct Cyc_Absyn_VarargInfo*)
_tmp83;cyc_varargs2=({struct Cyc_Absyn_VarargInfo*_tmpBE=_cycalloc(sizeof(*_tmpBE));
_tmpBE->name=cv->name;_tmpBE->tq=cv->tq;_tmpBE->type=Cyc_Tcutil_copy_type(cv->type);
_tmpBE->inject=cv->inject;_tmpBE;});}{struct Cyc_List_List*_tmpBF=((struct Cyc_List_List*(*)(
struct _tuple0*(*f)(struct _tuple0*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_rgncmp,
_tmp84);struct Cyc_List_List*_tmpC0=_tmp85;return(void*)({struct Cyc_Absyn_FnType_struct*
_tmpC1=_cycalloc(sizeof(*_tmpC1));_tmpC1[0]=({struct Cyc_Absyn_FnType_struct
_tmpC2;_tmpC2.tag=8;_tmpC2.f1=({struct Cyc_Absyn_FnInfo _tmpC3;_tmpC3.tvars=_tmpB9;
_tmpC3.effect=_tmpBA;_tmpC3.ret_typ=_tmpBB;_tmpC3.args=_tmpBC;_tmpC3.c_varargs=
_tmpBD;_tmpC3.cyc_varargs=cyc_varargs2;_tmpC3.rgn_po=_tmpBF;_tmpC3.attributes=
_tmpC0;_tmpC3;});_tmpC2;});_tmpC1;});}}_LL30: if(*((int*)_tmp65)!= 9)goto _LL32;
_tmp86=((struct Cyc_Absyn_TupleType_struct*)_tmp65)->f1;_LL31: return(void*)({
struct Cyc_Absyn_TupleType_struct*_tmpC5=_cycalloc(sizeof(*_tmpC5));_tmpC5[0]=({
struct Cyc_Absyn_TupleType_struct _tmpC6;_tmpC6.tag=9;_tmpC6.f1=((struct Cyc_List_List*(*)(
struct _tuple11*(*f)(struct _tuple11*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_tqt,
_tmp86);_tmpC6;});_tmpC5;});_LL32: if(*((int*)_tmp65)!= 10)goto _LL34;_tmp87=((
struct Cyc_Absyn_AggrType_struct*)_tmp65)->f1;_tmp88=_tmp87.aggr_info;if((_tmp88.UnknownAggr).tag
!= 1)goto _LL34;_tmp89=(struct _tuple4)(_tmp88.UnknownAggr).val;_tmp8A=_tmp89.f1;
_tmp8B=_tmp89.f2;_tmp8C=_tmp89.f3;_tmp8D=_tmp87.targs;_LL33: return(void*)({
struct Cyc_Absyn_AggrType_struct*_tmpC7=_cycalloc(sizeof(*_tmpC7));_tmpC7[0]=({
struct Cyc_Absyn_AggrType_struct _tmpC8;_tmpC8.tag=10;_tmpC8.f1=({struct Cyc_Absyn_AggrInfo
_tmpC9;_tmpC9.aggr_info=Cyc_Absyn_UnknownAggr(_tmp8A,_tmp8B,_tmp8C);_tmpC9.targs=
Cyc_Tcutil_copy_types(_tmp8D);_tmpC9;});_tmpC8;});_tmpC7;});_LL34: if(*((int*)
_tmp65)!= 10)goto _LL36;_tmp8E=((struct Cyc_Absyn_AggrType_struct*)_tmp65)->f1;
_tmp8F=_tmp8E.aggr_info;if((_tmp8F.KnownAggr).tag != 2)goto _LL36;_tmp90=(struct
Cyc_Absyn_Aggrdecl**)(_tmp8F.KnownAggr).val;_tmp91=_tmp8E.targs;_LL35: return(
void*)({struct Cyc_Absyn_AggrType_struct*_tmpCA=_cycalloc(sizeof(*_tmpCA));_tmpCA[
0]=({struct Cyc_Absyn_AggrType_struct _tmpCB;_tmpCB.tag=10;_tmpCB.f1=({struct Cyc_Absyn_AggrInfo
_tmpCC;_tmpCC.aggr_info=Cyc_Absyn_KnownAggr(_tmp90);_tmpCC.targs=Cyc_Tcutil_copy_types(
_tmp91);_tmpCC;});_tmpCB;});_tmpCA;});_LL36: if(*((int*)_tmp65)!= 11)goto _LL38;
_tmp92=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)_tmp65)->f1;_tmp93=((struct
Cyc_Absyn_AnonAggrType_struct*)_tmp65)->f2;_LL37: return(void*)({struct Cyc_Absyn_AnonAggrType_struct*
_tmpCD=_cycalloc(sizeof(*_tmpCD));_tmpCD[0]=({struct Cyc_Absyn_AnonAggrType_struct
_tmpCE;_tmpCE.tag=11;_tmpCE.f1=(void*)_tmp92;_tmpCE.f2=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Tcutil_copy_field,_tmp93);_tmpCE;});_tmpCD;});_LL38: if(*((int*)
_tmp65)!= 12)goto _LL3A;_tmp94=((struct Cyc_Absyn_EnumType_struct*)_tmp65)->f1;
_tmp95=((struct Cyc_Absyn_EnumType_struct*)_tmp65)->f2;_LL39: return(void*)({
struct Cyc_Absyn_EnumType_struct*_tmpCF=_cycalloc(sizeof(*_tmpCF));_tmpCF[0]=({
struct Cyc_Absyn_EnumType_struct _tmpD0;_tmpD0.tag=12;_tmpD0.f1=_tmp94;_tmpD0.f2=
_tmp95;_tmpD0;});_tmpCF;});_LL3A: if(*((int*)_tmp65)!= 13)goto _LL3C;_tmp96=((
struct Cyc_Absyn_AnonEnumType_struct*)_tmp65)->f1;_LL3B: return(void*)({struct Cyc_Absyn_AnonEnumType_struct*
_tmpD1=_cycalloc(sizeof(*_tmpD1));_tmpD1[0]=({struct Cyc_Absyn_AnonEnumType_struct
_tmpD2;_tmpD2.tag=13;_tmpD2.f1=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Enumfield*(*
f)(struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_enumfield,
_tmp96);_tmpD2;});_tmpD1;});_LL3C: if(*((int*)_tmp65)!= 18)goto _LL3E;_tmp97=(void*)((
struct Cyc_Absyn_TagType_struct*)_tmp65)->f1;_LL3D: return(void*)({struct Cyc_Absyn_TagType_struct*
_tmpD3=_cycalloc(sizeof(*_tmpD3));_tmpD3[0]=({struct Cyc_Absyn_TagType_struct
_tmpD4;_tmpD4.tag=18;_tmpD4.f1=(void*)Cyc_Tcutil_copy_type(_tmp97);_tmpD4;});
_tmpD3;});_LL3E: if(*((int*)_tmp65)!= 17)goto _LL40;_tmp98=((struct Cyc_Absyn_ValueofType_struct*)
_tmp65)->f1;_LL3F: return(void*)({struct Cyc_Absyn_ValueofType_struct*_tmpD5=
_cycalloc(sizeof(*_tmpD5));_tmpD5[0]=({struct Cyc_Absyn_ValueofType_struct _tmpD6;
_tmpD6.tag=17;_tmpD6.f1=_tmp98;_tmpD6;});_tmpD5;});_LL40: if(*((int*)_tmp65)!= 14)
goto _LL42;_tmp99=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)_tmp65)->f1;
_LL41: return(void*)({struct Cyc_Absyn_RgnHandleType_struct*_tmpD7=_cycalloc(
sizeof(*_tmpD7));_tmpD7[0]=({struct Cyc_Absyn_RgnHandleType_struct _tmpD8;_tmpD8.tag=
14;_tmpD8.f1=(void*)Cyc_Tcutil_copy_type(_tmp99);_tmpD8;});_tmpD7;});_LL42: if(*((
int*)_tmp65)!= 15)goto _LL44;_tmp9A=(void*)((struct Cyc_Absyn_DynRgnType_struct*)
_tmp65)->f1;_tmp9B=(void*)((struct Cyc_Absyn_DynRgnType_struct*)_tmp65)->f2;_LL43:
return(void*)({struct Cyc_Absyn_DynRgnType_struct*_tmpD9=_cycalloc(sizeof(*_tmpD9));
_tmpD9[0]=({struct Cyc_Absyn_DynRgnType_struct _tmpDA;_tmpDA.tag=15;_tmpDA.f1=(
void*)Cyc_Tcutil_copy_type(_tmp9A);_tmpDA.f2=(void*)Cyc_Tcutil_copy_type(_tmp9B);
_tmpDA;});_tmpD9;});_LL44: if(*((int*)_tmp65)!= 16)goto _LL46;_tmp9C=((struct Cyc_Absyn_TypedefType_struct*)
_tmp65)->f1;_tmp9D=((struct Cyc_Absyn_TypedefType_struct*)_tmp65)->f2;_tmp9E=((
struct Cyc_Absyn_TypedefType_struct*)_tmp65)->f3;_LL45: return(void*)({struct Cyc_Absyn_TypedefType_struct*
_tmpDB=_cycalloc(sizeof(*_tmpDB));_tmpDB[0]=({struct Cyc_Absyn_TypedefType_struct
_tmpDC;_tmpDC.tag=16;_tmpDC.f1=_tmp9C;_tmpDC.f2=Cyc_Tcutil_copy_types(_tmp9D);
_tmpDC.f3=_tmp9E;_tmpDC.f4=0;_tmpDC;});_tmpDB;});_LL46: if((int)_tmp65 != 3)goto
_LL48;_LL47: goto _LL49;_LL48: if((int)_tmp65 != 2)goto _LL4A;_LL49: return t;_LL4A: if(
_tmp65 <= (void*)4)goto _LL4C;if(*((int*)_tmp65)!= 19)goto _LL4C;_tmp9F=(void*)((
struct Cyc_Absyn_AccessEff_struct*)_tmp65)->f1;_LL4B: return(void*)({struct Cyc_Absyn_AccessEff_struct*
_tmpDD=_cycalloc(sizeof(*_tmpDD));_tmpDD[0]=({struct Cyc_Absyn_AccessEff_struct
_tmpDE;_tmpDE.tag=19;_tmpDE.f1=(void*)Cyc_Tcutil_copy_type(_tmp9F);_tmpDE;});
_tmpDD;});_LL4C: if(_tmp65 <= (void*)4)goto _LL4E;if(*((int*)_tmp65)!= 20)goto _LL4E;
_tmpA0=((struct Cyc_Absyn_JoinEff_struct*)_tmp65)->f1;_LL4D: return(void*)({struct
Cyc_Absyn_JoinEff_struct*_tmpDF=_cycalloc(sizeof(*_tmpDF));_tmpDF[0]=({struct Cyc_Absyn_JoinEff_struct
_tmpE0;_tmpE0.tag=20;_tmpE0.f1=Cyc_Tcutil_copy_types(_tmpA0);_tmpE0;});_tmpDF;});
_LL4E: if(_tmp65 <= (void*)4)goto _LL19;if(*((int*)_tmp65)!= 21)goto _LL19;_tmpA1=(
void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp65)->f1;_LL4F: return(void*)({struct
Cyc_Absyn_RgnsEff_struct*_tmpE1=_cycalloc(sizeof(*_tmpE1));_tmpE1[0]=({struct Cyc_Absyn_RgnsEff_struct
_tmpE2;_tmpE2.tag=21;_tmpE2.f1=(void*)Cyc_Tcutil_copy_type(_tmpA1);_tmpE2;});
_tmpE1;});_LL19:;}int Cyc_Tcutil_kind_leq(void*k1,void*k2){if(k1 == k2)return 1;{
struct _tuple0 _tmpE4=({struct _tuple0 _tmpE3;_tmpE3.f1=k1;_tmpE3.f2=k2;_tmpE3;});
void*_tmpE5;void*_tmpE6;void*_tmpE7;void*_tmpE8;void*_tmpE9;void*_tmpEA;void*
_tmpEB;void*_tmpEC;void*_tmpED;void*_tmpEE;_LL51: _tmpE5=_tmpE4.f1;if((int)_tmpE5
!= 2)goto _LL53;_tmpE6=_tmpE4.f2;if((int)_tmpE6 != 1)goto _LL53;_LL52: goto _LL54;
_LL53: _tmpE7=_tmpE4.f1;if((int)_tmpE7 != 2)goto _LL55;_tmpE8=_tmpE4.f2;if((int)
_tmpE8 != 0)goto _LL55;_LL54: goto _LL56;_LL55: _tmpE9=_tmpE4.f1;if((int)_tmpE9 != 1)
goto _LL57;_tmpEA=_tmpE4.f2;if((int)_tmpEA != 0)goto _LL57;_LL56: goto _LL58;_LL57:
_tmpEB=_tmpE4.f1;if((int)_tmpEB != 3)goto _LL59;_tmpEC=_tmpE4.f2;if((int)_tmpEC != 
5)goto _LL59;_LL58: goto _LL5A;_LL59: _tmpED=_tmpE4.f1;if((int)_tmpED != 4)goto _LL5B;
_tmpEE=_tmpE4.f2;if((int)_tmpEE != 5)goto _LL5B;_LL5A: return 1;_LL5B:;_LL5C: return 0;
_LL50:;}}static int Cyc_Tcutil_is_region_kind(void*k){void*_tmpEF=k;_LL5E: if((int)
_tmpEF != 3)goto _LL60;_LL5F: goto _LL61;_LL60: if((int)_tmpEF != 5)goto _LL62;_LL61:
goto _LL63;_LL62: if((int)_tmpEF != 4)goto _LL64;_LL63: return 1;_LL64:;_LL65: return 0;
_LL5D:;}void*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*tv){void*_tmpF0=Cyc_Absyn_compress_kb(
tv->kind);void*_tmpF1;void*_tmpF2;_LL67: if(*((int*)_tmpF0)!= 0)goto _LL69;_tmpF1=(
void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmpF0)->f1;_LL68: return _tmpF1;_LL69: if(*((
int*)_tmpF0)!= 2)goto _LL6B;_tmpF2=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmpF0)->f2;_LL6A: return _tmpF2;_LL6B:;_LL6C:({void*_tmpF3=0;((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmpF4="kind not suitably constrained!";
_tag_dyneither(_tmpF4,sizeof(char),31);}),_tag_dyneither(_tmpF3,sizeof(void*),0));});
_LL66:;}struct _tuple14 Cyc_Tcutil_swap_kind(void*t,void*kb){void*_tmpF5=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_Tvar*_tmpF6;_LL6E: if(_tmpF5 <= (void*)4)goto _LL70;if(*((int*)
_tmpF5)!= 1)goto _LL70;_tmpF6=((struct Cyc_Absyn_VarType_struct*)_tmpF5)->f1;_LL6F: {
void*_tmpF7=_tmpF6->kind;_tmpF6->kind=kb;return({struct _tuple14 _tmpF8;_tmpF8.f1=
_tmpF6;_tmpF8.f2=_tmpF7;_tmpF8;});}_LL70:;_LL71:({struct Cyc_String_pa_struct
_tmpFB;_tmpFB.tag=0;_tmpFB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t));{void*_tmpF9[1]={& _tmpFB};((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmpFA="swap_kind: cannot update the kind of %s";
_tag_dyneither(_tmpFA,sizeof(char),40);}),_tag_dyneither(_tmpF9,sizeof(void*),1));}});
_LL6D:;}void*Cyc_Tcutil_typ_kind(void*t){void*_tmpFC=Cyc_Tcutil_compress(t);
struct Cyc_Core_Opt*_tmpFD;struct Cyc_Absyn_Tvar*_tmpFE;void*_tmpFF;struct Cyc_Absyn_DatatypeFieldInfo
_tmp100;union Cyc_Absyn_DatatypeFieldInfoU _tmp101;struct _tuple3 _tmp102;struct Cyc_Absyn_Datatypedecl*
_tmp103;struct Cyc_Absyn_Datatypefield*_tmp104;struct Cyc_Absyn_DatatypeFieldInfo
_tmp105;union Cyc_Absyn_DatatypeFieldInfoU _tmp106;struct Cyc_Absyn_UnknownDatatypeFieldInfo
_tmp107;struct Cyc_Absyn_Enumdecl*_tmp108;struct Cyc_Absyn_AggrInfo _tmp109;union
Cyc_Absyn_AggrInfoU _tmp10A;struct _tuple4 _tmp10B;struct Cyc_Absyn_AggrInfo _tmp10C;
union Cyc_Absyn_AggrInfoU _tmp10D;struct Cyc_Absyn_Aggrdecl**_tmp10E;struct Cyc_Absyn_Aggrdecl*
_tmp10F;struct Cyc_Absyn_Aggrdecl _tmp110;struct Cyc_Absyn_AggrdeclImpl*_tmp111;
struct Cyc_Absyn_Enumdecl*_tmp112;struct Cyc_Absyn_PtrInfo _tmp113;struct Cyc_Absyn_Typedefdecl*
_tmp114;_LL73: if(_tmpFC <= (void*)4)goto _LL77;if(*((int*)_tmpFC)!= 0)goto _LL75;
_tmpFD=((struct Cyc_Absyn_Evar_struct*)_tmpFC)->f1;_LL74: return(void*)((struct Cyc_Core_Opt*)
_check_null(_tmpFD))->v;_LL75: if(*((int*)_tmpFC)!= 1)goto _LL77;_tmpFE=((struct
Cyc_Absyn_VarType_struct*)_tmpFC)->f1;_LL76: return Cyc_Tcutil_tvar_kind(_tmpFE);
_LL77: if((int)_tmpFC != 0)goto _LL79;_LL78: return(void*)1;_LL79: if(_tmpFC <= (void*)
4)goto _LL7B;if(*((int*)_tmpFC)!= 5)goto _LL7B;_tmpFF=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmpFC)->f2;_LL7A: return(_tmpFF == (void*)((void*)2) || _tmpFF == (void*)((void*)3))?(
void*)2:(void*)1;_LL7B: if((int)_tmpFC != 1)goto _LL7D;_LL7C: goto _LL7E;_LL7D: if(
_tmpFC <= (void*)4)goto _LL85;if(*((int*)_tmpFC)!= 6)goto _LL7F;_LL7E: goto _LL80;
_LL7F: if(*((int*)_tmpFC)!= 8)goto _LL81;_LL80: return(void*)1;_LL81: if(*((int*)
_tmpFC)!= 15)goto _LL83;_LL82: goto _LL84;_LL83: if(*((int*)_tmpFC)!= 14)goto _LL85;
_LL84: return(void*)2;_LL85: if((int)_tmpFC != 3)goto _LL87;_LL86: return(void*)4;
_LL87: if((int)_tmpFC != 2)goto _LL89;_LL88: return(void*)3;_LL89: if(_tmpFC <= (void*)
4)goto _LL8B;if(*((int*)_tmpFC)!= 2)goto _LL8B;_LL8A: return(void*)2;_LL8B: if(
_tmpFC <= (void*)4)goto _LL8D;if(*((int*)_tmpFC)!= 3)goto _LL8D;_tmp100=((struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmpFC)->f1;_tmp101=_tmp100.field_info;if((_tmp101.KnownDatatypefield).tag != 2)
goto _LL8D;_tmp102=(struct _tuple3)(_tmp101.KnownDatatypefield).val;_tmp103=
_tmp102.f1;_tmp104=_tmp102.f2;_LL8C: if(_tmp104->typs == 0)return(void*)2;else{
return(void*)1;}_LL8D: if(_tmpFC <= (void*)4)goto _LL8F;if(*((int*)_tmpFC)!= 3)goto
_LL8F;_tmp105=((struct Cyc_Absyn_DatatypeFieldType_struct*)_tmpFC)->f1;_tmp106=
_tmp105.field_info;if((_tmp106.UnknownDatatypefield).tag != 1)goto _LL8F;_tmp107=(
struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmp106.UnknownDatatypefield).val;
_LL8E:({void*_tmp115=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(({const char*_tmp116="typ_kind: Unresolved DatatypeFieldType";
_tag_dyneither(_tmp116,sizeof(char),39);}),_tag_dyneither(_tmp115,sizeof(void*),
0));});_LL8F: if(_tmpFC <= (void*)4)goto _LL91;if(*((int*)_tmpFC)!= 12)goto _LL91;
_tmp108=((struct Cyc_Absyn_EnumType_struct*)_tmpFC)->f2;if(_tmp108 != 0)goto _LL91;
_LL90: goto _LL92;_LL91: if(_tmpFC <= (void*)4)goto _LL93;if(*((int*)_tmpFC)!= 10)
goto _LL93;_tmp109=((struct Cyc_Absyn_AggrType_struct*)_tmpFC)->f1;_tmp10A=_tmp109.aggr_info;
if((_tmp10A.UnknownAggr).tag != 1)goto _LL93;_tmp10B=(struct _tuple4)(_tmp10A.UnknownAggr).val;
_LL92: return(void*)0;_LL93: if(_tmpFC <= (void*)4)goto _LL95;if(*((int*)_tmpFC)!= 
10)goto _LL95;_tmp10C=((struct Cyc_Absyn_AggrType_struct*)_tmpFC)->f1;_tmp10D=
_tmp10C.aggr_info;if((_tmp10D.KnownAggr).tag != 2)goto _LL95;_tmp10E=(struct Cyc_Absyn_Aggrdecl**)(
_tmp10D.KnownAggr).val;_tmp10F=*_tmp10E;_tmp110=*_tmp10F;_tmp111=_tmp110.impl;
_LL94: return _tmp111 == 0?(void*)0:(void*)1;_LL95: if(_tmpFC <= (void*)4)goto _LL97;
if(*((int*)_tmpFC)!= 11)goto _LL97;_LL96: goto _LL98;_LL97: if(_tmpFC <= (void*)4)
goto _LL99;if(*((int*)_tmpFC)!= 13)goto _LL99;_LL98: return(void*)1;_LL99: if(_tmpFC
<= (void*)4)goto _LL9B;if(*((int*)_tmpFC)!= 12)goto _LL9B;_tmp112=((struct Cyc_Absyn_EnumType_struct*)
_tmpFC)->f2;_LL9A: if(_tmp112->fields == 0)return(void*)0;else{return(void*)1;}
_LL9B: if(_tmpFC <= (void*)4)goto _LL9D;if(*((int*)_tmpFC)!= 4)goto _LL9D;_tmp113=((
struct Cyc_Absyn_PointerType_struct*)_tmpFC)->f1;_LL9C: {void*_tmp117=Cyc_Absyn_conref_def((
void*)((void*)0),(_tmp113.ptr_atts).bounds);_LLAE: if((int)_tmp117 != 0)goto _LLB0;
_LLAF: return(void*)1;_LLB0: if(_tmp117 <= (void*)1)goto _LLAD;if(*((int*)_tmp117)!= 
0)goto _LLAD;_LLB1: return(void*)2;_LLAD:;}_LL9D: if(_tmpFC <= (void*)4)goto _LL9F;
if(*((int*)_tmpFC)!= 17)goto _LL9F;_LL9E: return(void*)7;_LL9F: if(_tmpFC <= (void*)
4)goto _LLA1;if(*((int*)_tmpFC)!= 18)goto _LLA1;_LLA0: return(void*)2;_LLA1: if(
_tmpFC <= (void*)4)goto _LLA3;if(*((int*)_tmpFC)!= 7)goto _LLA3;_LLA2: goto _LLA4;
_LLA3: if(_tmpFC <= (void*)4)goto _LLA5;if(*((int*)_tmpFC)!= 9)goto _LLA5;_LLA4:
return(void*)1;_LLA5: if(_tmpFC <= (void*)4)goto _LLA7;if(*((int*)_tmpFC)!= 16)goto
_LLA7;_tmp114=((struct Cyc_Absyn_TypedefType_struct*)_tmpFC)->f3;_LLA6: if(_tmp114
== 0  || _tmp114->kind == 0)({struct Cyc_String_pa_struct _tmp11A;_tmp11A.tag=0;
_tmp11A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));{
void*_tmp118[1]={& _tmp11A};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap))Cyc_Tcutil_impos)(({const char*_tmp119="typ_kind: typedef found: %s";
_tag_dyneither(_tmp119,sizeof(char),28);}),_tag_dyneither(_tmp118,sizeof(void*),
1));}});return(void*)((struct Cyc_Core_Opt*)_check_null(_tmp114->kind))->v;_LLA7:
if(_tmpFC <= (void*)4)goto _LLA9;if(*((int*)_tmpFC)!= 19)goto _LLA9;_LLA8: goto _LLAA;
_LLA9: if(_tmpFC <= (void*)4)goto _LLAB;if(*((int*)_tmpFC)!= 20)goto _LLAB;_LLAA:
goto _LLAC;_LLAB: if(_tmpFC <= (void*)4)goto _LL72;if(*((int*)_tmpFC)!= 21)goto _LL72;
_LLAC: return(void*)6;_LL72:;}int Cyc_Tcutil_unify(void*t1,void*t2){struct
_handler_cons _tmp11B;_push_handler(& _tmp11B);{int _tmp11D=0;if(setjmp(_tmp11B.handler))
_tmp11D=1;if(!_tmp11D){Cyc_Tcutil_unify_it(t1,t2);{int _tmp11E=1;_npop_handler(0);
return _tmp11E;};_pop_handler();}else{void*_tmp11C=(void*)_exn_thrown;void*
_tmp120=_tmp11C;_LLB3: if(_tmp120 != Cyc_Tcutil_Unify)goto _LLB5;_LLB4: return 0;
_LLB5:;_LLB6:(void)_throw(_tmp120);_LLB2:;}}}static void Cyc_Tcutil_occurslist(
void*evar,struct _RegionHandle*r,struct Cyc_List_List*env,struct Cyc_List_List*ts);
static void Cyc_Tcutil_occurs(void*evar,struct _RegionHandle*r,struct Cyc_List_List*
env,void*t){t=Cyc_Tcutil_compress(t);{void*_tmp121=t;struct Cyc_Absyn_Tvar*
_tmp122;struct Cyc_Core_Opt*_tmp123;struct Cyc_Core_Opt*_tmp124;struct Cyc_Core_Opt**
_tmp125;struct Cyc_Absyn_PtrInfo _tmp126;struct Cyc_Absyn_ArrayInfo _tmp127;void*
_tmp128;struct Cyc_Absyn_FnInfo _tmp129;struct Cyc_List_List*_tmp12A;struct Cyc_Core_Opt*
_tmp12B;void*_tmp12C;struct Cyc_List_List*_tmp12D;int _tmp12E;struct Cyc_Absyn_VarargInfo*
_tmp12F;struct Cyc_List_List*_tmp130;struct Cyc_List_List*_tmp131;struct Cyc_List_List*
_tmp132;struct Cyc_Absyn_DatatypeInfo _tmp133;struct Cyc_List_List*_tmp134;struct
Cyc_Core_Opt*_tmp135;struct Cyc_List_List*_tmp136;struct Cyc_Absyn_DatatypeFieldInfo
_tmp137;struct Cyc_List_List*_tmp138;struct Cyc_Absyn_AggrInfo _tmp139;struct Cyc_List_List*
_tmp13A;struct Cyc_List_List*_tmp13B;void*_tmp13C;void*_tmp13D;void*_tmp13E;void*
_tmp13F;struct Cyc_List_List*_tmp140;_LLB8: if(_tmp121 <= (void*)4)goto _LLD8;if(*((
int*)_tmp121)!= 1)goto _LLBA;_tmp122=((struct Cyc_Absyn_VarType_struct*)_tmp121)->f1;
_LLB9: if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,
env,_tmp122)){Cyc_Tcutil_failure_reason=({const char*_tmp141="(type variable would escape scope)";
_tag_dyneither(_tmp141,sizeof(char),35);});(int)_throw((void*)Cyc_Tcutil_Unify);}
goto _LLB7;_LLBA: if(*((int*)_tmp121)!= 0)goto _LLBC;_tmp123=((struct Cyc_Absyn_Evar_struct*)
_tmp121)->f2;_tmp124=((struct Cyc_Absyn_Evar_struct*)_tmp121)->f4;_tmp125=(struct
Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)_tmp121)->f4;_LLBB: if(t == evar){
Cyc_Tcutil_failure_reason=({const char*_tmp142="(occurs check)";_tag_dyneither(
_tmp142,sizeof(char),15);});(int)_throw((void*)Cyc_Tcutil_Unify);}else{if(
_tmp123 != 0)Cyc_Tcutil_occurs(evar,r,env,(void*)_tmp123->v);else{int problem=0;{
struct Cyc_List_List*s=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*
_tmp125))->v;for(0;s != 0;s=s->tl){if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,
struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(
Cyc_Tcutil_fast_tvar_cmp,env,(struct Cyc_Absyn_Tvar*)s->hd)){problem=1;break;}}}
if(problem){struct Cyc_List_List*_tmp143=0;{struct Cyc_List_List*s=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(*_tmp125))->v;for(0;s != 0;s=s->tl){if(((int(*)(
int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,
struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,(struct Cyc_Absyn_Tvar*)
s->hd))_tmp143=({struct Cyc_List_List*_tmp144=_cycalloc(sizeof(*_tmp144));_tmp144->hd=(
struct Cyc_Absyn_Tvar*)s->hd;_tmp144->tl=_tmp143;_tmp144;});}}*_tmp125=({struct
Cyc_Core_Opt*_tmp145=_cycalloc(sizeof(*_tmp145));_tmp145->v=_tmp143;_tmp145;});}}}
goto _LLB7;_LLBC: if(*((int*)_tmp121)!= 4)goto _LLBE;_tmp126=((struct Cyc_Absyn_PointerType_struct*)
_tmp121)->f1;_LLBD: Cyc_Tcutil_occurs(evar,r,env,_tmp126.elt_typ);Cyc_Tcutil_occurs(
evar,r,env,(_tmp126.ptr_atts).rgn);goto _LLB7;_LLBE: if(*((int*)_tmp121)!= 7)goto
_LLC0;_tmp127=((struct Cyc_Absyn_ArrayType_struct*)_tmp121)->f1;_tmp128=_tmp127.elt_type;
_LLBF: Cyc_Tcutil_occurs(evar,r,env,_tmp128);goto _LLB7;_LLC0: if(*((int*)_tmp121)
!= 8)goto _LLC2;_tmp129=((struct Cyc_Absyn_FnType_struct*)_tmp121)->f1;_tmp12A=
_tmp129.tvars;_tmp12B=_tmp129.effect;_tmp12C=_tmp129.ret_typ;_tmp12D=_tmp129.args;
_tmp12E=_tmp129.c_varargs;_tmp12F=_tmp129.cyc_varargs;_tmp130=_tmp129.rgn_po;
_tmp131=_tmp129.attributes;_LLC1: env=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(r,
_tmp12A,env);if(_tmp12B != 0)Cyc_Tcutil_occurs(evar,r,env,(void*)_tmp12B->v);Cyc_Tcutil_occurs(
evar,r,env,_tmp12C);for(0;_tmp12D != 0;_tmp12D=_tmp12D->tl){Cyc_Tcutil_occurs(
evar,r,env,(*((struct _tuple9*)_tmp12D->hd)).f3);}if(_tmp12F != 0)Cyc_Tcutil_occurs(
evar,r,env,_tmp12F->type);for(0;_tmp130 != 0;_tmp130=_tmp130->tl){struct _tuple0
_tmp147;void*_tmp148;void*_tmp149;struct _tuple0*_tmp146=(struct _tuple0*)_tmp130->hd;
_tmp147=*_tmp146;_tmp148=_tmp147.f1;_tmp149=_tmp147.f2;Cyc_Tcutil_occurs(evar,r,
env,_tmp148);Cyc_Tcutil_occurs(evar,r,env,_tmp149);}goto _LLB7;_LLC2: if(*((int*)
_tmp121)!= 9)goto _LLC4;_tmp132=((struct Cyc_Absyn_TupleType_struct*)_tmp121)->f1;
_LLC3: for(0;_tmp132 != 0;_tmp132=_tmp132->tl){Cyc_Tcutil_occurs(evar,r,env,(*((
struct _tuple11*)_tmp132->hd)).f2);}goto _LLB7;_LLC4: if(*((int*)_tmp121)!= 2)goto
_LLC6;_tmp133=((struct Cyc_Absyn_DatatypeType_struct*)_tmp121)->f1;_tmp134=
_tmp133.targs;_tmp135=_tmp133.rgn;_LLC5: if((unsigned int)_tmp135)Cyc_Tcutil_occurs(
evar,r,env,(void*)_tmp135->v);Cyc_Tcutil_occurslist(evar,r,env,_tmp134);goto
_LLB7;_LLC6: if(*((int*)_tmp121)!= 16)goto _LLC8;_tmp136=((struct Cyc_Absyn_TypedefType_struct*)
_tmp121)->f2;_LLC7: _tmp138=_tmp136;goto _LLC9;_LLC8: if(*((int*)_tmp121)!= 3)goto
_LLCA;_tmp137=((struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp121)->f1;_tmp138=
_tmp137.targs;_LLC9: _tmp13A=_tmp138;goto _LLCB;_LLCA: if(*((int*)_tmp121)!= 10)
goto _LLCC;_tmp139=((struct Cyc_Absyn_AggrType_struct*)_tmp121)->f1;_tmp13A=
_tmp139.targs;_LLCB: Cyc_Tcutil_occurslist(evar,r,env,_tmp13A);goto _LLB7;_LLCC:
if(*((int*)_tmp121)!= 11)goto _LLCE;_tmp13B=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp121)->f2;_LLCD: for(0;_tmp13B != 0;_tmp13B=_tmp13B->tl){Cyc_Tcutil_occurs(evar,
r,env,((struct Cyc_Absyn_Aggrfield*)_tmp13B->hd)->type);}goto _LLB7;_LLCE: if(*((
int*)_tmp121)!= 18)goto _LLD0;_tmp13C=(void*)((struct Cyc_Absyn_TagType_struct*)
_tmp121)->f1;_LLCF: _tmp13D=_tmp13C;goto _LLD1;_LLD0: if(*((int*)_tmp121)!= 19)goto
_LLD2;_tmp13D=(void*)((struct Cyc_Absyn_AccessEff_struct*)_tmp121)->f1;_LLD1:
_tmp13E=_tmp13D;goto _LLD3;_LLD2: if(*((int*)_tmp121)!= 14)goto _LLD4;_tmp13E=(void*)((
struct Cyc_Absyn_RgnHandleType_struct*)_tmp121)->f1;_LLD3: _tmp13F=_tmp13E;goto
_LLD5;_LLD4: if(*((int*)_tmp121)!= 21)goto _LLD6;_tmp13F=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp121)->f1;_LLD5: Cyc_Tcutil_occurs(evar,r,env,_tmp13F);goto _LLB7;_LLD6: if(*((
int*)_tmp121)!= 20)goto _LLD8;_tmp140=((struct Cyc_Absyn_JoinEff_struct*)_tmp121)->f1;
_LLD7: Cyc_Tcutil_occurslist(evar,r,env,_tmp140);goto _LLB7;_LLD8:;_LLD9: goto _LLB7;
_LLB7:;}}static void Cyc_Tcutil_occurslist(void*evar,struct _RegionHandle*r,struct
Cyc_List_List*env,struct Cyc_List_List*ts){for(0;ts != 0;ts=ts->tl){Cyc_Tcutil_occurs(
evar,r,env,(void*)ts->hd);}}static void Cyc_Tcutil_unify_list(struct Cyc_List_List*
t1,struct Cyc_List_List*t2){for(0;t1 != 0  && t2 != 0;(t1=t1->tl,t2=t2->tl)){Cyc_Tcutil_unify_it((
void*)t1->hd,(void*)t2->hd);}if(t1 != 0  || t2 != 0)(int)_throw((void*)Cyc_Tcutil_Unify);}
static void Cyc_Tcutil_unify_tqual(struct Cyc_Absyn_Tqual tq1,void*t1,struct Cyc_Absyn_Tqual
tq2,void*t2){if(tq1.print_const  && !tq1.real_const)({void*_tmp14A=0;((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmp14B="tq1 real_const not set.";_tag_dyneither(_tmp14B,sizeof(char),24);}),
_tag_dyneither(_tmp14A,sizeof(void*),0));});if(tq2.print_const  && !tq2.real_const)({
void*_tmp14C=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp14D="tq2 real_const not set.";_tag_dyneither(_tmp14D,sizeof(char),
24);}),_tag_dyneither(_tmp14C,sizeof(void*),0));});if((tq1.real_const != tq2.real_const
 || tq1.q_volatile != tq2.q_volatile) || tq1.q_restrict != tq2.q_restrict){Cyc_Tcutil_t1_failure=
t1;Cyc_Tcutil_t2_failure=t2;Cyc_Tcutil_tq1_const=tq1.real_const;Cyc_Tcutil_tq2_const=
tq2.real_const;Cyc_Tcutil_failure_reason=({const char*_tmp14E="(qualifiers don't match)";
_tag_dyneither(_tmp14E,sizeof(char),25);});(int)_throw((void*)Cyc_Tcutil_Unify);}
Cyc_Tcutil_tq1_const=0;Cyc_Tcutil_tq2_const=0;}int Cyc_Tcutil_equal_tqual(struct
Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2){return(tq1.real_const == tq2.real_const
 && tq1.q_volatile == tq2.q_volatile) && tq1.q_restrict == tq2.q_restrict;}static
void Cyc_Tcutil_unify_it_conrefs(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*
x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason){x=Cyc_Absyn_compress_conref(
x);y=Cyc_Absyn_compress_conref(y);if(x == y)return;{union Cyc_Absyn_Constraint*
_tmp14F=x;union Cyc_Absyn_Constraint _tmp150;int _tmp151;union Cyc_Absyn_Constraint
_tmp152;void*_tmp153;union Cyc_Absyn_Constraint _tmp154;union Cyc_Absyn_Constraint*
_tmp155;_LLDB: _tmp150=*_tmp14F;if((_tmp150.No_constr).tag != 3)goto _LLDD;_tmp151=(
int)(_tmp150.No_constr).val;_LLDC:*x=({union Cyc_Absyn_Constraint _tmp156;(_tmp156.Forward_constr).val=
y;(_tmp156.Forward_constr).tag=2;_tmp156;});return;_LLDD: _tmp152=*_tmp14F;if((
_tmp152.Eq_constr).tag != 1)goto _LLDF;_tmp153=(void*)(_tmp152.Eq_constr).val;
_LLDE: {union Cyc_Absyn_Constraint*_tmp157=y;union Cyc_Absyn_Constraint _tmp158;int
_tmp159;union Cyc_Absyn_Constraint _tmp15A;void*_tmp15B;union Cyc_Absyn_Constraint
_tmp15C;union Cyc_Absyn_Constraint*_tmp15D;_LLE2: _tmp158=*_tmp157;if((_tmp158.No_constr).tag
!= 3)goto _LLE4;_tmp159=(int)(_tmp158.No_constr).val;_LLE3:*y=*x;return;_LLE4:
_tmp15A=*_tmp157;if((_tmp15A.Eq_constr).tag != 1)goto _LLE6;_tmp15B=(void*)(
_tmp15A.Eq_constr).val;_LLE5: if(cmp(_tmp153,_tmp15B)!= 0){Cyc_Tcutil_failure_reason=
reason;(int)_throw((void*)Cyc_Tcutil_Unify);}return;_LLE6: _tmp15C=*_tmp157;if((
_tmp15C.Forward_constr).tag != 2)goto _LLE1;_tmp15D=(union Cyc_Absyn_Constraint*)(
_tmp15C.Forward_constr).val;_LLE7:({void*_tmp15E=0;Cyc_Tcutil_impos(({const char*
_tmp15F="unify_conref: forward after compress(2)";_tag_dyneither(_tmp15F,sizeof(
char),40);}),_tag_dyneither(_tmp15E,sizeof(void*),0));});_LLE1:;}_LLDF: _tmp154=*
_tmp14F;if((_tmp154.Forward_constr).tag != 2)goto _LLDA;_tmp155=(union Cyc_Absyn_Constraint*)(
_tmp154.Forward_constr).val;_LLE0:({void*_tmp160=0;Cyc_Tcutil_impos(({const char*
_tmp161="unify_conref: forward after compress";_tag_dyneither(_tmp161,sizeof(
char),37);}),_tag_dyneither(_tmp160,sizeof(void*),0));});_LLDA:;}}static int Cyc_Tcutil_unify_conrefs(
int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y){
struct _handler_cons _tmp162;_push_handler(& _tmp162);{int _tmp164=0;if(setjmp(
_tmp162.handler))_tmp164=1;if(!_tmp164){Cyc_Tcutil_unify_it_conrefs(cmp,x,y,(
struct _dyneither_ptr)_tag_dyneither(0,0,0));{int _tmp165=1;_npop_handler(0);
return _tmp165;};_pop_handler();}else{void*_tmp163=(void*)_exn_thrown;void*
_tmp167=_tmp163;_LLE9: if(_tmp167 != Cyc_Tcutil_Unify)goto _LLEB;_LLEA: return 0;
_LLEB:;_LLEC:(void)_throw(_tmp167);_LLE8:;}}}static int Cyc_Tcutil_boundscmp(void*
b1,void*b2){struct _tuple0 _tmp169=({struct _tuple0 _tmp168;_tmp168.f1=b1;_tmp168.f2=
b2;_tmp168;});void*_tmp16A;void*_tmp16B;void*_tmp16C;void*_tmp16D;void*_tmp16E;
struct Cyc_Absyn_Exp*_tmp16F;void*_tmp170;struct Cyc_Absyn_Exp*_tmp171;_LLEE:
_tmp16A=_tmp169.f1;if((int)_tmp16A != 0)goto _LLF0;_tmp16B=_tmp169.f2;if((int)
_tmp16B != 0)goto _LLF0;_LLEF: return 0;_LLF0: _tmp16C=_tmp169.f1;if((int)_tmp16C != 0)
goto _LLF2;_LLF1: return - 1;_LLF2: _tmp16D=_tmp169.f2;if((int)_tmp16D != 0)goto _LLF4;
_LLF3: return 1;_LLF4: _tmp16E=_tmp169.f1;if(_tmp16E <= (void*)1)goto _LLED;if(*((int*)
_tmp16E)!= 0)goto _LLED;_tmp16F=((struct Cyc_Absyn_Upper_b_struct*)_tmp16E)->f1;
_tmp170=_tmp169.f2;if(_tmp170 <= (void*)1)goto _LLED;if(*((int*)_tmp170)!= 0)goto
_LLED;_tmp171=((struct Cyc_Absyn_Upper_b_struct*)_tmp170)->f1;_LLF5: return Cyc_Evexp_const_exp_cmp(
_tmp16F,_tmp171);_LLED:;}static int Cyc_Tcutil_unify_it_bounds(void*b1,void*b2){
struct _tuple0 _tmp173=({struct _tuple0 _tmp172;_tmp172.f1=b1;_tmp172.f2=b2;_tmp172;});
void*_tmp174;void*_tmp175;void*_tmp176;void*_tmp177;void*_tmp178;struct Cyc_Absyn_Exp*
_tmp179;void*_tmp17A;struct Cyc_Absyn_Exp*_tmp17B;_LLF7: _tmp174=_tmp173.f1;if((
int)_tmp174 != 0)goto _LLF9;_tmp175=_tmp173.f2;if((int)_tmp175 != 0)goto _LLF9;_LLF8:
return 0;_LLF9: _tmp176=_tmp173.f1;if((int)_tmp176 != 0)goto _LLFB;_LLFA: return - 1;
_LLFB: _tmp177=_tmp173.f2;if((int)_tmp177 != 0)goto _LLFD;_LLFC: return 1;_LLFD:
_tmp178=_tmp173.f1;if(_tmp178 <= (void*)1)goto _LLF6;if(*((int*)_tmp178)!= 0)goto
_LLF6;_tmp179=((struct Cyc_Absyn_Upper_b_struct*)_tmp178)->f1;_tmp17A=_tmp173.f2;
if(_tmp17A <= (void*)1)goto _LLF6;if(*((int*)_tmp17A)!= 0)goto _LLF6;_tmp17B=((
struct Cyc_Absyn_Upper_b_struct*)_tmp17A)->f1;_LLFE: return Cyc_Evexp_const_exp_cmp(
_tmp179,_tmp17B);_LLF6:;}static int Cyc_Tcutil_attribute_case_number(void*att){
void*_tmp17C=att;_LL100: if(_tmp17C <= (void*)17)goto _LL102;if(*((int*)_tmp17C)!= 
0)goto _LL102;_LL101: return 0;_LL102: if((int)_tmp17C != 0)goto _LL104;_LL103: return 1;
_LL104: if((int)_tmp17C != 1)goto _LL106;_LL105: return 2;_LL106: if((int)_tmp17C != 2)
goto _LL108;_LL107: return 3;_LL108: if((int)_tmp17C != 3)goto _LL10A;_LL109: return 4;
_LL10A: if((int)_tmp17C != 4)goto _LL10C;_LL10B: return 5;_LL10C: if(_tmp17C <= (void*)
17)goto _LL10E;if(*((int*)_tmp17C)!= 1)goto _LL10E;_LL10D: return 6;_LL10E: if((int)
_tmp17C != 5)goto _LL110;_LL10F: return 7;_LL110: if(_tmp17C <= (void*)17)goto _LL112;
if(*((int*)_tmp17C)!= 2)goto _LL112;_LL111: return 8;_LL112: if((int)_tmp17C != 6)
goto _LL114;_LL113: return 9;_LL114: if((int)_tmp17C != 7)goto _LL116;_LL115: return 10;
_LL116: if((int)_tmp17C != 8)goto _LL118;_LL117: return 11;_LL118: if((int)_tmp17C != 9)
goto _LL11A;_LL119: return 12;_LL11A: if((int)_tmp17C != 10)goto _LL11C;_LL11B: return
13;_LL11C: if((int)_tmp17C != 11)goto _LL11E;_LL11D: return 14;_LL11E: if((int)_tmp17C
!= 12)goto _LL120;_LL11F: return 15;_LL120: if((int)_tmp17C != 13)goto _LL122;_LL121:
return 16;_LL122: if((int)_tmp17C != 14)goto _LL124;_LL123: return 17;_LL124: if((int)
_tmp17C != 15)goto _LL126;_LL125: return 18;_LL126: if(_tmp17C <= (void*)17)goto _LL12A;
if(*((int*)_tmp17C)!= 3)goto _LL128;_LL127: return 19;_LL128: if(*((int*)_tmp17C)!= 
4)goto _LL12A;_LL129: return 20;_LL12A:;_LL12B: return 21;_LLFF:;}static int Cyc_Tcutil_attribute_cmp(
void*att1,void*att2){struct _tuple0 _tmp17E=({struct _tuple0 _tmp17D;_tmp17D.f1=att1;
_tmp17D.f2=att2;_tmp17D;});void*_tmp17F;int _tmp180;void*_tmp181;int _tmp182;void*
_tmp183;int _tmp184;void*_tmp185;int _tmp186;void*_tmp187;int _tmp188;void*_tmp189;
int _tmp18A;void*_tmp18B;struct _dyneither_ptr _tmp18C;void*_tmp18D;struct
_dyneither_ptr _tmp18E;void*_tmp18F;void*_tmp190;int _tmp191;int _tmp192;void*
_tmp193;void*_tmp194;int _tmp195;int _tmp196;_LL12D: _tmp17F=_tmp17E.f1;if(_tmp17F
<= (void*)17)goto _LL12F;if(*((int*)_tmp17F)!= 0)goto _LL12F;_tmp180=((struct Cyc_Absyn_Regparm_att_struct*)
_tmp17F)->f1;_tmp181=_tmp17E.f2;if(_tmp181 <= (void*)17)goto _LL12F;if(*((int*)
_tmp181)!= 0)goto _LL12F;_tmp182=((struct Cyc_Absyn_Regparm_att_struct*)_tmp181)->f1;
_LL12E: _tmp184=_tmp180;_tmp186=_tmp182;goto _LL130;_LL12F: _tmp183=_tmp17E.f1;if(
_tmp183 <= (void*)17)goto _LL131;if(*((int*)_tmp183)!= 4)goto _LL131;_tmp184=((
struct Cyc_Absyn_Initializes_att_struct*)_tmp183)->f1;_tmp185=_tmp17E.f2;if(
_tmp185 <= (void*)17)goto _LL131;if(*((int*)_tmp185)!= 4)goto _LL131;_tmp186=((
struct Cyc_Absyn_Initializes_att_struct*)_tmp185)->f1;_LL130: _tmp188=_tmp184;
_tmp18A=_tmp186;goto _LL132;_LL131: _tmp187=_tmp17E.f1;if(_tmp187 <= (void*)17)goto
_LL133;if(*((int*)_tmp187)!= 1)goto _LL133;_tmp188=((struct Cyc_Absyn_Aligned_att_struct*)
_tmp187)->f1;_tmp189=_tmp17E.f2;if(_tmp189 <= (void*)17)goto _LL133;if(*((int*)
_tmp189)!= 1)goto _LL133;_tmp18A=((struct Cyc_Absyn_Aligned_att_struct*)_tmp189)->f1;
_LL132: return Cyc_Core_intcmp(_tmp188,_tmp18A);_LL133: _tmp18B=_tmp17E.f1;if(
_tmp18B <= (void*)17)goto _LL135;if(*((int*)_tmp18B)!= 2)goto _LL135;_tmp18C=((
struct Cyc_Absyn_Section_att_struct*)_tmp18B)->f1;_tmp18D=_tmp17E.f2;if(_tmp18D <= (
void*)17)goto _LL135;if(*((int*)_tmp18D)!= 2)goto _LL135;_tmp18E=((struct Cyc_Absyn_Section_att_struct*)
_tmp18D)->f1;_LL134: return Cyc_strcmp((struct _dyneither_ptr)_tmp18C,(struct
_dyneither_ptr)_tmp18E);_LL135: _tmp18F=_tmp17E.f1;if(_tmp18F <= (void*)17)goto
_LL137;if(*((int*)_tmp18F)!= 3)goto _LL137;_tmp190=(void*)((struct Cyc_Absyn_Format_att_struct*)
_tmp18F)->f1;_tmp191=((struct Cyc_Absyn_Format_att_struct*)_tmp18F)->f2;_tmp192=((
struct Cyc_Absyn_Format_att_struct*)_tmp18F)->f3;_tmp193=_tmp17E.f2;if(_tmp193 <= (
void*)17)goto _LL137;if(*((int*)_tmp193)!= 3)goto _LL137;_tmp194=(void*)((struct
Cyc_Absyn_Format_att_struct*)_tmp193)->f1;_tmp195=((struct Cyc_Absyn_Format_att_struct*)
_tmp193)->f2;_tmp196=((struct Cyc_Absyn_Format_att_struct*)_tmp193)->f3;_LL136: {
int _tmp197=Cyc_Core_intcmp((int)((unsigned int)_tmp190),(int)((unsigned int)
_tmp194));if(_tmp197 != 0)return _tmp197;{int _tmp198=Cyc_Core_intcmp(_tmp191,
_tmp195);if(_tmp198 != 0)return _tmp198;return Cyc_Core_intcmp(_tmp192,_tmp196);}}
_LL137:;_LL138: return Cyc_Core_intcmp(Cyc_Tcutil_attribute_case_number(att1),Cyc_Tcutil_attribute_case_number(
att2));_LL12C:;}static int Cyc_Tcutil_equal_att(void*a1,void*a2){return Cyc_Tcutil_attribute_cmp(
a1,a2)== 0;}int Cyc_Tcutil_same_atts(struct Cyc_List_List*a1,struct Cyc_List_List*
a2){{struct Cyc_List_List*a=a1;for(0;a != 0;a=a->tl){if(!Cyc_List_exists_c(Cyc_Tcutil_equal_att,(
void*)a->hd,a2))return 0;}}{struct Cyc_List_List*a=a2;for(0;a != 0;a=a->tl){if(!Cyc_List_exists_c(
Cyc_Tcutil_equal_att,(void*)a->hd,a1))return 0;}}return 1;}static void*Cyc_Tcutil_rgns_of(
void*t);static void*Cyc_Tcutil_rgns_of_field(struct Cyc_Absyn_Aggrfield*af){return
Cyc_Tcutil_rgns_of(af->type);}static struct _tuple14*Cyc_Tcutil_region_free_subst(
struct Cyc_Absyn_Tvar*tv){void*t;{void*_tmp199=Cyc_Tcutil_tvar_kind(tv);_LL13A:
if((int)_tmp199 != 4)goto _LL13C;_LL13B: t=(void*)3;goto _LL139;_LL13C: if((int)
_tmp199 != 3)goto _LL13E;_LL13D: t=(void*)2;goto _LL139;_LL13E: if((int)_tmp199 != 6)
goto _LL140;_LL13F: t=Cyc_Absyn_empty_effect;goto _LL139;_LL140: if((int)_tmp199 != 7)
goto _LL142;_LL141: t=(void*)({struct Cyc_Absyn_ValueofType_struct*_tmp19A=
_cycalloc(sizeof(*_tmp19A));_tmp19A[0]=({struct Cyc_Absyn_ValueofType_struct
_tmp19B;_tmp19B.tag=17;_tmp19B.f1=Cyc_Absyn_uint_exp(0,0);_tmp19B;});_tmp19A;});
goto _LL139;_LL142:;_LL143: t=Cyc_Absyn_sint_typ;goto _LL139;_LL139:;}return({
struct _tuple14*_tmp19C=_cycalloc(sizeof(*_tmp19C));_tmp19C->f1=tv;_tmp19C->f2=t;
_tmp19C;});}static void*Cyc_Tcutil_rgns_of(void*t){void*_tmp19D=Cyc_Tcutil_compress(
t);void*_tmp19E;void*_tmp19F;void*_tmp1A0;struct Cyc_Absyn_DatatypeInfo _tmp1A1;
struct Cyc_List_List*_tmp1A2;struct Cyc_Core_Opt*_tmp1A3;struct Cyc_Absyn_PtrInfo
_tmp1A4;void*_tmp1A5;struct Cyc_Absyn_PtrAtts _tmp1A6;void*_tmp1A7;struct Cyc_Absyn_ArrayInfo
_tmp1A8;void*_tmp1A9;struct Cyc_List_List*_tmp1AA;struct Cyc_Absyn_DatatypeFieldInfo
_tmp1AB;struct Cyc_List_List*_tmp1AC;struct Cyc_Absyn_AggrInfo _tmp1AD;struct Cyc_List_List*
_tmp1AE;struct Cyc_List_List*_tmp1AF;struct Cyc_Absyn_FnInfo _tmp1B0;struct Cyc_List_List*
_tmp1B1;struct Cyc_Core_Opt*_tmp1B2;void*_tmp1B3;struct Cyc_List_List*_tmp1B4;
struct Cyc_Absyn_VarargInfo*_tmp1B5;struct Cyc_List_List*_tmp1B6;void*_tmp1B7;
struct Cyc_List_List*_tmp1B8;_LL145: if((int)_tmp19D != 0)goto _LL147;_LL146: goto
_LL148;_LL147: if((int)_tmp19D != 1)goto _LL149;_LL148: goto _LL14A;_LL149: if(_tmp19D
<= (void*)4)goto _LL16D;if(*((int*)_tmp19D)!= 6)goto _LL14B;_LL14A: goto _LL14C;
_LL14B: if(*((int*)_tmp19D)!= 12)goto _LL14D;_LL14C: goto _LL14E;_LL14D: if(*((int*)
_tmp19D)!= 13)goto _LL14F;_LL14E: goto _LL150;_LL14F: if(*((int*)_tmp19D)!= 17)goto
_LL151;_LL150: goto _LL152;_LL151: if(*((int*)_tmp19D)!= 5)goto _LL153;_LL152: return
Cyc_Absyn_empty_effect;_LL153: if(*((int*)_tmp19D)!= 0)goto _LL155;_LL154: goto
_LL156;_LL155: if(*((int*)_tmp19D)!= 1)goto _LL157;_LL156: {void*_tmp1B9=Cyc_Tcutil_typ_kind(
t);_LL17A: if((int)_tmp1B9 != 3)goto _LL17C;_LL17B: goto _LL17D;_LL17C: if((int)
_tmp1B9 != 4)goto _LL17E;_LL17D: goto _LL17F;_LL17E: if((int)_tmp1B9 != 5)goto _LL180;
_LL17F: return(void*)({struct Cyc_Absyn_AccessEff_struct*_tmp1BA=_cycalloc(sizeof(*
_tmp1BA));_tmp1BA[0]=({struct Cyc_Absyn_AccessEff_struct _tmp1BB;_tmp1BB.tag=19;
_tmp1BB.f1=(void*)t;_tmp1BB;});_tmp1BA;});_LL180: if((int)_tmp1B9 != 6)goto _LL182;
_LL181: return t;_LL182: if((int)_tmp1B9 != 7)goto _LL184;_LL183: return Cyc_Absyn_empty_effect;
_LL184:;_LL185: return(void*)({struct Cyc_Absyn_RgnsEff_struct*_tmp1BC=_cycalloc(
sizeof(*_tmp1BC));_tmp1BC[0]=({struct Cyc_Absyn_RgnsEff_struct _tmp1BD;_tmp1BD.tag=
21;_tmp1BD.f1=(void*)t;_tmp1BD;});_tmp1BC;});_LL179:;}_LL157: if(*((int*)_tmp19D)
!= 14)goto _LL159;_tmp19E=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)_tmp19D)->f1;
_LL158: return(void*)({struct Cyc_Absyn_AccessEff_struct*_tmp1BE=_cycalloc(sizeof(*
_tmp1BE));_tmp1BE[0]=({struct Cyc_Absyn_AccessEff_struct _tmp1BF;_tmp1BF.tag=19;
_tmp1BF.f1=(void*)_tmp19E;_tmp1BF;});_tmp1BE;});_LL159: if(*((int*)_tmp19D)!= 15)
goto _LL15B;_tmp19F=(void*)((struct Cyc_Absyn_DynRgnType_struct*)_tmp19D)->f1;
_tmp1A0=(void*)((struct Cyc_Absyn_DynRgnType_struct*)_tmp19D)->f2;_LL15A: return(
void*)({struct Cyc_Absyn_AccessEff_struct*_tmp1C0=_cycalloc(sizeof(*_tmp1C0));
_tmp1C0[0]=({struct Cyc_Absyn_AccessEff_struct _tmp1C1;_tmp1C1.tag=19;_tmp1C1.f1=(
void*)_tmp1A0;_tmp1C1;});_tmp1C0;});_LL15B: if(*((int*)_tmp19D)!= 2)goto _LL15D;
_tmp1A1=((struct Cyc_Absyn_DatatypeType_struct*)_tmp19D)->f1;_tmp1A2=_tmp1A1.targs;
_tmp1A3=_tmp1A1.rgn;_LL15C: {struct Cyc_List_List*ts=Cyc_List_map(Cyc_Tcutil_rgns_of,
_tmp1A2);if((unsigned int)_tmp1A3)ts=({struct Cyc_List_List*_tmp1C2=_cycalloc(
sizeof(*_tmp1C2));_tmp1C2->hd=(void*)((void*)({struct Cyc_Absyn_AccessEff_struct*
_tmp1C3=_cycalloc(sizeof(*_tmp1C3));_tmp1C3[0]=({struct Cyc_Absyn_AccessEff_struct
_tmp1C4;_tmp1C4.tag=19;_tmp1C4.f1=(void*)((void*)_tmp1A3->v);_tmp1C4;});_tmp1C3;}));
_tmp1C2->tl=ts;_tmp1C2;});return Cyc_Tcutil_normalize_effect((void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp1C5=_cycalloc(sizeof(*_tmp1C5));_tmp1C5[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp1C6;_tmp1C6.tag=20;_tmp1C6.f1=ts;_tmp1C6;});_tmp1C5;}));}_LL15D: if(*((int*)
_tmp19D)!= 4)goto _LL15F;_tmp1A4=((struct Cyc_Absyn_PointerType_struct*)_tmp19D)->f1;
_tmp1A5=_tmp1A4.elt_typ;_tmp1A6=_tmp1A4.ptr_atts;_tmp1A7=_tmp1A6.rgn;_LL15E:
return Cyc_Tcutil_normalize_effect((void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp1C7=_cycalloc(sizeof(*_tmp1C7));_tmp1C7[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp1C8;_tmp1C8.tag=20;_tmp1C8.f1=({void*_tmp1C9[2];_tmp1C9[1]=Cyc_Tcutil_rgns_of(
_tmp1A5);_tmp1C9[0]=(void*)({struct Cyc_Absyn_AccessEff_struct*_tmp1CA=_cycalloc(
sizeof(*_tmp1CA));_tmp1CA[0]=({struct Cyc_Absyn_AccessEff_struct _tmp1CB;_tmp1CB.tag=
19;_tmp1CB.f1=(void*)_tmp1A7;_tmp1CB;});_tmp1CA;});Cyc_List_list(_tag_dyneither(
_tmp1C9,sizeof(void*),2));});_tmp1C8;});_tmp1C7;}));_LL15F: if(*((int*)_tmp19D)!= 
7)goto _LL161;_tmp1A8=((struct Cyc_Absyn_ArrayType_struct*)_tmp19D)->f1;_tmp1A9=
_tmp1A8.elt_type;_LL160: return Cyc_Tcutil_normalize_effect(Cyc_Tcutil_rgns_of(
_tmp1A9));_LL161: if(*((int*)_tmp19D)!= 9)goto _LL163;_tmp1AA=((struct Cyc_Absyn_TupleType_struct*)
_tmp19D)->f1;_LL162: {struct Cyc_List_List*_tmp1CC=0;for(0;_tmp1AA != 0;_tmp1AA=
_tmp1AA->tl){_tmp1CC=({struct Cyc_List_List*_tmp1CD=_cycalloc(sizeof(*_tmp1CD));
_tmp1CD->hd=(void*)(*((struct _tuple11*)_tmp1AA->hd)).f2;_tmp1CD->tl=_tmp1CC;
_tmp1CD;});}_tmp1AC=_tmp1CC;goto _LL164;}_LL163: if(*((int*)_tmp19D)!= 3)goto
_LL165;_tmp1AB=((struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp19D)->f1;_tmp1AC=
_tmp1AB.targs;_LL164: _tmp1AE=_tmp1AC;goto _LL166;_LL165: if(*((int*)_tmp19D)!= 10)
goto _LL167;_tmp1AD=((struct Cyc_Absyn_AggrType_struct*)_tmp19D)->f1;_tmp1AE=
_tmp1AD.targs;_LL166: return Cyc_Tcutil_normalize_effect((void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp1CE=_cycalloc(sizeof(*_tmp1CE));_tmp1CE[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp1CF;_tmp1CF.tag=20;_tmp1CF.f1=Cyc_List_map(Cyc_Tcutil_rgns_of,_tmp1AE);
_tmp1CF;});_tmp1CE;}));_LL167: if(*((int*)_tmp19D)!= 11)goto _LL169;_tmp1AF=((
struct Cyc_Absyn_AnonAggrType_struct*)_tmp19D)->f2;_LL168: return Cyc_Tcutil_normalize_effect((
void*)({struct Cyc_Absyn_JoinEff_struct*_tmp1D0=_cycalloc(sizeof(*_tmp1D0));
_tmp1D0[0]=({struct Cyc_Absyn_JoinEff_struct _tmp1D1;_tmp1D1.tag=20;_tmp1D1.f1=((
struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Tcutil_rgns_of_field,_tmp1AF);_tmp1D1;});_tmp1D0;}));_LL169:
if(*((int*)_tmp19D)!= 18)goto _LL16B;_LL16A: return Cyc_Absyn_empty_effect;_LL16B:
if(*((int*)_tmp19D)!= 8)goto _LL16D;_tmp1B0=((struct Cyc_Absyn_FnType_struct*)
_tmp19D)->f1;_tmp1B1=_tmp1B0.tvars;_tmp1B2=_tmp1B0.effect;_tmp1B3=_tmp1B0.ret_typ;
_tmp1B4=_tmp1B0.args;_tmp1B5=_tmp1B0.cyc_varargs;_tmp1B6=_tmp1B0.rgn_po;_LL16C: {
void*_tmp1D2=Cyc_Tcutil_substitute(((struct Cyc_List_List*(*)(struct _tuple14*(*f)(
struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_region_free_subst,
_tmp1B1),(void*)((struct Cyc_Core_Opt*)_check_null(_tmp1B2))->v);return Cyc_Tcutil_normalize_effect(
_tmp1D2);}_LL16D: if((int)_tmp19D != 3)goto _LL16F;_LL16E: goto _LL170;_LL16F: if((int)
_tmp19D != 2)goto _LL171;_LL170: return Cyc_Absyn_empty_effect;_LL171: if(_tmp19D <= (
void*)4)goto _LL173;if(*((int*)_tmp19D)!= 19)goto _LL173;_LL172: goto _LL174;_LL173:
if(_tmp19D <= (void*)4)goto _LL175;if(*((int*)_tmp19D)!= 20)goto _LL175;_LL174:
return t;_LL175: if(_tmp19D <= (void*)4)goto _LL177;if(*((int*)_tmp19D)!= 21)goto
_LL177;_tmp1B7=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp19D)->f1;_LL176:
return Cyc_Tcutil_rgns_of(_tmp1B7);_LL177: if(_tmp19D <= (void*)4)goto _LL144;if(*((
int*)_tmp19D)!= 16)goto _LL144;_tmp1B8=((struct Cyc_Absyn_TypedefType_struct*)
_tmp19D)->f2;_LL178: return Cyc_Tcutil_normalize_effect((void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp1D3=_cycalloc(sizeof(*_tmp1D3));_tmp1D3[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp1D4;_tmp1D4.tag=20;_tmp1D4.f1=Cyc_List_map(Cyc_Tcutil_rgns_of,_tmp1B8);
_tmp1D4;});_tmp1D3;}));_LL144:;}void*Cyc_Tcutil_normalize_effect(void*e){e=Cyc_Tcutil_compress(
e);{void*_tmp1D5=e;struct Cyc_List_List*_tmp1D6;struct Cyc_List_List**_tmp1D7;void*
_tmp1D8;_LL187: if(_tmp1D5 <= (void*)4)goto _LL18B;if(*((int*)_tmp1D5)!= 20)goto
_LL189;_tmp1D6=((struct Cyc_Absyn_JoinEff_struct*)_tmp1D5)->f1;_tmp1D7=(struct Cyc_List_List**)&((
struct Cyc_Absyn_JoinEff_struct*)_tmp1D5)->f1;_LL188: {int redo_join=0;{struct Cyc_List_List*
effs=*_tmp1D7;for(0;effs != 0;effs=effs->tl){void*_tmp1D9=(void*)effs->hd;effs->hd=(
void*)Cyc_Tcutil_compress(Cyc_Tcutil_normalize_effect(_tmp1D9));{void*_tmp1DA=(
void*)effs->hd;void*_tmp1DB;_LL18E: if(_tmp1DA <= (void*)4)goto _LL192;if(*((int*)
_tmp1DA)!= 20)goto _LL190;_LL18F: goto _LL191;_LL190: if(*((int*)_tmp1DA)!= 19)goto
_LL192;_tmp1DB=(void*)((struct Cyc_Absyn_AccessEff_struct*)_tmp1DA)->f1;if((int)
_tmp1DB != 2)goto _LL192;_LL191: redo_join=1;goto _LL18D;_LL192:;_LL193: goto _LL18D;
_LL18D:;}}}if(!redo_join)return e;{struct Cyc_List_List*effects=0;{struct Cyc_List_List*
effs=*_tmp1D7;for(0;effs != 0;effs=effs->tl){void*_tmp1DC=Cyc_Tcutil_compress((
void*)effs->hd);struct Cyc_List_List*_tmp1DD;void*_tmp1DE;_LL195: if(_tmp1DC <= (
void*)4)goto _LL199;if(*((int*)_tmp1DC)!= 20)goto _LL197;_tmp1DD=((struct Cyc_Absyn_JoinEff_struct*)
_tmp1DC)->f1;_LL196: effects=Cyc_List_revappend(_tmp1DD,effects);goto _LL194;
_LL197: if(*((int*)_tmp1DC)!= 19)goto _LL199;_tmp1DE=(void*)((struct Cyc_Absyn_AccessEff_struct*)
_tmp1DC)->f1;if((int)_tmp1DE != 2)goto _LL199;_LL198: goto _LL194;_LL199:;_LL19A:
effects=({struct Cyc_List_List*_tmp1DF=_cycalloc(sizeof(*_tmp1DF));_tmp1DF->hd=(
void*)_tmp1DC;_tmp1DF->tl=effects;_tmp1DF;});goto _LL194;_LL194:;}}*_tmp1D7=Cyc_List_imp_rev(
effects);return e;}}_LL189: if(*((int*)_tmp1D5)!= 21)goto _LL18B;_tmp1D8=(void*)((
struct Cyc_Absyn_RgnsEff_struct*)_tmp1D5)->f1;_LL18A: {void*_tmp1E0=Cyc_Tcutil_compress(
_tmp1D8);_LL19C: if(_tmp1E0 <= (void*)4)goto _LL1A0;if(*((int*)_tmp1E0)!= 0)goto
_LL19E;_LL19D: goto _LL19F;_LL19E: if(*((int*)_tmp1E0)!= 1)goto _LL1A0;_LL19F: return
e;_LL1A0:;_LL1A1: return Cyc_Tcutil_rgns_of(_tmp1D8);_LL19B:;}_LL18B:;_LL18C:
return e;_LL186:;}}static void*Cyc_Tcutil_dummy_fntype(void*eff){struct Cyc_Absyn_FnType_struct*
_tmp1E1=({struct Cyc_Absyn_FnType_struct*_tmp1E2=_cycalloc(sizeof(*_tmp1E2));
_tmp1E2[0]=({struct Cyc_Absyn_FnType_struct _tmp1E3;_tmp1E3.tag=8;_tmp1E3.f1=({
struct Cyc_Absyn_FnInfo _tmp1E4;_tmp1E4.tvars=0;_tmp1E4.effect=({struct Cyc_Core_Opt*
_tmp1E5=_cycalloc(sizeof(*_tmp1E5));_tmp1E5->v=(void*)eff;_tmp1E5;});_tmp1E4.ret_typ=(
void*)0;_tmp1E4.args=0;_tmp1E4.c_varargs=0;_tmp1E4.cyc_varargs=0;_tmp1E4.rgn_po=
0;_tmp1E4.attributes=0;_tmp1E4;});_tmp1E3;});_tmp1E2;});return Cyc_Absyn_atb_typ((
void*)_tmp1E1,(void*)2,Cyc_Absyn_empty_tqual(0),Cyc_Absyn_bounds_one,Cyc_Absyn_false_conref);}
int Cyc_Tcutil_region_in_effect(int constrain,void*r,void*e){r=Cyc_Tcutil_compress(
r);if(r == (void*)2  || r == (void*)3)return 1;{void*_tmp1E6=Cyc_Tcutil_compress(e);
void*_tmp1E7;struct Cyc_List_List*_tmp1E8;void*_tmp1E9;struct Cyc_Core_Opt*_tmp1EA;
struct Cyc_Core_Opt*_tmp1EB;struct Cyc_Core_Opt**_tmp1EC;struct Cyc_Core_Opt*
_tmp1ED;_LL1A3: if(_tmp1E6 <= (void*)4)goto _LL1AB;if(*((int*)_tmp1E6)!= 19)goto
_LL1A5;_tmp1E7=(void*)((struct Cyc_Absyn_AccessEff_struct*)_tmp1E6)->f1;_LL1A4:
if(constrain)return Cyc_Tcutil_unify(r,_tmp1E7);_tmp1E7=Cyc_Tcutil_compress(
_tmp1E7);if(r == _tmp1E7)return 1;{struct _tuple0 _tmp1EF=({struct _tuple0 _tmp1EE;
_tmp1EE.f1=r;_tmp1EE.f2=_tmp1E7;_tmp1EE;});void*_tmp1F0;struct Cyc_Absyn_Tvar*
_tmp1F1;void*_tmp1F2;struct Cyc_Absyn_Tvar*_tmp1F3;_LL1AE: _tmp1F0=_tmp1EF.f1;if(
_tmp1F0 <= (void*)4)goto _LL1B0;if(*((int*)_tmp1F0)!= 1)goto _LL1B0;_tmp1F1=((
struct Cyc_Absyn_VarType_struct*)_tmp1F0)->f1;_tmp1F2=_tmp1EF.f2;if(_tmp1F2 <= (
void*)4)goto _LL1B0;if(*((int*)_tmp1F2)!= 1)goto _LL1B0;_tmp1F3=((struct Cyc_Absyn_VarType_struct*)
_tmp1F2)->f1;_LL1AF: return Cyc_Absyn_tvar_cmp(_tmp1F1,_tmp1F3)== 0;_LL1B0:;_LL1B1:
return 0;_LL1AD:;}_LL1A5: if(*((int*)_tmp1E6)!= 20)goto _LL1A7;_tmp1E8=((struct Cyc_Absyn_JoinEff_struct*)
_tmp1E6)->f1;_LL1A6: for(0;_tmp1E8 != 0;_tmp1E8=_tmp1E8->tl){if(Cyc_Tcutil_region_in_effect(
constrain,r,(void*)_tmp1E8->hd))return 1;}return 0;_LL1A7: if(*((int*)_tmp1E6)!= 21)
goto _LL1A9;_tmp1E9=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp1E6)->f1;_LL1A8: {
void*_tmp1F4=Cyc_Tcutil_rgns_of(_tmp1E9);void*_tmp1F5;_LL1B3: if(_tmp1F4 <= (void*)
4)goto _LL1B5;if(*((int*)_tmp1F4)!= 21)goto _LL1B5;_tmp1F5=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp1F4)->f1;_LL1B4: if(!constrain)return 0;{void*_tmp1F6=Cyc_Tcutil_compress(
_tmp1F5);struct Cyc_Core_Opt*_tmp1F7;struct Cyc_Core_Opt*_tmp1F8;struct Cyc_Core_Opt**
_tmp1F9;struct Cyc_Core_Opt*_tmp1FA;_LL1B8: if(_tmp1F6 <= (void*)4)goto _LL1BA;if(*((
int*)_tmp1F6)!= 0)goto _LL1BA;_tmp1F7=((struct Cyc_Absyn_Evar_struct*)_tmp1F6)->f1;
_tmp1F8=((struct Cyc_Absyn_Evar_struct*)_tmp1F6)->f2;_tmp1F9=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Evar_struct*)_tmp1F6)->f2;_tmp1FA=((struct Cyc_Absyn_Evar_struct*)
_tmp1F6)->f4;_LL1B9: {void*_tmp1FB=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,
_tmp1FA);Cyc_Tcutil_occurs(_tmp1FB,Cyc_Core_heap_region,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp1FA))->v,r);{void*_tmp1FC=Cyc_Tcutil_dummy_fntype((
void*)({struct Cyc_Absyn_JoinEff_struct*_tmp1FE=_cycalloc(sizeof(*_tmp1FE));
_tmp1FE[0]=({struct Cyc_Absyn_JoinEff_struct _tmp1FF;_tmp1FF.tag=20;_tmp1FF.f1=({
void*_tmp200[2];_tmp200[1]=(void*)({struct Cyc_Absyn_AccessEff_struct*_tmp201=
_cycalloc(sizeof(*_tmp201));_tmp201[0]=({struct Cyc_Absyn_AccessEff_struct _tmp202;
_tmp202.tag=19;_tmp202.f1=(void*)r;_tmp202;});_tmp201;});_tmp200[0]=_tmp1FB;Cyc_List_list(
_tag_dyneither(_tmp200,sizeof(void*),2));});_tmp1FF;});_tmp1FE;}));*_tmp1F9=({
struct Cyc_Core_Opt*_tmp1FD=_cycalloc(sizeof(*_tmp1FD));_tmp1FD->v=(void*)_tmp1FC;
_tmp1FD;});return 1;}}_LL1BA:;_LL1BB: return 0;_LL1B7:;}_LL1B5:;_LL1B6: return Cyc_Tcutil_region_in_effect(
constrain,r,_tmp1F4);_LL1B2:;}_LL1A9: if(*((int*)_tmp1E6)!= 0)goto _LL1AB;_tmp1EA=((
struct Cyc_Absyn_Evar_struct*)_tmp1E6)->f1;_tmp1EB=((struct Cyc_Absyn_Evar_struct*)
_tmp1E6)->f2;_tmp1EC=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)
_tmp1E6)->f2;_tmp1ED=((struct Cyc_Absyn_Evar_struct*)_tmp1E6)->f4;_LL1AA: if(
_tmp1EA == 0  || (void*)_tmp1EA->v != (void*)6)({void*_tmp203=0;((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp204="effect evar has wrong kind";
_tag_dyneither(_tmp204,sizeof(char),27);}),_tag_dyneither(_tmp203,sizeof(void*),
0));});if(!constrain)return 0;{void*_tmp205=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_ek,_tmp1ED);Cyc_Tcutil_occurs(_tmp205,Cyc_Core_heap_region,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp1ED))->v,r);{struct Cyc_Absyn_JoinEff_struct*
_tmp206=({struct Cyc_Absyn_JoinEff_struct*_tmp208=_cycalloc(sizeof(*_tmp208));
_tmp208[0]=({struct Cyc_Absyn_JoinEff_struct _tmp209;_tmp209.tag=20;_tmp209.f1=({
struct Cyc_List_List*_tmp20A=_cycalloc(sizeof(*_tmp20A));_tmp20A->hd=(void*)
_tmp205;_tmp20A->tl=({struct Cyc_List_List*_tmp20B=_cycalloc(sizeof(*_tmp20B));
_tmp20B->hd=(void*)((void*)({struct Cyc_Absyn_AccessEff_struct*_tmp20C=_cycalloc(
sizeof(*_tmp20C));_tmp20C[0]=({struct Cyc_Absyn_AccessEff_struct _tmp20D;_tmp20D.tag=
19;_tmp20D.f1=(void*)r;_tmp20D;});_tmp20C;}));_tmp20B->tl=0;_tmp20B;});_tmp20A;});
_tmp209;});_tmp208;});*_tmp1EC=({struct Cyc_Core_Opt*_tmp207=_cycalloc(sizeof(*
_tmp207));_tmp207->v=(void*)((void*)_tmp206);_tmp207;});return 1;}}_LL1AB:;_LL1AC:
return 0;_LL1A2:;}}static int Cyc_Tcutil_type_in_effect(int may_constrain_evars,void*
t,void*e){t=Cyc_Tcutil_compress(t);{void*_tmp20E=Cyc_Tcutil_normalize_effect(Cyc_Tcutil_compress(
e));struct Cyc_List_List*_tmp20F;void*_tmp210;struct Cyc_Core_Opt*_tmp211;struct
Cyc_Core_Opt*_tmp212;struct Cyc_Core_Opt**_tmp213;struct Cyc_Core_Opt*_tmp214;
_LL1BD: if(_tmp20E <= (void*)4)goto _LL1C5;if(*((int*)_tmp20E)!= 19)goto _LL1BF;
_LL1BE: return 0;_LL1BF: if(*((int*)_tmp20E)!= 20)goto _LL1C1;_tmp20F=((struct Cyc_Absyn_JoinEff_struct*)
_tmp20E)->f1;_LL1C0: for(0;_tmp20F != 0;_tmp20F=_tmp20F->tl){if(Cyc_Tcutil_type_in_effect(
may_constrain_evars,t,(void*)_tmp20F->hd))return 1;}return 0;_LL1C1: if(*((int*)
_tmp20E)!= 21)goto _LL1C3;_tmp210=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp20E)->f1;_LL1C2: _tmp210=Cyc_Tcutil_compress(_tmp210);if(t == _tmp210)return 1;
if(may_constrain_evars)return Cyc_Tcutil_unify(t,_tmp210);{void*_tmp215=Cyc_Tcutil_rgns_of(
t);void*_tmp216;_LL1C8: if(_tmp215 <= (void*)4)goto _LL1CA;if(*((int*)_tmp215)!= 21)
goto _LL1CA;_tmp216=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp215)->f1;_LL1C9: {
struct _tuple0 _tmp218=({struct _tuple0 _tmp217;_tmp217.f1=Cyc_Tcutil_compress(
_tmp216);_tmp217.f2=_tmp210;_tmp217;});void*_tmp219;struct Cyc_Absyn_Tvar*_tmp21A;
void*_tmp21B;struct Cyc_Absyn_Tvar*_tmp21C;_LL1CD: _tmp219=_tmp218.f1;if(_tmp219 <= (
void*)4)goto _LL1CF;if(*((int*)_tmp219)!= 1)goto _LL1CF;_tmp21A=((struct Cyc_Absyn_VarType_struct*)
_tmp219)->f1;_tmp21B=_tmp218.f2;if(_tmp21B <= (void*)4)goto _LL1CF;if(*((int*)
_tmp21B)!= 1)goto _LL1CF;_tmp21C=((struct Cyc_Absyn_VarType_struct*)_tmp21B)->f1;
_LL1CE: return Cyc_Tcutil_unify(t,_tmp210);_LL1CF:;_LL1D0: return _tmp216 == _tmp210;
_LL1CC:;}_LL1CA:;_LL1CB: return Cyc_Tcutil_type_in_effect(may_constrain_evars,t,
_tmp215);_LL1C7:;}_LL1C3: if(*((int*)_tmp20E)!= 0)goto _LL1C5;_tmp211=((struct Cyc_Absyn_Evar_struct*)
_tmp20E)->f1;_tmp212=((struct Cyc_Absyn_Evar_struct*)_tmp20E)->f2;_tmp213=(struct
Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)_tmp20E)->f2;_tmp214=((struct Cyc_Absyn_Evar_struct*)
_tmp20E)->f4;_LL1C4: if(_tmp211 == 0  || (void*)_tmp211->v != (void*)6)({void*
_tmp21D=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp21E="effect evar has wrong kind";_tag_dyneither(_tmp21E,sizeof(
char),27);}),_tag_dyneither(_tmp21D,sizeof(void*),0));});if(!may_constrain_evars)
return 0;{void*_tmp21F=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,
_tmp214);Cyc_Tcutil_occurs(_tmp21F,Cyc_Core_heap_region,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp214))->v,t);{struct Cyc_Absyn_JoinEff_struct*
_tmp220=({struct Cyc_Absyn_JoinEff_struct*_tmp222=_cycalloc(sizeof(*_tmp222));
_tmp222[0]=({struct Cyc_Absyn_JoinEff_struct _tmp223;_tmp223.tag=20;_tmp223.f1=({
struct Cyc_List_List*_tmp224=_cycalloc(sizeof(*_tmp224));_tmp224->hd=(void*)
_tmp21F;_tmp224->tl=({struct Cyc_List_List*_tmp225=_cycalloc(sizeof(*_tmp225));
_tmp225->hd=(void*)((void*)({struct Cyc_Absyn_RgnsEff_struct*_tmp226=_cycalloc(
sizeof(*_tmp226));_tmp226[0]=({struct Cyc_Absyn_RgnsEff_struct _tmp227;_tmp227.tag=
21;_tmp227.f1=(void*)t;_tmp227;});_tmp226;}));_tmp225->tl=0;_tmp225;});_tmp224;});
_tmp223;});_tmp222;});*_tmp213=({struct Cyc_Core_Opt*_tmp221=_cycalloc(sizeof(*
_tmp221));_tmp221->v=(void*)((void*)_tmp220);_tmp221;});return 1;}}_LL1C5:;_LL1C6:
return 0;_LL1BC:;}}static int Cyc_Tcutil_variable_in_effect(int may_constrain_evars,
struct Cyc_Absyn_Tvar*v,void*e){e=Cyc_Tcutil_compress(e);{void*_tmp228=e;struct
Cyc_Absyn_Tvar*_tmp229;struct Cyc_List_List*_tmp22A;void*_tmp22B;struct Cyc_Core_Opt*
_tmp22C;struct Cyc_Core_Opt*_tmp22D;struct Cyc_Core_Opt**_tmp22E;struct Cyc_Core_Opt*
_tmp22F;_LL1D2: if(_tmp228 <= (void*)4)goto _LL1DA;if(*((int*)_tmp228)!= 1)goto
_LL1D4;_tmp229=((struct Cyc_Absyn_VarType_struct*)_tmp228)->f1;_LL1D3: return Cyc_Absyn_tvar_cmp(
v,_tmp229)== 0;_LL1D4: if(*((int*)_tmp228)!= 20)goto _LL1D6;_tmp22A=((struct Cyc_Absyn_JoinEff_struct*)
_tmp228)->f1;_LL1D5: for(0;_tmp22A != 0;_tmp22A=_tmp22A->tl){if(Cyc_Tcutil_variable_in_effect(
may_constrain_evars,v,(void*)_tmp22A->hd))return 1;}return 0;_LL1D6: if(*((int*)
_tmp228)!= 21)goto _LL1D8;_tmp22B=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp228)->f1;_LL1D7: {void*_tmp230=Cyc_Tcutil_rgns_of(_tmp22B);void*_tmp231;
_LL1DD: if(_tmp230 <= (void*)4)goto _LL1DF;if(*((int*)_tmp230)!= 21)goto _LL1DF;
_tmp231=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp230)->f1;_LL1DE: if(!
may_constrain_evars)return 0;{void*_tmp232=Cyc_Tcutil_compress(_tmp231);struct Cyc_Core_Opt*
_tmp233;struct Cyc_Core_Opt*_tmp234;struct Cyc_Core_Opt**_tmp235;struct Cyc_Core_Opt*
_tmp236;_LL1E2: if(_tmp232 <= (void*)4)goto _LL1E4;if(*((int*)_tmp232)!= 0)goto
_LL1E4;_tmp233=((struct Cyc_Absyn_Evar_struct*)_tmp232)->f1;_tmp234=((struct Cyc_Absyn_Evar_struct*)
_tmp232)->f2;_tmp235=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)
_tmp232)->f2;_tmp236=((struct Cyc_Absyn_Evar_struct*)_tmp232)->f4;_LL1E3: {void*
_tmp237=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,_tmp236);if(!((
int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*
l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp236))->v,v))return 0;{void*_tmp238=Cyc_Tcutil_dummy_fntype((
void*)({struct Cyc_Absyn_JoinEff_struct*_tmp23A=_cycalloc(sizeof(*_tmp23A));
_tmp23A[0]=({struct Cyc_Absyn_JoinEff_struct _tmp23B;_tmp23B.tag=20;_tmp23B.f1=({
void*_tmp23C[2];_tmp23C[1]=(void*)({struct Cyc_Absyn_VarType_struct*_tmp23D=
_cycalloc(sizeof(*_tmp23D));_tmp23D[0]=({struct Cyc_Absyn_VarType_struct _tmp23E;
_tmp23E.tag=1;_tmp23E.f1=v;_tmp23E;});_tmp23D;});_tmp23C[0]=_tmp237;Cyc_List_list(
_tag_dyneither(_tmp23C,sizeof(void*),2));});_tmp23B;});_tmp23A;}));*_tmp235=({
struct Cyc_Core_Opt*_tmp239=_cycalloc(sizeof(*_tmp239));_tmp239->v=(void*)_tmp238;
_tmp239;});return 1;}}_LL1E4:;_LL1E5: return 0;_LL1E1:;}_LL1DF:;_LL1E0: return Cyc_Tcutil_variable_in_effect(
may_constrain_evars,v,_tmp230);_LL1DC:;}_LL1D8: if(*((int*)_tmp228)!= 0)goto
_LL1DA;_tmp22C=((struct Cyc_Absyn_Evar_struct*)_tmp228)->f1;_tmp22D=((struct Cyc_Absyn_Evar_struct*)
_tmp228)->f2;_tmp22E=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)
_tmp228)->f2;_tmp22F=((struct Cyc_Absyn_Evar_struct*)_tmp228)->f4;_LL1D9: if(
_tmp22C == 0  || (void*)_tmp22C->v != (void*)6)({void*_tmp23F=0;((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp240="effect evar has wrong kind";
_tag_dyneither(_tmp240,sizeof(char),27);}),_tag_dyneither(_tmp23F,sizeof(void*),
0));});{void*_tmp241=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,
_tmp22F);if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp22F))->v,v))return 0;{
struct Cyc_Absyn_JoinEff_struct*_tmp242=({struct Cyc_Absyn_JoinEff_struct*_tmp244=
_cycalloc(sizeof(*_tmp244));_tmp244[0]=({struct Cyc_Absyn_JoinEff_struct _tmp245;
_tmp245.tag=20;_tmp245.f1=({struct Cyc_List_List*_tmp246=_cycalloc(sizeof(*
_tmp246));_tmp246->hd=(void*)_tmp241;_tmp246->tl=({struct Cyc_List_List*_tmp247=
_cycalloc(sizeof(*_tmp247));_tmp247->hd=(void*)((void*)({struct Cyc_Absyn_VarType_struct*
_tmp248=_cycalloc(sizeof(*_tmp248));_tmp248[0]=({struct Cyc_Absyn_VarType_struct
_tmp249;_tmp249.tag=1;_tmp249.f1=v;_tmp249;});_tmp248;}));_tmp247->tl=0;_tmp247;});
_tmp246;});_tmp245;});_tmp244;});*_tmp22E=({struct Cyc_Core_Opt*_tmp243=_cycalloc(
sizeof(*_tmp243));_tmp243->v=(void*)((void*)_tmp242);_tmp243;});return 1;}}_LL1DA:;
_LL1DB: return 0;_LL1D1:;}}static int Cyc_Tcutil_evar_in_effect(void*evar,void*e){e=
Cyc_Tcutil_compress(e);{void*_tmp24A=e;struct Cyc_List_List*_tmp24B;void*_tmp24C;
_LL1E7: if(_tmp24A <= (void*)4)goto _LL1ED;if(*((int*)_tmp24A)!= 20)goto _LL1E9;
_tmp24B=((struct Cyc_Absyn_JoinEff_struct*)_tmp24A)->f1;_LL1E8: for(0;_tmp24B != 0;
_tmp24B=_tmp24B->tl){if(Cyc_Tcutil_evar_in_effect(evar,(void*)_tmp24B->hd))
return 1;}return 0;_LL1E9: if(*((int*)_tmp24A)!= 21)goto _LL1EB;_tmp24C=(void*)((
struct Cyc_Absyn_RgnsEff_struct*)_tmp24A)->f1;_LL1EA: {void*_tmp24D=Cyc_Tcutil_rgns_of(
_tmp24C);void*_tmp24E;_LL1F0: if(_tmp24D <= (void*)4)goto _LL1F2;if(*((int*)_tmp24D)
!= 21)goto _LL1F2;_tmp24E=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp24D)->f1;
_LL1F1: return 0;_LL1F2:;_LL1F3: return Cyc_Tcutil_evar_in_effect(evar,_tmp24D);
_LL1EF:;}_LL1EB: if(*((int*)_tmp24A)!= 0)goto _LL1ED;_LL1EC: return evar == e;_LL1ED:;
_LL1EE: return 0;_LL1E6:;}}int Cyc_Tcutil_subset_effect(int may_constrain_evars,void*
e1,void*e2){void*_tmp24F=Cyc_Tcutil_compress(e1);struct Cyc_List_List*_tmp250;
void*_tmp251;struct Cyc_Absyn_Tvar*_tmp252;void*_tmp253;struct Cyc_Core_Opt*
_tmp254;struct Cyc_Core_Opt**_tmp255;struct Cyc_Core_Opt*_tmp256;_LL1F5: if(_tmp24F
<= (void*)4)goto _LL1FF;if(*((int*)_tmp24F)!= 20)goto _LL1F7;_tmp250=((struct Cyc_Absyn_JoinEff_struct*)
_tmp24F)->f1;_LL1F6: for(0;_tmp250 != 0;_tmp250=_tmp250->tl){if(!Cyc_Tcutil_subset_effect(
may_constrain_evars,(void*)_tmp250->hd,e2))return 0;}return 1;_LL1F7: if(*((int*)
_tmp24F)!= 19)goto _LL1F9;_tmp251=(void*)((struct Cyc_Absyn_AccessEff_struct*)
_tmp24F)->f1;_LL1F8: return Cyc_Tcutil_region_in_effect(0,_tmp251,e2) || 
may_constrain_evars  && Cyc_Tcutil_unify(_tmp251,(void*)2);_LL1F9: if(*((int*)
_tmp24F)!= 1)goto _LL1FB;_tmp252=((struct Cyc_Absyn_VarType_struct*)_tmp24F)->f1;
_LL1FA: return Cyc_Tcutil_variable_in_effect(may_constrain_evars,_tmp252,e2);
_LL1FB: if(*((int*)_tmp24F)!= 21)goto _LL1FD;_tmp253=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp24F)->f1;_LL1FC: {void*_tmp257=Cyc_Tcutil_rgns_of(_tmp253);void*_tmp258;
_LL202: if(_tmp257 <= (void*)4)goto _LL204;if(*((int*)_tmp257)!= 21)goto _LL204;
_tmp258=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp257)->f1;_LL203: return Cyc_Tcutil_type_in_effect(
may_constrain_evars,_tmp258,e2) || may_constrain_evars  && Cyc_Tcutil_unify(
_tmp258,Cyc_Absyn_sint_typ);_LL204:;_LL205: return Cyc_Tcutil_subset_effect(
may_constrain_evars,_tmp257,e2);_LL201:;}_LL1FD: if(*((int*)_tmp24F)!= 0)goto
_LL1FF;_tmp254=((struct Cyc_Absyn_Evar_struct*)_tmp24F)->f2;_tmp255=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Evar_struct*)_tmp24F)->f2;_tmp256=((struct Cyc_Absyn_Evar_struct*)
_tmp24F)->f4;_LL1FE: if(!Cyc_Tcutil_evar_in_effect(e1,e2))*_tmp255=({struct Cyc_Core_Opt*
_tmp259=_cycalloc(sizeof(*_tmp259));_tmp259->v=(void*)Cyc_Absyn_empty_effect;
_tmp259;});return 1;_LL1FF:;_LL200:({struct Cyc_String_pa_struct _tmp25C;_tmp25C.tag=
0;_tmp25C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
e1));{void*_tmp25A[1]={& _tmp25C};((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp25B="subset_effect: bad effect: %s";
_tag_dyneither(_tmp25B,sizeof(char),30);}),_tag_dyneither(_tmp25A,sizeof(void*),
1));}});_LL1F4:;}static int Cyc_Tcutil_unify_effect(void*e1,void*e2){e1=Cyc_Tcutil_normalize_effect(
e1);e2=Cyc_Tcutil_normalize_effect(e2);if(Cyc_Tcutil_subset_effect(0,e1,e2) && 
Cyc_Tcutil_subset_effect(0,e2,e1))return 1;if(Cyc_Tcutil_subset_effect(1,e1,e2)
 && Cyc_Tcutil_subset_effect(1,e2,e1))return 1;return 0;}static int Cyc_Tcutil_sub_rgnpo(
struct Cyc_List_List*rpo1,struct Cyc_List_List*rpo2){{struct Cyc_List_List*r1=rpo1;
for(0;r1 != 0;r1=r1->tl){struct _tuple0 _tmp25E;void*_tmp25F;void*_tmp260;struct
_tuple0*_tmp25D=(struct _tuple0*)r1->hd;_tmp25E=*_tmp25D;_tmp25F=_tmp25E.f1;
_tmp260=_tmp25E.f2;{int found=_tmp25F == (void*)2;{struct Cyc_List_List*r2=rpo2;
for(0;r2 != 0  && !found;r2=r2->tl){struct _tuple0 _tmp262;void*_tmp263;void*_tmp264;
struct _tuple0*_tmp261=(struct _tuple0*)r2->hd;_tmp262=*_tmp261;_tmp263=_tmp262.f1;
_tmp264=_tmp262.f2;if(Cyc_Tcutil_unify(_tmp25F,_tmp263) && Cyc_Tcutil_unify(
_tmp260,_tmp264)){found=1;break;}}}if(!found)return 0;}}}return 1;}static int Cyc_Tcutil_same_rgn_po(
struct Cyc_List_List*rpo1,struct Cyc_List_List*rpo2){return Cyc_Tcutil_sub_rgnpo(
rpo1,rpo2) && Cyc_Tcutil_sub_rgnpo(rpo2,rpo1);}struct _tuple17{struct Cyc_Absyn_VarargInfo*
f1;struct Cyc_Absyn_VarargInfo*f2;};struct _tuple18{struct Cyc_Core_Opt*f1;struct
Cyc_Core_Opt*f2;};void Cyc_Tcutil_unify_it(void*t1,void*t2){Cyc_Tcutil_t1_failure=
t1;Cyc_Tcutil_t2_failure=t2;Cyc_Tcutil_failure_reason=(struct _dyneither_ptr)
_tag_dyneither(0,0,0);t1=Cyc_Tcutil_compress(t1);t2=Cyc_Tcutil_compress(t2);if(
t1 == t2)return;{void*_tmp265=t1;struct Cyc_Core_Opt*_tmp266;struct Cyc_Core_Opt*
_tmp267;struct Cyc_Core_Opt**_tmp268;struct Cyc_Core_Opt*_tmp269;_LL207: if(_tmp265
<= (void*)4)goto _LL209;if(*((int*)_tmp265)!= 0)goto _LL209;_tmp266=((struct Cyc_Absyn_Evar_struct*)
_tmp265)->f1;_tmp267=((struct Cyc_Absyn_Evar_struct*)_tmp265)->f2;_tmp268=(struct
Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)_tmp265)->f2;_tmp269=((struct Cyc_Absyn_Evar_struct*)
_tmp265)->f4;_LL208: Cyc_Tcutil_occurs(t1,Cyc_Core_heap_region,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp269))->v,t2);{void*_tmp26A=Cyc_Tcutil_typ_kind(
t2);if(Cyc_Tcutil_kind_leq(_tmp26A,(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp266))->v)){*_tmp268=({struct Cyc_Core_Opt*_tmp26B=_cycalloc(sizeof(*_tmp26B));
_tmp26B->v=(void*)t2;_tmp26B;});return;}else{{void*_tmp26C=t2;struct Cyc_Core_Opt*
_tmp26D;struct Cyc_Core_Opt**_tmp26E;struct Cyc_Core_Opt*_tmp26F;struct Cyc_Absyn_PtrInfo
_tmp270;_LL20C: if(_tmp26C <= (void*)4)goto _LL210;if(*((int*)_tmp26C)!= 0)goto
_LL20E;_tmp26D=((struct Cyc_Absyn_Evar_struct*)_tmp26C)->f2;_tmp26E=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Evar_struct*)_tmp26C)->f2;_tmp26F=((struct Cyc_Absyn_Evar_struct*)
_tmp26C)->f4;_LL20D: {struct Cyc_List_List*_tmp271=(struct Cyc_List_List*)_tmp269->v;{
struct Cyc_List_List*s2=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(
_tmp26F))->v;for(0;s2 != 0;s2=s2->tl){if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,
struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(
Cyc_Tcutil_fast_tvar_cmp,_tmp271,(struct Cyc_Absyn_Tvar*)s2->hd)){Cyc_Tcutil_failure_reason=({
const char*_tmp272="(type variable would escape scope)";_tag_dyneither(_tmp272,
sizeof(char),35);});(int)_throw((void*)Cyc_Tcutil_Unify);}}}if(Cyc_Tcutil_kind_leq((
void*)_tmp266->v,_tmp26A)){*_tmp26E=({struct Cyc_Core_Opt*_tmp273=_cycalloc(
sizeof(*_tmp273));_tmp273->v=(void*)t1;_tmp273;});return;}Cyc_Tcutil_failure_reason=({
const char*_tmp274="(kinds are incompatible)";_tag_dyneither(_tmp274,sizeof(char),
25);});goto _LL20B;}_LL20E: if(*((int*)_tmp26C)!= 4)goto _LL210;_tmp270=((struct Cyc_Absyn_PointerType_struct*)
_tmp26C)->f1;if(!((void*)_tmp266->v == (void*)2))goto _LL210;_LL20F: {union Cyc_Absyn_Constraint*
_tmp275=Cyc_Absyn_compress_conref((_tmp270.ptr_atts).bounds);{union Cyc_Absyn_Constraint*
_tmp276=_tmp275;union Cyc_Absyn_Constraint _tmp277;int _tmp278;_LL213: _tmp277=*
_tmp276;if((_tmp277.No_constr).tag != 3)goto _LL215;_tmp278=(int)(_tmp277.No_constr).val;
_LL214:({struct _union_Constraint_Eq_constr*_tmp279=& _tmp275->Eq_constr;_tmp279->tag=
1;_tmp279->val=Cyc_Absyn_bounds_one;});*_tmp268=({struct Cyc_Core_Opt*_tmp27A=
_cycalloc(sizeof(*_tmp27A));_tmp27A->v=(void*)t2;_tmp27A;});return;_LL215:;
_LL216: goto _LL212;_LL212:;}goto _LL20B;}_LL210:;_LL211: goto _LL20B;_LL20B:;}Cyc_Tcutil_failure_reason=({
const char*_tmp27B="(kinds are incompatible)";_tag_dyneither(_tmp27B,sizeof(char),
25);});(int)_throw((void*)Cyc_Tcutil_Unify);}}_LL209:;_LL20A: goto _LL206;_LL206:;}{
struct _tuple0 _tmp27D=({struct _tuple0 _tmp27C;_tmp27C.f1=t2;_tmp27C.f2=t1;_tmp27C;});
void*_tmp27E;void*_tmp27F;void*_tmp280;void*_tmp281;struct Cyc_Absyn_Tvar*_tmp282;
void*_tmp283;struct Cyc_Absyn_Tvar*_tmp284;void*_tmp285;struct Cyc_Absyn_AggrInfo
_tmp286;union Cyc_Absyn_AggrInfoU _tmp287;struct Cyc_List_List*_tmp288;void*_tmp289;
struct Cyc_Absyn_AggrInfo _tmp28A;union Cyc_Absyn_AggrInfoU _tmp28B;struct Cyc_List_List*
_tmp28C;void*_tmp28D;struct _tuple2*_tmp28E;void*_tmp28F;struct _tuple2*_tmp290;
void*_tmp291;struct Cyc_List_List*_tmp292;void*_tmp293;struct Cyc_List_List*
_tmp294;void*_tmp295;struct Cyc_Absyn_DatatypeInfo _tmp296;union Cyc_Absyn_DatatypeInfoU
_tmp297;struct Cyc_Absyn_Datatypedecl**_tmp298;struct Cyc_Absyn_Datatypedecl*
_tmp299;struct Cyc_List_List*_tmp29A;struct Cyc_Core_Opt*_tmp29B;void*_tmp29C;
struct Cyc_Absyn_DatatypeInfo _tmp29D;union Cyc_Absyn_DatatypeInfoU _tmp29E;struct
Cyc_Absyn_Datatypedecl**_tmp29F;struct Cyc_Absyn_Datatypedecl*_tmp2A0;struct Cyc_List_List*
_tmp2A1;struct Cyc_Core_Opt*_tmp2A2;void*_tmp2A3;struct Cyc_Absyn_DatatypeFieldInfo
_tmp2A4;union Cyc_Absyn_DatatypeFieldInfoU _tmp2A5;struct _tuple3 _tmp2A6;struct Cyc_Absyn_Datatypedecl*
_tmp2A7;struct Cyc_Absyn_Datatypefield*_tmp2A8;struct Cyc_List_List*_tmp2A9;void*
_tmp2AA;struct Cyc_Absyn_DatatypeFieldInfo _tmp2AB;union Cyc_Absyn_DatatypeFieldInfoU
_tmp2AC;struct _tuple3 _tmp2AD;struct Cyc_Absyn_Datatypedecl*_tmp2AE;struct Cyc_Absyn_Datatypefield*
_tmp2AF;struct Cyc_List_List*_tmp2B0;void*_tmp2B1;struct Cyc_Absyn_PtrInfo _tmp2B2;
void*_tmp2B3;struct Cyc_Absyn_Tqual _tmp2B4;struct Cyc_Absyn_PtrAtts _tmp2B5;void*
_tmp2B6;union Cyc_Absyn_Constraint*_tmp2B7;union Cyc_Absyn_Constraint*_tmp2B8;
union Cyc_Absyn_Constraint*_tmp2B9;void*_tmp2BA;struct Cyc_Absyn_PtrInfo _tmp2BB;
void*_tmp2BC;struct Cyc_Absyn_Tqual _tmp2BD;struct Cyc_Absyn_PtrAtts _tmp2BE;void*
_tmp2BF;union Cyc_Absyn_Constraint*_tmp2C0;union Cyc_Absyn_Constraint*_tmp2C1;
union Cyc_Absyn_Constraint*_tmp2C2;void*_tmp2C3;void*_tmp2C4;void*_tmp2C5;void*
_tmp2C6;void*_tmp2C7;void*_tmp2C8;void*_tmp2C9;void*_tmp2CA;void*_tmp2CB;int
_tmp2CC;void*_tmp2CD;int _tmp2CE;void*_tmp2CF;void*_tmp2D0;void*_tmp2D1;void*
_tmp2D2;void*_tmp2D3;struct Cyc_Absyn_Exp*_tmp2D4;void*_tmp2D5;struct Cyc_Absyn_Exp*
_tmp2D6;void*_tmp2D7;struct Cyc_Absyn_ArrayInfo _tmp2D8;void*_tmp2D9;struct Cyc_Absyn_Tqual
_tmp2DA;struct Cyc_Absyn_Exp*_tmp2DB;union Cyc_Absyn_Constraint*_tmp2DC;void*
_tmp2DD;struct Cyc_Absyn_ArrayInfo _tmp2DE;void*_tmp2DF;struct Cyc_Absyn_Tqual
_tmp2E0;struct Cyc_Absyn_Exp*_tmp2E1;union Cyc_Absyn_Constraint*_tmp2E2;void*
_tmp2E3;struct Cyc_Absyn_FnInfo _tmp2E4;struct Cyc_List_List*_tmp2E5;struct Cyc_Core_Opt*
_tmp2E6;void*_tmp2E7;struct Cyc_List_List*_tmp2E8;int _tmp2E9;struct Cyc_Absyn_VarargInfo*
_tmp2EA;struct Cyc_List_List*_tmp2EB;struct Cyc_List_List*_tmp2EC;void*_tmp2ED;
struct Cyc_Absyn_FnInfo _tmp2EE;struct Cyc_List_List*_tmp2EF;struct Cyc_Core_Opt*
_tmp2F0;void*_tmp2F1;struct Cyc_List_List*_tmp2F2;int _tmp2F3;struct Cyc_Absyn_VarargInfo*
_tmp2F4;struct Cyc_List_List*_tmp2F5;struct Cyc_List_List*_tmp2F6;void*_tmp2F7;
struct Cyc_List_List*_tmp2F8;void*_tmp2F9;struct Cyc_List_List*_tmp2FA;void*
_tmp2FB;void*_tmp2FC;struct Cyc_List_List*_tmp2FD;void*_tmp2FE;void*_tmp2FF;
struct Cyc_List_List*_tmp300;void*_tmp301;void*_tmp302;void*_tmp303;void*_tmp304;
void*_tmp305;void*_tmp306;void*_tmp307;void*_tmp308;void*_tmp309;void*_tmp30A;
void*_tmp30B;void*_tmp30C;void*_tmp30D;void*_tmp30E;void*_tmp30F;void*_tmp310;
void*_tmp311;void*_tmp312;void*_tmp313;void*_tmp314;_LL218: _tmp27E=_tmp27D.f1;
if(_tmp27E <= (void*)4)goto _LL21A;if(*((int*)_tmp27E)!= 0)goto _LL21A;_LL219: Cyc_Tcutil_unify_it(
t2,t1);return;_LL21A: _tmp27F=_tmp27D.f1;if((int)_tmp27F != 0)goto _LL21C;_tmp280=
_tmp27D.f2;if((int)_tmp280 != 0)goto _LL21C;_LL21B: return;_LL21C: _tmp281=_tmp27D.f1;
if(_tmp281 <= (void*)4)goto _LL21E;if(*((int*)_tmp281)!= 1)goto _LL21E;_tmp282=((
struct Cyc_Absyn_VarType_struct*)_tmp281)->f1;_tmp283=_tmp27D.f2;if(_tmp283 <= (
void*)4)goto _LL21E;if(*((int*)_tmp283)!= 1)goto _LL21E;_tmp284=((struct Cyc_Absyn_VarType_struct*)
_tmp283)->f1;_LL21D: {struct _dyneither_ptr*_tmp315=_tmp282->name;struct
_dyneither_ptr*_tmp316=_tmp284->name;int _tmp317=_tmp282->identity;int _tmp318=
_tmp284->identity;void*_tmp319=Cyc_Tcutil_tvar_kind(_tmp282);void*_tmp31A=Cyc_Tcutil_tvar_kind(
_tmp284);if(_tmp318 == _tmp317  && Cyc_strptrcmp(_tmp315,_tmp316)== 0){if(_tmp319
!= _tmp31A)({struct Cyc_String_pa_struct _tmp31F;_tmp31F.tag=0;_tmp31F.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(_tmp31A));{struct
Cyc_String_pa_struct _tmp31E;_tmp31E.tag=0;_tmp31E.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_kind2string(_tmp319));{struct Cyc_String_pa_struct
_tmp31D;_tmp31D.tag=0;_tmp31D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*
_tmp315);{void*_tmp31B[3]={& _tmp31D,& _tmp31E,& _tmp31F};((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp31C="same type variable %s has kinds %s and %s";
_tag_dyneither(_tmp31C,sizeof(char),42);}),_tag_dyneither(_tmp31B,sizeof(void*),
3));}}}});return;}Cyc_Tcutil_failure_reason=({const char*_tmp320="(variable types are not the same)";
_tag_dyneither(_tmp320,sizeof(char),34);});goto _LL217;}_LL21E: _tmp285=_tmp27D.f1;
if(_tmp285 <= (void*)4)goto _LL220;if(*((int*)_tmp285)!= 10)goto _LL220;_tmp286=((
struct Cyc_Absyn_AggrType_struct*)_tmp285)->f1;_tmp287=_tmp286.aggr_info;_tmp288=
_tmp286.targs;_tmp289=_tmp27D.f2;if(_tmp289 <= (void*)4)goto _LL220;if(*((int*)
_tmp289)!= 10)goto _LL220;_tmp28A=((struct Cyc_Absyn_AggrType_struct*)_tmp289)->f1;
_tmp28B=_tmp28A.aggr_info;_tmp28C=_tmp28A.targs;_LL21F: {void*_tmp322;struct
_tuple2*_tmp323;struct _tuple12 _tmp321=Cyc_Absyn_aggr_kinded_name(_tmp28B);
_tmp322=_tmp321.f1;_tmp323=_tmp321.f2;{void*_tmp325;struct _tuple2*_tmp326;struct
_tuple12 _tmp324=Cyc_Absyn_aggr_kinded_name(_tmp287);_tmp325=_tmp324.f1;_tmp326=
_tmp324.f2;if(_tmp322 != _tmp325){Cyc_Tcutil_failure_reason=({const char*_tmp327="(struct and union type)";
_tag_dyneither(_tmp327,sizeof(char),24);});goto _LL217;}if(Cyc_Absyn_qvar_cmp(
_tmp323,_tmp326)!= 0){Cyc_Tcutil_failure_reason=({const char*_tmp328="(different type name)";
_tag_dyneither(_tmp328,sizeof(char),22);});goto _LL217;}Cyc_Tcutil_unify_list(
_tmp28C,_tmp288);return;}}_LL220: _tmp28D=_tmp27D.f1;if(_tmp28D <= (void*)4)goto
_LL222;if(*((int*)_tmp28D)!= 12)goto _LL222;_tmp28E=((struct Cyc_Absyn_EnumType_struct*)
_tmp28D)->f1;_tmp28F=_tmp27D.f2;if(_tmp28F <= (void*)4)goto _LL222;if(*((int*)
_tmp28F)!= 12)goto _LL222;_tmp290=((struct Cyc_Absyn_EnumType_struct*)_tmp28F)->f1;
_LL221: if(Cyc_Absyn_qvar_cmp(_tmp28E,_tmp290)== 0)return;Cyc_Tcutil_failure_reason=({
const char*_tmp329="(different enum types)";_tag_dyneither(_tmp329,sizeof(char),
23);});goto _LL217;_LL222: _tmp291=_tmp27D.f1;if(_tmp291 <= (void*)4)goto _LL224;if(*((
int*)_tmp291)!= 13)goto _LL224;_tmp292=((struct Cyc_Absyn_AnonEnumType_struct*)
_tmp291)->f1;_tmp293=_tmp27D.f2;if(_tmp293 <= (void*)4)goto _LL224;if(*((int*)
_tmp293)!= 13)goto _LL224;_tmp294=((struct Cyc_Absyn_AnonEnumType_struct*)_tmp293)->f1;
_LL223: {int bad=0;for(0;_tmp292 != 0  && _tmp294 != 0;(_tmp292=_tmp292->tl,_tmp294=
_tmp294->tl)){struct Cyc_Absyn_Enumfield*_tmp32A=(struct Cyc_Absyn_Enumfield*)
_tmp292->hd;struct Cyc_Absyn_Enumfield*_tmp32B=(struct Cyc_Absyn_Enumfield*)
_tmp294->hd;if(Cyc_Absyn_qvar_cmp(_tmp32A->name,_tmp32B->name)!= 0){Cyc_Tcutil_failure_reason=({
const char*_tmp32C="(different names for enum fields)";_tag_dyneither(_tmp32C,
sizeof(char),34);});bad=1;break;}if(_tmp32A->tag == _tmp32B->tag)continue;if(
_tmp32A->tag == 0  || _tmp32B->tag == 0){Cyc_Tcutil_failure_reason=({const char*
_tmp32D="(different tag values for enum fields)";_tag_dyneither(_tmp32D,sizeof(
char),39);});bad=1;break;}if(!Cyc_Evexp_same_const_exp((struct Cyc_Absyn_Exp*)
_check_null(_tmp32A->tag),(struct Cyc_Absyn_Exp*)_check_null(_tmp32B->tag))){Cyc_Tcutil_failure_reason=({
const char*_tmp32E="(different tag values for enum fields)";_tag_dyneither(
_tmp32E,sizeof(char),39);});bad=1;break;}}if(bad)goto _LL217;if(_tmp292 == 0  && 
_tmp294 == 0)return;Cyc_Tcutil_failure_reason=({const char*_tmp32F="(different number of fields for enums)";
_tag_dyneither(_tmp32F,sizeof(char),39);});goto _LL217;}_LL224: _tmp295=_tmp27D.f1;
if(_tmp295 <= (void*)4)goto _LL226;if(*((int*)_tmp295)!= 2)goto _LL226;_tmp296=((
struct Cyc_Absyn_DatatypeType_struct*)_tmp295)->f1;_tmp297=_tmp296.datatype_info;
if((_tmp297.KnownDatatype).tag != 2)goto _LL226;_tmp298=(struct Cyc_Absyn_Datatypedecl**)(
_tmp297.KnownDatatype).val;_tmp299=*_tmp298;_tmp29A=_tmp296.targs;_tmp29B=
_tmp296.rgn;_tmp29C=_tmp27D.f2;if(_tmp29C <= (void*)4)goto _LL226;if(*((int*)
_tmp29C)!= 2)goto _LL226;_tmp29D=((struct Cyc_Absyn_DatatypeType_struct*)_tmp29C)->f1;
_tmp29E=_tmp29D.datatype_info;if((_tmp29E.KnownDatatype).tag != 2)goto _LL226;
_tmp29F=(struct Cyc_Absyn_Datatypedecl**)(_tmp29E.KnownDatatype).val;_tmp2A0=*
_tmp29F;_tmp2A1=_tmp29D.targs;_tmp2A2=_tmp29D.rgn;_LL225: if(_tmp299 == _tmp2A0
 || Cyc_Absyn_qvar_cmp(_tmp299->name,_tmp2A0->name)== 0){if(_tmp2A2 != 0  && 
_tmp29B != 0)Cyc_Tcutil_unify_it((void*)_tmp2A2->v,(void*)_tmp29B->v);else{if(
_tmp2A2 != 0  || _tmp29B != 0)goto Datatype_fail;}Cyc_Tcutil_unify_list(_tmp2A1,
_tmp29A);return;}Datatype_fail: Cyc_Tcutil_failure_reason=({const char*_tmp330="(different datatype types)";
_tag_dyneither(_tmp330,sizeof(char),27);});goto _LL217;_LL226: _tmp2A3=_tmp27D.f1;
if(_tmp2A3 <= (void*)4)goto _LL228;if(*((int*)_tmp2A3)!= 3)goto _LL228;_tmp2A4=((
struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp2A3)->f1;_tmp2A5=_tmp2A4.field_info;
if((_tmp2A5.KnownDatatypefield).tag != 2)goto _LL228;_tmp2A6=(struct _tuple3)(
_tmp2A5.KnownDatatypefield).val;_tmp2A7=_tmp2A6.f1;_tmp2A8=_tmp2A6.f2;_tmp2A9=
_tmp2A4.targs;_tmp2AA=_tmp27D.f2;if(_tmp2AA <= (void*)4)goto _LL228;if(*((int*)
_tmp2AA)!= 3)goto _LL228;_tmp2AB=((struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp2AA)->f1;_tmp2AC=_tmp2AB.field_info;if((_tmp2AC.KnownDatatypefield).tag != 2)
goto _LL228;_tmp2AD=(struct _tuple3)(_tmp2AC.KnownDatatypefield).val;_tmp2AE=
_tmp2AD.f1;_tmp2AF=_tmp2AD.f2;_tmp2B0=_tmp2AB.targs;_LL227: if((_tmp2A7 == _tmp2AE
 || Cyc_Absyn_qvar_cmp(_tmp2A7->name,_tmp2AE->name)== 0) && (_tmp2A8 == _tmp2AF
 || Cyc_Absyn_qvar_cmp(_tmp2A8->name,_tmp2AF->name)== 0)){Cyc_Tcutil_unify_list(
_tmp2B0,_tmp2A9);return;}Cyc_Tcutil_failure_reason=({const char*_tmp331="(different datatype field types)";
_tag_dyneither(_tmp331,sizeof(char),33);});goto _LL217;_LL228: _tmp2B1=_tmp27D.f1;
if(_tmp2B1 <= (void*)4)goto _LL22A;if(*((int*)_tmp2B1)!= 4)goto _LL22A;_tmp2B2=((
struct Cyc_Absyn_PointerType_struct*)_tmp2B1)->f1;_tmp2B3=_tmp2B2.elt_typ;_tmp2B4=
_tmp2B2.elt_tq;_tmp2B5=_tmp2B2.ptr_atts;_tmp2B6=_tmp2B5.rgn;_tmp2B7=_tmp2B5.nullable;
_tmp2B8=_tmp2B5.bounds;_tmp2B9=_tmp2B5.zero_term;_tmp2BA=_tmp27D.f2;if(_tmp2BA <= (
void*)4)goto _LL22A;if(*((int*)_tmp2BA)!= 4)goto _LL22A;_tmp2BB=((struct Cyc_Absyn_PointerType_struct*)
_tmp2BA)->f1;_tmp2BC=_tmp2BB.elt_typ;_tmp2BD=_tmp2BB.elt_tq;_tmp2BE=_tmp2BB.ptr_atts;
_tmp2BF=_tmp2BE.rgn;_tmp2C0=_tmp2BE.nullable;_tmp2C1=_tmp2BE.bounds;_tmp2C2=
_tmp2BE.zero_term;_LL229: Cyc_Tcutil_unify_it(_tmp2BC,_tmp2B3);Cyc_Tcutil_unify_it(
_tmp2B6,_tmp2BF);Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;((void(*)(int(*
cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct
_dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,_tmp2C2,
_tmp2B9,({const char*_tmp332="(not both zero terminated)";_tag_dyneither(_tmp332,
sizeof(char),27);}));Cyc_Tcutil_unify_tqual(_tmp2BD,_tmp2BC,_tmp2B4,_tmp2B3);Cyc_Tcutil_unify_it_conrefs(
Cyc_Tcutil_unify_it_bounds,_tmp2C1,_tmp2B8,({const char*_tmp333="(different pointer bounds)";
_tag_dyneither(_tmp333,sizeof(char),27);}));{void*_tmp334=Cyc_Absyn_conref_def(
Cyc_Absyn_bounds_one,_tmp2C1);_LL253: if((int)_tmp334 != 0)goto _LL255;_LL254:
return;_LL255:;_LL256: goto _LL252;_LL252:;}((void(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*
x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(
Cyc_Core_intcmp,_tmp2C0,_tmp2B7,({const char*_tmp335="(incompatible pointer types)";
_tag_dyneither(_tmp335,sizeof(char),29);}));return;_LL22A: _tmp2C3=_tmp27D.f1;if(
_tmp2C3 <= (void*)4)goto _LL22C;if(*((int*)_tmp2C3)!= 5)goto _LL22C;_tmp2C4=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp2C3)->f1;_tmp2C5=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp2C3)->f2;_tmp2C6=_tmp27D.f2;if(_tmp2C6 <= (void*)4)goto _LL22C;if(*((int*)
_tmp2C6)!= 5)goto _LL22C;_tmp2C7=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp2C6)->f1;
_tmp2C8=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp2C6)->f2;_LL22B: if(_tmp2C7
== _tmp2C4  && ((_tmp2C8 == _tmp2C5  || _tmp2C8 == (void*)2  && _tmp2C5 == (void*)3)
 || _tmp2C8 == (void*)3  && _tmp2C5 == (void*)2))return;Cyc_Tcutil_failure_reason=({
const char*_tmp336="(different integral types)";_tag_dyneither(_tmp336,sizeof(
char),27);});goto _LL217;_LL22C: _tmp2C9=_tmp27D.f1;if((int)_tmp2C9 != 1)goto _LL22E;
_tmp2CA=_tmp27D.f2;if((int)_tmp2CA != 1)goto _LL22E;_LL22D: return;_LL22E: _tmp2CB=
_tmp27D.f1;if(_tmp2CB <= (void*)4)goto _LL230;if(*((int*)_tmp2CB)!= 6)goto _LL230;
_tmp2CC=((struct Cyc_Absyn_DoubleType_struct*)_tmp2CB)->f1;_tmp2CD=_tmp27D.f2;if(
_tmp2CD <= (void*)4)goto _LL230;if(*((int*)_tmp2CD)!= 6)goto _LL230;_tmp2CE=((
struct Cyc_Absyn_DoubleType_struct*)_tmp2CD)->f1;_LL22F: if(_tmp2CC == _tmp2CE)
return;goto _LL217;_LL230: _tmp2CF=_tmp27D.f1;if(_tmp2CF <= (void*)4)goto _LL232;if(*((
int*)_tmp2CF)!= 18)goto _LL232;_tmp2D0=(void*)((struct Cyc_Absyn_TagType_struct*)
_tmp2CF)->f1;_tmp2D1=_tmp27D.f2;if(_tmp2D1 <= (void*)4)goto _LL232;if(*((int*)
_tmp2D1)!= 18)goto _LL232;_tmp2D2=(void*)((struct Cyc_Absyn_TagType_struct*)
_tmp2D1)->f1;_LL231: Cyc_Tcutil_unify_it(_tmp2D0,_tmp2D2);return;_LL232: _tmp2D3=
_tmp27D.f1;if(_tmp2D3 <= (void*)4)goto _LL234;if(*((int*)_tmp2D3)!= 17)goto _LL234;
_tmp2D4=((struct Cyc_Absyn_ValueofType_struct*)_tmp2D3)->f1;_tmp2D5=_tmp27D.f2;
if(_tmp2D5 <= (void*)4)goto _LL234;if(*((int*)_tmp2D5)!= 17)goto _LL234;_tmp2D6=((
struct Cyc_Absyn_ValueofType_struct*)_tmp2D5)->f1;_LL233: if(!Cyc_Evexp_same_const_exp(
_tmp2D4,_tmp2D6)){Cyc_Tcutil_failure_reason=({const char*_tmp337="(cannot prove expressions are the same)";
_tag_dyneither(_tmp337,sizeof(char),40);});goto _LL217;}return;_LL234: _tmp2D7=
_tmp27D.f1;if(_tmp2D7 <= (void*)4)goto _LL236;if(*((int*)_tmp2D7)!= 7)goto _LL236;
_tmp2D8=((struct Cyc_Absyn_ArrayType_struct*)_tmp2D7)->f1;_tmp2D9=_tmp2D8.elt_type;
_tmp2DA=_tmp2D8.tq;_tmp2DB=_tmp2D8.num_elts;_tmp2DC=_tmp2D8.zero_term;_tmp2DD=
_tmp27D.f2;if(_tmp2DD <= (void*)4)goto _LL236;if(*((int*)_tmp2DD)!= 7)goto _LL236;
_tmp2DE=((struct Cyc_Absyn_ArrayType_struct*)_tmp2DD)->f1;_tmp2DF=_tmp2DE.elt_type;
_tmp2E0=_tmp2DE.tq;_tmp2E1=_tmp2DE.num_elts;_tmp2E2=_tmp2DE.zero_term;_LL235: Cyc_Tcutil_unify_it(
_tmp2DF,_tmp2D9);Cyc_Tcutil_unify_tqual(_tmp2E0,_tmp2DF,_tmp2DA,_tmp2D9);Cyc_Tcutil_t1_failure=
t1;Cyc_Tcutil_t2_failure=t2;((void(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*
x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(
Cyc_Core_intcmp,_tmp2DC,_tmp2E2,({const char*_tmp338="(not both zero terminated)";
_tag_dyneither(_tmp338,sizeof(char),27);}));if(_tmp2DB == _tmp2E1)return;if(
_tmp2DB == 0  || _tmp2E1 == 0)goto _LL217;if(Cyc_Evexp_same_const_exp((struct Cyc_Absyn_Exp*)
_tmp2DB,(struct Cyc_Absyn_Exp*)_tmp2E1))return;Cyc_Tcutil_failure_reason=({const
char*_tmp339="(different array sizes)";_tag_dyneither(_tmp339,sizeof(char),24);});
goto _LL217;_LL236: _tmp2E3=_tmp27D.f1;if(_tmp2E3 <= (void*)4)goto _LL238;if(*((int*)
_tmp2E3)!= 8)goto _LL238;_tmp2E4=((struct Cyc_Absyn_FnType_struct*)_tmp2E3)->f1;
_tmp2E5=_tmp2E4.tvars;_tmp2E6=_tmp2E4.effect;_tmp2E7=_tmp2E4.ret_typ;_tmp2E8=
_tmp2E4.args;_tmp2E9=_tmp2E4.c_varargs;_tmp2EA=_tmp2E4.cyc_varargs;_tmp2EB=
_tmp2E4.rgn_po;_tmp2EC=_tmp2E4.attributes;_tmp2ED=_tmp27D.f2;if(_tmp2ED <= (void*)
4)goto _LL238;if(*((int*)_tmp2ED)!= 8)goto _LL238;_tmp2EE=((struct Cyc_Absyn_FnType_struct*)
_tmp2ED)->f1;_tmp2EF=_tmp2EE.tvars;_tmp2F0=_tmp2EE.effect;_tmp2F1=_tmp2EE.ret_typ;
_tmp2F2=_tmp2EE.args;_tmp2F3=_tmp2EE.c_varargs;_tmp2F4=_tmp2EE.cyc_varargs;
_tmp2F5=_tmp2EE.rgn_po;_tmp2F6=_tmp2EE.attributes;_LL237: {int done=0;struct
_RegionHandle _tmp33A=_new_region("rgn");struct _RegionHandle*rgn=& _tmp33A;
_push_region(rgn);{struct Cyc_List_List*inst=0;while(_tmp2EF != 0){if(_tmp2E5 == 0){
Cyc_Tcutil_failure_reason=({const char*_tmp33B="(second function type has too few type variables)";
_tag_dyneither(_tmp33B,sizeof(char),50);});(int)_throw((void*)Cyc_Tcutil_Unify);}{
void*_tmp33C=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp2EF->hd);void*
_tmp33D=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp2E5->hd);if(_tmp33C != 
_tmp33D){Cyc_Tcutil_failure_reason=(struct _dyneither_ptr)({struct Cyc_String_pa_struct
_tmp342;_tmp342.tag=0;_tmp342.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kind2string(_tmp33D));{struct Cyc_String_pa_struct _tmp341;_tmp341.tag=
0;_tmp341.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(
_tmp33C));{struct Cyc_String_pa_struct _tmp340;_tmp340.tag=0;_tmp340.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_tvar2string((struct Cyc_Absyn_Tvar*)
_tmp2EF->hd));{void*_tmp33E[3]={& _tmp340,& _tmp341,& _tmp342};Cyc_aprintf(({const
char*_tmp33F="(type var %s has different kinds %s and %s)";_tag_dyneither(
_tmp33F,sizeof(char),44);}),_tag_dyneither(_tmp33E,sizeof(void*),3));}}}});(int)
_throw((void*)Cyc_Tcutil_Unify);}inst=({struct Cyc_List_List*_tmp343=
_region_malloc(rgn,sizeof(*_tmp343));_tmp343->hd=({struct _tuple14*_tmp344=
_region_malloc(rgn,sizeof(*_tmp344));_tmp344->f1=(struct Cyc_Absyn_Tvar*)_tmp2E5->hd;
_tmp344->f2=(void*)({struct Cyc_Absyn_VarType_struct*_tmp345=_cycalloc(sizeof(*
_tmp345));_tmp345[0]=({struct Cyc_Absyn_VarType_struct _tmp346;_tmp346.tag=1;
_tmp346.f1=(struct Cyc_Absyn_Tvar*)_tmp2EF->hd;_tmp346;});_tmp345;});_tmp344;});
_tmp343->tl=inst;_tmp343;});_tmp2EF=_tmp2EF->tl;_tmp2E5=_tmp2E5->tl;}}if(_tmp2E5
!= 0){Cyc_Tcutil_failure_reason=({const char*_tmp347="(second function type has too many type variables)";
_tag_dyneither(_tmp347,sizeof(char),51);});_npop_handler(0);goto _LL217;}if(inst
!= 0){Cyc_Tcutil_unify_it((void*)({struct Cyc_Absyn_FnType_struct*_tmp348=
_cycalloc(sizeof(*_tmp348));_tmp348[0]=({struct Cyc_Absyn_FnType_struct _tmp349;
_tmp349.tag=8;_tmp349.f1=({struct Cyc_Absyn_FnInfo _tmp34A;_tmp34A.tvars=0;_tmp34A.effect=
_tmp2F0;_tmp34A.ret_typ=_tmp2F1;_tmp34A.args=_tmp2F2;_tmp34A.c_varargs=_tmp2F3;
_tmp34A.cyc_varargs=_tmp2F4;_tmp34A.rgn_po=_tmp2F5;_tmp34A.attributes=_tmp2F6;
_tmp34A;});_tmp349;});_tmp348;}),Cyc_Tcutil_rsubstitute(rgn,inst,(void*)({struct
Cyc_Absyn_FnType_struct*_tmp34B=_cycalloc(sizeof(*_tmp34B));_tmp34B[0]=({struct
Cyc_Absyn_FnType_struct _tmp34C;_tmp34C.tag=8;_tmp34C.f1=({struct Cyc_Absyn_FnInfo
_tmp34D;_tmp34D.tvars=0;_tmp34D.effect=_tmp2E6;_tmp34D.ret_typ=_tmp2E7;_tmp34D.args=
_tmp2E8;_tmp34D.c_varargs=_tmp2E9;_tmp34D.cyc_varargs=_tmp2EA;_tmp34D.rgn_po=
_tmp2EB;_tmp34D.attributes=_tmp2EC;_tmp34D;});_tmp34C;});_tmp34B;})));done=1;}}
if(done){_npop_handler(0);return;}Cyc_Tcutil_unify_it(_tmp2F1,_tmp2E7);for(0;
_tmp2F2 != 0  && _tmp2E8 != 0;(_tmp2F2=_tmp2F2->tl,_tmp2E8=_tmp2E8->tl)){struct Cyc_Absyn_Tqual
_tmp34F;void*_tmp350;struct _tuple9 _tmp34E=*((struct _tuple9*)_tmp2F2->hd);_tmp34F=
_tmp34E.f2;_tmp350=_tmp34E.f3;{struct Cyc_Absyn_Tqual _tmp352;void*_tmp353;struct
_tuple9 _tmp351=*((struct _tuple9*)_tmp2E8->hd);_tmp352=_tmp351.f2;_tmp353=_tmp351.f3;
Cyc_Tcutil_unify_it(_tmp350,_tmp353);Cyc_Tcutil_unify_tqual(_tmp34F,_tmp350,
_tmp352,_tmp353);}}Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;if(_tmp2F2
!= 0  || _tmp2E8 != 0){Cyc_Tcutil_failure_reason=({const char*_tmp354="(function types have different number of arguments)";
_tag_dyneither(_tmp354,sizeof(char),52);});_npop_handler(0);goto _LL217;}if(
_tmp2F3 != _tmp2E9){Cyc_Tcutil_failure_reason=({const char*_tmp355="(only one function type takes C varargs)";
_tag_dyneither(_tmp355,sizeof(char),41);});_npop_handler(0);goto _LL217;}{int
bad_cyc_vararg=0;{struct _tuple17 _tmp357=({struct _tuple17 _tmp356;_tmp356.f1=
_tmp2F4;_tmp356.f2=_tmp2EA;_tmp356;});struct Cyc_Absyn_VarargInfo*_tmp358;struct
Cyc_Absyn_VarargInfo*_tmp359;struct Cyc_Absyn_VarargInfo*_tmp35A;struct Cyc_Absyn_VarargInfo*
_tmp35B;struct Cyc_Absyn_VarargInfo*_tmp35C;struct Cyc_Absyn_VarargInfo _tmp35D;
struct Cyc_Core_Opt*_tmp35E;struct Cyc_Absyn_Tqual _tmp35F;void*_tmp360;int _tmp361;
struct Cyc_Absyn_VarargInfo*_tmp362;struct Cyc_Absyn_VarargInfo _tmp363;struct Cyc_Core_Opt*
_tmp364;struct Cyc_Absyn_Tqual _tmp365;void*_tmp366;int _tmp367;_LL258: _tmp358=
_tmp357.f1;if(_tmp358 != 0)goto _LL25A;_tmp359=_tmp357.f2;if(_tmp359 != 0)goto
_LL25A;_LL259: goto _LL257;_LL25A: _tmp35A=_tmp357.f1;if(_tmp35A != 0)goto _LL25C;
_LL25B: goto _LL25D;_LL25C: _tmp35B=_tmp357.f2;if(_tmp35B != 0)goto _LL25E;_LL25D:
bad_cyc_vararg=1;Cyc_Tcutil_failure_reason=({const char*_tmp368="(only one function type takes varargs)";
_tag_dyneither(_tmp368,sizeof(char),39);});goto _LL257;_LL25E: _tmp35C=_tmp357.f1;
if(_tmp35C == 0)goto _LL257;_tmp35D=*_tmp35C;_tmp35E=_tmp35D.name;_tmp35F=_tmp35D.tq;
_tmp360=_tmp35D.type;_tmp361=_tmp35D.inject;_tmp362=_tmp357.f2;if(_tmp362 == 0)
goto _LL257;_tmp363=*_tmp362;_tmp364=_tmp363.name;_tmp365=_tmp363.tq;_tmp366=
_tmp363.type;_tmp367=_tmp363.inject;_LL25F: Cyc_Tcutil_unify_it(_tmp360,_tmp366);
Cyc_Tcutil_unify_tqual(_tmp35F,_tmp360,_tmp365,_tmp366);if(_tmp361 != _tmp367){
bad_cyc_vararg=1;Cyc_Tcutil_failure_reason=({const char*_tmp369="(only one function type injects varargs)";
_tag_dyneither(_tmp369,sizeof(char),41);});}goto _LL257;_LL257:;}if(
bad_cyc_vararg){_npop_handler(0);goto _LL217;}{int bad_effect=0;{struct _tuple18
_tmp36B=({struct _tuple18 _tmp36A;_tmp36A.f1=_tmp2F0;_tmp36A.f2=_tmp2E6;_tmp36A;});
struct Cyc_Core_Opt*_tmp36C;struct Cyc_Core_Opt*_tmp36D;struct Cyc_Core_Opt*_tmp36E;
struct Cyc_Core_Opt*_tmp36F;_LL261: _tmp36C=_tmp36B.f1;if(_tmp36C != 0)goto _LL263;
_tmp36D=_tmp36B.f2;if(_tmp36D != 0)goto _LL263;_LL262: goto _LL260;_LL263: _tmp36E=
_tmp36B.f1;if(_tmp36E != 0)goto _LL265;_LL264: goto _LL266;_LL265: _tmp36F=_tmp36B.f2;
if(_tmp36F != 0)goto _LL267;_LL266: bad_effect=1;goto _LL260;_LL267:;_LL268:
bad_effect=!Cyc_Tcutil_unify_effect((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp2F0))->v,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp2E6))->v);goto _LL260;
_LL260:;}Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;if(bad_effect){Cyc_Tcutil_failure_reason=({
const char*_tmp370="(function type effects do not unify)";_tag_dyneither(_tmp370,
sizeof(char),37);});_npop_handler(0);goto _LL217;}if(!Cyc_Tcutil_same_atts(
_tmp2EC,_tmp2F6)){Cyc_Tcutil_failure_reason=({const char*_tmp371="(function types have different attributes)";
_tag_dyneither(_tmp371,sizeof(char),43);});_npop_handler(0);goto _LL217;}if(!Cyc_Tcutil_same_rgn_po(
_tmp2EB,_tmp2F5)){Cyc_Tcutil_failure_reason=({const char*_tmp372="(function types have different region lifetime orderings)";
_tag_dyneither(_tmp372,sizeof(char),58);});_npop_handler(0);goto _LL217;}
_npop_handler(0);return;}};_pop_region(rgn);}_LL238: _tmp2F7=_tmp27D.f1;if(
_tmp2F7 <= (void*)4)goto _LL23A;if(*((int*)_tmp2F7)!= 9)goto _LL23A;_tmp2F8=((
struct Cyc_Absyn_TupleType_struct*)_tmp2F7)->f1;_tmp2F9=_tmp27D.f2;if(_tmp2F9 <= (
void*)4)goto _LL23A;if(*((int*)_tmp2F9)!= 9)goto _LL23A;_tmp2FA=((struct Cyc_Absyn_TupleType_struct*)
_tmp2F9)->f1;_LL239: for(0;_tmp2FA != 0  && _tmp2F8 != 0;(_tmp2FA=_tmp2FA->tl,
_tmp2F8=_tmp2F8->tl)){struct Cyc_Absyn_Tqual _tmp374;void*_tmp375;struct _tuple11
_tmp373=*((struct _tuple11*)_tmp2FA->hd);_tmp374=_tmp373.f1;_tmp375=_tmp373.f2;{
struct Cyc_Absyn_Tqual _tmp377;void*_tmp378;struct _tuple11 _tmp376=*((struct
_tuple11*)_tmp2F8->hd);_tmp377=_tmp376.f1;_tmp378=_tmp376.f2;Cyc_Tcutil_unify_it(
_tmp375,_tmp378);Cyc_Tcutil_unify_tqual(_tmp374,_tmp375,_tmp377,_tmp378);}}if(
_tmp2FA == 0  && _tmp2F8 == 0)return;Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=
t2;Cyc_Tcutil_failure_reason=({const char*_tmp379="(tuple types have different numbers of components)";
_tag_dyneither(_tmp379,sizeof(char),51);});goto _LL217;_LL23A: _tmp2FB=_tmp27D.f1;
if(_tmp2FB <= (void*)4)goto _LL23C;if(*((int*)_tmp2FB)!= 11)goto _LL23C;_tmp2FC=(
void*)((struct Cyc_Absyn_AnonAggrType_struct*)_tmp2FB)->f1;_tmp2FD=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp2FB)->f2;_tmp2FE=_tmp27D.f2;if(_tmp2FE <= (void*)4)goto _LL23C;if(*((int*)
_tmp2FE)!= 11)goto _LL23C;_tmp2FF=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp2FE)->f1;_tmp300=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp2FE)->f2;_LL23B:
if(_tmp2FF != _tmp2FC){Cyc_Tcutil_failure_reason=({const char*_tmp37A="(struct and union type)";
_tag_dyneither(_tmp37A,sizeof(char),24);});goto _LL217;}for(0;_tmp300 != 0  && 
_tmp2FD != 0;(_tmp300=_tmp300->tl,_tmp2FD=_tmp2FD->tl)){struct Cyc_Absyn_Aggrfield*
_tmp37B=(struct Cyc_Absyn_Aggrfield*)_tmp300->hd;struct Cyc_Absyn_Aggrfield*
_tmp37C=(struct Cyc_Absyn_Aggrfield*)_tmp2FD->hd;if(Cyc_strptrcmp(_tmp37B->name,
_tmp37C->name)!= 0){Cyc_Tcutil_failure_reason=({const char*_tmp37D="(different member names)";
_tag_dyneither(_tmp37D,sizeof(char),25);});(int)_throw((void*)Cyc_Tcutil_Unify);}
Cyc_Tcutil_unify_it(_tmp37B->type,_tmp37C->type);Cyc_Tcutil_unify_tqual(_tmp37B->tq,
_tmp37B->type,_tmp37C->tq,_tmp37C->type);if(!Cyc_Tcutil_same_atts(_tmp37B->attributes,
_tmp37C->attributes)){Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;Cyc_Tcutil_failure_reason=({
const char*_tmp37E="(different attributes on member)";_tag_dyneither(_tmp37E,
sizeof(char),33);});(int)_throw((void*)Cyc_Tcutil_Unify);}if((_tmp37B->width != 0
 && _tmp37C->width == 0  || _tmp37C->width != 0  && _tmp37B->width == 0) || (_tmp37B->width
!= 0  && _tmp37C->width != 0) && !Cyc_Evexp_same_const_exp((struct Cyc_Absyn_Exp*)
_check_null(_tmp37B->width),(struct Cyc_Absyn_Exp*)_check_null(_tmp37C->width))){
Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;Cyc_Tcutil_failure_reason=({
const char*_tmp37F="(different bitfield widths on member)";_tag_dyneither(_tmp37F,
sizeof(char),38);});(int)_throw((void*)Cyc_Tcutil_Unify);}}if(_tmp300 == 0  && 
_tmp2FD == 0)return;Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;Cyc_Tcutil_failure_reason=({
const char*_tmp380="(different number of members)";_tag_dyneither(_tmp380,sizeof(
char),30);});goto _LL217;_LL23C: _tmp301=_tmp27D.f1;if((int)_tmp301 != 2)goto _LL23E;
_tmp302=_tmp27D.f2;if((int)_tmp302 != 2)goto _LL23E;_LL23D: return;_LL23E: _tmp303=
_tmp27D.f1;if((int)_tmp303 != 3)goto _LL240;_tmp304=_tmp27D.f2;if((int)_tmp304 != 3)
goto _LL240;_LL23F: return;_LL240: _tmp305=_tmp27D.f1;if(_tmp305 <= (void*)4)goto
_LL242;if(*((int*)_tmp305)!= 14)goto _LL242;_tmp306=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp305)->f1;_tmp307=_tmp27D.f2;if(_tmp307 <= (void*)4)goto _LL242;if(*((int*)
_tmp307)!= 14)goto _LL242;_tmp308=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp307)->f1;_LL241: Cyc_Tcutil_unify_it(_tmp306,_tmp308);return;_LL242: _tmp309=
_tmp27D.f1;if(_tmp309 <= (void*)4)goto _LL244;if(*((int*)_tmp309)!= 15)goto _LL244;
_tmp30A=(void*)((struct Cyc_Absyn_DynRgnType_struct*)_tmp309)->f1;_tmp30B=(void*)((
struct Cyc_Absyn_DynRgnType_struct*)_tmp309)->f2;_tmp30C=_tmp27D.f2;if(_tmp30C <= (
void*)4)goto _LL244;if(*((int*)_tmp30C)!= 15)goto _LL244;_tmp30D=(void*)((struct
Cyc_Absyn_DynRgnType_struct*)_tmp30C)->f1;_tmp30E=(void*)((struct Cyc_Absyn_DynRgnType_struct*)
_tmp30C)->f2;_LL243: Cyc_Tcutil_unify_it(_tmp30A,_tmp30D);Cyc_Tcutil_unify_it(
_tmp30B,_tmp30E);return;_LL244: _tmp30F=_tmp27D.f1;if(_tmp30F <= (void*)4)goto
_LL246;if(*((int*)_tmp30F)!= 20)goto _LL246;_LL245: goto _LL247;_LL246: _tmp310=
_tmp27D.f2;if(_tmp310 <= (void*)4)goto _LL248;if(*((int*)_tmp310)!= 20)goto _LL248;
_LL247: goto _LL249;_LL248: _tmp311=_tmp27D.f1;if(_tmp311 <= (void*)4)goto _LL24A;if(*((
int*)_tmp311)!= 19)goto _LL24A;_LL249: goto _LL24B;_LL24A: _tmp312=_tmp27D.f1;if(
_tmp312 <= (void*)4)goto _LL24C;if(*((int*)_tmp312)!= 21)goto _LL24C;_LL24B: goto
_LL24D;_LL24C: _tmp313=_tmp27D.f2;if(_tmp313 <= (void*)4)goto _LL24E;if(*((int*)
_tmp313)!= 21)goto _LL24E;_LL24D: goto _LL24F;_LL24E: _tmp314=_tmp27D.f2;if(_tmp314
<= (void*)4)goto _LL250;if(*((int*)_tmp314)!= 19)goto _LL250;_LL24F: if(Cyc_Tcutil_unify_effect(
t1,t2))return;Cyc_Tcutil_failure_reason=({const char*_tmp381="(effects don't unify)";
_tag_dyneither(_tmp381,sizeof(char),22);});goto _LL217;_LL250:;_LL251: goto _LL217;
_LL217:;}(int)_throw((void*)Cyc_Tcutil_Unify);}int Cyc_Tcutil_star_cmp(int(*cmp)(
void*,void*),void*a1,void*a2){if(a1 == a2)return 0;if(a1 == 0  && a2 != 0)return - 1;
if(a1 != 0  && a2 == 0)return 1;return cmp((void*)_check_null(a1),(void*)_check_null(
a2));}static int Cyc_Tcutil_tqual_cmp(struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual
tq2){int _tmp382=(tq1.real_const + (tq1.q_volatile << 1))+ (tq1.q_restrict << 2);int
_tmp383=(tq2.real_const + (tq2.q_volatile << 1))+ (tq2.q_restrict << 2);return Cyc_Core_intcmp(
_tmp382,_tmp383);}static int Cyc_Tcutil_conrefs_cmp(int(*cmp)(void*,void*),union
Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y){x=Cyc_Absyn_compress_conref(
x);y=Cyc_Absyn_compress_conref(y);if(x == y)return 0;{union Cyc_Absyn_Constraint*
_tmp384=x;union Cyc_Absyn_Constraint _tmp385;int _tmp386;union Cyc_Absyn_Constraint
_tmp387;void*_tmp388;union Cyc_Absyn_Constraint _tmp389;union Cyc_Absyn_Constraint*
_tmp38A;_LL26A: _tmp385=*_tmp384;if((_tmp385.No_constr).tag != 3)goto _LL26C;
_tmp386=(int)(_tmp385.No_constr).val;_LL26B: return - 1;_LL26C: _tmp387=*_tmp384;if((
_tmp387.Eq_constr).tag != 1)goto _LL26E;_tmp388=(void*)(_tmp387.Eq_constr).val;
_LL26D: {union Cyc_Absyn_Constraint*_tmp38B=y;union Cyc_Absyn_Constraint _tmp38C;
int _tmp38D;union Cyc_Absyn_Constraint _tmp38E;void*_tmp38F;union Cyc_Absyn_Constraint
_tmp390;union Cyc_Absyn_Constraint*_tmp391;_LL271: _tmp38C=*_tmp38B;if((_tmp38C.No_constr).tag
!= 3)goto _LL273;_tmp38D=(int)(_tmp38C.No_constr).val;_LL272: return 1;_LL273:
_tmp38E=*_tmp38B;if((_tmp38E.Eq_constr).tag != 1)goto _LL275;_tmp38F=(void*)(
_tmp38E.Eq_constr).val;_LL274: return cmp(_tmp388,_tmp38F);_LL275: _tmp390=*_tmp38B;
if((_tmp390.Forward_constr).tag != 2)goto _LL270;_tmp391=(union Cyc_Absyn_Constraint*)(
_tmp390.Forward_constr).val;_LL276:({void*_tmp392=0;Cyc_Tcutil_impos(({const char*
_tmp393="unify_conref: forward after compress(2)";_tag_dyneither(_tmp393,sizeof(
char),40);}),_tag_dyneither(_tmp392,sizeof(void*),0));});_LL270:;}_LL26E: _tmp389=*
_tmp384;if((_tmp389.Forward_constr).tag != 2)goto _LL269;_tmp38A=(union Cyc_Absyn_Constraint*)(
_tmp389.Forward_constr).val;_LL26F:({void*_tmp394=0;Cyc_Tcutil_impos(({const char*
_tmp395="unify_conref: forward after compress";_tag_dyneither(_tmp395,sizeof(
char),37);}),_tag_dyneither(_tmp394,sizeof(void*),0));});_LL269:;}}static int Cyc_Tcutil_tqual_type_cmp(
struct _tuple11*tqt1,struct _tuple11*tqt2){struct _tuple11 _tmp397;struct Cyc_Absyn_Tqual
_tmp398;void*_tmp399;struct _tuple11*_tmp396=tqt1;_tmp397=*_tmp396;_tmp398=
_tmp397.f1;_tmp399=_tmp397.f2;{struct _tuple11 _tmp39B;struct Cyc_Absyn_Tqual
_tmp39C;void*_tmp39D;struct _tuple11*_tmp39A=tqt2;_tmp39B=*_tmp39A;_tmp39C=
_tmp39B.f1;_tmp39D=_tmp39B.f2;{int _tmp39E=Cyc_Tcutil_tqual_cmp(_tmp398,_tmp39C);
if(_tmp39E != 0)return _tmp39E;return Cyc_Tcutil_typecmp(_tmp399,_tmp39D);}}}static
int Cyc_Tcutil_aggrfield_cmp(struct Cyc_Absyn_Aggrfield*f1,struct Cyc_Absyn_Aggrfield*
f2){int _tmp39F=Cyc_strptrcmp(f1->name,f2->name);if(_tmp39F != 0)return _tmp39F;{
int _tmp3A0=Cyc_Tcutil_tqual_cmp(f1->tq,f2->tq);if(_tmp3A0 != 0)return _tmp3A0;{int
_tmp3A1=Cyc_Tcutil_typecmp(f1->type,f2->type);if(_tmp3A1 != 0)return _tmp3A1;{int
_tmp3A2=Cyc_List_list_cmp(Cyc_Tcutil_attribute_cmp,f1->attributes,f2->attributes);
if(_tmp3A2 != 0)return _tmp3A2;return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(
Cyc_Evexp_const_exp_cmp,f1->width,f2->width);}}}}static int Cyc_Tcutil_enumfield_cmp(
struct Cyc_Absyn_Enumfield*e1,struct Cyc_Absyn_Enumfield*e2){int _tmp3A3=Cyc_Absyn_qvar_cmp(
e1->name,e2->name);if(_tmp3A3 != 0)return _tmp3A3;return((int(*)(int(*cmp)(struct
Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*
a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,e1->tag,e2->tag);}static int Cyc_Tcutil_type_case_number(
void*t){void*_tmp3A4=t;_LL278: if((int)_tmp3A4 != 0)goto _LL27A;_LL279: return 0;
_LL27A: if(_tmp3A4 <= (void*)4)goto _LL286;if(*((int*)_tmp3A4)!= 0)goto _LL27C;
_LL27B: return 1;_LL27C: if(*((int*)_tmp3A4)!= 1)goto _LL27E;_LL27D: return 2;_LL27E:
if(*((int*)_tmp3A4)!= 2)goto _LL280;_LL27F: return 3;_LL280: if(*((int*)_tmp3A4)!= 3)
goto _LL282;_LL281: return 4;_LL282: if(*((int*)_tmp3A4)!= 4)goto _LL284;_LL283:
return 5;_LL284: if(*((int*)_tmp3A4)!= 5)goto _LL286;_LL285: return 6;_LL286: if((int)
_tmp3A4 != 1)goto _LL288;_LL287: return 7;_LL288: if(_tmp3A4 <= (void*)4)goto _LL29C;
if(*((int*)_tmp3A4)!= 6)goto _LL28A;_LL289: return 8;_LL28A: if(*((int*)_tmp3A4)!= 7)
goto _LL28C;_LL28B: return 9;_LL28C: if(*((int*)_tmp3A4)!= 8)goto _LL28E;_LL28D:
return 10;_LL28E: if(*((int*)_tmp3A4)!= 9)goto _LL290;_LL28F: return 11;_LL290: if(*((
int*)_tmp3A4)!= 10)goto _LL292;_LL291: return 12;_LL292: if(*((int*)_tmp3A4)!= 11)
goto _LL294;_LL293: return 14;_LL294: if(*((int*)_tmp3A4)!= 12)goto _LL296;_LL295:
return 16;_LL296: if(*((int*)_tmp3A4)!= 13)goto _LL298;_LL297: return 17;_LL298: if(*((
int*)_tmp3A4)!= 14)goto _LL29A;_LL299: return 18;_LL29A: if(*((int*)_tmp3A4)!= 16)
goto _LL29C;_LL29B: return 19;_LL29C: if((int)_tmp3A4 != 3)goto _LL29E;_LL29D: return 20;
_LL29E: if((int)_tmp3A4 != 2)goto _LL2A0;_LL29F: return 21;_LL2A0: if(_tmp3A4 <= (void*)
4)goto _LL2A2;if(*((int*)_tmp3A4)!= 19)goto _LL2A2;_LL2A1: return 22;_LL2A2: if(
_tmp3A4 <= (void*)4)goto _LL2A4;if(*((int*)_tmp3A4)!= 20)goto _LL2A4;_LL2A3: return
23;_LL2A4: if(_tmp3A4 <= (void*)4)goto _LL2A6;if(*((int*)_tmp3A4)!= 21)goto _LL2A6;
_LL2A5: return 24;_LL2A6: if(_tmp3A4 <= (void*)4)goto _LL2A8;if(*((int*)_tmp3A4)!= 18)
goto _LL2A8;_LL2A7: return 26;_LL2A8: if(_tmp3A4 <= (void*)4)goto _LL2AA;if(*((int*)
_tmp3A4)!= 15)goto _LL2AA;_LL2A9: return 27;_LL2AA: if(_tmp3A4 <= (void*)4)goto _LL277;
if(*((int*)_tmp3A4)!= 17)goto _LL277;_LL2AB: return 28;_LL277:;}int Cyc_Tcutil_typecmp(
void*t1,void*t2){t1=Cyc_Tcutil_compress(t1);t2=Cyc_Tcutil_compress(t2);if(t1 == 
t2)return 0;{int _tmp3A5=Cyc_Core_intcmp(Cyc_Tcutil_type_case_number(t1),Cyc_Tcutil_type_case_number(
t2));if(_tmp3A5 != 0)return _tmp3A5;{struct _tuple0 _tmp3A7=({struct _tuple0 _tmp3A6;
_tmp3A6.f1=t2;_tmp3A6.f2=t1;_tmp3A6;});void*_tmp3A8;void*_tmp3A9;void*_tmp3AA;
struct Cyc_Absyn_Tvar*_tmp3AB;void*_tmp3AC;struct Cyc_Absyn_Tvar*_tmp3AD;void*
_tmp3AE;struct Cyc_Absyn_AggrInfo _tmp3AF;union Cyc_Absyn_AggrInfoU _tmp3B0;struct
Cyc_List_List*_tmp3B1;void*_tmp3B2;struct Cyc_Absyn_AggrInfo _tmp3B3;union Cyc_Absyn_AggrInfoU
_tmp3B4;struct Cyc_List_List*_tmp3B5;void*_tmp3B6;struct _tuple2*_tmp3B7;void*
_tmp3B8;struct _tuple2*_tmp3B9;void*_tmp3BA;struct Cyc_List_List*_tmp3BB;void*
_tmp3BC;struct Cyc_List_List*_tmp3BD;void*_tmp3BE;struct Cyc_Absyn_DatatypeInfo
_tmp3BF;union Cyc_Absyn_DatatypeInfoU _tmp3C0;struct Cyc_Absyn_Datatypedecl**
_tmp3C1;struct Cyc_Absyn_Datatypedecl*_tmp3C2;struct Cyc_List_List*_tmp3C3;struct
Cyc_Core_Opt*_tmp3C4;void*_tmp3C5;struct Cyc_Absyn_DatatypeInfo _tmp3C6;union Cyc_Absyn_DatatypeInfoU
_tmp3C7;struct Cyc_Absyn_Datatypedecl**_tmp3C8;struct Cyc_Absyn_Datatypedecl*
_tmp3C9;struct Cyc_List_List*_tmp3CA;struct Cyc_Core_Opt*_tmp3CB;void*_tmp3CC;
struct Cyc_Absyn_DatatypeFieldInfo _tmp3CD;union Cyc_Absyn_DatatypeFieldInfoU
_tmp3CE;struct _tuple3 _tmp3CF;struct Cyc_Absyn_Datatypedecl*_tmp3D0;struct Cyc_Absyn_Datatypefield*
_tmp3D1;struct Cyc_List_List*_tmp3D2;void*_tmp3D3;struct Cyc_Absyn_DatatypeFieldInfo
_tmp3D4;union Cyc_Absyn_DatatypeFieldInfoU _tmp3D5;struct _tuple3 _tmp3D6;struct Cyc_Absyn_Datatypedecl*
_tmp3D7;struct Cyc_Absyn_Datatypefield*_tmp3D8;struct Cyc_List_List*_tmp3D9;void*
_tmp3DA;struct Cyc_Absyn_PtrInfo _tmp3DB;void*_tmp3DC;struct Cyc_Absyn_Tqual _tmp3DD;
struct Cyc_Absyn_PtrAtts _tmp3DE;void*_tmp3DF;union Cyc_Absyn_Constraint*_tmp3E0;
union Cyc_Absyn_Constraint*_tmp3E1;union Cyc_Absyn_Constraint*_tmp3E2;void*_tmp3E3;
struct Cyc_Absyn_PtrInfo _tmp3E4;void*_tmp3E5;struct Cyc_Absyn_Tqual _tmp3E6;struct
Cyc_Absyn_PtrAtts _tmp3E7;void*_tmp3E8;union Cyc_Absyn_Constraint*_tmp3E9;union Cyc_Absyn_Constraint*
_tmp3EA;union Cyc_Absyn_Constraint*_tmp3EB;void*_tmp3EC;void*_tmp3ED;void*_tmp3EE;
void*_tmp3EF;void*_tmp3F0;void*_tmp3F1;void*_tmp3F2;void*_tmp3F3;void*_tmp3F4;
int _tmp3F5;void*_tmp3F6;int _tmp3F7;void*_tmp3F8;struct Cyc_Absyn_ArrayInfo _tmp3F9;
void*_tmp3FA;struct Cyc_Absyn_Tqual _tmp3FB;struct Cyc_Absyn_Exp*_tmp3FC;union Cyc_Absyn_Constraint*
_tmp3FD;void*_tmp3FE;struct Cyc_Absyn_ArrayInfo _tmp3FF;void*_tmp400;struct Cyc_Absyn_Tqual
_tmp401;struct Cyc_Absyn_Exp*_tmp402;union Cyc_Absyn_Constraint*_tmp403;void*
_tmp404;struct Cyc_Absyn_FnInfo _tmp405;struct Cyc_List_List*_tmp406;struct Cyc_Core_Opt*
_tmp407;void*_tmp408;struct Cyc_List_List*_tmp409;int _tmp40A;struct Cyc_Absyn_VarargInfo*
_tmp40B;struct Cyc_List_List*_tmp40C;struct Cyc_List_List*_tmp40D;void*_tmp40E;
struct Cyc_Absyn_FnInfo _tmp40F;struct Cyc_List_List*_tmp410;struct Cyc_Core_Opt*
_tmp411;void*_tmp412;struct Cyc_List_List*_tmp413;int _tmp414;struct Cyc_Absyn_VarargInfo*
_tmp415;struct Cyc_List_List*_tmp416;struct Cyc_List_List*_tmp417;void*_tmp418;
struct Cyc_List_List*_tmp419;void*_tmp41A;struct Cyc_List_List*_tmp41B;void*
_tmp41C;void*_tmp41D;struct Cyc_List_List*_tmp41E;void*_tmp41F;void*_tmp420;
struct Cyc_List_List*_tmp421;void*_tmp422;void*_tmp423;void*_tmp424;void*_tmp425;
void*_tmp426;void*_tmp427;void*_tmp428;void*_tmp429;void*_tmp42A;void*_tmp42B;
void*_tmp42C;void*_tmp42D;void*_tmp42E;void*_tmp42F;void*_tmp430;struct Cyc_Absyn_Exp*
_tmp431;void*_tmp432;struct Cyc_Absyn_Exp*_tmp433;void*_tmp434;void*_tmp435;void*
_tmp436;void*_tmp437;void*_tmp438;void*_tmp439;_LL2AD: _tmp3A8=_tmp3A7.f1;if(
_tmp3A8 <= (void*)4)goto _LL2AF;if(*((int*)_tmp3A8)!= 0)goto _LL2AF;_tmp3A9=_tmp3A7.f2;
if(_tmp3A9 <= (void*)4)goto _LL2AF;if(*((int*)_tmp3A9)!= 0)goto _LL2AF;_LL2AE:({
void*_tmp43A=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp43B="typecmp: can only compare closed types";_tag_dyneither(
_tmp43B,sizeof(char),39);}),_tag_dyneither(_tmp43A,sizeof(void*),0));});_LL2AF:
_tmp3AA=_tmp3A7.f1;if(_tmp3AA <= (void*)4)goto _LL2B1;if(*((int*)_tmp3AA)!= 1)goto
_LL2B1;_tmp3AB=((struct Cyc_Absyn_VarType_struct*)_tmp3AA)->f1;_tmp3AC=_tmp3A7.f2;
if(_tmp3AC <= (void*)4)goto _LL2B1;if(*((int*)_tmp3AC)!= 1)goto _LL2B1;_tmp3AD=((
struct Cyc_Absyn_VarType_struct*)_tmp3AC)->f1;_LL2B0: return Cyc_Core_intcmp(
_tmp3AD->identity,_tmp3AB->identity);_LL2B1: _tmp3AE=_tmp3A7.f1;if(_tmp3AE <= (
void*)4)goto _LL2B3;if(*((int*)_tmp3AE)!= 10)goto _LL2B3;_tmp3AF=((struct Cyc_Absyn_AggrType_struct*)
_tmp3AE)->f1;_tmp3B0=_tmp3AF.aggr_info;_tmp3B1=_tmp3AF.targs;_tmp3B2=_tmp3A7.f2;
if(_tmp3B2 <= (void*)4)goto _LL2B3;if(*((int*)_tmp3B2)!= 10)goto _LL2B3;_tmp3B3=((
struct Cyc_Absyn_AggrType_struct*)_tmp3B2)->f1;_tmp3B4=_tmp3B3.aggr_info;_tmp3B5=
_tmp3B3.targs;_LL2B2: {struct _tuple2*_tmp43D;struct _tuple12 _tmp43C=Cyc_Absyn_aggr_kinded_name(
_tmp3B0);_tmp43D=_tmp43C.f2;{struct _tuple2*_tmp43F;struct _tuple12 _tmp43E=Cyc_Absyn_aggr_kinded_name(
_tmp3B4);_tmp43F=_tmp43E.f2;{int _tmp440=Cyc_Absyn_qvar_cmp(_tmp43D,_tmp43F);if(
_tmp440 != 0)return _tmp440;else{return Cyc_List_list_cmp(Cyc_Tcutil_typecmp,
_tmp3B1,_tmp3B5);}}}}_LL2B3: _tmp3B6=_tmp3A7.f1;if(_tmp3B6 <= (void*)4)goto _LL2B5;
if(*((int*)_tmp3B6)!= 12)goto _LL2B5;_tmp3B7=((struct Cyc_Absyn_EnumType_struct*)
_tmp3B6)->f1;_tmp3B8=_tmp3A7.f2;if(_tmp3B8 <= (void*)4)goto _LL2B5;if(*((int*)
_tmp3B8)!= 12)goto _LL2B5;_tmp3B9=((struct Cyc_Absyn_EnumType_struct*)_tmp3B8)->f1;
_LL2B4: return Cyc_Absyn_qvar_cmp(_tmp3B7,_tmp3B9);_LL2B5: _tmp3BA=_tmp3A7.f1;if(
_tmp3BA <= (void*)4)goto _LL2B7;if(*((int*)_tmp3BA)!= 13)goto _LL2B7;_tmp3BB=((
struct Cyc_Absyn_AnonEnumType_struct*)_tmp3BA)->f1;_tmp3BC=_tmp3A7.f2;if(_tmp3BC
<= (void*)4)goto _LL2B7;if(*((int*)_tmp3BC)!= 13)goto _LL2B7;_tmp3BD=((struct Cyc_Absyn_AnonEnumType_struct*)
_tmp3BC)->f1;_LL2B6: return((int(*)(int(*cmp)(struct Cyc_Absyn_Enumfield*,struct
Cyc_Absyn_Enumfield*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(
Cyc_Tcutil_enumfield_cmp,_tmp3BB,_tmp3BD);_LL2B7: _tmp3BE=_tmp3A7.f1;if(_tmp3BE <= (
void*)4)goto _LL2B9;if(*((int*)_tmp3BE)!= 2)goto _LL2B9;_tmp3BF=((struct Cyc_Absyn_DatatypeType_struct*)
_tmp3BE)->f1;_tmp3C0=_tmp3BF.datatype_info;if((_tmp3C0.KnownDatatype).tag != 2)
goto _LL2B9;_tmp3C1=(struct Cyc_Absyn_Datatypedecl**)(_tmp3C0.KnownDatatype).val;
_tmp3C2=*_tmp3C1;_tmp3C3=_tmp3BF.targs;_tmp3C4=_tmp3BF.rgn;_tmp3C5=_tmp3A7.f2;
if(_tmp3C5 <= (void*)4)goto _LL2B9;if(*((int*)_tmp3C5)!= 2)goto _LL2B9;_tmp3C6=((
struct Cyc_Absyn_DatatypeType_struct*)_tmp3C5)->f1;_tmp3C7=_tmp3C6.datatype_info;
if((_tmp3C7.KnownDatatype).tag != 2)goto _LL2B9;_tmp3C8=(struct Cyc_Absyn_Datatypedecl**)(
_tmp3C7.KnownDatatype).val;_tmp3C9=*_tmp3C8;_tmp3CA=_tmp3C6.targs;_tmp3CB=
_tmp3C6.rgn;_LL2B8: if(_tmp3C9 == _tmp3C2)return 0;{int _tmp441=Cyc_Absyn_qvar_cmp(
_tmp3C9->name,_tmp3C2->name);if(_tmp441 != 0)return _tmp441;if((unsigned int)
_tmp3CB  && (unsigned int)_tmp3C4){int _tmp442=Cyc_Tcutil_typecmp((void*)_tmp3CB->v,(
void*)_tmp3C4->v);if(_tmp442 != 0)return _tmp442;}else{if((unsigned int)_tmp3CB)
return - 1;else{return 1;}}return Cyc_List_list_cmp(Cyc_Tcutil_typecmp,_tmp3CA,
_tmp3C3);}_LL2B9: _tmp3CC=_tmp3A7.f1;if(_tmp3CC <= (void*)4)goto _LL2BB;if(*((int*)
_tmp3CC)!= 3)goto _LL2BB;_tmp3CD=((struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp3CC)->f1;_tmp3CE=_tmp3CD.field_info;if((_tmp3CE.KnownDatatypefield).tag != 2)
goto _LL2BB;_tmp3CF=(struct _tuple3)(_tmp3CE.KnownDatatypefield).val;_tmp3D0=
_tmp3CF.f1;_tmp3D1=_tmp3CF.f2;_tmp3D2=_tmp3CD.targs;_tmp3D3=_tmp3A7.f2;if(
_tmp3D3 <= (void*)4)goto _LL2BB;if(*((int*)_tmp3D3)!= 3)goto _LL2BB;_tmp3D4=((
struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp3D3)->f1;_tmp3D5=_tmp3D4.field_info;
if((_tmp3D5.KnownDatatypefield).tag != 2)goto _LL2BB;_tmp3D6=(struct _tuple3)(
_tmp3D5.KnownDatatypefield).val;_tmp3D7=_tmp3D6.f1;_tmp3D8=_tmp3D6.f2;_tmp3D9=
_tmp3D4.targs;_LL2BA: if(_tmp3D7 == _tmp3D0)return 0;{int _tmp443=Cyc_Absyn_qvar_cmp(
_tmp3D0->name,_tmp3D7->name);if(_tmp443 != 0)return _tmp443;{int _tmp444=Cyc_Absyn_qvar_cmp(
_tmp3D1->name,_tmp3D8->name);if(_tmp444 != 0)return _tmp444;return Cyc_List_list_cmp(
Cyc_Tcutil_typecmp,_tmp3D9,_tmp3D2);}}_LL2BB: _tmp3DA=_tmp3A7.f1;if(_tmp3DA <= (
void*)4)goto _LL2BD;if(*((int*)_tmp3DA)!= 4)goto _LL2BD;_tmp3DB=((struct Cyc_Absyn_PointerType_struct*)
_tmp3DA)->f1;_tmp3DC=_tmp3DB.elt_typ;_tmp3DD=_tmp3DB.elt_tq;_tmp3DE=_tmp3DB.ptr_atts;
_tmp3DF=_tmp3DE.rgn;_tmp3E0=_tmp3DE.nullable;_tmp3E1=_tmp3DE.bounds;_tmp3E2=
_tmp3DE.zero_term;_tmp3E3=_tmp3A7.f2;if(_tmp3E3 <= (void*)4)goto _LL2BD;if(*((int*)
_tmp3E3)!= 4)goto _LL2BD;_tmp3E4=((struct Cyc_Absyn_PointerType_struct*)_tmp3E3)->f1;
_tmp3E5=_tmp3E4.elt_typ;_tmp3E6=_tmp3E4.elt_tq;_tmp3E7=_tmp3E4.ptr_atts;_tmp3E8=
_tmp3E7.rgn;_tmp3E9=_tmp3E7.nullable;_tmp3EA=_tmp3E7.bounds;_tmp3EB=_tmp3E7.zero_term;
_LL2BC: {int _tmp445=Cyc_Tcutil_typecmp(_tmp3E5,_tmp3DC);if(_tmp445 != 0)return
_tmp445;{int _tmp446=Cyc_Tcutil_typecmp(_tmp3E8,_tmp3DF);if(_tmp446 != 0)return
_tmp446;{int _tmp447=Cyc_Tcutil_tqual_cmp(_tmp3E6,_tmp3DD);if(_tmp447 != 0)return
_tmp447;{int _tmp448=Cyc_Tcutil_conrefs_cmp(Cyc_Tcutil_boundscmp,_tmp3EA,_tmp3E1);
if(_tmp448 != 0)return _tmp448;{int _tmp449=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*
x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_conrefs_cmp)(Cyc_Core_intcmp,_tmp3EB,
_tmp3E2);if(_tmp449 != 0)return _tmp449;{void*_tmp44A=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,
_tmp3EA);_LL2E2: if((int)_tmp44A != 0)goto _LL2E4;_LL2E3: return 0;_LL2E4:;_LL2E5:
goto _LL2E1;_LL2E1:;}return((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*
x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_conrefs_cmp)(Cyc_Core_intcmp,_tmp3E9,
_tmp3E0);}}}}}_LL2BD: _tmp3EC=_tmp3A7.f1;if(_tmp3EC <= (void*)4)goto _LL2BF;if(*((
int*)_tmp3EC)!= 5)goto _LL2BF;_tmp3ED=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp3EC)->f1;_tmp3EE=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp3EC)->f2;
_tmp3EF=_tmp3A7.f2;if(_tmp3EF <= (void*)4)goto _LL2BF;if(*((int*)_tmp3EF)!= 5)goto
_LL2BF;_tmp3F0=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp3EF)->f1;_tmp3F1=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp3EF)->f2;_LL2BE: if(_tmp3F0 != _tmp3ED)
return Cyc_Core_intcmp((int)((unsigned int)_tmp3F0),(int)((unsigned int)_tmp3ED));
if(_tmp3F1 != _tmp3EE)return Cyc_Core_intcmp((int)((unsigned int)_tmp3F1),(int)((
unsigned int)_tmp3EE));return 0;_LL2BF: _tmp3F2=_tmp3A7.f1;if((int)_tmp3F2 != 1)
goto _LL2C1;_tmp3F3=_tmp3A7.f2;if((int)_tmp3F3 != 1)goto _LL2C1;_LL2C0: return 0;
_LL2C1: _tmp3F4=_tmp3A7.f1;if(_tmp3F4 <= (void*)4)goto _LL2C3;if(*((int*)_tmp3F4)!= 
6)goto _LL2C3;_tmp3F5=((struct Cyc_Absyn_DoubleType_struct*)_tmp3F4)->f1;_tmp3F6=
_tmp3A7.f2;if(_tmp3F6 <= (void*)4)goto _LL2C3;if(*((int*)_tmp3F6)!= 6)goto _LL2C3;
_tmp3F7=((struct Cyc_Absyn_DoubleType_struct*)_tmp3F6)->f1;_LL2C2: if(_tmp3F5 == 
_tmp3F7)return 0;else{if(_tmp3F5)return - 1;else{return 1;}}_LL2C3: _tmp3F8=_tmp3A7.f1;
if(_tmp3F8 <= (void*)4)goto _LL2C5;if(*((int*)_tmp3F8)!= 7)goto _LL2C5;_tmp3F9=((
struct Cyc_Absyn_ArrayType_struct*)_tmp3F8)->f1;_tmp3FA=_tmp3F9.elt_type;_tmp3FB=
_tmp3F9.tq;_tmp3FC=_tmp3F9.num_elts;_tmp3FD=_tmp3F9.zero_term;_tmp3FE=_tmp3A7.f2;
if(_tmp3FE <= (void*)4)goto _LL2C5;if(*((int*)_tmp3FE)!= 7)goto _LL2C5;_tmp3FF=((
struct Cyc_Absyn_ArrayType_struct*)_tmp3FE)->f1;_tmp400=_tmp3FF.elt_type;_tmp401=
_tmp3FF.tq;_tmp402=_tmp3FF.num_elts;_tmp403=_tmp3FF.zero_term;_LL2C4: {int
_tmp44B=Cyc_Tcutil_tqual_cmp(_tmp401,_tmp3FB);if(_tmp44B != 0)return _tmp44B;{int
_tmp44C=Cyc_Tcutil_typecmp(_tmp400,_tmp3FA);if(_tmp44C != 0)return _tmp44C;{int
_tmp44D=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*
y))Cyc_Tcutil_conrefs_cmp)(Cyc_Core_intcmp,_tmp3FD,_tmp403);if(_tmp44D != 0)
return _tmp44D;if(_tmp3FC == _tmp402)return 0;if(_tmp3FC == 0  || _tmp402 == 0)({void*
_tmp44E=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp44F="missing expression in array index";_tag_dyneither(_tmp44F,
sizeof(char),34);}),_tag_dyneither(_tmp44E,sizeof(void*),0));});return((int(*)(
int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,
struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,_tmp3FC,
_tmp402);}}}_LL2C5: _tmp404=_tmp3A7.f1;if(_tmp404 <= (void*)4)goto _LL2C7;if(*((int*)
_tmp404)!= 8)goto _LL2C7;_tmp405=((struct Cyc_Absyn_FnType_struct*)_tmp404)->f1;
_tmp406=_tmp405.tvars;_tmp407=_tmp405.effect;_tmp408=_tmp405.ret_typ;_tmp409=
_tmp405.args;_tmp40A=_tmp405.c_varargs;_tmp40B=_tmp405.cyc_varargs;_tmp40C=
_tmp405.rgn_po;_tmp40D=_tmp405.attributes;_tmp40E=_tmp3A7.f2;if(_tmp40E <= (void*)
4)goto _LL2C7;if(*((int*)_tmp40E)!= 8)goto _LL2C7;_tmp40F=((struct Cyc_Absyn_FnType_struct*)
_tmp40E)->f1;_tmp410=_tmp40F.tvars;_tmp411=_tmp40F.effect;_tmp412=_tmp40F.ret_typ;
_tmp413=_tmp40F.args;_tmp414=_tmp40F.c_varargs;_tmp415=_tmp40F.cyc_varargs;
_tmp416=_tmp40F.rgn_po;_tmp417=_tmp40F.attributes;_LL2C6:({void*_tmp450=0;((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmp451="typecmp: function types not handled";_tag_dyneither(_tmp451,sizeof(char),
36);}),_tag_dyneither(_tmp450,sizeof(void*),0));});_LL2C7: _tmp418=_tmp3A7.f1;if(
_tmp418 <= (void*)4)goto _LL2C9;if(*((int*)_tmp418)!= 9)goto _LL2C9;_tmp419=((
struct Cyc_Absyn_TupleType_struct*)_tmp418)->f1;_tmp41A=_tmp3A7.f2;if(_tmp41A <= (
void*)4)goto _LL2C9;if(*((int*)_tmp41A)!= 9)goto _LL2C9;_tmp41B=((struct Cyc_Absyn_TupleType_struct*)
_tmp41A)->f1;_LL2C8: return((int(*)(int(*cmp)(struct _tuple11*,struct _tuple11*),
struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_tqual_type_cmp,
_tmp41B,_tmp419);_LL2C9: _tmp41C=_tmp3A7.f1;if(_tmp41C <= (void*)4)goto _LL2CB;if(*((
int*)_tmp41C)!= 11)goto _LL2CB;_tmp41D=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp41C)->f1;_tmp41E=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp41C)->f2;_tmp41F=
_tmp3A7.f2;if(_tmp41F <= (void*)4)goto _LL2CB;if(*((int*)_tmp41F)!= 11)goto _LL2CB;
_tmp420=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)_tmp41F)->f1;_tmp421=((
struct Cyc_Absyn_AnonAggrType_struct*)_tmp41F)->f2;_LL2CA: if(_tmp420 != _tmp41D){
if(_tmp420 == (void*)0)return - 1;else{return 1;}}return((int(*)(int(*cmp)(struct Cyc_Absyn_Aggrfield*,
struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(
Cyc_Tcutil_aggrfield_cmp,_tmp421,_tmp41E);_LL2CB: _tmp422=_tmp3A7.f1;if(_tmp422 <= (
void*)4)goto _LL2CD;if(*((int*)_tmp422)!= 14)goto _LL2CD;_tmp423=(void*)((struct
Cyc_Absyn_RgnHandleType_struct*)_tmp422)->f1;_tmp424=_tmp3A7.f2;if(_tmp424 <= (
void*)4)goto _LL2CD;if(*((int*)_tmp424)!= 14)goto _LL2CD;_tmp425=(void*)((struct
Cyc_Absyn_RgnHandleType_struct*)_tmp424)->f1;_LL2CC: return Cyc_Tcutil_typecmp(
_tmp423,_tmp425);_LL2CD: _tmp426=_tmp3A7.f1;if(_tmp426 <= (void*)4)goto _LL2CF;if(*((
int*)_tmp426)!= 15)goto _LL2CF;_tmp427=(void*)((struct Cyc_Absyn_DynRgnType_struct*)
_tmp426)->f1;_tmp428=(void*)((struct Cyc_Absyn_DynRgnType_struct*)_tmp426)->f2;
_tmp429=_tmp3A7.f2;if(_tmp429 <= (void*)4)goto _LL2CF;if(*((int*)_tmp429)!= 15)
goto _LL2CF;_tmp42A=(void*)((struct Cyc_Absyn_DynRgnType_struct*)_tmp429)->f1;
_tmp42B=(void*)((struct Cyc_Absyn_DynRgnType_struct*)_tmp429)->f2;_LL2CE: {int
_tmp452=Cyc_Tcutil_typecmp(_tmp427,_tmp42A);if(_tmp452 != 0)return _tmp452;else{
return Cyc_Tcutil_typecmp(_tmp428,_tmp42B);}}_LL2CF: _tmp42C=_tmp3A7.f1;if(_tmp42C
<= (void*)4)goto _LL2D1;if(*((int*)_tmp42C)!= 18)goto _LL2D1;_tmp42D=(void*)((
struct Cyc_Absyn_TagType_struct*)_tmp42C)->f1;_tmp42E=_tmp3A7.f2;if(_tmp42E <= (
void*)4)goto _LL2D1;if(*((int*)_tmp42E)!= 18)goto _LL2D1;_tmp42F=(void*)((struct
Cyc_Absyn_TagType_struct*)_tmp42E)->f1;_LL2D0: return Cyc_Tcutil_typecmp(_tmp42D,
_tmp42F);_LL2D1: _tmp430=_tmp3A7.f1;if(_tmp430 <= (void*)4)goto _LL2D3;if(*((int*)
_tmp430)!= 17)goto _LL2D3;_tmp431=((struct Cyc_Absyn_ValueofType_struct*)_tmp430)->f1;
_tmp432=_tmp3A7.f2;if(_tmp432 <= (void*)4)goto _LL2D3;if(*((int*)_tmp432)!= 17)
goto _LL2D3;_tmp433=((struct Cyc_Absyn_ValueofType_struct*)_tmp432)->f1;_LL2D2:
return Cyc_Evexp_const_exp_cmp(_tmp431,_tmp433);_LL2D3: _tmp434=_tmp3A7.f1;if(
_tmp434 <= (void*)4)goto _LL2D5;if(*((int*)_tmp434)!= 20)goto _LL2D5;_LL2D4: goto
_LL2D6;_LL2D5: _tmp435=_tmp3A7.f2;if(_tmp435 <= (void*)4)goto _LL2D7;if(*((int*)
_tmp435)!= 20)goto _LL2D7;_LL2D6: goto _LL2D8;_LL2D7: _tmp436=_tmp3A7.f1;if(_tmp436
<= (void*)4)goto _LL2D9;if(*((int*)_tmp436)!= 19)goto _LL2D9;_LL2D8: goto _LL2DA;
_LL2D9: _tmp437=_tmp3A7.f1;if(_tmp437 <= (void*)4)goto _LL2DB;if(*((int*)_tmp437)!= 
21)goto _LL2DB;_LL2DA: goto _LL2DC;_LL2DB: _tmp438=_tmp3A7.f2;if(_tmp438 <= (void*)4)
goto _LL2DD;if(*((int*)_tmp438)!= 21)goto _LL2DD;_LL2DC: goto _LL2DE;_LL2DD: _tmp439=
_tmp3A7.f2;if(_tmp439 <= (void*)4)goto _LL2DF;if(*((int*)_tmp439)!= 19)goto _LL2DF;
_LL2DE:({void*_tmp453=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(({const char*_tmp454="typecmp: effects not handled";
_tag_dyneither(_tmp454,sizeof(char),29);}),_tag_dyneither(_tmp453,sizeof(void*),
0));});_LL2DF:;_LL2E0:({void*_tmp455=0;((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp456="Unmatched case in typecmp";
_tag_dyneither(_tmp456,sizeof(char),26);}),_tag_dyneither(_tmp455,sizeof(void*),
0));});_LL2AC:;}}}int Cyc_Tcutil_is_arithmetic_type(void*t){void*_tmp457=Cyc_Tcutil_compress(
t);_LL2E7: if(_tmp457 <= (void*)4)goto _LL2E9;if(*((int*)_tmp457)!= 5)goto _LL2E9;
_LL2E8: goto _LL2EA;_LL2E9: if((int)_tmp457 != 1)goto _LL2EB;_LL2EA: goto _LL2EC;_LL2EB:
if(_tmp457 <= (void*)4)goto _LL2F1;if(*((int*)_tmp457)!= 6)goto _LL2ED;_LL2EC: goto
_LL2EE;_LL2ED: if(*((int*)_tmp457)!= 12)goto _LL2EF;_LL2EE: goto _LL2F0;_LL2EF: if(*((
int*)_tmp457)!= 13)goto _LL2F1;_LL2F0: return 1;_LL2F1:;_LL2F2: return 0;_LL2E6:;}int
Cyc_Tcutil_will_lose_precision(void*t1,void*t2){t1=Cyc_Tcutil_compress(t1);t2=
Cyc_Tcutil_compress(t2);{struct _tuple0 _tmp459=({struct _tuple0 _tmp458;_tmp458.f1=
t1;_tmp458.f2=t2;_tmp458;});void*_tmp45A;int _tmp45B;void*_tmp45C;int _tmp45D;void*
_tmp45E;void*_tmp45F;void*_tmp460;void*_tmp461;void*_tmp462;void*_tmp463;void*
_tmp464;void*_tmp465;void*_tmp466;void*_tmp467;void*_tmp468;void*_tmp469;void*
_tmp46A;void*_tmp46B;void*_tmp46C;void*_tmp46D;void*_tmp46E;void*_tmp46F;void*
_tmp470;void*_tmp471;void*_tmp472;void*_tmp473;void*_tmp474;void*_tmp475;void*
_tmp476;void*_tmp477;void*_tmp478;void*_tmp479;void*_tmp47A;void*_tmp47B;void*
_tmp47C;void*_tmp47D;void*_tmp47E;void*_tmp47F;void*_tmp480;void*_tmp481;void*
_tmp482;void*_tmp483;void*_tmp484;void*_tmp485;void*_tmp486;void*_tmp487;void*
_tmp488;void*_tmp489;void*_tmp48A;void*_tmp48B;void*_tmp48C;void*_tmp48D;void*
_tmp48E;void*_tmp48F;void*_tmp490;void*_tmp491;void*_tmp492;void*_tmp493;void*
_tmp494;void*_tmp495;_LL2F4: _tmp45A=_tmp459.f1;if(_tmp45A <= (void*)4)goto _LL2F6;
if(*((int*)_tmp45A)!= 6)goto _LL2F6;_tmp45B=((struct Cyc_Absyn_DoubleType_struct*)
_tmp45A)->f1;_tmp45C=_tmp459.f2;if(_tmp45C <= (void*)4)goto _LL2F6;if(*((int*)
_tmp45C)!= 6)goto _LL2F6;_tmp45D=((struct Cyc_Absyn_DoubleType_struct*)_tmp45C)->f1;
_LL2F5: return !_tmp45D  && _tmp45B;_LL2F6: _tmp45E=_tmp459.f1;if(_tmp45E <= (void*)4)
goto _LL2F8;if(*((int*)_tmp45E)!= 6)goto _LL2F8;_tmp45F=_tmp459.f2;if((int)_tmp45F
!= 1)goto _LL2F8;_LL2F7: goto _LL2F9;_LL2F8: _tmp460=_tmp459.f1;if(_tmp460 <= (void*)
4)goto _LL2FA;if(*((int*)_tmp460)!= 6)goto _LL2FA;_tmp461=_tmp459.f2;if(_tmp461 <= (
void*)4)goto _LL2FA;if(*((int*)_tmp461)!= 5)goto _LL2FA;_LL2F9: goto _LL2FB;_LL2FA:
_tmp462=_tmp459.f1;if(_tmp462 <= (void*)4)goto _LL2FC;if(*((int*)_tmp462)!= 6)goto
_LL2FC;_tmp463=_tmp459.f2;if(_tmp463 <= (void*)4)goto _LL2FC;if(*((int*)_tmp463)!= 
18)goto _LL2FC;_LL2FB: goto _LL2FD;_LL2FC: _tmp464=_tmp459.f1;if((int)_tmp464 != 1)
goto _LL2FE;_tmp465=_tmp459.f2;if(_tmp465 <= (void*)4)goto _LL2FE;if(*((int*)
_tmp465)!= 18)goto _LL2FE;_LL2FD: goto _LL2FF;_LL2FE: _tmp466=_tmp459.f1;if((int)
_tmp466 != 1)goto _LL300;_tmp467=_tmp459.f2;if(_tmp467 <= (void*)4)goto _LL300;if(*((
int*)_tmp467)!= 5)goto _LL300;_LL2FF: return 1;_LL300: _tmp468=_tmp459.f1;if(_tmp468
<= (void*)4)goto _LL302;if(*((int*)_tmp468)!= 5)goto _LL302;_tmp469=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp468)->f2;if((int)_tmp469 != 4)goto _LL302;
_tmp46A=_tmp459.f2;if(_tmp46A <= (void*)4)goto _LL302;if(*((int*)_tmp46A)!= 5)goto
_LL302;_tmp46B=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp46A)->f2;if((int)
_tmp46B != 4)goto _LL302;_LL301: return 0;_LL302: _tmp46C=_tmp459.f1;if(_tmp46C <= (
void*)4)goto _LL304;if(*((int*)_tmp46C)!= 5)goto _LL304;_tmp46D=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp46C)->f2;if((int)_tmp46D != 4)goto _LL304;_LL303: return 1;_LL304: _tmp46E=
_tmp459.f1;if(_tmp46E <= (void*)4)goto _LL306;if(*((int*)_tmp46E)!= 5)goto _LL306;
_tmp46F=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp46E)->f2;if((int)_tmp46F != 
3)goto _LL306;_tmp470=_tmp459.f2;if(_tmp470 <= (void*)4)goto _LL306;if(*((int*)
_tmp470)!= 5)goto _LL306;_tmp471=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp470)->f2;
if((int)_tmp471 != 2)goto _LL306;_LL305: goto _LL307;_LL306: _tmp472=_tmp459.f1;if(
_tmp472 <= (void*)4)goto _LL308;if(*((int*)_tmp472)!= 5)goto _LL308;_tmp473=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp472)->f2;if((int)_tmp473 != 2)goto _LL308;
_tmp474=_tmp459.f2;if(_tmp474 <= (void*)4)goto _LL308;if(*((int*)_tmp474)!= 5)goto
_LL308;_tmp475=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp474)->f2;if((int)
_tmp475 != 3)goto _LL308;_LL307: return 0;_LL308: _tmp476=_tmp459.f1;if(_tmp476 <= (
void*)4)goto _LL30A;if(*((int*)_tmp476)!= 5)goto _LL30A;_tmp477=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp476)->f2;if((int)_tmp477 != 3)goto _LL30A;_tmp478=_tmp459.f2;if((int)_tmp478 != 
1)goto _LL30A;_LL309: goto _LL30B;_LL30A: _tmp479=_tmp459.f1;if(_tmp479 <= (void*)4)
goto _LL30C;if(*((int*)_tmp479)!= 5)goto _LL30C;_tmp47A=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp479)->f2;if((int)_tmp47A != 2)goto _LL30C;_tmp47B=_tmp459.f2;if((int)_tmp47B != 
1)goto _LL30C;_LL30B: goto _LL30D;_LL30C: _tmp47C=_tmp459.f1;if(_tmp47C <= (void*)4)
goto _LL30E;if(*((int*)_tmp47C)!= 5)goto _LL30E;_tmp47D=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp47C)->f2;if((int)_tmp47D != 3)goto _LL30E;_tmp47E=_tmp459.f2;if(_tmp47E <= (
void*)4)goto _LL30E;if(*((int*)_tmp47E)!= 5)goto _LL30E;_tmp47F=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp47E)->f2;if((int)_tmp47F != 1)goto _LL30E;_LL30D: goto _LL30F;_LL30E: _tmp480=
_tmp459.f1;if(_tmp480 <= (void*)4)goto _LL310;if(*((int*)_tmp480)!= 5)goto _LL310;
_tmp481=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp480)->f2;if((int)_tmp481 != 
2)goto _LL310;_tmp482=_tmp459.f2;if(_tmp482 <= (void*)4)goto _LL310;if(*((int*)
_tmp482)!= 5)goto _LL310;_tmp483=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp482)->f2;
if((int)_tmp483 != 1)goto _LL310;_LL30F: goto _LL311;_LL310: _tmp484=_tmp459.f1;if(
_tmp484 <= (void*)4)goto _LL312;if(*((int*)_tmp484)!= 18)goto _LL312;_tmp485=
_tmp459.f2;if(_tmp485 <= (void*)4)goto _LL312;if(*((int*)_tmp485)!= 5)goto _LL312;
_tmp486=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp485)->f2;if((int)_tmp486 != 
1)goto _LL312;_LL311: goto _LL313;_LL312: _tmp487=_tmp459.f1;if(_tmp487 <= (void*)4)
goto _LL314;if(*((int*)_tmp487)!= 5)goto _LL314;_tmp488=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp487)->f2;if((int)_tmp488 != 3)goto _LL314;_tmp489=_tmp459.f2;if(_tmp489 <= (
void*)4)goto _LL314;if(*((int*)_tmp489)!= 5)goto _LL314;_tmp48A=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp489)->f2;if((int)_tmp48A != 0)goto _LL314;_LL313: goto _LL315;_LL314: _tmp48B=
_tmp459.f1;if(_tmp48B <= (void*)4)goto _LL316;if(*((int*)_tmp48B)!= 5)goto _LL316;
_tmp48C=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp48B)->f2;if((int)_tmp48C != 
2)goto _LL316;_tmp48D=_tmp459.f2;if(_tmp48D <= (void*)4)goto _LL316;if(*((int*)
_tmp48D)!= 5)goto _LL316;_tmp48E=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp48D)->f2;
if((int)_tmp48E != 0)goto _LL316;_LL315: goto _LL317;_LL316: _tmp48F=_tmp459.f1;if(
_tmp48F <= (void*)4)goto _LL318;if(*((int*)_tmp48F)!= 5)goto _LL318;_tmp490=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp48F)->f2;if((int)_tmp490 != 1)goto _LL318;
_tmp491=_tmp459.f2;if(_tmp491 <= (void*)4)goto _LL318;if(*((int*)_tmp491)!= 5)goto
_LL318;_tmp492=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp491)->f2;if((int)
_tmp492 != 0)goto _LL318;_LL317: goto _LL319;_LL318: _tmp493=_tmp459.f1;if(_tmp493 <= (
void*)4)goto _LL31A;if(*((int*)_tmp493)!= 18)goto _LL31A;_tmp494=_tmp459.f2;if(
_tmp494 <= (void*)4)goto _LL31A;if(*((int*)_tmp494)!= 5)goto _LL31A;_tmp495=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp494)->f2;if((int)_tmp495 != 0)goto _LL31A;
_LL319: return 1;_LL31A:;_LL31B: return 0;_LL2F3:;}}int Cyc_Tcutil_coerce_list(struct
Cyc_Tcenv_Tenv*te,void*t,struct Cyc_List_List*es){struct _RegionHandle*_tmp496=Cyc_Tcenv_get_fnrgn(
te);{struct Cyc_Core_Opt*max_arith_type=0;{struct Cyc_List_List*el=es;for(0;el != 0;
el=el->tl){void*t1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(((
struct Cyc_Absyn_Exp*)el->hd)->topt))->v);if(Cyc_Tcutil_is_arithmetic_type(t1)){
if(max_arith_type == 0  || Cyc_Tcutil_will_lose_precision(t1,(void*)max_arith_type->v))
max_arith_type=({struct Cyc_Core_Opt*_tmp497=_region_malloc(_tmp496,sizeof(*
_tmp497));_tmp497->v=(void*)t1;_tmp497;});}}}if(max_arith_type != 0){if(!Cyc_Tcutil_unify(
t,(void*)max_arith_type->v))return 0;}}{struct Cyc_List_List*el=es;for(0;el != 0;el=
el->tl){if(!Cyc_Tcutil_coerce_assign(te,(struct Cyc_Absyn_Exp*)el->hd,t)){({
struct Cyc_String_pa_struct _tmp49B;_tmp49B.tag=0;_tmp49B.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt))->v));{struct Cyc_String_pa_struct
_tmp49A;_tmp49A.tag=0;_tmp49A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t));{void*_tmp498[2]={& _tmp49A,& _tmp49B};Cyc_Tcutil_terr(((
struct Cyc_Absyn_Exp*)el->hd)->loc,({const char*_tmp499="type mismatch: expecting %s but found %s";
_tag_dyneither(_tmp499,sizeof(char),41);}),_tag_dyneither(_tmp498,sizeof(void*),
2));}}});return 0;}}}return 1;}int Cyc_Tcutil_coerce_to_bool(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e){if(!Cyc_Tcutil_coerce_sint_typ(te,e)){void*_tmp49C=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);_LL31D: if(_tmp49C <= (void*)
4)goto _LL31F;if(*((int*)_tmp49C)!= 4)goto _LL31F;_LL31E: Cyc_Tcutil_unchecked_cast(
te,e,Cyc_Absyn_uint_typ,(void*)3);goto _LL31C;_LL31F:;_LL320: return 0;_LL31C:;}
return 1;}int Cyc_Tcutil_is_integral_type(void*t){void*_tmp49D=Cyc_Tcutil_compress(
t);_LL322: if(_tmp49D <= (void*)4)goto _LL32A;if(*((int*)_tmp49D)!= 5)goto _LL324;
_LL323: goto _LL325;_LL324: if(*((int*)_tmp49D)!= 18)goto _LL326;_LL325: goto _LL327;
_LL326: if(*((int*)_tmp49D)!= 12)goto _LL328;_LL327: goto _LL329;_LL328: if(*((int*)
_tmp49D)!= 13)goto _LL32A;_LL329: return 1;_LL32A:;_LL32B: return 0;_LL321:;}int Cyc_Tcutil_coerce_uint_typ(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){if(Cyc_Tcutil_unify((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_Absyn_uint_typ))return 1;if(Cyc_Tcutil_is_integral_type((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)){if(Cyc_Tcutil_will_lose_precision((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_Absyn_uint_typ))({void*
_tmp49E=0;Cyc_Tcutil_warn(e->loc,({const char*_tmp49F="integral size mismatch; conversion supplied";
_tag_dyneither(_tmp49F,sizeof(char),44);}),_tag_dyneither(_tmp49E,sizeof(void*),
0));});Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_uint_typ,(void*)1);return 1;}
return 0;}int Cyc_Tcutil_coerce_sint_typ(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*
e){if(Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_Absyn_sint_typ))
return 1;if(Cyc_Tcutil_is_integral_type((void*)((struct Cyc_Core_Opt*)_check_null(
e->topt))->v)){if(Cyc_Tcutil_will_lose_precision((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,Cyc_Absyn_sint_typ))({void*_tmp4A0=0;Cyc_Tcutil_warn(e->loc,({
const char*_tmp4A1="integral size mismatch; conversion supplied";_tag_dyneither(
_tmp4A1,sizeof(char),44);}),_tag_dyneither(_tmp4A0,sizeof(void*),0));});Cyc_Tcutil_unchecked_cast(
te,e,Cyc_Absyn_sint_typ,(void*)1);return 1;}return 0;}int Cyc_Tcutil_silent_castable(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void*t1,void*t2){t1=Cyc_Tcutil_compress(
t1);t2=Cyc_Tcutil_compress(t2);{struct _tuple0 _tmp4A3=({struct _tuple0 _tmp4A2;
_tmp4A2.f1=t1;_tmp4A2.f2=t2;_tmp4A2;});void*_tmp4A4;struct Cyc_Absyn_PtrInfo
_tmp4A5;void*_tmp4A6;struct Cyc_Absyn_PtrInfo _tmp4A7;void*_tmp4A8;struct Cyc_Absyn_DatatypeInfo
_tmp4A9;union Cyc_Absyn_DatatypeInfoU _tmp4AA;struct Cyc_Absyn_Datatypedecl**
_tmp4AB;struct Cyc_Absyn_Datatypedecl*_tmp4AC;struct Cyc_List_List*_tmp4AD;struct
Cyc_Core_Opt*_tmp4AE;struct Cyc_Core_Opt _tmp4AF;void*_tmp4B0;void*_tmp4B1;struct
Cyc_Absyn_DatatypeInfo _tmp4B2;union Cyc_Absyn_DatatypeInfoU _tmp4B3;struct Cyc_Absyn_Datatypedecl**
_tmp4B4;struct Cyc_Absyn_Datatypedecl*_tmp4B5;struct Cyc_List_List*_tmp4B6;struct
Cyc_Core_Opt*_tmp4B7;struct Cyc_Core_Opt _tmp4B8;void*_tmp4B9;void*_tmp4BA;struct
Cyc_Absyn_ArrayInfo _tmp4BB;void*_tmp4BC;struct Cyc_Absyn_Tqual _tmp4BD;struct Cyc_Absyn_Exp*
_tmp4BE;union Cyc_Absyn_Constraint*_tmp4BF;void*_tmp4C0;struct Cyc_Absyn_ArrayInfo
_tmp4C1;void*_tmp4C2;struct Cyc_Absyn_Tqual _tmp4C3;struct Cyc_Absyn_Exp*_tmp4C4;
union Cyc_Absyn_Constraint*_tmp4C5;void*_tmp4C6;struct Cyc_Absyn_DatatypeFieldInfo
_tmp4C7;union Cyc_Absyn_DatatypeFieldInfoU _tmp4C8;struct _tuple3 _tmp4C9;struct Cyc_Absyn_Datatypedecl*
_tmp4CA;struct Cyc_Absyn_Datatypefield*_tmp4CB;struct Cyc_List_List*_tmp4CC;void*
_tmp4CD;struct Cyc_Absyn_DatatypeInfo _tmp4CE;union Cyc_Absyn_DatatypeInfoU _tmp4CF;
struct Cyc_Absyn_Datatypedecl**_tmp4D0;struct Cyc_Absyn_Datatypedecl*_tmp4D1;
struct Cyc_List_List*_tmp4D2;void*_tmp4D3;struct Cyc_Absyn_PtrInfo _tmp4D4;void*
_tmp4D5;struct Cyc_Absyn_Tqual _tmp4D6;struct Cyc_Absyn_PtrAtts _tmp4D7;void*_tmp4D8;
union Cyc_Absyn_Constraint*_tmp4D9;union Cyc_Absyn_Constraint*_tmp4DA;union Cyc_Absyn_Constraint*
_tmp4DB;void*_tmp4DC;struct Cyc_Absyn_DatatypeInfo _tmp4DD;union Cyc_Absyn_DatatypeInfoU
_tmp4DE;struct Cyc_Absyn_Datatypedecl**_tmp4DF;struct Cyc_Absyn_Datatypedecl*
_tmp4E0;struct Cyc_List_List*_tmp4E1;struct Cyc_Core_Opt*_tmp4E2;void*_tmp4E3;void*
_tmp4E4;_LL32D: _tmp4A4=_tmp4A3.f1;if(_tmp4A4 <= (void*)4)goto _LL32F;if(*((int*)
_tmp4A4)!= 4)goto _LL32F;_tmp4A5=((struct Cyc_Absyn_PointerType_struct*)_tmp4A4)->f1;
_tmp4A6=_tmp4A3.f2;if(_tmp4A6 <= (void*)4)goto _LL32F;if(*((int*)_tmp4A6)!= 4)goto
_LL32F;_tmp4A7=((struct Cyc_Absyn_PointerType_struct*)_tmp4A6)->f1;_LL32E: {int
okay=1;if(!((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*
y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,(_tmp4A5.ptr_atts).nullable,(
_tmp4A7.ptr_atts).nullable))okay=!((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
0,(_tmp4A5.ptr_atts).nullable);if(!Cyc_Tcutil_unify_conrefs(Cyc_Tcutil_unify_it_bounds,(
_tmp4A5.ptr_atts).bounds,(_tmp4A7.ptr_atts).bounds)){struct _tuple0 _tmp4E6=({
struct _tuple0 _tmp4E5;_tmp4E5.f1=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,(
_tmp4A5.ptr_atts).bounds);_tmp4E5.f2=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,(
_tmp4A7.ptr_atts).bounds);_tmp4E5;});void*_tmp4E7;void*_tmp4E8;void*_tmp4E9;void*
_tmp4EA;void*_tmp4EB;struct Cyc_Absyn_Exp*_tmp4EC;void*_tmp4ED;struct Cyc_Absyn_Exp*
_tmp4EE;void*_tmp4EF;void*_tmp4F0;struct Cyc_Absyn_Exp*_tmp4F1;_LL33C: _tmp4E7=
_tmp4E6.f1;if(_tmp4E7 <= (void*)1)goto _LL33E;if(*((int*)_tmp4E7)!= 0)goto _LL33E;
_tmp4E8=_tmp4E6.f2;if((int)_tmp4E8 != 0)goto _LL33E;_LL33D: goto _LL33F;_LL33E:
_tmp4E9=_tmp4E6.f1;if((int)_tmp4E9 != 0)goto _LL340;_tmp4EA=_tmp4E6.f2;if((int)
_tmp4EA != 0)goto _LL340;_LL33F: okay=1;goto _LL33B;_LL340: _tmp4EB=_tmp4E6.f1;if(
_tmp4EB <= (void*)1)goto _LL342;if(*((int*)_tmp4EB)!= 0)goto _LL342;_tmp4EC=((
struct Cyc_Absyn_Upper_b_struct*)_tmp4EB)->f1;_tmp4ED=_tmp4E6.f2;if(_tmp4ED <= (
void*)1)goto _LL342;if(*((int*)_tmp4ED)!= 0)goto _LL342;_tmp4EE=((struct Cyc_Absyn_Upper_b_struct*)
_tmp4ED)->f1;_LL341: okay=okay  && Cyc_Evexp_lte_const_exp(_tmp4EE,_tmp4EC);if(!((
int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp4A7.ptr_atts).zero_term))({
void*_tmp4F2=0;Cyc_Tcutil_warn(loc,({const char*_tmp4F3="implicit cast to shorter array";
_tag_dyneither(_tmp4F3,sizeof(char),31);}),_tag_dyneither(_tmp4F2,sizeof(void*),
0));});goto _LL33B;_LL342: _tmp4EF=_tmp4E6.f1;if((int)_tmp4EF != 0)goto _LL33B;
_tmp4F0=_tmp4E6.f2;if(_tmp4F0 <= (void*)1)goto _LL33B;if(*((int*)_tmp4F0)!= 0)goto
_LL33B;_tmp4F1=((struct Cyc_Absyn_Upper_b_struct*)_tmp4F0)->f1;_LL343: if(((int(*)(
int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp4A5.ptr_atts).zero_term)
 && Cyc_Tcutil_is_bound_one((_tmp4A7.ptr_atts).bounds))goto _LL33B;okay=0;goto
_LL33B;_LL33B:;}okay=okay  && Cyc_Tcutil_unify(_tmp4A5.elt_typ,_tmp4A7.elt_typ);
okay=okay  && (Cyc_Tcutil_unify((_tmp4A5.ptr_atts).rgn,(_tmp4A7.ptr_atts).rgn)
 || Cyc_Tcenv_region_outlives(te,(_tmp4A5.ptr_atts).rgn,(_tmp4A7.ptr_atts).rgn));
okay=okay  && (!(_tmp4A5.elt_tq).real_const  || (_tmp4A7.elt_tq).real_const);okay=
okay  && (((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*
y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,(_tmp4A5.ptr_atts).zero_term,(
_tmp4A7.ptr_atts).zero_term) || ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
1,(_tmp4A5.ptr_atts).zero_term) && (_tmp4A7.elt_tq).real_const);return okay;}
_LL32F: _tmp4A8=_tmp4A3.f1;if(_tmp4A8 <= (void*)4)goto _LL331;if(*((int*)_tmp4A8)!= 
2)goto _LL331;_tmp4A9=((struct Cyc_Absyn_DatatypeType_struct*)_tmp4A8)->f1;_tmp4AA=
_tmp4A9.datatype_info;if((_tmp4AA.KnownDatatype).tag != 2)goto _LL331;_tmp4AB=(
struct Cyc_Absyn_Datatypedecl**)(_tmp4AA.KnownDatatype).val;_tmp4AC=*_tmp4AB;
_tmp4AD=_tmp4A9.targs;_tmp4AE=_tmp4A9.rgn;if(_tmp4AE == 0)goto _LL331;_tmp4AF=*
_tmp4AE;_tmp4B0=(void*)_tmp4AF.v;_tmp4B1=_tmp4A3.f2;if(_tmp4B1 <= (void*)4)goto
_LL331;if(*((int*)_tmp4B1)!= 2)goto _LL331;_tmp4B2=((struct Cyc_Absyn_DatatypeType_struct*)
_tmp4B1)->f1;_tmp4B3=_tmp4B2.datatype_info;if((_tmp4B3.KnownDatatype).tag != 2)
goto _LL331;_tmp4B4=(struct Cyc_Absyn_Datatypedecl**)(_tmp4B3.KnownDatatype).val;
_tmp4B5=*_tmp4B4;_tmp4B6=_tmp4B2.targs;_tmp4B7=_tmp4B2.rgn;if(_tmp4B7 == 0)goto
_LL331;_tmp4B8=*_tmp4B7;_tmp4B9=(void*)_tmp4B8.v;_LL330: if(_tmp4AC != _tmp4B5  || 
!Cyc_Tcenv_region_outlives(te,_tmp4B0,_tmp4B9))return 0;for(0;_tmp4AD != 0  && 
_tmp4B6 != 0;(_tmp4AD=_tmp4AD->tl,_tmp4B6=_tmp4B6->tl)){if(!Cyc_Tcutil_unify((
void*)_tmp4AD->hd,(void*)_tmp4B6->hd))return 0;}if(_tmp4AD != 0  || _tmp4B6 != 0)
return 0;return 1;_LL331: _tmp4BA=_tmp4A3.f1;if(_tmp4BA <= (void*)4)goto _LL333;if(*((
int*)_tmp4BA)!= 7)goto _LL333;_tmp4BB=((struct Cyc_Absyn_ArrayType_struct*)_tmp4BA)->f1;
_tmp4BC=_tmp4BB.elt_type;_tmp4BD=_tmp4BB.tq;_tmp4BE=_tmp4BB.num_elts;_tmp4BF=
_tmp4BB.zero_term;_tmp4C0=_tmp4A3.f2;if(_tmp4C0 <= (void*)4)goto _LL333;if(*((int*)
_tmp4C0)!= 7)goto _LL333;_tmp4C1=((struct Cyc_Absyn_ArrayType_struct*)_tmp4C0)->f1;
_tmp4C2=_tmp4C1.elt_type;_tmp4C3=_tmp4C1.tq;_tmp4C4=_tmp4C1.num_elts;_tmp4C5=
_tmp4C1.zero_term;_LL332: {int okay;okay=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*
x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp4BF,
_tmp4C5) && ((_tmp4BE != 0  && _tmp4C4 != 0) && Cyc_Evexp_same_const_exp((struct Cyc_Absyn_Exp*)
_tmp4BE,(struct Cyc_Absyn_Exp*)_tmp4C4));return(okay  && Cyc_Tcutil_unify(_tmp4BC,
_tmp4C2)) && (!_tmp4BD.real_const  || _tmp4C3.real_const);}_LL333: _tmp4C6=_tmp4A3.f1;
if(_tmp4C6 <= (void*)4)goto _LL335;if(*((int*)_tmp4C6)!= 3)goto _LL335;_tmp4C7=((
struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp4C6)->f1;_tmp4C8=_tmp4C7.field_info;
if((_tmp4C8.KnownDatatypefield).tag != 2)goto _LL335;_tmp4C9=(struct _tuple3)(
_tmp4C8.KnownDatatypefield).val;_tmp4CA=_tmp4C9.f1;_tmp4CB=_tmp4C9.f2;_tmp4CC=
_tmp4C7.targs;_tmp4CD=_tmp4A3.f2;if(_tmp4CD <= (void*)4)goto _LL335;if(*((int*)
_tmp4CD)!= 2)goto _LL335;_tmp4CE=((struct Cyc_Absyn_DatatypeType_struct*)_tmp4CD)->f1;
_tmp4CF=_tmp4CE.datatype_info;if((_tmp4CF.KnownDatatype).tag != 2)goto _LL335;
_tmp4D0=(struct Cyc_Absyn_Datatypedecl**)(_tmp4CF.KnownDatatype).val;_tmp4D1=*
_tmp4D0;_tmp4D2=_tmp4CE.targs;_LL334: if((_tmp4CA == _tmp4D1  || Cyc_Absyn_qvar_cmp(
_tmp4CA->name,_tmp4D1->name)== 0) && _tmp4CB->typs == 0){for(0;_tmp4CC != 0  && 
_tmp4D2 != 0;(_tmp4CC=_tmp4CC->tl,_tmp4D2=_tmp4D2->tl)){if(!Cyc_Tcutil_unify((
void*)_tmp4CC->hd,(void*)_tmp4D2->hd))break;}if(_tmp4CC == 0  && _tmp4D2 == 0)
return 1;}return 0;_LL335: _tmp4D3=_tmp4A3.f1;if(_tmp4D3 <= (void*)4)goto _LL337;if(*((
int*)_tmp4D3)!= 4)goto _LL337;_tmp4D4=((struct Cyc_Absyn_PointerType_struct*)
_tmp4D3)->f1;_tmp4D5=_tmp4D4.elt_typ;_tmp4D6=_tmp4D4.elt_tq;_tmp4D7=_tmp4D4.ptr_atts;
_tmp4D8=_tmp4D7.rgn;_tmp4D9=_tmp4D7.nullable;_tmp4DA=_tmp4D7.bounds;_tmp4DB=
_tmp4D7.zero_term;_tmp4DC=_tmp4A3.f2;if(_tmp4DC <= (void*)4)goto _LL337;if(*((int*)
_tmp4DC)!= 2)goto _LL337;_tmp4DD=((struct Cyc_Absyn_DatatypeType_struct*)_tmp4DC)->f1;
_tmp4DE=_tmp4DD.datatype_info;if((_tmp4DE.KnownDatatype).tag != 2)goto _LL337;
_tmp4DF=(struct Cyc_Absyn_Datatypedecl**)(_tmp4DE.KnownDatatype).val;_tmp4E0=*
_tmp4DF;_tmp4E1=_tmp4DD.targs;_tmp4E2=_tmp4DD.rgn;_LL336:{void*_tmp4F4=Cyc_Tcutil_compress(
_tmp4D5);struct Cyc_Absyn_DatatypeFieldInfo _tmp4F5;union Cyc_Absyn_DatatypeFieldInfoU
_tmp4F6;struct _tuple3 _tmp4F7;struct Cyc_Absyn_Datatypedecl*_tmp4F8;struct Cyc_Absyn_Datatypefield*
_tmp4F9;struct Cyc_List_List*_tmp4FA;_LL345: if(_tmp4F4 <= (void*)4)goto _LL347;if(*((
int*)_tmp4F4)!= 3)goto _LL347;_tmp4F5=((struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp4F4)->f1;_tmp4F6=_tmp4F5.field_info;if((_tmp4F6.KnownDatatypefield).tag != 2)
goto _LL347;_tmp4F7=(struct _tuple3)(_tmp4F6.KnownDatatypefield).val;_tmp4F8=
_tmp4F7.f1;_tmp4F9=_tmp4F7.f2;_tmp4FA=_tmp4F5.targs;_LL346: if(!Cyc_Tcutil_unify(
_tmp4D8,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp4E2))->v) && !Cyc_Tcenv_region_outlives(
te,_tmp4D8,(void*)_tmp4E2->v))return 0;if(!((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*
x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp4D9,
Cyc_Absyn_false_conref))return 0;if(!Cyc_Tcutil_unify_conrefs(Cyc_Tcutil_unify_it_bounds,
_tmp4DA,Cyc_Absyn_bounds_one_conref))return 0;if(!((int(*)(int(*cmp)(int,int),
union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(
Cyc_Core_intcmp,_tmp4DB,Cyc_Absyn_false_conref))return 0;if(Cyc_Absyn_qvar_cmp(
_tmp4E0->name,_tmp4F8->name)== 0  && _tmp4F9->typs != 0){int okay=1;for(0;_tmp4FA != 
0  && _tmp4E1 != 0;(_tmp4FA=_tmp4FA->tl,_tmp4E1=_tmp4E1->tl)){if(!Cyc_Tcutil_unify((
void*)_tmp4FA->hd,(void*)_tmp4E1->hd)){okay=0;break;}}if((!okay  || _tmp4FA != 0)
 || _tmp4E1 != 0)return 0;return 1;}goto _LL344;_LL347:;_LL348: goto _LL344;_LL344:;}
return 0;_LL337: _tmp4E3=_tmp4A3.f1;if(_tmp4E3 <= (void*)4)goto _LL339;if(*((int*)
_tmp4E3)!= 18)goto _LL339;_tmp4E4=_tmp4A3.f2;if(_tmp4E4 <= (void*)4)goto _LL339;if(*((
int*)_tmp4E4)!= 5)goto _LL339;_LL338: return 0;_LL339:;_LL33A: return Cyc_Tcutil_unify(
t1,t2);_LL32C:;}}int Cyc_Tcutil_is_pointer_type(void*t){void*_tmp4FB=Cyc_Tcutil_compress(
t);_LL34A: if(_tmp4FB <= (void*)4)goto _LL34C;if(*((int*)_tmp4FB)!= 4)goto _LL34C;
_LL34B: return 1;_LL34C:;_LL34D: return 0;_LL349:;}int Cyc_Tcutil_rgn_of_pointer(void*
t,void**rgn){void*_tmp4FC=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp4FD;
struct Cyc_Absyn_PtrAtts _tmp4FE;void*_tmp4FF;_LL34F: if(_tmp4FC <= (void*)4)goto
_LL351;if(*((int*)_tmp4FC)!= 4)goto _LL351;_tmp4FD=((struct Cyc_Absyn_PointerType_struct*)
_tmp4FC)->f1;_tmp4FE=_tmp4FD.ptr_atts;_tmp4FF=_tmp4FE.rgn;_LL350:*rgn=_tmp4FF;
return 1;_LL351:;_LL352: return 0;_LL34E:;}int Cyc_Tcutil_is_pointer_or_boxed(void*t,
int*is_dyneither_ptr){void*_tmp500=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo
_tmp501;struct Cyc_Absyn_PtrAtts _tmp502;union Cyc_Absyn_Constraint*_tmp503;_LL354:
if(_tmp500 <= (void*)4)goto _LL356;if(*((int*)_tmp500)!= 4)goto _LL356;_tmp501=((
struct Cyc_Absyn_PointerType_struct*)_tmp500)->f1;_tmp502=_tmp501.ptr_atts;
_tmp503=_tmp502.bounds;_LL355:*is_dyneither_ptr=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,
_tmp503)== (void*)0;return 1;_LL356:;_LL357: return Cyc_Tcutil_typ_kind(t)== (void*)
2;_LL353:;}int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*e){void*_tmp504=e->r;union
Cyc_Absyn_Cnst _tmp505;struct _tuple7 _tmp506;int _tmp507;union Cyc_Absyn_Cnst _tmp508;
struct _tuple5 _tmp509;char _tmp50A;union Cyc_Absyn_Cnst _tmp50B;struct _tuple6 _tmp50C;
short _tmp50D;union Cyc_Absyn_Cnst _tmp50E;struct _tuple8 _tmp50F;long long _tmp510;
void*_tmp511;struct Cyc_Absyn_Exp*_tmp512;_LL359: if(*((int*)_tmp504)!= 0)goto
_LL35B;_tmp505=((struct Cyc_Absyn_Const_e_struct*)_tmp504)->f1;if((_tmp505.Int_c).tag
!= 4)goto _LL35B;_tmp506=(struct _tuple7)(_tmp505.Int_c).val;_tmp507=_tmp506.f2;
if(_tmp507 != 0)goto _LL35B;_LL35A: goto _LL35C;_LL35B: if(*((int*)_tmp504)!= 0)goto
_LL35D;_tmp508=((struct Cyc_Absyn_Const_e_struct*)_tmp504)->f1;if((_tmp508.Char_c).tag
!= 2)goto _LL35D;_tmp509=(struct _tuple5)(_tmp508.Char_c).val;_tmp50A=_tmp509.f2;
if(_tmp50A != 0)goto _LL35D;_LL35C: goto _LL35E;_LL35D: if(*((int*)_tmp504)!= 0)goto
_LL35F;_tmp50B=((struct Cyc_Absyn_Const_e_struct*)_tmp504)->f1;if((_tmp50B.Short_c).tag
!= 3)goto _LL35F;_tmp50C=(struct _tuple6)(_tmp50B.Short_c).val;_tmp50D=_tmp50C.f2;
if(_tmp50D != 0)goto _LL35F;_LL35E: goto _LL360;_LL35F: if(*((int*)_tmp504)!= 0)goto
_LL361;_tmp50E=((struct Cyc_Absyn_Const_e_struct*)_tmp504)->f1;if((_tmp50E.LongLong_c).tag
!= 5)goto _LL361;_tmp50F=(struct _tuple8)(_tmp50E.LongLong_c).val;_tmp510=_tmp50F.f2;
if(_tmp510 != 0)goto _LL361;_LL360: return 1;_LL361: if(*((int*)_tmp504)!= 15)goto
_LL363;_tmp511=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp504)->f1;_tmp512=((
struct Cyc_Absyn_Cast_e_struct*)_tmp504)->f2;_LL362: return Cyc_Tcutil_is_zero(
_tmp512) && Cyc_Tcutil_admits_zero(_tmp511);_LL363:;_LL364: return 0;_LL358:;}
struct Cyc_Core_Opt Cyc_Tcutil_trk={(void*)((void*)5)};struct Cyc_Core_Opt Cyc_Tcutil_urk={(
void*)((void*)4)};struct Cyc_Core_Opt Cyc_Tcutil_rk={(void*)((void*)3)};struct Cyc_Core_Opt
Cyc_Tcutil_ak={(void*)((void*)0)};struct Cyc_Core_Opt Cyc_Tcutil_bk={(void*)((void*)
2)};struct Cyc_Core_Opt Cyc_Tcutil_mk={(void*)((void*)1)};struct Cyc_Core_Opt Cyc_Tcutil_ik={(
void*)((void*)7)};struct Cyc_Core_Opt Cyc_Tcutil_ek={(void*)((void*)6)};struct Cyc_Core_Opt*
Cyc_Tcutil_kind_to_opt(void*k){void*_tmp513=k;_LL366: if((int)_tmp513 != 0)goto
_LL368;_LL367: return(struct Cyc_Core_Opt*)& Cyc_Tcutil_ak;_LL368: if((int)_tmp513 != 
1)goto _LL36A;_LL369: return(struct Cyc_Core_Opt*)& Cyc_Tcutil_mk;_LL36A: if((int)
_tmp513 != 2)goto _LL36C;_LL36B: return(struct Cyc_Core_Opt*)& Cyc_Tcutil_bk;_LL36C:
if((int)_tmp513 != 3)goto _LL36E;_LL36D: return(struct Cyc_Core_Opt*)& Cyc_Tcutil_rk;
_LL36E: if((int)_tmp513 != 4)goto _LL370;_LL36F: return(struct Cyc_Core_Opt*)& Cyc_Tcutil_urk;
_LL370: if((int)_tmp513 != 5)goto _LL372;_LL371: return(struct Cyc_Core_Opt*)& Cyc_Tcutil_trk;
_LL372: if((int)_tmp513 != 6)goto _LL374;_LL373: return(struct Cyc_Core_Opt*)& Cyc_Tcutil_ek;
_LL374: if((int)_tmp513 != 7)goto _LL365;_LL375: return(struct Cyc_Core_Opt*)& Cyc_Tcutil_ik;
_LL365:;}static void**Cyc_Tcutil_kind_to_b(void*k){static struct Cyc_Absyn_Eq_kb_struct
ab_v={0,(void*)((void*)0)};static struct Cyc_Absyn_Eq_kb_struct mb_v={0,(void*)((
void*)1)};static struct Cyc_Absyn_Eq_kb_struct bb_v={0,(void*)((void*)2)};static
struct Cyc_Absyn_Eq_kb_struct rb_v={0,(void*)((void*)3)};static struct Cyc_Absyn_Eq_kb_struct
ub_v={0,(void*)((void*)4)};static struct Cyc_Absyn_Eq_kb_struct tb_v={0,(void*)((
void*)5)};static struct Cyc_Absyn_Eq_kb_struct eb_v={0,(void*)((void*)6)};static
struct Cyc_Absyn_Eq_kb_struct ib_v={0,(void*)((void*)7)};static void*ab=(void*)&
ab_v;static void*mb=(void*)& mb_v;static void*bb=(void*)& bb_v;static void*rb=(void*)&
rb_v;static void*ub=(void*)& ub_v;static void*tb=(void*)& tb_v;static void*eb=(void*)&
eb_v;static void*ib=(void*)& ib_v;void*_tmp514=k;_LL377: if((int)_tmp514 != 0)goto
_LL379;_LL378: return& ab;_LL379: if((int)_tmp514 != 1)goto _LL37B;_LL37A: return& mb;
_LL37B: if((int)_tmp514 != 2)goto _LL37D;_LL37C: return& bb;_LL37D: if((int)_tmp514 != 
3)goto _LL37F;_LL37E: return& rb;_LL37F: if((int)_tmp514 != 4)goto _LL381;_LL380:
return& ub;_LL381: if((int)_tmp514 != 5)goto _LL383;_LL382: return& tb;_LL383: if((int)
_tmp514 != 6)goto _LL385;_LL384: return& eb;_LL385: if((int)_tmp514 != 7)goto _LL376;
_LL386: return& ib;_LL376:;}void*Cyc_Tcutil_kind_to_bound(void*k){return*Cyc_Tcutil_kind_to_b(
k);}struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_bound_opt(void*k){return(struct Cyc_Core_Opt*)
Cyc_Tcutil_kind_to_b(k);}int Cyc_Tcutil_zero_to_null(struct Cyc_Tcenv_Tenv*te,void*
t2,struct Cyc_Absyn_Exp*e1){if(Cyc_Tcutil_is_pointer_type(t2) && Cyc_Tcutil_is_zero(
e1)){e1->r=(void*)({struct Cyc_Absyn_Const_e_struct*_tmp51D=_cycalloc(sizeof(*
_tmp51D));_tmp51D[0]=({struct Cyc_Absyn_Const_e_struct _tmp51E;_tmp51E.tag=0;
_tmp51E.f1=Cyc_Absyn_Null_c;_tmp51E;});_tmp51D;});{struct Cyc_Core_Opt*_tmp51F=
Cyc_Tcenv_lookup_opt_type_vars(te);struct Cyc_Absyn_PointerType_struct*_tmp520=({
struct Cyc_Absyn_PointerType_struct*_tmp521=_cycalloc(sizeof(*_tmp521));_tmp521[0]=({
struct Cyc_Absyn_PointerType_struct _tmp522;_tmp522.tag=4;_tmp522.f1=({struct Cyc_Absyn_PtrInfo
_tmp523;_tmp523.elt_typ=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ak,
_tmp51F);_tmp523.elt_tq=Cyc_Absyn_empty_tqual(0);_tmp523.ptr_atts=({struct Cyc_Absyn_PtrAtts
_tmp524;_tmp524.rgn=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_trk,
_tmp51F);_tmp524.nullable=Cyc_Absyn_true_conref;_tmp524.bounds=Cyc_Absyn_empty_conref();
_tmp524.zero_term=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();
_tmp524.ptrloc=0;_tmp524;});_tmp523;});_tmp522;});_tmp521;});((struct Cyc_Core_Opt*)
_check_null(e1->topt))->v=(void*)((void*)_tmp520);return Cyc_Tcutil_coerce_arg(te,
e1,t2);}}return 0;}struct _dyneither_ptr Cyc_Tcutil_coercion2string(void*c){void*
_tmp525=c;_LL388: if((int)_tmp525 != 0)goto _LL38A;_LL389: return({const char*_tmp526="unknown";
_tag_dyneither(_tmp526,sizeof(char),8);});_LL38A: if((int)_tmp525 != 1)goto _LL38C;
_LL38B: return({const char*_tmp527="no coercion";_tag_dyneither(_tmp527,sizeof(
char),12);});_LL38C: if((int)_tmp525 != 2)goto _LL38E;_LL38D: return({const char*
_tmp528="null check";_tag_dyneither(_tmp528,sizeof(char),11);});_LL38E: if((int)
_tmp525 != 3)goto _LL387;_LL38F: return({const char*_tmp529="other coercion";
_tag_dyneither(_tmp529,sizeof(char),15);});_LL387:;}int Cyc_Tcutil_coerce_arg(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t2){void*t1=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);void*c;if(Cyc_Tcutil_unify(
t1,t2))return 1;if(Cyc_Tcutil_is_arithmetic_type(t2) && Cyc_Tcutil_is_arithmetic_type(
t1)){if(Cyc_Tcutil_will_lose_precision(t1,t2))({struct Cyc_String_pa_struct
_tmp52D;_tmp52D.tag=0;_tmp52D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2));{struct Cyc_String_pa_struct _tmp52C;_tmp52C.tag=0;
_tmp52C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t1));{void*_tmp52A[2]={& _tmp52C,& _tmp52D};Cyc_Tcutil_warn(e->loc,({const char*
_tmp52B="integral size mismatch; %s -> %s conversion supplied";_tag_dyneither(
_tmp52B,sizeof(char),53);}),_tag_dyneither(_tmp52A,sizeof(void*),2));}}});Cyc_Tcutil_unchecked_cast(
te,e,t2,(void*)1);return 1;}else{if(Cyc_Tcutil_silent_castable(te,e->loc,t1,t2)){
Cyc_Tcutil_unchecked_cast(te,e,t2,(void*)3);return 1;}else{if(Cyc_Tcutil_zero_to_null(
te,t2,e))return 1;else{if((c=Cyc_Tcutil_castable(te,e->loc,t1,t2))!= (void*)0){
Cyc_Tcutil_unchecked_cast(te,e,t2,c);if(c != (void*)2)({struct Cyc_String_pa_struct
_tmp531;_tmp531.tag=0;_tmp531.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2));{struct Cyc_String_pa_struct _tmp530;_tmp530.tag=0;
_tmp530.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t1));{void*_tmp52E[2]={& _tmp530,& _tmp531};Cyc_Tcutil_warn(e->loc,({const char*
_tmp52F="implicit cast from %s to %s";_tag_dyneither(_tmp52F,sizeof(char),28);}),
_tag_dyneither(_tmp52E,sizeof(void*),2));}}});return 1;}else{return 0;}}}}}int Cyc_Tcutil_coerce_assign(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t){return Cyc_Tcutil_coerce_arg(
te,e,t);}int Cyc_Tcutil_coerceable(void*t){void*_tmp532=Cyc_Tcutil_compress(t);
_LL391: if(_tmp532 <= (void*)4)goto _LL393;if(*((int*)_tmp532)!= 5)goto _LL393;
_LL392: goto _LL394;_LL393: if((int)_tmp532 != 1)goto _LL395;_LL394: goto _LL396;_LL395:
if(_tmp532 <= (void*)4)goto _LL397;if(*((int*)_tmp532)!= 6)goto _LL397;_LL396:
return 1;_LL397:;_LL398: return 0;_LL390:;}static struct _tuple11*Cyc_Tcutil_flatten_typ_f(
struct _tuple15*env,struct Cyc_Absyn_Aggrfield*x){struct Cyc_List_List*_tmp534;
struct _RegionHandle*_tmp535;struct _tuple15 _tmp533=*env;_tmp534=_tmp533.f1;
_tmp535=_tmp533.f2;return({struct _tuple11*_tmp536=_region_malloc(_tmp535,sizeof(*
_tmp536));_tmp536->f1=x->tq;_tmp536->f2=Cyc_Tcutil_rsubstitute(_tmp535,_tmp534,x->type);
_tmp536;});}static struct _tuple11*Cyc_Tcutil_rcopy_tqt(struct _RegionHandle*r,
struct _tuple11*x){struct Cyc_Absyn_Tqual _tmp538;void*_tmp539;struct _tuple11
_tmp537=*x;_tmp538=_tmp537.f1;_tmp539=_tmp537.f2;return({struct _tuple11*_tmp53A=
_region_malloc(r,sizeof(*_tmp53A));_tmp53A->f1=_tmp538;_tmp53A->f2=_tmp539;
_tmp53A;});}static struct Cyc_List_List*Cyc_Tcutil_flatten_typ(struct _RegionHandle*
r,struct Cyc_Tcenv_Tenv*te,void*t1){t1=Cyc_Tcutil_compress(t1);{void*_tmp53B=t1;
struct Cyc_List_List*_tmp53C;struct Cyc_Absyn_AggrInfo _tmp53D;union Cyc_Absyn_AggrInfoU
_tmp53E;struct Cyc_Absyn_Aggrdecl**_tmp53F;struct Cyc_Absyn_Aggrdecl*_tmp540;
struct Cyc_List_List*_tmp541;void*_tmp542;struct Cyc_List_List*_tmp543;struct Cyc_Absyn_FnInfo
_tmp544;_LL39A: if((int)_tmp53B != 0)goto _LL39C;_LL39B: return 0;_LL39C: if(_tmp53B <= (
void*)4)goto _LL3A4;if(*((int*)_tmp53B)!= 9)goto _LL39E;_tmp53C=((struct Cyc_Absyn_TupleType_struct*)
_tmp53B)->f1;_LL39D: return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct
_tuple11*(*f)(struct _RegionHandle*,struct _tuple11*),struct _RegionHandle*env,
struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_rcopy_tqt,r,_tmp53C);_LL39E:
if(*((int*)_tmp53B)!= 10)goto _LL3A0;_tmp53D=((struct Cyc_Absyn_AggrType_struct*)
_tmp53B)->f1;_tmp53E=_tmp53D.aggr_info;if((_tmp53E.KnownAggr).tag != 2)goto _LL3A0;
_tmp53F=(struct Cyc_Absyn_Aggrdecl**)(_tmp53E.KnownAggr).val;_tmp540=*_tmp53F;
_tmp541=_tmp53D.targs;_LL39F: if(((_tmp540->kind == (void*)1  || _tmp540->impl == 0)
 || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp540->impl))->exist_vars != 0)
 || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp540->impl))->rgn_po != 0)
return({struct Cyc_List_List*_tmp545=_region_malloc(r,sizeof(*_tmp545));_tmp545->hd=({
struct _tuple11*_tmp546=_region_malloc(r,sizeof(*_tmp546));_tmp546->f1=Cyc_Absyn_empty_tqual(
0);_tmp546->f2=t1;_tmp546;});_tmp545->tl=0;_tmp545;});{struct Cyc_List_List*
_tmp547=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp540->tvs,
_tmp541);struct _tuple15 env=({struct _tuple15 _tmp548;_tmp548.f1=_tmp547;_tmp548.f2=
r;_tmp548;});return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple11*(*
f)(struct _tuple15*,struct Cyc_Absyn_Aggrfield*),struct _tuple15*env,struct Cyc_List_List*
x))Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_typ_f,& env,((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp540->impl))->fields);}_LL3A0: if(*((int*)_tmp53B)!= 11)goto _LL3A2;
_tmp542=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)_tmp53B)->f1;if((int)
_tmp542 != 0)goto _LL3A2;_tmp543=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp53B)->f2;
_LL3A1: {struct _tuple15 env=({struct _tuple15 _tmp549;_tmp549.f1=0;_tmp549.f2=r;
_tmp549;});return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple11*(*
f)(struct _tuple15*,struct Cyc_Absyn_Aggrfield*),struct _tuple15*env,struct Cyc_List_List*
x))Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_typ_f,& env,_tmp543);}_LL3A2: if(*((int*)
_tmp53B)!= 8)goto _LL3A4;_tmp544=((struct Cyc_Absyn_FnType_struct*)_tmp53B)->f1;
_LL3A3: return({struct Cyc_List_List*_tmp54A=_region_malloc(r,sizeof(*_tmp54A));
_tmp54A->hd=({struct _tuple11*_tmp54B=_region_malloc(r,sizeof(*_tmp54B));_tmp54B->f1=
Cyc_Absyn_const_tqual(0);_tmp54B->f2=t1;_tmp54B;});_tmp54A->tl=0;_tmp54A;});
_LL3A4:;_LL3A5: return({struct Cyc_List_List*_tmp54C=_region_malloc(r,sizeof(*
_tmp54C));_tmp54C->hd=({struct _tuple11*_tmp54D=_region_malloc(r,sizeof(*_tmp54D));
_tmp54D->f1=Cyc_Absyn_empty_tqual(0);_tmp54D->f2=t1;_tmp54D;});_tmp54C->tl=0;
_tmp54C;});_LL399:;}}static int Cyc_Tcutil_sub_attributes(struct Cyc_List_List*a1,
struct Cyc_List_List*a2){{struct Cyc_List_List*t=a1;for(0;t != 0;t=t->tl){void*
_tmp54E=(void*)t->hd;_LL3A7: if((int)_tmp54E != 16)goto _LL3A9;_LL3A8: goto _LL3AA;
_LL3A9: if((int)_tmp54E != 3)goto _LL3AB;_LL3AA: goto _LL3AC;_LL3AB: if(_tmp54E <= (
void*)17)goto _LL3AD;if(*((int*)_tmp54E)!= 4)goto _LL3AD;_LL3AC: continue;_LL3AD:;
_LL3AE: if(!Cyc_List_exists_c(Cyc_Tcutil_equal_att,(void*)t->hd,a2))return 0;
_LL3A6:;}}for(0;a2 != 0;a2=a2->tl){if(!Cyc_List_exists_c(Cyc_Tcutil_equal_att,(
void*)a2->hd,a1))return 0;}return 1;}static int Cyc_Tcutil_ptrsubtype(struct Cyc_Tcenv_Tenv*
te,struct Cyc_List_List*assume,void*t1,void*t2);static int Cyc_Tcutil_subtype(
struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2){if(Cyc_Tcutil_unify(
t1,t2))return 1;{struct Cyc_List_List*a=assume;for(0;a != 0;a=a->tl){if(Cyc_Tcutil_unify(
t1,(*((struct _tuple0*)a->hd)).f1) && Cyc_Tcutil_unify(t2,(*((struct _tuple0*)a->hd)).f2))
return 1;}}t1=Cyc_Tcutil_compress(t1);t2=Cyc_Tcutil_compress(t2);{struct _tuple0
_tmp550=({struct _tuple0 _tmp54F;_tmp54F.f1=t1;_tmp54F.f2=t2;_tmp54F;});void*
_tmp551;struct Cyc_Absyn_PtrInfo _tmp552;void*_tmp553;struct Cyc_Absyn_Tqual _tmp554;
struct Cyc_Absyn_PtrAtts _tmp555;void*_tmp556;union Cyc_Absyn_Constraint*_tmp557;
union Cyc_Absyn_Constraint*_tmp558;union Cyc_Absyn_Constraint*_tmp559;void*_tmp55A;
struct Cyc_Absyn_PtrInfo _tmp55B;void*_tmp55C;struct Cyc_Absyn_Tqual _tmp55D;struct
Cyc_Absyn_PtrAtts _tmp55E;void*_tmp55F;union Cyc_Absyn_Constraint*_tmp560;union Cyc_Absyn_Constraint*
_tmp561;union Cyc_Absyn_Constraint*_tmp562;void*_tmp563;struct Cyc_Absyn_DatatypeInfo
_tmp564;union Cyc_Absyn_DatatypeInfoU _tmp565;struct Cyc_Absyn_Datatypedecl**
_tmp566;struct Cyc_Absyn_Datatypedecl*_tmp567;struct Cyc_List_List*_tmp568;struct
Cyc_Core_Opt*_tmp569;struct Cyc_Core_Opt _tmp56A;void*_tmp56B;void*_tmp56C;struct
Cyc_Absyn_DatatypeInfo _tmp56D;union Cyc_Absyn_DatatypeInfoU _tmp56E;struct Cyc_Absyn_Datatypedecl**
_tmp56F;struct Cyc_Absyn_Datatypedecl*_tmp570;struct Cyc_List_List*_tmp571;struct
Cyc_Core_Opt*_tmp572;struct Cyc_Core_Opt _tmp573;void*_tmp574;void*_tmp575;struct
Cyc_Absyn_FnInfo _tmp576;void*_tmp577;struct Cyc_Absyn_FnInfo _tmp578;_LL3B0:
_tmp551=_tmp550.f1;if(_tmp551 <= (void*)4)goto _LL3B2;if(*((int*)_tmp551)!= 4)goto
_LL3B2;_tmp552=((struct Cyc_Absyn_PointerType_struct*)_tmp551)->f1;_tmp553=
_tmp552.elt_typ;_tmp554=_tmp552.elt_tq;_tmp555=_tmp552.ptr_atts;_tmp556=_tmp555.rgn;
_tmp557=_tmp555.nullable;_tmp558=_tmp555.bounds;_tmp559=_tmp555.zero_term;
_tmp55A=_tmp550.f2;if(_tmp55A <= (void*)4)goto _LL3B2;if(*((int*)_tmp55A)!= 4)goto
_LL3B2;_tmp55B=((struct Cyc_Absyn_PointerType_struct*)_tmp55A)->f1;_tmp55C=
_tmp55B.elt_typ;_tmp55D=_tmp55B.elt_tq;_tmp55E=_tmp55B.ptr_atts;_tmp55F=_tmp55E.rgn;
_tmp560=_tmp55E.nullable;_tmp561=_tmp55E.bounds;_tmp562=_tmp55E.zero_term;_LL3B1:
if(_tmp554.real_const  && !_tmp55D.real_const)return 0;if((!((int(*)(int(*cmp)(int,
int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(
Cyc_Core_intcmp,_tmp557,_tmp560) && ((int(*)(int y,union Cyc_Absyn_Constraint*x))
Cyc_Absyn_conref_def)(0,_tmp557)) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))
Cyc_Absyn_conref_def)(0,_tmp560))return 0;if((!((int(*)(int(*cmp)(int,int),union
Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(
Cyc_Core_intcmp,_tmp559,_tmp562) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))
Cyc_Absyn_conref_def)(0,_tmp559)) && ((int(*)(int y,union Cyc_Absyn_Constraint*x))
Cyc_Absyn_conref_def)(0,_tmp562))return 0;if(!Cyc_Tcutil_unify(_tmp556,_tmp55F)
 && !Cyc_Tcenv_region_outlives(te,_tmp556,_tmp55F))return 0;if(!Cyc_Tcutil_unify_conrefs(
Cyc_Tcutil_unify_it_bounds,_tmp558,_tmp561)){struct _tuple0 _tmp57A=({struct
_tuple0 _tmp579;_tmp579.f1=Cyc_Absyn_conref_val(_tmp558);_tmp579.f2=Cyc_Absyn_conref_val(
_tmp561);_tmp579;});void*_tmp57B;void*_tmp57C;void*_tmp57D;struct Cyc_Absyn_Exp*
_tmp57E;void*_tmp57F;struct Cyc_Absyn_Exp*_tmp580;_LL3B9: _tmp57B=_tmp57A.f1;if(
_tmp57B <= (void*)1)goto _LL3BB;if(*((int*)_tmp57B)!= 0)goto _LL3BB;_tmp57C=_tmp57A.f2;
if((int)_tmp57C != 0)goto _LL3BB;_LL3BA: goto _LL3B8;_LL3BB: _tmp57D=_tmp57A.f1;if(
_tmp57D <= (void*)1)goto _LL3BD;if(*((int*)_tmp57D)!= 0)goto _LL3BD;_tmp57E=((
struct Cyc_Absyn_Upper_b_struct*)_tmp57D)->f1;_tmp57F=_tmp57A.f2;if(_tmp57F <= (
void*)1)goto _LL3BD;if(*((int*)_tmp57F)!= 0)goto _LL3BD;_tmp580=((struct Cyc_Absyn_Upper_b_struct*)
_tmp57F)->f1;_LL3BC: if(!Cyc_Evexp_lte_const_exp(_tmp580,_tmp57E))return 0;goto
_LL3B8;_LL3BD:;_LL3BE: return 0;_LL3B8:;}return Cyc_Tcutil_ptrsubtype(te,({struct
Cyc_List_List*_tmp581=_cycalloc(sizeof(*_tmp581));_tmp581->hd=({struct _tuple0*
_tmp582=_cycalloc(sizeof(*_tmp582));_tmp582->f1=t1;_tmp582->f2=t2;_tmp582;});
_tmp581->tl=assume;_tmp581;}),_tmp553,_tmp55C);_LL3B2: _tmp563=_tmp550.f1;if(
_tmp563 <= (void*)4)goto _LL3B4;if(*((int*)_tmp563)!= 2)goto _LL3B4;_tmp564=((
struct Cyc_Absyn_DatatypeType_struct*)_tmp563)->f1;_tmp565=_tmp564.datatype_info;
if((_tmp565.KnownDatatype).tag != 2)goto _LL3B4;_tmp566=(struct Cyc_Absyn_Datatypedecl**)(
_tmp565.KnownDatatype).val;_tmp567=*_tmp566;_tmp568=_tmp564.targs;_tmp569=
_tmp564.rgn;if(_tmp569 == 0)goto _LL3B4;_tmp56A=*_tmp569;_tmp56B=(void*)_tmp56A.v;
_tmp56C=_tmp550.f2;if(_tmp56C <= (void*)4)goto _LL3B4;if(*((int*)_tmp56C)!= 2)goto
_LL3B4;_tmp56D=((struct Cyc_Absyn_DatatypeType_struct*)_tmp56C)->f1;_tmp56E=
_tmp56D.datatype_info;if((_tmp56E.KnownDatatype).tag != 2)goto _LL3B4;_tmp56F=(
struct Cyc_Absyn_Datatypedecl**)(_tmp56E.KnownDatatype).val;_tmp570=*_tmp56F;
_tmp571=_tmp56D.targs;_tmp572=_tmp56D.rgn;if(_tmp572 == 0)goto _LL3B4;_tmp573=*
_tmp572;_tmp574=(void*)_tmp573.v;_LL3B3: if(_tmp567 != _tmp570  || !Cyc_Tcenv_region_outlives(
te,_tmp56B,_tmp574))return 0;for(0;_tmp568 != 0  && _tmp571 != 0;(_tmp568=_tmp568->tl,
_tmp571=_tmp571->tl)){if(!Cyc_Tcutil_unify((void*)_tmp568->hd,(void*)_tmp571->hd))
return 0;}if(_tmp568 != 0  || _tmp571 != 0)return 0;return 1;_LL3B4: _tmp575=_tmp550.f1;
if(_tmp575 <= (void*)4)goto _LL3B6;if(*((int*)_tmp575)!= 8)goto _LL3B6;_tmp576=((
struct Cyc_Absyn_FnType_struct*)_tmp575)->f1;_tmp577=_tmp550.f2;if(_tmp577 <= (
void*)4)goto _LL3B6;if(*((int*)_tmp577)!= 8)goto _LL3B6;_tmp578=((struct Cyc_Absyn_FnType_struct*)
_tmp577)->f1;_LL3B5: if(_tmp576.tvars != 0  || _tmp578.tvars != 0){struct Cyc_List_List*
_tmp583=_tmp576.tvars;struct Cyc_List_List*_tmp584=_tmp578.tvars;if(((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmp583)!= ((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(_tmp584))return 0;{struct _RegionHandle*_tmp585=Cyc_Tcenv_get_fnrgn(
te);struct Cyc_List_List*inst=0;while(_tmp583 != 0){if(Cyc_Tcutil_tvar_kind((
struct Cyc_Absyn_Tvar*)_tmp583->hd)!= Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*)_check_null(_tmp584))->hd))return 0;inst=({struct Cyc_List_List*
_tmp586=_region_malloc(_tmp585,sizeof(*_tmp586));_tmp586->hd=({struct _tuple14*
_tmp587=_region_malloc(_tmp585,sizeof(*_tmp587));_tmp587->f1=(struct Cyc_Absyn_Tvar*)
_tmp584->hd;_tmp587->f2=(void*)({struct Cyc_Absyn_VarType_struct*_tmp588=
_cycalloc(sizeof(*_tmp588));_tmp588[0]=({struct Cyc_Absyn_VarType_struct _tmp589;
_tmp589.tag=1;_tmp589.f1=(struct Cyc_Absyn_Tvar*)_tmp583->hd;_tmp589;});_tmp588;});
_tmp587;});_tmp586->tl=inst;_tmp586;});_tmp583=_tmp583->tl;_tmp584=_tmp584->tl;}
if(inst != 0){_tmp576.tvars=0;_tmp578.tvars=0;return Cyc_Tcutil_subtype(te,assume,(
void*)({struct Cyc_Absyn_FnType_struct*_tmp58A=_cycalloc(sizeof(*_tmp58A));
_tmp58A[0]=({struct Cyc_Absyn_FnType_struct _tmp58B;_tmp58B.tag=8;_tmp58B.f1=
_tmp576;_tmp58B;});_tmp58A;}),(void*)({struct Cyc_Absyn_FnType_struct*_tmp58C=
_cycalloc(sizeof(*_tmp58C));_tmp58C[0]=({struct Cyc_Absyn_FnType_struct _tmp58D;
_tmp58D.tag=8;_tmp58D.f1=_tmp578;_tmp58D;});_tmp58C;}));}}}if(!Cyc_Tcutil_subtype(
te,assume,_tmp576.ret_typ,_tmp578.ret_typ))return 0;{struct Cyc_List_List*_tmp58E=
_tmp576.args;struct Cyc_List_List*_tmp58F=_tmp578.args;if(((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(_tmp58E)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp58F))return 0;for(0;_tmp58E != 0;(_tmp58E=_tmp58E->tl,_tmp58F=_tmp58F->tl)){
struct Cyc_Absyn_Tqual _tmp591;void*_tmp592;struct _tuple9 _tmp590=*((struct _tuple9*)
_tmp58E->hd);_tmp591=_tmp590.f2;_tmp592=_tmp590.f3;{struct Cyc_Absyn_Tqual _tmp594;
void*_tmp595;struct _tuple9 _tmp593=*((struct _tuple9*)((struct Cyc_List_List*)
_check_null(_tmp58F))->hd);_tmp594=_tmp593.f2;_tmp595=_tmp593.f3;if(_tmp594.real_const
 && !_tmp591.real_const  || !Cyc_Tcutil_subtype(te,assume,_tmp595,_tmp592))return
0;}}if(_tmp576.c_varargs != _tmp578.c_varargs)return 0;if(_tmp576.cyc_varargs != 0
 && _tmp578.cyc_varargs != 0){struct Cyc_Absyn_VarargInfo _tmp596=*_tmp576.cyc_varargs;
struct Cyc_Absyn_VarargInfo _tmp597=*_tmp578.cyc_varargs;if((_tmp597.tq).real_const
 && !(_tmp596.tq).real_const  || !Cyc_Tcutil_subtype(te,assume,_tmp597.type,
_tmp596.type))return 0;}else{if(_tmp576.cyc_varargs != 0  || _tmp578.cyc_varargs != 
0)return 0;}if(!Cyc_Tcutil_subset_effect(1,(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp576.effect))->v,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp578.effect))->v))
return 0;if(!Cyc_Tcutil_sub_rgnpo(_tmp576.rgn_po,_tmp578.rgn_po))return 0;if(!Cyc_Tcutil_sub_attributes(
_tmp576.attributes,_tmp578.attributes))return 0;return 1;}_LL3B6:;_LL3B7: return 0;
_LL3AF:;}}static int Cyc_Tcutil_isomorphic(void*t1,void*t2){struct _tuple0 _tmp599=({
struct _tuple0 _tmp598;_tmp598.f1=Cyc_Tcutil_compress(t1);_tmp598.f2=Cyc_Tcutil_compress(
t2);_tmp598;});void*_tmp59A;void*_tmp59B;void*_tmp59C;void*_tmp59D;_LL3C0:
_tmp59A=_tmp599.f1;if(_tmp59A <= (void*)4)goto _LL3C2;if(*((int*)_tmp59A)!= 5)goto
_LL3C2;_tmp59B=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp59A)->f2;_tmp59C=
_tmp599.f2;if(_tmp59C <= (void*)4)goto _LL3C2;if(*((int*)_tmp59C)!= 5)goto _LL3C2;
_tmp59D=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp59C)->f2;_LL3C1: return(
_tmp59B == _tmp59D  || _tmp59B == (void*)2  && _tmp59D == (void*)3) || _tmp59B == (
void*)3  && _tmp59D == (void*)2;_LL3C2:;_LL3C3: return 0;_LL3BF:;}static int Cyc_Tcutil_ptrsubtype(
struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2){struct
_RegionHandle*_tmp59E=Cyc_Tcenv_get_fnrgn(te);struct Cyc_List_List*tqs1=Cyc_Tcutil_flatten_typ(
_tmp59E,te,t1);struct Cyc_List_List*tqs2=Cyc_Tcutil_flatten_typ(_tmp59E,te,t2);
for(0;tqs2 != 0;(tqs2=tqs2->tl,tqs1=tqs1->tl)){if(tqs1 == 0)return 0;{struct
_tuple11 _tmp5A0;struct Cyc_Absyn_Tqual _tmp5A1;void*_tmp5A2;struct _tuple11*_tmp59F=(
struct _tuple11*)tqs1->hd;_tmp5A0=*_tmp59F;_tmp5A1=_tmp5A0.f1;_tmp5A2=_tmp5A0.f2;{
struct _tuple11 _tmp5A4;struct Cyc_Absyn_Tqual _tmp5A5;void*_tmp5A6;struct _tuple11*
_tmp5A3=(struct _tuple11*)tqs2->hd;_tmp5A4=*_tmp5A3;_tmp5A5=_tmp5A4.f1;_tmp5A6=
_tmp5A4.f2;if(_tmp5A5.real_const  && Cyc_Tcutil_subtype(te,assume,_tmp5A2,_tmp5A6))
continue;else{if(Cyc_Tcutil_unify(_tmp5A2,_tmp5A6))continue;else{if(Cyc_Tcutil_isomorphic(
_tmp5A2,_tmp5A6))continue;else{return 0;}}}}}}return 1;}static int Cyc_Tcutil_is_char_type(
void*t){void*_tmp5A7=Cyc_Tcutil_compress(t);void*_tmp5A8;_LL3C5: if(_tmp5A7 <= (
void*)4)goto _LL3C7;if(*((int*)_tmp5A7)!= 5)goto _LL3C7;_tmp5A8=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp5A7)->f2;if((int)_tmp5A8 != 0)goto _LL3C7;_LL3C6: return 1;_LL3C7:;_LL3C8: return
0;_LL3C4:;}void*Cyc_Tcutil_castable(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void*t1,void*t2){if(Cyc_Tcutil_unify(t1,t2))return(void*)1;t1=Cyc_Tcutil_compress(
t1);t2=Cyc_Tcutil_compress(t2);if(t2 == (void*)0)return(void*)1;{void*_tmp5A9=t2;
void*_tmp5AA;void*_tmp5AB;_LL3CA: if(_tmp5A9 <= (void*)4)goto _LL3CE;if(*((int*)
_tmp5A9)!= 5)goto _LL3CC;_tmp5AA=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp5A9)->f2;
if((int)_tmp5AA != 2)goto _LL3CC;_LL3CB: goto _LL3CD;_LL3CC: if(*((int*)_tmp5A9)!= 5)
goto _LL3CE;_tmp5AB=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp5A9)->f2;if((int)
_tmp5AB != 3)goto _LL3CE;_LL3CD: if(Cyc_Tcutil_typ_kind(t1)== (void*)2)return(void*)
1;goto _LL3C9;_LL3CE:;_LL3CF: goto _LL3C9;_LL3C9:;}{void*_tmp5AC=t1;struct Cyc_Absyn_PtrInfo
_tmp5AD;void*_tmp5AE;struct Cyc_Absyn_Tqual _tmp5AF;struct Cyc_Absyn_PtrAtts _tmp5B0;
void*_tmp5B1;union Cyc_Absyn_Constraint*_tmp5B2;union Cyc_Absyn_Constraint*_tmp5B3;
union Cyc_Absyn_Constraint*_tmp5B4;struct Cyc_Absyn_ArrayInfo _tmp5B5;void*_tmp5B6;
struct Cyc_Absyn_Tqual _tmp5B7;struct Cyc_Absyn_Exp*_tmp5B8;union Cyc_Absyn_Constraint*
_tmp5B9;struct Cyc_Absyn_Enumdecl*_tmp5BA;void*_tmp5BB;_LL3D1: if(_tmp5AC <= (void*)
4)goto _LL3D9;if(*((int*)_tmp5AC)!= 4)goto _LL3D3;_tmp5AD=((struct Cyc_Absyn_PointerType_struct*)
_tmp5AC)->f1;_tmp5AE=_tmp5AD.elt_typ;_tmp5AF=_tmp5AD.elt_tq;_tmp5B0=_tmp5AD.ptr_atts;
_tmp5B1=_tmp5B0.rgn;_tmp5B2=_tmp5B0.nullable;_tmp5B3=_tmp5B0.bounds;_tmp5B4=
_tmp5B0.zero_term;_LL3D2:{void*_tmp5BC=t2;struct Cyc_Absyn_PtrInfo _tmp5BD;void*
_tmp5BE;struct Cyc_Absyn_Tqual _tmp5BF;struct Cyc_Absyn_PtrAtts _tmp5C0;void*_tmp5C1;
union Cyc_Absyn_Constraint*_tmp5C2;union Cyc_Absyn_Constraint*_tmp5C3;union Cyc_Absyn_Constraint*
_tmp5C4;_LL3E2: if(_tmp5BC <= (void*)4)goto _LL3E4;if(*((int*)_tmp5BC)!= 4)goto
_LL3E4;_tmp5BD=((struct Cyc_Absyn_PointerType_struct*)_tmp5BC)->f1;_tmp5BE=
_tmp5BD.elt_typ;_tmp5BF=_tmp5BD.elt_tq;_tmp5C0=_tmp5BD.ptr_atts;_tmp5C1=_tmp5C0.rgn;
_tmp5C2=_tmp5C0.nullable;_tmp5C3=_tmp5C0.bounds;_tmp5C4=_tmp5C0.zero_term;_LL3E3: {
void*coercion=(void*)3;struct Cyc_List_List*_tmp5C5=({struct Cyc_List_List*_tmp5CE=
_cycalloc(sizeof(*_tmp5CE));_tmp5CE->hd=({struct _tuple0*_tmp5CF=_cycalloc(
sizeof(*_tmp5CF));_tmp5CF->f1=t1;_tmp5CF->f2=t2;_tmp5CF;});_tmp5CE->tl=0;_tmp5CE;});
int _tmp5C6=Cyc_Tcutil_ptrsubtype(te,_tmp5C5,_tmp5AE,_tmp5BE) && (!_tmp5AF.real_const
 || _tmp5BF.real_const);Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;{int
zeroterm_ok=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*
y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp5B4,_tmp5C4) || !((int(*)(union
Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp5C4);int _tmp5C7=_tmp5C6?0:((
Cyc_Tcutil_bits_only(_tmp5AE) && Cyc_Tcutil_is_char_type(_tmp5BE)) && !((int(*)(
int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp5C4)) && (_tmp5BF.real_const
 || !_tmp5AF.real_const);int bounds_ok=Cyc_Tcutil_unify_conrefs(Cyc_Tcutil_unify_it_bounds,
_tmp5B3,_tmp5C3);if(!bounds_ok  && !_tmp5C7){struct _tuple0 _tmp5C9=({struct _tuple0
_tmp5C8;_tmp5C8.f1=Cyc_Absyn_conref_val(_tmp5B3);_tmp5C8.f2=Cyc_Absyn_conref_val(
_tmp5C3);_tmp5C8;});void*_tmp5CA;struct Cyc_Absyn_Exp*_tmp5CB;void*_tmp5CC;struct
Cyc_Absyn_Exp*_tmp5CD;_LL3E7: _tmp5CA=_tmp5C9.f1;if(_tmp5CA <= (void*)1)goto _LL3E9;
if(*((int*)_tmp5CA)!= 0)goto _LL3E9;_tmp5CB=((struct Cyc_Absyn_Upper_b_struct*)
_tmp5CA)->f1;_tmp5CC=_tmp5C9.f2;if(_tmp5CC <= (void*)1)goto _LL3E9;if(*((int*)
_tmp5CC)!= 0)goto _LL3E9;_tmp5CD=((struct Cyc_Absyn_Upper_b_struct*)_tmp5CC)->f1;
_LL3E8: if(Cyc_Evexp_lte_const_exp(_tmp5CD,_tmp5CB))bounds_ok=1;goto _LL3E6;_LL3E9:;
_LL3EA: bounds_ok=1;goto _LL3E6;_LL3E6:;}if(((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,_tmp5B2) && !((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,_tmp5C2))coercion=(void*)2;if(((bounds_ok  && 
zeroterm_ok) && (_tmp5C6  || _tmp5C7)) && (Cyc_Tcutil_unify(_tmp5B1,_tmp5C1) || 
Cyc_Tcenv_region_outlives(te,_tmp5B1,_tmp5C1)))return coercion;else{return(void*)
0;}}}_LL3E4:;_LL3E5: goto _LL3E1;_LL3E1:;}return(void*)0;_LL3D3: if(*((int*)_tmp5AC)
!= 7)goto _LL3D5;_tmp5B5=((struct Cyc_Absyn_ArrayType_struct*)_tmp5AC)->f1;_tmp5B6=
_tmp5B5.elt_type;_tmp5B7=_tmp5B5.tq;_tmp5B8=_tmp5B5.num_elts;_tmp5B9=_tmp5B5.zero_term;
_LL3D4:{void*_tmp5D0=t2;struct Cyc_Absyn_ArrayInfo _tmp5D1;void*_tmp5D2;struct Cyc_Absyn_Tqual
_tmp5D3;struct Cyc_Absyn_Exp*_tmp5D4;union Cyc_Absyn_Constraint*_tmp5D5;_LL3EC: if(
_tmp5D0 <= (void*)4)goto _LL3EE;if(*((int*)_tmp5D0)!= 7)goto _LL3EE;_tmp5D1=((
struct Cyc_Absyn_ArrayType_struct*)_tmp5D0)->f1;_tmp5D2=_tmp5D1.elt_type;_tmp5D3=
_tmp5D1.tq;_tmp5D4=_tmp5D1.num_elts;_tmp5D5=_tmp5D1.zero_term;_LL3ED: {int okay;
okay=((_tmp5B8 != 0  && _tmp5D4 != 0) && ((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*
x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp5B9,
_tmp5D5)) && Cyc_Evexp_lte_const_exp((struct Cyc_Absyn_Exp*)_tmp5D4,(struct Cyc_Absyn_Exp*)
_tmp5B8);return(okay  && Cyc_Tcutil_unify(_tmp5B6,_tmp5D2)) && (!_tmp5B7.real_const
 || _tmp5D3.real_const)?(void*)3:(void*)0;}_LL3EE:;_LL3EF: return(void*)0;_LL3EB:;}
return(void*)0;_LL3D5: if(*((int*)_tmp5AC)!= 12)goto _LL3D7;_tmp5BA=((struct Cyc_Absyn_EnumType_struct*)
_tmp5AC)->f2;_LL3D6:{void*_tmp5D6=t2;struct Cyc_Absyn_Enumdecl*_tmp5D7;_LL3F1: if(
_tmp5D6 <= (void*)4)goto _LL3F3;if(*((int*)_tmp5D6)!= 12)goto _LL3F3;_tmp5D7=((
struct Cyc_Absyn_EnumType_struct*)_tmp5D6)->f2;_LL3F2: if((_tmp5BA->fields != 0  && 
_tmp5D7->fields != 0) && ((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct
Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp5BA->fields))->v)>= ((int(*)(
struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp5D7->fields))->v))return(void*)1;goto _LL3F0;_LL3F3:;_LL3F4: goto
_LL3F0;_LL3F0:;}goto _LL3D8;_LL3D7: if(*((int*)_tmp5AC)!= 5)goto _LL3D9;_LL3D8: goto
_LL3DA;_LL3D9: if((int)_tmp5AC != 1)goto _LL3DB;_LL3DA: goto _LL3DC;_LL3DB: if(_tmp5AC
<= (void*)4)goto _LL3DF;if(*((int*)_tmp5AC)!= 6)goto _LL3DD;_LL3DC: return Cyc_Tcutil_coerceable(
t2)?(void*)1:(void*)0;_LL3DD: if(*((int*)_tmp5AC)!= 14)goto _LL3DF;_tmp5BB=(void*)((
struct Cyc_Absyn_RgnHandleType_struct*)_tmp5AC)->f1;_LL3DE:{void*_tmp5D8=t2;void*
_tmp5D9;_LL3F6: if(_tmp5D8 <= (void*)4)goto _LL3F8;if(*((int*)_tmp5D8)!= 14)goto
_LL3F8;_tmp5D9=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)_tmp5D8)->f1;
_LL3F7: if(Cyc_Tcenv_region_outlives(te,_tmp5BB,_tmp5D9))return(void*)1;goto
_LL3F5;_LL3F8:;_LL3F9: goto _LL3F5;_LL3F5:;}return(void*)0;_LL3DF:;_LL3E0: return(
void*)0;_LL3D0:;}}void Cyc_Tcutil_unchecked_cast(struct Cyc_Tcenv_Tenv*te,struct
Cyc_Absyn_Exp*e,void*t,void*c){if(!Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,t)){struct Cyc_Absyn_Exp*_tmp5DA=Cyc_Absyn_copy_exp(e);e->r=(
void*)({struct Cyc_Absyn_Cast_e_struct*_tmp5DB=_cycalloc(sizeof(*_tmp5DB));
_tmp5DB[0]=({struct Cyc_Absyn_Cast_e_struct _tmp5DC;_tmp5DC.tag=15;_tmp5DC.f1=(
void*)t;_tmp5DC.f2=_tmp5DA;_tmp5DC.f3=0;_tmp5DC.f4=(void*)c;_tmp5DC;});_tmp5DB;});
e->topt=({struct Cyc_Core_Opt*_tmp5DD=_cycalloc(sizeof(*_tmp5DD));_tmp5DD->v=(
void*)t;_tmp5DD;});}}int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*e){void*
_tmp5DE=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
_LL3FB: if(_tmp5DE <= (void*)4)goto _LL405;if(*((int*)_tmp5DE)!= 5)goto _LL3FD;
_LL3FC: goto _LL3FE;_LL3FD: if(*((int*)_tmp5DE)!= 12)goto _LL3FF;_LL3FE: goto _LL400;
_LL3FF: if(*((int*)_tmp5DE)!= 13)goto _LL401;_LL400: goto _LL402;_LL401: if(*((int*)
_tmp5DE)!= 18)goto _LL403;_LL402: return 1;_LL403: if(*((int*)_tmp5DE)!= 0)goto
_LL405;_LL404: return Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,
Cyc_Absyn_sint_typ);_LL405:;_LL406: return 0;_LL3FA:;}int Cyc_Tcutil_is_numeric(
struct Cyc_Absyn_Exp*e){if(Cyc_Tcutil_is_integral(e))return 1;{void*_tmp5DF=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);_LL408: if((int)_tmp5DF != 1)
goto _LL40A;_LL409: goto _LL40B;_LL40A: if(_tmp5DF <= (void*)4)goto _LL40C;if(*((int*)
_tmp5DF)!= 6)goto _LL40C;_LL40B: return 1;_LL40C:;_LL40D: return 0;_LL407:;}}int Cyc_Tcutil_is_function_type(
void*t){void*_tmp5E0=Cyc_Tcutil_compress(t);_LL40F: if(_tmp5E0 <= (void*)4)goto
_LL411;if(*((int*)_tmp5E0)!= 8)goto _LL411;_LL410: return 1;_LL411:;_LL412: return 0;
_LL40E:;}void*Cyc_Tcutil_max_arithmetic_type(void*t1,void*t2){struct _tuple0
_tmp5E2=({struct _tuple0 _tmp5E1;_tmp5E1.f1=t1;_tmp5E1.f2=t2;_tmp5E1;});void*
_tmp5E3;int _tmp5E4;void*_tmp5E5;int _tmp5E6;void*_tmp5E7;void*_tmp5E8;void*
_tmp5E9;void*_tmp5EA;void*_tmp5EB;void*_tmp5EC;void*_tmp5ED;void*_tmp5EE;void*
_tmp5EF;void*_tmp5F0;void*_tmp5F1;void*_tmp5F2;void*_tmp5F3;void*_tmp5F4;void*
_tmp5F5;void*_tmp5F6;void*_tmp5F7;void*_tmp5F8;void*_tmp5F9;void*_tmp5FA;void*
_tmp5FB;void*_tmp5FC;void*_tmp5FD;void*_tmp5FE;void*_tmp5FF;void*_tmp600;void*
_tmp601;void*_tmp602;void*_tmp603;void*_tmp604;_LL414: _tmp5E3=_tmp5E2.f1;if(
_tmp5E3 <= (void*)4)goto _LL416;if(*((int*)_tmp5E3)!= 6)goto _LL416;_tmp5E4=((
struct Cyc_Absyn_DoubleType_struct*)_tmp5E3)->f1;_tmp5E5=_tmp5E2.f2;if(_tmp5E5 <= (
void*)4)goto _LL416;if(*((int*)_tmp5E5)!= 6)goto _LL416;_tmp5E6=((struct Cyc_Absyn_DoubleType_struct*)
_tmp5E5)->f1;_LL415: if(_tmp5E4)return t1;else{return t2;}_LL416: _tmp5E7=_tmp5E2.f1;
if(_tmp5E7 <= (void*)4)goto _LL418;if(*((int*)_tmp5E7)!= 6)goto _LL418;_LL417:
return t1;_LL418: _tmp5E8=_tmp5E2.f2;if(_tmp5E8 <= (void*)4)goto _LL41A;if(*((int*)
_tmp5E8)!= 6)goto _LL41A;_LL419: return t2;_LL41A: _tmp5E9=_tmp5E2.f1;if((int)
_tmp5E9 != 1)goto _LL41C;_LL41B: goto _LL41D;_LL41C: _tmp5EA=_tmp5E2.f2;if((int)
_tmp5EA != 1)goto _LL41E;_LL41D: return(void*)1;_LL41E: _tmp5EB=_tmp5E2.f1;if(
_tmp5EB <= (void*)4)goto _LL420;if(*((int*)_tmp5EB)!= 5)goto _LL420;_tmp5EC=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp5EB)->f1;if((int)_tmp5EC != 1)goto _LL420;
_tmp5ED=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp5EB)->f2;if((int)_tmp5ED != 
4)goto _LL420;_LL41F: goto _LL421;_LL420: _tmp5EE=_tmp5E2.f2;if(_tmp5EE <= (void*)4)
goto _LL422;if(*((int*)_tmp5EE)!= 5)goto _LL422;_tmp5EF=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp5EE)->f1;if((int)_tmp5EF != 1)goto _LL422;_tmp5F0=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp5EE)->f2;if((int)_tmp5F0 != 4)goto _LL422;_LL421: return Cyc_Absyn_ulonglong_typ;
_LL422: _tmp5F1=_tmp5E2.f1;if(_tmp5F1 <= (void*)4)goto _LL424;if(*((int*)_tmp5F1)!= 
5)goto _LL424;_tmp5F2=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp5F1)->f2;if((
int)_tmp5F2 != 4)goto _LL424;_LL423: goto _LL425;_LL424: _tmp5F3=_tmp5E2.f2;if(
_tmp5F3 <= (void*)4)goto _LL426;if(*((int*)_tmp5F3)!= 5)goto _LL426;_tmp5F4=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp5F3)->f2;if((int)_tmp5F4 != 4)goto _LL426;
_LL425: return Cyc_Absyn_slonglong_typ;_LL426: _tmp5F5=_tmp5E2.f1;if(_tmp5F5 <= (
void*)4)goto _LL428;if(*((int*)_tmp5F5)!= 5)goto _LL428;_tmp5F6=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp5F5)->f1;if((int)_tmp5F6 != 1)goto _LL428;_tmp5F7=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp5F5)->f2;if((int)_tmp5F7 != 3)goto _LL428;_LL427: goto _LL429;_LL428: _tmp5F8=
_tmp5E2.f2;if(_tmp5F8 <= (void*)4)goto _LL42A;if(*((int*)_tmp5F8)!= 5)goto _LL42A;
_tmp5F9=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp5F8)->f1;if((int)_tmp5F9 != 
1)goto _LL42A;_tmp5FA=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp5F8)->f2;if((
int)_tmp5FA != 3)goto _LL42A;_LL429: return Cyc_Absyn_ulong_typ;_LL42A: _tmp5FB=
_tmp5E2.f1;if(_tmp5FB <= (void*)4)goto _LL42C;if(*((int*)_tmp5FB)!= 5)goto _LL42C;
_tmp5FC=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp5FB)->f1;if((int)_tmp5FC != 
1)goto _LL42C;_tmp5FD=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp5FB)->f2;if((
int)_tmp5FD != 2)goto _LL42C;_LL42B: goto _LL42D;_LL42C: _tmp5FE=_tmp5E2.f2;if(
_tmp5FE <= (void*)4)goto _LL42E;if(*((int*)_tmp5FE)!= 5)goto _LL42E;_tmp5FF=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp5FE)->f1;if((int)_tmp5FF != 1)goto _LL42E;
_tmp600=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp5FE)->f2;if((int)_tmp600 != 
2)goto _LL42E;_LL42D: return Cyc_Absyn_uint_typ;_LL42E: _tmp601=_tmp5E2.f1;if(
_tmp601 <= (void*)4)goto _LL430;if(*((int*)_tmp601)!= 5)goto _LL430;_tmp602=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp601)->f2;if((int)_tmp602 != 3)goto _LL430;
_LL42F: goto _LL431;_LL430: _tmp603=_tmp5E2.f2;if(_tmp603 <= (void*)4)goto _LL432;if(*((
int*)_tmp603)!= 5)goto _LL432;_tmp604=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp603)->f2;if((int)_tmp604 != 3)goto _LL432;_LL431: return Cyc_Absyn_slong_typ;
_LL432:;_LL433: return Cyc_Absyn_sint_typ;_LL413:;}void Cyc_Tcutil_check_contains_assign(
struct Cyc_Absyn_Exp*e){void*_tmp605=e->r;struct Cyc_Core_Opt*_tmp606;_LL435: if(*((
int*)_tmp605)!= 4)goto _LL437;_tmp606=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmp605)->f2;if(_tmp606 != 0)goto _LL437;_LL436:({void*_tmp607=0;Cyc_Tcutil_warn(e->loc,({
const char*_tmp608="assignment in test";_tag_dyneither(_tmp608,sizeof(char),19);}),
_tag_dyneither(_tmp607,sizeof(void*),0));});goto _LL434;_LL437:;_LL438: goto _LL434;
_LL434:;}static int Cyc_Tcutil_constrain_kinds(void*c1,void*c2){c1=Cyc_Absyn_compress_kb(
c1);c2=Cyc_Absyn_compress_kb(c2);{struct _tuple0 _tmp60A=({struct _tuple0 _tmp609;
_tmp609.f1=c1;_tmp609.f2=c2;_tmp609;});void*_tmp60B;void*_tmp60C;void*_tmp60D;
void*_tmp60E;void*_tmp60F;struct Cyc_Core_Opt*_tmp610;struct Cyc_Core_Opt**_tmp611;
void*_tmp612;struct Cyc_Core_Opt*_tmp613;struct Cyc_Core_Opt**_tmp614;void*_tmp615;
struct Cyc_Core_Opt*_tmp616;struct Cyc_Core_Opt**_tmp617;void*_tmp618;void*_tmp619;
void*_tmp61A;void*_tmp61B;void*_tmp61C;void*_tmp61D;struct Cyc_Core_Opt*_tmp61E;
struct Cyc_Core_Opt**_tmp61F;void*_tmp620;void*_tmp621;struct Cyc_Core_Opt*_tmp622;
struct Cyc_Core_Opt**_tmp623;void*_tmp624;void*_tmp625;struct Cyc_Core_Opt*_tmp626;
struct Cyc_Core_Opt**_tmp627;void*_tmp628;_LL43A: _tmp60B=_tmp60A.f1;if(*((int*)
_tmp60B)!= 0)goto _LL43C;_tmp60C=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp60B)->f1;
_tmp60D=_tmp60A.f2;if(*((int*)_tmp60D)!= 0)goto _LL43C;_tmp60E=(void*)((struct Cyc_Absyn_Eq_kb_struct*)
_tmp60D)->f1;_LL43B: return _tmp60C == _tmp60E;_LL43C: _tmp60F=_tmp60A.f2;if(*((int*)
_tmp60F)!= 1)goto _LL43E;_tmp610=((struct Cyc_Absyn_Unknown_kb_struct*)_tmp60F)->f1;
_tmp611=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)_tmp60F)->f1;
_LL43D:*_tmp611=({struct Cyc_Core_Opt*_tmp629=_cycalloc(sizeof(*_tmp629));_tmp629->v=(
void*)c1;_tmp629;});return 1;_LL43E: _tmp612=_tmp60A.f1;if(*((int*)_tmp612)!= 1)
goto _LL440;_tmp613=((struct Cyc_Absyn_Unknown_kb_struct*)_tmp612)->f1;_tmp614=(
struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)_tmp612)->f1;_LL43F:*
_tmp614=({struct Cyc_Core_Opt*_tmp62A=_cycalloc(sizeof(*_tmp62A));_tmp62A->v=(
void*)c2;_tmp62A;});return 1;_LL440: _tmp615=_tmp60A.f1;if(*((int*)_tmp615)!= 2)
goto _LL442;_tmp616=((struct Cyc_Absyn_Less_kb_struct*)_tmp615)->f1;_tmp617=(
struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)_tmp615)->f1;_tmp618=(
void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp615)->f2;_tmp619=_tmp60A.f2;if(*((
int*)_tmp619)!= 0)goto _LL442;_tmp61A=(void*)((struct Cyc_Absyn_Eq_kb_struct*)
_tmp619)->f1;_LL441: if(Cyc_Tcutil_kind_leq(_tmp61A,_tmp618)){*_tmp617=({struct
Cyc_Core_Opt*_tmp62B=_cycalloc(sizeof(*_tmp62B));_tmp62B->v=(void*)c2;_tmp62B;});
return 1;}else{return 0;}_LL442: _tmp61B=_tmp60A.f1;if(*((int*)_tmp61B)!= 0)goto
_LL444;_tmp61C=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp61B)->f1;_tmp61D=
_tmp60A.f2;if(*((int*)_tmp61D)!= 2)goto _LL444;_tmp61E=((struct Cyc_Absyn_Less_kb_struct*)
_tmp61D)->f1;_tmp61F=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp61D)->f1;_tmp620=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp61D)->f2;
_LL443: if(Cyc_Tcutil_kind_leq(_tmp61C,_tmp620)){*_tmp61F=({struct Cyc_Core_Opt*
_tmp62C=_cycalloc(sizeof(*_tmp62C));_tmp62C->v=(void*)c1;_tmp62C;});return 1;}
else{return 0;}_LL444: _tmp621=_tmp60A.f1;if(*((int*)_tmp621)!= 2)goto _LL439;
_tmp622=((struct Cyc_Absyn_Less_kb_struct*)_tmp621)->f1;_tmp623=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*)_tmp621)->f1;_tmp624=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp621)->f2;_tmp625=_tmp60A.f2;if(*((int*)_tmp625)!= 2)goto _LL439;_tmp626=((
struct Cyc_Absyn_Less_kb_struct*)_tmp625)->f1;_tmp627=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*)_tmp625)->f1;_tmp628=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp625)->f2;_LL445: if(Cyc_Tcutil_kind_leq(_tmp624,_tmp628)){*_tmp627=({struct
Cyc_Core_Opt*_tmp62D=_cycalloc(sizeof(*_tmp62D));_tmp62D->v=(void*)c1;_tmp62D;});
return 1;}else{if(Cyc_Tcutil_kind_leq(_tmp628,_tmp624)){*_tmp623=({struct Cyc_Core_Opt*
_tmp62E=_cycalloc(sizeof(*_tmp62E));_tmp62E->v=(void*)c2;_tmp62E;});return 1;}
else{return 0;}}_LL439:;}}static int Cyc_Tcutil_tvar_id_counter=0;int Cyc_Tcutil_new_tvar_id(){
return Cyc_Tcutil_tvar_id_counter ++;}static int Cyc_Tcutil_tvar_counter=0;struct Cyc_Absyn_Tvar*
Cyc_Tcutil_new_tvar(void*k){int i=Cyc_Tcutil_tvar_counter ++;struct _dyneither_ptr s=(
struct _dyneither_ptr)({struct Cyc_Int_pa_struct _tmp633;_tmp633.tag=1;_tmp633.f1=(
unsigned long)i;{void*_tmp631[1]={& _tmp633};Cyc_aprintf(({const char*_tmp632="#%d";
_tag_dyneither(_tmp632,sizeof(char),4);}),_tag_dyneither(_tmp631,sizeof(void*),1));}});
return({struct Cyc_Absyn_Tvar*_tmp62F=_cycalloc(sizeof(*_tmp62F));_tmp62F->name=({
struct _dyneither_ptr*_tmp630=_cycalloc(sizeof(struct _dyneither_ptr)* 1);_tmp630[
0]=s;_tmp630;});_tmp62F->identity=- 1;_tmp62F->kind=k;_tmp62F;});}int Cyc_Tcutil_is_temp_tvar(
struct Cyc_Absyn_Tvar*t){struct _dyneither_ptr _tmp634=*t->name;return*((const char*)
_check_dyneither_subscript(_tmp634,sizeof(char),0))== '#';}void Cyc_Tcutil_rewrite_temp_tvar(
struct Cyc_Absyn_Tvar*t){({struct Cyc_String_pa_struct _tmp637;_tmp637.tag=0;
_tmp637.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*t->name);{void*_tmp635[
1]={& _tmp637};Cyc_printf(({const char*_tmp636="%s";_tag_dyneither(_tmp636,sizeof(
char),3);}),_tag_dyneither(_tmp635,sizeof(void*),1));}});if(!Cyc_Tcutil_is_temp_tvar(
t))return;{struct _dyneither_ptr _tmp638=Cyc_strconcat(({const char*_tmp63D="`";
_tag_dyneither(_tmp63D,sizeof(char),2);}),(struct _dyneither_ptr)*t->name);({
struct _dyneither_ptr _tmp639=_dyneither_ptr_plus(_tmp638,sizeof(char),1);char
_tmp63A=*((char*)_check_dyneither_subscript(_tmp639,sizeof(char),0));char _tmp63B='t';
if(_get_dyneither_size(_tmp639,sizeof(char))== 1  && (_tmp63A == '\000'  && _tmp63B
!= '\000'))_throw_arraybounds();*((char*)_tmp639.curr)=_tmp63B;});t->name=({
struct _dyneither_ptr*_tmp63C=_cycalloc(sizeof(struct _dyneither_ptr)* 1);_tmp63C[
0]=(struct _dyneither_ptr)_tmp638;_tmp63C;});}}struct _tuple19{struct
_dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};static struct _tuple9*Cyc_Tcutil_fndecl2typ_f(
struct _tuple19*x){return({struct _tuple9*_tmp63E=_cycalloc(sizeof(*_tmp63E));
_tmp63E->f1=(struct Cyc_Core_Opt*)({struct Cyc_Core_Opt*_tmp63F=_cycalloc(sizeof(*
_tmp63F));_tmp63F->v=(*x).f1;_tmp63F;});_tmp63E->f2=(*x).f2;_tmp63E->f3=(*x).f3;
_tmp63E;});}void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*fd){if(fd->cached_typ
== 0){struct Cyc_List_List*_tmp640=0;{struct Cyc_List_List*atts=fd->attributes;
for(0;atts != 0;atts=atts->tl){if(Cyc_Absyn_fntype_att((void*)atts->hd))_tmp640=({
struct Cyc_List_List*_tmp641=_cycalloc(sizeof(*_tmp641));_tmp641->hd=(void*)((
void*)atts->hd);_tmp641->tl=_tmp640;_tmp641;});}}return(void*)({struct Cyc_Absyn_FnType_struct*
_tmp642=_cycalloc(sizeof(*_tmp642));_tmp642[0]=({struct Cyc_Absyn_FnType_struct
_tmp643;_tmp643.tag=8;_tmp643.f1=({struct Cyc_Absyn_FnInfo _tmp644;_tmp644.tvars=
fd->tvs;_tmp644.effect=fd->effect;_tmp644.ret_typ=fd->ret_type;_tmp644.args=((
struct Cyc_List_List*(*)(struct _tuple9*(*f)(struct _tuple19*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Tcutil_fndecl2typ_f,fd->args);_tmp644.c_varargs=fd->c_varargs;
_tmp644.cyc_varargs=fd->cyc_varargs;_tmp644.rgn_po=fd->rgn_po;_tmp644.attributes=
_tmp640;_tmp644;});_tmp643;});_tmp642;});}return(void*)((struct Cyc_Core_Opt*)
_check_null(fd->cached_typ))->v;}struct _tuple20{void*f1;struct Cyc_Absyn_Tqual f2;
void*f3;};static void*Cyc_Tcutil_fst_fdarg(struct _tuple20*t){return(*t).f1;}void*
Cyc_Tcutil_snd_tqt(struct _tuple11*t){return(*t).f2;}static struct _tuple11*Cyc_Tcutil_map2_tq(
struct _tuple11*pr,void*t){return({struct _tuple11*_tmp645=_cycalloc(sizeof(*
_tmp645));_tmp645->f1=(*pr).f1;_tmp645->f2=t;_tmp645;});}struct _tuple21{struct
Cyc_Core_Opt*f1;struct Cyc_Absyn_Tqual f2;};struct _tuple22{struct _tuple21*f1;void*
f2;};static struct _tuple22*Cyc_Tcutil_substitute_f1(struct _RegionHandle*rgn,
struct _tuple9*y){return({struct _tuple22*_tmp646=_region_malloc(rgn,sizeof(*
_tmp646));_tmp646->f1=({struct _tuple21*_tmp647=_region_malloc(rgn,sizeof(*
_tmp647));_tmp647->f1=(*y).f1;_tmp647->f2=(*y).f2;_tmp647;});_tmp646->f2=(*y).f3;
_tmp646;});}static struct _tuple9*Cyc_Tcutil_substitute_f2(struct _tuple22*w){
struct _tuple21*_tmp649;void*_tmp64A;struct _tuple22 _tmp648=*w;_tmp649=_tmp648.f1;
_tmp64A=_tmp648.f2;{struct Cyc_Core_Opt*_tmp64C;struct Cyc_Absyn_Tqual _tmp64D;
struct _tuple21 _tmp64B=*_tmp649;_tmp64C=_tmp64B.f1;_tmp64D=_tmp64B.f2;return({
struct _tuple9*_tmp64E=_cycalloc(sizeof(*_tmp64E));_tmp64E->f1=_tmp64C;_tmp64E->f2=
_tmp64D;_tmp64E->f3=_tmp64A;_tmp64E;});}}static void*Cyc_Tcutil_field_type(struct
Cyc_Absyn_Aggrfield*f){return f->type;}static struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_zip_field_type(
struct Cyc_Absyn_Aggrfield*f,void*t){return({struct Cyc_Absyn_Aggrfield*_tmp64F=
_cycalloc(sizeof(*_tmp64F));_tmp64F->name=f->name;_tmp64F->tq=f->tq;_tmp64F->type=
t;_tmp64F->width=f->width;_tmp64F->attributes=f->attributes;_tmp64F;});}static
struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*rgn,struct Cyc_List_List*
inst,struct Cyc_List_List*ts);static struct Cyc_Absyn_Exp*Cyc_Tcutil_copye(struct
Cyc_Absyn_Exp*old,void*r){return({struct Cyc_Absyn_Exp*_tmp650=_cycalloc(sizeof(*
_tmp650));_tmp650->topt=old->topt;_tmp650->r=r;_tmp650->loc=old->loc;_tmp650->annot=
old->annot;_tmp650;});}static struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubsexp(struct
_RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_Absyn_Exp*e){void*_tmp651=e->r;
void*_tmp652;struct Cyc_List_List*_tmp653;struct Cyc_Absyn_Exp*_tmp654;struct Cyc_Absyn_Exp*
_tmp655;struct Cyc_Absyn_Exp*_tmp656;struct Cyc_Absyn_Exp*_tmp657;struct Cyc_Absyn_Exp*
_tmp658;struct Cyc_Absyn_Exp*_tmp659;struct Cyc_Absyn_Exp*_tmp65A;struct Cyc_Absyn_Exp*
_tmp65B;struct Cyc_Absyn_Exp*_tmp65C;void*_tmp65D;struct Cyc_Absyn_Exp*_tmp65E;int
_tmp65F;void*_tmp660;void*_tmp661;struct Cyc_Absyn_Exp*_tmp662;void*_tmp663;void*
_tmp664;void*_tmp665;_LL447: if(*((int*)_tmp651)!= 0)goto _LL449;_LL448: goto _LL44A;
_LL449: if(*((int*)_tmp651)!= 33)goto _LL44B;_LL44A: goto _LL44C;_LL44B: if(*((int*)
_tmp651)!= 34)goto _LL44D;_LL44C: goto _LL44E;_LL44D: if(*((int*)_tmp651)!= 1)goto
_LL44F;_LL44E: return e;_LL44F: if(*((int*)_tmp651)!= 3)goto _LL451;_tmp652=(void*)((
struct Cyc_Absyn_Primop_e_struct*)_tmp651)->f1;_tmp653=((struct Cyc_Absyn_Primop_e_struct*)
_tmp651)->f2;_LL450: if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp653)
== 1){struct Cyc_Absyn_Exp*_tmp666=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp653))->hd;struct Cyc_Absyn_Exp*_tmp667=Cyc_Tcutil_rsubsexp(r,inst,
_tmp666);if(_tmp667 == _tmp666)return e;return Cyc_Tcutil_copye(e,(void*)({struct
Cyc_Absyn_Primop_e_struct*_tmp668=_cycalloc(sizeof(*_tmp668));_tmp668[0]=({
struct Cyc_Absyn_Primop_e_struct _tmp669;_tmp669.tag=3;_tmp669.f1=(void*)_tmp652;
_tmp669.f2=({struct Cyc_Absyn_Exp*_tmp66A[1];_tmp66A[0]=_tmp667;((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp66A,sizeof(struct Cyc_Absyn_Exp*),
1));});_tmp669;});_tmp668;}));}else{if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp653)== 2){struct Cyc_Absyn_Exp*_tmp66B=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp653))->hd;struct Cyc_Absyn_Exp*_tmp66C=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp653->tl))->hd;struct Cyc_Absyn_Exp*_tmp66D=
Cyc_Tcutil_rsubsexp(r,inst,_tmp66B);struct Cyc_Absyn_Exp*_tmp66E=Cyc_Tcutil_rsubsexp(
r,inst,_tmp66C);if(_tmp66D == _tmp66B  && _tmp66E == _tmp66C)return e;return Cyc_Tcutil_copye(
e,(void*)({struct Cyc_Absyn_Primop_e_struct*_tmp66F=_cycalloc(sizeof(*_tmp66F));
_tmp66F[0]=({struct Cyc_Absyn_Primop_e_struct _tmp670;_tmp670.tag=3;_tmp670.f1=(
void*)_tmp652;_tmp670.f2=({struct Cyc_Absyn_Exp*_tmp671[2];_tmp671[1]=_tmp66E;
_tmp671[0]=_tmp66D;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmp671,sizeof(struct Cyc_Absyn_Exp*),2));});_tmp670;});_tmp66F;}));}
else{return({void*_tmp672=0;((struct Cyc_Absyn_Exp*(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp673="primop does not have 1 or 2 args!";
_tag_dyneither(_tmp673,sizeof(char),34);}),_tag_dyneither(_tmp672,sizeof(void*),
0));});}}_LL451: if(*((int*)_tmp651)!= 6)goto _LL453;_tmp654=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp651)->f1;_tmp655=((struct Cyc_Absyn_Conditional_e_struct*)_tmp651)->f2;
_tmp656=((struct Cyc_Absyn_Conditional_e_struct*)_tmp651)->f3;_LL452: {struct Cyc_Absyn_Exp*
_tmp674=Cyc_Tcutil_rsubsexp(r,inst,_tmp654);struct Cyc_Absyn_Exp*_tmp675=Cyc_Tcutil_rsubsexp(
r,inst,_tmp655);struct Cyc_Absyn_Exp*_tmp676=Cyc_Tcutil_rsubsexp(r,inst,_tmp656);
if((_tmp674 == _tmp654  && _tmp675 == _tmp655) && _tmp676 == _tmp656)return e;return
Cyc_Tcutil_copye(e,(void*)({struct Cyc_Absyn_Conditional_e_struct*_tmp677=
_cycalloc(sizeof(*_tmp677));_tmp677[0]=({struct Cyc_Absyn_Conditional_e_struct
_tmp678;_tmp678.tag=6;_tmp678.f1=_tmp674;_tmp678.f2=_tmp675;_tmp678.f3=_tmp676;
_tmp678;});_tmp677;}));}_LL453: if(*((int*)_tmp651)!= 7)goto _LL455;_tmp657=((
struct Cyc_Absyn_And_e_struct*)_tmp651)->f1;_tmp658=((struct Cyc_Absyn_And_e_struct*)
_tmp651)->f2;_LL454: {struct Cyc_Absyn_Exp*_tmp679=Cyc_Tcutil_rsubsexp(r,inst,
_tmp657);struct Cyc_Absyn_Exp*_tmp67A=Cyc_Tcutil_rsubsexp(r,inst,_tmp658);if(
_tmp679 == _tmp657  && _tmp67A == _tmp658)return e;return Cyc_Tcutil_copye(e,(void*)({
struct Cyc_Absyn_And_e_struct*_tmp67B=_cycalloc(sizeof(*_tmp67B));_tmp67B[0]=({
struct Cyc_Absyn_And_e_struct _tmp67C;_tmp67C.tag=7;_tmp67C.f1=_tmp679;_tmp67C.f2=
_tmp67A;_tmp67C;});_tmp67B;}));}_LL455: if(*((int*)_tmp651)!= 8)goto _LL457;
_tmp659=((struct Cyc_Absyn_Or_e_struct*)_tmp651)->f1;_tmp65A=((struct Cyc_Absyn_Or_e_struct*)
_tmp651)->f2;_LL456: {struct Cyc_Absyn_Exp*_tmp67D=Cyc_Tcutil_rsubsexp(r,inst,
_tmp659);struct Cyc_Absyn_Exp*_tmp67E=Cyc_Tcutil_rsubsexp(r,inst,_tmp65A);if(
_tmp67D == _tmp659  && _tmp67E == _tmp65A)return e;return Cyc_Tcutil_copye(e,(void*)({
struct Cyc_Absyn_Or_e_struct*_tmp67F=_cycalloc(sizeof(*_tmp67F));_tmp67F[0]=({
struct Cyc_Absyn_Or_e_struct _tmp680;_tmp680.tag=8;_tmp680.f1=_tmp67D;_tmp680.f2=
_tmp67E;_tmp680;});_tmp67F;}));}_LL457: if(*((int*)_tmp651)!= 9)goto _LL459;
_tmp65B=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp651)->f1;_tmp65C=((struct Cyc_Absyn_SeqExp_e_struct*)
_tmp651)->f2;_LL458: {struct Cyc_Absyn_Exp*_tmp681=Cyc_Tcutil_rsubsexp(r,inst,
_tmp65B);struct Cyc_Absyn_Exp*_tmp682=Cyc_Tcutil_rsubsexp(r,inst,_tmp65C);if(
_tmp681 == _tmp65B  && _tmp682 == _tmp65C)return e;return Cyc_Tcutil_copye(e,(void*)({
struct Cyc_Absyn_SeqExp_e_struct*_tmp683=_cycalloc(sizeof(*_tmp683));_tmp683[0]=({
struct Cyc_Absyn_SeqExp_e_struct _tmp684;_tmp684.tag=9;_tmp684.f1=_tmp681;_tmp684.f2=
_tmp682;_tmp684;});_tmp683;}));}_LL459: if(*((int*)_tmp651)!= 15)goto _LL45B;
_tmp65D=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp651)->f1;_tmp65E=((struct Cyc_Absyn_Cast_e_struct*)
_tmp651)->f2;_tmp65F=((struct Cyc_Absyn_Cast_e_struct*)_tmp651)->f3;_tmp660=(void*)((
struct Cyc_Absyn_Cast_e_struct*)_tmp651)->f4;_LL45A: {struct Cyc_Absyn_Exp*_tmp685=
Cyc_Tcutil_rsubsexp(r,inst,_tmp65E);void*_tmp686=Cyc_Tcutil_rsubstitute(r,inst,
_tmp65D);if(_tmp685 == _tmp65E  && _tmp686 == _tmp65D)return e;return Cyc_Tcutil_copye(
e,(void*)({struct Cyc_Absyn_Cast_e_struct*_tmp687=_cycalloc(sizeof(*_tmp687));
_tmp687[0]=({struct Cyc_Absyn_Cast_e_struct _tmp688;_tmp688.tag=15;_tmp688.f1=(
void*)_tmp686;_tmp688.f2=_tmp685;_tmp688.f3=_tmp65F;_tmp688.f4=(void*)_tmp660;
_tmp688;});_tmp687;}));}_LL45B: if(*((int*)_tmp651)!= 18)goto _LL45D;_tmp661=(void*)((
struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp651)->f1;_LL45C: {void*_tmp689=Cyc_Tcutil_rsubstitute(
r,inst,_tmp661);if(_tmp689 == _tmp661)return e;return Cyc_Tcutil_copye(e,(void*)({
struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp68A=_cycalloc(sizeof(*_tmp68A));_tmp68A[0]=({
struct Cyc_Absyn_Sizeoftyp_e_struct _tmp68B;_tmp68B.tag=18;_tmp68B.f1=(void*)
_tmp689;_tmp68B;});_tmp68A;}));}_LL45D: if(*((int*)_tmp651)!= 19)goto _LL45F;
_tmp662=((struct Cyc_Absyn_Sizeofexp_e_struct*)_tmp651)->f1;_LL45E: {struct Cyc_Absyn_Exp*
_tmp68C=Cyc_Tcutil_rsubsexp(r,inst,_tmp662);if(_tmp68C == _tmp662)return e;return
Cyc_Tcutil_copye(e,(void*)({struct Cyc_Absyn_Sizeofexp_e_struct*_tmp68D=_cycalloc(
sizeof(*_tmp68D));_tmp68D[0]=({struct Cyc_Absyn_Sizeofexp_e_struct _tmp68E;_tmp68E.tag=
19;_tmp68E.f1=_tmp68C;_tmp68E;});_tmp68D;}));}_LL45F: if(*((int*)_tmp651)!= 20)
goto _LL461;_tmp663=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp651)->f1;
_tmp664=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp651)->f2;_LL460: {void*
_tmp68F=Cyc_Tcutil_rsubstitute(r,inst,_tmp663);if(_tmp68F == _tmp663)return e;
return Cyc_Tcutil_copye(e,(void*)({struct Cyc_Absyn_Offsetof_e_struct*_tmp690=
_cycalloc(sizeof(*_tmp690));_tmp690[0]=({struct Cyc_Absyn_Offsetof_e_struct
_tmp691;_tmp691.tag=20;_tmp691.f1=(void*)_tmp68F;_tmp691.f2=(void*)_tmp664;
_tmp691;});_tmp690;}));}_LL461: if(*((int*)_tmp651)!= 40)goto _LL463;_tmp665=(void*)((
struct Cyc_Absyn_Valueof_e_struct*)_tmp651)->f1;_LL462: {void*_tmp692=Cyc_Tcutil_rsubstitute(
r,inst,_tmp665);if(_tmp692 == _tmp665)return e;return Cyc_Tcutil_copye(e,(void*)({
struct Cyc_Absyn_Valueof_e_struct*_tmp693=_cycalloc(sizeof(*_tmp693));_tmp693[0]=({
struct Cyc_Absyn_Valueof_e_struct _tmp694;_tmp694.tag=40;_tmp694.f1=(void*)_tmp692;
_tmp694;});_tmp693;}));}_LL463:;_LL464: return({void*_tmp695=0;((struct Cyc_Absyn_Exp*(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmp696="non-type-level-expression in Tcutil::rsubsexp";_tag_dyneither(_tmp696,
sizeof(char),46);}),_tag_dyneither(_tmp695,sizeof(void*),0));});_LL446:;}void*
Cyc_Tcutil_rsubstitute(struct _RegionHandle*rgn,struct Cyc_List_List*inst,void*t){
void*_tmp697=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*_tmp698;struct Cyc_Absyn_AggrInfo
_tmp699;union Cyc_Absyn_AggrInfoU _tmp69A;struct Cyc_List_List*_tmp69B;struct Cyc_Absyn_DatatypeInfo
_tmp69C;union Cyc_Absyn_DatatypeInfoU _tmp69D;struct Cyc_List_List*_tmp69E;struct
Cyc_Core_Opt*_tmp69F;struct Cyc_Absyn_DatatypeFieldInfo _tmp6A0;union Cyc_Absyn_DatatypeFieldInfoU
_tmp6A1;struct Cyc_List_List*_tmp6A2;struct _tuple2*_tmp6A3;struct Cyc_List_List*
_tmp6A4;struct Cyc_Absyn_Typedefdecl*_tmp6A5;void**_tmp6A6;struct Cyc_Absyn_ArrayInfo
_tmp6A7;void*_tmp6A8;struct Cyc_Absyn_Tqual _tmp6A9;struct Cyc_Absyn_Exp*_tmp6AA;
union Cyc_Absyn_Constraint*_tmp6AB;struct Cyc_Position_Segment*_tmp6AC;struct Cyc_Absyn_PtrInfo
_tmp6AD;void*_tmp6AE;struct Cyc_Absyn_Tqual _tmp6AF;struct Cyc_Absyn_PtrAtts _tmp6B0;
void*_tmp6B1;union Cyc_Absyn_Constraint*_tmp6B2;union Cyc_Absyn_Constraint*_tmp6B3;
union Cyc_Absyn_Constraint*_tmp6B4;struct Cyc_Absyn_FnInfo _tmp6B5;struct Cyc_List_List*
_tmp6B6;struct Cyc_Core_Opt*_tmp6B7;void*_tmp6B8;struct Cyc_List_List*_tmp6B9;int
_tmp6BA;struct Cyc_Absyn_VarargInfo*_tmp6BB;struct Cyc_List_List*_tmp6BC;struct Cyc_List_List*
_tmp6BD;struct Cyc_List_List*_tmp6BE;void*_tmp6BF;struct Cyc_List_List*_tmp6C0;
struct Cyc_Core_Opt*_tmp6C1;void*_tmp6C2;void*_tmp6C3;void*_tmp6C4;void*_tmp6C5;
struct Cyc_Absyn_Exp*_tmp6C6;void*_tmp6C7;void*_tmp6C8;struct Cyc_List_List*
_tmp6C9;_LL466: if(_tmp697 <= (void*)4)goto _LL488;if(*((int*)_tmp697)!= 1)goto
_LL468;_tmp698=((struct Cyc_Absyn_VarType_struct*)_tmp697)->f1;_LL467: {struct
_handler_cons _tmp6CA;_push_handler(& _tmp6CA);{int _tmp6CC=0;if(setjmp(_tmp6CA.handler))
_tmp6CC=1;if(!_tmp6CC){{void*_tmp6CD=((void*(*)(int(*cmp)(struct Cyc_Absyn_Tvar*,
struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_assoc_cmp)(
Cyc_Absyn_tvar_cmp,inst,_tmp698);_npop_handler(0);return _tmp6CD;};_pop_handler();}
else{void*_tmp6CB=(void*)_exn_thrown;void*_tmp6CF=_tmp6CB;_LL49B: if(_tmp6CF != 
Cyc_Core_Not_found)goto _LL49D;_LL49C: return t;_LL49D:;_LL49E:(void)_throw(_tmp6CF);
_LL49A:;}}}_LL468: if(*((int*)_tmp697)!= 10)goto _LL46A;_tmp699=((struct Cyc_Absyn_AggrType_struct*)
_tmp697)->f1;_tmp69A=_tmp699.aggr_info;_tmp69B=_tmp699.targs;_LL469: {struct Cyc_List_List*
_tmp6D0=Cyc_Tcutil_substs(rgn,inst,_tmp69B);return _tmp6D0 == _tmp69B?t:(void*)({
struct Cyc_Absyn_AggrType_struct*_tmp6D1=_cycalloc(sizeof(*_tmp6D1));_tmp6D1[0]=({
struct Cyc_Absyn_AggrType_struct _tmp6D2;_tmp6D2.tag=10;_tmp6D2.f1=({struct Cyc_Absyn_AggrInfo
_tmp6D3;_tmp6D3.aggr_info=_tmp69A;_tmp6D3.targs=_tmp6D0;_tmp6D3;});_tmp6D2;});
_tmp6D1;});}_LL46A: if(*((int*)_tmp697)!= 2)goto _LL46C;_tmp69C=((struct Cyc_Absyn_DatatypeType_struct*)
_tmp697)->f1;_tmp69D=_tmp69C.datatype_info;_tmp69E=_tmp69C.targs;_tmp69F=_tmp69C.rgn;
_LL46B: {struct Cyc_List_List*_tmp6D4=Cyc_Tcutil_substs(rgn,inst,_tmp69E);struct
Cyc_Core_Opt*new_r;if((unsigned int)_tmp69F){void*_tmp6D5=Cyc_Tcutil_rsubstitute(
rgn,inst,(void*)_tmp69F->v);if(_tmp6D5 == (void*)_tmp69F->v)new_r=_tmp69F;else{
new_r=({struct Cyc_Core_Opt*_tmp6D6=_cycalloc(sizeof(*_tmp6D6));_tmp6D6->v=(void*)
_tmp6D5;_tmp6D6;});}}else{new_r=_tmp69F;}return _tmp6D4 == _tmp69E  && new_r == 
_tmp69F?t:(void*)({struct Cyc_Absyn_DatatypeType_struct*_tmp6D7=_cycalloc(sizeof(*
_tmp6D7));_tmp6D7[0]=({struct Cyc_Absyn_DatatypeType_struct _tmp6D8;_tmp6D8.tag=2;
_tmp6D8.f1=({struct Cyc_Absyn_DatatypeInfo _tmp6D9;_tmp6D9.datatype_info=_tmp69D;
_tmp6D9.targs=_tmp6D4;_tmp6D9.rgn=new_r;_tmp6D9;});_tmp6D8;});_tmp6D7;});}_LL46C:
if(*((int*)_tmp697)!= 3)goto _LL46E;_tmp6A0=((struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp697)->f1;_tmp6A1=_tmp6A0.field_info;_tmp6A2=_tmp6A0.targs;_LL46D: {struct Cyc_List_List*
_tmp6DA=Cyc_Tcutil_substs(rgn,inst,_tmp6A2);return _tmp6DA == _tmp6A2?t:(void*)({
struct Cyc_Absyn_DatatypeFieldType_struct*_tmp6DB=_cycalloc(sizeof(*_tmp6DB));
_tmp6DB[0]=({struct Cyc_Absyn_DatatypeFieldType_struct _tmp6DC;_tmp6DC.tag=3;
_tmp6DC.f1=({struct Cyc_Absyn_DatatypeFieldInfo _tmp6DD;_tmp6DD.field_info=_tmp6A1;
_tmp6DD.targs=_tmp6DA;_tmp6DD;});_tmp6DC;});_tmp6DB;});}_LL46E: if(*((int*)
_tmp697)!= 16)goto _LL470;_tmp6A3=((struct Cyc_Absyn_TypedefType_struct*)_tmp697)->f1;
_tmp6A4=((struct Cyc_Absyn_TypedefType_struct*)_tmp697)->f2;_tmp6A5=((struct Cyc_Absyn_TypedefType_struct*)
_tmp697)->f3;_tmp6A6=((struct Cyc_Absyn_TypedefType_struct*)_tmp697)->f4;_LL46F: {
struct Cyc_List_List*_tmp6DE=Cyc_Tcutil_substs(rgn,inst,_tmp6A4);return _tmp6DE == 
_tmp6A4?t:(void*)({struct Cyc_Absyn_TypedefType_struct*_tmp6DF=_cycalloc(sizeof(*
_tmp6DF));_tmp6DF[0]=({struct Cyc_Absyn_TypedefType_struct _tmp6E0;_tmp6E0.tag=16;
_tmp6E0.f1=_tmp6A3;_tmp6E0.f2=_tmp6DE;_tmp6E0.f3=_tmp6A5;_tmp6E0.f4=_tmp6A6;
_tmp6E0;});_tmp6DF;});}_LL470: if(*((int*)_tmp697)!= 7)goto _LL472;_tmp6A7=((
struct Cyc_Absyn_ArrayType_struct*)_tmp697)->f1;_tmp6A8=_tmp6A7.elt_type;_tmp6A9=
_tmp6A7.tq;_tmp6AA=_tmp6A7.num_elts;_tmp6AB=_tmp6A7.zero_term;_tmp6AC=_tmp6A7.zt_loc;
_LL471: {void*_tmp6E1=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6A8);return _tmp6E1 == 
_tmp6A8?t:(void*)({struct Cyc_Absyn_ArrayType_struct*_tmp6E2=_cycalloc(sizeof(*
_tmp6E2));_tmp6E2[0]=({struct Cyc_Absyn_ArrayType_struct _tmp6E3;_tmp6E3.tag=7;
_tmp6E3.f1=({struct Cyc_Absyn_ArrayInfo _tmp6E4;_tmp6E4.elt_type=_tmp6E1;_tmp6E4.tq=
_tmp6A9;_tmp6E4.num_elts=_tmp6AA;_tmp6E4.zero_term=_tmp6AB;_tmp6E4.zt_loc=
_tmp6AC;_tmp6E4;});_tmp6E3;});_tmp6E2;});}_LL472: if(*((int*)_tmp697)!= 4)goto
_LL474;_tmp6AD=((struct Cyc_Absyn_PointerType_struct*)_tmp697)->f1;_tmp6AE=
_tmp6AD.elt_typ;_tmp6AF=_tmp6AD.elt_tq;_tmp6B0=_tmp6AD.ptr_atts;_tmp6B1=_tmp6B0.rgn;
_tmp6B2=_tmp6B0.nullable;_tmp6B3=_tmp6B0.bounds;_tmp6B4=_tmp6B0.zero_term;_LL473: {
void*_tmp6E5=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6AE);void*_tmp6E6=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmp6B1);union Cyc_Absyn_Constraint*_tmp6E7=_tmp6B3;{void*_tmp6E8=Cyc_Absyn_conref_def((
void*)((void*)0),_tmp6B3);struct Cyc_Absyn_Exp*_tmp6E9;_LL4A0: if(_tmp6E8 <= (void*)
1)goto _LL4A2;if(*((int*)_tmp6E8)!= 0)goto _LL4A2;_tmp6E9=((struct Cyc_Absyn_Upper_b_struct*)
_tmp6E8)->f1;_LL4A1: {struct Cyc_Absyn_Exp*_tmp6EA=Cyc_Tcutil_rsubsexp(rgn,inst,
_tmp6E9);if(_tmp6EA != _tmp6E9)_tmp6E7=Cyc_Absyn_new_conref((void*)({struct Cyc_Absyn_Upper_b_struct*
_tmp6EB=_cycalloc(sizeof(*_tmp6EB));_tmp6EB[0]=({struct Cyc_Absyn_Upper_b_struct
_tmp6EC;_tmp6EC.tag=0;_tmp6EC.f1=_tmp6EA;_tmp6EC;});_tmp6EB;}));goto _LL49F;}
_LL4A2:;_LL4A3: goto _LL49F;_LL49F:;}if((_tmp6E5 == _tmp6AE  && _tmp6E6 == _tmp6B1)
 && _tmp6E7 == _tmp6B3)return t;return(void*)({struct Cyc_Absyn_PointerType_struct*
_tmp6ED=_cycalloc(sizeof(*_tmp6ED));_tmp6ED[0]=({struct Cyc_Absyn_PointerType_struct
_tmp6EE;_tmp6EE.tag=4;_tmp6EE.f1=({struct Cyc_Absyn_PtrInfo _tmp6EF;_tmp6EF.elt_typ=
_tmp6E5;_tmp6EF.elt_tq=_tmp6AF;_tmp6EF.ptr_atts=({struct Cyc_Absyn_PtrAtts _tmp6F0;
_tmp6F0.rgn=_tmp6E6;_tmp6F0.nullable=_tmp6B2;_tmp6F0.bounds=_tmp6E7;_tmp6F0.zero_term=
_tmp6B4;_tmp6F0.ptrloc=0;_tmp6F0;});_tmp6EF;});_tmp6EE;});_tmp6ED;});}_LL474: if(*((
int*)_tmp697)!= 8)goto _LL476;_tmp6B5=((struct Cyc_Absyn_FnType_struct*)_tmp697)->f1;
_tmp6B6=_tmp6B5.tvars;_tmp6B7=_tmp6B5.effect;_tmp6B8=_tmp6B5.ret_typ;_tmp6B9=
_tmp6B5.args;_tmp6BA=_tmp6B5.c_varargs;_tmp6BB=_tmp6B5.cyc_varargs;_tmp6BC=
_tmp6B5.rgn_po;_tmp6BD=_tmp6B5.attributes;_LL475:{struct Cyc_List_List*_tmp6F1=
_tmp6B6;for(0;_tmp6F1 != 0;_tmp6F1=_tmp6F1->tl){inst=({struct Cyc_List_List*
_tmp6F2=_region_malloc(rgn,sizeof(*_tmp6F2));_tmp6F2->hd=({struct _tuple14*
_tmp6F3=_region_malloc(rgn,sizeof(*_tmp6F3));_tmp6F3->f1=(struct Cyc_Absyn_Tvar*)
_tmp6F1->hd;_tmp6F3->f2=(void*)({struct Cyc_Absyn_VarType_struct*_tmp6F4=
_cycalloc(sizeof(*_tmp6F4));_tmp6F4[0]=({struct Cyc_Absyn_VarType_struct _tmp6F5;
_tmp6F5.tag=1;_tmp6F5.f1=(struct Cyc_Absyn_Tvar*)_tmp6F1->hd;_tmp6F5;});_tmp6F4;});
_tmp6F3;});_tmp6F2->tl=inst;_tmp6F2;});}}{struct Cyc_List_List*_tmp6F7;struct Cyc_List_List*
_tmp6F8;struct _tuple1 _tmp6F6=((struct _tuple1(*)(struct _RegionHandle*r1,struct
_RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(rgn,rgn,((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _tuple22*(*f)(struct _RegionHandle*,struct _tuple9*),
struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_substitute_f1,
rgn,_tmp6B9));_tmp6F7=_tmp6F6.f1;_tmp6F8=_tmp6F6.f2;{struct Cyc_List_List*_tmp6F9=
Cyc_Tcutil_substs(rgn,inst,_tmp6F8);struct Cyc_List_List*_tmp6FA=((struct Cyc_List_List*(*)(
struct _tuple9*(*f)(struct _tuple22*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_substitute_f2,((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmp6F7,_tmp6F9));struct Cyc_Core_Opt*
eff2;if(_tmp6B7 == 0)eff2=0;else{void*_tmp6FB=Cyc_Tcutil_rsubstitute(rgn,inst,(
void*)_tmp6B7->v);if(_tmp6FB == (void*)_tmp6B7->v)eff2=_tmp6B7;else{eff2=({struct
Cyc_Core_Opt*_tmp6FC=_cycalloc(sizeof(*_tmp6FC));_tmp6FC->v=(void*)_tmp6FB;
_tmp6FC;});}}{struct Cyc_Absyn_VarargInfo*cyc_varargs2;if(_tmp6BB == 0)
cyc_varargs2=0;else{struct Cyc_Core_Opt*_tmp6FE;struct Cyc_Absyn_Tqual _tmp6FF;void*
_tmp700;int _tmp701;struct Cyc_Absyn_VarargInfo _tmp6FD=*_tmp6BB;_tmp6FE=_tmp6FD.name;
_tmp6FF=_tmp6FD.tq;_tmp700=_tmp6FD.type;_tmp701=_tmp6FD.inject;{void*_tmp702=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmp700);if(_tmp702 == _tmp700)cyc_varargs2=_tmp6BB;else{cyc_varargs2=({
struct Cyc_Absyn_VarargInfo*_tmp703=_cycalloc(sizeof(*_tmp703));_tmp703->name=
_tmp6FE;_tmp703->tq=_tmp6FF;_tmp703->type=_tmp702;_tmp703->inject=_tmp701;
_tmp703;});}}}{struct Cyc_List_List*rgn_po2;struct Cyc_List_List*_tmp705;struct Cyc_List_List*
_tmp706;struct _tuple1 _tmp704=Cyc_List_rsplit(rgn,rgn,_tmp6BC);_tmp705=_tmp704.f1;
_tmp706=_tmp704.f2;{struct Cyc_List_List*_tmp707=Cyc_Tcutil_substs(rgn,inst,
_tmp705);struct Cyc_List_List*_tmp708=Cyc_Tcutil_substs(rgn,inst,_tmp706);if(
_tmp707 == _tmp705  && _tmp708 == _tmp706)rgn_po2=_tmp6BC;else{rgn_po2=Cyc_List_zip(
_tmp707,_tmp708);}return(void*)({struct Cyc_Absyn_FnType_struct*_tmp709=_cycalloc(
sizeof(*_tmp709));_tmp709[0]=({struct Cyc_Absyn_FnType_struct _tmp70A;_tmp70A.tag=
8;_tmp70A.f1=({struct Cyc_Absyn_FnInfo _tmp70B;_tmp70B.tvars=_tmp6B6;_tmp70B.effect=
eff2;_tmp70B.ret_typ=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6B8);_tmp70B.args=
_tmp6FA;_tmp70B.c_varargs=_tmp6BA;_tmp70B.cyc_varargs=cyc_varargs2;_tmp70B.rgn_po=
rgn_po2;_tmp70B.attributes=_tmp6BD;_tmp70B;});_tmp70A;});_tmp709;});}}}}}_LL476:
if(*((int*)_tmp697)!= 9)goto _LL478;_tmp6BE=((struct Cyc_Absyn_TupleType_struct*)
_tmp697)->f1;_LL477: {struct Cyc_List_List*_tmp70C=((struct Cyc_List_List*(*)(
struct _RegionHandle*,void*(*f)(struct _tuple11*),struct Cyc_List_List*x))Cyc_List_rmap)(
rgn,Cyc_Tcutil_snd_tqt,_tmp6BE);struct Cyc_List_List*_tmp70D=Cyc_Tcutil_substs(
rgn,inst,_tmp70C);if(_tmp70D == _tmp70C)return t;{struct Cyc_List_List*_tmp70E=((
struct Cyc_List_List*(*)(struct _tuple11*(*f)(struct _tuple11*,void*),struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_map2_tq,_tmp6BE,_tmp70D);
return(void*)({struct Cyc_Absyn_TupleType_struct*_tmp70F=_cycalloc(sizeof(*
_tmp70F));_tmp70F[0]=({struct Cyc_Absyn_TupleType_struct _tmp710;_tmp710.tag=9;
_tmp710.f1=_tmp70E;_tmp710;});_tmp70F;});}}_LL478: if(*((int*)_tmp697)!= 11)goto
_LL47A;_tmp6BF=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)_tmp697)->f1;
_tmp6C0=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp697)->f2;_LL479: {struct Cyc_List_List*
_tmp711=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct Cyc_Absyn_Aggrfield*),
struct Cyc_List_List*x))Cyc_List_rmap)(rgn,Cyc_Tcutil_field_type,_tmp6C0);struct
Cyc_List_List*_tmp712=Cyc_Tcutil_substs(rgn,inst,_tmp711);if(_tmp712 == _tmp711)
return t;{struct Cyc_List_List*_tmp713=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*
f)(struct Cyc_Absyn_Aggrfield*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_map2)(Cyc_Tcutil_zip_field_type,_tmp6C0,_tmp712);return(void*)({struct
Cyc_Absyn_AnonAggrType_struct*_tmp714=_cycalloc(sizeof(*_tmp714));_tmp714[0]=({
struct Cyc_Absyn_AnonAggrType_struct _tmp715;_tmp715.tag=11;_tmp715.f1=(void*)
_tmp6BF;_tmp715.f2=_tmp713;_tmp715;});_tmp714;});}}_LL47A: if(*((int*)_tmp697)!= 
0)goto _LL47C;_tmp6C1=((struct Cyc_Absyn_Evar_struct*)_tmp697)->f2;_LL47B: if(
_tmp6C1 != 0)return Cyc_Tcutil_rsubstitute(rgn,inst,(void*)_tmp6C1->v);else{return
t;}_LL47C: if(*((int*)_tmp697)!= 14)goto _LL47E;_tmp6C2=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp697)->f1;_LL47D: {void*_tmp716=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6C2);
return _tmp716 == _tmp6C2?t:(void*)({struct Cyc_Absyn_RgnHandleType_struct*_tmp717=
_cycalloc(sizeof(*_tmp717));_tmp717[0]=({struct Cyc_Absyn_RgnHandleType_struct
_tmp718;_tmp718.tag=14;_tmp718.f1=(void*)_tmp716;_tmp718;});_tmp717;});}_LL47E:
if(*((int*)_tmp697)!= 15)goto _LL480;_tmp6C3=(void*)((struct Cyc_Absyn_DynRgnType_struct*)
_tmp697)->f1;_tmp6C4=(void*)((struct Cyc_Absyn_DynRgnType_struct*)_tmp697)->f2;
_LL47F: {void*_tmp719=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6C3);void*_tmp71A=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmp6C4);return _tmp719 == _tmp6C3  && _tmp71A == _tmp6C4?t:(void*)({struct
Cyc_Absyn_DynRgnType_struct*_tmp71B=_cycalloc(sizeof(*_tmp71B));_tmp71B[0]=({
struct Cyc_Absyn_DynRgnType_struct _tmp71C;_tmp71C.tag=15;_tmp71C.f1=(void*)
_tmp719;_tmp71C.f2=(void*)_tmp71A;_tmp71C;});_tmp71B;});}_LL480: if(*((int*)
_tmp697)!= 18)goto _LL482;_tmp6C5=(void*)((struct Cyc_Absyn_TagType_struct*)
_tmp697)->f1;_LL481: {void*_tmp71D=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6C5);
return _tmp71D == _tmp6C5?t:(void*)({struct Cyc_Absyn_TagType_struct*_tmp71E=
_cycalloc(sizeof(*_tmp71E));_tmp71E[0]=({struct Cyc_Absyn_TagType_struct _tmp71F;
_tmp71F.tag=18;_tmp71F.f1=(void*)_tmp71D;_tmp71F;});_tmp71E;});}_LL482: if(*((int*)
_tmp697)!= 17)goto _LL484;_tmp6C6=((struct Cyc_Absyn_ValueofType_struct*)_tmp697)->f1;
_LL483: {struct Cyc_Absyn_Exp*_tmp720=Cyc_Tcutil_rsubsexp(rgn,inst,_tmp6C6);
return _tmp720 == _tmp6C6?t:(void*)({struct Cyc_Absyn_ValueofType_struct*_tmp721=
_cycalloc(sizeof(*_tmp721));_tmp721[0]=({struct Cyc_Absyn_ValueofType_struct
_tmp722;_tmp722.tag=17;_tmp722.f1=_tmp720;_tmp722;});_tmp721;});}_LL484: if(*((
int*)_tmp697)!= 12)goto _LL486;_LL485: goto _LL487;_LL486: if(*((int*)_tmp697)!= 13)
goto _LL488;_LL487: goto _LL489;_LL488: if((int)_tmp697 != 0)goto _LL48A;_LL489: goto
_LL48B;_LL48A: if(_tmp697 <= (void*)4)goto _LL48C;if(*((int*)_tmp697)!= 5)goto
_LL48C;_LL48B: goto _LL48D;_LL48C: if((int)_tmp697 != 1)goto _LL48E;_LL48D: goto _LL48F;
_LL48E: if(_tmp697 <= (void*)4)goto _LL490;if(*((int*)_tmp697)!= 6)goto _LL490;
_LL48F: goto _LL491;_LL490: if((int)_tmp697 != 3)goto _LL492;_LL491: goto _LL493;_LL492:
if((int)_tmp697 != 2)goto _LL494;_LL493: return t;_LL494: if(_tmp697 <= (void*)4)goto
_LL496;if(*((int*)_tmp697)!= 21)goto _LL496;_tmp6C7=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp697)->f1;_LL495: {void*_tmp723=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6C7);
return _tmp723 == _tmp6C7?t:(void*)({struct Cyc_Absyn_RgnsEff_struct*_tmp724=
_cycalloc(sizeof(*_tmp724));_tmp724[0]=({struct Cyc_Absyn_RgnsEff_struct _tmp725;
_tmp725.tag=21;_tmp725.f1=(void*)_tmp723;_tmp725;});_tmp724;});}_LL496: if(
_tmp697 <= (void*)4)goto _LL498;if(*((int*)_tmp697)!= 19)goto _LL498;_tmp6C8=(void*)((
struct Cyc_Absyn_AccessEff_struct*)_tmp697)->f1;_LL497: {void*_tmp726=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmp6C8);return _tmp726 == _tmp6C8?t:(void*)({struct Cyc_Absyn_AccessEff_struct*
_tmp727=_cycalloc(sizeof(*_tmp727));_tmp727[0]=({struct Cyc_Absyn_AccessEff_struct
_tmp728;_tmp728.tag=19;_tmp728.f1=(void*)_tmp726;_tmp728;});_tmp727;});}_LL498:
if(_tmp697 <= (void*)4)goto _LL465;if(*((int*)_tmp697)!= 20)goto _LL465;_tmp6C9=((
struct Cyc_Absyn_JoinEff_struct*)_tmp697)->f1;_LL499: {struct Cyc_List_List*
_tmp729=Cyc_Tcutil_substs(rgn,inst,_tmp6C9);return _tmp729 == _tmp6C9?t:(void*)({
struct Cyc_Absyn_JoinEff_struct*_tmp72A=_cycalloc(sizeof(*_tmp72A));_tmp72A[0]=({
struct Cyc_Absyn_JoinEff_struct _tmp72B;_tmp72B.tag=20;_tmp72B.f1=_tmp729;_tmp72B;});
_tmp72A;});}_LL465:;}static struct Cyc_List_List*Cyc_Tcutil_substs(struct
_RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*ts){if(ts == 0)
return 0;{void*_tmp72C=(void*)ts->hd;struct Cyc_List_List*_tmp72D=ts->tl;void*
_tmp72E=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp72C);struct Cyc_List_List*_tmp72F=Cyc_Tcutil_substs(
rgn,inst,_tmp72D);if(_tmp72C == _tmp72E  && _tmp72D == _tmp72F)return ts;return(
struct Cyc_List_List*)((struct Cyc_List_List*)({struct Cyc_List_List*_tmp730=
_cycalloc(sizeof(*_tmp730));_tmp730->hd=(void*)_tmp72E;_tmp730->tl=_tmp72F;
_tmp730;}));}}extern void*Cyc_Tcutil_substitute(struct Cyc_List_List*inst,void*t){
return Cyc_Tcutil_rsubstitute(Cyc_Core_heap_region,inst,t);}struct _tuple14*Cyc_Tcutil_make_inst_var(
struct Cyc_List_List*s,struct Cyc_Absyn_Tvar*tv){struct Cyc_Core_Opt*_tmp731=Cyc_Tcutil_kind_to_opt(
Cyc_Tcutil_tvar_kind(tv));return({struct _tuple14*_tmp732=_cycalloc(sizeof(*
_tmp732));_tmp732->f1=tv;_tmp732->f2=Cyc_Absyn_new_evar(_tmp731,({struct Cyc_Core_Opt*
_tmp733=_cycalloc(sizeof(*_tmp733));_tmp733->v=s;_tmp733;}));_tmp732;});}struct
_tuple14*Cyc_Tcutil_r_make_inst_var(struct _tuple15*env,struct Cyc_Absyn_Tvar*tv){
struct _tuple15 _tmp735;struct Cyc_List_List*_tmp736;struct _RegionHandle*_tmp737;
struct _tuple15*_tmp734=env;_tmp735=*_tmp734;_tmp736=_tmp735.f1;_tmp737=_tmp735.f2;{
struct Cyc_Core_Opt*_tmp738=Cyc_Tcutil_kind_to_opt(Cyc_Tcutil_tvar_kind(tv));
return({struct _tuple14*_tmp739=_region_malloc(_tmp737,sizeof(*_tmp739));_tmp739->f1=
tv;_tmp739->f2=Cyc_Absyn_new_evar(_tmp738,({struct Cyc_Core_Opt*_tmp73A=_cycalloc(
sizeof(*_tmp73A));_tmp73A->v=_tmp736;_tmp73A;}));_tmp739;});}}static struct Cyc_List_List*
Cyc_Tcutil_add_free_tvar(struct Cyc_Position_Segment*loc,struct Cyc_List_List*tvs,
struct Cyc_Absyn_Tvar*tv){{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
if(Cyc_strptrcmp(((struct Cyc_Absyn_Tvar*)tvs2->hd)->name,tv->name)== 0){void*k1=((
struct Cyc_Absyn_Tvar*)tvs2->hd)->kind;void*k2=tv->kind;if(!Cyc_Tcutil_constrain_kinds(
k1,k2))({struct Cyc_String_pa_struct _tmp73F;_tmp73F.tag=0;_tmp73F.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kindbound2string(k2));{struct
Cyc_String_pa_struct _tmp73E;_tmp73E.tag=0;_tmp73E.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_kindbound2string(k1));{struct Cyc_String_pa_struct
_tmp73D;_tmp73D.tag=0;_tmp73D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*
tv->name);{void*_tmp73B[3]={& _tmp73D,& _tmp73E,& _tmp73F};Cyc_Tcutil_terr(loc,({
const char*_tmp73C="type variable %s is used with inconsistent kinds %s and %s";
_tag_dyneither(_tmp73C,sizeof(char),59);}),_tag_dyneither(_tmp73B,sizeof(void*),
3));}}}});if(tv->identity == - 1)tv->identity=((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity;
else{if(tv->identity != ((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity)({void*
_tmp740=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp741="same type variable has different identity!";_tag_dyneither(
_tmp741,sizeof(char),43);}),_tag_dyneither(_tmp740,sizeof(void*),0));});}return
tvs;}}}tv->identity=Cyc_Tcutil_new_tvar_id();return({struct Cyc_List_List*_tmp742=
_cycalloc(sizeof(*_tmp742));_tmp742->hd=tv;_tmp742->tl=tvs;_tmp742;});}static
struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar(struct Cyc_List_List*tvs,struct
Cyc_Absyn_Tvar*tv){if(tv->identity == - 1)({void*_tmp743=0;((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp744="fast_add_free_tvar: bad identity in tv";
_tag_dyneither(_tmp744,sizeof(char),39);}),_tag_dyneither(_tmp743,sizeof(void*),
0));});{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){struct Cyc_Absyn_Tvar*
_tmp745=(struct Cyc_Absyn_Tvar*)tvs2->hd;if(_tmp745->identity == - 1)({void*_tmp746=
0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp747="fast_add_free_tvar: bad identity in tvs2";_tag_dyneither(
_tmp747,sizeof(char),41);}),_tag_dyneither(_tmp746,sizeof(void*),0));});if(
_tmp745->identity == tv->identity)return tvs;}}return({struct Cyc_List_List*_tmp748=
_cycalloc(sizeof(*_tmp748));_tmp748->hd=tv;_tmp748->tl=tvs;_tmp748;});}struct
_tuple23{struct Cyc_Absyn_Tvar*f1;int f2;};static struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar_bool(
struct _RegionHandle*r,struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv,int b){if(
tv->identity == - 1)({void*_tmp749=0;((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp74A="fast_add_free_tvar_bool: bad identity in tv";
_tag_dyneither(_tmp74A,sizeof(char),44);}),_tag_dyneither(_tmp749,sizeof(void*),
0));});{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){struct
_tuple23 _tmp74C;struct Cyc_Absyn_Tvar*_tmp74D;int _tmp74E;int*_tmp74F;struct
_tuple23*_tmp74B=(struct _tuple23*)tvs2->hd;_tmp74C=*_tmp74B;_tmp74D=_tmp74C.f1;
_tmp74E=_tmp74C.f2;_tmp74F=(int*)&(*_tmp74B).f2;if(_tmp74D->identity == - 1)({void*
_tmp750=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp751="fast_add_free_tvar_bool: bad identity in tvs2";_tag_dyneither(
_tmp751,sizeof(char),46);}),_tag_dyneither(_tmp750,sizeof(void*),0));});if(
_tmp74D->identity == tv->identity){*_tmp74F=*_tmp74F  || b;return tvs;}}}return({
struct Cyc_List_List*_tmp752=_region_malloc(r,sizeof(*_tmp752));_tmp752->hd=({
struct _tuple23*_tmp753=_region_malloc(r,sizeof(*_tmp753));_tmp753->f1=tv;_tmp753->f2=
b;_tmp753;});_tmp752->tl=tvs;_tmp752;});}static struct Cyc_List_List*Cyc_Tcutil_add_bound_tvar(
struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){if(tv->identity == - 1)({struct
Cyc_String_pa_struct _tmp756;_tmp756.tag=0;_tmp756.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Tcutil_tvar2string(tv));{void*_tmp754[1]={& _tmp756};((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const
char*_tmp755="bound tvar id for %s is NULL";_tag_dyneither(_tmp755,sizeof(char),
29);}),_tag_dyneither(_tmp754,sizeof(void*),1));}});return({struct Cyc_List_List*
_tmp757=_cycalloc(sizeof(*_tmp757));_tmp757->hd=tv;_tmp757->tl=tvs;_tmp757;});}
static struct Cyc_List_List*Cyc_Tcutil_add_free_evar(struct _RegionHandle*r,struct
Cyc_List_List*es,void*e,int b){void*_tmp758=Cyc_Tcutil_compress(e);int _tmp759;
_LL4A5: if(_tmp758 <= (void*)4)goto _LL4A7;if(*((int*)_tmp758)!= 0)goto _LL4A7;
_tmp759=((struct Cyc_Absyn_Evar_struct*)_tmp758)->f3;_LL4A6:{struct Cyc_List_List*
es2=es;for(0;es2 != 0;es2=es2->tl){struct _tuple7 _tmp75B;void*_tmp75C;int _tmp75D;
int*_tmp75E;struct _tuple7*_tmp75A=(struct _tuple7*)es2->hd;_tmp75B=*_tmp75A;
_tmp75C=_tmp75B.f1;_tmp75D=_tmp75B.f2;_tmp75E=(int*)&(*_tmp75A).f2;{void*_tmp75F=
Cyc_Tcutil_compress(_tmp75C);int _tmp760;_LL4AA: if(_tmp75F <= (void*)4)goto _LL4AC;
if(*((int*)_tmp75F)!= 0)goto _LL4AC;_tmp760=((struct Cyc_Absyn_Evar_struct*)
_tmp75F)->f3;_LL4AB: if(_tmp759 == _tmp760){if(b != *_tmp75E)*_tmp75E=1;return es;}
goto _LL4A9;_LL4AC:;_LL4AD: goto _LL4A9;_LL4A9:;}}}return({struct Cyc_List_List*
_tmp761=_region_malloc(r,sizeof(*_tmp761));_tmp761->hd=({struct _tuple7*_tmp762=
_region_malloc(r,sizeof(*_tmp762));_tmp762->f1=e;_tmp762->f2=b;_tmp762;});
_tmp761->tl=es;_tmp761;});_LL4A7:;_LL4A8: return es;_LL4A4:;}static struct Cyc_List_List*
Cyc_Tcutil_remove_bound_tvars(struct _RegionHandle*rgn,struct Cyc_List_List*tvs,
struct Cyc_List_List*btvs){struct Cyc_List_List*r=0;for(0;tvs != 0;tvs=tvs->tl){int
present=0;{struct Cyc_List_List*b=btvs;for(0;b != 0;b=b->tl){if(((struct Cyc_Absyn_Tvar*)
tvs->hd)->identity == ((struct Cyc_Absyn_Tvar*)b->hd)->identity){present=1;break;}}}
if(!present)r=({struct Cyc_List_List*_tmp763=_region_malloc(rgn,sizeof(*_tmp763));
_tmp763->hd=(struct Cyc_Absyn_Tvar*)tvs->hd;_tmp763->tl=r;_tmp763;});}r=((struct
Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(r);return r;}static
struct Cyc_List_List*Cyc_Tcutil_remove_bound_tvars_bool(struct _RegionHandle*r,
struct Cyc_List_List*tvs,struct Cyc_List_List*btvs){struct Cyc_List_List*res=0;for(
0;tvs != 0;tvs=tvs->tl){struct Cyc_Absyn_Tvar*_tmp765;int _tmp766;struct _tuple23
_tmp764=*((struct _tuple23*)tvs->hd);_tmp765=_tmp764.f1;_tmp766=_tmp764.f2;{int
present=0;{struct Cyc_List_List*b=btvs;for(0;b != 0;b=b->tl){if(_tmp765->identity
== ((struct Cyc_Absyn_Tvar*)b->hd)->identity){present=1;break;}}}if(!present)res=({
struct Cyc_List_List*_tmp767=_region_malloc(r,sizeof(*_tmp767));_tmp767->hd=(
struct _tuple23*)tvs->hd;_tmp767->tl=res;_tmp767;});}}res=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(res);return res;}void Cyc_Tcutil_check_bitfield(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,void*field_typ,struct Cyc_Absyn_Exp*
width,struct _dyneither_ptr*fn){if(width != 0){unsigned int w=0;if(!Cyc_Tcutil_is_const_exp(
te,(struct Cyc_Absyn_Exp*)width))({struct Cyc_String_pa_struct _tmp76A;_tmp76A.tag=
0;_tmp76A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*fn);{void*_tmp768[1]={&
_tmp76A};Cyc_Tcutil_terr(loc,({const char*_tmp769="bitfield %s does not have constant width";
_tag_dyneither(_tmp769,sizeof(char),41);}),_tag_dyneither(_tmp768,sizeof(void*),
1));}});else{unsigned int _tmp76C;int _tmp76D;struct _tuple13 _tmp76B=Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)width);_tmp76C=_tmp76B.f1;_tmp76D=_tmp76B.f2;if(!_tmp76D)({
void*_tmp76E=0;Cyc_Tcutil_terr(loc,({const char*_tmp76F="bitfield width cannot use sizeof or offsetof";
_tag_dyneither(_tmp76F,sizeof(char),45);}),_tag_dyneither(_tmp76E,sizeof(void*),
0));});w=_tmp76C;}{void*_tmp770=Cyc_Tcutil_compress(field_typ);void*_tmp771;
_LL4AF: if(_tmp770 <= (void*)4)goto _LL4B1;if(*((int*)_tmp770)!= 5)goto _LL4B1;
_tmp771=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp770)->f2;_LL4B0:{void*
_tmp772=_tmp771;_LL4B4: if((int)_tmp772 != 0)goto _LL4B6;_LL4B5: if(w > 8)({void*
_tmp773=0;Cyc_Tcutil_terr(loc,({const char*_tmp774="bitfield larger than type";
_tag_dyneither(_tmp774,sizeof(char),26);}),_tag_dyneither(_tmp773,sizeof(void*),
0));});goto _LL4B3;_LL4B6: if((int)_tmp772 != 1)goto _LL4B8;_LL4B7: if(w > 16)({void*
_tmp775=0;Cyc_Tcutil_terr(loc,({const char*_tmp776="bitfield larger than type";
_tag_dyneither(_tmp776,sizeof(char),26);}),_tag_dyneither(_tmp775,sizeof(void*),
0));});goto _LL4B3;_LL4B8: if((int)_tmp772 != 3)goto _LL4BA;_LL4B9: goto _LL4BB;_LL4BA:
if((int)_tmp772 != 2)goto _LL4BC;_LL4BB: if(w > 32)({void*_tmp777=0;Cyc_Tcutil_terr(
loc,({const char*_tmp778="bitfield larger than type";_tag_dyneither(_tmp778,
sizeof(char),26);}),_tag_dyneither(_tmp777,sizeof(void*),0));});goto _LL4B3;
_LL4BC: if((int)_tmp772 != 4)goto _LL4B3;_LL4BD: if(w > 64)({void*_tmp779=0;Cyc_Tcutil_terr(
loc,({const char*_tmp77A="bitfield larger than type";_tag_dyneither(_tmp77A,
sizeof(char),26);}),_tag_dyneither(_tmp779,sizeof(void*),0));});goto _LL4B3;
_LL4B3:;}goto _LL4AE;_LL4B1:;_LL4B2:({struct Cyc_String_pa_struct _tmp77E;_tmp77E.tag=
0;_tmp77E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
field_typ));{struct Cyc_String_pa_struct _tmp77D;_tmp77D.tag=0;_tmp77D.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*fn);{void*_tmp77B[2]={& _tmp77D,& _tmp77E};
Cyc_Tcutil_terr(loc,({const char*_tmp77C="bitfield %s must have integral type but has type %s";
_tag_dyneither(_tmp77C,sizeof(char),52);}),_tag_dyneither(_tmp77B,sizeof(void*),
2));}}});goto _LL4AE;_LL4AE:;}}}static void Cyc_Tcutil_check_field_atts(struct Cyc_Position_Segment*
loc,struct _dyneither_ptr*fn,struct Cyc_List_List*atts){for(0;atts != 0;atts=atts->tl){
void*_tmp77F=(void*)atts->hd;_LL4BF: if((int)_tmp77F != 5)goto _LL4C1;_LL4C0:
continue;_LL4C1: if(_tmp77F <= (void*)17)goto _LL4C3;if(*((int*)_tmp77F)!= 1)goto
_LL4C3;_LL4C2: continue;_LL4C3:;_LL4C4:({struct Cyc_String_pa_struct _tmp783;
_tmp783.tag=0;_tmp783.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*fn);{
struct Cyc_String_pa_struct _tmp782;_tmp782.tag=0;_tmp782.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absyn_attribute2string((void*)atts->hd));{void*_tmp780[
2]={& _tmp782,& _tmp783};Cyc_Tcutil_terr(loc,({const char*_tmp781="bad attribute %s on member %s";
_tag_dyneither(_tmp781,sizeof(char),30);}),_tag_dyneither(_tmp780,sizeof(void*),
2));}}});_LL4BE:;}}struct Cyc_Tcutil_CVTEnv{struct _RegionHandle*r;struct Cyc_List_List*
kind_env;struct Cyc_List_List*free_vars;struct Cyc_List_List*free_evars;int
generalize_evars;int fn_result;};int Cyc_Tcutil_extract_const_from_typedef(struct
Cyc_Position_Segment*loc,int declared_const,void*t){void*_tmp784=t;struct Cyc_Absyn_Typedefdecl*
_tmp785;void**_tmp786;_LL4C6: if(_tmp784 <= (void*)4)goto _LL4C8;if(*((int*)_tmp784)
!= 16)goto _LL4C8;_tmp785=((struct Cyc_Absyn_TypedefType_struct*)_tmp784)->f3;
_tmp786=((struct Cyc_Absyn_TypedefType_struct*)_tmp784)->f4;_LL4C7: if((((struct
Cyc_Absyn_Typedefdecl*)_check_null(_tmp785))->tq).real_const  || (_tmp785->tq).print_const){
if(declared_const)({void*_tmp787=0;Cyc_Tcutil_warn(loc,({const char*_tmp788="extra const";
_tag_dyneither(_tmp788,sizeof(char),12);}),_tag_dyneither(_tmp787,sizeof(void*),
0));});return 1;}if((unsigned int)_tmp786)return Cyc_Tcutil_extract_const_from_typedef(
loc,declared_const,*_tmp786);else{return declared_const;}_LL4C8:;_LL4C9: return
declared_const;_LL4C5:;}static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type_level_exp(
struct Cyc_Absyn_Exp*e,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv);
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type(struct Cyc_Position_Segment*
loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv,void*expected_kind,
void*t,int put_in_effect){static struct Cyc_Core_Opt urgn={(void*)((void*)3)};static
struct Cyc_Core_Opt hrgn={(void*)((void*)2)};{void*_tmp789=Cyc_Tcutil_compress(t);
struct Cyc_Core_Opt*_tmp78A;struct Cyc_Core_Opt**_tmp78B;struct Cyc_Core_Opt*
_tmp78C;struct Cyc_Core_Opt**_tmp78D;struct Cyc_Absyn_Tvar*_tmp78E;struct Cyc_List_List*
_tmp78F;struct _tuple2*_tmp790;struct Cyc_Absyn_Enumdecl*_tmp791;struct Cyc_Absyn_Enumdecl**
_tmp792;struct Cyc_Absyn_DatatypeInfo _tmp793;union Cyc_Absyn_DatatypeInfoU _tmp794;
union Cyc_Absyn_DatatypeInfoU*_tmp795;struct Cyc_List_List*_tmp796;struct Cyc_List_List**
_tmp797;struct Cyc_Core_Opt*_tmp798;struct Cyc_Core_Opt**_tmp799;struct Cyc_Absyn_DatatypeFieldInfo
_tmp79A;union Cyc_Absyn_DatatypeFieldInfoU _tmp79B;union Cyc_Absyn_DatatypeFieldInfoU*
_tmp79C;struct Cyc_List_List*_tmp79D;struct Cyc_Absyn_PtrInfo _tmp79E;void*_tmp79F;
struct Cyc_Absyn_Tqual _tmp7A0;struct Cyc_Absyn_Tqual*_tmp7A1;struct Cyc_Absyn_PtrAtts
_tmp7A2;void*_tmp7A3;union Cyc_Absyn_Constraint*_tmp7A4;union Cyc_Absyn_Constraint*
_tmp7A5;union Cyc_Absyn_Constraint*_tmp7A6;void*_tmp7A7;struct Cyc_Absyn_Exp*
_tmp7A8;struct Cyc_Absyn_ArrayInfo _tmp7A9;void*_tmp7AA;struct Cyc_Absyn_Tqual
_tmp7AB;struct Cyc_Absyn_Tqual*_tmp7AC;struct Cyc_Absyn_Exp*_tmp7AD;struct Cyc_Absyn_Exp**
_tmp7AE;union Cyc_Absyn_Constraint*_tmp7AF;struct Cyc_Position_Segment*_tmp7B0;
struct Cyc_Absyn_FnInfo _tmp7B1;struct Cyc_List_List*_tmp7B2;struct Cyc_List_List**
_tmp7B3;struct Cyc_Core_Opt*_tmp7B4;struct Cyc_Core_Opt**_tmp7B5;void*_tmp7B6;
struct Cyc_List_List*_tmp7B7;int _tmp7B8;struct Cyc_Absyn_VarargInfo*_tmp7B9;struct
Cyc_List_List*_tmp7BA;struct Cyc_List_List*_tmp7BB;struct Cyc_List_List*_tmp7BC;
void*_tmp7BD;struct Cyc_List_List*_tmp7BE;struct Cyc_Absyn_AggrInfo _tmp7BF;union
Cyc_Absyn_AggrInfoU _tmp7C0;union Cyc_Absyn_AggrInfoU*_tmp7C1;struct Cyc_List_List*
_tmp7C2;struct Cyc_List_List**_tmp7C3;struct _tuple2*_tmp7C4;struct Cyc_List_List*
_tmp7C5;struct Cyc_List_List**_tmp7C6;struct Cyc_Absyn_Typedefdecl*_tmp7C7;struct
Cyc_Absyn_Typedefdecl**_tmp7C8;void**_tmp7C9;void***_tmp7CA;void*_tmp7CB;void*
_tmp7CC;void*_tmp7CD;void*_tmp7CE;void*_tmp7CF;struct Cyc_List_List*_tmp7D0;
_LL4CB: if((int)_tmp789 != 0)goto _LL4CD;_LL4CC: goto _LL4CA;_LL4CD: if(_tmp789 <= (
void*)4)goto _LL4E1;if(*((int*)_tmp789)!= 0)goto _LL4CF;_tmp78A=((struct Cyc_Absyn_Evar_struct*)
_tmp789)->f1;_tmp78B=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)
_tmp789)->f1;_tmp78C=((struct Cyc_Absyn_Evar_struct*)_tmp789)->f2;_tmp78D=(struct
Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)_tmp789)->f2;_LL4CE: if(*_tmp78B == 
0)*_tmp78B=Cyc_Tcutil_kind_to_opt(expected_kind);if((cvtenv.fn_result  && cvtenv.generalize_evars)
 && Cyc_Tcutil_is_region_kind(expected_kind)){if(expected_kind == (void*)4)*
_tmp78D=(struct Cyc_Core_Opt*)& urgn;else{*_tmp78D=(struct Cyc_Core_Opt*)& hrgn;}}
else{if(cvtenv.generalize_evars){struct Cyc_Absyn_Tvar*_tmp7D1=Cyc_Tcutil_new_tvar((
void*)({struct Cyc_Absyn_Less_kb_struct*_tmp7D5=_cycalloc(sizeof(*_tmp7D5));
_tmp7D5[0]=({struct Cyc_Absyn_Less_kb_struct _tmp7D6;_tmp7D6.tag=2;_tmp7D6.f1=0;
_tmp7D6.f2=(void*)expected_kind;_tmp7D6;});_tmp7D5;}));*_tmp78D=({struct Cyc_Core_Opt*
_tmp7D2=_cycalloc(sizeof(*_tmp7D2));_tmp7D2->v=(void*)((void*)({struct Cyc_Absyn_VarType_struct*
_tmp7D3=_cycalloc(sizeof(*_tmp7D3));_tmp7D3[0]=({struct Cyc_Absyn_VarType_struct
_tmp7D4;_tmp7D4.tag=1;_tmp7D4.f1=_tmp7D1;_tmp7D4;});_tmp7D3;}));_tmp7D2;});
_tmp78E=_tmp7D1;goto _LL4D0;}else{cvtenv.free_evars=Cyc_Tcutil_add_free_evar(
cvtenv.r,cvtenv.free_evars,t,put_in_effect);}}goto _LL4CA;_LL4CF: if(*((int*)
_tmp789)!= 1)goto _LL4D1;_tmp78E=((struct Cyc_Absyn_VarType_struct*)_tmp789)->f1;
_LL4D0:{void*_tmp7D7=Cyc_Absyn_compress_kb(_tmp78E->kind);struct Cyc_Core_Opt*
_tmp7D8;struct Cyc_Core_Opt**_tmp7D9;_LL500: if(*((int*)_tmp7D7)!= 1)goto _LL502;
_tmp7D8=((struct Cyc_Absyn_Unknown_kb_struct*)_tmp7D7)->f1;_tmp7D9=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Unknown_kb_struct*)_tmp7D7)->f1;_LL501:*_tmp7D9=({struct Cyc_Core_Opt*
_tmp7DA=_cycalloc(sizeof(*_tmp7DA));_tmp7DA->v=(void*)((void*)({struct Cyc_Absyn_Less_kb_struct*
_tmp7DB=_cycalloc(sizeof(*_tmp7DB));_tmp7DB[0]=({struct Cyc_Absyn_Less_kb_struct
_tmp7DC;_tmp7DC.tag=2;_tmp7DC.f1=0;_tmp7DC.f2=(void*)expected_kind;_tmp7DC;});
_tmp7DB;}));_tmp7DA;});goto _LL4FF;_LL502:;_LL503: goto _LL4FF;_LL4FF:;}cvtenv.kind_env=
Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmp78E);cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(
cvtenv.r,cvtenv.free_vars,_tmp78E,put_in_effect);{void*_tmp7DD=Cyc_Absyn_compress_kb(
_tmp78E->kind);struct Cyc_Core_Opt*_tmp7DE;struct Cyc_Core_Opt**_tmp7DF;void*
_tmp7E0;_LL505: if(*((int*)_tmp7DD)!= 2)goto _LL507;_tmp7DE=((struct Cyc_Absyn_Less_kb_struct*)
_tmp7DD)->f1;_tmp7DF=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp7DD)->f1;_tmp7E0=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp7DD)->f2;
_LL506: if(Cyc_Tcutil_kind_leq(expected_kind,_tmp7E0))*_tmp7DF=({struct Cyc_Core_Opt*
_tmp7E1=_cycalloc(sizeof(*_tmp7E1));_tmp7E1->v=(void*)((void*)({struct Cyc_Absyn_Less_kb_struct*
_tmp7E2=_cycalloc(sizeof(*_tmp7E2));_tmp7E2[0]=({struct Cyc_Absyn_Less_kb_struct
_tmp7E3;_tmp7E3.tag=2;_tmp7E3.f1=0;_tmp7E3.f2=(void*)expected_kind;_tmp7E3;});
_tmp7E2;}));_tmp7E1;});goto _LL504;_LL507:;_LL508: goto _LL504;_LL504:;}goto _LL4CA;
_LL4D1: if(*((int*)_tmp789)!= 13)goto _LL4D3;_tmp78F=((struct Cyc_Absyn_AnonEnumType_struct*)
_tmp789)->f1;_LL4D2: {struct Cyc_Tcenv_Genv*ge=((struct Cyc_Tcenv_Genv*(*)(struct
Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);struct
_RegionHandle*_tmp7E4=Cyc_Tcenv_get_fnrgn(te);{struct Cyc_List_List*prev_fields=0;
unsigned int tag_count=0;for(0;_tmp78F != 0;_tmp78F=_tmp78F->tl){struct Cyc_Absyn_Enumfield*
_tmp7E5=(struct Cyc_Absyn_Enumfield*)_tmp78F->hd;if(((int(*)(int(*compare)(struct
_dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct
_dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,(*_tmp7E5->name).f2))({
struct Cyc_String_pa_struct _tmp7E8;_tmp7E8.tag=0;_tmp7E8.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)*(*_tmp7E5->name).f2);{void*_tmp7E6[1]={& _tmp7E8};Cyc_Tcutil_terr(
_tmp7E5->loc,({const char*_tmp7E7="duplicate enum field name %s";_tag_dyneither(
_tmp7E7,sizeof(char),29);}),_tag_dyneither(_tmp7E6,sizeof(void*),1));}});else{
prev_fields=({struct Cyc_List_List*_tmp7E9=_region_malloc(_tmp7E4,sizeof(*_tmp7E9));
_tmp7E9->hd=(*_tmp7E5->name).f2;_tmp7E9->tl=prev_fields;_tmp7E9;});}if(_tmp7E5->tag
== 0)_tmp7E5->tag=(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(tag_count,_tmp7E5->loc);
else{if(!Cyc_Tcutil_is_const_exp(te,(struct Cyc_Absyn_Exp*)_check_null(_tmp7E5->tag)))({
struct Cyc_String_pa_struct _tmp7EC;_tmp7EC.tag=0;_tmp7EC.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)*(*_tmp7E5->name).f2);{void*_tmp7EA[1]={& _tmp7EC};Cyc_Tcutil_terr(
loc,({const char*_tmp7EB="enum field %s: expression is not constant";
_tag_dyneither(_tmp7EB,sizeof(char),42);}),_tag_dyneither(_tmp7EA,sizeof(void*),
1));}});}{unsigned int t1=(Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)
_check_null(_tmp7E5->tag))).f1;tag_count=t1 + 1;(*_tmp7E5->name).f1=Cyc_Absyn_Abs_n(
te->ns);ge->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct
_dyneither_ptr*k,struct _tuple7*v))Cyc_Dict_insert)(ge->ordinaries,(*_tmp7E5->name).f2,(
struct _tuple7*)({struct _tuple7*_tmp7ED=_cycalloc(sizeof(*_tmp7ED));_tmp7ED->f1=(
void*)({struct Cyc_Tcenv_AnonEnumRes_struct*_tmp7EE=_cycalloc(sizeof(*_tmp7EE));
_tmp7EE[0]=({struct Cyc_Tcenv_AnonEnumRes_struct _tmp7EF;_tmp7EF.tag=4;_tmp7EF.f1=(
void*)t;_tmp7EF.f2=_tmp7E5;_tmp7EF;});_tmp7EE;});_tmp7ED->f2=1;_tmp7ED;}));}}}
goto _LL4CA;}_LL4D3: if(*((int*)_tmp789)!= 12)goto _LL4D5;_tmp790=((struct Cyc_Absyn_EnumType_struct*)
_tmp789)->f1;_tmp791=((struct Cyc_Absyn_EnumType_struct*)_tmp789)->f2;_tmp792=(
struct Cyc_Absyn_Enumdecl**)&((struct Cyc_Absyn_EnumType_struct*)_tmp789)->f2;
_LL4D4: if(*_tmp792 == 0  || ((struct Cyc_Absyn_Enumdecl*)_check_null(*_tmp792))->fields
== 0){struct _handler_cons _tmp7F0;_push_handler(& _tmp7F0);{int _tmp7F2=0;if(setjmp(
_tmp7F0.handler))_tmp7F2=1;if(!_tmp7F2){{struct Cyc_Absyn_Enumdecl**ed=Cyc_Tcenv_lookup_enumdecl(
te,loc,_tmp790);*_tmp792=(struct Cyc_Absyn_Enumdecl*)*ed;};_pop_handler();}else{
void*_tmp7F1=(void*)_exn_thrown;void*_tmp7F4=_tmp7F1;_LL50A: if(_tmp7F4 != Cyc_Dict_Absent)
goto _LL50C;_LL50B: {struct Cyc_Tcenv_Genv*_tmp7F5=((struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct Cyc_Absyn_Enumdecl*_tmp7F6=({struct Cyc_Absyn_Enumdecl*_tmp7F7=_cycalloc(
sizeof(*_tmp7F7));_tmp7F7->sc=(void*)3;_tmp7F7->name=_tmp790;_tmp7F7->fields=0;
_tmp7F7;});Cyc_Tc_tcEnumdecl(te,_tmp7F5,loc,_tmp7F6);{struct Cyc_Absyn_Enumdecl**
ed=Cyc_Tcenv_lookup_enumdecl(te,loc,_tmp790);*_tmp792=(struct Cyc_Absyn_Enumdecl*)*
ed;goto _LL509;}}_LL50C:;_LL50D:(void)_throw(_tmp7F4);_LL509:;}}}{struct Cyc_Absyn_Enumdecl*
ed=(struct Cyc_Absyn_Enumdecl*)_check_null(*_tmp792);*_tmp790=(ed->name)[
_check_known_subscript_notnull(1,0)];goto _LL4CA;}_LL4D5: if(*((int*)_tmp789)!= 2)
goto _LL4D7;_tmp793=((struct Cyc_Absyn_DatatypeType_struct*)_tmp789)->f1;_tmp794=
_tmp793.datatype_info;_tmp795=(union Cyc_Absyn_DatatypeInfoU*)&(((struct Cyc_Absyn_DatatypeType_struct*)
_tmp789)->f1).datatype_info;_tmp796=_tmp793.targs;_tmp797=(struct Cyc_List_List**)&(((
struct Cyc_Absyn_DatatypeType_struct*)_tmp789)->f1).targs;_tmp798=_tmp793.rgn;
_tmp799=(struct Cyc_Core_Opt**)&(((struct Cyc_Absyn_DatatypeType_struct*)_tmp789)->f1).rgn;
_LL4D6: {struct Cyc_List_List*_tmp7F8=*_tmp797;{union Cyc_Absyn_DatatypeInfoU
_tmp7F9=*_tmp795;struct Cyc_Absyn_UnknownDatatypeInfo _tmp7FA;struct _tuple2*
_tmp7FB;int _tmp7FC;struct Cyc_Absyn_Datatypedecl**_tmp7FD;struct Cyc_Absyn_Datatypedecl*
_tmp7FE;_LL50F: if((_tmp7F9.UnknownDatatype).tag != 1)goto _LL511;_tmp7FA=(struct
Cyc_Absyn_UnknownDatatypeInfo)(_tmp7F9.UnknownDatatype).val;_tmp7FB=_tmp7FA.name;
_tmp7FC=_tmp7FA.is_extensible;_LL510: {struct Cyc_Absyn_Datatypedecl**tudp;{
struct _handler_cons _tmp7FF;_push_handler(& _tmp7FF);{int _tmp801=0;if(setjmp(
_tmp7FF.handler))_tmp801=1;if(!_tmp801){tudp=Cyc_Tcenv_lookup_datatypedecl(te,
loc,_tmp7FB);;_pop_handler();}else{void*_tmp800=(void*)_exn_thrown;void*_tmp803=
_tmp800;_LL514: if(_tmp803 != Cyc_Dict_Absent)goto _LL516;_LL515: {struct Cyc_Tcenv_Genv*
_tmp804=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))
Cyc_Dict_lookup)(te->ae,te->ns);struct Cyc_Absyn_Datatypedecl*_tmp805=({struct Cyc_Absyn_Datatypedecl*
_tmp809=_cycalloc(sizeof(*_tmp809));_tmp809->sc=(void*)3;_tmp809->name=_tmp7FB;
_tmp809->tvs=0;_tmp809->fields=0;_tmp809->is_extensible=_tmp7FC;_tmp809;});Cyc_Tc_tcDatatypedecl(
te,_tmp804,loc,_tmp805);tudp=Cyc_Tcenv_lookup_datatypedecl(te,loc,_tmp7FB);if(
_tmp7F8 != 0){({struct Cyc_String_pa_struct _tmp808;_tmp808.tag=0;_tmp808.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp7FB));{
void*_tmp806[1]={& _tmp808};Cyc_Tcutil_terr(loc,({const char*_tmp807="declare parameterized datatype %s before using";
_tag_dyneither(_tmp807,sizeof(char),47);}),_tag_dyneither(_tmp806,sizeof(void*),
1));}});return cvtenv;}goto _LL513;}_LL516:;_LL517:(void)_throw(_tmp803);_LL513:;}}}
if(_tmp7FC  && !(*tudp)->is_extensible)({struct Cyc_String_pa_struct _tmp80C;
_tmp80C.tag=0;_tmp80C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp7FB));{void*_tmp80A[1]={& _tmp80C};Cyc_Tcutil_terr(loc,({const char*_tmp80B="datatype %s was not declared @extensible";
_tag_dyneither(_tmp80B,sizeof(char),41);}),_tag_dyneither(_tmp80A,sizeof(void*),
1));}});*_tmp795=Cyc_Absyn_KnownDatatype(tudp);_tmp7FE=*tudp;goto _LL512;}_LL511:
if((_tmp7F9.KnownDatatype).tag != 2)goto _LL50E;_tmp7FD=(struct Cyc_Absyn_Datatypedecl**)(
_tmp7F9.KnownDatatype).val;_tmp7FE=*_tmp7FD;_LL512: if(!((unsigned int)*_tmp799))*
_tmp799=({struct Cyc_Core_Opt*_tmp80D=_cycalloc(sizeof(*_tmp80D));_tmp80D->v=(
void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,0);_tmp80D;});if((
unsigned int)*_tmp799){void*_tmp80E=(void*)((struct Cyc_Core_Opt*)_check_null(*
_tmp799))->v;cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)3,_tmp80E,
1);}{struct Cyc_List_List*tvs=_tmp7FE->tvs;for(0;_tmp7F8 != 0  && tvs != 0;(_tmp7F8=
_tmp7F8->tl,tvs=tvs->tl)){void*t1=(void*)_tmp7F8->hd;void*k1=Cyc_Tcutil_tvar_kind((
struct Cyc_Absyn_Tvar*)tvs->hd);cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,
cvtenv,k1,t1,1);}if(_tmp7F8 != 0)({struct Cyc_String_pa_struct _tmp811;_tmp811.tag=
0;_tmp811.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp7FE->name));{void*_tmp80F[1]={& _tmp811};Cyc_Tcutil_terr(loc,({const char*
_tmp810="too many type arguments for datatype %s";_tag_dyneither(_tmp810,sizeof(
char),40);}),_tag_dyneither(_tmp80F,sizeof(void*),1));}});if(tvs != 0){struct Cyc_List_List*
hidden_ts=0;for(0;tvs != 0;tvs=tvs->tl){void*k1=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)
tvs->hd);void*e=Cyc_Absyn_new_evar(0,0);hidden_ts=({struct Cyc_List_List*_tmp812=
_cycalloc(sizeof(*_tmp812));_tmp812->hd=(void*)e;_tmp812->tl=hidden_ts;_tmp812;});
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k1,e,1);}*_tmp797=Cyc_List_imp_append(*
_tmp797,Cyc_List_imp_rev(hidden_ts));}goto _LL50E;}_LL50E:;}goto _LL4CA;}_LL4D7:
if(*((int*)_tmp789)!= 3)goto _LL4D9;_tmp79A=((struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp789)->f1;_tmp79B=_tmp79A.field_info;_tmp79C=(union Cyc_Absyn_DatatypeFieldInfoU*)&(((
struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp789)->f1).field_info;_tmp79D=
_tmp79A.targs;_LL4D8:{union Cyc_Absyn_DatatypeFieldInfoU _tmp813=*_tmp79C;struct
Cyc_Absyn_UnknownDatatypeFieldInfo _tmp814;struct _tuple2*_tmp815;struct _tuple2*
_tmp816;int _tmp817;struct _tuple3 _tmp818;struct Cyc_Absyn_Datatypedecl*_tmp819;
struct Cyc_Absyn_Datatypefield*_tmp81A;_LL519: if((_tmp813.UnknownDatatypefield).tag
!= 1)goto _LL51B;_tmp814=(struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmp813.UnknownDatatypefield).val;
_tmp815=_tmp814.datatype_name;_tmp816=_tmp814.field_name;_tmp817=_tmp814.is_extensible;
_LL51A: {struct Cyc_Absyn_Datatypedecl*tud;struct Cyc_Absyn_Datatypefield*tuf;{
struct _handler_cons _tmp81B;_push_handler(& _tmp81B);{int _tmp81D=0;if(setjmp(
_tmp81B.handler))_tmp81D=1;if(!_tmp81D){*Cyc_Tcenv_lookup_datatypedecl(te,loc,
_tmp815);;_pop_handler();}else{void*_tmp81C=(void*)_exn_thrown;void*_tmp81F=
_tmp81C;_LL51E: if(_tmp81F != Cyc_Dict_Absent)goto _LL520;_LL51F:({struct Cyc_String_pa_struct
_tmp822;_tmp822.tag=0;_tmp822.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp815));{void*_tmp820[1]={& _tmp822};Cyc_Tcutil_terr(
loc,({const char*_tmp821="unbound datatype %s";_tag_dyneither(_tmp821,sizeof(char),
20);}),_tag_dyneither(_tmp820,sizeof(void*),1));}});return cvtenv;_LL520:;_LL521:(
void)_throw(_tmp81F);_LL51D:;}}}{struct _handler_cons _tmp823;_push_handler(&
_tmp823);{int _tmp825=0;if(setjmp(_tmp823.handler))_tmp825=1;if(!_tmp825){{struct
_RegionHandle*_tmp826=Cyc_Tcenv_get_fnrgn(te);void*_tmp827=Cyc_Tcenv_lookup_ordinary(
_tmp826,te,loc,_tmp816);struct Cyc_Absyn_Datatypedecl*_tmp828;struct Cyc_Absyn_Datatypefield*
_tmp829;_LL523: if(*((int*)_tmp827)!= 2)goto _LL525;_tmp828=((struct Cyc_Tcenv_DatatypeRes_struct*)
_tmp827)->f1;_tmp829=((struct Cyc_Tcenv_DatatypeRes_struct*)_tmp827)->f2;_LL524:
tuf=_tmp829;tud=_tmp828;if(_tmp817  && !tud->is_extensible)({struct Cyc_String_pa_struct
_tmp82C;_tmp82C.tag=0;_tmp82C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp815));{void*_tmp82A[1]={& _tmp82C};Cyc_Tcutil_terr(
loc,({const char*_tmp82B="datatype %s was not declared @extensible";
_tag_dyneither(_tmp82B,sizeof(char),41);}),_tag_dyneither(_tmp82A,sizeof(void*),
1));}});goto _LL522;_LL525:;_LL526:({struct Cyc_String_pa_struct _tmp830;_tmp830.tag=
0;_tmp830.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp815));{struct Cyc_String_pa_struct _tmp82F;_tmp82F.tag=0;_tmp82F.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp816));{void*
_tmp82D[2]={& _tmp82F,& _tmp830};Cyc_Tcutil_terr(loc,({const char*_tmp82E="unbound field %s in type datatype %s";
_tag_dyneither(_tmp82E,sizeof(char),37);}),_tag_dyneither(_tmp82D,sizeof(void*),
2));}}});{struct Cyc_Tcutil_CVTEnv _tmp831=cvtenv;_npop_handler(0);return _tmp831;}
_LL522:;};_pop_handler();}else{void*_tmp824=(void*)_exn_thrown;void*_tmp833=
_tmp824;_LL528: if(_tmp833 != Cyc_Dict_Absent)goto _LL52A;_LL529:({struct Cyc_String_pa_struct
_tmp837;_tmp837.tag=0;_tmp837.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp815));{struct Cyc_String_pa_struct _tmp836;_tmp836.tag=
0;_tmp836.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp816));{void*_tmp834[2]={& _tmp836,& _tmp837};Cyc_Tcutil_terr(loc,({const char*
_tmp835="unbound field %s in type datatype %s";_tag_dyneither(_tmp835,sizeof(
char),37);}),_tag_dyneither(_tmp834,sizeof(void*),2));}}});return cvtenv;_LL52A:;
_LL52B:(void)_throw(_tmp833);_LL527:;}}}*_tmp79C=Cyc_Absyn_KnownDatatypefield(
tud,tuf);_tmp819=tud;_tmp81A=tuf;goto _LL51C;}_LL51B: if((_tmp813.KnownDatatypefield).tag
!= 2)goto _LL518;_tmp818=(struct _tuple3)(_tmp813.KnownDatatypefield).val;_tmp819=
_tmp818.f1;_tmp81A=_tmp818.f2;_LL51C: {struct Cyc_List_List*tvs=_tmp819->tvs;for(
0;_tmp79D != 0  && tvs != 0;(_tmp79D=_tmp79D->tl,tvs=tvs->tl)){void*t1=(void*)
_tmp79D->hd;void*k1=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd);cvtenv=
Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k1,t1,1);}if(_tmp79D != 0)({struct Cyc_String_pa_struct
_tmp83B;_tmp83B.tag=0;_tmp83B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp81A->name));{struct Cyc_String_pa_struct _tmp83A;
_tmp83A.tag=0;_tmp83A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp819->name));{void*_tmp838[2]={& _tmp83A,& _tmp83B};Cyc_Tcutil_terr(loc,({const
char*_tmp839="too many type arguments for datatype %s.%s";_tag_dyneither(_tmp839,
sizeof(char),43);}),_tag_dyneither(_tmp838,sizeof(void*),2));}}});if(tvs != 0)({
struct Cyc_String_pa_struct _tmp83F;_tmp83F.tag=0;_tmp83F.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp81A->name));{struct Cyc_String_pa_struct
_tmp83E;_tmp83E.tag=0;_tmp83E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp819->name));{void*_tmp83C[2]={& _tmp83E,& _tmp83F};Cyc_Tcutil_terr(
loc,({const char*_tmp83D="too few type arguments for datatype %s.%s";
_tag_dyneither(_tmp83D,sizeof(char),42);}),_tag_dyneither(_tmp83C,sizeof(void*),
2));}}});goto _LL518;}_LL518:;}goto _LL4CA;_LL4D9: if(*((int*)_tmp789)!= 4)goto
_LL4DB;_tmp79E=((struct Cyc_Absyn_PointerType_struct*)_tmp789)->f1;_tmp79F=
_tmp79E.elt_typ;_tmp7A0=_tmp79E.elt_tq;_tmp7A1=(struct Cyc_Absyn_Tqual*)&(((
struct Cyc_Absyn_PointerType_struct*)_tmp789)->f1).elt_tq;_tmp7A2=_tmp79E.ptr_atts;
_tmp7A3=_tmp7A2.rgn;_tmp7A4=_tmp7A2.nullable;_tmp7A5=_tmp7A2.bounds;_tmp7A6=
_tmp7A2.zero_term;_LL4DA: {int is_zero_terminated;cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,(void*)0,_tmp79F,1);_tmp7A1->real_const=Cyc_Tcutil_extract_const_from_typedef(
loc,_tmp7A1->print_const,_tmp79F);cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,
cvtenv,(void*)5,_tmp7A3,1);{union Cyc_Absyn_Constraint*_tmp840=((union Cyc_Absyn_Constraint*(*)(
union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)(_tmp7A6);union Cyc_Absyn_Constraint
_tmp841;int _tmp842;union Cyc_Absyn_Constraint _tmp843;int _tmp844;_LL52D: _tmp841=*
_tmp840;if((_tmp841.No_constr).tag != 3)goto _LL52F;_tmp842=(int)(_tmp841.No_constr).val;
_LL52E:{void*_tmp845=Cyc_Tcutil_compress(_tmp79F);void*_tmp846;_LL534: if(_tmp845
<= (void*)4)goto _LL536;if(*((int*)_tmp845)!= 5)goto _LL536;_tmp846=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp845)->f2;if((int)_tmp846 != 0)goto _LL536;
_LL535:((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*
y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp7A6,Cyc_Absyn_true_conref);
is_zero_terminated=1;goto _LL533;_LL536:;_LL537:((int(*)(int(*cmp)(int,int),union
Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(
Cyc_Core_intcmp,_tmp7A6,Cyc_Absyn_false_conref);is_zero_terminated=0;goto _LL533;
_LL533:;}goto _LL52C;_LL52F: _tmp843=*_tmp840;if((_tmp843.Eq_constr).tag != 1)goto
_LL531;_tmp844=(int)(_tmp843.Eq_constr).val;if(_tmp844 != 1)goto _LL531;_LL530: if(
!Cyc_Tcutil_admits_zero(_tmp79F))({struct Cyc_String_pa_struct _tmp849;_tmp849.tag=
0;_tmp849.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
_tmp79F));{void*_tmp847[1]={& _tmp849};Cyc_Tcutil_terr(loc,({const char*_tmp848="cannot have a pointer to zero-terminated %s elements";
_tag_dyneither(_tmp848,sizeof(char),53);}),_tag_dyneither(_tmp847,sizeof(void*),
1));}});is_zero_terminated=1;goto _LL52C;_LL531:;_LL532: is_zero_terminated=0;goto
_LL52C;_LL52C:;}{void*_tmp84A=Cyc_Absyn_conref_constr(Cyc_Absyn_bounds_one,
_tmp7A5);struct Cyc_Absyn_Exp*_tmp84B;_LL539: if((int)_tmp84A != 0)goto _LL53B;
_LL53A: goto _LL538;_LL53B: if(_tmp84A <= (void*)1)goto _LL538;if(*((int*)_tmp84A)!= 
0)goto _LL538;_tmp84B=((struct Cyc_Absyn_Upper_b_struct*)_tmp84A)->f1;_LL53C: {
struct _RegionHandle*_tmp84C=Cyc_Tcenv_get_fnrgn(te);{struct Cyc_Tcenv_Tenv*
_tmp84D=Cyc_Tcenv_allow_valueof(_tmp84C,te);Cyc_Tcexp_tcExp(_tmp84D,0,_tmp84B);}
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp84B,te,cvtenv);if(!Cyc_Tcutil_coerce_uint_typ(
te,_tmp84B))({void*_tmp84E=0;Cyc_Tcutil_terr(loc,({const char*_tmp84F="pointer bounds expression is not an unsigned int";
_tag_dyneither(_tmp84F,sizeof(char),49);}),_tag_dyneither(_tmp84E,sizeof(void*),
0));});{unsigned int _tmp851;int _tmp852;struct _tuple13 _tmp850=Cyc_Evexp_eval_const_uint_exp(
_tmp84B);_tmp851=_tmp850.f1;_tmp852=_tmp850.f2;if(is_zero_terminated  && (!
_tmp852  || _tmp851 < 1))({void*_tmp853=0;Cyc_Tcutil_terr(loc,({const char*_tmp854="zero-terminated pointer cannot point to empty sequence";
_tag_dyneither(_tmp854,sizeof(char),55);}),_tag_dyneither(_tmp853,sizeof(void*),
0));});goto _LL538;}}_LL538:;}goto _LL4CA;}_LL4DB: if(*((int*)_tmp789)!= 18)goto
_LL4DD;_tmp7A7=(void*)((struct Cyc_Absyn_TagType_struct*)_tmp789)->f1;_LL4DC:
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)7,_tmp7A7,1);goto
_LL4CA;_LL4DD: if(*((int*)_tmp789)!= 17)goto _LL4DF;_tmp7A8=((struct Cyc_Absyn_ValueofType_struct*)
_tmp789)->f1;_LL4DE: {struct _RegionHandle*_tmp855=Cyc_Tcenv_get_fnrgn(te);{
struct Cyc_Tcenv_Tenv*_tmp856=Cyc_Tcenv_allow_valueof(_tmp855,te);Cyc_Tcexp_tcExp(
_tmp856,0,_tmp7A8);}if(!Cyc_Tcutil_coerce_uint_typ(te,_tmp7A8))({void*_tmp857=0;
Cyc_Tcutil_terr(loc,({const char*_tmp858="valueof_t requires an int expression";
_tag_dyneither(_tmp858,sizeof(char),37);}),_tag_dyneither(_tmp857,sizeof(void*),
0));});cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp7A8,te,cvtenv);goto
_LL4CA;}_LL4DF: if(*((int*)_tmp789)!= 5)goto _LL4E1;_LL4E0: goto _LL4E2;_LL4E1: if((
int)_tmp789 != 1)goto _LL4E3;_LL4E2: goto _LL4E4;_LL4E3: if(_tmp789 <= (void*)4)goto
_LL4F1;if(*((int*)_tmp789)!= 6)goto _LL4E5;_LL4E4: goto _LL4CA;_LL4E5: if(*((int*)
_tmp789)!= 7)goto _LL4E7;_tmp7A9=((struct Cyc_Absyn_ArrayType_struct*)_tmp789)->f1;
_tmp7AA=_tmp7A9.elt_type;_tmp7AB=_tmp7A9.tq;_tmp7AC=(struct Cyc_Absyn_Tqual*)&(((
struct Cyc_Absyn_ArrayType_struct*)_tmp789)->f1).tq;_tmp7AD=_tmp7A9.num_elts;
_tmp7AE=(struct Cyc_Absyn_Exp**)&(((struct Cyc_Absyn_ArrayType_struct*)_tmp789)->f1).num_elts;
_tmp7AF=_tmp7A9.zero_term;_tmp7B0=_tmp7A9.zt_loc;_LL4E6: {struct Cyc_Absyn_Exp*
_tmp859=*_tmp7AE;cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)1,
_tmp7AA,1);_tmp7AC->real_const=Cyc_Tcutil_extract_const_from_typedef(loc,_tmp7AC->print_const,
_tmp7AA);{int is_zero_terminated;{union Cyc_Absyn_Constraint*_tmp85A=((union Cyc_Absyn_Constraint*(*)(
union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)(_tmp7AF);union Cyc_Absyn_Constraint
_tmp85B;int _tmp85C;union Cyc_Absyn_Constraint _tmp85D;int _tmp85E;_LL53E: _tmp85B=*
_tmp85A;if((_tmp85B.No_constr).tag != 3)goto _LL540;_tmp85C=(int)(_tmp85B.No_constr).val;
_LL53F:((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*
y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp7AF,Cyc_Absyn_false_conref);
is_zero_terminated=0;goto _LL53D;_LL540: _tmp85D=*_tmp85A;if((_tmp85D.Eq_constr).tag
!= 1)goto _LL542;_tmp85E=(int)(_tmp85D.Eq_constr).val;if(_tmp85E != 1)goto _LL542;
_LL541: if(!Cyc_Tcutil_admits_zero(_tmp7AA))({struct Cyc_String_pa_struct _tmp861;
_tmp861.tag=0;_tmp861.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
_tmp7AA));{void*_tmp85F[1]={& _tmp861};Cyc_Tcutil_terr(loc,({const char*_tmp860="cannot have a zero-terminated array of %s elements";
_tag_dyneither(_tmp860,sizeof(char),51);}),_tag_dyneither(_tmp85F,sizeof(void*),
1));}});is_zero_terminated=1;goto _LL53D;_LL542:;_LL543: is_zero_terminated=0;goto
_LL53D;_LL53D:;}if(_tmp859 == 0){if(is_zero_terminated)*_tmp7AE=(_tmp859=(struct
Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(1,0));else{({void*_tmp862=0;Cyc_Tcutil_warn(
loc,({const char*_tmp863="array bound defaults to 1 here";_tag_dyneither(_tmp863,
sizeof(char),31);}),_tag_dyneither(_tmp862,sizeof(void*),0));});*_tmp7AE=(
_tmp859=(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(1,0));}}Cyc_Tcexp_tcExp(te,0,(
struct Cyc_Absyn_Exp*)_tmp859);if(!Cyc_Tcutil_is_const_exp(te,(struct Cyc_Absyn_Exp*)
_tmp859))({void*_tmp864=0;Cyc_Tcutil_terr(loc,({const char*_tmp865="array bounds expression is not constant";
_tag_dyneither(_tmp865,sizeof(char),40);}),_tag_dyneither(_tmp864,sizeof(void*),
0));});if(!Cyc_Tcutil_coerce_uint_typ(te,(struct Cyc_Absyn_Exp*)_tmp859))({void*
_tmp866=0;Cyc_Tcutil_terr(loc,({const char*_tmp867="array bounds expression is not an unsigned int";
_tag_dyneither(_tmp867,sizeof(char),47);}),_tag_dyneither(_tmp866,sizeof(void*),
0));});{unsigned int _tmp869;int _tmp86A;struct _tuple13 _tmp868=Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)_tmp859);_tmp869=_tmp868.f1;_tmp86A=_tmp868.f2;if((
is_zero_terminated  && _tmp86A) && _tmp869 < 1)({void*_tmp86B=0;Cyc_Tcutil_warn(
loc,({const char*_tmp86C="zero terminated array cannot have zero elements";
_tag_dyneither(_tmp86C,sizeof(char),48);}),_tag_dyneither(_tmp86B,sizeof(void*),
0));});if((_tmp86A  && _tmp869 < 1) && Cyc_Cyclone_tovc_r){({void*_tmp86D=0;Cyc_Tcutil_warn(
loc,({const char*_tmp86E="arrays with 0 elements are not supported except with gcc -- changing to 1.";
_tag_dyneither(_tmp86E,sizeof(char),75);}),_tag_dyneither(_tmp86D,sizeof(void*),
0));});*_tmp7AE=(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(1,0);}goto _LL4CA;}}}
_LL4E7: if(*((int*)_tmp789)!= 8)goto _LL4E9;_tmp7B1=((struct Cyc_Absyn_FnType_struct*)
_tmp789)->f1;_tmp7B2=_tmp7B1.tvars;_tmp7B3=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_struct*)
_tmp789)->f1).tvars;_tmp7B4=_tmp7B1.effect;_tmp7B5=(struct Cyc_Core_Opt**)&(((
struct Cyc_Absyn_FnType_struct*)_tmp789)->f1).effect;_tmp7B6=_tmp7B1.ret_typ;
_tmp7B7=_tmp7B1.args;_tmp7B8=_tmp7B1.c_varargs;_tmp7B9=_tmp7B1.cyc_varargs;
_tmp7BA=_tmp7B1.rgn_po;_tmp7BB=_tmp7B1.attributes;_LL4E8: {int num_convs=0;int
seen_cdecl=0;int seen_stdcall=0;int seen_fastcall=0;int seen_format=0;void*ft=(void*)
0;int fmt_desc_arg=- 1;int fmt_arg_start=- 1;for(0;_tmp7BB != 0;_tmp7BB=_tmp7BB->tl){
if(!Cyc_Absyn_fntype_att((void*)_tmp7BB->hd))({struct Cyc_String_pa_struct _tmp871;
_tmp871.tag=0;_tmp871.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((
void*)_tmp7BB->hd));{void*_tmp86F[1]={& _tmp871};Cyc_Tcutil_terr(loc,({const char*
_tmp870="bad function type attribute %s";_tag_dyneither(_tmp870,sizeof(char),31);}),
_tag_dyneither(_tmp86F,sizeof(void*),1));}});{void*_tmp872=(void*)_tmp7BB->hd;
void*_tmp873;int _tmp874;int _tmp875;_LL545: if((int)_tmp872 != 0)goto _LL547;_LL546:
if(!seen_stdcall){seen_stdcall=1;++ num_convs;}goto _LL544;_LL547: if((int)_tmp872
!= 1)goto _LL549;_LL548: if(!seen_cdecl){seen_cdecl=1;++ num_convs;}goto _LL544;
_LL549: if((int)_tmp872 != 2)goto _LL54B;_LL54A: if(!seen_fastcall){seen_fastcall=1;
++ num_convs;}goto _LL544;_LL54B: if(_tmp872 <= (void*)17)goto _LL54D;if(*((int*)
_tmp872)!= 3)goto _LL54D;_tmp873=(void*)((struct Cyc_Absyn_Format_att_struct*)
_tmp872)->f1;_tmp874=((struct Cyc_Absyn_Format_att_struct*)_tmp872)->f2;_tmp875=((
struct Cyc_Absyn_Format_att_struct*)_tmp872)->f3;_LL54C: if(!seen_format){
seen_format=1;ft=_tmp873;fmt_desc_arg=_tmp874;fmt_arg_start=_tmp875;}else{({void*
_tmp876=0;Cyc_Tcutil_terr(loc,({const char*_tmp877="function can't have multiple format attributes";
_tag_dyneither(_tmp877,sizeof(char),47);}),_tag_dyneither(_tmp876,sizeof(void*),
0));});}goto _LL544;_LL54D:;_LL54E: goto _LL544;_LL544:;}}if(num_convs > 1)({void*
_tmp878=0;Cyc_Tcutil_terr(loc,({const char*_tmp879="function can't have multiple calling conventions";
_tag_dyneither(_tmp879,sizeof(char),49);}),_tag_dyneither(_tmp878,sizeof(void*),
0));});Cyc_Tcutil_check_unique_tvars(loc,*_tmp7B3);{struct Cyc_List_List*b=*
_tmp7B3;for(0;b != 0;b=b->tl){((struct Cyc_Absyn_Tvar*)b->hd)->identity=Cyc_Tcutil_new_tvar_id();
cvtenv.kind_env=Cyc_Tcutil_add_bound_tvar(cvtenv.kind_env,(struct Cyc_Absyn_Tvar*)
b->hd);{void*_tmp87A=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)b->hd)->kind);
void*_tmp87B;_LL550: if(*((int*)_tmp87A)!= 0)goto _LL552;_tmp87B=(void*)((struct
Cyc_Absyn_Eq_kb_struct*)_tmp87A)->f1;if((int)_tmp87B != 1)goto _LL552;_LL551:({
struct Cyc_String_pa_struct _tmp87E;_tmp87E.tag=0;_tmp87E.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)b->hd)->name);{void*_tmp87C[1]={&
_tmp87E};Cyc_Tcutil_terr(loc,({const char*_tmp87D="function attempts to abstract Mem type variable %s";
_tag_dyneither(_tmp87D,sizeof(char),51);}),_tag_dyneither(_tmp87C,sizeof(void*),
1));}});goto _LL54F;_LL552:;_LL553: goto _LL54F;_LL54F:;}}}{struct _RegionHandle*
_tmp87F=Cyc_Tcenv_get_fnrgn(te);{struct Cyc_Tcutil_CVTEnv _tmp880=({struct Cyc_Tcutil_CVTEnv
_tmp900;_tmp900.r=_tmp87F;_tmp900.kind_env=cvtenv.kind_env;_tmp900.free_vars=0;
_tmp900.free_evars=0;_tmp900.generalize_evars=cvtenv.generalize_evars;_tmp900.fn_result=
1;_tmp900;});_tmp880=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp880,(void*)1,
_tmp7B6,1);_tmp880.fn_result=0;{struct Cyc_List_List*a=_tmp7B7;for(0;a != 0;a=a->tl){
struct _tuple9*_tmp881=(struct _tuple9*)a->hd;void*_tmp882=(*_tmp881).f3;_tmp880=
Cyc_Tcutil_i_check_valid_type(loc,te,_tmp880,(void*)1,_tmp882,1);((*_tmp881).f2).real_const=
Cyc_Tcutil_extract_const_from_typedef(loc,((*_tmp881).f2).print_const,_tmp882);}}
if(_tmp7B9 != 0){if(_tmp7B8)({void*_tmp883=0;((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp884="both c_vararg and cyc_vararg";
_tag_dyneither(_tmp884,sizeof(char),29);}),_tag_dyneither(_tmp883,sizeof(void*),
0));});{void*_tmp886;int _tmp887;struct Cyc_Absyn_VarargInfo _tmp885=*_tmp7B9;
_tmp886=_tmp885.type;_tmp887=_tmp885.inject;_tmp880=Cyc_Tcutil_i_check_valid_type(
loc,te,_tmp880,(void*)1,_tmp886,1);(_tmp7B9->tq).real_const=Cyc_Tcutil_extract_const_from_typedef(
loc,(_tmp7B9->tq).print_const,_tmp886);if(_tmp887){void*_tmp888=Cyc_Tcutil_compress(
_tmp886);_LL555: if(_tmp888 <= (void*)4)goto _LL557;if(*((int*)_tmp888)!= 2)goto
_LL557;_LL556: goto _LL554;_LL557:;_LL558:({void*_tmp889=0;Cyc_Tcutil_terr(loc,({
const char*_tmp88A="can't inject a non-datatype type";_tag_dyneither(_tmp88A,
sizeof(char),33);}),_tag_dyneither(_tmp889,sizeof(void*),0));});goto _LL554;
_LL554:;}}}if(seen_format){int _tmp88B=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp7B7);if((((fmt_desc_arg < 0  || fmt_desc_arg > _tmp88B) || fmt_arg_start < 0)
 || _tmp7B9 == 0  && fmt_arg_start != 0) || _tmp7B9 != 0  && fmt_arg_start != _tmp88B + 
1)({void*_tmp88C=0;Cyc_Tcutil_terr(loc,({const char*_tmp88D="bad format descriptor";
_tag_dyneither(_tmp88D,sizeof(char),22);}),_tag_dyneither(_tmp88C,sizeof(void*),
0));});else{void*_tmp88F;struct _tuple9 _tmp88E=*((struct _tuple9*(*)(struct Cyc_List_List*
x,int n))Cyc_List_nth)(_tmp7B7,fmt_desc_arg - 1);_tmp88F=_tmp88E.f3;{void*_tmp890=
Cyc_Tcutil_compress(_tmp88F);struct Cyc_Absyn_PtrInfo _tmp891;void*_tmp892;struct
Cyc_Absyn_PtrAtts _tmp893;union Cyc_Absyn_Constraint*_tmp894;union Cyc_Absyn_Constraint*
_tmp895;_LL55A: if(_tmp890 <= (void*)4)goto _LL55C;if(*((int*)_tmp890)!= 4)goto
_LL55C;_tmp891=((struct Cyc_Absyn_PointerType_struct*)_tmp890)->f1;_tmp892=
_tmp891.elt_typ;_tmp893=_tmp891.ptr_atts;_tmp894=_tmp893.bounds;_tmp895=_tmp893.zero_term;
_LL55B:{struct _tuple0 _tmp897=({struct _tuple0 _tmp896;_tmp896.f1=Cyc_Tcutil_compress(
_tmp892);_tmp896.f2=Cyc_Absyn_conref_def((void*)((void*)0),_tmp894);_tmp896;});
void*_tmp898;void*_tmp899;void*_tmp89A;void*_tmp89B;_LL55F: _tmp898=_tmp897.f1;
if(_tmp898 <= (void*)4)goto _LL561;if(*((int*)_tmp898)!= 5)goto _LL561;_tmp899=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp898)->f1;if((int)_tmp899 != 2)goto
_LL561;_tmp89A=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp898)->f2;if((int)
_tmp89A != 0)goto _LL561;_tmp89B=_tmp897.f2;if((int)_tmp89B != 0)goto _LL561;_LL560:
goto _LL55E;_LL561:;_LL562:({void*_tmp89C=0;Cyc_Tcutil_terr(loc,({const char*
_tmp89D="format descriptor is not a char ? type";_tag_dyneither(_tmp89D,sizeof(
char),39);}),_tag_dyneither(_tmp89C,sizeof(void*),0));});goto _LL55E;_LL55E:;}
goto _LL559;_LL55C:;_LL55D:({void*_tmp89E=0;Cyc_Tcutil_terr(loc,({const char*
_tmp89F="format descriptor is not a char ? type";_tag_dyneither(_tmp89F,sizeof(
char),39);}),_tag_dyneither(_tmp89E,sizeof(void*),0));});goto _LL559;_LL559:;}if(
fmt_arg_start != 0){int problem;{struct _tuple0 _tmp8A1=({struct _tuple0 _tmp8A0;
_tmp8A0.f1=ft;_tmp8A0.f2=Cyc_Tcutil_compress(((struct Cyc_Absyn_VarargInfo*)
_check_null(_tmp7B9))->type);_tmp8A0;});void*_tmp8A2;void*_tmp8A3;struct Cyc_Absyn_DatatypeInfo
_tmp8A4;union Cyc_Absyn_DatatypeInfoU _tmp8A5;struct Cyc_Absyn_Datatypedecl**
_tmp8A6;struct Cyc_Absyn_Datatypedecl*_tmp8A7;void*_tmp8A8;void*_tmp8A9;struct Cyc_Absyn_DatatypeInfo
_tmp8AA;union Cyc_Absyn_DatatypeInfoU _tmp8AB;struct Cyc_Absyn_Datatypedecl**
_tmp8AC;struct Cyc_Absyn_Datatypedecl*_tmp8AD;_LL564: _tmp8A2=_tmp8A1.f1;if((int)
_tmp8A2 != 0)goto _LL566;_tmp8A3=_tmp8A1.f2;if(_tmp8A3 <= (void*)4)goto _LL566;if(*((
int*)_tmp8A3)!= 2)goto _LL566;_tmp8A4=((struct Cyc_Absyn_DatatypeType_struct*)
_tmp8A3)->f1;_tmp8A5=_tmp8A4.datatype_info;if((_tmp8A5.KnownDatatype).tag != 2)
goto _LL566;_tmp8A6=(struct Cyc_Absyn_Datatypedecl**)(_tmp8A5.KnownDatatype).val;
_tmp8A7=*_tmp8A6;_LL565: problem=Cyc_Absyn_qvar_cmp(_tmp8A7->name,Cyc_Absyn_datatype_print_arg_qvar)
!= 0;goto _LL563;_LL566: _tmp8A8=_tmp8A1.f1;if((int)_tmp8A8 != 1)goto _LL568;_tmp8A9=
_tmp8A1.f2;if(_tmp8A9 <= (void*)4)goto _LL568;if(*((int*)_tmp8A9)!= 2)goto _LL568;
_tmp8AA=((struct Cyc_Absyn_DatatypeType_struct*)_tmp8A9)->f1;_tmp8AB=_tmp8AA.datatype_info;
if((_tmp8AB.KnownDatatype).tag != 2)goto _LL568;_tmp8AC=(struct Cyc_Absyn_Datatypedecl**)(
_tmp8AB.KnownDatatype).val;_tmp8AD=*_tmp8AC;_LL567: problem=Cyc_Absyn_qvar_cmp(
_tmp8AD->name,Cyc_Absyn_datatype_scanf_arg_qvar)!= 0;goto _LL563;_LL568:;_LL569:
problem=1;goto _LL563;_LL563:;}if(problem)({void*_tmp8AE=0;Cyc_Tcutil_terr(loc,({
const char*_tmp8AF="format attribute and vararg types don't match";_tag_dyneither(
_tmp8AF,sizeof(char),46);}),_tag_dyneither(_tmp8AE,sizeof(void*),0));});}}}{
struct Cyc_List_List*rpo=_tmp7BA;for(0;rpo != 0;rpo=rpo->tl){struct _tuple0 _tmp8B1;
void*_tmp8B2;void*_tmp8B3;struct _tuple0*_tmp8B0=(struct _tuple0*)rpo->hd;_tmp8B1=*
_tmp8B0;_tmp8B2=_tmp8B1.f1;_tmp8B3=_tmp8B1.f2;_tmp880=Cyc_Tcutil_i_check_valid_type(
loc,te,_tmp880,(void*)6,_tmp8B2,1);_tmp880=Cyc_Tcutil_i_check_valid_type(loc,te,
_tmp880,(void*)5,_tmp8B3,1);}}if(*_tmp7B5 != 0)_tmp880=Cyc_Tcutil_i_check_valid_type(
loc,te,_tmp880,(void*)6,(void*)((struct Cyc_Core_Opt*)_check_null(*_tmp7B5))->v,1);
else{struct Cyc_List_List*effect=0;{struct Cyc_List_List*tvs=_tmp880.free_vars;
for(0;tvs != 0;tvs=tvs->tl){struct Cyc_Absyn_Tvar*_tmp8B5;int _tmp8B6;struct
_tuple23 _tmp8B4=*((struct _tuple23*)tvs->hd);_tmp8B5=_tmp8B4.f1;_tmp8B6=_tmp8B4.f2;
if(!_tmp8B6)continue;{void*_tmp8B7=Cyc_Absyn_compress_kb(_tmp8B5->kind);struct
Cyc_Core_Opt*_tmp8B8;struct Cyc_Core_Opt**_tmp8B9;void*_tmp8BA;struct Cyc_Core_Opt*
_tmp8BB;struct Cyc_Core_Opt**_tmp8BC;void*_tmp8BD;void*_tmp8BE;void*_tmp8BF;void*
_tmp8C0;struct Cyc_Core_Opt*_tmp8C1;struct Cyc_Core_Opt**_tmp8C2;void*_tmp8C3;void*
_tmp8C4;struct Cyc_Core_Opt*_tmp8C5;struct Cyc_Core_Opt**_tmp8C6;_LL56B: if(*((int*)
_tmp8B7)!= 2)goto _LL56D;_tmp8B8=((struct Cyc_Absyn_Less_kb_struct*)_tmp8B7)->f1;
_tmp8B9=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)_tmp8B7)->f1;
_tmp8BA=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp8B7)->f2;if((int)_tmp8BA != 
5)goto _LL56D;_LL56C: _tmp8BC=_tmp8B9;_tmp8BD=(void*)3;goto _LL56E;_LL56D: if(*((int*)
_tmp8B7)!= 2)goto _LL56F;_tmp8BB=((struct Cyc_Absyn_Less_kb_struct*)_tmp8B7)->f1;
_tmp8BC=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)_tmp8B7)->f1;
_tmp8BD=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp8B7)->f2;if(!(_tmp8BD == (
void*)3  || _tmp8BD == (void*)4))goto _LL56F;_LL56E:*_tmp8BC=Cyc_Tcutil_kind_to_bound_opt(
_tmp8BD);_tmp8BE=_tmp8BD;goto _LL570;_LL56F: if(*((int*)_tmp8B7)!= 0)goto _LL571;
_tmp8BE=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp8B7)->f1;if(!((_tmp8BE == (
void*)3  || _tmp8BE == (void*)4) || _tmp8BE == (void*)5))goto _LL571;_LL570: effect=({
struct Cyc_List_List*_tmp8C7=_cycalloc(sizeof(*_tmp8C7));_tmp8C7->hd=(void*)((
void*)({struct Cyc_Absyn_AccessEff_struct*_tmp8C8=_cycalloc(sizeof(*_tmp8C8));
_tmp8C8[0]=({struct Cyc_Absyn_AccessEff_struct _tmp8C9;_tmp8C9.tag=19;_tmp8C9.f1=(
void*)((void*)({struct Cyc_Absyn_VarType_struct*_tmp8CA=_cycalloc(sizeof(*_tmp8CA));
_tmp8CA[0]=({struct Cyc_Absyn_VarType_struct _tmp8CB;_tmp8CB.tag=1;_tmp8CB.f1=
_tmp8B5;_tmp8CB;});_tmp8CA;}));_tmp8C9;});_tmp8C8;}));_tmp8C7->tl=effect;_tmp8C7;});
goto _LL56A;_LL571: if(*((int*)_tmp8B7)!= 2)goto _LL573;_tmp8BF=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp8B7)->f2;if((int)_tmp8BF != 7)goto _LL573;_LL572: goto _LL574;_LL573: if(*((int*)
_tmp8B7)!= 0)goto _LL575;_tmp8C0=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp8B7)->f1;
if((int)_tmp8C0 != 7)goto _LL575;_LL574: goto _LL56A;_LL575: if(*((int*)_tmp8B7)!= 2)
goto _LL577;_tmp8C1=((struct Cyc_Absyn_Less_kb_struct*)_tmp8B7)->f1;_tmp8C2=(
struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)_tmp8B7)->f1;_tmp8C3=(
void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp8B7)->f2;if((int)_tmp8C3 != 6)goto
_LL577;_LL576:*_tmp8C2=Cyc_Tcutil_kind_to_bound_opt((void*)6);goto _LL578;_LL577:
if(*((int*)_tmp8B7)!= 0)goto _LL579;_tmp8C4=(void*)((struct Cyc_Absyn_Eq_kb_struct*)
_tmp8B7)->f1;if((int)_tmp8C4 != 6)goto _LL579;_LL578: effect=({struct Cyc_List_List*
_tmp8CC=_cycalloc(sizeof(*_tmp8CC));_tmp8CC->hd=(void*)((void*)({struct Cyc_Absyn_VarType_struct*
_tmp8CD=_cycalloc(sizeof(*_tmp8CD));_tmp8CD[0]=({struct Cyc_Absyn_VarType_struct
_tmp8CE;_tmp8CE.tag=1;_tmp8CE.f1=_tmp8B5;_tmp8CE;});_tmp8CD;}));_tmp8CC->tl=
effect;_tmp8CC;});goto _LL56A;_LL579: if(*((int*)_tmp8B7)!= 1)goto _LL57B;_tmp8C5=((
struct Cyc_Absyn_Unknown_kb_struct*)_tmp8B7)->f1;_tmp8C6=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Unknown_kb_struct*)_tmp8B7)->f1;_LL57A:*_tmp8C6=({struct Cyc_Core_Opt*
_tmp8CF=_cycalloc(sizeof(*_tmp8CF));_tmp8CF->v=(void*)((void*)({struct Cyc_Absyn_Less_kb_struct*
_tmp8D0=_cycalloc(sizeof(*_tmp8D0));_tmp8D0[0]=({struct Cyc_Absyn_Less_kb_struct
_tmp8D1;_tmp8D1.tag=2;_tmp8D1.f1=0;_tmp8D1.f2=(void*)((void*)0);_tmp8D1;});
_tmp8D0;}));_tmp8CF;});goto _LL57C;_LL57B:;_LL57C: effect=({struct Cyc_List_List*
_tmp8D2=_cycalloc(sizeof(*_tmp8D2));_tmp8D2->hd=(void*)((void*)({struct Cyc_Absyn_RgnsEff_struct*
_tmp8D3=_cycalloc(sizeof(*_tmp8D3));_tmp8D3[0]=({struct Cyc_Absyn_RgnsEff_struct
_tmp8D4;_tmp8D4.tag=21;_tmp8D4.f1=(void*)((void*)({struct Cyc_Absyn_VarType_struct*
_tmp8D5=_cycalloc(sizeof(*_tmp8D5));_tmp8D5[0]=({struct Cyc_Absyn_VarType_struct
_tmp8D6;_tmp8D6.tag=1;_tmp8D6.f1=_tmp8B5;_tmp8D6;});_tmp8D5;}));_tmp8D4;});
_tmp8D3;}));_tmp8D2->tl=effect;_tmp8D2;});goto _LL56A;_LL56A:;}}}{struct Cyc_List_List*
ts=_tmp880.free_evars;for(0;ts != 0;ts=ts->tl){void*_tmp8D8;int _tmp8D9;struct
_tuple7 _tmp8D7=*((struct _tuple7*)ts->hd);_tmp8D8=_tmp8D7.f1;_tmp8D9=_tmp8D7.f2;
if(!_tmp8D9)continue;{void*_tmp8DA=Cyc_Tcutil_typ_kind(_tmp8D8);_LL57E: if((int)
_tmp8DA != 5)goto _LL580;_LL57F: goto _LL581;_LL580: if((int)_tmp8DA != 4)goto _LL582;
_LL581: goto _LL583;_LL582: if((int)_tmp8DA != 3)goto _LL584;_LL583: effect=({struct
Cyc_List_List*_tmp8DB=_cycalloc(sizeof(*_tmp8DB));_tmp8DB->hd=(void*)((void*)({
struct Cyc_Absyn_AccessEff_struct*_tmp8DC=_cycalloc(sizeof(*_tmp8DC));_tmp8DC[0]=({
struct Cyc_Absyn_AccessEff_struct _tmp8DD;_tmp8DD.tag=19;_tmp8DD.f1=(void*)_tmp8D8;
_tmp8DD;});_tmp8DC;}));_tmp8DB->tl=effect;_tmp8DB;});goto _LL57D;_LL584: if((int)
_tmp8DA != 6)goto _LL586;_LL585: effect=({struct Cyc_List_List*_tmp8DE=_cycalloc(
sizeof(*_tmp8DE));_tmp8DE->hd=(void*)_tmp8D8;_tmp8DE->tl=effect;_tmp8DE;});goto
_LL57D;_LL586: if((int)_tmp8DA != 7)goto _LL588;_LL587: goto _LL57D;_LL588:;_LL589:
effect=({struct Cyc_List_List*_tmp8DF=_cycalloc(sizeof(*_tmp8DF));_tmp8DF->hd=(
void*)((void*)({struct Cyc_Absyn_RgnsEff_struct*_tmp8E0=_cycalloc(sizeof(*_tmp8E0));
_tmp8E0[0]=({struct Cyc_Absyn_RgnsEff_struct _tmp8E1;_tmp8E1.tag=21;_tmp8E1.f1=(
void*)_tmp8D8;_tmp8E1;});_tmp8E0;}));_tmp8DF->tl=effect;_tmp8DF;});goto _LL57D;
_LL57D:;}}}*_tmp7B5=({struct Cyc_Core_Opt*_tmp8E2=_cycalloc(sizeof(*_tmp8E2));
_tmp8E2->v=(void*)((void*)({struct Cyc_Absyn_JoinEff_struct*_tmp8E3=_cycalloc(
sizeof(*_tmp8E3));_tmp8E3[0]=({struct Cyc_Absyn_JoinEff_struct _tmp8E4;_tmp8E4.tag=
20;_tmp8E4.f1=effect;_tmp8E4;});_tmp8E3;}));_tmp8E2;});}if(*_tmp7B3 != 0){struct
Cyc_List_List*bs=*_tmp7B3;for(0;bs != 0;bs=bs->tl){void*_tmp8E5=Cyc_Absyn_compress_kb(((
struct Cyc_Absyn_Tvar*)bs->hd)->kind);struct Cyc_Core_Opt*_tmp8E6;struct Cyc_Core_Opt**
_tmp8E7;struct Cyc_Core_Opt*_tmp8E8;struct Cyc_Core_Opt**_tmp8E9;void*_tmp8EA;
struct Cyc_Core_Opt*_tmp8EB;struct Cyc_Core_Opt**_tmp8EC;void*_tmp8ED;struct Cyc_Core_Opt*
_tmp8EE;struct Cyc_Core_Opt**_tmp8EF;void*_tmp8F0;struct Cyc_Core_Opt*_tmp8F1;
struct Cyc_Core_Opt**_tmp8F2;void*_tmp8F3;void*_tmp8F4;_LL58B: if(*((int*)_tmp8E5)
!= 1)goto _LL58D;_tmp8E6=((struct Cyc_Absyn_Unknown_kb_struct*)_tmp8E5)->f1;
_tmp8E7=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)_tmp8E5)->f1;
_LL58C:({struct Cyc_String_pa_struct _tmp8F7;_tmp8F7.tag=0;_tmp8F7.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)bs->hd)->name);{
void*_tmp8F5[1]={& _tmp8F7};Cyc_Tcutil_warn(loc,({const char*_tmp8F6="Type variable %s unconstrained, assuming boxed";
_tag_dyneither(_tmp8F6,sizeof(char),47);}),_tag_dyneither(_tmp8F5,sizeof(void*),
1));}});_tmp8E9=_tmp8E7;goto _LL58E;_LL58D: if(*((int*)_tmp8E5)!= 2)goto _LL58F;
_tmp8E8=((struct Cyc_Absyn_Less_kb_struct*)_tmp8E5)->f1;_tmp8E9=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*)_tmp8E5)->f1;_tmp8EA=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp8E5)->f2;if((int)_tmp8EA != 1)goto _LL58F;_LL58E: _tmp8EC=_tmp8E9;goto _LL590;
_LL58F: if(*((int*)_tmp8E5)!= 2)goto _LL591;_tmp8EB=((struct Cyc_Absyn_Less_kb_struct*)
_tmp8E5)->f1;_tmp8EC=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp8E5)->f1;_tmp8ED=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp8E5)->f2;if((
int)_tmp8ED != 0)goto _LL591;_LL590:*_tmp8EC=Cyc_Tcutil_kind_to_bound_opt((void*)2);
goto _LL58A;_LL591: if(*((int*)_tmp8E5)!= 2)goto _LL593;_tmp8EE=((struct Cyc_Absyn_Less_kb_struct*)
_tmp8E5)->f1;_tmp8EF=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp8E5)->f1;_tmp8F0=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp8E5)->f2;if((
int)_tmp8F0 != 5)goto _LL593;_LL592:*_tmp8EF=Cyc_Tcutil_kind_to_bound_opt((void*)3);
goto _LL58A;_LL593: if(*((int*)_tmp8E5)!= 2)goto _LL595;_tmp8F1=((struct Cyc_Absyn_Less_kb_struct*)
_tmp8E5)->f1;_tmp8F2=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp8E5)->f1;_tmp8F3=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp8E5)->f2;
_LL594:*_tmp8F2=Cyc_Tcutil_kind_to_bound_opt(_tmp8F3);goto _LL58A;_LL595: if(*((
int*)_tmp8E5)!= 0)goto _LL597;_tmp8F4=(void*)((struct Cyc_Absyn_Eq_kb_struct*)
_tmp8E5)->f1;if((int)_tmp8F4 != 1)goto _LL597;_LL596:({void*_tmp8F8=0;Cyc_Tcutil_terr(
loc,({const char*_tmp8F9="functions can't abstract M types";_tag_dyneither(
_tmp8F9,sizeof(char),33);}),_tag_dyneither(_tmp8F8,sizeof(void*),0));});goto
_LL58A;_LL597:;_LL598: goto _LL58A;_LL58A:;}}cvtenv.kind_env=Cyc_Tcutil_remove_bound_tvars(
Cyc_Core_heap_region,_tmp880.kind_env,*_tmp7B3);_tmp880.free_vars=Cyc_Tcutil_remove_bound_tvars_bool(
_tmp880.r,_tmp880.free_vars,*_tmp7B3);{struct Cyc_List_List*tvs=_tmp880.free_vars;
for(0;tvs != 0;tvs=tvs->tl){struct Cyc_Absyn_Tvar*_tmp8FB;int _tmp8FC;struct
_tuple23 _tmp8FA=*((struct _tuple23*)tvs->hd);_tmp8FB=_tmp8FA.f1;_tmp8FC=_tmp8FA.f2;
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,
_tmp8FB,_tmp8FC);}}{struct Cyc_List_List*evs=_tmp880.free_evars;for(0;evs != 0;evs=
evs->tl){void*_tmp8FE;int _tmp8FF;struct _tuple7 _tmp8FD=*((struct _tuple7*)evs->hd);
_tmp8FE=_tmp8FD.f1;_tmp8FF=_tmp8FD.f2;cvtenv.free_evars=Cyc_Tcutil_add_free_evar(
cvtenv.r,cvtenv.free_evars,_tmp8FE,_tmp8FF);}}}goto _LL4CA;}}_LL4E9: if(*((int*)
_tmp789)!= 9)goto _LL4EB;_tmp7BC=((struct Cyc_Absyn_TupleType_struct*)_tmp789)->f1;
_LL4EA: for(0;_tmp7BC != 0;_tmp7BC=_tmp7BC->tl){struct _tuple11*_tmp901=(struct
_tuple11*)_tmp7BC->hd;cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)
1,(*_tmp901).f2,1);((*_tmp901).f1).real_const=Cyc_Tcutil_extract_const_from_typedef(
loc,((*_tmp901).f1).print_const,(*_tmp901).f2);}goto _LL4CA;_LL4EB: if(*((int*)
_tmp789)!= 11)goto _LL4ED;_tmp7BD=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp789)->f1;_tmp7BE=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp789)->f2;_LL4EC: {
struct _RegionHandle*_tmp902=Cyc_Tcenv_get_fnrgn(te);{struct Cyc_List_List*
prev_fields=0;for(0;_tmp7BE != 0;_tmp7BE=_tmp7BE->tl){struct Cyc_Absyn_Aggrfield
_tmp904;struct _dyneither_ptr*_tmp905;struct Cyc_Absyn_Tqual _tmp906;struct Cyc_Absyn_Tqual*
_tmp907;void*_tmp908;struct Cyc_Absyn_Exp*_tmp909;struct Cyc_List_List*_tmp90A;
struct Cyc_Absyn_Aggrfield*_tmp903=(struct Cyc_Absyn_Aggrfield*)_tmp7BE->hd;
_tmp904=*_tmp903;_tmp905=_tmp904.name;_tmp906=_tmp904.tq;_tmp907=(struct Cyc_Absyn_Tqual*)&(*
_tmp903).tq;_tmp908=_tmp904.type;_tmp909=_tmp904.width;_tmp90A=_tmp904.attributes;
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*
l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,_tmp905))({
struct Cyc_String_pa_struct _tmp90D;_tmp90D.tag=0;_tmp90D.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)*_tmp905);{void*_tmp90B[1]={& _tmp90D};Cyc_Tcutil_terr(loc,({
const char*_tmp90C="duplicate field %s";_tag_dyneither(_tmp90C,sizeof(char),19);}),
_tag_dyneither(_tmp90B,sizeof(void*),1));}});if(Cyc_strcmp((struct _dyneither_ptr)*
_tmp905,({const char*_tmp90E="";_tag_dyneither(_tmp90E,sizeof(char),1);}))!= 0)
prev_fields=({struct Cyc_List_List*_tmp90F=_region_malloc(_tmp902,sizeof(*_tmp90F));
_tmp90F->hd=_tmp905;_tmp90F->tl=prev_fields;_tmp90F;});cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,(void*)1,_tmp908,1);_tmp907->real_const=Cyc_Tcutil_extract_const_from_typedef(
loc,_tmp907->print_const,_tmp908);if(_tmp7BD == (void*)1  && !Cyc_Tcutil_bits_only(
_tmp908))({struct Cyc_String_pa_struct _tmp912;_tmp912.tag=0;_tmp912.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*_tmp905);{void*_tmp910[1]={& _tmp912};Cyc_Tcutil_warn(
loc,({const char*_tmp911="union member %s is not `bits-only' so it can only be written and not read";
_tag_dyneither(_tmp911,sizeof(char),74);}),_tag_dyneither(_tmp910,sizeof(void*),
1));}});Cyc_Tcutil_check_bitfield(loc,te,_tmp908,_tmp909,_tmp905);Cyc_Tcutil_check_field_atts(
loc,_tmp905,_tmp90A);}}goto _LL4CA;}_LL4ED: if(*((int*)_tmp789)!= 10)goto _LL4EF;
_tmp7BF=((struct Cyc_Absyn_AggrType_struct*)_tmp789)->f1;_tmp7C0=_tmp7BF.aggr_info;
_tmp7C1=(union Cyc_Absyn_AggrInfoU*)&(((struct Cyc_Absyn_AggrType_struct*)_tmp789)->f1).aggr_info;
_tmp7C2=_tmp7BF.targs;_tmp7C3=(struct Cyc_List_List**)&(((struct Cyc_Absyn_AggrType_struct*)
_tmp789)->f1).targs;_LL4EE:{union Cyc_Absyn_AggrInfoU _tmp913=*_tmp7C1;struct
_tuple4 _tmp914;void*_tmp915;struct _tuple2*_tmp916;struct Cyc_Core_Opt*_tmp917;
struct Cyc_Absyn_Aggrdecl**_tmp918;struct Cyc_Absyn_Aggrdecl*_tmp919;_LL59A: if((
_tmp913.UnknownAggr).tag != 1)goto _LL59C;_tmp914=(struct _tuple4)(_tmp913.UnknownAggr).val;
_tmp915=_tmp914.f1;_tmp916=_tmp914.f2;_tmp917=_tmp914.f3;_LL59B: {struct Cyc_Absyn_Aggrdecl**
adp;{struct _handler_cons _tmp91A;_push_handler(& _tmp91A);{int _tmp91C=0;if(setjmp(
_tmp91A.handler))_tmp91C=1;if(!_tmp91C){adp=Cyc_Tcenv_lookup_aggrdecl(te,loc,
_tmp916);{struct Cyc_Absyn_Aggrdecl*_tmp91D=*adp;if(_tmp91D->kind != _tmp915){if(
_tmp91D->kind == (void*)0)({struct Cyc_String_pa_struct _tmp921;_tmp921.tag=0;
_tmp921.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp916));{struct Cyc_String_pa_struct _tmp920;_tmp920.tag=0;_tmp920.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp916));{void*
_tmp91E[2]={& _tmp920,& _tmp921};Cyc_Tcutil_terr(loc,({const char*_tmp91F="expecting struct %s instead of union %s";
_tag_dyneither(_tmp91F,sizeof(char),40);}),_tag_dyneither(_tmp91E,sizeof(void*),
2));}}});else{({struct Cyc_String_pa_struct _tmp925;_tmp925.tag=0;_tmp925.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp916));{
struct Cyc_String_pa_struct _tmp924;_tmp924.tag=0;_tmp924.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp916));{void*_tmp922[2]={&
_tmp924,& _tmp925};Cyc_Tcutil_terr(loc,({const char*_tmp923="expecting union %s instead of struct %s";
_tag_dyneither(_tmp923,sizeof(char),40);}),_tag_dyneither(_tmp922,sizeof(void*),
2));}}});}}if((unsigned int)_tmp917  && (int)_tmp917->v){if(!((unsigned int)
_tmp91D->impl) || !((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp91D->impl))->tagged)({
struct Cyc_String_pa_struct _tmp928;_tmp928.tag=0;_tmp928.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp916));{void*_tmp926[1]={&
_tmp928};Cyc_Tcutil_terr(loc,({const char*_tmp927="@tagged qualfiers don't agree on union %s";
_tag_dyneither(_tmp927,sizeof(char),42);}),_tag_dyneither(_tmp926,sizeof(void*),
1));}});}*_tmp7C1=Cyc_Absyn_KnownAggr(adp);};_pop_handler();}else{void*_tmp91B=(
void*)_exn_thrown;void*_tmp92A=_tmp91B;_LL59F: if(_tmp92A != Cyc_Dict_Absent)goto
_LL5A1;_LL5A0: {struct Cyc_Tcenv_Genv*_tmp92B=((struct Cyc_Tcenv_Genv*(*)(struct
Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);struct Cyc_Absyn_Aggrdecl*
_tmp92C=({struct Cyc_Absyn_Aggrdecl*_tmp930=_cycalloc(sizeof(*_tmp930));_tmp930->kind=
_tmp915;_tmp930->sc=(void*)3;_tmp930->name=_tmp916;_tmp930->tvs=0;_tmp930->impl=
0;_tmp930->attributes=0;_tmp930;});Cyc_Tc_tcAggrdecl(te,_tmp92B,loc,_tmp92C);adp=
Cyc_Tcenv_lookup_aggrdecl(te,loc,_tmp916);*_tmp7C1=Cyc_Absyn_KnownAggr(adp);if(*
_tmp7C3 != 0){({struct Cyc_String_pa_struct _tmp92F;_tmp92F.tag=0;_tmp92F.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp916));{
void*_tmp92D[1]={& _tmp92F};Cyc_Tcutil_terr(loc,({const char*_tmp92E="declare parameterized type %s before using";
_tag_dyneither(_tmp92E,sizeof(char),43);}),_tag_dyneither(_tmp92D,sizeof(void*),
1));}});return cvtenv;}goto _LL59E;}_LL5A1:;_LL5A2:(void)_throw(_tmp92A);_LL59E:;}}}
_tmp919=*adp;goto _LL59D;}_LL59C: if((_tmp913.KnownAggr).tag != 2)goto _LL599;
_tmp918=(struct Cyc_Absyn_Aggrdecl**)(_tmp913.KnownAggr).val;_tmp919=*_tmp918;
_LL59D: {struct Cyc_List_List*tvs=_tmp919->tvs;struct Cyc_List_List*ts=*_tmp7C3;
for(0;ts != 0  && tvs != 0;(ts=ts->tl,tvs=tvs->tl)){void*k=Cyc_Tcutil_tvar_kind((
struct Cyc_Absyn_Tvar*)tvs->hd);cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,
cvtenv,k,(void*)ts->hd,1);}if(ts != 0)({struct Cyc_String_pa_struct _tmp933;_tmp933.tag=
0;_tmp933.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp919->name));{void*_tmp931[1]={& _tmp933};Cyc_Tcutil_terr(loc,({const char*
_tmp932="too many parameters for type %s";_tag_dyneither(_tmp932,sizeof(char),32);}),
_tag_dyneither(_tmp931,sizeof(void*),1));}});if(tvs != 0){struct Cyc_List_List*
hidden_ts=0;for(0;tvs != 0;tvs=tvs->tl){void*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)
tvs->hd);void*e=Cyc_Absyn_new_evar(0,0);hidden_ts=({struct Cyc_List_List*_tmp934=
_cycalloc(sizeof(*_tmp934));_tmp934->hd=(void*)e;_tmp934->tl=hidden_ts;_tmp934;});
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,e,1);}*_tmp7C3=Cyc_List_imp_append(*
_tmp7C3,Cyc_List_imp_rev(hidden_ts));}}_LL599:;}goto _LL4CA;_LL4EF: if(*((int*)
_tmp789)!= 16)goto _LL4F1;_tmp7C4=((struct Cyc_Absyn_TypedefType_struct*)_tmp789)->f1;
_tmp7C5=((struct Cyc_Absyn_TypedefType_struct*)_tmp789)->f2;_tmp7C6=(struct Cyc_List_List**)&((
struct Cyc_Absyn_TypedefType_struct*)_tmp789)->f2;_tmp7C7=((struct Cyc_Absyn_TypedefType_struct*)
_tmp789)->f3;_tmp7C8=(struct Cyc_Absyn_Typedefdecl**)&((struct Cyc_Absyn_TypedefType_struct*)
_tmp789)->f3;_tmp7C9=((struct Cyc_Absyn_TypedefType_struct*)_tmp789)->f4;_tmp7CA=(
void***)&((struct Cyc_Absyn_TypedefType_struct*)_tmp789)->f4;_LL4F0: {struct Cyc_List_List*
targs=*_tmp7C6;struct Cyc_Absyn_Typedefdecl*td;{struct _handler_cons _tmp935;
_push_handler(& _tmp935);{int _tmp937=0;if(setjmp(_tmp935.handler))_tmp937=1;if(!
_tmp937){td=Cyc_Tcenv_lookup_typedefdecl(te,loc,_tmp7C4);;_pop_handler();}else{
void*_tmp936=(void*)_exn_thrown;void*_tmp939=_tmp936;_LL5A4: if(_tmp939 != Cyc_Dict_Absent)
goto _LL5A6;_LL5A5:({struct Cyc_String_pa_struct _tmp93C;_tmp93C.tag=0;_tmp93C.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp7C4));{
void*_tmp93A[1]={& _tmp93C};Cyc_Tcutil_terr(loc,({const char*_tmp93B="unbound typedef name %s";
_tag_dyneither(_tmp93B,sizeof(char),24);}),_tag_dyneither(_tmp93A,sizeof(void*),
1));}});return cvtenv;_LL5A6:;_LL5A7:(void)_throw(_tmp939);_LL5A3:;}}}*_tmp7C8=(
struct Cyc_Absyn_Typedefdecl*)td;_tmp7C4[0]=(td->name)[
_check_known_subscript_notnull(1,0)];{struct Cyc_List_List*tvs=td->tvs;struct Cyc_List_List*
ts=targs;struct _RegionHandle*_tmp93D=Cyc_Tcenv_get_fnrgn(te);{struct Cyc_List_List*
inst=0;for(0;ts != 0  && tvs != 0;(ts=ts->tl,tvs=tvs->tl)){void*k=Cyc_Tcutil_tvar_kind((
struct Cyc_Absyn_Tvar*)tvs->hd);cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,
cvtenv,k,(void*)ts->hd,1);inst=({struct Cyc_List_List*_tmp93E=_region_malloc(
_tmp93D,sizeof(*_tmp93E));_tmp93E->hd=({struct _tuple14*_tmp93F=_region_malloc(
_tmp93D,sizeof(*_tmp93F));_tmp93F->f1=(struct Cyc_Absyn_Tvar*)tvs->hd;_tmp93F->f2=(
void*)ts->hd;_tmp93F;});_tmp93E->tl=inst;_tmp93E;});}if(ts != 0)({struct Cyc_String_pa_struct
_tmp942;_tmp942.tag=0;_tmp942.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp7C4));{void*_tmp940[1]={& _tmp942};Cyc_Tcutil_terr(
loc,({const char*_tmp941="too many parameters for typedef %s";_tag_dyneither(
_tmp941,sizeof(char),35);}),_tag_dyneither(_tmp940,sizeof(void*),1));}});if(tvs
!= 0){struct Cyc_List_List*hidden_ts=0;for(0;tvs != 0;tvs=tvs->tl){void*k=Cyc_Tcutil_tvar_kind((
struct Cyc_Absyn_Tvar*)tvs->hd);void*e=Cyc_Absyn_new_evar(0,0);hidden_ts=({struct
Cyc_List_List*_tmp943=_cycalloc(sizeof(*_tmp943));_tmp943->hd=(void*)e;_tmp943->tl=
hidden_ts;_tmp943;});cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,e,1);
inst=(struct Cyc_List_List*)({struct Cyc_List_List*_tmp944=_cycalloc(sizeof(*
_tmp944));_tmp944->hd=(struct _tuple14*)({struct _tuple14*_tmp945=_cycalloc(
sizeof(*_tmp945));_tmp945->f1=(struct Cyc_Absyn_Tvar*)tvs->hd;_tmp945->f2=e;
_tmp945;});_tmp944->tl=inst;_tmp944;});}*_tmp7C6=Cyc_List_imp_append(targs,Cyc_List_imp_rev(
hidden_ts));}if(td->defn != 0){void*new_typ=Cyc_Tcutil_rsubstitute(_tmp93D,inst,(
void*)((struct Cyc_Core_Opt*)_check_null(td->defn))->v);*_tmp7CA=({void**_tmp946=
_cycalloc(sizeof(*_tmp946));_tmp946[0]=new_typ;_tmp946;});}}goto _LL4CA;}}_LL4F1:
if((int)_tmp789 != 3)goto _LL4F3;_LL4F2: goto _LL4F4;_LL4F3: if((int)_tmp789 != 2)goto
_LL4F5;_LL4F4: goto _LL4CA;_LL4F5: if(_tmp789 <= (void*)4)goto _LL4F7;if(*((int*)
_tmp789)!= 14)goto _LL4F7;_tmp7CB=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp789)->f1;_LL4F6: cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)5,
_tmp7CB,1);goto _LL4CA;_LL4F7: if(_tmp789 <= (void*)4)goto _LL4F9;if(*((int*)_tmp789)
!= 15)goto _LL4F9;_tmp7CC=(void*)((struct Cyc_Absyn_DynRgnType_struct*)_tmp789)->f1;
_tmp7CD=(void*)((struct Cyc_Absyn_DynRgnType_struct*)_tmp789)->f2;_LL4F8: cvtenv=
Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)3,_tmp7CC,0);cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,(void*)3,_tmp7CD,1);goto _LL4CA;_LL4F9: if(_tmp789 <= (void*)4)goto
_LL4FB;if(*((int*)_tmp789)!= 19)goto _LL4FB;_tmp7CE=(void*)((struct Cyc_Absyn_AccessEff_struct*)
_tmp789)->f1;_LL4FA: cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)5,
_tmp7CE,1);goto _LL4CA;_LL4FB: if(_tmp789 <= (void*)4)goto _LL4FD;if(*((int*)_tmp789)
!= 21)goto _LL4FD;_tmp7CF=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp789)->f1;
_LL4FC: cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)0,_tmp7CF,1);
goto _LL4CA;_LL4FD: if(_tmp789 <= (void*)4)goto _LL4CA;if(*((int*)_tmp789)!= 20)goto
_LL4CA;_tmp7D0=((struct Cyc_Absyn_JoinEff_struct*)_tmp789)->f1;_LL4FE: for(0;
_tmp7D0 != 0;_tmp7D0=_tmp7D0->tl){cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,
cvtenv,(void*)6,(void*)_tmp7D0->hd,1);}goto _LL4CA;_LL4CA:;}if(!Cyc_Tcutil_kind_leq(
Cyc_Tcutil_typ_kind(t),expected_kind)){{void*_tmp947=t;struct Cyc_Core_Opt*
_tmp948;struct Cyc_Core_Opt*_tmp949;_LL5A9: if(_tmp947 <= (void*)4)goto _LL5AB;if(*((
int*)_tmp947)!= 0)goto _LL5AB;_tmp948=((struct Cyc_Absyn_Evar_struct*)_tmp947)->f1;
_tmp949=((struct Cyc_Absyn_Evar_struct*)_tmp947)->f2;_LL5AA: {struct
_dyneither_ptr s;{struct Cyc_Core_Opt*_tmp94A=_tmp948;struct Cyc_Core_Opt _tmp94B;
void*_tmp94C;_LL5AE: if(_tmp94A != 0)goto _LL5B0;_LL5AF: s=({const char*_tmp94D="kind=NULL ";
_tag_dyneither(_tmp94D,sizeof(char),11);});goto _LL5AD;_LL5B0: if(_tmp94A == 0)goto
_LL5AD;_tmp94B=*_tmp94A;_tmp94C=(void*)_tmp94B.v;_LL5B1: s=(struct _dyneither_ptr)({
struct Cyc_String_pa_struct _tmp950;_tmp950.tag=0;_tmp950.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_kind2string(_tmp94C));{void*_tmp94E[1]={&
_tmp950};Cyc_aprintf(({const char*_tmp94F="kind=%s ";_tag_dyneither(_tmp94F,
sizeof(char),9);}),_tag_dyneither(_tmp94E,sizeof(void*),1));}});goto _LL5AD;
_LL5AD:;}{struct Cyc_Core_Opt*_tmp951=_tmp949;struct Cyc_Core_Opt _tmp952;void*
_tmp953;_LL5B3: if(_tmp951 != 0)goto _LL5B5;_LL5B4: s=(struct _dyneither_ptr)({struct
Cyc_String_pa_struct _tmp956;_tmp956.tag=0;_tmp956.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)s);{void*_tmp954[1]={& _tmp956};Cyc_aprintf(({const char*
_tmp955="%s ref=NULL";_tag_dyneither(_tmp955,sizeof(char),12);}),_tag_dyneither(
_tmp954,sizeof(void*),1));}});goto _LL5B2;_LL5B5: if(_tmp951 == 0)goto _LL5B2;
_tmp952=*_tmp951;_tmp953=(void*)_tmp952.v;_LL5B6: s=(struct _dyneither_ptr)({
struct Cyc_String_pa_struct _tmp95A;_tmp95A.tag=0;_tmp95A.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp953));{struct Cyc_String_pa_struct
_tmp959;_tmp959.tag=0;_tmp959.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s);{
void*_tmp957[2]={& _tmp959,& _tmp95A};Cyc_aprintf(({const char*_tmp958="%s ref=%s";
_tag_dyneither(_tmp958,sizeof(char),10);}),_tag_dyneither(_tmp957,sizeof(void*),
2));}}});goto _LL5B2;_LL5B2:;}({struct Cyc_String_pa_struct _tmp95D;_tmp95D.tag=0;
_tmp95D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s);{void*_tmp95B[1]={&
_tmp95D};Cyc_fprintf(Cyc_stderr,({const char*_tmp95C="evar info: %s\n";
_tag_dyneither(_tmp95C,sizeof(char),15);}),_tag_dyneither(_tmp95B,sizeof(void*),
1));}});goto _LL5A8;}_LL5AB:;_LL5AC: goto _LL5A8;_LL5A8:;}({struct Cyc_String_pa_struct
_tmp962;_tmp962.tag=0;_tmp962.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kind2string(expected_kind));{struct Cyc_String_pa_struct _tmp961;
_tmp961.tag=0;_tmp961.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(
Cyc_Tcutil_typ_kind(t)));{struct Cyc_String_pa_struct _tmp960;_tmp960.tag=0;
_tmp960.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));{
void*_tmp95E[3]={& _tmp960,& _tmp961,& _tmp962};Cyc_Tcutil_terr(loc,({const char*
_tmp95F="type %s has kind %s but as used here needs kind %s";_tag_dyneither(
_tmp95F,sizeof(char),51);}),_tag_dyneither(_tmp95E,sizeof(void*),3));}}}});}
return cvtenv;}static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type_level_exp(
struct Cyc_Absyn_Exp*e,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv){{
void*_tmp963=e->r;struct Cyc_List_List*_tmp964;struct Cyc_Absyn_Exp*_tmp965;struct
Cyc_Absyn_Exp*_tmp966;struct Cyc_Absyn_Exp*_tmp967;struct Cyc_Absyn_Exp*_tmp968;
struct Cyc_Absyn_Exp*_tmp969;struct Cyc_Absyn_Exp*_tmp96A;struct Cyc_Absyn_Exp*
_tmp96B;struct Cyc_Absyn_Exp*_tmp96C;struct Cyc_Absyn_Exp*_tmp96D;void*_tmp96E;
struct Cyc_Absyn_Exp*_tmp96F;void*_tmp970;void*_tmp971;void*_tmp972;struct Cyc_Absyn_Exp*
_tmp973;_LL5B8: if(*((int*)_tmp963)!= 0)goto _LL5BA;_LL5B9: goto _LL5BB;_LL5BA: if(*((
int*)_tmp963)!= 33)goto _LL5BC;_LL5BB: goto _LL5BD;_LL5BC: if(*((int*)_tmp963)!= 34)
goto _LL5BE;_LL5BD: goto _LL5BF;_LL5BE: if(*((int*)_tmp963)!= 1)goto _LL5C0;_LL5BF:
goto _LL5B7;_LL5C0: if(*((int*)_tmp963)!= 3)goto _LL5C2;_tmp964=((struct Cyc_Absyn_Primop_e_struct*)
_tmp963)->f2;_LL5C1: for(0;_tmp964 != 0;_tmp964=_tmp964->tl){cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp((
struct Cyc_Absyn_Exp*)_tmp964->hd,te,cvtenv);}goto _LL5B7;_LL5C2: if(*((int*)
_tmp963)!= 6)goto _LL5C4;_tmp965=((struct Cyc_Absyn_Conditional_e_struct*)_tmp963)->f1;
_tmp966=((struct Cyc_Absyn_Conditional_e_struct*)_tmp963)->f2;_tmp967=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp963)->f3;_LL5C3: cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp965,te,
cvtenv);cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp966,te,cvtenv);cvtenv=
Cyc_Tcutil_i_check_valid_type_level_exp(_tmp967,te,cvtenv);goto _LL5B7;_LL5C4: if(*((
int*)_tmp963)!= 7)goto _LL5C6;_tmp968=((struct Cyc_Absyn_And_e_struct*)_tmp963)->f1;
_tmp969=((struct Cyc_Absyn_And_e_struct*)_tmp963)->f2;_LL5C5: _tmp96A=_tmp968;
_tmp96B=_tmp969;goto _LL5C7;_LL5C6: if(*((int*)_tmp963)!= 8)goto _LL5C8;_tmp96A=((
struct Cyc_Absyn_Or_e_struct*)_tmp963)->f1;_tmp96B=((struct Cyc_Absyn_Or_e_struct*)
_tmp963)->f2;_LL5C7: _tmp96C=_tmp96A;_tmp96D=_tmp96B;goto _LL5C9;_LL5C8: if(*((int*)
_tmp963)!= 9)goto _LL5CA;_tmp96C=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp963)->f1;
_tmp96D=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp963)->f2;_LL5C9: cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(
_tmp96C,te,cvtenv);cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp96D,te,
cvtenv);goto _LL5B7;_LL5CA: if(*((int*)_tmp963)!= 15)goto _LL5CC;_tmp96E=(void*)((
struct Cyc_Absyn_Cast_e_struct*)_tmp963)->f1;_tmp96F=((struct Cyc_Absyn_Cast_e_struct*)
_tmp963)->f2;_LL5CB: cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp96F,te,
cvtenv);cvtenv=Cyc_Tcutil_i_check_valid_type(e->loc,te,cvtenv,(void*)0,_tmp96E,1);
goto _LL5B7;_LL5CC: if(*((int*)_tmp963)!= 20)goto _LL5CE;_tmp970=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)
_tmp963)->f1;_LL5CD: _tmp971=_tmp970;goto _LL5CF;_LL5CE: if(*((int*)_tmp963)!= 18)
goto _LL5D0;_tmp971=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp963)->f1;
_LL5CF: cvtenv=Cyc_Tcutil_i_check_valid_type(e->loc,te,cvtenv,(void*)0,_tmp971,1);
goto _LL5B7;_LL5D0: if(*((int*)_tmp963)!= 40)goto _LL5D2;_tmp972=(void*)((struct Cyc_Absyn_Valueof_e_struct*)
_tmp963)->f1;_LL5D1: cvtenv=Cyc_Tcutil_i_check_valid_type(e->loc,te,cvtenv,(void*)
7,_tmp972,1);goto _LL5B7;_LL5D2: if(*((int*)_tmp963)!= 19)goto _LL5D4;_tmp973=((
struct Cyc_Absyn_Sizeofexp_e_struct*)_tmp963)->f1;_LL5D3: cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(
_tmp973,te,cvtenv);goto _LL5B7;_LL5D4:;_LL5D5:({void*_tmp974=0;((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp975="non-type-level-expression in Tcutil::i_check_valid_type_level_exp";
_tag_dyneither(_tmp975,sizeof(char),66);}),_tag_dyneither(_tmp974,sizeof(void*),
0));});_LL5B7:;}return cvtenv;}static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_check_valid_type(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv
cvt,void*expected_kind,void*t){struct Cyc_List_List*_tmp976=cvt.kind_env;cvt=Cyc_Tcutil_i_check_valid_type(
loc,te,cvt,expected_kind,t,1);{struct Cyc_List_List*vs=cvt.free_vars;for(0;vs != 0;
vs=vs->tl){struct Cyc_Absyn_Tvar*_tmp978;struct _tuple23 _tmp977=*((struct _tuple23*)
vs->hd);_tmp978=_tmp977.f1;cvt.kind_env=Cyc_Tcutil_fast_add_free_tvar(_tmp976,
_tmp978);}}{struct Cyc_List_List*evs=cvt.free_evars;for(0;evs != 0;evs=evs->tl){
void*_tmp97A;struct _tuple7 _tmp979=*((struct _tuple7*)evs->hd);_tmp97A=_tmp979.f1;{
void*_tmp97B=Cyc_Tcutil_compress(_tmp97A);struct Cyc_Core_Opt*_tmp97C;struct Cyc_Core_Opt**
_tmp97D;_LL5D7: if(_tmp97B <= (void*)4)goto _LL5D9;if(*((int*)_tmp97B)!= 0)goto
_LL5D9;_tmp97C=((struct Cyc_Absyn_Evar_struct*)_tmp97B)->f4;_tmp97D=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Evar_struct*)_tmp97B)->f4;_LL5D8: if(*_tmp97D == 0)*_tmp97D=({
struct Cyc_Core_Opt*_tmp97E=_cycalloc(sizeof(*_tmp97E));_tmp97E->v=_tmp976;
_tmp97E;});else{struct Cyc_List_List*_tmp97F=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(*_tmp97D))->v;struct Cyc_List_List*_tmp980=0;for(0;_tmp97F != 0;
_tmp97F=_tmp97F->tl){if(((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,
_tmp976,(struct Cyc_Absyn_Tvar*)_tmp97F->hd))_tmp980=({struct Cyc_List_List*
_tmp981=_cycalloc(sizeof(*_tmp981));_tmp981->hd=(struct Cyc_Absyn_Tvar*)_tmp97F->hd;
_tmp981->tl=_tmp980;_tmp981;});}*_tmp97D=({struct Cyc_Core_Opt*_tmp982=_cycalloc(
sizeof(*_tmp982));_tmp982->v=_tmp980;_tmp982;});}goto _LL5D6;_LL5D9:;_LL5DA: goto
_LL5D6;_LL5D6:;}}}return cvt;}void Cyc_Tcutil_check_valid_toplevel_type(struct Cyc_Position_Segment*
loc,struct Cyc_Tcenv_Tenv*te,void*t){int generalize_evars=Cyc_Tcutil_is_function_type(
t);struct Cyc_List_List*_tmp983=Cyc_Tcenv_lookup_type_vars(te);struct
_RegionHandle*_tmp984=Cyc_Tcenv_get_fnrgn(te);struct Cyc_Tcutil_CVTEnv _tmp985=Cyc_Tcutil_check_valid_type(
loc,te,({struct Cyc_Tcutil_CVTEnv _tmp9B6;_tmp9B6.r=_tmp984;_tmp9B6.kind_env=
_tmp983;_tmp9B6.free_vars=0;_tmp9B6.free_evars=0;_tmp9B6.generalize_evars=
generalize_evars;_tmp9B6.fn_result=0;_tmp9B6;}),(void*)1,t);struct Cyc_List_List*
_tmp986=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Tvar*(*f)(
struct _tuple23*),struct Cyc_List_List*x))Cyc_List_rmap)(_tmp984,(struct Cyc_Absyn_Tvar*(*)(
struct _tuple23*))Cyc_Core_fst,_tmp985.free_vars);struct Cyc_List_List*_tmp987=((
struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _tuple7*),struct Cyc_List_List*
x))Cyc_List_rmap)(_tmp984,(void*(*)(struct _tuple7*))Cyc_Core_fst,_tmp985.free_evars);
if(_tmp983 != 0){struct Cyc_List_List*_tmp988=0;{struct Cyc_List_List*_tmp989=
_tmp986;for(0;(unsigned int)_tmp989;_tmp989=_tmp989->tl){struct Cyc_Absyn_Tvar*
_tmp98A=(struct Cyc_Absyn_Tvar*)_tmp989->hd;int found=0;{struct Cyc_List_List*
_tmp98B=_tmp983;for(0;(unsigned int)_tmp98B;_tmp98B=_tmp98B->tl){if(Cyc_Absyn_tvar_cmp(
_tmp98A,(struct Cyc_Absyn_Tvar*)_tmp98B->hd)== 0){found=1;break;}}}if(!found)
_tmp988=({struct Cyc_List_List*_tmp98C=_region_malloc(_tmp984,sizeof(*_tmp98C));
_tmp98C->hd=(struct Cyc_Absyn_Tvar*)_tmp989->hd;_tmp98C->tl=_tmp988;_tmp98C;});}}
_tmp986=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmp988);}{struct Cyc_List_List*x=_tmp986;for(0;x != 0;x=x->tl){void*_tmp98D=Cyc_Absyn_compress_kb(((
struct Cyc_Absyn_Tvar*)x->hd)->kind);struct Cyc_Core_Opt*_tmp98E;struct Cyc_Core_Opt**
_tmp98F;struct Cyc_Core_Opt*_tmp990;struct Cyc_Core_Opt**_tmp991;void*_tmp992;
struct Cyc_Core_Opt*_tmp993;struct Cyc_Core_Opt**_tmp994;void*_tmp995;struct Cyc_Core_Opt*
_tmp996;struct Cyc_Core_Opt**_tmp997;void*_tmp998;void*_tmp999;_LL5DC: if(*((int*)
_tmp98D)!= 1)goto _LL5DE;_tmp98E=((struct Cyc_Absyn_Unknown_kb_struct*)_tmp98D)->f1;
_tmp98F=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)_tmp98D)->f1;
_LL5DD: _tmp991=_tmp98F;goto _LL5DF;_LL5DE: if(*((int*)_tmp98D)!= 2)goto _LL5E0;
_tmp990=((struct Cyc_Absyn_Less_kb_struct*)_tmp98D)->f1;_tmp991=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*)_tmp98D)->f1;_tmp992=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp98D)->f2;if((int)_tmp992 != 1)goto _LL5E0;_LL5DF:*_tmp991=Cyc_Tcutil_kind_to_bound_opt((
void*)2);goto _LL5DB;_LL5E0: if(*((int*)_tmp98D)!= 2)goto _LL5E2;_tmp993=((struct
Cyc_Absyn_Less_kb_struct*)_tmp98D)->f1;_tmp994=(struct Cyc_Core_Opt**)&((struct
Cyc_Absyn_Less_kb_struct*)_tmp98D)->f1;_tmp995=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp98D)->f2;if((int)_tmp995 != 5)goto _LL5E2;_LL5E1:*_tmp994=Cyc_Tcutil_kind_to_bound_opt((
void*)3);goto _LL5DB;_LL5E2: if(*((int*)_tmp98D)!= 2)goto _LL5E4;_tmp996=((struct
Cyc_Absyn_Less_kb_struct*)_tmp98D)->f1;_tmp997=(struct Cyc_Core_Opt**)&((struct
Cyc_Absyn_Less_kb_struct*)_tmp98D)->f1;_tmp998=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp98D)->f2;_LL5E3:*_tmp997=Cyc_Tcutil_kind_to_bound_opt(_tmp998);goto _LL5DB;
_LL5E4: if(*((int*)_tmp98D)!= 0)goto _LL5E6;_tmp999=(void*)((struct Cyc_Absyn_Eq_kb_struct*)
_tmp98D)->f1;if((int)_tmp999 != 1)goto _LL5E6;_LL5E5:({struct Cyc_String_pa_struct
_tmp99C;_tmp99C.tag=0;_tmp99C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Tcutil_tvar2string((struct Cyc_Absyn_Tvar*)x->hd));{void*_tmp99A[1]={& _tmp99C};
Cyc_Tcutil_terr(loc,({const char*_tmp99B="type variable %s cannot have kind M";
_tag_dyneither(_tmp99B,sizeof(char),36);}),_tag_dyneither(_tmp99A,sizeof(void*),
1));}});goto _LL5DB;_LL5E6:;_LL5E7: goto _LL5DB;_LL5DB:;}}if(_tmp986 != 0  || _tmp987
!= 0){{void*_tmp99D=Cyc_Tcutil_compress(t);struct Cyc_Absyn_FnInfo _tmp99E;struct
Cyc_List_List*_tmp99F;struct Cyc_List_List**_tmp9A0;struct Cyc_Core_Opt*_tmp9A1;
void*_tmp9A2;struct Cyc_List_List*_tmp9A3;int _tmp9A4;struct Cyc_Absyn_VarargInfo*
_tmp9A5;struct Cyc_List_List*_tmp9A6;struct Cyc_List_List*_tmp9A7;_LL5E9: if(
_tmp99D <= (void*)4)goto _LL5EB;if(*((int*)_tmp99D)!= 8)goto _LL5EB;_tmp99E=((
struct Cyc_Absyn_FnType_struct*)_tmp99D)->f1;_tmp99F=_tmp99E.tvars;_tmp9A0=(
struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_struct*)_tmp99D)->f1).tvars;
_tmp9A1=_tmp99E.effect;_tmp9A2=_tmp99E.ret_typ;_tmp9A3=_tmp99E.args;_tmp9A4=
_tmp99E.c_varargs;_tmp9A5=_tmp99E.cyc_varargs;_tmp9A6=_tmp99E.rgn_po;_tmp9A7=
_tmp99E.attributes;_LL5EA: if(*_tmp9A0 == 0){*_tmp9A0=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_copy)(_tmp986);_tmp986=0;}goto _LL5E8;_LL5EB:;
_LL5EC: goto _LL5E8;_LL5E8:;}if(_tmp986 != 0)({struct Cyc_String_pa_struct _tmp9AA;
_tmp9AA.tag=0;_tmp9AA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct
Cyc_Absyn_Tvar*)_tmp986->hd)->name);{void*_tmp9A8[1]={& _tmp9AA};Cyc_Tcutil_terr(
loc,({const char*_tmp9A9="unbound type variable %s";_tag_dyneither(_tmp9A9,
sizeof(char),25);}),_tag_dyneither(_tmp9A8,sizeof(void*),1));}});if(_tmp987 != 0)
for(0;_tmp987 != 0;_tmp987=_tmp987->tl){void*e=(void*)_tmp987->hd;void*_tmp9AB=
Cyc_Tcutil_typ_kind(e);_LL5EE: if((int)_tmp9AB != 4)goto _LL5F0;_LL5EF: if(!Cyc_Tcutil_unify(
e,(void*)3))({void*_tmp9AC=0;((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp9AD="can't unify evar with unique region!";
_tag_dyneither(_tmp9AD,sizeof(char),37);}),_tag_dyneither(_tmp9AC,sizeof(void*),
0));});goto _LL5ED;_LL5F0: if((int)_tmp9AB != 5)goto _LL5F2;_LL5F1: goto _LL5F3;_LL5F2:
if((int)_tmp9AB != 3)goto _LL5F4;_LL5F3: if(!Cyc_Tcutil_unify(e,(void*)2))({void*
_tmp9AE=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp9AF="can't unify evar with heap!";_tag_dyneither(_tmp9AF,sizeof(
char),28);}),_tag_dyneither(_tmp9AE,sizeof(void*),0));});goto _LL5ED;_LL5F4: if((
int)_tmp9AB != 6)goto _LL5F6;_LL5F5: if(!Cyc_Tcutil_unify(e,Cyc_Absyn_empty_effect))({
void*_tmp9B0=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp9B1="can't unify evar with {}!";_tag_dyneither(_tmp9B1,sizeof(char),
26);}),_tag_dyneither(_tmp9B0,sizeof(void*),0));});goto _LL5ED;_LL5F6:;_LL5F7:({
struct Cyc_String_pa_struct _tmp9B5;_tmp9B5.tag=0;_tmp9B5.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));{struct Cyc_String_pa_struct
_tmp9B4;_tmp9B4.tag=0;_tmp9B4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(e));{void*_tmp9B2[2]={& _tmp9B4,& _tmp9B5};Cyc_Tcutil_terr(
loc,({const char*_tmp9B3="hidden type variable %s isn't abstracted in type %s";
_tag_dyneither(_tmp9B3,sizeof(char),52);}),_tag_dyneither(_tmp9B2,sizeof(void*),
2));}}});goto _LL5ED;_LL5ED:;}}}void Cyc_Tcutil_check_fndecl_valid_type(struct Cyc_Position_Segment*
loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Fndecl*fd){void*t=Cyc_Tcutil_fndecl2typ(
fd);Cyc_Tcutil_check_valid_toplevel_type(loc,te,t);{void*_tmp9B7=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_FnInfo _tmp9B8;struct Cyc_List_List*_tmp9B9;struct Cyc_Core_Opt*
_tmp9BA;void*_tmp9BB;_LL5F9: if(_tmp9B7 <= (void*)4)goto _LL5FB;if(*((int*)_tmp9B7)
!= 8)goto _LL5FB;_tmp9B8=((struct Cyc_Absyn_FnType_struct*)_tmp9B7)->f1;_tmp9B9=
_tmp9B8.tvars;_tmp9BA=_tmp9B8.effect;_tmp9BB=_tmp9B8.ret_typ;_LL5FA: fd->tvs=
_tmp9B9;fd->effect=_tmp9BA;goto _LL5F8;_LL5FB:;_LL5FC:({void*_tmp9BC=0;((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmp9BD="check_fndecl_valid_type: not a FnType";_tag_dyneither(_tmp9BD,sizeof(
char),38);}),_tag_dyneither(_tmp9BC,sizeof(void*),0));});_LL5F8:;}{struct
_RegionHandle*_tmp9BE=Cyc_Tcenv_get_fnrgn(te);Cyc_Tcutil_check_unique_vars(((
struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr*(*f)(struct
_tuple19*),struct Cyc_List_List*x))Cyc_List_rmap)(_tmp9BE,(struct _dyneither_ptr*(*)(
struct _tuple19*t))Cyc_Tcutil_fst_fdarg,fd->args),loc,({const char*_tmp9BF="function declaration has repeated parameter";
_tag_dyneither(_tmp9BF,sizeof(char),44);}));fd->cached_typ=({struct Cyc_Core_Opt*
_tmp9C0=_cycalloc(sizeof(*_tmp9C0));_tmp9C0->v=(void*)t;_tmp9C0;});}}void Cyc_Tcutil_check_type(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*
bound_tvars,void*expected_kind,int allow_evars,void*t){struct _RegionHandle*
_tmp9C1=Cyc_Tcenv_get_fnrgn(te);struct Cyc_Tcutil_CVTEnv _tmp9C2=Cyc_Tcutil_check_valid_type(
loc,te,({struct Cyc_Tcutil_CVTEnv _tmp9D5;_tmp9D5.r=_tmp9C1;_tmp9D5.kind_env=
bound_tvars;_tmp9D5.free_vars=0;_tmp9D5.free_evars=0;_tmp9D5.generalize_evars=0;
_tmp9D5.fn_result=0;_tmp9D5;}),expected_kind,t);struct Cyc_List_List*_tmp9C3=Cyc_Tcutil_remove_bound_tvars(
_tmp9C1,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Tvar*(*f)(
struct _tuple23*),struct Cyc_List_List*x))Cyc_List_rmap)(_tmp9C1,(struct Cyc_Absyn_Tvar*(*)(
struct _tuple23*))Cyc_Core_fst,_tmp9C2.free_vars),bound_tvars);struct Cyc_List_List*
_tmp9C4=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _tuple7*),
struct Cyc_List_List*x))Cyc_List_rmap)(_tmp9C1,(void*(*)(struct _tuple7*))Cyc_Core_fst,
_tmp9C2.free_evars);{struct Cyc_List_List*fs=_tmp9C3;for(0;fs != 0;fs=fs->tl){
struct _dyneither_ptr*_tmp9C5=((struct Cyc_Absyn_Tvar*)fs->hd)->name;({struct Cyc_String_pa_struct
_tmp9C9;_tmp9C9.tag=0;_tmp9C9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t));{struct Cyc_String_pa_struct _tmp9C8;_tmp9C8.tag=0;
_tmp9C8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp9C5);{void*_tmp9C6[
2]={& _tmp9C8,& _tmp9C9};Cyc_Tcutil_terr(loc,({const char*_tmp9C7="unbound type variable %s in type %s";
_tag_dyneither(_tmp9C7,sizeof(char),36);}),_tag_dyneither(_tmp9C6,sizeof(void*),
2));}}});}}if(!allow_evars  && _tmp9C4 != 0)for(0;_tmp9C4 != 0;_tmp9C4=_tmp9C4->tl){
void*e=(void*)_tmp9C4->hd;void*_tmp9CA=Cyc_Tcutil_typ_kind(e);_LL5FE: if((int)
_tmp9CA != 4)goto _LL600;_LL5FF: if(!Cyc_Tcutil_unify(e,(void*)3))({void*_tmp9CB=0;((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const
char*_tmp9CC="can't unify evar with unique region!";_tag_dyneither(_tmp9CC,
sizeof(char),37);}),_tag_dyneither(_tmp9CB,sizeof(void*),0));});goto _LL5FD;
_LL600: if((int)_tmp9CA != 5)goto _LL602;_LL601: goto _LL603;_LL602: if((int)_tmp9CA != 
3)goto _LL604;_LL603: if(!Cyc_Tcutil_unify(e,(void*)2))({void*_tmp9CD=0;((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmp9CE="can't unify evar with heap!";_tag_dyneither(_tmp9CE,sizeof(char),28);}),
_tag_dyneither(_tmp9CD,sizeof(void*),0));});goto _LL5FD;_LL604: if((int)_tmp9CA != 
6)goto _LL606;_LL605: if(!Cyc_Tcutil_unify(e,Cyc_Absyn_empty_effect))({void*
_tmp9CF=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp9D0="can't unify evar with {}!";_tag_dyneither(_tmp9D0,sizeof(char),
26);}),_tag_dyneither(_tmp9CF,sizeof(void*),0));});goto _LL5FD;_LL606:;_LL607:({
struct Cyc_String_pa_struct _tmp9D4;_tmp9D4.tag=0;_tmp9D4.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));{struct Cyc_String_pa_struct
_tmp9D3;_tmp9D3.tag=0;_tmp9D3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(e));{void*_tmp9D1[2]={& _tmp9D3,& _tmp9D4};Cyc_Tcutil_terr(
loc,({const char*_tmp9D2="hidden type variable %s isn't abstracted in type %s";
_tag_dyneither(_tmp9D2,sizeof(char),52);}),_tag_dyneither(_tmp9D1,sizeof(void*),
2));}}});goto _LL5FD;_LL5FD:;}}void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*
tv){if(tv->identity == - 1)tv->identity=Cyc_Tcutil_new_tvar_id();}void Cyc_Tcutil_add_tvar_identities(
struct Cyc_List_List*tvs){((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*
x))Cyc_List_iter)(Cyc_Tcutil_add_tvar_identity,tvs);}static void Cyc_Tcutil_check_unique_unsorted(
int(*cmp)(void*,void*),struct Cyc_List_List*vs,struct Cyc_Position_Segment*loc,
struct _dyneither_ptr(*a2string)(void*),struct _dyneither_ptr msg){for(0;vs != 0;vs=
vs->tl){struct Cyc_List_List*vs2=vs->tl;for(0;vs2 != 0;vs2=vs2->tl){if(cmp((void*)
vs->hd,(void*)vs2->hd)== 0)({struct Cyc_String_pa_struct _tmp9D9;_tmp9D9.tag=0;
_tmp9D9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)a2string((void*)vs->hd));{
struct Cyc_String_pa_struct _tmp9D8;_tmp9D8.tag=0;_tmp9D8.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)msg);{void*_tmp9D6[2]={& _tmp9D8,& _tmp9D9};Cyc_Tcutil_terr(
loc,({const char*_tmp9D7="%s: %s";_tag_dyneither(_tmp9D7,sizeof(char),7);}),
_tag_dyneither(_tmp9D6,sizeof(void*),2));}}});}}}static struct _dyneither_ptr Cyc_Tcutil_strptr2string(
struct _dyneither_ptr*s){return*s;}void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*
vs,struct Cyc_Position_Segment*loc,struct _dyneither_ptr msg){((void(*)(int(*cmp)(
struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*vs,struct Cyc_Position_Segment*
loc,struct _dyneither_ptr(*a2string)(struct _dyneither_ptr*),struct _dyneither_ptr
msg))Cyc_Tcutil_check_unique_unsorted)(Cyc_strptrcmp,vs,loc,Cyc_Tcutil_strptr2string,
msg);}void Cyc_Tcutil_check_unique_tvars(struct Cyc_Position_Segment*loc,struct Cyc_List_List*
tvs){((void(*)(int(*cmp)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*
vs,struct Cyc_Position_Segment*loc,struct _dyneither_ptr(*a2string)(struct Cyc_Absyn_Tvar*),
struct _dyneither_ptr msg))Cyc_Tcutil_check_unique_unsorted)(Cyc_Absyn_tvar_cmp,
tvs,loc,Cyc_Tcutil_tvar2string,({const char*_tmp9DA="duplicate type variable";
_tag_dyneither(_tmp9DA,sizeof(char),24);}));}struct _tuple24{struct Cyc_Absyn_Aggrfield*
f1;int f2;};struct _tuple25{struct Cyc_List_List*f1;void*f2;};struct _tuple26{struct
Cyc_Absyn_Aggrfield*f1;void*f2;};struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(
struct _RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,
void*aggr_kind,struct Cyc_List_List*sdfields){struct Cyc_List_List*fields=0;{
struct Cyc_List_List*sd_fields=sdfields;for(0;sd_fields != 0;sd_fields=sd_fields->tl){
if(Cyc_strcmp((struct _dyneither_ptr)*((struct Cyc_Absyn_Aggrfield*)sd_fields->hd)->name,({
const char*_tmp9DB="";_tag_dyneither(_tmp9DB,sizeof(char),1);}))!= 0)fields=({
struct Cyc_List_List*_tmp9DC=_cycalloc(sizeof(*_tmp9DC));_tmp9DC->hd=({struct
_tuple24*_tmp9DD=_cycalloc(sizeof(*_tmp9DD));_tmp9DD->f1=(struct Cyc_Absyn_Aggrfield*)
sd_fields->hd;_tmp9DD->f2=0;_tmp9DD;});_tmp9DC->tl=fields;_tmp9DC;});}}fields=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields);{struct
_dyneither_ptr aggr_str=aggr_kind == (void*)0?({const char*_tmpA01="struct";
_tag_dyneither(_tmpA01,sizeof(char),7);}):({const char*_tmpA02="union";
_tag_dyneither(_tmpA02,sizeof(char),6);});struct Cyc_List_List*ans=0;for(0;des != 
0;des=des->tl){struct _tuple25 _tmp9DF;struct Cyc_List_List*_tmp9E0;void*_tmp9E1;
struct _tuple25*_tmp9DE=(struct _tuple25*)des->hd;_tmp9DF=*_tmp9DE;_tmp9E0=_tmp9DF.f1;
_tmp9E1=_tmp9DF.f2;if(_tmp9E0 == 0){struct Cyc_List_List*_tmp9E2=fields;for(0;
_tmp9E2 != 0;_tmp9E2=_tmp9E2->tl){if(!(*((struct _tuple24*)_tmp9E2->hd)).f2){(*((
struct _tuple24*)_tmp9E2->hd)).f2=1;(*((struct _tuple25*)des->hd)).f1=(struct Cyc_List_List*)({
struct Cyc_List_List*_tmp9E3=_cycalloc(sizeof(*_tmp9E3));_tmp9E3->hd=(void*)((
void*)({struct Cyc_Absyn_FieldName_struct*_tmp9E4=_cycalloc(sizeof(*_tmp9E4));
_tmp9E4[0]=({struct Cyc_Absyn_FieldName_struct _tmp9E5;_tmp9E5.tag=1;_tmp9E5.f1=((*((
struct _tuple24*)_tmp9E2->hd)).f1)->name;_tmp9E5;});_tmp9E4;}));_tmp9E3->tl=0;
_tmp9E3;});ans=({struct Cyc_List_List*_tmp9E6=_region_malloc(rgn,sizeof(*_tmp9E6));
_tmp9E6->hd=({struct _tuple26*_tmp9E7=_region_malloc(rgn,sizeof(*_tmp9E7));
_tmp9E7->f1=(*((struct _tuple24*)_tmp9E2->hd)).f1;_tmp9E7->f2=_tmp9E1;_tmp9E7;});
_tmp9E6->tl=ans;_tmp9E6;});break;}}if(_tmp9E2 == 0)({struct Cyc_String_pa_struct
_tmp9EA;_tmp9EA.tag=0;_tmp9EA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
aggr_str);{void*_tmp9E8[1]={& _tmp9EA};Cyc_Tcutil_terr(loc,({const char*_tmp9E9="too many arguments to %s";
_tag_dyneither(_tmp9E9,sizeof(char),25);}),_tag_dyneither(_tmp9E8,sizeof(void*),
1));}});}else{if(_tmp9E0->tl != 0)({void*_tmp9EB=0;Cyc_Tcutil_terr(loc,({const
char*_tmp9EC="multiple designators are not yet supported";_tag_dyneither(_tmp9EC,
sizeof(char),43);}),_tag_dyneither(_tmp9EB,sizeof(void*),0));});else{void*
_tmp9ED=(void*)_tmp9E0->hd;struct _dyneither_ptr*_tmp9EE;_LL609: if(*((int*)
_tmp9ED)!= 0)goto _LL60B;_LL60A:({struct Cyc_String_pa_struct _tmp9F1;_tmp9F1.tag=0;
_tmp9F1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)aggr_str);{void*_tmp9EF[
1]={& _tmp9F1};Cyc_Tcutil_terr(loc,({const char*_tmp9F0="array designator used in argument to %s";
_tag_dyneither(_tmp9F0,sizeof(char),40);}),_tag_dyneither(_tmp9EF,sizeof(void*),
1));}});goto _LL608;_LL60B: if(*((int*)_tmp9ED)!= 1)goto _LL608;_tmp9EE=((struct Cyc_Absyn_FieldName_struct*)
_tmp9ED)->f1;_LL60C: {struct Cyc_List_List*_tmp9F2=fields;for(0;_tmp9F2 != 0;
_tmp9F2=_tmp9F2->tl){if(Cyc_strptrcmp(_tmp9EE,((*((struct _tuple24*)_tmp9F2->hd)).f1)->name)
== 0){if((*((struct _tuple24*)_tmp9F2->hd)).f2)({struct Cyc_String_pa_struct
_tmp9F5;_tmp9F5.tag=0;_tmp9F5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*
_tmp9EE);{void*_tmp9F3[1]={& _tmp9F5};Cyc_Tcutil_terr(loc,({const char*_tmp9F4="member %s has already been used as an argument";
_tag_dyneither(_tmp9F4,sizeof(char),47);}),_tag_dyneither(_tmp9F3,sizeof(void*),
1));}});(*((struct _tuple24*)_tmp9F2->hd)).f2=1;ans=({struct Cyc_List_List*_tmp9F6=
_region_malloc(rgn,sizeof(*_tmp9F6));_tmp9F6->hd=({struct _tuple26*_tmp9F7=
_region_malloc(rgn,sizeof(*_tmp9F7));_tmp9F7->f1=(*((struct _tuple24*)_tmp9F2->hd)).f1;
_tmp9F7->f2=_tmp9E1;_tmp9F7;});_tmp9F6->tl=ans;_tmp9F6;});break;}}if(_tmp9F2 == 0)({
struct Cyc_String_pa_struct _tmp9FA;_tmp9FA.tag=0;_tmp9FA.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)*_tmp9EE);{void*_tmp9F8[1]={& _tmp9FA};Cyc_Tcutil_terr(loc,({
const char*_tmp9F9="bad field designator %s";_tag_dyneither(_tmp9F9,sizeof(char),
24);}),_tag_dyneither(_tmp9F8,sizeof(void*),1));}});goto _LL608;}_LL608:;}}}if(
aggr_kind == (void*)0)for(0;fields != 0;fields=fields->tl){if(!(*((struct _tuple24*)
fields->hd)).f2){({void*_tmp9FB=0;Cyc_Tcutil_terr(loc,({const char*_tmp9FC="too few arguments to struct";
_tag_dyneither(_tmp9FC,sizeof(char),28);}),_tag_dyneither(_tmp9FB,sizeof(void*),
0));});break;}}else{int found=0;for(0;fields != 0;fields=fields->tl){if((*((struct
_tuple24*)fields->hd)).f2){if(found)({void*_tmp9FD=0;Cyc_Tcutil_terr(loc,({const
char*_tmp9FE="only one member of a union is allowed";_tag_dyneither(_tmp9FE,
sizeof(char),38);}),_tag_dyneither(_tmp9FD,sizeof(void*),0));});found=1;}}if(!
found)({void*_tmp9FF=0;Cyc_Tcutil_terr(loc,({const char*_tmpA00="missing member for union";
_tag_dyneither(_tmpA00,sizeof(char),25);}),_tag_dyneither(_tmp9FF,sizeof(void*),
0));});}return((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
ans);}}int Cyc_Tcutil_is_tagged_pointer_typ_elt(void*t,void**elt_typ_dest){void*
_tmpA03=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmpA04;void*_tmpA05;
struct Cyc_Absyn_PtrAtts _tmpA06;union Cyc_Absyn_Constraint*_tmpA07;_LL60E: if(
_tmpA03 <= (void*)4)goto _LL610;if(*((int*)_tmpA03)!= 4)goto _LL610;_tmpA04=((
struct Cyc_Absyn_PointerType_struct*)_tmpA03)->f1;_tmpA05=_tmpA04.elt_typ;_tmpA06=
_tmpA04.ptr_atts;_tmpA07=_tmpA06.bounds;_LL60F: {void*_tmpA08=Cyc_Absyn_conref_constr((
void*)((void*)0),_tmpA07);_LL613: if((int)_tmpA08 != 0)goto _LL615;_LL614:*
elt_typ_dest=_tmpA05;return 1;_LL615:;_LL616: return 0;_LL612:;}_LL610:;_LL611:
return 0;_LL60D:;}int Cyc_Tcutil_is_zero_pointer_typ_elt(void*t,void**elt_typ_dest){
void*_tmpA09=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmpA0A;void*_tmpA0B;
struct Cyc_Absyn_PtrAtts _tmpA0C;union Cyc_Absyn_Constraint*_tmpA0D;_LL618: if(
_tmpA09 <= (void*)4)goto _LL61A;if(*((int*)_tmpA09)!= 4)goto _LL61A;_tmpA0A=((
struct Cyc_Absyn_PointerType_struct*)_tmpA09)->f1;_tmpA0B=_tmpA0A.elt_typ;_tmpA0C=
_tmpA0A.ptr_atts;_tmpA0D=_tmpA0C.zero_term;_LL619:*elt_typ_dest=_tmpA0B;return((
int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpA0D);_LL61A:;
_LL61B: return 0;_LL617:;}int Cyc_Tcutil_is_zero_ptr_type(void*t,void**ptr_type,int*
is_dyneither,void**elt_type){void*_tmpA0E=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo
_tmpA0F;void*_tmpA10;struct Cyc_Absyn_PtrAtts _tmpA11;union Cyc_Absyn_Constraint*
_tmpA12;union Cyc_Absyn_Constraint*_tmpA13;struct Cyc_Absyn_ArrayInfo _tmpA14;void*
_tmpA15;struct Cyc_Absyn_Tqual _tmpA16;struct Cyc_Absyn_Exp*_tmpA17;union Cyc_Absyn_Constraint*
_tmpA18;_LL61D: if(_tmpA0E <= (void*)4)goto _LL621;if(*((int*)_tmpA0E)!= 4)goto
_LL61F;_tmpA0F=((struct Cyc_Absyn_PointerType_struct*)_tmpA0E)->f1;_tmpA10=
_tmpA0F.elt_typ;_tmpA11=_tmpA0F.ptr_atts;_tmpA12=_tmpA11.bounds;_tmpA13=_tmpA11.zero_term;
_LL61E: if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmpA13)){*ptr_type=t;*elt_type=_tmpA10;{void*_tmpA19=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,
_tmpA12);_LL624: if((int)_tmpA19 != 0)goto _LL626;_LL625:*is_dyneither=1;goto _LL623;
_LL626:;_LL627:*is_dyneither=0;goto _LL623;_LL623:;}return 1;}else{return 0;}_LL61F:
if(*((int*)_tmpA0E)!= 7)goto _LL621;_tmpA14=((struct Cyc_Absyn_ArrayType_struct*)
_tmpA0E)->f1;_tmpA15=_tmpA14.elt_type;_tmpA16=_tmpA14.tq;_tmpA17=_tmpA14.num_elts;
_tmpA18=_tmpA14.zero_term;_LL620: if(((int(*)(int y,union Cyc_Absyn_Constraint*x))
Cyc_Absyn_conref_def)(0,_tmpA18)){*elt_type=_tmpA15;*is_dyneither=0;*ptr_type=(
void*)({struct Cyc_Absyn_PointerType_struct*_tmpA1A=_cycalloc(sizeof(*_tmpA1A));
_tmpA1A[0]=({struct Cyc_Absyn_PointerType_struct _tmpA1B;_tmpA1B.tag=4;_tmpA1B.f1=({
struct Cyc_Absyn_PtrInfo _tmpA1C;_tmpA1C.elt_typ=_tmpA15;_tmpA1C.elt_tq=_tmpA16;
_tmpA1C.ptr_atts=({struct Cyc_Absyn_PtrAtts _tmpA1D;_tmpA1D.rgn=(void*)2;_tmpA1D.nullable=
Cyc_Absyn_false_conref;_tmpA1D.bounds=Cyc_Absyn_new_conref((void*)({struct Cyc_Absyn_Upper_b_struct*
_tmpA1E=_cycalloc(sizeof(*_tmpA1E));_tmpA1E[0]=({struct Cyc_Absyn_Upper_b_struct
_tmpA1F;_tmpA1F.tag=0;_tmpA1F.f1=(struct Cyc_Absyn_Exp*)_check_null(_tmpA17);
_tmpA1F;});_tmpA1E;}));_tmpA1D.zero_term=_tmpA18;_tmpA1D.ptrloc=0;_tmpA1D;});
_tmpA1C;});_tmpA1B;});_tmpA1A;});return 1;}else{return 0;}_LL621:;_LL622: return 0;
_LL61C:;}int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*e1,void**ptr_type,
int*is_dyneither,void**elt_type){void*_tmpA20=e1->r;struct Cyc_Absyn_Exp*_tmpA21;
struct Cyc_Absyn_Exp*_tmpA22;struct Cyc_Absyn_Exp*_tmpA23;struct Cyc_Absyn_Exp*
_tmpA24;struct Cyc_Absyn_Exp*_tmpA25;struct Cyc_Absyn_Exp*_tmpA26;_LL629: if(*((int*)
_tmpA20)!= 15)goto _LL62B;_LL62A:({struct Cyc_String_pa_struct _tmpA29;_tmpA29.tag=
0;_tmpA29.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(
e1));{void*_tmpA27[1]={& _tmpA29};((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmpA28="we have a cast in a lhs:  %s";
_tag_dyneither(_tmpA28,sizeof(char),29);}),_tag_dyneither(_tmpA27,sizeof(void*),
1));}});_LL62B: if(*((int*)_tmpA20)!= 22)goto _LL62D;_tmpA21=((struct Cyc_Absyn_Deref_e_struct*)
_tmpA20)->f1;_LL62C: _tmpA22=_tmpA21;goto _LL62E;_LL62D: if(*((int*)_tmpA20)!= 25)
goto _LL62F;_tmpA22=((struct Cyc_Absyn_Subscript_e_struct*)_tmpA20)->f1;_LL62E:
return Cyc_Tcutil_is_zero_ptr_type((void*)((struct Cyc_Core_Opt*)_check_null(
_tmpA22->topt))->v,ptr_type,is_dyneither,elt_type);_LL62F: if(*((int*)_tmpA20)!= 
24)goto _LL631;_tmpA23=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmpA20)->f1;_LL630:
_tmpA24=_tmpA23;goto _LL632;_LL631: if(*((int*)_tmpA20)!= 23)goto _LL633;_tmpA24=((
struct Cyc_Absyn_AggrMember_e_struct*)_tmpA20)->f1;_LL632: if(Cyc_Tcutil_is_zero_ptr_type((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpA24->topt))->v,ptr_type,is_dyneither,
elt_type))({struct Cyc_String_pa_struct _tmpA2C;_tmpA2C.tag=0;_tmpA2C.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e1));{void*_tmpA2A[
1]={& _tmpA2C};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmpA2B="found zero pointer aggregate member assignment: %s";
_tag_dyneither(_tmpA2B,sizeof(char),51);}),_tag_dyneither(_tmpA2A,sizeof(void*),
1));}});return 0;_LL633: if(*((int*)_tmpA20)!= 14)goto _LL635;_tmpA25=((struct Cyc_Absyn_Instantiate_e_struct*)
_tmpA20)->f1;_LL634: _tmpA26=_tmpA25;goto _LL636;_LL635: if(*((int*)_tmpA20)!= 13)
goto _LL637;_tmpA26=((struct Cyc_Absyn_NoInstantiate_e_struct*)_tmpA20)->f1;_LL636:
if(Cyc_Tcutil_is_zero_ptr_type((void*)((struct Cyc_Core_Opt*)_check_null(_tmpA26->topt))->v,
ptr_type,is_dyneither,elt_type))({struct Cyc_String_pa_struct _tmpA2F;_tmpA2F.tag=
0;_tmpA2F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(
e1));{void*_tmpA2D[1]={& _tmpA2F};((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmpA2E="found zero pointer instantiate/noinstantiate: %s";
_tag_dyneither(_tmpA2E,sizeof(char),49);}),_tag_dyneither(_tmpA2D,sizeof(void*),
1));}});return 0;_LL637: if(*((int*)_tmpA20)!= 1)goto _LL639;_LL638: return 0;_LL639:;
_LL63A:({struct Cyc_String_pa_struct _tmpA32;_tmpA32.tag=0;_tmpA32.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e1));{void*_tmpA30[
1]={& _tmpA32};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmpA31="found bad lhs in is_zero_ptr_deref: %s";_tag_dyneither(
_tmpA31,sizeof(char),39);}),_tag_dyneither(_tmpA30,sizeof(void*),1));}});_LL628:;}
int Cyc_Tcutil_is_tagged_pointer_typ(void*t){void*ignore=(void*)0;return Cyc_Tcutil_is_tagged_pointer_typ_elt(
t,& ignore);}static int Cyc_Tcutil_is_noalias_region(void*r){void*_tmpA33=Cyc_Tcutil_compress(
r);struct Cyc_Absyn_Tvar*_tmpA34;_LL63C: if((int)_tmpA33 != 3)goto _LL63E;_LL63D:
return 1;_LL63E: if(_tmpA33 <= (void*)4)goto _LL640;if(*((int*)_tmpA33)!= 1)goto
_LL640;_tmpA34=((struct Cyc_Absyn_VarType_struct*)_tmpA33)->f1;_LL63F: return Cyc_Tcutil_tvar_kind(
_tmpA34)== (void*)4  || Cyc_Tcutil_tvar_kind(_tmpA34)== (void*)5;_LL640:;_LL641:
return 0;_LL63B:;}int Cyc_Tcutil_is_noalias_pointer(void*t){void*_tmpA35=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_PtrInfo _tmpA36;struct Cyc_Absyn_PtrAtts _tmpA37;void*_tmpA38;
_LL643: if(_tmpA35 <= (void*)4)goto _LL645;if(*((int*)_tmpA35)!= 4)goto _LL645;
_tmpA36=((struct Cyc_Absyn_PointerType_struct*)_tmpA35)->f1;_tmpA37=_tmpA36.ptr_atts;
_tmpA38=_tmpA37.rgn;_LL644: return Cyc_Tcutil_is_noalias_region(_tmpA38);_LL645:;
_LL646: return 0;_LL642:;}int Cyc_Tcutil_is_noalias_pointer_or_aggr(struct
_RegionHandle*rgn,void*t){void*_tmpA39=Cyc_Tcutil_compress(t);if(Cyc_Tcutil_is_noalias_pointer(
_tmpA39))return 1;{void*_tmpA3A=_tmpA39;struct Cyc_List_List*_tmpA3B;struct Cyc_Absyn_AggrInfo
_tmpA3C;union Cyc_Absyn_AggrInfoU _tmpA3D;struct Cyc_Absyn_Aggrdecl**_tmpA3E;struct
Cyc_List_List*_tmpA3F;struct Cyc_List_List*_tmpA40;struct Cyc_Absyn_AggrInfo
_tmpA41;union Cyc_Absyn_AggrInfoU _tmpA42;struct _tuple4 _tmpA43;struct Cyc_Absyn_DatatypeInfo
_tmpA44;union Cyc_Absyn_DatatypeInfoU _tmpA45;struct Cyc_List_List*_tmpA46;struct
Cyc_Core_Opt*_tmpA47;struct Cyc_Absyn_DatatypeFieldInfo _tmpA48;union Cyc_Absyn_DatatypeFieldInfoU
_tmpA49;struct Cyc_List_List*_tmpA4A;_LL648: if(_tmpA3A <= (void*)4)goto _LL654;if(*((
int*)_tmpA3A)!= 9)goto _LL64A;_tmpA3B=((struct Cyc_Absyn_TupleType_struct*)_tmpA3A)->f1;
_LL649: while(_tmpA3B != 0){if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,(*((
struct _tuple11*)_tmpA3B->hd)).f2))return 1;_tmpA3B=_tmpA3B->tl;}return 0;_LL64A:
if(*((int*)_tmpA3A)!= 10)goto _LL64C;_tmpA3C=((struct Cyc_Absyn_AggrType_struct*)
_tmpA3A)->f1;_tmpA3D=_tmpA3C.aggr_info;if((_tmpA3D.KnownAggr).tag != 2)goto _LL64C;
_tmpA3E=(struct Cyc_Absyn_Aggrdecl**)(_tmpA3D.KnownAggr).val;_tmpA3F=_tmpA3C.targs;
_LL64B: if((*_tmpA3E)->impl == 0)return 0;else{struct Cyc_List_List*_tmpA4B=((struct
Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,(*_tmpA3E)->tvs,_tmpA3F);struct
Cyc_List_List*_tmpA4C=((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmpA3E)->impl))->fields;
void*t;while(_tmpA4C != 0){t=Cyc_Tcutil_rsubstitute(rgn,_tmpA4B,((struct Cyc_Absyn_Aggrfield*)
_tmpA4C->hd)->type);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,t))return 1;
_tmpA4C=_tmpA4C->tl;}return 0;}_LL64C: if(*((int*)_tmpA3A)!= 11)goto _LL64E;_tmpA40=((
struct Cyc_Absyn_AnonAggrType_struct*)_tmpA3A)->f2;_LL64D: while(_tmpA40 != 0){if(
Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,((struct Cyc_Absyn_Aggrfield*)_tmpA40->hd)->type))
return 1;_tmpA40=_tmpA40->tl;}return 0;_LL64E: if(*((int*)_tmpA3A)!= 10)goto _LL650;
_tmpA41=((struct Cyc_Absyn_AggrType_struct*)_tmpA3A)->f1;_tmpA42=_tmpA41.aggr_info;
if((_tmpA42.UnknownAggr).tag != 1)goto _LL650;_tmpA43=(struct _tuple4)(_tmpA42.UnknownAggr).val;
_LL64F:({void*_tmpA4D=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(({const char*_tmpA4E="got unknown aggr in is_noalias_aggr";
_tag_dyneither(_tmpA4E,sizeof(char),36);}),_tag_dyneither(_tmpA4D,sizeof(void*),
0));});_LL650: if(*((int*)_tmpA3A)!= 2)goto _LL652;_tmpA44=((struct Cyc_Absyn_DatatypeType_struct*)
_tmpA3A)->f1;_tmpA45=_tmpA44.datatype_info;_tmpA46=_tmpA44.targs;_tmpA47=_tmpA44.rgn;
_LL651: {union Cyc_Absyn_DatatypeInfoU _tmpA4F=_tmpA45;struct Cyc_Absyn_UnknownDatatypeInfo
_tmpA50;struct _tuple2*_tmpA51;int _tmpA52;struct Cyc_Absyn_Datatypedecl**_tmpA53;
struct Cyc_Absyn_Datatypedecl*_tmpA54;struct Cyc_Absyn_Datatypedecl _tmpA55;struct
Cyc_List_List*_tmpA56;struct Cyc_Core_Opt*_tmpA57;_LL657: if((_tmpA4F.UnknownDatatype).tag
!= 1)goto _LL659;_tmpA50=(struct Cyc_Absyn_UnknownDatatypeInfo)(_tmpA4F.UnknownDatatype).val;
_tmpA51=_tmpA50.name;_tmpA52=_tmpA50.is_extensible;_LL658:({void*_tmpA58=0;((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmpA59="got unknown datatype in is_noalias_aggr";_tag_dyneither(_tmpA59,sizeof(
char),40);}),_tag_dyneither(_tmpA58,sizeof(void*),0));});_LL659: if((_tmpA4F.KnownDatatype).tag
!= 2)goto _LL656;_tmpA53=(struct Cyc_Absyn_Datatypedecl**)(_tmpA4F.KnownDatatype).val;
_tmpA54=*_tmpA53;_tmpA55=*_tmpA54;_tmpA56=_tmpA55.tvs;_tmpA57=_tmpA55.fields;
_LL65A: if(_tmpA47 == 0)return 0;return Cyc_Tcutil_is_noalias_region((void*)_tmpA47->v);
_LL656:;}_LL652: if(*((int*)_tmpA3A)!= 3)goto _LL654;_tmpA48=((struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmpA3A)->f1;_tmpA49=_tmpA48.field_info;_tmpA4A=_tmpA48.targs;_LL653: {union Cyc_Absyn_DatatypeFieldInfoU
_tmpA5A=_tmpA49;struct Cyc_Absyn_UnknownDatatypeFieldInfo _tmpA5B;struct _tuple3
_tmpA5C;struct Cyc_Absyn_Datatypedecl*_tmpA5D;struct Cyc_Absyn_Datatypefield*
_tmpA5E;_LL65C: if((_tmpA5A.UnknownDatatypefield).tag != 1)goto _LL65E;_tmpA5B=(
struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmpA5A.UnknownDatatypefield).val;
_LL65D:({void*_tmpA5F=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(({const char*_tmpA60="got unknown datatype field in is_noalias_aggr";
_tag_dyneither(_tmpA60,sizeof(char),46);}),_tag_dyneither(_tmpA5F,sizeof(void*),
0));});_LL65E: if((_tmpA5A.KnownDatatypefield).tag != 2)goto _LL65B;_tmpA5C=(struct
_tuple3)(_tmpA5A.KnownDatatypefield).val;_tmpA5D=_tmpA5C.f1;_tmpA5E=_tmpA5C.f2;
_LL65F: {struct Cyc_List_List*_tmpA61=((struct Cyc_List_List*(*)(struct
_RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(rgn,rgn,_tmpA5D->tvs,_tmpA4A);struct Cyc_List_List*_tmpA62=
_tmpA5E->typs;while(_tmpA62 != 0){_tmpA39=Cyc_Tcutil_rsubstitute(rgn,_tmpA61,(*((
struct _tuple11*)_tmpA62->hd)).f2);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,
_tmpA39))return 1;_tmpA62=_tmpA62->tl;}return 0;}_LL65B:;}_LL654:;_LL655: return 0;
_LL647:;}}static int Cyc_Tcutil_is_noalias_field(struct _RegionHandle*r,void*t,
struct _dyneither_ptr*f){void*_tmpA63=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo
_tmpA64;union Cyc_Absyn_AggrInfoU _tmpA65;struct Cyc_Absyn_Aggrdecl**_tmpA66;struct
Cyc_Absyn_Aggrdecl*_tmpA67;struct Cyc_List_List*_tmpA68;struct Cyc_List_List*
_tmpA69;_LL661: if(_tmpA63 <= (void*)4)goto _LL665;if(*((int*)_tmpA63)!= 10)goto
_LL663;_tmpA64=((struct Cyc_Absyn_AggrType_struct*)_tmpA63)->f1;_tmpA65=_tmpA64.aggr_info;
if((_tmpA65.KnownAggr).tag != 2)goto _LL663;_tmpA66=(struct Cyc_Absyn_Aggrdecl**)(
_tmpA65.KnownAggr).val;_tmpA67=*_tmpA66;_tmpA68=_tmpA64.targs;_LL662: _tmpA69=
_tmpA67->impl == 0?0:((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpA67->impl))->fields;
goto _LL664;_LL663: if(*((int*)_tmpA63)!= 11)goto _LL665;_tmpA69=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmpA63)->f2;_LL664: {struct Cyc_Absyn_Aggrfield*_tmpA6A=Cyc_Absyn_lookup_field(
_tmpA69,f);{struct Cyc_Absyn_Aggrfield*_tmpA6B=_tmpA6A;struct Cyc_Absyn_Aggrfield
_tmpA6C;void*_tmpA6D;_LL668: if(_tmpA6B != 0)goto _LL66A;_LL669:({void*_tmpA6E=0;((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const
char*_tmpA6F="is_noalias_field: missing field";_tag_dyneither(_tmpA6F,sizeof(
char),32);}),_tag_dyneither(_tmpA6E,sizeof(void*),0));});_LL66A: if(_tmpA6B == 0)
goto _LL667;_tmpA6C=*_tmpA6B;_tmpA6D=_tmpA6C.type;_LL66B: return Cyc_Tcutil_is_noalias_pointer_or_aggr(
r,_tmpA6D);_LL667:;}return 0;}_LL665:;_LL666:({struct Cyc_String_pa_struct _tmpA72;
_tmpA72.tag=0;_tmpA72.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t));{void*_tmpA70[1]={& _tmpA72};((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmpA71="is_noalias_field: invalid type |%s|";
_tag_dyneither(_tmpA71,sizeof(char),36);}),_tag_dyneither(_tmpA70,sizeof(void*),
1));}});_LL660:;}static int Cyc_Tcutil_is_noalias_path_aux(struct _RegionHandle*r,
struct Cyc_Absyn_Exp*e,int ignore_leaf){void*_tmpA73=e->r;void*_tmpA74;struct Cyc_Absyn_Exp*
_tmpA75;struct _dyneither_ptr*_tmpA76;struct Cyc_Absyn_Exp*_tmpA77;struct Cyc_Absyn_Exp*
_tmpA78;void*_tmpA79;void*_tmpA7A;void*_tmpA7B;struct Cyc_Absyn_Exp*_tmpA7C;
struct Cyc_Absyn_Exp*_tmpA7D;struct Cyc_Absyn_Exp*_tmpA7E;struct Cyc_Absyn_Exp*
_tmpA7F;void*_tmpA80;struct Cyc_Absyn_Exp*_tmpA81;struct Cyc_Absyn_Stmt*_tmpA82;
_LL66D: if(*((int*)_tmpA73)!= 5)goto _LL66F;_LL66E: goto _LL670;_LL66F: if(*((int*)
_tmpA73)!= 7)goto _LL671;_LL670: goto _LL672;_LL671: if(*((int*)_tmpA73)!= 8)goto
_LL673;_LL672: goto _LL674;_LL673: if(*((int*)_tmpA73)!= 12)goto _LL675;_LL674: goto
_LL676;_LL675: if(*((int*)_tmpA73)!= 16)goto _LL677;_LL676: goto _LL678;_LL677: if(*((
int*)_tmpA73)!= 18)goto _LL679;_LL678: goto _LL67A;_LL679: if(*((int*)_tmpA73)!= 19)
goto _LL67B;_LL67A: goto _LL67C;_LL67B: if(*((int*)_tmpA73)!= 20)goto _LL67D;_LL67C:
goto _LL67E;_LL67D: if(*((int*)_tmpA73)!= 21)goto _LL67F;_LL67E: goto _LL680;_LL67F:
if(*((int*)_tmpA73)!= 27)goto _LL681;_LL680: goto _LL682;_LL681: if(*((int*)_tmpA73)
!= 29)goto _LL683;_LL682: goto _LL684;_LL683: if(*((int*)_tmpA73)!= 28)goto _LL685;
_LL684: goto _LL686;_LL685: if(*((int*)_tmpA73)!= 33)goto _LL687;_LL686: goto _LL688;
_LL687: if(*((int*)_tmpA73)!= 34)goto _LL689;_LL688: goto _LL68A;_LL689: if(*((int*)
_tmpA73)!= 36)goto _LL68B;_LL68A: goto _LL68C;_LL68B: if(*((int*)_tmpA73)!= 1)goto
_LL68D;_tmpA74=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmpA73)->f2;if(_tmpA74 <= (
void*)1)goto _LL68D;if(*((int*)_tmpA74)!= 0)goto _LL68D;_LL68C: goto _LL68E;_LL68D:
if(*((int*)_tmpA73)!= 3)goto _LL68F;_LL68E: return 0;_LL68F: if(*((int*)_tmpA73)!= 
22)goto _LL691;_LL690: goto _LL692;_LL691: if(*((int*)_tmpA73)!= 24)goto _LL693;
_LL692: return 0;_LL693: if(*((int*)_tmpA73)!= 23)goto _LL695;_tmpA75=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmpA73)->f1;_tmpA76=((struct Cyc_Absyn_AggrMember_e_struct*)_tmpA73)->f2;_LL694:
return(ignore_leaf  || Cyc_Tcutil_is_noalias_field(r,(void*)((struct Cyc_Core_Opt*)
_check_null(_tmpA75->topt))->v,_tmpA76)) && Cyc_Tcutil_is_noalias_path_aux(r,
_tmpA75,0);_LL695: if(*((int*)_tmpA73)!= 25)goto _LL697;_tmpA77=((struct Cyc_Absyn_Subscript_e_struct*)
_tmpA73)->f1;_tmpA78=((struct Cyc_Absyn_Subscript_e_struct*)_tmpA73)->f2;_LL696: {
void*_tmpA83=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmpA77->topt))->v);struct Cyc_Absyn_PtrInfo _tmpA84;void*_tmpA85;struct Cyc_List_List*
_tmpA86;_LL6BA: if(_tmpA83 <= (void*)4)goto _LL6BE;if(*((int*)_tmpA83)!= 4)goto
_LL6BC;_tmpA84=((struct Cyc_Absyn_PointerType_struct*)_tmpA83)->f1;_tmpA85=
_tmpA84.elt_typ;_LL6BB: return 0;_LL6BC: if(*((int*)_tmpA83)!= 9)goto _LL6BE;_tmpA86=((
struct Cyc_Absyn_TupleType_struct*)_tmpA83)->f1;_LL6BD: {unsigned int _tmpA88;int
_tmpA89;struct _tuple13 _tmpA87=Cyc_Evexp_eval_const_uint_exp(_tmpA78);_tmpA88=
_tmpA87.f1;_tmpA89=_tmpA87.f2;if(!_tmpA89)({void*_tmpA8A=0;((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmpA8B="is_noalias_path_aux: non-constant subscript";
_tag_dyneither(_tmpA8B,sizeof(char),44);}),_tag_dyneither(_tmpA8A,sizeof(void*),
0));});{struct _handler_cons _tmpA8C;_push_handler(& _tmpA8C);{int _tmpA8E=0;if(
setjmp(_tmpA8C.handler))_tmpA8E=1;if(!_tmpA8E){{void*_tmpA8F=(*((struct _tuple11*(*)(
struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmpA86,(int)_tmpA88)).f2;int _tmpA90=(
ignore_leaf  || Cyc_Tcutil_is_noalias_pointer_or_aggr(r,_tmpA8F)) && Cyc_Tcutil_is_noalias_path_aux(
r,_tmpA77,0);_npop_handler(0);return _tmpA90;};_pop_handler();}else{void*_tmpA8D=(
void*)_exn_thrown;void*_tmpA92=_tmpA8D;_LL6C1: if(_tmpA92 != Cyc_List_Nth)goto
_LL6C3;_LL6C2: return({void*_tmpA93=0;((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmpA94="is_noalias_path_aux: out-of-bounds subscript";
_tag_dyneither(_tmpA94,sizeof(char),45);}),_tag_dyneither(_tmpA93,sizeof(void*),
0));});_LL6C3:;_LL6C4:(void)_throw(_tmpA92);_LL6C0:;}}}}_LL6BE:;_LL6BF:({void*
_tmpA95=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmpA96="is_noalias_path_aux: subscript on non-pointer/tuple";
_tag_dyneither(_tmpA96,sizeof(char),52);}),_tag_dyneither(_tmpA95,sizeof(void*),
0));});_LL6B9:;}_LL697: if(*((int*)_tmpA73)!= 32)goto _LL699;_LL698: goto _LL69A;
_LL699: if(*((int*)_tmpA73)!= 26)goto _LL69B;_LL69A: goto _LL69C;_LL69B: if(*((int*)
_tmpA73)!= 30)goto _LL69D;_LL69C: goto _LL69E;_LL69D: if(*((int*)_tmpA73)!= 31)goto
_LL69F;_LL69E: goto _LL6A0;_LL69F: if(*((int*)_tmpA73)!= 0)goto _LL6A1;_LL6A0: goto
_LL6A2;_LL6A1: if(*((int*)_tmpA73)!= 35)goto _LL6A3;_LL6A2: goto _LL6A4;_LL6A3: if(*((
int*)_tmpA73)!= 17)goto _LL6A5;_LL6A4: goto _LL6A6;_LL6A5: if(*((int*)_tmpA73)!= 1)
goto _LL6A7;_tmpA79=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmpA73)->f2;if(
_tmpA79 <= (void*)1)goto _LL6A7;if(*((int*)_tmpA79)!= 3)goto _LL6A7;_LL6A6: goto
_LL6A8;_LL6A7: if(*((int*)_tmpA73)!= 1)goto _LL6A9;_tmpA7A=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmpA73)->f2;if(_tmpA7A <= (void*)1)goto _LL6A9;if(*((int*)_tmpA7A)!= 4)goto _LL6A9;
_LL6A8: goto _LL6AA;_LL6A9: if(*((int*)_tmpA73)!= 1)goto _LL6AB;_tmpA7B=(void*)((
struct Cyc_Absyn_Var_e_struct*)_tmpA73)->f2;if(_tmpA7B <= (void*)1)goto _LL6AB;if(*((
int*)_tmpA7B)!= 2)goto _LL6AB;_LL6AA: {int _tmpA97=ignore_leaf  || Cyc_Tcutil_is_noalias_pointer_or_aggr(
r,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);return _tmpA97;}_LL6AB:
if(*((int*)_tmpA73)!= 6)goto _LL6AD;_tmpA7C=((struct Cyc_Absyn_Conditional_e_struct*)
_tmpA73)->f2;_LL6AC: _tmpA7D=_tmpA7C;goto _LL6AE;_LL6AD: if(*((int*)_tmpA73)!= 9)
goto _LL6AF;_tmpA7D=((struct Cyc_Absyn_SeqExp_e_struct*)_tmpA73)->f2;_LL6AE:
_tmpA7E=_tmpA7D;goto _LL6B0;_LL6AF: if(*((int*)_tmpA73)!= 4)goto _LL6B1;_tmpA7E=((
struct Cyc_Absyn_AssignOp_e_struct*)_tmpA73)->f1;_LL6B0: return Cyc_Tcutil_is_noalias_path_aux(
r,_tmpA7E,ignore_leaf);_LL6B1: if(*((int*)_tmpA73)!= 11)goto _LL6B3;_tmpA7F=((
struct Cyc_Absyn_FnCall_e_struct*)_tmpA73)->f1;_LL6B2: {void*_tmpA98=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpA7F->topt))->v);struct Cyc_Absyn_FnInfo
_tmpA99;void*_tmpA9A;struct Cyc_Absyn_PtrInfo _tmpA9B;void*_tmpA9C;_LL6C6: if(
_tmpA98 <= (void*)4)goto _LL6CA;if(*((int*)_tmpA98)!= 8)goto _LL6C8;_tmpA99=((
struct Cyc_Absyn_FnType_struct*)_tmpA98)->f1;_tmpA9A=_tmpA99.ret_typ;_LL6C7:
return ignore_leaf  || Cyc_Tcutil_is_noalias_pointer_or_aggr(r,_tmpA9A);_LL6C8: if(*((
int*)_tmpA98)!= 4)goto _LL6CA;_tmpA9B=((struct Cyc_Absyn_PointerType_struct*)
_tmpA98)->f1;_tmpA9C=_tmpA9B.elt_typ;_LL6C9:{void*_tmpA9D=Cyc_Tcutil_compress(
_tmpA9C);struct Cyc_Absyn_FnInfo _tmpA9E;void*_tmpA9F;_LL6CD: if(_tmpA9D <= (void*)4)
goto _LL6CF;if(*((int*)_tmpA9D)!= 8)goto _LL6CF;_tmpA9E=((struct Cyc_Absyn_FnType_struct*)
_tmpA9D)->f1;_tmpA9F=_tmpA9E.ret_typ;_LL6CE: return ignore_leaf  || Cyc_Tcutil_is_noalias_pointer_or_aggr(
r,_tmpA9F);_LL6CF:;_LL6D0: goto _LL6CC;_LL6CC:;}goto _LL6CB;_LL6CA:;_LL6CB:({struct
Cyc_String_pa_struct _tmpAA2;_tmpAA2.tag=0;_tmpAA2.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(_tmpA7F->topt))->v));{void*_tmpAA0[1]={& _tmpAA2};((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmpAA1="Fncall has non function type %s";
_tag_dyneither(_tmpAA1,sizeof(char),32);}),_tag_dyneither(_tmpAA0,sizeof(void*),
1));}});_LL6C5:;}_LL6B3: if(*((int*)_tmpA73)!= 15)goto _LL6B5;_tmpA80=(void*)((
struct Cyc_Absyn_Cast_e_struct*)_tmpA73)->f1;_tmpA81=((struct Cyc_Absyn_Cast_e_struct*)
_tmpA73)->f2;_LL6B4: return Cyc_Tcutil_is_noalias_pointer_or_aggr(r,_tmpA80) && 
Cyc_Tcutil_is_noalias_path_aux(r,_tmpA81,1);_LL6B5: if(*((int*)_tmpA73)!= 38)goto
_LL6B7;_tmpA82=((struct Cyc_Absyn_StmtExp_e_struct*)_tmpA73)->f1;_LL6B6: while(1){
void*_tmpAA3=_tmpA82->r;struct Cyc_Absyn_Stmt*_tmpAA4;struct Cyc_Absyn_Stmt*
_tmpAA5;struct Cyc_Absyn_Decl*_tmpAA6;struct Cyc_Absyn_Stmt*_tmpAA7;struct Cyc_Absyn_Exp*
_tmpAA8;_LL6D2: if(_tmpAA3 <= (void*)1)goto _LL6D8;if(*((int*)_tmpAA3)!= 1)goto
_LL6D4;_tmpAA4=((struct Cyc_Absyn_Seq_s_struct*)_tmpAA3)->f1;_tmpAA5=((struct Cyc_Absyn_Seq_s_struct*)
_tmpAA3)->f2;_LL6D3: _tmpA82=_tmpAA5;goto _LL6D1;_LL6D4: if(*((int*)_tmpAA3)!= 11)
goto _LL6D6;_tmpAA6=((struct Cyc_Absyn_Decl_s_struct*)_tmpAA3)->f1;_tmpAA7=((
struct Cyc_Absyn_Decl_s_struct*)_tmpAA3)->f2;_LL6D5: _tmpA82=_tmpAA7;goto _LL6D1;
_LL6D6: if(*((int*)_tmpAA3)!= 0)goto _LL6D8;_tmpAA8=((struct Cyc_Absyn_Exp_s_struct*)
_tmpAA3)->f1;_LL6D7: return Cyc_Tcutil_is_noalias_path_aux(r,_tmpAA8,ignore_leaf);
_LL6D8:;_LL6D9:({void*_tmpAA9=0;((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmpAAA="is_noalias_stmt_exp: ill-formed StmtExp";
_tag_dyneither(_tmpAAA,sizeof(char),40);}),_tag_dyneither(_tmpAA9,sizeof(void*),
0));});_LL6D1:;}_LL6B7:;_LL6B8: return 0;_LL66C:;}int Cyc_Tcutil_is_noalias_path(
struct _RegionHandle*r,struct Cyc_Absyn_Exp*e){return Cyc_Tcutil_is_noalias_path_aux(
r,e,0);}struct _tuple16 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*te,struct
Cyc_Absyn_Exp*e){struct _tuple16 bogus_ans=({struct _tuple16 _tmpAF0;_tmpAF0.f1=0;
_tmpAF0.f2=(void*)2;_tmpAF0;});void*_tmpAAB=e->r;struct _tuple2*_tmpAAC;void*
_tmpAAD;struct Cyc_Absyn_Exp*_tmpAAE;struct _dyneither_ptr*_tmpAAF;int _tmpAB0;
struct Cyc_Absyn_Exp*_tmpAB1;struct _dyneither_ptr*_tmpAB2;int _tmpAB3;struct Cyc_Absyn_Exp*
_tmpAB4;struct Cyc_Absyn_Exp*_tmpAB5;struct Cyc_Absyn_Exp*_tmpAB6;_LL6DB: if(*((int*)
_tmpAAB)!= 1)goto _LL6DD;_tmpAAC=((struct Cyc_Absyn_Var_e_struct*)_tmpAAB)->f1;
_tmpAAD=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmpAAB)->f2;_LL6DC: {void*
_tmpAB7=_tmpAAD;struct Cyc_Absyn_Vardecl*_tmpAB8;struct Cyc_Absyn_Vardecl*_tmpAB9;
struct Cyc_Absyn_Vardecl*_tmpABA;struct Cyc_Absyn_Vardecl*_tmpABB;_LL6E8: if((int)
_tmpAB7 != 0)goto _LL6EA;_LL6E9: goto _LL6EB;_LL6EA: if(_tmpAB7 <= (void*)1)goto _LL6EC;
if(*((int*)_tmpAB7)!= 1)goto _LL6EC;_LL6EB: return bogus_ans;_LL6EC: if(_tmpAB7 <= (
void*)1)goto _LL6EE;if(*((int*)_tmpAB7)!= 0)goto _LL6EE;_tmpAB8=((struct Cyc_Absyn_Global_b_struct*)
_tmpAB7)->f1;_LL6ED: {void*_tmpABC=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);_LL6F5: if(_tmpABC <= (void*)4)goto _LL6F7;if(*((int*)
_tmpABC)!= 7)goto _LL6F7;_LL6F6: return({struct _tuple16 _tmpABD;_tmpABD.f1=1;
_tmpABD.f2=(void*)2;_tmpABD;});_LL6F7:;_LL6F8: return({struct _tuple16 _tmpABE;
_tmpABE.f1=(_tmpAB8->tq).real_const;_tmpABE.f2=(void*)2;_tmpABE;});_LL6F4:;}
_LL6EE: if(_tmpAB7 <= (void*)1)goto _LL6F0;if(*((int*)_tmpAB7)!= 3)goto _LL6F0;
_tmpAB9=((struct Cyc_Absyn_Local_b_struct*)_tmpAB7)->f1;_LL6EF: {void*_tmpABF=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);_LL6FA: if(_tmpABF <= (void*)
4)goto _LL6FC;if(*((int*)_tmpABF)!= 7)goto _LL6FC;_LL6FB: return({struct _tuple16
_tmpAC0;_tmpAC0.f1=1;_tmpAC0.f2=(void*)((struct Cyc_Core_Opt*)_check_null(_tmpAB9->rgn))->v;
_tmpAC0;});_LL6FC:;_LL6FD: _tmpAB9->escapes=1;return({struct _tuple16 _tmpAC1;
_tmpAC1.f1=(_tmpAB9->tq).real_const;_tmpAC1.f2=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmpAB9->rgn))->v;_tmpAC1;});_LL6F9:;}_LL6F0: if(_tmpAB7 <= (void*)1)
goto _LL6F2;if(*((int*)_tmpAB7)!= 4)goto _LL6F2;_tmpABA=((struct Cyc_Absyn_Pat_b_struct*)
_tmpAB7)->f1;_LL6F1: _tmpABB=_tmpABA;goto _LL6F3;_LL6F2: if(_tmpAB7 <= (void*)1)goto
_LL6E7;if(*((int*)_tmpAB7)!= 2)goto _LL6E7;_tmpABB=((struct Cyc_Absyn_Param_b_struct*)
_tmpAB7)->f1;_LL6F3: _tmpABB->escapes=1;return({struct _tuple16 _tmpAC2;_tmpAC2.f1=(
_tmpABB->tq).real_const;_tmpAC2.f2=(void*)((struct Cyc_Core_Opt*)_check_null(
_tmpABB->rgn))->v;_tmpAC2;});_LL6E7:;}_LL6DD: if(*((int*)_tmpAAB)!= 23)goto _LL6DF;
_tmpAAE=((struct Cyc_Absyn_AggrMember_e_struct*)_tmpAAB)->f1;_tmpAAF=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmpAAB)->f2;_tmpAB0=((struct Cyc_Absyn_AggrMember_e_struct*)_tmpAAB)->f3;_LL6DE:
if(_tmpAB0)return bogus_ans;{void*_tmpAC3=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmpAAE->topt))->v);struct Cyc_List_List*_tmpAC4;struct Cyc_Absyn_AggrInfo
_tmpAC5;union Cyc_Absyn_AggrInfoU _tmpAC6;struct Cyc_Absyn_Aggrdecl**_tmpAC7;struct
Cyc_Absyn_Aggrdecl*_tmpAC8;_LL6FF: if(_tmpAC3 <= (void*)4)goto _LL703;if(*((int*)
_tmpAC3)!= 11)goto _LL701;_tmpAC4=((struct Cyc_Absyn_AnonAggrType_struct*)_tmpAC3)->f2;
_LL700: {struct Cyc_Absyn_Aggrfield*_tmpAC9=Cyc_Absyn_lookup_field(_tmpAC4,
_tmpAAF);if(_tmpAC9 != 0  && _tmpAC9->width != 0)return({struct _tuple16 _tmpACA;
_tmpACA.f1=(_tmpAC9->tq).real_const;_tmpACA.f2=(Cyc_Tcutil_addressof_props(te,
_tmpAAE)).f2;_tmpACA;});return bogus_ans;}_LL701: if(*((int*)_tmpAC3)!= 10)goto
_LL703;_tmpAC5=((struct Cyc_Absyn_AggrType_struct*)_tmpAC3)->f1;_tmpAC6=_tmpAC5.aggr_info;
if((_tmpAC6.KnownAggr).tag != 2)goto _LL703;_tmpAC7=(struct Cyc_Absyn_Aggrdecl**)(
_tmpAC6.KnownAggr).val;_tmpAC8=*_tmpAC7;_LL702: {struct Cyc_Absyn_Aggrfield*
_tmpACB=Cyc_Absyn_lookup_decl_field(_tmpAC8,_tmpAAF);if(_tmpACB != 0  && _tmpACB->width
!= 0)return({struct _tuple16 _tmpACC;_tmpACC.f1=(_tmpACB->tq).real_const;_tmpACC.f2=(
Cyc_Tcutil_addressof_props(te,_tmpAAE)).f2;_tmpACC;});return bogus_ans;}_LL703:;
_LL704: return bogus_ans;_LL6FE:;}_LL6DF: if(*((int*)_tmpAAB)!= 24)goto _LL6E1;
_tmpAB1=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmpAAB)->f1;_tmpAB2=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmpAAB)->f2;_tmpAB3=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmpAAB)->f3;_LL6E0:
if(_tmpAB3)return bogus_ans;{void*_tmpACD=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmpAB1->topt))->v);struct Cyc_Absyn_PtrInfo _tmpACE;void*_tmpACF;
struct Cyc_Absyn_PtrAtts _tmpAD0;void*_tmpAD1;_LL706: if(_tmpACD <= (void*)4)goto
_LL708;if(*((int*)_tmpACD)!= 4)goto _LL708;_tmpACE=((struct Cyc_Absyn_PointerType_struct*)
_tmpACD)->f1;_tmpACF=_tmpACE.elt_typ;_tmpAD0=_tmpACE.ptr_atts;_tmpAD1=_tmpAD0.rgn;
_LL707: {struct Cyc_Absyn_Aggrfield*finfo;{void*_tmpAD2=Cyc_Tcutil_compress(
_tmpACF);struct Cyc_List_List*_tmpAD3;struct Cyc_Absyn_AggrInfo _tmpAD4;union Cyc_Absyn_AggrInfoU
_tmpAD5;struct Cyc_Absyn_Aggrdecl**_tmpAD6;struct Cyc_Absyn_Aggrdecl*_tmpAD7;
_LL70B: if(_tmpAD2 <= (void*)4)goto _LL70F;if(*((int*)_tmpAD2)!= 11)goto _LL70D;
_tmpAD3=((struct Cyc_Absyn_AnonAggrType_struct*)_tmpAD2)->f2;_LL70C: finfo=Cyc_Absyn_lookup_field(
_tmpAD3,_tmpAB2);goto _LL70A;_LL70D: if(*((int*)_tmpAD2)!= 10)goto _LL70F;_tmpAD4=((
struct Cyc_Absyn_AggrType_struct*)_tmpAD2)->f1;_tmpAD5=_tmpAD4.aggr_info;if((
_tmpAD5.KnownAggr).tag != 2)goto _LL70F;_tmpAD6=(struct Cyc_Absyn_Aggrdecl**)(
_tmpAD5.KnownAggr).val;_tmpAD7=*_tmpAD6;_LL70E: finfo=Cyc_Absyn_lookup_decl_field(
_tmpAD7,_tmpAB2);goto _LL70A;_LL70F:;_LL710: return bogus_ans;_LL70A:;}if(finfo != 0
 && finfo->width != 0)return({struct _tuple16 _tmpAD8;_tmpAD8.f1=(finfo->tq).real_const;
_tmpAD8.f2=_tmpAD1;_tmpAD8;});return bogus_ans;}_LL708:;_LL709: return bogus_ans;
_LL705:;}_LL6E1: if(*((int*)_tmpAAB)!= 22)goto _LL6E3;_tmpAB4=((struct Cyc_Absyn_Deref_e_struct*)
_tmpAAB)->f1;_LL6E2: {void*_tmpAD9=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmpAB4->topt))->v);struct Cyc_Absyn_PtrInfo _tmpADA;struct Cyc_Absyn_Tqual
_tmpADB;struct Cyc_Absyn_PtrAtts _tmpADC;void*_tmpADD;_LL712: if(_tmpAD9 <= (void*)4)
goto _LL714;if(*((int*)_tmpAD9)!= 4)goto _LL714;_tmpADA=((struct Cyc_Absyn_PointerType_struct*)
_tmpAD9)->f1;_tmpADB=_tmpADA.elt_tq;_tmpADC=_tmpADA.ptr_atts;_tmpADD=_tmpADC.rgn;
_LL713: return({struct _tuple16 _tmpADE;_tmpADE.f1=_tmpADB.real_const;_tmpADE.f2=
_tmpADD;_tmpADE;});_LL714:;_LL715: return bogus_ans;_LL711:;}_LL6E3: if(*((int*)
_tmpAAB)!= 25)goto _LL6E5;_tmpAB5=((struct Cyc_Absyn_Subscript_e_struct*)_tmpAAB)->f1;
_tmpAB6=((struct Cyc_Absyn_Subscript_e_struct*)_tmpAAB)->f2;_LL6E4: {void*t=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpAB5->topt))->v);void*_tmpADF=t;
struct Cyc_List_List*_tmpAE0;struct Cyc_Absyn_PtrInfo _tmpAE1;struct Cyc_Absyn_Tqual
_tmpAE2;struct Cyc_Absyn_PtrAtts _tmpAE3;void*_tmpAE4;struct Cyc_Absyn_ArrayInfo
_tmpAE5;struct Cyc_Absyn_Tqual _tmpAE6;_LL717: if(_tmpADF <= (void*)4)goto _LL71D;if(*((
int*)_tmpADF)!= 9)goto _LL719;_tmpAE0=((struct Cyc_Absyn_TupleType_struct*)_tmpADF)->f1;
_LL718: {unsigned int _tmpAE8;int _tmpAE9;struct _tuple13 _tmpAE7=Cyc_Evexp_eval_const_uint_exp(
_tmpAB6);_tmpAE8=_tmpAE7.f1;_tmpAE9=_tmpAE7.f2;if(!_tmpAE9)return bogus_ans;{
struct _tuple11*_tmpAEA=Cyc_Absyn_lookup_tuple_field(_tmpAE0,(int)_tmpAE8);if(
_tmpAEA != 0)return({struct _tuple16 _tmpAEB;_tmpAEB.f1=((*_tmpAEA).f1).real_const;
_tmpAEB.f2=(Cyc_Tcutil_addressof_props(te,_tmpAB5)).f2;_tmpAEB;});return
bogus_ans;}}_LL719: if(*((int*)_tmpADF)!= 4)goto _LL71B;_tmpAE1=((struct Cyc_Absyn_PointerType_struct*)
_tmpADF)->f1;_tmpAE2=_tmpAE1.elt_tq;_tmpAE3=_tmpAE1.ptr_atts;_tmpAE4=_tmpAE3.rgn;
_LL71A: return({struct _tuple16 _tmpAEC;_tmpAEC.f1=_tmpAE2.real_const;_tmpAEC.f2=
_tmpAE4;_tmpAEC;});_LL71B: if(*((int*)_tmpADF)!= 7)goto _LL71D;_tmpAE5=((struct Cyc_Absyn_ArrayType_struct*)
_tmpADF)->f1;_tmpAE6=_tmpAE5.tq;_LL71C: return({struct _tuple16 _tmpAED;_tmpAED.f1=
_tmpAE6.real_const;_tmpAED.f2=(Cyc_Tcutil_addressof_props(te,_tmpAB5)).f2;
_tmpAED;});_LL71D:;_LL71E: return bogus_ans;_LL716:;}_LL6E5:;_LL6E6:({void*_tmpAEE=
0;Cyc_Tcutil_terr(e->loc,({const char*_tmpAEF="unary & applied to non-lvalue";
_tag_dyneither(_tmpAEF,sizeof(char),30);}),_tag_dyneither(_tmpAEE,sizeof(void*),
0));});return bogus_ans;_LL6DA:;}void*Cyc_Tcutil_array_to_ptr(struct Cyc_Tcenv_Tenv*
te,void*e_typ,struct Cyc_Absyn_Exp*e){void*_tmpAF1=Cyc_Tcutil_compress(e_typ);
struct Cyc_Absyn_ArrayInfo _tmpAF2;void*_tmpAF3;struct Cyc_Absyn_Tqual _tmpAF4;union
Cyc_Absyn_Constraint*_tmpAF5;_LL720: if(_tmpAF1 <= (void*)4)goto _LL722;if(*((int*)
_tmpAF1)!= 7)goto _LL722;_tmpAF2=((struct Cyc_Absyn_ArrayType_struct*)_tmpAF1)->f1;
_tmpAF3=_tmpAF2.elt_type;_tmpAF4=_tmpAF2.tq;_tmpAF5=_tmpAF2.zero_term;_LL721: {
void*_tmpAF7;struct _tuple16 _tmpAF6=Cyc_Tcutil_addressof_props(te,e);_tmpAF7=
_tmpAF6.f2;return Cyc_Absyn_atb_typ(_tmpAF3,_tmpAF7,_tmpAF4,(void*)({struct Cyc_Absyn_Upper_b_struct*
_tmpAF8=_cycalloc(sizeof(*_tmpAF8));_tmpAF8[0]=({struct Cyc_Absyn_Upper_b_struct
_tmpAF9;_tmpAF9.tag=0;_tmpAF9.f1=e;_tmpAF9;});_tmpAF8;}),_tmpAF5);}_LL722:;
_LL723: return e_typ;_LL71F:;}void Cyc_Tcutil_check_bound(struct Cyc_Position_Segment*
loc,unsigned int i,union Cyc_Absyn_Constraint*b){b=Cyc_Absyn_compress_conref(b);{
void*_tmpAFA=Cyc_Absyn_conref_constr(Cyc_Absyn_bounds_one,b);struct Cyc_Absyn_Exp*
_tmpAFB;_LL725: if((int)_tmpAFA != 0)goto _LL727;_LL726: return;_LL727: if(_tmpAFA <= (
void*)1)goto _LL724;if(*((int*)_tmpAFA)!= 0)goto _LL724;_tmpAFB=((struct Cyc_Absyn_Upper_b_struct*)
_tmpAFA)->f1;_LL728: {unsigned int _tmpAFD;int _tmpAFE;struct _tuple13 _tmpAFC=Cyc_Evexp_eval_const_uint_exp(
_tmpAFB);_tmpAFD=_tmpAFC.f1;_tmpAFE=_tmpAFC.f2;if(_tmpAFE  && _tmpAFD <= i)({
struct Cyc_Int_pa_struct _tmpB02;_tmpB02.tag=1;_tmpB02.f1=(unsigned long)((int)i);{
struct Cyc_Int_pa_struct _tmpB01;_tmpB01.tag=1;_tmpB01.f1=(unsigned long)((int)
_tmpAFD);{void*_tmpAFF[2]={& _tmpB01,& _tmpB02};Cyc_Tcutil_terr(loc,({const char*
_tmpB00="dereference is out of bounds: %d <= %d";_tag_dyneither(_tmpB00,sizeof(
char),39);}),_tag_dyneither(_tmpAFF,sizeof(void*),2));}}});return;}_LL724:;}}
void Cyc_Tcutil_check_nonzero_bound(struct Cyc_Position_Segment*loc,union Cyc_Absyn_Constraint*
b){Cyc_Tcutil_check_bound(loc,0,b);}int Cyc_Tcutil_is_bound_one(union Cyc_Absyn_Constraint*
b){void*_tmpB03=Cyc_Absyn_conref_def((void*)((void*)0),b);struct Cyc_Absyn_Exp*
_tmpB04;_LL72A: if(_tmpB03 <= (void*)1)goto _LL72C;if(*((int*)_tmpB03)!= 0)goto
_LL72C;_tmpB04=((struct Cyc_Absyn_Upper_b_struct*)_tmpB03)->f1;_LL72B: {
unsigned int _tmpB06;int _tmpB07;struct _tuple13 _tmpB05=Cyc_Evexp_eval_const_uint_exp(
_tmpB04);_tmpB06=_tmpB05.f1;_tmpB07=_tmpB05.f2;return _tmpB07  && _tmpB06 == 1;}
_LL72C:;_LL72D: return 0;_LL729:;}int Cyc_Tcutil_bits_only(void*t){void*_tmpB08=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_ArrayInfo _tmpB09;void*_tmpB0A;union Cyc_Absyn_Constraint*
_tmpB0B;struct Cyc_List_List*_tmpB0C;struct Cyc_Absyn_AggrInfo _tmpB0D;union Cyc_Absyn_AggrInfoU
_tmpB0E;struct _tuple4 _tmpB0F;struct Cyc_Absyn_AggrInfo _tmpB10;union Cyc_Absyn_AggrInfoU
_tmpB11;struct Cyc_Absyn_Aggrdecl**_tmpB12;struct Cyc_Absyn_Aggrdecl*_tmpB13;
struct Cyc_List_List*_tmpB14;struct Cyc_List_List*_tmpB15;_LL72F: if((int)_tmpB08 != 
0)goto _LL731;_LL730: goto _LL732;_LL731: if(_tmpB08 <= (void*)4)goto _LL733;if(*((int*)
_tmpB08)!= 5)goto _LL733;_LL732: goto _LL734;_LL733: if((int)_tmpB08 != 1)goto _LL735;
_LL734: goto _LL736;_LL735: if(_tmpB08 <= (void*)4)goto _LL745;if(*((int*)_tmpB08)!= 
6)goto _LL737;_LL736: return 1;_LL737: if(*((int*)_tmpB08)!= 12)goto _LL739;_LL738:
goto _LL73A;_LL739: if(*((int*)_tmpB08)!= 13)goto _LL73B;_LL73A: return 0;_LL73B: if(*((
int*)_tmpB08)!= 7)goto _LL73D;_tmpB09=((struct Cyc_Absyn_ArrayType_struct*)_tmpB08)->f1;
_tmpB0A=_tmpB09.elt_type;_tmpB0B=_tmpB09.zero_term;_LL73C: return !((int(*)(int y,
union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpB0B) && Cyc_Tcutil_bits_only(
_tmpB0A);_LL73D: if(*((int*)_tmpB08)!= 9)goto _LL73F;_tmpB0C=((struct Cyc_Absyn_TupleType_struct*)
_tmpB08)->f1;_LL73E: for(0;_tmpB0C != 0;_tmpB0C=_tmpB0C->tl){if(!Cyc_Tcutil_bits_only((*((
struct _tuple11*)_tmpB0C->hd)).f2))return 0;}return 1;_LL73F: if(*((int*)_tmpB08)!= 
10)goto _LL741;_tmpB0D=((struct Cyc_Absyn_AggrType_struct*)_tmpB08)->f1;_tmpB0E=
_tmpB0D.aggr_info;if((_tmpB0E.UnknownAggr).tag != 1)goto _LL741;_tmpB0F=(struct
_tuple4)(_tmpB0E.UnknownAggr).val;_LL740: return 0;_LL741: if(*((int*)_tmpB08)!= 10)
goto _LL743;_tmpB10=((struct Cyc_Absyn_AggrType_struct*)_tmpB08)->f1;_tmpB11=
_tmpB10.aggr_info;if((_tmpB11.KnownAggr).tag != 2)goto _LL743;_tmpB12=(struct Cyc_Absyn_Aggrdecl**)(
_tmpB11.KnownAggr).val;_tmpB13=*_tmpB12;_tmpB14=_tmpB10.targs;_LL742: if(_tmpB13->impl
== 0)return 0;{struct _RegionHandle _tmpB16=_new_region("rgn");struct _RegionHandle*
rgn=& _tmpB16;_push_region(rgn);{struct Cyc_List_List*_tmpB17=((struct Cyc_List_List*(*)(
struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(rgn,rgn,_tmpB13->tvs,_tmpB14);{struct Cyc_List_List*fs=((struct
Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpB13->impl))->fields;for(0;fs != 0;fs=fs->tl){
if(!Cyc_Tcutil_bits_only(Cyc_Tcutil_rsubstitute(rgn,_tmpB17,((struct Cyc_Absyn_Aggrfield*)
fs->hd)->type))){int _tmpB18=0;_npop_handler(0);return _tmpB18;}}}{int _tmpB19=1;
_npop_handler(0);return _tmpB19;}};_pop_region(rgn);}_LL743: if(*((int*)_tmpB08)!= 
11)goto _LL745;_tmpB15=((struct Cyc_Absyn_AnonAggrType_struct*)_tmpB08)->f2;_LL744:
for(0;_tmpB15 != 0;_tmpB15=_tmpB15->tl){if(!Cyc_Tcutil_bits_only(((struct Cyc_Absyn_Aggrfield*)
_tmpB15->hd)->type))return 0;}return 1;_LL745:;_LL746: return 0;_LL72E:;}struct
_tuple27{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};static int Cyc_Tcutil_cnst_exp(
struct Cyc_Tcenv_Tenv*te,int var_okay,struct Cyc_Absyn_Exp*e){void*_tmpB1A=e->r;
struct _tuple2*_tmpB1B;void*_tmpB1C;struct Cyc_Absyn_Exp*_tmpB1D;struct Cyc_Absyn_Exp*
_tmpB1E;struct Cyc_Absyn_Exp*_tmpB1F;struct Cyc_Absyn_Exp*_tmpB20;struct Cyc_Absyn_Exp*
_tmpB21;struct Cyc_Absyn_Exp*_tmpB22;struct Cyc_Absyn_Exp*_tmpB23;void*_tmpB24;
struct Cyc_Absyn_Exp*_tmpB25;void*_tmpB26;void*_tmpB27;struct Cyc_Absyn_Exp*
_tmpB28;struct Cyc_Absyn_Exp*_tmpB29;struct Cyc_Absyn_Exp*_tmpB2A;struct Cyc_Absyn_Exp*
_tmpB2B;struct Cyc_List_List*_tmpB2C;struct Cyc_List_List*_tmpB2D;struct Cyc_List_List*
_tmpB2E;void*_tmpB2F;struct Cyc_List_List*_tmpB30;struct Cyc_List_List*_tmpB31;
struct Cyc_List_List*_tmpB32;_LL748: if(*((int*)_tmpB1A)!= 0)goto _LL74A;_LL749:
goto _LL74B;_LL74A: if(*((int*)_tmpB1A)!= 18)goto _LL74C;_LL74B: goto _LL74D;_LL74C:
if(*((int*)_tmpB1A)!= 19)goto _LL74E;_LL74D: goto _LL74F;_LL74E: if(*((int*)_tmpB1A)
!= 20)goto _LL750;_LL74F: goto _LL751;_LL750: if(*((int*)_tmpB1A)!= 21)goto _LL752;
_LL751: goto _LL753;_LL752: if(*((int*)_tmpB1A)!= 33)goto _LL754;_LL753: goto _LL755;
_LL754: if(*((int*)_tmpB1A)!= 34)goto _LL756;_LL755: return 1;_LL756: if(*((int*)
_tmpB1A)!= 1)goto _LL758;_tmpB1B=((struct Cyc_Absyn_Var_e_struct*)_tmpB1A)->f1;
_tmpB1C=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmpB1A)->f2;_LL757: {void*
_tmpB33=_tmpB1C;struct Cyc_Absyn_Vardecl*_tmpB34;_LL777: if(_tmpB33 <= (void*)1)
goto _LL77B;if(*((int*)_tmpB33)!= 1)goto _LL779;_LL778: return 1;_LL779: if(*((int*)
_tmpB33)!= 0)goto _LL77B;_tmpB34=((struct Cyc_Absyn_Global_b_struct*)_tmpB33)->f1;
_LL77A: {void*_tmpB35=Cyc_Tcutil_compress(_tmpB34->type);_LL780: if(_tmpB35 <= (
void*)4)goto _LL784;if(*((int*)_tmpB35)!= 7)goto _LL782;_LL781: goto _LL783;_LL782:
if(*((int*)_tmpB35)!= 8)goto _LL784;_LL783: return 1;_LL784:;_LL785: return var_okay;
_LL77F:;}_LL77B: if((int)_tmpB33 != 0)goto _LL77D;_LL77C: return 0;_LL77D:;_LL77E:
return var_okay;_LL776:;}_LL758: if(*((int*)_tmpB1A)!= 6)goto _LL75A;_tmpB1D=((
struct Cyc_Absyn_Conditional_e_struct*)_tmpB1A)->f1;_tmpB1E=((struct Cyc_Absyn_Conditional_e_struct*)
_tmpB1A)->f2;_tmpB1F=((struct Cyc_Absyn_Conditional_e_struct*)_tmpB1A)->f3;_LL759:
return(Cyc_Tcutil_cnst_exp(te,0,_tmpB1D) && Cyc_Tcutil_cnst_exp(te,0,_tmpB1E))
 && Cyc_Tcutil_cnst_exp(te,0,_tmpB1F);_LL75A: if(*((int*)_tmpB1A)!= 9)goto _LL75C;
_tmpB20=((struct Cyc_Absyn_SeqExp_e_struct*)_tmpB1A)->f1;_tmpB21=((struct Cyc_Absyn_SeqExp_e_struct*)
_tmpB1A)->f2;_LL75B: return Cyc_Tcutil_cnst_exp(te,0,_tmpB20) && Cyc_Tcutil_cnst_exp(
te,0,_tmpB21);_LL75C: if(*((int*)_tmpB1A)!= 13)goto _LL75E;_tmpB22=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmpB1A)->f1;_LL75D: _tmpB23=_tmpB22;goto _LL75F;_LL75E: if(*((int*)_tmpB1A)!= 14)
goto _LL760;_tmpB23=((struct Cyc_Absyn_Instantiate_e_struct*)_tmpB1A)->f1;_LL75F:
return Cyc_Tcutil_cnst_exp(te,var_okay,_tmpB23);_LL760: if(*((int*)_tmpB1A)!= 15)
goto _LL762;_tmpB24=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmpB1A)->f1;_tmpB25=((
struct Cyc_Absyn_Cast_e_struct*)_tmpB1A)->f2;_tmpB26=(void*)((struct Cyc_Absyn_Cast_e_struct*)
_tmpB1A)->f4;if((int)_tmpB26 != 1)goto _LL762;_LL761: return Cyc_Tcutil_cnst_exp(te,
var_okay,_tmpB25);_LL762: if(*((int*)_tmpB1A)!= 15)goto _LL764;_tmpB27=(void*)((
struct Cyc_Absyn_Cast_e_struct*)_tmpB1A)->f1;_tmpB28=((struct Cyc_Absyn_Cast_e_struct*)
_tmpB1A)->f2;_LL763: return Cyc_Tcutil_cnst_exp(te,var_okay,_tmpB28);_LL764: if(*((
int*)_tmpB1A)!= 16)goto _LL766;_tmpB29=((struct Cyc_Absyn_Address_e_struct*)
_tmpB1A)->f1;_LL765: return Cyc_Tcutil_cnst_exp(te,1,_tmpB29);_LL766: if(*((int*)
_tmpB1A)!= 29)goto _LL768;_tmpB2A=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmpB1A)->f2;_tmpB2B=((struct Cyc_Absyn_Comprehension_e_struct*)_tmpB1A)->f3;
_LL767: return Cyc_Tcutil_cnst_exp(te,0,_tmpB2A) && Cyc_Tcutil_cnst_exp(te,0,
_tmpB2B);_LL768: if(*((int*)_tmpB1A)!= 28)goto _LL76A;_tmpB2C=((struct Cyc_Absyn_Array_e_struct*)
_tmpB1A)->f1;_LL769: _tmpB2D=_tmpB2C;goto _LL76B;_LL76A: if(*((int*)_tmpB1A)!= 31)
goto _LL76C;_tmpB2D=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmpB1A)->f2;_LL76B:
_tmpB2E=_tmpB2D;goto _LL76D;_LL76C: if(*((int*)_tmpB1A)!= 30)goto _LL76E;_tmpB2E=((
struct Cyc_Absyn_Aggregate_e_struct*)_tmpB1A)->f3;_LL76D: for(0;_tmpB2E != 0;
_tmpB2E=_tmpB2E->tl){if(!Cyc_Tcutil_cnst_exp(te,0,(*((struct _tuple27*)_tmpB2E->hd)).f2))
return 0;}return 1;_LL76E: if(*((int*)_tmpB1A)!= 3)goto _LL770;_tmpB2F=(void*)((
struct Cyc_Absyn_Primop_e_struct*)_tmpB1A)->f1;_tmpB30=((struct Cyc_Absyn_Primop_e_struct*)
_tmpB1A)->f2;_LL76F: _tmpB31=_tmpB30;goto _LL771;_LL770: if(*((int*)_tmpB1A)!= 26)
goto _LL772;_tmpB31=((struct Cyc_Absyn_Tuple_e_struct*)_tmpB1A)->f1;_LL771: _tmpB32=
_tmpB31;goto _LL773;_LL772: if(*((int*)_tmpB1A)!= 32)goto _LL774;_tmpB32=((struct
Cyc_Absyn_Datatype_e_struct*)_tmpB1A)->f1;_LL773: for(0;_tmpB32 != 0;_tmpB32=
_tmpB32->tl){if(!Cyc_Tcutil_cnst_exp(te,0,(struct Cyc_Absyn_Exp*)_tmpB32->hd))
return 0;}return 1;_LL774:;_LL775: return 0;_LL747:;}int Cyc_Tcutil_is_const_exp(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){return Cyc_Tcutil_cnst_exp(te,0,e);}
static int Cyc_Tcutil_fields_support_default(struct Cyc_List_List*tvs,struct Cyc_List_List*
ts,struct Cyc_List_List*fs);int Cyc_Tcutil_supports_default(void*t){void*_tmpB36=
Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmpB37;struct Cyc_Absyn_PtrAtts
_tmpB38;union Cyc_Absyn_Constraint*_tmpB39;union Cyc_Absyn_Constraint*_tmpB3A;
struct Cyc_Absyn_ArrayInfo _tmpB3B;void*_tmpB3C;struct Cyc_List_List*_tmpB3D;struct
Cyc_Absyn_AggrInfo _tmpB3E;union Cyc_Absyn_AggrInfoU _tmpB3F;struct Cyc_List_List*
_tmpB40;struct Cyc_List_List*_tmpB41;_LL787: if((int)_tmpB36 != 0)goto _LL789;_LL788:
goto _LL78A;_LL789: if(_tmpB36 <= (void*)4)goto _LL78B;if(*((int*)_tmpB36)!= 5)goto
_LL78B;_LL78A: goto _LL78C;_LL78B: if((int)_tmpB36 != 1)goto _LL78D;_LL78C: goto _LL78E;
_LL78D: if(_tmpB36 <= (void*)4)goto _LL79D;if(*((int*)_tmpB36)!= 6)goto _LL78F;
_LL78E: return 1;_LL78F: if(*((int*)_tmpB36)!= 4)goto _LL791;_tmpB37=((struct Cyc_Absyn_PointerType_struct*)
_tmpB36)->f1;_tmpB38=_tmpB37.ptr_atts;_tmpB39=_tmpB38.nullable;_tmpB3A=_tmpB38.bounds;
_LL790: {void*_tmpB42=Cyc_Absyn_conref_def((void*)((void*)0),_tmpB3A);_LL7A0: if((
int)_tmpB42 != 0)goto _LL7A2;_LL7A1: return 1;_LL7A2:;_LL7A3: return((int(*)(int y,
union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(1,_tmpB39);_LL79F:;}_LL791:
if(*((int*)_tmpB36)!= 7)goto _LL793;_tmpB3B=((struct Cyc_Absyn_ArrayType_struct*)
_tmpB36)->f1;_tmpB3C=_tmpB3B.elt_type;_LL792: return Cyc_Tcutil_supports_default(
_tmpB3C);_LL793: if(*((int*)_tmpB36)!= 9)goto _LL795;_tmpB3D=((struct Cyc_Absyn_TupleType_struct*)
_tmpB36)->f1;_LL794: for(0;_tmpB3D != 0;_tmpB3D=_tmpB3D->tl){if(!Cyc_Tcutil_supports_default((*((
struct _tuple11*)_tmpB3D->hd)).f2))return 0;}return 1;_LL795: if(*((int*)_tmpB36)!= 
10)goto _LL797;_tmpB3E=((struct Cyc_Absyn_AggrType_struct*)_tmpB36)->f1;_tmpB3F=
_tmpB3E.aggr_info;_tmpB40=_tmpB3E.targs;_LL796: {struct Cyc_Absyn_Aggrdecl*
_tmpB43=Cyc_Absyn_get_known_aggrdecl(_tmpB3F);if(_tmpB43->impl == 0)return 0;if(((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpB43->impl))->exist_vars != 0)return
0;return Cyc_Tcutil_fields_support_default(_tmpB43->tvs,_tmpB40,((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmpB43->impl))->fields);}_LL797: if(*((int*)_tmpB36)!= 11)goto _LL799;
_tmpB41=((struct Cyc_Absyn_AnonAggrType_struct*)_tmpB36)->f2;_LL798: return Cyc_Tcutil_fields_support_default(
0,0,_tmpB41);_LL799: if(*((int*)_tmpB36)!= 13)goto _LL79B;_LL79A: goto _LL79C;_LL79B:
if(*((int*)_tmpB36)!= 12)goto _LL79D;_LL79C: return 1;_LL79D:;_LL79E: return 0;_LL786:;}
static int Cyc_Tcutil_fields_support_default(struct Cyc_List_List*tvs,struct Cyc_List_List*
ts,struct Cyc_List_List*fs){struct _RegionHandle _tmpB44=_new_region("rgn");struct
_RegionHandle*rgn=& _tmpB44;_push_region(rgn);{struct Cyc_List_List*_tmpB45=((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,tvs,ts);for(0;fs != 0;fs=fs->tl){
void*t=Cyc_Tcutil_rsubstitute(rgn,_tmpB45,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type);
if(!Cyc_Tcutil_supports_default(t)){int _tmpB46=0;_npop_handler(0);return _tmpB46;}}}{
int _tmpB47=1;_npop_handler(0);return _tmpB47;};_pop_region(rgn);}int Cyc_Tcutil_admits_zero(
void*t){void*_tmpB48=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmpB49;
struct Cyc_Absyn_PtrAtts _tmpB4A;union Cyc_Absyn_Constraint*_tmpB4B;union Cyc_Absyn_Constraint*
_tmpB4C;_LL7A5: if(_tmpB48 <= (void*)4)goto _LL7A7;if(*((int*)_tmpB48)!= 5)goto
_LL7A7;_LL7A6: goto _LL7A8;_LL7A7: if((int)_tmpB48 != 1)goto _LL7A9;_LL7A8: goto _LL7AA;
_LL7A9: if(_tmpB48 <= (void*)4)goto _LL7AD;if(*((int*)_tmpB48)!= 6)goto _LL7AB;
_LL7AA: return 1;_LL7AB: if(*((int*)_tmpB48)!= 4)goto _LL7AD;_tmpB49=((struct Cyc_Absyn_PointerType_struct*)
_tmpB48)->f1;_tmpB4A=_tmpB49.ptr_atts;_tmpB4B=_tmpB4A.nullable;_tmpB4C=_tmpB4A.bounds;
_LL7AC: {void*_tmpB4D=Cyc_Absyn_conref_def((void*)((void*)0),_tmpB4C);_LL7B0: if((
int)_tmpB4D != 0)goto _LL7B2;_LL7B1: return 0;_LL7B2:;_LL7B3: return((int(*)(int y,
union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpB4B);_LL7AF:;}_LL7AD:;
_LL7AE: return 0;_LL7A4:;}int Cyc_Tcutil_is_noreturn(void*t){{void*_tmpB4E=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_PtrInfo _tmpB4F;void*_tmpB50;struct Cyc_Absyn_FnInfo _tmpB51;
struct Cyc_List_List*_tmpB52;_LL7B5: if(_tmpB4E <= (void*)4)goto _LL7B9;if(*((int*)
_tmpB4E)!= 4)goto _LL7B7;_tmpB4F=((struct Cyc_Absyn_PointerType_struct*)_tmpB4E)->f1;
_tmpB50=_tmpB4F.elt_typ;_LL7B6: return Cyc_Tcutil_is_noreturn(_tmpB50);_LL7B7: if(*((
int*)_tmpB4E)!= 8)goto _LL7B9;_tmpB51=((struct Cyc_Absyn_FnType_struct*)_tmpB4E)->f1;
_tmpB52=_tmpB51.attributes;_LL7B8: for(0;_tmpB52 != 0;_tmpB52=_tmpB52->tl){void*
_tmpB53=(void*)_tmpB52->hd;_LL7BC: if((int)_tmpB53 != 3)goto _LL7BE;_LL7BD: return 1;
_LL7BE:;_LL7BF: continue;_LL7BB:;}goto _LL7B4;_LL7B9:;_LL7BA: goto _LL7B4;_LL7B4:;}
return 0;}struct Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(void*t,struct Cyc_List_List*
atts){void*_tmpB54=Cyc_Tcutil_compress(t);struct Cyc_Absyn_FnInfo _tmpB55;struct
Cyc_List_List*_tmpB56;struct Cyc_List_List**_tmpB57;_LL7C1: if(_tmpB54 <= (void*)4)
goto _LL7C3;if(*((int*)_tmpB54)!= 8)goto _LL7C3;_tmpB55=((struct Cyc_Absyn_FnType_struct*)
_tmpB54)->f1;_tmpB56=_tmpB55.attributes;_tmpB57=(struct Cyc_List_List**)&(((
struct Cyc_Absyn_FnType_struct*)_tmpB54)->f1).attributes;_LL7C2: {struct Cyc_List_List*
_tmpB58=0;for(0;atts != 0;atts=atts->tl){if(Cyc_Absyn_fntype_att((void*)atts->hd))*
_tmpB57=({struct Cyc_List_List*_tmpB59=_cycalloc(sizeof(*_tmpB59));_tmpB59->hd=(
void*)((void*)atts->hd);_tmpB59->tl=*_tmpB57;_tmpB59;});else{_tmpB58=({struct Cyc_List_List*
_tmpB5A=_cycalloc(sizeof(*_tmpB5A));_tmpB5A->hd=(void*)((void*)atts->hd);_tmpB5A->tl=
_tmpB58;_tmpB5A;});}}return _tmpB58;}_LL7C3:;_LL7C4:({void*_tmpB5B=0;((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmpB5C="transfer_fn_type_atts";_tag_dyneither(_tmpB5C,sizeof(char),22);}),
_tag_dyneither(_tmpB5B,sizeof(void*),0));});_LL7C0:;}
