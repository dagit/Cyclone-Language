#ifndef _SETJMP_H_
#define _SETJMP_H_
#ifndef _jmp_buf_def_
#define _jmp_buf_def_
typedef int jmp_buf[192];
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
struct Cyc_Null_Exception_struct { char *tag; };
struct Cyc_Array_bounds_struct { char *tag; };
struct Cyc_Match_Exception_struct { char *tag; };
struct Cyc_Bad_alloc_struct { char *tag; };
extern char Cyc_Null_Exception[];
extern char Cyc_Array_bounds[];
extern char Cyc_Match_Exception[];
extern char Cyc_Bad_alloc[];

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
 void exit(int);void*abort();struct Cyc_Core_NewRegion{struct _DynRegionHandle*
dynregion;};struct Cyc_Core_Opt{void*v;};extern char Cyc_Core_Invalid_argument[17];
struct Cyc_Core_Invalid_argument_struct{char*tag;struct _dyneither_ptr f1;};extern
char Cyc_Core_Failure[8];struct Cyc_Core_Failure_struct{char*tag;struct
_dyneither_ptr f1;};extern char Cyc_Core_Impossible[11];struct Cyc_Core_Impossible_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10];struct Cyc_Core_Not_found_struct{
char*tag;};extern char Cyc_Core_Unreachable[12];struct Cyc_Core_Unreachable_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Open_Region[12];struct Cyc_Core_Open_Region_struct{
char*tag;};extern char Cyc_Core_Free_Region[12];struct Cyc_Core_Free_Region_struct{
char*tag;};struct _dyneither_ptr wrap_Cbuffer_as_buffer(char*,unsigned long);
struct Cyc_in_addr{unsigned int s_addr;};unsigned int Cyc_htonl(unsigned int x);
unsigned short Cyc_htons(unsigned short x);unsigned int Cyc_ntohl(unsigned int x);
unsigned short Cyc_ntohs(unsigned short x);unsigned int __stub_htonl(unsigned int x);
unsigned short __stub_htons(unsigned short x);unsigned int __stub_ntohl(
unsigned int x);unsigned short __stub_ntohs(unsigned short x);unsigned int Cyc_htonl(
unsigned int x);unsigned int Cyc_htonl(unsigned int x){return __stub_htonl(x);}
unsigned short Cyc_htons(unsigned short x);unsigned short Cyc_htons(unsigned short x){
return __stub_htons(x);}unsigned int Cyc_ntohl(unsigned int x);unsigned int Cyc_ntohl(
unsigned int x){return __stub_ntohl(x);}unsigned short Cyc_ntohs(unsigned short x);
unsigned short Cyc_ntohs(unsigned short x){return __stub_ntohs(x);}struct Cyc_dirent{
unsigned int d_fileno;unsigned short d_reclen;unsigned char d_type;unsigned char
d_namlen;char d_name[256];};struct Cyc___cycDIR;int Cyc_closedir(struct Cyc___cycDIR*);
struct Cyc___cycDIR*Cyc_opendir(const char*);struct Cyc_dirent*Cyc_readdir(struct
Cyc___cycDIR*);void Cyc_rewinddir(struct Cyc___cycDIR*);void Cyc_seekdir(struct Cyc___cycDIR*,
long);long Cyc_telldir(struct Cyc___cycDIR*);struct Cyc_Cdirent___abstractDIR;
struct Cyc___cycDIR{struct Cyc_Cdirent___abstractDIR*dir;};int closedir(struct Cyc_Cdirent___abstractDIR*
d);int Cyc_closedir(struct Cyc___cycDIR*d);int Cyc_closedir(struct Cyc___cycDIR*d){
return closedir(d->dir);}struct Cyc_Cdirent___abstractDIR*opendir(const char*);
static char _tmp1[32]="opendir called with NULL string";static struct Cyc_Core_Failure_struct
Cyc___opendir_failure={Cyc_Core_Failure,{_tmp1,_tmp1,_tmp1 + 32}};struct Cyc___cycDIR*
Cyc_opendir(const char*f);struct Cyc___cycDIR*Cyc_opendir(const char*f){struct Cyc_Cdirent___abstractDIR*
_tmp2=opendir(f);struct Cyc___cycDIR*_tmpBE;return(unsigned int)_tmp2?(_tmpBE=
_cycalloc(sizeof(*_tmpBE)),((_tmpBE->dir=(struct Cyc_Cdirent___abstractDIR*)_tmp2,
_tmpBE))): 0;}struct Cyc_dirent*readdir(struct Cyc_Cdirent___abstractDIR*);struct
Cyc_dirent*Cyc_readdir(struct Cyc___cycDIR*d);struct Cyc_dirent*Cyc_readdir(struct
Cyc___cycDIR*d){return readdir(d->dir);}void rewinddir(struct Cyc_Cdirent___abstractDIR*
d);void Cyc_rewinddir(struct Cyc___cycDIR*d);void Cyc_rewinddir(struct Cyc___cycDIR*
d){return rewinddir(d->dir);}void seekdir(struct Cyc_Cdirent___abstractDIR*d,long);
void Cyc_seekdir(struct Cyc___cycDIR*d,long x);void Cyc_seekdir(struct Cyc___cycDIR*d,
long x){return seekdir(d->dir,x);}long telldir(struct Cyc_Cdirent___abstractDIR*d);
long Cyc_telldir(struct Cyc___cycDIR*d);long Cyc_telldir(struct Cyc___cycDIR*d){
return telldir(d->dir);}int*__CYCLONE_ERRNO();struct Cyc_flock{long long l_start;
long long l_len;int l_pid;short l_type;short l_whence;};struct Cyc_Flock_struct{int
tag;struct Cyc_flock*f1;};struct Cyc_Long_struct{int tag;long f1;};int Cyc_fcntl(int
fd,int cmd,struct _dyneither_ptr);int Cyc_open(const char*,int,struct _dyneither_ptr);
int fcntl(int fd,int cmd);int fcntl_with_arg(int fd,int cmd,long arg);int
fcntl_with_lock(int fd,int cmd,struct Cyc_flock*lock);int open_without_mode(const
char*,int);int open_with_mode(const char*,int,unsigned short);static char _tmp5[21]="fcntl: too many args";
static struct Cyc_Core_Failure_struct Cyc___fcntl_failure={Cyc_Core_Failure,{_tmp5,
_tmp5,_tmp5 + 21}};int Cyc_fcntl(int fd,int cmd,struct _dyneither_ptr argv);int Cyc_fcntl(
int fd,int cmd,struct _dyneither_ptr argv){if(_get_dyneither_size(argv,sizeof(void*))
== 0)return fcntl(fd,cmd);else{if(_get_dyneither_size(argv,sizeof(void*))!= 1)(
int)_throw((void*)& Cyc___fcntl_failure);else{void*_tmp6=*((void**)
_check_dyneither_subscript(argv,sizeof(void*),0));long _tmp8;struct Cyc_flock*
_tmpA;_LL1: {struct Cyc_Long_struct*_tmp7=(struct Cyc_Long_struct*)_tmp6;if(_tmp7->tag
!= 1)goto _LL3;else{_tmp8=_tmp7->f1;}}_LL2: return fcntl_with_arg(fd,cmd,_tmp8);
_LL3: {struct Cyc_Flock_struct*_tmp9=(struct Cyc_Flock_struct*)_tmp6;if(_tmp9->tag
!= 0)goto _LL0;else{_tmpA=_tmp9->f1;}}_LL4: return fcntl_with_lock(fd,cmd,(struct
Cyc_flock*)_tmpA);_LL0:;}}}int Cyc_open(const char*s,int i,struct _dyneither_ptr ms);
int Cyc_open(const char*s,int i,struct _dyneither_ptr ms){if(_get_dyneither_size(ms,
sizeof(unsigned short))>= 1)return open_with_mode(s,i,*((unsigned short*)
_check_dyneither_subscript(ms,sizeof(unsigned short),0)));else{return
open_without_mode(s,i);}}struct Cyc_servent{char*s_name;char**s_aliases;int s_port;
char*s_proto;};struct Cyc_protoent{char*p_name;char**p_aliases;int p_proto;};
struct Cyc_hostent{char*h_name;char**h_aliases;int h_addrtype;int h_length;struct
Cyc_in_addr**h_addr_list;};struct Cyc_sockaddr_in{unsigned char sin_len;
unsigned char sin_family;unsigned short sin_port;struct Cyc_in_addr sin_addr;char
sin_zero[8];};struct Cyc_in6_addr{union{unsigned char __u6_addr8[16];
unsigned short __u6_addr16[8];unsigned int __u6_addr32[4];}__u6_addr;};struct Cyc_sockaddr_in6{
unsigned char sin6_len;unsigned char sin6_family;unsigned short sin6_port;
unsigned int sin6_flowinfo;struct Cyc_in6_addr sin6_addr;unsigned int sin6_scope_id;
};struct Cyc_ipv6_mreq{struct Cyc_in6_addr ipv6mr_multiaddr;unsigned int
ipv6mr_interface;};enum Cyc_SigBuiltin{Cyc_SIG_DFL  = 0,Cyc_SIG_ERR  = 1,Cyc_SIG_IGN
 = 2};struct Cyc___SIG_BUILTIN_struct{int tag;enum Cyc_SigBuiltin f1;};struct Cyc___SIG_HANDLER_struct{
int tag;void(*f1)(int);};void*Cyc_signal(int,struct _dyneither_ptr);void*Cyc_Csignal_make_SIG_DFL();
void*Cyc_Csignal_make_SIG_DFL(){struct Cyc___SIG_BUILTIN_struct _tmpC1;struct Cyc___SIG_BUILTIN_struct*
_tmpC0;return(void*)((_tmpC0=_cycalloc_atomic(sizeof(*_tmpC0)),((_tmpC0[0]=((
_tmpC1.tag=0,((_tmpC1.f1=Cyc_SIG_DFL,_tmpC1)))),_tmpC0))));}void*Cyc_Csignal_make_SIG_ERR();
void*Cyc_Csignal_make_SIG_ERR(){struct Cyc___SIG_BUILTIN_struct _tmpC4;struct Cyc___SIG_BUILTIN_struct*
_tmpC3;return(void*)((_tmpC3=_cycalloc_atomic(sizeof(*_tmpC3)),((_tmpC3[0]=((
_tmpC4.tag=0,((_tmpC4.f1=Cyc_SIG_ERR,_tmpC4)))),_tmpC3))));}void*Cyc_Csignal_make_SIG_IGN();
void*Cyc_Csignal_make_SIG_IGN(){struct Cyc___SIG_BUILTIN_struct _tmpC7;struct Cyc___SIG_BUILTIN_struct*
_tmpC6;return(void*)((_tmpC6=_cycalloc_atomic(sizeof(*_tmpC6)),((_tmpC6[0]=((
_tmpC7.tag=0,((_tmpC7.f1=Cyc_SIG_ERR,_tmpC7)))),_tmpC6))));}void*Cyc_Csignal_make_SIG_HANDLER(
void(*f)(int));void*Cyc_Csignal_make_SIG_HANDLER(void(*f)(int)){struct Cyc___SIG_HANDLER_struct
_tmpCA;struct Cyc___SIG_HANDLER_struct*_tmpC9;return(void*)((_tmpC9=_cycalloc(
sizeof(*_tmpC9)),((_tmpC9[0]=((_tmpCA.tag=1,((_tmpCA.f1=f,_tmpCA)))),_tmpC9))));}
void*signal_SIG_DFL(int);void*signal_SIG_ERR(int);void*signal_SIG_IGN(int);void*
signal(int,void(*)(int));static char _tmp14[49]="signal called with the wrong number of arguments";
static struct Cyc_Core_Failure_struct Cyc___signal_failure={Cyc_Core_Failure,{
_tmp14,_tmp14,_tmp14 + 49}};void*Cyc_signal(int a,struct _dyneither_ptr b);void*Cyc_signal(
int a,struct _dyneither_ptr b){if(_get_dyneither_size(b,sizeof(void*))!= 1)(int)
_throw((void*)& Cyc___signal_failure);{void*_tmp15=*((void**)
_check_dyneither_subscript(b,sizeof(void*),0));enum Cyc_SigBuiltin _tmp17;enum 
Cyc_SigBuiltin _tmp19;enum Cyc_SigBuiltin _tmp1B;void(*_tmp1D)(int);_LL6: {struct
Cyc___SIG_BUILTIN_struct*_tmp16=(struct Cyc___SIG_BUILTIN_struct*)_tmp15;if(
_tmp16->tag != 0)goto _LL8;else{_tmp17=_tmp16->f1;if(_tmp17 != Cyc_SIG_DFL)goto _LL8;}}
_LL7: return signal_SIG_DFL(a);_LL8: {struct Cyc___SIG_BUILTIN_struct*_tmp18=(
struct Cyc___SIG_BUILTIN_struct*)_tmp15;if(_tmp18->tag != 0)goto _LLA;else{_tmp19=
_tmp18->f1;if(_tmp19 != Cyc_SIG_ERR)goto _LLA;}}_LL9: return signal_SIG_ERR(a);_LLA: {
struct Cyc___SIG_BUILTIN_struct*_tmp1A=(struct Cyc___SIG_BUILTIN_struct*)_tmp15;
if(_tmp1A->tag != 0)goto _LLC;else{_tmp1B=_tmp1A->f1;if(_tmp1B != Cyc_SIG_IGN)goto
_LLC;}}_LLB: return signal_SIG_IGN(a);_LLC: {struct Cyc___SIG_HANDLER_struct*_tmp1C=(
struct Cyc___SIG_HANDLER_struct*)_tmp15;if(_tmp1C->tag != 1)goto _LL5;else{_tmp1D=
_tmp1C->f1;}}_LLD: return signal(a,_tmp1D);_LL5:;};}struct Cyc___cycFILE;struct Cyc_Cstdio___abstractFILE;
struct Cyc___cycFILE*Cyc_fromCfile(struct Cyc_Cstdio___abstractFILE*cf);struct Cyc_String_pa_struct{
int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_struct{int tag;unsigned long f1;}
;struct Cyc_Double_pa_struct{int tag;double f1;};struct Cyc_LongDouble_pa_struct{int
tag;long double f1;};struct Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{
int tag;unsigned long*f1;};void Cyc_clearerr(struct Cyc___cycFILE*);int Cyc_fclose(
struct Cyc___cycFILE*);struct Cyc___cycFILE*Cyc_fdopen(int,const char*);int Cyc_feof(
struct Cyc___cycFILE*);int Cyc_ferror(struct Cyc___cycFILE*);int Cyc_fflush(struct
Cyc___cycFILE*);int Cyc_fgetc(struct Cyc___cycFILE*);int Cyc_fgetpos(struct Cyc___cycFILE*,
long long*);struct _dyneither_ptr Cyc_fgets(struct _dyneither_ptr,int,struct Cyc___cycFILE*);
int Cyc_fileno(struct Cyc___cycFILE*);struct Cyc___cycFILE*Cyc_fopen(const char*,
const char*);int Cyc_fputc(int,struct Cyc___cycFILE*);int Cyc_fputs(const char*,
struct Cyc___cycFILE*);unsigned long Cyc_fread(struct _dyneither_ptr,unsigned long,
unsigned long,struct Cyc___cycFILE*);struct Cyc___cycFILE*Cyc_freopen(const char*,
const char*,struct Cyc___cycFILE*);struct Cyc_ShortPtr_sa_struct{int tag;short*f1;};
struct Cyc_UShortPtr_sa_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_struct{
int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_struct{
int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_struct{int tag;double*f1;}
;struct Cyc_FloatPtr_sa_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int
tag;struct _dyneither_ptr f1;};int Cyc_fseek(struct Cyc___cycFILE*,long,int);int Cyc_fseeko(
struct Cyc___cycFILE*,long long,int);int Cyc_fsetpos(struct Cyc___cycFILE*,const
long long*);long Cyc_ftell(struct Cyc___cycFILE*);long long Cyc_ftello(struct Cyc___cycFILE*);
unsigned long Cyc_fwrite(struct _dyneither_ptr,unsigned long,unsigned long,struct
Cyc___cycFILE*);int Cyc_getc(struct Cyc___cycFILE*);int Cyc_getchar();int Cyc_pclose(
struct Cyc___cycFILE*);struct Cyc___cycFILE*Cyc_popen(const char*,const char*);int
Cyc_putc(int,struct Cyc___cycFILE*);void Cyc_rewind(struct Cyc___cycFILE*);int Cyc_setvbuf(
struct Cyc___cycFILE*,struct _dyneither_ptr,int,unsigned long);void Cyc_setbuf(
struct Cyc___cycFILE*,struct _dyneither_ptr);int Cyc_ungetc(int,struct Cyc___cycFILE*);
int Cyc_getw(struct Cyc___cycFILE*);int Cyc_putw(int,struct Cyc___cycFILE*);extern
char Cyc_FileCloseError[15];struct Cyc_FileCloseError_struct{char*tag;};extern char
Cyc_FileOpenError[14];struct Cyc_FileOpenError_struct{char*tag;struct
_dyneither_ptr f1;};struct Cyc___cycFILE*Cyc_file_open(struct _dyneither_ptr,struct
_dyneither_ptr);void Cyc_file_close(struct Cyc___cycFILE*);struct Cyc_Cstdio___abstractFILE;
struct Cyc___cycFILE{struct Cyc_Cstdio___abstractFILE*file;};struct Cyc___cycFILE*
Cyc_fromCfile(struct Cyc_Cstdio___abstractFILE*cf);struct Cyc___cycFILE*Cyc_fromCfile(
struct Cyc_Cstdio___abstractFILE*cf){struct Cyc___cycFILE*_tmpCB;return(
unsigned int)cf?(_tmpCB=_cycalloc(sizeof(*_tmpCB)),((_tmpCB->file=(struct Cyc_Cstdio___abstractFILE*)
cf,_tmpCB))): 0;}void clearerr(struct Cyc_Cstdio___abstractFILE*);void Cyc_clearerr(
struct Cyc___cycFILE*f);void Cyc_clearerr(struct Cyc___cycFILE*f){return clearerr(f->file);}
int fclose(struct Cyc_Cstdio___abstractFILE*);int Cyc_fclose(struct Cyc___cycFILE*f);
int Cyc_fclose(struct Cyc___cycFILE*f){return fclose(f->file);}struct Cyc_Cstdio___abstractFILE*
fdopen(int,const char*);struct Cyc___cycFILE*Cyc_fdopen(int i,const char*s);struct
Cyc___cycFILE*Cyc_fdopen(int i,const char*s){struct Cyc_Cstdio___abstractFILE*
_tmp1F=fdopen(i,s);struct Cyc___cycFILE*_tmpCC;return(unsigned int)_tmp1F?(_tmpCC=
_cycalloc(sizeof(*_tmpCC)),((_tmpCC->file=(struct Cyc_Cstdio___abstractFILE*)
_tmp1F,_tmpCC))): 0;}int feof(struct Cyc_Cstdio___abstractFILE*);int Cyc_feof(struct
Cyc___cycFILE*f);int Cyc_feof(struct Cyc___cycFILE*f){return feof(f->file);}int
ferror(struct Cyc_Cstdio___abstractFILE*);int Cyc_ferror(struct Cyc___cycFILE*f);
int Cyc_ferror(struct Cyc___cycFILE*f){return ferror(f->file);}int fflush(struct Cyc_Cstdio___abstractFILE*);
int Cyc_fflush(struct Cyc___cycFILE*f);int Cyc_fflush(struct Cyc___cycFILE*f){if((
unsigned int)f)return fflush((struct Cyc_Cstdio___abstractFILE*)f->file);else{
return fflush(0);}}int fgetc(struct Cyc_Cstdio___abstractFILE*);int Cyc_fgetc(struct
Cyc___cycFILE*f);int Cyc_fgetc(struct Cyc___cycFILE*f){return fgetc(f->file);}int
fgetpos(struct Cyc_Cstdio___abstractFILE*,long long*);int Cyc_fgetpos(struct Cyc___cycFILE*
f,long long*x);int Cyc_fgetpos(struct Cyc___cycFILE*f,long long*x){return fgetpos(f->file,
x);}char*fgets(char*,int,struct Cyc_Cstdio___abstractFILE*);static char _tmp22[30]="fgets called with NULL string";
static struct Cyc_Core_Failure_struct Cyc___fgets_failure_1={Cyc_Core_Failure,{
_tmp22,_tmp22,_tmp22 + 30}};static char _tmp24[27]="fgets: buffer insufficient";
static struct Cyc_Core_Failure_struct Cyc___fgets_failure_2={Cyc_Core_Failure,{
_tmp24,_tmp24,_tmp24 + 27}};struct _dyneither_ptr Cyc_fgets(struct _dyneither_ptr s,
int n,struct Cyc___cycFILE*f);struct _dyneither_ptr Cyc_fgets(struct _dyneither_ptr s,
int n,struct Cyc___cycFILE*f){if(!((unsigned int)s.curr))(int)_throw((void*)& Cyc___fgets_failure_1);{
char*result;char*buffer=(char*)_check_null(_untag_dyneither_ptr(s,sizeof(char),1));
unsigned int len=_get_dyneither_size(s,sizeof(char));if(len < n)(int)_throw((void*)&
Cyc___fgets_failure_2);result=fgets(buffer,n,f->file);if(result == 0)return
_tag_dyneither(0,0,0);else{return s;}};}int fileno(struct Cyc_Cstdio___abstractFILE*);
int Cyc_fileno(struct Cyc___cycFILE*f);int Cyc_fileno(struct Cyc___cycFILE*f){return
fileno(f->file);}struct Cyc_Cstdio___abstractFILE*fopen(const char*,const char*);
struct Cyc___cycFILE*Cyc_fopen(const char*name,const char*type);struct Cyc___cycFILE*
Cyc_fopen(const char*name,const char*type){struct Cyc_Cstdio___abstractFILE*_tmp25=
fopen(name,type);struct Cyc___cycFILE*_tmpCD;return(unsigned int)_tmp25?(_tmpCD=
_cycalloc(sizeof(*_tmpCD)),((_tmpCD->file=(struct Cyc_Cstdio___abstractFILE*)
_tmp25,_tmpCD))): 0;}int fputc(int,struct Cyc_Cstdio___abstractFILE*);int Cyc_fputc(
int x,struct Cyc___cycFILE*f);int Cyc_fputc(int x,struct Cyc___cycFILE*f){return fputc(
x,f->file);}int fputs(const char*,struct Cyc_Cstdio___abstractFILE*);int Cyc_fputs(
const char*x,struct Cyc___cycFILE*f);int Cyc_fputs(const char*x,struct Cyc___cycFILE*
f){return fputs(x,f->file);}unsigned long fread(char*,unsigned long,unsigned long,
struct Cyc_Cstdio___abstractFILE*);static char _tmp28[27]="fread: buffer insufficient";
static struct Cyc_Core_Failure_struct Cyc___fread_failure={Cyc_Core_Failure,{_tmp28,
_tmp28,_tmp28 + 27}};unsigned long Cyc_fread(struct _dyneither_ptr ptr,unsigned long
size,unsigned long nmemb,struct Cyc___cycFILE*f);unsigned long Cyc_fread(struct
_dyneither_ptr ptr,unsigned long size,unsigned long nmemb,struct Cyc___cycFILE*f){
if(size * nmemb > _get_dyneither_size(ptr,sizeof(char)))(int)_throw((void*)& Cyc___fread_failure);
return fread((char*)_check_null(_untag_dyneither_ptr(ptr,sizeof(char),1)),size,
nmemb,f->file);}struct Cyc_Cstdio___abstractFILE*freopen(const char*,const char*,
struct Cyc_Cstdio___abstractFILE*);struct Cyc___cycFILE*Cyc_freopen(const char*
filename,const char*modes,struct Cyc___cycFILE*f);struct Cyc___cycFILE*Cyc_freopen(
const char*filename,const char*modes,struct Cyc___cycFILE*f){struct Cyc_Cstdio___abstractFILE*
_tmp29=freopen(filename,modes,f->file);struct Cyc___cycFILE*_tmpCE;return(
unsigned int)_tmp29?(struct Cyc___cycFILE*)((_tmpCE=_cycalloc(sizeof(*_tmpCE)),((
_tmpCE->file=(struct Cyc_Cstdio___abstractFILE*)_tmp29,_tmpCE)))): 0;}int fseek(
struct Cyc_Cstdio___abstractFILE*,long,int);int Cyc_fseek(struct Cyc___cycFILE*a,
long b,int c);int Cyc_fseek(struct Cyc___cycFILE*a,long b,int c){return fseek(a->file,b,
c);}int fseeko(struct Cyc_Cstdio___abstractFILE*,long long,int);int Cyc_fseeko(
struct Cyc___cycFILE*a,long long b,int c);int Cyc_fseeko(struct Cyc___cycFILE*a,
long long b,int c){return fseeko(a->file,b,c);}int fsetpos(struct Cyc_Cstdio___abstractFILE*,
const long long*);int Cyc_fsetpos(struct Cyc___cycFILE*f,const long long*b);int Cyc_fsetpos(
struct Cyc___cycFILE*f,const long long*b){return fsetpos(f->file,b);}long ftell(
struct Cyc_Cstdio___abstractFILE*);long Cyc_ftell(struct Cyc___cycFILE*f);long Cyc_ftell(
struct Cyc___cycFILE*f){return ftell(f->file);}long long ftello(struct Cyc_Cstdio___abstractFILE*);
long long Cyc_ftello(struct Cyc___cycFILE*f);long long Cyc_ftello(struct Cyc___cycFILE*
f){return ftello(f->file);}unsigned long fwrite(const char*,unsigned long,
unsigned long,struct Cyc_Cstdio___abstractFILE*);static char _tmp2C[31]="fwrite called with NULL string";
static struct Cyc_Core_Failure_struct Cyc___fwrite_failure_1={Cyc_Core_Failure,{
_tmp2C,_tmp2C,_tmp2C + 31}};static char _tmp2E[28]="fwrite: buffer insufficient";
static struct Cyc_Core_Failure_struct Cyc___fwrite_failure_2={Cyc_Core_Failure,{
_tmp2E,_tmp2E,_tmp2E + 28}};unsigned long Cyc_fwrite(struct _dyneither_ptr ptr,
unsigned long size,unsigned long nmemb,struct Cyc___cycFILE*f);unsigned long Cyc_fwrite(
struct _dyneither_ptr ptr,unsigned long size,unsigned long nmemb,struct Cyc___cycFILE*
f){if(!((unsigned int)ptr.curr))(int)_throw((void*)& Cyc___fwrite_failure_1);
else{if(size * nmemb > _get_dyneither_size(ptr,sizeof(char)))(int)_throw((void*)&
Cyc___fwrite_failure_2);return fwrite((const char*)_untag_dyneither_ptr(ptr,
sizeof(char),1),size,nmemb,f->file);}}int getc(struct Cyc_Cstdio___abstractFILE*);
int Cyc_getc(struct Cyc___cycFILE*f);int Cyc_getc(struct Cyc___cycFILE*f){return getc(
f->file);}int getchar();int Cyc_getchar();int Cyc_getchar(){return getchar();}int
pclose(struct Cyc_Cstdio___abstractFILE*);int Cyc_pclose(struct Cyc___cycFILE*f);
int Cyc_pclose(struct Cyc___cycFILE*f){return pclose(f->file);}struct Cyc_Cstdio___abstractFILE*
popen(const char*,const char*);struct Cyc___cycFILE*Cyc_popen(const char*name,const
char*type);struct Cyc___cycFILE*Cyc_popen(const char*name,const char*type){struct
Cyc_Cstdio___abstractFILE*_tmp2F=popen(name,type);struct Cyc___cycFILE*_tmpCF;
return(unsigned int)_tmp2F?(_tmpCF=_cycalloc(sizeof(*_tmpCF)),((_tmpCF->file=(
struct Cyc_Cstdio___abstractFILE*)_tmp2F,_tmpCF))): 0;}int putc(int,struct Cyc_Cstdio___abstractFILE*);
int Cyc_putc(int x,struct Cyc___cycFILE*f);int Cyc_putc(int x,struct Cyc___cycFILE*f){
return putc(x,f->file);}void rewind(struct Cyc_Cstdio___abstractFILE*);void Cyc_rewind(
struct Cyc___cycFILE*f);void Cyc_rewind(struct Cyc___cycFILE*f){return rewind(f->file);}
int setvbuf(struct Cyc_Cstdio___abstractFILE*,char*,int,unsigned long);static char
_tmp32[24]="setvbuf: buffer is NULL";static struct Cyc_Core_Failure_struct Cyc___setvbuf_failure_1={
Cyc_Core_Failure,{_tmp32,_tmp32,_tmp32 + 24}};static char _tmp34[29]="setvbuf: buffer insufficient";
static struct Cyc_Core_Failure_struct Cyc___setvbuf_failure_2={Cyc_Core_Failure,{
_tmp34,_tmp34,_tmp34 + 29}};int Cyc_setvbuf(struct Cyc___cycFILE*f,struct
_dyneither_ptr buf,int type,unsigned long size);int Cyc_setvbuf(struct Cyc___cycFILE*
f,struct _dyneither_ptr buf,int type,unsigned long size){if(!((unsigned int)buf.curr))(
int)_throw((void*)& Cyc___setvbuf_failure_1);if(_get_dyneither_size(buf,sizeof(
char))< size)(int)_throw((void*)& Cyc___setvbuf_failure_2);return setvbuf(f->file,(
char*)_check_null(_untag_dyneither_ptr(buf,sizeof(char),1)),type,size);}void Cyc_setbuf(
struct Cyc___cycFILE*f,struct _dyneither_ptr buf);void Cyc_setbuf(struct Cyc___cycFILE*
f,struct _dyneither_ptr buf){Cyc_setvbuf(f,buf,(unsigned int)buf.curr?0: 2,1024);
return;}int ungetc(int,struct Cyc_Cstdio___abstractFILE*);int Cyc_ungetc(int x,
struct Cyc___cycFILE*f);int Cyc_ungetc(int x,struct Cyc___cycFILE*f){return ungetc(x,
f->file);}int getw(struct Cyc_Cstdio___abstractFILE*);int Cyc_getw(struct Cyc___cycFILE*
f);int Cyc_getw(struct Cyc___cycFILE*f){return getw(f->file);}int putw(int,struct Cyc_Cstdio___abstractFILE*);
int Cyc_putw(int x,struct Cyc___cycFILE*f);int Cyc_putw(int x,struct Cyc___cycFILE*f){
return putw(x,f->file);}char Cyc_FileCloseError[15]="FileCloseError";char Cyc_FileOpenError[
14]="FileOpenError";struct Cyc___cycFILE*Cyc_file_open(struct _dyneither_ptr fname,
struct _dyneither_ptr mode);static void _tmpD5(struct _dyneither_ptr*fname,
unsigned int*_tmpD4,unsigned int*_tmpD3,char**_tmpD1){for(*_tmpD4=0;*_tmpD4 < *
_tmpD3;(*_tmpD4)++){(*_tmpD1)[*_tmpD4]=((const char*)(*fname).curr)[(int)*_tmpD4];}}
struct Cyc___cycFILE*Cyc_file_open(struct _dyneither_ptr fname,struct _dyneither_ptr
mode){struct Cyc___cycFILE*f=Cyc_fopen((const char*)_untag_dyneither_ptr(fname,
sizeof(char),1),(const char*)_untag_dyneither_ptr(mode,sizeof(char),1));if(f == 0){
unsigned int _tmpD4;unsigned int _tmpD3;struct _dyneither_ptr _tmpD2;char*_tmpD1;
unsigned int _tmpD0;struct _dyneither_ptr fn=(_tmpD0=_get_dyneither_size(fname,
sizeof(char)),((_tmpD1=(char*)_cycalloc_atomic(_check_times(sizeof(char),_tmpD0 + 
1)),((_tmpD2=_tag_dyneither(_tmpD1,sizeof(char),_tmpD0 + 1),((((_tmpD3=_tmpD0,((
_tmpD5(& fname,& _tmpD4,& _tmpD3,& _tmpD1),_tmpD1[_tmpD3]=(char)0)))),_tmpD2)))))));
struct Cyc_FileOpenError_struct _tmpD8;struct Cyc_FileOpenError_struct*_tmpD7;(int)
_throw((void*)((_tmpD7=_cycalloc(sizeof(*_tmpD7)),((_tmpD7[0]=((_tmpD8.tag=Cyc_FileOpenError,((
_tmpD8.f1=fn,_tmpD8)))),_tmpD7)))));}return(struct Cyc___cycFILE*)f;}struct Cyc_FileCloseError_struct
Cyc_FileCloseError_val={Cyc_FileCloseError};void Cyc_file_close(struct Cyc___cycFILE*
f);void Cyc_file_close(struct Cyc___cycFILE*f){if(Cyc_fclose(f)!= 0)(int)_throw((
void*)& Cyc_FileCloseError_val);}typedef struct{int quot;int rem;}Cyc_div_t;typedef
struct{long quot;long rem;}Cyc_ldiv_t;void*abort();void exit(int);void Cyc_free(
struct _dyneither_ptr);void Cyc_qsort(struct _dyneither_ptr,unsigned long,
unsigned int,int(*)(const void*,const void*));void Cyc_free(struct _dyneither_ptr ptr);
void Cyc_free(struct _dyneither_ptr ptr){;}void qsort(void*,unsigned long,
unsigned long,int(*)(const void*,const void*));static char _tmp3D[23]="qsort called with NULL";
static struct Cyc_Core_Failure_struct Cyc___qsort_failure_1={Cyc_Core_Failure,{
_tmp3D,_tmp3D,_tmp3D + 23}};static char _tmp3F[42]="qsort called with insufficient array size";
static struct Cyc_Core_Failure_struct Cyc___qsort_failure_2={Cyc_Core_Failure,{
_tmp3F,_tmp3F,_tmp3F + 42}};void Cyc_qsort(struct _dyneither_ptr tab,unsigned long
nmemb,unsigned int szmemb,int(*compar)(const void*,const void*));void Cyc_qsort(
struct _dyneither_ptr tab,unsigned long nmemb,unsigned int szmemb,int(*compar)(const
void*,const void*)){if(!((unsigned int)tab.curr))(int)_throw((void*)& Cyc___qsort_failure_1);
if(_get_dyneither_size(tab,sizeof(void))< nmemb)(int)_throw((void*)& Cyc___qsort_failure_2);
qsort((void*)_untag_dyneither_ptr(tab,sizeof(void),1),nmemb,(unsigned long)
szmemb,compar);}struct Cyc_ipc_perm{unsigned short cuid;unsigned short cgid;
unsigned short uid;unsigned short gid;unsigned short mode;unsigned short seq;long key;
};struct _dyneither_ptr Cyc_mmap(struct _dyneither_ptr,unsigned long length,int prot,
int flags,int fd,long long offset);int Cyc_munmap(struct _dyneither_ptr start,
unsigned long length);char*__stub_mmap(char*start,unsigned long length,int prot,int
flags,int fd,long long offset);int munmap(const char*start,unsigned long length);
static char _tmp41[42]="mmap called with illegal start/prot/flags";static struct Cyc_Core_Failure_struct
Cyc___mmap_failure={Cyc_Core_Failure,{_tmp41,_tmp41,_tmp41 + 42}};struct
_dyneither_ptr Cyc_mmap(struct _dyneither_ptr ignored,unsigned long length,int prot,
int flags,int fd,long long offset);struct _dyneither_ptr Cyc_mmap(struct
_dyneither_ptr ignored,unsigned long length,int prot,int flags,int fd,long long offset){
if((ignored.curr == (_tag_dyneither(0,0,0)).curr  && prot == 1) && (flags == 2  || 
flags == 1))return(struct _dyneither_ptr)wrap_Cbuffer_as_buffer(__stub_mmap(0,
length,prot,flags,fd,offset),length);else{(int)_throw((void*)& Cyc___mmap_failure);}}
int Cyc_munmap(struct _dyneither_ptr start,unsigned long length);int Cyc_munmap(
struct _dyneither_ptr start,unsigned long length){return munmap((const char*)
_check_null(_untag_dyneither_ptr(start,sizeof(char),1)),length);}struct Cyc_timeval{
int tv_sec;int tv_usec;};struct Cyc_rusage{struct Cyc_timeval ru_utime;struct Cyc_timeval
ru_stime;long ru_maxrss;long ru_ixrss;long ru_idrss;long ru_isrss;long ru_minflt;long
ru_majflt;long ru_nswap;long ru_inblock;long ru_oublock;long ru_msgsnd;long ru_msgrcv;
long ru_nsignals;long ru_nvcsw;long ru_nivcsw;};struct Cyc_rlimit{long long rlim_cur;
long long rlim_max;};struct Cyc_fd_set{int fds_bits[(1024 + (sizeof(int)* 8 - 1))/ (
sizeof(int)* 8)];};struct Cyc_timespec{long tv_sec;int tv_nsec;};int Cyc_select(int a,
struct Cyc_fd_set*b,struct Cyc_fd_set*c,struct Cyc_fd_set*d,struct Cyc_timeval*e);
void Cyc_FD_CLR(int,struct Cyc_fd_set*);int Cyc_FD_ISSET(int,struct Cyc_fd_set*);
void Cyc_FD_SET(int,struct Cyc_fd_set*);void Cyc_FD_ZERO(struct Cyc_fd_set*);int
select(int,struct Cyc_fd_set*,struct Cyc_fd_set*,struct Cyc_fd_set*,struct Cyc_timeval*);
int Cyc_select(int a,struct Cyc_fd_set*b,struct Cyc_fd_set*c,struct Cyc_fd_set*d,
struct Cyc_timeval*e);extern int Cyc_select(int a,struct Cyc_fd_set*b,struct Cyc_fd_set*
c,struct Cyc_fd_set*d,struct Cyc_timeval*e){if(a < 0  || a > 1024){*((int*)
_check_null(__CYCLONE_ERRNO()))=22;return - 1;}return select(a,b,c,d,e);}void
__stub_FD_CLR(int,struct Cyc_fd_set*);int __stub_FD_ISSET(int,struct Cyc_fd_set*);
void __stub_FD_SET(int,struct Cyc_fd_set*);void __stub_FD_ZERO(struct Cyc_fd_set*);
void Cyc_FD_CLR(int a,struct Cyc_fd_set*b);void Cyc_FD_CLR(int a,struct Cyc_fd_set*b){
if(a < 0  || a > 1024)return;return __stub_FD_CLR(a,b);}int Cyc_FD_ISSET(int a,struct
Cyc_fd_set*b);int Cyc_FD_ISSET(int a,struct Cyc_fd_set*b){if(a < 0  || a > 1024)return
0;return __stub_FD_ISSET(a,b);}void Cyc_FD_SET(int a,struct Cyc_fd_set*b);void Cyc_FD_SET(
int a,struct Cyc_fd_set*b){if(a < 0  || a > 1024)return;return __stub_FD_SET(a,b);}
void Cyc_FD_ZERO(struct Cyc_fd_set*a);void Cyc_FD_ZERO(struct Cyc_fd_set*a){return
__stub_FD_ZERO(a);}struct Cyc_cmsghdr;struct Cyc_linger{int l_onoff;int l_linger;};
struct Cyc_SA_sockaddr_in_struct{int tag;struct Cyc_sockaddr_in*f1;};struct Cyc_SA_socklenptr_struct{
int tag;int*f1;};struct Cyc_SA_socklen_struct{int tag;int f1;};struct Cyc_SO_int_struct{
int tag;int*f1;};struct Cyc_SO_timeval_struct{int tag;struct Cyc_timeval*f1;};struct
Cyc_SO_socklenptr_struct{int tag;int*f1;};struct Cyc_SO_socklen_struct{int tag;int
f1;};int Cyc_accept(int fd,struct _dyneither_ptr);extern char Cyc_SocketError[12];
struct Cyc_SocketError_struct{char*tag;};int Cyc_bind(int fd,struct _dyneither_ptr);
int Cyc_connect(int fd,struct _dyneither_ptr);int Cyc_getpeername(int fd,struct
_dyneither_ptr);int Cyc_getsockname(int fd,struct _dyneither_ptr);int Cyc_getsockopt(
int fd,int level,int optname,struct _dyneither_ptr);int Cyc_recv(int fd,struct
_dyneither_ptr buf,unsigned long n,int flags);int Cyc_recvfrom(int fd,struct
_dyneither_ptr buf,unsigned long n,int flags,struct _dyneither_ptr);int Cyc_send(int
fd,struct _dyneither_ptr buf,unsigned long n,int flags);int Cyc_sendto(int fd,struct
_dyneither_ptr buf,unsigned long n,int flags,struct _dyneither_ptr);int Cyc_setsockopt(
int fd,int level,int optname,struct _dyneither_ptr);char Cyc_SocketError[12]="SocketError";
struct Cyc_SocketError_struct Cyc_SocketError_val={Cyc_SocketError};int accept_in(
int,const struct Cyc_sockaddr_in*,int*);struct _tuple0{void*f1;void*f2;};int Cyc_accept(
int fd,struct _dyneither_ptr ap);int Cyc_accept(int fd,struct _dyneither_ptr ap){if(
_get_dyneither_size(ap,sizeof(void*))!= 2)(int)_throw((void*)& Cyc_SocketError_val);{
struct _tuple0 _tmpD9;struct _tuple0 _tmp44=(_tmpD9.f1=*((void**)
_check_dyneither_subscript(ap,sizeof(void*),0)),((_tmpD9.f2=*((void**)
_check_dyneither_subscript(ap,sizeof(void*),1)),_tmpD9)));void*_tmp45;struct Cyc_sockaddr_in*
_tmp47;void*_tmp48;int*_tmp4A;_LLF: _tmp45=_tmp44.f1;{struct Cyc_SA_sockaddr_in_struct*
_tmp46=(struct Cyc_SA_sockaddr_in_struct*)_tmp45;if(_tmp46->tag != 0)goto _LL11;
else{_tmp47=_tmp46->f1;}};_tmp48=_tmp44.f2;{struct Cyc_SA_socklenptr_struct*
_tmp49=(struct Cyc_SA_socklenptr_struct*)_tmp48;if(_tmp49->tag != 1)goto _LL11;
else{_tmp4A=_tmp49->f1;}};_LL10: return accept_in(fd,(const struct Cyc_sockaddr_in*)
_tmp47,_tmp4A);_LL11:;_LL12:(int)_throw((void*)& Cyc_SocketError_val);_LLE:;};}
int bind_in(int,const struct Cyc_sockaddr_in*,int);int Cyc_bind(int fd,struct
_dyneither_ptr ap);int Cyc_bind(int fd,struct _dyneither_ptr ap){if(
_get_dyneither_size(ap,sizeof(void*))!= 2)(int)_throw((void*)& Cyc_SocketError_val);{
struct _tuple0 _tmpDA;struct _tuple0 _tmp4C=(_tmpDA.f1=*((void**)
_check_dyneither_subscript(ap,sizeof(void*),0)),((_tmpDA.f2=*((void**)
_check_dyneither_subscript(ap,sizeof(void*),1)),_tmpDA)));void*_tmp4D;struct Cyc_sockaddr_in*
_tmp4F;void*_tmp50;int _tmp52;_LL14: _tmp4D=_tmp4C.f1;{struct Cyc_SA_sockaddr_in_struct*
_tmp4E=(struct Cyc_SA_sockaddr_in_struct*)_tmp4D;if(_tmp4E->tag != 0)goto _LL16;
else{_tmp4F=_tmp4E->f1;}};_tmp50=_tmp4C.f2;{struct Cyc_SA_socklen_struct*_tmp51=(
struct Cyc_SA_socklen_struct*)_tmp50;if(_tmp51->tag != 2)goto _LL16;else{_tmp52=
_tmp51->f1;}};_LL15: return bind_in(fd,(const struct Cyc_sockaddr_in*)_tmp4F,_tmp52);
_LL16:;_LL17:(int)_throw((void*)& Cyc_SocketError_val);_LL13:;};}int connect_in(
int,const struct Cyc_sockaddr_in*,int);int Cyc_connect(int fd,struct _dyneither_ptr ap);
int Cyc_connect(int fd,struct _dyneither_ptr ap){if(_get_dyneither_size(ap,sizeof(
void*))!= 2)(int)_throw((void*)& Cyc_SocketError_val);{struct _tuple0 _tmpDB;struct
_tuple0 _tmp54=(_tmpDB.f1=*((void**)_check_dyneither_subscript(ap,sizeof(void*),0)),((
_tmpDB.f2=*((void**)_check_dyneither_subscript(ap,sizeof(void*),1)),_tmpDB)));
void*_tmp55;struct Cyc_sockaddr_in*_tmp57;void*_tmp58;int _tmp5A;_LL19: _tmp55=
_tmp54.f1;{struct Cyc_SA_sockaddr_in_struct*_tmp56=(struct Cyc_SA_sockaddr_in_struct*)
_tmp55;if(_tmp56->tag != 0)goto _LL1B;else{_tmp57=_tmp56->f1;}};_tmp58=_tmp54.f2;{
struct Cyc_SA_socklen_struct*_tmp59=(struct Cyc_SA_socklen_struct*)_tmp58;if(
_tmp59->tag != 2)goto _LL1B;else{_tmp5A=_tmp59->f1;}};_LL1A: return connect_in(fd,(
const struct Cyc_sockaddr_in*)_tmp57,_tmp5A);_LL1B:;_LL1C:(int)_throw((void*)& Cyc_SocketError_val);
_LL18:;};}int getpeername_in(int,const struct Cyc_sockaddr_in*,int*);int Cyc_getpeername(
int fd,struct _dyneither_ptr ap);int Cyc_getpeername(int fd,struct _dyneither_ptr ap){
if(_get_dyneither_size(ap,sizeof(void*))!= 2)(int)_throw((void*)& Cyc_SocketError_val);{
struct _tuple0 _tmpDC;struct _tuple0 _tmp5C=(_tmpDC.f1=*((void**)
_check_dyneither_subscript(ap,sizeof(void*),0)),((_tmpDC.f2=*((void**)
_check_dyneither_subscript(ap,sizeof(void*),1)),_tmpDC)));void*_tmp5D;struct Cyc_sockaddr_in*
_tmp5F;void*_tmp60;int*_tmp62;_LL1E: _tmp5D=_tmp5C.f1;{struct Cyc_SA_sockaddr_in_struct*
_tmp5E=(struct Cyc_SA_sockaddr_in_struct*)_tmp5D;if(_tmp5E->tag != 0)goto _LL20;
else{_tmp5F=_tmp5E->f1;}};_tmp60=_tmp5C.f2;{struct Cyc_SA_socklenptr_struct*
_tmp61=(struct Cyc_SA_socklenptr_struct*)_tmp60;if(_tmp61->tag != 1)goto _LL20;
else{_tmp62=_tmp61->f1;}};_LL1F: return getpeername_in(fd,(const struct Cyc_sockaddr_in*)
_tmp5F,_tmp62);_LL20:;_LL21:(int)_throw((void*)& Cyc_SocketError_val);_LL1D:;};}
int getsockname_in(int,const struct Cyc_sockaddr_in*,int*);int Cyc_getsockname(int fd,
struct _dyneither_ptr ap);int Cyc_getsockname(int fd,struct _dyneither_ptr ap){if(
_get_dyneither_size(ap,sizeof(void*))!= 2)(int)_throw((void*)& Cyc_SocketError_val);{
struct _tuple0 _tmpDD;struct _tuple0 _tmp64=(_tmpDD.f1=*((void**)
_check_dyneither_subscript(ap,sizeof(void*),0)),((_tmpDD.f2=*((void**)
_check_dyneither_subscript(ap,sizeof(void*),1)),_tmpDD)));void*_tmp65;struct Cyc_sockaddr_in*
_tmp67;void*_tmp68;int*_tmp6A;_LL23: _tmp65=_tmp64.f1;{struct Cyc_SA_sockaddr_in_struct*
_tmp66=(struct Cyc_SA_sockaddr_in_struct*)_tmp65;if(_tmp66->tag != 0)goto _LL25;
else{_tmp67=_tmp66->f1;}};_tmp68=_tmp64.f2;{struct Cyc_SA_socklenptr_struct*
_tmp69=(struct Cyc_SA_socklenptr_struct*)_tmp68;if(_tmp69->tag != 1)goto _LL25;
else{_tmp6A=_tmp69->f1;}};_LL24: return getsockname_in(fd,(const struct Cyc_sockaddr_in*)
_tmp67,_tmp6A);_LL25:;_LL26:(int)_throw((void*)& Cyc_SocketError_val);_LL22:;};}
int getsockopt_int(int,int,int,int*,int*);int getsockopt_timeval(int,int,int,
struct Cyc_timeval*,int*);int Cyc_getsockopt(int fd,int level,int optname,struct
_dyneither_ptr ap);int Cyc_getsockopt(int fd,int level,int optname,struct
_dyneither_ptr ap){if(_get_dyneither_size(ap,sizeof(void*))!= 2)(int)_throw((void*)&
Cyc_SocketError_val);{struct _tuple0 _tmpDE;struct _tuple0 _tmp6C=(_tmpDE.f1=*((void**)
_check_dyneither_subscript(ap,sizeof(void*),0)),((_tmpDE.f2=*((void**)
_check_dyneither_subscript(ap,sizeof(void*),1)),_tmpDE)));void*_tmp6D;int*_tmp6F;
void*_tmp70;int*_tmp72;void*_tmp73;int*_tmp75;void*_tmp76;int*_tmp78;void*_tmp79;
struct Cyc_timeval*_tmp7B;void*_tmp7C;int*_tmp7E;void*_tmp7F;struct Cyc_timeval*
_tmp81;void*_tmp82;int*_tmp84;_LL28: _tmp6D=_tmp6C.f1;{struct Cyc_SO_int_struct*
_tmp6E=(struct Cyc_SO_int_struct*)_tmp6D;if(_tmp6E->tag != 0)goto _LL2A;else{_tmp6F=
_tmp6E->f1;}};_tmp70=_tmp6C.f2;{struct Cyc_SO_socklenptr_struct*_tmp71=(struct Cyc_SO_socklenptr_struct*)
_tmp70;if(_tmp71->tag != 2)goto _LL2A;else{_tmp72=_tmp71->f1;}};_LL29: return
getsockopt_int(fd,level,optname,_tmp6F,_tmp72);_LL2A: _tmp73=_tmp6C.f1;{struct Cyc_SO_int_struct*
_tmp74=(struct Cyc_SO_int_struct*)_tmp73;if(_tmp74->tag != 0)goto _LL2C;else{_tmp75=
_tmp74->f1;}};_tmp76=_tmp6C.f2;{struct Cyc_SO_int_struct*_tmp77=(struct Cyc_SO_int_struct*)
_tmp76;if(_tmp77->tag != 0)goto _LL2C;else{_tmp78=_tmp77->f1;}};_LL2B: return
getsockopt_int(fd,level,optname,_tmp75,(int*)_tmp78);_LL2C: _tmp79=_tmp6C.f1;{
struct Cyc_SO_timeval_struct*_tmp7A=(struct Cyc_SO_timeval_struct*)_tmp79;if(
_tmp7A->tag != 1)goto _LL2E;else{_tmp7B=_tmp7A->f1;}};_tmp7C=_tmp6C.f2;{struct Cyc_SO_socklenptr_struct*
_tmp7D=(struct Cyc_SO_socklenptr_struct*)_tmp7C;if(_tmp7D->tag != 2)goto _LL2E;
else{_tmp7E=_tmp7D->f1;}};_LL2D: return getsockopt_timeval(fd,level,optname,_tmp7B,
_tmp7E);_LL2E: _tmp7F=_tmp6C.f1;{struct Cyc_SO_timeval_struct*_tmp80=(struct Cyc_SO_timeval_struct*)
_tmp7F;if(_tmp80->tag != 1)goto _LL30;else{_tmp81=_tmp80->f1;}};_tmp82=_tmp6C.f2;{
struct Cyc_SO_int_struct*_tmp83=(struct Cyc_SO_int_struct*)_tmp82;if(_tmp83->tag != 
0)goto _LL30;else{_tmp84=_tmp83->f1;}};_LL2F: return getsockopt_timeval(fd,level,
optname,_tmp81,(int*)_tmp84);_LL30:;_LL31:(int)_throw((void*)& Cyc_SocketError_val);
_LL27:;};}int recv_wrapped(int,struct _dyneither_ptr,unsigned long,int);int Cyc_recv(
int fd,struct _dyneither_ptr buf,unsigned long n,int flags);int Cyc_recv(int fd,struct
_dyneither_ptr buf,unsigned long n,int flags){return recv_wrapped(fd,buf,n,flags);}
int recvfrom_in(int,struct _dyneither_ptr,unsigned long,int,const struct Cyc_sockaddr_in*,
int*);int Cyc_recvfrom(int fd,struct _dyneither_ptr buf,unsigned long n,int flags,
struct _dyneither_ptr ap);int Cyc_recvfrom(int fd,struct _dyneither_ptr buf,
unsigned long n,int flags,struct _dyneither_ptr ap){if(_get_dyneither_size(ap,
sizeof(void*))!= 2)(int)_throw((void*)& Cyc_SocketError_val);{struct _tuple0 _tmpDF;
struct _tuple0 _tmp86=(_tmpDF.f1=*((void**)_check_dyneither_subscript(ap,sizeof(
void*),0)),((_tmpDF.f2=*((void**)_check_dyneither_subscript(ap,sizeof(void*),1)),
_tmpDF)));void*_tmp87;struct Cyc_sockaddr_in*_tmp89;void*_tmp8A;int*_tmp8C;_LL33:
_tmp87=_tmp86.f1;{struct Cyc_SA_sockaddr_in_struct*_tmp88=(struct Cyc_SA_sockaddr_in_struct*)
_tmp87;if(_tmp88->tag != 0)goto _LL35;else{_tmp89=_tmp88->f1;}};_tmp8A=_tmp86.f2;{
struct Cyc_SA_socklenptr_struct*_tmp8B=(struct Cyc_SA_socklenptr_struct*)_tmp8A;
if(_tmp8B->tag != 1)goto _LL35;else{_tmp8C=_tmp8B->f1;}};_LL34: return recvfrom_in(
fd,buf,n,flags,(const struct Cyc_sockaddr_in*)_tmp89,_tmp8C);_LL35:;_LL36:(int)
_throw((void*)& Cyc_SocketError_val);_LL32:;};}int send_wrapped(int,struct
_dyneither_ptr,unsigned long,int);int Cyc_send(int fd,struct _dyneither_ptr buf,
unsigned long n,int flags);int Cyc_send(int fd,struct _dyneither_ptr buf,unsigned long
n,int flags){return send_wrapped(fd,buf,n,flags);}int sendto_in(int,struct
_dyneither_ptr,unsigned long,int,const struct Cyc_sockaddr_in*,int);int Cyc_sendto(
int fd,struct _dyneither_ptr buf,unsigned long n,int flags,struct _dyneither_ptr ap);
int Cyc_sendto(int fd,struct _dyneither_ptr buf,unsigned long n,int flags,struct
_dyneither_ptr ap){if(_get_dyneither_size(ap,sizeof(void*))!= 2)(int)_throw((void*)&
Cyc_SocketError_val);{struct _tuple0 _tmpE0;struct _tuple0 _tmp8E=(_tmpE0.f1=*((void**)
_check_dyneither_subscript(ap,sizeof(void*),0)),((_tmpE0.f2=*((void**)
_check_dyneither_subscript(ap,sizeof(void*),1)),_tmpE0)));void*_tmp8F;struct Cyc_sockaddr_in*
_tmp91;void*_tmp92;int _tmp94;_LL38: _tmp8F=_tmp8E.f1;{struct Cyc_SA_sockaddr_in_struct*
_tmp90=(struct Cyc_SA_sockaddr_in_struct*)_tmp8F;if(_tmp90->tag != 0)goto _LL3A;
else{_tmp91=_tmp90->f1;}};_tmp92=_tmp8E.f2;{struct Cyc_SA_socklen_struct*_tmp93=(
struct Cyc_SA_socklen_struct*)_tmp92;if(_tmp93->tag != 2)goto _LL3A;else{_tmp94=
_tmp93->f1;}};_LL39: return sendto_in(fd,buf,n,flags,(const struct Cyc_sockaddr_in*)
_tmp91,_tmp94);_LL3A:;_LL3B:(int)_throw((void*)& Cyc_SocketError_val);_LL37:;};}
int setsockopt_int(int,int,int,const int*,int);int setsockopt_timeval(int,int,int,
const struct Cyc_timeval*,int);int Cyc_setsockopt(int fd,int level,int optname,struct
_dyneither_ptr ap);int Cyc_setsockopt(int fd,int level,int optname,struct
_dyneither_ptr ap){if(_get_dyneither_size(ap,sizeof(void*))!= 2)(int)_throw((void*)&
Cyc_SocketError_val);{struct _tuple0 _tmpE1;struct _tuple0 _tmp96=(_tmpE1.f1=*((void**)
_check_dyneither_subscript(ap,sizeof(void*),0)),((_tmpE1.f2=*((void**)
_check_dyneither_subscript(ap,sizeof(void*),1)),_tmpE1)));void*_tmp97;int*_tmp99;
void*_tmp9A;int _tmp9C;void*_tmp9D;struct Cyc_timeval*_tmp9F;void*_tmpA0;int _tmpA2;
_LL3D: _tmp97=_tmp96.f1;{struct Cyc_SO_int_struct*_tmp98=(struct Cyc_SO_int_struct*)
_tmp97;if(_tmp98->tag != 0)goto _LL3F;else{_tmp99=_tmp98->f1;}};_tmp9A=_tmp96.f2;{
struct Cyc_SO_socklen_struct*_tmp9B=(struct Cyc_SO_socklen_struct*)_tmp9A;if(
_tmp9B->tag != 3)goto _LL3F;else{_tmp9C=_tmp9B->f1;}};_LL3E: return setsockopt_int(
fd,level,optname,(const int*)_tmp99,_tmp9C);_LL3F: _tmp9D=_tmp96.f1;{struct Cyc_SO_timeval_struct*
_tmp9E=(struct Cyc_SO_timeval_struct*)_tmp9D;if(_tmp9E->tag != 1)goto _LL41;else{
_tmp9F=_tmp9E->f1;}};_tmpA0=_tmp96.f2;{struct Cyc_SO_socklen_struct*_tmpA1=(
struct Cyc_SO_socklen_struct*)_tmpA0;if(_tmpA1->tag != 3)goto _LL41;else{_tmpA2=
_tmpA1->f1;}};_LL40: return setsockopt_timeval(fd,level,optname,(const struct Cyc_timeval*)
_tmp9F,_tmpA2);_LL41:;_LL42:(int)_throw((void*)& Cyc_SocketError_val);_LL3C:;};}
struct Cyc_stat{int st_dev;unsigned int st_ino;unsigned short st_mode;unsigned short
st_nlink;unsigned int st_uid;unsigned int st_gid;int st_rdev;struct Cyc_timespec
st_atimespec;struct Cyc_timespec st_mtimespec;struct Cyc_timespec st_ctimespec;
long long st_size;long long st_blocks;unsigned int st_blksize;unsigned int st_flags;
unsigned int st_gen;int st_lspare;long long st_qspare[2];};struct Cyc_timezone{int
tz_minuteswest;int tz_dsttime;};struct Cyc_itimerval{struct Cyc_timeval it_interval;
struct Cyc_timeval it_value;};int Cyc_gettimeofday(struct Cyc_timeval*,struct Cyc_timezone*);
int Cyc_select(int,struct Cyc_fd_set*,struct Cyc_fd_set*,struct Cyc_fd_set*,struct
Cyc_timeval*);void Cyc_FD_CLR(int,struct Cyc_fd_set*);int Cyc_FD_ISSET(int,struct
Cyc_fd_set*);void Cyc_FD_SET(int,struct Cyc_fd_set*);void Cyc_FD_ZERO(struct Cyc_fd_set*);
int gettimeofday(struct Cyc_timeval*,struct Cyc_timezone*);int Cyc_gettimeofday(
struct Cyc_timeval*a,struct Cyc_timezone*b);int Cyc_gettimeofday(struct Cyc_timeval*
a,struct Cyc_timezone*b){return gettimeofday(a,0);}struct Cyc_timeb{long time;
unsigned short millitm;short timezone;short dstflag;};struct Cyc_tms{unsigned long
tms_utime;unsigned long tms_stime;unsigned long tms_cutime;unsigned long tms_cstime;
};struct Cyc_sockaddr_un{unsigned char sun_len;unsigned char sun_family;char
sun_path[104];};struct Cyc_utsname{char sysname[256];char nodename[256];char release[
256];char version[256];char machine[256];};struct Cyc_tm{int tm_sec;int tm_min;int
tm_hour;int tm_mday;int tm_mon;int tm_year;int tm_wday;int tm_yday;int tm_isdst;long
tm_gmtoff;char*tm_zone;};unsigned long Cyc_strftime(struct _dyneither_ptr,
unsigned long,struct _dyneither_ptr,const struct Cyc_tm*);unsigned long strftime(
char*,unsigned long,const char*,const struct Cyc_tm*);static char _tmpA4[27]="fgets: buffer insufficient";
static struct Cyc_Core_Failure_struct Cyc___strftime_failure={Cyc_Core_Failure,{
_tmpA4,_tmpA4,_tmpA4 + 27}};unsigned long Cyc_strftime(struct _dyneither_ptr s,
unsigned long maxsize,struct _dyneither_ptr fmt,const struct Cyc_tm*t);unsigned long
Cyc_strftime(struct _dyneither_ptr s,unsigned long maxsize,struct _dyneither_ptr fmt,
const struct Cyc_tm*t){if(maxsize > _get_dyneither_size(s,sizeof(char)))(int)_throw((
void*)& Cyc___strftime_failure);return strftime((char*)_check_null(
_untag_dyneither_ptr(s,sizeof(char),1)),maxsize,(const char*)_check_null(
_untag_dyneither_ptr(fmt,sizeof(char),1)),t);}struct Cyc_option{struct
_dyneither_ptr name;int has_arg;int*flag;int val;};int Cyc_execlp(const char*,const
char*,struct _dyneither_ptr);int execvp(const char*file,const char**argv);struct
_dyneither_ptr Cyc_getcwd(struct _dyneither_ptr buf,unsigned long size);int Cyc_gethostname(
struct _dyneither_ptr,unsigned long);int Cyc_read(int,struct _dyneither_ptr,
unsigned long);int Cyc_write(int,struct _dyneither_ptr,unsigned long);static char
_tmpA6[36]="execlp arg list not NULL-terminated";static struct Cyc_Core_Failure_struct
Cyc___execlp_failure={Cyc_Core_Failure,{_tmpA6,_tmpA6,_tmpA6 + 36}};int Cyc_execlp(
const char*path,const char*arg0,struct _dyneither_ptr argv);static void _tmpE7(
unsigned int*_tmpE6,unsigned int*_tmpE5,const char***_tmpE3){for(*_tmpE6=0;*
_tmpE6 < *_tmpE5;(*_tmpE6)++){(*_tmpE3)[*_tmpE6]=(const char*)0;}}int Cyc_execlp(
const char*path,const char*arg0,struct _dyneither_ptr argv){if(*((const char**)
_check_dyneither_subscript(argv,sizeof(const char*),(int)(_get_dyneither_size(
argv,sizeof(const char*))- 1)))!= (const char*)0)(int)_throw((void*)& Cyc___execlp_failure);{
unsigned int _tmpE6;unsigned int _tmpE5;struct _dyneither_ptr _tmpE4;const char**
_tmpE3;unsigned int _tmpE2;const char**newargs=(const char**)_untag_dyneither_ptr(((
_tmpE2=_get_dyneither_size(argv,sizeof(const char*)),((_tmpE3=(const char**)
_cycalloc(_check_times(sizeof(const char*),_tmpE2 + 1)),((_tmpE4=_tag_dyneither(
_tmpE3,sizeof(const char*),_tmpE2 + 1),((((_tmpE5=_tmpE2,((_tmpE7(& _tmpE6,& _tmpE5,&
_tmpE3),_tmpE3[_tmpE5]=(const char*)0)))),_tmpE4)))))))),sizeof(const char*),1);{
const char*_tmpEC;const char*_tmpEB;struct _dyneither_ptr _tmpEA;const char**_tmpE9;(
_tmpEA=_dyneither_ptr_plus(((_tmpE9=newargs,_tag_dyneither(_tmpE9,sizeof(const
char*),_get_zero_arr_size_voidstar(_tmpE9,1)))),sizeof(const char*),0),((_tmpEB=*((
const char**)_check_dyneither_subscript(_tmpEA,sizeof(const char*),0)),((_tmpEC=(
const char*)arg0,((_get_dyneither_size(_tmpEA,sizeof(const char*))== 1  && (_tmpEB
== 0  && _tmpEC != 0)?_throw_arraybounds(): 1,*((const char**)_tmpEA.curr)=_tmpEC)))))));}{
int i=0;for(0;i < _get_dyneither_size(argv,sizeof(const char*))- 1;++ i){const char*
_tmpF1;const char*_tmpF0;struct _dyneither_ptr _tmpEF;const char**_tmpEE;(_tmpEF=
_dyneither_ptr_plus(((_tmpEE=newargs,_tag_dyneither(_tmpEE,sizeof(const char*),
_get_zero_arr_size_voidstar(_tmpEE,1)))),sizeof(const char*),i + 1),((_tmpF0=*((
const char**)_check_dyneither_subscript(_tmpEF,sizeof(const char*),0)),((_tmpF1=*((
const char**)_check_dyneither_subscript(argv,sizeof(const char*),i)),((
_get_dyneither_size(_tmpEF,sizeof(const char*))== 1  && (_tmpF0 == 0  && _tmpF1 != 0)?
_throw_arraybounds(): 1,*((const char**)_tmpEF.curr)=_tmpF1)))))));}}return execvp(
path,(const char**)newargs);};}char*getcwd(char*buf,unsigned long size);static char
_tmpB4[29]="getcwd: invalid buf argument";static struct Cyc_Core_Failure_struct Cyc___getcwd_failure={
Cyc_Core_Failure,{_tmpB4,_tmpB4,_tmpB4 + 29}};struct _dyneither_ptr Cyc_getcwd(
struct _dyneither_ptr buf,unsigned long size);struct _dyneither_ptr Cyc_getcwd(struct
_dyneither_ptr buf,unsigned long size){if(_get_dyneither_size(buf,sizeof(char))< 
size)(int)_throw((void*)& Cyc___getcwd_failure);{char*_tmpB5=getcwd((char*)
_check_null(_untag_dyneither_ptr(buf,sizeof(char),1)),size);return(unsigned int)
_tmpB5?buf: _tag_dyneither(0,0,0);};}int gethostname(char*,unsigned long);static
char _tmpB7[34]="gethostname: called with NULL buf";static struct Cyc_Core_Failure_struct
Cyc___gethostname_failure_1={Cyc_Core_Failure,{_tmpB7,_tmpB7,_tmpB7 + 34}};static
char _tmpB9[46]="gethostname: called with count > numelts(buf)";static struct Cyc_Core_Failure_struct
Cyc___gethostname_failure_2={Cyc_Core_Failure,{_tmpB9,_tmpB9,_tmpB9 + 46}};int Cyc_gethostname(
struct _dyneither_ptr buf,unsigned long count);int Cyc_gethostname(struct
_dyneither_ptr buf,unsigned long count){if(!((unsigned int)buf.curr))(int)_throw((
void*)& Cyc___gethostname_failure_1);else{if(count > _get_dyneither_size(buf,
sizeof(char)))(int)_throw((void*)& Cyc___gethostname_failure_2);return gethostname((
char*)_untag_dyneither_ptr(buf,sizeof(char),1),count);}}int read(int fd,char*buf,
unsigned long count);static char _tmpBB[39]="read: called with count > numelts(buf)";
static struct Cyc_Core_Failure_struct Cyc___read_failure={Cyc_Core_Failure,{_tmpBB,
_tmpBB,_tmpBB + 39}};int Cyc_read(int fd,struct _dyneither_ptr buf,unsigned long count);
int Cyc_read(int fd,struct _dyneither_ptr buf,unsigned long count){if(count > 
_get_dyneither_size(buf,sizeof(char)))(int)_throw((void*)& Cyc___read_failure);
return read(fd,(char*)_check_null(_untag_dyneither_ptr(buf,sizeof(char),1)),count);}
int write(int fd,const char*buf,unsigned long count);static char _tmpBD[40]="write: called with count > numelts(buf)";
static struct Cyc_Core_Failure_struct Cyc___write_failure={Cyc_Core_Failure,{_tmpBD,
_tmpBD,_tmpBD + 40}};int Cyc_write(int fd,struct _dyneither_ptr buf,unsigned long
count);int Cyc_write(int fd,struct _dyneither_ptr buf,unsigned long count){if(count > 
_get_dyneither_size(buf,sizeof(char)))(int)_throw((void*)& Cyc___write_failure);
return write(fd,(const char*)_untag_dyneither_ptr(buf,sizeof(char),1),count);}
struct Cyc_utimbuf{long actime;long modtime;};
