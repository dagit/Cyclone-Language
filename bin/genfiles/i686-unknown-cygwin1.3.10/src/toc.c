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
struct _RegionHandle*Cyc_Core_heap_region;struct Cyc_Core_NewRegion Cyc_Core_new_dynregion();
extern char Cyc_Core_Open_Region[16];extern char Cyc_Core_Free_Region[16];void Cyc_Core_free_dynregion(
struct _DynRegionHandle*);struct Cyc___cycFILE;extern struct Cyc___cycFILE*Cyc_stderr;
struct Cyc_Cstdio___abstractFILE;struct Cyc_String_pa_struct{int tag;struct
_dyneither_ptr f1;};struct Cyc_Int_pa_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_struct{
int tag;double f1;};struct Cyc_LongDouble_pa_struct{int tag;long double f1;};struct
Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{int tag;
unsigned long*f1;};struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr,struct
_dyneither_ptr);int Cyc_fflush(struct Cyc___cycFILE*);int Cyc_fprintf(struct Cyc___cycFILE*,
struct _dyneither_ptr,struct _dyneither_ptr);struct Cyc_ShortPtr_sa_struct{int tag;
short*f1;};struct Cyc_UShortPtr_sa_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_struct{
int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_struct{
int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_struct{int tag;double*f1;}
;struct Cyc_FloatPtr_sa_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int
tag;struct _dyneither_ptr f1;};int Cyc_vfprintf(struct Cyc___cycFILE*,struct
_dyneither_ptr,struct _dyneither_ptr);extern char Cyc_FileCloseError[19];extern char
Cyc_FileOpenError[18];struct Cyc_FileOpenError_struct{char*tag;struct
_dyneither_ptr f1;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};struct
Cyc_List_List*Cyc_List_list(struct _dyneither_ptr);int Cyc_List_length(struct Cyc_List_List*
x);struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);
struct Cyc_List_List*Cyc_List_rmap(struct _RegionHandle*,void*(*f)(void*),struct
Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rmap_c(struct _RegionHandle*,void*(*
f)(void*,void*),void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[
18];void Cyc_List_iter(void(*f)(void*),struct Cyc_List_List*x);void Cyc_List_iter_c(
void(*f)(void*,void*),void*env,struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rev(
struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rrev(struct _RegionHandle*,
struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*
x);struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*
y);struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*x,struct Cyc_List_List*
y);extern char Cyc_List_Nth[8];void*Cyc_List_nth(struct Cyc_List_List*x,int n);int
Cyc_List_forall(int(*pred)(void*),struct Cyc_List_List*x);int Cyc_strcmp(struct
_dyneither_ptr s1,struct _dyneither_ptr s2);struct _dyneither_ptr Cyc_strconcat(
struct _dyneither_ptr,struct _dyneither_ptr);struct Cyc_Iter_Iter{void*env;int(*
next)(void*env,void*dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct
Cyc_Set_Set;struct Cyc_Set_Set*Cyc_Set_rempty(struct _RegionHandle*r,int(*cmp)(
void*,void*));struct Cyc_Set_Set*Cyc_Set_rinsert(struct _RegionHandle*r,struct Cyc_Set_Set*
s,void*elt);int Cyc_Set_member(struct Cyc_Set_Set*s,void*elt);extern char Cyc_Set_Absent[
11];struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct
_RegionHandle*r;struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[12];extern char
Cyc_Dict_Absent[11];struct Cyc_Dict_Dict Cyc_Dict_rempty(struct _RegionHandle*,int(*
cmp)(void*,void*));struct Cyc_Dict_Dict Cyc_Dict_rshare(struct _RegionHandle*,
struct Cyc_Dict_Dict);struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict d,
void*k,void*v);void*Cyc_Dict_lookup(struct Cyc_Dict_Dict d,void*k);void**Cyc_Dict_lookup_opt(
struct Cyc_Dict_Dict d,void*k);struct _tuple0{void*f1;void*f2;};struct _tuple0*Cyc_Dict_rchoose(
struct _RegionHandle*r,struct Cyc_Dict_Dict d);struct _tuple0*Cyc_Dict_rchoose(
struct _RegionHandle*,struct Cyc_Dict_Dict d);struct Cyc_Lineno_Pos{struct
_dyneither_ptr logical_file;struct _dyneither_ptr line;int line_no;int col;};extern
char Cyc_Position_Exit[9];struct Cyc_Position_Segment;struct _dyneither_ptr Cyc_Position_string_of_segment(
struct Cyc_Position_Segment*);struct Cyc_Position_Error{struct _dyneither_ptr source;
struct Cyc_Position_Segment*seg;void*kind;struct _dyneither_ptr desc;};extern char
Cyc_Position_Nocontext[14];struct Cyc_Xarray_Xarray{struct _RegionHandle*r;struct
_dyneither_ptr elmts;int num_elmts;};int Cyc_Xarray_length(struct Cyc_Xarray_Xarray*);
void*Cyc_Xarray_get(struct Cyc_Xarray_Xarray*,int);struct Cyc_Xarray_Xarray*Cyc_Xarray_rcreate_empty(
struct _RegionHandle*);int Cyc_Xarray_add_ind(struct Cyc_Xarray_Xarray*,void*);
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
Cyc_List_List*rgn_po;struct Cyc_List_List*fields;};struct Cyc_Absyn_Aggrdecl{void*
kind;void*sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*
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
_dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];int Cyc_Absyn_qvar_cmp(
struct _tuple1*,struct _tuple1*);struct Cyc_Absyn_Conref*Cyc_Absyn_compress_conref(
struct Cyc_Absyn_Conref*x);void*Cyc_Absyn_conref_val(struct Cyc_Absyn_Conref*x);
void*Cyc_Absyn_conref_def(void*,struct Cyc_Absyn_Conref*x);extern struct Cyc_Absyn_Conref*
Cyc_Absyn_true_conref;extern struct Cyc_Absyn_Conref*Cyc_Absyn_false_conref;extern
void*Cyc_Absyn_char_typ;extern void*Cyc_Absyn_uint_typ;extern void*Cyc_Absyn_sint_typ;
extern void*Cyc_Absyn_exn_typ;extern void*Cyc_Absyn_bounds_one;void*Cyc_Absyn_star_typ(
void*t,void*rgn,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Conref*zero_term);void*
Cyc_Absyn_cstar_typ(void*t,struct Cyc_Absyn_Tqual tq);void*Cyc_Absyn_dyneither_typ(
void*t,void*rgn,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Conref*zero_term);void*
Cyc_Absyn_void_star_typ();void*Cyc_Absyn_strct(struct _dyneither_ptr*name);void*
Cyc_Absyn_strctq(struct _tuple1*name);void*Cyc_Absyn_unionq_typ(struct _tuple1*
name);void*Cyc_Absyn_array_typ(void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*
num_elts,struct Cyc_Absyn_Conref*zero_term,struct Cyc_Position_Segment*ztloc);
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,struct Cyc_Position_Segment*);struct
Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(
union Cyc_Absyn_Cnst_union,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_null_exp(
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(void*,int,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_signed_int_exp(int,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_char_exp(char c,struct
Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_float_exp(struct
_dyneither_ptr f,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(
struct _dyneither_ptr s,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_var_exp(
struct _tuple1*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(
struct _tuple1*,void*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(
void*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(
void*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_add_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_times_exp(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_divide_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_eq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_neq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_lt_exp(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_lte_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_assignop_exp(struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_assign_exp(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_post_inc_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_fncall_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_throw_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*,struct Cyc_Absyn_Exp*,int user_cast,
void*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(
void*t,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_sizeofexp_exp(
struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(
struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,struct Cyc_Position_Segment*);struct
Cyc_Absyn_Exp*Cyc_Absyn_aggrarrow_exp(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct
Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_match_exn_exp(struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(
void*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_unresolvedmem_exp(
struct Cyc_Core_Opt*,struct Cyc_List_List*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Stmt*
Cyc_Absyn_new_stmt(void*s,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*
Cyc_Absyn_skip_stmt(struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(
struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(
struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmts(struct Cyc_List_List*,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_return_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(struct Cyc_Absyn_Exp*e,struct
Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,struct Cyc_Position_Segment*loc);struct
Cyc_Absyn_Stmt*Cyc_Absyn_for_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,
struct Cyc_Absyn_Exp*e3,struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_switch_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*,
struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(struct
Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*loc);struct
Cyc_Absyn_Stmt*Cyc_Absyn_declare_stmt(struct _tuple1*,void*,struct Cyc_Absyn_Exp*
init,struct Cyc_Absyn_Stmt*,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*
Cyc_Absyn_label_stmt(struct _dyneither_ptr*v,struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_goto_stmt(struct _dyneither_ptr*lab,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_assign_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*
r,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(
struct _tuple1*x,void*t,struct Cyc_Absyn_Exp*init);struct Cyc_Absyn_Vardecl*Cyc_Absyn_static_vardecl(
struct _tuple1*x,void*t,struct Cyc_Absyn_Exp*init);int Cyc_Absyn_is_lvalue(struct
Cyc_Absyn_Exp*);struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,
struct _dyneither_ptr*);struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(
struct Cyc_Absyn_Aggrdecl*,struct _dyneither_ptr*);struct _dyneither_ptr*Cyc_Absyn_fieldname(
int);struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfoU_union
info);extern int Cyc_Absyn_no_regions;struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct
Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs: 1;int qvar_to_Cids: 1;int
add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int rewrite_temp_tvars: 1;int
print_all_tvars: 1;int print_all_kinds: 1;int print_all_effects: 1;int
print_using_stmts: 1;int print_externC_stmts: 1;int print_full_evars: 1;int
print_zeroterm: 1;int generate_line_directives: 1;int use_curr_namespace: 1;struct Cyc_List_List*
curr_namespace;};struct _dyneither_ptr Cyc_Absynpp_typ2string(void*);struct
_dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);struct _dyneither_ptr
Cyc_Absynpp_stmt2string(struct Cyc_Absyn_Stmt*);struct _dyneither_ptr Cyc_Absynpp_qvar2string(
struct _tuple1*);struct Cyc_RgnOrder_RgnPO;struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_initial_fn_po(
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
f1;};struct Cyc_Tcenv_TunionRes_struct{int tag;struct Cyc_Absyn_Tuniondecl*f1;
struct Cyc_Absyn_Tunionfield*f2;};struct Cyc_Tcenv_EnumRes_struct{int tag;struct Cyc_Absyn_Enumdecl*
f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcenv_AnonEnumRes_struct{int tag;void*
f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcenv_Genv{struct _RegionHandle*grgn;
struct Cyc_Set_Set*namespaces;struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict
tuniondecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict
ordinaries;struct Cyc_List_List*availables;};struct Cyc_Tcenv_Fenv;struct Cyc_Tcenv_Stmt_j_struct{
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;
struct Cyc_Dict_Dict ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof;};void*Cyc_Tcutil_impos(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap);void Cyc_Tcutil_terr(struct Cyc_Position_Segment*,
struct _dyneither_ptr fmt,struct _dyneither_ptr ap);void Cyc_Tcutil_warn(struct Cyc_Position_Segment*,
struct _dyneither_ptr fmt,struct _dyneither_ptr ap);void*Cyc_Tcutil_tvar_kind(struct
Cyc_Absyn_Tvar*t);void*Cyc_Tcutil_typ_kind(void*t);void*Cyc_Tcutil_compress(void*
t);int Cyc_Tcutil_is_pointer_type(void*t);int Cyc_Tcutil_is_pointer_or_boxed(void*
t,int*is_dyneither_ptr);int Cyc_Tcutil_unify(void*,void*);struct Cyc_List_List*Cyc_Tcutil_resolve_struct_designators(
struct _RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,
struct Cyc_List_List*fields);int Cyc_Tcutil_is_tagged_pointer_typ(void*);int Cyc_Tcutil_is_tagged_pointer_typ_elt(
void*t,void**elt_typ_dest);int Cyc_Tcutil_is_zero_pointer_typ_elt(void*t,void**
elt_typ_dest);int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*e1,void**
ptr_type,int*is_dyneither,void**elt_type);struct _tuple4{struct Cyc_Absyn_Tqual f1;
void*f2;};void*Cyc_Tcutil_snd_tqt(struct _tuple4*);struct _tuple5{unsigned int f1;
int f2;};struct _tuple5 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);int Cyc_Evexp_c_can_eval(
struct Cyc_Absyn_Exp*e);int Cyc_Evexp_same_const_exp(struct Cyc_Absyn_Exp*e1,struct
Cyc_Absyn_Exp*e2);int Cyc_Evexp_lte_const_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2);struct Cyc_CfFlowInfo_VarRoot_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};
struct Cyc_CfFlowInfo_MallocPt_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;};
struct Cyc_CfFlowInfo_InitParam_struct{int tag;int f1;void*f2;};struct Cyc_CfFlowInfo_Place{
void*root;struct Cyc_List_List*fields;};struct Cyc_CfFlowInfo_UniquePlace{struct
Cyc_CfFlowInfo_Place place;struct Cyc_List_List*path;};struct Cyc_CfFlowInfo_EqualConst_struct{
int tag;unsigned int f1;};struct Cyc_CfFlowInfo_LessVar_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;void*f2;};struct Cyc_CfFlowInfo_LessNumelts_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_CfFlowInfo_LessConst_struct{int tag;unsigned int f1;};struct Cyc_CfFlowInfo_LessEqNumelts_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};union Cyc_CfFlowInfo_RelnOp_union{struct Cyc_CfFlowInfo_EqualConst_struct
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
f1;};struct Cyc_List_List*Cyc_Toc_toc(struct Cyc_List_List*ds);struct _tuple1*Cyc_Toc_temp_var();
extern struct _dyneither_ptr Cyc_Toc_globals;extern int Cyc_noexpand_r;int Cyc_Toc_warn_bounds_checks=
0;int Cyc_Toc_warn_all_null_deref=0;unsigned int Cyc_Toc_total_bounds_checks=0;
unsigned int Cyc_Toc_bounds_checks_eliminated=0;static struct Cyc_List_List*Cyc_Toc_result_decls=
0;struct Cyc_Toc_TocState{struct _DynRegionHandle*dyn;struct Cyc_List_List**
tuple_types;struct Cyc_Dict_Dict*aggrs_so_far;struct Cyc_Set_Set**tunions_so_far;
struct Cyc_Dict_Dict*xtunions_so_far;struct Cyc_Dict_Dict*qvar_tags;struct Cyc_Xarray_Xarray*
temp_labels;};static struct Cyc_Toc_TocState*Cyc_Toc_toc_state=0;struct _tuple6{
struct _tuple1*f1;struct _dyneither_ptr f2;};int Cyc_Toc_qvar_tag_cmp(struct _tuple6*
x,struct _tuple6*y){struct _tuple1*_tmp1;struct _dyneither_ptr _tmp2;struct _tuple6
_tmp0=*x;_tmp1=_tmp0.f1;_tmp2=_tmp0.f2;{struct _tuple1*_tmp4;struct _dyneither_ptr
_tmp5;struct _tuple6 _tmp3=*y;_tmp4=_tmp3.f1;_tmp5=_tmp3.f2;{int i=Cyc_Absyn_qvar_cmp(
_tmp1,_tmp4);if(i != 0)return i;return Cyc_strcmp((struct _dyneither_ptr)_tmp2,(
struct _dyneither_ptr)_tmp5);}}}struct _tuple7{struct Cyc_Absyn_Aggrdecl*f1;void*f2;
};void*Cyc_Toc_aggrdecl_type(struct _tuple1*q,void*(*type_maker)(struct _tuple1*)){
struct _DynRegionHandle*_tmp7;struct Cyc_Dict_Dict*_tmp8;struct Cyc_Toc_TocState
_tmp6=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp7=_tmp6.dyn;
_tmp8=_tmp6.aggrs_so_far;{struct _DynRegionFrame _tmp9;struct _RegionHandle*d=
_open_dynregion(& _tmp9,_tmp7);{struct _tuple7**v=((struct _tuple7**(*)(struct Cyc_Dict_Dict
d,struct _tuple1*k))Cyc_Dict_lookup_opt)(*_tmp8,q);if(v == 0){void*_tmpA=
type_maker(q);_npop_handler(0);return _tmpA;}else{struct _tuple7 _tmpC;void*_tmpD;
struct _tuple7*_tmpB=*v;_tmpC=*_tmpB;_tmpD=_tmpC.f2;{void*_tmpE=_tmpD;
_npop_handler(0);return _tmpE;}}};_pop_dynregion(d);}}static int Cyc_Toc_tuple_type_counter=
0;static int Cyc_Toc_temp_var_counter=0;static int Cyc_Toc_fresh_label_counter=0;
char Cyc_Toc_Toc_Unimplemented[22]="\000\000\000\000Toc_Unimplemented\000";char
Cyc_Toc_Toc_Impossible[19]="\000\000\000\000Toc_Impossible\000";static void*Cyc_Toc_unimp(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap){Cyc_vfprintf(Cyc_stderr,fmt,ap);({
void*_tmpF=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp10="\n";_tag_dyneither(
_tmp10,sizeof(char),2);}),_tag_dyneither(_tmpF,sizeof(void*),0));});Cyc_fflush((
struct Cyc___cycFILE*)Cyc_stderr);(int)_throw((void*)Cyc_Toc_Toc_Unimplemented);}
static void*Cyc_Toc_toc_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){Cyc_vfprintf(
Cyc_stderr,fmt,ap);({void*_tmp11=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp12="\n";
_tag_dyneither(_tmp12,sizeof(char),2);}),_tag_dyneither(_tmp11,sizeof(void*),0));});
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);(int)_throw((void*)Cyc_Toc_Toc_Impossible);}
char Cyc_Toc_Match_error[16]="\000\000\000\000Match_error\000";static char _tmp13[5]="curr";
static struct _dyneither_ptr Cyc_Toc_curr_string={_tmp13,_tmp13,_tmp13 + 5};static
struct _dyneither_ptr*Cyc_Toc_curr_sp=& Cyc_Toc_curr_string;static char _tmp14[4]="tag";
static struct _dyneither_ptr Cyc_Toc_tag_string={_tmp14,_tmp14,_tmp14 + 4};static
struct _dyneither_ptr*Cyc_Toc_tag_sp=& Cyc_Toc_tag_string;static char _tmp15[4]="val";
static struct _dyneither_ptr Cyc_Toc_val_string={_tmp15,_tmp15,_tmp15 + 4};static
struct _dyneither_ptr*Cyc_Toc_val_sp=& Cyc_Toc_val_string;static char _tmp16[14]="_handler_cons";
static struct _dyneither_ptr Cyc_Toc__handler_cons_string={_tmp16,_tmp16,_tmp16 + 14};
static struct _dyneither_ptr*Cyc_Toc__handler_cons_sp=& Cyc_Toc__handler_cons_string;
static char _tmp17[8]="handler";static struct _dyneither_ptr Cyc_Toc_handler_string={
_tmp17,_tmp17,_tmp17 + 8};static struct _dyneither_ptr*Cyc_Toc_handler_sp=& Cyc_Toc_handler_string;
static char _tmp18[14]="_RegionHandle";static struct _dyneither_ptr Cyc_Toc__RegionHandle_string={
_tmp18,_tmp18,_tmp18 + 14};static struct _dyneither_ptr*Cyc_Toc__RegionHandle_sp=&
Cyc_Toc__RegionHandle_string;static char _tmp19[17]="_DynRegionHandle";static
struct _dyneither_ptr Cyc_Toc__DynRegionHandle_string={_tmp19,_tmp19,_tmp19 + 17};
static struct _dyneither_ptr*Cyc_Toc__DynRegionHandle_sp=& Cyc_Toc__DynRegionHandle_string;
static char _tmp1A[16]="_DynRegionFrame";static struct _dyneither_ptr Cyc_Toc__DynRegionFrame_string={
_tmp1A,_tmp1A,_tmp1A + 16};static struct _dyneither_ptr*Cyc_Toc__DynRegionFrame_sp=&
Cyc_Toc__DynRegionFrame_string;struct _dyneither_ptr Cyc_Toc_globals={(void*)0,(
void*)0,(void*)(0 + 0)};static char _tmp1B[7]="_throw";static struct _dyneither_ptr
Cyc_Toc__throw_str={_tmp1B,_tmp1B,_tmp1B + 7};static struct _tuple1 Cyc_Toc__throw_pr={(
union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__throw_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__throw_re={1,& Cyc_Toc__throw_pr,(void*)((
void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__throw_ev={0,(void*)((void*)& Cyc_Toc__throw_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__throw_e=&
Cyc_Toc__throw_ev;static char _tmp1E[7]="setjmp";static struct _dyneither_ptr Cyc_Toc_setjmp_str={
_tmp1E,_tmp1E,_tmp1E + 7};static struct _tuple1 Cyc_Toc_setjmp_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc_setjmp_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc_setjmp_re={1,& Cyc_Toc_setjmp_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp
Cyc_Toc_setjmp_ev={0,(void*)((void*)& Cyc_Toc_setjmp_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc_setjmp_e=& Cyc_Toc_setjmp_ev;static char _tmp21[
14]="_push_handler";static struct _dyneither_ptr Cyc_Toc__push_handler_str={_tmp21,
_tmp21,_tmp21 + 14};static struct _tuple1 Cyc_Toc__push_handler_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__push_handler_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__push_handler_re={1,& Cyc_Toc__push_handler_pr,(void*)((void*)0)};static
struct Cyc_Absyn_Exp Cyc_Toc__push_handler_ev={0,(void*)((void*)& Cyc_Toc__push_handler_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__push_handler_e=&
Cyc_Toc__push_handler_ev;static char _tmp24[13]="_pop_handler";static struct
_dyneither_ptr Cyc_Toc__pop_handler_str={_tmp24,_tmp24,_tmp24 + 13};static struct
_tuple1 Cyc_Toc__pop_handler_pr={(union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){
0}),& Cyc_Toc__pop_handler_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__pop_handler_re={
1,& Cyc_Toc__pop_handler_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__pop_handler_ev={
0,(void*)((void*)& Cyc_Toc__pop_handler_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__pop_handler_e=& Cyc_Toc__pop_handler_ev;static
char _tmp27[12]="_exn_thrown";static struct _dyneither_ptr Cyc_Toc__exn_thrown_str={
_tmp27,_tmp27,_tmp27 + 12};static struct _tuple1 Cyc_Toc__exn_thrown_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__exn_thrown_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__exn_thrown_re={1,& Cyc_Toc__exn_thrown_pr,(void*)((void*)0)};static
struct Cyc_Absyn_Exp Cyc_Toc__exn_thrown_ev={0,(void*)((void*)& Cyc_Toc__exn_thrown_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__exn_thrown_e=&
Cyc_Toc__exn_thrown_ev;static char _tmp2A[14]="_npop_handler";static struct
_dyneither_ptr Cyc_Toc__npop_handler_str={_tmp2A,_tmp2A,_tmp2A + 14};static struct
_tuple1 Cyc_Toc__npop_handler_pr={(union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){
0}),& Cyc_Toc__npop_handler_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__npop_handler_re={
1,& Cyc_Toc__npop_handler_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__npop_handler_ev={
0,(void*)((void*)& Cyc_Toc__npop_handler_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__npop_handler_e=& Cyc_Toc__npop_handler_ev;
static char _tmp2D[12]="_check_null";static struct _dyneither_ptr Cyc_Toc__check_null_str={
_tmp2D,_tmp2D,_tmp2D + 12};static struct _tuple1 Cyc_Toc__check_null_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__check_null_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__check_null_re={1,& Cyc_Toc__check_null_pr,(void*)((void*)0)};static
struct Cyc_Absyn_Exp Cyc_Toc__check_null_ev={0,(void*)((void*)& Cyc_Toc__check_null_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__check_null_e=&
Cyc_Toc__check_null_ev;static char _tmp30[28]="_check_known_subscript_null";static
struct _dyneither_ptr Cyc_Toc__check_known_subscript_null_str={_tmp30,_tmp30,
_tmp30 + 28};static struct _tuple1 Cyc_Toc__check_known_subscript_null_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__check_known_subscript_null_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__check_known_subscript_null_re={1,& Cyc_Toc__check_known_subscript_null_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__check_known_subscript_null_ev={
0,(void*)((void*)& Cyc_Toc__check_known_subscript_null_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__check_known_subscript_null_e=& Cyc_Toc__check_known_subscript_null_ev;
static char _tmp33[31]="_check_known_subscript_notnull";static struct _dyneither_ptr
Cyc_Toc__check_known_subscript_notnull_str={_tmp33,_tmp33,_tmp33 + 31};static
struct _tuple1 Cyc_Toc__check_known_subscript_notnull_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__check_known_subscript_notnull_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__check_known_subscript_notnull_re={1,&
Cyc_Toc__check_known_subscript_notnull_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp
Cyc_Toc__check_known_subscript_notnull_ev={0,(void*)((void*)& Cyc_Toc__check_known_subscript_notnull_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__check_known_subscript_notnull_e=&
Cyc_Toc__check_known_subscript_notnull_ev;static char _tmp36[27]="_check_dyneither_subscript";
static struct _dyneither_ptr Cyc_Toc__check_dyneither_subscript_str={_tmp36,_tmp36,
_tmp36 + 27};static struct _tuple1 Cyc_Toc__check_dyneither_subscript_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__check_dyneither_subscript_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__check_dyneither_subscript_re={1,& Cyc_Toc__check_dyneither_subscript_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__check_dyneither_subscript_ev={
0,(void*)((void*)& Cyc_Toc__check_dyneither_subscript_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__check_dyneither_subscript_e=& Cyc_Toc__check_dyneither_subscript_ev;
static char _tmp39[15]="_dyneither_ptr";static struct _dyneither_ptr Cyc_Toc__dyneither_ptr_str={
_tmp39,_tmp39,_tmp39 + 15};static struct _tuple1 Cyc_Toc__dyneither_ptr_pr={(union
Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__dyneither_ptr_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__dyneither_ptr_re={1,& Cyc_Toc__dyneither_ptr_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_ev={0,(void*)((
void*)& Cyc_Toc__dyneither_ptr_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static
struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_e=& Cyc_Toc__dyneither_ptr_ev;static
char _tmp3C[15]="_tag_dyneither";static struct _dyneither_ptr Cyc_Toc__tag_dyneither_str={
_tmp3C,_tmp3C,_tmp3C + 15};static struct _tuple1 Cyc_Toc__tag_dyneither_pr={(union
Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__tag_dyneither_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__tag_dyneither_re={1,& Cyc_Toc__tag_dyneither_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__tag_dyneither_ev={0,(void*)((
void*)& Cyc_Toc__tag_dyneither_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static
struct Cyc_Absyn_Exp*Cyc_Toc__tag_dyneither_e=& Cyc_Toc__tag_dyneither_ev;static
char _tmp3F[20]="_init_dyneither_ptr";static struct _dyneither_ptr Cyc_Toc__init_dyneither_ptr_str={
_tmp3F,_tmp3F,_tmp3F + 20};static struct _tuple1 Cyc_Toc__init_dyneither_ptr_pr={(
union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__init_dyneither_ptr_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__init_dyneither_ptr_re={1,& Cyc_Toc__init_dyneither_ptr_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__init_dyneither_ptr_ev={0,(
void*)((void*)& Cyc_Toc__init_dyneither_ptr_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__init_dyneither_ptr_e=& Cyc_Toc__init_dyneither_ptr_ev;
static char _tmp42[21]="_untag_dyneither_ptr";static struct _dyneither_ptr Cyc_Toc__untag_dyneither_ptr_str={
_tmp42,_tmp42,_tmp42 + 21};static struct _tuple1 Cyc_Toc__untag_dyneither_ptr_pr={(
union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__untag_dyneither_ptr_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__untag_dyneither_ptr_re={1,& Cyc_Toc__untag_dyneither_ptr_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__untag_dyneither_ptr_ev={0,(
void*)((void*)& Cyc_Toc__untag_dyneither_ptr_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__untag_dyneither_ptr_e=& Cyc_Toc__untag_dyneither_ptr_ev;
static char _tmp45[20]="_get_dyneither_size";static struct _dyneither_ptr Cyc_Toc__get_dyneither_size_str={
_tmp45,_tmp45,_tmp45 + 20};static struct _tuple1 Cyc_Toc__get_dyneither_size_pr={(
union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__get_dyneither_size_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__get_dyneither_size_re={1,& Cyc_Toc__get_dyneither_size_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__get_dyneither_size_ev={0,(
void*)((void*)& Cyc_Toc__get_dyneither_size_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__get_dyneither_size_e=& Cyc_Toc__get_dyneither_size_ev;
static char _tmp48[19]="_get_zero_arr_size";static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_str={
_tmp48,_tmp48,_tmp48 + 19};static struct _tuple1 Cyc_Toc__get_zero_arr_size_pr={(
union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__get_zero_arr_size_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__get_zero_arr_size_re={1,& Cyc_Toc__get_zero_arr_size_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_ev={0,(
void*)((void*)& Cyc_Toc__get_zero_arr_size_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_e=& Cyc_Toc__get_zero_arr_size_ev;
static char _tmp4B[24]="_get_zero_arr_size_char";static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_char_str={
_tmp4B,_tmp4B,_tmp4B + 24};static struct _tuple1 Cyc_Toc__get_zero_arr_size_char_pr={(
union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__get_zero_arr_size_char_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__get_zero_arr_size_char_re={1,& Cyc_Toc__get_zero_arr_size_char_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_char_ev={0,(
void*)((void*)& Cyc_Toc__get_zero_arr_size_char_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_char_e=& Cyc_Toc__get_zero_arr_size_char_ev;
static char _tmp4E[25]="_get_zero_arr_size_short";static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_short_str={
_tmp4E,_tmp4E,_tmp4E + 25};static struct _tuple1 Cyc_Toc__get_zero_arr_size_short_pr={(
union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__get_zero_arr_size_short_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__get_zero_arr_size_short_re={1,& Cyc_Toc__get_zero_arr_size_short_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_short_ev={
0,(void*)((void*)& Cyc_Toc__get_zero_arr_size_short_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_short_e=& Cyc_Toc__get_zero_arr_size_short_ev;
static char _tmp51[23]="_get_zero_arr_size_int";static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_int_str={
_tmp51,_tmp51,_tmp51 + 23};static struct _tuple1 Cyc_Toc__get_zero_arr_size_int_pr={(
union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__get_zero_arr_size_int_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__get_zero_arr_size_int_re={1,& Cyc_Toc__get_zero_arr_size_int_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_int_ev={0,(
void*)((void*)& Cyc_Toc__get_zero_arr_size_int_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_int_e=& Cyc_Toc__get_zero_arr_size_int_ev;
static char _tmp54[25]="_get_zero_arr_size_float";static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_float_str={
_tmp54,_tmp54,_tmp54 + 25};static struct _tuple1 Cyc_Toc__get_zero_arr_size_float_pr={(
union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__get_zero_arr_size_float_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__get_zero_arr_size_float_re={1,& Cyc_Toc__get_zero_arr_size_float_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_float_ev={
0,(void*)((void*)& Cyc_Toc__get_zero_arr_size_float_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_float_e=& Cyc_Toc__get_zero_arr_size_float_ev;
static char _tmp57[26]="_get_zero_arr_size_double";static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_double_str={
_tmp57,_tmp57,_tmp57 + 26};static struct _tuple1 Cyc_Toc__get_zero_arr_size_double_pr={(
union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__get_zero_arr_size_double_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__get_zero_arr_size_double_re={1,& Cyc_Toc__get_zero_arr_size_double_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_double_ev={
0,(void*)((void*)& Cyc_Toc__get_zero_arr_size_double_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_double_e=& Cyc_Toc__get_zero_arr_size_double_ev;
static char _tmp5A[30]="_get_zero_arr_size_longdouble";static struct _dyneither_ptr
Cyc_Toc__get_zero_arr_size_longdouble_str={_tmp5A,_tmp5A,_tmp5A + 30};static
struct _tuple1 Cyc_Toc__get_zero_arr_size_longdouble_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__get_zero_arr_size_longdouble_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__get_zero_arr_size_longdouble_re={1,&
Cyc_Toc__get_zero_arr_size_longdouble_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp
Cyc_Toc__get_zero_arr_size_longdouble_ev={0,(void*)((void*)& Cyc_Toc__get_zero_arr_size_longdouble_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_longdouble_e=&
Cyc_Toc__get_zero_arr_size_longdouble_ev;static char _tmp5D[28]="_get_zero_arr_size_voidstar";
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_voidstar_str={_tmp5D,_tmp5D,
_tmp5D + 28};static struct _tuple1 Cyc_Toc__get_zero_arr_size_voidstar_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__get_zero_arr_size_voidstar_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__get_zero_arr_size_voidstar_re={1,& Cyc_Toc__get_zero_arr_size_voidstar_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_voidstar_ev={
0,(void*)((void*)& Cyc_Toc__get_zero_arr_size_voidstar_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_voidstar_e=& Cyc_Toc__get_zero_arr_size_voidstar_ev;
static char _tmp60[20]="_dyneither_ptr_plus";static struct _dyneither_ptr Cyc_Toc__dyneither_ptr_plus_str={
_tmp60,_tmp60,_tmp60 + 20};static struct _tuple1 Cyc_Toc__dyneither_ptr_plus_pr={(
union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__dyneither_ptr_plus_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__dyneither_ptr_plus_re={1,& Cyc_Toc__dyneither_ptr_plus_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_plus_ev={0,(
void*)((void*)& Cyc_Toc__dyneither_ptr_plus_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_plus_e=& Cyc_Toc__dyneither_ptr_plus_ev;
static char _tmp63[15]="_zero_arr_plus";static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_str={
_tmp63,_tmp63,_tmp63 + 15};static struct _tuple1 Cyc_Toc__zero_arr_plus_pr={(union
Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__zero_arr_plus_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_plus_re={1,& Cyc_Toc__zero_arr_plus_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_ev={0,(void*)((
void*)& Cyc_Toc__zero_arr_plus_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static
struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_e=& Cyc_Toc__zero_arr_plus_ev;static
char _tmp66[20]="_zero_arr_plus_char";static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_char_str={
_tmp66,_tmp66,_tmp66 + 20};static struct _tuple1 Cyc_Toc__zero_arr_plus_char_pr={(
union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__zero_arr_plus_char_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_plus_char_re={1,& Cyc_Toc__zero_arr_plus_char_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_char_ev={0,(
void*)((void*)& Cyc_Toc__zero_arr_plus_char_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_char_e=& Cyc_Toc__zero_arr_plus_char_ev;
static char _tmp69[21]="_zero_arr_plus_short";static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_short_str={
_tmp69,_tmp69,_tmp69 + 21};static struct _tuple1 Cyc_Toc__zero_arr_plus_short_pr={(
union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__zero_arr_plus_short_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_plus_short_re={1,& Cyc_Toc__zero_arr_plus_short_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_short_ev={0,(
void*)((void*)& Cyc_Toc__zero_arr_plus_short_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_short_e=& Cyc_Toc__zero_arr_plus_short_ev;
static char _tmp6C[19]="_zero_arr_plus_int";static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_int_str={
_tmp6C,_tmp6C,_tmp6C + 19};static struct _tuple1 Cyc_Toc__zero_arr_plus_int_pr={(
union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__zero_arr_plus_int_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_plus_int_re={1,& Cyc_Toc__zero_arr_plus_int_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_int_ev={0,(
void*)((void*)& Cyc_Toc__zero_arr_plus_int_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_int_e=& Cyc_Toc__zero_arr_plus_int_ev;
static char _tmp6F[21]="_zero_arr_plus_float";static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_float_str={
_tmp6F,_tmp6F,_tmp6F + 21};static struct _tuple1 Cyc_Toc__zero_arr_plus_float_pr={(
union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__zero_arr_plus_float_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_plus_float_re={1,& Cyc_Toc__zero_arr_plus_float_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_float_ev={0,(
void*)((void*)& Cyc_Toc__zero_arr_plus_float_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_float_e=& Cyc_Toc__zero_arr_plus_float_ev;
static char _tmp72[22]="_zero_arr_plus_double";static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_double_str={
_tmp72,_tmp72,_tmp72 + 22};static struct _tuple1 Cyc_Toc__zero_arr_plus_double_pr={(
union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__zero_arr_plus_double_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_plus_double_re={1,& Cyc_Toc__zero_arr_plus_double_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_double_ev={0,(
void*)((void*)& Cyc_Toc__zero_arr_plus_double_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_double_e=& Cyc_Toc__zero_arr_plus_double_ev;
static char _tmp75[26]="_zero_arr_plus_longdouble";static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_longdouble_str={
_tmp75,_tmp75,_tmp75 + 26};static struct _tuple1 Cyc_Toc__zero_arr_plus_longdouble_pr={(
union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__zero_arr_plus_longdouble_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_plus_longdouble_re={1,& Cyc_Toc__zero_arr_plus_longdouble_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_longdouble_ev={
0,(void*)((void*)& Cyc_Toc__zero_arr_plus_longdouble_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_longdouble_e=& Cyc_Toc__zero_arr_plus_longdouble_ev;
static char _tmp78[24]="_zero_arr_plus_voidstar";static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_voidstar_str={
_tmp78,_tmp78,_tmp78 + 24};static struct _tuple1 Cyc_Toc__zero_arr_plus_voidstar_pr={(
union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__zero_arr_plus_voidstar_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_plus_voidstar_re={1,& Cyc_Toc__zero_arr_plus_voidstar_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_voidstar_ev={0,(
void*)((void*)& Cyc_Toc__zero_arr_plus_voidstar_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_voidstar_e=& Cyc_Toc__zero_arr_plus_voidstar_ev;
static char _tmp7B[28]="_dyneither_ptr_inplace_plus";static struct _dyneither_ptr Cyc_Toc__dyneither_ptr_inplace_plus_str={
_tmp7B,_tmp7B,_tmp7B + 28};static struct _tuple1 Cyc_Toc__dyneither_ptr_inplace_plus_pr={(
union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__dyneither_ptr_inplace_plus_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__dyneither_ptr_inplace_plus_re={1,& Cyc_Toc__dyneither_ptr_inplace_plus_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_inplace_plus_ev={
0,(void*)((void*)& Cyc_Toc__dyneither_ptr_inplace_plus_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_inplace_plus_e=& Cyc_Toc__dyneither_ptr_inplace_plus_ev;
static char _tmp7E[23]="_zero_arr_inplace_plus";static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_str={
_tmp7E,_tmp7E,_tmp7E + 23};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_pr={(
union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__zero_arr_inplace_plus_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_inplace_plus_re={1,& Cyc_Toc__zero_arr_inplace_plus_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_ev={0,(
void*)((void*)& Cyc_Toc__zero_arr_inplace_plus_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_e=& Cyc_Toc__zero_arr_inplace_plus_ev;
static char _tmp81[28]="_zero_arr_inplace_plus_char";static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_char_str={
_tmp81,_tmp81,_tmp81 + 28};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_char_pr={(
union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__zero_arr_inplace_plus_char_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_inplace_plus_char_re={1,& Cyc_Toc__zero_arr_inplace_plus_char_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_char_ev={
0,(void*)((void*)& Cyc_Toc__zero_arr_inplace_plus_char_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_char_e=& Cyc_Toc__zero_arr_inplace_plus_char_ev;
static char _tmp84[29]="_zero_arr_inplace_plus_short";static struct _dyneither_ptr
Cyc_Toc__zero_arr_inplace_plus_short_str={_tmp84,_tmp84,_tmp84 + 29};static struct
_tuple1 Cyc_Toc__zero_arr_inplace_plus_short_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__zero_arr_inplace_plus_short_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_inplace_plus_short_re={1,&
Cyc_Toc__zero_arr_inplace_plus_short_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp
Cyc_Toc__zero_arr_inplace_plus_short_ev={0,(void*)((void*)& Cyc_Toc__zero_arr_inplace_plus_short_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_short_e=&
Cyc_Toc__zero_arr_inplace_plus_short_ev;static char _tmp87[27]="_zero_arr_inplace_plus_int";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_int_str={_tmp87,_tmp87,
_tmp87 + 27};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_int_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__zero_arr_inplace_plus_int_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_inplace_plus_int_re={1,& Cyc_Toc__zero_arr_inplace_plus_int_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_int_ev={
0,(void*)((void*)& Cyc_Toc__zero_arr_inplace_plus_int_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_int_e=& Cyc_Toc__zero_arr_inplace_plus_int_ev;
static char _tmp8A[29]="_zero_arr_inplace_plus_float";static struct _dyneither_ptr
Cyc_Toc__zero_arr_inplace_plus_float_str={_tmp8A,_tmp8A,_tmp8A + 29};static struct
_tuple1 Cyc_Toc__zero_arr_inplace_plus_float_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__zero_arr_inplace_plus_float_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_inplace_plus_float_re={1,&
Cyc_Toc__zero_arr_inplace_plus_float_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp
Cyc_Toc__zero_arr_inplace_plus_float_ev={0,(void*)((void*)& Cyc_Toc__zero_arr_inplace_plus_float_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_float_e=&
Cyc_Toc__zero_arr_inplace_plus_float_ev;static char _tmp8D[30]="_zero_arr_inplace_plus_double";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_double_str={_tmp8D,
_tmp8D,_tmp8D + 30};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_double_pr={(
union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__zero_arr_inplace_plus_double_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_inplace_plus_double_re={1,&
Cyc_Toc__zero_arr_inplace_plus_double_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp
Cyc_Toc__zero_arr_inplace_plus_double_ev={0,(void*)((void*)& Cyc_Toc__zero_arr_inplace_plus_double_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_double_e=&
Cyc_Toc__zero_arr_inplace_plus_double_ev;static char _tmp90[34]="_zero_arr_inplace_plus_longdouble";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_longdouble_str={_tmp90,
_tmp90,_tmp90 + 34};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_longdouble_pr={(
union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__zero_arr_inplace_plus_longdouble_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_inplace_plus_longdouble_re={
1,& Cyc_Toc__zero_arr_inplace_plus_longdouble_pr,(void*)((void*)0)};static struct
Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_longdouble_ev={0,(void*)((void*)& Cyc_Toc__zero_arr_inplace_plus_longdouble_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_longdouble_e=&
Cyc_Toc__zero_arr_inplace_plus_longdouble_ev;static char _tmp93[32]="_zero_arr_inplace_plus_voidstar";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_voidstar_str={_tmp93,
_tmp93,_tmp93 + 32};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_voidstar_pr={(
union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__zero_arr_inplace_plus_voidstar_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_inplace_plus_voidstar_re={1,&
Cyc_Toc__zero_arr_inplace_plus_voidstar_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp
Cyc_Toc__zero_arr_inplace_plus_voidstar_ev={0,(void*)((void*)& Cyc_Toc__zero_arr_inplace_plus_voidstar_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_voidstar_e=&
Cyc_Toc__zero_arr_inplace_plus_voidstar_ev;static char _tmp96[33]="_dyneither_ptr_inplace_plus_post";
static struct _dyneither_ptr Cyc_Toc__dyneither_ptr_inplace_plus_post_str={_tmp96,
_tmp96,_tmp96 + 33};static struct _tuple1 Cyc_Toc__dyneither_ptr_inplace_plus_post_pr={(
union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__dyneither_ptr_inplace_plus_post_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__dyneither_ptr_inplace_plus_post_re={1,&
Cyc_Toc__dyneither_ptr_inplace_plus_post_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp
Cyc_Toc__dyneither_ptr_inplace_plus_post_ev={0,(void*)((void*)& Cyc_Toc__dyneither_ptr_inplace_plus_post_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_inplace_plus_post_e=&
Cyc_Toc__dyneither_ptr_inplace_plus_post_ev;static char _tmp99[28]="_zero_arr_inplace_plus_post";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_str={_tmp99,_tmp99,
_tmp99 + 28};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__zero_arr_inplace_plus_post_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_inplace_plus_post_re={1,& Cyc_Toc__zero_arr_inplace_plus_post_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_ev={
0,(void*)((void*)& Cyc_Toc__zero_arr_inplace_plus_post_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_e=& Cyc_Toc__zero_arr_inplace_plus_post_ev;
static char _tmp9C[33]="_zero_arr_inplace_plus_post_char";static struct
_dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_char_str={_tmp9C,_tmp9C,_tmp9C
+ 33};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_char_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__zero_arr_inplace_plus_post_char_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_inplace_plus_post_char_re={1,&
Cyc_Toc__zero_arr_inplace_plus_post_char_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp
Cyc_Toc__zero_arr_inplace_plus_post_char_ev={0,(void*)((void*)& Cyc_Toc__zero_arr_inplace_plus_post_char_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_char_e=&
Cyc_Toc__zero_arr_inplace_plus_post_char_ev;static char _tmp9F[34]="_zero_arr_inplace_plus_post_short";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_short_str={_tmp9F,
_tmp9F,_tmp9F + 34};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_short_pr={(
union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__zero_arr_inplace_plus_post_short_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_inplace_plus_post_short_re={
1,& Cyc_Toc__zero_arr_inplace_plus_post_short_pr,(void*)((void*)0)};static struct
Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_short_ev={0,(void*)((void*)& Cyc_Toc__zero_arr_inplace_plus_post_short_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_short_e=&
Cyc_Toc__zero_arr_inplace_plus_post_short_ev;static char _tmpA2[32]="_zero_arr_inplace_plus_post_int";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_int_str={_tmpA2,
_tmpA2,_tmpA2 + 32};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_int_pr={(
union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__zero_arr_inplace_plus_post_int_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_inplace_plus_post_int_re={1,&
Cyc_Toc__zero_arr_inplace_plus_post_int_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp
Cyc_Toc__zero_arr_inplace_plus_post_int_ev={0,(void*)((void*)& Cyc_Toc__zero_arr_inplace_plus_post_int_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_int_e=&
Cyc_Toc__zero_arr_inplace_plus_post_int_ev;static char _tmpA5[34]="_zero_arr_inplace_plus_post_float";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_float_str={_tmpA5,
_tmpA5,_tmpA5 + 34};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_float_pr={(
union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__zero_arr_inplace_plus_post_float_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_inplace_plus_post_float_re={
1,& Cyc_Toc__zero_arr_inplace_plus_post_float_pr,(void*)((void*)0)};static struct
Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_float_ev={0,(void*)((void*)& Cyc_Toc__zero_arr_inplace_plus_post_float_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_float_e=&
Cyc_Toc__zero_arr_inplace_plus_post_float_ev;static char _tmpA8[35]="_zero_arr_inplace_plus_post_double";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_double_str={_tmpA8,
_tmpA8,_tmpA8 + 35};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_double_pr={(
union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__zero_arr_inplace_plus_post_double_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_inplace_plus_post_double_re={
1,& Cyc_Toc__zero_arr_inplace_plus_post_double_pr,(void*)((void*)0)};static struct
Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_double_ev={0,(void*)((void*)&
Cyc_Toc__zero_arr_inplace_plus_post_double_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_double_e=& Cyc_Toc__zero_arr_inplace_plus_post_double_ev;
static char _tmpAB[39]="_zero_arr_inplace_plus_post_longdouble";static struct
_dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_longdouble_str={_tmpAB,_tmpAB,
_tmpAB + 39};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_longdouble_pr={(
union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_inplace_plus_post_longdouble_re={
1,& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_pr,(void*)((void*)0)};static
struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_longdouble_ev={0,(void*)((
void*)& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_longdouble_e=& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_ev;
static char _tmpAE[37]="_zero_arr_inplace_plus_post_voidstar";static struct
_dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_voidstar_str={_tmpAE,_tmpAE,
_tmpAE + 37};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_voidstar_pr={(
union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_inplace_plus_post_voidstar_re={
1,& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_pr,(void*)((void*)0)};static
struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_voidstar_ev={0,(void*)((
void*)& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_voidstar_e=& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_ev;
static char _tmpB1[10]="_cycalloc";static struct _dyneither_ptr Cyc_Toc__cycalloc_str={
_tmpB1,_tmpB1,_tmpB1 + 10};static struct _tuple1 Cyc_Toc__cycalloc_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__cycalloc_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__cycalloc_re={1,& Cyc_Toc__cycalloc_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp
Cyc_Toc__cycalloc_ev={0,(void*)((void*)& Cyc_Toc__cycalloc_re),0,(void*)((void*)
Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__cycalloc_e=& Cyc_Toc__cycalloc_ev;
static char _tmpB4[11]="_cyccalloc";static struct _dyneither_ptr Cyc_Toc__cyccalloc_str={
_tmpB4,_tmpB4,_tmpB4 + 11};static struct _tuple1 Cyc_Toc__cyccalloc_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__cyccalloc_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__cyccalloc_re={1,& Cyc_Toc__cyccalloc_pr,(void*)((void*)0)};static struct
Cyc_Absyn_Exp Cyc_Toc__cyccalloc_ev={0,(void*)((void*)& Cyc_Toc__cyccalloc_re),0,(
void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__cyccalloc_e=&
Cyc_Toc__cyccalloc_ev;static char _tmpB7[17]="_cycalloc_atomic";static struct
_dyneither_ptr Cyc_Toc__cycalloc_atomic_str={_tmpB7,_tmpB7,_tmpB7 + 17};static
struct _tuple1 Cyc_Toc__cycalloc_atomic_pr={(union Cyc_Absyn_Nmspace_union)((struct
Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__cycalloc_atomic_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__cycalloc_atomic_re={1,& Cyc_Toc__cycalloc_atomic_pr,(void*)((void*)0)};
static struct Cyc_Absyn_Exp Cyc_Toc__cycalloc_atomic_ev={0,(void*)((void*)& Cyc_Toc__cycalloc_atomic_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__cycalloc_atomic_e=&
Cyc_Toc__cycalloc_atomic_ev;static char _tmpBA[18]="_cyccalloc_atomic";static
struct _dyneither_ptr Cyc_Toc__cyccalloc_atomic_str={_tmpBA,_tmpBA,_tmpBA + 18};
static struct _tuple1 Cyc_Toc__cyccalloc_atomic_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__cyccalloc_atomic_str};static struct
Cyc_Absyn_Var_e_struct Cyc_Toc__cyccalloc_atomic_re={1,& Cyc_Toc__cyccalloc_atomic_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__cyccalloc_atomic_ev={0,(void*)((
void*)& Cyc_Toc__cyccalloc_atomic_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__cyccalloc_atomic_e=& Cyc_Toc__cyccalloc_atomic_ev;
static char _tmpBD[15]="_region_malloc";static struct _dyneither_ptr Cyc_Toc__region_malloc_str={
_tmpBD,_tmpBD,_tmpBD + 15};static struct _tuple1 Cyc_Toc__region_malloc_pr={(union
Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__region_malloc_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__region_malloc_re={1,& Cyc_Toc__region_malloc_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__region_malloc_ev={0,(void*)((
void*)& Cyc_Toc__region_malloc_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static
struct Cyc_Absyn_Exp*Cyc_Toc__region_malloc_e=& Cyc_Toc__region_malloc_ev;static
char _tmpC0[15]="_region_calloc";static struct _dyneither_ptr Cyc_Toc__region_calloc_str={
_tmpC0,_tmpC0,_tmpC0 + 15};static struct _tuple1 Cyc_Toc__region_calloc_pr={(union
Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__region_calloc_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__region_calloc_re={1,& Cyc_Toc__region_calloc_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__region_calloc_ev={0,(void*)((
void*)& Cyc_Toc__region_calloc_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static
struct Cyc_Absyn_Exp*Cyc_Toc__region_calloc_e=& Cyc_Toc__region_calloc_ev;static
char _tmpC3[13]="_check_times";static struct _dyneither_ptr Cyc_Toc__check_times_str={
_tmpC3,_tmpC3,_tmpC3 + 13};static struct _tuple1 Cyc_Toc__check_times_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__check_times_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__check_times_re={1,& Cyc_Toc__check_times_pr,(void*)((void*)0)};static
struct Cyc_Absyn_Exp Cyc_Toc__check_times_ev={0,(void*)((void*)& Cyc_Toc__check_times_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__check_times_e=&
Cyc_Toc__check_times_ev;static char _tmpC6[12]="_new_region";static struct
_dyneither_ptr Cyc_Toc__new_region_str={_tmpC6,_tmpC6,_tmpC6 + 12};static struct
_tuple1 Cyc_Toc__new_region_pr={(union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){
0}),& Cyc_Toc__new_region_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__new_region_re={
1,& Cyc_Toc__new_region_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__new_region_ev={
0,(void*)((void*)& Cyc_Toc__new_region_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__new_region_e=& Cyc_Toc__new_region_ev;static
char _tmpC9[13]="_push_region";static struct _dyneither_ptr Cyc_Toc__push_region_str={
_tmpC9,_tmpC9,_tmpC9 + 13};static struct _tuple1 Cyc_Toc__push_region_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__push_region_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__push_region_re={1,& Cyc_Toc__push_region_pr,(void*)((void*)0)};static
struct Cyc_Absyn_Exp Cyc_Toc__push_region_ev={0,(void*)((void*)& Cyc_Toc__push_region_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__push_region_e=&
Cyc_Toc__push_region_ev;static char _tmpCC[12]="_pop_region";static struct
_dyneither_ptr Cyc_Toc__pop_region_str={_tmpCC,_tmpCC,_tmpCC + 12};static struct
_tuple1 Cyc_Toc__pop_region_pr={(union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){
0}),& Cyc_Toc__pop_region_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__pop_region_re={
1,& Cyc_Toc__pop_region_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__pop_region_ev={
0,(void*)((void*)& Cyc_Toc__pop_region_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__pop_region_e=& Cyc_Toc__pop_region_ev;static
char _tmpCF[16]="_open_dynregion";static struct _dyneither_ptr Cyc_Toc__open_dynregion_str={
_tmpCF,_tmpCF,_tmpCF + 16};static struct _tuple1 Cyc_Toc__open_dynregion_pr={(union
Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__open_dynregion_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__open_dynregion_re={1,& Cyc_Toc__open_dynregion_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__open_dynregion_ev={0,(void*)((
void*)& Cyc_Toc__open_dynregion_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__open_dynregion_e=& Cyc_Toc__open_dynregion_ev;
static char _tmpD2[16]="_push_dynregion";static struct _dyneither_ptr Cyc_Toc__push_dynregion_str={
_tmpD2,_tmpD2,_tmpD2 + 16};static struct _tuple1 Cyc_Toc__push_dynregion_pr={(union
Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__push_dynregion_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__push_dynregion_re={1,& Cyc_Toc__push_dynregion_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__push_dynregion_ev={0,(void*)((
void*)& Cyc_Toc__push_dynregion_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__push_dynregion_e=& Cyc_Toc__push_dynregion_ev;
static char _tmpD5[15]="_pop_dynregion";static struct _dyneither_ptr Cyc_Toc__pop_dynregion_str={
_tmpD5,_tmpD5,_tmpD5 + 15};static struct _tuple1 Cyc_Toc__pop_dynregion_pr={(union
Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__pop_dynregion_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__pop_dynregion_re={1,& Cyc_Toc__pop_dynregion_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__pop_dynregion_ev={0,(void*)((
void*)& Cyc_Toc__pop_dynregion_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static
struct Cyc_Absyn_Exp*Cyc_Toc__pop_dynregion_e=& Cyc_Toc__pop_dynregion_ev;static
char _tmpD8[14]="_reset_region";static struct _dyneither_ptr Cyc_Toc__reset_region_str={
_tmpD8,_tmpD8,_tmpD8 + 14};static struct _tuple1 Cyc_Toc__reset_region_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__reset_region_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__reset_region_re={1,& Cyc_Toc__reset_region_pr,(void*)((void*)0)};static
struct Cyc_Absyn_Exp Cyc_Toc__reset_region_ev={0,(void*)((void*)& Cyc_Toc__reset_region_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__reset_region_e=&
Cyc_Toc__reset_region_ev;static char _tmpDB[19]="_throw_arraybounds";static struct
_dyneither_ptr Cyc_Toc__throw_arraybounds_str={_tmpDB,_tmpDB,_tmpDB + 19};static
struct _tuple1 Cyc_Toc__throw_arraybounds_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__throw_arraybounds_str};static struct
Cyc_Absyn_Var_e_struct Cyc_Toc__throw_arraybounds_re={1,& Cyc_Toc__throw_arraybounds_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__throw_arraybounds_ev={0,(
void*)((void*)& Cyc_Toc__throw_arraybounds_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__throw_arraybounds_e=& Cyc_Toc__throw_arraybounds_ev;
static char _tmpDE[29]="_dyneither_ptr_decrease_size";static struct _dyneither_ptr
Cyc_Toc__dyneither_ptr_decrease_size_str={_tmpDE,_tmpDE,_tmpDE + 29};static struct
_tuple1 Cyc_Toc__dyneither_ptr_decrease_size_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__dyneither_ptr_decrease_size_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__dyneither_ptr_decrease_size_re={1,&
Cyc_Toc__dyneither_ptr_decrease_size_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp
Cyc_Toc__dyneither_ptr_decrease_size_ev={0,(void*)((void*)& Cyc_Toc__dyneither_ptr_decrease_size_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_decrease_size_e=&
Cyc_Toc__dyneither_ptr_decrease_size_ev;static char _tmpE1[11]="_swap_word";static
struct _dyneither_ptr Cyc_Toc__swap_word_str={_tmpE1,_tmpE1,_tmpE1 + 11};static
struct _tuple1 Cyc_Toc__swap_word_pr={(union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){
0}),& Cyc_Toc__swap_word_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__swap_word_re={
1,& Cyc_Toc__swap_word_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__swap_word_ev={
0,(void*)((void*)& Cyc_Toc__swap_word_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__swap_word_e=& Cyc_Toc__swap_word_ev;static char
_tmpE4[16]="_swap_dyneither";static struct _dyneither_ptr Cyc_Toc__swap_dyneither_str={
_tmpE4,_tmpE4,_tmpE4 + 16};static struct _tuple1 Cyc_Toc__swap_dyneither_pr={(union
Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__swap_dyneither_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__swap_dyneither_re={1,& Cyc_Toc__swap_dyneither_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__swap_dyneither_ev={0,(void*)((
void*)& Cyc_Toc__swap_dyneither_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__swap_dyneither_e=& Cyc_Toc__swap_dyneither_ev;
static struct Cyc_Absyn_AggrType_struct Cyc_Toc_dyneither_ptr_typ_v={10,{(union Cyc_Absyn_AggrInfoU_union)((
struct Cyc_Absyn_UnknownAggr_struct){0,(void*)((void*)0),& Cyc_Toc__dyneither_ptr_pr}),
0}};static void*Cyc_Toc_dyneither_ptr_typ=(void*)& Cyc_Toc_dyneither_ptr_typ_v;
static struct Cyc_Absyn_Tqual Cyc_Toc_mt_tq={0,0,0,0,0};static struct Cyc_Absyn_Stmt*
Cyc_Toc_skip_stmt_dl(){static struct Cyc_Absyn_Stmt**skip_stmt_opt=0;if(
skip_stmt_opt == 0)skip_stmt_opt=({struct Cyc_Absyn_Stmt**_tmpE9=_cycalloc(sizeof(*
_tmpE9));_tmpE9[0]=Cyc_Absyn_skip_stmt(0);_tmpE9;});return*skip_stmt_opt;}static
struct Cyc_Absyn_Exp*Cyc_Toc_cast_it(void*t,struct Cyc_Absyn_Exp*e){return Cyc_Absyn_cast_exp(
t,e,0,(void*)1,0);}static void*Cyc_Toc_cast_it_r(void*t,struct Cyc_Absyn_Exp*e){
return(void*)({struct Cyc_Absyn_Cast_e_struct*_tmpEA=_cycalloc(sizeof(*_tmpEA));
_tmpEA[0]=({struct Cyc_Absyn_Cast_e_struct _tmpEB;_tmpEB.tag=15;_tmpEB.f1=(void*)t;
_tmpEB.f2=e;_tmpEB.f3=0;_tmpEB.f4=(void*)((void*)1);_tmpEB;});_tmpEA;});}static
void*Cyc_Toc_deref_exp_r(struct Cyc_Absyn_Exp*e){return(void*)({struct Cyc_Absyn_Deref_e_struct*
_tmpEC=_cycalloc(sizeof(*_tmpEC));_tmpEC[0]=({struct Cyc_Absyn_Deref_e_struct
_tmpED;_tmpED.tag=22;_tmpED.f1=e;_tmpED;});_tmpEC;});}static void*Cyc_Toc_subscript_exp_r(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){return(void*)({struct Cyc_Absyn_Subscript_e_struct*
_tmpEE=_cycalloc(sizeof(*_tmpEE));_tmpEE[0]=({struct Cyc_Absyn_Subscript_e_struct
_tmpEF;_tmpEF.tag=25;_tmpEF.f1=e1;_tmpEF.f2=e2;_tmpEF;});_tmpEE;});}static void*
Cyc_Toc_stmt_exp_r(struct Cyc_Absyn_Stmt*s){return(void*)({struct Cyc_Absyn_StmtExp_e_struct*
_tmpF0=_cycalloc(sizeof(*_tmpF0));_tmpF0[0]=({struct Cyc_Absyn_StmtExp_e_struct
_tmpF1;_tmpF1.tag=38;_tmpF1.f1=s;_tmpF1;});_tmpF0;});}static void*Cyc_Toc_sizeoftyp_exp_r(
void*t){return(void*)({struct Cyc_Absyn_Sizeoftyp_e_struct*_tmpF2=_cycalloc(
sizeof(*_tmpF2));_tmpF2[0]=({struct Cyc_Absyn_Sizeoftyp_e_struct _tmpF3;_tmpF3.tag=
18;_tmpF3.f1=(void*)t;_tmpF3;});_tmpF2;});}static void*Cyc_Toc_fncall_exp_r(
struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es){return(void*)({struct Cyc_Absyn_FnCall_e_struct*
_tmpF4=_cycalloc(sizeof(*_tmpF4));_tmpF4[0]=({struct Cyc_Absyn_FnCall_e_struct
_tmpF5;_tmpF5.tag=11;_tmpF5.f1=e;_tmpF5.f2=es;_tmpF5.f3=0;_tmpF5;});_tmpF4;});}
static void*Cyc_Toc_exp_stmt_r(struct Cyc_Absyn_Exp*e){return(void*)({struct Cyc_Absyn_Exp_s_struct*
_tmpF6=_cycalloc(sizeof(*_tmpF6));_tmpF6[0]=({struct Cyc_Absyn_Exp_s_struct _tmpF7;
_tmpF7.tag=0;_tmpF7.f1=e;_tmpF7;});_tmpF6;});}static void*Cyc_Toc_seq_stmt_r(
struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2){return(void*)({struct Cyc_Absyn_Seq_s_struct*
_tmpF8=_cycalloc(sizeof(*_tmpF8));_tmpF8[0]=({struct Cyc_Absyn_Seq_s_struct _tmpF9;
_tmpF9.tag=1;_tmpF9.f1=s1;_tmpF9.f2=s2;_tmpF9;});_tmpF8;});}static void*Cyc_Toc_conditional_exp_r(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3){return(
void*)({struct Cyc_Absyn_Conditional_e_struct*_tmpFA=_cycalloc(sizeof(*_tmpFA));
_tmpFA[0]=({struct Cyc_Absyn_Conditional_e_struct _tmpFB;_tmpFB.tag=6;_tmpFB.f1=e1;
_tmpFB.f2=e2;_tmpFB.f3=e3;_tmpFB;});_tmpFA;});}static void*Cyc_Toc_aggrmember_exp_r(
struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n){return(void*)({struct Cyc_Absyn_AggrMember_e_struct*
_tmpFC=_cycalloc(sizeof(*_tmpFC));_tmpFC[0]=({struct Cyc_Absyn_AggrMember_e_struct
_tmpFD;_tmpFD.tag=23;_tmpFD.f1=e;_tmpFD.f2=n;_tmpFD;});_tmpFC;});}static void*Cyc_Toc_aggrarrow_exp_r(
struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n){return(void*)({struct Cyc_Absyn_AggrArrow_e_struct*
_tmpFE=_cycalloc(sizeof(*_tmpFE));_tmpFE[0]=({struct Cyc_Absyn_AggrArrow_e_struct
_tmpFF;_tmpFF.tag=24;_tmpFF.f1=e;_tmpFF.f2=n;_tmpFF;});_tmpFE;});}static void*Cyc_Toc_unresolvedmem_exp_r(
struct Cyc_Core_Opt*tdopt,struct Cyc_List_List*ds){return(void*)({struct Cyc_Absyn_UnresolvedMem_e_struct*
_tmp100=_cycalloc(sizeof(*_tmp100));_tmp100[0]=({struct Cyc_Absyn_UnresolvedMem_e_struct
_tmp101;_tmp101.tag=37;_tmp101.f1=tdopt;_tmp101.f2=ds;_tmp101;});_tmp100;});}
static void*Cyc_Toc_goto_stmt_r(struct _dyneither_ptr*v,struct Cyc_Absyn_Stmt*s){
return(void*)({struct Cyc_Absyn_Goto_s_struct*_tmp102=_cycalloc(sizeof(*_tmp102));
_tmp102[0]=({struct Cyc_Absyn_Goto_s_struct _tmp103;_tmp103.tag=7;_tmp103.f1=v;
_tmp103.f2=s;_tmp103;});_tmp102;});}static struct Cyc_Absyn_Const_e_struct Cyc_Toc_zero_exp={
0,(union Cyc_Absyn_Cnst_union)((struct Cyc_Absyn_Int_c_struct){2,(void*)((void*)0),
0})};struct Cyc_Toc_functionSet{struct Cyc_Absyn_Exp*fchar;struct Cyc_Absyn_Exp*
fshort;struct Cyc_Absyn_Exp*fint;struct Cyc_Absyn_Exp*ffloat;struct Cyc_Absyn_Exp*
fdouble;struct Cyc_Absyn_Exp*flongdouble;struct Cyc_Absyn_Exp*fvoidstar;};struct
Cyc_Toc_functionSet Cyc_Toc__zero_arr_plus_functionSet={& Cyc_Toc__zero_arr_plus_char_ev,&
Cyc_Toc__zero_arr_plus_short_ev,& Cyc_Toc__zero_arr_plus_int_ev,& Cyc_Toc__zero_arr_plus_float_ev,&
Cyc_Toc__zero_arr_plus_double_ev,& Cyc_Toc__zero_arr_plus_longdouble_ev,& Cyc_Toc__zero_arr_plus_voidstar_ev};
struct Cyc_Toc_functionSet Cyc_Toc__get_zero_arr_size_functionSet={& Cyc_Toc__get_zero_arr_size_char_ev,&
Cyc_Toc__get_zero_arr_size_short_ev,& Cyc_Toc__get_zero_arr_size_int_ev,& Cyc_Toc__get_zero_arr_size_float_ev,&
Cyc_Toc__get_zero_arr_size_double_ev,& Cyc_Toc__get_zero_arr_size_longdouble_ev,&
Cyc_Toc__get_zero_arr_size_voidstar_ev};struct Cyc_Toc_functionSet Cyc_Toc__zero_arr_inplace_plus_functionSet={&
Cyc_Toc__zero_arr_inplace_plus_char_ev,& Cyc_Toc__zero_arr_inplace_plus_short_ev,&
Cyc_Toc__zero_arr_inplace_plus_int_ev,& Cyc_Toc__zero_arr_inplace_plus_float_ev,&
Cyc_Toc__zero_arr_inplace_plus_double_ev,& Cyc_Toc__zero_arr_inplace_plus_longdouble_ev,&
Cyc_Toc__zero_arr_inplace_plus_voidstar_ev};struct Cyc_Toc_functionSet Cyc_Toc__zero_arr_inplace_plus_post_functionSet={&
Cyc_Toc__zero_arr_inplace_plus_post_char_ev,& Cyc_Toc__zero_arr_inplace_plus_post_short_ev,&
Cyc_Toc__zero_arr_inplace_plus_post_int_ev,& Cyc_Toc__zero_arr_inplace_plus_post_float_ev,&
Cyc_Toc__zero_arr_inplace_plus_post_double_ev,& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_ev,&
Cyc_Toc__zero_arr_inplace_plus_post_voidstar_ev};static struct Cyc_Absyn_Exp*Cyc_Toc_getFunctionType(
struct Cyc_Toc_functionSet*fS,void*t){struct Cyc_Absyn_Exp*function;{void*_tmp106=
t;void*_tmp107;int _tmp108;_LL1: if(_tmp106 <= (void*)4)goto _LL3;if(*((int*)_tmp106)
!= 5)goto _LL3;_tmp107=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp106)->f2;_LL2:{
void*_tmp109=_tmp107;_LLC: if((int)_tmp109 != 0)goto _LLE;_LLD: function=fS->fchar;
goto _LLB;_LLE: if((int)_tmp109 != 1)goto _LL10;_LLF: function=fS->fshort;goto _LLB;
_LL10: if((int)_tmp109 != 2)goto _LL12;_LL11: function=fS->fint;goto _LLB;_LL12:;
_LL13:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp10A=_cycalloc(
sizeof(*_tmp10A));_tmp10A[0]=({struct Cyc_Core_Impossible_struct _tmp10B;_tmp10B.tag=
Cyc_Core_Impossible;_tmp10B.f1=({const char*_tmp10C="impossible IntType (not char, short or int)";
_tag_dyneither(_tmp10C,sizeof(char),44);});_tmp10B;});_tmp10A;}));_LLB:;}goto
_LL0;_LL3: if((int)_tmp106 != 1)goto _LL5;_LL4: function=fS->ffloat;goto _LL0;_LL5:
if(_tmp106 <= (void*)4)goto _LL9;if(*((int*)_tmp106)!= 6)goto _LL7;_tmp108=((struct
Cyc_Absyn_DoubleType_struct*)_tmp106)->f1;_LL6: switch(_tmp108){case 1: _LL14:
function=fS->flongdouble;break;default: _LL15: function=fS->fdouble;}goto _LL0;_LL7:
if(*((int*)_tmp106)!= 4)goto _LL9;_LL8: function=fS->fvoidstar;goto _LL0;_LL9:;_LLA:(
int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp10D=_cycalloc(sizeof(*
_tmp10D));_tmp10D[0]=({struct Cyc_Core_Impossible_struct _tmp10E;_tmp10E.tag=Cyc_Core_Impossible;
_tmp10E.f1=({const char*_tmp10F="impossible expression type (not int, float, double, or pointer)";
_tag_dyneither(_tmp10F,sizeof(char),64);});_tmp10E;});_tmp10D;}));_LL0:;}return
function;}struct Cyc_Absyn_Exp*Cyc_Toc_getFunction(struct Cyc_Toc_functionSet*fS,
struct Cyc_Absyn_Exp*arr){return Cyc_Toc_getFunctionType(fS,Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(arr->topt))->v));}struct Cyc_Absyn_Exp*
Cyc_Toc_getFunctionRemovePointer(struct Cyc_Toc_functionSet*fS,struct Cyc_Absyn_Exp*
arr){void*_tmp110=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
arr->topt))->v);struct Cyc_Absyn_PtrInfo _tmp111;void*_tmp112;_LL18: if(_tmp110 <= (
void*)4)goto _LL1A;if(*((int*)_tmp110)!= 4)goto _LL1A;_tmp111=((struct Cyc_Absyn_PointerType_struct*)
_tmp110)->f1;_tmp112=(void*)_tmp111.elt_typ;_LL19: return Cyc_Toc_getFunctionType(
fS,_tmp112);_LL1A:;_LL1B:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp113=_cycalloc(sizeof(*_tmp113));_tmp113[0]=({struct Cyc_Core_Impossible_struct
_tmp114;_tmp114.tag=Cyc_Core_Impossible;_tmp114.f1=({const char*_tmp115="impossible type (not pointer)";
_tag_dyneither(_tmp115,sizeof(char),30);});_tmp114;});_tmp113;}));_LL17:;}struct
_tuple8{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};static int Cyc_Toc_is_zero(
struct Cyc_Absyn_Exp*e){void*_tmp116=(void*)e->r;union Cyc_Absyn_Cnst_union _tmp117;
char _tmp118;union Cyc_Absyn_Cnst_union _tmp119;short _tmp11A;union Cyc_Absyn_Cnst_union
_tmp11B;int _tmp11C;union Cyc_Absyn_Cnst_union _tmp11D;long long _tmp11E;union Cyc_Absyn_Cnst_union
_tmp11F;struct Cyc_Absyn_Exp*_tmp120;struct Cyc_List_List*_tmp121;struct Cyc_List_List*
_tmp122;struct Cyc_List_List*_tmp123;struct Cyc_List_List*_tmp124;struct Cyc_List_List*
_tmp125;_LL1D: if(*((int*)_tmp116)!= 0)goto _LL1F;_tmp117=((struct Cyc_Absyn_Const_e_struct*)
_tmp116)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)_tmp116)->f1).Char_c).tag != 
0)goto _LL1F;_tmp118=(_tmp117.Char_c).f2;_LL1E: return _tmp118 == '\000';_LL1F: if(*((
int*)_tmp116)!= 0)goto _LL21;_tmp119=((struct Cyc_Absyn_Const_e_struct*)_tmp116)->f1;
if(((((struct Cyc_Absyn_Const_e_struct*)_tmp116)->f1).Short_c).tag != 1)goto _LL21;
_tmp11A=(_tmp119.Short_c).f2;_LL20: return _tmp11A == 0;_LL21: if(*((int*)_tmp116)!= 
0)goto _LL23;_tmp11B=((struct Cyc_Absyn_Const_e_struct*)_tmp116)->f1;if(((((struct
Cyc_Absyn_Const_e_struct*)_tmp116)->f1).Int_c).tag != 2)goto _LL23;_tmp11C=(
_tmp11B.Int_c).f2;_LL22: return _tmp11C == 0;_LL23: if(*((int*)_tmp116)!= 0)goto
_LL25;_tmp11D=((struct Cyc_Absyn_Const_e_struct*)_tmp116)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)
_tmp116)->f1).LongLong_c).tag != 3)goto _LL25;_tmp11E=(_tmp11D.LongLong_c).f2;
_LL24: return _tmp11E == 0;_LL25: if(*((int*)_tmp116)!= 0)goto _LL27;_tmp11F=((struct
Cyc_Absyn_Const_e_struct*)_tmp116)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)
_tmp116)->f1).Null_c).tag != 6)goto _LL27;_LL26: return 1;_LL27: if(*((int*)_tmp116)
!= 15)goto _LL29;_tmp120=((struct Cyc_Absyn_Cast_e_struct*)_tmp116)->f2;_LL28:
return Cyc_Toc_is_zero(_tmp120);_LL29: if(*((int*)_tmp116)!= 26)goto _LL2B;_tmp121=((
struct Cyc_Absyn_Tuple_e_struct*)_tmp116)->f1;_LL2A: return((int(*)(int(*pred)(
struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_forall)(Cyc_Toc_is_zero,
_tmp121);_LL2B: if(*((int*)_tmp116)!= 28)goto _LL2D;_tmp122=((struct Cyc_Absyn_Array_e_struct*)
_tmp116)->f1;_LL2C: _tmp123=_tmp122;goto _LL2E;_LL2D: if(*((int*)_tmp116)!= 30)goto
_LL2F;_tmp123=((struct Cyc_Absyn_Struct_e_struct*)_tmp116)->f3;_LL2E: _tmp124=
_tmp123;goto _LL30;_LL2F: if(*((int*)_tmp116)!= 27)goto _LL31;_tmp124=((struct Cyc_Absyn_CompoundLit_e_struct*)
_tmp116)->f2;_LL30: _tmp125=_tmp124;goto _LL32;_LL31: if(*((int*)_tmp116)!= 37)goto
_LL33;_tmp125=((struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp116)->f2;_LL32: for(0;
_tmp125 != 0;_tmp125=_tmp125->tl){if(!Cyc_Toc_is_zero((*((struct _tuple8*)_tmp125->hd)).f2))
return 0;}return 1;_LL33:;_LL34: return 0;_LL1C:;}static int Cyc_Toc_is_nullable(void*
t){void*_tmp126=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp127;struct Cyc_Absyn_PtrAtts
_tmp128;struct Cyc_Absyn_Conref*_tmp129;_LL36: if(_tmp126 <= (void*)4)goto _LL38;if(*((
int*)_tmp126)!= 4)goto _LL38;_tmp127=((struct Cyc_Absyn_PointerType_struct*)
_tmp126)->f1;_tmp128=_tmp127.ptr_atts;_tmp129=_tmp128.nullable;_LL37: return((int(*)(
int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp129);_LL38:;_LL39:({
void*_tmp12A=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp12B="is_nullable";_tag_dyneither(_tmp12B,sizeof(char),12);}),
_tag_dyneither(_tmp12A,sizeof(void*),0));});_LL35:;}static char _tmp138[1]="";
static char _tmp13A[8]="*bogus*";static struct _tuple1*Cyc_Toc_collapse_qvar_tag(
struct _tuple1*x,struct _dyneither_ptr tag){struct _DynRegionHandle*_tmp12D;struct
Cyc_Dict_Dict*_tmp12E;struct Cyc_Toc_TocState _tmp12C=*((struct Cyc_Toc_TocState*)
_check_null(Cyc_Toc_toc_state));_tmp12D=_tmp12C.dyn;_tmp12E=_tmp12C.qvar_tags;{
static struct _dyneither_ptr bogus_string={_tmp13A,_tmp13A,_tmp13A + 8};static struct
_tuple1 bogus_qvar={(union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){
0}),& bogus_string};static struct _tuple6 pair={& bogus_qvar,{_tmp138,_tmp138,_tmp138
+ 1}};pair=({struct _tuple6 _tmp12F;_tmp12F.f1=x;_tmp12F.f2=tag;_tmp12F;});{struct
_DynRegionFrame _tmp130;struct _RegionHandle*d=_open_dynregion(& _tmp130,_tmp12D);{
struct _tuple1**_tmp131=((struct _tuple1**(*)(struct Cyc_Dict_Dict d,struct _tuple6*k))
Cyc_Dict_lookup_opt)(*_tmp12E,(struct _tuple6*)& pair);if(_tmp131 != 0){struct
_tuple1*_tmp132=*_tmp131;_npop_handler(0);return _tmp132;}{struct _tuple6*_tmp133=({
struct _tuple6*_tmp137=_cycalloc(sizeof(*_tmp137));_tmp137->f1=x;_tmp137->f2=tag;
_tmp137;});struct _tuple1*res=({struct _tuple1*_tmp135=_cycalloc(sizeof(*_tmp135));
_tmp135->f1=(*x).f1;_tmp135->f2=({struct _dyneither_ptr*_tmp136=_cycalloc(sizeof(*
_tmp136));_tmp136[0]=(struct _dyneither_ptr)Cyc_strconcat((struct _dyneither_ptr)*(*
x).f2,(struct _dyneither_ptr)tag);_tmp136;});_tmp135;});*_tmp12E=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,struct _tuple6*k,struct _tuple1*v))Cyc_Dict_insert)(*_tmp12E,(
struct _tuple6*)_tmp133,res);{struct _tuple1*_tmp134=res;_npop_handler(0);return
_tmp134;}}};_pop_dynregion(d);}}}struct _tuple9{void*f1;struct Cyc_List_List*f2;};
static void*Cyc_Toc_add_tuple_type(struct Cyc_List_List*tqs0){struct
_DynRegionHandle*_tmp13C;struct Cyc_List_List**_tmp13D;struct Cyc_Toc_TocState
_tmp13B=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp13C=
_tmp13B.dyn;_tmp13D=_tmp13B.tuple_types;{struct _DynRegionFrame _tmp13E;struct
_RegionHandle*d=_open_dynregion(& _tmp13E,_tmp13C);{struct Cyc_List_List*_tmp13F=*
_tmp13D;for(0;_tmp13F != 0;_tmp13F=_tmp13F->tl){struct _tuple9 _tmp141;void*_tmp142;
struct Cyc_List_List*_tmp143;struct _tuple9*_tmp140=(struct _tuple9*)_tmp13F->hd;
_tmp141=*_tmp140;_tmp142=_tmp141.f1;_tmp143=_tmp141.f2;{struct Cyc_List_List*
_tmp144=tqs0;for(0;_tmp144 != 0  && _tmp143 != 0;(_tmp144=_tmp144->tl,_tmp143=
_tmp143->tl)){if(!Cyc_Tcutil_unify((*((struct _tuple4*)_tmp144->hd)).f2,(void*)
_tmp143->hd))break;}if(_tmp144 == 0  && _tmp143 == 0){void*_tmp145=_tmp142;
_npop_handler(0);return _tmp145;}}}}{struct _dyneither_ptr*xname=({struct
_dyneither_ptr*_tmp154=_cycalloc(sizeof(*_tmp154));_tmp154[0]=(struct
_dyneither_ptr)({struct Cyc_Int_pa_struct _tmp157;_tmp157.tag=1;_tmp157.f1=(
unsigned long)Cyc_Toc_tuple_type_counter ++;{void*_tmp155[1]={& _tmp157};Cyc_aprintf(({
const char*_tmp156="_tuple%d";_tag_dyneither(_tmp156,sizeof(char),9);}),
_tag_dyneither(_tmp155,sizeof(void*),1));}});_tmp154;});void*x=Cyc_Absyn_strct(
xname);struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(struct _RegionHandle*,
void*(*f)(struct _tuple4*),struct Cyc_List_List*x))Cyc_List_rmap)(d,Cyc_Tcutil_snd_tqt,
tqs0);struct Cyc_List_List*_tmp146=0;struct Cyc_List_List*ts2=ts;{int i=1;for(0;ts2
!= 0;(ts2=ts2->tl,i ++)){_tmp146=({struct Cyc_List_List*_tmp147=_cycalloc(sizeof(*
_tmp147));_tmp147->hd=({struct Cyc_Absyn_Aggrfield*_tmp148=_cycalloc(sizeof(*
_tmp148));_tmp148->name=Cyc_Absyn_fieldname(i);_tmp148->tq=Cyc_Toc_mt_tq;_tmp148->type=(
void*)((void*)ts2->hd);_tmp148->width=0;_tmp148->attributes=0;_tmp148;});_tmp147->tl=
_tmp146;_tmp147;});}}_tmp146=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(_tmp146);{struct Cyc_Absyn_Aggrdecl*_tmp149=({struct Cyc_Absyn_Aggrdecl*
_tmp150=_cycalloc(sizeof(*_tmp150));_tmp150->kind=(void*)((void*)0);_tmp150->sc=(
void*)((void*)2);_tmp150->name=({struct _tuple1*_tmp152=_cycalloc(sizeof(*_tmp152));
_tmp152->f1=(union Cyc_Absyn_Nmspace_union)({union Cyc_Absyn_Nmspace_union _tmp153;(
_tmp153.Rel_n).tag=1;(_tmp153.Rel_n).f1=0;_tmp153;});_tmp152->f2=xname;_tmp152;});
_tmp150->tvs=0;_tmp150->impl=({struct Cyc_Absyn_AggrdeclImpl*_tmp151=_cycalloc(
sizeof(*_tmp151));_tmp151->exist_vars=0;_tmp151->rgn_po=0;_tmp151->fields=
_tmp146;_tmp151;});_tmp150->attributes=0;_tmp150;});Cyc_Toc_result_decls=({
struct Cyc_List_List*_tmp14A=_cycalloc(sizeof(*_tmp14A));_tmp14A->hd=Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Aggr_d_struct*_tmp14B=_cycalloc(sizeof(*_tmp14B));
_tmp14B[0]=({struct Cyc_Absyn_Aggr_d_struct _tmp14C;_tmp14C.tag=6;_tmp14C.f1=
_tmp149;_tmp14C;});_tmp14B;}),0);_tmp14A->tl=Cyc_Toc_result_decls;_tmp14A;});*
_tmp13D=({struct Cyc_List_List*_tmp14D=_region_malloc(d,sizeof(*_tmp14D));_tmp14D->hd=({
struct _tuple9*_tmp14E=_region_malloc(d,sizeof(*_tmp14E));_tmp14E->f1=x;_tmp14E->f2=
ts;_tmp14E;});_tmp14D->tl=*_tmp13D;_tmp14D;});{void*_tmp14F=x;_npop_handler(0);
return _tmp14F;}}};_pop_dynregion(d);}}struct _tuple1*Cyc_Toc_temp_var(){int
_tmp158=Cyc_Toc_temp_var_counter ++;struct _tuple1*res=({struct _tuple1*_tmp159=
_cycalloc(sizeof(*_tmp159));_tmp159->f1=(union Cyc_Absyn_Nmspace_union)({union Cyc_Absyn_Nmspace_union
_tmp15E;(_tmp15E.Loc_n).tag=0;_tmp15E;});_tmp159->f2=({struct _dyneither_ptr*
_tmp15A=_cycalloc(sizeof(*_tmp15A));_tmp15A[0]=(struct _dyneither_ptr)({struct Cyc_Int_pa_struct
_tmp15D;_tmp15D.tag=1;_tmp15D.f1=(unsigned int)_tmp158;{void*_tmp15B[1]={&
_tmp15D};Cyc_aprintf(({const char*_tmp15C="_tmp%X";_tag_dyneither(_tmp15C,sizeof(
char),7);}),_tag_dyneither(_tmp15B,sizeof(void*),1));}});_tmp15A;});_tmp159;});
return res;}static struct _dyneither_ptr*Cyc_Toc_fresh_label(){struct
_DynRegionHandle*_tmp160;struct Cyc_Xarray_Xarray*_tmp161;struct Cyc_Toc_TocState
_tmp15F=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp160=
_tmp15F.dyn;_tmp161=_tmp15F.temp_labels;{struct _DynRegionFrame _tmp162;struct
_RegionHandle*d=_open_dynregion(& _tmp162,_tmp160);{int _tmp163=Cyc_Toc_fresh_label_counter
++;if(_tmp163 < ((int(*)(struct Cyc_Xarray_Xarray*))Cyc_Xarray_length)(_tmp161)){
struct _dyneither_ptr*_tmp164=((struct _dyneither_ptr*(*)(struct Cyc_Xarray_Xarray*,
int))Cyc_Xarray_get)(_tmp161,_tmp163);_npop_handler(0);return _tmp164;}{struct
_dyneither_ptr*res=({struct _dyneither_ptr*_tmp168=_cycalloc(sizeof(*_tmp168));
_tmp168[0]=(struct _dyneither_ptr)({struct Cyc_Int_pa_struct _tmp16B;_tmp16B.tag=1;
_tmp16B.f1=(unsigned int)_tmp163;{void*_tmp169[1]={& _tmp16B};Cyc_aprintf(({const
char*_tmp16A="_LL%X";_tag_dyneither(_tmp16A,sizeof(char),6);}),_tag_dyneither(
_tmp169,sizeof(void*),1));}});_tmp168;});if(((int(*)(struct Cyc_Xarray_Xarray*,
struct _dyneither_ptr*))Cyc_Xarray_add_ind)(_tmp161,res)!= _tmp163)({void*_tmp165=
0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp166="fresh_label: add_ind returned bad index...";_tag_dyneither(
_tmp166,sizeof(char),43);}),_tag_dyneither(_tmp165,sizeof(void*),0));});{struct
_dyneither_ptr*_tmp167=res;_npop_handler(0);return _tmp167;}}};_pop_dynregion(d);}}
static struct Cyc_Absyn_Exp*Cyc_Toc_tunion_tag(struct Cyc_Absyn_Tuniondecl*td,
struct _tuple1*name,int carries_value){int ans=0;struct Cyc_List_List*_tmp16C=(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(td->fields))->v;while(Cyc_Absyn_qvar_cmp(
name,((struct Cyc_Absyn_Tunionfield*)((struct Cyc_List_List*)_check_null(_tmp16C))->hd)->name)
!= 0){if((td->is_flat  || carries_value  && ((struct Cyc_Absyn_Tunionfield*)_tmp16C->hd)->typs
!= 0) || !carries_value  && ((struct Cyc_Absyn_Tunionfield*)_tmp16C->hd)->typs == 0)
++ ans;_tmp16C=_tmp16C->tl;}return Cyc_Absyn_uint_exp((unsigned int)ans,0);}static
int Cyc_Toc_num_void_tags(struct Cyc_Absyn_Tuniondecl*td){int ans=0;{struct Cyc_List_List*
_tmp16D=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(td->fields))->v;
for(0;_tmp16D != 0;_tmp16D=_tmp16D->tl){if(((struct Cyc_Absyn_Tunionfield*)_tmp16D->hd)->typs
== 0)++ ans;}}return ans;}static void*Cyc_Toc_typ_to_c(void*t);static struct _tuple2*
Cyc_Toc_arg_to_c(struct _tuple2*a){struct Cyc_Core_Opt*_tmp16F;struct Cyc_Absyn_Tqual
_tmp170;void*_tmp171;struct _tuple2 _tmp16E=*a;_tmp16F=_tmp16E.f1;_tmp170=_tmp16E.f2;
_tmp171=_tmp16E.f3;return({struct _tuple2*_tmp172=_cycalloc(sizeof(*_tmp172));
_tmp172->f1=_tmp16F;_tmp172->f2=_tmp170;_tmp172->f3=Cyc_Toc_typ_to_c(_tmp171);
_tmp172;});}static struct _tuple4*Cyc_Toc_typ_to_c_f(struct _tuple4*x){struct Cyc_Absyn_Tqual
_tmp174;void*_tmp175;struct _tuple4 _tmp173=*x;_tmp174=_tmp173.f1;_tmp175=_tmp173.f2;
return({struct _tuple4*_tmp176=_cycalloc(sizeof(*_tmp176));_tmp176->f1=_tmp174;
_tmp176->f2=Cyc_Toc_typ_to_c(_tmp175);_tmp176;});}static void*Cyc_Toc_typ_to_c_array(
void*t){void*_tmp177=Cyc_Tcutil_compress(t);struct Cyc_Absyn_ArrayInfo _tmp178;
void*_tmp179;struct Cyc_Absyn_Tqual _tmp17A;struct Cyc_Absyn_Exp*_tmp17B;struct Cyc_Absyn_Conref*
_tmp17C;struct Cyc_Position_Segment*_tmp17D;struct Cyc_Core_Opt*_tmp17E;struct Cyc_Core_Opt
_tmp17F;void*_tmp180;_LL3B: if(_tmp177 <= (void*)4)goto _LL3F;if(*((int*)_tmp177)!= 
7)goto _LL3D;_tmp178=((struct Cyc_Absyn_ArrayType_struct*)_tmp177)->f1;_tmp179=(
void*)_tmp178.elt_type;_tmp17A=_tmp178.tq;_tmp17B=_tmp178.num_elts;_tmp17C=
_tmp178.zero_term;_tmp17D=_tmp178.zt_loc;_LL3C: return Cyc_Absyn_array_typ(Cyc_Toc_typ_to_c_array(
_tmp179),_tmp17A,_tmp17B,Cyc_Absyn_false_conref,_tmp17D);_LL3D: if(*((int*)
_tmp177)!= 0)goto _LL3F;_tmp17E=((struct Cyc_Absyn_Evar_struct*)_tmp177)->f2;if(
_tmp17E == 0)goto _LL3F;_tmp17F=*_tmp17E;_tmp180=(void*)_tmp17F.v;_LL3E: return Cyc_Toc_typ_to_c_array(
_tmp180);_LL3F:;_LL40: return Cyc_Toc_typ_to_c(t);_LL3A:;}static struct Cyc_Absyn_Aggrfield*
Cyc_Toc_aggrfield_to_c(struct Cyc_Absyn_Aggrfield*f){return({struct Cyc_Absyn_Aggrfield*
_tmp181=_cycalloc(sizeof(*_tmp181));_tmp181->name=f->name;_tmp181->tq=Cyc_Toc_mt_tq;
_tmp181->type=(void*)Cyc_Toc_typ_to_c((void*)f->type);_tmp181->width=f->width;
_tmp181->attributes=f->attributes;_tmp181;});}static void Cyc_Toc_enumfields_to_c(
struct Cyc_List_List*fs){return;}static void*Cyc_Toc_char_star_typ(){static void**
cs=0;if(cs == 0)cs=({void**_tmp182=_cycalloc(sizeof(*_tmp182));_tmp182[0]=Cyc_Absyn_star_typ(
Cyc_Absyn_char_typ,(void*)2,Cyc_Toc_mt_tq,Cyc_Absyn_false_conref);_tmp182;});
return*cs;}static void*Cyc_Toc_rgn_typ(){static void**r=0;if(r == 0)r=({void**
_tmp183=_cycalloc(sizeof(*_tmp183));_tmp183[0]=Cyc_Absyn_cstar_typ(Cyc_Absyn_strct(
Cyc_Toc__RegionHandle_sp),Cyc_Toc_mt_tq);_tmp183;});return*r;}static void*Cyc_Toc_dyn_rgn_typ(){
static void**r=0;if(r == 0)r=({void**_tmp184=_cycalloc(sizeof(*_tmp184));_tmp184[0]=
Cyc_Absyn_cstar_typ(Cyc_Absyn_strct(Cyc_Toc__DynRegionHandle_sp),Cyc_Toc_mt_tq);
_tmp184;});return*r;}static void*Cyc_Toc_typ_to_c(void*t){void*_tmp185=t;struct
Cyc_Core_Opt*_tmp186;struct Cyc_Core_Opt*_tmp187;struct Cyc_Core_Opt _tmp188;void*
_tmp189;struct Cyc_Absyn_Tvar*_tmp18A;struct Cyc_Absyn_TunionInfo _tmp18B;union Cyc_Absyn_TunionInfoU_union
_tmp18C;struct Cyc_Absyn_Tuniondecl**_tmp18D;struct Cyc_Absyn_Tuniondecl*_tmp18E;
struct Cyc_Absyn_TunionFieldInfo _tmp18F;union Cyc_Absyn_TunionFieldInfoU_union
_tmp190;struct Cyc_Absyn_Tuniondecl*_tmp191;struct Cyc_Absyn_Tunionfield*_tmp192;
struct Cyc_Absyn_PtrInfo _tmp193;void*_tmp194;struct Cyc_Absyn_Tqual _tmp195;struct
Cyc_Absyn_PtrAtts _tmp196;struct Cyc_Absyn_Conref*_tmp197;struct Cyc_Absyn_ArrayInfo
_tmp198;void*_tmp199;struct Cyc_Absyn_Tqual _tmp19A;struct Cyc_Absyn_Exp*_tmp19B;
struct Cyc_Position_Segment*_tmp19C;struct Cyc_Absyn_FnInfo _tmp19D;void*_tmp19E;
struct Cyc_List_List*_tmp19F;int _tmp1A0;struct Cyc_Absyn_VarargInfo*_tmp1A1;struct
Cyc_List_List*_tmp1A2;struct Cyc_List_List*_tmp1A3;void*_tmp1A4;struct Cyc_List_List*
_tmp1A5;struct Cyc_Absyn_AggrInfo _tmp1A6;union Cyc_Absyn_AggrInfoU_union _tmp1A7;
struct Cyc_List_List*_tmp1A8;struct _tuple1*_tmp1A9;struct Cyc_List_List*_tmp1AA;
struct _tuple1*_tmp1AB;struct Cyc_List_List*_tmp1AC;struct Cyc_Absyn_Typedefdecl*
_tmp1AD;void**_tmp1AE;void*_tmp1AF;_LL42: if((int)_tmp185 != 0)goto _LL44;_LL43:
return t;_LL44: if(_tmp185 <= (void*)4)goto _LL56;if(*((int*)_tmp185)!= 0)goto _LL46;
_tmp186=((struct Cyc_Absyn_Evar_struct*)_tmp185)->f2;if(_tmp186 != 0)goto _LL46;
_LL45: return Cyc_Absyn_sint_typ;_LL46: if(*((int*)_tmp185)!= 0)goto _LL48;_tmp187=((
struct Cyc_Absyn_Evar_struct*)_tmp185)->f2;if(_tmp187 == 0)goto _LL48;_tmp188=*
_tmp187;_tmp189=(void*)_tmp188.v;_LL47: return Cyc_Toc_typ_to_c(_tmp189);_LL48: if(*((
int*)_tmp185)!= 1)goto _LL4A;_tmp18A=((struct Cyc_Absyn_VarType_struct*)_tmp185)->f1;
_LL49: if(Cyc_Tcutil_tvar_kind(_tmp18A)== (void*)0)return(void*)0;else{return Cyc_Absyn_void_star_typ();}
_LL4A: if(*((int*)_tmp185)!= 2)goto _LL4C;_tmp18B=((struct Cyc_Absyn_TunionType_struct*)
_tmp185)->f1;_tmp18C=_tmp18B.tunion_info;if((((((struct Cyc_Absyn_TunionType_struct*)
_tmp185)->f1).tunion_info).KnownTunion).tag != 1)goto _LL4C;_tmp18D=(_tmp18C.KnownTunion).f1;
_tmp18E=*_tmp18D;_LL4B: if(_tmp18E->is_flat)return Cyc_Absyn_unionq_typ(Cyc_Toc_collapse_qvar_tag(
_tmp18E->name,({const char*_tmp1B0="_union";_tag_dyneither(_tmp1B0,sizeof(char),7);})));
else{return Cyc_Absyn_void_star_typ();}_LL4C: if(*((int*)_tmp185)!= 2)goto _LL4E;
_LL4D:({void*_tmp1B1=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Toc_toc_impos)(({const char*_tmp1B2="unresolved TunionType";_tag_dyneither(
_tmp1B2,sizeof(char),22);}),_tag_dyneither(_tmp1B1,sizeof(void*),0));});_LL4E:
if(*((int*)_tmp185)!= 3)goto _LL50;_tmp18F=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp185)->f1;_tmp190=_tmp18F.field_info;if((((((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp185)->f1).field_info).KnownTunionfield).tag != 1)goto _LL50;_tmp191=(_tmp190.KnownTunionfield).f1;
_tmp192=(_tmp190.KnownTunionfield).f2;_LL4F: if(_tmp191->is_flat)return Cyc_Absyn_unionq_typ(
Cyc_Toc_collapse_qvar_tag(_tmp191->name,({const char*_tmp1B3="_union";
_tag_dyneither(_tmp1B3,sizeof(char),7);})));if(_tmp192->typs == 0){if(_tmp191->is_xtunion)
return Cyc_Toc_char_star_typ();else{return Cyc_Absyn_uint_typ;}}else{return Cyc_Absyn_strctq(
Cyc_Toc_collapse_qvar_tag(_tmp192->name,({const char*_tmp1B4="_struct";
_tag_dyneither(_tmp1B4,sizeof(char),8);})));}_LL50: if(*((int*)_tmp185)!= 3)goto
_LL52;_LL51:({void*_tmp1B5=0;((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp1B6="unresolved TunionFieldType";
_tag_dyneither(_tmp1B6,sizeof(char),27);}),_tag_dyneither(_tmp1B5,sizeof(void*),
0));});_LL52: if(*((int*)_tmp185)!= 4)goto _LL54;_tmp193=((struct Cyc_Absyn_PointerType_struct*)
_tmp185)->f1;_tmp194=(void*)_tmp193.elt_typ;_tmp195=_tmp193.elt_tq;_tmp196=
_tmp193.ptr_atts;_tmp197=_tmp196.bounds;_LL53: _tmp194=Cyc_Toc_typ_to_c_array(
_tmp194);{union Cyc_Absyn_Constraint_union _tmp1B7=(Cyc_Absyn_compress_conref(
_tmp197))->v;void*_tmp1B8;_LL7D: if((_tmp1B7.No_constr).tag != 2)goto _LL7F;_LL7E:
goto _LL80;_LL7F: if((_tmp1B7.Eq_constr).tag != 0)goto _LL81;_tmp1B8=(_tmp1B7.Eq_constr).f1;
if((int)_tmp1B8 != 0)goto _LL81;_LL80: return Cyc_Toc_dyneither_ptr_typ;_LL81:;_LL82:
return Cyc_Absyn_star_typ(_tmp194,(void*)2,_tmp195,Cyc_Absyn_false_conref);_LL7C:;}
_LL54: if(*((int*)_tmp185)!= 5)goto _LL56;_LL55: goto _LL57;_LL56: if((int)_tmp185 != 
1)goto _LL58;_LL57: goto _LL59;_LL58: if(_tmp185 <= (void*)4)goto _LL70;if(*((int*)
_tmp185)!= 6)goto _LL5A;_LL59: return t;_LL5A: if(*((int*)_tmp185)!= 7)goto _LL5C;
_tmp198=((struct Cyc_Absyn_ArrayType_struct*)_tmp185)->f1;_tmp199=(void*)_tmp198.elt_type;
_tmp19A=_tmp198.tq;_tmp19B=_tmp198.num_elts;_tmp19C=_tmp198.zt_loc;_LL5B: return
Cyc_Absyn_array_typ(Cyc_Toc_typ_to_c_array(_tmp199),_tmp19A,_tmp19B,Cyc_Absyn_false_conref,
_tmp19C);_LL5C: if(*((int*)_tmp185)!= 8)goto _LL5E;_tmp19D=((struct Cyc_Absyn_FnType_struct*)
_tmp185)->f1;_tmp19E=(void*)_tmp19D.ret_typ;_tmp19F=_tmp19D.args;_tmp1A0=_tmp19D.c_varargs;
_tmp1A1=_tmp19D.cyc_varargs;_tmp1A2=_tmp19D.attributes;_LL5D: {struct Cyc_List_List*
_tmp1B9=0;for(0;_tmp1A2 != 0;_tmp1A2=_tmp1A2->tl){void*_tmp1BA=(void*)_tmp1A2->hd;
_LL84: if((int)_tmp1BA != 3)goto _LL86;_LL85: goto _LL87;_LL86: if((int)_tmp1BA != 4)
goto _LL88;_LL87: goto _LL89;_LL88: if(_tmp1BA <= (void*)17)goto _LL8C;if(*((int*)
_tmp1BA)!= 3)goto _LL8A;_LL89: continue;_LL8A: if(*((int*)_tmp1BA)!= 4)goto _LL8C;
_LL8B: continue;_LL8C:;_LL8D: _tmp1B9=({struct Cyc_List_List*_tmp1BB=_cycalloc(
sizeof(*_tmp1BB));_tmp1BB->hd=(void*)((void*)_tmp1A2->hd);_tmp1BB->tl=_tmp1B9;
_tmp1BB;});goto _LL83;_LL83:;}{struct Cyc_List_List*_tmp1BC=((struct Cyc_List_List*(*)(
struct _tuple2*(*f)(struct _tuple2*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_arg_to_c,
_tmp19F);if(_tmp1A1 != 0){void*_tmp1BD=Cyc_Toc_typ_to_c(Cyc_Absyn_dyneither_typ((
void*)_tmp1A1->type,(void*)2,Cyc_Toc_mt_tq,Cyc_Absyn_false_conref));struct
_tuple2*_tmp1BE=({struct _tuple2*_tmp1C0=_cycalloc(sizeof(*_tmp1C0));_tmp1C0->f1=
_tmp1A1->name;_tmp1C0->f2=_tmp1A1->tq;_tmp1C0->f3=_tmp1BD;_tmp1C0;});_tmp1BC=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(
_tmp1BC,({struct Cyc_List_List*_tmp1BF=_cycalloc(sizeof(*_tmp1BF));_tmp1BF->hd=
_tmp1BE;_tmp1BF->tl=0;_tmp1BF;}));}return(void*)({struct Cyc_Absyn_FnType_struct*
_tmp1C1=_cycalloc(sizeof(*_tmp1C1));_tmp1C1[0]=({struct Cyc_Absyn_FnType_struct
_tmp1C2;_tmp1C2.tag=8;_tmp1C2.f1=({struct Cyc_Absyn_FnInfo _tmp1C3;_tmp1C3.tvars=0;
_tmp1C3.effect=0;_tmp1C3.ret_typ=(void*)Cyc_Toc_typ_to_c(_tmp19E);_tmp1C3.args=
_tmp1BC;_tmp1C3.c_varargs=_tmp1A0;_tmp1C3.cyc_varargs=0;_tmp1C3.rgn_po=0;_tmp1C3.attributes=
_tmp1B9;_tmp1C3;});_tmp1C2;});_tmp1C1;});}}_LL5E: if(*((int*)_tmp185)!= 9)goto
_LL60;_tmp1A3=((struct Cyc_Absyn_TupleType_struct*)_tmp185)->f1;_LL5F: _tmp1A3=((
struct Cyc_List_List*(*)(struct _tuple4*(*f)(struct _tuple4*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Toc_typ_to_c_f,_tmp1A3);return Cyc_Toc_add_tuple_type(_tmp1A3);
_LL60: if(*((int*)_tmp185)!= 11)goto _LL62;_tmp1A4=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp185)->f1;_tmp1A5=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp185)->f2;_LL61:
return(void*)({struct Cyc_Absyn_AnonAggrType_struct*_tmp1C4=_cycalloc(sizeof(*
_tmp1C4));_tmp1C4[0]=({struct Cyc_Absyn_AnonAggrType_struct _tmp1C5;_tmp1C5.tag=11;
_tmp1C5.f1=(void*)_tmp1A4;_tmp1C5.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*
f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_aggrfield_to_c,
_tmp1A5);_tmp1C5;});_tmp1C4;});_LL62: if(*((int*)_tmp185)!= 10)goto _LL64;_tmp1A6=((
struct Cyc_Absyn_AggrType_struct*)_tmp185)->f1;_tmp1A7=_tmp1A6.aggr_info;_tmp1A8=
_tmp1A6.targs;_LL63:{union Cyc_Absyn_AggrInfoU_union _tmp1C6=_tmp1A7;_LL8F: if((
_tmp1C6.UnknownAggr).tag != 0)goto _LL91;_LL90: return t;_LL91:;_LL92: goto _LL8E;
_LL8E:;}{struct Cyc_Absyn_Aggrdecl*_tmp1C7=Cyc_Absyn_get_known_aggrdecl(_tmp1A7);
if((void*)_tmp1C7->kind == (void*)1)return Cyc_Toc_aggrdecl_type(_tmp1C7->name,Cyc_Absyn_unionq_typ);
else{return Cyc_Toc_aggrdecl_type(_tmp1C7->name,Cyc_Absyn_strctq);}}_LL64: if(*((
int*)_tmp185)!= 12)goto _LL66;_tmp1A9=((struct Cyc_Absyn_EnumType_struct*)_tmp185)->f1;
_LL65: return t;_LL66: if(*((int*)_tmp185)!= 13)goto _LL68;_tmp1AA=((struct Cyc_Absyn_AnonEnumType_struct*)
_tmp185)->f1;_LL67: Cyc_Toc_enumfields_to_c(_tmp1AA);return t;_LL68: if(*((int*)
_tmp185)!= 16)goto _LL6A;_tmp1AB=((struct Cyc_Absyn_TypedefType_struct*)_tmp185)->f1;
_tmp1AC=((struct Cyc_Absyn_TypedefType_struct*)_tmp185)->f2;_tmp1AD=((struct Cyc_Absyn_TypedefType_struct*)
_tmp185)->f3;_tmp1AE=((struct Cyc_Absyn_TypedefType_struct*)_tmp185)->f4;_LL69:
if(_tmp1AE == 0  || Cyc_noexpand_r){if(_tmp1AC != 0)return(void*)({struct Cyc_Absyn_TypedefType_struct*
_tmp1C8=_cycalloc(sizeof(*_tmp1C8));_tmp1C8[0]=({struct Cyc_Absyn_TypedefType_struct
_tmp1C9;_tmp1C9.tag=16;_tmp1C9.f1=_tmp1AB;_tmp1C9.f2=0;_tmp1C9.f3=_tmp1AD;
_tmp1C9.f4=0;_tmp1C9;});_tmp1C8;});else{return t;}}else{return(void*)({struct Cyc_Absyn_TypedefType_struct*
_tmp1CA=_cycalloc(sizeof(*_tmp1CA));_tmp1CA[0]=({struct Cyc_Absyn_TypedefType_struct
_tmp1CB;_tmp1CB.tag=16;_tmp1CB.f1=_tmp1AB;_tmp1CB.f2=0;_tmp1CB.f3=_tmp1AD;
_tmp1CB.f4=({void**_tmp1CC=_cycalloc(sizeof(*_tmp1CC));_tmp1CC[0]=Cyc_Toc_typ_to_c_array(*
_tmp1AE);_tmp1CC;});_tmp1CB;});_tmp1CA;});}_LL6A: if(*((int*)_tmp185)!= 18)goto
_LL6C;_LL6B: return Cyc_Absyn_uint_typ;_LL6C: if(*((int*)_tmp185)!= 14)goto _LL6E;
_tmp1AF=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)_tmp185)->f1;_LL6D: return
Cyc_Toc_rgn_typ();_LL6E: if(*((int*)_tmp185)!= 15)goto _LL70;_LL6F: return Cyc_Toc_dyn_rgn_typ();
_LL70: if((int)_tmp185 != 2)goto _LL72;_LL71:({void*_tmp1CD=0;((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp1CE="Toc::typ_to_c: type translation passed the heap region";
_tag_dyneither(_tmp1CE,sizeof(char),55);}),_tag_dyneither(_tmp1CD,sizeof(void*),
0));});_LL72: if((int)_tmp185 != 3)goto _LL74;_LL73:({void*_tmp1CF=0;((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp1D0="Toc::typ_to_c: type translation passed the unique region";
_tag_dyneither(_tmp1D0,sizeof(char),57);}),_tag_dyneither(_tmp1CF,sizeof(void*),
0));});_LL74: if(_tmp185 <= (void*)4)goto _LL76;if(*((int*)_tmp185)!= 19)goto _LL76;
_LL75: goto _LL77;_LL76: if(_tmp185 <= (void*)4)goto _LL78;if(*((int*)_tmp185)!= 20)
goto _LL78;_LL77: goto _LL79;_LL78: if(_tmp185 <= (void*)4)goto _LL7A;if(*((int*)
_tmp185)!= 21)goto _LL7A;_LL79:({void*_tmp1D1=0;((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp1D2="Toc::typ_to_c: type translation passed an effect";
_tag_dyneither(_tmp1D2,sizeof(char),49);}),_tag_dyneither(_tmp1D1,sizeof(void*),
0));});_LL7A: if(_tmp185 <= (void*)4)goto _LL41;if(*((int*)_tmp185)!= 17)goto _LL41;
_LL7B:({void*_tmp1D3=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Toc_toc_impos)(({const char*_tmp1D4="Toc::typ_to_c: type translation passed a valueof_t";
_tag_dyneither(_tmp1D4,sizeof(char),51);}),_tag_dyneither(_tmp1D3,sizeof(void*),
0));});_LL41:;}static struct Cyc_Absyn_Exp*Cyc_Toc_array_to_ptr_cast(void*t,struct
Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*l){void*_tmp1D5=t;struct Cyc_Absyn_ArrayInfo
_tmp1D6;void*_tmp1D7;struct Cyc_Absyn_Tqual _tmp1D8;_LL94: if(_tmp1D5 <= (void*)4)
goto _LL96;if(*((int*)_tmp1D5)!= 7)goto _LL96;_tmp1D6=((struct Cyc_Absyn_ArrayType_struct*)
_tmp1D5)->f1;_tmp1D7=(void*)_tmp1D6.elt_type;_tmp1D8=_tmp1D6.tq;_LL95: return Cyc_Toc_cast_it(
Cyc_Absyn_star_typ(_tmp1D7,(void*)2,_tmp1D8,Cyc_Absyn_false_conref),e);_LL96:;
_LL97: return Cyc_Toc_cast_it(t,e);_LL93:;}static int Cyc_Toc_atomic_typ(void*t){
void*_tmp1D9=Cyc_Tcutil_compress(t);struct Cyc_Absyn_ArrayInfo _tmp1DA;void*
_tmp1DB;struct Cyc_Absyn_AggrInfo _tmp1DC;union Cyc_Absyn_AggrInfoU_union _tmp1DD;
struct Cyc_List_List*_tmp1DE;struct Cyc_Absyn_TunionFieldInfo _tmp1DF;union Cyc_Absyn_TunionFieldInfoU_union
_tmp1E0;struct Cyc_Absyn_Tuniondecl*_tmp1E1;struct Cyc_Absyn_Tunionfield*_tmp1E2;
struct Cyc_List_List*_tmp1E3;_LL99: if((int)_tmp1D9 != 0)goto _LL9B;_LL9A: return 1;
_LL9B: if(_tmp1D9 <= (void*)4)goto _LLA3;if(*((int*)_tmp1D9)!= 1)goto _LL9D;_LL9C:
return 0;_LL9D: if(*((int*)_tmp1D9)!= 5)goto _LL9F;_LL9E: goto _LLA0;_LL9F: if(*((int*)
_tmp1D9)!= 12)goto _LLA1;_LLA0: goto _LLA2;_LLA1: if(*((int*)_tmp1D9)!= 13)goto _LLA3;
_LLA2: goto _LLA4;_LLA3: if((int)_tmp1D9 != 1)goto _LLA5;_LLA4: goto _LLA6;_LLA5: if(
_tmp1D9 <= (void*)4)goto _LLBB;if(*((int*)_tmp1D9)!= 6)goto _LLA7;_LLA6: goto _LLA8;
_LLA7: if(*((int*)_tmp1D9)!= 8)goto _LLA9;_LLA8: goto _LLAA;_LLA9: if(*((int*)_tmp1D9)
!= 18)goto _LLAB;_LLAA: return 1;_LLAB: if(*((int*)_tmp1D9)!= 7)goto _LLAD;_tmp1DA=((
struct Cyc_Absyn_ArrayType_struct*)_tmp1D9)->f1;_tmp1DB=(void*)_tmp1DA.elt_type;
_LLAC: return Cyc_Toc_atomic_typ(_tmp1DB);_LLAD: if(*((int*)_tmp1D9)!= 10)goto _LLAF;
_tmp1DC=((struct Cyc_Absyn_AggrType_struct*)_tmp1D9)->f1;_tmp1DD=_tmp1DC.aggr_info;
_LLAE:{union Cyc_Absyn_AggrInfoU_union _tmp1E4=_tmp1DD;_LLBE: if((_tmp1E4.UnknownAggr).tag
!= 0)goto _LLC0;_LLBF: return 0;_LLC0:;_LLC1: goto _LLBD;_LLBD:;}{struct Cyc_Absyn_Aggrdecl*
_tmp1E5=Cyc_Absyn_get_known_aggrdecl(_tmp1DD);if(_tmp1E5->impl == 0)return 0;{
struct Cyc_List_List*_tmp1E6=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp1E5->impl))->fields;
for(0;_tmp1E6 != 0;_tmp1E6=_tmp1E6->tl){if(!Cyc_Toc_atomic_typ((void*)((struct Cyc_Absyn_Aggrfield*)
_tmp1E6->hd)->type))return 0;}}return 1;}_LLAF: if(*((int*)_tmp1D9)!= 11)goto _LLB1;
_tmp1DE=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp1D9)->f2;_LLB0: for(0;_tmp1DE
!= 0;_tmp1DE=_tmp1DE->tl){if(!Cyc_Toc_atomic_typ((void*)((struct Cyc_Absyn_Aggrfield*)
_tmp1DE->hd)->type))return 0;}return 1;_LLB1: if(*((int*)_tmp1D9)!= 3)goto _LLB3;
_tmp1DF=((struct Cyc_Absyn_TunionFieldType_struct*)_tmp1D9)->f1;_tmp1E0=_tmp1DF.field_info;
if((((((struct Cyc_Absyn_TunionFieldType_struct*)_tmp1D9)->f1).field_info).KnownTunionfield).tag
!= 1)goto _LLB3;_tmp1E1=(_tmp1E0.KnownTunionfield).f1;_tmp1E2=(_tmp1E0.KnownTunionfield).f2;
_LLB2: _tmp1E3=_tmp1E2->typs;goto _LLB4;_LLB3: if(*((int*)_tmp1D9)!= 9)goto _LLB5;
_tmp1E3=((struct Cyc_Absyn_TupleType_struct*)_tmp1D9)->f1;_LLB4: for(0;_tmp1E3 != 0;
_tmp1E3=_tmp1E3->tl){if(!Cyc_Toc_atomic_typ((*((struct _tuple4*)_tmp1E3->hd)).f2))
return 0;}return 1;_LLB5: if(*((int*)_tmp1D9)!= 2)goto _LLB7;_LLB6: goto _LLB8;_LLB7:
if(*((int*)_tmp1D9)!= 4)goto _LLB9;_LLB8: goto _LLBA;_LLB9: if(*((int*)_tmp1D9)!= 14)
goto _LLBB;_LLBA: return 0;_LLBB:;_LLBC:({struct Cyc_String_pa_struct _tmp1E9;_tmp1E9.tag=
0;_tmp1E9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t));{void*_tmp1E7[1]={& _tmp1E9};((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp1E8="atomic_typ:  bad type %s";
_tag_dyneither(_tmp1E8,sizeof(char),25);}),_tag_dyneither(_tmp1E7,sizeof(void*),
1));}});_LL98:;}static int Cyc_Toc_is_void_star(void*t){void*_tmp1EA=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_PtrInfo _tmp1EB;void*_tmp1EC;_LLC3: if(_tmp1EA <= (void*)4)goto
_LLC5;if(*((int*)_tmp1EA)!= 4)goto _LLC5;_tmp1EB=((struct Cyc_Absyn_PointerType_struct*)
_tmp1EA)->f1;_tmp1EC=(void*)_tmp1EB.elt_typ;_LLC4: {void*_tmp1ED=Cyc_Tcutil_compress(
_tmp1EC);_LLC8: if((int)_tmp1ED != 0)goto _LLCA;_LLC9: return 1;_LLCA:;_LLCB: return 0;
_LLC7:;}_LLC5:;_LLC6: return 0;_LLC2:;}static int Cyc_Toc_is_poly_field(void*t,
struct _dyneither_ptr*f){void*_tmp1EE=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo
_tmp1EF;union Cyc_Absyn_AggrInfoU_union _tmp1F0;struct Cyc_List_List*_tmp1F1;_LLCD:
if(_tmp1EE <= (void*)4)goto _LLD1;if(*((int*)_tmp1EE)!= 10)goto _LLCF;_tmp1EF=((
struct Cyc_Absyn_AggrType_struct*)_tmp1EE)->f1;_tmp1F0=_tmp1EF.aggr_info;_LLCE: {
struct Cyc_Absyn_Aggrdecl*_tmp1F2=Cyc_Absyn_get_known_aggrdecl(_tmp1F0);if(
_tmp1F2->impl == 0)({void*_tmp1F3=0;((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp1F4="is_poly_field: type missing fields";
_tag_dyneither(_tmp1F4,sizeof(char),35);}),_tag_dyneither(_tmp1F3,sizeof(void*),
0));});_tmp1F1=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp1F2->impl))->fields;
goto _LLD0;}_LLCF: if(*((int*)_tmp1EE)!= 11)goto _LLD1;_tmp1F1=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp1EE)->f2;_LLD0: {struct Cyc_Absyn_Aggrfield*_tmp1F5=Cyc_Absyn_lookup_field(
_tmp1F1,f);if(_tmp1F5 == 0)({struct Cyc_String_pa_struct _tmp1F8;_tmp1F8.tag=0;
_tmp1F8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);{void*_tmp1F6[1]={&
_tmp1F8};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp1F7="is_poly_field: bad field %s";_tag_dyneither(_tmp1F7,sizeof(
char),28);}),_tag_dyneither(_tmp1F6,sizeof(void*),1));}});return Cyc_Toc_is_void_star((
void*)_tmp1F5->type);}_LLD1:;_LLD2:({struct Cyc_String_pa_struct _tmp1FB;_tmp1FB.tag=
0;_tmp1FB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t));{void*_tmp1F9[1]={& _tmp1FB};((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp1FA="is_poly_field: bad type %s";
_tag_dyneither(_tmp1FA,sizeof(char),27);}),_tag_dyneither(_tmp1F9,sizeof(void*),
1));}});_LLCC:;}static int Cyc_Toc_is_poly_project(struct Cyc_Absyn_Exp*e){void*
_tmp1FC=(void*)e->r;struct Cyc_Absyn_Exp*_tmp1FD;struct _dyneither_ptr*_tmp1FE;
struct Cyc_Absyn_Exp*_tmp1FF;struct _dyneither_ptr*_tmp200;_LLD4: if(*((int*)
_tmp1FC)!= 23)goto _LLD6;_tmp1FD=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp1FC)->f1;
_tmp1FE=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp1FC)->f2;_LLD5: return Cyc_Toc_is_poly_field((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp1FD->topt))->v,_tmp1FE);_LLD6: if(*((
int*)_tmp1FC)!= 24)goto _LLD8;_tmp1FF=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp1FC)->f1;_tmp200=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp1FC)->f2;_LLD7: {
void*_tmp201=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp1FF->topt))->v);struct Cyc_Absyn_PtrInfo _tmp202;void*_tmp203;_LLDB: if(_tmp201
<= (void*)4)goto _LLDD;if(*((int*)_tmp201)!= 4)goto _LLDD;_tmp202=((struct Cyc_Absyn_PointerType_struct*)
_tmp201)->f1;_tmp203=(void*)_tmp202.elt_typ;_LLDC: return Cyc_Toc_is_poly_field(
_tmp203,_tmp200);_LLDD:;_LLDE:({struct Cyc_String_pa_struct _tmp206;_tmp206.tag=0;
_tmp206.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp1FF->topt))->v));{void*_tmp204[1]={&
_tmp206};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp205="is_poly_project: bad type %s";_tag_dyneither(_tmp205,sizeof(
char),29);}),_tag_dyneither(_tmp204,sizeof(void*),1));}});_LLDA:;}_LLD8:;_LLD9:
return 0;_LLD3:;}static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_ptr(struct Cyc_Absyn_Exp*
s){return Cyc_Absyn_fncall_exp(Cyc_Toc__cycalloc_e,({struct Cyc_List_List*_tmp207=
_cycalloc(sizeof(*_tmp207));_tmp207->hd=s;_tmp207->tl=0;_tmp207;}),0);}static
struct Cyc_Absyn_Exp*Cyc_Toc_malloc_atomic(struct Cyc_Absyn_Exp*s){return Cyc_Absyn_fncall_exp(
Cyc_Toc__cycalloc_atomic_e,({struct Cyc_List_List*_tmp208=_cycalloc(sizeof(*
_tmp208));_tmp208->hd=s;_tmp208->tl=0;_tmp208;}),0);}static struct Cyc_Absyn_Exp*
Cyc_Toc_malloc_exp(void*t,struct Cyc_Absyn_Exp*s){if(Cyc_Toc_atomic_typ(t))return
Cyc_Toc_malloc_atomic(s);return Cyc_Toc_malloc_ptr(s);}static struct Cyc_Absyn_Exp*
Cyc_Toc_rmalloc_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s){return Cyc_Absyn_fncall_exp(
Cyc_Toc__region_malloc_e,({struct Cyc_Absyn_Exp*_tmp209[2];_tmp209[1]=s;_tmp209[0]=
rgn;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmp209,sizeof(struct Cyc_Absyn_Exp*),2));}),0);}static struct Cyc_Absyn_Exp*
Cyc_Toc_calloc_exp(void*elt_type,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n){
if(Cyc_Toc_atomic_typ(elt_type))return Cyc_Absyn_fncall_exp(Cyc_Toc__cyccalloc_atomic_e,({
struct Cyc_Absyn_Exp*_tmp20A[2];_tmp20A[1]=n;_tmp20A[0]=s;((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp20A,sizeof(struct Cyc_Absyn_Exp*),
2));}),0);else{return Cyc_Absyn_fncall_exp(Cyc_Toc__cyccalloc_e,({struct Cyc_Absyn_Exp*
_tmp20B[2];_tmp20B[1]=n;_tmp20B[0]=s;((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp20B,sizeof(struct Cyc_Absyn_Exp*),
2));}),0);}}static struct Cyc_Absyn_Exp*Cyc_Toc_rcalloc_exp(struct Cyc_Absyn_Exp*
rgn,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n){return Cyc_Absyn_fncall_exp(Cyc_Toc__region_calloc_e,({
struct Cyc_Absyn_Exp*_tmp20C[3];_tmp20C[2]=n;_tmp20C[1]=s;_tmp20C[0]=rgn;((struct
Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp20C,
sizeof(struct Cyc_Absyn_Exp*),3));}),0);}static struct Cyc_Absyn_Exp*Cyc_Toc_newthrow_exp(
struct Cyc_Absyn_Exp*e){return Cyc_Absyn_fncall_exp(Cyc_Toc__throw_e,({struct Cyc_List_List*
_tmp20D=_cycalloc(sizeof(*_tmp20D));_tmp20D->hd=e;_tmp20D->tl=0;_tmp20D;}),0);}
static struct Cyc_Absyn_Exp*Cyc_Toc_make_toplevel_dyn_arr(void*t,struct Cyc_Absyn_Exp*
sz,struct Cyc_Absyn_Exp*e){int is_string=0;{void*_tmp20E=(void*)e->r;union Cyc_Absyn_Cnst_union
_tmp20F;_LLE0: if(*((int*)_tmp20E)!= 0)goto _LLE2;_tmp20F=((struct Cyc_Absyn_Const_e_struct*)
_tmp20E)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)_tmp20E)->f1).String_c).tag
!= 5)goto _LLE2;_LLE1: is_string=1;goto _LLDF;_LLE2:;_LLE3: goto _LLDF;_LLDF:;}{
struct Cyc_Absyn_Exp*xexp;struct Cyc_Absyn_Exp*xplussz;if(is_string){struct _tuple1*
x=Cyc_Toc_temp_var();void*vd_typ=Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq,(
struct Cyc_Absyn_Exp*)sz,Cyc_Absyn_false_conref,0);struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_static_vardecl(
x,vd_typ,(struct Cyc_Absyn_Exp*)e);Cyc_Toc_result_decls=({struct Cyc_List_List*
_tmp210=_cycalloc(sizeof(*_tmp210));_tmp210->hd=Cyc_Absyn_new_decl((void*)({
struct Cyc_Absyn_Var_d_struct*_tmp211=_cycalloc(sizeof(*_tmp211));_tmp211[0]=({
struct Cyc_Absyn_Var_d_struct _tmp212;_tmp212.tag=0;_tmp212.f1=vd;_tmp212;});
_tmp211;}),0);_tmp210->tl=Cyc_Toc_result_decls;_tmp210;});xexp=Cyc_Absyn_var_exp(
x,0);xplussz=Cyc_Absyn_add_exp(xexp,sz,0);}else{xexp=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),
e);xplussz=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),Cyc_Absyn_add_exp(e,sz,0));}{
struct Cyc_Absyn_Exp*urm_exp;urm_exp=Cyc_Absyn_unresolvedmem_exp(0,({struct
_tuple8*_tmp213[3];_tmp213[2]=({struct _tuple8*_tmp216=_cycalloc(sizeof(*_tmp216));
_tmp216->f1=0;_tmp216->f2=xplussz;_tmp216;});_tmp213[1]=({struct _tuple8*_tmp215=
_cycalloc(sizeof(*_tmp215));_tmp215->f1=0;_tmp215->f2=xexp;_tmp215;});_tmp213[0]=({
struct _tuple8*_tmp214=_cycalloc(sizeof(*_tmp214));_tmp214->f1=0;_tmp214->f2=xexp;
_tmp214;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmp213,sizeof(struct _tuple8*),3));}),0);return urm_exp;}}}struct
Cyc_Toc_FallthruInfo{struct _dyneither_ptr*label;struct Cyc_List_List*binders;
struct Cyc_Dict_Dict next_case_env;};struct Cyc_Toc_Env{struct _dyneither_ptr**
break_lab;struct _dyneither_ptr**continue_lab;struct Cyc_Toc_FallthruInfo*
fallthru_info;struct Cyc_Dict_Dict varmap;int toplevel;struct _RegionHandle*rgn;};
static int Cyc_Toc_is_toplevel(struct Cyc_Toc_Env*nv){struct Cyc_Toc_Env _tmp218;int
_tmp219;struct Cyc_Toc_Env*_tmp217=nv;_tmp218=*_tmp217;_tmp219=_tmp218.toplevel;
return _tmp219;}static struct Cyc_Absyn_Exp*Cyc_Toc_lookup_varmap(struct Cyc_Toc_Env*
nv,struct _tuple1*x){struct Cyc_Toc_Env _tmp21B;struct Cyc_Dict_Dict _tmp21C;struct
Cyc_Toc_Env*_tmp21A=nv;_tmp21B=*_tmp21A;_tmp21C=_tmp21B.varmap;return((struct Cyc_Absyn_Exp*(*)(
struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup)(_tmp21C,x);}static struct
Cyc_Toc_Env*Cyc_Toc_empty_env(struct _RegionHandle*r){return({struct Cyc_Toc_Env*
_tmp21D=_region_malloc(r,sizeof(*_tmp21D));_tmp21D->break_lab=(struct
_dyneither_ptr**)0;_tmp21D->continue_lab=(struct _dyneither_ptr**)0;_tmp21D->fallthru_info=(
struct Cyc_Toc_FallthruInfo*)0;_tmp21D->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(
struct _RegionHandle*,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_rempty)(
r,Cyc_Absyn_qvar_cmp);_tmp21D->toplevel=(int)1;_tmp21D->rgn=(struct _RegionHandle*)
r;_tmp21D;});}static struct Cyc_Toc_Env*Cyc_Toc_share_env(struct _RegionHandle*r,
struct Cyc_Toc_Env*e){struct Cyc_Toc_Env _tmp21F;struct _dyneither_ptr**_tmp220;
struct _dyneither_ptr**_tmp221;struct Cyc_Toc_FallthruInfo*_tmp222;struct Cyc_Dict_Dict
_tmp223;int _tmp224;struct Cyc_Toc_Env*_tmp21E=e;_tmp21F=*_tmp21E;_tmp220=_tmp21F.break_lab;
_tmp221=_tmp21F.continue_lab;_tmp222=_tmp21F.fallthru_info;_tmp223=_tmp21F.varmap;
_tmp224=_tmp21F.toplevel;return({struct Cyc_Toc_Env*_tmp225=_region_malloc(r,
sizeof(*_tmp225));_tmp225->break_lab=(struct _dyneither_ptr**)((struct
_dyneither_ptr**)_tmp220);_tmp225->continue_lab=(struct _dyneither_ptr**)((struct
_dyneither_ptr**)_tmp221);_tmp225->fallthru_info=(struct Cyc_Toc_FallthruInfo*)
_tmp222;_tmp225->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp223);_tmp225->toplevel=(
int)_tmp224;_tmp225->rgn=(struct _RegionHandle*)r;_tmp225;});}static struct Cyc_Toc_Env*
Cyc_Toc_clear_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*e){struct Cyc_Toc_Env
_tmp227;struct _dyneither_ptr**_tmp228;struct _dyneither_ptr**_tmp229;struct Cyc_Toc_FallthruInfo*
_tmp22A;struct Cyc_Dict_Dict _tmp22B;int _tmp22C;struct Cyc_Toc_Env*_tmp226=e;
_tmp227=*_tmp226;_tmp228=_tmp227.break_lab;_tmp229=_tmp227.continue_lab;_tmp22A=
_tmp227.fallthru_info;_tmp22B=_tmp227.varmap;_tmp22C=_tmp227.toplevel;return({
struct Cyc_Toc_Env*_tmp22D=_region_malloc(r,sizeof(*_tmp22D));_tmp22D->break_lab=(
struct _dyneither_ptr**)((struct _dyneither_ptr**)_tmp228);_tmp22D->continue_lab=(
struct _dyneither_ptr**)((struct _dyneither_ptr**)_tmp229);_tmp22D->fallthru_info=(
struct Cyc_Toc_FallthruInfo*)_tmp22A;_tmp22D->varmap=(struct Cyc_Dict_Dict)((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(
r,_tmp22B);_tmp22D->toplevel=(int)0;_tmp22D->rgn=(struct _RegionHandle*)r;_tmp22D;});}
static struct Cyc_Toc_Env*Cyc_Toc_set_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*
e){struct Cyc_Toc_Env _tmp22F;struct _dyneither_ptr**_tmp230;struct _dyneither_ptr**
_tmp231;struct Cyc_Toc_FallthruInfo*_tmp232;struct Cyc_Dict_Dict _tmp233;int _tmp234;
struct Cyc_Toc_Env*_tmp22E=e;_tmp22F=*_tmp22E;_tmp230=_tmp22F.break_lab;_tmp231=
_tmp22F.continue_lab;_tmp232=_tmp22F.fallthru_info;_tmp233=_tmp22F.varmap;
_tmp234=_tmp22F.toplevel;return({struct Cyc_Toc_Env*_tmp235=_region_malloc(r,
sizeof(*_tmp235));_tmp235->break_lab=(struct _dyneither_ptr**)((struct
_dyneither_ptr**)_tmp230);_tmp235->continue_lab=(struct _dyneither_ptr**)((struct
_dyneither_ptr**)_tmp231);_tmp235->fallthru_info=(struct Cyc_Toc_FallthruInfo*)
_tmp232;_tmp235->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp233);_tmp235->toplevel=(
int)1;_tmp235->rgn=(struct _RegionHandle*)r;_tmp235;});}static struct Cyc_Toc_Env*
Cyc_Toc_add_varmap(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _tuple1*x,
struct Cyc_Absyn_Exp*y){{union Cyc_Absyn_Nmspace_union _tmp236=(*x).f1;_LLE5: if((
_tmp236.Rel_n).tag != 1)goto _LLE7;_LLE6:({struct Cyc_String_pa_struct _tmp239;
_tmp239.tag=0;_tmp239.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
x));{void*_tmp237[1]={& _tmp239};((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp238="Toc::add_varmap on Rel_n: %s\n";
_tag_dyneither(_tmp238,sizeof(char),30);}),_tag_dyneither(_tmp237,sizeof(void*),
1));}});_LLE7:;_LLE8: goto _LLE4;_LLE4:;}{struct Cyc_Toc_Env _tmp23B;struct
_dyneither_ptr**_tmp23C;struct _dyneither_ptr**_tmp23D;struct Cyc_Toc_FallthruInfo*
_tmp23E;struct Cyc_Dict_Dict _tmp23F;int _tmp240;struct Cyc_Toc_Env*_tmp23A=e;
_tmp23B=*_tmp23A;_tmp23C=_tmp23B.break_lab;_tmp23D=_tmp23B.continue_lab;_tmp23E=
_tmp23B.fallthru_info;_tmp23F=_tmp23B.varmap;_tmp240=_tmp23B.toplevel;{struct Cyc_Dict_Dict
_tmp241=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,struct Cyc_Absyn_Exp*
v))Cyc_Dict_insert)(((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))
Cyc_Dict_rshare)(r,_tmp23F),x,y);return({struct Cyc_Toc_Env*_tmp242=
_region_malloc(r,sizeof(*_tmp242));_tmp242->break_lab=(struct _dyneither_ptr**)((
struct _dyneither_ptr**)_tmp23C);_tmp242->continue_lab=(struct _dyneither_ptr**)((
struct _dyneither_ptr**)_tmp23D);_tmp242->fallthru_info=(struct Cyc_Toc_FallthruInfo*)
_tmp23E;_tmp242->varmap=(struct Cyc_Dict_Dict)_tmp241;_tmp242->toplevel=(int)
_tmp240;_tmp242->rgn=(struct _RegionHandle*)r;_tmp242;});}}}static struct Cyc_Toc_Env*
Cyc_Toc_loop_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e){struct Cyc_Toc_Env
_tmp244;struct _dyneither_ptr**_tmp245;struct _dyneither_ptr**_tmp246;struct Cyc_Toc_FallthruInfo*
_tmp247;struct Cyc_Dict_Dict _tmp248;int _tmp249;struct Cyc_Toc_Env*_tmp243=e;
_tmp244=*_tmp243;_tmp245=_tmp244.break_lab;_tmp246=_tmp244.continue_lab;_tmp247=
_tmp244.fallthru_info;_tmp248=_tmp244.varmap;_tmp249=_tmp244.toplevel;return({
struct Cyc_Toc_Env*_tmp24A=_region_malloc(r,sizeof(*_tmp24A));_tmp24A->break_lab=(
struct _dyneither_ptr**)0;_tmp24A->continue_lab=(struct _dyneither_ptr**)0;_tmp24A->fallthru_info=(
struct Cyc_Toc_FallthruInfo*)_tmp247;_tmp24A->varmap=(struct Cyc_Dict_Dict)((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(
r,_tmp248);_tmp24A->toplevel=(int)_tmp249;_tmp24A->rgn=(struct _RegionHandle*)r;
_tmp24A;});}static struct Cyc_Toc_Env*Cyc_Toc_non_last_switchclause_env(struct
_RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*break_l,struct
_dyneither_ptr*fallthru_l,struct Cyc_List_List*fallthru_binders,struct Cyc_Toc_Env*
next_case_env){struct Cyc_List_List*fallthru_vars=0;for(0;fallthru_binders != 0;
fallthru_binders=fallthru_binders->tl){fallthru_vars=({struct Cyc_List_List*
_tmp24B=_region_malloc(r,sizeof(*_tmp24B));_tmp24B->hd=((struct Cyc_Absyn_Vardecl*)
fallthru_binders->hd)->name;_tmp24B->tl=fallthru_vars;_tmp24B;});}fallthru_vars=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fallthru_vars);{
struct Cyc_Toc_Env _tmp24D;struct _dyneither_ptr**_tmp24E;struct _dyneither_ptr**
_tmp24F;struct Cyc_Toc_FallthruInfo*_tmp250;struct Cyc_Dict_Dict _tmp251;int _tmp252;
struct Cyc_Toc_Env*_tmp24C=e;_tmp24D=*_tmp24C;_tmp24E=_tmp24D.break_lab;_tmp24F=
_tmp24D.continue_lab;_tmp250=_tmp24D.fallthru_info;_tmp251=_tmp24D.varmap;
_tmp252=_tmp24D.toplevel;{struct Cyc_Toc_Env _tmp254;struct Cyc_Dict_Dict _tmp255;
struct Cyc_Toc_Env*_tmp253=next_case_env;_tmp254=*_tmp253;_tmp255=_tmp254.varmap;{
struct Cyc_Toc_FallthruInfo*fi=({struct Cyc_Toc_FallthruInfo*_tmp258=
_region_malloc(r,sizeof(*_tmp258));_tmp258->label=fallthru_l;_tmp258->binders=
fallthru_vars;_tmp258->next_case_env=((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp255);_tmp258;});
return({struct Cyc_Toc_Env*_tmp256=_region_malloc(r,sizeof(*_tmp256));_tmp256->break_lab=(
struct _dyneither_ptr**)({struct _dyneither_ptr**_tmp257=_region_malloc(r,sizeof(*
_tmp257));_tmp257[0]=break_l;_tmp257;});_tmp256->continue_lab=(struct
_dyneither_ptr**)((struct _dyneither_ptr**)_tmp24F);_tmp256->fallthru_info=(
struct Cyc_Toc_FallthruInfo*)fi;_tmp256->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(
struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp251);_tmp256->toplevel=(
int)_tmp252;_tmp256->rgn=(struct _RegionHandle*)r;_tmp256;});}}}}static struct Cyc_Toc_Env*
Cyc_Toc_last_switchclause_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct
_dyneither_ptr*break_l){struct Cyc_Toc_Env _tmp25A;struct _dyneither_ptr**_tmp25B;
struct _dyneither_ptr**_tmp25C;struct Cyc_Toc_FallthruInfo*_tmp25D;struct Cyc_Dict_Dict
_tmp25E;int _tmp25F;struct Cyc_Toc_Env*_tmp259=e;_tmp25A=*_tmp259;_tmp25B=_tmp25A.break_lab;
_tmp25C=_tmp25A.continue_lab;_tmp25D=_tmp25A.fallthru_info;_tmp25E=_tmp25A.varmap;
_tmp25F=_tmp25A.toplevel;return({struct Cyc_Toc_Env*_tmp260=_region_malloc(r,
sizeof(*_tmp260));_tmp260->break_lab=(struct _dyneither_ptr**)({struct
_dyneither_ptr**_tmp261=_region_malloc(r,sizeof(*_tmp261));_tmp261[0]=break_l;
_tmp261;});_tmp260->continue_lab=(struct _dyneither_ptr**)((struct _dyneither_ptr**)
_tmp25C);_tmp260->fallthru_info=(struct Cyc_Toc_FallthruInfo*)0;_tmp260->varmap=(
struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))
Cyc_Dict_rshare)(r,_tmp25E);_tmp260->toplevel=(int)_tmp25F;_tmp260->rgn=(struct
_RegionHandle*)r;_tmp260;});}static struct Cyc_Toc_Env*Cyc_Toc_switch_as_switch_env(
struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*next_l){struct Cyc_Toc_Env
_tmp263;struct _dyneither_ptr**_tmp264;struct _dyneither_ptr**_tmp265;struct Cyc_Toc_FallthruInfo*
_tmp266;struct Cyc_Dict_Dict _tmp267;int _tmp268;struct Cyc_Toc_Env*_tmp262=e;
_tmp263=*_tmp262;_tmp264=_tmp263.break_lab;_tmp265=_tmp263.continue_lab;_tmp266=
_tmp263.fallthru_info;_tmp267=_tmp263.varmap;_tmp268=_tmp263.toplevel;return({
struct Cyc_Toc_Env*_tmp269=_region_malloc(r,sizeof(*_tmp269));_tmp269->break_lab=(
struct _dyneither_ptr**)0;_tmp269->continue_lab=(struct _dyneither_ptr**)((struct
_dyneither_ptr**)_tmp265);_tmp269->fallthru_info=(struct Cyc_Toc_FallthruInfo*)({
struct Cyc_Toc_FallthruInfo*_tmp26A=_region_malloc(r,sizeof(*_tmp26A));_tmp26A->label=
next_l;_tmp26A->binders=0;_tmp26A->next_case_env=((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_rempty)(r,Cyc_Absyn_qvar_cmp);
_tmp26A;});_tmp269->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp267);_tmp269->toplevel=(
int)_tmp268;_tmp269->rgn=(struct _RegionHandle*)r;_tmp269;});}static void Cyc_Toc_exp_to_c(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e);static void Cyc_Toc_stmt_to_c(struct
Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*s);static int Cyc_Toc_need_null_check(struct
Cyc_Absyn_Exp*e){void*_tmp26B=(void*)e->annot;_LLEA: if(*((void**)_tmp26B)!= Cyc_CfFlowInfo_UnknownZ)
goto _LLEC;_LLEB: return Cyc_Toc_is_nullable((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);_LLEC: if(*((void**)_tmp26B)!= Cyc_CfFlowInfo_NotZero)
goto _LLEE;_LLED: return 0;_LLEE: if(_tmp26B != Cyc_CfFlowInfo_IsZero)goto _LLF0;_LLEF:({
void*_tmp26C=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp26D="dereference of NULL pointer";
_tag_dyneither(_tmp26D,sizeof(char),28);}),_tag_dyneither(_tmp26C,sizeof(void*),
0));});return 0;_LLF0: if(_tmp26B != Cyc_Absyn_EmptyAnnot)goto _LLF2;_LLF1: return 0;
_LLF2: if(*((void**)_tmp26B)!= Cyc_CfFlowInfo_HasTagCmps)goto _LLF4;_LLF3:({void*
_tmp26E=0;Cyc_Tcutil_warn(e->loc,({const char*_tmp26F="compiler oddity: pointer compared to tag type";
_tag_dyneither(_tmp26F,sizeof(char),46);}),_tag_dyneither(_tmp26E,sizeof(void*),
0));});return Cyc_Toc_is_nullable((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
_LLF4:;_LLF5:({void*_tmp270=0;((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp271="need_null_check";
_tag_dyneither(_tmp271,sizeof(char),16);}),_tag_dyneither(_tmp270,sizeof(void*),
0));});_LLE9:;}static struct Cyc_List_List*Cyc_Toc_get_relns(struct Cyc_Absyn_Exp*e){
void*_tmp272=(void*)e->annot;struct Cyc_List_List*_tmp273;struct Cyc_List_List*
_tmp274;_LLF7: if(*((void**)_tmp272)!= Cyc_CfFlowInfo_UnknownZ)goto _LLF9;_tmp273=((
struct Cyc_CfFlowInfo_UnknownZ_struct*)_tmp272)->f1;_LLF8: return _tmp273;_LLF9: if(*((
void**)_tmp272)!= Cyc_CfFlowInfo_NotZero)goto _LLFB;_tmp274=((struct Cyc_CfFlowInfo_NotZero_struct*)
_tmp272)->f1;_LLFA: return _tmp274;_LLFB: if(_tmp272 != Cyc_CfFlowInfo_IsZero)goto
_LLFD;_LLFC:({void*_tmp275=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp276="dereference of NULL pointer";
_tag_dyneither(_tmp276,sizeof(char),28);}),_tag_dyneither(_tmp275,sizeof(void*),
0));});return 0;_LLFD: if(*((void**)_tmp272)!= Cyc_CfFlowInfo_HasTagCmps)goto _LLFF;
_LLFE: goto _LL100;_LLFF: if(_tmp272 != Cyc_Absyn_EmptyAnnot)goto _LL101;_LL100:
return 0;_LL101:;_LL102:({void*_tmp277=0;((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp278="get_relns";
_tag_dyneither(_tmp278,sizeof(char),10);}),_tag_dyneither(_tmp277,sizeof(void*),
0));});_LLF6:;}static int Cyc_Toc_check_const_array(unsigned int i,void*t){void*
_tmp279=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp27A;struct Cyc_Absyn_PtrAtts
_tmp27B;struct Cyc_Absyn_Conref*_tmp27C;struct Cyc_Absyn_Conref*_tmp27D;struct Cyc_Absyn_ArrayInfo
_tmp27E;struct Cyc_Absyn_Exp*_tmp27F;_LL104: if(_tmp279 <= (void*)4)goto _LL108;if(*((
int*)_tmp279)!= 4)goto _LL106;_tmp27A=((struct Cyc_Absyn_PointerType_struct*)
_tmp279)->f1;_tmp27B=_tmp27A.ptr_atts;_tmp27C=_tmp27B.bounds;_tmp27D=_tmp27B.zero_term;
_LL105: {void*_tmp280=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp27C);struct
Cyc_Absyn_Exp*_tmp281;_LL10B: if((int)_tmp280 != 0)goto _LL10D;_LL10C: return 0;
_LL10D: if(_tmp280 <= (void*)1)goto _LL10A;if(*((int*)_tmp280)!= 0)goto _LL10A;
_tmp281=((struct Cyc_Absyn_Upper_b_struct*)_tmp280)->f1;_LL10E: {unsigned int
_tmp283;int _tmp284;struct _tuple5 _tmp282=Cyc_Evexp_eval_const_uint_exp(_tmp281);
_tmp283=_tmp282.f1;_tmp284=_tmp282.f2;return _tmp284  && i <= _tmp283;}_LL10A:;}
_LL106: if(*((int*)_tmp279)!= 7)goto _LL108;_tmp27E=((struct Cyc_Absyn_ArrayType_struct*)
_tmp279)->f1;_tmp27F=_tmp27E.num_elts;_LL107: if(_tmp27F == 0)return 0;{
unsigned int _tmp286;int _tmp287;struct _tuple5 _tmp285=Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)_tmp27F);_tmp286=_tmp285.f1;_tmp287=_tmp285.f2;return
_tmp287  && i <= _tmp286;}_LL108:;_LL109: return 0;_LL103:;}static int Cyc_Toc_check_leq_size_var(
struct Cyc_List_List*relns,struct Cyc_Absyn_Vardecl*v,struct Cyc_Absyn_Vardecl*y){
for(0;relns != 0;relns=relns->tl){struct Cyc_CfFlowInfo_Reln*_tmp288=(struct Cyc_CfFlowInfo_Reln*)
relns->hd;if(_tmp288->vd != y)continue;{union Cyc_CfFlowInfo_RelnOp_union _tmp289=
_tmp288->rop;struct Cyc_Absyn_Vardecl*_tmp28A;struct Cyc_Absyn_Vardecl*_tmp28B;
_LL110: if((_tmp289.LessNumelts).tag != 2)goto _LL112;_tmp28A=(_tmp289.LessNumelts).f1;
_LL111: _tmp28B=_tmp28A;goto _LL113;_LL112: if((_tmp289.LessEqNumelts).tag != 4)goto
_LL114;_tmp28B=(_tmp289.LessEqNumelts).f1;_LL113: if(_tmp28B == v)return 1;else{
goto _LL10F;}_LL114:;_LL115: continue;_LL10F:;}}return 0;}static int Cyc_Toc_check_leq_size(
struct Cyc_List_List*relns,struct Cyc_Absyn_Vardecl*v,struct Cyc_Absyn_Exp*e){{void*
_tmp28C=(void*)e->r;void*_tmp28D;struct Cyc_Absyn_Vardecl*_tmp28E;void*_tmp28F;
struct Cyc_Absyn_Vardecl*_tmp290;void*_tmp291;struct Cyc_Absyn_Vardecl*_tmp292;
void*_tmp293;struct Cyc_Absyn_Vardecl*_tmp294;void*_tmp295;struct Cyc_List_List*
_tmp296;struct Cyc_List_List _tmp297;struct Cyc_Absyn_Exp*_tmp298;_LL117: if(*((int*)
_tmp28C)!= 1)goto _LL119;_tmp28D=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp28C)->f2;
if(_tmp28D <= (void*)1)goto _LL119;if(*((int*)_tmp28D)!= 4)goto _LL119;_tmp28E=((
struct Cyc_Absyn_Pat_b_struct*)_tmp28D)->f1;_LL118: _tmp290=_tmp28E;goto _LL11A;
_LL119: if(*((int*)_tmp28C)!= 1)goto _LL11B;_tmp28F=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp28C)->f2;if(_tmp28F <= (void*)1)goto _LL11B;if(*((int*)_tmp28F)!= 3)goto _LL11B;
_tmp290=((struct Cyc_Absyn_Local_b_struct*)_tmp28F)->f1;_LL11A: _tmp292=_tmp290;
goto _LL11C;_LL11B: if(*((int*)_tmp28C)!= 1)goto _LL11D;_tmp291=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp28C)->f2;if(_tmp291 <= (void*)1)goto _LL11D;if(*((int*)_tmp291)!= 0)goto _LL11D;
_tmp292=((struct Cyc_Absyn_Global_b_struct*)_tmp291)->f1;_LL11C: _tmp294=_tmp292;
goto _LL11E;_LL11D: if(*((int*)_tmp28C)!= 1)goto _LL11F;_tmp293=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp28C)->f2;if(_tmp293 <= (void*)1)goto _LL11F;if(*((int*)_tmp293)!= 2)goto _LL11F;
_tmp294=((struct Cyc_Absyn_Param_b_struct*)_tmp293)->f1;_LL11E: if(_tmp294->escapes)
return 0;if(Cyc_Toc_check_leq_size_var(relns,v,_tmp294))return 1;goto _LL116;_LL11F:
if(*((int*)_tmp28C)!= 3)goto _LL121;_tmp295=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp28C)->f1;if((int)_tmp295 != 19)goto _LL121;_tmp296=((struct Cyc_Absyn_Primop_e_struct*)
_tmp28C)->f2;if(_tmp296 == 0)goto _LL121;_tmp297=*_tmp296;_tmp298=(struct Cyc_Absyn_Exp*)
_tmp297.hd;_LL120:{void*_tmp299=(void*)_tmp298->r;void*_tmp29A;struct Cyc_Absyn_Vardecl*
_tmp29B;void*_tmp29C;struct Cyc_Absyn_Vardecl*_tmp29D;void*_tmp29E;struct Cyc_Absyn_Vardecl*
_tmp29F;void*_tmp2A0;struct Cyc_Absyn_Vardecl*_tmp2A1;_LL124: if(*((int*)_tmp299)
!= 1)goto _LL126;_tmp29A=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp299)->f2;if(
_tmp29A <= (void*)1)goto _LL126;if(*((int*)_tmp29A)!= 4)goto _LL126;_tmp29B=((
struct Cyc_Absyn_Pat_b_struct*)_tmp29A)->f1;_LL125: _tmp29D=_tmp29B;goto _LL127;
_LL126: if(*((int*)_tmp299)!= 1)goto _LL128;_tmp29C=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp299)->f2;if(_tmp29C <= (void*)1)goto _LL128;if(*((int*)_tmp29C)!= 3)goto _LL128;
_tmp29D=((struct Cyc_Absyn_Local_b_struct*)_tmp29C)->f1;_LL127: _tmp29F=_tmp29D;
goto _LL129;_LL128: if(*((int*)_tmp299)!= 1)goto _LL12A;_tmp29E=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp299)->f2;if(_tmp29E <= (void*)1)goto _LL12A;if(*((int*)_tmp29E)!= 0)goto _LL12A;
_tmp29F=((struct Cyc_Absyn_Global_b_struct*)_tmp29E)->f1;_LL129: _tmp2A1=_tmp29F;
goto _LL12B;_LL12A: if(*((int*)_tmp299)!= 1)goto _LL12C;_tmp2A0=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp299)->f2;if(_tmp2A0 <= (void*)1)goto _LL12C;if(*((int*)_tmp2A0)!= 2)goto _LL12C;
_tmp2A1=((struct Cyc_Absyn_Param_b_struct*)_tmp2A0)->f1;_LL12B: return _tmp2A1 == v;
_LL12C:;_LL12D: goto _LL123;_LL123:;}goto _LL116;_LL121:;_LL122: goto _LL116;_LL116:;}
return 0;}static int Cyc_Toc_check_bounds(struct Cyc_List_List*relns,struct Cyc_Absyn_Exp*
a,struct Cyc_Absyn_Exp*i){{void*_tmp2A2=(void*)a->r;void*_tmp2A3;struct Cyc_Absyn_Vardecl*
_tmp2A4;void*_tmp2A5;struct Cyc_Absyn_Vardecl*_tmp2A6;void*_tmp2A7;struct Cyc_Absyn_Vardecl*
_tmp2A8;void*_tmp2A9;struct Cyc_Absyn_Vardecl*_tmp2AA;_LL12F: if(*((int*)_tmp2A2)
!= 1)goto _LL131;_tmp2A3=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp2A2)->f2;if(
_tmp2A3 <= (void*)1)goto _LL131;if(*((int*)_tmp2A3)!= 4)goto _LL131;_tmp2A4=((
struct Cyc_Absyn_Pat_b_struct*)_tmp2A3)->f1;_LL130: _tmp2A6=_tmp2A4;goto _LL132;
_LL131: if(*((int*)_tmp2A2)!= 1)goto _LL133;_tmp2A5=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp2A2)->f2;if(_tmp2A5 <= (void*)1)goto _LL133;if(*((int*)_tmp2A5)!= 3)goto _LL133;
_tmp2A6=((struct Cyc_Absyn_Local_b_struct*)_tmp2A5)->f1;_LL132: _tmp2A8=_tmp2A6;
goto _LL134;_LL133: if(*((int*)_tmp2A2)!= 1)goto _LL135;_tmp2A7=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp2A2)->f2;if(_tmp2A7 <= (void*)1)goto _LL135;if(*((int*)_tmp2A7)!= 0)goto _LL135;
_tmp2A8=((struct Cyc_Absyn_Global_b_struct*)_tmp2A7)->f1;_LL134: _tmp2AA=_tmp2A8;
goto _LL136;_LL135: if(*((int*)_tmp2A2)!= 1)goto _LL137;_tmp2A9=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp2A2)->f2;if(_tmp2A9 <= (void*)1)goto _LL137;if(*((int*)_tmp2A9)!= 2)goto _LL137;
_tmp2AA=((struct Cyc_Absyn_Param_b_struct*)_tmp2A9)->f1;_LL136: if(_tmp2AA->escapes)
return 0;inner_loop: {void*_tmp2AB=(void*)i->r;void*_tmp2AC;struct Cyc_Absyn_Exp*
_tmp2AD;union Cyc_Absyn_Cnst_union _tmp2AE;void*_tmp2AF;int _tmp2B0;union Cyc_Absyn_Cnst_union
_tmp2B1;void*_tmp2B2;int _tmp2B3;union Cyc_Absyn_Cnst_union _tmp2B4;void*_tmp2B5;
int _tmp2B6;void*_tmp2B7;struct Cyc_List_List*_tmp2B8;struct Cyc_List_List _tmp2B9;
struct Cyc_Absyn_Exp*_tmp2BA;struct Cyc_List_List*_tmp2BB;struct Cyc_List_List
_tmp2BC;struct Cyc_Absyn_Exp*_tmp2BD;void*_tmp2BE;struct Cyc_Absyn_Vardecl*_tmp2BF;
void*_tmp2C0;struct Cyc_Absyn_Vardecl*_tmp2C1;void*_tmp2C2;struct Cyc_Absyn_Vardecl*
_tmp2C3;void*_tmp2C4;struct Cyc_Absyn_Vardecl*_tmp2C5;_LL13A: if(*((int*)_tmp2AB)
!= 15)goto _LL13C;_tmp2AC=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp2AB)->f1;
_tmp2AD=((struct Cyc_Absyn_Cast_e_struct*)_tmp2AB)->f2;_LL13B: i=_tmp2AD;goto
inner_loop;_LL13C: if(*((int*)_tmp2AB)!= 0)goto _LL13E;_tmp2AE=((struct Cyc_Absyn_Const_e_struct*)
_tmp2AB)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)_tmp2AB)->f1).Int_c).tag != 2)
goto _LL13E;_tmp2AF=(_tmp2AE.Int_c).f1;if((int)_tmp2AF != 2)goto _LL13E;_tmp2B0=(
_tmp2AE.Int_c).f2;_LL13D: _tmp2B3=_tmp2B0;goto _LL13F;_LL13E: if(*((int*)_tmp2AB)!= 
0)goto _LL140;_tmp2B1=((struct Cyc_Absyn_Const_e_struct*)_tmp2AB)->f1;if(((((
struct Cyc_Absyn_Const_e_struct*)_tmp2AB)->f1).Int_c).tag != 2)goto _LL140;_tmp2B2=(
_tmp2B1.Int_c).f1;if((int)_tmp2B2 != 0)goto _LL140;_tmp2B3=(_tmp2B1.Int_c).f2;
_LL13F: return _tmp2B3 >= 0  && Cyc_Toc_check_const_array((unsigned int)(_tmp2B3 + 1),(
void*)_tmp2AA->type);_LL140: if(*((int*)_tmp2AB)!= 0)goto _LL142;_tmp2B4=((struct
Cyc_Absyn_Const_e_struct*)_tmp2AB)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)
_tmp2AB)->f1).Int_c).tag != 2)goto _LL142;_tmp2B5=(_tmp2B4.Int_c).f1;if((int)
_tmp2B5 != 1)goto _LL142;_tmp2B6=(_tmp2B4.Int_c).f2;_LL141: return Cyc_Toc_check_const_array((
unsigned int)(_tmp2B6 + 1),(void*)_tmp2AA->type);_LL142: if(*((int*)_tmp2AB)!= 3)
goto _LL144;_tmp2B7=(void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp2AB)->f1;if((
int)_tmp2B7 != 4)goto _LL144;_tmp2B8=((struct Cyc_Absyn_Primop_e_struct*)_tmp2AB)->f2;
if(_tmp2B8 == 0)goto _LL144;_tmp2B9=*_tmp2B8;_tmp2BA=(struct Cyc_Absyn_Exp*)_tmp2B9.hd;
_tmp2BB=_tmp2B9.tl;if(_tmp2BB == 0)goto _LL144;_tmp2BC=*_tmp2BB;_tmp2BD=(struct Cyc_Absyn_Exp*)
_tmp2BC.hd;_LL143: return Cyc_Toc_check_leq_size(relns,_tmp2AA,_tmp2BD);_LL144: if(*((
int*)_tmp2AB)!= 1)goto _LL146;_tmp2BE=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp2AB)->f2;if(_tmp2BE <= (void*)1)goto _LL146;if(*((int*)_tmp2BE)!= 4)goto _LL146;
_tmp2BF=((struct Cyc_Absyn_Pat_b_struct*)_tmp2BE)->f1;_LL145: _tmp2C1=_tmp2BF;goto
_LL147;_LL146: if(*((int*)_tmp2AB)!= 1)goto _LL148;_tmp2C0=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp2AB)->f2;if(_tmp2C0 <= (void*)1)goto _LL148;if(*((int*)_tmp2C0)!= 3)goto _LL148;
_tmp2C1=((struct Cyc_Absyn_Local_b_struct*)_tmp2C0)->f1;_LL147: _tmp2C3=_tmp2C1;
goto _LL149;_LL148: if(*((int*)_tmp2AB)!= 1)goto _LL14A;_tmp2C2=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp2AB)->f2;if(_tmp2C2 <= (void*)1)goto _LL14A;if(*((int*)_tmp2C2)!= 0)goto _LL14A;
_tmp2C3=((struct Cyc_Absyn_Global_b_struct*)_tmp2C2)->f1;_LL149: _tmp2C5=_tmp2C3;
goto _LL14B;_LL14A: if(*((int*)_tmp2AB)!= 1)goto _LL14C;_tmp2C4=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp2AB)->f2;if(_tmp2C4 <= (void*)1)goto _LL14C;if(*((int*)_tmp2C4)!= 2)goto _LL14C;
_tmp2C5=((struct Cyc_Absyn_Param_b_struct*)_tmp2C4)->f1;_LL14B: if(_tmp2C5->escapes)
return 0;{struct Cyc_List_List*_tmp2C6=relns;for(0;_tmp2C6 != 0;_tmp2C6=_tmp2C6->tl){
struct Cyc_CfFlowInfo_Reln*_tmp2C7=(struct Cyc_CfFlowInfo_Reln*)_tmp2C6->hd;if(
_tmp2C7->vd == _tmp2C5){union Cyc_CfFlowInfo_RelnOp_union _tmp2C8=_tmp2C7->rop;
struct Cyc_Absyn_Vardecl*_tmp2C9;struct Cyc_Absyn_Vardecl*_tmp2CA;void*_tmp2CB;
unsigned int _tmp2CC;_LL14F: if((_tmp2C8.LessNumelts).tag != 2)goto _LL151;_tmp2C9=(
_tmp2C8.LessNumelts).f1;_LL150: if(_tmp2AA == _tmp2C9)return 1;else{goto _LL14E;}
_LL151: if((_tmp2C8.LessVar).tag != 1)goto _LL153;_tmp2CA=(_tmp2C8.LessVar).f1;
_tmp2CB=(_tmp2C8.LessVar).f2;_LL152:{struct _tuple0 _tmp2CE=({struct _tuple0 _tmp2CD;
_tmp2CD.f1=Cyc_Tcutil_compress(_tmp2CB);_tmp2CD.f2=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(a->topt))->v);_tmp2CD;});void*_tmp2CF;void*
_tmp2D0;void*_tmp2D1;struct Cyc_Absyn_PtrInfo _tmp2D2;struct Cyc_Absyn_PtrAtts
_tmp2D3;struct Cyc_Absyn_Conref*_tmp2D4;_LL158: _tmp2CF=_tmp2CE.f1;if(_tmp2CF <= (
void*)4)goto _LL15A;if(*((int*)_tmp2CF)!= 18)goto _LL15A;_tmp2D0=(void*)((struct
Cyc_Absyn_TagType_struct*)_tmp2CF)->f1;_tmp2D1=_tmp2CE.f2;if(_tmp2D1 <= (void*)4)
goto _LL15A;if(*((int*)_tmp2D1)!= 4)goto _LL15A;_tmp2D2=((struct Cyc_Absyn_PointerType_struct*)
_tmp2D1)->f1;_tmp2D3=_tmp2D2.ptr_atts;_tmp2D4=_tmp2D3.bounds;_LL159:{void*
_tmp2D5=Cyc_Absyn_conref_val(_tmp2D4);struct Cyc_Absyn_Exp*_tmp2D6;_LL15D: if(
_tmp2D5 <= (void*)1)goto _LL15F;if(*((int*)_tmp2D5)!= 0)goto _LL15F;_tmp2D6=((
struct Cyc_Absyn_Upper_b_struct*)_tmp2D5)->f1;_LL15E: {struct Cyc_Absyn_Exp*
_tmp2D7=Cyc_Absyn_cast_exp(Cyc_Absyn_uint_typ,Cyc_Absyn_valueof_exp(_tmp2D0,0),0,(
void*)1,0);if(Cyc_Evexp_lte_const_exp(_tmp2D7,_tmp2D6))return 1;goto _LL15C;}
_LL15F:;_LL160: goto _LL15C;_LL15C:;}goto _LL157;_LL15A:;_LL15B: goto _LL157;_LL157:;}{
struct Cyc_List_List*_tmp2D8=relns;for(0;_tmp2D8 != 0;_tmp2D8=_tmp2D8->tl){struct
Cyc_CfFlowInfo_Reln*_tmp2D9=(struct Cyc_CfFlowInfo_Reln*)_tmp2D8->hd;if(_tmp2D9->vd
== _tmp2CA){union Cyc_CfFlowInfo_RelnOp_union _tmp2DA=_tmp2D9->rop;struct Cyc_Absyn_Vardecl*
_tmp2DB;struct Cyc_Absyn_Vardecl*_tmp2DC;unsigned int _tmp2DD;struct Cyc_Absyn_Vardecl*
_tmp2DE;_LL162: if((_tmp2DA.LessEqNumelts).tag != 4)goto _LL164;_tmp2DB=(_tmp2DA.LessEqNumelts).f1;
_LL163: _tmp2DC=_tmp2DB;goto _LL165;_LL164: if((_tmp2DA.LessNumelts).tag != 2)goto
_LL166;_tmp2DC=(_tmp2DA.LessNumelts).f1;_LL165: if(_tmp2AA == _tmp2DC)return 1;goto
_LL161;_LL166: if((_tmp2DA.EqualConst).tag != 0)goto _LL168;_tmp2DD=(_tmp2DA.EqualConst).f1;
_LL167: return Cyc_Toc_check_const_array(_tmp2DD,(void*)_tmp2AA->type);_LL168: if((
_tmp2DA.LessVar).tag != 1)goto _LL16A;_tmp2DE=(_tmp2DA.LessVar).f1;_LL169: if(Cyc_Toc_check_leq_size_var(
relns,_tmp2AA,_tmp2DE))return 1;goto _LL161;_LL16A:;_LL16B: goto _LL161;_LL161:;}}}
goto _LL14E;_LL153: if((_tmp2C8.LessConst).tag != 3)goto _LL155;_tmp2CC=(_tmp2C8.LessConst).f1;
_LL154: return Cyc_Toc_check_const_array(_tmp2CC,(void*)_tmp2AA->type);_LL155:;
_LL156: goto _LL14E;_LL14E:;}}}goto _LL139;_LL14C:;_LL14D: goto _LL139;_LL139:;}goto
_LL12E;_LL137:;_LL138: goto _LL12E;_LL12E:;}return 0;}static void*Cyc_Toc_get_c_typ(
struct Cyc_Absyn_Exp*e){if(e->topt == 0)({void*_tmp2DF=0;((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp2E0="Missing type in primop ";
_tag_dyneither(_tmp2E0,sizeof(char),24);}),_tag_dyneither(_tmp2DF,sizeof(void*),
0));});return Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);}
static void*Cyc_Toc_get_cyc_typ(struct Cyc_Absyn_Exp*e){if(e->topt == 0)({void*
_tmp2E1=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp2E2="Missing type in primop ";_tag_dyneither(_tmp2E2,sizeof(char),
24);}),_tag_dyneither(_tmp2E1,sizeof(void*),0));});return(void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v;}static struct _tuple4*Cyc_Toc_tup_to_c(struct Cyc_Absyn_Exp*
e){return({struct _tuple4*_tmp2E3=_cycalloc(sizeof(*_tmp2E3));_tmp2E3->f1=Cyc_Toc_mt_tq;
_tmp2E3->f2=Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
_tmp2E3;});}static struct _tuple8*Cyc_Toc_add_designator(struct Cyc_Toc_Env*nv,
struct Cyc_Absyn_Exp*e){Cyc_Toc_exp_to_c(nv,e);return({struct _tuple8*_tmp2E4=
_cycalloc(sizeof(*_tmp2E4));_tmp2E4->f1=0;_tmp2E4->f2=e;_tmp2E4;});}static struct
Cyc_Absyn_Exp*Cyc_Toc_make_struct(struct Cyc_Toc_Env*nv,struct _tuple1*x,void*
struct_typ,struct Cyc_Absyn_Stmt*s,int pointer,struct Cyc_Absyn_Exp*rgnopt,int
is_atomic){struct Cyc_Absyn_Exp*eo;void*t;if(pointer){t=Cyc_Absyn_cstar_typ(
struct_typ,Cyc_Toc_mt_tq);{struct Cyc_Absyn_Exp*_tmp2E5=Cyc_Absyn_sizeofexp_exp(
Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(x,0),0),0);if(rgnopt == 0  || Cyc_Absyn_no_regions)
eo=(struct Cyc_Absyn_Exp*)(is_atomic?Cyc_Toc_malloc_atomic(_tmp2E5): Cyc_Toc_malloc_ptr(
_tmp2E5));else{struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)rgnopt;Cyc_Toc_exp_to_c(
nv,r);eo=(struct Cyc_Absyn_Exp*)Cyc_Toc_rmalloc_exp(r,_tmp2E5);}}}else{t=
struct_typ;eo=0;}return Cyc_Absyn_stmt_exp(Cyc_Absyn_declare_stmt(x,t,eo,s,0),0);}
static struct Cyc_Absyn_Stmt*Cyc_Toc_init_comprehension(struct Cyc_Toc_Env*nv,
struct Cyc_Absyn_Exp*lhs,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*e1,struct
Cyc_Absyn_Exp*e2,int zero_term,struct Cyc_Absyn_Stmt*s,int e1_already_translated);
static struct Cyc_Absyn_Stmt*Cyc_Toc_init_anon_struct(struct Cyc_Toc_Env*nv,struct
Cyc_Absyn_Exp*lhs,void*struct_type,struct Cyc_List_List*dles,struct Cyc_Absyn_Stmt*
s);static struct Cyc_Absyn_Stmt*Cyc_Toc_init_array(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*
lhs,struct Cyc_List_List*dles0,struct Cyc_Absyn_Stmt*s){int count=((int(*)(struct
Cyc_List_List*x))Cyc_List_length)(dles0)- 1;{struct Cyc_List_List*_tmp2E6=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(dles0);for(0;
_tmp2E6 != 0;_tmp2E6=_tmp2E6->tl){struct _tuple8 _tmp2E8;struct Cyc_List_List*
_tmp2E9;struct Cyc_Absyn_Exp*_tmp2EA;struct _tuple8*_tmp2E7=(struct _tuple8*)
_tmp2E6->hd;_tmp2E8=*_tmp2E7;_tmp2E9=_tmp2E8.f1;_tmp2EA=_tmp2E8.f2;{struct Cyc_Absyn_Exp*
e_index;if(_tmp2E9 == 0)e_index=Cyc_Absyn_signed_int_exp(count --,0);else{if(
_tmp2E9->tl != 0)({void*_tmp2EB=0;((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_unimp)(({const char*_tmp2EC="multiple designators in array";
_tag_dyneither(_tmp2EC,sizeof(char),30);}),_tag_dyneither(_tmp2EB,sizeof(void*),
0));});{void*_tmp2ED=(void*)_tmp2E9->hd;void*_tmp2EE=_tmp2ED;struct Cyc_Absyn_Exp*
_tmp2EF;_LL16D: if(*((int*)_tmp2EE)!= 0)goto _LL16F;_tmp2EF=((struct Cyc_Absyn_ArrayElement_struct*)
_tmp2EE)->f1;_LL16E: Cyc_Toc_exp_to_c(nv,_tmp2EF);e_index=_tmp2EF;goto _LL16C;
_LL16F: if(*((int*)_tmp2EE)!= 1)goto _LL16C;_LL170:({void*_tmp2F0=0;((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(({const char*
_tmp2F1="field name designators in array";_tag_dyneither(_tmp2F1,sizeof(char),32);}),
_tag_dyneither(_tmp2F0,sizeof(void*),0));});_LL16C:;}}{struct Cyc_Absyn_Exp*lval=
Cyc_Absyn_subscript_exp(lhs,e_index,0);void*_tmp2F2=(void*)_tmp2EA->r;struct Cyc_List_List*
_tmp2F3;struct Cyc_Absyn_Vardecl*_tmp2F4;struct Cyc_Absyn_Exp*_tmp2F5;struct Cyc_Absyn_Exp*
_tmp2F6;int _tmp2F7;void*_tmp2F8;struct Cyc_List_List*_tmp2F9;_LL172: if(*((int*)
_tmp2F2)!= 28)goto _LL174;_tmp2F3=((struct Cyc_Absyn_Array_e_struct*)_tmp2F2)->f1;
_LL173: s=Cyc_Toc_init_array(nv,lval,_tmp2F3,s);goto _LL171;_LL174: if(*((int*)
_tmp2F2)!= 29)goto _LL176;_tmp2F4=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp2F2)->f1;_tmp2F5=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp2F2)->f2;
_tmp2F6=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp2F2)->f3;_tmp2F7=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmp2F2)->f4;_LL175: s=Cyc_Toc_init_comprehension(
nv,lval,_tmp2F4,_tmp2F5,_tmp2F6,_tmp2F7,s,0);goto _LL171;_LL176: if(*((int*)
_tmp2F2)!= 31)goto _LL178;_tmp2F8=(void*)((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp2F2)->f1;_tmp2F9=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp2F2)->f2;_LL177:
s=Cyc_Toc_init_anon_struct(nv,lval,_tmp2F8,_tmp2F9,s);goto _LL171;_LL178:;_LL179:
Cyc_Toc_exp_to_c(nv,_tmp2EA);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(
lhs,e_index,0),_tmp2EA,0),s,0);goto _LL171;_LL171:;}}}}return s;}static struct Cyc_Absyn_Stmt*
Cyc_Toc_init_comprehension(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,struct
Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int zero_term,
struct Cyc_Absyn_Stmt*s,int e1_already_translated){struct _tuple1*_tmp2FA=vd->name;
void*_tmp2FB=Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v);
if(!e1_already_translated)Cyc_Toc_exp_to_c(nv,e1);{struct Cyc_Toc_Env _tmp2FD;
struct _RegionHandle*_tmp2FE;struct Cyc_Toc_Env*_tmp2FC=nv;_tmp2FD=*_tmp2FC;
_tmp2FE=_tmp2FD.rgn;{struct Cyc_Toc_Env*nv2=Cyc_Toc_add_varmap(_tmp2FE,nv,_tmp2FA,
Cyc_Absyn_varb_exp(_tmp2FA,(void*)({struct Cyc_Absyn_Local_b_struct*_tmp307=
_cycalloc(sizeof(*_tmp307));_tmp307[0]=({struct Cyc_Absyn_Local_b_struct _tmp308;
_tmp308.tag=3;_tmp308.f1=vd;_tmp308;});_tmp307;}),0));struct _tuple1*max=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*ea=Cyc_Absyn_assign_exp(Cyc_Absyn_var_exp(_tmp2FA,0),Cyc_Absyn_signed_int_exp(
0,0),0);struct Cyc_Absyn_Exp*eb=Cyc_Absyn_lt_exp(Cyc_Absyn_var_exp(_tmp2FA,0),Cyc_Absyn_var_exp(
max,0),0);struct Cyc_Absyn_Exp*ec=Cyc_Absyn_post_inc_exp(Cyc_Absyn_var_exp(
_tmp2FA,0),0);struct Cyc_Absyn_Exp*lval=Cyc_Absyn_subscript_exp(lhs,Cyc_Absyn_var_exp(
_tmp2FA,0),0);struct Cyc_Absyn_Stmt*body;{void*_tmp2FF=(void*)e2->r;struct Cyc_List_List*
_tmp300;struct Cyc_Absyn_Vardecl*_tmp301;struct Cyc_Absyn_Exp*_tmp302;struct Cyc_Absyn_Exp*
_tmp303;int _tmp304;void*_tmp305;struct Cyc_List_List*_tmp306;_LL17B: if(*((int*)
_tmp2FF)!= 28)goto _LL17D;_tmp300=((struct Cyc_Absyn_Array_e_struct*)_tmp2FF)->f1;
_LL17C: body=Cyc_Toc_init_array(nv2,lval,_tmp300,Cyc_Toc_skip_stmt_dl());goto
_LL17A;_LL17D: if(*((int*)_tmp2FF)!= 29)goto _LL17F;_tmp301=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp2FF)->f1;_tmp302=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp2FF)->f2;
_tmp303=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp2FF)->f3;_tmp304=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmp2FF)->f4;_LL17E: body=Cyc_Toc_init_comprehension(
nv2,lval,_tmp301,_tmp302,_tmp303,_tmp304,Cyc_Toc_skip_stmt_dl(),0);goto _LL17A;
_LL17F: if(*((int*)_tmp2FF)!= 31)goto _LL181;_tmp305=(void*)((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp2FF)->f1;_tmp306=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp2FF)->f2;_LL180:
body=Cyc_Toc_init_anon_struct(nv,lval,_tmp305,_tmp306,Cyc_Toc_skip_stmt_dl());
goto _LL17A;_LL181:;_LL182: Cyc_Toc_exp_to_c(nv2,e2);body=Cyc_Absyn_assign_stmt(
lval,e2,0);goto _LL17A;_LL17A:;}{struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_for_stmt(ea,eb,
ec,body,0);if(zero_term){struct Cyc_Absyn_Exp*ex=Cyc_Absyn_assign_exp(Cyc_Absyn_subscript_exp(
Cyc_Absyn_new_exp((void*)lhs->r,0),Cyc_Absyn_var_exp(max,0),0),Cyc_Toc_cast_it(
_tmp2FB,Cyc_Absyn_uint_exp(0,0)),0);s2=Cyc_Absyn_seq_stmt(s2,Cyc_Absyn_exp_stmt(
ex,0),0);}return Cyc_Absyn_seq_stmt(Cyc_Absyn_declare_stmt(max,Cyc_Absyn_uint_typ,(
struct Cyc_Absyn_Exp*)e1,Cyc_Absyn_declare_stmt(_tmp2FA,Cyc_Absyn_uint_typ,0,s2,0),
0),s,0);}}}}static struct Cyc_Absyn_Stmt*Cyc_Toc_init_anon_struct(struct Cyc_Toc_Env*
nv,struct Cyc_Absyn_Exp*lhs,void*struct_type,struct Cyc_List_List*dles,struct Cyc_Absyn_Stmt*
s){{struct Cyc_List_List*_tmp309=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_rev)(dles);for(0;_tmp309 != 0;_tmp309=_tmp309->tl){struct _tuple8 _tmp30B;
struct Cyc_List_List*_tmp30C;struct Cyc_Absyn_Exp*_tmp30D;struct _tuple8*_tmp30A=(
struct _tuple8*)_tmp309->hd;_tmp30B=*_tmp30A;_tmp30C=_tmp30B.f1;_tmp30D=_tmp30B.f2;
if(_tmp30C == 0)({void*_tmp30E=0;((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp30F="empty designator list";
_tag_dyneither(_tmp30F,sizeof(char),22);}),_tag_dyneither(_tmp30E,sizeof(void*),
0));});if(_tmp30C->tl != 0)({void*_tmp310=0;((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp311="too many designators in anonymous struct";
_tag_dyneither(_tmp311,sizeof(char),41);}),_tag_dyneither(_tmp310,sizeof(void*),
0));});{void*_tmp312=(void*)_tmp30C->hd;struct _dyneither_ptr*_tmp313;_LL184: if(*((
int*)_tmp312)!= 1)goto _LL186;_tmp313=((struct Cyc_Absyn_FieldName_struct*)_tmp312)->f1;
_LL185: {struct Cyc_Absyn_Exp*lval=Cyc_Absyn_aggrmember_exp(lhs,_tmp313,0);{void*
_tmp314=(void*)_tmp30D->r;struct Cyc_List_List*_tmp315;struct Cyc_Absyn_Vardecl*
_tmp316;struct Cyc_Absyn_Exp*_tmp317;struct Cyc_Absyn_Exp*_tmp318;int _tmp319;void*
_tmp31A;struct Cyc_List_List*_tmp31B;_LL189: if(*((int*)_tmp314)!= 28)goto _LL18B;
_tmp315=((struct Cyc_Absyn_Array_e_struct*)_tmp314)->f1;_LL18A: s=Cyc_Toc_init_array(
nv,lval,_tmp315,s);goto _LL188;_LL18B: if(*((int*)_tmp314)!= 29)goto _LL18D;_tmp316=((
struct Cyc_Absyn_Comprehension_e_struct*)_tmp314)->f1;_tmp317=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp314)->f2;_tmp318=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp314)->f3;
_tmp319=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp314)->f4;_LL18C: s=Cyc_Toc_init_comprehension(
nv,lval,_tmp316,_tmp317,_tmp318,_tmp319,s,0);goto _LL188;_LL18D: if(*((int*)
_tmp314)!= 31)goto _LL18F;_tmp31A=(void*)((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp314)->f1;_tmp31B=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp314)->f2;_LL18E:
s=Cyc_Toc_init_anon_struct(nv,lval,_tmp31A,_tmp31B,s);goto _LL188;_LL18F:;_LL190:
Cyc_Toc_exp_to_c(nv,_tmp30D);if(Cyc_Toc_is_poly_field(struct_type,_tmp313))
_tmp30D=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp30D);s=Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(lval,_tmp30D,0),0),s,0);goto _LL188;
_LL188:;}goto _LL183;}_LL186:;_LL187:({void*_tmp31C=0;((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp31D="array designator in struct";
_tag_dyneither(_tmp31D,sizeof(char),27);}),_tag_dyneither(_tmp31C,sizeof(void*),
0));});_LL183:;}}}return s;}static struct Cyc_Absyn_Exp*Cyc_Toc_init_tuple(struct
Cyc_Toc_Env*nv,int pointer,struct Cyc_Absyn_Exp*rgnopt,struct Cyc_List_List*es){
struct Cyc_Toc_Env _tmp31F;struct _RegionHandle*_tmp320;struct Cyc_Toc_Env*_tmp31E=
nv;_tmp31F=*_tmp31E;_tmp320=_tmp31F.rgn;{struct Cyc_List_List*_tmp321=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _tuple4*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*
x))Cyc_List_rmap)(_tmp320,Cyc_Toc_tup_to_c,es);void*_tmp322=Cyc_Toc_add_tuple_type(
_tmp321);struct _tuple1*_tmp323=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp324=
Cyc_Absyn_var_exp(_tmp323,0);struct Cyc_Absyn_Stmt*_tmp325=Cyc_Absyn_exp_stmt(
_tmp324,0);struct Cyc_Absyn_Exp*(*_tmp326)(struct Cyc_Absyn_Exp*,struct
_dyneither_ptr*,struct Cyc_Position_Segment*)=pointer?Cyc_Absyn_aggrarrow_exp: Cyc_Absyn_aggrmember_exp;
int is_atomic=1;struct Cyc_List_List*_tmp327=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(_tmp320,es);{int i=((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmp327);for(0;_tmp327 != 0;(_tmp327=
_tmp327->tl,-- i)){struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)_tmp327->hd;struct
Cyc_Absyn_Exp*lval=_tmp326(_tmp324,Cyc_Absyn_fieldname(i),0);is_atomic=is_atomic
 && Cyc_Toc_atomic_typ((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);{
void*_tmp328=(void*)e->r;struct Cyc_List_List*_tmp329;struct Cyc_Absyn_Vardecl*
_tmp32A;struct Cyc_Absyn_Exp*_tmp32B;struct Cyc_Absyn_Exp*_tmp32C;int _tmp32D;
_LL192: if(*((int*)_tmp328)!= 28)goto _LL194;_tmp329=((struct Cyc_Absyn_Array_e_struct*)
_tmp328)->f1;_LL193: _tmp325=Cyc_Toc_init_array(nv,lval,_tmp329,_tmp325);goto
_LL191;_LL194: if(*((int*)_tmp328)!= 29)goto _LL196;_tmp32A=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp328)->f1;_tmp32B=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp328)->f2;
_tmp32C=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp328)->f3;_tmp32D=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmp328)->f4;_LL195: _tmp325=Cyc_Toc_init_comprehension(
nv,lval,_tmp32A,_tmp32B,_tmp32C,_tmp32D,_tmp325,0);goto _LL191;_LL196:;_LL197: Cyc_Toc_exp_to_c(
nv,e);_tmp325=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp326(
_tmp324,Cyc_Absyn_fieldname(i),0),e,0),0),_tmp325,0);goto _LL191;_LL191:;}}}
return Cyc_Toc_make_struct(nv,_tmp323,_tmp322,_tmp325,pointer,rgnopt,is_atomic);}}
static struct Cyc_Absyn_Exp*Cyc_Toc_init_struct(struct Cyc_Toc_Env*nv,void*
struct_type,int has_exists,int pointer,struct Cyc_Absyn_Exp*rgnopt,struct Cyc_List_List*
dles,struct _tuple1*tdn){struct _tuple1*_tmp32E=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*
_tmp32F=Cyc_Absyn_var_exp(_tmp32E,0);struct Cyc_Absyn_Stmt*_tmp330=Cyc_Absyn_exp_stmt(
_tmp32F,0);struct Cyc_Absyn_Exp*(*_tmp331)(struct Cyc_Absyn_Exp*,struct
_dyneither_ptr*,struct Cyc_Position_Segment*)=pointer?Cyc_Absyn_aggrarrow_exp: Cyc_Absyn_aggrmember_exp;
void*_tmp332=Cyc_Toc_aggrdecl_type(tdn,Cyc_Absyn_strctq);int is_atomic=1;struct
Cyc_Absyn_Aggrdecl*ad;{void*_tmp333=Cyc_Tcutil_compress(struct_type);struct Cyc_Absyn_AggrInfo
_tmp334;union Cyc_Absyn_AggrInfoU_union _tmp335;_LL199: if(_tmp333 <= (void*)4)goto
_LL19B;if(*((int*)_tmp333)!= 10)goto _LL19B;_tmp334=((struct Cyc_Absyn_AggrType_struct*)
_tmp333)->f1;_tmp335=_tmp334.aggr_info;_LL19A: ad=Cyc_Absyn_get_known_aggrdecl(
_tmp335);goto _LL198;_LL19B:;_LL19C:({void*_tmp336=0;((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp337="init_struct: bad struct type";
_tag_dyneither(_tmp337,sizeof(char),29);}),_tag_dyneither(_tmp336,sizeof(void*),
0));});_LL198:;}{struct Cyc_Toc_Env _tmp339;struct _RegionHandle*_tmp33A;struct Cyc_Toc_Env*
_tmp338=nv;_tmp339=*_tmp338;_tmp33A=_tmp339.rgn;{struct Cyc_List_List*_tmp33B=((
struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(
_tmp33A,dles);for(0;_tmp33B != 0;_tmp33B=_tmp33B->tl){struct _tuple8 _tmp33D;struct
Cyc_List_List*_tmp33E;struct Cyc_Absyn_Exp*_tmp33F;struct _tuple8*_tmp33C=(struct
_tuple8*)_tmp33B->hd;_tmp33D=*_tmp33C;_tmp33E=_tmp33D.f1;_tmp33F=_tmp33D.f2;
is_atomic=is_atomic  && Cyc_Toc_atomic_typ((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp33F->topt))->v);if(_tmp33E == 0)({void*_tmp340=0;((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp341="empty designator list";
_tag_dyneither(_tmp341,sizeof(char),22);}),_tag_dyneither(_tmp340,sizeof(void*),
0));});if(_tmp33E->tl != 0){struct _tuple1*_tmp342=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*
_tmp343=Cyc_Absyn_var_exp(_tmp342,0);for(0;_tmp33E != 0;_tmp33E=_tmp33E->tl){void*
_tmp344=(void*)_tmp33E->hd;struct _dyneither_ptr*_tmp345;_LL19E: if(*((int*)
_tmp344)!= 1)goto _LL1A0;_tmp345=((struct Cyc_Absyn_FieldName_struct*)_tmp344)->f1;
_LL19F: if(Cyc_Toc_is_poly_field(struct_type,_tmp345))_tmp343=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),
_tmp343);_tmp330=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(
_tmp331(_tmp32F,_tmp345,0),_tmp343,0),0),_tmp330,0);goto _LL19D;_LL1A0:;_LL1A1:({
void*_tmp346=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp347="array designator in struct";_tag_dyneither(_tmp347,sizeof(
char),27);}),_tag_dyneither(_tmp346,sizeof(void*),0));});_LL19D:;}Cyc_Toc_exp_to_c(
nv,_tmp33F);_tmp330=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(
_tmp343,_tmp33F,0),0),_tmp330,0);}else{void*_tmp348=(void*)_tmp33E->hd;struct
_dyneither_ptr*_tmp349;_LL1A3: if(*((int*)_tmp348)!= 1)goto _LL1A5;_tmp349=((
struct Cyc_Absyn_FieldName_struct*)_tmp348)->f1;_LL1A4: {struct Cyc_Absyn_Exp*lval=
_tmp331(_tmp32F,_tmp349,0);{void*_tmp34A=(void*)_tmp33F->r;struct Cyc_List_List*
_tmp34B;struct Cyc_Absyn_Vardecl*_tmp34C;struct Cyc_Absyn_Exp*_tmp34D;struct Cyc_Absyn_Exp*
_tmp34E;int _tmp34F;void*_tmp350;struct Cyc_List_List*_tmp351;_LL1A8: if(*((int*)
_tmp34A)!= 28)goto _LL1AA;_tmp34B=((struct Cyc_Absyn_Array_e_struct*)_tmp34A)->f1;
_LL1A9: _tmp330=Cyc_Toc_init_array(nv,lval,_tmp34B,_tmp330);goto _LL1A7;_LL1AA: if(*((
int*)_tmp34A)!= 29)goto _LL1AC;_tmp34C=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp34A)->f1;_tmp34D=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp34A)->f2;
_tmp34E=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp34A)->f3;_tmp34F=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmp34A)->f4;_LL1AB: _tmp330=Cyc_Toc_init_comprehension(
nv,lval,_tmp34C,_tmp34D,_tmp34E,_tmp34F,_tmp330,0);goto _LL1A7;_LL1AC: if(*((int*)
_tmp34A)!= 31)goto _LL1AE;_tmp350=(void*)((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp34A)->f1;_tmp351=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp34A)->f2;_LL1AD:
_tmp330=Cyc_Toc_init_anon_struct(nv,lval,_tmp350,_tmp351,_tmp330);goto _LL1A7;
_LL1AE:;_LL1AF: {int was_ptr_type=Cyc_Tcutil_is_pointer_type((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp33F->topt))->v);Cyc_Toc_exp_to_c(nv,_tmp33F);{struct Cyc_Absyn_Aggrfield*
_tmp352=Cyc_Absyn_lookup_decl_field(ad,_tmp349);if(Cyc_Toc_is_poly_field(
struct_type,_tmp349) && !was_ptr_type)_tmp33F=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),
_tmp33F);if(has_exists)_tmp33F=Cyc_Toc_cast_it((void*)((struct Cyc_Absyn_Aggrfield*)
_check_null(_tmp352))->type,_tmp33F);_tmp330=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(
Cyc_Absyn_assign_exp(lval,_tmp33F,0),0),_tmp330,0);goto _LL1A7;}}_LL1A7:;}goto
_LL1A2;}_LL1A5:;_LL1A6:({void*_tmp353=0;((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp354="array designator in struct";
_tag_dyneither(_tmp354,sizeof(char),27);}),_tag_dyneither(_tmp353,sizeof(void*),
0));});_LL1A2:;}}}return Cyc_Toc_make_struct(nv,_tmp32E,_tmp332,_tmp330,pointer,
rgnopt,is_atomic);}}struct _tuple10{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Exp*f2;
};static struct Cyc_Absyn_Exp*Cyc_Toc_assignop_lvalue(struct Cyc_Absyn_Exp*el,
struct _tuple10*pr){return Cyc_Absyn_assignop_exp(el,(*pr).f1,(*pr).f2,0);}static
struct Cyc_Absyn_Exp*Cyc_Toc_address_lvalue(struct Cyc_Absyn_Exp*e1,int ignore){
return Cyc_Absyn_address_exp(e1,0);}static struct Cyc_Absyn_Exp*Cyc_Toc_incr_lvalue(
struct Cyc_Absyn_Exp*e1,void*incr){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Increment_e_struct*
_tmp355=_cycalloc(sizeof(*_tmp355));_tmp355[0]=({struct Cyc_Absyn_Increment_e_struct
_tmp356;_tmp356.tag=5;_tmp356.f1=e1;_tmp356.f2=(void*)incr;_tmp356;});_tmp355;}),
0);}static void Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*
fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env);static void
Cyc_Toc_lvalue_assign(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*
f)(struct Cyc_Absyn_Exp*,void*),void*f_env){void*_tmp357=(void*)e1->r;struct Cyc_Absyn_Stmt*
_tmp358;void*_tmp359;struct Cyc_Absyn_Exp*_tmp35A;struct Cyc_Absyn_Exp*_tmp35B;
struct _dyneither_ptr*_tmp35C;_LL1B1: if(*((int*)_tmp357)!= 38)goto _LL1B3;_tmp358=((
struct Cyc_Absyn_StmtExp_e_struct*)_tmp357)->f1;_LL1B2: Cyc_Toc_lvalue_assign_stmt(
_tmp358,fs,f,f_env);goto _LL1B0;_LL1B3: if(*((int*)_tmp357)!= 15)goto _LL1B5;
_tmp359=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp357)->f1;_tmp35A=((struct Cyc_Absyn_Cast_e_struct*)
_tmp357)->f2;_LL1B4: Cyc_Toc_lvalue_assign(_tmp35A,fs,f,f_env);goto _LL1B0;_LL1B5:
if(*((int*)_tmp357)!= 23)goto _LL1B7;_tmp35B=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp357)->f1;_tmp35C=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp357)->f2;_LL1B6:(
void*)(e1->r=(void*)((void*)_tmp35B->r));Cyc_Toc_lvalue_assign(e1,(struct Cyc_List_List*)({
struct Cyc_List_List*_tmp35D=_cycalloc(sizeof(*_tmp35D));_tmp35D->hd=_tmp35C;
_tmp35D->tl=fs;_tmp35D;}),f,f_env);goto _LL1B0;_LL1B7:;_LL1B8: {struct Cyc_Absyn_Exp*
e1_copy=Cyc_Absyn_copy_exp(e1);for(0;fs != 0;fs=fs->tl){e1_copy=Cyc_Absyn_aggrmember_exp(
e1_copy,(struct _dyneither_ptr*)fs->hd,e1_copy->loc);}(void*)(e1->r=(void*)((void*)(
f(e1_copy,f_env))->r));goto _LL1B0;}_LL1B0:;}static void Cyc_Toc_lvalue_assign_stmt(
struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,
void*),void*f_env){void*_tmp35E=(void*)s->r;struct Cyc_Absyn_Exp*_tmp35F;struct
Cyc_Absyn_Decl*_tmp360;struct Cyc_Absyn_Stmt*_tmp361;struct Cyc_Absyn_Stmt*_tmp362;
_LL1BA: if(_tmp35E <= (void*)1)goto _LL1C0;if(*((int*)_tmp35E)!= 0)goto _LL1BC;
_tmp35F=((struct Cyc_Absyn_Exp_s_struct*)_tmp35E)->f1;_LL1BB: Cyc_Toc_lvalue_assign(
_tmp35F,fs,f,f_env);goto _LL1B9;_LL1BC: if(*((int*)_tmp35E)!= 11)goto _LL1BE;
_tmp360=((struct Cyc_Absyn_Decl_s_struct*)_tmp35E)->f1;_tmp361=((struct Cyc_Absyn_Decl_s_struct*)
_tmp35E)->f2;_LL1BD: Cyc_Toc_lvalue_assign_stmt(_tmp361,fs,f,f_env);goto _LL1B9;
_LL1BE: if(*((int*)_tmp35E)!= 1)goto _LL1C0;_tmp362=((struct Cyc_Absyn_Seq_s_struct*)
_tmp35E)->f2;_LL1BF: Cyc_Toc_lvalue_assign_stmt(_tmp362,fs,f,f_env);goto _LL1B9;
_LL1C0:;_LL1C1:({struct Cyc_String_pa_struct _tmp365;_tmp365.tag=0;_tmp365.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_stmt2string(s));{void*
_tmp363[1]={& _tmp365};Cyc_Toc_toc_impos(({const char*_tmp364="lvalue_assign_stmt: %s";
_tag_dyneither(_tmp364,sizeof(char),23);}),_tag_dyneither(_tmp363,sizeof(void*),
1));}});_LL1B9:;}static struct Cyc_List_List*Cyc_Toc_rmap_2c(struct _RegionHandle*
r2,void*(*f)(void*,void*),void*env,struct Cyc_List_List*x){struct Cyc_List_List*
result;struct Cyc_List_List*prev;if(x == 0)return 0;result=({struct Cyc_List_List*
_tmp366=_region_malloc(r2,sizeof(*_tmp366));_tmp366->hd=(void*)f((void*)x->hd,
env);_tmp366->tl=0;_tmp366;});prev=result;for(x=x->tl;x != 0;x=x->tl){((struct Cyc_List_List*)
_check_null(prev))->tl=({struct Cyc_List_List*_tmp367=_region_malloc(r2,sizeof(*
_tmp367));_tmp367->hd=(void*)f((void*)x->hd,env);_tmp367->tl=0;_tmp367;});prev=((
struct Cyc_List_List*)_check_null(prev))->tl;}return result;}static struct Cyc_List_List*
Cyc_Toc_map_2c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*x){return Cyc_Toc_rmap_2c(
Cyc_Core_heap_region,f,env,x);}static struct _tuple8*Cyc_Toc_make_dle(struct Cyc_Absyn_Exp*
e){return({struct _tuple8*_tmp368=_cycalloc(sizeof(*_tmp368));_tmp368->f1=0;
_tmp368->f2=e;_tmp368;});}static struct Cyc_Absyn_PtrInfo Cyc_Toc_get_ptr_type(void*
t){void*_tmp369=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp36A;_LL1C3: if(
_tmp369 <= (void*)4)goto _LL1C5;if(*((int*)_tmp369)!= 4)goto _LL1C5;_tmp36A=((
struct Cyc_Absyn_PointerType_struct*)_tmp369)->f1;_LL1C4: return _tmp36A;_LL1C5:;
_LL1C6:({void*_tmp36B=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Toc_toc_impos)(({const char*_tmp36C="get_ptr_typ: not a pointer!";
_tag_dyneither(_tmp36C,sizeof(char),28);}),_tag_dyneither(_tmp36B,sizeof(void*),
0));});_LL1C2:;}static struct Cyc_Absyn_Exp*Cyc_Toc_generate_zero(void*t){struct
Cyc_Absyn_Exp*res;{void*_tmp36D=Cyc_Tcutil_compress(t);void*_tmp36E;void*_tmp36F;
void*_tmp370;void*_tmp371;void*_tmp372;void*_tmp373;void*_tmp374;void*_tmp375;
void*_tmp376;void*_tmp377;_LL1C8: if(_tmp36D <= (void*)4)goto _LL1D8;if(*((int*)
_tmp36D)!= 4)goto _LL1CA;_LL1C9: res=Cyc_Absyn_null_exp(0);goto _LL1C7;_LL1CA: if(*((
int*)_tmp36D)!= 5)goto _LL1CC;_tmp36E=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp36D)->f1;_tmp36F=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp36D)->f2;if((
int)_tmp36F != 0)goto _LL1CC;_LL1CB: res=Cyc_Absyn_const_exp((union Cyc_Absyn_Cnst_union)({
union Cyc_Absyn_Cnst_union _tmp378;(_tmp378.Char_c).tag=0;(_tmp378.Char_c).f1=(
void*)_tmp36E;(_tmp378.Char_c).f2='\000';_tmp378;}),0);goto _LL1C7;_LL1CC: if(*((
int*)_tmp36D)!= 5)goto _LL1CE;_tmp370=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp36D)->f1;_tmp371=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp36D)->f2;if((
int)_tmp371 != 1)goto _LL1CE;_LL1CD: res=Cyc_Absyn_const_exp((union Cyc_Absyn_Cnst_union)({
union Cyc_Absyn_Cnst_union _tmp379;(_tmp379.Short_c).tag=1;(_tmp379.Short_c).f1=(
void*)_tmp370;(_tmp379.Short_c).f2=0;_tmp379;}),0);goto _LL1C7;_LL1CE: if(*((int*)
_tmp36D)!= 12)goto _LL1D0;_LL1CF: goto _LL1D1;_LL1D0: if(*((int*)_tmp36D)!= 13)goto
_LL1D2;_LL1D1: _tmp372=(void*)1;goto _LL1D3;_LL1D2: if(*((int*)_tmp36D)!= 5)goto
_LL1D4;_tmp372=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp36D)->f1;_tmp373=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp36D)->f2;if((int)_tmp373 != 2)goto
_LL1D4;_LL1D3: _tmp374=_tmp372;goto _LL1D5;_LL1D4: if(*((int*)_tmp36D)!= 5)goto
_LL1D6;_tmp374=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp36D)->f1;_tmp375=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp36D)->f2;if((int)_tmp375 != 3)goto
_LL1D6;_LL1D5: res=Cyc_Absyn_const_exp((union Cyc_Absyn_Cnst_union)({union Cyc_Absyn_Cnst_union
_tmp37A;(_tmp37A.Int_c).tag=2;(_tmp37A.Int_c).f1=(void*)_tmp374;(_tmp37A.Int_c).f2=
0;_tmp37A;}),0);goto _LL1C7;_LL1D6: if(*((int*)_tmp36D)!= 5)goto _LL1D8;_tmp376=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp36D)->f1;_tmp377=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp36D)->f2;if((int)_tmp377 != 4)goto _LL1D8;_LL1D7: res=Cyc_Absyn_const_exp((
union Cyc_Absyn_Cnst_union)({union Cyc_Absyn_Cnst_union _tmp37B;(_tmp37B.LongLong_c).tag=
3;(_tmp37B.LongLong_c).f1=(void*)_tmp376;(_tmp37B.LongLong_c).f2=(long long)0;
_tmp37B;}),0);goto _LL1C7;_LL1D8: if((int)_tmp36D != 1)goto _LL1DA;_LL1D9: goto _LL1DB;
_LL1DA: if(_tmp36D <= (void*)4)goto _LL1DC;if(*((int*)_tmp36D)!= 6)goto _LL1DC;
_LL1DB: res=Cyc_Absyn_const_exp((union Cyc_Absyn_Cnst_union)({union Cyc_Absyn_Cnst_union
_tmp37C;(_tmp37C.Float_c).tag=4;(_tmp37C.Float_c).f1=({const char*_tmp37D="0.0";
_tag_dyneither(_tmp37D,sizeof(char),4);});_tmp37C;}),0);goto _LL1C7;_LL1DC:;
_LL1DD:({struct Cyc_String_pa_struct _tmp380;_tmp380.tag=0;_tmp380.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));{void*_tmp37E[1]={&
_tmp380};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp37F="found non-zero type %s in generate_zero";_tag_dyneither(
_tmp37F,sizeof(char),40);}),_tag_dyneither(_tmp37E,sizeof(void*),1));}});_LL1C7:;}
res->topt=({struct Cyc_Core_Opt*_tmp381=_cycalloc(sizeof(*_tmp381));_tmp381->v=(
void*)t;_tmp381;});return res;}static void Cyc_Toc_zero_ptr_assign_to_c(struct Cyc_Toc_Env*
nv,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*popt,struct
Cyc_Absyn_Exp*e2,void*ptr_type,int is_dyneither,void*elt_type){void*fat_ptr_type=
Cyc_Absyn_dyneither_typ(elt_type,(void*)2,Cyc_Toc_mt_tq,Cyc_Absyn_true_conref);
void*_tmp382=Cyc_Toc_typ_to_c(elt_type);void*_tmp383=Cyc_Toc_typ_to_c(
fat_ptr_type);void*_tmp384=Cyc_Absyn_cstar_typ(_tmp382,Cyc_Toc_mt_tq);struct Cyc_Core_Opt*
_tmp385=({struct Cyc_Core_Opt*_tmp3C4=_cycalloc(sizeof(*_tmp3C4));_tmp3C4->v=(
void*)_tmp384;_tmp3C4;});struct Cyc_Absyn_Exp*xinit;{void*_tmp386=(void*)e1->r;
struct Cyc_Absyn_Exp*_tmp387;struct Cyc_Absyn_Exp*_tmp388;struct Cyc_Absyn_Exp*
_tmp389;_LL1DF: if(*((int*)_tmp386)!= 22)goto _LL1E1;_tmp387=((struct Cyc_Absyn_Deref_e_struct*)
_tmp386)->f1;_LL1E0: if(!is_dyneither){_tmp387=Cyc_Toc_cast_it(fat_ptr_type,
_tmp387);_tmp387->topt=({struct Cyc_Core_Opt*_tmp38A=_cycalloc(sizeof(*_tmp38A));
_tmp38A->v=(void*)fat_ptr_type;_tmp38A;});}Cyc_Toc_exp_to_c(nv,_tmp387);xinit=
_tmp387;goto _LL1DE;_LL1E1: if(*((int*)_tmp386)!= 25)goto _LL1E3;_tmp388=((struct
Cyc_Absyn_Subscript_e_struct*)_tmp386)->f1;_tmp389=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp386)->f2;_LL1E2: if(!is_dyneither){_tmp388=Cyc_Toc_cast_it(fat_ptr_type,
_tmp388);_tmp388->topt=({struct Cyc_Core_Opt*_tmp38B=_cycalloc(sizeof(*_tmp38B));
_tmp38B->v=(void*)fat_ptr_type;_tmp38B;});}Cyc_Toc_exp_to_c(nv,_tmp388);Cyc_Toc_exp_to_c(
nv,_tmp389);xinit=Cyc_Absyn_fncall_exp(Cyc_Toc__dyneither_ptr_plus_e,({struct Cyc_Absyn_Exp*
_tmp38C[3];_tmp38C[2]=_tmp389;_tmp38C[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
elt_type),0);_tmp38C[0]=_tmp388;((struct Cyc_List_List*(*)(struct _dyneither_ptr))
Cyc_List_list)(_tag_dyneither(_tmp38C,sizeof(struct Cyc_Absyn_Exp*),3));}),0);
goto _LL1DE;_LL1E3:;_LL1E4:({void*_tmp38D=0;((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp38E="found bad lhs for zero-terminated pointer assignment";
_tag_dyneither(_tmp38E,sizeof(char),53);}),_tag_dyneither(_tmp38D,sizeof(void*),
0));});_LL1DE:;}{struct _tuple1*_tmp38F=Cyc_Toc_temp_var();struct Cyc_Toc_Env
_tmp391;struct _RegionHandle*_tmp392;struct Cyc_Toc_Env*_tmp390=nv;_tmp391=*
_tmp390;_tmp392=_tmp391.rgn;{struct Cyc_Toc_Env*_tmp393=Cyc_Toc_add_varmap(
_tmp392,nv,_tmp38F,Cyc_Absyn_var_exp(_tmp38F,0));struct Cyc_Absyn_Vardecl*_tmp394=({
struct Cyc_Absyn_Vardecl*_tmp3C3=_cycalloc(sizeof(*_tmp3C3));_tmp3C3->sc=(void*)((
void*)2);_tmp3C3->name=_tmp38F;_tmp3C3->tq=Cyc_Toc_mt_tq;_tmp3C3->type=(void*)
_tmp383;_tmp3C3->initializer=(struct Cyc_Absyn_Exp*)xinit;_tmp3C3->rgn=0;_tmp3C3->attributes=
0;_tmp3C3->escapes=0;_tmp3C3;});struct Cyc_Absyn_Local_b_struct*_tmp395=({struct
Cyc_Absyn_Local_b_struct*_tmp3C1=_cycalloc(sizeof(*_tmp3C1));_tmp3C1[0]=({struct
Cyc_Absyn_Local_b_struct _tmp3C2;_tmp3C2.tag=3;_tmp3C2.f1=_tmp394;_tmp3C2;});
_tmp3C1;});struct Cyc_Absyn_Exp*_tmp396=Cyc_Absyn_varb_exp(_tmp38F,(void*)_tmp395,
0);_tmp396->topt=({struct Cyc_Core_Opt*_tmp397=_cycalloc(sizeof(*_tmp397));
_tmp397->v=(void*)fat_ptr_type;_tmp397;});{struct Cyc_Absyn_Exp*_tmp398=Cyc_Absyn_deref_exp(
_tmp396,0);_tmp398->topt=({struct Cyc_Core_Opt*_tmp399=_cycalloc(sizeof(*_tmp399));
_tmp399->v=(void*)elt_type;_tmp399;});Cyc_Toc_exp_to_c(_tmp393,_tmp398);{struct
_tuple1*_tmp39A=Cyc_Toc_temp_var();_tmp393=Cyc_Toc_add_varmap(_tmp392,_tmp393,
_tmp39A,Cyc_Absyn_var_exp(_tmp39A,0));{struct Cyc_Absyn_Vardecl*_tmp39B=({struct
Cyc_Absyn_Vardecl*_tmp3C0=_cycalloc(sizeof(*_tmp3C0));_tmp3C0->sc=(void*)((void*)
2);_tmp3C0->name=_tmp39A;_tmp3C0->tq=Cyc_Toc_mt_tq;_tmp3C0->type=(void*)_tmp382;
_tmp3C0->initializer=(struct Cyc_Absyn_Exp*)_tmp398;_tmp3C0->rgn=0;_tmp3C0->attributes=
0;_tmp3C0->escapes=0;_tmp3C0;});struct Cyc_Absyn_Local_b_struct*_tmp39C=({struct
Cyc_Absyn_Local_b_struct*_tmp3BE=_cycalloc(sizeof(*_tmp3BE));_tmp3BE[0]=({struct
Cyc_Absyn_Local_b_struct _tmp3BF;_tmp3BF.tag=3;_tmp3BF.f1=_tmp39B;_tmp3BF;});
_tmp3BE;});struct Cyc_Absyn_Exp*z_init=e2;if(popt != 0){struct Cyc_Absyn_Exp*
_tmp39D=Cyc_Absyn_varb_exp(_tmp39A,(void*)_tmp39C,0);_tmp39D->topt=_tmp398->topt;
z_init=Cyc_Absyn_prim2_exp((void*)popt->v,_tmp39D,e2,0);z_init->topt=_tmp39D->topt;}
Cyc_Toc_exp_to_c(_tmp393,z_init);{struct _tuple1*_tmp39E=Cyc_Toc_temp_var();
struct Cyc_Absyn_Vardecl*_tmp39F=({struct Cyc_Absyn_Vardecl*_tmp3BD=_cycalloc(
sizeof(*_tmp3BD));_tmp3BD->sc=(void*)((void*)2);_tmp3BD->name=_tmp39E;_tmp3BD->tq=
Cyc_Toc_mt_tq;_tmp3BD->type=(void*)_tmp382;_tmp3BD->initializer=(struct Cyc_Absyn_Exp*)
z_init;_tmp3BD->rgn=0;_tmp3BD->attributes=0;_tmp3BD->escapes=0;_tmp3BD;});struct
Cyc_Absyn_Local_b_struct*_tmp3A0=({struct Cyc_Absyn_Local_b_struct*_tmp3BB=
_cycalloc(sizeof(*_tmp3BB));_tmp3BB[0]=({struct Cyc_Absyn_Local_b_struct _tmp3BC;
_tmp3BC.tag=3;_tmp3BC.f1=_tmp39F;_tmp3BC;});_tmp3BB;});_tmp393=Cyc_Toc_add_varmap(
_tmp392,_tmp393,_tmp39E,Cyc_Absyn_var_exp(_tmp39E,0));{struct Cyc_Absyn_Exp*
_tmp3A1=Cyc_Absyn_varb_exp(_tmp39A,(void*)_tmp39C,0);_tmp3A1->topt=_tmp398->topt;{
struct Cyc_Absyn_Exp*_tmp3A2=Cyc_Toc_generate_zero(elt_type);struct Cyc_Absyn_Exp*
_tmp3A3=Cyc_Absyn_prim2_exp((void*)5,_tmp3A1,_tmp3A2,0);_tmp3A3->topt=({struct
Cyc_Core_Opt*_tmp3A4=_cycalloc(sizeof(*_tmp3A4));_tmp3A4->v=(void*)Cyc_Absyn_sint_typ;
_tmp3A4;});Cyc_Toc_exp_to_c(_tmp393,_tmp3A3);{struct Cyc_Absyn_Exp*_tmp3A5=Cyc_Absyn_varb_exp(
_tmp39E,(void*)_tmp3A0,0);_tmp3A5->topt=_tmp398->topt;{struct Cyc_Absyn_Exp*
_tmp3A6=Cyc_Toc_generate_zero(elt_type);struct Cyc_Absyn_Exp*_tmp3A7=Cyc_Absyn_prim2_exp((
void*)6,_tmp3A5,_tmp3A6,0);_tmp3A7->topt=({struct Cyc_Core_Opt*_tmp3A8=_cycalloc(
sizeof(*_tmp3A8));_tmp3A8->v=(void*)Cyc_Absyn_sint_typ;_tmp3A8;});Cyc_Toc_exp_to_c(
_tmp393,_tmp3A7);{struct Cyc_List_List*_tmp3A9=({struct Cyc_Absyn_Exp*_tmp3BA[2];
_tmp3BA[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_type),0);_tmp3BA[0]=Cyc_Absyn_varb_exp(
_tmp38F,(void*)_tmp395,0);((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmp3BA,sizeof(struct Cyc_Absyn_Exp*),2));});struct Cyc_Absyn_Exp*
_tmp3AA=Cyc_Absyn_uint_exp(1,0);struct Cyc_Absyn_Exp*xsize;xsize=Cyc_Absyn_prim2_exp((
void*)5,Cyc_Absyn_fncall_exp(Cyc_Toc__get_dyneither_size_e,_tmp3A9,0),_tmp3AA,0);{
struct Cyc_Absyn_Exp*_tmp3AB=Cyc_Absyn_and_exp(xsize,Cyc_Absyn_and_exp(_tmp3A3,
_tmp3A7,0),0);struct Cyc_Absyn_Stmt*_tmp3AC=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(
Cyc_Toc__throw_arraybounds_e,0,0),0);struct Cyc_Absyn_Exp*_tmp3AD=Cyc_Absyn_aggrmember_exp(
Cyc_Absyn_varb_exp(_tmp38F,(void*)_tmp395,0),Cyc_Toc_curr_sp,0);_tmp3AD=Cyc_Toc_cast_it(
_tmp384,_tmp3AD);{struct Cyc_Absyn_Exp*_tmp3AE=Cyc_Absyn_deref_exp(_tmp3AD,0);
struct Cyc_Absyn_Exp*_tmp3AF=Cyc_Absyn_assign_exp(_tmp3AE,Cyc_Absyn_var_exp(
_tmp39E,0),0);struct Cyc_Absyn_Stmt*_tmp3B0=Cyc_Absyn_exp_stmt(_tmp3AF,0);_tmp3B0=
Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(_tmp3AB,_tmp3AC,Cyc_Absyn_skip_stmt(
0),0),_tmp3B0,0);_tmp3B0=Cyc_Absyn_decl_stmt(({struct Cyc_Absyn_Decl*_tmp3B1=
_cycalloc(sizeof(*_tmp3B1));_tmp3B1->r=(void*)((void*)({struct Cyc_Absyn_Var_d_struct*
_tmp3B2=_cycalloc(sizeof(*_tmp3B2));_tmp3B2[0]=({struct Cyc_Absyn_Var_d_struct
_tmp3B3;_tmp3B3.tag=0;_tmp3B3.f1=_tmp39F;_tmp3B3;});_tmp3B2;}));_tmp3B1->loc=0;
_tmp3B1;}),_tmp3B0,0);_tmp3B0=Cyc_Absyn_decl_stmt(({struct Cyc_Absyn_Decl*_tmp3B4=
_cycalloc(sizeof(*_tmp3B4));_tmp3B4->r=(void*)((void*)({struct Cyc_Absyn_Var_d_struct*
_tmp3B5=_cycalloc(sizeof(*_tmp3B5));_tmp3B5[0]=({struct Cyc_Absyn_Var_d_struct
_tmp3B6;_tmp3B6.tag=0;_tmp3B6.f1=_tmp39B;_tmp3B6;});_tmp3B5;}));_tmp3B4->loc=0;
_tmp3B4;}),_tmp3B0,0);_tmp3B0=Cyc_Absyn_decl_stmt(({struct Cyc_Absyn_Decl*_tmp3B7=
_cycalloc(sizeof(*_tmp3B7));_tmp3B7->r=(void*)((void*)({struct Cyc_Absyn_Var_d_struct*
_tmp3B8=_cycalloc(sizeof(*_tmp3B8));_tmp3B8[0]=({struct Cyc_Absyn_Var_d_struct
_tmp3B9;_tmp3B9.tag=0;_tmp3B9.f1=_tmp394;_tmp3B9;});_tmp3B8;}));_tmp3B7->loc=0;
_tmp3B7;}),_tmp3B0,0);(void*)(e->r=(void*)Cyc_Toc_stmt_exp_r(_tmp3B0));}}}}}}}}}}}}}}
struct _tuple11{struct _tuple1*f1;void*f2;struct Cyc_Absyn_Exp*f3;};struct _tuple12{
struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Exp*f2;};static void Cyc_Toc_exp_to_c(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e){void*_tmp3C5=(void*)e->r;if(e->topt
== 0)({struct Cyc_String_pa_struct _tmp3C8;_tmp3C8.tag=0;_tmp3C8.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));{void*_tmp3C6[1]={&
_tmp3C8};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp3C7="exp_to_c: no type for %s";_tag_dyneither(_tmp3C7,sizeof(char),
25);}),_tag_dyneither(_tmp3C6,sizeof(void*),1));}});{void*old_typ=(void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v;void*_tmp3C9=_tmp3C5;union Cyc_Absyn_Cnst_union
_tmp3CA;struct _tuple1*_tmp3CB;void*_tmp3CC;struct _tuple1*_tmp3CD;void*_tmp3CE;
struct Cyc_List_List*_tmp3CF;struct Cyc_Absyn_Exp*_tmp3D0;void*_tmp3D1;struct Cyc_Absyn_Exp*
_tmp3D2;struct Cyc_Core_Opt*_tmp3D3;struct Cyc_Absyn_Exp*_tmp3D4;struct Cyc_Absyn_Exp*
_tmp3D5;struct Cyc_Absyn_Exp*_tmp3D6;struct Cyc_Absyn_Exp*_tmp3D7;struct Cyc_Absyn_Exp*
_tmp3D8;struct Cyc_Absyn_Exp*_tmp3D9;struct Cyc_Absyn_Exp*_tmp3DA;struct Cyc_Absyn_Exp*
_tmp3DB;struct Cyc_Absyn_Exp*_tmp3DC;struct Cyc_Absyn_Exp*_tmp3DD;struct Cyc_Absyn_Exp*
_tmp3DE;struct Cyc_List_List*_tmp3DF;struct Cyc_Absyn_Exp*_tmp3E0;struct Cyc_List_List*
_tmp3E1;struct Cyc_Absyn_VarargCallInfo*_tmp3E2;struct Cyc_Absyn_Exp*_tmp3E3;
struct Cyc_List_List*_tmp3E4;struct Cyc_Absyn_VarargCallInfo*_tmp3E5;struct Cyc_Absyn_VarargCallInfo
_tmp3E6;int _tmp3E7;struct Cyc_List_List*_tmp3E8;struct Cyc_Absyn_VarargInfo*
_tmp3E9;struct Cyc_Absyn_Exp*_tmp3EA;struct Cyc_Absyn_Exp*_tmp3EB;struct Cyc_Absyn_Exp*
_tmp3EC;struct Cyc_List_List*_tmp3ED;void*_tmp3EE;void**_tmp3EF;struct Cyc_Absyn_Exp*
_tmp3F0;int _tmp3F1;void*_tmp3F2;struct Cyc_Absyn_Exp*_tmp3F3;struct Cyc_Absyn_Exp*
_tmp3F4;struct Cyc_Absyn_Exp*_tmp3F5;struct Cyc_Absyn_Exp*_tmp3F6;void*_tmp3F7;
void*_tmp3F8;void*_tmp3F9;struct _dyneither_ptr*_tmp3FA;void*_tmp3FB;void*_tmp3FC;
unsigned int _tmp3FD;struct Cyc_Absyn_Exp*_tmp3FE;struct Cyc_Absyn_Exp*_tmp3FF;
struct _dyneither_ptr*_tmp400;struct Cyc_Absyn_Exp*_tmp401;struct _dyneither_ptr*
_tmp402;struct Cyc_Absyn_Exp*_tmp403;struct Cyc_Absyn_Exp*_tmp404;struct Cyc_List_List*
_tmp405;struct Cyc_List_List*_tmp406;struct Cyc_Absyn_Vardecl*_tmp407;struct Cyc_Absyn_Exp*
_tmp408;struct Cyc_Absyn_Exp*_tmp409;int _tmp40A;struct _tuple1*_tmp40B;struct Cyc_List_List*
_tmp40C;struct Cyc_List_List*_tmp40D;struct Cyc_Absyn_Aggrdecl*_tmp40E;void*
_tmp40F;struct Cyc_List_List*_tmp410;struct Cyc_List_List*_tmp411;struct Cyc_Absyn_Tuniondecl*
_tmp412;struct Cyc_Absyn_Tunionfield*_tmp413;struct Cyc_List_List*_tmp414;struct
Cyc_Absyn_Tuniondecl*_tmp415;struct Cyc_Absyn_Tunionfield*_tmp416;struct Cyc_Absyn_MallocInfo
_tmp417;int _tmp418;struct Cyc_Absyn_Exp*_tmp419;void**_tmp41A;struct Cyc_Absyn_Exp*
_tmp41B;int _tmp41C;struct Cyc_Absyn_Exp*_tmp41D;struct Cyc_Absyn_Exp*_tmp41E;
struct Cyc_Absyn_Stmt*_tmp41F;_LL1E6: if(*((int*)_tmp3C9)!= 0)goto _LL1E8;_tmp3CA=((
struct Cyc_Absyn_Const_e_struct*)_tmp3C9)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)
_tmp3C9)->f1).Null_c).tag != 6)goto _LL1E8;_LL1E7: {struct Cyc_Absyn_Exp*_tmp420=
Cyc_Absyn_uint_exp(0,0);if(Cyc_Tcutil_is_tagged_pointer_typ(old_typ)){if(Cyc_Toc_is_toplevel(
nv))(void*)(e->r=(void*)((void*)(Cyc_Toc_make_toplevel_dyn_arr(old_typ,_tmp420,
_tmp420))->r));else{(void*)(e->r=(void*)Cyc_Toc_fncall_exp_r(Cyc_Toc__tag_dyneither_e,({
struct Cyc_Absyn_Exp*_tmp421[3];_tmp421[2]=_tmp420;_tmp421[1]=_tmp420;_tmp421[0]=
_tmp420;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmp421,sizeof(struct Cyc_Absyn_Exp*),3));})));}}else{(void*)(e->r=(
void*)((void*)& Cyc_Toc_zero_exp));}goto _LL1E5;}_LL1E8: if(*((int*)_tmp3C9)!= 0)
goto _LL1EA;_LL1E9: goto _LL1E5;_LL1EA: if(*((int*)_tmp3C9)!= 1)goto _LL1EC;_tmp3CB=((
struct Cyc_Absyn_Var_e_struct*)_tmp3C9)->f1;_tmp3CC=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp3C9)->f2;_LL1EB:{struct _handler_cons _tmp422;_push_handler(& _tmp422);{int
_tmp424=0;if(setjmp(_tmp422.handler))_tmp424=1;if(!_tmp424){(void*)(e->r=(void*)((
void*)(Cyc_Toc_lookup_varmap(nv,_tmp3CB))->r));;_pop_handler();}else{void*
_tmp423=(void*)_exn_thrown;void*_tmp426=_tmp423;_LL23F: if(_tmp426 != Cyc_Dict_Absent)
goto _LL241;_LL240:({struct Cyc_String_pa_struct _tmp429;_tmp429.tag=0;_tmp429.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp3CB));{
void*_tmp427[1]={& _tmp429};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap))Cyc_Toc_toc_impos)(({const char*_tmp428="Can't find %s in exp_to_c, Var\n";
_tag_dyneither(_tmp428,sizeof(char),32);}),_tag_dyneither(_tmp427,sizeof(void*),
1));}});_LL241:;_LL242:(void)_throw(_tmp426);_LL23E:;}}}goto _LL1E5;_LL1EC: if(*((
int*)_tmp3C9)!= 2)goto _LL1EE;_tmp3CD=((struct Cyc_Absyn_UnknownId_e_struct*)
_tmp3C9)->f1;_LL1ED:({void*_tmp42A=0;((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp42B="unknownid";
_tag_dyneither(_tmp42B,sizeof(char),10);}),_tag_dyneither(_tmp42A,sizeof(void*),
0));});_LL1EE: if(*((int*)_tmp3C9)!= 3)goto _LL1F0;_tmp3CE=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp3C9)->f1;_tmp3CF=((struct Cyc_Absyn_Primop_e_struct*)_tmp3C9)->f2;_LL1EF: {
struct Cyc_List_List*_tmp42C=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Exp*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_get_cyc_typ,_tmp3CF);((void(*)(void(*
f)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*
x))Cyc_List_iter_c)(Cyc_Toc_exp_to_c,nv,_tmp3CF);{void*_tmp42D=_tmp3CE;_LL244:
if((int)_tmp42D != 19)goto _LL246;_LL245: {struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp3CF))->hd;{void*_tmp42E=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(arg->topt))->v);struct Cyc_Absyn_ArrayInfo
_tmp42F;struct Cyc_Absyn_Exp*_tmp430;struct Cyc_Absyn_PtrInfo _tmp431;void*_tmp432;
struct Cyc_Absyn_PtrAtts _tmp433;struct Cyc_Absyn_Conref*_tmp434;struct Cyc_Absyn_Conref*
_tmp435;struct Cyc_Absyn_Conref*_tmp436;_LL259: if(_tmp42E <= (void*)4)goto _LL25D;
if(*((int*)_tmp42E)!= 7)goto _LL25B;_tmp42F=((struct Cyc_Absyn_ArrayType_struct*)
_tmp42E)->f1;_tmp430=_tmp42F.num_elts;_LL25A: if(!Cyc_Evexp_c_can_eval((struct Cyc_Absyn_Exp*)
_check_null(_tmp430)))({void*_tmp437=0;Cyc_Tcutil_terr(e->loc,({const char*
_tmp438="can't calculate numelts";_tag_dyneither(_tmp438,sizeof(char),24);}),
_tag_dyneither(_tmp437,sizeof(void*),0));});(void*)(e->r=(void*)((void*)_tmp430->r));
goto _LL258;_LL25B: if(*((int*)_tmp42E)!= 4)goto _LL25D;_tmp431=((struct Cyc_Absyn_PointerType_struct*)
_tmp42E)->f1;_tmp432=(void*)_tmp431.elt_typ;_tmp433=_tmp431.ptr_atts;_tmp434=
_tmp433.nullable;_tmp435=_tmp433.bounds;_tmp436=_tmp433.zero_term;_LL25C:{void*
_tmp439=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp435);struct Cyc_Absyn_Exp*
_tmp43A;_LL260: if((int)_tmp439 != 0)goto _LL262;_LL261:(void*)(e->r=(void*)Cyc_Toc_fncall_exp_r(
Cyc_Toc__get_dyneither_size_e,({struct Cyc_Absyn_Exp*_tmp43B[2];_tmp43B[1]=Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c(_tmp432),0);_tmp43B[0]=(struct Cyc_Absyn_Exp*)_tmp3CF->hd;((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmp43B,sizeof(struct Cyc_Absyn_Exp*),2));})));goto _LL25F;_LL262: if(_tmp439 <= (
void*)1)goto _LL25F;if(*((int*)_tmp439)!= 0)goto _LL25F;_tmp43A=((struct Cyc_Absyn_Upper_b_struct*)
_tmp439)->f1;_LL263: if(((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,_tmp436)){struct Cyc_Absyn_Exp*function_e=Cyc_Toc_getFunction(& Cyc_Toc__get_zero_arr_size_functionSet,(
struct Cyc_Absyn_Exp*)_tmp3CF->hd);(void*)(e->r=(void*)Cyc_Toc_fncall_exp_r(
function_e,({struct Cyc_Absyn_Exp*_tmp43C[2];_tmp43C[1]=_tmp43A;_tmp43C[0]=(
struct Cyc_Absyn_Exp*)_tmp3CF->hd;((struct Cyc_List_List*(*)(struct _dyneither_ptr))
Cyc_List_list)(_tag_dyneither(_tmp43C,sizeof(struct Cyc_Absyn_Exp*),2));})));}
else{if(((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp434)){
if(!Cyc_Evexp_c_can_eval(_tmp43A))({void*_tmp43D=0;Cyc_Tcutil_terr(e->loc,({
const char*_tmp43E="can't calculate numelts";_tag_dyneither(_tmp43E,sizeof(char),
24);}),_tag_dyneither(_tmp43D,sizeof(void*),0));});(void*)(e->r=(void*)Cyc_Toc_conditional_exp_r(
arg,_tmp43A,Cyc_Absyn_uint_exp(0,0)));}else{(void*)(e->r=(void*)((void*)_tmp43A->r));
goto _LL25F;}}goto _LL25F;_LL25F:;}goto _LL258;_LL25D:;_LL25E:({struct Cyc_String_pa_struct
_tmp442;_tmp442.tag=0;_tmp442.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(arg->topt))->v));{
struct Cyc_String_pa_struct _tmp441;_tmp441.tag=0;_tmp441.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(arg->topt))->v));{void*_tmp43F[2]={& _tmp441,& _tmp442};((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp440="size primop applied to non-array %s (%s)";_tag_dyneither(_tmp440,
sizeof(char),41);}),_tag_dyneither(_tmp43F,sizeof(void*),2));}}});_LL258:;}goto
_LL243;}_LL246: if((int)_tmp42D != 0)goto _LL248;_LL247:{void*_tmp443=Cyc_Tcutil_compress((
void*)((struct Cyc_List_List*)_check_null(_tmp42C))->hd);struct Cyc_Absyn_PtrInfo
_tmp444;void*_tmp445;struct Cyc_Absyn_PtrAtts _tmp446;struct Cyc_Absyn_Conref*
_tmp447;struct Cyc_Absyn_Conref*_tmp448;_LL265: if(_tmp443 <= (void*)4)goto _LL267;
if(*((int*)_tmp443)!= 4)goto _LL267;_tmp444=((struct Cyc_Absyn_PointerType_struct*)
_tmp443)->f1;_tmp445=(void*)_tmp444.elt_typ;_tmp446=_tmp444.ptr_atts;_tmp447=
_tmp446.bounds;_tmp448=_tmp446.zero_term;_LL266:{void*_tmp449=Cyc_Absyn_conref_def(
Cyc_Absyn_bounds_one,_tmp447);struct Cyc_Absyn_Exp*_tmp44A;_LL26A: if((int)_tmp449
!= 0)goto _LL26C;_LL26B: {struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct
Cyc_List_List*)_check_null(_tmp3CF))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp3CF->tl))->hd;(void*)(e->r=(void*)Cyc_Toc_fncall_exp_r(
Cyc_Toc__dyneither_ptr_plus_e,({struct Cyc_Absyn_Exp*_tmp44B[3];_tmp44B[2]=e2;
_tmp44B[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp445),0);_tmp44B[0]=e1;((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmp44B,sizeof(struct Cyc_Absyn_Exp*),3));})));goto _LL269;}_LL26C: if(_tmp449 <= (
void*)1)goto _LL269;if(*((int*)_tmp449)!= 0)goto _LL269;_tmp44A=((struct Cyc_Absyn_Upper_b_struct*)
_tmp449)->f1;_LL26D: if(((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,_tmp448)){struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp3CF))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct
Cyc_List_List*)_check_null(_tmp3CF->tl))->hd;(void*)(e->r=(void*)((void*)(Cyc_Absyn_fncall_exp(
Cyc_Toc_getFunction(& Cyc_Toc__zero_arr_plus_functionSet,e1),({struct Cyc_Absyn_Exp*
_tmp44C[3];_tmp44C[2]=e2;_tmp44C[1]=_tmp44A;_tmp44C[0]=e1;((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp44C,sizeof(struct Cyc_Absyn_Exp*),
3));}),0))->r));}goto _LL269;_LL269:;}goto _LL264;_LL267:;_LL268: goto _LL264;_LL264:;}
goto _LL243;_LL248: if((int)_tmp42D != 2)goto _LL24A;_LL249: {void*elt_typ=(void*)0;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt((void*)((struct Cyc_List_List*)
_check_null(_tmp42C))->hd,& elt_typ)){struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp3CF))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp3CF->tl))->hd;if(Cyc_Tcutil_is_tagged_pointer_typ((
void*)((struct Cyc_List_List*)_check_null(_tmp42C->tl))->hd)){(void*)(e1->r=(void*)
Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp((void*)e1->r,0),Cyc_Toc_curr_sp));(
void*)(e2->r=(void*)Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp((void*)e2->r,0),
Cyc_Toc_curr_sp));(void*)(e->r=(void*)((void*)(Cyc_Absyn_divide_exp(Cyc_Absyn_copy_exp(
e),Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0),0))->r));}else{(void*)(e->r=(
void*)Cyc_Toc_fncall_exp_r(Cyc_Toc__dyneither_ptr_plus_e,({struct Cyc_Absyn_Exp*
_tmp44D[3];_tmp44D[2]=Cyc_Absyn_prim1_exp((void*)2,e2,0);_tmp44D[1]=Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c(elt_typ),0);_tmp44D[0]=e1;((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp44D,sizeof(struct Cyc_Absyn_Exp*),
3));})));}}goto _LL243;}_LL24A: if((int)_tmp42D != 5)goto _LL24C;_LL24B: goto _LL24D;
_LL24C: if((int)_tmp42D != 6)goto _LL24E;_LL24D: goto _LL24F;_LL24E: if((int)_tmp42D != 
7)goto _LL250;_LL24F: goto _LL251;_LL250: if((int)_tmp42D != 9)goto _LL252;_LL251: goto
_LL253;_LL252: if((int)_tmp42D != 8)goto _LL254;_LL253: goto _LL255;_LL254: if((int)
_tmp42D != 10)goto _LL256;_LL255: {struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp3CF))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp3CF->tl))->hd;void*t1=(void*)((struct Cyc_List_List*)
_check_null(_tmp42C))->hd;void*t2=(void*)((struct Cyc_List_List*)_check_null(
_tmp42C->tl))->hd;if(Cyc_Tcutil_is_tagged_pointer_typ(t1))(void*)(e1->r=(void*)
Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp((void*)e1->r,0),Cyc_Toc_curr_sp));if(
Cyc_Tcutil_is_tagged_pointer_typ(t2))(void*)(e2->r=(void*)Cyc_Toc_aggrmember_exp_r(
Cyc_Absyn_new_exp((void*)e2->r,0),Cyc_Toc_curr_sp));goto _LL243;}_LL256:;_LL257:
goto _LL243;_LL243:;}goto _LL1E5;}_LL1F0: if(*((int*)_tmp3C9)!= 5)goto _LL1F2;
_tmp3D0=((struct Cyc_Absyn_Increment_e_struct*)_tmp3C9)->f1;_tmp3D1=(void*)((
struct Cyc_Absyn_Increment_e_struct*)_tmp3C9)->f2;_LL1F1: {void*e2_cyc_typ=(void*)((
struct Cyc_Core_Opt*)_check_null(_tmp3D0->topt))->v;void*ptr_type=(void*)0;void*
elt_type=(void*)0;int is_dyneither=0;struct _dyneither_ptr incr_str=({const char*
_tmp460="increment";_tag_dyneither(_tmp460,sizeof(char),10);});if(_tmp3D1 == (
void*)2  || _tmp3D1 == (void*)3)incr_str=({const char*_tmp44E="decrement";
_tag_dyneither(_tmp44E,sizeof(char),10);});if(Cyc_Tcutil_is_zero_ptr_deref(
_tmp3D0,& ptr_type,& is_dyneither,& elt_type)){({struct Cyc_String_pa_struct _tmp451;
_tmp451.tag=0;_tmp451.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)incr_str);{
void*_tmp44F[1]={& _tmp451};Cyc_Tcutil_terr(e->loc,({const char*_tmp450="in-place %s is not supported when dereferencing a zero-terminated pointer";
_tag_dyneither(_tmp450,sizeof(char),74);}),_tag_dyneither(_tmp44F,sizeof(void*),
1));}});({void*_tmp452=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Toc_toc_impos)(({const char*_tmp453="in-place inc on zero-term";
_tag_dyneither(_tmp453,sizeof(char),26);}),_tag_dyneither(_tmp452,sizeof(void*),
0));});}Cyc_Toc_exp_to_c(nv,_tmp3D0);{void*elt_typ=(void*)0;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(
old_typ,& elt_typ)){struct Cyc_Absyn_Exp*fn_e;int change=1;fn_e=(_tmp3D1 == (void*)1
 || _tmp3D1 == (void*)3)?Cyc_Toc__dyneither_ptr_inplace_plus_post_e: Cyc_Toc__dyneither_ptr_inplace_plus_e;
if(_tmp3D1 == (void*)2  || _tmp3D1 == (void*)3)change=- 1;(void*)(e->r=(void*)Cyc_Toc_fncall_exp_r(
fn_e,({struct Cyc_Absyn_Exp*_tmp454[3];_tmp454[2]=Cyc_Absyn_signed_int_exp(change,
0);_tmp454[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0);_tmp454[0]=
Cyc_Absyn_address_exp(_tmp3D0,0);((struct Cyc_List_List*(*)(struct _dyneither_ptr))
Cyc_List_list)(_tag_dyneither(_tmp454,sizeof(struct Cyc_Absyn_Exp*),3));})));}
else{if(Cyc_Tcutil_is_zero_pointer_typ_elt(old_typ,& elt_typ)){struct Cyc_Absyn_Exp*
fn_e;{void*_tmp455=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp3D0->topt))->v);void*_tmp456;int _tmp457;_LL26F: if(_tmp455 <= (void*)4)goto
_LL271;if(*((int*)_tmp455)!= 5)goto _LL271;_tmp456=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp455)->f2;_LL270:{void*_tmp458=_tmp456;_LL27A: if((int)_tmp458 != 0)goto _LL27C;
_LL27B: fn_e=_tmp3D1 == (void*)1?Cyc_Toc__zero_arr_inplace_plus_post_char_e: Cyc_Toc__zero_arr_inplace_plus_char_e;
goto _LL279;_LL27C: if((int)_tmp458 != 1)goto _LL27E;_LL27D: fn_e=_tmp3D1 == (void*)1?
Cyc_Toc__zero_arr_inplace_plus_post_short_e: Cyc_Toc__zero_arr_inplace_plus_short_e;
goto _LL279;_LL27E: if((int)_tmp458 != 2)goto _LL280;_LL27F: fn_e=_tmp3D1 == (void*)1?
Cyc_Toc__zero_arr_inplace_plus_post_int_e: Cyc_Toc__zero_arr_inplace_plus_int_e;
goto _LL279;_LL280:;_LL281:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp459=_cycalloc(sizeof(*_tmp459));_tmp459[0]=({struct Cyc_Core_Impossible_struct
_tmp45A;_tmp45A.tag=Cyc_Core_Impossible;_tmp45A.f1=({const char*_tmp45B="impossible IntType (not char, short or int)";
_tag_dyneither(_tmp45B,sizeof(char),44);});_tmp45A;});_tmp459;}));_LL279:;}goto
_LL26E;_LL271: if((int)_tmp455 != 1)goto _LL273;_LL272: fn_e=_tmp3D1 == (void*)1?Cyc_Toc__zero_arr_inplace_plus_post_float_e:
Cyc_Toc__zero_arr_inplace_plus_float_e;goto _LL26E;_LL273: if(_tmp455 <= (void*)4)
goto _LL277;if(*((int*)_tmp455)!= 6)goto _LL275;_tmp457=((struct Cyc_Absyn_DoubleType_struct*)
_tmp455)->f1;_LL274: switch(_tmp457){case 1: _LL282: fn_e=_tmp3D1 == (void*)1?Cyc_Toc__zero_arr_inplace_plus_post_longdouble_e:
Cyc_Toc__zero_arr_inplace_plus_longdouble_e;break;default: _LL283: fn_e=_tmp3D1 == (
void*)1?Cyc_Toc__zero_arr_inplace_plus_post_double_e: Cyc_Toc__zero_arr_inplace_plus_double_e;}
goto _LL26E;_LL275: if(*((int*)_tmp455)!= 4)goto _LL277;_LL276: fn_e=_tmp3D1 == (void*)
1?Cyc_Toc__zero_arr_inplace_plus_post_voidstar_e: Cyc_Toc__zero_arr_inplace_plus_voidstar_e;
goto _LL26E;_LL277:;_LL278:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp45C=_cycalloc(sizeof(*_tmp45C));_tmp45C[0]=({struct Cyc_Core_Impossible_struct
_tmp45D;_tmp45D.tag=Cyc_Core_Impossible;_tmp45D.f1=({const char*_tmp45E="impossible expression type (not int, float, double, or pointer)";
_tag_dyneither(_tmp45E,sizeof(char),64);});_tmp45D;});_tmp45C;}));_LL26E:;}(void*)(
e->r=(void*)Cyc_Toc_fncall_exp_r(fn_e,({struct Cyc_Absyn_Exp*_tmp45F[2];_tmp45F[1]=
Cyc_Absyn_signed_int_exp(1,0);_tmp45F[0]=_tmp3D0;((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp45F,sizeof(struct Cyc_Absyn_Exp*),
2));})));}else{if(elt_typ == (void*)0  && !Cyc_Absyn_is_lvalue(_tmp3D0)){Cyc_Toc_lvalue_assign(
_tmp3D0,0,Cyc_Toc_incr_lvalue,_tmp3D1);(void*)(e->r=(void*)((void*)_tmp3D0->r));}}}
goto _LL1E5;}}_LL1F2: if(*((int*)_tmp3C9)!= 4)goto _LL1F4;_tmp3D2=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmp3C9)->f1;_tmp3D3=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp3C9)->f2;_tmp3D4=((
struct Cyc_Absyn_AssignOp_e_struct*)_tmp3C9)->f3;_LL1F3: {void*ptr_type=(void*)0;
void*elt_type=(void*)0;int is_dyneither=0;if(Cyc_Tcutil_is_zero_ptr_deref(_tmp3D2,&
ptr_type,& is_dyneither,& elt_type)){Cyc_Toc_zero_ptr_assign_to_c(nv,e,_tmp3D2,
_tmp3D3,_tmp3D4,ptr_type,is_dyneither,elt_type);return;}{int e1_poly=Cyc_Toc_is_poly_project(
_tmp3D2);void*e1_old_typ=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp3D2->topt))->v;
void*e2_old_typ=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp3D4->topt))->v;Cyc_Toc_exp_to_c(
nv,_tmp3D2);Cyc_Toc_exp_to_c(nv,_tmp3D4);{int done=0;if(_tmp3D3 != 0){void*elt_typ=(
void*)0;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(old_typ,& elt_typ)){struct Cyc_Absyn_Exp*
change;{void*_tmp461=(void*)_tmp3D3->v;_LL286: if((int)_tmp461 != 0)goto _LL288;
_LL287: change=_tmp3D4;goto _LL285;_LL288: if((int)_tmp461 != 2)goto _LL28A;_LL289:
change=Cyc_Absyn_prim1_exp((void*)2,_tmp3D4,0);goto _LL285;_LL28A:;_LL28B:({void*
_tmp462=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp463="bad t ? pointer arithmetic";_tag_dyneither(_tmp463,sizeof(
char),27);}),_tag_dyneither(_tmp462,sizeof(void*),0));});_LL285:;}done=1;{struct
Cyc_Absyn_Exp*_tmp464=Cyc_Toc__dyneither_ptr_inplace_plus_e;(void*)(e->r=(void*)
Cyc_Toc_fncall_exp_r(_tmp464,({struct Cyc_Absyn_Exp*_tmp465[3];_tmp465[2]=change;
_tmp465[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0);_tmp465[0]=Cyc_Absyn_address_exp(
_tmp3D2,0);((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmp465,sizeof(struct Cyc_Absyn_Exp*),3));})));}}else{if(Cyc_Tcutil_is_zero_pointer_typ_elt(
old_typ,& elt_typ)){void*_tmp466=(void*)_tmp3D3->v;_LL28D: if((int)_tmp466 != 0)
goto _LL28F;_LL28E: done=1;(void*)(e->r=(void*)Cyc_Toc_fncall_exp_r(Cyc_Toc_getFunction(&
Cyc_Toc__zero_arr_inplace_plus_functionSet,_tmp3D2),({struct Cyc_Absyn_Exp*
_tmp467[2];_tmp467[1]=_tmp3D4;_tmp467[0]=_tmp3D2;((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp467,sizeof(struct Cyc_Absyn_Exp*),
2));})));goto _LL28C;_LL28F:;_LL290:({void*_tmp468=0;((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp469="bad zero-terminated pointer arithmetic";
_tag_dyneither(_tmp469,sizeof(char),39);}),_tag_dyneither(_tmp468,sizeof(void*),
0));});_LL28C:;}}}if(!done){if(e1_poly)(void*)(_tmp3D4->r=(void*)Cyc_Toc_cast_it_r(
Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp((void*)_tmp3D4->r,0)));if(!Cyc_Absyn_is_lvalue(
_tmp3D2)){((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*
f)(struct Cyc_Absyn_Exp*,struct _tuple10*),struct _tuple10*f_env))Cyc_Toc_lvalue_assign)(
_tmp3D2,0,Cyc_Toc_assignop_lvalue,({struct _tuple10*_tmp46A=_cycalloc(sizeof(
struct _tuple10)* 1);_tmp46A[0]=({struct _tuple10 _tmp46B;_tmp46B.f1=_tmp3D3;
_tmp46B.f2=_tmp3D4;_tmp46B;});_tmp46A;}));(void*)(e->r=(void*)((void*)_tmp3D2->r));}}
goto _LL1E5;}}}_LL1F4: if(*((int*)_tmp3C9)!= 6)goto _LL1F6;_tmp3D5=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp3C9)->f1;_tmp3D6=((struct Cyc_Absyn_Conditional_e_struct*)_tmp3C9)->f2;
_tmp3D7=((struct Cyc_Absyn_Conditional_e_struct*)_tmp3C9)->f3;_LL1F5: Cyc_Toc_exp_to_c(
nv,_tmp3D5);Cyc_Toc_exp_to_c(nv,_tmp3D6);Cyc_Toc_exp_to_c(nv,_tmp3D7);goto _LL1E5;
_LL1F6: if(*((int*)_tmp3C9)!= 7)goto _LL1F8;_tmp3D8=((struct Cyc_Absyn_And_e_struct*)
_tmp3C9)->f1;_tmp3D9=((struct Cyc_Absyn_And_e_struct*)_tmp3C9)->f2;_LL1F7: Cyc_Toc_exp_to_c(
nv,_tmp3D8);Cyc_Toc_exp_to_c(nv,_tmp3D9);goto _LL1E5;_LL1F8: if(*((int*)_tmp3C9)!= 
8)goto _LL1FA;_tmp3DA=((struct Cyc_Absyn_Or_e_struct*)_tmp3C9)->f1;_tmp3DB=((
struct Cyc_Absyn_Or_e_struct*)_tmp3C9)->f2;_LL1F9: Cyc_Toc_exp_to_c(nv,_tmp3DA);
Cyc_Toc_exp_to_c(nv,_tmp3DB);goto _LL1E5;_LL1FA: if(*((int*)_tmp3C9)!= 9)goto
_LL1FC;_tmp3DC=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp3C9)->f1;_tmp3DD=((struct
Cyc_Absyn_SeqExp_e_struct*)_tmp3C9)->f2;_LL1FB: Cyc_Toc_exp_to_c(nv,_tmp3DC);Cyc_Toc_exp_to_c(
nv,_tmp3DD);goto _LL1E5;_LL1FC: if(*((int*)_tmp3C9)!= 10)goto _LL1FE;_tmp3DE=((
struct Cyc_Absyn_UnknownCall_e_struct*)_tmp3C9)->f1;_tmp3DF=((struct Cyc_Absyn_UnknownCall_e_struct*)
_tmp3C9)->f2;_LL1FD: _tmp3E0=_tmp3DE;_tmp3E1=_tmp3DF;goto _LL1FF;_LL1FE: if(*((int*)
_tmp3C9)!= 11)goto _LL200;_tmp3E0=((struct Cyc_Absyn_FnCall_e_struct*)_tmp3C9)->f1;
_tmp3E1=((struct Cyc_Absyn_FnCall_e_struct*)_tmp3C9)->f2;_tmp3E2=((struct Cyc_Absyn_FnCall_e_struct*)
_tmp3C9)->f3;if(_tmp3E2 != 0)goto _LL200;_LL1FF: Cyc_Toc_exp_to_c(nv,_tmp3E0);((
void(*)(void(*f)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,
struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Toc_exp_to_c,nv,_tmp3E1);goto _LL1E5;
_LL200: if(*((int*)_tmp3C9)!= 11)goto _LL202;_tmp3E3=((struct Cyc_Absyn_FnCall_e_struct*)
_tmp3C9)->f1;_tmp3E4=((struct Cyc_Absyn_FnCall_e_struct*)_tmp3C9)->f2;_tmp3E5=((
struct Cyc_Absyn_FnCall_e_struct*)_tmp3C9)->f3;if(_tmp3E5 == 0)goto _LL202;_tmp3E6=*
_tmp3E5;_tmp3E7=_tmp3E6.num_varargs;_tmp3E8=_tmp3E6.injectors;_tmp3E9=_tmp3E6.vai;
_LL201: {struct Cyc_Toc_Env _tmp46D;struct _RegionHandle*_tmp46E;struct Cyc_Toc_Env*
_tmp46C=nv;_tmp46D=*_tmp46C;_tmp46E=_tmp46D.rgn;{struct _tuple1*argv=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*argvexp=Cyc_Absyn_var_exp(argv,0);struct Cyc_Absyn_Exp*
num_varargs_exp=Cyc_Absyn_uint_exp((unsigned int)_tmp3E7,0);void*cva_type=Cyc_Toc_typ_to_c((
void*)_tmp3E9->type);void*arr_type=Cyc_Absyn_array_typ(cva_type,Cyc_Toc_mt_tq,(
struct Cyc_Absyn_Exp*)num_varargs_exp,Cyc_Absyn_false_conref,0);int num_args=((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmp3E4);int num_normargs=num_args - 
_tmp3E7;struct Cyc_List_List*new_args=0;{int i=0;for(0;i < num_normargs;(++ i,
_tmp3E4=_tmp3E4->tl)){Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp3E4))->hd);new_args=({struct Cyc_List_List*_tmp46F=_cycalloc(
sizeof(*_tmp46F));_tmp46F->hd=(struct Cyc_Absyn_Exp*)_tmp3E4->hd;_tmp46F->tl=
new_args;_tmp46F;});}}new_args=({struct Cyc_List_List*_tmp470=_cycalloc(sizeof(*
_tmp470));_tmp470->hd=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_dyneither_e,({struct Cyc_Absyn_Exp*
_tmp471[3];_tmp471[2]=num_varargs_exp;_tmp471[1]=Cyc_Absyn_sizeoftyp_exp(
cva_type,0);_tmp471[0]=argvexp;((struct Cyc_List_List*(*)(struct _dyneither_ptr))
Cyc_List_list)(_tag_dyneither(_tmp471,sizeof(struct Cyc_Absyn_Exp*),3));}),0);
_tmp470->tl=new_args;_tmp470;});new_args=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(new_args);Cyc_Toc_exp_to_c(nv,_tmp3E3);{struct Cyc_Absyn_Stmt*
s=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(_tmp3E3,new_args,0),0);if(_tmp3E9->inject){
struct Cyc_Absyn_Tuniondecl*tud;{void*_tmp472=Cyc_Tcutil_compress((void*)_tmp3E9->type);
struct Cyc_Absyn_TunionInfo _tmp473;union Cyc_Absyn_TunionInfoU_union _tmp474;struct
Cyc_Absyn_Tuniondecl**_tmp475;struct Cyc_Absyn_Tuniondecl*_tmp476;_LL292: if(
_tmp472 <= (void*)4)goto _LL294;if(*((int*)_tmp472)!= 2)goto _LL294;_tmp473=((
struct Cyc_Absyn_TunionType_struct*)_tmp472)->f1;_tmp474=_tmp473.tunion_info;if((((((
struct Cyc_Absyn_TunionType_struct*)_tmp472)->f1).tunion_info).KnownTunion).tag != 
1)goto _LL294;_tmp475=(_tmp474.KnownTunion).f1;_tmp476=*_tmp475;_LL293: tud=
_tmp476;goto _LL291;_LL294:;_LL295:({void*_tmp477=0;((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp478="toc: unknown tunion in vararg with inject";
_tag_dyneither(_tmp478,sizeof(char),42);}),_tag_dyneither(_tmp477,sizeof(void*),
0));});_LL291:;}{struct _dyneither_ptr vs=({unsigned int _tmp482=(unsigned int)
_tmp3E7;struct _tuple1**_tmp483=(struct _tuple1**)_region_malloc(_tmp46E,
_check_times(sizeof(struct _tuple1*),_tmp482));struct _dyneither_ptr _tmp485=
_tag_dyneither(_tmp483,sizeof(struct _tuple1*),_tmp482);{unsigned int _tmp484=
_tmp482;unsigned int i;for(i=0;i < _tmp484;i ++){_tmp483[i]=Cyc_Toc_temp_var();}}
_tmp485;});if(_tmp3E7 != 0){struct Cyc_List_List*_tmp479=0;{int i=_tmp3E7 - 1;for(0;
i >= 0;-- i){_tmp479=({struct Cyc_List_List*_tmp47A=_cycalloc(sizeof(*_tmp47A));
_tmp47A->hd=Cyc_Toc_make_dle(Cyc_Absyn_address_exp(Cyc_Absyn_var_exp(*((struct
_tuple1**)_check_dyneither_subscript(vs,sizeof(struct _tuple1*),i)),0),0));
_tmp47A->tl=_tmp479;_tmp47A;});}}s=Cyc_Absyn_declare_stmt(argv,arr_type,(struct
Cyc_Absyn_Exp*)Cyc_Absyn_unresolvedmem_exp(0,_tmp479,0),s,0);{int i=0;for(0;
_tmp3E4 != 0;(((_tmp3E4=_tmp3E4->tl,_tmp3E8=_tmp3E8->tl)),++ i)){struct Cyc_Absyn_Exp*
arg=(struct Cyc_Absyn_Exp*)_tmp3E4->hd;void*arg_type=(void*)((struct Cyc_Core_Opt*)
_check_null(arg->topt))->v;struct _tuple1*var=*((struct _tuple1**)
_check_dyneither_subscript(vs,sizeof(struct _tuple1*),i));struct Cyc_Absyn_Exp*
varexp=Cyc_Absyn_var_exp(var,0);struct Cyc_Absyn_Tunionfield _tmp47C;struct _tuple1*
_tmp47D;struct Cyc_List_List*_tmp47E;struct Cyc_Absyn_Tunionfield*_tmp47B=(struct
Cyc_Absyn_Tunionfield*)((struct Cyc_List_List*)_check_null(_tmp3E8))->hd;_tmp47C=*
_tmp47B;_tmp47D=_tmp47C.name;_tmp47E=_tmp47C.typs;{void*field_typ=Cyc_Toc_typ_to_c((*((
struct _tuple4*)((struct Cyc_List_List*)_check_null(_tmp47E))->hd)).f2);Cyc_Toc_exp_to_c(
nv,arg);if(Cyc_Toc_is_void_star(field_typ))arg=Cyc_Toc_cast_it(field_typ,arg);s=
Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_aggrmember_exp(varexp,Cyc_Absyn_fieldname(
1),0),arg,0),s,0);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_aggrmember_exp(
varexp,Cyc_Toc_tag_sp,0),Cyc_Toc_tunion_tag(tud,_tmp47D,1),0),s,0);s=Cyc_Absyn_declare_stmt(
var,Cyc_Absyn_strctq(Cyc_Toc_collapse_qvar_tag(_tmp47D,({const char*_tmp47F="_struct";
_tag_dyneither(_tmp47F,sizeof(char),8);}))),0,s,0);}}}}else{struct Cyc_List_List*
_tmp480=({struct _tuple8*_tmp481[3];_tmp481[2]=Cyc_Toc_make_dle(Cyc_Absyn_uint_exp(
0,0));_tmp481[1]=Cyc_Toc_make_dle(Cyc_Absyn_uint_exp(0,0));_tmp481[0]=Cyc_Toc_make_dle(
Cyc_Absyn_uint_exp(0,0));((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmp481,sizeof(struct _tuple8*),3));});s=Cyc_Absyn_declare_stmt(
argv,Cyc_Absyn_void_star_typ(),(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(0,0),s,0);}}}
else{{int i=0;for(0;_tmp3E4 != 0;(_tmp3E4=_tmp3E4->tl,++ i)){Cyc_Toc_exp_to_c(nv,(
struct Cyc_Absyn_Exp*)_tmp3E4->hd);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(
argvexp,Cyc_Absyn_uint_exp((unsigned int)i,0),0),(struct Cyc_Absyn_Exp*)_tmp3E4->hd,
0),s,0);}}s=Cyc_Absyn_declare_stmt(argv,arr_type,0,s,0);}(void*)(e->r=(void*)Cyc_Toc_stmt_exp_r(
s));}}goto _LL1E5;}_LL202: if(*((int*)_tmp3C9)!= 12)goto _LL204;_tmp3EA=((struct Cyc_Absyn_Throw_e_struct*)
_tmp3C9)->f1;_LL203: Cyc_Toc_exp_to_c(nv,_tmp3EA);(void*)(e->r=(void*)((void*)(
Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c(old_typ),Cyc_Toc_newthrow_exp(_tmp3EA),
0))->r));goto _LL1E5;_LL204: if(*((int*)_tmp3C9)!= 13)goto _LL206;_tmp3EB=((struct
Cyc_Absyn_NoInstantiate_e_struct*)_tmp3C9)->f1;_LL205: Cyc_Toc_exp_to_c(nv,
_tmp3EB);goto _LL1E5;_LL206: if(*((int*)_tmp3C9)!= 14)goto _LL208;_tmp3EC=((struct
Cyc_Absyn_Instantiate_e_struct*)_tmp3C9)->f1;_tmp3ED=((struct Cyc_Absyn_Instantiate_e_struct*)
_tmp3C9)->f2;_LL207: Cyc_Toc_exp_to_c(nv,_tmp3EC);for(0;_tmp3ED != 0;_tmp3ED=
_tmp3ED->tl){void*k=Cyc_Tcutil_typ_kind((void*)_tmp3ED->hd);if(((k != (void*)6
 && k != (void*)3) && k != (void*)4) && k != (void*)5){{void*_tmp486=Cyc_Tcutil_compress((
void*)_tmp3ED->hd);_LL297: if(_tmp486 <= (void*)4)goto _LL29B;if(*((int*)_tmp486)!= 
1)goto _LL299;_LL298: goto _LL29A;_LL299: if(*((int*)_tmp486)!= 2)goto _LL29B;_LL29A:
continue;_LL29B:;_LL29C:(void*)(e->r=(void*)((void*)(Cyc_Toc_array_to_ptr_cast(
Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),_tmp3EC,
0))->r));goto _LL296;_LL296:;}break;}}goto _LL1E5;_LL208: if(*((int*)_tmp3C9)!= 15)
goto _LL20A;_tmp3EE=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp3C9)->f1;_tmp3EF=(
void**)&((void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp3C9)->f1);_tmp3F0=((struct
Cyc_Absyn_Cast_e_struct*)_tmp3C9)->f2;_tmp3F1=((struct Cyc_Absyn_Cast_e_struct*)
_tmp3C9)->f3;_tmp3F2=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp3C9)->f4;_LL209: {
void*old_t2=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp3F0->topt))->v;void*
new_typ=*_tmp3EF;*_tmp3EF=Cyc_Toc_typ_to_c(new_typ);Cyc_Toc_exp_to_c(nv,_tmp3F0);{
struct _tuple0 _tmp488=({struct _tuple0 _tmp487;_tmp487.f1=Cyc_Tcutil_compress(
old_t2);_tmp487.f2=Cyc_Tcutil_compress(new_typ);_tmp487;});void*_tmp489;struct
Cyc_Absyn_PtrInfo _tmp48A;void*_tmp48B;struct Cyc_Absyn_PtrInfo _tmp48C;void*
_tmp48D;struct Cyc_Absyn_PtrInfo _tmp48E;void*_tmp48F;_LL29E: _tmp489=_tmp488.f1;
if(_tmp489 <= (void*)4)goto _LL2A0;if(*((int*)_tmp489)!= 4)goto _LL2A0;_tmp48A=((
struct Cyc_Absyn_PointerType_struct*)_tmp489)->f1;_tmp48B=_tmp488.f2;if(_tmp48B <= (
void*)4)goto _LL2A0;if(*((int*)_tmp48B)!= 4)goto _LL2A0;_tmp48C=((struct Cyc_Absyn_PointerType_struct*)
_tmp48B)->f1;_LL29F: {int _tmp490=((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,(_tmp48A.ptr_atts).nullable);int _tmp491=((int(*)(int,struct Cyc_Absyn_Conref*x))
Cyc_Absyn_conref_def)(0,(_tmp48C.ptr_atts).nullable);void*_tmp492=Cyc_Absyn_conref_def(
Cyc_Absyn_bounds_one,(_tmp48A.ptr_atts).bounds);void*_tmp493=Cyc_Absyn_conref_def(
Cyc_Absyn_bounds_one,(_tmp48C.ptr_atts).bounds);int _tmp494=((int(*)(int,struct
Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,(_tmp48A.ptr_atts).zero_term);int
_tmp495=((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,(_tmp48C.ptr_atts).zero_term);{
struct _tuple0 _tmp497=({struct _tuple0 _tmp496;_tmp496.f1=_tmp492;_tmp496.f2=
_tmp493;_tmp496;});void*_tmp498;struct Cyc_Absyn_Exp*_tmp499;void*_tmp49A;struct
Cyc_Absyn_Exp*_tmp49B;void*_tmp49C;struct Cyc_Absyn_Exp*_tmp49D;void*_tmp49E;void*
_tmp49F;void*_tmp4A0;struct Cyc_Absyn_Exp*_tmp4A1;void*_tmp4A2;void*_tmp4A3;
_LL2A5: _tmp498=_tmp497.f1;if(_tmp498 <= (void*)1)goto _LL2A7;if(*((int*)_tmp498)!= 
0)goto _LL2A7;_tmp499=((struct Cyc_Absyn_Upper_b_struct*)_tmp498)->f1;_tmp49A=
_tmp497.f2;if(_tmp49A <= (void*)1)goto _LL2A7;if(*((int*)_tmp49A)!= 0)goto _LL2A7;
_tmp49B=((struct Cyc_Absyn_Upper_b_struct*)_tmp49A)->f1;_LL2A6: if((!Cyc_Evexp_c_can_eval(
_tmp499) || !Cyc_Evexp_c_can_eval(_tmp49B)) && !Cyc_Evexp_same_const_exp(_tmp499,
_tmp49B))({void*_tmp4A4=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp4A5="can't validate cast due to potential size differences";
_tag_dyneither(_tmp4A5,sizeof(char),54);}),_tag_dyneither(_tmp4A4,sizeof(void*),
0));});if(_tmp490  && !_tmp491){if(Cyc_Toc_is_toplevel(nv))({void*_tmp4A6=0;((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(({const char*
_tmp4A7="can't do NULL-check conversion at top-level";_tag_dyneither(_tmp4A7,
sizeof(char),44);}),_tag_dyneither(_tmp4A6,sizeof(void*),0));});if(_tmp3F2 != (
void*)2)({struct Cyc_String_pa_struct _tmp4AA;_tmp4AA.tag=0;_tmp4AA.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));{void*_tmp4A8[1]={&
_tmp4AA};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp4A9="null-check conversion mis-classified: %s";_tag_dyneither(
_tmp4A9,sizeof(char),41);}),_tag_dyneither(_tmp4A8,sizeof(void*),1));}});{int
do_null_check=Cyc_Toc_need_null_check(_tmp3F0);if(do_null_check){if(!_tmp3F1)({
void*_tmp4AB=0;Cyc_Tcutil_warn(e->loc,({const char*_tmp4AC="inserted null check due to implicit cast from * to @ type";
_tag_dyneither(_tmp4AC,sizeof(char),58);}),_tag_dyneither(_tmp4AB,sizeof(void*),
0));});(void*)(e->r=(void*)Cyc_Toc_cast_it_r(*_tmp3EF,Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,({
struct Cyc_List_List*_tmp4AD=_cycalloc(sizeof(*_tmp4AD));_tmp4AD->hd=_tmp3F0;
_tmp4AD->tl=0;_tmp4AD;}),0)));}}}goto _LL2A4;_LL2A7: _tmp49C=_tmp497.f1;if(_tmp49C
<= (void*)1)goto _LL2A9;if(*((int*)_tmp49C)!= 0)goto _LL2A9;_tmp49D=((struct Cyc_Absyn_Upper_b_struct*)
_tmp49C)->f1;_tmp49E=_tmp497.f2;if((int)_tmp49E != 0)goto _LL2A9;_LL2A8: if(!Cyc_Evexp_c_can_eval(
_tmp49D))({void*_tmp4AE=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp4AF="cannot perform coercion since numelts cannot be determined statically.";
_tag_dyneither(_tmp4AF,sizeof(char),71);}),_tag_dyneither(_tmp4AE,sizeof(void*),
0));});if(_tmp3F2 == (void*)2)({struct Cyc_String_pa_struct _tmp4B2;_tmp4B2.tag=0;
_tmp4B2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));{
void*_tmp4B0[1]={& _tmp4B2};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap))Cyc_Tcutil_impos)(({const char*_tmp4B1="conversion mis-classified as null-check: %s";
_tag_dyneither(_tmp4B1,sizeof(char),44);}),_tag_dyneither(_tmp4B0,sizeof(void*),
1));}});if(Cyc_Toc_is_toplevel(nv)){if((_tmp494  && !(_tmp48C.elt_tq).real_const)
 && !_tmp495)_tmp49D=Cyc_Absyn_prim2_exp((void*)2,_tmp49D,Cyc_Absyn_uint_exp(1,0),
0);(void*)(e->r=(void*)((void*)(Cyc_Toc_make_toplevel_dyn_arr(old_t2,_tmp49D,
_tmp3F0))->r));}else{struct Cyc_Absyn_Exp*_tmp4B3=Cyc_Toc__tag_dyneither_e;if(
_tmp494){struct _tuple1*_tmp4B4=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp4B5=
Cyc_Absyn_var_exp(_tmp4B4,0);struct Cyc_Absyn_Exp*arg3;{void*_tmp4B6=(void*)
_tmp3F0->r;union Cyc_Absyn_Cnst_union _tmp4B7;_LL2AE: if(*((int*)_tmp4B6)!= 0)goto
_LL2B0;_tmp4B7=((struct Cyc_Absyn_Const_e_struct*)_tmp4B6)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)
_tmp4B6)->f1).String_c).tag != 5)goto _LL2B0;_LL2AF: arg3=_tmp49D;goto _LL2AD;_LL2B0:;
_LL2B1: arg3=Cyc_Absyn_fncall_exp(Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__get_zero_arr_size_functionSet,
_tmp3F0),({struct Cyc_Absyn_Exp*_tmp4B8[2];_tmp4B8[1]=_tmp49D;_tmp4B8[0]=_tmp4B5;((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmp4B8,sizeof(struct Cyc_Absyn_Exp*),2));}),0);goto _LL2AD;_LL2AD:;}if(!_tmp495
 && !(_tmp48C.elt_tq).real_const)arg3=Cyc_Absyn_prim2_exp((void*)2,arg3,Cyc_Absyn_uint_exp(
1,0),0);{struct Cyc_Absyn_Exp*_tmp4B9=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c((
void*)_tmp48C.elt_typ),0);struct Cyc_Absyn_Exp*_tmp4BA=Cyc_Absyn_fncall_exp(
_tmp4B3,({struct Cyc_Absyn_Exp*_tmp4BC[3];_tmp4BC[2]=arg3;_tmp4BC[1]=_tmp4B9;
_tmp4BC[0]=_tmp4B5;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmp4BC,sizeof(struct Cyc_Absyn_Exp*),3));}),0);struct Cyc_Absyn_Stmt*
_tmp4BB=Cyc_Absyn_exp_stmt(_tmp4BA,0);_tmp4BB=Cyc_Absyn_declare_stmt(_tmp4B4,Cyc_Toc_typ_to_c(
old_t2),(struct Cyc_Absyn_Exp*)_tmp3F0,_tmp4BB,0);(void*)(e->r=(void*)Cyc_Toc_stmt_exp_r(
_tmp4BB));}}else{(void*)(e->r=(void*)Cyc_Toc_fncall_exp_r(_tmp4B3,({struct Cyc_Absyn_Exp*
_tmp4BD[3];_tmp4BD[2]=_tmp49D;_tmp4BD[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c((
void*)_tmp48C.elt_typ),0);_tmp4BD[0]=_tmp3F0;((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp4BD,sizeof(struct Cyc_Absyn_Exp*),
3));})));}}goto _LL2A4;_LL2A9: _tmp49F=_tmp497.f1;if((int)_tmp49F != 0)goto _LL2AB;
_tmp4A0=_tmp497.f2;if(_tmp4A0 <= (void*)1)goto _LL2AB;if(*((int*)_tmp4A0)!= 0)goto
_LL2AB;_tmp4A1=((struct Cyc_Absyn_Upper_b_struct*)_tmp4A0)->f1;_LL2AA: if(!Cyc_Evexp_c_can_eval(
_tmp4A1))({void*_tmp4BE=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp4BF="cannot perform coercion since numelts cannot be determined statically.";
_tag_dyneither(_tmp4BF,sizeof(char),71);}),_tag_dyneither(_tmp4BE,sizeof(void*),
0));});if(Cyc_Toc_is_toplevel(nv))({void*_tmp4C0=0;((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(({const char*_tmp4C1="can't coerce t? to t* or t@ at the top-level";
_tag_dyneither(_tmp4C1,sizeof(char),45);}),_tag_dyneither(_tmp4C0,sizeof(void*),
0));});{struct Cyc_Absyn_Exp*_tmp4C2=_tmp4A1;if(_tmp494  && !_tmp495)_tmp4C2=Cyc_Absyn_add_exp(
_tmp4A1,Cyc_Absyn_uint_exp(1,0),0);{struct Cyc_Absyn_Exp*_tmp4C3=Cyc_Toc__untag_dyneither_ptr_e;
struct Cyc_Absyn_Exp*_tmp4C4=Cyc_Absyn_fncall_exp(_tmp4C3,({struct Cyc_Absyn_Exp*
_tmp4C6[3];_tmp4C6[2]=_tmp4C2;_tmp4C6[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c((
void*)_tmp48A.elt_typ),0);_tmp4C6[0]=_tmp3F0;((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp4C6,sizeof(struct Cyc_Absyn_Exp*),
3));}),0);if(_tmp491)(void*)(_tmp4C4->r=(void*)Cyc_Toc_fncall_exp_r(Cyc_Toc__check_null_e,({
struct Cyc_List_List*_tmp4C5=_cycalloc(sizeof(*_tmp4C5));_tmp4C5->hd=Cyc_Absyn_copy_exp(
_tmp4C4);_tmp4C5->tl=0;_tmp4C5;})));(void*)(e->r=(void*)Cyc_Toc_cast_it_r(*
_tmp3EF,_tmp4C4));goto _LL2A4;}}_LL2AB: _tmp4A2=_tmp497.f1;if((int)_tmp4A2 != 0)
goto _LL2A4;_tmp4A3=_tmp497.f2;if((int)_tmp4A3 != 0)goto _LL2A4;_LL2AC: DynCast: if((
_tmp494  && !_tmp495) && !(_tmp48C.elt_tq).real_const){if(Cyc_Toc_is_toplevel(nv))({
void*_tmp4C7=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(({
const char*_tmp4C8="can't coerce a ZEROTERM to a non-const NOZEROTERM pointer at toplevel";
_tag_dyneither(_tmp4C8,sizeof(char),70);}),_tag_dyneither(_tmp4C7,sizeof(void*),
0));});{struct Cyc_Absyn_Exp*_tmp4C9=Cyc_Toc__dyneither_ptr_decrease_size_e;(void*)(
e->r=(void*)Cyc_Toc_fncall_exp_r(_tmp4C9,({struct Cyc_Absyn_Exp*_tmp4CA[3];
_tmp4CA[2]=Cyc_Absyn_uint_exp(1,0);_tmp4CA[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c((
void*)_tmp48A.elt_typ),0);_tmp4CA[0]=_tmp3F0;((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp4CA,sizeof(struct Cyc_Absyn_Exp*),
3));})));}}goto _LL2A4;_LL2A4:;}goto _LL29D;}_LL2A0: _tmp48D=_tmp488.f1;if(_tmp48D
<= (void*)4)goto _LL2A2;if(*((int*)_tmp48D)!= 4)goto _LL2A2;_tmp48E=((struct Cyc_Absyn_PointerType_struct*)
_tmp48D)->f1;_tmp48F=_tmp488.f2;if(_tmp48F <= (void*)4)goto _LL2A2;if(*((int*)
_tmp48F)!= 5)goto _LL2A2;_LL2A1:{void*_tmp4CB=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,(
_tmp48E.ptr_atts).bounds);_LL2B3: if((int)_tmp4CB != 0)goto _LL2B5;_LL2B4:(void*)(
_tmp3F0->r=(void*)Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp((void*)_tmp3F0->r,
_tmp3F0->loc),Cyc_Toc_curr_sp));goto _LL2B2;_LL2B5:;_LL2B6: goto _LL2B2;_LL2B2:;}
goto _LL29D;_LL2A2:;_LL2A3: goto _LL29D;_LL29D:;}goto _LL1E5;}_LL20A: if(*((int*)
_tmp3C9)!= 16)goto _LL20C;_tmp3F3=((struct Cyc_Absyn_Address_e_struct*)_tmp3C9)->f1;
_LL20B:{void*_tmp4CC=(void*)_tmp3F3->r;struct _tuple1*_tmp4CD;struct Cyc_List_List*
_tmp4CE;struct Cyc_List_List*_tmp4CF;struct Cyc_List_List*_tmp4D0;_LL2B8: if(*((int*)
_tmp4CC)!= 30)goto _LL2BA;_tmp4CD=((struct Cyc_Absyn_Struct_e_struct*)_tmp4CC)->f1;
_tmp4CE=((struct Cyc_Absyn_Struct_e_struct*)_tmp4CC)->f2;_tmp4CF=((struct Cyc_Absyn_Struct_e_struct*)
_tmp4CC)->f3;_LL2B9: if(Cyc_Toc_is_toplevel(nv))({struct Cyc_String_pa_struct
_tmp4D3;_tmp4D3.tag=0;_tmp4D3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Position_string_of_segment(_tmp3F3->loc));{void*_tmp4D1[1]={& _tmp4D3};((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(({const char*
_tmp4D2="%s: & on non-identifiers at the top-level";_tag_dyneither(_tmp4D2,
sizeof(char),42);}),_tag_dyneither(_tmp4D1,sizeof(void*),1));}});(void*)(e->r=(
void*)((void*)(Cyc_Toc_init_struct(nv,(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp3F3->topt))->v,_tmp4CE != 0,1,0,_tmp4CF,_tmp4CD))->r));goto _LL2B7;_LL2BA: if(*((
int*)_tmp4CC)!= 26)goto _LL2BC;_tmp4D0=((struct Cyc_Absyn_Tuple_e_struct*)_tmp4CC)->f1;
_LL2BB: if(Cyc_Toc_is_toplevel(nv))({struct Cyc_String_pa_struct _tmp4D6;_tmp4D6.tag=
0;_tmp4D6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Position_string_of_segment(
_tmp3F3->loc));{void*_tmp4D4[1]={& _tmp4D6};((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_unimp)(({const char*_tmp4D5="%s: & on non-identifiers at the top-level";
_tag_dyneither(_tmp4D5,sizeof(char),42);}),_tag_dyneither(_tmp4D4,sizeof(void*),
1));}});(void*)(e->r=(void*)((void*)(Cyc_Toc_init_tuple(nv,1,0,_tmp4D0))->r));
goto _LL2B7;_LL2BC:;_LL2BD: Cyc_Toc_exp_to_c(nv,_tmp3F3);if(!Cyc_Absyn_is_lvalue(
_tmp3F3)){((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*
f)(struct Cyc_Absyn_Exp*,int),int f_env))Cyc_Toc_lvalue_assign)(_tmp3F3,0,Cyc_Toc_address_lvalue,
1);(void*)(e->r=(void*)Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v),_tmp3F3));}goto _LL2B7;_LL2B7:;}goto _LL1E5;_LL20C: if(*((
int*)_tmp3C9)!= 17)goto _LL20E;_tmp3F4=((struct Cyc_Absyn_New_e_struct*)_tmp3C9)->f1;
_tmp3F5=((struct Cyc_Absyn_New_e_struct*)_tmp3C9)->f2;_LL20D: if(Cyc_Toc_is_toplevel(
nv))({struct Cyc_String_pa_struct _tmp4D9;_tmp4D9.tag=0;_tmp4D9.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Position_string_of_segment(_tmp3F5->loc));{
void*_tmp4D7[1]={& _tmp4D9};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap))Cyc_Toc_unimp)(({const char*_tmp4D8="%s: new at top-level";_tag_dyneither(
_tmp4D8,sizeof(char),21);}),_tag_dyneither(_tmp4D7,sizeof(void*),1));}});{void*
_tmp4DA=(void*)_tmp3F5->r;struct Cyc_List_List*_tmp4DB;struct Cyc_Absyn_Vardecl*
_tmp4DC;struct Cyc_Absyn_Exp*_tmp4DD;struct Cyc_Absyn_Exp*_tmp4DE;int _tmp4DF;
struct _tuple1*_tmp4E0;struct Cyc_List_List*_tmp4E1;struct Cyc_List_List*_tmp4E2;
struct Cyc_List_List*_tmp4E3;_LL2BF: if(*((int*)_tmp4DA)!= 28)goto _LL2C1;_tmp4DB=((
struct Cyc_Absyn_Array_e_struct*)_tmp4DA)->f1;_LL2C0: {struct _tuple1*_tmp4E4=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp4E5=Cyc_Absyn_var_exp(_tmp4E4,0);struct Cyc_Absyn_Stmt*
_tmp4E6=Cyc_Toc_init_array(nv,_tmp4E5,_tmp4DB,Cyc_Absyn_exp_stmt(_tmp4E5,0));
void*old_elt_typ;{void*_tmp4E7=Cyc_Tcutil_compress(old_typ);struct Cyc_Absyn_PtrInfo
_tmp4E8;void*_tmp4E9;struct Cyc_Absyn_Tqual _tmp4EA;struct Cyc_Absyn_PtrAtts _tmp4EB;
struct Cyc_Absyn_Conref*_tmp4EC;_LL2CA: if(_tmp4E7 <= (void*)4)goto _LL2CC;if(*((int*)
_tmp4E7)!= 4)goto _LL2CC;_tmp4E8=((struct Cyc_Absyn_PointerType_struct*)_tmp4E7)->f1;
_tmp4E9=(void*)_tmp4E8.elt_typ;_tmp4EA=_tmp4E8.elt_tq;_tmp4EB=_tmp4E8.ptr_atts;
_tmp4EC=_tmp4EB.zero_term;_LL2CB: old_elt_typ=_tmp4E9;goto _LL2C9;_LL2CC:;_LL2CD:
old_elt_typ=({void*_tmp4ED=0;Cyc_Toc_toc_impos(({const char*_tmp4EE="exp_to_c:new array expression doesn't have ptr type";
_tag_dyneither(_tmp4EE,sizeof(char),52);}),_tag_dyneither(_tmp4ED,sizeof(void*),
0));});_LL2C9:;}{void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);void*_tmp4EF=Cyc_Absyn_cstar_typ(
elt_typ,Cyc_Toc_mt_tq);struct Cyc_Absyn_Exp*_tmp4F0=Cyc_Absyn_times_exp(Cyc_Absyn_sizeoftyp_exp(
elt_typ,0),Cyc_Absyn_signed_int_exp(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp4DB),0),0);struct Cyc_Absyn_Exp*e1;if(_tmp3F4 == 0  || Cyc_Absyn_no_regions)e1=
Cyc_Toc_malloc_exp(old_elt_typ,_tmp4F0);else{struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)
_tmp3F4;Cyc_Toc_exp_to_c(nv,r);e1=Cyc_Toc_rmalloc_exp(r,_tmp4F0);}(void*)(e->r=(
void*)Cyc_Toc_stmt_exp_r(Cyc_Absyn_declare_stmt(_tmp4E4,_tmp4EF,(struct Cyc_Absyn_Exp*)
e1,_tmp4E6,0)));goto _LL2BE;}}_LL2C1: if(*((int*)_tmp4DA)!= 29)goto _LL2C3;_tmp4DC=((
struct Cyc_Absyn_Comprehension_e_struct*)_tmp4DA)->f1;_tmp4DD=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp4DA)->f2;_tmp4DE=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp4DA)->f3;
_tmp4DF=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp4DA)->f4;_LL2C2: {int
is_dyneither_ptr=0;{void*_tmp4F1=Cyc_Tcutil_compress(old_typ);struct Cyc_Absyn_PtrInfo
_tmp4F2;void*_tmp4F3;struct Cyc_Absyn_Tqual _tmp4F4;struct Cyc_Absyn_PtrAtts _tmp4F5;
struct Cyc_Absyn_Conref*_tmp4F6;struct Cyc_Absyn_Conref*_tmp4F7;_LL2CF: if(_tmp4F1
<= (void*)4)goto _LL2D1;if(*((int*)_tmp4F1)!= 4)goto _LL2D1;_tmp4F2=((struct Cyc_Absyn_PointerType_struct*)
_tmp4F1)->f1;_tmp4F3=(void*)_tmp4F2.elt_typ;_tmp4F4=_tmp4F2.elt_tq;_tmp4F5=
_tmp4F2.ptr_atts;_tmp4F6=_tmp4F5.bounds;_tmp4F7=_tmp4F5.zero_term;_LL2D0:
is_dyneither_ptr=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp4F6)== (void*)0;
goto _LL2CE;_LL2D1:;_LL2D2:({void*_tmp4F8=0;((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp4F9="exp_to_c: comprehension not an array type";
_tag_dyneither(_tmp4F9,sizeof(char),42);}),_tag_dyneither(_tmp4F8,sizeof(void*),
0));});_LL2CE:;}{struct _tuple1*max=Cyc_Toc_temp_var();struct _tuple1*a=Cyc_Toc_temp_var();
void*old_elt_typ=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp4DE->topt))->v;
void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);void*ptr_typ=Cyc_Absyn_cstar_typ(
elt_typ,Cyc_Toc_mt_tq);Cyc_Toc_exp_to_c(nv,_tmp4DD);{struct Cyc_Absyn_Exp*_tmp4FA=
Cyc_Absyn_var_exp(max,0);if(_tmp4DF)_tmp4FA=Cyc_Absyn_add_exp(_tmp4FA,Cyc_Absyn_uint_exp(
1,0),0);{struct Cyc_Absyn_Stmt*s=Cyc_Toc_init_comprehension(nv,Cyc_Absyn_var_exp(
a,0),_tmp4DC,Cyc_Absyn_var_exp(max,0),_tmp4DE,_tmp4DF,Cyc_Toc_skip_stmt_dl(),1);
struct Cyc_Toc_Env _tmp4FC;struct _RegionHandle*_tmp4FD;struct Cyc_Toc_Env*_tmp4FB=
nv;_tmp4FC=*_tmp4FB;_tmp4FD=_tmp4FC.rgn;{struct Cyc_List_List*decls=({struct Cyc_List_List*
_tmp50E=_region_malloc(_tmp4FD,sizeof(*_tmp50E));_tmp50E->hd=({struct _tuple11*
_tmp50F=_region_malloc(_tmp4FD,sizeof(*_tmp50F));_tmp50F->f1=max;_tmp50F->f2=Cyc_Absyn_uint_typ;
_tmp50F->f3=(struct Cyc_Absyn_Exp*)_tmp4DD;_tmp50F;});_tmp50E->tl=0;_tmp50E;});
struct Cyc_Absyn_Exp*ai;if(_tmp3F4 == 0  || Cyc_Absyn_no_regions)ai=Cyc_Toc_malloc_exp(
old_elt_typ,Cyc_Absyn_fncall_exp(Cyc_Toc__check_times_e,({struct Cyc_Absyn_Exp*
_tmp4FE[2];_tmp4FE[1]=_tmp4FA;_tmp4FE[0]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0);((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmp4FE,sizeof(struct Cyc_Absyn_Exp*),2));}),0));else{struct Cyc_Absyn_Exp*r=(
struct Cyc_Absyn_Exp*)_tmp3F4;Cyc_Toc_exp_to_c(nv,r);ai=Cyc_Toc_rmalloc_exp(r,Cyc_Absyn_fncall_exp(
Cyc_Toc__check_times_e,({struct Cyc_Absyn_Exp*_tmp4FF[2];_tmp4FF[1]=_tmp4FA;
_tmp4FF[0]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0);((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp4FF,sizeof(struct Cyc_Absyn_Exp*),
2));}),0));}{struct Cyc_Absyn_Exp*ainit=Cyc_Toc_cast_it(ptr_typ,ai);decls=({
struct Cyc_List_List*_tmp500=_region_malloc(_tmp4FD,sizeof(*_tmp500));_tmp500->hd=({
struct _tuple11*_tmp501=_region_malloc(_tmp4FD,sizeof(*_tmp501));_tmp501->f1=a;
_tmp501->f2=ptr_typ;_tmp501->f3=(struct Cyc_Absyn_Exp*)ainit;_tmp501;});_tmp500->tl=
decls;_tmp500;});if(is_dyneither_ptr){struct _tuple1*_tmp502=Cyc_Toc_temp_var();
void*_tmp503=Cyc_Toc_typ_to_c(old_typ);struct Cyc_Absyn_Exp*_tmp504=Cyc_Toc__tag_dyneither_e;
struct Cyc_Absyn_Exp*_tmp505=Cyc_Absyn_fncall_exp(_tmp504,({struct Cyc_Absyn_Exp*
_tmp508[3];_tmp508[2]=_tmp4FA;_tmp508[1]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0);
_tmp508[0]=Cyc_Absyn_var_exp(a,0);((struct Cyc_List_List*(*)(struct _dyneither_ptr))
Cyc_List_list)(_tag_dyneither(_tmp508,sizeof(struct Cyc_Absyn_Exp*),3));}),0);
decls=({struct Cyc_List_List*_tmp506=_region_malloc(_tmp4FD,sizeof(*_tmp506));
_tmp506->hd=({struct _tuple11*_tmp507=_region_malloc(_tmp4FD,sizeof(*_tmp507));
_tmp507->f1=_tmp502;_tmp507->f2=_tmp503;_tmp507->f3=(struct Cyc_Absyn_Exp*)
_tmp505;_tmp507;});_tmp506->tl=decls;_tmp506;});s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(
Cyc_Absyn_var_exp(_tmp502,0),0),0);}else{s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(
Cyc_Absyn_var_exp(a,0),0),0);}{struct Cyc_List_List*_tmp509=decls;for(0;_tmp509 != 
0;_tmp509=_tmp509->tl){struct _tuple1*_tmp50B;void*_tmp50C;struct Cyc_Absyn_Exp*
_tmp50D;struct _tuple11 _tmp50A=*((struct _tuple11*)_tmp509->hd);_tmp50B=_tmp50A.f1;
_tmp50C=_tmp50A.f2;_tmp50D=_tmp50A.f3;s=Cyc_Absyn_declare_stmt(_tmp50B,_tmp50C,
_tmp50D,s,0);}}(void*)(e->r=(void*)Cyc_Toc_stmt_exp_r(s));}}goto _LL2BE;}}}}
_LL2C3: if(*((int*)_tmp4DA)!= 30)goto _LL2C5;_tmp4E0=((struct Cyc_Absyn_Struct_e_struct*)
_tmp4DA)->f1;_tmp4E1=((struct Cyc_Absyn_Struct_e_struct*)_tmp4DA)->f2;_tmp4E2=((
struct Cyc_Absyn_Struct_e_struct*)_tmp4DA)->f3;_LL2C4:(void*)(e->r=(void*)((void*)(
Cyc_Toc_init_struct(nv,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp3F5->topt))->v,
_tmp4E1 != 0,1,_tmp3F4,_tmp4E2,_tmp4E0))->r));goto _LL2BE;_LL2C5: if(*((int*)
_tmp4DA)!= 26)goto _LL2C7;_tmp4E3=((struct Cyc_Absyn_Tuple_e_struct*)_tmp4DA)->f1;
_LL2C6:(void*)(e->r=(void*)((void*)(Cyc_Toc_init_tuple(nv,1,_tmp3F4,_tmp4E3))->r));
goto _LL2BE;_LL2C7:;_LL2C8: {void*old_elt_typ=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp3F5->topt))->v;void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);struct
_tuple1*_tmp510=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp511=Cyc_Absyn_var_exp(
_tmp510,0);struct Cyc_Absyn_Exp*mexp=Cyc_Absyn_sizeofexp_exp(Cyc_Absyn_deref_exp(
_tmp511,0),0);struct Cyc_Absyn_Exp*inner_mexp=mexp;if(_tmp3F4 == 0  || Cyc_Absyn_no_regions)
mexp=Cyc_Toc_malloc_exp(old_elt_typ,mexp);else{struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)
_tmp3F4;Cyc_Toc_exp_to_c(nv,r);mexp=Cyc_Toc_rmalloc_exp(r,mexp);}{int done=0;{
void*_tmp512=(void*)_tmp3F5->r;void*_tmp513;struct Cyc_Absyn_Exp*_tmp514;_LL2D4:
if(*((int*)_tmp512)!= 15)goto _LL2D6;_tmp513=(void*)((struct Cyc_Absyn_Cast_e_struct*)
_tmp512)->f1;_tmp514=((struct Cyc_Absyn_Cast_e_struct*)_tmp512)->f2;_LL2D5:{
struct _tuple0 _tmp516=({struct _tuple0 _tmp515;_tmp515.f1=Cyc_Tcutil_compress(
_tmp513);_tmp515.f2=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp514->topt))->v);_tmp515;});void*_tmp517;struct Cyc_Absyn_PtrInfo _tmp518;void*
_tmp519;struct Cyc_Absyn_PtrAtts _tmp51A;struct Cyc_Absyn_Conref*_tmp51B;void*
_tmp51C;struct Cyc_Absyn_PtrInfo _tmp51D;struct Cyc_Absyn_PtrAtts _tmp51E;struct Cyc_Absyn_Conref*
_tmp51F;_LL2D9: _tmp517=_tmp516.f1;if(_tmp517 <= (void*)4)goto _LL2DB;if(*((int*)
_tmp517)!= 4)goto _LL2DB;_tmp518=((struct Cyc_Absyn_PointerType_struct*)_tmp517)->f1;
_tmp519=(void*)_tmp518.elt_typ;_tmp51A=_tmp518.ptr_atts;_tmp51B=_tmp51A.bounds;
_tmp51C=_tmp516.f2;if(_tmp51C <= (void*)4)goto _LL2DB;if(*((int*)_tmp51C)!= 4)goto
_LL2DB;_tmp51D=((struct Cyc_Absyn_PointerType_struct*)_tmp51C)->f1;_tmp51E=
_tmp51D.ptr_atts;_tmp51F=_tmp51E.bounds;_LL2DA:{struct _tuple0 _tmp521=({struct
_tuple0 _tmp520;_tmp520.f1=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp51B);
_tmp520.f2=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp51F);_tmp520;});void*
_tmp522;void*_tmp523;struct Cyc_Absyn_Exp*_tmp524;_LL2DE: _tmp522=_tmp521.f1;if((
int)_tmp522 != 0)goto _LL2E0;_tmp523=_tmp521.f2;if(_tmp523 <= (void*)1)goto _LL2E0;
if(*((int*)_tmp523)!= 0)goto _LL2E0;_tmp524=((struct Cyc_Absyn_Upper_b_struct*)
_tmp523)->f1;_LL2DF: Cyc_Toc_exp_to_c(nv,_tmp514);(void*)(inner_mexp->r=(void*)
Cyc_Toc_sizeoftyp_exp_r(elt_typ));done=1;{struct Cyc_Absyn_Exp*_tmp525=Cyc_Toc__init_dyneither_ptr_e;(
void*)(e->r=(void*)Cyc_Toc_fncall_exp_r(_tmp525,({struct Cyc_Absyn_Exp*_tmp526[4];
_tmp526[3]=_tmp524;_tmp526[2]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp519),
0);_tmp526[1]=_tmp514;_tmp526[0]=mexp;((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp526,sizeof(struct Cyc_Absyn_Exp*),
4));})));goto _LL2DD;}_LL2E0:;_LL2E1: goto _LL2DD;_LL2DD:;}goto _LL2D8;_LL2DB:;
_LL2DC: goto _LL2D8;_LL2D8:;}goto _LL2D3;_LL2D6:;_LL2D7: goto _LL2D3;_LL2D3:;}if(!
done){struct Cyc_Absyn_Stmt*_tmp527=Cyc_Absyn_exp_stmt(_tmp511,0);struct Cyc_Absyn_Exp*
_tmp528=Cyc_Absyn_signed_int_exp(0,0);Cyc_Toc_exp_to_c(nv,_tmp3F5);_tmp527=Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(_tmp511,_tmp528,0),_tmp3F5,0),
_tmp527,0);{void*_tmp529=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);(void*)(e->r=(
void*)Cyc_Toc_stmt_exp_r(Cyc_Absyn_declare_stmt(_tmp510,_tmp529,(struct Cyc_Absyn_Exp*)
mexp,_tmp527,0)));}}goto _LL2BE;}}_LL2BE:;}goto _LL1E5;_LL20E: if(*((int*)_tmp3C9)
!= 19)goto _LL210;_tmp3F6=((struct Cyc_Absyn_Sizeofexp_e_struct*)_tmp3C9)->f1;
_LL20F: Cyc_Toc_exp_to_c(nv,_tmp3F6);goto _LL1E5;_LL210: if(*((int*)_tmp3C9)!= 18)
goto _LL212;_tmp3F7=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp3C9)->f1;
_LL211:(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp52A=
_cycalloc(sizeof(*_tmp52A));_tmp52A[0]=({struct Cyc_Absyn_Sizeoftyp_e_struct
_tmp52B;_tmp52B.tag=18;_tmp52B.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp3F7);_tmp52B;});
_tmp52A;})));goto _LL1E5;_LL212: if(*((int*)_tmp3C9)!= 21)goto _LL214;_LL213:({void*
_tmp52C=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp52D="__gen() in code generator";_tag_dyneither(_tmp52D,sizeof(char),
26);}),_tag_dyneither(_tmp52C,sizeof(void*),0));});_LL214: if(*((int*)_tmp3C9)!= 
20)goto _LL216;_tmp3F8=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp3C9)->f1;
_tmp3F9=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp3C9)->f2;if(*((int*)
_tmp3F9)!= 0)goto _LL216;_tmp3FA=((struct Cyc_Absyn_StructField_struct*)_tmp3F9)->f1;
_LL215:(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Offsetof_e_struct*_tmp52E=
_cycalloc(sizeof(*_tmp52E));_tmp52E[0]=({struct Cyc_Absyn_Offsetof_e_struct
_tmp52F;_tmp52F.tag=20;_tmp52F.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp3F8);_tmp52F.f2=(
void*)((void*)({struct Cyc_Absyn_StructField_struct*_tmp530=_cycalloc(sizeof(*
_tmp530));_tmp530[0]=({struct Cyc_Absyn_StructField_struct _tmp531;_tmp531.tag=0;
_tmp531.f1=_tmp3FA;_tmp531;});_tmp530;}));_tmp52F;});_tmp52E;})));goto _LL1E5;
_LL216: if(*((int*)_tmp3C9)!= 20)goto _LL218;_tmp3FB=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)
_tmp3C9)->f1;_tmp3FC=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp3C9)->f2;
if(*((int*)_tmp3FC)!= 1)goto _LL218;_tmp3FD=((struct Cyc_Absyn_TupleIndex_struct*)
_tmp3FC)->f1;_LL217:{void*_tmp532=Cyc_Tcutil_compress(_tmp3FB);struct Cyc_Absyn_AggrInfo
_tmp533;union Cyc_Absyn_AggrInfoU_union _tmp534;struct Cyc_List_List*_tmp535;_LL2E3:
if(_tmp532 <= (void*)4)goto _LL2EB;if(*((int*)_tmp532)!= 10)goto _LL2E5;_tmp533=((
struct Cyc_Absyn_AggrType_struct*)_tmp532)->f1;_tmp534=_tmp533.aggr_info;_LL2E4: {
struct Cyc_Absyn_Aggrdecl*_tmp536=Cyc_Absyn_get_known_aggrdecl(_tmp534);if(
_tmp536->impl == 0)({void*_tmp537=0;((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp538="struct fields must be known";
_tag_dyneither(_tmp538,sizeof(char),28);}),_tag_dyneither(_tmp537,sizeof(void*),
0));});_tmp535=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp536->impl))->fields;
goto _LL2E6;}_LL2E5: if(*((int*)_tmp532)!= 11)goto _LL2E7;_tmp535=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp532)->f2;_LL2E6: {struct Cyc_Absyn_Aggrfield*_tmp539=((struct Cyc_Absyn_Aggrfield*(*)(
struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp535,(int)_tmp3FD);(void*)(e->r=(
void*)((void*)({struct Cyc_Absyn_Offsetof_e_struct*_tmp53A=_cycalloc(sizeof(*
_tmp53A));_tmp53A[0]=({struct Cyc_Absyn_Offsetof_e_struct _tmp53B;_tmp53B.tag=20;
_tmp53B.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp3FB);_tmp53B.f2=(void*)((void*)({
struct Cyc_Absyn_StructField_struct*_tmp53C=_cycalloc(sizeof(*_tmp53C));_tmp53C[0]=({
struct Cyc_Absyn_StructField_struct _tmp53D;_tmp53D.tag=0;_tmp53D.f1=_tmp539->name;
_tmp53D;});_tmp53C;}));_tmp53B;});_tmp53A;})));goto _LL2E2;}_LL2E7: if(*((int*)
_tmp532)!= 9)goto _LL2E9;_LL2E8:(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Offsetof_e_struct*
_tmp53E=_cycalloc(sizeof(*_tmp53E));_tmp53E[0]=({struct Cyc_Absyn_Offsetof_e_struct
_tmp53F;_tmp53F.tag=20;_tmp53F.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp3FB);_tmp53F.f2=(
void*)((void*)({struct Cyc_Absyn_StructField_struct*_tmp540=_cycalloc(sizeof(*
_tmp540));_tmp540[0]=({struct Cyc_Absyn_StructField_struct _tmp541;_tmp541.tag=0;
_tmp541.f1=Cyc_Absyn_fieldname((int)(_tmp3FD + 1));_tmp541;});_tmp540;}));_tmp53F;});
_tmp53E;})));goto _LL2E2;_LL2E9: if(*((int*)_tmp532)!= 3)goto _LL2EB;_LL2EA: if(
_tmp3FD == 0)(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Offsetof_e_struct*
_tmp542=_cycalloc(sizeof(*_tmp542));_tmp542[0]=({struct Cyc_Absyn_Offsetof_e_struct
_tmp543;_tmp543.tag=20;_tmp543.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp3FB);_tmp543.f2=(
void*)((void*)({struct Cyc_Absyn_StructField_struct*_tmp544=_cycalloc(sizeof(*
_tmp544));_tmp544[0]=({struct Cyc_Absyn_StructField_struct _tmp545;_tmp545.tag=0;
_tmp545.f1=Cyc_Toc_tag_sp;_tmp545;});_tmp544;}));_tmp543;});_tmp542;})));else{(
void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Offsetof_e_struct*_tmp546=_cycalloc(
sizeof(*_tmp546));_tmp546[0]=({struct Cyc_Absyn_Offsetof_e_struct _tmp547;_tmp547.tag=
20;_tmp547.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp3FB);_tmp547.f2=(void*)((void*)({
struct Cyc_Absyn_StructField_struct*_tmp548=_cycalloc(sizeof(*_tmp548));_tmp548[0]=({
struct Cyc_Absyn_StructField_struct _tmp549;_tmp549.tag=0;_tmp549.f1=Cyc_Absyn_fieldname((
int)_tmp3FD);_tmp549;});_tmp548;}));_tmp547;});_tmp546;})));}goto _LL2E2;_LL2EB:;
_LL2EC:({void*_tmp54A=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Toc_toc_impos)(({const char*_tmp54B="impossible type for offsetof tuple index";
_tag_dyneither(_tmp54B,sizeof(char),41);}),_tag_dyneither(_tmp54A,sizeof(void*),
0));});_LL2E2:;}goto _LL1E5;_LL218: if(*((int*)_tmp3C9)!= 22)goto _LL21A;_tmp3FE=((
struct Cyc_Absyn_Deref_e_struct*)_tmp3C9)->f1;_LL219: {void*_tmp54C=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp3FE->topt))->v);{void*_tmp54D=
_tmp54C;struct Cyc_Absyn_PtrInfo _tmp54E;void*_tmp54F;struct Cyc_Absyn_Tqual _tmp550;
struct Cyc_Absyn_PtrAtts _tmp551;void*_tmp552;struct Cyc_Absyn_Conref*_tmp553;
struct Cyc_Absyn_Conref*_tmp554;struct Cyc_Absyn_Conref*_tmp555;_LL2EE: if(_tmp54D
<= (void*)4)goto _LL2F0;if(*((int*)_tmp54D)!= 4)goto _LL2F0;_tmp54E=((struct Cyc_Absyn_PointerType_struct*)
_tmp54D)->f1;_tmp54F=(void*)_tmp54E.elt_typ;_tmp550=_tmp54E.elt_tq;_tmp551=
_tmp54E.ptr_atts;_tmp552=(void*)_tmp551.rgn;_tmp553=_tmp551.nullable;_tmp554=
_tmp551.bounds;_tmp555=_tmp551.zero_term;_LL2EF:{void*_tmp556=Cyc_Absyn_conref_def(
Cyc_Absyn_bounds_one,_tmp554);struct Cyc_Absyn_Exp*_tmp557;_LL2F3: if(_tmp556 <= (
void*)1)goto _LL2F5;if(*((int*)_tmp556)!= 0)goto _LL2F5;_tmp557=((struct Cyc_Absyn_Upper_b_struct*)
_tmp556)->f1;_LL2F4: {int do_null_check=Cyc_Toc_need_null_check(_tmp3FE);Cyc_Toc_exp_to_c(
nv,_tmp3FE);if(do_null_check){if(Cyc_Toc_warn_all_null_deref)({void*_tmp558=0;
Cyc_Tcutil_warn(e->loc,({const char*_tmp559="inserted null check due to dereference";
_tag_dyneither(_tmp559,sizeof(char),39);}),_tag_dyneither(_tmp558,sizeof(void*),
0));});(void*)(_tmp3FE->r=(void*)Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(_tmp54C),Cyc_Absyn_fncall_exp(
Cyc_Toc__check_null_e,({struct Cyc_List_List*_tmp55A=_cycalloc(sizeof(*_tmp55A));
_tmp55A->hd=Cyc_Absyn_copy_exp(_tmp3FE);_tmp55A->tl=0;_tmp55A;}),0)));}if(!((int(*)(
int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp555)){unsigned int
_tmp55C;int _tmp55D;struct _tuple5 _tmp55B=Cyc_Evexp_eval_const_uint_exp(_tmp557);
_tmp55C=_tmp55B.f1;_tmp55D=_tmp55B.f2;if(!_tmp55D  || _tmp55C <= 0)({void*_tmp55E=
0;Cyc_Tcutil_terr(e->loc,({const char*_tmp55F="cannot determine dereference is in bounds";
_tag_dyneither(_tmp55F,sizeof(char),42);}),_tag_dyneither(_tmp55E,sizeof(void*),
0));});}goto _LL2F2;}_LL2F5: if((int)_tmp556 != 0)goto _LL2F2;_LL2F6: {struct Cyc_Absyn_Exp*
_tmp560=Cyc_Absyn_uint_exp(0,0);_tmp560->topt=({struct Cyc_Core_Opt*_tmp561=
_cycalloc(sizeof(*_tmp561));_tmp561->v=(void*)Cyc_Absyn_uint_typ;_tmp561;});(
void*)(e->r=(void*)Cyc_Toc_subscript_exp_r(_tmp3FE,_tmp560));Cyc_Toc_exp_to_c(nv,
e);goto _LL2F2;}_LL2F2:;}goto _LL2ED;_LL2F0:;_LL2F1:({void*_tmp562=0;((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp563="exp_to_c: Deref: non-pointer";_tag_dyneither(_tmp563,sizeof(char),29);}),
_tag_dyneither(_tmp562,sizeof(void*),0));});_LL2ED:;}goto _LL1E5;}_LL21A: if(*((
int*)_tmp3C9)!= 23)goto _LL21C;_tmp3FF=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp3C9)->f1;_tmp400=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp3C9)->f2;_LL21B:
Cyc_Toc_exp_to_c(nv,_tmp3FF);if(Cyc_Toc_is_poly_project(e))(void*)(e->r=(void*)((
void*)(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v),Cyc_Absyn_new_exp((void*)e->r,0),0))->r));goto _LL1E5;
_LL21C: if(*((int*)_tmp3C9)!= 24)goto _LL21E;_tmp401=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp3C9)->f1;_tmp402=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp3C9)->f2;_LL21D: {
void*e1typ=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp401->topt))->v);
int do_null_check=Cyc_Toc_need_null_check(_tmp401);Cyc_Toc_exp_to_c(nv,_tmp401);{
int is_poly=Cyc_Toc_is_poly_project(e);void*_tmp565;struct Cyc_Absyn_Tqual _tmp566;
struct Cyc_Absyn_PtrAtts _tmp567;void*_tmp568;struct Cyc_Absyn_Conref*_tmp569;
struct Cyc_Absyn_Conref*_tmp56A;struct Cyc_Absyn_Conref*_tmp56B;struct Cyc_Absyn_PtrInfo
_tmp564=Cyc_Toc_get_ptr_type(e1typ);_tmp565=(void*)_tmp564.elt_typ;_tmp566=
_tmp564.elt_tq;_tmp567=_tmp564.ptr_atts;_tmp568=(void*)_tmp567.rgn;_tmp569=
_tmp567.nullable;_tmp56A=_tmp567.bounds;_tmp56B=_tmp567.zero_term;{void*_tmp56C=
Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp56A);struct Cyc_Absyn_Exp*_tmp56D;
_LL2F8: if(_tmp56C <= (void*)1)goto _LL2FA;if(*((int*)_tmp56C)!= 0)goto _LL2FA;
_tmp56D=((struct Cyc_Absyn_Upper_b_struct*)_tmp56C)->f1;_LL2F9: {unsigned int
_tmp56F;int _tmp570;struct _tuple5 _tmp56E=Cyc_Evexp_eval_const_uint_exp(_tmp56D);
_tmp56F=_tmp56E.f1;_tmp570=_tmp56E.f2;if(_tmp570){if(_tmp56F < 1)({void*_tmp571=0;((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const
char*_tmp572="exp_to_c:  AggrArrow_e on pointer of size 0";_tag_dyneither(
_tmp572,sizeof(char),44);}),_tag_dyneither(_tmp571,sizeof(void*),0));});if(
do_null_check){if(Cyc_Toc_warn_all_null_deref)({void*_tmp573=0;Cyc_Tcutil_warn(e->loc,({
const char*_tmp574="inserted null check due to dereference";_tag_dyneither(
_tmp574,sizeof(char),39);}),_tag_dyneither(_tmp573,sizeof(void*),0));});(void*)(
e->r=(void*)Cyc_Toc_aggrarrow_exp_r(Cyc_Toc_cast_it(Cyc_Toc_typ_to_c((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp401->topt))->v),Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,({
struct Cyc_List_List*_tmp575=_cycalloc(sizeof(*_tmp575));_tmp575->hd=_tmp401;
_tmp575->tl=0;_tmp575;}),0)),_tmp402));}}else{if(!Cyc_Evexp_c_can_eval(_tmp56D))({
void*_tmp576=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp577="cannot determine pointer dereference in bounds";
_tag_dyneither(_tmp577,sizeof(char),47);}),_tag_dyneither(_tmp576,sizeof(void*),
0));});(void*)(e->r=(void*)Cyc_Toc_aggrarrow_exp_r(Cyc_Toc_cast_it(Cyc_Toc_typ_to_c((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp401->topt))->v),Cyc_Absyn_fncall_exp(
Cyc_Toc__check_known_subscript_null_e,({struct Cyc_Absyn_Exp*_tmp578[4];_tmp578[3]=
Cyc_Absyn_uint_exp(0,0);_tmp578[2]=Cyc_Absyn_sizeoftyp_exp(_tmp565,0);_tmp578[1]=
_tmp56D;_tmp578[0]=_tmp401;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmp578,sizeof(struct Cyc_Absyn_Exp*),4));}),0)),_tmp402));}goto
_LL2F7;}_LL2FA: if((int)_tmp56C != 0)goto _LL2F7;_LL2FB: {struct Cyc_Absyn_Exp*
_tmp579=Cyc_Toc__check_dyneither_subscript_e;void*ta1=Cyc_Toc_typ_to_c_array(
_tmp565);(void*)(_tmp401->r=(void*)Cyc_Toc_cast_it_r(Cyc_Absyn_cstar_typ(ta1,
_tmp566),Cyc_Absyn_fncall_exp(_tmp579,({struct Cyc_Absyn_Exp*_tmp57A[3];_tmp57A[2]=
Cyc_Absyn_uint_exp(0,0);_tmp57A[1]=Cyc_Absyn_sizeoftyp_exp(ta1,0);_tmp57A[0]=Cyc_Absyn_copy_exp(
_tmp401);((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmp57A,sizeof(struct Cyc_Absyn_Exp*),3));}),0)));goto _LL2F7;}
_LL2F7:;}if(is_poly)(void*)(e->r=(void*)((void*)(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),Cyc_Absyn_new_exp((void*)e->r,
0),0))->r));goto _LL1E5;}}_LL21E: if(*((int*)_tmp3C9)!= 25)goto _LL220;_tmp403=((
struct Cyc_Absyn_Subscript_e_struct*)_tmp3C9)->f1;_tmp404=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp3C9)->f2;_LL21F: {void*_tmp57B=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp403->topt))->v);{void*_tmp57C=_tmp57B;struct Cyc_List_List*
_tmp57D;struct Cyc_Absyn_PtrInfo _tmp57E;void*_tmp57F;struct Cyc_Absyn_Tqual _tmp580;
struct Cyc_Absyn_PtrAtts _tmp581;void*_tmp582;struct Cyc_Absyn_Conref*_tmp583;
struct Cyc_Absyn_Conref*_tmp584;struct Cyc_Absyn_Conref*_tmp585;_LL2FD: if(_tmp57C
<= (void*)4)goto _LL301;if(*((int*)_tmp57C)!= 9)goto _LL2FF;_tmp57D=((struct Cyc_Absyn_TupleType_struct*)
_tmp57C)->f1;_LL2FE: Cyc_Toc_exp_to_c(nv,_tmp403);Cyc_Toc_exp_to_c(nv,_tmp404);{
unsigned int _tmp587;int _tmp588;struct _tuple5 _tmp586=Cyc_Evexp_eval_const_uint_exp(
_tmp404);_tmp587=_tmp586.f1;_tmp588=_tmp586.f2;if(!_tmp588)({void*_tmp589=0;((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const
char*_tmp58A="unknown tuple subscript in translation to C";_tag_dyneither(
_tmp58A,sizeof(char),44);}),_tag_dyneither(_tmp589,sizeof(void*),0));});(void*)(
e->r=(void*)Cyc_Toc_aggrmember_exp_r(_tmp403,Cyc_Absyn_fieldname((int)(_tmp587 + 
1))));goto _LL2FC;}_LL2FF: if(*((int*)_tmp57C)!= 4)goto _LL301;_tmp57E=((struct Cyc_Absyn_PointerType_struct*)
_tmp57C)->f1;_tmp57F=(void*)_tmp57E.elt_typ;_tmp580=_tmp57E.elt_tq;_tmp581=
_tmp57E.ptr_atts;_tmp582=(void*)_tmp581.rgn;_tmp583=_tmp581.nullable;_tmp584=
_tmp581.bounds;_tmp585=_tmp581.zero_term;_LL300: {struct Cyc_List_List*_tmp58B=
Cyc_Toc_get_relns(_tmp403);int in_bnds=0;{void*_tmp58C=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,
_tmp584);_LL304:;_LL305: in_bnds=Cyc_Toc_check_bounds(_tmp58B,_tmp403,_tmp404);
if(Cyc_Toc_warn_bounds_checks  && !in_bnds)({struct Cyc_String_pa_struct _tmp58F;
_tmp58F.tag=0;_tmp58F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(
e));{void*_tmp58D[1]={& _tmp58F};Cyc_Tcutil_warn(e->loc,({const char*_tmp58E="bounds check necessary for %s";
_tag_dyneither(_tmp58E,sizeof(char),30);}),_tag_dyneither(_tmp58D,sizeof(void*),
1));}});_LL303:;}Cyc_Toc_exp_to_c(nv,_tmp403);Cyc_Toc_exp_to_c(nv,_tmp404);++ Cyc_Toc_total_bounds_checks;{
void*_tmp590=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp584);struct Cyc_Absyn_Exp*
_tmp591;_LL307: if(_tmp590 <= (void*)1)goto _LL309;if(*((int*)_tmp590)!= 0)goto
_LL309;_tmp591=((struct Cyc_Absyn_Upper_b_struct*)_tmp590)->f1;_LL308: {int
possibly_null=((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,
_tmp583);void*ta1=Cyc_Toc_typ_to_c(_tmp57F);void*ta2=Cyc_Absyn_cstar_typ(ta1,
_tmp580);if(in_bnds)++ Cyc_Toc_bounds_checks_eliminated;else{if(((int(*)(int,
struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp585)){if(!Cyc_Evexp_c_can_eval(
_tmp591))({void*_tmp592=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp593="cannot determine subscript is in bounds";
_tag_dyneither(_tmp593,sizeof(char),40);}),_tag_dyneither(_tmp592,sizeof(void*),
0));});{struct Cyc_Absyn_Exp*function_e=Cyc_Toc_getFunction(& Cyc_Toc__zero_arr_plus_functionSet,
_tmp403);(void*)(e->r=(void*)Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(ta2,Cyc_Absyn_fncall_exp(
function_e,({struct Cyc_Absyn_Exp*_tmp594[3];_tmp594[2]=_tmp404;_tmp594[1]=
_tmp591;_tmp594[0]=_tmp403;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmp594,sizeof(struct Cyc_Absyn_Exp*),3));}),0))));}}else{if(
possibly_null){if(!Cyc_Evexp_c_can_eval(_tmp591))({void*_tmp595=0;Cyc_Tcutil_terr(
e->loc,({const char*_tmp596="cannot determine subscript is in bounds";
_tag_dyneither(_tmp596,sizeof(char),40);}),_tag_dyneither(_tmp595,sizeof(void*),
0));});if(Cyc_Toc_warn_all_null_deref)({void*_tmp597=0;Cyc_Tcutil_warn(e->loc,({
const char*_tmp598="inserted null check due to dereference";_tag_dyneither(
_tmp598,sizeof(char),39);}),_tag_dyneither(_tmp597,sizeof(void*),0));});(void*)(
e->r=(void*)Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(ta2,Cyc_Absyn_fncall_exp(Cyc_Toc__check_known_subscript_null_e,({
struct Cyc_Absyn_Exp*_tmp599[4];_tmp599[3]=_tmp404;_tmp599[2]=Cyc_Absyn_sizeoftyp_exp(
ta1,0);_tmp599[1]=_tmp591;_tmp599[0]=_tmp403;((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp599,sizeof(struct Cyc_Absyn_Exp*),
4));}),0))));}else{(void*)(_tmp404->r=(void*)Cyc_Toc_fncall_exp_r(Cyc_Toc__check_known_subscript_notnull_e,({
struct Cyc_Absyn_Exp*_tmp59A[2];_tmp59A[1]=Cyc_Absyn_copy_exp(_tmp404);_tmp59A[0]=
_tmp591;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmp59A,sizeof(struct Cyc_Absyn_Exp*),2));})));}}}goto _LL306;}
_LL309: if((int)_tmp590 != 0)goto _LL306;_LL30A: {void*ta1=Cyc_Toc_typ_to_c_array(
_tmp57F);if(in_bnds){++ Cyc_Toc_bounds_checks_eliminated;(void*)(e->r=(void*)Cyc_Toc_subscript_exp_r(
Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(ta1,_tmp580),Cyc_Absyn_aggrmember_exp(
_tmp403,Cyc_Toc_curr_sp,0)),_tmp404));}else{struct Cyc_Absyn_Exp*_tmp59B=Cyc_Toc__check_dyneither_subscript_e;(
void*)(e->r=(void*)Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(ta1,
_tmp580),Cyc_Absyn_fncall_exp(_tmp59B,({struct Cyc_Absyn_Exp*_tmp59C[3];_tmp59C[2]=
_tmp404;_tmp59C[1]=Cyc_Absyn_sizeoftyp_exp(ta1,0);_tmp59C[0]=_tmp403;((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp59C,sizeof(struct Cyc_Absyn_Exp*),
3));}),0))));}goto _LL306;}_LL306:;}goto _LL2FC;}_LL301:;_LL302:({void*_tmp59D=0;((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp59E="exp_to_c: Subscript on non-tuple/array/tuple ptr";
_tag_dyneither(_tmp59E,sizeof(char),49);}),_tag_dyneither(_tmp59D,sizeof(void*),
0));});_LL2FC:;}goto _LL1E5;}_LL220: if(*((int*)_tmp3C9)!= 26)goto _LL222;_tmp405=((
struct Cyc_Absyn_Tuple_e_struct*)_tmp3C9)->f1;_LL221: if(!Cyc_Toc_is_toplevel(nv))(
void*)(e->r=(void*)((void*)(Cyc_Toc_init_tuple(nv,0,0,_tmp405))->r));else{struct
Cyc_List_List*_tmp59F=((struct Cyc_List_List*(*)(struct _tuple4*(*f)(struct Cyc_Absyn_Exp*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_tup_to_c,_tmp405);void*_tmp5A0=Cyc_Toc_add_tuple_type(
_tmp59F);struct Cyc_List_List*dles=0;{int i=1;for(0;_tmp405 != 0;(_tmp405=_tmp405->tl,
i ++)){Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp405->hd);dles=({struct Cyc_List_List*
_tmp5A1=_cycalloc(sizeof(*_tmp5A1));_tmp5A1->hd=({struct _tuple8*_tmp5A2=
_cycalloc(sizeof(*_tmp5A2));_tmp5A2->f1=0;_tmp5A2->f2=(struct Cyc_Absyn_Exp*)
_tmp405->hd;_tmp5A2;});_tmp5A1->tl=dles;_tmp5A1;});}}dles=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);(void*)(e->r=(void*)Cyc_Toc_unresolvedmem_exp_r(
0,dles));}goto _LL1E5;_LL222: if(*((int*)_tmp3C9)!= 28)goto _LL224;_tmp406=((struct
Cyc_Absyn_Array_e_struct*)_tmp3C9)->f1;_LL223:(void*)(e->r=(void*)Cyc_Toc_unresolvedmem_exp_r(
0,_tmp406));{struct Cyc_List_List*_tmp5A3=_tmp406;for(0;_tmp5A3 != 0;_tmp5A3=
_tmp5A3->tl){struct _tuple8 _tmp5A5;struct Cyc_Absyn_Exp*_tmp5A6;struct _tuple8*
_tmp5A4=(struct _tuple8*)_tmp5A3->hd;_tmp5A5=*_tmp5A4;_tmp5A6=_tmp5A5.f2;Cyc_Toc_exp_to_c(
nv,_tmp5A6);}}goto _LL1E5;_LL224: if(*((int*)_tmp3C9)!= 29)goto _LL226;_tmp407=((
struct Cyc_Absyn_Comprehension_e_struct*)_tmp3C9)->f1;_tmp408=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp3C9)->f2;_tmp409=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp3C9)->f3;
_tmp40A=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp3C9)->f4;_LL225: {
unsigned int _tmp5A8;int _tmp5A9;struct _tuple5 _tmp5A7=Cyc_Evexp_eval_const_uint_exp(
_tmp408);_tmp5A8=_tmp5A7.f1;_tmp5A9=_tmp5A7.f2;{void*_tmp5AA=Cyc_Toc_typ_to_c((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp409->topt))->v);Cyc_Toc_exp_to_c(nv,
_tmp409);{struct Cyc_List_List*es=0;if(!Cyc_Toc_is_zero(_tmp409)){if(!_tmp5A9)({
void*_tmp5AB=0;Cyc_Tcutil_terr(_tmp408->loc,({const char*_tmp5AC="cannot determine value of constant";
_tag_dyneither(_tmp5AC,sizeof(char),35);}),_tag_dyneither(_tmp5AB,sizeof(void*),
0));});{unsigned int i=0;for(0;i < _tmp5A8;++ i){es=({struct Cyc_List_List*_tmp5AD=
_cycalloc(sizeof(*_tmp5AD));_tmp5AD->hd=({struct _tuple8*_tmp5AE=_cycalloc(
sizeof(*_tmp5AE));_tmp5AE->f1=0;_tmp5AE->f2=_tmp409;_tmp5AE;});_tmp5AD->tl=es;
_tmp5AD;});}}if(_tmp40A){struct Cyc_Absyn_Exp*_tmp5AF=Cyc_Toc_cast_it(_tmp5AA,Cyc_Absyn_uint_exp(
0,0));es=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_imp_append)(es,({struct Cyc_List_List*_tmp5B0=_cycalloc(sizeof(*_tmp5B0));
_tmp5B0->hd=({struct _tuple8*_tmp5B1=_cycalloc(sizeof(*_tmp5B1));_tmp5B1->f1=0;
_tmp5B1->f2=_tmp5AF;_tmp5B1;});_tmp5B0->tl=0;_tmp5B0;}));}}(void*)(e->r=(void*)
Cyc_Toc_unresolvedmem_exp_r(0,es));goto _LL1E5;}}}_LL226: if(*((int*)_tmp3C9)!= 30)
goto _LL228;_tmp40B=((struct Cyc_Absyn_Struct_e_struct*)_tmp3C9)->f1;_tmp40C=((
struct Cyc_Absyn_Struct_e_struct*)_tmp3C9)->f2;_tmp40D=((struct Cyc_Absyn_Struct_e_struct*)
_tmp3C9)->f3;_tmp40E=((struct Cyc_Absyn_Struct_e_struct*)_tmp3C9)->f4;_LL227: if(!
Cyc_Toc_is_toplevel(nv))(void*)(e->r=(void*)((void*)(Cyc_Toc_init_struct(nv,
old_typ,_tmp40C != 0,0,0,_tmp40D,_tmp40B))->r));else{if(_tmp40E == 0)({void*
_tmp5B2=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp5B3="Struct_e: missing aggrdecl pointer";_tag_dyneither(_tmp5B3,
sizeof(char),35);}),_tag_dyneither(_tmp5B2,sizeof(void*),0));});{struct Cyc_Absyn_Aggrdecl*
sd2=(struct Cyc_Absyn_Aggrdecl*)_tmp40E;struct Cyc_Toc_Env _tmp5B5;struct
_RegionHandle*_tmp5B6;struct Cyc_Toc_Env*_tmp5B4=nv;_tmp5B5=*_tmp5B4;_tmp5B6=
_tmp5B5.rgn;{struct Cyc_List_List*_tmp5B7=((struct Cyc_List_List*(*)(struct
_RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,struct
Cyc_List_List*fields))Cyc_Tcutil_resolve_struct_designators)(_tmp5B6,e->loc,
_tmp40D,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(sd2->impl))->fields);struct
Cyc_List_List*_tmp5B8=0;struct Cyc_List_List*_tmp5B9=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(sd2->impl))->fields;for(0;_tmp5B9 != 0;_tmp5B9=_tmp5B9->tl){struct Cyc_List_List*
_tmp5BA=_tmp5B7;for(0;_tmp5BA != 0;_tmp5BA=_tmp5BA->tl){if((*((struct _tuple12*)
_tmp5BA->hd)).f1 == (struct Cyc_Absyn_Aggrfield*)_tmp5B9->hd){struct _tuple12
_tmp5BC;struct Cyc_Absyn_Aggrfield*_tmp5BD;struct Cyc_Absyn_Exp*_tmp5BE;struct
_tuple12*_tmp5BB=(struct _tuple12*)_tmp5BA->hd;_tmp5BC=*_tmp5BB;_tmp5BD=_tmp5BC.f1;
_tmp5BE=_tmp5BC.f2;{void*_tmp5BF=(void*)_tmp5BD->type;Cyc_Toc_exp_to_c(nv,
_tmp5BE);if(Cyc_Toc_is_void_star(_tmp5BF))(void*)(_tmp5BE->r=(void*)Cyc_Toc_cast_it_r(
Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp((void*)_tmp5BE->r,0)));_tmp5B8=({
struct Cyc_List_List*_tmp5C0=_cycalloc(sizeof(*_tmp5C0));_tmp5C0->hd=({struct
_tuple8*_tmp5C1=_cycalloc(sizeof(*_tmp5C1));_tmp5C1->f1=0;_tmp5C1->f2=_tmp5BE;
_tmp5C1;});_tmp5C0->tl=_tmp5B8;_tmp5C0;});break;}}}}(void*)(e->r=(void*)Cyc_Toc_unresolvedmem_exp_r(
0,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp5B8)));}}}
goto _LL1E5;_LL228: if(*((int*)_tmp3C9)!= 31)goto _LL22A;_tmp40F=(void*)((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp3C9)->f1;_tmp410=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp3C9)->f2;_LL229: {
struct Cyc_List_List*fs;{void*_tmp5C2=Cyc_Tcutil_compress(_tmp40F);struct Cyc_List_List*
_tmp5C3;_LL30C: if(_tmp5C2 <= (void*)4)goto _LL30E;if(*((int*)_tmp5C2)!= 11)goto
_LL30E;_tmp5C3=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp5C2)->f2;_LL30D: fs=
_tmp5C3;goto _LL30B;_LL30E:;_LL30F:({struct Cyc_String_pa_struct _tmp5C6;_tmp5C6.tag=
0;_tmp5C6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
_tmp40F));{void*_tmp5C4[1]={& _tmp5C6};((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp5C5="anon struct has type %s";
_tag_dyneither(_tmp5C5,sizeof(char),24);}),_tag_dyneither(_tmp5C4,sizeof(void*),
1));}});_LL30B:;}{struct Cyc_Toc_Env _tmp5C8;struct _RegionHandle*_tmp5C9;struct Cyc_Toc_Env*
_tmp5C7=nv;_tmp5C8=*_tmp5C7;_tmp5C9=_tmp5C8.rgn;{struct Cyc_List_List*_tmp5CA=((
struct Cyc_List_List*(*)(struct _RegionHandle*rgn,struct Cyc_Position_Segment*loc,
struct Cyc_List_List*des,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_struct_designators)(
_tmp5C9,e->loc,_tmp410,fs);for(0;_tmp5CA != 0;_tmp5CA=_tmp5CA->tl){struct _tuple12
_tmp5CC;struct Cyc_Absyn_Aggrfield*_tmp5CD;struct Cyc_Absyn_Exp*_tmp5CE;struct
_tuple12*_tmp5CB=(struct _tuple12*)_tmp5CA->hd;_tmp5CC=*_tmp5CB;_tmp5CD=_tmp5CC.f1;
_tmp5CE=_tmp5CC.f2;{void*_tmp5CF=(void*)_tmp5CD->type;Cyc_Toc_exp_to_c(nv,
_tmp5CE);if(Cyc_Toc_is_void_star(_tmp5CF))(void*)(_tmp5CE->r=(void*)Cyc_Toc_cast_it_r(
Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp((void*)_tmp5CE->r,0)));}}(void*)(e->r=(
void*)Cyc_Toc_unresolvedmem_exp_r(0,_tmp410));}goto _LL1E5;}}_LL22A: if(*((int*)
_tmp3C9)!= 32)goto _LL22C;_tmp411=((struct Cyc_Absyn_Tunion_e_struct*)_tmp3C9)->f1;
if(_tmp411 != 0)goto _LL22C;_tmp412=((struct Cyc_Absyn_Tunion_e_struct*)_tmp3C9)->f2;
_tmp413=((struct Cyc_Absyn_Tunion_e_struct*)_tmp3C9)->f3;if(!(!_tmp412->is_flat))
goto _LL22C;_LL22B: {struct _tuple1*qv=_tmp413->name;struct Cyc_Absyn_Exp*tag_exp=
_tmp412->is_xtunion?Cyc_Absyn_var_exp(qv,0): Cyc_Toc_tunion_tag(_tmp412,qv,0);(
void*)(e->r=(void*)((void*)tag_exp->r));goto _LL1E5;}_LL22C: if(*((int*)_tmp3C9)!= 
32)goto _LL22E;_tmp414=((struct Cyc_Absyn_Tunion_e_struct*)_tmp3C9)->f1;_tmp415=((
struct Cyc_Absyn_Tunion_e_struct*)_tmp3C9)->f2;_tmp416=((struct Cyc_Absyn_Tunion_e_struct*)
_tmp3C9)->f3;_LL22D: {void*tunion_ctype;struct Cyc_Absyn_Exp*tag_exp;struct
_tuple1*_tmp5D0=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp5D1=Cyc_Absyn_var_exp(
_tmp5D0,0);struct Cyc_Absyn_Exp*member_exp;if(_tmp415->is_flat){tunion_ctype=Cyc_Absyn_unionq_typ(
Cyc_Toc_collapse_qvar_tag(_tmp415->name,({const char*_tmp5D2="_union";
_tag_dyneither(_tmp5D2,sizeof(char),7);})));tag_exp=Cyc_Toc_tunion_tag(_tmp415,
_tmp416->name,1);{struct _tuple1 _tmp5D4;struct _dyneither_ptr*_tmp5D5;struct
_tuple1*_tmp5D3=_tmp416->name;_tmp5D4=*_tmp5D3;_tmp5D5=_tmp5D4.f2;member_exp=Cyc_Absyn_aggrmember_exp(
_tmp5D1,_tmp5D5,0);}}else{tunion_ctype=Cyc_Absyn_strctq(Cyc_Toc_collapse_qvar_tag(
_tmp416->name,({const char*_tmp5D6="_struct";_tag_dyneither(_tmp5D6,sizeof(char),
8);})));tag_exp=_tmp415->is_xtunion?Cyc_Absyn_var_exp(_tmp416->name,0): Cyc_Toc_tunion_tag(
_tmp415,_tmp416->name,1);member_exp=_tmp5D1;}{struct Cyc_List_List*_tmp5D7=
_tmp416->typs;if(Cyc_Toc_is_toplevel(nv)){struct Cyc_List_List*dles=0;for(0;
_tmp414 != 0;(_tmp414=_tmp414->tl,_tmp5D7=_tmp5D7->tl)){struct Cyc_Absyn_Exp*cur_e=(
struct Cyc_Absyn_Exp*)_tmp414->hd;void*field_typ=Cyc_Toc_typ_to_c((*((struct
_tuple4*)((struct Cyc_List_List*)_check_null(_tmp5D7))->hd)).f2);Cyc_Toc_exp_to_c(
nv,cur_e);if(Cyc_Toc_is_void_star(field_typ))cur_e=Cyc_Toc_cast_it(field_typ,
cur_e);dles=({struct Cyc_List_List*_tmp5D8=_cycalloc(sizeof(*_tmp5D8));_tmp5D8->hd=({
struct _tuple8*_tmp5D9=_cycalloc(sizeof(*_tmp5D9));_tmp5D9->f1=0;_tmp5D9->f2=
cur_e;_tmp5D9;});_tmp5D8->tl=dles;_tmp5D8;});}dles=({struct Cyc_List_List*_tmp5DA=
_cycalloc(sizeof(*_tmp5DA));_tmp5DA->hd=({struct _tuple8*_tmp5DB=_cycalloc(
sizeof(*_tmp5DB));_tmp5DB->f1=0;_tmp5DB->f2=tag_exp;_tmp5DB;});_tmp5DA->tl=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);_tmp5DA;});(
void*)(e->r=(void*)Cyc_Toc_unresolvedmem_exp_r(0,dles));if(_tmp415->is_flat)(
void*)(e->r=(void*)Cyc_Toc_cast_it_r(Cyc_Absyn_strctq(Cyc_Toc_collapse_qvar_tag(
_tmp416->name,({const char*_tmp5DC="_struct";_tag_dyneither(_tmp5DC,sizeof(char),
8);}))),Cyc_Absyn_copy_exp(e)));}else{struct Cyc_List_List*_tmp5DD=({struct Cyc_List_List*
_tmp5E3=_cycalloc(sizeof(*_tmp5E3));_tmp5E3->hd=Cyc_Absyn_assign_stmt(Cyc_Absyn_aggrmember_exp(
member_exp,Cyc_Toc_tag_sp,0),tag_exp,0);_tmp5E3->tl=0;_tmp5E3;});{int i=1;for(0;
_tmp414 != 0;(((_tmp414=_tmp414->tl,i ++)),_tmp5D7=_tmp5D7->tl)){struct Cyc_Absyn_Exp*
cur_e=(struct Cyc_Absyn_Exp*)_tmp414->hd;void*field_typ=Cyc_Toc_typ_to_c((*((
struct _tuple4*)((struct Cyc_List_List*)_check_null(_tmp5D7))->hd)).f2);Cyc_Toc_exp_to_c(
nv,cur_e);if(Cyc_Toc_is_void_star(field_typ))cur_e=Cyc_Toc_cast_it(field_typ,
cur_e);{struct Cyc_Absyn_Stmt*_tmp5DE=Cyc_Absyn_assign_stmt(Cyc_Absyn_aggrmember_exp(
member_exp,Cyc_Absyn_fieldname(i),0),cur_e,0);_tmp5DD=({struct Cyc_List_List*
_tmp5DF=_cycalloc(sizeof(*_tmp5DF));_tmp5DF->hd=_tmp5DE;_tmp5DF->tl=_tmp5DD;
_tmp5DF;});}}}{struct Cyc_Absyn_Stmt*_tmp5E0=Cyc_Absyn_exp_stmt(_tmp5D1,0);struct
Cyc_Absyn_Stmt*_tmp5E1=Cyc_Absyn_seq_stmts(((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(({struct Cyc_List_List*_tmp5E2=_cycalloc(sizeof(*_tmp5E2));
_tmp5E2->hd=_tmp5E0;_tmp5E2->tl=_tmp5DD;_tmp5E2;})),0);(void*)(e->r=(void*)Cyc_Toc_stmt_exp_r(
Cyc_Absyn_declare_stmt(_tmp5D0,tunion_ctype,0,_tmp5E1,0)));}}goto _LL1E5;}}_LL22E:
if(*((int*)_tmp3C9)!= 33)goto _LL230;_LL22F: goto _LL231;_LL230: if(*((int*)_tmp3C9)
!= 34)goto _LL232;_LL231: goto _LL1E5;_LL232: if(*((int*)_tmp3C9)!= 35)goto _LL234;
_tmp417=((struct Cyc_Absyn_Malloc_e_struct*)_tmp3C9)->f1;_tmp418=_tmp417.is_calloc;
_tmp419=_tmp417.rgn;_tmp41A=_tmp417.elt_type;_tmp41B=_tmp417.num_elts;_tmp41C=
_tmp417.fat_result;_LL233: {void*t_c=Cyc_Toc_typ_to_c(*((void**)_check_null(
_tmp41A)));Cyc_Toc_exp_to_c(nv,_tmp41B);if(_tmp41C){struct _tuple1*_tmp5E4=Cyc_Toc_temp_var();
struct _tuple1*_tmp5E5=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*pexp;struct Cyc_Absyn_Exp*
xexp;struct Cyc_Absyn_Exp*rexp;if(_tmp418){xexp=_tmp41B;if(_tmp419 != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=(struct Cyc_Absyn_Exp*)_tmp419;Cyc_Toc_exp_to_c(nv,rgn);
pexp=Cyc_Toc_rcalloc_exp(rgn,Cyc_Absyn_sizeoftyp_exp(t_c,0),Cyc_Absyn_var_exp(
_tmp5E4,0));}else{pexp=Cyc_Toc_calloc_exp(*_tmp41A,Cyc_Absyn_sizeoftyp_exp(t_c,0),
Cyc_Absyn_var_exp(_tmp5E4,0));}rexp=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_dyneither_e,({
struct Cyc_Absyn_Exp*_tmp5E6[3];_tmp5E6[2]=Cyc_Absyn_var_exp(_tmp5E4,0);_tmp5E6[1]=
Cyc_Absyn_sizeoftyp_exp(t_c,0);_tmp5E6[0]=Cyc_Absyn_var_exp(_tmp5E5,0);((struct
Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp5E6,
sizeof(struct Cyc_Absyn_Exp*),3));}),0);}else{xexp=Cyc_Absyn_times_exp(Cyc_Absyn_sizeoftyp_exp(
t_c,0),_tmp41B,0);if(_tmp419 != 0  && !Cyc_Absyn_no_regions){struct Cyc_Absyn_Exp*
rgn=(struct Cyc_Absyn_Exp*)_tmp419;Cyc_Toc_exp_to_c(nv,rgn);pexp=Cyc_Toc_rmalloc_exp(
rgn,Cyc_Absyn_var_exp(_tmp5E4,0));}else{pexp=Cyc_Toc_malloc_exp(*_tmp41A,Cyc_Absyn_var_exp(
_tmp5E4,0));}rexp=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_dyneither_e,({struct Cyc_Absyn_Exp*
_tmp5E7[3];_tmp5E7[2]=Cyc_Absyn_var_exp(_tmp5E4,0);_tmp5E7[1]=Cyc_Absyn_uint_exp(
1,0);_tmp5E7[0]=Cyc_Absyn_var_exp(_tmp5E5,0);((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp5E7,sizeof(struct Cyc_Absyn_Exp*),
3));}),0);}{struct Cyc_Absyn_Stmt*_tmp5E8=Cyc_Absyn_declare_stmt(_tmp5E4,Cyc_Absyn_uint_typ,(
struct Cyc_Absyn_Exp*)xexp,Cyc_Absyn_declare_stmt(_tmp5E5,Cyc_Absyn_cstar_typ(t_c,
Cyc_Toc_mt_tq),(struct Cyc_Absyn_Exp*)pexp,Cyc_Absyn_exp_stmt(rexp,0),0),0);(void*)(
e->r=(void*)Cyc_Toc_stmt_exp_r(_tmp5E8));}}else{struct Cyc_Absyn_Exp*_tmp5E9=Cyc_Absyn_sizeoftyp_exp(
t_c,0);{void*_tmp5EA=(void*)_tmp41B->r;union Cyc_Absyn_Cnst_union _tmp5EB;int
_tmp5EC;_LL311: if(*((int*)_tmp5EA)!= 0)goto _LL313;_tmp5EB=((struct Cyc_Absyn_Const_e_struct*)
_tmp5EA)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)_tmp5EA)->f1).Int_c).tag != 2)
goto _LL313;_tmp5EC=(_tmp5EB.Int_c).f2;if(_tmp5EC != 1)goto _LL313;_LL312: goto
_LL310;_LL313:;_LL314: _tmp5E9=Cyc_Absyn_times_exp(_tmp5E9,_tmp41B,0);goto _LL310;
_LL310:;}if(_tmp419 != 0  && !Cyc_Absyn_no_regions){struct Cyc_Absyn_Exp*rgn=(
struct Cyc_Absyn_Exp*)_tmp419;Cyc_Toc_exp_to_c(nv,rgn);(void*)(e->r=(void*)((void*)(
Cyc_Toc_rmalloc_exp(rgn,_tmp5E9))->r));}else{(void*)(e->r=(void*)((void*)(Cyc_Toc_malloc_exp(*
_tmp41A,_tmp5E9))->r));}}goto _LL1E5;}_LL234: if(*((int*)_tmp3C9)!= 36)goto _LL236;
_tmp41D=((struct Cyc_Absyn_Swap_e_struct*)_tmp3C9)->f1;_tmp41E=((struct Cyc_Absyn_Swap_e_struct*)
_tmp3C9)->f2;_LL235: {int is_dyneither_ptr=0;void*e1_old_typ=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp41D->topt))->v;void*e2_old_typ=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp41E->topt))->v;if(!Cyc_Tcutil_is_pointer_or_boxed(e1_old_typ,&
is_dyneither_ptr))({void*_tmp5ED=0;((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp5EE="Swap_e: is_pointer_or_boxed: not a pointer or boxed type";
_tag_dyneither(_tmp5EE,sizeof(char),57);}),_tag_dyneither(_tmp5ED,sizeof(void*),
0));});{struct Cyc_Absyn_Exp*swap_fn;if(is_dyneither_ptr)swap_fn=Cyc_Toc__swap_dyneither_e;
else{swap_fn=Cyc_Toc__swap_word_e;}if(!Cyc_Absyn_is_lvalue(_tmp41D))({struct Cyc_String_pa_struct
_tmp5F1;_tmp5F1.tag=0;_tmp5F1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(_tmp41D));{void*_tmp5EF[1]={& _tmp5F1};((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp5F0="Swap_e: %s is not an l-value\n";
_tag_dyneither(_tmp5F0,sizeof(char),30);}),_tag_dyneither(_tmp5EF,sizeof(void*),
1));}});if(!Cyc_Absyn_is_lvalue(_tmp41E))({struct Cyc_String_pa_struct _tmp5F4;
_tmp5F4.tag=0;_tmp5F4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(
_tmp41E));{void*_tmp5F2[1]={& _tmp5F4};((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp5F3="Swap_e: %s is not an l-value\n";
_tag_dyneither(_tmp5F3,sizeof(char),30);}),_tag_dyneither(_tmp5F2,sizeof(void*),
1));}});Cyc_Toc_exp_to_c(nv,_tmp41D);Cyc_Toc_exp_to_c(nv,_tmp41E);(void*)(e->r=(
void*)Cyc_Toc_fncall_exp_r(swap_fn,({struct Cyc_Absyn_Exp*_tmp5F5[2];_tmp5F5[1]=
Cyc_Absyn_address_exp(_tmp41E,0);_tmp5F5[0]=Cyc_Absyn_address_exp(_tmp41D,0);((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmp5F5,sizeof(struct Cyc_Absyn_Exp*),2));})));goto _LL1E5;}}_LL236: if(*((int*)
_tmp3C9)!= 38)goto _LL238;_tmp41F=((struct Cyc_Absyn_StmtExp_e_struct*)_tmp3C9)->f1;
_LL237: Cyc_Toc_stmt_to_c(nv,_tmp41F);goto _LL1E5;_LL238: if(*((int*)_tmp3C9)!= 37)
goto _LL23A;_LL239:({void*_tmp5F6=0;((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp5F7="UnresolvedMem";
_tag_dyneither(_tmp5F7,sizeof(char),14);}),_tag_dyneither(_tmp5F6,sizeof(void*),
0));});_LL23A: if(*((int*)_tmp3C9)!= 27)goto _LL23C;_LL23B:({void*_tmp5F8=0;((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(({const char*
_tmp5F9="compoundlit";_tag_dyneither(_tmp5F9,sizeof(char),12);}),_tag_dyneither(
_tmp5F8,sizeof(void*),0));});_LL23C: if(*((int*)_tmp3C9)!= 39)goto _LL1E5;_LL23D:({
void*_tmp5FA=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp5FB="valueof(-)";_tag_dyneither(_tmp5FB,sizeof(char),11);}),
_tag_dyneither(_tmp5FA,sizeof(void*),0));});_LL1E5:;}}static struct Cyc_Absyn_Stmt*
Cyc_Toc_if_neq_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Stmt*
fail_stmt){return Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_neq_exp(e1,e2,0),fail_stmt,
Cyc_Toc_skip_stmt_dl(),0);}struct _tuple13{struct Cyc_Toc_Env*f1;struct Cyc_List_List*
f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple14{struct _tuple1*f1;void*f2;};struct
_tuple15{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};static struct _tuple13 Cyc_Toc_xlate_pat(
struct Cyc_Toc_Env*nv,struct _RegionHandle*rgn,void*t,struct Cyc_Absyn_Exp*r,struct
Cyc_Absyn_Exp*path,struct Cyc_Absyn_Pat*p,struct Cyc_Absyn_Stmt**tag_fail_stmt,
struct Cyc_Absyn_Stmt*fail_stmt,struct Cyc_List_List*decls){struct Cyc_Absyn_Stmt*s;{
void*_tmp5FC=(void*)p->r;struct Cyc_Absyn_Vardecl*_tmp5FD;struct Cyc_Absyn_Vardecl
_tmp5FE;struct _tuple1*_tmp5FF;struct Cyc_Absyn_Pat*_tmp600;struct Cyc_Absyn_Vardecl*
_tmp601;struct Cyc_Absyn_Vardecl _tmp602;struct _tuple1*_tmp603;struct Cyc_Absyn_Vardecl*
_tmp604;struct Cyc_Absyn_Pat*_tmp605;void*_tmp606;int _tmp607;char _tmp608;struct
_dyneither_ptr _tmp609;struct Cyc_Absyn_Enumdecl*_tmp60A;struct Cyc_Absyn_Enumfield*
_tmp60B;void*_tmp60C;struct Cyc_Absyn_Enumfield*_tmp60D;struct Cyc_Absyn_Tuniondecl*
_tmp60E;struct Cyc_Absyn_Tunionfield*_tmp60F;struct Cyc_List_List*_tmp610;struct
Cyc_Absyn_Pat*_tmp611;struct Cyc_Absyn_Pat _tmp612;void*_tmp613;struct Cyc_Absyn_Tuniondecl*
_tmp614;struct Cyc_Absyn_Tunionfield*_tmp615;struct Cyc_List_List*_tmp616;struct
Cyc_Absyn_Tuniondecl*_tmp617;struct Cyc_Absyn_Tunionfield*_tmp618;struct Cyc_List_List*
_tmp619;struct Cyc_List_List*_tmp61A;struct Cyc_List_List*_tmp61B;struct Cyc_Absyn_AggrInfo
_tmp61C;union Cyc_Absyn_AggrInfoU_union _tmp61D;struct Cyc_List_List*_tmp61E;struct
Cyc_Absyn_Pat*_tmp61F;_LL316: if(_tmp5FC <= (void*)2)goto _LL31A;if(*((int*)_tmp5FC)
!= 0)goto _LL318;_tmp5FD=((struct Cyc_Absyn_Var_p_struct*)_tmp5FC)->f1;_tmp5FE=*
_tmp5FD;_tmp5FF=_tmp5FE.name;_tmp600=((struct Cyc_Absyn_Var_p_struct*)_tmp5FC)->f2;
_LL317: return Cyc_Toc_xlate_pat(Cyc_Toc_add_varmap(rgn,nv,_tmp5FF,r),rgn,t,r,path,
_tmp600,tag_fail_stmt,fail_stmt,decls);_LL318: if(*((int*)_tmp5FC)!= 2)goto _LL31A;
_tmp601=((struct Cyc_Absyn_TagInt_p_struct*)_tmp5FC)->f2;_tmp602=*_tmp601;_tmp603=
_tmp602.name;_LL319: nv=Cyc_Toc_add_varmap(rgn,nv,_tmp603,r);goto _LL31B;_LL31A:
if((int)_tmp5FC != 0)goto _LL31C;_LL31B: s=Cyc_Toc_skip_stmt_dl();goto _LL315;_LL31C:
if(_tmp5FC <= (void*)2)goto _LL31E;if(*((int*)_tmp5FC)!= 1)goto _LL31E;_tmp604=((
struct Cyc_Absyn_Reference_p_struct*)_tmp5FC)->f1;_tmp605=((struct Cyc_Absyn_Reference_p_struct*)
_tmp5FC)->f2;_LL31D: {struct _tuple1*_tmp620=Cyc_Toc_temp_var();decls=({struct Cyc_List_List*
_tmp621=_region_malloc(rgn,sizeof(*_tmp621));_tmp621->hd=({struct _tuple14*
_tmp622=_region_malloc(rgn,sizeof(*_tmp622));_tmp622->f1=_tmp620;_tmp622->f2=Cyc_Absyn_cstar_typ(
Cyc_Toc_typ_to_c(t),Cyc_Toc_mt_tq);_tmp622;});_tmp621->tl=decls;_tmp621;});nv=
Cyc_Toc_add_varmap(rgn,nv,_tmp604->name,Cyc_Absyn_var_exp(_tmp620,0));s=Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp(_tmp620,0),Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c(
t),Cyc_Toc_mt_tq),Cyc_Absyn_address_exp(path,0)),0);{struct _tuple13 _tmp623=Cyc_Toc_xlate_pat(
nv,rgn,t,r,path,_tmp605,tag_fail_stmt,fail_stmt,decls);_tmp623.f3=Cyc_Absyn_seq_stmt(
s,_tmp623.f3,0);return _tmp623;}}_LL31E: if((int)_tmp5FC != 1)goto _LL320;_LL31F: s=
Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_signed_int_exp(0,0),fail_stmt);goto _LL315;_LL320:
if(_tmp5FC <= (void*)2)goto _LL322;if(*((int*)_tmp5FC)!= 7)goto _LL322;_tmp606=(
void*)((struct Cyc_Absyn_Int_p_struct*)_tmp5FC)->f1;_tmp607=((struct Cyc_Absyn_Int_p_struct*)
_tmp5FC)->f2;_LL321: s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_int_exp(_tmp606,_tmp607,0),
fail_stmt);goto _LL315;_LL322: if(_tmp5FC <= (void*)2)goto _LL324;if(*((int*)_tmp5FC)
!= 8)goto _LL324;_tmp608=((struct Cyc_Absyn_Char_p_struct*)_tmp5FC)->f1;_LL323: s=
Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_char_exp(_tmp608,0),fail_stmt);goto _LL315;_LL324:
if(_tmp5FC <= (void*)2)goto _LL326;if(*((int*)_tmp5FC)!= 9)goto _LL326;_tmp609=((
struct Cyc_Absyn_Float_p_struct*)_tmp5FC)->f1;_LL325: s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_float_exp(
_tmp609,0),fail_stmt);goto _LL315;_LL326: if(_tmp5FC <= (void*)2)goto _LL328;if(*((
int*)_tmp5FC)!= 10)goto _LL328;_tmp60A=((struct Cyc_Absyn_Enum_p_struct*)_tmp5FC)->f1;
_tmp60B=((struct Cyc_Absyn_Enum_p_struct*)_tmp5FC)->f2;_LL327: s=Cyc_Toc_if_neq_stmt(
r,Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Enum_e_struct*_tmp624=_cycalloc(
sizeof(*_tmp624));_tmp624[0]=({struct Cyc_Absyn_Enum_e_struct _tmp625;_tmp625.tag=
33;_tmp625.f1=_tmp60B->name;_tmp625.f2=(struct Cyc_Absyn_Enumdecl*)_tmp60A;
_tmp625.f3=(struct Cyc_Absyn_Enumfield*)_tmp60B;_tmp625;});_tmp624;}),0),
fail_stmt);goto _LL315;_LL328: if(_tmp5FC <= (void*)2)goto _LL32A;if(*((int*)_tmp5FC)
!= 11)goto _LL32A;_tmp60C=(void*)((struct Cyc_Absyn_AnonEnum_p_struct*)_tmp5FC)->f1;
_tmp60D=((struct Cyc_Absyn_AnonEnum_p_struct*)_tmp5FC)->f2;_LL329: s=Cyc_Toc_if_neq_stmt(
r,Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_AnonEnum_e_struct*_tmp626=_cycalloc(
sizeof(*_tmp626));_tmp626[0]=({struct Cyc_Absyn_AnonEnum_e_struct _tmp627;_tmp627.tag=
34;_tmp627.f1=_tmp60D->name;_tmp627.f2=(void*)_tmp60C;_tmp627.f3=(struct Cyc_Absyn_Enumfield*)
_tmp60D;_tmp627;});_tmp626;}),0),fail_stmt);goto _LL315;_LL32A: if(_tmp5FC <= (void*)
2)goto _LL32C;if(*((int*)_tmp5FC)!= 6)goto _LL32C;_tmp60E=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp5FC)->f1;_tmp60F=((struct Cyc_Absyn_Tunion_p_struct*)_tmp5FC)->f2;_tmp610=((
struct Cyc_Absyn_Tunion_p_struct*)_tmp5FC)->f3;if(_tmp610 != 0)goto _LL32C;if(!(!
_tmp60E->is_flat))goto _LL32C;_LL32B: {struct Cyc_Absyn_Exp*cmp_exp;if(_tmp60E->is_xtunion)
cmp_exp=Cyc_Absyn_var_exp(_tmp60F->name,0);else{cmp_exp=Cyc_Toc_tunion_tag(
_tmp60E,_tmp60F->name,0);r=Cyc_Toc_cast_it(Cyc_Absyn_sint_typ,r);}s=Cyc_Toc_if_neq_stmt(
r,cmp_exp,fail_stmt);goto _LL315;}_LL32C: if(_tmp5FC <= (void*)2)goto _LL32E;if(*((
int*)_tmp5FC)!= 4)goto _LL32E;_tmp611=((struct Cyc_Absyn_Pointer_p_struct*)_tmp5FC)->f1;
_tmp612=*_tmp611;_tmp613=(void*)_tmp612.r;if(_tmp613 <= (void*)2)goto _LL32E;if(*((
int*)_tmp613)!= 6)goto _LL32E;_tmp614=((struct Cyc_Absyn_Tunion_p_struct*)_tmp613)->f1;
_tmp615=((struct Cyc_Absyn_Tunion_p_struct*)_tmp613)->f2;_tmp616=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp613)->f3;if(!(_tmp616 != 0  && !_tmp614->is_flat))goto _LL32E;_LL32D: s=Cyc_Toc_skip_stmt_dl();{
int cnt=1;struct _tuple1*tufstrct=Cyc_Toc_collapse_qvar_tag(_tmp615->name,({const
char*_tmp634="_struct";_tag_dyneither(_tmp634,sizeof(char),8);}));struct Cyc_Absyn_Exp*
rcast=Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(Cyc_Absyn_strctq(tufstrct),Cyc_Toc_mt_tq),
r);struct Cyc_List_List*_tmp628=_tmp615->typs;for(0;_tmp616 != 0;(((_tmp616=
_tmp616->tl,_tmp628=((struct Cyc_List_List*)_check_null(_tmp628))->tl)),++ cnt)){
struct Cyc_Absyn_Pat*_tmp629=(struct Cyc_Absyn_Pat*)_tmp616->hd;if((void*)_tmp629->r
== (void*)0)continue;{void*_tmp62A=(*((struct _tuple4*)((struct Cyc_List_List*)
_check_null(_tmp628))->hd)).f2;struct _tuple1*_tmp62B=Cyc_Toc_temp_var();void*
_tmp62C=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp629->topt))->v;void*_tmp62D=
Cyc_Toc_typ_to_c(_tmp62C);struct Cyc_Absyn_Exp*_tmp62E=Cyc_Absyn_aggrarrow_exp(
rcast,Cyc_Absyn_fieldname(cnt),0);if(Cyc_Toc_is_void_star(Cyc_Toc_typ_to_c(
_tmp62A)))_tmp62E=Cyc_Toc_cast_it(_tmp62D,_tmp62E);decls=({struct Cyc_List_List*
_tmp62F=_region_malloc(rgn,sizeof(*_tmp62F));_tmp62F->hd=({struct _tuple14*
_tmp630=_region_malloc(rgn,sizeof(*_tmp630));_tmp630->f1=_tmp62B;_tmp630->f2=
_tmp62D;_tmp630;});_tmp62F->tl=decls;_tmp62F;});{struct _tuple13 _tmp631=Cyc_Toc_xlate_pat(
nv,rgn,_tmp62C,Cyc_Absyn_var_exp(_tmp62B,0),_tmp62E,_tmp629,(struct Cyc_Absyn_Stmt**)&
fail_stmt,fail_stmt,decls);nv=_tmp631.f1;decls=_tmp631.f2;{struct Cyc_Absyn_Stmt*
_tmp632=_tmp631.f3;struct Cyc_Absyn_Stmt*_tmp633=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(
_tmp62B,0),_tmp62E,0);s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_seq_stmt(_tmp633,_tmp632,
0),0);}}}}{struct Cyc_Absyn_Exp*test_exp;if(_tmp614->is_xtunion){struct Cyc_Absyn_Exp*
e2=Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(Cyc_Absyn_void_star_typ(),Cyc_Toc_mt_tq),
r);struct Cyc_Absyn_Exp*e1=Cyc_Absyn_deref_exp(e2,0);struct Cyc_Absyn_Exp*e=Cyc_Absyn_var_exp(
_tmp615->name,0);test_exp=Cyc_Absyn_neq_exp(e1,e,0);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(
test_exp,fail_stmt,Cyc_Toc_skip_stmt_dl(),0),s,0);}else{struct Cyc_Absyn_Exp*e3=
Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(Cyc_Absyn_sint_typ,Cyc_Toc_mt_tq),r);struct
Cyc_Absyn_Exp*e1=Cyc_Absyn_deref_exp(e3,0);struct Cyc_Absyn_Exp*e=Cyc_Toc_tunion_tag(
_tmp614,_tmp615->name,1);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_neq_exp(
e1,e,0),fail_stmt,Cyc_Toc_skip_stmt_dl(),0),s,0);if(tag_fail_stmt != 0){int
max_tag=Cyc_Toc_num_void_tags(_tmp614);if(max_tag != 0){struct Cyc_Absyn_Exp*
max_tag_exp=Cyc_Absyn_uint_exp((unsigned int)max_tag,0);struct Cyc_Absyn_Exp*e5=
Cyc_Absyn_lte_exp(r,Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),max_tag_exp),0);s=
Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(e5,*tag_fail_stmt,Cyc_Toc_skip_stmt_dl(),
0),s,0);}}}goto _LL315;}}_LL32E: if(_tmp5FC <= (void*)2)goto _LL330;if(*((int*)
_tmp5FC)!= 6)goto _LL330;_tmp617=((struct Cyc_Absyn_Tunion_p_struct*)_tmp5FC)->f1;
_tmp618=((struct Cyc_Absyn_Tunion_p_struct*)_tmp5FC)->f2;_tmp619=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp5FC)->f3;if(!_tmp617->is_flat)goto _LL330;_LL32F: {struct _tuple1 _tmp636;
struct _dyneither_ptr*_tmp637;struct _tuple1*_tmp635=_tmp618->name;_tmp636=*
_tmp635;_tmp637=_tmp636.f2;r=Cyc_Absyn_aggrmember_exp(r,_tmp637,0);path=Cyc_Absyn_aggrmember_exp(
path,_tmp637,0);s=Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_neq_exp(Cyc_Absyn_aggrmember_exp(
path,Cyc_Toc_tag_sp,0),Cyc_Toc_tunion_tag(_tmp617,_tmp618->name,1),0),fail_stmt,
Cyc_Toc_skip_stmt_dl(),0);{int cnt=1;for(0;_tmp619 != 0;(_tmp619=_tmp619->tl,++ cnt)){
struct Cyc_Absyn_Pat*_tmp638=(struct Cyc_Absyn_Pat*)_tmp619->hd;if((void*)_tmp638->r
== (void*)0)continue;{struct _tuple1*_tmp639=Cyc_Toc_temp_var();void*_tmp63A=(
void*)((struct Cyc_Core_Opt*)_check_null(_tmp638->topt))->v;decls=({struct Cyc_List_List*
_tmp63B=_region_malloc(rgn,sizeof(*_tmp63B));_tmp63B->hd=({struct _tuple14*
_tmp63C=_region_malloc(rgn,sizeof(*_tmp63C));_tmp63C->f1=_tmp639;_tmp63C->f2=Cyc_Toc_typ_to_c(
_tmp63A);_tmp63C;});_tmp63B->tl=decls;_tmp63B;});{struct _tuple13 _tmp63D=Cyc_Toc_xlate_pat(
nv,rgn,_tmp63A,Cyc_Absyn_var_exp(_tmp639,0),Cyc_Absyn_aggrmember_exp(path,Cyc_Absyn_fieldname(
cnt),0),_tmp638,(struct Cyc_Absyn_Stmt**)& fail_stmt,fail_stmt,decls);nv=_tmp63D.f1;
decls=_tmp63D.f2;{struct Cyc_Absyn_Stmt*_tmp63E=_tmp63D.f3;struct Cyc_Absyn_Stmt*
_tmp63F=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp639,0),Cyc_Absyn_aggrmember_exp(
r,Cyc_Absyn_fieldname(cnt),0),0);s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_seq_stmt(
_tmp63F,_tmp63E,0),0);}}}}goto _LL315;}}_LL330: if(_tmp5FC <= (void*)2)goto _LL332;
if(*((int*)_tmp5FC)!= 6)goto _LL332;_tmp61A=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp5FC)->f3;_LL331: _tmp61B=_tmp61A;goto _LL333;_LL332: if(_tmp5FC <= (void*)2)goto
_LL334;if(*((int*)_tmp5FC)!= 3)goto _LL334;_tmp61B=((struct Cyc_Absyn_Tuple_p_struct*)
_tmp5FC)->f1;_LL333: s=Cyc_Toc_skip_stmt_dl();{int cnt=1;for(0;_tmp61B != 0;(
_tmp61B=_tmp61B->tl,++ cnt)){struct Cyc_Absyn_Pat*_tmp640=(struct Cyc_Absyn_Pat*)
_tmp61B->hd;if((void*)_tmp640->r == (void*)0)continue;{struct _tuple1*_tmp641=Cyc_Toc_temp_var();
void*_tmp642=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp640->topt))->v;decls=({
struct Cyc_List_List*_tmp643=_region_malloc(rgn,sizeof(*_tmp643));_tmp643->hd=({
struct _tuple14*_tmp644=_region_malloc(rgn,sizeof(*_tmp644));_tmp644->f1=_tmp641;
_tmp644->f2=Cyc_Toc_typ_to_c(_tmp642);_tmp644;});_tmp643->tl=decls;_tmp643;});{
struct _tuple13 _tmp645=Cyc_Toc_xlate_pat(nv,rgn,_tmp642,Cyc_Absyn_var_exp(_tmp641,
0),Cyc_Absyn_aggrmember_exp(path,Cyc_Absyn_fieldname(cnt),0),_tmp640,(struct Cyc_Absyn_Stmt**)&
fail_stmt,fail_stmt,decls);nv=_tmp645.f1;decls=_tmp645.f2;{struct Cyc_Absyn_Stmt*
_tmp646=_tmp645.f3;struct Cyc_Absyn_Stmt*_tmp647=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(
_tmp641,0),Cyc_Absyn_aggrmember_exp(r,Cyc_Absyn_fieldname(cnt),0),0);s=Cyc_Absyn_seq_stmt(
s,Cyc_Absyn_seq_stmt(_tmp647,_tmp646,0),0);}}}}goto _LL315;}_LL334: if(_tmp5FC <= (
void*)2)goto _LL336;if(*((int*)_tmp5FC)!= 5)goto _LL336;_tmp61C=((struct Cyc_Absyn_Aggr_p_struct*)
_tmp5FC)->f1;_tmp61D=_tmp61C.aggr_info;_tmp61E=((struct Cyc_Absyn_Aggr_p_struct*)
_tmp5FC)->f3;_LL335: {struct Cyc_Absyn_Aggrdecl*_tmp648=Cyc_Absyn_get_known_aggrdecl(
_tmp61D);s=Cyc_Toc_skip_stmt_dl();for(0;_tmp61E != 0;_tmp61E=_tmp61E->tl){struct
_tuple15*_tmp649=(struct _tuple15*)_tmp61E->hd;struct Cyc_Absyn_Pat*_tmp64A=(*
_tmp649).f2;if((void*)_tmp64A->r == (void*)0)continue;{struct _dyneither_ptr*f;{
void*_tmp64B=(void*)((struct Cyc_List_List*)_check_null((*_tmp649).f1))->hd;
struct _dyneither_ptr*_tmp64C;_LL33F: if(*((int*)_tmp64B)!= 1)goto _LL341;_tmp64C=((
struct Cyc_Absyn_FieldName_struct*)_tmp64B)->f1;_LL340: f=_tmp64C;goto _LL33E;
_LL341:;_LL342:(int)_throw((void*)Cyc_Toc_Match_error);_LL33E:;}{struct _tuple1*
_tmp64D=Cyc_Toc_temp_var();void*_tmp64E=(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp64A->topt))->v;void*_tmp64F=Cyc_Toc_typ_to_c(_tmp64E);decls=({struct Cyc_List_List*
_tmp650=_region_malloc(rgn,sizeof(*_tmp650));_tmp650->hd=({struct _tuple14*
_tmp651=_region_malloc(rgn,sizeof(*_tmp651));_tmp651->f1=_tmp64D;_tmp651->f2=
_tmp64F;_tmp651;});_tmp650->tl=decls;_tmp650;});{struct _tuple13 _tmp652=Cyc_Toc_xlate_pat(
nv,rgn,_tmp64E,Cyc_Absyn_var_exp(_tmp64D,0),Cyc_Absyn_aggrmember_exp(path,f,0),
_tmp64A,(struct Cyc_Absyn_Stmt**)& fail_stmt,fail_stmt,decls);nv=_tmp652.f1;decls=
_tmp652.f2;{struct Cyc_Absyn_Exp*_tmp653=Cyc_Absyn_aggrmember_exp(r,f,0);if(Cyc_Toc_is_void_star((
void*)((struct Cyc_Absyn_Aggrfield*)_check_null(Cyc_Absyn_lookup_field(((struct
Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp648->impl))->fields,f)))->type))_tmp653=
Cyc_Toc_cast_it(_tmp64F,_tmp653);{struct Cyc_Absyn_Stmt*_tmp654=_tmp652.f3;struct
Cyc_Absyn_Stmt*_tmp655=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp64D,0),
_tmp653,0);s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_seq_stmt(_tmp655,_tmp654,0),0);}}}}}}
goto _LL315;}_LL336: if(_tmp5FC <= (void*)2)goto _LL338;if(*((int*)_tmp5FC)!= 4)goto
_LL338;_tmp61F=((struct Cyc_Absyn_Pointer_p_struct*)_tmp5FC)->f1;_LL337: {struct
_tuple1*_tmp656=Cyc_Toc_temp_var();void*_tmp657=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp61F->topt))->v;decls=({struct Cyc_List_List*_tmp658=
_region_malloc(rgn,sizeof(*_tmp658));_tmp658->hd=({struct _tuple14*_tmp659=
_region_malloc(rgn,sizeof(*_tmp659));_tmp659->f1=_tmp656;_tmp659->f2=Cyc_Toc_typ_to_c(
_tmp657);_tmp659;});_tmp658->tl=decls;_tmp658;});{struct _tuple13 _tmp65A=Cyc_Toc_xlate_pat(
nv,rgn,_tmp657,Cyc_Absyn_var_exp(_tmp656,0),Cyc_Absyn_deref_exp(path,0),_tmp61F,(
struct Cyc_Absyn_Stmt**)& fail_stmt,fail_stmt,decls);nv=_tmp65A.f1;decls=_tmp65A.f2;{
struct Cyc_Absyn_Stmt*_tmp65B=_tmp65A.f3;struct Cyc_Absyn_Stmt*_tmp65C=Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp656,0),Cyc_Absyn_deref_exp(r,0),0),
_tmp65B,0);if(Cyc_Toc_is_nullable(t))s=Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(
Cyc_Absyn_eq_exp(r,Cyc_Absyn_signed_int_exp(0,0),0),fail_stmt,Cyc_Toc_skip_stmt_dl(),
0),_tmp65C,0);else{s=_tmp65C;}goto _LL315;}}}_LL338: if(_tmp5FC <= (void*)2)goto
_LL33A;if(*((int*)_tmp5FC)!= 12)goto _LL33A;_LL339:({void*_tmp65D=0;((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp65E="unknownid pat";_tag_dyneither(_tmp65E,sizeof(char),14);}),
_tag_dyneither(_tmp65D,sizeof(void*),0));});_LL33A: if(_tmp5FC <= (void*)2)goto
_LL33C;if(*((int*)_tmp5FC)!= 13)goto _LL33C;_LL33B:({void*_tmp65F=0;((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp660="unknowncall pat";_tag_dyneither(_tmp660,sizeof(char),16);}),
_tag_dyneither(_tmp65F,sizeof(void*),0));});_LL33C: if(_tmp5FC <= (void*)2)goto
_LL315;if(*((int*)_tmp5FC)!= 14)goto _LL315;_LL33D:({void*_tmp661=0;((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp662="exp pat";_tag_dyneither(_tmp662,sizeof(char),8);}),_tag_dyneither(
_tmp661,sizeof(void*),0));});_LL315:;}return({struct _tuple13 _tmp663;_tmp663.f1=
nv;_tmp663.f2=decls;_tmp663.f3=s;_tmp663;});}struct _tuple16{struct _dyneither_ptr*
f1;struct _dyneither_ptr*f2;struct Cyc_Absyn_Switch_clause*f3;};static struct
_tuple16*Cyc_Toc_gen_label(struct _RegionHandle*r,struct Cyc_Absyn_Switch_clause*
sc){return({struct _tuple16*_tmp664=_region_malloc(r,sizeof(*_tmp664));_tmp664->f1=
Cyc_Toc_fresh_label();_tmp664->f2=Cyc_Toc_fresh_label();_tmp664->f3=sc;_tmp664;});}
static int Cyc_Toc_is_mixed_tunion(void*t){{void*_tmp665=Cyc_Tcutil_compress(t);
struct Cyc_Absyn_TunionInfo _tmp666;union Cyc_Absyn_TunionInfoU_union _tmp667;struct
Cyc_Absyn_Tuniondecl**_tmp668;struct Cyc_Absyn_Tuniondecl*_tmp669;_LL344: if(
_tmp665 <= (void*)4)goto _LL346;if(*((int*)_tmp665)!= 2)goto _LL346;_tmp666=((
struct Cyc_Absyn_TunionType_struct*)_tmp665)->f1;_tmp667=_tmp666.tunion_info;if((((((
struct Cyc_Absyn_TunionType_struct*)_tmp665)->f1).tunion_info).KnownTunion).tag != 
1)goto _LL346;_tmp668=(_tmp667.KnownTunion).f1;_tmp669=*_tmp668;if(!(!_tmp669->is_flat))
goto _LL346;_LL345: {int seen_novalue=0;int seen_value=0;{struct Cyc_List_List*
_tmp66A=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp669->fields))->v;
for(0;(unsigned int)_tmp66A;_tmp66A=_tmp66A->tl){if(((struct Cyc_Absyn_Tunionfield*)
_tmp66A->hd)->typs == 0)seen_value=1;else{seen_novalue=1;}if(seen_value  && 
seen_novalue)return 1;}}goto _LL343;}_LL346:;_LL347: goto _LL343;_LL343:;}return 0;}
static int Cyc_Toc_no_tag_test(struct Cyc_Absyn_Pat*p){void*_tmp66B=(void*)p->r;
struct Cyc_Absyn_Pat*_tmp66C;struct Cyc_Absyn_Pat*_tmp66D;struct Cyc_Absyn_Pat
_tmp66E;void*_tmp66F;struct Cyc_Absyn_Tuniondecl*_tmp670;struct Cyc_Absyn_Tunionfield*
_tmp671;struct Cyc_List_List*_tmp672;_LL349: if(_tmp66B <= (void*)2)goto _LL34D;if(*((
int*)_tmp66B)!= 0)goto _LL34B;_tmp66C=((struct Cyc_Absyn_Var_p_struct*)_tmp66B)->f2;
_LL34A: return Cyc_Toc_no_tag_test(_tmp66C);_LL34B: if(*((int*)_tmp66B)!= 4)goto
_LL34D;_tmp66D=((struct Cyc_Absyn_Pointer_p_struct*)_tmp66B)->f1;_tmp66E=*_tmp66D;
_tmp66F=(void*)_tmp66E.r;if(_tmp66F <= (void*)2)goto _LL34D;if(*((int*)_tmp66F)!= 
6)goto _LL34D;_tmp670=((struct Cyc_Absyn_Tunion_p_struct*)_tmp66F)->f1;_tmp671=((
struct Cyc_Absyn_Tunion_p_struct*)_tmp66F)->f2;_tmp672=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp66F)->f3;if(!(_tmp672 != 0  && !_tmp670->is_flat))goto _LL34D;_LL34C: return 0;
_LL34D:;_LL34E: return 1;_LL348:;}static void Cyc_Toc_xlate_switch(struct Cyc_Toc_Env*
nv,struct Cyc_Absyn_Stmt*whole_s,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*scs){
Cyc_Toc_exp_to_c(nv,e);{void*_tmp673=(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;
int leave_as_switch;{void*_tmp674=Cyc_Tcutil_compress(_tmp673);_LL350: if(_tmp674
<= (void*)4)goto _LL354;if(*((int*)_tmp674)!= 5)goto _LL352;_LL351: goto _LL353;
_LL352: if(*((int*)_tmp674)!= 12)goto _LL354;_LL353: leave_as_switch=1;goto _LL34F;
_LL354:;_LL355: leave_as_switch=0;goto _LL34F;_LL34F:;}{struct Cyc_List_List*
_tmp675=scs;for(0;_tmp675 != 0;_tmp675=_tmp675->tl){if((struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Switch_clause*)_tmp675->hd)->pat_vars))->v
!= 0  || ((struct Cyc_Absyn_Switch_clause*)_tmp675->hd)->where_clause != 0){
leave_as_switch=0;break;}}}if(leave_as_switch){struct _dyneither_ptr*next_l=Cyc_Toc_fresh_label();{
struct Cyc_List_List*_tmp676=scs;for(0;_tmp676 != 0;_tmp676=_tmp676->tl){struct Cyc_Absyn_Stmt*
_tmp677=((struct Cyc_Absyn_Switch_clause*)_tmp676->hd)->body;((struct Cyc_Absyn_Switch_clause*)
_tmp676->hd)->body=Cyc_Absyn_label_stmt(next_l,_tmp677,0);next_l=Cyc_Toc_fresh_label();{
struct Cyc_Toc_Env _tmp679;struct _RegionHandle*_tmp67A;struct Cyc_Toc_Env*_tmp678=
nv;_tmp679=*_tmp678;_tmp67A=_tmp679.rgn;Cyc_Toc_stmt_to_c(Cyc_Toc_switch_as_switch_env(
_tmp67A,nv,next_l),_tmp677);}}}return;}{struct _tuple1*v=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*r=Cyc_Absyn_var_exp(v,0);struct Cyc_Absyn_Exp*path=Cyc_Absyn_var_exp(
v,0);struct _dyneither_ptr*end_l=Cyc_Toc_fresh_label();struct Cyc_Toc_Env _tmp67C;
struct _RegionHandle*_tmp67D;struct Cyc_Toc_Env*_tmp67B=nv;_tmp67C=*_tmp67B;
_tmp67D=_tmp67C.rgn;{struct Cyc_Toc_Env*_tmp67E=Cyc_Toc_share_env(_tmp67D,nv);
struct Cyc_List_List*lscs=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct
_tuple16*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct
_RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(_tmp67D,Cyc_Toc_gen_label,
_tmp67D,scs);struct Cyc_List_List*test_stmts=0;struct Cyc_List_List*nvs=0;struct
Cyc_List_List*decls=0;int is_tunion=Cyc_Toc_is_mixed_tunion(_tmp673);int
needs_tag_test=is_tunion;{struct Cyc_List_List*_tmp67F=lscs;for(0;_tmp67F != 0;
_tmp67F=_tmp67F->tl){struct Cyc_Absyn_Switch_clause*sc=(*((struct _tuple16*)
_tmp67F->hd)).f3;struct _dyneither_ptr*fail_lab=_tmp67F->tl == 0?end_l:(*((struct
_tuple16*)((struct Cyc_List_List*)_check_null(_tmp67F->tl))->hd)).f1;struct Cyc_Absyn_Stmt**
tag_fail_stmt=0;if(needs_tag_test  && !Cyc_Toc_no_tag_test(sc->pattern)){{struct
Cyc_List_List*_tmp680=_tmp67F->tl;for(0;(unsigned int)_tmp680;_tmp680=_tmp680->tl){
if(Cyc_Toc_no_tag_test(((*((struct _tuple16*)_tmp680->hd)).f3)->pattern)){
tag_fail_stmt=({struct Cyc_Absyn_Stmt**_tmp681=_region_malloc(_tmp67D,sizeof(*
_tmp681));_tmp681[0]=Cyc_Absyn_goto_stmt((*((struct _tuple16*)_tmp680->hd)).f1,0);
_tmp681;});needs_tag_test=0;break;}}}if(tag_fail_stmt == 0)tag_fail_stmt=({struct
Cyc_Absyn_Stmt**_tmp682=_region_malloc(_tmp67D,sizeof(*_tmp682));_tmp682[0]=Cyc_Absyn_goto_stmt(
fail_lab,0);_tmp682;});}{struct Cyc_Toc_Env*_tmp684;struct Cyc_List_List*_tmp685;
struct Cyc_Absyn_Stmt*_tmp686;struct _tuple13 _tmp683=Cyc_Toc_xlate_pat(_tmp67E,
_tmp67D,_tmp673,r,path,sc->pattern,tag_fail_stmt,Cyc_Absyn_goto_stmt(fail_lab,0),
decls);_tmp684=_tmp683.f1;_tmp685=_tmp683.f2;_tmp686=_tmp683.f3;if(is_tunion  && 
Cyc_Toc_no_tag_test(sc->pattern))needs_tag_test=1;if(sc->where_clause != 0){
struct Cyc_Absyn_Exp*_tmp687=(struct Cyc_Absyn_Exp*)_check_null(sc->where_clause);
Cyc_Toc_exp_to_c(_tmp684,_tmp687);_tmp686=Cyc_Absyn_seq_stmt(_tmp686,Cyc_Absyn_ifthenelse_stmt(
Cyc_Absyn_prim1_exp((void*)11,_tmp687,0),Cyc_Absyn_goto_stmt(fail_lab,0),Cyc_Toc_skip_stmt_dl(),
0),0);}decls=_tmp685;nvs=({struct Cyc_List_List*_tmp688=_region_malloc(_tmp67D,
sizeof(*_tmp688));_tmp688->hd=_tmp684;_tmp688->tl=nvs;_tmp688;});test_stmts=({
struct Cyc_List_List*_tmp689=_region_malloc(_tmp67D,sizeof(*_tmp689));_tmp689->hd=
_tmp686;_tmp689->tl=test_stmts;_tmp689;});}}}nvs=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(nvs);test_stmts=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(test_stmts);{struct Cyc_List_List*stmts=
0;for(0;lscs != 0;(((lscs=lscs->tl,nvs=nvs->tl)),test_stmts=test_stmts->tl)){
struct _tuple16 _tmp68B;struct _dyneither_ptr*_tmp68C;struct _dyneither_ptr*_tmp68D;
struct Cyc_Absyn_Switch_clause*_tmp68E;struct _tuple16*_tmp68A=(struct _tuple16*)
lscs->hd;_tmp68B=*_tmp68A;_tmp68C=_tmp68B.f1;_tmp68D=_tmp68B.f2;_tmp68E=_tmp68B.f3;{
struct Cyc_Toc_Env*_tmp68F=(struct Cyc_Toc_Env*)((struct Cyc_List_List*)_check_null(
nvs))->hd;struct Cyc_Absyn_Stmt*s=_tmp68E->body;struct Cyc_Toc_Env _tmp691;struct
_RegionHandle*_tmp692;struct Cyc_Toc_Env*_tmp690=_tmp67E;_tmp691=*_tmp690;_tmp692=
_tmp691.rgn;if(lscs->tl != 0){struct _tuple16 _tmp694;struct _dyneither_ptr*_tmp695;
struct Cyc_Absyn_Switch_clause*_tmp696;struct _tuple16*_tmp693=(struct _tuple16*)((
struct Cyc_List_List*)_check_null(lscs->tl))->hd;_tmp694=*_tmp693;_tmp695=_tmp694.f2;
_tmp696=_tmp694.f3;Cyc_Toc_stmt_to_c(Cyc_Toc_non_last_switchclause_env(_tmp692,
_tmp68F,end_l,_tmp695,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(
_tmp696->pat_vars))->v,(struct Cyc_Toc_Env*)((struct Cyc_List_List*)_check_null(
nvs->tl))->hd),s);}else{Cyc_Toc_stmt_to_c(Cyc_Toc_last_switchclause_env(_tmp692,
_tmp68F,end_l),s);}s=Cyc_Absyn_seq_stmt(Cyc_Absyn_label_stmt(_tmp68C,(struct Cyc_Absyn_Stmt*)((
struct Cyc_List_List*)_check_null(test_stmts))->hd,0),Cyc_Absyn_label_stmt(
_tmp68D,s,0),0);stmts=({struct Cyc_List_List*_tmp697=_region_malloc(_tmp67D,
sizeof(*_tmp697));_tmp697->hd=s;_tmp697->tl=stmts;_tmp697;});}}{struct Cyc_Absyn_Stmt*
res=Cyc_Absyn_seq_stmt(Cyc_Absyn_seq_stmts(((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(stmts),0),Cyc_Absyn_label_stmt(end_l,Cyc_Toc_skip_stmt_dl(),
0),0);for(decls;decls != 0;decls=decls->tl){struct _tuple14 _tmp699;struct _tuple1*
_tmp69A;void*_tmp69B;struct _tuple14*_tmp698=(struct _tuple14*)decls->hd;_tmp699=*
_tmp698;_tmp69A=_tmp699.f1;_tmp69B=_tmp699.f2;res=Cyc_Absyn_declare_stmt(_tmp69A,
_tmp69B,0,res,0);}(void*)(whole_s->r=(void*)((void*)(Cyc_Absyn_declare_stmt(v,
Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),(struct
Cyc_Absyn_Exp*)e,res,0))->r));}}}}}}static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*p,void*t,struct Cyc_Absyn_Exp*e,struct
Cyc_Absyn_Stmt*s);static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*body_nv,
struct Cyc_Toc_Env*init_nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*s);
static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int
cinclude);struct Cyc_Absyn_Stmt*Cyc_Toc_make_npop_handler(int n){return Cyc_Absyn_exp_stmt(
Cyc_Absyn_fncall_exp(Cyc_Toc__npop_handler_e,({struct Cyc_List_List*_tmp69C=
_cycalloc(sizeof(*_tmp69C));_tmp69C->hd=Cyc_Absyn_uint_exp((unsigned int)(n - 1),
0);_tmp69C->tl=0;_tmp69C;}),0),0);}void Cyc_Toc_do_npop_before(int n,struct Cyc_Absyn_Stmt*
s){if(n > 0)(void*)(s->r=(void*)Cyc_Toc_seq_stmt_r(Cyc_Toc_make_npop_handler(n),
Cyc_Absyn_new_stmt((void*)s->r,0)));}static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*
nv,struct Cyc_Absyn_Stmt*s){while(1){void*_tmp69D=(void*)s->r;struct Cyc_Absyn_Exp*
_tmp69E;struct Cyc_Absyn_Stmt*_tmp69F;struct Cyc_Absyn_Stmt*_tmp6A0;struct Cyc_Absyn_Exp*
_tmp6A1;struct Cyc_Absyn_Exp*_tmp6A2;struct Cyc_Absyn_Stmt*_tmp6A3;struct Cyc_Absyn_Stmt*
_tmp6A4;struct _tuple3 _tmp6A5;struct Cyc_Absyn_Exp*_tmp6A6;struct Cyc_Absyn_Stmt*
_tmp6A7;struct Cyc_Absyn_Stmt*_tmp6A8;struct Cyc_Absyn_Stmt*_tmp6A9;struct Cyc_Absyn_Stmt*
_tmp6AA;struct Cyc_Absyn_Exp*_tmp6AB;struct _tuple3 _tmp6AC;struct Cyc_Absyn_Exp*
_tmp6AD;struct _tuple3 _tmp6AE;struct Cyc_Absyn_Exp*_tmp6AF;struct Cyc_Absyn_Stmt*
_tmp6B0;struct Cyc_Absyn_Exp*_tmp6B1;struct Cyc_List_List*_tmp6B2;struct Cyc_List_List*
_tmp6B3;struct Cyc_Absyn_Switch_clause**_tmp6B4;struct Cyc_Absyn_Decl*_tmp6B5;
struct Cyc_Absyn_Stmt*_tmp6B6;struct _dyneither_ptr*_tmp6B7;struct Cyc_Absyn_Stmt*
_tmp6B8;struct Cyc_Absyn_Stmt*_tmp6B9;struct _tuple3 _tmp6BA;struct Cyc_Absyn_Exp*
_tmp6BB;struct Cyc_Absyn_Stmt*_tmp6BC;struct Cyc_List_List*_tmp6BD;struct Cyc_Absyn_Exp*
_tmp6BE;_LL357: if((int)_tmp69D != 0)goto _LL359;_LL358: return;_LL359: if(_tmp69D <= (
void*)1)goto _LL35B;if(*((int*)_tmp69D)!= 0)goto _LL35B;_tmp69E=((struct Cyc_Absyn_Exp_s_struct*)
_tmp69D)->f1;_LL35A: Cyc_Toc_exp_to_c(nv,_tmp69E);return;_LL35B: if(_tmp69D <= (
void*)1)goto _LL35D;if(*((int*)_tmp69D)!= 1)goto _LL35D;_tmp69F=((struct Cyc_Absyn_Seq_s_struct*)
_tmp69D)->f1;_tmp6A0=((struct Cyc_Absyn_Seq_s_struct*)_tmp69D)->f2;_LL35C: Cyc_Toc_stmt_to_c(
nv,_tmp69F);s=_tmp6A0;continue;_LL35D: if(_tmp69D <= (void*)1)goto _LL35F;if(*((int*)
_tmp69D)!= 2)goto _LL35F;_tmp6A1=((struct Cyc_Absyn_Return_s_struct*)_tmp69D)->f1;
_LL35E: {struct Cyc_Core_Opt*topt=0;if(_tmp6A1 != 0){topt=({struct Cyc_Core_Opt*
_tmp6BF=_cycalloc(sizeof(*_tmp6BF));_tmp6BF->v=(void*)Cyc_Toc_typ_to_c((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp6A1->topt))->v);_tmp6BF;});Cyc_Toc_exp_to_c(
nv,(struct Cyc_Absyn_Exp*)_tmp6A1);}if(s->try_depth > 0){if(topt != 0){struct
_tuple1*_tmp6C0=Cyc_Toc_temp_var();struct Cyc_Absyn_Stmt*_tmp6C1=Cyc_Absyn_return_stmt((
struct Cyc_Absyn_Exp*)Cyc_Absyn_var_exp(_tmp6C0,0),0);(void*)(s->r=(void*)((void*)(
Cyc_Absyn_declare_stmt(_tmp6C0,(void*)topt->v,_tmp6A1,Cyc_Absyn_seq_stmt(Cyc_Toc_make_npop_handler(
s->try_depth),_tmp6C1,0),0))->r));}else{Cyc_Toc_do_npop_before(s->try_depth,s);}}
return;}_LL35F: if(_tmp69D <= (void*)1)goto _LL361;if(*((int*)_tmp69D)!= 3)goto
_LL361;_tmp6A2=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmp69D)->f1;_tmp6A3=((
struct Cyc_Absyn_IfThenElse_s_struct*)_tmp69D)->f2;_tmp6A4=((struct Cyc_Absyn_IfThenElse_s_struct*)
_tmp69D)->f3;_LL360: Cyc_Toc_exp_to_c(nv,_tmp6A2);Cyc_Toc_stmt_to_c(nv,_tmp6A3);s=
_tmp6A4;continue;_LL361: if(_tmp69D <= (void*)1)goto _LL363;if(*((int*)_tmp69D)!= 4)
goto _LL363;_tmp6A5=((struct Cyc_Absyn_While_s_struct*)_tmp69D)->f1;_tmp6A6=
_tmp6A5.f1;_tmp6A7=((struct Cyc_Absyn_While_s_struct*)_tmp69D)->f2;_LL362: Cyc_Toc_exp_to_c(
nv,_tmp6A6);{struct Cyc_Toc_Env _tmp6C3;struct _RegionHandle*_tmp6C4;struct Cyc_Toc_Env*
_tmp6C2=nv;_tmp6C3=*_tmp6C2;_tmp6C4=_tmp6C3.rgn;Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(
_tmp6C4,nv),_tmp6A7);return;}_LL363: if(_tmp69D <= (void*)1)goto _LL365;if(*((int*)
_tmp69D)!= 5)goto _LL365;_tmp6A8=((struct Cyc_Absyn_Break_s_struct*)_tmp69D)->f1;
_LL364: {struct Cyc_Toc_Env _tmp6C6;struct _dyneither_ptr**_tmp6C7;struct Cyc_Toc_Env*
_tmp6C5=nv;_tmp6C6=*_tmp6C5;_tmp6C7=_tmp6C6.break_lab;if(_tmp6C7 != 0)(void*)(s->r=(
void*)Cyc_Toc_goto_stmt_r(*_tmp6C7,0));{int dest_depth=_tmp6A8 == 0?0: _tmp6A8->try_depth;
Cyc_Toc_do_npop_before(s->try_depth - dest_depth,s);return;}}_LL365: if(_tmp69D <= (
void*)1)goto _LL367;if(*((int*)_tmp69D)!= 6)goto _LL367;_tmp6A9=((struct Cyc_Absyn_Continue_s_struct*)
_tmp69D)->f1;_LL366: {struct Cyc_Toc_Env _tmp6C9;struct _dyneither_ptr**_tmp6CA;
struct Cyc_Toc_Env*_tmp6C8=nv;_tmp6C9=*_tmp6C8;_tmp6CA=_tmp6C9.continue_lab;if(
_tmp6CA != 0)(void*)(s->r=(void*)Cyc_Toc_goto_stmt_r(*_tmp6CA,0));_tmp6AA=_tmp6A9;
goto _LL368;}_LL367: if(_tmp69D <= (void*)1)goto _LL369;if(*((int*)_tmp69D)!= 7)goto
_LL369;_tmp6AA=((struct Cyc_Absyn_Goto_s_struct*)_tmp69D)->f2;_LL368: Cyc_Toc_do_npop_before(
s->try_depth - ((struct Cyc_Absyn_Stmt*)_check_null(_tmp6AA))->try_depth,s);
return;_LL369: if(_tmp69D <= (void*)1)goto _LL36B;if(*((int*)_tmp69D)!= 8)goto
_LL36B;_tmp6AB=((struct Cyc_Absyn_For_s_struct*)_tmp69D)->f1;_tmp6AC=((struct Cyc_Absyn_For_s_struct*)
_tmp69D)->f2;_tmp6AD=_tmp6AC.f1;_tmp6AE=((struct Cyc_Absyn_For_s_struct*)_tmp69D)->f3;
_tmp6AF=_tmp6AE.f1;_tmp6B0=((struct Cyc_Absyn_For_s_struct*)_tmp69D)->f4;_LL36A:
Cyc_Toc_exp_to_c(nv,_tmp6AB);Cyc_Toc_exp_to_c(nv,_tmp6AD);Cyc_Toc_exp_to_c(nv,
_tmp6AF);{struct Cyc_Toc_Env _tmp6CC;struct _RegionHandle*_tmp6CD;struct Cyc_Toc_Env*
_tmp6CB=nv;_tmp6CC=*_tmp6CB;_tmp6CD=_tmp6CC.rgn;Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(
_tmp6CD,nv),_tmp6B0);return;}_LL36B: if(_tmp69D <= (void*)1)goto _LL36D;if(*((int*)
_tmp69D)!= 9)goto _LL36D;_tmp6B1=((struct Cyc_Absyn_Switch_s_struct*)_tmp69D)->f1;
_tmp6B2=((struct Cyc_Absyn_Switch_s_struct*)_tmp69D)->f2;_LL36C: Cyc_Toc_xlate_switch(
nv,s,_tmp6B1,_tmp6B2);return;_LL36D: if(_tmp69D <= (void*)1)goto _LL36F;if(*((int*)
_tmp69D)!= 10)goto _LL36F;_tmp6B3=((struct Cyc_Absyn_Fallthru_s_struct*)_tmp69D)->f1;
_tmp6B4=((struct Cyc_Absyn_Fallthru_s_struct*)_tmp69D)->f2;_LL36E: {struct Cyc_Toc_Env
_tmp6CF;struct Cyc_Toc_FallthruInfo*_tmp6D0;struct Cyc_Toc_Env*_tmp6CE=nv;_tmp6CF=*
_tmp6CE;_tmp6D0=_tmp6CF.fallthru_info;if(_tmp6D0 == 0)({void*_tmp6D1=0;((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp6D2="fallthru in unexpected place";_tag_dyneither(_tmp6D2,sizeof(char),29);}),
_tag_dyneither(_tmp6D1,sizeof(void*),0));});{struct _dyneither_ptr*_tmp6D4;struct
Cyc_List_List*_tmp6D5;struct Cyc_Dict_Dict _tmp6D6;struct Cyc_Toc_FallthruInfo
_tmp6D3=*_tmp6D0;_tmp6D4=_tmp6D3.label;_tmp6D5=_tmp6D3.binders;_tmp6D6=_tmp6D3.next_case_env;{
struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_goto_stmt(_tmp6D4,0);Cyc_Toc_do_npop_before(s->try_depth
- ((*((struct Cyc_Absyn_Switch_clause**)_check_null(_tmp6B4)))->body)->try_depth,
s2);{struct Cyc_List_List*_tmp6D7=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_rev)(_tmp6D5);struct Cyc_List_List*_tmp6D8=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(_tmp6B3);for(0;_tmp6D7 != 0;(_tmp6D7=_tmp6D7->tl,
_tmp6D8=_tmp6D8->tl)){Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp6D8))->hd);s2=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(((struct
Cyc_Absyn_Exp*(*)(struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup)(_tmp6D6,(
struct _tuple1*)_tmp6D7->hd),(struct Cyc_Absyn_Exp*)_tmp6D8->hd,0),s2,0);}(void*)(
s->r=(void*)((void*)s2->r));return;}}}}_LL36F: if(_tmp69D <= (void*)1)goto _LL371;
if(*((int*)_tmp69D)!= 11)goto _LL371;_tmp6B5=((struct Cyc_Absyn_Decl_s_struct*)
_tmp69D)->f1;_tmp6B6=((struct Cyc_Absyn_Decl_s_struct*)_tmp69D)->f2;_LL370:{void*
_tmp6D9=(void*)_tmp6B5->r;struct Cyc_Absyn_Vardecl*_tmp6DA;struct Cyc_Absyn_Pat*
_tmp6DB;struct Cyc_Absyn_Exp*_tmp6DC;struct Cyc_List_List*_tmp6DD;struct Cyc_Absyn_Fndecl*
_tmp6DE;struct Cyc_Absyn_Tvar*_tmp6DF;struct Cyc_Absyn_Vardecl*_tmp6E0;int _tmp6E1;
struct Cyc_Absyn_Exp*_tmp6E2;struct Cyc_Absyn_Exp*_tmp6E3;struct Cyc_Absyn_Tvar*
_tmp6E4;struct Cyc_Absyn_Vardecl*_tmp6E5;_LL37A: if(_tmp6D9 <= (void*)2)goto _LL386;
if(*((int*)_tmp6D9)!= 0)goto _LL37C;_tmp6DA=((struct Cyc_Absyn_Var_d_struct*)
_tmp6D9)->f1;_LL37B: {struct Cyc_Toc_Env _tmp6E7;struct _RegionHandle*_tmp6E8;
struct Cyc_Toc_Env*_tmp6E6=nv;_tmp6E7=*_tmp6E6;_tmp6E8=_tmp6E7.rgn;{struct Cyc_Toc_Env*
_tmp6E9=Cyc_Toc_add_varmap(_tmp6E8,nv,_tmp6DA->name,Cyc_Absyn_varb_exp(_tmp6DA->name,(
void*)({struct Cyc_Absyn_Local_b_struct*_tmp6EA=_cycalloc(sizeof(*_tmp6EA));
_tmp6EA[0]=({struct Cyc_Absyn_Local_b_struct _tmp6EB;_tmp6EB.tag=3;_tmp6EB.f1=
_tmp6DA;_tmp6EB;});_tmp6EA;}),0));Cyc_Toc_local_decl_to_c(_tmp6E9,_tmp6E9,
_tmp6DA,_tmp6B6);}goto _LL379;}_LL37C: if(*((int*)_tmp6D9)!= 2)goto _LL37E;_tmp6DB=((
struct Cyc_Absyn_Let_d_struct*)_tmp6D9)->f1;_tmp6DC=((struct Cyc_Absyn_Let_d_struct*)
_tmp6D9)->f3;_LL37D:{void*_tmp6EC=(void*)_tmp6DB->r;struct Cyc_Absyn_Vardecl*
_tmp6ED;struct Cyc_Absyn_Pat*_tmp6EE;struct Cyc_Absyn_Pat _tmp6EF;void*_tmp6F0;
_LL389: if(_tmp6EC <= (void*)2)goto _LL38B;if(*((int*)_tmp6EC)!= 0)goto _LL38B;
_tmp6ED=((struct Cyc_Absyn_Var_p_struct*)_tmp6EC)->f1;_tmp6EE=((struct Cyc_Absyn_Var_p_struct*)
_tmp6EC)->f2;_tmp6EF=*_tmp6EE;_tmp6F0=(void*)_tmp6EF.r;if((int)_tmp6F0 != 0)goto
_LL38B;_LL38A: {struct _tuple1*old_name=_tmp6ED->name;struct _tuple1*new_name=Cyc_Toc_temp_var();
_tmp6ED->name=new_name;_tmp6ED->initializer=(struct Cyc_Absyn_Exp*)_tmp6DC;(void*)(
_tmp6B5->r=(void*)((void*)({struct Cyc_Absyn_Var_d_struct*_tmp6F1=_cycalloc(
sizeof(*_tmp6F1));_tmp6F1[0]=({struct Cyc_Absyn_Var_d_struct _tmp6F2;_tmp6F2.tag=0;
_tmp6F2.f1=_tmp6ED;_tmp6F2;});_tmp6F1;})));{struct Cyc_Toc_Env _tmp6F4;struct
_RegionHandle*_tmp6F5;struct Cyc_Toc_Env*_tmp6F3=nv;_tmp6F4=*_tmp6F3;_tmp6F5=
_tmp6F4.rgn;{struct Cyc_Toc_Env*_tmp6F6=Cyc_Toc_add_varmap(_tmp6F5,nv,old_name,
Cyc_Absyn_varb_exp(new_name,(void*)({struct Cyc_Absyn_Local_b_struct*_tmp6F7=
_cycalloc(sizeof(*_tmp6F7));_tmp6F7[0]=({struct Cyc_Absyn_Local_b_struct _tmp6F8;
_tmp6F8.tag=3;_tmp6F8.f1=_tmp6ED;_tmp6F8;});_tmp6F7;}),0));Cyc_Toc_local_decl_to_c(
_tmp6F6,nv,_tmp6ED,_tmp6B6);}goto _LL388;}}_LL38B:;_LL38C:(void*)(s->r=(void*)((
void*)(Cyc_Toc_letdecl_to_c(nv,_tmp6DB,(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp6DC->topt))->v,_tmp6DC,_tmp6B6))->r));goto _LL388;_LL388:;}goto _LL379;_LL37E:
if(*((int*)_tmp6D9)!= 3)goto _LL380;_tmp6DD=((struct Cyc_Absyn_Letv_d_struct*)
_tmp6D9)->f1;_LL37F: {struct Cyc_List_List*_tmp6F9=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(_tmp6DD);if(_tmp6F9 == 0)({void*_tmp6FA=0;((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const
char*_tmp6FB="empty Letv_d";_tag_dyneither(_tmp6FB,sizeof(char),13);}),
_tag_dyneither(_tmp6FA,sizeof(void*),0));});(void*)(_tmp6B5->r=(void*)((void*)({
struct Cyc_Absyn_Var_d_struct*_tmp6FC=_cycalloc(sizeof(*_tmp6FC));_tmp6FC[0]=({
struct Cyc_Absyn_Var_d_struct _tmp6FD;_tmp6FD.tag=0;_tmp6FD.f1=(struct Cyc_Absyn_Vardecl*)
_tmp6F9->hd;_tmp6FD;});_tmp6FC;})));_tmp6F9=_tmp6F9->tl;for(0;_tmp6F9 != 0;
_tmp6F9=_tmp6F9->tl){struct Cyc_Absyn_Decl*_tmp6FE=Cyc_Absyn_new_decl((void*)({
struct Cyc_Absyn_Var_d_struct*_tmp6FF=_cycalloc(sizeof(*_tmp6FF));_tmp6FF[0]=({
struct Cyc_Absyn_Var_d_struct _tmp700;_tmp700.tag=0;_tmp700.f1=(struct Cyc_Absyn_Vardecl*)
_tmp6F9->hd;_tmp700;});_tmp6FF;}),0);(void*)(s->r=(void*)((void*)(Cyc_Absyn_decl_stmt(
_tmp6FE,Cyc_Absyn_new_stmt((void*)s->r,0),0))->r));}Cyc_Toc_stmt_to_c(nv,s);goto
_LL379;}_LL380: if(*((int*)_tmp6D9)!= 1)goto _LL382;_tmp6DE=((struct Cyc_Absyn_Fn_d_struct*)
_tmp6D9)->f1;_LL381: {struct _tuple1*_tmp701=_tmp6DE->name;struct Cyc_Toc_Env
_tmp703;struct _RegionHandle*_tmp704;struct Cyc_Toc_Env*_tmp702=nv;_tmp703=*
_tmp702;_tmp704=_tmp703.rgn;{struct Cyc_Toc_Env*_tmp705=Cyc_Toc_add_varmap(
_tmp704,nv,_tmp6DE->name,Cyc_Absyn_var_exp(_tmp701,0));Cyc_Toc_fndecl_to_c(
_tmp705,_tmp6DE,0);Cyc_Toc_stmt_to_c(_tmp705,_tmp6B6);}goto _LL379;}_LL382: if(*((
int*)_tmp6D9)!= 4)goto _LL384;_tmp6DF=((struct Cyc_Absyn_Region_d_struct*)_tmp6D9)->f1;
_tmp6E0=((struct Cyc_Absyn_Region_d_struct*)_tmp6D9)->f2;_tmp6E1=((struct Cyc_Absyn_Region_d_struct*)
_tmp6D9)->f3;_tmp6E2=((struct Cyc_Absyn_Region_d_struct*)_tmp6D9)->f4;_LL383: {
struct Cyc_Absyn_Stmt*_tmp706=_tmp6B6;void*rh_struct_typ=Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp);
void*rh_struct_ptr_typ=Cyc_Absyn_cstar_typ(rh_struct_typ,Cyc_Toc_mt_tq);struct
_tuple1*rh_var=Cyc_Toc_temp_var();struct _tuple1*x_var=_tmp6E0->name;struct Cyc_Absyn_Exp*
rh_exp=Cyc_Absyn_var_exp(rh_var,0);struct Cyc_Absyn_Exp*x_exp=Cyc_Absyn_var_exp(
x_var,0);struct Cyc_Toc_Env _tmp708;struct _RegionHandle*_tmp709;struct Cyc_Toc_Env*
_tmp707=nv;_tmp708=*_tmp707;_tmp709=_tmp708.rgn;Cyc_Toc_stmt_to_c(Cyc_Toc_add_varmap(
_tmp709,nv,x_var,x_exp),_tmp706);if(Cyc_Absyn_no_regions)(void*)(s->r=(void*)((
void*)(Cyc_Absyn_declare_stmt(x_var,rh_struct_ptr_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(
0,0),_tmp706,0))->r));else{if(_tmp6E2 == 0)(void*)(s->r=(void*)((void*)(Cyc_Absyn_declare_stmt(
rh_var,rh_struct_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_fncall_exp(Cyc_Toc__new_region_e,({
struct Cyc_List_List*_tmp70A=_cycalloc(sizeof(*_tmp70A));_tmp70A->hd=Cyc_Absyn_string_exp(
Cyc_Absynpp_qvar2string(x_var),0);_tmp70A->tl=0;_tmp70A;}),0),Cyc_Absyn_declare_stmt(
x_var,rh_struct_ptr_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(rh_exp,0),
Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__push_region_e,({
struct Cyc_Absyn_Exp*_tmp70B[1];_tmp70B[0]=x_exp;((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp70B,sizeof(struct Cyc_Absyn_Exp*),
1));}),0),0),Cyc_Absyn_seq_stmt(_tmp706,Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(
Cyc_Toc__pop_region_e,({struct Cyc_Absyn_Exp*_tmp70C[1];_tmp70C[0]=x_exp;((struct
Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp70C,
sizeof(struct Cyc_Absyn_Exp*),1));}),0),0),0),0),0),0))->r));else{Cyc_Toc_exp_to_c(
nv,(struct Cyc_Absyn_Exp*)_tmp6E2);(void*)(s->r=(void*)((void*)(Cyc_Absyn_declare_stmt(
rh_var,Cyc_Absyn_strct(Cyc_Toc__DynRegionFrame_sp),0,Cyc_Absyn_declare_stmt(
x_var,rh_struct_ptr_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_fncall_exp(Cyc_Toc__open_dynregion_e,({
struct Cyc_Absyn_Exp*_tmp70D[2];_tmp70D[1]=(struct Cyc_Absyn_Exp*)_tmp6E2;_tmp70D[
0]=Cyc_Absyn_address_exp(rh_exp,0);((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp70D,sizeof(struct Cyc_Absyn_Exp*),
2));}),0),Cyc_Absyn_seq_stmt(_tmp706,Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_dynregion_e,({
struct Cyc_Absyn_Exp*_tmp70E[1];_tmp70E[0]=x_exp;((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp70E,sizeof(struct Cyc_Absyn_Exp*),
1));}),0),0),0),0),0))->r));}}return;}_LL384: if(*((int*)_tmp6D9)!= 5)goto _LL386;
_tmp6E3=((struct Cyc_Absyn_Alias_d_struct*)_tmp6D9)->f1;_tmp6E4=((struct Cyc_Absyn_Alias_d_struct*)
_tmp6D9)->f2;_tmp6E5=((struct Cyc_Absyn_Alias_d_struct*)_tmp6D9)->f3;_LL385: {
struct _tuple1*old_name=_tmp6E5->name;struct _tuple1*new_name=Cyc_Toc_temp_var();
_tmp6E5->name=new_name;_tmp6E5->initializer=(struct Cyc_Absyn_Exp*)_tmp6E3;(void*)(
s->r=(void*)((void*)({struct Cyc_Absyn_Decl_s_struct*_tmp70F=_cycalloc(sizeof(*
_tmp70F));_tmp70F[0]=({struct Cyc_Absyn_Decl_s_struct _tmp710;_tmp710.tag=11;
_tmp710.f1=({struct Cyc_Absyn_Decl*_tmp711=_cycalloc(sizeof(*_tmp711));_tmp711->r=(
void*)((void*)({struct Cyc_Absyn_Var_d_struct*_tmp712=_cycalloc(sizeof(*_tmp712));
_tmp712[0]=({struct Cyc_Absyn_Var_d_struct _tmp713;_tmp713.tag=0;_tmp713.f1=
_tmp6E5;_tmp713;});_tmp712;}));_tmp711->loc=0;_tmp711;});_tmp710.f2=_tmp6B6;
_tmp710;});_tmp70F;})));{struct Cyc_Toc_Env _tmp715;struct _RegionHandle*_tmp716;
struct Cyc_Toc_Env*_tmp714=nv;_tmp715=*_tmp714;_tmp716=_tmp715.rgn;{struct Cyc_Toc_Env*
_tmp717=Cyc_Toc_add_varmap(_tmp716,nv,old_name,Cyc_Absyn_varb_exp(new_name,(void*)({
struct Cyc_Absyn_Local_b_struct*_tmp718=_cycalloc(sizeof(*_tmp718));_tmp718[0]=({
struct Cyc_Absyn_Local_b_struct _tmp719;_tmp719.tag=3;_tmp719.f1=_tmp6E5;_tmp719;});
_tmp718;}),0));Cyc_Toc_local_decl_to_c(_tmp717,nv,_tmp6E5,_tmp6B6);}return;}}
_LL386:;_LL387:({void*_tmp71A=0;((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp71B="bad nested declaration within function";
_tag_dyneither(_tmp71B,sizeof(char),39);}),_tag_dyneither(_tmp71A,sizeof(void*),
0));});_LL379:;}return;_LL371: if(_tmp69D <= (void*)1)goto _LL373;if(*((int*)
_tmp69D)!= 12)goto _LL373;_tmp6B7=((struct Cyc_Absyn_Label_s_struct*)_tmp69D)->f1;
_tmp6B8=((struct Cyc_Absyn_Label_s_struct*)_tmp69D)->f2;_LL372: s=_tmp6B8;
continue;_LL373: if(_tmp69D <= (void*)1)goto _LL375;if(*((int*)_tmp69D)!= 13)goto
_LL375;_tmp6B9=((struct Cyc_Absyn_Do_s_struct*)_tmp69D)->f1;_tmp6BA=((struct Cyc_Absyn_Do_s_struct*)
_tmp69D)->f2;_tmp6BB=_tmp6BA.f1;_LL374: {struct Cyc_Toc_Env _tmp71D;struct
_RegionHandle*_tmp71E;struct Cyc_Toc_Env*_tmp71C=nv;_tmp71D=*_tmp71C;_tmp71E=
_tmp71D.rgn;Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(_tmp71E,nv),_tmp6B9);Cyc_Toc_exp_to_c(
nv,_tmp6BB);return;}_LL375: if(_tmp69D <= (void*)1)goto _LL377;if(*((int*)_tmp69D)
!= 14)goto _LL377;_tmp6BC=((struct Cyc_Absyn_TryCatch_s_struct*)_tmp69D)->f1;
_tmp6BD=((struct Cyc_Absyn_TryCatch_s_struct*)_tmp69D)->f2;_LL376: {struct _tuple1*
h_var=Cyc_Toc_temp_var();struct _tuple1*e_var=Cyc_Toc_temp_var();struct _tuple1*
was_thrown_var=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*h_exp=Cyc_Absyn_var_exp(
h_var,0);struct Cyc_Absyn_Exp*e_exp=Cyc_Absyn_var_exp(e_var,0);struct Cyc_Absyn_Exp*
was_thrown_exp=Cyc_Absyn_var_exp(was_thrown_var,0);void*h_typ=Cyc_Absyn_strct(
Cyc_Toc__handler_cons_sp);void*e_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_exn_typ);void*
was_thrown_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_sint_typ);e_exp->topt=({struct Cyc_Core_Opt*
_tmp71F=_cycalloc(sizeof(*_tmp71F));_tmp71F->v=(void*)e_typ;_tmp71F;});{struct
Cyc_Toc_Env _tmp721;struct _RegionHandle*_tmp722;struct Cyc_Toc_Env*_tmp720=nv;
_tmp721=*_tmp720;_tmp722=_tmp721.rgn;{struct Cyc_Toc_Env*_tmp723=Cyc_Toc_add_varmap(
_tmp722,nv,e_var,e_exp);Cyc_Toc_stmt_to_c(_tmp723,_tmp6BC);{struct Cyc_Absyn_Stmt*
_tmp724=Cyc_Absyn_seq_stmt(_tmp6BC,Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_handler_e,
0,0),0),0);struct _tuple1*_tmp725=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp726=
Cyc_Absyn_var_exp(_tmp725,0);struct Cyc_Absyn_Vardecl*_tmp727=Cyc_Absyn_new_vardecl(
_tmp725,Cyc_Absyn_exn_typ,0);_tmp726->topt=({struct Cyc_Core_Opt*_tmp728=
_cycalloc(sizeof(*_tmp728));_tmp728->v=(void*)Cyc_Absyn_exn_typ;_tmp728;});{
struct Cyc_Absyn_Pat*_tmp729=({struct Cyc_Absyn_Pat*_tmp739=_cycalloc(sizeof(*
_tmp739));_tmp739->r=(void*)((void*)({struct Cyc_Absyn_Var_p_struct*_tmp73B=
_cycalloc(sizeof(*_tmp73B));_tmp73B[0]=({struct Cyc_Absyn_Var_p_struct _tmp73C;
_tmp73C.tag=0;_tmp73C.f1=_tmp727;_tmp73C.f2=({struct Cyc_Absyn_Pat*_tmp73D=
_cycalloc(sizeof(*_tmp73D));_tmp73D->r=(void*)((void*)0);_tmp73D->topt=({struct
Cyc_Core_Opt*_tmp73E=_cycalloc(sizeof(*_tmp73E));_tmp73E->v=(void*)Cyc_Absyn_exn_typ;
_tmp73E;});_tmp73D->loc=0;_tmp73D;});_tmp73C;});_tmp73B;}));_tmp739->topt=({
struct Cyc_Core_Opt*_tmp73A=_cycalloc(sizeof(*_tmp73A));_tmp73A->v=(void*)Cyc_Absyn_exn_typ;
_tmp73A;});_tmp739->loc=0;_tmp739;});struct Cyc_Absyn_Exp*_tmp72A=Cyc_Absyn_throw_exp(
_tmp726,0);_tmp72A->topt=({struct Cyc_Core_Opt*_tmp72B=_cycalloc(sizeof(*_tmp72B));
_tmp72B->v=(void*)((void*)0);_tmp72B;});{struct Cyc_Absyn_Stmt*_tmp72C=Cyc_Absyn_exp_stmt(
_tmp72A,0);struct Cyc_Absyn_Switch_clause*_tmp72D=({struct Cyc_Absyn_Switch_clause*
_tmp736=_cycalloc(sizeof(*_tmp736));_tmp736->pattern=_tmp729;_tmp736->pat_vars=({
struct Cyc_Core_Opt*_tmp737=_cycalloc(sizeof(*_tmp737));_tmp737->v=({struct Cyc_List_List*
_tmp738=_cycalloc(sizeof(*_tmp738));_tmp738->hd=_tmp727;_tmp738->tl=0;_tmp738;});
_tmp737;});_tmp736->where_clause=0;_tmp736->body=_tmp72C;_tmp736->loc=0;_tmp736;});
struct Cyc_Absyn_Stmt*_tmp72E=Cyc_Absyn_switch_stmt(e_exp,((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp6BD,({struct
Cyc_List_List*_tmp735=_cycalloc(sizeof(*_tmp735));_tmp735->hd=_tmp72D;_tmp735->tl=
0;_tmp735;})),0);Cyc_Toc_stmt_to_c(_tmp723,_tmp72E);{struct Cyc_Absyn_Exp*_tmp72F=
Cyc_Absyn_fncall_exp(Cyc_Toc_setjmp_e,({struct Cyc_List_List*_tmp734=_cycalloc(
sizeof(*_tmp734));_tmp734->hd=Cyc_Absyn_aggrmember_exp(h_exp,Cyc_Toc_handler_sp,
0);_tmp734->tl=0;_tmp734;}),0);struct Cyc_Absyn_Stmt*_tmp730=Cyc_Absyn_exp_stmt(
Cyc_Absyn_fncall_exp(Cyc_Toc__push_handler_e,({struct Cyc_List_List*_tmp733=
_cycalloc(sizeof(*_tmp733));_tmp733->hd=Cyc_Absyn_address_exp(h_exp,0);_tmp733->tl=
0;_tmp733;}),0),0);struct Cyc_Absyn_Exp*_tmp731=Cyc_Absyn_int_exp((void*)0,0,0);
struct Cyc_Absyn_Exp*_tmp732=Cyc_Absyn_int_exp((void*)0,1,0);(void*)(s->r=(void*)((
void*)(Cyc_Absyn_declare_stmt(h_var,h_typ,0,Cyc_Absyn_seq_stmt(_tmp730,Cyc_Absyn_declare_stmt(
was_thrown_var,was_thrown_typ,(struct Cyc_Absyn_Exp*)_tmp731,Cyc_Absyn_seq_stmt(
Cyc_Absyn_ifthenelse_stmt(_tmp72F,Cyc_Absyn_assign_stmt(was_thrown_exp,_tmp732,0),
Cyc_Toc_skip_stmt_dl(),0),Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_prim1_exp((void*)
11,was_thrown_exp,0),_tmp724,Cyc_Absyn_declare_stmt(e_var,e_typ,(struct Cyc_Absyn_Exp*)
Cyc_Toc_cast_it(e_typ,Cyc_Toc__exn_thrown_e),_tmp72E,0),0),0),0),0),0))->r));}}}}}
return;}}_LL377: if(_tmp69D <= (void*)1)goto _LL356;if(*((int*)_tmp69D)!= 15)goto
_LL356;_tmp6BE=((struct Cyc_Absyn_ResetRegion_s_struct*)_tmp69D)->f1;_LL378: if(
Cyc_Absyn_no_regions)(void*)(s->r=(void*)((void*)0));else{Cyc_Toc_exp_to_c(nv,
_tmp6BE);(void*)(s->r=(void*)Cyc_Toc_exp_stmt_r(Cyc_Absyn_fncall_exp(Cyc_Toc__reset_region_e,({
struct Cyc_List_List*_tmp73F=_cycalloc(sizeof(*_tmp73F));_tmp73F->hd=_tmp6BE;
_tmp73F->tl=0;_tmp73F;}),0)));}return;_LL356:;}}static void Cyc_Toc_stmttypes_to_c(
struct Cyc_Absyn_Stmt*s);struct _tuple17{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual
f2;void*f3;};static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*
f,int cinclude){f->tvs=0;f->effect=0;f->rgn_po=0;(void*)(f->ret_type=(void*)Cyc_Toc_typ_to_c((
void*)f->ret_type));{struct _RegionHandle _tmp740=_new_region("frgn");struct
_RegionHandle*frgn=& _tmp740;_push_region(frgn);{struct Cyc_Toc_Env*_tmp741=Cyc_Toc_share_env(
frgn,nv);{struct Cyc_List_List*_tmp742=f->args;for(0;_tmp742 != 0;_tmp742=_tmp742->tl){
struct _tuple1*_tmp743=({struct _tuple1*_tmp744=_cycalloc(sizeof(*_tmp744));
_tmp744->f1=(union Cyc_Absyn_Nmspace_union)((union Cyc_Absyn_Nmspace_union)({union
Cyc_Absyn_Nmspace_union _tmp745;(_tmp745.Loc_n).tag=0;_tmp745;}));_tmp744->f2=(*((
struct _tuple17*)_tmp742->hd)).f1;_tmp744;});(*((struct _tuple17*)_tmp742->hd)).f3=
Cyc_Toc_typ_to_c((*((struct _tuple17*)_tmp742->hd)).f3);_tmp741=Cyc_Toc_add_varmap(
frgn,_tmp741,_tmp743,Cyc_Absyn_var_exp(_tmp743,0));}}if(cinclude){Cyc_Toc_stmttypes_to_c(
f->body);_npop_handler(0);return;}if((unsigned int)f->cyc_varargs  && ((struct Cyc_Absyn_VarargInfo*)
_check_null(f->cyc_varargs))->name != 0){struct Cyc_Core_Opt*_tmp747;struct Cyc_Absyn_Tqual
_tmp748;void*_tmp749;int _tmp74A;struct Cyc_Absyn_VarargInfo _tmp746=*((struct Cyc_Absyn_VarargInfo*)
_check_null(f->cyc_varargs));_tmp747=_tmp746.name;_tmp748=_tmp746.tq;_tmp749=(
void*)_tmp746.type;_tmp74A=_tmp746.inject;{void*_tmp74B=Cyc_Toc_typ_to_c(Cyc_Absyn_dyneither_typ(
_tmp749,(void*)2,_tmp748,Cyc_Absyn_false_conref));struct _tuple1*_tmp74C=({struct
_tuple1*_tmp74F=_cycalloc(sizeof(*_tmp74F));_tmp74F->f1=(union Cyc_Absyn_Nmspace_union)((
union Cyc_Absyn_Nmspace_union)({union Cyc_Absyn_Nmspace_union _tmp750;(_tmp750.Loc_n).tag=
0;_tmp750;}));_tmp74F->f2=(struct _dyneither_ptr*)((struct Cyc_Core_Opt*)
_check_null(_tmp747))->v;_tmp74F;});f->args=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_append)(f->args,({struct Cyc_List_List*_tmp74D=
_cycalloc(sizeof(*_tmp74D));_tmp74D->hd=({struct _tuple17*_tmp74E=_cycalloc(
sizeof(*_tmp74E));_tmp74E->f1=(struct _dyneither_ptr*)_tmp747->v;_tmp74E->f2=
_tmp748;_tmp74E->f3=_tmp74B;_tmp74E;});_tmp74D->tl=0;_tmp74D;}));_tmp741=Cyc_Toc_add_varmap(
frgn,_tmp741,_tmp74C,Cyc_Absyn_var_exp(_tmp74C,0));f->cyc_varargs=0;}}{struct Cyc_List_List*
_tmp751=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(f->param_vardecls))->v;
for(0;_tmp751 != 0;_tmp751=_tmp751->tl){(void*)(((struct Cyc_Absyn_Vardecl*)
_tmp751->hd)->type=(void*)Cyc_Toc_typ_to_c((void*)((struct Cyc_Absyn_Vardecl*)
_tmp751->hd)->type));}}Cyc_Toc_stmt_to_c(Cyc_Toc_clear_toplevel(frgn,_tmp741),f->body);};
_pop_region(frgn);}}static void*Cyc_Toc_scope_to_c(void*s){void*_tmp752=s;_LL38E:
if((int)_tmp752 != 1)goto _LL390;_LL38F: return(void*)2;_LL390: if((int)_tmp752 != 4)
goto _LL392;_LL391: return(void*)3;_LL392:;_LL393: return s;_LL38D:;}static void Cyc_Toc_aggrdecl_to_c(
struct Cyc_Absyn_Aggrdecl*ad){struct _tuple1*_tmp753=ad->name;struct
_DynRegionHandle*_tmp755;struct Cyc_Dict_Dict*_tmp756;struct Cyc_Toc_TocState
_tmp754=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp755=
_tmp754.dyn;_tmp756=_tmp754.aggrs_so_far;{struct _DynRegionFrame _tmp757;struct
_RegionHandle*d=_open_dynregion(& _tmp757,_tmp755);{int seen_defn_before;struct
_tuple7**_tmp758=((struct _tuple7**(*)(struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup_opt)(*
_tmp756,_tmp753);if(_tmp758 == 0){seen_defn_before=0;{struct _tuple7*v;if((void*)
ad->kind == (void*)0)v=({struct _tuple7*_tmp759=_region_malloc(d,sizeof(*_tmp759));
_tmp759->f1=ad;_tmp759->f2=Cyc_Absyn_strctq(ad->name);_tmp759;});else{v=({struct
_tuple7*_tmp75A=_region_malloc(d,sizeof(*_tmp75A));_tmp75A->f1=ad;_tmp75A->f2=
Cyc_Absyn_unionq_typ(ad->name);_tmp75A;});}*_tmp756=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,struct _tuple1*k,struct _tuple7*v))Cyc_Dict_insert)(*_tmp756,
_tmp753,v);}}else{struct _tuple7 _tmp75C;struct Cyc_Absyn_Aggrdecl*_tmp75D;void*
_tmp75E;struct _tuple7*_tmp75B=*_tmp758;_tmp75C=*_tmp75B;_tmp75D=_tmp75C.f1;
_tmp75E=_tmp75C.f2;if(_tmp75D->impl == 0){*_tmp756=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,struct _tuple1*k,struct _tuple7*v))Cyc_Dict_insert)(*_tmp756,
_tmp753,({struct _tuple7*_tmp75F=_region_malloc(d,sizeof(*_tmp75F));_tmp75F->f1=
ad;_tmp75F->f2=_tmp75E;_tmp75F;}));seen_defn_before=0;}else{seen_defn_before=1;}}(
void*)(ad->sc=(void*)((void*)2));ad->tvs=0;if(ad->impl != 0){((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(ad->impl))->exist_vars=0;((struct Cyc_Absyn_AggrdeclImpl*)_check_null(
ad->impl))->rgn_po=0;if(seen_defn_before)ad->impl=0;else{struct Cyc_List_List*
_tmp760=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;for(0;
_tmp760 != 0;_tmp760=_tmp760->tl){((struct Cyc_Absyn_Aggrfield*)_tmp760->hd)->tq=
Cyc_Toc_mt_tq;(void*)(((struct Cyc_Absyn_Aggrfield*)_tmp760->hd)->type=(void*)Cyc_Toc_typ_to_c_array((
void*)((struct Cyc_Absyn_Aggrfield*)_tmp760->hd)->type));}}}};_pop_dynregion(d);}}
static void Cyc_Toc_tuniondecl_to_c(struct Cyc_Absyn_Tuniondecl*tud){struct
_DynRegionHandle*_tmp762;struct Cyc_Set_Set**_tmp763;struct Cyc_Toc_TocState
_tmp761=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp762=
_tmp761.dyn;_tmp763=_tmp761.tunions_so_far;{struct _DynRegionFrame _tmp764;struct
_RegionHandle*d=_open_dynregion(& _tmp764,_tmp762);{struct _tuple1*_tmp765=tud->name;
if(tud->fields == 0  || ((int(*)(struct Cyc_Set_Set*s,struct _tuple1*elt))Cyc_Set_member)(*
_tmp763,_tmp765)){_npop_handler(0);return;}*_tmp763=((struct Cyc_Set_Set*(*)(
struct _RegionHandle*r,struct Cyc_Set_Set*s,struct _tuple1*elt))Cyc_Set_rinsert)(d,*
_tmp763,_tmp765);}{struct Cyc_List_List*flat_structs=0;{struct Cyc_List_List*
_tmp766=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(tud->fields))->v;
for(0;_tmp766 != 0;_tmp766=_tmp766->tl){struct Cyc_Absyn_Tunionfield*f=(struct Cyc_Absyn_Tunionfield*)
_tmp766->hd;if(f->typs != 0  || tud->is_flat){struct Cyc_List_List*_tmp767=0;int i=1;{
struct Cyc_List_List*_tmp768=f->typs;for(0;_tmp768 != 0;(_tmp768=_tmp768->tl,i ++)){
struct _dyneither_ptr*_tmp769=Cyc_Absyn_fieldname(i);struct Cyc_Absyn_Aggrfield*
_tmp76A=({struct Cyc_Absyn_Aggrfield*_tmp76C=_cycalloc(sizeof(*_tmp76C));_tmp76C->name=
_tmp769;_tmp76C->tq=(*((struct _tuple4*)_tmp768->hd)).f1;_tmp76C->type=(void*)Cyc_Toc_typ_to_c_array((*((
struct _tuple4*)_tmp768->hd)).f2);_tmp76C->width=0;_tmp76C->attributes=0;_tmp76C;});
_tmp767=({struct Cyc_List_List*_tmp76B=_cycalloc(sizeof(*_tmp76B));_tmp76B->hd=
_tmp76A;_tmp76B->tl=_tmp767;_tmp76B;});}}_tmp767=({struct Cyc_List_List*_tmp76D=
_cycalloc(sizeof(*_tmp76D));_tmp76D->hd=({struct Cyc_Absyn_Aggrfield*_tmp76E=
_cycalloc(sizeof(*_tmp76E));_tmp76E->name=Cyc_Toc_tag_sp;_tmp76E->tq=Cyc_Toc_mt_tq;
_tmp76E->type=(void*)Cyc_Absyn_sint_typ;_tmp76E->width=0;_tmp76E->attributes=0;
_tmp76E;});_tmp76D->tl=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmp767);_tmp76D;});{struct Cyc_Absyn_Aggrdecl*_tmp76F=({struct Cyc_Absyn_Aggrdecl*
_tmp77B=_cycalloc(sizeof(*_tmp77B));_tmp77B->kind=(void*)((void*)0);_tmp77B->sc=(
void*)((void*)2);_tmp77B->name=Cyc_Toc_collapse_qvar_tag(f->name,({const char*
_tmp77D="_struct";_tag_dyneither(_tmp77D,sizeof(char),8);}));_tmp77B->tvs=0;
_tmp77B->impl=({struct Cyc_Absyn_AggrdeclImpl*_tmp77C=_cycalloc(sizeof(*_tmp77C));
_tmp77C->exist_vars=0;_tmp77C->rgn_po=0;_tmp77C->fields=_tmp767;_tmp77C;});
_tmp77B->attributes=0;_tmp77B;});Cyc_Toc_result_decls=({struct Cyc_List_List*
_tmp770=_cycalloc(sizeof(*_tmp770));_tmp770->hd=Cyc_Absyn_new_decl((void*)({
struct Cyc_Absyn_Aggr_d_struct*_tmp771=_cycalloc(sizeof(*_tmp771));_tmp771[0]=({
struct Cyc_Absyn_Aggr_d_struct _tmp772;_tmp772.tag=6;_tmp772.f1=_tmp76F;_tmp772;});
_tmp771;}),0);_tmp770->tl=Cyc_Toc_result_decls;_tmp770;});if(tud->is_flat){
struct Cyc_Absyn_Aggrfield*_tmp773=({struct Cyc_Absyn_Aggrfield*_tmp775=_cycalloc(
sizeof(*_tmp775));_tmp775->name=(*f->name).f2;_tmp775->tq=Cyc_Toc_mt_tq;_tmp775->type=(
void*)((void*)({struct Cyc_Absyn_AggrType_struct*_tmp776=_cycalloc(sizeof(*
_tmp776));_tmp776[0]=({struct Cyc_Absyn_AggrType_struct _tmp777;_tmp777.tag=10;
_tmp777.f1=({struct Cyc_Absyn_AggrInfo _tmp778;_tmp778.aggr_info=(union Cyc_Absyn_AggrInfoU_union)({
union Cyc_Absyn_AggrInfoU_union _tmp779;(_tmp779.KnownAggr).tag=1;(_tmp779.KnownAggr).f1=({
struct Cyc_Absyn_Aggrdecl**_tmp77A=_cycalloc(sizeof(*_tmp77A));_tmp77A[0]=_tmp76F;
_tmp77A;});_tmp779;});_tmp778.targs=0;_tmp778;});_tmp777;});_tmp776;}));_tmp775->width=
0;_tmp775->attributes=0;_tmp775;});flat_structs=({struct Cyc_List_List*_tmp774=
_cycalloc(sizeof(*_tmp774));_tmp774->hd=_tmp773;_tmp774->tl=flat_structs;_tmp774;});}}}}}
if(tud->is_flat){flat_structs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(flat_structs);{struct Cyc_Absyn_Aggrdecl*_tmp77E=({struct Cyc_Absyn_Aggrdecl*
_tmp782=_cycalloc(sizeof(*_tmp782));_tmp782->kind=(void*)((void*)1);_tmp782->sc=(
void*)((void*)2);_tmp782->name=Cyc_Toc_collapse_qvar_tag(tud->name,({const char*
_tmp784="_union";_tag_dyneither(_tmp784,sizeof(char),7);}));_tmp782->tvs=0;
_tmp782->impl=({struct Cyc_Absyn_AggrdeclImpl*_tmp783=_cycalloc(sizeof(*_tmp783));
_tmp783->exist_vars=0;_tmp783->rgn_po=0;_tmp783->fields=flat_structs;_tmp783;});
_tmp782->attributes=0;_tmp782;});Cyc_Toc_result_decls=({struct Cyc_List_List*
_tmp77F=_cycalloc(sizeof(*_tmp77F));_tmp77F->hd=Cyc_Absyn_new_decl((void*)({
struct Cyc_Absyn_Aggr_d_struct*_tmp780=_cycalloc(sizeof(*_tmp780));_tmp780[0]=({
struct Cyc_Absyn_Aggr_d_struct _tmp781;_tmp781.tag=6;_tmp781.f1=_tmp77E;_tmp781;});
_tmp780;}),0);_tmp77F->tl=Cyc_Toc_result_decls;_tmp77F;});}}};_pop_dynregion(d);}}
static void Cyc_Toc_xtuniondecl_to_c(struct Cyc_Absyn_Tuniondecl*xd){if(xd->fields
== 0)return;{struct _DynRegionHandle*_tmp786;struct Cyc_Dict_Dict*_tmp787;struct
Cyc_Toc_TocState _tmp785=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));
_tmp786=_tmp785.dyn;_tmp787=_tmp785.xtunions_so_far;{struct _DynRegionFrame
_tmp788;struct _RegionHandle*d=_open_dynregion(& _tmp788,_tmp786);{struct _tuple1*
_tmp789=xd->name;struct Cyc_List_List*_tmp78A=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(xd->fields))->v;for(0;_tmp78A != 0;_tmp78A=_tmp78A->tl){struct Cyc_Absyn_Tunionfield*
f=(struct Cyc_Absyn_Tunionfield*)_tmp78A->hd;struct _dyneither_ptr*fn=(*f->name).f2;
struct Cyc_Absyn_Exp*_tmp78B=Cyc_Absyn_uint_exp(_get_dyneither_size(*fn,sizeof(
char))+ 4,0);void*_tmp78C=Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq,(
struct Cyc_Absyn_Exp*)_tmp78B,Cyc_Absyn_false_conref,0);int*_tmp78D=((int*(*)(
struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup_opt)(*_tmp787,f->name);int
_tmp78E;_LL395: if(_tmp78D != 0)goto _LL397;_LL396: {struct Cyc_Absyn_Exp*initopt=0;
if((void*)f->sc != (void*)3){char zero='\000';initopt=(struct Cyc_Absyn_Exp*)Cyc_Absyn_string_exp((
struct _dyneither_ptr)({struct Cyc_String_pa_struct _tmp795;_tmp795.tag=0;_tmp795.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*fn);{struct Cyc_Int_pa_struct _tmp794;
_tmp794.tag=1;_tmp794.f1=(unsigned long)((int)zero);{struct Cyc_Int_pa_struct
_tmp793;_tmp793.tag=1;_tmp793.f1=(unsigned long)((int)zero);{struct Cyc_Int_pa_struct
_tmp792;_tmp792.tag=1;_tmp792.f1=(unsigned long)((int)zero);{struct Cyc_Int_pa_struct
_tmp791;_tmp791.tag=1;_tmp791.f1=(unsigned long)((int)zero);{void*_tmp78F[5]={&
_tmp791,& _tmp792,& _tmp793,& _tmp794,& _tmp795};Cyc_aprintf(({const char*_tmp790="%c%c%c%c%s";
_tag_dyneither(_tmp790,sizeof(char),11);}),_tag_dyneither(_tmp78F,sizeof(void*),
5));}}}}}}),0);}{struct Cyc_Absyn_Vardecl*_tmp796=Cyc_Absyn_new_vardecl(f->name,
_tmp78C,initopt);(void*)(_tmp796->sc=(void*)((void*)f->sc));Cyc_Toc_result_decls=({
struct Cyc_List_List*_tmp797=_cycalloc(sizeof(*_tmp797));_tmp797->hd=Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Var_d_struct*_tmp798=_cycalloc(sizeof(*_tmp798));_tmp798[
0]=({struct Cyc_Absyn_Var_d_struct _tmp799;_tmp799.tag=0;_tmp799.f1=_tmp796;
_tmp799;});_tmp798;}),0);_tmp797->tl=Cyc_Toc_result_decls;_tmp797;});*_tmp787=((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,int v))Cyc_Dict_insert)(*
_tmp787,f->name,(void*)f->sc != (void*)3);if(f->typs != 0){struct Cyc_List_List*
fields=0;int i=1;{struct Cyc_List_List*_tmp79A=f->typs;for(0;_tmp79A != 0;(_tmp79A=
_tmp79A->tl,i ++)){struct _dyneither_ptr*_tmp79B=({struct _dyneither_ptr*_tmp79F=
_cycalloc(sizeof(*_tmp79F));_tmp79F[0]=(struct _dyneither_ptr)({struct Cyc_Int_pa_struct
_tmp7A2;_tmp7A2.tag=1;_tmp7A2.f1=(unsigned long)i;{void*_tmp7A0[1]={& _tmp7A2};
Cyc_aprintf(({const char*_tmp7A1="f%d";_tag_dyneither(_tmp7A1,sizeof(char),4);}),
_tag_dyneither(_tmp7A0,sizeof(void*),1));}});_tmp79F;});struct Cyc_Absyn_Aggrfield*
_tmp79C=({struct Cyc_Absyn_Aggrfield*_tmp79E=_cycalloc(sizeof(*_tmp79E));_tmp79E->name=
_tmp79B;_tmp79E->tq=(*((struct _tuple4*)_tmp79A->hd)).f1;_tmp79E->type=(void*)Cyc_Toc_typ_to_c_array((*((
struct _tuple4*)_tmp79A->hd)).f2);_tmp79E->width=0;_tmp79E->attributes=0;_tmp79E;});
fields=({struct Cyc_List_List*_tmp79D=_cycalloc(sizeof(*_tmp79D));_tmp79D->hd=
_tmp79C;_tmp79D->tl=fields;_tmp79D;});}}fields=({struct Cyc_List_List*_tmp7A3=
_cycalloc(sizeof(*_tmp7A3));_tmp7A3->hd=({struct Cyc_Absyn_Aggrfield*_tmp7A4=
_cycalloc(sizeof(*_tmp7A4));_tmp7A4->name=Cyc_Toc_tag_sp;_tmp7A4->tq=Cyc_Toc_mt_tq;
_tmp7A4->type=(void*)Cyc_Absyn_cstar_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq);
_tmp7A4->width=0;_tmp7A4->attributes=0;_tmp7A4;});_tmp7A3->tl=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(fields);_tmp7A3;});{struct Cyc_Absyn_Aggrdecl*
_tmp7A5=({struct Cyc_Absyn_Aggrdecl*_tmp7A9=_cycalloc(sizeof(*_tmp7A9));_tmp7A9->kind=(
void*)((void*)0);_tmp7A9->sc=(void*)((void*)2);_tmp7A9->name=Cyc_Toc_collapse_qvar_tag(
f->name,({const char*_tmp7AB="_struct";_tag_dyneither(_tmp7AB,sizeof(char),8);}));
_tmp7A9->tvs=0;_tmp7A9->impl=({struct Cyc_Absyn_AggrdeclImpl*_tmp7AA=_cycalloc(
sizeof(*_tmp7AA));_tmp7AA->exist_vars=0;_tmp7AA->rgn_po=0;_tmp7AA->fields=fields;
_tmp7AA;});_tmp7A9->attributes=0;_tmp7A9;});Cyc_Toc_result_decls=({struct Cyc_List_List*
_tmp7A6=_cycalloc(sizeof(*_tmp7A6));_tmp7A6->hd=Cyc_Absyn_new_decl((void*)({
struct Cyc_Absyn_Aggr_d_struct*_tmp7A7=_cycalloc(sizeof(*_tmp7A7));_tmp7A7[0]=({
struct Cyc_Absyn_Aggr_d_struct _tmp7A8;_tmp7A8.tag=6;_tmp7A8.f1=_tmp7A5;_tmp7A8;});
_tmp7A7;}),0);_tmp7A6->tl=Cyc_Toc_result_decls;_tmp7A6;});}}goto _LL394;}}_LL397:
if(_tmp78D == 0)goto _LL399;_tmp78E=*_tmp78D;if(_tmp78E != 0)goto _LL399;_LL398: if((
void*)f->sc != (void*)3){char zero='\000';struct Cyc_Absyn_Exp*_tmp7AC=Cyc_Absyn_string_exp((
struct _dyneither_ptr)({struct Cyc_String_pa_struct _tmp7B7;_tmp7B7.tag=0;_tmp7B7.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*fn);{struct Cyc_Int_pa_struct _tmp7B6;
_tmp7B6.tag=1;_tmp7B6.f1=(unsigned long)((int)zero);{struct Cyc_Int_pa_struct
_tmp7B5;_tmp7B5.tag=1;_tmp7B5.f1=(unsigned long)((int)zero);{struct Cyc_Int_pa_struct
_tmp7B4;_tmp7B4.tag=1;_tmp7B4.f1=(unsigned long)((int)zero);{struct Cyc_Int_pa_struct
_tmp7B3;_tmp7B3.tag=1;_tmp7B3.f1=(unsigned long)((int)zero);{void*_tmp7B1[5]={&
_tmp7B3,& _tmp7B4,& _tmp7B5,& _tmp7B6,& _tmp7B7};Cyc_aprintf(({const char*_tmp7B2="%c%c%c%c%s";
_tag_dyneither(_tmp7B2,sizeof(char),11);}),_tag_dyneither(_tmp7B1,sizeof(void*),
5));}}}}}}),0);struct Cyc_Absyn_Vardecl*_tmp7AD=Cyc_Absyn_new_vardecl(f->name,
_tmp78C,(struct Cyc_Absyn_Exp*)_tmp7AC);(void*)(_tmp7AD->sc=(void*)((void*)f->sc));
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp7AE=_cycalloc(sizeof(*_tmp7AE));
_tmp7AE->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_struct*_tmp7AF=
_cycalloc(sizeof(*_tmp7AF));_tmp7AF[0]=({struct Cyc_Absyn_Var_d_struct _tmp7B0;
_tmp7B0.tag=0;_tmp7B0.f1=_tmp7AD;_tmp7B0;});_tmp7AF;}),0);_tmp7AE->tl=Cyc_Toc_result_decls;
_tmp7AE;});*_tmp787=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*
k,int v))Cyc_Dict_insert)(*_tmp787,f->name,1);}goto _LL394;_LL399:;_LL39A: goto
_LL394;_LL394:;}};_pop_dynregion(d);}}}static void Cyc_Toc_enumdecl_to_c(struct Cyc_Toc_Env*
nv,struct Cyc_Absyn_Enumdecl*ed){(void*)(ed->sc=(void*)((void*)2));if(ed->fields
!= 0)Cyc_Toc_enumfields_to_c((struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(ed->fields))->v);}static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*
body_nv,struct Cyc_Toc_Env*init_nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*
s){void*old_typ=(void*)vd->type;(void*)(vd->type=(void*)Cyc_Toc_typ_to_c_array(
old_typ));if((void*)vd->sc == (void*)5  && Cyc_Tcutil_is_tagged_pointer_typ(
old_typ))(void*)(vd->sc=(void*)((void*)2));Cyc_Toc_stmt_to_c(body_nv,s);if(vd->initializer
!= 0){struct Cyc_Absyn_Exp*init=(struct Cyc_Absyn_Exp*)_check_null(vd->initializer);
void*_tmp7B8=(void*)init->r;struct Cyc_Absyn_Vardecl*_tmp7B9;struct Cyc_Absyn_Exp*
_tmp7BA;struct Cyc_Absyn_Exp*_tmp7BB;int _tmp7BC;_LL39C: if(*((int*)_tmp7B8)!= 29)
goto _LL39E;_tmp7B9=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp7B8)->f1;
_tmp7BA=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp7B8)->f2;_tmp7BB=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmp7B8)->f3;_tmp7BC=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp7B8)->f4;_LL39D: vd->initializer=0;(void*)(s->r=(void*)((void*)(Cyc_Toc_init_comprehension(
init_nv,Cyc_Absyn_var_exp(vd->name,0),_tmp7B9,_tmp7BA,_tmp7BB,_tmp7BC,Cyc_Absyn_new_stmt((
void*)s->r,0),0))->r));goto _LL39B;_LL39E:;_LL39F: if((void*)vd->sc == (void*)0){
struct Cyc_Toc_Env _tmp7BE;struct _RegionHandle*_tmp7BF;struct Cyc_Toc_Env*_tmp7BD=
init_nv;_tmp7BE=*_tmp7BD;_tmp7BF=_tmp7BE.rgn;{struct Cyc_Toc_Env*_tmp7C0=Cyc_Toc_set_toplevel(
_tmp7BF,init_nv);Cyc_Toc_exp_to_c(_tmp7C0,init);}}else{Cyc_Toc_exp_to_c(init_nv,
init);}goto _LL39B;_LL39B:;}else{void*_tmp7C1=Cyc_Tcutil_compress(old_typ);struct
Cyc_Absyn_ArrayInfo _tmp7C2;void*_tmp7C3;struct Cyc_Absyn_Exp*_tmp7C4;struct Cyc_Absyn_Conref*
_tmp7C5;_LL3A1: if(_tmp7C1 <= (void*)4)goto _LL3A3;if(*((int*)_tmp7C1)!= 7)goto
_LL3A3;_tmp7C2=((struct Cyc_Absyn_ArrayType_struct*)_tmp7C1)->f1;_tmp7C3=(void*)
_tmp7C2.elt_type;_tmp7C4=_tmp7C2.num_elts;_tmp7C5=_tmp7C2.zero_term;if(!((int(*)(
int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp7C5))goto _LL3A3;_LL3A2:
if(_tmp7C4 == 0)({void*_tmp7C6=0;((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp7C7="can't initialize zero-terminated array -- size unknown";
_tag_dyneither(_tmp7C7,sizeof(char),55);}),_tag_dyneither(_tmp7C6,sizeof(void*),
0));});{struct Cyc_Absyn_Exp*num_elts=(struct Cyc_Absyn_Exp*)_tmp7C4;struct Cyc_Absyn_Exp*
_tmp7C8=Cyc_Absyn_subscript_exp(Cyc_Absyn_var_exp(vd->name,0),Cyc_Absyn_add_exp(
num_elts,Cyc_Absyn_signed_int_exp(- 1,0),0),0);struct Cyc_Absyn_Exp*_tmp7C9=Cyc_Absyn_signed_int_exp(
0,0);(void*)(s->r=(void*)Cyc_Toc_seq_stmt_r(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(
_tmp7C8,_tmp7C9,0),0),Cyc_Absyn_new_stmt((void*)s->r,0)));goto _LL3A0;}_LL3A3:;
_LL3A4: goto _LL3A0;_LL3A0:;}}static struct Cyc_Absyn_Stmt**Cyc_Toc_throw_match_stmt_opt=
0;static struct Cyc_Absyn_Stmt*Cyc_Toc_throw_match_stmt(){if(Cyc_Toc_throw_match_stmt_opt
== 0)Cyc_Toc_throw_match_stmt_opt=({struct Cyc_Absyn_Stmt**_tmp7CA=_cycalloc(
sizeof(*_tmp7CA));_tmp7CA[0]=Cyc_Absyn_exp_stmt(Cyc_Toc_newthrow_exp(Cyc_Absyn_match_exn_exp(
0)),0);_tmp7CA;});return*((struct Cyc_Absyn_Stmt**)_check_null(Cyc_Toc_throw_match_stmt_opt));}
static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*
p,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s){Cyc_Toc_exp_to_c(nv,e);{
struct _tuple1*x=Cyc_Toc_temp_var();struct Cyc_Toc_Env _tmp7CC;struct _RegionHandle*
_tmp7CD;struct Cyc_Toc_Env*_tmp7CB=nv;_tmp7CC=*_tmp7CB;_tmp7CD=_tmp7CC.rgn;{
struct Cyc_Absyn_Stmt*_tmp7CE=Cyc_Toc_throw_match_stmt();struct Cyc_Toc_Env*
_tmp7CF=Cyc_Toc_share_env(_tmp7CD,nv);struct Cyc_Toc_Env*_tmp7D1;struct Cyc_List_List*
_tmp7D2;struct Cyc_Absyn_Stmt*_tmp7D3;struct _tuple13 _tmp7D0=Cyc_Toc_xlate_pat(
_tmp7CF,_tmp7CD,t,Cyc_Absyn_var_exp(x,0),Cyc_Absyn_var_exp(x,0),p,(struct Cyc_Absyn_Stmt**)&
_tmp7CE,Cyc_Toc_throw_match_stmt(),0);_tmp7D1=_tmp7D0.f1;_tmp7D2=_tmp7D0.f2;
_tmp7D3=_tmp7D0.f3;Cyc_Toc_stmt_to_c(_tmp7D1,s);s=Cyc_Absyn_declare_stmt(x,Cyc_Toc_typ_to_c(
t),(struct Cyc_Absyn_Exp*)e,Cyc_Absyn_seq_stmt(_tmp7D3,s,0),0);for(0;_tmp7D2 != 0;
_tmp7D2=_tmp7D2->tl){struct _tuple14 _tmp7D5;struct _tuple1*_tmp7D6;void*_tmp7D7;
struct _tuple14*_tmp7D4=(struct _tuple14*)_tmp7D2->hd;_tmp7D5=*_tmp7D4;_tmp7D6=
_tmp7D5.f1;_tmp7D7=_tmp7D5.f2;s=Cyc_Absyn_declare_stmt(_tmp7D6,_tmp7D7,0,s,0);}}
return s;}}static void Cyc_Toc_exptypes_to_c(struct Cyc_Absyn_Exp*e){void*_tmp7D8=(
void*)e->r;struct Cyc_Absyn_Exp*_tmp7D9;struct Cyc_Absyn_Exp*_tmp7DA;struct Cyc_Absyn_Exp*
_tmp7DB;struct Cyc_Absyn_Exp*_tmp7DC;struct Cyc_Absyn_Exp*_tmp7DD;struct Cyc_Absyn_Exp*
_tmp7DE;struct Cyc_Absyn_Exp*_tmp7DF;struct Cyc_Absyn_Exp*_tmp7E0;struct Cyc_List_List*
_tmp7E1;struct Cyc_Absyn_Exp*_tmp7E2;struct Cyc_Absyn_Exp*_tmp7E3;struct Cyc_Absyn_Exp*
_tmp7E4;struct Cyc_Absyn_Exp*_tmp7E5;struct Cyc_Absyn_Exp*_tmp7E6;struct Cyc_Absyn_Exp*
_tmp7E7;struct Cyc_Absyn_Exp*_tmp7E8;struct Cyc_Absyn_Exp*_tmp7E9;struct Cyc_Absyn_Exp*
_tmp7EA;struct Cyc_Absyn_Exp*_tmp7EB;struct Cyc_Absyn_Exp*_tmp7EC;struct Cyc_Absyn_Exp*
_tmp7ED;struct Cyc_Absyn_Exp*_tmp7EE;struct Cyc_Absyn_Exp*_tmp7EF;struct Cyc_Absyn_Exp*
_tmp7F0;struct Cyc_Absyn_Exp*_tmp7F1;struct Cyc_List_List*_tmp7F2;struct Cyc_Absyn_Exp*
_tmp7F3;struct Cyc_List_List*_tmp7F4;void*_tmp7F5;void**_tmp7F6;struct Cyc_Absyn_Exp*
_tmp7F7;struct _tuple2*_tmp7F8;struct _tuple2 _tmp7F9;void*_tmp7FA;void**_tmp7FB;
struct Cyc_List_List*_tmp7FC;struct Cyc_List_List*_tmp7FD;struct Cyc_List_List*
_tmp7FE;void*_tmp7FF;void**_tmp800;void*_tmp801;void**_tmp802;struct Cyc_Absyn_Stmt*
_tmp803;struct Cyc_Absyn_MallocInfo _tmp804;struct Cyc_Absyn_MallocInfo*_tmp805;
_LL3A6: if(*((int*)_tmp7D8)!= 22)goto _LL3A8;_tmp7D9=((struct Cyc_Absyn_Deref_e_struct*)
_tmp7D8)->f1;_LL3A7: _tmp7DA=_tmp7D9;goto _LL3A9;_LL3A8: if(*((int*)_tmp7D8)!= 23)
goto _LL3AA;_tmp7DA=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp7D8)->f1;_LL3A9:
_tmp7DB=_tmp7DA;goto _LL3AB;_LL3AA: if(*((int*)_tmp7D8)!= 24)goto _LL3AC;_tmp7DB=((
struct Cyc_Absyn_AggrArrow_e_struct*)_tmp7D8)->f1;_LL3AB: _tmp7DC=_tmp7DB;goto
_LL3AD;_LL3AC: if(*((int*)_tmp7D8)!= 16)goto _LL3AE;_tmp7DC=((struct Cyc_Absyn_Address_e_struct*)
_tmp7D8)->f1;_LL3AD: _tmp7DD=_tmp7DC;goto _LL3AF;_LL3AE: if(*((int*)_tmp7D8)!= 12)
goto _LL3B0;_tmp7DD=((struct Cyc_Absyn_Throw_e_struct*)_tmp7D8)->f1;_LL3AF: _tmp7DE=
_tmp7DD;goto _LL3B1;_LL3B0: if(*((int*)_tmp7D8)!= 13)goto _LL3B2;_tmp7DE=((struct
Cyc_Absyn_NoInstantiate_e_struct*)_tmp7D8)->f1;_LL3B1: _tmp7DF=_tmp7DE;goto _LL3B3;
_LL3B2: if(*((int*)_tmp7D8)!= 19)goto _LL3B4;_tmp7DF=((struct Cyc_Absyn_Sizeofexp_e_struct*)
_tmp7D8)->f1;_LL3B3: _tmp7E0=_tmp7DF;goto _LL3B5;_LL3B4: if(*((int*)_tmp7D8)!= 5)
goto _LL3B6;_tmp7E0=((struct Cyc_Absyn_Increment_e_struct*)_tmp7D8)->f1;_LL3B5: Cyc_Toc_exptypes_to_c(
_tmp7E0);goto _LL3A5;_LL3B6: if(*((int*)_tmp7D8)!= 3)goto _LL3B8;_tmp7E1=((struct
Cyc_Absyn_Primop_e_struct*)_tmp7D8)->f2;_LL3B7:((void(*)(void(*f)(struct Cyc_Absyn_Exp*),
struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Toc_exptypes_to_c,_tmp7E1);goto _LL3A5;
_LL3B8: if(*((int*)_tmp7D8)!= 7)goto _LL3BA;_tmp7E2=((struct Cyc_Absyn_And_e_struct*)
_tmp7D8)->f1;_tmp7E3=((struct Cyc_Absyn_And_e_struct*)_tmp7D8)->f2;_LL3B9: _tmp7E4=
_tmp7E2;_tmp7E5=_tmp7E3;goto _LL3BB;_LL3BA: if(*((int*)_tmp7D8)!= 8)goto _LL3BC;
_tmp7E4=((struct Cyc_Absyn_Or_e_struct*)_tmp7D8)->f1;_tmp7E5=((struct Cyc_Absyn_Or_e_struct*)
_tmp7D8)->f2;_LL3BB: _tmp7E6=_tmp7E4;_tmp7E7=_tmp7E5;goto _LL3BD;_LL3BC: if(*((int*)
_tmp7D8)!= 9)goto _LL3BE;_tmp7E6=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp7D8)->f1;
_tmp7E7=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp7D8)->f2;_LL3BD: _tmp7E8=_tmp7E6;
_tmp7E9=_tmp7E7;goto _LL3BF;_LL3BE: if(*((int*)_tmp7D8)!= 25)goto _LL3C0;_tmp7E8=((
struct Cyc_Absyn_Subscript_e_struct*)_tmp7D8)->f1;_tmp7E9=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp7D8)->f2;_LL3BF: _tmp7EA=_tmp7E8;_tmp7EB=_tmp7E9;goto _LL3C1;_LL3C0: if(*((int*)
_tmp7D8)!= 36)goto _LL3C2;_tmp7EA=((struct Cyc_Absyn_Swap_e_struct*)_tmp7D8)->f1;
_tmp7EB=((struct Cyc_Absyn_Swap_e_struct*)_tmp7D8)->f2;_LL3C1: _tmp7EC=_tmp7EA;
_tmp7ED=_tmp7EB;goto _LL3C3;_LL3C2: if(*((int*)_tmp7D8)!= 4)goto _LL3C4;_tmp7EC=((
struct Cyc_Absyn_AssignOp_e_struct*)_tmp7D8)->f1;_tmp7ED=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmp7D8)->f3;_LL3C3: Cyc_Toc_exptypes_to_c(_tmp7EC);Cyc_Toc_exptypes_to_c(_tmp7ED);
goto _LL3A5;_LL3C4: if(*((int*)_tmp7D8)!= 6)goto _LL3C6;_tmp7EE=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp7D8)->f1;_tmp7EF=((struct Cyc_Absyn_Conditional_e_struct*)_tmp7D8)->f2;
_tmp7F0=((struct Cyc_Absyn_Conditional_e_struct*)_tmp7D8)->f3;_LL3C5: Cyc_Toc_exptypes_to_c(
_tmp7EE);Cyc_Toc_exptypes_to_c(_tmp7EF);Cyc_Toc_exptypes_to_c(_tmp7F0);goto
_LL3A5;_LL3C6: if(*((int*)_tmp7D8)!= 11)goto _LL3C8;_tmp7F1=((struct Cyc_Absyn_FnCall_e_struct*)
_tmp7D8)->f1;_tmp7F2=((struct Cyc_Absyn_FnCall_e_struct*)_tmp7D8)->f2;_LL3C7:
_tmp7F3=_tmp7F1;_tmp7F4=_tmp7F2;goto _LL3C9;_LL3C8: if(*((int*)_tmp7D8)!= 10)goto
_LL3CA;_tmp7F3=((struct Cyc_Absyn_UnknownCall_e_struct*)_tmp7D8)->f1;_tmp7F4=((
struct Cyc_Absyn_UnknownCall_e_struct*)_tmp7D8)->f2;_LL3C9: Cyc_Toc_exptypes_to_c(
_tmp7F3);((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(
Cyc_Toc_exptypes_to_c,_tmp7F4);goto _LL3A5;_LL3CA: if(*((int*)_tmp7D8)!= 15)goto
_LL3CC;_tmp7F5=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp7D8)->f1;_tmp7F6=(
void**)&((void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp7D8)->f1);_tmp7F7=((struct
Cyc_Absyn_Cast_e_struct*)_tmp7D8)->f2;_LL3CB:*_tmp7F6=Cyc_Toc_typ_to_c(*_tmp7F6);
Cyc_Toc_exptypes_to_c(_tmp7F7);goto _LL3A5;_LL3CC: if(*((int*)_tmp7D8)!= 27)goto
_LL3CE;_tmp7F8=((struct Cyc_Absyn_CompoundLit_e_struct*)_tmp7D8)->f1;_tmp7F9=*
_tmp7F8;_tmp7FA=_tmp7F9.f3;_tmp7FB=(void**)&(*((struct Cyc_Absyn_CompoundLit_e_struct*)
_tmp7D8)->f1).f3;_tmp7FC=((struct Cyc_Absyn_CompoundLit_e_struct*)_tmp7D8)->f2;
_LL3CD:*_tmp7FB=Cyc_Toc_typ_to_c(*_tmp7FB);_tmp7FD=_tmp7FC;goto _LL3CF;_LL3CE: if(*((
int*)_tmp7D8)!= 37)goto _LL3D0;_tmp7FD=((struct Cyc_Absyn_UnresolvedMem_e_struct*)
_tmp7D8)->f2;_LL3CF: _tmp7FE=_tmp7FD;goto _LL3D1;_LL3D0: if(*((int*)_tmp7D8)!= 28)
goto _LL3D2;_tmp7FE=((struct Cyc_Absyn_Array_e_struct*)_tmp7D8)->f1;_LL3D1: for(0;
_tmp7FE != 0;_tmp7FE=_tmp7FE->tl){struct Cyc_Absyn_Exp*_tmp807;struct _tuple8
_tmp806=*((struct _tuple8*)_tmp7FE->hd);_tmp807=_tmp806.f2;Cyc_Toc_exptypes_to_c(
_tmp807);}goto _LL3A5;_LL3D2: if(*((int*)_tmp7D8)!= 20)goto _LL3D4;_tmp7FF=(void*)((
struct Cyc_Absyn_Offsetof_e_struct*)_tmp7D8)->f1;_tmp800=(void**)&((void*)((
struct Cyc_Absyn_Offsetof_e_struct*)_tmp7D8)->f1);_LL3D3: _tmp802=_tmp800;goto
_LL3D5;_LL3D4: if(*((int*)_tmp7D8)!= 18)goto _LL3D6;_tmp801=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp7D8)->f1;_tmp802=(void**)&((void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp7D8)->f1);_LL3D5:*_tmp802=Cyc_Toc_typ_to_c(*_tmp802);goto _LL3A5;_LL3D6: if(*((
int*)_tmp7D8)!= 38)goto _LL3D8;_tmp803=((struct Cyc_Absyn_StmtExp_e_struct*)
_tmp7D8)->f1;_LL3D7: Cyc_Toc_stmttypes_to_c(_tmp803);goto _LL3A5;_LL3D8: if(*((int*)
_tmp7D8)!= 35)goto _LL3DA;_tmp804=((struct Cyc_Absyn_Malloc_e_struct*)_tmp7D8)->f1;
_tmp805=(struct Cyc_Absyn_MallocInfo*)&((struct Cyc_Absyn_Malloc_e_struct*)_tmp7D8)->f1;
_LL3D9: if(_tmp805->elt_type != 0)_tmp805->elt_type=({void**_tmp808=_cycalloc(
sizeof(*_tmp808));_tmp808[0]=Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp805->elt_type)));
_tmp808;});Cyc_Toc_exptypes_to_c(_tmp805->num_elts);goto _LL3A5;_LL3DA: if(*((int*)
_tmp7D8)!= 0)goto _LL3DC;_LL3DB: goto _LL3DD;_LL3DC: if(*((int*)_tmp7D8)!= 1)goto
_LL3DE;_LL3DD: goto _LL3DF;_LL3DE: if(*((int*)_tmp7D8)!= 2)goto _LL3E0;_LL3DF: goto
_LL3E1;_LL3E0: if(*((int*)_tmp7D8)!= 33)goto _LL3E2;_LL3E1: goto _LL3E3;_LL3E2: if(*((
int*)_tmp7D8)!= 34)goto _LL3E4;_LL3E3: goto _LL3A5;_LL3E4: if(*((int*)_tmp7D8)!= 31)
goto _LL3E6;_LL3E5: goto _LL3E7;_LL3E6: if(*((int*)_tmp7D8)!= 32)goto _LL3E8;_LL3E7:
goto _LL3E9;_LL3E8: if(*((int*)_tmp7D8)!= 30)goto _LL3EA;_LL3E9: goto _LL3EB;_LL3EA:
if(*((int*)_tmp7D8)!= 29)goto _LL3EC;_LL3EB: goto _LL3ED;_LL3EC: if(*((int*)_tmp7D8)
!= 26)goto _LL3EE;_LL3ED: goto _LL3EF;_LL3EE: if(*((int*)_tmp7D8)!= 14)goto _LL3F0;
_LL3EF: goto _LL3F1;_LL3F0: if(*((int*)_tmp7D8)!= 17)goto _LL3F2;_LL3F1: goto _LL3F3;
_LL3F2: if(*((int*)_tmp7D8)!= 39)goto _LL3F4;_LL3F3: goto _LL3F5;_LL3F4: if(*((int*)
_tmp7D8)!= 21)goto _LL3A5;_LL3F5:({void*_tmp809=0;Cyc_Tcutil_terr(e->loc,({const
char*_tmp80A="Cyclone expression within C code";_tag_dyneither(_tmp80A,sizeof(
char),33);}),_tag_dyneither(_tmp809,sizeof(void*),0));});goto _LL3A5;_LL3A5:;}
static void Cyc_Toc_decltypes_to_c(struct Cyc_Absyn_Decl*d){void*_tmp80B=(void*)d->r;
struct Cyc_Absyn_Vardecl*_tmp80C;struct Cyc_Absyn_Fndecl*_tmp80D;struct Cyc_Absyn_Aggrdecl*
_tmp80E;struct Cyc_Absyn_Enumdecl*_tmp80F;struct Cyc_Absyn_Typedefdecl*_tmp810;
_LL3F7: if(_tmp80B <= (void*)2)goto _LL413;if(*((int*)_tmp80B)!= 0)goto _LL3F9;
_tmp80C=((struct Cyc_Absyn_Var_d_struct*)_tmp80B)->f1;_LL3F8:(void*)(_tmp80C->type=(
void*)Cyc_Toc_typ_to_c((void*)_tmp80C->type));if(_tmp80C->initializer != 0)Cyc_Toc_exptypes_to_c((
struct Cyc_Absyn_Exp*)_check_null(_tmp80C->initializer));goto _LL3F6;_LL3F9: if(*((
int*)_tmp80B)!= 1)goto _LL3FB;_tmp80D=((struct Cyc_Absyn_Fn_d_struct*)_tmp80B)->f1;
_LL3FA:(void*)(_tmp80D->ret_type=(void*)Cyc_Toc_typ_to_c((void*)_tmp80D->ret_type));{
struct Cyc_List_List*_tmp811=_tmp80D->args;for(0;_tmp811 != 0;_tmp811=_tmp811->tl){(*((
struct _tuple17*)_tmp811->hd)).f3=Cyc_Toc_typ_to_c((*((struct _tuple17*)_tmp811->hd)).f3);}}
goto _LL3F6;_LL3FB: if(*((int*)_tmp80B)!= 6)goto _LL3FD;_tmp80E=((struct Cyc_Absyn_Aggr_d_struct*)
_tmp80B)->f1;_LL3FC: Cyc_Toc_aggrdecl_to_c(_tmp80E);goto _LL3F6;_LL3FD: if(*((int*)
_tmp80B)!= 8)goto _LL3FF;_tmp80F=((struct Cyc_Absyn_Enum_d_struct*)_tmp80B)->f1;
_LL3FE: if(_tmp80F->fields != 0){struct Cyc_List_List*_tmp812=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp80F->fields))->v;for(0;_tmp812 != 0;_tmp812=
_tmp812->tl){struct Cyc_Absyn_Enumfield*_tmp813=(struct Cyc_Absyn_Enumfield*)
_tmp812->hd;if(_tmp813->tag != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)
_check_null(_tmp813->tag));}}goto _LL3F6;_LL3FF: if(*((int*)_tmp80B)!= 9)goto
_LL401;_tmp810=((struct Cyc_Absyn_Typedef_d_struct*)_tmp80B)->f1;_LL400: _tmp810->defn=({
struct Cyc_Core_Opt*_tmp814=_cycalloc(sizeof(*_tmp814));_tmp814->v=(void*)Cyc_Toc_typ_to_c_array((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp810->defn))->v);_tmp814;});goto
_LL3F6;_LL401: if(*((int*)_tmp80B)!= 2)goto _LL403;_LL402: goto _LL404;_LL403: if(*((
int*)_tmp80B)!= 3)goto _LL405;_LL404: goto _LL406;_LL405: if(*((int*)_tmp80B)!= 7)
goto _LL407;_LL406: goto _LL408;_LL407: if(*((int*)_tmp80B)!= 10)goto _LL409;_LL408:
goto _LL40A;_LL409: if(*((int*)_tmp80B)!= 11)goto _LL40B;_LL40A: goto _LL40C;_LL40B:
if(*((int*)_tmp80B)!= 12)goto _LL40D;_LL40C: goto _LL40E;_LL40D: if(*((int*)_tmp80B)
!= 13)goto _LL40F;_LL40E: goto _LL410;_LL40F: if(*((int*)_tmp80B)!= 4)goto _LL411;
_LL410: goto _LL412;_LL411: if(*((int*)_tmp80B)!= 5)goto _LL413;_LL412:({void*
_tmp815=0;Cyc_Tcutil_terr(d->loc,({const char*_tmp816="Cyclone declaration within C code";
_tag_dyneither(_tmp816,sizeof(char),34);}),_tag_dyneither(_tmp815,sizeof(void*),
0));});goto _LL3F6;_LL413: if((int)_tmp80B != 0)goto _LL415;_LL414: goto _LL416;_LL415:
if((int)_tmp80B != 1)goto _LL3F6;_LL416: goto _LL3F6;_LL3F6:;}static void Cyc_Toc_stmttypes_to_c(
struct Cyc_Absyn_Stmt*s){void*_tmp817=(void*)s->r;struct Cyc_Absyn_Exp*_tmp818;
struct Cyc_Absyn_Stmt*_tmp819;struct Cyc_Absyn_Stmt*_tmp81A;struct Cyc_Absyn_Exp*
_tmp81B;struct Cyc_Absyn_Exp*_tmp81C;struct Cyc_Absyn_Stmt*_tmp81D;struct Cyc_Absyn_Stmt*
_tmp81E;struct _tuple3 _tmp81F;struct Cyc_Absyn_Exp*_tmp820;struct Cyc_Absyn_Stmt*
_tmp821;struct Cyc_Absyn_Exp*_tmp822;struct _tuple3 _tmp823;struct Cyc_Absyn_Exp*
_tmp824;struct _tuple3 _tmp825;struct Cyc_Absyn_Exp*_tmp826;struct Cyc_Absyn_Stmt*
_tmp827;struct Cyc_Absyn_Exp*_tmp828;struct Cyc_List_List*_tmp829;struct Cyc_Absyn_Decl*
_tmp82A;struct Cyc_Absyn_Stmt*_tmp82B;struct Cyc_Absyn_Stmt*_tmp82C;struct _tuple3
_tmp82D;struct Cyc_Absyn_Exp*_tmp82E;_LL418: if(_tmp817 <= (void*)1)goto _LL42A;if(*((
int*)_tmp817)!= 0)goto _LL41A;_tmp818=((struct Cyc_Absyn_Exp_s_struct*)_tmp817)->f1;
_LL419: Cyc_Toc_exptypes_to_c(_tmp818);goto _LL417;_LL41A: if(*((int*)_tmp817)!= 1)
goto _LL41C;_tmp819=((struct Cyc_Absyn_Seq_s_struct*)_tmp817)->f1;_tmp81A=((struct
Cyc_Absyn_Seq_s_struct*)_tmp817)->f2;_LL41B: Cyc_Toc_stmttypes_to_c(_tmp819);Cyc_Toc_stmttypes_to_c(
_tmp81A);goto _LL417;_LL41C: if(*((int*)_tmp817)!= 2)goto _LL41E;_tmp81B=((struct
Cyc_Absyn_Return_s_struct*)_tmp817)->f1;_LL41D: if(_tmp81B != 0)Cyc_Toc_exptypes_to_c((
struct Cyc_Absyn_Exp*)_tmp81B);goto _LL417;_LL41E: if(*((int*)_tmp817)!= 3)goto
_LL420;_tmp81C=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmp817)->f1;_tmp81D=((
struct Cyc_Absyn_IfThenElse_s_struct*)_tmp817)->f2;_tmp81E=((struct Cyc_Absyn_IfThenElse_s_struct*)
_tmp817)->f3;_LL41F: Cyc_Toc_exptypes_to_c(_tmp81C);Cyc_Toc_stmttypes_to_c(
_tmp81D);Cyc_Toc_stmttypes_to_c(_tmp81E);goto _LL417;_LL420: if(*((int*)_tmp817)!= 
4)goto _LL422;_tmp81F=((struct Cyc_Absyn_While_s_struct*)_tmp817)->f1;_tmp820=
_tmp81F.f1;_tmp821=((struct Cyc_Absyn_While_s_struct*)_tmp817)->f2;_LL421: Cyc_Toc_exptypes_to_c(
_tmp820);Cyc_Toc_stmttypes_to_c(_tmp821);goto _LL417;_LL422: if(*((int*)_tmp817)!= 
8)goto _LL424;_tmp822=((struct Cyc_Absyn_For_s_struct*)_tmp817)->f1;_tmp823=((
struct Cyc_Absyn_For_s_struct*)_tmp817)->f2;_tmp824=_tmp823.f1;_tmp825=((struct
Cyc_Absyn_For_s_struct*)_tmp817)->f3;_tmp826=_tmp825.f1;_tmp827=((struct Cyc_Absyn_For_s_struct*)
_tmp817)->f4;_LL423: Cyc_Toc_exptypes_to_c(_tmp822);Cyc_Toc_exptypes_to_c(_tmp824);
Cyc_Toc_exptypes_to_c(_tmp826);Cyc_Toc_stmttypes_to_c(_tmp827);goto _LL417;_LL424:
if(*((int*)_tmp817)!= 9)goto _LL426;_tmp828=((struct Cyc_Absyn_Switch_s_struct*)
_tmp817)->f1;_tmp829=((struct Cyc_Absyn_Switch_s_struct*)_tmp817)->f2;_LL425: Cyc_Toc_exptypes_to_c(
_tmp828);for(0;_tmp829 != 0;_tmp829=_tmp829->tl){Cyc_Toc_stmttypes_to_c(((struct
Cyc_Absyn_Switch_clause*)_tmp829->hd)->body);}goto _LL417;_LL426: if(*((int*)
_tmp817)!= 11)goto _LL428;_tmp82A=((struct Cyc_Absyn_Decl_s_struct*)_tmp817)->f1;
_tmp82B=((struct Cyc_Absyn_Decl_s_struct*)_tmp817)->f2;_LL427: Cyc_Toc_decltypes_to_c(
_tmp82A);Cyc_Toc_stmttypes_to_c(_tmp82B);goto _LL417;_LL428: if(*((int*)_tmp817)!= 
13)goto _LL42A;_tmp82C=((struct Cyc_Absyn_Do_s_struct*)_tmp817)->f1;_tmp82D=((
struct Cyc_Absyn_Do_s_struct*)_tmp817)->f2;_tmp82E=_tmp82D.f1;_LL429: Cyc_Toc_stmttypes_to_c(
_tmp82C);Cyc_Toc_exptypes_to_c(_tmp82E);goto _LL417;_LL42A: if((int)_tmp817 != 0)
goto _LL42C;_LL42B: goto _LL42D;_LL42C: if(_tmp817 <= (void*)1)goto _LL42E;if(*((int*)
_tmp817)!= 5)goto _LL42E;_LL42D: goto _LL42F;_LL42E: if(_tmp817 <= (void*)1)goto
_LL430;if(*((int*)_tmp817)!= 6)goto _LL430;_LL42F: goto _LL431;_LL430: if(_tmp817 <= (
void*)1)goto _LL432;if(*((int*)_tmp817)!= 7)goto _LL432;_LL431: goto _LL417;_LL432:
if(_tmp817 <= (void*)1)goto _LL434;if(*((int*)_tmp817)!= 10)goto _LL434;_LL433: goto
_LL435;_LL434: if(_tmp817 <= (void*)1)goto _LL436;if(*((int*)_tmp817)!= 12)goto
_LL436;_LL435: goto _LL437;_LL436: if(_tmp817 <= (void*)1)goto _LL438;if(*((int*)
_tmp817)!= 14)goto _LL438;_LL437: goto _LL439;_LL438: if(_tmp817 <= (void*)1)goto
_LL417;if(*((int*)_tmp817)!= 15)goto _LL417;_LL439:({void*_tmp82F=0;Cyc_Tcutil_terr(
s->loc,({const char*_tmp830="Cyclone statement in C code";_tag_dyneither(_tmp830,
sizeof(char),28);}),_tag_dyneither(_tmp82F,sizeof(void*),0));});goto _LL417;
_LL417:;}static struct Cyc_Toc_Env*Cyc_Toc_decls_to_c(struct _RegionHandle*r,struct
Cyc_Toc_Env*nv,struct Cyc_List_List*ds,int top,int cinclude){for(0;ds != 0;ds=ds->tl){
if(!Cyc_Toc_is_toplevel(nv))({void*_tmp831=0;((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp832="decls_to_c: not at toplevel!";
_tag_dyneither(_tmp832,sizeof(char),29);}),_tag_dyneither(_tmp831,sizeof(void*),
0));});{struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)ds->hd;void*_tmp833=(void*)
d->r;struct Cyc_Absyn_Vardecl*_tmp834;struct Cyc_Absyn_Fndecl*_tmp835;struct Cyc_Absyn_Aggrdecl*
_tmp836;struct Cyc_Absyn_Tuniondecl*_tmp837;struct Cyc_Absyn_Enumdecl*_tmp838;
struct Cyc_Absyn_Typedefdecl*_tmp839;struct Cyc_List_List*_tmp83A;struct Cyc_List_List*
_tmp83B;struct Cyc_List_List*_tmp83C;struct Cyc_List_List*_tmp83D;_LL43B: if(
_tmp833 <= (void*)2)goto _LL44F;if(*((int*)_tmp833)!= 0)goto _LL43D;_tmp834=((
struct Cyc_Absyn_Var_d_struct*)_tmp833)->f1;_LL43C: {struct _tuple1*_tmp83E=
_tmp834->name;if((void*)_tmp834->sc == (void*)4)_tmp83E=({struct _tuple1*_tmp83F=
_cycalloc(sizeof(*_tmp83F));_tmp83F->f1=(union Cyc_Absyn_Nmspace_union)({union Cyc_Absyn_Nmspace_union
_tmp840;(_tmp840.Rel_n).tag=1;(_tmp840.Rel_n).f1=0;_tmp840;});_tmp83F->f2=(*
_tmp83E).f2;_tmp83F;});if(_tmp834->initializer != 0){if(cinclude)Cyc_Toc_exptypes_to_c((
struct Cyc_Absyn_Exp*)_check_null(_tmp834->initializer));else{Cyc_Toc_exp_to_c(nv,(
struct Cyc_Absyn_Exp*)_check_null(_tmp834->initializer));}}nv=Cyc_Toc_add_varmap(
r,nv,_tmp834->name,Cyc_Absyn_varb_exp(_tmp83E,(void*)({struct Cyc_Absyn_Global_b_struct*
_tmp841=_cycalloc(sizeof(*_tmp841));_tmp841[0]=({struct Cyc_Absyn_Global_b_struct
_tmp842;_tmp842.tag=0;_tmp842.f1=_tmp834;_tmp842;});_tmp841;}),0));_tmp834->name=
_tmp83E;(void*)(_tmp834->sc=(void*)Cyc_Toc_scope_to_c((void*)_tmp834->sc));(void*)(
_tmp834->type=(void*)Cyc_Toc_typ_to_c_array((void*)_tmp834->type));Cyc_Toc_result_decls=({
struct Cyc_List_List*_tmp843=_cycalloc(sizeof(*_tmp843));_tmp843->hd=d;_tmp843->tl=
Cyc_Toc_result_decls;_tmp843;});goto _LL43A;}_LL43D: if(*((int*)_tmp833)!= 1)goto
_LL43F;_tmp835=((struct Cyc_Absyn_Fn_d_struct*)_tmp833)->f1;_LL43E: {struct
_tuple1*_tmp844=_tmp835->name;if((void*)_tmp835->sc == (void*)4){_tmp844=({struct
_tuple1*_tmp845=_cycalloc(sizeof(*_tmp845));_tmp845->f1=(union Cyc_Absyn_Nmspace_union)({
union Cyc_Absyn_Nmspace_union _tmp846;(_tmp846.Rel_n).tag=1;(_tmp846.Rel_n).f1=0;
_tmp846;});_tmp845->f2=(*_tmp844).f2;_tmp845;});(void*)(_tmp835->sc=(void*)((
void*)2));}nv=Cyc_Toc_add_varmap(r,nv,_tmp835->name,Cyc_Absyn_var_exp(_tmp844,0));
_tmp835->name=_tmp844;Cyc_Toc_fndecl_to_c(nv,_tmp835,cinclude);Cyc_Toc_result_decls=({
struct Cyc_List_List*_tmp847=_cycalloc(sizeof(*_tmp847));_tmp847->hd=d;_tmp847->tl=
Cyc_Toc_result_decls;_tmp847;});goto _LL43A;}_LL43F: if(*((int*)_tmp833)!= 2)goto
_LL441;_LL440: goto _LL442;_LL441: if(*((int*)_tmp833)!= 3)goto _LL443;_LL442:({void*
_tmp848=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp849="letdecl at toplevel";_tag_dyneither(_tmp849,sizeof(char),20);}),
_tag_dyneither(_tmp848,sizeof(void*),0));});_LL443: if(*((int*)_tmp833)!= 4)goto
_LL445;_LL444:({void*_tmp84A=0;((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp84B="region decl at toplevel";
_tag_dyneither(_tmp84B,sizeof(char),24);}),_tag_dyneither(_tmp84A,sizeof(void*),
0));});_LL445: if(*((int*)_tmp833)!= 5)goto _LL447;_LL446:({void*_tmp84C=0;((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp84D="alias decl at toplevel";_tag_dyneither(_tmp84D,sizeof(char),23);}),
_tag_dyneither(_tmp84C,sizeof(void*),0));});_LL447: if(*((int*)_tmp833)!= 6)goto
_LL449;_tmp836=((struct Cyc_Absyn_Aggr_d_struct*)_tmp833)->f1;_LL448: Cyc_Toc_aggrdecl_to_c(
_tmp836);Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp84E=_cycalloc(sizeof(*
_tmp84E));_tmp84E->hd=d;_tmp84E->tl=Cyc_Toc_result_decls;_tmp84E;});goto _LL43A;
_LL449: if(*((int*)_tmp833)!= 7)goto _LL44B;_tmp837=((struct Cyc_Absyn_Tunion_d_struct*)
_tmp833)->f1;_LL44A: if(_tmp837->is_xtunion)Cyc_Toc_xtuniondecl_to_c(_tmp837);
else{Cyc_Toc_tuniondecl_to_c(_tmp837);}goto _LL43A;_LL44B: if(*((int*)_tmp833)!= 8)
goto _LL44D;_tmp838=((struct Cyc_Absyn_Enum_d_struct*)_tmp833)->f1;_LL44C: Cyc_Toc_enumdecl_to_c(
nv,_tmp838);Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp84F=_cycalloc(sizeof(*
_tmp84F));_tmp84F->hd=d;_tmp84F->tl=Cyc_Toc_result_decls;_tmp84F;});goto _LL43A;
_LL44D: if(*((int*)_tmp833)!= 9)goto _LL44F;_tmp839=((struct Cyc_Absyn_Typedef_d_struct*)
_tmp833)->f1;_LL44E: _tmp839->name=_tmp839->name;_tmp839->tvs=0;if(_tmp839->defn
!= 0)_tmp839->defn=({struct Cyc_Core_Opt*_tmp850=_cycalloc(sizeof(*_tmp850));
_tmp850->v=(void*)Cyc_Toc_typ_to_c_array((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp839->defn))->v);_tmp850;});else{void*_tmp851=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp839->kind))->v;_LL45C: if((int)_tmp851 != 2)goto _LL45E;_LL45D:
_tmp839->defn=({struct Cyc_Core_Opt*_tmp852=_cycalloc(sizeof(*_tmp852));_tmp852->v=(
void*)Cyc_Absyn_void_star_typ();_tmp852;});goto _LL45B;_LL45E:;_LL45F: _tmp839->defn=({
struct Cyc_Core_Opt*_tmp853=_cycalloc(sizeof(*_tmp853));_tmp853->v=(void*)((void*)
0);_tmp853;});goto _LL45B;_LL45B:;}Cyc_Toc_result_decls=({struct Cyc_List_List*
_tmp854=_cycalloc(sizeof(*_tmp854));_tmp854->hd=d;_tmp854->tl=Cyc_Toc_result_decls;
_tmp854;});goto _LL43A;_LL44F: if((int)_tmp833 != 0)goto _LL451;_LL450: goto _LL452;
_LL451: if((int)_tmp833 != 1)goto _LL453;_LL452: goto _LL43A;_LL453: if(_tmp833 <= (
void*)2)goto _LL455;if(*((int*)_tmp833)!= 10)goto _LL455;_tmp83A=((struct Cyc_Absyn_Namespace_d_struct*)
_tmp833)->f2;_LL454: _tmp83B=_tmp83A;goto _LL456;_LL455: if(_tmp833 <= (void*)2)goto
_LL457;if(*((int*)_tmp833)!= 11)goto _LL457;_tmp83B=((struct Cyc_Absyn_Using_d_struct*)
_tmp833)->f2;_LL456: _tmp83C=_tmp83B;goto _LL458;_LL457: if(_tmp833 <= (void*)2)goto
_LL459;if(*((int*)_tmp833)!= 12)goto _LL459;_tmp83C=((struct Cyc_Absyn_ExternC_d_struct*)
_tmp833)->f1;_LL458: nv=Cyc_Toc_decls_to_c(r,nv,_tmp83C,top,cinclude);goto _LL43A;
_LL459: if(_tmp833 <= (void*)2)goto _LL43A;if(*((int*)_tmp833)!= 13)goto _LL43A;
_tmp83D=((struct Cyc_Absyn_ExternCinclude_d_struct*)_tmp833)->f1;_LL45A: nv=Cyc_Toc_decls_to_c(
r,nv,_tmp83D,top,1);goto _LL43A;_LL43A:;}}return nv;}static void Cyc_Toc_init(){
struct _DynRegionHandle*_tmp856;struct Cyc_Core_NewRegion _tmp855=Cyc_Core_new_dynregion();
_tmp856=_tmp855.dynregion;{struct _DynRegionFrame _tmp857;struct _RegionHandle*d=
_open_dynregion(& _tmp857,_tmp856);Cyc_Toc_toc_state=({struct Cyc_Toc_TocState*
_tmp858=_cycalloc(sizeof(*_tmp858));_tmp858->dyn=(struct _DynRegionHandle*)
_tmp856;_tmp858->tuple_types=(struct Cyc_List_List**)({struct Cyc_List_List**
_tmp85D=_region_malloc(d,sizeof(*_tmp85D));_tmp85D[0]=0;_tmp85D;});_tmp858->aggrs_so_far=(
struct Cyc_Dict_Dict*)({struct Cyc_Dict_Dict*_tmp85C=_region_malloc(d,sizeof(*
_tmp85C));_tmp85C[0]=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(
struct _tuple1*,struct _tuple1*)))Cyc_Dict_rempty)(d,Cyc_Absyn_qvar_cmp);_tmp85C;});
_tmp858->tunions_so_far=(struct Cyc_Set_Set**)({struct Cyc_Set_Set**_tmp85B=
_region_malloc(d,sizeof(*_tmp85B));_tmp85B[0]=((struct Cyc_Set_Set*(*)(struct
_RegionHandle*r,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Set_rempty)(d,Cyc_Absyn_qvar_cmp);
_tmp85B;});_tmp858->xtunions_so_far=(struct Cyc_Dict_Dict*)({struct Cyc_Dict_Dict*
_tmp85A=_region_malloc(d,sizeof(*_tmp85A));_tmp85A[0]=((struct Cyc_Dict_Dict(*)(
struct _RegionHandle*,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_rempty)(
d,Cyc_Absyn_qvar_cmp);_tmp85A;});_tmp858->qvar_tags=(struct Cyc_Dict_Dict*)({
struct Cyc_Dict_Dict*_tmp859=_region_malloc(d,sizeof(*_tmp859));_tmp859[0]=((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple6*,struct
_tuple6*)))Cyc_Dict_rempty)(d,Cyc_Toc_qvar_tag_cmp);_tmp859;});_tmp858->temp_labels=(
struct Cyc_Xarray_Xarray*)((struct Cyc_Xarray_Xarray*(*)(struct _RegionHandle*))Cyc_Xarray_rcreate_empty)(
d);_tmp858;});Cyc_Toc_result_decls=0;Cyc_Toc_tuple_type_counter=0;Cyc_Toc_temp_var_counter=
0;Cyc_Toc_fresh_label_counter=0;Cyc_Toc_total_bounds_checks=0;Cyc_Toc_bounds_checks_eliminated=
0;Cyc_Toc_globals=_tag_dyneither(({struct _dyneither_ptr**_tmp85E=_cycalloc(
sizeof(struct _dyneither_ptr*)* 38);_tmp85E[0]=& Cyc_Toc__throw_str;_tmp85E[1]=&
Cyc_Toc_setjmp_str;_tmp85E[2]=& Cyc_Toc__push_handler_str;_tmp85E[3]=& Cyc_Toc__pop_handler_str;
_tmp85E[4]=& Cyc_Toc__exn_thrown_str;_tmp85E[5]=& Cyc_Toc__npop_handler_str;
_tmp85E[6]=& Cyc_Toc__check_null_str;_tmp85E[7]=& Cyc_Toc__check_known_subscript_null_str;
_tmp85E[8]=& Cyc_Toc__check_known_subscript_notnull_str;_tmp85E[9]=& Cyc_Toc__check_dyneither_subscript_str;
_tmp85E[10]=& Cyc_Toc__dyneither_ptr_str;_tmp85E[11]=& Cyc_Toc__tag_dyneither_str;
_tmp85E[12]=& Cyc_Toc__init_dyneither_ptr_str;_tmp85E[13]=& Cyc_Toc__untag_dyneither_ptr_str;
_tmp85E[14]=& Cyc_Toc__get_dyneither_size_str;_tmp85E[15]=& Cyc_Toc__get_zero_arr_size_str;
_tmp85E[16]=& Cyc_Toc__dyneither_ptr_plus_str;_tmp85E[17]=& Cyc_Toc__zero_arr_plus_str;
_tmp85E[18]=& Cyc_Toc__dyneither_ptr_inplace_plus_str;_tmp85E[19]=& Cyc_Toc__zero_arr_inplace_plus_str;
_tmp85E[20]=& Cyc_Toc__dyneither_ptr_inplace_plus_post_str;_tmp85E[21]=& Cyc_Toc__zero_arr_inplace_plus_post_str;
_tmp85E[22]=& Cyc_Toc__cycalloc_str;_tmp85E[23]=& Cyc_Toc__cyccalloc_str;_tmp85E[
24]=& Cyc_Toc__cycalloc_atomic_str;_tmp85E[25]=& Cyc_Toc__cyccalloc_atomic_str;
_tmp85E[26]=& Cyc_Toc__region_malloc_str;_tmp85E[27]=& Cyc_Toc__region_calloc_str;
_tmp85E[28]=& Cyc_Toc__check_times_str;_tmp85E[29]=& Cyc_Toc__new_region_str;
_tmp85E[30]=& Cyc_Toc__push_region_str;_tmp85E[31]=& Cyc_Toc__pop_region_str;
_tmp85E[32]=& Cyc_Toc__open_dynregion_str;_tmp85E[33]=& Cyc_Toc__push_dynregion_str;
_tmp85E[34]=& Cyc_Toc__pop_dynregion_str;_tmp85E[35]=& Cyc_Toc__reset_region_str;
_tmp85E[36]=& Cyc_Toc__throw_arraybounds_str;_tmp85E[37]=& Cyc_Toc__dyneither_ptr_decrease_size_str;
_tmp85E;}),sizeof(struct _dyneither_ptr*),38);;_pop_dynregion(d);}}struct Cyc_List_List*
Cyc_Toc_toc(struct Cyc_List_List*ds){Cyc_Toc_init();{struct _RegionHandle _tmp85F=
_new_region("start");struct _RegionHandle*start=& _tmp85F;_push_region(start);Cyc_Toc_decls_to_c(
start,Cyc_Toc_empty_env(start),ds,1,0);{struct _DynRegionHandle*_tmp861;struct Cyc_Toc_TocState
_tmp860=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp861=
_tmp860.dyn;Cyc_Core_free_dynregion(_tmp861);}{struct Cyc_List_List*_tmp862=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_Toc_result_decls);
_npop_handler(0);return _tmp862;};_pop_region(start);}}
