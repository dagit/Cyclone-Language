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
 void exit(int);struct Cyc_Core_NewRegion{struct _DynRegionHandle*dynregion;};struct
Cyc_Core_Opt{void*v;};extern char Cyc_Core_Invalid_argument[17];struct Cyc_Core_Invalid_argument_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8];struct Cyc_Core_Failure_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11];struct Cyc_Core_Impossible_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10];struct Cyc_Core_Not_found_struct{
char*tag;};extern char Cyc_Core_Unreachable[12];struct Cyc_Core_Unreachable_struct{
char*tag;struct _dyneither_ptr f1;};extern struct _RegionHandle*Cyc_Core_heap_region;
extern char Cyc_Core_Open_Region[12];struct Cyc_Core_Open_Region_struct{char*tag;};
extern char Cyc_Core_Free_Region[12];struct Cyc_Core_Free_Region_struct{char*tag;};
struct _dyneither_ptr Cstring_to_string(char*);struct Cyc_List_List{void*hd;struct
Cyc_List_List*tl;};extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_struct{
char*tag;};extern char Cyc_List_Nth[4];struct Cyc_List_Nth_struct{char*tag;};struct
Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(struct
Cyc_Iter_Iter,void*);struct Cyc_Set_Set;struct Cyc_Set_Set*Cyc_Set_empty(int(*cmp)(
void*,void*));struct Cyc_Set_Set*Cyc_Set_insert(struct Cyc_Set_Set*s,void*elt);int
Cyc_Set_member(struct Cyc_Set_Set*s,void*elt);extern char Cyc_Set_Absent[7];struct
Cyc_Set_Absent_struct{char*tag;};struct Cyc_Typerep_Int_struct{int tag;int f1;
unsigned int f2;};struct Cyc_Typerep_Float_struct{int tag;};struct Cyc_Typerep_Double_struct{
int tag;};struct Cyc_Typerep_ThinPtr_struct{int tag;unsigned int f1;void*f2;};struct
Cyc_Typerep_FatPtr_struct{int tag;void*f1;};struct _tuple0{unsigned int f1;struct
_dyneither_ptr f2;void*f3;};struct Cyc_Typerep_Struct_struct{int tag;struct
_dyneither_ptr*f1;unsigned int f2;struct _dyneither_ptr f3;};struct _tuple1{
unsigned int f1;void*f2;};struct Cyc_Typerep_Tuple_struct{int tag;unsigned int f1;
struct _dyneither_ptr f2;};struct _tuple2{unsigned int f1;struct _dyneither_ptr f2;};
struct Cyc_Typerep_TUnion_struct{int tag;struct _dyneither_ptr f1;struct
_dyneither_ptr f2;struct _dyneither_ptr f3;};struct Cyc_Typerep_TUnionField_struct{
int tag;struct _dyneither_ptr f1;struct _dyneither_ptr f2;unsigned int f3;struct
_dyneither_ptr f4;};struct _tuple3{struct _dyneither_ptr f1;void*f2;};struct Cyc_Typerep_XTUnion_struct{
int tag;struct _dyneither_ptr f1;struct _dyneither_ptr f2;};struct Cyc_Typerep_Union_struct{
int tag;struct _dyneither_ptr*f1;int f2;struct _dyneither_ptr f3;};struct Cyc_Typerep_Enum_struct{
int tag;struct _dyneither_ptr*f1;int f2;struct _dyneither_ptr f3;};struct _tuple4{int
f1;void*f2;};struct _tuple4 Cyc_Typerep_get_unionbranch(unsigned int tag,struct
_dyneither_ptr l);struct _dyneither_ptr Cyc_Typerep_get_tagname(unsigned int tag,
struct _dyneither_ptr l);struct _dyneither_ptr Cyc_Typerep_get_tagname2(unsigned int
tag,struct _dyneither_ptr l);struct _tuple4 Cyc_Typerep_get_xtunionbranch(struct
_dyneither_ptr tag,struct _dyneither_ptr l);unsigned int Cyc_Typerep_size_type(void*
rep);struct Cyc___cycFILE;extern struct Cyc___cycFILE*Cyc_stdout;extern struct Cyc___cycFILE*
Cyc_stderr;struct Cyc_Cstdio___abstractFILE;struct Cyc_String_pa_struct{int tag;
struct _dyneither_ptr f1;};struct Cyc_Int_pa_struct{int tag;unsigned long f1;};struct
Cyc_Double_pa_struct{int tag;double f1;};struct Cyc_LongDouble_pa_struct{int tag;
long double f1;};struct Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{
int tag;unsigned long*f1;};int Cyc_fgetc(struct Cyc___cycFILE*);int Cyc_fprintf(
struct Cyc___cycFILE*,struct _dyneither_ptr,struct _dyneither_ptr);int Cyc_fputc(int,
struct Cyc___cycFILE*);unsigned long Cyc_fread(struct _dyneither_ptr,unsigned long,
unsigned long,struct Cyc___cycFILE*);struct Cyc_ShortPtr_sa_struct{int tag;short*f1;
};struct Cyc_UShortPtr_sa_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_struct{
int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_struct{
int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_struct{int tag;double*f1;}
;struct Cyc_FloatPtr_sa_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int
tag;struct _dyneither_ptr f1;};extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_struct{
char*tag;};extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_struct{char*
tag;struct _dyneither_ptr f1;};int Cyc_strcmp(struct _dyneither_ptr s1,struct
_dyneither_ptr s2);int isprint(int);struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(
void*,void*);struct _RegionHandle*r;struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[
8];struct Cyc_Dict_Present_struct{char*tag;};extern char Cyc_Dict_Absent[7];struct
Cyc_Dict_Absent_struct{char*tag;};struct Cyc_Dict_Dict Cyc_Dict_empty(int(*cmp)(
void*,void*));int Cyc_Dict_member(struct Cyc_Dict_Dict d,void*k);struct Cyc_Dict_Dict
Cyc_Dict_insert(struct Cyc_Dict_Dict d,void*k,void*v);void*Cyc_Dict_lookup(struct
Cyc_Dict_Dict d,void*k);void**Cyc_Dict_lookup_opt(struct Cyc_Dict_Dict d,void*k);
struct _tuple5{void*f1;void*f2;};struct _tuple5*Cyc_Dict_rchoose(struct
_RegionHandle*r,struct Cyc_Dict_Dict d);struct _tuple5*Cyc_Dict_rchoose(struct
_RegionHandle*,struct Cyc_Dict_Dict d);void Cyc_Marshal_fprint_type(void*rep,struct
Cyc___cycFILE*fp,void*val);void Cyc_Marshal_print_type(void*rep,void*val);void Cyc_Marshal_write_type(
void*rep,struct Cyc___cycFILE*fp,void*val);struct _tuple6{struct Cyc_Dict_Dict f1;
int f2;};struct _tuple7{struct _dyneither_ptr f1;int f2;};struct _tuple6 Cyc_Marshal_empty_addr_index();
struct _tuple6 Cyc_Marshal_write_type_base(void*rep,struct _tuple6 env,struct Cyc___cycFILE*
fp,void*val);void*Cyc_Marshal_rread_type(struct _RegionHandle*r,void*rep,struct
Cyc___cycFILE*fp);void*Cyc_Marshal_read_type(void*rep,struct Cyc___cycFILE*fp);
void*Cyc_Marshal_rcopy_type(struct _RegionHandle*,void*rep,void*val);void*Cyc_Marshal_copy_type(
void*rep,void*val);int Cyc_Marshal_leq_type(void*rep,void*x,void*y);int Cyc_Marshal_eq_type(
void*rep,void*x,void*y);int Cyc_Marshal_cmp_type(void*rep,void*x,void*y);void*
unsafe_cast(void*x);static int Cyc_Marshal_uint_cmp(unsigned int a,unsigned int b);
static int Cyc_Marshal_uint_cmp(unsigned int a,unsigned int b){if(a == b)return 0;
else{if(a < b)return 1;else{return - 1;}}}static struct Cyc_Set_Set*Cyc_Marshal_empty_addr_set();
static struct Cyc_Set_Set*Cyc_Marshal_empty_addr_set(){return((struct Cyc_Set_Set*(*)(
struct Cyc_Set_Set*s,unsigned int elt))Cyc_Set_insert)(((struct Cyc_Set_Set*(*)(int(*
cmp)(unsigned int,unsigned int)))Cyc_Set_empty)(Cyc_Marshal_uint_cmp),0);}struct
_tuple8{int f1;struct Cyc_Set_Set*f2;};static struct _tuple8*Cyc_Marshal_member_insert(
struct Cyc_Set_Set*set,unsigned int val);static struct _tuple8*Cyc_Marshal_member_insert(
struct Cyc_Set_Set*set,unsigned int val){struct _tuple8*_tmp2EE;return(_tmp2EE=
_cycalloc(sizeof(*_tmp2EE)),((_tmp2EE->f1=((int(*)(struct Cyc_Set_Set*s,
unsigned int elt))Cyc_Set_member)(set,val),((_tmp2EE->f2=((struct Cyc_Set_Set*(*)(
struct Cyc_Set_Set*s,unsigned int elt))Cyc_Set_insert)(set,val),_tmp2EE)))));}
static int Cyc_Marshal_index_member(struct _tuple6 idx,unsigned int m);inline static
int Cyc_Marshal_index_member(struct _tuple6 idx,unsigned int m){int _tmp2;struct
_tuple6 _tmp1=idx;_tmp2=_tmp1.f2;return m < _tmp2;}struct _tuple6 Cyc_Marshal_empty_addr_index();
struct _tuple6 Cyc_Marshal_empty_addr_index(){struct _tuple6 _tmp2EF;return(_tmp2EF.f1=((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,unsigned int k,int v))Cyc_Dict_insert)(((
struct Cyc_Dict_Dict(*)(int(*cmp)(unsigned int,unsigned int)))Cyc_Dict_empty)(Cyc_Marshal_uint_cmp),
0,0),((_tmp2EF.f2=1,_tmp2EF)));}static unsigned int Cyc_Marshal_index_lookup(
struct _tuple6 index,unsigned int val);static unsigned int Cyc_Marshal_index_lookup(
struct _tuple6 index,unsigned int val){struct Cyc_Dict_Dict _tmp5;int _tmp6;struct
_tuple6 _tmp4=index;_tmp5=_tmp4.f1;_tmp6=_tmp4.f2;{int*_tmp7=((int*(*)(struct Cyc_Dict_Dict
d,unsigned int k))Cyc_Dict_lookup_opt)(_tmp5,val);if(_tmp7 != 0)return(
unsigned int)*_tmp7;else{return(unsigned int)_tmp6;}}}static struct _tuple6 Cyc_Marshal_index_insert(
struct _tuple6 index,unsigned int val);static struct _tuple6 Cyc_Marshal_index_insert(
struct _tuple6 index,unsigned int val){struct Cyc_Dict_Dict _tmp9;int _tmpA;struct
_tuple6 _tmp8=index;_tmp9=_tmp8.f1;_tmpA=_tmp8.f2;{struct _tuple6 _tmp2F0;return(
_tmp2F0.f1=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,unsigned int k,int v))Cyc_Dict_insert)(
_tmp9,val,_tmpA),((_tmp2F0.f2=_tmpA + 1,_tmp2F0)));}}static struct _tuple7 Cyc_Marshal_empty_addr_table();
static void _tmp2F5(unsigned int*_tmp2F4,unsigned int*_tmp2F3,unsigned int**
_tmp2F2){for(*_tmp2F4=0;*_tmp2F4 < *_tmp2F3;(*_tmp2F4)++){(*_tmp2F2)[*_tmp2F4]=0;}}
static struct _tuple7 Cyc_Marshal_empty_addr_table(){unsigned int _tmp2FA;
unsigned int*_tmp2F9;unsigned int _tmp2F8;unsigned int _tmp2F7;struct _tuple7
_tmp2F6;return(_tmp2F6.f1=_tag_dyneither(((_tmp2FA=(unsigned int)1,((_tmp2F9=(
unsigned int*)_cycalloc_atomic(_check_times(sizeof(unsigned int),_tmp2FA)),((((
_tmp2F8=_tmp2FA,_tmp2F5(& _tmp2F7,& _tmp2F8,& _tmp2F9))),_tmp2F9)))))),sizeof(
unsigned int),(unsigned int)1),((_tmp2F6.f2=1,_tmp2F6)));}static int Cyc_Marshal_table_member(
struct _tuple7 table,unsigned int m);inline static int Cyc_Marshal_table_member(struct
_tuple7 table,unsigned int m){int _tmp11;struct _tuple7 _tmp10=table;_tmp11=_tmp10.f2;
return m < _tmp11;}static struct _tuple7 Cyc_Marshal_table_insert(struct _tuple7 tbl,
unsigned int i);static void _tmp300(unsigned int*_tmp2FF,unsigned int*_tmp2FE,
unsigned int**_tmp2FC,int*_tmp14,struct _dyneither_ptr*_tmp13){for(*_tmp2FF=0;*
_tmp2FF < *_tmp2FE;(*_tmp2FF)++){(*_tmp2FC)[*_tmp2FF]=*_tmp2FF < *_tmp14?*((
unsigned int*)_check_dyneither_subscript(*_tmp13,sizeof(unsigned int),(int)*
_tmp2FF)): 0;}}static struct _tuple7 Cyc_Marshal_table_insert(struct _tuple7 tbl,
unsigned int i){struct _dyneither_ptr _tmp13;int _tmp14;struct _tuple7 _tmp12=tbl;
_tmp13=_tmp12.f1;_tmp14=_tmp12.f2;if(_get_dyneither_size(_tmp13,sizeof(
unsigned int))== _tmp14){unsigned int _tmp2FF;unsigned int _tmp2FE;struct
_dyneither_ptr _tmp2FD;unsigned int*_tmp2FC;unsigned int _tmp2FB;_tmp13=((_tmp2FB=(
unsigned int)(2 * _tmp14),((_tmp2FC=(unsigned int*)_cycalloc_atomic(_check_times(
sizeof(unsigned int),_tmp2FB)),((_tmp2FD=_tag_dyneither(_tmp2FC,sizeof(
unsigned int),_tmp2FB),((((_tmp2FE=_tmp2FB,_tmp300(& _tmp2FF,& _tmp2FE,& _tmp2FC,&
_tmp14,& _tmp13))),_tmp2FD))))))));}*((unsigned int*)_check_dyneither_subscript(
_tmp13,sizeof(unsigned int),_tmp14))=i;{struct _tuple7 _tmp301;return(_tmp301.f1=
_tmp13,((_tmp301.f2=_tmp14 + 1,_tmp301)));}}static unsigned int Cyc_Marshal_table_lookup(
struct _tuple7 tbl,int i);static unsigned int Cyc_Marshal_table_lookup(struct _tuple7
tbl,int i){return*((unsigned int*)_check_dyneither_subscript(tbl.f1,sizeof(
unsigned int),i));}static void Cyc_Marshal_fprint_ref(struct Cyc___cycFILE*fp,
unsigned int ref);static void Cyc_Marshal_fprint_ref(struct Cyc___cycFILE*fp,
unsigned int ref){if(ref == 0){const char*_tmp304;void*_tmp303;(_tmp303=0,Cyc_fprintf(
fp,((_tmp304="NULL",_tag_dyneither(_tmp304,sizeof(char),5))),_tag_dyneither(
_tmp303,sizeof(void*),0)));}else{const char*_tmp308;void*_tmp307[1];struct Cyc_Int_pa_struct
_tmp306;(_tmp306.tag=1,((_tmp306.f1=ref,((_tmp307[0]=& _tmp306,Cyc_fprintf(fp,((
_tmp308="(#%x)",_tag_dyneither(_tmp308,sizeof(char),6))),_tag_dyneither(_tmp307,
sizeof(void*),1)))))));}}struct _tuple9{unsigned int f1;unsigned int f2;
unsigned int f3;};struct Cyc_Set_Set*Cyc_Marshal_fprint_type_base(void*rep,struct
Cyc_Set_Set*env,struct Cyc___cycFILE*fp,void*val);struct Cyc_Set_Set*Cyc_Marshal_fprint_type_base(
void*rep,struct Cyc_Set_Set*env,struct Cyc___cycFILE*fp,void*val){{void*_tmp1F=rep;
int _tmp21;unsigned int _tmp22;unsigned int _tmp26;void*_tmp27;void*_tmp29;struct
_dyneither_ptr*_tmp2B;struct _dyneither_ptr _tmp2C;struct _dyneither_ptr _tmp2E;
struct _dyneither_ptr _tmp30;struct _dyneither_ptr _tmp31;struct _dyneither_ptr _tmp32;
struct _dyneither_ptr _tmp34;struct _dyneither_ptr _tmp35;struct _dyneither_ptr _tmp36;
struct _dyneither_ptr _tmp38;struct _dyneither_ptr _tmp39;struct _dyneither_ptr*
_tmp3B;struct _dyneither_ptr _tmp3C;int _tmp3E;struct _dyneither_ptr _tmp3F;_LL1: {
struct Cyc_Typerep_Int_struct*_tmp20=(struct Cyc_Typerep_Int_struct*)_tmp1F;if(
_tmp20->tag != 0)goto _LL3;else{_tmp21=_tmp20->f1;_tmp22=_tmp20->f2;}}_LL2: switch(
_tmp22){case 8: _LL19: {char*x=((char*(*)(void*x))unsafe_cast)(val);if(isprint((
int)*x)){const char*_tmp30C;void*_tmp30B[1];struct Cyc_Int_pa_struct _tmp30A;(
_tmp30A.tag=1,((_tmp30A.f1=(unsigned long)((int)*x),((_tmp30B[0]=& _tmp30A,Cyc_fprintf(
fp,((_tmp30C="'%c'",_tag_dyneither(_tmp30C,sizeof(char),5))),_tag_dyneither(
_tmp30B,sizeof(void*),1)))))));}else{const char*_tmp310;void*_tmp30F[1];struct Cyc_Int_pa_struct
_tmp30E;(_tmp30E.tag=1,((_tmp30E.f1=(unsigned long)((int)*x),((_tmp30F[0]=&
_tmp30E,Cyc_fprintf(fp,((_tmp310="'\\%d'",_tag_dyneither(_tmp310,sizeof(char),6))),
_tag_dyneither(_tmp30F,sizeof(void*),1)))))));}break;}case 16: _LL1A: {short*x=((
short*(*)(void*x))unsafe_cast)(val);{const char*_tmp316;const char*_tmp315;void*
_tmp314[1];struct Cyc_Int_pa_struct _tmp313;(_tmp313.tag=1,((_tmp313.f1=(
unsigned long)((int)*x),((_tmp314[0]=& _tmp313,Cyc_fprintf(fp,_tmp21?(_tmp315="%d",
_tag_dyneither(_tmp315,sizeof(char),3)):((_tmp316="%u",_tag_dyneither(_tmp316,
sizeof(char),3))),_tag_dyneither(_tmp314,sizeof(void*),1)))))));}break;}case 32:
_LL1B: {int*x=((int*(*)(void*x))unsafe_cast)(val);{const char*_tmp31C;const char*
_tmp31B;void*_tmp31A[1];struct Cyc_Int_pa_struct _tmp319;(_tmp319.tag=1,((_tmp319.f1=(
unsigned long)*x,((_tmp31A[0]=& _tmp319,Cyc_fprintf(fp,_tmp21?(_tmp31B="%d",
_tag_dyneither(_tmp31B,sizeof(char),3)):((_tmp31C="%u",_tag_dyneither(_tmp31C,
sizeof(char),3))),_tag_dyneither(_tmp31A,sizeof(void*),1)))))));}break;}default:
_LL1C:{const char*_tmp320;void*_tmp31F[1];struct Cyc_Int_pa_struct _tmp31E;(_tmp31E.tag=
1,((_tmp31E.f1=(unsigned long)((int)_tmp22),((_tmp31F[0]=& _tmp31E,Cyc_fprintf(
Cyc_stderr,((_tmp320="fprint_type_base: illegal int size %d bits\n",
_tag_dyneither(_tmp320,sizeof(char),44))),_tag_dyneither(_tmp31F,sizeof(void*),1)))))));}
exit(1);}goto _LL0;_LL3: {struct Cyc_Typerep_Float_struct*_tmp23=(struct Cyc_Typerep_Float_struct*)
_tmp1F;if(_tmp23->tag != 1)goto _LL5;}_LL4: {float*x=((float*(*)(void*x))
unsafe_cast)(val);{const char*_tmp324;void*_tmp323[1];struct Cyc_Double_pa_struct
_tmp322;(_tmp322.tag=2,((_tmp322.f1=(double)*x,((_tmp323[0]=& _tmp322,Cyc_fprintf(
fp,((_tmp324="%f",_tag_dyneither(_tmp324,sizeof(char),3))),_tag_dyneither(
_tmp323,sizeof(void*),1)))))));}goto _LL0;}_LL5: {struct Cyc_Typerep_Double_struct*
_tmp24=(struct Cyc_Typerep_Double_struct*)_tmp1F;if(_tmp24->tag != 2)goto _LL7;}
_LL6: {double*x=((double*(*)(void*x))unsafe_cast)(val);{const char*_tmp328;void*
_tmp327[1];struct Cyc_Double_pa_struct _tmp326;(_tmp326.tag=2,((_tmp326.f1=*x,((
_tmp327[0]=& _tmp326,Cyc_fprintf(fp,((_tmp328="%f",_tag_dyneither(_tmp328,sizeof(
char),3))),_tag_dyneither(_tmp327,sizeof(void*),1)))))));}goto _LL0;}_LL7: {
struct Cyc_Typerep_ThinPtr_struct*_tmp25=(struct Cyc_Typerep_ThinPtr_struct*)
_tmp1F;if(_tmp25->tag != 3)goto _LL9;else{_tmp26=_tmp25->f1;_tmp27=(void*)_tmp25->f2;}}
_LL8: {int*x=((int*(*)(void*x))unsafe_cast)(val);int y=*((int*)_check_null(x));
struct _tuple8 _tmp58;int _tmp59;struct Cyc_Set_Set*_tmp5A;struct _tuple8*_tmp57=Cyc_Marshal_member_insert(
env,(unsigned int)y);_tmp58=*_tmp57;_tmp59=_tmp58.f1;_tmp5A=_tmp58.f2;env=_tmp5A;
if(!_tmp59){{const char*_tmp32C;void*_tmp32B[1];struct Cyc_Int_pa_struct _tmp32A;(
_tmp32A.tag=1,((_tmp32A.f1=(unsigned int)y,((_tmp32B[0]=& _tmp32A,Cyc_fprintf(fp,((
_tmp32C="{(@%x) ",_tag_dyneither(_tmp32C,sizeof(char),8))),_tag_dyneither(
_tmp32B,sizeof(void*),1)))))));}if(y != 0){int typ_szb=(int)Cyc_Typerep_size_type(
_tmp27);int i=0;for(0;i < _tmp26;++ i){env=Cyc_Marshal_fprint_type_base(_tmp27,env,
fp,((void*(*)(int x))unsafe_cast)(y));if(i != _tmp26 - 1){const char*_tmp32F;void*
_tmp32E;(_tmp32E=0,Cyc_fprintf(fp,((_tmp32F=", ",_tag_dyneither(_tmp32F,sizeof(
char),3))),_tag_dyneither(_tmp32E,sizeof(void*),0)));}y +=typ_szb;}}else{const
char*_tmp332;void*_tmp331;(_tmp331=0,Cyc_fprintf(fp,((_tmp332="NULL",
_tag_dyneither(_tmp332,sizeof(char),5))),_tag_dyneither(_tmp331,sizeof(void*),0)));}{
const char*_tmp335;void*_tmp334;(_tmp334=0,Cyc_fprintf(fp,((_tmp335=" }",
_tag_dyneither(_tmp335,sizeof(char),3))),_tag_dyneither(_tmp334,sizeof(void*),0)));}}
else{Cyc_Marshal_fprint_ref(fp,(unsigned int)y);}goto _LL0;}_LL9: {struct Cyc_Typerep_FatPtr_struct*
_tmp28=(struct Cyc_Typerep_FatPtr_struct*)_tmp1F;if(_tmp28->tag != 4)goto _LLB;
else{_tmp29=(void*)_tmp28->f1;}}_LLA: {struct _tuple9*x=((struct _tuple9*(*)(void*
x))unsafe_cast)(val);unsigned int typ_szb=Cyc_Typerep_size_type(_tmp29);
unsigned int base=(*x).f1;unsigned int curr=(*x).f2;unsigned int last_plus_one=(*x).f3;
struct _tuple8 _tmp65;int _tmp66;struct Cyc_Set_Set*_tmp67;struct _tuple8*_tmp64=Cyc_Marshal_member_insert(
env,base);_tmp65=*_tmp64;_tmp66=_tmp65.f1;_tmp67=_tmp65.f2;env=_tmp67;if(!_tmp66){{
const char*_tmp339;void*_tmp338[1];struct Cyc_Int_pa_struct _tmp337;(_tmp337.tag=1,((
_tmp337.f1=base,((_tmp338[0]=& _tmp337,Cyc_fprintf(fp,((_tmp339="[(@%x) ",
_tag_dyneither(_tmp339,sizeof(char),8))),_tag_dyneither(_tmp338,sizeof(void*),1)))))));}{
unsigned int i=base;for(0;i < last_plus_one;i +=typ_szb){env=Cyc_Marshal_fprint_type_base(
_tmp29,env,fp,((void*(*)(unsigned int x))unsafe_cast)(i));if(i + typ_szb < 
last_plus_one){const char*_tmp33C;void*_tmp33B;(_tmp33B=0,Cyc_fprintf(fp,((
_tmp33C=", ",_tag_dyneither(_tmp33C,sizeof(char),3))),_tag_dyneither(_tmp33B,
sizeof(void*),0)));}}}{const char*_tmp33F;void*_tmp33E;(_tmp33E=0,Cyc_fprintf(fp,((
_tmp33F=" ]",_tag_dyneither(_tmp33F,sizeof(char),3))),_tag_dyneither(_tmp33E,
sizeof(void*),0)));}}else{Cyc_Marshal_fprint_ref(fp,base);}goto _LL0;}_LLB: {
struct Cyc_Typerep_Struct_struct*_tmp2A=(struct Cyc_Typerep_Struct_struct*)_tmp1F;
if(_tmp2A->tag != 5)goto _LLD;else{_tmp2B=_tmp2A->f1;_tmp2C=_tmp2A->f3;}}_LLC: if(
_tmp2B == 0){const char*_tmp342;void*_tmp341;(_tmp341=0,Cyc_fprintf(fp,((_tmp342="{ ",
_tag_dyneither(_tmp342,sizeof(char),3))),_tag_dyneither(_tmp341,sizeof(void*),0)));}
else{const char*_tmp346;void*_tmp345[1];struct Cyc_String_pa_struct _tmp344;(
_tmp344.tag=0,((_tmp344.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp2B),((
_tmp345[0]=& _tmp344,Cyc_fprintf(fp,((_tmp346="%s{ ",_tag_dyneither(_tmp346,
sizeof(char),5))),_tag_dyneither(_tmp345,sizeof(void*),1)))))));}{int x=((int(*)(
void*x))unsafe_cast)(val);{int i=0;for(0;i < _get_dyneither_size(_tmp2C,sizeof(
struct _tuple0*));++ i){unsigned int _tmp75;struct _dyneither_ptr _tmp76;void*_tmp77;
struct _tuple0 _tmp74=*((struct _tuple0**)_tmp2C.curr)[i];_tmp75=_tmp74.f1;_tmp76=
_tmp74.f2;_tmp77=_tmp74.f3;{const char*_tmp34A;void*_tmp349[1];struct Cyc_String_pa_struct
_tmp348;(_tmp348.tag=0,((_tmp348.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
_tmp76),((_tmp349[0]=& _tmp348,Cyc_fprintf(fp,((_tmp34A="%s=",_tag_dyneither(
_tmp34A,sizeof(char),4))),_tag_dyneither(_tmp349,sizeof(void*),1)))))));}{int
x_ofs=(int)(x + _tmp75);env=Cyc_Marshal_fprint_type_base(_tmp77,env,fp,((void*(*)(
int x))unsafe_cast)(x_ofs));if(i != _get_dyneither_size(_tmp2C,sizeof(struct
_tuple0*))- 1){const char*_tmp34D;void*_tmp34C;(_tmp34C=0,Cyc_fprintf(fp,((
_tmp34D=", ",_tag_dyneither(_tmp34D,sizeof(char),3))),_tag_dyneither(_tmp34C,
sizeof(void*),0)));}}}}{const char*_tmp350;void*_tmp34F;(_tmp34F=0,Cyc_fprintf(fp,((
_tmp350=" }",_tag_dyneither(_tmp350,sizeof(char),3))),_tag_dyneither(_tmp34F,
sizeof(void*),0)));}goto _LL0;}_LLD: {struct Cyc_Typerep_Tuple_struct*_tmp2D=(
struct Cyc_Typerep_Tuple_struct*)_tmp1F;if(_tmp2D->tag != 6)goto _LLF;else{_tmp2E=
_tmp2D->f2;}}_LLE:{const char*_tmp353;void*_tmp352;(_tmp352=0,Cyc_fprintf(fp,((
_tmp353="$( ",_tag_dyneither(_tmp353,sizeof(char),4))),_tag_dyneither(_tmp352,
sizeof(void*),0)));}{int x=((int(*)(void*x))unsafe_cast)(val);{int i=0;for(0;i < 
_get_dyneither_size(_tmp2E,sizeof(struct _tuple1*));++ i){unsigned int _tmp82;void*
_tmp83;struct _tuple1 _tmp81=*((struct _tuple1**)_tmp2E.curr)[i];_tmp82=_tmp81.f1;
_tmp83=_tmp81.f2;{int x_ofs=(int)(x + _tmp82);env=Cyc_Marshal_fprint_type_base(
_tmp83,env,fp,((void*(*)(int x))unsafe_cast)(x_ofs));if(i != _get_dyneither_size(
_tmp2E,sizeof(struct _tuple1*))- 1){const char*_tmp356;void*_tmp355;(_tmp355=0,Cyc_fprintf(
fp,((_tmp356=", ",_tag_dyneither(_tmp356,sizeof(char),3))),_tag_dyneither(
_tmp355,sizeof(void*),0)));}}}}{const char*_tmp359;void*_tmp358;(_tmp358=0,Cyc_fprintf(
fp,((_tmp359=" )",_tag_dyneither(_tmp359,sizeof(char),3))),_tag_dyneither(
_tmp358,sizeof(void*),0)));}goto _LL0;}_LLF: {struct Cyc_Typerep_TUnionField_struct*
_tmp2F=(struct Cyc_Typerep_TUnionField_struct*)_tmp1F;if(_tmp2F->tag != 8)goto
_LL11;else{_tmp30=_tmp2F->f1;_tmp31=_tmp2F->f2;_tmp32=_tmp2F->f4;}}_LL10:{const
char*_tmp35E;void*_tmp35D[2];struct Cyc_String_pa_struct _tmp35C;struct Cyc_String_pa_struct
_tmp35B;(_tmp35B.tag=0,((_tmp35B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
_tmp31),((_tmp35C.tag=0,((_tmp35C.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)_tmp30),((_tmp35D[0]=& _tmp35C,((_tmp35D[1]=& _tmp35B,Cyc_fprintf(
fp,((_tmp35E="%s.%s(",_tag_dyneither(_tmp35E,sizeof(char),7))),_tag_dyneither(
_tmp35D,sizeof(void*),2)))))))))))));}{int x=((int(*)(void*x))unsafe_cast)(val);{
int i=1;for(0;i < _get_dyneither_size(_tmp32,sizeof(struct _tuple1*));++ i){
unsigned int _tmp8D;void*_tmp8E;struct _tuple1 _tmp8C=*((struct _tuple1**)_tmp32.curr)[
i];_tmp8D=_tmp8C.f1;_tmp8E=_tmp8C.f2;{int x_ofs=(int)(x + _tmp8D);env=Cyc_Marshal_fprint_type_base(
_tmp8E,env,fp,((void*(*)(int x))unsafe_cast)(x_ofs));if(i != _get_dyneither_size(
_tmp32,sizeof(struct _tuple1*))- 1){const char*_tmp361;void*_tmp360;(_tmp360=0,Cyc_fprintf(
fp,((_tmp361=", ",_tag_dyneither(_tmp361,sizeof(char),3))),_tag_dyneither(
_tmp360,sizeof(void*),0)));}}}}{const char*_tmp364;void*_tmp363;(_tmp363=0,Cyc_fprintf(
fp,((_tmp364=" )",_tag_dyneither(_tmp364,sizeof(char),3))),_tag_dyneither(
_tmp363,sizeof(void*),0)));}goto _LL0;}_LL11: {struct Cyc_Typerep_TUnion_struct*
_tmp33=(struct Cyc_Typerep_TUnion_struct*)_tmp1F;if(_tmp33->tag != 7)goto _LL13;
else{_tmp34=_tmp33->f1;_tmp35=_tmp33->f2;_tmp36=_tmp33->f3;}}_LL12: {
unsigned int*tagp=((unsigned int*(*)(void*x))unsafe_cast)(val);if(*tagp < 1024){
const char*_tmp368;void*_tmp367[1];struct Cyc_String_pa_struct _tmp366;(_tmp366.tag=
0,((_tmp366.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Typerep_get_tagname(*
tagp,_tmp35)),((_tmp367[0]=& _tmp366,Cyc_fprintf(fp,((_tmp368="%s",_tag_dyneither(
_tmp368,sizeof(char),3))),_tag_dyneither(_tmp367,sizeof(void*),1)))))));}else{
struct _tuple8 _tmp97;int _tmp98;struct Cyc_Set_Set*_tmp99;struct _tuple8*_tmp96=Cyc_Marshal_member_insert(
env,*tagp);_tmp97=*_tmp96;_tmp98=_tmp97.f1;_tmp99=_tmp97.f2;env=_tmp99;if(!
_tmp98){{const char*_tmp36C;void*_tmp36B[1];struct Cyc_Int_pa_struct _tmp36A;(
_tmp36A.tag=1,((_tmp36A.f1=*tagp,((_tmp36B[0]=& _tmp36A,Cyc_fprintf(fp,((_tmp36C="(@%x)",
_tag_dyneither(_tmp36C,sizeof(char),6))),_tag_dyneither(_tmp36B,sizeof(void*),1)))))));}{
unsigned int*x=((unsigned int*(*)(unsigned int x))unsafe_cast)(*tagp);
unsigned int tag=*x;int _tmp9E;void*_tmp9F;struct _tuple4 _tmp9D=Cyc_Typerep_get_unionbranch(
tag,_tmp36);_tmp9E=_tmp9D.f1;_tmp9F=_tmp9D.f2;{void*xoff=((void*(*)(unsigned int
x))unsafe_cast)(*tagp + _tmp9E);{const char*_tmp370;void*_tmp36F[1];struct Cyc_String_pa_struct
_tmp36E;(_tmp36E.tag=0,((_tmp36E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Typerep_get_tagname2(tag,_tmp36)),((_tmp36F[0]=& _tmp36E,Cyc_fprintf(fp,((
_tmp370="&%s[",_tag_dyneither(_tmp370,sizeof(char),5))),_tag_dyneither(_tmp36F,
sizeof(void*),1)))))));}env=Cyc_Marshal_fprint_type_base(_tmp9F,env,fp,xoff);{
const char*_tmp373;void*_tmp372;(_tmp372=0,Cyc_fprintf(fp,((_tmp373="]",
_tag_dyneither(_tmp373,sizeof(char),2))),_tag_dyneither(_tmp372,sizeof(void*),0)));}}}}
else{Cyc_Marshal_fprint_ref(fp,*tagp);}}goto _LL0;}_LL13: {struct Cyc_Typerep_XTUnion_struct*
_tmp37=(struct Cyc_Typerep_XTUnion_struct*)_tmp1F;if(_tmp37->tag != 9)goto _LL15;
else{_tmp38=_tmp37->f1;_tmp39=_tmp37->f2;}}_LL14: {unsigned int*xtunionp=((
unsigned int*(*)(void*x))unsafe_cast)(val);struct _tuple8 _tmpA6;int _tmpA7;struct
Cyc_Set_Set*_tmpA8;struct _tuple8*_tmpA5=Cyc_Marshal_member_insert(env,*xtunionp);
_tmpA6=*_tmpA5;_tmpA7=_tmpA6.f1;_tmpA8=_tmpA6.f2;env=_tmpA8;if(!_tmpA7){{const
char*_tmp377;void*_tmp376[1];struct Cyc_Int_pa_struct _tmp375;(_tmp375.tag=1,((
_tmp375.f1=*xtunionp,((_tmp376[0]=& _tmp375,Cyc_fprintf(fp,((_tmp377="(@%x)",
_tag_dyneither(_tmp377,sizeof(char),6))),_tag_dyneither(_tmp376,sizeof(void*),1)))))));}{
unsigned int*xtstructp=((unsigned int*(*)(unsigned int x))unsafe_cast)(*xtunionp);
if(*xtstructp != 0){char*xtnamec=((char*(*)(unsigned int x))unsafe_cast)(*
xtstructp + 4);struct _dyneither_ptr xtname=(struct _dyneither_ptr)Cstring_to_string(
xtnamec);{const char*_tmp37B;void*_tmp37A[1];struct Cyc_String_pa_struct _tmp379;(
_tmp379.tag=0,((_tmp379.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)xtname),((
_tmp37A[0]=& _tmp379,Cyc_fprintf(fp,((_tmp37B="&%s[( ",_tag_dyneither(_tmp37B,
sizeof(char),7))),_tag_dyneither(_tmp37A,sizeof(void*),1)))))));}{int _tmpB0;void*
_tmpB1;struct _tuple4 _tmpAF=Cyc_Typerep_get_xtunionbranch(xtname,_tmp39);_tmpB0=
_tmpAF.f1;_tmpB1=_tmpAF.f2;{void*xoff=((void*(*)(unsigned int x))unsafe_cast)(*
xtunionp + _tmpB0);env=Cyc_Marshal_fprint_type_base(_tmpB1,env,fp,xoff);{const
char*_tmp37E;void*_tmp37D;(_tmp37D=0,Cyc_fprintf(fp,((_tmp37E=" )]",
_tag_dyneither(_tmp37E,sizeof(char),4))),_tag_dyneither(_tmp37D,sizeof(void*),0)));}}}}
else{char*xtnamec=((char*(*)(unsigned int x))unsafe_cast)(*xtunionp + 4);struct
_dyneither_ptr xtname=(struct _dyneither_ptr)Cstring_to_string(xtnamec);const char*
_tmp382;void*_tmp381[1];struct Cyc_String_pa_struct _tmp380;(_tmp380.tag=0,((
_tmp380.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)xtname),((_tmp381[0]=&
_tmp380,Cyc_fprintf(fp,((_tmp382="%s",_tag_dyneither(_tmp382,sizeof(char),3))),
_tag_dyneither(_tmp381,sizeof(void*),1)))))));}}}else{Cyc_Marshal_fprint_ref(fp,*
xtunionp);}goto _LL0;}_LL15: {struct Cyc_Typerep_Union_struct*_tmp3A=(struct Cyc_Typerep_Union_struct*)
_tmp1F;if(_tmp3A->tag != 10)goto _LL17;else{_tmp3B=_tmp3A->f1;_tmp3C=_tmp3A->f3;}}
_LL16: if(_tmp3B == 0){const char*_tmp385;void*_tmp384;(_tmp384=0,Cyc_fprintf(fp,((
_tmp385="{",_tag_dyneither(_tmp385,sizeof(char),2))),_tag_dyneither(_tmp384,
sizeof(void*),0)));}else{const char*_tmp389;void*_tmp388[1];struct Cyc_String_pa_struct
_tmp387;(_tmp387.tag=0,((_tmp387.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*
_tmp3B),((_tmp388[0]=& _tmp387,Cyc_fprintf(fp,((_tmp389="%s{",_tag_dyneither(
_tmp389,sizeof(char),4))),_tag_dyneither(_tmp388,sizeof(void*),1)))))));}{int i=0;
for(0;i < _get_dyneither_size(_tmp3C,sizeof(struct _tuple3*));++ i){struct
_dyneither_ptr _tmpBD;void*_tmpBE;struct _tuple3 _tmpBC=*((struct _tuple3**)_tmp3C.curr)[
i];_tmpBD=_tmpBC.f1;_tmpBE=_tmpBC.f2;{const char*_tmp38D;void*_tmp38C[1];struct
Cyc_String_pa_struct _tmp38B;(_tmp38B.tag=0,((_tmp38B.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)_tmpBD),((_tmp38C[0]=& _tmp38B,Cyc_fprintf(fp,((_tmp38D="%s=",
_tag_dyneither(_tmp38D,sizeof(char),4))),_tag_dyneither(_tmp38C,sizeof(void*),1)))))));}
Cyc_Marshal_fprint_type_base(_tmpBE,env,fp,val);if(i < _get_dyneither_size(_tmp3C,
sizeof(struct _tuple3*))- 1){const char*_tmp390;void*_tmp38F;(_tmp38F=0,Cyc_fprintf(
fp,((_tmp390=" | ",_tag_dyneither(_tmp390,sizeof(char),4))),_tag_dyneither(
_tmp38F,sizeof(void*),0)));}}}{const char*_tmp393;void*_tmp392;(_tmp392=0,Cyc_fprintf(
fp,((_tmp393="}",_tag_dyneither(_tmp393,sizeof(char),2))),_tag_dyneither(_tmp392,
sizeof(void*),0)));}goto _LL0;_LL17: {struct Cyc_Typerep_Enum_struct*_tmp3D=(
struct Cyc_Typerep_Enum_struct*)_tmp1F;if(_tmp3D->tag != 11)goto _LL0;else{_tmp3E=
_tmp3D->f2;_tmp3F=_tmp3D->f3;}}_LL18: {unsigned int*tagp=((unsigned int*(*)(void*
x))unsafe_cast)(val);{const char*_tmp397;void*_tmp396[1];struct Cyc_String_pa_struct
_tmp395;(_tmp395.tag=0,((_tmp395.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Typerep_get_tagname(*tagp,_tmp3F)),((_tmp396[0]=& _tmp395,Cyc_fprintf(fp,((
_tmp397="%s",_tag_dyneither(_tmp397,sizeof(char),3))),_tag_dyneither(_tmp396,
sizeof(void*),1)))))));}goto _LL0;}_LL0:;}return env;}void Cyc_Marshal_fprint_type(
void*ts,struct Cyc___cycFILE*fp,void*val);void Cyc_Marshal_fprint_type(void*ts,
struct Cyc___cycFILE*fp,void*val){struct _handler_cons _tmpC9;_push_handler(& _tmpC9);{
int _tmpCB=0;if(setjmp(_tmpC9.handler))_tmpCB=1;if(!_tmpCB){Cyc_Marshal_fprint_type_base(
ts,Cyc_Marshal_empty_addr_set(),fp,val);;_pop_handler();}else{void*_tmpCA=(void*)
_exn_thrown;void*_tmpCD=_tmpCA;struct _dyneither_ptr _tmpCF;_LL1F: {struct Cyc_Core_Failure_struct*
_tmpCE=(struct Cyc_Core_Failure_struct*)_tmpCD;if(_tmpCE->tag != Cyc_Core_Failure)
goto _LL21;else{_tmpCF=_tmpCE->f1;}}_LL20:{const char*_tmp39B;void*_tmp39A[1];
struct Cyc_String_pa_struct _tmp399;(_tmp399.tag=0,((_tmp399.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)_tmpCF),((_tmp39A[0]=& _tmp399,Cyc_fprintf(
Cyc_stderr,((_tmp39B="Core::Failure(%s)\n",_tag_dyneither(_tmp39B,sizeof(char),
19))),_tag_dyneither(_tmp39A,sizeof(void*),1)))))));}{struct Cyc_Core_Failure_struct
_tmp39E;struct Cyc_Core_Failure_struct*_tmp39D;(int)_throw((void*)((_tmp39D=
_cycalloc(sizeof(*_tmp39D)),((_tmp39D[0]=((_tmp39E.tag=Cyc_Core_Failure,((
_tmp39E.f1=_tmpCF,_tmp39E)))),_tmp39D)))));}_LL21:;_LL22:(void)_throw(_tmpCD);
_LL1E:;}}}void Cyc_Marshal_print_type(void*ts,void*val);void Cyc_Marshal_print_type(
void*ts,void*val){Cyc_Marshal_fprint_type(ts,Cyc_stdout,val);}void Cyc_Marshal_cycputchar(
char x,struct Cyc___cycFILE*fp);void Cyc_Marshal_cycputchar(char x,struct Cyc___cycFILE*
fp){struct Cyc_Core_Failure_struct _tmp3A4;const char*_tmp3A3;struct Cyc_Core_Failure_struct*
_tmp3A2;(int)_throw((void*)((_tmp3A2=_cycalloc(sizeof(*_tmp3A2)),((_tmp3A2[0]=((
_tmp3A4.tag=Cyc_Core_Failure,((_tmp3A4.f1=((_tmp3A3="Write failure",
_tag_dyneither(_tmp3A3,sizeof(char),14))),_tmp3A4)))),_tmp3A2)))));}void Cyc_Marshal_cycputshort(
short x,struct Cyc___cycFILE*fp);void Cyc_Marshal_cycputshort(short x,struct Cyc___cycFILE*
fp){struct Cyc_Core_Failure_struct _tmp3AA;const char*_tmp3A9;struct Cyc_Core_Failure_struct*
_tmp3A8;(int)_throw((void*)((_tmp3A8=_cycalloc(sizeof(*_tmp3A8)),((_tmp3A8[0]=((
_tmp3AA.tag=Cyc_Core_Failure,((_tmp3AA.f1=((_tmp3A9="Write failure",
_tag_dyneither(_tmp3A9,sizeof(char),14))),_tmp3AA)))),_tmp3A8)))));}void Cyc_Marshal_cycputint(
int x,struct Cyc___cycFILE*fp);void Cyc_Marshal_cycputint(int x,struct Cyc___cycFILE*
fp){struct Cyc_Core_Failure_struct _tmp3B0;const char*_tmp3AF;struct Cyc_Core_Failure_struct*
_tmp3AE;(int)_throw((void*)((_tmp3AE=_cycalloc(sizeof(*_tmp3AE)),((_tmp3AE[0]=((
_tmp3B0.tag=Cyc_Core_Failure,((_tmp3B0.f1=((_tmp3AF="Write failure",
_tag_dyneither(_tmp3AF,sizeof(char),14))),_tmp3B0)))),_tmp3AE)))));}void Cyc_Marshal_cycputfloat(
float x,struct Cyc___cycFILE*fp);void Cyc_Marshal_cycputfloat(float x,struct Cyc___cycFILE*
fp){struct Cyc_Core_Failure_struct _tmp3B6;const char*_tmp3B5;struct Cyc_Core_Failure_struct*
_tmp3B4;(int)_throw((void*)((_tmp3B4=_cycalloc(sizeof(*_tmp3B4)),((_tmp3B4[0]=((
_tmp3B6.tag=Cyc_Core_Failure,((_tmp3B6.f1=((_tmp3B5="Write failure",
_tag_dyneither(_tmp3B5,sizeof(char),14))),_tmp3B6)))),_tmp3B4)))));}void Cyc_Marshal_cycputdouble(
double x,struct Cyc___cycFILE*fp);void Cyc_Marshal_cycputdouble(double x,struct Cyc___cycFILE*
fp){struct Cyc_Core_Failure_struct _tmp3BC;const char*_tmp3BB;struct Cyc_Core_Failure_struct*
_tmp3BA;(int)_throw((void*)((_tmp3BA=_cycalloc(sizeof(*_tmp3BA)),((_tmp3BA[0]=((
_tmp3BC.tag=Cyc_Core_Failure,((_tmp3BC.f1=((_tmp3BB="Write failure",
_tag_dyneither(_tmp3BB,sizeof(char),14))),_tmp3BC)))),_tmp3BA)))));}void Cyc_Marshal_cycputvarint(
unsigned int i,struct Cyc___cycFILE*fp);void Cyc_Marshal_cycputvarint(unsigned int i,
struct Cyc___cycFILE*fp){int width=0;unsigned int mask=-256;while((int)mask){if((
int)(i & mask))++ width;mask <<=8;}Cyc_fputc((int)(width << 6 | i & 63),fp);i >>=6;{int
j=0;for(0;j < width;++ j){Cyc_fputc((int)(i & 255),fp);i >>=8;}}}char Cyc_Marshal_cycgetchar(
struct Cyc___cycFILE*fp);static void _tmp3C1(unsigned int*_tmp3C0,unsigned int*
_tmp3BF,char**_tmp3BE){for(*_tmp3C0=0;*_tmp3C0 < *_tmp3BF;(*_tmp3C0)++){(*_tmp3BE)[*
_tmp3C0]='\000';}}char Cyc_Marshal_cycgetchar(struct Cyc___cycFILE*fp){struct
_RegionHandle _tmpE4=_new_region("r");struct _RegionHandle*r=& _tmpE4;_push_region(
r);{unsigned int _tmp3C0;unsigned int _tmp3BF;char*_tmp3BE;unsigned int _tmp3BD;
struct _dyneither_ptr x=_tag_dyneither(((_tmp3BD=sizeof(char),((_tmp3BE=(char*)
_region_malloc(r,_check_times(sizeof(char),_tmp3BD)),((((_tmp3BF=_tmp3BD,_tmp3C1(&
_tmp3C0,& _tmp3BF,& _tmp3BE))),_tmp3BE)))))),sizeof(char),sizeof(char));if(Cyc_fread(
x,sizeof(char),1,fp)< 1){struct Cyc_Core_Failure_struct _tmp3C7;const char*_tmp3C6;
struct Cyc_Core_Failure_struct*_tmp3C5;(int)_throw((void*)((_tmp3C5=_cycalloc(
sizeof(*_tmp3C5)),((_tmp3C5[0]=((_tmp3C7.tag=Cyc_Core_Failure,((_tmp3C7.f1=((
_tmp3C6="Read failure",_tag_dyneither(_tmp3C6,sizeof(char),13))),_tmp3C7)))),
_tmp3C5)))));}{char*xx=((char*(*)(char*x))unsafe_cast)((char*)_check_null(
_untag_dyneither_ptr(x,sizeof(char),1)));char _tmpE8=*xx;_npop_handler(0);return
_tmpE8;}};_pop_region(r);}short Cyc_Marshal_cycgetshort(struct Cyc___cycFILE*fp);
static void _tmp3CC(unsigned int*_tmp3CB,unsigned int*_tmp3CA,char**_tmp3C9){for(*
_tmp3CB=0;*_tmp3CB < *_tmp3CA;(*_tmp3CB)++){(*_tmp3C9)[*_tmp3CB]='\000';}}short
Cyc_Marshal_cycgetshort(struct Cyc___cycFILE*fp){struct _RegionHandle _tmpEC=
_new_region("r");struct _RegionHandle*r=& _tmpEC;_push_region(r);{unsigned int
_tmp3CB;unsigned int _tmp3CA;char*_tmp3C9;unsigned int _tmp3C8;struct
_dyneither_ptr x=_tag_dyneither(((_tmp3C8=sizeof(short),((_tmp3C9=(char*)
_region_malloc(r,_check_times(sizeof(char),_tmp3C8)),((((_tmp3CA=_tmp3C8,_tmp3CC(&
_tmp3CB,& _tmp3CA,& _tmp3C9))),_tmp3C9)))))),sizeof(char),sizeof(short));if(Cyc_fread(
x,sizeof(short),1,fp)< 1){struct Cyc_Core_Failure_struct _tmp3D2;const char*_tmp3D1;
struct Cyc_Core_Failure_struct*_tmp3D0;(int)_throw((void*)((_tmp3D0=_cycalloc(
sizeof(*_tmp3D0)),((_tmp3D0[0]=((_tmp3D2.tag=Cyc_Core_Failure,((_tmp3D2.f1=((
_tmp3D1="Read failure",_tag_dyneither(_tmp3D1,sizeof(char),13))),_tmp3D2)))),
_tmp3D0)))));}{short*xx=((short*(*)(char*x))unsafe_cast)((char*)_check_null(
_untag_dyneither_ptr(x,sizeof(char),1)));short _tmpF0=*xx;_npop_handler(0);return
_tmpF0;}};_pop_region(r);}int Cyc_Marshal_cycgetint(struct Cyc___cycFILE*fp);
static void _tmp3D7(unsigned int*_tmp3D6,unsigned int*_tmp3D5,char**_tmp3D4){for(*
_tmp3D6=0;*_tmp3D6 < *_tmp3D5;(*_tmp3D6)++){(*_tmp3D4)[*_tmp3D6]='\000';}}int Cyc_Marshal_cycgetint(
struct Cyc___cycFILE*fp){struct _RegionHandle _tmpF4=_new_region("r");struct
_RegionHandle*r=& _tmpF4;_push_region(r);{unsigned int _tmp3D6;unsigned int _tmp3D5;
char*_tmp3D4;unsigned int _tmp3D3;struct _dyneither_ptr x=_tag_dyneither(((_tmp3D3=
sizeof(int),((_tmp3D4=(char*)_region_malloc(r,_check_times(sizeof(char),_tmp3D3)),((((
_tmp3D5=_tmp3D3,_tmp3D7(& _tmp3D6,& _tmp3D5,& _tmp3D4))),_tmp3D4)))))),sizeof(char),
sizeof(int));if(Cyc_fread(x,sizeof(int),1,fp)< 1){struct Cyc_Core_Failure_struct
_tmp3DD;const char*_tmp3DC;struct Cyc_Core_Failure_struct*_tmp3DB;(int)_throw((
void*)((_tmp3DB=_cycalloc(sizeof(*_tmp3DB)),((_tmp3DB[0]=((_tmp3DD.tag=Cyc_Core_Failure,((
_tmp3DD.f1=((_tmp3DC="Read failure",_tag_dyneither(_tmp3DC,sizeof(char),13))),
_tmp3DD)))),_tmp3DB)))));}{int*xx=((int*(*)(char*x))unsafe_cast)((char*)
_check_null(_untag_dyneither_ptr(x,sizeof(char),1)));int _tmpF8=*xx;_npop_handler(
0);return _tmpF8;}};_pop_region(r);}float Cyc_Marshal_cycgetfloat(struct Cyc___cycFILE*
fp);static void _tmp3E2(unsigned int*_tmp3E1,unsigned int*_tmp3E0,char**_tmp3DF){
for(*_tmp3E1=0;*_tmp3E1 < *_tmp3E0;(*_tmp3E1)++){(*_tmp3DF)[*_tmp3E1]='\000';}}
float Cyc_Marshal_cycgetfloat(struct Cyc___cycFILE*fp){struct _RegionHandle _tmpFC=
_new_region("r");struct _RegionHandle*r=& _tmpFC;_push_region(r);{unsigned int
_tmp3E1;unsigned int _tmp3E0;char*_tmp3DF;unsigned int _tmp3DE;struct
_dyneither_ptr x=_tag_dyneither(((_tmp3DE=sizeof(float),((_tmp3DF=(char*)
_region_malloc(r,_check_times(sizeof(char),_tmp3DE)),((((_tmp3E0=_tmp3DE,_tmp3E2(&
_tmp3E1,& _tmp3E0,& _tmp3DF))),_tmp3DF)))))),sizeof(char),sizeof(float));if(Cyc_fread(
x,sizeof(float),1,fp)< 1){struct Cyc_Core_Failure_struct _tmp3E8;const char*_tmp3E7;
struct Cyc_Core_Failure_struct*_tmp3E6;(int)_throw((void*)((_tmp3E6=_cycalloc(
sizeof(*_tmp3E6)),((_tmp3E6[0]=((_tmp3E8.tag=Cyc_Core_Failure,((_tmp3E8.f1=((
_tmp3E7="Read failure",_tag_dyneither(_tmp3E7,sizeof(char),13))),_tmp3E8)))),
_tmp3E6)))));}{float*xx=((float*(*)(char*x))unsafe_cast)((char*)_check_null(
_untag_dyneither_ptr(x,sizeof(char),1)));float _tmp100=*xx;_npop_handler(0);
return _tmp100;}};_pop_region(r);}double Cyc_Marshal_cycgetdouble(struct Cyc___cycFILE*
fp);static void _tmp3ED(unsigned int*_tmp3EC,unsigned int*_tmp3EB,char**_tmp3EA){
for(*_tmp3EC=0;*_tmp3EC < *_tmp3EB;(*_tmp3EC)++){(*_tmp3EA)[*_tmp3EC]='\000';}}
double Cyc_Marshal_cycgetdouble(struct Cyc___cycFILE*fp){struct _RegionHandle
_tmp104=_new_region("r");struct _RegionHandle*r=& _tmp104;_push_region(r);{
unsigned int _tmp3EC;unsigned int _tmp3EB;char*_tmp3EA;unsigned int _tmp3E9;struct
_dyneither_ptr x=_tag_dyneither(((_tmp3E9=sizeof(double),((_tmp3EA=(char*)
_region_malloc(r,_check_times(sizeof(char),_tmp3E9)),((((_tmp3EB=_tmp3E9,_tmp3ED(&
_tmp3EC,& _tmp3EB,& _tmp3EA))),_tmp3EA)))))),sizeof(char),sizeof(double));if(Cyc_fread(
x,sizeof(double),1,fp)< 1){struct Cyc_Core_Failure_struct _tmp3F3;const char*
_tmp3F2;struct Cyc_Core_Failure_struct*_tmp3F1;(int)_throw((void*)((_tmp3F1=
_cycalloc(sizeof(*_tmp3F1)),((_tmp3F1[0]=((_tmp3F3.tag=Cyc_Core_Failure,((
_tmp3F3.f1=((_tmp3F2="Read failure",_tag_dyneither(_tmp3F2,sizeof(char),13))),
_tmp3F3)))),_tmp3F1)))));}{double*xx=((double*(*)(char*x))unsafe_cast)((char*)
_check_null(_untag_dyneither_ptr(x,sizeof(char),1)));double _tmp108=*xx;
_npop_handler(0);return _tmp108;}};_pop_region(r);}unsigned int Cyc_Marshal_cycgetvarint(
struct Cyc___cycFILE*fp);unsigned int Cyc_Marshal_cycgetvarint(struct Cyc___cycFILE*
fp){int i=0;int j=(int)Cyc_Marshal_cycgetchar(fp);int width=(j & 192)>> 6;int pos=6;i
+=j & 63;{int k=0;for(0;k < width;++ k){j=(int)Cyc_Marshal_cycgetchar(fp);i |=j << pos;
pos +=8;}}return(unsigned int)i;}struct _tuple6 Cyc_Marshal_write_type_base(void*
rep,struct _tuple6 env,struct Cyc___cycFILE*fp,void*val);static void _tmp40B(
unsigned int*_tmp40A,unsigned int*_tmp409,struct _tuple1***_tmp407){for(*_tmp40A=
0;*_tmp40A < *_tmp409;(*_tmp40A)++){struct Cyc_Typerep_Int_struct*_tmp405;struct
Cyc_Typerep_Int_struct _tmp404;struct _tuple1*_tmp403;(*_tmp407)[*_tmp40A]=((
_tmp403=_cycalloc(sizeof(*_tmp403)),((_tmp403->f1=*_tmp40A,((_tmp403->f2=(void*)((
_tmp405=_cycalloc_atomic(sizeof(*_tmp405)),((_tmp405[0]=((_tmp404.tag=0,((
_tmp404.f1=0,((_tmp404.f2=8,_tmp404)))))),_tmp405)))),_tmp403))))));}}struct
_tuple6 Cyc_Marshal_write_type_base(void*rep,struct _tuple6 env,struct Cyc___cycFILE*
fp,void*val){{void*_tmp10C=rep;unsigned int _tmp10E;unsigned int _tmp112;void*
_tmp113;void*_tmp115;struct _dyneither_ptr _tmp117;struct _dyneither_ptr _tmp119;
struct _dyneither_ptr _tmp11B;struct _dyneither_ptr _tmp11D;struct _dyneither_ptr
_tmp11E;struct _dyneither_ptr _tmp120;int _tmp122;int _tmp124;_LL24: {struct Cyc_Typerep_Int_struct*
_tmp10D=(struct Cyc_Typerep_Int_struct*)_tmp10C;if(_tmp10D->tag != 0)goto _LL26;
else{_tmp10E=_tmp10D->f2;}}_LL25: switch(_tmp10E){case 8: _LL3C: {char*x=((char*(*)(
void*x))unsafe_cast)(val);Cyc_Marshal_cycputchar(*x,fp);break;}case 16: _LL3D: {
short*x=((short*(*)(void*x))unsafe_cast)(val);Cyc_Marshal_cycputshort(*x,fp);
break;}case 32: _LL3E: {int*x=((int*(*)(void*x))unsafe_cast)(val);Cyc_Marshal_cycputint(*
x,fp);break;}default: _LL3F:{const char*_tmp3F7;void*_tmp3F6[1];struct Cyc_Int_pa_struct
_tmp3F5;(_tmp3F5.tag=1,((_tmp3F5.f1=(unsigned long)((int)_tmp10E),((_tmp3F6[0]=&
_tmp3F5,Cyc_fprintf(Cyc_stderr,((_tmp3F7="write_type_base: illegal int size %d bits\n",
_tag_dyneither(_tmp3F7,sizeof(char),43))),_tag_dyneither(_tmp3F6,sizeof(void*),1)))))));}
exit(1);}goto _LL23;_LL26: {struct Cyc_Typerep_Float_struct*_tmp10F=(struct Cyc_Typerep_Float_struct*)
_tmp10C;if(_tmp10F->tag != 1)goto _LL28;}_LL27: {float*x=((float*(*)(void*x))
unsafe_cast)(val);Cyc_Marshal_cycputfloat(*x,fp);goto _LL23;}_LL28: {struct Cyc_Typerep_Double_struct*
_tmp110=(struct Cyc_Typerep_Double_struct*)_tmp10C;if(_tmp110->tag != 2)goto _LL2A;}
_LL29: {double*x=((double*(*)(void*x))unsafe_cast)(val);Cyc_Marshal_cycputdouble(*
x,fp);goto _LL23;}_LL2A: {struct Cyc_Typerep_ThinPtr_struct*_tmp111=(struct Cyc_Typerep_ThinPtr_struct*)
_tmp10C;if(_tmp111->tag != 3)goto _LL2C;else{_tmp112=_tmp111->f1;_tmp113=(void*)
_tmp111->f2;}}_LL2B: {int*x=((int*(*)(void*x))unsafe_cast)(val);int y=*((int*)
_check_null(x));unsigned int _tmp128=Cyc_Marshal_index_lookup(env,(unsigned int)y);
Cyc_Marshal_cycputvarint(_tmp128,fp);if(!Cyc_Marshal_index_member(env,_tmp128)){
env=Cyc_Marshal_index_insert(env,(unsigned int)y);{int typ_szb=(int)Cyc_Typerep_size_type(
_tmp113);int i=0;for(0;i < _tmp112;++ i){env=Cyc_Marshal_write_type_base(_tmp113,
env,fp,((void*(*)(int x))unsafe_cast)(y));y +=typ_szb;}}}goto _LL23;}_LL2C: {struct
Cyc_Typerep_FatPtr_struct*_tmp114=(struct Cyc_Typerep_FatPtr_struct*)_tmp10C;if(
_tmp114->tag != 4)goto _LL2E;else{_tmp115=(void*)_tmp114->f1;}}_LL2D: {struct
_tuple9*x=((struct _tuple9*(*)(void*x))unsafe_cast)(val);unsigned int typ_szb=Cyc_Typerep_size_type(
_tmp115);unsigned int base=(*x).f1;unsigned int curr=(*x).f2;unsigned int
last_plus_one=(*x).f3;unsigned int pos=(curr - base)/ typ_szb;unsigned int sz=(
last_plus_one - base)/ typ_szb;unsigned int _tmp129=Cyc_Marshal_index_lookup(env,
base);Cyc_Marshal_cycputvarint(_tmp129,fp);Cyc_Marshal_cycputvarint(pos,fp);Cyc_Marshal_cycputvarint(
sz,fp);if(!Cyc_Marshal_index_member(env,_tmp129)){env=Cyc_Marshal_index_insert(
env,base);{unsigned int i=base;for(0;i < last_plus_one;i +=typ_szb){env=Cyc_Marshal_write_type_base(
_tmp115,env,fp,((void*(*)(unsigned int x))unsafe_cast)(i));}}}goto _LL23;}_LL2E: {
struct Cyc_Typerep_Struct_struct*_tmp116=(struct Cyc_Typerep_Struct_struct*)
_tmp10C;if(_tmp116->tag != 5)goto _LL30;else{_tmp117=_tmp116->f3;}}_LL2F: {int x=((
int(*)(void*x))unsafe_cast)(val);{int i=0;for(0;i < _get_dyneither_size(_tmp117,
sizeof(struct _tuple0*));++ i){unsigned int _tmp12B;void*_tmp12C;struct _tuple0
_tmp12A=*((struct _tuple0**)_tmp117.curr)[i];_tmp12B=_tmp12A.f1;_tmp12C=_tmp12A.f3;{
int x_ofs=(int)(x + _tmp12B);env=Cyc_Marshal_write_type_base(_tmp12C,env,fp,((void*(*)(
int x))unsafe_cast)(x_ofs));}}}goto _LL23;}_LL30: {struct Cyc_Typerep_Tuple_struct*
_tmp118=(struct Cyc_Typerep_Tuple_struct*)_tmp10C;if(_tmp118->tag != 6)goto _LL32;
else{_tmp119=_tmp118->f2;}}_LL31: {int x=((int(*)(void*x))unsafe_cast)(val);{int i=
0;for(0;i < _get_dyneither_size(_tmp119,sizeof(struct _tuple1*));++ i){unsigned int
_tmp12E;void*_tmp12F;struct _tuple1 _tmp12D=*((struct _tuple1**)_tmp119.curr)[i];
_tmp12E=_tmp12D.f1;_tmp12F=_tmp12D.f2;{int x_ofs=(int)(x + _tmp12E);env=Cyc_Marshal_write_type_base(
_tmp12F,env,fp,((void*(*)(int x))unsafe_cast)(x_ofs));}}}goto _LL23;}_LL32: {
struct Cyc_Typerep_TUnionField_struct*_tmp11A=(struct Cyc_Typerep_TUnionField_struct*)
_tmp10C;if(_tmp11A->tag != 8)goto _LL34;else{_tmp11B=_tmp11A->f4;}}_LL33: {int x=((
int(*)(void*x))unsafe_cast)(val);{int i=0;for(0;i < _get_dyneither_size(_tmp11B,
sizeof(struct _tuple1*));++ i){unsigned int _tmp131;void*_tmp132;struct _tuple1
_tmp130=*((struct _tuple1**)_tmp11B.curr)[i];_tmp131=_tmp130.f1;_tmp132=_tmp130.f2;{
int x_ofs=(int)(x + _tmp131);env=Cyc_Marshal_write_type_base(_tmp132,env,fp,((void*(*)(
int x))unsafe_cast)(x_ofs));}}}goto _LL23;}_LL34: {struct Cyc_Typerep_TUnion_struct*
_tmp11C=(struct Cyc_Typerep_TUnion_struct*)_tmp10C;if(_tmp11C->tag != 7)goto _LL36;
else{_tmp11D=_tmp11C->f2;_tmp11E=_tmp11C->f3;}}_LL35: {unsigned int*tagp=((
unsigned int*(*)(void*x))unsafe_cast)(val);if(*tagp < 1024)Cyc_Marshal_cycputvarint(*
tagp,fp);else{unsigned int _tmp133=Cyc_Marshal_index_lookup(env,*tagp);Cyc_Marshal_cycputvarint(
_tmp133 + 1024,fp);if(!Cyc_Marshal_index_member(env,_tmp133)){env=Cyc_Marshal_index_insert(
env,*tagp);{unsigned int*x=((unsigned int*(*)(unsigned int x))unsafe_cast)(*tagp);
unsigned int tag=*x;Cyc_Marshal_cycputvarint(tag,fp);{int _tmp135;void*_tmp136;
struct _tuple4 _tmp134=Cyc_Typerep_get_unionbranch(tag,_tmp11E);_tmp135=_tmp134.f1;
_tmp136=_tmp134.f2;{void*xoff=((void*(*)(unsigned int x))unsafe_cast)(*tagp + 
_tmp135);env=Cyc_Marshal_write_type_base(_tmp136,env,fp,xoff);}}}}}goto _LL23;}
_LL36: {struct Cyc_Typerep_XTUnion_struct*_tmp11F=(struct Cyc_Typerep_XTUnion_struct*)
_tmp10C;if(_tmp11F->tag != 9)goto _LL38;else{_tmp120=_tmp11F->f2;}}_LL37: {
unsigned int*xtunionp=((unsigned int*(*)(void*x))unsafe_cast)(val);unsigned int
_tmp137=Cyc_Marshal_index_lookup(env,*xtunionp);Cyc_Marshal_cycputvarint(_tmp137,
fp);if(!Cyc_Marshal_index_member(env,_tmp137)){env=Cyc_Marshal_index_insert(env,*
xtunionp);{unsigned int*xtstructp=((unsigned int*(*)(unsigned int x))unsafe_cast)(*
xtunionp);if(*xtstructp != 0){Cyc_Marshal_cycputchar('\001',fp);{char*xtnamec=((
char*(*)(unsigned int x))unsafe_cast)(*xtstructp + 4);struct _dyneither_ptr xtname=(
struct _dyneither_ptr)Cstring_to_string(xtnamec);{const char*_tmp3FB;void*_tmp3FA[
1];struct Cyc_String_pa_struct _tmp3F9;(_tmp3F9.tag=0,((_tmp3F9.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)xtname),((_tmp3FA[0]=& _tmp3F9,Cyc_fprintf(
fp,((_tmp3FB="%s",_tag_dyneither(_tmp3FB,sizeof(char),3))),_tag_dyneither(
_tmp3FA,sizeof(void*),1)))))));}Cyc_Marshal_cycputchar('\000',fp);{int _tmp13C;
void*_tmp13D;struct _tuple4 _tmp13B=Cyc_Typerep_get_xtunionbranch(xtname,_tmp120);
_tmp13C=_tmp13B.f1;_tmp13D=_tmp13B.f2;{void*xoff=((void*(*)(unsigned int x))
unsafe_cast)(*xtunionp + _tmp13C);env=Cyc_Marshal_write_type_base(_tmp13D,env,fp,
xoff);}}}}else{Cyc_Marshal_cycputchar('\000',fp);{char*xtnamec=((char*(*)(
unsigned int x))unsafe_cast)(*xtunionp + 4);struct _dyneither_ptr xtname=(struct
_dyneither_ptr)Cstring_to_string(xtnamec);{const char*_tmp3FF;void*_tmp3FE[1];
struct Cyc_String_pa_struct _tmp3FD;(_tmp3FD.tag=0,((_tmp3FD.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)xtname),((_tmp3FE[0]=& _tmp3FD,Cyc_fprintf(
fp,((_tmp3FF="%s",_tag_dyneither(_tmp3FF,sizeof(char),3))),_tag_dyneither(
_tmp3FE,sizeof(void*),1)))))));}Cyc_Marshal_cycputchar('\000',fp);}}}}goto _LL23;}
_LL38: {struct Cyc_Typerep_Union_struct*_tmp121=(struct Cyc_Typerep_Union_struct*)
_tmp10C;if(_tmp121->tag != 10)goto _LL3A;else{_tmp122=_tmp121->f2;}}_LL39:{struct
Cyc_Typerep_Tuple_struct _tmp418;unsigned int _tmp417;unsigned int _tmp416;struct
_dyneither_ptr _tmp415;struct _tuple1**_tmp414;unsigned int _tmp413;struct Cyc_Typerep_Tuple_struct*
_tmp412;env=Cyc_Marshal_write_type_base((void*)((_tmp412=_cycalloc(sizeof(*
_tmp412)),((_tmp412[0]=((_tmp418.tag=6,((_tmp418.f1=(unsigned int)_tmp122,((
_tmp418.f2=((_tmp413=(unsigned int)_tmp122,((_tmp414=(struct _tuple1**)_cycalloc(
_check_times(sizeof(struct _tuple1*),_tmp413)),((_tmp415=_tag_dyneither(_tmp414,
sizeof(struct _tuple1*),_tmp413),((((_tmp416=_tmp413,_tmp40B(& _tmp417,& _tmp416,&
_tmp414))),_tmp415)))))))),_tmp418)))))),_tmp412)))),env,fp,val);}goto _LL23;
_LL3A: {struct Cyc_Typerep_Enum_struct*_tmp123=(struct Cyc_Typerep_Enum_struct*)
_tmp10C;if(_tmp123->tag != 11)goto _LL23;else{_tmp124=_tmp123->f2;}}_LL3B: {struct
Cyc_Typerep_Int_struct _tmp41B;struct Cyc_Typerep_Int_struct*_tmp41A;env=Cyc_Marshal_write_type_base((
void*)((_tmp41A=_cycalloc_atomic(sizeof(*_tmp41A)),((_tmp41A[0]=((_tmp41B.tag=0,((
_tmp41B.f1=0,((_tmp41B.f2=(unsigned int)(_tmp124 << 3),_tmp41B)))))),_tmp41A)))),
env,fp,val);}_LL23:;}return env;}void Cyc_Marshal_write_type(void*ts,struct Cyc___cycFILE*
fp,void*val);void Cyc_Marshal_write_type(void*ts,struct Cyc___cycFILE*fp,void*val){
struct _handler_cons _tmp14C;_push_handler(& _tmp14C);{int _tmp14E=0;if(setjmp(
_tmp14C.handler))_tmp14E=1;if(!_tmp14E){Cyc_Marshal_write_type_base(ts,Cyc_Marshal_empty_addr_index(),
fp,val);;_pop_handler();}else{void*_tmp14D=(void*)_exn_thrown;void*_tmp150=
_tmp14D;struct _dyneither_ptr _tmp152;_LL42: {struct Cyc_Core_Failure_struct*
_tmp151=(struct Cyc_Core_Failure_struct*)_tmp150;if(_tmp151->tag != Cyc_Core_Failure)
goto _LL44;else{_tmp152=_tmp151->f1;}}_LL43:{const char*_tmp41F;void*_tmp41E[1];
struct Cyc_String_pa_struct _tmp41D;(_tmp41D.tag=0,((_tmp41D.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)_tmp152),((_tmp41E[0]=& _tmp41D,Cyc_fprintf(
Cyc_stderr,((_tmp41F="Core::Failure(%s)\n",_tag_dyneither(_tmp41F,sizeof(char),
19))),_tag_dyneither(_tmp41E,sizeof(void*),1)))))));}{struct Cyc_Core_Failure_struct
_tmp422;struct Cyc_Core_Failure_struct*_tmp421;(int)_throw((void*)((_tmp421=
_cycalloc(sizeof(*_tmp421)),((_tmp421[0]=((_tmp422.tag=Cyc_Core_Failure,((
_tmp422.f1=_tmp152,_tmp422)))),_tmp421)))));}_LL44:;_LL45:(void)_throw(_tmp150);
_LL41:;}}}void Cyc_Marshal_fscanntstring(struct Cyc___cycFILE*fp,struct
_dyneither_ptr s);void Cyc_Marshal_fscanntstring(struct Cyc___cycFILE*fp,struct
_dyneither_ptr s){int c;int i=0;while((c=Cyc_fgetc(fp))!= '\000'){if(c == - 1){struct
Cyc_Core_Failure_struct _tmp428;const char*_tmp427;struct Cyc_Core_Failure_struct*
_tmp426;(int)_throw((void*)((_tmp426=_cycalloc(sizeof(*_tmp426)),((_tmp426[0]=((
_tmp428.tag=Cyc_Core_Failure,((_tmp428.f1=((_tmp427="Read error",_tag_dyneither(
_tmp427,sizeof(char),11))),_tmp428)))),_tmp426)))));}{char _tmp42B;char _tmp42A;
struct _dyneither_ptr _tmp429;(_tmp429=_dyneither_ptr_plus(s,sizeof(char),i),((
_tmp42A=*((char*)_check_dyneither_subscript(_tmp429,sizeof(char),0)),((_tmp42B=(
char)c,((_get_dyneither_size(_tmp429,sizeof(char))== 1  && (_tmp42A == '\000'  && 
_tmp42B != '\000')?_throw_arraybounds(): 1,*((char*)_tmp429.curr)=_tmp42B)))))));}
++ i;}{char _tmp42E;char _tmp42D;struct _dyneither_ptr _tmp42C;(_tmp42C=
_dyneither_ptr_plus(s,sizeof(char),i),((_tmp42D=*((char*)
_check_dyneither_subscript(_tmp42C,sizeof(char),0)),((_tmp42E='\000',((
_get_dyneither_size(_tmp42C,sizeof(char))== 1  && (_tmp42D == '\000'  && _tmp42E != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp42C.curr)=_tmp42E)))))));}}struct _tuple7 Cyc_Marshal_rread_type_base(
struct _RegionHandle*r,void*rep,struct _tuple7 env,struct Cyc___cycFILE*fp,void*
result);static void _tmp43F(unsigned int*_tmp43E,unsigned int*_tmp43D,char**
_tmp43B){for(*_tmp43E=0;*_tmp43E < *_tmp43D;(*_tmp43E)++){(*_tmp43B)[*_tmp43E]='\000';}}
static void _tmp445(unsigned int*_tmp444,unsigned int*_tmp443,char**_tmp441){for(*
_tmp444=0;*_tmp444 < *_tmp443;(*_tmp444)++){(*_tmp441)[*_tmp444]='\000';}}static
void _tmp44B(unsigned int*_tmp44A,unsigned int*_tmp449,char**_tmp447){for(*
_tmp44A=0;*_tmp44A < *_tmp449;(*_tmp44A)++){(*_tmp447)[*_tmp44A]='\000';}}static
void _tmp450(unsigned int*_tmp44F,unsigned int*_tmp44E,char**_tmp44D){for(*
_tmp44F=0;*_tmp44F < *_tmp44E;(*_tmp44F)++){(*_tmp44D)[*_tmp44F]='\000';}}static
void _tmp45B(unsigned int*_tmp45A,unsigned int*_tmp459,char**_tmp457){for(*
_tmp45A=0;*_tmp45A < *_tmp459;(*_tmp45A)++){(*_tmp457)[*_tmp45A]='\000';}}static
void _tmp467(unsigned int*_tmp466,unsigned int*_tmp465,struct _tuple1***_tmp463){
for(*_tmp466=0;*_tmp466 < *_tmp465;(*_tmp466)++){struct Cyc_Typerep_Int_struct*
_tmp461;struct Cyc_Typerep_Int_struct _tmp460;struct _tuple1*_tmp45F;(*_tmp463)[*
_tmp466]=((_tmp45F=_cycalloc(sizeof(*_tmp45F)),((_tmp45F->f1=*_tmp466,((_tmp45F->f2=(
void*)((_tmp461=_cycalloc_atomic(sizeof(*_tmp461)),((_tmp461[0]=((_tmp460.tag=0,((
_tmp460.f1=0,((_tmp460.f2=8,_tmp460)))))),_tmp461)))),_tmp45F))))));}}struct
_tuple7 Cyc_Marshal_rread_type_base(struct _RegionHandle*r,void*rep,struct _tuple7
env,struct Cyc___cycFILE*fp,void*result){{void*_tmp161=rep;unsigned int _tmp163;
unsigned int _tmp167;void*_tmp168;void*_tmp16A;struct _dyneither_ptr _tmp16C;struct
_dyneither_ptr _tmp16E;struct _dyneither_ptr _tmp170;struct _dyneither_ptr _tmp172;
struct _dyneither_ptr _tmp173;struct _dyneither_ptr _tmp175;int _tmp177;int _tmp179;
_LL47: {struct Cyc_Typerep_Int_struct*_tmp162=(struct Cyc_Typerep_Int_struct*)
_tmp161;if(_tmp162->tag != 0)goto _LL49;else{_tmp163=_tmp162->f2;}}_LL48: switch(
_tmp163){case 8: _LL5F: {char*x=((char*(*)(void*x))unsafe_cast)(result);{char
_tmp433;char _tmp432;struct _dyneither_ptr _tmp431;char*_tmp430;(_tmp431=((_tmp430=
x,_tag_dyneither(_tmp430,sizeof(char),_get_zero_arr_size_char(_tmp430,1)))),((
_tmp432=*((char*)_check_dyneither_subscript(_tmp431,sizeof(char),0)),((_tmp433=
Cyc_Marshal_cycgetchar(fp),((_get_dyneither_size(_tmp431,sizeof(char))== 1  && (
_tmp432 == '\000'  && _tmp433 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp431.curr)=
_tmp433)))))));}break;}case 16: _LL60: {short*x=((short*(*)(void*x))unsafe_cast)(
result);*x=Cyc_Marshal_cycgetshort(fp);break;}case 32: _LL61: {int*x=((int*(*)(
void*x))unsafe_cast)(result);*x=Cyc_Marshal_cycgetint(fp);break;}default: _LL62: {
struct Cyc_Core_Failure_struct _tmp439;const char*_tmp438;struct Cyc_Core_Failure_struct*
_tmp437;(int)_throw((void*)((_tmp437=_cycalloc(sizeof(*_tmp437)),((_tmp437[0]=((
_tmp439.tag=Cyc_Core_Failure,((_tmp439.f1=((_tmp438="read_type_base: illegal int size bits",
_tag_dyneither(_tmp438,sizeof(char),38))),_tmp439)))),_tmp437)))));}}goto _LL46;
_LL49: {struct Cyc_Typerep_Float_struct*_tmp164=(struct Cyc_Typerep_Float_struct*)
_tmp161;if(_tmp164->tag != 1)goto _LL4B;}_LL4A: {float*y=((float*(*)(void*x))
unsafe_cast)(result);*y=Cyc_Marshal_cycgetfloat(fp);goto _LL46;}_LL4B: {struct Cyc_Typerep_Double_struct*
_tmp165=(struct Cyc_Typerep_Double_struct*)_tmp161;if(_tmp165->tag != 2)goto _LL4D;}
_LL4C: {double*y=((double*(*)(void*x))unsafe_cast)(result);*y=Cyc_Marshal_cycgetdouble(
fp);goto _LL46;}_LL4D: {struct Cyc_Typerep_ThinPtr_struct*_tmp166=(struct Cyc_Typerep_ThinPtr_struct*)
_tmp161;if(_tmp166->tag != 3)goto _LL4F;else{_tmp167=_tmp166->f1;_tmp168=(void*)
_tmp166->f2;}}_LL4E: {int mem=(int)Cyc_Marshal_cycgetvarint(fp);if(!Cyc_Marshal_table_member(
env,(unsigned int)mem)){int typ_szb=(int)Cyc_Typerep_size_type(_tmp168);
unsigned int _tmp43E;unsigned int _tmp43D;struct _dyneither_ptr _tmp43C;char*_tmp43B;
unsigned int _tmp43A;struct _dyneither_ptr buf=(_tmp43A=_tmp167 * typ_szb,((_tmp43B=(
char*)_cycalloc_atomic(_check_times(sizeof(char),_tmp43A + 1)),((_tmp43C=
_tag_dyneither(_tmp43B,sizeof(char),_tmp43A + 1),((((_tmp43D=_tmp43A,((_tmp43F(&
_tmp43E,& _tmp43D,& _tmp43B),_tmp43B[_tmp43D]=(char)0)))),_tmp43C)))))));
unsigned int bufptr=(unsigned int)((char*)_check_null(_untag_dyneither_ptr(buf,
sizeof(char),1 + 1)));unsigned int*x=((unsigned int*(*)(void*x))unsafe_cast)(
result);*x=bufptr;env=Cyc_Marshal_table_insert(env,bufptr);{int y=(int)bufptr;int
i=0;for(0;i < _tmp167;++ i){env=Cyc_Marshal_rread_type_base(r,_tmp168,env,fp,((
void*(*)(int x))unsafe_cast)(y));y +=typ_szb;}}}else{unsigned int*x=((unsigned int*(*)(
void*x))unsafe_cast)(result);*x=Cyc_Marshal_table_lookup(env,mem);}goto _LL46;}
_LL4F: {struct Cyc_Typerep_FatPtr_struct*_tmp169=(struct Cyc_Typerep_FatPtr_struct*)
_tmp161;if(_tmp169->tag != 4)goto _LL51;else{_tmp16A=(void*)_tmp169->f1;}}_LL50: {
int mem=(int)Cyc_Marshal_cycgetvarint(fp);int pos=(int)Cyc_Marshal_cycgetvarint(fp);
int sz=(int)Cyc_Marshal_cycgetvarint(fp);int typ_szb=(int)Cyc_Typerep_size_type(
_tmp16A);if(!Cyc_Marshal_table_member(env,(unsigned int)mem)){unsigned int
_tmp444;unsigned int _tmp443;struct _dyneither_ptr _tmp442;char*_tmp441;
unsigned int _tmp440;struct _dyneither_ptr buf=(_tmp440=(unsigned int)(sz * typ_szb),((
_tmp441=(char*)_cycalloc_atomic(_check_times(sizeof(char),_tmp440 + 1)),((_tmp442=
_tag_dyneither(_tmp441,sizeof(char),_tmp440 + 1),((((_tmp443=_tmp440,((_tmp445(&
_tmp444,& _tmp443,& _tmp441),_tmp441[_tmp443]=(char)0)))),_tmp442)))))));
unsigned int bufptr=(unsigned int)((char*)_check_null(_untag_dyneither_ptr(buf,
sizeof(char),1 + 1)));struct _tuple9*x=((struct _tuple9*(*)(void*x))unsafe_cast)(
result);(*x).f1=bufptr;(*x).f2=bufptr + pos * typ_szb;(*x).f3=bufptr + sz * typ_szb;
env=Cyc_Marshal_table_insert(env,bufptr);{int y=(int)bufptr;int i=0;for(0;i < sz;++
i){env=Cyc_Marshal_rread_type_base(r,_tmp16A,env,fp,((void*(*)(int x))unsafe_cast)(
y));y +=typ_szb;}}}else{struct _tuple9*x=((struct _tuple9*(*)(void*x))unsafe_cast)(
result);(*x).f1=Cyc_Marshal_table_lookup(env,mem);(*x).f2=(*x).f1 + typ_szb * pos;(*
x).f3=(*x).f1 + typ_szb * sz;}goto _LL46;}_LL51: {struct Cyc_Typerep_Struct_struct*
_tmp16B=(struct Cyc_Typerep_Struct_struct*)_tmp161;if(_tmp16B->tag != 5)goto _LL53;
else{_tmp16C=_tmp16B->f3;}}_LL52: {unsigned int x=((unsigned int(*)(void*x))
unsafe_cast)(result);{int i=0;for(0;i < _get_dyneither_size(_tmp16C,sizeof(struct
_tuple0*));++ i){unsigned int _tmp18A;void*_tmp18B;struct _tuple0 _tmp189=*((struct
_tuple0**)_tmp16C.curr)[i];_tmp18A=_tmp189.f1;_tmp18B=_tmp189.f3;{int x_ofs=(int)(
x + _tmp18A);env=Cyc_Marshal_rread_type_base(r,_tmp18B,env,fp,((void*(*)(int x))
unsafe_cast)(x_ofs));}}}goto _LL46;}_LL53: {struct Cyc_Typerep_Tuple_struct*
_tmp16D=(struct Cyc_Typerep_Tuple_struct*)_tmp161;if(_tmp16D->tag != 6)goto _LL55;
else{_tmp16E=_tmp16D->f2;}}_LL54: {unsigned int x=((unsigned int(*)(void*x))
unsafe_cast)(result);{int i=0;for(0;i < _get_dyneither_size(_tmp16E,sizeof(struct
_tuple1*));++ i){unsigned int _tmp18D;void*_tmp18E;struct _tuple1 _tmp18C=*((struct
_tuple1**)_tmp16E.curr)[i];_tmp18D=_tmp18C.f1;_tmp18E=_tmp18C.f2;{int x_ofs=(int)(
x + _tmp18D);env=Cyc_Marshal_rread_type_base(r,_tmp18E,env,fp,((void*(*)(int x))
unsafe_cast)(x_ofs));}}}goto _LL46;}_LL55: {struct Cyc_Typerep_TUnionField_struct*
_tmp16F=(struct Cyc_Typerep_TUnionField_struct*)_tmp161;if(_tmp16F->tag != 8)goto
_LL57;else{_tmp170=_tmp16F->f4;}}_LL56: {unsigned int x=((unsigned int(*)(void*x))
unsafe_cast)(result);{int i=0;for(0;i < _get_dyneither_size(_tmp170,sizeof(struct
_tuple1*));++ i){unsigned int _tmp190;void*_tmp191;struct _tuple1 _tmp18F=*((struct
_tuple1**)_tmp170.curr)[i];_tmp190=_tmp18F.f1;_tmp191=_tmp18F.f2;{int x_ofs=(int)(
x + _tmp190);env=Cyc_Marshal_rread_type_base(r,_tmp191,env,fp,((void*(*)(int x))
unsafe_cast)(x_ofs));}}}goto _LL46;}_LL57: {struct Cyc_Typerep_TUnion_struct*
_tmp171=(struct Cyc_Typerep_TUnion_struct*)_tmp161;if(_tmp171->tag != 7)goto _LL59;
else{_tmp172=_tmp171->f2;_tmp173=_tmp171->f3;}}_LL58: {int tagmem=(int)Cyc_Marshal_cycgetvarint(
fp);if(tagmem < 1024){int tag=tagmem;unsigned int*x=((unsigned int*(*)(void*x))
unsafe_cast)(result);*x=(unsigned int)tag;}else{int mem=tagmem - 1024;if(!Cyc_Marshal_table_member(
env,(unsigned int)mem)){int tag=(int)Cyc_Marshal_cycgetvarint(fp);int _tmp193;void*
_tmp194;struct _tuple4 _tmp192=Cyc_Typerep_get_unionbranch((unsigned int)tag,
_tmp173);_tmp193=_tmp192.f1;_tmp194=_tmp192.f2;{unsigned int _tmp44A;unsigned int
_tmp449;struct _dyneither_ptr _tmp448;char*_tmp447;unsigned int _tmp446;struct
_dyneither_ptr buf=(_tmp446=_tmp193 + Cyc_Typerep_size_type(_tmp194),((_tmp447=(
char*)_cycalloc_atomic(_check_times(sizeof(char),_tmp446 + 1)),((_tmp448=
_tag_dyneither(_tmp447,sizeof(char),_tmp446 + 1),((((_tmp449=_tmp446,((_tmp44B(&
_tmp44A,& _tmp449,& _tmp447),_tmp447[_tmp449]=(char)0)))),_tmp448)))))));
unsigned int bufptr=(unsigned int)((char*)_check_null(_untag_dyneither_ptr(buf,
sizeof(char),1 + 1)));env=Cyc_Marshal_table_insert(env,bufptr);{unsigned int*x=((
unsigned int*(*)(void*x))unsafe_cast)(result);*x=bufptr;{unsigned int*tagptr=((
unsigned int*(*)(unsigned int x))unsafe_cast)(bufptr);*tagptr=(unsigned int)tag;
env=Cyc_Marshal_rread_type_base(r,_tmp194,env,fp,((void*(*)(unsigned int x))
unsafe_cast)(bufptr + _tmp193));}}}}else{unsigned int*x=((unsigned int*(*)(void*x))
unsafe_cast)(result);*x=Cyc_Marshal_table_lookup(env,mem);}}goto _LL46;}_LL59: {
struct Cyc_Typerep_XTUnion_struct*_tmp174=(struct Cyc_Typerep_XTUnion_struct*)
_tmp161;if(_tmp174->tag != 9)goto _LL5B;else{_tmp175=_tmp174->f2;}}_LL5A: {int mem=(
int)Cyc_Marshal_cycgetvarint(fp);if(!Cyc_Marshal_table_member(env,(unsigned int)
mem)){char nonnull=(char)Cyc_Marshal_cycgetchar(fp);char*_tmp455;unsigned int
_tmp454;char*_tmp453;unsigned int _tmp452;unsigned int _tmp451;struct
_dyneither_ptr s=(_tmp455=((_tmp454=(unsigned int)100,((_tmp453=(char*)
_cycalloc_atomic(_check_times(sizeof(char),_tmp454 + 1)),((((_tmp452=_tmp454,((
_tmp450(& _tmp451,& _tmp452,& _tmp453),_tmp453[_tmp452]=(char)0)))),_tmp453)))))),
_tag_dyneither(_tmp455,sizeof(char),_get_zero_arr_size_char(_tmp455,(
unsigned int)100 + 1)));struct _dyneither_ptr xtname=_dyneither_ptr_plus(s,sizeof(
char),4);Cyc_Marshal_fscanntstring(fp,xtname);if((int)nonnull){int _tmp19A;void*
_tmp19B;struct _tuple4 _tmp199=Cyc_Typerep_get_xtunionbranch((struct _dyneither_ptr)
xtname,_tmp175);_tmp19A=_tmp199.f1;_tmp19B=_tmp199.f2;{unsigned int _tmp45A;
unsigned int _tmp459;struct _dyneither_ptr _tmp458;char*_tmp457;unsigned int _tmp456;
struct _dyneither_ptr buf=(_tmp456=_tmp19A + Cyc_Typerep_size_type(_tmp19B),((
_tmp457=(char*)_cycalloc_atomic(_check_times(sizeof(char),_tmp456 + 1)),((_tmp458=
_tag_dyneither(_tmp457,sizeof(char),_tmp456 + 1),((((_tmp459=_tmp456,((_tmp45B(&
_tmp45A,& _tmp459,& _tmp457),_tmp457[_tmp459]=(char)0)))),_tmp458)))))));
unsigned int bufptr=(unsigned int)((char*)_check_null(_untag_dyneither_ptr(buf,
sizeof(char),1 + 1)));env=Cyc_Marshal_table_insert(env,bufptr);{unsigned int*x=((
unsigned int*(*)(void*x))unsafe_cast)(result);*x=bufptr;{unsigned int*tagptr=((
unsigned int*(*)(unsigned int x))unsafe_cast)(bufptr);*tagptr=(unsigned int)((
char*)_check_null(_untag_dyneither_ptr(s,sizeof(char),1 + 1)));env=Cyc_Marshal_rread_type_base(
r,_tmp19B,env,fp,((void*(*)(unsigned int x))unsafe_cast)(bufptr + _tmp19A));}}}}
else{unsigned int*x=((unsigned int*(*)(void*x))unsafe_cast)(result);*x=(
unsigned int)((char*)_check_null(_untag_dyneither_ptr(s,sizeof(char),1 + 1)));env=
Cyc_Marshal_table_insert(env,*x);}}else{unsigned int*x=((unsigned int*(*)(void*x))
unsafe_cast)(result);*x=Cyc_Marshal_table_lookup(env,mem);}goto _LL46;}_LL5B: {
struct Cyc_Typerep_Union_struct*_tmp176=(struct Cyc_Typerep_Union_struct*)_tmp161;
if(_tmp176->tag != 10)goto _LL5D;else{_tmp177=_tmp176->f2;}}_LL5C:{struct Cyc_Typerep_Tuple_struct
_tmp474;unsigned int _tmp473;unsigned int _tmp472;struct _dyneither_ptr _tmp471;
struct _tuple1**_tmp470;unsigned int _tmp46F;struct Cyc_Typerep_Tuple_struct*
_tmp46E;env=Cyc_Marshal_rread_type_base(r,(void*)((_tmp46E=_cycalloc(sizeof(*
_tmp46E)),((_tmp46E[0]=((_tmp474.tag=6,((_tmp474.f1=(unsigned int)_tmp177,((
_tmp474.f2=((_tmp46F=(unsigned int)_tmp177,((_tmp470=(struct _tuple1**)_cycalloc(
_check_times(sizeof(struct _tuple1*),_tmp46F)),((_tmp471=_tag_dyneither(_tmp470,
sizeof(struct _tuple1*),_tmp46F),((((_tmp472=_tmp46F,_tmp467(& _tmp473,& _tmp472,&
_tmp470))),_tmp471)))))))),_tmp474)))))),_tmp46E)))),env,fp,result);}goto _LL46;
_LL5D: {struct Cyc_Typerep_Enum_struct*_tmp178=(struct Cyc_Typerep_Enum_struct*)
_tmp161;if(_tmp178->tag != 11)goto _LL46;else{_tmp179=_tmp178->f2;}}_LL5E: {struct
Cyc_Typerep_Int_struct _tmp477;struct Cyc_Typerep_Int_struct*_tmp476;env=Cyc_Marshal_rread_type_base(
r,(void*)((_tmp476=_cycalloc_atomic(sizeof(*_tmp476)),((_tmp476[0]=((_tmp477.tag=
0,((_tmp477.f1=0,((_tmp477.f2=(unsigned int)(_tmp179 << 3),_tmp477)))))),_tmp476)))),
env,fp,result);}_LL46:;}return env;}void*Cyc_Marshal_rread_type(struct
_RegionHandle*r,void*ts,struct Cyc___cycFILE*fp);static void _tmp47D(unsigned int*
_tmp47C,unsigned int*_tmp47B,char**_tmp479){for(*_tmp47C=0;*_tmp47C < *_tmp47B;(*
_tmp47C)++){(*_tmp479)[*_tmp47C]='\000';}}void*Cyc_Marshal_rread_type(struct
_RegionHandle*r,void*ts,struct Cyc___cycFILE*fp){struct _handler_cons _tmp1AF;
_push_handler(& _tmp1AF);{int _tmp1B1=0;if(setjmp(_tmp1AF.handler))_tmp1B1=1;if(!
_tmp1B1){{unsigned int _tmp47C;unsigned int _tmp47B;struct _dyneither_ptr _tmp47A;
char*_tmp479;unsigned int _tmp478;struct _dyneither_ptr buf=(_tmp478=Cyc_Typerep_size_type(
ts),((_tmp479=(char*)_region_malloc(r,_check_times(sizeof(char),_tmp478 + 1)),((
_tmp47A=_tag_dyneither(_tmp479,sizeof(char),_tmp478 + 1),((((_tmp47B=_tmp478,((
_tmp47D(& _tmp47C,& _tmp47B,& _tmp479),_tmp479[_tmp47B]=(char)0)))),_tmp47A)))))));
void*result=((void*(*)(char*x))unsafe_cast)((char*)_check_null(
_untag_dyneither_ptr(buf,sizeof(char),1 + 1)));Cyc_Marshal_rread_type_base(r,ts,
Cyc_Marshal_empty_addr_table(),fp,result);{void*_tmp1B2=result;_npop_handler(0);
return _tmp1B2;}};_pop_handler();}else{void*_tmp1B0=(void*)_exn_thrown;void*
_tmp1B8=_tmp1B0;struct _dyneither_ptr _tmp1BA;_LL65: {struct Cyc_Core_Failure_struct*
_tmp1B9=(struct Cyc_Core_Failure_struct*)_tmp1B8;if(_tmp1B9->tag != Cyc_Core_Failure)
goto _LL67;else{_tmp1BA=_tmp1B9->f1;}}_LL66:{const char*_tmp481;void*_tmp480[1];
struct Cyc_String_pa_struct _tmp47F;(_tmp47F.tag=0,((_tmp47F.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)_tmp1BA),((_tmp480[0]=& _tmp47F,Cyc_fprintf(
Cyc_stderr,((_tmp481="Core::Failure(%s)\n",_tag_dyneither(_tmp481,sizeof(char),
19))),_tag_dyneither(_tmp480,sizeof(void*),1)))))));}{struct Cyc_Core_Failure_struct
_tmp484;struct Cyc_Core_Failure_struct*_tmp483;(int)_throw((void*)((_tmp483=
_cycalloc(sizeof(*_tmp483)),((_tmp483[0]=((_tmp484.tag=Cyc_Core_Failure,((
_tmp484.f1=_tmp1BA,_tmp484)))),_tmp483)))));}_LL67:;_LL68:(void)_throw(_tmp1B8);
_LL64:;}}}void*Cyc_Marshal_read_type(void*ts,struct Cyc___cycFILE*fp);void*Cyc_Marshal_read_type(
void*ts,struct Cyc___cycFILE*fp){return Cyc_Marshal_rread_type(Cyc_Core_heap_region,
ts,fp);}static struct Cyc_Dict_Dict Cyc_Marshal_empty_addr_map();static struct Cyc_Dict_Dict
Cyc_Marshal_empty_addr_map(){return((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,
unsigned int k,unsigned int v))Cyc_Dict_insert)(((struct Cyc_Dict_Dict(*)(int(*cmp)(
unsigned int,unsigned int)))Cyc_Dict_empty)(Cyc_Marshal_uint_cmp),0,0);}struct
Cyc_Dict_Dict Cyc_Marshal_rcopy_type_base(struct _RegionHandle*r,void*rep,struct
Cyc_Dict_Dict env,void*val,void*result);static void _tmp48F(unsigned int*_tmp48E,
unsigned int*_tmp48D,char**_tmp48B){for(*_tmp48E=0;*_tmp48E < *_tmp48D;(*_tmp48E)
++){(*_tmp48B)[*_tmp48E]='\000';}}static void _tmp496(unsigned int*_tmp495,
unsigned int*_tmp494,char**_tmp492){for(*_tmp495=0;*_tmp495 < *_tmp494;(*_tmp495)
++){(*_tmp492)[*_tmp495]='\000';}}static void _tmp49D(unsigned int*_tmp49C,
unsigned int*_tmp49B,char**_tmp499){for(*_tmp49C=0;*_tmp49C < *_tmp49B;(*_tmp49C)
++){(*_tmp499)[*_tmp49C]='\000';}}static void _tmp4A3(unsigned int*_tmp4A2,
unsigned int*_tmp4A1,char**_tmp49F){for(*_tmp4A2=0;*_tmp4A2 < *_tmp4A1;(*_tmp4A2)
++){(*_tmp49F)[*_tmp4A2]='\000';}}static void _tmp4AF(unsigned int*_tmp4AE,
unsigned int*_tmp4AD,struct _tuple1***_tmp4AB){for(*_tmp4AE=0;*_tmp4AE < *_tmp4AD;(*
_tmp4AE)++){struct Cyc_Typerep_Int_struct*_tmp4A9;struct Cyc_Typerep_Int_struct
_tmp4A8;struct _tuple1*_tmp4A7;(*_tmp4AB)[*_tmp4AE]=((_tmp4A7=_cycalloc(sizeof(*
_tmp4A7)),((_tmp4A7->f1=*_tmp4AE,((_tmp4A7->f2=(void*)((_tmp4A9=_cycalloc_atomic(
sizeof(*_tmp4A9)),((_tmp4A9[0]=((_tmp4A8.tag=0,((_tmp4A8.f1=0,((_tmp4A8.f2=8,
_tmp4A8)))))),_tmp4A9)))),_tmp4A7))))));}}struct Cyc_Dict_Dict Cyc_Marshal_rcopy_type_base(
struct _RegionHandle*r,void*rep,struct Cyc_Dict_Dict env,void*val,void*result){{
void*_tmp1C0=rep;unsigned int _tmp1C2;unsigned int _tmp1C4;unsigned int _tmp1C6;
unsigned int _tmp1C8;unsigned int _tmp1CC;void*_tmp1CD;void*_tmp1CF;struct
_dyneither_ptr _tmp1D1;struct _dyneither_ptr _tmp1D3;struct _dyneither_ptr _tmp1D5;
struct _dyneither_ptr _tmp1D7;struct _dyneither_ptr _tmp1D8;struct _dyneither_ptr
_tmp1DA;int _tmp1DC;int _tmp1DE;_LL6A: {struct Cyc_Typerep_Int_struct*_tmp1C1=(
struct Cyc_Typerep_Int_struct*)_tmp1C0;if(_tmp1C1->tag != 0)goto _LL6C;else{_tmp1C2=
_tmp1C1->f2;if(_tmp1C2 != 8)goto _LL6C;}}_LL6B: {char*val0=((char*(*)(void*x))
unsafe_cast)(val);char*result0=((char*(*)(void*x))unsafe_cast)(result);{char
_tmp489;char _tmp488;struct _dyneither_ptr _tmp487;char*_tmp486;(_tmp487=((_tmp486=
result0,_tag_dyneither(_tmp486,sizeof(char),_get_zero_arr_size_char(_tmp486,1)))),((
_tmp488=*((char*)_check_dyneither_subscript(_tmp487,sizeof(char),0)),((_tmp489=*
val0,((_get_dyneither_size(_tmp487,sizeof(char))== 1  && (_tmp488 == '\000'  && 
_tmp489 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp487.curr)=_tmp489)))))));}
goto _LL69;}_LL6C: {struct Cyc_Typerep_Int_struct*_tmp1C3=(struct Cyc_Typerep_Int_struct*)
_tmp1C0;if(_tmp1C3->tag != 0)goto _LL6E;else{_tmp1C4=_tmp1C3->f2;if(_tmp1C4 != 16)
goto _LL6E;}}_LL6D: {short*val0=((short*(*)(void*x))unsafe_cast)(val);short*
result0=((short*(*)(void*x))unsafe_cast)(result);*result0=*val0;goto _LL69;}_LL6E: {
struct Cyc_Typerep_Int_struct*_tmp1C5=(struct Cyc_Typerep_Int_struct*)_tmp1C0;if(
_tmp1C5->tag != 0)goto _LL70;else{_tmp1C6=_tmp1C5->f2;if(_tmp1C6 != 32)goto _LL70;}}
_LL6F: {int*val0=((int*(*)(void*x))unsafe_cast)(val);int*result0=((int*(*)(void*
x))unsafe_cast)(result);*result0=*val0;goto _LL69;}_LL70: {struct Cyc_Typerep_Int_struct*
_tmp1C7=(struct Cyc_Typerep_Int_struct*)_tmp1C0;if(_tmp1C7->tag != 0)goto _LL72;
else{_tmp1C8=_tmp1C7->f2;if(_tmp1C8 != 64)goto _LL72;}}_LL71: {long long*val0=((
long long*(*)(void*x))unsafe_cast)(val);long long*result0=((long long*(*)(void*x))
unsafe_cast)(result);*result0=*val0;goto _LL69;}_LL72: {struct Cyc_Typerep_Float_struct*
_tmp1C9=(struct Cyc_Typerep_Float_struct*)_tmp1C0;if(_tmp1C9->tag != 1)goto _LL74;}
_LL73: {float*val0=((float*(*)(void*x))unsafe_cast)(val);float*result0=((float*(*)(
void*x))unsafe_cast)(result);*result0=*val0;goto _LL69;}_LL74: {struct Cyc_Typerep_Double_struct*
_tmp1CA=(struct Cyc_Typerep_Double_struct*)_tmp1C0;if(_tmp1CA->tag != 2)goto _LL76;}
_LL75: {double*val0=((double*(*)(void*x))unsafe_cast)(val);double*result0=((
double*(*)(void*x))unsafe_cast)(result);*result0=*val0;goto _LL69;}_LL76: {struct
Cyc_Typerep_ThinPtr_struct*_tmp1CB=(struct Cyc_Typerep_ThinPtr_struct*)_tmp1C0;
if(_tmp1CB->tag != 3)goto _LL78;else{_tmp1CC=_tmp1CB->f1;_tmp1CD=(void*)_tmp1CB->f2;}}
_LL77: {unsigned int*val0=((unsigned int*(*)(void*x))unsafe_cast)(val);
unsigned int*result0=((unsigned int*(*)(void*x))unsafe_cast)(result);int addr=(
int)*val0;if(((int(*)(struct Cyc_Dict_Dict d,unsigned int k))Cyc_Dict_member)(env,(
unsigned int)addr))*result0=((unsigned int(*)(struct Cyc_Dict_Dict d,unsigned int k))
Cyc_Dict_lookup)(env,(unsigned int)addr);else{int typ_szb=(int)Cyc_Typerep_size_type(
_tmp1CD);unsigned int _tmp48E;unsigned int _tmp48D;struct _dyneither_ptr _tmp48C;
char*_tmp48B;unsigned int _tmp48A;struct _dyneither_ptr buf=(_tmp48A=_tmp1CC * 
typ_szb,((_tmp48B=(char*)_region_malloc(r,_check_times(sizeof(char),_tmp48A + 1)),((
_tmp48C=_tag_dyneither(_tmp48B,sizeof(char),_tmp48A + 1),((((_tmp48D=_tmp48A,((
_tmp48F(& _tmp48E,& _tmp48D,& _tmp48B),_tmp48B[_tmp48D]=(char)0)))),_tmp48C)))))));
unsigned int bufptr=(unsigned int)((char*)_check_null(_untag_dyneither_ptr(buf,
sizeof(char),1 + 1)));*result0=bufptr;env=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict
d,unsigned int k,unsigned int v))Cyc_Dict_insert)(env,(unsigned int)addr,bufptr);{
int x=addr;int y=(int)bufptr;int i=0;for(0;i < _tmp1CC;++ i){env=Cyc_Marshal_rcopy_type_base(
r,_tmp1CD,env,((void*(*)(int x))unsafe_cast)(x),((void*(*)(int x))unsafe_cast)(y));
x +=typ_szb;y +=typ_szb;}}}goto _LL69;}_LL78: {struct Cyc_Typerep_FatPtr_struct*
_tmp1CE=(struct Cyc_Typerep_FatPtr_struct*)_tmp1C0;if(_tmp1CE->tag != 4)goto _LL7A;
else{_tmp1CF=(void*)_tmp1CE->f1;}}_LL79: {struct _tuple9*val0=((struct _tuple9*(*)(
void*x))unsafe_cast)(val);struct _tuple9*result0=((struct _tuple9*(*)(void*x))
unsafe_cast)(result);unsigned int _tmp1E8;unsigned int _tmp1E9;unsigned int _tmp1EA;
struct _tuple9 _tmp1E7=*val0;_tmp1E8=_tmp1E7.f1;_tmp1E9=_tmp1E7.f2;_tmp1EA=_tmp1E7.f3;{
int typ_szb=(int)Cyc_Typerep_size_type(_tmp1CF);int size=(int)((_tmp1EA - _tmp1E8)/ 
typ_szb);int pos=(int)((_tmp1E9 - _tmp1E8)/ typ_szb);if(((int(*)(struct Cyc_Dict_Dict
d,unsigned int k))Cyc_Dict_member)(env,_tmp1E8)){unsigned int _tmp1EB=((
unsigned int(*)(struct Cyc_Dict_Dict d,unsigned int k))Cyc_Dict_lookup)(env,_tmp1E8);
struct _tuple9 _tmp490;*result0=((_tmp490.f1=_tmp1EB,((_tmp490.f2=_tmp1EB + pos * 
typ_szb,((_tmp490.f3=_tmp1EB + size * typ_szb,_tmp490))))));}else{unsigned int
_tmp495;unsigned int _tmp494;struct _dyneither_ptr _tmp493;char*_tmp492;
unsigned int _tmp491;struct _dyneither_ptr buf=(_tmp491=_tmp1EA - _tmp1E8,((_tmp492=(
char*)_region_malloc(r,_check_times(sizeof(char),_tmp491 + 1)),((_tmp493=
_tag_dyneither(_tmp492,sizeof(char),_tmp491 + 1),((((_tmp494=_tmp491,((_tmp496(&
_tmp495,& _tmp494,& _tmp492),_tmp492[_tmp494]=(char)0)))),_tmp493)))))));
unsigned int bufptr=(unsigned int)((char*)_check_null(_untag_dyneither_ptr(buf,
sizeof(char),1 + 1)));{struct _tuple9 _tmp497;*result0=((_tmp497.f1=bufptr,((
_tmp497.f2=bufptr + pos * typ_szb,((_tmp497.f3=bufptr + size * typ_szb,_tmp497))))));}
env=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,unsigned int k,unsigned int v))
Cyc_Dict_insert)(env,_tmp1E8,bufptr);{int y=(int)_tmp1E8;int z=(int)bufptr;int i=0;
for(0;i < size;++ i){env=Cyc_Marshal_rcopy_type_base(r,_tmp1CF,env,((void*(*)(int x))
unsafe_cast)(y),((void*(*)(int x))unsafe_cast)(z));y +=typ_szb;z +=typ_szb;}}}goto
_LL69;}}_LL7A: {struct Cyc_Typerep_Struct_struct*_tmp1D0=(struct Cyc_Typerep_Struct_struct*)
_tmp1C0;if(_tmp1D0->tag != 5)goto _LL7C;else{_tmp1D1=_tmp1D0->f3;}}_LL7B: {int x=((
int(*)(void*x))unsafe_cast)(val);int y=((int(*)(void*x))unsafe_cast)(result);{int
i=0;for(0;i < _get_dyneither_size(_tmp1D1,sizeof(struct _tuple0*));++ i){
unsigned int _tmp1F3;void*_tmp1F4;struct _tuple0 _tmp1F2=*((struct _tuple0**)_tmp1D1.curr)[
i];_tmp1F3=_tmp1F2.f1;_tmp1F4=_tmp1F2.f3;{int x_ofs=(int)(x + _tmp1F3);int y_ofs=(
int)(y + _tmp1F3);env=Cyc_Marshal_rcopy_type_base(r,_tmp1F4,env,((void*(*)(int x))
unsafe_cast)(x_ofs),((void*(*)(int x))unsafe_cast)(y_ofs));}}}goto _LL69;}_LL7C: {
struct Cyc_Typerep_Tuple_struct*_tmp1D2=(struct Cyc_Typerep_Tuple_struct*)_tmp1C0;
if(_tmp1D2->tag != 6)goto _LL7E;else{_tmp1D3=_tmp1D2->f2;}}_LL7D: {int x=((int(*)(
void*x))unsafe_cast)(val);int y=((int(*)(void*x))unsafe_cast)(result);{int i=0;
for(0;i < _get_dyneither_size(_tmp1D3,sizeof(struct _tuple1*));++ i){unsigned int
_tmp1F6;void*_tmp1F7;struct _tuple1 _tmp1F5=*((struct _tuple1**)_tmp1D3.curr)[i];
_tmp1F6=_tmp1F5.f1;_tmp1F7=_tmp1F5.f2;{int x_ofs=(int)(x + _tmp1F6);int y_ofs=(int)(
y + _tmp1F6);env=Cyc_Marshal_rcopy_type_base(r,_tmp1F7,env,((void*(*)(int x))
unsafe_cast)(x_ofs),((void*(*)(int x))unsafe_cast)(y_ofs));}}}goto _LL69;}_LL7E: {
struct Cyc_Typerep_TUnionField_struct*_tmp1D4=(struct Cyc_Typerep_TUnionField_struct*)
_tmp1C0;if(_tmp1D4->tag != 8)goto _LL80;else{_tmp1D5=_tmp1D4->f4;}}_LL7F: {int x=((
int(*)(void*x))unsafe_cast)(val);int y=((int(*)(void*x))unsafe_cast)(result);{int
i=0;for(0;i < _get_dyneither_size(_tmp1D5,sizeof(struct _tuple1*));++ i){
unsigned int _tmp1F9;void*_tmp1FA;struct _tuple1 _tmp1F8=*((struct _tuple1**)_tmp1D5.curr)[
i];_tmp1F9=_tmp1F8.f1;_tmp1FA=_tmp1F8.f2;{int x_ofs=(int)(x + _tmp1F9);int y_ofs=(
int)(y + _tmp1F9);env=Cyc_Marshal_rcopy_type_base(r,_tmp1FA,env,((void*(*)(int x))
unsafe_cast)(x_ofs),((void*(*)(int x))unsafe_cast)(y_ofs));}}}goto _LL69;}_LL80: {
struct Cyc_Typerep_TUnion_struct*_tmp1D6=(struct Cyc_Typerep_TUnion_struct*)
_tmp1C0;if(_tmp1D6->tag != 7)goto _LL82;else{_tmp1D7=_tmp1D6->f2;_tmp1D8=_tmp1D6->f3;}}
_LL81: {unsigned int*val0=((unsigned int*(*)(void*x))unsafe_cast)(val);
unsigned int*result0=((unsigned int*(*)(void*x))unsafe_cast)(result);if(*val0 < 
1024)*result0=*val0;else{unsigned int addr=*val0;if(((int(*)(struct Cyc_Dict_Dict d,
unsigned int k))Cyc_Dict_member)(env,addr))*result0=((unsigned int(*)(struct Cyc_Dict_Dict
d,unsigned int k))Cyc_Dict_lookup)(env,addr);else{unsigned int*tufp=((
unsigned int*(*)(unsigned int x))unsafe_cast)(*val0);unsigned int tag=*tufp;int
_tmp1FC;void*_tmp1FD;struct _tuple4 _tmp1FB=Cyc_Typerep_get_unionbranch(tag,
_tmp1D8);_tmp1FC=_tmp1FB.f1;_tmp1FD=_tmp1FB.f2;{unsigned int _tmp49C;unsigned int
_tmp49B;struct _dyneither_ptr _tmp49A;char*_tmp499;unsigned int _tmp498;struct
_dyneither_ptr buf=(_tmp498=_tmp1FC + Cyc_Typerep_size_type(_tmp1FD),((_tmp499=(
char*)_region_malloc(r,_check_times(sizeof(char),_tmp498 + 1)),((_tmp49A=
_tag_dyneither(_tmp499,sizeof(char),_tmp498 + 1),((((_tmp49B=_tmp498,((_tmp49D(&
_tmp49C,& _tmp49B,& _tmp499),_tmp499[_tmp49B]=(char)0)))),_tmp49A)))))));
unsigned int bufptr=(unsigned int)((char*)_check_null(_untag_dyneither_ptr(buf,
sizeof(char),1 + 1)));env=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,
unsigned int k,unsigned int v))Cyc_Dict_insert)(env,addr,bufptr);*result0=bufptr;{
unsigned int*newtagp=((unsigned int*(*)(unsigned int x))unsafe_cast)(bufptr);*
newtagp=tag;env=Cyc_Marshal_rcopy_type_base(r,_tmp1FD,env,((void*(*)(
unsigned int x))unsafe_cast)(addr + _tmp1FC),((void*(*)(unsigned int x))unsafe_cast)(
bufptr + _tmp1FC));}}}}goto _LL69;}_LL82: {struct Cyc_Typerep_XTUnion_struct*
_tmp1D9=(struct Cyc_Typerep_XTUnion_struct*)_tmp1C0;if(_tmp1D9->tag != 9)goto _LL84;
else{_tmp1DA=_tmp1D9->f2;}}_LL83: {unsigned int*val0=((unsigned int*(*)(void*x))
unsafe_cast)(val);unsigned int*result0=((unsigned int*(*)(void*x))unsafe_cast)(
result);unsigned int addr=*val0;if(((int(*)(struct Cyc_Dict_Dict d,unsigned int k))
Cyc_Dict_member)(env,addr))*result0=((unsigned int(*)(struct Cyc_Dict_Dict d,
unsigned int k))Cyc_Dict_lookup)(env,addr);else{unsigned int*xtstructp=((
unsigned int*(*)(unsigned int x))unsafe_cast)(*val0);if(*xtstructp != 0){
unsigned int xtnamep=*xtstructp;char*xtnamec=((char*(*)(unsigned int x))
unsafe_cast)(xtnamep + 4);struct _dyneither_ptr xtname=(struct _dyneither_ptr)
Cstring_to_string(xtnamec);int _tmp203;void*_tmp204;struct _tuple4 _tmp202=Cyc_Typerep_get_xtunionbranch(
xtname,_tmp1DA);_tmp203=_tmp202.f1;_tmp204=_tmp202.f2;{unsigned int _tmp4A2;
unsigned int _tmp4A1;struct _dyneither_ptr _tmp4A0;char*_tmp49F;unsigned int _tmp49E;
struct _dyneither_ptr buf=(_tmp49E=_tmp203 + Cyc_Typerep_size_type(_tmp204),((
_tmp49F=(char*)_region_malloc(r,_check_times(sizeof(char),_tmp49E + 1)),((_tmp4A0=
_tag_dyneither(_tmp49F,sizeof(char),_tmp49E + 1),((((_tmp4A1=_tmp49E,((_tmp4A3(&
_tmp4A2,& _tmp4A1,& _tmp49F),_tmp49F[_tmp4A1]=(char)0)))),_tmp4A0)))))));
unsigned int bufptr=(unsigned int)((char*)_check_null(_untag_dyneither_ptr(buf,
sizeof(char),1 + 1)));*result0=bufptr;{unsigned int*tagptr=((unsigned int*(*)(
unsigned int x))unsafe_cast)(bufptr);*tagptr=xtnamep;{void*xoff=((void*(*)(
unsigned int x))unsafe_cast)(*val0 + _tmp203);env=Cyc_Marshal_rcopy_type_base(r,
_tmp204,env,xoff,((void*(*)(unsigned int x))unsafe_cast)(bufptr + _tmp203));env=((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,unsigned int k,unsigned int v))Cyc_Dict_insert)(
env,addr,bufptr);}}}}else{*result0=*val0;}}goto _LL69;}_LL84: {struct Cyc_Typerep_Union_struct*
_tmp1DB=(struct Cyc_Typerep_Union_struct*)_tmp1C0;if(_tmp1DB->tag != 10)goto _LL86;
else{_tmp1DC=_tmp1DB->f2;}}_LL85:{struct Cyc_Typerep_Tuple_struct _tmp4BC;
unsigned int _tmp4BB;unsigned int _tmp4BA;struct _dyneither_ptr _tmp4B9;struct
_tuple1**_tmp4B8;unsigned int _tmp4B7;struct Cyc_Typerep_Tuple_struct*_tmp4B6;env=
Cyc_Marshal_rcopy_type_base(r,(void*)((_tmp4B6=_cycalloc(sizeof(*_tmp4B6)),((
_tmp4B6[0]=((_tmp4BC.tag=6,((_tmp4BC.f1=(unsigned int)_tmp1DC,((_tmp4BC.f2=((
_tmp4B7=(unsigned int)_tmp1DC,((_tmp4B8=(struct _tuple1**)_cycalloc(_check_times(
sizeof(struct _tuple1*),_tmp4B7)),((_tmp4B9=_tag_dyneither(_tmp4B8,sizeof(struct
_tuple1*),_tmp4B7),((((_tmp4BA=_tmp4B7,_tmp4AF(& _tmp4BB,& _tmp4BA,& _tmp4B8))),
_tmp4B9)))))))),_tmp4BC)))))),_tmp4B6)))),env,val,result);}goto _LL69;_LL86: {
struct Cyc_Typerep_Enum_struct*_tmp1DD=(struct Cyc_Typerep_Enum_struct*)_tmp1C0;
if(_tmp1DD->tag != 11)goto _LL88;else{_tmp1DE=_tmp1DD->f2;}}_LL87:{struct Cyc_Typerep_Int_struct
_tmp4BF;struct Cyc_Typerep_Int_struct*_tmp4BE;env=Cyc_Marshal_rcopy_type_base(r,(
void*)((_tmp4BE=_cycalloc_atomic(sizeof(*_tmp4BE)),((_tmp4BE[0]=((_tmp4BF.tag=0,((
_tmp4BF.f1=0,((_tmp4BF.f2=(unsigned int)(_tmp1DE << 3),_tmp4BF)))))),_tmp4BE)))),
env,val,result);}goto _LL69;_LL88:;_LL89: {struct Cyc_Core_Failure_struct _tmp4C5;
const char*_tmp4C4;struct Cyc_Core_Failure_struct*_tmp4C3;(int)_throw((void*)((
_tmp4C3=_cycalloc(sizeof(*_tmp4C3)),((_tmp4C3[0]=((_tmp4C5.tag=Cyc_Core_Failure,((
_tmp4C5.f1=((_tmp4C4="Unhandled case in copy_type_base",_tag_dyneither(_tmp4C4,
sizeof(char),33))),_tmp4C5)))),_tmp4C3)))));}_LL69:;}return env;}void*Cyc_Marshal_rcopy_type(
struct _RegionHandle*r,void*ts,void*val);static void _tmp4CB(unsigned int*_tmp4CA,
unsigned int*_tmp4C9,char**_tmp4C7){for(*_tmp4CA=0;*_tmp4CA < *_tmp4C9;(*_tmp4CA)
++){(*_tmp4C7)[*_tmp4CA]='\000';}}void*Cyc_Marshal_rcopy_type(struct
_RegionHandle*r,void*ts,void*val){struct _handler_cons _tmp217;_push_handler(&
_tmp217);{int _tmp219=0;if(setjmp(_tmp217.handler))_tmp219=1;if(!_tmp219){{
unsigned int _tmp4CA;unsigned int _tmp4C9;struct _dyneither_ptr _tmp4C8;char*_tmp4C7;
unsigned int _tmp4C6;struct _dyneither_ptr buf=(_tmp4C6=Cyc_Typerep_size_type(ts),((
_tmp4C7=(char*)_region_malloc(r,_check_times(sizeof(char),_tmp4C6 + 1)),((_tmp4C8=
_tag_dyneither(_tmp4C7,sizeof(char),_tmp4C6 + 1),((((_tmp4C9=_tmp4C6,((_tmp4CB(&
_tmp4CA,& _tmp4C9,& _tmp4C7),_tmp4C7[_tmp4C9]=(char)0)))),_tmp4C8)))))));void*
result=((void*(*)(char*x))unsafe_cast)((char*)_check_null(_untag_dyneither_ptr(
buf,sizeof(char),1 + 1)));Cyc_Marshal_rcopy_type_base(r,ts,Cyc_Marshal_empty_addr_map(),
val,result);{void*_tmp21A=result;_npop_handler(0);return _tmp21A;}};_pop_handler();}
else{void*_tmp218=(void*)_exn_thrown;void*_tmp220=_tmp218;struct _dyneither_ptr
_tmp222;_LL8B: {struct Cyc_Core_Failure_struct*_tmp221=(struct Cyc_Core_Failure_struct*)
_tmp220;if(_tmp221->tag != Cyc_Core_Failure)goto _LL8D;else{_tmp222=_tmp221->f1;}}
_LL8C:{const char*_tmp4CF;void*_tmp4CE[1];struct Cyc_String_pa_struct _tmp4CD;(
_tmp4CD.tag=0,((_tmp4CD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp222),((
_tmp4CE[0]=& _tmp4CD,Cyc_fprintf(Cyc_stderr,((_tmp4CF="Core::Failure(%s)\n",
_tag_dyneither(_tmp4CF,sizeof(char),19))),_tag_dyneither(_tmp4CE,sizeof(void*),1)))))));}{
struct Cyc_Core_Failure_struct _tmp4D2;struct Cyc_Core_Failure_struct*_tmp4D1;(int)
_throw((void*)((_tmp4D1=_cycalloc(sizeof(*_tmp4D1)),((_tmp4D1[0]=((_tmp4D2.tag=
Cyc_Core_Failure,((_tmp4D2.f1=_tmp222,_tmp4D2)))),_tmp4D1)))));}_LL8D:;_LL8E:(
void)_throw(_tmp220);_LL8A:;}}}void*Cyc_Marshal_copy_type(void*ts,void*val);void*
Cyc_Marshal_copy_type(void*ts,void*val){return Cyc_Marshal_rcopy_type(Cyc_Core_heap_region,
ts,val);}enum Cyc_Marshal_Order{Cyc_Marshal_Gt_ord  = 0,Cyc_Marshal_Eq_ord  = 1,
Cyc_Marshal_Lt_ord  = 2};enum Cyc_Marshal_Order Cyc_Marshal_order_lex(enum Cyc_Marshal_Order
ord1,enum Cyc_Marshal_Order ord2);enum Cyc_Marshal_Order Cyc_Marshal_order_lex(
enum Cyc_Marshal_Order ord1,enum Cyc_Marshal_Order ord2){if(ord1 != Cyc_Marshal_Eq_ord)
return ord1;else{return ord2;}}struct _tuple10{unsigned int f1;unsigned int f2;};
static int Cyc_Marshal_addrp_cmp(struct _tuple10*addrp1,struct _tuple10*addrp2);
static int Cyc_Marshal_addrp_cmp(struct _tuple10*addrp1,struct _tuple10*addrp2){if((*
addrp1).f1 < (*addrp2).f1)return 1;else{if((*addrp1).f1 == (*addrp2).f1)return Cyc_Marshal_uint_cmp((*
addrp1).f2,(*addrp2).f2);else{return - 1;}}}struct Cyc_Dict_Dict Cyc_Marshal_empty_eq_dict();
struct Cyc_Dict_Dict Cyc_Marshal_empty_eq_dict(){struct _tuple10*_tmp4D3;return((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple10*k,enum Cyc_Marshal_Order
v))Cyc_Dict_insert)(((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _tuple10*,struct
_tuple10*)))Cyc_Dict_empty)(Cyc_Marshal_addrp_cmp),((_tmp4D3=_cycalloc_atomic(
sizeof(*_tmp4D3)),((_tmp4D3->f1=0,((_tmp4D3->f2=0,_tmp4D3)))))),Cyc_Marshal_Eq_ord);}
struct _tuple11{enum Cyc_Marshal_Order f1;struct Cyc_Dict_Dict f2;};struct _tuple11
Cyc_Marshal_return_update(struct _tuple10*pair,struct _tuple11 ordenv);struct
_tuple11 Cyc_Marshal_return_update(struct _tuple10*pair,struct _tuple11 ordenv){
enum Cyc_Marshal_Order _tmp22A;struct Cyc_Dict_Dict _tmp22B;struct _tuple11 _tmp229=
ordenv;_tmp22A=_tmp229.f1;_tmp22B=_tmp229.f2;_tmp22B=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,struct _tuple10*k,enum Cyc_Marshal_Order v))Cyc_Dict_insert)(
_tmp22B,pair,_tmp22A);{struct _tuple11 _tmp4D4;return(_tmp4D4.f1=_tmp22A,((_tmp4D4.f2=
_tmp22B,_tmp4D4)));}}struct _tuple11 Cyc_Marshal_leq_eq_type(void*rep,struct Cyc_Dict_Dict
env,void*x,void*y);static void _tmp50A(unsigned int*_tmp509,unsigned int*_tmp508,
struct _tuple1***_tmp506){for(*_tmp509=0;*_tmp509 < *_tmp508;(*_tmp509)++){struct
Cyc_Typerep_Int_struct*_tmp504;struct Cyc_Typerep_Int_struct _tmp503;struct _tuple1*
_tmp502;(*_tmp506)[*_tmp509]=((_tmp502=_cycalloc(sizeof(*_tmp502)),((_tmp502->f1=*
_tmp509,((_tmp502->f2=(void*)((_tmp504=_cycalloc_atomic(sizeof(*_tmp504)),((
_tmp504[0]=((_tmp503.tag=0,((_tmp503.f1=0,((_tmp503.f2=8,_tmp503)))))),_tmp504)))),
_tmp502))))));}}struct _tuple11 Cyc_Marshal_leq_eq_type(void*rep,struct Cyc_Dict_Dict
env,void*x,void*y){if(x == y){struct _tuple11 _tmp4D5;return(_tmp4D5.f1=Cyc_Marshal_Eq_ord,((
_tmp4D5.f2=env,_tmp4D5)));}{void*_tmp22E=rep;int _tmp230;unsigned int _tmp231;int
_tmp233;unsigned int _tmp234;int _tmp236;unsigned int _tmp237;int _tmp239;
unsigned int _tmp23A;int _tmp23C;unsigned int _tmp23D;int _tmp23F;unsigned int
_tmp240;int _tmp242;unsigned int _tmp243;int _tmp245;unsigned int _tmp246;
unsigned int _tmp24A;void*_tmp24B;void*_tmp24D;struct _dyneither_ptr _tmp24F;struct
_dyneither_ptr _tmp251;struct _dyneither_ptr _tmp253;struct _dyneither_ptr _tmp255;
struct _dyneither_ptr _tmp256;struct _dyneither_ptr _tmp258;int _tmp25A;struct
_dyneither_ptr _tmp25B;int _tmp25D;_LL90: {struct Cyc_Typerep_Int_struct*_tmp22F=(
struct Cyc_Typerep_Int_struct*)_tmp22E;if(_tmp22F->tag != 0)goto _LL92;else{_tmp230=
_tmp22F->f1;if(_tmp230 != 0)goto _LL92;_tmp231=_tmp22F->f2;if(_tmp231 != 8)goto
_LL92;}}_LL91: {unsigned char*x0=((unsigned char*(*)(void*x))unsafe_cast)(x);
unsigned char*y0=((unsigned char*(*)(void*x))unsafe_cast)(y);struct _tuple11
_tmp4D6;return(_tmp4D6.f1=*x0 == *y0?Cyc_Marshal_Eq_ord:(*x0 <= *y0?Cyc_Marshal_Lt_ord:
Cyc_Marshal_Gt_ord),((_tmp4D6.f2=env,_tmp4D6)));}_LL92: {struct Cyc_Typerep_Int_struct*
_tmp232=(struct Cyc_Typerep_Int_struct*)_tmp22E;if(_tmp232->tag != 0)goto _LL94;
else{_tmp233=_tmp232->f1;if(_tmp233 != 1)goto _LL94;_tmp234=_tmp232->f2;if(_tmp234
!= 8)goto _LL94;}}_LL93: {signed char*x0=((signed char*(*)(void*x))unsafe_cast)(x);
signed char*y0=((signed char*(*)(void*x))unsafe_cast)(y);struct _tuple11 _tmp4D7;
return(_tmp4D7.f1=*x0 == *y0?Cyc_Marshal_Eq_ord:(*x0 <= *y0?Cyc_Marshal_Lt_ord: Cyc_Marshal_Gt_ord),((
_tmp4D7.f2=env,_tmp4D7)));}_LL94: {struct Cyc_Typerep_Int_struct*_tmp235=(struct
Cyc_Typerep_Int_struct*)_tmp22E;if(_tmp235->tag != 0)goto _LL96;else{_tmp236=
_tmp235->f1;if(_tmp236 != 1)goto _LL96;_tmp237=_tmp235->f2;if(_tmp237 != 16)goto
_LL96;}}_LL95: {short*x0=((short*(*)(void*x))unsafe_cast)(x);short*y0=((short*(*)(
void*x))unsafe_cast)(y);struct _tuple11 _tmp4D8;return(_tmp4D8.f1=*x0 == *y0?Cyc_Marshal_Eq_ord:(*
x0 <= *y0?Cyc_Marshal_Lt_ord: Cyc_Marshal_Gt_ord),((_tmp4D8.f2=env,_tmp4D8)));}
_LL96: {struct Cyc_Typerep_Int_struct*_tmp238=(struct Cyc_Typerep_Int_struct*)
_tmp22E;if(_tmp238->tag != 0)goto _LL98;else{_tmp239=_tmp238->f1;if(_tmp239 != 0)
goto _LL98;_tmp23A=_tmp238->f2;if(_tmp23A != 16)goto _LL98;}}_LL97: {unsigned short*
x0=((unsigned short*(*)(void*x))unsafe_cast)(x);unsigned short*y0=((
unsigned short*(*)(void*x))unsafe_cast)(y);struct _tuple11 _tmp4D9;return(_tmp4D9.f1=*
x0 == *y0?Cyc_Marshal_Eq_ord:(*x0 <= *y0?Cyc_Marshal_Lt_ord: Cyc_Marshal_Gt_ord),((
_tmp4D9.f2=env,_tmp4D9)));}_LL98: {struct Cyc_Typerep_Int_struct*_tmp23B=(struct
Cyc_Typerep_Int_struct*)_tmp22E;if(_tmp23B->tag != 0)goto _LL9A;else{_tmp23C=
_tmp23B->f1;if(_tmp23C != 1)goto _LL9A;_tmp23D=_tmp23B->f2;if(_tmp23D != 32)goto
_LL9A;}}_LL99: {int*x0=((int*(*)(void*x))unsafe_cast)(x);int*y0=((int*(*)(void*x))
unsafe_cast)(y);struct _tuple11 _tmp4DA;return(_tmp4DA.f1=*x0 == *y0?Cyc_Marshal_Eq_ord:(*
x0 <= *y0?Cyc_Marshal_Lt_ord: Cyc_Marshal_Gt_ord),((_tmp4DA.f2=env,_tmp4DA)));}
_LL9A: {struct Cyc_Typerep_Int_struct*_tmp23E=(struct Cyc_Typerep_Int_struct*)
_tmp22E;if(_tmp23E->tag != 0)goto _LL9C;else{_tmp23F=_tmp23E->f1;if(_tmp23F != 0)
goto _LL9C;_tmp240=_tmp23E->f2;if(_tmp240 != 32)goto _LL9C;}}_LL9B: {unsigned int*
x0=((unsigned int*(*)(void*x))unsafe_cast)(x);unsigned int*y0=((unsigned int*(*)(
void*x))unsafe_cast)(y);struct _tuple11 _tmp4DB;return(_tmp4DB.f1=*x0 == *y0?Cyc_Marshal_Eq_ord:(*
x0 <= *y0?Cyc_Marshal_Lt_ord: Cyc_Marshal_Gt_ord),((_tmp4DB.f2=env,_tmp4DB)));}
_LL9C: {struct Cyc_Typerep_Int_struct*_tmp241=(struct Cyc_Typerep_Int_struct*)
_tmp22E;if(_tmp241->tag != 0)goto _LL9E;else{_tmp242=_tmp241->f1;if(_tmp242 != 1)
goto _LL9E;_tmp243=_tmp241->f2;if(_tmp243 != 64)goto _LL9E;}}_LL9D: {long long*x0=((
long long*(*)(void*x))unsafe_cast)(x);long long*y0=((long long*(*)(void*x))
unsafe_cast)(y);struct _tuple11 _tmp4DC;return(_tmp4DC.f1=*x0 == *y0?Cyc_Marshal_Eq_ord:(*
x0 <= *y0?Cyc_Marshal_Lt_ord: Cyc_Marshal_Gt_ord),((_tmp4DC.f2=env,_tmp4DC)));}
_LL9E: {struct Cyc_Typerep_Int_struct*_tmp244=(struct Cyc_Typerep_Int_struct*)
_tmp22E;if(_tmp244->tag != 0)goto _LLA0;else{_tmp245=_tmp244->f1;if(_tmp245 != 0)
goto _LLA0;_tmp246=_tmp244->f2;if(_tmp246 != 64)goto _LLA0;}}_LL9F: {
unsigned long long*x0=((unsigned long long*(*)(void*x))unsafe_cast)(x);
unsigned long long*y0=((unsigned long long*(*)(void*x))unsafe_cast)(y);struct
_tuple11 _tmp4DD;return(_tmp4DD.f1=*x0 == *y0?Cyc_Marshal_Eq_ord:(*x0 <= *y0?Cyc_Marshal_Lt_ord:
Cyc_Marshal_Gt_ord),((_tmp4DD.f2=env,_tmp4DD)));}_LLA0: {struct Cyc_Typerep_Float_struct*
_tmp247=(struct Cyc_Typerep_Float_struct*)_tmp22E;if(_tmp247->tag != 1)goto _LLA2;}
_LLA1: {float*x0=((float*(*)(void*x))unsafe_cast)(x);float*y0=((float*(*)(void*x))
unsafe_cast)(y);struct _tuple11 _tmp4DE;return(_tmp4DE.f1=*x0 == *y0?Cyc_Marshal_Eq_ord:(*
x0 <= *y0?Cyc_Marshal_Lt_ord: Cyc_Marshal_Gt_ord),((_tmp4DE.f2=env,_tmp4DE)));}
_LLA2: {struct Cyc_Typerep_Double_struct*_tmp248=(struct Cyc_Typerep_Double_struct*)
_tmp22E;if(_tmp248->tag != 2)goto _LLA4;}_LLA3: {double*x0=((double*(*)(void*x))
unsafe_cast)(x);double*y0=((double*(*)(void*x))unsafe_cast)(y);struct _tuple11
_tmp4DF;return(_tmp4DF.f1=*x0 == *y0?Cyc_Marshal_Eq_ord:(*x0 <= *y0?Cyc_Marshal_Lt_ord:
Cyc_Marshal_Gt_ord),((_tmp4DF.f2=env,_tmp4DF)));}_LLA4: {struct Cyc_Typerep_ThinPtr_struct*
_tmp249=(struct Cyc_Typerep_ThinPtr_struct*)_tmp22E;if(_tmp249->tag != 3)goto _LLA6;
else{_tmp24A=_tmp249->f1;_tmp24B=(void*)_tmp249->f2;}}_LLA5: {unsigned int*x0=((
unsigned int*(*)(void*x))unsafe_cast)(x);unsigned int*y0=((unsigned int*(*)(void*
x))unsafe_cast)(y);unsigned int addrx=*x0;unsigned int addry=*y0;struct _tuple10*
_tmp4E0;struct _tuple10*_tmp268=(_tmp4E0=_cycalloc_atomic(sizeof(*_tmp4E0)),((
_tmp4E0->f1=addrx,((_tmp4E0->f2=addry,_tmp4E0)))));if(((int(*)(struct Cyc_Dict_Dict
d,struct _tuple10*k))Cyc_Dict_member)(env,_tmp268)){struct _tuple11 _tmp4E1;return(
_tmp4E1.f1=((enum Cyc_Marshal_Order(*)(struct Cyc_Dict_Dict d,struct _tuple10*k))
Cyc_Dict_lookup)(env,_tmp268),((_tmp4E1.f2=env,_tmp4E1)));}else{if(addrx == 0){
struct _tuple11 _tmp4E2;return(_tmp4E2.f1=Cyc_Marshal_Lt_ord,((_tmp4E2.f2=env,
_tmp4E2)));}else{if(addry == 0){struct _tuple11 _tmp4E3;return(_tmp4E3.f1=Cyc_Marshal_Gt_ord,((
_tmp4E3.f2=env,_tmp4E3)));}else{env=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict
d,struct _tuple10*k,enum Cyc_Marshal_Order v))Cyc_Dict_insert)(env,_tmp268,Cyc_Marshal_Eq_ord);{
int typ_szb=(int)Cyc_Typerep_size_type(_tmp24B);int xa=(int)addrx;int ya=(int)addry;
enum Cyc_Marshal_Order ord=Cyc_Marshal_Eq_ord;{int i=0;for(0;i < _tmp24A;++ i){enum 
Cyc_Marshal_Order _tmp26D;struct Cyc_Dict_Dict _tmp26E;struct _tuple11 _tmp26C=Cyc_Marshal_leq_eq_type(
_tmp24B,env,((void*(*)(int x))unsafe_cast)(xa),((void*(*)(int x))unsafe_cast)(ya));
_tmp26D=_tmp26C.f1;_tmp26E=_tmp26C.f2;ord=Cyc_Marshal_order_lex(ord,_tmp26D);env=
_tmp26E;xa +=typ_szb;ya +=typ_szb;}}{struct _tuple11 _tmp4E4;return Cyc_Marshal_return_update(
_tmp268,((_tmp4E4.f1=ord,((_tmp4E4.f2=env,_tmp4E4)))));}}}}}}_LLA6: {struct Cyc_Typerep_FatPtr_struct*
_tmp24C=(struct Cyc_Typerep_FatPtr_struct*)_tmp22E;if(_tmp24C->tag != 4)goto _LLA8;
else{_tmp24D=(void*)_tmp24C->f1;}}_LLA7: {struct _tuple9*x0=((struct _tuple9*(*)(
void*x))unsafe_cast)(x);struct _tuple9*y0=((struct _tuple9*(*)(void*x))unsafe_cast)(
y);unsigned int _tmp272;unsigned int _tmp273;unsigned int _tmp274;struct _tuple9
_tmp271=*x0;_tmp272=_tmp271.f1;_tmp273=_tmp271.f2;_tmp274=_tmp271.f3;{
unsigned int _tmp276;unsigned int _tmp277;unsigned int _tmp278;struct _tuple9 _tmp275=*
y0;_tmp276=_tmp275.f1;_tmp277=_tmp275.f2;_tmp278=_tmp275.f3;{int typ_szb=(int)Cyc_Typerep_size_type(
_tmp24D);int xpos=(int)((_tmp273 - _tmp272)/ typ_szb);int ypos=(int)((_tmp277 - 
_tmp276)/ typ_szb);if(xpos > ypos){struct _tuple11 _tmp4E5;return(_tmp4E5.f1=Cyc_Marshal_Gt_ord,((
_tmp4E5.f2=env,_tmp4E5)));}{int xsize=(int)((_tmp274 - _tmp272)/ typ_szb);int ysize=(
int)((_tmp278 - _tmp276)/ typ_szb);if(xsize > ysize){struct _tuple11 _tmp4E6;return(
_tmp4E6.f1=Cyc_Marshal_Gt_ord,((_tmp4E6.f2=env,_tmp4E6)));}{struct _tuple10*
_tmp4E7;struct _tuple10*_tmp27B=(_tmp4E7=_cycalloc_atomic(sizeof(*_tmp4E7)),((
_tmp4E7->f1=_tmp272,((_tmp4E7->f2=_tmp276,_tmp4E7)))));if(((int(*)(struct Cyc_Dict_Dict
d,struct _tuple10*k))Cyc_Dict_member)(env,_tmp27B)){struct _tuple11 _tmp4E8;return(
_tmp4E8.f1=((enum Cyc_Marshal_Order(*)(struct Cyc_Dict_Dict d,struct _tuple10*k))
Cyc_Dict_lookup)(env,_tmp27B),((_tmp4E8.f2=env,_tmp4E8)));}else{if(_tmp272 == 0){
struct _tuple11 _tmp4E9;return(_tmp4E9.f1=Cyc_Marshal_Lt_ord,((_tmp4E9.f2=env,
_tmp4E9)));}else{if(_tmp276 == 0){struct _tuple11 _tmp4EA;return(_tmp4EA.f1=Cyc_Marshal_Gt_ord,((
_tmp4EA.f2=env,_tmp4EA)));}else{env=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict
d,struct _tuple10*k,enum Cyc_Marshal_Order v))Cyc_Dict_insert)(env,_tmp27B,Cyc_Marshal_Eq_ord);{
int xb=(int)_tmp272;int yb=(int)_tmp276;enum Cyc_Marshal_Order ord=Cyc_Marshal_Eq_ord;{
int i=0;for(0;i < xsize;++ i){enum Cyc_Marshal_Order _tmp280;struct Cyc_Dict_Dict
_tmp281;struct _tuple11 _tmp27F=Cyc_Marshal_leq_eq_type(_tmp24D,env,((void*(*)(int
x))unsafe_cast)(xb),((void*(*)(int x))unsafe_cast)(yb));_tmp280=_tmp27F.f1;
_tmp281=_tmp27F.f2;ord=Cyc_Marshal_order_lex(ord,_tmp280);env=_tmp281;xb +=
typ_szb;yb +=typ_szb;}}if(ord == Cyc_Marshal_Eq_ord  && xsize < ysize)ord=Cyc_Marshal_Lt_ord;{
struct _tuple11 _tmp4EB;return Cyc_Marshal_return_update(_tmp27B,((_tmp4EB.f1=ord,((
_tmp4EB.f2=env,_tmp4EB)))));}}}}}}}}}}_LLA8: {struct Cyc_Typerep_Tuple_struct*
_tmp24E=(struct Cyc_Typerep_Tuple_struct*)_tmp22E;if(_tmp24E->tag != 6)goto _LLAA;
else{_tmp24F=_tmp24E->f2;}}_LLA9: {int x0=((int(*)(void*x))unsafe_cast)(x);int y0=((
int(*)(void*x))unsafe_cast)(y);enum Cyc_Marshal_Order ord=Cyc_Marshal_Eq_ord;{int
i=0;for(0;i < _get_dyneither_size(_tmp24F,sizeof(struct _tuple1*));++ i){
unsigned int _tmp285;void*_tmp286;struct _tuple1 _tmp284=*((struct _tuple1**)_tmp24F.curr)[
i];_tmp285=_tmp284.f1;_tmp286=_tmp284.f2;{int x_ofs=(int)(x0 + _tmp285);int y_ofs=(
int)(y0 + _tmp285);enum Cyc_Marshal_Order _tmp288;struct Cyc_Dict_Dict _tmp289;
struct _tuple11 _tmp287=Cyc_Marshal_leq_eq_type(_tmp286,env,((void*(*)(int x))
unsafe_cast)(x_ofs),((void*(*)(int x))unsafe_cast)(y_ofs));_tmp288=_tmp287.f1;
_tmp289=_tmp287.f2;ord=Cyc_Marshal_order_lex(ord,_tmp288);env=_tmp289;}}}{struct
_tuple11 _tmp4EC;return(_tmp4EC.f1=ord,((_tmp4EC.f2=env,_tmp4EC)));}}_LLAA: {
struct Cyc_Typerep_Struct_struct*_tmp250=(struct Cyc_Typerep_Struct_struct*)
_tmp22E;if(_tmp250->tag != 5)goto _LLAC;else{_tmp251=_tmp250->f3;}}_LLAB: {int x0=((
int(*)(void*x))unsafe_cast)(x);int y0=((int(*)(void*x))unsafe_cast)(y);enum Cyc_Marshal_Order
ord=Cyc_Marshal_Eq_ord;{int i=0;for(0;i < _get_dyneither_size(_tmp251,sizeof(
struct _tuple0*));++ i){unsigned int _tmp28C;void*_tmp28D;struct _tuple0 _tmp28B=*((
struct _tuple0**)_tmp251.curr)[i];_tmp28C=_tmp28B.f1;_tmp28D=_tmp28B.f3;{int x_ofs=(
int)(x0 + _tmp28C);int y_ofs=(int)(y0 + _tmp28C);enum Cyc_Marshal_Order _tmp28F;
struct Cyc_Dict_Dict _tmp290;struct _tuple11 _tmp28E=Cyc_Marshal_leq_eq_type(_tmp28D,
env,((void*(*)(int x))unsafe_cast)(x_ofs),((void*(*)(int x))unsafe_cast)(y_ofs));
_tmp28F=_tmp28E.f1;_tmp290=_tmp28E.f2;ord=Cyc_Marshal_order_lex(ord,_tmp28F);env=
_tmp290;}}}{struct _tuple11 _tmp4ED;return(_tmp4ED.f1=ord,((_tmp4ED.f2=env,_tmp4ED)));}}
_LLAC: {struct Cyc_Typerep_TUnionField_struct*_tmp252=(struct Cyc_Typerep_TUnionField_struct*)
_tmp22E;if(_tmp252->tag != 8)goto _LLAE;else{_tmp253=_tmp252->f4;}}_LLAD: {int x0=((
int(*)(void*x))unsafe_cast)(x);int y0=((int(*)(void*x))unsafe_cast)(y);enum Cyc_Marshal_Order
ord=Cyc_Marshal_Eq_ord;{int i=1;for(0;i < _get_dyneither_size(_tmp253,sizeof(
struct _tuple1*));++ i){unsigned int _tmp293;void*_tmp294;struct _tuple1 _tmp292=*((
struct _tuple1**)_tmp253.curr)[i];_tmp293=_tmp292.f1;_tmp294=_tmp292.f2;{int x_ofs=(
int)(x0 + _tmp293);int y_ofs=(int)(y0 + _tmp293);enum Cyc_Marshal_Order _tmp296;
struct Cyc_Dict_Dict _tmp297;struct _tuple11 _tmp295=Cyc_Marshal_leq_eq_type(_tmp294,
env,((void*(*)(int x))unsafe_cast)(x_ofs),((void*(*)(int x))unsafe_cast)(y_ofs));
_tmp296=_tmp295.f1;_tmp297=_tmp295.f2;ord=Cyc_Marshal_order_lex(ord,_tmp296);env=
_tmp297;}}}{struct _tuple11 _tmp4EE;return(_tmp4EE.f1=ord,((_tmp4EE.f2=env,_tmp4EE)));}}
_LLAE: {struct Cyc_Typerep_TUnion_struct*_tmp254=(struct Cyc_Typerep_TUnion_struct*)
_tmp22E;if(_tmp254->tag != 7)goto _LLB0;else{_tmp255=_tmp254->f2;_tmp256=_tmp254->f3;}}
_LLAF: {unsigned int*x0=((unsigned int*(*)(void*x))unsafe_cast)(x);unsigned int*
y0=((unsigned int*(*)(void*x))unsafe_cast)(y);if(*x0 < 1024  && *y0 < 1024){struct
_tuple11 _tmp4EF;return(_tmp4EF.f1=*x0 == *y0?Cyc_Marshal_Eq_ord:(*x0 <= *y0?Cyc_Marshal_Lt_ord:
Cyc_Marshal_Gt_ord),((_tmp4EF.f2=env,_tmp4EF)));}else{if(*x0 < 1024){struct
_tuple11 _tmp4F0;return(_tmp4F0.f1=Cyc_Marshal_Lt_ord,((_tmp4F0.f2=env,_tmp4F0)));}
else{if(*y0 < 1024){struct _tuple11 _tmp4F1;return(_tmp4F1.f1=Cyc_Marshal_Gt_ord,((
_tmp4F1.f2=env,_tmp4F1)));}else{struct _tuple10*_tmp4F2;struct _tuple10*_tmp29C=(
_tmp4F2=_cycalloc_atomic(sizeof(*_tmp4F2)),((_tmp4F2->f1=*x0,((_tmp4F2->f2=*y0,
_tmp4F2)))));if(((int(*)(struct Cyc_Dict_Dict d,struct _tuple10*k))Cyc_Dict_member)(
env,_tmp29C)){struct _tuple11 _tmp4F3;return(_tmp4F3.f1=((enum Cyc_Marshal_Order(*)(
struct Cyc_Dict_Dict d,struct _tuple10*k))Cyc_Dict_lookup)(env,_tmp29C),((_tmp4F3.f2=
env,_tmp4F3)));}else{env=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct
_tuple10*k,enum Cyc_Marshal_Order v))Cyc_Dict_insert)(env,_tmp29C,Cyc_Marshal_Eq_ord);{
unsigned int*xtagp=((unsigned int*(*)(unsigned int x))unsafe_cast)(*x0);
unsigned int*ytagp=((unsigned int*(*)(unsigned int x))unsafe_cast)(*y0);if(*xtagp
!= *ytagp){struct _tuple11 _tmp4F4;return Cyc_Marshal_return_update(_tmp29C,((
_tmp4F4.f1=*xtagp == *ytagp?Cyc_Marshal_Eq_ord:(*xtagp <= *ytagp?Cyc_Marshal_Lt_ord:
Cyc_Marshal_Gt_ord),((_tmp4F4.f2=env,_tmp4F4)))));}else{int _tmp2A0;void*_tmp2A1;
struct _tuple4 _tmp29F=Cyc_Typerep_get_unionbranch(*xtagp,_tmp256);_tmp2A0=_tmp29F.f1;
_tmp2A1=_tmp29F.f2;{void*xoff=((void*(*)(unsigned int x))unsafe_cast)(*x0 + 
_tmp2A0);void*yoff=((void*(*)(unsigned int x))unsafe_cast)(*y0 + _tmp2A0);return
Cyc_Marshal_return_update(_tmp29C,Cyc_Marshal_leq_eq_type(_tmp2A1,env,xoff,yoff));}}}}}}}}
_LLB0: {struct Cyc_Typerep_XTUnion_struct*_tmp257=(struct Cyc_Typerep_XTUnion_struct*)
_tmp22E;if(_tmp257->tag != 9)goto _LLB2;else{_tmp258=_tmp257->f2;}}_LLB1: {
unsigned int*x0=((unsigned int*(*)(void*x))unsafe_cast)(x);unsigned int*y0=((
unsigned int*(*)(void*x))unsafe_cast)(y);struct _tuple10*_tmp4F5;struct _tuple10*
_tmp2A3=(_tmp4F5=_cycalloc_atomic(sizeof(*_tmp4F5)),((_tmp4F5->f1=*x0,((_tmp4F5->f2=*
y0,_tmp4F5)))));if(((int(*)(struct Cyc_Dict_Dict d,struct _tuple10*k))Cyc_Dict_member)(
env,_tmp2A3)){struct _tuple11 _tmp4F6;return(_tmp4F6.f1=((enum Cyc_Marshal_Order(*)(
struct Cyc_Dict_Dict d,struct _tuple10*k))Cyc_Dict_lookup)(env,_tmp2A3),((_tmp4F6.f2=
env,_tmp4F6)));}else{{struct _tuple10*_tmp4F7;env=((struct Cyc_Dict_Dict(*)(struct
Cyc_Dict_Dict d,struct _tuple10*k,enum Cyc_Marshal_Order v))Cyc_Dict_insert)(env,((
_tmp4F7=_cycalloc_atomic(sizeof(*_tmp4F7)),((_tmp4F7->f1=*x0,((_tmp4F7->f2=*y0,
_tmp4F7)))))),Cyc_Marshal_Eq_ord);}{unsigned int*xp=((unsigned int*(*)(
unsigned int x))unsafe_cast)(*x0);unsigned int*yp=((unsigned int*(*)(unsigned int
x))unsafe_cast)(*y0);if(*xp == 0  && *yp == 0){char*xnamec=((char*(*)(unsigned int x))
unsafe_cast)(*x0 + 4);char*ynamec=((char*(*)(unsigned int x))unsafe_cast)(*y0 + 4);
struct _dyneither_ptr xname=(struct _dyneither_ptr)Cstring_to_string(xnamec);struct
_dyneither_ptr yname=(struct _dyneither_ptr)Cstring_to_string(ynamec);int cmp=Cyc_strcmp((
struct _dyneither_ptr)xname,(struct _dyneither_ptr)yname);if(cmp < 0){struct
_tuple11 _tmp4F8;return Cyc_Marshal_return_update(_tmp2A3,((_tmp4F8.f1=Cyc_Marshal_Gt_ord,((
_tmp4F8.f2=env,_tmp4F8)))));}else{if(cmp == 0){struct _tuple11 _tmp4F9;return Cyc_Marshal_return_update(
_tmp2A3,((_tmp4F9.f1=Cyc_Marshal_Eq_ord,((_tmp4F9.f2=env,_tmp4F9)))));}else{
struct _tuple11 _tmp4FA;return Cyc_Marshal_return_update(_tmp2A3,((_tmp4FA.f1=Cyc_Marshal_Lt_ord,((
_tmp4FA.f2=env,_tmp4FA)))));}}}else{if(*xp == 0){struct _tuple11 _tmp4FB;return Cyc_Marshal_return_update(
_tmp2A3,((_tmp4FB.f1=Cyc_Marshal_Lt_ord,((_tmp4FB.f2=env,_tmp4FB)))));}else{if(*
yp == 0){struct _tuple11 _tmp4FC;return Cyc_Marshal_return_update(_tmp2A3,((_tmp4FC.f1=
Cyc_Marshal_Gt_ord,((_tmp4FC.f2=env,_tmp4FC)))));}else{char*xnamec=((char*(*)(
unsigned int x))unsafe_cast)(*xp + 4);char*ynamec=((char*(*)(unsigned int x))
unsafe_cast)(*yp + 4);struct _dyneither_ptr xname=(struct _dyneither_ptr)
Cstring_to_string(xnamec);struct _dyneither_ptr yname=(struct _dyneither_ptr)
Cstring_to_string(ynamec);int cmp=Cyc_strcmp((struct _dyneither_ptr)xname,(struct
_dyneither_ptr)yname);if(cmp < 0){struct _tuple11 _tmp4FD;return Cyc_Marshal_return_update(
_tmp2A3,((_tmp4FD.f1=Cyc_Marshal_Gt_ord,((_tmp4FD.f2=env,_tmp4FD)))));}else{if(
cmp == 0){int _tmp2AD;void*_tmp2AE;struct _tuple4 _tmp2AC=Cyc_Typerep_get_xtunionbranch(
xname,_tmp258);_tmp2AD=_tmp2AC.f1;_tmp2AE=_tmp2AC.f2;{void*xoff=((void*(*)(
unsigned int x))unsafe_cast)(*x0 + _tmp2AD);void*yoff=((void*(*)(unsigned int x))
unsafe_cast)(*y0 + _tmp2AD);return Cyc_Marshal_return_update(_tmp2A3,Cyc_Marshal_leq_eq_type(
_tmp2AE,env,xoff,yoff));}}else{struct _tuple11 _tmp4FE;return Cyc_Marshal_return_update(
_tmp2A3,((_tmp4FE.f1=Cyc_Marshal_Lt_ord,((_tmp4FE.f2=env,_tmp4FE)))));}}}}}}}}
_LLB2: {struct Cyc_Typerep_Union_struct*_tmp259=(struct Cyc_Typerep_Union_struct*)
_tmp22E;if(_tmp259->tag != 10)goto _LLB4;else{_tmp25A=_tmp259->f2;_tmp25B=_tmp259->f3;}}
_LLB3: {struct Cyc_Typerep_Tuple_struct _tmp517;unsigned int _tmp516;unsigned int
_tmp515;struct _dyneither_ptr _tmp514;struct _tuple1**_tmp513;unsigned int _tmp512;
struct Cyc_Typerep_Tuple_struct*_tmp511;return Cyc_Marshal_leq_eq_type((void*)((
_tmp511=_cycalloc(sizeof(*_tmp511)),((_tmp511[0]=((_tmp517.tag=6,((_tmp517.f1=(
unsigned int)_tmp25A,((_tmp517.f2=((_tmp512=(unsigned int)_tmp25A,((_tmp513=(
struct _tuple1**)_cycalloc(_check_times(sizeof(struct _tuple1*),_tmp512)),((
_tmp514=_tag_dyneither(_tmp513,sizeof(struct _tuple1*),_tmp512),((((_tmp515=
_tmp512,_tmp50A(& _tmp516,& _tmp515,& _tmp513))),_tmp514)))))))),_tmp517)))))),
_tmp511)))),env,x,y);}_LLB4: {struct Cyc_Typerep_Enum_struct*_tmp25C=(struct Cyc_Typerep_Enum_struct*)
_tmp22E;if(_tmp25C->tag != 11)goto _LLB6;else{_tmp25D=_tmp25C->f2;}}_LLB5: {struct
Cyc_Typerep_Int_struct _tmp51A;struct Cyc_Typerep_Int_struct*_tmp519;return Cyc_Marshal_leq_eq_type((
void*)((_tmp519=_cycalloc_atomic(sizeof(*_tmp519)),((_tmp519[0]=((_tmp51A.tag=0,((
_tmp51A.f1=0,((_tmp51A.f2=(unsigned int)(_tmp25D << 3),_tmp51A)))))),_tmp519)))),
env,x,y);}_LLB6:;_LLB7: {struct Cyc_Core_Failure_struct _tmp520;const char*_tmp51F;
struct Cyc_Core_Failure_struct*_tmp51E;(int)_throw((void*)((_tmp51E=_cycalloc(
sizeof(*_tmp51E)),((_tmp51E[0]=((_tmp520.tag=Cyc_Core_Failure,((_tmp520.f1=((
_tmp51F="leq_eq_type case unmatched",_tag_dyneither(_tmp51F,sizeof(char),27))),
_tmp520)))),_tmp51E)))));}_LL8F:;}}int Cyc_Marshal_eq_type(void*ts,void*x,void*y);
int Cyc_Marshal_eq_type(void*ts,void*x,void*y){struct _handler_cons _tmp2BF;
_push_handler(& _tmp2BF);{int _tmp2C1=0;if(setjmp(_tmp2BF.handler))_tmp2C1=1;if(!
_tmp2C1){{enum Cyc_Marshal_Order _tmp2C3;struct _tuple11 _tmp2C2=Cyc_Marshal_leq_eq_type(
ts,Cyc_Marshal_empty_eq_dict(),x,y);_tmp2C3=_tmp2C2.f1;{int _tmp2C4=_tmp2C3 == Cyc_Marshal_Eq_ord;
_npop_handler(0);return _tmp2C4;}};_pop_handler();}else{void*_tmp2C0=(void*)
_exn_thrown;void*_tmp2C6=_tmp2C0;struct _dyneither_ptr _tmp2C8;_LLB9: {struct Cyc_Core_Failure_struct*
_tmp2C7=(struct Cyc_Core_Failure_struct*)_tmp2C6;if(_tmp2C7->tag != Cyc_Core_Failure)
goto _LLBB;else{_tmp2C8=_tmp2C7->f1;}}_LLBA:{const char*_tmp524;void*_tmp523[1];
struct Cyc_String_pa_struct _tmp522;(_tmp522.tag=0,((_tmp522.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)_tmp2C8),((_tmp523[0]=& _tmp522,Cyc_fprintf(
Cyc_stderr,((_tmp524="Core::Failure(%s)\n",_tag_dyneither(_tmp524,sizeof(char),
19))),_tag_dyneither(_tmp523,sizeof(void*),1)))))));}{struct Cyc_Core_Failure_struct
_tmp527;struct Cyc_Core_Failure_struct*_tmp526;(int)_throw((void*)((_tmp526=
_cycalloc(sizeof(*_tmp526)),((_tmp526[0]=((_tmp527.tag=Cyc_Core_Failure,((
_tmp527.f1=_tmp2C8,_tmp527)))),_tmp526)))));}_LLBB:;_LLBC:(void)_throw(_tmp2C6);
_LLB8:;}}}int Cyc_Marshal_leq_type(void*ts,void*x,void*y);int Cyc_Marshal_leq_type(
void*ts,void*x,void*y){struct _handler_cons _tmp2CE;_push_handler(& _tmp2CE);{int
_tmp2D0=0;if(setjmp(_tmp2CE.handler))_tmp2D0=1;if(!_tmp2D0){{enum Cyc_Marshal_Order
_tmp2D2;struct _tuple11 _tmp2D1=Cyc_Marshal_leq_eq_type(ts,Cyc_Marshal_empty_eq_dict(),
x,y);_tmp2D2=_tmp2D1.f1;{int _tmp2D3=_tmp2D2 != Cyc_Marshal_Gt_ord;_npop_handler(0);
return _tmp2D3;}};_pop_handler();}else{void*_tmp2CF=(void*)_exn_thrown;void*
_tmp2D5=_tmp2CF;struct _dyneither_ptr _tmp2D7;_LLBE: {struct Cyc_Core_Failure_struct*
_tmp2D6=(struct Cyc_Core_Failure_struct*)_tmp2D5;if(_tmp2D6->tag != Cyc_Core_Failure)
goto _LLC0;else{_tmp2D7=_tmp2D6->f1;}}_LLBF:{const char*_tmp52B;void*_tmp52A[1];
struct Cyc_String_pa_struct _tmp529;(_tmp529.tag=0,((_tmp529.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)_tmp2D7),((_tmp52A[0]=& _tmp529,Cyc_fprintf(
Cyc_stderr,((_tmp52B="Core::Failure(%s)\n",_tag_dyneither(_tmp52B,sizeof(char),
19))),_tag_dyneither(_tmp52A,sizeof(void*),1)))))));}{struct Cyc_Core_Failure_struct
_tmp52E;struct Cyc_Core_Failure_struct*_tmp52D;(int)_throw((void*)((_tmp52D=
_cycalloc(sizeof(*_tmp52D)),((_tmp52D[0]=((_tmp52E.tag=Cyc_Core_Failure,((
_tmp52E.f1=_tmp2D7,_tmp52E)))),_tmp52D)))));}_LLC0:;_LLC1:(void)_throw(_tmp2D5);
_LLBD:;}}}int Cyc_Marshal_cmp_type(void*ts,void*x,void*y);int Cyc_Marshal_cmp_type(
void*ts,void*x,void*y){struct _handler_cons _tmp2DD;_push_handler(& _tmp2DD);{int
_tmp2DF=0;if(setjmp(_tmp2DD.handler))_tmp2DF=1;if(!_tmp2DF){{enum Cyc_Marshal_Order
_tmp2E1;struct _tuple11 _tmp2E0=Cyc_Marshal_leq_eq_type(ts,Cyc_Marshal_empty_eq_dict(),
x,y);_tmp2E1=_tmp2E0.f1;switch(_tmp2E1){case Cyc_Marshal_Gt_ord: _LLC2: {int
_tmp2E2=- 1;_npop_handler(0);return _tmp2E2;}case Cyc_Marshal_Eq_ord: _LLC3: {int
_tmp2E3=0;_npop_handler(0);return _tmp2E3;}case Cyc_Marshal_Lt_ord: _LLC4: {int
_tmp2E4=1;_npop_handler(0);return _tmp2E4;}}};_pop_handler();}else{void*_tmp2DE=(
void*)_exn_thrown;void*_tmp2E6=_tmp2DE;struct _dyneither_ptr _tmp2E8;_LLC7: {
struct Cyc_Core_Failure_struct*_tmp2E7=(struct Cyc_Core_Failure_struct*)_tmp2E6;
if(_tmp2E7->tag != Cyc_Core_Failure)goto _LLC9;else{_tmp2E8=_tmp2E7->f1;}}_LLC8:{
const char*_tmp532;void*_tmp531[1];struct Cyc_String_pa_struct _tmp530;(_tmp530.tag=
0,((_tmp530.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp2E8),((_tmp531[0]=&
_tmp530,Cyc_fprintf(Cyc_stderr,((_tmp532="Core::Failure(%s)\n",_tag_dyneither(
_tmp532,sizeof(char),19))),_tag_dyneither(_tmp531,sizeof(void*),1)))))));}{
struct Cyc_Core_Failure_struct _tmp535;struct Cyc_Core_Failure_struct*_tmp534;(int)
_throw((void*)((_tmp534=_cycalloc(sizeof(*_tmp534)),((_tmp534[0]=((_tmp535.tag=
Cyc_Core_Failure,((_tmp535.f1=_tmp2E8,_tmp535)))),_tmp534)))));}_LLC9:;_LLCA:(
void)_throw(_tmp2E6);_LLC6:;}}}
