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
struct Cyc_Absyn_Loc_n_struct{int tag;};struct Cyc_Absyn_Rel_n_struct{int tag;struct
Cyc_List_List*f1;};struct Cyc_Absyn_Abs_n_struct{int tag;struct Cyc_List_List*f1;};
union Cyc_Absyn_Nmspace_union{struct Cyc_Absyn_Loc_n_struct Loc_n;struct Cyc_Absyn_Rel_n_struct
Rel_n;struct Cyc_Absyn_Abs_n_struct Abs_n;};struct _tuple2{union Cyc_Absyn_Nmspace_union
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
Cyc_Absyn_UnknownTunionInfo{struct _tuple2*name;int is_xtunion;int is_flat;};struct
Cyc_Absyn_UnknownTunion_struct{int tag;struct Cyc_Absyn_UnknownTunionInfo f1;};
struct Cyc_Absyn_KnownTunion_struct{int tag;struct Cyc_Absyn_Tuniondecl**f1;};union
Cyc_Absyn_TunionInfoU_union{struct Cyc_Absyn_UnknownTunion_struct UnknownTunion;
struct Cyc_Absyn_KnownTunion_struct KnownTunion;};struct Cyc_Absyn_TunionInfo{union
Cyc_Absyn_TunionInfoU_union tunion_info;struct Cyc_List_List*targs;struct Cyc_Core_Opt*
rgn;};struct Cyc_Absyn_UnknownTunionFieldInfo{struct _tuple2*tunion_name;struct
_tuple2*field_name;int is_xtunion;};struct Cyc_Absyn_UnknownTunionfield_struct{int
tag;struct Cyc_Absyn_UnknownTunionFieldInfo f1;};struct Cyc_Absyn_KnownTunionfield_struct{
int tag;struct Cyc_Absyn_Tuniondecl*f1;struct Cyc_Absyn_Tunionfield*f2;};union Cyc_Absyn_TunionFieldInfoU_union{
struct Cyc_Absyn_UnknownTunionfield_struct UnknownTunionfield;struct Cyc_Absyn_KnownTunionfield_struct
KnownTunionfield;};struct Cyc_Absyn_TunionFieldInfo{union Cyc_Absyn_TunionFieldInfoU_union
field_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownAggr_struct{int tag;
void*f1;struct _tuple2*f2;};struct Cyc_Absyn_KnownAggr_struct{int tag;struct Cyc_Absyn_Aggrdecl**
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
int tag;struct _tuple2*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_struct{int tag;void*
f1;};struct Cyc_Absyn_DynRgnType_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_struct{
int tag;struct _tuple2*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;
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
_tuple2*f1;void*f2;};struct Cyc_Absyn_UnknownId_e_struct{int tag;struct _tuple2*f1;
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
f1;};struct _tuple3{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Tqual f2;void*f3;};
struct Cyc_Absyn_CompoundLit_e_struct{int tag;struct _tuple3*f1;struct Cyc_List_List*
f2;};struct Cyc_Absyn_Array_e_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;
int f4;};struct Cyc_Absyn_Struct_e_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*
f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_struct{
int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Tunion_e_struct{int tag;
struct Cyc_List_List*f1;struct Cyc_Absyn_Tuniondecl*f2;struct Cyc_Absyn_Tunionfield*
f3;};struct Cyc_Absyn_Enum_e_struct{int tag;struct _tuple2*f1;struct Cyc_Absyn_Enumdecl*
f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_struct{int tag;
struct _tuple2*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_struct{
int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_struct{int tag;
struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_struct{
int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_struct{
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Valueof_e_struct{int tag;void*f1;
};struct Cyc_Absyn_Exp{struct Cyc_Core_Opt*topt;void*r;struct Cyc_Position_Segment*
loc;void*annot;};struct Cyc_Absyn_Exp_s_struct{int tag;struct Cyc_Absyn_Exp*f1;};
struct Cyc_Absyn_Seq_s_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*
f2;};struct Cyc_Absyn_Return_s_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};
struct _tuple4{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_struct{
int tag;struct _tuple4 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_struct{
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_struct{int tag;struct
Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_struct{int tag;struct _dyneither_ptr*f1;
struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_struct{int tag;struct Cyc_Absyn_Exp*
f1;struct _tuple4 f2;struct _tuple4 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Fallthru_s_struct{
int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_struct{
int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_struct{
int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_struct{
int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple4 f2;};struct Cyc_Absyn_TryCatch_s_struct{
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
f2;};struct Cyc_Absyn_UnknownId_p_struct{int tag;struct _tuple2*f1;};struct Cyc_Absyn_UnknownCall_p_struct{
int tag;struct _tuple2*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;struct Cyc_Core_Opt*
topt;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*
pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*
body;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Global_b_struct{int tag;
struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_struct{int tag;struct Cyc_Absyn_Fndecl*
f1;};struct Cyc_Absyn_Param_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct
Cyc_Absyn_Local_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{void*sc;struct
_tuple2*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;
struct Cyc_Core_Opt*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{
void*sc;int is_inline;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*
effect;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*
cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;struct Cyc_Core_Opt*
cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;
struct Cyc_List_List*attributes;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*
name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*
attributes;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct
Cyc_List_List*rgn_po;struct Cyc_List_List*fields;};struct Cyc_Absyn_Aggrdecl{void*
kind;void*sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*
impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Tunionfield{struct _tuple2*
name;struct Cyc_List_List*typs;struct Cyc_Position_Segment*loc;void*sc;};struct Cyc_Absyn_Tuniondecl{
void*sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int
is_xtunion;int is_flat;};struct Cyc_Absyn_Enumfield{struct _tuple2*name;struct Cyc_Absyn_Exp*
tag;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Enumdecl{void*sc;struct
_tuple2*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct
_tuple2*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*
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
f2;};struct Cyc_Absyn_Using_d_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*
f2;};struct Cyc_Absyn_ExternC_d_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_struct{
int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Decl{void*
r;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_ArrayElement_struct{int tag;
struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_struct{int tag;struct
_dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];int Cyc_Absyn_qvar_cmp(
struct _tuple2*,struct _tuple2*);int Cyc_Absyn_tvar_cmp(struct Cyc_Absyn_Tvar*,
struct Cyc_Absyn_Tvar*);struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(struct Cyc_Position_Segment*);
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(struct Cyc_Position_Segment*);struct
Cyc_Absyn_Conref*Cyc_Absyn_new_conref(void*x);struct Cyc_Absyn_Conref*Cyc_Absyn_empty_conref();
struct Cyc_Absyn_Conref*Cyc_Absyn_compress_conref(struct Cyc_Absyn_Conref*x);void*
Cyc_Absyn_conref_val(struct Cyc_Absyn_Conref*x);void*Cyc_Absyn_conref_def(void*,
struct Cyc_Absyn_Conref*x);extern struct Cyc_Absyn_Conref*Cyc_Absyn_true_conref;
extern struct Cyc_Absyn_Conref*Cyc_Absyn_false_conref;extern struct Cyc_Absyn_Conref*
Cyc_Absyn_bounds_one_conref;void*Cyc_Absyn_compress_kb(void*);void*Cyc_Absyn_new_evar(
struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);extern void*Cyc_Absyn_uint_typ;
extern void*Cyc_Absyn_ulong_typ;extern void*Cyc_Absyn_ulonglong_typ;extern void*Cyc_Absyn_sint_typ;
extern void*Cyc_Absyn_slong_typ;extern void*Cyc_Absyn_slonglong_typ;extern void*Cyc_Absyn_empty_effect;
extern struct _tuple2*Cyc_Absyn_tunion_print_arg_qvar;extern struct _tuple2*Cyc_Absyn_tunion_scanf_arg_qvar;
extern void*Cyc_Absyn_bounds_one;void*Cyc_Absyn_atb_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual
tq,void*b,struct Cyc_Absyn_Conref*zero_term);struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(
struct Cyc_Absyn_Exp*);struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,struct
Cyc_Position_Segment*);struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct
Cyc_List_List*,struct _dyneither_ptr*);struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(
struct Cyc_Absyn_Aggrdecl*,struct _dyneither_ptr*);struct _tuple5{struct Cyc_Absyn_Tqual
f1;void*f2;};struct _tuple5*Cyc_Absyn_lookup_tuple_field(struct Cyc_List_List*,int);
struct _dyneither_ptr Cyc_Absyn_attribute2string(void*);int Cyc_Absyn_fntype_att(
void*a);struct _tuple6{void*f1;struct _tuple2*f2;};struct _tuple6 Cyc_Absyn_aggr_kinded_name(
union Cyc_Absyn_AggrInfoU_union);struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(
union Cyc_Absyn_AggrInfoU_union info);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct
Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs: 1;int qvar_to_Cids: 1;int
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
int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Tcenv_TunionRes_struct{int tag;
struct Cyc_Absyn_Tuniondecl*f1;struct Cyc_Absyn_Tunionfield*f2;};struct Cyc_Tcenv_EnumRes_struct{
int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcenv_AnonEnumRes_struct{
int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcenv_Genv{struct
_RegionHandle*grgn;struct Cyc_Set_Set*namespaces;struct Cyc_Dict_Dict aggrdecls;
struct Cyc_Dict_Dict tuniondecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict
typedefs;struct Cyc_Dict_Dict ordinaries;struct Cyc_List_List*availables;};struct
Cyc_Tcenv_Fenv;struct Cyc_Tcenv_Stmt_j_struct{int tag;struct Cyc_Absyn_Stmt*f1;};
struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Dict_Dict ae;struct Cyc_Tcenv_Fenv*
le;int allow_valueof;};struct _RegionHandle*Cyc_Tcenv_get_fnrgn(struct Cyc_Tcenv_Tenv*);
void*Cyc_Tcenv_lookup_ordinary(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,struct
Cyc_Position_Segment*,struct _tuple2*);struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl(
struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,struct _tuple2*);struct Cyc_Absyn_Tuniondecl**
Cyc_Tcenv_lookup_tuniondecl(struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,
struct _tuple2*);struct Cyc_Absyn_Enumdecl**Cyc_Tcenv_lookup_enumdecl(struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*,struct _tuple2*);struct Cyc_Absyn_Typedefdecl*Cyc_Tcenv_lookup_typedefdecl(
struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,struct _tuple2*);struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_allow_valueof(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*);struct Cyc_List_List*
Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*);struct Cyc_Core_Opt*Cyc_Tcenv_lookup_opt_type_vars(
struct Cyc_Tcenv_Tenv*te);int Cyc_Tcenv_region_outlives(struct Cyc_Tcenv_Tenv*,void*
r1,void*r2);unsigned int Cyc_strlen(struct _dyneither_ptr s);int Cyc_strcmp(struct
_dyneither_ptr s1,struct _dyneither_ptr s2);int Cyc_strptrcmp(struct _dyneither_ptr*
s1,struct _dyneither_ptr*s2);struct _dyneither_ptr Cyc_strconcat(struct
_dyneither_ptr,struct _dyneither_ptr);struct _tuple7{unsigned int f1;int f2;};struct
_tuple7 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);int Cyc_Evexp_same_const_exp(
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
void*k);void*Cyc_Tcutil_kind_to_bound(void*k);int Cyc_Tcutil_zero_to_null(struct
Cyc_Tcenv_Tenv*,void*t,struct Cyc_Absyn_Exp*e);void*Cyc_Tcutil_max_arithmetic_type(
void*,void*);void Cyc_Tcutil_explain_failure();int Cyc_Tcutil_unify(void*,void*);
int Cyc_Tcutil_typecmp(void*,void*);void*Cyc_Tcutil_substitute(struct Cyc_List_List*,
void*);void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,
void*);int Cyc_Tcutil_subset_effect(int may_constrain_evars,void*e1,void*e2);int
Cyc_Tcutil_region_in_effect(int constrain,void*r,void*e);void*Cyc_Tcutil_fndecl2typ(
struct Cyc_Absyn_Fndecl*);struct _tuple8{struct Cyc_Absyn_Tvar*f1;void*f2;};struct
_tuple8*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*,struct Cyc_Absyn_Tvar*);
struct _tuple9{struct Cyc_List_List*f1;struct _RegionHandle*f2;};struct _tuple8*Cyc_Tcutil_r_make_inst_var(
struct _tuple9*,struct Cyc_Absyn_Tvar*);void Cyc_Tcutil_check_bitfield(struct Cyc_Position_Segment*
loc,struct Cyc_Tcenv_Tenv*te,void*field_typ,struct Cyc_Absyn_Exp*width,struct
_dyneither_ptr*fn);void Cyc_Tcutil_check_contains_assign(struct Cyc_Absyn_Exp*);
void Cyc_Tcutil_check_valid_toplevel_type(struct Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*,
void*);void Cyc_Tcutil_check_fndecl_valid_type(struct Cyc_Position_Segment*,struct
Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Fndecl*);void Cyc_Tcutil_check_type(struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*bound_tvars,void*k,int allow_evars,void*);
void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*vs,struct Cyc_Position_Segment*
loc,struct _dyneither_ptr err_msg);void Cyc_Tcutil_check_unique_tvars(struct Cyc_Position_Segment*,
struct Cyc_List_List*);void Cyc_Tcutil_check_nonzero_bound(struct Cyc_Position_Segment*,
struct Cyc_Absyn_Conref*);void Cyc_Tcutil_check_bound(struct Cyc_Position_Segment*,
unsigned int i,struct Cyc_Absyn_Conref*);int Cyc_Tcutil_is_bound_one(struct Cyc_Absyn_Conref*
b);int Cyc_Tcutil_equal_tqual(struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2);
struct Cyc_List_List*Cyc_Tcutil_resolve_struct_designators(struct _RegionHandle*
rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,struct Cyc_List_List*
fields);int Cyc_Tcutil_is_tagged_pointer_typ(void*);int Cyc_Tcutil_is_tagged_pointer_typ_elt(
void*t,void**elt_typ_dest);int Cyc_Tcutil_is_zero_pointer_typ_elt(void*t,void**
elt_typ_dest);int Cyc_Tcutil_is_zero_ptr_type(void*t,void**ptr_type,int*
is_dyneither,void**elt_type);int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*
e1,void**ptr_type,int*is_dyneither,void**elt_type);int Cyc_Tcutil_is_noalias_pointer(
void*t);int Cyc_Tcutil_is_noalias_path(struct _RegionHandle*,struct Cyc_Absyn_Exp*e);
int Cyc_Tcutil_is_noalias_pointer_or_aggr(struct _RegionHandle*,void*t);void*Cyc_Tcutil_array_to_ptr(
struct Cyc_Tcenv_Tenv*,void*t,struct Cyc_Absyn_Exp*e);struct _tuple10{int f1;void*f2;
};struct _tuple10 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*
e);void*Cyc_Tcutil_normalize_effect(void*e);struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(
void*k);int Cyc_Tcutil_new_tvar_id();void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*);
void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*);int Cyc_Tcutil_is_temp_tvar(
struct Cyc_Absyn_Tvar*);void Cyc_Tcutil_rewrite_temp_tvar(struct Cyc_Absyn_Tvar*);
int Cyc_Tcutil_same_atts(struct Cyc_List_List*,struct Cyc_List_List*);int Cyc_Tcutil_bits_only(
void*t);int Cyc_Tcutil_is_const_exp(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e);
void*Cyc_Tcutil_snd_tqt(struct _tuple5*);int Cyc_Tcutil_supports_default(void*);
int Cyc_Tcutil_admits_zero(void*t);int Cyc_Tcutil_is_noreturn(void*);int Cyc_Tcutil_extract_const_from_typedef(
struct Cyc_Position_Segment*,int declared_const,void*);struct Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(
void*t,struct Cyc_List_List*atts);void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*,void**,
struct Cyc_Absyn_Exp*);void Cyc_Tc_tcAggrdecl(struct Cyc_Tcenv_Tenv*,struct Cyc_Tcenv_Genv*,
struct Cyc_Position_Segment*,struct Cyc_Absyn_Aggrdecl*);void Cyc_Tc_tcTuniondecl(
struct Cyc_Tcenv_Tenv*,struct Cyc_Tcenv_Genv*,struct Cyc_Position_Segment*,struct
Cyc_Absyn_Tuniondecl*);void Cyc_Tc_tcEnumdecl(struct Cyc_Tcenv_Tenv*,struct Cyc_Tcenv_Genv*,
struct Cyc_Position_Segment*,struct Cyc_Absyn_Enumdecl*);extern int Cyc_Cyclone_tovc_r;
char Cyc_Tcutil_Unify[10]="\000\000\000\000Unify\000";void Cyc_Tcutil_unify_it(
void*t1,void*t2);void*Cyc_Tcutil_t1_failure=(void*)0;int Cyc_Tcutil_tq1_const=0;
void*Cyc_Tcutil_t2_failure=(void*)0;int Cyc_Tcutil_tq2_const=0;struct
_dyneither_ptr Cyc_Tcutil_failure_reason=(struct _dyneither_ptr){(void*)0,(void*)0,(
void*)(0 + 0)};void Cyc_Tcutil_explain_failure(){if(Cyc_Position_num_errors >= Cyc_Position_max_errors)
return;Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);{struct _dyneither_ptr s1=(
struct _dyneither_ptr)({struct Cyc_String_pa_struct _tmp22;_tmp22.tag=0;_tmp22.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(Cyc_Tcutil_t1_failure));{
struct Cyc_String_pa_struct _tmp21;_tmp21.tag=0;_tmp21.f1=(struct _dyneither_ptr)(
Cyc_Tcutil_tq1_const?({const char*_tmp23="const ";_tag_dyneither(_tmp23,sizeof(
char),7);}):({const char*_tmp24="";_tag_dyneither(_tmp24,sizeof(char),1);}));{
void*_tmp1F[2]={& _tmp21,& _tmp22};Cyc_aprintf(({const char*_tmp20="%s%s";
_tag_dyneither(_tmp20,sizeof(char),5);}),_tag_dyneither(_tmp1F,sizeof(void*),2));}}});
struct _dyneither_ptr s2=(struct _dyneither_ptr)({struct Cyc_String_pa_struct _tmp1C;
_tmp1C.tag=0;_tmp1C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
Cyc_Tcutil_t2_failure));{struct Cyc_String_pa_struct _tmp1B;_tmp1B.tag=0;_tmp1B.f1=(
struct _dyneither_ptr)(Cyc_Tcutil_tq2_const?({const char*_tmp1D="const ";
_tag_dyneither(_tmp1D,sizeof(char),7);}):({const char*_tmp1E="";_tag_dyneither(
_tmp1E,sizeof(char),1);}));{void*_tmp19[2]={& _tmp1B,& _tmp1C};Cyc_aprintf(({const
char*_tmp1A="%s%s";_tag_dyneither(_tmp1A,sizeof(char),5);}),_tag_dyneither(
_tmp19,sizeof(void*),2));}}});int pos=2;({struct Cyc_String_pa_struct _tmp2;_tmp2.tag=
0;_tmp2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s1);{void*_tmp0[1]={&
_tmp2};Cyc_fprintf(Cyc_stderr,({const char*_tmp1="  %s";_tag_dyneither(_tmp1,
sizeof(char),5);}),_tag_dyneither(_tmp0,sizeof(void*),1));}});pos +=
_get_dyneither_size(s1,sizeof(char));if(pos + 5 >= 80){({void*_tmp3=0;Cyc_fprintf(
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
struct _dyneither_ptr)Cyc_Absynpp_kindbound2string((void*)((struct Cyc_Absyn_Tvar*)
tvs->hd)->kind));{struct Cyc_String_pa_struct _tmp2C;_tmp2C.tag=0;_tmp2C.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_tvar2string((struct Cyc_Absyn_Tvar*)
tvs->hd));{void*_tmp2A[2]={& _tmp2C,& _tmp2D};Cyc_fprintf(Cyc_stderr,({const char*
_tmp2B="%s::%s ";_tag_dyneither(_tmp2B,sizeof(char),8);}),_tag_dyneither(_tmp2A,
sizeof(void*),2));}}});}({void*_tmp2E=0;Cyc_fprintf(Cyc_stderr,({const char*
_tmp2F="\n";_tag_dyneither(_tmp2F,sizeof(char),2);}),_tag_dyneither(_tmp2E,
sizeof(void*),0));});Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);}static struct
Cyc_List_List*Cyc_Tcutil_warning_segs=0;static struct Cyc_List_List*Cyc_Tcutil_warning_msgs=
0;void Cyc_Tcutil_warn(struct Cyc_Position_Segment*sg,struct _dyneither_ptr fmt,
struct _dyneither_ptr ap){struct _dyneither_ptr msg=(struct _dyneither_ptr)Cyc_vrprintf(
Cyc_Core_heap_region,fmt,ap);Cyc_Tcutil_warning_segs=({struct Cyc_List_List*
_tmp30=_cycalloc(sizeof(*_tmp30));_tmp30->hd=sg;_tmp30->tl=Cyc_Tcutil_warning_segs;
_tmp30;});Cyc_Tcutil_warning_msgs=({struct Cyc_List_List*_tmp31=_cycalloc(sizeof(*
_tmp31));_tmp31->hd=({struct _dyneither_ptr*_tmp32=_cycalloc(sizeof(*_tmp32));
_tmp32[0]=msg;_tmp32;});_tmp31->tl=Cyc_Tcutil_warning_msgs;_tmp31;});}void Cyc_Tcutil_flush_warnings(){
if(Cyc_Tcutil_warning_segs == 0)return;({void*_tmp33=0;Cyc_fprintf(Cyc_stderr,({
const char*_tmp34="***Warnings***\n";_tag_dyneither(_tmp34,sizeof(char),16);}),
_tag_dyneither(_tmp33,sizeof(void*),0));});{struct Cyc_List_List*_tmp35=Cyc_Position_strings_of_segments(
Cyc_Tcutil_warning_segs);Cyc_Tcutil_warning_segs=0;Cyc_Tcutil_warning_msgs=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_Tcutil_warning_msgs);
while(Cyc_Tcutil_warning_msgs != 0){({struct Cyc_String_pa_struct _tmp39;_tmp39.tag=
0;_tmp39.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)((
struct Cyc_List_List*)_check_null(Cyc_Tcutil_warning_msgs))->hd));{struct Cyc_String_pa_struct
_tmp38;_tmp38.tag=0;_tmp38.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((
struct _dyneither_ptr*)((struct Cyc_List_List*)_check_null(_tmp35))->hd));{void*
_tmp36[2]={& _tmp38,& _tmp39};Cyc_fprintf(Cyc_stderr,({const char*_tmp37="%s: %s\n";
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
struct Cyc_List_List*ts){return Cyc_List_map(Cyc_Tcutil_copy_type,ts);}static
struct Cyc_Absyn_Conref*Cyc_Tcutil_copy_conref(struct Cyc_Absyn_Conref*c){union Cyc_Absyn_Constraint_union
_tmp45=c->v;void*_tmp46;struct Cyc_Absyn_Conref*_tmp47;_LLC: if((_tmp45.No_constr).tag
!= 2)goto _LLE;_LLD: return Cyc_Absyn_empty_conref();_LLE: if((_tmp45.Eq_constr).tag
!= 0)goto _LL10;_tmp46=(_tmp45.Eq_constr).f1;_LLF: return Cyc_Absyn_new_conref(
_tmp46);_LL10: if((_tmp45.Forward_constr).tag != 1)goto _LLB;_tmp47=(_tmp45.Forward_constr).f1;
_LL11: return Cyc_Tcutil_copy_conref(_tmp47);_LLB:;}static void*Cyc_Tcutil_copy_kindbound(
void*kb){void*_tmp48=Cyc_Absyn_compress_kb(kb);void*_tmp49;_LL13: if(*((int*)
_tmp48)!= 1)goto _LL15;_LL14: return(void*)({struct Cyc_Absyn_Unknown_kb_struct*
_tmp4A=_cycalloc(sizeof(*_tmp4A));_tmp4A[0]=({struct Cyc_Absyn_Unknown_kb_struct
_tmp4B;_tmp4B.tag=1;_tmp4B.f1=0;_tmp4B;});_tmp4A;});_LL15: if(*((int*)_tmp48)!= 2)
goto _LL17;_tmp49=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp48)->f2;_LL16:
return(void*)({struct Cyc_Absyn_Less_kb_struct*_tmp4C=_cycalloc(sizeof(*_tmp4C));
_tmp4C[0]=({struct Cyc_Absyn_Less_kb_struct _tmp4D;_tmp4D.tag=2;_tmp4D.f1=0;_tmp4D.f2=(
void*)_tmp49;_tmp4D;});_tmp4C;});_LL17:;_LL18: return kb;_LL12:;}static struct Cyc_Absyn_Tvar*
Cyc_Tcutil_copy_tvar(struct Cyc_Absyn_Tvar*tv){return({struct Cyc_Absyn_Tvar*
_tmp4E=_cycalloc(sizeof(*_tmp4E));_tmp4E->name=tv->name;_tmp4E->identity=- 1;
_tmp4E->kind=(void*)Cyc_Tcutil_copy_kindbound((void*)tv->kind);_tmp4E;});}static
struct _tuple3*Cyc_Tcutil_copy_arg(struct _tuple3*arg){struct _tuple3 _tmp50;struct
Cyc_Core_Opt*_tmp51;struct Cyc_Absyn_Tqual _tmp52;void*_tmp53;struct _tuple3*_tmp4F=
arg;_tmp50=*_tmp4F;_tmp51=_tmp50.f1;_tmp52=_tmp50.f2;_tmp53=_tmp50.f3;return({
struct _tuple3*_tmp54=_cycalloc(sizeof(*_tmp54));_tmp54->f1=_tmp51;_tmp54->f2=
_tmp52;_tmp54->f3=Cyc_Tcutil_copy_type(_tmp53);_tmp54;});}static struct _tuple5*
Cyc_Tcutil_copy_tqt(struct _tuple5*arg){struct _tuple5 _tmp56;struct Cyc_Absyn_Tqual
_tmp57;void*_tmp58;struct _tuple5*_tmp55=arg;_tmp56=*_tmp55;_tmp57=_tmp56.f1;
_tmp58=_tmp56.f2;return({struct _tuple5*_tmp59=_cycalloc(sizeof(*_tmp59));_tmp59->f1=
_tmp57;_tmp59->f2=Cyc_Tcutil_copy_type(_tmp58);_tmp59;});}static struct Cyc_Absyn_Aggrfield*
Cyc_Tcutil_copy_field(struct Cyc_Absyn_Aggrfield*f){return({struct Cyc_Absyn_Aggrfield*
_tmp5A=_cycalloc(sizeof(*_tmp5A));_tmp5A->name=f->name;_tmp5A->tq=f->tq;_tmp5A->type=(
void*)Cyc_Tcutil_copy_type((void*)f->type);_tmp5A->width=f->width;_tmp5A->attributes=
f->attributes;_tmp5A;});}static struct _tuple0*Cyc_Tcutil_copy_rgncmp(struct
_tuple0*x){struct _tuple0 _tmp5C;void*_tmp5D;void*_tmp5E;struct _tuple0*_tmp5B=x;
_tmp5C=*_tmp5B;_tmp5D=_tmp5C.f1;_tmp5E=_tmp5C.f2;return({struct _tuple0*_tmp5F=
_cycalloc(sizeof(*_tmp5F));_tmp5F->f1=Cyc_Tcutil_copy_type(_tmp5D);_tmp5F->f2=
Cyc_Tcutil_copy_type(_tmp5E);_tmp5F;});}static struct Cyc_Absyn_Enumfield*Cyc_Tcutil_copy_enumfield(
struct Cyc_Absyn_Enumfield*f){return({struct Cyc_Absyn_Enumfield*_tmp60=_cycalloc(
sizeof(*_tmp60));_tmp60->name=f->name;_tmp60->tag=f->tag;_tmp60->loc=f->loc;
_tmp60;});}void*Cyc_Tcutil_copy_type(void*t){void*_tmp61=Cyc_Tcutil_compress(t);
struct Cyc_Absyn_Tvar*_tmp62;struct Cyc_Absyn_TunionInfo _tmp63;union Cyc_Absyn_TunionInfoU_union
_tmp64;struct Cyc_List_List*_tmp65;struct Cyc_Core_Opt*_tmp66;struct Cyc_Absyn_TunionFieldInfo
_tmp67;union Cyc_Absyn_TunionFieldInfoU_union _tmp68;struct Cyc_List_List*_tmp69;
struct Cyc_Absyn_PtrInfo _tmp6A;void*_tmp6B;struct Cyc_Absyn_Tqual _tmp6C;struct Cyc_Absyn_PtrAtts
_tmp6D;void*_tmp6E;struct Cyc_Absyn_Conref*_tmp6F;struct Cyc_Absyn_Conref*_tmp70;
struct Cyc_Absyn_Conref*_tmp71;struct Cyc_Absyn_PtrLoc*_tmp72;struct Cyc_Absyn_ArrayInfo
_tmp73;void*_tmp74;struct Cyc_Absyn_Tqual _tmp75;struct Cyc_Absyn_Exp*_tmp76;struct
Cyc_Absyn_Conref*_tmp77;struct Cyc_Position_Segment*_tmp78;struct Cyc_Absyn_FnInfo
_tmp79;struct Cyc_List_List*_tmp7A;struct Cyc_Core_Opt*_tmp7B;void*_tmp7C;struct
Cyc_List_List*_tmp7D;int _tmp7E;struct Cyc_Absyn_VarargInfo*_tmp7F;struct Cyc_List_List*
_tmp80;struct Cyc_List_List*_tmp81;struct Cyc_List_List*_tmp82;struct Cyc_Absyn_AggrInfo
_tmp83;union Cyc_Absyn_AggrInfoU_union _tmp84;void*_tmp85;struct _tuple2*_tmp86;
struct Cyc_List_List*_tmp87;struct Cyc_Absyn_AggrInfo _tmp88;union Cyc_Absyn_AggrInfoU_union
_tmp89;struct Cyc_Absyn_Aggrdecl**_tmp8A;struct Cyc_List_List*_tmp8B;void*_tmp8C;
struct Cyc_List_List*_tmp8D;struct _tuple2*_tmp8E;struct Cyc_Absyn_Enumdecl*_tmp8F;
struct Cyc_List_List*_tmp90;void*_tmp91;struct Cyc_Absyn_Exp*_tmp92;void*_tmp93;
void*_tmp94;void*_tmp95;struct _tuple2*_tmp96;struct Cyc_List_List*_tmp97;struct
Cyc_Absyn_Typedefdecl*_tmp98;void*_tmp99;struct Cyc_List_List*_tmp9A;void*_tmp9B;
_LL1A: if((int)_tmp61 != 0)goto _LL1C;_LL1B: goto _LL1D;_LL1C: if(_tmp61 <= (void*)4)
goto _LL28;if(*((int*)_tmp61)!= 0)goto _LL1E;_LL1D: return t;_LL1E: if(*((int*)_tmp61)
!= 1)goto _LL20;_tmp62=((struct Cyc_Absyn_VarType_struct*)_tmp61)->f1;_LL1F: return(
void*)({struct Cyc_Absyn_VarType_struct*_tmp9C=_cycalloc(sizeof(*_tmp9C));_tmp9C[
0]=({struct Cyc_Absyn_VarType_struct _tmp9D;_tmp9D.tag=1;_tmp9D.f1=Cyc_Tcutil_copy_tvar(
_tmp62);_tmp9D;});_tmp9C;});_LL20: if(*((int*)_tmp61)!= 2)goto _LL22;_tmp63=((
struct Cyc_Absyn_TunionType_struct*)_tmp61)->f1;_tmp64=_tmp63.tunion_info;_tmp65=
_tmp63.targs;_tmp66=_tmp63.rgn;_LL21: {struct Cyc_Core_Opt*_tmp9E=(unsigned int)
_tmp66?({struct Cyc_Core_Opt*_tmpA2=_cycalloc(sizeof(*_tmpA2));_tmpA2->v=(void*)
Cyc_Tcutil_copy_type((void*)_tmp66->v);_tmpA2;}): 0;return(void*)({struct Cyc_Absyn_TunionType_struct*
_tmp9F=_cycalloc(sizeof(*_tmp9F));_tmp9F[0]=({struct Cyc_Absyn_TunionType_struct
_tmpA0;_tmpA0.tag=2;_tmpA0.f1=({struct Cyc_Absyn_TunionInfo _tmpA1;_tmpA1.tunion_info=
_tmp64;_tmpA1.targs=Cyc_Tcutil_copy_types(_tmp65);_tmpA1.rgn=_tmp9E;_tmpA1;});
_tmpA0;});_tmp9F;});}_LL22: if(*((int*)_tmp61)!= 3)goto _LL24;_tmp67=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp61)->f1;_tmp68=_tmp67.field_info;_tmp69=_tmp67.targs;_LL23: return(void*)({
struct Cyc_Absyn_TunionFieldType_struct*_tmpA3=_cycalloc(sizeof(*_tmpA3));_tmpA3[
0]=({struct Cyc_Absyn_TunionFieldType_struct _tmpA4;_tmpA4.tag=3;_tmpA4.f1=({
struct Cyc_Absyn_TunionFieldInfo _tmpA5;_tmpA5.field_info=_tmp68;_tmpA5.targs=Cyc_Tcutil_copy_types(
_tmp69);_tmpA5;});_tmpA4;});_tmpA3;});_LL24: if(*((int*)_tmp61)!= 4)goto _LL26;
_tmp6A=((struct Cyc_Absyn_PointerType_struct*)_tmp61)->f1;_tmp6B=(void*)_tmp6A.elt_typ;
_tmp6C=_tmp6A.elt_tq;_tmp6D=_tmp6A.ptr_atts;_tmp6E=(void*)_tmp6D.rgn;_tmp6F=
_tmp6D.nullable;_tmp70=_tmp6D.bounds;_tmp71=_tmp6D.zero_term;_tmp72=_tmp6D.ptrloc;
_LL25: {void*_tmpA6=Cyc_Tcutil_copy_type(_tmp6B);void*_tmpA7=Cyc_Tcutil_copy_type(
_tmp6E);struct Cyc_Absyn_Conref*_tmpA8=((struct Cyc_Absyn_Conref*(*)(struct Cyc_Absyn_Conref*
c))Cyc_Tcutil_copy_conref)(_tmp6F);struct Cyc_Absyn_Tqual _tmpA9=_tmp6C;struct Cyc_Absyn_Conref*
_tmpAA=Cyc_Tcutil_copy_conref(_tmp70);struct Cyc_Absyn_Conref*_tmpAB=((struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref*c))Cyc_Tcutil_copy_conref)(_tmp71);return(void*)({struct
Cyc_Absyn_PointerType_struct*_tmpAC=_cycalloc(sizeof(*_tmpAC));_tmpAC[0]=({
struct Cyc_Absyn_PointerType_struct _tmpAD;_tmpAD.tag=4;_tmpAD.f1=({struct Cyc_Absyn_PtrInfo
_tmpAE;_tmpAE.elt_typ=(void*)_tmpA6;_tmpAE.elt_tq=_tmpA9;_tmpAE.ptr_atts=({
struct Cyc_Absyn_PtrAtts _tmpAF;_tmpAF.rgn=(void*)_tmpA7;_tmpAF.nullable=_tmpA8;
_tmpAF.bounds=_tmpAA;_tmpAF.zero_term=_tmpAB;_tmpAF.ptrloc=_tmp72;_tmpAF;});
_tmpAE;});_tmpAD;});_tmpAC;});}_LL26: if(*((int*)_tmp61)!= 5)goto _LL28;_LL27: goto
_LL29;_LL28: if((int)_tmp61 != 1)goto _LL2A;_LL29: goto _LL2B;_LL2A: if(_tmp61 <= (void*)
4)goto _LL46;if(*((int*)_tmp61)!= 6)goto _LL2C;_LL2B: return t;_LL2C: if(*((int*)
_tmp61)!= 7)goto _LL2E;_tmp73=((struct Cyc_Absyn_ArrayType_struct*)_tmp61)->f1;
_tmp74=(void*)_tmp73.elt_type;_tmp75=_tmp73.tq;_tmp76=_tmp73.num_elts;_tmp77=
_tmp73.zero_term;_tmp78=_tmp73.zt_loc;_LL2D: return(void*)({struct Cyc_Absyn_ArrayType_struct*
_tmpB0=_cycalloc(sizeof(*_tmpB0));_tmpB0[0]=({struct Cyc_Absyn_ArrayType_struct
_tmpB1;_tmpB1.tag=7;_tmpB1.f1=({struct Cyc_Absyn_ArrayInfo _tmpB2;_tmpB2.elt_type=(
void*)Cyc_Tcutil_copy_type(_tmp74);_tmpB2.tq=_tmp75;_tmpB2.num_elts=_tmp76;
_tmpB2.zero_term=((struct Cyc_Absyn_Conref*(*)(struct Cyc_Absyn_Conref*c))Cyc_Tcutil_copy_conref)(
_tmp77);_tmpB2.zt_loc=_tmp78;_tmpB2;});_tmpB1;});_tmpB0;});_LL2E: if(*((int*)
_tmp61)!= 8)goto _LL30;_tmp79=((struct Cyc_Absyn_FnType_struct*)_tmp61)->f1;_tmp7A=
_tmp79.tvars;_tmp7B=_tmp79.effect;_tmp7C=(void*)_tmp79.ret_typ;_tmp7D=_tmp79.args;
_tmp7E=_tmp79.c_varargs;_tmp7F=_tmp79.cyc_varargs;_tmp80=_tmp79.rgn_po;_tmp81=
_tmp79.attributes;_LL2F: {struct Cyc_List_List*_tmpB3=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Tcutil_copy_tvar,_tmp7A);struct Cyc_Core_Opt*_tmpB4=_tmp7B == 0?0:({struct Cyc_Core_Opt*
_tmpBE=_cycalloc(sizeof(*_tmpBE));_tmpBE->v=(void*)Cyc_Tcutil_copy_type((void*)
_tmp7B->v);_tmpBE;});void*_tmpB5=Cyc_Tcutil_copy_type(_tmp7C);struct Cyc_List_List*
_tmpB6=((struct Cyc_List_List*(*)(struct _tuple3*(*f)(struct _tuple3*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Tcutil_copy_arg,_tmp7D);int _tmpB7=_tmp7E;struct Cyc_Absyn_VarargInfo*
cyc_varargs2=0;if(_tmp7F != 0){struct Cyc_Absyn_VarargInfo*cv=(struct Cyc_Absyn_VarargInfo*)
_tmp7F;cyc_varargs2=({struct Cyc_Absyn_VarargInfo*_tmpB8=_cycalloc(sizeof(*_tmpB8));
_tmpB8->name=cv->name;_tmpB8->tq=cv->tq;_tmpB8->type=(void*)Cyc_Tcutil_copy_type((
void*)cv->type);_tmpB8->inject=cv->inject;_tmpB8;});}{struct Cyc_List_List*_tmpB9=((
struct Cyc_List_List*(*)(struct _tuple0*(*f)(struct _tuple0*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Tcutil_copy_rgncmp,_tmp80);struct Cyc_List_List*_tmpBA=_tmp81;
return(void*)({struct Cyc_Absyn_FnType_struct*_tmpBB=_cycalloc(sizeof(*_tmpBB));
_tmpBB[0]=({struct Cyc_Absyn_FnType_struct _tmpBC;_tmpBC.tag=8;_tmpBC.f1=({struct
Cyc_Absyn_FnInfo _tmpBD;_tmpBD.tvars=_tmpB3;_tmpBD.effect=_tmpB4;_tmpBD.ret_typ=(
void*)_tmpB5;_tmpBD.args=_tmpB6;_tmpBD.c_varargs=_tmpB7;_tmpBD.cyc_varargs=
cyc_varargs2;_tmpBD.rgn_po=_tmpB9;_tmpBD.attributes=_tmpBA;_tmpBD;});_tmpBC;});
_tmpBB;});}}_LL30: if(*((int*)_tmp61)!= 9)goto _LL32;_tmp82=((struct Cyc_Absyn_TupleType_struct*)
_tmp61)->f1;_LL31: return(void*)({struct Cyc_Absyn_TupleType_struct*_tmpBF=
_cycalloc(sizeof(*_tmpBF));_tmpBF[0]=({struct Cyc_Absyn_TupleType_struct _tmpC0;
_tmpC0.tag=9;_tmpC0.f1=((struct Cyc_List_List*(*)(struct _tuple5*(*f)(struct
_tuple5*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_tqt,_tmp82);
_tmpC0;});_tmpBF;});_LL32: if(*((int*)_tmp61)!= 10)goto _LL34;_tmp83=((struct Cyc_Absyn_AggrType_struct*)
_tmp61)->f1;_tmp84=_tmp83.aggr_info;if((((((struct Cyc_Absyn_AggrType_struct*)
_tmp61)->f1).aggr_info).UnknownAggr).tag != 0)goto _LL34;_tmp85=(_tmp84.UnknownAggr).f1;
_tmp86=(_tmp84.UnknownAggr).f2;_tmp87=_tmp83.targs;_LL33: return(void*)({struct
Cyc_Absyn_AggrType_struct*_tmpC1=_cycalloc(sizeof(*_tmpC1));_tmpC1[0]=({struct
Cyc_Absyn_AggrType_struct _tmpC2;_tmpC2.tag=10;_tmpC2.f1=({struct Cyc_Absyn_AggrInfo
_tmpC3;_tmpC3.aggr_info=(union Cyc_Absyn_AggrInfoU_union)({union Cyc_Absyn_AggrInfoU_union
_tmpC4;(_tmpC4.UnknownAggr).tag=0;(_tmpC4.UnknownAggr).f1=(void*)_tmp85;(_tmpC4.UnknownAggr).f2=
_tmp86;_tmpC4;});_tmpC3.targs=Cyc_Tcutil_copy_types(_tmp87);_tmpC3;});_tmpC2;});
_tmpC1;});_LL34: if(*((int*)_tmp61)!= 10)goto _LL36;_tmp88=((struct Cyc_Absyn_AggrType_struct*)
_tmp61)->f1;_tmp89=_tmp88.aggr_info;if((((((struct Cyc_Absyn_AggrType_struct*)
_tmp61)->f1).aggr_info).KnownAggr).tag != 1)goto _LL36;_tmp8A=(_tmp89.KnownAggr).f1;
_tmp8B=_tmp88.targs;_LL35: return(void*)({struct Cyc_Absyn_AggrType_struct*_tmpC5=
_cycalloc(sizeof(*_tmpC5));_tmpC5[0]=({struct Cyc_Absyn_AggrType_struct _tmpC6;
_tmpC6.tag=10;_tmpC6.f1=({struct Cyc_Absyn_AggrInfo _tmpC7;_tmpC7.aggr_info=(union
Cyc_Absyn_AggrInfoU_union)({union Cyc_Absyn_AggrInfoU_union _tmpC8;(_tmpC8.KnownAggr).tag=
1;(_tmpC8.KnownAggr).f1=_tmp8A;_tmpC8;});_tmpC7.targs=Cyc_Tcutil_copy_types(
_tmp8B);_tmpC7;});_tmpC6;});_tmpC5;});_LL36: if(*((int*)_tmp61)!= 11)goto _LL38;
_tmp8C=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)_tmp61)->f1;_tmp8D=((struct
Cyc_Absyn_AnonAggrType_struct*)_tmp61)->f2;_LL37: return(void*)({struct Cyc_Absyn_AnonAggrType_struct*
_tmpC9=_cycalloc(sizeof(*_tmpC9));_tmpC9[0]=({struct Cyc_Absyn_AnonAggrType_struct
_tmpCA;_tmpCA.tag=11;_tmpCA.f1=(void*)_tmp8C;_tmpCA.f2=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Tcutil_copy_field,_tmp8D);_tmpCA;});_tmpC9;});_LL38: if(*((int*)
_tmp61)!= 12)goto _LL3A;_tmp8E=((struct Cyc_Absyn_EnumType_struct*)_tmp61)->f1;
_tmp8F=((struct Cyc_Absyn_EnumType_struct*)_tmp61)->f2;_LL39: return(void*)({
struct Cyc_Absyn_EnumType_struct*_tmpCB=_cycalloc(sizeof(*_tmpCB));_tmpCB[0]=({
struct Cyc_Absyn_EnumType_struct _tmpCC;_tmpCC.tag=12;_tmpCC.f1=_tmp8E;_tmpCC.f2=
_tmp8F;_tmpCC;});_tmpCB;});_LL3A: if(*((int*)_tmp61)!= 13)goto _LL3C;_tmp90=((
struct Cyc_Absyn_AnonEnumType_struct*)_tmp61)->f1;_LL3B: return(void*)({struct Cyc_Absyn_AnonEnumType_struct*
_tmpCD=_cycalloc(sizeof(*_tmpCD));_tmpCD[0]=({struct Cyc_Absyn_AnonEnumType_struct
_tmpCE;_tmpCE.tag=13;_tmpCE.f1=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Enumfield*(*
f)(struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_enumfield,
_tmp90);_tmpCE;});_tmpCD;});_LL3C: if(*((int*)_tmp61)!= 18)goto _LL3E;_tmp91=(void*)((
struct Cyc_Absyn_TagType_struct*)_tmp61)->f1;_LL3D: return(void*)({struct Cyc_Absyn_TagType_struct*
_tmpCF=_cycalloc(sizeof(*_tmpCF));_tmpCF[0]=({struct Cyc_Absyn_TagType_struct
_tmpD0;_tmpD0.tag=18;_tmpD0.f1=(void*)Cyc_Tcutil_copy_type(_tmp91);_tmpD0;});
_tmpCF;});_LL3E: if(*((int*)_tmp61)!= 17)goto _LL40;_tmp92=((struct Cyc_Absyn_ValueofType_struct*)
_tmp61)->f1;_LL3F: return(void*)({struct Cyc_Absyn_ValueofType_struct*_tmpD1=
_cycalloc(sizeof(*_tmpD1));_tmpD1[0]=({struct Cyc_Absyn_ValueofType_struct _tmpD2;
_tmpD2.tag=17;_tmpD2.f1=_tmp92;_tmpD2;});_tmpD1;});_LL40: if(*((int*)_tmp61)!= 14)
goto _LL42;_tmp93=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)_tmp61)->f1;
_LL41: return(void*)({struct Cyc_Absyn_RgnHandleType_struct*_tmpD3=_cycalloc(
sizeof(*_tmpD3));_tmpD3[0]=({struct Cyc_Absyn_RgnHandleType_struct _tmpD4;_tmpD4.tag=
14;_tmpD4.f1=(void*)Cyc_Tcutil_copy_type(_tmp93);_tmpD4;});_tmpD3;});_LL42: if(*((
int*)_tmp61)!= 15)goto _LL44;_tmp94=(void*)((struct Cyc_Absyn_DynRgnType_struct*)
_tmp61)->f1;_tmp95=(void*)((struct Cyc_Absyn_DynRgnType_struct*)_tmp61)->f2;_LL43:
return(void*)({struct Cyc_Absyn_DynRgnType_struct*_tmpD5=_cycalloc(sizeof(*_tmpD5));
_tmpD5[0]=({struct Cyc_Absyn_DynRgnType_struct _tmpD6;_tmpD6.tag=15;_tmpD6.f1=(
void*)Cyc_Tcutil_copy_type(_tmp94);_tmpD6.f2=(void*)Cyc_Tcutil_copy_type(_tmp95);
_tmpD6;});_tmpD5;});_LL44: if(*((int*)_tmp61)!= 16)goto _LL46;_tmp96=((struct Cyc_Absyn_TypedefType_struct*)
_tmp61)->f1;_tmp97=((struct Cyc_Absyn_TypedefType_struct*)_tmp61)->f2;_tmp98=((
struct Cyc_Absyn_TypedefType_struct*)_tmp61)->f3;_LL45: return(void*)({struct Cyc_Absyn_TypedefType_struct*
_tmpD7=_cycalloc(sizeof(*_tmpD7));_tmpD7[0]=({struct Cyc_Absyn_TypedefType_struct
_tmpD8;_tmpD8.tag=16;_tmpD8.f1=_tmp96;_tmpD8.f2=Cyc_Tcutil_copy_types(_tmp97);
_tmpD8.f3=_tmp98;_tmpD8.f4=0;_tmpD8;});_tmpD7;});_LL46: if((int)_tmp61 != 3)goto
_LL48;_LL47: goto _LL49;_LL48: if((int)_tmp61 != 2)goto _LL4A;_LL49: return t;_LL4A: if(
_tmp61 <= (void*)4)goto _LL4C;if(*((int*)_tmp61)!= 19)goto _LL4C;_tmp99=(void*)((
struct Cyc_Absyn_AccessEff_struct*)_tmp61)->f1;_LL4B: return(void*)({struct Cyc_Absyn_AccessEff_struct*
_tmpD9=_cycalloc(sizeof(*_tmpD9));_tmpD9[0]=({struct Cyc_Absyn_AccessEff_struct
_tmpDA;_tmpDA.tag=19;_tmpDA.f1=(void*)Cyc_Tcutil_copy_type(_tmp99);_tmpDA;});
_tmpD9;});_LL4C: if(_tmp61 <= (void*)4)goto _LL4E;if(*((int*)_tmp61)!= 20)goto _LL4E;
_tmp9A=((struct Cyc_Absyn_JoinEff_struct*)_tmp61)->f1;_LL4D: return(void*)({struct
Cyc_Absyn_JoinEff_struct*_tmpDB=_cycalloc(sizeof(*_tmpDB));_tmpDB[0]=({struct Cyc_Absyn_JoinEff_struct
_tmpDC;_tmpDC.tag=20;_tmpDC.f1=Cyc_Tcutil_copy_types(_tmp9A);_tmpDC;});_tmpDB;});
_LL4E: if(_tmp61 <= (void*)4)goto _LL19;if(*((int*)_tmp61)!= 21)goto _LL19;_tmp9B=(
void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp61)->f1;_LL4F: return(void*)({struct
Cyc_Absyn_RgnsEff_struct*_tmpDD=_cycalloc(sizeof(*_tmpDD));_tmpDD[0]=({struct Cyc_Absyn_RgnsEff_struct
_tmpDE;_tmpDE.tag=21;_tmpDE.f1=(void*)Cyc_Tcutil_copy_type(_tmp9B);_tmpDE;});
_tmpDD;});_LL19:;}int Cyc_Tcutil_kind_leq(void*k1,void*k2){if(k1 == k2)return 1;{
struct _tuple0 _tmpE0=({struct _tuple0 _tmpDF;_tmpDF.f1=k1;_tmpDF.f2=k2;_tmpDF;});
void*_tmpE1;void*_tmpE2;void*_tmpE3;void*_tmpE4;void*_tmpE5;void*_tmpE6;void*
_tmpE7;void*_tmpE8;void*_tmpE9;void*_tmpEA;_LL51: _tmpE1=_tmpE0.f1;if((int)_tmpE1
!= 2)goto _LL53;_tmpE2=_tmpE0.f2;if((int)_tmpE2 != 1)goto _LL53;_LL52: goto _LL54;
_LL53: _tmpE3=_tmpE0.f1;if((int)_tmpE3 != 2)goto _LL55;_tmpE4=_tmpE0.f2;if((int)
_tmpE4 != 0)goto _LL55;_LL54: goto _LL56;_LL55: _tmpE5=_tmpE0.f1;if((int)_tmpE5 != 1)
goto _LL57;_tmpE6=_tmpE0.f2;if((int)_tmpE6 != 0)goto _LL57;_LL56: goto _LL58;_LL57:
_tmpE7=_tmpE0.f1;if((int)_tmpE7 != 3)goto _LL59;_tmpE8=_tmpE0.f2;if((int)_tmpE8 != 
5)goto _LL59;_LL58: goto _LL5A;_LL59: _tmpE9=_tmpE0.f1;if((int)_tmpE9 != 4)goto _LL5B;
_tmpEA=_tmpE0.f2;if((int)_tmpEA != 5)goto _LL5B;_LL5A: return 1;_LL5B:;_LL5C: return 0;
_LL50:;}}static int Cyc_Tcutil_is_region_kind(void*k){void*_tmpEB=k;_LL5E: if((int)
_tmpEB != 3)goto _LL60;_LL5F: goto _LL61;_LL60: if((int)_tmpEB != 5)goto _LL62;_LL61:
goto _LL63;_LL62: if((int)_tmpEB != 4)goto _LL64;_LL63: return 1;_LL64:;_LL65: return 0;
_LL5D:;}void*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*tv){void*_tmpEC=Cyc_Absyn_compress_kb((
void*)tv->kind);void*_tmpED;void*_tmpEE;_LL67: if(*((int*)_tmpEC)!= 0)goto _LL69;
_tmpED=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmpEC)->f1;_LL68: return _tmpED;
_LL69: if(*((int*)_tmpEC)!= 2)goto _LL6B;_tmpEE=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmpEC)->f2;_LL6A: return _tmpEE;_LL6B:;_LL6C:({void*_tmpEF=0;((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmpF0="kind not suitably constrained!";
_tag_dyneither(_tmpF0,sizeof(char),31);}),_tag_dyneither(_tmpEF,sizeof(void*),0));});
_LL66:;}void*Cyc_Tcutil_typ_kind(void*t){void*_tmpF1=Cyc_Tcutil_compress(t);
struct Cyc_Core_Opt*_tmpF2;struct Cyc_Absyn_Tvar*_tmpF3;void*_tmpF4;struct Cyc_Absyn_TunionInfo
_tmpF5;union Cyc_Absyn_TunionInfoU_union _tmpF6;struct Cyc_Absyn_UnknownTunionInfo
_tmpF7;struct Cyc_Absyn_TunionInfo _tmpF8;union Cyc_Absyn_TunionInfoU_union _tmpF9;
struct Cyc_Absyn_Tuniondecl**_tmpFA;struct Cyc_Absyn_Tuniondecl*_tmpFB;struct Cyc_Absyn_TunionFieldInfo
_tmpFC;union Cyc_Absyn_TunionFieldInfoU_union _tmpFD;struct Cyc_Absyn_Tuniondecl*
_tmpFE;struct Cyc_Absyn_Tunionfield*_tmpFF;struct Cyc_Absyn_TunionFieldInfo _tmp100;
union Cyc_Absyn_TunionFieldInfoU_union _tmp101;struct Cyc_Absyn_Enumdecl*_tmp102;
struct Cyc_Absyn_AggrInfo _tmp103;union Cyc_Absyn_AggrInfoU_union _tmp104;struct Cyc_Absyn_AggrInfo
_tmp105;union Cyc_Absyn_AggrInfoU_union _tmp106;struct Cyc_Absyn_Aggrdecl**_tmp107;
struct Cyc_Absyn_Aggrdecl*_tmp108;struct Cyc_Absyn_Aggrdecl _tmp109;struct Cyc_Absyn_AggrdeclImpl*
_tmp10A;struct Cyc_Absyn_Enumdecl*_tmp10B;struct Cyc_Absyn_PtrInfo _tmp10C;struct
Cyc_Absyn_Typedefdecl*_tmp10D;_LL6E: if(_tmpF1 <= (void*)4)goto _LL72;if(*((int*)
_tmpF1)!= 0)goto _LL70;_tmpF2=((struct Cyc_Absyn_Evar_struct*)_tmpF1)->f1;_LL6F:
return(void*)((struct Cyc_Core_Opt*)_check_null(_tmpF2))->v;_LL70: if(*((int*)
_tmpF1)!= 1)goto _LL72;_tmpF3=((struct Cyc_Absyn_VarType_struct*)_tmpF1)->f1;_LL71:
return Cyc_Tcutil_tvar_kind(_tmpF3);_LL72: if((int)_tmpF1 != 0)goto _LL74;_LL73:
return(void*)1;_LL74: if(_tmpF1 <= (void*)4)goto _LL76;if(*((int*)_tmpF1)!= 5)goto
_LL76;_tmpF4=(void*)((struct Cyc_Absyn_IntType_struct*)_tmpF1)->f2;_LL75: return(
_tmpF4 == (void*)((void*)2) || _tmpF4 == (void*)((void*)3))?(void*)2:(void*)1;
_LL76: if((int)_tmpF1 != 1)goto _LL78;_LL77: goto _LL79;_LL78: if(_tmpF1 <= (void*)4)
goto _LL80;if(*((int*)_tmpF1)!= 6)goto _LL7A;_LL79: goto _LL7B;_LL7A: if(*((int*)
_tmpF1)!= 8)goto _LL7C;_LL7B: return(void*)1;_LL7C: if(*((int*)_tmpF1)!= 15)goto
_LL7E;_LL7D: goto _LL7F;_LL7E: if(*((int*)_tmpF1)!= 14)goto _LL80;_LL7F: return(void*)
2;_LL80: if((int)_tmpF1 != 3)goto _LL82;_LL81: return(void*)4;_LL82: if((int)_tmpF1 != 
2)goto _LL84;_LL83: return(void*)3;_LL84: if(_tmpF1 <= (void*)4)goto _LL86;if(*((int*)
_tmpF1)!= 2)goto _LL86;_tmpF5=((struct Cyc_Absyn_TunionType_struct*)_tmpF1)->f1;
_tmpF6=_tmpF5.tunion_info;if((((((struct Cyc_Absyn_TunionType_struct*)_tmpF1)->f1).tunion_info).UnknownTunion).tag
!= 0)goto _LL86;_tmpF7=(_tmpF6.UnknownTunion).f1;_LL85: if(_tmpF7.is_flat)return(
void*)1;else{return(void*)2;}_LL86: if(_tmpF1 <= (void*)4)goto _LL88;if(*((int*)
_tmpF1)!= 2)goto _LL88;_tmpF8=((struct Cyc_Absyn_TunionType_struct*)_tmpF1)->f1;
_tmpF9=_tmpF8.tunion_info;if((((((struct Cyc_Absyn_TunionType_struct*)_tmpF1)->f1).tunion_info).KnownTunion).tag
!= 1)goto _LL88;_tmpFA=(_tmpF9.KnownTunion).f1;_tmpFB=*_tmpFA;_LL87: if(_tmpFB->is_flat)
return(void*)1;else{return(void*)2;}_LL88: if(_tmpF1 <= (void*)4)goto _LL8A;if(*((
int*)_tmpF1)!= 3)goto _LL8A;_tmpFC=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmpF1)->f1;_tmpFD=_tmpFC.field_info;if((((((struct Cyc_Absyn_TunionFieldType_struct*)
_tmpF1)->f1).field_info).KnownTunionfield).tag != 1)goto _LL8A;_tmpFE=(_tmpFD.KnownTunionfield).f1;
_tmpFF=(_tmpFD.KnownTunionfield).f2;_LL89: if(_tmpFE->is_flat)return(void*)1;
else{if(_tmpFF->typs == 0)return(void*)2;else{return(void*)1;}}_LL8A: if(_tmpF1 <= (
void*)4)goto _LL8C;if(*((int*)_tmpF1)!= 3)goto _LL8C;_tmp100=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmpF1)->f1;_tmp101=_tmp100.field_info;if((((((struct Cyc_Absyn_TunionFieldType_struct*)
_tmpF1)->f1).field_info).UnknownTunionfield).tag != 0)goto _LL8C;_LL8B:({void*
_tmp10E=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp10F="typ_kind: Unresolved TunionFieldType";_tag_dyneither(_tmp10F,
sizeof(char),37);}),_tag_dyneither(_tmp10E,sizeof(void*),0));});_LL8C: if(_tmpF1
<= (void*)4)goto _LL8E;if(*((int*)_tmpF1)!= 12)goto _LL8E;_tmp102=((struct Cyc_Absyn_EnumType_struct*)
_tmpF1)->f2;if(_tmp102 != 0)goto _LL8E;_LL8D: goto _LL8F;_LL8E: if(_tmpF1 <= (void*)4)
goto _LL90;if(*((int*)_tmpF1)!= 10)goto _LL90;_tmp103=((struct Cyc_Absyn_AggrType_struct*)
_tmpF1)->f1;_tmp104=_tmp103.aggr_info;if((((((struct Cyc_Absyn_AggrType_struct*)
_tmpF1)->f1).aggr_info).UnknownAggr).tag != 0)goto _LL90;_LL8F: return(void*)0;
_LL90: if(_tmpF1 <= (void*)4)goto _LL92;if(*((int*)_tmpF1)!= 10)goto _LL92;_tmp105=((
struct Cyc_Absyn_AggrType_struct*)_tmpF1)->f1;_tmp106=_tmp105.aggr_info;if((((((
struct Cyc_Absyn_AggrType_struct*)_tmpF1)->f1).aggr_info).KnownAggr).tag != 1)goto
_LL92;_tmp107=(_tmp106.KnownAggr).f1;_tmp108=*_tmp107;_tmp109=*_tmp108;_tmp10A=
_tmp109.impl;_LL91: return _tmp10A == 0?(void*)0:(void*)1;_LL92: if(_tmpF1 <= (void*)
4)goto _LL94;if(*((int*)_tmpF1)!= 11)goto _LL94;_LL93: goto _LL95;_LL94: if(_tmpF1 <= (
void*)4)goto _LL96;if(*((int*)_tmpF1)!= 13)goto _LL96;_LL95: return(void*)1;_LL96:
if(_tmpF1 <= (void*)4)goto _LL98;if(*((int*)_tmpF1)!= 12)goto _LL98;_tmp10B=((
struct Cyc_Absyn_EnumType_struct*)_tmpF1)->f2;_LL97: if(_tmp10B->fields == 0)return(
void*)0;else{return(void*)1;}_LL98: if(_tmpF1 <= (void*)4)goto _LL9A;if(*((int*)
_tmpF1)!= 4)goto _LL9A;_tmp10C=((struct Cyc_Absyn_PointerType_struct*)_tmpF1)->f1;
_LL99: {union Cyc_Absyn_Constraint_union _tmp110=(Cyc_Absyn_compress_conref((
_tmp10C.ptr_atts).bounds))->v;void*_tmp111;void*_tmp112;_LLAB: if((_tmp110.No_constr).tag
!= 2)goto _LLAD;_LLAC: goto _LLAE;_LLAD: if((_tmp110.Eq_constr).tag != 0)goto _LLAF;
_tmp111=(_tmp110.Eq_constr).f1;if((int)_tmp111 != 0)goto _LLAF;_LLAE: return(void*)
1;_LLAF: if((_tmp110.Eq_constr).tag != 0)goto _LLB1;_tmp112=(_tmp110.Eq_constr).f1;
if(_tmp112 <= (void*)1)goto _LLB1;if(*((int*)_tmp112)!= 0)goto _LLB1;_LLB0: return(
void*)2;_LLB1: if((_tmp110.Forward_constr).tag != 1)goto _LLAA;_LLB2:({void*_tmp113=
0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp114="typ_kind: forward constr in ptr bounds";_tag_dyneither(
_tmp114,sizeof(char),39);}),_tag_dyneither(_tmp113,sizeof(void*),0));});_LLAA:;}
_LL9A: if(_tmpF1 <= (void*)4)goto _LL9C;if(*((int*)_tmpF1)!= 17)goto _LL9C;_LL9B:
return(void*)7;_LL9C: if(_tmpF1 <= (void*)4)goto _LL9E;if(*((int*)_tmpF1)!= 18)goto
_LL9E;_LL9D: return(void*)2;_LL9E: if(_tmpF1 <= (void*)4)goto _LLA0;if(*((int*)
_tmpF1)!= 7)goto _LLA0;_LL9F: goto _LLA1;_LLA0: if(_tmpF1 <= (void*)4)goto _LLA2;if(*((
int*)_tmpF1)!= 9)goto _LLA2;_LLA1: return(void*)1;_LLA2: if(_tmpF1 <= (void*)4)goto
_LLA4;if(*((int*)_tmpF1)!= 16)goto _LLA4;_tmp10D=((struct Cyc_Absyn_TypedefType_struct*)
_tmpF1)->f3;_LLA3: if(_tmp10D == 0  || _tmp10D->kind == 0)({struct Cyc_String_pa_struct
_tmp117;_tmp117.tag=0;_tmp117.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t));{void*_tmp115[1]={& _tmp117};((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp116="typ_kind: typedef found: %s";
_tag_dyneither(_tmp116,sizeof(char),28);}),_tag_dyneither(_tmp115,sizeof(void*),
1));}});return(void*)((struct Cyc_Core_Opt*)_check_null(_tmp10D->kind))->v;_LLA4:
if(_tmpF1 <= (void*)4)goto _LLA6;if(*((int*)_tmpF1)!= 19)goto _LLA6;_LLA5: goto _LLA7;
_LLA6: if(_tmpF1 <= (void*)4)goto _LLA8;if(*((int*)_tmpF1)!= 20)goto _LLA8;_LLA7:
goto _LLA9;_LLA8: if(_tmpF1 <= (void*)4)goto _LL6D;if(*((int*)_tmpF1)!= 21)goto _LL6D;
_LLA9: return(void*)6;_LL6D:;}int Cyc_Tcutil_unify(void*t1,void*t2){struct
_handler_cons _tmp118;_push_handler(& _tmp118);{int _tmp11A=0;if(setjmp(_tmp118.handler))
_tmp11A=1;if(!_tmp11A){Cyc_Tcutil_unify_it(t1,t2);{int _tmp11B=1;_npop_handler(0);
return _tmp11B;};_pop_handler();}else{void*_tmp119=(void*)_exn_thrown;void*
_tmp11D=_tmp119;_LLB4: if(_tmp11D != Cyc_Tcutil_Unify)goto _LLB6;_LLB5: return 0;
_LLB6:;_LLB7:(void)_throw(_tmp11D);_LLB3:;}}}static void Cyc_Tcutil_occurslist(
void*evar,struct _RegionHandle*r,struct Cyc_List_List*env,struct Cyc_List_List*ts);
static void Cyc_Tcutil_occurs(void*evar,struct _RegionHandle*r,struct Cyc_List_List*
env,void*t){t=Cyc_Tcutil_compress(t);{void*_tmp11E=t;struct Cyc_Absyn_Tvar*
_tmp11F;struct Cyc_Core_Opt*_tmp120;struct Cyc_Core_Opt*_tmp121;struct Cyc_Core_Opt**
_tmp122;struct Cyc_Absyn_PtrInfo _tmp123;struct Cyc_Absyn_ArrayInfo _tmp124;void*
_tmp125;struct Cyc_Absyn_FnInfo _tmp126;struct Cyc_List_List*_tmp127;struct Cyc_Core_Opt*
_tmp128;void*_tmp129;struct Cyc_List_List*_tmp12A;int _tmp12B;struct Cyc_Absyn_VarargInfo*
_tmp12C;struct Cyc_List_List*_tmp12D;struct Cyc_List_List*_tmp12E;struct Cyc_List_List*
_tmp12F;struct Cyc_Absyn_TunionInfo _tmp130;struct Cyc_List_List*_tmp131;struct Cyc_Core_Opt*
_tmp132;struct Cyc_List_List*_tmp133;struct Cyc_Absyn_TunionFieldInfo _tmp134;
struct Cyc_List_List*_tmp135;struct Cyc_Absyn_AggrInfo _tmp136;struct Cyc_List_List*
_tmp137;struct Cyc_List_List*_tmp138;void*_tmp139;void*_tmp13A;void*_tmp13B;void*
_tmp13C;struct Cyc_List_List*_tmp13D;_LLB9: if(_tmp11E <= (void*)4)goto _LLD9;if(*((
int*)_tmp11E)!= 1)goto _LLBB;_tmp11F=((struct Cyc_Absyn_VarType_struct*)_tmp11E)->f1;
_LLBA: if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,
env,_tmp11F)){Cyc_Tcutil_failure_reason=({const char*_tmp13E="(type variable would escape scope)";
_tag_dyneither(_tmp13E,sizeof(char),35);});(int)_throw((void*)Cyc_Tcutil_Unify);}
goto _LLB8;_LLBB: if(*((int*)_tmp11E)!= 0)goto _LLBD;_tmp120=((struct Cyc_Absyn_Evar_struct*)
_tmp11E)->f2;_tmp121=((struct Cyc_Absyn_Evar_struct*)_tmp11E)->f4;_tmp122=(struct
Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)_tmp11E)->f4;_LLBC: if(t == evar){
Cyc_Tcutil_failure_reason=({const char*_tmp13F="(occurs check)";_tag_dyneither(
_tmp13F,sizeof(char),15);});(int)_throw((void*)Cyc_Tcutil_Unify);}else{if(
_tmp120 != 0)Cyc_Tcutil_occurs(evar,r,env,(void*)_tmp120->v);else{int problem=0;{
struct Cyc_List_List*s=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*
_tmp122))->v;for(0;s != 0;s=s->tl){if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,
struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(
Cyc_Tcutil_fast_tvar_cmp,env,(struct Cyc_Absyn_Tvar*)s->hd)){problem=1;break;}}}
if(problem){struct Cyc_List_List*_tmp140=0;{struct Cyc_List_List*s=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(*_tmp122))->v;for(0;s != 0;s=s->tl){if(((int(*)(
int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,
struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,(struct Cyc_Absyn_Tvar*)
s->hd))_tmp140=({struct Cyc_List_List*_tmp141=_cycalloc(sizeof(*_tmp141));_tmp141->hd=(
struct Cyc_Absyn_Tvar*)s->hd;_tmp141->tl=_tmp140;_tmp141;});}}*_tmp122=({struct
Cyc_Core_Opt*_tmp142=_cycalloc(sizeof(*_tmp142));_tmp142->v=_tmp140;_tmp142;});}}}
goto _LLB8;_LLBD: if(*((int*)_tmp11E)!= 4)goto _LLBF;_tmp123=((struct Cyc_Absyn_PointerType_struct*)
_tmp11E)->f1;_LLBE: Cyc_Tcutil_occurs(evar,r,env,(void*)_tmp123.elt_typ);Cyc_Tcutil_occurs(
evar,r,env,(void*)(_tmp123.ptr_atts).rgn);goto _LLB8;_LLBF: if(*((int*)_tmp11E)!= 
7)goto _LLC1;_tmp124=((struct Cyc_Absyn_ArrayType_struct*)_tmp11E)->f1;_tmp125=(
void*)_tmp124.elt_type;_LLC0: Cyc_Tcutil_occurs(evar,r,env,_tmp125);goto _LLB8;
_LLC1: if(*((int*)_tmp11E)!= 8)goto _LLC3;_tmp126=((struct Cyc_Absyn_FnType_struct*)
_tmp11E)->f1;_tmp127=_tmp126.tvars;_tmp128=_tmp126.effect;_tmp129=(void*)_tmp126.ret_typ;
_tmp12A=_tmp126.args;_tmp12B=_tmp126.c_varargs;_tmp12C=_tmp126.cyc_varargs;
_tmp12D=_tmp126.rgn_po;_tmp12E=_tmp126.attributes;_LLC2: env=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(
r,_tmp127,env);if(_tmp128 != 0)Cyc_Tcutil_occurs(evar,r,env,(void*)_tmp128->v);
Cyc_Tcutil_occurs(evar,r,env,_tmp129);for(0;_tmp12A != 0;_tmp12A=_tmp12A->tl){Cyc_Tcutil_occurs(
evar,r,env,(*((struct _tuple3*)_tmp12A->hd)).f3);}if(_tmp12C != 0)Cyc_Tcutil_occurs(
evar,r,env,(void*)_tmp12C->type);for(0;_tmp12D != 0;_tmp12D=_tmp12D->tl){struct
_tuple0 _tmp144;void*_tmp145;void*_tmp146;struct _tuple0*_tmp143=(struct _tuple0*)
_tmp12D->hd;_tmp144=*_tmp143;_tmp145=_tmp144.f1;_tmp146=_tmp144.f2;Cyc_Tcutil_occurs(
evar,r,env,_tmp145);Cyc_Tcutil_occurs(evar,r,env,_tmp146);}goto _LLB8;_LLC3: if(*((
int*)_tmp11E)!= 9)goto _LLC5;_tmp12F=((struct Cyc_Absyn_TupleType_struct*)_tmp11E)->f1;
_LLC4: for(0;_tmp12F != 0;_tmp12F=_tmp12F->tl){Cyc_Tcutil_occurs(evar,r,env,(*((
struct _tuple5*)_tmp12F->hd)).f2);}goto _LLB8;_LLC5: if(*((int*)_tmp11E)!= 2)goto
_LLC7;_tmp130=((struct Cyc_Absyn_TunionType_struct*)_tmp11E)->f1;_tmp131=_tmp130.targs;
_tmp132=_tmp130.rgn;_LLC6: if((unsigned int)_tmp132)Cyc_Tcutil_occurs(evar,r,env,(
void*)_tmp132->v);Cyc_Tcutil_occurslist(evar,r,env,_tmp131);goto _LLB8;_LLC7: if(*((
int*)_tmp11E)!= 16)goto _LLC9;_tmp133=((struct Cyc_Absyn_TypedefType_struct*)
_tmp11E)->f2;_LLC8: _tmp135=_tmp133;goto _LLCA;_LLC9: if(*((int*)_tmp11E)!= 3)goto
_LLCB;_tmp134=((struct Cyc_Absyn_TunionFieldType_struct*)_tmp11E)->f1;_tmp135=
_tmp134.targs;_LLCA: _tmp137=_tmp135;goto _LLCC;_LLCB: if(*((int*)_tmp11E)!= 10)
goto _LLCD;_tmp136=((struct Cyc_Absyn_AggrType_struct*)_tmp11E)->f1;_tmp137=
_tmp136.targs;_LLCC: Cyc_Tcutil_occurslist(evar,r,env,_tmp137);goto _LLB8;_LLCD:
if(*((int*)_tmp11E)!= 11)goto _LLCF;_tmp138=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp11E)->f2;_LLCE: for(0;_tmp138 != 0;_tmp138=_tmp138->tl){Cyc_Tcutil_occurs(evar,
r,env,(void*)((struct Cyc_Absyn_Aggrfield*)_tmp138->hd)->type);}goto _LLB8;_LLCF:
if(*((int*)_tmp11E)!= 18)goto _LLD1;_tmp139=(void*)((struct Cyc_Absyn_TagType_struct*)
_tmp11E)->f1;_LLD0: _tmp13A=_tmp139;goto _LLD2;_LLD1: if(*((int*)_tmp11E)!= 19)goto
_LLD3;_tmp13A=(void*)((struct Cyc_Absyn_AccessEff_struct*)_tmp11E)->f1;_LLD2:
_tmp13B=_tmp13A;goto _LLD4;_LLD3: if(*((int*)_tmp11E)!= 14)goto _LLD5;_tmp13B=(void*)((
struct Cyc_Absyn_RgnHandleType_struct*)_tmp11E)->f1;_LLD4: _tmp13C=_tmp13B;goto
_LLD6;_LLD5: if(*((int*)_tmp11E)!= 21)goto _LLD7;_tmp13C=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp11E)->f1;_LLD6: Cyc_Tcutil_occurs(evar,r,env,_tmp13C);goto _LLB8;_LLD7: if(*((
int*)_tmp11E)!= 20)goto _LLD9;_tmp13D=((struct Cyc_Absyn_JoinEff_struct*)_tmp11E)->f1;
_LLD8: Cyc_Tcutil_occurslist(evar,r,env,_tmp13D);goto _LLB8;_LLD9:;_LLDA: goto _LLB8;
_LLB8:;}}static void Cyc_Tcutil_occurslist(void*evar,struct _RegionHandle*r,struct
Cyc_List_List*env,struct Cyc_List_List*ts){for(0;ts != 0;ts=ts->tl){Cyc_Tcutil_occurs(
evar,r,env,(void*)ts->hd);}}static void Cyc_Tcutil_unify_list(struct Cyc_List_List*
t1,struct Cyc_List_List*t2){for(0;t1 != 0  && t2 != 0;(t1=t1->tl,t2=t2->tl)){Cyc_Tcutil_unify_it((
void*)t1->hd,(void*)t2->hd);}if(t1 != 0  || t2 != 0)(int)_throw((void*)Cyc_Tcutil_Unify);}
static void Cyc_Tcutil_unify_tqual(struct Cyc_Absyn_Tqual tq1,void*t1,struct Cyc_Absyn_Tqual
tq2,void*t2){if(tq1.print_const  && !tq1.real_const)({void*_tmp147=0;((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmp148="tq1 real_const not set.";_tag_dyneither(_tmp148,sizeof(char),24);}),
_tag_dyneither(_tmp147,sizeof(void*),0));});if(tq2.print_const  && !tq2.real_const)({
void*_tmp149=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp14A="tq2 real_const not set.";_tag_dyneither(_tmp14A,sizeof(char),
24);}),_tag_dyneither(_tmp149,sizeof(void*),0));});if((tq1.real_const != tq2.real_const
 || tq1.q_volatile != tq2.q_volatile) || tq1.q_restrict != tq2.q_restrict){Cyc_Tcutil_t1_failure=
t1;Cyc_Tcutil_t2_failure=t2;Cyc_Tcutil_tq1_const=tq1.real_const;Cyc_Tcutil_tq2_const=
tq2.real_const;Cyc_Tcutil_failure_reason=({const char*_tmp14B="(qualifiers don't match)";
_tag_dyneither(_tmp14B,sizeof(char),25);});(int)_throw((void*)Cyc_Tcutil_Unify);}
Cyc_Tcutil_tq1_const=0;Cyc_Tcutil_tq2_const=0;}int Cyc_Tcutil_equal_tqual(struct
Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2){return(tq1.real_const == tq2.real_const
 && tq1.q_volatile == tq2.q_volatile) && tq1.q_restrict == tq2.q_restrict;}static
void Cyc_Tcutil_unify_it_conrefs(int(*cmp)(void*,void*),struct Cyc_Absyn_Conref*x,
struct Cyc_Absyn_Conref*y,struct _dyneither_ptr reason){x=Cyc_Absyn_compress_conref(
x);y=Cyc_Absyn_compress_conref(y);if(x == y)return;{union Cyc_Absyn_Constraint_union
_tmp14C=x->v;void*_tmp14D;_LLDC: if((_tmp14C.No_constr).tag != 2)goto _LLDE;_LLDD: x->v=(
union Cyc_Absyn_Constraint_union)({union Cyc_Absyn_Constraint_union _tmp14E;(
_tmp14E.Forward_constr).tag=1;(_tmp14E.Forward_constr).f1=y;_tmp14E;});return;
_LLDE: if((_tmp14C.Eq_constr).tag != 0)goto _LLE0;_tmp14D=(_tmp14C.Eq_constr).f1;
_LLDF: {union Cyc_Absyn_Constraint_union _tmp14F=y->v;void*_tmp150;_LLE3: if((
_tmp14F.No_constr).tag != 2)goto _LLE5;_LLE4: y->v=x->v;return;_LLE5: if((_tmp14F.Eq_constr).tag
!= 0)goto _LLE7;_tmp150=(_tmp14F.Eq_constr).f1;_LLE6: if(cmp(_tmp14D,_tmp150)!= 0){
Cyc_Tcutil_failure_reason=reason;(int)_throw((void*)Cyc_Tcutil_Unify);}return;
_LLE7: if((_tmp14F.Forward_constr).tag != 1)goto _LLE2;_LLE8:({void*_tmp151=0;Cyc_Tcutil_impos(({
const char*_tmp152="unify_conref: forward after compress(2)";_tag_dyneither(
_tmp152,sizeof(char),40);}),_tag_dyneither(_tmp151,sizeof(void*),0));});_LLE2:;}
_LLE0: if((_tmp14C.Forward_constr).tag != 1)goto _LLDB;_LLE1:({void*_tmp153=0;Cyc_Tcutil_impos(({
const char*_tmp154="unify_conref: forward after compress";_tag_dyneither(_tmp154,
sizeof(char),37);}),_tag_dyneither(_tmp153,sizeof(void*),0));});_LLDB:;}}static
int Cyc_Tcutil_unify_conrefs(int(*cmp)(void*,void*),struct Cyc_Absyn_Conref*x,
struct Cyc_Absyn_Conref*y){struct _handler_cons _tmp155;_push_handler(& _tmp155);{
int _tmp157=0;if(setjmp(_tmp155.handler))_tmp157=1;if(!_tmp157){Cyc_Tcutil_unify_it_conrefs(
cmp,x,y,(struct _dyneither_ptr)_tag_dyneither(0,0,0));{int _tmp158=1;_npop_handler(
0);return _tmp158;};_pop_handler();}else{void*_tmp156=(void*)_exn_thrown;void*
_tmp15A=_tmp156;_LLEA: if(_tmp15A != Cyc_Tcutil_Unify)goto _LLEC;_LLEB: return 0;
_LLEC:;_LLED:(void)_throw(_tmp15A);_LLE9:;}}}static int Cyc_Tcutil_boundscmp(void*
b1,void*b2){struct _tuple0 _tmp15C=({struct _tuple0 _tmp15B;_tmp15B.f1=b1;_tmp15B.f2=
b2;_tmp15B;});void*_tmp15D;void*_tmp15E;void*_tmp15F;void*_tmp160;void*_tmp161;
struct Cyc_Absyn_Exp*_tmp162;void*_tmp163;struct Cyc_Absyn_Exp*_tmp164;_LLEF:
_tmp15D=_tmp15C.f1;if((int)_tmp15D != 0)goto _LLF1;_tmp15E=_tmp15C.f2;if((int)
_tmp15E != 0)goto _LLF1;_LLF0: return 0;_LLF1: _tmp15F=_tmp15C.f1;if((int)_tmp15F != 0)
goto _LLF3;_LLF2: return - 1;_LLF3: _tmp160=_tmp15C.f2;if((int)_tmp160 != 0)goto _LLF5;
_LLF4: return 1;_LLF5: _tmp161=_tmp15C.f1;if(_tmp161 <= (void*)1)goto _LLEE;if(*((int*)
_tmp161)!= 0)goto _LLEE;_tmp162=((struct Cyc_Absyn_Upper_b_struct*)_tmp161)->f1;
_tmp163=_tmp15C.f2;if(_tmp163 <= (void*)1)goto _LLEE;if(*((int*)_tmp163)!= 0)goto
_LLEE;_tmp164=((struct Cyc_Absyn_Upper_b_struct*)_tmp163)->f1;_LLF6: return Cyc_Evexp_const_exp_cmp(
_tmp162,_tmp164);_LLEE:;}static int Cyc_Tcutil_unify_it_bounds(void*b1,void*b2){
struct _tuple0 _tmp166=({struct _tuple0 _tmp165;_tmp165.f1=b1;_tmp165.f2=b2;_tmp165;});
void*_tmp167;void*_tmp168;void*_tmp169;void*_tmp16A;void*_tmp16B;struct Cyc_Absyn_Exp*
_tmp16C;void*_tmp16D;struct Cyc_Absyn_Exp*_tmp16E;_LLF8: _tmp167=_tmp166.f1;if((
int)_tmp167 != 0)goto _LLFA;_tmp168=_tmp166.f2;if((int)_tmp168 != 0)goto _LLFA;_LLF9:
return 0;_LLFA: _tmp169=_tmp166.f1;if((int)_tmp169 != 0)goto _LLFC;_LLFB: return - 1;
_LLFC: _tmp16A=_tmp166.f2;if((int)_tmp16A != 0)goto _LLFE;_LLFD: return 1;_LLFE:
_tmp16B=_tmp166.f1;if(_tmp16B <= (void*)1)goto _LLF7;if(*((int*)_tmp16B)!= 0)goto
_LLF7;_tmp16C=((struct Cyc_Absyn_Upper_b_struct*)_tmp16B)->f1;_tmp16D=_tmp166.f2;
if(_tmp16D <= (void*)1)goto _LLF7;if(*((int*)_tmp16D)!= 0)goto _LLF7;_tmp16E=((
struct Cyc_Absyn_Upper_b_struct*)_tmp16D)->f1;_LLFF: return Cyc_Evexp_const_exp_cmp(
_tmp16C,_tmp16E);_LLF7:;}static int Cyc_Tcutil_attribute_case_number(void*att){
void*_tmp16F=att;_LL101: if(_tmp16F <= (void*)17)goto _LL103;if(*((int*)_tmp16F)!= 
0)goto _LL103;_LL102: return 0;_LL103: if((int)_tmp16F != 0)goto _LL105;_LL104: return 1;
_LL105: if((int)_tmp16F != 1)goto _LL107;_LL106: return 2;_LL107: if((int)_tmp16F != 2)
goto _LL109;_LL108: return 3;_LL109: if((int)_tmp16F != 3)goto _LL10B;_LL10A: return 4;
_LL10B: if((int)_tmp16F != 4)goto _LL10D;_LL10C: return 5;_LL10D: if(_tmp16F <= (void*)
17)goto _LL10F;if(*((int*)_tmp16F)!= 1)goto _LL10F;_LL10E: return 6;_LL10F: if((int)
_tmp16F != 5)goto _LL111;_LL110: return 7;_LL111: if(_tmp16F <= (void*)17)goto _LL113;
if(*((int*)_tmp16F)!= 2)goto _LL113;_LL112: return 8;_LL113: if((int)_tmp16F != 6)
goto _LL115;_LL114: return 9;_LL115: if((int)_tmp16F != 7)goto _LL117;_LL116: return 10;
_LL117: if((int)_tmp16F != 8)goto _LL119;_LL118: return 11;_LL119: if((int)_tmp16F != 9)
goto _LL11B;_LL11A: return 12;_LL11B: if((int)_tmp16F != 10)goto _LL11D;_LL11C: return
13;_LL11D: if((int)_tmp16F != 11)goto _LL11F;_LL11E: return 14;_LL11F: if((int)_tmp16F
!= 12)goto _LL121;_LL120: return 15;_LL121: if((int)_tmp16F != 13)goto _LL123;_LL122:
return 16;_LL123: if((int)_tmp16F != 14)goto _LL125;_LL124: return 17;_LL125: if((int)
_tmp16F != 15)goto _LL127;_LL126: return 18;_LL127: if(_tmp16F <= (void*)17)goto _LL12B;
if(*((int*)_tmp16F)!= 3)goto _LL129;_LL128: return 19;_LL129: if(*((int*)_tmp16F)!= 
4)goto _LL12B;_LL12A: return 20;_LL12B:;_LL12C: return 21;_LL100:;}static int Cyc_Tcutil_attribute_cmp(
void*att1,void*att2){struct _tuple0 _tmp171=({struct _tuple0 _tmp170;_tmp170.f1=att1;
_tmp170.f2=att2;_tmp170;});void*_tmp172;int _tmp173;void*_tmp174;int _tmp175;void*
_tmp176;int _tmp177;void*_tmp178;int _tmp179;void*_tmp17A;int _tmp17B;void*_tmp17C;
int _tmp17D;void*_tmp17E;struct _dyneither_ptr _tmp17F;void*_tmp180;struct
_dyneither_ptr _tmp181;void*_tmp182;void*_tmp183;int _tmp184;int _tmp185;void*
_tmp186;void*_tmp187;int _tmp188;int _tmp189;_LL12E: _tmp172=_tmp171.f1;if(_tmp172
<= (void*)17)goto _LL130;if(*((int*)_tmp172)!= 0)goto _LL130;_tmp173=((struct Cyc_Absyn_Regparm_att_struct*)
_tmp172)->f1;_tmp174=_tmp171.f2;if(_tmp174 <= (void*)17)goto _LL130;if(*((int*)
_tmp174)!= 0)goto _LL130;_tmp175=((struct Cyc_Absyn_Regparm_att_struct*)_tmp174)->f1;
_LL12F: _tmp177=_tmp173;_tmp179=_tmp175;goto _LL131;_LL130: _tmp176=_tmp171.f1;if(
_tmp176 <= (void*)17)goto _LL132;if(*((int*)_tmp176)!= 4)goto _LL132;_tmp177=((
struct Cyc_Absyn_Initializes_att_struct*)_tmp176)->f1;_tmp178=_tmp171.f2;if(
_tmp178 <= (void*)17)goto _LL132;if(*((int*)_tmp178)!= 4)goto _LL132;_tmp179=((
struct Cyc_Absyn_Initializes_att_struct*)_tmp178)->f1;_LL131: _tmp17B=_tmp177;
_tmp17D=_tmp179;goto _LL133;_LL132: _tmp17A=_tmp171.f1;if(_tmp17A <= (void*)17)goto
_LL134;if(*((int*)_tmp17A)!= 1)goto _LL134;_tmp17B=((struct Cyc_Absyn_Aligned_att_struct*)
_tmp17A)->f1;_tmp17C=_tmp171.f2;if(_tmp17C <= (void*)17)goto _LL134;if(*((int*)
_tmp17C)!= 1)goto _LL134;_tmp17D=((struct Cyc_Absyn_Aligned_att_struct*)_tmp17C)->f1;
_LL133: return Cyc_Core_intcmp(_tmp17B,_tmp17D);_LL134: _tmp17E=_tmp171.f1;if(
_tmp17E <= (void*)17)goto _LL136;if(*((int*)_tmp17E)!= 2)goto _LL136;_tmp17F=((
struct Cyc_Absyn_Section_att_struct*)_tmp17E)->f1;_tmp180=_tmp171.f2;if(_tmp180 <= (
void*)17)goto _LL136;if(*((int*)_tmp180)!= 2)goto _LL136;_tmp181=((struct Cyc_Absyn_Section_att_struct*)
_tmp180)->f1;_LL135: return Cyc_strcmp((struct _dyneither_ptr)_tmp17F,(struct
_dyneither_ptr)_tmp181);_LL136: _tmp182=_tmp171.f1;if(_tmp182 <= (void*)17)goto
_LL138;if(*((int*)_tmp182)!= 3)goto _LL138;_tmp183=(void*)((struct Cyc_Absyn_Format_att_struct*)
_tmp182)->f1;_tmp184=((struct Cyc_Absyn_Format_att_struct*)_tmp182)->f2;_tmp185=((
struct Cyc_Absyn_Format_att_struct*)_tmp182)->f3;_tmp186=_tmp171.f2;if(_tmp186 <= (
void*)17)goto _LL138;if(*((int*)_tmp186)!= 3)goto _LL138;_tmp187=(void*)((struct
Cyc_Absyn_Format_att_struct*)_tmp186)->f1;_tmp188=((struct Cyc_Absyn_Format_att_struct*)
_tmp186)->f2;_tmp189=((struct Cyc_Absyn_Format_att_struct*)_tmp186)->f3;_LL137: {
int _tmp18A=Cyc_Core_intcmp((int)((unsigned int)_tmp183),(int)((unsigned int)
_tmp187));if(_tmp18A != 0)return _tmp18A;{int _tmp18B=Cyc_Core_intcmp(_tmp184,
_tmp188);if(_tmp18B != 0)return _tmp18B;return Cyc_Core_intcmp(_tmp185,_tmp189);}}
_LL138:;_LL139: return Cyc_Core_intcmp(Cyc_Tcutil_attribute_case_number(att1),Cyc_Tcutil_attribute_case_number(
att2));_LL12D:;}static int Cyc_Tcutil_equal_att(void*a1,void*a2){return Cyc_Tcutil_attribute_cmp(
a1,a2)== 0;}int Cyc_Tcutil_same_atts(struct Cyc_List_List*a1,struct Cyc_List_List*
a2){{struct Cyc_List_List*a=a1;for(0;a != 0;a=a->tl){if(!Cyc_List_exists_c(Cyc_Tcutil_equal_att,(
void*)a->hd,a2))return 0;}}{struct Cyc_List_List*a=a2;for(0;a != 0;a=a->tl){if(!Cyc_List_exists_c(
Cyc_Tcutil_equal_att,(void*)a->hd,a1))return 0;}}return 1;}static void*Cyc_Tcutil_rgns_of(
void*t);static void*Cyc_Tcutil_rgns_of_field(struct Cyc_Absyn_Aggrfield*af){return
Cyc_Tcutil_rgns_of((void*)af->type);}static struct _tuple8*Cyc_Tcutil_region_free_subst(
struct Cyc_Absyn_Tvar*tv){void*t;{void*_tmp18C=Cyc_Tcutil_tvar_kind(tv);_LL13B:
if((int)_tmp18C != 4)goto _LL13D;_LL13C: t=(void*)3;goto _LL13A;_LL13D: if((int)
_tmp18C != 3)goto _LL13F;_LL13E: t=(void*)2;goto _LL13A;_LL13F: if((int)_tmp18C != 6)
goto _LL141;_LL140: t=Cyc_Absyn_empty_effect;goto _LL13A;_LL141: if((int)_tmp18C != 7)
goto _LL143;_LL142: t=(void*)({struct Cyc_Absyn_ValueofType_struct*_tmp18D=
_cycalloc(sizeof(*_tmp18D));_tmp18D[0]=({struct Cyc_Absyn_ValueofType_struct
_tmp18E;_tmp18E.tag=17;_tmp18E.f1=Cyc_Absyn_uint_exp(0,0);_tmp18E;});_tmp18D;});
goto _LL13A;_LL143:;_LL144: t=Cyc_Absyn_sint_typ;goto _LL13A;_LL13A:;}return({
struct _tuple8*_tmp18F=_cycalloc(sizeof(*_tmp18F));_tmp18F->f1=tv;_tmp18F->f2=t;
_tmp18F;});}static void*Cyc_Tcutil_rgns_of(void*t){void*_tmp190=Cyc_Tcutil_compress(
t);void*_tmp191;void*_tmp192;void*_tmp193;struct Cyc_Absyn_TunionInfo _tmp194;
struct Cyc_List_List*_tmp195;struct Cyc_Core_Opt*_tmp196;struct Cyc_Absyn_PtrInfo
_tmp197;void*_tmp198;struct Cyc_Absyn_PtrAtts _tmp199;void*_tmp19A;struct Cyc_Absyn_ArrayInfo
_tmp19B;void*_tmp19C;struct Cyc_List_List*_tmp19D;struct Cyc_Absyn_TunionFieldInfo
_tmp19E;struct Cyc_List_List*_tmp19F;struct Cyc_Absyn_AggrInfo _tmp1A0;struct Cyc_List_List*
_tmp1A1;struct Cyc_List_List*_tmp1A2;struct Cyc_Absyn_FnInfo _tmp1A3;struct Cyc_List_List*
_tmp1A4;struct Cyc_Core_Opt*_tmp1A5;void*_tmp1A6;struct Cyc_List_List*_tmp1A7;
struct Cyc_Absyn_VarargInfo*_tmp1A8;struct Cyc_List_List*_tmp1A9;void*_tmp1AA;
struct Cyc_List_List*_tmp1AB;_LL146: if((int)_tmp190 != 0)goto _LL148;_LL147: goto
_LL149;_LL148: if((int)_tmp190 != 1)goto _LL14A;_LL149: goto _LL14B;_LL14A: if(_tmp190
<= (void*)4)goto _LL16E;if(*((int*)_tmp190)!= 6)goto _LL14C;_LL14B: goto _LL14D;
_LL14C: if(*((int*)_tmp190)!= 12)goto _LL14E;_LL14D: goto _LL14F;_LL14E: if(*((int*)
_tmp190)!= 13)goto _LL150;_LL14F: goto _LL151;_LL150: if(*((int*)_tmp190)!= 17)goto
_LL152;_LL151: goto _LL153;_LL152: if(*((int*)_tmp190)!= 5)goto _LL154;_LL153: return
Cyc_Absyn_empty_effect;_LL154: if(*((int*)_tmp190)!= 0)goto _LL156;_LL155: goto
_LL157;_LL156: if(*((int*)_tmp190)!= 1)goto _LL158;_LL157: {void*_tmp1AC=Cyc_Tcutil_typ_kind(
t);_LL17B: if((int)_tmp1AC != 3)goto _LL17D;_LL17C: goto _LL17E;_LL17D: if((int)
_tmp1AC != 4)goto _LL17F;_LL17E: goto _LL180;_LL17F: if((int)_tmp1AC != 5)goto _LL181;
_LL180: return(void*)({struct Cyc_Absyn_AccessEff_struct*_tmp1AD=_cycalloc(sizeof(*
_tmp1AD));_tmp1AD[0]=({struct Cyc_Absyn_AccessEff_struct _tmp1AE;_tmp1AE.tag=19;
_tmp1AE.f1=(void*)t;_tmp1AE;});_tmp1AD;});_LL181: if((int)_tmp1AC != 6)goto _LL183;
_LL182: return t;_LL183: if((int)_tmp1AC != 7)goto _LL185;_LL184: return Cyc_Absyn_empty_effect;
_LL185:;_LL186: return(void*)({struct Cyc_Absyn_RgnsEff_struct*_tmp1AF=_cycalloc(
sizeof(*_tmp1AF));_tmp1AF[0]=({struct Cyc_Absyn_RgnsEff_struct _tmp1B0;_tmp1B0.tag=
21;_tmp1B0.f1=(void*)t;_tmp1B0;});_tmp1AF;});_LL17A:;}_LL158: if(*((int*)_tmp190)
!= 14)goto _LL15A;_tmp191=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)_tmp190)->f1;
_LL159: return(void*)({struct Cyc_Absyn_AccessEff_struct*_tmp1B1=_cycalloc(sizeof(*
_tmp1B1));_tmp1B1[0]=({struct Cyc_Absyn_AccessEff_struct _tmp1B2;_tmp1B2.tag=19;
_tmp1B2.f1=(void*)_tmp191;_tmp1B2;});_tmp1B1;});_LL15A: if(*((int*)_tmp190)!= 15)
goto _LL15C;_tmp192=(void*)((struct Cyc_Absyn_DynRgnType_struct*)_tmp190)->f1;
_tmp193=(void*)((struct Cyc_Absyn_DynRgnType_struct*)_tmp190)->f2;_LL15B: return(
void*)({struct Cyc_Absyn_AccessEff_struct*_tmp1B3=_cycalloc(sizeof(*_tmp1B3));
_tmp1B3[0]=({struct Cyc_Absyn_AccessEff_struct _tmp1B4;_tmp1B4.tag=19;_tmp1B4.f1=(
void*)_tmp193;_tmp1B4;});_tmp1B3;});_LL15C: if(*((int*)_tmp190)!= 2)goto _LL15E;
_tmp194=((struct Cyc_Absyn_TunionType_struct*)_tmp190)->f1;_tmp195=_tmp194.targs;
_tmp196=_tmp194.rgn;_LL15D: {struct Cyc_List_List*ts=Cyc_List_map(Cyc_Tcutil_rgns_of,
_tmp195);if((unsigned int)_tmp196)ts=({struct Cyc_List_List*_tmp1B5=_cycalloc(
sizeof(*_tmp1B5));_tmp1B5->hd=(void*)((void*)({struct Cyc_Absyn_AccessEff_struct*
_tmp1B6=_cycalloc(sizeof(*_tmp1B6));_tmp1B6[0]=({struct Cyc_Absyn_AccessEff_struct
_tmp1B7;_tmp1B7.tag=19;_tmp1B7.f1=(void*)((void*)_tmp196->v);_tmp1B7;});_tmp1B6;}));
_tmp1B5->tl=ts;_tmp1B5;});return Cyc_Tcutil_normalize_effect((void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp1B8=_cycalloc(sizeof(*_tmp1B8));_tmp1B8[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp1B9;_tmp1B9.tag=20;_tmp1B9.f1=ts;_tmp1B9;});_tmp1B8;}));}_LL15E: if(*((int*)
_tmp190)!= 4)goto _LL160;_tmp197=((struct Cyc_Absyn_PointerType_struct*)_tmp190)->f1;
_tmp198=(void*)_tmp197.elt_typ;_tmp199=_tmp197.ptr_atts;_tmp19A=(void*)_tmp199.rgn;
_LL15F: return Cyc_Tcutil_normalize_effect((void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp1BA=_cycalloc(sizeof(*_tmp1BA));_tmp1BA[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp1BB;_tmp1BB.tag=20;_tmp1BB.f1=({void*_tmp1BC[2];_tmp1BC[1]=Cyc_Tcutil_rgns_of(
_tmp198);_tmp1BC[0]=(void*)({struct Cyc_Absyn_AccessEff_struct*_tmp1BD=_cycalloc(
sizeof(*_tmp1BD));_tmp1BD[0]=({struct Cyc_Absyn_AccessEff_struct _tmp1BE;_tmp1BE.tag=
19;_tmp1BE.f1=(void*)_tmp19A;_tmp1BE;});_tmp1BD;});Cyc_List_list(_tag_dyneither(
_tmp1BC,sizeof(void*),2));});_tmp1BB;});_tmp1BA;}));_LL160: if(*((int*)_tmp190)!= 
7)goto _LL162;_tmp19B=((struct Cyc_Absyn_ArrayType_struct*)_tmp190)->f1;_tmp19C=(
void*)_tmp19B.elt_type;_LL161: return Cyc_Tcutil_normalize_effect(Cyc_Tcutil_rgns_of(
_tmp19C));_LL162: if(*((int*)_tmp190)!= 9)goto _LL164;_tmp19D=((struct Cyc_Absyn_TupleType_struct*)
_tmp190)->f1;_LL163: {struct Cyc_List_List*_tmp1BF=0;for(0;_tmp19D != 0;_tmp19D=
_tmp19D->tl){_tmp1BF=({struct Cyc_List_List*_tmp1C0=_cycalloc(sizeof(*_tmp1C0));
_tmp1C0->hd=(void*)(*((struct _tuple5*)_tmp19D->hd)).f2;_tmp1C0->tl=_tmp1BF;
_tmp1C0;});}_tmp19F=_tmp1BF;goto _LL165;}_LL164: if(*((int*)_tmp190)!= 3)goto
_LL166;_tmp19E=((struct Cyc_Absyn_TunionFieldType_struct*)_tmp190)->f1;_tmp19F=
_tmp19E.targs;_LL165: _tmp1A1=_tmp19F;goto _LL167;_LL166: if(*((int*)_tmp190)!= 10)
goto _LL168;_tmp1A0=((struct Cyc_Absyn_AggrType_struct*)_tmp190)->f1;_tmp1A1=
_tmp1A0.targs;_LL167: return Cyc_Tcutil_normalize_effect((void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp1C1=_cycalloc(sizeof(*_tmp1C1));_tmp1C1[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp1C2;_tmp1C2.tag=20;_tmp1C2.f1=Cyc_List_map(Cyc_Tcutil_rgns_of,_tmp1A1);
_tmp1C2;});_tmp1C1;}));_LL168: if(*((int*)_tmp190)!= 11)goto _LL16A;_tmp1A2=((
struct Cyc_Absyn_AnonAggrType_struct*)_tmp190)->f2;_LL169: return Cyc_Tcutil_normalize_effect((
void*)({struct Cyc_Absyn_JoinEff_struct*_tmp1C3=_cycalloc(sizeof(*_tmp1C3));
_tmp1C3[0]=({struct Cyc_Absyn_JoinEff_struct _tmp1C4;_tmp1C4.tag=20;_tmp1C4.f1=((
struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Tcutil_rgns_of_field,_tmp1A2);_tmp1C4;});_tmp1C3;}));_LL16A:
if(*((int*)_tmp190)!= 18)goto _LL16C;_LL16B: return Cyc_Absyn_empty_effect;_LL16C:
if(*((int*)_tmp190)!= 8)goto _LL16E;_tmp1A3=((struct Cyc_Absyn_FnType_struct*)
_tmp190)->f1;_tmp1A4=_tmp1A3.tvars;_tmp1A5=_tmp1A3.effect;_tmp1A6=(void*)_tmp1A3.ret_typ;
_tmp1A7=_tmp1A3.args;_tmp1A8=_tmp1A3.cyc_varargs;_tmp1A9=_tmp1A3.rgn_po;_LL16D: {
void*_tmp1C5=Cyc_Tcutil_substitute(((struct Cyc_List_List*(*)(struct _tuple8*(*f)(
struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_region_free_subst,
_tmp1A4),(void*)((struct Cyc_Core_Opt*)_check_null(_tmp1A5))->v);return Cyc_Tcutil_normalize_effect(
_tmp1C5);}_LL16E: if((int)_tmp190 != 3)goto _LL170;_LL16F: goto _LL171;_LL170: if((int)
_tmp190 != 2)goto _LL172;_LL171: return Cyc_Absyn_empty_effect;_LL172: if(_tmp190 <= (
void*)4)goto _LL174;if(*((int*)_tmp190)!= 19)goto _LL174;_LL173: goto _LL175;_LL174:
if(_tmp190 <= (void*)4)goto _LL176;if(*((int*)_tmp190)!= 20)goto _LL176;_LL175:
return t;_LL176: if(_tmp190 <= (void*)4)goto _LL178;if(*((int*)_tmp190)!= 21)goto
_LL178;_tmp1AA=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp190)->f1;_LL177:
return Cyc_Tcutil_rgns_of(_tmp1AA);_LL178: if(_tmp190 <= (void*)4)goto _LL145;if(*((
int*)_tmp190)!= 16)goto _LL145;_tmp1AB=((struct Cyc_Absyn_TypedefType_struct*)
_tmp190)->f2;_LL179: return Cyc_Tcutil_normalize_effect((void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp1C6=_cycalloc(sizeof(*_tmp1C6));_tmp1C6[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp1C7;_tmp1C7.tag=20;_tmp1C7.f1=Cyc_List_map(Cyc_Tcutil_rgns_of,_tmp1AB);
_tmp1C7;});_tmp1C6;}));_LL145:;}void*Cyc_Tcutil_normalize_effect(void*e){e=Cyc_Tcutil_compress(
e);{void*_tmp1C8=e;struct Cyc_List_List*_tmp1C9;struct Cyc_List_List**_tmp1CA;void*
_tmp1CB;_LL188: if(_tmp1C8 <= (void*)4)goto _LL18C;if(*((int*)_tmp1C8)!= 20)goto
_LL18A;_tmp1C9=((struct Cyc_Absyn_JoinEff_struct*)_tmp1C8)->f1;_tmp1CA=(struct Cyc_List_List**)&((
struct Cyc_Absyn_JoinEff_struct*)_tmp1C8)->f1;_LL189: {int redo_join=0;{struct Cyc_List_List*
effs=*_tmp1CA;for(0;effs != 0;effs=effs->tl){void*_tmp1CC=(void*)effs->hd;(void*)(
effs->hd=(void*)Cyc_Tcutil_compress(Cyc_Tcutil_normalize_effect(_tmp1CC)));{void*
_tmp1CD=(void*)effs->hd;void*_tmp1CE;_LL18F: if(_tmp1CD <= (void*)4)goto _LL193;if(*((
int*)_tmp1CD)!= 20)goto _LL191;_LL190: goto _LL192;_LL191: if(*((int*)_tmp1CD)!= 19)
goto _LL193;_tmp1CE=(void*)((struct Cyc_Absyn_AccessEff_struct*)_tmp1CD)->f1;if((
int)_tmp1CE != 2)goto _LL193;_LL192: redo_join=1;goto _LL18E;_LL193:;_LL194: goto
_LL18E;_LL18E:;}}}if(!redo_join)return e;{struct Cyc_List_List*effects=0;{struct
Cyc_List_List*effs=*_tmp1CA;for(0;effs != 0;effs=effs->tl){void*_tmp1CF=Cyc_Tcutil_compress((
void*)effs->hd);struct Cyc_List_List*_tmp1D0;void*_tmp1D1;_LL196: if(_tmp1CF <= (
void*)4)goto _LL19A;if(*((int*)_tmp1CF)!= 20)goto _LL198;_tmp1D0=((struct Cyc_Absyn_JoinEff_struct*)
_tmp1CF)->f1;_LL197: effects=Cyc_List_revappend(_tmp1D0,effects);goto _LL195;
_LL198: if(*((int*)_tmp1CF)!= 19)goto _LL19A;_tmp1D1=(void*)((struct Cyc_Absyn_AccessEff_struct*)
_tmp1CF)->f1;if((int)_tmp1D1 != 2)goto _LL19A;_LL199: goto _LL195;_LL19A:;_LL19B:
effects=({struct Cyc_List_List*_tmp1D2=_cycalloc(sizeof(*_tmp1D2));_tmp1D2->hd=(
void*)_tmp1CF;_tmp1D2->tl=effects;_tmp1D2;});goto _LL195;_LL195:;}}*_tmp1CA=Cyc_List_imp_rev(
effects);return e;}}_LL18A: if(*((int*)_tmp1C8)!= 21)goto _LL18C;_tmp1CB=(void*)((
struct Cyc_Absyn_RgnsEff_struct*)_tmp1C8)->f1;_LL18B: {void*_tmp1D3=Cyc_Tcutil_compress(
_tmp1CB);_LL19D: if(_tmp1D3 <= (void*)4)goto _LL1A1;if(*((int*)_tmp1D3)!= 0)goto
_LL19F;_LL19E: goto _LL1A0;_LL19F: if(*((int*)_tmp1D3)!= 1)goto _LL1A1;_LL1A0: return
e;_LL1A1:;_LL1A2: return Cyc_Tcutil_rgns_of(_tmp1CB);_LL19C:;}_LL18C:;_LL18D:
return e;_LL187:;}}static void*Cyc_Tcutil_dummy_fntype(void*eff){struct Cyc_Absyn_FnType_struct*
_tmp1D4=({struct Cyc_Absyn_FnType_struct*_tmp1D5=_cycalloc(sizeof(*_tmp1D5));
_tmp1D5[0]=({struct Cyc_Absyn_FnType_struct _tmp1D6;_tmp1D6.tag=8;_tmp1D6.f1=({
struct Cyc_Absyn_FnInfo _tmp1D7;_tmp1D7.tvars=0;_tmp1D7.effect=({struct Cyc_Core_Opt*
_tmp1D8=_cycalloc(sizeof(*_tmp1D8));_tmp1D8->v=(void*)eff;_tmp1D8;});_tmp1D7.ret_typ=(
void*)((void*)0);_tmp1D7.args=0;_tmp1D7.c_varargs=0;_tmp1D7.cyc_varargs=0;
_tmp1D7.rgn_po=0;_tmp1D7.attributes=0;_tmp1D7;});_tmp1D6;});_tmp1D5;});return Cyc_Absyn_atb_typ((
void*)_tmp1D4,(void*)2,Cyc_Absyn_empty_tqual(0),Cyc_Absyn_bounds_one,Cyc_Absyn_false_conref);}
int Cyc_Tcutil_region_in_effect(int constrain,void*r,void*e){r=Cyc_Tcutil_compress(
r);if(r == (void*)2  || r == (void*)3)return 1;{void*_tmp1D9=Cyc_Tcutil_compress(e);
void*_tmp1DA;struct Cyc_List_List*_tmp1DB;void*_tmp1DC;struct Cyc_Core_Opt*_tmp1DD;
struct Cyc_Core_Opt*_tmp1DE;struct Cyc_Core_Opt**_tmp1DF;struct Cyc_Core_Opt*
_tmp1E0;_LL1A4: if(_tmp1D9 <= (void*)4)goto _LL1AC;if(*((int*)_tmp1D9)!= 19)goto
_LL1A6;_tmp1DA=(void*)((struct Cyc_Absyn_AccessEff_struct*)_tmp1D9)->f1;_LL1A5:
if(constrain)return Cyc_Tcutil_unify(r,_tmp1DA);_tmp1DA=Cyc_Tcutil_compress(
_tmp1DA);if(r == _tmp1DA)return 1;{struct _tuple0 _tmp1E2=({struct _tuple0 _tmp1E1;
_tmp1E1.f1=r;_tmp1E1.f2=_tmp1DA;_tmp1E1;});void*_tmp1E3;struct Cyc_Absyn_Tvar*
_tmp1E4;void*_tmp1E5;struct Cyc_Absyn_Tvar*_tmp1E6;_LL1AF: _tmp1E3=_tmp1E2.f1;if(
_tmp1E3 <= (void*)4)goto _LL1B1;if(*((int*)_tmp1E3)!= 1)goto _LL1B1;_tmp1E4=((
struct Cyc_Absyn_VarType_struct*)_tmp1E3)->f1;_tmp1E5=_tmp1E2.f2;if(_tmp1E5 <= (
void*)4)goto _LL1B1;if(*((int*)_tmp1E5)!= 1)goto _LL1B1;_tmp1E6=((struct Cyc_Absyn_VarType_struct*)
_tmp1E5)->f1;_LL1B0: return Cyc_Absyn_tvar_cmp(_tmp1E4,_tmp1E6)== 0;_LL1B1:;_LL1B2:
return 0;_LL1AE:;}_LL1A6: if(*((int*)_tmp1D9)!= 20)goto _LL1A8;_tmp1DB=((struct Cyc_Absyn_JoinEff_struct*)
_tmp1D9)->f1;_LL1A7: for(0;_tmp1DB != 0;_tmp1DB=_tmp1DB->tl){if(Cyc_Tcutil_region_in_effect(
constrain,r,(void*)_tmp1DB->hd))return 1;}return 0;_LL1A8: if(*((int*)_tmp1D9)!= 21)
goto _LL1AA;_tmp1DC=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp1D9)->f1;_LL1A9: {
void*_tmp1E7=Cyc_Tcutil_rgns_of(_tmp1DC);void*_tmp1E8;_LL1B4: if(_tmp1E7 <= (void*)
4)goto _LL1B6;if(*((int*)_tmp1E7)!= 21)goto _LL1B6;_tmp1E8=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp1E7)->f1;_LL1B5: if(!constrain)return 0;{void*_tmp1E9=Cyc_Tcutil_compress(
_tmp1E8);struct Cyc_Core_Opt*_tmp1EA;struct Cyc_Core_Opt*_tmp1EB;struct Cyc_Core_Opt**
_tmp1EC;struct Cyc_Core_Opt*_tmp1ED;_LL1B9: if(_tmp1E9 <= (void*)4)goto _LL1BB;if(*((
int*)_tmp1E9)!= 0)goto _LL1BB;_tmp1EA=((struct Cyc_Absyn_Evar_struct*)_tmp1E9)->f1;
_tmp1EB=((struct Cyc_Absyn_Evar_struct*)_tmp1E9)->f2;_tmp1EC=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Evar_struct*)_tmp1E9)->f2;_tmp1ED=((struct Cyc_Absyn_Evar_struct*)
_tmp1E9)->f4;_LL1BA: {void*_tmp1EE=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,
_tmp1ED);Cyc_Tcutil_occurs(_tmp1EE,Cyc_Core_heap_region,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp1ED))->v,r);{void*_tmp1EF=Cyc_Tcutil_dummy_fntype((
void*)({struct Cyc_Absyn_JoinEff_struct*_tmp1F1=_cycalloc(sizeof(*_tmp1F1));
_tmp1F1[0]=({struct Cyc_Absyn_JoinEff_struct _tmp1F2;_tmp1F2.tag=20;_tmp1F2.f1=({
void*_tmp1F3[2];_tmp1F3[1]=(void*)({struct Cyc_Absyn_AccessEff_struct*_tmp1F4=
_cycalloc(sizeof(*_tmp1F4));_tmp1F4[0]=({struct Cyc_Absyn_AccessEff_struct _tmp1F5;
_tmp1F5.tag=19;_tmp1F5.f1=(void*)r;_tmp1F5;});_tmp1F4;});_tmp1F3[0]=_tmp1EE;Cyc_List_list(
_tag_dyneither(_tmp1F3,sizeof(void*),2));});_tmp1F2;});_tmp1F1;}));*_tmp1EC=({
struct Cyc_Core_Opt*_tmp1F0=_cycalloc(sizeof(*_tmp1F0));_tmp1F0->v=(void*)_tmp1EF;
_tmp1F0;});return 1;}}_LL1BB:;_LL1BC: return 0;_LL1B8:;}_LL1B6:;_LL1B7: return Cyc_Tcutil_region_in_effect(
constrain,r,_tmp1E7);_LL1B3:;}_LL1AA: if(*((int*)_tmp1D9)!= 0)goto _LL1AC;_tmp1DD=((
struct Cyc_Absyn_Evar_struct*)_tmp1D9)->f1;_tmp1DE=((struct Cyc_Absyn_Evar_struct*)
_tmp1D9)->f2;_tmp1DF=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)
_tmp1D9)->f2;_tmp1E0=((struct Cyc_Absyn_Evar_struct*)_tmp1D9)->f4;_LL1AB: if(
_tmp1DD == 0  || (void*)_tmp1DD->v != (void*)6)({void*_tmp1F6=0;((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp1F7="effect evar has wrong kind";
_tag_dyneither(_tmp1F7,sizeof(char),27);}),_tag_dyneither(_tmp1F6,sizeof(void*),
0));});if(!constrain)return 0;{void*_tmp1F8=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_ek,_tmp1E0);Cyc_Tcutil_occurs(_tmp1F8,Cyc_Core_heap_region,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp1E0))->v,r);{struct Cyc_Absyn_JoinEff_struct*
_tmp1F9=({struct Cyc_Absyn_JoinEff_struct*_tmp1FB=_cycalloc(sizeof(*_tmp1FB));
_tmp1FB[0]=({struct Cyc_Absyn_JoinEff_struct _tmp1FC;_tmp1FC.tag=20;_tmp1FC.f1=({
struct Cyc_List_List*_tmp1FD=_cycalloc(sizeof(*_tmp1FD));_tmp1FD->hd=(void*)
_tmp1F8;_tmp1FD->tl=({struct Cyc_List_List*_tmp1FE=_cycalloc(sizeof(*_tmp1FE));
_tmp1FE->hd=(void*)((void*)({struct Cyc_Absyn_AccessEff_struct*_tmp1FF=_cycalloc(
sizeof(*_tmp1FF));_tmp1FF[0]=({struct Cyc_Absyn_AccessEff_struct _tmp200;_tmp200.tag=
19;_tmp200.f1=(void*)r;_tmp200;});_tmp1FF;}));_tmp1FE->tl=0;_tmp1FE;});_tmp1FD;});
_tmp1FC;});_tmp1FB;});*_tmp1DF=({struct Cyc_Core_Opt*_tmp1FA=_cycalloc(sizeof(*
_tmp1FA));_tmp1FA->v=(void*)((void*)_tmp1F9);_tmp1FA;});return 1;}}_LL1AC:;_LL1AD:
return 0;_LL1A3:;}}static int Cyc_Tcutil_type_in_effect(int may_constrain_evars,void*
t,void*e){t=Cyc_Tcutil_compress(t);{void*_tmp201=Cyc_Tcutil_normalize_effect(Cyc_Tcutil_compress(
e));struct Cyc_List_List*_tmp202;void*_tmp203;struct Cyc_Core_Opt*_tmp204;struct
Cyc_Core_Opt*_tmp205;struct Cyc_Core_Opt**_tmp206;struct Cyc_Core_Opt*_tmp207;
_LL1BE: if(_tmp201 <= (void*)4)goto _LL1C6;if(*((int*)_tmp201)!= 19)goto _LL1C0;
_LL1BF: return 0;_LL1C0: if(*((int*)_tmp201)!= 20)goto _LL1C2;_tmp202=((struct Cyc_Absyn_JoinEff_struct*)
_tmp201)->f1;_LL1C1: for(0;_tmp202 != 0;_tmp202=_tmp202->tl){if(Cyc_Tcutil_type_in_effect(
may_constrain_evars,t,(void*)_tmp202->hd))return 1;}return 0;_LL1C2: if(*((int*)
_tmp201)!= 21)goto _LL1C4;_tmp203=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp201)->f1;_LL1C3: _tmp203=Cyc_Tcutil_compress(_tmp203);if(t == _tmp203)return 1;
if(may_constrain_evars)return Cyc_Tcutil_unify(t,_tmp203);{void*_tmp208=Cyc_Tcutil_rgns_of(
t);void*_tmp209;_LL1C9: if(_tmp208 <= (void*)4)goto _LL1CB;if(*((int*)_tmp208)!= 21)
goto _LL1CB;_tmp209=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp208)->f1;_LL1CA: {
struct _tuple0 _tmp20B=({struct _tuple0 _tmp20A;_tmp20A.f1=Cyc_Tcutil_compress(
_tmp209);_tmp20A.f2=_tmp203;_tmp20A;});void*_tmp20C;struct Cyc_Absyn_Tvar*_tmp20D;
void*_tmp20E;struct Cyc_Absyn_Tvar*_tmp20F;_LL1CE: _tmp20C=_tmp20B.f1;if(_tmp20C <= (
void*)4)goto _LL1D0;if(*((int*)_tmp20C)!= 1)goto _LL1D0;_tmp20D=((struct Cyc_Absyn_VarType_struct*)
_tmp20C)->f1;_tmp20E=_tmp20B.f2;if(_tmp20E <= (void*)4)goto _LL1D0;if(*((int*)
_tmp20E)!= 1)goto _LL1D0;_tmp20F=((struct Cyc_Absyn_VarType_struct*)_tmp20E)->f1;
_LL1CF: return Cyc_Tcutil_unify(t,_tmp203);_LL1D0:;_LL1D1: return _tmp209 == _tmp203;
_LL1CD:;}_LL1CB:;_LL1CC: return Cyc_Tcutil_type_in_effect(may_constrain_evars,t,
_tmp208);_LL1C8:;}_LL1C4: if(*((int*)_tmp201)!= 0)goto _LL1C6;_tmp204=((struct Cyc_Absyn_Evar_struct*)
_tmp201)->f1;_tmp205=((struct Cyc_Absyn_Evar_struct*)_tmp201)->f2;_tmp206=(struct
Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)_tmp201)->f2;_tmp207=((struct Cyc_Absyn_Evar_struct*)
_tmp201)->f4;_LL1C5: if(_tmp204 == 0  || (void*)_tmp204->v != (void*)6)({void*
_tmp210=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp211="effect evar has wrong kind";_tag_dyneither(_tmp211,sizeof(
char),27);}),_tag_dyneither(_tmp210,sizeof(void*),0));});if(!may_constrain_evars)
return 0;{void*_tmp212=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,
_tmp207);Cyc_Tcutil_occurs(_tmp212,Cyc_Core_heap_region,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp207))->v,t);{struct Cyc_Absyn_JoinEff_struct*
_tmp213=({struct Cyc_Absyn_JoinEff_struct*_tmp215=_cycalloc(sizeof(*_tmp215));
_tmp215[0]=({struct Cyc_Absyn_JoinEff_struct _tmp216;_tmp216.tag=20;_tmp216.f1=({
struct Cyc_List_List*_tmp217=_cycalloc(sizeof(*_tmp217));_tmp217->hd=(void*)
_tmp212;_tmp217->tl=({struct Cyc_List_List*_tmp218=_cycalloc(sizeof(*_tmp218));
_tmp218->hd=(void*)((void*)({struct Cyc_Absyn_RgnsEff_struct*_tmp219=_cycalloc(
sizeof(*_tmp219));_tmp219[0]=({struct Cyc_Absyn_RgnsEff_struct _tmp21A;_tmp21A.tag=
21;_tmp21A.f1=(void*)t;_tmp21A;});_tmp219;}));_tmp218->tl=0;_tmp218;});_tmp217;});
_tmp216;});_tmp215;});*_tmp206=({struct Cyc_Core_Opt*_tmp214=_cycalloc(sizeof(*
_tmp214));_tmp214->v=(void*)((void*)_tmp213);_tmp214;});return 1;}}_LL1C6:;_LL1C7:
return 0;_LL1BD:;}}static int Cyc_Tcutil_variable_in_effect(int may_constrain_evars,
struct Cyc_Absyn_Tvar*v,void*e){e=Cyc_Tcutil_compress(e);{void*_tmp21B=e;struct
Cyc_Absyn_Tvar*_tmp21C;struct Cyc_List_List*_tmp21D;void*_tmp21E;struct Cyc_Core_Opt*
_tmp21F;struct Cyc_Core_Opt*_tmp220;struct Cyc_Core_Opt**_tmp221;struct Cyc_Core_Opt*
_tmp222;_LL1D3: if(_tmp21B <= (void*)4)goto _LL1DB;if(*((int*)_tmp21B)!= 1)goto
_LL1D5;_tmp21C=((struct Cyc_Absyn_VarType_struct*)_tmp21B)->f1;_LL1D4: return Cyc_Absyn_tvar_cmp(
v,_tmp21C)== 0;_LL1D5: if(*((int*)_tmp21B)!= 20)goto _LL1D7;_tmp21D=((struct Cyc_Absyn_JoinEff_struct*)
_tmp21B)->f1;_LL1D6: for(0;_tmp21D != 0;_tmp21D=_tmp21D->tl){if(Cyc_Tcutil_variable_in_effect(
may_constrain_evars,v,(void*)_tmp21D->hd))return 1;}return 0;_LL1D7: if(*((int*)
_tmp21B)!= 21)goto _LL1D9;_tmp21E=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp21B)->f1;_LL1D8: {void*_tmp223=Cyc_Tcutil_rgns_of(_tmp21E);void*_tmp224;
_LL1DE: if(_tmp223 <= (void*)4)goto _LL1E0;if(*((int*)_tmp223)!= 21)goto _LL1E0;
_tmp224=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp223)->f1;_LL1DF: if(!
may_constrain_evars)return 0;{void*_tmp225=Cyc_Tcutil_compress(_tmp224);struct Cyc_Core_Opt*
_tmp226;struct Cyc_Core_Opt*_tmp227;struct Cyc_Core_Opt**_tmp228;struct Cyc_Core_Opt*
_tmp229;_LL1E3: if(_tmp225 <= (void*)4)goto _LL1E5;if(*((int*)_tmp225)!= 0)goto
_LL1E5;_tmp226=((struct Cyc_Absyn_Evar_struct*)_tmp225)->f1;_tmp227=((struct Cyc_Absyn_Evar_struct*)
_tmp225)->f2;_tmp228=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)
_tmp225)->f2;_tmp229=((struct Cyc_Absyn_Evar_struct*)_tmp225)->f4;_LL1E4: {void*
_tmp22A=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,_tmp229);if(!((
int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*
l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp229))->v,v))return 0;{void*_tmp22B=Cyc_Tcutil_dummy_fntype((
void*)({struct Cyc_Absyn_JoinEff_struct*_tmp22D=_cycalloc(sizeof(*_tmp22D));
_tmp22D[0]=({struct Cyc_Absyn_JoinEff_struct _tmp22E;_tmp22E.tag=20;_tmp22E.f1=({
void*_tmp22F[2];_tmp22F[1]=(void*)({struct Cyc_Absyn_VarType_struct*_tmp230=
_cycalloc(sizeof(*_tmp230));_tmp230[0]=({struct Cyc_Absyn_VarType_struct _tmp231;
_tmp231.tag=1;_tmp231.f1=v;_tmp231;});_tmp230;});_tmp22F[0]=_tmp22A;Cyc_List_list(
_tag_dyneither(_tmp22F,sizeof(void*),2));});_tmp22E;});_tmp22D;}));*_tmp228=({
struct Cyc_Core_Opt*_tmp22C=_cycalloc(sizeof(*_tmp22C));_tmp22C->v=(void*)_tmp22B;
_tmp22C;});return 1;}}_LL1E5:;_LL1E6: return 0;_LL1E2:;}_LL1E0:;_LL1E1: return Cyc_Tcutil_variable_in_effect(
may_constrain_evars,v,_tmp223);_LL1DD:;}_LL1D9: if(*((int*)_tmp21B)!= 0)goto
_LL1DB;_tmp21F=((struct Cyc_Absyn_Evar_struct*)_tmp21B)->f1;_tmp220=((struct Cyc_Absyn_Evar_struct*)
_tmp21B)->f2;_tmp221=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)
_tmp21B)->f2;_tmp222=((struct Cyc_Absyn_Evar_struct*)_tmp21B)->f4;_LL1DA: if(
_tmp21F == 0  || (void*)_tmp21F->v != (void*)6)({void*_tmp232=0;((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp233="effect evar has wrong kind";
_tag_dyneither(_tmp233,sizeof(char),27);}),_tag_dyneither(_tmp232,sizeof(void*),
0));});{void*_tmp234=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,
_tmp222);if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp222))->v,v))return 0;{
struct Cyc_Absyn_JoinEff_struct*_tmp235=({struct Cyc_Absyn_JoinEff_struct*_tmp237=
_cycalloc(sizeof(*_tmp237));_tmp237[0]=({struct Cyc_Absyn_JoinEff_struct _tmp238;
_tmp238.tag=20;_tmp238.f1=({struct Cyc_List_List*_tmp239=_cycalloc(sizeof(*
_tmp239));_tmp239->hd=(void*)_tmp234;_tmp239->tl=({struct Cyc_List_List*_tmp23A=
_cycalloc(sizeof(*_tmp23A));_tmp23A->hd=(void*)((void*)({struct Cyc_Absyn_VarType_struct*
_tmp23B=_cycalloc(sizeof(*_tmp23B));_tmp23B[0]=({struct Cyc_Absyn_VarType_struct
_tmp23C;_tmp23C.tag=1;_tmp23C.f1=v;_tmp23C;});_tmp23B;}));_tmp23A->tl=0;_tmp23A;});
_tmp239;});_tmp238;});_tmp237;});*_tmp221=({struct Cyc_Core_Opt*_tmp236=_cycalloc(
sizeof(*_tmp236));_tmp236->v=(void*)((void*)_tmp235);_tmp236;});return 1;}}_LL1DB:;
_LL1DC: return 0;_LL1D2:;}}static int Cyc_Tcutil_evar_in_effect(void*evar,void*e){e=
Cyc_Tcutil_compress(e);{void*_tmp23D=e;struct Cyc_List_List*_tmp23E;void*_tmp23F;
_LL1E8: if(_tmp23D <= (void*)4)goto _LL1EE;if(*((int*)_tmp23D)!= 20)goto _LL1EA;
_tmp23E=((struct Cyc_Absyn_JoinEff_struct*)_tmp23D)->f1;_LL1E9: for(0;_tmp23E != 0;
_tmp23E=_tmp23E->tl){if(Cyc_Tcutil_evar_in_effect(evar,(void*)_tmp23E->hd))
return 1;}return 0;_LL1EA: if(*((int*)_tmp23D)!= 21)goto _LL1EC;_tmp23F=(void*)((
struct Cyc_Absyn_RgnsEff_struct*)_tmp23D)->f1;_LL1EB: {void*_tmp240=Cyc_Tcutil_rgns_of(
_tmp23F);void*_tmp241;_LL1F1: if(_tmp240 <= (void*)4)goto _LL1F3;if(*((int*)_tmp240)
!= 21)goto _LL1F3;_tmp241=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp240)->f1;
_LL1F2: return 0;_LL1F3:;_LL1F4: return Cyc_Tcutil_evar_in_effect(evar,_tmp240);
_LL1F0:;}_LL1EC: if(*((int*)_tmp23D)!= 0)goto _LL1EE;_LL1ED: return evar == e;_LL1EE:;
_LL1EF: return 0;_LL1E7:;}}int Cyc_Tcutil_subset_effect(int may_constrain_evars,void*
e1,void*e2){void*_tmp242=Cyc_Tcutil_compress(e1);struct Cyc_List_List*_tmp243;
void*_tmp244;struct Cyc_Absyn_Tvar*_tmp245;void*_tmp246;struct Cyc_Core_Opt*
_tmp247;struct Cyc_Core_Opt**_tmp248;struct Cyc_Core_Opt*_tmp249;_LL1F6: if(_tmp242
<= (void*)4)goto _LL200;if(*((int*)_tmp242)!= 20)goto _LL1F8;_tmp243=((struct Cyc_Absyn_JoinEff_struct*)
_tmp242)->f1;_LL1F7: for(0;_tmp243 != 0;_tmp243=_tmp243->tl){if(!Cyc_Tcutil_subset_effect(
may_constrain_evars,(void*)_tmp243->hd,e2))return 0;}return 1;_LL1F8: if(*((int*)
_tmp242)!= 19)goto _LL1FA;_tmp244=(void*)((struct Cyc_Absyn_AccessEff_struct*)
_tmp242)->f1;_LL1F9: return Cyc_Tcutil_region_in_effect(0,_tmp244,e2) || 
may_constrain_evars  && Cyc_Tcutil_unify(_tmp244,(void*)2);_LL1FA: if(*((int*)
_tmp242)!= 1)goto _LL1FC;_tmp245=((struct Cyc_Absyn_VarType_struct*)_tmp242)->f1;
_LL1FB: return Cyc_Tcutil_variable_in_effect(may_constrain_evars,_tmp245,e2);
_LL1FC: if(*((int*)_tmp242)!= 21)goto _LL1FE;_tmp246=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp242)->f1;_LL1FD: {void*_tmp24A=Cyc_Tcutil_rgns_of(_tmp246);void*_tmp24B;
_LL203: if(_tmp24A <= (void*)4)goto _LL205;if(*((int*)_tmp24A)!= 21)goto _LL205;
_tmp24B=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp24A)->f1;_LL204: return Cyc_Tcutil_type_in_effect(
may_constrain_evars,_tmp24B,e2) || may_constrain_evars  && Cyc_Tcutil_unify(
_tmp24B,Cyc_Absyn_sint_typ);_LL205:;_LL206: return Cyc_Tcutil_subset_effect(
may_constrain_evars,_tmp24A,e2);_LL202:;}_LL1FE: if(*((int*)_tmp242)!= 0)goto
_LL200;_tmp247=((struct Cyc_Absyn_Evar_struct*)_tmp242)->f2;_tmp248=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Evar_struct*)_tmp242)->f2;_tmp249=((struct Cyc_Absyn_Evar_struct*)
_tmp242)->f4;_LL1FF: if(!Cyc_Tcutil_evar_in_effect(e1,e2))*_tmp248=({struct Cyc_Core_Opt*
_tmp24C=_cycalloc(sizeof(*_tmp24C));_tmp24C->v=(void*)Cyc_Absyn_empty_effect;
_tmp24C;});return 1;_LL200:;_LL201:({struct Cyc_String_pa_struct _tmp24F;_tmp24F.tag=
0;_tmp24F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
e1));{void*_tmp24D[1]={& _tmp24F};((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp24E="subset_effect: bad effect: %s";
_tag_dyneither(_tmp24E,sizeof(char),30);}),_tag_dyneither(_tmp24D,sizeof(void*),
1));}});_LL1F5:;}static int Cyc_Tcutil_unify_effect(void*e1,void*e2){e1=Cyc_Tcutil_normalize_effect(
e1);e2=Cyc_Tcutil_normalize_effect(e2);if(Cyc_Tcutil_subset_effect(0,e1,e2) && 
Cyc_Tcutil_subset_effect(0,e2,e1))return 1;if(Cyc_Tcutil_subset_effect(1,e1,e2)
 && Cyc_Tcutil_subset_effect(1,e2,e1))return 1;return 0;}static int Cyc_Tcutil_sub_rgnpo(
struct Cyc_List_List*rpo1,struct Cyc_List_List*rpo2){{struct Cyc_List_List*r1=rpo1;
for(0;r1 != 0;r1=r1->tl){struct _tuple0 _tmp251;void*_tmp252;void*_tmp253;struct
_tuple0*_tmp250=(struct _tuple0*)r1->hd;_tmp251=*_tmp250;_tmp252=_tmp251.f1;
_tmp253=_tmp251.f2;{int found=_tmp252 == (void*)2;{struct Cyc_List_List*r2=rpo2;
for(0;r2 != 0  && !found;r2=r2->tl){struct _tuple0 _tmp255;void*_tmp256;void*_tmp257;
struct _tuple0*_tmp254=(struct _tuple0*)r2->hd;_tmp255=*_tmp254;_tmp256=_tmp255.f1;
_tmp257=_tmp255.f2;if(Cyc_Tcutil_unify(_tmp252,_tmp256) && Cyc_Tcutil_unify(
_tmp253,_tmp257)){found=1;break;}}}if(!found)return 0;}}}return 1;}static int Cyc_Tcutil_same_rgn_po(
struct Cyc_List_List*rpo1,struct Cyc_List_List*rpo2){return Cyc_Tcutil_sub_rgnpo(
rpo1,rpo2) && Cyc_Tcutil_sub_rgnpo(rpo2,rpo1);}struct _tuple11{struct Cyc_Absyn_VarargInfo*
f1;struct Cyc_Absyn_VarargInfo*f2;};struct _tuple12{struct Cyc_Core_Opt*f1;struct
Cyc_Core_Opt*f2;};void Cyc_Tcutil_unify_it(void*t1,void*t2){Cyc_Tcutil_t1_failure=
t1;Cyc_Tcutil_t2_failure=t2;Cyc_Tcutil_failure_reason=(struct _dyneither_ptr)
_tag_dyneither(0,0,0);t1=Cyc_Tcutil_compress(t1);t2=Cyc_Tcutil_compress(t2);if(
t1 == t2)return;{void*_tmp258=t1;struct Cyc_Core_Opt*_tmp259;struct Cyc_Core_Opt*
_tmp25A;struct Cyc_Core_Opt**_tmp25B;struct Cyc_Core_Opt*_tmp25C;_LL208: if(_tmp258
<= (void*)4)goto _LL20A;if(*((int*)_tmp258)!= 0)goto _LL20A;_tmp259=((struct Cyc_Absyn_Evar_struct*)
_tmp258)->f1;_tmp25A=((struct Cyc_Absyn_Evar_struct*)_tmp258)->f2;_tmp25B=(struct
Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)_tmp258)->f2;_tmp25C=((struct Cyc_Absyn_Evar_struct*)
_tmp258)->f4;_LL209: Cyc_Tcutil_occurs(t1,Cyc_Core_heap_region,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp25C))->v,t2);{void*_tmp25D=Cyc_Tcutil_typ_kind(
t2);if(Cyc_Tcutil_kind_leq(_tmp25D,(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp259))->v)){*_tmp25B=({struct Cyc_Core_Opt*_tmp25E=_cycalloc(sizeof(*_tmp25E));
_tmp25E->v=(void*)t2;_tmp25E;});return;}else{{void*_tmp25F=t2;struct Cyc_Core_Opt*
_tmp260;struct Cyc_Core_Opt**_tmp261;struct Cyc_Core_Opt*_tmp262;struct Cyc_Absyn_PtrInfo
_tmp263;_LL20D: if(_tmp25F <= (void*)4)goto _LL211;if(*((int*)_tmp25F)!= 0)goto
_LL20F;_tmp260=((struct Cyc_Absyn_Evar_struct*)_tmp25F)->f2;_tmp261=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Evar_struct*)_tmp25F)->f2;_tmp262=((struct Cyc_Absyn_Evar_struct*)
_tmp25F)->f4;_LL20E: {struct Cyc_List_List*_tmp264=(struct Cyc_List_List*)_tmp25C->v;{
struct Cyc_List_List*s2=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(
_tmp262))->v;for(0;s2 != 0;s2=s2->tl){if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,
struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(
Cyc_Tcutil_fast_tvar_cmp,_tmp264,(struct Cyc_Absyn_Tvar*)s2->hd)){Cyc_Tcutil_failure_reason=({
const char*_tmp265="(type variable would escape scope)";_tag_dyneither(_tmp265,
sizeof(char),35);});(int)_throw((void*)Cyc_Tcutil_Unify);}}}if(Cyc_Tcutil_kind_leq((
void*)_tmp259->v,_tmp25D)){*_tmp261=({struct Cyc_Core_Opt*_tmp266=_cycalloc(
sizeof(*_tmp266));_tmp266->v=(void*)t1;_tmp266;});return;}Cyc_Tcutil_failure_reason=({
const char*_tmp267="(kinds are incompatible)";_tag_dyneither(_tmp267,sizeof(char),
25);});goto _LL20C;}_LL20F: if(*((int*)_tmp25F)!= 4)goto _LL211;_tmp263=((struct Cyc_Absyn_PointerType_struct*)
_tmp25F)->f1;if(!((void*)_tmp259->v == (void*)2))goto _LL211;_LL210: {struct Cyc_Absyn_Conref*
_tmp268=Cyc_Absyn_compress_conref((_tmp263.ptr_atts).bounds);{union Cyc_Absyn_Constraint_union
_tmp269=_tmp268->v;_LL214: if((_tmp269.No_constr).tag != 2)goto _LL216;_LL215:
_tmp268->v=(union Cyc_Absyn_Constraint_union)({union Cyc_Absyn_Constraint_union
_tmp26A;(_tmp26A.Eq_constr).tag=0;(_tmp26A.Eq_constr).f1=(void*)Cyc_Absyn_bounds_one;
_tmp26A;});*_tmp25B=({struct Cyc_Core_Opt*_tmp26B=_cycalloc(sizeof(*_tmp26B));
_tmp26B->v=(void*)t2;_tmp26B;});return;_LL216:;_LL217: goto _LL213;_LL213:;}goto
_LL20C;}_LL211:;_LL212: goto _LL20C;_LL20C:;}Cyc_Tcutil_failure_reason=({const char*
_tmp26C="(kinds are incompatible)";_tag_dyneither(_tmp26C,sizeof(char),25);});(
int)_throw((void*)Cyc_Tcutil_Unify);}}_LL20A:;_LL20B: goto _LL207;_LL207:;}{struct
_tuple0 _tmp26E=({struct _tuple0 _tmp26D;_tmp26D.f1=t2;_tmp26D.f2=t1;_tmp26D;});
void*_tmp26F;void*_tmp270;void*_tmp271;void*_tmp272;struct Cyc_Absyn_Tvar*_tmp273;
void*_tmp274;struct Cyc_Absyn_Tvar*_tmp275;void*_tmp276;struct Cyc_Absyn_AggrInfo
_tmp277;union Cyc_Absyn_AggrInfoU_union _tmp278;struct Cyc_List_List*_tmp279;void*
_tmp27A;struct Cyc_Absyn_AggrInfo _tmp27B;union Cyc_Absyn_AggrInfoU_union _tmp27C;
struct Cyc_List_List*_tmp27D;void*_tmp27E;struct _tuple2*_tmp27F;void*_tmp280;
struct _tuple2*_tmp281;void*_tmp282;struct Cyc_List_List*_tmp283;void*_tmp284;
struct Cyc_List_List*_tmp285;void*_tmp286;struct Cyc_Absyn_TunionInfo _tmp287;union
Cyc_Absyn_TunionInfoU_union _tmp288;struct Cyc_Absyn_Tuniondecl**_tmp289;struct Cyc_Absyn_Tuniondecl*
_tmp28A;struct Cyc_List_List*_tmp28B;struct Cyc_Core_Opt*_tmp28C;void*_tmp28D;
struct Cyc_Absyn_TunionInfo _tmp28E;union Cyc_Absyn_TunionInfoU_union _tmp28F;struct
Cyc_Absyn_Tuniondecl**_tmp290;struct Cyc_Absyn_Tuniondecl*_tmp291;struct Cyc_List_List*
_tmp292;struct Cyc_Core_Opt*_tmp293;void*_tmp294;struct Cyc_Absyn_TunionFieldInfo
_tmp295;union Cyc_Absyn_TunionFieldInfoU_union _tmp296;struct Cyc_Absyn_Tuniondecl*
_tmp297;struct Cyc_Absyn_Tunionfield*_tmp298;struct Cyc_List_List*_tmp299;void*
_tmp29A;struct Cyc_Absyn_TunionFieldInfo _tmp29B;union Cyc_Absyn_TunionFieldInfoU_union
_tmp29C;struct Cyc_Absyn_Tuniondecl*_tmp29D;struct Cyc_Absyn_Tunionfield*_tmp29E;
struct Cyc_List_List*_tmp29F;void*_tmp2A0;struct Cyc_Absyn_PtrInfo _tmp2A1;void*
_tmp2A2;struct Cyc_Absyn_Tqual _tmp2A3;struct Cyc_Absyn_PtrAtts _tmp2A4;void*_tmp2A5;
struct Cyc_Absyn_Conref*_tmp2A6;struct Cyc_Absyn_Conref*_tmp2A7;struct Cyc_Absyn_Conref*
_tmp2A8;void*_tmp2A9;struct Cyc_Absyn_PtrInfo _tmp2AA;void*_tmp2AB;struct Cyc_Absyn_Tqual
_tmp2AC;struct Cyc_Absyn_PtrAtts _tmp2AD;void*_tmp2AE;struct Cyc_Absyn_Conref*
_tmp2AF;struct Cyc_Absyn_Conref*_tmp2B0;struct Cyc_Absyn_Conref*_tmp2B1;void*
_tmp2B2;void*_tmp2B3;void*_tmp2B4;void*_tmp2B5;void*_tmp2B6;void*_tmp2B7;void*
_tmp2B8;void*_tmp2B9;void*_tmp2BA;int _tmp2BB;void*_tmp2BC;int _tmp2BD;void*
_tmp2BE;void*_tmp2BF;void*_tmp2C0;void*_tmp2C1;void*_tmp2C2;struct Cyc_Absyn_Exp*
_tmp2C3;void*_tmp2C4;struct Cyc_Absyn_Exp*_tmp2C5;void*_tmp2C6;struct Cyc_Absyn_ArrayInfo
_tmp2C7;void*_tmp2C8;struct Cyc_Absyn_Tqual _tmp2C9;struct Cyc_Absyn_Exp*_tmp2CA;
struct Cyc_Absyn_Conref*_tmp2CB;void*_tmp2CC;struct Cyc_Absyn_ArrayInfo _tmp2CD;
void*_tmp2CE;struct Cyc_Absyn_Tqual _tmp2CF;struct Cyc_Absyn_Exp*_tmp2D0;struct Cyc_Absyn_Conref*
_tmp2D1;void*_tmp2D2;struct Cyc_Absyn_FnInfo _tmp2D3;struct Cyc_List_List*_tmp2D4;
struct Cyc_Core_Opt*_tmp2D5;void*_tmp2D6;struct Cyc_List_List*_tmp2D7;int _tmp2D8;
struct Cyc_Absyn_VarargInfo*_tmp2D9;struct Cyc_List_List*_tmp2DA;struct Cyc_List_List*
_tmp2DB;void*_tmp2DC;struct Cyc_Absyn_FnInfo _tmp2DD;struct Cyc_List_List*_tmp2DE;
struct Cyc_Core_Opt*_tmp2DF;void*_tmp2E0;struct Cyc_List_List*_tmp2E1;int _tmp2E2;
struct Cyc_Absyn_VarargInfo*_tmp2E3;struct Cyc_List_List*_tmp2E4;struct Cyc_List_List*
_tmp2E5;void*_tmp2E6;struct Cyc_List_List*_tmp2E7;void*_tmp2E8;struct Cyc_List_List*
_tmp2E9;void*_tmp2EA;void*_tmp2EB;struct Cyc_List_List*_tmp2EC;void*_tmp2ED;void*
_tmp2EE;struct Cyc_List_List*_tmp2EF;void*_tmp2F0;void*_tmp2F1;void*_tmp2F2;void*
_tmp2F3;void*_tmp2F4;void*_tmp2F5;void*_tmp2F6;void*_tmp2F7;void*_tmp2F8;void*
_tmp2F9;void*_tmp2FA;void*_tmp2FB;void*_tmp2FC;void*_tmp2FD;void*_tmp2FE;void*
_tmp2FF;void*_tmp300;void*_tmp301;void*_tmp302;void*_tmp303;_LL219: _tmp26F=
_tmp26E.f1;if(_tmp26F <= (void*)4)goto _LL21B;if(*((int*)_tmp26F)!= 0)goto _LL21B;
_LL21A: Cyc_Tcutil_unify_it(t2,t1);return;_LL21B: _tmp270=_tmp26E.f1;if((int)
_tmp270 != 0)goto _LL21D;_tmp271=_tmp26E.f2;if((int)_tmp271 != 0)goto _LL21D;_LL21C:
return;_LL21D: _tmp272=_tmp26E.f1;if(_tmp272 <= (void*)4)goto _LL21F;if(*((int*)
_tmp272)!= 1)goto _LL21F;_tmp273=((struct Cyc_Absyn_VarType_struct*)_tmp272)->f1;
_tmp274=_tmp26E.f2;if(_tmp274 <= (void*)4)goto _LL21F;if(*((int*)_tmp274)!= 1)goto
_LL21F;_tmp275=((struct Cyc_Absyn_VarType_struct*)_tmp274)->f1;_LL21E: {struct
_dyneither_ptr*_tmp304=_tmp273->name;struct _dyneither_ptr*_tmp305=_tmp275->name;
int _tmp306=_tmp273->identity;int _tmp307=_tmp275->identity;void*_tmp308=Cyc_Tcutil_tvar_kind(
_tmp273);void*_tmp309=Cyc_Tcutil_tvar_kind(_tmp275);if(_tmp307 == _tmp306  && Cyc_strptrcmp(
_tmp304,_tmp305)== 0){if(_tmp308 != _tmp309)({struct Cyc_String_pa_struct _tmp30E;
_tmp30E.tag=0;_tmp30E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(
_tmp309));{struct Cyc_String_pa_struct _tmp30D;_tmp30D.tag=0;_tmp30D.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(_tmp308));{struct
Cyc_String_pa_struct _tmp30C;_tmp30C.tag=0;_tmp30C.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)*_tmp304);{void*_tmp30A[3]={& _tmp30C,& _tmp30D,& _tmp30E};((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const
char*_tmp30B="same type variable %s has kinds %s and %s";_tag_dyneither(_tmp30B,
sizeof(char),42);}),_tag_dyneither(_tmp30A,sizeof(void*),3));}}}});return;}Cyc_Tcutil_failure_reason=({
const char*_tmp30F="(variable types are not the same)";_tag_dyneither(_tmp30F,
sizeof(char),34);});goto _LL218;}_LL21F: _tmp276=_tmp26E.f1;if(_tmp276 <= (void*)4)
goto _LL221;if(*((int*)_tmp276)!= 10)goto _LL221;_tmp277=((struct Cyc_Absyn_AggrType_struct*)
_tmp276)->f1;_tmp278=_tmp277.aggr_info;_tmp279=_tmp277.targs;_tmp27A=_tmp26E.f2;
if(_tmp27A <= (void*)4)goto _LL221;if(*((int*)_tmp27A)!= 10)goto _LL221;_tmp27B=((
struct Cyc_Absyn_AggrType_struct*)_tmp27A)->f1;_tmp27C=_tmp27B.aggr_info;_tmp27D=
_tmp27B.targs;_LL220: {void*_tmp311;struct _tuple2*_tmp312;struct _tuple6 _tmp310=
Cyc_Absyn_aggr_kinded_name(_tmp27C);_tmp311=_tmp310.f1;_tmp312=_tmp310.f2;{void*
_tmp314;struct _tuple2*_tmp315;struct _tuple6 _tmp313=Cyc_Absyn_aggr_kinded_name(
_tmp278);_tmp314=_tmp313.f1;_tmp315=_tmp313.f2;if(_tmp311 != _tmp314){Cyc_Tcutil_failure_reason=({
const char*_tmp316="(struct and union type)";_tag_dyneither(_tmp316,sizeof(char),
24);});goto _LL218;}if(Cyc_Absyn_qvar_cmp(_tmp312,_tmp315)!= 0){Cyc_Tcutil_failure_reason=({
const char*_tmp317="(different type name)";_tag_dyneither(_tmp317,sizeof(char),22);});
goto _LL218;}Cyc_Tcutil_unify_list(_tmp27D,_tmp279);return;}}_LL221: _tmp27E=
_tmp26E.f1;if(_tmp27E <= (void*)4)goto _LL223;if(*((int*)_tmp27E)!= 12)goto _LL223;
_tmp27F=((struct Cyc_Absyn_EnumType_struct*)_tmp27E)->f1;_tmp280=_tmp26E.f2;if(
_tmp280 <= (void*)4)goto _LL223;if(*((int*)_tmp280)!= 12)goto _LL223;_tmp281=((
struct Cyc_Absyn_EnumType_struct*)_tmp280)->f1;_LL222: if(Cyc_Absyn_qvar_cmp(
_tmp27F,_tmp281)== 0)return;Cyc_Tcutil_failure_reason=({const char*_tmp318="(different enum types)";
_tag_dyneither(_tmp318,sizeof(char),23);});goto _LL218;_LL223: _tmp282=_tmp26E.f1;
if(_tmp282 <= (void*)4)goto _LL225;if(*((int*)_tmp282)!= 13)goto _LL225;_tmp283=((
struct Cyc_Absyn_AnonEnumType_struct*)_tmp282)->f1;_tmp284=_tmp26E.f2;if(_tmp284
<= (void*)4)goto _LL225;if(*((int*)_tmp284)!= 13)goto _LL225;_tmp285=((struct Cyc_Absyn_AnonEnumType_struct*)
_tmp284)->f1;_LL224: {int bad=0;for(0;_tmp283 != 0  && _tmp285 != 0;(_tmp283=_tmp283->tl,
_tmp285=_tmp285->tl)){struct Cyc_Absyn_Enumfield*_tmp319=(struct Cyc_Absyn_Enumfield*)
_tmp283->hd;struct Cyc_Absyn_Enumfield*_tmp31A=(struct Cyc_Absyn_Enumfield*)
_tmp285->hd;if(Cyc_Absyn_qvar_cmp(_tmp319->name,_tmp31A->name)!= 0){Cyc_Tcutil_failure_reason=({
const char*_tmp31B="(different names for enum fields)";_tag_dyneither(_tmp31B,
sizeof(char),34);});bad=1;break;}if(_tmp319->tag == _tmp31A->tag)continue;if(
_tmp319->tag == 0  || _tmp31A->tag == 0){Cyc_Tcutil_failure_reason=({const char*
_tmp31C="(different tag values for enum fields)";_tag_dyneither(_tmp31C,sizeof(
char),39);});bad=1;break;}if(!Cyc_Evexp_same_const_exp((struct Cyc_Absyn_Exp*)
_check_null(_tmp319->tag),(struct Cyc_Absyn_Exp*)_check_null(_tmp31A->tag))){Cyc_Tcutil_failure_reason=({
const char*_tmp31D="(different tag values for enum fields)";_tag_dyneither(
_tmp31D,sizeof(char),39);});bad=1;break;}}if(bad)goto _LL218;if(_tmp283 == 0  && 
_tmp285 == 0)return;Cyc_Tcutil_failure_reason=({const char*_tmp31E="(different number of fields for enums)";
_tag_dyneither(_tmp31E,sizeof(char),39);});goto _LL218;}_LL225: _tmp286=_tmp26E.f1;
if(_tmp286 <= (void*)4)goto _LL227;if(*((int*)_tmp286)!= 2)goto _LL227;_tmp287=((
struct Cyc_Absyn_TunionType_struct*)_tmp286)->f1;_tmp288=_tmp287.tunion_info;if((((((
struct Cyc_Absyn_TunionType_struct*)_tmp286)->f1).tunion_info).KnownTunion).tag != 
1)goto _LL227;_tmp289=(_tmp288.KnownTunion).f1;_tmp28A=*_tmp289;_tmp28B=_tmp287.targs;
_tmp28C=_tmp287.rgn;_tmp28D=_tmp26E.f2;if(_tmp28D <= (void*)4)goto _LL227;if(*((
int*)_tmp28D)!= 2)goto _LL227;_tmp28E=((struct Cyc_Absyn_TunionType_struct*)
_tmp28D)->f1;_tmp28F=_tmp28E.tunion_info;if((((((struct Cyc_Absyn_TunionType_struct*)
_tmp28D)->f1).tunion_info).KnownTunion).tag != 1)goto _LL227;_tmp290=(_tmp28F.KnownTunion).f1;
_tmp291=*_tmp290;_tmp292=_tmp28E.targs;_tmp293=_tmp28E.rgn;_LL226: if(_tmp28A == 
_tmp291  || Cyc_Absyn_qvar_cmp(_tmp28A->name,_tmp291->name)== 0){if(_tmp293 != 0
 && _tmp28C != 0)Cyc_Tcutil_unify_it((void*)_tmp293->v,(void*)_tmp28C->v);else{
if(_tmp293 != 0  || _tmp28C != 0)goto Tunion_fail;}Cyc_Tcutil_unify_list(_tmp292,
_tmp28B);return;}Tunion_fail: Cyc_Tcutil_failure_reason=({const char*_tmp31F="(different tunion types)";
_tag_dyneither(_tmp31F,sizeof(char),25);});goto _LL218;_LL227: _tmp294=_tmp26E.f1;
if(_tmp294 <= (void*)4)goto _LL229;if(*((int*)_tmp294)!= 3)goto _LL229;_tmp295=((
struct Cyc_Absyn_TunionFieldType_struct*)_tmp294)->f1;_tmp296=_tmp295.field_info;
if((((((struct Cyc_Absyn_TunionFieldType_struct*)_tmp294)->f1).field_info).KnownTunionfield).tag
!= 1)goto _LL229;_tmp297=(_tmp296.KnownTunionfield).f1;_tmp298=(_tmp296.KnownTunionfield).f2;
_tmp299=_tmp295.targs;_tmp29A=_tmp26E.f2;if(_tmp29A <= (void*)4)goto _LL229;if(*((
int*)_tmp29A)!= 3)goto _LL229;_tmp29B=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp29A)->f1;_tmp29C=_tmp29B.field_info;if((((((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp29A)->f1).field_info).KnownTunionfield).tag != 1)goto _LL229;_tmp29D=(_tmp29C.KnownTunionfield).f1;
_tmp29E=(_tmp29C.KnownTunionfield).f2;_tmp29F=_tmp29B.targs;_LL228: if((_tmp297 == 
_tmp29D  || Cyc_Absyn_qvar_cmp(_tmp297->name,_tmp29D->name)== 0) && (_tmp298 == 
_tmp29E  || Cyc_Absyn_qvar_cmp(_tmp298->name,_tmp29E->name)== 0)){Cyc_Tcutil_unify_list(
_tmp29F,_tmp299);return;}Cyc_Tcutil_failure_reason=({const char*_tmp320="(different tunion field types)";
_tag_dyneither(_tmp320,sizeof(char),31);});goto _LL218;_LL229: _tmp2A0=_tmp26E.f1;
if(_tmp2A0 <= (void*)4)goto _LL22B;if(*((int*)_tmp2A0)!= 4)goto _LL22B;_tmp2A1=((
struct Cyc_Absyn_PointerType_struct*)_tmp2A0)->f1;_tmp2A2=(void*)_tmp2A1.elt_typ;
_tmp2A3=_tmp2A1.elt_tq;_tmp2A4=_tmp2A1.ptr_atts;_tmp2A5=(void*)_tmp2A4.rgn;
_tmp2A6=_tmp2A4.nullable;_tmp2A7=_tmp2A4.bounds;_tmp2A8=_tmp2A4.zero_term;
_tmp2A9=_tmp26E.f2;if(_tmp2A9 <= (void*)4)goto _LL22B;if(*((int*)_tmp2A9)!= 4)goto
_LL22B;_tmp2AA=((struct Cyc_Absyn_PointerType_struct*)_tmp2A9)->f1;_tmp2AB=(void*)
_tmp2AA.elt_typ;_tmp2AC=_tmp2AA.elt_tq;_tmp2AD=_tmp2AA.ptr_atts;_tmp2AE=(void*)
_tmp2AD.rgn;_tmp2AF=_tmp2AD.nullable;_tmp2B0=_tmp2AD.bounds;_tmp2B1=_tmp2AD.zero_term;
_LL22A: Cyc_Tcutil_unify_it(_tmp2AB,_tmp2A2);Cyc_Tcutil_unify_it(_tmp2A5,_tmp2AE);
Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;((void(*)(int(*cmp)(int,int),
struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y,struct _dyneither_ptr reason))
Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,_tmp2B1,_tmp2A8,({const char*_tmp321="(not both zero terminated)";
_tag_dyneither(_tmp321,sizeof(char),27);}));Cyc_Tcutil_unify_tqual(_tmp2AC,
_tmp2AB,_tmp2A3,_tmp2A2);Cyc_Tcutil_unify_it_conrefs(Cyc_Tcutil_unify_it_bounds,
_tmp2B0,_tmp2A7,({const char*_tmp322="(different pointer bounds)";_tag_dyneither(
_tmp322,sizeof(char),27);}));{union Cyc_Absyn_Constraint_union _tmp323=(Cyc_Absyn_compress_conref(
_tmp2B0))->v;void*_tmp324;_LL254: if((_tmp323.Eq_constr).tag != 0)goto _LL256;
_tmp324=(_tmp323.Eq_constr).f1;if((int)_tmp324 != 0)goto _LL256;_LL255: return;
_LL256:;_LL257: goto _LL253;_LL253:;}((void(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*
x,struct Cyc_Absyn_Conref*y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(
Cyc_Core_intcmp,_tmp2AF,_tmp2A6,({const char*_tmp325="(incompatible pointer types)";
_tag_dyneither(_tmp325,sizeof(char),29);}));return;_LL22B: _tmp2B2=_tmp26E.f1;if(
_tmp2B2 <= (void*)4)goto _LL22D;if(*((int*)_tmp2B2)!= 5)goto _LL22D;_tmp2B3=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp2B2)->f1;_tmp2B4=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp2B2)->f2;_tmp2B5=_tmp26E.f2;if(_tmp2B5 <= (void*)4)goto _LL22D;if(*((int*)
_tmp2B5)!= 5)goto _LL22D;_tmp2B6=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp2B5)->f1;
_tmp2B7=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp2B5)->f2;_LL22C: if(_tmp2B6
== _tmp2B3  && ((_tmp2B7 == _tmp2B4  || _tmp2B7 == (void*)2  && _tmp2B4 == (void*)3)
 || _tmp2B7 == (void*)3  && _tmp2B4 == (void*)2))return;Cyc_Tcutil_failure_reason=({
const char*_tmp326="(different integral types)";_tag_dyneither(_tmp326,sizeof(
char),27);});goto _LL218;_LL22D: _tmp2B8=_tmp26E.f1;if((int)_tmp2B8 != 1)goto _LL22F;
_tmp2B9=_tmp26E.f2;if((int)_tmp2B9 != 1)goto _LL22F;_LL22E: return;_LL22F: _tmp2BA=
_tmp26E.f1;if(_tmp2BA <= (void*)4)goto _LL231;if(*((int*)_tmp2BA)!= 6)goto _LL231;
_tmp2BB=((struct Cyc_Absyn_DoubleType_struct*)_tmp2BA)->f1;_tmp2BC=_tmp26E.f2;if(
_tmp2BC <= (void*)4)goto _LL231;if(*((int*)_tmp2BC)!= 6)goto _LL231;_tmp2BD=((
struct Cyc_Absyn_DoubleType_struct*)_tmp2BC)->f1;_LL230: if(_tmp2BB == _tmp2BD)
return;goto _LL218;_LL231: _tmp2BE=_tmp26E.f1;if(_tmp2BE <= (void*)4)goto _LL233;if(*((
int*)_tmp2BE)!= 18)goto _LL233;_tmp2BF=(void*)((struct Cyc_Absyn_TagType_struct*)
_tmp2BE)->f1;_tmp2C0=_tmp26E.f2;if(_tmp2C0 <= (void*)4)goto _LL233;if(*((int*)
_tmp2C0)!= 18)goto _LL233;_tmp2C1=(void*)((struct Cyc_Absyn_TagType_struct*)
_tmp2C0)->f1;_LL232: Cyc_Tcutil_unify_it(_tmp2BF,_tmp2C1);return;_LL233: _tmp2C2=
_tmp26E.f1;if(_tmp2C2 <= (void*)4)goto _LL235;if(*((int*)_tmp2C2)!= 17)goto _LL235;
_tmp2C3=((struct Cyc_Absyn_ValueofType_struct*)_tmp2C2)->f1;_tmp2C4=_tmp26E.f2;
if(_tmp2C4 <= (void*)4)goto _LL235;if(*((int*)_tmp2C4)!= 17)goto _LL235;_tmp2C5=((
struct Cyc_Absyn_ValueofType_struct*)_tmp2C4)->f1;_LL234: if(!Cyc_Evexp_same_const_exp(
_tmp2C3,_tmp2C5)){Cyc_Tcutil_failure_reason=({const char*_tmp327="(cannot prove expressions are the same)";
_tag_dyneither(_tmp327,sizeof(char),40);});goto _LL218;}return;_LL235: _tmp2C6=
_tmp26E.f1;if(_tmp2C6 <= (void*)4)goto _LL237;if(*((int*)_tmp2C6)!= 7)goto _LL237;
_tmp2C7=((struct Cyc_Absyn_ArrayType_struct*)_tmp2C6)->f1;_tmp2C8=(void*)_tmp2C7.elt_type;
_tmp2C9=_tmp2C7.tq;_tmp2CA=_tmp2C7.num_elts;_tmp2CB=_tmp2C7.zero_term;_tmp2CC=
_tmp26E.f2;if(_tmp2CC <= (void*)4)goto _LL237;if(*((int*)_tmp2CC)!= 7)goto _LL237;
_tmp2CD=((struct Cyc_Absyn_ArrayType_struct*)_tmp2CC)->f1;_tmp2CE=(void*)_tmp2CD.elt_type;
_tmp2CF=_tmp2CD.tq;_tmp2D0=_tmp2CD.num_elts;_tmp2D1=_tmp2CD.zero_term;_LL236: Cyc_Tcutil_unify_it(
_tmp2CE,_tmp2C8);Cyc_Tcutil_unify_tqual(_tmp2CF,_tmp2CE,_tmp2C9,_tmp2C8);Cyc_Tcutil_t1_failure=
t1;Cyc_Tcutil_t2_failure=t2;((void(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*
x,struct Cyc_Absyn_Conref*y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(
Cyc_Core_intcmp,_tmp2CB,_tmp2D1,({const char*_tmp328="(not both zero terminated)";
_tag_dyneither(_tmp328,sizeof(char),27);}));if(_tmp2CA == _tmp2D0)return;if(
_tmp2CA == 0  || _tmp2D0 == 0)goto _LL218;if(Cyc_Evexp_same_const_exp((struct Cyc_Absyn_Exp*)
_tmp2CA,(struct Cyc_Absyn_Exp*)_tmp2D0))return;Cyc_Tcutil_failure_reason=({const
char*_tmp329="(different array sizes)";_tag_dyneither(_tmp329,sizeof(char),24);});
goto _LL218;_LL237: _tmp2D2=_tmp26E.f1;if(_tmp2D2 <= (void*)4)goto _LL239;if(*((int*)
_tmp2D2)!= 8)goto _LL239;_tmp2D3=((struct Cyc_Absyn_FnType_struct*)_tmp2D2)->f1;
_tmp2D4=_tmp2D3.tvars;_tmp2D5=_tmp2D3.effect;_tmp2D6=(void*)_tmp2D3.ret_typ;
_tmp2D7=_tmp2D3.args;_tmp2D8=_tmp2D3.c_varargs;_tmp2D9=_tmp2D3.cyc_varargs;
_tmp2DA=_tmp2D3.rgn_po;_tmp2DB=_tmp2D3.attributes;_tmp2DC=_tmp26E.f2;if(_tmp2DC
<= (void*)4)goto _LL239;if(*((int*)_tmp2DC)!= 8)goto _LL239;_tmp2DD=((struct Cyc_Absyn_FnType_struct*)
_tmp2DC)->f1;_tmp2DE=_tmp2DD.tvars;_tmp2DF=_tmp2DD.effect;_tmp2E0=(void*)_tmp2DD.ret_typ;
_tmp2E1=_tmp2DD.args;_tmp2E2=_tmp2DD.c_varargs;_tmp2E3=_tmp2DD.cyc_varargs;
_tmp2E4=_tmp2DD.rgn_po;_tmp2E5=_tmp2DD.attributes;_LL238: {int done=0;struct
_RegionHandle _tmp32A=_new_region("rgn");struct _RegionHandle*rgn=& _tmp32A;
_push_region(rgn);{struct Cyc_List_List*inst=0;while(_tmp2DE != 0){if(_tmp2D4 == 0){
Cyc_Tcutil_failure_reason=({const char*_tmp32B="(second function type has too few type variables)";
_tag_dyneither(_tmp32B,sizeof(char),50);});(int)_throw((void*)Cyc_Tcutil_Unify);}{
void*_tmp32C=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp2DE->hd);void*
_tmp32D=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp2D4->hd);if(_tmp32C != 
_tmp32D){Cyc_Tcutil_failure_reason=(struct _dyneither_ptr)({struct Cyc_String_pa_struct
_tmp332;_tmp332.tag=0;_tmp332.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kind2string(_tmp32D));{struct Cyc_String_pa_struct _tmp331;_tmp331.tag=
0;_tmp331.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(
_tmp32C));{struct Cyc_String_pa_struct _tmp330;_tmp330.tag=0;_tmp330.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_tvar2string((struct Cyc_Absyn_Tvar*)
_tmp2DE->hd));{void*_tmp32E[3]={& _tmp330,& _tmp331,& _tmp332};Cyc_aprintf(({const
char*_tmp32F="(type var %s has different kinds %s and %s)";_tag_dyneither(
_tmp32F,sizeof(char),44);}),_tag_dyneither(_tmp32E,sizeof(void*),3));}}}});(int)
_throw((void*)Cyc_Tcutil_Unify);}inst=({struct Cyc_List_List*_tmp333=
_region_malloc(rgn,sizeof(*_tmp333));_tmp333->hd=({struct _tuple8*_tmp334=
_region_malloc(rgn,sizeof(*_tmp334));_tmp334->f1=(struct Cyc_Absyn_Tvar*)_tmp2D4->hd;
_tmp334->f2=(void*)({struct Cyc_Absyn_VarType_struct*_tmp335=_cycalloc(sizeof(*
_tmp335));_tmp335[0]=({struct Cyc_Absyn_VarType_struct _tmp336;_tmp336.tag=1;
_tmp336.f1=(struct Cyc_Absyn_Tvar*)_tmp2DE->hd;_tmp336;});_tmp335;});_tmp334;});
_tmp333->tl=inst;_tmp333;});_tmp2DE=_tmp2DE->tl;_tmp2D4=_tmp2D4->tl;}}if(_tmp2D4
!= 0){Cyc_Tcutil_failure_reason=({const char*_tmp337="(second function type has too many type variables)";
_tag_dyneither(_tmp337,sizeof(char),51);});_npop_handler(0);goto _LL218;}if(inst
!= 0){Cyc_Tcutil_unify_it((void*)({struct Cyc_Absyn_FnType_struct*_tmp338=
_cycalloc(sizeof(*_tmp338));_tmp338[0]=({struct Cyc_Absyn_FnType_struct _tmp339;
_tmp339.tag=8;_tmp339.f1=({struct Cyc_Absyn_FnInfo _tmp33A;_tmp33A.tvars=0;_tmp33A.effect=
_tmp2DF;_tmp33A.ret_typ=(void*)_tmp2E0;_tmp33A.args=_tmp2E1;_tmp33A.c_varargs=
_tmp2E2;_tmp33A.cyc_varargs=_tmp2E3;_tmp33A.rgn_po=_tmp2E4;_tmp33A.attributes=
_tmp2E5;_tmp33A;});_tmp339;});_tmp338;}),Cyc_Tcutil_rsubstitute(rgn,inst,(void*)({
struct Cyc_Absyn_FnType_struct*_tmp33B=_cycalloc(sizeof(*_tmp33B));_tmp33B[0]=({
struct Cyc_Absyn_FnType_struct _tmp33C;_tmp33C.tag=8;_tmp33C.f1=({struct Cyc_Absyn_FnInfo
_tmp33D;_tmp33D.tvars=0;_tmp33D.effect=_tmp2D5;_tmp33D.ret_typ=(void*)_tmp2D6;
_tmp33D.args=_tmp2D7;_tmp33D.c_varargs=_tmp2D8;_tmp33D.cyc_varargs=_tmp2D9;
_tmp33D.rgn_po=_tmp2DA;_tmp33D.attributes=_tmp2DB;_tmp33D;});_tmp33C;});_tmp33B;})));
done=1;}}if(done){_npop_handler(0);return;}Cyc_Tcutil_unify_it(_tmp2E0,_tmp2D6);
for(0;_tmp2E1 != 0  && _tmp2D7 != 0;(_tmp2E1=_tmp2E1->tl,_tmp2D7=_tmp2D7->tl)){
struct Cyc_Absyn_Tqual _tmp33F;void*_tmp340;struct _tuple3 _tmp33E=*((struct _tuple3*)
_tmp2E1->hd);_tmp33F=_tmp33E.f2;_tmp340=_tmp33E.f3;{struct Cyc_Absyn_Tqual _tmp342;
void*_tmp343;struct _tuple3 _tmp341=*((struct _tuple3*)_tmp2D7->hd);_tmp342=_tmp341.f2;
_tmp343=_tmp341.f3;Cyc_Tcutil_unify_it(_tmp340,_tmp343);Cyc_Tcutil_unify_tqual(
_tmp33F,_tmp340,_tmp342,_tmp343);}}Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=
t2;if(_tmp2E1 != 0  || _tmp2D7 != 0){Cyc_Tcutil_failure_reason=({const char*_tmp344="(function types have different number of arguments)";
_tag_dyneither(_tmp344,sizeof(char),52);});_npop_handler(0);goto _LL218;}if(
_tmp2E2 != _tmp2D8){Cyc_Tcutil_failure_reason=({const char*_tmp345="(only one function type takes C varargs)";
_tag_dyneither(_tmp345,sizeof(char),41);});_npop_handler(0);goto _LL218;}{int
bad_cyc_vararg=0;{struct _tuple11 _tmp347=({struct _tuple11 _tmp346;_tmp346.f1=
_tmp2E3;_tmp346.f2=_tmp2D9;_tmp346;});struct Cyc_Absyn_VarargInfo*_tmp348;struct
Cyc_Absyn_VarargInfo*_tmp349;struct Cyc_Absyn_VarargInfo*_tmp34A;struct Cyc_Absyn_VarargInfo*
_tmp34B;struct Cyc_Absyn_VarargInfo*_tmp34C;struct Cyc_Absyn_VarargInfo _tmp34D;
struct Cyc_Core_Opt*_tmp34E;struct Cyc_Absyn_Tqual _tmp34F;void*_tmp350;int _tmp351;
struct Cyc_Absyn_VarargInfo*_tmp352;struct Cyc_Absyn_VarargInfo _tmp353;struct Cyc_Core_Opt*
_tmp354;struct Cyc_Absyn_Tqual _tmp355;void*_tmp356;int _tmp357;_LL259: _tmp348=
_tmp347.f1;if(_tmp348 != 0)goto _LL25B;_tmp349=_tmp347.f2;if(_tmp349 != 0)goto
_LL25B;_LL25A: goto _LL258;_LL25B: _tmp34A=_tmp347.f1;if(_tmp34A != 0)goto _LL25D;
_LL25C: goto _LL25E;_LL25D: _tmp34B=_tmp347.f2;if(_tmp34B != 0)goto _LL25F;_LL25E:
bad_cyc_vararg=1;Cyc_Tcutil_failure_reason=({const char*_tmp358="(only one function type takes varargs)";
_tag_dyneither(_tmp358,sizeof(char),39);});goto _LL258;_LL25F: _tmp34C=_tmp347.f1;
if(_tmp34C == 0)goto _LL258;_tmp34D=*_tmp34C;_tmp34E=_tmp34D.name;_tmp34F=_tmp34D.tq;
_tmp350=(void*)_tmp34D.type;_tmp351=_tmp34D.inject;_tmp352=_tmp347.f2;if(_tmp352
== 0)goto _LL258;_tmp353=*_tmp352;_tmp354=_tmp353.name;_tmp355=_tmp353.tq;_tmp356=(
void*)_tmp353.type;_tmp357=_tmp353.inject;_LL260: Cyc_Tcutil_unify_it(_tmp350,
_tmp356);Cyc_Tcutil_unify_tqual(_tmp34F,_tmp350,_tmp355,_tmp356);if(_tmp351 != 
_tmp357){bad_cyc_vararg=1;Cyc_Tcutil_failure_reason=({const char*_tmp359="(only one function type injects varargs)";
_tag_dyneither(_tmp359,sizeof(char),41);});}goto _LL258;_LL258:;}if(
bad_cyc_vararg){_npop_handler(0);goto _LL218;}{int bad_effect=0;{struct _tuple12
_tmp35B=({struct _tuple12 _tmp35A;_tmp35A.f1=_tmp2DF;_tmp35A.f2=_tmp2D5;_tmp35A;});
struct Cyc_Core_Opt*_tmp35C;struct Cyc_Core_Opt*_tmp35D;struct Cyc_Core_Opt*_tmp35E;
struct Cyc_Core_Opt*_tmp35F;_LL262: _tmp35C=_tmp35B.f1;if(_tmp35C != 0)goto _LL264;
_tmp35D=_tmp35B.f2;if(_tmp35D != 0)goto _LL264;_LL263: goto _LL261;_LL264: _tmp35E=
_tmp35B.f1;if(_tmp35E != 0)goto _LL266;_LL265: goto _LL267;_LL266: _tmp35F=_tmp35B.f2;
if(_tmp35F != 0)goto _LL268;_LL267: bad_effect=1;goto _LL261;_LL268:;_LL269:
bad_effect=!Cyc_Tcutil_unify_effect((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp2DF))->v,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp2D5))->v);goto _LL261;
_LL261:;}Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;if(bad_effect){Cyc_Tcutil_failure_reason=({
const char*_tmp360="(function type effects do not unify)";_tag_dyneither(_tmp360,
sizeof(char),37);});_npop_handler(0);goto _LL218;}if(!Cyc_Tcutil_same_atts(
_tmp2DB,_tmp2E5)){Cyc_Tcutil_failure_reason=({const char*_tmp361="(function types have different attributes)";
_tag_dyneither(_tmp361,sizeof(char),43);});_npop_handler(0);goto _LL218;}if(!Cyc_Tcutil_same_rgn_po(
_tmp2DA,_tmp2E4)){Cyc_Tcutil_failure_reason=({const char*_tmp362="(function types have different region lifetime orderings)";
_tag_dyneither(_tmp362,sizeof(char),58);});_npop_handler(0);goto _LL218;}
_npop_handler(0);return;}};_pop_region(rgn);}_LL239: _tmp2E6=_tmp26E.f1;if(
_tmp2E6 <= (void*)4)goto _LL23B;if(*((int*)_tmp2E6)!= 9)goto _LL23B;_tmp2E7=((
struct Cyc_Absyn_TupleType_struct*)_tmp2E6)->f1;_tmp2E8=_tmp26E.f2;if(_tmp2E8 <= (
void*)4)goto _LL23B;if(*((int*)_tmp2E8)!= 9)goto _LL23B;_tmp2E9=((struct Cyc_Absyn_TupleType_struct*)
_tmp2E8)->f1;_LL23A: for(0;_tmp2E9 != 0  && _tmp2E7 != 0;(_tmp2E9=_tmp2E9->tl,
_tmp2E7=_tmp2E7->tl)){struct Cyc_Absyn_Tqual _tmp364;void*_tmp365;struct _tuple5
_tmp363=*((struct _tuple5*)_tmp2E9->hd);_tmp364=_tmp363.f1;_tmp365=_tmp363.f2;{
struct Cyc_Absyn_Tqual _tmp367;void*_tmp368;struct _tuple5 _tmp366=*((struct _tuple5*)
_tmp2E7->hd);_tmp367=_tmp366.f1;_tmp368=_tmp366.f2;Cyc_Tcutil_unify_it(_tmp365,
_tmp368);Cyc_Tcutil_unify_tqual(_tmp364,_tmp365,_tmp367,_tmp368);}}if(_tmp2E9 == 
0  && _tmp2E7 == 0)return;Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;Cyc_Tcutil_failure_reason=({
const char*_tmp369="(tuple types have different numbers of components)";
_tag_dyneither(_tmp369,sizeof(char),51);});goto _LL218;_LL23B: _tmp2EA=_tmp26E.f1;
if(_tmp2EA <= (void*)4)goto _LL23D;if(*((int*)_tmp2EA)!= 11)goto _LL23D;_tmp2EB=(
void*)((struct Cyc_Absyn_AnonAggrType_struct*)_tmp2EA)->f1;_tmp2EC=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp2EA)->f2;_tmp2ED=_tmp26E.f2;if(_tmp2ED <= (void*)4)goto _LL23D;if(*((int*)
_tmp2ED)!= 11)goto _LL23D;_tmp2EE=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp2ED)->f1;_tmp2EF=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp2ED)->f2;_LL23C:
if(_tmp2EE != _tmp2EB){Cyc_Tcutil_failure_reason=({const char*_tmp36A="(struct and union type)";
_tag_dyneither(_tmp36A,sizeof(char),24);});goto _LL218;}for(0;_tmp2EF != 0  && 
_tmp2EC != 0;(_tmp2EF=_tmp2EF->tl,_tmp2EC=_tmp2EC->tl)){struct Cyc_Absyn_Aggrfield*
_tmp36B=(struct Cyc_Absyn_Aggrfield*)_tmp2EF->hd;struct Cyc_Absyn_Aggrfield*
_tmp36C=(struct Cyc_Absyn_Aggrfield*)_tmp2EC->hd;if(Cyc_strptrcmp(_tmp36B->name,
_tmp36C->name)!= 0){Cyc_Tcutil_failure_reason=({const char*_tmp36D="(different member names)";
_tag_dyneither(_tmp36D,sizeof(char),25);});(int)_throw((void*)Cyc_Tcutil_Unify);}
Cyc_Tcutil_unify_it((void*)_tmp36B->type,(void*)_tmp36C->type);Cyc_Tcutil_unify_tqual(
_tmp36B->tq,(void*)_tmp36B->type,_tmp36C->tq,(void*)_tmp36C->type);if(!Cyc_Tcutil_same_atts(
_tmp36B->attributes,_tmp36C->attributes)){Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=
t2;Cyc_Tcutil_failure_reason=({const char*_tmp36E="(different attributes on member)";
_tag_dyneither(_tmp36E,sizeof(char),33);});(int)_throw((void*)Cyc_Tcutil_Unify);}
if((_tmp36B->width != 0  && _tmp36C->width == 0  || _tmp36C->width != 0  && _tmp36B->width
== 0) || (_tmp36B->width != 0  && _tmp36C->width != 0) && !Cyc_Evexp_same_const_exp((
struct Cyc_Absyn_Exp*)_check_null(_tmp36B->width),(struct Cyc_Absyn_Exp*)
_check_null(_tmp36C->width))){Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;
Cyc_Tcutil_failure_reason=({const char*_tmp36F="(different bitfield widths on member)";
_tag_dyneither(_tmp36F,sizeof(char),38);});(int)_throw((void*)Cyc_Tcutil_Unify);}}
if(_tmp2EF == 0  && _tmp2EC == 0)return;Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=
t2;Cyc_Tcutil_failure_reason=({const char*_tmp370="(different number of members)";
_tag_dyneither(_tmp370,sizeof(char),30);});goto _LL218;_LL23D: _tmp2F0=_tmp26E.f1;
if((int)_tmp2F0 != 2)goto _LL23F;_tmp2F1=_tmp26E.f2;if((int)_tmp2F1 != 2)goto _LL23F;
_LL23E: return;_LL23F: _tmp2F2=_tmp26E.f1;if((int)_tmp2F2 != 3)goto _LL241;_tmp2F3=
_tmp26E.f2;if((int)_tmp2F3 != 3)goto _LL241;_LL240: return;_LL241: _tmp2F4=_tmp26E.f1;
if(_tmp2F4 <= (void*)4)goto _LL243;if(*((int*)_tmp2F4)!= 14)goto _LL243;_tmp2F5=(
void*)((struct Cyc_Absyn_RgnHandleType_struct*)_tmp2F4)->f1;_tmp2F6=_tmp26E.f2;
if(_tmp2F6 <= (void*)4)goto _LL243;if(*((int*)_tmp2F6)!= 14)goto _LL243;_tmp2F7=(
void*)((struct Cyc_Absyn_RgnHandleType_struct*)_tmp2F6)->f1;_LL242: Cyc_Tcutil_unify_it(
_tmp2F5,_tmp2F7);return;_LL243: _tmp2F8=_tmp26E.f1;if(_tmp2F8 <= (void*)4)goto
_LL245;if(*((int*)_tmp2F8)!= 15)goto _LL245;_tmp2F9=(void*)((struct Cyc_Absyn_DynRgnType_struct*)
_tmp2F8)->f1;_tmp2FA=(void*)((struct Cyc_Absyn_DynRgnType_struct*)_tmp2F8)->f2;
_tmp2FB=_tmp26E.f2;if(_tmp2FB <= (void*)4)goto _LL245;if(*((int*)_tmp2FB)!= 15)
goto _LL245;_tmp2FC=(void*)((struct Cyc_Absyn_DynRgnType_struct*)_tmp2FB)->f1;
_tmp2FD=(void*)((struct Cyc_Absyn_DynRgnType_struct*)_tmp2FB)->f2;_LL244: Cyc_Tcutil_unify_it(
_tmp2F9,_tmp2FC);Cyc_Tcutil_unify_it(_tmp2FA,_tmp2FD);return;_LL245: _tmp2FE=
_tmp26E.f1;if(_tmp2FE <= (void*)4)goto _LL247;if(*((int*)_tmp2FE)!= 20)goto _LL247;
_LL246: goto _LL248;_LL247: _tmp2FF=_tmp26E.f2;if(_tmp2FF <= (void*)4)goto _LL249;if(*((
int*)_tmp2FF)!= 20)goto _LL249;_LL248: goto _LL24A;_LL249: _tmp300=_tmp26E.f1;if(
_tmp300 <= (void*)4)goto _LL24B;if(*((int*)_tmp300)!= 19)goto _LL24B;_LL24A: goto
_LL24C;_LL24B: _tmp301=_tmp26E.f1;if(_tmp301 <= (void*)4)goto _LL24D;if(*((int*)
_tmp301)!= 21)goto _LL24D;_LL24C: goto _LL24E;_LL24D: _tmp302=_tmp26E.f2;if(_tmp302
<= (void*)4)goto _LL24F;if(*((int*)_tmp302)!= 21)goto _LL24F;_LL24E: goto _LL250;
_LL24F: _tmp303=_tmp26E.f2;if(_tmp303 <= (void*)4)goto _LL251;if(*((int*)_tmp303)!= 
19)goto _LL251;_LL250: if(Cyc_Tcutil_unify_effect(t1,t2))return;Cyc_Tcutil_failure_reason=({
const char*_tmp371="(effects don't unify)";_tag_dyneither(_tmp371,sizeof(char),22);});
goto _LL218;_LL251:;_LL252: goto _LL218;_LL218:;}(int)_throw((void*)Cyc_Tcutil_Unify);}
int Cyc_Tcutil_star_cmp(int(*cmp)(void*,void*),void*a1,void*a2){if(a1 == a2)return
0;if(a1 == 0  && a2 != 0)return - 1;if(a1 != 0  && a2 == 0)return 1;return cmp((void*)
_check_null(a1),(void*)_check_null(a2));}static int Cyc_Tcutil_tqual_cmp(struct Cyc_Absyn_Tqual
tq1,struct Cyc_Absyn_Tqual tq2){int _tmp372=(tq1.real_const + (tq1.q_volatile << 1))+ (
tq1.q_restrict << 2);int _tmp373=(tq2.real_const + (tq2.q_volatile << 1))+ (tq2.q_restrict
<< 2);return Cyc_Core_intcmp(_tmp372,_tmp373);}static int Cyc_Tcutil_conrefs_cmp(
int(*cmp)(void*,void*),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y){x=Cyc_Absyn_compress_conref(
x);y=Cyc_Absyn_compress_conref(y);if(x == y)return 0;{union Cyc_Absyn_Constraint_union
_tmp374=x->v;void*_tmp375;_LL26B: if((_tmp374.No_constr).tag != 2)goto _LL26D;
_LL26C: return - 1;_LL26D: if((_tmp374.Eq_constr).tag != 0)goto _LL26F;_tmp375=(
_tmp374.Eq_constr).f1;_LL26E: {union Cyc_Absyn_Constraint_union _tmp376=y->v;void*
_tmp377;_LL272: if((_tmp376.No_constr).tag != 2)goto _LL274;_LL273: return 1;_LL274:
if((_tmp376.Eq_constr).tag != 0)goto _LL276;_tmp377=(_tmp376.Eq_constr).f1;_LL275:
return cmp(_tmp375,_tmp377);_LL276: if((_tmp376.Forward_constr).tag != 1)goto _LL271;
_LL277:({void*_tmp378=0;Cyc_Tcutil_impos(({const char*_tmp379="unify_conref: forward after compress(2)";
_tag_dyneither(_tmp379,sizeof(char),40);}),_tag_dyneither(_tmp378,sizeof(void*),
0));});_LL271:;}_LL26F: if((_tmp374.Forward_constr).tag != 1)goto _LL26A;_LL270:({
void*_tmp37A=0;Cyc_Tcutil_impos(({const char*_tmp37B="unify_conref: forward after compress";
_tag_dyneither(_tmp37B,sizeof(char),37);}),_tag_dyneither(_tmp37A,sizeof(void*),
0));});_LL26A:;}}static int Cyc_Tcutil_tqual_type_cmp(struct _tuple5*tqt1,struct
_tuple5*tqt2){struct _tuple5 _tmp37D;struct Cyc_Absyn_Tqual _tmp37E;void*_tmp37F;
struct _tuple5*_tmp37C=tqt1;_tmp37D=*_tmp37C;_tmp37E=_tmp37D.f1;_tmp37F=_tmp37D.f2;{
struct _tuple5 _tmp381;struct Cyc_Absyn_Tqual _tmp382;void*_tmp383;struct _tuple5*
_tmp380=tqt2;_tmp381=*_tmp380;_tmp382=_tmp381.f1;_tmp383=_tmp381.f2;{int _tmp384=
Cyc_Tcutil_tqual_cmp(_tmp37E,_tmp382);if(_tmp384 != 0)return _tmp384;return Cyc_Tcutil_typecmp(
_tmp37F,_tmp383);}}}static int Cyc_Tcutil_aggrfield_cmp(struct Cyc_Absyn_Aggrfield*
f1,struct Cyc_Absyn_Aggrfield*f2){int _tmp385=Cyc_strptrcmp(f1->name,f2->name);if(
_tmp385 != 0)return _tmp385;{int _tmp386=Cyc_Tcutil_tqual_cmp(f1->tq,f2->tq);if(
_tmp386 != 0)return _tmp386;{int _tmp387=Cyc_Tcutil_typecmp((void*)f1->type,(void*)
f2->type);if(_tmp387 != 0)return _tmp387;{int _tmp388=Cyc_List_list_cmp(Cyc_Tcutil_attribute_cmp,
f1->attributes,f2->attributes);if(_tmp388 != 0)return _tmp388;return((int(*)(int(*
cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*
a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,f1->width,f2->width);}}}}static
int Cyc_Tcutil_enumfield_cmp(struct Cyc_Absyn_Enumfield*e1,struct Cyc_Absyn_Enumfield*
e2){int _tmp389=Cyc_Absyn_qvar_cmp(e1->name,e2->name);if(_tmp389 != 0)return
_tmp389;return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),
struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,
e1->tag,e2->tag);}static int Cyc_Tcutil_type_case_number(void*t){void*_tmp38A=t;
_LL279: if((int)_tmp38A != 0)goto _LL27B;_LL27A: return 0;_LL27B: if(_tmp38A <= (void*)
4)goto _LL287;if(*((int*)_tmp38A)!= 0)goto _LL27D;_LL27C: return 1;_LL27D: if(*((int*)
_tmp38A)!= 1)goto _LL27F;_LL27E: return 2;_LL27F: if(*((int*)_tmp38A)!= 2)goto _LL281;
_LL280: return 3;_LL281: if(*((int*)_tmp38A)!= 3)goto _LL283;_LL282: return 4;_LL283:
if(*((int*)_tmp38A)!= 4)goto _LL285;_LL284: return 5;_LL285: if(*((int*)_tmp38A)!= 5)
goto _LL287;_LL286: return 6;_LL287: if((int)_tmp38A != 1)goto _LL289;_LL288: return 7;
_LL289: if(_tmp38A <= (void*)4)goto _LL29D;if(*((int*)_tmp38A)!= 6)goto _LL28B;
_LL28A: return 8;_LL28B: if(*((int*)_tmp38A)!= 7)goto _LL28D;_LL28C: return 9;_LL28D:
if(*((int*)_tmp38A)!= 8)goto _LL28F;_LL28E: return 10;_LL28F: if(*((int*)_tmp38A)!= 
9)goto _LL291;_LL290: return 11;_LL291: if(*((int*)_tmp38A)!= 10)goto _LL293;_LL292:
return 12;_LL293: if(*((int*)_tmp38A)!= 11)goto _LL295;_LL294: return 14;_LL295: if(*((
int*)_tmp38A)!= 12)goto _LL297;_LL296: return 16;_LL297: if(*((int*)_tmp38A)!= 13)
goto _LL299;_LL298: return 17;_LL299: if(*((int*)_tmp38A)!= 14)goto _LL29B;_LL29A:
return 18;_LL29B: if(*((int*)_tmp38A)!= 16)goto _LL29D;_LL29C: return 19;_LL29D: if((
int)_tmp38A != 3)goto _LL29F;_LL29E: return 20;_LL29F: if((int)_tmp38A != 2)goto _LL2A1;
_LL2A0: return 21;_LL2A1: if(_tmp38A <= (void*)4)goto _LL2A3;if(*((int*)_tmp38A)!= 19)
goto _LL2A3;_LL2A2: return 22;_LL2A3: if(_tmp38A <= (void*)4)goto _LL2A5;if(*((int*)
_tmp38A)!= 20)goto _LL2A5;_LL2A4: return 23;_LL2A5: if(_tmp38A <= (void*)4)goto _LL2A7;
if(*((int*)_tmp38A)!= 21)goto _LL2A7;_LL2A6: return 24;_LL2A7: if(_tmp38A <= (void*)4)
goto _LL2A9;if(*((int*)_tmp38A)!= 18)goto _LL2A9;_LL2A8: return 26;_LL2A9: if(_tmp38A
<= (void*)4)goto _LL2AB;if(*((int*)_tmp38A)!= 15)goto _LL2AB;_LL2AA: return 27;
_LL2AB: if(_tmp38A <= (void*)4)goto _LL278;if(*((int*)_tmp38A)!= 17)goto _LL278;
_LL2AC: return 28;_LL278:;}int Cyc_Tcutil_typecmp(void*t1,void*t2){t1=Cyc_Tcutil_compress(
t1);t2=Cyc_Tcutil_compress(t2);if(t1 == t2)return 0;{int _tmp38B=Cyc_Core_intcmp(
Cyc_Tcutil_type_case_number(t1),Cyc_Tcutil_type_case_number(t2));if(_tmp38B != 0)
return _tmp38B;{struct _tuple0 _tmp38D=({struct _tuple0 _tmp38C;_tmp38C.f1=t2;_tmp38C.f2=
t1;_tmp38C;});void*_tmp38E;void*_tmp38F;void*_tmp390;struct Cyc_Absyn_Tvar*
_tmp391;void*_tmp392;struct Cyc_Absyn_Tvar*_tmp393;void*_tmp394;struct Cyc_Absyn_AggrInfo
_tmp395;union Cyc_Absyn_AggrInfoU_union _tmp396;struct Cyc_List_List*_tmp397;void*
_tmp398;struct Cyc_Absyn_AggrInfo _tmp399;union Cyc_Absyn_AggrInfoU_union _tmp39A;
struct Cyc_List_List*_tmp39B;void*_tmp39C;struct _tuple2*_tmp39D;void*_tmp39E;
struct _tuple2*_tmp39F;void*_tmp3A0;struct Cyc_List_List*_tmp3A1;void*_tmp3A2;
struct Cyc_List_List*_tmp3A3;void*_tmp3A4;struct Cyc_Absyn_TunionInfo _tmp3A5;union
Cyc_Absyn_TunionInfoU_union _tmp3A6;struct Cyc_Absyn_Tuniondecl**_tmp3A7;struct Cyc_Absyn_Tuniondecl*
_tmp3A8;struct Cyc_List_List*_tmp3A9;struct Cyc_Core_Opt*_tmp3AA;void*_tmp3AB;
struct Cyc_Absyn_TunionInfo _tmp3AC;union Cyc_Absyn_TunionInfoU_union _tmp3AD;struct
Cyc_Absyn_Tuniondecl**_tmp3AE;struct Cyc_Absyn_Tuniondecl*_tmp3AF;struct Cyc_List_List*
_tmp3B0;struct Cyc_Core_Opt*_tmp3B1;void*_tmp3B2;struct Cyc_Absyn_TunionFieldInfo
_tmp3B3;union Cyc_Absyn_TunionFieldInfoU_union _tmp3B4;struct Cyc_Absyn_Tuniondecl*
_tmp3B5;struct Cyc_Absyn_Tunionfield*_tmp3B6;struct Cyc_List_List*_tmp3B7;void*
_tmp3B8;struct Cyc_Absyn_TunionFieldInfo _tmp3B9;union Cyc_Absyn_TunionFieldInfoU_union
_tmp3BA;struct Cyc_Absyn_Tuniondecl*_tmp3BB;struct Cyc_Absyn_Tunionfield*_tmp3BC;
struct Cyc_List_List*_tmp3BD;void*_tmp3BE;struct Cyc_Absyn_PtrInfo _tmp3BF;void*
_tmp3C0;struct Cyc_Absyn_Tqual _tmp3C1;struct Cyc_Absyn_PtrAtts _tmp3C2;void*_tmp3C3;
struct Cyc_Absyn_Conref*_tmp3C4;struct Cyc_Absyn_Conref*_tmp3C5;struct Cyc_Absyn_Conref*
_tmp3C6;void*_tmp3C7;struct Cyc_Absyn_PtrInfo _tmp3C8;void*_tmp3C9;struct Cyc_Absyn_Tqual
_tmp3CA;struct Cyc_Absyn_PtrAtts _tmp3CB;void*_tmp3CC;struct Cyc_Absyn_Conref*
_tmp3CD;struct Cyc_Absyn_Conref*_tmp3CE;struct Cyc_Absyn_Conref*_tmp3CF;void*
_tmp3D0;void*_tmp3D1;void*_tmp3D2;void*_tmp3D3;void*_tmp3D4;void*_tmp3D5;void*
_tmp3D6;void*_tmp3D7;void*_tmp3D8;int _tmp3D9;void*_tmp3DA;int _tmp3DB;void*
_tmp3DC;struct Cyc_Absyn_ArrayInfo _tmp3DD;void*_tmp3DE;struct Cyc_Absyn_Tqual
_tmp3DF;struct Cyc_Absyn_Exp*_tmp3E0;struct Cyc_Absyn_Conref*_tmp3E1;void*_tmp3E2;
struct Cyc_Absyn_ArrayInfo _tmp3E3;void*_tmp3E4;struct Cyc_Absyn_Tqual _tmp3E5;
struct Cyc_Absyn_Exp*_tmp3E6;struct Cyc_Absyn_Conref*_tmp3E7;void*_tmp3E8;struct
Cyc_Absyn_FnInfo _tmp3E9;struct Cyc_List_List*_tmp3EA;struct Cyc_Core_Opt*_tmp3EB;
void*_tmp3EC;struct Cyc_List_List*_tmp3ED;int _tmp3EE;struct Cyc_Absyn_VarargInfo*
_tmp3EF;struct Cyc_List_List*_tmp3F0;struct Cyc_List_List*_tmp3F1;void*_tmp3F2;
struct Cyc_Absyn_FnInfo _tmp3F3;struct Cyc_List_List*_tmp3F4;struct Cyc_Core_Opt*
_tmp3F5;void*_tmp3F6;struct Cyc_List_List*_tmp3F7;int _tmp3F8;struct Cyc_Absyn_VarargInfo*
_tmp3F9;struct Cyc_List_List*_tmp3FA;struct Cyc_List_List*_tmp3FB;void*_tmp3FC;
struct Cyc_List_List*_tmp3FD;void*_tmp3FE;struct Cyc_List_List*_tmp3FF;void*
_tmp400;void*_tmp401;struct Cyc_List_List*_tmp402;void*_tmp403;void*_tmp404;
struct Cyc_List_List*_tmp405;void*_tmp406;void*_tmp407;void*_tmp408;void*_tmp409;
void*_tmp40A;void*_tmp40B;void*_tmp40C;void*_tmp40D;void*_tmp40E;void*_tmp40F;
void*_tmp410;void*_tmp411;void*_tmp412;void*_tmp413;void*_tmp414;struct Cyc_Absyn_Exp*
_tmp415;void*_tmp416;struct Cyc_Absyn_Exp*_tmp417;void*_tmp418;void*_tmp419;void*
_tmp41A;void*_tmp41B;void*_tmp41C;void*_tmp41D;_LL2AE: _tmp38E=_tmp38D.f1;if(
_tmp38E <= (void*)4)goto _LL2B0;if(*((int*)_tmp38E)!= 0)goto _LL2B0;_tmp38F=_tmp38D.f2;
if(_tmp38F <= (void*)4)goto _LL2B0;if(*((int*)_tmp38F)!= 0)goto _LL2B0;_LL2AF:({
void*_tmp41E=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp41F="typecmp: can only compare closed types";_tag_dyneither(
_tmp41F,sizeof(char),39);}),_tag_dyneither(_tmp41E,sizeof(void*),0));});_LL2B0:
_tmp390=_tmp38D.f1;if(_tmp390 <= (void*)4)goto _LL2B2;if(*((int*)_tmp390)!= 1)goto
_LL2B2;_tmp391=((struct Cyc_Absyn_VarType_struct*)_tmp390)->f1;_tmp392=_tmp38D.f2;
if(_tmp392 <= (void*)4)goto _LL2B2;if(*((int*)_tmp392)!= 1)goto _LL2B2;_tmp393=((
struct Cyc_Absyn_VarType_struct*)_tmp392)->f1;_LL2B1: return Cyc_Core_intcmp(
_tmp393->identity,_tmp391->identity);_LL2B2: _tmp394=_tmp38D.f1;if(_tmp394 <= (
void*)4)goto _LL2B4;if(*((int*)_tmp394)!= 10)goto _LL2B4;_tmp395=((struct Cyc_Absyn_AggrType_struct*)
_tmp394)->f1;_tmp396=_tmp395.aggr_info;_tmp397=_tmp395.targs;_tmp398=_tmp38D.f2;
if(_tmp398 <= (void*)4)goto _LL2B4;if(*((int*)_tmp398)!= 10)goto _LL2B4;_tmp399=((
struct Cyc_Absyn_AggrType_struct*)_tmp398)->f1;_tmp39A=_tmp399.aggr_info;_tmp39B=
_tmp399.targs;_LL2B3: {struct _tuple2*_tmp421;struct _tuple6 _tmp420=Cyc_Absyn_aggr_kinded_name(
_tmp396);_tmp421=_tmp420.f2;{struct _tuple2*_tmp423;struct _tuple6 _tmp422=Cyc_Absyn_aggr_kinded_name(
_tmp39A);_tmp423=_tmp422.f2;{int _tmp424=Cyc_Absyn_qvar_cmp(_tmp421,_tmp423);if(
_tmp424 != 0)return _tmp424;else{return Cyc_List_list_cmp(Cyc_Tcutil_typecmp,
_tmp397,_tmp39B);}}}}_LL2B4: _tmp39C=_tmp38D.f1;if(_tmp39C <= (void*)4)goto _LL2B6;
if(*((int*)_tmp39C)!= 12)goto _LL2B6;_tmp39D=((struct Cyc_Absyn_EnumType_struct*)
_tmp39C)->f1;_tmp39E=_tmp38D.f2;if(_tmp39E <= (void*)4)goto _LL2B6;if(*((int*)
_tmp39E)!= 12)goto _LL2B6;_tmp39F=((struct Cyc_Absyn_EnumType_struct*)_tmp39E)->f1;
_LL2B5: return Cyc_Absyn_qvar_cmp(_tmp39D,_tmp39F);_LL2B6: _tmp3A0=_tmp38D.f1;if(
_tmp3A0 <= (void*)4)goto _LL2B8;if(*((int*)_tmp3A0)!= 13)goto _LL2B8;_tmp3A1=((
struct Cyc_Absyn_AnonEnumType_struct*)_tmp3A0)->f1;_tmp3A2=_tmp38D.f2;if(_tmp3A2
<= (void*)4)goto _LL2B8;if(*((int*)_tmp3A2)!= 13)goto _LL2B8;_tmp3A3=((struct Cyc_Absyn_AnonEnumType_struct*)
_tmp3A2)->f1;_LL2B7: return((int(*)(int(*cmp)(struct Cyc_Absyn_Enumfield*,struct
Cyc_Absyn_Enumfield*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(
Cyc_Tcutil_enumfield_cmp,_tmp3A1,_tmp3A3);_LL2B8: _tmp3A4=_tmp38D.f1;if(_tmp3A4 <= (
void*)4)goto _LL2BA;if(*((int*)_tmp3A4)!= 2)goto _LL2BA;_tmp3A5=((struct Cyc_Absyn_TunionType_struct*)
_tmp3A4)->f1;_tmp3A6=_tmp3A5.tunion_info;if((((((struct Cyc_Absyn_TunionType_struct*)
_tmp3A4)->f1).tunion_info).KnownTunion).tag != 1)goto _LL2BA;_tmp3A7=(_tmp3A6.KnownTunion).f1;
_tmp3A8=*_tmp3A7;_tmp3A9=_tmp3A5.targs;_tmp3AA=_tmp3A5.rgn;_tmp3AB=_tmp38D.f2;
if(_tmp3AB <= (void*)4)goto _LL2BA;if(*((int*)_tmp3AB)!= 2)goto _LL2BA;_tmp3AC=((
struct Cyc_Absyn_TunionType_struct*)_tmp3AB)->f1;_tmp3AD=_tmp3AC.tunion_info;if((((((
struct Cyc_Absyn_TunionType_struct*)_tmp3AB)->f1).tunion_info).KnownTunion).tag != 
1)goto _LL2BA;_tmp3AE=(_tmp3AD.KnownTunion).f1;_tmp3AF=*_tmp3AE;_tmp3B0=_tmp3AC.targs;
_tmp3B1=_tmp3AC.rgn;_LL2B9: if(_tmp3AF == _tmp3A8)return 0;{int _tmp425=Cyc_Absyn_qvar_cmp(
_tmp3AF->name,_tmp3A8->name);if(_tmp425 != 0)return _tmp425;if((unsigned int)
_tmp3B1  && (unsigned int)_tmp3AA){int _tmp426=Cyc_Tcutil_typecmp((void*)_tmp3B1->v,(
void*)_tmp3AA->v);if(_tmp426 != 0)return _tmp426;}else{if((unsigned int)_tmp3B1)
return - 1;else{return 1;}}return Cyc_List_list_cmp(Cyc_Tcutil_typecmp,_tmp3B0,
_tmp3A9);}_LL2BA: _tmp3B2=_tmp38D.f1;if(_tmp3B2 <= (void*)4)goto _LL2BC;if(*((int*)
_tmp3B2)!= 3)goto _LL2BC;_tmp3B3=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp3B2)->f1;_tmp3B4=_tmp3B3.field_info;if((((((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp3B2)->f1).field_info).KnownTunionfield).tag != 1)goto _LL2BC;_tmp3B5=(_tmp3B4.KnownTunionfield).f1;
_tmp3B6=(_tmp3B4.KnownTunionfield).f2;_tmp3B7=_tmp3B3.targs;_tmp3B8=_tmp38D.f2;
if(_tmp3B8 <= (void*)4)goto _LL2BC;if(*((int*)_tmp3B8)!= 3)goto _LL2BC;_tmp3B9=((
struct Cyc_Absyn_TunionFieldType_struct*)_tmp3B8)->f1;_tmp3BA=_tmp3B9.field_info;
if((((((struct Cyc_Absyn_TunionFieldType_struct*)_tmp3B8)->f1).field_info).KnownTunionfield).tag
!= 1)goto _LL2BC;_tmp3BB=(_tmp3BA.KnownTunionfield).f1;_tmp3BC=(_tmp3BA.KnownTunionfield).f2;
_tmp3BD=_tmp3B9.targs;_LL2BB: if(_tmp3BB == _tmp3B5)return 0;{int _tmp427=Cyc_Absyn_qvar_cmp(
_tmp3B5->name,_tmp3BB->name);if(_tmp427 != 0)return _tmp427;{int _tmp428=Cyc_Absyn_qvar_cmp(
_tmp3B6->name,_tmp3BC->name);if(_tmp428 != 0)return _tmp428;return Cyc_List_list_cmp(
Cyc_Tcutil_typecmp,_tmp3BD,_tmp3B7);}}_LL2BC: _tmp3BE=_tmp38D.f1;if(_tmp3BE <= (
void*)4)goto _LL2BE;if(*((int*)_tmp3BE)!= 4)goto _LL2BE;_tmp3BF=((struct Cyc_Absyn_PointerType_struct*)
_tmp3BE)->f1;_tmp3C0=(void*)_tmp3BF.elt_typ;_tmp3C1=_tmp3BF.elt_tq;_tmp3C2=
_tmp3BF.ptr_atts;_tmp3C3=(void*)_tmp3C2.rgn;_tmp3C4=_tmp3C2.nullable;_tmp3C5=
_tmp3C2.bounds;_tmp3C6=_tmp3C2.zero_term;_tmp3C7=_tmp38D.f2;if(_tmp3C7 <= (void*)
4)goto _LL2BE;if(*((int*)_tmp3C7)!= 4)goto _LL2BE;_tmp3C8=((struct Cyc_Absyn_PointerType_struct*)
_tmp3C7)->f1;_tmp3C9=(void*)_tmp3C8.elt_typ;_tmp3CA=_tmp3C8.elt_tq;_tmp3CB=
_tmp3C8.ptr_atts;_tmp3CC=(void*)_tmp3CB.rgn;_tmp3CD=_tmp3CB.nullable;_tmp3CE=
_tmp3CB.bounds;_tmp3CF=_tmp3CB.zero_term;_LL2BD: {int _tmp429=Cyc_Tcutil_typecmp(
_tmp3C9,_tmp3C0);if(_tmp429 != 0)return _tmp429;{int _tmp42A=Cyc_Tcutil_typecmp(
_tmp3CC,_tmp3C3);if(_tmp42A != 0)return _tmp42A;{int _tmp42B=Cyc_Tcutil_tqual_cmp(
_tmp3CA,_tmp3C1);if(_tmp42B != 0)return _tmp42B;{int _tmp42C=Cyc_Tcutil_conrefs_cmp(
Cyc_Tcutil_boundscmp,_tmp3CE,_tmp3C5);if(_tmp42C != 0)return _tmp42C;{int _tmp42D=((
int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_conrefs_cmp)(
Cyc_Core_intcmp,_tmp3CF,_tmp3C6);if(_tmp42D != 0)return _tmp42D;{union Cyc_Absyn_Constraint_union
_tmp42E=(Cyc_Absyn_compress_conref(_tmp3CE))->v;void*_tmp42F;_LL2E3: if((_tmp42E.Eq_constr).tag
!= 0)goto _LL2E5;_tmp42F=(_tmp42E.Eq_constr).f1;if((int)_tmp42F != 0)goto _LL2E5;
_LL2E4: return 0;_LL2E5:;_LL2E6: goto _LL2E2;_LL2E2:;}return((int(*)(int(*cmp)(int,
int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_conrefs_cmp)(
Cyc_Core_intcmp,_tmp3CD,_tmp3C4);}}}}}_LL2BE: _tmp3D0=_tmp38D.f1;if(_tmp3D0 <= (
void*)4)goto _LL2C0;if(*((int*)_tmp3D0)!= 5)goto _LL2C0;_tmp3D1=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp3D0)->f1;_tmp3D2=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp3D0)->f2;
_tmp3D3=_tmp38D.f2;if(_tmp3D3 <= (void*)4)goto _LL2C0;if(*((int*)_tmp3D3)!= 5)goto
_LL2C0;_tmp3D4=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp3D3)->f1;_tmp3D5=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp3D3)->f2;_LL2BF: if(_tmp3D4 != _tmp3D1)
return Cyc_Core_intcmp((int)((unsigned int)_tmp3D4),(int)((unsigned int)_tmp3D1));
if(_tmp3D5 != _tmp3D2)return Cyc_Core_intcmp((int)((unsigned int)_tmp3D5),(int)((
unsigned int)_tmp3D2));return 0;_LL2C0: _tmp3D6=_tmp38D.f1;if((int)_tmp3D6 != 1)
goto _LL2C2;_tmp3D7=_tmp38D.f2;if((int)_tmp3D7 != 1)goto _LL2C2;_LL2C1: return 0;
_LL2C2: _tmp3D8=_tmp38D.f1;if(_tmp3D8 <= (void*)4)goto _LL2C4;if(*((int*)_tmp3D8)!= 
6)goto _LL2C4;_tmp3D9=((struct Cyc_Absyn_DoubleType_struct*)_tmp3D8)->f1;_tmp3DA=
_tmp38D.f2;if(_tmp3DA <= (void*)4)goto _LL2C4;if(*((int*)_tmp3DA)!= 6)goto _LL2C4;
_tmp3DB=((struct Cyc_Absyn_DoubleType_struct*)_tmp3DA)->f1;_LL2C3: if(_tmp3D9 == 
_tmp3DB)return 0;else{if(_tmp3D9)return - 1;else{return 1;}}_LL2C4: _tmp3DC=_tmp38D.f1;
if(_tmp3DC <= (void*)4)goto _LL2C6;if(*((int*)_tmp3DC)!= 7)goto _LL2C6;_tmp3DD=((
struct Cyc_Absyn_ArrayType_struct*)_tmp3DC)->f1;_tmp3DE=(void*)_tmp3DD.elt_type;
_tmp3DF=_tmp3DD.tq;_tmp3E0=_tmp3DD.num_elts;_tmp3E1=_tmp3DD.zero_term;_tmp3E2=
_tmp38D.f2;if(_tmp3E2 <= (void*)4)goto _LL2C6;if(*((int*)_tmp3E2)!= 7)goto _LL2C6;
_tmp3E3=((struct Cyc_Absyn_ArrayType_struct*)_tmp3E2)->f1;_tmp3E4=(void*)_tmp3E3.elt_type;
_tmp3E5=_tmp3E3.tq;_tmp3E6=_tmp3E3.num_elts;_tmp3E7=_tmp3E3.zero_term;_LL2C5: {
int _tmp430=Cyc_Tcutil_tqual_cmp(_tmp3E5,_tmp3DF);if(_tmp430 != 0)return _tmp430;{
int _tmp431=Cyc_Tcutil_typecmp(_tmp3E4,_tmp3DE);if(_tmp431 != 0)return _tmp431;{int
_tmp432=((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*
y))Cyc_Tcutil_conrefs_cmp)(Cyc_Core_intcmp,_tmp3E1,_tmp3E7);if(_tmp432 != 0)
return _tmp432;if(_tmp3E0 == _tmp3E6)return 0;if(_tmp3E0 == 0  || _tmp3E6 == 0)({void*
_tmp433=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp434="missing expression in array index";_tag_dyneither(_tmp434,
sizeof(char),34);}),_tag_dyneither(_tmp433,sizeof(void*),0));});return((int(*)(
int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,
struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,_tmp3E0,
_tmp3E6);}}}_LL2C6: _tmp3E8=_tmp38D.f1;if(_tmp3E8 <= (void*)4)goto _LL2C8;if(*((int*)
_tmp3E8)!= 8)goto _LL2C8;_tmp3E9=((struct Cyc_Absyn_FnType_struct*)_tmp3E8)->f1;
_tmp3EA=_tmp3E9.tvars;_tmp3EB=_tmp3E9.effect;_tmp3EC=(void*)_tmp3E9.ret_typ;
_tmp3ED=_tmp3E9.args;_tmp3EE=_tmp3E9.c_varargs;_tmp3EF=_tmp3E9.cyc_varargs;
_tmp3F0=_tmp3E9.rgn_po;_tmp3F1=_tmp3E9.attributes;_tmp3F2=_tmp38D.f2;if(_tmp3F2
<= (void*)4)goto _LL2C8;if(*((int*)_tmp3F2)!= 8)goto _LL2C8;_tmp3F3=((struct Cyc_Absyn_FnType_struct*)
_tmp3F2)->f1;_tmp3F4=_tmp3F3.tvars;_tmp3F5=_tmp3F3.effect;_tmp3F6=(void*)_tmp3F3.ret_typ;
_tmp3F7=_tmp3F3.args;_tmp3F8=_tmp3F3.c_varargs;_tmp3F9=_tmp3F3.cyc_varargs;
_tmp3FA=_tmp3F3.rgn_po;_tmp3FB=_tmp3F3.attributes;_LL2C7:({void*_tmp435=0;((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmp436="typecmp: function types not handled";_tag_dyneither(_tmp436,sizeof(char),
36);}),_tag_dyneither(_tmp435,sizeof(void*),0));});_LL2C8: _tmp3FC=_tmp38D.f1;if(
_tmp3FC <= (void*)4)goto _LL2CA;if(*((int*)_tmp3FC)!= 9)goto _LL2CA;_tmp3FD=((
struct Cyc_Absyn_TupleType_struct*)_tmp3FC)->f1;_tmp3FE=_tmp38D.f2;if(_tmp3FE <= (
void*)4)goto _LL2CA;if(*((int*)_tmp3FE)!= 9)goto _LL2CA;_tmp3FF=((struct Cyc_Absyn_TupleType_struct*)
_tmp3FE)->f1;_LL2C9: return((int(*)(int(*cmp)(struct _tuple5*,struct _tuple5*),
struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_tqual_type_cmp,
_tmp3FF,_tmp3FD);_LL2CA: _tmp400=_tmp38D.f1;if(_tmp400 <= (void*)4)goto _LL2CC;if(*((
int*)_tmp400)!= 11)goto _LL2CC;_tmp401=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp400)->f1;_tmp402=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp400)->f2;_tmp403=
_tmp38D.f2;if(_tmp403 <= (void*)4)goto _LL2CC;if(*((int*)_tmp403)!= 11)goto _LL2CC;
_tmp404=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)_tmp403)->f1;_tmp405=((
struct Cyc_Absyn_AnonAggrType_struct*)_tmp403)->f2;_LL2CB: if(_tmp404 != _tmp401){
if(_tmp404 == (void*)0)return - 1;else{return 1;}}return((int(*)(int(*cmp)(struct Cyc_Absyn_Aggrfield*,
struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(
Cyc_Tcutil_aggrfield_cmp,_tmp405,_tmp402);_LL2CC: _tmp406=_tmp38D.f1;if(_tmp406 <= (
void*)4)goto _LL2CE;if(*((int*)_tmp406)!= 14)goto _LL2CE;_tmp407=(void*)((struct
Cyc_Absyn_RgnHandleType_struct*)_tmp406)->f1;_tmp408=_tmp38D.f2;if(_tmp408 <= (
void*)4)goto _LL2CE;if(*((int*)_tmp408)!= 14)goto _LL2CE;_tmp409=(void*)((struct
Cyc_Absyn_RgnHandleType_struct*)_tmp408)->f1;_LL2CD: return Cyc_Tcutil_typecmp(
_tmp407,_tmp409);_LL2CE: _tmp40A=_tmp38D.f1;if(_tmp40A <= (void*)4)goto _LL2D0;if(*((
int*)_tmp40A)!= 15)goto _LL2D0;_tmp40B=(void*)((struct Cyc_Absyn_DynRgnType_struct*)
_tmp40A)->f1;_tmp40C=(void*)((struct Cyc_Absyn_DynRgnType_struct*)_tmp40A)->f2;
_tmp40D=_tmp38D.f2;if(_tmp40D <= (void*)4)goto _LL2D0;if(*((int*)_tmp40D)!= 15)
goto _LL2D0;_tmp40E=(void*)((struct Cyc_Absyn_DynRgnType_struct*)_tmp40D)->f1;
_tmp40F=(void*)((struct Cyc_Absyn_DynRgnType_struct*)_tmp40D)->f2;_LL2CF: {int
_tmp437=Cyc_Tcutil_typecmp(_tmp40B,_tmp40E);if(_tmp437 != 0)return _tmp437;else{
return Cyc_Tcutil_typecmp(_tmp40C,_tmp40F);}}_LL2D0: _tmp410=_tmp38D.f1;if(_tmp410
<= (void*)4)goto _LL2D2;if(*((int*)_tmp410)!= 18)goto _LL2D2;_tmp411=(void*)((
struct Cyc_Absyn_TagType_struct*)_tmp410)->f1;_tmp412=_tmp38D.f2;if(_tmp412 <= (
void*)4)goto _LL2D2;if(*((int*)_tmp412)!= 18)goto _LL2D2;_tmp413=(void*)((struct
Cyc_Absyn_TagType_struct*)_tmp412)->f1;_LL2D1: return Cyc_Tcutil_typecmp(_tmp411,
_tmp413);_LL2D2: _tmp414=_tmp38D.f1;if(_tmp414 <= (void*)4)goto _LL2D4;if(*((int*)
_tmp414)!= 17)goto _LL2D4;_tmp415=((struct Cyc_Absyn_ValueofType_struct*)_tmp414)->f1;
_tmp416=_tmp38D.f2;if(_tmp416 <= (void*)4)goto _LL2D4;if(*((int*)_tmp416)!= 17)
goto _LL2D4;_tmp417=((struct Cyc_Absyn_ValueofType_struct*)_tmp416)->f1;_LL2D3:
return Cyc_Evexp_const_exp_cmp(_tmp415,_tmp417);_LL2D4: _tmp418=_tmp38D.f1;if(
_tmp418 <= (void*)4)goto _LL2D6;if(*((int*)_tmp418)!= 20)goto _LL2D6;_LL2D5: goto
_LL2D7;_LL2D6: _tmp419=_tmp38D.f2;if(_tmp419 <= (void*)4)goto _LL2D8;if(*((int*)
_tmp419)!= 20)goto _LL2D8;_LL2D7: goto _LL2D9;_LL2D8: _tmp41A=_tmp38D.f1;if(_tmp41A
<= (void*)4)goto _LL2DA;if(*((int*)_tmp41A)!= 19)goto _LL2DA;_LL2D9: goto _LL2DB;
_LL2DA: _tmp41B=_tmp38D.f1;if(_tmp41B <= (void*)4)goto _LL2DC;if(*((int*)_tmp41B)!= 
21)goto _LL2DC;_LL2DB: goto _LL2DD;_LL2DC: _tmp41C=_tmp38D.f2;if(_tmp41C <= (void*)4)
goto _LL2DE;if(*((int*)_tmp41C)!= 21)goto _LL2DE;_LL2DD: goto _LL2DF;_LL2DE: _tmp41D=
_tmp38D.f2;if(_tmp41D <= (void*)4)goto _LL2E0;if(*((int*)_tmp41D)!= 19)goto _LL2E0;
_LL2DF:({void*_tmp438=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(({const char*_tmp439="typecmp: effects not handled";
_tag_dyneither(_tmp439,sizeof(char),29);}),_tag_dyneither(_tmp438,sizeof(void*),
0));});_LL2E0:;_LL2E1:({void*_tmp43A=0;((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp43B="Unmatched case in typecmp";
_tag_dyneither(_tmp43B,sizeof(char),26);}),_tag_dyneither(_tmp43A,sizeof(void*),
0));});_LL2AD:;}}}int Cyc_Tcutil_is_arithmetic_type(void*t){void*_tmp43C=Cyc_Tcutil_compress(
t);_LL2E8: if(_tmp43C <= (void*)4)goto _LL2EA;if(*((int*)_tmp43C)!= 5)goto _LL2EA;
_LL2E9: goto _LL2EB;_LL2EA: if((int)_tmp43C != 1)goto _LL2EC;_LL2EB: goto _LL2ED;_LL2EC:
if(_tmp43C <= (void*)4)goto _LL2F2;if(*((int*)_tmp43C)!= 6)goto _LL2EE;_LL2ED: goto
_LL2EF;_LL2EE: if(*((int*)_tmp43C)!= 12)goto _LL2F0;_LL2EF: goto _LL2F1;_LL2F0: if(*((
int*)_tmp43C)!= 13)goto _LL2F2;_LL2F1: return 1;_LL2F2:;_LL2F3: return 0;_LL2E7:;}int
Cyc_Tcutil_will_lose_precision(void*t1,void*t2){t1=Cyc_Tcutil_compress(t1);t2=
Cyc_Tcutil_compress(t2);{struct _tuple0 _tmp43E=({struct _tuple0 _tmp43D;_tmp43D.f1=
t1;_tmp43D.f2=t2;_tmp43D;});void*_tmp43F;int _tmp440;void*_tmp441;int _tmp442;void*
_tmp443;void*_tmp444;void*_tmp445;void*_tmp446;void*_tmp447;void*_tmp448;void*
_tmp449;void*_tmp44A;void*_tmp44B;void*_tmp44C;void*_tmp44D;void*_tmp44E;void*
_tmp44F;void*_tmp450;void*_tmp451;void*_tmp452;void*_tmp453;void*_tmp454;void*
_tmp455;void*_tmp456;void*_tmp457;void*_tmp458;void*_tmp459;void*_tmp45A;void*
_tmp45B;void*_tmp45C;void*_tmp45D;void*_tmp45E;void*_tmp45F;void*_tmp460;void*
_tmp461;void*_tmp462;void*_tmp463;void*_tmp464;void*_tmp465;void*_tmp466;void*
_tmp467;void*_tmp468;void*_tmp469;void*_tmp46A;void*_tmp46B;void*_tmp46C;void*
_tmp46D;void*_tmp46E;void*_tmp46F;void*_tmp470;void*_tmp471;void*_tmp472;void*
_tmp473;void*_tmp474;void*_tmp475;void*_tmp476;void*_tmp477;void*_tmp478;void*
_tmp479;void*_tmp47A;_LL2F5: _tmp43F=_tmp43E.f1;if(_tmp43F <= (void*)4)goto _LL2F7;
if(*((int*)_tmp43F)!= 6)goto _LL2F7;_tmp440=((struct Cyc_Absyn_DoubleType_struct*)
_tmp43F)->f1;_tmp441=_tmp43E.f2;if(_tmp441 <= (void*)4)goto _LL2F7;if(*((int*)
_tmp441)!= 6)goto _LL2F7;_tmp442=((struct Cyc_Absyn_DoubleType_struct*)_tmp441)->f1;
_LL2F6: return !_tmp442  && _tmp440;_LL2F7: _tmp443=_tmp43E.f1;if(_tmp443 <= (void*)4)
goto _LL2F9;if(*((int*)_tmp443)!= 6)goto _LL2F9;_tmp444=_tmp43E.f2;if((int)_tmp444
!= 1)goto _LL2F9;_LL2F8: goto _LL2FA;_LL2F9: _tmp445=_tmp43E.f1;if(_tmp445 <= (void*)
4)goto _LL2FB;if(*((int*)_tmp445)!= 6)goto _LL2FB;_tmp446=_tmp43E.f2;if(_tmp446 <= (
void*)4)goto _LL2FB;if(*((int*)_tmp446)!= 5)goto _LL2FB;_LL2FA: goto _LL2FC;_LL2FB:
_tmp447=_tmp43E.f1;if(_tmp447 <= (void*)4)goto _LL2FD;if(*((int*)_tmp447)!= 6)goto
_LL2FD;_tmp448=_tmp43E.f2;if(_tmp448 <= (void*)4)goto _LL2FD;if(*((int*)_tmp448)!= 
18)goto _LL2FD;_LL2FC: goto _LL2FE;_LL2FD: _tmp449=_tmp43E.f1;if((int)_tmp449 != 1)
goto _LL2FF;_tmp44A=_tmp43E.f2;if(_tmp44A <= (void*)4)goto _LL2FF;if(*((int*)
_tmp44A)!= 18)goto _LL2FF;_LL2FE: goto _LL300;_LL2FF: _tmp44B=_tmp43E.f1;if((int)
_tmp44B != 1)goto _LL301;_tmp44C=_tmp43E.f2;if(_tmp44C <= (void*)4)goto _LL301;if(*((
int*)_tmp44C)!= 5)goto _LL301;_LL300: return 1;_LL301: _tmp44D=_tmp43E.f1;if(_tmp44D
<= (void*)4)goto _LL303;if(*((int*)_tmp44D)!= 5)goto _LL303;_tmp44E=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp44D)->f2;if((int)_tmp44E != 4)goto _LL303;
_tmp44F=_tmp43E.f2;if(_tmp44F <= (void*)4)goto _LL303;if(*((int*)_tmp44F)!= 5)goto
_LL303;_tmp450=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp44F)->f2;if((int)
_tmp450 != 4)goto _LL303;_LL302: return 0;_LL303: _tmp451=_tmp43E.f1;if(_tmp451 <= (
void*)4)goto _LL305;if(*((int*)_tmp451)!= 5)goto _LL305;_tmp452=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp451)->f2;if((int)_tmp452 != 4)goto _LL305;_LL304: return 1;_LL305: _tmp453=
_tmp43E.f1;if(_tmp453 <= (void*)4)goto _LL307;if(*((int*)_tmp453)!= 5)goto _LL307;
_tmp454=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp453)->f2;if((int)_tmp454 != 
3)goto _LL307;_tmp455=_tmp43E.f2;if(_tmp455 <= (void*)4)goto _LL307;if(*((int*)
_tmp455)!= 5)goto _LL307;_tmp456=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp455)->f2;
if((int)_tmp456 != 2)goto _LL307;_LL306: goto _LL308;_LL307: _tmp457=_tmp43E.f1;if(
_tmp457 <= (void*)4)goto _LL309;if(*((int*)_tmp457)!= 5)goto _LL309;_tmp458=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp457)->f2;if((int)_tmp458 != 2)goto _LL309;
_tmp459=_tmp43E.f2;if(_tmp459 <= (void*)4)goto _LL309;if(*((int*)_tmp459)!= 5)goto
_LL309;_tmp45A=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp459)->f2;if((int)
_tmp45A != 3)goto _LL309;_LL308: return 0;_LL309: _tmp45B=_tmp43E.f1;if(_tmp45B <= (
void*)4)goto _LL30B;if(*((int*)_tmp45B)!= 5)goto _LL30B;_tmp45C=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp45B)->f2;if((int)_tmp45C != 3)goto _LL30B;_tmp45D=_tmp43E.f2;if((int)_tmp45D != 
1)goto _LL30B;_LL30A: goto _LL30C;_LL30B: _tmp45E=_tmp43E.f1;if(_tmp45E <= (void*)4)
goto _LL30D;if(*((int*)_tmp45E)!= 5)goto _LL30D;_tmp45F=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp45E)->f2;if((int)_tmp45F != 2)goto _LL30D;_tmp460=_tmp43E.f2;if((int)_tmp460 != 
1)goto _LL30D;_LL30C: goto _LL30E;_LL30D: _tmp461=_tmp43E.f1;if(_tmp461 <= (void*)4)
goto _LL30F;if(*((int*)_tmp461)!= 5)goto _LL30F;_tmp462=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp461)->f2;if((int)_tmp462 != 3)goto _LL30F;_tmp463=_tmp43E.f2;if(_tmp463 <= (
void*)4)goto _LL30F;if(*((int*)_tmp463)!= 5)goto _LL30F;_tmp464=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp463)->f2;if((int)_tmp464 != 1)goto _LL30F;_LL30E: goto _LL310;_LL30F: _tmp465=
_tmp43E.f1;if(_tmp465 <= (void*)4)goto _LL311;if(*((int*)_tmp465)!= 5)goto _LL311;
_tmp466=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp465)->f2;if((int)_tmp466 != 
2)goto _LL311;_tmp467=_tmp43E.f2;if(_tmp467 <= (void*)4)goto _LL311;if(*((int*)
_tmp467)!= 5)goto _LL311;_tmp468=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp467)->f2;
if((int)_tmp468 != 1)goto _LL311;_LL310: goto _LL312;_LL311: _tmp469=_tmp43E.f1;if(
_tmp469 <= (void*)4)goto _LL313;if(*((int*)_tmp469)!= 18)goto _LL313;_tmp46A=
_tmp43E.f2;if(_tmp46A <= (void*)4)goto _LL313;if(*((int*)_tmp46A)!= 5)goto _LL313;
_tmp46B=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp46A)->f2;if((int)_tmp46B != 
1)goto _LL313;_LL312: goto _LL314;_LL313: _tmp46C=_tmp43E.f1;if(_tmp46C <= (void*)4)
goto _LL315;if(*((int*)_tmp46C)!= 5)goto _LL315;_tmp46D=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp46C)->f2;if((int)_tmp46D != 3)goto _LL315;_tmp46E=_tmp43E.f2;if(_tmp46E <= (
void*)4)goto _LL315;if(*((int*)_tmp46E)!= 5)goto _LL315;_tmp46F=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp46E)->f2;if((int)_tmp46F != 0)goto _LL315;_LL314: goto _LL316;_LL315: _tmp470=
_tmp43E.f1;if(_tmp470 <= (void*)4)goto _LL317;if(*((int*)_tmp470)!= 5)goto _LL317;
_tmp471=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp470)->f2;if((int)_tmp471 != 
2)goto _LL317;_tmp472=_tmp43E.f2;if(_tmp472 <= (void*)4)goto _LL317;if(*((int*)
_tmp472)!= 5)goto _LL317;_tmp473=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp472)->f2;
if((int)_tmp473 != 0)goto _LL317;_LL316: goto _LL318;_LL317: _tmp474=_tmp43E.f1;if(
_tmp474 <= (void*)4)goto _LL319;if(*((int*)_tmp474)!= 5)goto _LL319;_tmp475=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp474)->f2;if((int)_tmp475 != 1)goto _LL319;
_tmp476=_tmp43E.f2;if(_tmp476 <= (void*)4)goto _LL319;if(*((int*)_tmp476)!= 5)goto
_LL319;_tmp477=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp476)->f2;if((int)
_tmp477 != 0)goto _LL319;_LL318: goto _LL31A;_LL319: _tmp478=_tmp43E.f1;if(_tmp478 <= (
void*)4)goto _LL31B;if(*((int*)_tmp478)!= 18)goto _LL31B;_tmp479=_tmp43E.f2;if(
_tmp479 <= (void*)4)goto _LL31B;if(*((int*)_tmp479)!= 5)goto _LL31B;_tmp47A=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp479)->f2;if((int)_tmp47A != 0)goto _LL31B;
_LL31A: return 1;_LL31B:;_LL31C: return 0;_LL2F4:;}}int Cyc_Tcutil_coerce_list(struct
Cyc_Tcenv_Tenv*te,void*t,struct Cyc_List_List*es){struct _RegionHandle*_tmp47B=Cyc_Tcenv_get_fnrgn(
te);{struct Cyc_Core_Opt*max_arith_type=0;{struct Cyc_List_List*el=es;for(0;el != 0;
el=el->tl){void*t1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(((
struct Cyc_Absyn_Exp*)el->hd)->topt))->v);if(Cyc_Tcutil_is_arithmetic_type(t1)){
if(max_arith_type == 0  || Cyc_Tcutil_will_lose_precision(t1,(void*)max_arith_type->v))
max_arith_type=({struct Cyc_Core_Opt*_tmp47C=_region_malloc(_tmp47B,sizeof(*
_tmp47C));_tmp47C->v=(void*)t1;_tmp47C;});}}}if(max_arith_type != 0){if(!Cyc_Tcutil_unify(
t,(void*)max_arith_type->v))return 0;}}{struct Cyc_List_List*el=es;for(0;el != 0;el=
el->tl){if(!Cyc_Tcutil_coerce_assign(te,(struct Cyc_Absyn_Exp*)el->hd,t)){({
struct Cyc_String_pa_struct _tmp480;_tmp480.tag=0;_tmp480.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt))->v));{struct Cyc_String_pa_struct
_tmp47F;_tmp47F.tag=0;_tmp47F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t));{void*_tmp47D[2]={& _tmp47F,& _tmp480};Cyc_Tcutil_terr(((
struct Cyc_Absyn_Exp*)el->hd)->loc,({const char*_tmp47E="type mismatch: expecting %s but found %s";
_tag_dyneither(_tmp47E,sizeof(char),41);}),_tag_dyneither(_tmp47D,sizeof(void*),
2));}}});return 0;}}}return 1;}int Cyc_Tcutil_coerce_to_bool(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e){if(!Cyc_Tcutil_coerce_sint_typ(te,e)){void*_tmp481=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);_LL31E: if(_tmp481 <= (void*)
4)goto _LL320;if(*((int*)_tmp481)!= 4)goto _LL320;_LL31F: Cyc_Tcutil_unchecked_cast(
te,e,Cyc_Absyn_uint_typ,(void*)3);goto _LL31D;_LL320:;_LL321: return 0;_LL31D:;}
return 1;}int Cyc_Tcutil_is_integral_type(void*t){void*_tmp482=Cyc_Tcutil_compress(
t);_LL323: if(_tmp482 <= (void*)4)goto _LL32B;if(*((int*)_tmp482)!= 5)goto _LL325;
_LL324: goto _LL326;_LL325: if(*((int*)_tmp482)!= 18)goto _LL327;_LL326: goto _LL328;
_LL327: if(*((int*)_tmp482)!= 12)goto _LL329;_LL328: goto _LL32A;_LL329: if(*((int*)
_tmp482)!= 13)goto _LL32B;_LL32A: return 1;_LL32B:;_LL32C: return 0;_LL322:;}int Cyc_Tcutil_coerce_uint_typ(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){if(Cyc_Tcutil_unify((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_Absyn_uint_typ))return 1;if(Cyc_Tcutil_is_integral_type((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)){if(Cyc_Tcutil_will_lose_precision((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_Absyn_uint_typ))({void*
_tmp483=0;Cyc_Tcutil_warn(e->loc,({const char*_tmp484="integral size mismatch; conversion supplied";
_tag_dyneither(_tmp484,sizeof(char),44);}),_tag_dyneither(_tmp483,sizeof(void*),
0));});Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_uint_typ,(void*)1);return 1;}
return 0;}int Cyc_Tcutil_coerce_sint_typ(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*
e){if(Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_Absyn_sint_typ))
return 1;if(Cyc_Tcutil_is_integral_type((void*)((struct Cyc_Core_Opt*)_check_null(
e->topt))->v)){if(Cyc_Tcutil_will_lose_precision((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,Cyc_Absyn_sint_typ))({void*_tmp485=0;Cyc_Tcutil_warn(e->loc,({
const char*_tmp486="integral size mismatch; conversion supplied";_tag_dyneither(
_tmp486,sizeof(char),44);}),_tag_dyneither(_tmp485,sizeof(void*),0));});Cyc_Tcutil_unchecked_cast(
te,e,Cyc_Absyn_sint_typ,(void*)1);return 1;}return 0;}struct _tuple13{union Cyc_Absyn_Constraint_union
f1;union Cyc_Absyn_Constraint_union f2;};int Cyc_Tcutil_silent_castable(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void*t1,void*t2){t1=Cyc_Tcutil_compress(t1);t2=
Cyc_Tcutil_compress(t2);{struct _tuple0 _tmp488=({struct _tuple0 _tmp487;_tmp487.f1=
t1;_tmp487.f2=t2;_tmp487;});void*_tmp489;struct Cyc_Absyn_PtrInfo _tmp48A;void*
_tmp48B;struct Cyc_Absyn_PtrInfo _tmp48C;void*_tmp48D;struct Cyc_Absyn_TunionInfo
_tmp48E;union Cyc_Absyn_TunionInfoU_union _tmp48F;struct Cyc_Absyn_Tuniondecl**
_tmp490;struct Cyc_Absyn_Tuniondecl*_tmp491;struct Cyc_List_List*_tmp492;struct Cyc_Core_Opt*
_tmp493;struct Cyc_Core_Opt _tmp494;void*_tmp495;void*_tmp496;struct Cyc_Absyn_TunionInfo
_tmp497;union Cyc_Absyn_TunionInfoU_union _tmp498;struct Cyc_Absyn_Tuniondecl**
_tmp499;struct Cyc_Absyn_Tuniondecl*_tmp49A;struct Cyc_List_List*_tmp49B;struct Cyc_Core_Opt*
_tmp49C;struct Cyc_Core_Opt _tmp49D;void*_tmp49E;void*_tmp49F;struct Cyc_Absyn_ArrayInfo
_tmp4A0;void*_tmp4A1;struct Cyc_Absyn_Tqual _tmp4A2;struct Cyc_Absyn_Exp*_tmp4A3;
struct Cyc_Absyn_Conref*_tmp4A4;void*_tmp4A5;struct Cyc_Absyn_ArrayInfo _tmp4A6;
void*_tmp4A7;struct Cyc_Absyn_Tqual _tmp4A8;struct Cyc_Absyn_Exp*_tmp4A9;struct Cyc_Absyn_Conref*
_tmp4AA;void*_tmp4AB;struct Cyc_Absyn_TunionFieldInfo _tmp4AC;union Cyc_Absyn_TunionFieldInfoU_union
_tmp4AD;struct Cyc_Absyn_Tuniondecl*_tmp4AE;struct Cyc_Absyn_Tunionfield*_tmp4AF;
struct Cyc_List_List*_tmp4B0;void*_tmp4B1;struct Cyc_Absyn_TunionInfo _tmp4B2;union
Cyc_Absyn_TunionInfoU_union _tmp4B3;struct Cyc_Absyn_Tuniondecl**_tmp4B4;struct Cyc_Absyn_Tuniondecl*
_tmp4B5;struct Cyc_List_List*_tmp4B6;void*_tmp4B7;struct Cyc_Absyn_PtrInfo _tmp4B8;
void*_tmp4B9;struct Cyc_Absyn_Tqual _tmp4BA;struct Cyc_Absyn_PtrAtts _tmp4BB;void*
_tmp4BC;struct Cyc_Absyn_Conref*_tmp4BD;struct Cyc_Absyn_Conref*_tmp4BE;struct Cyc_Absyn_Conref*
_tmp4BF;void*_tmp4C0;struct Cyc_Absyn_TunionInfo _tmp4C1;union Cyc_Absyn_TunionInfoU_union
_tmp4C2;struct Cyc_Absyn_Tuniondecl**_tmp4C3;struct Cyc_Absyn_Tuniondecl*_tmp4C4;
struct Cyc_List_List*_tmp4C5;struct Cyc_Core_Opt*_tmp4C6;void*_tmp4C7;void*_tmp4C8;
_LL32E: _tmp489=_tmp488.f1;if(_tmp489 <= (void*)4)goto _LL330;if(*((int*)_tmp489)!= 
4)goto _LL330;_tmp48A=((struct Cyc_Absyn_PointerType_struct*)_tmp489)->f1;_tmp48B=
_tmp488.f2;if(_tmp48B <= (void*)4)goto _LL330;if(*((int*)_tmp48B)!= 4)goto _LL330;
_tmp48C=((struct Cyc_Absyn_PointerType_struct*)_tmp48B)->f1;_LL32F: {int okay=1;
if(!((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*
y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,(_tmp48A.ptr_atts).nullable,(
_tmp48C.ptr_atts).nullable)){union Cyc_Absyn_Constraint_union _tmp4C9=(((struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref*x))Cyc_Absyn_compress_conref)((_tmp48A.ptr_atts).nullable))->v;
int _tmp4CA;_LL33D: if((_tmp4C9.Eq_constr).tag != 0)goto _LL33F;_tmp4CA=(_tmp4C9.Eq_constr).f1;
_LL33E: okay=!_tmp4CA;goto _LL33C;_LL33F:;_LL340:({void*_tmp4CB=0;((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp4CC="silent_castable conref not eq";
_tag_dyneither(_tmp4CC,sizeof(char),30);}),_tag_dyneither(_tmp4CB,sizeof(void*),
0));});_LL33C:;}if(!Cyc_Tcutil_unify_conrefs(Cyc_Tcutil_unify_it_bounds,(_tmp48A.ptr_atts).bounds,(
_tmp48C.ptr_atts).bounds)){struct _tuple13 _tmp4CE=({struct _tuple13 _tmp4CD;_tmp4CD.f1=(
Cyc_Absyn_compress_conref((_tmp48A.ptr_atts).bounds))->v;_tmp4CD.f2=(Cyc_Absyn_compress_conref((
_tmp48C.ptr_atts).bounds))->v;_tmp4CD;});union Cyc_Absyn_Constraint_union _tmp4CF;
void*_tmp4D0;union Cyc_Absyn_Constraint_union _tmp4D1;void*_tmp4D2;union Cyc_Absyn_Constraint_union
_tmp4D3;_LL342: _tmp4CF=_tmp4CE.f1;if(((_tmp4CE.f1).Eq_constr).tag != 0)goto _LL344;
_tmp4D0=(_tmp4CF.Eq_constr).f1;_tmp4D1=_tmp4CE.f2;if(((_tmp4CE.f2).Eq_constr).tag
!= 0)goto _LL344;_tmp4D2=(_tmp4D1.Eq_constr).f1;_LL343:{struct _tuple0 _tmp4D5=({
struct _tuple0 _tmp4D4;_tmp4D4.f1=_tmp4D0;_tmp4D4.f2=_tmp4D2;_tmp4D4;});void*
_tmp4D6;void*_tmp4D7;void*_tmp4D8;void*_tmp4D9;void*_tmp4DA;struct Cyc_Absyn_Exp*
_tmp4DB;void*_tmp4DC;struct Cyc_Absyn_Exp*_tmp4DD;void*_tmp4DE;void*_tmp4DF;
struct Cyc_Absyn_Exp*_tmp4E0;_LL349: _tmp4D6=_tmp4D5.f1;if(_tmp4D6 <= (void*)1)goto
_LL34B;if(*((int*)_tmp4D6)!= 0)goto _LL34B;_tmp4D7=_tmp4D5.f2;if((int)_tmp4D7 != 0)
goto _LL34B;_LL34A: goto _LL34C;_LL34B: _tmp4D8=_tmp4D5.f1;if((int)_tmp4D8 != 0)goto
_LL34D;_tmp4D9=_tmp4D5.f2;if((int)_tmp4D9 != 0)goto _LL34D;_LL34C: okay=1;goto
_LL348;_LL34D: _tmp4DA=_tmp4D5.f1;if(_tmp4DA <= (void*)1)goto _LL34F;if(*((int*)
_tmp4DA)!= 0)goto _LL34F;_tmp4DB=((struct Cyc_Absyn_Upper_b_struct*)_tmp4DA)->f1;
_tmp4DC=_tmp4D5.f2;if(_tmp4DC <= (void*)1)goto _LL34F;if(*((int*)_tmp4DC)!= 0)goto
_LL34F;_tmp4DD=((struct Cyc_Absyn_Upper_b_struct*)_tmp4DC)->f1;_LL34E: okay=okay
 && Cyc_Evexp_lte_const_exp(_tmp4DD,_tmp4DB);if(!((int(*)(int,struct Cyc_Absyn_Conref*
x))Cyc_Absyn_conref_def)(0,(_tmp48C.ptr_atts).zero_term))({void*_tmp4E1=0;Cyc_Tcutil_warn(
loc,({const char*_tmp4E2="implicit cast to shorter array";_tag_dyneither(_tmp4E2,
sizeof(char),31);}),_tag_dyneither(_tmp4E1,sizeof(void*),0));});goto _LL348;
_LL34F: _tmp4DE=_tmp4D5.f1;if((int)_tmp4DE != 0)goto _LL348;_tmp4DF=_tmp4D5.f2;if(
_tmp4DF <= (void*)1)goto _LL348;if(*((int*)_tmp4DF)!= 0)goto _LL348;_tmp4E0=((
struct Cyc_Absyn_Upper_b_struct*)_tmp4DF)->f1;_LL350: if(((int(*)(int,struct Cyc_Absyn_Conref*
x))Cyc_Absyn_conref_def)(0,(_tmp48A.ptr_atts).zero_term) && Cyc_Tcutil_is_bound_one((
_tmp48C.ptr_atts).bounds))goto _LL348;okay=0;goto _LL348;_LL348:;}goto _LL341;
_LL344: _tmp4D3=_tmp4CE.f1;if(((_tmp4CE.f1).No_constr).tag != 2)goto _LL346;_LL345:
okay=0;goto _LL341;_LL346:;_LL347:({void*_tmp4E3=0;((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp4E4="silent_castable conrefs didn't unify";
_tag_dyneither(_tmp4E4,sizeof(char),37);}),_tag_dyneither(_tmp4E3,sizeof(void*),
0));});_LL341:;}okay=okay  && Cyc_Tcutil_unify((void*)_tmp48A.elt_typ,(void*)
_tmp48C.elt_typ);okay=okay  && (Cyc_Tcutil_unify((void*)(_tmp48A.ptr_atts).rgn,(
void*)(_tmp48C.ptr_atts).rgn) || Cyc_Tcenv_region_outlives(te,(void*)(_tmp48A.ptr_atts).rgn,(
void*)(_tmp48C.ptr_atts).rgn));okay=okay  && (!(_tmp48A.elt_tq).real_const  || (
_tmp48C.elt_tq).real_const);okay=okay  && (((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*
x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,(_tmp48A.ptr_atts).zero_term,(
_tmp48C.ptr_atts).zero_term) || ((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
1,(_tmp48A.ptr_atts).zero_term) && (_tmp48C.elt_tq).real_const);return okay;}
_LL330: _tmp48D=_tmp488.f1;if(_tmp48D <= (void*)4)goto _LL332;if(*((int*)_tmp48D)!= 
2)goto _LL332;_tmp48E=((struct Cyc_Absyn_TunionType_struct*)_tmp48D)->f1;_tmp48F=
_tmp48E.tunion_info;if((((((struct Cyc_Absyn_TunionType_struct*)_tmp48D)->f1).tunion_info).KnownTunion).tag
!= 1)goto _LL332;_tmp490=(_tmp48F.KnownTunion).f1;_tmp491=*_tmp490;_tmp492=
_tmp48E.targs;_tmp493=_tmp48E.rgn;if(_tmp493 == 0)goto _LL332;_tmp494=*_tmp493;
_tmp495=(void*)_tmp494.v;_tmp496=_tmp488.f2;if(_tmp496 <= (void*)4)goto _LL332;if(*((
int*)_tmp496)!= 2)goto _LL332;_tmp497=((struct Cyc_Absyn_TunionType_struct*)
_tmp496)->f1;_tmp498=_tmp497.tunion_info;if((((((struct Cyc_Absyn_TunionType_struct*)
_tmp496)->f1).tunion_info).KnownTunion).tag != 1)goto _LL332;_tmp499=(_tmp498.KnownTunion).f1;
_tmp49A=*_tmp499;_tmp49B=_tmp497.targs;_tmp49C=_tmp497.rgn;if(_tmp49C == 0)goto
_LL332;_tmp49D=*_tmp49C;_tmp49E=(void*)_tmp49D.v;_LL331: if(_tmp491 != _tmp49A  || 
!Cyc_Tcenv_region_outlives(te,_tmp495,_tmp49E))return 0;for(0;_tmp492 != 0  && 
_tmp49B != 0;(_tmp492=_tmp492->tl,_tmp49B=_tmp49B->tl)){if(!Cyc_Tcutil_unify((
void*)_tmp492->hd,(void*)_tmp49B->hd))return 0;}if(_tmp492 != 0  || _tmp49B != 0)
return 0;return 1;_LL332: _tmp49F=_tmp488.f1;if(_tmp49F <= (void*)4)goto _LL334;if(*((
int*)_tmp49F)!= 7)goto _LL334;_tmp4A0=((struct Cyc_Absyn_ArrayType_struct*)_tmp49F)->f1;
_tmp4A1=(void*)_tmp4A0.elt_type;_tmp4A2=_tmp4A0.tq;_tmp4A3=_tmp4A0.num_elts;
_tmp4A4=_tmp4A0.zero_term;_tmp4A5=_tmp488.f2;if(_tmp4A5 <= (void*)4)goto _LL334;
if(*((int*)_tmp4A5)!= 7)goto _LL334;_tmp4A6=((struct Cyc_Absyn_ArrayType_struct*)
_tmp4A5)->f1;_tmp4A7=(void*)_tmp4A6.elt_type;_tmp4A8=_tmp4A6.tq;_tmp4A9=_tmp4A6.num_elts;
_tmp4AA=_tmp4A6.zero_term;_LL333: {int okay;okay=((int(*)(int(*cmp)(int,int),
struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,
_tmp4A4,_tmp4AA) && ((_tmp4A3 != 0  && _tmp4A9 != 0) && Cyc_Evexp_same_const_exp((
struct Cyc_Absyn_Exp*)_tmp4A3,(struct Cyc_Absyn_Exp*)_tmp4A9));return(okay  && Cyc_Tcutil_unify(
_tmp4A1,_tmp4A7)) && (!_tmp4A2.real_const  || _tmp4A8.real_const);}_LL334: _tmp4AB=
_tmp488.f1;if(_tmp4AB <= (void*)4)goto _LL336;if(*((int*)_tmp4AB)!= 3)goto _LL336;
_tmp4AC=((struct Cyc_Absyn_TunionFieldType_struct*)_tmp4AB)->f1;_tmp4AD=_tmp4AC.field_info;
if((((((struct Cyc_Absyn_TunionFieldType_struct*)_tmp4AB)->f1).field_info).KnownTunionfield).tag
!= 1)goto _LL336;_tmp4AE=(_tmp4AD.KnownTunionfield).f1;_tmp4AF=(_tmp4AD.KnownTunionfield).f2;
_tmp4B0=_tmp4AC.targs;_tmp4B1=_tmp488.f2;if(_tmp4B1 <= (void*)4)goto _LL336;if(*((
int*)_tmp4B1)!= 2)goto _LL336;_tmp4B2=((struct Cyc_Absyn_TunionType_struct*)
_tmp4B1)->f1;_tmp4B3=_tmp4B2.tunion_info;if((((((struct Cyc_Absyn_TunionType_struct*)
_tmp4B1)->f1).tunion_info).KnownTunion).tag != 1)goto _LL336;_tmp4B4=(_tmp4B3.KnownTunion).f1;
_tmp4B5=*_tmp4B4;_tmp4B6=_tmp4B2.targs;_LL335: if((_tmp4AE == _tmp4B5  || Cyc_Absyn_qvar_cmp(
_tmp4AE->name,_tmp4B5->name)== 0) && (_tmp4AF->typs == 0  || _tmp4AE->is_flat)){
for(0;_tmp4B0 != 0  && _tmp4B6 != 0;(_tmp4B0=_tmp4B0->tl,_tmp4B6=_tmp4B6->tl)){if(!
Cyc_Tcutil_unify((void*)_tmp4B0->hd,(void*)_tmp4B6->hd))break;}if(_tmp4B0 == 0
 && _tmp4B6 == 0)return 1;}return 0;_LL336: _tmp4B7=_tmp488.f1;if(_tmp4B7 <= (void*)4)
goto _LL338;if(*((int*)_tmp4B7)!= 4)goto _LL338;_tmp4B8=((struct Cyc_Absyn_PointerType_struct*)
_tmp4B7)->f1;_tmp4B9=(void*)_tmp4B8.elt_typ;_tmp4BA=_tmp4B8.elt_tq;_tmp4BB=
_tmp4B8.ptr_atts;_tmp4BC=(void*)_tmp4BB.rgn;_tmp4BD=_tmp4BB.nullable;_tmp4BE=
_tmp4BB.bounds;_tmp4BF=_tmp4BB.zero_term;_tmp4C0=_tmp488.f2;if(_tmp4C0 <= (void*)
4)goto _LL338;if(*((int*)_tmp4C0)!= 2)goto _LL338;_tmp4C1=((struct Cyc_Absyn_TunionType_struct*)
_tmp4C0)->f1;_tmp4C2=_tmp4C1.tunion_info;if((((((struct Cyc_Absyn_TunionType_struct*)
_tmp4C0)->f1).tunion_info).KnownTunion).tag != 1)goto _LL338;_tmp4C3=(_tmp4C2.KnownTunion).f1;
_tmp4C4=*_tmp4C3;_tmp4C5=_tmp4C1.targs;_tmp4C6=_tmp4C1.rgn;if(!(!_tmp4C4->is_flat))
goto _LL338;_LL337:{void*_tmp4E5=Cyc_Tcutil_compress(_tmp4B9);struct Cyc_Absyn_TunionFieldInfo
_tmp4E6;union Cyc_Absyn_TunionFieldInfoU_union _tmp4E7;struct Cyc_Absyn_Tuniondecl*
_tmp4E8;struct Cyc_Absyn_Tunionfield*_tmp4E9;struct Cyc_List_List*_tmp4EA;_LL352:
if(_tmp4E5 <= (void*)4)goto _LL354;if(*((int*)_tmp4E5)!= 3)goto _LL354;_tmp4E6=((
struct Cyc_Absyn_TunionFieldType_struct*)_tmp4E5)->f1;_tmp4E7=_tmp4E6.field_info;
if((((((struct Cyc_Absyn_TunionFieldType_struct*)_tmp4E5)->f1).field_info).KnownTunionfield).tag
!= 1)goto _LL354;_tmp4E8=(_tmp4E7.KnownTunionfield).f1;_tmp4E9=(_tmp4E7.KnownTunionfield).f2;
_tmp4EA=_tmp4E6.targs;_LL353: if(!Cyc_Tcutil_unify(_tmp4BC,(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp4C6))->v) && !Cyc_Tcenv_region_outlives(te,_tmp4BC,(void*)
_tmp4C6->v))return 0;if(!((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,
struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp4BD,Cyc_Absyn_false_conref))
return 0;if(!Cyc_Tcutil_unify_conrefs(Cyc_Tcutil_unify_it_bounds,_tmp4BE,Cyc_Absyn_bounds_one_conref))
return 0;if(!((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*
y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp4BF,Cyc_Absyn_false_conref))
return 0;if(Cyc_Absyn_qvar_cmp(_tmp4C4->name,_tmp4E8->name)== 0  && _tmp4E9->typs
!= 0){int okay=1;for(0;_tmp4EA != 0  && _tmp4C5 != 0;(_tmp4EA=_tmp4EA->tl,_tmp4C5=
_tmp4C5->tl)){if(!Cyc_Tcutil_unify((void*)_tmp4EA->hd,(void*)_tmp4C5->hd)){okay=
0;break;}}if((!okay  || _tmp4EA != 0) || _tmp4C5 != 0)return 0;return 1;}goto _LL351;
_LL354:;_LL355: goto _LL351;_LL351:;}return 0;_LL338: _tmp4C7=_tmp488.f1;if(_tmp4C7
<= (void*)4)goto _LL33A;if(*((int*)_tmp4C7)!= 18)goto _LL33A;_tmp4C8=_tmp488.f2;
if(_tmp4C8 <= (void*)4)goto _LL33A;if(*((int*)_tmp4C8)!= 5)goto _LL33A;_LL339:
return 0;_LL33A:;_LL33B: return Cyc_Tcutil_unify(t1,t2);_LL32D:;}}int Cyc_Tcutil_is_pointer_type(
void*t){void*_tmp4EB=Cyc_Tcutil_compress(t);_LL357: if(_tmp4EB <= (void*)4)goto
_LL359;if(*((int*)_tmp4EB)!= 4)goto _LL359;_LL358: return 1;_LL359:;_LL35A: return 0;
_LL356:;}int Cyc_Tcutil_is_pointer_or_boxed(void*t,int*is_dyneither_ptr){void*
_tmp4EC=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp4ED;struct Cyc_Absyn_PtrAtts
_tmp4EE;struct Cyc_Absyn_Conref*_tmp4EF;_LL35C: if(_tmp4EC <= (void*)4)goto _LL35E;
if(*((int*)_tmp4EC)!= 4)goto _LL35E;_tmp4ED=((struct Cyc_Absyn_PointerType_struct*)
_tmp4EC)->f1;_tmp4EE=_tmp4ED.ptr_atts;_tmp4EF=_tmp4EE.bounds;_LL35D:*
is_dyneither_ptr=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp4EF)== (void*)0;
return 1;_LL35E:;_LL35F: return Cyc_Tcutil_typ_kind(t)== (void*)2;_LL35B:;}int Cyc_Tcutil_is_zero(
struct Cyc_Absyn_Exp*e){void*_tmp4F0=(void*)e->r;union Cyc_Absyn_Cnst_union _tmp4F1;
int _tmp4F2;union Cyc_Absyn_Cnst_union _tmp4F3;char _tmp4F4;union Cyc_Absyn_Cnst_union
_tmp4F5;short _tmp4F6;union Cyc_Absyn_Cnst_union _tmp4F7;long long _tmp4F8;void*
_tmp4F9;struct Cyc_Absyn_Exp*_tmp4FA;_LL361: if(*((int*)_tmp4F0)!= 0)goto _LL363;
_tmp4F1=((struct Cyc_Absyn_Const_e_struct*)_tmp4F0)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)
_tmp4F0)->f1).Int_c).tag != 2)goto _LL363;_tmp4F2=(_tmp4F1.Int_c).f2;if(_tmp4F2 != 
0)goto _LL363;_LL362: goto _LL364;_LL363: if(*((int*)_tmp4F0)!= 0)goto _LL365;_tmp4F3=((
struct Cyc_Absyn_Const_e_struct*)_tmp4F0)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)
_tmp4F0)->f1).Char_c).tag != 0)goto _LL365;_tmp4F4=(_tmp4F3.Char_c).f2;if(_tmp4F4
!= 0)goto _LL365;_LL364: goto _LL366;_LL365: if(*((int*)_tmp4F0)!= 0)goto _LL367;
_tmp4F5=((struct Cyc_Absyn_Const_e_struct*)_tmp4F0)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)
_tmp4F0)->f1).Short_c).tag != 1)goto _LL367;_tmp4F6=(_tmp4F5.Short_c).f2;if(
_tmp4F6 != 0)goto _LL367;_LL366: goto _LL368;_LL367: if(*((int*)_tmp4F0)!= 0)goto
_LL369;_tmp4F7=((struct Cyc_Absyn_Const_e_struct*)_tmp4F0)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)
_tmp4F0)->f1).LongLong_c).tag != 3)goto _LL369;_tmp4F8=(_tmp4F7.LongLong_c).f2;if(
_tmp4F8 != 0)goto _LL369;_LL368: return 1;_LL369: if(*((int*)_tmp4F0)!= 15)goto _LL36B;
_tmp4F9=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp4F0)->f1;_tmp4FA=((struct Cyc_Absyn_Cast_e_struct*)
_tmp4F0)->f2;_LL36A: return Cyc_Tcutil_is_zero(_tmp4FA) && Cyc_Tcutil_admits_zero(
_tmp4F9);_LL36B:;_LL36C: return 0;_LL360:;}struct Cyc_Core_Opt Cyc_Tcutil_trk={(void*)((
void*)5)};struct Cyc_Core_Opt Cyc_Tcutil_urk={(void*)((void*)4)};struct Cyc_Core_Opt
Cyc_Tcutil_rk={(void*)((void*)3)};struct Cyc_Core_Opt Cyc_Tcutil_ak={(void*)((void*)
0)};struct Cyc_Core_Opt Cyc_Tcutil_bk={(void*)((void*)2)};struct Cyc_Core_Opt Cyc_Tcutil_mk={(
void*)((void*)1)};struct Cyc_Core_Opt Cyc_Tcutil_ik={(void*)((void*)7)};struct Cyc_Core_Opt
Cyc_Tcutil_ek={(void*)((void*)6)};struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_opt(void*
k){void*_tmp4FB=k;_LL36E: if((int)_tmp4FB != 0)goto _LL370;_LL36F: return(struct Cyc_Core_Opt*)&
Cyc_Tcutil_ak;_LL370: if((int)_tmp4FB != 1)goto _LL372;_LL371: return(struct Cyc_Core_Opt*)&
Cyc_Tcutil_mk;_LL372: if((int)_tmp4FB != 2)goto _LL374;_LL373: return(struct Cyc_Core_Opt*)&
Cyc_Tcutil_bk;_LL374: if((int)_tmp4FB != 3)goto _LL376;_LL375: return(struct Cyc_Core_Opt*)&
Cyc_Tcutil_rk;_LL376: if((int)_tmp4FB != 4)goto _LL378;_LL377: return(struct Cyc_Core_Opt*)&
Cyc_Tcutil_urk;_LL378: if((int)_tmp4FB != 5)goto _LL37A;_LL379: return(struct Cyc_Core_Opt*)&
Cyc_Tcutil_trk;_LL37A: if((int)_tmp4FB != 6)goto _LL37C;_LL37B: return(struct Cyc_Core_Opt*)&
Cyc_Tcutil_ek;_LL37C: if((int)_tmp4FB != 7)goto _LL36D;_LL37D: return(struct Cyc_Core_Opt*)&
Cyc_Tcutil_ik;_LL36D:;}static void**Cyc_Tcutil_kind_to_b(void*k){static struct Cyc_Absyn_Eq_kb_struct
ab_v={0,(void*)((void*)0)};static struct Cyc_Absyn_Eq_kb_struct mb_v={0,(void*)((
void*)1)};static struct Cyc_Absyn_Eq_kb_struct bb_v={0,(void*)((void*)2)};static
struct Cyc_Absyn_Eq_kb_struct rb_v={0,(void*)((void*)3)};static struct Cyc_Absyn_Eq_kb_struct
ub_v={0,(void*)((void*)4)};static struct Cyc_Absyn_Eq_kb_struct tb_v={0,(void*)((
void*)5)};static struct Cyc_Absyn_Eq_kb_struct eb_v={0,(void*)((void*)6)};static
struct Cyc_Absyn_Eq_kb_struct ib_v={0,(void*)((void*)7)};static void*ab=(void*)&
ab_v;static void*mb=(void*)& mb_v;static void*bb=(void*)& bb_v;static void*rb=(void*)&
rb_v;static void*ub=(void*)& ub_v;static void*tb=(void*)& tb_v;static void*eb=(void*)&
eb_v;static void*ib=(void*)& ib_v;void*_tmp4FC=k;_LL37F: if((int)_tmp4FC != 0)goto
_LL381;_LL380: return& ab;_LL381: if((int)_tmp4FC != 1)goto _LL383;_LL382: return& mb;
_LL383: if((int)_tmp4FC != 2)goto _LL385;_LL384: return& bb;_LL385: if((int)_tmp4FC != 
3)goto _LL387;_LL386: return& rb;_LL387: if((int)_tmp4FC != 4)goto _LL389;_LL388:
return& ub;_LL389: if((int)_tmp4FC != 5)goto _LL38B;_LL38A: return& tb;_LL38B: if((int)
_tmp4FC != 6)goto _LL38D;_LL38C: return& eb;_LL38D: if((int)_tmp4FC != 7)goto _LL37E;
_LL38E: return& ib;_LL37E:;}void*Cyc_Tcutil_kind_to_bound(void*k){return*Cyc_Tcutil_kind_to_b(
k);}struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_bound_opt(void*k){return(struct Cyc_Core_Opt*)
Cyc_Tcutil_kind_to_b(k);}int Cyc_Tcutil_zero_to_null(struct Cyc_Tcenv_Tenv*te,void*
t2,struct Cyc_Absyn_Exp*e1){if(Cyc_Tcutil_is_pointer_type(t2) && Cyc_Tcutil_is_zero(
e1)){(void*)(e1->r=(void*)((void*)({struct Cyc_Absyn_Const_e_struct*_tmp505=
_cycalloc(sizeof(*_tmp505));_tmp505[0]=({struct Cyc_Absyn_Const_e_struct _tmp506;
_tmp506.tag=0;_tmp506.f1=(union Cyc_Absyn_Cnst_union)({union Cyc_Absyn_Cnst_union
_tmp507;(_tmp507.Null_c).tag=6;_tmp507;});_tmp506;});_tmp505;})));{struct Cyc_Core_Opt*
_tmp508=Cyc_Tcenv_lookup_opt_type_vars(te);struct Cyc_Absyn_PointerType_struct*
_tmp509=({struct Cyc_Absyn_PointerType_struct*_tmp50A=_cycalloc(sizeof(*_tmp50A));
_tmp50A[0]=({struct Cyc_Absyn_PointerType_struct _tmp50B;_tmp50B.tag=4;_tmp50B.f1=({
struct Cyc_Absyn_PtrInfo _tmp50C;_tmp50C.elt_typ=(void*)Cyc_Absyn_new_evar((struct
Cyc_Core_Opt*)& Cyc_Tcutil_ak,_tmp508);_tmp50C.elt_tq=Cyc_Absyn_empty_tqual(0);
_tmp50C.ptr_atts=({struct Cyc_Absyn_PtrAtts _tmp50D;_tmp50D.rgn=(void*)Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_trk,_tmp508);_tmp50D.nullable=Cyc_Absyn_true_conref;
_tmp50D.bounds=Cyc_Absyn_empty_conref();_tmp50D.zero_term=((struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)();_tmp50D.ptrloc=0;_tmp50D;});_tmp50C;});_tmp50B;});
_tmp50A;});(void*)(((struct Cyc_Core_Opt*)_check_null(e1->topt))->v=(void*)((void*)
_tmp509));return Cyc_Tcutil_coerce_arg(te,e1,t2);}}return 0;}struct _dyneither_ptr
Cyc_Tcutil_coercion2string(void*c){void*_tmp50E=c;_LL390: if((int)_tmp50E != 0)
goto _LL392;_LL391: return({const char*_tmp50F="unknown";_tag_dyneither(_tmp50F,
sizeof(char),8);});_LL392: if((int)_tmp50E != 1)goto _LL394;_LL393: return({const
char*_tmp510="no coercion";_tag_dyneither(_tmp510,sizeof(char),12);});_LL394: if((
int)_tmp50E != 2)goto _LL396;_LL395: return({const char*_tmp511="null check";
_tag_dyneither(_tmp511,sizeof(char),11);});_LL396: if((int)_tmp50E != 3)goto _LL38F;
_LL397: return({const char*_tmp512="other coercion";_tag_dyneither(_tmp512,sizeof(
char),15);});_LL38F:;}int Cyc_Tcutil_coerce_arg(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*
e,void*t2){void*t1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
e->topt))->v);void*c;if(Cyc_Tcutil_unify(t1,t2))return 1;if(Cyc_Tcutil_is_arithmetic_type(
t2) && Cyc_Tcutil_is_arithmetic_type(t1)){if(Cyc_Tcutil_will_lose_precision(t1,
t2))({struct Cyc_String_pa_struct _tmp516;_tmp516.tag=0;_tmp516.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2));{struct Cyc_String_pa_struct
_tmp515;_tmp515.tag=0;_tmp515.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t1));{void*_tmp513[2]={& _tmp515,& _tmp516};Cyc_Tcutil_warn(
e->loc,({const char*_tmp514="integral size mismatch; %s -> %s conversion supplied";
_tag_dyneither(_tmp514,sizeof(char),53);}),_tag_dyneither(_tmp513,sizeof(void*),
2));}}});Cyc_Tcutil_unchecked_cast(te,e,t2,(void*)1);return 1;}else{if(Cyc_Tcutil_silent_castable(
te,e->loc,t1,t2)){Cyc_Tcutil_unchecked_cast(te,e,t2,(void*)3);return 1;}else{if(
Cyc_Tcutil_zero_to_null(te,t2,e))return 1;else{if((c=Cyc_Tcutil_castable(te,e->loc,
t1,t2))!= (void*)0){Cyc_Tcutil_unchecked_cast(te,e,t2,c);if(c != (void*)2)({
struct Cyc_String_pa_struct _tmp51A;_tmp51A.tag=0;_tmp51A.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2));{struct Cyc_String_pa_struct
_tmp519;_tmp519.tag=0;_tmp519.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t1));{void*_tmp517[2]={& _tmp519,& _tmp51A};Cyc_Tcutil_warn(
e->loc,({const char*_tmp518="implicit cast from %s to %s";_tag_dyneither(_tmp518,
sizeof(char),28);}),_tag_dyneither(_tmp517,sizeof(void*),2));}}});return 1;}else{
return 0;}}}}}int Cyc_Tcutil_coerce_assign(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*
e,void*t){return Cyc_Tcutil_coerce_arg(te,e,t);}int Cyc_Tcutil_coerceable(void*t){
void*_tmp51B=Cyc_Tcutil_compress(t);_LL399: if(_tmp51B <= (void*)4)goto _LL39B;if(*((
int*)_tmp51B)!= 5)goto _LL39B;_LL39A: goto _LL39C;_LL39B: if((int)_tmp51B != 1)goto
_LL39D;_LL39C: goto _LL39E;_LL39D: if(_tmp51B <= (void*)4)goto _LL39F;if(*((int*)
_tmp51B)!= 6)goto _LL39F;_LL39E: return 1;_LL39F:;_LL3A0: return 0;_LL398:;}static
struct _tuple5*Cyc_Tcutil_flatten_typ_f(struct _tuple9*env,struct Cyc_Absyn_Aggrfield*
x){struct Cyc_List_List*_tmp51D;struct _RegionHandle*_tmp51E;struct _tuple9 _tmp51C=*
env;_tmp51D=_tmp51C.f1;_tmp51E=_tmp51C.f2;return({struct _tuple5*_tmp51F=
_region_malloc(_tmp51E,sizeof(*_tmp51F));_tmp51F->f1=x->tq;_tmp51F->f2=Cyc_Tcutil_rsubstitute(
_tmp51E,_tmp51D,(void*)x->type);_tmp51F;});}static struct _tuple5*Cyc_Tcutil_rcopy_tqt(
struct _RegionHandle*r,struct _tuple5*x){struct Cyc_Absyn_Tqual _tmp521;void*_tmp522;
struct _tuple5 _tmp520=*x;_tmp521=_tmp520.f1;_tmp522=_tmp520.f2;return({struct
_tuple5*_tmp523=_region_malloc(r,sizeof(*_tmp523));_tmp523->f1=_tmp521;_tmp523->f2=
_tmp522;_tmp523;});}static struct Cyc_List_List*Cyc_Tcutil_flatten_typ(struct
_RegionHandle*r,struct Cyc_Tcenv_Tenv*te,void*t1){t1=Cyc_Tcutil_compress(t1);{
void*_tmp524=t1;struct Cyc_List_List*_tmp525;struct Cyc_Absyn_AggrInfo _tmp526;
union Cyc_Absyn_AggrInfoU_union _tmp527;struct Cyc_Absyn_Aggrdecl**_tmp528;struct
Cyc_Absyn_Aggrdecl*_tmp529;struct Cyc_List_List*_tmp52A;void*_tmp52B;struct Cyc_List_List*
_tmp52C;struct Cyc_Absyn_FnInfo _tmp52D;_LL3A2: if((int)_tmp524 != 0)goto _LL3A4;
_LL3A3: return 0;_LL3A4: if(_tmp524 <= (void*)4)goto _LL3AC;if(*((int*)_tmp524)!= 9)
goto _LL3A6;_tmp525=((struct Cyc_Absyn_TupleType_struct*)_tmp524)->f1;_LL3A5:
return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple5*(*f)(struct
_RegionHandle*,struct _tuple5*),struct _RegionHandle*env,struct Cyc_List_List*x))
Cyc_List_rmap_c)(r,Cyc_Tcutil_rcopy_tqt,r,_tmp525);_LL3A6: if(*((int*)_tmp524)!= 
10)goto _LL3A8;_tmp526=((struct Cyc_Absyn_AggrType_struct*)_tmp524)->f1;_tmp527=
_tmp526.aggr_info;if((((((struct Cyc_Absyn_AggrType_struct*)_tmp524)->f1).aggr_info).KnownAggr).tag
!= 1)goto _LL3A8;_tmp528=(_tmp527.KnownAggr).f1;_tmp529=*_tmp528;_tmp52A=_tmp526.targs;
_LL3A7: if((((void*)_tmp529->kind == (void*)1  || _tmp529->impl == 0) || ((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp529->impl))->exist_vars != 0) || ((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp529->impl))->rgn_po != 0)return({struct Cyc_List_List*_tmp52E=
_region_malloc(r,sizeof(*_tmp52E));_tmp52E->hd=({struct _tuple5*_tmp52F=
_region_malloc(r,sizeof(*_tmp52F));_tmp52F->f1=Cyc_Absyn_empty_tqual(0);_tmp52F->f2=
t1;_tmp52F;});_tmp52E->tl=0;_tmp52E;});{struct Cyc_List_List*_tmp530=((struct Cyc_List_List*(*)(
struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(r,r,_tmp529->tvs,_tmp52A);struct _tuple9 env=({struct _tuple9
_tmp531;_tmp531.f1=_tmp530;_tmp531.f2=r;_tmp531;});return((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _tuple5*(*f)(struct _tuple9*,struct Cyc_Absyn_Aggrfield*),
struct _tuple9*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_typ_f,&
env,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp529->impl))->fields);}_LL3A8:
if(*((int*)_tmp524)!= 11)goto _LL3AA;_tmp52B=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp524)->f1;if((int)_tmp52B != 0)goto _LL3AA;_tmp52C=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp524)->f2;_LL3A9: {struct _tuple9 env=({struct _tuple9 _tmp532;_tmp532.f1=0;
_tmp532.f2=r;_tmp532;});return((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct _tuple5*(*f)(struct _tuple9*,struct Cyc_Absyn_Aggrfield*),struct _tuple9*env,
struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_typ_f,& env,_tmp52C);}
_LL3AA: if(*((int*)_tmp524)!= 8)goto _LL3AC;_tmp52D=((struct Cyc_Absyn_FnType_struct*)
_tmp524)->f1;_LL3AB: return({struct Cyc_List_List*_tmp533=_region_malloc(r,sizeof(*
_tmp533));_tmp533->hd=({struct _tuple5*_tmp534=_region_malloc(r,sizeof(*_tmp534));
_tmp534->f1=Cyc_Absyn_const_tqual(0);_tmp534->f2=t1;_tmp534;});_tmp533->tl=0;
_tmp533;});_LL3AC:;_LL3AD: return({struct Cyc_List_List*_tmp535=_region_malloc(r,
sizeof(*_tmp535));_tmp535->hd=({struct _tuple5*_tmp536=_region_malloc(r,sizeof(*
_tmp536));_tmp536->f1=Cyc_Absyn_empty_tqual(0);_tmp536->f2=t1;_tmp536;});_tmp535->tl=
0;_tmp535;});_LL3A1:;}}static int Cyc_Tcutil_sub_attributes(struct Cyc_List_List*a1,
struct Cyc_List_List*a2){{struct Cyc_List_List*t=a1;for(0;t != 0;t=t->tl){void*
_tmp537=(void*)t->hd;_LL3AF: if((int)_tmp537 != 16)goto _LL3B1;_LL3B0: goto _LL3B2;
_LL3B1: if((int)_tmp537 != 3)goto _LL3B3;_LL3B2: goto _LL3B4;_LL3B3: if(_tmp537 <= (
void*)17)goto _LL3B5;if(*((int*)_tmp537)!= 4)goto _LL3B5;_LL3B4: continue;_LL3B5:;
_LL3B6: if(!Cyc_List_exists_c(Cyc_Tcutil_equal_att,(void*)t->hd,a2))return 0;
_LL3AE:;}}for(0;a2 != 0;a2=a2->tl){if(!Cyc_List_exists_c(Cyc_Tcutil_equal_att,(
void*)a2->hd,a1))return 0;}return 1;}static int Cyc_Tcutil_ptrsubtype(struct Cyc_Tcenv_Tenv*
te,struct Cyc_List_List*assume,void*t1,void*t2);static int Cyc_Tcutil_subtype(
struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2){if(Cyc_Tcutil_unify(
t1,t2))return 1;{struct Cyc_List_List*a=assume;for(0;a != 0;a=a->tl){if(Cyc_Tcutil_unify(
t1,(*((struct _tuple0*)a->hd)).f1) && Cyc_Tcutil_unify(t2,(*((struct _tuple0*)a->hd)).f2))
return 1;}}t1=Cyc_Tcutil_compress(t1);t2=Cyc_Tcutil_compress(t2);{struct _tuple0
_tmp539=({struct _tuple0 _tmp538;_tmp538.f1=t1;_tmp538.f2=t2;_tmp538;});void*
_tmp53A;struct Cyc_Absyn_PtrInfo _tmp53B;void*_tmp53C;struct Cyc_Absyn_Tqual _tmp53D;
struct Cyc_Absyn_PtrAtts _tmp53E;void*_tmp53F;struct Cyc_Absyn_Conref*_tmp540;
struct Cyc_Absyn_Conref*_tmp541;struct Cyc_Absyn_Conref*_tmp542;void*_tmp543;
struct Cyc_Absyn_PtrInfo _tmp544;void*_tmp545;struct Cyc_Absyn_Tqual _tmp546;struct
Cyc_Absyn_PtrAtts _tmp547;void*_tmp548;struct Cyc_Absyn_Conref*_tmp549;struct Cyc_Absyn_Conref*
_tmp54A;struct Cyc_Absyn_Conref*_tmp54B;void*_tmp54C;struct Cyc_Absyn_TunionInfo
_tmp54D;union Cyc_Absyn_TunionInfoU_union _tmp54E;struct Cyc_Absyn_Tuniondecl**
_tmp54F;struct Cyc_Absyn_Tuniondecl*_tmp550;struct Cyc_List_List*_tmp551;struct Cyc_Core_Opt*
_tmp552;struct Cyc_Core_Opt _tmp553;void*_tmp554;void*_tmp555;struct Cyc_Absyn_TunionInfo
_tmp556;union Cyc_Absyn_TunionInfoU_union _tmp557;struct Cyc_Absyn_Tuniondecl**
_tmp558;struct Cyc_Absyn_Tuniondecl*_tmp559;struct Cyc_List_List*_tmp55A;struct Cyc_Core_Opt*
_tmp55B;struct Cyc_Core_Opt _tmp55C;void*_tmp55D;void*_tmp55E;struct Cyc_Absyn_FnInfo
_tmp55F;void*_tmp560;struct Cyc_Absyn_FnInfo _tmp561;_LL3B8: _tmp53A=_tmp539.f1;if(
_tmp53A <= (void*)4)goto _LL3BA;if(*((int*)_tmp53A)!= 4)goto _LL3BA;_tmp53B=((
struct Cyc_Absyn_PointerType_struct*)_tmp53A)->f1;_tmp53C=(void*)_tmp53B.elt_typ;
_tmp53D=_tmp53B.elt_tq;_tmp53E=_tmp53B.ptr_atts;_tmp53F=(void*)_tmp53E.rgn;
_tmp540=_tmp53E.nullable;_tmp541=_tmp53E.bounds;_tmp542=_tmp53E.zero_term;
_tmp543=_tmp539.f2;if(_tmp543 <= (void*)4)goto _LL3BA;if(*((int*)_tmp543)!= 4)goto
_LL3BA;_tmp544=((struct Cyc_Absyn_PointerType_struct*)_tmp543)->f1;_tmp545=(void*)
_tmp544.elt_typ;_tmp546=_tmp544.elt_tq;_tmp547=_tmp544.ptr_atts;_tmp548=(void*)
_tmp547.rgn;_tmp549=_tmp547.nullable;_tmp54A=_tmp547.bounds;_tmp54B=_tmp547.zero_term;
_LL3B9: if(_tmp53D.real_const  && !_tmp546.real_const)return 0;if((!((int(*)(int(*
cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(
Cyc_Core_intcmp,_tmp540,_tmp549) && ((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,_tmp540)) && !((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,
_tmp549))return 0;if((!((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,
struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp542,
_tmp54B) && !((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,
_tmp542)) && ((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,
_tmp54B))return 0;if(!Cyc_Tcutil_unify(_tmp53F,_tmp548) && !Cyc_Tcenv_region_outlives(
te,_tmp53F,_tmp548))return 0;if(!Cyc_Tcutil_unify_conrefs(Cyc_Tcutil_unify_it_bounds,
_tmp541,_tmp54A)){struct _tuple0 _tmp563=({struct _tuple0 _tmp562;_tmp562.f1=Cyc_Absyn_conref_val(
_tmp541);_tmp562.f2=Cyc_Absyn_conref_val(_tmp54A);_tmp562;});void*_tmp564;void*
_tmp565;void*_tmp566;struct Cyc_Absyn_Exp*_tmp567;void*_tmp568;struct Cyc_Absyn_Exp*
_tmp569;_LL3C1: _tmp564=_tmp563.f1;if(_tmp564 <= (void*)1)goto _LL3C3;if(*((int*)
_tmp564)!= 0)goto _LL3C3;_tmp565=_tmp563.f2;if((int)_tmp565 != 0)goto _LL3C3;_LL3C2:
goto _LL3C0;_LL3C3: _tmp566=_tmp563.f1;if(_tmp566 <= (void*)1)goto _LL3C5;if(*((int*)
_tmp566)!= 0)goto _LL3C5;_tmp567=((struct Cyc_Absyn_Upper_b_struct*)_tmp566)->f1;
_tmp568=_tmp563.f2;if(_tmp568 <= (void*)1)goto _LL3C5;if(*((int*)_tmp568)!= 0)goto
_LL3C5;_tmp569=((struct Cyc_Absyn_Upper_b_struct*)_tmp568)->f1;_LL3C4: if(!Cyc_Evexp_lte_const_exp(
_tmp569,_tmp567))return 0;goto _LL3C0;_LL3C5:;_LL3C6: return 0;_LL3C0:;}return Cyc_Tcutil_ptrsubtype(
te,({struct Cyc_List_List*_tmp56A=_cycalloc(sizeof(*_tmp56A));_tmp56A->hd=({
struct _tuple0*_tmp56B=_cycalloc(sizeof(*_tmp56B));_tmp56B->f1=t1;_tmp56B->f2=t2;
_tmp56B;});_tmp56A->tl=assume;_tmp56A;}),_tmp53C,_tmp545);_LL3BA: _tmp54C=_tmp539.f1;
if(_tmp54C <= (void*)4)goto _LL3BC;if(*((int*)_tmp54C)!= 2)goto _LL3BC;_tmp54D=((
struct Cyc_Absyn_TunionType_struct*)_tmp54C)->f1;_tmp54E=_tmp54D.tunion_info;if((((((
struct Cyc_Absyn_TunionType_struct*)_tmp54C)->f1).tunion_info).KnownTunion).tag != 
1)goto _LL3BC;_tmp54F=(_tmp54E.KnownTunion).f1;_tmp550=*_tmp54F;_tmp551=_tmp54D.targs;
_tmp552=_tmp54D.rgn;if(_tmp552 == 0)goto _LL3BC;_tmp553=*_tmp552;_tmp554=(void*)
_tmp553.v;_tmp555=_tmp539.f2;if(_tmp555 <= (void*)4)goto _LL3BC;if(*((int*)_tmp555)
!= 2)goto _LL3BC;_tmp556=((struct Cyc_Absyn_TunionType_struct*)_tmp555)->f1;
_tmp557=_tmp556.tunion_info;if((((((struct Cyc_Absyn_TunionType_struct*)_tmp555)->f1).tunion_info).KnownTunion).tag
!= 1)goto _LL3BC;_tmp558=(_tmp557.KnownTunion).f1;_tmp559=*_tmp558;_tmp55A=
_tmp556.targs;_tmp55B=_tmp556.rgn;if(_tmp55B == 0)goto _LL3BC;_tmp55C=*_tmp55B;
_tmp55D=(void*)_tmp55C.v;_LL3BB: if(_tmp550 != _tmp559  || !Cyc_Tcenv_region_outlives(
te,_tmp554,_tmp55D))return 0;for(0;_tmp551 != 0  && _tmp55A != 0;(_tmp551=_tmp551->tl,
_tmp55A=_tmp55A->tl)){if(!Cyc_Tcutil_unify((void*)_tmp551->hd,(void*)_tmp55A->hd))
return 0;}if(_tmp551 != 0  || _tmp55A != 0)return 0;return 1;_LL3BC: _tmp55E=_tmp539.f1;
if(_tmp55E <= (void*)4)goto _LL3BE;if(*((int*)_tmp55E)!= 8)goto _LL3BE;_tmp55F=((
struct Cyc_Absyn_FnType_struct*)_tmp55E)->f1;_tmp560=_tmp539.f2;if(_tmp560 <= (
void*)4)goto _LL3BE;if(*((int*)_tmp560)!= 8)goto _LL3BE;_tmp561=((struct Cyc_Absyn_FnType_struct*)
_tmp560)->f1;_LL3BD: if(_tmp55F.tvars != 0  || _tmp561.tvars != 0){struct Cyc_List_List*
_tmp56C=_tmp55F.tvars;struct Cyc_List_List*_tmp56D=_tmp561.tvars;if(((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmp56C)!= ((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(_tmp56D))return 0;{struct _RegionHandle*_tmp56E=Cyc_Tcenv_get_fnrgn(
te);struct Cyc_List_List*inst=0;while(_tmp56C != 0){if(Cyc_Tcutil_tvar_kind((
struct Cyc_Absyn_Tvar*)_tmp56C->hd)!= Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*)_check_null(_tmp56D))->hd))return 0;inst=({struct Cyc_List_List*
_tmp56F=_region_malloc(_tmp56E,sizeof(*_tmp56F));_tmp56F->hd=({struct _tuple8*
_tmp570=_region_malloc(_tmp56E,sizeof(*_tmp570));_tmp570->f1=(struct Cyc_Absyn_Tvar*)
_tmp56D->hd;_tmp570->f2=(void*)({struct Cyc_Absyn_VarType_struct*_tmp571=
_cycalloc(sizeof(*_tmp571));_tmp571[0]=({struct Cyc_Absyn_VarType_struct _tmp572;
_tmp572.tag=1;_tmp572.f1=(struct Cyc_Absyn_Tvar*)_tmp56C->hd;_tmp572;});_tmp571;});
_tmp570;});_tmp56F->tl=inst;_tmp56F;});_tmp56C=_tmp56C->tl;_tmp56D=_tmp56D->tl;}
if(inst != 0){_tmp55F.tvars=0;_tmp561.tvars=0;return Cyc_Tcutil_subtype(te,assume,(
void*)({struct Cyc_Absyn_FnType_struct*_tmp573=_cycalloc(sizeof(*_tmp573));
_tmp573[0]=({struct Cyc_Absyn_FnType_struct _tmp574;_tmp574.tag=8;_tmp574.f1=
_tmp55F;_tmp574;});_tmp573;}),(void*)({struct Cyc_Absyn_FnType_struct*_tmp575=
_cycalloc(sizeof(*_tmp575));_tmp575[0]=({struct Cyc_Absyn_FnType_struct _tmp576;
_tmp576.tag=8;_tmp576.f1=_tmp561;_tmp576;});_tmp575;}));}}}if(!Cyc_Tcutil_subtype(
te,assume,(void*)_tmp55F.ret_typ,(void*)_tmp561.ret_typ))return 0;{struct Cyc_List_List*
_tmp577=_tmp55F.args;struct Cyc_List_List*_tmp578=_tmp561.args;if(((int(*)(struct
Cyc_List_List*x))Cyc_List_length)(_tmp577)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp578))return 0;for(0;_tmp577 != 0;(_tmp577=_tmp577->tl,_tmp578=_tmp578->tl)){
struct Cyc_Absyn_Tqual _tmp57A;void*_tmp57B;struct _tuple3 _tmp579=*((struct _tuple3*)
_tmp577->hd);_tmp57A=_tmp579.f2;_tmp57B=_tmp579.f3;{struct Cyc_Absyn_Tqual _tmp57D;
void*_tmp57E;struct _tuple3 _tmp57C=*((struct _tuple3*)((struct Cyc_List_List*)
_check_null(_tmp578))->hd);_tmp57D=_tmp57C.f2;_tmp57E=_tmp57C.f3;if(_tmp57D.real_const
 && !_tmp57A.real_const  || !Cyc_Tcutil_subtype(te,assume,_tmp57E,_tmp57B))return
0;}}if(_tmp55F.c_varargs != _tmp561.c_varargs)return 0;if(_tmp55F.cyc_varargs != 0
 && _tmp561.cyc_varargs != 0){struct Cyc_Absyn_VarargInfo _tmp57F=*_tmp55F.cyc_varargs;
struct Cyc_Absyn_VarargInfo _tmp580=*_tmp561.cyc_varargs;if((_tmp580.tq).real_const
 && !(_tmp57F.tq).real_const  || !Cyc_Tcutil_subtype(te,assume,(void*)_tmp580.type,(
void*)_tmp57F.type))return 0;}else{if(_tmp55F.cyc_varargs != 0  || _tmp561.cyc_varargs
!= 0)return 0;}if(!Cyc_Tcutil_subset_effect(1,(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp55F.effect))->v,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp561.effect))->v))
return 0;if(!Cyc_Tcutil_sub_rgnpo(_tmp55F.rgn_po,_tmp561.rgn_po))return 0;if(!Cyc_Tcutil_sub_attributes(
_tmp55F.attributes,_tmp561.attributes))return 0;return 1;}_LL3BE:;_LL3BF: return 0;
_LL3B7:;}}static int Cyc_Tcutil_isomorphic(void*t1,void*t2){struct _tuple0 _tmp582=({
struct _tuple0 _tmp581;_tmp581.f1=Cyc_Tcutil_compress(t1);_tmp581.f2=Cyc_Tcutil_compress(
t2);_tmp581;});void*_tmp583;void*_tmp584;void*_tmp585;void*_tmp586;_LL3C8:
_tmp583=_tmp582.f1;if(_tmp583 <= (void*)4)goto _LL3CA;if(*((int*)_tmp583)!= 5)goto
_LL3CA;_tmp584=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp583)->f2;_tmp585=
_tmp582.f2;if(_tmp585 <= (void*)4)goto _LL3CA;if(*((int*)_tmp585)!= 5)goto _LL3CA;
_tmp586=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp585)->f2;_LL3C9: return(
_tmp584 == _tmp586  || _tmp584 == (void*)2  && _tmp586 == (void*)3) || _tmp584 == (
void*)3  && _tmp586 == (void*)2;_LL3CA:;_LL3CB: return 0;_LL3C7:;}static int Cyc_Tcutil_ptrsubtype(
struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2){struct
_RegionHandle*_tmp587=Cyc_Tcenv_get_fnrgn(te);struct Cyc_List_List*tqs1=Cyc_Tcutil_flatten_typ(
_tmp587,te,t1);struct Cyc_List_List*tqs2=Cyc_Tcutil_flatten_typ(_tmp587,te,t2);
for(0;tqs2 != 0;(tqs2=tqs2->tl,tqs1=tqs1->tl)){if(tqs1 == 0)return 0;{struct _tuple5
_tmp589;struct Cyc_Absyn_Tqual _tmp58A;void*_tmp58B;struct _tuple5*_tmp588=(struct
_tuple5*)tqs1->hd;_tmp589=*_tmp588;_tmp58A=_tmp589.f1;_tmp58B=_tmp589.f2;{struct
_tuple5 _tmp58D;struct Cyc_Absyn_Tqual _tmp58E;void*_tmp58F;struct _tuple5*_tmp58C=(
struct _tuple5*)tqs2->hd;_tmp58D=*_tmp58C;_tmp58E=_tmp58D.f1;_tmp58F=_tmp58D.f2;
if(_tmp58E.real_const  && Cyc_Tcutil_subtype(te,assume,_tmp58B,_tmp58F))continue;
else{if(Cyc_Tcutil_unify(_tmp58B,_tmp58F))continue;else{if(Cyc_Tcutil_isomorphic(
_tmp58B,_tmp58F))continue;else{return 0;}}}}}}return 1;}static int Cyc_Tcutil_is_char_type(
void*t){void*_tmp590=Cyc_Tcutil_compress(t);void*_tmp591;_LL3CD: if(_tmp590 <= (
void*)4)goto _LL3CF;if(*((int*)_tmp590)!= 5)goto _LL3CF;_tmp591=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp590)->f2;if((int)_tmp591 != 0)goto _LL3CF;_LL3CE: return 1;_LL3CF:;_LL3D0: return
0;_LL3CC:;}void*Cyc_Tcutil_castable(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void*t1,void*t2){if(Cyc_Tcutil_unify(t1,t2))return(void*)1;t1=Cyc_Tcutil_compress(
t1);t2=Cyc_Tcutil_compress(t2);if(t2 == (void*)0)return(void*)1;{void*_tmp592=t2;
void*_tmp593;void*_tmp594;_LL3D2: if(_tmp592 <= (void*)4)goto _LL3D6;if(*((int*)
_tmp592)!= 5)goto _LL3D4;_tmp593=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp592)->f2;
if((int)_tmp593 != 2)goto _LL3D4;_LL3D3: goto _LL3D5;_LL3D4: if(*((int*)_tmp592)!= 5)
goto _LL3D6;_tmp594=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp592)->f2;if((int)
_tmp594 != 3)goto _LL3D6;_LL3D5: if(Cyc_Tcutil_typ_kind(t1)== (void*)2)return(void*)
1;goto _LL3D1;_LL3D6:;_LL3D7: goto _LL3D1;_LL3D1:;}{void*_tmp595=t1;struct Cyc_Absyn_PtrInfo
_tmp596;void*_tmp597;struct Cyc_Absyn_Tqual _tmp598;struct Cyc_Absyn_PtrAtts _tmp599;
void*_tmp59A;struct Cyc_Absyn_Conref*_tmp59B;struct Cyc_Absyn_Conref*_tmp59C;
struct Cyc_Absyn_Conref*_tmp59D;struct Cyc_Absyn_ArrayInfo _tmp59E;void*_tmp59F;
struct Cyc_Absyn_Tqual _tmp5A0;struct Cyc_Absyn_Exp*_tmp5A1;struct Cyc_Absyn_Conref*
_tmp5A2;struct Cyc_Absyn_Enumdecl*_tmp5A3;void*_tmp5A4;_LL3D9: if(_tmp595 <= (void*)
4)goto _LL3E1;if(*((int*)_tmp595)!= 4)goto _LL3DB;_tmp596=((struct Cyc_Absyn_PointerType_struct*)
_tmp595)->f1;_tmp597=(void*)_tmp596.elt_typ;_tmp598=_tmp596.elt_tq;_tmp599=
_tmp596.ptr_atts;_tmp59A=(void*)_tmp599.rgn;_tmp59B=_tmp599.nullable;_tmp59C=
_tmp599.bounds;_tmp59D=_tmp599.zero_term;_LL3DA:{void*_tmp5A5=t2;struct Cyc_Absyn_PtrInfo
_tmp5A6;void*_tmp5A7;struct Cyc_Absyn_Tqual _tmp5A8;struct Cyc_Absyn_PtrAtts _tmp5A9;
void*_tmp5AA;struct Cyc_Absyn_Conref*_tmp5AB;struct Cyc_Absyn_Conref*_tmp5AC;
struct Cyc_Absyn_Conref*_tmp5AD;_LL3EA: if(_tmp5A5 <= (void*)4)goto _LL3EC;if(*((int*)
_tmp5A5)!= 4)goto _LL3EC;_tmp5A6=((struct Cyc_Absyn_PointerType_struct*)_tmp5A5)->f1;
_tmp5A7=(void*)_tmp5A6.elt_typ;_tmp5A8=_tmp5A6.elt_tq;_tmp5A9=_tmp5A6.ptr_atts;
_tmp5AA=(void*)_tmp5A9.rgn;_tmp5AB=_tmp5A9.nullable;_tmp5AC=_tmp5A9.bounds;
_tmp5AD=_tmp5A9.zero_term;_LL3EB: {void*coercion=(void*)3;struct Cyc_List_List*
_tmp5AE=({struct Cyc_List_List*_tmp5B7=_cycalloc(sizeof(*_tmp5B7));_tmp5B7->hd=({
struct _tuple0*_tmp5B8=_cycalloc(sizeof(*_tmp5B8));_tmp5B8->f1=t1;_tmp5B8->f2=t2;
_tmp5B8;});_tmp5B7->tl=0;_tmp5B7;});int _tmp5AF=Cyc_Tcutil_ptrsubtype(te,_tmp5AE,
_tmp597,_tmp5A7) && (!_tmp598.real_const  || _tmp5A8.real_const);Cyc_Tcutil_t1_failure=
t1;Cyc_Tcutil_t2_failure=t2;{int zeroterm_ok=((int(*)(int(*cmp)(int,int),struct
Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,
_tmp59D,_tmp5AD) || !((int(*)(struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_val)(
_tmp5AD);int _tmp5B0=_tmp5AF?0:((Cyc_Tcutil_bits_only(_tmp597) && Cyc_Tcutil_is_char_type(
_tmp5A7)) && !((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,
_tmp5AD)) && (_tmp5A8.real_const  || !_tmp598.real_const);int bounds_ok=Cyc_Tcutil_unify_conrefs(
Cyc_Tcutil_unify_it_bounds,_tmp59C,_tmp5AC);if(!bounds_ok  && !_tmp5B0){struct
_tuple0 _tmp5B2=({struct _tuple0 _tmp5B1;_tmp5B1.f1=Cyc_Absyn_conref_val(_tmp59C);
_tmp5B1.f2=Cyc_Absyn_conref_val(_tmp5AC);_tmp5B1;});void*_tmp5B3;struct Cyc_Absyn_Exp*
_tmp5B4;void*_tmp5B5;struct Cyc_Absyn_Exp*_tmp5B6;_LL3EF: _tmp5B3=_tmp5B2.f1;if(
_tmp5B3 <= (void*)1)goto _LL3F1;if(*((int*)_tmp5B3)!= 0)goto _LL3F1;_tmp5B4=((
struct Cyc_Absyn_Upper_b_struct*)_tmp5B3)->f1;_tmp5B5=_tmp5B2.f2;if(_tmp5B5 <= (
void*)1)goto _LL3F1;if(*((int*)_tmp5B5)!= 0)goto _LL3F1;_tmp5B6=((struct Cyc_Absyn_Upper_b_struct*)
_tmp5B5)->f1;_LL3F0: if(Cyc_Evexp_lte_const_exp(_tmp5B6,_tmp5B4))bounds_ok=1;goto
_LL3EE;_LL3F1:;_LL3F2: bounds_ok=1;goto _LL3EE;_LL3EE:;}if(((int(*)(int,struct Cyc_Absyn_Conref*
x))Cyc_Absyn_conref_def)(0,_tmp59B) && !((int(*)(int,struct Cyc_Absyn_Conref*x))
Cyc_Absyn_conref_def)(0,_tmp5AB))coercion=(void*)2;if(((bounds_ok  && zeroterm_ok)
 && (_tmp5AF  || _tmp5B0)) && (Cyc_Tcutil_unify(_tmp59A,_tmp5AA) || Cyc_Tcenv_region_outlives(
te,_tmp59A,_tmp5AA)))return coercion;else{return(void*)0;}}}_LL3EC:;_LL3ED: goto
_LL3E9;_LL3E9:;}return(void*)0;_LL3DB: if(*((int*)_tmp595)!= 7)goto _LL3DD;_tmp59E=((
struct Cyc_Absyn_ArrayType_struct*)_tmp595)->f1;_tmp59F=(void*)_tmp59E.elt_type;
_tmp5A0=_tmp59E.tq;_tmp5A1=_tmp59E.num_elts;_tmp5A2=_tmp59E.zero_term;_LL3DC:{
void*_tmp5B9=t2;struct Cyc_Absyn_ArrayInfo _tmp5BA;void*_tmp5BB;struct Cyc_Absyn_Tqual
_tmp5BC;struct Cyc_Absyn_Exp*_tmp5BD;struct Cyc_Absyn_Conref*_tmp5BE;_LL3F4: if(
_tmp5B9 <= (void*)4)goto _LL3F6;if(*((int*)_tmp5B9)!= 7)goto _LL3F6;_tmp5BA=((
struct Cyc_Absyn_ArrayType_struct*)_tmp5B9)->f1;_tmp5BB=(void*)_tmp5BA.elt_type;
_tmp5BC=_tmp5BA.tq;_tmp5BD=_tmp5BA.num_elts;_tmp5BE=_tmp5BA.zero_term;_LL3F5: {
int okay;okay=((_tmp5A1 != 0  && _tmp5BD != 0) && ((int(*)(int(*cmp)(int,int),struct
Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,
_tmp5A2,_tmp5BE)) && Cyc_Evexp_lte_const_exp((struct Cyc_Absyn_Exp*)_tmp5BD,(
struct Cyc_Absyn_Exp*)_tmp5A1);return(okay  && Cyc_Tcutil_unify(_tmp59F,_tmp5BB))
 && (!_tmp5A0.real_const  || _tmp5BC.real_const)?(void*)3:(void*)0;}_LL3F6:;
_LL3F7: return(void*)0;_LL3F3:;}return(void*)0;_LL3DD: if(*((int*)_tmp595)!= 12)
goto _LL3DF;_tmp5A3=((struct Cyc_Absyn_EnumType_struct*)_tmp595)->f2;_LL3DE:{void*
_tmp5BF=t2;struct Cyc_Absyn_Enumdecl*_tmp5C0;_LL3F9: if(_tmp5BF <= (void*)4)goto
_LL3FB;if(*((int*)_tmp5BF)!= 12)goto _LL3FB;_tmp5C0=((struct Cyc_Absyn_EnumType_struct*)
_tmp5BF)->f2;_LL3FA: if((_tmp5A3->fields != 0  && _tmp5C0->fields != 0) && ((int(*)(
struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp5A3->fields))->v)>= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)((
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp5C0->fields))->v))
return(void*)1;goto _LL3F8;_LL3FB:;_LL3FC: goto _LL3F8;_LL3F8:;}goto _LL3E0;_LL3DF:
if(*((int*)_tmp595)!= 5)goto _LL3E1;_LL3E0: goto _LL3E2;_LL3E1: if((int)_tmp595 != 1)
goto _LL3E3;_LL3E2: goto _LL3E4;_LL3E3: if(_tmp595 <= (void*)4)goto _LL3E7;if(*((int*)
_tmp595)!= 6)goto _LL3E5;_LL3E4: return Cyc_Tcutil_coerceable(t2)?(void*)1:(void*)0;
_LL3E5: if(*((int*)_tmp595)!= 14)goto _LL3E7;_tmp5A4=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp595)->f1;_LL3E6:{void*_tmp5C1=t2;void*_tmp5C2;_LL3FE: if(_tmp5C1 <= (void*)4)
goto _LL400;if(*((int*)_tmp5C1)!= 14)goto _LL400;_tmp5C2=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp5C1)->f1;_LL3FF: if(Cyc_Tcenv_region_outlives(te,_tmp5A4,_tmp5C2))return(void*)
1;goto _LL3FD;_LL400:;_LL401: goto _LL3FD;_LL3FD:;}return(void*)0;_LL3E7:;_LL3E8:
return(void*)0;_LL3D8:;}}void Cyc_Tcutil_unchecked_cast(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Absyn_Exp*e,void*t,void*c){if(!Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,t)){struct Cyc_Absyn_Exp*_tmp5C3=Cyc_Absyn_copy_exp(e);(
void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Cast_e_struct*_tmp5C4=_cycalloc(
sizeof(*_tmp5C4));_tmp5C4[0]=({struct Cyc_Absyn_Cast_e_struct _tmp5C5;_tmp5C5.tag=
15;_tmp5C5.f1=(void*)t;_tmp5C5.f2=_tmp5C3;_tmp5C5.f3=0;_tmp5C5.f4=(void*)c;
_tmp5C5;});_tmp5C4;})));e->topt=({struct Cyc_Core_Opt*_tmp5C6=_cycalloc(sizeof(*
_tmp5C6));_tmp5C6->v=(void*)t;_tmp5C6;});}}int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*
e){void*_tmp5C7=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
_LL403: if(_tmp5C7 <= (void*)4)goto _LL40D;if(*((int*)_tmp5C7)!= 5)goto _LL405;
_LL404: goto _LL406;_LL405: if(*((int*)_tmp5C7)!= 12)goto _LL407;_LL406: goto _LL408;
_LL407: if(*((int*)_tmp5C7)!= 13)goto _LL409;_LL408: goto _LL40A;_LL409: if(*((int*)
_tmp5C7)!= 18)goto _LL40B;_LL40A: return 1;_LL40B: if(*((int*)_tmp5C7)!= 0)goto
_LL40D;_LL40C: return Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,
Cyc_Absyn_sint_typ);_LL40D:;_LL40E: return 0;_LL402:;}int Cyc_Tcutil_is_numeric(
struct Cyc_Absyn_Exp*e){if(Cyc_Tcutil_is_integral(e))return 1;{void*_tmp5C8=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);_LL410: if((int)_tmp5C8 != 1)
goto _LL412;_LL411: goto _LL413;_LL412: if(_tmp5C8 <= (void*)4)goto _LL414;if(*((int*)
_tmp5C8)!= 6)goto _LL414;_LL413: return 1;_LL414:;_LL415: return 0;_LL40F:;}}int Cyc_Tcutil_is_function_type(
void*t){void*_tmp5C9=Cyc_Tcutil_compress(t);_LL417: if(_tmp5C9 <= (void*)4)goto
_LL419;if(*((int*)_tmp5C9)!= 8)goto _LL419;_LL418: return 1;_LL419:;_LL41A: return 0;
_LL416:;}void*Cyc_Tcutil_max_arithmetic_type(void*t1,void*t2){struct _tuple0
_tmp5CB=({struct _tuple0 _tmp5CA;_tmp5CA.f1=t1;_tmp5CA.f2=t2;_tmp5CA;});void*
_tmp5CC;int _tmp5CD;void*_tmp5CE;int _tmp5CF;void*_tmp5D0;void*_tmp5D1;void*
_tmp5D2;void*_tmp5D3;void*_tmp5D4;void*_tmp5D5;void*_tmp5D6;void*_tmp5D7;void*
_tmp5D8;void*_tmp5D9;void*_tmp5DA;void*_tmp5DB;void*_tmp5DC;void*_tmp5DD;void*
_tmp5DE;void*_tmp5DF;void*_tmp5E0;void*_tmp5E1;void*_tmp5E2;void*_tmp5E3;void*
_tmp5E4;void*_tmp5E5;void*_tmp5E6;void*_tmp5E7;void*_tmp5E8;void*_tmp5E9;void*
_tmp5EA;void*_tmp5EB;void*_tmp5EC;void*_tmp5ED;_LL41C: _tmp5CC=_tmp5CB.f1;if(
_tmp5CC <= (void*)4)goto _LL41E;if(*((int*)_tmp5CC)!= 6)goto _LL41E;_tmp5CD=((
struct Cyc_Absyn_DoubleType_struct*)_tmp5CC)->f1;_tmp5CE=_tmp5CB.f2;if(_tmp5CE <= (
void*)4)goto _LL41E;if(*((int*)_tmp5CE)!= 6)goto _LL41E;_tmp5CF=((struct Cyc_Absyn_DoubleType_struct*)
_tmp5CE)->f1;_LL41D: if(_tmp5CD)return t1;else{return t2;}_LL41E: _tmp5D0=_tmp5CB.f1;
if(_tmp5D0 <= (void*)4)goto _LL420;if(*((int*)_tmp5D0)!= 6)goto _LL420;_LL41F:
return t1;_LL420: _tmp5D1=_tmp5CB.f2;if(_tmp5D1 <= (void*)4)goto _LL422;if(*((int*)
_tmp5D1)!= 6)goto _LL422;_LL421: return t2;_LL422: _tmp5D2=_tmp5CB.f1;if((int)
_tmp5D2 != 1)goto _LL424;_LL423: goto _LL425;_LL424: _tmp5D3=_tmp5CB.f2;if((int)
_tmp5D3 != 1)goto _LL426;_LL425: return(void*)1;_LL426: _tmp5D4=_tmp5CB.f1;if(
_tmp5D4 <= (void*)4)goto _LL428;if(*((int*)_tmp5D4)!= 5)goto _LL428;_tmp5D5=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp5D4)->f1;if((int)_tmp5D5 != 1)goto _LL428;
_tmp5D6=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp5D4)->f2;if((int)_tmp5D6 != 
4)goto _LL428;_LL427: goto _LL429;_LL428: _tmp5D7=_tmp5CB.f2;if(_tmp5D7 <= (void*)4)
goto _LL42A;if(*((int*)_tmp5D7)!= 5)goto _LL42A;_tmp5D8=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp5D7)->f1;if((int)_tmp5D8 != 1)goto _LL42A;_tmp5D9=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp5D7)->f2;if((int)_tmp5D9 != 4)goto _LL42A;_LL429: return Cyc_Absyn_ulonglong_typ;
_LL42A: _tmp5DA=_tmp5CB.f1;if(_tmp5DA <= (void*)4)goto _LL42C;if(*((int*)_tmp5DA)!= 
5)goto _LL42C;_tmp5DB=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp5DA)->f2;if((
int)_tmp5DB != 4)goto _LL42C;_LL42B: goto _LL42D;_LL42C: _tmp5DC=_tmp5CB.f2;if(
_tmp5DC <= (void*)4)goto _LL42E;if(*((int*)_tmp5DC)!= 5)goto _LL42E;_tmp5DD=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp5DC)->f2;if((int)_tmp5DD != 4)goto _LL42E;
_LL42D: return Cyc_Absyn_slonglong_typ;_LL42E: _tmp5DE=_tmp5CB.f1;if(_tmp5DE <= (
void*)4)goto _LL430;if(*((int*)_tmp5DE)!= 5)goto _LL430;_tmp5DF=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp5DE)->f1;if((int)_tmp5DF != 1)goto _LL430;_tmp5E0=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp5DE)->f2;if((int)_tmp5E0 != 3)goto _LL430;_LL42F: goto _LL431;_LL430: _tmp5E1=
_tmp5CB.f2;if(_tmp5E1 <= (void*)4)goto _LL432;if(*((int*)_tmp5E1)!= 5)goto _LL432;
_tmp5E2=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp5E1)->f1;if((int)_tmp5E2 != 
1)goto _LL432;_tmp5E3=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp5E1)->f2;if((
int)_tmp5E3 != 3)goto _LL432;_LL431: return Cyc_Absyn_ulong_typ;_LL432: _tmp5E4=
_tmp5CB.f1;if(_tmp5E4 <= (void*)4)goto _LL434;if(*((int*)_tmp5E4)!= 5)goto _LL434;
_tmp5E5=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp5E4)->f1;if((int)_tmp5E5 != 
1)goto _LL434;_tmp5E6=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp5E4)->f2;if((
int)_tmp5E6 != 2)goto _LL434;_LL433: goto _LL435;_LL434: _tmp5E7=_tmp5CB.f2;if(
_tmp5E7 <= (void*)4)goto _LL436;if(*((int*)_tmp5E7)!= 5)goto _LL436;_tmp5E8=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp5E7)->f1;if((int)_tmp5E8 != 1)goto _LL436;
_tmp5E9=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp5E7)->f2;if((int)_tmp5E9 != 
2)goto _LL436;_LL435: return Cyc_Absyn_uint_typ;_LL436: _tmp5EA=_tmp5CB.f1;if(
_tmp5EA <= (void*)4)goto _LL438;if(*((int*)_tmp5EA)!= 5)goto _LL438;_tmp5EB=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp5EA)->f2;if((int)_tmp5EB != 3)goto _LL438;
_LL437: goto _LL439;_LL438: _tmp5EC=_tmp5CB.f2;if(_tmp5EC <= (void*)4)goto _LL43A;if(*((
int*)_tmp5EC)!= 5)goto _LL43A;_tmp5ED=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp5EC)->f2;if((int)_tmp5ED != 3)goto _LL43A;_LL439: return Cyc_Absyn_slong_typ;
_LL43A:;_LL43B: return Cyc_Absyn_sint_typ;_LL41B:;}void Cyc_Tcutil_check_contains_assign(
struct Cyc_Absyn_Exp*e){void*_tmp5EE=(void*)e->r;struct Cyc_Core_Opt*_tmp5EF;
_LL43D: if(*((int*)_tmp5EE)!= 4)goto _LL43F;_tmp5EF=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmp5EE)->f2;if(_tmp5EF != 0)goto _LL43F;_LL43E:({void*_tmp5F0=0;Cyc_Tcutil_warn(e->loc,({
const char*_tmp5F1="assignment in test";_tag_dyneither(_tmp5F1,sizeof(char),19);}),
_tag_dyneither(_tmp5F0,sizeof(void*),0));});goto _LL43C;_LL43F:;_LL440: goto _LL43C;
_LL43C:;}static int Cyc_Tcutil_constrain_kinds(void*c1,void*c2){c1=Cyc_Absyn_compress_kb(
c1);c2=Cyc_Absyn_compress_kb(c2);{struct _tuple0 _tmp5F3=({struct _tuple0 _tmp5F2;
_tmp5F2.f1=c1;_tmp5F2.f2=c2;_tmp5F2;});void*_tmp5F4;void*_tmp5F5;void*_tmp5F6;
void*_tmp5F7;void*_tmp5F8;struct Cyc_Core_Opt*_tmp5F9;struct Cyc_Core_Opt**_tmp5FA;
void*_tmp5FB;struct Cyc_Core_Opt*_tmp5FC;struct Cyc_Core_Opt**_tmp5FD;void*_tmp5FE;
struct Cyc_Core_Opt*_tmp5FF;struct Cyc_Core_Opt**_tmp600;void*_tmp601;void*_tmp602;
void*_tmp603;void*_tmp604;void*_tmp605;void*_tmp606;struct Cyc_Core_Opt*_tmp607;
struct Cyc_Core_Opt**_tmp608;void*_tmp609;void*_tmp60A;struct Cyc_Core_Opt*_tmp60B;
struct Cyc_Core_Opt**_tmp60C;void*_tmp60D;void*_tmp60E;struct Cyc_Core_Opt*_tmp60F;
struct Cyc_Core_Opt**_tmp610;void*_tmp611;_LL442: _tmp5F4=_tmp5F3.f1;if(*((int*)
_tmp5F4)!= 0)goto _LL444;_tmp5F5=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp5F4)->f1;
_tmp5F6=_tmp5F3.f2;if(*((int*)_tmp5F6)!= 0)goto _LL444;_tmp5F7=(void*)((struct Cyc_Absyn_Eq_kb_struct*)
_tmp5F6)->f1;_LL443: return _tmp5F5 == _tmp5F7;_LL444: _tmp5F8=_tmp5F3.f2;if(*((int*)
_tmp5F8)!= 1)goto _LL446;_tmp5F9=((struct Cyc_Absyn_Unknown_kb_struct*)_tmp5F8)->f1;
_tmp5FA=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)_tmp5F8)->f1;
_LL445:*_tmp5FA=({struct Cyc_Core_Opt*_tmp612=_cycalloc(sizeof(*_tmp612));_tmp612->v=(
void*)c1;_tmp612;});return 1;_LL446: _tmp5FB=_tmp5F3.f1;if(*((int*)_tmp5FB)!= 1)
goto _LL448;_tmp5FC=((struct Cyc_Absyn_Unknown_kb_struct*)_tmp5FB)->f1;_tmp5FD=(
struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)_tmp5FB)->f1;_LL447:*
_tmp5FD=({struct Cyc_Core_Opt*_tmp613=_cycalloc(sizeof(*_tmp613));_tmp613->v=(
void*)c2;_tmp613;});return 1;_LL448: _tmp5FE=_tmp5F3.f1;if(*((int*)_tmp5FE)!= 2)
goto _LL44A;_tmp5FF=((struct Cyc_Absyn_Less_kb_struct*)_tmp5FE)->f1;_tmp600=(
struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)_tmp5FE)->f1;_tmp601=(
void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp5FE)->f2;_tmp602=_tmp5F3.f2;if(*((
int*)_tmp602)!= 0)goto _LL44A;_tmp603=(void*)((struct Cyc_Absyn_Eq_kb_struct*)
_tmp602)->f1;_LL449: if(Cyc_Tcutil_kind_leq(_tmp603,_tmp601)){*_tmp600=({struct
Cyc_Core_Opt*_tmp614=_cycalloc(sizeof(*_tmp614));_tmp614->v=(void*)c2;_tmp614;});
return 1;}else{return 0;}_LL44A: _tmp604=_tmp5F3.f1;if(*((int*)_tmp604)!= 0)goto
_LL44C;_tmp605=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp604)->f1;_tmp606=
_tmp5F3.f2;if(*((int*)_tmp606)!= 2)goto _LL44C;_tmp607=((struct Cyc_Absyn_Less_kb_struct*)
_tmp606)->f1;_tmp608=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp606)->f1;_tmp609=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp606)->f2;
_LL44B: if(Cyc_Tcutil_kind_leq(_tmp605,_tmp609)){*_tmp608=({struct Cyc_Core_Opt*
_tmp615=_cycalloc(sizeof(*_tmp615));_tmp615->v=(void*)c1;_tmp615;});return 1;}
else{return 0;}_LL44C: _tmp60A=_tmp5F3.f1;if(*((int*)_tmp60A)!= 2)goto _LL441;
_tmp60B=((struct Cyc_Absyn_Less_kb_struct*)_tmp60A)->f1;_tmp60C=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*)_tmp60A)->f1;_tmp60D=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp60A)->f2;_tmp60E=_tmp5F3.f2;if(*((int*)_tmp60E)!= 2)goto _LL441;_tmp60F=((
struct Cyc_Absyn_Less_kb_struct*)_tmp60E)->f1;_tmp610=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*)_tmp60E)->f1;_tmp611=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp60E)->f2;_LL44D: if(Cyc_Tcutil_kind_leq(_tmp60D,_tmp611)){*_tmp610=({struct
Cyc_Core_Opt*_tmp616=_cycalloc(sizeof(*_tmp616));_tmp616->v=(void*)c1;_tmp616;});
return 1;}else{if(Cyc_Tcutil_kind_leq(_tmp611,_tmp60D)){*_tmp60C=({struct Cyc_Core_Opt*
_tmp617=_cycalloc(sizeof(*_tmp617));_tmp617->v=(void*)c2;_tmp617;});return 1;}
else{return 0;}}_LL441:;}}static int Cyc_Tcutil_tvar_id_counter=0;int Cyc_Tcutil_new_tvar_id(){
return Cyc_Tcutil_tvar_id_counter ++;}static int Cyc_Tcutil_tvar_counter=0;struct Cyc_Absyn_Tvar*
Cyc_Tcutil_new_tvar(void*k){int i=Cyc_Tcutil_tvar_counter ++;struct _dyneither_ptr s=(
struct _dyneither_ptr)({struct Cyc_Int_pa_struct _tmp61C;_tmp61C.tag=1;_tmp61C.f1=(
unsigned long)i;{void*_tmp61A[1]={& _tmp61C};Cyc_aprintf(({const char*_tmp61B="#%d";
_tag_dyneither(_tmp61B,sizeof(char),4);}),_tag_dyneither(_tmp61A,sizeof(void*),1));}});
return({struct Cyc_Absyn_Tvar*_tmp618=_cycalloc(sizeof(*_tmp618));_tmp618->name=({
struct _dyneither_ptr*_tmp619=_cycalloc(sizeof(struct _dyneither_ptr)* 1);_tmp619[
0]=s;_tmp619;});_tmp618->identity=- 1;_tmp618->kind=(void*)k;_tmp618;});}int Cyc_Tcutil_is_temp_tvar(
struct Cyc_Absyn_Tvar*t){struct _dyneither_ptr _tmp61D=*t->name;return*((const char*)
_check_dyneither_subscript(_tmp61D,sizeof(char),0))== '#';}void Cyc_Tcutil_rewrite_temp_tvar(
struct Cyc_Absyn_Tvar*t){({struct Cyc_String_pa_struct _tmp620;_tmp620.tag=0;
_tmp620.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*t->name);{void*_tmp61E[
1]={& _tmp620};Cyc_printf(({const char*_tmp61F="%s";_tag_dyneither(_tmp61F,sizeof(
char),3);}),_tag_dyneither(_tmp61E,sizeof(void*),1));}});if(!Cyc_Tcutil_is_temp_tvar(
t))return;{struct _dyneither_ptr _tmp621=Cyc_strconcat(({const char*_tmp626="`";
_tag_dyneither(_tmp626,sizeof(char),2);}),(struct _dyneither_ptr)*t->name);({
struct _dyneither_ptr _tmp622=_dyneither_ptr_plus(_tmp621,sizeof(char),1);char
_tmp623=*((char*)_check_dyneither_subscript(_tmp622,sizeof(char),0));char _tmp624='t';
if(_get_dyneither_size(_tmp622,sizeof(char))== 1  && (_tmp623 == '\000'  && _tmp624
!= '\000'))_throw_arraybounds();*((char*)_tmp622.curr)=_tmp624;});t->name=({
struct _dyneither_ptr*_tmp625=_cycalloc(sizeof(struct _dyneither_ptr)* 1);_tmp625[
0]=(struct _dyneither_ptr)_tmp621;_tmp625;});}}struct _tuple14{struct
_dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};static struct _tuple3*Cyc_Tcutil_fndecl2typ_f(
struct _tuple14*x){return({struct _tuple3*_tmp627=_cycalloc(sizeof(*_tmp627));
_tmp627->f1=(struct Cyc_Core_Opt*)({struct Cyc_Core_Opt*_tmp628=_cycalloc(sizeof(*
_tmp628));_tmp628->v=(*x).f1;_tmp628;});_tmp627->f2=(*x).f2;_tmp627->f3=(*x).f3;
_tmp627;});}void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*fd){if(fd->cached_typ
== 0){struct Cyc_List_List*_tmp629=0;{struct Cyc_List_List*atts=fd->attributes;
for(0;atts != 0;atts=atts->tl){if(Cyc_Absyn_fntype_att((void*)atts->hd))_tmp629=({
struct Cyc_List_List*_tmp62A=_cycalloc(sizeof(*_tmp62A));_tmp62A->hd=(void*)((
void*)atts->hd);_tmp62A->tl=_tmp629;_tmp62A;});}}return(void*)({struct Cyc_Absyn_FnType_struct*
_tmp62B=_cycalloc(sizeof(*_tmp62B));_tmp62B[0]=({struct Cyc_Absyn_FnType_struct
_tmp62C;_tmp62C.tag=8;_tmp62C.f1=({struct Cyc_Absyn_FnInfo _tmp62D;_tmp62D.tvars=
fd->tvs;_tmp62D.effect=fd->effect;_tmp62D.ret_typ=(void*)((void*)fd->ret_type);
_tmp62D.args=((struct Cyc_List_List*(*)(struct _tuple3*(*f)(struct _tuple14*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_fndecl2typ_f,fd->args);_tmp62D.c_varargs=
fd->c_varargs;_tmp62D.cyc_varargs=fd->cyc_varargs;_tmp62D.rgn_po=fd->rgn_po;
_tmp62D.attributes=_tmp629;_tmp62D;});_tmp62C;});_tmp62B;});}return(void*)((
struct Cyc_Core_Opt*)_check_null(fd->cached_typ))->v;}struct _tuple15{void*f1;
struct Cyc_Absyn_Tqual f2;void*f3;};static void*Cyc_Tcutil_fst_fdarg(struct _tuple15*
t){return(*t).f1;}void*Cyc_Tcutil_snd_tqt(struct _tuple5*t){return(*t).f2;}static
struct _tuple5*Cyc_Tcutil_map2_tq(struct _tuple5*pr,void*t){return({struct _tuple5*
_tmp62E=_cycalloc(sizeof(*_tmp62E));_tmp62E->f1=(*pr).f1;_tmp62E->f2=t;_tmp62E;});}
struct _tuple16{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Tqual f2;};struct _tuple17{
struct _tuple16*f1;void*f2;};static struct _tuple17*Cyc_Tcutil_substitute_f1(struct
_RegionHandle*rgn,struct _tuple3*y){return({struct _tuple17*_tmp62F=_region_malloc(
rgn,sizeof(*_tmp62F));_tmp62F->f1=({struct _tuple16*_tmp630=_region_malloc(rgn,
sizeof(*_tmp630));_tmp630->f1=(*y).f1;_tmp630->f2=(*y).f2;_tmp630;});_tmp62F->f2=(*
y).f3;_tmp62F;});}static struct _tuple3*Cyc_Tcutil_substitute_f2(struct _tuple17*w){
struct _tuple16*_tmp632;void*_tmp633;struct _tuple17 _tmp631=*w;_tmp632=_tmp631.f1;
_tmp633=_tmp631.f2;{struct Cyc_Core_Opt*_tmp635;struct Cyc_Absyn_Tqual _tmp636;
struct _tuple16 _tmp634=*_tmp632;_tmp635=_tmp634.f1;_tmp636=_tmp634.f2;return({
struct _tuple3*_tmp637=_cycalloc(sizeof(*_tmp637));_tmp637->f1=_tmp635;_tmp637->f2=
_tmp636;_tmp637->f3=_tmp633;_tmp637;});}}static void*Cyc_Tcutil_field_type(struct
Cyc_Absyn_Aggrfield*f){return(void*)f->type;}static struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_zip_field_type(
struct Cyc_Absyn_Aggrfield*f,void*t){return({struct Cyc_Absyn_Aggrfield*_tmp638=
_cycalloc(sizeof(*_tmp638));_tmp638->name=f->name;_tmp638->tq=f->tq;_tmp638->type=(
void*)t;_tmp638->width=f->width;_tmp638->attributes=f->attributes;_tmp638;});}
static struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*rgn,struct Cyc_List_List*
inst,struct Cyc_List_List*ts);static struct Cyc_Absyn_Exp*Cyc_Tcutil_copye(struct
Cyc_Absyn_Exp*old,void*r){return({struct Cyc_Absyn_Exp*_tmp639=_cycalloc(sizeof(*
_tmp639));_tmp639->topt=old->topt;_tmp639->r=(void*)r;_tmp639->loc=old->loc;
_tmp639->annot=(void*)((void*)old->annot);_tmp639;});}static struct Cyc_Absyn_Exp*
Cyc_Tcutil_rsubsexp(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_Absyn_Exp*
e){void*_tmp63A=(void*)e->r;void*_tmp63B;struct Cyc_List_List*_tmp63C;struct Cyc_Absyn_Exp*
_tmp63D;struct Cyc_Absyn_Exp*_tmp63E;struct Cyc_Absyn_Exp*_tmp63F;struct Cyc_Absyn_Exp*
_tmp640;struct Cyc_Absyn_Exp*_tmp641;struct Cyc_Absyn_Exp*_tmp642;struct Cyc_Absyn_Exp*
_tmp643;struct Cyc_Absyn_Exp*_tmp644;struct Cyc_Absyn_Exp*_tmp645;void*_tmp646;
struct Cyc_Absyn_Exp*_tmp647;int _tmp648;void*_tmp649;void*_tmp64A;struct Cyc_Absyn_Exp*
_tmp64B;void*_tmp64C;void*_tmp64D;void*_tmp64E;_LL44F: if(*((int*)_tmp63A)!= 0)
goto _LL451;_LL450: goto _LL452;_LL451: if(*((int*)_tmp63A)!= 33)goto _LL453;_LL452:
goto _LL454;_LL453: if(*((int*)_tmp63A)!= 34)goto _LL455;_LL454: goto _LL456;_LL455:
if(*((int*)_tmp63A)!= 1)goto _LL457;_LL456: return e;_LL457: if(*((int*)_tmp63A)!= 3)
goto _LL459;_tmp63B=(void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp63A)->f1;
_tmp63C=((struct Cyc_Absyn_Primop_e_struct*)_tmp63A)->f2;_LL458: if(((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmp63C)== 1){struct Cyc_Absyn_Exp*
_tmp64F=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp63C))->hd;
struct Cyc_Absyn_Exp*_tmp650=Cyc_Tcutil_rsubsexp(r,inst,_tmp64F);if(_tmp650 == 
_tmp64F)return e;return Cyc_Tcutil_copye(e,(void*)({struct Cyc_Absyn_Primop_e_struct*
_tmp651=_cycalloc(sizeof(*_tmp651));_tmp651[0]=({struct Cyc_Absyn_Primop_e_struct
_tmp652;_tmp652.tag=3;_tmp652.f1=(void*)_tmp63B;_tmp652.f2=({struct Cyc_Absyn_Exp*
_tmp653[1];_tmp653[0]=_tmp650;((struct Cyc_List_List*(*)(struct _dyneither_ptr))
Cyc_List_list)(_tag_dyneither(_tmp653,sizeof(struct Cyc_Absyn_Exp*),1));});
_tmp652;});_tmp651;}));}else{if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp63C)== 2){struct Cyc_Absyn_Exp*_tmp654=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp63C))->hd;struct Cyc_Absyn_Exp*_tmp655=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp63C->tl))->hd;struct Cyc_Absyn_Exp*_tmp656=
Cyc_Tcutil_rsubsexp(r,inst,_tmp654);struct Cyc_Absyn_Exp*_tmp657=Cyc_Tcutil_rsubsexp(
r,inst,_tmp655);if(_tmp656 == _tmp654  && _tmp657 == _tmp655)return e;return Cyc_Tcutil_copye(
e,(void*)({struct Cyc_Absyn_Primop_e_struct*_tmp658=_cycalloc(sizeof(*_tmp658));
_tmp658[0]=({struct Cyc_Absyn_Primop_e_struct _tmp659;_tmp659.tag=3;_tmp659.f1=(
void*)_tmp63B;_tmp659.f2=({struct Cyc_Absyn_Exp*_tmp65A[2];_tmp65A[1]=_tmp657;
_tmp65A[0]=_tmp656;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmp65A,sizeof(struct Cyc_Absyn_Exp*),2));});_tmp659;});_tmp658;}));}
else{return({void*_tmp65B=0;((struct Cyc_Absyn_Exp*(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp65C="primop does not have 1 or 2 args!";
_tag_dyneither(_tmp65C,sizeof(char),34);}),_tag_dyneither(_tmp65B,sizeof(void*),
0));});}}_LL459: if(*((int*)_tmp63A)!= 6)goto _LL45B;_tmp63D=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp63A)->f1;_tmp63E=((struct Cyc_Absyn_Conditional_e_struct*)_tmp63A)->f2;
_tmp63F=((struct Cyc_Absyn_Conditional_e_struct*)_tmp63A)->f3;_LL45A: {struct Cyc_Absyn_Exp*
_tmp65D=Cyc_Tcutil_rsubsexp(r,inst,_tmp63D);struct Cyc_Absyn_Exp*_tmp65E=Cyc_Tcutil_rsubsexp(
r,inst,_tmp63E);struct Cyc_Absyn_Exp*_tmp65F=Cyc_Tcutil_rsubsexp(r,inst,_tmp63F);
if((_tmp65D == _tmp63D  && _tmp65E == _tmp63E) && _tmp65F == _tmp63F)return e;return
Cyc_Tcutil_copye(e,(void*)({struct Cyc_Absyn_Conditional_e_struct*_tmp660=
_cycalloc(sizeof(*_tmp660));_tmp660[0]=({struct Cyc_Absyn_Conditional_e_struct
_tmp661;_tmp661.tag=6;_tmp661.f1=_tmp65D;_tmp661.f2=_tmp65E;_tmp661.f3=_tmp65F;
_tmp661;});_tmp660;}));}_LL45B: if(*((int*)_tmp63A)!= 7)goto _LL45D;_tmp640=((
struct Cyc_Absyn_And_e_struct*)_tmp63A)->f1;_tmp641=((struct Cyc_Absyn_And_e_struct*)
_tmp63A)->f2;_LL45C: {struct Cyc_Absyn_Exp*_tmp662=Cyc_Tcutil_rsubsexp(r,inst,
_tmp640);struct Cyc_Absyn_Exp*_tmp663=Cyc_Tcutil_rsubsexp(r,inst,_tmp641);if(
_tmp662 == _tmp640  && _tmp663 == _tmp641)return e;return Cyc_Tcutil_copye(e,(void*)({
struct Cyc_Absyn_And_e_struct*_tmp664=_cycalloc(sizeof(*_tmp664));_tmp664[0]=({
struct Cyc_Absyn_And_e_struct _tmp665;_tmp665.tag=7;_tmp665.f1=_tmp662;_tmp665.f2=
_tmp663;_tmp665;});_tmp664;}));}_LL45D: if(*((int*)_tmp63A)!= 8)goto _LL45F;
_tmp642=((struct Cyc_Absyn_Or_e_struct*)_tmp63A)->f1;_tmp643=((struct Cyc_Absyn_Or_e_struct*)
_tmp63A)->f2;_LL45E: {struct Cyc_Absyn_Exp*_tmp666=Cyc_Tcutil_rsubsexp(r,inst,
_tmp642);struct Cyc_Absyn_Exp*_tmp667=Cyc_Tcutil_rsubsexp(r,inst,_tmp643);if(
_tmp666 == _tmp642  && _tmp667 == _tmp643)return e;return Cyc_Tcutil_copye(e,(void*)({
struct Cyc_Absyn_Or_e_struct*_tmp668=_cycalloc(sizeof(*_tmp668));_tmp668[0]=({
struct Cyc_Absyn_Or_e_struct _tmp669;_tmp669.tag=8;_tmp669.f1=_tmp666;_tmp669.f2=
_tmp667;_tmp669;});_tmp668;}));}_LL45F: if(*((int*)_tmp63A)!= 9)goto _LL461;
_tmp644=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp63A)->f1;_tmp645=((struct Cyc_Absyn_SeqExp_e_struct*)
_tmp63A)->f2;_LL460: {struct Cyc_Absyn_Exp*_tmp66A=Cyc_Tcutil_rsubsexp(r,inst,
_tmp644);struct Cyc_Absyn_Exp*_tmp66B=Cyc_Tcutil_rsubsexp(r,inst,_tmp645);if(
_tmp66A == _tmp644  && _tmp66B == _tmp645)return e;return Cyc_Tcutil_copye(e,(void*)({
struct Cyc_Absyn_SeqExp_e_struct*_tmp66C=_cycalloc(sizeof(*_tmp66C));_tmp66C[0]=({
struct Cyc_Absyn_SeqExp_e_struct _tmp66D;_tmp66D.tag=9;_tmp66D.f1=_tmp66A;_tmp66D.f2=
_tmp66B;_tmp66D;});_tmp66C;}));}_LL461: if(*((int*)_tmp63A)!= 15)goto _LL463;
_tmp646=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp63A)->f1;_tmp647=((struct Cyc_Absyn_Cast_e_struct*)
_tmp63A)->f2;_tmp648=((struct Cyc_Absyn_Cast_e_struct*)_tmp63A)->f3;_tmp649=(void*)((
struct Cyc_Absyn_Cast_e_struct*)_tmp63A)->f4;_LL462: {struct Cyc_Absyn_Exp*_tmp66E=
Cyc_Tcutil_rsubsexp(r,inst,_tmp647);void*_tmp66F=Cyc_Tcutil_rsubstitute(r,inst,
_tmp646);if(_tmp66E == _tmp647  && _tmp66F == _tmp646)return e;return Cyc_Tcutil_copye(
e,(void*)({struct Cyc_Absyn_Cast_e_struct*_tmp670=_cycalloc(sizeof(*_tmp670));
_tmp670[0]=({struct Cyc_Absyn_Cast_e_struct _tmp671;_tmp671.tag=15;_tmp671.f1=(
void*)_tmp66F;_tmp671.f2=_tmp66E;_tmp671.f3=_tmp648;_tmp671.f4=(void*)_tmp649;
_tmp671;});_tmp670;}));}_LL463: if(*((int*)_tmp63A)!= 18)goto _LL465;_tmp64A=(void*)((
struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp63A)->f1;_LL464: {void*_tmp672=Cyc_Tcutil_rsubstitute(
r,inst,_tmp64A);if(_tmp672 == _tmp64A)return e;return Cyc_Tcutil_copye(e,(void*)({
struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp673=_cycalloc(sizeof(*_tmp673));_tmp673[0]=({
struct Cyc_Absyn_Sizeoftyp_e_struct _tmp674;_tmp674.tag=18;_tmp674.f1=(void*)
_tmp672;_tmp674;});_tmp673;}));}_LL465: if(*((int*)_tmp63A)!= 19)goto _LL467;
_tmp64B=((struct Cyc_Absyn_Sizeofexp_e_struct*)_tmp63A)->f1;_LL466: {struct Cyc_Absyn_Exp*
_tmp675=Cyc_Tcutil_rsubsexp(r,inst,_tmp64B);if(_tmp675 == _tmp64B)return e;return
Cyc_Tcutil_copye(e,(void*)({struct Cyc_Absyn_Sizeofexp_e_struct*_tmp676=_cycalloc(
sizeof(*_tmp676));_tmp676[0]=({struct Cyc_Absyn_Sizeofexp_e_struct _tmp677;_tmp677.tag=
19;_tmp677.f1=_tmp675;_tmp677;});_tmp676;}));}_LL467: if(*((int*)_tmp63A)!= 20)
goto _LL469;_tmp64C=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp63A)->f1;
_tmp64D=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp63A)->f2;_LL468: {void*
_tmp678=Cyc_Tcutil_rsubstitute(r,inst,_tmp64C);if(_tmp678 == _tmp64C)return e;
return Cyc_Tcutil_copye(e,(void*)({struct Cyc_Absyn_Offsetof_e_struct*_tmp679=
_cycalloc(sizeof(*_tmp679));_tmp679[0]=({struct Cyc_Absyn_Offsetof_e_struct
_tmp67A;_tmp67A.tag=20;_tmp67A.f1=(void*)_tmp678;_tmp67A.f2=(void*)_tmp64D;
_tmp67A;});_tmp679;}));}_LL469: if(*((int*)_tmp63A)!= 39)goto _LL46B;_tmp64E=(void*)((
struct Cyc_Absyn_Valueof_e_struct*)_tmp63A)->f1;_LL46A: {void*_tmp67B=Cyc_Tcutil_rsubstitute(
r,inst,_tmp64E);if(_tmp67B == _tmp64E)return e;return Cyc_Tcutil_copye(e,(void*)({
struct Cyc_Absyn_Valueof_e_struct*_tmp67C=_cycalloc(sizeof(*_tmp67C));_tmp67C[0]=({
struct Cyc_Absyn_Valueof_e_struct _tmp67D;_tmp67D.tag=39;_tmp67D.f1=(void*)_tmp67B;
_tmp67D;});_tmp67C;}));}_LL46B:;_LL46C: return({void*_tmp67E=0;((struct Cyc_Absyn_Exp*(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmp67F="non-type-level-expression in Tcutil::rsubsexp";_tag_dyneither(_tmp67F,
sizeof(char),46);}),_tag_dyneither(_tmp67E,sizeof(void*),0));});_LL44E:;}void*
Cyc_Tcutil_rsubstitute(struct _RegionHandle*rgn,struct Cyc_List_List*inst,void*t){
void*_tmp680=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*_tmp681;struct Cyc_Absyn_AggrInfo
_tmp682;union Cyc_Absyn_AggrInfoU_union _tmp683;struct Cyc_List_List*_tmp684;struct
Cyc_Absyn_TunionInfo _tmp685;union Cyc_Absyn_TunionInfoU_union _tmp686;struct Cyc_List_List*
_tmp687;struct Cyc_Core_Opt*_tmp688;struct Cyc_Absyn_TunionFieldInfo _tmp689;union
Cyc_Absyn_TunionFieldInfoU_union _tmp68A;struct Cyc_List_List*_tmp68B;struct
_tuple2*_tmp68C;struct Cyc_List_List*_tmp68D;struct Cyc_Absyn_Typedefdecl*_tmp68E;
void**_tmp68F;struct Cyc_Absyn_ArrayInfo _tmp690;void*_tmp691;struct Cyc_Absyn_Tqual
_tmp692;struct Cyc_Absyn_Exp*_tmp693;struct Cyc_Absyn_Conref*_tmp694;struct Cyc_Position_Segment*
_tmp695;struct Cyc_Absyn_PtrInfo _tmp696;void*_tmp697;struct Cyc_Absyn_Tqual _tmp698;
struct Cyc_Absyn_PtrAtts _tmp699;void*_tmp69A;struct Cyc_Absyn_Conref*_tmp69B;
struct Cyc_Absyn_Conref*_tmp69C;struct Cyc_Absyn_Conref*_tmp69D;struct Cyc_Absyn_FnInfo
_tmp69E;struct Cyc_List_List*_tmp69F;struct Cyc_Core_Opt*_tmp6A0;void*_tmp6A1;
struct Cyc_List_List*_tmp6A2;int _tmp6A3;struct Cyc_Absyn_VarargInfo*_tmp6A4;struct
Cyc_List_List*_tmp6A5;struct Cyc_List_List*_tmp6A6;struct Cyc_List_List*_tmp6A7;
void*_tmp6A8;struct Cyc_List_List*_tmp6A9;struct Cyc_Core_Opt*_tmp6AA;void*_tmp6AB;
void*_tmp6AC;void*_tmp6AD;void*_tmp6AE;struct Cyc_Absyn_Exp*_tmp6AF;void*_tmp6B0;
void*_tmp6B1;struct Cyc_List_List*_tmp6B2;_LL46E: if(_tmp680 <= (void*)4)goto _LL490;
if(*((int*)_tmp680)!= 1)goto _LL470;_tmp681=((struct Cyc_Absyn_VarType_struct*)
_tmp680)->f1;_LL46F: {struct _handler_cons _tmp6B3;_push_handler(& _tmp6B3);{int
_tmp6B5=0;if(setjmp(_tmp6B3.handler))_tmp6B5=1;if(!_tmp6B5){{void*_tmp6B6=((void*(*)(
int(*cmp)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,
struct Cyc_Absyn_Tvar*x))Cyc_List_assoc_cmp)(Cyc_Absyn_tvar_cmp,inst,_tmp681);
_npop_handler(0);return _tmp6B6;};_pop_handler();}else{void*_tmp6B4=(void*)
_exn_thrown;void*_tmp6B8=_tmp6B4;_LL4A3: if(_tmp6B8 != Cyc_Core_Not_found)goto
_LL4A5;_LL4A4: return t;_LL4A5:;_LL4A6:(void)_throw(_tmp6B8);_LL4A2:;}}}_LL470: if(*((
int*)_tmp680)!= 10)goto _LL472;_tmp682=((struct Cyc_Absyn_AggrType_struct*)_tmp680)->f1;
_tmp683=_tmp682.aggr_info;_tmp684=_tmp682.targs;_LL471: {struct Cyc_List_List*
_tmp6B9=Cyc_Tcutil_substs(rgn,inst,_tmp684);return _tmp6B9 == _tmp684?t:(void*)({
struct Cyc_Absyn_AggrType_struct*_tmp6BA=_cycalloc(sizeof(*_tmp6BA));_tmp6BA[0]=({
struct Cyc_Absyn_AggrType_struct _tmp6BB;_tmp6BB.tag=10;_tmp6BB.f1=({struct Cyc_Absyn_AggrInfo
_tmp6BC;_tmp6BC.aggr_info=_tmp683;_tmp6BC.targs=_tmp6B9;_tmp6BC;});_tmp6BB;});
_tmp6BA;});}_LL472: if(*((int*)_tmp680)!= 2)goto _LL474;_tmp685=((struct Cyc_Absyn_TunionType_struct*)
_tmp680)->f1;_tmp686=_tmp685.tunion_info;_tmp687=_tmp685.targs;_tmp688=_tmp685.rgn;
_LL473: {struct Cyc_List_List*_tmp6BD=Cyc_Tcutil_substs(rgn,inst,_tmp687);struct
Cyc_Core_Opt*new_r;if((unsigned int)_tmp688){void*_tmp6BE=Cyc_Tcutil_rsubstitute(
rgn,inst,(void*)_tmp688->v);if(_tmp6BE == (void*)_tmp688->v)new_r=_tmp688;else{
new_r=({struct Cyc_Core_Opt*_tmp6BF=_cycalloc(sizeof(*_tmp6BF));_tmp6BF->v=(void*)
_tmp6BE;_tmp6BF;});}}else{new_r=_tmp688;}return _tmp6BD == _tmp687  && new_r == 
_tmp688?t:(void*)({struct Cyc_Absyn_TunionType_struct*_tmp6C0=_cycalloc(sizeof(*
_tmp6C0));_tmp6C0[0]=({struct Cyc_Absyn_TunionType_struct _tmp6C1;_tmp6C1.tag=2;
_tmp6C1.f1=({struct Cyc_Absyn_TunionInfo _tmp6C2;_tmp6C2.tunion_info=_tmp686;
_tmp6C2.targs=_tmp6BD;_tmp6C2.rgn=new_r;_tmp6C2;});_tmp6C1;});_tmp6C0;});}_LL474:
if(*((int*)_tmp680)!= 3)goto _LL476;_tmp689=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp680)->f1;_tmp68A=_tmp689.field_info;_tmp68B=_tmp689.targs;_LL475: {struct Cyc_List_List*
_tmp6C3=Cyc_Tcutil_substs(rgn,inst,_tmp68B);return _tmp6C3 == _tmp68B?t:(void*)({
struct Cyc_Absyn_TunionFieldType_struct*_tmp6C4=_cycalloc(sizeof(*_tmp6C4));
_tmp6C4[0]=({struct Cyc_Absyn_TunionFieldType_struct _tmp6C5;_tmp6C5.tag=3;_tmp6C5.f1=({
struct Cyc_Absyn_TunionFieldInfo _tmp6C6;_tmp6C6.field_info=_tmp68A;_tmp6C6.targs=
_tmp6C3;_tmp6C6;});_tmp6C5;});_tmp6C4;});}_LL476: if(*((int*)_tmp680)!= 16)goto
_LL478;_tmp68C=((struct Cyc_Absyn_TypedefType_struct*)_tmp680)->f1;_tmp68D=((
struct Cyc_Absyn_TypedefType_struct*)_tmp680)->f2;_tmp68E=((struct Cyc_Absyn_TypedefType_struct*)
_tmp680)->f3;_tmp68F=((struct Cyc_Absyn_TypedefType_struct*)_tmp680)->f4;_LL477: {
struct Cyc_List_List*_tmp6C7=Cyc_Tcutil_substs(rgn,inst,_tmp68D);return _tmp6C7 == 
_tmp68D?t:(void*)({struct Cyc_Absyn_TypedefType_struct*_tmp6C8=_cycalloc(sizeof(*
_tmp6C8));_tmp6C8[0]=({struct Cyc_Absyn_TypedefType_struct _tmp6C9;_tmp6C9.tag=16;
_tmp6C9.f1=_tmp68C;_tmp6C9.f2=_tmp6C7;_tmp6C9.f3=_tmp68E;_tmp6C9.f4=_tmp68F;
_tmp6C9;});_tmp6C8;});}_LL478: if(*((int*)_tmp680)!= 7)goto _LL47A;_tmp690=((
struct Cyc_Absyn_ArrayType_struct*)_tmp680)->f1;_tmp691=(void*)_tmp690.elt_type;
_tmp692=_tmp690.tq;_tmp693=_tmp690.num_elts;_tmp694=_tmp690.zero_term;_tmp695=
_tmp690.zt_loc;_LL479: {void*_tmp6CA=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp691);
return _tmp6CA == _tmp691?t:(void*)({struct Cyc_Absyn_ArrayType_struct*_tmp6CB=
_cycalloc(sizeof(*_tmp6CB));_tmp6CB[0]=({struct Cyc_Absyn_ArrayType_struct _tmp6CC;
_tmp6CC.tag=7;_tmp6CC.f1=({struct Cyc_Absyn_ArrayInfo _tmp6CD;_tmp6CD.elt_type=(
void*)_tmp6CA;_tmp6CD.tq=_tmp692;_tmp6CD.num_elts=_tmp693;_tmp6CD.zero_term=
_tmp694;_tmp6CD.zt_loc=_tmp695;_tmp6CD;});_tmp6CC;});_tmp6CB;});}_LL47A: if(*((
int*)_tmp680)!= 4)goto _LL47C;_tmp696=((struct Cyc_Absyn_PointerType_struct*)
_tmp680)->f1;_tmp697=(void*)_tmp696.elt_typ;_tmp698=_tmp696.elt_tq;_tmp699=
_tmp696.ptr_atts;_tmp69A=(void*)_tmp699.rgn;_tmp69B=_tmp699.nullable;_tmp69C=
_tmp699.bounds;_tmp69D=_tmp699.zero_term;_LL47B: {void*_tmp6CE=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmp697);void*_tmp6CF=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp69A);struct
Cyc_Absyn_Conref*_tmp6D0=_tmp69C;{union Cyc_Absyn_Constraint_union _tmp6D1=(Cyc_Absyn_compress_conref(
_tmp69C))->v;void*_tmp6D2;struct Cyc_Absyn_Exp*_tmp6D3;_LL4A8: if((_tmp6D1.Eq_constr).tag
!= 0)goto _LL4AA;_tmp6D2=(_tmp6D1.Eq_constr).f1;if(_tmp6D2 <= (void*)1)goto _LL4AA;
if(*((int*)_tmp6D2)!= 0)goto _LL4AA;_tmp6D3=((struct Cyc_Absyn_Upper_b_struct*)
_tmp6D2)->f1;_LL4A9: {struct Cyc_Absyn_Exp*_tmp6D4=Cyc_Tcutil_rsubsexp(rgn,inst,
_tmp6D3);if(_tmp6D4 != _tmp6D3)_tmp6D0=Cyc_Absyn_new_conref((void*)({struct Cyc_Absyn_Upper_b_struct*
_tmp6D5=_cycalloc(sizeof(*_tmp6D5));_tmp6D5[0]=({struct Cyc_Absyn_Upper_b_struct
_tmp6D6;_tmp6D6.tag=0;_tmp6D6.f1=_tmp6D4;_tmp6D6;});_tmp6D5;}));goto _LL4A7;}
_LL4AA:;_LL4AB: goto _LL4A7;_LL4A7:;}if((_tmp6CE == _tmp697  && _tmp6CF == _tmp69A)
 && _tmp6D0 == _tmp69C)return t;return(void*)({struct Cyc_Absyn_PointerType_struct*
_tmp6D7=_cycalloc(sizeof(*_tmp6D7));_tmp6D7[0]=({struct Cyc_Absyn_PointerType_struct
_tmp6D8;_tmp6D8.tag=4;_tmp6D8.f1=({struct Cyc_Absyn_PtrInfo _tmp6D9;_tmp6D9.elt_typ=(
void*)_tmp6CE;_tmp6D9.elt_tq=_tmp698;_tmp6D9.ptr_atts=({struct Cyc_Absyn_PtrAtts
_tmp6DA;_tmp6DA.rgn=(void*)_tmp6CF;_tmp6DA.nullable=_tmp69B;_tmp6DA.bounds=
_tmp6D0;_tmp6DA.zero_term=_tmp69D;_tmp6DA.ptrloc=0;_tmp6DA;});_tmp6D9;});_tmp6D8;});
_tmp6D7;});}_LL47C: if(*((int*)_tmp680)!= 8)goto _LL47E;_tmp69E=((struct Cyc_Absyn_FnType_struct*)
_tmp680)->f1;_tmp69F=_tmp69E.tvars;_tmp6A0=_tmp69E.effect;_tmp6A1=(void*)_tmp69E.ret_typ;
_tmp6A2=_tmp69E.args;_tmp6A3=_tmp69E.c_varargs;_tmp6A4=_tmp69E.cyc_varargs;
_tmp6A5=_tmp69E.rgn_po;_tmp6A6=_tmp69E.attributes;_LL47D:{struct Cyc_List_List*
_tmp6DB=_tmp69F;for(0;_tmp6DB != 0;_tmp6DB=_tmp6DB->tl){inst=({struct Cyc_List_List*
_tmp6DC=_region_malloc(rgn,sizeof(*_tmp6DC));_tmp6DC->hd=({struct _tuple8*_tmp6DD=
_region_malloc(rgn,sizeof(*_tmp6DD));_tmp6DD->f1=(struct Cyc_Absyn_Tvar*)_tmp6DB->hd;
_tmp6DD->f2=(void*)({struct Cyc_Absyn_VarType_struct*_tmp6DE=_cycalloc(sizeof(*
_tmp6DE));_tmp6DE[0]=({struct Cyc_Absyn_VarType_struct _tmp6DF;_tmp6DF.tag=1;
_tmp6DF.f1=(struct Cyc_Absyn_Tvar*)_tmp6DB->hd;_tmp6DF;});_tmp6DE;});_tmp6DD;});
_tmp6DC->tl=inst;_tmp6DC;});}}{struct Cyc_List_List*_tmp6E1;struct Cyc_List_List*
_tmp6E2;struct _tuple1 _tmp6E0=((struct _tuple1(*)(struct _RegionHandle*r1,struct
_RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(rgn,rgn,((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _tuple17*(*f)(struct _RegionHandle*,struct _tuple3*),
struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_substitute_f1,
rgn,_tmp6A2));_tmp6E1=_tmp6E0.f1;_tmp6E2=_tmp6E0.f2;{struct Cyc_List_List*_tmp6E3=
Cyc_Tcutil_substs(rgn,inst,_tmp6E2);struct Cyc_List_List*_tmp6E4=((struct Cyc_List_List*(*)(
struct _tuple3*(*f)(struct _tuple17*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_substitute_f2,((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmp6E1,_tmp6E3));struct Cyc_Core_Opt*
eff2;if(_tmp6A0 == 0)eff2=0;else{void*_tmp6E5=Cyc_Tcutil_rsubstitute(rgn,inst,(
void*)_tmp6A0->v);if(_tmp6E5 == (void*)_tmp6A0->v)eff2=_tmp6A0;else{eff2=({struct
Cyc_Core_Opt*_tmp6E6=_cycalloc(sizeof(*_tmp6E6));_tmp6E6->v=(void*)_tmp6E5;
_tmp6E6;});}}{struct Cyc_Absyn_VarargInfo*cyc_varargs2;if(_tmp6A4 == 0)
cyc_varargs2=0;else{struct Cyc_Core_Opt*_tmp6E8;struct Cyc_Absyn_Tqual _tmp6E9;void*
_tmp6EA;int _tmp6EB;struct Cyc_Absyn_VarargInfo _tmp6E7=*_tmp6A4;_tmp6E8=_tmp6E7.name;
_tmp6E9=_tmp6E7.tq;_tmp6EA=(void*)_tmp6E7.type;_tmp6EB=_tmp6E7.inject;{void*
_tmp6EC=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6EA);if(_tmp6EC == _tmp6EA)
cyc_varargs2=_tmp6A4;else{cyc_varargs2=({struct Cyc_Absyn_VarargInfo*_tmp6ED=
_cycalloc(sizeof(*_tmp6ED));_tmp6ED->name=_tmp6E8;_tmp6ED->tq=_tmp6E9;_tmp6ED->type=(
void*)_tmp6EC;_tmp6ED->inject=_tmp6EB;_tmp6ED;});}}}{struct Cyc_List_List*rgn_po2;
struct Cyc_List_List*_tmp6EF;struct Cyc_List_List*_tmp6F0;struct _tuple1 _tmp6EE=Cyc_List_rsplit(
rgn,rgn,_tmp6A5);_tmp6EF=_tmp6EE.f1;_tmp6F0=_tmp6EE.f2;{struct Cyc_List_List*
_tmp6F1=Cyc_Tcutil_substs(rgn,inst,_tmp6EF);struct Cyc_List_List*_tmp6F2=Cyc_Tcutil_substs(
rgn,inst,_tmp6F0);if(_tmp6F1 == _tmp6EF  && _tmp6F2 == _tmp6F0)rgn_po2=_tmp6A5;
else{rgn_po2=Cyc_List_zip(_tmp6F1,_tmp6F2);}return(void*)({struct Cyc_Absyn_FnType_struct*
_tmp6F3=_cycalloc(sizeof(*_tmp6F3));_tmp6F3[0]=({struct Cyc_Absyn_FnType_struct
_tmp6F4;_tmp6F4.tag=8;_tmp6F4.f1=({struct Cyc_Absyn_FnInfo _tmp6F5;_tmp6F5.tvars=
_tmp69F;_tmp6F5.effect=eff2;_tmp6F5.ret_typ=(void*)Cyc_Tcutil_rsubstitute(rgn,
inst,_tmp6A1);_tmp6F5.args=_tmp6E4;_tmp6F5.c_varargs=_tmp6A3;_tmp6F5.cyc_varargs=
cyc_varargs2;_tmp6F5.rgn_po=rgn_po2;_tmp6F5.attributes=_tmp6A6;_tmp6F5;});
_tmp6F4;});_tmp6F3;});}}}}}_LL47E: if(*((int*)_tmp680)!= 9)goto _LL480;_tmp6A7=((
struct Cyc_Absyn_TupleType_struct*)_tmp680)->f1;_LL47F: {struct Cyc_List_List*
_tmp6F6=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _tuple5*),
struct Cyc_List_List*x))Cyc_List_rmap)(rgn,Cyc_Tcutil_snd_tqt,_tmp6A7);struct Cyc_List_List*
_tmp6F7=Cyc_Tcutil_substs(rgn,inst,_tmp6F6);if(_tmp6F7 == _tmp6F6)return t;{struct
Cyc_List_List*_tmp6F8=((struct Cyc_List_List*(*)(struct _tuple5*(*f)(struct _tuple5*,
void*),struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_map2_tq,
_tmp6A7,_tmp6F7);return(void*)({struct Cyc_Absyn_TupleType_struct*_tmp6F9=
_cycalloc(sizeof(*_tmp6F9));_tmp6F9[0]=({struct Cyc_Absyn_TupleType_struct _tmp6FA;
_tmp6FA.tag=9;_tmp6FA.f1=_tmp6F8;_tmp6FA;});_tmp6F9;});}}_LL480: if(*((int*)
_tmp680)!= 11)goto _LL482;_tmp6A8=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp680)->f1;_tmp6A9=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp680)->f2;_LL481: {
struct Cyc_List_List*_tmp6FB=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*
f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_rmap)(rgn,Cyc_Tcutil_field_type,
_tmp6A9);struct Cyc_List_List*_tmp6FC=Cyc_Tcutil_substs(rgn,inst,_tmp6FB);if(
_tmp6FC == _tmp6FB)return t;{struct Cyc_List_List*_tmp6FD=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*,void*),struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_zip_field_type,_tmp6A9,
_tmp6FC);return(void*)({struct Cyc_Absyn_AnonAggrType_struct*_tmp6FE=_cycalloc(
sizeof(*_tmp6FE));_tmp6FE[0]=({struct Cyc_Absyn_AnonAggrType_struct _tmp6FF;
_tmp6FF.tag=11;_tmp6FF.f1=(void*)_tmp6A8;_tmp6FF.f2=_tmp6FD;_tmp6FF;});_tmp6FE;});}}
_LL482: if(*((int*)_tmp680)!= 0)goto _LL484;_tmp6AA=((struct Cyc_Absyn_Evar_struct*)
_tmp680)->f2;_LL483: if(_tmp6AA != 0)return Cyc_Tcutil_rsubstitute(rgn,inst,(void*)
_tmp6AA->v);else{return t;}_LL484: if(*((int*)_tmp680)!= 14)goto _LL486;_tmp6AB=(
void*)((struct Cyc_Absyn_RgnHandleType_struct*)_tmp680)->f1;_LL485: {void*_tmp700=
Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6AB);return _tmp700 == _tmp6AB?t:(void*)({
struct Cyc_Absyn_RgnHandleType_struct*_tmp701=_cycalloc(sizeof(*_tmp701));_tmp701[
0]=({struct Cyc_Absyn_RgnHandleType_struct _tmp702;_tmp702.tag=14;_tmp702.f1=(void*)
_tmp700;_tmp702;});_tmp701;});}_LL486: if(*((int*)_tmp680)!= 15)goto _LL488;
_tmp6AC=(void*)((struct Cyc_Absyn_DynRgnType_struct*)_tmp680)->f1;_tmp6AD=(void*)((
struct Cyc_Absyn_DynRgnType_struct*)_tmp680)->f2;_LL487: {void*_tmp703=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmp6AC);void*_tmp704=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6AD);return
_tmp703 == _tmp6AC  && _tmp704 == _tmp6AD?t:(void*)({struct Cyc_Absyn_DynRgnType_struct*
_tmp705=_cycalloc(sizeof(*_tmp705));_tmp705[0]=({struct Cyc_Absyn_DynRgnType_struct
_tmp706;_tmp706.tag=15;_tmp706.f1=(void*)_tmp703;_tmp706.f2=(void*)_tmp704;
_tmp706;});_tmp705;});}_LL488: if(*((int*)_tmp680)!= 18)goto _LL48A;_tmp6AE=(void*)((
struct Cyc_Absyn_TagType_struct*)_tmp680)->f1;_LL489: {void*_tmp707=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmp6AE);return _tmp707 == _tmp6AE?t:(void*)({struct Cyc_Absyn_TagType_struct*
_tmp708=_cycalloc(sizeof(*_tmp708));_tmp708[0]=({struct Cyc_Absyn_TagType_struct
_tmp709;_tmp709.tag=18;_tmp709.f1=(void*)_tmp707;_tmp709;});_tmp708;});}_LL48A:
if(*((int*)_tmp680)!= 17)goto _LL48C;_tmp6AF=((struct Cyc_Absyn_ValueofType_struct*)
_tmp680)->f1;_LL48B: {struct Cyc_Absyn_Exp*_tmp70A=Cyc_Tcutil_rsubsexp(rgn,inst,
_tmp6AF);return _tmp70A == _tmp6AF?t:(void*)({struct Cyc_Absyn_ValueofType_struct*
_tmp70B=_cycalloc(sizeof(*_tmp70B));_tmp70B[0]=({struct Cyc_Absyn_ValueofType_struct
_tmp70C;_tmp70C.tag=17;_tmp70C.f1=_tmp70A;_tmp70C;});_tmp70B;});}_LL48C: if(*((
int*)_tmp680)!= 12)goto _LL48E;_LL48D: goto _LL48F;_LL48E: if(*((int*)_tmp680)!= 13)
goto _LL490;_LL48F: goto _LL491;_LL490: if((int)_tmp680 != 0)goto _LL492;_LL491: goto
_LL493;_LL492: if(_tmp680 <= (void*)4)goto _LL494;if(*((int*)_tmp680)!= 5)goto
_LL494;_LL493: goto _LL495;_LL494: if((int)_tmp680 != 1)goto _LL496;_LL495: goto _LL497;
_LL496: if(_tmp680 <= (void*)4)goto _LL498;if(*((int*)_tmp680)!= 6)goto _LL498;
_LL497: goto _LL499;_LL498: if((int)_tmp680 != 3)goto _LL49A;_LL499: goto _LL49B;_LL49A:
if((int)_tmp680 != 2)goto _LL49C;_LL49B: return t;_LL49C: if(_tmp680 <= (void*)4)goto
_LL49E;if(*((int*)_tmp680)!= 21)goto _LL49E;_tmp6B0=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp680)->f1;_LL49D: {void*_tmp70D=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6B0);
return _tmp70D == _tmp6B0?t:(void*)({struct Cyc_Absyn_RgnsEff_struct*_tmp70E=
_cycalloc(sizeof(*_tmp70E));_tmp70E[0]=({struct Cyc_Absyn_RgnsEff_struct _tmp70F;
_tmp70F.tag=21;_tmp70F.f1=(void*)_tmp70D;_tmp70F;});_tmp70E;});}_LL49E: if(
_tmp680 <= (void*)4)goto _LL4A0;if(*((int*)_tmp680)!= 19)goto _LL4A0;_tmp6B1=(void*)((
struct Cyc_Absyn_AccessEff_struct*)_tmp680)->f1;_LL49F: {void*_tmp710=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmp6B1);return _tmp710 == _tmp6B1?t:(void*)({struct Cyc_Absyn_AccessEff_struct*
_tmp711=_cycalloc(sizeof(*_tmp711));_tmp711[0]=({struct Cyc_Absyn_AccessEff_struct
_tmp712;_tmp712.tag=19;_tmp712.f1=(void*)_tmp710;_tmp712;});_tmp711;});}_LL4A0:
if(_tmp680 <= (void*)4)goto _LL46D;if(*((int*)_tmp680)!= 20)goto _LL46D;_tmp6B2=((
struct Cyc_Absyn_JoinEff_struct*)_tmp680)->f1;_LL4A1: {struct Cyc_List_List*
_tmp713=Cyc_Tcutil_substs(rgn,inst,_tmp6B2);return _tmp713 == _tmp6B2?t:(void*)({
struct Cyc_Absyn_JoinEff_struct*_tmp714=_cycalloc(sizeof(*_tmp714));_tmp714[0]=({
struct Cyc_Absyn_JoinEff_struct _tmp715;_tmp715.tag=20;_tmp715.f1=_tmp713;_tmp715;});
_tmp714;});}_LL46D:;}static struct Cyc_List_List*Cyc_Tcutil_substs(struct
_RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*ts){if(ts == 0)
return 0;{void*_tmp716=(void*)ts->hd;struct Cyc_List_List*_tmp717=ts->tl;void*
_tmp718=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp716);struct Cyc_List_List*_tmp719=Cyc_Tcutil_substs(
rgn,inst,_tmp717);if(_tmp716 == _tmp718  && _tmp717 == _tmp719)return ts;return(
struct Cyc_List_List*)((struct Cyc_List_List*)({struct Cyc_List_List*_tmp71A=
_cycalloc(sizeof(*_tmp71A));_tmp71A->hd=(void*)_tmp718;_tmp71A->tl=_tmp719;
_tmp71A;}));}}extern void*Cyc_Tcutil_substitute(struct Cyc_List_List*inst,void*t){
return Cyc_Tcutil_rsubstitute(Cyc_Core_heap_region,inst,t);}struct _tuple8*Cyc_Tcutil_make_inst_var(
struct Cyc_List_List*s,struct Cyc_Absyn_Tvar*tv){struct Cyc_Core_Opt*_tmp71B=Cyc_Tcutil_kind_to_opt(
Cyc_Tcutil_tvar_kind(tv));return({struct _tuple8*_tmp71C=_cycalloc(sizeof(*
_tmp71C));_tmp71C->f1=tv;_tmp71C->f2=Cyc_Absyn_new_evar(_tmp71B,({struct Cyc_Core_Opt*
_tmp71D=_cycalloc(sizeof(*_tmp71D));_tmp71D->v=s;_tmp71D;}));_tmp71C;});}struct
_tuple8*Cyc_Tcutil_r_make_inst_var(struct _tuple9*env,struct Cyc_Absyn_Tvar*tv){
struct _tuple9 _tmp71F;struct Cyc_List_List*_tmp720;struct _RegionHandle*_tmp721;
struct _tuple9*_tmp71E=env;_tmp71F=*_tmp71E;_tmp720=_tmp71F.f1;_tmp721=_tmp71F.f2;{
struct Cyc_Core_Opt*_tmp722=Cyc_Tcutil_kind_to_opt(Cyc_Tcutil_tvar_kind(tv));
return({struct _tuple8*_tmp723=_region_malloc(_tmp721,sizeof(*_tmp723));_tmp723->f1=
tv;_tmp723->f2=Cyc_Absyn_new_evar(_tmp722,({struct Cyc_Core_Opt*_tmp724=_cycalloc(
sizeof(*_tmp724));_tmp724->v=_tmp720;_tmp724;}));_tmp723;});}}static struct Cyc_List_List*
Cyc_Tcutil_add_free_tvar(struct Cyc_Position_Segment*loc,struct Cyc_List_List*tvs,
struct Cyc_Absyn_Tvar*tv){{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
if(Cyc_strptrcmp(((struct Cyc_Absyn_Tvar*)tvs2->hd)->name,tv->name)== 0){void*k1=(
void*)((struct Cyc_Absyn_Tvar*)tvs2->hd)->kind;void*k2=(void*)tv->kind;if(!Cyc_Tcutil_constrain_kinds(
k1,k2))({struct Cyc_String_pa_struct _tmp729;_tmp729.tag=0;_tmp729.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kindbound2string(k2));{struct
Cyc_String_pa_struct _tmp728;_tmp728.tag=0;_tmp728.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_kindbound2string(k1));{struct Cyc_String_pa_struct
_tmp727;_tmp727.tag=0;_tmp727.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*
tv->name);{void*_tmp725[3]={& _tmp727,& _tmp728,& _tmp729};Cyc_Tcutil_terr(loc,({
const char*_tmp726="type variable %s is used with inconsistent kinds %s and %s";
_tag_dyneither(_tmp726,sizeof(char),59);}),_tag_dyneither(_tmp725,sizeof(void*),
3));}}}});if(tv->identity == - 1)tv->identity=((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity;
else{if(tv->identity != ((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity)({void*
_tmp72A=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp72B="same type variable has different identity!";_tag_dyneither(
_tmp72B,sizeof(char),43);}),_tag_dyneither(_tmp72A,sizeof(void*),0));});}return
tvs;}}}tv->identity=Cyc_Tcutil_new_tvar_id();return({struct Cyc_List_List*_tmp72C=
_cycalloc(sizeof(*_tmp72C));_tmp72C->hd=tv;_tmp72C->tl=tvs;_tmp72C;});}static
struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar(struct Cyc_List_List*tvs,struct
Cyc_Absyn_Tvar*tv){if(tv->identity == - 1)({void*_tmp72D=0;((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp72E="fast_add_free_tvar: bad identity in tv";
_tag_dyneither(_tmp72E,sizeof(char),39);}),_tag_dyneither(_tmp72D,sizeof(void*),
0));});{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){struct Cyc_Absyn_Tvar*
_tmp72F=(struct Cyc_Absyn_Tvar*)tvs2->hd;if(_tmp72F->identity == - 1)({void*_tmp730=
0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp731="fast_add_free_tvar: bad identity in tvs2";_tag_dyneither(
_tmp731,sizeof(char),41);}),_tag_dyneither(_tmp730,sizeof(void*),0));});if(
_tmp72F->identity == tv->identity)return tvs;}}return({struct Cyc_List_List*_tmp732=
_cycalloc(sizeof(*_tmp732));_tmp732->hd=tv;_tmp732->tl=tvs;_tmp732;});}struct
_tuple18{struct Cyc_Absyn_Tvar*f1;int f2;};static struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar_bool(
struct _RegionHandle*r,struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv,int b){if(
tv->identity == - 1)({void*_tmp733=0;((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp734="fast_add_free_tvar_bool: bad identity in tv";
_tag_dyneither(_tmp734,sizeof(char),44);}),_tag_dyneither(_tmp733,sizeof(void*),
0));});{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){struct
_tuple18 _tmp736;struct Cyc_Absyn_Tvar*_tmp737;int _tmp738;int*_tmp739;struct
_tuple18*_tmp735=(struct _tuple18*)tvs2->hd;_tmp736=*_tmp735;_tmp737=_tmp736.f1;
_tmp738=_tmp736.f2;_tmp739=(int*)&(*_tmp735).f2;if(_tmp737->identity == - 1)({void*
_tmp73A=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp73B="fast_add_free_tvar_bool: bad identity in tvs2";_tag_dyneither(
_tmp73B,sizeof(char),46);}),_tag_dyneither(_tmp73A,sizeof(void*),0));});if(
_tmp737->identity == tv->identity){*_tmp739=*_tmp739  || b;return tvs;}}}return({
struct Cyc_List_List*_tmp73C=_region_malloc(r,sizeof(*_tmp73C));_tmp73C->hd=({
struct _tuple18*_tmp73D=_region_malloc(r,sizeof(*_tmp73D));_tmp73D->f1=tv;_tmp73D->f2=
b;_tmp73D;});_tmp73C->tl=tvs;_tmp73C;});}static struct Cyc_List_List*Cyc_Tcutil_add_bound_tvar(
struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){if(tv->identity == - 1)({struct
Cyc_String_pa_struct _tmp740;_tmp740.tag=0;_tmp740.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Tcutil_tvar2string(tv));{void*_tmp73E[1]={& _tmp740};((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const
char*_tmp73F="bound tvar id for %s is NULL";_tag_dyneither(_tmp73F,sizeof(char),
29);}),_tag_dyneither(_tmp73E,sizeof(void*),1));}});return({struct Cyc_List_List*
_tmp741=_cycalloc(sizeof(*_tmp741));_tmp741->hd=tv;_tmp741->tl=tvs;_tmp741;});}
struct _tuple19{void*f1;int f2;};static struct Cyc_List_List*Cyc_Tcutil_add_free_evar(
struct _RegionHandle*r,struct Cyc_List_List*es,void*e,int b){void*_tmp742=Cyc_Tcutil_compress(
e);int _tmp743;_LL4AD: if(_tmp742 <= (void*)4)goto _LL4AF;if(*((int*)_tmp742)!= 0)
goto _LL4AF;_tmp743=((struct Cyc_Absyn_Evar_struct*)_tmp742)->f3;_LL4AE:{struct Cyc_List_List*
es2=es;for(0;es2 != 0;es2=es2->tl){struct _tuple19 _tmp745;void*_tmp746;int _tmp747;
int*_tmp748;struct _tuple19*_tmp744=(struct _tuple19*)es2->hd;_tmp745=*_tmp744;
_tmp746=_tmp745.f1;_tmp747=_tmp745.f2;_tmp748=(int*)&(*_tmp744).f2;{void*_tmp749=
Cyc_Tcutil_compress(_tmp746);int _tmp74A;_LL4B2: if(_tmp749 <= (void*)4)goto _LL4B4;
if(*((int*)_tmp749)!= 0)goto _LL4B4;_tmp74A=((struct Cyc_Absyn_Evar_struct*)
_tmp749)->f3;_LL4B3: if(_tmp743 == _tmp74A){if(b != *_tmp748)*_tmp748=1;return es;}
goto _LL4B1;_LL4B4:;_LL4B5: goto _LL4B1;_LL4B1:;}}}return({struct Cyc_List_List*
_tmp74B=_region_malloc(r,sizeof(*_tmp74B));_tmp74B->hd=({struct _tuple19*_tmp74C=
_region_malloc(r,sizeof(*_tmp74C));_tmp74C->f1=e;_tmp74C->f2=b;_tmp74C;});
_tmp74B->tl=es;_tmp74B;});_LL4AF:;_LL4B0: return es;_LL4AC:;}static struct Cyc_List_List*
Cyc_Tcutil_remove_bound_tvars(struct _RegionHandle*rgn,struct Cyc_List_List*tvs,
struct Cyc_List_List*btvs){struct Cyc_List_List*r=0;for(0;tvs != 0;tvs=tvs->tl){int
present=0;{struct Cyc_List_List*b=btvs;for(0;b != 0;b=b->tl){if(((struct Cyc_Absyn_Tvar*)
tvs->hd)->identity == ((struct Cyc_Absyn_Tvar*)b->hd)->identity){present=1;break;}}}
if(!present)r=({struct Cyc_List_List*_tmp74D=_region_malloc(rgn,sizeof(*_tmp74D));
_tmp74D->hd=(struct Cyc_Absyn_Tvar*)tvs->hd;_tmp74D->tl=r;_tmp74D;});}r=((struct
Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(r);return r;}static
struct Cyc_List_List*Cyc_Tcutil_remove_bound_tvars_bool(struct _RegionHandle*r,
struct Cyc_List_List*tvs,struct Cyc_List_List*btvs){struct Cyc_List_List*res=0;for(
0;tvs != 0;tvs=tvs->tl){struct Cyc_Absyn_Tvar*_tmp74F;int _tmp750;struct _tuple18
_tmp74E=*((struct _tuple18*)tvs->hd);_tmp74F=_tmp74E.f1;_tmp750=_tmp74E.f2;{int
present=0;{struct Cyc_List_List*b=btvs;for(0;b != 0;b=b->tl){if(_tmp74F->identity
== ((struct Cyc_Absyn_Tvar*)b->hd)->identity){present=1;break;}}}if(!present)res=({
struct Cyc_List_List*_tmp751=_region_malloc(r,sizeof(*_tmp751));_tmp751->hd=(
struct _tuple18*)tvs->hd;_tmp751->tl=res;_tmp751;});}}res=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(res);return res;}void Cyc_Tcutil_check_bitfield(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,void*field_typ,struct Cyc_Absyn_Exp*
width,struct _dyneither_ptr*fn){if(width != 0){unsigned int w=0;if(!Cyc_Tcutil_is_const_exp(
te,(struct Cyc_Absyn_Exp*)width))({struct Cyc_String_pa_struct _tmp754;_tmp754.tag=
0;_tmp754.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*fn);{void*_tmp752[1]={&
_tmp754};Cyc_Tcutil_terr(loc,({const char*_tmp753="bitfield %s does not have constant width";
_tag_dyneither(_tmp753,sizeof(char),41);}),_tag_dyneither(_tmp752,sizeof(void*),
1));}});else{unsigned int _tmp756;int _tmp757;struct _tuple7 _tmp755=Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)width);_tmp756=_tmp755.f1;_tmp757=_tmp755.f2;if(!_tmp757)({
void*_tmp758=0;Cyc_Tcutil_terr(loc,({const char*_tmp759="bitfield width cannot use sizeof or offsetof";
_tag_dyneither(_tmp759,sizeof(char),45);}),_tag_dyneither(_tmp758,sizeof(void*),
0));});w=_tmp756;}{void*_tmp75A=Cyc_Tcutil_compress(field_typ);void*_tmp75B;
_LL4B7: if(_tmp75A <= (void*)4)goto _LL4B9;if(*((int*)_tmp75A)!= 5)goto _LL4B9;
_tmp75B=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp75A)->f2;_LL4B8:{void*
_tmp75C=_tmp75B;_LL4BC: if((int)_tmp75C != 0)goto _LL4BE;_LL4BD: if(w > 8)({void*
_tmp75D=0;Cyc_Tcutil_terr(loc,({const char*_tmp75E="bitfield larger than type";
_tag_dyneither(_tmp75E,sizeof(char),26);}),_tag_dyneither(_tmp75D,sizeof(void*),
0));});goto _LL4BB;_LL4BE: if((int)_tmp75C != 1)goto _LL4C0;_LL4BF: if(w > 16)({void*
_tmp75F=0;Cyc_Tcutil_terr(loc,({const char*_tmp760="bitfield larger than type";
_tag_dyneither(_tmp760,sizeof(char),26);}),_tag_dyneither(_tmp75F,sizeof(void*),
0));});goto _LL4BB;_LL4C0: if((int)_tmp75C != 3)goto _LL4C2;_LL4C1: goto _LL4C3;_LL4C2:
if((int)_tmp75C != 2)goto _LL4C4;_LL4C3: if(w > 32)({void*_tmp761=0;Cyc_Tcutil_terr(
loc,({const char*_tmp762="bitfield larger than type";_tag_dyneither(_tmp762,
sizeof(char),26);}),_tag_dyneither(_tmp761,sizeof(void*),0));});goto _LL4BB;
_LL4C4: if((int)_tmp75C != 4)goto _LL4BB;_LL4C5: if(w > 64)({void*_tmp763=0;Cyc_Tcutil_terr(
loc,({const char*_tmp764="bitfield larger than type";_tag_dyneither(_tmp764,
sizeof(char),26);}),_tag_dyneither(_tmp763,sizeof(void*),0));});goto _LL4BB;
_LL4BB:;}goto _LL4B6;_LL4B9:;_LL4BA:({struct Cyc_String_pa_struct _tmp768;_tmp768.tag=
0;_tmp768.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
field_typ));{struct Cyc_String_pa_struct _tmp767;_tmp767.tag=0;_tmp767.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*fn);{void*_tmp765[2]={& _tmp767,& _tmp768};
Cyc_Tcutil_terr(loc,({const char*_tmp766="bitfield %s must have integral type but has type %s";
_tag_dyneither(_tmp766,sizeof(char),52);}),_tag_dyneither(_tmp765,sizeof(void*),
2));}}});goto _LL4B6;_LL4B6:;}}}static void Cyc_Tcutil_check_field_atts(struct Cyc_Position_Segment*
loc,struct _dyneither_ptr*fn,struct Cyc_List_List*atts){for(0;atts != 0;atts=atts->tl){
void*_tmp769=(void*)atts->hd;_LL4C7: if((int)_tmp769 != 5)goto _LL4C9;_LL4C8:
continue;_LL4C9: if(_tmp769 <= (void*)17)goto _LL4CB;if(*((int*)_tmp769)!= 1)goto
_LL4CB;_LL4CA: continue;_LL4CB:;_LL4CC:({struct Cyc_String_pa_struct _tmp76D;
_tmp76D.tag=0;_tmp76D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*fn);{
struct Cyc_String_pa_struct _tmp76C;_tmp76C.tag=0;_tmp76C.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absyn_attribute2string((void*)atts->hd));{void*_tmp76A[
2]={& _tmp76C,& _tmp76D};Cyc_Tcutil_terr(loc,({const char*_tmp76B="bad attribute %s on member %s";
_tag_dyneither(_tmp76B,sizeof(char),30);}),_tag_dyneither(_tmp76A,sizeof(void*),
2));}}});_LL4C6:;}}struct Cyc_Tcutil_CVTEnv{struct _RegionHandle*r;struct Cyc_List_List*
kind_env;struct Cyc_List_List*free_vars;struct Cyc_List_List*free_evars;int
generalize_evars;int fn_result;};int Cyc_Tcutil_extract_const_from_typedef(struct
Cyc_Position_Segment*loc,int declared_const,void*t){void*_tmp76E=t;struct Cyc_Absyn_Typedefdecl*
_tmp76F;void**_tmp770;_LL4CE: if(_tmp76E <= (void*)4)goto _LL4D0;if(*((int*)_tmp76E)
!= 16)goto _LL4D0;_tmp76F=((struct Cyc_Absyn_TypedefType_struct*)_tmp76E)->f3;
_tmp770=((struct Cyc_Absyn_TypedefType_struct*)_tmp76E)->f4;_LL4CF: if((((struct
Cyc_Absyn_Typedefdecl*)_check_null(_tmp76F))->tq).real_const  || (_tmp76F->tq).print_const){
if(declared_const)({void*_tmp771=0;Cyc_Tcutil_warn(loc,({const char*_tmp772="extra const";
_tag_dyneither(_tmp772,sizeof(char),12);}),_tag_dyneither(_tmp771,sizeof(void*),
0));});return 1;}if((unsigned int)_tmp770)return Cyc_Tcutil_extract_const_from_typedef(
loc,declared_const,*_tmp770);else{return declared_const;}_LL4D0:;_LL4D1: return
declared_const;_LL4CD:;}static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type_level_exp(
struct Cyc_Absyn_Exp*e,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv);
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type(struct Cyc_Position_Segment*
loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv,void*expected_kind,
void*t,int put_in_effect){static struct Cyc_Core_Opt urgn={(void*)((void*)3)};static
struct Cyc_Core_Opt hrgn={(void*)((void*)2)};{void*_tmp773=Cyc_Tcutil_compress(t);
struct Cyc_Core_Opt*_tmp774;struct Cyc_Core_Opt**_tmp775;struct Cyc_Core_Opt*
_tmp776;struct Cyc_Core_Opt**_tmp777;struct Cyc_Absyn_Tvar*_tmp778;struct Cyc_List_List*
_tmp779;struct _tuple2*_tmp77A;struct Cyc_Absyn_Enumdecl*_tmp77B;struct Cyc_Absyn_Enumdecl**
_tmp77C;struct Cyc_Absyn_TunionInfo _tmp77D;union Cyc_Absyn_TunionInfoU_union
_tmp77E;union Cyc_Absyn_TunionInfoU_union*_tmp77F;struct Cyc_List_List*_tmp780;
struct Cyc_List_List**_tmp781;struct Cyc_Core_Opt*_tmp782;struct Cyc_Core_Opt**
_tmp783;struct Cyc_Absyn_TunionFieldInfo _tmp784;union Cyc_Absyn_TunionFieldInfoU_union
_tmp785;union Cyc_Absyn_TunionFieldInfoU_union*_tmp786;struct Cyc_List_List*
_tmp787;struct Cyc_Absyn_PtrInfo _tmp788;void*_tmp789;struct Cyc_Absyn_Tqual _tmp78A;
struct Cyc_Absyn_Tqual*_tmp78B;struct Cyc_Absyn_PtrAtts _tmp78C;void*_tmp78D;struct
Cyc_Absyn_Conref*_tmp78E;struct Cyc_Absyn_Conref*_tmp78F;struct Cyc_Absyn_Conref*
_tmp790;void*_tmp791;struct Cyc_Absyn_Exp*_tmp792;struct Cyc_Absyn_ArrayInfo
_tmp793;void*_tmp794;struct Cyc_Absyn_Tqual _tmp795;struct Cyc_Absyn_Tqual*_tmp796;
struct Cyc_Absyn_Exp*_tmp797;struct Cyc_Absyn_Exp**_tmp798;struct Cyc_Absyn_Conref*
_tmp799;struct Cyc_Position_Segment*_tmp79A;struct Cyc_Absyn_FnInfo _tmp79B;struct
Cyc_List_List*_tmp79C;struct Cyc_List_List**_tmp79D;struct Cyc_Core_Opt*_tmp79E;
struct Cyc_Core_Opt**_tmp79F;void*_tmp7A0;struct Cyc_List_List*_tmp7A1;int _tmp7A2;
struct Cyc_Absyn_VarargInfo*_tmp7A3;struct Cyc_List_List*_tmp7A4;struct Cyc_List_List*
_tmp7A5;struct Cyc_List_List*_tmp7A6;void*_tmp7A7;struct Cyc_List_List*_tmp7A8;
struct Cyc_Absyn_AggrInfo _tmp7A9;union Cyc_Absyn_AggrInfoU_union _tmp7AA;union Cyc_Absyn_AggrInfoU_union*
_tmp7AB;struct Cyc_List_List*_tmp7AC;struct Cyc_List_List**_tmp7AD;struct _tuple2*
_tmp7AE;struct Cyc_List_List*_tmp7AF;struct Cyc_List_List**_tmp7B0;struct Cyc_Absyn_Typedefdecl*
_tmp7B1;struct Cyc_Absyn_Typedefdecl**_tmp7B2;void**_tmp7B3;void***_tmp7B4;void*
_tmp7B5;void*_tmp7B6;void*_tmp7B7;void*_tmp7B8;void*_tmp7B9;struct Cyc_List_List*
_tmp7BA;_LL4D3: if((int)_tmp773 != 0)goto _LL4D5;_LL4D4: goto _LL4D2;_LL4D5: if(
_tmp773 <= (void*)4)goto _LL4E9;if(*((int*)_tmp773)!= 0)goto _LL4D7;_tmp774=((
struct Cyc_Absyn_Evar_struct*)_tmp773)->f1;_tmp775=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Evar_struct*)_tmp773)->f1;_tmp776=((struct Cyc_Absyn_Evar_struct*)
_tmp773)->f2;_tmp777=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)
_tmp773)->f2;_LL4D6: if(*_tmp775 == 0)*_tmp775=Cyc_Tcutil_kind_to_opt(
expected_kind);if((cvtenv.fn_result  && cvtenv.generalize_evars) && Cyc_Tcutil_is_region_kind(
expected_kind)){if(expected_kind == (void*)4)*_tmp777=(struct Cyc_Core_Opt*)& urgn;
else{*_tmp777=(struct Cyc_Core_Opt*)& hrgn;}}else{if(cvtenv.generalize_evars){
struct Cyc_Absyn_Tvar*_tmp7BB=Cyc_Tcutil_new_tvar((void*)({struct Cyc_Absyn_Less_kb_struct*
_tmp7BF=_cycalloc(sizeof(*_tmp7BF));_tmp7BF[0]=({struct Cyc_Absyn_Less_kb_struct
_tmp7C0;_tmp7C0.tag=2;_tmp7C0.f1=0;_tmp7C0.f2=(void*)expected_kind;_tmp7C0;});
_tmp7BF;}));*_tmp777=({struct Cyc_Core_Opt*_tmp7BC=_cycalloc(sizeof(*_tmp7BC));
_tmp7BC->v=(void*)((void*)({struct Cyc_Absyn_VarType_struct*_tmp7BD=_cycalloc(
sizeof(*_tmp7BD));_tmp7BD[0]=({struct Cyc_Absyn_VarType_struct _tmp7BE;_tmp7BE.tag=
1;_tmp7BE.f1=_tmp7BB;_tmp7BE;});_tmp7BD;}));_tmp7BC;});_tmp778=_tmp7BB;goto
_LL4D8;}else{cvtenv.free_evars=Cyc_Tcutil_add_free_evar(cvtenv.r,cvtenv.free_evars,
t,put_in_effect);}}goto _LL4D2;_LL4D7: if(*((int*)_tmp773)!= 1)goto _LL4D9;_tmp778=((
struct Cyc_Absyn_VarType_struct*)_tmp773)->f1;_LL4D8:{void*_tmp7C1=Cyc_Absyn_compress_kb((
void*)_tmp778->kind);struct Cyc_Core_Opt*_tmp7C2;struct Cyc_Core_Opt**_tmp7C3;
_LL508: if(*((int*)_tmp7C1)!= 1)goto _LL50A;_tmp7C2=((struct Cyc_Absyn_Unknown_kb_struct*)
_tmp7C1)->f1;_tmp7C3=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)
_tmp7C1)->f1;_LL509:*_tmp7C3=({struct Cyc_Core_Opt*_tmp7C4=_cycalloc(sizeof(*
_tmp7C4));_tmp7C4->v=(void*)((void*)({struct Cyc_Absyn_Less_kb_struct*_tmp7C5=
_cycalloc(sizeof(*_tmp7C5));_tmp7C5[0]=({struct Cyc_Absyn_Less_kb_struct _tmp7C6;
_tmp7C6.tag=2;_tmp7C6.f1=0;_tmp7C6.f2=(void*)expected_kind;_tmp7C6;});_tmp7C5;}));
_tmp7C4;});goto _LL507;_LL50A:;_LL50B: goto _LL507;_LL507:;}cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(
loc,cvtenv.kind_env,_tmp778);cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(
cvtenv.r,cvtenv.free_vars,_tmp778,put_in_effect);{void*_tmp7C7=Cyc_Absyn_compress_kb((
void*)_tmp778->kind);struct Cyc_Core_Opt*_tmp7C8;struct Cyc_Core_Opt**_tmp7C9;void*
_tmp7CA;_LL50D: if(*((int*)_tmp7C7)!= 2)goto _LL50F;_tmp7C8=((struct Cyc_Absyn_Less_kb_struct*)
_tmp7C7)->f1;_tmp7C9=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp7C7)->f1;_tmp7CA=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp7C7)->f2;
_LL50E: if(Cyc_Tcutil_kind_leq(expected_kind,_tmp7CA))*_tmp7C9=({struct Cyc_Core_Opt*
_tmp7CB=_cycalloc(sizeof(*_tmp7CB));_tmp7CB->v=(void*)((void*)({struct Cyc_Absyn_Less_kb_struct*
_tmp7CC=_cycalloc(sizeof(*_tmp7CC));_tmp7CC[0]=({struct Cyc_Absyn_Less_kb_struct
_tmp7CD;_tmp7CD.tag=2;_tmp7CD.f1=0;_tmp7CD.f2=(void*)expected_kind;_tmp7CD;});
_tmp7CC;}));_tmp7CB;});goto _LL50C;_LL50F:;_LL510: goto _LL50C;_LL50C:;}goto _LL4D2;
_LL4D9: if(*((int*)_tmp773)!= 13)goto _LL4DB;_tmp779=((struct Cyc_Absyn_AnonEnumType_struct*)
_tmp773)->f1;_LL4DA: {struct Cyc_Tcenv_Genv*ge=((struct Cyc_Tcenv_Genv*(*)(struct
Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);struct
_RegionHandle*_tmp7CE=Cyc_Tcenv_get_fnrgn(te);{struct Cyc_List_List*prev_fields=0;
unsigned int tag_count=0;for(0;_tmp779 != 0;_tmp779=_tmp779->tl){struct Cyc_Absyn_Enumfield*
_tmp7CF=(struct Cyc_Absyn_Enumfield*)_tmp779->hd;if(((int(*)(int(*compare)(struct
_dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct
_dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,(*_tmp7CF->name).f2))({
struct Cyc_String_pa_struct _tmp7D2;_tmp7D2.tag=0;_tmp7D2.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)*(*_tmp7CF->name).f2);{void*_tmp7D0[1]={& _tmp7D2};Cyc_Tcutil_terr(
_tmp7CF->loc,({const char*_tmp7D1="duplicate enum field name %s";_tag_dyneither(
_tmp7D1,sizeof(char),29);}),_tag_dyneither(_tmp7D0,sizeof(void*),1));}});else{
prev_fields=({struct Cyc_List_List*_tmp7D3=_region_malloc(_tmp7CE,sizeof(*_tmp7D3));
_tmp7D3->hd=(*_tmp7CF->name).f2;_tmp7D3->tl=prev_fields;_tmp7D3;});}if(_tmp7CF->tag
== 0)_tmp7CF->tag=(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(tag_count,_tmp7CF->loc);
else{if(!Cyc_Tcutil_is_const_exp(te,(struct Cyc_Absyn_Exp*)_check_null(_tmp7CF->tag)))({
struct Cyc_String_pa_struct _tmp7D6;_tmp7D6.tag=0;_tmp7D6.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)*(*_tmp7CF->name).f2);{void*_tmp7D4[1]={& _tmp7D6};Cyc_Tcutil_terr(
loc,({const char*_tmp7D5="enum field %s: expression is not constant";
_tag_dyneither(_tmp7D5,sizeof(char),42);}),_tag_dyneither(_tmp7D4,sizeof(void*),
1));}});}{unsigned int t1=(Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)
_check_null(_tmp7CF->tag))).f1;tag_count=t1 + 1;(*_tmp7CF->name).f1=(union Cyc_Absyn_Nmspace_union)({
union Cyc_Absyn_Nmspace_union _tmp7D7;(_tmp7D7.Abs_n).tag=2;(_tmp7D7.Abs_n).f1=te->ns;
_tmp7D7;});ge->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct
_dyneither_ptr*k,struct _tuple19*v))Cyc_Dict_insert)(ge->ordinaries,(*_tmp7CF->name).f2,(
struct _tuple19*)({struct _tuple19*_tmp7D8=_cycalloc(sizeof(*_tmp7D8));_tmp7D8->f1=(
void*)({struct Cyc_Tcenv_AnonEnumRes_struct*_tmp7D9=_cycalloc(sizeof(*_tmp7D9));
_tmp7D9[0]=({struct Cyc_Tcenv_AnonEnumRes_struct _tmp7DA;_tmp7DA.tag=4;_tmp7DA.f1=(
void*)t;_tmp7DA.f2=_tmp7CF;_tmp7DA;});_tmp7D9;});_tmp7D8->f2=1;_tmp7D8;}));}}}
goto _LL4D2;}_LL4DB: if(*((int*)_tmp773)!= 12)goto _LL4DD;_tmp77A=((struct Cyc_Absyn_EnumType_struct*)
_tmp773)->f1;_tmp77B=((struct Cyc_Absyn_EnumType_struct*)_tmp773)->f2;_tmp77C=(
struct Cyc_Absyn_Enumdecl**)&((struct Cyc_Absyn_EnumType_struct*)_tmp773)->f2;
_LL4DC: if(*_tmp77C == 0  || ((struct Cyc_Absyn_Enumdecl*)_check_null(*_tmp77C))->fields
== 0){struct _handler_cons _tmp7DB;_push_handler(& _tmp7DB);{int _tmp7DD=0;if(setjmp(
_tmp7DB.handler))_tmp7DD=1;if(!_tmp7DD){{struct Cyc_Absyn_Enumdecl**ed=Cyc_Tcenv_lookup_enumdecl(
te,loc,_tmp77A);*_tmp77C=(struct Cyc_Absyn_Enumdecl*)*ed;};_pop_handler();}else{
void*_tmp7DC=(void*)_exn_thrown;void*_tmp7DF=_tmp7DC;_LL512: if(_tmp7DF != Cyc_Dict_Absent)
goto _LL514;_LL513: {struct Cyc_Tcenv_Genv*_tmp7E0=((struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct Cyc_Absyn_Enumdecl*_tmp7E1=({struct Cyc_Absyn_Enumdecl*_tmp7E2=_cycalloc(
sizeof(*_tmp7E2));_tmp7E2->sc=(void*)((void*)3);_tmp7E2->name=_tmp77A;_tmp7E2->fields=
0;_tmp7E2;});Cyc_Tc_tcEnumdecl(te,_tmp7E0,loc,_tmp7E1);{struct Cyc_Absyn_Enumdecl**
ed=Cyc_Tcenv_lookup_enumdecl(te,loc,_tmp77A);*_tmp77C=(struct Cyc_Absyn_Enumdecl*)*
ed;goto _LL511;}}_LL514:;_LL515:(void)_throw(_tmp7DF);_LL511:;}}}{struct Cyc_Absyn_Enumdecl*
ed=(struct Cyc_Absyn_Enumdecl*)_check_null(*_tmp77C);*_tmp77A=(ed->name)[
_check_known_subscript_notnull(1,0)];goto _LL4D2;}_LL4DD: if(*((int*)_tmp773)!= 2)
goto _LL4DF;_tmp77D=((struct Cyc_Absyn_TunionType_struct*)_tmp773)->f1;_tmp77E=
_tmp77D.tunion_info;_tmp77F=(union Cyc_Absyn_TunionInfoU_union*)&(((struct Cyc_Absyn_TunionType_struct*)
_tmp773)->f1).tunion_info;_tmp780=_tmp77D.targs;_tmp781=(struct Cyc_List_List**)&(((
struct Cyc_Absyn_TunionType_struct*)_tmp773)->f1).targs;_tmp782=_tmp77D.rgn;
_tmp783=(struct Cyc_Core_Opt**)&(((struct Cyc_Absyn_TunionType_struct*)_tmp773)->f1).rgn;
_LL4DE: {struct Cyc_List_List*_tmp7E3=*_tmp781;{union Cyc_Absyn_TunionInfoU_union
_tmp7E4=*_tmp77F;struct Cyc_Absyn_UnknownTunionInfo _tmp7E5;struct _tuple2*_tmp7E6;
int _tmp7E7;int _tmp7E8;struct Cyc_Absyn_Tuniondecl**_tmp7E9;struct Cyc_Absyn_Tuniondecl*
_tmp7EA;_LL517: if((_tmp7E4.UnknownTunion).tag != 0)goto _LL519;_tmp7E5=(_tmp7E4.UnknownTunion).f1;
_tmp7E6=_tmp7E5.name;_tmp7E7=_tmp7E5.is_xtunion;_tmp7E8=_tmp7E5.is_flat;_LL518: {
struct Cyc_Absyn_Tuniondecl**tudp;{struct _handler_cons _tmp7EB;_push_handler(&
_tmp7EB);{int _tmp7ED=0;if(setjmp(_tmp7EB.handler))_tmp7ED=1;if(!_tmp7ED){tudp=
Cyc_Tcenv_lookup_tuniondecl(te,loc,_tmp7E6);;_pop_handler();}else{void*_tmp7EC=(
void*)_exn_thrown;void*_tmp7EF=_tmp7EC;_LL51C: if(_tmp7EF != Cyc_Dict_Absent)goto
_LL51E;_LL51D: {struct Cyc_Tcenv_Genv*_tmp7F0=((struct Cyc_Tcenv_Genv*(*)(struct
Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);struct Cyc_Absyn_Tuniondecl*
_tmp7F1=({struct Cyc_Absyn_Tuniondecl*_tmp7F8=_cycalloc(sizeof(*_tmp7F8));_tmp7F8->sc=(
void*)((void*)3);_tmp7F8->name=_tmp7E6;_tmp7F8->tvs=0;_tmp7F8->fields=0;_tmp7F8->is_xtunion=
_tmp7E7;_tmp7F8->is_flat=_tmp7E8;_tmp7F8;});Cyc_Tc_tcTuniondecl(te,_tmp7F0,loc,
_tmp7F1);tudp=Cyc_Tcenv_lookup_tuniondecl(te,loc,_tmp7E6);if(_tmp7E3 != 0){({
struct Cyc_String_pa_struct _tmp7F5;_tmp7F5.tag=0;_tmp7F5.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp7E6));{struct Cyc_String_pa_struct
_tmp7F4;_tmp7F4.tag=0;_tmp7F4.f1=(struct _dyneither_ptr)(_tmp7E7?(struct
_dyneither_ptr)((struct _dyneither_ptr)({const char*_tmp7F6="xtunion";
_tag_dyneither(_tmp7F6,sizeof(char),8);})):(struct _dyneither_ptr)({const char*
_tmp7F7="tunion";_tag_dyneither(_tmp7F7,sizeof(char),7);}));{void*_tmp7F2[2]={&
_tmp7F4,& _tmp7F5};Cyc_Tcutil_terr(loc,({const char*_tmp7F3="declare parameterized %s %s before using";
_tag_dyneither(_tmp7F3,sizeof(char),41);}),_tag_dyneither(_tmp7F2,sizeof(void*),
2));}}});return cvtenv;}goto _LL51B;}_LL51E:;_LL51F:(void)_throw(_tmp7EF);_LL51B:;}}}
if((*tudp)->is_xtunion != _tmp7E7)({struct Cyc_String_pa_struct _tmp7FB;_tmp7FB.tag=
0;_tmp7FB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp7E6));{void*_tmp7F9[1]={& _tmp7FB};Cyc_Tcutil_terr(loc,({const char*_tmp7FA="[x]tunion is different from type declaration %s";
_tag_dyneither(_tmp7FA,sizeof(char),48);}),_tag_dyneither(_tmp7F9,sizeof(void*),
1));}});*_tmp77F=(union Cyc_Absyn_TunionInfoU_union)({union Cyc_Absyn_TunionInfoU_union
_tmp7FC;(_tmp7FC.KnownTunion).tag=1;(_tmp7FC.KnownTunion).f1=tudp;_tmp7FC;});
_tmp7EA=*tudp;goto _LL51A;}_LL519: if((_tmp7E4.KnownTunion).tag != 1)goto _LL516;
_tmp7E9=(_tmp7E4.KnownTunion).f1;_tmp7EA=*_tmp7E9;_LL51A: if(_tmp7EA->is_flat  && (
unsigned int)*_tmp783)({struct Cyc_String_pa_struct _tmp7FF;_tmp7FF.tag=0;_tmp7FF.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)((
struct Cyc_Core_Opt*)_check_null(*_tmp783))->v));{void*_tmp7FD[1]={& _tmp7FF};Cyc_Tcutil_terr(
loc,({const char*_tmp7FE="flat tunion has region %s";_tag_dyneither(_tmp7FE,
sizeof(char),26);}),_tag_dyneither(_tmp7FD,sizeof(void*),1));}});if(!_tmp7EA->is_flat
 && !((unsigned int)*_tmp783))*_tmp783=({struct Cyc_Core_Opt*_tmp800=_cycalloc(
sizeof(*_tmp800));_tmp800->v=(void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,
0);_tmp800;});if((unsigned int)*_tmp783){void*_tmp801=(void*)((struct Cyc_Core_Opt*)
_check_null(*_tmp783))->v;cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(
void*)3,_tmp801,1);}{struct Cyc_List_List*tvs=_tmp7EA->tvs;for(0;_tmp7E3 != 0  && 
tvs != 0;(_tmp7E3=_tmp7E3->tl,tvs=tvs->tl)){void*t1=(void*)_tmp7E3->hd;void*k1=
Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd);cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,k1,t1,1);}if(_tmp7E3 != 0)({struct Cyc_String_pa_struct _tmp804;
_tmp804.tag=0;_tmp804.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp7EA->name));{void*_tmp802[1]={& _tmp804};Cyc_Tcutil_terr(loc,({const char*
_tmp803="too many type arguments for tunion %s";_tag_dyneither(_tmp803,sizeof(
char),38);}),_tag_dyneither(_tmp802,sizeof(void*),1));}});if(tvs != 0){struct Cyc_List_List*
hidden_ts=0;for(0;tvs != 0;tvs=tvs->tl){void*k1=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)
tvs->hd);void*e=Cyc_Absyn_new_evar(0,0);hidden_ts=({struct Cyc_List_List*_tmp805=
_cycalloc(sizeof(*_tmp805));_tmp805->hd=(void*)e;_tmp805->tl=hidden_ts;_tmp805;});
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k1,e,1);}*_tmp781=Cyc_List_imp_append(*
_tmp781,Cyc_List_imp_rev(hidden_ts));}goto _LL516;}_LL516:;}goto _LL4D2;}_LL4DF:
if(*((int*)_tmp773)!= 3)goto _LL4E1;_tmp784=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp773)->f1;_tmp785=_tmp784.field_info;_tmp786=(union Cyc_Absyn_TunionFieldInfoU_union*)&(((
struct Cyc_Absyn_TunionFieldType_struct*)_tmp773)->f1).field_info;_tmp787=_tmp784.targs;
_LL4E0:{union Cyc_Absyn_TunionFieldInfoU_union _tmp806=*_tmp786;struct Cyc_Absyn_UnknownTunionFieldInfo
_tmp807;struct _tuple2*_tmp808;struct _tuple2*_tmp809;int _tmp80A;struct Cyc_Absyn_Tuniondecl*
_tmp80B;struct Cyc_Absyn_Tunionfield*_tmp80C;_LL521: if((_tmp806.UnknownTunionfield).tag
!= 0)goto _LL523;_tmp807=(_tmp806.UnknownTunionfield).f1;_tmp808=_tmp807.tunion_name;
_tmp809=_tmp807.field_name;_tmp80A=_tmp807.is_xtunion;_LL522: {struct Cyc_Absyn_Tuniondecl*
tud;struct Cyc_Absyn_Tunionfield*tuf;{struct _handler_cons _tmp80D;_push_handler(&
_tmp80D);{int _tmp80F=0;if(setjmp(_tmp80D.handler))_tmp80F=1;if(!_tmp80F){*Cyc_Tcenv_lookup_tuniondecl(
te,loc,_tmp808);;_pop_handler();}else{void*_tmp80E=(void*)_exn_thrown;void*
_tmp811=_tmp80E;_LL526: if(_tmp811 != Cyc_Dict_Absent)goto _LL528;_LL527:({struct
Cyc_String_pa_struct _tmp814;_tmp814.tag=0;_tmp814.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp808));{void*_tmp812[1]={&
_tmp814};Cyc_Tcutil_terr(loc,({const char*_tmp813="unbound type tunion %s";
_tag_dyneither(_tmp813,sizeof(char),23);}),_tag_dyneither(_tmp812,sizeof(void*),
1));}});return cvtenv;_LL528:;_LL529:(void)_throw(_tmp811);_LL525:;}}}{struct
_handler_cons _tmp815;_push_handler(& _tmp815);{int _tmp817=0;if(setjmp(_tmp815.handler))
_tmp817=1;if(!_tmp817){{struct _RegionHandle*_tmp818=Cyc_Tcenv_get_fnrgn(te);void*
_tmp819=Cyc_Tcenv_lookup_ordinary(_tmp818,te,loc,_tmp809);struct Cyc_Absyn_Tuniondecl*
_tmp81A;struct Cyc_Absyn_Tunionfield*_tmp81B;_LL52B: if(*((int*)_tmp819)!= 2)goto
_LL52D;_tmp81A=((struct Cyc_Tcenv_TunionRes_struct*)_tmp819)->f1;_tmp81B=((struct
Cyc_Tcenv_TunionRes_struct*)_tmp819)->f2;_LL52C: tuf=_tmp81B;tud=_tmp81A;if(tud->is_xtunion
!= _tmp80A)({struct Cyc_String_pa_struct _tmp81E;_tmp81E.tag=0;_tmp81E.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp808));{void*
_tmp81C[1]={& _tmp81E};Cyc_Tcutil_terr(loc,({const char*_tmp81D="[x]tunion is different from type declaration %s";
_tag_dyneither(_tmp81D,sizeof(char),48);}),_tag_dyneither(_tmp81C,sizeof(void*),
1));}});goto _LL52A;_LL52D:;_LL52E:({struct Cyc_String_pa_struct _tmp822;_tmp822.tag=
0;_tmp822.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp808));{struct Cyc_String_pa_struct _tmp821;_tmp821.tag=0;_tmp821.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp809));{void*
_tmp81F[2]={& _tmp821,& _tmp822};Cyc_Tcutil_terr(loc,({const char*_tmp820="unbound field %s in type tunion %s";
_tag_dyneither(_tmp820,sizeof(char),35);}),_tag_dyneither(_tmp81F,sizeof(void*),
2));}}});{struct Cyc_Tcutil_CVTEnv _tmp823=cvtenv;_npop_handler(0);return _tmp823;}
_LL52A:;};_pop_handler();}else{void*_tmp816=(void*)_exn_thrown;void*_tmp825=
_tmp816;_LL530: if(_tmp825 != Cyc_Dict_Absent)goto _LL532;_LL531:({struct Cyc_String_pa_struct
_tmp829;_tmp829.tag=0;_tmp829.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp808));{struct Cyc_String_pa_struct _tmp828;_tmp828.tag=
0;_tmp828.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp809));{void*_tmp826[2]={& _tmp828,& _tmp829};Cyc_Tcutil_terr(loc,({const char*
_tmp827="unbound field %s in type tunion %s";_tag_dyneither(_tmp827,sizeof(char),
35);}),_tag_dyneither(_tmp826,sizeof(void*),2));}}});return cvtenv;_LL532:;_LL533:(
void)_throw(_tmp825);_LL52F:;}}}*_tmp786=(union Cyc_Absyn_TunionFieldInfoU_union)({
union Cyc_Absyn_TunionFieldInfoU_union _tmp82A;(_tmp82A.KnownTunionfield).tag=1;(
_tmp82A.KnownTunionfield).f1=tud;(_tmp82A.KnownTunionfield).f2=tuf;_tmp82A;});
_tmp80B=tud;_tmp80C=tuf;goto _LL524;}_LL523: if((_tmp806.KnownTunionfield).tag != 1)
goto _LL520;_tmp80B=(_tmp806.KnownTunionfield).f1;_tmp80C=(_tmp806.KnownTunionfield).f2;
_LL524: {struct Cyc_List_List*tvs=_tmp80B->tvs;for(0;_tmp787 != 0  && tvs != 0;(
_tmp787=_tmp787->tl,tvs=tvs->tl)){void*t1=(void*)_tmp787->hd;void*k1=Cyc_Tcutil_tvar_kind((
struct Cyc_Absyn_Tvar*)tvs->hd);cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,
cvtenv,k1,t1,1);}if(_tmp787 != 0)({struct Cyc_String_pa_struct _tmp82E;_tmp82E.tag=
0;_tmp82E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp80C->name));{struct Cyc_String_pa_struct _tmp82D;_tmp82D.tag=0;_tmp82D.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp80B->name));{
void*_tmp82B[2]={& _tmp82D,& _tmp82E};Cyc_Tcutil_terr(loc,({const char*_tmp82C="too many type arguments for tunion %s.%s";
_tag_dyneither(_tmp82C,sizeof(char),41);}),_tag_dyneither(_tmp82B,sizeof(void*),
2));}}});if(tvs != 0)({struct Cyc_String_pa_struct _tmp832;_tmp832.tag=0;_tmp832.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp80C->name));{
struct Cyc_String_pa_struct _tmp831;_tmp831.tag=0;_tmp831.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp80B->name));{void*_tmp82F[2]={&
_tmp831,& _tmp832};Cyc_Tcutil_terr(loc,({const char*_tmp830="too few type arguments for tunion %s.%s";
_tag_dyneither(_tmp830,sizeof(char),40);}),_tag_dyneither(_tmp82F,sizeof(void*),
2));}}});goto _LL520;}_LL520:;}goto _LL4D2;_LL4E1: if(*((int*)_tmp773)!= 4)goto
_LL4E3;_tmp788=((struct Cyc_Absyn_PointerType_struct*)_tmp773)->f1;_tmp789=(void*)
_tmp788.elt_typ;_tmp78A=_tmp788.elt_tq;_tmp78B=(struct Cyc_Absyn_Tqual*)&(((
struct Cyc_Absyn_PointerType_struct*)_tmp773)->f1).elt_tq;_tmp78C=_tmp788.ptr_atts;
_tmp78D=(void*)_tmp78C.rgn;_tmp78E=_tmp78C.nullable;_tmp78F=_tmp78C.bounds;
_tmp790=_tmp78C.zero_term;_LL4E2: {int is_zero_terminated;cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,(void*)0,_tmp789,1);_tmp78B->real_const=Cyc_Tcutil_extract_const_from_typedef(
loc,_tmp78B->print_const,_tmp789);cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,
cvtenv,(void*)5,_tmp78D,1);{union Cyc_Absyn_Constraint_union _tmp833=(((struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref*x))Cyc_Absyn_compress_conref)(_tmp790))->v;int _tmp834;
_LL535: if((_tmp833.No_constr).tag != 2)goto _LL537;_LL536:{void*_tmp835=Cyc_Tcutil_compress(
_tmp789);void*_tmp836;_LL53C: if(_tmp835 <= (void*)4)goto _LL53E;if(*((int*)_tmp835)
!= 5)goto _LL53E;_tmp836=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp835)->f2;
if((int)_tmp836 != 0)goto _LL53E;_LL53D:((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*
x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp790,
Cyc_Absyn_true_conref);is_zero_terminated=1;goto _LL53B;_LL53E:;_LL53F:((int(*)(
int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(
Cyc_Core_intcmp,_tmp790,Cyc_Absyn_false_conref);is_zero_terminated=0;goto _LL53B;
_LL53B:;}goto _LL534;_LL537: if((_tmp833.Eq_constr).tag != 0)goto _LL539;_tmp834=(
_tmp833.Eq_constr).f1;if(_tmp834 != 1)goto _LL539;_LL538: if(!Cyc_Tcutil_admits_zero(
_tmp789))({struct Cyc_String_pa_struct _tmp839;_tmp839.tag=0;_tmp839.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp789));{void*
_tmp837[1]={& _tmp839};Cyc_Tcutil_terr(loc,({const char*_tmp838="cannot have a pointer to zero-terminated %s elements";
_tag_dyneither(_tmp838,sizeof(char),53);}),_tag_dyneither(_tmp837,sizeof(void*),
1));}});is_zero_terminated=1;goto _LL534;_LL539:;_LL53A: is_zero_terminated=0;goto
_LL534;_LL534:;}{union Cyc_Absyn_Constraint_union _tmp83A=(Cyc_Absyn_compress_conref(
_tmp78F))->v;void*_tmp83B;void*_tmp83C;struct Cyc_Absyn_Exp*_tmp83D;_LL541: if((
_tmp83A.No_constr).tag != 2)goto _LL543;_LL542: goto _LL544;_LL543: if((_tmp83A.Eq_constr).tag
!= 0)goto _LL545;_tmp83B=(_tmp83A.Eq_constr).f1;if((int)_tmp83B != 0)goto _LL545;
_LL544: goto _LL540;_LL545: if((_tmp83A.Eq_constr).tag != 0)goto _LL547;_tmp83C=(
_tmp83A.Eq_constr).f1;if(_tmp83C <= (void*)1)goto _LL547;if(*((int*)_tmp83C)!= 0)
goto _LL547;_tmp83D=((struct Cyc_Absyn_Upper_b_struct*)_tmp83C)->f1;_LL546: {
struct _RegionHandle*_tmp83E=Cyc_Tcenv_get_fnrgn(te);{struct Cyc_Tcenv_Tenv*
_tmp83F=Cyc_Tcenv_allow_valueof(_tmp83E,te);Cyc_Tcexp_tcExp(_tmp83F,0,_tmp83D);}
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp83D,te,cvtenv);if(!Cyc_Tcutil_coerce_uint_typ(
te,_tmp83D))({void*_tmp840=0;Cyc_Tcutil_terr(loc,({const char*_tmp841="pointer bounds expression is not an unsigned int";
_tag_dyneither(_tmp841,sizeof(char),49);}),_tag_dyneither(_tmp840,sizeof(void*),
0));});{unsigned int _tmp843;int _tmp844;struct _tuple7 _tmp842=Cyc_Evexp_eval_const_uint_exp(
_tmp83D);_tmp843=_tmp842.f1;_tmp844=_tmp842.f2;if(is_zero_terminated  && (!
_tmp844  || _tmp843 < 1))({void*_tmp845=0;Cyc_Tcutil_terr(loc,({const char*_tmp846="zero-terminated pointer cannot point to empty sequence";
_tag_dyneither(_tmp846,sizeof(char),55);}),_tag_dyneither(_tmp845,sizeof(void*),
0));});goto _LL540;}}_LL547: if((_tmp83A.Forward_constr).tag != 1)goto _LL540;_LL548:({
void*_tmp847=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp848="forward constraint";_tag_dyneither(_tmp848,sizeof(char),19);}),
_tag_dyneither(_tmp847,sizeof(void*),0));});_LL540:;}goto _LL4D2;}_LL4E3: if(*((
int*)_tmp773)!= 18)goto _LL4E5;_tmp791=(void*)((struct Cyc_Absyn_TagType_struct*)
_tmp773)->f1;_LL4E4: cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)7,
_tmp791,1);goto _LL4D2;_LL4E5: if(*((int*)_tmp773)!= 17)goto _LL4E7;_tmp792=((
struct Cyc_Absyn_ValueofType_struct*)_tmp773)->f1;_LL4E6: {struct _RegionHandle*
_tmp849=Cyc_Tcenv_get_fnrgn(te);{struct Cyc_Tcenv_Tenv*_tmp84A=Cyc_Tcenv_allow_valueof(
_tmp849,te);Cyc_Tcexp_tcExp(_tmp84A,0,_tmp792);}if(!Cyc_Tcutil_coerce_uint_typ(
te,_tmp792))({void*_tmp84B=0;Cyc_Tcutil_terr(loc,({const char*_tmp84C="valueof_t requires an int expression";
_tag_dyneither(_tmp84C,sizeof(char),37);}),_tag_dyneither(_tmp84B,sizeof(void*),
0));});cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp792,te,cvtenv);goto
_LL4D2;}_LL4E7: if(*((int*)_tmp773)!= 5)goto _LL4E9;_LL4E8: goto _LL4EA;_LL4E9: if((
int)_tmp773 != 1)goto _LL4EB;_LL4EA: goto _LL4EC;_LL4EB: if(_tmp773 <= (void*)4)goto
_LL4F9;if(*((int*)_tmp773)!= 6)goto _LL4ED;_LL4EC: goto _LL4D2;_LL4ED: if(*((int*)
_tmp773)!= 7)goto _LL4EF;_tmp793=((struct Cyc_Absyn_ArrayType_struct*)_tmp773)->f1;
_tmp794=(void*)_tmp793.elt_type;_tmp795=_tmp793.tq;_tmp796=(struct Cyc_Absyn_Tqual*)&(((
struct Cyc_Absyn_ArrayType_struct*)_tmp773)->f1).tq;_tmp797=_tmp793.num_elts;
_tmp798=(struct Cyc_Absyn_Exp**)&(((struct Cyc_Absyn_ArrayType_struct*)_tmp773)->f1).num_elts;
_tmp799=_tmp793.zero_term;_tmp79A=_tmp793.zt_loc;_LL4EE: {struct Cyc_Absyn_Exp*
_tmp84D=*_tmp798;cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)1,
_tmp794,1);_tmp796->real_const=Cyc_Tcutil_extract_const_from_typedef(loc,_tmp796->print_const,
_tmp794);{int is_zero_terminated;{union Cyc_Absyn_Constraint_union _tmp84E=(((
struct Cyc_Absyn_Conref*(*)(struct Cyc_Absyn_Conref*x))Cyc_Absyn_compress_conref)(
_tmp799))->v;int _tmp84F;_LL54A: if((_tmp84E.No_constr).tag != 2)goto _LL54C;_LL54B:((
int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(
Cyc_Core_intcmp,_tmp799,Cyc_Absyn_false_conref);is_zero_terminated=0;goto _LL549;
_LL54C: if((_tmp84E.Eq_constr).tag != 0)goto _LL54E;_tmp84F=(_tmp84E.Eq_constr).f1;
if(_tmp84F != 1)goto _LL54E;_LL54D: if(!Cyc_Tcutil_admits_zero(_tmp794))({struct Cyc_String_pa_struct
_tmp852;_tmp852.tag=0;_tmp852.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp794));{void*_tmp850[1]={& _tmp852};Cyc_Tcutil_terr(loc,({
const char*_tmp851="cannot have a zero-terminated array of %s elements";
_tag_dyneither(_tmp851,sizeof(char),51);}),_tag_dyneither(_tmp850,sizeof(void*),
1));}});is_zero_terminated=1;goto _LL549;_LL54E:;_LL54F: is_zero_terminated=0;goto
_LL549;_LL549:;}if(_tmp84D == 0){if(is_zero_terminated)*_tmp798=(_tmp84D=(struct
Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(1,0));else{({void*_tmp853=0;Cyc_Tcutil_warn(
loc,({const char*_tmp854="array bound defaults to 1 here";_tag_dyneither(_tmp854,
sizeof(char),31);}),_tag_dyneither(_tmp853,sizeof(void*),0));});*_tmp798=(
_tmp84D=(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(1,0));}}Cyc_Tcexp_tcExp(te,0,(
struct Cyc_Absyn_Exp*)_tmp84D);if(!Cyc_Tcutil_is_const_exp(te,(struct Cyc_Absyn_Exp*)
_tmp84D))({void*_tmp855=0;Cyc_Tcutil_terr(loc,({const char*_tmp856="array bounds expression is not constant";
_tag_dyneither(_tmp856,sizeof(char),40);}),_tag_dyneither(_tmp855,sizeof(void*),
0));});if(!Cyc_Tcutil_coerce_uint_typ(te,(struct Cyc_Absyn_Exp*)_tmp84D))({void*
_tmp857=0;Cyc_Tcutil_terr(loc,({const char*_tmp858="array bounds expression is not an unsigned int";
_tag_dyneither(_tmp858,sizeof(char),47);}),_tag_dyneither(_tmp857,sizeof(void*),
0));});{unsigned int _tmp85A;int _tmp85B;struct _tuple7 _tmp859=Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)_tmp84D);_tmp85A=_tmp859.f1;_tmp85B=_tmp859.f2;if((
is_zero_terminated  && _tmp85B) && _tmp85A < 1)({void*_tmp85C=0;Cyc_Tcutil_warn(
loc,({const char*_tmp85D="zero terminated array cannot have zero elements";
_tag_dyneither(_tmp85D,sizeof(char),48);}),_tag_dyneither(_tmp85C,sizeof(void*),
0));});if((_tmp85B  && _tmp85A < 1) && Cyc_Cyclone_tovc_r){({void*_tmp85E=0;Cyc_Tcutil_warn(
loc,({const char*_tmp85F="arrays with 0 elements are not supported except with gcc -- changing to 1.";
_tag_dyneither(_tmp85F,sizeof(char),75);}),_tag_dyneither(_tmp85E,sizeof(void*),
0));});*_tmp798=(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(1,0);}goto _LL4D2;}}}
_LL4EF: if(*((int*)_tmp773)!= 8)goto _LL4F1;_tmp79B=((struct Cyc_Absyn_FnType_struct*)
_tmp773)->f1;_tmp79C=_tmp79B.tvars;_tmp79D=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_struct*)
_tmp773)->f1).tvars;_tmp79E=_tmp79B.effect;_tmp79F=(struct Cyc_Core_Opt**)&(((
struct Cyc_Absyn_FnType_struct*)_tmp773)->f1).effect;_tmp7A0=(void*)_tmp79B.ret_typ;
_tmp7A1=_tmp79B.args;_tmp7A2=_tmp79B.c_varargs;_tmp7A3=_tmp79B.cyc_varargs;
_tmp7A4=_tmp79B.rgn_po;_tmp7A5=_tmp79B.attributes;_LL4F0: {int num_convs=0;int
seen_cdecl=0;int seen_stdcall=0;int seen_fastcall=0;int seen_format=0;void*ft=(void*)
0;int fmt_desc_arg=- 1;int fmt_arg_start=- 1;for(0;_tmp7A5 != 0;_tmp7A5=_tmp7A5->tl){
if(!Cyc_Absyn_fntype_att((void*)_tmp7A5->hd))({struct Cyc_String_pa_struct _tmp862;
_tmp862.tag=0;_tmp862.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((
void*)_tmp7A5->hd));{void*_tmp860[1]={& _tmp862};Cyc_Tcutil_terr(loc,({const char*
_tmp861="bad function type attribute %s";_tag_dyneither(_tmp861,sizeof(char),31);}),
_tag_dyneither(_tmp860,sizeof(void*),1));}});{void*_tmp863=(void*)_tmp7A5->hd;
void*_tmp864;int _tmp865;int _tmp866;_LL551: if((int)_tmp863 != 0)goto _LL553;_LL552:
if(!seen_stdcall){seen_stdcall=1;++ num_convs;}goto _LL550;_LL553: if((int)_tmp863
!= 1)goto _LL555;_LL554: if(!seen_cdecl){seen_cdecl=1;++ num_convs;}goto _LL550;
_LL555: if((int)_tmp863 != 2)goto _LL557;_LL556: if(!seen_fastcall){seen_fastcall=1;
++ num_convs;}goto _LL550;_LL557: if(_tmp863 <= (void*)17)goto _LL559;if(*((int*)
_tmp863)!= 3)goto _LL559;_tmp864=(void*)((struct Cyc_Absyn_Format_att_struct*)
_tmp863)->f1;_tmp865=((struct Cyc_Absyn_Format_att_struct*)_tmp863)->f2;_tmp866=((
struct Cyc_Absyn_Format_att_struct*)_tmp863)->f3;_LL558: if(!seen_format){
seen_format=1;ft=_tmp864;fmt_desc_arg=_tmp865;fmt_arg_start=_tmp866;}else{({void*
_tmp867=0;Cyc_Tcutil_terr(loc,({const char*_tmp868="function can't have multiple format attributes";
_tag_dyneither(_tmp868,sizeof(char),47);}),_tag_dyneither(_tmp867,sizeof(void*),
0));});}goto _LL550;_LL559:;_LL55A: goto _LL550;_LL550:;}}if(num_convs > 1)({void*
_tmp869=0;Cyc_Tcutil_terr(loc,({const char*_tmp86A="function can't have multiple calling conventions";
_tag_dyneither(_tmp86A,sizeof(char),49);}),_tag_dyneither(_tmp869,sizeof(void*),
0));});Cyc_Tcutil_check_unique_tvars(loc,*_tmp79D);{struct Cyc_List_List*b=*
_tmp79D;for(0;b != 0;b=b->tl){((struct Cyc_Absyn_Tvar*)b->hd)->identity=Cyc_Tcutil_new_tvar_id();
cvtenv.kind_env=Cyc_Tcutil_add_bound_tvar(cvtenv.kind_env,(struct Cyc_Absyn_Tvar*)
b->hd);{void*_tmp86B=Cyc_Absyn_compress_kb((void*)((struct Cyc_Absyn_Tvar*)b->hd)->kind);
void*_tmp86C;_LL55C: if(*((int*)_tmp86B)!= 0)goto _LL55E;_tmp86C=(void*)((struct
Cyc_Absyn_Eq_kb_struct*)_tmp86B)->f1;if((int)_tmp86C != 1)goto _LL55E;_LL55D:({
struct Cyc_String_pa_struct _tmp86F;_tmp86F.tag=0;_tmp86F.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)b->hd)->name);{void*_tmp86D[1]={&
_tmp86F};Cyc_Tcutil_terr(loc,({const char*_tmp86E="function attempts to abstract Mem type variable %s";
_tag_dyneither(_tmp86E,sizeof(char),51);}),_tag_dyneither(_tmp86D,sizeof(void*),
1));}});goto _LL55B;_LL55E:;_LL55F: goto _LL55B;_LL55B:;}}}{struct _RegionHandle*
_tmp870=Cyc_Tcenv_get_fnrgn(te);{struct Cyc_Tcutil_CVTEnv _tmp871=({struct Cyc_Tcutil_CVTEnv
_tmp8F6;_tmp8F6.r=_tmp870;_tmp8F6.kind_env=cvtenv.kind_env;_tmp8F6.free_vars=0;
_tmp8F6.free_evars=0;_tmp8F6.generalize_evars=cvtenv.generalize_evars;_tmp8F6.fn_result=
1;_tmp8F6;});_tmp871=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp871,(void*)1,
_tmp7A0,1);_tmp871.fn_result=0;{struct Cyc_List_List*a=_tmp7A1;for(0;a != 0;a=a->tl){
struct _tuple3*_tmp872=(struct _tuple3*)a->hd;void*_tmp873=(*_tmp872).f3;_tmp871=
Cyc_Tcutil_i_check_valid_type(loc,te,_tmp871,(void*)1,_tmp873,1);((*_tmp872).f2).real_const=
Cyc_Tcutil_extract_const_from_typedef(loc,((*_tmp872).f2).print_const,_tmp873);}}
if(_tmp7A3 != 0){if(_tmp7A2)({void*_tmp874=0;((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp875="both c_vararg and cyc_vararg";
_tag_dyneither(_tmp875,sizeof(char),29);}),_tag_dyneither(_tmp874,sizeof(void*),
0));});{void*_tmp877;int _tmp878;struct Cyc_Absyn_VarargInfo _tmp876=*_tmp7A3;
_tmp877=(void*)_tmp876.type;_tmp878=_tmp876.inject;_tmp871=Cyc_Tcutil_i_check_valid_type(
loc,te,_tmp871,(void*)1,_tmp877,1);(_tmp7A3->tq).real_const=Cyc_Tcutil_extract_const_from_typedef(
loc,(_tmp7A3->tq).print_const,_tmp877);if(_tmp878){void*_tmp879=Cyc_Tcutil_compress(
_tmp877);struct Cyc_Absyn_TunionInfo _tmp87A;union Cyc_Absyn_TunionInfoU_union
_tmp87B;struct Cyc_Absyn_Tuniondecl**_tmp87C;_LL561: if(_tmp879 <= (void*)4)goto
_LL563;if(*((int*)_tmp879)!= 2)goto _LL563;_tmp87A=((struct Cyc_Absyn_TunionType_struct*)
_tmp879)->f1;_tmp87B=_tmp87A.tunion_info;if((((((struct Cyc_Absyn_TunionType_struct*)
_tmp879)->f1).tunion_info).KnownTunion).tag != 1)goto _LL563;_tmp87C=(_tmp87B.KnownTunion).f1;
_LL562: if((*_tmp87C)->is_flat)({void*_tmp87D=0;Cyc_Tcutil_terr(loc,({const char*
_tmp87E="cant inject into a flat tunion";_tag_dyneither(_tmp87E,sizeof(char),31);}),
_tag_dyneither(_tmp87D,sizeof(void*),0));});goto _LL560;_LL563:;_LL564:({void*
_tmp87F=0;Cyc_Tcutil_terr(loc,({const char*_tmp880="can't inject a non-[x]tunion type";
_tag_dyneither(_tmp880,sizeof(char),34);}),_tag_dyneither(_tmp87F,sizeof(void*),
0));});goto _LL560;_LL560:;}}}if(seen_format){int _tmp881=((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(_tmp7A1);if((((fmt_desc_arg < 0  || fmt_desc_arg > _tmp881) || 
fmt_arg_start < 0) || _tmp7A3 == 0  && fmt_arg_start != 0) || _tmp7A3 != 0  && 
fmt_arg_start != _tmp881 + 1)({void*_tmp882=0;Cyc_Tcutil_terr(loc,({const char*
_tmp883="bad format descriptor";_tag_dyneither(_tmp883,sizeof(char),22);}),
_tag_dyneither(_tmp882,sizeof(void*),0));});else{void*_tmp885;struct _tuple3
_tmp884=*((struct _tuple3*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp7A1,
fmt_desc_arg - 1);_tmp885=_tmp884.f3;{void*_tmp886=Cyc_Tcutil_compress(_tmp885);
struct Cyc_Absyn_PtrInfo _tmp887;void*_tmp888;struct Cyc_Absyn_PtrAtts _tmp889;
struct Cyc_Absyn_Conref*_tmp88A;struct Cyc_Absyn_Conref*_tmp88B;_LL566: if(_tmp886
<= (void*)4)goto _LL568;if(*((int*)_tmp886)!= 4)goto _LL568;_tmp887=((struct Cyc_Absyn_PointerType_struct*)
_tmp886)->f1;_tmp888=(void*)_tmp887.elt_typ;_tmp889=_tmp887.ptr_atts;_tmp88A=
_tmp889.bounds;_tmp88B=_tmp889.zero_term;_LL567:{struct _tuple0 _tmp88D=({struct
_tuple0 _tmp88C;_tmp88C.f1=Cyc_Tcutil_compress(_tmp888);_tmp88C.f2=Cyc_Absyn_conref_def((
void*)((void*)0),_tmp88A);_tmp88C;});void*_tmp88E;void*_tmp88F;void*_tmp890;void*
_tmp891;_LL56B: _tmp88E=_tmp88D.f1;if(_tmp88E <= (void*)4)goto _LL56D;if(*((int*)
_tmp88E)!= 5)goto _LL56D;_tmp88F=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp88E)->f1;
if((int)_tmp88F != 2)goto _LL56D;_tmp890=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp88E)->f2;if((int)_tmp890 != 0)goto _LL56D;_tmp891=_tmp88D.f2;if((int)_tmp891 != 
0)goto _LL56D;_LL56C: goto _LL56A;_LL56D:;_LL56E:({void*_tmp892=0;Cyc_Tcutil_terr(
loc,({const char*_tmp893="format descriptor is not a char ? type";_tag_dyneither(
_tmp893,sizeof(char),39);}),_tag_dyneither(_tmp892,sizeof(void*),0));});goto
_LL56A;_LL56A:;}goto _LL565;_LL568:;_LL569:({void*_tmp894=0;Cyc_Tcutil_terr(loc,({
const char*_tmp895="format descriptor is not a char ? type";_tag_dyneither(
_tmp895,sizeof(char),39);}),_tag_dyneither(_tmp894,sizeof(void*),0));});goto
_LL565;_LL565:;}if(fmt_arg_start != 0){int problem;{struct _tuple0 _tmp897=({struct
_tuple0 _tmp896;_tmp896.f1=ft;_tmp896.f2=Cyc_Tcutil_compress((void*)((struct Cyc_Absyn_VarargInfo*)
_check_null(_tmp7A3))->type);_tmp896;});void*_tmp898;void*_tmp899;struct Cyc_Absyn_TunionInfo
_tmp89A;union Cyc_Absyn_TunionInfoU_union _tmp89B;struct Cyc_Absyn_Tuniondecl**
_tmp89C;struct Cyc_Absyn_Tuniondecl*_tmp89D;void*_tmp89E;void*_tmp89F;struct Cyc_Absyn_TunionInfo
_tmp8A0;union Cyc_Absyn_TunionInfoU_union _tmp8A1;struct Cyc_Absyn_Tuniondecl**
_tmp8A2;struct Cyc_Absyn_Tuniondecl*_tmp8A3;_LL570: _tmp898=_tmp897.f1;if((int)
_tmp898 != 0)goto _LL572;_tmp899=_tmp897.f2;if(_tmp899 <= (void*)4)goto _LL572;if(*((
int*)_tmp899)!= 2)goto _LL572;_tmp89A=((struct Cyc_Absyn_TunionType_struct*)
_tmp899)->f1;_tmp89B=_tmp89A.tunion_info;if((((((struct Cyc_Absyn_TunionType_struct*)
_tmp899)->f1).tunion_info).KnownTunion).tag != 1)goto _LL572;_tmp89C=(_tmp89B.KnownTunion).f1;
_tmp89D=*_tmp89C;_LL571: problem=Cyc_Absyn_qvar_cmp(_tmp89D->name,Cyc_Absyn_tunion_print_arg_qvar)
!= 0;goto _LL56F;_LL572: _tmp89E=_tmp897.f1;if((int)_tmp89E != 1)goto _LL574;_tmp89F=
_tmp897.f2;if(_tmp89F <= (void*)4)goto _LL574;if(*((int*)_tmp89F)!= 2)goto _LL574;
_tmp8A0=((struct Cyc_Absyn_TunionType_struct*)_tmp89F)->f1;_tmp8A1=_tmp8A0.tunion_info;
if((((((struct Cyc_Absyn_TunionType_struct*)_tmp89F)->f1).tunion_info).KnownTunion).tag
!= 1)goto _LL574;_tmp8A2=(_tmp8A1.KnownTunion).f1;_tmp8A3=*_tmp8A2;_LL573: problem=
Cyc_Absyn_qvar_cmp(_tmp8A3->name,Cyc_Absyn_tunion_scanf_arg_qvar)!= 0;goto _LL56F;
_LL574:;_LL575: problem=1;goto _LL56F;_LL56F:;}if(problem)({void*_tmp8A4=0;Cyc_Tcutil_terr(
loc,({const char*_tmp8A5="format attribute and vararg types don't match";
_tag_dyneither(_tmp8A5,sizeof(char),46);}),_tag_dyneither(_tmp8A4,sizeof(void*),
0));});}}}{struct Cyc_List_List*rpo=_tmp7A4;for(0;rpo != 0;rpo=rpo->tl){struct
_tuple0 _tmp8A7;void*_tmp8A8;void*_tmp8A9;struct _tuple0*_tmp8A6=(struct _tuple0*)
rpo->hd;_tmp8A7=*_tmp8A6;_tmp8A8=_tmp8A7.f1;_tmp8A9=_tmp8A7.f2;_tmp871=Cyc_Tcutil_i_check_valid_type(
loc,te,_tmp871,(void*)6,_tmp8A8,1);_tmp871=Cyc_Tcutil_i_check_valid_type(loc,te,
_tmp871,(void*)5,_tmp8A9,1);}}if(*_tmp79F != 0)_tmp871=Cyc_Tcutil_i_check_valid_type(
loc,te,_tmp871,(void*)6,(void*)((struct Cyc_Core_Opt*)_check_null(*_tmp79F))->v,1);
else{struct Cyc_List_List*effect=0;{struct Cyc_List_List*tvs=_tmp871.free_vars;
for(0;tvs != 0;tvs=tvs->tl){struct Cyc_Absyn_Tvar*_tmp8AB;int _tmp8AC;struct
_tuple18 _tmp8AA=*((struct _tuple18*)tvs->hd);_tmp8AB=_tmp8AA.f1;_tmp8AC=_tmp8AA.f2;
if(!_tmp8AC)continue;{void*_tmp8AD=Cyc_Absyn_compress_kb((void*)_tmp8AB->kind);
struct Cyc_Core_Opt*_tmp8AE;struct Cyc_Core_Opt**_tmp8AF;void*_tmp8B0;struct Cyc_Core_Opt*
_tmp8B1;struct Cyc_Core_Opt**_tmp8B2;void*_tmp8B3;void*_tmp8B4;void*_tmp8B5;void*
_tmp8B6;struct Cyc_Core_Opt*_tmp8B7;struct Cyc_Core_Opt**_tmp8B8;void*_tmp8B9;void*
_tmp8BA;struct Cyc_Core_Opt*_tmp8BB;struct Cyc_Core_Opt**_tmp8BC;_LL577: if(*((int*)
_tmp8AD)!= 2)goto _LL579;_tmp8AE=((struct Cyc_Absyn_Less_kb_struct*)_tmp8AD)->f1;
_tmp8AF=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)_tmp8AD)->f1;
_tmp8B0=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp8AD)->f2;if((int)_tmp8B0 != 
5)goto _LL579;_LL578: _tmp8B2=_tmp8AF;_tmp8B3=(void*)3;goto _LL57A;_LL579: if(*((int*)
_tmp8AD)!= 2)goto _LL57B;_tmp8B1=((struct Cyc_Absyn_Less_kb_struct*)_tmp8AD)->f1;
_tmp8B2=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)_tmp8AD)->f1;
_tmp8B3=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp8AD)->f2;if(!(_tmp8B3 == (
void*)3  || _tmp8B3 == (void*)4))goto _LL57B;_LL57A:*_tmp8B2=Cyc_Tcutil_kind_to_bound_opt(
_tmp8B3);_tmp8B4=_tmp8B3;goto _LL57C;_LL57B: if(*((int*)_tmp8AD)!= 0)goto _LL57D;
_tmp8B4=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp8AD)->f1;if(!((_tmp8B4 == (
void*)3  || _tmp8B4 == (void*)4) || _tmp8B4 == (void*)5))goto _LL57D;_LL57C: effect=({
struct Cyc_List_List*_tmp8BD=_cycalloc(sizeof(*_tmp8BD));_tmp8BD->hd=(void*)((
void*)({struct Cyc_Absyn_AccessEff_struct*_tmp8BE=_cycalloc(sizeof(*_tmp8BE));
_tmp8BE[0]=({struct Cyc_Absyn_AccessEff_struct _tmp8BF;_tmp8BF.tag=19;_tmp8BF.f1=(
void*)((void*)({struct Cyc_Absyn_VarType_struct*_tmp8C0=_cycalloc(sizeof(*_tmp8C0));
_tmp8C0[0]=({struct Cyc_Absyn_VarType_struct _tmp8C1;_tmp8C1.tag=1;_tmp8C1.f1=
_tmp8AB;_tmp8C1;});_tmp8C0;}));_tmp8BF;});_tmp8BE;}));_tmp8BD->tl=effect;_tmp8BD;});
goto _LL576;_LL57D: if(*((int*)_tmp8AD)!= 2)goto _LL57F;_tmp8B5=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp8AD)->f2;if((int)_tmp8B5 != 7)goto _LL57F;_LL57E: goto _LL580;_LL57F: if(*((int*)
_tmp8AD)!= 0)goto _LL581;_tmp8B6=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp8AD)->f1;
if((int)_tmp8B6 != 7)goto _LL581;_LL580: goto _LL576;_LL581: if(*((int*)_tmp8AD)!= 2)
goto _LL583;_tmp8B7=((struct Cyc_Absyn_Less_kb_struct*)_tmp8AD)->f1;_tmp8B8=(
struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)_tmp8AD)->f1;_tmp8B9=(
void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp8AD)->f2;if((int)_tmp8B9 != 6)goto
_LL583;_LL582:*_tmp8B8=Cyc_Tcutil_kind_to_bound_opt((void*)6);goto _LL584;_LL583:
if(*((int*)_tmp8AD)!= 0)goto _LL585;_tmp8BA=(void*)((struct Cyc_Absyn_Eq_kb_struct*)
_tmp8AD)->f1;if((int)_tmp8BA != 6)goto _LL585;_LL584: effect=({struct Cyc_List_List*
_tmp8C2=_cycalloc(sizeof(*_tmp8C2));_tmp8C2->hd=(void*)((void*)({struct Cyc_Absyn_VarType_struct*
_tmp8C3=_cycalloc(sizeof(*_tmp8C3));_tmp8C3[0]=({struct Cyc_Absyn_VarType_struct
_tmp8C4;_tmp8C4.tag=1;_tmp8C4.f1=_tmp8AB;_tmp8C4;});_tmp8C3;}));_tmp8C2->tl=
effect;_tmp8C2;});goto _LL576;_LL585: if(*((int*)_tmp8AD)!= 1)goto _LL587;_tmp8BB=((
struct Cyc_Absyn_Unknown_kb_struct*)_tmp8AD)->f1;_tmp8BC=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Unknown_kb_struct*)_tmp8AD)->f1;_LL586:*_tmp8BC=({struct Cyc_Core_Opt*
_tmp8C5=_cycalloc(sizeof(*_tmp8C5));_tmp8C5->v=(void*)((void*)({struct Cyc_Absyn_Less_kb_struct*
_tmp8C6=_cycalloc(sizeof(*_tmp8C6));_tmp8C6[0]=({struct Cyc_Absyn_Less_kb_struct
_tmp8C7;_tmp8C7.tag=2;_tmp8C7.f1=0;_tmp8C7.f2=(void*)((void*)0);_tmp8C7;});
_tmp8C6;}));_tmp8C5;});goto _LL588;_LL587:;_LL588: effect=({struct Cyc_List_List*
_tmp8C8=_cycalloc(sizeof(*_tmp8C8));_tmp8C8->hd=(void*)((void*)({struct Cyc_Absyn_RgnsEff_struct*
_tmp8C9=_cycalloc(sizeof(*_tmp8C9));_tmp8C9[0]=({struct Cyc_Absyn_RgnsEff_struct
_tmp8CA;_tmp8CA.tag=21;_tmp8CA.f1=(void*)((void*)({struct Cyc_Absyn_VarType_struct*
_tmp8CB=_cycalloc(sizeof(*_tmp8CB));_tmp8CB[0]=({struct Cyc_Absyn_VarType_struct
_tmp8CC;_tmp8CC.tag=1;_tmp8CC.f1=_tmp8AB;_tmp8CC;});_tmp8CB;}));_tmp8CA;});
_tmp8C9;}));_tmp8C8->tl=effect;_tmp8C8;});goto _LL576;_LL576:;}}}{struct Cyc_List_List*
ts=_tmp871.free_evars;for(0;ts != 0;ts=ts->tl){void*_tmp8CE;int _tmp8CF;struct
_tuple19 _tmp8CD=*((struct _tuple19*)ts->hd);_tmp8CE=_tmp8CD.f1;_tmp8CF=_tmp8CD.f2;
if(!_tmp8CF)continue;{void*_tmp8D0=Cyc_Tcutil_typ_kind(_tmp8CE);_LL58A: if((int)
_tmp8D0 != 5)goto _LL58C;_LL58B: goto _LL58D;_LL58C: if((int)_tmp8D0 != 4)goto _LL58E;
_LL58D: goto _LL58F;_LL58E: if((int)_tmp8D0 != 3)goto _LL590;_LL58F: effect=({struct
Cyc_List_List*_tmp8D1=_cycalloc(sizeof(*_tmp8D1));_tmp8D1->hd=(void*)((void*)({
struct Cyc_Absyn_AccessEff_struct*_tmp8D2=_cycalloc(sizeof(*_tmp8D2));_tmp8D2[0]=({
struct Cyc_Absyn_AccessEff_struct _tmp8D3;_tmp8D3.tag=19;_tmp8D3.f1=(void*)_tmp8CE;
_tmp8D3;});_tmp8D2;}));_tmp8D1->tl=effect;_tmp8D1;});goto _LL589;_LL590: if((int)
_tmp8D0 != 6)goto _LL592;_LL591: effect=({struct Cyc_List_List*_tmp8D4=_cycalloc(
sizeof(*_tmp8D4));_tmp8D4->hd=(void*)_tmp8CE;_tmp8D4->tl=effect;_tmp8D4;});goto
_LL589;_LL592: if((int)_tmp8D0 != 7)goto _LL594;_LL593: goto _LL589;_LL594:;_LL595:
effect=({struct Cyc_List_List*_tmp8D5=_cycalloc(sizeof(*_tmp8D5));_tmp8D5->hd=(
void*)((void*)({struct Cyc_Absyn_RgnsEff_struct*_tmp8D6=_cycalloc(sizeof(*_tmp8D6));
_tmp8D6[0]=({struct Cyc_Absyn_RgnsEff_struct _tmp8D7;_tmp8D7.tag=21;_tmp8D7.f1=(
void*)_tmp8CE;_tmp8D7;});_tmp8D6;}));_tmp8D5->tl=effect;_tmp8D5;});goto _LL589;
_LL589:;}}}*_tmp79F=({struct Cyc_Core_Opt*_tmp8D8=_cycalloc(sizeof(*_tmp8D8));
_tmp8D8->v=(void*)((void*)({struct Cyc_Absyn_JoinEff_struct*_tmp8D9=_cycalloc(
sizeof(*_tmp8D9));_tmp8D9[0]=({struct Cyc_Absyn_JoinEff_struct _tmp8DA;_tmp8DA.tag=
20;_tmp8DA.f1=effect;_tmp8DA;});_tmp8D9;}));_tmp8D8;});}if(*_tmp79D != 0){struct
Cyc_List_List*bs=*_tmp79D;for(0;bs != 0;bs=bs->tl){void*_tmp8DB=Cyc_Absyn_compress_kb((
void*)((struct Cyc_Absyn_Tvar*)bs->hd)->kind);struct Cyc_Core_Opt*_tmp8DC;struct
Cyc_Core_Opt**_tmp8DD;struct Cyc_Core_Opt*_tmp8DE;struct Cyc_Core_Opt**_tmp8DF;
void*_tmp8E0;struct Cyc_Core_Opt*_tmp8E1;struct Cyc_Core_Opt**_tmp8E2;void*_tmp8E3;
struct Cyc_Core_Opt*_tmp8E4;struct Cyc_Core_Opt**_tmp8E5;void*_tmp8E6;struct Cyc_Core_Opt*
_tmp8E7;struct Cyc_Core_Opt**_tmp8E8;void*_tmp8E9;void*_tmp8EA;_LL597: if(*((int*)
_tmp8DB)!= 1)goto _LL599;_tmp8DC=((struct Cyc_Absyn_Unknown_kb_struct*)_tmp8DB)->f1;
_tmp8DD=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)_tmp8DB)->f1;
_LL598:({struct Cyc_String_pa_struct _tmp8ED;_tmp8ED.tag=0;_tmp8ED.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)bs->hd)->name);{
void*_tmp8EB[1]={& _tmp8ED};Cyc_Tcutil_warn(loc,({const char*_tmp8EC="Type variable %s unconstrained, assuming boxed";
_tag_dyneither(_tmp8EC,sizeof(char),47);}),_tag_dyneither(_tmp8EB,sizeof(void*),
1));}});_tmp8DF=_tmp8DD;goto _LL59A;_LL599: if(*((int*)_tmp8DB)!= 2)goto _LL59B;
_tmp8DE=((struct Cyc_Absyn_Less_kb_struct*)_tmp8DB)->f1;_tmp8DF=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*)_tmp8DB)->f1;_tmp8E0=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp8DB)->f2;if((int)_tmp8E0 != 1)goto _LL59B;_LL59A: _tmp8E2=_tmp8DF;goto _LL59C;
_LL59B: if(*((int*)_tmp8DB)!= 2)goto _LL59D;_tmp8E1=((struct Cyc_Absyn_Less_kb_struct*)
_tmp8DB)->f1;_tmp8E2=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp8DB)->f1;_tmp8E3=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp8DB)->f2;if((
int)_tmp8E3 != 0)goto _LL59D;_LL59C:*_tmp8E2=Cyc_Tcutil_kind_to_bound_opt((void*)2);
goto _LL596;_LL59D: if(*((int*)_tmp8DB)!= 2)goto _LL59F;_tmp8E4=((struct Cyc_Absyn_Less_kb_struct*)
_tmp8DB)->f1;_tmp8E5=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp8DB)->f1;_tmp8E6=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp8DB)->f2;if((
int)_tmp8E6 != 5)goto _LL59F;_LL59E:*_tmp8E5=Cyc_Tcutil_kind_to_bound_opt((void*)3);
goto _LL596;_LL59F: if(*((int*)_tmp8DB)!= 2)goto _LL5A1;_tmp8E7=((struct Cyc_Absyn_Less_kb_struct*)
_tmp8DB)->f1;_tmp8E8=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp8DB)->f1;_tmp8E9=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp8DB)->f2;
_LL5A0:*_tmp8E8=Cyc_Tcutil_kind_to_bound_opt(_tmp8E9);goto _LL596;_LL5A1: if(*((
int*)_tmp8DB)!= 0)goto _LL5A3;_tmp8EA=(void*)((struct Cyc_Absyn_Eq_kb_struct*)
_tmp8DB)->f1;if((int)_tmp8EA != 1)goto _LL5A3;_LL5A2:({void*_tmp8EE=0;Cyc_Tcutil_terr(
loc,({const char*_tmp8EF="functions can't abstract M types";_tag_dyneither(
_tmp8EF,sizeof(char),33);}),_tag_dyneither(_tmp8EE,sizeof(void*),0));});goto
_LL596;_LL5A3:;_LL5A4: goto _LL596;_LL596:;}}cvtenv.kind_env=Cyc_Tcutil_remove_bound_tvars(
Cyc_Core_heap_region,_tmp871.kind_env,*_tmp79D);_tmp871.free_vars=Cyc_Tcutil_remove_bound_tvars_bool(
_tmp871.r,_tmp871.free_vars,*_tmp79D);{struct Cyc_List_List*tvs=_tmp871.free_vars;
for(0;tvs != 0;tvs=tvs->tl){struct Cyc_Absyn_Tvar*_tmp8F1;int _tmp8F2;struct
_tuple18 _tmp8F0=*((struct _tuple18*)tvs->hd);_tmp8F1=_tmp8F0.f1;_tmp8F2=_tmp8F0.f2;
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,
_tmp8F1,_tmp8F2);}}{struct Cyc_List_List*evs=_tmp871.free_evars;for(0;evs != 0;evs=
evs->tl){void*_tmp8F4;int _tmp8F5;struct _tuple19 _tmp8F3=*((struct _tuple19*)evs->hd);
_tmp8F4=_tmp8F3.f1;_tmp8F5=_tmp8F3.f2;cvtenv.free_evars=Cyc_Tcutil_add_free_evar(
cvtenv.r,cvtenv.free_evars,_tmp8F4,_tmp8F5);}}}goto _LL4D2;}}_LL4F1: if(*((int*)
_tmp773)!= 9)goto _LL4F3;_tmp7A6=((struct Cyc_Absyn_TupleType_struct*)_tmp773)->f1;
_LL4F2: for(0;_tmp7A6 != 0;_tmp7A6=_tmp7A6->tl){struct _tuple5*_tmp8F7=(struct
_tuple5*)_tmp7A6->hd;cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)1,(*
_tmp8F7).f2,1);((*_tmp8F7).f1).real_const=Cyc_Tcutil_extract_const_from_typedef(
loc,((*_tmp8F7).f1).print_const,(*_tmp8F7).f2);}goto _LL4D2;_LL4F3: if(*((int*)
_tmp773)!= 11)goto _LL4F5;_tmp7A7=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp773)->f1;_tmp7A8=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp773)->f2;_LL4F4: {
struct _RegionHandle*_tmp8F8=Cyc_Tcenv_get_fnrgn(te);{struct Cyc_List_List*
prev_fields=0;for(0;_tmp7A8 != 0;_tmp7A8=_tmp7A8->tl){struct Cyc_Absyn_Aggrfield
_tmp8FA;struct _dyneither_ptr*_tmp8FB;struct Cyc_Absyn_Tqual _tmp8FC;struct Cyc_Absyn_Tqual*
_tmp8FD;void*_tmp8FE;struct Cyc_Absyn_Exp*_tmp8FF;struct Cyc_List_List*_tmp900;
struct Cyc_Absyn_Aggrfield*_tmp8F9=(struct Cyc_Absyn_Aggrfield*)_tmp7A8->hd;
_tmp8FA=*_tmp8F9;_tmp8FB=_tmp8FA.name;_tmp8FC=_tmp8FA.tq;_tmp8FD=(struct Cyc_Absyn_Tqual*)&(*
_tmp8F9).tq;_tmp8FE=(void*)_tmp8FA.type;_tmp8FF=_tmp8FA.width;_tmp900=_tmp8FA.attributes;
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*
l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,_tmp8FB))({
struct Cyc_String_pa_struct _tmp903;_tmp903.tag=0;_tmp903.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)*_tmp8FB);{void*_tmp901[1]={& _tmp903};Cyc_Tcutil_terr(loc,({
const char*_tmp902="duplicate field %s";_tag_dyneither(_tmp902,sizeof(char),19);}),
_tag_dyneither(_tmp901,sizeof(void*),1));}});if(Cyc_strcmp((struct _dyneither_ptr)*
_tmp8FB,({const char*_tmp904="";_tag_dyneither(_tmp904,sizeof(char),1);}))!= 0)
prev_fields=({struct Cyc_List_List*_tmp905=_region_malloc(_tmp8F8,sizeof(*_tmp905));
_tmp905->hd=_tmp8FB;_tmp905->tl=prev_fields;_tmp905;});cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,(void*)1,_tmp8FE,1);_tmp8FD->real_const=Cyc_Tcutil_extract_const_from_typedef(
loc,_tmp8FD->print_const,_tmp8FE);if(_tmp7A7 == (void*)1  && !Cyc_Tcutil_bits_only(
_tmp8FE))({struct Cyc_String_pa_struct _tmp908;_tmp908.tag=0;_tmp908.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*_tmp8FB);{void*_tmp906[1]={& _tmp908};Cyc_Tcutil_warn(
loc,({const char*_tmp907="union member %s is not `bits-only' so it can only be written and not read";
_tag_dyneither(_tmp907,sizeof(char),74);}),_tag_dyneither(_tmp906,sizeof(void*),
1));}});Cyc_Tcutil_check_bitfield(loc,te,_tmp8FE,_tmp8FF,_tmp8FB);Cyc_Tcutil_check_field_atts(
loc,_tmp8FB,_tmp900);}}goto _LL4D2;}_LL4F5: if(*((int*)_tmp773)!= 10)goto _LL4F7;
_tmp7A9=((struct Cyc_Absyn_AggrType_struct*)_tmp773)->f1;_tmp7AA=_tmp7A9.aggr_info;
_tmp7AB=(union Cyc_Absyn_AggrInfoU_union*)&(((struct Cyc_Absyn_AggrType_struct*)
_tmp773)->f1).aggr_info;_tmp7AC=_tmp7A9.targs;_tmp7AD=(struct Cyc_List_List**)&(((
struct Cyc_Absyn_AggrType_struct*)_tmp773)->f1).targs;_LL4F6:{union Cyc_Absyn_AggrInfoU_union
_tmp909=*_tmp7AB;void*_tmp90A;struct _tuple2*_tmp90B;struct Cyc_Absyn_Aggrdecl**
_tmp90C;struct Cyc_Absyn_Aggrdecl*_tmp90D;_LL5A6: if((_tmp909.UnknownAggr).tag != 0)
goto _LL5A8;_tmp90A=(_tmp909.UnknownAggr).f1;_tmp90B=(_tmp909.UnknownAggr).f2;
_LL5A7: {struct Cyc_Absyn_Aggrdecl**adp;{struct _handler_cons _tmp90E;_push_handler(&
_tmp90E);{int _tmp910=0;if(setjmp(_tmp90E.handler))_tmp910=1;if(!_tmp910){adp=Cyc_Tcenv_lookup_aggrdecl(
te,loc,_tmp90B);*_tmp7AB=(union Cyc_Absyn_AggrInfoU_union)({union Cyc_Absyn_AggrInfoU_union
_tmp911;(_tmp911.KnownAggr).tag=1;(_tmp911.KnownAggr).f1=adp;_tmp911;});;
_pop_handler();}else{void*_tmp90F=(void*)_exn_thrown;void*_tmp913=_tmp90F;_LL5AB:
if(_tmp913 != Cyc_Dict_Absent)goto _LL5AD;_LL5AC: {struct Cyc_Tcenv_Genv*_tmp914=((
struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(
te->ae,te->ns);struct Cyc_Absyn_Aggrdecl*_tmp915=({struct Cyc_Absyn_Aggrdecl*
_tmp91A=_cycalloc(sizeof(*_tmp91A));_tmp91A->kind=(void*)_tmp90A;_tmp91A->sc=(
void*)((void*)3);_tmp91A->name=_tmp90B;_tmp91A->tvs=0;_tmp91A->impl=0;_tmp91A->attributes=
0;_tmp91A;});Cyc_Tc_tcAggrdecl(te,_tmp914,loc,_tmp915);adp=Cyc_Tcenv_lookup_aggrdecl(
te,loc,_tmp90B);*_tmp7AB=(union Cyc_Absyn_AggrInfoU_union)({union Cyc_Absyn_AggrInfoU_union
_tmp916;(_tmp916.KnownAggr).tag=1;(_tmp916.KnownAggr).f1=adp;_tmp916;});if(*
_tmp7AD != 0){({struct Cyc_String_pa_struct _tmp919;_tmp919.tag=0;_tmp919.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp90B));{
void*_tmp917[1]={& _tmp919};Cyc_Tcutil_terr(loc,({const char*_tmp918="declare parameterized type %s before using";
_tag_dyneither(_tmp918,sizeof(char),43);}),_tag_dyneither(_tmp917,sizeof(void*),
1));}});return cvtenv;}goto _LL5AA;}_LL5AD:;_LL5AE:(void)_throw(_tmp913);_LL5AA:;}}}
_tmp90D=*adp;goto _LL5A9;}_LL5A8: if((_tmp909.KnownAggr).tag != 1)goto _LL5A5;
_tmp90C=(_tmp909.KnownAggr).f1;_tmp90D=*_tmp90C;_LL5A9: {struct Cyc_List_List*tvs=
_tmp90D->tvs;struct Cyc_List_List*ts=*_tmp7AD;for(0;ts != 0  && tvs != 0;(ts=ts->tl,
tvs=tvs->tl)){void*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd);cvtenv=
Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,(void*)ts->hd,1);}if(ts != 0)({
struct Cyc_String_pa_struct _tmp91D;_tmp91D.tag=0;_tmp91D.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp90D->name));{void*_tmp91B[1]={&
_tmp91D};Cyc_Tcutil_terr(loc,({const char*_tmp91C="too many parameters for type %s";
_tag_dyneither(_tmp91C,sizeof(char),32);}),_tag_dyneither(_tmp91B,sizeof(void*),
1));}});if(tvs != 0){struct Cyc_List_List*hidden_ts=0;for(0;tvs != 0;tvs=tvs->tl){
void*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd);void*e=Cyc_Absyn_new_evar(
0,0);hidden_ts=({struct Cyc_List_List*_tmp91E=_cycalloc(sizeof(*_tmp91E));_tmp91E->hd=(
void*)e;_tmp91E->tl=hidden_ts;_tmp91E;});cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,k,e,1);}*_tmp7AD=Cyc_List_imp_append(*_tmp7AD,Cyc_List_imp_rev(
hidden_ts));}}_LL5A5:;}goto _LL4D2;_LL4F7: if(*((int*)_tmp773)!= 16)goto _LL4F9;
_tmp7AE=((struct Cyc_Absyn_TypedefType_struct*)_tmp773)->f1;_tmp7AF=((struct Cyc_Absyn_TypedefType_struct*)
_tmp773)->f2;_tmp7B0=(struct Cyc_List_List**)&((struct Cyc_Absyn_TypedefType_struct*)
_tmp773)->f2;_tmp7B1=((struct Cyc_Absyn_TypedefType_struct*)_tmp773)->f3;_tmp7B2=(
struct Cyc_Absyn_Typedefdecl**)&((struct Cyc_Absyn_TypedefType_struct*)_tmp773)->f3;
_tmp7B3=((struct Cyc_Absyn_TypedefType_struct*)_tmp773)->f4;_tmp7B4=(void***)&((
struct Cyc_Absyn_TypedefType_struct*)_tmp773)->f4;_LL4F8: {struct Cyc_List_List*
targs=*_tmp7B0;struct Cyc_Absyn_Typedefdecl*td;{struct _handler_cons _tmp91F;
_push_handler(& _tmp91F);{int _tmp921=0;if(setjmp(_tmp91F.handler))_tmp921=1;if(!
_tmp921){td=Cyc_Tcenv_lookup_typedefdecl(te,loc,_tmp7AE);;_pop_handler();}else{
void*_tmp920=(void*)_exn_thrown;void*_tmp923=_tmp920;_LL5B0: if(_tmp923 != Cyc_Dict_Absent)
goto _LL5B2;_LL5B1:({struct Cyc_String_pa_struct _tmp926;_tmp926.tag=0;_tmp926.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp7AE));{
void*_tmp924[1]={& _tmp926};Cyc_Tcutil_terr(loc,({const char*_tmp925="unbound typedef name %s";
_tag_dyneither(_tmp925,sizeof(char),24);}),_tag_dyneither(_tmp924,sizeof(void*),
1));}});return cvtenv;_LL5B2:;_LL5B3:(void)_throw(_tmp923);_LL5AF:;}}}*_tmp7B2=(
struct Cyc_Absyn_Typedefdecl*)td;_tmp7AE[0]=(td->name)[
_check_known_subscript_notnull(1,0)];{struct Cyc_List_List*tvs=td->tvs;struct Cyc_List_List*
ts=targs;struct _RegionHandle*_tmp927=Cyc_Tcenv_get_fnrgn(te);{struct Cyc_List_List*
inst=0;for(0;ts != 0  && tvs != 0;(ts=ts->tl,tvs=tvs->tl)){void*k=Cyc_Tcutil_tvar_kind((
struct Cyc_Absyn_Tvar*)tvs->hd);cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,
cvtenv,k,(void*)ts->hd,1);inst=({struct Cyc_List_List*_tmp928=_region_malloc(
_tmp927,sizeof(*_tmp928));_tmp928->hd=({struct _tuple8*_tmp929=_region_malloc(
_tmp927,sizeof(*_tmp929));_tmp929->f1=(struct Cyc_Absyn_Tvar*)tvs->hd;_tmp929->f2=(
void*)ts->hd;_tmp929;});_tmp928->tl=inst;_tmp928;});}if(ts != 0)({struct Cyc_String_pa_struct
_tmp92C;_tmp92C.tag=0;_tmp92C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp7AE));{void*_tmp92A[1]={& _tmp92C};Cyc_Tcutil_terr(
loc,({const char*_tmp92B="too many parameters for typedef %s";_tag_dyneither(
_tmp92B,sizeof(char),35);}),_tag_dyneither(_tmp92A,sizeof(void*),1));}});if(tvs
!= 0){struct Cyc_List_List*hidden_ts=0;for(0;tvs != 0;tvs=tvs->tl){void*k=Cyc_Tcutil_tvar_kind((
struct Cyc_Absyn_Tvar*)tvs->hd);void*e=Cyc_Absyn_new_evar(0,0);hidden_ts=({struct
Cyc_List_List*_tmp92D=_cycalloc(sizeof(*_tmp92D));_tmp92D->hd=(void*)e;_tmp92D->tl=
hidden_ts;_tmp92D;});cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,e,1);
inst=(struct Cyc_List_List*)({struct Cyc_List_List*_tmp92E=_cycalloc(sizeof(*
_tmp92E));_tmp92E->hd=(struct _tuple8*)({struct _tuple8*_tmp92F=_cycalloc(sizeof(*
_tmp92F));_tmp92F->f1=(struct Cyc_Absyn_Tvar*)tvs->hd;_tmp92F->f2=e;_tmp92F;});
_tmp92E->tl=inst;_tmp92E;});}*_tmp7B0=Cyc_List_imp_append(targs,Cyc_List_imp_rev(
hidden_ts));}if(td->defn != 0){void*new_typ=Cyc_Tcutil_rsubstitute(_tmp927,inst,(
void*)((struct Cyc_Core_Opt*)_check_null(td->defn))->v);*_tmp7B4=({void**_tmp930=
_cycalloc(sizeof(*_tmp930));_tmp930[0]=new_typ;_tmp930;});}}goto _LL4D2;}}_LL4F9:
if((int)_tmp773 != 3)goto _LL4FB;_LL4FA: goto _LL4FC;_LL4FB: if((int)_tmp773 != 2)goto
_LL4FD;_LL4FC: goto _LL4D2;_LL4FD: if(_tmp773 <= (void*)4)goto _LL4FF;if(*((int*)
_tmp773)!= 14)goto _LL4FF;_tmp7B5=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp773)->f1;_LL4FE: cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)5,
_tmp7B5,1);goto _LL4D2;_LL4FF: if(_tmp773 <= (void*)4)goto _LL501;if(*((int*)_tmp773)
!= 15)goto _LL501;_tmp7B6=(void*)((struct Cyc_Absyn_DynRgnType_struct*)_tmp773)->f1;
_tmp7B7=(void*)((struct Cyc_Absyn_DynRgnType_struct*)_tmp773)->f2;_LL500: cvtenv=
Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)3,_tmp7B6,0);cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,(void*)3,_tmp7B7,1);goto _LL4D2;_LL501: if(_tmp773 <= (void*)4)goto
_LL503;if(*((int*)_tmp773)!= 19)goto _LL503;_tmp7B8=(void*)((struct Cyc_Absyn_AccessEff_struct*)
_tmp773)->f1;_LL502: cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)5,
_tmp7B8,1);goto _LL4D2;_LL503: if(_tmp773 <= (void*)4)goto _LL505;if(*((int*)_tmp773)
!= 21)goto _LL505;_tmp7B9=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp773)->f1;
_LL504: cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)0,_tmp7B9,1);
goto _LL4D2;_LL505: if(_tmp773 <= (void*)4)goto _LL4D2;if(*((int*)_tmp773)!= 20)goto
_LL4D2;_tmp7BA=((struct Cyc_Absyn_JoinEff_struct*)_tmp773)->f1;_LL506: for(0;
_tmp7BA != 0;_tmp7BA=_tmp7BA->tl){cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,
cvtenv,(void*)6,(void*)_tmp7BA->hd,1);}goto _LL4D2;_LL4D2:;}if(!Cyc_Tcutil_kind_leq(
Cyc_Tcutil_typ_kind(t),expected_kind)){{void*_tmp931=t;struct Cyc_Core_Opt*
_tmp932;struct Cyc_Core_Opt*_tmp933;_LL5B5: if(_tmp931 <= (void*)4)goto _LL5B7;if(*((
int*)_tmp931)!= 0)goto _LL5B7;_tmp932=((struct Cyc_Absyn_Evar_struct*)_tmp931)->f1;
_tmp933=((struct Cyc_Absyn_Evar_struct*)_tmp931)->f2;_LL5B6: {struct
_dyneither_ptr s;{struct Cyc_Core_Opt*_tmp934=_tmp932;struct Cyc_Core_Opt _tmp935;
void*_tmp936;_LL5BA: if(_tmp934 != 0)goto _LL5BC;_LL5BB: s=({const char*_tmp937="kind=NULL ";
_tag_dyneither(_tmp937,sizeof(char),11);});goto _LL5B9;_LL5BC: if(_tmp934 == 0)goto
_LL5B9;_tmp935=*_tmp934;_tmp936=(void*)_tmp935.v;_LL5BD: s=(struct _dyneither_ptr)({
struct Cyc_String_pa_struct _tmp93A;_tmp93A.tag=0;_tmp93A.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_kind2string(_tmp936));{void*_tmp938[1]={&
_tmp93A};Cyc_aprintf(({const char*_tmp939="kind=%s ";_tag_dyneither(_tmp939,
sizeof(char),9);}),_tag_dyneither(_tmp938,sizeof(void*),1));}});goto _LL5B9;
_LL5B9:;}{struct Cyc_Core_Opt*_tmp93B=_tmp933;struct Cyc_Core_Opt _tmp93C;void*
_tmp93D;_LL5BF: if(_tmp93B != 0)goto _LL5C1;_LL5C0: s=(struct _dyneither_ptr)({struct
Cyc_String_pa_struct _tmp940;_tmp940.tag=0;_tmp940.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)s);{void*_tmp93E[1]={& _tmp940};Cyc_aprintf(({const char*
_tmp93F="%s ref=NULL";_tag_dyneither(_tmp93F,sizeof(char),12);}),_tag_dyneither(
_tmp93E,sizeof(void*),1));}});goto _LL5BE;_LL5C1: if(_tmp93B == 0)goto _LL5BE;
_tmp93C=*_tmp93B;_tmp93D=(void*)_tmp93C.v;_LL5C2: s=(struct _dyneither_ptr)({
struct Cyc_String_pa_struct _tmp944;_tmp944.tag=0;_tmp944.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp93D));{struct Cyc_String_pa_struct
_tmp943;_tmp943.tag=0;_tmp943.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s);{
void*_tmp941[2]={& _tmp943,& _tmp944};Cyc_aprintf(({const char*_tmp942="%s ref=%s";
_tag_dyneither(_tmp942,sizeof(char),10);}),_tag_dyneither(_tmp941,sizeof(void*),
2));}}});goto _LL5BE;_LL5BE:;}({struct Cyc_String_pa_struct _tmp947;_tmp947.tag=0;
_tmp947.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s);{void*_tmp945[1]={&
_tmp947};Cyc_fprintf(Cyc_stderr,({const char*_tmp946="evar info: %s\n";
_tag_dyneither(_tmp946,sizeof(char),15);}),_tag_dyneither(_tmp945,sizeof(void*),
1));}});goto _LL5B4;}_LL5B7:;_LL5B8: goto _LL5B4;_LL5B4:;}({struct Cyc_String_pa_struct
_tmp94C;_tmp94C.tag=0;_tmp94C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kind2string(expected_kind));{struct Cyc_String_pa_struct _tmp94B;
_tmp94B.tag=0;_tmp94B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(
Cyc_Tcutil_typ_kind(t)));{struct Cyc_String_pa_struct _tmp94A;_tmp94A.tag=0;
_tmp94A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));{
void*_tmp948[3]={& _tmp94A,& _tmp94B,& _tmp94C};Cyc_Tcutil_terr(loc,({const char*
_tmp949="type %s has kind %s but as used here needs kind %s";_tag_dyneither(
_tmp949,sizeof(char),51);}),_tag_dyneither(_tmp948,sizeof(void*),3));}}}});}
return cvtenv;}static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type_level_exp(
struct Cyc_Absyn_Exp*e,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv){{
void*_tmp94D=(void*)e->r;struct Cyc_List_List*_tmp94E;struct Cyc_Absyn_Exp*_tmp94F;
struct Cyc_Absyn_Exp*_tmp950;struct Cyc_Absyn_Exp*_tmp951;struct Cyc_Absyn_Exp*
_tmp952;struct Cyc_Absyn_Exp*_tmp953;struct Cyc_Absyn_Exp*_tmp954;struct Cyc_Absyn_Exp*
_tmp955;struct Cyc_Absyn_Exp*_tmp956;struct Cyc_Absyn_Exp*_tmp957;void*_tmp958;
struct Cyc_Absyn_Exp*_tmp959;void*_tmp95A;void*_tmp95B;void*_tmp95C;struct Cyc_Absyn_Exp*
_tmp95D;_LL5C4: if(*((int*)_tmp94D)!= 0)goto _LL5C6;_LL5C5: goto _LL5C7;_LL5C6: if(*((
int*)_tmp94D)!= 33)goto _LL5C8;_LL5C7: goto _LL5C9;_LL5C8: if(*((int*)_tmp94D)!= 34)
goto _LL5CA;_LL5C9: goto _LL5CB;_LL5CA: if(*((int*)_tmp94D)!= 1)goto _LL5CC;_LL5CB:
goto _LL5C3;_LL5CC: if(*((int*)_tmp94D)!= 3)goto _LL5CE;_tmp94E=((struct Cyc_Absyn_Primop_e_struct*)
_tmp94D)->f2;_LL5CD: for(0;_tmp94E != 0;_tmp94E=_tmp94E->tl){cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp((
struct Cyc_Absyn_Exp*)_tmp94E->hd,te,cvtenv);}goto _LL5C3;_LL5CE: if(*((int*)
_tmp94D)!= 6)goto _LL5D0;_tmp94F=((struct Cyc_Absyn_Conditional_e_struct*)_tmp94D)->f1;
_tmp950=((struct Cyc_Absyn_Conditional_e_struct*)_tmp94D)->f2;_tmp951=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp94D)->f3;_LL5CF: cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp94F,te,
cvtenv);cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp950,te,cvtenv);cvtenv=
Cyc_Tcutil_i_check_valid_type_level_exp(_tmp951,te,cvtenv);goto _LL5C3;_LL5D0: if(*((
int*)_tmp94D)!= 7)goto _LL5D2;_tmp952=((struct Cyc_Absyn_And_e_struct*)_tmp94D)->f1;
_tmp953=((struct Cyc_Absyn_And_e_struct*)_tmp94D)->f2;_LL5D1: _tmp954=_tmp952;
_tmp955=_tmp953;goto _LL5D3;_LL5D2: if(*((int*)_tmp94D)!= 8)goto _LL5D4;_tmp954=((
struct Cyc_Absyn_Or_e_struct*)_tmp94D)->f1;_tmp955=((struct Cyc_Absyn_Or_e_struct*)
_tmp94D)->f2;_LL5D3: _tmp956=_tmp954;_tmp957=_tmp955;goto _LL5D5;_LL5D4: if(*((int*)
_tmp94D)!= 9)goto _LL5D6;_tmp956=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp94D)->f1;
_tmp957=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp94D)->f2;_LL5D5: cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(
_tmp956,te,cvtenv);cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp957,te,
cvtenv);goto _LL5C3;_LL5D6: if(*((int*)_tmp94D)!= 15)goto _LL5D8;_tmp958=(void*)((
struct Cyc_Absyn_Cast_e_struct*)_tmp94D)->f1;_tmp959=((struct Cyc_Absyn_Cast_e_struct*)
_tmp94D)->f2;_LL5D7: cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp959,te,
cvtenv);cvtenv=Cyc_Tcutil_i_check_valid_type(e->loc,te,cvtenv,(void*)0,_tmp958,1);
goto _LL5C3;_LL5D8: if(*((int*)_tmp94D)!= 20)goto _LL5DA;_tmp95A=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)
_tmp94D)->f1;_LL5D9: _tmp95B=_tmp95A;goto _LL5DB;_LL5DA: if(*((int*)_tmp94D)!= 18)
goto _LL5DC;_tmp95B=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp94D)->f1;
_LL5DB: cvtenv=Cyc_Tcutil_i_check_valid_type(e->loc,te,cvtenv,(void*)0,_tmp95B,1);
goto _LL5C3;_LL5DC: if(*((int*)_tmp94D)!= 39)goto _LL5DE;_tmp95C=(void*)((struct Cyc_Absyn_Valueof_e_struct*)
_tmp94D)->f1;_LL5DD: cvtenv=Cyc_Tcutil_i_check_valid_type(e->loc,te,cvtenv,(void*)
7,_tmp95C,1);goto _LL5C3;_LL5DE: if(*((int*)_tmp94D)!= 19)goto _LL5E0;_tmp95D=((
struct Cyc_Absyn_Sizeofexp_e_struct*)_tmp94D)->f1;_LL5DF: cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(
_tmp95D,te,cvtenv);goto _LL5C3;_LL5E0:;_LL5E1:({void*_tmp95E=0;((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp95F="non-type-level-expression in Tcutil::i_check_valid_type_level_exp";
_tag_dyneither(_tmp95F,sizeof(char),66);}),_tag_dyneither(_tmp95E,sizeof(void*),
0));});_LL5C3:;}return cvtenv;}static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_check_valid_type(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv
cvt,void*expected_kind,void*t){struct Cyc_List_List*_tmp960=cvt.kind_env;cvt=Cyc_Tcutil_i_check_valid_type(
loc,te,cvt,expected_kind,t,1);{struct Cyc_List_List*vs=cvt.free_vars;for(0;vs != 0;
vs=vs->tl){struct Cyc_Absyn_Tvar*_tmp962;struct _tuple18 _tmp961=*((struct _tuple18*)
vs->hd);_tmp962=_tmp961.f1;cvt.kind_env=Cyc_Tcutil_fast_add_free_tvar(_tmp960,
_tmp962);}}{struct Cyc_List_List*evs=cvt.free_evars;for(0;evs != 0;evs=evs->tl){
void*_tmp964;struct _tuple19 _tmp963=*((struct _tuple19*)evs->hd);_tmp964=_tmp963.f1;{
void*_tmp965=Cyc_Tcutil_compress(_tmp964);struct Cyc_Core_Opt*_tmp966;struct Cyc_Core_Opt**
_tmp967;_LL5E3: if(_tmp965 <= (void*)4)goto _LL5E5;if(*((int*)_tmp965)!= 0)goto
_LL5E5;_tmp966=((struct Cyc_Absyn_Evar_struct*)_tmp965)->f4;_tmp967=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Evar_struct*)_tmp965)->f4;_LL5E4: if(*_tmp967 == 0)*_tmp967=({
struct Cyc_Core_Opt*_tmp968=_cycalloc(sizeof(*_tmp968));_tmp968->v=_tmp960;
_tmp968;});else{struct Cyc_List_List*_tmp969=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(*_tmp967))->v;struct Cyc_List_List*_tmp96A=0;for(0;_tmp969 != 0;
_tmp969=_tmp969->tl){if(((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,
_tmp960,(struct Cyc_Absyn_Tvar*)_tmp969->hd))_tmp96A=({struct Cyc_List_List*
_tmp96B=_cycalloc(sizeof(*_tmp96B));_tmp96B->hd=(struct Cyc_Absyn_Tvar*)_tmp969->hd;
_tmp96B->tl=_tmp96A;_tmp96B;});}*_tmp967=({struct Cyc_Core_Opt*_tmp96C=_cycalloc(
sizeof(*_tmp96C));_tmp96C->v=_tmp96A;_tmp96C;});}goto _LL5E2;_LL5E5:;_LL5E6: goto
_LL5E2;_LL5E2:;}}}return cvt;}void Cyc_Tcutil_check_valid_toplevel_type(struct Cyc_Position_Segment*
loc,struct Cyc_Tcenv_Tenv*te,void*t){int generalize_evars=Cyc_Tcutil_is_function_type(
t);struct Cyc_List_List*_tmp96D=Cyc_Tcenv_lookup_type_vars(te);struct
_RegionHandle*_tmp96E=Cyc_Tcenv_get_fnrgn(te);struct Cyc_Tcutil_CVTEnv _tmp96F=Cyc_Tcutil_check_valid_type(
loc,te,({struct Cyc_Tcutil_CVTEnv _tmp9A0;_tmp9A0.r=_tmp96E;_tmp9A0.kind_env=
_tmp96D;_tmp9A0.free_vars=0;_tmp9A0.free_evars=0;_tmp9A0.generalize_evars=
generalize_evars;_tmp9A0.fn_result=0;_tmp9A0;}),(void*)1,t);struct Cyc_List_List*
_tmp970=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Tvar*(*f)(
struct _tuple18*),struct Cyc_List_List*x))Cyc_List_rmap)(_tmp96E,(struct Cyc_Absyn_Tvar*(*)(
struct _tuple18*))Cyc_Core_fst,_tmp96F.free_vars);struct Cyc_List_List*_tmp971=((
struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _tuple19*),struct
Cyc_List_List*x))Cyc_List_rmap)(_tmp96E,(void*(*)(struct _tuple19*))Cyc_Core_fst,
_tmp96F.free_evars);if(_tmp96D != 0){struct Cyc_List_List*_tmp972=0;{struct Cyc_List_List*
_tmp973=_tmp970;for(0;(unsigned int)_tmp973;_tmp973=_tmp973->tl){struct Cyc_Absyn_Tvar*
_tmp974=(struct Cyc_Absyn_Tvar*)_tmp973->hd;int found=0;{struct Cyc_List_List*
_tmp975=_tmp96D;for(0;(unsigned int)_tmp975;_tmp975=_tmp975->tl){if(Cyc_Absyn_tvar_cmp(
_tmp974,(struct Cyc_Absyn_Tvar*)_tmp975->hd)== 0){found=1;break;}}}if(!found)
_tmp972=({struct Cyc_List_List*_tmp976=_region_malloc(_tmp96E,sizeof(*_tmp976));
_tmp976->hd=(struct Cyc_Absyn_Tvar*)_tmp973->hd;_tmp976->tl=_tmp972;_tmp976;});}}
_tmp970=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmp972);}{struct Cyc_List_List*x=_tmp970;for(0;x != 0;x=x->tl){void*_tmp977=Cyc_Absyn_compress_kb((
void*)((struct Cyc_Absyn_Tvar*)x->hd)->kind);struct Cyc_Core_Opt*_tmp978;struct Cyc_Core_Opt**
_tmp979;struct Cyc_Core_Opt*_tmp97A;struct Cyc_Core_Opt**_tmp97B;void*_tmp97C;
struct Cyc_Core_Opt*_tmp97D;struct Cyc_Core_Opt**_tmp97E;void*_tmp97F;struct Cyc_Core_Opt*
_tmp980;struct Cyc_Core_Opt**_tmp981;void*_tmp982;void*_tmp983;_LL5E8: if(*((int*)
_tmp977)!= 1)goto _LL5EA;_tmp978=((struct Cyc_Absyn_Unknown_kb_struct*)_tmp977)->f1;
_tmp979=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)_tmp977)->f1;
_LL5E9: _tmp97B=_tmp979;goto _LL5EB;_LL5EA: if(*((int*)_tmp977)!= 2)goto _LL5EC;
_tmp97A=((struct Cyc_Absyn_Less_kb_struct*)_tmp977)->f1;_tmp97B=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*)_tmp977)->f1;_tmp97C=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp977)->f2;if((int)_tmp97C != 1)goto _LL5EC;_LL5EB:*_tmp97B=Cyc_Tcutil_kind_to_bound_opt((
void*)2);goto _LL5E7;_LL5EC: if(*((int*)_tmp977)!= 2)goto _LL5EE;_tmp97D=((struct
Cyc_Absyn_Less_kb_struct*)_tmp977)->f1;_tmp97E=(struct Cyc_Core_Opt**)&((struct
Cyc_Absyn_Less_kb_struct*)_tmp977)->f1;_tmp97F=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp977)->f2;if((int)_tmp97F != 5)goto _LL5EE;_LL5ED:*_tmp97E=Cyc_Tcutil_kind_to_bound_opt((
void*)3);goto _LL5E7;_LL5EE: if(*((int*)_tmp977)!= 2)goto _LL5F0;_tmp980=((struct
Cyc_Absyn_Less_kb_struct*)_tmp977)->f1;_tmp981=(struct Cyc_Core_Opt**)&((struct
Cyc_Absyn_Less_kb_struct*)_tmp977)->f1;_tmp982=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp977)->f2;_LL5EF:*_tmp981=Cyc_Tcutil_kind_to_bound_opt(_tmp982);goto _LL5E7;
_LL5F0: if(*((int*)_tmp977)!= 0)goto _LL5F2;_tmp983=(void*)((struct Cyc_Absyn_Eq_kb_struct*)
_tmp977)->f1;if((int)_tmp983 != 1)goto _LL5F2;_LL5F1:({struct Cyc_String_pa_struct
_tmp986;_tmp986.tag=0;_tmp986.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Tcutil_tvar2string((struct Cyc_Absyn_Tvar*)x->hd));{void*_tmp984[1]={& _tmp986};
Cyc_Tcutil_terr(loc,({const char*_tmp985="type variable %s cannot have kind M";
_tag_dyneither(_tmp985,sizeof(char),36);}),_tag_dyneither(_tmp984,sizeof(void*),
1));}});goto _LL5E7;_LL5F2:;_LL5F3: goto _LL5E7;_LL5E7:;}}if(_tmp970 != 0  || _tmp971
!= 0){{void*_tmp987=Cyc_Tcutil_compress(t);struct Cyc_Absyn_FnInfo _tmp988;struct
Cyc_List_List*_tmp989;struct Cyc_List_List**_tmp98A;struct Cyc_Core_Opt*_tmp98B;
void*_tmp98C;struct Cyc_List_List*_tmp98D;int _tmp98E;struct Cyc_Absyn_VarargInfo*
_tmp98F;struct Cyc_List_List*_tmp990;struct Cyc_List_List*_tmp991;_LL5F5: if(
_tmp987 <= (void*)4)goto _LL5F7;if(*((int*)_tmp987)!= 8)goto _LL5F7;_tmp988=((
struct Cyc_Absyn_FnType_struct*)_tmp987)->f1;_tmp989=_tmp988.tvars;_tmp98A=(
struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_struct*)_tmp987)->f1).tvars;
_tmp98B=_tmp988.effect;_tmp98C=(void*)_tmp988.ret_typ;_tmp98D=_tmp988.args;
_tmp98E=_tmp988.c_varargs;_tmp98F=_tmp988.cyc_varargs;_tmp990=_tmp988.rgn_po;
_tmp991=_tmp988.attributes;_LL5F6: if(*_tmp98A == 0){*_tmp98A=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_copy)(_tmp970);_tmp970=0;}goto _LL5F4;_LL5F7:;
_LL5F8: goto _LL5F4;_LL5F4:;}if(_tmp970 != 0)({struct Cyc_String_pa_struct _tmp994;
_tmp994.tag=0;_tmp994.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct
Cyc_Absyn_Tvar*)_tmp970->hd)->name);{void*_tmp992[1]={& _tmp994};Cyc_Tcutil_terr(
loc,({const char*_tmp993="unbound type variable %s";_tag_dyneither(_tmp993,
sizeof(char),25);}),_tag_dyneither(_tmp992,sizeof(void*),1));}});if(_tmp971 != 0)
for(0;_tmp971 != 0;_tmp971=_tmp971->tl){void*e=(void*)_tmp971->hd;void*_tmp995=
Cyc_Tcutil_typ_kind(e);_LL5FA: if((int)_tmp995 != 4)goto _LL5FC;_LL5FB: if(!Cyc_Tcutil_unify(
e,(void*)3))({void*_tmp996=0;((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp997="can't unify evar with unique region!";
_tag_dyneither(_tmp997,sizeof(char),37);}),_tag_dyneither(_tmp996,sizeof(void*),
0));});goto _LL5F9;_LL5FC: if((int)_tmp995 != 5)goto _LL5FE;_LL5FD: goto _LL5FF;_LL5FE:
if((int)_tmp995 != 3)goto _LL600;_LL5FF: if(!Cyc_Tcutil_unify(e,(void*)2))({void*
_tmp998=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp999="can't unify evar with heap!";_tag_dyneither(_tmp999,sizeof(
char),28);}),_tag_dyneither(_tmp998,sizeof(void*),0));});goto _LL5F9;_LL600: if((
int)_tmp995 != 6)goto _LL602;_LL601: if(!Cyc_Tcutil_unify(e,Cyc_Absyn_empty_effect))({
void*_tmp99A=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp99B="can't unify evar with {}!";_tag_dyneither(_tmp99B,sizeof(char),
26);}),_tag_dyneither(_tmp99A,sizeof(void*),0));});goto _LL5F9;_LL602:;_LL603:({
struct Cyc_String_pa_struct _tmp99F;_tmp99F.tag=0;_tmp99F.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));{struct Cyc_String_pa_struct
_tmp99E;_tmp99E.tag=0;_tmp99E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(e));{void*_tmp99C[2]={& _tmp99E,& _tmp99F};Cyc_Tcutil_terr(
loc,({const char*_tmp99D="hidden type variable %s isn't abstracted in type %s";
_tag_dyneither(_tmp99D,sizeof(char),52);}),_tag_dyneither(_tmp99C,sizeof(void*),
2));}}});goto _LL5F9;_LL5F9:;}}}void Cyc_Tcutil_check_fndecl_valid_type(struct Cyc_Position_Segment*
loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Fndecl*fd){void*t=Cyc_Tcutil_fndecl2typ(
fd);Cyc_Tcutil_check_valid_toplevel_type(loc,te,t);{void*_tmp9A1=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_FnInfo _tmp9A2;struct Cyc_List_List*_tmp9A3;struct Cyc_Core_Opt*
_tmp9A4;void*_tmp9A5;_LL605: if(_tmp9A1 <= (void*)4)goto _LL607;if(*((int*)_tmp9A1)
!= 8)goto _LL607;_tmp9A2=((struct Cyc_Absyn_FnType_struct*)_tmp9A1)->f1;_tmp9A3=
_tmp9A2.tvars;_tmp9A4=_tmp9A2.effect;_tmp9A5=(void*)_tmp9A2.ret_typ;_LL606: fd->tvs=
_tmp9A3;fd->effect=_tmp9A4;goto _LL604;_LL607:;_LL608:({void*_tmp9A6=0;((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmp9A7="check_fndecl_valid_type: not a FnType";_tag_dyneither(_tmp9A7,sizeof(
char),38);}),_tag_dyneither(_tmp9A6,sizeof(void*),0));});_LL604:;}{struct
_RegionHandle*_tmp9A8=Cyc_Tcenv_get_fnrgn(te);Cyc_Tcutil_check_unique_vars(((
struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr*(*f)(struct
_tuple14*),struct Cyc_List_List*x))Cyc_List_rmap)(_tmp9A8,(struct _dyneither_ptr*(*)(
struct _tuple14*t))Cyc_Tcutil_fst_fdarg,fd->args),loc,({const char*_tmp9A9="function declaration has repeated parameter";
_tag_dyneither(_tmp9A9,sizeof(char),44);}));fd->cached_typ=({struct Cyc_Core_Opt*
_tmp9AA=_cycalloc(sizeof(*_tmp9AA));_tmp9AA->v=(void*)t;_tmp9AA;});}}void Cyc_Tcutil_check_type(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*
bound_tvars,void*expected_kind,int allow_evars,void*t){struct _RegionHandle*
_tmp9AB=Cyc_Tcenv_get_fnrgn(te);struct Cyc_Tcutil_CVTEnv _tmp9AC=Cyc_Tcutil_check_valid_type(
loc,te,({struct Cyc_Tcutil_CVTEnv _tmp9BF;_tmp9BF.r=_tmp9AB;_tmp9BF.kind_env=
bound_tvars;_tmp9BF.free_vars=0;_tmp9BF.free_evars=0;_tmp9BF.generalize_evars=0;
_tmp9BF.fn_result=0;_tmp9BF;}),expected_kind,t);struct Cyc_List_List*_tmp9AD=Cyc_Tcutil_remove_bound_tvars(
_tmp9AB,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Tvar*(*f)(
struct _tuple18*),struct Cyc_List_List*x))Cyc_List_rmap)(_tmp9AB,(struct Cyc_Absyn_Tvar*(*)(
struct _tuple18*))Cyc_Core_fst,_tmp9AC.free_vars),bound_tvars);struct Cyc_List_List*
_tmp9AE=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _tuple19*),
struct Cyc_List_List*x))Cyc_List_rmap)(_tmp9AB,(void*(*)(struct _tuple19*))Cyc_Core_fst,
_tmp9AC.free_evars);{struct Cyc_List_List*fs=_tmp9AD;for(0;fs != 0;fs=fs->tl){
struct _dyneither_ptr*_tmp9AF=((struct Cyc_Absyn_Tvar*)fs->hd)->name;({struct Cyc_String_pa_struct
_tmp9B3;_tmp9B3.tag=0;_tmp9B3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t));{struct Cyc_String_pa_struct _tmp9B2;_tmp9B2.tag=0;
_tmp9B2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp9AF);{void*_tmp9B0[
2]={& _tmp9B2,& _tmp9B3};Cyc_Tcutil_terr(loc,({const char*_tmp9B1="unbound type variable %s in type %s";
_tag_dyneither(_tmp9B1,sizeof(char),36);}),_tag_dyneither(_tmp9B0,sizeof(void*),
2));}}});}}if(!allow_evars  && _tmp9AE != 0)for(0;_tmp9AE != 0;_tmp9AE=_tmp9AE->tl){
void*e=(void*)_tmp9AE->hd;void*_tmp9B4=Cyc_Tcutil_typ_kind(e);_LL60A: if((int)
_tmp9B4 != 4)goto _LL60C;_LL60B: if(!Cyc_Tcutil_unify(e,(void*)3))({void*_tmp9B5=0;((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const
char*_tmp9B6="can't unify evar with unique region!";_tag_dyneither(_tmp9B6,
sizeof(char),37);}),_tag_dyneither(_tmp9B5,sizeof(void*),0));});goto _LL609;
_LL60C: if((int)_tmp9B4 != 5)goto _LL60E;_LL60D: goto _LL60F;_LL60E: if((int)_tmp9B4 != 
3)goto _LL610;_LL60F: if(!Cyc_Tcutil_unify(e,(void*)2))({void*_tmp9B7=0;((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmp9B8="can't unify evar with heap!";_tag_dyneither(_tmp9B8,sizeof(char),28);}),
_tag_dyneither(_tmp9B7,sizeof(void*),0));});goto _LL609;_LL610: if((int)_tmp9B4 != 
6)goto _LL612;_LL611: if(!Cyc_Tcutil_unify(e,Cyc_Absyn_empty_effect))({void*
_tmp9B9=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp9BA="can't unify evar with {}!";_tag_dyneither(_tmp9BA,sizeof(char),
26);}),_tag_dyneither(_tmp9B9,sizeof(void*),0));});goto _LL609;_LL612:;_LL613:({
struct Cyc_String_pa_struct _tmp9BE;_tmp9BE.tag=0;_tmp9BE.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));{struct Cyc_String_pa_struct
_tmp9BD;_tmp9BD.tag=0;_tmp9BD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(e));{void*_tmp9BB[2]={& _tmp9BD,& _tmp9BE};Cyc_Tcutil_terr(
loc,({const char*_tmp9BC="hidden type variable %s isn't abstracted in type %s";
_tag_dyneither(_tmp9BC,sizeof(char),52);}),_tag_dyneither(_tmp9BB,sizeof(void*),
2));}}});goto _LL609;_LL609:;}}void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*
tv){if(tv->identity == - 1)tv->identity=Cyc_Tcutil_new_tvar_id();}void Cyc_Tcutil_add_tvar_identities(
struct Cyc_List_List*tvs){((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*
x))Cyc_List_iter)(Cyc_Tcutil_add_tvar_identity,tvs);}static void Cyc_Tcutil_check_unique_unsorted(
int(*cmp)(void*,void*),struct Cyc_List_List*vs,struct Cyc_Position_Segment*loc,
struct _dyneither_ptr(*a2string)(void*),struct _dyneither_ptr msg){for(0;vs != 0;vs=
vs->tl){struct Cyc_List_List*vs2=vs->tl;for(0;vs2 != 0;vs2=vs2->tl){if(cmp((void*)
vs->hd,(void*)vs2->hd)== 0)({struct Cyc_String_pa_struct _tmp9C3;_tmp9C3.tag=0;
_tmp9C3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)a2string((void*)vs->hd));{
struct Cyc_String_pa_struct _tmp9C2;_tmp9C2.tag=0;_tmp9C2.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)msg);{void*_tmp9C0[2]={& _tmp9C2,& _tmp9C3};Cyc_Tcutil_terr(
loc,({const char*_tmp9C1="%s: %s";_tag_dyneither(_tmp9C1,sizeof(char),7);}),
_tag_dyneither(_tmp9C0,sizeof(void*),2));}}});}}}static struct _dyneither_ptr Cyc_Tcutil_strptr2string(
struct _dyneither_ptr*s){return*s;}void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*
vs,struct Cyc_Position_Segment*loc,struct _dyneither_ptr msg){((void(*)(int(*cmp)(
struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*vs,struct Cyc_Position_Segment*
loc,struct _dyneither_ptr(*a2string)(struct _dyneither_ptr*),struct _dyneither_ptr
msg))Cyc_Tcutil_check_unique_unsorted)(Cyc_strptrcmp,vs,loc,Cyc_Tcutil_strptr2string,
msg);}void Cyc_Tcutil_check_unique_tvars(struct Cyc_Position_Segment*loc,struct Cyc_List_List*
tvs){((void(*)(int(*cmp)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*
vs,struct Cyc_Position_Segment*loc,struct _dyneither_ptr(*a2string)(struct Cyc_Absyn_Tvar*),
struct _dyneither_ptr msg))Cyc_Tcutil_check_unique_unsorted)(Cyc_Absyn_tvar_cmp,
tvs,loc,Cyc_Tcutil_tvar2string,({const char*_tmp9C4="duplicate type variable";
_tag_dyneither(_tmp9C4,sizeof(char),24);}));}struct _tuple20{struct Cyc_Absyn_Aggrfield*
f1;int f2;};struct _tuple21{struct Cyc_List_List*f1;void*f2;};struct _tuple22{struct
Cyc_Absyn_Aggrfield*f1;void*f2;};struct Cyc_List_List*Cyc_Tcutil_resolve_struct_designators(
struct _RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,
struct Cyc_List_List*sdfields){struct Cyc_List_List*fields=0;{struct Cyc_List_List*
sd_fields=sdfields;for(0;sd_fields != 0;sd_fields=sd_fields->tl){if(Cyc_strcmp((
struct _dyneither_ptr)*((struct Cyc_Absyn_Aggrfield*)sd_fields->hd)->name,({const
char*_tmp9C5="";_tag_dyneither(_tmp9C5,sizeof(char),1);}))!= 0)fields=({struct
Cyc_List_List*_tmp9C6=_cycalloc(sizeof(*_tmp9C6));_tmp9C6->hd=({struct _tuple20*
_tmp9C7=_cycalloc(sizeof(*_tmp9C7));_tmp9C7->f1=(struct Cyc_Absyn_Aggrfield*)
sd_fields->hd;_tmp9C7->f2=0;_tmp9C7;});_tmp9C6->tl=fields;_tmp9C6;});}}fields=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields);{struct
Cyc_List_List*ans=0;for(0;des != 0;des=des->tl){struct _tuple21 _tmp9C9;struct Cyc_List_List*
_tmp9CA;void*_tmp9CB;struct _tuple21*_tmp9C8=(struct _tuple21*)des->hd;_tmp9C9=*
_tmp9C8;_tmp9CA=_tmp9C9.f1;_tmp9CB=_tmp9C9.f2;if(_tmp9CA == 0){struct Cyc_List_List*
_tmp9CC=fields;for(0;_tmp9CC != 0;_tmp9CC=_tmp9CC->tl){if(!(*((struct _tuple20*)
_tmp9CC->hd)).f2){(*((struct _tuple20*)_tmp9CC->hd)).f2=1;(*((struct _tuple21*)des->hd)).f1=(
struct Cyc_List_List*)({struct Cyc_List_List*_tmp9CD=_cycalloc(sizeof(*_tmp9CD));
_tmp9CD->hd=(void*)((void*)({struct Cyc_Absyn_FieldName_struct*_tmp9CE=_cycalloc(
sizeof(*_tmp9CE));_tmp9CE[0]=({struct Cyc_Absyn_FieldName_struct _tmp9CF;_tmp9CF.tag=
1;_tmp9CF.f1=((*((struct _tuple20*)_tmp9CC->hd)).f1)->name;_tmp9CF;});_tmp9CE;}));
_tmp9CD->tl=0;_tmp9CD;});ans=({struct Cyc_List_List*_tmp9D0=_region_malloc(rgn,
sizeof(*_tmp9D0));_tmp9D0->hd=({struct _tuple22*_tmp9D1=_region_malloc(rgn,
sizeof(*_tmp9D1));_tmp9D1->f1=(*((struct _tuple20*)_tmp9CC->hd)).f1;_tmp9D1->f2=
_tmp9CB;_tmp9D1;});_tmp9D0->tl=ans;_tmp9D0;});break;}}if(_tmp9CC == 0)({void*
_tmp9D2=0;Cyc_Tcutil_terr(loc,({const char*_tmp9D3="too many arguments to struct";
_tag_dyneither(_tmp9D3,sizeof(char),29);}),_tag_dyneither(_tmp9D2,sizeof(void*),
0));});}else{if(_tmp9CA->tl != 0)({void*_tmp9D4=0;Cyc_Tcutil_terr(loc,({const char*
_tmp9D5="multiple designators are not supported";_tag_dyneither(_tmp9D5,sizeof(
char),39);}),_tag_dyneither(_tmp9D4,sizeof(void*),0));});else{void*_tmp9D6=(void*)
_tmp9CA->hd;struct _dyneither_ptr*_tmp9D7;_LL615: if(*((int*)_tmp9D6)!= 0)goto
_LL617;_LL616:({void*_tmp9D8=0;Cyc_Tcutil_terr(loc,({const char*_tmp9D9="array designator used in argument to struct";
_tag_dyneither(_tmp9D9,sizeof(char),44);}),_tag_dyneither(_tmp9D8,sizeof(void*),
0));});goto _LL614;_LL617: if(*((int*)_tmp9D6)!= 1)goto _LL614;_tmp9D7=((struct Cyc_Absyn_FieldName_struct*)
_tmp9D6)->f1;_LL618: {struct Cyc_List_List*_tmp9DA=fields;for(0;_tmp9DA != 0;
_tmp9DA=_tmp9DA->tl){if(Cyc_strptrcmp(_tmp9D7,((*((struct _tuple20*)_tmp9DA->hd)).f1)->name)
== 0){if((*((struct _tuple20*)_tmp9DA->hd)).f2)({struct Cyc_String_pa_struct
_tmp9DD;_tmp9DD.tag=0;_tmp9DD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*
_tmp9D7);{void*_tmp9DB[1]={& _tmp9DD};Cyc_Tcutil_terr(loc,({const char*_tmp9DC="field %s has already been used as an argument";
_tag_dyneither(_tmp9DC,sizeof(char),46);}),_tag_dyneither(_tmp9DB,sizeof(void*),
1));}});(*((struct _tuple20*)_tmp9DA->hd)).f2=1;ans=({struct Cyc_List_List*_tmp9DE=
_region_malloc(rgn,sizeof(*_tmp9DE));_tmp9DE->hd=({struct _tuple22*_tmp9DF=
_region_malloc(rgn,sizeof(*_tmp9DF));_tmp9DF->f1=(*((struct _tuple20*)_tmp9DA->hd)).f1;
_tmp9DF->f2=_tmp9CB;_tmp9DF;});_tmp9DE->tl=ans;_tmp9DE;});break;}}if(_tmp9DA == 0)({
struct Cyc_String_pa_struct _tmp9E2;_tmp9E2.tag=0;_tmp9E2.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)*_tmp9D7);{void*_tmp9E0[1]={& _tmp9E2};Cyc_Tcutil_terr(loc,({
const char*_tmp9E1="bad field designator %s";_tag_dyneither(_tmp9E1,sizeof(char),
24);}),_tag_dyneither(_tmp9E0,sizeof(void*),1));}});goto _LL614;}_LL614:;}}}for(0;
fields != 0;fields=fields->tl){if(!(*((struct _tuple20*)fields->hd)).f2){({void*
_tmp9E3=0;Cyc_Tcutil_terr(loc,({const char*_tmp9E4="too few arguments to struct";
_tag_dyneither(_tmp9E4,sizeof(char),28);}),_tag_dyneither(_tmp9E3,sizeof(void*),
0));});break;}}return((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
ans);}}int Cyc_Tcutil_is_tagged_pointer_typ_elt(void*t,void**elt_typ_dest){void*
_tmp9E5=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp9E6;void*_tmp9E7;
struct Cyc_Absyn_PtrAtts _tmp9E8;struct Cyc_Absyn_Conref*_tmp9E9;_LL61A: if(_tmp9E5
<= (void*)4)goto _LL61C;if(*((int*)_tmp9E5)!= 4)goto _LL61C;_tmp9E6=((struct Cyc_Absyn_PointerType_struct*)
_tmp9E5)->f1;_tmp9E7=(void*)_tmp9E6.elt_typ;_tmp9E8=_tmp9E6.ptr_atts;_tmp9E9=
_tmp9E8.bounds;_LL61B: {struct Cyc_Absyn_Conref*_tmp9EA=Cyc_Absyn_compress_conref(
_tmp9E9);union Cyc_Absyn_Constraint_union _tmp9EB=(Cyc_Absyn_compress_conref(
_tmp9EA))->v;void*_tmp9EC;_LL61F: if((_tmp9EB.Eq_constr).tag != 0)goto _LL621;
_tmp9EC=(_tmp9EB.Eq_constr).f1;if((int)_tmp9EC != 0)goto _LL621;_LL620:*
elt_typ_dest=_tmp9E7;return 1;_LL621: if((_tmp9EB.No_constr).tag != 2)goto _LL623;
_LL622: _tmp9EA->v=(union Cyc_Absyn_Constraint_union)({union Cyc_Absyn_Constraint_union
_tmp9ED;(_tmp9ED.Eq_constr).tag=0;(_tmp9ED.Eq_constr).f1=(void*)((void*)0);
_tmp9ED;});*elt_typ_dest=_tmp9E7;return 1;_LL623:;_LL624: return 0;_LL61E:;}_LL61C:;
_LL61D: return 0;_LL619:;}int Cyc_Tcutil_is_zero_pointer_typ_elt(void*t,void**
elt_typ_dest){void*_tmp9EE=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp9EF;
void*_tmp9F0;struct Cyc_Absyn_PtrAtts _tmp9F1;struct Cyc_Absyn_Conref*_tmp9F2;
_LL626: if(_tmp9EE <= (void*)4)goto _LL628;if(*((int*)_tmp9EE)!= 4)goto _LL628;
_tmp9EF=((struct Cyc_Absyn_PointerType_struct*)_tmp9EE)->f1;_tmp9F0=(void*)
_tmp9EF.elt_typ;_tmp9F1=_tmp9EF.ptr_atts;_tmp9F2=_tmp9F1.zero_term;_LL627:*
elt_typ_dest=_tmp9F0;return((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,_tmp9F2);_LL628:;_LL629: return 0;_LL625:;}int Cyc_Tcutil_is_zero_ptr_type(void*t,
void**ptr_type,int*is_dyneither,void**elt_type){void*_tmp9F3=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_PtrInfo _tmp9F4;void*_tmp9F5;struct Cyc_Absyn_PtrAtts _tmp9F6;
struct Cyc_Absyn_Conref*_tmp9F7;struct Cyc_Absyn_Conref*_tmp9F8;struct Cyc_Absyn_ArrayInfo
_tmp9F9;void*_tmp9FA;struct Cyc_Absyn_Tqual _tmp9FB;struct Cyc_Absyn_Exp*_tmp9FC;
struct Cyc_Absyn_Conref*_tmp9FD;_LL62B: if(_tmp9F3 <= (void*)4)goto _LL62F;if(*((int*)
_tmp9F3)!= 4)goto _LL62D;_tmp9F4=((struct Cyc_Absyn_PointerType_struct*)_tmp9F3)->f1;
_tmp9F5=(void*)_tmp9F4.elt_typ;_tmp9F6=_tmp9F4.ptr_atts;_tmp9F7=_tmp9F6.bounds;
_tmp9F8=_tmp9F6.zero_term;_LL62C: if(((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,_tmp9F8)){*ptr_type=t;*elt_type=_tmp9F5;{void*_tmp9FE=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,
_tmp9F7);_LL632: if((int)_tmp9FE != 0)goto _LL634;_LL633:*is_dyneither=1;goto _LL631;
_LL634:;_LL635:*is_dyneither=0;goto _LL631;_LL631:;}return 1;}else{return 0;}_LL62D:
if(*((int*)_tmp9F3)!= 7)goto _LL62F;_tmp9F9=((struct Cyc_Absyn_ArrayType_struct*)
_tmp9F3)->f1;_tmp9FA=(void*)_tmp9F9.elt_type;_tmp9FB=_tmp9F9.tq;_tmp9FC=_tmp9F9.num_elts;
_tmp9FD=_tmp9F9.zero_term;_LL62E: if(((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,_tmp9FD)){*elt_type=_tmp9FA;*is_dyneither=0;*ptr_type=(void*)({struct Cyc_Absyn_PointerType_struct*
_tmp9FF=_cycalloc(sizeof(*_tmp9FF));_tmp9FF[0]=({struct Cyc_Absyn_PointerType_struct
_tmpA00;_tmpA00.tag=4;_tmpA00.f1=({struct Cyc_Absyn_PtrInfo _tmpA01;_tmpA01.elt_typ=(
void*)_tmp9FA;_tmpA01.elt_tq=_tmp9FB;_tmpA01.ptr_atts=({struct Cyc_Absyn_PtrAtts
_tmpA02;_tmpA02.rgn=(void*)((void*)2);_tmpA02.nullable=Cyc_Absyn_false_conref;
_tmpA02.bounds=Cyc_Absyn_new_conref((void*)({struct Cyc_Absyn_Upper_b_struct*
_tmpA03=_cycalloc(sizeof(*_tmpA03));_tmpA03[0]=({struct Cyc_Absyn_Upper_b_struct
_tmpA04;_tmpA04.tag=0;_tmpA04.f1=(struct Cyc_Absyn_Exp*)_check_null(_tmp9FC);
_tmpA04;});_tmpA03;}));_tmpA02.zero_term=_tmp9FD;_tmpA02.ptrloc=0;_tmpA02;});
_tmpA01;});_tmpA00;});_tmp9FF;});return 1;}else{return 0;}_LL62F:;_LL630: return 0;
_LL62A:;}int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*e1,void**ptr_type,
int*is_dyneither,void**elt_type){void*_tmpA05=(void*)e1->r;struct Cyc_Absyn_Exp*
_tmpA06;struct Cyc_Absyn_Exp*_tmpA07;struct Cyc_Absyn_Exp*_tmpA08;struct Cyc_Absyn_Exp*
_tmpA09;struct Cyc_Absyn_Exp*_tmpA0A;struct Cyc_Absyn_Exp*_tmpA0B;_LL637: if(*((int*)
_tmpA05)!= 15)goto _LL639;_LL638:({struct Cyc_String_pa_struct _tmpA0E;_tmpA0E.tag=
0;_tmpA0E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(
e1));{void*_tmpA0C[1]={& _tmpA0E};((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmpA0D="we have a cast in a lhs:  %s";
_tag_dyneither(_tmpA0D,sizeof(char),29);}),_tag_dyneither(_tmpA0C,sizeof(void*),
1));}});_LL639: if(*((int*)_tmpA05)!= 22)goto _LL63B;_tmpA06=((struct Cyc_Absyn_Deref_e_struct*)
_tmpA05)->f1;_LL63A: _tmpA07=_tmpA06;goto _LL63C;_LL63B: if(*((int*)_tmpA05)!= 25)
goto _LL63D;_tmpA07=((struct Cyc_Absyn_Subscript_e_struct*)_tmpA05)->f1;_LL63C:
return Cyc_Tcutil_is_zero_ptr_type((void*)((struct Cyc_Core_Opt*)_check_null(
_tmpA07->topt))->v,ptr_type,is_dyneither,elt_type);_LL63D: if(*((int*)_tmpA05)!= 
24)goto _LL63F;_tmpA08=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmpA05)->f1;_LL63E:
_tmpA09=_tmpA08;goto _LL640;_LL63F: if(*((int*)_tmpA05)!= 23)goto _LL641;_tmpA09=((
struct Cyc_Absyn_AggrMember_e_struct*)_tmpA05)->f1;_LL640: if(Cyc_Tcutil_is_zero_ptr_type((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpA09->topt))->v,ptr_type,is_dyneither,
elt_type))({struct Cyc_String_pa_struct _tmpA11;_tmpA11.tag=0;_tmpA11.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e1));{void*_tmpA0F[
1]={& _tmpA11};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmpA10="found zero pointer aggregate member assignment: %s";
_tag_dyneither(_tmpA10,sizeof(char),51);}),_tag_dyneither(_tmpA0F,sizeof(void*),
1));}});return 0;_LL641: if(*((int*)_tmpA05)!= 14)goto _LL643;_tmpA0A=((struct Cyc_Absyn_Instantiate_e_struct*)
_tmpA05)->f1;_LL642: _tmpA0B=_tmpA0A;goto _LL644;_LL643: if(*((int*)_tmpA05)!= 13)
goto _LL645;_tmpA0B=((struct Cyc_Absyn_NoInstantiate_e_struct*)_tmpA05)->f1;_LL644:
if(Cyc_Tcutil_is_zero_ptr_type((void*)((struct Cyc_Core_Opt*)_check_null(_tmpA0B->topt))->v,
ptr_type,is_dyneither,elt_type))({struct Cyc_String_pa_struct _tmpA14;_tmpA14.tag=
0;_tmpA14.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(
e1));{void*_tmpA12[1]={& _tmpA14};((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmpA13="found zero pointer instantiate/noinstantiate: %s";
_tag_dyneither(_tmpA13,sizeof(char),49);}),_tag_dyneither(_tmpA12,sizeof(void*),
1));}});return 0;_LL645: if(*((int*)_tmpA05)!= 1)goto _LL647;_LL646: return 0;_LL647:;
_LL648:({struct Cyc_String_pa_struct _tmpA17;_tmpA17.tag=0;_tmpA17.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e1));{void*_tmpA15[
1]={& _tmpA17};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmpA16="found bad lhs in is_zero_ptr_deref: %s";_tag_dyneither(
_tmpA16,sizeof(char),39);}),_tag_dyneither(_tmpA15,sizeof(void*),1));}});_LL636:;}
int Cyc_Tcutil_is_tagged_pointer_typ(void*t){void*ignore=(void*)0;return Cyc_Tcutil_is_tagged_pointer_typ_elt(
t,& ignore);}static int Cyc_Tcutil_is_noalias_region(void*r){void*_tmpA18=Cyc_Tcutil_compress(
r);struct Cyc_Absyn_Tvar*_tmpA19;_LL64A: if((int)_tmpA18 != 3)goto _LL64C;_LL64B:
return 1;_LL64C: if(_tmpA18 <= (void*)4)goto _LL64E;if(*((int*)_tmpA18)!= 1)goto
_LL64E;_tmpA19=((struct Cyc_Absyn_VarType_struct*)_tmpA18)->f1;_LL64D: return Cyc_Tcutil_tvar_kind(
_tmpA19)== (void*)4  || Cyc_Tcutil_tvar_kind(_tmpA19)== (void*)5;_LL64E:;_LL64F:
return 0;_LL649:;}int Cyc_Tcutil_is_noalias_pointer(void*t){void*_tmpA1A=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_PtrInfo _tmpA1B;struct Cyc_Absyn_PtrAtts _tmpA1C;void*_tmpA1D;
_LL651: if(_tmpA1A <= (void*)4)goto _LL653;if(*((int*)_tmpA1A)!= 4)goto _LL653;
_tmpA1B=((struct Cyc_Absyn_PointerType_struct*)_tmpA1A)->f1;_tmpA1C=_tmpA1B.ptr_atts;
_tmpA1D=(void*)_tmpA1C.rgn;_LL652: return Cyc_Tcutil_is_noalias_region(_tmpA1D);
_LL653:;_LL654: return 0;_LL650:;}int Cyc_Tcutil_is_noalias_pointer_or_aggr(struct
_RegionHandle*rgn,void*t){void*_tmpA1E=Cyc_Tcutil_compress(t);if(Cyc_Tcutil_is_noalias_pointer(
_tmpA1E))return 1;{void*_tmpA1F=_tmpA1E;struct Cyc_List_List*_tmpA20;struct Cyc_Absyn_AggrInfo
_tmpA21;union Cyc_Absyn_AggrInfoU_union _tmpA22;struct Cyc_Absyn_Aggrdecl**_tmpA23;
struct Cyc_List_List*_tmpA24;struct Cyc_List_List*_tmpA25;struct Cyc_Absyn_AggrInfo
_tmpA26;union Cyc_Absyn_AggrInfoU_union _tmpA27;struct Cyc_Absyn_TunionInfo _tmpA28;
union Cyc_Absyn_TunionInfoU_union _tmpA29;struct Cyc_List_List*_tmpA2A;struct Cyc_Core_Opt*
_tmpA2B;struct Cyc_Absyn_TunionFieldInfo _tmpA2C;union Cyc_Absyn_TunionFieldInfoU_union
_tmpA2D;struct Cyc_List_List*_tmpA2E;_LL656: if(_tmpA1F <= (void*)4)goto _LL662;if(*((
int*)_tmpA1F)!= 9)goto _LL658;_tmpA20=((struct Cyc_Absyn_TupleType_struct*)_tmpA1F)->f1;
_LL657: while(_tmpA20 != 0){if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,(*((
struct _tuple5*)_tmpA20->hd)).f2))return 1;_tmpA20=_tmpA20->tl;}return 0;_LL658: if(*((
int*)_tmpA1F)!= 10)goto _LL65A;_tmpA21=((struct Cyc_Absyn_AggrType_struct*)_tmpA1F)->f1;
_tmpA22=_tmpA21.aggr_info;if((((((struct Cyc_Absyn_AggrType_struct*)_tmpA1F)->f1).aggr_info).KnownAggr).tag
!= 1)goto _LL65A;_tmpA23=(_tmpA22.KnownAggr).f1;_tmpA24=_tmpA21.targs;_LL659: if((*
_tmpA23)->impl == 0)return 0;else{struct Cyc_List_List*_tmpA2F=((struct Cyc_List_List*(*)(
struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(rgn,rgn,(*_tmpA23)->tvs,_tmpA24);struct Cyc_List_List*_tmpA30=((
struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmpA23)->impl))->fields;void*t;
while(_tmpA30 != 0){t=Cyc_Tcutil_rsubstitute(rgn,_tmpA2F,(void*)((struct Cyc_Absyn_Aggrfield*)
_tmpA30->hd)->type);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,t))return 1;
_tmpA30=_tmpA30->tl;}return 0;}_LL65A: if(*((int*)_tmpA1F)!= 11)goto _LL65C;_tmpA25=((
struct Cyc_Absyn_AnonAggrType_struct*)_tmpA1F)->f2;_LL65B: while(_tmpA25 != 0){if(
Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,(void*)((struct Cyc_Absyn_Aggrfield*)
_tmpA25->hd)->type))return 1;_tmpA25=_tmpA25->tl;}return 0;_LL65C: if(*((int*)
_tmpA1F)!= 10)goto _LL65E;_tmpA26=((struct Cyc_Absyn_AggrType_struct*)_tmpA1F)->f1;
_tmpA27=_tmpA26.aggr_info;if((((((struct Cyc_Absyn_AggrType_struct*)_tmpA1F)->f1).aggr_info).UnknownAggr).tag
!= 0)goto _LL65E;_LL65D:({void*_tmpA31=0;((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmpA32="got unknown aggr in is_noalias_aggr";
_tag_dyneither(_tmpA32,sizeof(char),36);}),_tag_dyneither(_tmpA31,sizeof(void*),
0));});_LL65E: if(*((int*)_tmpA1F)!= 2)goto _LL660;_tmpA28=((struct Cyc_Absyn_TunionType_struct*)
_tmpA1F)->f1;_tmpA29=_tmpA28.tunion_info;_tmpA2A=_tmpA28.targs;_tmpA2B=_tmpA28.rgn;
_LL65F: {union Cyc_Absyn_TunionInfoU_union _tmpA33=_tmpA29;struct Cyc_Absyn_UnknownTunionInfo
_tmpA34;struct _tuple2*_tmpA35;int _tmpA36;int _tmpA37;struct Cyc_Absyn_Tuniondecl**
_tmpA38;struct Cyc_Absyn_Tuniondecl*_tmpA39;struct Cyc_Absyn_Tuniondecl _tmpA3A;
struct Cyc_List_List*_tmpA3B;struct Cyc_Core_Opt*_tmpA3C;int _tmpA3D;_LL665: if((
_tmpA33.UnknownTunion).tag != 0)goto _LL667;_tmpA34=(_tmpA33.UnknownTunion).f1;
_tmpA35=_tmpA34.name;_tmpA36=_tmpA34.is_xtunion;_tmpA37=_tmpA34.is_flat;_LL666:({
void*_tmpA3E=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmpA3F="got unknown tunion in is_noalias_aggr";_tag_dyneither(_tmpA3F,
sizeof(char),38);}),_tag_dyneither(_tmpA3E,sizeof(void*),0));});_LL667: if((
_tmpA33.KnownTunion).tag != 1)goto _LL664;_tmpA38=(_tmpA33.KnownTunion).f1;_tmpA39=*
_tmpA38;_tmpA3A=*_tmpA39;_tmpA3B=_tmpA3A.tvs;_tmpA3C=_tmpA3A.fields;_tmpA3D=
_tmpA3A.is_flat;_LL668: if(!_tmpA3D){if(_tmpA2B == 0)return 0;return Cyc_Tcutil_is_noalias_region((
void*)_tmpA2B->v);}else{if(_tmpA3C == 0)({void*_tmpA40=0;((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmpA41="got tunion with no fields";
_tag_dyneither(_tmpA41,sizeof(char),26);}),_tag_dyneither(_tmpA40,sizeof(void*),
0));});else{struct Cyc_List_List*_tmpA42=((struct Cyc_List_List*(*)(struct
_RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(rgn,rgn,_tmpA3B,_tmpA2A);struct Cyc_List_List*_tmpA43=(struct
Cyc_List_List*)_tmpA3C->v;while(_tmpA43 != 0){struct Cyc_List_List*_tmpA44=((
struct Cyc_Absyn_Tunionfield*)_tmpA43->hd)->typs;while(_tmpA44 != 0){_tmpA1E=Cyc_Tcutil_rsubstitute(
rgn,_tmpA42,(*((struct _tuple5*)_tmpA44->hd)).f2);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(
rgn,_tmpA1E))return 1;_tmpA44=_tmpA44->tl;}_tmpA43=_tmpA43->tl;}return 0;}}_LL664:;}
_LL660: if(*((int*)_tmpA1F)!= 3)goto _LL662;_tmpA2C=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmpA1F)->f1;_tmpA2D=_tmpA2C.field_info;_tmpA2E=_tmpA2C.targs;_LL661: {union Cyc_Absyn_TunionFieldInfoU_union
_tmpA45=_tmpA2D;struct Cyc_Absyn_Tuniondecl*_tmpA46;struct Cyc_Absyn_Tunionfield*
_tmpA47;_LL66A: if((_tmpA45.UnknownTunionfield).tag != 0)goto _LL66C;_LL66B:({void*
_tmpA48=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmpA49="got unknown tunion field in is_noalias_aggr";_tag_dyneither(
_tmpA49,sizeof(char),44);}),_tag_dyneither(_tmpA48,sizeof(void*),0));});_LL66C:
if((_tmpA45.KnownTunionfield).tag != 1)goto _LL669;_tmpA46=(_tmpA45.KnownTunionfield).f1;
_tmpA47=(_tmpA45.KnownTunionfield).f2;_LL66D: {struct Cyc_List_List*_tmpA4A=((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmpA46->tvs,_tmpA2E);struct Cyc_List_List*
_tmpA4B=_tmpA47->typs;while(_tmpA4B != 0){_tmpA1E=Cyc_Tcutil_rsubstitute(rgn,
_tmpA4A,(*((struct _tuple5*)_tmpA4B->hd)).f2);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(
rgn,_tmpA1E))return 1;_tmpA4B=_tmpA4B->tl;}return 0;}_LL669:;}_LL662:;_LL663:
return 0;_LL655:;}}static int Cyc_Tcutil_is_noalias_field(struct _RegionHandle*r,
void*t,struct _dyneither_ptr*f){void*_tmpA4C=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo
_tmpA4D;union Cyc_Absyn_AggrInfoU_union _tmpA4E;struct Cyc_Absyn_Aggrdecl**_tmpA4F;
struct Cyc_Absyn_Aggrdecl*_tmpA50;struct Cyc_List_List*_tmpA51;struct Cyc_List_List*
_tmpA52;_LL66F: if(_tmpA4C <= (void*)4)goto _LL673;if(*((int*)_tmpA4C)!= 10)goto
_LL671;_tmpA4D=((struct Cyc_Absyn_AggrType_struct*)_tmpA4C)->f1;_tmpA4E=_tmpA4D.aggr_info;
if((((((struct Cyc_Absyn_AggrType_struct*)_tmpA4C)->f1).aggr_info).KnownAggr).tag
!= 1)goto _LL671;_tmpA4F=(_tmpA4E.KnownAggr).f1;_tmpA50=*_tmpA4F;_tmpA51=_tmpA4D.targs;
_LL670: _tmpA52=_tmpA50->impl == 0?0:((struct Cyc_Absyn_AggrdeclImpl*)_check_null(
_tmpA50->impl))->fields;goto _LL672;_LL671: if(*((int*)_tmpA4C)!= 11)goto _LL673;
_tmpA52=((struct Cyc_Absyn_AnonAggrType_struct*)_tmpA4C)->f2;_LL672: {struct Cyc_Absyn_Aggrfield*
_tmpA53=Cyc_Absyn_lookup_field(_tmpA52,f);{struct Cyc_Absyn_Aggrfield*_tmpA54=
_tmpA53;struct Cyc_Absyn_Aggrfield _tmpA55;void*_tmpA56;_LL676: if(_tmpA54 != 0)goto
_LL678;_LL677:({void*_tmpA57=0;((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmpA58="is_noalias_field: missing field";
_tag_dyneither(_tmpA58,sizeof(char),32);}),_tag_dyneither(_tmpA57,sizeof(void*),
0));});_LL678: if(_tmpA54 == 0)goto _LL675;_tmpA55=*_tmpA54;_tmpA56=(void*)_tmpA55.type;
_LL679: return Cyc_Tcutil_is_noalias_pointer_or_aggr(r,_tmpA56);_LL675:;}return 0;}
_LL673:;_LL674:({struct Cyc_String_pa_struct _tmpA5B;_tmpA5B.tag=0;_tmpA5B.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));{void*
_tmpA59[1]={& _tmpA5B};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(({const char*_tmpA5A="is_noalias_field: invalid type |%s|";
_tag_dyneither(_tmpA5A,sizeof(char),36);}),_tag_dyneither(_tmpA59,sizeof(void*),
1));}});_LL66E:;}static int Cyc_Tcutil_is_noalias_path_aux(struct _RegionHandle*r,
struct Cyc_Absyn_Exp*e,int ignore_leaf){void*_tmpA5C=(void*)e->r;void*_tmpA5D;
struct Cyc_Absyn_Exp*_tmpA5E;struct _dyneither_ptr*_tmpA5F;struct Cyc_Absyn_Exp*
_tmpA60;struct Cyc_Absyn_Exp*_tmpA61;void*_tmpA62;void*_tmpA63;void*_tmpA64;
struct Cyc_Absyn_Exp*_tmpA65;struct Cyc_Absyn_Exp*_tmpA66;struct Cyc_Absyn_Exp*
_tmpA67;struct Cyc_Absyn_Exp*_tmpA68;void*_tmpA69;struct Cyc_Absyn_Exp*_tmpA6A;
struct Cyc_Absyn_Stmt*_tmpA6B;_LL67B: if(*((int*)_tmpA5C)!= 5)goto _LL67D;_LL67C:
goto _LL67E;_LL67D: if(*((int*)_tmpA5C)!= 7)goto _LL67F;_LL67E: goto _LL680;_LL67F:
if(*((int*)_tmpA5C)!= 8)goto _LL681;_LL680: goto _LL682;_LL681: if(*((int*)_tmpA5C)
!= 12)goto _LL683;_LL682: goto _LL684;_LL683: if(*((int*)_tmpA5C)!= 16)goto _LL685;
_LL684: goto _LL686;_LL685: if(*((int*)_tmpA5C)!= 18)goto _LL687;_LL686: goto _LL688;
_LL687: if(*((int*)_tmpA5C)!= 19)goto _LL689;_LL688: goto _LL68A;_LL689: if(*((int*)
_tmpA5C)!= 20)goto _LL68B;_LL68A: goto _LL68C;_LL68B: if(*((int*)_tmpA5C)!= 21)goto
_LL68D;_LL68C: goto _LL68E;_LL68D: if(*((int*)_tmpA5C)!= 27)goto _LL68F;_LL68E: goto
_LL690;_LL68F: if(*((int*)_tmpA5C)!= 29)goto _LL691;_LL690: goto _LL692;_LL691: if(*((
int*)_tmpA5C)!= 28)goto _LL693;_LL692: goto _LL694;_LL693: if(*((int*)_tmpA5C)!= 33)
goto _LL695;_LL694: goto _LL696;_LL695: if(*((int*)_tmpA5C)!= 34)goto _LL697;_LL696:
goto _LL698;_LL697: if(*((int*)_tmpA5C)!= 36)goto _LL699;_LL698: goto _LL69A;_LL699:
if(*((int*)_tmpA5C)!= 1)goto _LL69B;_tmpA5D=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmpA5C)->f2;if(_tmpA5D <= (void*)1)goto _LL69B;if(*((int*)_tmpA5D)!= 0)goto _LL69B;
_LL69A: goto _LL69C;_LL69B: if(*((int*)_tmpA5C)!= 3)goto _LL69D;_LL69C: return 0;
_LL69D: if(*((int*)_tmpA5C)!= 22)goto _LL69F;_LL69E: goto _LL6A0;_LL69F: if(*((int*)
_tmpA5C)!= 24)goto _LL6A1;_LL6A0: return 0;_LL6A1: if(*((int*)_tmpA5C)!= 23)goto
_LL6A3;_tmpA5E=((struct Cyc_Absyn_AggrMember_e_struct*)_tmpA5C)->f1;_tmpA5F=((
struct Cyc_Absyn_AggrMember_e_struct*)_tmpA5C)->f2;_LL6A2: return(ignore_leaf  || 
Cyc_Tcutil_is_noalias_field(r,(void*)((struct Cyc_Core_Opt*)_check_null(_tmpA5E->topt))->v,
_tmpA5F)) && Cyc_Tcutil_is_noalias_path_aux(r,_tmpA5E,0);_LL6A3: if(*((int*)
_tmpA5C)!= 25)goto _LL6A5;_tmpA60=((struct Cyc_Absyn_Subscript_e_struct*)_tmpA5C)->f1;
_tmpA61=((struct Cyc_Absyn_Subscript_e_struct*)_tmpA5C)->f2;_LL6A4: {void*_tmpA6C=
Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmpA60->topt))->v);
struct Cyc_Absyn_PtrInfo _tmpA6D;void*_tmpA6E;struct Cyc_List_List*_tmpA6F;_LL6C8:
if(_tmpA6C <= (void*)4)goto _LL6CC;if(*((int*)_tmpA6C)!= 4)goto _LL6CA;_tmpA6D=((
struct Cyc_Absyn_PointerType_struct*)_tmpA6C)->f1;_tmpA6E=(void*)_tmpA6D.elt_typ;
_LL6C9: return 0;_LL6CA: if(*((int*)_tmpA6C)!= 9)goto _LL6CC;_tmpA6F=((struct Cyc_Absyn_TupleType_struct*)
_tmpA6C)->f1;_LL6CB: {unsigned int _tmpA71;int _tmpA72;struct _tuple7 _tmpA70=Cyc_Evexp_eval_const_uint_exp(
_tmpA61);_tmpA71=_tmpA70.f1;_tmpA72=_tmpA70.f2;if(!_tmpA72)({void*_tmpA73=0;((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const
char*_tmpA74="is_noalias_path_aux: non-constant subscript";_tag_dyneither(
_tmpA74,sizeof(char),44);}),_tag_dyneither(_tmpA73,sizeof(void*),0));});{struct
_handler_cons _tmpA75;_push_handler(& _tmpA75);{int _tmpA77=0;if(setjmp(_tmpA75.handler))
_tmpA77=1;if(!_tmpA77){{void*_tmpA78=(*((struct _tuple5*(*)(struct Cyc_List_List*x,
int n))Cyc_List_nth)(_tmpA6F,(int)_tmpA71)).f2;int _tmpA79=(ignore_leaf  || Cyc_Tcutil_is_noalias_pointer_or_aggr(
r,_tmpA78)) && Cyc_Tcutil_is_noalias_path_aux(r,_tmpA60,0);_npop_handler(0);
return _tmpA79;};_pop_handler();}else{void*_tmpA76=(void*)_exn_thrown;void*
_tmpA7B=_tmpA76;_LL6CF: if(_tmpA7B != Cyc_List_Nth)goto _LL6D1;_LL6D0: return({void*
_tmpA7C=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmpA7D="is_noalias_path_aux: out-of-bounds subscript";_tag_dyneither(
_tmpA7D,sizeof(char),45);}),_tag_dyneither(_tmpA7C,sizeof(void*),0));});_LL6D1:;
_LL6D2:(void)_throw(_tmpA7B);_LL6CE:;}}}}_LL6CC:;_LL6CD:({void*_tmpA7E=0;((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmpA7F="is_noalias_path_aux: subscript on non-pointer/tuple";_tag_dyneither(
_tmpA7F,sizeof(char),52);}),_tag_dyneither(_tmpA7E,sizeof(void*),0));});_LL6C7:;}
_LL6A5: if(*((int*)_tmpA5C)!= 32)goto _LL6A7;_LL6A6: goto _LL6A8;_LL6A7: if(*((int*)
_tmpA5C)!= 26)goto _LL6A9;_LL6A8: goto _LL6AA;_LL6A9: if(*((int*)_tmpA5C)!= 30)goto
_LL6AB;_LL6AA: goto _LL6AC;_LL6AB: if(*((int*)_tmpA5C)!= 31)goto _LL6AD;_LL6AC: goto
_LL6AE;_LL6AD: if(*((int*)_tmpA5C)!= 0)goto _LL6AF;_LL6AE: goto _LL6B0;_LL6AF: if(*((
int*)_tmpA5C)!= 35)goto _LL6B1;_LL6B0: goto _LL6B2;_LL6B1: if(*((int*)_tmpA5C)!= 17)
goto _LL6B3;_LL6B2: goto _LL6B4;_LL6B3: if(*((int*)_tmpA5C)!= 1)goto _LL6B5;_tmpA62=(
void*)((struct Cyc_Absyn_Var_e_struct*)_tmpA5C)->f2;if(_tmpA62 <= (void*)1)goto
_LL6B5;if(*((int*)_tmpA62)!= 3)goto _LL6B5;_LL6B4: goto _LL6B6;_LL6B5: if(*((int*)
_tmpA5C)!= 1)goto _LL6B7;_tmpA63=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmpA5C)->f2;
if(_tmpA63 <= (void*)1)goto _LL6B7;if(*((int*)_tmpA63)!= 4)goto _LL6B7;_LL6B6: goto
_LL6B8;_LL6B7: if(*((int*)_tmpA5C)!= 1)goto _LL6B9;_tmpA64=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmpA5C)->f2;if(_tmpA64 <= (void*)1)goto _LL6B9;if(*((int*)_tmpA64)!= 2)goto _LL6B9;
_LL6B8: {int _tmpA80=ignore_leaf  || Cyc_Tcutil_is_noalias_pointer_or_aggr(r,(void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v);return _tmpA80;}_LL6B9: if(*((int*)
_tmpA5C)!= 6)goto _LL6BB;_tmpA65=((struct Cyc_Absyn_Conditional_e_struct*)_tmpA5C)->f2;
_LL6BA: _tmpA66=_tmpA65;goto _LL6BC;_LL6BB: if(*((int*)_tmpA5C)!= 9)goto _LL6BD;
_tmpA66=((struct Cyc_Absyn_SeqExp_e_struct*)_tmpA5C)->f2;_LL6BC: _tmpA67=_tmpA66;
goto _LL6BE;_LL6BD: if(*((int*)_tmpA5C)!= 4)goto _LL6BF;_tmpA67=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmpA5C)->f1;_LL6BE: return Cyc_Tcutil_is_noalias_path_aux(r,_tmpA67,ignore_leaf);
_LL6BF: if(*((int*)_tmpA5C)!= 11)goto _LL6C1;_tmpA68=((struct Cyc_Absyn_FnCall_e_struct*)
_tmpA5C)->f1;_LL6C0: {void*_tmpA81=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmpA68->topt))->v);struct Cyc_Absyn_FnInfo _tmpA82;void*_tmpA83;
struct Cyc_Absyn_PtrInfo _tmpA84;void*_tmpA85;_LL6D4: if(_tmpA81 <= (void*)4)goto
_LL6D8;if(*((int*)_tmpA81)!= 8)goto _LL6D6;_tmpA82=((struct Cyc_Absyn_FnType_struct*)
_tmpA81)->f1;_tmpA83=(void*)_tmpA82.ret_typ;_LL6D5: return ignore_leaf  || Cyc_Tcutil_is_noalias_pointer_or_aggr(
r,_tmpA83);_LL6D6: if(*((int*)_tmpA81)!= 4)goto _LL6D8;_tmpA84=((struct Cyc_Absyn_PointerType_struct*)
_tmpA81)->f1;_tmpA85=(void*)_tmpA84.elt_typ;_LL6D7:{void*_tmpA86=Cyc_Tcutil_compress(
_tmpA85);struct Cyc_Absyn_FnInfo _tmpA87;void*_tmpA88;_LL6DB: if(_tmpA86 <= (void*)4)
goto _LL6DD;if(*((int*)_tmpA86)!= 8)goto _LL6DD;_tmpA87=((struct Cyc_Absyn_FnType_struct*)
_tmpA86)->f1;_tmpA88=(void*)_tmpA87.ret_typ;_LL6DC: return ignore_leaf  || Cyc_Tcutil_is_noalias_pointer_or_aggr(
r,_tmpA88);_LL6DD:;_LL6DE: goto _LL6DA;_LL6DA:;}goto _LL6D9;_LL6D8:;_LL6D9:({struct
Cyc_String_pa_struct _tmpA8B;_tmpA8B.tag=0;_tmpA8B.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(_tmpA68->topt))->v));{void*_tmpA89[1]={& _tmpA8B};((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmpA8A="Fncall has non function type %s";
_tag_dyneither(_tmpA8A,sizeof(char),32);}),_tag_dyneither(_tmpA89,sizeof(void*),
1));}});_LL6D3:;}_LL6C1: if(*((int*)_tmpA5C)!= 15)goto _LL6C3;_tmpA69=(void*)((
struct Cyc_Absyn_Cast_e_struct*)_tmpA5C)->f1;_tmpA6A=((struct Cyc_Absyn_Cast_e_struct*)
_tmpA5C)->f2;_LL6C2: return Cyc_Tcutil_is_noalias_pointer_or_aggr(r,_tmpA69) && 
Cyc_Tcutil_is_noalias_path_aux(r,_tmpA6A,1);_LL6C3: if(*((int*)_tmpA5C)!= 38)goto
_LL6C5;_tmpA6B=((struct Cyc_Absyn_StmtExp_e_struct*)_tmpA5C)->f1;_LL6C4: while(1){
void*_tmpA8C=(void*)_tmpA6B->r;struct Cyc_Absyn_Stmt*_tmpA8D;struct Cyc_Absyn_Stmt*
_tmpA8E;struct Cyc_Absyn_Decl*_tmpA8F;struct Cyc_Absyn_Stmt*_tmpA90;struct Cyc_Absyn_Exp*
_tmpA91;_LL6E0: if(_tmpA8C <= (void*)1)goto _LL6E6;if(*((int*)_tmpA8C)!= 1)goto
_LL6E2;_tmpA8D=((struct Cyc_Absyn_Seq_s_struct*)_tmpA8C)->f1;_tmpA8E=((struct Cyc_Absyn_Seq_s_struct*)
_tmpA8C)->f2;_LL6E1: _tmpA6B=_tmpA8E;goto _LL6DF;_LL6E2: if(*((int*)_tmpA8C)!= 11)
goto _LL6E4;_tmpA8F=((struct Cyc_Absyn_Decl_s_struct*)_tmpA8C)->f1;_tmpA90=((
struct Cyc_Absyn_Decl_s_struct*)_tmpA8C)->f2;_LL6E3: _tmpA6B=_tmpA90;goto _LL6DF;
_LL6E4: if(*((int*)_tmpA8C)!= 0)goto _LL6E6;_tmpA91=((struct Cyc_Absyn_Exp_s_struct*)
_tmpA8C)->f1;_LL6E5: return Cyc_Tcutil_is_noalias_path_aux(r,_tmpA91,ignore_leaf);
_LL6E6:;_LL6E7:({void*_tmpA92=0;((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmpA93="is_noalias_stmt_exp: ill-formed StmtExp";
_tag_dyneither(_tmpA93,sizeof(char),40);}),_tag_dyneither(_tmpA92,sizeof(void*),
0));});_LL6DF:;}_LL6C5:;_LL6C6:({struct Cyc_String_pa_struct _tmpA96;_tmpA96.tag=0;
_tmpA96.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));{
void*_tmpA94[1]={& _tmpA96};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap))Cyc_Tcutil_impos)(({const char*_tmpA95="Called is_noalias_path_aux with bogus exp |%s|\n";
_tag_dyneither(_tmpA95,sizeof(char),48);}),_tag_dyneither(_tmpA94,sizeof(void*),
1));}});_LL67A:;}int Cyc_Tcutil_is_noalias_path(struct _RegionHandle*r,struct Cyc_Absyn_Exp*
e){return Cyc_Tcutil_is_noalias_path_aux(r,e,0);}struct _tuple10 Cyc_Tcutil_addressof_props(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){struct _tuple10 bogus_ans=({struct
_tuple10 _tmpADA;_tmpADA.f1=0;_tmpADA.f2=(void*)2;_tmpADA;});void*_tmpA97=(void*)
e->r;struct _tuple2*_tmpA98;void*_tmpA99;struct Cyc_Absyn_Exp*_tmpA9A;struct
_dyneither_ptr*_tmpA9B;struct Cyc_Absyn_Exp*_tmpA9C;struct _dyneither_ptr*_tmpA9D;
struct Cyc_Absyn_Exp*_tmpA9E;struct Cyc_Absyn_Exp*_tmpA9F;struct Cyc_Absyn_Exp*
_tmpAA0;_LL6E9: if(*((int*)_tmpA97)!= 1)goto _LL6EB;_tmpA98=((struct Cyc_Absyn_Var_e_struct*)
_tmpA97)->f1;_tmpA99=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmpA97)->f2;_LL6EA: {
void*_tmpAA1=_tmpA99;struct Cyc_Absyn_Vardecl*_tmpAA2;struct Cyc_Absyn_Vardecl*
_tmpAA3;struct Cyc_Absyn_Vardecl*_tmpAA4;struct Cyc_Absyn_Vardecl*_tmpAA5;_LL6F6:
if((int)_tmpAA1 != 0)goto _LL6F8;_LL6F7: goto _LL6F9;_LL6F8: if(_tmpAA1 <= (void*)1)
goto _LL6FA;if(*((int*)_tmpAA1)!= 1)goto _LL6FA;_LL6F9: return bogus_ans;_LL6FA: if(
_tmpAA1 <= (void*)1)goto _LL6FC;if(*((int*)_tmpAA1)!= 0)goto _LL6FC;_tmpAA2=((
struct Cyc_Absyn_Global_b_struct*)_tmpAA1)->f1;_LL6FB: {void*_tmpAA6=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);_LL703: if(_tmpAA6 <= (void*)
4)goto _LL705;if(*((int*)_tmpAA6)!= 7)goto _LL705;_LL704: return({struct _tuple10
_tmpAA7;_tmpAA7.f1=1;_tmpAA7.f2=(void*)2;_tmpAA7;});_LL705:;_LL706: return({
struct _tuple10 _tmpAA8;_tmpAA8.f1=(_tmpAA2->tq).real_const;_tmpAA8.f2=(void*)2;
_tmpAA8;});_LL702:;}_LL6FC: if(_tmpAA1 <= (void*)1)goto _LL6FE;if(*((int*)_tmpAA1)
!= 3)goto _LL6FE;_tmpAA3=((struct Cyc_Absyn_Local_b_struct*)_tmpAA1)->f1;_LL6FD: {
void*_tmpAA9=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
_LL708: if(_tmpAA9 <= (void*)4)goto _LL70A;if(*((int*)_tmpAA9)!= 7)goto _LL70A;
_LL709: return({struct _tuple10 _tmpAAA;_tmpAAA.f1=1;_tmpAAA.f2=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmpAA3->rgn))->v;_tmpAAA;});_LL70A:;_LL70B: _tmpAA3->escapes=1;
return({struct _tuple10 _tmpAAB;_tmpAAB.f1=(_tmpAA3->tq).real_const;_tmpAAB.f2=(
void*)((struct Cyc_Core_Opt*)_check_null(_tmpAA3->rgn))->v;_tmpAAB;});_LL707:;}
_LL6FE: if(_tmpAA1 <= (void*)1)goto _LL700;if(*((int*)_tmpAA1)!= 4)goto _LL700;
_tmpAA4=((struct Cyc_Absyn_Pat_b_struct*)_tmpAA1)->f1;_LL6FF: _tmpAA5=_tmpAA4;goto
_LL701;_LL700: if(_tmpAA1 <= (void*)1)goto _LL6F5;if(*((int*)_tmpAA1)!= 2)goto
_LL6F5;_tmpAA5=((struct Cyc_Absyn_Param_b_struct*)_tmpAA1)->f1;_LL701: _tmpAA5->escapes=
1;return({struct _tuple10 _tmpAAC;_tmpAAC.f1=(_tmpAA5->tq).real_const;_tmpAAC.f2=(
void*)((struct Cyc_Core_Opt*)_check_null(_tmpAA5->rgn))->v;_tmpAAC;});_LL6F5:;}
_LL6EB: if(*((int*)_tmpA97)!= 23)goto _LL6ED;_tmpA9A=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmpA97)->f1;_tmpA9B=((struct Cyc_Absyn_AggrMember_e_struct*)_tmpA97)->f2;_LL6EC: {
void*_tmpAAD=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmpA9A->topt))->v);struct Cyc_List_List*_tmpAAE;struct Cyc_Absyn_AggrInfo _tmpAAF;
union Cyc_Absyn_AggrInfoU_union _tmpAB0;struct Cyc_Absyn_Aggrdecl**_tmpAB1;struct
Cyc_Absyn_Aggrdecl*_tmpAB2;_LL70D: if(_tmpAAD <= (void*)4)goto _LL711;if(*((int*)
_tmpAAD)!= 11)goto _LL70F;_tmpAAE=((struct Cyc_Absyn_AnonAggrType_struct*)_tmpAAD)->f2;
_LL70E: {struct Cyc_Absyn_Aggrfield*_tmpAB3=Cyc_Absyn_lookup_field(_tmpAAE,
_tmpA9B);if(_tmpAB3 != 0  && _tmpAB3->width != 0)return({struct _tuple10 _tmpAB4;
_tmpAB4.f1=(_tmpAB3->tq).real_const;_tmpAB4.f2=(Cyc_Tcutil_addressof_props(te,
_tmpA9A)).f2;_tmpAB4;});return bogus_ans;}_LL70F: if(*((int*)_tmpAAD)!= 10)goto
_LL711;_tmpAAF=((struct Cyc_Absyn_AggrType_struct*)_tmpAAD)->f1;_tmpAB0=_tmpAAF.aggr_info;
if((((((struct Cyc_Absyn_AggrType_struct*)_tmpAAD)->f1).aggr_info).KnownAggr).tag
!= 1)goto _LL711;_tmpAB1=(_tmpAB0.KnownAggr).f1;_tmpAB2=*_tmpAB1;_LL710: {struct
Cyc_Absyn_Aggrfield*_tmpAB5=Cyc_Absyn_lookup_decl_field(_tmpAB2,_tmpA9B);if(
_tmpAB5 != 0  && _tmpAB5->width != 0)return({struct _tuple10 _tmpAB6;_tmpAB6.f1=(
_tmpAB5->tq).real_const;_tmpAB6.f2=(Cyc_Tcutil_addressof_props(te,_tmpA9A)).f2;
_tmpAB6;});return bogus_ans;}_LL711:;_LL712: return bogus_ans;_LL70C:;}_LL6ED: if(*((
int*)_tmpA97)!= 24)goto _LL6EF;_tmpA9C=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmpA97)->f1;_tmpA9D=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmpA97)->f2;_LL6EE: {
void*_tmpAB7=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmpA9C->topt))->v);struct Cyc_Absyn_PtrInfo _tmpAB8;void*_tmpAB9;struct Cyc_Absyn_PtrAtts
_tmpABA;void*_tmpABB;_LL714: if(_tmpAB7 <= (void*)4)goto _LL716;if(*((int*)_tmpAB7)
!= 4)goto _LL716;_tmpAB8=((struct Cyc_Absyn_PointerType_struct*)_tmpAB7)->f1;
_tmpAB9=(void*)_tmpAB8.elt_typ;_tmpABA=_tmpAB8.ptr_atts;_tmpABB=(void*)_tmpABA.rgn;
_LL715: {struct Cyc_Absyn_Aggrfield*finfo;{void*_tmpABC=Cyc_Tcutil_compress(
_tmpAB9);struct Cyc_List_List*_tmpABD;struct Cyc_Absyn_AggrInfo _tmpABE;union Cyc_Absyn_AggrInfoU_union
_tmpABF;struct Cyc_Absyn_Aggrdecl**_tmpAC0;struct Cyc_Absyn_Aggrdecl*_tmpAC1;
_LL719: if(_tmpABC <= (void*)4)goto _LL71D;if(*((int*)_tmpABC)!= 11)goto _LL71B;
_tmpABD=((struct Cyc_Absyn_AnonAggrType_struct*)_tmpABC)->f2;_LL71A: finfo=Cyc_Absyn_lookup_field(
_tmpABD,_tmpA9D);goto _LL718;_LL71B: if(*((int*)_tmpABC)!= 10)goto _LL71D;_tmpABE=((
struct Cyc_Absyn_AggrType_struct*)_tmpABC)->f1;_tmpABF=_tmpABE.aggr_info;if((((((
struct Cyc_Absyn_AggrType_struct*)_tmpABC)->f1).aggr_info).KnownAggr).tag != 1)
goto _LL71D;_tmpAC0=(_tmpABF.KnownAggr).f1;_tmpAC1=*_tmpAC0;_LL71C: finfo=Cyc_Absyn_lookup_decl_field(
_tmpAC1,_tmpA9D);goto _LL718;_LL71D:;_LL71E: return bogus_ans;_LL718:;}if(finfo != 0
 && finfo->width != 0)return({struct _tuple10 _tmpAC2;_tmpAC2.f1=(finfo->tq).real_const;
_tmpAC2.f2=_tmpABB;_tmpAC2;});return bogus_ans;}_LL716:;_LL717: return bogus_ans;
_LL713:;}_LL6EF: if(*((int*)_tmpA97)!= 22)goto _LL6F1;_tmpA9E=((struct Cyc_Absyn_Deref_e_struct*)
_tmpA97)->f1;_LL6F0: {void*_tmpAC3=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmpA9E->topt))->v);struct Cyc_Absyn_PtrInfo _tmpAC4;struct Cyc_Absyn_Tqual
_tmpAC5;struct Cyc_Absyn_PtrAtts _tmpAC6;void*_tmpAC7;_LL720: if(_tmpAC3 <= (void*)4)
goto _LL722;if(*((int*)_tmpAC3)!= 4)goto _LL722;_tmpAC4=((struct Cyc_Absyn_PointerType_struct*)
_tmpAC3)->f1;_tmpAC5=_tmpAC4.elt_tq;_tmpAC6=_tmpAC4.ptr_atts;_tmpAC7=(void*)
_tmpAC6.rgn;_LL721: return({struct _tuple10 _tmpAC8;_tmpAC8.f1=_tmpAC5.real_const;
_tmpAC8.f2=_tmpAC7;_tmpAC8;});_LL722:;_LL723: return bogus_ans;_LL71F:;}_LL6F1: if(*((
int*)_tmpA97)!= 25)goto _LL6F3;_tmpA9F=((struct Cyc_Absyn_Subscript_e_struct*)
_tmpA97)->f1;_tmpAA0=((struct Cyc_Absyn_Subscript_e_struct*)_tmpA97)->f2;_LL6F2: {
void*t=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmpA9F->topt))->v);
void*_tmpAC9=t;struct Cyc_List_List*_tmpACA;struct Cyc_Absyn_PtrInfo _tmpACB;struct
Cyc_Absyn_Tqual _tmpACC;struct Cyc_Absyn_PtrAtts _tmpACD;void*_tmpACE;struct Cyc_Absyn_ArrayInfo
_tmpACF;struct Cyc_Absyn_Tqual _tmpAD0;_LL725: if(_tmpAC9 <= (void*)4)goto _LL72B;if(*((
int*)_tmpAC9)!= 9)goto _LL727;_tmpACA=((struct Cyc_Absyn_TupleType_struct*)_tmpAC9)->f1;
_LL726: {unsigned int _tmpAD2;int _tmpAD3;struct _tuple7 _tmpAD1=Cyc_Evexp_eval_const_uint_exp(
_tmpAA0);_tmpAD2=_tmpAD1.f1;_tmpAD3=_tmpAD1.f2;if(!_tmpAD3)return bogus_ans;{
struct _tuple5*_tmpAD4=Cyc_Absyn_lookup_tuple_field(_tmpACA,(int)_tmpAD2);if(
_tmpAD4 != 0)return({struct _tuple10 _tmpAD5;_tmpAD5.f1=((*_tmpAD4).f1).real_const;
_tmpAD5.f2=(Cyc_Tcutil_addressof_props(te,_tmpA9F)).f2;_tmpAD5;});return
bogus_ans;}}_LL727: if(*((int*)_tmpAC9)!= 4)goto _LL729;_tmpACB=((struct Cyc_Absyn_PointerType_struct*)
_tmpAC9)->f1;_tmpACC=_tmpACB.elt_tq;_tmpACD=_tmpACB.ptr_atts;_tmpACE=(void*)
_tmpACD.rgn;_LL728: return({struct _tuple10 _tmpAD6;_tmpAD6.f1=_tmpACC.real_const;
_tmpAD6.f2=_tmpACE;_tmpAD6;});_LL729: if(*((int*)_tmpAC9)!= 7)goto _LL72B;_tmpACF=((
struct Cyc_Absyn_ArrayType_struct*)_tmpAC9)->f1;_tmpAD0=_tmpACF.tq;_LL72A: return({
struct _tuple10 _tmpAD7;_tmpAD7.f1=_tmpAD0.real_const;_tmpAD7.f2=(Cyc_Tcutil_addressof_props(
te,_tmpA9F)).f2;_tmpAD7;});_LL72B:;_LL72C: return bogus_ans;_LL724:;}_LL6F3:;
_LL6F4:({void*_tmpAD8=0;Cyc_Tcutil_terr(e->loc,({const char*_tmpAD9="unary & applied to non-lvalue";
_tag_dyneither(_tmpAD9,sizeof(char),30);}),_tag_dyneither(_tmpAD8,sizeof(void*),
0));});return bogus_ans;_LL6E8:;}void*Cyc_Tcutil_array_to_ptr(struct Cyc_Tcenv_Tenv*
te,void*e_typ,struct Cyc_Absyn_Exp*e){void*_tmpADB=Cyc_Tcutil_compress(e_typ);
struct Cyc_Absyn_ArrayInfo _tmpADC;void*_tmpADD;struct Cyc_Absyn_Tqual _tmpADE;
struct Cyc_Absyn_Conref*_tmpADF;_LL72E: if(_tmpADB <= (void*)4)goto _LL730;if(*((int*)
_tmpADB)!= 7)goto _LL730;_tmpADC=((struct Cyc_Absyn_ArrayType_struct*)_tmpADB)->f1;
_tmpADD=(void*)_tmpADC.elt_type;_tmpADE=_tmpADC.tq;_tmpADF=_tmpADC.zero_term;
_LL72F: {void*_tmpAE1;struct _tuple10 _tmpAE0=Cyc_Tcutil_addressof_props(te,e);
_tmpAE1=_tmpAE0.f2;return Cyc_Absyn_atb_typ(_tmpADD,_tmpAE1,_tmpADE,(void*)({
struct Cyc_Absyn_Upper_b_struct*_tmpAE2=_cycalloc(sizeof(*_tmpAE2));_tmpAE2[0]=({
struct Cyc_Absyn_Upper_b_struct _tmpAE3;_tmpAE3.tag=0;_tmpAE3.f1=e;_tmpAE3;});
_tmpAE2;}),_tmpADF);}_LL730:;_LL731: return e_typ;_LL72D:;}void Cyc_Tcutil_check_bound(
struct Cyc_Position_Segment*loc,unsigned int i,struct Cyc_Absyn_Conref*b){b=Cyc_Absyn_compress_conref(
b);{union Cyc_Absyn_Constraint_union _tmpAE4=b->v;void*_tmpAE5;void*_tmpAE6;struct
Cyc_Absyn_Exp*_tmpAE7;_LL733: if((_tmpAE4.Eq_constr).tag != 0)goto _LL735;_tmpAE5=(
_tmpAE4.Eq_constr).f1;if((int)_tmpAE5 != 0)goto _LL735;_LL734: return;_LL735: if((
_tmpAE4.Eq_constr).tag != 0)goto _LL737;_tmpAE6=(_tmpAE4.Eq_constr).f1;if(_tmpAE6
<= (void*)1)goto _LL737;if(*((int*)_tmpAE6)!= 0)goto _LL737;_tmpAE7=((struct Cyc_Absyn_Upper_b_struct*)
_tmpAE6)->f1;_LL736: {unsigned int _tmpAE9;int _tmpAEA;struct _tuple7 _tmpAE8=Cyc_Evexp_eval_const_uint_exp(
_tmpAE7);_tmpAE9=_tmpAE8.f1;_tmpAEA=_tmpAE8.f2;if(_tmpAEA  && _tmpAE9 <= i)({
struct Cyc_Int_pa_struct _tmpAEE;_tmpAEE.tag=1;_tmpAEE.f1=(unsigned long)((int)i);{
struct Cyc_Int_pa_struct _tmpAED;_tmpAED.tag=1;_tmpAED.f1=(unsigned long)((int)
_tmpAE9);{void*_tmpAEB[2]={& _tmpAED,& _tmpAEE};Cyc_Tcutil_terr(loc,({const char*
_tmpAEC="dereference is out of bounds: %d <= %d";_tag_dyneither(_tmpAEC,sizeof(
char),39);}),_tag_dyneither(_tmpAEB,sizeof(void*),2));}}});return;}_LL737: if((
_tmpAE4.No_constr).tag != 2)goto _LL739;_LL738: b->v=(union Cyc_Absyn_Constraint_union)({
union Cyc_Absyn_Constraint_union _tmpAEF;(_tmpAEF.Eq_constr).tag=0;(_tmpAEF.Eq_constr).f1=(
void*)((void*)((void*)({struct Cyc_Absyn_Upper_b_struct*_tmpAF0=_cycalloc(sizeof(*
_tmpAF0));_tmpAF0[0]=({struct Cyc_Absyn_Upper_b_struct _tmpAF1;_tmpAF1.tag=0;
_tmpAF1.f1=Cyc_Absyn_uint_exp(i + 1,0);_tmpAF1;});_tmpAF0;})));_tmpAEF;});return;
_LL739:;_LL73A:({void*_tmpAF2=0;((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmpAF3="check_bound -- bad compressed conref";
_tag_dyneither(_tmpAF3,sizeof(char),37);}),_tag_dyneither(_tmpAF2,sizeof(void*),
0));});_LL732:;}}void Cyc_Tcutil_check_nonzero_bound(struct Cyc_Position_Segment*
loc,struct Cyc_Absyn_Conref*b){Cyc_Tcutil_check_bound(loc,0,b);}int Cyc_Tcutil_is_bound_one(
struct Cyc_Absyn_Conref*b){union Cyc_Absyn_Constraint_union _tmpAF4=(Cyc_Absyn_compress_conref(
b))->v;void*_tmpAF5;struct Cyc_Absyn_Exp*_tmpAF6;_LL73C: if((_tmpAF4.Eq_constr).tag
!= 0)goto _LL73E;_tmpAF5=(_tmpAF4.Eq_constr).f1;if(_tmpAF5 <= (void*)1)goto _LL73E;
if(*((int*)_tmpAF5)!= 0)goto _LL73E;_tmpAF6=((struct Cyc_Absyn_Upper_b_struct*)
_tmpAF5)->f1;_LL73D: {unsigned int _tmpAF8;int _tmpAF9;struct _tuple7 _tmpAF7=Cyc_Evexp_eval_const_uint_exp(
_tmpAF6);_tmpAF8=_tmpAF7.f1;_tmpAF9=_tmpAF7.f2;return _tmpAF9  && _tmpAF8 == 1;}
_LL73E:;_LL73F: return 0;_LL73B:;}int Cyc_Tcutil_bits_only(void*t){void*_tmpAFA=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_ArrayInfo _tmpAFB;void*_tmpAFC;struct Cyc_Absyn_Conref*_tmpAFD;
struct Cyc_List_List*_tmpAFE;struct Cyc_Absyn_AggrInfo _tmpAFF;union Cyc_Absyn_AggrInfoU_union
_tmpB00;struct Cyc_Absyn_AggrInfo _tmpB01;union Cyc_Absyn_AggrInfoU_union _tmpB02;
struct Cyc_Absyn_Aggrdecl**_tmpB03;struct Cyc_Absyn_Aggrdecl*_tmpB04;struct Cyc_List_List*
_tmpB05;struct Cyc_List_List*_tmpB06;_LL741: if((int)_tmpAFA != 0)goto _LL743;_LL742:
goto _LL744;_LL743: if(_tmpAFA <= (void*)4)goto _LL745;if(*((int*)_tmpAFA)!= 5)goto
_LL745;_LL744: goto _LL746;_LL745: if((int)_tmpAFA != 1)goto _LL747;_LL746: goto _LL748;
_LL747: if(_tmpAFA <= (void*)4)goto _LL757;if(*((int*)_tmpAFA)!= 6)goto _LL749;
_LL748: return 1;_LL749: if(*((int*)_tmpAFA)!= 12)goto _LL74B;_LL74A: goto _LL74C;
_LL74B: if(*((int*)_tmpAFA)!= 13)goto _LL74D;_LL74C: return 0;_LL74D: if(*((int*)
_tmpAFA)!= 7)goto _LL74F;_tmpAFB=((struct Cyc_Absyn_ArrayType_struct*)_tmpAFA)->f1;
_tmpAFC=(void*)_tmpAFB.elt_type;_tmpAFD=_tmpAFB.zero_term;_LL74E: return !((int(*)(
int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmpAFD) && Cyc_Tcutil_bits_only(
_tmpAFC);_LL74F: if(*((int*)_tmpAFA)!= 9)goto _LL751;_tmpAFE=((struct Cyc_Absyn_TupleType_struct*)
_tmpAFA)->f1;_LL750: for(0;_tmpAFE != 0;_tmpAFE=_tmpAFE->tl){if(!Cyc_Tcutil_bits_only((*((
struct _tuple5*)_tmpAFE->hd)).f2))return 0;}return 1;_LL751: if(*((int*)_tmpAFA)!= 
10)goto _LL753;_tmpAFF=((struct Cyc_Absyn_AggrType_struct*)_tmpAFA)->f1;_tmpB00=
_tmpAFF.aggr_info;if((((((struct Cyc_Absyn_AggrType_struct*)_tmpAFA)->f1).aggr_info).UnknownAggr).tag
!= 0)goto _LL753;_LL752: return 0;_LL753: if(*((int*)_tmpAFA)!= 10)goto _LL755;
_tmpB01=((struct Cyc_Absyn_AggrType_struct*)_tmpAFA)->f1;_tmpB02=_tmpB01.aggr_info;
if((((((struct Cyc_Absyn_AggrType_struct*)_tmpAFA)->f1).aggr_info).KnownAggr).tag
!= 1)goto _LL755;_tmpB03=(_tmpB02.KnownAggr).f1;_tmpB04=*_tmpB03;_tmpB05=_tmpB01.targs;
_LL754: if(_tmpB04->impl == 0)return 0;{struct _RegionHandle _tmpB07=_new_region("rgn");
struct _RegionHandle*rgn=& _tmpB07;_push_region(rgn);{struct Cyc_List_List*_tmpB08=((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmpB04->tvs,_tmpB05);{struct Cyc_List_List*
fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpB04->impl))->fields;for(0;fs
!= 0;fs=fs->tl){if(!Cyc_Tcutil_bits_only(Cyc_Tcutil_rsubstitute(rgn,_tmpB08,(
void*)((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))){int _tmpB09=0;_npop_handler(0);
return _tmpB09;}}}{int _tmpB0A=1;_npop_handler(0);return _tmpB0A;}};_pop_region(rgn);}
_LL755: if(*((int*)_tmpAFA)!= 11)goto _LL757;_tmpB06=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmpAFA)->f2;_LL756: for(0;_tmpB06 != 0;_tmpB06=_tmpB06->tl){if(!Cyc_Tcutil_bits_only((
void*)((struct Cyc_Absyn_Aggrfield*)_tmpB06->hd)->type))return 0;}return 1;_LL757:;
_LL758: return 0;_LL740:;}struct _tuple23{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*
f2;};static int Cyc_Tcutil_cnst_exp(struct Cyc_Tcenv_Tenv*te,int var_okay,struct Cyc_Absyn_Exp*
e){void*_tmpB0B=(void*)e->r;struct _tuple2*_tmpB0C;void*_tmpB0D;struct Cyc_Absyn_Exp*
_tmpB0E;struct Cyc_Absyn_Exp*_tmpB0F;struct Cyc_Absyn_Exp*_tmpB10;struct Cyc_Absyn_Exp*
_tmpB11;struct Cyc_Absyn_Exp*_tmpB12;struct Cyc_Absyn_Exp*_tmpB13;struct Cyc_Absyn_Exp*
_tmpB14;void*_tmpB15;struct Cyc_Absyn_Exp*_tmpB16;void*_tmpB17;void*_tmpB18;
struct Cyc_Absyn_Exp*_tmpB19;struct Cyc_Absyn_Exp*_tmpB1A;struct Cyc_Absyn_Exp*
_tmpB1B;struct Cyc_Absyn_Exp*_tmpB1C;struct Cyc_List_List*_tmpB1D;struct Cyc_List_List*
_tmpB1E;struct Cyc_List_List*_tmpB1F;void*_tmpB20;struct Cyc_List_List*_tmpB21;
struct Cyc_List_List*_tmpB22;struct Cyc_List_List*_tmpB23;_LL75A: if(*((int*)
_tmpB0B)!= 0)goto _LL75C;_LL75B: goto _LL75D;_LL75C: if(*((int*)_tmpB0B)!= 18)goto
_LL75E;_LL75D: goto _LL75F;_LL75E: if(*((int*)_tmpB0B)!= 19)goto _LL760;_LL75F: goto
_LL761;_LL760: if(*((int*)_tmpB0B)!= 20)goto _LL762;_LL761: goto _LL763;_LL762: if(*((
int*)_tmpB0B)!= 21)goto _LL764;_LL763: goto _LL765;_LL764: if(*((int*)_tmpB0B)!= 33)
goto _LL766;_LL765: goto _LL767;_LL766: if(*((int*)_tmpB0B)!= 34)goto _LL768;_LL767:
return 1;_LL768: if(*((int*)_tmpB0B)!= 1)goto _LL76A;_tmpB0C=((struct Cyc_Absyn_Var_e_struct*)
_tmpB0B)->f1;_tmpB0D=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmpB0B)->f2;_LL769: {
void*_tmpB24=_tmpB0D;struct Cyc_Absyn_Vardecl*_tmpB25;_LL789: if(_tmpB24 <= (void*)
1)goto _LL78D;if(*((int*)_tmpB24)!= 1)goto _LL78B;_LL78A: return 1;_LL78B: if(*((int*)
_tmpB24)!= 0)goto _LL78D;_tmpB25=((struct Cyc_Absyn_Global_b_struct*)_tmpB24)->f1;
_LL78C: {void*_tmpB26=Cyc_Tcutil_compress((void*)_tmpB25->type);_LL792: if(
_tmpB26 <= (void*)4)goto _LL796;if(*((int*)_tmpB26)!= 7)goto _LL794;_LL793: goto
_LL795;_LL794: if(*((int*)_tmpB26)!= 8)goto _LL796;_LL795: return 1;_LL796:;_LL797:
return var_okay;_LL791:;}_LL78D: if((int)_tmpB24 != 0)goto _LL78F;_LL78E: return 0;
_LL78F:;_LL790: return var_okay;_LL788:;}_LL76A: if(*((int*)_tmpB0B)!= 6)goto _LL76C;
_tmpB0E=((struct Cyc_Absyn_Conditional_e_struct*)_tmpB0B)->f1;_tmpB0F=((struct Cyc_Absyn_Conditional_e_struct*)
_tmpB0B)->f2;_tmpB10=((struct Cyc_Absyn_Conditional_e_struct*)_tmpB0B)->f3;_LL76B:
return(Cyc_Tcutil_cnst_exp(te,0,_tmpB0E) && Cyc_Tcutil_cnst_exp(te,0,_tmpB0F))
 && Cyc_Tcutil_cnst_exp(te,0,_tmpB10);_LL76C: if(*((int*)_tmpB0B)!= 9)goto _LL76E;
_tmpB11=((struct Cyc_Absyn_SeqExp_e_struct*)_tmpB0B)->f1;_tmpB12=((struct Cyc_Absyn_SeqExp_e_struct*)
_tmpB0B)->f2;_LL76D: return Cyc_Tcutil_cnst_exp(te,0,_tmpB11) && Cyc_Tcutil_cnst_exp(
te,0,_tmpB12);_LL76E: if(*((int*)_tmpB0B)!= 13)goto _LL770;_tmpB13=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmpB0B)->f1;_LL76F: _tmpB14=_tmpB13;goto _LL771;_LL770: if(*((int*)_tmpB0B)!= 14)
goto _LL772;_tmpB14=((struct Cyc_Absyn_Instantiate_e_struct*)_tmpB0B)->f1;_LL771:
return Cyc_Tcutil_cnst_exp(te,var_okay,_tmpB14);_LL772: if(*((int*)_tmpB0B)!= 15)
goto _LL774;_tmpB15=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmpB0B)->f1;_tmpB16=((
struct Cyc_Absyn_Cast_e_struct*)_tmpB0B)->f2;_tmpB17=(void*)((struct Cyc_Absyn_Cast_e_struct*)
_tmpB0B)->f4;if((int)_tmpB17 != 1)goto _LL774;_LL773: return Cyc_Tcutil_cnst_exp(te,
var_okay,_tmpB16);_LL774: if(*((int*)_tmpB0B)!= 15)goto _LL776;_tmpB18=(void*)((
struct Cyc_Absyn_Cast_e_struct*)_tmpB0B)->f1;_tmpB19=((struct Cyc_Absyn_Cast_e_struct*)
_tmpB0B)->f2;_LL775: return Cyc_Tcutil_cnst_exp(te,var_okay,_tmpB19);_LL776: if(*((
int*)_tmpB0B)!= 16)goto _LL778;_tmpB1A=((struct Cyc_Absyn_Address_e_struct*)
_tmpB0B)->f1;_LL777: return Cyc_Tcutil_cnst_exp(te,1,_tmpB1A);_LL778: if(*((int*)
_tmpB0B)!= 29)goto _LL77A;_tmpB1B=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmpB0B)->f2;_tmpB1C=((struct Cyc_Absyn_Comprehension_e_struct*)_tmpB0B)->f3;
_LL779: return Cyc_Tcutil_cnst_exp(te,0,_tmpB1B) && Cyc_Tcutil_cnst_exp(te,0,
_tmpB1C);_LL77A: if(*((int*)_tmpB0B)!= 28)goto _LL77C;_tmpB1D=((struct Cyc_Absyn_Array_e_struct*)
_tmpB0B)->f1;_LL77B: _tmpB1E=_tmpB1D;goto _LL77D;_LL77C: if(*((int*)_tmpB0B)!= 31)
goto _LL77E;_tmpB1E=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmpB0B)->f2;_LL77D:
_tmpB1F=_tmpB1E;goto _LL77F;_LL77E: if(*((int*)_tmpB0B)!= 30)goto _LL780;_tmpB1F=((
struct Cyc_Absyn_Struct_e_struct*)_tmpB0B)->f3;_LL77F: for(0;_tmpB1F != 0;_tmpB1F=
_tmpB1F->tl){if(!Cyc_Tcutil_cnst_exp(te,0,(*((struct _tuple23*)_tmpB1F->hd)).f2))
return 0;}return 1;_LL780: if(*((int*)_tmpB0B)!= 3)goto _LL782;_tmpB20=(void*)((
struct Cyc_Absyn_Primop_e_struct*)_tmpB0B)->f1;_tmpB21=((struct Cyc_Absyn_Primop_e_struct*)
_tmpB0B)->f2;_LL781: _tmpB22=_tmpB21;goto _LL783;_LL782: if(*((int*)_tmpB0B)!= 26)
goto _LL784;_tmpB22=((struct Cyc_Absyn_Tuple_e_struct*)_tmpB0B)->f1;_LL783: _tmpB23=
_tmpB22;goto _LL785;_LL784: if(*((int*)_tmpB0B)!= 32)goto _LL786;_tmpB23=((struct
Cyc_Absyn_Tunion_e_struct*)_tmpB0B)->f1;_LL785: for(0;_tmpB23 != 0;_tmpB23=_tmpB23->tl){
if(!Cyc_Tcutil_cnst_exp(te,0,(struct Cyc_Absyn_Exp*)_tmpB23->hd))return 0;}return 1;
_LL786:;_LL787: return 0;_LL759:;}int Cyc_Tcutil_is_const_exp(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e){return Cyc_Tcutil_cnst_exp(te,0,e);}static int Cyc_Tcutil_fields_support_default(
struct Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs);int Cyc_Tcutil_supports_default(
void*t){void*_tmpB27=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmpB28;
struct Cyc_Absyn_PtrAtts _tmpB29;struct Cyc_Absyn_Conref*_tmpB2A;struct Cyc_Absyn_Conref*
_tmpB2B;struct Cyc_Absyn_ArrayInfo _tmpB2C;void*_tmpB2D;struct Cyc_List_List*
_tmpB2E;struct Cyc_Absyn_AggrInfo _tmpB2F;union Cyc_Absyn_AggrInfoU_union _tmpB30;
struct Cyc_List_List*_tmpB31;struct Cyc_List_List*_tmpB32;_LL799: if((int)_tmpB27 != 
0)goto _LL79B;_LL79A: goto _LL79C;_LL79B: if(_tmpB27 <= (void*)4)goto _LL79D;if(*((int*)
_tmpB27)!= 5)goto _LL79D;_LL79C: goto _LL79E;_LL79D: if((int)_tmpB27 != 1)goto _LL79F;
_LL79E: goto _LL7A0;_LL79F: if(_tmpB27 <= (void*)4)goto _LL7AF;if(*((int*)_tmpB27)!= 
6)goto _LL7A1;_LL7A0: return 1;_LL7A1: if(*((int*)_tmpB27)!= 4)goto _LL7A3;_tmpB28=((
struct Cyc_Absyn_PointerType_struct*)_tmpB27)->f1;_tmpB29=_tmpB28.ptr_atts;
_tmpB2A=_tmpB29.nullable;_tmpB2B=_tmpB29.bounds;_LL7A2: {void*_tmpB33=Cyc_Absyn_conref_def((
void*)((void*)0),_tmpB2B);_LL7B2: if((int)_tmpB33 != 0)goto _LL7B4;_LL7B3: return 1;
_LL7B4:;_LL7B5: return((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
1,_tmpB2A);_LL7B1:;}_LL7A3: if(*((int*)_tmpB27)!= 7)goto _LL7A5;_tmpB2C=((struct
Cyc_Absyn_ArrayType_struct*)_tmpB27)->f1;_tmpB2D=(void*)_tmpB2C.elt_type;_LL7A4:
return Cyc_Tcutil_supports_default(_tmpB2D);_LL7A5: if(*((int*)_tmpB27)!= 9)goto
_LL7A7;_tmpB2E=((struct Cyc_Absyn_TupleType_struct*)_tmpB27)->f1;_LL7A6: for(0;
_tmpB2E != 0;_tmpB2E=_tmpB2E->tl){if(!Cyc_Tcutil_supports_default((*((struct
_tuple5*)_tmpB2E->hd)).f2))return 0;}return 1;_LL7A7: if(*((int*)_tmpB27)!= 10)goto
_LL7A9;_tmpB2F=((struct Cyc_Absyn_AggrType_struct*)_tmpB27)->f1;_tmpB30=_tmpB2F.aggr_info;
_tmpB31=_tmpB2F.targs;_LL7A8: {struct Cyc_Absyn_Aggrdecl*_tmpB34=Cyc_Absyn_get_known_aggrdecl(
_tmpB30);if(_tmpB34->impl == 0)return 0;if(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmpB34->impl))->exist_vars != 0)return 0;return Cyc_Tcutil_fields_support_default(
_tmpB34->tvs,_tmpB31,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpB34->impl))->fields);}
_LL7A9: if(*((int*)_tmpB27)!= 11)goto _LL7AB;_tmpB32=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmpB27)->f2;_LL7AA: return Cyc_Tcutil_fields_support_default(0,0,_tmpB32);_LL7AB:
if(*((int*)_tmpB27)!= 13)goto _LL7AD;_LL7AC: goto _LL7AE;_LL7AD: if(*((int*)_tmpB27)
!= 12)goto _LL7AF;_LL7AE: return 1;_LL7AF:;_LL7B0: return 0;_LL798:;}static int Cyc_Tcutil_fields_support_default(
struct Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs){struct
_RegionHandle _tmpB35=_new_region("rgn");struct _RegionHandle*rgn=& _tmpB35;
_push_region(rgn);{struct Cyc_List_List*_tmpB36=((struct Cyc_List_List*(*)(struct
_RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(rgn,rgn,tvs,ts);for(0;fs != 0;fs=fs->tl){void*t=Cyc_Tcutil_rsubstitute(
rgn,_tmpB36,(void*)((struct Cyc_Absyn_Aggrfield*)fs->hd)->type);if(!Cyc_Tcutil_supports_default(
t)){int _tmpB37=0;_npop_handler(0);return _tmpB37;}}}{int _tmpB38=1;_npop_handler(0);
return _tmpB38;};_pop_region(rgn);}int Cyc_Tcutil_admits_zero(void*t){void*_tmpB39=
Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmpB3A;struct Cyc_Absyn_PtrAtts
_tmpB3B;struct Cyc_Absyn_Conref*_tmpB3C;struct Cyc_Absyn_Conref*_tmpB3D;_LL7B7: if(
_tmpB39 <= (void*)4)goto _LL7B9;if(*((int*)_tmpB39)!= 5)goto _LL7B9;_LL7B8: goto
_LL7BA;_LL7B9: if((int)_tmpB39 != 1)goto _LL7BB;_LL7BA: goto _LL7BC;_LL7BB: if(_tmpB39
<= (void*)4)goto _LL7BF;if(*((int*)_tmpB39)!= 6)goto _LL7BD;_LL7BC: return 1;_LL7BD:
if(*((int*)_tmpB39)!= 4)goto _LL7BF;_tmpB3A=((struct Cyc_Absyn_PointerType_struct*)
_tmpB39)->f1;_tmpB3B=_tmpB3A.ptr_atts;_tmpB3C=_tmpB3B.nullable;_tmpB3D=_tmpB3B.bounds;
_LL7BE: {union Cyc_Absyn_Constraint_union _tmpB3E=(Cyc_Absyn_compress_conref(
_tmpB3D))->v;void*_tmpB3F;_LL7C2: if((_tmpB3E.Eq_constr).tag != 0)goto _LL7C4;
_tmpB3F=(_tmpB3E.Eq_constr).f1;if((int)_tmpB3F != 0)goto _LL7C4;_LL7C3: return 0;
_LL7C4: if((_tmpB3E.Eq_constr).tag != 0)goto _LL7C6;_LL7C5: {union Cyc_Absyn_Constraint_union
_tmpB40=(((struct Cyc_Absyn_Conref*(*)(struct Cyc_Absyn_Conref*x))Cyc_Absyn_compress_conref)(
_tmpB3C))->v;int _tmpB41;_LL7C9: if((_tmpB40.Eq_constr).tag != 0)goto _LL7CB;_tmpB41=(
_tmpB40.Eq_constr).f1;_LL7CA: return _tmpB41;_LL7CB:;_LL7CC: return 0;_LL7C8:;}
_LL7C6:;_LL7C7: return 0;_LL7C1:;}_LL7BF:;_LL7C0: return 0;_LL7B6:;}int Cyc_Tcutil_is_noreturn(
void*t){{void*_tmpB42=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmpB43;void*
_tmpB44;struct Cyc_Absyn_FnInfo _tmpB45;struct Cyc_List_List*_tmpB46;_LL7CE: if(
_tmpB42 <= (void*)4)goto _LL7D2;if(*((int*)_tmpB42)!= 4)goto _LL7D0;_tmpB43=((
struct Cyc_Absyn_PointerType_struct*)_tmpB42)->f1;_tmpB44=(void*)_tmpB43.elt_typ;
_LL7CF: return Cyc_Tcutil_is_noreturn(_tmpB44);_LL7D0: if(*((int*)_tmpB42)!= 8)goto
_LL7D2;_tmpB45=((struct Cyc_Absyn_FnType_struct*)_tmpB42)->f1;_tmpB46=_tmpB45.attributes;
_LL7D1: for(0;_tmpB46 != 0;_tmpB46=_tmpB46->tl){void*_tmpB47=(void*)_tmpB46->hd;
_LL7D5: if((int)_tmpB47 != 3)goto _LL7D7;_LL7D6: return 1;_LL7D7:;_LL7D8: continue;
_LL7D4:;}goto _LL7CD;_LL7D2:;_LL7D3: goto _LL7CD;_LL7CD:;}return 0;}struct Cyc_List_List*
Cyc_Tcutil_transfer_fn_type_atts(void*t,struct Cyc_List_List*atts){void*_tmpB48=
Cyc_Tcutil_compress(t);struct Cyc_Absyn_FnInfo _tmpB49;struct Cyc_List_List*_tmpB4A;
struct Cyc_List_List**_tmpB4B;_LL7DA: if(_tmpB48 <= (void*)4)goto _LL7DC;if(*((int*)
_tmpB48)!= 8)goto _LL7DC;_tmpB49=((struct Cyc_Absyn_FnType_struct*)_tmpB48)->f1;
_tmpB4A=_tmpB49.attributes;_tmpB4B=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_struct*)
_tmpB48)->f1).attributes;_LL7DB: {struct Cyc_List_List*_tmpB4C=0;for(0;atts != 0;
atts=atts->tl){if(Cyc_Absyn_fntype_att((void*)atts->hd))*_tmpB4B=({struct Cyc_List_List*
_tmpB4D=_cycalloc(sizeof(*_tmpB4D));_tmpB4D->hd=(void*)((void*)atts->hd);_tmpB4D->tl=*
_tmpB4B;_tmpB4D;});else{_tmpB4C=({struct Cyc_List_List*_tmpB4E=_cycalloc(sizeof(*
_tmpB4E));_tmpB4E->hd=(void*)((void*)atts->hd);_tmpB4E->tl=_tmpB4C;_tmpB4E;});}}
return _tmpB4C;}_LL7DC:;_LL7DD:({void*_tmpB4F=0;((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmpB50="transfer_fn_type_atts";
_tag_dyneither(_tmpB50,sizeof(char),22);}),_tag_dyneither(_tmpB4F,sizeof(void*),
0));});_LL7D9:;}
